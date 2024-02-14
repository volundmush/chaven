
#if defined (_WIN32)
#if defined (_DEBUG)
#pragma warning (disable : 4786)
#pragma warning (disable : 4800)
#endif
#endif
#include "Note.h"

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include <vector>
#include <map>
#include <string>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(macintosh)
  extern  int     _filbuf         args( (FILE *) );
#endif


  void save_pc_color args((CHAR_DATA * ch, FILE *fp));
  void fwrite_pc_creation args((CHAR_DATA * ch, FILE *fp));
  void mod_agility args((CHAR_DATA * ch));
  /* int rename(const char *oldfname, const char *newfname); viene en stdio.h */

  char *print_flags(int flag) {
    int count, pos = 0;
    static char buf[52];

    for (count = 0; count < 32; count++) {
      if (IS_SET(flag, 1 << count)) {
        if (count < 26)
        buf[pos] = 'A' + count;
        else
        buf[pos] = 'a' + (count - 26);
        pos++;
      }
    }

    if (pos == 0) {
      buf[pos] = '0';
      pos++;
    }

    buf[pos] = '\0';

    return buf;
  }

  /*
* Array of containers read for proper re-nesting of objects.
*/
#define MAX_NEST 100
  static OBJ_DATA *rgObjNest[MAX_NEST];

  /*
* Local functions.
*/
  void fwrite_char args((CHAR_DATA * ch, FILE *fp, bool, bool Forsaken,
  CHAR_DATA *vch));
  void fwrite_obj args((OBJ_DATA * obj, FILE *fp, int iNest));
  void fread_char args((CHAR_DATA * ch, FILE *fp));

  /*
* Save a character and inventory.
* Would be cool to save NPC's too for quest purposes,
*   some of the infrastructure is provided.
*/
  void save_char_obj(CHAR_DATA *ch, bool crash, bool Forsaken) {
    char strsave[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA d;
    DESCRIPTOR_DATA *dold = NULL;
    CHAR_DATA *vch = ch;
    FILE *fp = NULL;

    if (IS_NPC(ch))
    return;

    if (IS_FLAG(ch->act, PLR_NOSAVE))
    return;

    if (!IS_FLAG(ch->act, PLR_SINSPIRIT)) {
      if (ch->pcdata->account != NULL)
      save_account(ch->pcdata->account, FALSE);
    }
    if (is_name("NewCharacter", ch->name))
    return;

    /*
if ( ch->desc != NULL && ch->desc->original != NULL )
ch = ch->desc->original;
*/
    d.character = NULL;

#if defined(unix)
    /* create god log */
    if (IS_IMMORTAL(ch) && ch->level != 150) {
      sprintf(strsave, "%s%s", GOD_DIR, capitalize(ch->name));

      if ((fp = fopen(strsave, "w")) == NULL) {
        bugf("[%s::%s] Could not open file: %s", __FILE__, __FUNCTION__, strsave);
      }
      else {
        fprintf(fp, "Lev %2d Trust %2d Security %d Pass %s\n", ch->level,
        get_trust(ch), ch->pcdata->security, ch->pcdata->pwd);
        fclose(fp);
      }
    }
#endif

    // level 150 saves in remortt dir
    if (ch->level == 150) {
      sprintf(strsave, "%s%s", REMORT_DIR, capitalize(ch->name));
      ch->level = 101;
    }
    else if (ch->pcdata->confirm_delete)
    sprintf(strsave, "%s%s", DELETE_DIR, capitalize(ch->name));
    else if (1 == 0 && Forsaken) {
      d.character->desc = NULL;
      char_list.push_front(d.character);
      d.connected = CON_PLAYING;
      char_to_room(d.character, ch->in_room);
      vch = d.character;
      bug("Char list push1", 0);
      if (vch && vch->name)
      bug(vch->name, 0);
    }
    else
    sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(ch->name));

    if ((fp = fopen(TEMP_FILE, "w")) == NULL) {
      bug("Save_char_obj: fopen", 0);
      perror(strsave);
    }
    else {
      fwrite_char(ch, fp, crash, Forsaken, d.character);
      if (!Forsaken)
      if (ch->carrying != NULL)
      fwrite_obj(ch->carrying, fp, 0);
      if (Forsaken)
      if (vch->carrying != NULL)
      fwrite_obj(vch->carrying, fp, 0);
      fprintf(fp, "#END\n");
    }
    if (fp == NULL)
    return;
    fclose(fp);
    rename(TEMP_FILE, strsave);
    if (time_info.day % 7 == 0)
    sprintf(strsave, "%s%s", BACK1_DIR, capitalize(ch->name));
    else if (time_info.day % 6 == 0)
    sprintf(strsave, "%s%s", BACK2_DIR, capitalize(ch->name));
    else if (time_info.day % 5 == 0)
    sprintf(strsave, "%s%s", BACK3_DIR, capitalize(ch->name));
    else if (time_info.day % 4 == 0)
    sprintf(strsave, "%s%s", BACK4_DIR, capitalize(ch->name));
    else if (time_info.day % 3 == 0)
    sprintf(strsave, "%s%s", BACK5_DIR, capitalize(ch->name));
    else if (time_info.day % 2 == 0)
    sprintf(strsave, "%s%s", BACK6_DIR, capitalize(ch->name));
    else
    sprintf(strsave, "%s%s", BACK7_DIR, capitalize(ch->name));

    if (ch->played / 3600 > 5) {
      if ((fp = fopen(TEMP_FILE, "w")) == NULL) {
        bug("Save_char_obj: fopen", 0);
        perror(strsave);
      }
      else {
        fwrite_char(ch, fp, crash, Forsaken, d.character);
        if (!Forsaken)
        if (ch->carrying != NULL)
        fwrite_obj(ch->carrying, fp, 0);
        if (Forsaken)
        if (vch->carrying != NULL)
        fwrite_obj(vch->carrying, fp, 0);

        fprintf(fp, "#END\n");
      }

      fclose(fp);
      rename(TEMP_FILE, strsave);
    }

    if (vch != ch) {
      dold = vch->desc;
      extract_char(vch, TRUE);
      if (dold != NULL)
      dold->connected = CON_QUITTING;
      //         close_desc( dold );
    }

    return;
  }

  /*
* Write the char.
*/
  void fwrite_char(CHAR_DATA *ch, FILE *fp, bool crash, bool Forsaken,
  CHAR_DATA *vch) {
    AFFECT_DATA *paf;
    CHAR_DATA *tch;
    int pos, x = 0;
    fprintf(fp, "#%s\n", IS_NPC(ch) ? "MOB" : "PLAYER");

    if (ch->level == 0) {
      fwrite_pc_creation(ch, fp);
      return;
    }

    fprintf(fp, "Name %s~\n", ch->name);

    /*
* This check is for Forsaken, it makes it so the descs are not
* overwritten - Palin
*/
    if (!Forsaken)
    tch = ch;
    else
    tch = vch;

    fprintf(fp, "Id   %ld\n", tch->id);
    fprintf(fp, "LogO %ld\n", current_time);
    fprintf(fp, "Vers %d\n", CURR_VERSION);
    fprintf(fp, "Sex  %d\n", ch->sex);
    fprintf(fp, "Levl %d\n", ch->level);
    fprintf(fp, "BalMoney %ld\n", ch->pcdata->total_money);
    fprintf(fp, "BalCredit %d\n", ch->pcdata->total_credit);
    fprintf(fp, "Money %ld\n", ch->money);

    fprintf(fp, "LifeForce %d\n", ch->lifeforce);
    fprintf(fp, "LifeForceCooldown %d\n", ch->pcdata->lifeforcecooldown);
    fprintf(fp, "LFCount %d\n", ch->pcdata->lfcount);
    fprintf(fp, "LFTotal %d\n", ch->pcdata->lftotal);
    fprintf(fp, "LFMod %d\n", ch->pcdata->lf_modifier);
    fprintf(fp, "LFUsed %d\n", ch->lf_used);
    fprintf(fp, "LFTaken %d\n", ch->lf_taken);
    fprintf(fp, "LFSUsed %d\n", ch->lf_sused);

    if (ch->pcdata->home != 0)
    fprintf(fp, "Hom  %d\n", ch->pcdata->home);

    fprintf(fp, "Plyd %d\n", ch->played + (int)(current_time - ch->logon));
    fprintf(fp, "PlydCopFree %d\n",
    ch->playedcopfree + (int)(current_time - ch->logon));

    if (ch->lines != PAGELEN)
    fprintf(fp, "Scro %d\n", ch->lines);
    if (ch->linewidth != WORD_WRAP)
    fprintf(fp, "ScrWidth %d\n", ch->linewidth);

    if (ch != NULL && ch->pcdata->account != NULL)
    fprintf(fp, "Account %s~\n", ch->pcdata->account->name);

    fprintf(fp, "AccountName %s~\n", ch->pcdata->account_name);
    fprintf(fp, "Email %s~\n", ch->pcdata->email);

    if (ch->pcdata->penalty != 0)
    fprintf(fp, "Pnlty %d\n", ch->pcdata->penalty);

    fprintf(fp, "Jtlag %d\n", ch->pcdata->jetlag);

    fprintf(fp, "Race %s~\n", race_table[tch->race].name);
    fprintf(fp, "Modifier %d\n", ch->modifier);
    fprintf(fp, "RacialPower %d\n", ch->pcdata->racial_power);
    if (ch->pcdata->racial_power_two > 0)
    fprintf(fp, "RacialPowerTwo %d\n", ch->pcdata->racial_power_two);

    fprintf(fp, "Intro %s~\n", ch->pcdata->intro_desc);

    if (ch->pcdata->pwd[0] != '\0')
    fprintf(fp, "Pass %s~\n", ch->pcdata->pwd);
    if (ch->pcdata->upwd[0] != '\0')
    fprintf(fp, "UPass %s~\n", ch->pcdata->upwd);
    // Eliminating wild and bushy - Discordance
    if (ch->pcdata->pubic_hair[0] != '\0') {
      if (str_cmp(ch->pcdata->pubic_hair, "wild and bushy"))
      fprintf(fp, "PHair %s~\n", ch->pcdata->pubic_hair);
    }
    fprintf(fp, "Place %s~\n", ch->pcdata->place);
    if (ch->pcdata->bad_abomination != 0)
    fprintf(fp, "BadAbomination %d\n", ch->pcdata->bad_abomination);
    if (ch->pcdata->lucky_abomination != 0)
    fprintf(fp, "LuckyAbomination %d\n", ch->pcdata->lucky_abomination);
    if(higher_power(ch))
    fprintf(fp, "EidilonAmbient %s~\n", ch->pcdata->eidilon_ambient);
    if(ch->pcdata->last_victim_bled != 0)
    fprintf(fp, "LastVictimBled %d\n", ch->pcdata->last_victim_bled);
    if(ch->pcdata->last_imbue != 0)
    fprintf(fp, "LastImbue %d\n", ch->pcdata->last_imbue);


    if(ch->pcdata->eidilon_of != 0)
    fprintf(fp, "EidilonOf %d\n", ch->pcdata->eidilon_of);
    if (ch->pcdata->dtrains != 0)
    fprintf(fp, "Dtrains %d\n", ch->pcdata->dtrains);
    if (ch->pcdata->strains != 0)
    fprintf(fp, "Strains %d\n", ch->pcdata->strains);
    //    if (ch->pcdata->aexp != 0)
    fprintf(fp, "Aexp %d\n", ch->pcdata->aexp);
    //    if (ch->pcdata->wexp != 0)
    fprintf(fp, "Wexp %d\n", ch->pcdata->wexp);
    //    if( ch->pcdata->dexp != 0)
    fprintf(fp, "Dexp %d\n", ch->pcdata->dexp);
    fprintf(fp, "Oexp %d\n", ch->pcdata->oexp);
    fprintf(fp, "Hexp %d\n", ch->pcdata->hexp);
    fprintf(fp, "Gexp %d\n", ch->pcdata->gexp);
    if (ch->pcdata->awins != 0)
    fprintf(fp, "Awins %d\n", ch->pcdata->awins);
    if (ch->pcdata->alosses != 0)
    fprintf(fp, "Alosses %d\n", ch->pcdata->alosses);
    if (ch->pcdata->astatus != 0)
    fprintf(fp, "Astatus %d\n", ch->pcdata->astatus);

    if (ch->pcdata->selfbondage != 0)
    fprintf(fp, "SelfBondage %d\n", ch->pcdata->selfbondage);

    if (ch->pcdata->neutralized != 0)
    fprintf(fp, "Neutralized %d\n", ch->pcdata->neutralized);

    if (ch->pcdata->healthtimer != 0)
    fprintf(fp, "Healthtimer %d\n", ch->pcdata->healthtimer);
    if (ch->pcdata->hangoutone != 0) {
      fprintf(fp, "Hangouts %d %d %d\n", ch->pcdata->hangouttemp,
      ch->pcdata->hangoutone, ch->pcdata->hangouttwo);
    }
    if(safe_strlen(ch->pcdata->implant_dream) > 0)
    fprintf(fp, "ImplantDream %s~\n", ch->pcdata->implant_dream);
    if(safe_strlen(ch->pcdata->ff_knowledge) > 0)
    fprintf(fp, "FFKnowledge %s~\n", ch->pcdata->ff_knowledge);
    if(safe_strlen(ch->pcdata->ff_secret) > 0)
    fprintf(fp, "FFSecret %s~\n", ch->pcdata->ff_secret);

    if (safe_strlen(ch->pcdata->murder_name) > 0)
    fprintf(fp, "MurderName %s~\n", ch->pcdata->murder_name);
    if (ch->pcdata->murder_timer > 0)
    fprintf(fp, "MurderTimer %d\n", ch->pcdata->murder_timer);
    if (ch->pcdata->murder_cooldown > 0)
    fprintf(fp, "MurderCooldown %d\n", ch->pcdata->murder_cooldown);

    if (safe_strlen(ch->pcdata->dream_origin) > 0 && (ch->race == RACE_DREAMCHILD || ch->race == RACE_FANTASY))
    fprintf(fp, "DreamOrigin %s~\n", ch->pcdata->dream_origin);

    if (ch->pcdata->dream_intro[0] != '\0')
    fprintf(fp, "DreamIntro %s~\n", ch->pcdata->dream_intro);
    if (ch->pcdata->dream_description[0] != '\0')
    fprintf(fp, "DreamDesc %s~\n", ch->pcdata->dream_description);
    if (ch->pcdata->home_territory[0] != '\0')
    fprintf(fp, "HomeTerritory %s~\n", ch->pcdata->home_territory);

    fprintf(fp, "Facing %d\n", ch->facing);
    fprintf(fp, "Class %d %d %d\n", ch->pcdata->class_type,
    ch->pcdata->class_faction, ch->pcdata->class_cooldown);

    if (ch->pcdata->class_fame[0] != '\0')
    fprintf(fp, "ClFame %s~\n", ch->pcdata->class_fame);

    fprintf(fp, "Classpoints %d %d\n", ch->pcdata->classpoints,
    ch->pcdata->classtotal);
    fprintf(fp, "ClassOld %d %d\n", ch->pcdata->class_oldfaction,
    ch->pcdata->class_oldlevel);
    fprintf(fp, "ClassNeutral %d\n", ch->pcdata->class_neutral);

    if (ch->pcdata->divine_focus > 0)
    fprintf(fp, "DivineFocus %d\n", ch->pcdata->divine_focus);

    fprintf(fp, "ExtraFocus %d\n", ch->pcdata->extra_focus);
    fprintf(fp,  "BonusOrigin %d\n", ch->pcdata->bonus_origin);

    if (ch->pcdata->last_outside_fullmoon > 0)
    fprintf(fp, "LastMoon %d\n", ch->pcdata->last_outside_fullmoon);
    if (ch->pcdata->last_drained_person > 0)
    fprintf(fp, "LastDrained %d\n", ch->pcdata->last_drained_person);

    if (ch->pcdata->tboosts != 0)
    fprintf(fp, "Tboosts %d\n", ch->pcdata->tboosts);

    fprintf(fp, "Rank %d\n", ch->pcdata->rank_type);

    if (ch->pcdata->vbloodcool != 0)
    fprintf(fp, "VBloodCool %d\n", ch->pcdata->vbloodcool);

    if (ch->pcdata->dream_room > 0)
    fprintf(fp, "DreamRoom %d\n", ch->pcdata->dream_room);
    if (ch->pcdata->dream_sex != 0)
    fprintf(fp, "DreamSex %d\n", ch->pcdata->dream_sex);

    if (ch->pcdata->prison_care > 0)
    fprintf(fp, "PrisonCare %d\n", ch->pcdata->prison_care);
    if (ch->pcdata->prison_mult > 0)
    fprintf(fp, "PrisonMult %d\n", ch->pcdata->prison_mult);

    if (ch->pcdata->understanding[0] != '\0')
    fprintf(fp, "Understanding %s~\n", ch->pcdata->understanding);
    if (ch->pcdata->understandtoggle != 0)
    fprintf(fp, "Understandtoggle %d\n", ch->pcdata->understandtoggle);
    if (ch->pcdata->timeswept > 0)
    fprintf(fp, "Timeswept %d\n", ch->pcdata->timeswept);
    if (ch->pcdata->deaged != 0)
    fprintf(fp, "Timedeaged %d\n", ch->pcdata->deaged);
    if (ch->pcdata->agemod != 0)
    fprintf(fp, "Timeagemod %d\n", ch->pcdata->agemod);
    if (ch->pcdata->maskednumber != 0)
    fprintf(fp, "MaskedNumber %d\n", ch->pcdata->maskednumber);
    if (ch->pcdata->mask_intro_one != NULL && safe_strlen(ch->pcdata->mask_intro_one) > 3)
    fprintf(fp, "MaskIntroOne %s~\n", ch->pcdata->mask_intro_one);
    if (ch->pcdata->mask_intro_two != NULL && safe_strlen(ch->pcdata->mask_intro_two) > 3)
    fprintf(fp, "MaskIntroTwo %s~\n", ch->pcdata->mask_intro_two);
    if (ch->pcdata->eyes_genetic > 0 || ch->pcdata->hair_genetic > 0)
    fprintf(fp, "Genes %d %d %s~\n", ch->pcdata->eyes_genetic,
    ch->pcdata->hair_genetic, ch->pcdata->hair_dyed);
    if (ch->pcdata->abommistcool > 0)
    fprintf(fp, "AbomMistCool %d\n", ch->pcdata->abommistcool);
    if (ch->pcdata->abomblackcool > 0)
    fprintf(fp, "AbomBlackCool %d\n", ch->pcdata->abomblackcool);

    if (ch->pcdata->breach_timer > 0)
    fprintf(fp, "Breach %d %d %d\n", ch->pcdata->breach_origin,
    ch->pcdata->breach_destination, ch->pcdata->breach_timer);

    if (ch->pcdata->boobjob != 0)
    fprintf(fp, "Boobjob %d\n", ch->pcdata->boobjob);

    if (ch->pcdata->fatigue != 0)
    fprintf(fp, "Fatigue %d\n", ch->pcdata->fatigue);

    fprintf(fp, "Difficulty %d\n", ch->pcdata->difficulty);
    if (ch->shape != 0)
    fprintf(fp, "Shape %d\n", ch->shape);

    fprintf(fp, "Skin %s~\n", ch->pcdata->skin);
    fprintf(fp, "Hair %s~\n", ch->pcdata->hair);
    fprintf(fp, "Eyes %s~\n", ch->pcdata->eyes);

    fprintf(fp, "SecretDays %d\n", ch->pcdata->secret_days);
    if (ch->pcdata->secret_recover > 0)
    fprintf(fp, "SecretRecover %d\n", ch->pcdata->secret_recover);

    if (ch->pcdata->intel > 0)
    fprintf(fp, "Intel %d\n", ch->pcdata->intel);
    if (ch->pcdata->last_intel > 0)
    fprintf(fp, "LastIntel %d\n", ch->pcdata->last_intel);

    if (ch->pcdata->in_domain > 0)
    fprintf(fp, "InDomain %d\n", ch->pcdata->in_domain);
    if (ch->pcdata->domain_timer > 0)
    fprintf(fp, "DomainTimer %d\n", ch->pcdata->domain_timer);

    if (ch->pcdata->pending_resources > 0)
    fprintf(fp, "PendingResources %d\n", ch->pcdata->pending_resources);

    if (ch->pcdata->recent_exp > 0)
    fprintf(fp, "RecentExp %d\n", ch->pcdata->recent_exp);

    if (ch->pcdata->spent_resources != 0)
    fprintf(fp, "SpentResources %d\n", ch->pcdata->spent_resources);
    if (ch->pcdata->stasis_time != 0)
    fprintf(fp, "StasisTime %d\n", ch->pcdata->stasis_time);
    if (ch->pcdata->summon_bound != 0)
    fprintf(fp, "SummonBound %d\n", ch->pcdata->summon_bound);

    if (ch->pcdata->stasis_spent_pkarma != 0)
    fprintf(fp, "StasisSpentPKarma %d\n", ch->pcdata->stasis_spent_pkarma);
    if (ch->pcdata->stasis_spent_karma != 0)
    fprintf(fp, "StasisSpentKarma %d\n", ch->pcdata->stasis_spent_karma);
    if (ch->pcdata->stasis_spent_exp != 0)
    fprintf(fp, "StasisSpentExp %d\n", ch->pcdata->stasis_spent_exp);
    if (ch->pcdata->stasis_spent_rpexp != 0)
    fprintf(fp, "StasisSpentRPExp %d\n", ch->pcdata->stasis_spent_rpexp);

    if (ch->wounds != 0)
    fprintf(fp, "Wounds %d\n", ch->wounds);

    if (ch->heal_timer != 0)
    fprintf(fp, "HTime %d\n", ch->heal_timer);
    if (ch->death_timer != 0)
    fprintf(fp, "DTime %d\n", ch->death_timer);

    fprintf(fp, "Atime %d\n", ch->activeat);

    if (ch->pcdata->detention_time > 0)
    fprintf(fp, "DetentionTime %d\n", ch->pcdata->detention_time);

    if (ch->pcdata->market_visit != 0)
    fprintf(fp, "Market %d\n", ch->pcdata->market_visit);

    if (ch->pcdata->hell_power > current_time)
    fprintf(fp, "PowerHell %d\n", ch->pcdata->hell_power);
    if (ch->pcdata->other_power > current_time)
    fprintf(fp, "PowerOther %d\n", ch->pcdata->other_power);
    if (ch->pcdata->godrealm_power > current_time)
    fprintf(fp, "PowerGodrealm %d\n", ch->pcdata->godrealm_power);

    if(safe_strlen(ch->pcdata->private_security) > 0)
    fprintf(fp, "PrivateSecurity %s~\n", ch->pcdata->private_security);

    if (ch->pcdata->timenotalone > 0)
    fprintf(fp, "TimeNotAlone %d\n", ch->pcdata->timenotalone);
    if (ch->pcdata->lastnotalone > 0)
    fprintf(fp, "LastNotAlone %d\n", ch->pcdata->lastnotalone);
    if (ch->pcdata->weakness_status > 0)
    fprintf(fp, "WeaknessStatus %d\n", ch->pcdata->weakness_status);
    if (ch->pcdata->weakness_timer > 0)
    fprintf(fp, "WeaknessTimer %d\n", ch->pcdata->weakness_timer);
    if (ch->pcdata->overworked > 0)
    fprintf(fp, "Overworked %d\n", ch->pcdata->overworked);

    if (safe_strlen(ch->pcdata->ritual_dreamworld) > 1)
    fprintf(fp, "RitualDreamWorld %s~\n", ch->pcdata->ritual_dreamworld);
    if (safe_strlen(ch->pcdata->dream_identity) > 1)
    fprintf(fp, "DreamIdentity %s~\n", ch->pcdata->dream_identity);
    if (safe_strlen(ch->pcdata->identity_world) > 1)
    fprintf(fp, "IdentityWorld %s~\n", ch->pcdata->identity_world);
    if (ch->pcdata->dream_identity_timer > 0)
    fprintf(fp, "DreamIdentityTimer %d\n", ch->pcdata->dream_identity_timer);

    if (safe_strlen(ch->pcdata->last_villain_mod) > 1)
    fprintf(fp, "LastVillainMod %s~\n", ch->pcdata->last_villain_mod);

    if (ch->pcdata->deluded_duration > current_time) {
      fprintf(fp, "DeludedReason %s~\n", ch->pcdata->deluded_reason);
      fprintf(fp, "DeludedDuration %d\n", ch->pcdata->deluded_duration);
      fprintf(fp, "DeludedCost %d\n", ch->pcdata->deluded_cost);
    }

    if (ch->pcdata->nightmare_shifted != 0)
    fprintf(fp, "NightmareShifted %d\n", ch->pcdata->nightmare_shifted);

    if (ch->pcdata->dreamoutfit != 0)
    fprintf(fp, "DreamOutfit %d\n", ch->pcdata->dreamoutfit);

    if (ch->pcdata->dreamfromoutfit != 0)
    fprintf(fp, "DreamFromOutfit %d\n", ch->pcdata->dreamfromoutfit);

    if (ch->pcdata->nightmare_dragged != 0)
    fprintf(fp, "NightMaredragged %d\n", ch->pcdata->nightmare_dragged);

    if (ch->pcdata->last_rp_room != 0)
    fprintf(fp, "LastRPRoom %d\n", ch->pcdata->last_rp_room);

    if (ch->pcdata->availability != 0)
    fprintf(fp, "Availability %d\n", ch->pcdata->availability);

    if (ch->pcdata->heroic > 0)
    fprintf(fp, "Heroic %d\n", ch->pcdata->heroic);
    if (ch->pcdata->maintain_cost > 0)
    fprintf(fp, "MaintainCost %d\n", ch->pcdata->maintain_cost);
    if (ch->pcdata->truepenis > 0)
    fprintf(fp, "TruePenis %d\n", ch->pcdata->truepenis);
    if (ch->pcdata->truebreasts > 0)
    fprintf(fp, "TrueBreasts %d\n", ch->pcdata->truebreasts);
    if (ch->pcdata->sexchange_time > 0)
    fprintf(fp, "SexChange %d\n", ch->pcdata->sexchange_time);
    if (ch->pcdata->maintained_ritual > 0)
    fprintf(fp, "MaintainedRitual %d\n", ch->pcdata->maintained_ritual);
    if (safe_strlen(ch->pcdata->last_ritual) > 1)
    fprintf(fp, "RitualLast %s~\n", ch->pcdata->last_ritual);
    if (safe_strlen(ch->pcdata->ritual_maintainer) > 1)
    fprintf(fp, "RitualMaintainer %s~\n", ch->pcdata->ritual_maintainer);
    if (safe_strlen(ch->pcdata->maintained_target) > 1)
    fprintf(fp, "RitualMaintainTarget %s~\n", ch->pcdata->maintained_target);

    fprintf(fp, "Penis %d\n", ch->pcdata->penis);

    if (ch->pcdata->solitary_time != 0)
    fprintf(fp, "SolitaryTime %d\n", ch->pcdata->solitary_time);

    if (ch->pcdata->tier_raised != 0)
    fprintf(fp, "TierRaised %d\n", ch->pcdata->tier_raised);

    if (ch->pcdata->psychic_attempts > 0)
    fprintf(fp, "PsychicAttempts %d\n", ch->pcdata->psychic_attempts);
    fprintf(fp, "PsychicProof %d\n", ch->pcdata->psychic_proof);

    if (ch->pcdata->extra_powers != 0)
    fprintf(fp, "ExtraPowers %d\n", ch->pcdata->extra_powers);

    if (ch->diminished > 0)
    fprintf(fp, "Diminished %d\n", ch->diminished);
    if (ch->diminish_lt > 0)
    fprintf(fp, "DiminishLT %d\n", ch->diminish_lt);
    if (ch->diminish_till > current_time)
    fprintf(fp, "DiminishTill %d\n", ch->diminish_till);
    if (ch->diminish_vis > 0)
    fprintf(fp, "DiminishVis %d\n", ch->diminish_vis);

    if (ch->pcdata->bust != 0)
    fprintf(fp, "Bust %d\n", ch->pcdata->bust);
    if (ch->pcdata->portaled_from != 0)
    fprintf(fp, "Portal %d\n", ch->pcdata->portaled_from);

    fprintf(fp, "LPaid %d\n", ch->pcdata->last_paid);

    if (ch->pcdata->became_tyrant > 0)
    fprintf(fp, "BecameTyrant %d\n", ch->pcdata->became_tyrant);

    if (ch->donated != 0)
    fprintf(fp, "Donated %d\n", ch->donated);

    fprintf(fp, "Exp %d\n", ch->exp);
    fprintf(fp, "ExpSpent %d\n", ch->spentexp);
    fprintf(fp, "RPExp %d\n", ch->rpexp);
    fprintf(fp, "RPExpSpent %d\n", ch->spentrpexp);

    fprintf(fp, "ExpNew %d\n", ch->newexp);
    fprintf(fp, "ExpNewSpent %d\n", ch->spentnewexp);
    fprintf(fp, "RPExpNew %d\n", ch->newrpexp);
    fprintf(fp, "RPExpNewSpent %d\n", ch->spentnewrpexp);

    if (ch->esteem_faction != 0)
    fprintf(fp, "Esteem %d\n", ch->esteem_faction);
    if (ch->faction != 0)
    fprintf(fp, "Faction %d\n", ch->faction);
    if (ch->esteem_cult != 0)
    fprintf(fp, "EsteemCult %d\n", ch->esteem_cult);
    if (ch->esteem_sect != 0)
    fprintf(fp, "EsteemSect %d\n", ch->esteem_sect);

    if (ch->factiontwo != 0)
    fprintf(fp, "FactionTwo %d\n", ch->factiontwo);

    if (ch->fcore != 0)
    fprintf(fp, "FCore %d\n", ch->fcore);

    if (ch->fsect != 0)
    fprintf(fp, "FSect %d\n", ch->fsect);

    if (ch->fcult != 0)
    fprintf(fp, "FCult %d\n", ch->fcult);

    fprintf(fp, "DeployCore %d\n", ch->deploy_core);
    fprintf(fp, "DeploySect %d\n", ch->deploy_sect);
    fprintf(fp, "DeployCult %d\n", ch->deploy_cult);

    if (ch->factiontrue != 0)
    fprintf(fp, "FactionTrue %d\n", ch->factiontrue);
    if (ch->vassal != 0)
    fprintf(fp, "Vassal %d\n", ch->vassal);
    if (ch->oppress != 0)
    fprintf(fp, "Oppress %d\n", ch->oppress);
    if (ch->abomination != 0)
    fprintf(fp, "Abomination %d\n", ch->abomination);
    if (ch->abomination > 0)
    fprintf(fp, "BlackEyes %d %d\n", ch->pcdata->blackeyes,
    ch->pcdata->blackeyes_power);
    if (ch->pcdata->fleshformed != 0)
    fprintf(fp, "Fleshformed %d\n", ch->pcdata->fleshformed);
    if (ch->pcdata->infuse_date > 0)
    fprintf(fp, "Infused %d\n", ch->pcdata->infuse_date);
    if (ch->pcdata->apower != 0)
    fprintf(fp, "Apower %d\n", ch->pcdata->apower);
    if (ch->pcdata->superlocked != 0)
    fprintf(fp, "Superlocked %d\n", ch->pcdata->superlocked);
    if (ch->pcdata->translocked != 0)
    fprintf(fp, "Translocked %d\n", ch->pcdata->translocked);
    if (ch->pcdata->medic_uses != 0)
    fprintf(fp, "MedicUses %d\n", ch->pcdata->medic_uses);
    if (ch->pcdata->experiment_uses != 0)
    fprintf(fp, "ExperimentUses %d\n", ch->pcdata->experiment_uses);
    if (ch->pcdata->implant_frequency != 0)
    fprintf(fp, "Implant %d\n", ch->pcdata->implant_frequency);
    if (ch->pcdata->mindmessed != 0)
    fprintf(fp, "MindMessed %d\n", ch->pcdata->mindmessed);
    if (ch->pcdata->brainwash_loyalty != NULL && safe_strlen(ch->pcdata->brainwash_loyalty) > 1)
    fprintf(fp, "BrainwashLoyalty %s~\n", ch->pcdata->brainwash_loyalty);
    if (ch->pcdata->brainwash_reidentity != NULL && safe_strlen(ch->pcdata->brainwash_reidentity) > 1)
    fprintf(fp, "BrainwashReidentity %s~\n", ch->pcdata->brainwash_reidentity);

    if (ch->pcdata->maim != NULL && safe_strlen(ch->pcdata->maim) > 1)
    fprintf(fp, "Maim %s~\n", ch->pcdata->maim);

    if (ch->pcdata->egg_daddy > 0)
    fprintf(fp, "Egg %d %d\n", ch->pcdata->egg_daddy, ch->pcdata->egg_date);

    if (is_vampire(ch) || is_werewolf(ch))
    fprintf(fp, "LastAccident %d\n", ch->pcdata->lastaccident);
    if (ch->pcdata->lastshift > 0)
    fprintf(fp, "LastShift %d\n", ch->pcdata->lastshift);
    if (ch->pcdata->lunacy_curse > 0)
    fprintf(fp, "LunacyCurse %d\n", ch->pcdata->lunacy_curse);
    if (is_abom(ch))
    fprintf(fp, "LastNormal %d\n", ch->pcdata->lastnormal);
    if (ch->pcdata->mimic > 0)
    fprintf(fp, "Mimic %d\n", ch->pcdata->mimic);
    if (ch->pcdata->lastidentity > 0)
    fprintf(fp, "LastIdentity %d\n", ch->pcdata->lastidentity);
    if (ch->race == RACE_WIGHT) {
      fprintf(fp, "DecayStage %d\n", ch->pcdata->decay_stage);
      fprintf(fp, "DecayTimer %d\n", ch->pcdata->decay_timer);
      fprintf(fp, "DecayMaxTimer %d\n", ch->pcdata->max_decay_timer);
    }
    if (ch->pcdata->renames > 0)
    fprintf(fp, "Renames %d\n", ch->pcdata->renames);

    fprintf(fp, "Locationx %d\n", ch->x);
    fprintf(fp, "Locationy %d\n", ch->y);

    if (ch->pcdata->mentor[0] != '\0')
    fprintf(fp, "Mentor %s~\n", ch->pcdata->mentor);
    if (ch->pcdata->atolerance != 0)
    fprintf(fp, "Atolerance %d\n", ch->pcdata->atolerance);

    fprintf(fp, "Hfeet %d\n", ch->pcdata->height_feet);
    fprintf(fp, "Hinch %d\n", ch->pcdata->height_inches);

    if (ch->pcdata->stories_run != 0)
    fprintf(fp, "SRun %d\n", ch->pcdata->stories_run);
    if (ch->pcdata->stories_countdown != 0)
    fprintf(fp, "SCount %d\n", ch->pcdata->stories_countdown);
    if (ch->pcdata->rooms_run != 0)
    fprintf(fp, "RRun %d\n", ch->pcdata->rooms_run);
    if (ch->pcdata->rooms_countdown != 0)
    fprintf(fp, "RCount %d\n", ch->pcdata->rooms_countdown);
    if (ch->pcdata->cop_cooldown != 0)
    fprintf(fp, "CopCool %d\n", ch->pcdata->cop_cooldown);
    if (ch->pcdata->nightmare != NULL && safe_strlen(ch->pcdata->nightmare) > 3)
    fprintf(fp, "Nightmare %s~\n", ch->pcdata->nightmare);
    if (ch->pcdata->fame[0] != '\0')
    fprintf(fp, "Fame %s~\n", ch->pcdata->fame);
    if (ch->pcdata->art1[0] != '\0')
    fprintf(fp, "ArtFirst %s~\n", ch->pcdata->art1);
    if (ch->pcdata->art2[0] != '\0')
    fprintf(fp, "ArtSecond %s~\n", ch->pcdata->art2);
    if (ch->pcdata->art3[0] != '\0')
    fprintf(fp, "ArtThird %s~\n", ch->pcdata->art3);
    if (ch->affected != NULL)
    fprintf(fp, "AfBy %s\n", set_print_flag(ch->affected_by));
    if (ch->pcdata->squish != 0)
    fprintf(fp, "Squish %ld\n", ch->pcdata->squish);

    if (ch->pcdata->relapsevampire != 0)
    fprintf(fp, "RelapseVampire %d\n", ch->pcdata->relapsevampire);
    if (ch->pcdata->relapsewerewolf != 0)
    fprintf(fp, "RelapseWerewolf %d\n", ch->pcdata->relapsewerewolf);

    fprintf(fp, "Ftim %s~\n", ch->pcdata->ftime);
    fprintf(fp, "Floc %s~\n", ch->pcdata->floc);
    fprintf(fp, "Fdcr %s~\n", ch->pcdata->fdesc);
    fprintf(fp, "Comm %s\n", set_print_flag(ch->comm));

    if (tch->short_descr[0] != '\0')
    fprintf(fp, "ShD  %s~\n", tch->short_descr);

    if (tch->long_descr[0] != '\0')
    fprintf(fp, "LnD  %s~\n", tch->long_descr);

    if (tch->description[0] != '\0')
    fprintf(fp, "Desc %s~\n", tch->description);

    if (tch->pcdata->history[0] != '\0')
    fprintf(fp, "Hist %s~\n", tch->pcdata->history);

    if (tch->pcdata->file[0] != '\0')
    fprintf(fp, "File %s~\n", tch->pcdata->file);

    if (tch->pcdata->classified[0] != '\0')
    fprintf(fp, "Classified %s~\n", tch->pcdata->classified);

    if (tch->pcdata->char_goals[0] != '\0')
    fprintf(fp, "CharGoals %s~\n", tch->pcdata->char_goals);

    if (tch->pcdata->char_fears[0] != '\0')
    fprintf(fp, "CharFears %s~\n", tch->pcdata->char_fears);

    if (tch->pcdata->char_secrets[0] != '\0')
    fprintf(fp, "CharSecrets %s~\n", tch->pcdata->char_secrets);

    if (tch->pcdata->char_timeline[0] != '\0')
    fprintf(fp, "CharTimeline %s~\n", tch->pcdata->char_timeline);

    if (tch->pcdata->messages[0] != '\0')
    fprintf(fp, "Messages %s~\n", tch->pcdata->messages);
    if (tch->pcdata->chat_handle[0] != '\0')
    fprintf(fp, "ChatHandle %s~\n", tch->pcdata->chat_handle);

    if (ch->pcdata->job_type_one != 0)
    fprintf(fp, "JobOne %d %d %d %s~\n", ch->pcdata->job_type_one,
    ch->pcdata->job_room_one, ch->pcdata->job_delay_one,
    ch->pcdata->job_title_one);
    if (ch->pcdata->job_type_two != 0)
    fprintf(fp, "JobTwo %d %d %d %s~\n", ch->pcdata->job_type_two,
    ch->pcdata->job_room_two, ch->pcdata->job_delay_two,
    ch->pcdata->job_title_two);

    if (ch->pcdata->process_timer > 0 && ch->pcdata->process_target == NULL) {
      fprintf(fp, "Process %d %d %d %s~\n%s~\n", ch->pcdata->process,
      ch->pcdata->process_timer, ch->pcdata->process_subtype,
      ch->pcdata->process_argumentone, ch->pcdata->process_argumenttwo);
    }

    if (ch->pcdata->survey_stage > 0 || ch->pcdata->survey_delay > 0)
    fprintf(fp, "Survey %d %d\n", ch->pcdata->survey_stage,
    ch->pcdata->survey_delay);

    if (ch->pcdata->diss_target != NULL && safe_strlen(ch->pcdata->diss_target) > 1)
    fprintf(fp, "Disspending %s %s~\n", ch->pcdata->diss_target,
    ch->pcdata->diss_message);

    // chatroom gets logged if the player is in the channel now - Discordance
    /*
if (tch->pcdata->chat_history[0] != '\0')


fprintf( fp, "ChatHistory %s~\n",      tch->pcdata->chat_history );
*/

    if(ch->pcdata->missed_chat_connections > 0)
    fprintf(fp, "MissedChatConnections %d\n", ch->pcdata->missed_chat_connections);

    if(ch->pcdata->missed_rp_connections > 0)
    fprintf(fp, "MissedRpConnections %d\n", ch->pcdata->missed_rp_connections);

    if(ch->pcdata->influencer_bank > 0)
    fprintf(fp, "InfluencerBank %d\n", ch->pcdata->influencer_bank);

    if(ch->pcdata->suspend_myhaven > 0)
    fprintf(fp, "SuspendMyHaven %d\n", ch->pcdata->suspend_myhaven);

    if(ch->pcdata->ritual_prey_timer > 0)
    {
      fprintf(fp, "RitualPreyTimer %d\n", ch->pcdata->ritual_prey_timer);
      fprintf(fp, "RitualPreyTarget %s~\n", ch->pcdata->ritual_prey_target);
    }
    fprintf(fp, "QuitRoom %d\n", ch->pcdata->quit_room);

    /*
char * summary_name;
char * summary_intro;
char * summary_content;
char * summary_conclusion;
int summary_type;
int summary_stage;
int summary_blood;
int summary_intel;
int summary_lifeforce;
int summary_helpless;*/
    if(safe_strlen(ch->pcdata->summary_name) > 1)
    {
      fprintf(fp, "SummaryName %s~\n", ch->pcdata->summary_name);
      fprintf(fp, "SummaryIntro %s~\n", ch->pcdata->summary_intro);
      fprintf(fp, "SummaryContent %s~\n", ch->pcdata->summary_content);
      fprintf(fp, "SummaryConclusion %s~\n", ch->pcdata->summary_conclusion);
      fprintf(fp, "SummaryType %d\n", ch->pcdata->summary_type);
      fprintf(fp, "SummaryStage %d\n", ch->pcdata->summary_stage);
      fprintf(fp, "SummaryBlood %d\n", ch->pcdata->summary_blood);
      fprintf(fp, "SummaryIntel %d\n", ch->pcdata->summary_intel);
      fprintf(fp, "SummaryLifeforce %d\n", ch->pcdata->summary_lifeforce);
      fprintf(fp, "SummaryHelpless %d\n", ch->pcdata->summary_helpless);
      fprintf(fp, "SummaryWhen %d\n", ch->pcdata->summary_when);
      fprintf(fp, "SummaryHome %d\n", ch->pcdata->summary_home);
    }

    fprintf(fp, "OperativeCreationType %d\n", ch->pcdata->operative_creation_type);
    fprintf(fp, "OperativeCore %s~\n", ch->pcdata->operative_core);
    fprintf(fp, "OperativeSect %s~\n", ch->pcdata->operative_sect);
    fprintf(fp, "OperativeCult %s~\n", ch->pcdata->operative_cult);

    if(ch->pcdata->active_chatroom > 0)
    fprintf(fp, "ActiveChatroom %d\n", ch->pcdata->active_chatroom);
    fprintf(fp, "ChatroomNew");
    for (x = 0; x < 50; x++)
    fprintf(fp, " %d", ch->pcdata->in_chatroom[x]);
    fprintf(fp, "\n");

    for (pos = 0; pos < 10; pos++) {
      if (ch->pcdata->destiny_in_one[pos] > 0)
      fprintf(fp, "DestinyOne %d %d %s~\n", ch->pcdata->destiny_in_one[pos],
      ch->pcdata->destiny_stage_one[pos],
      ch->pcdata->destiny_in_with_one[pos]);
      if (ch->pcdata->destiny_in_two[pos] > 0)
      fprintf(fp, "DestinyTwo %d %d %s~\n", ch->pcdata->destiny_in_two[pos],
      ch->pcdata->destiny_stage_two[pos],
      ch->pcdata->destiny_in_with_two[pos]);
    }

    fprintf(fp, "DestinyInterestOne");
    for (x = 0; x < 50; x++)
    fprintf(fp, " %d", ch->pcdata->destiny_interest_one[x]);
    fprintf(fp, "\n");
    fprintf(fp, "DestinyInterestTwo");
    for (x = 0; x < 50; x++)
    fprintf(fp, " %d", ch->pcdata->destiny_interest_two[x]);
    fprintf(fp, "\n");

    fprintf(fp, "DestinyBlockOne");
    for (x = 0; x < 50; x++)
    fprintf(fp, " %d", ch->pcdata->destiny_block_one[x]);
    fprintf(fp, "\n");
    fprintf(fp, "DestinyBlockTwo");
    for (x = 0; x < 50; x++)
    fprintf(fp, " %d", ch->pcdata->destiny_block_two[x]);
    fprintf(fp, "\n");

    for (pos = 0; pos < 10; pos++) {
      if (ch->pcdata->destiny_lockout_time[pos] > current_time)
      fprintf(fp, "DestinyLockout %d %s~\n",
      ch->pcdata->destiny_lockout_time[pos],
      ch->pcdata->destiny_lockout_char[pos]);
    }
    for (pos = 0; pos < 20; pos++) {
      if (safe_strlen(ch->pcdata->rp_log[pos]) > 1)
      fprintf(fp, "RPLog %d %s~\n", pos, ch->pcdata->rp_log[pos]);
    }
    for (pos = 0; pos < 20; pos++) {
      if (ch->pcdata->outfit_nickpoint[pos] > 0)
      fprintf(fp, "OutNick %d %d %s~\n", pos, ch->pcdata->outfit_nickpoint[pos],
      ch->pcdata->outfit_nicknames[pos]);
    }

    for (pos = 0; pos < 10; pos++) {
      if(ch->pcdata->boon_history[pos] > 0)
      fprintf(fp, "BoonHistory %d %d\n", pos, ch->pcdata->boon_history[pos]);
      if(ch->pcdata->curse_history[pos] > 0)
      fprintf(fp, "CurseHistory %d %d\n", pos, ch->pcdata->curse_history[pos]);
    }
    if(ch->pcdata->boon > 0 && ch->pcdata->boon_timeout > current_time)
    fprintf(fp, "Boon %d %d\n", ch->pcdata->boon, ch->pcdata->boon_timeout);
    if(ch->pcdata->curse > 0 && ch->pcdata->curse_timeout > current_time)
    fprintf(fp, "Curse %d %d %s~\n", ch->pcdata->curse, ch->pcdata->curse_timeout, ch->pcdata->curse_text);
    for (pos = 0; pos < 10; pos++) {
      if (ch->pcdata->driveloc[pos] > 0)
      fprintf(fp, "Drive %d %d %s~\n", pos, ch->pcdata->driveloc[pos],
      ch->pcdata->drivenames[pos]);
    }
    for (pos = 0; pos < 10; pos++) {
      if (ch->pcdata->garage_cost[pos] > 0) {
        fprintf(fp, "Garage %d %d %d %d %d %d %d %s~\n%s~\n%s~\n", pos,
        ch->pcdata->garage_typeone[pos], ch->pcdata->garage_typetwo[pos],
        ch->pcdata->garage_status[pos], ch->pcdata->garage_timer[pos],
        ch->pcdata->garage_location[pos], ch->pcdata->garage_cost[pos],
        ch->pcdata->garage_name[pos], ch->pcdata->garage_desc[pos],
        ch->pcdata->garage_lplate[pos]);
      }
    }
    for (pos = 0; pos < 20; pos++) {
      if (ch->pcdata->contact_names[pos] != NULL && safe_strlen(ch->pcdata->contact_names[pos]) > 2) {
        fprintf(fp, "Contact %d %d %d %s~\n%s~\n", pos,
        ch->pcdata->contact_jobs[pos], ch->pcdata->contact_cooldowns[pos],
        ch->pcdata->contact_names[pos], ch->pcdata->contact_descs[pos]);
      }
    }
    if(strlen(ch->pcdata->mermaid_change_to) > 1)
    fprintf(fp, "MermaidChangeTo %s~\n", ch->pcdata->mermaid_change_to);
    if(strlen(ch->pcdata->mermaid_change_from) > 1)
    fprintf(fp, "MermaidChangeFrom %s~\n", ch->pcdata->mermaid_change_from);
    if(strlen(ch->pcdata->wolf_change_to) > 1)
    fprintf(fp, "WolfChangeTo %s~\n", ch->pcdata->wolf_change_to);
    if(strlen(ch->pcdata->wolf_change_from) > 1)
    fprintf(fp, "WolfChangeFrom %s~\n", ch->pcdata->wolf_change_from);
    if(strlen(ch->pcdata->radio_action) > 1)
    fprintf(fp, "RadioAction %s~\n", ch->pcdata->radio_action);
    if(strlen(ch->pcdata->makeup_light) > 1)
    fprintf(fp, "MakeupLight %s~\n", ch->pcdata->makeup_light);
    if(strlen(ch->pcdata->makeup_medium) > 1)
    fprintf(fp, "MakeupMedium %s~\n", ch->pcdata->makeup_medium);
    if(strlen(ch->pcdata->makeup_heavy) > 1)
    fprintf(fp, "MakeupHeavy %s~\n", ch->pcdata->makeup_heavy);
    pos = 0;
    for (int ipos = 0; ipos < 6; ipos++) {
      if (ch->pcdata->animal_names[ipos] != NULL && safe_strlen(ch->pcdata->animal_names[ipos]) > 1) {
        fprintf(fp, "AnimalForm %d %d %d %s~\n%s~\n%s~\n%s~\n", pos,
        ch->pcdata->animal_weights[pos], ch->pcdata->animal_genus[pos],
        ch->pcdata->animal_names[pos], ch->pcdata->animal_intros[pos],
        ch->pcdata->animal_species[pos], ch->pcdata->animal_descs[pos]);
        fprintf(fp, "AnimalChange %d %s~\n%s~\n", pos,
        ch->pcdata->animal_change_to[pos],
        ch->pcdata->animal_change_from[pos]);
        fprintf(
        fp,
        "AnimalStats %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
        pos, ch->pcdata->animal_stats[pos][0],
        ch->pcdata->animal_stats[pos][1], ch->pcdata->animal_stats[pos][2],
        ch->pcdata->animal_stats[pos][3], ch->pcdata->animal_stats[pos][4],
        ch->pcdata->animal_stats[pos][5], ch->pcdata->animal_stats[pos][6],
        ch->pcdata->animal_stats[pos][7], ch->pcdata->animal_stats[pos][8],
        ch->pcdata->animal_stats[pos][9], ch->pcdata->animal_stats[pos][10],
        ch->pcdata->animal_stats[pos][11], ch->pcdata->animal_stats[pos][12],
        ch->pcdata->animal_stats[pos][13], ch->pcdata->animal_stats[pos][14],
        ch->pcdata->animal_stats[pos][15], ch->pcdata->animal_stats[pos][16],
        ch->pcdata->animal_stats[pos][17], ch->pcdata->animal_stats[pos][18],
        ch->pcdata->animal_stats[pos][19], ch->pcdata->animal_stats[pos][20],
        ch->pcdata->animal_stats[pos][21], ch->pcdata->animal_stats[pos][22],
        ch->pcdata->animal_stats[pos][23], ch->pcdata->animal_stats[pos][24]);
        pos++;
      }
    }
    for (pos = 0; pos < 5; pos++) {
      fprintf(fp, "Monstername %d %s~\n", pos, ch->pcdata->monster_names[0][pos]);
      fprintf(fp, "Monstershort %d %s~\n", pos,
      ch->pcdata->monster_names[1][pos]);
      fprintf(fp, "Monsterdesc %d %s~\n", pos, ch->pcdata->monster_names[2][pos]);
      fprintf(
      fp, "Monsterdisciplines %d %d %d %d %d %d %d %d %d %d %d\n", pos,
      ch->pcdata->monster_discs[0][pos], ch->pcdata->monster_discs[1][pos],
      ch->pcdata->monster_discs[2][pos], ch->pcdata->monster_discs[3][pos],
      ch->pcdata->monster_discs[4][pos], ch->pcdata->monster_discs[5][pos],
      ch->pcdata->monster_discs[6][pos], ch->pcdata->monster_discs[7][pos],
      ch->pcdata->monster_discs[8][pos], ch->pcdata->monster_discs[9][pos]);
    }

    for (pos = 0; pos < 10; pos++) {
      if (ch->pcdata->storycon[pos][0] > 0 && ch->pcdata->storycon[pos][2] > 0) {
        fprintf(fp, "ConditionStory %d %d %d\n", ch->pcdata->storycon[pos][0],
        ch->pcdata->storycon[pos][1], ch->pcdata->storycon[pos][2]);
      }
    }
    for (pos = 0; pos < 3; pos++) {
      fprintf(fp, "LastPraiseMessage %d %s~\n", pos,
      ch->pcdata->last_praise_message[pos]);
      fprintf(fp, "LastDissMessage %d %s~\n", pos,
      ch->pcdata->last_diss_message[pos]);
    }
    fprintf(fp, "LastRumor %s~\n", ch->pcdata->last_rumor);

    if (ch->pcdata->territory_editing[TEDIT_TIMER] > 0) {
      fprintf(fp, "TEdit");
      for (pos = 0; pos < 15; pos++)
      fprintf(fp, " %d", ch->pcdata->territory_editing[pos]);
      fprintf(fp, "\n");
    }

    fprintf(fp, "WeekTracker");
    for (pos = 0; pos < 40; pos++)
    fprintf(fp, " %d", ch->pcdata->week_tracker[pos]);
    fprintf(fp, "\n");
    fprintf(fp, "LifeTracker");
    for (pos = 0; pos < 40; pos++)
    fprintf(fp, " %d", ch->pcdata->life_tracker[pos]);
    fprintf(fp, "\n");

    for (pos = 0; pos < 10; pos++) {
      if (ch->pcdata->augdisc_timer[pos] > 0) {
        fprintf(fp, "AugDisc %d %d %d %d\n", pos, ch->pcdata->augdisc_timer[pos],
        ch->pcdata->augdisc_disc[pos], ch->pcdata->augdisc_level[pos]);
      }
      if (ch->pcdata->augskill_timer[pos] > 0) {
        fprintf(fp, "AugSkill %d %d %d %d\n", pos,
        ch->pcdata->augskill_timer[pos], ch->pcdata->augskill_skill[pos],
        ch->pcdata->augskill_level[pos]);
      }
    }

    if (ch->pcdata->destiny_feature > 0) {
      fprintf(fp, "DestinyFeature %d\n", ch->pcdata->destiny_feature);
      fprintf(fp, "DestinyFeatureRoom %d\n", ch->pcdata->destiny_feature_room);
    }

    if (ch->pcdata->feedback[0] != '\0')
    fprintf(fp, "Feedback %s~\n", ch->pcdata->feedback);
    if (ch->pcdata->feedbackcooldown != 0)
    fprintf(fp, "Feedbackcool %d\n", ch->pcdata->feedbackcooldown);
    if (ch->pcdata->travel_to != 0)
    fprintf(fp, "Travelto %d\n", ch->pcdata->travel_to);
    if (ch->pcdata->travel_from != 0)
    fprintf(fp, "Travelfrom %d\n", ch->pcdata->travel_from);
    if (ch->pcdata->travel_time != 0)
    fprintf(fp, "Traveltime %d\n", ch->pcdata->travel_time);
    if (ch->pcdata->travel_type != 0)
    fprintf(fp, "Traveltype %d\n", ch->pcdata->travel_type);

    for (pos = 0; pos < 10; pos++) {
      if (ch->pcdata->feedbacktim[pos] == 0)
      continue;

      fprintf(fp, "Feedbackincoming %d %d %s~\n", ch->pcdata->feedbackbon[pos],
      ch->pcdata->feedbacktim[pos], ch->pcdata->feedbackinc[pos]);
    }
    if (ch->pcdata->xpbonus != 0)
    fprintf(fp, "BonusXP %d\n", ch->pcdata->xpbonus);
    if (ch->pcdata->rpxpbonus != 0)
    fprintf(fp, "BonusRPXP %d\n", ch->pcdata->rpxpbonus);

    fprintf(fp, "CreateDate %d\n", ch->pcdata->create_date);

    if (ch->pcdata->beacon != 0)
    fprintf(fp, "Beacon %d\n", ch->pcdata->beacon);
    if (ch->pcdata->last_share != 0)
    fprintf(fp, "LastShare %d\n", ch->pcdata->last_share);
    if (ch->pcdata->timebanished != 0)
    fprintf(fp, "BanishedTime %d\n", ch->pcdata->timebanished);
    if (ch->pcdata->ally_limit != 0)
    fprintf(fp, "LimitAlly %d\n", ch->pcdata->ally_limit);
    if (ch->pcdata->minion_limit != 0)
    fprintf(fp, "LimitMinion %d\n", ch->pcdata->minion_limit);
    if (ch->pcdata->monster_fed != 800)
    fprintf(fp, "MonsterFed %d\n", ch->pcdata->monster_fed);

    // ignored characters - Discordance
    for (pos = 0; pos < 50; pos++) {
      if (!str_cmp(ch->pcdata->ignored_characters[pos], ""))
      continue;
      fprintf(fp, "IgnoredCharacter %s\n", ch->pcdata->ignored_characters[pos]);
    }
    // ignored accounts - Discordance 10/13/2018
    for (pos = 0; pos < 50; pos++) {
      if (!str_cmp(ch->pcdata->ignored_accounts[pos], ""))
      continue;
      fprintf(fp, "IgnoredAccount %s\n", ch->pcdata->ignored_accounts[pos]);
    }
    // where banned characters - Discordance
    for (pos = 0; pos < 50; pos++) {
      if (!str_cmp(ch->pcdata->nowhere_characters[pos], ""))
      continue;
      fprintf(fp, "NoWhereCharacter %s\n", ch->pcdata->nowhere_characters[pos]);
    }
    // where banned characters - Discordance
    for (pos = 0; pos < 50; pos++) {
      if (!str_cmp(ch->pcdata->nowhere_accounts[pos], ""))
      continue;
      fprintf(fp, "NoWhereAccount %s\n", ch->pcdata->nowhere_accounts[pos]);
    }
    // where banned sexes - Discordance
    if (ch->pcdata->nowhere_female != 0)
    fprintf(fp, "NoWhereFemale %d\n", ch->pcdata->nowhere_female);
    if (ch->pcdata->nowhere_male != 0)
    fprintf(fp, "NoWhereMale %d\n", ch->pcdata->nowhere_male);

    for (pos = 0; pos < 50; pos++) {
      if (ch->pcdata->speed_numbers[pos] == 0)
      continue;

      fprintf(fp, "SDial %d %s\n", ch->pcdata->speed_numbers[pos],
      ch->pcdata->speed_names[pos]);
    }
    for (pos = 0; pos < 50; pos++) {
      if (ch->pcdata->file_factions[pos] == 0)
      continue;

      fprintf(fp, "FFile %d %s~\n", ch->pcdata->file_factions[pos],
      ch->pcdata->file_message[pos]);
    }

    for (pos = 0; pos < 10; pos++) {
      if (ch->pcdata->chan_names[pos] != NULL && safe_strlen(ch->pcdata->chan_names[pos]) > 1)
      fprintf(fp, "CommChans %d %d %s\n", ch->pcdata->chan_status[pos],
      ch->pcdata->chan_numbers[pos], ch->pcdata->chan_names[pos]);
    }

    for (pos = 0; pos < MAX_TRUSTS; pos++) {
      if (ch->pcdata->trust_levels[pos] == 0)
      continue;

      fprintf(fp, "Trusts %d %s\n", ch->pcdata->trust_levels[pos],
      ch->pcdata->trust_names[pos]);
    }

    if (tch->prompt != NULL || !str_cmp(tch->prompt, "<%dhp %dmv> %p%c"))
    fprintf(fp, "Prom %s~\n", tch->prompt);
    if (tch->pcdata->last_name[0] != '\0')
    fprintf(fp, "Lname %s~\n", tch->pcdata->last_name);
    if (tch->pcdata->storyrunner[0] != '\0')
    fprintf(fp, "StoryRunner %s~\n", tch->pcdata->storyrunner);
    if (tch->pcdata->guest_of[0] != '\0')
    fprintf(fp, "GuestOf %s~\n", tch->pcdata->guest_of);
    if (tch->pcdata->pledge[0] != '\0')
    fprintf(fp, "Pledge %s~\n", tch->pcdata->pledge);
    if (tch->pcdata->pledge_stealth != 0)
    fprintf(fp, "PledgeStealth %d\n", tch->pcdata->pledge_stealth);
    if (tch->pcdata->talk[0] != '\0')
    fprintf(fp, "Talk %s~\n", tch->pcdata->talk);
    if (tch->pcdata->verbal_color != NULL && tch->pcdata->verbal_color[0] != '\0')
    fprintf(fp, "VerbalColor %s~\n", tch->pcdata->verbal_color);

    fprintf(fp, "LastIP %s~\n", tch->last_ip);

    if (tch->pcdata->lastwarcheck != 0)
    fprintf(fp, "LastWar %d\n", tch->pcdata->lastwarcheck);
    if (tch->pcdata->police_timer != 0)
    fprintf(fp, "PoliceTimer %d\n", tch->pcdata->police_timer);
    if (tch->pcdata->police_intensity != 0)
    fprintf(fp, "PoliceIntensity %d\n", tch->pcdata->police_intensity);
    if (tch->pcdata->police_number != 0)
    fprintf(fp, "PoliceNumber %d\n", tch->pcdata->police_number);

    if (tch->pcdata->base_standing != 0)
    fprintf(fp, "InBase %d\n", tch->pcdata->base_standing);
    if (tch->pcdata->social_praise != 0)
    fprintf(fp, "InPraise %d\n", tch->pcdata->social_praise);
    if (tch->pcdata->social_behave != 0)
    fprintf(fp, "InBehave %d\n", tch->pcdata->social_behave);
    if (tch->pcdata->influence_balance != 0)
    fprintf(fp, "InBalance %d\n", tch->pcdata->influence_balance);
    if (tch->pcdata->scheme_influence != 0)
    fprintf(fp, "InScheme %d\n", tch->pcdata->scheme_influence);
    if (tch->pcdata->faction_influence != 0)
    fprintf(fp, "InFaction %d\n", tch->pcdata->faction_influence);
    if (tch->pcdata->super_influence != 0)
    fprintf(fp, "InSuper %d\n", tch->pcdata->super_influence);
    if (tch->pcdata->influence_bank != 0)
    fprintf(fp, "InBank %d\n", tch->pcdata->influence_bank);

    fprintf(fp, "LastPraised 0 %s~\n", tch->pcdata->last_praised[0]);
    fprintf(fp, "LastPraised 1 %s~\n", tch->pcdata->last_praised[1]);
    fprintf(fp, "LastPraised 2 %s~\n", tch->pcdata->last_praised[2]);

    fprintf(fp, "LastDissed 0 %s~\n", tch->pcdata->last_dissed[0]);
    fprintf(fp, "LastDissed 1 %s~\n", tch->pcdata->last_dissed[1]);
    fprintf(fp, "LastDissed 2 %s~\n", tch->pcdata->last_dissed[2]);

    if (tch->pcdata->association != 0)
    fprintf(fp, "Association %d\n", tch->pcdata->association);
    if (tch->pcdata->adventure_fatigue != 0)
    fprintf(fp, "AdventureFatigue %d\n", tch->pcdata->adventure_fatigue);

    if (tch->pcdata->true_id > 0) {
      fprintf(fp, "TrueID %d\n", tch->pcdata->true_id);
    }

    if (tch->pcdata->influence != 0)
    fprintf(fp, "Influence %d\n", tch->pcdata->influence);

    fprintf(fp, "AttractBase %d\n", tch->pcdata->attract_base);

    fprintf(fp, "AttractCount %d\n", (int)tch->pcdata->attract_count*100);
    fprintf(fp, "AttractCFashion %d\n", (int)tch->pcdata->attract_count_fashion*100);
    fprintf(fp, "AttractCPhys %d\n", (int)tch->pcdata->attract_count_phys*100);
    fprintf(fp, "AttractCSkin %d\n", (int)tch->pcdata->attract_count_skin*100);

    fprintf(fp, "Photos %s~\n", ch->pcdata->photo_msgs);

    if (tch->pcdata->attract_mod != 0)
    fprintf(fp, "AttractMod %d\n", tch->pcdata->attract_mod);
    if (tch->karma != 0)
    fprintf(fp, "Karma %d\n", tch->karma);
    if (tch->pcdata->earned_karma != 0)
    fprintf(fp, "KarmaEarned %d\n", tch->pcdata->earned_karma);
    fprintf(fp, "KarmaSpent %d\n", tch->spentkarma);
    fprintf(fp, "KarmaPersonal %d\n", tch->pkarma);
    fprintf(fp, "KarmaPersonalSpent %d\n", tch->spentpkarma);

    fprintf(fp, "TimeSinceTrain %d\n", tch->pcdata->time_since_train);
    fprintf(fp, "ExpCap %d\n", tch->pcdata->exp_cap);
    fprintf(fp, "RPExpCap %d\n", tch->pcdata->rpexp_cap);


    if (tch->pcdata->lured_room > 0)
    fprintf(fp, "Lure %d\n", ch->pcdata->lured_room);

    fprintf(fp, "BloodPotency %d\n", tch->pcdata->blood_potency);

    if (tch->pcdata->blood_ticks != 0)
    fprintf(fp, "BloodTicks %d\n", tch->pcdata->blood_ticks);
    if (tch->pcdata->bond_drop != 0)
    fprintf(fp, "BondDrop %d\n", tch->pcdata->bond_drop);
    if (tch->pcdata->blood_date != 0)
    fprintf(fp, "BloodDate %d\n", tch->pcdata->blood_date);
    if (tch->pcdata->blood_level != 0)
    fprintf(fp, "BloodLevel %d\n", tch->pcdata->blood_level);

    fprintf(fp, "Payscale %d\n", tch->pcdata->payscale);

    for (pos = 0; pos < 5; pos++) {
      fprintf(fp, "Bonds %d %s~\n", pos, tch->pcdata->bonds[pos]);
    }

    for (pos = 0; pos < 10; pos++) {
      if (tch->pcdata->relationship_type[pos] != 0)
      fprintf(fp, "Relationship %d %d %s~\n", pos,
      tch->pcdata->relationship_type[pos],
      tch->pcdata->relationship[pos]);
    }

    if (tch->pcdata->packcooldown != 0)
    fprintf(fp, "RelPackCooldown %d\n", tch->pcdata->packcooldown);
    if (tch->pcdata->teachercooldown != 0)
    fprintf(fp, "RelTeacherCooldown %d\n", tch->pcdata->teachercooldown);

    // Shapeshifting variables - Discordance
    if (tch->pcdata->wolfdesc[0] != '\0')
    fprintf(fp, "Wolfdesc %s~\n", tch->pcdata->wolfdesc);
    if (tch->pcdata->wolfintro[0] != '\0')
    fprintf(fp, "Wolfintro %s~\n", tch->pcdata->wolfintro);
    if (tch->pcdata->mermaiddesc[0] != '\0')
    fprintf(fp, "Mermaiddesc %s~\n", tch->pcdata->mermaiddesc);
    if (tch->pcdata->mermaidintro[0] != '\0')
    fprintf(fp, "Mermaidintro %s~\n", tch->pcdata->mermaidintro);

    // Institute Variables - Discordance
    if (tch->pcdata->institute_contraband_violations != 0)
    fprintf(fp, "InstituteContrabandViolations %d\n", tch->pcdata->institute_contraband_violations);
    if (tch->pcdata->clique_role != 0)
    fprintf(fp, "CliqueRole %d\n", tch->pcdata->clique_role);
    if (tch->pcdata->clique_rolechange != 0)
    fprintf(fp, "CliqueRoleChange %d\n", tch->pcdata->clique_rolechange);

    // Sex Variables - Discordance
    if (tch->pcdata->last_sex != 0)
    fprintf(fp, "LastSex %d\n", tch->pcdata->last_sex);
    if (tch->pcdata->last_sextype != 0)
    fprintf(fp, "LastSextype %d\n", tch->pcdata->last_sextype);
    if (tch->pcdata->last_sexprotection != 0)
    fprintf(fp, "LastSexprotection %d\n", tch->pcdata->last_sexprotection);
    if (tch->pcdata->last_shower != 0)
    fprintf(fp, "LastShower %d\n", tch->pcdata->last_shower);
    if (tch->pcdata->hp_sex != 0)
    fprintf(fp, "HPSex %d\n", tch->pcdata->hp_sex);

    fprintf(fp, "SexPotency %d\n",
    tch->pcdata->sex_potency); // this always needs to be known because it
    // defaults to 100 for men and 0 for women
    if (tch->pcdata->natural_fertility != 100)
    fprintf(fp, "Fertility %d\n", tch->pcdata->natural_fertility);
    if (tch->pcdata->testosterone != 0)
    fprintf(fp, "Testosterone %d\n", tch->pcdata->testosterone);
    if (tch->pcdata->maledevelopment != 0)
    fprintf(fp, "MaleDevelopment %d\n", tch->pcdata->maledevelopment);
    if (tch->pcdata->estrogen != 0)
    fprintf(fp, "Estrogen %d\n", tch->pcdata->estrogen);
    if (tch->pcdata->femaledevelopment != 0)
    fprintf(fp, "FemaleDevelopment %d\n", tch->pcdata->femaledevelopment);
    if (tch->pcdata->menstruation != 0)
    fprintf(fp, "Menstruation %d\n", tch->pcdata->menstruation);
    if (tch->pcdata->ovulation != 0)
    fprintf(fp, "Ovulation %d\n", tch->pcdata->ovulation);
    if (tch->pcdata->sex_dirty == TRUE)
    fprintf(fp, "SexDirty %d\n", tch->pcdata->sex_dirty);
    if (tch->pcdata->virginity_lost != 0)
    fprintf(fp, "VirginityLost %d\n", tch->pcdata->virginity_lost);
    if (tch->pcdata->hymen_lost != 0)
    fprintf(fp, "HymenLost %d\n", tch->pcdata->hymen_lost);
    if (tch->pcdata->inseminated != 0)
    fprintf(fp, "Inseminated %d\n", tch->pcdata->inseminated);
    if (tch->pcdata->inseminated_type != 0)
    fprintf(fp, "InseminatedType %d\n", tch->pcdata->inseminated_type);
    if (tch->pcdata->inseminated_daddy_ID != 0)
    fprintf(fp, "InseminatedDaddyID %d\n", tch->pcdata->inseminated_daddy_ID);
    if (tch->pcdata->impregnated != 0)
    fprintf(fp, "Impregnated %d\n", tch->pcdata->impregnated);
    if (tch->pcdata->impregnated_type != 0)
    fprintf(fp, "ImpregnatedType %d\n", tch->pcdata->impregnated_type);
    if (tch->pcdata->impregnated_daddy_ID != 0)
    fprintf(fp, "ImpregnatedDaddyID %d\n", tch->pcdata->impregnated_daddy_ID);
    if (tch->pcdata->due_date != 0)
    fprintf(fp, "DueDate %d\n", tch->pcdata->due_date);
    if (tch->pcdata->auntflo_called != FALSE)
    fprintf(fp, "AuntFloCalled %d\n", tch->pcdata->auntflo_called);
    if (tch->pcdata->daily_upkeep != 0)
    fprintf(fp, "DailyUpkeep %d\n", tch->pcdata->daily_upkeep);
    if (tch->pcdata->manual_upkeep != 0)
    fprintf(fp, "ManualUpkeep %d\n", tch->pcdata->manual_upkeep);
    fprintf(fp, "LastNewSexed 0 %s~\n", tch->pcdata->last_sexed[0]);
    fprintf(fp, "LastNewSexed 1 %s~\n", tch->pcdata->last_sexed[1]);
    fprintf(fp, "LastNewSexed 2 %s~\n", tch->pcdata->last_sexed[2]);
    fprintf(fp, "LastSexedID %d %d %d\n", tch->pcdata->last_sexed_ID[0],
    tch->pcdata->last_sexed_ID[1], tch->pcdata->last_sexed_ID[2]);
    if (tch->pcdata->last_true_sexed_ID != 0)
    fprintf(fp, "LastTrueSexedID %d\n", tch->pcdata->last_true_sexed_ID);

    fprintf(fp, "CountDreamsex %d\n", tch->pcdata->count_dreamsex);
    if (tch->pcdata->without_sleep != 0)
    fprintf(fp, "WithoutSleep %d\n", tch->pcdata->without_sleep);

    fprintf(fp, "VPlayer %d\n", ch->pcdata->version_player);

    // Ghost Variables - Discordance
    if (tch->pcdata->ghost_pool != 0)
    fprintf(fp, "GhostPool %d\n", tch->pcdata->ghost_pool);
    if (tch->pcdata->ghost_banishment != 0)
    fprintf(fp, "GhostBanishment %d\n", tch->pcdata->ghost_banishment);
    if (tch->pcdata->final_death_date != 0)
    fprintf(fp, "FinalDeathDate %d\n", tch->pcdata->final_death_date);
    if (tch->pcdata->ghost_room != 0)
    fprintf(fp, "GhostRoom %d\n", tch->pcdata->ghost_room);

    if (tch->pcdata->spectre != 0)
    fprintf(fp, "Spectre %d\n", tch->pcdata->spectre);
    if (ch->privaterpexp > 0)
    fprintf(fp, "PrivateRPExp %d\n", tch->privaterpexp);
    if (ch->privatekarma > 0)
    fprintf(fp, "PrivateKarma %d\n", tch->privatekarma);

    fprintf(fp, "RememberDetail %s~\n", tch->pcdata->remember_detail);

    if (tch->pcdata->enthralling[0] != '\0')
    fprintf(fp, "Enthralling %s~\n", tch->pcdata->enthralling);
    if (tch->pcdata->enrapturing[0] != '\0')
    fprintf(fp, "Enrapturing %s~\n", tch->pcdata->enrapturing);
    if (tch->pcdata->enthralled[0] != '\0')
    fprintf(fp, "Enthralled %s~\n", tch->pcdata->enthralled);
    if (tch->pcdata->enraptured[0] != '\0')
    fprintf(fp, "Enraptured %s~\n", tch->pcdata->enraptured);
    if (tch->pcdata->enlinked[0] != '\0')
    fprintf(fp, "Enlinked %s~\n", tch->pcdata->enlinked);
    if (tch->pcdata->enlinktime != 0)
    fprintf(fp, "Enlinktime %d\n", tch->pcdata->enlinktime);

    fprintf(fp, "CDisc %d %s~\n", tch->pcdata->cdisc_range,
    tch->pcdata->cdisc_name);

    if (tch->pcdata->haunt_timer > 0)
    fprintf(fp, "Haunt %d %s~\n", tch->pcdata->haunt_timer,
    tch->pcdata->haunter);

    if (tch->pcdata->order_timer > 0)
    fprintf(fp, "Order %d %d %d %s~\n", tch->pcdata->order_timer,
    tch->pcdata->order_type, tch->pcdata->order_amount,
    tch->pcdata->order_target);

    if (tch->pcdata->tail_timer > 0)
    fprintf(fp, "Tail %d %s~\n", tch->pcdata->tail_timer, tch->pcdata->tailing);

    if (tch->pcdata->tail_mask > 0)
    fprintf(fp, "TailMask %d\n", tch->pcdata->tail_mask);

    if (tch->pcdata->guard_number > 0)
    fprintf(fp, "Guarded %d %d %d\n", tch->pcdata->guard_number,
    tch->pcdata->guard_faction, tch->pcdata->guard_expiration);

    if (tch->pcdata->bloodaura > 0)
    fprintf(fp, "BloodAura %d\n", tch->pcdata->bloodaura);

    for (x = 0; x < MAX_COVERS + 10; x++) {
      if (str_cmp(tch->pcdata->focused_descs[x], ""))
      fprintf(fp, "FDesc %d %s~\n", x, tch->pcdata->focused_descs[x]);
    }
    for (x = 0; x < MAX_COVERS + 10; x++) {
      if (str_cmp(tch->pcdata->detail_over[x], ""))
      fprintf(fp, "DetailOver %d %s~\n", x, tch->pcdata->detail_over[x]);
    }
    for (x = 0; x < MAX_COVERS + 10; x++) {
      if (str_cmp(tch->pcdata->detail_under[x], ""))
      fprintf(fp, "DetailUnder %d %s~\n", x, tch->pcdata->detail_under[x]);
    }
    for (x = 0; x < MAX_COVERS + 10; x++) {
      if (str_cmp(tch->pcdata->scars[x], ""))
      fprintf(fp, "Scar %d %s~\n", x, tch->pcdata->scars[x]);
    }

    for (x = 0; x < 5; x++) {
      if (str_cmp(tch->pcdata->languages[x], ""))
      fprintf(fp, "Language %d %s~\n", x, tch->pcdata->languages[x]);
    }

    if (str_cmp(tch->pcdata->scent, "")) {
      fprintf(fp, "Scent %s~\n", tch->pcdata->scent);
      fprintf(fp, "ScCost %d\n", tch->pcdata->perfume_cost);
    }

    if (str_cmp(tch->pcdata->taste, ""))
    fprintf(fp, "Taste %s~\n", tch->pcdata->taste);

    if (ch->pcdata->encounter_number > 0)
    fprintf(fp, "EncounterNumber %d\n", tch->pcdata->encounter_number);
    if (ch->pcdata->encounter_orig_room > 0)
    fprintf(fp, "EncounterRoom %d\n", tch->pcdata->encounter_orig_room);

    if (ch->pcdata->genesis_stage > 0)
    fprintf(fp, "Genesis %d\n", tch->pcdata->genesis_stage);

    if (ch->pcdata->karma_battery > 0)
    fprintf(fp, "KarmaBattery %d\n", tch->pcdata->karma_battery);

    if (ch->pcdata->escape_timer > 0)
    fprintf(fp, "EscapeTimer %d\n", tch->pcdata->escape_timer);

    if (ch->pcdata->mindbroken > 0)
    fprintf(fp, "MindBroken %d\n", tch->pcdata->mindbroken);

    if (tch->pcdata->form_change_date > current_time)
    fprintf(fp, "FormChangeDate %d\n", tch->pcdata->form_change_date);

    if (pact_holder(tch))
    fprintf(fp, "DemonPact %s~\n", tch->pcdata->demon_pact);

    if (tch->pcdata->lingering_sanc > 0)
    fprintf(fp, "LingeringSanc %d\n", tch->pcdata->lingering_sanc);

    if (tch->pcdata->prison_emotes > 0)
    fprintf(fp, "PrisonEmotes %d\n", tch->pcdata->prison_emotes);

    for (x = 0; x < 25; x++) {
      if (tch->pcdata->previous_focus_vnum[x] > 0) {
        fprintf(fp, "PreviousFocus %d %d\n", tch->pcdata->previous_focus_vnum[x],
        tch->pcdata->previous_focus_level[x]);
      }
    }

    for (x = 0; x < 25; x++) {
      if (tch->pcdata->imprint_type[x] > 0)
      fprintf(fp, "Imprinted %d %d %d %d %d %s~\n%s~\n",
      tch->pcdata->imprint_type[x],
      tch->pcdata->imprint_pressure_one[x],
      tch->pcdata->imprint_pressure_two[x],
      tch->pcdata->imprint_pressure_three[x],
      tch->pcdata->imprint_pending[x], tch->pcdata->imprint[x],
      tch->pcdata->imprint_trigger[x]);
    }
    if (tch->pcdata->branddate > 0)
    fprintf(fp, "Brand %d %d %s~\n%s~\n", tch->pcdata->branddate,
    tch->pcdata->brandlocation, tch->pcdata->brander,
    tch->pcdata->brandstring);

    if (tch->pcdata->brandcool > 0)
    fprintf(fp, "BrandCool %d\n", tch->pcdata->brandcool);

    if (safe_strlen(tch->pcdata->villain_praise) > 1)
    fprintf(fp, "Villain %d %s %s~\n", tch->pcdata->villain_type,
    tch->pcdata->villain_praise, tch->pcdata->villain_message);

    if (safe_strlen(tch->pcdata->last_bitten) > 1)
    fprintf(fp, "LastBitten %s~\n", tch->pcdata->last_bitten);

    if (tch->pcdata->doom_date > 1)
    {
      fprintf(fp, "Doom %d\n", tch->pcdata->doom_date);
      fprintf(fp, "DoomDesc %s~\n", tch->pcdata->doom_desc);
    }
    fprintf(fp, "DoomCustom %s~\n", tch->pcdata->doom_custom);
    for (x = 0; x < 25; x++) {
      if (tch->pcdata->memories[x] != NULL && safe_strlen(tch->pcdata->memories[x]) > 1)
      fprintf(fp, "Memory %s~\n", tch->pcdata->memories[x]);
      if (tch->pcdata->repressions[x] != NULL && safe_strlen(tch->pcdata->repressions[x]) > 1)
      fprintf(fp, "Repression %s~\n", tch->pcdata->repressions[x]);
    }
    if (tch->pcdata->finale_timer > 0) {
      fprintf(fp, "Finale %d %s~\n", tch->pcdata->finale_timer,
      tch->pcdata->finale_location);
    }
    if (tch->pcdata->last_develop_type > 0)
    fprintf(fp, "LastDevelop %d %d\n", tch->pcdata->last_develop_type,
    tch->pcdata->last_develop_time);

    if (tch->pcdata->rerollselftime > 0)
    fprintf(fp, "RerollSelf %d\n", tch->pcdata->rerollselftime);

    if (tch->pcdata->ill_count > 0)
    fprintf(fp, "IllCount %d\n", tch->pcdata->ill_count);

    if (tch->pcdata->ill_time > 0)
    fprintf(fp, "IllTime %d\n", tch->pcdata->ill_time);

    if (tch->pcdata->default_speed != 3)
    fprintf(fp, "DefaultSpeed %d\n", tch->pcdata->default_speed);

    if (tch->pcdata->offworld_protection > 0)
    fprintf(fp, "OffworldProtect %d\n", tch->pcdata->offworld_protection);
    if (tch->pcdata->guest_type > 0)
    fprintf(fp, "GuestType %d\n", tch->pcdata->guest_type);
    if (tch->pcdata->guest_tier > 0)
    fprintf(fp, "GuestTier %d\n", tch->pcdata->guest_tier);

    if (tch->pcdata->legendary_cool > current_time)
    fprintf(fp, "LegendaryCool %d\n", tch->pcdata->legendary_cool);

    if (tch->pcdata->wilds_fame_level > 0)
    fprintf(fp, "WildFame %d %s~\n", tch->pcdata->wilds_fame_level,
    tch->pcdata->wilds_fame);
    //    if(tch->pcdata->wilds_legendary > 0)
    fprintf(fp, "WildLegendary %d\n", tch->pcdata->wilds_legendary);

    if (tch->pcdata->other_fame_level > 0)
    fprintf(fp, "OtherFame %d %s~\n", tch->pcdata->other_fame_level,
    tch->pcdata->other_fame);
    if (tch->pcdata->other_legendary > 0)
    fprintf(fp, "OtherLegendary %d\n", tch->pcdata->other_legendary);

    if (tch->pcdata->godrealm_fame_level > 0)
    fprintf(fp, "GodrealmFame %d %s~\n", tch->pcdata->godrealm_fame_level,
    tch->pcdata->godrealm_fame);
    if (tch->pcdata->godrealm_legendary > 0)
    fprintf(fp, "GodrealmLegendary %d\n", tch->pcdata->godrealm_legendary);

    if (tch->pcdata->hell_fame_level > 0)
    fprintf(fp, "HellFame %d %s~\n", tch->pcdata->hell_fame_level,
    tch->pcdata->hell_fame);
    if (tch->pcdata->hell_legendary > 0)
    fprintf(fp, "HellLegendary %d\n", tch->pcdata->hell_legendary);

    for (x = 0; x < 4; x++) {
      if (tch->pcdata->mark_timer[x] > 0)
      fprintf(fp, "Mark %d %d %s~\n", x, tch->pcdata->mark_timer[x],
      tch->pcdata->mark[x]);
    }
    fprintf(fp, "NextHenchman %d\n", tch->pcdata->next_henchman);
    fprintf(fp, "NextEnemy %d\n", tch->pcdata->next_enemy);
    fprintf(fp, "NextMonster %d\n", tch->pcdata->next_monster);

    if (tch->pcdata->training_stat > 0)
    fprintf(fp, "TrainStat %d\n", tch->pcdata->training_stat);
    if (tch->pcdata->training_disc > 0)
    fprintf(fp, "TrainDisc %d\n", tch->pcdata->training_disc);

    for (x = 0; x < 25; x++) {
      if (str_cmp(tch->pcdata->customstrings[x][0], "")) {
        fprintf(
        fp, "Custom %d %d %d %s~\n%s~\n%s~\n%s~\n%s~\n%s~\n%s~\n", x,
        tch->pcdata->customstats[x][0], tch->pcdata->customstats[x][1],
        tch->pcdata->customstrings[x][0], tch->pcdata->customstrings[x][1],
        tch->pcdata->customstrings[x][2], tch->pcdata->customstrings[x][3],
        tch->pcdata->customstrings[x][4], tch->pcdata->customstrings[x][5],
        tch->pcdata->customstrings[x][6]);
      }
    }
    for (x = 0; x < 25; x++) {
      if (tch->pcdata->specials[x][0] != 0) {
        fprintf(fp, "Special %d %d %d\n", x, tch->pcdata->specials[x][0],
        tch->pcdata->specials[x][1]);
      }
    }
    for (x = 0; x < 20; x++) {
      if (tch->pcdata->shadow_attacks[x][0] > 0 || tch->pcdata->shadow_attacks[x][1] > 0) {
        fprintf(fp, "Shadow %d %d %d\n", x, tch->pcdata->shadow_attacks[x][0],
        tch->pcdata->shadow_attacks[x][1]);
      }
    }

    if (str_cmp(tch->pcdata->speaking, "English"))
    fprintf(fp, "Speaking %s~\n", tch->pcdata->speaking);

    if (tch->pcdata->spirit_type > 0)
    fprintf(fp, "SpiritType %d\n", tch->pcdata->spirit_type);

    if (tch->in_room != NULL)
    fprintf(fp, "Room %d\n", tch->in_room->vnum);
    else
    fprintf(fp, "Room 50\n");

    if (tch->pcdata->logoff_room != 0)
    fprintf(fp, "LogoffRoom %d\n", tch->pcdata->logoff_room);

    if (ch->trust != 0)
    fprintf(fp, "Tru  %d\n", ch->trust);

    fprintf(fp, "AbilityTree");
    for (x = 0; x < 5; x++)
    fprintf(fp, " %d", ch->pcdata->ability_trees[x]);
    fprintf(fp, "\n");

    fprintf(fp, "Stat");
    for (x = 0; x < SKILL_MAX; x++)
    fprintf(fp, " %d", ch->skills[x]);
    fprintf(fp, "\n");

    fprintf(fp, "WildsStat");
    for (x = 0; x < SKILL_MAX; x++)
    fprintf(fp, " %d", ch->wilds_skills[x]);
    fprintf(fp, "\n");

    fprintf(fp, "OtherStat");
    for (x = 0; x < SKILL_MAX; x++)
    fprintf(fp, " %d", ch->other_skills[x]);
    fprintf(fp, "\n");

    fprintf(fp, "GodrealmStat");
    for (x = 0; x < SKILL_MAX; x++)
    fprintf(fp, " %d", ch->godrealm_skills[x]);
    fprintf(fp, "\n");

    fprintf(fp, "HellStat");
    for (x = 0; x < SKILL_MAX; x++)
    fprintf(fp, " %d", ch->hell_skills[x]);
    fprintf(fp, "\n");

    fprintf(fp, "FOrders");
    for (x = 0; x < MAX_COVERS + 10; x++)
    fprintf(fp, " %d", ch->pcdata->focused_order[x]);
    fprintf(fp, "\n");

    fprintf(fp, "Disc");
    for (x = 0; x < MAX_DIS; x++)
    fprintf(fp, " %d", ch->disciplines[x]);
    fprintf(fp, "\n");

    fprintf(fp, "Sec  %d\n", ch->pcdata->security); /* OLC */

    fprintf(fp, "SelfEsteem %d\n", ch->pcdata->selfesteem);

    // Role Variables
    if (ch->pcdata->role > 0) {
      fprintf(fp, "Role %d\n", ch->pcdata->role);
    }

    /*
fprintf( fp, "Habits");
for(x=0;x<MAX_HABITS;x++)
fprintf(fp, " %d", ch->pcdata->habit[x]);
fprintf(fp, "\n");
*/

    fprintf(fp, "NewHabits");
    for (x = 0; x < MAX_NEWHABITS; x++)
    fprintf(fp, " %d", ch->pcdata->habit[x]);
    fprintf(fp, "\n");

    fprintf(fp, "Patrols");
    for (x = 0; x < 10; x++)
    fprintf(fp, " %d", ch->pcdata->patrol_habits[x]);
    fprintf(fp, "\n");

    fprintf(fp, "Gateways");
    for (x = 0; x < 50; x++)
    fprintf(fp, " %d", ch->pcdata->known_gateways[x]);
    fprintf(fp, "\n");

    for (x = 0; x < 300; x++) {
      if (ch->pcdata->stat_log_stat[x] > 0 && ch->pcdata->stat_log_stat[x] < SKILL_MAX && ch->pcdata->stat_log_cost[x] > 0)
      fprintf(fp, "StatLog %d %d %d %d %d %s~\n", ch->pcdata->stat_log_stat[x],
      ch->pcdata->stat_log_from[x], ch->pcdata->stat_log_to[x],
      ch->pcdata->stat_log_cost[x], ch->pcdata->stat_log_method[x],
      ch->pcdata->stat_log_string[x]);
    }

    fprintf(fp, "HMV  %d\n", ch->hit);
    fprintf(fp, "Act %s\n", set_print_flag(ch->act));

    fprintf(fp, "Legacy %s\n", set_print_flag(ch->legacy));

    if (ch->wiznet)
    fprintf(fp, "Wizn %s\n", print_flags(ch->wiznet));

    if (ch->invis_level)
    fprintf(fp, "Invi %d\n", ch->invis_level);
    if (ch->incog_level)
    fprintf(fp, "Inco %d\n", ch->incog_level);

    fprintf(fp, "Pos  %d\n",
    ch->position == POS_FIGHTING ? POS_STANDING : ch->position);

    if (ch->pcdata->home != 0)
    fprintf(fp, "Hom   %d\n", ch->pcdata->home);

    /*Birth details */
    fprintf(fp, "BDay %d\n", ch->pcdata->birth_day);
    fprintf(fp, "BMonth %d\n", ch->pcdata->birth_month);
    fprintf(fp, "BYear %d\n", ch->pcdata->birth_year);

    fprintf(fp, "SDay %d\n", ch->pcdata->sire_day);
    fprintf(fp, "SMonth %d\n", ch->pcdata->sire_month);
    fprintf(fp, "SYear %d\n", ch->pcdata->sire_year);
    fprintf(fp, "Aage %d\n", ch->pcdata->apparant_age);

    fprintf(fp, "Sleeping %d\n", ch->pcdata->sleeping);
    fprintf(fp, "CaptiveTimer %d\n", ch->pcdata->captive_timer);
    fprintf(fp, "LastPublic %d\n", ch->pcdata->last_public_room);

    fprintf(fp, "DescLock %d\n", ch->pcdata->desclock);

    fprintf(fp, "Ccolours %d\n", ch->pcdata->colours);
    fprintf(fp, "Cbandaids %d\n", ch->pcdata->bandaids);
    fprintf(fp, "Caura %s~\n", ch->pcdata->caura);
    fprintf(fp, "Cwalk %s~\n", ch->pcdata->cwalk);
    fprintf(fp, "Crank %s~\n", ch->pcdata->crank);

    fprintf(fp, "Sincooldown %d\n", ch->pcdata->sincool);

    fprintf(fp, "YouBadCounter %d\n", ch->pcdata->youbadcounter);

    if (ch->pcdata->resident > 0)
    fprintf(fp, "Resident %d\n", ch->pcdata->resident);

    if (ch->pcdata->last_page > 0)
    fprintf(fp, "LastPage %d\n", ch->pcdata->last_page);

    fprintf(fp, "LastHaven %d\n", ch->pcdata->last_inhaven);

    fprintf(fp, "Sins %d %d %d %d %d %d %d\n", ch->pcdata->sins[0],
    ch->pcdata->sins[1], ch->pcdata->sins[2], ch->pcdata->sins[3],
    ch->pcdata->sins[4], ch->pcdata->sins[5], ch->pcdata->sins[6]);

    fprintf(fp, "Dirt %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
    ch->pcdata->dirt[0], ch->pcdata->dirt[1], ch->pcdata->dirt[2],
    ch->pcdata->dirt[3], ch->pcdata->dirt[4], ch->pcdata->dirt[5],
    ch->pcdata->dirt[6], ch->pcdata->dirt[7], ch->pcdata->dirt[8],
    ch->pcdata->dirt[9], ch->pcdata->dirt[10], ch->pcdata->dirt[11],
    ch->pcdata->dirt[12], ch->pcdata->dirt[13], ch->pcdata->dirt[14],
    ch->pcdata->dirt[15], ch->pcdata->dirt[16], ch->pcdata->dirt[17]);

    fprintf(fp,
    "BloodAmount %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
    ch->pcdata->blood[0], ch->pcdata->blood[1], ch->pcdata->blood[2],
    ch->pcdata->blood[3], ch->pcdata->blood[4], ch->pcdata->blood[5],
    ch->pcdata->blood[6], ch->pcdata->blood[7], ch->pcdata->blood[8],
    ch->pcdata->blood[9], ch->pcdata->blood[10], ch->pcdata->blood[11],
    ch->pcdata->blood[12], ch->pcdata->blood[13], ch->pcdata->blood[14],
    ch->pcdata->blood[15], ch->pcdata->blood[16], ch->pcdata->blood[17]);

    fprintf(
    fp,
    "Attract %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
    ch->pcdata->attract[0], ch->pcdata->attract[1], ch->pcdata->attract[2],
    ch->pcdata->attract[3], ch->pcdata->attract[4], ch->pcdata->attract[5],
    ch->pcdata->attract[6], ch->pcdata->attract[7], ch->pcdata->attract[8],
    ch->pcdata->attract[9], ch->pcdata->attract[10], ch->pcdata->attract[11],
    ch->pcdata->attract[12], ch->pcdata->attract[13], ch->pcdata->attract[14],
    ch->pcdata->attract[15], ch->pcdata->attract[16], ch->pcdata->attract[17],
    ch->pcdata->attract[18], ch->pcdata->attract[19]);

    if (ch->pcdata->bamfin[0] != '\0')
    fprintf(fp, "Bin  %s~\n", ch->pcdata->bamfin);

    if (ch->pcdata->bamfout[0] != '\0')
    fprintf(fp, "Bout %s~\n", ch->pcdata->bamfout);

    vector<NoteBoard *> nbl = NoteBoard::getBoards();
    vector<NoteBoard *>::iterator nbit;
    for (nbit = nbl.begin(); nbit != nbl.end(); ++nbit) {
      fprintf(fp, "NoteLast %s %ld\n", (*nbit)->getName().c_str(),
      (*ch->pcdata->lastReadTimes)[(*nbit)->getName()]);
    }

    fprintf(fp, "Whot %s~\n", tch->pcdata->whotitle);
    fprintf(fp, "Titl %s~\n", tch->pcdata->title);

    // Write Club Info
    /*
*/
    fprintf(fp, "Firsts %d %d %d %d %d\n", ch->pcdata->firsts[0],
    ch->pcdata->firsts[1], ch->pcdata->firsts[2], ch->pcdata->firsts[3],
    ch->pcdata->firsts[4]);

    fprintf(fp, "Conditions %d %d %d %d %d %d %d %d %d %d\n",
    ch->pcdata->conditions[0], ch->pcdata->conditions[1],
    ch->pcdata->conditions[2], ch->pcdata->conditions[3],
    ch->pcdata->conditions[4], ch->pcdata->conditions[5],
    ch->pcdata->conditions[6], ch->pcdata->conditions[7],
    ch->pcdata->conditions[8], ch->pcdata->conditions[9]);

    fprintf(fp, "Emotes %d %d %d %d %d %d %d %d %d %d\n", ch->pcdata->emotes[0],
    ch->pcdata->emotes[1], ch->pcdata->emotes[2], ch->pcdata->emotes[3],
    ch->pcdata->emotes[4], ch->pcdata->emotes[5], ch->pcdata->emotes[6],
    ch->pcdata->emotes[7], ch->pcdata->emotes[8], ch->pcdata->emotes[9]);


    if (ch->pcdata->shadow_walk_room > 0)
    {
      fprintf(fp, "ShadowWalkRoom %d\n", ch->pcdata->shadow_walk_room);
      fprintf(fp, "ShadowWalkCooldown %d\n", ch->pcdata->shadow_walk_cooldown);

    }

    if(ch->pcdata->brand_timeout > 0)
    fprintf(fp, "BrandTimeout %d\n", ch->pcdata->brand_timeout);
    if(ch->pcdata->enthrall_timeout > 0)
    fprintf(fp, "EnthrallTimeout %d\n", ch->pcdata->enthrall_timeout);
    if(ch->pcdata->enrapture_timeout > 0)
    fprintf(fp, "EnraptureTimeout %d\n", ch->pcdata->enrapture_timeout);

    if(ch->pcdata->fixation_mourning > 0)
    {
      fprintf(fp, "FixationMourning %d\n", ch->pcdata->fixation_mourning);
    }
    if(ch->pcdata->building_fixation_level > 0)
    {
      fprintf(fp, "BuildingFixationLevel %d\n", ch->pcdata->building_fixation_level);
      fprintf(fp, "BuildingFixationName %s~\n", ch->pcdata->building_fixation_name);
    }
    if(ch->pcdata->fixation_timeout > 0)
    {
      fprintf(fp, "FixationTimeout %d\n", ch->pcdata->fixation_timeout);
      fprintf(fp, "FixationName %s~\n", ch->pcdata->fixation_name);
      fprintf(fp, "FixationLevel %d\n", ch->pcdata->fixation_level);
      fprintf(fp, "FixationCharges %d\n", ch->pcdata->fixation_charges);
    }
    if(ch->pcdata->prey_option > 0)
    {
      fprintf(fp, "PreyOption %d\n", ch->pcdata->prey_option);
      fprintf(fp, "PreyOptionCooldown %d\n", ch->pcdata->prey_option_cooldown);
      fprintf(fp, "PreyEmoteCooldown %d\n", ch->pcdata->prey_emote_cooldown);
    }
    if(ch->pcdata->villain_option > 0)
    {
      fprintf(fp, "VillainOption %d\n", ch->pcdata->villain_option);
      fprintf(fp, "VillainOptionCooldown %d\n", ch->pcdata->villain_option_cooldown);
      fprintf(fp, "VillainEmoteCooldown %d\n", ch->pcdata->villain_emote_cooldown);
    }
    if(ch->pcdata->patrol_status > 0)
    {
      fprintf(fp, "PatrolStatus %d\n", ch->pcdata->patrol_status);
      fprintf(fp, "PatrolTimer %d\n", ch->pcdata->patrol_timer);
      fprintf(fp, "PatrolAmount %d\n", ch->pcdata->patrol_amount);
      fprintf(fp, "PatrolPledged %d\n", ch->pcdata->patrol_pledged);
      fprintf(fp, "PatrolSubtype %d\n", ch->pcdata->patrol_subtype);
    }


    if (ch->pcdata->coma > 0)
    fprintf(fp, "Coma %d\n", ch->pcdata->coma);

    if (ch->pcdata->abomtime > 0)
    fprintf(fp, "AbomTime %d\n", ch->pcdata->abomtime);
    if (ch->pcdata->abomcuring > 0)
    fprintf(fp, "AbomCure %d\n", ch->pcdata->abomcuring);
    if (ch->pcdata->abominfecting > 0)
    fprintf(fp, "AbomInfect %d\n", ch->pcdata->abominfecting);

    fprintf(fp, "VictimizeHistory");
    for (x = 0; x < 20; x++)
    fprintf(fp, " %d", ch->pcdata->victimize_history[x]);
    fprintf(fp, "\n");
    if (ch->pcdata->victimize_difficult_time > current_time)
    fprintf(fp, "VictimizeDifficult %d\n",
    ch->pcdata->victimize_difficult_time);

    fprintf(fp, "NarrativeGive");
    for (x = 0; x < 50; x++)
    fprintf(fp, " %d", ch->pcdata->narrative_give[x]);
    fprintf(fp, "\n");
    fprintf(fp, "NarrativeWant");
    for (x = 0; x < 50; x++)
    fprintf(fp, " %d", ch->pcdata->narrative_want[x]);
    fprintf(fp, "\n");

    save_pc_color(ch, fp);

    /* write alias */
    for (pos = 0; pos < MAX_ALIAS; pos++) {
      if (ch->pcdata->alias[pos] == NULL || ch->pcdata->alias_sub[pos] == NULL)
      break;

      fprintf(fp, "Alias %s %s~\n", ch->pcdata->alias[pos],
      ch->pcdata->alias_sub[pos]);
    }

    // NameMaps
    int count = 0;
    for (NameMap::iterator it = ch->pcdata->male_names->begin();
    it != ch->pcdata->male_names->end() && count < 500; ++it) {
      if (it->first && it->second)
      fprintf(fp, "MaleNameMap %ld %s\n", it->first, it->second);
      count++;
    }
    count = 0;
    for (NameMap::iterator it = ch->pcdata->female_names->begin();
    it != ch->pcdata->female_names->end() && count < 500; ++it) {
      if (it->first && it->second)
      fprintf(fp, "FemaleNameMap %ld %s\n", it->first, it->second);
      count++;
    }

    for (paf = ch->affected; paf != NULL; paf = paf->next) {

      fprintf(fp, "Affc %3d %3d %3d %3d %3d %3d\n",

      paf->where, paf->level, paf->duration, paf->modifier, paf->location,
      paf->bitvector);
    }
    fprintf(fp, "End\n\n");
    return;
  }

  /*
* Write an object and its contents.
*/
  void fwrite_obj(OBJ_DATA *obj, FILE *fp, int iNest) {
    EXTRA_DESCR_DATA *ed;
    AFFECT_DATA *paf;

    /*
* Slick recursion to write lists backwards,
*   so loading them will load in forwards order.
*/
    if (obj->next_content != NULL)
    fwrite_obj(obj->next_content, fp, iNest);

    /*
* Castrate storage characters.
*/

    fprintf(fp, "#O\n");
    fprintf(fp, "Vnum %d\n", obj->pIndexData->vnum);

    fprintf(fp, "Nest %d\n", iNest);

    /* these data are only used if they do not match the defaults */

    fprintf(fp, "Name %s~\n", obj->name);
    fprintf(fp, "ShD  %s~\n", obj->short_descr);
    fprintf(fp, "Desc %s~\n", obj->description);
    fprintf(fp, "Adjust %s~\n", obj->adjust_string);
    fprintf(fp, "ExtF %d\n", obj->extra_flags);
    fprintf(fp, "WeaF %d\n", obj->wear_flags);
    fprintf(fp, "Ityp %d\n", obj->item_type);
    fprintf(fp, "Wt   %d\n", obj->size);
    if (obj->condition != obj->pIndexData->condition)
    fprintf(fp, "Cond %d\n", obj->condition);
    if (obj->material != obj->pIndexData->material)
    fprintf(fp, "Material %s~\n", obj->material);

    if (obj->stash_room != 0)
    fprintf(fp, "Stash %d\n", obj->stash_room);
    fprintf(fp, "WearString %s~\n", obj->wear_string);

    if (iNest == 0) {
      if (obj->carried_by == NULL && obj->in_room != NULL)
      fprintf(fp, "InRoom %d\n", obj->in_room->vnum);
    }

    if (obj->outfit[0] != 0 || obj->outfit[1] != 0 || obj->outfit[2] != 0 || obj->outfit[3] != 0 || obj->outfit[4] != 0 || obj->outfit[5] != 0 || obj->outfit[6] != 0 || obj->outfit[7] != 0 || obj->outfit[8] != 0 || obj->outfit[9] != 0) {
      fprintf(fp, "Outfit %d %d %d %d %d %d %d %d %d %d\n", obj->outfit[0],
      obj->outfit[1], obj->outfit[2], obj->outfit[3], obj->outfit[4],
      obj->outfit[5], obj->outfit[6], obj->outfit[7], obj->outfit[8],
      obj->outfit[9]);

      fprintf(fp, "OLocation %d %d %d %d %d %d %d %d %d %d\n",
      obj->outfit_location[0], obj->outfit_location[1],
      obj->outfit_location[2], obj->outfit_location[3],
      obj->outfit_location[4], obj->outfit_location[5],
      obj->outfit_location[6], obj->outfit_location[7],
      obj->outfit_location[8], obj->outfit_location[9]);
    }

    /* variable data */

    fprintf(fp, "Wear %d\n", obj->wear_loc);
    if (obj->level != obj->pIndexData->level)
    fprintf(fp, "Lev  %d\n", obj->level);
    if (obj->timer != 0)
    fprintf(fp, "Time %d\n", obj->timer);
    fprintf(fp, "Cost %d\n", obj->cost);
    fprintf(fp, "Val  %d %d %d %d %d %d\n", obj->value[0], obj->value[1],
    obj->value[2], obj->value[3], obj->value[4], obj->value[5]);

    fprintf(fp, "Rot %d\n", obj->rot_timer);
    fprintf(fp, "Buff %d\n", obj->buff);
    fprintf(fp, "Faction %d\n", obj->faction);

    switch (obj->item_type) {
    case ITEM_POTION:
      break;
    }
    for (paf = obj->affected; paf != NULL; paf = paf->next) {
      fprintf(fp, "Affc %3d %3d %3d %3d %3d %3d\n",

      paf->where, paf->level, paf->duration, paf->modifier, paf->location,
      paf->bitvector);
    }
    for (ed = obj->extra_descr; ed != NULL; ed = ed->next) {
      fprintf(fp, "ExDe %s~ %s~\n", ed->keyword, ed->description);
    }

    fprintf(fp, "End\n\n");

    if (obj->contains != NULL)
    fwrite_obj(obj->contains, fp, iNest + 1);

    return;
  }

  void fwrite_obj_2(OBJ_DATA *obj, FILE *fp, int iNest) {
    EXTRA_DESCR_DATA *ed;
    AFFECT_DATA *paf;

    /*
* Castrate storage characters.
*/

    fprintf(fp, "#O\n");
    fprintf(fp, "Vnum %d\n", obj->pIndexData->vnum);

    fprintf(fp, "Nest %d\n", iNest);

    /* these data are only used if they do not match the defaults */

    if (obj->name != obj->pIndexData->name)
    fprintf(fp, "Name %s~\n", obj->name);
    if (obj->short_descr != obj->pIndexData->short_descr)
    fprintf(fp, "ShD  %s~\n", obj->short_descr);
    if (obj->description != obj->pIndexData->description)
    fprintf(fp, "Desc %s~\n", obj->description);
    if (obj->description != obj->pIndexData->adjust_string) // Disco 9/5/2018
    fprintf(fp, "Adjust %s~\n", obj->adjust_string);
    if (obj->extra_flags != obj->pIndexData->extra_flags)
    fprintf(fp, "ExtF %d\n", obj->extra_flags);
    if (obj->wear_flags != obj->pIndexData->wear_flags)
    fprintf(fp, "WeaF %d\n", obj->wear_flags);
    if (obj->item_type != obj->pIndexData->item_type)
    fprintf(fp, "Ityp %d\n", obj->item_type);
    if (obj->size != obj->pIndexData->size)
    fprintf(fp, "Wt   %d\n", obj->size);
    if (obj->condition != obj->pIndexData->condition)
    fprintf(fp, "Cond %d\n", obj->condition);
    if (obj->material != obj->pIndexData->material)
    fprintf(fp, "Material %s~\n", obj->material);

    if (obj->stash_room != 0)
    fprintf(fp, "Stash %d\n", obj->stash_room);
    fprintf(fp, "WearString %s~\n", obj->wear_string);

    if (iNest == 0) {
      if (obj->carried_by == NULL && obj->in_room != NULL)
      fprintf(fp, "InRoom %d\n", obj->in_room->vnum);
    }

    if (obj->outfit[0] != 0 || obj->outfit[1] != 0 || obj->outfit[2] != 0 || obj->outfit[3] != 0 || obj->outfit[4] != 0 || obj->outfit[5] != 0 || obj->outfit[6] != 0 || obj->outfit[7] != 0 || obj->outfit[8] != 0 || obj->outfit[9] != 0) {
      fprintf(fp, "Outfit %d %d %d %d %d %d %d %d %d %d\n", obj->outfit[0],
      obj->outfit[1], obj->outfit[2], obj->outfit[3], obj->outfit[4],
      obj->outfit[5], obj->outfit[6], obj->outfit[7], obj->outfit[8],
      obj->outfit[9]);

      fprintf(fp, "OLocation %d %d %d %d %d %d %d %d %d %d\n",
      obj->outfit_location[0], obj->outfit_location[1],
      obj->outfit_location[2], obj->outfit_location[3],
      obj->outfit_location[4], obj->outfit_location[5],
      obj->outfit_location[6], obj->outfit_location[7],
      obj->outfit_location[8], obj->outfit_location[9]);
    }

    /* variable data */

    fprintf(fp, "Wear %d\n", obj->wear_loc);
    if (obj->level != obj->pIndexData->level)
    fprintf(fp, "Lev  %d\n", obj->level);
    if (obj->timer != 0)
    fprintf(fp, "Time %d\n", obj->timer);
    fprintf(fp, "Cost %d\n", obj->cost);
    fprintf(fp, "Val  %d %d %d %d %d %d\n", obj->value[0], obj->value[1],
    obj->value[2], obj->value[3], obj->value[4], obj->value[5]);

    fprintf(fp, "Rot %d\n", obj->rot_timer);
    fprintf(fp, "Buff %d\n", obj->buff);
    fprintf(fp, "Faction %d\n", obj->faction);

    switch (obj->item_type) {
    case ITEM_POTION:
      break;
    }
    for (paf = obj->affected; paf != NULL; paf = paf->next) {
      fprintf(fp, "Affc %3d %3d %3d %3d %3d %3d\n",

      paf->where, paf->level, paf->duration, paf->modifier, paf->location,
      paf->bitvector);
    }
    for (ed = obj->extra_descr; ed != NULL; ed = ed->next) {
      fprintf(fp, "ExDe %s~ %s~\n", ed->keyword, ed->description);
    }

    fprintf(fp, "End\n\n");

    if (obj->contains != NULL)
    fwrite_obj(obj->contains, fp, iNest + 1);

    return;
  }

  /*
* Load a char and inventory into a new ch structure.
*/
  bool load_char_obj(DESCRIPTOR_DATA *d, char *name) {

    if (!str_cmp(name, "")) {
      bug("Load_char_obj: no name.", 0);
      return FALSE;
    }

    THING th;
    char strsave[MAX_INPUT_LENGTH];
    char buf[100];
    CHAR_DATA *ch;
    FILE *fp;
    bool found;
    // int stat;

    ch = new_char();
    ch->pcdata = new_pcdata();

    d->character = ch;
    ch->desc = d;
    ch->name = str_dup(name);
    ch->short_descr = str_dup(name); /* NEW */
    ch->long_descr = str_dup(name);  /* NEW */
    ch->id = get_pc_id();

    /* Get the "thing" type set */
    th.thing_type = THING_CH;
    th.thing.ch = ch;

    found = FALSE;

#if defined(unix)
    /* decompress if .gz file exists */
    sprintf(strsave, "%s%s%s", PLAYER_DIR, capitalize(name), ".gz");
    if ((fp = fopen(strsave, "r")) != NULL) {
      fclose(fp);
      sprintf(buf, "gzip -dfq %s", strsave);
      system(buf);
    }
#endif

    sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(name));
    if ((fp = fopen(strsave, "r")) != NULL) {

      int iNest;

      for (iNest = 0; iNest < MAX_NEST; iNest++)
      rgObjNest[iNest] = NULL;

      found = TRUE;
      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          char buf[MSL];
          sprintf(buf, "Load_char_obj: # not found, letter: %c, name: %s", letter,
          name);
          bug(buf, 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "PLAYER"))
        fread_char(ch, fp);
        else if (!str_cmp(word, "OBJECT"))
        fread_obj(th, fp);
        else if (!str_cmp(word, "O"))
        fread_obj(th, fp);
        else if (!str_cmp(word, "END"))
        break;
        else {
          bug("Load_char_obj: bad section.", 0);
          break;
        }
      }
      fclose(fp);
    }
    else {
      bug("Load_char_obj: Couldn't open.", 0);
      bug(strsave, 0);
    }

    /* initialize race */
    if (found && ch->level > 0) {

      //  ch->affected_by = ch->affected_by|subrace_table[ch->sub_race].aff;
    }
    return found;
  }

  /*
* Read in a char.
*/
  /*
#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                    \
if ( !str_cmp( word, literal ) )    \
{                   \
field  = value;         \
fMatch = TRUE;          \
break;              \
}
*/

  /* provided to free strings */
#if defined(KEYS)
#undef KEYS
#endif

#define KEYS(literal, field, value)                                            \
    if (!str_cmp(word, literal)) {                                               \
      free_string(field);                                                        \
      field = value;                                                             \
      fMatch = TRUE;                                                             \
      break;                                                                     \
    }

  void fread_char(CHAR_DATA *ch, FILE *fp) {
    char buf[MAX_STRING_LENGTH];
    const char *word;
    bool fMatch;
    int count = 0;
    //    int lastlogoff = current_time;
    int i;

    sprintf(buf, "Loading %s.", ch->name);
    log_string(buf);

    for (i = 0; i < 2000; i++) {
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;

      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'A':
        KEY("Atolerance", ch->pcdata->atolerance, fread_number(fp));
        KEY("Atime", ch->activeat, fread_number(fp));
        KEY("Aage", ch->pcdata->apparant_age, fread_number(fp));
        KEYS("ArtFirst", ch->pcdata->art1, fread_string(fp));
        KEYS("ArtSecond", ch->pcdata->art2, fread_string(fp));
        KEYS("ArtThird", ch->pcdata->art3, fread_string(fp));
        KEY("AttractBase", ch->pcdata->attract_base, fread_number(fp));
        KEY("AttractMod", ch->pcdata->attract_mod, fread_number(fp));
        KEY("AuntFloCalled", ch->pcdata->auntflo_called, fread_number(fp));
        KEY("Association", ch->pcdata->association, fread_number(fp));
        KEY("AdventureFatigue", ch->pcdata->adventure_fatigue, fread_number(fp));
        KEY("Abomination", ch->abomination, fread_number(fp));
        KEY("AbomTime", ch->pcdata->abomtime, fread_number(fp));
        KEY("AbomCure", ch->pcdata->abomcuring, fread_number(fp));
        KEY("AbomInfect", ch->pcdata->abominfecting, fread_number(fp));
        KEY("AbomMistCool", ch->pcdata->abommistcool, fread_number(fp));
        KEY("AbomBlackCool", ch->pcdata->abomblackcool, fread_number(fp));
        KEY("Aexp", ch->pcdata->aexp, fread_number(fp));
        KEY("Awins", ch->pcdata->awins, fread_number(fp));
        KEY("Alosses", ch->pcdata->alosses, fread_number(fp));
        KEY("Astatus", ch->pcdata->astatus, fread_number(fp));
        KEY("AccountName", ch->pcdata->account_name, fread_string(fp));
        KEY("Apower", ch->pcdata->apower, fread_number(fp));
        KEY("Availability", ch->pcdata->availability, fread_number(fp));
        KEY("ActiveChatroom", ch->pcdata->active_chatroom, fread_number(fp));

        if (!str_cmp(word, "Account")) {
          fMatch = TRUE;
          ch->pcdata->account = reload_account(fread_string(fp));
        }
        if(!str_cmp(word, "AttractCount")){
          fMatch = TRUE;
          ch->pcdata->attract_count = (float)(fread_number(fp))/100;
        }
        if(!str_cmp(word, "AttractCFashion")){
          fMatch = TRUE;
          ch->pcdata->attract_count_fashion = (float)(fread_number(fp))/100;
        }
        if(!str_cmp(word, "AttractCSkin")){
          fMatch = TRUE;
          ch->pcdata->attract_count_skin = (float)(fread_number(fp))/100;
        }
        if(!str_cmp(word, "AttractCPhys")){
          fMatch = TRUE;
          ch->pcdata->attract_count_phys = (float)(fread_number(fp))/100;
        }



        if (!str_cmp(word, "AnimalForm")) {
          int pos = fread_number(fp);
          ch->pcdata->animal_weights[pos] = fread_number(fp);
          ch->pcdata->animal_genus[pos] = fread_number(fp);
          free_string(ch->pcdata->animal_names[pos]);
          ch->pcdata->animal_names[pos] = str_dup(fread_string(fp));
          free_string(ch->pcdata->animal_intros[pos]);
          ch->pcdata->animal_intros[pos] = str_dup(fread_string(fp));
          free_string(ch->pcdata->animal_species[pos]);
          ch->pcdata->animal_species[pos] = str_dup(fread_string(fp));
          free_string(ch->pcdata->animal_descs[pos]);
          ch->pcdata->animal_descs[pos] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }
        if(!str_cmp(word, "AnimalChange")) {
          int pos = fread_number(fp);
          free_string(ch->pcdata->animal_change_to[pos]);
          ch->pcdata->animal_change_to[pos] = str_dup(fread_string(fp));
          free_string(ch->pcdata->animal_change_from[pos]);
          ch->pcdata->animal_change_from[pos] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "AnimalStats")) {
          int pos = fread_number(fp);
          for (int x = 0; x < 25; x++) {
            ch->pcdata->animal_stats[pos][x] = fread_number(fp);
          }
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Act")) {
          fMatch = TRUE;
          SET_INIT(ch->act);
          set_fread_flag(fp, ch->act);
          if (ch->version == 5 && ch->level < 20) {
            ch->version++;
          }
          else if (ch->version == 5)
          ch->version++;
        }

        if (!str_cmp(word, "AugDisc")) {
          int i = fread_number(fp);
          ch->pcdata->augdisc_timer[i] = fread_number(fp);
          ch->pcdata->augdisc_disc[i] = fread_number(fp);
          ch->pcdata->augdisc_level[i] = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "AugSkill")) {
          int i = fread_number(fp);
          ch->pcdata->augskill_timer[i] = fread_number(fp);
          ch->pcdata->augskill_skill[i] = fread_number(fp);
          ch->pcdata->augskill_level[i] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Attract")) {
          int stat;

          for (stat = 0; stat < 20; stat++)
          ch->pcdata->attract[stat] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "AffectedBy") || !str_cmp(word, "AfBy")) {

          fMatch = TRUE;
          SET_INIT(ch->affected_by);
          set_fread_flag(fp, ch->affected_by);
          break;
        }

        if (!str_cmp(word, "Alia")) {
          if (count >= MAX_ALIAS) {
            fread_to_eol(fp);
            fMatch = TRUE;
            break;
          }

          ch->pcdata->alias[count] = str_dup(fread_word(fp));
          ch->pcdata->alias_sub[count] = str_dup(fread_word(fp));
          count++;
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "AbilityTree")) {
          int stat;
          for (stat = 0; stat < 5; stat++)
          ch->pcdata->ability_trees[stat] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Alias")) {
          if (count >= MAX_ALIAS) {
            fread_to_eol(fp);
            fMatch = TRUE;
            break;
          }

          ch->pcdata->alias[count] = str_dup(fread_word(fp));
          ch->pcdata->alias_sub[count] = fread_string(fp);
          count++;
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Affc")) {
          AFFECT_DATA *paf;

          paf = new_affect();

          paf->where = fread_number(fp);
          paf->level = fread_number(fp);
          paf->duration = fread_number(fp);
          if (paf->duration < 0)
          paf->duration = 1;
          paf->modifier = fread_number(fp);
          paf->location = fread_number(fp);
          paf->bitvector = fread_number(fp);

          paf->weave = FALSE;

          paf->type = 0;

          paf->held = FALSE;
          paf->caster = NULL;
          paf->inverted = FALSE;
          paf->next = ch->affected;
          ch->affected = paf;
          fMatch = TRUE;
          break;
        }

        break;

      case 'B':
        KEYS("Bamfin", ch->pcdata->bamfin, fread_string(fp));

        KEYS("Bamfout", ch->pcdata->bamfout, fread_string(fp));
        KEY("BecameTyrant", ch->pcdata->became_tyrant, fread_number(fp));
        KEY("BDay", ch->pcdata->birth_day, fread_number(fp));
        KEYS("Bin", ch->pcdata->bamfin, fread_string(fp));
        KEYS("Bout", ch->pcdata->bamfout, fread_string(fp));
        KEY("BalMoney", ch->pcdata->total_money, fread_number(fp));
        KEY("BalCredit", ch->pcdata->total_credit, fread_number(fp));
        KEY("BrandTimeout", ch->pcdata->brand_timeout, fread_number(fp));
        KEY("BuildingFixationLevel", ch->pcdata->building_fixation_level,
        fread_number(fp));
        KEYS("BuildingFixationName", ch->pcdata->building_fixation_name,
        fread_string(fp));
        KEY("BadAbomination", ch->pcdata->bad_abomination, fread_number(fp));
        KEY("Bust", ch->pcdata->bust, fread_number(fp));
        KEY("BMonth", ch->pcdata->birth_month, fread_number(fp));
        KEY("BYear", ch->pcdata->birth_year, fread_number(fp));
        KEY("BonusXP", ch->pcdata->xpbonus, fread_number(fp));
        KEY("BonusRPXP", ch->pcdata->rpxpbonus, fread_number(fp));
        KEY("Beacon", ch->pcdata->beacon, fread_number(fp));
        KEY("BloodAura", ch->pcdata->bloodaura, fread_number(fp));
        KEY("BanishedTime", ch->pcdata->timebanished, fread_number(fp));
        KEY("BloodPotency", ch->pcdata->blood_potency, fread_number(fp));
        KEY("BloodTicks", ch->pcdata->blood_ticks, fread_number(fp));
        KEY("BondDrop", ch->pcdata->bond_drop, fread_number(fp));
        KEY("BonusOrigin", ch->pcdata->bonus_origin, fread_number(fp));
        KEY("BloodDate", ch->pcdata->blood_date, fread_number(fp));
        KEY("BloodLevel", ch->pcdata->blood_level, fread_number(fp));
        KEYS("BrainwashLoyalty", ch->pcdata->brainwash_loyalty, fread_string(fp));
        KEYS("BrainwashReidentity", ch->pcdata->brainwash_reidentity,
        fread_string(fp));
        KEY("Boobjob", ch->pcdata->boobjob, fread_number(fp));
        KEY("BrandCool", ch->pcdata->brandcool, fread_number(fp));
        if (!str_cmp(word, "BlackEyes")) {
          ch->pcdata->blackeyes = fread_number(fp);
          ch->pcdata->blackeyes_power = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Boon")) {
          ch->pcdata->boon = fread_number(fp);
          ch->pcdata->boon_timeout = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "BoonHistory")) {
          int pos = fread_number(fp);
          ch->pcdata->boon_history[pos] = fread_number(fp);
          fMatch = TRUE;
          break;
        }


        if (!str_cmp(word, "Brand")) {
          ch->pcdata->branddate = fread_number(fp);
          ch->pcdata->brandlocation = fread_number(fp);
          free_string(ch->pcdata->brander);
          ch->pcdata->brander = str_dup(fread_string(fp));
          free_string(ch->pcdata->brandstring);
          ch->pcdata->brandstring = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Breach")) {
          ch->pcdata->breach_origin = fread_number(fp);
          ch->pcdata->breach_destination = fread_number(fp);
          ch->pcdata->breach_timer = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Bonds")) {
          int stat;
          char *temp;
          stat = fread_number(fp);
          temp = fread_string(fp);
          free_string(ch->pcdata->bonds[stat]);
          ch->pcdata->bonds[stat] = str_dup(temp);

          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "BloodAmount")) {
          int stat;

          for (stat = 0; stat < MAX_COVERS; stat++)
          ch->pcdata->blood[stat] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        break;

      case 'C':
        KEY("CLocation", ch->pcdata->creation_location, fread_number(fp));
        KEY("CreateDate", ch->pcdata->create_date, fread_number(fp));
        KEY("CopCool", ch->pcdata->cop_cooldown, fread_number(fp));
        KEY("Ccolours", ch->pcdata->colours, fread_number(fp));
        KEY("Cbandaids", ch->pcdata->bandaids, fread_number(fp));
        KEY("CaptiveTimer", ch->pcdata->captive_timer, fread_number(fp));
        KEYS("CharGoals", ch->pcdata->char_goals, fread_string(fp));
        KEYS("CharFears", ch->pcdata->char_fears, fread_string(fp));
        KEYS("CharSecrets", ch->pcdata->char_secrets, fread_string(fp));
        KEYS("CharTimeline", ch->pcdata->char_timeline, fread_string(fp));
        KEYS("Classified", ch->pcdata->classified, fread_string(fp));
        /*
KEYS( "ChatHistory", ch->pcdata->chat_history,      fread_string( fp )
);
*/
        KEYS("ChatHandle", ch->pcdata->chat_handle, fread_string(fp));
        KEYS("Caura", ch->pcdata->caura, fread_string(fp));
        KEYS("Cwalk", ch->pcdata->cwalk, fread_string(fp));
        KEYS("Crank", ch->pcdata->crank, fread_string(fp));
        KEYS("ClFame", ch->pcdata->class_fame, fread_string(fp));
        KEY("ClassNeutral", ch->pcdata->class_neutral, fread_number(fp));
        KEY("Coma", ch->pcdata->coma, fread_number(fp));
        KEY("CliqueRole", ch->pcdata->clique_role, fread_number(fp));
        KEY("CliqueRoleChange", ch->pcdata->clique_rolechange, fread_number(fp));

        // Statistic counters
        KEY("CountDreamsex", ch->pcdata->count_dreamsex, fread_number(fp));

        if (!str_cmp(word, "Curse")) {
          ch->pcdata->curse = fread_number(fp);
          ch->pcdata->curse_timeout = fread_number(fp);
          free_string(ch->pcdata->curse_text);
          ch->pcdata->curse_text = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "CurseHistory")) {
          int pos = fread_number(fp);
          ch->pcdata->curse_history[pos] = fread_number(fp);
          fMatch = TRUE;
          break;
        }


        if (!str_cmp(word, "CharNotes")) {
          char *temporary = str_dup(fread_string(fp));
          free_string(temporary);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Conditions")) {
          ch->pcdata->conditions[0] = fread_number(fp);
          ch->pcdata->conditions[1] = fread_number(fp);
          ch->pcdata->conditions[2] = fread_number(fp);
          ch->pcdata->conditions[3] = fread_number(fp);
          ch->pcdata->conditions[4] = fread_number(fp);
          ch->pcdata->conditions[5] = fread_number(fp);
          ch->pcdata->conditions[6] = fread_number(fp);
          ch->pcdata->conditions[7] = fread_number(fp);
          ch->pcdata->conditions[8] = fread_number(fp);
          ch->pcdata->conditions[9] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Class")) {
          ch->pcdata->class_type = fread_number(fp);
          ch->pcdata->class_faction = fread_number(fp);
          ch->pcdata->class_cooldown = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "ClassOld")) {
          ch->pcdata->class_oldfaction = fread_number(fp);
          ch->pcdata->class_oldlevel = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Classpoints")) {
          ch->pcdata->classpoints = fread_number(fp);
          ch->pcdata->classtotal = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Contact")) {
          int pos = fread_number(fp);
          ch->pcdata->contact_jobs[pos] = fread_number(fp);
          ch->pcdata->contact_cooldowns[pos] = fread_number(fp);
          free_string(ch->pcdata->contact_names[pos]);
          ch->pcdata->contact_names[pos] = str_dup(fread_string(fp));
          free_string(ch->pcdata->contact_descs[pos]);
          ch->pcdata->contact_descs[pos] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "CommChans")) {
          int i;
          char *temp;
          for (i = 0; i < 10 && ch->pcdata->chan_numbers[i] != 0; i++) {
          }
          ch->pcdata->chan_status[i] = fread_number(fp);
          ch->pcdata->chan_numbers[i] = fread_number(fp);

          temp = fread_word(fp);
          free_string(ch->pcdata->chan_names[i]);
          ch->pcdata->chan_names[i] = str_dup(temp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "ConditionStory")) {
          for (int i = 0; i < 10; i++) {
            if (ch->pcdata->storycon[i][0] == 0) {
              ch->pcdata->storycon[i][0] = fread_number(fp);
              ch->pcdata->storycon[i][1] = fread_number(fp);
              ch->pcdata->storycon[i][2] = fread_number(fp);
              i = 10;
              break;
            }
          }
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "CDisc")) {
          ch->pcdata->cdisc_range = fread_number(fp);
          ch->pcdata->cdisc_name = fread_string(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Custom")) {
          int i = fread_number(fp);
          ch->pcdata->customstats[i][0] = fread_number(fp);
          ch->pcdata->customstats[i][1] = fread_number(fp);
          for (int j = 0; j < 7; j++) {
            free_string(ch->pcdata->customstrings[i][j]);
            ch->pcdata->customstrings[i][j] = str_dup(fread_string(fp));
          }
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "ChatroomNew")) {
          for (int x = 0; x < 50; x++)
          ch->pcdata->in_chatroom[x] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Colora")) {
          LOAD_COLOR(text)
          LOAD_COLOR(auction)
          LOAD_COLOR(gossip)
          LOAD_COLOR(irl)
          LOAD_COLOR(immortal)
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Colorb")) {
          LOAD_COLOR(implementor)
          LOAD_COLOR(newbie)
          LOAD_COLOR(gtell)
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Colorc")) {
          LOAD_COLOR(info)
          LOAD_COLOR(tells)
          LOAD_COLOR(reply)
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Colord")) {
          LOAD_COLOR(room_exits)
          LOAD_COLOR(room_things)
          LOAD_COLOR(prompt)
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Colore")) {
          LOAD_COLOR(fight_death)
          LOAD_COLOR(fight_yhit)
          LOAD_COLOR(fight_ohit)
          LOAD_COLOR(fight_thit)
          LOAD_COLOR(fight_skill)
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Colorf")) {
          LOAD_COLOR(wiznet)
          LOAD_COLOR(say)
          LOAD_COLOR(ooc)
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Colorg")) {
          LOAD_COLOR(hint)
          LOAD_COLOR(minioncolor)
          LOAD_COLOR(pray)
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Colorz")) {
          LOAD_COLOR(yells)
          LOAD_COLOR(osay)
          LOAD_COLOR(hero)
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Comm")) {
          SET_INIT(ch->comm);
          set_fread_flag(fp, ch->comm);

          fMatch = TRUE;
          break;
        }

        break;

      case 'D':
        KEY("DomainTimer", ch->pcdata->domain_timer, fread_number(fp));
        KEY("DailyUpkeep", ch->pcdata->daily_upkeep, fread_number(fp));
        KEYS("Description", ch->description, fread_string(fp));
        KEYS("Desc", ch->description, fread_string(fp));
        KEY("Donated", ch->donated, fread_number(fp));
        KEY("DescLock", ch->pcdata->desclock, fread_number(fp));
        KEY("DTime", ch->death_timer, fread_number(fp));
        KEY("Difficulty", ch->pcdata->difficulty, fread_number(fp));
        KEY("Dtrains", ch->pcdata->dtrains, fread_number(fp));
        KEY("DetentionTime", ch->pcdata->detention_time, fread_number(fp));
        KEY("DefaultSpeed", ch->pcdata->default_speed, fread_number(fp));
        KEYS("DreamIntro", ch->pcdata->dream_intro, fread_string(fp));
        KEYS("DreamIdentity", ch->pcdata->dream_identity, fread_string(fp));
        KEY("DreamIdentityTimer", ch->pcdata->dream_identity_timer,
        fread_number(fp));
        KEYS("DreamDesc", ch->pcdata->dream_description, fread_string(fp));
        KEYS("DreamOrigin", ch->pcdata->dream_origin, fread_string(fp));
        KEYS("DeludedReason", ch->pcdata->deluded_reason, fread_string(fp));
        KEY("DeludedCost", ch->pcdata->deluded_cost, fread_number(fp));
        KEY("DeludedDuration", ch->pcdata->deluded_duration, fread_number(fp));
        KEY("DecayStage", ch->pcdata->decay_stage, fread_number(fp));
        KEY("DecayTimer", ch->pcdata->decay_timer, fread_number(fp));
        KEY("DecayMaxTimer", ch->pcdata->max_decay_timer, fread_number(fp));
        KEY("DueDate", ch->pcdata->due_date, fread_number(fp));
        KEY("Diminished", ch->diminished, fread_number(fp));
        KEY("DiminishLT", ch->diminish_lt, fread_number(fp));
        KEY("DiminishTill", ch->diminish_till, fread_number(fp));
        KEY("DiminishVis", ch->diminish_vis, fread_number(fp));
        KEY("DivineFocus", ch->pcdata->divine_focus, fread_number(fp));
        KEY("DreamRoom", ch->pcdata->dream_room, fread_number(fp));
        KEY("DreamSex", ch->pcdata->dream_sex, fread_number(fp));
        KEY("Dexp", ch->pcdata->dexp, fread_number(fp));
        KEY("DemonPact", ch->pcdata->demon_pact, fread_string(fp));
        KEY("DreamOutfit", ch->pcdata->dreamoutfit, fread_number(fp));
        KEY("DestinyFeature", ch->pcdata->destiny_feature, fread_number(fp));
        KEY("DestinyFeatureRoom", ch->pcdata->destiny_feature_room,
        fread_number(fp));
        KEY("DreamFromOutfit", ch->pcdata->dreamfromoutfit, fread_number(fp));
        KEY("Doom", ch->pcdata->doom_date, fread_number(fp));
        KEY("DoomDesc", ch->pcdata->doom_desc, fread_string(fp));
        KEY("DoomCustom", ch->pcdata->doom_custom, fread_string(fp));
        KEY("DeployCore", ch->deploy_core, fread_number(fp));
        KEY("DeployCult", ch->deploy_cult, fread_number(fp));
        KEY("DeploySect", ch->deploy_sect, fread_number(fp));


        if (!str_cmp(word, "Disc")) {
          int stat;
          for (stat = 0; stat < MAX_DIS; stat++)
          ch->disciplines[stat] = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "DestinyOne")) {
          int point = 9;
          for (int x = 9; x >= 0; x--) {
            if (ch->pcdata->destiny_in_one[x] == 0)
            point = x;
          }
          ch->pcdata->destiny_in_one[point] = fread_number(fp);
          ch->pcdata->destiny_stage_one[point] = fread_number(fp);
          ch->pcdata->destiny_in_with_one[point] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "DestinyTwo")) {
          int point = 9;
          for (int x = 9; x >= 0; x--) {
            if (ch->pcdata->destiny_in_two[x] == 0)
            point = x;
          }
          ch->pcdata->destiny_in_two[point] = fread_number(fp);
          ch->pcdata->destiny_stage_two[point] = fread_number(fp);
          ch->pcdata->destiny_in_with_two[point] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "DestinyInterestOne")) {
          for (int x = 0; x < 50; x++)
          ch->pcdata->destiny_interest_one[x] = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "DestinyInterestTwo")) {
          for (int x = 0; x < 50; x++)
          ch->pcdata->destiny_interest_two[x] = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "DestinyBlockOne")) {
          for (int x = 0; x < 50; x++)
          ch->pcdata->destiny_block_one[x] = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "DestinyBlockTwo")) {
          for (int x = 0; x < 50; x++)
          ch->pcdata->destiny_block_two[x] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "DestinyLockout")) {
          int point = 9;
          for (int x = 9; x >= 0; x--) {
            if (ch->pcdata->destiny_lockout_time[x] == 0)
            point = x;
          }
          ch->pcdata->destiny_lockout_time[point] = fread_number(fp);
          free_string(ch->pcdata->destiny_lockout_char[point]);
          ch->pcdata->destiny_lockout_char[point] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Drive")) {
          int pos = fread_number(fp);
          ch->pcdata->driveloc[pos] = fread_number(fp);
          free_string(ch->pcdata->drivenames[pos]);
          ch->pcdata->drivenames[pos] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Disspending")) {
          free_string(ch->pcdata->diss_target);
          ch->pcdata->diss_target = str_dup(fread_word(fp));
          free_string(ch->pcdata->diss_message);
          ch->pcdata->diss_message = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "DetailOver")) {
          int i = fread_number(fp);
          free_string(ch->pcdata->detail_over[i]);
          ch->pcdata->detail_over[i] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "DetailUnder")) {
          int i = fread_number(fp);
          free_string(ch->pcdata->detail_under[i]);
          ch->pcdata->detail_under[i] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Dirt")) {
          int stat;

          for (stat = 0; stat < MAX_COVERS; stat++)
          ch->pcdata->dirt[stat] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        break;

      case 'E':
        KEY("Exp", ch->exp, fread_number(fp));
        KEY("ExpSpent", ch->spentexp, fread_number(fp));
        KEY("EidilonOf", ch->pcdata->eidilon_of, fread_number(fp));
        KEY("EidilonAmbient", ch->pcdata->eidilon_ambient, fread_string(fp));
        KEY("ExpNew", ch->newexp, fread_number(fp));
        KEY("ExpNewSpent", ch->spentnewexp, fread_number(fp));
        KEY("Esteem", ch->esteem_faction, fread_number(fp));
        KEY("EsteemCult", ch->esteem_cult, fread_number(fp));
        KEY("EsteemSect", ch->esteem_sect, fread_number(fp));
        KEY("EnthrallTimeout", ch->pcdata->enthrall_timeout, fread_number(fp));
        KEY("EnraptureTimeout", ch->pcdata->enrapture_timeout,
        fread_number(fp));
        KEY("Estrogen", ch->pcdata->estrogen, fread_number(fp));
        KEYS("Eyes", ch->pcdata->eyes, fread_string(fp));
        KEYS("Email", ch->pcdata->email, fread_string(fp));
        KEY("ExpCap", ch->pcdata->exp_cap, fread_number(fp));
        KEY("ExtraFocus", ch->pcdata->extra_focus, fread_number(fp));
        KEY("ExperimentUses", ch->pcdata->experiment_uses, fread_number(fp));
        KEYS("Enthralling", ch->pcdata->enthralling, fread_string(fp));
        KEYS("Enrapturing", ch->pcdata->enrapturing, fread_string(fp));
        KEYS("Enthralled", ch->pcdata->enthralled, fread_string(fp));
        KEYS("Enraptured", ch->pcdata->enraptured, fread_string(fp));
        KEYS("Enlinked", ch->pcdata->enlinked, fread_string(fp));
        KEY("Enlinktime", ch->pcdata->enlinktime, fread_number(fp));
        KEY("ExtraPowers", ch->pcdata->extra_powers, fread_number(fp));
        KEY("EscapeTimer", ch->pcdata->escape_timer, fread_number(fp));
        KEY("EncounterNumber", ch->pcdata->encounter_number, fread_number(fp));
        KEY("EncounterRoom", ch->pcdata->encounter_orig_room, fread_number(fp));

        if (!str_cmp(word, "Egg")) {
          ch->pcdata->egg_daddy = fread_number(fp);
          ch->pcdata->egg_date = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Emotes")) {
          ch->pcdata->emotes[0] = fread_number(fp);
          ch->pcdata->emotes[1] = fread_number(fp);
          ch->pcdata->emotes[2] = fread_number(fp);
          ch->pcdata->emotes[3] = fread_number(fp);
          ch->pcdata->emotes[4] = fread_number(fp);
          ch->pcdata->emotes[5] = fread_number(fp);
          ch->pcdata->emotes[6] = fread_number(fp);
          ch->pcdata->emotes[7] = fread_number(fp);
          ch->pcdata->emotes[8] = fread_number(fp);
          ch->pcdata->emotes[9] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "End")) {
          /* adjust hp move up  -- here for speed's sake */
          int minoffline = (current_time / 60 - ch->lastlogoff / 60);

          if (ch->pcdata->captive_timer > 0)
          ch->pcdata->captive_timer += minoffline * 6;

          if (minoffline >= 240) {
            if (IS_FLAG(ch->comm, COMM_VBLOOD))
            REMOVE_FLAG(ch->comm, COMM_VBLOOD);
          }

          if (ch->pcdata->desclock < 0)
          ch->pcdata->desclock = 0;

          return;
        }
        break;

      case 'F':
        KEYS("Fame", ch->pcdata->fame, fread_string(fp));
        KEY("Facing", ch->facing, fread_number(fp));
        KEY("Faction", ch->faction, fread_number(fp));
        KEY("FactionTwo", ch->factiontwo, fread_number(fp));
        KEY("FCore", ch->fcore, fread_number(fp));
        KEY("FCult", ch->fcult, fread_number(fp));
        KEY("FSect", ch->fsect, fread_number(fp));
        KEY("FactionTrue", ch->factiontrue, fread_number(fp));
        KEY("Fatigue", ch->pcdata->fatigue, fread_number(fp));
        KEYS("Feedback", ch->pcdata->feedback, fread_string(fp));
        KEY("Feedbackcool", ch->pcdata->feedbackcooldown, fread_number(fp));
        KEYS("File", ch->pcdata->file, fread_string(fp));
        KEY("FemaleDevelopment", ch->pcdata->femaledevelopment, fread_number(fp));
        KEY("Fertility", ch->pcdata->natural_fertility, fread_number(fp));
        KEY("FinalDeathDate", ch->pcdata->final_death_date, fread_number(fp));
        KEY("FreeRitual", ch->pcdata->free_ritual, fread_number(fp));
        KEY("Fleshformed", ch->pcdata->fleshformed, fread_number(fp));
        KEY("FormChangeDate", ch->pcdata->form_change_date, fread_number(fp));
        KEYS("FFKnowledge", ch->pcdata->ff_knowledge, fread_string(fp));
        KEYS("FFSecrets", ch->pcdata->ff_secret, fread_string(fp));
        KEY("FixationTimeout", ch->pcdata->fixation_timeout, fread_number(fp));
        KEY("FixationMourning", ch->pcdata->fixation_mourning, fread_number(fp));
        KEY("FixationCharges", ch->pcdata->fixation_charges, fread_number(fp));
        KEY("FixationLevel", ch->pcdata->fixation_level, fread_number(fp));
        KEYS("FixationName", ch->pcdata->fixation_name, fread_string(fp));

        if (!str_cmp(word, "Finale")) {
          ch->pcdata->finale_timer = fread_number(fp);
          free_string(ch->pcdata->finale_location);
          ch->pcdata->finale_location = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Firsts")) {
          ch->pcdata->firsts[0] = fread_number(fp);
          ch->pcdata->firsts[1] = fread_number(fp);
          ch->pcdata->firsts[2] = fread_number(fp);
          ch->pcdata->firsts[3] = fread_number(fp);
          ch->pcdata->firsts[4] = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "FullTime")) {
          ch->pcdata->partpay_timer = fread_number(fp);
          ch->pcdata->fulltime_timer = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "FemaleNameMap")) {
          long id = fread_number(fp);
          char *temp = fread_word(fp);
          add_intro(ch, id, SEX_FEMALE, temp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "FFile")) {
          int i;
          for (i = 0; i < 50 && ch->pcdata->file_factions[i] != 0; i++) {
          }
          ch->pcdata->file_factions[i] = fread_number(fp);
          free_string(ch->pcdata->file_message[i]);
          ch->pcdata->file_message[i] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Feedbackincoming")) {
          int i;
          char *temp;
          for (i = 0; i < 10 && ch->pcdata->feedbacktim[i] != 0; i++) {
          }
          ch->pcdata->feedbackbon[i] = fread_number(fp);
          ch->pcdata->feedbacktim[i] = fread_number(fp);
          temp = fread_string(fp);
          free_string(ch->pcdata->feedbackinc[i]);
          ch->pcdata->feedbackinc[i] = str_dup(temp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Ftim")) {
          free_string(ch->pcdata->ftime);
          ch->pcdata->ftime = fread_string(fp);
          sprintf(buf, "%s", ch->pcdata->ftime);
          free_string(ch->pcdata->ftime);
          ch->pcdata->ftime = str_dup(buf);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Floc")) {
          free_string(ch->pcdata->floc);
          ch->pcdata->floc = fread_string(fp);
          sprintf(buf, "%s", ch->pcdata->floc);
          free_string(ch->pcdata->floc);
          ch->pcdata->floc = str_dup(buf);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Fdcr")) {
          free_string(ch->pcdata->fdesc);
          ch->pcdata->fdesc = fread_string(fp);
          sprintf(buf, "%s", ch->pcdata->fdesc);
          free_string(ch->pcdata->fdesc);
          ch->pcdata->fdesc = str_dup(buf);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "FDesc")) {
          int i = fread_number(fp);
          free_string(ch->pcdata->focused_descs[i]);
          ch->pcdata->focused_descs[i] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "FOrders")) {
          int stat;
          for (stat = 0; stat < MAX_COVERS + 10; stat++)
          ch->pcdata->focused_order[stat] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        break;

      case 'G':
        KEY("GhostBanishment", ch->pcdata->ghost_banishment, fread_number(fp));
        KEY("GhostPool", ch->pcdata->ghost_pool, fread_number(fp));
        KEY("GhostRoom", ch->pcdata->ghost_room, fread_number(fp));
        KEYS("GuestOf", ch->pcdata->guest_of, fread_string(fp));
        KEY("GuestType", ch->pcdata->guest_type, fread_number(fp));
        KEY("GuestTier", ch->pcdata->guest_tier, fread_number(fp));
        KEY("Gexp", ch->pcdata->gexp, fread_number(fp));
        KEY("Genesis", ch->pcdata->genesis_stage, fread_number(fp));
        KEY("GodrealmLegendary", ch->pcdata->godrealm_legendary,
        fread_number(fp));

        if (!str_cmp(word, "GodrealmFame")) {
          ch->pcdata->godrealm_fame_level = fread_number(fp);
          free_string(ch->pcdata->godrealm_fame);
          ch->pcdata->godrealm_fame = fread_string(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "GodrealmStat")) {
          int stat;
          for (stat = 0; stat < SKILL_MAX; stat++)
          ch->godrealm_skills[stat] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Gateways")) {
          int stat;
          for (stat = 0; stat < 50; stat++)
          ch->pcdata->known_gateways[stat] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Guarded")) {
          ch->pcdata->guard_number = fread_number(fp);
          ch->pcdata->guard_faction = fread_number(fp);
          ch->pcdata->guard_expiration = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Genes")) {
          ch->pcdata->eyes_genetic = fread_number(fp);
          ch->pcdata->hair_genetic = fread_number(fp);
          free_string(ch->pcdata->hair_dyed);
          ch->pcdata->hair_dyed = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Garage")) {
          int pos = fread_number(fp);
          ch->pcdata->garage_typeone[pos] = fread_number(fp);
          ch->pcdata->garage_typetwo[pos] = fread_number(fp);
          ch->pcdata->garage_status[pos] = fread_number(fp);
          ch->pcdata->garage_timer[pos] = fread_number(fp);
          ch->pcdata->garage_location[pos] = fread_number(fp);
          ch->pcdata->garage_cost[pos] = fread_number(fp);
          free_string(ch->pcdata->garage_name[pos]);
          ch->pcdata->garage_name[pos] = str_dup(fread_string(fp));
          free_string(ch->pcdata->garage_desc[pos]);
          ch->pcdata->garage_desc[pos] = str_dup(fread_string(fp));
          free_string(ch->pcdata->garage_lplate[pos]);
          ch->pcdata->garage_lplate[pos] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }

        break;

      case 'H':
        if (!str_cmp(word, "Hist")) {
          free_string(ch->pcdata->history);
          ch->pcdata->history = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }

        KEY("Hinch", ch->pcdata->height_inches, fread_number(fp));
        KEY("Hfeet", ch->pcdata->height_feet, fread_number(fp));
        KEY("HTime", ch->heal_timer, fread_number(fp));
        KEY("Hom", ch->pcdata->home, fread_number(fp));
        KEYS("Hair", ch->pcdata->hair, fread_string(fp));
        KEYS("HomeTerritory", ch->pcdata->home_territory, fread_string(fp));
        KEY("Healthtimer", ch->pcdata->healthtimer, fread_number(fp));
        KEY("Heroic", ch->pcdata->heroic, fread_number(fp));
        KEY("Hexp", ch->pcdata->hexp, fread_number(fp));
        KEY("HellLegendary", ch->pcdata->hell_legendary, fread_number(fp));
        KEY("HymenLost", ch->pcdata->hymen_lost, fread_number(fp));
        KEY("HPSex", ch->pcdata->hp_sex, fread_number(fp));

        if (!str_cmp(word, "HellFame")) {
          ch->pcdata->hell_fame_level = fread_number(fp);
          free_string(ch->pcdata->hell_fame);
          ch->pcdata->hell_fame = fread_string(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "HellStat")) {
          int stat;
          for (stat = 0; stat < SKILL_MAX; stat++)
          ch->hell_skills[stat] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Habits")) {
          int stat;
          for (stat = 0; stat < MAX_HABITS; stat++)
          ch->pcdata->habit[stat] = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Hangouts")) {
          ch->pcdata->hangouttemp = fread_number(fp);
          ch->pcdata->hangoutone = fread_number(fp);
          ch->pcdata->hangouttwo = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "HMV")) {
          ch->hit = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Haunt")) {
          ch->pcdata->haunt_timer = fread_number(fp);
          ch->pcdata->haunter = fread_string(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "HeldBlood")) {
          int stat;

          for (stat = 0; stat < 6; stat++)
          ch->pcdata->heldblood[stat] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        break;

      case 'I':
        KEY("InDomain", ch->pcdata->in_domain, fread_number(fp));
        KEY("Intel", ch->pcdata->intel, fread_number(fp));
        KEY("Id", ch->id, fread_number(fp));
        KEY("Impregnated", ch->pcdata->impregnated, fread_number(fp));
        KEY("ImpregnatedType", ch->pcdata->impregnated_type, fread_number(fp));
        KEY("ImpregnatedDaddyID", ch->pcdata->impregnated_daddy_ID,
        fread_number(fp));
        KEY("Inseminated", ch->pcdata->inseminated, fread_number(fp));
        KEY("InseminatedType", ch->pcdata->inseminated_type, fread_number(fp));
        KEY("InseminatedDaddyID", ch->pcdata->inseminated_daddy_ID,
        fread_number(fp));
        KEY("InvisLevel", ch->invis_level, fread_number(fp));
        KEYS("ImplantDream", ch->pcdata->implant_dream, fread_string(fp));
        KEY("Inco", ch->incog_level, fread_number(fp));
        KEY("Invi", ch->invis_level, fread_number(fp));
        KEY("InBase", ch->pcdata->base_standing, fread_number(fp));
        KEY("InPraise", ch->pcdata->social_praise, fread_number(fp));
        KEY("InBehave", ch->pcdata->social_behave, fread_number(fp));
        KEY("InBalance", ch->pcdata->influence_balance, fread_number(fp));
        KEY("Influence", ch->pcdata->influence, fread_number(fp));
        KEYS("IdentityWorld", ch->pcdata->identity_world, fread_string(fp));
        KEY("InScheme", ch->pcdata->scheme_influence, fread_number(fp));
        KEY("InFaction", ch->pcdata->faction_influence, fread_number(fp));
        KEY("InSuper", ch->pcdata->super_influence, fread_number(fp));
        KEY("InBank", ch->pcdata->influence_bank, fread_number(fp));
        KEY("Infused", ch->pcdata->infuse_date, fread_number(fp));
        KEYS("Intro", ch->pcdata->intro_desc, fread_string(fp));
        KEY("IllCount", ch->pcdata->ill_count, fread_number(fp));
        KEY("IllTime", ch->pcdata->ill_time, fread_number(fp));
        KEY("InfluencerBank", ch->pcdata->influencer_bank, fread_number(fp));
        KEY("Implant", ch->pcdata->implant_frequency, fread_number(fp));
        KEY("InstituteContrabandViolations",ch->pcdata->institute_contraband_violations, fread_number(fp));
        if (!str_cmp(word, "IgnoredCharacter")) {
          int i;
          char *temp;
          for (i = 0; i < 50 && str_cmp(ch->pcdata->ignored_characters[i], "");
          i++) {
          }
          temp = fread_word(fp);
          free_string(ch->pcdata->ignored_characters[i]);
          ch->pcdata->ignored_characters[i] = str_dup(temp);
          // free_string(temp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "IgnoredAccount")) {
          int i;
          char *temp;
          for (i = 0; i < 50 && str_cmp(ch->pcdata->ignored_accounts[i], "");
          i++) {
          }
          temp = fread_word(fp);
          free_string(ch->pcdata->ignored_accounts[i]);
          ch->pcdata->ignored_accounts[i] = str_dup(temp);
          // free_string(temp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Imprinted")) {
          int ref = 0;
          for (int i = 0; i < 25; i++) {
            if (ch->pcdata->imprint_type[i] == 0) {
              ref = i;
              continue;
            }
          }
          ch->pcdata->imprint_type[ref] = fread_number(fp);
          ch->pcdata->imprint_pressure_one[ref] = fread_number(fp);
          ch->pcdata->imprint_pressure_two[ref] = fread_number(fp);
          ch->pcdata->imprint_pressure_three[ref] = fread_number(fp);
          ch->pcdata->imprint_pending[ref] = fread_number(fp);
          free_string(ch->pcdata->imprint[ref]);
          ch->pcdata->imprint[ref] = str_dup(fread_string(fp));
          free_string(ch->pcdata->imprint_trigger[ref]);
          ch->pcdata->imprint_trigger[ref] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }
        break;

      case 'J':
        KEY("Jtlag", ch->pcdata->jetlag, fread_number(fp));

        if (!str_cmp(word, "JobOne")) {
          ch->pcdata->job_type_one = fread_number(fp);
          ch->pcdata->job_room_one = fread_number(fp);
          ch->pcdata->job_delay_one = fread_number(fp);
          free_string(ch->pcdata->job_title_one);
          ch->pcdata->job_title_one = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "JobTwo")) {
          ch->pcdata->job_type_two = fread_number(fp);
          ch->pcdata->job_room_two = fread_number(fp);
          ch->pcdata->job_delay_two = fread_number(fp);
          free_string(ch->pcdata->job_title_two);
          ch->pcdata->job_title_two = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }

        break;
      case 'K':
        KEY("Karma", ch->karma, fread_number(fp));
        KEY("KarmaEarned", ch->pcdata->earned_karma, fread_number(fp));
        KEY("KarmaBattery", ch->pcdata->karma_battery, fread_number(fp));
        KEY("KarmaSpent", ch->spentkarma, fread_number(fp));
        KEY("KarmaPersonal", ch->pkarma, fread_number(fp));
        KEY("KarmaPersonalSpent", ch->spentpkarma, fread_number(fp));
        break;

      case 'L':
        KEY("LastIntel", ch->pcdata->last_intel, fread_number(fp));
        KEYS("Lname", ch->pcdata->last_name, fread_string(fp));
        KEYS("LastIP", ch->last_ip, fread_string(fp));
        KEY("LastSex", ch->pcdata->last_sex, fread_number(fp));
        KEY("LastSextype", ch->pcdata->last_sextype, fread_number(fp));
        KEY("LastShower", ch->pcdata->last_shower, fread_number(fp));
        KEYS("LastVillainMod", ch->pcdata->last_villain_mod, fread_string(fp));
        KEY("LastVictimBled", ch->pcdata->last_victim_bled, fread_number(fp));
        KEY("LastImbue", ch->pcdata->last_imbue, fread_number(fp));
        KEY("LifeForce", ch->lifeforce, fread_number(fp));
        KEY("LifeForceCooldown", ch->pcdata->lifeforcecooldown, fread_number(fp));
        KEY("LFCount", ch->pcdata->lfcount, fread_number(fp));
        KEY("LFTotal", ch->pcdata->lftotal, fread_number(fp));
        KEY("LFMod", ch->pcdata->lf_modifier, fread_number(fp));
        KEY("LFUsed", ch->lf_used, fread_number(fp));
        KEY("LFTaken", ch->lf_taken, fread_number(fp));
        KEY("LFSUsed", ch->lf_sused, fread_number(fp));
        KEY("LastWar", ch->pcdata->lastwarcheck, fread_number(fp));
        KEY("LastShare", ch->pcdata->last_share, fread_number(fp));
        KEY("LastPublic", ch->pcdata->last_public_room, fread_number(fp));
        KEY("LimitAlly", ch->pcdata->ally_limit, fread_number(fp));
        KEY("LimitMinion", ch->pcdata->minion_limit, fread_number(fp));
        KEY("Level", ch->level, fread_number(fp));
        KEY("Lev", ch->level, fread_number(fp));
        KEY("Levl", ch->level, fread_number(fp));
        KEY("LuckyAbomination", ch->pcdata->lucky_abomination, fread_number(fp));
        KEY("LogO", ch->lastlogoff, fread_number(fp));
        KEY("LPaid", ch->pcdata->last_paid, fread_number(fp));
        KEY("LastMoon", ch->pcdata->last_outside_fullmoon, fread_number(fp));
        KEY("LastDrained", ch->pcdata->last_drained_person, fread_number(fp));
        KEY("Locationx", ch->x, fread_number(fp));
        KEY("Locationy", ch->y, fread_number(fp));
        KEYS("LongDescr", ch->long_descr, fread_string(fp));
        KEYS("LnD", ch->long_descr, fread_string(fp));
        KEY("LastAccident", ch->pcdata->lastaccident, fread_number(fp));
        KEY("LastNormal", ch->pcdata->lastnormal, fread_number(fp));
        KEY("LastIdentity", ch->pcdata->lastidentity, fread_number(fp));
        KEY("LastNotAlone", ch->pcdata->lastnotalone, fread_number(fp));
        KEY("LastShift", ch->pcdata->lastshift, fread_number(fp));
        KEY("Lure", ch->pcdata->lured_room, fread_number(fp));
        KEY("LunacyCurse", ch->pcdata->lunacy_curse, fread_number(fp));
        KEY("LegendaryCool", ch->pcdata->legendary_cool, fread_number(fp));
        KEY("LastRumor", ch->pcdata->last_rumor, fread_string(fp));
        KEY("LastBitten", ch->pcdata->last_bitten, fread_string(fp));
        KEY("LastHaven", ch->pcdata->last_inhaven, fread_number(fp));
        KEY("LastPage", ch->pcdata->last_page, fread_number(fp));
        KEY("LogoffRoom", ch->pcdata->logoff_room, fread_number(fp));
        KEY("LingeringSanc", ch->pcdata->lingering_sanc, fread_number(fp));
        KEY("LastRPRoom", ch->pcdata->last_rp_room, fread_number(fp));
        KEY("LastTrueSexedID", ch->pcdata->last_true_sexed_ID,
        fread_number(fp));
        if (!str_cmp(word, "Legacy")) {

          set_fread_flag(fp, ch->legacy);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "LastNewSexed")) {
          int i = fread_number(fp);
          free_string(ch->pcdata->last_sexed[i]);
          ch->pcdata->last_sexed[i] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "LastSexedID")) {
          int stat;
          for (stat = 0; stat < 3; stat++) {
            ch->pcdata->last_sexed_ID[stat] = fread_number(fp);
          }
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "LastPraiseMessage")) {
          int i = fread_number(fp);
          free_string(ch->pcdata->last_praise_message[i]);
          ch->pcdata->last_praise_message[i] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "LastDissMessage")) {
          int i = fread_number(fp);
          free_string(ch->pcdata->last_diss_message[i]);
          ch->pcdata->last_diss_message[i] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "LastPraised")) {
          int i = fread_number(fp);
          free_string(ch->pcdata->last_praised[i]);
          ch->pcdata->last_praised[i] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "LastDissed")) {
          int i = fread_number(fp);
          free_string(ch->pcdata->last_dissed[i]);
          ch->pcdata->last_dissed[i] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "LastDevelop")) {
          ch->pcdata->last_develop_type = fread_number(fp);
          ch->pcdata->last_develop_time = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "LifeTracker")) {
          for (int i = 0; i < 40; i++)
          ch->pcdata->life_tracker[i] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Language")) {
          int i = fread_number(fp);
          free_string(ch->pcdata->languages[i]);
          ch->pcdata->languages[i] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }
        break;
      case 'M':
        KEY("MaleDevelopment", ch->pcdata->maledevelopment, fread_number(fp));
        KEY("ManualUpkeep", ch->pcdata->manual_upkeep, fread_number(fp));
        KEY("Menstruation", ch->pcdata->menstruation, fread_number(fp));
        KEYS("Messages", ch->pcdata->messages, fread_string(fp));
        KEYS("Mermaiddesc", ch->pcdata->mermaiddesc, fread_string(fp));
        KEYS("Mermaidintro", ch->pcdata->mermaidintro, fread_string(fp));
        KEYS("MermaidChangeTo", ch->pcdata->mermaid_change_to, fread_string(fp));
        KEYS("MermaidChangeFrom", ch->pcdata->mermaid_change_from, fread_string(fp));
        KEYS("MakeupLight", ch->pcdata->makeup_light, fread_string(fp));
        KEYS("MakeupMedium", ch->pcdata->makeup_medium, fread_string(fp));
        KEYS("MakeupHeavy", ch->pcdata->makeup_heavy, fread_string(fp));
        KEY("Money", ch->money, fread_number(fp));
        KEY("MissedChatConnections", ch->pcdata->missed_chat_connections, fread_number(fp));
        KEY("MissedRpConnections", ch->pcdata->missed_rp_connections, fread_number(fp));
        KEYS("Mentor", ch->pcdata->mentor, fread_string(fp));
        KEY("Mimic", ch->pcdata->mimic, fread_number(fp));
        KEY("MedicUses", ch->pcdata->medic_uses, fread_number(fp));
        KEY("MaskedNumber", ch->pcdata->maskednumber, fread_number(fp));
        KEYS("MaskIntroOne", ch->pcdata->mask_intro_one, fread_string(fp));
        KEYS("MaskIntroTwo", ch->pcdata->mask_intro_two, fread_string(fp));
        KEY("MindMessed", ch->pcdata->mindmessed, fread_number(fp));
        KEY("Modifier", ch->modifier, fread_number(fp));
        KEY("Maim", ch->pcdata->maim, fread_string(fp));
        KEY("MindBroken", ch->pcdata->mindbroken, fread_number(fp));
        KEY("MonsterFed", ch->pcdata->monster_fed, fread_number(fp));
        KEY("MurderName", ch->pcdata->murder_name, fread_string(fp));
        KEY("MurderTimer", ch->pcdata->murder_timer, fread_number(fp));
        KEY("Market", ch->pcdata->market_visit, fread_number(fp));
        KEY("MurderCooldown", ch->pcdata->murder_cooldown, fread_number(fp));
        KEY("MaintainedRitual", ch->pcdata->maintained_ritual, fread_number(fp));
        KEY("MaintainCost", ch->pcdata->maintain_cost, fread_number(fp));

        if (!str_cmp(word, "MaleNameMap")) {
          long id = fread_number(fp);
          char *temp = fread_word(fp);
          add_intro(ch, id, SEX_MALE, temp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Memory")) {
          int ref = 0;
          for (int i = 0; i < 25; i++) {
            if (ch->pcdata->memories[i] == NULL || safe_strlen(ch->pcdata->memories[i]) < 1) {
              ref = i;
              continue;
            }
          }
          free_string(ch->pcdata->memories[ref]);
          ch->pcdata->memories[ref] = str_dup(fread_string(fp));
          fMatch = TRUE;
        }
        if (!str_cmp(word, "Mark")) {
          int ref = 0;
          ref = fread_number(fp);
          ch->pcdata->mark_timer[ref] = fread_number(fp);
          free_string(ch->pcdata->mark[ref]);
          ch->pcdata->mark[ref] = str_dup(fread_string(fp));

          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Monstername")) {
          int i = fread_number(fp);
          char *temp;
          temp = fread_string(fp);
          free_string(ch->pcdata->monster_names[0][i]);
          ch->pcdata->monster_names[0][i] = str_dup(temp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Monstershort")) {
          int i = fread_number(fp);
          char *temp;
          temp = fread_string(fp);
          free_string(ch->pcdata->monster_names[1][i]);
          ch->pcdata->monster_names[1][i] = str_dup(temp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Monsterdesc")) {
          int i = fread_number(fp);
          char *temp;
          temp = fread_string(fp);
          free_string(ch->pcdata->monster_names[2][i]);
          ch->pcdata->monster_names[2][i] = str_dup(temp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Monsterdisciplines")) {
          int i = fread_number(fp);
          ch->pcdata->monster_discs[0][i] = fread_number(fp);
          ch->pcdata->monster_discs[1][i] = fread_number(fp);
          ch->pcdata->monster_discs[2][i] = fread_number(fp);
          ch->pcdata->monster_discs[3][i] = fread_number(fp);
          ch->pcdata->monster_discs[4][i] = fread_number(fp);
          ch->pcdata->monster_discs[5][i] = fread_number(fp);
          ch->pcdata->monster_discs[6][i] = fread_number(fp);
          ch->pcdata->monster_discs[7][i] = fread_number(fp);
          ch->pcdata->monster_discs[8][i] = fread_number(fp);
          ch->pcdata->monster_discs[9][i] = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        break;

      case 'N':
        KEYS("Name", ch->name, fread_string(fp));
        KEYS("Nightmare", ch->pcdata->nightmare, fread_string(fp));
        KEY("Neutralized", ch->pcdata->neutralized, fread_number(fp));
        KEY("NextHenchman", ch->pcdata->next_henchman, fread_number(fp));
        KEY("NextEnemy", ch->pcdata->next_enemy, fread_number(fp));
        KEY("NextMonster", ch->pcdata->next_monster, fread_number(fp));
        KEY("NightmareShifted", ch->pcdata->nightmare_shifted, fread_number(fp));
        KEY("NightMaredragged", ch->pcdata->nightmare_dragged, fread_number(fp));
        KEY("NoWhereFemale", ch->pcdata->nowhere_female, fread_number(fp));
        KEY("NoWhereMale", ch->pcdata->nowhere_male, fread_number(fp));

        if (!str_cmp(word, "NewHabits")) {
          int stat;
          for (stat = 0; stat < MAX_NEWHABITS; stat++) {
            ch->pcdata->habit[stat] = fread_number(fp);
          }
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "NarrativeWant")) {
          int stat;
          for (stat = 0; stat < 50; stat++)
          ch->pcdata->narrative_want[stat] = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "NarrativeGive")) {
          int stat;
          for (stat = 0; stat < 50; stat++)
          ch->pcdata->narrative_give[stat] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "NoWhereCharacter")) {
          int i;
          char *temp;
          for (i = 0; i < 50 && str_cmp(ch->pcdata->nowhere_characters[i], "");
          i++) {
          }
          temp = fread_word(fp);
          free_string(ch->pcdata->nowhere_characters[i]);
          ch->pcdata->nowhere_characters[i] = str_dup(temp);
          // free_string(temp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "NoWhereAccount")) {
          int i;
          char *temp;
          for (i = 0; i < 50 && str_cmp(ch->pcdata->nowhere_accounts[i], "");
          i++) {
          }
          temp = fread_word(fp);
          free_string(ch->pcdata->nowhere_accounts[i]);
          ch->pcdata->nowhere_accounts[i] = str_dup(temp);
          // free_string(temp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "NoteLast")) {
          string temp(fread_word(fp));
          (*ch->pcdata->lastReadTimes)[temp] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        break;

      case 'O':
        KEY("Ovulation", ch->pcdata->ovulation, fread_number(fp));
        KEY("OffworldProtect", ch->pcdata->offworld_protection, fread_number(fp));
        KEY("Oppress", ch->oppress, fread_number(fp));
        KEY("Overworked", ch->pcdata->overworked, fread_number(fp));
        KEY("Oexp", ch->pcdata->oexp, fread_number(fp));
        KEY("OtherLegendary", ch->pcdata->other_legendary, fread_number(fp));
        KEY("OperativeCreationType", ch->pcdata->operative_creation_type,
        fread_number(fp));
        KEYS("OperativeCore", ch->pcdata->operative_core, fread_string(fp));
        KEYS("OperativeSect", ch->pcdata->operative_sect, fread_string(fp));
        KEYS("OperativeCult", ch->pcdata->operative_cult, fread_string(fp));

        if (!str_cmp(word, "OtherFame")) {
          ch->pcdata->other_fame_level = fread_number(fp);
          free_string(ch->pcdata->other_fame);
          ch->pcdata->other_fame = fread_string(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "OtherStat")) {
          int stat;
          for (stat = 0; stat < SKILL_MAX; stat++)
          ch->other_skills[stat] = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "OutNick")) {
          int pos = fread_number(fp);
          ch->pcdata->outfit_nickpoint[pos] = fread_number(fp);
          free_string(ch->pcdata->outfit_nicknames[pos]);
          ch->pcdata->outfit_nicknames[pos] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Order")) {
          ch->pcdata->order_timer = fread_number(fp);
          ch->pcdata->order_type = fread_number(fp);
          ch->pcdata->order_amount = fread_number(fp);
          ch->pcdata->order_target = fread_string(fp);
          fMatch = TRUE;
          break;
        }
        break;

      case 'P':
        KEYS("Photos", ch->pcdata->photo_msgs, fread_string(fp));
        KEY("PendingResources", ch->pcdata->pending_resources, fread_number(fp));
        KEYS("Password", ch->pcdata->pwd, fread_string(fp));
        KEYS("Pass", ch->pcdata->pwd, fread_string(fp));
        KEYS("Pledge", ch->pcdata->pledge, fread_string(fp));
        KEY("PledgeStealth", ch->pcdata->pledge_stealth, fread_number(fp));
        KEY("Payscale", ch->pcdata->payscale, fread_number(fp));
        KEY("Portal", ch->pcdata->portaled_from, fread_number(fp));
        KEY("Penis", ch->pcdata->penis, fread_number(fp));
        KEY("PowerHell", ch->pcdata->hell_power, fread_number(fp));
        KEY("PowerGodrealm", ch->pcdata->godrealm_power, fread_number(fp));
        KEY("PowerOther", ch->pcdata->other_power, fread_number(fp));
        KEY("PsychicAttempts", ch->pcdata->psychic_attempts, fread_number(fp));
        KEY("PsychicProof", ch->pcdata->psychic_proof, fread_number(fp));
        KEY("PoliceTimer", ch->pcdata->police_timer, fread_number(fp));
        KEY("PoliceIntensity", ch->pcdata->police_intensity, fread_number(fp));
        KEY("PoliceNumber", ch->pcdata->police_number, fread_number(fp));
        KEY("PrivateSecurity", ch->pcdata->private_security, fread_string(fp));
        KEY("Played", ch->played, fread_number(fp));
        KEY("Plyd", ch->played, fread_number(fp));
        KEY("PlydCopFree", ch->playedcopfree, fread_number(fp));
        KEY("Position", ch->position, fread_number(fp));
        KEY("Pos", ch->position, fread_number(fp));
        KEY("Pnlty", ch->pcdata->penalty, fread_number(fp));
        KEYS("Prompt", ch->prompt, fread_string(fp));
        KEY("Prom", ch->prompt, fread_string(fp));
        KEYS("PHair", ch->pcdata->pubic_hair, fread_string(fp));
        KEYS("Place", ch->pcdata->place, fread_string(fp));
        KEY("PrivateRPExp", ch->privaterpexp, fread_number(fp));
        KEY("PrivateKarma", ch->privatekarma, fread_number(fp));
        KEY("PrisonCare", ch->pcdata->prison_care, fread_number(fp));
        KEY("PrisonMult", ch->pcdata->prison_mult, fread_number(fp));
        KEY("PrisonEmotes", ch->pcdata->prison_emotes, fread_number(fp));
        KEY("PreyOption", ch->pcdata->prey_option, fread_number(fp));
        KEY("PreyOptionCooldown", ch->pcdata->prey_option_cooldown,
        fread_number(fp));
        KEY("PreyEmoteCooldown", ch->pcdata->prey_emote_cooldown,
        fread_number(fp));
        KEY("PatrolStatus", ch->pcdata->patrol_status, fread_number(fp));
        KEY("PatrolTimer", ch->pcdata->patrol_timer, fread_number(fp));
        KEY("PatrolAmount", ch->pcdata->patrol_amount, fread_number(fp));
        KEY("PatrolPledged", ch->pcdata->patrol_pledged, fread_number(fp));
        KEY("PatrolSubtype", ch->pcdata->patrol_subtype, fread_number(fp));


        if (!str_cmp(word, "PreviousFocus")) {
          int stat;
          for (stat = 0; stat < 25; stat++) {
            if (ch->pcdata->previous_focus_vnum[stat] == 0) {
              ch->pcdata->previous_focus_vnum[stat] = fread_number(fp);
              ch->pcdata->previous_focus_level[stat] = fread_number(fp);
              stat = 25;
            }
          }
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Patrols")) {
          int stat;
          for (stat = 0; stat < 10; stat++)
          ch->pcdata->patrol_habits[stat] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Paths")) {
          int stat;
          int count = 0;
          for (stat = 0; stat < MAX_PATHS; stat++)
          count += fread_number(fp);
          ch->pcdata->pathtotal = count;
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Process")) {
          ch->pcdata->process = fread_number(fp);
          ch->pcdata->process_timer = fread_number(fp);
          ch->pcdata->process_subtype = fread_number(fp);
          free_string(ch->pcdata->process_argumentone);
          ch->pcdata->process_argumentone = str_dup(fread_string(fp));
          free_string(ch->pcdata->process_argumenttwo);
          ch->pcdata->process_argumenttwo = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }

        break;

      case 'Q':
        KEY("QuitRoom", ch->pcdata->quit_room, fread_number(fp));
        break;

      case 'R':
        KEY("RacialPower", ch->pcdata->racial_power, fread_number(fp));
        KEY("RacialPowerTwo", ch->pcdata->racial_power_two, fread_number(fp));
        KEY("RPExpCap", ch->pcdata->rpexp_cap, fread_number(fp));
        KEYS("RadioAction", ch->pcdata->radio_action, fread_string(fp));
        KEY("Renames", ch->pcdata->renames, fread_number(fp));
        KEY("RitualPreyTimer", ch->pcdata->ritual_prey_timer, fread_number(fp));
        KEY("RitualPreyTarget", ch->pcdata->ritual_prey_target, fread_string(fp));
        KEY("RPExp", ch->rpexp, fread_number(fp));
        KEY("RPExpSpent", ch->spentrpexp, fread_number(fp));
        KEY("RitualDreamWorld", ch->pcdata->ritual_dreamworld, fread_string(fp));
        KEY("RPExpNew", ch->newrpexp, fread_number(fp));
        KEY("RPExpNewSpent", ch->spentnewrpexp, fread_number(fp));
        KEY("RRun", ch->pcdata->rooms_run, fread_number(fp));
        KEY("RCount", ch->pcdata->rooms_countdown, fread_number(fp));
        KEY("Resident", ch->pcdata->resident, fread_number(fp));
        KEY("Rank", ch->pcdata->rank_type, fread_number(fp));
        KEY("Role", ch->pcdata->role, fread_number(fp));
        KEY("RecentExp", ch->pcdata->recent_exp, fread_number(fp));
        KEY("RelPackCooldown", ch->pcdata->packcooldown, fread_number(fp));
        KEY("RelTeacherCooldown", ch->pcdata->teachercooldown, fread_number(fp));
        KEY("RememberDetail", ch->pcdata->remember_detail, fread_string(fp));
        KEY("RelapseVampire", ch->pcdata->relapsevampire, fread_number(fp));
        KEY("RelapseWerewolf", ch->pcdata->relapsewerewolf, fread_number(fp));
        KEY("RerollSelf", ch->pcdata->rerollselftime, fread_number(fp));
        KEYS("RitualLast", ch->pcdata->last_ritual, fread_string(fp));
        KEYS("RitualMaintainer", ch->pcdata->ritual_maintainer, fread_string(fp));
        KEYS("RitualMaintainTarget", ch->pcdata->maintained_target,
        fread_string(fp));
        if (!str_cmp(word, "Repression")) {
          int ref = 0;
          for (int i = 0; i < 25; i++) {
            if (ch->pcdata->repressions[i] == NULL || safe_strlen(ch->pcdata->repressions[i]) < 1) {
              ref = i;
              continue;
            }
          }
          free_string(ch->pcdata->repressions[ref]);
          ch->pcdata->repressions[ref] = str_dup(fread_string(fp));
          fMatch = TRUE;
        }
        if (!str_cmp(word, "RPLog")) {
          int pos = fread_number(fp);
          free_string(ch->pcdata->rp_log[pos]);
          ch->pcdata->rp_log[pos] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Relationship")) {
          int stat;
          char *temp;
          stat = fread_number(fp);
          ch->pcdata->relationship_type[stat] = fread_number(fp);
          temp = fread_string(fp);
          free_string(ch->pcdata->relationship[stat]);
          ch->pcdata->relationship[stat] = str_dup(temp);

          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Race")) {
          char *tmp = fread_string(fp);
          ch->race = race_lookup(tmp);
          free_string(tmp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Room")) {
          ch->in_room = get_room_index(fread_number(fp));
          if (ch->in_room == NULL)
          ch->in_room = get_room_index(ROOM_VNUM_LIMBO);
          fMatch = TRUE;
          break;
        }

        break;

      case 'S':
        KEY("Scro", ch->lines, fread_number(fp));
        KEYS("Skin", ch->pcdata->skin, fread_string(fp));
        KEY("SuspendMyHaven", ch->pcdata->suspend_myhaven, fread_number(fp));
        KEYS("SummaryName", ch->pcdata->summary_name, fread_string(fp));
        KEYS("SummaryIntro", ch->pcdata->summary_intro, fread_string(fp));
        KEYS("SummaryContent", ch->pcdata->summary_content, fread_string(fp));
        KEYS("SummaryConclusion", ch->pcdata->summary_conclusion,
        fread_string(fp));
        KEY("SummaryType", ch->pcdata->summary_type, fread_number(fp));
        KEY("SummaryStage", ch->pcdata->summary_stage, fread_number(fp));
        KEY("SummaryBlood", ch->pcdata->summary_blood, fread_number(fp));
        KEY("SummaryIntel", ch->pcdata->summary_intel, fread_number(fp));
        KEY("SummaryLifeforce", ch->pcdata->summary_lifeforce, fread_number(fp));
        KEY("SummaryHelpless", ch->pcdata->summary_helpless, fread_number(fp));
        KEY("SummaryWhen", ch->pcdata->summary_when, fread_number(fp));
        KEY("SummaryHome", ch->pcdata->summary_home, fread_number(fp));
        KEYS("StoryRunner", ch->pcdata->storyrunner, fread_string(fp));
        KEY("Strains", ch->pcdata->strains, fread_number(fp));
        KEY("SelfBondage", ch->pcdata->selfbondage, fread_number(fp));
        KEY("ScrWidth", ch->linewidth, fread_number(fp));
        KEY("Shape", ch->shape, fread_number(fp));
        KEY("Sex", ch->sex, fread_number(fp));
        KEY("SexPotency", ch->pcdata->sex_potency, fread_number(fp));
        KEY("SexDirty", ch->pcdata->sex_dirty, fread_number(fp));
        KEY("SecretDays", ch->pcdata->secret_days, fread_number(fp));
        KEY("SecretRecover", ch->pcdata->secret_recover, fread_number(fp));
        KEY("StasisTime", ch->pcdata->stasis_time, fread_number(fp));
        KEY("SummonBound", ch->pcdata->summon_bound, fread_number(fp));
        KEY("ShadowWalkRoom", ch->pcdata->shadow_walk_room, fread_number(fp));
        KEY("ShadowWalkCooldown", ch->pcdata->shadow_walk_cooldown, fread_number(fp));
        KEY("StasisTime", ch->pcdata->stasis_time, fread_number(fp));
        KEY("StasisSpentPKarma", ch->pcdata->stasis_spent_pkarma,
        fread_number(fp));
        KEY("StasisSpentKarma", ch->pcdata->stasis_spent_karma, fread_number(fp));
        KEY("StasisSpentRPExp", ch->pcdata->stasis_spent_rpexp, fread_number(fp));
        KEY("StasisSpentExp", ch->pcdata->stasis_spent_exp, fread_number(fp));
        KEY("SpentResources", ch->pcdata->spent_resources, fread_number(fp));
        KEYS("ShortDescr", ch->short_descr, fread_string(fp));
        KEY("SMonth", ch->pcdata->sire_month, fread_number(fp));
        KEY("SYear", ch->pcdata->sire_year, fread_number(fp));
        KEY("SDay", ch->pcdata->sire_day, fread_number(fp));
        KEY("Sleeping", ch->pcdata->sleeping, fread_number(fp));
        KEY("Spectre", ch->pcdata->spectre, fread_number(fp));
        KEY("SolitaryTime", ch->pcdata->solitary_time, fread_number(fp));
        KEYS("ShD", ch->short_descr, fread_string(fp));
        KEY("Sec", ch->pcdata->security, fread_number(fp)); /* OLC */
        KEY("SelfEsteem", ch->pcdata->selfesteem, fread_number(fp));
        KEY("Squish", ch->pcdata->squish, fread_number(fp));
        KEY("SRun", ch->pcdata->stories_run, fread_number(fp));
        KEY("SCount", ch->pcdata->stories_countdown, fread_number(fp));
        KEY("Superlocked", ch->pcdata->superlocked, fread_number(fp));
        KEYS("Scent", ch->pcdata->scent, fread_string(fp));
        KEY("ScCost", ch->pcdata->perfume_cost, fread_number(fp));
        KEY("Sincooldown", ch->pcdata->sincool, fread_number(fp));
        KEYS("Speaking", ch->pcdata->speaking, fread_string(fp));
        KEY("SpiritType", ch->pcdata->spirit_type, fread_number(fp));
        KEY("SexChange", ch->pcdata->sexchange_time, fread_number(fp));
        if (!str_cmp(word, "Scar")) {
          int i = fread_number(fp);
          free_string(ch->pcdata->scars[i]);
          ch->pcdata->scars[i] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "StatLog")) {
          int ref = 0;
          for (int i = 0; i < 300; i++) {
            if (ch->pcdata->stat_log_stat[i] <= 0 || ch->pcdata->stat_log_stat[i] > SKILL_MAX || ch->pcdata->stat_log_cost[i] <= 0) {
              ref = i;
              i = 300;
            }
          }
          ch->pcdata->stat_log_stat[ref] = fread_number(fp);
          ch->pcdata->stat_log_from[ref] = fread_number(fp);
          ch->pcdata->stat_log_to[ref] = fread_number(fp);
          ch->pcdata->stat_log_cost[ref] = fread_number(fp);
          ch->pcdata->stat_log_method[ref] = fread_number(fp);
          free_string(ch->pcdata->stat_log_string[ref]);
          ch->pcdata->stat_log_string[ref] = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "SDial")) {
          int i;
          char *temp;
          for (i = 0; i < 50 && ch->pcdata->speed_numbers[i] != 0; i++) {
          }
          ch->pcdata->speed_numbers[i] = fread_number(fp);
          temp = fread_word(fp);
          free_string(ch->pcdata->speed_names[i]);
          ch->pcdata->speed_names[i] = str_dup(temp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Sins")) {
          int stat;

          for (stat = 0; stat < 7; stat++)
          ch->pcdata->sins[stat] = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Survey")) {
          ch->pcdata->survey_stage = fread_number(fp);
          ch->pcdata->survey_delay = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Stat")) {
          int stat;
          for (stat = 0; stat < SKILL_MAX; stat++)
          ch->skills[stat] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Special")) {
          int i = fread_number(fp);
          ch->pcdata->specials[i][0] = fread_number(fp);
          ch->pcdata->specials[i][1] = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Shadow")) {
          int i = fread_number(fp);
          ch->pcdata->shadow_attacks[i][0] = fread_number(fp);
          ch->pcdata->shadow_attacks[i][1] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        break;

      case 'T':
        KEY("Testosterone", ch->pcdata->testosterone, fread_number(fp));
        KEY("Trust", ch->trust, fread_number(fp));
        KEY("Tru", ch->trust, fread_number(fp));
        KEYS("Talk", ch->pcdata->talk, fread_string(fp));
        KEY("TrainStat", ch->pcdata->training_stat, fread_number(fp));
        KEY("TierRaised", ch->pcdata->tier_raised, fread_number(fp));
        KEY("TrainDisc", ch->pcdata->training_disc, fread_number(fp));
        KEY("Timeswept", ch->pcdata->timeswept, fread_number(fp));
        KEY("TBoosts", ch->pcdata->tboosts, fread_number(fp));
        KEY("Timedeaged", ch->pcdata->deaged, fread_number(fp));
        KEY("Timeagemod", ch->pcdata->agemod, fread_number(fp));
        KEY("Travelto", ch->pcdata->travel_to, fread_number(fp));
        KEY("Travelfrom", ch->pcdata->travel_from, fread_number(fp));
        KEY("Traveltime", ch->pcdata->travel_time, fread_number(fp));
        KEY("Traveltype", ch->pcdata->travel_type, fread_number(fp));
        KEY("TimeSinceTrain", ch->pcdata->time_since_train, fread_number(fp));
        KEY("TrueID", ch->pcdata->true_id, fread_number(fp));
        KEY("TailMask", ch->pcdata->tail_mask, fread_number(fp));
        KEYS("Taste", ch->pcdata->taste, fread_string(fp));
        KEY("TimeNotAlone", ch->pcdata->timenotalone, fread_number(fp));
        KEY("Translocked", ch->pcdata->translocked, fread_number(fp));
        KEY("TruePenis", ch->pcdata->truepenis, fread_number(fp));
        KEY("TrueBreasts", ch->pcdata->truebreasts, fread_number(fp));

        if (!str_cmp(word, "TEdit")) {
          for (int i = 0; i < 15; i++)
          ch->pcdata->territory_editing[i] = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Tail")) {
          ch->pcdata->tail_timer = fread_number(fp);
          ch->pcdata->tailing = fread_string(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Trusts")) {
          int i;
          char *temp;
          for (i = 0; i < MAX_TRUSTS && ch->pcdata->trust_levels[i] != 0; i++) {
          }
          ch->pcdata->trust_levels[i] = fread_number(fp);
          temp = fread_word(fp);
          free_string(ch->pcdata->trust_names[i]);
          ch->pcdata->trust_names[i] = str_dup(temp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Title") || !str_cmp(word, "Titl")) {
          ch->pcdata->title = fread_string(fp);

          if (ch->pcdata->title[0] != '.' && ch->pcdata->title[0] != ',' && ch->pcdata->title[0] != '!' && ch->pcdata->title[0] != '?' && ch->pcdata->title[0] != ';' && ch->pcdata->title[0] != '\'') {
            sprintf(buf, " %s", ch->pcdata->title);
            free_string(ch->pcdata->title);
            ch->pcdata->title = str_dup(buf);
          }

          fMatch = TRUE;
          break;
        }

        break;

      case 'U':
        KEYS("Understanding", ch->pcdata->understanding, fread_string(fp));
        KEY("Understandtoggle", ch->pcdata->understandtoggle, fread_number(fp));
        KEYS("UPass", ch->pcdata->upwd, fread_string(fp));
        break;

      case 'V':
        KEYS("VerbalColor", ch->pcdata->verbal_color, fread_string(fp));
        KEY("Version", ch->version, fread_number(fp));
        KEY("VPlayer", ch->pcdata->version_player, fread_number(fp));
        KEY("Vers", ch->version, fread_number(fp));
        KEY("VBloodCool", ch->pcdata->vbloodcool, fread_number(fp));
        KEY("Vassal", ch->vassal, fread_number(fp));
        KEY("VillainOption", ch->pcdata->villain_option, fread_number(fp));
        KEY("VillainOptionCooldown", ch->pcdata->villain_option_cooldown,
        fread_number(fp));
        KEY("VillainEmoteCooldown", ch->pcdata->villain_emote_cooldown,
        fread_number(fp));
        KEY("VictimizeDifficult", ch->pcdata->victimize_difficult_time,
        fread_number(fp));
        KEY("VirginityLost", ch->pcdata->virginity_lost, fread_number(fp));
        if (!str_cmp(word, "Villain")) {
          ch->pcdata->villain_type = fread_number(fp);
          free_string(ch->pcdata->villain_praise);
          ch->pcdata->villain_praise = str_dup(fread_word(fp));
          free_string(ch->pcdata->villain_message);
          ch->pcdata->villain_message = str_dup(fread_string(fp));
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "VictimizeHistory")) {
          int stat;
          for (stat = 0; stat < 20; stat++)
          ch->pcdata->victimize_history[stat] = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        if (!str_cmp(word, "Vnum")) {
          ch->pIndexData = get_mob_index(fread_number(fp));
          fMatch = TRUE;
          break;
        }
        break;

      case 'W':
        KEY("Wizn", ch->wiznet, fread_flag(fp));
        KEY("WithoutSleep", ch->pcdata->without_sleep, fread_number(fp));
        KEY("Wounds", ch->wounds, fread_number(fp));
        KEY("WeaknessStatus", ch->pcdata->weakness_status, fread_number(fp));
        KEY("WeaknessTimer", ch->pcdata->weakness_timer, fread_number(fp));
        KEY("Wexp", ch->pcdata->wexp, fread_number(fp));
        KEYS("Wolfdesc", ch->pcdata->wolfdesc, fread_string(fp));
        KEYS("wolfintro", ch->pcdata->wolfintro, fread_string(fp));
        KEYS("WolfChangeTo", ch->pcdata->wolf_change_to, fread_string(fp));
        KEYS("WolfChangeFrom", ch->pcdata->wolf_change_from, fread_string(fp));
        KEY("WildLegendary", ch->pcdata->wilds_legendary, fread_number(fp));

        if (!str_cmp(word, "WildFame")) {
          ch->pcdata->wilds_fame_level = fread_number(fp);
          free_string(ch->pcdata->wilds_fame);
          ch->pcdata->wilds_fame = fread_string(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "WildsStat")) {
          int stat;
          for (stat = 0; stat < SKILL_MAX; stat++)
          ch->wilds_skills[stat] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "WeekTracker")) {
          for (int i = 0; i < 40; i++)
          ch->pcdata->week_tracker[i] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Whot")) {
          ch->pcdata->whotitle = fread_string(fp);
          // added blurb to maintain punctuation upon login - Discordance
          if (ch->pcdata->whotitle[0] != '.' && ch->pcdata->whotitle[0] != ',' && ch->pcdata->whotitle[0] != '!' && ch->pcdata->whotitle[0] != '?' && ch->pcdata->whotitle[0] != ';' && ch->pcdata->whotitle[0] != '\'') {
            sprintf(buf, " %s", ch->pcdata->whotitle);
            free_string(ch->pcdata->whotitle);
            ch->pcdata->whotitle = str_dup(buf);
          }
          fMatch = TRUE;
          break;
        }

        break;
      case 'Y':
        KEY("YouBadCounter", ch->pcdata->youbadcounter, fread_number(fp));
        break;
      }

      if (!fMatch) {
        sprintf(buf, "Fread_char: no match %s", word);
        bug(buf, 0);
        fread_to_eol(fp);
      }
    }
  }

  extern OBJ_DATA *obj_free;

  void fread_obj(THING th, FILE *fp) {
    OBJ_DATA *obj;
    const char *word;
    int iNest;
    bool fMatch;
    bool fNest;
    bool fVnum;
    bool first;
    bool new_format; /* to prevent errors */
    bool make_new;   /* update object */
    bool donotload = FALSE;
    fVnum = FALSE;
    obj = NULL;
    first = TRUE; /* used to counter fp offset */
    new_format = FALSE;
    make_new = FALSE;

    word = feof(fp) ? "End" : fread_word(fp);
    if (!str_cmp(word, "Vnum")) {
      int vnum;
      first = FALSE; /* fp will be in right place */

      vnum = fread_number(fp);
      if (get_obj_index(vnum) == NULL) {
        bug("Fread_obj: bad vnum %d.", vnum);
        donotload = TRUE;
      }
      else {
        obj = create_object(get_obj_index(vnum), -1);
        new_format = TRUE;
      }
    }

    if (donotload == TRUE) {
      for (;;) {
        if (first)
        first = FALSE;
        else
        word = feof(fp) ? "End" : fread_word(fp);
        fMatch = FALSE;
        if (word != NULL) {
          switch (UPPER(word[0])) {
          case 'E':
            if (!str_cmp(word, "End")) {
              return;
            }
            break;
          default:
            fread_to_eol(fp);
            break;
          }
        }
      }
    }

    if (obj == NULL) /* either not found or old style */
    {
      obj = new_obj();

      obj->name = str_dup("");
      obj->short_descr = str_dup("");
      obj->description = str_dup("");
      obj->adjust_string = str_dup("");
    }

    fNest = FALSE;
    fVnum = TRUE;
    iNest = 0;

    for (;;) {
      if (first)
      first = FALSE;
      else
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;

      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'A':
        KEY("Adjust", obj->adjust_string, fread_string(fp));
        if (!str_cmp(word, "Affc")) {
          AFFECT_DATA *paf;

          paf = new_affect();

          paf->where = fread_number(fp);
          paf->level = fread_number(fp);
          paf->duration = fread_number(fp);
          if (paf->duration < 0)
          paf->duration = 1;
          paf->modifier = fread_number(fp);
          paf->location = fread_number(fp);
          paf->bitvector = fread_number(fp);

          paf->weave = FALSE;
          paf->type = 0;
          paf->held = FALSE;
          paf->caster = NULL;
          paf->inverted = FALSE;
          paf->next = obj->affected;
          obj->affected = paf;
          fMatch = TRUE;
          break;
        }

        break;

      case 'B':
        KEY("Buff", obj->buff, fread_number(fp));
        break;
      case 'C':
        KEY("Cond", obj->condition, fread_number(fp));
        KEY("Cost", obj->cost, fread_number(fp));
        break;

      case 'D':
        KEY("Description", obj->description, fread_string(fp));
        KEY("Desc", obj->description, fread_string(fp));
        break;

      case 'E':
        KEY("Enchanted", obj->number_enchant, fread_number(fp));

        if (!str_cmp(word, "Enchanted")) {
          fMatch = TRUE;
          break;
        }

        KEY("ExtraFlags", obj->extra_flags, fread_number(fp));
        KEY("ExtF", obj->extra_flags, fread_number(fp));

        if (!str_cmp(word, "ExtraDescr") || !str_cmp(word, "ExDe")) {
          EXTRA_DESCR_DATA *ed;

          ed = new_extra_descr();

          ed->keyword = fread_string(fp);
          ed->description = fread_string(fp);
          ed->next = obj->extra_descr;
          obj->extra_descr = ed;
          fMatch = TRUE;
        }

        if (!str_cmp(word, "End")) {
          if (!fNest || (fVnum && obj->pIndexData == NULL)) {
            bug("Fread_obj: incomplete object.", 0);
            free_obj(obj);
            return;
          }
          else {
            if (!fVnum) {
              free_obj(obj);
              obj = create_object(get_obj_index(OBJ_VNUM_DUMMY), 0);
            }

            if (!new_format) {
              object_list.push_front(obj);
              obj->pIndexData->count++;
            }

            if (make_new) {
              int wear;

              wear = obj->wear_loc;
              extract_obj_silent(obj);

              obj = create_object(obj->pIndexData, 0);
              obj->wear_loc = wear;
            }
            if (iNest == 0 || rgObjNest[iNest - 1] == NULL) {
              if (th.thing_type == THING_CH) {
                obj_to_char(obj, th.thing.ch);
                if (obj->wear_loc > -1) {
                }
              }
              else if (th.thing_type == THING_OBJ) {
                obj_to_obj(obj, th.thing.obj);
              }
            }
            else
            obj_to_obj(obj, rgObjNest[iNest - 1]);
            return;
          }
        }
        break;

      case 'F':
        KEY("Faction", obj->faction, fread_number(fp));
        break;

      case 'I':
        KEY("ItemType", obj->item_type, fread_number(fp));
        KEY("Ityp", obj->item_type, fread_number(fp));
        break;

      case 'L':
        KEY("Level", obj->level, fread_number(fp));
        KEY("Lev", obj->level, fread_number(fp));
        break;

      case 'M':
        KEY("Material", obj->material, fread_string(fp));
        break;
      case 'N':
        KEY("Name", obj->name, fread_string(fp));

        if (!str_cmp(word, "Nest")) {
          iNest = fread_number(fp);
          if (iNest < 0 || iNest >= MAX_NEST) {
            bug("Fread_obj: bad nest %d.", iNest);
          }
          else {
            rgObjNest[iNest] = obj;
            fNest = TRUE;
          }
          fMatch = TRUE;
        }
        break;
      case 'O':

        if (!str_cmp(word, "Outfit")) {
          obj->outfit[0] = fread_number(fp);
          obj->outfit[1] = fread_number(fp);
          obj->outfit[2] = fread_number(fp);
          obj->outfit[3] = fread_number(fp);
          obj->outfit[4] = fread_number(fp);
          obj->outfit[5] = fread_number(fp);
          obj->outfit[6] = fread_number(fp);
          obj->outfit[7] = fread_number(fp);
          obj->outfit[8] = fread_number(fp);
          obj->outfit[9] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "OLocation")) {
          obj->outfit_location[0] = fread_number(fp);
          obj->outfit_location[1] = fread_number(fp);
          obj->outfit_location[2] = fread_number(fp);
          obj->outfit_location[3] = fread_number(fp);
          obj->outfit_location[4] = fread_number(fp);
          obj->outfit_location[5] = fread_number(fp);
          obj->outfit_location[6] = fread_number(fp);
          obj->outfit_location[7] = fread_number(fp);
          obj->outfit_location[8] = fread_number(fp);
          obj->outfit_location[9] = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        break;
      case 'R':
        KEY("Rot", obj->rot_timer, fread_number(fp));
        break;
      case 'S':
        KEY("ShortDescr", obj->short_descr, fread_string(fp));
        KEY("ShD", obj->short_descr, fread_string(fp));
        KEY("Stash", obj->stash_room, fread_number(fp));

        if (!str_cmp(word, "Spell")) {
          int iValue;

          iValue = fread_number(fp);
          if (iValue < 0 || iValue > 3) {
            bug("Fread_obj: bad iValue %d.", iValue);
          }

          fMatch = TRUE;
          break;
        }

        break;

      case 'T':
        KEY("Timer", obj->timer, fread_number(fp));
        KEY("Time", obj->timer, fread_number(fp));
        break;

      case 'V':
        /*
if ( !str_cmp( word, "Values" ) || !str_cmp(word,"Vals"))
{
obj->value[0]   = fread_number( fp );
obj->value[1]   = fread_number( fp );
obj->value[2]   = fread_number( fp );
obj->value[3]   = fread_number( fp );
if (obj->item_type == ITEM_WEAPON && obj->value[0] == 0)
obj->value[0] = obj->pIndexData->value[0];
fMatch      = TRUE;
break;
}
*/
        if (!str_cmp(word, "Val")) {
          obj->value[0] = fread_number(fp);
          obj->value[1] = fread_number(fp);
          obj->value[2] = fread_number(fp);
          obj->value[3] = fread_number(fp);
          obj->value[4] = fread_number(fp);
          obj->value[5] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Vnum")) {
          int vnum;

          vnum = fread_number(fp);
          if ((obj->pIndexData = get_obj_index(vnum)) == NULL)
          bug("Fread_obj: bad vnum %d.", vnum);
          else
          fVnum = TRUE;
          fMatch = TRUE;
          break;
        }
        break;

      case 'W':
        KEY("WearFlags", obj->wear_flags, fread_number(fp));
        KEY("WeaF", obj->wear_flags, fread_number(fp));
        KEY("WearLoc", obj->wear_loc, fread_number(fp));
        KEY("Wear", obj->wear_loc, fread_number(fp));
        KEY("WearString", obj->wear_string, fread_string(fp));
        KEY("Weight", obj->size, fread_number(fp));
        KEY("Wt", obj->size, fread_number(fp));
        break;
      }

      if (!fMatch) {
        char buf[256];
        sprintf(buf, "Fread_obj: no match -> %s.", word);
        bug(buf, 0);
        fread_to_eol(fp);
      }
    }
  }

  void fread_gobj(FILE *fp) {
    OBJ_DATA *obj;
    const char *word;
    int iNest;
    bool fMatch;
    bool fNest;
    bool fVnum;
    bool first;
    bool new_format; /* to prevent errors */
    bool make_new;   /* update object */
    int roomvnum = 0;

    fVnum = FALSE;
    obj = NULL;
    first = TRUE; /* used to counter fp offset */
    new_format = FALSE;
    make_new = FALSE;

    word = feof(fp) ? "End" : fread_word(fp);
    if (!str_cmp(word, "Vnum")) {
      int vnum;
      first = FALSE; /* fp will be in right place */

      vnum = fread_number(fp);
      //    char buf[MSL];
      //   sprintf(buf, "Load Obj: %d", vnum);
      //   log_string(buf);
      if (get_obj_index(vnum) == NULL) {
        bug("Fread_obj: bad vnum %d.", vnum);
      }
      else {
        obj = create_object(get_obj_index(vnum), -1);
        new_format = TRUE;
      }
      /*
char buf[MSL];
sprintf(buf, "Loading gobj: %d", vnum);
log_string(buf);
*/
    }

    if (obj == NULL) /* either not found or old style */
    {
      obj = new_obj();

      obj->name = str_dup("");
      obj->short_descr = str_dup("");
      obj->description = str_dup("");
      obj->adjust_string = str_dup("");
    }

    fNest = FALSE;
    fVnum = TRUE;
    iNest = 0;

    for (;;) {
      if (first)
      first = FALSE;
      else
      word = feof(fp) ? "End" : fread_word(fp);
      fMatch = FALSE;

      switch (UPPER(word[0])) {
      case '*':
        fMatch = TRUE;
        fread_to_eol(fp);
        break;

      case 'A':
        KEY("Adjust", obj->adjust_string, fread_string(fp));
        if (!str_cmp(word, "Affc")) {
          AFFECT_DATA *paf;

          paf = new_affect();

          paf->where = fread_number(fp);
          paf->level = fread_number(fp);
          paf->duration = fread_number(fp);
          if (paf->duration < 0)
          paf->duration = 1;
          paf->modifier = fread_number(fp);
          paf->location = fread_number(fp);
          paf->bitvector = fread_number(fp);

          paf->weave = FALSE;
          paf->type = 0;
          paf->held = FALSE;
          paf->caster = NULL;
          paf->inverted = FALSE;
          paf->next = obj->affected;
          obj->affected = paf;
          fMatch = TRUE;
          break;
        }
        break;

      case 'B':
        KEY("Buff", obj->buff, fread_number(fp));
        break;
      case 'C':
        KEY("Cond", obj->condition, fread_number(fp));
        KEY("Cost", obj->cost, fread_number(fp));
        break;

      case 'D':
        KEY("Description", obj->description, fread_string(fp));
        KEY("Desc", obj->description, fread_string(fp));
        break;

      case 'E':
        KEY("Enchanted", obj->number_enchant, fread_number(fp));

        if (!str_cmp(word, "Enchanted")) {
          fMatch = TRUE;
          break;
        }

        KEY("ExtraFlags", obj->extra_flags, fread_number(fp));
        KEY("ExtF", obj->extra_flags, fread_number(fp));

        if (!str_cmp(word, "ExtraDescr") || !str_cmp(word, "ExDe")) {
          EXTRA_DESCR_DATA *ed;

          ed = new_extra_descr();

          ed->keyword = fread_string(fp);
          ed->description = fread_string(fp);
          ed->next = obj->extra_descr;
          obj->extra_descr = ed;
          fMatch = TRUE;
        }

        if (!str_cmp(word, "End")) {
          if (!fNest || (fVnum && obj->pIndexData == NULL)) {
            bug("Fread_obj: incomplete object.", 0);
            free_obj(obj);
            return;
          }
          else if ((roomvnum == 0 || get_room_index(roomvnum) == NULL) && (iNest == 0 || rgObjNest[iNest] == NULL)) {
            bug("Fread_obj: Invalid Room.", 0);
            free_obj(obj);
            return;
          }
          else {
            if (!fVnum) {
              free_obj(obj);
              obj = create_object(get_obj_index(OBJ_VNUM_DUMMY), 0);
            }

            if (!new_format) {
              object_list.push_front(obj);
              obj->pIndexData->count++;
            }

            if (make_new) {
              int wear;

              wear = obj->wear_loc;
              extract_obj_silent(obj);

              obj = create_object(obj->pIndexData, 0);
              obj->wear_loc = wear;
            }
            if (iNest == 0 || rgObjNest[iNest] == NULL) {
              obj_to_room(obj, get_room_index(roomvnum));
              obj->in_room = get_room_index(roomvnum);
              /*
if ( th.thing_type == THING_CH )
{
obj_to_char( obj, th.thing.ch );
if ( obj->wear_loc > -1 )
{
}
}
else if ( th.thing_type == THING_OBJ )
{
obj_to_obj( obj, th.thing.obj );
}
*/
            }
            else {
              obj_to_obj(obj, rgObjNest[iNest - 1]);
              if (roomvnum != 0)
              obj->in_room = get_room_index(roomvnum);
            }
            return;
          }
        }
        break;
      case 'F':
        KEY("Faction", obj->faction, fread_number(fp));
        break;
      case 'I':
        if (!str_cmp(word, "InRoom")) {
          roomvnum = fread_number(fp);
          //      obj->in_room = get_room_index(fread_number(fp));
          fMatch = TRUE;
        }
        KEY("ItemType", obj->item_type, fread_number(fp));
        KEY("Ityp", obj->item_type, fread_number(fp));
        break;

      case 'L':
        KEY("Level", obj->level, fread_number(fp));
        KEY("Lev", obj->level, fread_number(fp));
        break;

      case 'M':
        KEY("Material", obj->material, fread_string(fp));
        break;

      case 'N':
        KEY("Name", obj->name, fread_string(fp));

        if (!str_cmp(word, "Nest")) {
          iNest = fread_number(fp);
          if (iNest < 0 || iNest >= MAX_NEST) {
            bug("Fread_obj: bad nest %d.", iNest);
          }
          else {
            rgObjNest[iNest] = obj;
            fNest = TRUE;
          }
          fMatch = TRUE;
        }
        break;
      case 'O':

        if (!str_cmp(word, "Outfit")) {
          obj->outfit[0] = fread_number(fp);
          obj->outfit[1] = fread_number(fp);
          obj->outfit[2] = fread_number(fp);
          obj->outfit[3] = fread_number(fp);
          obj->outfit[4] = fread_number(fp);
          obj->outfit[5] = fread_number(fp);
          obj->outfit[6] = fread_number(fp);
          obj->outfit[7] = fread_number(fp);
          obj->outfit[8] = fread_number(fp);
          obj->outfit[9] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "OLocation")) {
          obj->outfit_location[0] = fread_number(fp);
          obj->outfit_location[1] = fread_number(fp);
          obj->outfit_location[2] = fread_number(fp);
          obj->outfit_location[3] = fread_number(fp);
          obj->outfit_location[4] = fread_number(fp);
          obj->outfit_location[5] = fread_number(fp);
          obj->outfit_location[6] = fread_number(fp);
          obj->outfit_location[7] = fread_number(fp);
          obj->outfit_location[8] = fread_number(fp);
          obj->outfit_location[9] = fread_number(fp);
          fMatch = TRUE;
          break;
        }
        break;
      case 'R':
        KEY("Rot", obj->rot_timer, fread_number(fp));
      case 'S':
        KEY("ShortDescr", obj->short_descr, fread_string(fp));
        KEY("ShD", obj->short_descr, fread_string(fp));
        KEY("Stash", obj->stash_room, fread_number(fp));

        if (!str_cmp(word, "Spell")) {
          int iValue;

          iValue = fread_number(fp);
          if (iValue < 0 || iValue > 3) {
            bug("Fread_obj: bad iValue %d.", iValue);
          }

          fMatch = TRUE;
          break;
        }

        break;

      case 'T':
        KEY("Timer", obj->timer, fread_number(fp));
        KEY("Time", obj->timer, fread_number(fp));
        break;

      case 'V':
        /*
if ( !str_cmp( word, "Values" ) || !str_cmp(word,"Vals"))
{
obj->value[0]   = fread_number( fp );
obj->value[1]   = fread_number( fp );
obj->value[2]   = fread_number( fp );
obj->value[3]   = fread_number( fp );
if (obj->item_type == ITEM_WEAPON && obj->value[0] == 0)
obj->value[0] = obj->pIndexData->value[0];
fMatch          = TRUE;
break;
}
*/
        if (!str_cmp(word, "Val")) {
          obj->value[0] = fread_number(fp);
          obj->value[1] = fread_number(fp);
          obj->value[2] = fread_number(fp);
          obj->value[3] = fread_number(fp);
          obj->value[4] = fread_number(fp);
          obj->value[5] = fread_number(fp);
          fMatch = TRUE;
          break;
        }

        if (!str_cmp(word, "Vnum")) {
          int vnum;

          vnum = fread_number(fp);
          if ((obj->pIndexData = get_obj_index(vnum)) == NULL)
          bug("Fread_obj: bad vnum %d.", vnum);
          else
          fVnum = TRUE;
          fMatch = TRUE;
          break;
        }
        break;

      case 'W':
        KEY("WearFlags", obj->wear_flags, fread_number(fp));
        KEY("WeaF", obj->wear_flags, fread_number(fp));
        KEY("WearLoc", obj->wear_loc, fread_number(fp));
        KEY("Wear", obj->wear_loc, fread_number(fp));
        KEY("WearString", obj->wear_string, fread_string(fp));
        KEY("Weight", obj->size, fread_number(fp));
        KEY("Wt", obj->size, fread_number(fp));
        break;
      }

      if (!fMatch) {
        char buf[256];
        sprintf(buf, "Fread_obj: no match -> %s.", word);
        bug(buf, 0);
        fread_to_eol(fp);
      }
    }
  }

  /******************************************************************
* Name: save_corpses
* Paramters:
*  void
* Returns:
*  void
*
* Purpose: Save the contents of all PC corpses in the game to
*  seperate files to be loaded back into the game at boot
* Author: Cameron Matthews-Dickson (Scaelorn)
******************************************************************/
  void save_corpses() {
    FILE *list, *fp;
    char filename[MAX_INPUT_LENGTH];
    char *name, *argument;

    sprintf(filename, "%s%s", CORPSE_DIR, CORPSE_LIST);
    if ((list = fopen(filename, "w")) == NULL) {
      bug("save_corpses: fopen list", 0);
      perror(filename);
      fclose(list);
    }
    else {
      /*
* Go through a list of all objects and find the PC corpses
* that contain objects
*/
      for (ObjList::iterator it = object_list.begin(); it != object_list.end();
      ++it) {
        OBJ_DATA *obj = *it;

        if (obj->pIndexData->vnum == OBJ_VNUM_CORPSE_PC && obj->contains && obj->carried_by == NULL) {
          /* Get the name of the player from the corpses desc */
          name = str_dup(obj->short_descr);
          argument = &name[14];

          /* make an entry in the list for this corpse */
          fprintf(list, "%s\n", argument);

          /* Create the file for this corpse and saave contained objs */
          sprintf(filename, "%s%s", CORPSE_DIR, argument);
          if ((fp = fopen(filename, "w")) == NULL) {
            bug("save_corpses: fopen corpse file", 0);
            perror(filename);
            fclose(fp);
          }
          else {
            fprintf(fp, "#InRoom %d\n\n", obj->in_room->vnum);
            fwrite_obj(obj->contains, fp, 0);
            fprintf(fp, "#END\n");
          }
          free_string(name);
          fclose(fp);
        }
      }
    }

    fprintf(list, "$\n");
    fclose(list);
    return;
  }

  /******************************************************************
* Name: load_corpse_file
* Parameters:
*  char *fname - name of the file to open
* Returns:
*  bool - TRUE if success else FALSE
*
* Purpose: Creates a corpse object and loads all the items
*  contained in the file specified by the 'fname' parameter
*  into the corpse.  Then puts corpse where it was saved at.
* Author: Cameron Matthews-Dickson (Scaelorn)
******************************************************************/
  bool load_corpse_file(const char *fname) {
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *corpse = NULL;
    FILE *fp;
    THING th;
    int vnum = ROOM_VNUM_LIMBO;

    /* Open the actual corpse file */
    sprintf(buf, "%s%s", CORPSE_DIR, fname);
    if ((fp = fopen(buf, "r")) == NULL) {
      perror(buf);
      return FALSE;
    }

    /* Load a PC corpse object to hold the items for this PCs corpse */
    corpse = create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0);
    corpse->timer = number_range(25, 40);
    corpse->cost = 0;

    /* Set the short and long descs to the name of whomever the corpse
* belongs to
*/
    sprintf(buf, corpse->short_descr, fname);
    free_string(corpse->short_descr);
    corpse->short_descr = str_dup(buf);

    sprintf(buf, corpse->description, fname);
    free_string(corpse->description);
    corpse->description = str_dup(buf);

    /* Lets setup the "thing" object */
    th.thing_type = THING_OBJ;
    th.thing.obj = corpse;

    for (;;) {
      char letter;
      char *word;

      letter = fread_letter(fp);
      if (letter == '*') {
        fread_to_eol(fp);
        continue;
      }

      if (letter != '#') {
        bug("load_corpse_file: # not found.", 0);
        break;
      }

      word = fread_word(fp);
      if (!str_cmp(word, "OBJECT"))
      fread_obj(th, fp);
      else if (!str_cmp(word, "O"))
      fread_obj(th, fp);
      else if (!str_cmp(word, "InRoom"))
      vnum = fread_number(fp);
      else if (!str_cmp(word, "END"))
      break;
      else {
        bug("load_corpse_file: bad section.", 0);
        break;
      }
    }

    obj_to_room(corpse, get_room_index(vnum));
    fclose(fp);

    return TRUE;
  }

  /******************************************************************
* Name: load_corpses
* Parameters:
*  void
* Returns:
*  void
*
* Purpose: Opens the list of corpses that need to be loaded and
*  calls load_corpse_file to actually load each
* Author: Cameron Matthews-Dickson (Scaelorn)
******************************************************************/
  void load_corpses() {
    FILE *fpList;
    const char *filename;
    char corpselist[256];
    char buf[MAX_STRING_LENGTH];

    /* Open the list of corpses that need to be loaded */
    sprintf(corpselist, "%s%s", CORPSE_DIR, CORPSE_LIST);
    if ((fpList = fopen(corpselist, "r")) == NULL) {
      perror(corpselist);
      return;
    }

    /* Until we read a '$' keep reading the names of corpse files */
    for (;;) {
      filename = feof(fpList) ? "$" : fread_word(fpList);
      if (filename[0] == '$')
      break;

      sprintf(buf, "Loading corpse: %s", filename);
      log_string(buf);
      if (!load_corpse_file(filename)) {
        sprintf(buf, "Cannot load corpse file: %s", filename);
        bug(buf, 0);
      }
    }

    fclose(fpList);
    return;
  }

  void save_pc_color(CHAR_DATA *ch, FILE *fp) {
    fprintf(fp, "Colora     %d%d%d %d%d%d %d%d%d %d%d%d %d%d%d\n",
    ch->pcdata->text[2], ch->pcdata->text[0], ch->pcdata->text[1],
    ch->pcdata->auction[2], ch->pcdata->auction[0],
    ch->pcdata->auction[1], ch->pcdata->gossip[2], ch->pcdata->gossip[0],
    ch->pcdata->gossip[1], ch->pcdata->irl[2], ch->pcdata->irl[0],
    ch->pcdata->irl[1], ch->pcdata->immortal[2], ch->pcdata->immortal[0],
    ch->pcdata->immortal[1]);
    fprintf(fp, "Colorb      %d%d%d %d%d%d %d%d%d\n", ch->pcdata->implementor[2],
    ch->pcdata->implementor[0], ch->pcdata->implementor[1],
    ch->pcdata->newbie[2], ch->pcdata->newbie[0], ch->pcdata->newbie[1],
    ch->pcdata->gtell[2], ch->pcdata->gtell[0], ch->pcdata->gtell[1]);
    fprintf(fp, "Colorc     %d%d%d %d%d%d %d%d%d\n", ch->pcdata->info[2],
    ch->pcdata->info[0], ch->pcdata->info[1], ch->pcdata->tells[2],
    ch->pcdata->tells[0], ch->pcdata->tells[1], ch->pcdata->reply[2],
    ch->pcdata->reply[0], ch->pcdata->reply[1]);
    fprintf(fp, "Colord     %d%d%d %d%d%d %d%d%d\n", ch->pcdata->room_exits[2],
    ch->pcdata->room_exits[0], ch->pcdata->room_exits[1],
    ch->pcdata->room_things[2], ch->pcdata->room_things[0],
    ch->pcdata->room_things[1], ch->pcdata->prompt[2],
    ch->pcdata->prompt[0], ch->pcdata->prompt[1]);
    fprintf(fp, "Colore     %d%d%d %d%d%d %d%d%d %d%d%d %d%d%d\n",
    ch->pcdata->fight_death[2], ch->pcdata->fight_death[0],
    ch->pcdata->fight_death[1], ch->pcdata->fight_yhit[2],
    ch->pcdata->fight_yhit[0], ch->pcdata->fight_yhit[1],
    ch->pcdata->fight_ohit[2], ch->pcdata->fight_ohit[0],
    ch->pcdata->fight_ohit[1], ch->pcdata->fight_thit[2],
    ch->pcdata->fight_thit[0], ch->pcdata->fight_thit[1],
    ch->pcdata->fight_skill[2], ch->pcdata->fight_skill[0],
    ch->pcdata->fight_skill[1]);
    fprintf(fp, "Colorf     %d%d%d %d%d%d %d%d%d\n", ch->pcdata->wiznet[2],
    ch->pcdata->wiznet[0], ch->pcdata->wiznet[1], ch->pcdata->say[2],
    ch->pcdata->say[0], ch->pcdata->say[1], ch->pcdata->ooc[2],
    ch->pcdata->ooc[0], ch->pcdata->ooc[1]);
    fprintf(fp, "Colorg       %d%d%d %d%d%d %d%d%d\n", ch->pcdata->hint[2],
    ch->pcdata->hint[0], ch->pcdata->hint[1], ch->pcdata->minioncolor[2],
    ch->pcdata->minioncolor[0], ch->pcdata->minioncolor[1],
    ch->pcdata->pray[2], ch->pcdata->pray[0], ch->pcdata->pray[1]);
    fprintf(fp, "Colorz       %d%d%d %d%d%d %d%d%d\n", ch->pcdata->yells[2],
    ch->pcdata->yells[0], ch->pcdata->yells[1], ch->pcdata->osay[2],
    ch->pcdata->osay[0], ch->pcdata->osay[1], ch->pcdata->hero[2],
    ch->pcdata->hero[0], ch->pcdata->hero[1]);
  }

  void fwrite_pc_creation(CHAR_DATA *ch, FILE *fp) {
    fprintf(fp, "Id   %ld\n", ch->id);
    fprintf(fp, "LogO %ld\n", current_time);
    fprintf(fp, "Vers %d\n", CURR_VERSION);
    fprintf(fp, "Sex  %d\n", ch->sex);
    fprintf(fp, "Levl %d\n", ch->level);
    fprintf(fp, "Lname %s~\n", ch->pcdata->last_name);
    fprintf(fp, "Email %s~\n", ch->pcdata->email);
    fprintf(fp, "CLocation %d", ch->pcdata->creation_location);

    if (ch->race > -1)
    fprintf(fp, "Race %s~\n", race_table[ch->race].name);

    fprintf(fp, "Pass %s~\n", ch->pcdata->pwd);

    if (ch->description[0] != '\0')
    fprintf(fp, "Desc %s~\n", ch->description);

    fprintf(fp, "Comm %s\n", set_print_flag(ch->comm));
    fprintf(fp, "Act  %s\n", set_print_flag(ch->act));

    fprintf(fp, "End\n\n");
    return;
  }

  bool is_obj_reset(OBJ_DATA *obj, ROOM_INDEX_DATA *room) {
    RESET_DATA *pReset;

    for (pReset = room->reset_first; pReset; pReset = pReset->next) {
      if (pReset->command != 'O')
      continue;

      if (pReset->arg1 == obj->pIndexData->vnum)
      return TRUE;
    }
    return FALSE;
  }

  void save_ground_objects() {
    char strsave[MAX_INPUT_LENGTH];
    sprintf(strsave, "%s%s", PLAYER_DIR, "GroundObjects");
    FILE *fp = NULL;
    int p = 0;
    if ((fp = fopen(TEMP_FILE, "w")) == NULL) {
      bug("Save_ground_obj: fopen", 0);
      perror(strsave);
    }
    else {

      for (ObjList::iterator it = object_list.begin();
      it != object_list.end() && p < 500000; p++) {
        OBJ_DATA *obj = *it;
        ++it;

        if (obj == NULL)
        continue;

        if (obj->carried_by != NULL)
        continue;

        if (obj->in_room == NULL)
        continue;

        if (is_obj_reset(obj, obj->in_room))
        continue;

        fwrite_obj_2(obj, fp, 0);
      }

      fprintf(fp, "#END\n");
      fclose(fp);
    }

    rename(TEMP_FILE, strsave);

    if (time_info.day % 7 == 0)
    sprintf(strsave, "%s%s", BACK1_DIR, "GroundObjects");
    else if (time_info.day % 6 == 0)
    sprintf(strsave, "%s%s", BACK2_DIR, "GroundObjects");
    else if (time_info.day % 5 == 0)
    sprintf(strsave, "%s%s", BACK3_DIR, "GroundObjects");
    else if (time_info.day % 4 == 0)
    sprintf(strsave, "%s%s", BACK4_DIR, "GroundObjects");
    else if (time_info.day % 3 == 0)
    sprintf(strsave, "%s%s", BACK5_DIR, "GroundObjects");
    else if (time_info.day % 2 == 0)
    sprintf(strsave, "%s%s", BACK6_DIR, "GroundObjects");
    else
    sprintf(strsave, "%s%s", BACK7_DIR, "GroundObjects");

    fp = NULL;
    p = 0;
    if ((fp = fopen(TEMP_FILE, "w")) == NULL) {
      bug("Save_ground_obj: fopen", 0);
      perror(strsave);
    }
    else {

      for (ObjList::iterator it = object_list.begin();
      it != object_list.end() && p < 500000; p++) {
        OBJ_DATA *obj = *it;
        ++it;

        if (obj == NULL)
        continue;

        if (obj->carried_by != NULL)
        continue;

        if (obj->in_room == NULL)
        continue;

        if (is_obj_reset(obj, obj->in_room))
        continue;

        fwrite_obj_2(obj, fp, 0);
      }

      fprintf(fp, "#END\n");
      fclose(fp);
    }

    rename(TEMP_FILE, strsave);
  }

  void load_ground_objects() {

    char strsave[MAX_INPUT_LENGTH];
    FILE *fp = NULL;

    sprintf(strsave, "%s%s", PLAYER_DIR, "GroundObjects");

    if ((fp = fopen(strsave, "r")) != NULL) {
      int iNest;

      for (iNest = 0; iNest < MAX_NEST; iNest++)
      rgObjNest[iNest] = NULL;

      for (;;) {
        char letter;
        char *word;

        letter = fread_letter(fp);
        if (letter == '*') {
          fread_to_eol(fp);
          continue;
        }

        if (letter != '#') {
          char buf[MSL];
          sprintf(buf, "Load_char_obj: # not found, letter: %c", letter);
          bug(buf, 0);
          break;
        }

        word = fread_word(fp);
        if (!str_cmp(word, "OBJECT"))
        fread_gobj(fp);
        else if (!str_cmp(word, "O"))
        fread_gobj(fp);
        else if (!str_cmp(word, "END"))
        break;
        else {
          bug("Load_ground_obj: bad section.", 0);
          break;
        }
      }
      fclose(fp);
    }
    else {
      bug("Load_ground_obj: Couldn't open.", 0);
      bug(strsave, 0);
    }
  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

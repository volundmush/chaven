#if defined(_WIN32)
#if defined(_DEBUG)
#pragma warning(disable : 4786)
#endif
#endif
#include "Note.h"

#ifndef WIN32
#include <sys/types.h>
#endif

#include "merc.h"
#include "recycle.h"
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <vector>

class Note;
class NoteBoard;

#if defined(__cplusplus)
extern "C" {
#endif

  /* stuff for recycling ban structures */
  BanList ban_free;

  BAN_DATA *new_ban(void) {
    static BAN_DATA ban_zero;
    BAN_DATA *ban;

    if (ban_free.empty())
    ban = (BAN_DATA *)alloc_perm(sizeof(*ban));
    else {
      ban = ban_free.front();
      ban_free.pop_front();
    }

    *ban = ban_zero;
    VALIDATE(ban);
    ban->name = &str_empty[0];
    return ban;
  }

  void free_ban(BAN_DATA *ban) {
    if (!IS_VALID(ban))
    return;

    free_string(ban->name);
    INVALIDATE(ban);

    ban_free.push_front(ban);
  }

  DescList descriptor_free;

  DESCRIPTOR_DATA *new_descriptor() {
    static DESCRIPTOR_DATA d_zero;
    DESCRIPTOR_DATA *d;

    if (descriptor_free.empty())
    d = (DESCRIPTOR_DATA *)alloc_perm(sizeof(*d));
    else {
      d = descriptor_free.front();
      descriptor_free.pop_front();
    }

    *d = d_zero;
    VALIDATE(d);

    d->snoop_by = NULL;
    d->character = NULL;
    d->account = NULL;
    d->original = NULL;
    d->host = str_dup("");
    d->hostip = str_dup("");

    d->connected = CON_GET_NAME;
    d->showstr_head = NULL;
    d->showstr_point = NULL;
    d->mxp = FALSE;
    d->outsize = 2000;
    d->pEdit = NULL;   /* OLC */
    d->pString = NULL; /* OLC */
    d->editor = 0;     /* OLC */
    d->outbuf = (char *)alloc_mem(d->outsize);
    d->ansi = 0;

    return d;
  }

  void free_descriptor(DESCRIPTOR_DATA *d) {
    if (!IS_VALID(d))
    return;

    free_string(d->host);
    free_string(d->hostip);
    free_string(d->ident);
    free_mem(d->outbuf, d->outsize);
    INVALIDATE(d);
    descriptor_free.push_front(d);
  }

  /* stuff for recycling extended descs */
  EXTRA_DESCR_DATA *extra_descr_free;

  EXTRA_DESCR_DATA *new_extra_descr(void) {
    EXTRA_DESCR_DATA *ed;

    if (extra_descr_free == NULL)
    ed = (EXTRA_DESCR_DATA *)alloc_perm(sizeof(*ed));
    else {
      ed = extra_descr_free;
      extra_descr_free = extra_descr_free->next;
    }

    ed->keyword = str_dup("");
    ed->description = str_dup("");
    VALIDATE(ed);
    return ed;
  }

  void free_extra_descr(EXTRA_DESCR_DATA *ed) {
    if (!IS_VALID(ed))
    return;

    free_string(ed->keyword);
    free_string(ed->description);
    INVALIDATE(ed);

    ed->next = extra_descr_free;
    extra_descr_free = ed;
  }

  /* stuff for recycling mobile memory data */
  MEM_DATA *cmem_free;

  MEM_DATA *new_memory() {
    static MEM_DATA cmem_zero;
    MEM_DATA *mem;

    if (cmem_free == NULL)
    mem = (MEM_DATA *)alloc_perm(sizeof(*mem));
    else {
      mem = cmem_free;
      cmem_free = cmem_free->next;
    }

    *mem = cmem_zero;
    return mem;
  }

  void free_memory(MEM_DATA *mem) {
    mem->next = cmem_free;
    cmem_free = mem;
  }

  /* stuff for recycling mobile memory data */
  MEMORIZE_DATA *memorize_free;

  MEMORIZE_DATA *new_memorize() {
    static MEMORIZE_DATA memorize_zero;
    MEMORIZE_DATA *mem;

    if (memorize_free == NULL)
    mem = (MEMORIZE_DATA *)alloc_perm(sizeof(*mem));
    else {
      mem = memorize_free;
      memorize_free = memorize_free->next;
    }

    *mem = memorize_zero;

    mem->next = NULL;
    mem->keyword = str_dup("");
    mem->vnum = -1;
    mem->complete = FALSE;

    return mem;
  }

  void free_memorize(MEMORIZE_DATA *mem) {
    free_string(mem->keyword);
    mem->next = memorize_free;
    memorize_free = mem;
  }

  /* stuff for recycling affects */
  AFFECT_DATA *affect_free;

  AFFECT_DATA *new_affect(void) {
    static AFFECT_DATA af_zero;
    AFFECT_DATA *af;

    if (affect_free == NULL)
    af = (AFFECT_DATA *)alloc_perm(sizeof(*af));
    else {
      af = affect_free;
      affect_free = affect_free->next;
    }

    *af = af_zero;

    af->caster = NULL;
    af->rev = NULL;
    VALIDATE(af);
    return af;
  }

  void free_affect(AFFECT_DATA *af) {
    if (!IS_VALID(af))
    return;

    INVALIDATE(af);
    af->next = affect_free;
    affect_free = af;
  }

  /* stuff for recycling objects */
  ObjList obj_free;

  OBJ_DATA *new_obj(void) {
    static OBJ_DATA obj_zero;
    OBJ_DATA *obj;

    if (obj_free.empty())
    obj = (OBJ_DATA *)alloc_perm(sizeof(*obj));
    else {
      obj = obj_free.front();
      obj_free.pop_front();
    }

    *obj = obj_zero;

    obj->adjust_string = str_dup("");
    VALIDATE(obj);

    return obj;
  }

  void free_obj(OBJ_DATA *obj) {
    AFFECT_DATA *paf, *paf_next;
    EXTRA_DESCR_DATA *ed, *ed_next;

    if (!IS_VALID(obj))
    return;

    for (paf = obj->affected; paf != NULL; paf = paf_next) {
      paf_next = paf->next;
      free_affect(paf);
    }
    obj->affected = NULL;

    for (ed = obj->extra_descr; ed != NULL; ed = ed_next) {
      ed_next = ed->next;
      free_extra_descr(ed);
    }
    obj->extra_descr = NULL;

    // Uncommented short, adjust, and desc - Discordance
    free_string(obj->name);
    free_string(obj->description);
    free_string(obj->short_descr);
    free_string(obj->wear_string);
    free_string(obj->wear_temp);
    free_string(obj->adjust_string);
    free_string(obj->owner);

    INVALIDATE(obj);

    obj_free.push_front(obj);
  }

  /* stuff for recyling characters */
  CharList char_free;

  CHAR_DATA *new_char(void) {
    static CHAR_DATA ch_zero;
    CHAR_DATA *ch;

    if (char_free.empty())
    ch = (CHAR_DATA *)alloc_perm(sizeof(*ch));
    else {
      ch = char_free.front();
      char_free.pop_front();
    }

    *ch = ch_zero;

    /* Set all the default values for the ch struct */
    set_ch_default(ch);
    VALIDATE(ch);

    return ch;
  }

  void free_char(CHAR_DATA *ch) {
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;

    if (!IS_VALID(ch))
    return;

    if (IS_NPC(ch))
    mobile_count--;

    for (obj = ch->carrying; obj != NULL; obj = obj_next) {
      obj_next = obj->next_content;
      extract_obj_silent(obj);
    }

    for (paf = ch->affected; paf != NULL; paf = paf_next) {
      paf_next = paf->next;
      affect_remove(ch, paf);
    }

    free_string(ch->name);
    free_string(ch->short_descr);
    free_string(ch->long_descr);
    free_string(ch->description);
    free_string(ch->prompt);
    free_string(ch->last_ip);
    free_string(ch->aggression);
    free_string(ch->protecting);
    free_string(ch->clan);
    free_string(ch->prefix);

    free_string(ch->ordertarget);
    free_string(ch->qmove);
    free_string(ch->amove);
    free_string(ch->abilmove);


    free_pcdata(ch->pcdata);

    char_free.push_front(ch);

    INVALIDATE(ch);
    return;
  }

  PC_DATA *new_pcdata(void) {
    PC_DATA *pcdata;

    pcdata = (PC_DATA *)alloc_mem(sizeof(*pcdata));

    set_pcdata_default(pcdata);

    VALIDATE(pcdata);
    return pcdata;
  }

  void free_pcdata(PC_DATA *pcdata) {
    int alias;

    if (!IS_VALID(pcdata))
    return;

    delete pcdata->buffer;
    free_string(pcdata->last_name);
    free_string(pcdata->verbal_color);
    free_string(pcdata->remember_detail);
    free_string(pcdata->enthralling);
    free_string(pcdata->enrapturing);
    free_string(pcdata->enthralled);
    free_string(pcdata->enraptured);
    free_string(pcdata->privatepartner);
    free_string(pcdata->enlinked);
    free_string(pcdata->account_name);
    free_string(pcdata->storyrunner);
    free_string(pcdata->guest_of);
    free_string(pcdata->pledge);
    free_string(pcdata->pubic_hair);
    free_string(pcdata->messages);
    free_string(pcdata->class_fame);
    free_string(pcdata->home_territory);
    free_string(pcdata->deathcause);
    free_string(pcdata->place);
    free_string(pcdata->last_emote);
    free_string(pcdata->last_newbie);
    free_string(pcdata->nightmare);
    free_string(pcdata->pinned_by);
    free_string(pcdata->scent);
    free_string(pcdata->taste);
    free_string(pcdata->pwd);
    free_string(pcdata->upwd);
    free_string(pcdata->bamfin);
    free_string(pcdata->mentor);
    free_string(pcdata->fame);
    free_string(pcdata->art1);
    free_string(pcdata->art2);
    free_string(pcdata->art3);
    free_string(pcdata->process_argumentone);
    free_string(pcdata->process_argumenttwo);
    free_string(pcdata->bamfout);
    free_string(pcdata->whotitle);
    free_string(pcdata->ftime);
    free_string(pcdata->floc);
    free_string(pcdata->fdesc);
    free_string(pcdata->file);
    free_string(pcdata->history);
    free_string(pcdata->classified);

    free_string(pcdata->eidilon_ambient);
    free_string(pcdata->implant_dream);
    free_string(pcdata->luck_string_one);
    free_string(pcdata->luck_string_two);
    free_string(pcdata->attempt_emote);
    free_string(pcdata->ff_knowledge);
    free_string(pcdata->ff_secret);
    free_string(pcdata->enc_prompt_one);
    free_string(pcdata->enc_prompt_two);
    free_string(pcdata->enc_prompt_three);

    free_string(pcdata->operative_core);
    free_string(pcdata->operative_sect);
    free_string(pcdata->operative_cult);
    free_string(pcdata->eyes);
    free_string(pcdata->hair);
    free_string(pcdata->skin);



    free_string(pcdata->title);
    free_string(pcdata->email); /* NEW */

    free_string(pcdata->aiding_thwart);

    free_string(pcdata->dream_environment);
    free_string(pcdata->dream_intro);
    free_string(pcdata->dream_description);

    free_string(pcdata->mask_intro_one);
    free_string(pcdata->mask_intro_two);

    free_string(pcdata->hair_dyed);

    free_string(pcdata->editing_territory);
    free_string(pcdata->understanding);

    free_string(pcdata->private_security);
    free_string(pcdata->wolfdesc);
    free_string(pcdata->wolfintro);
    free_string(pcdata->mermaiddesc);
    free_string(pcdata->mermaidintro);
    free_string(pcdata->fishmanintro);

    free_string(pcdata->job_title_one);
    free_string(pcdata->job_title_two);

    free_string(pcdata->building_fixation_name);
    free_string(pcdata->fixation_name);

    free_string(pcdata->diss_target);
    free_string(pcdata->diss_message);
    free_string(pcdata->survey_comment);
    free_string(pcdata->survey_improve);
    free_string(pcdata->surveying);

    free_string(pcdata->sex_risk);
    free_string(pcdata->sex_type);

    free_string(pcdata->cdisc_name);
    free_string(pcdata->chat_history);


    for (alias = 0; alias < 3; alias++) {
      free_string(pcdata->last_sexed[alias]);
    }

    pcdata->account = NULL;
    for (alias = 0; alias < 10; alias++) {
      free_string(pcdata->drivenames[alias]);
    }
    for (alias = 0; alias < 6; alias++) {
      free_string(pcdata->animal_names[alias]);
      free_string(pcdata->animal_intros[alias]);
      free_string(pcdata->animal_change_to[alias]);
      free_string(pcdata->animal_change_from[alias]);
      free_string(pcdata->animal_descs[alias]);
      free_string(pcdata->animal_species[alias]);
    }
    for (alias = 0; alias < 10; alias++) {
      free_string(pcdata->garage_name[alias]);
      free_string(pcdata->garage_desc[alias]);
      free_string(pcdata->garage_lplate[alias]);
    }
    for (alias = 0; alias < 20; alias++) {
      free_string(pcdata->contact_names[alias]);
      free_string(pcdata->contact_descs[alias]);
    }
    free_string(pcdata->mermaid_change_to);
    free_string(pcdata->mermaid_change_from);
    free_string(pcdata->wolf_change_to);
    free_string(pcdata->wolf_change_from);
    free_string(pcdata->radio_action);
    free_string(pcdata->makeup_light);
    free_string(pcdata->makeup_medium);
    free_string(pcdata->makeup_heavy);

    free_string(pcdata->curse_text);
    // nowhere sex based ban
    pcdata->nowhere_male = 0;
    pcdata->nowhere_female = 0;
    for (alias = 0; alias < 50; alias++) {
      free_string(pcdata->ignored_characters[alias]); // ignored characters
      free_string(pcdata->ignored_accounts[alias]);   // ignored accounts
      free_string(pcdata->nowhere_characters[alias]); // nowhere characters
      free_string(pcdata->nowhere_accounts[alias]);   // nowhere accounts
      free_string(pcdata->file_message[alias]);
    }

    for (alias = 0; alias < 5; alias++)
    free_string(pcdata->bonds[alias]);

    for (alias = 0; alias < 10; alias++)
    free_string(pcdata->relationship[alias]);

    for (alias = 0; alias < 50; alias++) {
      free_string(pcdata->speed_names[alias]);
    }
    for (alias = 0; alias < 10; alias++) {
      free_string(pcdata->attention_history[alias]);
      free_string(pcdata->chan_names[alias]);
    }
    free_string(pcdata->brainwash_loyalty);
    free_string(pcdata->brainwash_reidentity);
    free_string(pcdata->maim);

    free_string(pcdata->feedback);
    for (alias = 0; alias < 10; alias++) {
      free_string(pcdata->feedbackinc[alias]);
    }
    free_string(pcdata->chat_handle);
    for (alias = 0; alias < 5; alias++) {
      free_string(pcdata->monster_names[0][alias]);
      free_string(pcdata->monster_names[1][alias]);
      free_string(pcdata->monster_names[2][alias]);
    }
    for (alias = 0; alias < 25; alias++) {
      free_string(pcdata->imprint[alias]);
      free_string(pcdata->memories[alias]);
      free_string(pcdata->repressions[alias]);
      free_string(pcdata->imprint_trigger[alias]);
    }
    for (alias = 0; alias < 300; alias++)
    free_string(pcdata->stat_log_string[alias]);

    free_string(pcdata->char_goals);
    free_string(pcdata->char_fears);
    free_string(pcdata->char_secrets);
    free_string(pcdata->char_timeline);

    free_string(pcdata->wilds_fame);
    free_string(pcdata->other_fame);
    free_string(pcdata->godrealm_fame);
    free_string(pcdata->hell_fame);

    free_string(pcdata->demon_pact);
    free_string(pcdata->persuade_message);
    free_string(pcdata->villain_praise);
    free_string(pcdata->villain_message);
    free_string(pcdata->brander);
    free_string(pcdata->brandstring);
    free_string(pcdata->last_bitten);
    free_string(pcdata->roster_description);
    free_string(pcdata->roster_shortdesc);
    free_string(pcdata->encounter_storyline);
    free_string(pcdata->encounter_bringin);
    free_string(pcdata->finale_location);
    free_string(pcdata->speaking);
    free_string(pcdata->dream_origin);
    free_string(pcdata->doom_desc);
    free_string(pcdata->doom_custom);
    free_string(pcdata->dreamplace);
    free_string(pcdata->dreamtitle);
    free_string(pcdata->murder_name);
    free_string(pcdata->ritual_dreamworld);
    free_string(pcdata->dream_identity);
    free_string(pcdata->identity_world);
    free_string(pcdata->deluded_reason);
    free_string(pcdata->last_villain_mod);

    free_string(pcdata->last_ritual);
    free_string(pcdata->ritual_maintainer);
    free_string(pcdata->maintained_target);
    pcdata->dream_identity_timer = 0;
    for(int x = 0;x < 4;x++)
    free_string(pcdata->mark[x]);
    free_string(pcdata->haunter);
    free_string(pcdata->order_target);
    free_string(pcdata->tailing);
    free_string(pcdata->text_msgs);
    free_string(pcdata->photo_msgs);
    free_string(pcdata->photo_pose);
    free_string(pcdata->talk);
    free_string(pcdata->caura);
    free_string(pcdata->cwalk);
    free_string(pcdata->crank);

    free_string(pcdata->ci_name);
    free_string(pcdata->ci_short);
    free_string(pcdata->ci_long);
    free_string(pcdata->ci_taste);
    free_string(pcdata->ci_imprint);
    free_string(pcdata->ci_wear);
    free_string(pcdata->ci_desc);
    free_string(pcdata->ci_myself);
    free_string(pcdata->ci_target);
    free_string(pcdata->ci_bystanders);
    free_string(pcdata->ci_myselfdelayed);
    free_string(pcdata->ci_bystandersdelayed);
    free_string(pcdata->ci_message);




    for (int x = 0; x < 20; x++)
    free_string(pcdata->rp_log[x]);

    for (int x = 0; x < 5; x++)
    free_string(pcdata->languages[x]);

    for (int x = 0; x < MAX_COVERS + 10; x++) {
      free_string(pcdata->focused_descs[x]);
      free_string(pcdata->detail_under[x]);
      free_string(pcdata->detail_over[x]);
      free_string(pcdata->scars[x]);
    }
    for (int x = 0; x < 3; x++) {
      free_string(pcdata->last_praise_message[x]);
      free_string(pcdata->last_diss_message[x]);
      free_string(pcdata->last_praised[x]);
      free_string(pcdata->last_dissed[x]);
    }
    free_string(pcdata->last_rumor);
    for (alias = 0; alias < 90; alias++) {
      free_string(pcdata->trust_names[alias]);
    }
    for (alias = 0; alias < MAX_ALIAS; alias++) {
      free_string(pcdata->alias[alias]);
      free_string(pcdata->alias_sub[alias]);
    }
    for (int x = 0; x < 25; x++) {
      for (int y = 0; y < 7; y++)
      free_string(pcdata->customstrings[x][y]);
    }

    // NameMaps
    for (NameMap::iterator it = pcdata->male_names->begin();
    it != pcdata->male_names->end(); ++it) {
      free_string(it->second);
    }

    for (NameMap::iterator it = pcdata->female_names->begin();
    it != pcdata->female_names->end(); ++it) {
      free_string(it->second);
    }

    // Free all the bond structs

    // Free all student structs
    delete pcdata->male_names;
    delete pcdata->female_names;
    free_string(pcdata->implant_dream);
    free_string(pcdata->eidilon_ambient);
    free_string(pcdata->intro_desc);
    free_string(pcdata->ff_knowledge);
    free_string(pcdata->ff_secret);
    pcdata->pledge_stealth = 0;
    pcdata->offering = NULL;
    pcdata->offer_type = 0;
    pcdata->bond_offer = NULL;
    pcdata->blood_date = 0;
    pcdata->blood_level = 0;
    pcdata->bond_drop = 0;
    pcdata->vbond = 0;
    pcdata->clan_offering = NULL;
    pcdata->clan_position = -10;
    pcdata->clan_bonus = 0;
    pcdata->payscale = 100;
    pcdata->racial_power = 0;
    pcdata->racial_power_two = 0;
    if (pcdata->pNote)
    delete pcdata->pNote;

    pcdata->encounter_storyline_added = FALSE;
    pcdata->encounter_mastermind_added = FALSE;
    pcdata->encounter_status = 0;
    pcdata->encounter_countdown = 0;
    pcdata->encounter_orig_room = 0;
    pcdata->encounter_number = 0;
    pcdata->karma_battery = 0;
    pcdata->genesis_stage = 0;
    pcdata->pathtotal = 0;

    pcdata->escape_timer = 0;
    pcdata->enlinktime = 0;
    pcdata->bust = 0;
    pcdata->market_visit = 0;
    pcdata->gen_stage = 0;
    pcdata->tier_raised = 0;
    pcdata->hell_power = 0;
    pcdata->godrealm_power = 0;
    pcdata->other_power = 0;
    pcdata->timenotalone = 0;
    pcdata->lastnotalone = 0;
    pcdata->weakness_status = 0;
    pcdata->weakness_timer = 0;
    pcdata->victimize_difficult_time = 0;
    for (int x = 0; x < 20; x++) {
      pcdata->victimize_history[x] = 0;
      pcdata->outfit_nickpoint[x] = 0;
      free_string(pcdata->outfit_nicknames[x]);
    }
    for (int x = 0; x < 50; x++) {
      pcdata->narrative_give[x] = 0;
      pcdata->narrative_want[x] = 0;
    }

    pcdata->victimize_char_point = NULL;
    pcdata->victimize_vic_point = NULL;
    pcdata->prep_target = NULL;
    pcdata->prep_action = 0;
    pcdata->destiny_feature = 0;
    pcdata->destiny_feature_room = 0;
    pcdata->destiny_offer = 0;
    pcdata->scene_offer = 0;
    pcdata->scene_vote = 0;
    pcdata->scene_offer_role = 0;
    pcdata->destiny_offer_role = 0;
    pcdata->victimize_vic_timer = 0;
    pcdata->victimize_char_select = 0;
    pcdata->victimize_vic_response_to = 0;
    pcdata->victimize_vic_select = 0;
    pcdata->victimize_difficult_count = 0;
    pcdata->victimize_vic_pending = 0;
    pcdata->victimize_lf_pending_loss = 0;
    pcdata->victimize_pending_lf_gain = 0;
    pcdata->victimize_last_response = 0;
    pcdata->stasis_spent_pkarma = 0;
    pcdata->stasis_spent_exp = 0;
    pcdata->stasis_spent_rpexp = 0;
    pcdata->stasis_spent_karma = 0;
    pcdata->last_dreamworld = 0;
    pcdata->dream_timer = 0;
    pcdata->patrol_rp = 0;
    pcdata->combat_emoted = FALSE;
    pcdata->dreamoutfit = 0;
    pcdata->dreamfromoutfit = 0;
    pcdata->rp_logging = 0;
    pcdata->nightmare_dragged = 0;
    pcdata->last_rp_room = 0;
    pcdata->availability = 0;
    pcdata->watching = 0;
    pcdata->truepenis = 0;
    pcdata->heroic = 0;
    pcdata->truebreasts = 0;
    pcdata->maintain_cost = 0;
    pcdata->sexchange_time = 0;
    pcdata->maintained_ritual = 0;
    pcdata->penis = 70;
    pcdata->wilds_fame_level = 0;
    pcdata->wilds_legendary = 0;
    pcdata->other_fame_level = 0;
    pcdata->other_legendary = 0;
    pcdata->hell_fame_level = 0;
    pcdata->hell_legendary = 0;
    pcdata->nightmare_shifted = 0;
    pcdata->deluded_cost = 0;
    pcdata->deluded_duration = 0;
    pcdata->mentor_of = 0;
    pcdata->solitary_time = 0;
    pcdata->dream_exit = -1;
    pcdata->dream_door = -1;
    pcdata->institute_action = 0;
    pcdata->psychic_attempts = 0;
    pcdata->psychic_proof = 0;
    pcdata->stasis_time = 0;
    pcdata->summon_bound = 0;
    pcdata->secret_days = 0;
    pcdata->secret_recover = 0;
    pcdata->intel = 0;
    pcdata->last_intel = 0;
    pcdata->in_domain = 0;
    pcdata->pending_resources = 0;
    pcdata->recent_exp = 0;
    pcdata->op_emotes = 0;
    pcdata->destiny_reject = 0;
    pcdata->destiny_reject_cooldown = 0;
    pcdata->bloodstorm_resist = 0;
    pcdata->domain_timer = 0;
    pcdata->spent_resources = 0;
    pcdata->sleeping = 0;
    pcdata->earned_karma = 0;
    pcdata->police_catch_timer = 0;
    pcdata->captive_timer = 0;
    pcdata->desclock = 0;
    pcdata->poisontimer = 0;
    pcdata->rohyptimer = 0;
    pcdata->relapsewerewolf = 0;
    pcdata->relapsevampire = 0;
    pcdata->last_paid = 0;
    pcdata->became_tyrant = 0;
    pcdata->monster_fed = 800;
    pcdata->wetness = 0;
    pcdata->true_id = 0;
    pcdata->icooldown = 0;
    pcdata->rpainamount = 0;
    pcdata->tcooldown = 0;
    pcdata->timesincedamage = 0;
    pcdata->class_type = 0;
    pcdata->vbloodcool = 0;
    pcdata->healthtimer = 0;
    pcdata->hangoutone = 0;
    pcdata->hangouttwo = 0;
    pcdata->hangouttemp = 0;
    pcdata->class_neutral = 0;
    pcdata->tboosts = 0;
    pcdata->blackout_cooldown = 0;
    pcdata->deputy_cooldown = 0;
    pcdata->disperse_timer = 0;
    pcdata->disperse_room = 0;
    pcdata->movealong_timer = 0;
    pcdata->movealong_vnum = 0;
    pcdata->last_outside_fullmoon = 0;
    pcdata->last_drained_person = 0;
    pcdata->divine_focus = 0;
    pcdata->extra_focus = 0;
    pcdata->bonus_origin = 0;
    pcdata->class_faction = 0;
    pcdata->class_cooldown = 0;
    pcdata->classpoints = 0;
    pcdata->deepshroud = 0;
    pcdata->superlocked = 0;
    pcdata->translocked = 0;
    pcdata->medic_uses = 0;
    pcdata->experiment_uses = 0;
    pcdata->classtotal = 0;
    pcdata->blackeyes = 0;
    pcdata->implant_frequency = 0;
    pcdata->paranoid = 0;
    pcdata->eidilon_of = 0;
    pcdata->wander_time = 0;
    pcdata->crowded_room = FALSE;
    pcdata->last_victim_bled = 0;
    pcdata->last_imbue = 0;
    pcdata->ftype = 0;
    pcdata->blackeyes_power = 0;
    pcdata->fleshformed = 0;
    pcdata->apower = 0;
    pcdata->lastaccident = 0;
    pcdata->lastshift = 0;
    pcdata->lunacy_curse = 0;
    pcdata->lastnormal = 0;
    pcdata->lastidentity = 0;
    pcdata->mimic = 0;

    pcdata->decay_stage = 0;
    pcdata->decay_timer = 0;
    pcdata->max_decay_timer = 0;

    pcdata->class_oldfaction = 0;
    pcdata->class_oldlevel = 0;

    // Institute
    pcdata->institute_contraband_violations = 0;
    pcdata->clique_role = 0;
    pcdata->clique_rolechange = 0;

    pcdata->wexp = 0;
    pcdata->oexp = 0;
    pcdata->gexp = 0;
    pcdata->hexp = 0;
    pcdata->aexp = 0;
    pcdata->dexp = 0;
    pcdata->awins = 0;
    pcdata->alosses = 0;
    pcdata->astatus = 0;

    pcdata->boobjob = 0;

    pcdata->extra_powers = 0;

    pcdata->scheme_influence = 0;
    pcdata->faction_influence = 0;
    pcdata->super_influence = 0;
    pcdata->aiding_scheme = 0;

    pcdata->resident = 0;
    pcdata->last_inhaven = 0;
    pcdata->last_page = 0;
    for (int x = 0; x < 10; x++) {
      pcdata->destiny_lockout_time[x] = 0;
      free_string(pcdata->destiny_lockout_char[x]);
      pcdata->destiny_in_one[x] = 0;
      pcdata->destiny_in_two[x] = 0;
      pcdata->destiny_stage_one[x] = 0;
      pcdata->destiny_stage_two[x] = 0;
      free_string(pcdata->destiny_in_with_one[x]);
      free_string(pcdata->destiny_in_with_two[x]);
    }
    for (int x = 0; x < 50; x++) {
      pcdata->destiny_interest_one[x] = 0;
      pcdata->destiny_interest_two[x] = 0;
      pcdata->destiny_block_one[x] = 0;
      pcdata->destiny_block_two[x] = 0;
    }

    for (int x = 0; x < 20; x++) {
      pcdata->ci_exclude_type[x] = 0;
      free_string(pcdata->ci_excludes[x]);
      pcdata->ci_include_type[x] = 0;
      free_string(pcdata->ci_includes[x]);
    }
    for (int x = 0; x < MAX_COVERS; x++) {
      pcdata->dirt[x] = 0;
      pcdata->blood[x] = 0;
      pcdata->exposed[x] = 0;
    }
    for (int x = 0; x < 7; x++) {
      pcdata->sins[x] = 0;
    }
    for (int x = 0; x < 50; x++) {
      pcdata->known_gateways[x] = 0;
    }
    for (int x = 0; x < 20; x++) {
      pcdata->attract[x] = 0;
    }
    for (int x = 0; x < 10; x++)
    pcdata->relationship_type[x] = 0;
    for (int x = 0; x < 15; x++)
    pcdata->survey_ratings[x] = 0;
    pcdata->survey_stage = 0;
    pcdata->survey_delay = 0;
    pcdata->diss_sendable = 0;

    pcdata->packcooldown = 0;
    pcdata->teachercooldown = 0;

    if (pcdata->lastReadTimes)
    delete pcdata->lastReadTimes;

    if (pcdata->noteText != NULL) {
      log_string(pcdata->noteText);
      free_string(pcdata->noteText);
    }

    INVALIDATE(pcdata);

    free_mem(pcdata, sizeof(*pcdata));

    return;
  }

  /* stuff for setting ids */
  long last_pc_id;
  long last_mob_id;

  long get_pc_id(void) {
    int val;

    val = (current_time <= last_pc_id) ? last_pc_id + 1 : current_time;
    last_pc_id = val;
    return val;
  }

  long get_mob_id(void) {
    last_mob_id++;
    return last_mob_id;
  }

  /* stuff for recycling mobprograms */
  PROG_LIST *mprog_free;
  PROG_LIST *oprog_free;
  PROG_LIST *rprog_free;

  PROG_LIST *new_mprog(void) {
    static PROG_LIST mp_zero;
    PROG_LIST *mp;

    if (mprog_free == NULL)
    mp = (PROG_LIST *)alloc_perm(sizeof(*mp));
    else {
      mp = mprog_free;
      mprog_free = mprog_free->next;
    }

    *mp = mp_zero;
    mp->vnum = 0;
    mp->trig_type = 0;
    mp->code = str_dup("");
    VALIDATE(mp);
    return mp;
  }

  void free_mprog(PROG_LIST *mp) {
    if (!IS_VALID(mp))
    return;

    INVALIDATE(mp);
    mp->next = mprog_free;
    mprog_free = mp;
  }

  HELP_AREA *had_free;

  HELP_AREA *new_had(void) {
    HELP_AREA *had;
    static HELP_AREA zHad;

    if (had_free) {
      had = had_free;
      had_free = had_free->next;
    }
    else
    had = (HELP_AREA *)alloc_perm(sizeof(*had));

    *had = zHad;

    return had;
  }

  HELP_DATA *help_free;

  HELP_DATA *new_help(void) {
    HELP_DATA *NewHelp;

    NewHelp = (HELP_DATA *)alloc_perm(sizeof(*NewHelp));
    NewHelp->level = 0;
    NewHelp->keyword = str_dup("");
    NewHelp->text = str_dup("");
    NewHelp->see_also = str_dup("");
    NewHelp->type = 0;
    NewHelp->del = FALSE;
    NewHelp->online = FALSE;
    NewHelp->next = NULL;
    NewHelp->see_also = str_dup("");
    NewHelp->type = 0;

    return NewHelp;
  }

  void free_help(HELP_DATA *help) {
    free_string(help->keyword);
    free_string(help->text);
    help->next = help_free;
    help_free = help;
  }

  PROG_LIST *new_oprog(void) {
    static PROG_LIST op_zero;
    PROG_LIST *op;

    if (oprog_free == NULL)
    op = (PROG_LIST *)alloc_perm(sizeof(*op));
    else {
      op = oprog_free;
      oprog_free = oprog_free->next;
    }

    *op = op_zero;
    op->vnum = 0;
    op->trig_type = 0;
    op->code = str_dup("");
    VALIDATE(op);
    return op;
  }

  void free_oprog(PROG_LIST *op) {
    if (!IS_VALID(op))
    return;

    INVALIDATE(op);
    op->next = oprog_free;
    oprog_free = op;
  }

  PROG_LIST *new_rprog(void) {
    static PROG_LIST rp_zero;
    PROG_LIST *rp;

    if (rprog_free == NULL)
    rp = (PROG_LIST *)alloc_perm(sizeof(*rp));
    else {
      rp = rprog_free;
      rprog_free = rprog_free->next;
    }

    *rp = rp_zero;
    rp->vnum = 0;
    rp->trig_type = 0;
    rp->code = str_dup("");
    VALIDATE(rp);
    return rp;
  }

  void free_rprog(PROG_LIST *rp) {
    if (!IS_VALID(rp))
    return;

    INVALIDATE(rp);
    rp->next = rprog_free;
    rprog_free = rp;
  }

  DUNGEON_TYPE *dungeon_free;

  DUNGEON_TYPE *new_dungeon(void) {
    int i;

    static DUNGEON_TYPE dungeon_zero;
    DUNGEON_TYPE *dungeon;

    dungeon = (DUNGEON_TYPE *)alloc_perm(sizeof(*dungeon));

    *dungeon = dungeon_zero;
    dungeon->type = 0;
    dungeon->target = 0;
    dungeon->align = 0;
    dungeon->size = 0;
    dungeon->minion_1 = str_dup("");
    dungeon->minion_2 = str_dup("");
    dungeon->level = 0;
    dungeon->end_room = 0;
    dungeon->start_vnum = 0;
    dungeon->boss = NULL;
    for (i = 0; i < 60; i++) {
      dungeon->traps[i] = NULL;
      dungeon->trap_info[i][0] = 0;
      dungeon->trap_info[i][1] = 0;
      dungeon->trap_info[i][2] = 0;
      dungeon->trap_info[i][3] = 0;
      dungeon->trap_info[i][4] = 0;
    }
    for (i = 0; i < 30; i++)
    dungeon->terminal_rooms[i] = 0;
    for (i = 0; i < 50; i++) {
      dungeon->mobs[i] = NULL;
    }
    for (i = 0; i < 20; i++) {
      dungeon->buttons[i] = NULL;
      dungeon->button_info[i][0] = 0;
      dungeon->button_info[i][1] = 0;
    }

    VALIDATE(dungeon);

    return dungeon;
  }

  void free_dungeon(DUNGEON_TYPE *dungeon) {

    if (!IS_VALID(dungeon))
    return;

    INVALIDATE(dungeon);
    free_string(dungeon->minion_1);
    free_string(dungeon->minion_2);
  }

  STORY_TYPE *story_free;

  STORY_TYPE *new_story(void) {
    static STORY_TYPE story_zero;
    STORY_TYPE *story;

    story = (STORY_TYPE *)alloc_perm(sizeof(*story));

    *story = story_zero;

    story->author = str_dup("");
    story->type = str_dup("");
    story->location = str_dup("");
    story->stats = str_dup("");
    story->description = str_dup("");
    story->invite_list = str_dup("");
    story->hide_list = str_dup("");
    story->isprivate = 0;
    story->ctype = 0;
    story->security_ban_list = str_dup("");
    story->security_letin_list = str_dup("");
    story->advertised = 0;
    story->crisis = 0;
    story->isprotected = 0;
    story->dedication = 0;
    for (int i = 0; i < 50; i++)
    story->comments[i] = str_dup("");

    story->time = 0;

    VALIDATE(story);

    return story;
  }

  void free_story(STORY_TYPE *story) {
    if (!IS_VALID(story))
    return;

    INVALIDATE(story);

    free_string(story->author);
    free_string(story->type);
    free_string(story->location);
    free_string(story->stats);
    free_string(story->description);

    for (int i = 0; i < 50; i++)
    free_string(story->comments[i]);
  }

  STORYIDEA_TYPE *storyidea_free;

  STORYIDEA_TYPE *new_storyidea(void) {
    static STORYIDEA_TYPE story_zero;
    STORYIDEA_TYPE *story;

    story = (STORYIDEA_TYPE *)alloc_perm(sizeof(*story));

    *story = story_zero;

    story->author = str_dup("");
    story->description = str_dup("");
    story->mastermind = str_dup("");
    story->npcs = str_dup("");
    story->lore = str_dup("");
    story->secrets = str_dup("");
    story->territory = str_dup("");
    story->eidolon = str_dup("");
    story->members = str_dup("");
    story->claimant = str_dup("");
    story->storyline = str_dup("");
    for (int i = 0; i < 10; i++) {
      story->galleries[i] = str_dup("");
    }
    story->crisis = 0;
    story->type = 0;
    story->subfactionone = 0;
    story->subfactiontwo = 0;
    story->seeking_type = 0;
    story->finale = 0;
    story->status = 0;
    story->time_locked = 0;
    story->time_owned = 0;
    story->time_made = 0;

    VALIDATE(story);

    return story;
  }
  void free_storyidea(STORYIDEA_TYPE *story) {
    if (!IS_VALID(story))
    return;

    INVALIDATE(story);

    free_string(story->author);
    free_string(story->description);
    free_string(story->npcs);
    free_string(story->lore);
    free_string(story->secrets);
    free_string(story->claimant);
    free_string(story->mastermind);
    free_string(story->territory);
    free_string(story->members);
    free_string(story->storyline);
    for (int i = 0; i < 10; i++) {
      free_string(story->galleries[i]);
    }
  }

  WEEKLY_TYPE *weekly_free;

  WEEKLY_TYPE *new_weekly(void) {
    static WEEKLY_TYPE weekly_zero;
    WEEKLY_TYPE *weekly;

    weekly = (WEEKLY_TYPE *)alloc_perm(sizeof(*weekly));

    *weekly = weekly_zero;

    weekly->charname = str_dup("");
    weekly->surname = str_dup("");
    weekly->logon = current_time;
    VALIDATE(weekly);

    return weekly;
  }
  void free_weekly(WEEKLY_TYPE *weekly) {
    if (!IS_VALID(weekly))
    return;

    INVALIDATE(weekly);

    free_string(weekly->charname);
    free_string(weekly->surname);
  }

  PETITION_TYPE *petition_free;

  PETITION_TYPE *new_petition(void) {
    static PETITION_TYPE pet_zero;
    PETITION_TYPE *pet;

    pet = (PETITION_TYPE *)alloc_perm(sizeof(*pet));

    *pet = pet_zero;

    pet->author = str_dup("");
    pet->description = str_dup("");
    pet->account = str_dup("");
    pet->sentto = str_dup("");
    pet->status = 0;
    pet->timestamp = 0;
    pet->type = 0;
    pet->room = 0;
    pet->create_time = 0;
    VALIDATE(pet);

    return pet;
  }
  void free_petition(PETITION_TYPE *pet) {
    if (!IS_VALID(pet))
    return;

    INVALIDATE(pet);

    free_string(pet->author);
    free_string(pet->description);
    free_string(pet->account);
    free_string(pet->sentto);
  }

  OPERATION_TYPE *operation_free;

  OPERATION_TYPE *new_operation(void) {
    static OPERATION_TYPE op_zero;
    OPERATION_TYPE *op;

    op = (OPERATION_TYPE *)alloc_perm(sizeof(*op));

    *op = op_zero;
    op->author = str_dup("");
    op->territoryvnum = 0;
    op->adversary_type = 0;
    op->timeshifted = 0;
    op->adversary_name = str_dup("");
    op->elitestring = str_dup("");
    op->storyrunners = str_dup("");
    op->atmosphere = str_dup("");
    op->room_name = str_dup("");
    op->description = str_dup("");
    op->bag_name = str_dup("");
    op->upload_name = str_dup("");
    op->preferred = str_dup("");
    op->storyline = str_dup("");
    op->timeline = str_dup("");
    op->target = str_dup("");
    op->calseen = FALSE;
    for (int i = 0; i < 100; i++) {
      op->sign_up[i] = str_dup("");
      op->sign_up_type[i] = 0;
    }
    op->initdays = 0;
    op->spam = 0;
    op->goal = 0;
    op->type = 0;
    op->max_pcs = 0;
    op->speed = 0;
    op->battleground_number = 0;
    op->terrain = 0;
    op->size = 0;
    op->hour = 0;
    op->day = 0;
    op->faction = 0;
    op->bonus = 0;
    op->timer = 0;
    op->home_soldiers = 0;
    op->initial_power = 0;
    op->power = 0;
    op->waves = 0;
    op->upload = 0;
    op->home_uploads = 0;
    op->competition = 0;
    op->challenge = 0;

    for (int i = 0; i < 3; i++)
    op->upload_cooldown[i] = 0;
    for (int i = 0; i < 10; i++) {
      op->enrolled[i] = 0;
      op->factimers[i] = 0;
      op->poix[i] = 0;
      op->poiy[i] = 0;
      op->poitype[i] = 0;
      op->poifaction[i] = 0;
      op->poibg[i] = 0;
      op->soldiers[i] = 0;
      op->uploads[i] = 0;
    }

    VALIDATE(op);

    return op;
  }
  void free_operation(OPERATION_TYPE *op) {
    if (!IS_VALID(op))
    return;
    INVALIDATE(op);

    free_string(op->author);
    free_string(op->description);
    free_string(op->room_name);
    free_string(op->adversary_name);
    free_string(op->elitestring);
    free_string(op->storyrunners);
    free_string(op->atmosphere);
    free_string(op->bag_name);
    free_string(op->upload_name);
    free_string(op->storyline);
    free_string(op->preferred);
    free_string(op->timeline);
    for (int i = 0; i < 100; i++)
    free_string(op->sign_up[i]);
  }

  ACCOUNT_TYPE *account_free;

  ACCOUNT_TYPE *new_account(void) {
    static ACCOUNT_TYPE account_zero;
    ACCOUNT_TYPE *account;

    account = (ACCOUNT_TYPE *)alloc_perm(sizeof(*account));

    *account = account_zero;

    SET_INIT(account->flags);
    account->name = str_dup("");
    account->pwd = str_dup("");
    account->upwd = str_dup("");
    account->creation_ip = str_dup("");
    account->email = str_dup("");
    account->sr_history = str_dup("");
    account->last_scheme = 0;
    account->maxhours = 0;
    account->focusedchar = 0;
    account->focusedcount = 0;
    account->last_bribe = 0;
    account->last_bargain = 0;
    account->last_ghost = 0;
    account->lastnotalone = 0;
    account->lastshrine = 0;
    account->last_decurse = 0;
    account->last_war = 0;
    account->last_awar = 0;
    account->last_pred = 0;
    account->last_hunt = 0;
    account->unrewarded_donation = 0;
    account->factiontime = 0;
    account->newcharcool = 0;
    account->newcharcount = 3;
    account->paycooldown = 0;
    account->socialcooldown = 0;
    account->hightiercooldown = 0;
    account->tier_count = 0;
    account->total_count = 0;
    account->villain_cooldown = 0;
    account->villain_score = 0;
    account->villain_mod = 0;
    account->sparring_xp = 0;
    account->factioncooldown = 0;
    account->xp = 0;
    account->rpxp = 0;
    account->karma = 0;
    account->pkarma = 0;
    account->karmabank = 0;
    account->karmaearned = 0;
    account->encounter_karma = 0;
    account->adventure_karma = 0;
    account->mystery_karma = 0;
    account->ambiant_karma = 0;
    account->monster_karma = 0;
    account->misc_karma = 0;
    account->other_karma = 0;
    account->scheme_karma = 0;
    account->dream_karma = 0;
    account->offworld_karma = 0;
    account->world_books = 0;
    account->earth_books = 0;
    account->haven_books = 0;
    account->donated = 0;
    account->colours = 0;
    account->bandaids = 0;
    account->renames = 0;
    account->pkarmaspent = 0;
    account->daily_pvp_exp = 0;
    account->last_infuse_date = 0;
    account->award_karma = 0;
    account->lastfacchar = str_dup("");
    account->lasttierchar = str_dup("");
    account->sr_website = str_dup("");
    account->sr_info = str_dup("");
    account->sr_logs = str_dup("");
    account->sr_fdesc = str_dup("");
    account->sr_aexp = 0;
    account->awards = 0;
    account->award_progress = 0;
    account->storyidea_cooldown = 0;
    account->encounter_cooldown = 0;
    account->target_encounter_cooldown = 0;


    for (int i = 0; i < 25; i++) {
      account->friends[i] = str_dup("");
      account->friend_type[i] = 0;
    }
    account->pastcharacters = str_dup("");
    for (int i = 0; i < 25; i++) {
      account->characters[i] = str_dup("");
      account->char_stasis[i] = 0;
    }
    VALIDATE(account);

    return account;
  }

  void free_account(ACCOUNT_TYPE *account) {
    if (!IS_VALID(account))
    return;

    INVALIDATE(account);

    free_string(account->name);
    free_string(account->pwd);
    free_string(account->upwd);
    free_string(account->creation_ip);
    free_string(account->lastfacchar);
    free_string(account->lasttierchar);
    free_string(account->sr_website);
    free_string(account->sr_info);
    free_string(account->sr_logs);
    free_string(account->sr_fdesc);
    free_string(account->pastcharacters);
    free_string(account->sr_history);
    free_string(account->email);

    for (int i = 0; i < 25; i++) {
      free_string(account->friends[i]);
    }
    for (int i = 0; i < 25; i++)
    free_string(account->characters[i]);
  }

  LOCATION_TYPE *location_free;

  LOCATION_TYPE *new_location(void) {
    static LOCATION_TYPE location_zero;
    LOCATION_TYPE *location;

    location = (LOCATION_TYPE *)alloc_perm(sizeof(*location));

    *location = location_zero;

    location->name = str_dup("");
    location->notes = str_dup("");
    location->government = str_dup("");
    location->fixtures = str_dup("");
    location->people = str_dup("");
    location->place = str_dup("");
    location->timeline = str_dup("");
    location->status = 0;
    location->continent = 0;
    location->hand = 0;
    location->temple = 0;
    location->status_timer = 0;
    location->lockout = 0;
    location->timezone = 0;
    location->x = 0;
    location->y = 0;
    location->radius = 0;
    location->colour = 0;
    location->base_type = 0;
    location->battleclimate = 3;
    for (int i = 0; i < 20; i++)
    location->phil_amount[i] = 0;

    for (int i = 0; i < 10; i++) {
      location->other_name[i] = str_dup("");
      location->other_amount[i] = 0;
    }
    for (int i = 0; i < 20; i++) {
      location->plant_desc[i] = str_dup("");
      location->place_desc[i] = str_dup("");
    }
    location->base_desc_core = str_dup("");
    location->base_faction_core = 0;
    location->base_desc_cult = str_dup("");
    location->base_faction_cult = 0;
    location->base_desc_sect = str_dup("");
    location->base_faction_sect = 0;
    VALIDATE(location);

    return location;
  }

  void free_location(LOCATION_TYPE *location) {
    if (!IS_VALID(location))
    return;

    INVALIDATE(location);

    free_string(location->name);
    free_string(location->notes);
    free_string(location->government);
    free_string(location->fixtures);
    free_string(location->people);
    free_string(location->place);
    free_string(location->timeline);

    for (int i = 0; i < 5; i++)
    free_string(location->other_name[i]);
    for (int i = 0; i < 20; i++) {
      free_string(location->plant_desc[i]);
      free_string(location->place_desc[i]);
    }
    free_string(location->base_desc_core);
    free_string(location->base_desc_cult);
    free_string(location->base_desc_sect);
  }

  PLOT_TYPE *plot_free;

  PLOT_TYPE *new_plot(void) {
    static PLOT_TYPE story_zero;
    PLOT_TYPE *story;

    story = (PLOT_TYPE *)alloc_perm(sizeof(*story));

    *story = story_zero;

    story->author = str_dup("");
    story->title = str_dup("");
    story->mastermind = str_dup("");
    story->events = str_dup("");
    story->challenge = str_dup("");
    story->character = str_dup("");
    story->eidolon = str_dup("");
    story->storyrunners = str_dup("");
    story->immortals = str_dup("");
    story->territory = str_dup("");
    story->crisis = 0;
    story->type = 0;
    story->subfactionone = 0;
    story->subfactiontwo = 0;
    story->seeking_type = 0;
    story->sponsored = 0;
    story->earnedkarma = 0;
    story->karmacap = 0;
    story->time_made = 0;
    story->storyline = str_dup("");
    story->members = str_dup("");
    story->finale = 0;
    story->timezone = str_dup("");

    for (int i = 0; i < 30; i++) {
      story->participants[i] = str_dup("");
      story->comments[i] = str_dup("");
      story->rating[i] = 0;
    }
    for (int i = 0; i < 10; i++) {
      story->galleries[i] = str_dup("");
    }

    story->calendar = 0;
    story->size = 0;

    VALIDATE(story);

    return story;
  }

  void free_plot(PLOT_TYPE *story) {
    if (!IS_VALID(story))
    return;
    INVALIDATE(story);

    free_string(story->author);
    free_string(story->title);
    free_string(story->events);
    free_string(story->mastermind);
    free_string(story->challenge);
    free_string(story->character);
    free_string(story->territory);
    free_string(story->members);
    free_string(story->storyrunners);
    free_string(story->immortals);
    free_string(story->storyline);

    for (int i = 0; i < 30; i++) {
      free_string(story->participants[i]);
      free_string(story->comments[i]);
    }
    for (int i = 0; i < 10; i++) {
      free_string(story->galleries[i]);
    }
  }

  GATEWAY_TYPE *gateway_free;

  GATEWAY_TYPE *new_gateway(void) {
    static GATEWAY_TYPE gateway_zero;
    GATEWAY_TYPE *gateway;

    gateway = (GATEWAY_TYPE *)alloc_perm(sizeof(*gateway));

    *gateway = gateway_zero;

    gateway->havenroom = 0;
    gateway->worldroom = 0;
    gateway->world = 0;
    gateway->direction = 0;
    gateway->lastused = 0;
    VALIDATE(gateway);

    return gateway;
  }

  void free_gateway(GATEWAY_TYPE *gateway) {
    if (!IS_VALID(gateway))
    return;
    INVALIDATE(gateway);
  }

  FANTASY_TYPE *fantasy_free;

  FANTASY_TYPE *new_fantasy(void) {
    static FANTASY_TYPE fantasy_zero;
    FANTASY_TYPE *story;

    story = (FANTASY_TYPE *)alloc_perm(sizeof(*story));

    *story = fantasy_zero;

    story->author = str_dup("");
    story->name = str_dup("");
    story->description = str_dup("");
    story->domain = str_dup("");
    story->statcost = 0;
    story->since_highlight = 0;
    story->highlight_time = 0;
    story->emit = 0;
    story->porn = 0;
    story->stupid = 0;
    story->lastused = 0;
    story->expdebuff = 0;
    story->locked = 0;
    story->startingexp = 0;
    story->active = FALSE;
    story->godmode = FALSE;
    story->petitions = str_dup("");
    story->elevator = str_dup("");
    story->entrance = 0;
    for (int i = 0; i < 100; i++)
    story->rooms[i] = 0;
    for (int i = 0; i < 30; i++) {
      story->stats[i] = str_dup("");
      story->stat_desc[i] = str_dup("");
      story->stat_offense_power[i] = 0;
      story->stat_defense_power[i] = 0;
    }

    for (int i = 0; i < 20; i++) {
      story->roles[i] = str_dup("");
      story->role_desc[i] = str_dup("");
      story->role_cost[i] = 0;
      for (int j = 0; j < 30; j++)
      story->role_stats[i][j] = 0;
    }
    for (int i = 0; i < 100; i++) {
      story->ban_list[i] = str_dup("");
    }
    for (int i = 0; i < 200; i++) {
      story->participants[i] = str_dup("");
      story->participant_shorts[i] = str_dup("");
      story->participant_descs[i] = str_dup("");
      story->participant_names[i] = str_dup("");
      story->participant_fames[i] = str_dup("");
      story->participant_eq[i] = str_dup("");
      story->participant_blind[i] = FALSE;
      story->participant_noaction[i] = FALSE;
      story->participant_nomove[i] = FALSE;
      story->participant_trusted[i] = FALSE;
      story->participant_godmode[i] = FALSE;
      story->participant_role[i] = 0;
      story->participant_exp[i] = 0;
      story->participant_inroom[i] = 0;
      story->safe_room[i] = 0;
      story->exits[i] = 0;
      story->entrances[i] = 0;
      story->exit_name[i] = str_dup("");
      story->exit_alias[i] = str_dup("");
      for (int j = 0; j < 30; j++) {
        story->participant_stats[i][j] = 0;
      }
    }
    VALIDATE(story);

    return story;
  }
  void free_fantasy(FANTASY_TYPE *story) {
    if (!IS_VALID(story))
    return;
    INVALIDATE(story);
    free_string(story->author);
    free_string(story->name);
    free_string(story->description);
    free_string(story->petitions);
    free_string(story->elevator);
    free_string(story->domain);

    for (int i = 0; i < 30; i++) {
      free_string(story->stats[i]);
      free_string(story->stat_desc[i]);
    }
    for (int i = 0; i < 20; i++) {
      free_string(story->roles[i]);
      free_string(story->role_desc[i]);
    }
    for (int i = 0; i < 100; i++)
    free_string(story->ban_list[i]);
    for (int i = 0; i < 200; i++) {
      free_string(story->participants[i]);
      free_string(story->participant_shorts[i]);
      free_string(story->participant_descs[i]);
      free_string(story->exit_name[i]);
      free_string(story->exit_alias[i]);
      free_string(story->participant_names[i]);
      free_string(story->participant_fames[i]);
      free_string(story->participant_eq[i]);
    }
  }
  /*
struct grouptext_type
{
char * tname;
int last_msg;
char *history;
int pnumber[10];
bool valid;
};
*/
  GROUPTEXT_TYPE *grouptext_free;

  GROUPTEXT_TYPE *new_grouptext(void) {
    static GROUPTEXT_TYPE grouptext_zero;
    GROUPTEXT_TYPE *grouptext;

    grouptext = (GROUPTEXT_TYPE *)alloc_perm(sizeof(*grouptext));

    *grouptext = grouptext_zero;

    grouptext->tname = str_dup("");
    grouptext->history = str_dup("");
    grouptext->last_msg = 0;
    for (int i = 0; i < 20; i++) {
      grouptext->pnumber[i] = 0;
    }
    VALIDATE(grouptext);

    return grouptext;
  }

  /*
struct profile_type
{
char * name;
char * handle;
char * profile;
char * photo;
char * quote;
char * joke;
char * prefs;
int last_active;
int core_symbol;
int sect_symbol;
int cult_symbol;
int wealth;
int education;
int rating;
int rating_boost;
int rating_count;
int plus;
int premium;
bool valid;
};
*/

  PROFILE_TYPE *profile_free;

  PROFILE_TYPE *new_profile(void) {
    static PROFILE_TYPE profile_zero;
    PROFILE_TYPE *profile;

    profile = (PROFILE_TYPE *)alloc_perm(sizeof(*profile));

    *profile = profile_zero;

    profile->name = str_dup("");
    profile->handle = str_dup("");
    profile->profile = str_dup("");
    profile->display_handle = str_dup("");
    profile->photo = str_dup("");
    profile->quote = str_dup("");
    profile->joke = str_dup("");
    profile->prefs = str_dup("");
    profile->party_total = 0;
    profile->party_count = 0;
    profile->event_total = 0;
    profile->event_count = 0;
    profile->perf_total = 0;
    profile->last_logon = 0;
    profile->perf_count = 0;
    profile->party_boost = 0;
    profile->last_active = 0;
    profile->core_symbol = 0;
    profile->sect_symbol = 0;
    profile->cult_symbol = 0;
    profile->wealth = 0;
    profile->education = 0;
    profile->rating = 0;
    profile->rating_boost = 0;
    profile->rating_count = 0;
    profile->host_rating = 0;
    profile->plus = 0;
    profile->last_browsed = 0;
    profile->premium = 0;
    profile->influencer = 0;
    profile->grating_total = 0;
    profile->grating_count = 0;
    profile->prating_total = 0;
    profile->prating_count = 0;
    VALIDATE(profile);

    return profile;
  }

  /*
struct match_type
{
char * nameone;
char * nametwo;
int friendmatch_one;
int datematch_one;
int profmatch_one;
int friendmatch_two;
int datematch_two;
int profmatch_two;
int status_one;
int status_two;
int last_activity;
int last_prompt;
int last_msg_one_one;
int last_msg_one_two;
int last_msg_one_three;
int last_msg_two_one;
int last_msg_two_two;
int last_msg_two_three;
int premium_one;
int premium_two;
int camhack_one;
int camhack_two;
int score_one_manual_chat;
int score_two_manual_chat;
int score_one_auto_chat;
int score_two_auto_chat;
char * auto_chat_review;
int auto_score_when;
int score_one_manual_inperson;
int score_two_manual_inperson;
int chat_active_one;
int chat_active_two;
int rp_active_one;
int rp_active_two;
int chat_count;
int rp_count;
int last_chat_count;
int last_rp_count;
int last_rp_when;
int last_chat_when;
int failed_rp_one;
int failed_rp_two;
int failed_chat_one;
int failed_chat_two;
int success_rp_one;
int success_rp_two;
int success_chat_one;
int success_chat_two;
bool valid;
}
*/

  MATCH_TYPE *match_free;

  MATCH_TYPE *new_match(void) {
    static MATCH_TYPE match_zero;
    MATCH_TYPE *match;

    match = (MATCH_TYPE *)alloc_perm(sizeof(*match));

    *match = match_zero;

    match->nameone = str_dup("");
    match->nametwo = str_dup("");
    match->friendmatch_one = 0;
    match->datematch_one = 0;
    match->profmatch_one = 0;
    match->friendmatch_two = 0;
    match->datematch_two = 0;
    match->profmatch_two = 0;
    match->status_one = 0;
    match->status_two = 0;
    match->last_activity = 0;
    match->last_prompt = 0;
    match->last_msg_one_one = 0;
    match->last_msg_one_two = 0;
    match->last_msg_one_three = 0;
    match->last_msg_two_one = 0;
    match->last_msg_two_two = 0;
    match->last_msg_two_three = 0;
    match->premium_one = 0;
    match->premium_two = 0;
    match->camhack_one = 0;
    match->camhack_two = 0;
    match->score_one_manual_chat = 0;
    match->score_two_manual_chat = 0;
    match->score_one_auto_chat = 0;
    match->score_two_auto_chat = 0;
    match->auto_chat_review = str_dup("");
    match->auto_score_when = 0;
    match->score_one_manual_inperson = 0;
    match->score_two_manual_inperson = 0;
    match->chat_active_one = 0;
    match->chat_active_two = 0;
    match->rp_active_one = 0;
    match->rp_active_two = 0;
    match->chat_count = 0;
    match->rp_count = 0;
    match->last_chat_count = 0;
    match->last_rp_count = 0;
    match->last_rp_when = 0;
    match->last_chat_when = 0;
    match->failed_rp_one = 0;
    match->failed_rp_two = 0;
    match->failed_chat_one = 0;
    match->failed_chat_two = 0;
    match->success_rp_one = 0;
    match->success_rp_two = 0;
    match->success_chat_one = 0;
    match->success_chat_two = 0;
    match->chat_initiatior = 0;
    match->rp_initiatior = 0;
    match->rate_party_one = 0;
    match->rate_party_two = 0;
    match->party_one_title = str_dup("");
    match->party_two_title = str_dup("");
    match->rate_party_type_one = 0;
    match->rate_party_type_two = 0;

    VALIDATE(match);

    return match;
  }

  /*
struct chatroom_type
{
int vnum;
char * name;
char * description;
char * history;
int last_msg;
}
*/

  CHATROOM_TYPE *chatroom_free;

  CHATROOM_TYPE *new_chatroom(void) {
    static CHATROOM_TYPE chatroom_zero;
    CHATROOM_TYPE *chatroom;

    chatroom = (CHATROOM_TYPE *)alloc_perm(sizeof(*chatroom));

    *chatroom = chatroom_zero;

    chatroom->vnum = 0;
    chatroom->name = str_dup("");
    chatroom->description = str_dup("");
    chatroom->history = str_dup("");
    chatroom->last_msg = 0;
    VALIDATE(chatroom);

    return chatroom;
  }


  TEXTHISTORY_TYPE *texthistory_free;

  TEXTHISTORY_TYPE *new_texthistory(void) {
    static TEXTHISTORY_TYPE texthistory_zero;
    TEXTHISTORY_TYPE *texthistory;

    texthistory = (TEXTHISTORY_TYPE *)alloc_perm(sizeof(*texthistory));

    *texthistory = texthistory_zero;

    texthistory->name_one = str_dup("");
    texthistory->name_two = str_dup("");
    texthistory->history = str_dup("");
    texthistory->last_msg = 0;
    VALIDATE(texthistory);

    return texthistory;
  }

  NEWDESTINY_TYPE *destiny_free;

  NEWDESTINY_TYPE *new_destiny(void) {
    static NEWDESTINY_TYPE destiny_zero;
    NEWDESTINY_TYPE *story;

    story = (NEWDESTINY_TYPE *)alloc_perm(sizeof(*story));

    *story = destiny_zero;

    for (int i = 0; i < 20; i++) {
      story->scene_descs[i] = str_dup("");
      story->scene_location[i] = 0;
      story->scene_special_one[i] = 0;
      story->scene_special_two[i] = 0;
      story->scene_conclusion[i] = FALSE;
    }
    story->author = str_dup("");
    story->premise = str_dup("");
    story->vnum = 0;
    story->role_one_name = str_dup("");
    story->role_two_name = str_dup("");
    story->arch_one = 0;
    story->arch_two = 0;
    story->lastused = 0;
    story->created_at = 0;
    story->restrict_one = 0;
    story->restrict_two = 0;
    story->conclude_type = 0;
    story->isprivate = FALSE;

    VALIDATE(story);

    return story;
  }
  void free_destiny(NEWDESTINY_TYPE *story) {
    if (!IS_VALID(story))
    return;
    INVALIDATE(story);
    free_string(story->author);
    free_string(story->premise);
    free_string(story->role_one_name);
    free_string(story->role_two_name);
    for (int i = 0; i < 20; i++) {
      free_string(story->scene_descs[i]);
    }
  }

  EVENT_TYPE *event_free;

  EVENT_TYPE *new_event(void) {
    static EVENT_TYPE event_zero;
    EVENT_TYPE *event;

    event = (EVENT_TYPE *)alloc_perm(sizeof(*event));

    *event = event_zero;

    event->author = str_dup("");
    event->target = str_dup("");
    event->description = str_dup("");
    event->introduction = str_dup("");
    event->message = str_dup("");
    event->thwart_method = str_dup("");
    event->imprint = str_dup("");
    event->researched = str_dup("");
    event->account = str_dup("");
    event->storyline = str_dup("");
    event->coauthors = str_dup("");
    event->karma_battery_author = 0;
    event->karma_battery_storyrunner = 0;
    event->limited = 0;
    event->faction = 0;
    event->type = 0;
    event->type = 0;
    event->active_time = 0;
    event->deactive_time = 0;
    event->shown_news = 0;
    event->thwart_attempted = 0;
    event->nothwart = 0;
    VALIDATE(event);

    return event;
  }

  void free_event(EVENT_TYPE *event) {
    if (!IS_VALID(event))
    return;

    INVALIDATE(event);

    free_string(event->author);
    free_string(event->target);
    free_string(event->description);
    free_string(event->introduction);
    free_string(event->message);
    free_string(event->thwart_method);
    free_string(event->imprint);
    free_string(event->researched);
    free_string(event->account);
    free_string(event->storyline);
    free_string(event->coauthors);
  }

  RESEARCH_TYPE *research_free;

  RESEARCH_TYPE *new_research(void) {
    static RESEARCH_TYPE research_zero;
    RESEARCH_TYPE *research;

    research = (RESEARCH_TYPE *)alloc_perm(sizeof(*research));

    *research = research_zero;

    research->author = str_dup("");
    research->message = str_dup("");
    research->participants = str_dup("");

    for (int i = 0; i < 10; i++)
    research->stats[i] = 0;

    research->timer = 0;
    research->difficulty = 0;

    VALIDATE(research);

    return research;
  }
  void free_research(RESEARCH_TYPE *research) {
    if (!IS_VALID(research))
    return;
    INVALIDATE(research);

    free_string(research->author);
    free_string(research->message);
    free_string(research->participants);
  }


  EXTRA_ENCOUNTER_TYPE *encounter_free;

  EXTRA_ENCOUNTER_TYPE *new_eencounter(void) {
    static EXTRA_ENCOUNTER_TYPE enc_zero;
    EXTRA_ENCOUNTER_TYPE *enc;

    enc = (EXTRA_ENCOUNTER_TYPE *)alloc_perm(sizeof(*enc));

    *enc = enc_zero;

    enc->text = str_dup("");
    enc->btype = 0;
    enc->id = 0;
    enc->encounter_id = 0;
    enc->clan_id = 0;

    VALIDATE(enc);

    return enc;
  }
  void free_eencounter(EXTRA_ENCOUNTER_TYPE *enc) {
    if (!IS_VALID(enc))
    return;
    INVALIDATE(enc);

    free_string(enc->text);
  }


  EXTRA_PATROL_TYPE *patrol_free;

  EXTRA_PATROL_TYPE *new_epatrol(void) {
    static EXTRA_PATROL_TYPE pat_zero;
    EXTRA_PATROL_TYPE *pat;

    pat = (EXTRA_PATROL_TYPE *)alloc_perm(sizeof(*pat));

    *pat = pat_zero;

    pat->id = 0;
    pat->clan_id = 0;
    pat->intro = str_dup("");
    pat->conclusion = str_dup("");
    pat->messages = str_dup("");
    VALIDATE(pat);

    return pat;
  }
  void free_epatrol(EXTRA_PATROL_TYPE *pat) {
    if (!IS_VALID(pat))
    return;
    INVALIDATE(pat);

    free_string(pat->intro);
    free_string(pat->conclusion);
    free_string(pat->messages);
  }


  ANNIVERSARY_TYPE *anniversary_free;

  ANNIVERSARY_TYPE *new_anniversary(void) {
    static ANNIVERSARY_TYPE ann_zero;
    ANNIVERSARY_TYPE *ann;

    ann = (ANNIVERSARY_TYPE *)alloc_perm(sizeof(*ann));

    *ann = ann_zero;

    ann->id = 0;
    ann->btype = 0;
    ann->summary = str_dup("");
    ann->news = str_dup("");
    ann->eidilon_name = str_dup("");
    ann->cult_name = str_dup("");
    ann->sect_name = str_dup("");
    ann->messages = str_dup("");
    VALIDATE(ann);

    return ann;
  }
  void free_anniversary(ANNIVERSARY_TYPE *ann) {
    if (!IS_VALID(ann))
    return;
    INVALIDATE(ann);

    free_string(ann->summary);
    free_string(ann->news);
    free_string(ann->eidilon_name);
    free_string(ann->cult_name);
    free_string(ann->sect_name);
    free_string(ann->messages);
  }




  DECREE_TYPE *decree_free;

  DECREE_TYPE *new_decree(void) {
    static DECREE_TYPE dec_zero;
    DECREE_TYPE *dec;

    dec = (DECREE_TYPE *)alloc_perm(sizeof(*dec));

    *dec = dec_zero;

    dec->territory_vnum = 0;
    dec->btype = 0;
    dec->created_at = 0;
    dec->start_time = 0;
    dec->end_time = 0;
    dec->vote_1 = str_dup("");
    dec->vote_2 = str_dup("");
    dec->vote_3 = str_dup("");
    dec->desc = str_dup("");
    dec->target = str_dup("");
    dec->target_number = 0;

    VALIDATE(dec);

    return dec;
  }
  void free_decree(DECREE_TYPE *dec) {
    if (!IS_VALID(dec))
    return;
    INVALIDATE(dec);

    free_string(dec->vote_1);
    free_string(dec->vote_2);
    free_string(dec->vote_3);
    free_string(dec->desc);
    free_string(dec->target);

  }



  NEWS_TYPE *news_free;

  NEWS_TYPE *new_news(void) {
    static NEWS_TYPE news_zero;
    NEWS_TYPE *news;

    news = (NEWS_TYPE *)alloc_perm(sizeof(*news));

    *news = news_zero;

    news->author = str_dup("");
    news->message = str_dup("");

    for (int i = 0; i < 10; i++)
    news->stats[i] = 0;

    news->timer = 0;

    VALIDATE(news);

    return news;
  }
  void free_news(NEWS_TYPE *news) {
    if (!IS_VALID(news))
    return;
    INVALIDATE(news);

    free_string(news->author);
    free_string(news->message);
  }

  PLAYERROOM_TYPE *playerroom_free;

  PLAYERROOM_TYPE *new_playerroom(void) {
    static PLAYERROOM_TYPE playerroom_zero;
    PLAYERROOM_TYPE *playerroom;

    playerroom = (PLAYERROOM_TYPE *)alloc_perm(sizeof(*playerroom));

    *playerroom = playerroom_zero;

    playerroom->author = str_dup("");
    playerroom->cooldown = 0;
    playerroom->status = 0;
    playerroom->vnum = 0;
    playerroom->checked = FALSE;
    VALIDATE(playerroom);

    return playerroom;
  }

  void free_playerroom(PLAYERROOM_TYPE *playerroom) {
    if (!IS_VALID(playerroom))
    return;

    INVALIDATE(playerroom);

    free_string(playerroom->author);
  }

  HOUSE_TYPE *house_free;

  HOUSE_TYPE *new_house(void) {
    int i;

    static HOUSE_TYPE house_zero;
    HOUSE_TYPE *house;

    house = (HOUSE_TYPE *)alloc_perm(sizeof(*house));

    *house = house_zero;
    house->vnum = 0;
    house->house_name = str_dup("");
    house->lower_vnum = 0;
    house->higher_vnum = 0;
    house->transfer_point = 0;
    house->door = 0;
    house->price = 0;
    house->current_price = 0;
    house->auction_day = 0;
    house->auction_month = 0;
    house->owner = str_dup("");
    house->last_owner = str_dup("");
    house->address = str_dup("");
    house->faction = 0;
    house->toughness = 0;
    house->security = 0;
    house->guards = 0;
    house->blackout = 0;

    for (i = 0; i < 30; i++) {
      house->bid_names[i] = str_dup("");
      house->bid_amounts[i] = 0;
    }
    for (i = 0; i < 50; i++) {
      house->tenants[i] = str_dup("");
      house->tenant_trust[i] = 0;
    }

    VALIDATE(house);

    return house;
  }

  void free_house(HOUSE_TYPE *house) {
    int i;

    if (!IS_VALID(house))
    return;

    INVALIDATE(house);
    free_string(house->house_name);
    free_string(house->owner);
    free_string(house->last_owner);
    free_string(house->address);

    for (i = 0; i < 30; i++) {
      free_string(house->bid_names[i]);
    }
    for (i = 0; i < 50; i++) {
      free_string(house->tenants[i]);
    }
  }

  DOMAIN_TYPE *new_domain(void) {
    int i;

    static DOMAIN_TYPE domain_zero;
    DOMAIN_TYPE *domain;

    domain = (DOMAIN_TYPE *)alloc_perm(sizeof(*domain));

    *domain = domain_zero;
    domain->domain_of = str_dup("");
    domain->desc = str_dup("");
    domain->ambients = str_dup("");
    domain->vnum = 0;
    domain->archetype = 0;
    domain->power = 0;
    domain->exp = 0;
    for (i = 0; i < 250; i++) {
      domain->roomlist[i] = 0;
      domain->smallshrines[i] = 0;
      domain->medshrines[i] = 0;
      domain->bigshrines[i] = 0;
    }

    VALIDATE(domain);

    return domain;
  }
  void free_domain(DOMAIN_TYPE *domain) {
    if (!IS_VALID(domain))
    return;

    free_string(domain->domain_of);
    free_string(domain->desc);
    free_string(domain->ambients);

    INVALIDATE(domain);
  }

  PROP_TYPE *prop_free;

  PROP_TYPE *new_prop(void) {
    int i;

    static PROP_TYPE prop_zero;
    PROP_TYPE *prop;

    prop = (PROP_TYPE *)alloc_perm(sizeof(*prop));

    *prop = prop_zero;
    prop->type = 0;
    prop->industry = 0;
    prop->nochange = 0;
    prop->airfix = FALSE;
    prop->profit = 0;
    prop->bankrupt = 0;
    prop->compromised = 0;
    prop->renovate = 0;
    prop->electric = 0;
    prop->lightcount = -1;
    prop->realhouse = -1;
    prop->market_dir = 0;
    prop->market_room = 0;
    prop->sale_price = 0;
    prop->last_encroach = 0;
    prop->creditprofit = 0;
    prop->vnum = 0;
    prop->propname = str_dup("");
    prop->price = 0;
    prop->auction_day = 0;
    prop->auction_month = 0;
    prop->owner = str_dup("");
    prop->address = str_dup("");
    prop->advertisement = str_dup("");
    prop->reclaim = FALSE;
    prop->faction = 0;
    prop->industry = 0;
    prop->blackout = 0;
    prop->warded = 0;
    prop->decay = 0;
    prop->cloaked = 0;
    prop->logoffs = 0;
    prop->autoclear = 0;
    prop->utilities = 0;
    prop->minx = 0;
    prop->miny = 0;
    prop->maxx = 0;
    prop->maxy = 0;
    prop->minz = 0;
    prop->maxz = 0;
    prop->timefrozen = 0;
    prop->timeshift = 0;
    prop->tempfrozen = 0;
    prop->tempshift = 0;
    prop->weather = 0;
    prop->shroudshield = 0;
    prop->roadroom = 0;
    prop->firstroom = 0;
    prop->type_special = 0;
    prop->orientation = 0;
    prop->prop_state = 0;

    for (i = 0; i < 5; i++) {
      prop->excluded[i] = 0;
      prop->included[i] = 0;
    }
    for (i = 0; i < 200; i++) {
      prop->roomlist[i] = 0;
    }
    for (i = 0; i < 20; i++) {
      prop->workers[i] = str_dup("");
      prop->worker_position[i] = str_dup("");
      prop->worker_idle[i] = 0;
    }

    for (i = 0; i < 200; i++)
    prop->objects[i] = 0;

    for (i = 0; i < 50; i++) {
      prop->tenants[i] = str_dup("");
      prop->tenant_trust[i] = 0;
      prop->sleepers[i] = str_dup("");
    }

    VALIDATE(prop);

    return prop;
  }

  void free_prop(PROP_TYPE *prop) {
    int i;

    if (!IS_VALID(prop))
    return;

    INVALIDATE(prop);
    free_string(prop->propname);
    free_string(prop->owner);
    free_string(prop->address);
    free_string(prop->advertisement);
    for (i = 0; i < 50; i++) {
      free_string(prop->tenants[i]);
      free_string(prop->sleepers[i]);
    }
    for (i = 0; i < 20; i++) {
      free_string(prop->workers[i]);
      free_string(prop->worker_position[i]);
    }
  }

  SHOP_TYPE *pshop_free;

  SHOP_TYPE *new_pshop(void) {
    int i;

    static SHOP_TYPE shop_zero;
    SHOP_TYPE *shop;

    shop = (SHOP_TYPE *)alloc_perm(sizeof(*shop));

    *shop = shop_zero;
    shop->vnum = 0;
    shop->shop_name = str_dup("");
    shop->lower_vnum = 0;
    shop->higher_vnum = 0;
    shop->transfer_point = 0;
    shop->door = 0;
    shop->price = 0;
    shop->current_price = 0;
    shop->auction_day = 0;
    shop->auction_month = 0;
    shop->owner = str_dup("");
    shop->owner_transfer = 0;

    for (i = 0; i < 30; i++) {
      shop->bid_names[i] = str_dup("");
      shop->bid_amounts[i] = 0;
    }
    for (i = 0; i < 50; i++) {
      shop->tenants[i] = str_dup("");
      shop->tenant_transfer[i] = 0;
      shop->tenant_trust[i] = 0;
    }

    VALIDATE(shop);

    return shop;
  }

  void free_pshop(SHOP_TYPE *shop) {
    int i;

    if (!IS_VALID(shop))
    return;

    INVALIDATE(shop);
    free_string(shop->shop_name);
    free_string(shop->owner);

    for (i = 0; i < 30; i++) {
      free_string(shop->bid_names[i]);
    }
    for (i = 0; i < 50; i++) {
      free_string(shop->tenants[i]);
    }
  }
  CLAN_TYPE *clan_free;

  CLAN_TYPE *new_clan(void) {
    int i;

    static CLAN_TYPE clan_zero;
    CLAN_TYPE *clan;

    clan = (CLAN_TYPE *)alloc_perm(sizeof(*clan));

    *clan = clan_zero;
    clan->clan_name = str_dup("");
    clan->clan_desc = str_dup("");
    clan->clan_hist = str_dup("");

    for (i = 0; i < 100; i++) {
      clan->member_names[i] = str_dup("");
      clan->member_voting[i] = str_dup("");
    }
    for (i = 0; i < 6; i++) {
      clan->division_leaders[i] = str_dup("");
    }

    clan->leader = str_dup("");
    clan->leader_voting = str_dup("");

    clan->type = 0;

    clan->oak = 0;
    clan->stone = 0;
    clan->well = 0;
    clan->glade = 0;
    clan->bonus = 0;
    clan->penalty = 0;

    clan->total = 0;

    VALIDATE(clan);

    return clan;
  }

  void free_clan(CLAN_TYPE *clan) {
    int i;

    if (!IS_VALID(clan))
    return;

    INVALIDATE(clan);
    free_string(clan->clan_name);
    free_string(clan->clan_desc);
    free_string(clan->clan_hist);
    for (i = 0; i < 100; i++) {
      free_string(clan->member_names[i]);
      free_string(clan->member_voting[i]);
    }

    free_string(clan->leader);
    free_string(clan->leader_voting);
  }

  FACTION_TYPE *faction_free;

  FACTION_TYPE *new_faction(void) {
    int i;

    static FACTION_TYPE fac_zero;
    FACTION_TYPE *fac;

    fac = (FACTION_TYPE *)alloc_perm(sizeof(*fac));

    *fac = fac_zero;
    fac->vnum = 0;
    fac->last_operation = 0;
    fac->last_intel = 0;
    fac->last_deploy = 0;
    fac->lifeearned = 0;
    fac->steal_mult = 0;
    fac->operation_wins = 0;
    fac->name = str_dup("");
    fac->description = str_dup("");
    fac->manifesto = str_dup("");
    fac->eidilon	= str_dup("");
    fac->eidilon_players = str_dup("");
    fac->missions = str_dup("");
    fac->roles = str_dup("");
    fac->history = str_dup("");
    fac->scenes = str_dup("");
    fac->ooc = str_dup("");
    fac->weekly_ops = 5;
    fac->secret_days = 0;
    fac->college = 0;
    fac->eidilon_type = 0;
    fac->outcast = 0;
    fac->alliance = 0;
    fac->staff = 0;
    fac->nopart = 0;
    fac->reportone_title = str_dup("");
    fac->reportone_text = str_dup("");
    fac->reportone_time = 0;
    fac->reporttwo_title = str_dup("");
    fac->reporttwo_text = str_dup("");
    fac->reporttwo_time = 0;
    fac->reportthree_title = str_dup("");
    fac->reportthree_text = str_dup("");
    fac->reportthree_time = 0;
    fac->reportone_log_sent = 0;
    fac->reporttwo_log_sent = 0;
    fac->reportthree_log_sent = 0;
    fac->reportone_plog_sent = 0;
    fac->reporttwo_plog_sent = 0;
    fac->reportthree_plog_sent = 0;

    for(i=0;i<50;i++)
    {
      fac->reportone_participants[i] = str_dup("");
      fac->reporttwo_participants[i] = str_dup("");
      fac->reportthree_participants[i] = str_dup("");
    }


    fac->cardinal = 0;
    fac->soft_restrict = 0;
    fac->alliance_name = str_dup("");
    fac->position_time = 0;
    fac->restrict_time = 0;
    fac->stasis_time = 0;
    fac->stasis_account = str_dup("");
    for (i = 0; i < 10; i++) {
      fac->enemy_descs[i] = str_dup("");
      for (int j = 0; j < 4; j++)
      fac->enemies[i][j] = 0;
    }
    for (i = 0; i < 20; i++) {
      fac->event_time[i] = 0;
      fac->event_log_sent[i] = 0;
      fac->event_plog_sent[i] = 0;
      fac->event_type[i] = 0;
      fac->event_subtype[i] = 0;
      fac->event_title[i] = str_dup("");
      for (int j = 0; j < 20; j++) {
        fac->event_text[i][j] = str_dup("");
      }
      for(int j=0;j<50;j++) {
        fac->event_participants[i][j] = str_dup("");
      }
    }
    for (i = 0; i < 20; i++) {
      fac->message_timer[i] = 0;
      fac->messages[i] = str_dup("");
      fac->attributes[i] = 0;
      fac->kidnap_name[i] = str_dup("");
      fac->kidnap_territory[i] = str_dup("");
    }
    for (int i = 0; i < 10; i++)
    fac->axes[i] = 0;
    for (int i = 0; i < 30; i++)
    fac->restrictions[i] = 0;
    for (int i = 0; i < 40; i++)
    fac->total_tracked[i] = 0;
    fac->population = 0;
    for (i = 0; i < 50; i++) {
      fac->log_timer[i] = 0;
      fac->log[i] = str_dup("");
    }
    fac->last_incite = current_time;
    fac->can_alt = 0;
    fac->prison_emotes = 0;
    for (i = 0; i < 25; i++)
    fac->op_hour_ran[i] = 0;
    for (i = 0; i < 5; i++)
    fac->op_second_place[i] = 0;
    fac->type = 0;
    fac->subtype = 0;
    fac->antagonist = 0;
    fac->adversary = 0;
    fac->battle_x = 0;
    fac->battle_y = 0;

    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 10; j++)
      fac->report_overflow[i][j] = str_dup("");
    }

    fac->last_defeated = 0;
    fac->defeated_pcs = 0;
    fac->stasis = 0;
    fac->battlewins = 0;
    fac->antag_group_wins = 0;
    fac->antag_solo_wins = 0;
    fac->battle_type = 0;
    fac->deployed_pcs = 0;
    fac->deployed_super = 0;
    fac->deployed_nosuper = 0;
    fac->deployed_power = 0;
    fac->battlehour = 0;
    fac->battleday = 0;
    fac->parent = 0;
    fac->resource = 0;
    fac->research = 0;
    fac->support = 0;
    fac->closed = 0;
    fac->manpower = 0;
    fac->ritual_timer = 0;
    fac->ritual_string = str_dup("");
    fac->ritual_orderer = NULL;
    fac->update = 0;
    fac->symbol = str_dup("");
    fac->dayresources = 0;
    fac->soldier_name = str_dup("");
    fac->soldier_desc = str_dup("");
    fac->report = str_dup("");
    fac->soldiers_deploying = 0;
    fac->soldier_reinforce = 0;
    fac->patrolling = 0;
    fac->guarding = 0;
    for (i = 0; i < 10; i++) {
      fac->ranks[i] = str_dup("");
      for (int y = 0; y < 20; y++)
      fac->rank_stats[i][y] = 0;
    }
    for (i = 0; i < 5; i++) {
      fac->crystals[i] = 0;
      fac->shipment_date[i] = 0;
      fac->shipment_destination[i] = 0;
      fac->shipment_amount[i] = 0;
      fac->shipment_warned[i] = 0;
    }

    fac->leader = str_dup("");
    fac->leaderesteem = 0;
    fac->leaderinactivity = 0;
    fac->last_high_intel = 0;
    fac->weekly_resources = 0;

    fac->battle_order = 0;
    fac->battle_leader = str_dup("");
    fac->battle_target = str_dup("");

    for (i = 0; i < 100; i++) {
      fac->member_names[i] = str_dup("");
      fac->member_rosternotes[i] = str_dup("");
      fac->member_filenotes[i] = str_dup("");
      fac->member_esteem[i] = 0;
      fac->member_inactivity[i] = 0;
      fac->member_rank[i] = 0;
      fac->member_highest_rank[i] = 0;
      fac->member_position[i] = 0;
      fac->member_suspended[i] = 0;
      fac->member_pay[i] = 0;
      fac->member_tier[i] = 0;
      fac->member_noleader[i] = 0;
      fac->member_flags[i] = 0;
      fac->member_power[i] = 0;
      fac->vassal_names[i] = str_dup("");
      fac->vassal_inactivity[i] = 0;
      fac->vassal_tier[i] = 0;
      fac->exmember_names[i] = str_dup("");
      fac->exmember_inactivity[i] = 0;
      fac->exmember_quit[i] = 0;
      fac->exmember_loyalty[i] = 0;
    }

    VALIDATE(fac);

    return fac;
  }

  void free_faction(FACTION_TYPE *fac) {
    int i;

    if (!IS_VALID(fac))
    return;

    INVALIDATE(fac);
    free_string(fac->leader);
    free_string(fac->name);
    free_string(fac->description);
    free_string(fac->manifesto);
    free_string(fac->eidilon);
    free_string(fac->missions);
    free_string(fac->roles);
    free_string(fac->history);
    free_string(fac->scenes);
    free_string(fac->ooc);
    free_string(fac->report);
    free_string(fac->alliance_name);
    free_string(fac->stasis_account);
    for (i = 0; i < 10; i++)
    free_string(fac->enemy_descs[i]);

    for (i = 0; i < 20; i++) {
      free_string(fac->event_title[i]);
      for (int j = 0; j < 20; j++)
      free_string(fac->event_text[i][j]);
    }
    for (i = 0; i < 20; i++) {
      free_string(fac->messages[i]);
      free_string(fac->kidnap_name[i]);
      free_string(fac->kidnap_territory[i]);
    }
    for (i = 0; i < 50; i++)
    free_string(fac->log[i]);
    for (i = 0; i < 10; i++)
    free_string(fac->ranks[i]);
    free_string(fac->battle_leader);
    free_string(fac->battle_target);
    free_string(fac->symbol);
    free_string(fac->soldier_name);
    free_string(fac->soldier_desc);
    free_string(fac->ritual_string);
    free_string(fac->reportone_title);
    free_string(fac->reportone_text);
    free_string(fac->reporttwo_title);
    free_string(fac->reporttwo_text);
    free_string(fac->reportthree_title);
    free_string(fac->reportthree_text);

    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 10; j++)
      free_string(fac->report_overflow[i][j]);
    }

    for (i = 0; i < 100; i++) {
      free_string(fac->member_names[i]);
      free_string(fac->member_rosternotes[i]);
      free_string(fac->member_filenotes[i]);

      free_string(fac->vassal_names[i]);
      free_string(fac->exmember_names[i]);
    }
  }

  INSTITUTE_TYPE *institute_free;

  INSTITUTE_TYPE *new_institute(void) {
    static INSTITUTE_TYPE ins_zero;
    INSTITUTE_TYPE *ins;

    ins = (INSTITUTE_TYPE *)alloc_perm(sizeof(*ins));

    *ins = ins_zero;
    ins->name = str_dup("");
    ins->inactivity = 0;
    ins->college_prestige = 0;
    ins->asylum_prestige = 0;
    ins->college_rank = 0;
    ins->asylum_rank = 0;
    ins->asylum_inactive = 0;
    ins->asylum_protest_days = 0;
    ins->age = 0;
    ins->surname = str_dup("");
    ins->college_house = 0;
    ins->dorm_room = 0;
    ins->gender = 0;
    ins->nophone = 0;
    ins->race = 0;
    ins->modifier = 0;
    ins->clique_role = 0;
    ins->school_habit = 0;
    ins->arrest_cost = 0;
    ins->college_pending_prestige = 0;
    ins->college_grade = 0;
    ins->college_group = 0;
    ins->college_power = 0;
    ins->clinic_breakout = 0;
    ins->college_immune = 0;
    ins->asylum_status = 0;
    ins->asylum_basecost = 0;
    ins->asylum_extracost = 0;
    ins->college_committer = str_dup("");
    ins->asylum_diagnoses = str_dup("");
    ins->college_notes = str_dup("");
    ins->asylum_notes = str_dup("");
    ins->college_pending_notes = str_dup("");
    ins->asylum_pending_notes = str_dup("");
    ins->commit_date = 0;
    ins->asylum_owner = str_dup("");
    ins->college_suspended = 0;
    ins->asylum_suspended = 0;
    ins->college_suspend_rank = 0;
    ins->asylum_suspend_rank = 0;
    ins->arrest_notes = str_dup("");
    ins->college_staff_prestige = 0;
    ins->asylum_staff_prestige = 0;
    ins->college_pending_suma = 0;
    ins->college_suma = 0;

    VALIDATE(ins);

    return ins;
  }

  void free_institute(INSTITUTE_TYPE *ins) {
    if (!IS_VALID(ins))
    return;

    INVALIDATE(ins);
    free_string(ins->name);
    free_string(ins->college_notes);
    free_string(ins->asylum_notes);
    free_string(ins->surname);
    free_string(ins->asylum_diagnoses);
    free_string(ins->college_pending_notes);
    free_string(ins->asylum_pending_notes);
    free_string(ins->asylum_owner);
    free_string(ins->arrest_notes);
    free_string(ins->college_committer);
  }

  PHONEBOOK_TYPE *phonebook_free;

  PHONEBOOK_TYPE *new_phonebook(void) {
    static PHONEBOOK_TYPE phone_zero;
    PHONEBOOK_TYPE *phone;

    phone = (PHONEBOOK_TYPE *)alloc_perm(sizeof(*phone));

    *phone = phone_zero;
    phone->owner = str_dup("");
    phone->inactivity = 0;
    phone->number = 0;
    VALIDATE(phone);

    return phone;
  }

  void free_phonebook(PHONEBOOK_TYPE *phone) {
    if (!IS_VALID(phone))
    return;

    INVALIDATE(phone);
    free_string(phone->owner);
  }

  CABAL_TYPE *cabal_free;

  CABAL_TYPE *new_cabal(void) {
    int i;

    static CABAL_TYPE cabal_zero;
    CABAL_TYPE *cabal;

    cabal = (CABAL_TYPE *)alloc_perm(sizeof(*cabal));

    *cabal = cabal_zero;

    cabal->name = str_dup("");
    cabal->leader = str_dup("");
    cabal->description = str_dup("");
    for (i = 0; i < 20; i++) {
      cabal->messages[i] = str_dup("");
      cabal->message_timer[i] = 0;
    }

    for (i = 0; i < 100; i++) {
      cabal->member_names[i] = str_dup("");
      cabal->member_votes[i] = str_dup("");
      cabal->member_inactivity[i] = 0;
    }
    cabal->leaderinactivity = 0;

    VALIDATE(cabal);

    return cabal;
  }

  void free_cabal(CABAL_TYPE *cabal) {
    int i;

    if (!IS_VALID(cabal))
    return;

    INVALIDATE(cabal);
    free_string(cabal->leader);
    free_string(cabal->name);
    free_string(cabal->description);
    for (i = 0; i < 20; i++) {
      free_string(cabal->messages[i]);
    }

    for (i = 0; i < 100; i++) {
      free_string(cabal->member_names[i]);
      free_string(cabal->member_votes[i]);
    }
  }

  GRAVE_TYPE *grave_free;

  GRAVE_TYPE *new_grave(void) {
    static GRAVE_TYPE grave_zero;
    GRAVE_TYPE *grave;

    grave = (GRAVE_TYPE *)alloc_perm(sizeof(*grave));

    *grave = grave_zero;

    grave->name = str_dup("");
    grave->description = str_dup("");
    grave->type = 0;
    grave->room = 0;
    grave->lastvisit = 0;

    VALIDATE(grave);

    return grave;
  }

  void free_grave(GRAVE_TYPE *grave) {

    if (!IS_VALID(grave))
    return;

    INVALIDATE(grave);
    free_string(grave->name);
    free_string(grave->description);
  }

  LAIR_TYPE *lair_free;

  LAIR_TYPE *new_lair(void) {
    static LAIR_TYPE lair_zero;
    LAIR_TYPE *lair;

    lair = (LAIR_TYPE *)alloc_perm(sizeof(*lair));

    *lair = lair_zero;

    lair->victim = str_dup("");
    lair->room = 0;
    lair->mob = 0;
    lair->killed = 0;
    lair->population = 0;
    lair->abduct_date = 0;
    lair->eat_date = 0;

    VALIDATE(lair);

    return lair;
  }
  void free_lair(LAIR_TYPE *lair) {

    if (!IS_VALID(lair))
    return;

    INVALIDATE(lair);
    free_string(lair->victim);
  }

  ARCHIVE_INDEX *archive_index_free;

  ARCHIVE_INDEX *new_archive_index(void) {
    static ARCHIVE_INDEX arch_zero;
    ARCHIVE_INDEX *arch;

    arch = (ARCHIVE_INDEX *)alloc_perm(sizeof(*arch));

    *arch = arch_zero;
    arch->title = str_dup("");
    arch->keywords = str_dup("");
    arch->number = 0;
    VALIDATE(arch);

    return arch;
  }
  void free_archive_index(ARCHIVE_INDEX *arch) {
    if (!IS_VALID(arch))
    return;

    INVALIDATE(arch);
    free_string(arch->title);
    free_string(arch->keywords);
  }

  ARCHIVE_ENTRY *archive_entry_free;

  ARCHIVE_ENTRY *new_archive_entry(void) {
    static ARCHIVE_ENTRY arch_zero;
    ARCHIVE_ENTRY *arch;

    arch = (ARCHIVE_ENTRY *)alloc_perm(sizeof(*arch));

    *arch = arch_zero;
    arch->title = str_dup("");
    arch->owner = str_dup("");
    arch->text = str_dup("");
    arch->number = 0;
    VALIDATE(arch);

    return arch;
  }
  void free_archive_entry(ARCHIVE_ENTRY *arch) {
    if (!IS_VALID(arch))
    return;

    INVALIDATE(arch);
    free_string(arch->title);
    free_string(arch->owner);
    free_string(arch->text);
  }

  ROSTERCHAR_TYPE *roster_free;

  ROSTERCHAR_TYPE *new_rosterchar(void) {
    static ROSTERCHAR_TYPE roster_zero;
    ROSTERCHAR_TYPE *roster;

    roster = (ROSTERCHAR_TYPE *)alloc_perm(sizeof(*roster));

    *roster = roster_zero;

    roster->owner = str_dup("");
    roster->name = str_dup("");
    roster->lastname = str_dup("");
    roster->shortdesc = str_dup("");
    roster->description = str_dup("");
    roster->sponsored = 0;
    roster->lastactive = 0;
    roster->claimed = 0;
    VALIDATE(roster);

    return roster;
  }
  void free_rosterchar(ROSTERCHAR_TYPE *roster) {

    if (!IS_VALID(roster))
    return;

    INVALIDATE(roster);
    free_string(roster->owner);
    free_string(roster->name);
    free_string(roster->lastname);
    free_string(roster->shortdesc);
    free_string(roster->description);
  }

  GALLERY_TYPE *gallery_free;

  GALLERY_TYPE *new_gallery(void) {
    static GALLERY_TYPE gallery_zero;
    GALLERY_TYPE *gallery;

    gallery = (GALLERY_TYPE *)alloc_perm(sizeof(*gallery));

    *gallery = gallery_zero;
    gallery->name = str_dup("");
    gallery->territory = str_dup("");
    gallery->subfaction = 0;
    gallery->description = str_dup("");
    gallery->timeline = str_dup("");
    gallery->owner = str_dup("");
    gallery->coauthors = str_dup("");
    gallery->account_owner = str_dup("");
    gallery->xp = 0;
    gallery->editable = 0;
    gallery->lastmodified = 0;
    VALIDATE(gallery);

    return gallery;
  }

  void free_gallery(GALLERY_TYPE *gallery) {
    if (!IS_VALID(gallery))
    return;

    INVALIDATE(gallery);
    free_string(gallery->name);
    free_string(gallery->territory);
    free_string(gallery->description);
    free_string(gallery->timeline);
    free_string(gallery->owner);
    free_string(gallery->coauthors);
    free_string(gallery->account_owner);
  }

  PAGE_TYPE *page_free;

  PAGE_TYPE *new_page(void) {
    static PAGE_TYPE page_zero;
    PAGE_TYPE *page;

    page = (PAGE_TYPE *)alloc_perm(sizeof(*page));

    *page = page_zero;
    page->author = str_dup("");
    page->text = str_dup("");
    page->createdate = 0;
    page->number = 0;
    page->world = 0;
    VALIDATE(page);

    return page;
  }

  void free_page(PAGE_TYPE *page) {
    if (!IS_VALID(page))
    return;

    INVALIDATE(page);
    free_string(page->author);
    free_string(page->text);
  }

  STORYLINE_TYPE *storyline_free;

  STORYLINE_TYPE *new_storyline(void) {
    static STORYLINE_TYPE storyline_zero;
    STORYLINE_TYPE *storyline;

    storyline = (STORYLINE_TYPE *)alloc_perm(sizeof(*storyline));

    *storyline = storyline_zero;

    storyline->name = str_dup("");
    storyline->owner = str_dup("");
    storyline->coauthors = str_dup("");
    storyline->description = str_dup("");
    storyline->timeline = str_dup("");
    storyline->lastmodified = 0;
    for (int i = 0; i < 100; i++)
    storyline->participants[i] = str_dup("");
    for (int i = 0; i < 10; i++) {
      storyline->masterminds[i] = str_dup("");
      storyline->mastermind_power[i] = 0;
    }
    storyline->power = 0;
    storyline->total_power = 0;

    VALIDATE(storyline);

    return storyline;
  }
  void free_storyline(STORYLINE_TYPE *storyline) {
    if (!IS_VALID(storyline))
    return;

    INVALIDATE(storyline);
    free_string(storyline->name);
    free_string(storyline->description);
    free_string(storyline->timeline);
    free_string(storyline->owner);
    free_string(storyline->coauthors);
    for (int i = 0; i < 100; i++)
    free_string(storyline->participants[i]);
    for (int i = 0; i < 10; i++)
    free_string(storyline->masterminds[i]);
  }

  CMD_TYPE *cmd_free;

  CMD_TYPE *new_command(void) {
    static CMD_TYPE cmd_zero;
    CMD_TYPE *cmd;

    if (cmd_free == NULL)
    cmd = (CMD_TYPE *)alloc_perm(sizeof(*cmd));
    else {
      cmd = cmd_free;
      cmd_free = cmd_free->next;
    }

    *cmd = cmd_zero;
    cmd->name = str_dup("");
    cmd->lookup_name = str_dup("");
    cmd->do_fun = NULL;
    cmd->position = 0;
    cmd->level = 0;
    cmd->log = 0;
    cmd->flags = 0;
    cmd->show = 0;
    cmd->department = DEP_PLAYER;

    VALIDATE(cmd);

    return cmd;
  }

  void free_command(CMD_TYPE *cmd) {
    if (!IS_VALID(cmd))
    return;

    INVALIDATE(cmd);
    free_string(cmd->name);
    free_string(cmd->lookup_name);
    cmd->next = cmd_free;
    cmd_free = cmd;
  }

  void set_ch_default(CHAR_DATA *ch) {
    int x;
    ch->name = &str_empty[0];
    ch->desc = NULL;
    ch->short_descr = &str_empty[0];
    ch->long_descr = &str_empty[0];
    ch->description = &str_empty[0];
    ch->prompt = &str_empty[0];
    ch->last_ip = &str_empty[0];
    ch->prompt = str_dup("<%dhp %dmv> %p%c");
    ch->prefix = &str_empty[0];
    ch->logon = current_time;
    ch->lines = PAGELEN;
    ch->linewidth = WORD_WRAP;
    ch->position = POS_STANDING;

    ch->clan = str_dup("");

    ch->order = 0;
    ch->ordertarget = str_dup("");

    ch->sex = 0;
    ch->ttl = -1;
    ch->walking = 0;
    ch->car_inroom = 0;
    ch->esteem_faction = 0;
    ch->esteem_cult = 0;
    ch->esteem_sect = 0;
    ch->abomination = 0;
    ch->faction = 0;
    ch->killed = FALSE;
    ch->x = 0;
    ch->y = 0;
    ch->hit = 10000;
    ch->lastlogoff = 0;
    ch->vassal = 0;
    ch->factiontwo = 0;
    ch->mob_ai = 0;
    ch->fcore = 0;
    ch->fcult = 0;
    ch->fsect = 0;
    ch->deploy_cult = 1;
    ch->deploy_sect = 1;
    ch->deploy_cult = 1;
    ch->oppress = 0;
    ch->factiontrue = -1;
    ch->reply_type = 0;
    ch->aggression = str_dup("");
    ch->protecting = str_dup("");
    ch->donated = 0;
    ch->recent_moved = 0;
    ch->money = 0;
    ch->exp = 0;
    ch->rpexp = 0;
    ch->spentexp = 0;
    ch->mapcount = 0;
    ch->lifeforce = 10000;
    ch->lf_used = 0;
    ch->lf_taken = 0;
    ch->lf_sused = 0;
    ch->positive_magic = 0;
    ch->negative_magic = 0;
    ch->command_force = 0;
    ch->spentrpexp = 0;
    ch->privaterpexp = 0; // Record of rpexp gained while in private mode
    ch->privatekarma = 0; // Record of rpexp gained while in private mode
    ch->karma = 0;
    ch->spentkarma = 0;
    ch->pkarma = 0;
    ch->spentpkarma = 0;
    ch->newexp = STARTING_EXP;
    ch->spentnewexp = 0;
    ch->newrpexp = STARTING_RPEXP;
    ch->spentnewrpexp = 0;

    ch->fight_fast = TRUE;
    ch->fight_current = NULL;
    ch->fight_next = NULL;
    ch->hadturn = FALSE;
    ch->fight_attacks = 0;
    ch->fight_speed = 1;

    ch->disx = 0;
    ch->disy = 0;
    ch->valuable = FALSE;
    for (x = 0; x < SKILL_MAX; x++) {
      ch->skills[x] = 0;
      ch->wilds_skills[x] = 0;
      ch->other_skills[x] = 0;
      ch->godrealm_skills[x] = 0;
      ch->hell_skills[x] = 0;
    }
    for (x = 0; x < MAX_DIS; x++) {
      ch->disciplines[x] = 0;
    }

    ch->endurance = 0;
    ch->fistattack = 0;
    ch->guard = 0;
    ch->fisttimer = 0;

    ch->timer = 0;

    ch->shape = 0;
    ch->lastshape = 0;
    ch->heal_timer = 0;
    ch->death_timer = 0;
    ch->bagcarrier = 0;
    ch->activeat = 0;
    ch->diminished = 0;
    ch->diminish_lt = 0;
    ch->diminish_till = 0;
    ch->diminish_vis = 0;
    ch->last_hit_damage = 0;
    ch->rounddiminish = 0;
    ch->diminish_offervalue = 0;
    ch->attacking = 0;
    ch->specialpoint = -1;
    ch->lastdisc = 0;
    ch->facing = DIR_NORTH;
    ch->hits_taken = 0;

    ch->attack_turn = 0;
    ch->attack_timer = 0;
    ch->move_timer = 0;
    ch->ability_timer = 0;
    ch->ability_dead = 0;
    ch->round_timer = 0;
    ch->damage_absorb = 0;
    ch->debuff = 0;
    ch->to_debuff = 0;
    ch->qmove = str_dup("");
    ch->amove = str_dup("");
    ch->abilmove = str_dup("");
    ch->moving = FALSE;
    ch->doneabil = FALSE;
    ch->preyvictim = FALSE;
    ch->fighting = FALSE;
    ch->in_fight = FALSE;
    ch->target_dam = 0;
    ch->target_dam_2 = 0;
    ch->target_dam_3 = 0;
    ch->attackdam = 0;
    ch->knockfails = 0;
    ch->actiontimer = 0;
    ch->tsincemove = 0;
    ch->attackdisc = 0;
    ch->shadowdisc = 0;
    ch->defensedisc = 0;
    ch->moved = 0;
    ch->run_dir = 0;
    ch->wound_location = 0;

    for (x = 0; x < 10; x++) {
      ch->delays[x][0] = 0;
      ch->delays[x][1] = 0;
      ch->delays[x][2] = 0;
    }
    for (x = 0; x < 30; x++) {
      ch->caff[x] = 0;
      ch->caff_duration[x] = 0;
    }

    SET_INIT(ch->fightflag);

    ch->race = 0;
    ch->modifier = 0;
    SET_INIT(ch->act);
    SET_INIT(ch->comm);
    SET_INIT(ch->legacy);
    SET_FLAG(ch->comm, COMM_COMBINE);
    //    SET_FLAG(ch->comm, COMM_PROMPT);
    SET_FLAG(ch->comm, COMM_NOGUIDE);
    SET_INIT(ch->affected_by);

    ch->next_observing_room = NULL;
  }

  void set_pcdata_default(PC_DATA *pcdata) {
    int x;

    pcdata->buffer = new Buffer();
    // pcdata->buffer 		= new_buf(1000);
    pcdata->pwd = str_dup("");
    pcdata->upwd = str_dup("");
    pcdata->bamfin = str_dup("");
    pcdata->bamfout = str_dup("");
    pcdata->title = str_dup("");
    pcdata->floc = str_dup("");
    pcdata->ftime = str_dup("");
    pcdata->fdesc = str_dup("");

    pcdata->email = str_dup("");
    pcdata->whotitle = str_dup("");

    pcdata->logoff_room = 0;

    pcdata->spec_trust = FALSE;

    pcdata->last_name = str_dup("");
    pcdata->verbal_color = str_dup("");
    pcdata->enthralling = str_dup("");
    pcdata->enrapturing = str_dup("");
    pcdata->enthralled = str_dup("");
    pcdata->enraptured = str_dup("");
    pcdata->privatepartner = str_dup("");
    pcdata->enlinked = str_dup("");
    pcdata->remember_detail = str_dup("");
    pcdata->account_name = str_dup("");
    pcdata->storyrunner = str_dup("");
    pcdata->guest_of = str_dup("");
    pcdata->aiding_thwart = str_dup("");
    pcdata->pledge = str_dup("");
    pcdata->pubic_hair = str_dup("");
    pcdata->class_fame = str_dup("");
    pcdata->home_territory = str_dup("");
    pcdata->deathcause = str_dup("");
    pcdata->place = str_dup("");
    pcdata->last_emote = str_dup("");
    pcdata->last_newbie = str_dup("");
    pcdata->nightmare = str_dup("");
    pcdata->mentor = str_dup("");
    pcdata->fame = str_dup("");
    pcdata->art1 = str_dup("");
    pcdata->art2 = str_dup("");
    pcdata->art3 = str_dup("");
    pcdata->ritual_dreamworld = str_dup("");
    pcdata->dream_identity = str_dup("");
    pcdata->identity_world = str_dup("");
    pcdata->deluded_reason = str_dup("");
    pcdata->last_villain_mod = str_dup("");
    pcdata->last_ritual = str_dup("");
    pcdata->ritual_maintainer = str_dup("");
    pcdata->maintained_target = str_dup("");
    pcdata->dream_identity_timer = 0;
    pcdata->bad_abomination = 0;
    pcdata->lucky_abomination = 0;
    pcdata->sleeping = 0;
    pcdata->earned_karma = 0;
    pcdata->police_catch_timer = 0;
    pcdata->captive_timer = 0;
    pcdata->desclock = 0;
    pcdata->enlinktime = 0;
    pcdata->bust = 0;
    pcdata->market_visit = 0;
    pcdata->gen_stage = 0;
    pcdata->tier_raised = 0;
    pcdata->detention_time = 0;
    pcdata->hell_power = 0;
    pcdata->godrealm_power = 0;
    pcdata->other_power = 0;
    pcdata->timenotalone = 0;
    pcdata->lastnotalone = 0;
    pcdata->overworked = 0;
    pcdata->weakness_status = 0;
    pcdata->weakness_timer = 0;
    pcdata->victimize_difficult_time = 0;
    for (int x = 0; x < 20; x++) {
      pcdata->rp_log[x] = str_dup("");
      pcdata->outfit_nickpoint[x] = 0;
      pcdata->outfit_nicknames[x] = str_dup("");
    }
    for (int x = 0; x < 20; x++)
    pcdata->victimize_history[x] = 0;
    for (int x = 0; x < 50; x++) {
      pcdata->narrative_give[x] = 0;
      pcdata->narrative_want[x] = 0;
    }
    pcdata->victimize_char_point = NULL;
    pcdata->victimize_vic_point = NULL;
    pcdata->prep_target = NULL;
    pcdata->prep_action = 0;
    pcdata->destiny_offer = 0;
    pcdata->destiny_offer_role = 0;
    pcdata->destiny_offer_char = NULL;
    pcdata->victimize_vic_timer = 0;
    pcdata->victimize_char_select = 0;
    pcdata->victimize_vic_response_to = 0;
    pcdata->victimize_vic_select = 0;
    pcdata->victimize_difficult_count = 0;
    pcdata->victimize_vic_pending = 0;
    pcdata->victimize_lf_pending_loss = 0;
    pcdata->victimize_pending_lf_gain = 0;
    pcdata->victimize_last_response = 0;
    pcdata->stasis_spent_pkarma = 0;
    pcdata->stasis_spent_exp = 0;
    pcdata->stasis_spent_rpexp = 0;
    pcdata->stasis_spent_karma = 0;
    pcdata->last_dreamworld = 0;
    pcdata->dream_timer = 0;
    pcdata->patrol_rp = 0;
    pcdata->combat_emoted = FALSE;
    pcdata->dreamoutfit = 0;
    pcdata->dreamfromoutfit = 0;
    pcdata->rp_logging = 0;
    pcdata->nightmare_dragged = 0;
    pcdata->last_rp_room = 0;
    pcdata->availability = 0;
    pcdata->watching = 0;
    pcdata->truepenis = 0;
    pcdata->heroic = 0;
    pcdata->truebreasts = 0;
    pcdata->maintain_cost = 0;
    pcdata->sexchange_time = 0;
    pcdata->maintained_ritual = 0;
    pcdata->penis = 70;
    pcdata->nightmare_shifted = 0;
    pcdata->deluded_cost = 0;
    pcdata->deluded_duration = 0;
    pcdata->mentor_of = 0;
    pcdata->solitary_time = 0;
    pcdata->dream_exit = -1;
    pcdata->dream_door = -1;
    pcdata->institute_action = 0;
    pcdata->psychic_attempts = 0;
    pcdata->psychic_proof = 0;
    pcdata->stasis_time = 0;
    pcdata->summon_bound = 0;
    pcdata->secret_days = 0;
    pcdata->secret_recover = 0;
    pcdata->intel = 0;
    pcdata->last_intel = 0;
    pcdata->in_domain = 0;
    pcdata->pending_resources = 0;
    pcdata->recent_exp = 0;
    pcdata->op_emotes = 0;
    pcdata->destiny_reject = 0;
    pcdata->destiny_reject_cooldown = 0;
    pcdata->bloodstorm_resist = 0;
    pcdata->domain_timer = 0;
    pcdata->spent_resources = 0;
    pcdata->poisontimer = 0;
    pcdata->rohyptimer = 0;
    pcdata->relapsewerewolf = 0;
    pcdata->relapsevampire = 0;
    pcdata->icooldown = 0;
    pcdata->rpainamount = 0;
    pcdata->tcooldown = 0;
    pcdata->timesincedamage = 0;

    pcdata->lfcount = 0;
    pcdata->lftotal = 0;

    // Deputy defaults - Unsure why they weren't here.  Adding to address being
    // arrested constantly - Discordance
    pcdata->deputy_cooldown = 0;
    pcdata->disperse_timer = 0;
    pcdata->disperse_room = 0;
    pcdata->movealong_timer = 0;
    pcdata->movealong_vnum = 0;

    pcdata->pledge_stealth = 0;
    pcdata->class_type = 0;
    pcdata->class_neutral = 0;
    pcdata->tboosts = 0;
    pcdata->blackout_cooldown = 0;
    pcdata->class_faction = 0;
    pcdata->class_cooldown = 0;
    pcdata->classpoints = 0;
    pcdata->deepshroud = 0;
    pcdata->classtotal = 0;
    pcdata->blackeyes = 0;
    pcdata->implant_frequency = 0;
    pcdata->mindmessed = 0;
    pcdata->brainwash_reidentity = str_dup("");
    pcdata->brainwash_loyalty = str_dup("");
    pcdata->eidilon_ambient = str_dup("");
    pcdata->implant_dream = str_dup("");
    pcdata->ff_knowledge = str_dup("");
    pcdata->ff_secret = str_dup("");
    pcdata->luck_string_one = str_dup("");
    pcdata->luck_string_two = str_dup("");
    pcdata->luck_type = 0;
    pcdata->luck_character = NULL;
    pcdata->cam_spy_char = NULL;
    pcdata->quit_room = 0;
    pcdata->ritual_prey_timer = 0;
    pcdata->ritual_prey_target = str_dup("");
    pcdata->summary_target = NULL;
    pcdata->summary_name = str_dup("");
    pcdata->summary_intro = str_dup("");
    pcdata->summary_content = str_dup("");
    pcdata->summary_conclusion = str_dup("");
    pcdata->summary_when = 0;
    pcdata->summary_type = 0;
    pcdata->summary_stage = 0;
    pcdata->summary_blood = 0;
    pcdata->summary_intel = 0;
    pcdata->summary_lifeforce = 0;
    pcdata->summary_helpless = 0;
    pcdata->summary_home = 0;

    pcdata->operative_creation_type = 0;
    pcdata->operative_core = str_dup("");
    pcdata->operative_cult = str_dup("");
    pcdata->operative_sect = str_dup("");

    pcdata->narrative_query_char = NULL;
    pcdata->attempt_emote = str_dup("");
    pcdata->attempt_character = NULL;
    pcdata->is_attempt_emote = FALSE;
    pcdata->ftype = 0;
    pcdata->paranoid = 0;
    pcdata->eidilon_of =  0;
    pcdata->wander_time = 0;
    pcdata->crowded_room = FALSE;
    pcdata->last_victim_bled = 0;
    pcdata->last_imbue = 0;
    pcdata->blackeyes_power = 0;
    pcdata->fleshformed = 0;
    pcdata->infuse_date = 0;
    pcdata->shroud_dragging = 0;
    pcdata->shroud_drag_depth = 0;
    pcdata->monster_beaten = 0;
    pcdata->egg_date = 0;
    pcdata->egg_daddy = 0;
    pcdata->maim = str_dup("");
    pcdata->class_oldfaction = 0;
    pcdata->class_oldlevel = 0;

    pcdata->lastaccident = 0;
    pcdata->lastshift = 0;
    pcdata->lastnormal = 0;
    pcdata->lastidentity = 0;
    pcdata->mimic = 0;
    pcdata->decay_stage = 0;
    pcdata->decay_timer = 0;
    pcdata->max_decay_timer = 0;

    pcdata->aexp = 0;
    pcdata->awins = 0;
    pcdata->alosses = 0;
    pcdata->astatus = 0;

    pcdata->monster_fed = 800;

    pcdata->job_title_one = str_dup("");
    pcdata->job_title_two = str_dup("");
    pcdata->job_type_one = 0;
    pcdata->job_type_two = 0;
    pcdata->job_room_one = 0;
    pcdata->job_room_two = 0;
    pcdata->job_delay_one = 0;
    pcdata->job_delay_two = 0;

    pcdata->eyes = str_dup("");
    pcdata->hair = str_dup("");
    pcdata->skin = str_dup("");

    pcdata->understanding = str_dup("");
    pcdata->understandtoggle = 0;
    pcdata->public_alarm = 0;
    pcdata->last_drove = 0;
    pcdata->blood_ticks = 0;
    pcdata->payscale = 100;
    pcdata->racial_power = 0;
    pcdata->racial_power_two = 0;
    pcdata->blood_potency = 0;
    pcdata->lured_room = 0;
    pcdata->free_ritual = 0;
    pcdata->resident = 0;
    pcdata->last_inhaven = 0;
    pcdata->last_page = 0;
    for (x = 0; x < 10; x++) {
      pcdata->destiny_lockout_time[x] = 0;
      pcdata->destiny_lockout_char[x] = str_dup("");
      pcdata->destiny_in_one[x] = 0;
      pcdata->destiny_in_two[x] = 0;
      pcdata->destiny_stage_one[x] = 0;
      pcdata->destiny_stage_two[x] = 0;
      pcdata->destiny_in_with_one[x] = str_dup("");
      pcdata->destiny_in_with_two[x] = str_dup("");
    }
    for (x = 0; x < 50; x++) {
      pcdata->destiny_interest_one[x] = 0;
      pcdata->destiny_interest_two[x] = 0;
      pcdata->destiny_block_one[x] = 0;
      pcdata->destiny_block_two[x] = 0;
    }
    for (x = 0; x < 20; x++) {
      pcdata->ci_exclude_type[x] = 0;
      pcdata->ci_excludes[x] = str_dup("");
      pcdata->ci_include_type[x] = 0;
      pcdata->ci_includes[x] = str_dup("");
    }
    for (x = 0; x < MAX_COVERS; x++) {
      pcdata->dirt[x] = 0;
      pcdata->blood[x] = 0;
      pcdata->exposed[x] = 0;
    }
    for (x = 0; x < 7; x++) {
      pcdata->sins[x] = 0;
    }
    for (x = 0; x < 50; x++) {
      pcdata->known_gateways[x] = 0;
    }
    for (x = 0; x < 6; x++) {
      pcdata->heldblood[x] = 0;
    }
    for (x = 0; x < 20; x++) {
      pcdata->attract[x] = 0;
    }

    pcdata->aiding_scheme = 0;
    pcdata->scheme_influence = 0;
    pcdata->super_influence = 0;
    pcdata->faction_influence = 0;
    for (x = 0; x < MAX_TRUSTS; x++) {
      pcdata->trust_names[x] = str_dup("");
      pcdata->trust_levels[x] = 0;
    }
    pcdata->feedback = str_dup("");

    for (x = 0; x < 50; x++) {
      pcdata->speed_names[x] = str_dup("");
      pcdata->speed_numbers[x] = 0;
      pcdata->file_factions[x] = 0;
      pcdata->file_message[x] = str_dup("");
      pcdata->ignored_characters[x] = str_dup(""); // ignored characters
      pcdata->ignored_accounts[x] = str_dup("");   // ignored accounts
      pcdata->nowhere_characters[x] = str_dup(""); // nowhere characters
      pcdata->nowhere_accounts[x] = str_dup("");   // nowhereaccounts
    }

    pcdata->nowhere_male = 0;
    pcdata->nowhere_female = 0;

    for (x = 0; x < 10; x++) {
      pcdata->attention_history[x] = str_dup("");
      pcdata->chan_names[x] = str_dup("");
      pcdata->chan_numbers[x] = 0;
      pcdata->chan_status[x] = 0;
    }

    for (x = 0; x < MAX_NEWHABITS; x++) {
      pcdata->habit[x] = 0;
    }

    for (x = 0; x < 10; x++) {
      pcdata->feedbackinc[x] = str_dup("");
      pcdata->feedbacktim[x] = 0;
      pcdata->feedbackbon[x] = 0;
    }
    pcdata->chat_handle = str_dup("");
    pcdata->chatroom = 1;
    for (x = 0; x < 5; x++) {
      pcdata->monster_names[0][x] = str_dup("");
      pcdata->monster_names[1][x] = str_dup("");
      pcdata->monster_names[2][x] = str_dup("");

      pcdata->monster_discs[0][x] = 0;
      pcdata->monster_discs[1][x] = 0;
      pcdata->monster_discs[2][x] = 0;
      pcdata->monster_discs[3][x] = 0;
      pcdata->monster_discs[4][x] = 0;
      pcdata->monster_discs[5][x] = 0;
      pcdata->monster_discs[6][x] = 0;
      pcdata->monster_discs[7][x] = 0;
      pcdata->monster_discs[8][x] = 0;
      pcdata->monster_discs[9][x] = 0;
    }
    for (x = 0; x < 10; x++) {
      pcdata->storycon[x][0] = 0;
      pcdata->storycon[x][1] = 0;
      pcdata->storycon[x][2] = 0;
    }

    pcdata->fall_timer = 0;
    pcdata->move_damage = 0;
    pcdata->walking = -1;
    pcdata->xpbonus = 0;
    pcdata->rpxpbonus = 0;

    pcdata->deploy_from = 0;
    pcdata->selfbondage = 0;
    pcdata->neutralized = 0;
    pcdata->boobjob = 0;

    pcdata->base_standing = 150;
    pcdata->social_praise = 0;
    pcdata->social_behave = 0;
    pcdata->attract_base = 50;
    pcdata->attract_mod = 0;
    pcdata->influence = 0;
    pcdata->influence_bank = 500;
    pcdata->influence_balance = 0;
    pcdata->proxies = 0;
    pcdata->super_influence = 0;
    pcdata->association = 0;
    pcdata->adventure_fatigue = 0;
    pcdata->true_id = 0;

    pcdata->vamp_fullness = 0;
    pcdata->vomit_timer = 0;

    pcdata->timebanished = 0;
    pcdata->create_date = 0;
    pcdata->beacon = 0;
    pcdata->last_share = 0;
    pcdata->ally_limit = 0;
    pcdata->minion_limit = 0;

    pcdata->lifeforcecooldown = 0;

    pcdata->currentmonster = 0;
    pcdata->stories_run = 0;
    pcdata->stories_countdown = 0;
    pcdata->rooms_run = 0;
    pcdata->rooms_countdown = 0;

    pcdata->sincool = 0;

    pcdata->youbadcounter = 0;

    pcdata->cop_cooldown = 0;
    pcdata->ftype = 0;
    pcdata->blackeyes = 0;
    pcdata->paranoid = 0;
    pcdata->eidilon_of = 0;
    pcdata->wander_time = 0;
    pcdata->eidilon_ambient = str_dup("");
    pcdata->implant_dream = str_dup("");
    pcdata->ff_knowledge = str_dup("");
    pcdata->ff_secret = str_dup("");
    pcdata->blackeyes_power = 0;

    pcdata->height_feet = 0;
    pcdata->height_inches = 0;

    pcdata->vote_timer = 0;
    pcdata->yesvotes = 0;
    pcdata->novotes = 0;

    pcdata->making_event = NULL;
    pcdata->event_cooldown = 0;

    pcdata->travel_to = -1;
    pcdata->travel_type = -1;
    pcdata->travel_time = -1;
    pcdata->travel_from = -1;
    pcdata->travel_slow = 0;
    pcdata->talk = str_dup("");

    pcdata->pinned_by = str_dup("");

    pcdata->scent = str_dup("");
    pcdata->taste = str_dup("");
    pcdata->perfume_cost = 0;
    pcdata->attract_count = 10.0;
    pcdata->attract_count_fashion = 70.0;
    pcdata->attract_count_phys = 3000.0;
    pcdata->attract_count_skin = 1200.0;

    pcdata->exp_cap = DEFAULT_MAXEXP;
    pcdata->rpexp_cap = DEFAULT_MAXRPEXP;

    pcdata->time_since_train = 0;
    pcdata->time_since_emote = 0;
    pcdata->time_since_action = 0;

    for (x = 0; x < MAX_COVERS + 10; x++) {
      pcdata->focused_descs[x] = str_dup("");
      pcdata->detail_over[x] = str_dup("");
      pcdata->detail_under[x] = str_dup("");
      pcdata->scars[x] = str_dup("");
      pcdata->focused_order[x] = x + 1;
    }

    for (x = 0; x < 5; x++)
    pcdata->languages[x] = str_dup("");

    for (x = 0; x < 25; x++) {
      pcdata->imprint[x] = str_dup("");
      pcdata->imprint_trigger[x] = str_dup("");
      pcdata->imprint_type[x] = 0;
      pcdata->imprint_pressure_one[x] = 0;
      pcdata->imprint_pressure_two[x] = 0;
      pcdata->imprint_pressure_three[x] = 0;
      pcdata->imprint_pending[x] = 0;
      pcdata->memories[x] = str_dup("");
      pcdata->repressions[x] = str_dup("");
    }
    pcdata->imprint_resist_cost = 0;
    pcdata->imprinter = NULL;
    pcdata->trance = 0;
    pcdata->mindbroken = 0;
    pcdata->persuade_target = NULL;
    pcdata->persuade_cooldown = 0;
    pcdata->persuade_message = str_dup("");
    pcdata->persuade_pone = 0;
    pcdata->persuade_ptwo = 0;
    pcdata->persuade_pthree = 0;
    pcdata->note_gain = 0;

    pcdata->prison_care = 0;
    pcdata->prison_mult = 0;
    pcdata->dream_room = 0;
    pcdata->dream_sex = 0;
    pcdata->dream_invite = 0;
    pcdata->tempdreamgodworld = 0;
    pcdata->tempdreamgodchar = 0;
    pcdata->dream_attack_cool = 0;
    pcdata->travel_prepped = FALSE;

    pcdata->training_stage = 0;
    pcdata->training_stat = 0;
    pcdata->training_type = 0;

    for (x = 0; x < 10; x++)
    pcdata->deactivated_stats[x] = 0;
    for (x = 0; x < 300; x++) {
      pcdata->stat_log_stat[x] = 0;
      pcdata->stat_log_from[x] = 0;
      pcdata->stat_log_to[x] = 0;
      pcdata->stat_log_cost[x] = 0;
      pcdata->stat_log_method[x] = 0;
      pcdata->stat_log_string[x] = str_dup("");
    }

    for (x = 0; x < 10; x++)
    pcdata->patrol_habits[x] = 0;
    pcdata->patrol_status = 0;
    pcdata->patrol_timer = 0;
    pcdata->patrol_amount = 0;
    pcdata->patrol_pledged = 0;
    pcdata->patrol_subtype = 0;
    pcdata->patrol_room = NULL;
    pcdata->patrol_target = NULL;
    pcdata->last_patrol = NULL;
    pcdata->encounter_sr = NULL;

    pcdata->enc_prompt_one = str_dup("");
    pcdata->enc_prompt_two = str_dup("");
    pcdata->enc_prompt_three = str_dup("");
    pcdata->encounter_pnumber = 0;

    pcdata->patrol_did_arcane = FALSE;
    pcdata->patrol_did_war = FALSE;
    pcdata->patrol_did_diplomatic = FALSE;
    pcdata->patrol_did_hunting = FALSE;

    pcdata->lf_modifier = 0;
    pcdata->spawned_monsters = 0;
    pcdata->training_stat = 0;
    pcdata->training_disc = 0;
    pcdata->offworld_protection = 0;
    pcdata->wilds_fame = str_dup("");
    pcdata->other_fame = str_dup("");
    pcdata->godrealm_fame = str_dup("");
    pcdata->hell_fame = str_dup("");
    pcdata->wilds_fame_level = 0;
    pcdata->wilds_legendary = 0;
    pcdata->other_fame_level = 0;
    pcdata->other_legendary = 0;
    pcdata->hell_fame_level = 0;
    pcdata->hell_legendary = 0;
    pcdata->godrealm_fame_level = 0;
    pcdata->godrealm_legendary = 0;

    pcdata->sr_nomove = 0;
    pcdata->form_change_date = 0;
    pcdata->lingering_sanc = 0;
    pcdata->prison_emotes = 0;
    pcdata->demon_pact = str_dup("");
    pcdata->driving_around = FALSE;
    pcdata->villain_praise = str_dup("");
    pcdata->villain_type = 0;
    pcdata->villain_message = str_dup("");
    pcdata->brander = str_dup("");
    pcdata->brandstring = str_dup("");
    pcdata->last_bitten = str_dup("");
    pcdata->mind_guarded = 0;
    pcdata->mind_guard_mult = 0;
    pcdata->ill_count = 0;
    pcdata->ill_time = 0;
    pcdata->bittenloss = 0;
    pcdata->rerollselftime = 0;
    pcdata->brandcool = 0;
    pcdata->branddate = 0;
    pcdata->brandlocation = 0;
    pcdata->brandstatus = 0;
    pcdata->last_develop_type = 0;
    pcdata->last_develop_time = 0;
    pcdata->doom_date = 0;
    pcdata->doom_countdown = 0;
    pcdata->doom_desc = str_dup("");
    pcdata->doom_custom = str_dup("");
    pcdata->speaking = str_dup("English");
    pcdata->dream_origin = str_dup("");
    pcdata->murder_name = str_dup("");
    pcdata->murder_timer = 0;
    pcdata->murder_cooldown = 0;
    pcdata->sniffing = FALSE;
    pcdata->spirit_type = 0;
    pcdata->guest_type = 0;
    pcdata->guest_tier = 0;
    pcdata->legendary_cool = 0;
    pcdata->sr_nomove = 0;
    pcdata->next_henchman = 0;
    pcdata->next_enemy = 0;
    pcdata->next_monster = 0;
    pcdata->default_speed = 3;
    pcdata->monster_wounds = 0;
    pcdata->dreamplace = str_dup("");
    pcdata->dreamtitle = str_dup("");

    pcdata->finale_location = str_dup("");
    pcdata->finale_timer = 0;
    pcdata->encounter_storyline = str_dup("");
    pcdata->encounter_bringin = str_dup("");
    pcdata->roster_description = str_dup("");
    pcdata->roster_shortdesc = str_dup("");
    pcdata->wexp = 0;
    pcdata->oexp = 0;
    pcdata->gexp = 0;
    pcdata->hexp = 0;
    pcdata->aexp = 0;
    pcdata->dexp = 0;
    pcdata->awins = 0;
    pcdata->alosses = 0;
    pcdata->astatus = 0;

    pcdata->boobjob = 0;

    pcdata->extra_powers = 0;

    pcdata->scheme_influence = 0;
    pcdata->faction_influence = 0;
    pcdata->super_influence = 0;
    pcdata->aiding_scheme = 0;

    for (x = 0; x < 25; x++) {
      pcdata->previous_focus_vnum[x] = 0;
      pcdata->previous_focus_level[x] = 0;
    }
    for (x = 0; x < 3; x++) {
      pcdata->last_praise_message[x] = str_dup("");
      pcdata->last_diss_message[x] = str_dup("");
    }
    pcdata->factionwin = 0;
    pcdata->last_rumor = str_dup("");
    for (x = 0; x < 15; x++)
    pcdata->territory_editing[x] = 0;

    for (x = 0; x < 40; x++) {
      pcdata->week_tracker[x] = 0;
      pcdata->life_tracker[x] = 0;
    }
    for (x = 0; x < 4; x++) {
      pcdata->mark[x] = str_dup("");
      pcdata->mark_timer[x] = 0;
    }
    for (x = 0; x < 5; x++)
    pcdata->ability_trees[x] = 0;
    for (x = 0; x < 20; x++)
    pcdata->abilcools[x] = 0;

    for (x = 0; x < 10; x++) {
      pcdata->augdisc_timer[x] = 0;
      pcdata->augdisc_disc[x] = 0;
      pcdata->augdisc_level[x] = 0;

      pcdata->augskill_timer[x] = 0;
      pcdata->augskill_skill[x] = 0;
      pcdata->augskill_level[x] = 0;
    }

    pcdata->haunter = str_dup("");
    pcdata->haunt_timer = 0;

    pcdata->order_target = str_dup("");
    pcdata->tailing = str_dup("");

    pcdata->encounter_status = 0;
    pcdata->encounter_countdown = 0;
    pcdata->encounter_orig_room = 0;
    pcdata->encounter_number = 0;
    pcdata->karma_battery = 0;
    pcdata->genesis_stage = 0;
    pcdata->pathtotal = 0;
    pcdata->escape_timer = 0;

    pcdata->bloodaura = 0;
    pcdata->ambush = 0;
    pcdata->selfesteem = 0;

    // Role Variables
    pcdata->job = 0;
    pcdata->role = 0;

    pcdata->energy_recovery = 0;
    pcdata->extra_powers = 0;

    pcdata->order_type = 0;
    pcdata->order_amount = 0;
    pcdata->order_timer = 0;
    pcdata->tail_timer = 0;
    pcdata->tail_mask = 0;
    pcdata->last_public_room = 0;

    pcdata->guard_number = 0;
    pcdata->guard_faction = 0;
    pcdata->guard_expiration = 0;

    for (x = 0; x < 25; x++) {
      pcdata->customstats[x][0] = 0;
      pcdata->customstats[x][1] = 0;
    }
    for (x = 0; x < 25; x++) {
      for (int y = 0; y < 7; y++)
      pcdata->customstrings[x][y] = str_dup("");
    }
    for (x = 0; x < 20; x++) {
      for (int y = 0; y < 4; y++)
      pcdata->specialcools[x][y] = 0;
    }
    for (x = 0; x < 25; x++) {
      pcdata->specials[x][0] = 0;
      pcdata->specials[x][1] = 0;
    }
    for (x = 0; x < 10; x++) {
      pcdata->delays[x][0] = 0;
      pcdata->delays[x][1] = 0;
      pcdata->delays[x][2] = 0;
    }

    pcdata->lastwarcheck = 0;

    pcdata->history = str_dup("");
    pcdata->chat_history = str_dup("");
    pcdata->char_goals = str_dup("");
    pcdata->char_fears = str_dup("");
    pcdata->char_secrets = str_dup("");
    pcdata->char_timeline = str_dup("");
    pcdata->text_msgs = str_dup("");
    pcdata->photo_msgs = str_dup("");
    pcdata->photo_pose = str_dup("");
    pcdata->file = str_dup("");
    pcdata->classified = str_dup("");
    pcdata->ci_name = str_dup("");
    pcdata->ci_short = str_dup("");
    pcdata->ci_long = str_dup("");
    pcdata->ci_taste = str_dup("");
    pcdata->ci_imprint = str_dup("");
    pcdata->ci_wear = str_dup("");
    pcdata->ci_desc = str_dup("");

    pcdata->ci_myself = str_dup("");
    pcdata->ci_message = str_dup("");
    pcdata->ci_target = str_dup("");
    pcdata->ci_bystanders = str_dup("");
    pcdata->ci_myselfdelayed = str_dup("");
    pcdata->ci_targetdelayed = str_dup("");
    pcdata->ci_bystandersdelayed = str_dup("");

    pcdata->earnt_xp = 0;

    pcdata->ci_discipline = 0;
    pcdata->ci_disclevel = 0;
    pcdata->ci_discipline2 = 0;
    pcdata->ci_disclevel2 = 0;
    pcdata->ci_discipline3 = 0;
    pcdata->ci_disclevel3 = 0;
    pcdata->ci_discipline4 = 0;
    pcdata->ci_disclevel4 = 0;
    pcdata->ci_discipline5 = 0;
    pcdata->ci_disclevel5 = 0;

    for (x = 0; x < 10; x++) {
      pcdata->ci_stats[x] = 0;
    }
    for (x = 0; x < 20; x++) {
      pcdata->shadow_attacks[x][0] = 0;
      pcdata->shadow_attacks[x][1] = 0;
    }

    pcdata->colours = 0;
    pcdata->bandaids = 0;
    pcdata->renames = 0;
    pcdata->caura = str_dup("");
    pcdata->cwalk = str_dup("");
    pcdata->crank = str_dup("");

    pcdata->police_timer = 0;
    pcdata->police_intensity = 0;
    pcdata->police_number = 0;

    pcdata->tresspassing = 0;
    pcdata->rank_type = 0;
    pcdata->version_player = 2;

    pcdata->ci_special = 0;
    pcdata->ci_editing = 0;
    pcdata->ci_cost = 0;
    pcdata->ci_size = 0;
    pcdata->ci_alcohol = 0;
    pcdata->ci_type = 0;
    pcdata->ci_covers = 0;
    pcdata->ci_zips = 0;

    pcdata->ci_absorb = 0;
    pcdata->ci_area = 0;
    pcdata->ci_status = 0;
    pcdata->ci_x = 0;
    pcdata->ci_y = 0;
    pcdata->habits_done = 0;
    pcdata->litup = 0;
    pcdata->autoskip = 0;

    pcdata->ci_mod = 0;
    pcdata->ci_layer = 0;

    pcdata->messages = str_dup("");

    pcdata->timeswept = 0;
    pcdata->deaged = 0;
    pcdata->agemod = 0;
    pcdata->dtrains = 0;
    pcdata->strains = 0;
    pcdata->faction = 0;

    pcdata->total_money = 0;
    pcdata->total_credit = 0;
    pcdata->portaled_from = 0;
    pcdata->penalty = 0;
    pcdata->jetlag = 0;

    pcdata->aexp = 0;
    pcdata->awins = 0;
    pcdata->alosses = 0;
    pcdata->astatus = 0;

    pcdata->fatigue_temp = 0;
    pcdata->fatigue = 0;
    pcdata->difficulty = 5;

    pcdata->maskednumber = 0;

    pcdata->garbled = 0;

    pcdata->abommistcool = 0;
    pcdata->abomblackcool = 0;

    pcdata->breach_origin = 0;
    pcdata->breach_destination = 0;
    pcdata->breach_timer = 0;

    pcdata->eyes_genetic = 0;
    pcdata->hair_genetic = 0;

    pcdata->dream_environment = str_dup("");
    pcdata->dream_description = str_dup("");
    pcdata->dream_intro = str_dup("");

    pcdata->mask_intro_one = str_dup("");
    pcdata->mask_intro_two = str_dup("");

    pcdata->hair_dyed = str_dup("");

    pcdata->editing_territory = str_dup("");

    pcdata->dream_controller = 0;
    pcdata->dream_cooldown = 0;
    pcdata->dream_duration = 0;

    for (x = 0; x < 10; x++) {
      pcdata->conditions[x] = 0;
    }
    for (x = 0; x < 5; x++)
    pcdata->firsts[x] = 0;
    for (x = 0; x < 10; x++)
    pcdata->emotes[x] = 0;

    pcdata->shadow_walk_room = 0;
    pcdata->shadow_walk_cooldown = 0;

    pcdata->brand_timeout = 0;
    pcdata->enthrall_timeout = 0;
    pcdata->enrapture_timeout = 0;
    pcdata->fixation_mourning = 0;
    pcdata->building_fixation_name = str_dup("");
    pcdata->building_fixation_level = 0;
    pcdata->fixation_name = str_dup("");
    pcdata->fixation_level = 0;
    pcdata->fixation_timeout = 0;
    pcdata->fixation_charges = 0;
    pcdata->villain_grab = FALSE;

    pcdata->prey_option_cooldown = 0;
    pcdata->prey_option = 0;
    pcdata->prey_emote_cooldown = 0;
    pcdata->villain_option_cooldown = 0;
    pcdata->villain_option = 0;
    pcdata->villain_emote_cooldown = 0;

    pcdata->coma = 0;
    pcdata->abomtime = 0;
    pcdata->abomcuring = 0;
    pcdata->abominfecting = 0;

    pcdata->diss_target = str_dup("");
    pcdata->diss_message = str_dup("");
    pcdata->survey_comment = str_dup("");
    pcdata->survey_improve = str_dup("");
    pcdata->surveying = str_dup("");
    pcdata->survey_stage = 0;
    pcdata->diss_sendable = 0;
    pcdata->survey_delay = 0;
    for (x = 0; x < 15; x++)
    pcdata->survey_ratings[x] = 0;

    pcdata->confirm_delete = FALSE;

    for (x = 0; x < 10; x++) {
      pcdata->drivenames[x] = str_dup("");
      pcdata->driveloc[x] = 0;
    }

    for (x = 0; x < 10; x++) {
      pcdata->boon_history[x] = 0;
      pcdata->curse_history[x] = 0;
    }
    pcdata->boon = 0;
    pcdata->curse = 0;
    pcdata->boon_timeout = 0;
    pcdata->curse_timeout = 0;
    pcdata->curse_text = str_dup("");
    pcdata->boon_gift = NULL;

    for (x = 0; x < 10; x++) {
      pcdata->garage_name[x] = str_dup("");
      pcdata->garage_desc[x] = str_dup("");
      pcdata->garage_lplate[x] = str_dup("");
      pcdata->garage_typeone[x] = 0;
      pcdata->garage_typetwo[x] = 0;
      pcdata->garage_status[x] = 0;
      pcdata->garage_timer[x] = 0;
      pcdata->garage_cost[x] = 0;
      pcdata->garage_location[x] = 0;
    }
    for (x = 0; x < 20; x++) {
      pcdata->contact_names[x] = str_dup("");
      pcdata->contact_descs[x] = str_dup("");
      pcdata->contact_cooldowns[x] = 0;
      pcdata->contact_jobs[x] = 0;
    }

    for (x = 0; x < 5; x++) {
      pcdata->bonds[x] = str_dup("");
    }
    for (x = 0; x < 10; x++) {
      pcdata->relationship[x] = str_dup("");
      pcdata->relationship_type[x] = 0;
    }
    for (x = 0; x < 6; x++) {
      pcdata->animal_names[x] = str_dup("");
      pcdata->animal_change_to[x] = str_dup("");
      pcdata->animal_change_from[x] = str_dup("");
      pcdata->animal_intros[x] = str_dup("");
      pcdata->animal_descs[x] = str_dup("");
      pcdata->animal_species[x] = str_dup("");
      pcdata->animal_weights[x] = 0;
      pcdata->animal_genus[x] = 0;
      for (int y = 0; y < 25; y++)
      pcdata->animal_stats[x][y] = 0;
    }
    pcdata->active_chatroom = 0;
    pcdata->missed_chat_connections = 0;
    pcdata->missed_rp_connections = 0;
    pcdata->influencer_bank = 0;
    pcdata->suspend_myhaven = 0;
    pcdata->page_timeout = 0;
    pcdata->is_target_encounter = FALSE;

    for (int y = 0; y < 50; y++)
    pcdata->in_chatroom[y] = 0;

    pcdata->photo_attract = 0;
    pcdata->mermaid_change_to = str_dup("");
    pcdata->mermaid_change_from = str_dup("");
    pcdata->wolf_change_to = str_dup("");
    pcdata->wolf_change_from = str_dup("");
    pcdata->radio_action = str_dup("");
    pcdata->makeup_light = str_dup("");
    pcdata->makeup_medium = str_dup("");
    pcdata->makeup_heavy = str_dup("");
    pcdata->pathtraining = 0;
    pcdata->pathlosing = 0;
    pcdata->pathtimer = 0;
    pcdata->partpay_timer = 0;
    pcdata->fulltime_timer = 0;
    pcdata->packcooldown = 0;
    pcdata->teachercooldown = 0;
    pcdata->blood_date = 0;
    pcdata->blood_level = 0;
    pcdata->bond_drop = 0;
    pcdata->payscale = 100;
    pcdata->racial_power = 0;
    pcdata->racial_power_two = 0;

    for (x = 0; x < MAX_ALIAS; x++) {
      pcdata->alias[x] = NULL;
      pcdata->alias_sub[x] = NULL;
    }

    pcdata->security = 0; /* OLC */
    pcdata->squish = 0;

    pcdata->birth_day = 1;
    pcdata->birth_month = 1;
    pcdata->birth_year = 1;

    pcdata->sire_day = 1;
    pcdata->sire_month = 1;
    pcdata->sire_year = 1;

    pcdata->apparant_age = 0;

    /* Set all the default channel colors */
    pcdata->text[0] = (NORMAL);
    pcdata->text[1] = (WHITE);
    pcdata->text[2] = 0;
    pcdata->auction[0] = (BRIGHT);
    pcdata->auction[1] = (YELLOW);
    pcdata->auction[2] = 0;
    pcdata->irl[0] = (NORMAL);
    pcdata->irl[1] = (RED);
    pcdata->irl[2] = 0;
    pcdata->immortal[0] = (BRIGHT);
    pcdata->immortal[1] = (YELLOW);
    pcdata->immortal[2] = 0;
    pcdata->implementor[0] = (BRIGHT);
    pcdata->implementor[1] = (YELLOW);
    pcdata->implementor[2] = 0;
    pcdata->ooc[0] = (NORMAL);
    pcdata->ooc[1] = (CYAN);
    pcdata->ooc[2] = 0;
    pcdata->yells[0] = (NORMAL);
    pcdata->yells[1] = (WHITE);
    pcdata->yells[2] = 0;
    pcdata->osay[0] = (NORMAL);
    pcdata->osay[1] = (GREEN);
    pcdata->osay[2] = 0;
    pcdata->gossip[0] = (NORMAL);
    pcdata->gossip[1] = (MAGENTA);
    pcdata->gossip[2] = 0;
    pcdata->info[0] = (BRIGHT);
    pcdata->info[1] = (YELLOW);
    pcdata->info[2] = 1;
    pcdata->say[0] = (NORMAL);
    pcdata->say[1] = (WHITE);
    pcdata->say[2] = 0;
    pcdata->tells[0] = (NORMAL);
    pcdata->tells[1] = (GREEN);
    pcdata->tells[2] = 0;
    pcdata->reply[0] = (NORMAL);
    pcdata->reply[1] = (GREEN);
    pcdata->reply[2] = 0;
    pcdata->gtell[0] = (NORMAL);
    pcdata->gtell[1] = (GREEN);
    pcdata->gtell[2] = 0;
    pcdata->wiznet[0] = (NORMAL);
    pcdata->wiznet[1] = (GREEN);
    pcdata->wiznet[2] = 0;
    pcdata->room_exits[0] = (NORMAL);
    pcdata->room_exits[1] = (GREEN);
    pcdata->room_exits[2] = 0;
    pcdata->room_things[0] = (NORMAL);
    pcdata->room_things[1] = (CYAN);
    pcdata->room_things[2] = 0;
    pcdata->prompt[0] = (NORMAL);
    pcdata->prompt[1] = (CYAN);
    pcdata->prompt[2] = 0;
    pcdata->fight_death[0] = (BRIGHT);
    pcdata->fight_death[1] = (RED);
    pcdata->fight_death[2] = 0;
    pcdata->fight_yhit[0] = (NORMAL);
    pcdata->fight_yhit[1] = (GREEN);
    pcdata->fight_yhit[2] = 0;
    pcdata->fight_ohit[0] = (NORMAL);
    pcdata->fight_ohit[1] = (YELLOW);
    pcdata->fight_ohit[2] = 0;
    pcdata->fight_thit[0] = (NORMAL);
    pcdata->fight_thit[1] = (RED);
    pcdata->fight_thit[2] = 0;
    pcdata->fight_skill[0] = (BRIGHT);
    pcdata->fight_skill[1] = (WHITE);
    pcdata->fight_skill[2] = 0;
    pcdata->hero[0] = (NORMAL);
    pcdata->hero[1] = (YELLOW);
    pcdata->hero[2] = 0;
    pcdata->hint[0] = (NORMAL);
    pcdata->hint[1] = (YELLOW);
    pcdata->hint[2] = 0;
    pcdata->minioncolor[0] = (NORMAL);
    pcdata->minioncolor[1] = (YELLOW);
    pcdata->minioncolor[2] = 0;
    pcdata->pray[0] = (NORMAL);
    pcdata->pray[1] = (YELLOW);
    pcdata->pray[2] = 0;
    pcdata->newbie[0] = (BRIGHT);
    pcdata->newbie[1] = (GREEN);
    pcdata->newbie[2] = 0;

    /* Init bond structure in pcdata to NULL */

    // Shapeshifting defaults - Discordance
    pcdata->private_security = str_dup("");
    pcdata->wolfdesc = str_dup("");
    pcdata->wolfintro = str_dup("An unnaturally large wolf");
    pcdata->mermaiddesc = str_dup("");
    pcdata->mermaidintro = str_dup("");

    // Institute defaults
    pcdata->institute_contraband_violations = 0;
    pcdata->clique_role = 0;
    pcdata->clique_rolechange = 0;

    // Sex variable defaults
    pcdata->last_sex = 0; // Date of last sex
    pcdata->last_sextype = 0; // Last sex_type
    pcdata->last_shower = 0; // Date of last shower
    pcdata->hp_sex = 0;
    pcdata->sex_dirty = FALSE;       // If dirty from sex
    pcdata->sexing = NULL;           // Sex partner
    pcdata->sex_type = str_dup("");  // Coital/Noncoital/Outercourse
    pcdata->sex_risk = str_dup("");  // Safe or risky
    pcdata->sex_penetration = FALSE; // Whether sex threatens virginity
    pcdata->last_sexed[0] = str_dup("");
    pcdata->last_sexed[1] = str_dup("");
    pcdata->last_sexed[2] = str_dup("");
    for (int i = 0; i < 3; i++) {
      pcdata->last_sexed_ID[i] = 0;
    }
    pcdata->last_true_sexed_ID = 0;
    pcdata->last_praised[0] = str_dup("");
    pcdata->last_praised[1] = str_dup("");
    pcdata->last_praised[2] = str_dup("");
    pcdata->last_dissed[0] = str_dup("");
    pcdata->last_dissed[1] = str_dup("");
    pcdata->last_dissed[2] = str_dup("");
    pcdata->sex_potency = 100;       // Sperm count, mobility, etc
    pcdata->daily_upkeep = 0;        // Date last updated
    pcdata->manual_upkeep = 0;       // Date last updated manually
    pcdata->menstruation = 0;        // Date cycle started
    pcdata->natural_fertility = 100; // Egg viability
    pcdata->testosterone = 0;
    pcdata->maledevelopment = 0;
    pcdata->estrogen = 0;
    pcdata->femaledevelopment = 0;
    pcdata->ovulation = 0;      // Date of last ovulation
    pcdata->virginity_lost = 0; // Date of mystic virginity loss
    pcdata->hymen_lost = 0;     // Date of physical virginity loss
    pcdata->inseminated = 0;    // Date inseminated
    pcdata->inseminated_type = 0;
    pcdata->inseminated_daddy_ID = 0;
    pcdata->impregnated = 0; // Date impregnated
    pcdata->impregnated_type = 0;
    pcdata->impregnated_daddy_ID = 0;
    pcdata->due_date = 0;           // Date baby is due
    pcdata->auntflo_called = FALSE; // Menses notification check

    pcdata->count_dreamsex = 0;     // dreamsex counter

    pcdata->cdisc_name = str_dup("");
    pcdata->cdisc_range = 0;
    pcdata->without_sleep = 0;

    /*Ghost variable defaults -Discordance                    */
    pcdata->ghost_pool = 0;       // action pool for ghosts
    pcdata->ghost_banishment = 0; // date ghost was banished
    pcdata->final_death_date = 0; // date PC was removed from play
    pcdata->ghost_room = 0;
    pcdata->ghost_wound = 0;

    pcdata->spectre = 0;
    /* Not currently in a linked state */

    pcdata->protecting = NULL;
    pcdata->connected_to = NULL;
    pcdata->dream_link = NULL;
    pcdata->process_target = NULL;
    pcdata->tracing = NULL;
    pcdata->scheme_running = NULL;
    pcdata->scheme_request = NULL;
    pcdata->wardrobe_pointer = NULL;
    pcdata->scheme_requester = NULL;
    pcdata->wardrobe_direction = 0;
    pcdata->process_timer = 0;
    pcdata->process = 0;
    pcdata->process_subtype = 0;
    pcdata->process_argumentone = str_dup("");
    pcdata->process_argumenttwo = str_dup("");
    pcdata->connection_stage = 0;
    pcdata->bond_offer = NULL;
    pcdata->sr_connection = NULL;
    pcdata->offering = NULL;
    pcdata->offer_type = 0;
    pcdata->cansee = NULL;
    pcdata->cansee2 = NULL;
    pcdata->cansee3 = NULL;

    pcdata->customizing = NULL;

    pcdata->account = NULL;

    /* PC Quest info */

    pcdata->home = 0;

    // NameMaps
    pcdata->male_names = new NameMap;
    pcdata->female_names = new NameMap;

    pcdata->intro_desc = str_dup("");

    pcdata->creation_location = CR_ALL;
    pcdata->spec_trust = FALSE;

    pcdata->pNote = NULL;
    pcdata->pNoteBoard = NoteBoard::getBoard(string("announcements"));
    pcdata->lastReadTimes = new map<string, long>;
    pcdata->noteText = str_dup("");
    pcdata->secondary_timer = 0;
    pcdata->tertiary_timer = 0;
    pcdata->idling = 0;
    pcdata->idling_values[0] = 0;
    pcdata->idling_values[1] = 0;

    for (int b = 0; b < 5; b++)
    pcdata->recent_command[b] = NULL;

    vector<NoteBoard *> nb = NoteBoard::getBoards();
    vector<NoteBoard *>::iterator it;
    for (it = nb.begin(); it != nb.end(); ++it) {
      (*pcdata->lastReadTimes)[(*it)->getName()] = 0;
    }

    return;
  }

  // This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

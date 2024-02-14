#ifndef _GSN_H_
#define _GSN_H_

#if defined(__cplusplus)
extern "C" {
#endif

#define GSN(gsn) ,gsn

//sh_int useless_filler_variable

/*
 * These are skill_lookup return values for common skills and spells.
 * THIS FILE HAS BEEN NICELY FORMATTED AND ALPHEBETIZED.  DO NOT MESS
 * IT UP!!!  - Scaelorn
 */
/**
 * These are the weaves that require gsns in the code.
 */
extern sh_int gsn_boil;
extern sh_int gsn_plague;
extern sh_int gsn_poison;
extern sh_int gsn_refresh;
extern sh_int gsn_regen;
extern sh_int gsn_shatter;
extern sh_int gsn_solid_grip;
extern sh_int gsn_xice_patch;

extern sh_int gsn_cointoss;
extern sh_int gsn_march_song;
extern sh_int gsn_deathstrike;
extern sh_int gsn_quarter_the_soul;
/* End of Weave Section */

/* REMORT SKILLS - ROGUE SECTION */

extern sh_int gsn_black_rain;
extern sh_int gsn_coin_toss;
extern sh_int gsn_crippling_blow;
extern sh_int gsn_cross_throat_slash;
extern sh_int gsn_dagger_hurricane;
extern sh_int gsn_dagger_storm;
extern sh_int gsn_dagger_throw;
extern sh_int gsn_detect_movement;
extern sh_int gsn_direct_injection;
extern sh_int gsn_evade_death;
extern sh_int gsn_halflife;
extern sh_int gsn_high_chant;
extern sh_int gsn_juggle;
extern sh_int gsn_light_sleeper;
extern sh_int gsn_needles;
extern sh_int gsn_parting_blow;
extern sh_int gsn_patched_cloak;
extern sh_int gsn_final_death;
extern sh_int gsn_rogue_crit;
extern sh_int gsn_scar;
extern sh_int gsn_shadowstab;
extern sh_int gsn_stun_shot;
extern sh_int gsn_sweeping_slash;
extern sh_int gsn_tattered_cloak;

/* Poisons */
extern sh_int gsn_black_widow;
extern sh_int gsn_blinding_stars;
extern sh_int gsn_crippling_roots;
extern sh_int gsn_flaming_heart;
extern sh_int gsn_greek_fire;
extern sh_int gsn_paralyzing_poison;
extern sh_int gsn_serrated_blades;
extern sh_int gsn_sleeper_stab;
extern sh_int gsn_strength_sapper;
extern sh_int gsn_webbing;
extern sh_int gsn_poison_cleanse;

/* Songs */
extern sh_int gsn_chant_battle;
extern sh_int gsn_chant_peace;
extern sh_int gsn_final_battle;
extern sh_int gsn_song_march;
extern sh_int gsn_hunt_horn;  
extern sh_int gsn_song_swords; 
extern sh_int gsn_tales_dragon;
extern sh_int gsn_tales_hunt;  
extern sh_int gsn_whistle_wind;


/* End Remort Skills */
extern sh_int gsn_recall;
extern sh_int gsn_subdue;
extern sh_int gsn_bow;
extern sh_int gsn_lore;

/* Skills? */
extern sh_int gsn_ogier;
extern sh_int gsn_shadowwalk;

/* Other Assorted Skills */
extern sh_int gsn_bash;

/* Blacksmith Tree */
extern sh_int gsn_repair;
extern sh_int gsn_sharpen;

extern sh_int gsn_dirt;
extern sh_int gsn_hand_to_hand;
extern sh_int gsn_haggle;
extern sh_int gsn_kick;
extern sh_int gsn_trip;

/**
 * Channeler Trees
 */

/* Concentration Tree */
extern sh_int gsn_concentration;
extern sh_int gsn_hunger_resistance;
extern sh_int gsn_weather_resistance;
extern sh_int gsn_meditation;

/* Detect Channeling */
extern sh_int gsn_detect_channeling;
extern sh_int gsn_flow_sight;  
extern sh_int gsn_invert_detection;
extern sh_int gsn_opposition;
extern sh_int gsn_sense_ability;
extern sh_int gsn_weave_sight;
 
/* Grasping Tree */
extern sh_int gsn_grasp;
extern sh_int gsn_advanced_grasp;
extern sh_int gsn_expert_grasp; 
extern sh_int gsn_master_grasp;
extern sh_int gsn_break_shielding;

/* Tie Tree */
extern sh_int gsn_block_weaves;
extern sh_int gsn_cut; 
extern sh_int gsn_tie;
extern sh_int gsn_untie;

/* Weave Handling Tree */   
extern sh_int gsn_weave_handling;
extern sh_int gsn_advanced_handling;
extern sh_int gsn_focus;
extern sh_int gsn_split_weave;
extern sh_int gsn_triple_weave;
extern sh_int gsn_oneness;

/* End of Channeler Skill Trees */

/**
 * Equestrian Skill Trees
 */
/* Animal Handling Tree */
extern sh_int gsn_animal_handling;
extern sh_int gsn_bird_messanger;

/* Horse Riding Tree */   
extern sh_int gsn_riding;
extern sh_int gsn_combat_riding;  
extern sh_int gsn_charge;
extern sh_int gsn_master_riding;

/* End of Equestrian Skill Trees */
   
/**
 * Stealth Skill Trees
 */
/* Hide Tree */
extern sh_int gsn_hide;
extern sh_int gsn_advanced_hide; 
extern sh_int gsn_conceal;
extern sh_int gsn_master_hide;

/* Mug Tree */
extern sh_int gsn_mug;
extern sh_int gsn_steal;
extern sh_int gsn_theft;

/* Nimble Tree */
extern sh_int gsn_nimble;
extern sh_int gsn_palm;
extern sh_int gsn_pick;
 
/* Sneak Tree */
extern sh_int gsn_sneak;
extern sh_int gsn_advanced_sneak;
extern sh_int gsn_expert_sneak; 
extern sh_int gsn_master_sneak;

/* Trap Tree */ 
extern sh_int gsn_animal_trap;
extern sh_int gsn_pitfall;
extern sh_int gsn_loose_stone;
extern sh_int gsn_rope_trap;
extern sh_int gsn_hangman;
extern sh_int gsn_disarm_trap;

/* Attack Tree */
extern sh_int gsn_assassinate;
extern sh_int gsn_backstab;
extern sh_int gsn_advanced_backstab;
extern sh_int gsn_circle;
extern sh_int gsn_sap;


/* End of Stealth Skill Trees */

/**
 * Survival Skill Trees
 */
/* Awareness Tree */
extern sh_int gsn_awareness;
extern sh_int gsn_advanced_awareness;
extern sh_int gsn_expert_awareness;
extern sh_int gsn_foresight;

/* Campfire Tree */
extern sh_int gsn_campfire;
extern sh_int gsn_bury_fire;
extern sh_int gsn_block_wind;
extern sh_int gsn_hide_camp;

/* Collection Tree */
extern sh_int gsn_collect_wood;
extern sh_int gsn_create_fire;   
extern sh_int gsn_create_arrows;
extern sh_int gsn_create_spears;
extern sh_int gsn_create_bows;
extern sh_int gsn_sharpen_wood;

/* Endurance Tree */
extern sh_int gsn_endurance;
extern sh_int gsn_enhanced_endurance;

/* Forage Tree */
extern sh_int gsn_food_forage;
extern sh_int gsn_water_forage;
extern sh_int gsn_create_well;  
extern sh_int gsn_kindling_forage;
extern sh_int gsn_search;

/* Tracking Tree */   
extern sh_int gsn_tracking; 
extern sh_int gsn_advanced_tracking;
extern sh_int gsn_expert_tracking;
extern sh_int gsn_cover_tracks;
extern sh_int gsn_mask_path;     
extern sh_int gsn_fake_trail;   

/* End of Survival Skill Trees */

/**
 * Weapon Trees
 */
extern sh_int gsn_axe;
extern sh_int gsn_bow;
extern sh_int gsn_dagger;
extern sh_int gsn_flail;
extern sh_int gsn_mace;
extern sh_int gsn_polearm;
extern sh_int gsn_quarterstaff;
extern sh_int gsn_spear;
extern sh_int gsn_sword;
extern sh_int gsn_whip;

/* Sword Parry Skills */
extern sh_int gsn_axe_parry;
extern sh_int gsn_dagger_parry;
extern sh_int gsn_mace_parry;
extern sh_int gsn_staff_parry;
extern sh_int gsn_spear_parry;
extern sh_int gsn_sword_parry;

/* Combat/Expert Weapon Skills */
extern sh_int gsn_advanced_axe;
extern sh_int gsn_advanced_dagger;
extern sh_int gsn_advanced_flail;
extern sh_int gsn_advanced_mace;
extern sh_int gsn_advanced_spear;
extern sh_int gsn_advanced_staff;
extern sh_int gsn_advanced_sword;
extern sh_int gsn_advanced_whip;

extern sh_int gsn_expert_axe;
extern sh_int gsn_expert_dagger;
extern sh_int gsn_expert_flail;
extern sh_int gsn_expert_mace;
extern sh_int gsn_expert_spear;
extern sh_int gsn_expert_staff;
extern sh_int gsn_expert_sword;
extern sh_int gsn_expert_whip;

extern sh_int gsn_master_axe;
extern sh_int gsn_master_dagger;
extern sh_int gsn_master_flail;
extern sh_int gsn_master_mace;
extern sh_int gsn_master_spear;
extern sh_int gsn_master_staff;
extern sh_int gsn_master_sword;
extern sh_int gsn_master_whip;

/* Combat Strike Skills */
extern sh_int gsn_strike_axe;
extern sh_int gsn_strike_dagger;
extern sh_int gsn_strike_flail;
extern sh_int gsn_strike_mace;
extern sh_int gsn_strike_spear;
extern sh_int gsn_strike_staff;
extern sh_int gsn_strike_sword;
extern sh_int gsn_strike_whip;

/* Axe Tree Skills */
extern sh_int gsn_woodsman_hand;
extern sh_int gsn_combat_axe;

/* Dagger Tree Skills */
extern sh_int gsn_carving_dagger;
extern sh_int gsn_neck_thrust;
extern sh_int gsn_pierce_dagger;
extern sh_int gsn_slash_dagger;

/* Flail Tree Skills */
extern sh_int gsn_twirl_flail;
extern sh_int gsn_smash_flail;

/* Mace Tree Skills */
extern sh_int gsn_bash_mace;
extern sh_int gsn_thrust_mace;

/* Spear Tree Skills */
extern sh_int gsn_bash_spear;
extern sh_int gsn_thrust_spear;

/* Staff Tree Skills */
extern sh_int gsn_bash_staff;
extern sh_int gsn_double_staff;
extern sh_int gsn_thrust_staff;

/* Sword Tree Skills */
extern sh_int gsn_jab_sword;
extern sh_int gsn_slash_sword;

/* Whip Tree Skills */
extern sh_int gsn_lasso_whip;
extern sh_int gsn_slash_whip;

/* Combat Weapon Attacks */
extern sh_int gsn_moonlight;
extern sh_int gsn_silver_doom;
extern sh_int gsn_spiked_fury;
extern sh_int gsn_chain_storm;
extern sh_int gsn_crushing_blow;
extern sh_int gsn_bone_smasher;
extern sh_int gsn_shadowstrike;
extern sh_int gsn_silent_eye;
extern sh_int gsn_quickstrike;
extern sh_int gsn_twirling_fate;
extern sh_int gsn_three_step;     
extern sh_int gsn_eruption_blade;
extern sh_int gsn_whiplash;
extern sh_int gsn_sonic_slash;   
/*
 * End of Weapon Trees
 */

/* 
 * Defensive Skill Trees 
 */

 /* Armor Usage Tree */ 

extern sh_int gsn_armor_usage;
extern sh_int gsn_enhanced_arm_guard;
extern sh_int gsn_enhanced_head_guard;
extern sh_int gsn_enhanced_leg_guard;
extern sh_int gsn_enhanced_neck_guard;
extern sh_int gsn_enhanced_torso_guard;

 /* Blocking Patterns Tree */

extern sh_int gsn_block_patterns;
extern sh_int gsn_block_slash; 
extern sh_int gsn_block_bash;
extern sh_int gsn_block_pierce;
extern sh_int gsn_block_expert;
 
 /* Evasion Tree */

extern sh_int gsn_dodge;
extern sh_int gsn_advanced_evasion;
extern sh_int gsn_expert_evasion;   
extern sh_int gsn_sidestep;

 /* Rescue Tree */

extern sh_int gsn_rescue;
extern sh_int gsn_reinforced_rescue;

 /* Shield Usage Tree */ 

extern sh_int gsn_shield_usage;
extern sh_int gsn_shield_bash; 
extern sh_int gsn_shield_block;
 
/*
 * End Defensive Skill Trees
 */

/* 
 * Offensive Skill Trees 
 */

 /* Combat Tree */
extern sh_int gsn_combat_strike;
extern sh_int gsn_advanced_strike;
extern sh_int gsn_expert_strike;
extern sh_int gsn_master_strike;

 /* Damaging Strike Tree */
extern sh_int gsn_damage_strike;
extern sh_int gsn_arm_break;    
extern sh_int gsn_leg_break;
extern sh_int gsn_face_smash;  

 /* Dual Wield Tree */   
extern sh_int gsn_dual;
extern sh_int gsn_dual_blunt;
extern sh_int gsn_dual_exotic;
extern sh_int gsn_dual_mix;
extern sh_int gsn_dual_slash;   

 /* Enhanced Attack */   
extern sh_int gsn_enhanced_attack;
extern sh_int gsn_enhanced_damage;
extern sh_int gsn_reinforced_damage;
extern sh_int gsn_dual_bonus;   
extern sh_int gsn_enhanced_shield;
extern sh_int gsn_enhanced_sharp;

 /* Enhanced Speed */   
extern sh_int gsn_enhanced_speed;
extern sh_int gsn_multiple_attacks;
extern sh_int gsn_enhanced_multiple;
extern sh_int gsn_bonus_attacks;

extern sh_int gsn_quicksteps;
extern sh_int gsn_quicksilver;

/*
 * End Offensive Skill Tree
 */

/**
 * Advanced Skill Lines
 */

/*
 * Sword Apprentice Skills
 */

extern sh_int gsn_swordsmanship;
extern sh_int gsn_three_step;
extern sh_int gsn_free_hand;
extern sh_int gsn_deep_breath;
extern sh_int gsn_flickering_steel;
extern sh_int gsn_eruption_blade;

extern sh_int gsn_damage_enhancement;
extern sh_int gsn_composure;
extern sh_int gsn_double_grip;
extern sh_int gsn_firm_grip;
extern sh_int gsn_disarm;
extern sh_int gsn_weapon_handling;
extern sh_int gsn_parry_strike;
extern sh_int gsn_crush_weapon;
extern sh_int gsn_wristsnap;
extern sh_int gsn_silver_strike;
/*
 * End Sword Apprentice Skill Tree
 */

/* 
 * Duellist Tree
 */
extern sh_int gsn_calm;
extern sh_int gsn_shadowstrike;
extern sh_int gsn_bladeweave;
extern sh_int gsn_silent_eye;

extern sh_int gsn_armor_pierce;
extern sh_int gsn_burst;
extern sh_int gsn_deep_wounds;
extern sh_int gsn_feint;
extern sh_int gsn_grappling;
extern sh_int gsn_lunge;
extern sh_int gsn_pivot;
extern sh_int gsn_stance_fool;
extern sh_int gsn_stance_ox;
extern sh_int gsn_stance_plough;
extern sh_int gsn_tranquil_fury;

/*
 * End Duellist Tree
 */

/*
 * Ranger Class
 */
extern sh_int gsn_archery;
extern sh_int gsn_advanced_archery;
extern sh_int gsn_expert_archery;
extern sh_int gsn_master_archery;
extern sh_int gsn_bow_handling; 
extern sh_int gsn_taught_string;
extern sh_int gsn_snapshot;
extern sh_int gsn_quickdraw;
extern sh_int gsn_arrow_sharp;
extern sh_int gsn_arrow_pierce;
extern sh_int gsn_arrow_blunt;
extern sh_int gsn_arrow_serrated;
extern sh_int gsn_arrow_flamming;
extern sh_int gsn_mental_release;
extern sh_int gsn_strength_of_birgitte; 
extern sh_int gsn_speed_of_birgitte;
extern sh_int gsn_sight_of_birgitte;
/*
 * Battlefield Tactics
 */
extern sh_int gsn_tactical_formation;
extern sh_int gsn_know_enemy;
extern sh_int gsn_lesser_nourishment;
extern sh_int gsn_soldier_sleep;
extern sh_int gsn_physical_fitness;

extern sh_int gsn_advanced_armor_usage;
extern sh_int gsn_advanced_tactical_formation;
extern sh_int gsn_block;
extern sh_int gsn_defend;
extern sh_int gsn_deploy;
extern sh_int gsn_expert_tactical_formation;
extern sh_int gsn_first_strike;
extern sh_int gsn_hp_buffer;
extern sh_int gsn_master_tactical_formation;
extern sh_int gsn_military_strike;
extern sh_int gsn_rear_guard_placement;
extern sh_int gsn_safe_retreat;
extern sh_int gsn_shield_enhancement;
extern sh_int gsn_tactical_strike;

/*
 * End Battlefield Tactics
 */

/**
 * Rogue Advanced Skill Line
 */

extern sh_int gsn_double_prof;
extern sh_int gsn_sync_knife;
extern sh_int gsn_slit_slash;
extern sh_int gsn_betrayal;
extern sh_int gsn_sliding_steel;
extern sh_int gsn_living_blade;
extern sh_int gsn_eavesdrop;
extern sh_int gsn_loot_corpse;
extern sh_int gsn_appraise_item;
extern sh_int gsn_blind_luck;
/*
 * End Rogue Skill Line
 */

/* Wisdom Herbs - ALL Herbs go Here! (Palin */
extern sh_int gsn_adilyn;
extern sh_int gsn_bitterroot;
extern sh_int gsn_graymoss;
extern sh_int gsn_heartsblush;
extern sh_int gsn_marisin;
extern sh_int gsn_milkroot;
extern sh_int gsn_mountains_tooth;
extern sh_int gsn_night_blossom;
extern sh_int gsn_silverleaf;
extern sh_int gsn_white;

/* Guild Skills - ALL guild skills go here (Scaelorn */

/* True power*/
extern sh_int gsn_bubble_of_evil;
extern sh_int gsn_cleanse;
extern sh_int gsn_fullshield;
extern sh_int gsn_insomnia;
extern sh_int gsn_luck_of_darkone;
extern sh_int gsn_pain;
extern sh_int gsn_rip_pattern;
extern sh_int gsn_touch_of_darkone;
extern sh_int gsn_tp_heal;

/* Weaves */
extern sh_int gsn_break_lock;
extern sh_int gsn_blindness;
extern sh_int gsn_bind;
extern sh_int gsn_compulsion;
extern sh_int gsn_concealing_mist;
extern sh_int gsn_invis;
extern sh_int gsn_shielding;
extern sh_int gsn_sleep;
extern sh_int gsn_taint;

// This must be at the end of the file - Scaelorn
#if defined(__cplusplus)
}
#endif

#endif


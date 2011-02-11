// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

// ==========================================================================
//
// TODO:
//  Sooner:
//   * Recheck the fixmes.
//   * Measure and implement the formula for rage from damage taken 
//     (n.b., based on percentage of HP for unmitigated hit size).
//   * Watch Raging Blow and see if Blizzard fix the bug where it's
//     not refunding 80% of the rage cost if it misses.
//   * Consider testing the rest of the abilities for that too.
//   * Sanity check init_buffs() wrt durations and chances.
//  Later:
//   * Verify that Colossus Smash itself doesn't ignore armor.
//   * Get Heroic Strike to trigger properly "off gcd" using priority.
//   * Move the bleeds out of being warrior_attack_t to stop them
//     triggering effects or having special cases in the class.
//   * Prot? O_O
//
// NOTES:
//  Damage increase types per spell:
//
//   * battle_stance                    = 21156 = mod_damage_done% (0x7f)
//   * berserker_stance                 =  7381 = mod_damage_done% (0x7f)
//   * blood_frenzy                     = 30070 = mod_damage_taken% (0x1)
//                                      = 46857 = ???
//   * cruelty                          = 12582 = add_flat_mod_spell_crit_chance (7)
//   * death_wish                       = 12292 = mod_damage_done% (0x1)
//   * dual_wield_specialization        = 23588 = mod_damage_done% (0x7f)
//   * enrage (bastion_of_defense)      = 57516 = mod_damage_done% (0x1)
//   * enrage (fury)                    = 14202 = mod_damage_done% (0x1)
//   * enrage (wrecking_crew)           = 57519 = mod_damage_done% (0x1)
//   * heavy_repercussions              = 86896 = add_flat_mod_spell_effect2 (12)
//   * hold_the_line                    = 84620 = mod_crit% (127)
//   * improved_revenge                 = 12799 = add_percent_mod_generic
//   * incite                           = 50687 = add_flat_mod_spell_crit_chance (7)
//                                      = 86627 = add_flat_mod_spell_crit_chance (7)
//   * inner_rage                       =  1134 = mod_damage_done% (0x7f)
//   * lambs_to_the_slaughter           = 84586 = add_percent_mod_generic
//   * meat_cleaver                     = 85739 = add_percent_mod_generic
//   * rampage                          = 29801 = mod_crit% (7)
//   * recklessness                     =  1719 = add_flat_mod (7)
//   * rude_interruption                = 86663 = mod_damage_done% (0x7f)
//   * singleminded_fury                = 81099 = mod_damage_done% (0x7f)
//   * sword_and_board                  = 46953 = add_flat_mod_spell_crit_chance (7)
//   * thunderstruck                    = 87096 = add_percent_mod_generic
//   * war_acacdemy                     = 84572 = add_percent_mod_generic
//   * warrior_t10_melee_2pc_bonus      = 70854 = mod_damage_done% (0x1)
//   * warrior_t10_melee_4pc_bonus      = 70847 = mod_damage_done% (0x7f)
//   * warrior_tier11_dps_2pc_bonus     = 90293 = add_percent_mod_generic
//   * warrior_tier11_dps_4pc_bonus     = 90294 = mod_melee_attack_power%
//   * twohanded_weapon_specialization  = 12712 = mod_damage_done% (0x1)
//   * unshackled_fury                  = 76856 = add_percent_mod_spell_effect1/2
//
//   * glyph_of_bloodthirst             = 58367 = add_percent_mod
//   * glyph_of_devastate               = 58388 = add_flat_mod_spell_crit_chance (7)
//   * glyph_of_mortal_strike           = 58368 = add_percent_mod_generic
//   * glyph_of_overpower               = 58386 = add_percent_mod_generic
//   * glyph_of_raging_blow             = 58370 = add_flat_mod_spell_crit_chance (7)
//   * glyph_of_revenge                 = 58364 = add_percent_mod_generic
//   * glyph_of_shield_slam             = 58375 = add_percent_mod_generic
//   * glyph_of_slam                    = 58385 = add_flat_mod_spell_crit_chance (7)
// 
// ==========================================================================


// ==========================================================================
// Warrior
// ==========================================================================

enum warrior_stance { STANCE_BATTLE=1, STANCE_BERSERKER, STANCE_DEFENSE=4 };

struct warrior_t : public player_t
{
  bool instant_flurry_haste;
  int initial_rage;
  double max_deep_wounds_refresh;

  // Active
  action_t* active_deep_wounds;
  action_t* active_opportunity_strike;
  int       active_stance;

  // Active Spells
  struct active_spells_t
  {
    active_spell_t* battle_shout;
    active_spell_t* berserker_rage;
    active_spell_t* bladestorm;
    active_spell_t* bloodthirst;
    active_spell_t* cleave;
    active_spell_t* colossus_smash;
    active_spell_t* concussion_blow;
    active_spell_t* deadly_calm;
    active_spell_t* death_wish;
    active_spell_t* devastate;
    active_spell_t* execute;
    active_spell_t* heroic_strike;
    active_spell_t* inner_rage;
    active_spell_t* mortal_strike;
    active_spell_t* overpower;
    active_spell_t* pummel;
    active_spell_t* raging_blow;
    active_spell_t* recklessness;
    active_spell_t* rend;
    active_spell_t* revenge;
    active_spell_t* shattering_throw;
    active_spell_t* shield_bash;
    active_spell_t* shield_block;
    active_spell_t* shield_slam;
    active_spell_t* shockwave;
    active_spell_t* slam;
    active_spell_t* sweeping_strikes;
    active_spell_t* thunder_clap;
    active_spell_t* victory_rush;
    active_spell_t* whirlwind;
  };
  active_spells_t   active_spells;

  // Buffs
  buff_t* buffs_bastion_of_defense;
  buff_t* buffs_battle_stance;
  buff_t* buffs_battle_trance;
  buff_t* buffs_berserker_rage;
  buff_t* buffs_berserker_stance;
  buff_t* buffs_bloodsurge;
  buff_t* buffs_colossus_smash;
  buff_t* buffs_deadly_calm;
  buff_t* buffs_death_wish;
  buff_t* buffs_defensive_stance;
  buff_t* buffs_enrage;
  buff_t* buffs_executioner_talent;
  buff_t* buffs_flurry;
  buff_t* buffs_hold_the_line;
  buff_t* buffs_incite;
  buff_t* buffs_inner_rage;
  buff_t* buffs_lambs_to_the_slaughter;
  buff_t* buffs_meat_cleaver;
  buff_t* buffs_overpower;
  buff_t* buffs_recklessness;
  buff_t* buffs_revenge;
  buff_t* buffs_rude_interruption;
  buff_t* buffs_shield_block;
  buff_t* buffs_sweeping_strikes;
  buff_t* buffs_sword_and_board;
  buff_t* buffs_taste_for_blood;
  buff_t* buffs_thunderstruck;
  buff_t* buffs_victory_rush;
  buff_t* buffs_wrecking_crew;
  buff_t* buffs_tier10_2pc_melee;
  buff_t* buffs_tier11_4pc_melee;

  // Cooldowns
  cooldown_t* cooldowns_colossus_smash;
  cooldown_t* cooldowns_shield_slam;
  cooldown_t* cooldowns_strikes_of_opportunity;

  // Dots
  dot_t* dots_deep_wounds;
  dot_t* dots_rend;

  // Gains
  gain_t* gains_anger_management;
  gain_t* gains_avoided_attacks;
  gain_t* gains_battle_shout;
  gain_t* gains_berserker_rage;
  gain_t* gains_blood_frenzy;
  gain_t* gains_incoming_damage;
  gain_t* gains_melee_main_hand;
  gain_t* gains_melee_off_hand;
  gain_t* gains_shield_specialization;
  gain_t* gains_sudden_death;

  // Glyphs
  struct glyphs_t
  {
    glyph_t* battle;
    glyph_t* berserker_rage;
    glyph_t* bladestorm;
    glyph_t* bloodthirst;
    glyph_t* bloody_healing;
    glyph_t* cleaving;
    glyph_t* colossus_smash;
    glyph_t* command;
    glyph_t* devastate;
    glyph_t* heroic_throw;
    glyph_t* mortal_strike;
    glyph_t* overpower;
    glyph_t* raging_blow;
    glyph_t* resonating_power;
    glyph_t* revenge;
    glyph_t* shield_slam;
    glyph_t* shockwave;
    glyph_t* slam;
    glyph_t* sweeping_strikes;
    glyph_t* victory_rush;
    glyphs_t() { memset( ( void* ) this, 0x0, sizeof( glyphs_t ) ); }
  };
  glyphs_t glyphs;

  // Mastery
  struct war_mastery_t
  {
    mastery_t* critical_block;
    mastery_t* strikes_of_opportunity;
    mastery_t* unshackled_fury;

    war_mastery_t() { memset( ( void* ) this, 0x0, sizeof( war_mastery_t ) ); }
  };
  war_mastery_t mastery;

  // Procs
  proc_t* procs_munched_deep_wounds;
  proc_t* procs_rolled_deep_wounds;
  proc_t* procs_parry_haste;
  proc_t* procs_strikes_of_opportunity;
  proc_t* procs_sudden_death;

  // Random Number Generation
  rng_t* rng_blood_frenzy;
  rng_t* rng_executioner_talent;
  rng_t* rng_impending_victory;
  rng_t* rng_strikes_of_opportunity;
  rng_t* rng_sudden_death;
  rng_t* rng_wrecking_crew;

  // Spec Passives
  struct spec_t
  {
    passive_spell_t* anger_management;    
    passive_spell_t* dual_wield_specialization;
    passive_spell_t* precision;
    passive_spell_t* sentinel;
    passive_spell_t* two_handed_weapon_specialization;

    spec_t() { memset( ( void* ) this, 0x0, sizeof( spec_t ) ); }
  };
  spec_t spec;

  // Talents
  struct talents_t
  {
    // Arms
    talent_t* bladestorm;
    talent_t* blood_frenzy;
    talent_t* deadly_calm;
    talent_t* deep_wounds;
    talent_t* drums_of_war;
    talent_t* impale;
    talent_t* improved_slam;
    talent_t* lambs_to_the_slaughter;
    talent_t* sudden_death;
    talent_t* sweeping_strikes;
    talent_t* tactical_mastery;
    talent_t* taste_for_blood;
    talent_t* war_academy;
    talent_t* wrecking_crew;

    // Fury
    talent_t* battle_trance;
    talent_t* bloodsurge;
    talent_t* booming_voice;
    talent_t* cruelty;
    talent_t* death_wish;
    talent_t* enrage;
    talent_t* executioner;
    talent_t* flurry;
    talent_t* gag_order;
    talent_t* intensify_rage;
    talent_t* meat_cleaver;
    talent_t* raging_blow;
    talent_t* rampage;
    talent_t* rude_interruption;
    talent_t* single_minded_fury;
    talent_t* titans_grip;

    // Prot
    talent_t* bastion_of_defense;
    talent_t* concussion_blow;
    talent_t* devastate;
    talent_t* heavy_repercussions;
    talent_t* hold_the_line;
    talent_t* impending_victory;
    talent_t* improved_revenge;
    talent_t* incite;
    talent_t* shockwave;
    talent_t* shield_mastery;
    talent_t* shield_specialization;
    talent_t* sword_and_board;
    talent_t* toughness;
    talent_t* thunderstruck;

    talents_t() { memset( ( void* ) this, 0x0, sizeof( talents_t ) ); }
  };
  talents_t talents;

  // Up-Times
  uptime_t* uptimes_rage_cap;

  warrior_t( sim_t* sim, const std::string& name, race_type r = RACE_NONE ) :
      player_t( sim, WARRIOR, name, r )
  {
    if ( race == RACE_NONE ) race = RACE_NIGHT_ELF;

    tree_type[ WARRIOR_ARMS       ] = TREE_ARMS;
    tree_type[ WARRIOR_FURY       ] = TREE_FURY;
    tree_type[ WARRIOR_PROTECTION ] = TREE_PROTECTION;

    // Active
    active_deep_wounds        = 0;
    active_opportunity_strike = 0;
    active_stance             = STANCE_BATTLE;

    // Cooldowns
    cooldowns_colossus_smash         = get_cooldown( "colossus_smash"         );
    cooldowns_shield_slam            = get_cooldown( "shield_slam"            );
    cooldowns_strikes_of_opportunity = get_cooldown( "strikes_of_opportunity" );

    // Dots
    dots_deep_wounds = get_dot( "deep_wounds" );
    dots_rend        = get_dot( "rend"        );

    instant_flurry_haste = true;
    initial_rage = 0;
    max_deep_wounds_refresh = 6.0;

    create_talents();
    create_glyphs();
    create_options();
  }

  // Character Definition
  virtual void      init_talents();
  virtual void      init_spells();
  virtual void      init_base();
  virtual void      init_scaling();
  virtual void      init_buffs();
  virtual void      init_gains();
  virtual void      init_procs();
  virtual void      init_uptimes();
  virtual void      init_rng();
  virtual void      init_actions();
  virtual void      combat_begin();
  virtual double    composite_attack_power_multiplier() SC_CONST;
  virtual double    composite_attack_hit() SC_CONST;
  virtual double    matching_gear_multiplier( const attribute_type attr ) SC_CONST;
  virtual double    composite_tank_block() SC_CONST;
  virtual double    composite_tank_crit( const school_type school ) SC_CONST;
  virtual void      reset();
  virtual void      interrupt();
  virtual void      regen( double periodicity );
  virtual double    resource_loss( int resurce, double amount, action_t* );
  virtual void      create_options();
  virtual action_t* create_action( const std::string& name, const std::string& options );
  virtual int       decode_set( item_t& item );
  virtual int       primary_resource() SC_CONST { return RESOURCE_RAGE; }
  virtual int       primary_role() SC_CONST;
  virtual double    assess_damage( double amount, const school_type school, int    dmg_type, int result, action_t* a );
  virtual void      copy_from( player_t* source );
};

namespace   // ANONYMOUS NAMESPACE ==========================================
{

// ==========================================================================
// Warrior Attack
// ==========================================================================

struct warrior_attack_t : public attack_t
{
  int stancemask;

  warrior_attack_t( const char* n, warrior_t* p, const school_type s=SCHOOL_PHYSICAL, int t=TREE_NONE, bool special=true  ) :
      attack_t( n, p, RESOURCE_RAGE, s, t, special ),
      stancemask( STANCE_BATTLE|STANCE_BERSERKER|STANCE_DEFENSE )
  {
    _init_warrior_attack_t();
  }

  warrior_attack_t( const char* n, uint32_t id, warrior_t* p, int t=TREE_NONE, bool special = true ) :
      attack_t( n, id, p, t, special ),
      stancemask( STANCE_BATTLE|STANCE_BERSERKER|STANCE_DEFENSE )
  {
    _init_warrior_attack_t();
  }

  /* Class spell data based construction, spell name in s_name */
  warrior_attack_t( const char* n, const char* s_name, warrior_t* p ) :
    attack_t( n, s_name, p, TREE_NONE, true ),
    stancemask( STANCE_BATTLE|STANCE_BERSERKER|STANCE_DEFENSE )
  { 
    _init_warrior_attack_t();
  }

  void _init_warrior_attack_t()
  {
    may_crit   = true;
    may_glance = false;
    // normalize_weapon_speed is set correctly by parse_data now
   }

  virtual double armor() SC_CONST;

  virtual void   consume_resource();
  virtual double cost() SC_CONST;
  virtual void   execute();
  virtual double calculate_weapon_damage();
  virtual void   player_buff();
  virtual bool   ready();
  virtual void   assess_damage( player_t* t, double amount, int dmg_type, int travel_result );
  virtual void   parse_options( option_t* options, const std::string& options_str );
};


// ==========================================================================
// Static Functions
// ==========================================================================

// trigger_blood_frenzy =====================================================

static void trigger_blood_frenzy( action_t* a )
{
  warrior_t* p = a -> player -> cast_warrior();

  if ( ! p -> talents.blood_frenzy -> ok() )
    return;

  player_t* t = a -> target;

  // Don't alter the duration if it is set to 0 (override/optimal_raid)

  if ( t -> debuffs.blood_frenzy_bleed -> buff_duration > 0 )
  {
    t -> debuffs.blood_frenzy_bleed -> buff_duration = a -> num_ticks * a -> base_tick_time;
  }
  if ( t -> debuffs.blood_frenzy_physical -> buff_duration > 0 )
  {
    t -> debuffs.blood_frenzy_physical -> buff_duration = a -> num_ticks * a -> base_tick_time;
  }

  double rank = p -> talents.blood_frenzy -> rank();

  if ( rank * 15 >= t -> debuffs.blood_frenzy_bleed -> current_value )
  {
    t -> debuffs.blood_frenzy_bleed -> trigger( 1, rank * 15 );
    t -> debuffs.blood_frenzy_bleed -> source = p;
  }
  if ( rank * 2 >= t -> debuffs.blood_frenzy_physical -> current_value )
  {
    t -> debuffs.blood_frenzy_physical -> trigger( 1, rank * 2 );
    t -> debuffs.blood_frenzy_physical -> source = p;
  }
}

// trigger_bloodsurge =======================================================

static void trigger_bloodsurge( action_t* a )
{
  warrior_t* p = a -> player -> cast_warrior();

  if ( ! p -> talents.bloodsurge -> ok() )
    return;

  p -> buffs_bloodsurge -> trigger();
}

// trigger_deep_wounds ======================================================

static void trigger_deep_wounds( action_t* a )
{
  warrior_t* p = a -> player -> cast_warrior();
  sim_t*   sim = a -> sim;

  if ( ! p -> talents.deep_wounds -> ok() )
    return;

  struct deep_wounds_t : public warrior_attack_t
  {
    deep_wounds_t( warrior_t* p ) :
      warrior_attack_t( "deep_wounds", p, SCHOOL_BLEED, TREE_ARMS )
    {
      background = true;
      trigger_gcd = 0;
      weapon_multiplier = p -> talents.deep_wounds -> rank() * 0.16;
      may_crit = false;
      base_tick_time = 1.0;
      num_ticks = 6;
      tick_may_crit = false;
      hasted_ticks  = false;
      dot_behavior  = DOT_REFRESH;
      reset(); // required since construction occurs after player_t::init()

      id = 12834;
    }

    virtual void target_debuff( player_t* t, int dmg_type )
    {
      warrior_attack_t::target_debuff( t, dmg_type );

      // Deep Wounds doesn't benefit from Blood Frenzy or Savage Combat despite being a Bleed so disable it.
      if ( t -> debuffs.blood_frenzy_bleed  -> check() ||
           t -> debuffs.savage_combat       -> check() )
      {
        target_multiplier /= 1.04;
      }
    }

    virtual double total_td_multiplier() SC_CONST
    {
      return target_multiplier;
    }

    virtual void travel( player_t* t, int travel_result, double deep_wounds_dmg )
    {
      warrior_attack_t::travel( t, travel_result, 0 );
      if ( result_is_hit( travel_result ) )
      {
        base_td = deep_wounds_dmg / dot -> num_ticks;
        trigger_blood_frenzy( this );
      }
    }

    virtual double travel_time() 
    { 
      return sim -> gauss( sim -> aura_delay, 0.25 * sim -> aura_delay );
    }

    virtual void tick()
    {
      warrior_attack_t::tick();
      warrior_t* p = player -> cast_warrior();
      p -> buffs_tier10_2pc_melee -> trigger();
    }
  };

  if ( ! p -> active_deep_wounds ) p -> active_deep_wounds = new deep_wounds_t( p );

  if ( a -> weapon )
  {
    p -> active_deep_wounds -> weapon = a -> weapon;
  }
  else
  {
    p -> active_deep_wounds -> weapon = &( p -> main_hand_weapon );
  }

  // Normally, we cache the base damage and then combine them with the multipliers at the point of damage.
  // However, in this case we need to maintain remaining damage on refresh and the player-buff multipliers may change.
  // So we neeed to push the player-buff multipliers into the damage bank and then make sure we only use
  // the target-debuff multipliers at tick time.

  p -> active_deep_wounds -> player_buff();

  double deep_wounds_dmg = ( p -> active_deep_wounds -> calculate_weapon_damage() *
                             p -> active_deep_wounds -> weapon_multiplier *
                             p -> active_deep_wounds -> player_multiplier );

  dot_t* dot = p -> active_deep_wounds -> dot;
  
  if ( dot -> ticking )
  {
    deep_wounds_dmg += p -> active_deep_wounds -> base_td * dot -> ticks();
  }

  if ( p -> max_deep_wounds_refresh > 0 )
  {
    if( ( p -> max_deep_wounds_refresh + sim -> aura_delay ) < dot -> remains() )
    {
      if ( sim -> log ) log_t::output( sim, "Player %s munches Deep_Wounds due to Max Deep Wounds Duration.", p -> name() );
      p -> procs_munched_deep_wounds -> occur();
      return;
    }
  }

  if ( p -> active_deep_wounds -> travel_event ) 
  {
    // There is an SPELL_AURA_APPLIED already in the queue, which will get munched.
    if ( sim -> log ) log_t::output( sim, "Player %s munches previous Deep Wounds due to Aura Delay.", p -> name() );
    p -> procs_munched_deep_wounds -> occur();
  }
  
  p -> active_deep_wounds -> direct_dmg = deep_wounds_dmg;
  p -> active_deep_wounds -> schedule_travel( a -> target );

  if ( p -> active_deep_wounds -> travel_event && dot -> ticking ) 
  {
    if ( dot -> tick_event -> occurs() < p -> active_deep_wounds -> travel_event -> occurs() )
    {
      // Deep_Wounds will tick before SPELL_AURA_APPLIED occurs, which means that the current Deep_Wounds will
      // both tick -and- get rolled into the next Deep_Wounds.
      if ( sim -> log ) log_t::output( sim, "Player %s rolls Deep_Wounds.", p -> name() );
      p -> procs_rolled_deep_wounds -> occur();
    }
  }
}

// trigger_rage_gain ========================================================

static void trigger_rage_gain( attack_t* a )
{
  // Since 4.0.1 rage gain is 6.5 * weaponspeed and half that for off-hand

  if ( a -> proc )
    return;

  warrior_t* p = a -> player -> cast_warrior();
  weapon_t*  w = a -> weapon;

  double rage_gain = 6.5 * w -> swing_time;

  if ( w -> slot == SLOT_OFF_HAND ) rage_gain /= 2.0;
  
  rage_gain *= 1.0 + p -> spec.anger_management -> effect_base_value( 2 ) / 100.0;

  p -> resource_gain( RESOURCE_RAGE, rage_gain, w -> slot == SLOT_OFF_HAND ? p -> gains_melee_off_hand : p -> gains_melee_main_hand );
}

// trigger_strikes_of_opportunity ===========================================

static void trigger_strikes_of_opportunity( attack_t* a )
{
  if ( a -> proc )
    return;

  warrior_t* p = a -> player -> cast_warrior();

  if ( ! p -> mastery.strikes_of_opportunity -> ok() )
    return;

  if ( p -> cooldowns_strikes_of_opportunity -> remains() > 0 ) return;

  double chance = p -> composite_mastery() * p -> mastery.strikes_of_opportunity -> effect_base_value( 2 ) / 10000.0;

  if ( ! p -> rng_strikes_of_opportunity -> roll( chance ) )
    return;

  p -> cooldowns_strikes_of_opportunity -> start( 0.5 );

  if ( p -> sim -> debug )
    log_t::output( p -> sim, "Opportunity Strike procced from %s", a -> name() );

  if ( ! p -> active_opportunity_strike )
  {
    struct opportunity_strike_t : public warrior_attack_t
    {
      opportunity_strike_t( warrior_t* p ) :
          warrior_attack_t( "opportunity_strike", 76858, p, TREE_ARMS )
      {
        background = true;
        reset();
      }
    };
    p -> active_opportunity_strike = new opportunity_strike_t( p );
  }

  p -> procs_strikes_of_opportunity -> occur();
  p -> active_opportunity_strike -> execute();
}

// trigger_sudden_death =====================================================

static void trigger_sudden_death( action_t* a )
{
  warrior_t* p = a -> player -> cast_warrior();

  if ( a -> proc )
    return;

  if ( p -> rng_sudden_death -> roll ( p -> talents.sudden_death -> proc_chance() ) )
  {
    p -> cooldowns_colossus_smash -> reset();
    p -> procs_sudden_death       -> occur();
  }
}

// trigger_sword_and_board ==================================================

static void trigger_sword_and_board( attack_t* a )
{
  warrior_t* p = a -> player -> cast_warrior();

  if ( p -> buffs_sword_and_board -> trigger() )
  {
    p -> cooldowns_shield_slam -> reset();
  }
}

// trigger_enrage ===========================================================

static void trigger_enrage( attack_t* a )
{
  warrior_t* p = a -> player -> cast_warrior();

  if ( ! p -> talents.enrage -> ok() )
    return;

  // Can't proc while DW is active as of 403
  if ( p -> buffs_death_wish -> check() )
    return;

  double enrage_value = p -> buffs_enrage -> effect_base_value( 1 ) / 100.0;

  if ( p -> mastery.unshackled_fury -> ok() )
  {
    enrage_value *= 1.0 + p -> composite_mastery() * p -> mastery.unshackled_fury -> effect_base_value( 3 ) / 10000.0;
  }

  p -> buffs_enrage -> trigger( 1, enrage_value);
}

// trigger_flurry ===========================================================

static void trigger_flurry( attack_t* a, int stacks )
{
  warrior_t* p = a -> player -> cast_warrior();

  bool up_before = p -> buffs_flurry -> check() != 0;

  if ( stacks >= 0 )
    p -> buffs_flurry -> trigger( stacks );
  else
    p -> buffs_flurry -> decrement();

  if ( ! p -> instant_flurry_haste )
    return;

  // Flurry needs to haste the in-progress attacks, and flurry dropping
  // needs to de-haste them.

  bool up_after = p -> buffs_flurry -> check() != 0;

  if ( up_before == up_after )
    return;

  sim_t *sim = p -> sim;

  // Default mult is the up -> down case
  double mult = 1 + util_t::talent_rank( p -> talents.flurry -> rank(), 3, 0.08, 0.16, 0.25 );

  // down -> up case
  if ( ! up_before && up_after )
    mult = 1 / mult;

  // This mess would be a lot easier if we could give a time instead of
  // a delta to reschedule_execute().
  if( p -> main_hand_attack )
  {
    event_t* mhe = p -> main_hand_attack -> execute_event;
    if ( mhe ) {
      double delta;
      if ( mhe -> reschedule_time )
        delta = ( mhe -> reschedule_time - sim -> current_time ) * mult;
      else 
        delta = ( mhe -> time - sim -> current_time ) * mult;
      p -> main_hand_attack -> reschedule_execute( delta );
    }
  }
  if( p -> off_hand_attack )
  {
    event_t* ohe = p -> off_hand_attack -> execute_event;
    if ( ohe ) {
      double delta;
      if ( ohe -> reschedule_time )
        delta = ( ohe -> reschedule_time - sim -> current_time ) * mult;
      else 
        delta = ( ohe -> time - sim -> current_time ) * mult;
      p -> off_hand_attack -> reschedule_execute( delta );
    }
  }
}

// ==========================================================================
// Warrior Attacks
// ==========================================================================

double warrior_attack_t::armor() SC_CONST
{
  warrior_t* p = player -> cast_warrior();

  double a = attack_t::armor();

  if ( p -> buffs_colossus_smash -> up() )
  {
    a *= 1.0 - p -> buffs_colossus_smash -> value();
  }

  return a;
}

// warrior_attack_t::assess_damage ==========================================

void warrior_attack_t::assess_damage( player_t* t, double amount, int dmg_type, int travel_result )
{
  attack_t::assess_damage( t, amount, dmg_type, travel_result );

  warrior_t* p = player -> cast_warrior();

  if ( t -> is_enemy() )
  {
    target_t* q =  t -> cast_target();

    if ( p -> buffs_sweeping_strikes -> up() && q -> adds_nearby )
    {
      attack_t::additional_damage( q, amount, dmg_type, travel_result );
    }
  }
}

// warrior_attack_t::cost ===================================================

double warrior_attack_t::cost() SC_CONST
{
  warrior_t* p = player -> cast_warrior();
  double c = attack_t::cost() / 10; // Rage Costs are stored as * 10
  if ( p -> buffs_deadly_calm -> up()                 ) c  = 0;
  if ( p -> buffs_battle_trance -> check() && c > 5   ) c  = 0;
  return c;
}

// warrior_attack_t::consume_resource =======================================

void warrior_attack_t::consume_resource()
{
  warrior_t* p = player -> cast_warrior();

  attack_t::consume_resource();

  if ( proc ) 
    return;

  if ( result == RESULT_CRIT )
  {
    // Triggered here so it's applied between melee hits and next schedule.
    trigger_flurry( this, 3 );
  }

  // Warrior attacks (non-AoE) which are are avoided by the target consume only 20%

  if ( resource_consumed > 0 && ! aoe && result_is_miss() )
  {
    double rage_restored = resource_consumed * 0.80;
    p -> resource_gain( RESOURCE_RAGE, rage_restored, p -> gains_avoided_attacks );
  }
}

// warrior_attack_t::execute ================================================

void warrior_attack_t::execute()
{
  attack_t::execute();

  warrior_t* p = player -> cast_warrior();

  // Battle Trance only is effective+consumed if action cost was >5
  if ( attack_t::cost() > 50 && p -> buffs_battle_trance -> up() )
    p -> buffs_battle_trance -> expire();

  if ( proc ) return;

  if ( result_is_hit() )
  {
    trigger_sudden_death( this );

    trigger_strikes_of_opportunity( this );

    trigger_enrage( this );

    if ( result == RESULT_CRIT )
    {
      trigger_deep_wounds( this );
    }
  }
  else if ( result == RESULT_DODGE  )
  {
    p -> buffs_overpower -> trigger();
  }
}

// warrior_attack_t::parse_options ==========================================

void warrior_attack_t::parse_options( option_t* options, const std::string& options_str )
{
  option_t base_options[] =
  {
    { NULL, OPT_UNKNOWN, NULL }
  };
  std::vector<option_t> merged_options;
  attack_t::parse_options( merge_options( merged_options, base_options, options ), options_str );
}

// warrior_attack_t::calculate_weapon_damage ================================

double warrior_attack_t::calculate_weapon_damage()
{
  double dmg = attack_t::calculate_weapon_damage();

  // Catch the case where weapon == 0 so we don't crash/retest below.
  if ( dmg == 0 ) return 0;

  warrior_t* p = player -> cast_warrior();

  if ( weapon -> slot == SLOT_OFF_HAND )
  {
    dmg *= 1.0 + p -> spec.dual_wield_specialization -> effect_base_value( 2 ) / 100.0;
  }

  return dmg;
}

// warrior_attack_t::player_buff ============================================

void warrior_attack_t::player_buff()
{
  attack_t::player_buff();

  // FIXME: much of this can be moved to base for efficiency, but we need
  // to be careful to get the add_percent_mod effect ordering in the
  // abilities right.

  warrior_t* p = player -> cast_warrior();

  // --- Stances ---

  if ( p -> active_stance == STANCE_BATTLE && p -> buffs_battle_stance -> up() )
  {
    player_multiplier *= ( 1.0 + p -> buffs_battle_stance -> effect_base_value( 1 ) / 100.0 );
  }
  else if ( p -> active_stance == STANCE_BERSERKER && p -> buffs_berserker_stance -> up() )
  {
    player_multiplier *= ( 1.0 + p -> buffs_berserker_stance -> effect_base_value( 1 ) / 100.0 );
  }
  else if ( p -> active_stance == STANCE_DEFENSE && p -> buffs_defensive_stance -> up() )
  {
    // No penalty for defensive stance at the moment.
  }
  
  // --- Specializations --

  if ( weapon && weapon -> group() == WEAPON_2H )
    player_multiplier *= 1.0 + p -> spec.two_handed_weapon_specialization -> effect_base_value( 1 ) / 100.0;

  if ( p -> dual_wield() && school == SCHOOL_PHYSICAL )
    player_multiplier *= 1.0 + p -> spec.dual_wield_specialization -> effect_base_value( 3 ) / 100.0;

  // --- Enrages ---

  player_multiplier *= 1.0 + p -> buffs_wrecking_crew -> value();

  if ( school == SCHOOL_PHYSICAL )
    player_multiplier *= 1.0 + p -> buffs_enrage -> value();
  
  if ( school == SCHOOL_PHYSICAL && p -> buffs_bastion_of_defense -> up() )
    player_multiplier *= 1.0 + p -> talents.bastion_of_defense -> rank() * 0.05;

  // --- Passive Talents ---

  player_multiplier *= 1.0 + p -> buffs_death_wish -> value();

  if ( p -> talents.single_minded_fury -> ok() && p -> dual_wield() )
  {
    // FIXME: seems like you should need to have two 1Hers (&&)
    if ( p -> main_hand_attack -> weapon -> group() == WEAPON_1H ||
         p ->  off_hand_attack -> weapon -> group() == WEAPON_1H )
    {
      player_multiplier *= 1.0 + p -> talents.single_minded_fury -> effect_base_value( 1 ) / 100.0;
    }
  }

  if ( p -> talents.rampage -> ok() )
    player_crit += p -> talents.rampage -> effect_base_value( 2 ) / 100.0;

  // --- Buffs / Procs ---

  if ( p -> buffs_rude_interruption -> up() )
    player_multiplier *= 1.05;

  if ( special && p -> buffs_recklessness -> up() )
    player_crit += 0.5;

  if ( p -> buffs_hold_the_line -> up() )
    player_crit += 0.10;

  // --- Set Bonuses ---

  if ( p -> set_bonus.tier10_4pc_melee() )
    player_multiplier *= 1.05;

  if ( school == SCHOOL_PHYSICAL && p -> set_bonus.tier10_2pc_melee() )
    player_multiplier *= 1.05;

  if ( sim -> debug )
    log_t::output( sim, "warrior_attack_t::player_buff: %s hit=%.2f expertise=%.2f crit=%.2f crit_multiplier=%.2f",
                   name(), player_hit, player_expertise, player_crit, player_crit_multiplier );
}

// warrior_attack_t::ready() ================================================

bool warrior_attack_t::ready()
{
  if ( ! attack_t::ready() )
    return false;

  warrior_t* p = player -> cast_warrior();

  // Attack vailable in current stance?
  if ( ( stancemask & p -> active_stance ) == 0 )
    return false;

  return true;
}

// Melee Attack =============================================================

struct melee_t : public warrior_attack_t
{
  int sync_weapons;

  melee_t( const char* name, warrior_t* p, int sw ) :
    warrior_attack_t( name, p, SCHOOL_PHYSICAL, TREE_NONE, false ), sync_weapons( sw )
  {
    may_glance      = true;
    background      = true;
    repeating       = true;
    trigger_gcd     = 0;
    base_cost       = 0;

    if ( p -> dual_wield() ) base_hit -= 0.19;
  }

  virtual double swing_haste() SC_CONST
  {
    warrior_t* p = player -> cast_warrior();
    double h = warrior_attack_t::swing_haste();

    if ( p -> buffs_flurry -> up() )
    {
      h *= 1.0 / ( 1.0 + p -> buffs_flurry -> base_value() / 100.0 );
    }
    if ( p -> buffs_executioner_talent -> up() )
    {
      h *= 1.0 / ( 1.0 + p -> buffs_executioner_talent -> stack() * p -> buffs_executioner_talent -> effect_base_value( 1 ) / 100.0 );
    }

    // FIXME: does unholy_frenzy benefit from mastery.unshackled_fury?

    return h;
  }

  virtual double execute_time() SC_CONST
  {
    double t = warrior_attack_t::execute_time();

    if ( player -> in_combat )
      return t;

    if ( weapon -> slot == SLOT_MAIN_HAND || sync_weapons )
      return 0.02;

    // Before combat begins, unless we are under sync_weapons the OH is
    // delayed by half its swing time.

    return 0.02 + t / 2;

  }

  virtual void execute()
  {
    warrior_t* p = player -> cast_warrior();

    // Be careful changing where this is done.  Flurry that procs from melee
    // must be applied before the (repeating) event schedule, and the decrement
    // here must be done before it.
    trigger_flurry( this, -1 );

    warrior_attack_t::execute();

    if ( result != RESULT_MISS ) // Any attack that hits or is dodged/blocked/parried generates rage
      trigger_rage_gain( this );

    if ( result_is_hit() )
    {      
      if ( ! proc &&  p -> rng_blood_frenzy -> roll( p -> talents.blood_frenzy -> proc_chance() ) )
      {
        p -> resource_gain( RESOURCE_RAGE, p -> talents.blood_frenzy -> effect_base_value( 3 ), p -> gains_blood_frenzy );
      }
    }
  }
};

// Auto Attack ==============================================================

struct auto_attack_t : public warrior_attack_t
{
  int sync_weapons;

  auto_attack_t( warrior_t* p, const std::string& options_str ) :
      warrior_attack_t( "auto_attack", p ), sync_weapons( 0 )
  {
    option_t options[] =
    {
      { "sync_weapons", OPT_BOOL, &sync_weapons },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    assert( p -> main_hand_weapon.type != WEAPON_NONE );

    p -> main_hand_attack = new melee_t( "melee_main_hand", p, sync_weapons );
    p -> main_hand_attack -> weapon = &( p -> main_hand_weapon );
    p -> main_hand_attack -> base_execute_time = p -> main_hand_weapon.swing_time;

    if ( p -> off_hand_weapon.type != WEAPON_NONE )
    {
      p -> off_hand_attack = new melee_t( "melee_off_hand", p, sync_weapons );
      p -> off_hand_attack -> weapon = &( p -> off_hand_weapon );
      p -> off_hand_attack -> base_execute_time = p -> off_hand_weapon.swing_time;
    }

    trigger_gcd = 0;
  }

  virtual void execute()
  {
    warrior_t* p = player -> cast_warrior();
    p -> main_hand_attack -> schedule_execute();
    if ( p -> off_hand_attack )
    {
      p -> off_hand_attack -> schedule_execute();
    }
  }

  virtual bool ready()
  {
    warrior_t* p = player -> cast_warrior();
    if ( p -> buffs.moving -> check() ) return false;
    return( p -> main_hand_attack -> execute_event == 0 ); // not swinging
  }
};

// Bladestorm ===============================================================

struct bladestorm_tick_t : public warrior_attack_t
{
  bladestorm_tick_t( warrior_t* p, const char* name ) :
      warrior_attack_t( name, 50622, p, TREE_ARMS, false )
  {
    dual        = true;
    background  = true;
    aoe         = -1;
    direct_tick = true;
    aoe         = 4;
  }
};

struct bladestorm_t : public warrior_attack_t
{
  attack_t* bladestorm_mh;
  attack_t* bladestorm_oh;

  bladestorm_t( warrior_t* p, const std::string& options_str ) :
    warrior_attack_t( "bladestorm", p, SCHOOL_PHYSICAL, TREE_ARMS ),
    bladestorm_mh(0), bladestorm_oh(0)
  {
    check_talent( p -> talents.bladestorm -> rank() );

    parse_options( NULL, options_str );

    id = 46924;
    parse_data( p -> player_data );

    aoe       = -1;
    harmful   = false;
    channeled = true;
    tick_zero = true;

    weapon_multiplier = base_dd_min = base_dd_max = 0;
    may_miss = may_dodge = may_parry = false;

    cooldown -> duration += p -> glyphs.bladestorm -> base_value() / 1000.0;

    bladestorm_mh = new bladestorm_tick_t( p, "bladestorm_mh" );
    bladestorm_mh -> weapon = &( player -> main_hand_weapon );
    add_child( bladestorm_mh );

    if ( player -> off_hand_weapon.type != WEAPON_NONE )
    {
      bladestorm_oh = new bladestorm_tick_t( p, "bladestorm_oh" );
      bladestorm_oh -> weapon = &( player -> off_hand_weapon );
      add_child( bladestorm_oh );
    }
  }
  
  virtual void execute()
  {
    warrior_attack_t::execute();
    warrior_t* p = player -> cast_warrior();
    if ( p -> main_hand_attack ) p -> main_hand_attack -> cancel();
    if ( p ->  off_hand_attack ) p -> off_hand_attack -> cancel();
  }

  virtual void tick()
  {
    warrior_attack_t::tick();
    bladestorm_mh -> execute();
    if ( bladestorm_mh -> result_is_hit() && bladestorm_oh )
    {
      bladestorm_oh -> execute();
    }
  }

  // Bladestorm not modified by haste effects
  virtual double haste() SC_CONST { return 1.0; }
};

// Bloodthirst ==============================================================

struct bloodthirst_t : public warrior_attack_t
{
  bloodthirst_t( warrior_t* p, const std::string& options_str ) :
      warrior_attack_t( "bloodthirst", "Bloodthirst", p )
  {
    check_spec( TREE_FURY );

    parse_options( NULL, options_str );

    // Include the weapon so we benefit from racials
    weapon             = &( player -> main_hand_weapon );
    weapon_multiplier  = 0;

    direct_power_mod   = effect_min( 1 ) / 100.0;
    base_dd_min        = 0.0;
    base_dd_max        = 0.0;
    base_multiplier   *= 1.0 + p -> glyphs.bloodthirst -> effect_base_value( 1 ) / 100.0
                             + p -> set_bonus.tier11_2pc_melee() * 0.05;
    base_crit         += p -> talents.cruelty -> effect_base_value ( 1 ) / 100.0;
  }

  virtual void execute()
  {
    warrior_attack_t::execute();
    if ( result_is_hit() )
    {
      warrior_t* p = player -> cast_warrior();
      p -> buffs_battle_trance -> trigger();
      trigger_bloodsurge( this );
    }
  }
};

// Cleave ===================================================================

struct cleave_t : public warrior_attack_t
{
  cleave_t( warrior_t* p, const std::string& options_str ) :
      warrior_attack_t( "cleave", p, SCHOOL_PHYSICAL, TREE_FURY )
  {
    parse_options( NULL, options_str );

    id = 845;
    parse_data( p -> player_data );

    direct_power_mod = 0.4496;
    base_dd_min      = 6;
    base_dd_max      = 6;
    aoe              = -1;

    base_multiplier *= 1.0 + p -> talents.thunderstruck -> effect_base_value( 1 ) / 100.0;

    aoe = 1 +  p -> glyphs.cleaving -> effect_base_value( 1 );
  }

  virtual void execute()
  {
    warrior_attack_t::execute();
    warrior_t* p = player -> cast_warrior();  
    if ( result_is_hit() ) 
      p -> buffs_meat_cleaver -> trigger();
  }

  virtual void player_buff()
  {
    warrior_attack_t::player_buff();
    warrior_t* p = player -> cast_warrior();
    if ( p -> buffs_meat_cleaver -> up() )
    {
      player_multiplier *= 1.0 + p -> talents.meat_cleaver -> rank() * 0.05 * p -> buffs_meat_cleaver -> stack();
    }
  }

  virtual void update_ready()
  {
    warrior_t* p = player -> cast_warrior();
    cooldown -> duration = ( p -> buffs_inner_rage -> check() ? 1.5 : 3.0 );
    warrior_attack_t::update_ready();
  }
};

// Colossus Smash ===========================================================

struct colossus_smash_t : public warrior_attack_t
{
  colossus_smash_t( warrior_t* p, const std::string& options_str ) :
      warrior_attack_t( "colossus_smash",  p, SCHOOL_PHYSICAL, TREE_ARMS )
  {
    check_min_level( 81 );

    parse_options( NULL, options_str );

    id = 86346;
    parse_data( p -> player_data );

    base_dd_min = base_dd_max = p -> player_data.effect_base_value( 87876 );

    stancemask  = STANCE_BERSERKER | STANCE_BATTLE;
  }

  // Does CS benefit from its own effect?
  // virtual double armor() SC_CONST { return 0.0; }

  virtual void execute()
  {
    warrior_attack_t::execute();
    warrior_t* p = player -> cast_warrior();
    if ( result_is_hit() )
      p -> buffs_colossus_smash -> trigger( 1, 1.0 );
  }
};

// Concussion Blow ==========================================================

struct concussion_blow_t : public warrior_attack_t
{
  concussion_blow_t( warrior_t* p, const std::string& options_str ) :
      warrior_attack_t( "concussion_blow", p, SCHOOL_PHYSICAL, TREE_PROTECTION )
  {
    check_talent( p -> talents.concussion_blow -> rank() );

    parse_options( NULL, options_str );

    id = 12809;
    parse_data( p -> player_data );

    direct_power_mod  = effect_base_value( 3 ) / 100.0;
  }
};

// Devastate ================================================================

struct devastate_t : public warrior_attack_t
{
  devastate_t( warrior_t* p, const std::string& options_str ) :
      warrior_attack_t( "devastate", p, SCHOOL_PHYSICAL, TREE_PROTECTION )
  {
    check_talent( p -> talents.devastate -> rank() );

    parse_options( NULL, options_str );

    id = 20243;
    parse_data( p -> player_data );

    base_multiplier *= 1.0 + p -> talents.war_academy -> effect_base_value( 1 ) / 100.0;

    base_crit += p -> talents.sword_and_board -> effect_base_value( 2 ) / 100.0;
    base_crit += p -> glyphs.devastate -> effect_base_value( 1 ) / 100.0;
  }

  virtual void execute()
  {
    warrior_attack_t::execute();
    warrior_t* p = player -> cast_warrior();

    if ( result_is_hit() ) trigger_sword_and_board( this );

    if ( target -> health_percentage() <= 20 )
    {
      if ( p -> rng_impending_victory -> roll( p -> talents.impending_victory -> proc_chance() ) )
        p -> buffs_victory_rush -> trigger();
    }      
  }
};

// Execute ==================================================================

struct execute_t : public warrior_attack_t
{
  execute_t( warrior_t* p, const std::string& options_str ) :
      warrior_attack_t( "execute", p, SCHOOL_PHYSICAL, TREE_FURY )
  {
    parse_options( NULL, options_str );

    id = 5308;
    parse_data( p -> player_data );

    // Include the weapon so we benefit from racials
    weapon             = &( player -> main_hand_weapon );
    weapon_multiplier  = 0;
    base_dd_min        = 10;
    base_dd_max        = 10;
    
    // Rage scaling is handled in player_buff()

    stancemask = STANCE_BATTLE | STANCE_BERSERKER;
  }

  virtual void consume_resource()
  {
    warrior_t* p = player -> cast_warrior();

    // Consumes base_cost + 20
    resource_consumed = std::min( p -> resource_current[ RESOURCE_RAGE ], 20.0 + cost() );
    
    if ( sim -> debug )
      log_t::output( sim, "%s consumes %.1f %s for %s", p -> name(),
                     resource_consumed, util_t::resource_type_string( resource ), name() );

    player -> resource_loss( resource, resource_consumed );

    stats -> consume_resource( resource_consumed );

    if ( p -> talents.sudden_death -> ok() )
    {
      double current_rage = p -> resource_current[ RESOURCE_RAGE ];
      double sudden_death_rage = p -> talents.sudden_death -> effect_base_value( 1 );

      if ( current_rage < sudden_death_rage )
      {
        p -> resource_gain( RESOURCE_RAGE, sudden_death_rage - current_rage, p -> gains_sudden_death ); // FIXME: Do we keep up to 10 or always at least 10?
      }
    }
  }

  virtual void execute()
  {
    warrior_attack_t::execute();
    warrior_t* p = player -> cast_warrior();
    if ( result_is_hit() && p -> rng_executioner_talent -> roll( p -> talents.executioner -> proc_chance() ) )
    {
      p -> buffs_executioner_talent -> trigger();
    }
  }

  virtual void player_buff()
  {
    warrior_attack_t::player_buff();
    warrior_t* p = player -> cast_warrior();

    // player_buff happens before consume_resource
    // so we can safely check here how much excess rage we will spend
    double base_consumed = cost();
    double max_consumed = std::min( p -> resource_current[ RESOURCE_RAGE ], 20.0 + base_consumed );
    
    // Damage scales directly with AP per rage since 4.0.1.
    // Can't be derived by parse_data() for now.
    direct_power_mod = 0.0437 * max_consumed;

    if ( p -> buffs_lambs_to_the_slaughter -> up() )
    {
      int stack = p -> buffs_lambs_to_the_slaughter -> stack();
      player_multiplier *= 1.0 + stack * 0.10;
    }
  }

  virtual bool ready()
  {
    if ( ! warrior_attack_t::ready() )
      return false;

    if ( target -> health_percentage() > 20 )
      return false;

    return true;
  }
};

// Heroic Strike ============================================================

struct heroic_strike_t : public warrior_attack_t
{
  heroic_strike_t( warrior_t* p, const std::string& options_str ) :
      warrior_attack_t( "heroic_strike", p, SCHOOL_PHYSICAL, TREE_ARMS )
  {
    parse_options( NULL, options_str );

    id = 78;
    parse_data( p -> player_data );
    // Include the weapon so we benefit from racials
    weapon = &( player -> main_hand_weapon );
    weapon_multiplier = 0;

    base_crit        += p -> talents.incite -> effect_base_value( 1 ) / 100.0;
    base_dd_min       = 8;
    base_dd_max       = 8;
    direct_power_mod  = 0.6;
  }

  virtual void execute()
  {
    warrior_t* p = player -> cast_warrior();
    warrior_attack_t::execute();    

    if ( p -> buffs_incite -> check() )
      p -> buffs_incite -> expire();
    else if ( result == RESULT_CRIT )
      p -> buffs_incite -> trigger();

  }
  
  virtual void player_buff()
  {
    warrior_attack_t::player_buff();
    warrior_t* p = player -> cast_warrior();
    if ( p -> buffs_incite -> up() )
      player_crit += 1.0;
  }

  virtual void update_ready()
  {
    warrior_t* p = player -> cast_warrior();
    cooldown -> duration = ( p -> buffs_inner_rage -> check() ? 1.5 : 3.0 );
    warrior_attack_t::update_ready();
  }
};

// Mortal Strike ============================================================

struct mortal_strike_t : public warrior_attack_t
{
  mortal_strike_t( warrior_t* p, const std::string& options_str ) :
      warrior_attack_t( "mortal_strike", p, SCHOOL_PHYSICAL, TREE_ARMS )
  {
    check_spec( TREE_ARMS );

    parse_options( NULL, options_str );

    id = 12294;
    parse_data( p -> player_data );
   
    base_multiplier *= 1.0 + p -> glyphs.mortal_strike -> effect_base_value( 1 ) / 100.0
                           + p -> set_bonus.tier11_2pc_melee() * 0.05
                           + p -> talents.war_academy -> effect_base_value( 1 ) / 100.0;
    base_crit_bonus_multiplier *= 1.0 + p -> talents.impale -> effect_base_value( 1 ) / 100.0;
    base_crit                  += p -> talents.cruelty -> effect_base_value ( 1 ) / 100.0;
  }

  virtual void execute()
  {
    warrior_attack_t::execute();
    warrior_t* p = player -> cast_warrior();
    if ( result_is_hit() )
    {
      p -> buffs_lambs_to_the_slaughter -> trigger();
      p -> buffs_battle_trance -> trigger();
      if ( result == RESULT_CRIT && p -> rng_wrecking_crew -> roll( p -> talents.wrecking_crew -> proc_chance() ) )
      {
        double value = p -> talents.wrecking_crew -> rank() * 0.05;
        p -> buffs_wrecking_crew -> trigger( 1, value );
      }
    }
  }

  virtual void player_buff()
  {
    warrior_attack_t::player_buff();
    warrior_t* p = player -> cast_warrior();
    if ( p -> buffs_lambs_to_the_slaughter -> up() )
    {
      int stack = p -> buffs_lambs_to_the_slaughter -> stack();
      player_multiplier *= 1.0 + stack * 0.10;
    }
  }
};

// Overpower ================================================================

struct overpower_t : public warrior_attack_t
{
  overpower_t( warrior_t* p, const std::string& options_str ) :
      warrior_attack_t( "overpower", p, SCHOOL_PHYSICAL, TREE_ARMS )
  {
    parse_options( NULL, options_str );

    id = 7384;
    parse_data( p -> player_data );

    may_dodge  = false;
    may_parry  = false;
    may_block  = false; // The Overpower cannot be blocked, dodged or parried.

    base_crit += p -> talents.taste_for_blood -> effect_base_value( 2 ) / 100.0;
    base_crit_bonus_multiplier *= 1.0 + p -> talents.impale -> effect_base_value( 1 ) / 100.0;
    base_multiplier            *= 1.0 + p -> glyphs.overpower -> effect_base_value( 1 ) / 100.0;

    stancemask = STANCE_BATTLE;    
  }

  virtual void execute()
  {
    warrior_t* p = player -> cast_warrior();
    // Track some information on what got us the overpower
    // Talents or lack of expertise
    p -> buffs_overpower -> up();
    p -> buffs_taste_for_blood -> up();
    warrior_attack_t::execute();
    p -> buffs_overpower -> expire();
    p -> buffs_taste_for_blood -> expire();

    // FIXME: The wording indicates it triggers even if you miss.
    p -> buffs_tier11_4pc_melee -> trigger();
  }

  virtual void player_buff()
  {
    warrior_attack_t::player_buff();
    warrior_t* p = player -> cast_warrior();
    if ( p -> buffs_lambs_to_the_slaughter -> up() )
    {
      int stack = p -> buffs_lambs_to_the_slaughter -> stack();
      player_multiplier *= 1.0 + stack * 0.10;
    }
  }

  virtual bool ready()
  {
    if ( ! warrior_attack_t::ready() )
      return false;

    warrior_t* p = player -> cast_warrior();

    if ( p -> buffs_overpower -> check() || p -> buffs_taste_for_blood -> check() )
      return true;

    return false;
  }
};

// Pummel ===================================================================

struct pummel_t : public warrior_attack_t
{
  pummel_t( warrior_t* p, const std::string& options_str ) :
      warrior_attack_t( "pummel", p, SCHOOL_PHYSICAL, TREE_FURY )
  {
    parse_options( NULL, options_str );

    id = 6552;
    parse_data( p -> player_data );

    base_cost *= 1.0 + p -> talents.drums_of_war -> effect_base_value( 1 ) / 100.0;

    may_miss = may_resist = may_glance = may_block = may_dodge = may_crit = false;
  }

  virtual bool ready()
  {
    if ( ! target -> debuffs.casting -> check() ) return false;
    return warrior_attack_t::ready();
  }
};

// Raging Blow ==============================================================

struct raging_blow_attack_t : public warrior_attack_t
{
  raging_blow_attack_t( warrior_t* p, const char* name ) :
      warrior_attack_t( name, p, SCHOOL_PHYSICAL, TREE_FURY )
  {
    id = 96103;
    parse_data( p -> player_data );
    base_cost = 0;
    may_miss = may_dodge = may_parry = false;
    background = true;
    dual = true;
    base_multiplier *= 1.0 + p -> talents.war_academy -> effect_base_value( 1 ) / 100.0;
    base_crit += p -> glyphs.raging_blow -> effect_base_value( 1 ) / 100.0;
  }

  virtual void player_buff()
  {
    warrior_t* p = player -> cast_warrior();
    warrior_attack_t::player_buff();
    player_multiplier *= 1.0 + p -> composite_mastery() * p -> mastery.unshackled_fury -> effect_base_value( 3 ) / 10000.0;
  }
};

struct raging_blow_t : public warrior_attack_t
{
  raging_blow_attack_t* mh_attack;
  raging_blow_attack_t* oh_attack;

  raging_blow_t( warrior_t* p, const std::string& options_str ) :
    warrior_attack_t( "raging_blow", p, SCHOOL_PHYSICAL, TREE_FURY ), mh_attack(0), oh_attack(0)
  {
    check_talent( p -> talents.raging_blow -> rank() );

    id = 85288;
    parse_data( p -> player_data );

    // Parent attack is only to determine miss/dodge/parry
    base_dd_min = base_dd_max = 0;
    weapon_multiplier = direct_power_mod = 0; 
    may_crit = false;

    parse_options( NULL, options_str );

    stancemask = STANCE_BERSERKER;

    mh_attack = new raging_blow_attack_t( p, "raging_blow_mh" );
    mh_attack -> weapon = &( p -> main_hand_weapon );
    add_child( mh_attack );

    if ( p -> off_hand_weapon.type != WEAPON_NONE )
    {
      oh_attack = new raging_blow_attack_t( p, "raging_blow_oh" );
      oh_attack -> weapon = &( p -> off_hand_weapon );
      add_child( oh_attack );
    }
  }

  virtual void execute()
  {
    warrior_t* p = player -> cast_warrior();
    attack_t::execute();
    if ( result_is_hit() ) 
    {
      mh_attack -> execute();
      if ( oh_attack ) 
      {
        oh_attack -> execute();
      }
    }
    p -> buffs_tier11_4pc_melee -> trigger();
  }

  virtual bool ready()
  {
    warrior_t* p = player -> cast_warrior();
    if ( ! ( p -> buffs_berserker_rage -> check() || 
             p -> buffs_death_wish     -> check() || 
             p -> buffs_enrage         -> check() ||
             p -> buffs.unholy_frenzy  -> check() ) )
      return false;
    return warrior_attack_t::ready();
  }
};

// Rend =====================================================================

struct rend_t : public warrior_attack_t
{
  rend_t( warrior_t* p, const std::string& options_str ) :
      warrior_attack_t( "rend", p, SCHOOL_BLEED, TREE_ARMS )
  {
    parse_options( NULL, options_str );

    id = 772;
    parse_data( p -> player_data );

    weapon                 = &( p -> main_hand_weapon );
    may_crit               = false;
    base_td_init           = p -> player_data.effect_min( 98699, p -> type, p -> level );
    base_tick_time         = p -> player_data.effect_period ( 98699 );
    num_ticks              = (int) ( p -> player_data.spell_duration ( 94009 ) / base_tick_time );
    tick_may_crit          = true;
    tick_zero              = true;
    normalize_weapon_speed = false;
    base_multiplier       *= 1.0 + p -> talents.thunderstruck -> effect_base_value( 1 ) / 100.0;
    stancemask             = STANCE_BATTLE | STANCE_DEFENSE;
    school                 = stats -> school = SCHOOL_BLEED;
  }

  virtual double calculate_direct_damage()
  {
    // Rend doesn't actually hit with the weapon, but ticks on application
    return 0.0;
  }

  virtual void execute()
  {
    base_td = base_td_init + calculate_weapon_damage() * 0.25;
    warrior_attack_t::execute();
    if ( result_is_hit() )
      trigger_blood_frenzy( this );
  }

  virtual void tick()
  {
    warrior_attack_t::tick();
    warrior_t* p = player -> cast_warrior();
    p -> buffs_tier10_2pc_melee -> trigger();
    p -> buffs_taste_for_blood -> trigger();
  }
};

// Revenge ==================================================================

struct revenge_t : public warrior_attack_t
{
  revenge_t( warrior_t* p, const std::string& options_str ) :
      warrior_attack_t( "revenge", p, SCHOOL_PHYSICAL, TREE_PROTECTION )
  {
    parse_options( NULL, options_str );

    id = 6572;
    parse_data( p -> player_data );

    direct_power_mod  = 0.31; // Assumption from 3.3.5
    base_multiplier  *= 1.0 + p -> talents.improved_revenge -> effect_base_value( 2 ) / 100.0
                            + p -> glyphs.revenge -> effect_base_value( 1 ) / 100.0;
    stancemask = STANCE_DEFENSE;

    if ( p -> talents.improved_revenge -> rank() )
    {
      aoe = 1;
      base_add_multiplier = p -> talents.improved_revenge -> rank() * 0.50;
    }
  }

  virtual void execute()
  {
    warrior_t* p = player -> cast_warrior();
    warrior_attack_t::execute();
    p -> buffs_revenge -> expire();
    if ( result_is_hit() ) trigger_sword_and_board( this );
  }

  virtual bool ready()
  {
    warrior_t* p = player -> cast_warrior();
    if ( ! p -> buffs_revenge -> check() ) return false;
    return warrior_attack_t::ready();
  }
};

// Shattering Throw ==============================================================

// TO-DO: Only a shell at the moment. Needs testing for damage etc.
struct shattering_throw_t : public warrior_attack_t
{
  shattering_throw_t( warrior_t* p, const std::string& options_str ) :
    warrior_attack_t( "shattering_throw", p, SCHOOL_PHYSICAL, TREE_PROTECTION )
  {
    parse_options( NULL, options_str );

    id = 64382;
    parse_data( p -> player_data );

    stancemask = STANCE_BATTLE;
  }

  virtual void execute()
  {
    warrior_attack_t::execute();
    if ( result_is_hit() )
      target -> debuffs.shattering_throw -> trigger();
  }

  virtual bool ready()
  {
    if ( target -> debuffs.shattering_throw -> check() ) return false;
    return warrior_attack_t::ready();
  }
};

// Shield Bash ==============================================================

struct shield_bash_t : public warrior_attack_t
{
  shield_bash_t( warrior_t* p, const std::string& options_str ) :
    warrior_attack_t( "shield_bash", p, SCHOOL_PHYSICAL, TREE_PROTECTION )
  {
    parse_options( NULL, options_str );

    id = 72;
    parse_data( p -> player_data );

    base_cost *= 1.0 + p -> talents.drums_of_war -> effect_base_value( 1 ) / 100.0;

    stancemask = STANCE_DEFENSE | STANCE_BATTLE;
  }

  virtual bool ready()
  {
    if ( ! target -> debuffs.casting -> check() ) return false;
    return warrior_attack_t::ready();
  }
};

// Shield Slam ==============================================================

struct shield_slam_t : public warrior_attack_t
{
  shield_slam_t( warrior_t* p, const std::string& options_str ) :
      warrior_attack_t( "shield_slam", p, SCHOOL_PHYSICAL, TREE_PROTECTION )
  {
    check_spec( TREE_PROTECTION );

    parse_options( NULL, options_str );

    id = 23922;
    parse_data( p -> player_data );

    direct_power_mod  = 0; // FIXME: What is this?
    base_crit        += p -> talents.cruelty -> effect_base_value ( 1 ) / 100.0;
    base_multiplier  *= 1.0  + p -> glyphs.shield_slam -> effect_base_value( 1 ) / 100.0;
  }

  virtual void player_buff()
  {
    warrior_attack_t::player_buff();

    warrior_t* p = player -> cast_warrior();
    if ( p -> buffs_shield_block -> up() )
    {
      player_multiplier *= 1.0 + p -> talents.heavy_repercussions -> effect_base_value( 1 ) / 100.0;
    }
  }

  virtual void execute()
  {
    warrior_t* p = player -> cast_warrior();
    warrior_attack_t::execute();
    p -> buffs_sword_and_board -> expire();
    if ( result_is_hit() )
      p -> buffs_battle_trance -> trigger();
  }

  virtual double cost() SC_CONST
  {
    warrior_t* p = player -> cast_warrior();
    if ( p -> buffs_sword_and_board -> up() ) return 0;
    double c = warrior_attack_t::cost();
    return c;
  }
};

// Shockwave ================================================================

struct shockwave_t : public warrior_attack_t
{
  shockwave_t( warrior_t* p, const std::string& options_str ) :
      warrior_attack_t( "shockwave", p, SCHOOL_PHYSICAL, TREE_PROTECTION )
  {
    check_talent( p -> talents.shockwave -> rank() );

    parse_options( NULL, options_str );

    id = 46968;
    parse_data( p -> player_data );

    direct_power_mod  = 0.75;
    may_dodge         = false;
    may_parry         = false;
    may_block         = false;
    cooldown -> duration += p -> glyphs.shockwave -> effect_base_value( 1 ) / 1000.0;
  }

  virtual void execute()
  {
    warrior_attack_t::execute();
    warrior_t* p = player -> cast_warrior();
    p -> buffs_thunderstruck -> expire();
  }

  virtual void player_buff()
  {
    warrior_t* p = player -> cast_warrior();
    warrior_attack_t::player_buff();    
    player_multiplier *= 1.0 + p -> buffs_thunderstruck -> stack() * p -> talents.thunderstruck -> effect_base_value( 2 ) / 100.0;
  }
};

// Slam =====================================================================

struct slam_attack_t : public warrior_attack_t
{
  slam_attack_t( warrior_t* p, const char* name ) :
    warrior_attack_t( name, p, SCHOOL_PHYSICAL, TREE_FURY )
  {
    id = 50783;
    parse_data( p -> player_data );

    base_cost = 0;
    may_miss = may_dodge = may_parry = false;
    background = true;
    dual = true;

    base_crit                  += p -> glyphs.slam -> effect_base_value( 1 ) / 100.0;
    base_crit_bonus_multiplier *= 1.0 + p -> talents.impale -> effect_base_value( 1 ) / 100.0;
    base_execute_time          += p -> talents.improved_slam -> effect_base_value( 1 ) / 1000.0;

    weapon_multiplier = 1.45;  // FIXME!  Should be right in DBC.
    base_multiplier *= 1.0 + ( p -> talents.improved_slam -> effect_base_value( 2 ) / 100.0 + 
                               p -> talents.war_academy   -> effect_base_value( 1 ) / 100.0 );
  }

  virtual void player_buff()
  {
    warrior_t* p = player -> cast_warrior();
    warrior_attack_t::player_buff();
    if ( p -> buffs_bloodsurge -> up() )
    {
      player_multiplier *= 1.0 + p -> talents.bloodsurge -> effect_base_value( 1 ) / 100.0;
    }
    if ( p -> buffs_lambs_to_the_slaughter -> up() )
    {
      player_multiplier *= 1.0 + p -> buffs_lambs_to_the_slaughter -> stack() * 0.10;
    }
  }
};

struct slam_t : public warrior_attack_t
{
  attack_t* mh_attack;
  attack_t* oh_attack;

  slam_t( warrior_t* p, const std::string& options_str ) :
    warrior_attack_t( "slam", p, SCHOOL_PHYSICAL, TREE_FURY ), mh_attack(0), oh_attack(0)
  {
    id = 1464;
    parse_data( p -> player_data );
    parse_options( NULL, options_str );
    may_crit = false;

    // Ensure we include racial expertise
    weapon = &( p -> main_hand_weapon );
    weapon_multiplier = 0;

    mh_attack = new slam_attack_t( p, "slam_mh" );
    mh_attack -> weapon = &( p -> main_hand_weapon );
    add_child( mh_attack );

    if ( p -> talents.single_minded_fury -> ok() && p -> off_hand_weapon.type != WEAPON_NONE )
    {
      oh_attack = new slam_attack_t( p, "slam_oh" );
      oh_attack -> weapon = &( p -> off_hand_weapon );
      add_child( oh_attack );
    }
  }

  virtual double haste() SC_CONST { return 1.0; }

  virtual double cost() SC_CONST
  {
    warrior_t* p = player -> cast_warrior();
    if ( p -> buffs_bloodsurge -> check() )
      return 0;
    return warrior_attack_t::cost();
  }

  virtual double execute_time() SC_CONST
  {
    warrior_t* p = player -> cast_warrior();
    if ( p -> buffs_bloodsurge -> check() )
      return 0.0;
    return warrior_attack_t::execute_time();
  }

  virtual void execute()
  {
    warrior_t* p = player -> cast_warrior();
    attack_t::execute();
    if ( result_is_hit() ) 
    {
      mh_attack -> execute();
      if ( oh_attack ) 
      {
        oh_attack -> execute();
      }
    }
    p -> buffs_bloodsurge -> decrement();
  }
};

// Thunder Clap =============================================================

struct thunder_clap_t : public warrior_attack_t
{
  thunder_clap_t( warrior_t* p, const std::string& options_str ) :
      warrior_attack_t( "thunder_clap", p, SCHOOL_PHYSICAL, TREE_PROTECTION )
  {
    parse_options( NULL, options_str );

    id = 6343;
    parse_data( p -> player_data );

    aoe               = -1;
    may_dodge         = false;
    may_parry         = false;
    may_block         = false;
    direct_power_mod  = 0.12; // FIXME: Is this correct?
    base_multiplier  *= 1.0 + p -> talents.thunderstruck -> effect_base_value( 1 ) / 100.0;
    base_crit        += p -> talents.incite -> effect_base_value( 1 ) / 100.0;
    base_cost        += p -> glyphs.resonating_power -> effect_base_value( 1 );
  }

  virtual void execute()
  {
    warrior_attack_t::execute();
    warrior_t* p = player -> cast_warrior();
    p -> buffs_thunderstruck -> trigger();
  }
};

// Whirlwind ================================================================

struct whirlwind_t : public warrior_attack_t
{
  whirlwind_t( warrior_t* p, const std::string& options_str ) :
      warrior_attack_t( "whirlwind", p, SCHOOL_PHYSICAL, TREE_FURY )
  {
    parse_options( NULL, options_str );

    id = 1680;
    parse_data( p -> player_data );

    aoe               = -1;
    stancemask        = STANCE_BERSERKER;
  }

  virtual void consume_resource() { }

  virtual void execute()
  {
    warrior_t* p = player -> cast_warrior();

    // MH hit
    weapon = &( p -> main_hand_weapon );
    warrior_attack_t::execute();

    if ( result_is_hit() )
      p -> buffs_meat_cleaver -> trigger();

    if ( p -> off_hand_weapon.type != WEAPON_NONE )
    {
      weapon = &( p -> off_hand_weapon );
      warrior_attack_t::execute();
      if ( result_is_hit() )
        p -> buffs_meat_cleaver -> trigger();
    }

    warrior_attack_t::consume_resource();
  }

  virtual void player_buff()
  {
    warrior_attack_t::player_buff();
    warrior_t* p = player -> cast_warrior();
    if ( p -> buffs_meat_cleaver -> up() )
    {
      // Caution: additive
      player_multiplier *= 1.0 + p -> talents.meat_cleaver -> rank() * 0.05 * p -> buffs_meat_cleaver -> stack();
    }
  }
};

// Victory Rush =============================================================

struct victory_rush_t : public warrior_attack_t
{
  victory_rush_t( warrior_t* p, const std::string& options_str ) :
      warrior_attack_t( "victory_rush", p, SCHOOL_PHYSICAL, TREE_FURY )
  {
    parse_options( NULL, options_str );

    id = 34428;
    parse_data( p -> player_data );

    base_multiplier *= 1.0 + p -> talents.war_academy -> effect_base_value( 1 ) / 100.0;
  }

  virtual bool ready()
  {
     warrior_t* p = player -> cast_warrior();
    if ( ! p -> buffs_victory_rush -> check() )
      return false;

    return warrior_attack_t::ready();
  }
};

// ==========================================================================
// Warrior Spells
// ==========================================================================

struct warrior_spell_t : public spell_t
{
  int stancemask;
  warrior_spell_t( const char* n, warrior_t* p, const school_type s=SCHOOL_PHYSICAL, int t=TREE_NONE ) :
      spell_t( n, p, RESOURCE_RAGE, s, t ),
      stancemask( STANCE_BATTLE|STANCE_BERSERKER|STANCE_DEFENSE )
  {
    _init_warrior_spell_t();
  }

  warrior_spell_t( const char* n, uint32_t id, warrior_t* p, int t=TREE_NONE ) :
      spell_t( n, id, p, t ),
      stancemask( STANCE_BATTLE|STANCE_BERSERKER|STANCE_DEFENSE )
  {
    _init_warrior_spell_t();
  }

  void _init_warrior_spell_t()
  {
  }
   
  virtual double cost() SC_CONST;
  virtual double gcd() SC_CONST;
  virtual bool   ready();
  virtual void   parse_options( option_t* options, const std::string& options_str );
};

// warrior_spell_t::cost ====================================================

double warrior_spell_t::cost() SC_CONST
{
  double c = spell_t::cost() / 10; // Rage Costs are stored as * 10;
  return c;
}

// warrior_spell_t::gcd =====================================================

double warrior_spell_t::gcd() SC_CONST
{
  // Unaffected by haste
  return trigger_gcd;
}

// warrior_spell_t::ready() =================================================

bool warrior_spell_t::ready()
{
  if ( ! spell_t::ready() )
    return false;

  warrior_t* p = player -> cast_warrior();

  // Attack vailable in current stance?
  if ( ( stancemask & p -> active_stance ) == 0 )
    return false;

  return true;
}

// warrior_spell_t::parse_options() =========================================

void warrior_spell_t::parse_options( option_t* options, const std::string& options_str )
{
  option_t base_options[] =
  {
    { NULL, OPT_UNKNOWN, NULL }
  };
  std::vector<option_t> merged_options;
  spell_t::parse_options( merge_options( merged_options, base_options, options ), options_str );
}

// Battle Shout =============================================================

struct battle_shout_t : public warrior_spell_t
{
  double rage_gain;

  battle_shout_t( warrior_t* p, const std::string& options_str ) :
      warrior_spell_t( "battle_shout", p )
  {
    parse_options( NULL, options_str );

    id = 6673;
    parse_data( p -> player_data );

    harmful   = false;
    rage_gain = 20 + p -> talents.booming_voice -> effect_base_value( 2 ) / 10.0;
    cooldown -> duration += p -> talents.booming_voice -> effect_base_value( 1 ) / 1000.0;
  }
  
  virtual void execute()
  {
    warrior_spell_t::execute();
    warrior_t* p = player -> cast_warrior();
    
    if ( ! sim -> overrides.battle_shout )
      sim -> auras.battle_shout -> buff_duration = 120 + p -> glyphs.battle -> effect_base_value( 1 ) / 1000.0;
    sim -> auras.battle_shout -> trigger( 1, sim -> sim_data.effect_min( 6673, p -> level, E_APPLY_AURA, A_MOD_STAT ) );

    p -> resource_gain( RESOURCE_RAGE, rage_gain , p -> gains_battle_shout );
  }
};

// Berserker Rage ===========================================================

struct berserker_rage_t : public warrior_spell_t
{
  berserker_rage_t( warrior_t* p, const std::string& options_str ) :
      warrior_spell_t( "berserker_rage", p )
  {
    parse_options( NULL, options_str );

    id = 18499;
    parse_data( p -> player_data );

    harmful = false;
    if ( p -> talents.intensify_rage -> ok() )
      cooldown -> duration *= ( 1.0 + p -> talents.intensify_rage -> effect_base_value( 1 ) / 100.0 );
  }

  virtual void execute()
  {
    warrior_spell_t::execute();
    warrior_t* p = player -> cast_warrior();
    if ( sim -> log ) log_t::output( sim, "%s performs %s", p -> name(), name() );  
    if ( p -> glyphs.berserker_rage -> ok() ) {
      double ragegain = 5.0;
      ragegain *= 1.0 + p -> composite_mastery() * p -> mastery.unshackled_fury -> effect_base_value( 3 ) / 10000.0;
      p -> resource_gain( RESOURCE_RAGE, ragegain, p -> gains_berserker_rage );
    }
    p -> buffs_berserker_rage -> trigger();
  }
};

// Deadly Calm ==============================================================

struct deadly_calm_t : public warrior_spell_t
{
  deadly_calm_t( warrior_t* p, const std::string& options_str ) :
    warrior_spell_t( "deadly_calm", p, SCHOOL_PHYSICAL, TREE_PROTECTION )
  {
    check_talent( p -> talents.deadly_calm -> rank() );

    parse_options( NULL, options_str );

    id = 85730;
    parse_data( p -> player_data );

    harmful = false;
  }

  virtual void execute()
  {
    warrior_spell_t::execute();
    warrior_t* p = player -> cast_warrior();
    if ( sim -> log ) log_t::output( sim, "%s performs %s", p -> name(), name() );    
    p -> buffs_deadly_calm -> trigger();  
    update_ready();
  }

  virtual bool ready()
  {
    warrior_t* p = player -> cast_warrior();
    if ( p -> buffs_inner_rage -> check() )
      return false;
    return warrior_spell_t::ready();
  }
};

// Death Wish ===============================================================

struct death_wish_t : public warrior_spell_t
{
  double enrage_bonus;

  death_wish_t( warrior_t* p, const std::string& options_str ) :
      warrior_spell_t( "death_wish", p )
  {
    check_talent( p -> talents.death_wish -> rank() );

    parse_options( NULL, options_str );

    id = 12292;
    parse_data( p -> player_data );

    harmful = false;
    if ( p -> talents.intensify_rage -> ok() )
      cooldown -> duration *= ( 1.0 + p -> talents.intensify_rage -> effect_base_value( 1 ) / 100.0 );
  }

  virtual void execute()
  {
    warrior_spell_t::execute();
    warrior_t* p = player -> cast_warrior();

    enrage_bonus = p -> talents.death_wish -> effect_base_value( 1 ) / 100.0;
    enrage_bonus *= 1.0 + p -> composite_mastery() * p -> mastery.unshackled_fury -> effect_base_value( 3 ) / 10000.0; 

    p -> buffs_death_wish -> trigger( 1, enrage_bonus );
    p -> buffs_enrage -> expire();
  }
};

// Inner Rage ===============================================================

struct inner_rage_t : public warrior_spell_t
{
  inner_rage_t( warrior_t* p, const std::string& options_str ) :
    warrior_spell_t( "inner_rage", p, SCHOOL_PHYSICAL, TREE_PROTECTION )
  {
    check_min_level( 83 );

    parse_options( NULL, options_str );

    id = 1134;
    parse_data( p -> player_data );

    harmful = false;
    cooldown -> duration = 1.5;
  }

  virtual void execute()
  {
    warrior_t* p = player -> cast_warrior();
    if ( sim -> log ) log_t::output( sim, "%s performs %s", p -> name(), name() );    
    p -> buffs_inner_rage -> trigger();  
    update_ready();
  }

  virtual bool ready()
  {
    warrior_t* p = player -> cast_warrior();
    if ( p -> resource_current[ RESOURCE_RAGE ] < 75.0 )
      return false;
    return warrior_spell_t::ready();
  }
};

// Recklessness =============================================================

struct recklessness_t : public warrior_spell_t
{
  recklessness_t( warrior_t* p, const std::string& options_str ) :
      warrior_spell_t( "recklessness", p )
  {
    parse_options( NULL, options_str );

    id = 1719;
    parse_data( p -> player_data );

    harmful = false;

    cooldown -> duration *= 1.0 + p -> talents.intensify_rage -> effect_base_value( 1 ) / 100.0;

    stancemask  = STANCE_BERSERKER;
  }

  virtual void execute()
  {
    warrior_spell_t::execute();
    warrior_t* p = player -> cast_warrior();
    p -> buffs_recklessness -> trigger( 3 );
  }
};

// Shield Block =============================================================

struct shield_block_t : public warrior_spell_t
{
  shield_block_t( warrior_t* p, const std::string& options_str ) :
      warrior_spell_t( "shield_block", p )
  {
    parse_options( NULL, options_str );

    id = 2565;
    parse_data( p -> player_data );

    harmful = false;
    if ( p -> talents.shield_mastery -> ok() )
      cooldown -> duration += p -> talents.shield_mastery -> effect_base_value( 1 ) / 1000.0;
  }

  virtual void execute()
  {
    warrior_spell_t::execute();
    warrior_t* p = player -> cast_warrior();
    p -> buffs_shield_block -> trigger();
  }
};

// Stance ===================================================================

struct stance_t : public warrior_spell_t
{
  int switch_to_stance;
  stance_t( warrior_t* p, const std::string& options_str ) :
      warrior_spell_t( "stance", p ), switch_to_stance( 0 )
  {
    std::string stance_str;
    option_t options[] =
    {
      { "choose",  OPT_STRING, &stance_str     },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    if ( ! stance_str.empty() )
    {
      if ( stance_str == "battle" )
        switch_to_stance = STANCE_BATTLE;
      else if ( stance_str == "berserker" || stance_str == "zerker" )
        switch_to_stance = STANCE_BERSERKER;
      else if ( stance_str == "def" || stance_str == "defensive" )
        switch_to_stance = STANCE_DEFENSE;
    }
    else
    {
      // Default to Battle Stance
      switch_to_stance = STANCE_BATTLE;
    }

    harmful = false;
    base_cost   = 0;
    trigger_gcd = 0;
    cooldown -> duration = 1.0;
  }

  virtual void execute()
  {
    warrior_t* p = player -> cast_warrior();

    switch ( p -> active_stance )
    {
      case STANCE_BATTLE:     p -> buffs_battle_stance    -> expire(); break;
      case STANCE_BERSERKER:  p -> buffs_berserker_stance -> expire(); break;
      case STANCE_DEFENSE:    p -> buffs_defensive_stance -> expire(); break;
    }
    p -> active_stance = switch_to_stance;

    switch ( p -> active_stance )
    {
      case STANCE_BATTLE:     p -> buffs_battle_stance    -> trigger(); break;
      case STANCE_BERSERKER:  p -> buffs_berserker_stance -> trigger(); break;
      case STANCE_DEFENSE:    p -> buffs_defensive_stance -> trigger(); break;
    }
    consume_resource();

    update_ready();
  }

  virtual double cost() SC_CONST
  {
    warrior_t* p = player -> cast_warrior();
    double c = p -> resource_current [ RESOURCE_RAGE ];
    c -= 25.0; // Stance Mastery
    if ( p -> talents.tactical_mastery -> ok() )
      c -= p -> talents.tactical_mastery -> effect_base_value( 1 );
    if ( c < 0 ) c = 0;
    return c;
  }

  virtual bool ready()
  {
    warrior_t* p = player -> cast_warrior();

    if ( ! warrior_spell_t::ready() )
      return false;

    return p -> active_stance != switch_to_stance;
  }
};

// Sweeping Strikes =========================================================

struct sweeping_strikes_t : public warrior_spell_t
{
  sweeping_strikes_t( warrior_t* p, const std::string& options_str ) :
    warrior_spell_t( "sweeping_strikes", p, SCHOOL_PHYSICAL, TREE_PROTECTION )
  {
    check_talent( p -> talents.sweeping_strikes -> rank() );

    parse_options( NULL, options_str );

    id = 12328;
    parse_data( p -> player_data );

    base_cost *= 1.0 + p -> glyphs.sweeping_strikes -> effect_base_value( 1 ) / 100.0;

    stancemask = STANCE_BERSERKER | STANCE_BATTLE;
  }

  virtual void execute()
  {
    warrior_spell_t::execute();
    warrior_t* p = player -> cast_warrior();
    if ( sim -> log ) log_t::output( sim, "%s performs %s", p -> name(), name() );    
    p -> buffs_sweeping_strikes -> trigger();  
    update_ready();
  }
};

} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// Warrior Character Definition
// ==========================================================================

// warrior_t::create_action  ================================================

action_t* warrior_t::create_action( const std::string& name,
                                    const std::string& options_str )
{
  if ( name == "auto_attack"      ) return new auto_attack_t     ( this, options_str );
  if ( name == "battle_shout"     ) return new battle_shout_t    ( this, options_str );
  if ( name == "berserker_rage"   ) return new berserker_rage_t  ( this, options_str );
  if ( name == "bladestorm"       ) return new bladestorm_t      ( this, options_str );
  if ( name == "bloodthirst"      ) return new bloodthirst_t     ( this, options_str );
  if ( name == "cleave"           ) return new cleave_t          ( this, options_str );
  if ( name == "colossus_smash"   ) return new colossus_smash_t  ( this, options_str );
  if ( name == "concussion_blow"  ) return new concussion_blow_t ( this, options_str );
  if ( name == "deadly_calm"      ) return new deadly_calm_t     ( this, options_str );
  if ( name == "death_wish"       ) return new death_wish_t      ( this, options_str );
  if ( name == "devastate"        ) return new devastate_t       ( this, options_str );
  if ( name == "execute"          ) return new execute_t         ( this, options_str );
  if ( name == "heroic_strike"    ) return new heroic_strike_t   ( this, options_str );
  if ( name == "inner_rage"       ) return new inner_rage_t      ( this, options_str );
  if ( name == "mortal_strike"    ) return new mortal_strike_t   ( this, options_str );
  if ( name == "overpower"        ) return new overpower_t       ( this, options_str );
  if ( name == "pummel"           ) return new pummel_t          ( this, options_str );
  if ( name == "raging_blow"      ) return new raging_blow_t     ( this, options_str );
  if ( name == "recklessness"     ) return new recklessness_t    ( this, options_str );
  if ( name == "rend"             ) return new rend_t            ( this, options_str );
  if ( name == "revenge"          ) return new revenge_t         ( this, options_str );
  if ( name == "shattering_throw" ) return new shattering_throw_t( this, options_str );
  if ( name == "shield_bash"      ) return new shield_bash_t     ( this, options_str );
  if ( name == "shield_block"     ) return new shield_block_t    ( this, options_str );
  if ( name == "shield_slam"      ) return new shield_slam_t     ( this, options_str );
  if ( name == "shockwave"        ) return new shockwave_t       ( this, options_str );
  if ( name == "slam"             ) return new slam_t            ( this, options_str );
  if ( name == "stance"           ) return new stance_t          ( this, options_str );
  if ( name == "sweeping_strikes" ) return new sweeping_strikes_t( this, options_str );
  if ( name == "thunder_clap"     ) return new thunder_clap_t    ( this, options_str );
  if ( name == "victory_rush"     ) return new victory_rush_t    ( this, options_str );
  if ( name == "whirlwind"        ) return new whirlwind_t       ( this, options_str );

  return player_t::create_action( name, options_str );
}

// warrior_t::init_talents ==================================================

void warrior_t::init_talents()
{
  // Talents

  // Arms
  talents.bladestorm              = find_talent( "Bladestorm" );
  talents.blood_frenzy            = find_talent( "Blood Frenzy" );
  talents.deadly_calm             = find_talent( "Deadly Calm" );
  talents.deep_wounds             = find_talent( "Deep Wounds" );
  talents.drums_of_war            = find_talent( "Drums of War" );
  talents.impale                  = find_talent( "Impale" );
  talents.improved_slam           = find_talent( "Improved Slam" );
  talents.lambs_to_the_slaughter  = find_talent( "Lambs to the Slaughter" );
  talents.sudden_death            = find_talent( "Sudden Death" );
  talents.sweeping_strikes        = find_talent( "Sweeping Strikes" );
  talents.tactical_mastery        = find_talent( "Tactical Mastery" );
  talents.taste_for_blood         = find_talent( "Taste for Blood" );
  talents.war_academy             = find_talent( "War Academy" );
  talents.wrecking_crew           = find_talent( "Wrecking Crew" );

  // Fury
  talents.battle_trance           = find_talent( "Battle Trance" );
  talents.bloodsurge              = find_talent( "Bloodsurge" );
  talents.booming_voice           = find_talent( "Booming Voice" );
  talents.cruelty                 = find_talent( "Cruelty" );
  talents.death_wish              = find_talent( "Death Wish" );
  talents.enrage                  = find_talent( "Enrage" );
  talents.executioner             = find_talent( "Executioner" );
  talents.flurry                  = find_talent( "Flurry" );
  talents.gag_order               = find_talent( "Gag Order" );
  talents.intensify_rage          = find_talent( "Intensify Rage" );
  talents.meat_cleaver            = find_talent( "Meat Cleaver" );
  talents.raging_blow             = find_talent( "Raging Blow" );
  talents.rampage                 = find_talent( "Rampage" );
  talents.rude_interruption       = find_talent( "Rude Interruption" );
  talents.single_minded_fury      = find_talent( "Single-Minded Fury" );
  talents.titans_grip             = find_talent( "Titan's Grip" );

  // Prot
  talents.bastion_of_defense      = find_talent( "Bastion of Defense" );
  talents.concussion_blow         = find_talent( "Concussion Blow" );
  talents.devastate               = find_talent( "Devastate" );
  talents.heavy_repercussions     = find_talent( "Heavy Repercussions" );
  talents.hold_the_line           = find_talent( "Hold the Line" );
  talents.impending_victory       = find_talent( "Impending Victory" );
  talents.improved_revenge        = find_talent( "Improved Revenge" );
  talents.incite                  = find_talent( "Incite" );
  talents.shockwave               = find_talent( "Shockwave" );
  talents.shield_mastery          = find_talent( "Shield Mastery" );
  talents.shield_specialization   = find_talent( "Shield Specialization" );
  talents.sword_and_board         = find_talent( "Sword and Board" );
  talents.toughness               = find_talent( "Toughness" );
  talents.thunderstruck           = find_talent( "Thunderstruck" );

  player_t::init_talents();
}

// warrior_t::init_spells ==================================================

void warrior_t::init_spells()
{
  player_t::init_spells();

  // Active Spells
  active_spells.battle_shout      = new active_spell_t( this, "battle_shout", "Battle Shout" );
  active_spells.berserker_rage    = new active_spell_t( this, "berserker_rage", "Berserker Rage" );
  active_spells.bladestorm        = new active_spell_t( this, "bladestorm", "Bladestorm" );
  active_spells.bloodthirst       = new active_spell_t( this, "bloodthirst", "Bloodthirst" );
  active_spells.cleave            = new active_spell_t( this, "cleave", "Cleave" );
  active_spells.colossus_smash    = new active_spell_t( this, "colossus_smash", "Colossus Smash" );
  active_spells.concussion_blow   = new active_spell_t( this, "concussion_blow", "Concussion Blow" );
  active_spells.deadly_calm       = new active_spell_t( this, "deadly_calm", "Deadly Calm" );
  active_spells.death_wish        = new active_spell_t( this, "death_wish", "Death Wish" );
  active_spells.devastate         = new active_spell_t( this, "devastate", "Devastate" );
  active_spells.execute           = new active_spell_t( this, "execute", "Execute" );
  active_spells.heroic_strike     = new active_spell_t( this, "heroic_strike", "Heroic Strike" );
  active_spells.inner_rage        = new active_spell_t( this, "inner_rage", "Inner Rage" );
  active_spells.mortal_strike     = new active_spell_t( this, "mortal_strike", "Mortal Strike" );
  active_spells.overpower         = new active_spell_t( this, "overpower", "Overpower" );
  active_spells.pummel            = new active_spell_t( this, "pummel", "Pummel" );
  active_spells.raging_blow       = new active_spell_t( this, "raging_blow", "Raging Blow" );
  active_spells.recklessness      = new active_spell_t( this, "recklessness", "Recklessness" );
  active_spells.rend              = new active_spell_t( this, "rend", "Rend" );
  active_spells.revenge           = new active_spell_t( this, "revenge", "Revenge" );
  active_spells.shattering_throw  = new active_spell_t( this, "shattering_throw", "Shattering Throw" );
  active_spells.shield_bash       = new active_spell_t( this, "shield_bash", "Shield Bash" );
  active_spells.shield_block      = new active_spell_t( this, "shield_block", "Shield Block" );
  active_spells.shield_slam       = new active_spell_t( this, "shield_slam", "Shield Slam" );
  active_spells.shockwave         = new active_spell_t( this, "shockwave", "Shockwave" );
  active_spells.slam              = new active_spell_t( this, "slam", "Slam" );
  active_spells.sweeping_strikes  = new active_spell_t( this, "sweeping_strikes", "Sweeping Strikes" );
  active_spells.thunder_clap      = new active_spell_t( this, "thunder_clap", "Thunder Clap" );
  active_spells.victory_rush      = new active_spell_t( this, "victory_rush", "Victory Rush" );
  active_spells.whirlwind         = new active_spell_t( this, "whirlwind", "Whirlwind" );

  // Mastery
  mastery.critical_block         = new mastery_t( this, "critical_block",         76857, TREE_PROTECTION );
  mastery.strikes_of_opportunity = new mastery_t( this, "strikes_of_opportunity", 76838, TREE_ARMS );
  mastery.unshackled_fury        = new mastery_t( this, "unshackled_fury",        76856, TREE_FURY );

  // Spec Passives
  spec.anger_management                 = new passive_spell_t( this, "anger_management",                 12296 );
  spec.dual_wield_specialization        = new passive_spell_t( this, "dual_wield_specialization",        23588 );
  spec.precision                        = new passive_spell_t( this, "precision",                        29592 );
  spec.sentinel                         = new passive_spell_t( this, "sentinel",                         29144 );
  spec.two_handed_weapon_specialization = new passive_spell_t( this, "two_handed_weapon_specialization", 12712 );

  glyphs.battle              = find_glyph( "Glyph of Battle" );
  glyphs.berserker_rage      = find_glyph( "Glyph of Berserker Rage" );
  glyphs.bladestorm          = find_glyph( "Glyph of Bladestorm" );
  glyphs.bloodthirst         = find_glyph( "Glyph of Bloodthirst" );
  glyphs.bloody_healing      = find_glyph( "Glyph of Bloody Healing" );
  glyphs.cleaving            = find_glyph( "Glyph of Cleaving" );
  glyphs.command             = find_glyph( "Glyph of Command" );
  glyphs.devastate           = find_glyph( "Glyph of Devastate" );
  glyphs.heroic_throw        = find_glyph( "Glyph of Heroic Throw" );
  glyphs.mortal_strike       = find_glyph( "Glyph of Mortal Strike" );
  glyphs.overpower           = find_glyph( "Glyph of Overpower" );
  glyphs.raging_blow         = find_glyph( "Glyph of Raging Blow" );
  glyphs.resonating_power    = find_glyph( "Glyph of Resonating Power" );
  glyphs.revenge             = find_glyph( "Glyph of Revenge" );
  glyphs.shield_slam         = find_glyph( "Glyph of Shield Slam" );
  glyphs.shockwave           = find_glyph( "Glyph of Shockwave" );
  glyphs.slam                = find_glyph( "Glyph of Slam" );
  glyphs.sweeping_strikes    = find_glyph( "Glyph of Sweeping Strikes" );
  glyphs.victory_rush        = find_glyph( "Glyph of Victory Rush" );

  static uint32_t set_bonuses[N_TIER][N_TIER_BONUS] = 
  {
    //  C2P    C4P    M2P    M4P    T2P    T4P    H2P    H4P
    {     0,     0, 70854, 70847, 70843, 70844,     0,     0 }, // Tier10
    {     0,     0, 90293, 90295, 90296, 90297,     0,     0 }, // Tier11
    {     0,     0,     0,     0,     0,     0,     0,     0 },
  };

  sets = new set_bonus_array_t( this, set_bonuses );
}

// warrior_t::init_base =====================================================

void warrior_t::init_base()
{
  player_t::init_base();

  resource_base[  RESOURCE_RAGE  ] = 100;

  initial_attack_power_per_strength = 2.0;
  initial_attack_power_per_agility  = 0.0;

  base_attack_power = level * 2 + 60;

  // FIXME! Level-specific!
  base_miss    = 0.05;
  base_parry   = 0.05;
  base_block   = 0.05;



  if ( talents.toughness -> ok() )
    initial_armor_multiplier *= 1.0 + talents.toughness -> effect_base_value( 1 ) / 100.0;

  diminished_kfactor    = 0.009560;
  diminished_dodge_capi = 0.01523660;
  diminished_parry_capi = 0.01523660;

  if ( spec.sentinel -> ok() )
  {
    attribute_multiplier_initial[ ATTR_STAMINA ] *= 1.0  + spec.sentinel -> effect_base_value( 1 ) / 100.0;
    base_block += spec.sentinel -> effect_base_value( 3 ) / 100.0;
  }

  base_gcd = 1.5;
}

// warrior_t::init_scaling ==================================================

void warrior_t::init_scaling()
{
  player_t::init_scaling();

  if ( talents.single_minded_fury -> ok() || talents.titans_grip -> ok() )
  {
    scales_with[ STAT_WEAPON_OFFHAND_DPS    ] = 1;
    scales_with[ STAT_WEAPON_OFFHAND_SPEED  ] = 1;
  }
}

// warrior_t::init_buffs ====================================================

void warrior_t::init_buffs()
{
  player_t::init_buffs();

  // buff_t( sim, name, max_stack, duration, cooldown, proc_chance, quiet )
  buffs_bastion_of_defense        = new buff_t( this, "bastion_of_defense",        1, 12.0,   0, talents.bastion_of_defense -> proc_chance() );
  buffs_battle_stance             = new buff_t( this, 21156, "battle_stance" );
  buffs_battle_trance             = new buff_t( this, "battle_trance",             1, 15.0,   0, talents.battle_trance -> proc_chance() );
  buffs_berserker_rage            = new buff_t( this, "berserker_rage",            1, 10.0 );
  buffs_berserker_stance          = new buff_t( this, 7381, "berserker_stance" );
  buffs_bloodsurge                = new buff_t( this, "bloodsurge",                1, 10.0,   0, talents.bloodsurge -> proc_chance() );
  buffs_colossus_smash            = new buff_t( this, "colossus_smash",            1,  6.0 );
  buffs_deadly_calm               = new buff_t( this, "deadly_calm",               1, 10.0 );
  buffs_death_wish                = new buff_t( this, "death_wish",                1, 30.0 );
  buffs_defensive_stance          = new buff_t( this, 7376, "defensive_stance" );
  buffs_enrage                    = new buff_t( this, talents.enrage -> effect_trigger_spell( 1 ), "enrage",  talents.enrage -> proc_chance() );
  buffs_executioner_talent        = new buff_t( this, talents.executioner -> effect_trigger_spell( 1 ), "executioner_talent", talents.executioner -> proc_chance() );
  buffs_flurry                    = new buff_t( this, talents.flurry -> effect_trigger_spell( 1 ), "flurry", talents.flurry -> proc_chance() );
  buffs_hold_the_line             = new buff_t( this, "hold_the_line",             1,  5.0 * talents.hold_the_line -> rank() ); 
  buffs_incite                    = new buff_t( this, "incite",                    1, 10.0, 0.0, talents.incite -> proc_chance() );
  buffs_inner_rage                = new buff_t( this, 1134, "inner_rage" );
  buffs_overpower                 = new buff_t( this, "overpower",                 1,  6.0, 1.0 );
  buffs_lambs_to_the_slaughter    = new buff_t( this, "lambs_to_the_slaughter",    3, 15.0, 0, talents.lambs_to_the_slaughter -> proc_chance() );
  buffs_meat_cleaver              = new buff_t( this, "meat_cleaver",              3, 10.0,  0, talents.meat_cleaver -> proc_chance() );
  buffs_recklessness              = new buff_t( this, "recklessness",              3, 12.0 );
  buffs_revenge                   = new buff_t( this, "revenge",                   1,  5.0 );
  buffs_rude_interruption         = new buff_t( this, "rude_interruption",         1, 15.0 * talents.rude_interruption ->rank() );
  buffs_shield_block              = new buff_t( this, "shield_block",              1, 10.0 );
  buffs_sweeping_strikes          = new buff_t( this, "sweeping_strikes",          1, 10.0 );
  buffs_sword_and_board           = new buff_t( this, "sword_and_board",           1,  5.0,   0, talents.sword_and_board -> proc_chance() );
  buffs_taste_for_blood           = new buff_t( this, "taste_for_blood",           1,  9.0, 5.0, talents.taste_for_blood -> proc_chance() );
  buffs_thunderstruck             = new buff_t( this, "thunderstruck",             3, 20.0,   0, talents.thunderstruck -> proc_chance() );
  buffs_victory_rush              = new buff_t( this, "victory_rush",              1, 20.0 );
  buffs_wrecking_crew             = new buff_t( this, "wrecking_crew",             1, 12.0,   0 );
  buffs_tier10_2pc_melee          = new buff_t( this, "tier10_2pc_melee",          1, 10.0,   0, set_bonus.tier10_2pc_melee() * 0.02 );
  buffs_tier11_4pc_melee          = new buff_t( this, "tier11_4pc_melee",          3, 30.0,   0, set_bonus.tier11_4pc_melee() );
  // buff_t( sim, name, max_stack, duration, cooldown, proc_chance, quiet )
}

// warrior_t::init_gains ====================================================

void warrior_t::init_gains()
{
  player_t::init_gains();

  gains_anger_management       = get_gain( "anger_management"      );
  gains_avoided_attacks        = get_gain( "avoided_attacks"       );
  gains_battle_shout           = get_gain( "battle_shout"          );
  gains_berserker_rage         = get_gain( "berserker_rage"        );
  gains_blood_frenzy           = get_gain( "blood_frenzy"          );
  gains_incoming_damage        = get_gain( "incoming_damage"       );
  gains_melee_main_hand        = get_gain( "melee_main_hand"       );
  gains_melee_off_hand         = get_gain( "melee_off_hand"        );
  gains_shield_specialization  = get_gain( "shield_specialization" );
  gains_sudden_death           = get_gain( "sudden_death"          );
}

// warrior_t::init_procs ====================================================

void warrior_t::init_procs()
{
  player_t::init_procs();

  procs_munched_deep_wounds     = get_proc( "munched_deep_wounds"    );
  procs_rolled_deep_wounds      = get_proc( "rolled_deep_wounds"     );
  procs_parry_haste             = get_proc( "parry_haste"            );
  procs_strikes_of_opportunity  = get_proc( "strikes_of_opportunity" );
  procs_sudden_death            = get_proc( "sudden_death"           );
}

// warrior_t::init_uptimes ==================================================

void warrior_t::init_uptimes()
{
  player_t::init_uptimes();

  uptimes_rage_cap    = get_uptime( "rage_cap" );
}

// warrior_t::init_rng ======================================================

void warrior_t::init_rng()
{
  player_t::init_rng();

  rng_blood_frenzy              = get_rng( "blood_frenzy"              );
  rng_executioner_talent        = get_rng( "executioner_talent"        );
  rng_impending_victory         = get_rng( "impending_victory"         );
  rng_strikes_of_opportunity    = get_rng( "strikes_of_opportunity"    );
  rng_sudden_death              = get_rng( "sudden_death"              );
  rng_wrecking_crew             = get_rng( "wrecking_crew"             );
}

// warrior_t::init_actions ==================================================

void warrior_t::init_actions()
{
  if ( main_hand_weapon.type == WEAPON_NONE )
  {
    sim -> errorf( "Player %s has no weapon equipped at the Main-Hand slot.", name() );
    quiet = true;
    return;
  }

  if ( action_list_str.empty() )
  {
    // Flask
    // TO-DO: Revert to >= 80 when Cata is out
    if ( level > 80 )
      action_list_str += "/flask,type=titanic_strength";
    else
      action_list_str += "/flask,type=endless_rage";
    
    // Food
    if ( level >= 85 )
      action_list_str += "/food,type=beer_basted_crocolisk";
    else
      action_list_str += "/food,type=dragonfin_filet";

    // Stance
    if ( primary_tree() == TREE_ARMS )
      action_list_str += "/stance,choose=battle,if=!in_combat";
    else if ( primary_tree() == TREE_FURY )
      action_list_str += "/stance,choose=berserker,if=!in_combat";
    else if ( primary_tree() == TREE_PROTECTION )
      action_list_str += "/stance,choose=defensive,if=!in_combat";

    action_list_str += "/snapshot_stats";

    // Potion
    if ( primary_tree() == TREE_ARMS || primary_tree() == TREE_FURY )
    {
      // TO-DO: Revert to >= 80 when Cata is out
      if (level > 80 )
        action_list_str += "/golemblood_potion,if=!in_combat|buff.bloodlust.react";
      else
        action_list_str += "/speed_potion,if=!in_combat|buff.bloodlust.react";
    }
    else
    {
      // FIXME: earthen potion, but when to use?
    }

    action_list_str += "/auto_attack";

    // Usable Item
    int num_items = ( int ) items.size();
    for ( int i=0; i < num_items; i++ )
    {
      if ( items[ i ].use.active() )
      {
        action_list_str += "/use_item,name=";
        action_list_str += items[ i ].name();
      }
    }

    // Race Skills
    if ( race == RACE_ORC )
      action_list_str += "/blood_fury";
    else if ( race == RACE_TROLL )
      action_list_str += "/berserking";
    else if ( race == RACE_BLOOD_ELF )
      action_list_str += "/arcane_torrent";

    if ( primary_tree() == TREE_ARMS )
    {
      if ( talents.deadly_calm -> ok() ) action_list_str += "/deadly_calm,if=rage<20";
      if ( talents.sweeping_strikes -> ok() ) action_list_str += "/sweeping_strikes,if=target.adds>0";
      if ( glyphs.berserker_rage -> ok() ) action_list_str += "/berserker_rage";
      // Don't want to bladestorm during SS as it's only 1 extra hit per WW not per target
      action_list_str += "/bladestorm,if=target.adds>0&!buff.deadly_calm.up&!buff.sweeping_strikes.up";
      action_list_str += "/cleave,if=target.adds>0";
      action_list_str += "/heroic_strike,if=target.adds=0&(rage>65|buff.deadly_calm.up|buff.incite.up|buff.battle_trance.up)";
      action_list_str += "/overpower,if=buff.taste_for_blood.remains<1.5";
      action_list_str += "/rend,if=!ticking";
      if ( level >= 81 ) action_list_str += "/colossus_smash,if=!buff.colossus_smash.up";
      action_list_str += "/mortal_strike";
      action_list_str += "/overpower";
      action_list_str += "/execute";
      if ( glyphs.slam -> ok() ) action_list_str += "/slam,if=cooldown.mortal_strike.remains>=1.5";
      action_list_str += "/battle_shout,if=rage<25";
    }
    else if ( primary_tree() == TREE_FURY )
    {
      action_list_str += "/recklessness";
      if ( talents.death_wish -> ok() ) action_list_str += "/death_wish";
      action_list_str += "/heroic_strike,if=target.adds=0&(rage>60|buff.battle_trance.up|buff.incite.up)&target.health_pct>=20";
      action_list_str += "/cleave,if=target.adds>0";
      action_list_str += "/whirlwind,if=target.adds>0";      
      if ( level >= 81 ) action_list_str += "/colossus_smash";
      if ( level >= 83 ) action_list_str += "/inner_rage,if=target.health_pct<20";
      action_list_str += "/execute,if=buff.colossus_smash.up|buff.inner_rage.up";
      if ( talents.titans_grip -> ok() && talents.raging_blow -> ok() )
      {
        action_list_str += "/berserker_rage,if=!(buff.death_wish.up|buff.enrage.up|buff.unholy_frenzy.up)&rage>15&cooldown.raging_blow.remains<1";
        action_list_str += "/raging_blow,if=target.health_pct>=20";
      }
      action_list_str += "/bloodthirst";
      action_list_str += "/slam,if=buff.bloodsurge.react";
      if ( ! talents.titans_grip -> ok() && talents.raging_blow -> ok() )
      {
        action_list_str += "/berserker_rage,if=!(buff.death_wish.up|buff.enrage.up|buff.unholy_frenzy.up)&rage>15&cooldown.raging_blow.remains<1";
        action_list_str += "/raging_blow";
      }
      action_list_str += "/battle_shout,if=rage<70";
      if (! talents.raging_blow -> ok() && glyphs.berserker_rage -> ok() ) 
        action_list_str += "/berserker_rage";
    }
    else if ( primary_tree() == TREE_PROTECTION )
    {
      action_list_str += "/heroic_strike,if=rage>=15";
      action_list_str += "/revenge";
      if ( talents.shockwave -> ok() ) action_list_str += "/shockwave";
      action_list_str += "/shield_block,sync=shield_slam";
      action_list_str += "/shield_slam";
      if ( talents.devastate -> ok() ) action_list_str += "/devastate";
      action_list_str += "/battle_shout";
      if ( glyphs.berserker_rage -> ok() ) action_list_str += "/berserker_rage";
    }
    else
    {
      action_list_str += "/stance,choose=berserker/auto_attack";
    }

    action_list_default = 1;
  }

  player_t::init_actions();
}

// warrior_t::combat_begin ==================================================

void warrior_t::combat_begin()
{
  player_t::combat_begin();

  // We (usually) start combat with zero rage.
  resource_current[ RESOURCE_RAGE ] = std::min( initial_rage, 100 );

  if ( active_stance == STANCE_BATTLE && ! buffs_battle_stance -> check() )
  {
    buffs_battle_stance -> trigger();
  }
  if ( talents.rampage -> ok() )
  {
    sim -> auras.rampage -> trigger();
  }
}

// warrior_t::reset =========================================================

void warrior_t::reset()
{
  player_t::reset();
  active_stance = STANCE_BATTLE;
}

// warrior_t::interrupt =====================================================

void warrior_t::interrupt()
{
  player_t::interrupt();

  buffs_rude_interruption -> trigger();

  if ( main_hand_attack ) main_hand_attack -> cancel();
  if (  off_hand_attack )  off_hand_attack -> cancel();
}

// warrior_t::composite_attack_power_multiplier =============================

double warrior_t::composite_attack_power_multiplier() SC_CONST
{
  double mult = player_t::composite_attack_power_multiplier();

  if ( buffs_tier11_4pc_melee -> up() )
  {
    mult *= 1.0 + 0.01 * buffs_tier11_4pc_melee -> stack();
  }
  return mult;
}

// warrior_t::composite_attack_hit ==========================================

double warrior_t::composite_attack_hit() SC_CONST
{
  double ah = player_t::composite_attack_hit();
  ah += spec.precision -> effect_base_value( 1 ) / 100.0;
  return ah;
}

// warrior_t::matching_gear_multiplier ======================================

double warrior_t::matching_gear_multiplier( const attribute_type attr ) SC_CONST
{
  if ( ( attr == ATTR_STRENGTH ) && ( primary_tree() == TREE_ARMS || primary_tree() == TREE_FURY ) )
    return 0.05;
  if ( ( attr == ATTR_STAMINA ) && ( primary_tree() == TREE_PROTECTION ) )
    return 0.05;

  return 0.0;
}

// warrior_t::composite_tank_block ==========================================

double warrior_t::composite_tank_block() SC_CONST
{
  double b = player_t::composite_tank_block();
  b += composite_mastery() * mastery.critical_block -> effect_base_value( 3 ) / 10000.0;

  if ( buffs_shield_block -> up() )
    b = 1.0;
  return b;
}

// warrior_t::composite_tank_crit ==========================================

double warrior_t::composite_tank_crit( const school_type school ) SC_CONST
{
  double c = player_t::composite_tank_crit( school );

  if ( school == SCHOOL_PHYSICAL && talents.bastion_of_defense -> rank() )
    c += talents.bastion_of_defense -> effect_base_value( 1 ) / 100.0;

  return c;
}

// warrior_t::regen =========================================================

void warrior_t::regen( double periodicity )
{
  player_t::regen( periodicity );

  if ( spec.anger_management -> ok() )
  {
    resource_gain( RESOURCE_RAGE, ( periodicity / 3.0 ), gains_anger_management );
  }

  uptimes_rage_cap -> update( resource_current[ RESOURCE_RAGE ] ==
                              resource_max    [ RESOURCE_RAGE] );
}

// warrior_t::resource_loss =================================================

double warrior_t::resource_loss( int       resource,
                                 double    amount,
                                 action_t* action )
{
  double actual_amount = player_t::resource_loss( resource, amount, action );

  if ( resource == RESOURCE_HEALTH )
  {
    // FIXME!!! Only works for level 80 currently!
    double coeff_60 = 453.3;
    double coeff_70 = 453.3;
    double coeff_80 = 453.3;
    double coeff_85 = 453.3; // FIXME

    double coeff = rating_t::interpolate( level, coeff_60, coeff_70, coeff_80, coeff_85 );

    double rage_gain = 5 * amount / ( 2 * coeff );

    resource_gain( RESOURCE_RAGE, rage_gain, gains_incoming_damage );
  }

  return actual_amount;
}

// warrior_t::primary_role() ================================================

int warrior_t::primary_role() SC_CONST
{
  if ( player_t::primary_role() == ROLE_TANK || primary_tree() == TREE_PROTECTION )
    return ROLE_TANK;

  return ROLE_ATTACK;
}

// warrior_t::assess_damage ==================================================

double warrior_t::assess_damage( double            amount,
				 const school_type school,
				 int               dmg_type,
				 int               result,
				 action_t*         action )
{
  if ( result == RESULT_HIT    ||
       result == RESULT_CRIT   ||
       result == RESULT_GLANCE ||
       result == RESULT_BLOCK  )
  {
    resource_gain( RESOURCE_RAGE, 100.0, gains_incoming_damage );  // FIXME! Assume it caps rage every time.
  }
  if ( result == RESULT_BLOCK )
  {
    if ( talents.shield_specialization -> ok() )
    {
      resource_gain( RESOURCE_RAGE, 5.0 * talents.shield_specialization -> rank(), gains_shield_specialization );
    }
  }
  if ( result == RESULT_BLOCK || 
       result == RESULT_DODGE ||
       result == RESULT_PARRY )
  {
       buffs_bastion_of_defense -> trigger();
  }
  if ( result == RESULT_PARRY )
  {
    buffs_hold_the_line -> trigger();

    if ( main_hand_attack && main_hand_attack -> execute_event )
    {
      double swing_time = main_hand_attack -> time_to_execute;
      double max_reschedule = ( main_hand_attack -> execute_event -> occurs() - 0.20 * swing_time ) - sim -> current_time;

      if ( max_reschedule > 0 )
      {
        main_hand_attack -> reschedule_execute( std::min( ( 0.40 * swing_time ), max_reschedule ) );
        procs_parry_haste -> occur();
      }
    }
  }

  return player_t::assess_damage( amount, school, dmg_type, result, action );
}

// warrior_t::create_options ================================================

void warrior_t::create_options()
{
  player_t::create_options();

  option_t warrior_options[] =
  {
    { "initial_rage",            OPT_INT,  &initial_rage            },
    { "instant_flurry_haste",    OPT_BOOL, &instant_flurry_haste    },
    { "max_deep_wounds_refresh", OPT_FLT,  &max_deep_wounds_refresh },
    { NULL, OPT_UNKNOWN, NULL }
  };

  option_t::copy( options, warrior_options );
}

// warrior_t::copy_from ===================================================

void warrior_t::copy_from( player_t* source )
{
  player_t::copy_from( source );
  warrior_t* p = source -> cast_warrior();
  initial_rage            = p -> initial_rage;
  instant_flurry_haste    = p -> instant_flurry_haste;
  max_deep_wounds_refresh = p -> max_deep_wounds_refresh;
}

// warrior_t::decode_set ====================================================

int warrior_t::decode_set( item_t& item )
{
  if ( item.slot != SLOT_HEAD      &&
       item.slot != SLOT_SHOULDERS &&
       item.slot != SLOT_CHEST     &&
       item.slot != SLOT_HANDS     &&
       item.slot != SLOT_LEGS      )
  {
    return SET_NONE;
  }

  const char* s = item.name();

  bool is_melee = ( strstr( s, "helmet"         ) ||
                    strstr( s, "shoulderplates" ) ||
                    strstr( s, "battleplate"    ) ||
                    strstr( s, "legplates"      ) ||
                    strstr( s, "gauntlets"      ) );

  bool is_tank = ( strstr( s, "greathelm"   ) ||
                   strstr( s, "pauldrons"   ) ||
                   strstr( s, "breastplate" ) ||
                   strstr( s, "legguards"   ) ||
                   strstr( s, "handguards"  ) );

  if ( strstr( s, "ymirjar" ) )
  {
    if ( is_melee ) return SET_T10_MELEE;
    if ( is_tank  ) return SET_T10_TANK;
  }
  if ( strstr( s, "earthen" ) )
  {
    bool is_melee = ( strstr( s, "helmet"        ) ||
                      strstr( s, "pauldrons"     ) ||
                      strstr( s, "battleplate"   ) ||
                      strstr( s, "legplates"     ) ||
                      strstr( s, "gauntlets"     ) );

    bool is_tank = ( strstr( s, "faceguard"      ) ||
                     strstr( s, "shoulderguards" ) ||
                     strstr( s, "chestguard"     ) ||
                     strstr( s, "legguards"      ) ||
                     strstr( s, "handguards"     ) );

    if ( is_melee ) return SET_T11_MELEE;
    if ( is_tank  ) return SET_T11_TANK;
  }

  return SET_NONE;
}

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

// player_t::create_warrior =================================================

player_t* player_t::create_warrior( sim_t* sim, const std::string& name, race_type r )
{
  return new warrior_t( sim, name, r );
}

// warrior_init =============================================================

void player_t::warrior_init( sim_t* sim )
{
  sim -> auras.battle_shout = new aura_t( sim, "battle_shout", 1, 120.0 );
  sim -> auras.rampage      = new aura_t( sim, "rampage",      1, 0.0 );

  for ( unsigned int i = 0; i < sim -> actor_list.size(); i++ )
  {
    player_t* p = sim -> actor_list[i];
    p -> debuffs.blood_frenzy_bleed    = new debuff_t( p, "blood_frenzy_bleed",    1, 60.0 );
    p -> debuffs.blood_frenzy_physical = new debuff_t( p, "blood_frenzy_physical", 1, 60.0 );
    p -> debuffs.demoralizing_shout    = new debuff_t( p, "demoralizing_shout",    1, 30.0 );
    p -> debuffs.shattering_throw      = new debuff_t( p, "shattering_throw",      1 );
    p -> debuffs.sunder_armor          = new debuff_t( p, "sunder_armor",          1, 30.0 );
    p -> debuffs.thunder_clap          = new debuff_t( p, "thunder_clap",          1, 30.0 );
  }
}

// player_t::warrior_combat_begin ===========================================

void player_t::warrior_combat_begin( sim_t* sim )
{
  if ( sim -> overrides.battle_shout ) 
    sim -> auras.battle_shout -> override( 1, sim -> sim_data.effect_min( 6673, sim -> max_player_level, E_APPLY_AURA, A_MOD_STAT ) );

  if ( sim -> overrides.rampage      ) sim -> auras.rampage      -> override();

  for ( target_t* t = sim -> target_list; t; t = t -> next )
  {
    if ( sim -> overrides.blood_frenzy_bleed    ) t -> debuffs.blood_frenzy_bleed    -> override( 1, 30 );
    if ( sim -> overrides.blood_frenzy_physical ) t -> debuffs.blood_frenzy_physical -> override( 1,  4 );
    if ( sim -> overrides.demoralizing_shout    ) t -> debuffs.demoralizing_shout    -> override();
    if ( sim -> overrides.sunder_armor          ) t -> debuffs.sunder_armor          -> override( 3 );
    if ( sim -> overrides.thunder_clap          ) t -> debuffs.thunder_clap          -> override();
  }
}

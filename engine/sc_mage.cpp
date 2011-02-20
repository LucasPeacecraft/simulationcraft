// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

// ==========================================================================
// Mage
// ==========================================================================

enum { ROTATION_NONE=0, ROTATION_DPS, ROTATION_DPM, ROTATION_MAX };

struct mage_t : public player_t
{
  // Active
  spell_t* active_ignite;
  pet_t*   active_water_elemental;

  // Dots
  dot_t* dots_frostfire_bolt;
  dot_t* dots_ignite;
  dot_t* dots_living_bomb;
  dot_t* dots_pyroblast;

  // Buffs
  buff_t* buffs_arcane_blast;
  buff_t* buffs_arcane_missiles;
  buff_t* buffs_arcane_potency;
  buff_t* buffs_arcane_power;
  buff_t* buffs_brain_freeze;
  buff_t* buffs_clearcasting;
  buff_t* buffs_fingers_of_frost;
  buff_t* buffs_focus_magic_feedback;
  buff_t* buffs_hot_streak;
  buff_t* buffs_hot_streak_crits;
  buff_t* buffs_icy_veins;
  buff_t* buffs_improved_mana_gem;
  buff_t* buffs_incanters_absorption;
  buff_t* buffs_invocation;
  buff_t* buffs_mage_armor;
  buff_t* buffs_missile_barrage;
  buff_t* buffs_molten_armor;
  buff_t* buffs_tier10_2pc;
  buff_t* buffs_tier10_4pc;

  // Cooldowns
  cooldown_t* cooldowns_deep_freeze;
  cooldown_t* cooldowns_fire_blast;
  cooldown_t* cooldowns_early_frost;

  // Gains
  gain_t* gains_clearcasting;
  gain_t* gains_empowered_fire;
  gain_t* gains_evocation;
  gain_t* gains_mage_armor;
  gain_t* gains_mana_gem;
  gain_t* gains_master_of_elements;

  // Glyphs
  struct glyphs_t
  {
    // Prime
    glyph_t* arcane_barrage;
    glyph_t* arcane_blast;
    glyph_t* arcane_missiles;
    glyph_t* cone_of_cold;
    glyph_t* deep_freeze;
    glyph_t* fireball;
    glyph_t* frostbolt;
    glyph_t* frostfire;
    glyph_t* ice_lance;
    glyph_t* living_bomb;
    glyph_t* mage_armor;
    glyph_t* molten_armor;
    glyph_t* pyroblast;

    // Major
    glyph_t* dragons_breath;
    glyph_t* mirror_image;

    // Minor
    glyph_t* arcane_brilliance;
  };
  glyphs_t glyphs;

  // Options
  std::string focus_magic_target_str;
  double max_ignite_refresh;

  // Spell Data
  struct spells_t
  {
    spell_data_t* arcane_blast;
    spell_data_t* arcane_missiles;
    spell_data_t* hot_streak;
    spell_data_t* mage_armor;
    spell_data_t* molten_armor;

    spell_data_t* flashburn;
    spell_data_t* frostburn;
    spell_data_t* mana_adept;

    spell_data_t* arcane_specialization;
    spell_data_t* fire_specialization;
    spell_data_t* frost_specialization;
  };
  spells_t spells;

  // Specializations
  struct specializations_t
  {
    double arcane;
    double fire;
    double frost1;
    double frost2;
    double frost3;
    double flashburn;
    double frostburn;
    double mana_adept;
  };
  specializations_t specializations;

  // Procs
  proc_t* procs_deferred_ignite;
  proc_t* procs_munched_ignite;
  proc_t* procs_rolled_ignite;
  proc_t* procs_mana_gem;
  proc_t* procs_early_frost;

  // Random Number Generation
  rng_t* rng_arcane_missiles;
  rng_t* rng_empowered_fire;
  rng_t* rng_enduring_winter;
  rng_t* rng_fire_power;
  rng_t* rng_impact;
  rng_t* rng_improved_freeze;
  rng_t* rng_nether_vortex;

  // Rotation (DPS vs DPM)
  struct rotation_t
  {
    int    current;
    double mana_gain;
    double dps_mana_loss;
    double dpm_mana_loss;
    double dps_time;
    double dpm_time;

    void reset() { memset( ( void* ) this, 0x00, sizeof( rotation_t ) ); current = ROTATION_DPS; }
    rotation_t() { reset(); }
  };
  rotation_t rotation;

  // Talents
  struct talents_list_t
  {
    // Arcane
    talent_t* arcane_concentration;
    talent_t* arcane_flows;
    talent_t* arcane_potency;
    talent_t* arcane_power;
    talent_t* arcane_tactics;
    talent_t* focus_magic;
    talent_t* improved_arcane_explosion;
    talent_t* improved_arcane_missiles;
    talent_t* improved_counterspell;
    talent_t* improved_mana_gem;
    talent_t* invocation;
    talent_t* missile_barrage;
    talent_t* nether_vortex;
    talent_t* netherwind_presence;
    talent_t* presence_of_mind;
    talent_t* slow;
    talent_t* torment_the_weak;

    // Fire
    talent_t* blast_wave;
    talent_t* combustion;
    talent_t* critical_mass;
    talent_t* dragons_breath;
    talent_t* fire_power;
    talent_t* firestarter;
    talent_t* hot_streak;
    talent_t* ignite;
    talent_t* impact;
    talent_t* improved_fire_blast;
    talent_t* improved_flamestrike;
    talent_t* improved_hot_streak;
    talent_t* improved_scorch;
    talent_t* living_bomb;
    talent_t* master_of_elements;
    talent_t* molten_fury;

    // Frost
    talent_t* brain_freeze;
    talent_t* cold_snap;
    talent_t* deep_freeze;
    talent_t* early_frost;
    talent_t* enduring_winter;
    talent_t* fingers_of_frost;
    talent_t* frostfire_orb;
    talent_t* ice_barrier;
    talent_t* ice_floes;
    talent_t* icy_veins;
    talent_t* improved_freeze;
    talent_t* piercing_chill;
    talent_t* piercing_ice;
    talent_t* shatter;

    talents_list_t() { memset( ( void* ) this, 0x0, sizeof( talents_list_t ) ); }
  };
  talents_list_t talents;

  // Up-Times
  uptime_t* uptimes_arcane_blast[ 5 ];
  uptime_t* uptimes_dps_rotation;
  uptime_t* uptimes_dpm_rotation;
  uptime_t* uptimes_water_elemental;

  int mana_gem_charges;

  mage_t( sim_t* sim, const std::string& name, race_type r = RACE_NONE ) : player_t( sim, MAGE, name, r )
  {
    if( race == RACE_NONE ) race = RACE_UNDEAD;

    tree_type[ MAGE_ARCANE ] = TREE_ARCANE;
    tree_type[ MAGE_FIRE   ] = TREE_FIRE;
    tree_type[ MAGE_FROST  ] = TREE_FROST;

    // Active
    active_ignite          = 0;
    active_water_elemental = 0;

    // Dots
    dots_frostfire_bolt = get_dot( "frostfire_bolt" );
    dots_ignite         = get_dot( "ignite"         );
    dots_living_bomb    = get_dot( "living_bomb"    );
    dots_pyroblast      = get_dot( "pyroblast"      );

    // Cooldowns
    cooldowns_deep_freeze = get_cooldown( "deep_freeze" );
    cooldowns_fire_blast  = get_cooldown( "fire_blast"  );
    cooldowns_early_frost = get_cooldown( "early_frost" );

    distance = 40;
    mana_gem_charges =  3;
    max_ignite_refresh = 4.0;

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
  virtual void      reset();
  virtual action_expr_t* create_expression( action_t*, const std::string& name );
  virtual void      create_options();
  virtual bool      create_profile( std::string& profile_str, int save_type=SAVE_ALL, bool save_html=false );
  virtual action_t* create_action( const std::string& name, const std::string& options );
  virtual pet_t*    create_pet   ( const std::string& name, const std::string& type = std::string() );
  virtual void      create_pets();
  virtual void      copy_from( player_t* source );
  virtual int       decode_set( item_t& item );
  virtual int       primary_resource() SC_CONST { return RESOURCE_MANA; }
  virtual int       primary_role() SC_CONST     { return ROLE_SPELL; }
  virtual double    composite_mastery() SC_CONST;
  virtual double    composite_spell_power( const school_type school ) SC_CONST;
  virtual double    composite_spell_crit() SC_CONST;
  virtual double    matching_gear_multiplier( const attribute_type attr ) SC_CONST;

  // Event Tracking
  virtual void   regen( double periodicity );
  virtual double resource_gain( int resource, double amount, gain_t* source=0, action_t* action=0 );
  virtual double resource_loss( int resource, double amount, action_t* action=0 );
};

namespace { // ANONYMOUS NAMESPACE ==========================================

// ==========================================================================
// Mage Spell
// ==========================================================================

struct mage_spell_t : public spell_t
{
  bool may_chill, may_hot_streak, may_brain_freeze;
  bool fof_frozen, consumes_arcane_blast;
  int dps_rotation;
  int dpm_rotation;

  void _init_mage_spell_t()
  {
    may_crit      = ( base_dd_min > 0 ) && ( base_dd_max > 0 );
    tick_may_crit = true;
    base_crit_multiplier = 1.33;
    may_chill = false;
    may_hot_streak = false;
    may_brain_freeze = false;
    fof_frozen = false;
    consumes_arcane_blast = false;
    dps_rotation = 0;
    dpm_rotation = 0;
    mage_t* p = player -> cast_mage();
    if ( p -> talents.piercing_ice -> rank() )
    {
      base_crit += p -> talents.piercing_ice -> sd->effect1->base_value/100.0;
    }
    if ( p -> talents.enduring_winter -> rank() )
    {
      base_cost *= 1.0 + p -> talents.enduring_winter -> sd->effect1->base_value/100.0;
    }
  }

  mage_spell_t( const char* n, player_t* player, const school_type s, int t ) :
      spell_t( n, player, RESOURCE_MANA, s, t )
  {
    _init_mage_spell_t();
  }

  mage_spell_t( const char* n, uint32_t id, player_t* player ) :
    spell_t( n, id, player )
  {
    _init_mage_spell_t();
  }

  mage_spell_t( const char* n, const char* sname, player_t* player ) :
    spell_t( n, sname, player )
  {
    _init_mage_spell_t();
  }

  virtual void   parse_options( option_t*, const std::string& );
  virtual bool   ready();
  virtual double cost() SC_CONST;
  virtual double haste() SC_CONST;
  virtual void   execute();
  virtual void   travel( player_t* t, int travel_result, double travel_dmg );
  virtual void   tick();
  virtual void   consume_resource();
  virtual void   player_buff();
  virtual void   target_debuff( player_t* t, int dmg_type );
  virtual double hot_streak_crit() { return base_crit + player_crit; }
};

// ==========================================================================
// Pet Water Elemental
// ==========================================================================

struct water_elemental_pet_t : public pet_t
{
  struct freeze_t : public spell_t
  {
    freeze_t( player_t* player):
      spell_t( "freeze", 33395, player )
    {
      aoe                  = -1;
      may_crit             = true;
      base_crit_multiplier = 1.33;
      base_cost            = 0;
      if ( player -> cast_pet() -> owner -> cast_mage() -> race == RACE_ORC )
        base_multiplier *= 1.05;
    }

    virtual void player_buff()
    {
      spell_t::player_buff();
      player_spell_power = player -> cast_pet() -> owner -> composite_spell_power( SCHOOL_FROST ) * 0.4;
      player_crit = player -> cast_pet() -> owner -> composite_spell_crit(); // Needs testing, but closer than before
    }

    virtual void execute()
    {
      spell_t::execute();
      mage_t* o = player -> cast_pet() -> owner -> cast_mage();

      o -> buffs_fingers_of_frost -> trigger( 2, 1, o -> talents.improved_freeze -> effect_base_value( 1 ) / 100.0 );
    }

    virtual bool ready()
    {
      mage_t* o = player -> cast_pet() -> owner -> cast_mage();

      if ( ! o -> talents.improved_freeze -> rank() )
        return false;

      if ( o -> buffs_fingers_of_frost -> stack() ) // || o -> cooldowns_deep_freeze -> remains() )
      {
        return false;
      }

      return spell_t::ready();
    }
  };

  struct water_bolt_t : public spell_t
  {
    water_bolt_t( player_t* player ):
      spell_t( "water_bolt", 31707, player )
    {
      may_crit             = true;
      base_crit_multiplier = 1.33;
      direct_power_mod     = 0.833;
      if ( player -> cast_pet() -> owner -> cast_mage() -> race == RACE_ORC )
        base_multiplier *= 1.05;
    }

    virtual void player_buff()
    {
      spell_t::player_buff();
      player_spell_power = player -> cast_pet() -> owner -> composite_spell_power( SCHOOL_FROST ) * 0.4;
      player_crit = player -> cast_pet() -> owner -> composite_spell_crit(); // Needs testing, but closer than before
    }
  };

  water_elemental_pet_t( sim_t* sim, player_t* owner ) :
      pet_t( sim, owner, "water_elemental" )
  {
    action_list_str = "freeze/water_bolt";
  }

  virtual void init_base()
  {
    pet_t::init_base();

    // Stolen from Priest's Shadowfiend
    attribute_base[ ATTR_STRENGTH  ] = 145;
    attribute_base[ ATTR_AGILITY   ] =  38;
    attribute_base[ ATTR_STAMINA   ] = 190;
    attribute_base[ ATTR_INTELLECT ] = 133;

    health_per_stamina = 7.5;
    mana_per_intellect = 5;
  }

  virtual double composite_spell_haste() SC_CONST
  {
    double h = player_t::composite_spell_haste();
    h *= owner -> spell_haste;
    return h;
  }

  virtual void summon( double duration=0 )
  {
    pet_t::summon( duration );
    mage_t* o = cast_pet() -> owner -> cast_mage();
    o -> active_water_elemental = this;
  }

  virtual void dismiss()
  {
    pet_t::dismiss();
    mage_t* o = cast_pet() -> owner -> cast_mage();
    o -> active_water_elemental = 0;
  }

  virtual action_t* create_action( const std::string& name,
                                   const std::string& options_str )
  {
    if ( name == "freeze"     ) return new     freeze_t( this );
    if ( name == "water_bolt" ) return new water_bolt_t( this );

    return pet_t::create_action( name, options_str );
  }
};

// ==========================================================================
// Pet Mirror Image
// ==========================================================================

struct mirror_image_pet_t : public pet_t
{
  int num_images;
  int num_rotations;
  std::vector<action_t*> sequences;
  int sequence_finished;
  double snapshot_arcane_sp;
  double snapshot_fire_sp;
  double snapshot_frost_sp;

  mirror_image_pet_t( sim_t* sim, player_t* owner ) :
    pet_t( sim, owner, "mirror_image_3", true /*guardian*/ ),
    num_images( 3 ), num_rotations( 2 ), sequence_finished( 0 )
  {}

  struct arcane_blast_t : public spell_t
  {
    action_t* next_in_sequence;

    arcane_blast_t( mirror_image_pet_t* mirror_image, action_t* nis ):
      spell_t( "mirror_arcane_blast", 88084, mirror_image ), next_in_sequence( nis )
    {
      may_crit          = true;
      background        = true;
    }

    virtual void player_buff()
    {
      spell_t::player_buff();
      mage_t* o = player -> cast_pet() -> owner -> cast_mage();
      double ab_stack_multiplier = o -> spells.arcane_blast -> effect1 -> base_value / 100.0;
      double ab_glyph_multiplier = o -> glyphs.arcane_blast -> effect_base_value( 1 ) / 100.0;
      player_multiplier *= 1.0 + o ->  buffs_arcane_blast -> stack() * ( ab_stack_multiplier + ab_glyph_multiplier );
    }

    virtual void execute()
    {
      spell_t::execute();
      if ( next_in_sequence )
      {
        next_in_sequence -> schedule_execute();
      }
      else
      {
        mirror_image_pet_t* mi = ( mirror_image_pet_t* ) player;
        mi -> sequence_finished++;
        if ( mi -> sequence_finished == mi -> num_images ) mi -> dismiss();
      }
    }
  };

  struct fire_blast_t : public spell_t
  {
    action_t* next_in_sequence;

    fire_blast_t( mirror_image_pet_t* mirror_image, action_t* nis ):
      spell_t( "mirror_fire_blast", 59637, mirror_image ), next_in_sequence( nis )
    {
      background        = true;
      may_crit          = true;
    }

    virtual void execute()
    {
      spell_t::execute();
      if ( next_in_sequence )
      {
        next_in_sequence -> schedule_execute();
      }
      else
      {
        mirror_image_pet_t* mi = ( mirror_image_pet_t* ) player;
        mi -> sequence_finished++;
        if ( mi -> sequence_finished == mi -> num_images ) mi -> dismiss();
      }
    }
  };

  struct fireball_t : public spell_t
  {
    action_t* next_in_sequence;

    fireball_t( mirror_image_pet_t* mirror_image, action_t* nis ):
      spell_t( "mirror_fireball", 88082, mirror_image ), next_in_sequence( nis )
    {
      may_crit          = true;
      background        = true;
    }

    virtual void execute()
    {
      spell_t::execute();
      if ( next_in_sequence )
      {
        next_in_sequence -> schedule_execute();
      }
      else
      {
        mirror_image_pet_t* mi = ( mirror_image_pet_t* ) player;
        mi -> sequence_finished++;
        if ( mi -> sequence_finished == mi -> num_images ) mi -> dismiss();
      }
    }
  };

  struct frostbolt_t : public spell_t
  {
    action_t* next_in_sequence;

    frostbolt_t( mirror_image_pet_t* mirror_image, action_t* nis ):
      spell_t( "mirror_frost_bolt", 59638, mirror_image ), next_in_sequence( nis )
    {
      may_crit          = true;
      background        = true;
    }

    virtual void execute()
    {
      spell_t::execute();
      if ( next_in_sequence )
      {
        next_in_sequence -> schedule_execute();
      }
      else
      {
        mirror_image_pet_t* mi = ( mirror_image_pet_t* ) player;
        mi -> sequence_finished++;
        if ( mi -> sequence_finished == mi -> num_images ) mi -> dismiss();
      }
    }
  };

  virtual void init_base()
  {
    pet_t::init_base();

    // Stolen from Priest's Shadowfiend
    attribute_base[ ATTR_STRENGTH  ] = 145;
    attribute_base[ ATTR_AGILITY   ] =  38;
    attribute_base[ ATTR_STAMINA   ] = 190;
    attribute_base[ ATTR_INTELLECT ] = 133;

    health_per_stamina = 7.5;
    mana_per_intellect = 5;
  }

  virtual void init_actions()
  {
    for ( int i=0; i < num_images; i++ )
    {
      action_t* front=0;

      if ( owner -> cast_mage() -> glyphs.mirror_image -> ok() && owner -> cast_mage() -> primary_tree() != TREE_FROST )
      {
        // Fire/Arcane Mages cast 9 Fireballs/Arcane Blasts
        num_rotations = 9;
        for ( int j=0; j < num_rotations; j++ )
        {
          if ( owner -> cast_mage() -> primary_tree() == TREE_FIRE )
          {
            front = new fireball_t ( this, front );
          }
          else
          {
            front = new arcane_blast_t ( this, front );
          }
        }
      }
      else
      {
        num_rotations = 2;
        for ( int j=0; j < num_rotations; j++ )
        {
          // Mirror Image casts 10 Frostbolts, 4 Fire Blasts
          front = new frostbolt_t ( this, front );
          front = new frostbolt_t ( this, front );
          front = new frostbolt_t ( this, front );
          front = new fire_blast_t( this, front );
          front = new frostbolt_t ( this, front );
          front = new frostbolt_t ( this, front );
          front = new fire_blast_t( this, front );
        }
      }
      sequences.push_back( front );
    }

    pet_t::init_actions();
  }

  virtual double composite_spell_power( const school_type school ) SC_CONST
  {
    if( school == SCHOOL_ARCANE )
    {
      return snapshot_arcane_sp * 0.75;
    }
    else if ( school == SCHOOL_FIRE )
    {
      return snapshot_fire_sp * 0.75;
    }
    else if ( school == SCHOOL_FROST )
    {
      return snapshot_frost_sp * 0.75;
    }

    return 0;
  }

  virtual void summon( double duration=0 )
  {
    pet_t::summon( duration );

    mage_t* o = owner -> cast_mage();

    snapshot_arcane_sp = o -> composite_spell_power( SCHOOL_ARCANE );
    snapshot_fire_sp   = o -> composite_spell_power( SCHOOL_FIRE   );
    snapshot_frost_sp  = o -> composite_spell_power( SCHOOL_FROST  );

    sequence_finished = 0;

    for ( int i=0; i < num_images; i++ )
    {
      sequences[ i ] -> schedule_execute();
    }
  }

  virtual void halt()
  {
    pet_t::halt();
    dismiss(); // FIXME! Interrupting them is too hard, just dismiss for now.
  }
};

// calculate_dot_dps ========================================================

static double calculate_dot_dps( dot_t* dot )
{
  if( ! dot -> ticking ) return 0;

  action_t* a = dot -> action;

  a -> result = RESULT_HIT;

  return ( a -> calculate_tick_damage() / a -> base_tick_time );
}

// consume_brain_freeze =====================================================

static void consume_brain_freeze( spell_t* s )
{
  mage_t* p = s -> player -> cast_mage();

  if ( p -> buffs_brain_freeze -> check() )
  {
    p -> buffs_brain_freeze -> expire();
    p -> buffs_tier10_2pc -> trigger();
  }
}

// trigger_hot_streak =======================================================

static void trigger_hot_streak( mage_spell_t* s )
{
  sim_t* sim = s -> sim;
  mage_t*  p = s -> player -> cast_mage();

  if( ! s -> may_hot_streak )
    return;

  if ( ! p -> talents.hot_streak -> rank() )
    return;

  int result = s -> result;

  if ( sim -> smooth_rng )
  {
    // Decouple Hot Streak proc from actual crit to reduce wild swings during RNG smoothing.
    result = sim -> rng -> roll( s -> total_crit() ) ? RESULT_CRIT : RESULT_HIT;
  }
  if ( result == RESULT_CRIT )
  {
    // Reference: http://elitistjerks.com/f75/t110326-cataclysm_fire_mage_compendium/p6/#post1831143

    double hot_streak_chance = -2.73 * s -> hot_streak_crit() + 0.95;

    if( hot_streak_chance > 0 && p -> buffs_hot_streak -> trigger( 1, 0, hot_streak_chance ) )
    {
      p -> buffs_hot_streak_crits -> expire();
    }
    else if( p -> talents.improved_hot_streak -> rank() )
    {
      p -> buffs_hot_streak_crits -> trigger();

      if ( p -> buffs_hot_streak_crits -> stack() == 2 )
      {
        p -> buffs_hot_streak_crits -> expire();

        hot_streak_chance = p -> talents.improved_hot_streak -> effect_base_value( 1 ) / 100.0;

        p -> buffs_hot_streak -> trigger( 1, 0, hot_streak_chance );
      }
    }
  }
  else
  {
    p -> buffs_hot_streak_crits -> expire();
  }
}

// trigger_ignite ===========================================================

static void trigger_ignite( spell_t* s, double dmg )
{
  if ( s -> school != SCHOOL_FIRE &&
       s -> school != SCHOOL_FROSTFIRE ) return;

  mage_t* p = s -> player -> cast_mage();
  sim_t* sim = s -> sim;

  if ( ! p -> talents.ignite -> rank() ) return;

  struct ignite_t : public mage_spell_t
  {
    ignite_t( player_t* player ) :
      mage_spell_t( "ignite", 12654, player )
    {
      background    = true;
      proc          = true;
      may_resist    = true;
      tick_may_crit = false;
      hasted_ticks  = false;
      dot_behavior  = DOT_REFRESH;
      reset();
    }
    virtual void travel( player_t* t, int travel_result, double ignite_dmg )
    {
      mage_spell_t::travel( t, travel_result, 0 );

      // FIXME: Is a is_hit check necessary here?
      base_td = ignite_dmg / dot -> num_ticks;
    }
    virtual double travel_time()
    {
      return sim -> gauss( sim -> aura_delay, 0.25 * sim -> aura_delay );
    }
    virtual double total_td_multiplier() SC_CONST { return 1.0; }
  };

  double ignite_dmg = dmg * p -> talents.ignite -> effect_base_value( 1 ) / 100.0;

  if ( p -> specialization == MAGE_FIRE )
  {
    ignite_dmg *= 1.0 + p -> specializations.flashburn * p -> composite_mastery();
  }

  if ( sim -> merge_ignite ) // Does not report Ignite seperately.
  {
    int result = s -> result;
    s -> result = RESULT_HIT;
    s -> assess_damage( s -> target, ignite_dmg, DMG_OVER_TIME, s -> result );
    s -> result = result;
    return;
  }

  if ( ! p -> active_ignite ) p -> active_ignite = new ignite_t( p );

  dot_t* dot = p -> active_ignite -> dot;

  if ( dot -> ticking )
  {
    ignite_dmg += p -> active_ignite -> base_td * dot -> ticks();
  }

  if ( p -> max_ignite_refresh > 0 )
  {
    if( (  p -> max_ignite_refresh + sim -> aura_delay ) < dot -> remains() )
    {
      if ( sim -> log ) log_t::output( sim, "Player %s munches Ignite due to Max Ignite Duration.", p -> name() );
      p -> procs_munched_ignite -> occur();
      return;
    }
  }

  if ( p -> active_ignite -> travel_event )
  {
    // There is an SPELL_AURA_APPLIED already in the queue, which will get munched.
    if ( sim -> log ) log_t::output( sim, "Player %s munches previous Ignite due to Aura Delay.", p -> name() );
    p -> procs_munched_ignite -> occur();
  }

  p -> active_ignite -> direct_dmg = ignite_dmg;
  p -> active_ignite -> result = RESULT_HIT;
  p -> active_ignite -> schedule_travel( s -> target );

  if ( p -> active_ignite -> travel_event && dot -> ticking )
  {
    if ( dot -> tick_event -> occurs() < p -> active_ignite -> travel_event -> occurs() )
    {
      // Ignite will tick before SPELL_AURA_APPLIED occurs, which means that the current Ignite will
      // both tick -and- get rolled into the next Ignite.
      if ( sim -> log ) log_t::output( sim, "Player %s rolls Ignite.", p -> name() );
      p -> procs_rolled_ignite -> occur();
    }
  }
}

// trigger_master_of_elements ===============================================

static void trigger_master_of_elements( spell_t* s )
{
  mage_t* p = s -> player -> cast_mage();

  if ( s -> base_cost == 0 )
    return;

  if ( ! p -> talents.master_of_elements -> rank() )
    return;

  p -> resource_gain( RESOURCE_MANA, s -> base_cost * p -> talents.master_of_elements -> effect_base_value( 1 ) / 100.0, p -> gains_master_of_elements );
}

// trigger_replenishment ====================================================

static void trigger_replenishment( spell_t* s )
{
  mage_t* p = s -> player -> cast_mage();

  if ( ! p -> talents.enduring_winter -> rank() )
    return;

  if ( ! p -> rng_enduring_winter -> roll( p -> talents.enduring_winter -> sd->proc_chance/100.0 ) )
    return;

  p -> trigger_replenishment();
}

// ==========================================================================
// Mage Spell
// ==========================================================================

// mage_spell_t::parse_options ==============================================

void mage_spell_t::parse_options( option_t*          options,
                                  const std::string& options_str )
{
  option_t base_options[] =
  {
    { "dps",          OPT_BOOL, &dps_rotation },
    { "dpm",          OPT_BOOL, &dpm_rotation },
    { NULL, OPT_UNKNOWN, NULL }
  };
  std::vector<option_t> merged_options;
  spell_t::parse_options( merge_options( merged_options, options, base_options ), options_str );
}

// mage_spell_t::ready ======================================================

bool mage_spell_t::ready()
{
  mage_t* p = player -> cast_mage();

  if ( dps_rotation )
    if ( p -> rotation.current != ROTATION_DPS )
      return false;

  if ( dpm_rotation )
    if ( p -> rotation.current != ROTATION_DPM )
      return false;

  return spell_t::ready();
}

// mage_spell_t::cost =======================================================

double mage_spell_t::cost() SC_CONST
{
  mage_t* p = player -> cast_mage();

  if ( p -> buffs_clearcasting -> check() )
    return 0;

  double c = spell_t::cost();

  if ( p -> buffs_arcane_power -> check() )
  {
    c *= 1.0 + p -> buffs_arcane_power -> effect_base_value( 2 ) / 100.0;
  }

  return c;
}

// mage_spell_t::haste ======================================================

double mage_spell_t::haste() SC_CONST
{
  mage_t* p = player -> cast_mage();
  double h = spell_t::haste();
  if ( p -> buffs_icy_veins -> up() )
  {
    h *= 1.0 / ( 1.0 + p -> buffs_icy_veins -> effect_base_value( 1 ) / 100.0 );
  }
  if ( p -> talents.netherwind_presence -> rank() )
  {
    h *= 1.0 / ( 1.0 + p -> talents.netherwind_presence -> effect_base_value( 1 ) / 100.0 );
  }
  if ( p -> buffs_tier10_2pc -> up() )
  {
    h *= 1.0 / 1.12;
  }
  return h;
}

// mage_spell_t::execute ====================================================

void mage_spell_t::execute()
{
  mage_t* p = player -> cast_mage();

  p -> uptimes_dps_rotation -> update( p -> rotation.current == ROTATION_DPS );
  p -> uptimes_dpm_rotation -> update( p -> rotation.current == ROTATION_DPM );

  spell_t::execute();

  if ( result == RESULT_CRIT )
  {
    trigger_master_of_elements( this );
  }

  if( background ) return;

  if( consumes_arcane_blast ) p -> buffs_arcane_blast -> expire();

  p -> buffs_arcane_potency -> decrement();

  if( fof_frozen )
  {
    p -> buffs_fingers_of_frost -> decrement();
  }

  if( may_brain_freeze )
  {
    p -> buffs_brain_freeze -> trigger();
  }

  if ( result_is_hit() )
  {
    if ( p -> rng_impact -> roll( p -> talents.impact -> proc_chance() ) )
    {
      p -> cooldowns_fire_blast -> reset();
    }
    if ( p -> buffs_clearcasting -> trigger() && p -> talents.arcane_potency -> rank() )
    {
      p -> buffs_arcane_potency -> trigger( 2 );
    }

    trigger_hot_streak( this );
  }

  if ( ! p -> talents.hot_streak   -> ok() &&
       ! p -> talents.brain_freeze -> ok() )
  {
    p -> buffs_arcane_missiles -> trigger();
  }
}

// mage_spell_t::travel =====================================================

void mage_spell_t::travel( player_t* t, int travel_result, double travel_dmg )
{
  mage_t* p = player -> cast_mage();

  spell_t::travel( t, travel_result, travel_dmg );

  if ( travel_result == RESULT_CRIT )
  {
    trigger_ignite( this, direct_dmg );
  }

  if( may_chill )
  {
    if( result_is_hit( travel_result ) )
    {
      p -> buffs_fingers_of_frost -> trigger();
    }
  }
}

// mage_spell_t::tick =======================================================

void mage_spell_t::tick()
{
  spell_t::tick();

  if ( result == RESULT_CRIT )
  {
    trigger_ignite( this, tick_dmg );
  }
}

// mage_spell_t::consume_resource ===========================================

void mage_spell_t::consume_resource()
{
  mage_t* p = player -> cast_mage();
  spell_t::consume_resource();
  if ( p -> buffs_clearcasting -> check() )
  {
    // Treat the savings like a mana gain.
    double amount = spell_t::cost();
    if ( amount > 0 )
    {
      p -> buffs_clearcasting -> expire();
      p -> gains_clearcasting -> add( amount );
    }
  }
}

// mage_spell_t::player_buff ================================================

void mage_spell_t::player_buff()
{
  mage_t* p = player -> cast_mage();

  spell_t::player_buff();

  if ( p -> talents.molten_fury -> rank() )
  {
    if ( target -> health_percentage() < 35 )
    {
      player_multiplier *= 1.0 + p -> talents.molten_fury -> effect_base_value( 1 ) / 100.0;
    }
  }
  if ( p -> buffs_tier10_4pc -> up() )
  {
    player_multiplier *= 1.18;
  }
  if ( p -> buffs_invocation -> up() )
  {
    player_multiplier *= 1.0 + p -> talents.invocation -> effect_base_value( 1 ) / 100.0;
  }
  if ( p -> buffs_arcane_power -> up() )
  {
    player_multiplier *= 1.0 + p -> buffs_arcane_power -> value();
  }
  if ( p -> buffs_arcane_potency -> up() )
  {
    player_crit += p -> talents.arcane_potency -> effect_base_value( 1 ) / 100.0;
  }

  double mana_pct = player -> resource_current[ RESOURCE_MANA ] / player -> resource_max [ RESOURCE_MANA ];
  player_multiplier *= 1.0 + mana_pct * p -> specializations.mana_adept * p -> composite_mastery();

  if ( school == SCHOOL_ARCANE )
  {
    if ( target -> debuffs.snared() && p -> talents.torment_the_weak -> rank() )
    {
      player_multiplier *= 1.0 + p -> talents.torment_the_weak -> effect_base_value( 1 ) / 100.0;
    }
    player_multiplier *= 1.0 + p -> specializations.arcane;
  }
  if ( school == SCHOOL_FIRE || school == SCHOOL_FROSTFIRE )
  {
    player_multiplier *= 1.0 + p -> specializations.fire;
    player_multiplier *= 1.0 + p -> talents.fire_power -> effect_base_value( 1 ) / 100.0;
  }
  if ( school == SCHOOL_FROST || school == SCHOOL_FROSTFIRE )
  {
    player_multiplier *= 1.0 + p -> specializations.frost1;
  }
  if ( fof_frozen && p -> buffs_fingers_of_frost -> up() )
  {
    player_multiplier *= 1.0 + p -> specializations.frostburn * p -> composite_mastery();

    double shatter = util_t::talent_rank( p -> talents.shatter -> rank(), 2, 1.0, 2.0 );

    if( shatter > 0 ) // Affects "player" and "base" crit only, not target_crit
    {
      player_crit += player_crit * shatter;
      player_crit +=   base_crit * shatter;
    }
  }

  if ( sim -> debug )
    log_t::output( sim, "mage_spell_t::player_buff: %s hit=%.2f crit=%.2f power=%.2f penetration=%.0f mult=%.2f",
                   name(), player_hit, player_crit, player_spell_power, player_penetration, player_multiplier );
}

// mage_spell_t::target_debuff ==============================================

void mage_spell_t::target_debuff( player_t* t, int dmg_type )
{
  spell_t::target_debuff( t, dmg_type );

  if ( school == SCHOOL_FIRE && dmg_type == DMG_OVER_TIME )
  {
    mage_t* p = player -> cast_mage();
    target_multiplier *= 1.0 + p -> specializations.flashburn * p -> composite_mastery();
  }
}

// ==========================================================================
// Mage Spells
// ==========================================================================

// Arcane Barrage Spell =====================================================

struct arcane_barrage_t : public mage_spell_t
{
  arcane_barrage_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "arcane_barrage", 44425, p )
  {
    check_spec( TREE_ARCANE );
    parse_options( NULL, options_str );
    base_multiplier *= 1.0 + p -> glyphs.arcane_barrage -> effect_base_value( 1 ) / 100.0;
    consumes_arcane_blast = true;
  }
};

// Arcane Blast Spell =======================================================

struct arcane_blast_t : public mage_spell_t
{
  arcane_blast_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "arcane_blast", 30451, p )
  {
    parse_options( NULL, options_str );
    if( p -> set_bonus.tier11_4pc_caster() ) base_execute_time *= 0.9;

    base_cost *= 0.05 / 0.07; // FIXME: Hotfixed value from: http://blue.mmo-champion.com/topic/158233/arcane-hotfixes
  }

  virtual double cost() SC_CONST
  {
    mage_t* p = player -> cast_mage();

    if ( p -> buffs_clearcasting -> check() )
      return 0;

    double c = spell_t::cost();

    if ( p -> buffs_arcane_blast -> check() )
    {
      c += c * p -> buffs_arcane_blast -> stack() * p -> spells.arcane_blast -> effect2 -> base_value / 100.0;
    }
    if ( p -> buffs_arcane_power -> check() )
    {
      c *= 1.0 + p -> buffs_arcane_power -> effect_base_value( 2 ) / 100.0;
    }
    return c;
  }

  virtual void execute()
  {
    mage_t* p = player -> cast_mage();
    for ( int i=0; i < 5; i++ )
    {
      p -> uptimes_arcane_blast[ i ] -> update( i == p -> buffs_arcane_blast -> stack() );
    }
    mage_spell_t::execute();
    p -> buffs_arcane_blast -> trigger();
    if ( ! target -> debuffs.snared() )
    {
      if ( p -> rng_nether_vortex -> roll( p -> talents.nether_vortex -> proc_chance() ) )
      {
        target -> debuffs.slow -> trigger();
        target -> debuffs.slow -> source = p;
      }
    }
  }

  virtual double execute_time() SC_CONST
  {
    mage_t* p = player -> cast_mage();
    double t = mage_spell_t::execute_time();
    t += p -> buffs_arcane_blast -> stack() * p -> spells.arcane_blast -> effect3 -> base_value / 1000.0;
    return t;
  }

  virtual void player_buff()
  {
    mage_t* p = player -> cast_mage();
    mage_spell_t::player_buff();
    double ab_stack_multiplier = p -> spells.arcane_blast -> effect1 -> base_value / 100.0;
    double ab_glyph_multiplier = p -> glyphs.arcane_blast -> effect_base_value( 1 ) / 100.0;
    player_multiplier *= 1.0 + p ->  buffs_arcane_blast -> stack() * ( ab_stack_multiplier + ab_glyph_multiplier );
  }
};

// Arcane Brilliance Spell =================================================

struct arcane_brilliance_t : public mage_spell_t
{
  double bonus;

  arcane_brilliance_t( mage_t* p, const std::string& options_str ) :
      mage_spell_t( "arcane_brilliance", 1459, p ), bonus( 0 )
  {
    bonus      = p -> player_data.effect_min( 79058, p -> level, E_APPLY_AURA, A_MOD_INCREASE_ENERGY );
    base_cost *= 1.0 + p -> glyphs.arcane_brilliance -> effect_base_value( 1 ) / 100.0;
  }

  virtual void execute()
  {
    if ( sim -> log ) log_t::output( sim, "%s performs %s", player -> name(), name() );

    for ( player_t* p = sim -> player_list; p; p = p -> next )
    {
      if ( p -> ooc_buffs() )
      {
        p -> buffs.arcane_brilliance -> trigger( 1, bonus );
      }
    }
  }

  virtual bool ready()
  {
    return( player -> buffs.arcane_brilliance -> current_value < bonus );
  }
};

// Arcane Explosion Spell ===================================================

struct arcane_explosion_t : public mage_spell_t
{
  arcane_explosion_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "arcane_explosion", 1449, p )
  {
    check_spec( TREE_ARCANE );
    parse_options( NULL, options_str );
    aoe = -1;
    consumes_arcane_blast = true;

    if ( p -> talents.improved_arcane_explosion -> rank() )
    {
      trigger_gcd += p -> talents.improved_arcane_explosion -> effect_base_value( 1 ) / 1000.0;
    }
  }

  virtual void execute()
  {
    mage_t* p = player -> cast_mage();
    mage_spell_t::execute();
    p -> buffs_arcane_blast -> expire();
  }
};

// Arcane Missiles Spell ====================================================

struct arcane_missiles_tick_t : public mage_spell_t
{
  arcane_missiles_tick_t( mage_t* p ) :
    mage_spell_t( "arcane_missiles_tick", 7268, p )
  {
    dual        = true;
    background  = true;
    direct_tick = true;
    base_crit  += p -> glyphs.arcane_missiles -> effect_base_value( 1 ) / 100.0;
    base_crit  += p -> set_bonus.tier11_2pc_caster() * 0.05;
    stats = player -> get_stats( "arcane_missiles" );
  }
};

struct arcane_missiles_t : public mage_spell_t
{
  arcane_missiles_tick_t* tick_spell;

  arcane_missiles_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "arcane_missiles", 5143, p )
  {
    parse_options( NULL, options_str );
    channeled = true;
    may_miss = may_resist = false;
    num_ticks += p -> talents.improved_arcane_missiles -> rank();
    hasted_ticks = false;

    base_tick_time += p -> talents.missile_barrage -> mod_additive( P_TICK_TIME );

    tick_spell = new arcane_missiles_tick_t( p );
  }

  virtual void execute()
  {
    mage_t* p = player -> cast_mage();
    mage_spell_t::execute();
    p -> buffs_arcane_missiles -> expire();
  }

  virtual void last_tick()
  {
    mage_t* p = player -> cast_mage();
    mage_spell_t::last_tick();
    p -> buffs_arcane_blast -> expire();
    p -> buffs_tier10_2pc -> trigger();
  }

  virtual void tick()
  {
    tick_spell -> execute();
    stats -> add_tick( time_to_tick );
  }

  virtual bool ready()
  {
    mage_t* p = player -> cast_mage();
    if ( ! p -> buffs_arcane_missiles -> may_react() )
      return false;
    return mage_spell_t::ready();
  }
};

// Arcane Power Spell =======================================================

struct arcane_power_t : public mage_spell_t
{
  arcane_power_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "arcane_power", 12042, p )
  {
    check_talent( p -> talents.arcane_power -> rank() );
    parse_options( NULL, options_str );
    harmful = false;
    cooldown -> duration *= 1.0 + p -> talents.arcane_flows -> effect_base_value( 1 ) / 100.0;
  }

  virtual void execute()
  {
    mage_t* p = player -> cast_mage();
    mage_spell_t::execute();
    p -> buffs_arcane_power -> trigger( 1, effect_base_value( 1 ) / 100.0 );
  }
};

// Blast Wave Spell =========================================================

struct blast_wave_t : public mage_spell_t
{
  blast_wave_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "blast_wave", 11113, p )
  {
    parse_options( NULL, options_str );
    aoe = -1;
  }
};

// Cold Snap Spell ==========================================================

struct cold_snap_t : public mage_spell_t
{
  std::vector<cooldown_t*> cooldown_list;

  cold_snap_t( mage_t* p, const std::string& options_str ) :
      mage_spell_t( "cold_snap", 11958, p )
  {
    check_talent( p -> talents.cold_snap -> rank() );
    parse_options( NULL, options_str );

    harmful = false;

    cooldown -> duration *= 1.0 + p -> talents.ice_floes -> effect_base_value( 1 ) / 100.0;

    cooldown_list.push_back( p -> get_cooldown( "cone_of_cold"  ) );
    cooldown_list.push_back( p -> get_cooldown( "deep_freeze"   ) );
    cooldown_list.push_back( p -> get_cooldown( "frostfire_orb" ) );
    cooldown_list.push_back( p -> get_cooldown( "icy_veins"     ) );
  }

  virtual void execute()
  {
    mage_spell_t::execute();

    int num_cooldowns = (int) cooldown_list.size();
    for ( int i=0; i < num_cooldowns; i++ )
    {
      cooldown_list[ i ] -> reset();
    }
  }
};

// Combustion Spell =========================================================

struct combustion_t : public mage_spell_t
{
  combustion_t( mage_t* p, const std::string& options_str ) :
      mage_spell_t( "combustion", 11129, p )
  {
    check_talent( p -> talents.combustion -> rank() );
    parse_options( NULL, options_str );

    // The "tick" portion of spell is specified in the DBC data in an alternate version of Combustion
    num_ticks      = 10;
    base_tick_time = 1.0;
  }

  virtual void execute()
  {
    mage_t* p = player -> cast_mage();
    base_td = 0;
    base_td += calculate_dot_dps( p -> dots_frostfire_bolt );
    base_td += calculate_dot_dps( p -> dots_ignite         );
    base_td += calculate_dot_dps( p -> dots_living_bomb    );
    base_td += calculate_dot_dps( p -> dots_pyroblast      );
    mage_spell_t::execute();
  }

  virtual double total_td_multiplier() SC_CONST { return 1.0; } // No double-dipping!
};

// Cone of Cold Spell =======================================================

struct cone_of_cold_t : public mage_spell_t
{
  cone_of_cold_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "cone_of_cold", 120, p )
  {
    parse_options( NULL, options_str );
    aoe = -1;
    base_multiplier *= 1.0 + p -> glyphs.cone_of_cold -> effect_base_value( 1 ) / 100.0;
    cooldown -> duration *= 1.0 + p -> talents.ice_floes -> effect_base_value( 1 ) / 100.0;

    may_brain_freeze = true;
    may_chill = true;
  }
};

// Counterspell Spell =======================================================

struct counterspell_t : public mage_spell_t
{
  counterspell_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "counterspell", 2139, p )
  {
    parse_options( NULL, options_str );
    may_miss = may_resist = false;
  }

  virtual void execute()
  {
    mage_t* p = player -> cast_mage();
    mage_spell_t::execute();
    p -> buffs_invocation -> trigger();
  }

  virtual bool ready()
  {
    if ( ! target -> debuffs.casting -> check() )
      return false;

    return mage_spell_t::ready();
  }
};

// Deep Freeze Spell =========================================================

struct deep_freeze_t : public mage_spell_t
{
  deep_freeze_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "deep_freeze", 71757, p )
  {
    parse_options( NULL, options_str );

    // The spell data is spread across two separate Spell IDs.  Hard code missing for now.
    base_cost = 0.09 * p -> resource_base[ RESOURCE_MANA ];
    cooldown -> duration = 30.0;

    fof_frozen = true;
    base_multiplier *= 1.0 + p -> glyphs.deep_freeze -> effect_base_value( 1 ) / 100.0;
    trigger_gcd = p -> base_gcd;
  }

  virtual bool ready()
  {
    mage_t* p = player -> cast_mage();
    if ( ! p -> buffs_fingers_of_frost -> may_react() )
      return false;
    return mage_spell_t::ready();
  }
};

// Dragon's Breath Spell ====================================================

struct dragons_breath_t : public mage_spell_t
{
  dragons_breath_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "dragons_breath", 31661, p )
  {
    parse_options( NULL, options_str );
    aoe = -1;
    cooldown -> duration += p -> glyphs.dragons_breath -> effect_base_value( 1 ) / 1000.0;
  }
};

// Evocation Spell ==========================================================

struct evocation_t : public mage_spell_t
{
  evocation_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "evocation", 12051, p )
  {
    parse_options( NULL, options_str );

    base_execute_time = 6.0;
    base_tick_time    = 2.0;
    num_ticks         = 3;
    channeled         = true;
    harmful           = false;
    hasted_ticks      = false;

    cooldown -> duration += p -> talents.arcane_flows -> effect_base_value( 2 ) / 1000.0;
  }

  virtual void execute()
  {
    mage_t* p = player -> cast_mage();
    mage_spell_t::execute();
    double mana = p -> resource_max[ RESOURCE_MANA ] * effect_base_value( 1 ) / 100.0;
    p -> resource_gain( RESOURCE_MANA, mana, p -> gains_evocation );
  }

  virtual void tick()
  {
    mage_t* p = player -> cast_mage();
    double mana = p -> resource_max[ RESOURCE_MANA ] * effect_base_value( 1 ) / 100.0;
    p -> resource_gain( RESOURCE_MANA, mana, p -> gains_evocation );
  }

  virtual bool ready()
  {
    if ( ! mage_spell_t::ready() )
      return false;

    return ( player -> resource_current[ RESOURCE_MANA ] /
             player -> resource_max    [ RESOURCE_MANA ] ) < 0.40;
  }
};

// Fire Blast Spell =========================================================

struct fire_blast_t : public mage_spell_t
{
  fire_blast_t( mage_t* p, const std::string& options_str ) :
      mage_spell_t( "fire_blast", 2136, p )
  {
    parse_options( NULL, options_str );
    base_crit += p -> talents.improved_fire_blast -> effect_base_value( 1 ) / 100.0;
    may_hot_streak = true;
  }
};

// Fireball Spell ===========================================================

struct fireball_t : public mage_spell_t
{
  fireball_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "fireball", 133, p )
  {
    parse_options( NULL, options_str );
    base_crit += p -> glyphs.fireball -> effect_base_value( 1 ) / 100.0;
    may_hot_streak = true;
    if( p -> set_bonus.tier11_4pc_caster() ) base_execute_time *= 0.9;
  }

  virtual double hot_streak_crit()
  {
    // When calculating Hot-Streak proc chance, do not include Fireball glyph
    mage_t* p = player -> cast_mage();
    return base_crit + player_crit - p -> glyphs.fireball -> effect_base_value( 1 ) / 100.0;
  }

  virtual double cost() SC_CONST
  {
    mage_t* p = player -> cast_mage();
    if ( p -> buffs_brain_freeze -> check() )
      return 0;

    return mage_spell_t::cost();
  }

  virtual double execute_time() SC_CONST
  {
    mage_t* p = player -> cast_mage();
    if ( p -> buffs_brain_freeze -> check() )
      return 0;
    return mage_spell_t::execute_time();
  }

  virtual void execute()
  {
    mage_spell_t::execute();
    consume_brain_freeze( this );
  }
};

// Flame Orb Spell ==========================================================

struct flame_orb_explosion_t : public mage_spell_t
{
  flame_orb_explosion_t( mage_t* p ) :
    mage_spell_t( "flame_orb_explosion", 83619, p )
  {
    background = true;
    aoe = -1;
    base_multiplier *= 1.0 + p -> talents.critical_mass -> effect_base_value( 2 ) / 100.0;
  }
};

struct flame_orb_tick_t : public mage_spell_t
{
  flame_orb_tick_t( mage_t* p ) :
    mage_spell_t( "flame_orb_tick", 82739, p )
  {
    background = true;
    direct_tick = true;
    base_multiplier *= 1.0 + p -> talents.critical_mass -> effect_base_value( 2 ) / 100.0;
  }
};

struct flame_orb_t : public mage_spell_t
{
  flame_orb_explosion_t* explosion_spell;
  flame_orb_tick_t*      tick_spell;

  flame_orb_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "flame_orb", 82731, p )
  {
    parse_options( NULL, options_str );

    num_ticks = 15;
    base_tick_time = 1.0;
    hasted_ticks = false;

    explosion_spell = new flame_orb_explosion_t( p );
    add_child( explosion_spell );

    tick_spell = new flame_orb_tick_t( p );
    add_child( tick_spell );
  }

  virtual void tick()
  {
    tick_spell -> execute();
    stats -> add_tick( time_to_tick );
  }

  virtual void last_tick()
  {
    mage_spell_t::last_tick();
    mage_t* p = player -> cast_mage();
    if ( p -> rng_fire_power -> roll( p -> talents.fire_power -> proc_chance() ) )
    {
      explosion_spell -> execute();
    }
  }
};

// Focus Magic Spell ========================================================

struct focus_magic_t : public mage_spell_t
{
  player_t*          focus_magic_target;
  action_callback_t* focus_magic_cb;

  focus_magic_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "focus_magic", 54646, p ),
    focus_magic_target(0), focus_magic_cb(0)
  {
    check_talent( p -> talents.focus_magic -> rank() );

    std::string target_str = p -> focus_magic_target_str;

    option_t options[] =
    {
      { "target", OPT_STRING, &target_str },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    if ( target_str.empty() )
    {
      // If no target specified, assume 100% up-time by forcing "buffs.focus_magic_feedback = 1"
      focus_magic_target = p;
    }
    else
    {
      focus_magic_target = sim -> find_player( target_str );

      assert ( focus_magic_target != 0 );
      assert ( focus_magic_target != p );
    }

    struct focus_magic_feedback_callback_t : public action_callback_t
    {
      focus_magic_feedback_callback_t( player_t* p ) : action_callback_t( p -> sim, p ) {}

      virtual void trigger( action_t* a, void* call_data )
      {
        listener -> cast_mage() -> buffs_focus_magic_feedback -> trigger();
      }
    };

    focus_magic_cb = new focus_magic_feedback_callback_t( p );
    focus_magic_cb -> active = false;
    focus_magic_target -> register_spell_callback( RESULT_CRIT_MASK, focus_magic_cb );
  }

  virtual void execute()
  {
    mage_t* p = player -> cast_mage();
    if ( sim -> log )
      log_t::output( sim, "%s performs %s", p -> name(), name() );

    if ( focus_magic_target == p )
    {
      if ( sim -> log )
        log_t::output( sim, "%s grants SomebodySomewhere Focus Magic", p -> name() );

      p -> buffs_focus_magic_feedback -> override();
    }
    else
    {
      if ( sim -> log )
        log_t::output( sim, "%s grants %s Focus Magic", p -> name(), focus_magic_target -> name() );

      focus_magic_target -> buffs.focus_magic -> trigger();
      focus_magic_cb -> active = true;
    }
  }

  virtual bool ready()
  {
    mage_t* p = player -> cast_mage();

    if ( focus_magic_target == p )
    {
      return ! p -> buffs_focus_magic_feedback -> check();
    }
    else
    {
      return ! focus_magic_target -> buffs.focus_magic -> check();
    }
  }
};

// Frostbolt Spell ==========================================================

struct frostbolt_t : public mage_spell_t
{
  frostbolt_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "frostbolt", 116, p )
  {
    parse_options( NULL, options_str );
    base_crit += p -> glyphs.frostbolt -> effect_base_value( 1 ) / 100.0;
    may_chill = true;
    may_brain_freeze = true;
    if( p -> set_bonus.tier11_4pc_caster() ) base_execute_time *= 0.9;
    base_multiplier *= 1.0 + p -> specializations.frost3;
  }

  virtual void schedule_execute()
  {
    mage_t* p = player -> cast_mage();
    mage_spell_t::schedule_execute();
    if ( ! p -> cooldowns_early_frost -> remains() )
    {
      p -> procs_early_frost -> occur();
      p -> cooldowns_early_frost -> start( 15.0 );
    }
  }

  virtual void execute()
  {
    mage_t* p = player -> cast_mage();
    mage_spell_t::execute();
    if ( result_is_hit() )
    {
      trigger_replenishment( this );
      if ( result == RESULT_CRIT )
      {
        int max_targets = p -> talents.piercing_chill -> rank();
        if ( target -> is_enemy() )
        {
          target_t* q = target -> cast_target();

          for ( int i=0; i < q -> adds_nearby && i < max_targets; i ++ )
          {
            p -> buffs_fingers_of_frost -> trigger();
          }
        }
      }
    }
  }

  virtual double execute_time() SC_CONST
  {
    mage_t* p = player -> cast_mage();
    double ct = mage_spell_t::execute_time();
    if ( p -> talents.early_frost -> rank() )
    {
      if ( ! p -> cooldowns_early_frost -> remains() )
      {
        ct += p -> talents.early_frost -> sd->effect1->base_value/1000.0;
      }
    }
    return ct;
  }

  virtual double gcd() SC_CONST
  {
    mage_t* p = player -> cast_mage();
    if ( p -> talents.early_frost -> rank() )
      if( ! p -> cooldowns_early_frost -> remains() )
  return 1.0;
    return mage_spell_t::gcd();
  }
};

// Frostfire Bolt Spell =====================================================

struct frostfire_bolt_t : public mage_spell_t
{
  int dot_stack;

  frostfire_bolt_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "frostfire_bolt", 44614, p ), dot_stack(0)
  {
    parse_options( NULL, options_str );

    may_chill = true;
    may_hot_streak = true;

    if ( p -> glyphs.frostfire -> ok() )
    {
      base_multiplier *= 1.0 + p -> glyphs.frostfire -> effect_base_value( 1 ) / 100.0;
      num_ticks = 4;
      base_tick_time = 3.0;
      dot_behavior = DOT_REFRESH;
    }
    if( p -> set_bonus.tier11_4pc_caster() ) base_execute_time *= 0.9;
  }

  virtual void reset()
  {
    mage_spell_t::reset();
    dot_stack=0;
  }

  virtual void last_tick()
  {
    mage_spell_t::last_tick();
    dot_stack=0;
  }

  virtual double cost() SC_CONST
  {
    mage_t* p = player -> cast_mage();
    if ( p -> buffs_brain_freeze -> check() )
      return 0;

    return mage_spell_t::cost();
  }

  virtual double execute_time() SC_CONST
  {
    mage_t* p = player -> cast_mage();
    if ( p -> buffs_brain_freeze -> check() )
      return 0;
    return mage_spell_t::execute_time();
  }

  virtual void execute()
  {
    mage_t* p = player -> cast_mage();
    fof_frozen = p -> buffs_brain_freeze -> up();
    mage_spell_t::execute();
    consume_brain_freeze( this );
  }

  virtual void travel( player_t* t, int travel_result, double travel_dmg )
  {
    mage_t* p = player -> cast_mage();

    if ( p -> glyphs.frostfire -> ok() && result_is_hit( travel_result ) )
    {
      if( dot_stack < 3 ) dot_stack++;
      result = RESULT_HIT;
      double dot_dmg = calculate_direct_damage() * 0.03;
      base_td = dot_stack * dot_dmg / num_ticks;
    }
    mage_spell_t::travel( t, travel_result, travel_dmg );
  }

  virtual double total_td_multiplier() SC_CONST { return 1.0; } // No double-dipping!
};

// Frostfire Orb Spell ==========================================================

struct frostfire_orb_explosion_t : public mage_spell_t
{
  frostfire_orb_explosion_t( mage_t* p ) :
    mage_spell_t( "frostfire_orb_explosion", 83619, p )
  {
    background = true;
    aoe = -1;
    school = SCHOOL_FROSTFIRE; // required since defaults to FIRE
    may_chill = ( p -> talents.frostfire_orb -> rank() == 2 );
  }
};

struct frostfire_orb_tick_t : public mage_spell_t
{
  frostfire_orb_tick_t( mage_t* p ) :
    mage_spell_t( "frostfire_orb_tick", 84721, p )
  {
    background = true;
    direct_tick = true;
    may_chill = ( p -> talents.frostfire_orb -> rank() == 2 );
  }
};

struct frostfire_orb_t : public mage_spell_t
{
  frostfire_orb_explosion_t* explosion_spell;
  frostfire_orb_tick_t*      tick_spell;

  frostfire_orb_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "frostfire_orb", 92283, p )
  {
    check_min_level( 81 );
    parse_options( NULL, options_str );

    num_ticks = 15;
    base_tick_time = 1.0;
    hasted_ticks = false;

    explosion_spell = new frostfire_orb_explosion_t( p );
    add_child( explosion_spell );

    tick_spell = new frostfire_orb_tick_t( p );
    add_child( tick_spell );
  }

  virtual void tick()
  {
    tick_spell -> execute();
    stats -> add_tick( time_to_tick );
  }

  virtual void last_tick()
  {
    mage_spell_t::last_tick();
    mage_t* p = player -> cast_mage();
    if ( p -> rng_fire_power -> roll( p -> talents.fire_power -> rank() / 3 ) )
    {
      explosion_spell -> execute();
    }
  }
};

// Ice Lance Spell ==========================================================

struct ice_lance_t : public mage_spell_t
{
  ice_lance_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "ice_lance", 30455, p )
  {
    parse_options( NULL, options_str );
    base_multiplier *= 1.0 + p -> glyphs.ice_lance -> effect_base_value( 1 ) / 100.0;
    base_crit  += p -> set_bonus.tier11_2pc_caster() * 0.05;
    fof_frozen = true;
  }

  virtual void player_buff()
  {
    mage_t* p = player -> cast_mage();
    mage_spell_t::player_buff();
    if ( p -> buffs_fingers_of_frost -> up() )
    {
      player_multiplier *= 2.0; // Built in bonus against frozen targets
      player_multiplier *= 1.15;
    }
  }
};

// Icy Veins Spell ==========================================================

struct icy_veins_t : public mage_spell_t
{
  icy_veins_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "icy_veins", 12472, p )
  {
    check_talent( p -> talents.icy_veins -> rank() );
    parse_options( NULL, options_str );
    cooldown -> duration *= 1.0 + p -> talents.ice_floes -> effect_base_value( 1 ) / 100.0;
  }

  virtual void execute()
  {
    mage_t* p = player -> cast_mage();
    if ( sim -> log ) log_t::output( sim, "%s performs %s", p -> name(), name() );
    consume_resource();
    update_ready();
    p -> buffs_icy_veins -> trigger();
  }
};

// Living Bomb Spell ========================================================

struct living_bomb_explosion_t : public mage_spell_t
{
  living_bomb_explosion_t( mage_t* p ) :
    mage_spell_t( "living_bomb_explosion", 44461, p )
  {
    aoe = -1;
    background = true;
    base_multiplier *= 1.0 + ( p -> glyphs.living_bomb -> effect_base_value( 1 ) / 100.0 +
             p -> talents.critical_mass -> effect_base_value( 2 ) / 100.0 );
  }
};

struct living_bomb_t : public mage_spell_t
{
  living_bomb_explosion_t* explosion_spell;

  living_bomb_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "living_bomb", 44457, p )
  {
    check_talent( p -> talents.living_bomb -> rank() );
    parse_options( NULL, options_str );

    dot_behavior = DOT_REFRESH;

    explosion_spell = new living_bomb_explosion_t( p );
    explosion_spell -> resource = RESOURCE_NONE; // Trickery to make MoE work
    explosion_spell -> base_cost = base_cost;
    add_child( explosion_spell );
  }

  virtual void target_debuff( player_t* t, int dmg_type )
  {
    // Override the mage_spell_t version to ensure mastery effect is stacked additively.  Someday I will make this cleaner.
    mage_t* p = player -> cast_mage();
    spell_t::target_debuff( t, dmg_type );

    target_multiplier *= 1.0 + ( p -> glyphs.living_bomb -> effect_base_value( 1 ) / 100.0 +
         p -> talents.critical_mass -> effect_base_value( 2 ) / 100.0 +
         p -> specializations.flashburn * p -> composite_mastery() );
  }

  virtual void last_tick()
  {
    mage_spell_t::last_tick();
    explosion_spell -> execute();
  }
};

// Mage Armor Spell =========================================================

struct mage_armor_t : public mage_spell_t
{
  mage_armor_t( mage_t* p, const std::string& options_str ) :
      mage_spell_t( "mage_armor", 6117, p )
  {
    parse_options( NULL, options_str );
  }

  virtual void execute()
  {
    mage_t* p = player -> cast_mage();
    if ( sim -> log ) log_t::output( sim, "%s performs %s", p -> name(), name() );
    p -> buffs_molten_armor -> expire();
    p -> buffs_mage_armor -> trigger();
  }

  virtual bool ready()
  {
    mage_t* p = player -> cast_mage();
    if( p -> buffs_mage_armor -> check() ) return false;
    return mage_spell_t::ready();
  }
};

// Mana Gem =================================================================

struct mana_gem_t : public action_t
{
  double min;
  double max;

  mana_gem_t( mage_t* p, const std::string& options_str ) :
      action_t( ACTION_USE, "mana_gem", p )
  {
    parse_options( NULL, options_str );

    min = p -> player_data.effect_min( 16856, p -> player_data.spell_scaling_class( 27103 ), p -> level );
    max = p -> player_data.effect_max( 16856, p -> player_data.spell_scaling_class( 27103 ), p -> level );

    if ( p -> level <= 80 )
    {
      min = 3330.0;
      max = 3500.0;
    }

    cooldown = p -> get_cooldown( "mana_gem" );
    cooldown -> duration = 120.0;
    trigger_gcd = 0;
    harmful = false;
  }

  virtual void execute()
  {
    mage_t* p = player -> cast_mage();

    if ( sim -> log ) log_t::output( sim, "%s uses Mana Gem", p -> name() );

    p -> procs_mana_gem -> occur();
    p -> mana_gem_charges--;

    double gain = sim -> rng -> range( min, max );

    p -> resource_gain( RESOURCE_MANA, gain, p -> gains_mana_gem );

    p -> buffs_improved_mana_gem -> trigger();

    update_ready();
  }

  virtual bool ready()
  {
    mage_t* p = player -> cast_mage();

    if ( p -> mana_gem_charges <= 0 )
      return false;

    if ( ( player -> resource_max[ RESOURCE_MANA ] - player -> resource_current[ RESOURCE_MANA ] ) < max )
      return false;

    return action_t::ready();
  }
};

// Mirror Image Spell =======================================================

struct mirror_image_t : public mage_spell_t
{
  mirror_image_t( mage_t* p, const std::string& options_str ) :
      mage_spell_t( "mirror_image", 55342, p )
  {
    parse_options( NULL, options_str );
    harmful = false;
  }

  virtual void execute()
  {
    mage_t* p = player -> cast_mage();
    consume_resource();
    update_ready();
    p -> summon_pet( "mirror_image_3" );
    p -> buffs_tier10_4pc -> trigger();
  }

  virtual double gcd() SC_CONST
  {
    mage_t* p = player -> cast_mage();
    if ( p -> buffs_arcane_power -> check() ) return 0;
    return mage_spell_t::gcd();
  }
};

// Molten Armor Spell =======================================================

struct molten_armor_t : public mage_spell_t
{
  molten_armor_t( mage_t* p, const std::string& options_str ) :
      mage_spell_t( "molten_armor", 30482, p )
  {
    parse_options( NULL, options_str );
  }

  virtual void execute()
  {
    mage_t* p = player -> cast_mage();
    if ( sim -> log ) log_t::output( sim, "%s performs %s", p -> name(), name() );
    p -> buffs_mage_armor -> expire();
    p -> buffs_molten_armor -> trigger();
  }

  virtual bool ready()
  {
    mage_t* p = player -> cast_mage();
    if( p -> buffs_molten_armor -> check() ) return false;
    return mage_spell_t::ready();
  }
};

// Presence of Mind Spell ===================================================

struct presence_of_mind_t : public mage_spell_t
{
  action_t* fast_action;

  presence_of_mind_t( mage_t* p, const std::string& options_str ) :
      mage_spell_t( "presence_of_mind", 12043, p )
  {
    check_talent( p -> talents.presence_of_mind -> rank() );

    harmful = false;

    cooldown -> duration *= 1.0 + p -> talents.arcane_flows -> effect_base_value( 1 ) / 100.0;

    if ( options_str.empty() )
    {
      sim -> errorf( "Player %s: The presence_of_mind action must be coupled with a second action.", p -> name() );
      sim -> cancel();
    }

    std::string spell_name    = options_str;
    std::string spell_options = "";

    std::string::size_type cut_pt = spell_name.find_first_of( "," );

    if ( cut_pt != spell_name.npos )
    {
      spell_options = spell_name.substr( cut_pt + 1 );
      spell_name    = spell_name.substr( 0, cut_pt );
    }

    fast_action = p -> create_action( spell_name.c_str(), spell_options.c_str() );
    fast_action -> base_execute_time = 0;
    fast_action -> background = true;
  }

  virtual void execute()
  {
    mage_spell_t::execute();
    mage_t* p = player -> cast_mage();
    p -> aura_gain( "Presence of Mind" );
    p -> last_foreground_action = fast_action;
    p -> buffs_arcane_potency -> trigger( 2 );
    fast_action -> execute();
    p -> aura_loss( "Presence of Mind" );
    update_ready();
  }

  virtual bool ready()
  {
    return( mage_spell_t::ready() && fast_action -> ready() );
  }
};

// Pyroblast Spell ==========================================================

struct pyroblast_t : public mage_spell_t
{
  pyroblast_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "pyroblast", 11366, p )
  {
    check_spec( TREE_FIRE );
    parse_options( NULL, options_str );
    base_crit += p -> glyphs.pyroblast -> effect_base_value( 1 ) / 100.0;
    base_crit += p -> set_bonus.tier11_2pc_caster() * 0.05;
    may_hot_streak = true;
    dot_behavior = DOT_REFRESH;
  }

  virtual void execute()
  {
    mage_t* p = player -> cast_mage();
    mage_spell_t::execute();
    if( result_is_hit() )
    {
      target -> debuffs.critical_mass -> trigger( 1, 1.0, p -> talents.critical_mass -> proc_chance() );
      target -> debuffs.critical_mass -> source = p;
    }
  }
};

// Pyroblast! Spell ==========================================================

struct pyroblast_hs_t : public mage_spell_t
{
  pyroblast_hs_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "pyroblast_hs", 92315, p )
  {
    check_spec( TREE_FIRE );
    parse_options( NULL, options_str );
    base_crit += p -> glyphs.pyroblast -> effect_base_value( 1 ) / 100.0;
    base_crit += p -> set_bonus.tier11_2pc_caster() * 0.05;
    dot = p -> get_dot( "pyroblast" );
    dot_behavior = DOT_REFRESH;
  }

  virtual void execute()
  {
    mage_t* p = player -> cast_mage();
    if ( p -> buffs_hot_streak -> up() )
    {
      p -> buffs_hot_streak -> expire();
      p -> buffs_tier10_2pc -> trigger();
    }
    mage_spell_t::execute();
    if( result_is_hit() )
    {
      target -> debuffs.critical_mass -> trigger( 1, 1.0, p -> talents.critical_mass -> proc_chance() );
      target -> debuffs.critical_mass -> source = p;
    }
  }

  virtual double cost() SC_CONST
  {
    mage_t* p = player -> cast_mage();
    if ( p -> buffs_hot_streak -> check() )
      return 0;
    return mage_spell_t::cost();
  }

  virtual double execute_time() SC_CONST
  {
    mage_t* p = player -> cast_mage();
    if ( p -> buffs_hot_streak -> check() )
      return 0;
    return mage_spell_t::execute_time();
  }
};

// Scorch Spell =============================================================

struct scorch_t : public mage_spell_t
{
  int debuff;

  scorch_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "scorch", 2948, p ), debuff( 0 )
  {
    option_t options[] =
    {
      { "debuff",    OPT_BOOL, &debuff     },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    base_cost *= 1.0 + 0.01 * p -> talents.improved_scorch -> effect_base_value( 1 );
    may_hot_streak = true;

    if ( debuff )
      check_talent( p -> talents.critical_mass -> rank() );

  }

  virtual bool usable_moving()
  {
    mage_t* p = player -> cast_mage();
    return p -> talents.firestarter -> rank() != 0;
  }

  virtual void execute()
  {
    mage_t* p = player -> cast_mage();
    mage_spell_t::execute();
    if ( result_is_hit() )
    {
      target -> debuffs.critical_mass -> trigger( 1, 1.0, p -> talents.critical_mass -> proc_chance() );
      target -> debuffs.critical_mass -> source = p;
    }
  }

  virtual bool ready()
  {
    if ( ! mage_spell_t::ready() )
      return false;

    if ( debuff )
    {

      if ( target -> debuffs.shadow_and_flame -> check() )
        return false;

      if ( target -> debuffs.critical_mass -> remains_gt( 6.0 ) )
        return false;
    }

    return true;
  }
};

// Slow Spell ===============================================================

struct slow_t : public mage_spell_t
{
  slow_t( mage_t* p, const std::string& options_str ) :
      mage_spell_t( "slow", 31589, p )
  {
    check_talent( p -> talents.slow -> rank() );
    parse_options( NULL, options_str );
  }

  virtual void execute()
  {
    mage_spell_t::execute();
    target -> debuffs.slow -> trigger();
  }

  virtual bool ready()
  {
    if ( target -> debuffs.snared() )
      return false;
    return mage_spell_t::ready();
  }
};

// Time Warp Spell ==========================================================

struct time_warp_t : public mage_spell_t
{
  time_warp_t( mage_t* p, const std::string& options_str ) :
    mage_spell_t( "time_warp", 80353, p )
  {
    parse_options( NULL, options_str );
    harmful = false;
  }

  virtual void execute()
  {
    mage_spell_t::execute();

    for ( player_t* p = sim -> player_list; p; p = p -> next )
    {
      if ( p -> sleeping || p -> buffs.exhaustion -> check() )
        continue;

      p -> buffs.bloodlust -> trigger(); // Bloodlust and Timewarp are the same
      p -> buffs.exhaustion -> trigger();
    }
  }

  virtual bool ready()
  {
    if ( player -> buffs.exhaustion -> check() )
      return false;
    return mage_spell_t::ready();
  }
};

// Water Elemental Spell ====================================================

struct water_elemental_spell_t : public mage_spell_t
{
  water_elemental_spell_t( mage_t* p, const std::string& options_str ) :
      mage_spell_t( "water_elemental", 31687, p )
  {
    check_spec( TREE_FROST );
    parse_options( NULL, options_str );
    harmful = false;
    trigger_gcd = 0;
    base_cost = 0;
  }

  virtual void execute()
  {
    mage_spell_t::execute();
    player -> summon_pet( "water_elemental" );
  }

  virtual bool ready()
  {
    if ( ! mage_spell_t::ready() )
      return false;
    return player -> cast_mage() -> active_water_elemental == 0;
  }
};

// Choose Rotation ==========================================================

struct choose_rotation_t : public action_t
{
  double last_time;

  choose_rotation_t( mage_t* p, const std::string& options_str ) :
      action_t( ACTION_USE, "choose_rotation", p ), last_time( 0 )
  {
    cooldown -> duration = 10;

    option_t options[] =
    {
      { "cooldown", OPT_FLT, &( cooldown -> duration ) },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    if ( cooldown -> duration < 1.0 )
    {
      sim -> errorf( "Player %s: choose_rotation cannot have cooldown -> duration less than 1.0sec", p -> name() );
      cooldown -> duration = 1.0;
    }

    trigger_gcd = 0;
    harmful = false;
  }

  virtual void execute()
  {
    mage_t* p = player -> cast_mage();

    if ( sim -> log ) log_t::output( sim, "%s Considers Spell Rotation", p -> name() );

    // It is important to smooth out the regen rate by averaging out the returns from Evocation and Mana Gems.
    // In order for this to work, the resource_gain() method must filter out these sources when
    // tracking "rotation.mana_gain".

    double regen_rate = p -> rotation.mana_gain / sim -> current_time;

    // Evocation
    regen_rate += p -> resource_max[ RESOURCE_MANA ] * 0.60 / ( 240.0 - p -> talents.arcane_flows -> rank() * 60.0 );

    // Mana Gem, if we have uses left
    if ( p -> mana_gem_charges > 0 )
      regen_rate += p ->player_data.effect_max( 16856, p ->type, p ->level ) / 60.0;

    if ( p -> rotation.current == ROTATION_DPS )
    {
      p -> rotation.dps_time += ( sim -> current_time - last_time );

      double consumption_rate = ( p -> rotation.dps_mana_loss / p -> rotation.dps_time ) - regen_rate;

      if ( consumption_rate > 0 )
      {
        double oom_time = p -> resource_current[ RESOURCE_MANA ] / consumption_rate;

        if ( oom_time < sim -> target -> time_to_die() )
        {
          if ( sim -> log ) log_t::output( sim, "%s switches to DPM spell rotation", p -> name() );

          p -> rotation.current = ROTATION_DPM;
        }
      }
    }
    else if ( p -> rotation.current == ROTATION_DPM )
    {
      p -> rotation.dpm_time += ( sim -> current_time - last_time );

      double consumption_rate = ( p -> rotation.dpm_mana_loss / p -> rotation.dpm_time ) - regen_rate;

      if ( consumption_rate > 0 )
      {
        double oom_time = p -> resource_current[ RESOURCE_MANA ] / consumption_rate;

        if ( oom_time > sim -> target -> time_to_die() )
        {
          if ( sim -> log ) log_t::output( sim, "%s switches to DPS spell rotation", p -> name() );

          p -> rotation.current = ROTATION_DPS;
        }
      }
      else
      {
        if ( sim -> log ) log_t::output( sim, "%s switches to DPS rotation (negative consumption)", p -> name() );

        p -> rotation.current = ROTATION_DPS;
      }
    }
    last_time = sim -> current_time;

    update_ready();
  }

  virtual bool ready()
  {
    if ( cooldown -> remains() > 0 )
      return false;

    if ( sim -> current_time < cooldown -> duration )
      return false;

    return true;
  }

  virtual void reset()
  {
    action_t::reset();
    last_time=0;
  }
};

} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// Mage Character Definition
// ==========================================================================

// mage_t::create_action ====================================================

action_t* mage_t::create_action( const std::string& name,
                                 const std::string& options_str )
{
  if ( name == "arcane_barrage"    ) return new          arcane_barrage_t( this, options_str );
  if ( name == "arcane_blast"      ) return new            arcane_blast_t( this, options_str );
  if ( name == "arcane_brilliance" ) return new       arcane_brilliance_t( this, options_str );
  if ( name == "arcane_explosion"  ) return new        arcane_explosion_t( this, options_str );
  if ( name == "arcane_missiles"   ) return new         arcane_missiles_t( this, options_str );
  if ( name == "arcane_power"      ) return new            arcane_power_t( this, options_str );
  if ( name == "blast_wave"        ) return new              blast_wave_t( this, options_str );
  if ( name == "choose_rotation"   ) return new         choose_rotation_t( this, options_str );
  if ( name == "cold_snap"         ) return new               cold_snap_t( this, options_str );
  if ( name == "cone_of_cold"      ) return new            cone_of_cold_t( this, options_str );
  if ( name == "combustion"        ) return new              combustion_t( this, options_str );
  if ( name == "counterspell"      ) return new            counterspell_t( this, options_str );
  if ( name == "deep_freeze"       ) return new             deep_freeze_t( this, options_str );
  if ( name == "dragons_breath"    ) return new          dragons_breath_t( this, options_str );
  if ( name == "evocation"         ) return new               evocation_t( this, options_str );
  if ( name == "fire_blast"        ) return new              fire_blast_t( this, options_str );
  if ( name == "fireball"          ) return new                fireball_t( this, options_str );
  if ( name == "flame_orb"         ) return new               flame_orb_t( this, options_str );
  if ( name == "focus_magic"       ) return new             focus_magic_t( this, options_str );
  if ( name == "frostbolt"         ) return new               frostbolt_t( this, options_str );
  if ( name == "frostfire_bolt"    ) return new          frostfire_bolt_t( this, options_str );
  if ( name == "frostfire_orb"     ) return new           frostfire_orb_t( this, options_str );
  if ( name == "ice_lance"         ) return new               ice_lance_t( this, options_str );
  if ( name == "icy_veins"         ) return new               icy_veins_t( this, options_str );
  if ( name == "living_bomb"       ) return new             living_bomb_t( this, options_str );
  if ( name == "mage_armor"        ) return new              mage_armor_t( this, options_str );
  if ( name == "mana_gem"          ) return new                mana_gem_t( this, options_str );
  if ( name == "mirror_image"      ) return new            mirror_image_t( this, options_str );
  if ( name == "molten_armor"      ) return new            molten_armor_t( this, options_str );
  if ( name == "presence_of_mind"  ) return new        presence_of_mind_t( this, options_str );
  if ( name == "pyroblast"         ) return new               pyroblast_t( this, options_str );
  if ( name == "pyroblast_hs"      ) return new            pyroblast_hs_t( this, options_str );
  if ( name == "scorch"            ) return new                  scorch_t( this, options_str );
  if ( name == "slow"              ) return new                    slow_t( this, options_str );
  if ( name == "time_warp"         ) return new               time_warp_t( this, options_str );
  if ( name == "water_elemental"   ) return new   water_elemental_spell_t( this, options_str );

  return player_t::create_action( name, options_str );
}

// mage_t::create_pet =======================================================

pet_t* mage_t::create_pet( const std::string& pet_name,
                           const std::string& pet_type )
{
  pet_t* p = find_pet( pet_name );

  if ( p ) return p;

  if ( pet_name == "mirror_image_3"  ) return new mirror_image_pet_t   ( sim, this );
  if ( pet_name == "water_elemental" ) return new water_elemental_pet_t( sim, this );

  return 0;
}

// mage_t::create_pets ======================================================

void mage_t::create_pets()
{
  create_pet( "mirror_image_3"  );
  create_pet( "water_elemental" );
}

// mage_t::init_talents =====================================================

void mage_t::init_talents()
{
  player_t::init_talents();

  talent_t* t=0;

  // Arcane
  talents.arcane_concentration        = t = find_talent( "Arcane Concentration" );
  talents.arcane_flows                = t = find_talent( "Arcane Flows" );
  talents.arcane_potency              = t = find_talent( "Arcane Potency" );
  talents.arcane_power                = t = find_talent( "Arcane Power" );
  talents.arcane_tactics              = t = find_talent( "Arcane Tactics" );
  talents.focus_magic                 = t = find_talent( "Focus Magic" );
  talents.improved_arcane_explosion   = t = find_talent( "Improved Arcane Explosion" );
  talents.improved_arcane_missiles    = t = find_talent( "Improved Arcane Missiles" );
  talents.improved_counterspell       = t = find_talent( "Improved Counterspell" );
  talents.improved_mana_gem           = t = find_talent( "Improved Mana Gem" );
  talents.invocation                  = t = find_talent( "Invocation" );
  talents.missile_barrage             = t = find_talent( "Missile Barrage" );
  talents.nether_vortex               = t = find_talent( "Nether Vortex" );
  talents.netherwind_presence         = t = find_talent( "Netherwind Presence" );
  talents.presence_of_mind            = t = find_talent( "Presence of Mind" );
  talents.slow                        = t = find_talent( "Slow" );
  talents.torment_the_weak            = t = find_talent( "Torment the Weak" );

  // Fire
  talents.blast_wave                  = t = find_talent( "Blast Wave" );
  talents.combustion                  = t = find_talent( "Combustion" );
  talents.critical_mass               = t = find_talent( "Critical Mass" );
  talents.dragons_breath              = t = find_talent( "Dragon's Breath" );
  talents.fire_power                  = t = find_talent( "Fire Power" );
  talents.firestarter                 = t = find_talent( "Firestarter" );
  talents.hot_streak                  = t = find_talent( "Hot Streak" );
  talents.ignite                      = t = find_talent( "Ignite" );
  talents.impact                      = t = find_talent( "Impact" );
  talents.improved_fire_blast         = t = find_talent( "Improved Fire Blast" );
  talents.improved_flamestrike        = t = find_talent( "Improved Flamestrike" );
  talents.improved_hot_streak         = t = find_talent( "Improved Hot Streak" );
  talents.improved_scorch             = t = find_talent( "Improved Scorch" );
  talents.living_bomb                 = t = find_talent( "Living Bomb" );
  talents.master_of_elements          = t = find_talent( "Master of Elements" );
  talents.molten_fury                 = t = find_talent( "Molten Fury" );

  // Frost
  talents.brain_freeze     = t = find_talent( "Brain Freeze"     );
  talents.cold_snap        = t = find_talent( "Cold Snap"        );
  talents.deep_freeze      = t = find_talent( "Deep Freeze"      );
  talents.early_frost      = t = find_talent( "Early Frost"      );
  talents.enduring_winter  = t = find_talent( "Enduring Winter"  );
  talents.fingers_of_frost = t = find_talent( "Fingers of Frost" );
  talents.frostfire_orb    = t = find_talent( "Frostfire Orb"    );
  talents.ice_barrier      = t = find_talent( "Ice Barrier"      );
  talents.ice_floes        = t = find_talent( "Ice Floes"        );
  talents.icy_veins        = t = find_talent( "Icy Veins"        );
  talents.improved_freeze  = t = find_talent( "Improved Freeze"  );
  talents.piercing_ice     = t = find_talent( "Piercing Ice"     );
  talents.piercing_chill   = t = find_talent( "Piercing Chill"   );
  talents.shatter          = t = find_talent( "Shatter"          );
}

// mage_t::init_spells ======================================================

void mage_t::init_spells()
{
  player_t::init_spells();

  spells.arcane_blast    = spell_data_t::find( 36032, "Arcane Blast" );
  spells.arcane_missiles = spell_data_t::find( 79683, "Arcane Missiles!" );
  spells.hot_streak      = spell_data_t::find( 48108, "Hot Streak" );
  spells.mage_armor      = spell_data_t::find(  6117, "Mage Armor" );
  spells.molten_armor    = spell_data_t::find( 30482, "Molten Armor" );

  spells.flashburn  = spell_data_t::find( 76595, "Flashburn" );
  spells.frostburn  = spell_data_t::find( 76613, "Frostburn" );
  spells.mana_adept = spell_data_t::find( 76547, "Mana Adept" );

  spells.arcane_specialization = spell_data_t::find( 84671, "Arcane Specialization" );
  spells.fire_specialization   = spell_data_t::find( 84668, "Fire Specialization" );
  spells.frost_specialization  = spell_data_t::find( 84669, "Frost Specialization" );

  memset( (void*) &specializations, 0x00, sizeof( specializations_t ) );

  if ( specialization == MAGE_ARCANE )
  {
    specializations.arcane     = spells.arcane_specialization -> effect1 -> base_value / 100.0;
    specializations.mana_adept = spells.mana_adept -> effect2 -> base_value / 10000.0;
  }
  else if ( specialization == MAGE_FIRE )
  {
    specializations.fire      = spells.fire_specialization -> effect1 -> base_value / 100.0;
    specializations.flashburn = spells.flashburn -> effect2 -> base_value / 10000.0;
  }
  else if ( specialization == MAGE_FROST )
  {
    specializations.frost1    = spells.frost_specialization -> effect1 -> base_value / 100.0;
    specializations.frost2    = spells.frost_specialization -> effect2 -> base_value;
    specializations.frost3    = spells.frost_specialization -> effect3 -> base_value / 100.0;
    specializations.frostburn = spells.frostburn -> effect2 -> base_value / 10000.0;
  }

  glyphs.arcane_barrage       = find_glyph( "Glyph of Arcane Barrage" );
  glyphs.arcane_blast         = find_glyph( "Glyph of Arcane Blast" );
  glyphs.arcane_brilliance    = find_glyph( "Glyph of Arcane Brilliance" );
  glyphs.arcane_missiles      = find_glyph( "Glyph of Arcane Missiles" );
  glyphs.cone_of_cold         = find_glyph( "Glyph of Cone of Cold" );
  glyphs.deep_freeze          = find_glyph( "Glyph of Deep Freeze" );
  glyphs.dragons_breath       = find_glyph( "Glyph of Dragon's Breath" );
  glyphs.fireball             = find_glyph( "Glyph of Fireball" );
  glyphs.frostbolt            = find_glyph( "Glyph of Frostbolt" );
  glyphs.frostfire            = find_glyph( "Glyph of Frostfire" );
  glyphs.ice_lance            = find_glyph( "Glyph of Ice Lance" );
  glyphs.living_bomb          = find_glyph( "Glyph of Living Bomb" );
  glyphs.mage_armor           = find_glyph( "Glyph of Mage Armor" );
  glyphs.mirror_image         = find_glyph( "Glyph of Mirror Image" );
  glyphs.molten_armor         = find_glyph( "Glyph of Molten Armor" );
  glyphs.pyroblast            = find_glyph( "Glyph of Pyroblast" );

  static uint32_t set_bonuses[N_TIER][N_TIER_BONUS] =
  {
    //  C2P    C4P    M2P    M4P    T2P    T4P    H2P    H4P
    { 70752, 70748,     0,     0,     0,     0,     0,     0 }, // Tier10
    { 90290, 90291,     0,     0,     0,     0,     0,     0 }, // Tier11
    {     0,     0,     0,     0,     0,     0,     0,     0 },
  };

  sets = new set_bonus_array_t( this, set_bonuses );
}

// mage_t::init_base ========================================================

void mage_t::init_base()
{
  player_t::init_base();

  initial_spell_power_per_intellect = 1.0;

  base_attack_power = -10;
  initial_attack_power_per_strength = 1.0;

  mana_per_intellect = 15;

  diminished_kfactor    = 0.009830;
  diminished_dodge_capi = 0.006650;
  diminished_parry_capi = 0.006650;
}

// mage_t::init_scaling ========================================================

void mage_t::init_scaling()
{
  player_t::init_scaling();
  scales_with[ STAT_SPIRIT ] = 0;
}

// mage_t::init_buffs =======================================================

void mage_t::init_buffs()
{
  player_t::init_buffs();

  // buff_t( sim, player, name, max_stack, duration, cooldown, proc_chance, quiet )

  buffs_arcane_blast         = new buff_t( this, spells.arcane_blast,          NULL );
  buffs_arcane_missiles      = new buff_t( this, spells.arcane_missiles,       "chance", 0.40, NULL );
  buffs_arcane_potency       = new buff_t( this, talents.arcane_potency,       "stacks", 2, NULL );
  buffs_arcane_power         = new buff_t( this, talents.arcane_power,         "cooldown", 0.0, NULL ); // CD managed in action
  buffs_brain_freeze         = new buff_t( this, talents.brain_freeze,         NULL );
  buffs_clearcasting         = new buff_t( this, talents.arcane_concentration, "cooldown", 15.0, NULL );
  buffs_fingers_of_frost     = new buff_t( this, talents.fingers_of_frost,     "chance", talents.fingers_of_frost->sd->effect1->base_value/100.0, NULL );
  buffs_hot_streak           = new buff_t( this, spells.hot_streak,            NULL );
  buffs_icy_veins            = new buff_t( this, talents.icy_veins,            "cooldown", 0.0, NULL ); // CD managed in action
  buffs_improved_mana_gem    = new buff_t( this, talents.improved_mana_gem,    "duration", 15.0, NULL );
  buffs_invocation           = new buff_t( this, talents.invocation,           NULL );
  buffs_mage_armor           = new buff_t( this, spells.mage_armor,            NULL );
  buffs_molten_armor         = new buff_t( this, spells.molten_armor,          NULL );

  buffs_focus_magic_feedback = new buff_t( this, "focus_magic_feedback", 1, 10.0 );
  buffs_hot_streak_crits     = new buff_t( this, "hot_streak_crits",     2,    0, 0, 1.0, true );
  buffs_tier10_2pc           = new buff_t( this, "tier10_2pc",           1,  5.0, 0, set_bonus.tier10_2pc_caster() );
  buffs_tier10_4pc           = new buff_t( this, "tier10_4pc",           1, 30.0, 0, set_bonus.tier10_4pc_caster() );
}

// mage_t::init_gains =======================================================

void mage_t::init_gains()
{
  player_t::init_gains();

  gains_clearcasting       = get_gain( "clearcasting"       );
  gains_evocation          = get_gain( "evocation"          );
  gains_mage_armor         = get_gain( "mage_armor"         );
  gains_mana_gem           = get_gain( "mana_gem"           );
  gains_master_of_elements = get_gain( "master_of_elements" );
}

// mage_t::init_procs =======================================================

void mage_t::init_procs()
{
  player_t::init_procs();

  procs_deferred_ignite = get_proc( "deferred_ignite" );
  procs_munched_ignite  = get_proc( "munched_ignite"  );
  procs_rolled_ignite   = get_proc( "rolled_ignite"   );
  procs_mana_gem        = get_proc( "mana_gem"        );
  procs_early_frost     = get_proc( "early_frost"     );
}

// mage_t::init_uptimes =====================================================

void mage_t::init_uptimes()
{
  player_t::init_uptimes();

  uptimes_arcane_blast[ 0 ]    = get_uptime( "arcane_blast_0"  );
  uptimes_arcane_blast[ 1 ]    = get_uptime( "arcane_blast_1"  );
  uptimes_arcane_blast[ 2 ]    = get_uptime( "arcane_blast_2"  );
  uptimes_arcane_blast[ 3 ]    = get_uptime( "arcane_blast_3"  );
  uptimes_arcane_blast[ 4 ]    = get_uptime( "arcane_blast_4"  );
  uptimes_dps_rotation         = get_uptime( "dps_rotation"    );
  uptimes_dpm_rotation         = get_uptime( "dpm_rotation"    );
  uptimes_water_elemental      = get_uptime( "water_elemental" );
}

// mage_t::init_rng =========================================================

void mage_t::init_rng()
{
  player_t::init_rng();

  rng_arcane_missiles = get_rng( "arcane_missiles" );
  rng_empowered_fire  = get_rng( "empowered_fire"  );
  rng_enduring_winter = get_rng( "enduring_winter" );
  rng_fire_power      = get_rng( "fire_power"      );
  rng_impact          = get_rng( "impact"          );
  rng_improved_freeze = get_rng( "improved_freeze" );
  rng_nether_vortex   = get_rng( "nether_vortex"   );
}

// mage_t::init_actions =====================================================

void mage_t::init_actions()
{
  if ( action_list_str.empty() )
  {
    // Flask
    // TO-DO: Revert to >= 80 once Cata is out
    if ( level > 80 )
      action_list_str += "/flask,type=draconic_mind";
    else if ( level >= 75 )
      action_list_str += "/flask,type=frost_wyrm";

    // Food
    // TO-DO: Revert to >= 80 once Cata is out
    if ( level > 80 ) action_list_str += "/food,type=seafood_magnifique_feast";
    else if ( level >= 70 ) action_list_str += "/food,type=fish_feast";

    // Focus Magic
    if ( talents.focus_magic -> rank() ) action_list_str += "/focus_magic";
    // Arcane Brilliance
    action_list_str += "/arcane_brilliance";

    // Armor
    if ( primary_tree() == TREE_ARCANE )
    {
      action_list_str += "/mage_armor";
    }
    else if ( primary_tree() == TREE_FIRE )
    {
      action_list_str += "/molten_armor,if=buff.mage_armor.down&buff.molten_armor.down";
    }
    else
    {
      action_list_str += "/molten_armor,if=buff.mage_armor.down&buff.molten_armor.down";
    }

    // Water Elemental
    if ( primary_tree() == TREE_FROST ) action_list_str += "/water_elemental";

    // Snapshot Stats
    action_list_str += "/snapshot_stats";
    if ( level > 80 )
    {
      action_list_str += "/volcanic_potion,if=!in_combat";
    }
    // Counterspell
    action_list_str += "/counterspell";
    // Usable Items
    int num_items = ( int ) items.size();
    for ( int i=0; i < num_items; i++ )
    {
      if ( items[ i ].use.active() )
      {
        action_list_str += "/use_item,name=";
        action_list_str += items[ i ].name();
      }
      if ( ! strcmp( items[ i ].name(), "shard_of_woe" ) )
      {
        action_list_str += ",if=cooldown.evocation.remains>26";
      }
    }
    //Potions
    if ( level > 80 )
    {
      if ( primary_tree() == TREE_FROST )
      {
        action_list_str += "/volcanic_potion,if=buff.bloodlust.react|buff.icy_veins.react|target.time_to_die<=40";
      }
      else if ( primary_tree() == TREE_ARCANE )
      {
        action_list_str += "/volcanic_potion,if=cooldown.evocation.remains<35&buff.arcane_blast.stack=4";
      }
      else
      {
        action_list_str += "/volcanic_potion,if=buff.bloodlust.react|target.time_to_die<=40";
      }
    }
    else if ( level >= 70 )
    {
      action_list_str += "/speed_potion,if=!in_combat";
      action_list_str += "/speed_potion,if=buff.bloodlust.react|target.time_to_die<=20";
    }
    // Race Abilities
    if ( race == RACE_TROLL && primary_tree() == TREE_FIRE )
    {
      action_list_str += "/berserking";
    }
    else if ( race == RACE_BLOOD_ELF && primary_tree() != TREE_ARCANE )
    {
      action_list_str += "/arcane_torrent";
    }
    else if ( race == RACE_ORC && primary_tree() != TREE_ARCANE )
    {
      action_list_str += "/blood_fury";
    }

    // Talents by Spec
    // Arcane
    if ( primary_tree() == TREE_ARCANE )
    {
      //Special handling for Race Abilities
      if ( race == RACE_BLOOD_ELF )
      {
        action_list_str += "/arcane_torrent,if=mana_pct<75";
      }
      else if ( race == RACE_ORC )
      {
        action_list_str += "/blood_fury,if=cooldown.evocation.remains<35&buff.arcane_blast.stack=4";
      }
      else if ( race == RACE_TROLL )
      {
        action_list_str += "/berserking,if=buff.arcane_power.up|cooldown.arcane_power.remains>20";
      }
      if ( talents.arcane_power -> rank() ) action_list_str += "/arcane_power,if=target.time_to_die<40";
      if ( talents.arcane_power -> rank() ) action_list_str += "/arcane_power,if=cooldown.evocation.remains<35&buff.arcane_blast.stack=4";
      action_list_str += "/mana_gem,if=target.time_to_die<40";
      action_list_str += "/mana_gem,if=cooldown.evocation.remains<35&buff.arcane_blast.stack=4";
      action_list_str += "/mirror_image,if=buff.arcane_power.up|(cooldown.arcane_power.remains>20&target.time_to_die>15)";
      if ( level >= 81 ) action_list_str += "/flame_orb,if=target.time_to_die>=10";
      if ( talents.presence_of_mind -> rank() )
      {
        action_list_str += "/presence_of_mind,arcane_blast";
      }
      action_list_str += "/arcane_blast,if=target.time_to_die<40&mana_pct>5";
      action_list_str += "/arcane_blast,if=buff.clearcasting.react&buff.arcane_blast.stack>=2";
      action_list_str += "/arcane_blast,if=(cooldown.evocation.remains<35&mana_pct>26)";
      action_list_str += "/arcane_blast,if=mana_pct>94";
      action_list_str += "/evocation,if=target.time_to_die>=31";
      // Switch conserve action list based on Shard of Woe presence
      bool has_shard = false;
      for ( int i=0; i < SLOT_MAX; i++ )
      {
        item_t& item = items[ i ];
        if ( strstr( item.name(), "shard_of_woe") )
        {
          has_shard = true;
          break;
        }
      }
      if ( has_shard == true ) // AB5 conserve with the Shard
      {
        action_list_str += "/sequence,name=conserve:arcane_blast:arcane_blast:arcane_blast:arcane_blast:arcane_blast";
        action_list_str += "/arcane_missiles";
        action_list_str += "/arcane_barrage,if=buff.arcane_blast.stack>0";
        action_list_str += "/restart_sequence,name=conserve";
        action_list_str += "/arcane_barrage,moving=1";
      }
      else // AB4 conserve without Shard (AB3 during Bloodlust)
      {
        action_list_str += "/arcane_blast,if=buff.arcane_blast.stack<4&!buff.bloodlust.react";
        action_list_str += "/arcane_blast,if=buff.arcane_blast.stack<3&buff.bloodlust.react";
        action_list_str += "/arcane_missiles";
        action_list_str += "/arcane_barrage";
      }
      action_list_str += "/fire_blast,moving=1"; // when moving
      action_list_str += "/ice_lance,moving=1"; // when moving
    }
    // Fire
    else if ( primary_tree() == TREE_FIRE )
    {
      action_list_str += "/mana_gem,if=mana_deficit>12500";
      if ( talents.critical_mass -> rank() ) action_list_str += "/scorch,debuff=1";
      if ( talents.combustion -> rank()   )
       {
         action_list_str += "/combustion,if=dot.living_bomb.ticking&dot.ignite.ticking&dot.pyroblast.ticking";
      }
      action_list_str += "/mirror_image,if=target.time_to_die>=25";
      if ( talents.living_bomb -> rank() ) action_list_str += "/living_bomb,if=!ticking";
      if ( talents.hot_streak -> rank()  ) action_list_str += "/pyroblast_hs,if=buff.hot_streak.react";
      if ( level >= 81 ) action_list_str += "/flame_orb,if=target.time_to_die>=12";
      action_list_str += "/mage_armor,if=mana_pct<5";
      if ( glyphs.frostfire -> ok() )
      {
        action_list_str += "/frostfire_bolt";
      }
      else
      {
        action_list_str += "/fireball";
      }
      action_list_str += "/scorch"; // This can be free, so cast it last
    }
    // Frost
    else if ( primary_tree() == TREE_FROST )
    {
      action_list_str += "/mana_gem,if=mana_deficit>12500";
      if ( talents.cold_snap -> rank() ) action_list_str += "/cold_snap,if=cooldown.deep_freeze.remains>15&cooldown.frostfire_orb.remains>30&cooldown.icy_veins.remains>30";
      if ( talents.frostfire_orb -> rank() && level >= 81 )
      {
        action_list_str += "/frostfire_orb,if=target.time_to_die>=12";
      }
      action_list_str += "/mirror_image,if=target.time_to_die>=25";
      if ( race == RACE_TROLL )
      {
        action_list_str += "/berserking,if=buff.icy_veins.down&buff.bloodlust.down";
      }
      if ( talents.icy_veins -> rank() ) action_list_str += "/icy_veins,if=buff.icy_veins.down&buff.bloodlust.down";
      if ( talents.deep_freeze -> rank() ) action_list_str += "/deep_freeze";
      if ( talents.brain_freeze -> rank() )
      {
        action_list_str += "/frostfire_bolt,if=buff.brain_freeze.react&buff.fingers_of_frost.react";
      }
      action_list_str += "/ice_lance,if=buff.fingers_of_frost.stack>1";
      action_list_str += "/ice_lance,if=buff.fingers_of_frost.react&pet.water_elemental.cooldown.freeze.remains<gcd";
      if ( glyphs.frostbolt -> ok() )
      {
        if ( level >= 68 ) action_list_str += "/mage_armor,if=(mana_pct*12)<target.time_to_die";
      }
      else
      {
        if ( level >= 68 ) action_list_str += "/mage_armor,if=(mana_pct*15)<target.time_to_die";
      }
      action_list_str += "/evocation,if=mana_pct<5&target.time_to_die>60";
      if ( glyphs.frostbolt -> ok() )
      {
        action_list_str += "/frostbolt";
      }
      else
      {
        action_list_str += "/frostbolt,if=!cooldown.early_frost.remains";
        action_list_str += "/frostfire_bolt";
      }
      action_list_str += "/ice_lance,moving=1"; // when moving
      action_list_str += "/fire_blast,moving=1"; // when moving
    }


    action_list_default = 1;
  }

  player_t::init_actions();
}

// mage_t::composite_spell_power ============================================

double mage_t::composite_spell_power( const school_type school ) SC_CONST
{
  double sp = player_t::composite_spell_power( school );

  if ( buffs_improved_mana_gem -> check() )
  {
    sp += resource_max[ RESOURCE_MANA ] * talents.improved_mana_gem->sd->effect1->base_value/100.0;
  }

  return sp;
}

// mage_t::composite_mastery ================================================

double mage_t::composite_mastery() SC_CONST
{
  double m = player_t::composite_mastery();

  m += specializations.frost2;

  return m;
}

// mage_t::composite_spell_crit =============================================

double mage_t::composite_spell_crit() SC_CONST
{
  double c = player_t::composite_spell_crit();

  if ( buffs_molten_armor -> up() )
  {
    c += spells.molten_armor -> effect3 -> base_value / 100.0 + glyphs.molten_armor -> effect_base_value( 1 ) / 100.0;
  }

  if ( buffs_focus_magic_feedback -> up() ) c += 0.03;

  return c;
}

// mage_t::matching_gear_multiplier =========================================

double mage_t::matching_gear_multiplier( const attribute_type attr ) SC_CONST
{
  if ( attr == ATTR_INTELLECT )
    return 0.05;

  return 0.0;
}

// mage_t::combat_begin =====================================================

void mage_t::combat_begin()
{
  player_t::combat_begin();

  if ( talents.arcane_tactics -> rank() ) sim -> auras.arcane_tactics -> trigger();
}

// mage_t::reset ============================================================

void mage_t::reset()
{
  player_t::reset();
  active_water_elemental = 0;
  rotation.reset();
  mana_gem_charges = 3;
}

// mage_t::regen  ===========================================================

void mage_t::regen( double periodicity )
{
  mana_regen_while_casting  = 0;

  player_t::regen( periodicity );

  if ( buffs_mage_armor -> up() )
  {
    double gain_amount = resource_max[ RESOURCE_MANA ] * spells.mage_armor -> effect2 -> base_value / 100.0;
    gain_amount *= periodicity / 5.0;
    gain_amount *= 1.0 + glyphs.mage_armor -> effect_base_value( 1 ) / 100.0;

    resource_gain( RESOURCE_MANA, gain_amount, gains_mage_armor );
  }

  uptimes_water_elemental -> update( active_water_elemental != 0 );
}

// mage_t::resource_gain ====================================================

double mage_t::resource_gain( int       resource,
                              double    amount,
                              gain_t*   source,
                              action_t* action )
{
  double actual_amount = player_t::resource_gain( resource, amount, source, action );

  if ( resource == RESOURCE_MANA )
  {
    if ( source != gains_evocation &&
         source != gains_mana_gem )
    {
      rotation.mana_gain += actual_amount;
    }
  }

  return actual_amount;
}

// mage_t::resource_loss ====================================================

double mage_t::resource_loss( int       resource,
                              double    amount,
                              action_t* action )
{
  double actual_amount = player_t::resource_loss( resource, amount, action );

  if ( resource == RESOURCE_MANA )
  {
    if ( rotation.current == ROTATION_DPS )
    {
      rotation.dps_mana_loss += actual_amount;
    }
    else if ( rotation.current == ROTATION_DPM )
    {
      rotation.dpm_mana_loss += actual_amount;
    }
  }

  return actual_amount;
}

// mage_t::create_expression ================================================

action_expr_t* mage_t::create_expression( action_t* a, const std::string& name_str )
{
  if ( name_str == "dps" )
  {
    struct dps_rotation_expr_t : public action_expr_t
    {
      dps_rotation_expr_t( action_t* a ) : action_expr_t( a, "dps_rotation", TOK_NUM ) {}
      virtual int evaluate() { result_num = ( action -> player -> cast_mage() -> rotation.current == ROTATION_DPS ) ? 1.0 : 0.0; return TOK_NUM; }
    };
    return new dps_rotation_expr_t( a );
  }
  else if ( name_str == "dpm" )
  {
    struct dpm_rotation_expr_t : public action_expr_t
    {
      dpm_rotation_expr_t( action_t* a ) : action_expr_t( a, "dpm_rotation", TOK_NUM ) {}
      virtual int evaluate() { result_num = ( action -> player -> cast_mage() -> rotation.current == ROTATION_DPM ) ? 1.0 : 0.0; return TOK_NUM; }
    };
    return new dpm_rotation_expr_t( a );
  }

  return player_t::create_expression( a, name_str );
}

// mage_t::create_options ===================================================

void mage_t::create_options()
{
  player_t::create_options();

  option_t mage_options[] =
  {
    { "focus_magic_target",  OPT_STRING, &( focus_magic_target_str ) },
    { "max_ignite_refresh",  OPT_FLT,    &( max_ignite_refresh     ) },
    { NULL, OPT_UNKNOWN, NULL }
  };

  option_t::copy( options, mage_options );
}

// mage_t::create_profile ===================================================

bool mage_t::create_profile( std::string& profile_str, int save_type, bool save_html )
{
  player_t::create_profile( profile_str, save_type, save_html );

  if ( save_type == SAVE_ALL )
  {
    if ( ! focus_magic_target_str.empty() ) profile_str += "focus_magic_target=" + focus_magic_target_str + "\n";
  }

  return true;
}

// mage_t::copy_from ===================================================

void mage_t::copy_from( player_t* source )
{
  player_t::copy_from( source );
  focus_magic_target_str = source -> cast_mage() -> focus_magic_target_str;
  max_ignite_refresh     = source -> cast_mage() -> max_ignite_refresh;
}

// mage_t::decode_set =======================================================

int mage_t::decode_set( item_t& item )
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

  if ( strstr( s, "bloodmage"   ) ) return SET_T10_CASTER;
  if ( strstr( s, "firelord"    ) ) return SET_T11_CASTER;

  return SET_NONE;
}

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

// player_t::create_mage  ===================================================

player_t* player_t::create_mage( sim_t* sim, const std::string& name, race_type r )
{
  return new mage_t( sim, name, r );
}

// player_t::mage_init ======================================================

void player_t::mage_init( sim_t* sim )
{
  sim -> auras.arcane_tactics = new aura_t( sim, "arcane_tactics", 1, 0.0 );

  for ( unsigned int i = 0; i < sim -> actor_list.size(); i++ )
  {
    player_t* p = sim -> actor_list[i];
    p -> buffs.arcane_brilliance = new stat_buff_t( p, "arcane_brilliance", STAT_MANA, p -> level < MAX_LEVEL ? p -> player_data.effect_min( 79058, p -> level, E_APPLY_AURA, A_MOD_INCREASE_ENERGY ) : 0, !p -> is_pet() );
    p -> buffs.focus_magic       = new      buff_t( p, 54646, "focus_magic" );
    p -> debuffs.critical_mass   = new debuff_t( p, 22959, "critical_mass" );
    p -> debuffs.slow            = new debuff_t( p, 31589, "slow" );
  }

}

// player_t::mage_combat_begin ==============================================

void player_t::mage_combat_begin( sim_t* sim )
{
  for ( player_t* p = sim -> player_list; p; p = p -> next )
  {
    if ( p -> ooc_buffs() )
    {
      if ( sim -> overrides.arcane_brilliance ) p -> buffs.arcane_brilliance -> override( 1, p -> player_data.effect_min( 79058, p -> level, E_APPLY_AURA, A_MOD_INCREASE_ENERGY ) );
      if ( sim -> overrides.focus_magic       ) p -> buffs.focus_magic       -> override();
    }
  }

  for ( target_t* t = sim -> target_list; t; t = t -> next )
  {
    if ( sim -> overrides.critical_mass ) t -> debuffs.critical_mass -> override( 1 );
  }

  if ( sim -> overrides.arcane_tactics ) sim -> auras.arcane_tactics -> override( 1 );
}


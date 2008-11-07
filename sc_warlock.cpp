// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simcraft.h"

// ==========================================================================
// Warlock
// ==========================================================================

struct warlock_pet_t;

struct warlock_t : public player_t
{
  // Active
  warlock_pet_t* active_pet;
  action_t*      active_corruption;
  action_t*      active_curse;
  action_t*      active_immolate;
  action_t*      active_pandemic;
  int8_t         active_dots;

  // Buffs
  int8_t buffs_backdraft;
  int8_t buffs_demon_armor;
  int8_t buffs_demonic_empathy;
  int8_t buffs_empowered_imp;
  double buffs_eradication;
  double buffs_fel_armor;
  int8_t buffs_flame_shadow;
  int8_t buffs_haunted;
  int8_t buffs_metamorphosis;
  double buffs_molten_core;
  int8_t buffs_pet_sacrifice;
  int8_t buffs_shadow_embrace;
  int8_t buffs_shadow_flame;
  int8_t buffs_shadow_trance;
  double buffs_shadow_vulnerability;
  int8_t buffs_shadow_vulnerability_charges;

  // Cooldowns
  double cooldowns_eradication;

  // Expirations
  event_t* expirations_backdraft;
  event_t* expirations_demonic_empathy;
  event_t* expirations_empowered_imp;
  event_t* expirations_flame_shadow;
  event_t* expirations_haunted;
  event_t* expirations_molten_core;
  event_t* expirations_shadow_embrace;
  event_t* expirations_shadow_flame;
  event_t* expirations_shadow_vulnerability;

  // Gains
  gain_t* gains_dark_pact;
  gain_t* gains_felhunter;
  gain_t* gains_life_tap;
  gain_t* gains_sacrifice;
  gain_t* gains_soul_leech;

  // Procs
  proc_t* procs_dark_pact;
  proc_t* procs_life_tap;
  proc_t* procs_shadow_trance;

  // Up-Times
  uptime_t* uptimes_backdraft;
  uptime_t* uptimes_demonic_empathy;
  uptime_t* uptimes_empowered_imp;
  uptime_t* uptimes_eradication;
  uptime_t* uptimes_molten_core;
  uptime_t* uptimes_shadow_flame;
  uptime_t* uptimes_flame_shadow;
  uptime_t* uptimes_shadow_trance;
  uptime_t* uptimes_shadow_vulnerability;

  struct talents_t
  {
    int8_t  amplify_curse;
    int8_t  backdraft;
    int8_t  backlash;
    int8_t  bane;
    int8_t  cataclysm;
    int8_t  chaos_bolt;
    int8_t  conflagrate;
    int8_t  contagion;
    int8_t  dark_pact;
    int8_t  deaths_embrace;
    int8_t  demonic_aegis;
    int8_t  demonic_brutality;
    int8_t  demonic_embrace;
    int8_t  demonic_empathy;
    int8_t  demonic_empowerment;
    int8_t  demonic_knowledge;
    int8_t  demonic_pact;
    int8_t  demonic_power;
    int8_t  demonic_sacrifice;
    int8_t  demonic_tactics;
    int8_t  destructive_reach;
    int8_t  devastation;
    int8_t  emberstorm;
    int8_t  empowered_corruption;
    int8_t  empowered_imp;
    int8_t  eradication;
    int8_t  everlasting_affliction;
    int8_t  fel_domination;
    int8_t  fel_intellect;
    int8_t  fel_stamina;
    int8_t  fel_synergy;
    int8_t  fel_vitality;
    int8_t  fire_and_brimstone;
    int8_t  haunt;
    int8_t  improved_corruption;
    int8_t  improved_curse_of_agony;
    int8_t  improved_demonic_tactics;
    int8_t  improved_drain_soul;
    int8_t  improved_felhunter;
    int8_t  improved_fire_bolt;
    int8_t  improved_immolate;
    int8_t  improved_imp;
    int8_t  improved_lash_of_pain;
    int8_t  improved_life_tap;
    int8_t  improved_searing_pain;
    int8_t  improved_shadow_bolt;
    int8_t  improved_soul_leech;
    int8_t  improved_succubus;
    int8_t  improved_voidwalker;
    int8_t  malediction;
    int8_t  mana_feed;
    int8_t  master_conjuror;
    int8_t  master_demonologist;
    int8_t  master_summoner;
    int8_t  metamorphosis;
    int8_t  molten_core;
    int8_t  nightfall;
    int8_t  pandemic;
    int8_t  ruin;
    int8_t  shadow_and_flame;
    int8_t  shadow_burn;
    int8_t  shadow_embrace;
    int8_t  shadow_mastery;
    int8_t  siphon_life;
    int8_t  soul_leech;
    int8_t  soul_link;
    int8_t  soul_siphon;
    int8_t  summon_felguard;
    int8_t  suppression;
    int8_t  unholy_power;
    int8_t  unstable_affliction;
        
    talents_t() { memset( (void*) this, 0x0, sizeof( talents_t ) ); }
  };
  talents_t talents;

  struct glyphs_t
  {
    int8_t conflagrate;
    int8_t corruption;
    int8_t curse_of_agony;
    int8_t felguard;
    int8_t felhunter;
    int8_t immolate;
    int8_t imp;
    int8_t searing_pain;
    int8_t shadow_bolt;
    int8_t shadow_burn;
    int8_t siphon_life;
    glyphs_t() { memset( (void*) this, 0x0, sizeof( glyphs_t ) ); }
  };
  glyphs_t glyphs;

  warlock_t( sim_t* sim, std::string& name ) : player_t( sim, WARLOCK, name ) 
  {
    // Active
    active_pet        = 0;
    active_corruption = 0;
    active_curse      = 0;
    active_immolate   = 0;
    active_pandemic   = 0;
    active_dots       = 0;

    // Buffs
    buffs_backdraft                    = 0;
    buffs_demon_armor                  = 0;
    buffs_demonic_empathy              = 0;
    buffs_empowered_imp                = 0;
    buffs_eradication                  = 0;
    buffs_fel_armor                    = 0;
    buffs_flame_shadow                 = 0;
    buffs_haunted                      = 0;
    buffs_metamorphosis                = 0;
    buffs_molten_core                  = 0;
    buffs_pet_sacrifice                = 0;
    buffs_shadow_embrace               = 0;
    buffs_shadow_flame                 = 0;
    buffs_shadow_trance                = 0;
    buffs_shadow_vulnerability         = 0;
    buffs_shadow_vulnerability_charges = 0;

    // Cooldowns
    cooldowns_eradication              = 0;

    // Expirations
    expirations_backdraft              = 0;
    expirations_demonic_empathy        = 0;
    expirations_empowered_imp          = 0;
    expirations_flame_shadow           = 0;
    expirations_haunted                = 0;
    expirations_molten_core            = 0;
    expirations_shadow_embrace         = 0;
    expirations_shadow_flame           = 0;
    expirations_shadow_vulnerability   = 0;

    // Gains
    gains_dark_pact  = get_gain( "dark_pact"  );
    gains_felhunter  = get_gain( "felhunter"  );
    gains_life_tap   = get_gain( "life_tap"   );
    gains_sacrifice  = get_gain( "sacrifice"  );
    gains_soul_leech = get_gain( "soul_leech" );

    // Procs
    procs_dark_pact     = get_proc( "dark_pact" );
    procs_life_tap      = get_proc( "life_tap" );
    procs_shadow_trance = get_proc( "shadow_trance" );

    // Up-Times
    uptimes_backdraft            = get_uptime( "backdraft"            );
    uptimes_demonic_empathy      = get_uptime( "demonic_empathy"      );
    uptimes_empowered_imp        = get_uptime( "empowered_imp"        );
    uptimes_eradication          = get_uptime( "eradication"          );
    uptimes_flame_shadow         = get_uptime( "flame_shadow"         );
    uptimes_molten_core          = get_uptime( "molten_core"          );
    uptimes_shadow_flame         = get_uptime( "shadow_flame"         );
    uptimes_shadow_trance        = get_uptime( "shadow_trance"        );
    uptimes_shadow_vulnerability = get_uptime( "shadow_vulnerability" );
  }

  // Character Definition
  virtual void      init_base();
  virtual void      reset();
  virtual bool      get_talent_trees( std::vector<int8_t*>& affliction, std::vector<int8_t*>& demonology, std::vector<int8_t*>& destruction );
  virtual bool      parse_talents_mmo( const std::string& talent_string );
  virtual bool      parse_option ( const std::string& name, const std::string& value );
  virtual action_t* create_action( const std::string& name, const std::string& options );
  virtual pet_t*    create_pet   ( const std::string& name );
  virtual int       primary_resource() { return RESOURCE_MANA; }
  virtual double    composite_spell_power( int8_t school );

  // Event Tracking
  virtual void regen( double periodicity );
};

// ==========================================================================
// Warlock Spell
// ==========================================================================

struct warlock_spell_t : public spell_t
{
  warlock_spell_t( const char* n, player_t* player, int8_t s, int8_t t ) : 
    spell_t( n, player, RESOURCE_MANA, s, t ) 
  {
  }

  // Overridden Methods
  virtual double haste();
  virtual double execute_time();
  virtual void   player_buff();
  virtual void   target_debuff( int8_t dmg_type );
  virtual void   execute();
  virtual void   tick();

  // Passthru Methods
  virtual double cost()                              { return spell_t::cost();                  }
  virtual void   last_tick()                         { spell_t::last_tick();                    }
  virtual void   assess_damage( double a, int8_t d ) { spell_t::assess_damage( a, d );          }
  virtual bool   ready()                             { return spell_t::ready();                 }
};

// ==========================================================================
// Warlock Pet
// ==========================================================================

enum pet_type_t { PET_NONE=0, PET_FELGUARD, PET_FELHUNTER, PET_IMP, PET_VOIDWALKER, PET_SUCCUBUS };

struct warlock_pet_t : public pet_t
{
  int8_t pet_type;
  int8_t buffs_demonic_empathy;
  int8_t buffs_demonic_empowerment;
  event_t* expirations_demonic_empathy;
  event_t* expirations_demonic_pact;

  warlock_pet_t( sim_t* sim, player_t* owner, const std::string& pet_name, int8_t pt ) :
    pet_t( sim, owner, pet_name ), pet_type(pt)
  {
    buffs_demonic_empathy = 0;
    buffs_demonic_empowerment = 0;
    expirations_demonic_empathy = 0;
    expirations_demonic_pact = 0;
  }

  virtual void init_base()
  {
    warlock_t* o = owner -> cast_warlock();

    attribute_base[ ATTR_STRENGTH  ] = 314;
    attribute_base[ ATTR_AGILITY   ] =  90;
    attribute_base[ ATTR_STAMINA   ] = 328;
    attribute_base[ ATTR_INTELLECT ] = 150;
    attribute_base[ ATTR_SPIRIT    ] = 209;

    attribute_multiplier_initial[ ATTR_STAMINA   ] *= 1.0 + ( o -> talents.fel_stamina  * 0.03 +
							      o -> talents.fel_vitality * 0.05 );

    attribute_multiplier_initial[ ATTR_INTELLECT ] *= 1.0 + ( o -> talents.fel_intellect * 0.03 +
							      o -> talents.fel_vitality  * 0.05 );

      stamina_per_owner += o -> talents.fel_synergy * 0.05;
    intellect_per_owner += o -> talents.fel_synergy * 0.05;

    initial_attack_power_per_strength = 2.0;
    initial_attack_crit_per_agility   = rating_t::interpolate( level, 0.01/16.0, 0.01/24.9, 0.01/52.1 );

    health_per_stamina = 10;
    mana_per_intellect = 15;
  }

  virtual void summon()
  {
    warlock_t* o = owner -> cast_warlock();
    pet_t::summon();
    o -> active_pet = this;

    buffs_demonic_empathy = 0;
    buffs_demonic_empowerment = 0;
    expirations_demonic_empathy = 0;
    expirations_demonic_pact = 0;
  }

  void adjust_base_modifiers( action_t* a )
  {
    warlock_t* o = owner -> cast_warlock();

    a -> base_crit        += o -> talents.demonic_tactics * 0.02;
    a -> base_multiplier  *= 1.0 + o -> talents.unholy_power * 0.04;

    if( pet_type == PET_FELGUARD )
    {
      a -> base_multiplier *= 1.0 + o -> talents.master_demonologist * 0.01;
    }
    else if( pet_type == PET_IMP )
    {
      if( a -> school == SCHOOL_FIRE ) 
      {
	a -> base_multiplier *= 1.0 + o -> talents.master_demonologist * 0.01;
	a -> base_crit       +=       o -> talents.master_demonologist * 0.01;
      }
    }
    else if( pet_type == PET_SUCCUBUS )
    {
      if( a -> school == SCHOOL_SHADOW ) 
      {
	a -> base_multiplier *= 1.0 + o -> talents.master_demonologist * 0.01;
	a -> base_crit       +=       o -> talents.master_demonologist * 0.01;
      }
    }
  }

  void adjust_player_modifiers( action_t* a )
  {
    warlock_t* o = owner -> cast_warlock();
    if( o -> talents.improved_demonic_tactics )
    {
      a -> player_crit += o -> composite_spell_crit() * o -> talents.improved_demonic_tactics * 0.10;
    }
  }

  virtual void attack_hit_event( attack_t* a );
  virtual void  spell_hit_event( spell_t* s );
};

// ==========================================================================
// Warlock Pet Melee
// ==========================================================================

struct warlock_pet_melee_t : public attack_t
{
  warlock_pet_melee_t( player_t* player, const char* name ) : 
    attack_t( name, player, RESOURCE_NONE, SCHOOL_PHYSICAL )
  {
    warlock_pet_t* p = (warlock_pet_t*) player -> cast_pet();

    weapon = &( p -> main_hand_weapon );
    base_execute_time = weapon -> swing_time;
    base_direct_dmg   = 1;
    background        = true;
    repeating         = true;

    p -> adjust_base_modifiers( this );
  }

  virtual void player_buff()
  {
    warlock_pet_t* p = (warlock_pet_t*) player -> cast_pet();
    warlock_t* o = p -> owner -> cast_warlock();
    attack_t::player_buff();
    player_power += 0.57 * o -> composite_spell_power( SCHOOL_MAX );
    p -> adjust_player_modifiers( this );
  }
};

// ==========================================================================
// Warlock Pet Attack
// ==========================================================================

struct warlock_pet_attack_t : public attack_t
{
  warlock_pet_attack_t( const char* n, player_t* player, int8_t r=RESOURCE_MANA, int8_t s=SCHOOL_PHYSICAL ) : 
    attack_t( n, player, r, s )
  {
    warlock_pet_t* p = (warlock_pet_t*) player -> cast_pet();
    p -> adjust_base_modifiers( this );
  }

  virtual void execute()
  {
    warlock_pet_t* p = (warlock_pet_t*) player -> cast_pet();
    if( p -> buffs_demonic_empathy > 0 )
    {
      p -> buffs_demonic_empathy--;
      if( p -> buffs_demonic_empathy == 0 ) event_t::early( p -> expirations_demonic_empathy );
    }
    attack_t::execute();
  }

  virtual void player_buff()
  {
    warlock_pet_t* p = (warlock_pet_t*) player -> cast_pet();
    warlock_t* o = p -> owner -> cast_warlock();
    attack_t::player_buff();
    player_power += 0.57 * player -> cast_pet() -> owner -> composite_spell_power( SCHOOL_MAX );
    if( p -> buffs_demonic_empathy ) player_multiplier *= 1.0 + o -> talents.demonic_empathy * 0.01;
    p -> adjust_player_modifiers( this );
  }
};

// ==========================================================================
// Warlock Pet Spell
// ==========================================================================

struct warlock_pet_spell_t : public spell_t
{
  warlock_pet_spell_t( const char* n, player_t* player, int8_t r=RESOURCE_MANA, int8_t s=SCHOOL_SHADOW ) : 
    spell_t( n, player, r, s )
  {
    warlock_pet_t* p = (warlock_pet_t*) player -> cast_pet();
    p -> adjust_base_modifiers( this );
  }

  virtual void execute()
  {
    warlock_pet_t* p = (warlock_pet_t*) player -> cast_pet();
    if( p -> buffs_demonic_empathy > 0 )
    {
      p -> buffs_demonic_empathy--;
      if( p -> buffs_demonic_empathy == 0 ) event_t::early( p -> expirations_demonic_empathy );
    }
    spell_t::execute();
  }

  virtual void player_buff()
  {
    warlock_pet_t* p = (warlock_pet_t*) player -> cast_pet();
    warlock_t* o = p -> owner -> cast_warlock();
    spell_t::player_buff();
    player_power += 0.15 * player -> cast_pet() -> owner -> composite_spell_power( SCHOOL_MAX );
    if( p -> buffs_demonic_empathy ) player_multiplier *= 1.0 + o -> talents.demonic_empathy * 0.01;
    p -> adjust_player_modifiers( this );
  }
};

// ==========================================================================
// Pet Imp
// ==========================================================================

struct imp_pet_t : public warlock_pet_t
{
  struct fire_bolt_t : public warlock_pet_spell_t
  {
    fire_bolt_t( player_t* player ):
      warlock_pet_spell_t( "fire_bolt", player, RESOURCE_MANA, SCHOOL_FIRE ) 
    {
      imp_pet_t* p = (imp_pet_t*) player -> cast_pet();
      warlock_t* o = p -> owner -> cast_warlock();

      static rank_t ranks[] =
      {
	{ 78, 8, 212, 212, 0, 180 },
	{ 68, 8, 110, 124, 0, 145 },
	{ 58, 7,  83,  93, 0, 115 },
	{ 0,  0 }
      };
      rank = choose_rank( ranks );

      base_cost         = 0;
      base_execute_time = 2.5;
      direct_power_mod  = ( 2.0 / 3.5 );
      may_crit          = true;

      base_execute_time -= 0.25 * ( o -> talents.improved_fire_bolt + o -> talents.demonic_power );

      base_multiplier *= 1.0 + ( o -> talents.improved_imp  * 0.10 +
				 o -> talents.empowered_imp * 0.05 +
				 o -> glyphs.imp            * 0.10 );
    }
    virtual void player_buff()
    {
      imp_pet_t* p = (imp_pet_t*) player -> cast_pet();
      warlock_pet_spell_t::player_buff();
      if( p -> buffs_demonic_empowerment ) player_crit += 0.20;
    }
  };

  imp_pet_t( sim_t* sim, player_t* owner, const std::string& pet_name ) :
    warlock_pet_t( sim, owner, pet_name, PET_IMP )
  {
  }

  virtual void init_base()
  {
    warlock_pet_t::init_base();

    attribute_base[ ATTR_STRENGTH  ] = 297;
    attribute_base[ ATTR_AGILITY   ] =  79;
    attribute_base[ ATTR_STAMINA   ] = 118;
    attribute_base[ ATTR_INTELLECT ] = 369;
    attribute_base[ ATTR_SPIRIT    ] = 357;

    resource_base[ RESOURCE_HEALTH ] = 2719;
    resource_base[ RESOURCE_MANA   ] = 150;

    health_per_stamina = 5;
    mana_per_intellect = 7.5;
  }

  virtual void summon()
  {
    warlock_pet_t::summon();
    schedule_ready();
  }

  virtual action_t* create_action( const std::string& name,
				   const std::string& options_str )
  {
    if( name == "fire_bolt" ) return new fire_bolt_t( this );

    return player_t::create_action( name, options_str );
  }

  virtual void spell_hit_event( spell_t* s );
};

// ==========================================================================
// Pet Felguard
// ==========================================================================

struct felguard_pet_t : public warlock_pet_t
{
  int8_t buffs_demonic_frenzy;

  struct cleave_t : public warlock_pet_attack_t
  {
    cleave_t( player_t* player ) : 
      warlock_pet_attack_t( "cleave", player, RESOURCE_MANA, SCHOOL_PHYSICAL )
    {
      felguard_pet_t* p = (felguard_pet_t*) player -> cast_pet();
      weapon   = &( p -> main_hand_weapon );
      cooldown = 6.0;
      base_direct_dmg = ( p -> level < 68 ? 50 :
			  p -> level < 80 ? 78 : 124 );
    }
    virtual void player_buff()
    {
      felguard_pet_t* p = (felguard_pet_t*) player -> cast_pet();
      warlock_t*      o = p -> owner -> cast_warlock();

      warlock_pet_attack_t::player_buff();
      player_power *= 1.0 + p -> buffs_demonic_frenzy * ( 0.05 + o -> talents.demonic_brutality * 0.01 );
      if( o -> glyphs.felguard ) player_power *= 1.10;
    }
  };

  struct melee_t : public warlock_pet_melee_t
  {
    melee_t( player_t* player ) : 
      warlock_pet_melee_t( player, "felguard_melee" )
    {
      felguard_pet_t* p = (felguard_pet_t*) player -> cast_pet();
      warlock_t*      o = p -> owner -> cast_warlock();

      if( o -> glyphs.felguard ) base_multiplier *= 1.20;
    }
    virtual double execute_time()
    {
      felguard_pet_t* p = (felguard_pet_t*) player -> cast_pet();
      double t = attack_t::execute_time();
      if( p -> buffs_demonic_empowerment ) t *= 1.0 / 1.20;
      return t;
    }
    virtual void player_buff()
    {
      felguard_pet_t* p = (felguard_pet_t*) player -> cast_pet();
      warlock_t*      o = p -> owner -> cast_warlock();

      warlock_pet_melee_t::player_buff();
      player_power *= 1.0 + p -> buffs_demonic_frenzy * ( 0.05 + o -> talents.demonic_brutality * 0.01 );
    }
    virtual void assess_damage( double amount, int8_t dmg_type )
    {
      felguard_pet_t* p = (felguard_pet_t*) player -> cast_pet();

      attack_t::assess_damage( amount, dmg_type );
      if( p -> buffs_demonic_frenzy < 10 ) p -> buffs_demonic_frenzy++;
    }    
  };

  melee_t* melee;

  felguard_pet_t( sim_t* sim, player_t* owner, const std::string& pet_name ) :
    warlock_pet_t( sim, owner, pet_name, PET_FELGUARD ), melee(0)
  {
    main_hand_weapon.type       = WEAPON_BEAST;
    main_hand_weapon.damage     = 130;
    main_hand_weapon.swing_time = 2.0;

    buffs_demonic_frenzy = 0;
  }
  virtual void init_base()
  {
    warlock_pet_t::init_base();

    resource_base[ RESOURCE_HEALTH ] = 1627;
    resource_base[ RESOURCE_MANA   ] = 3331;

    base_attack_power = +20;

    melee = new melee_t( this );
  }
  virtual void reset()
  {
    pet_t::reset();
    buffs_demonic_frenzy = 0;
  }
  virtual void summon()
  {
    warlock_pet_t::summon();
    melee -> schedule_execute(); 
    schedule_ready();
  }
  virtual action_t* create_action( const std::string& name,
				   const std::string& options_str )
  {
    if( name == "cleave" ) return new cleave_t( this );

    return player_t::create_action( name, options_str );
  }
};

// ==========================================================================
// Pet Felhunter
// ==========================================================================

struct felhunter_pet_t : public warlock_pet_t
{
  struct shadow_bite_t : public warlock_pet_attack_t
  {
    shadow_bite_t( player_t* player ) : 
      warlock_pet_attack_t( "shadow_bite", player, RESOURCE_MANA, SCHOOL_SHADOW )
    {
      felguard_pet_t* p = (felguard_pet_t*) player -> cast_pet();
      cooldown = 6.0;
      base_direct_dmg = ( p -> level < 66 ?  88 :
			  p -> level < 74 ? 101 : 118 );
    }
    virtual void player_buff()
    {
      felhunter_pet_t* p = (felhunter_pet_t*) player -> cast_pet();
      warlock_t*      o = p -> owner -> cast_warlock();
      warlock_pet_attack_t::player_buff();
      player_multiplier *= 1.0 + o -> active_dots * 0.05;;
      if( o -> glyphs.felhunter ) player_crit += 0.06;
    }
  };

  warlock_pet_melee_t* melee;

  felhunter_pet_t( sim_t* sim, player_t* owner, const std::string& pet_name ) :
    warlock_pet_t( sim, owner, pet_name, PET_FELHUNTER ), melee(0)
  {
    main_hand_weapon.type       = WEAPON_BEAST;
    main_hand_weapon.damage     = 70;
    main_hand_weapon.swing_time = 2.0;
  }
  virtual void init_base()
  {
    warlock_pet_t::init_base();

    base_attack_power = -20;

    resource_base[ RESOURCE_HEALTH ] = 947;
    resource_base[ RESOURCE_MANA   ] = 940;

    melee = new warlock_pet_melee_t( this, "felhunter_melee" );
  }
  virtual void summon()
  {
    warlock_pet_t::summon();
    melee -> schedule_execute();
    schedule_ready();
  }
  virtual action_t* create_action( const std::string& name,
				   const std::string& options_str )
  {
    if( name == "shadow_bite" ) return new shadow_bite_t( this );

    return player_t::create_action( name, options_str );
  }
};

// ==========================================================================
// Pet Succubus
// ==========================================================================

struct succubus_pet_t : public warlock_pet_t
{
  struct lash_of_pain_t : public warlock_pet_attack_t
  {
    lash_of_pain_t( player_t* player ) : 
      warlock_pet_attack_t( "lash_of_pain", player, RESOURCE_MANA, SCHOOL_SHADOW )
    {
      felguard_pet_t* p = (felguard_pet_t*) player -> cast_pet();
      warlock_t*      o = p -> owner -> cast_warlock();

      cooldown   = 12.0;
      base_direct_dmg = ( p -> level < 68 ?  99 :
			  p -> level < 74 ? 123 : 
			  p -> level < 80 ? 193 : 237 );
      cooldown  -= 3.0 * ( o -> talents.improved_lash_of_pain + o -> talents.demonic_power );
    }
  };

  warlock_pet_melee_t* melee;

  succubus_pet_t( sim_t* sim, player_t* owner, const std::string& pet_name ) :
    warlock_pet_t( sim, owner, pet_name, PET_SUCCUBUS ), melee(0)
  {
    main_hand_weapon.type       = WEAPON_BEAST;
    main_hand_weapon.damage     = 120;
    main_hand_weapon.swing_time = 2.0;
  }
  virtual void init_base()
  {
    warlock_pet_t::init_base();

    base_attack_power = -20;

    resource_base[ RESOURCE_HEALTH ] = 721;
    resource_base[ RESOURCE_MANA   ] = 940;

    melee = new warlock_pet_melee_t( this, "succubus_melee" );
  }
  virtual void summon()
  {
    warlock_pet_t::summon();
    melee -> schedule_execute();
    schedule_ready();
  }
  virtual action_t* create_action( const std::string& name,
				   const std::string& options_str )
  {
    if( name == "lash_of_pain" ) return new lash_of_pain_t( this );

    return player_t::create_action( name, options_str );
  }
};

namespace { // ANONYMOUS NAMESPACE ==========================================

// trigger_tier5_4pc ========================================================

static void trigger_tier5_4pc( spell_t*  s,
			       action_t* dot_spell )
{
  warlock_t* p = s -> player -> cast_warlock();

  if( p -> gear.tier5_4pc )
  {
    if( dot_spell )
    {
      dot_spell -> base_tick_dmg *= 1.10;
    }
  }
}

// trigger_tier4_2pc ========================================================

static void trigger_tier4_2pc( spell_t* s )
{
  struct expiration_t : public event_t
  {
    int8_t school;
    expiration_t( sim_t* sim, warlock_t* p, spell_t* s ) : event_t( sim, p ), school( s -> school )
    {
      name = "Tier4 2-Piece Buff Expiration";
      if( school == SCHOOL_SHADOW )
      {
	p -> aura_gain( "flame_shadow" );
	p -> buffs_flame_shadow = 1;
      }
      else
      {
	p -> aura_gain( "shadow_flame" );
	p -> buffs_shadow_flame = 1;
      }
      sim -> add_event( this, 15.0 );
    }
    virtual void execute()
    {
      warlock_t* p = player -> cast_warlock();
      if( school == SCHOOL_SHADOW )
      {
	p -> aura_loss( "flame_shadow" );
	p -> buffs_flame_shadow = 0;
	p -> expirations_flame_shadow = 0;
      }
      else
      {
	p -> aura_loss( "shadow_flame" );
	p -> buffs_shadow_flame = 0;
	p -> expirations_shadow_flame = 0;
      }
    }
  };
  
  warlock_t* p = s -> player -> cast_warlock();

  if( p -> gear.tier4_2pc )
  {
    if( s -> sim -> roll( 0.05 ) )
    {
      event_t*& e = ( s -> school == SCHOOL_SHADOW ) ? p -> expirations_shadow_flame : p -> expirations_flame_shadow;

      if( e )
      {
	e -> reschedule( 15.0 );
      }
      else
      {
	e = new ( s -> sim ) expiration_t( s -> sim, p, s );
      }
    }
  }
}

// decrement_shadow_vulnerability ===========================================

static void decrement_shadow_vulnerability( warlock_t* p )
{
  if( p -> buffs_shadow_vulnerability_charges > 0 )
  {
    p -> buffs_shadow_vulnerability_charges--;
    if( p -> buffs_shadow_vulnerability_charges == 0 )
    {
      event_t::early( p -> expirations_shadow_vulnerability );
    }
  }
}

// trigger_shadow_vulnerability =============================================

static void trigger_shadow_vulnerability( spell_t* s )
{
  struct expiration_t : public event_t
  {
    expiration_t( sim_t* sim, warlock_t* p ) : event_t( sim, p )
    {
      name = "Shadow Vulnerability Expiration";
      p -> aura_gain( "Shadow Vulnerability" );
      p -> buffs_shadow_vulnerability = sim -> current_time;
      sim -> add_event( this, 12.0 );
    }
    virtual void execute()
    {
      warlock_t* p = player -> cast_warlock();
      p -> aura_loss( "Shadow Vulnerability" );
      p -> buffs_shadow_vulnerability = 0;
      p -> buffs_shadow_vulnerability_charges = 0;
      p -> expirations_shadow_vulnerability = 0;
    }
  };
  
  warlock_t* p = s -> player -> cast_warlock();

  if( p -> talents.improved_shadow_bolt )
  {
    p -> buffs_shadow_vulnerability_charges = 4;

    event_t*& e = p -> expirations_shadow_vulnerability;

    if( e )
    {
      e -> reschedule( 12.0 );
    }
    else
    {
      e = new ( s -> sim ) expiration_t( s -> sim, p );
    }
  }
}

// stack_shadow_embrace =====================================================

static void stack_shadow_embrace( spell_t* s )
{
  struct expiration_t : public event_t
  {
    expiration_t( sim_t* sim, warlock_t* p ) : event_t( sim, p )
    {
      name = "Shadow Embrace Expiration";
      p -> aura_gain( "Shadow Embrace" );
      sim -> target -> debuffs.affliction_effects++;
      sim -> add_event( this, 12.0 );
    }
    virtual void execute()
    {
      warlock_t* p = player -> cast_warlock();
      p -> aura_loss( "Shadow Embrace" );
      p -> buffs_shadow_embrace = 0;
      p -> expirations_shadow_embrace = 0;
      sim -> target -> debuffs.affliction_effects--;
    }
  };
  
  warlock_t* p = s -> player -> cast_warlock();

  if( p -> talents.shadow_embrace )
  {
    if( p -> buffs_shadow_embrace < 2 ) p -> buffs_shadow_embrace++;

    event_t*& e = p -> expirations_shadow_embrace;

    if( e )
    {
      e -> reschedule( 12.0 );
    }
    else
    {
      e = new ( s -> sim ) expiration_t( s -> sim, p );
    }
  }
}

// trigger_nightfall ========================================================

static void trigger_nightfall( spell_t* s )
{
  warlock_t* p = s -> player -> cast_warlock();

  if( p -> talents.nightfall && ! p -> buffs_shadow_trance )
  {
    if( s -> sim -> roll( 0.02 * p -> talents.nightfall ) )
    {
      p -> procs_shadow_trance -> occur();
      p -> aura_gain( "Shadow Trance" );
      p -> buffs_shadow_trance = 1;
    }
  }
}

// trigger_corruption_glyph =================================================

static void trigger_corruption_glyph( spell_t* s )
{
  warlock_t* p = s -> player -> cast_warlock();

  if( p -> glyphs.corruption && ! p -> buffs_shadow_trance )
  {
    if( s -> sim -> roll( 0.04 ) )
    {
      p -> procs_shadow_trance -> occur();
      p -> aura_gain( "Shadow Trance" );
      p -> buffs_shadow_trance = 1;
    }
  }
}

// trigger_soul_leech =======================================================

static void trigger_soul_leech( spell_t* s )
{
  warlock_t* p = s -> player -> cast_warlock();

  if( p -> talents.soul_leech )
  {
    if( s -> sim -> roll( 0.10 * p -> talents.soul_leech ) )
    {
      p -> resource_gain( RESOURCE_HEALTH, s -> direct_dmg * 0.20 );

      if( p -> talents.improved_soul_leech )
      {
	double amount = p -> resource_max[ RESOURCE_MANA ] * p -> talents.improved_soul_leech * 0.01;

	p -> resource_gain( RESOURCE_MANA, amount, p -> gains_soul_leech );
      }
    }
  }
}

// trigger_backdraft ========================================================

static void trigger_backdraft( spell_t* s )
{
  struct expiration_t : public event_t
  {
    expiration_t( sim_t* sim, warlock_t* p ) : event_t( sim, p )
    {
      name = "Backdraft Expiration";
      p -> aura_gain( "Backdraft" );
      sim -> add_event( this, 15.0 );
    }
    virtual void execute()
    {
      warlock_t* p = player -> cast_warlock();
      p -> aura_loss( "Backdraft" );
      p -> buffs_backdraft = 0;
      p -> expirations_backdraft = 0;
    }
  };
  
  warlock_t* p = s -> player -> cast_warlock();

  if( p -> talents.backdraft )
  {
    p -> buffs_backdraft = 3;

    event_t*& e = p -> expirations_backdraft;

    if( e )
    {
      e -> reschedule( 15.0 );
    }
    else
    {
      e = new ( s -> sim ) expiration_t( s -> sim, p );
    }
  }
}

// trigger_haunt ========================================================

static void trigger_haunted( spell_t* s )
{
  struct expiration_t : public event_t
  {
    expiration_t( sim_t* sim, warlock_t* p ) : event_t( sim, p )
    {
      name = "Haunt Expiration";
      p -> aura_gain( "Haunted" );
      p -> buffs_haunted = 1;
      sim -> target -> debuffs.affliction_effects++;
      sim -> add_event( this, 12.0 );
    }
    virtual void execute()
    {
      warlock_t* p = player -> cast_warlock();
      p -> aura_loss( "Haunted" );
      p -> buffs_haunted = 0;
      sim -> target -> debuffs.affliction_effects--;
      p -> expirations_haunted = 0;
    }
  };
  
  warlock_t* p = s -> player -> cast_warlock();
  event_t*&  e = p -> expirations_haunted;

  if( e )
  {
    e -> reschedule( 12.0 );
  }
  else
  {
    e = new ( s -> sim ) expiration_t( s -> sim, p );
  }
}

// trigger_molten_core =====================================================

static void trigger_molten_core( spell_t* s )
{
  struct expiration_t : public event_t
  {
    expiration_t( sim_t* sim, warlock_t* p ) : event_t( sim, p )
    {
      name = "Molten Core Expiration";
      p -> aura_gain( "Molten Core" );
      p -> buffs_molten_core = sim -> current_time;
      sim -> add_event( this, 12.0 );
    }
    virtual void execute()
    {
      warlock_t* p = player -> cast_warlock();
      p -> aura_loss( "Molten Core" );
      p -> buffs_molten_core = 0;
      p -> expirations_molten_core = 0;
    }
  };

  if( s -> school != SCHOOL_SHADOW ) return;
  
  warlock_t* p = s -> player -> cast_warlock();

  if( ! p -> talents.molten_core ) return;

  if( s -> sim -> roll( p -> talents.molten_core * 0.05 ) )
  {
    event_t*&  e = p -> expirations_molten_core;

    if( e )
    {
      e -> reschedule( 12.0 );
    }
    else
    {
      e = new ( s -> sim ) expiration_t( s -> sim, p );
    }
  }
}

// trigger_eradication =====================================================

static void trigger_eradication( spell_t* s )
{
  struct expiration_t : public event_t
  {
    expiration_t( sim_t* sim, warlock_t* p ) : event_t( sim, p )
    {
      name = "Eradication Expiration";
      p -> aura_gain( "Eradication" );
      p -> buffs_eradication = 1;
      sim -> add_event( this, 12.0 );
    }
    virtual void execute()
    {
      warlock_t* p = player -> cast_warlock();
      p -> aura_loss( "Eradication" );
      p -> buffs_eradication = 0;
    }
  };
  
  warlock_t* p = s -> player -> cast_warlock();

  if( ! p -> talents.eradication ) return;

  if( s -> sim -> cooldown_ready( p -> cooldowns_eradication ) )
  {
    if( s -> sim -> roll( p -> talents.eradication * 0.04 ) )
    {
      p -> cooldowns_eradication = s -> sim -> current_time + 30;
      new ( s -> sim ) expiration_t( s -> sim, p );
    }
  }
}

// trigger_deaths_embrace ===================================================

static int trigger_deaths_embrace( spell_t* s )
{
  warlock_t* p = s -> player -> cast_warlock();
  target_t*  t = s -> sim -> target;

  if( ! p -> talents.deaths_embrace ) return 0;

  // Target health recalculated halfway through fight duration.
  if( t -> initial_health <= 0 ) return 0;

  if( ( t -> current_health / t -> initial_health ) < 0.35 )
  {
    return p -> talents.deaths_embrace * 4;
  }
  
  return 0;
}

// trigger_everlasting_affliction ===========================================

static void trigger_everlasting_affliction( spell_t* s )
{
  warlock_t* p = s -> player -> cast_warlock();

  if( ! p -> talents.everlasting_affliction ) return;

  if( ! p -> active_corruption ) return;

  if( s -> sim -> roll( p -> talents.everlasting_affliction * 0.20 ) )
  {
    p -> active_corruption -> refresh_duration();
  }
}

// trigger_pandemic ========================================================

static void trigger_pandemic( spell_t* s )
{
  warlock_t* p = s -> player -> cast_warlock();

  if( ! p -> talents.pandemic ) return;

  struct pandemic_t : public spell_t
  {
    pandemic_t( player_t* player ) : 
      spell_t( "pandemic", player, RESOURCE_MANA, SCHOOL_SHADOW, TREE_AFFLICTION )
    {
      background = true;
      proc = true;
      trigger_gcd = 0;
      base_hit += player -> cast_warlock() -> talents.suppression * 0.01;
    }
    virtual void target_debuff( int8_t dmg_type ) 
    {
      spell_t::target_debuff( dmg_type );
      target_multiplier = 1.0;
    }
    virtual void player_buff()
    {
      spell_t::player_buff();
      player_multiplier = 1.0;
    }
  };

  // Currently, Pandemic takes player crit into account.

  if( s -> sim -> roll( s -> player_crit ) )
  {
    if( ! p -> active_pandemic ) p -> active_pandemic = new pandemic_t( p );

    p -> active_pandemic -> base_direct_dmg = s -> tick_dmg * p -> talents.pandemic / 3.0;
    p -> active_pandemic -> execute();
  }
}

// trigger_empowered_imp ====================================================

static void trigger_empowered_imp( spell_t* s )
{
  struct expiration_t : public event_t
  {
    expiration_t( sim_t* sim, warlock_t* o ) : event_t( sim, o )
    {
      name = "Empowered Imp Expiration";
      o -> aura_gain( "Empowered Imp" );
      o -> buffs_empowered_imp = 1;
      sim -> add_event( this, 8.0 );
    }
    virtual void execute()
    {
      warlock_t* o = player -> cast_warlock();
      o -> aura_loss( "Empowered Imp" );
      o -> buffs_empowered_imp = 0;
      o -> expirations_empowered_imp = 0;
    }
  };
  
  imp_pet_t* p = (imp_pet_t*) s -> player -> cast_pet();
  warlock_t* o = p -> owner -> cast_warlock();

  if( ! o -> talents.empowered_imp ) return;

  if( s -> sim -> roll( o -> talents.empowered_imp / 3.0 ) )
  {
    event_t*& e = o -> expirations_empowered_imp;

    if( e )
    {
      e -> reschedule( 8.0 );
    }
    else
    {
      e = new ( s -> sim ) expiration_t( s -> sim, o );
    }
  }
}

// trigger_demonic_empathy_on_owner ========================================

static void trigger_demonic_empathy_on_owner( action_t* a )
{
  struct expiration_t : public event_t
  {
    expiration_t( sim_t* sim, warlock_t* o ) : event_t( sim, o )
    {
      name = "Demonic Empathy Expiration";
      o -> aura_gain( "Demonic Empathy" );
      sim -> add_event( this, 15.0 );
    }
    virtual void execute()
    {
      warlock_t* o = player -> cast_warlock();
      o -> aura_loss( "Demonic Empathy" );
      o -> buffs_demonic_empathy = 0;
      o -> expirations_demonic_empathy = 0;
    }
  };
  
  warlock_pet_t* p = (warlock_pet_t*) a -> player -> cast_pet();
  warlock_t* o = p -> owner -> cast_warlock();

  if( ! o -> talents.demonic_empathy ) return;

  o -> buffs_demonic_empathy = 3;

  event_t*& e = o -> expirations_demonic_empathy;

  if( e )
  {
    e -> reschedule( 15.0 );
  }
  else
  {
    e = new ( a -> sim ) expiration_t( a -> sim, o );
  }
}

// trigger_demonic_empathy_on_pet ==========================================

static void trigger_demonic_empathy_on_pet( spell_t* s )
{
  struct expiration_t : public event_t
  {
    expiration_t( sim_t* sim, warlock_pet_t* p ) : event_t( sim, p )
    {
      name = "Demonic Empathy Expiration";
      p -> aura_gain( "Demonic Empathy" );
      sim -> add_event( this, 15.0 );
    }
    virtual void execute()
    {
      warlock_pet_t* p = (warlock_pet_t*) player -> cast_pet();
      p -> aura_loss( "Demonic Empathy" );
      p -> buffs_demonic_empathy = 0;
      p -> expirations_demonic_empathy = 0;
    }
  };
  
  warlock_t* o = s -> player -> cast_warlock();
  warlock_pet_t* p = o -> active_pet;

  if( ! o -> talents.demonic_empathy ) return;

  p -> buffs_demonic_empathy = 3;

  event_t*& e = p -> expirations_demonic_empathy;

  if( e )
  {
    e -> reschedule( 15.0 );
  }
  else
  {
    e = new ( s -> sim ) expiration_t( s -> sim, p );
  }
}

// trigger_demonic_pact ====================================================

static void trigger_demonic_pact( action_t* a )
{
  struct expiration_t : public event_t
  {
    expiration_t( sim_t* sim, warlock_pet_t* pet ) : event_t( sim, pet )
    {
      name = "Demonic Pact Expiration";
      for( player_t* p = sim -> player_list; p; p = p -> next )
      {
	if( p -> buffs.demonic_pact == 0 ) p -> aura_gain( "Demonic Pact" );
	p -> buffs.demonic_pact++;
      }
      sim -> add_event( this, 12.0 );
    }
    virtual void execute()
    {
      warlock_pet_t* pet = (warlock_pet_t*) player -> cast_pet();
      for( player_t* p = sim -> player_list; p; p = p -> next )
      {
	p -> buffs.demonic_pact--;
	if( p -> buffs.demonic_pact == 0 ) p -> aura_loss( "Demonic Pact" );
      }
      pet -> expirations_demonic_pact = 0;
    }
  };
  
  warlock_pet_t* p = (warlock_pet_t*) a -> player -> cast_pet();
  warlock_t* o = p -> owner -> cast_warlock();

  if( ! o -> talents.demonic_pact ) return;

  event_t*& e = p -> expirations_demonic_pact;

  if( e )
  {
    e -> reschedule( 12.0 );
  }
  else
  {
    e = new ( a -> sim ) expiration_t( a -> sim, p );
  }
}

// trigger_ashtongue_talisman ===============================================

static void trigger_ashtongue_talisman( spell_t* s )
{
  struct expiration_t : public event_t
  {
    expiration_t( sim_t* sim, player_t* p ) : event_t( sim, p )
    {
      name = "Ashtongue Talisman Expiration";
      player -> aura_gain( "Ashtongue Talisman" );
      player -> spell_power[ SCHOOL_MAX ] += 220;
      sim -> add_event( this, 5.0 );
    }
    virtual void execute()
    {
      player -> aura_loss( "Ashtongue Talisman" );
      player -> spell_power[ SCHOOL_MAX ] -= 220;
      player -> expirations.ashtongue_talisman = 0;
    }
  };

  player_t* p = s -> player;

  if( p -> gear.ashtongue_talisman && s -> sim -> roll( 0.20 ) )
  {
    p -> procs.ashtongue_talisman -> occur();

    event_t*& e = p -> expirations.ashtongue_talisman;

    if( e )
    {
      e -> reschedule( 5.0 );
    }
    else
    {
      e = new ( s -> sim ) expiration_t( s -> sim, p );
    }
  }
}

} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// Warlock Pet Events
// ==========================================================================

// warlock_pet_t::attack_hit_event ==========================================

void warlock_pet_t::attack_hit_event( attack_t* a )
{
  player_t::attack_hit_event( a );

  if( a -> result == RESULT_CRIT )
  {
    trigger_demonic_pact( a );
    trigger_demonic_empathy_on_owner( a );
  }
}

// warlock_pet_t::spell_hit_event ===========================================

void warlock_pet_t::spell_hit_event( spell_t* s )
{
  player_t::spell_hit_event( s );

  if( s -> result == RESULT_CRIT )
  {
    trigger_demonic_pact( s );
    trigger_demonic_empathy_on_owner( s );
  }
}

// imp_pet_t::spell_hit_event ===============================================

void imp_pet_t::spell_hit_event( spell_t* s )
{
  warlock_pet_t::spell_hit_event( s );

  if( s -> result == RESULT_CRIT )
  {
    trigger_empowered_imp( s );
  }
}

// ==========================================================================
// Warlock Spell
// ==========================================================================

// warlock_spell_t::haste ===================================================

double warlock_spell_t::haste()
{
  warlock_t* p = player -> cast_warlock();
  double h = spell_t::haste();
  if( p -> buffs_eradication ) h *= ( 1.0 / 1.20 );
  return h;
}

// warlock_spell_t::execute_time ============================================

double warlock_spell_t::execute_time()
{
  warlock_t* p = player -> cast_warlock();
  double t = spell_t::execute_time();
  if( t > 0 )
  {
    p -> uptimes_backdraft   -> update( p -> buffs_backdraft   != 0 );
    p -> uptimes_eradication -> update( p -> buffs_eradication != 0 );

    if( p -> buffs_backdraft && tree == TREE_DESTRUCTION ) t *= 1.0 - p -> talents.backdraft * 0.10;
  }
  return t;
}

// warlock_spell_t::player_buff =============================================

void warlock_spell_t::player_buff()
{
  warlock_t* p = player -> cast_warlock();

  spell_t::player_buff();

  if( p -> buffs_metamorphosis ) player_multiplier *= 1.20;

  if( p -> talents.malediction ) player_multiplier *= 1.0 + p ->talents.malediction * 0.01;

  if( school == SCHOOL_SHADOW )
  {
    player_multiplier *= 1.0 + trigger_deaths_embrace( this ) * 0.01;

    if( p -> buffs_pet_sacrifice == PET_FELGUARD ) player_multiplier *= 1.0 + 0.07;
    if( p -> buffs_pet_sacrifice == PET_SUCCUBUS ) player_multiplier *= 1.0 + 0.10;

    if( p -> buffs_flame_shadow ) player_power += 135;
    p -> uptimes_flame_shadow -> update( p -> buffs_flame_shadow != 0 );
  }
  else if( school == SCHOOL_FIRE )
  {
    if( p -> buffs_pet_sacrifice == PET_FELGUARD ) player_multiplier *= 1.0 + 0.07;
    if( p -> buffs_pet_sacrifice == PET_IMP      ) player_multiplier *= 1.0 + 0.10;

    if( p -> buffs_shadow_flame ) player_power += 135;
    p -> uptimes_shadow_flame -> update( p -> buffs_shadow_flame != 0 );

    if( p -> buffs_molten_core ) player_multiplier *= 1.10;
    p -> uptimes_molten_core -> update( p -> buffs_molten_core != 0 );
  }

  if( p -> active_pet )
  {
    if( p -> talents.master_demonologist )
    {
      if( p -> active_pet -> pet_type == PET_FELGUARD ) 
      {
	player_multiplier *= 1.0 + p -> talents.master_demonologist * 0.01;
      }
      else if( p -> active_pet -> pet_type == PET_IMP ) 
      {
	if( school == SCHOOL_FIRE )
	{
	  player_multiplier *= 1.0 + p -> talents.master_demonologist * 0.01;
	  player_crit       +=       p -> talents.master_demonologist * 0.01;
	}
      }
      else if( p -> active_pet -> pet_type == PET_SUCCUBUS ) 
      {
	if( school == SCHOOL_SHADOW )
	{
	  player_multiplier *= 1.0 + p -> talents.master_demonologist * 0.01;
	  player_crit       +=       p -> talents.master_demonologist * 0.01;
	}
      }
    }

    if( p -> talents.demonic_tactics )
    {
      player_crit += p -> talents.demonic_tactics * 0.02;
    }

    if( p -> buffs_demonic_empathy ) player_multiplier *= 1.0 + p -> talents.demonic_empathy * 0.01;
    p -> uptimes_demonic_empathy -> update( p -> buffs_demonic_empathy != 0 );

    if( may_crit )
    {
      if( p -> buffs_empowered_imp ) player_crit += 0.20;
      p -> uptimes_empowered_imp -> update( p -> buffs_empowered_imp != 0 );
    }
  }
}

// warlock_spell_t::target_debuff ============================================

void warlock_spell_t::target_debuff( int8_t dmg_type )
{
  warlock_t* p = player -> cast_warlock();

  spell_t::target_debuff( dmg_type );

  if( dmg_type == DMG_OVER_TIME )
  {
    if( p -> buffs_shadow_embrace )
    {
      target_multiplier *= 1.0 + p -> buffs_shadow_embrace * p -> talents.shadow_embrace * 0.01;
    }
    if( p -> buffs_haunted )
    {
      target_multiplier *= 1.20;
    }

    if( p -> main_hand_weapon.buff == SPELL_STONE ) 
    {
      target_multiplier *= 1.01;
    }
  }
  else
  {
    if( p -> main_hand_weapon.buff == FIRE_STONE ) 
    {
      target_multiplier *= 1.01;
    }
  }
}

// warlock_spell_t::execute ==================================================

void warlock_spell_t::execute()
{
  warlock_t* p = player -> cast_warlock();

  if( time_to_execute > 0 && tree == TREE_DESTRUCTION )
  {
    if( p -> buffs_backdraft > 0 ) p -> buffs_backdraft--;
  }
  if( p -> buffs_demonic_empathy > 0 )
  {
    p -> buffs_demonic_empathy--;
    if( p -> buffs_demonic_empathy == 0 ) event_t::early( p -> expirations_demonic_empathy );
  }

  spell_t::execute();

  if( result_is_hit() )
  {
    trigger_tier4_2pc( this );
    trigger_molten_core( this );

    if( result == RESULT_CRIT )
    {
      trigger_demonic_empathy_on_pet( this );
    }
  }

  if( may_crit && p -> expirations_empowered_imp ) 
  {
    event_t::early( p -> expirations_empowered_imp );
  }
}

// warlock_spell_t::tick =====================================================

void warlock_spell_t::tick()
{
  spell_t::tick();
  trigger_molten_core( this );
}

// Curse of Elements Spell ===================================================

struct curse_of_elements_t : public warlock_spell_t
{
  curse_of_elements_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "curse_of_elements", player, SCHOOL_SHADOW, TREE_AFFLICTION )
  {
    warlock_t* p = player -> cast_warlock();

    option_t options[] =
    {
      { "rank", OPT_INT8, &rank_index },
      { NULL }
    };
    parse_options( options, options_str );
      
    static rank_t ranks[] =
    {
      { 67, 3, 0, 0, 0, 260 },
      { 56, 2, 0, 0, 0, 200 },
      { 0,  0 }
    };
    player -> init_mana_costs( ranks );
    rank = choose_rank( ranks );
    
    base_execute_time = 0; 
    base_cost         = rank -> cost;
    base_cost        *= 1.0 - p -> talents.suppression * 0.02;
    base_hit         +=       p -> talents.suppression * 0.01;

    if( p -> talents.amplify_curse ) trigger_gcd = 1.0;
  }

  virtual void execute()
  {
    struct expiration_t : public event_t
    {
      expiration_t( sim_t* sim, warlock_t* p ) : event_t( sim, p )
      {
        name = "Cure of Elements Expiration";
        target_t* t = sim -> target;
        t -> debuffs.curse_of_elements = 10 + p -> talents.malediction;
        t -> debuffs.affliction_effects++;
        sim -> add_event( this, 300.0 );
      }
      virtual void execute()
      {
        warlock_t* p = player -> cast_warlock();
        target_t*  t = sim -> target;
        p -> active_curse = 0;
        t -> debuffs.curse_of_elements = 0;
        t -> expirations.curse_of_elements = 0;
        t -> debuffs.affliction_effects--;
      }
    };

    warlock_spell_t::execute(); 

    if( result_is_hit() )
    {
      warlock_t* p = player -> cast_warlock();
      target_t* t = sim -> target;
      event_t::early( t -> expirations.curse_of_elements );
      t -> expirations.curse_of_elements = new ( sim ) expiration_t( sim, p );
      p -> active_curse = this;
    }
  }
  
  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();

    if( p -> active_curse != 0 )
      return false;

    if( ! warlock_spell_t::ready() ) 
      return false;

    target_t* t = sim -> target;

    return std::max( t -> debuffs.curse_of_elements, t -> debuffs.earth_and_moon ) < 10 + p -> talents.malediction;
  }
};

// Curse of Agony Spell ===========================================================

struct curse_of_agony_t : public warlock_spell_t
{
  curse_of_agony_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "curse_of_agony", player, SCHOOL_SHADOW, TREE_AFFLICTION )
  {
    warlock_t* p = player -> cast_warlock();

    option_t options[] =
    {
      { "rank", OPT_INT8, &rank_index },
      { NULL }
    };
    parse_options( options, options_str );
      
    static rank_t ranks[] =
    {
      { 79, 9, 0, 0, 145, 0.10 },
      { 73, 8, 0, 0, 120, 0.10 },
      { 67, 7, 0, 0, 113, 265  },
      { 58, 6, 0, 0,  87, 215  },
      { 0, 0 }
    };
    player -> init_mana_costs( ranks );
    rank = choose_rank( ranks );
      
    base_execute_time = 0; 
    base_tick_time    = 2.0; 
    num_ticks         = 12;
    tick_power_mod    = base_tick_time / 15.0;
    tick_power_mod    = 1.20 / num_ticks;  // Nerf Bat!

    base_cost        = rank -> cost;
    base_cost       *= 1.0 - p -> talents.suppression * 0.02;
    base_hit        +=       p -> talents.suppression * 0.01;
    base_multiplier *= 1.0 + ( p -> talents.shadow_mastery          * 0.03 +
			       p -> talents.contagion               * 0.01 +
			       p -> talents.improved_curse_of_agony * 0.05 );

    if( p -> talents.amplify_curse ) trigger_gcd = 1.0;

    if( p -> glyphs.curse_of_agony ) num_ticks += 2;
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::execute();
    if( result_is_hit() )
    {
      p -> active_dots++;
      p -> active_curse = this;
      sim -> target -> debuffs.affliction_effects++;
    }
  }

  virtual void last_tick()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::last_tick(); 
    p -> active_dots--;
    p -> active_curse = 0;
    sim -> target -> debuffs.affliction_effects--;
  }
  
  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();

    if( p -> active_curse != 0 )
      return false;

    if( ! warlock_spell_t::ready() ) 
      return false;

    return true;
  }
};

// Curse of Doom Spell ===========================================================

struct curse_of_doom_t : public warlock_spell_t
{
  curse_of_doom_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "curse_of_doom", player, SCHOOL_SHADOW, TREE_AFFLICTION )
  {
    warlock_t* p = player -> cast_warlock();

    option_t options[] =
    {
      { "rank", OPT_INT8, &rank_index },
      { NULL }
    };
    parse_options( options, options_str );
      
    static rank_t ranks[] =
    {
      { 80, 3, 0, 0, 7300, 0.15 },
      { 70, 2, 0, 0, 4200, 380  },
      { 60, 1, 0, 0, 3200, 300  },
      { 0, 0 }
    };
    player -> init_mana_costs( ranks );
    rank = choose_rank( ranks );
      
    base_execute_time = 0; 
    base_tick_time    = 60.0; 
    num_ticks         = 1;
    tick_power_mod    = 2.0;

    base_cost        = rank -> cost;
    base_cost       *= 1.0 - p -> talents.suppression * 0.02;
    base_hit        +=       p -> talents.suppression * 0.01;

    if( p -> talents.amplify_curse ) trigger_gcd = 1.0;
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::execute();
    if( result_is_hit() )
    {
      p -> active_curse = this;
      p -> active_dots++;
      sim -> target -> debuffs.affliction_effects++;
    }
  }

  virtual void assess_damage( double amount, 
			      int8_t dmg_type )
  {
    warlock_spell_t::assess_damage( amount, DMG_DIRECT );
  }

  virtual void last_tick()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::last_tick(); 
    p -> active_curse = 0;
    p -> active_dots--;
    sim -> target -> debuffs.affliction_effects--;
  }
  
  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();

    if( p -> active_curse != 0 )
      return false;

    if( ! warlock_spell_t::ready() ) 
      return false;

    return true;
  }
};

// Shadow Bolt Spell ===========================================================

struct shadow_bolt_t : public warlock_spell_t
{
  int8_t shadow_trance;
  int8_t backdraft;
  int8_t isb_benefit;
  int8_t isb_trigger;

  shadow_bolt_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "shadow_bolt", player, SCHOOL_SHADOW, TREE_DESTRUCTION ), 
    shadow_trance(0), backdraft(0), isb_benefit(0), isb_trigger(0)
  {
    warlock_t* p = player -> cast_warlock();

    option_t options[] =
    {
      { "rank",          OPT_INT8, &rank_index    },
      { "shadow_trance", OPT_INT8, &shadow_trance },
      { "backdraft",     OPT_INT8, &backdraft     },
      { "isb_benefit",   OPT_INT8, &isb_benefit   },
      { "isb_trigger",   OPT_INT8, &isb_trigger   },
      { NULL }
    };
    parse_options( options, options_str );
      
    static rank_t ranks[] =
    {
      { 79, 13, 690, 770, 0, 0.17 },
      { 74, 12, 596, 664, 0, 0.17 },
      { 69, 11, 541, 603, 0, 420  },
      { 60, 10, 482, 538, 0, 380  },
      { 60,  9, 455, 507, 0, 370  },
      { 0, 0 }
    };
    player -> init_mana_costs( ranks );
    rank = choose_rank( ranks );
    
    base_execute_time = 3.0; 
    may_crit          = true;
    direct_power_mod  = base_execute_time / 3.5;
      
    base_cost          = rank -> cost;
    base_cost         *= 1.0 - ( p -> talents.cataclysm  * 0.01 +
				 p -> glyphs.shadow_bolt * 0.10 ); 
    base_execute_time -=  p -> talents.bane * 0.1;
    base_multiplier   *= 1.0 + ( p -> talents.shadow_mastery * 0.03 +
				 p -> gear.tier6_4pc         * 0.06 );
    base_crit         += p -> talents.devastation * 0.05;
    base_crit         += p -> talents.backlash * 0.01;
    direct_power_mod  *= 1.0 + p -> talents.shadow_and_flame * 0.04;
    base_crit_bonus   *= 1.0 + p -> talents.ruin * 0.20;
    base_hit          += p -> talents.cataclysm * 0.01;
  }

  virtual double execute_time()
  {
    warlock_t* p = player -> cast_warlock();
    p -> uptimes_shadow_trance -> update( p -> buffs_shadow_trance != 0 );
    if( p -> buffs_shadow_trance ) return 0;
    return warlock_spell_t::execute_time();
  }
  
  virtual void schedule_execute()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::schedule_execute(); 
    if( p -> buffs_shadow_trance )
    {
      p -> aura_loss( "Shadow Trance" );
      p -> buffs_shadow_trance = 0;
    }
  }
  
  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::execute(); 
    if( result_is_hit() )
    {
      stack_shadow_embrace( this );
      trigger_soul_leech( this );
      trigger_tier5_4pc( this, p -> active_corruption );
      decrement_shadow_vulnerability( p );
      if( result == RESULT_CRIT ) trigger_shadow_vulnerability( this );
    }
  }

  virtual void player_buff()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::player_buff();
    if( p -> buffs_shadow_vulnerability ) player_multiplier *= 1.0 + p -> talents.improved_shadow_bolt * 0.02;
    p -> uptimes_shadow_vulnerability -> update( p -> buffs_shadow_vulnerability != 0 );
  }
  
  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();

    if( ! warlock_spell_t::ready() )
      return false;

    if( shadow_trance )
      if( ! p -> buffs_shadow_trance )
	return false;

    if( backdraft )
      if( ! p -> buffs_backdraft )
	return false;

    if( isb_benefit )
      if( ! sim -> time_to_think( p -> buffs_shadow_vulnerability ) )
	return false;

    if( isb_trigger )
      if( sim -> time_to_think( p -> buffs_shadow_vulnerability ) )
	return false;

    return true;
  }
};

// Chaos Bolt Spell ===========================================================

struct chaos_bolt_t : public warlock_spell_t
{
  int8_t backdraft;
  int8_t molten_core;

  chaos_bolt_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "chaos_bolt", player, SCHOOL_FIRE, TREE_DESTRUCTION ), backdraft(0), molten_core(0)
  {
    warlock_t* p = player -> cast_warlock();

    option_t options[] =
    {
      { "rank",        OPT_INT8, &rank_index  },
      { "backdraft",   OPT_INT8, &backdraft   },
      { "molten_core", OPT_INT8, &molten_core },
      { NULL }
    };
    parse_options( options, options_str );
      
    static rank_t ranks[] =
    {
      { 80, 4, 1036, 1314, 0, 0.07 },
      { 75, 3,  882, 1120, 0, 0.07 },
      { 70, 2,  781,  991, 0, 0.07 },
      { 60, 1,  607,  769, 0, 0.09 },
      { 0, 0 }
    };
    player -> init_mana_costs( ranks );
    rank = choose_rank( ranks );
    
    base_execute_time = 2.5; 
    direct_power_mod  = base_execute_time / 3.5; 
    cooldown          = 12.0;
    may_crit          = true;
    may_resist        = false;
      
    base_cost          = rank -> cost;
    base_cost         *= 1.0 -  p -> talents.cataclysm * 0.01;
    base_execute_time -=  p -> talents.bane * 0.1;
    base_multiplier   *= 1.0 + p -> talents.emberstorm  * 0.03;
    base_crit         += p -> talents.devastation * 0.05;
    base_crit         += p -> talents.backlash * 0.01;
    direct_power_mod  *= 1.0 + p -> talents.shadow_and_flame * 0.04;
    base_crit_bonus   *= 1.0 + p -> talents.ruin * 0.20;
    base_hit          += p -> talents.cataclysm * 0.01;
  }

  virtual void execute()
  {
    warlock_spell_t::execute(); 
    if( result_is_hit() )
    {
      trigger_soul_leech( this );
    }
  }

  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();

    if( ! warlock_spell_t::ready() )
      return false;

    if( backdraft )
      if( ! p -> buffs_backdraft )
	return false;

    if( molten_core )
    {
      if( ! sim -> time_to_think( p -> buffs_molten_core ) )
	return false;

      if( sim -> current_time + execute_time() > p -> expirations_molten_core -> occurs() )
	return false;
    }
	  
    return true;
  }
};

// Death Coil Spell ===========================================================

struct death_coil_t : public warlock_spell_t
{
  death_coil_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "death_coil", player, SCHOOL_SHADOW, TREE_AFFLICTION )
  {
    warlock_t* p = player -> cast_warlock();
    
    option_t options[] =
    {
      { "rank", OPT_INT8, &rank_index },
      { NULL }
    };
    parse_options( options, options_str );
      
    static rank_t ranks[] =
    {
      { 78, 6, 790, 790, 0, 0.23 },
      { 73, 5, 670, 670, 0, 0.23 },
      { 68, 4, 519, 519, 0, 600  },
      { 58, 3, 400, 400, 0, 480  },
      { 0, 0 }
    };
    player -> init_mana_costs( ranks );
    rank = choose_rank( ranks );
      
    base_execute_time = 0; 
    may_crit          = true; 
    binary            = true; 
    cooldown          = 120;
    direct_power_mod  = ( 1.5 / 3.5 ) / 2.0; 
      
    base_cost        = rank -> cost;
    base_cost       *= 1.0 -  p -> talents.suppression * 0.02;
    base_multiplier *= 1.0 + p -> talents.shadow_mastery * 0.03;
    base_crit_bonus *= 1.0 + p -> talents.ruin * 0.20;
    base_hit        += p -> talents.suppression * 0.01;
  }

  virtual void execute() 
  {
    warlock_spell_t::execute();
    if( result_is_hit() )
    {
      player -> resource_gain( RESOURCE_HEALTH, direct_dmg );
      decrement_shadow_vulnerability( player -> cast_warlock() );
    }
  }

  virtual void player_buff()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::player_buff();
    if( p -> buffs_shadow_vulnerability ) player_multiplier *= 1.0 + p -> talents.improved_shadow_bolt * 0.02;
    p -> uptimes_shadow_vulnerability -> update( p -> buffs_shadow_vulnerability != 0 );
  }
};

// Shadow Burn Spell ===========================================================

struct shadow_burn_t : public warlock_spell_t
{
  shadow_burn_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "shadow_burn", player, SCHOOL_SHADOW, TREE_DESTRUCTION )
  {
    warlock_t* p = player -> cast_warlock();

    assert( p -> talents.shadow_burn );

    option_t options[] =
    {
      { "rank", OPT_INT8, &rank_index },
      { NULL }
    };
    parse_options( options, options_str );
      
    static rank_t ranks[] =
    {
      { 80, 10, 775, 865, 0, 0.20 },
      { 75,  9, 662, 738, 0, 0.20 },
      { 70,  8, 597, 665, 0, 0.20 },
      { 63,  7, 518, 578, 0, 0.27 },
      { 56,  6, 450, 502, 0, 0.27 },
      { 0, 0 }
    };
    player -> init_mana_costs( ranks );
    rank = choose_rank( ranks );
      
    may_crit         = true; 
    cooldown         = 15;
    direct_power_mod = ( 1.5 / 3.5 ); 
      
    base_cost        = rank -> cost;
    base_cost       *= 1.0 -  p -> talents.cataclysm * 0.01;
    base_multiplier *= 1.0 + p -> talents.shadow_mastery * 0.03;
    base_crit_bonus *= 1.0 + p -> talents.ruin * 0.20;
    base_hit        += p -> talents.cataclysm * 0.01;
   }

  virtual void execute()
  {
    warlock_spell_t::execute(); 
    if( result_is_hit() )
    {
      trigger_soul_leech( this );
      decrement_shadow_vulnerability( player -> cast_warlock() );
    }
  }

  virtual void player_buff()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::player_buff();
    if( p -> buffs_shadow_vulnerability ) player_multiplier *= 1.0 + p -> talents.improved_shadow_bolt * 0.02;
    p -> uptimes_shadow_vulnerability -> update( p -> buffs_shadow_vulnerability != 0 );
    if( p -> glyphs.shadow_burn )
    {
      target_t* t = sim -> target;

      if( t -> initial_health > 0 )
      {
	if( ( t -> current_health / t -> initial_health ) < 0.35 )
	{
	  player_crit += 0.20;
	}
      }
    }
  }
};

// Corruption Spell ===========================================================

struct corruption_t : public warlock_spell_t
{
  corruption_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "corruption", player, SCHOOL_SHADOW, TREE_AFFLICTION )
  {
    warlock_t* p = player -> cast_warlock();

    option_t options[] =
    {
      { "rank", OPT_INT8, &rank_index },
      { NULL }
    };
    parse_options( options, options_str );
      
    static rank_t ranks[] =
    {
      { 77, 10, 0, 0, 180, 0.14 },
      { 71,  9, 0, 0, 165, 0.14 },
      { 65,  8, 0, 0, 150, 370  },
      { 60,  7, 0, 0, 137, 340  },
      { 0, 0 }
    };
    player -> init_mana_costs( ranks );
    rank = choose_rank( ranks );

    base_execute_time = 0.0; 
    base_tick_time    = 3.0; 
    num_ticks         = 6;
    tick_power_mod    = base_tick_time / 15.0;

    base_cost        = rank -> cost;
    base_cost       *= 1.0 - p -> talents.suppression * 0.02;
    base_hit        +=  p -> talents.suppression * 0.01;
    base_multiplier *= 1.0 + ( p -> talents.shadow_mastery      * 0.03 +
			       p -> talents.contagion           * 0.01 +
			       p -> talents.improved_corruption * 0.02 );
    tick_power_mod  += p -> talents.empowered_corruption * 0.02;
    tick_power_mod  += p -> talents.everlasting_affliction * 0.01;

    if( p -> gear.tier4_4pc ) num_ticks++;

    observer = &( p -> active_corruption );
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    base_tick_dmg = rank -> tick;
    warlock_spell_t::execute();
    if( result_is_hit() )
    {
      p -> active_dots++;
      sim -> target -> debuffs.affliction_effects++;
    }
  }

  virtual void tick()
  {
    warlock_spell_t::tick(); 
    trigger_pandemic( this );
    trigger_eradication( this );
    trigger_nightfall( this );
    trigger_corruption_glyph( this );
    trigger_ashtongue_talisman( this );
    if( player -> gear.tier6_2pc ) player -> resource_gain( RESOURCE_HEALTH, 70 );
  }

  virtual void last_tick()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::last_tick(); 
    p -> active_dots--;
    sim -> target -> debuffs.affliction_effects--;
  }
};

// Drain Life Spell ===========================================================

struct drain_life_t : public warlock_spell_t
{
  drain_life_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "drain_life", player, SCHOOL_SHADOW, TREE_AFFLICTION )
  {
    warlock_t* p = player -> cast_warlock();

    option_t options[] =
    {
      { "rank", OPT_INT8, &rank_index },
      { NULL }
    };
    parse_options( options, options_str );
      
    static rank_t ranks[] =
    {
      { 78, 9, 0, 0, 133, 0.17 },
      { 69, 8, 0, 0, 108, 425  },
      { 62, 7, 0, 0,  87, 355  },
      { 54, 6, 0, 0,  71, 300  },
      { 0, 0 }
    };
    player -> init_mana_costs( ranks );
    rank = choose_rank( ranks );
    
    base_execute_time = 0; 
    base_tick_time    = 1.0; 
    num_ticks         = 5; 
    channeled         = true; 
    binary            = true;
    tick_power_mod    = ( base_tick_time / 3.5 ) / 2.0; 

    base_cost        = rank -> cost;
    base_cost       *= 1.0 - p -> talents.suppression * 0.02;
    base_hit        +=       p -> talents.suppression * 0.01;
    base_multiplier *= 1.0 + p -> talents.shadow_mastery * 0.03;
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::execute();
    if( result_is_hit() )
    {
      trigger_everlasting_affliction( this );
      p -> active_dots++;
      sim -> target -> debuffs.affliction_effects++;
    }
  }

  virtual void last_tick()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::last_tick(); 
    p -> active_dots--;
    sim -> target -> debuffs.affliction_effects--;
  }

  virtual void player_buff()
  {
    warlock_spell_t::player_buff();

    warlock_t* p = player -> cast_warlock();
    target_t*  t = sim -> target;

    double min_multiplier[] = { 0, 0.02, 0.04 };
    double max_multiplier[] = { 0, 0.24, 0.60 };

    assert( p -> talents.soul_siphon >= 0 &&
	    p -> talents.soul_siphon <= 2 );

    double min = min_multiplier[ p -> talents.soul_siphon ];
    double max = max_multiplier[ p -> talents.soul_siphon ];

    double multiplier = t -> debuffs.affliction_effects * min;
    if( multiplier > max ) multiplier = max;

    player_multiplier *= 1.0 + multiplier;    
  }

  virtual void tick()
  {
    warlock_spell_t::tick(); 
    trigger_nightfall( this );
  }
};

// Drain Soul Spell ===========================================================

struct drain_soul_t : public warlock_spell_t
{
  int8_t target_pct;

  drain_soul_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "drain_soul", player, SCHOOL_SHADOW, TREE_AFFLICTION ), target_pct(0)
  {
    warlock_t* p = player -> cast_warlock();

    option_t options[] =
    {
      { "rank",       OPT_INT8, &rank_index },
      { "target_pct", OPT_INT8, &target_pct },
      { NULL }
    };
    parse_options( options, options_str );
      
    static rank_t ranks[] =
    {
      { 77, 6, 0, 0, 142, 0.14 },
      { 67, 5, 0, 0, 124, 360  },
      { 52, 4, 0, 0,  91, 290  },
      { 0, 0 }
    };
    player -> init_mana_costs( ranks );
    rank = choose_rank( ranks );
    
    base_execute_time = 0; 
    base_tick_time    = 3.0; 
    num_ticks         = 5; 
    channeled         = true; 
    binary            = true;
    tick_power_mod    = ( base_tick_time / 3.5 ) / 2.0; 

    base_cost         = rank -> cost;
    base_cost        *= 1.0 - p -> talents.suppression * 0.02;
    base_hit         += p -> talents.suppression * 0.01;
    base_multiplier  *= 1.0 + p -> talents.shadow_mastery * 0.03;
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::execute();
    if( result_is_hit() )
    {
      p -> active_dots++;
      sim -> target -> debuffs.affliction_effects++;
    }
  }

  virtual void last_tick()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::last_tick(); 
    p -> active_dots--;
    sim -> target -> debuffs.affliction_effects--;
  }

  virtual void player_buff()
  {
    warlock_spell_t::player_buff();

    warlock_t* p = player -> cast_warlock();
    target_t*  t = sim -> target;

    double min_multiplier[] = { 0, 0.02, 0.04 };
    double max_multiplier[] = { 0, 0.24, 0.60 };

    assert( p -> talents.soul_siphon >= 0 &&
	    p -> talents.soul_siphon <= 2 );

    double min = min_multiplier[ p -> talents.soul_siphon ];
    double max = max_multiplier[ p -> talents.soul_siphon ];

    double multiplier = t -> debuffs.affliction_effects * min;
    if( multiplier > max ) multiplier = max;

    player_multiplier *= 1.0 + multiplier;

    // FIXME! Hack! Deaths Embrace is additive with Drain Soul "execute".
    // Perhaps it is time to add notion of "execute" into action_t class.

    int de_bonus = trigger_deaths_embrace( this );
    if( de_bonus ) player_multiplier /= 1.0 + de_bonus * 0.01;

    if( rank -> index >= 6 && t -> initial_health > 0 )
    {
      if( ( t -> current_health / t -> initial_health ) < 0.25 )
      {
	player_multiplier *= 4.0 + de_bonus * 0.01;
      }
    }
    
  }

  virtual void target_debuff( int8_t dmg_type )
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::target_debuff( dmg_type );
    if( p -> buffs_haunted )
    {
      // Haunt currently does not affect Drain Soul
      target_multiplier /= 1.20;
    }
  }

  virtual bool ready()
  {
    if( ! warlock_spell_t::ready() )
      return false;

    target_t* t = sim -> target;

    if( target_pct == 0 )
      return true;

    if( t -> initial_health <= 0 )
      return false;

    return( ( t -> current_health / t -> initial_health ) < ( target_pct / 100.0 ) );
  }
};

// Siphon Life Spell ===========================================================

struct siphon_life_t : public warlock_spell_t
{
  siphon_life_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "siphon_life", player, SCHOOL_SHADOW, TREE_AFFLICTION )
  {
    warlock_t* p = player -> cast_warlock();

    assert( p -> talents.siphon_life );

    option_t options[] =
    {
      { "rank", OPT_INT8, &rank_index },
      { NULL }
    };
    parse_options( options, options_str );
      
    static rank_t ranks[] =
    {
      { 80, 8, 0, 0, 81, 0.16 },
      { 75, 7, 0, 0, 70, 0.16 },
      { 70, 6, 0, 0, 63, 410  },
      { 63, 5, 0, 0, 52, 350  },
      { 58, 4, 0, 0, 45, 310  },
      { 0, 0 }
    };
    player -> init_mana_costs( ranks );
    rank = choose_rank( ranks );
     
    base_execute_time = 0; 
    base_tick_time    = 3.0; 
    num_ticks         = 10; 
    binary            = true;
    tick_power_mod    = ( base_tick_time / 15.0 ) / 2.0; 

    base_cost        = rank -> cost;
    base_cost       *= 1.0 - p -> talents.suppression * 0.02;
    base_hit        +=       p -> talents.suppression * 0.01;
    base_multiplier *= 1.0 + ( p -> talents.shadow_mastery * 0.03 +
			       p -> glyphs.siphon_life     * 0.20 );			
    tick_power_mod  += p -> talents.everlasting_affliction * 0.01;
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::execute();
    if( result_is_hit() )
    {
      p -> active_dots++;
      sim -> target -> debuffs.affliction_effects++;
    }
  }

  virtual void last_tick()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::last_tick(); 
    p -> active_dots--;
    sim -> target -> debuffs.affliction_effects--;
  }

  virtual void tick()
  {
    warlock_spell_t::tick(); 
    player -> resource_gain( RESOURCE_HEALTH, tick_dmg );
  }
};

// Unstable Affliction Spell ======================================================

struct unstable_affliction_t : public warlock_spell_t
{
  unstable_affliction_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "unstable_affliction", player, SCHOOL_SHADOW, TREE_AFFLICTION )
  {
    warlock_t* p = player -> cast_warlock();

    assert( p -> talents.unstable_affliction );
    
    option_t options[] =
    {
      { "rank", OPT_INT8, &rank_index },
      { NULL }
    };
    parse_options( options, options_str );
      
    static rank_t ranks[] =
    {
      { 80, 5,  0, 0, 230, 0.15 },
      { 75, 4,  0, 0, 197, 0.15 },
      { 70, 3,  0, 0, 175, 400  },
      { 60, 2,  0, 0, 155, 315  },
      { 0, 0 }
    };
    player -> init_mana_costs( ranks );
    rank = choose_rank( ranks );
    
    base_execute_time = 1.5; 
    base_tick_time    = 3.0; 
    num_ticks         = 5;
    tick_power_mod    = base_tick_time / 15.0; 
    
    base_cost         = rank -> cost;
    base_cost        *= 1.0 - p -> talents.suppression * 0.02;
    base_hit         +=       p -> talents.suppression * 0.01;
    base_multiplier  *= 1.0 + p -> talents.shadow_mastery * 0.03;
    tick_power_mod   += p -> talents.everlasting_affliction * 0.01;
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::execute();
    if( result_is_hit() )
    {
      p -> active_dots++;;
      sim -> target -> debuffs.affliction_effects++;
    }
  }

  virtual void tick()
  {
    warlock_spell_t::tick(); 
    trigger_pandemic( this );
  }

  virtual void last_tick()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::last_tick(); 
    p -> active_dots--;;
    sim -> target -> debuffs.affliction_effects--;
  }
};

// Haunt Spell ==============================================================

struct haunt_t : public warlock_spell_t
{
  int8_t debuff;

  haunt_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "haunt", player, SCHOOL_SHADOW, TREE_AFFLICTION ), debuff(0)
  {
    warlock_t* p = player -> cast_warlock();
    assert( p -> talents.haunt );

    option_t options[] =
    {
      { "rank",   OPT_INT8, &rank_index },
      { "debuff", OPT_INT8, &debuff     },
      { "only_for_debuff", OPT_DEPRECATED, (void*) "debuff" },
      { NULL }
    };

    parse_options( options, options_str );
      
    static rank_t ranks[] =
    {
      { 80, 4, 645, 753, 0, 0.12 },
      { 75, 3, 550, 642, 0, 0.12 },
      { 70, 2, 487, 569, 0, 0.12 },
      { 60, 1, 405, 473, 0, 0.12 },
      { 0, 0 }
    };
    player -> init_mana_costs( ranks );
    rank = choose_rank( ranks );
    
    base_execute_time = 1.5; 
    direct_power_mod  = base_execute_time / 3.5; 
    cooldown          = 8.0;
    may_crit          = true;
      
    base_cost         = rank -> cost;
    base_cost        *= 1.0 - p -> talents.suppression * 0.02;
    base_hit         +=       p -> talents.suppression * 0.01;
    base_multiplier  *= 1.0 + p -> talents.shadow_mastery * 0.03;
  }

  virtual void execute()
  {
    warlock_spell_t::execute(); 
    if( result_is_hit() )
    {
      trigger_haunted( this );
      trigger_everlasting_affliction( this );
      stack_shadow_embrace( this );
      decrement_shadow_vulnerability( player -> cast_warlock() );
    }
  }

  virtual void player_buff()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::player_buff();
    if( p -> buffs_shadow_vulnerability ) player_multiplier *= 1.0 + p -> talents.improved_shadow_bolt * 0.02;
    p -> uptimes_shadow_vulnerability -> update( p -> buffs_shadow_vulnerability != 0 );
  }

  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();

    if( ! warlock_spell_t::ready() )
      return false;

    if( debuff && p -> expirations_haunted )
      if( ( sim -> current_time + execute_time() ) < p -> expirations_haunted -> time )
	return false;

    return true;
  }
};

// Immolate Spell =============================================================

struct immolate_t : public warlock_spell_t
{
  immolate_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "immolate", player, SCHOOL_FIRE, TREE_DESTRUCTION )
  {
    warlock_t* p = player -> cast_warlock();

    option_t options[] =
    {
      { "rank", OPT_INT8, &rank_index },
      { NULL }
    };
    parse_options( options, options_str );
      
    static rank_t ranks[] =
    {
      { 80, 11, 460, 460, 157, 0.17 },
      { 75, 10, 370, 370, 139, 0.17 },
      { 69,  9, 327, 327, 123, 445  },
      { 60,  8, 279, 279, 102, 380  },
      { 60,  7, 258, 258,  97, 370  },
      { 0, 0 }
    };
    player -> init_mana_costs( ranks );
    rank = choose_rank( ranks );

    base_execute_time = 2.0; 
    may_crit          = true;
    base_tick_time    = 3.0; 
    num_ticks         = 5;
    direct_power_mod  = 0.20; 
    tick_power_mod    = 0.20;

    base_cost          = rank -> cost;
    base_cost         *= 1.0 -  p -> talents.cataclysm * 0.01;
    base_execute_time -= p -> talents.bane * 0.1;
    base_multiplier   *= 1.0 + p -> talents.emberstorm  * 0.03;
    base_crit         += p -> talents.devastation * 0.05;
    base_crit         += p -> talents.backlash * 0.01;
    base_crit_bonus   *= 1.0 + p -> talents.ruin * 0.20;
    base_hit          += p -> talents.cataclysm * 0.01;
    direct_power_mod  += (1.0/3.0) * p -> talents.fire_and_brimstone * 0.03;
    tick_power_mod    += (2.0/3.0) * p -> talents.fire_and_brimstone * 0.03 / num_ticks;

    if( p -> gear.tier4_4pc ) num_ticks++;

    observer = &( p -> active_immolate );
  }

  virtual double calculate_direct_damage()
  {
    warlock_t* p = player -> cast_warlock();
    spell_t::calculate_direct_damage();
    direct_dmg *= 1.0 + ( p -> talents.improved_immolate * 0.10 ) - ( p -> glyphs.immolate * 0.10 );
    return direct_dmg;
  }

  virtual double calculate_tick_damage()
  {
    warlock_t* p = player -> cast_warlock();
    spell_t::calculate_tick_damage();
    if( p -> glyphs.immolate ) tick_dmg *= 1.20;
    return tick_dmg;
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    base_tick_dmg = rank -> tick;
    warlock_spell_t::execute();
    if( result_is_hit() )
    {
      p -> active_dots++;
    }
  }

  virtual void tick()
  {
    warlock_spell_t::tick(); 
    if( player -> gear.tier6_2pc ) player -> resource_gain( RESOURCE_HEALTH, 35 );
  }

  virtual void last_tick()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::last_tick(); 
    p -> active_dots--;
  }
};

// Conflagrate Spell =========================================================

struct conflagrate_t : public warlock_spell_t
{
  int8_t ticks_lost;

  conflagrate_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "conflagrate", player, SCHOOL_FIRE, TREE_DESTRUCTION ), ticks_lost(1)
  {
    warlock_t* p = player -> cast_warlock();

    assert( p -> talents.conflagrate );

    option_t options[] =
    {
      { "rank",       OPT_INT8, &rank_index },
      { "ticks_lost", OPT_INT8, &ticks_lost },
      { NULL }
    };
    parse_options( options, options_str );
      
    static rank_t ranks[] =
    {
      { 80, 8, 766, 954, 0, 0.12 },
      { 75, 7, 650, 810, 0, 0.12 },
      { 70, 6, 579, 721, 0, 305  },
      { 65, 5, 512, 638, 0, 280  },
      { 60, 4, 447, 557, 0, 255  },
      { 0, 0 }
    };
    player -> init_mana_costs( ranks );
    rank = choose_rank( ranks );
     
    base_execute_time = 0; 
    may_crit          = true;
    direct_power_mod  = (1.5/3.5);
    cooldown          = 10;

    base_cost         = rank -> cost;
    base_cost        *= 1.0 -  p -> talents.cataclysm * 0.01;
    base_multiplier  *= 1.0 + p -> talents.emberstorm  * 0.03;
    base_crit        += p -> talents.devastation * 0.05;
    base_crit        += p -> talents.backlash * 0.01;
    base_crit_bonus  *= 1.0 + p -> talents.ruin * 0.20;
    base_hit         += p -> talents.cataclysm * 0.01;

    if( p -> glyphs.conflagrate ) base_cost *= 0.80;
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::execute(); 
    if( result_is_hit() )
    {
      trigger_soul_leech( this );
      trigger_backdraft( this );
    }
    p -> active_immolate -> cancel();
    p -> active_immolate = 0;
  }

  virtual void player_buff()
  {
    warlock_t* p = player -> cast_warlock();
    warlock_spell_t::player_buff(); 
    if( p -> talents.fire_and_brimstone )
    {
      int ticks_remaining = ( p -> active_immolate -> num_ticks - 
			      p -> active_immolate -> current_tick );
      
      if( ticks_remaining <= 2 )
      {
	player_crit += p -> talents.fire_and_brimstone * 0.05;
      }
    }
  }

  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();

    if( ! spell_t::ready() ) 
      return false;

    if( ! p -> active_immolate )
      return false;

    int ticks_remaining = ( p -> active_immolate -> num_ticks - 
			    p -> active_immolate -> current_tick );

    return( ticks_remaining <= ticks_lost );
  }
};

// Incinerate Spell =========================================================

struct incinerate_t : public warlock_spell_t
{
  int8_t backdraft;
  int8_t molten_core;

  incinerate_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "incinerate", player, SCHOOL_FIRE, TREE_DESTRUCTION ), backdraft(0), molten_core(0)
  {
    warlock_t* p = player -> cast_warlock();

    option_t options[] =
    {
      { "rank",        OPT_INT8, &rank_index  },
      { "backdraft",   OPT_INT8, &backdraft   },
      { "molten_core", OPT_INT8, &molten_core },
      { NULL }
    };
    parse_options( options, options_str );
      
    static rank_t ranks[] =
    {
      { 80, 4, 582, 676, 0, 0.14 },
      { 74, 3, 485, 563, 0, 0.14 },
      { 70, 2, 429, 497, 0, 300  },
      { 64, 1, 357, 413, 0, 256  },
      { 0, 0 }
    };
    player -> init_mana_costs( ranks );
    rank = choose_rank( ranks );
     
    base_execute_time  = 2.5; 
    may_crit           = true;
    direct_power_mod   = (2.5/3.5); 

    base_cost          = rank -> cost;
    base_cost         *= 1.0 - p -> talents.cataclysm * 0.01;
    base_execute_time -= p -> talents.emberstorm * 0.05;
    base_multiplier   *= 1.0 + ( p -> talents.emberstorm  * 0.03 +
				 p -> gear.tier6_4pc      * 0.06 );
    base_crit         += p -> talents.devastation * 0.05;
    base_crit         += p -> talents.backlash * 0.01;
    direct_power_mod  *= 1.0 + p -> talents.shadow_and_flame * 0.04;
    base_crit_bonus   *= 1.0 + p -> talents.ruin * 0.20;
    base_hit          += p -> talents.cataclysm * 0.01;
  }

  virtual void execute()
  {
    warlock_t* p = player -> cast_warlock();
    base_direct_dmg = ( rank -> dd_min + rank -> dd_max ) / 2.0;
    if( p -> active_immolate )
    {
      switch( rank -> index )
      {
      case 4: base_direct_dmg += 157; break;
      case 3: base_direct_dmg += 130; break;
      case 2: base_direct_dmg += 120; break;
      case 1: base_direct_dmg += 108; break;
      default: assert(0);
      }
    }
    warlock_spell_t::execute(); 
    if( result_is_hit() )
    {
      trigger_soul_leech( this );
      trigger_tier5_4pc( this, p -> active_immolate );
    }
  }

  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();

    if( ! warlock_spell_t::ready() )
      return false;

    if( backdraft )
      if( ! p -> buffs_backdraft )
	return false;

    if( molten_core )
    {
      if( ! sim -> time_to_think( p -> buffs_molten_core ) )
	return false;

      if( sim -> current_time + execute_time() > p -> expirations_molten_core -> occurs() )
	return false;
    }
	  
    return true;
  }
};

// Searing Pain Spell =========================================================

struct searing_pain_t : public warlock_spell_t
{
  searing_pain_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "searing_pain", player, SCHOOL_FIRE, TREE_DESTRUCTION )
  {
    warlock_t* p = player -> cast_warlock();

    option_t options[] =
    {
      { "rank", OPT_INT8, &rank_index },
      { NULL }
    };
    parse_options( options, options_str );
      
    static rank_t ranks[] =
    {
      { 79, 10, 343, 405, 0, 0.08 },
      { 74,  9, 295, 349, 0, 0.08 },
      { 70,  8, 270, 320, 0, 205  },
      { 65,  7, 243, 287, 0, 191  },
      { 58,  6, 204, 240, 0, 168  },
      { 0, 0 }
    };
    player -> init_mana_costs( ranks );
    rank = choose_rank( ranks );
     
    base_execute_time = 1.5; 
    may_crit          = true;
    direct_power_mod  = base_execute_time / 3.5; 

    base_cost        = rank -> cost;
    base_cost       *= 1.0 -  p -> talents.cataclysm * 0.01;
    base_multiplier *= 1.0 + p -> talents.emberstorm  * 0.03;
    base_crit       += p -> talents.devastation * 0.05;
    base_crit       += p -> talents.backlash * 0.01;
    base_crit       += p -> talents.improved_searing_pain * 0.04;
    base_crit_bonus *= 1.0 + p -> talents.ruin * 0.20;
    base_hit        += p -> talents.cataclysm * 0.01;

    if( p -> glyphs.searing_pain ) base_crit_bonus *= 1.20;
  }

  virtual void execute()
  {
    warlock_spell_t::execute(); 
    if( result_is_hit() )
    {
      trigger_soul_leech( this );
    }
  }
};

// Soul Fire Spell ============================================================

struct soul_fire_t : public warlock_spell_t
{
  soul_fire_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "soul_fire", player, SCHOOL_FIRE, TREE_DESTRUCTION )
  {
    warlock_t* p = player -> cast_warlock();

    option_t options[] =
    {
      { "rank", OPT_INT8, &rank_index },
      { NULL }
    };
    parse_options( options, options_str );
      
    static rank_t ranks[] =
    {
      { 80, 6, 1323, 1657, 0, 0.09 },
      { 75, 5, 1137, 1423, 0, 0.09 },
      { 70, 4, 1003, 1257, 0, 455  },
      { 64, 3,  839, 1051, 0, 390  },
      { 56, 2,  703,  881, 0, 335  },
      { 0, 0 }
    };
    player -> init_mana_costs( ranks );
    rank = choose_rank( ranks );
     
    base_execute_time = 6.0; 
    may_crit          = true; 
    direct_power_mod  = 1.15; 

    base_cost          = rank -> cost;
    base_cost         *= 1.0 -  p -> talents.cataclysm * 0.01;
    base_execute_time -= p -> talents.bane * 0.4;
    base_multiplier   *= 1.0 + p -> talents.emberstorm  * 0.03;
    base_crit         += p -> talents.devastation * 0.05;
    base_crit         += p -> talents.backlash * 0.01;
    base_crit_bonus   *= 1.0 + p -> talents.ruin * 0.20;
    base_hit          += p -> talents.cataclysm * 0.01;
  }

  virtual void execute()
  {
    warlock_spell_t::execute(); 
    if( result_is_hit() )
    {
      trigger_soul_leech( this );
    }
  }
};

// Life Tap Spell ===========================================================

struct life_tap_t : public warlock_spell_t
{
  int16_t trigger;

  life_tap_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "life_tap", player, SCHOOL_SHADOW, TREE_AFFLICTION ), trigger(1000)
  {
    option_t options[] =
    {
      { "trigger", OPT_INT16, &trigger },
      { NULL }
    };
    parse_options( options, options_str );

    static rank_t ranks[] =
    {
      { 80, 8, 0, 0, 1490, 0 },
      { 68, 7, 0, 0,  710, 0 },
      { 56, 6, 0, 0,  500, 0 },
      { 0, 0 }
    };
    player -> init_mana_costs( ranks );
    rank = choose_rank( ranks );
  }

  virtual void execute() 
  {
    warlock_t* p = player -> cast_warlock();
    if( sim -> log ) report_t::log( sim, "%s performs %s", p -> name(), name() );
    p -> procs_life_tap -> occur();
    double dmg = rank -> tick + 3.0 * p -> spirit();
    p -> resource_loss( RESOURCE_HEALTH, dmg );
    p -> resource_gain( RESOURCE_MANA, dmg * ( 1.0 + p -> talents.improved_life_tap * 0.10 ), p -> gains_life_tap );
  }

  virtual bool ready()
  {
    return( player -> resource_current[ RESOURCE_MANA ] < trigger );
  }
};

// Dark Pact Spell =========================================================

struct dark_pact_t : public warlock_spell_t
{
  dark_pact_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "dark_pact", player, SCHOOL_SHADOW, TREE_AFFLICTION )
  {
    warlock_t* p = player -> cast_warlock();
    assert( p -> talents.dark_pact );

    static rank_t ranks[] =
    {
      { 80, 5, 1200, 1200, 0, 0 },
      { 70, 3,  700,  700, 0, 0 },
      { 60, 2,  545,  545, 0, 0 },
	{ 0, 0 }
    };
    player -> init_mana_costs( ranks );
    rank = choose_rank( ranks );

    base_execute_time = 0.0; 
    direct_power_mod  = 0.96;
  }

  virtual void execute() 
  {
    warlock_t* p = player -> cast_warlock();
    if( sim -> log ) report_t::log( sim, "%s performs %s", p -> name(), name() );
    p -> procs_dark_pact -> occur();
    player_buff();
    double mana = ( rank -> dd_max + ( direct_power_mod * p -> composite_spell_power( SCHOOL_SHADOW ) ) ) * base_multiplier * player_multiplier;
    p -> resource_gain( RESOURCE_MANA, mana, p -> gains_dark_pact );
  }

  virtual bool ready()
  {
    return( player -> resource_current[ RESOURCE_MANA ] < 1000 );
  }
};

// Fel Armor Spell ==========================================================

struct fel_armor_t : public warlock_spell_t
{
  double bonus_spell_power;

  fel_armor_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "fel_armor", player, SCHOOL_SHADOW, TREE_DEMONOLOGY ), bonus_spell_power(0)
  {
    warlock_t* p = player -> cast_warlock();
    trigger_gcd = 0;
    bonus_spell_power = ( p -> level < 62 ?   0 :
			  p -> level < 67 ?  50 :
			  p -> level < 73 ? 100 :
			  p -> level < 78 ? 150 : 180 );
    bonus_spell_power *= 1.0 + p -> talents.demonic_aegis * 0.10;
  }

  virtual void execute() 
  {
    warlock_t* p = player -> cast_warlock();
    if( sim -> log ) report_t::log( sim, "%s performs %s", p -> name(), name() );
    p -> buffs_fel_armor = bonus_spell_power;
    p -> buffs_demon_armor = 0;
    p -> spell_power_per_spirit += 0.30 * ( 1.0 + p -> talents.demonic_aegis * 0.10 );
  }

  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();
    return p -> buffs_fel_armor == 0;
  }
};

// Sacrifice Pet Spell =======================================================

struct sacrifice_pet_t : public warlock_spell_t
{
  sacrifice_pet_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "sacrifice_pet", player, SCHOOL_SHADOW, TREE_DEMONOLOGY )
  {
    warlock_t* p = player -> cast_warlock();
    assert( p -> talents.demonic_sacrifice );
    trigger_gcd = 0;
  }

  virtual void execute() 
  {
    warlock_t* p = player -> cast_warlock();
    if( sim -> log ) report_t::log( sim, "%s performs %s", p -> name(), name() );
    p -> buffs_pet_sacrifice = p -> active_pet -> pet_type;
    p -> active_pet -> dismiss();
    p -> active_pet = 0;
  }

  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();
    return p -> active_pet != 0;
  }
};

// Summon Pet Spell ==========================================================

struct summon_pet_t : public warlock_spell_t
{
  std::string pet_name;

  summon_pet_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "summon_pet", player, SCHOOL_SHADOW, TREE_DEMONOLOGY )
  {
    pet_name = options_str;
    trigger_gcd = 0;
  }

  virtual void execute()
  {
    player -> summon_pet( pet_name.c_str() );
  }

  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();
    if( p -> active_pet ) return false;
    if( p -> buffs_pet_sacrifice ) return false;
    return true;
  }
};

// Metamorphosis Spell =======================================================

struct metamorphosis_t : public warlock_spell_t
{
  int8_t target_pct;

  metamorphosis_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "metamorphosis", player, SCHOOL_SHADOW, TREE_DEMONOLOGY ), target_pct(0)
  {
    option_t options[] =
    {
      { "target_pct", OPT_INT8, &target_pct },
      { NULL }
    };
    parse_options( options, options_str );
      
    base_cost   = 0;
    trigger_gcd = 0;
    cooldown    = 180;
  }

  virtual void execute()
  {
    struct expiration_t : public event_t
    {
      expiration_t( sim_t* sim, player_t* player ) : event_t( sim, player )
      {
	name = "Metamorphosis Expiration";
	warlock_t* p = player -> cast_warlock();
	p -> aura_gain( "Metamorphosis" );
	p -> buffs_metamorphosis = 1;
	sim -> add_event( this, 30.0 );
      }
      virtual void execute()
      {
	warlock_t* p = player -> cast_warlock();
	p -> aura_loss( "Metamorphosis" );
	p -> buffs_metamorphosis = 0;
      }
    };

    if( sim -> log ) report_t::log( sim, "%s performs %s", player -> name(), name() );
    update_ready();
    player -> action_finish( this );
    new ( sim ) expiration_t( sim, player );
  }

  virtual bool ready()
  {
    if( ! warlock_spell_t::ready() )
      return false;

    target_t* t = sim -> target;

    if( target_pct == 0 )
      return true;

    if( t -> initial_health <= 0 )
      return false;

    return( ( t -> current_health / t -> initial_health ) < ( target_pct / 100.0 ) );
  }
};

// Demonic Empowerment Spell ================================================

struct demonic_empowerment_t : public warlock_spell_t
{
  int8_t target_pct;

  demonic_empowerment_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "demonic_empowerment", player, SCHOOL_SHADOW, TREE_DEMONOLOGY ), target_pct(0)
  {
    option_t options[] =
    {
      { "target_pct", OPT_INT8, &target_pct },
      { NULL }
    };
    parse_options( options, options_str );
      
    base_cost = player -> resource_base[ RESOURCE_MANA ] * 0.06;
    cooldown  = 60;
    trigger_gcd = 0;
  }

  virtual void execute()
  {
    struct expiration_t : public event_t
    {
      expiration_t( sim_t* sim, warlock_pet_t* pet ) : event_t( sim, pet )
      {
	name = "Demonic Empowerment Expiration";
	pet -> aura_gain( "Demonic Empowerment" );
	pet -> buffs_demonic_empowerment = 1;
	double duration = ( pet -> pet_type == PET_FELGUARD ? 15.0 :
			    pet -> pet_type == PET_IMP      ? 30.0 : 0.01 );
	sim -> add_event( this, duration );
      }
      virtual void execute()
      {
	warlock_pet_t* pet = (warlock_pet_t*) player -> cast_pet();
	pet -> aura_loss( "Demonic Empowerment" );
	pet -> buffs_demonic_empowerment = 0;
      }
    };

    warlock_t* p = player -> cast_warlock();
    if( sim -> log ) report_t::log( sim, "%s performs %s", p -> name(), name() );
    update_ready();
    p -> action_finish( this );
    new ( sim ) expiration_t( sim, p -> active_pet );
  }

  virtual bool ready()
  {
    warlock_t* p = player -> cast_warlock();

    if( ! warlock_spell_t::ready() )
      return false;

    if( ! p -> active_pet )
      return false;

    target_t* t = sim -> target;

    if( target_pct == 0 )
      return true;

    if( t -> initial_health <= 0 )
      return false;

    return( ( t -> current_health / t -> initial_health ) < ( target_pct / 100.0 ) );
  }
};

// Fire Stone Spell ===========================================================

struct fire_stone_t : public warlock_spell_t
{
  int16_t bonus_crit;

  fire_stone_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "fire_stone", player, SCHOOL_FIRE, TREE_DEMONOLOGY )
  {
    warlock_t* p = player -> cast_warlock();
    trigger_gcd = 0;

    bonus_crit = ( ( p -> level <= 56 ) ? 28  : 
		   ( p -> level <= 66 ) ? 35  :
		   ( p -> level <= 74 ) ? 42  : 49 );

    bonus_crit = (int16_t) ( bonus_crit * ( 1.0 + p -> talents.master_conjuror * 0.15 ) );
  }

  virtual void execute()
  {
    if( sim -> log ) report_t::log( sim, "%s performs %s", player -> name(), name() );

    player -> main_hand_weapon.buff = FIRE_STONE;
    player -> spell_crit += bonus_crit / player -> rating.spell_crit;
  };

  virtual bool ready()
  {
    return( player -> main_hand_weapon.buff != FIRE_STONE );
  }
};

// Spell Stone Spell ===========================================================

struct spell_stone_t : public warlock_spell_t
{
  int16_t bonus_haste;

  spell_stone_t( player_t* player, const std::string& options_str ) : 
    warlock_spell_t( "spell_stone", player, SCHOOL_FIRE, TREE_DEMONOLOGY )
  {
    warlock_t* p = player -> cast_warlock();
    trigger_gcd = 0;

    bonus_haste = ( ( p -> level <= 60 ) ? 30  : 
		    ( p -> level <= 66 ) ? 40  :
		    ( p -> level <= 72 ) ? 50  : 60 );
    
    bonus_haste = (int16_t) ( bonus_haste * ( 1.0 + p -> talents.master_conjuror * 0.15 ) );
  }

  virtual void execute()
  {
    if( sim -> log ) report_t::log( sim, "%s performs %s", player -> name(), name() );

    player -> main_hand_weapon.buff = SPELL_STONE;
    player -> haste_rating += bonus_haste;
    player -> recalculate_haste();
  };

  virtual bool ready()
  {
    return( player -> main_hand_weapon.buff != SPELL_STONE );
  }
};

// ==========================================================================
// Warlock Character Definition
// ==========================================================================

// warlock_t::composite_spell_power =========================================

double warlock_t::composite_spell_power( int8_t school )
{
  double sp = player_t::composite_spell_power( school );

  sp += buffs_fel_armor;

  if( active_pet && talents.demonic_knowledge )
  {
    sp += ( active_pet -> stamina() + 
	    active_pet -> intellect() ) * talents.demonic_knowledge * 0.04;
  }

  return sp;
}

// warlock_t::create_action =================================================

action_t* warlock_t::create_action( const std::string& name,
				    const std::string& options_str )
{
  if( name == "chaos_bolt"          ) return new          chaos_bolt_t( this, options_str );
  if( name == "conflagrate"         ) return new         conflagrate_t( this, options_str );
  if( name == "corruption"          ) return new          corruption_t( this, options_str );
  if( name == "curse_of_agony"      ) return new      curse_of_agony_t( this, options_str );
  if( name == "curse_of_doom"       ) return new       curse_of_doom_t( this, options_str );
  if( name == "curse_of_elements"   ) return new   curse_of_elements_t( this, options_str );
  if( name == "dark_pact"           ) return new           dark_pact_t( this, options_str );
  if( name == "death_coil"          ) return new          death_coil_t( this, options_str );
  if( name == "demonic_empowerment" ) return new demonic_empowerment_t( this, options_str );
  if( name == "drain_life"          ) return new          drain_life_t( this, options_str );
  if( name == "drain_soul"          ) return new          drain_soul_t( this, options_str );
  if( name == "fel_armor"           ) return new           fel_armor_t( this, options_str );
  if( name == "fire_stone"          ) return new          fire_stone_t( this, options_str );
  if( name == "spell_stone"         ) return new         spell_stone_t( this, options_str );
  if( name == "haunt"               ) return new               haunt_t( this, options_str );
  if( name == "immolate"            ) return new            immolate_t( this, options_str );
  if( name == "incinerate"          ) return new          incinerate_t( this, options_str );
  if( name == "life_tap"            ) return new            life_tap_t( this, options_str );
  if( name == "metamorphosis"       ) return new       metamorphosis_t( this, options_str );
  if( name == "sacrifice_pet"       ) return new       sacrifice_pet_t( this, options_str );
  if( name == "shadow_bolt"         ) return new         shadow_bolt_t( this, options_str );
  if( name == "shadow_burn"         ) return new         shadow_burn_t( this, options_str );
  if( name == "searing_pain"        ) return new        searing_pain_t( this, options_str );
  if( name == "soul_fire"           ) return new           soul_fire_t( this, options_str );
  if( name == "siphon_life"         ) return new         siphon_life_t( this, options_str );
  if( name == "summon_pet"          ) return new          summon_pet_t( this, options_str );
  if( name == "unstable_affliction" ) return new unstable_affliction_t( this, options_str );

  return 0;
}

// warlock_t::create_pet =====================================================

pet_t* warlock_t::create_pet( const std::string& pet_name )
{
  if( pet_name == "felguard"  ) return new  felguard_pet_t( sim, this, pet_name );
  if( pet_name == "felhunter" ) return new felhunter_pet_t( sim, this, pet_name );
  if( pet_name == "imp"       ) return new       imp_pet_t( sim, this, pet_name );
  if( pet_name == "succubus"  ) return new  succubus_pet_t( sim, this, pet_name );

  return 0;
}

// warlock_t::init_base ======================================================

void warlock_t::init_base()
{
  attribute_base[ ATTR_STRENGTH  ] =  54;
  attribute_base[ ATTR_AGILITY   ] =  55;
  attribute_base[ ATTR_STAMINA   ] =  78;
  attribute_base[ ATTR_INTELLECT ] = 130;
  attribute_base[ ATTR_SPIRIT    ] = 142;

  attribute_multiplier_initial[ ATTR_STAMINA ] *= 1.0 + talents.demonic_embrace * 0.02;

  base_spell_crit = 0.0170;
  initial_spell_crit_per_intellect = rating_t::interpolate( level, 0.01/60.0, 0.01/80.0, 0.01/166.6 );

  base_attack_power = -10;
  base_attack_crit  = 0.02;
  initial_attack_power_per_strength = 1.0;
  initial_attack_crit_per_agility = rating_t::interpolate( level, 0.01/16.0, 0.01/24.9, 0.01/52.1 );

  // FIXME! Make this level-specific.
  resource_base[ RESOURCE_HEALTH ] = 3200;
  resource_base[ RESOURCE_MANA   ] = rating_t::interpolate( level, 1383, 2620, 3863 );

  health_per_stamina = 10;
  mana_per_intellect = 15;

  mana_per_intellect *= 1.0 + ( talents.fel_intellect + talents.fel_vitality ) * 0.01;
  health_per_stamina *= 1.0 + ( talents.fel_stamina   + talents.fel_vitality ) * 0.01;
}

// warlock_t::reset ==========================================================

void warlock_t::reset()
{
  player_t::reset();

  // Active
  active_pet        = 0;
  active_corruption = 0;
  active_curse      = 0;
  active_immolate   = 0;
  active_dots       = 0;

  // Buffs
  buffs_backdraft                    = 0;
  buffs_demon_armor                  = 0;
  buffs_demonic_empathy              = 0;
  buffs_empowered_imp                = 0;
  buffs_eradication                  = 0;
  buffs_fel_armor                    = 0;
  buffs_flame_shadow                 = 0;
  buffs_haunted                      = 0;
  buffs_metamorphosis                = 0;
  buffs_molten_core                  = 0;
  buffs_pet_sacrifice                = 0;
  buffs_shadow_embrace               = 0;
  buffs_shadow_flame                 = 0;
  buffs_shadow_trance                = 0;
  buffs_shadow_vulnerability         = 0;
  buffs_shadow_vulnerability_charges = 0;

  // Cooldowns
  cooldowns_eradication              = 0;

  // Expirations
  expirations_backdraft              = 0;
  expirations_demonic_empathy        = 0;
  expirations_empowered_imp          = 0;
  expirations_flame_shadow           = 0;
  expirations_haunted                = 0;
  expirations_molten_core            = 0;
  expirations_shadow_embrace         = 0;
  expirations_shadow_flame           = 0;
  expirations_shadow_vulnerability   = 0;
}

// warlock_t::regen ==========================================================

void warlock_t::regen( double periodicity )
{
  player_t::regen( periodicity );

  if( buffs_pet_sacrifice == PET_FELHUNTER )
  {
    double felhunter_regen = periodicity * resource_max[ RESOURCE_MANA ] * 0.03 / 4.0;

    resource_gain( RESOURCE_MANA, felhunter_regen, gains_felhunter );
  }
}

// warlock_t::get_talent_trees =============================================

bool warlock_t::get_talent_trees( std::vector<int8_t*>& affliction,
				  std::vector<int8_t*>& demonology,
				  std::vector<int8_t*>& destruction )
{
  talent_translation_t translation[][3] =
  {
    { {  1, &( talents.improved_curse_of_agony ) }, {  1, NULL                                  }, {  1, &( talents.improved_shadow_bolt  ) } },
    { {  2, &( talents.suppression             ) }, {  2, &( talents.improved_imp             ) }, {  2, &( talents.bane                  ) } },
    { {  3, &( talents.improved_corruption     ) }, {  3, &( talents.demonic_embrace          ) }, {  3, NULL                               } },
    { {  4, NULL                                 }, {  4, NULL                                  }, {  4, &( talents.molten_core           ) } },
    { {  5, &( talents.improved_drain_soul     ) }, {  5, &( talents.demonic_brutality        ) }, {  5, &( talents.cataclysm             ) } },
    { {  6, &( talents.improved_life_tap       ) }, {  6, &( talents.fel_vitality             ) }, {  6, &( talents.demonic_power         ) } },
    { {  7, &( talents.soul_siphon             ) }, {  7, NULL                                  }, {  7, &( talents.shadow_burn           ) } },
    { {  8, NULL                                 }, {  8, NULL                                  }, {  8, &( talents.ruin                  ) } },
    { {  9, NULL                                 }, {  9, &( talents.fel_domination           ) }, {  9, NULL                               } },
    { { 10, &( talents.amplify_curse           ) }, { 10, &( talents.demonic_aegis            ) }, { 10, &( talents.destructive_reach     ) } },
    { { 11, NULL                                 }, { 11, &( talents.unholy_power             ) }, { 11, &( talents.improved_searing_pain ) } },
    { { 12, &( talents.nightfall               ) }, { 12, &( talents.master_summoner          ) }, { 12, NULL                               } },
    { { 13, &( talents.empowered_corruption    ) }, { 13, &( talents.demonic_sacrifice        ) }, { 13, &( talents.improved_immolate     ) } },
    { { 14, &( talents.shadow_embrace          ) }, { 14, &( talents.master_conjuror          ) }, { 14, &( talents.devastation           ) } },
    { { 15, &( talents.siphon_life             ) }, { 15, &( talents.mana_feed                ) }, { 15, NULL                               } },
    { { 16, NULL                                 }, { 16, &( talents.master_demonologist      ) }, { 16, &( talents.emberstorm            ) } },
    { { 17, &( talents.improved_felhunter      ) }, { 17, NULL                                  }, { 17, &( talents.conflagrate           ) } },
    { { 18, &( talents.shadow_mastery          ) }, { 18, NULL                                  }, { 18, &( talents.soul_leech            ) } },
    { { 19, &( talents.eradication             ) }, { 19, &( talents.demonic_empowerment      ) }, { 19, &( talents.backlash              ) } },
    { { 20, &( talents.contagion               ) }, { 20, &( talents.demonic_knowledge        ) }, { 20, &( talents.shadow_and_flame      ) } },
    { { 21, &( talents.dark_pact               ) }, { 21, &( talents.demonic_tactics          ) }, { 21, &( talents.improved_soul_leech   ) } },
    { { 22, NULL                                 }, { 22, &( talents.fel_synergy              ) }, { 22, &( talents.backdraft             ) } },
    { { 23, &( talents.malediction             ) }, { 23, &( talents.improved_demonic_tactics ) }, { 23, NULL                               } },
    { { 24, &( talents.deaths_embrace          ) }, { 24, &( talents.summon_felguard          ) }, { 24, &( talents.empowered_imp         ) } },
    { { 25, &( talents.unstable_affliction     ) }, { 25, &( talents.demonic_empathy          ) }, { 25, &( talents.fire_and_brimstone    ) } },
    { { 26, &( talents.pandemic                ) }, { 26, &( talents.demonic_pact             ) }, { 26, &( talents.chaos_bolt            ) } },
    { { 27, &( talents.everlasting_affliction  ) }, { 27, &( talents.metamorphosis            ) }, {  0, NULL                               } },
    { { 28, &( talents.haunt                   ) }, {  0, NULL                                  }, {  0, NULL                               } },
    { {  0, NULL                                 }, {  0, NULL                                  }, {  0, NULL                               } }
  };
  
  return player_t::get_talent_trees( affliction, demonology, destruction, translation );
}

// warlock_t::parse_talents_mmo ============================================

bool warlock_t::parse_talents_mmo( const std::string& talent_string )
{
  // warlock mmo encoding: Destruction-Affliction-Demonology

  int size1 = 26;
  int size2 = 28;

  std::string destruction_string( talent_string,     0,  size1 );
  std::string  affliction_string( talent_string, size1,  size2 );
  std::string  demonology_string( talent_string, size1 + size2 );

  if( sim -> debug )
  {
    fprintf( sim -> output_file, "%s affliction:  %s\n", name(),  affliction_string.c_str() );
    fprintf( sim -> output_file, "%s demonology:  %s\n", name(),  demonology_string.c_str() );
    fprintf( sim -> output_file, "%s destruction: %s\n", name(), destruction_string.c_str() );
  }

  return parse_talents( affliction_string + demonology_string + destruction_string );
}

// warlock_t::parse_option =================================================

bool warlock_t::parse_option( const std::string& name,
			      const std::string& value )
{
  option_t options[] =
  {
    { "amplify_curse",            OPT_INT8,  &( talents.amplify_curse            ) },
    { "backdraft",                OPT_INT8,  &( talents.backdraft                ) },
    { "backlash",                 OPT_INT8,  &( talents.backlash                 ) },
    { "bane",                     OPT_INT8,  &( talents.bane                     ) },
    { "cataclysm",                OPT_INT8,  &( talents.cataclysm                ) },
    { "chaos_bolt",               OPT_INT8,  &( talents.chaos_bolt               ) },
    { "conflagrate",              OPT_INT8,  &( talents.conflagrate              ) },
    { "contagion",                OPT_INT8,  &( talents.contagion                ) },
    { "dark_pact",                OPT_INT8,  &( talents.dark_pact                ) },
    { "deaths_embrace",           OPT_INT8,  &( talents.deaths_embrace           ) },
    { "demonic_aegis",            OPT_INT8,  &( talents.demonic_aegis            ) },
    { "demonic_embrace",          OPT_INT8,  &( talents.demonic_embrace          ) },
    { "demonic_empathy",          OPT_INT8,  &( talents.demonic_empathy          ) },
    { "demonic_empowerment",      OPT_INT8,  &( talents.demonic_empowerment      ) },
    { "demonic_knowledge",        OPT_INT8,  &( talents.demonic_knowledge        ) },
    { "demonic_pact",             OPT_INT8,  &( talents.demonic_pact             ) },
    { "demonic_power",            OPT_INT8,  &( talents.demonic_power            ) },
    { "demonic_sacrifice",        OPT_INT8,  &( talents.demonic_sacrifice        ) },
    { "demonic_tactics",          OPT_INT8,  &( talents.demonic_tactics          ) },
    { "destructive_reach",        OPT_INT8,  &( talents.destructive_reach        ) },
    { "devastation",              OPT_INT8,  &( talents.devastation              ) },
    { "emberstorm",               OPT_INT8,  &( talents.emberstorm               ) },
    { "empowered_corruption",     OPT_INT8,  &( talents.empowered_corruption     ) },
    { "empowered_imp",            OPT_INT8,  &( talents.empowered_imp            ) },
    { "eradication",              OPT_INT8,  &( talents.eradication              ) },
    { "everlasting_affliction",   OPT_INT8,  &( talents.everlasting_affliction   ) },
    { "fel_domination",           OPT_INT8,  &( talents.fel_domination           ) },
    { "fel_intellect",            OPT_INT8,  &( talents.fel_intellect            ) },
    { "fel_stamina",              OPT_INT8,  &( talents.fel_stamina              ) },
    { "fel_synergy",              OPT_INT8,  &( talents.fel_synergy              ) },
    { "fel_vitality",             OPT_INT8,  &( talents.fel_vitality             ) },
    { "fire_and_brimstone",       OPT_INT8,  &( talents.fire_and_brimstone       ) },
    { "haunt",                    OPT_INT8,  &( talents.haunt                    ) },
    { "improved_corruption",      OPT_INT8,  &( talents.improved_corruption      ) },
    { "improved_curse_of_agony",  OPT_INT8,  &( talents.improved_curse_of_agony  ) },
    { "improved_demonic_tactics", OPT_INT8,  &( talents.improved_demonic_tactics ) },
    { "improved_drain_soul",      OPT_INT8,  &( talents.improved_drain_soul      ) },
    { "improved_felhunter",       OPT_INT8,  &( talents.improved_felhunter       ) },
    { "improved_fire_bolt",       OPT_INT8,  &( talents.improved_fire_bolt       ) },
    { "improved_immolate",        OPT_INT8,  &( talents.improved_immolate        ) },
    { "improved_imp",             OPT_INT8,  &( talents.improved_imp             ) },
    { "improved_lash_of_pain",    OPT_INT8,  &( talents.improved_lash_of_pain    ) },
    { "improved_life_tap",        OPT_INT8,  &( talents.improved_life_tap        ) },
    { "improved_searing_pain",    OPT_INT8,  &( talents.improved_searing_pain    ) },
    { "improved_shadow_bolt",     OPT_INT8,  &( talents.improved_shadow_bolt     ) },
    { "improved_soul_leech",      OPT_INT8,  &( talents.improved_soul_leech      ) },
    { "improved_succubus",        OPT_INT8,  &( talents.improved_succubus        ) },
    { "improved_voidwalker",      OPT_INT8,  &( talents.improved_voidwalker      ) },
    { "malediction",              OPT_INT8,  &( talents.malediction              ) },
    { "mana_feed",                OPT_INT8,  &( talents.mana_feed                ) },
    { "master_conjuror",          OPT_INT8,  &( talents.master_conjuror          ) },
    { "master_demonologist",      OPT_INT8,  &( talents.master_demonologist      ) },
    { "master_summoner",          OPT_INT8,  &( talents.master_summoner          ) },
    { "metamorphosis",            OPT_INT8,  &( talents.metamorphosis            ) },
    { "molten_core",              OPT_INT8,  &( talents.molten_core              ) },
    { "nightfall",                OPT_INT8,  &( talents.nightfall                ) },
    { "pandemic",                 OPT_INT8,  &( talents.pandemic                 ) },
    { "ruin",                     OPT_INT8,  &( talents.ruin                     ) },
    { "shadow_and_flame",         OPT_INT8,  &( talents.shadow_and_flame         ) },
    { "shadow_burn",              OPT_INT8,  &( talents.shadow_burn              ) },
    { "shadow_mastery",           OPT_INT8,  &( talents.shadow_mastery           ) },
    { "siphon_life",              OPT_INT8,  &( talents.siphon_life              ) },
    { "soul_leech",               OPT_INT8,  &( talents.soul_leech               ) },
    { "soul_link",                OPT_INT8,  &( talents.soul_link                ) },
    { "soul_siphon",              OPT_INT8,  &( talents.soul_siphon              ) },
    { "summon_felguard",          OPT_INT8,  &( talents.summon_felguard          ) },
    { "suppression",              OPT_INT8,  &( talents.suppression              ) },
    { "unholy_power",             OPT_INT8,  &( talents.unholy_power             ) },
    { "unstable_affliction",      OPT_INT8,  &( talents.unstable_affliction      ) },
    // Glyphs
    { "glyph_conflagrate",        OPT_INT8,  &( glyphs.conflagrate               ) },
    { "glyph_corruption",         OPT_INT8,  &( glyphs.corruption                ) },
    { "glyph_curse_of_agony",     OPT_INT8,  &( glyphs.curse_of_agony            ) },
    { "glyph_felguard",           OPT_INT8,  &( glyphs.felguard                  ) },
    { "glyph_felhunter",          OPT_INT8,  &( glyphs.felhunter                 ) },
    { "glyph_immolate",           OPT_INT8,  &( glyphs.immolate                  ) },
    { "glyph_imp",                OPT_INT8,  &( glyphs.imp                       ) },
    { "glyph_searing_pain",       OPT_INT8,  &( glyphs.searing_pain              ) },
    { "glyph_shadow_bolt",        OPT_INT8,  &( glyphs.shadow_bolt               ) },
    { "glyph_shadow_burn",        OPT_INT8,  &( glyphs.shadow_burn               ) },
    { "glyph_siphon_life",        OPT_INT8,  &( glyphs.siphon_life               ) },
    { NULL, OPT_UNKNOWN }
  };

  if( name.empty() )
  {
    player_t::parse_option( std::string(), std::string() );
    option_t::print( sim, options );
    return false;
  }

  if( player_t::parse_option( name, value ) ) return true;

  return option_t::parse( sim, options, name, value );
}

// player_t::create_warlock ================================================

player_t* player_t::create_warlock( sim_t*       sim, 
				    std::string& name ) 
{
  return new warlock_t( sim, name );
}



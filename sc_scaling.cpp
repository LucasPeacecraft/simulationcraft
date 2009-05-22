// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simcraft.h"

namespace { // ANONYMOUS NAMESPACE ==========================================

// is_scaling_stat ===========================================================

static bool is_scaling_stat( sim_t* sim,
			     int    stat )
{
  for( player_t* p = sim -> player_list; p; p = p -> next )
  {
    if( p -> quiet ) continue;
    if( p -> is_pet() ) continue;

    if( p -> scales_with[ stat ] ) return true;
  }
  
  return false;
}

// stat_may_cap ==============================================================

static bool stat_may_cap( int stat )
{
  if( stat == STAT_HIT_RATING ) return true;
  if( stat == STAT_EXPERTISE_RATING ) return true;
  return false;
}

} // ANONYMOUS NAMESPACE =====================================================

// ==========================================================================
// Scaling
// ==========================================================================

// scaling_t::scaling_t =====================================================

scaling_t::scaling_t( sim_t* s ) : 
  sim(s), 
  calculate_scale_factors(0), 
  center_scale_delta(0),
  scale_lag(1),
  scale_factor_noise(0)
{
}

// scaling_t::init_deltas ===================================================

void scaling_t::init_deltas()
{
  for( int i=ATTRIBUTE_NONE+1; i < ATTRIBUTE_MAX; i++ )
  {
    if( stats.attribute[ i ] == 0 ) stats.attribute[ i ] = sim -> normalized_sf ? 100 : 250;
  }

  if( stats.spell_power == 0 ) stats.spell_power = sim -> normalized_sf ? 100 : 250;

  if( stats.attack_power             == 0 ) stats.attack_power             =  sim -> normalized_sf ?  100 :  250;
  if( stats.armor_penetration_rating == 0 ) stats.armor_penetration_rating =  sim -> normalized_sf ?  100 :  250;
  if( stats.expertise_rating         == 0 ) stats.expertise_rating         =  sim -> normalized_sf ?  -75 : -150;

  if( stats.hit_rating   == 0 ) stats.hit_rating   = sim -> normalized_sf ? -100 : -200;
  if( stats.crit_rating  == 0 ) stats.crit_rating  = sim -> normalized_sf ?  100 :  250;
  if( stats.haste_rating == 0 ) stats.haste_rating = sim -> normalized_sf ?  100 :  250;

  if( stats.weapon_dps == 0 ) stats.weapon_dps = sim -> normalized_sf ? 25 : 50;
}

// scaling_t::analyze_stats =================================================

void scaling_t::analyze_stats()
{
  if( ! calculate_scale_factors ) return;

  int num_players = sim -> players_by_name.size();
  if( num_players == 0 ) return;

  for( int i=0; i < STAT_MAX; i++ )
  {
    if( ! is_scaling_stat( sim, i ) ) continue;

    int scale_delta = (int) stats.get_stat( i );
    if( scale_delta == 0 ) continue;

    fprintf( stdout, "\nGenerating scale factors for %s...\n", util_t::stat_type_string( i ) );
    fflush( stdout );

    bool center = center_scale_delta && ! stat_may_cap( i );

    sim_t* child_sim = new sim_t( sim );
    child_sim -> gear_delta.set_stat( i, +scale_delta / ( center ? 2 : 1 ) );
    child_sim -> normalized_rng     += sim -> normalized_sf;
    child_sim -> deterministic_roll += sim -> normalized_sf;
    child_sim -> average_range      += sim -> normalized_sf;
    child_sim -> execute();

    sim_t* ref_sim = sim;
    if( center || sim -> normalized_sf )
    {
      ref_sim = new sim_t( sim );
      ref_sim -> gear_delta.set_stat( i, center ? -( scale_delta / 2 ) : 0 );
      ref_sim -> normalized_rng     += sim -> normalized_sf;
      ref_sim -> deterministic_roll += sim -> normalized_sf;
      ref_sim -> average_range      += sim -> normalized_sf;
      ref_sim -> execute();
    }

    for( int j=0; j < num_players; j++ )
    {
      player_t* p = sim -> players_by_name[ j ];

      if( ! p -> scales_with[ i ] ) continue;

      player_t*   ref_p =   ref_sim -> find_player( p -> name() );
      player_t* child_p = child_sim -> find_player( p -> name() );

      double f = ( child_p -> dps - ref_p -> dps ) / scale_delta;

      if( f >= scale_factor_noise ) p -> scaling.set_stat( i, f );
    }

    if( ref_sim != sim ) delete ref_sim;
    delete child_sim;
  }
}

// scaling_t::analyze_lag ===================================================

void scaling_t::analyze_lag()
{
  if( ! calculate_scale_factors || ! scale_lag ) return;

  int num_players = sim -> players_by_name.size();
  if( num_players == 0 ) return;

  fprintf( stdout, "\nGenerating scale factors for lag...\n" );
  fflush( stdout );

  sim_t* child_sim = new sim_t( sim );
  child_sim ->   queue_lag *= 1.100;
  child_sim ->     gcd_lag *= 1.100;
  child_sim -> channel_lag *= 1.100;
  child_sim -> normalized_rng     += sim -> normalized_sf;
  child_sim -> deterministic_roll += sim -> normalized_sf;
  child_sim -> average_range      += sim -> normalized_sf;
  child_sim -> execute();

  sim_t* ref_sim = sim;
  if( sim -> normalized_sf )
  {
    ref_sim = new sim_t( sim );
    ref_sim -> normalized_rng     += sim -> normalized_sf;
    ref_sim -> deterministic_roll += sim -> normalized_sf;
    ref_sim -> average_range      += sim -> normalized_sf;
    ref_sim -> execute();
  }

  for( int i=0; i < num_players; i++ )
  {
    player_t*       p =       sim -> players_by_name[ i ];
    player_t*   ref_p =   ref_sim -> find_player( p -> name() );
    player_t* child_p = child_sim -> find_player( p -> name() );

    // Calculate DPS difference per millisecond of lag
    double f = ( ref_p -> dps - child_p -> dps ) / ( ( child_sim -> queue_lag - sim -> queue_lag ) * 1000 );

    if( f >= scale_factor_noise ) p -> scaling_lag = f;
  }
  
  if( ref_sim != sim ) delete ref_sim;
  delete child_sim;
}

// scaling_t::analyze_gear_weights ==========================================

void scaling_t::analyze_gear_weights()
{
  for( player_t* p = sim -> player_list; p; p = p -> next )
  {
    if( p -> quiet ) continue;

    chart_t::gear_weights_lootrank( p -> gear_weights_lootrank_link, p );
    chart_t::gear_weights_wowhead ( p -> gear_weights_wowhead_link,  p );
    chart_t::gear_weights_pawn    ( p -> gear_weights_pawn_string,   p );
  }
}

// scaling_t::analyze =======================================================

void scaling_t::analyze()
{
  init_deltas();
  analyze_stats();
  analyze_lag();
  analyze_gear_weights();
}

// scaling_t::parse_option ==================================================

bool scaling_t::parse_option( const std::string& name,
                              const std::string& value )
{
  option_t options[] =
  {
    // @option_doc loc=global/scale_factors title="Scale Factors"
    { "calculate_scale_factors",        OPT_BOOL, &( calculate_scale_factors              ) },
    { "center_scale_delta",             OPT_BOOL, &( center_scale_delta                   ) },
    { "scale_lag",                      OPT_BOOL, &( scale_lag                            ) },
    { "scale_factor_noise",             OPT_FLT,  &( scale_factor_noise                   ) },
    { "scale_strength",                 OPT_FLT,  &( stats.attribute[ ATTR_STRENGTH  ]    ) },
    { "scale_agility",                  OPT_FLT,  &( stats.attribute[ ATTR_AGILITY   ]    ) },
    { "scale_stamina",                  OPT_FLT,  &( stats.attribute[ ATTR_STAMINA   ]    ) },
    { "scale_intellect",                OPT_FLT,  &( stats.attribute[ ATTR_INTELLECT ]    ) },
    { "scale_spirit",                   OPT_FLT,  &( stats.attribute[ ATTR_SPIRIT    ]    ) },
    { "scale_spell_power",              OPT_FLT,  &( stats.spell_power                    ) },
    { "scale_attack_power",             OPT_FLT,  &( stats.attack_power                   ) },
    { "scale_expertise_rating",         OPT_FLT,  &( stats.expertise_rating               ) },
    { "scale_armor_penetration_rating", OPT_FLT,  &( stats.armor_penetration_rating       ) },
    { "scale_hit_rating",               OPT_FLT,  &( stats.hit_rating                     ) },
    { "scale_crit_rating",              OPT_FLT,  &( stats.crit_rating                    ) },
    { "scale_haste_rating",             OPT_FLT,  &( stats.haste_rating                   ) },
    { "scale_weapon_dps",               OPT_FLT,  &( stats.weapon_dps                     ) },
    { NULL, OPT_UNKNOWN }
  };

  if( name.empty() )
  {
    option_t::print( sim -> output_file, options );
    return false;
  }

  return option_t::parse( sim, options, name, value );
}

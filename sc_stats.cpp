// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include <simcraft.h>

// stats_t::stats_t =========================================================

stats_t::stats_t( action_t* a ) : 
  name(a->name_str), sim(a->sim), player(a->player), channeled(a->channeled), next(0)
{
  
}

// stats_t::init ============================================================

void stats_t::init()
{
  num_buckets = (int) sim -> max_time;
  if( num_buckets == 0 ) num_buckets = 600; // Default to 10 minutes
  num_buckets *= 2;

  timeline_dmg.clear();
  timeline_dmg.insert( timeline_dmg.begin(), num_buckets, 0 );

  for( int i=0; i < RESULT_MAX; i++ )
  {
    execute_results[ i ].count     = 0;
    execute_results[ i ].min_dmg   = FLT_MAX;
    execute_results[ i ].max_dmg   = 0;
    execute_results[ i ].avg_dmg   = 0;
    execute_results[ i ].total_dmg = 0;

    tick_results[ i ].count     = 0;
    tick_results[ i ].min_dmg   = FLT_MAX;
    tick_results[ i ].max_dmg   = 0;
    tick_results[ i ].avg_dmg   = 0;
    tick_results[ i ].total_dmg = 0;
  }

  num_executes = num_ticks = 0;
  total_execute_time = total_tick_time = 0;
  total_dmg = 0;
  dps = dpe = 0;
}

// stats_t::add =============================================================

void stats_t::add( double amount,
		   int8_t dmg_type,
		   int8_t result,
		   double time )
{
  player -> iteration_dmg += amount;
  total_dmg += amount;

  if( dmg_type == DMG_DIRECT )
  {
    if( ! channeled ) time = std::max( time, player -> gcd() );
    num_executes++;
    total_execute_time += time;
    stats_results_t& r = execute_results[ result ];
    r.count += 1;
    r.total_dmg += amount;
    if( amount < r.min_dmg ) r.min_dmg = amount;
    if( amount > r.max_dmg ) r.max_dmg = amount;
  }
  else
  {
    num_ticks++;
    total_tick_time += time;
    stats_results_t& r = tick_results[ result ];
    r.count += 1;
    r.total_dmg += amount;
    if( amount < r.min_dmg ) r.min_dmg = amount;
    if( amount > r.max_dmg ) r.max_dmg = amount;
  }

  int index = (int) ( player -> sim -> current_time );
  if( index >= num_buckets )
  {
    // Double the size of the vector, initializing the new elements to zero.
    timeline_dmg.insert( timeline_dmg.begin() + num_buckets, num_buckets, 0 );
    num_buckets *= 2;
  }

  timeline_dmg[ index ] += amount;
}

// stats_t::analyze =========================================================

void stats_t::analyze()
{
  int num_iterations = player -> sim -> iterations;

  for( int i=0; i < RESULT_MAX; i++ )
  {
    if( execute_results[ i ].count != 0 )
    {
      stats_results_t& r = execute_results[ i ];

      r.avg_dmg = r.total_dmg / r.count;

      r.count     /= num_iterations;
      r.total_dmg /= num_iterations;
    }
    if( tick_results[ i ].count != 0 )
    {
      stats_results_t& r = tick_results[ i ];

      r.avg_dmg = r.total_dmg / r.count;

      r.count     /= num_iterations;
      r.total_dmg /= num_iterations;
    }
  }

  if( total_dmg > 0 )
  {
    dps = total_dmg / ( total_execute_time + total_tick_time );
    dpe = total_dmg / num_executes;
  }

  num_executes /= num_iterations;
  num_ticks    /= num_iterations;

  total_execute_time /= num_iterations;
  total_tick_time    /= num_iterations;
  total_dmg          /= num_iterations;

  timeline_dps.clear();
  timeline_dps.insert( timeline_dps.begin(), num_buckets, 0 );

  for( int i=0; i < num_buckets; i++ )
  {
    double window_dmg  = timeline_dmg[ i ];
    int    window_size = 1;

    for( int j=1; ( j <= 10 ) && ( (i-j) >=0 ); j++ )
    {
      window_dmg += timeline_dmg[ i-j ];
      window_size++;
    }
    for( int j=1; ( j <= 10 ) && ( (i+j) < num_buckets ); j++ )
    {
      window_dmg += timeline_dmg[ i+j ];
      window_size++;
    }

    window_dmg /= num_iterations;

    timeline_dps[ i ] = window_dmg / window_size;
  }
}


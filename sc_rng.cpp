// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simcraft.h"

// ==========================================================================
// Standard Random Number Generator
// ==========================================================================

// rng_t::rng_t =============================================================

rng_t::rng_t( const std::string& n, bool avg_range, bool avg_gauss ) : 
  name_str(n), gauss_pair_use(false), 
  expected_roll(0),  actual_roll(0),
  expected_range(0), actual_range(0),
  expected_gauss(0), actual_gauss(0),
  average_range(avg_range), 
  average_gauss(avg_gauss),
  next(0)
{
}

// rng_t::real ==============================================================

double rng_t::real()
{
  return rand() * ( 1.0 / ( ( (double) RAND_MAX ) + 1.0 ) );
}

// rng_t::roll ==============================================================

int rng_t::roll( double chance )
{
  if( chance <= 0 ) return 0;
  if( chance >= 1 ) return 1;
  int result = ( real() < chance ) ? 1 : 0;
  expected_roll += chance;
  if( result ) actual_roll++;
  return result;
}

// rng_t::range =============================================================

double rng_t::range( double min,
                     double max )
{
  assert( min <= max );
  double result = ( min + max ) / 2.0;
  if( average_range ) return result;
  expected_range += result;
  if( min < max ) result =  min + real() * ( max - min );
  actual_range += result;
  return result;
}

// rng_t::gauss =============================================================

double rng_t::gauss( double mean,
                     double stddev )
{
  if( average_gauss ) return mean;
  
  // This code adapted from ftp://ftp.taygeta.com/pub/c/boxmuller.c
  // Implements the Polar form of the Box-Muller Transformation
  //
  //                  (c) Copyright 1994, Everett F. Carter Jr.
  //                      Permission is granted by the author to use
  //                      this software for any application provided this
  //                      copyright notice is preserved.

  double x1, x2, w, y1, y2, z;

  if( gauss_pair_use )
  {
    z = gauss_pair_value;
    gauss_pair_use = false;
  }
  else
  {
    do {
      x1 = 2.0 * real() - 1.0;
      x2 = 2.0 * real() - 1.0;
      w = x1 * x1 + x2 * x2;
    } while ( w >= 1.0 );
    
    w = sqrt( (-2.0 * log( w ) ) / w );
    y1 = x1 * w;
    y2 = x2 * w;

    z = y1;
    gauss_pair_value = y2;
    gauss_pair_use = true;
  }

  // True gaussian distribution can of course yield any number at some probability.  So truncate on the low end.
  double result = mean + z * stddev;
  if( result < 0 ) result = 0;

  expected_gauss += mean;
    actual_gauss += result;

  return result;
}

// rng_t::report ============================================================

void rng_t::report( FILE* file )
{
  fprintf( file, "RNG %s Actual/Expected Roll=%.6f Range=%.6f Gauss=%.6f\n", 
           name_str.c_str(), 
           ( ( expected_roll  == 0 ) ? 1.0 : actual_roll  / expected_roll  ),
           ( ( expected_range == 0 ) ? 1.0 : actual_range / expected_range ),
           ( ( expected_gauss == 0 ) ? 1.0 : actual_gauss / expected_gauss ) );
}

// ==========================================================================
// SFMT Random Number Generator
// ==========================================================================

/** 
 * SIMD oriented Fast Mersenne Twister(SFMT) pseudorandom number generator
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (Hiroshima University)
 *
 * Copyright (C) 2006, 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * The new BSD License is applied to this software.
 */

// Mersenne Exponent. The period of the sequence is a multiple of 2^MEXP-1.
#define MEXP 1279

// SFMT generator has an internal state array of 128-bit integers, and N_sfmt is its size.
#define N_sfmt (MEXP / 128 + 1)

// N32 is the size of internal state array when regarded as an array of 32-bit integers.
#define N32 (N_sfmt * 4)

// MEXP_1279 paramaters
#define POS1    7
#define SL1     14
#define SL2     3
#define SR1     5
#define SR2     1
#define MSK1    0xf7fefffdU
#define MSK2    0x7fefcfffU
#define MSK3    0xaff3ef3fU
#define MSK4    0xb5ffff7fU
#define PARITY1 0x00000001U
#define PARITY2 0x00000000U
#define PARITY3 0x00000000U
#define PARITY4 0x20000000U

// a parity check vector which certificate the period of 2^{MEXP} 
static uint32_t parity[4] = {PARITY1, PARITY2, PARITY3, PARITY4};

// 128-bit data structure 
struct w128_t 
{
  uint32_t u[4];
};

struct rng_sfmt_t : public rng_t
{
  w128_t sfmt[N_sfmt]; // the 128-bit internal state array 

  uint32_t *psfmt32; // the 32bit integer pointer to the 128-bit internal state array 

  int idx; // index counter to the 32-bit internal state array 
  
  rng_sfmt_t( const std::string& name, bool avg_range, bool avg_gauss );

  virtual ~rng_sfmt_t() {}
  virtual double real();
  virtual int type() { return RNG_MERSENNE_TWISTER; }
};

// period_certification =====================================================

inline static void period_certification( w128_t* sfmt, uint32_t* psfmt32 ) 
{
    int inner = 0;
    int i, j;
    uint32_t work;

    for (i = 0; i < 4; i++)
        inner ^= psfmt32[i] & parity[i];
    for (i = 16; i > 0; i >>= 1)
        inner ^= inner >> i;
    inner &= 1;
    /* check OK */
    if (inner == 1) {
        return;
    }
    /* check NG, and modification */
    for (i = 0; i < 4; i++) {
        work = 1;
        for (j = 0; j < 32; j++) {
            if ((work & parity[i]) != 0) {
                psfmt32[i] ^= work;
                return;
            }
            work = work << 1;
        }
    }
}

// init_gen_rand ============================================================

inline static void init_gen_rand( rng_sfmt_t* r, uint32_t seed )
{
    w128_t*   sfmt    = r -> sfmt;
    uint32_t* psfmt32 = r -> psfmt32;

    int i;

    psfmt32[0] = seed;
    for (i = 1; i < N32; i++) {
        psfmt32[i] = 1812433253UL * (psfmt32[i - 1] ^ (psfmt32[i - 1] >> 30)) + i;
    }
    period_certification( sfmt, psfmt32 );
}

// rshift128 ================================================================

inline static void rshift128(w128_t *out, w128_t const *in, int shift) 
{
    uint64_t th, tl, oh, ol;

    th = ((uint64_t)in->u[3] << 32) | ((uint64_t)in->u[2]);
    tl = ((uint64_t)in->u[1] << 32) | ((uint64_t)in->u[0]);

    oh = th >> (shift * 8);
    ol = tl >> (shift * 8);
    ol |= th << (64 - shift * 8);
    out->u[1] = (uint32_t)(ol >> 32);
    out->u[0] = (uint32_t)ol;
    out->u[3] = (uint32_t)(oh >> 32);
    out->u[2] = (uint32_t)oh;
}

// lshift128 ================================================================

inline static void lshift128(w128_t *out, w128_t const *in, int shift) 
{
    uint64_t th, tl, oh, ol;

    th = ((uint64_t)in->u[3] << 32) | ((uint64_t)in->u[2]);
    tl = ((uint64_t)in->u[1] << 32) | ((uint64_t)in->u[0]);

    oh = th << (shift * 8);
    ol = tl << (shift * 8);
    oh |= tl >> (64 - shift * 8);
    out->u[1] = (uint32_t)(ol >> 32);
    out->u[0] = (uint32_t)ol;
    out->u[3] = (uint32_t)(oh >> 32);
    out->u[2] = (uint32_t)oh;
}

// do_recursion =============================================================

inline static void do_recursion(w128_t *r, w128_t *a, w128_t *b, w128_t *c, w128_t *d) 
{
    w128_t x;
    w128_t y;

    lshift128(&x, a, SL2);
    rshift128(&y, c, SR2);
    r->u[0] = a->u[0] ^ x.u[0] ^ ((b->u[0] >> SR1) & MSK1) ^ y.u[0] ^ (d->u[0] << SL1);
    r->u[1] = a->u[1] ^ x.u[1] ^ ((b->u[1] >> SR1) & MSK2) ^ y.u[1] ^ (d->u[1] << SL1);
    r->u[2] = a->u[2] ^ x.u[2] ^ ((b->u[2] >> SR1) & MSK3) ^ y.u[2] ^ (d->u[2] << SL1);
    r->u[3] = a->u[3] ^ x.u[3] ^ ((b->u[3] >> SR1) & MSK4) ^ y.u[3] ^ (d->u[3] << SL1);
}

// gen_rand_all =============================================================

inline static void gen_rand_all( w128_t*   sfmt,
                                 uint32_t* psfmt32 ) 
{
    int i;
    w128_t *r1, *r2;

    r1 = &sfmt[N_sfmt - 2];
    r2 = &sfmt[N_sfmt - 1];
    for (i = 0; i < N_sfmt - POS1; i++) {
        do_recursion(&sfmt[i], &sfmt[i], &sfmt[i + POS1], r1, r2);
        r1 = r2;
        r2 = &sfmt[i];
    }
    for (; i < N_sfmt; i++) {
        do_recursion(&sfmt[i], &sfmt[i], &sfmt[i + POS1 - N_sfmt], r1, r2);
        r1 = r2;
        r2 = &sfmt[i];
    }
}

// gen_rand32 ===============================================================

inline static uint32_t gen_rand32(rng_sfmt_t* r)
{
    if (r->idx >= N32) {
        gen_rand_all(r->sfmt, r->psfmt32);
        r->idx = 0;
    }
    return r->psfmt32[r->idx++];
}

// gen_rand_real ============================================================

inline static double gen_rand_real(rng_sfmt_t* r)
{
  return gen_rand32(r) * ( 1.0 / 4294967295.0 );  // divide by 2^32-1 
}

// rng_sfmt_t::rng_sfmt_t ===================================================

rng_sfmt_t::rng_sfmt_t( const std::string& name, bool avg_range, bool avg_gauss ) : 
  rng_t( name, avg_range, avg_gauss )
{ 
  psfmt32 = &sfmt[0].u[0]; 
  idx = N32; 

  init_gen_rand( this, rand() );
}

// rng_sfmt_t::real ==========================================================

double rng_sfmt_t::real()
{
  return gen_rand_real( this );
}

// ==========================================================================
// Base-Class for Normalized RNGs
// ==========================================================================

struct rng_normalized_t : public rng_t
{
  rng_t* base;

  rng_normalized_t( const std::string& name, rng_t* b, bool avg_range, bool avg_gauss ) :
    rng_t( name, avg_range, avg_gauss ), base(b) {}
  virtual ~rng_normalized_t() {}

  virtual double real() { return base -> real(); }
  virtual double range( double min, double max ) { return ( min + max ) / 2.0; }
  virtual double gauss( double mean, double stddev ) { return mean; }
  virtual int    roll( double chance ) { assert(0); return 0; } // must be overridden
};

// ==========================================================================
// Normalized RNG via Variable Phase Shift
// ==========================================================================

struct rng_phase_shift_t : public rng_normalized_t
{
  static const int size = 10; // must be even number
  double range_distribution[ size ];
  double gauss_distribution[ size ];
  int range_index, gauss_index;

  rng_phase_shift_t( const std::string& name, rng_t* b, bool avg_range, bool avg_gauss ) :
    rng_normalized_t( name, b, avg_range, avg_gauss ) 
  {
    for( int i=0; i < size/2; i++ )
    {
      range_distribution[ i*2   ] = (   i) * 1.0/size + 1.0/(size*2);
      range_distribution[ i*2+1 ] = (10-i) * 1.0/size - 1.0/(size*2);
      gauss_distribution[ i*2   ] = -2.0 / (1<<i);
      gauss_distribution[ i*2+1 ] = +2.0 / (1<<i);
    }
    range_index = (int) real() * size;
    gauss_index = (int) real() * size;
    actual_roll = real();
  }
  virtual ~rng_phase_shift_t() {}
  virtual int type() { return RNG_PHASE_SHIFT; }

  virtual double range( double min, double max )
  {
    if( average_range ) return ( min + max ) / 2.0;
    if( ++range_index >= size ) range_index = 0;
    double result = min + range_distribution[ range_index ] * ( max - min );
    expected_range += ( max - min ) / 2.0;
    actual_range += result;
    return result;
  }
  virtual double gauss( double mean, double stddev )
  {
    if( average_gauss ) return mean; 
    if( ++gauss_index >= size ) gauss_index = 0;
    double result = mean + gauss_distribution[ gauss_index ] * stddev;
    expected_gauss += mean;
    actual_gauss += result;
    return result;
  }
  virtual int roll( double chance )
  {
    if( chance <= 0 ) return 0;
    if( chance >= 1 ) return 1;
    expected_roll += chance;
    if( actual_roll < expected_roll )
    {
      actual_roll++;
      return 1;
    }
    return 0;
  }
};

// ==========================================================================
// Normalized RNG via Distribution Pre-Fill
// ==========================================================================

struct rng_pre_fill_t : public rng_normalized_t
{
  static const int size = 10; // must be even number
  double  roll_distribution[ size ];
  double range_distribution[ size ];
  double gauss_distribution[ size ];
  int roll_index, range_index, gauss_index;

  rng_pre_fill_t( const std::string& name, rng_t* b, bool avg_range, bool avg_gauss ) :
    rng_normalized_t( name, b, avg_range, avg_gauss )
  {
    for( int i=0; i < size/2; i++ )
    {
      range_distribution[ i*2   ] = (   i) * 1.0/size + 1.0/(size*2);
      range_distribution[ i*2+1 ] = (10-i) * 1.0/size - 1.0/(size*2);
      gauss_distribution[ i*2   ] = -2.0 / (1<<i);
      gauss_distribution[ i*2+1 ] = +2.0 / (1<<i);
    }
    range_index = (int) real() * size;
    gauss_index = (int) real() * size;
     roll_index = size;
  }
  virtual ~rng_pre_fill_t() {}
  virtual int type() { return RNG_PRE_FILL; }

  virtual int roll( double chance )
  {
    if( chance <= 0 ) return 0;
    if( chance >= 1 ) return 1;
    if( ++roll_index >= size )
    {
      double exact_procs = chance * size + expected_roll - actual_roll;
      int num_procs = (int) floor( exact_procs );
      num_procs += base -> roll( exact_procs - num_procs );
      if( num_procs > size ) num_procs = size;
      int up=1, down=0;
      if( num_procs > ( size >> 1 ) ) 
      {
        up = 0;
        down = 1;
        num_procs = size - num_procs;
      }
      for( int i=0; i < size; i++ ) roll_distribution[ i ] = down;
      while( num_procs > 0 )
      {
        int index = real() * size;
        if( roll_distribution[ index ] == up ) continue;
        roll_distribution[ index ] = up;
        num_procs--;
      }
      roll_index = 0;
    }
    expected_roll += chance;
    if( roll_distribution[ roll_index ] )
    {
      actual_roll++;
      return 1;
    }
    return 0;
  }

  virtual double range( double min, double max )
  {
    if( average_range ) return ( min + max ) / 2.0;
    if( ++range_index >= size ) 
    {
      for( int i=0; i < size; i++ )
      {
        int other = (int) real() * size * 0.999;
        std::swap( range_distribution[ i ], range_distribution[ other ] );
      }
      range_index = 0;
    }
    double result = min + range_distribution[ range_index ] * ( max - min );
    expected_range += ( max - min ) / 2.0;
    actual_range += result;
    return result;
  }

  virtual double gauss( double mean, double stddev )
  {
    if( average_gauss ) return mean; 
    if( ++gauss_index >= size ) 
    {
      for( int i=0; i < size; i++ )
      {
        int other = (int) real() * size * 0.999;
        std::swap( gauss_distribution[ i ], gauss_distribution[ other ] );
      }
      gauss_index = 0;
    }
    double result = mean + gauss_distribution[ gauss_index ] * stddev;
    expected_gauss += mean;
    actual_gauss += result;
    return result;
  }
};

// ==========================================================================
// Normalized RNG via Distance Tracking
// ==========================================================================

struct rng_distance_simple_t : public rng_normalized_t
{
  // roll() distance-based distribution
  static const int maxN4 = 400;
  int nTries;
  int lastOk;
  int nextGive;
  int nit[maxN4 + 2];
  int N4, N1;
  double lastAvg;

  // simple repeating sequence for range() and gauss()
  static const int size = 10; // must be even number
  double range_distribution[ size ];
  double gauss_distribution[ size ];
  int range_index, gauss_index;

  rng_distance_simple_t( const std::string& n, rng_t* b, bool avg_range, bool avg_gauss ) :
    rng_normalized_t( n, b, avg_range, avg_gauss ), nTries(0), lastOk(0), range_index(0), gauss_index(0)
  {
    for (int i = 0; i < maxN4 + 2; i++) nit[i] = 0;
    actual_roll = real();
    for( int i=0; i < size/2; i++ )
    {
      range_distribution[ i*2   ] = (   i) * 1.0/size + 1.0/(size*2);
      range_distribution[ i*2+1 ] = (10-i) * 1.0/size - 1.0/(size*2);
      gauss_distribution[ i*2   ] = -2.0 / (1<<i);
      gauss_distribution[ i*2+1 ] = +2.0 / (1<<i);
    }
  }
  virtual ~rng_distance_simple_t() {}

  virtual int type() { return RNG_DISTANCE_SIMPLE; }

  void setN4(double expP)
  {
    lastAvg = expP;
    N1 = (int) (1 / expP);
    if (N1 > maxN4) N1 = maxN4;
    N4 = 4 * N1;
    if (N4 > maxN4) N4 = maxN4;
    nextGive = N1;
  }

  void resetN4(double expP)
  {
    lastAvg = expP;
    N1 = (int)(1 / expP);
    if (N1 > maxN4) N1 = maxN4;
    int newN4 = 4 * N1;
    if (newN4 > maxN4) newN4 = maxN4;
    if (newN4 > N4)
    {
      int sumOver = 0;
      for (int i = N4 + 2; i <= newN4 + 1; i++) sumOver += nit[i];
      nit[N4 + 1] -= sumOver;
    }
    else
    {
      int sumOver = 0;
      for (int i = newN4 + 2; i <= N4 + 1; i++) sumOver += nit[i];
      nit[newN4 + 1] += sumOver;
    }
    N4=newN4;
  }

  void findNextGive()
  {
    double avgP = expected_roll / nTries;

    // check if average chance is significantly different, to increase number of tracked distances
    if ((lastAvg-avgP) / lastAvg > 0.10) 
      resetN4(avgP);

    // find position that is lagging most
    double exTot = avgP * nTries;
    double exP = avgP * exTot;
    double avg1P = 1 - avgP;
    double sumExp = 0;
    double worst = -1;
    int worstID = 0;
    for (int i = 1; i <= N4+1; i++)
    {
      if (i > N4) 
        exP = exTot - sumExp;
      double difP = exP - nit[i];
      if (difP > worst)
      {
        worstID = i;
        worst = difP;
      }
      sumExp += exP;
      exP *= avg1P;
    }
    if (worstID <= 0)
    {
      worstID = N1;
    }
    // fill that position
    nextGive = worstID;
  }

  virtual int roll( double chance )
  {
    if( chance <= 0 ) return 0;
    if( chance >= 1 ) return 1;

    expected_roll += chance;

    if( ++nTries <= 10 )
    {
      if( nTries == 10 ) // switch to distance-based formula
      {
        double avgP = expected_roll / nTries;
        setN4( avgP );
      }

      if( actual_roll < expected_roll ) 
      {
        actual_roll++;
        lastOk = nTries;
        return 1;
      }
    }
    else
    {
      nextGive--;
      if( nextGive <= 0 )
      {
        actual_roll++;
        int dist = nTries - lastOk;
        if( dist > N4 ) dist = N4 + 1;
        nit[dist]++;
        lastOk = nTries;
        findNextGive();
        return 1;
      }
      return 0;
    }

    return 0;
  }

  virtual double range( double min, double max )
  {
    if( average_range ) return ( min + max ) / 2.0;
    if( ++range_index >= size ) range_index = 0;
    double result = min + range_distribution[ range_index ] * ( max - min );
    expected_range += ( max - min ) / 2.0;
    actual_range += result;
    return result;
  }

  virtual double gauss( double mean, double stddev )
  {
    if( average_gauss ) return mean; 
    if( ++gauss_index >= size ) gauss_index = 0;
    double result = mean + gauss_distribution[ gauss_index ] * stddev;
    expected_gauss += mean;
    actual_gauss += result;
    return result;
  }
};


// ==========================================================================
// Normalized Distribution
// ==========================================================================

// utility class that ensure desired distribution and average in "fastest" way
// Parameters for constructor:
//   - avg: desired average value
//   - N_elements:  number of possible elements/values that will be kept/distributed
//   - values_n: value of each element in array [0..n-1]
//   - chance_n: distribution probability that this element will be chosen
// Since it is for internal use, those using it must ensure:
//   - that supplied distribution probabilities (chance_n) add to 1.00
//   - that average of values_n * chances_n equals supplied "avg"
// Methods:
//   - getNextID(): return next element that should be used [main method]
//   - getNextValue(): same as above, only return value of element
//   - change_N(): change number of tracked elements. 
//     Note: caller must previously resize his chances_n, values_n
//   - change_avg(): change desired average value

struct norm_distribution_t
{
  int* counter;
  int N;
  int nTries;
  
  double* chances;
  double* values;
  int arrSz;
  double currSum;
  
  norm_distribution_t(int N_elements)
  {
    N=N_elements;
    if (N<=0) N=1;
    arrSz=N*3/2+2;
    counter= new int[arrSz];
    chances= new double[arrSz];
    values= new double[arrSz];
    memset(counter,0,arrSz*sizeof(int));
    memset(chances,0,arrSz*sizeof(double));
    memset(values,0,arrSz*sizeof(double));
    nTries=0;
    currSum=0;
  }
 ~norm_distribution_t()
  {
    delete chances;
    delete values;
    delete counter;
  }

  void change_N(int newN)
  {
    // do we need resizing of arrays?
    if (newN>=arrSz)
    {
      int newSz= newN*3/2+2;
      //create larger arrays
      int* n_counter= new int[newSz];
      double* n_chances= new double[newSz];
      double* n_values= new double[newSz];
      memset(n_counter,0,newSz*sizeof(int));
      memset(n_chances,0,newSz*sizeof(double));
      memset(n_values,0,newSz*sizeof(double));
      //copy old content
      for (int i=0; i<arrSz; i++)
      {
        n_counter[i]= counter[i];
        n_chances[i]=chances[i];
        n_values[i]=values[i];
      }
      // delete old ones and link new ones
      delete counter;
      delete chances;
      delete values;
      counter=n_counter;
      chances=n_chances;
      values=n_values;
      arrSz=newSz;
    }
    if (newN > N) // expand counters
    {
      int sumOver = 0;
      for (int i = N; i < newN; i++) sumOver += counter[i];
      counter[N-1] -= sumOver;
    }
    else if (newN < N) // compress counters
    {
      int sumOver = 0;
      for (int i = newN; i < N; i++) sumOver += counter[i];
      counter[newN-1] += sumOver;
    }
    N=newN;
  }
        
  int getNextID(double avgP)
  {
    nTries++;
    // find next best suitable: one that is lagging most behind expected number of occurences
    int selected_ID = -1;
    int selected_lag = 0;
    double selected_avg_diff=0;
    for (int i = 0; i < N; i++)
    {
      // difference to expected number of occurences for this element
      int lag = (int) floor(nTries* chances[i] - counter[i] + 0.5);
      // how would selection of this element result in average difference
      double avg_diff= abs((currSum+values[i])/nTries - avgP);
      // now see if this is best choice so far
      if( (selected_ID<0) || (lag>selected_lag) || ((lag==selected_lag)&&(avg_diff<selected_avg_diff)) ) 
      {
        selected_ID=i;
        selected_lag=lag;
        selected_avg_diff= avg_diff;
      }
    }
    // use that position
    counter[selected_ID]++;
    currSum+=values[selected_ID];
    return selected_ID;
  }

  double getNextValue(double avgP)
  {
    return values[getNextID(avgP)];
  }
};

// ==========================================================================
// Base variable RNG class
// (1) satisfy RNG_DISTRIBUTED: used when distribution of possible values needs to be simulated.
// (2) suitable for modeling overlapping procs
// (3) return simulation of gauss/range  (ie not only mean values but distributed values also)
// ==========================================================================

struct rng_distance_variable_t : public rng_normalized_t
{
  int nTries;
  int nextGive;
  double lastAvg;

  norm_distribution_t* nd_roll;
  norm_distribution_t* nd_gauss;
  norm_distribution_t* nd_range;

  rng_distance_variable_t( const std::string& n, rng_t* b, bool avg_range, bool avg_gauss ) :
    rng_normalized_t( n, b, avg_range, avg_gauss ), nTries(0), nextGive(0), lastAvg(0), nd_roll(0), nd_gauss(0), nd_range(0)
  {
    actual_roll=real();
  }

  virtual ~rng_distance_variable_t() 
  {
    if (nd_gauss) delete nd_gauss;
    if (nd_roll)  delete nd_roll;
    if (nd_range) delete nd_range;
  }

  virtual void setGaussN()
  {
    //set gauss values, fixed 7 elements, track +/- multiples of stddev [-3..+3]
    if (nd_gauss) return;
    int maxGN = 3;
    nd_gauss = new norm_distribution_t(2*maxGN+1);
    nd_gauss->values[maxGN]=0; // center value, 0
    nd_gauss->chances[maxGN+0]=0.382925;
    nd_gauss->chances[maxGN+1]=0.24173;
    nd_gauss->chances[maxGN+2]=0.060598;
    nd_gauss->chances[maxGN+3]=0.0062095;
    for (int i=1; i<=maxGN; i++)
    {
      nd_gauss->values[maxGN+i]=i;
      nd_gauss->values[maxGN-i]=-i;
      nd_gauss->chances[maxGN-i]=nd_gauss->chances[maxGN+i];
    }
  }

  virtual void setRangeN()
  {
    //set gauss values, fixed 7 elements, track +/- % of half-range  [-0.5..+0.5]
    if (nd_range) return;
    int maxGN = 3;
    nd_range = new norm_distribution_t(2*maxGN+1);
    for (int i=0; i<=maxGN; i++)
    {
      nd_range->values[maxGN+i]=+i*0.5/maxGN;
      nd_range->values[maxGN-i]=-i*0.5/maxGN;
      nd_range->chances[maxGN+i]=1.0/(2*maxGN+1);
      nd_range->chances[maxGN-i]=1.0/(2*maxGN+1);
    }
  }

  virtual void setRollN()
  {
    // set roll values, dynamically based on avgP
    double avgP = expected_roll / nTries;
    int maxN4=400;
    int N=100;
    if (avgP>0.001) N = (int) (1 / avgP);
    if (N > maxN4) N = maxN4;
    int N4 = 4 * N;
    if (N4 > maxN4) N4 = maxN4;
    if (!nd_roll)
      nd_roll=new norm_distribution_t(N4+1);
    else
      nd_roll->change_N(N4+1);
    double exP = avgP;
    double sumExp = 0;
    for (int i = 0; i < N4; i++)
    {
      nd_roll->chances[i]=exP;
      nd_roll->values[i]=i+1;
      sumExp += exP;
      exP *= (1-avgP);
    }
    nd_roll->chances[N4]=1-sumExp;
    nd_roll->values[N4]=N4+1;
    nextGive=N;
    lastAvg=avgP;
  }

  virtual int roll( double chance )
  {
    if( chance <= 0 ) return 0;
    if( chance >= 1 ) return 1;

    nTries++;
    expected_roll += chance;

    if( !nd_roll )
    {
      if( nTries == 10 ) setRollN(); // switch to distance-based formula
      if( actual_roll < expected_roll )
      {
        actual_roll++;
        return 1;
      }
    }
    else
    {
      nextGive--;
      if( nextGive <= 0 )
      {
        actual_roll++;
        // check if average chance is significantly different
        double avgP = expected_roll / nTries;
        if (abs(lastAvg-avgP) / lastAvg > 0.05) setRollN(); // increase number of tracked distances
        // take next best distance
        nextGive= floor(nd_roll->getNextValue(1/chance)+0.5); 
        return 1;
      }
      return 0;
    }
    return 0;
  }

  virtual double range( double min, double max ) 
  { 
    if( average_range ) return ( min + max ) / 2.0;
    setRangeN();  
    //get shift from middle
    double rngShift= nd_range->getNextValue(0)*(max-min);
    //return result
    return ( min + max ) / 2.0 + rngShift; 
  }

  virtual double gauss( double mean, double stddev ) 
  {
    if( average_gauss ) return mean;
    setGaussN();
    //get how many "stddev" i should shift this time
    double stdShift= nd_gauss->getNextValue(0)*stddev;
    // limit to 0..2*mean
    if (stdShift<-mean) stdShift=-mean;
    if (stdShift>+mean) stdShift=+mean;
    return mean+stdShift; 
  }
};



// ==========================================================================
// Normalized RNG via Proc-Distribution Tracking 
// (1) accepts variable proc chance
// (2) suitable for modeling overlapping procs
// ==========================================================================

struct rng_distance_reduced_t : public rng_distance_variable_t
{
  rng_distance_reduced_t( const std::string& n, rng_t* b, bool avg_range, bool avg_gauss ) :
    rng_distance_variable_t( n, b, avg_range, avg_gauss ) {}

  // override to reduce number of tracked distances into 10% bands
  virtual void setRollN()
  {
    // calculate distribution, dynamically based on avgP
    if (nTries<=0) return;
    double avgP = expected_roll / nTries;
    if (avgP>=0.40) return; // high chances shoud use simple rng_fixed
    double Nr=1000;
    if (avgP>0.0001) Nr =  1 / avgP;
    int N= floor(Nr+0.5);
    int values[100]={0};
    double chances[100]={0};
    int k=0;
    double sumChnc=0;
    double totSumChnc=0;
    double delta=0.11;
    double exP = avgP;
    int nIdx=0;
    int i=0;
    int lastI=0;
    // group them into 10% chance groups, note last distance in group
    // expected number of bands is 10
    while ((totSumChnc<1-2*delta)&&(((k<10)&&(lastI<3*N))||!nIdx))
    {
      i++;
      double oldSumChnc=sumChnc;
      sumChnc+=exP;
      totSumChnc+=exP;
      if (sumChnc>=delta)
      {
        k++;
        if ((abs(oldSumChnc-delta)<abs(sumChnc-delta)) && (i>lastI+1))
        {
          // 8,14 -> better 8 than 14
          chances[k]=oldSumChnc;
          lastI=i-1;
          sumChnc=exP;
        }
        else
        {
          // 7,11 -> better 11
          chances[k]=sumChnc;
          lastI=i;
          sumChnc=0;
        }
        values[k]=lastI;
        if ((lastI>=N)&&(nIdx==0))
        {
          nIdx=k;
          delta/=2; // thinner bands after N
        }
      }
      exP *= (1-avgP);
    }
    // distribute points to be at middle of their ranges (if asymetric, bit up). Also
    // calculate at which point M to put last value [k+1], so that it match 1/p while still p(N)~10%
    sumChnc=0;
    double sumVal=0;
    for (int i=1; i<=k; i++)
    {
      if (i!=nIdx)
      {
        values[i]=(values[i]+values[i-1]+1)/2; // puts it at middle of range
        sumChnc+=chances[i];
        sumVal+=chances[i]*values[i];
      }
      else values[i]=N;
    }
    // find adequate value for last band to ensure sum(chance*value)==1
    int M=0;
    double pN= 0.10;
    if (chances[nIdx]<=pN)
    { 
      // approximate M if pN<10%, to be ~10%
      M= floor( (Nr-pN*N-sumVal)/(1-sumChnc-pN) + 0.5 );
    }
    if (M<=values[k])
    {
      //approximate M if pN as given, or previous M too low
      pN=chances[nIdx];
      M= floor( (Nr-pN*N-sumVal)/(1-sumChnc-pN) + 0.5 );
    }
    // fill values at N and last band
    if (M>values[k])
    {
      pN= (sumVal-Nr+M-M*sumChnc)/(M-N); //exact pN if integer M, to match average N=1/p
      chances[nIdx]=pN;
      k++;
      values[k]=M;
      chances[k]=1-sumChnc-pN;
    }
    else chances[nIdx]=1-sumChnc;

    // sanity checks
    if (0)
    {
      bool ok=true;
      sumChnc=0;
      sumVal=0;
      for (int i=1; i<=k; i++)
      {
        if (chances[i]<0) ok=false;
        sumChnc+=chances[i];
        sumVal+=chances[i]*values[i];
        //printf("p[%d]=%1.2lf%%\n",values[i],chances[i]*100);
      }
      if (abs(1-sumChnc)>0.001) ok=false;
      if (abs(Nr-sumVal)/Nr>0.05) ok=false;
      if (!ok) printf("error");
    }

    // create distribution counter
    if (!nd_roll)
      nd_roll=new norm_distribution_t(k);
    else
      nd_roll->change_N(k);

    // copy distance values and chances
    for (int i = 0; i < k; i++)
    {
      nd_roll->chances[i]= chances[i+1];
      nd_roll->values[i]= values[i+1];
    }
    nextGive=N;
    lastAvg=avgP;
  }
};

// ==========================================================================
// Choosing the RNG package.........
// ==========================================================================

rng_t* rng_t::create( sim_t*             sim,
                      const std::string& name,
                      int                rng_type )
{
  switch( rng_type )
  {
  case RNG_STANDARD:          return new rng_t                  ( name,             sim -> average_range, sim -> average_gauss );
  case RNG_MERSENNE_TWISTER:  return new rng_sfmt_t             ( name,             sim -> average_range, sim -> average_gauss );
  case RNG_PHASE_SHIFT:       return new rng_phase_shift_t      ( name, sim -> rng, sim -> average_range, sim -> average_gauss );
  case RNG_PRE_FILL:          return new rng_pre_fill_t         ( name, sim -> rng, sim -> average_range, sim -> average_gauss );
  case RNG_DISTANCE_SIMPLE:   return new rng_distance_simple_t  ( name, sim -> rng, sim -> average_range, sim -> average_gauss );
  case RNG_DISTANCE_VARIABLE: return new rng_distance_variable_t( name, sim -> rng, sim -> average_range, sim -> average_gauss );
  case RNG_DISTANCE_REDUCED:  return new rng_distance_reduced_t ( name, sim -> rng, sim -> average_range, sim -> average_gauss );
  }
  assert(0);
  return 0;
}

#ifdef UNIT_TEST

// ==========================================================================
// test for speed of convergence and correct distribution of possible values
// ==========================================================================

int main( int argc, char** argv )
{
  rng_t global_rng( "global" );

  printf("Roll test\n\n");
  rng_distance_variable_t rng_r("test_roll", &global_rng);
  int lastRep=1;
  int nDsp=0;
  int lastOK=0, numOK=0;
  int distHist[100]={0};
  double p=0.1;
  int delta=10;
  for (int i=1; i<10000; i++){
          bool ok=rng_r.roll(p);
          if (ok){
                  numOK++;
                  int dist=i-lastOK;
                  lastOK=i;
                  distHist[dist]++;
                  if (nDsp<10){
                          printf("%d,",dist);
                          nDsp++;
                  }
                  if (i>lastRep+delta){
                          char sb[100];
                          double avg=numOK/(i+0.0);
                          printf(" >> i=%d, OK=%d, A=%1.2lf \n", i, numOK, avg*100 );
                          std::string sr="";
                          double exTot = p * i;
                          double exP = p * exTot;
                          double avg1P = 1 - p;
                          for (int u=1; u<20; u++){
                                  double diff= (exP-distHist[u])/exP*100;
                                  if (diff<100)
                                        sprintf(sb,"[%d]=%1.0lf%%,", u, diff  );
                                  else
                                    sprintf(sb,"[%d]!,", u  );
                                  sr+=sb;
                      exP *= avg1P;
                          }
                          sr+="\n\n";
                          printf(sr.c_str());
                          lastRep=i;
                          nDsp=0;
                          if (i>delta*10) delta*=10;
                  }
          }
  }
  // gauss test
  printf("Gauss test\n\n");
  rng_distance_variable_t rng_g("test_gauss", &global_rng);
  lastRep=1;
  nDsp=0;
  double avgSum=0;
  for (int i=1; i<1000000; i++){
          double n=rng_g.range(7,13);
          avgSum+=n;
          if (nDsp<10){
                  printf("%1.0lf,",n);
                  nDsp++;
          }
          if (i>=10*lastRep){
                  std::string sr="\n";
                  char sb[100];
                  sprintf(sb,"A=%1.2lf;", avgSum/i );
                  sr+=sb;
                  for (int u=0; u<7; u++){
                          double diff=rng_g.nd_gauss->counter[u]/(i+0.0);
                          diff=(diff-rng_g.nd_gauss->chances[u])/rng_g.nd_gauss->chances[u]*100;
                          sprintf(sb,"[%1.0lf]=%3.0lf%%,", rng_g.nd_gauss->values[u], diff  );
                          sr+=sb;
                  }
                  sr+="\n";
                  printf(sr.c_str());
                  lastRep=i;
                  nDsp=0;
          }
  }
}

#endif


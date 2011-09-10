// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

// ==========================================================================
// Rating
// ==========================================================================

namespace { // ANONYMOUS NAMESPACE ==========================================

struct _stat_list_t
{
  int id;
  double stats[ BASE_STAT_MAX ];
};

static const _stat_list_t race_stats[] =
{
  { RACE_HUMAN,     { 20, 20, 20, 20, 20, 0.0 } },
  { RACE_ORC,	      { 23, 17, 21, 17, 22, 0.0 } },
  { RACE_DWARF,     { 25, 16, 21, 19, 19, 0.0 } },
  { RACE_NIGHT_ELF, { 16, 24, 20, 20, 20, 0.0 } },
  { RACE_UNDEAD,    { 19, 18, 20, 18, 25, 0.0 } },
  { RACE_TAUREN,    { 25, 16, 21, 16, 22, 0.0 } },
  { RACE_GNOME,     { 15, 22, 20, 24, 20, 0.0 } },
  { RACE_TROLL,     { 21, 22, 20, 16, 21, 0.0 } },
  { RACE_BLOOD_ELF, { 17, 22, 20, 23, 18, 0.0 } },
  { RACE_DRAENEI,   { 21, 17, 20, 20, 22, 0.0 } },
  { RACE_WORGEN,    { 23, 22, 20, 16, 19, 0.0 } },
  { RACE_GOBLIN,    { 17, 22, 20, 23, 20, 0.0 } },
  { RACE_NONE, { 0 } }
};

//         Str    Agi  Sta   Int   Spi  Health    Mana  Crit/Agi Crit/Int Ddg/Agi   MeleCrit  SpellCrit
// Level 81-85 some stats extrapolated most are just the same as with 80

static const _stat_list_t death_knight_stats[] =
{
  {	60, {  100,   60,   90,   10,   25,   1689,    100,  0.0505,  0.0000,  0.0431,  3.16649,  0.0000 } },
  {	61, {  102,   61,   92,   10,   26,   1902,    100,  0.0479,  0.0000,  0.0408,  3.16649,  0.0000 } },
  {	62, {  105,   63,   94,   10,   26,   2129,    100,  0.0455,  0.0000,  0.0388,  3.16649,  0.0000 } },
  {	63, {  107,   64,   97,   11,   27,   2357,    100,  0.0433,  0.0000,  0.0370,  3.16649,  0.0000 } },
  {	64, {  110,   66,   99,   11,   27,   2612,    100,  0.0422,  0.0000,  0.0361,  3.16649,  0.0000 } },
  {	65, {  113,   67,  101,   11,   28,   2883,    100,  0.0403,  0.0000,  0.0345,  3.16649,  0.0000 } },
  {	66, {  115,   69,  104,   11,   29,   3169,    100,  0.0386,  0.0000,  0.0330,  3.16649,  0.0000 } },
  {	67, {  118,   70,  106,   12,   29,   3455,    100,  0.0371,  0.0000,  0.0317,  3.16649,  0.0000 } },
  {	68, {  120,   72,  108,   12,   30,   3774,    100,  0.0356,  0.0000,  0.0304,  3.16649,  0.0000 } },
  {	69, {  123,   73,  111,   12,   31,   4109,    100,  0.0344,  0.0000,  0.0295,  3.16649,  0.0000 } },
  {	70, {  126,   75,  113,   12,   31,   4444,    100,  0.0337,  0.0000,  0.0290,  3.16649,  0.0000 } },
  {	71, {  129,   77,  116,   13,   32,   4720,    100,  0.0314,  0.0000,  0.0269,  3.16649,  0.0000 } },
  {	72, {  131,   78,  118,   13,   33,   5013,    100,  0.0289,  0.0000,  0.0240,  3.16649,  0.0000 } },
  {	73, {  134,   80,  121,   13,   34,   5325,    100,  0.0268,  0.0000,  0.0220,  3.16649,  0.0000 } },
  {	74, {  137,   82,  123,   13,   34,   5656,    100,  0.0250,  0.0000,  0.0200,  3.16649,  0.0000 } },
  {	75, {  140,   83,  126,   14,   35,   6008,    100,  0.0234,  0.0000,  0.0192,  3.16649,  0.0000 } },
  {	76, {  143,   85,  129,   14,   36,   6381,    100,  0.0218,  0.0000,  0.0185,  3.16649,  0.0000 } },
  {	77, {  146,   87,  131,   14,   37,   6778,    100,  0.0201,  0.0000,  0.0159,  3.16649,  0.0000 } },
  {	78, {  149,   88,  134,   14,   37,   7199,    100,  0.0187,  0.0000,  0.0157,  3.16649,  0.0000 } },
  {	79, {  152,   90,  137,   15,   38,   7646,    100,  0.0174,  0.0000,  0.0139,  3.16649,  0.0000 } },
  {	80, {  155,   92,  140,   15,   39,   8121,    100,  0.0161,  0.0000,  0.0098,  3.16649,  0.0000 } },
  {	81, {  158,   94,  143,   15,   39,   8121,    100,  0.0161,  0.0000,  0.0098,  3.16649,  0.0000 } },
  {	82, {  161,   96,  146,   15,   40,   8121,    100,  0.0161,  0.0000,  0.0098,  3.16649,  0.0000 } },
  {	83, {  164,   98,  149,   15,   41,   8121,    100,  0.0161,  0.0000,  0.0098,  3.16649,  0.0000 } },
  {	84, {  167,  100,  152,   15,   41,   8121,    100,  0.0161,  0.0000,  0.0098,  3.16649,  0.0000 } },
  {	85, {  171,  101,  154,   16,   43,  43285,    100,  0.0161,  0.0000,  0.0098,  3.16649,  0.0000 } },
  { 0, { 0 } }
};

static const _stat_list_t druid_stats[] =
{
  {	60, {   45,   40,   50,   80,   90,   1483,   1244,  0.0307,  0.0163,  0.0615,  7.48769,  1.84532 } },
  {	61, {   46,   41,   51,   82,   92,   1657,   1357,  0.0298,  0.0160,  0.0585,  7.48769,  1.84532 } },
  {	62, {   47,   42,   52,   84,   94,   1840,   1469,  0.0294,  0.0155,  0.0574,  7.48769,  1.84532 } },
  {	63, {   48,   43,   54,   86,   97,   2020,   1582,  0.0284,  0.0148,  0.0551,  7.48769,  1.84532 } },
  {	64, {   49,   44,   55,   88,   99,   2222,   1694,  0.0278,  0.0145,  0.0541,  7.48769,  1.84532 } },
  {	65, {   51,   45,   56,   90,  101,   2433,   1807,  0.0273,  0.0140,  0.0531,  7.48769,  1.84532 } },
  {	66, {   52,   46,   58,   92,  103,   2640,   1919,  0.0268,  0.0136,  0.0520,  7.48769,  1.84532 } },
  {	67, {   53,   47,   59,   94,  106,   2872,   2032,  0.0264,  0.0132,  0.0510,  7.48769,  1.84532 } },
  {	68, {   54,   48,   60,   96,  108,   3114,   2145,  0.0257,  0.0130,  0.0496,  7.48769,  1.84532 } },
  {	69, {   55,   49,   62,   98,  110,   3351,   2257,  0.0253,  0.0127,  0.0489,  7.48769,  1.84532 } },
  {	70, {   56,   50,   63,  100,  113,   3614,   2370,  0.0249,  0.0124,  0.0477,  7.48769,  1.84532 } },
  {	71, {   58,   51,   64,  102,  115,   3883,   2482,  0.0231,  0.0115,  0.0453,  7.48769,  1.84532 } },
  {	72, {   59,   53,   66,  105,  118,   4172,   2595,  0.0215,  0.0107,  0.0459,  7.48769,  1.84532 } },
  {	73, {   60,   54,   67,  107,  120,   4483,   2708,  0.0200,  0.0100,  0.0392,  7.48769,  1.84532 } },
  {	74, {   61,   55,   69,  109,  123,   4817,   2820,  0.0185,  0.0092,  0.0370,  7.48769,  1.84532 } },
  {	75, {   63,   56,   70,  111,  125,   5176,   2933,  0.0172,  0.0086,  0.0334,  7.48769,  1.84532 } },
  {	76, {   64,   57,   72,  114,  128,   5562,   3045,  0.0160,  0.0081,  0.0320,  7.48769,  1.84532 } },
  {	77, {   65,   58,   73,  116,  131,   5977,   3158,  0.0149,  0.0074,  0.0295,  7.48769,  1.84532 } },
  {	78, {   67,   60,   75,  118,  133,   6423,   3270,  0.0138,  0.0069,  0.0273,  7.48769,  1.84532 } },
  {	79, {   68,   61,   76,  121,  136,   6902,   3383,  0.0128,  0.0064,  0.0254,  7.48769,  1.84532 } },
  {	80, {   69,   62,   78,  123,  139,   7417,   3496,  0.0120,  0.0060,  0.0232,  7.48769,  1.84532 } },
  {	81, {   70,   63,   78,  123,  139,   7417,   3496,  0.0120,  0.0060,  0.0232,  7.48769,  1.84532 } },
  {	82, {   71,   64,   78,  123,  139,   7417,   3496,  0.0120,  0.0060,  0.0232,  7.48769,  1.84532 } },
  {	83, {   72,   65,   78,  123,  139,   7417,   3496,  0.0120,  0.0060,  0.0232,  7.48769,  1.84532 } },
  {	84, {   73,   66,   78,  123,  139,   7417,   3496,  0.0120,  0.0060,  0.0232,  7.48769,  1.84532 } },
  {	85, {   74,   67,   78,  123,  139,   7417,   3496,  0.0120,  0.0060,  0.0232,  7.48769,  1.84532 } },
  { 0, { 0 } }
};

static const _stat_list_t hunter_stats[] =
{
  {	60, {   35,  105,   70,   45,   50,   1467,   1720,  0.0301,  0.0164,  0.0421, -1.53580,  3.60183 } },
  {	61, {   36,  107,   72,   46,   51,   1633,   1886,  0.0297,  0.0157,  0.0371, -1.53580,  3.60183 } },
  {	62, {   37,  110,   73,   47,   52,   1819,   2053,  0.0290,  0.0154,  0.0355, -1.53580,  3.60183 } },
  {	63, {   38,  113,   75,   48,   54,   2003,   2219,  0.0284,  0.0150,  0.0338, -1.53580,  3.60183 } },
  {	64, {   38,  115,   77,   49,   55,   2195,   2385,  0.0279,  0.0144,  0.0325, -1.53580,  3.60183 } },
  {	65, {   39,  118,   79,   51,   56,   2397,   2552,  0.0273,  0.0141,  0.0317, -1.53580,  3.60183 } },
  {	66, {   40,  120,   80,   52,   57,   2623,   2718,  0.0270,  0.0137,  0.0302, -1.53580,  3.60183 } },
  {	67, {   41,  123,   82,   53,   59,   2844,   2884,  0.0264,  0.0133,  0.0295, -1.53580,  3.60183 } },
  {	68, {   42,  126,   84,   54,   60,   3075,   3050,  0.0259,  0.0130,  0.0288, -1.53580,  3.60183 } },
  {	69, {   43,  129,   86,   55,   61,   3316,   3217,  0.0254,  0.0128,  0.0280, -1.53580,  3.60183 } },
  {	70, {   44,  131,   88,   57,   63,   3568,   3383,  0.0250,  0.0125,  0.0271, -1.53580,  3.60183 } },
  {	71, {   45,  134,   90,   58,   64,   3834,   3549,  0.0232,  0.0116,  0.0254, -1.53580,  3.60183 } },
  {	72, {   46,  137,   92,   59,   65,   4120,   3716,  0.0216,  0.0108,  0.0235, -1.53580,  3.60183 } },
  {	73, {   47,  140,   94,   60,   67,   4427,   3882,  0.0201,  0.0101,  0.0221, -1.53580,  3.60183 } },
  {	74, {   48,  143,   96,   62,   68,   4757,   4048,  0.0187,  0.0093,  0.0234, -1.53580,  3.60183 } },
  {	75, {   49,  146,   98,   63,   70,   5112,   4215,  0.0173,  0.0087,  0.0224, -1.53580,  3.60183 } },
  {	76, {   50,  149,  100,   64,   71,   5493,   4381,  0.0161,  0.0081,  0.0231, -1.53580,  3.60183 } },
  {	77, {   51,  152,  102,   66,   73,   5903,   4547,  0.0150,  0.0075,  0.0194, -1.53580,  3.60183 } },
  {	78, {   52,  155,  104,   67,   74,   6343,   4713,  0.0139,  0.0070,  0.0175, -1.53580,  3.60183 } },
  {	79, {   53,  158,  106,   68,   76,   6816,   4880,  0.0129,  0.0065,  0.0192, -1.53580,  3.60183 } },
  {	80, {   54,  161,  108,   70,   77,   7324,   5046,  0.0120,  0.0060,  0.0173, -1.53580,  3.60183 } },
  {	81, {   55,  161,  108,   70,   77,   7324,   5046,  0.0120,  0.0060,  0.0173, -1.53580,  3.60183 } },
  {	82, {   56,  161,  108,   70,   77,   7324,   5046,  0.0120,  0.0060,  0.0173, -1.53580,  3.60183 } },
  {	83, {   57,  161,  108,   70,   77,   7324,   5046,  0.0120,  0.0060,  0.0173, -1.53580,  3.60183 } },
  {	84, {   58,  161,  108,   70,   77,   7324,   5046,  0.0120,  0.0060,  0.0173, -1.53580,  3.60183 } },
  {	85, {   59,  161,  108,   70,   77,   7324,   5046,  0.0120,  0.0060,  0.0173, -1.53580,  3.60183 } },
  { 0, { 0 } }
};

static const _stat_list_t mage_stats[] =
{
  {	60, {   10,   15,   25,  105,  100,   1370,   1213,  0.0441,  0.0143,  0.0521,  3.45472,  0.90105 } },
  {	61, {   10,   15,   26,  107,  102,   1526,   1316,  0.0441,  0.0143,  0.0454,  3.45472,  0.90105 } },
  {	62, {   10,   16,   26,  110,  105,   1702,   1419,  0.0442,  0.0143,  0.0442,  3.45472,  0.90105 } },
  {	63, {   11,   16,   27,  113,  107,   1875,   1521,  0.0429,  0.0143,  0.0428,  3.45472,  0.90105 } },
  {	64, {   11,   16,   27,  115,  110,   2070,   1624,  0.0429,  0.0143,  0.0429,  3.45472,  0.90105 } },
  {	65, {   11,   17,   28,  118,  112,   2261,   1727,  0.0429,  0.0142,  0.0428,  3.45472,  0.90105 } },
  {	66, {   12,   17,   29,  120,  115,   2461,   1830,  0.0418,  0.0138,  0.0417,  3.45472,  0.90105 } },
  {	67, {   12,   18,   29,  123,  117,   2686,   1932,  0.0418,  0.0134,  0.0417,  3.45472,  0.90105 } },
  {	68, {   12,   18,   30,  126,  120,   2906,   2035,  0.0418,  0.0131,  0.0418,  3.45472,  0.90105 } },
  {	69, {   12,   18,   31,  129,  123,   3136,   2138,  0.0407,  0.0128,  0.0406,  3.45472,  0.90105 } },
  {	70, {   13,   19,   31,  131,  125,   3393,   2241,  0.0407,  0.0125,  0.0406,  3.45472,  0.90105 } },
  {	71, {   13,   19,   32,  134,  128,   3646,   2343,  0.0377,  0.0116,  0.0376,  3.45472,  0.90105 } },
  {	72, {   13,   20,   33,  137,  131,   3918,   2446,  0.0351,  0.0108,  0.0352,  3.45472,  0.90105 } },
  {	73, {   13,   20,   34,  140,  134,   4210,   2549,  0.0329,  0.0101,  0.0329,  3.45472,  0.90105 } },
  {	74, {   14,   21,   34,  143,  136,   4524,   2652,  0.0304,  0.0093,  0.0303,  3.45472,  0.90105 } },
  {	75, {   14,   21,   35,  146,  139,   4861,   2754,  0.0281,  0.0087,  0.0280,  3.45472,  0.90105 } },
  {	76, {   14,   21,   36,  149,  142,   5223,   2857,  0.0262,  0.0081,  0.0262,  3.45472,  0.90105 } },
  {	77, {   15,   22,   37,  152,  145,   5612,   2960,  0.0242,  0.0075,  0.0241,  3.45472,  0.90105 } },
  {	78, {   15,   22,   37,  155,  148,   6030,   3063,  0.0227,  0.0070,  0.0227,  3.45472,  0.90105 } },
  {	79, {   15,   23,   38,  158,  151,   6480,   3165,  0.0209,  0.0065,  0.0208,  3.45472,  0.90105 } },
  {	80, {   16,   23,   39,  161,  154,   6963,   3268,  0.0196,  0.0060,  0.0197,  3.45472,  0.90105 } },
  {	81, {   16,   23,   39,  161,  154,   6963,   3268,  0.0196,  0.0060,  0.0197,  3.45472,  0.90105 } },
  {	82, {   16,   23,   39,  161,  154,   6963,   3268,  0.0196,  0.0060,  0.0197,  3.45472,  0.90105 } },
  {	83, {   16,   23,   39,  161,  154,   6963,   3268,  0.0196,  0.0060,  0.0197,  3.45472,  0.90105 } },
  {	84, {   16,   23,   39,  161,  154,   6963,   3268,  0.0196,  0.0060,  0.0197,  3.45472,  0.90105 } },
  {	85, {   16,   23,   39,  161,  154,   6963,   3268,  0.0196,  0.0060,  0.0197,  3.45472,  0.90105 } },
  { 0, { 0 } }
};

static const _stat_list_t paladin_stats[] =
{
  {	60, {   85,   45,   80,   50,   55,   1381,   1512,  0.0508,  0.0185,  0.0542,  3.27401,  3.33764 } },
  {	61, {   87,   46,   82,   51,   56,   1540,   1656,  0.0495,  0.0159,  0.0501,  3.27401,  3.33764 } },
  {	62, {   89,   47,   84,   52,   58,   1708,   1800,  0.0480,  0.0154,  0.0485,  3.27401,  3.33764 } },
  {	63, {   91,   48,   86,   54,   59,   1884,   1944,  0.0467,  0.0149,  0.0471,  3.27401,  3.33764 } },
  {	64, {   93,   49,   88,   55,   60,   2068,   2088,  0.0455,  0.0145,  0.0460,  3.27401,  3.33764 } },
  {	65, {   95,   51,   90,   56,   62,   2262,   2232,  0.0444,  0.0140,  0.0446,  3.27401,  3.33764 } },
  {	66, {   98,   52,   92,   58,   63,   2466,   2377,  0.0444,  0.0136,  0.0445,  3.27401,  3.33764 } },
  {	67, {  100,   53,   94,   59,   65,   2679,   2521,  0.0421,  0.0134,  0.0422,  3.27401,  3.33764 } },
  {	68, {  102,   54,   96,   60,   66,   2901,   2665,  0.0421,  0.0131,  0.0424,  3.27401,  3.33764 } },
  {	69, {  104,   55,   98,   62,   67,   3134,   2809,  0.0412,  0.0128,  0.0412,  3.27401,  3.33764 } },
  {	70, {  106,   57,  100,   63,   69,   3377,   2953,  0.0402,  0.0125,  0.0382,  3.27401,  3.33764 } },
  {	71, {  109,   58,  102,   64,   70,   3629,   3097,  0.0367,  0.0116,  0.0349,  3.27401,  3.33764 } },
  {	72, {  111,   59,  105,   66,   72,   3900,   3241,  0.0345,  0.0108,  0.0347,  3.27401,  3.33764 } },
  {	73, {  113,   60,  107,   67,   74,   4191,   3385,  0.0320,  0.0101,  0.0327,  3.27401,  3.33764 } },
  {	74, {  116,   62,  109,   69,   75,   4503,   3529,  0.0298,  0.0093,  0.0295,  3.27401,  3.33764 } },
  {	75, {  118,   63,  111,   70,   77,   4839,   3673,  0.0274,  0.0087,  0.0273,  3.27401,  3.33764 } },
  {	76, {  121,   64,  114,   72,   78,   5200,   3817,  0.0258,  0.0081,  0.0257,  3.27401,  3.33764 } },
  {	77, {  123,   66,  116,   73,   80,   5588,   3962,  0.0240,  0.0075,  0.0242,  3.27401,  3.33764 } },
  {	78, {  126,   67,  118,   75,   82,   6005,   4106,  0.0221,  0.0070,  0.0224,  3.27401,  3.33764 } },
  {	79, {  128,   68,  121,   76,   83,   6453,   4250,  0.0206,  0.0065,  0.0202,  3.27401,  3.33764 } },
  {	80, {  131,   70,  123,   78,   85,   6934,   4394,  0.0192,  0.0060,  0.0191,  3.27401,  3.33764 } },
  {	81, {  131,   70,  123,   78,   85,   6934,   4394,  0.0192,  0.0060,  0.0191,  3.27401,  3.33764 } },
  {	82, {  131,   70,  123,   78,   85,   6934,   4394,  0.0192,  0.0060,  0.0191,  3.27401,  3.33764 } },
  {	83, {  131,   70,  123,   78,   85,   6934,   4394,  0.0192,  0.0060,  0.0191,  3.27401,  3.33764 } },
  {	84, {  131,   70,  123,   78,   85,   6934,   4394,  0.0192,  0.0060,  0.0191,  3.27401,  3.33764 } },
  {	85, {  131,   70,  123,   78,   85,   6934,   4394,  0.0192,  0.0060,  0.0191,  3.27401,  3.33764 } },
  { 0, { 0 } }
};

static const _stat_list_t priest_stats[] =
{
  {	60, {   15,   20,   30,  100,  105,   1397,   1376,  0.0457,  0.0151,  0.0492,  3.17418,  1.24036 } },
  {	61, {   15,   20,   31,  102,  107,   1557,   1500,  0.0446,  0.0148,  0.0450,  3.17418,  1.24036 } },
  {	62, {   16,   21,   31,  105,  110,   1738,   1625,  0.0447,  0.0145,  0.0444,  3.17418,  1.24036 } },
  {	63, {   16,   21,   32,  107,  113,   1916,   1749,  0.0443,  0.0143,  0.0441,  3.17418,  1.24036 } },
  {	64, {   16,   22,   33,  110,  115,   2101,   1873,  0.0435,  0.0139,  0.0433,  3.17418,  1.24036 } },
  {	65, {   17,   23,   34,  112,  118,   2295,   1998,  0.0428,  0.0137,  0.0428,  3.17418,  1.24036 } },
  {	66, {   17,   23,   35,  115,  120,   2495,   2122,  0.0422,  0.0134,  0.0419,  3.17418,  1.24036 } },
  {	67, {   18,   24,   35,  117,  123,   2719,   2247,  0.0416,  0.0132,  0.0414,  3.17418,  1.24036 } },
  {	68, {   18,   24,   36,  120,  126,   2936,   2371,  0.0414,  0.0130,  0.0412,  3.17418,  1.24036 } },
  {	69, {   18,   25,   37,  123,  129,   3160,   2495,  0.0412,  0.0127,  0.0410,  3.17418,  1.24036 } },
  {	70, {   19,   25,   38,  125,  131,   3391,   2620,  0.0401,  0.0125,  0.0400,  3.17418,  1.24036 } },
  {	71, {   19,   26,   39,  128,  134,   3644,   2744,  0.0372,  0.0116,  0.0372,  3.17418,  1.24036 } },
  {	72, {   20,   26,   39,  131,  137,   3916,   2868,  0.0345,  0.0108,  0.0346,  3.17418,  1.24036 } },
  {	73, {   20,   27,   40,  134,  140,   4208,   2993,  0.0320,  0.0101,  0.0322,  3.17418,  1.24036 } },
  {	74, {   21,   27,   41,  136,  143,   4522,   3117,  0.0299,  0.0093,  0.0298,  3.17418,  1.24036 } },
  {	75, {   21,   28,   42,  139,  146,   4859,   3242,  0.0277,  0.0087,  0.0277,  3.17418,  1.24036 } },
  {	76, {   21,   29,   43,  142,  149,   5221,   3366,  0.0257,  0.0081,  0.0258,  3.17418,  1.24036 } },
  {	77, {   22,   29,   44,  145,  152,   5610,   3490,  0.0240,  0.0075,  0.0240,  3.17418,  1.24036 } },
  {	78, {   22,   30,   45,  148,  155,   6028,   3615,  0.0223,  0.0070,  0.0223,  3.17418,  1.24036 } },
  {	79, {   23,   30,   46,  151,  158,   6477,   3739,  0.0207,  0.0065,  0.0207,  3.17418,  1.24036 } },
  {	80, {   23,   31,   47,  154,  161,   6960,   3863,  0.0192,  0.0060,  0.0192,  3.17418,  1.24036 } },
  {	81, {   23,   31,   47,  154,  161,   6960,   5398,  0.0192,  0.0060,  0.0192,  3.17418,  1.24036 } }, // correct mana values
  {	82, {   23,   31,   47,  154,  161,   6960,   7544,  0.0192,  0.0060,  0.0192,  3.17418,  1.24036 } }, // correct mana values
  {	83, {   23,   31,   47,  154,  161,   6960,  10543,  0.0192,  0.0060,  0.0192,  3.17418,  1.24036 } }, // correct mana values
  {	84, {   23,   31,   47,  154,  161,   6960,  13000,  0.0192,  0.0060,  0.0192,  3.17418,  1.24036 } },
  {	85, {   23,   31,   47,  154,  161,   6960,  18000,  0.0192,  0.0060,  0.0192,  3.17418,  1.24036 } },
  { 0, { 0 } }
};

static const _stat_list_t rogue_stats[] =
{
  {	60, {   60,  110,   55,   15,   30,   1523,    100,  0.0355,  0.0000,  0.0689, -0.29560,  0.0000 } },
  {	61, {   61,  113,   56,   15,   31,   1702,    100,  0.0334,  0.0000,  0.0642, -0.29560,  0.0000 } },
  {	62, {   63,  115,   58,   16,   31,   1879,    100,  0.0322,  0.0000,  0.0618, -0.29560,  0.0000 } },
  {	63, {   64,  118,   59,   16,   32,   2077,    100,  0.0307,  0.0000,  0.0584, -0.29560,  0.0000 } },
  {	64, {   66,  121,   60,   16,   33,   2285,    100,  0.0296,  0.0000,  0.0567, -0.29560,  0.0000 } },
  {	65, {   67,  123,   62,   17,   34,   2489,    100,  0.0286,  0.0000,  0.0547, -0.29560,  0.0000 } },
  {	66, {   69,  126,   63,   17,   35,   2717,    100,  0.0276,  0.0000,  0.0529, -0.29560,  0.0000 } },
  {	67, {   70,  129,   65,   18,   35,   2941,    100,  0.0268,  0.0000,  0.0514, -0.29560,  0.0000 } },
  {	68, {   72,  132,   66,   18,   36,   3190,    100,  0.0262,  0.0000,  0.0501, -0.29560,  0.0000 } },
  {	69, {   74,  135,   67,   18,   37,   3450,    100,  0.0256,  0.0000,  0.0494, -0.29560,  0.0000 } },
  {	70, {   75,  138,   69,   19,   38,   3704,    100,  0.0250,  0.0000,  0.0475, -0.29560,  0.0000 } },
  {	71, {   77,  141,   70,   19,   39,   3980,    100,  0.0232,  0.0000,  0.0445, -0.29560,  0.0000 } },
  {	72, {   79,  144,   72,   20,   39,   4277,    100,  0.0216,  0.0000,  0.0413, -0.29560,  0.0000 } },
  {	73, {   80,  147,   74,   20,   40,   4596,    100,  0.0201,  0.0000,  0.0385, -0.29560,  0.0000 } },
  {	74, {   82,  150,   75,   21,   41,   4939,    100,  0.0187,  0.0000,  0.0357, -0.29560,  0.0000 } },
  {	75, {   84,  153,   77,   21,   42,   5307,    100,  0.0173,  0.0000,  0.0336, -0.29560,  0.0000 } },
  {	76, {   85,  156,   78,   21,   43,   5703,    100,  0.0161,  0.0000,  0.0309, -0.29560,  0.0000 } },
  {	77, {   87,  159,   80,   22,   44,   6128,    100,  0.0150,  0.0000,  0.0290, -0.29560,  0.0000 } },
  {	78, {   89,  163,   82,   22,   45,   6585,    100,  0.0139,  0.0000,  0.0271, -0.29560,  0.0000 } },
  {	79, {   91,  166,   83,   23,   46,   7076,    100,  0.0129,  0.0000,  0.0251, -0.29560,  0.0000 } },
  {	80, {   93,  169,   85,   23,   47,   7604,    100,  0.0120,  0.0000,  0.0225, -0.29560,  0.0000 } },
  {	81, {   93,  169,   85,   23,   47,   7604,    100,  0.0120,  0.0000,  0.0225, -0.29560,  0.0000 } },
  {	82, {   93,  169,   85,   23,   47,   7604,    100,  0.0120,  0.0000,  0.0225, -0.29560,  0.0000 } },
  {	83, {   93,  169,   85,   23,   47,   7604,    100,  0.0120,  0.0000,  0.0225, -0.29560,  0.0000 } },
  {	84, {   93,  169,   85,   23,   47,   7604,    100,  0.0120,  0.0000,  0.0225, -0.29560,  0.0000 } },
  {	85, {   93,  169,   85,   23,   47,   7604,    100,  0.0120,  0.0000,  0.0225, -0.29560,  0.0000 } },
  { 0, { 0 } }
};

static const _stat_list_t shaman_stats[] =
{
  {	60, {   65,   35,   75,   70,   80,   1376,   1520,  0.0320,  0.0175,  0.0536,  2.92384,  2.20283 } },
  {	61, {   67,   36,   77,   72,   82,   1557,   1664,  0.0310,  0.0164,  0.0494,  2.92384,  2.20283 } },
  {	62, {   68,   37,   79,   73,   84,   1738,   1808,  0.0304,  0.0159,  0.0482,  2.92384,  2.20283 } },
  {	63, {   70,   38,   80,   75,   86,   1916,   1951,  0.0294,  0.0152,  0.0472,  2.92384,  2.20283 } },
  {	64, {   71,   38,   82,   77,   88,   2101,   2095,  0.0285,  0.0147,  0.0449,  2.92384,  2.20283 } },
  {	65, {   73,   39,   84,   79,   90,   2295,   2239,  0.0281,  0.0142,  0.0442,  2.92384,  2.20283 } },
  {	66, {   75,   40,   86,   80,   92,   2495,   2383,  0.0273,  0.0138,  0.0430,  2.92384,  2.20283 } },
  {	67, {   76,   41,   88,   82,   94,   2719,   2527,  0.0267,  0.0134,  0.0421,  2.92384,  2.20283 } },
  {	68, {   78,   42,   90,   84,   96,   2936,   2670,  0.0261,  0.0131,  0.0409,  2.92384,  2.20283 } },
  {	69, {   80,   43,   92,   86,   98,   3160,   2814,  0.0255,  0.0128,  0.0403,  2.92384,  2.20283 } },
  {	70, {   82,   44,   94,   88,  100,   3391,   2958,  0.0250,  0.0125,  0.0396,  2.92384,  2.20283 } },
  {	71, {   83,   45,   96,   90,  102,   3644,   3102,  0.0232,  0.0116,  0.0368,  2.92384,  2.20283 } },
  {	72, {   85,   46,   98,   92,  105,   3916,   3246,  0.0216,  0.0108,  0.0342,  2.92384,  2.20283 } },
  {	73, {   87,   47,  100,   94,  107,   4208,   3389,  0.0201,  0.0101,  0.0317,  2.92384,  2.20283 } },
  {	74, {   89,   48,  103,   96,  109,   4522,   3533,  0.0187,  0.0093,  0.0295,  2.92384,  2.20283 } },
  {	75, {   91,   49,  105,   98,  111,   4859,   3677,  0.0173,  0.0087,  0.0275,  2.92384,  2.20283 } },
  {	76, {   93,   50,  107,  100,  114,   5221,   3821,  0.0161,  0.0081,  0.0255,  2.92384,  2.20283 } },
  {	77, {   94,   51,  109,  102,  116,   5610,   3965,  0.0150,  0.0075,  0.0238,  2.92384,  2.20283 } },
  {	78, {   96,   52,  111,  104,  118,   6028,   4108,  0.0139,  0.0070,  0.0221,  2.92384,  2.20283 } },
  {	79, {   98,   53,  114,  106,  121,   6477,   4252,  0.0129,  0.0065,  0.0206,  2.92384,  2.20283 } },
  {	80, {  100,   54,  116,  108,  123,   6960,   4396,  0.0120,  0.0060,  0.0190,  2.92384,  2.20283 } },
  {	81, {  100,   54,  116,  108,  123,   6960,   4396,  0.0120,  0.0060,  0.0190,  2.92384,  2.20283 } },
  {	82, {  100,   54,  116,  108,  123,   6960,   4396,  0.0120,  0.0060,  0.0190,  2.92384,  2.20283 } },
  {	83, {  100,   54,  116,  108,  123,   6960,   4396,  0.0120,  0.0060,  0.0190,  2.92384,  2.20283 } },
  {	84, {  100,   54,  116,  108,  123,   6960,   4396,  0.0120,  0.0060,  0.0190,  2.92384,  2.20283 } },
  {	85, {  100,   54,  116,  108,  123,   6960,   4396,  0.0120,  0.0060,  0.0190,  2.92384,  2.20283 } },
  { 0, { 0 } }
};

static const _stat_list_t warlock_stats[] =
{
  {	60, {   25,   30,   45,   90,   95,   1414,   1373,  0.0477,  0.0165,  0.0570,  2.61372,  1.70458 } },
  {	61, {   26,   31,   46,   92,   97,   1580,   1497,  0.0470,  0.0159,  0.0488,  2.61372,  1.70458 } },
  {	62, {   26,   31,   47,   94,  100,   1755,   1621,  0.0464,  0.0154,  0.0477,  2.61372,  1.70458 } },
  {	63, {   27,   32,   48,   97,  102,   1939,   1745,  0.0457,  0.0148,  0.0465,  2.61372,  1.70458 } },
  {	64, {   27,   33,   49,   99,  104,   2133,   1870,  0.0451,  0.0143,  0.0450,  2.61372,  1.70458 } },
  {	65, {   28,   34,   51,  101,  107,   2323,   1994,  0.0444,  0.0138,  0.0442,  2.61372,  1.70458 } },
  {	66, {   29,   35,   52,  103,  109,   2535,   2118,  0.0438,  0.0135,  0.0431,  2.61372,  1.70458 } },
  {	67, {   29,   35,   53,  106,  112,   2758,   2242,  0.0432,  0.0130,  0.0430,  2.61372,  1.70458 } },
  {	68, {   30,   36,   54,  108,  114,   2991,   2366,  0.0425,  0.0127,  0.0420,  2.61372,  1.70458 } },
  {	69, {   31,   37,   55,  110,  116,   3235,   2490,  0.0419,  0.0126,  0.0409,  2.61372,  1.70458 } },
  {	70, {   31,   38,   56,  113,  119,   3490,   2615,  0.0413,  0.0125,  0.0401,  2.61372,  1.70458 } },
  {	71, {   32,   39,   58,  115,  122,   3750,   2739,  0.0385,  0.0116,  0.0372,  2.61372,  1.70458 } },
  {	72, {   33,   39,   59,  118,  124,   4030,   2863,  0.0357,  0.0108,  0.0347,  2.61372,  1.70458 } },
  {	73, {   34,   40,   60,  120,  127,   4330,   2987,  0.0331,  0.0101,  0.0322,  2.61372,  1.70458 } },
  {	74, {   34,   41,   61,  123,  130,   4653,   3111,  0.0310,  0.0093,  0.0298,  2.61372,  1.70458 } },
  {	75, {   35,   42,   63,  125,  132,   5000,   3235,  0.0288,  0.0087,  0.0277,  2.61372,  1.70458 } },
  {	76, {   36,   43,   64,  128,  135,   5373,   3360,  0.0265,  0.0081,  0.0258,  2.61372,  1.70458 } },
  {	77, {   37,   44,   65,  131,  138,   5774,   3484,  0.0246,  0.0075,  0.0240,  2.61372,  1.70458 } },
  {	78, {   37,   45,   67,  133,  141,   6204,   3608,  0.0231,  0.0070,  0.0224,  2.61372,  1.70458 } },
  {	79, {   38,   46,   68,  136,  144,   6667,   3732,  0.0213,  0.0065,  0.0207,  2.61372,  1.70458 } },
  {	80, {   39,   47,   69,  139,  146,   7164,   3856,  0.0199,  0.0060,  0.0192,  2.61372,  1.70458 } },
  {	81, {   39,   47,   69,  139,  146,   7164,   3856,  0.0199,  0.0060,  0.0192,  2.61372,  1.70458 } },
  {	82, {   39,   47,   69,  139,  146,   7164,   3856,  0.0199,  0.0060,  0.0192,  2.61372,  1.70458 } },
  {	83, {   39,   47,   69,  139,  146,   7164,   3856,  0.0199,  0.0060,  0.0192,  2.61372,  1.70458 } },
  {	84, {   39,   47,   69,  139,  146,   7164,   3856,  0.0199,  0.0060,  0.0192,  2.61372,  1.70458 } },
  {	85, {   39,   47,   69,  139,  146,   7164,   3856,  0.0199,  0.0060,  0.0192,  2.61372,  1.70458 } },
  { 0, { 0 } }
};

static const _stat_list_t warrior_stats[] =
{
  {	60, {  100,   60,   90,   10,   25,   1689,    100,  0.0504,  0.0000,  0.0430,  3.17876,  0.0000 } },
  {	61, {  102,   61,   92,   10,   26,   1902,    100,  0.0478,  0.0000,  0.0404,  3.17876,  0.0000 } },
  {	62, {  105,   63,   94,   10,   26,   2129,    100,  0.0454,  0.0000,  0.0384,  3.17876,  0.0000 } },
  {	63, {  107,   64,   97,   11,   27,   2357,    100,  0.0432,  0.0000,  0.0366,  3.17876,  0.0000 } },
  {	64, {  110,   66,   99,   11,   27,   2612,    100,  0.0422,  0.0000,  0.0357,  3.17876,  0.0000 } },
  {	65, {  112,   68,  101,   11,   28,   2883,    100,  0.0403,  0.0000,  0.0343,  3.17876,  0.0000 } },
  {	66, {  115,   69,  103,   12,   29,   3169,    100,  0.0386,  0.0000,  0.0327,  3.17876,  0.0000 } },
  {	67, {  117,   71,  106,   12,   29,   3455,    100,  0.0371,  0.0000,  0.0314,  3.17876,  0.0000 } },
  {	68, {  120,   72,  108,   12,   30,   3774,    100,  0.0356,  0.0000,  0.0303,  3.17876,  0.0000 } },
  {	69, {  122,   74,  110,   12,   31,   4109,    100,  0.0343,  0.0000,  0.0292,  3.17876,  0.0000 } },
  {	70, {  125,   76,  113,   13,   31,   4444,    100,  0.0336,  0.0000,  0.0257,  3.17876,  0.0000 } },
  {	71, {  128,   77,  115,   13,   32,   4720,    100,  0.0313,  0.0000,  0.0257,  3.17876,  0.0000 } },
  {	72, {  130,   79,  118,   13,   33,   5013,    100,  0.0288,  0.0000,  0.0239,  3.17876,  0.0000 } },
  {	73, {  133,   81,  120,   13,   34,   5325,    100,  0.0267,  0.0000,  0.0218,  3.17876,  0.0000 } },
  {	74, {  136,   82,  123,   14,   34,   5656,    100,  0.0249,  0.0000,  0.0206,  3.17876,  0.0000 } },
  {	75, {  139,   84,  125,   14,   35,   6008,    100,  0.0233,  0.0000,  0.0193,  3.17876,  0.0000 } },
  {	76, {  142,   86,  128,   14,   36,   6381,    100,  0.0217,  0.0000,  0.0183,  3.17876,  0.0000 } },
  {	77, {  145,   88,  131,   15,   37,   6778,    100,  0.0200,  0.0000,  0.0172,  3.17876,  0.0000 } },
  {	78, {  148,   89,  133,   15,   37,   7199,    100,  0.0186,  0.0000,  0.0156,  3.17876,  0.0000 } },
  {	79, {  151,   91,  136,   15,   38,   7646,    100,  0.0173,  0.0000,  0.0147,  3.17876,  0.0000 } },
  {	80, {  154,   93,  139,   16,   39,   8121,    100,  0.0161,  0.0000,  0.0121,  3.17876,  0.0000 } },
  {	81, {  154,   93,  139,   16,   39,   8121,    100,  0.0161,  0.0000,  0.0121,  3.17876,  0.0000 } },
  {	82, {  154,   93,  139,   16,   39,   8121,    100,  0.0161,  0.0000,  0.0121,  3.17876,  0.0000 } },
  {	83, {  154,   93,  139,   16,   39,   8121,    100,  0.0161,  0.0000,  0.0121,  3.17876,  0.0000 } },
  {	84, {  154,   93,  139,   16,   39,   8121,    100,  0.0161,  0.0000,  0.0121,  3.17876,  0.0000 } },
  {	85, {  154,   93,  139,   16,   39,   8121,    100,  0.0161,  0.0000,  0.0121,  3.17876,  0.0000 } },
  { 0, { 0 } }
};





} // ANONYMOUS NAMESPACE =====================================================

// rating_t::init ===========================================================

void rating_t::init( sim_t* sim, dbc_t& dbc, int level, int type )
{
  if ( sim -> debug ) log_t::output( sim, "rating_t::init: level=%d type=%s",
                                     level, util_t::player_type_string( type ) );

  if ( type == ENEMY || type == ENEMY_ADD )
  {
    double max = +1.0E+50;
    spell_haste       = max;
    spell_hit         = max;
    spell_crit        = max;
    attack_haste      = max;
    attack_hit        = max;
    attack_crit       = max;
    ranged_haste      = max;
    ranged_hit        = max;
    ranged_crit       = max;
    expertise         = max;
    dodge             = max;
    parry             = max;
    block             = max;
    mastery           = max;
  }
  else
  {
    spell_haste       = dbc.combat_rating( RATING_SPELL_HASTE,  level );
    spell_hit         = dbc.combat_rating( RATING_SPELL_HIT,    level );
    spell_crit        = dbc.combat_rating( RATING_SPELL_CRIT,   level );
    attack_haste      = dbc.combat_rating( RATING_MELEE_HASTE,  level );
    attack_hit        = dbc.combat_rating( RATING_MELEE_HIT,    level );
    attack_crit       = dbc.combat_rating( RATING_MELEE_CRIT,   level );
    ranged_haste      = dbc.combat_rating( RATING_RANGED_HASTE, level );
    ranged_hit        = dbc.combat_rating( RATING_RANGED_HIT,   level );
    ranged_crit       = dbc.combat_rating( RATING_RANGED_CRIT,  level );
    expertise         = dbc.combat_rating( RATING_EXPERTISE,    level );
    dodge             = dbc.combat_rating( RATING_DODGE,        level );
    parry             = dbc.combat_rating( RATING_PARRY,        level );
    block             = dbc.combat_rating( RATING_BLOCK,        level );
    mastery           = dbc.combat_rating( RATING_MASTERY,      level ) / 100;
  }
}

// rating_t::interpolate ====================================================

double rating_t::interpolate( int    level,
                              double val_60,
                              double val_70,
                              double val_80,
                              double val_85 )
{
  if ( val_85 < 0 ) val_85 = val_80; // TODO
  if ( level <= 60 )
  {
    return val_60;
  }
  else if ( level == 70 )
  {
    return val_70;
  }
  else if ( level == 80 )
  {
    return val_80;
  }
  else if ( level >= 85 )
  {
    return val_85;
  }
  else if ( level < 70 )
  {
    // Assume linear progression for now.
    double adjust = ( level - 60 ) / 10.0;
    return val_60 + adjust * ( val_70 - val_60 );
  }
  else if ( level < 80 )
  {
    // Assume linear progression for now.
    double adjust = ( level - 70 ) / 10.0;
    return val_70 + adjust * ( val_80 - val_70 );
  }
  else // ( level < 85 )
  {
    // Assume linear progression for now.
    double adjust = ( level - 80 ) / 5.0;
    return val_80 + adjust * ( val_85 - val_80 );
  }
  assert( 0 );
  return 0;
}

// rating_t::get_attribute_base =============================================

double rating_t::get_attribute_base( sim_t* /* sim */, dbc_t& dbc, int level, player_type class_type, race_type race, base_stat_type stat_type )
{
  double res                       = 0.0;

  switch ( stat_type )
  {
  case BASE_STAT_STRENGTH:           res = dbc.race_base( race ).strength + dbc.attribute_base( class_type, level ).strength; break;
  case BASE_STAT_AGILITY:            res = dbc.race_base( race ).agility + dbc.attribute_base( class_type, level ).agility; break;
  case BASE_STAT_STAMINA:            res = dbc.race_base( race ).stamina + dbc.attribute_base( class_type, level ).stamina; break;
  case BASE_STAT_INTELLECT:          res = dbc.race_base( race ).intellect + dbc.attribute_base( class_type, level ).intellect; break;
  case BASE_STAT_SPIRIT:             res = dbc.race_base( race ).spirit + dbc.attribute_base( class_type, level ).spirit;
                                     if ( race == RACE_HUMAN ) res *= 1.03; break;
  case BASE_STAT_HEALTH:             res = dbc.attribute_base( class_type, level ).base_health; break;
  case BASE_STAT_MANA:               res = dbc.attribute_base( class_type, level ).base_resource; break;
  case BASE_STAT_MELEE_CRIT_PER_AGI: res = dbc.melee_crit_scaling( class_type, level ); break;
  case BASE_STAT_SPELL_CRIT_PER_INT: res = dbc.spell_crit_scaling( class_type, level ); break;
  case BASE_STAT_DODGE_PER_AGI:      res = dbc.dodge_scaling( class_type, level ); break;
  case BASE_STAT_MELEE_CRIT:         res = dbc.melee_crit_base( class_type ); break;
  case BASE_STAT_SPELL_CRIT:         res = dbc.spell_crit_base( class_type ); break;
  case BASE_STAT_MP5:                res = dbc.regen_base( class_type, level ); break;
  case BASE_STAT_SPI_REGEN:          res = dbc.regen_spirit( class_type, level ); break;
  default: break;
  }

  return res;
}

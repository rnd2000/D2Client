//  Missile.h -- the global tables of Missile.cpp: the missile client do- and hit-function dispatch tables and their counts, the target-test table,
//  and the offset rings and effect-stage tables the missile handlers spawn by. Address range 0x6FB70160..0x6FB91517.
//  Each is a C++17 `inline` variable, so every file that includes this header shares one object.
//  The tables from .rdata are `const`; the ones from .data (the target tests, the two dispatch tables and their counts, gaFireSprayMissiles
//  and gnLightQuality) are writable, as in the original.
//  Include it after D2Client.h, which declares every function named here.
#pragma once

//  D2Client 0x6FB91218
//  gaMissileTargetTests: nine records of { target test, collision mask }, indexed by the missile's CollideType (missiles.txt, +183h), which
//  a missile unit also takes as its anim mode. MISSILE_CltDoFunc1 reads the test and MISSILE_InitClientUnit the mask.
//  Entries 0 and 4 have no test, 6 has a mask and no test, and IsMonsterTarget appears twice (2 and 5) with the same mask.
//  The slot takes the callback type of D2Common #10407, int (__fastcall *)(D2UnitStrc *, void *). The tests are declared with their context's
//  type, and MISSILE_IsTargetableMissile with none, so each is cast to the slot's type: under __fastcall the unit is ecx and the context edx
//  either way, and a test with no context leaves edx unread.
//  BUG: neither reader bounds the index, so a CollideType past 8 reads whatever follows the table. LATENT: stock rows carry only 1, 3, 6 or 8.
//  DIFFERENCE: past index 8 the original reads its next table, gpMissileCltDoFuncs, where ours reads past this array.
inline D2MissileUnitFindTableStrc gaMissileTargetTests[9] =
{
    { NULL, COLLIDE_NONE },                                                         // 0 NONE
    { (decltype(D2MissileUnitFindTableStrc::pfUnitFindCallback))MISSILE_IsEnemyTarget,
      COLLIDE_PLAYER | COLLIDE_MISSILE_BARRIER },                                   // 1 PLAYER
    { (decltype(D2MissileUnitFindTableStrc::pfUnitFindCallback))MISSILE_IsMonsterTarget,
      COLLIDE_MONSTER | COLLIDE_MISSILE_BARRIER },                                  // 2 MONSTER
    { (decltype(D2MissileUnitFindTableStrc::pfUnitFindCallback))MISSILE_IsPlayerOrMonsterTarget,
      COLLIDE_MONSTER | COLLIDE_PLAYER | COLLIDE_MISSILE_BARRIER },                 // 3 PLAYER_AND_MONSTER
    { NULL, COLLIDE_NONE },                                                         // 4 NONE4
    { (decltype(D2MissileUnitFindTableStrc::pfUnitFindCallback))MISSILE_IsMonsterTarget,
      COLLIDE_MONSTER | COLLIDE_MISSILE_BARRIER },                                  // 5 MONSTER_5
    { NULL, COLLIDE_MISSILE_BARRIER },                                              // 6 MISSILE_BARRIER
    { (decltype(D2MissileUnitFindTableStrc::pfUnitFindCallback))MISSILE_IsTargetableMissile,
      COLLIDE_MISSILE },                                                            // 7 MISSILE
    { (decltype(D2MissileUnitFindTableStrc::pfUnitFindCallback))MISSILE_IsPlayerOrMonsterTarget,
      COLLIDE_MONSTER | COLLIDE_PLAYER | COLLIDE_MISSILE_BARRIER | COLLIDE_WALL },  // 8 PLAYER_MONSTER_BARRIER_WALL
};

static_assert(offsetof(D2MissileUnitFindTableStrc, pfUnitFindCallback) == 0
                  && offsetof(D2MissileUnitFindTableStrc, nCollisionMask) == 4
                  && sizeof(gaMissileTargetTests) == 0x6FB91260 - 0x6FB91218,
              "the test at +0 (0x6FB3DAA6), the mask at +4 (0x6FB397E8); nine records up to gpMissileCltDoFuncs");
static_assert((COLLIDE_PLAYER | COLLIDE_MISSILE_BARRIER) == 0x84 && (COLLIDE_MONSTER | COLLIDE_MISSILE_BARRIER) == 0x104
                  && (COLLIDE_MONSTER | COLLIDE_PLAYER | COLLIDE_MISSILE_BARRIER) == 0x184
                  && (COLLIDE_MONSTER | COLLIDE_PLAYER | COLLIDE_MISSILE_BARRIER | COLLIDE_WALL) == 0x185
                  && COLLIDE_MISSILE_BARRIER == 4 && COLLIDE_MISSILE == 0x40,
              "the image's masks: 84h, 104h, 184h, 0, 104h, 4, 40h, 185h");

//  D2Client 0x6FB91260
//  gpMissileCltDoFuncs: the missile client do-function table, dispatched by MISSILE_ClientUpdate through a tail jump with one register
//  argument (the missile, in ecx), hence __fastcall. 91 slots (gnMissileCltDoFuncCount): slot 0 and 69..90 are NULL, 1..68 are
//  MISSILE_CltDoFunc<index>. Slot 0 is never dispatched: the caller requires an index above 0.
inline void (__fastcall *gpMissileCltDoFuncs[91])(D2UnitStrc *pMissile) =
{
    NULL,                           //   0
    MISSILE_CltDoFunc1,             //   1  0x6FB3D880
    MISSILE_CltDoFunc2,             //   2  0x6FB30260
    MISSILE_CltDoFunc3,             //   3  0x6FB303E0
    MISSILE_CltDoFunc4,             //   4  0x6FB30690
    MISSILE_CltDoFunc5,             //   5  0x6FB306F0
    MISSILE_CltDoFunc6,             //   6  0x6FB30820
    MISSILE_CltDoFunc7,             //   7  0x6FB30AD0
    MISSILE_CltDoFunc8,             //   8  0x6FB30D20
    MISSILE_CltDoFunc9,             //   9  0x6FB30E80
    MISSILE_CltDoFunc10,            //  10  0x6FB31390
    MISSILE_CltDoFunc11,            //  11  0x6FB31540
    MISSILE_CltDoFunc12,            //  12  0x6FB31570
    MISSILE_CltDoFunc13,            //  13  0x6FB31990
    MISSILE_CltDoFunc14,            //  14  0x6FB31BA0
    MISSILE_CltDoFunc15,            //  15  0x6FB31DD0
    MISSILE_CltDoFunc16,            //  16  0x6FB32150
    MISSILE_CltDoFunc17,            //  17  0x6FB32310
    MISSILE_CltDoFunc18,            //  18  0x6FB32470
    MISSILE_CltDoFunc19,            //  19  0x6FB32610
    MISSILE_CltDoFunc20,            //  20  0x6FB32750
    MISSILE_CltDoFunc21,            //  21  0x6FB32870
    MISSILE_CltDoFunc22,            //  22  0x6FB32AC0
    MISSILE_CltDoFunc23,            //  23  0x6FB32CB0
    MISSILE_CltDoFunc24,            //  24  0x6FB32CE0
    MISSILE_CltDoFunc25,            //  25  0x6FB33180
    MISSILE_CltDoFunc26,            //  26  0x6FB33440
    MISSILE_CltDoFunc27,            //  27  0x6FB337E0
    MISSILE_CltDoFunc28,            //  28  0x6FB339B0
    MISSILE_CltDoFunc29,            //  29  0x6FB34300
    MISSILE_CltDoFunc30,            //  30  0x6FB34500
    MISSILE_CltDoFunc31,            //  31  0x6FB3A3E0
    MISSILE_CltDoFunc32,            //  32  0x6FB3A960
    MISSILE_CltDoFunc33,            //  33  0x6FB3AAA0
    MISSILE_CltDoFunc34,            //  34  0x6FB3AE40
    MISSILE_CltDoFunc35,            //  35  0x6FB3B280
    MISSILE_CltDoFunc36,            //  36  0x6FB3B4B0
    MISSILE_CltDoFunc37,            //  37  0x6FB39AC0
    MISSILE_CltDoFunc38,            //  38  0x6FB39ED0
    MISSILE_CltDoFunc39,            //  39  0x6FB39D00
    MISSILE_CltDoFunc40,            //  40  0x6FB39B60
    MISSILE_CltDoFunc41,            //  41  0x6FB39B10
    MISSILE_CltDoFunc42,            //  42  0x6FB3A130
    MISSILE_CltDoFunc43,            //  43  0x6FB34760
    MISSILE_CltDoFunc44,            //  44  0x6FB347B0
    MISSILE_CltDoFunc45,            //  45  0x6FB348F0
    MISSILE_CltDoFunc46,            //  46  0x6FB34960
    MISSILE_CltDoFunc47,            //  47  0x6FB34C30
    MISSILE_CltDoFunc48,            //  48  0x6FB34CC0
    MISSILE_CltDoFunc49,            //  49  0x6FB34FF0
    MISSILE_CltDoFunc50,            //  50  0x6FB35110
    MISSILE_CltDoFunc51,            //  51  0x6FB353C0
    MISSILE_CltDoFunc52,            //  52  0x6FB35670
    MISSILE_CltDoFunc53,            //  53  0x6FB35780
    MISSILE_CltDoFunc54,            //  54  0x6FB3CE10
    MISSILE_CltDoFunc55,            //  55  0x6FB3D200
    MISSILE_CltDoFunc56,            //  56  0x6FB35840
    MISSILE_CltDoFunc57,            //  57  0x6FB359C0
    MISSILE_CltDoFunc58,            //  58  0x6FB35B40
    MISSILE_CltDoFunc59,            //  59  0x6FB3CA50
    MISSILE_CltDoFunc60,            //  60  0x6FB3D380
    MISSILE_CltDoFunc61,            //  61  0x6FB3B7D0
    MISSILE_CltDoFunc62,            //  62  0x6FB3BAF0
    MISSILE_CltDoFunc63,            //  63  0x6FB3BFE0
    MISSILE_CltDoFunc64,            //  64  0x6FB3C080
    MISSILE_CltDoFunc65,            //  65  0x6FB3C630
    MISSILE_CltDoFunc66,            //  66  0x6FB3C8A0
    MISSILE_CltDoFunc67,            //  67  0x6FB31660
    MISSILE_CltDoFunc68,            //  68  0x6FB34B00
    NULL, NULL, NULL, NULL, NULL,   //  69..73
    NULL, NULL, NULL, NULL, NULL,   //  74..78
    NULL, NULL, NULL, NULL, NULL,   //  79..83
    NULL, NULL, NULL, NULL, NULL,   //  84..88
    NULL, NULL,                     //  89..90
};
static_assert(sizeof(gpMissileCltDoFuncs) == 0x6FB913CC - 0x6FB91260
                  && sizeof(gpMissileCltDoFuncs) / sizeof(gpMissileCltDoFuncs[0]) == 91,
              "gpMissileCltDoFuncs: 91 slots, 0x6FB91260 up to its count at 0x6FB913CC");

//  D2Client 0x6FB913D0
//  gpMissileCltHitFuncs: the dispatch table for the missiles.txt CltHitFunc column, 81 slots (gnMissileCltHitFuncCount), indexed by that
//  column. 47 slots hold MISSILE_CltHitFunc<index> and 34 are NULL, which the caller skips. Slot 0 is unused (the call site requires an
//  index above 0) and the highest non-null index is 64.
//  The only reader, MISSILE_ClientOnHit, passes the missile in ecx and the target in edx and tests the return value.
inline int32_t (__fastcall *gpMissileCltHitFuncs[81])(D2UnitStrc *, D2UnitStrc *) =
{
    NULL,                           //   0
    MISSILE_CltHitFunc1,            //   1
    MISSILE_CltHitFunc2,            //   2
    MISSILE_CltHitFunc3,            //   3
    MISSILE_CltHitFunc4,            //   4
    NULL, NULL, NULL, NULL,         //   5..8
    MISSILE_CltHitFunc9,            //   9
    MISSILE_CltHitFunc10,           //  10
    NULL,                           //  11
    MISSILE_CltHitFunc12,           //  12
    MISSILE_CltHitFunc13,           //  13
    MISSILE_CltHitFunc14,           //  14
    NULL,                           //  15
    MISSILE_CltHitFunc16,           //  16
    NULL,                           //  17
    MISSILE_CltHitFunc18,           //  18
    MISSILE_CltHitFunc19,           //  19
    NULL, NULL, NULL, NULL,         //  20..23
    MISSILE_CltHitFunc24,           //  24
    MISSILE_CltHitFunc25,           //  25
    MISSILE_CltHitFunc26,           //  26
    NULL,                           //  27
    MISSILE_CltHitFunc28,           //  28
    MISSILE_CltHitFunc29,           //  29
    MISSILE_CltHitFunc30,           //  30
    MISSILE_CltHitFunc31,           //  31
    MISSILE_CltHitFunc32,           //  32
    MISSILE_CltHitFunc33,           //  33
    MISSILE_CltHitFunc34,           //  34
    NULL,                           //  35
    MISSILE_CltHitFunc36,           //  36
    MISSILE_CltHitFunc37,           //  37
    MISSILE_CltHitFunc38,           //  38
    MISSILE_CltHitFunc39,           //  39
    MISSILE_CltHitFunc40,           //  40
    MISSILE_CltHitFunc41,           //  41
    MISSILE_CltHitFunc42,           //  42
    MISSILE_CltHitFunc43,           //  43
    MISSILE_CltHitFunc44,           //  44
    NULL,                           //  45
    MISSILE_CltHitFunc46,           //  46
    MISSILE_CltHitFunc47,           //  47
    MISSILE_CltHitFunc48,           //  48
    NULL,                           //  49
    MISSILE_CltHitFunc50,           //  50
    MISSILE_CltHitFunc51,           //  51
    MISSILE_CltHitFunc52,           //  52
    MISSILE_CltHitFunc53,           //  53
    MISSILE_CltHitFunc54,           //  54
    MISSILE_CltHitFunc55,           //  55
    MISSILE_CltHitFunc56,           //  56
    MISSILE_CltHitFunc57,           //  57
    NULL, NULL,                     //  58..59
    MISSILE_CltHitFunc60,           //  60
    MISSILE_CltHitFunc61,           //  61
    MISSILE_CltHitFunc62,           //  62
    MISSILE_CltHitFunc63,           //  63
    MISSILE_CltHitFunc64,           //  64
    NULL, NULL, NULL, NULL,         //  65..68
    NULL, NULL, NULL, NULL,         //  69..72
    NULL, NULL, NULL, NULL,         //  73..76
    NULL, NULL, NULL, NULL,         //  77..80
};
static_assert(sizeof(gpMissileCltHitFuncs) == 0x6FB91514 - 0x6FB913D0
                  && sizeof(gpMissileCltHitFuncs) / sizeof(gpMissileCltHitFuncs[0]) == 81,
              "gpMissileCltHitFuncs: 81 slots, 0x6FB913D0 up to its count at 0x6FB91514");

//  D2Client 0x6FB70160
//  X half of a radius-30 circle sampled at 64 steps: entry i is trunc(30 * cos(i * 2*pi / 64)). gCircle30OffsetY is the sine half;
//  MISSILE_CltDoFunc19 reads both at the same index to step a missile effect around a ring.
inline const int32_t gCircle30OffsetX[64] =
{
    30,   // [0]
    29,   // [1]
    29,   // [2]
    28,   // [3]
    27,   // [4]
    26,   // [5]
    24,   // [6]
    23,   // [7]
    21,   // [8]
    19,   // [9]
    16,   // [10]
    14,   // [11]
    11,   // [12]
    8,   // [13]
    5,   // [14]
    2,   // [15]
    0,   // [16]
    -2,   // [17]
    -5,   // [18]
    -8,   // [19]
    -11,   // [20]
    -14,   // [21]
    -16,   // [22]
    -19,   // [23]
    -21,   // [24]
    -23,   // [25]
    -24,   // [26]
    -26,   // [27]
    -27,   // [28]
    -28,   // [29]
    -29,   // [30]
    -29,   // [31]
    -30,   // [32]
    -29,   // [33]
    -29,   // [34]
    -28,   // [35]
    -27,   // [36]
    -26,   // [37]
    -24,   // [38]
    -23,   // [39]
    -21,   // [40]
    -19,   // [41]
    -16,   // [42]
    -14,   // [43]
    -11,   // [44]
    -8,   // [45]
    -5,   // [46]
    -2,   // [47]
    0,   // [48]
    2,   // [49]
    5,   // [50]
    8,   // [51]
    11,   // [52]
    14,   // [53]
    16,   // [54]
    19,   // [55]
    21,   // [56]
    23,   // [57]
    24,   // [58]
    26,   // [59]
    27,   // [60]
    28,   // [61]
    29,   // [62]
    29,   // [63]
};
static_assert(sizeof(gCircle30OffsetX) == 256, "gCircle30OffsetX: the item's 256 bytes in the .i64");

//  D2Client 0x6FB70260
//  Y half of the radius-30, 64-step circle: entry i is trunc(30 * sin(i * 2*pi / 64)). Paired with gCircle30OffsetX.
inline const int32_t gCircle30OffsetY[64] =
{
    0,   // [0]
    2,   // [1]
    5,   // [2]
    8,   // [3]
    11,   // [4]
    14,   // [5]
    16,   // [6]
    19,   // [7]
    21,   // [8]
    23,   // [9]
    24,   // [10]
    26,   // [11]
    27,   // [12]
    28,   // [13]
    29,   // [14]
    29,   // [15]
    30,   // [16]
    29,   // [17]
    29,   // [18]
    28,   // [19]
    27,   // [20]
    26,   // [21]
    24,   // [22]
    23,   // [23]
    21,   // [24]
    19,   // [25]
    16,   // [26]
    14,   // [27]
    11,   // [28]
    8,   // [29]
    5,   // [30]
    2,   // [31]
    0,   // [32]
    -2,   // [33]
    -5,   // [34]
    -8,   // [35]
    -11,   // [36]
    -14,   // [37]
    -16,   // [38]
    -19,   // [39]
    -21,   // [40]
    -23,   // [41]
    -24,   // [42]
    -26,   // [43]
    -27,   // [44]
    -28,   // [45]
    -29,   // [46]
    -29,   // [47]
    -30,   // [48]
    -29,   // [49]
    -29,   // [50]
    -28,   // [51]
    -27,   // [52]
    -26,   // [53]
    -24,   // [54]
    -23,   // [55]
    -21,   // [56]
    -19,   // [57]
    -16,   // [58]
    -14,   // [59]
    -11,   // [60]
    -8,   // [61]
    -5,   // [62]
    -2,   // [63]
};
static_assert(sizeof(gCircle30OffsetY) == 256, "gCircle30OffsetY: the item's 256 bytes in the .i64");

//  D2Client 0x6FB70364
//  `int[4]` -- the X half of a four-direction unit offset, {-1, 0, 1, 0}, read by MISSILE_CltDoFunc28 (stock user: mephistodonotdraw).
//  With gnMephistoDir4OffsetY it gives (-1,0) (0,-1) (1,0) (0,1).
inline const int32_t gnMephistoDir4OffsetX[4] = { -1, 0, 1 };
static_assert(sizeof(gnMephistoDir4OffsetX) == 16, "gnMephistoDir4OffsetX: the item's 16 bytes in the .i64");

//  D2Client 0x6FB70374
//  `int[4]` -- the Y half of MISSILE_CltDoFunc28's four-direction offset, {0, -1, 0, 1}, read with the same index as the X half.
inline const int32_t gnMephistoDir4OffsetY[4] = { 0, -1, 0, 1 };
static_assert(sizeof(gnMephistoDir4OffsetY) == 16, "gnMephistoDir4OffsetY: the item's 16 bytes in the .i64");

//  D2Client 0x6FB70388
//  `int[16]` -- one axis of a radius-2.5 circle sampled sixteen times, read by MISSILE_SpawnFromMissile: entry k is
//  round(2.5 * sin(2*pi*k / 16)), rounded to nearest (the radius-30 circles are truncated).
inline const int32_t gnSpawnRing16X[16] = { 0, 1, 2, 2, 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2, -1 };
static_assert(sizeof(gnSpawnRing16X) == 64, "gnSpawnRing16X: the item's 64 bytes in the .i64");

//  D2Client 0x6FB703C8
//  `int[16]` -- the other axis of MISSILE_SpawnFromMissile's radius-2.5 circle, round(2.5 * cos(2*pi*k / 16)). It is the sine table shifted by
//  four entries: gnSpawnRing16Y[k] == gnSpawnRing16X[(k + 4) % 16].
inline const int32_t gnSpawnRing16Y[16] = { 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2, -1, 0, 1, 2, 2 };
static_assert(sizeof(gnSpawnRing16Y) == 64, "gnSpawnRing16Y: the item's 64 bytes in the .i64");

//  D2Client 0x6FB70408
//  `int[8]` -- the X half of an eight-direction offset table, +/-16 per step: (16,0) (16,16) (0,16) (-16,16) (-16,0) (-16,-16) (0,-16) (16,-16).
//  The diagonals are as long as the axes (not 16/sqrt(2)). Read by MISSILE_CltHitFunc64, which no stock missile uses.
inline const int32_t gnMissileDir8OffsetX[8] = { 16, 16, 0, -16, -16, -16, 0, 16 };
static_assert(sizeof(gnMissileDir8OffsetX) == 32, "gnMissileDir8OffsetX: the item's 32 bytes in the .i64");

//  D2Client 0x6FB70428
//  `int[8]` -- the Y half of the eight-direction offset table: 0, 16, 16, 16, 0, -16, -16, -16. Paired with gnMissileDir8OffsetX in
//  MISSILE_CltHitFunc64, which reads each of the pair twice per direction: for MISSILE_ClientSpawn, and for D2COMMON_MISSILE_SetTargetX / SetTargetY.
inline const int32_t gnMissileDir8OffsetY[8] = { 0, 16, 16, 16, 0, -16, -16, -16 };
static_assert(sizeof(gnMissileDir8OffsetY) == 32, "gnMissileDir8OffsetY: the item's 32 bytes in the .i64");

//  D2Client 0x6FB7044C
//  X deltas for the eight compass directions, paired with gnDirection8OffsetY: (0,1) (1,1) (1,0) (1,-1) (0,-1) (-1,-1) (-1,0) (-1,1), unit
//  steps starting at +Y and turning. MISSILE_CltHitFunc14 passes this table as nX and gnDirection8OffsetY as nY to PATH_FaceCoords.
inline const int32_t gnDirection8OffsetX[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
static_assert(sizeof(gnDirection8OffsetX) == 32, "gnDirection8OffsetX: the item's 32 bytes in the .i64");

//  D2Client 0x6FB7046C
//  Y deltas for the eight compass directions; see gnDirection8OffsetX.
inline const int32_t gnDirection8OffsetY[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
static_assert(sizeof(gnDirection8OffsetY) == 32, "gnDirection8OffsetY: the item's 32 bytes in the .i64");

//  D2Client 0x6FB7048C
//  `int[15]` -- three records of five ints, { int nCount; int nDirDelta[4]; }, a 20-byte stride picked by `seed % 3`; MISSILE_CltHitFunc14
//  indexes it twice. The handler is the ice burst that throws sub-missiles on impact (stock users: freezingarrow, glacialspike,
//  monglacialspike). Element 0 is read as a count and elements 1..4 as direction deltas.
inline const int32_t gnGlacialSpikeBurstParams[15] = { 4, 0, 1, 4, 5, 4, 0, 2, 4, 6, 3, 0, 3, 5 };
static_assert(sizeof(gnGlacialSpikeBurstParams) == 60, "gnGlacialSpikeBurstParams: the item's 60 bytes in the .i64");

//  D2Client 0x6FB704C8
//  `int[18]` -- the X offsets of the scattered fire splashes around a meteor's crater, indexed together with gnMeteorSplashOffsetY by
//  MISSILE_CltHitFunc18 as the loop counter modulo 18, in both of its loops (stock users: meteorcenter, vampiremeteorcenter,
//  royalstrikemeteorcenter). The same 18 values are repeated in gnBoulderFireOffsetX.
inline const int32_t gnMeteorSplashOffsetX[18] = { 2, -2, 0, 0, -3, 0, 3, -1, 1, -1, 2, -4, -3, -1, 0, 1, 3, 4 };
static_assert(sizeof(gnMeteorSplashOffsetX) == 72, "gnMeteorSplashOffsetX: the item's 72 bytes in the .i64");

//  D2Client 0x6FB70510
//  `int[18]` -- the Y half of MISSILE_CltHitFunc18's splash offsets, indexed like gnMeteorSplashOffsetX. The same 18 values are repeated in
//  gnBoulderFireOffsetY.
inline const int32_t gnMeteorSplashOffsetY[18] = { -2, -2, 2, 5, 3, 3, 3, 2, 1, -1, -1, -2, -2, -3, -4, -3, -3, -2 };
static_assert(sizeof(gnMeteorSplashOffsetY) == 72, "gnMeteorSplashOffsetY: the item's 72 bytes in the .i64");

//  D2Client 0x6FB70558
//  `int[5]` -- {0, 4, 0, -4, 0}, indexed modulo 5 by MISSILE_CltHitFunc18 together with gnMeteorCrossOffsetY. The pairs are (0,0) (4,0) (0,4)
//  (-4,0) (0,-4): a centre plus four compass points at radius 4, the meteor's five fire splashes.
inline const int32_t gnMeteorCrossOffsetX[5] = { 0, 4, 0, -4 };
static_assert(sizeof(gnMeteorCrossOffsetX) == 20, "gnMeteorCrossOffsetX: the item's 20 bytes in the .i64");

//  D2Client 0x6FB7056C
//  `int[5]` -- {0, 0, 4, 0, -4}, the Y half of MISSILE_CltHitFunc18's centre-plus-cross offsets. It follows gnMeteorCrossOffsetX directly:
//  the two are the halves of one 10-int block.
inline const int32_t gnMeteorCrossOffsetY[5] = { 0, 0, 4, 0, -4 };
static_assert(sizeof(gnMeteorCrossOffsetY) == 20, "gnMeteorCrossOffsetY: the item's 20 bytes in the .i64");

//  D2Client 0x6FB70580
//  X offsets of the eight directions MISSILE_SpawnRingOfMissiles fires into; gaMissileRingOffsetY begins 32 bytes later. The caller's nStep
//  strides through them, so the same tables serve an eight-, four- or two-way spray.
inline const int32_t gaMissileRingOffsetX[8] = { 0, 2, 2, 2, 0, -2, -2, -2 };
static_assert(sizeof(gaMissileRingOffsetX) == 32, "gaMissileRingOffsetX: the item's 32 bytes in the .i64");

//  D2Client 0x6FB705A0
//  Y offsets of the eight directions; the partner of gaMissileRingOffsetX.
inline const int32_t gaMissileRingOffsetY[8] = { 2, 2, 0, -2, -2, -2, 0, 2 };
static_assert(sizeof(gaMissileRingOffsetY) == 32, "gaMissileRingOffsetY: the item's 32 bytes in the .i64");

//  D2Client 0x6FB705C0
//  `int[64]` -- the X offsets of Frozen Orb's bolt ring: entry k is trunc(30 * cos(2*pi*k / 64)). MISSILE_CltHitFunc30 (stock user: frozenorb)
//  reads it with gnFrozenOrbRingY, twice per use. 64 points is the resolution of the orb's spray. gnHitFunc4RingX is a second copy of this pair.
inline const int32_t gnFrozenOrbRingX[64] =
{
    30,   // [0]
    29,   // [1]
    29,   // [2]
    28,   // [3]
    27,   // [4]
    26,   // [5]
    24,   // [6]
    23,   // [7]
    21,   // [8]
    19,   // [9]
    16,   // [10]
    14,   // [11]
    11,   // [12]
    8,   // [13]
    5,   // [14]
    2,   // [15]
    0,   // [16]
    -2,   // [17]
    -5,   // [18]
    -8,   // [19]
    -11,   // [20]
    -14,   // [21]
    -16,   // [22]
    -19,   // [23]
    -21,   // [24]
    -23,   // [25]
    -24,   // [26]
    -26,   // [27]
    -27,   // [28]
    -28,   // [29]
    -29,   // [30]
    -29,   // [31]
    -30,   // [32]
    -29,   // [33]
    -29,   // [34]
    -28,   // [35]
    -27,   // [36]
    -26,   // [37]
    -24,   // [38]
    -23,   // [39]
    -21,   // [40]
    -19,   // [41]
    -16,   // [42]
    -14,   // [43]
    -11,   // [44]
    -8,   // [45]
    -5,   // [46]
    -2,   // [47]
    0,   // [48]
    2,   // [49]
    5,   // [50]
    8,   // [51]
    11,   // [52]
    14,   // [53]
    16,   // [54]
    19,   // [55]
    21,   // [56]
    23,   // [57]
    24,   // [58]
    26,   // [59]
    27,   // [60]
    28,   // [61]
    29,   // [62]
    29,   // [63]
};
static_assert(sizeof(gnFrozenOrbRingX) == 256, "gnFrozenOrbRingX: the item's 256 bytes in the .i64");

//  D2Client 0x6FB706C0
//  `int[64]` -- the Y offsets of Frozen Orb's bolt ring: entry k is trunc(30 * sin(2*pi*k / 64)). The partner of gnFrozenOrbRingX.
inline const int32_t gnFrozenOrbRingY[64] =
{
    0,   // [0]
    2,   // [1]
    5,   // [2]
    8,   // [3]
    11,   // [4]
    14,   // [5]
    16,   // [6]
    19,   // [7]
    21,   // [8]
    23,   // [9]
    24,   // [10]
    26,   // [11]
    27,   // [12]
    28,   // [13]
    29,   // [14]
    29,   // [15]
    30,   // [16]
    29,   // [17]
    29,   // [18]
    28,   // [19]
    27,   // [20]
    26,   // [21]
    24,   // [22]
    23,   // [23]
    21,   // [24]
    19,   // [25]
    16,   // [26]
    14,   // [27]
    11,   // [28]
    8,   // [29]
    5,   // [30]
    2,   // [31]
    0,   // [32]
    -2,   // [33]
    -5,   // [34]
    -8,   // [35]
    -11,   // [36]
    -14,   // [37]
    -16,   // [38]
    -19,   // [39]
    -21,   // [40]
    -23,   // [41]
    -24,   // [42]
    -26,   // [43]
    -27,   // [44]
    -28,   // [45]
    -29,   // [46]
    -29,   // [47]
    -30,   // [48]
    -29,   // [49]
    -29,   // [50]
    -28,   // [51]
    -27,   // [52]
    -26,   // [53]
    -24,   // [54]
    -23,   // [55]
    -21,   // [56]
    -19,   // [57]
    -16,   // [58]
    -14,   // [59]
    -11,   // [60]
    -8,   // [61]
    -5,   // [62]
    -2,   // [63]
};
static_assert(sizeof(gnFrozenOrbRingY) == 256, "gnFrozenOrbRingY: the item's 256 bytes in the .i64");

//  D2Client 0x6FB707C0
//  `int[64]` -- a second copy of gnFrozenOrbRingX, compiled for MISSILE_CltHitFunc4. No stock missile selects CltHitFunc 4: it is dead in the
//  stock data and live only in a mod that sets the column.
inline const int32_t gnHitFunc4RingX[64] =
{
    30,   // [0]
    29,   // [1]
    29,   // [2]
    28,   // [3]
    27,   // [4]
    26,   // [5]
    24,   // [6]
    23,   // [7]
    21,   // [8]
    19,   // [9]
    16,   // [10]
    14,   // [11]
    11,   // [12]
    8,   // [13]
    5,   // [14]
    2,   // [15]
    0,   // [16]
    -2,   // [17]
    -5,   // [18]
    -8,   // [19]
    -11,   // [20]
    -14,   // [21]
    -16,   // [22]
    -19,   // [23]
    -21,   // [24]
    -23,   // [25]
    -24,   // [26]
    -26,   // [27]
    -27,   // [28]
    -28,   // [29]
    -29,   // [30]
    -29,   // [31]
    -30,   // [32]
    -29,   // [33]
    -29,   // [34]
    -28,   // [35]
    -27,   // [36]
    -26,   // [37]
    -24,   // [38]
    -23,   // [39]
    -21,   // [40]
    -19,   // [41]
    -16,   // [42]
    -14,   // [43]
    -11,   // [44]
    -8,   // [45]
    -5,   // [46]
    -2,   // [47]
    0,   // [48]
    2,   // [49]
    5,   // [50]
    8,   // [51]
    11,   // [52]
    14,   // [53]
    16,   // [54]
    19,   // [55]
    21,   // [56]
    23,   // [57]
    24,   // [58]
    26,   // [59]
    27,   // [60]
    28,   // [61]
    29,   // [62]
    29,   // [63]
};
static_assert(sizeof(gnHitFunc4RingX) == 256, "gnHitFunc4RingX: the item's 256 bytes in the .i64");

//  D2Client 0x6FB708C0
//  `int[64]` -- the copy of gnFrozenOrbRingY that belongs to MISSILE_CltHitFunc4; see gnHitFunc4RingX.
inline const int32_t gnHitFunc4RingY[64] =
{
    0,   // [0]
    2,   // [1]
    5,   // [2]
    8,   // [3]
    11,   // [4]
    14,   // [5]
    16,   // [6]
    19,   // [7]
    21,   // [8]
    23,   // [9]
    24,   // [10]
    26,   // [11]
    27,   // [12]
    28,   // [13]
    29,   // [14]
    29,   // [15]
    30,   // [16]
    29,   // [17]
    29,   // [18]
    28,   // [19]
    27,   // [20]
    26,   // [21]
    24,   // [22]
    23,   // [23]
    21,   // [24]
    19,   // [25]
    16,   // [26]
    14,   // [27]
    11,   // [28]
    8,   // [29]
    5,   // [30]
    2,   // [31]
    0,   // [32]
    -2,   // [33]
    -5,   // [34]
    -8,   // [35]
    -11,   // [36]
    -14,   // [37]
    -16,   // [38]
    -19,   // [39]
    -21,   // [40]
    -23,   // [41]
    -24,   // [42]
    -26,   // [43]
    -27,   // [44]
    -28,   // [45]
    -29,   // [46]
    -29,   // [47]
    -30,   // [48]
    -29,   // [49]
    -29,   // [50]
    -28,   // [51]
    -27,   // [52]
    -26,   // [53]
    -24,   // [54]
    -23,   // [55]
    -21,   // [56]
    -19,   // [57]
    -16,   // [58]
    -14,   // [59]
    -11,   // [60]
    -8,   // [61]
    -5,   // [62]
    -2,   // [63]
};
static_assert(sizeof(gnHitFunc4RingY) == 256, "gnHitFunc4RingY: the item's 256 bytes in the .i64");

//  D2Client 0x6FB709C0
//  `int[18]` -- the X offsets of the 18 ground-fire missiles MISSILE_SpawnBoulderImpactDebris drops around a molten boulder's impact: a
//  hand-placed scatter, not a circle. Paired with gnBoulderFireOffsetY, where this array ends.
inline const int32_t gnBoulderFireOffsetX[18] = { 2, -2, 0, 0, -3, 0, 3, -1, 1, -1, 2, -4, -3, -1, 0, 1, 3, 4 };
static_assert(sizeof(gnBoulderFireOffsetX) == 72, "gnBoulderFireOffsetX: the item's 72 bytes in the .i64");

//  D2Client 0x6FB70A08
//  `int[18]` -- the Y half of gnBoulderFireOffsetX.
inline const int32_t gnBoulderFireOffsetY[18] = { -2, -2, 2, 5, 3, 3, 3, 2, 1, -1, -1, -2, -2, -3, -4, -3, -3, -2 };
static_assert(sizeof(gnBoulderFireOffsetY) == 72, "gnBoulderFireOffsetY: the item's 72 bytes in the .i64");

//  D2Client 0x6FB70A50
inline const int32_t gnHoradricLightSteps[21] = { 3, -2, 4, 1, -3, 58, 3, 1, 13, -2, -3, 49, 1, 3, 22, -3, 0, 40, -2, 2, 31 };
static_assert(sizeof(gnHoradricLightSteps) == 84, "gnHoradricLightSteps: the item's 84 bytes in the .i64");

//  D2Client 0x6FB70AA8
//  gRadamentDeathFx: the stage table MISSILE_CltDoFunc32 walks (stock user: radamentdeath, Radament's death effect), six D2MissileFxStageStrc
//  records { nFrameBound, nPeriod, nParam } from this address; the period is the divisor, indexed by stage. Row 5 is the terminator row: when
//  no row's bound is below the frame the walk ends on it and reads its nPeriod and nParam ({0, 100, 0}).
//  The nParam column's meaning is not established.
inline const D2MissileFxStageStrc gRadamentDeathFx[6] =
{
    { .nFrameBound = 300, .nPeriod = 25, .nParam = 1 },   // [0]
    { .nFrameBound = 250, .nPeriod = 10, .nParam = 3 },   // [1]
    { .nFrameBound = 100, .nPeriod = 8, .nParam = 4 },   // [2]
    { .nFrameBound = 75, .nPeriod = 6, .nParam = 4 },   // [3]
    { .nFrameBound = 30, .nPeriod = 4, .nParam = 3 },   // [4]
    { .nPeriod = 100 },   // [5]
};
static_assert(sizeof(gRadamentDeathFx) == 72, "gRadamentDeathFx: the item's 72 bytes in the .i64");

//  D2Client 0x6FB70AF0
//  gTaintedSunFx: the stage table MISSILE_CltDoFunc33 walks (stock user: taintedsuncontrol, the Tainted Sun quest's darkness effect), five
//  D2MissileFxStageStrc records from this address; nPeriod is the divisor, indexed by stage. Row 4 is the terminator row, read when no bound
//  is below the frame ({0, 100, 0}).
inline const D2MissileFxStageStrc gTaintedSunFx[5] =
{
    { .nFrameBound = 350, .nPeriod = 10, .nParam = 1 },   // [0]
    { .nFrameBound = 300, .nPeriod = 5, .nParam = 1 },   // [1]
    { .nFrameBound = 200, .nPeriod = 4, .nParam = 1 },   // [2]
    { .nFrameBound = 175, .nPeriod = 3, .nParam = 1 },   // [3]
    { .nPeriod = 100 },   // [4]
};
static_assert(sizeof(gTaintedSunFx) == 60, "gTaintedSunFx: the item's 60 bytes in the .i64");

//  D2Client 0x6FB70B2C
//  gDurielDeathFx: the stage table MISSILE_CltDoFunc36 walks (stock user: durieldeathcontrol), five D2MissileFxStageStrc records from this
//  address; nPeriod and nParam are both divisors here, indexed by stage. Row 4 is the terminator row, read when no bound is below the frame
//  ({0, 100, 5}).
inline const D2MissileFxStageStrc gDurielDeathFx[5] =
{
    { .nFrameBound = 350, .nPeriod = 3, .nParam = 5 },   // [0]
    { .nFrameBound = 300, .nPeriod = 2, .nParam = 3 },   // [1]
    { .nFrameBound = 200, .nPeriod = 3, .nParam = 2 },   // [2]
    { .nFrameBound = 175, .nPeriod = 5, .nParam = 1 },   // [3]
    { .nPeriod = 100, .nParam = 5 },   // [4]
};
static_assert(sizeof(gDurielDeathFx) == 60, "gDurielDeathFx: the item's 60 bytes in the .i64");

//  D2Client 0x6FB70B68
//  gBaalFx: the stage table MISSILE_CltDoFunc64 walks (stock user: `baalfx control`), five D2MissileFxStageStrc records from this address;
//  nPeriod is the divisor, indexed by stage, and nParam a spirit flag. The bounds do not descend (420, 435, 470, 270), so rows 1 and 2 never
//  answer. Row 4 is the terminator row ({0, 64, 0}). The dword after it (0x6FB70BA4, value 5) is outside the array; nothing references it.
inline const D2MissileFxStageStrc gBaalFx[5] =
{
    { .nFrameBound = 420, .nPeriod = 64 },   // [0]
    { .nFrameBound = 435, .nPeriod = 20, .nParam = 1 },   // [1]
    { .nFrameBound = 470, .nPeriod = 8, .nParam = 1 },   // [2]
    { .nFrameBound = 270, .nPeriod = 5, .nParam = 1 },   // [3]
    { .nPeriod = 64 },   // [4]
};
static_assert(sizeof(gBaalFx) == 60, "gBaalFx: the item's 60 bytes in the .i64");

//  D2Client 0x6FB91208
//  Three Missiles.txt ids the Hell Meteor scatters: 69 firewall, 104 firesmall, 105 firemedium, sprayed per tile by MISSILE_SprayRandomOverArea
//  for MISSILE_CltHitFunc36. The count 3 is pushed at the single call site (0x6FB38435).
inline int32_t gaFireSprayMissiles[3] = { 69, 104, 105 };
static_assert(sizeof(gaFireSprayMissiles) == 12, "gaFireSprayMissiles: the item's 12 bytes in the .i64");

//  D2Client 0x6FB91214
//  The Lighting Quality setting from the ESC menu's video page. Written by MISSILE_EnvSetLightQuality (which ESCMENU_LightQualityOnChange
//  and ESCMENU_LightQualityRefresh tail-jump to), read by MISSILE_ClientSpawn to decide whether a spawned missile carries a light, and by
//  MISSILE_GetLightQuality for DLIGHTMAP_UpdateQualityLevel.
inline int32_t gnLightQuality = 1;
static_assert(sizeof(gnLightQuality) == 4, "gnLightQuality: the item's 4 bytes in the .i64");

//  D2Client 0x6FB913CC
//  The number of entries in gpMissileCltDoFuncs, 91. MISSILE_ClientUpdate reads it as the upper bound of the dispatch (`cmp eax, ecx / jge`)
//  and rejects an index of 0 or below separately, so the live range is 1..90.
inline int32_t gnMissileCltDoFuncCount = 91;
static_assert(sizeof(gnMissileCltDoFuncCount) == 4, "gnMissileCltDoFuncCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FB91514
//  The number of entries in gpMissileCltHitFuncs, 81; it sits right after the table.
inline int32_t gnMissileCltHitFuncCount = 81;
static_assert(sizeof(gnMissileCltHitFuncCount) == 4, "gnMissileCltHitFuncCount: the item's 4 bytes in the .i64");

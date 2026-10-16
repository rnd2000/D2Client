//  Skills.h -- the globals of Skills.cpp that are ours: the client skill dispatch tables (gpSkillCltStFuncs, gpSkillCltDoFuncs,
//  gpStateCltEventFuncs) and their counts, the missile ring and array tables, and gnOpenWoundsNextFrame.
//  Each is a C++17 inline variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above a global
//  gives its address in the original. The three dispatch tables and the two counts lie in .data and are not const; the eight missile tables
//  lie in .rdata and are const. gnOpenWoundsNextFrame lies past .data's raw data (0x6FB9B000) and is zero at load.
//  Include it after D2Client.h, which declares every function named here.
#pragma once

//  D2Client 0x6FB77488
//  The skills.txt CltStFunc dispatch table, 70 entries, count in gnSkillCltStFuncCount (0x6FB775A0). The index is the wCltStFunc word at +0xF2
//  of the skill's D2SkillsTxt record; SKILLS_RunCltStFunc calls f(pUnit, nSkillId, nSkillLevel). [0] is null, [1..53] are SKILLS_CltStFunc1..53,
//  [54..69] are reserved (null). Every handler takes (pUnit, nSkillId, nSkillLevel).
inline int32_t (__fastcall *gpSkillCltStFuncs[70])(D2UnitStrc *, D2C_Skills, int32_t) =
{
    NULL,                      //   0  0x6FB77488
    SKILLS_CltStFunc1,         //   1  0x6FABB960
    SKILLS_CltStFunc2,         //   2  0x6FABB9B0
    SKILLS_CltStFunc3,         //   3  0x6FABB9E0
    SKILLS_CltStFunc4,         //   4  0x6FABBA80
    SKILLS_CltStFunc5,         //   5  0x6FABF4B0
    SKILLS_CltStFunc6,         //   6  0x6FABF580
    SKILLS_CltStFunc7,         //   7  0x6FABF910
    SKILLS_CltStFunc8,         //   8  0x6FABFAC0
    SKILLS_CltStFunc9,         //   9  0x6FABFC50
    SKILLS_CltStFunc10,        //  10  0x6FABFE60
    SKILLS_CltStFunc11,        //  11  0x6FABE2A0
    SKILLS_CltStFunc12,        //  12  0x6FABE2B0
    SKILLS_CltStFunc13,        //  13  0x6FABE2D0
    SKILLS_CltStFunc14,        //  14  0x6FABE490
    SKILLS_CltStFunc15,        //  15  0x6FACA050
    SKILLS_CltStFunc16,        //  16  0x6FACA3D0
    SKILLS_CltStFunc17,        //  17  0x6FACACA0
    SKILLS_CltStFunc18,        //  18  0x6FAC7F90
    SKILLS_CltStFunc19,        //  19  0x6FAC8150
    SKILLS_CltStFunc20,        //  20  0x6FAC8370
    SKILLS_CltStFunc21,        //  21  0x6FAC8530
    SKILLS_CltStFunc22,        //  22  0x6FAC8B30
    SKILLS_CltStFunc23,        //  23  0x6FAC8C80
    SKILLS_CltStFunc24,        //  24  0x6FAC8D00
    SKILLS_CltStFunc25,        //  25  0x6FAC9820
    SKILLS_CltStFunc26,        //  26  0x6FAC0D30
    SKILLS_CltStFunc27,        //  27  0x6FAC0D90
    SKILLS_CltStFunc28,        //  28  0x6FAC1000
    SKILLS_CltStFunc29,        //  29  0x6FAC1310
    SKILLS_CltStFunc30,        //  30  0x6FAC2180
    SKILLS_CltStFunc31,        //  31  0x6FAC24C0
    SKILLS_CltStFunc32,        //  32  0x6FAC51F0
    SKILLS_CltStFunc33,        //  33  0x6FAC5440
    SKILLS_CltStFunc34,        //  34  0x6FAC5510
    SKILLS_CltStFunc35,        //  35  0x6FAC57D0
    SKILLS_CltStFunc36,        //  36  0x6FAC5900
    SKILLS_CltStFunc37,        //  37  0x6FAC5DC0
    SKILLS_CltStFunc38,        //  38  0x6FAC6240
    SKILLS_CltStFunc39,        //  39  0x6FAC6260
    SKILLS_CltStFunc40,        //  40  0x6FAC6460
    SKILLS_CltStFunc41,        //  41  0x6FAC6B50
    SKILLS_CltStFunc42,        //  42  0x6FAC71E0
    SKILLS_CltStFunc43,        //  43  0x6FAC7400
    SKILLS_CltStFunc44,        //  44  0x6FAC7770
    SKILLS_CltStFunc45,        //  45  0x6FAC2840
    SKILLS_CltStFunc46,        //  46  0x6FAC38E0
    SKILLS_CltStFunc47,        //  47  0x6FAC3970
    SKILLS_CltStFunc48,        //  48  0x6FAC3B00
    SKILLS_CltStFunc49,        //  49  0x6FAC3D50
    SKILLS_CltStFunc50,        //  50  0x6FAC4540
    SKILLS_CltStFunc51,        //  51  0x6FAC4710
    SKILLS_CltStFunc52,        //  52  0x6FAC43B0
    SKILLS_CltStFunc53,        //  53  0x6FABE610
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //  54..61
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //  62..69
};

//  D2Client 0x6FB775A8
//  The client effect dispatch table, 130 entries, 1-based with a null at index 0; its count is gnSkillCltDoFuncCount (0x6FB777B0). Slots 1..96
//  are populated.
//  It is indexed by six different excel columns: skills.txt cltdofunc, ItemCltEffect and cltprgfunc1/2/3, plus states.txt cltactivefunc. That is
//  why it has 130 slots when states.txt only reaches 90, and why SKILLS_RunClientDoFunc bounds a skills column against its count.
//  BUG: the PrgStack loop of SKILLS_ApplyClientSkillEffect indexes this table by an index nothing bounds, so the original reads past the end.
//  LATENT: the one sender of a progressive skill keeps the index in range.
//  DIFFERENCE: ours answers NULL for a slot past the table (SKILLS_GetCltDoFuncOrNull in Skills.cpp) and both callers test it.
inline int32_t (__fastcall *gpSkillCltDoFuncs[130])(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel) =
{
    NULL,                      //   0  0x6FB775A8
    SKILLS_CltDoFunc1,         //   1  0x6FABBA90
    SKILLS_CltDoFunc2,         //   2  0x6FABBC70
    SKILLS_CltDoFunc3,         //   3  0x6FABF510
    SKILLS_CltDoFunc4,         //   4  0x6FABF620
    SKILLS_CltDoFunc5,         //   5  0x6FABF850
    SKILLS_CltDoFunc6,         //   6  0x6FABFAD0
    SKILLS_CltDoFunc7,         //   7  0x6FABFCB0
    SKILLS_CltDoFunc8,         //   8  0x6FABFD40
    SKILLS_CltDoFunc9,         //   9  0x6FAC00B0
    SKILLS_CltDoFunc10,        //  10  0x6FAC01F0
    SKILLS_CltDoFunc11,        //  11  0x6FAC0280
    SKILLS_CltDoFunc12,        //  12  0x6FAC08E0
    SKILLS_CltDoFunc13,        //  13  0x6FAC09F0
    SKILLS_CltDoFunc14,        //  14  0x6FAC0AE0
    SKILLS_CltDoFunc15,        //  15  0x6FAC0B80
    SKILLS_CltDoFunc16,        //  16  0x6FABE760
    SKILLS_CltDoFunc17,        //  17  0x6FABE7F0
    SKILLS_CltDoFunc18,        //  18  0x6FABEA20
    SKILLS_CltDoFunc19,        //  19  0x6FABEBC0
    SKILLS_CltDoFunc20,        //  20  0x6FABEDC0
    SKILLS_CltDoFunc21,        //  21  0x6FABF0C0
    SKILLS_CltDoFunc22,        //  22  0x6FABF2A0
    SKILLS_CltDoFunc23,        //  23  0x6FAC9EE0
    SKILLS_CltDoFunc24,        //  24  0x6FACA3B0
    SKILLS_CltDoFunc25,        //  25  0x6FACA570
    SKILLS_CltDoFunc26,        //  26  0x6FACA630
    SKILLS_CltDoFunc27,        //  27  0x6FACA900
    SKILLS_CltDoFunc28,        //  28  0x6FACAC10
    SKILLS_CltDoFunc29,        //  29  0x6FACADF0
    SKILLS_CltDoFunc30,        //  30  0x6FAC7FF0
    SKILLS_CltDoFunc31,        //  31  0x6FAC8440
    SKILLS_CltDoFunc32,        //  32  0x6FAC88F0
    SKILLS_CltDoFunc33,        //  33  0x6FAC8AE0
    SKILLS_CltDoFunc34,        //  34  0x6FAC8EE0
    SKILLS_CltDoFunc35,        //  35  0x6FAC90E0
    SKILLS_CltDoFunc36,        //  36  0x6FAC91F0
    SKILLS_CltDoFunc37,        //  37  0x6FAC9B50
    SKILLS_CltDoFunc38,        //  38  0x6FAC0D50
    SKILLS_CltDoFunc39,        //  39  0x6FAC0DF0
    SKILLS_CltDoFunc40,        //  40  0x6FAC1020
    SKILLS_CltDoFunc41,        //  41  0x6FAC1070
    SKILLS_CltDoFunc42,        //  42  0x6FAC0F40
    SKILLS_CltDoFunc43,        //  43  0x6FAC1DC0
    SKILLS_CltDoFunc44,        //  44  0x6FAC22F0
    SKILLS_CltDoFunc45,        //  45  0x6FAC2700
    SKILLS_CltDoFunc46,        //  46  0x6FAC51E0
    SKILLS_CltDoFunc47,        //  47  0x6FAC54D0
    SKILLS_CltDoFunc48,        //  48  0x6FAC55C0
    SKILLS_CltDoFunc49,        //  49  0x6FAC5860
    SKILLS_CltDoFunc50,        //  50  0x6FAC5A50
    SKILLS_CltDoFunc51,        //  51  0x6FAC5B50
    SKILLS_CltDoFunc47,        //  52  0x6FAC54D0, the function slot 47 holds
    SKILLS_CltDoFunc53,        //  53  0x6FAC5DE0
    SKILLS_CltDoFunc54,        //  54  0x6FAC5ED0
    SKILLS_CltDoFunc55,        //  55  0x6FAC6290
    SKILLS_CltDoFunc56,        //  56  0x6FAC64E0
    SKILLS_CltDoFunc57,        //  57  0x6FAC66A0
    SKILLS_CltDoFunc58,        //  58  0x6FAC6960
    SKILLS_CltDoFunc59,        //  59  0x6FAC6B70
    SKILLS_CltDoFunc60,        //  60  0x6FAC6DB0
    SKILLS_CltDoFunc61,        //  61  0x6FAC7030
    SKILLS_CltDoFunc62,        //  62  0x6FAC7330
    SKILLS_CltDoFunc63,        //  63  0x6FAC7460
    SKILLS_CltDoFunc64,        //  64  0x6FAC75C0
    SKILLS_CltDoFunc65,        //  65  0x6FAC7650
    SKILLS_CltDoFunc66,        //  66  0x6FAC7C30
    SKILLS_CltDoFunc67,        //  67  0x6FAC2A80
    SKILLS_CltDoFunc68,        //  68  0x6FAC2B30
    SKILLS_CltDoFunc69,        //  69  0x6FAC2BE0
    SKILLS_CltDoFunc70,        //  70  0x6FAC3310
    SKILLS_CltDoFunc71,        //  71  0x6FAC3480
    SKILLS_CltDoFunc72,        //  72  0x6FAC37C0
    SKILLS_CltDoFunc73,        //  73  0x6FAC3BC0
    SKILLS_CltDoFunc74,        //  74  0x6FAC3C90
    SKILLS_CltDoFunc75,        //  75  0x6FAC3F30
    SKILLS_CltDoFunc76,        //  76  0x6FAC4070
    SKILLS_CltDoFunc77,        //  77  0x6FAC4450
    SKILLS_CltDoFunc78,        //  78  0x6FAC4600
    SKILLS_CltDoFunc79,        //  79  0x6FAC47C0
    SKILLS_CltDoFunc80,        //  80  0x6FAC4CA0
    SKILLS_CltDoFunc81,        //  81  0x6FAC4EC0
    SKILLS_CltDoFunc82,        //  82  0x6FAC4FE0
    SKILLS_CltDoFunc83,        //  83  0x6FAC04E0
    SKILLS_CltDoFunc84,        //  84  0x6FACB070
    SKILLS_CltDoFunc85,        //  85  0x6FABA5E0
    SKILLS_CltDoFunc86,        //  86  0x6FAC9350
    SKILLS_CltDoFunc87,        //  87  0x6FACB3C0
    SKILLS_CltDoFunc88,        //  88  0x6FAC4870
    SKILLS_CltDoFunc89,        //  89  0x6FAC4890
    SKILLS_CltDoFunc90,        //  90  0x6FAC2C90
    SKILLS_CltDoFunc91,        //  91  0x6FAC3200
    SKILLS_CltDoFunc92,        //  92  0x6FAC3010
    SKILLS_CltDoFunc93,        //  93  0x6FAC4BE0
    SKILLS_CltDoFunc94,        //  94  0x6FAC79F0
    SKILLS_CltDoFunc95,        //  95  0x6FAC7B10
    SKILLS_CltDoFunc96,        //  96  0x6FACA9C0
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, //  97..104
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 105..112
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 113..120
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 121..128
    NULL,                                    // 129
};
static_assert(sizeof(gpSkillCltStFuncs) / sizeof(gpSkillCltStFuncs[0]) == 70
                  && sizeof(gpSkillCltDoFuncs) / sizeof(gpSkillCltDoFuncs[0]) == 130,
              "the two tables: 280 bytes at 0x6FB77488, then gnSkillCltStFuncCount (70) at 0x6FB775A0; "
              "520 bytes at 0x6FB775A8, then gnSkillCltDoFuncCount (130) at 0x6FB777B0");

//  D2Client 0x6FB777B4
//  The states.txt clteventfunc dispatch table, 2 entries: [0] is null, [1] is SKILLS_StatesCltEventFunc1. SKILLS_StatesAddCltEventFunc, its only
//  user, bounds the index with a `<= 1` test. It sits right after gnSkillCltDoFuncCount (0x6FB777B0), which is the count of the 130-entry table
//  before it, not of this one. In stock 1.10 exactly one states.txt row reaches it: state 20 chillingarmor.
inline int32_t (__fastcall *gpStateCltEventFuncs[2])(int32_t, D2UnitStrc *, D2UnitStrc *, int32_t, D2C_Skills, int32_t) = { NULL, SKILLS_StatesCltEventFunc1 };
static_assert(sizeof(gpStateCltEventFuncs) / sizeof(gpStateCltEventFuncs[0]) == 2,
              "gpStateCltEventFuncs is 8 bytes at 0x6FB777B4, bounded by `cmp eax, 2` / `jge` (0x6FABE268); "
              "Skills.cpp's path string follows at 0x6FB777BC");

//  D2Client 0x6FB6E2E0
//  X half of a precomputed 64-step circle of radius 30: entry i is trunc(30 * cos(2*pi*i/64)); the values are truncated, not rounded (index 63
//  is 29). Paired with gnRingOffsetY and read only by SKILLS_SpawnMissileRing, which uses the pair as the per-missile offset around the ring's
//  centre.
inline const int32_t gnRingOffsetX[64] =
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
static_assert(sizeof(gnRingOffsetX) == 256, "gnRingOffsetX: the item's 256 bytes in the .i64");

//  D2Client 0x6FB6E3E0
//  Y half of the circle: entry i is trunc(30 * sin(2*pi*i/64)). See gnRingOffsetX.
inline const int32_t gnRingOffsetY[64] =
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
static_assert(sizeof(gnRingOffsetY) == 256, "gnRingOffsetY: the item's 256 bytes in the .i64");

//  D2Client 0x6FB6E4E0
//  Quadrant sign pairs for SKILLS_SpawnMissileArray's X offset: -1, 1, 1, -1. With gnMissileArrayQuadrantY (-1, -1, 1, 1) the four entries are
//  the quadrants in order (-,-), (+,-), (+,+), (-,+). The inner loop multiplies a ring magnitude by these, so one ring pair covers eight
//  directions: four quadrants times the two mirror orders (A*qx, B*qy) and (B*qx, A*qy). The products are stored as the missile's nTargetX and
//  nTargetY (offsets, not velocities).
inline const int32_t gnMissileArrayQuadrantX[4] = { -1, 1, 1, -1 };
static_assert(sizeof(gnMissileArrayQuadrantX) == 16, "gnMissileArrayQuadrantX: the item's 16 bytes in the .i64");

//  D2Client 0x6FB6E4F0
//  Quadrant sign pairs for SKILLS_SpawnMissileArray's Y offset: -1, -1, 1, 1. The twin of gnMissileArrayQuadrantX; the products go to nTargetY.
inline const int32_t gnMissileArrayQuadrantY[4] = { -1, -1, 1, 1 };
static_assert(sizeof(gnMissileArrayQuadrantY) == 16, "gnMissileArrayQuadrantY: the item's 16 bytes in the .i64");

//  D2Client 0x6FB6E500
//  First magnitude of each of SKILLS_SpawnMissileArray's seven ring directions: 18, 20, 17, 20, 15, 19, 18. Paired with gnMissileArrayRingB
//  (8, 2, 11, 4, 13, 6, 9), the pairs (18,8), (20,2), (17,11), (20,4), (15,13), (19,6), (18,9) are all of norm about 20, points on a circle.
//  Seven entries (the assert's MISSILEARRAY_TABLE_LENGTH is 7).
inline const int32_t gnMissileArrayRingA[7] = { 18, 20, 17, 20, 15, 19, 18 };
static_assert(sizeof(gnMissileArrayRingA) == 28, "gnMissileArrayRingA: the item's 28 bytes in the .i64");

//  D2Client 0x6FB6E51C
//  Second magnitude of each of the seven ring directions: 8, 2, 11, 4, 13, 6, 9. Used both ways round: the loop spawns (A*qx, B*qy) and then
//  (B*qx, A*qy), which is how one pair covers eight of the 64 directions. See gnMissileArrayRingA.
inline const int32_t gnMissileArrayRingB[7] = { 8, 2, 11, 4, 13, 6, 9 };
static_assert(sizeof(gnMissileArrayRingB) == 28, "gnMissileArrayRingB: the item's 28 bytes in the .i64");

//  D2Client 0x6FB6E538
//  X offset of the eight axis and diagonal directions SKILLS_SpawnMissileArray uses, one per ring iteration: 20, -20, 0, 0, 14, -14, -14, 14.
//  With gnMissileArrayAxisY (0, 0, 20, -20, 14, 14, -14, -14) these are (+-20, 0), (0, +-20) and the four (+-14, +-14) diagonals, all of norm
//  about 20. Stored into nTargetX. The table has eight entries although the loop reads seven (its bound is `cmp ebx, 1Ch`), so the last is never
//  read.
inline const int32_t gnMissileArrayAxisX[8] = { 20, -20, 0, 0, 14, -14, -14, 14 };
static_assert(sizeof(gnMissileArrayAxisX) == 32, "gnMissileArrayAxisX: the item's 32 bytes in the .i64");

//  D2Client 0x6FB6E558
//  Y offset of the eight axis and diagonal directions, stored into nTargetY. The twin of gnMissileArrayAxisX.
inline const int32_t gnMissileArrayAxisY[8] = { 0, 0, 20, -20, 14, 14, -14, -14 };
static_assert(sizeof(gnMissileArrayAxisY) == 32, "gnMissileArrayAxisY: the item's 32 bytes in the .i64");

//  D2Client 0x6FB775A0
//  Number of entries in gpSkillCltStFuncs; every wCltStFunc is bounds-checked against it.
inline int32_t gnSkillCltStFuncCount = 70;
static_assert(sizeof(gnSkillCltStFuncCount) == 4, "gnSkillCltStFuncCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FB777B0
//  Number of entries in gpSkillCltDoFuncs.
inline int32_t gnSkillCltDoFuncCount = 130;
static_assert(sizeof(gnSkillCltDoFuncCount) == 4, "gnSkillCltDoFuncCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF98C
//  The client frame before which SKILLS_CltDoFunc85 does nothing: a rate limit, not a counter. The handler compares GAME_GetFrameCount() against
//  this (unsigned, strict) and on passing sets it to frame + 5 before spawning, so the effect fires at most once every six client frames: after a
//  spawn at frame F the next can come at F + 6. Slot 85 is the states.txt cltactivefunc of row 62 openwounds.
inline uint32_t gnOpenWoundsNextFrame;
static_assert(sizeof(gnOpenWoundsNextFrame) == 4, "gnOpenWoundsNextFrame: the item's 4 bytes in the .i64");

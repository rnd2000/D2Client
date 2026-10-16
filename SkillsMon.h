//  SkillsMon.h -- the data tables of SkillsMon.cpp, ten inline .rdata arrays read by SkillsMon.cpp alone (0x6FB6EA80..0x6FB6EDC0, but for one
//  unreferenced zero dword at 0x6FB6EBBC between gnDiabLightVelJitter and gnArcaneTowerCircleX): the direction codes and sector lookups of
//  Andariel's spray and the gargoyle trap, two velocity-jitter tables, the border of a 5x5 square and the radius-30 circle of the Arcane Tower.
//  Each is a C++17 inline variable, so every file that includes this header shares one object.
//  Include it after DLLmain.h and D2Client.h, as every file of ours does.
#pragma once

//  D2Client 0x6FB6EA80
//  8 records of 9 bytes, indexed 9 * direction + step by SKILLS_CltDoFunc48: D2Common #11055 codes, 99 (a sentinel) in column 4.
inline const uint8_t gnCltEffect48DirMatrix[72] =
{
    27,   // [0]
    14,   // [1]
    15,   // [2]
    3,   // [3]
    99,   // [4]
    7,   // [5]
    21,   // [6]
    22,   // [7]
    31,   // [8]
    26,   // [9]
    12,   // [10]
    13,   // [11]
    2,   // [12]
    99,   // [13]
    6,   // [14]
    19,   // [15]
    20,   // [16]
    30,   // [17]
    25,   // [18]
    10,   // [19]
    11,   // [20]
    1,   // [21]
    99,   // [22]
    5,   // [23]
    17,   // [24]
    18,   // [25]
    29,   // [26]
    24,   // [27]
    8,   // [28]
    9,   // [29]
    0,   // [30]
    99,   // [31]
    4,   // [32]
    15,   // [33]
    16,   // [34]
    28,   // [35]
    31,   // [36]
    22,   // [37]
    23,   // [38]
    7,   // [39]
    99,   // [40]
    3,   // [41]
    13,   // [42]
    14,   // [43]
    27,   // [44]
    30,   // [45]
    20,   // [46]
    7,   // [47]
    6,   // [48]
    99,   // [49]
    2,   // [50]
    1,   // [51]
    12,   // [52]
    26,   // [53]
    29,   // [54]
    18,   // [55]
    19,   // [56]
    5,   // [57]
    99,   // [58]
    1,   // [59]
    9,   // [60]
    10,   // [61]
    25,   // [62]
    28,   // [63]
    16,   // [64]
    17,   // [65]
    4,   // [66]
    99,   // [67]
    0,   // [68]
    23,   // [69]
    8,   // [70]
    24,   // [71]
};
static_assert(sizeof(gnCltEffect48DirMatrix) == 72, "gnCltEffect48DirMatrix: the item's 72 bytes in the .i64");

//  D2Client 0x6FB6EAC8
//  Eight #11055 codes (the steps three subtiles out) that SKILLS_CltDoFunc48 indexes by direction.
inline const uint8_t gnCltEffect48Dirs[8] = { 29, 28, 27, 26, 25, 24, 31, 30 };
static_assert(sizeof(gnCltEffect48Dirs) == 8, "gnCltEffect48Dirs: the item's 8 bytes in the .i64");

//  D2Client 0x6FB6EAD0
//  Collapses a 64-step facing into one of four sectors; indexed by (direction & 0x3F) in SKILLS_CltDoFunc51.
inline const uint8_t gnDir64ToSector4[64] =
{
    0,   // [0]
    0,   // [1]
    0,   // [2]
    0,   // [3]
    0,   // [4]
    0,   // [5]
    0,   // [6]
    0,   // [7]
    0,   // [8]
    1,   // [9]
    1,   // [10]
    1,   // [11]
    1,   // [12]
    1,   // [13]
    1,   // [14]
    1,   // [15]
    1,   // [16]
    1,   // [17]
    1,   // [18]
    1,   // [19]
    1,   // [20]
    1,   // [21]
    1,   // [22]
    1,   // [23]
    1,   // [24]
    2,   // [25]
    2,   // [26]
    2,   // [27]
    2,   // [28]
    2,   // [29]
    2,   // [30]
    2,   // [31]
    2,   // [32]
    2,   // [33]
    2,   // [34]
    2,   // [35]
    2,   // [36]
    2,   // [37]
    2,   // [38]
    2,   // [39]
    2,   // [40]
    3,   // [41]
    3,   // [42]
    3,   // [43]
    3,   // [44]
    3,   // [45]
    3,   // [46]
    3,   // [47]
    3,   // [48]
    3,   // [49]
    3,   // [50]
    3,   // [51]
    3,   // [52]
    3,   // [53]
    3,   // [54]
    3,   // [55]
    3,   // [56]
};
static_assert(sizeof(gnDir64ToSector4) == 64, "gnDir64ToSector4: the item's 64 bytes in the .i64");

//  D2Client 0x6FB6EB10
//  The facing SKILLS_CltDoFunc51 snaps to for each sector of gnDir64ToSector4 (the initializer leaves the fourth, 0, to the zero fill).
inline const uint8_t gnSector4Direction[4] = { 49, 31, 17 };
static_assert(sizeof(gnSector4Direction) == 4, "gnSector4Direction: the item's 4 bytes in the .i64");

//  D2Client 0x6FB6EB14
//  Velocity jitter added to a missile's missiles.txt vel by SKILLS_MissileCltSpawnWithSkillRange (seed % 5); passed by SKILLS_CltDoFunc54.
inline const int32_t gMissileVelJitter[5] = { -2, -1, 0, 1, 2 };
static_assert(sizeof(gMissileVelJitter) == 20, "gMissileVelJitter: the item's 20 bytes in the .i64");

//  D2Client 0x6FB6EB28
//  X of sixteen directions on the border of a 5x5 square (radius 2), read by SKILLS_CltDoFunc55; with gnDir16SquareY.
inline const int32_t gnDir16SquareX[16] = { 0, 1, 2, 2, 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2, -1 };
static_assert(sizeof(gnDir16SquareX) == 64, "gnDir16SquareX: the item's 64 bytes in the .i64");

//  D2Client 0x6FB6EB68
//  Y of the sixteen square-border directions; the twin of gnDir16SquareX.
inline const int32_t gnDir16SquareY[16] = { 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2, -1, 0, 1, 2, 2 };
static_assert(sizeof(gnDir16SquareY) == 64, "gnDir16SquareY: the item's 64 bytes in the .i64");

//  D2Client 0x6FB6EBA8
//  Velocity jitter passed by SKILLS_CltDoFunc56 (every entry <= 0, so it only slows the missile); the fifth, 0, is left to the zero fill.
inline const int32_t gnDiabLightVelJitter[5] = { -1, -3, -2, -1 };
static_assert(sizeof(gnDiabLightVelJitter) == 20, "gnDiabLightVelJitter: the item's 20 bytes in the .i64");

//  D2Client 0x6FB6EBC0
//  X of the radius-30, 64-direction circle, private to SKILLS_CltDoFunc61 (ArcaneTower); the same values as gnDir64CircleX.
inline const int32_t gnArcaneTowerCircleX[64] =
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
static_assert(sizeof(gnArcaneTowerCircleX) == 256, "gnArcaneTowerCircleX: the item's 256 bytes in the .i64");

//  D2Client 0x6FB6ECC0
//  Y of ArcaneTower's circle; the twin of gnArcaneTowerCircleX.
inline const int32_t gnArcaneTowerCircleY[64] =
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
static_assert(sizeof(gnArcaneTowerCircleY) == 256, "gnArcaneTowerCircleY: the item's 256 bytes in the .i64");

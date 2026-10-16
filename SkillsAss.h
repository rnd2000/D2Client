//  SkillsAss.h -- the globals of SkillsAss.cpp's data, read by SkillsAss.cpp alone: the two 64-point circles of radius 30 (gnDir64CircleX/Y), the
//  eight-direction steps (gnDir8StepX/Y), Royal Strike's own copy of the circle (gnRoyalStrikeCircleX/Y) and the two constants of the wavering path
//  (gnWaverFlipFrames, gnWaverBaseTurn). Address ranges 0x6FB6E578..0x6FB6E9BF (.rdata, const) and 0x6FB778BC..0x6FB778C3 (.data). Each is a C++17
//  inline variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above a global gives its address in
//  the original. The three circles hold the same values, value for value, as three objects, as in the image.
//  DIFFERENCE: SKILLS_CltDoFunc11 and SKILLS_CltDoFunc83 never leave their loop on a step of 0 or less (a latent defect of the original). A step of
//  0 reads entry 0 for ever, as before; a negative one walked back through the .rdata below the original's table and would read below ours, outside
//  the array, which C++ leaves undefined. Declared, not reproduced; SkillsAss.cpp says so beside the reads.
//  Include it after DLLmain.h and D2Client.h.
#pragma once

//  D2Client 0x6FB6E578
//  The X of 64 points on a circle of radius 30, one per 1/64 turn, in the same E, SE, S, SW, W, NW, N, NE rotation as gnDir8StepX. Entry 0 is
//  (30, 0) and entry 16 is (0, 30). Paired with gnDir64CircleY, which begins exactly where this ends. SKILLS_CltDoFunc11 (Claws of Thunder's
//  cltprgfunc3) walks it in strides to lay out its nova ring.
inline const int32_t gnDir64CircleX[64] =
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
static_assert(sizeof(gnDir64CircleX) == 256, "gnDir64CircleX: the item's 256 bytes in the .i64");

//  D2Client 0x6FB6E678
//  The Y half of the pair, radius 30 over 64 directions; entry 0 is 0 and entry 16 is 30. See gnDir64CircleX.
inline const int32_t gnDir64CircleY[64] =
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
static_assert(sizeof(gnDir64CircleY) == 256, "gnDir64CircleY: the item's 256 bytes in the .i64");

//  D2Client 0x6FB6E77C
//  The X step for each of the eight compass directions, at 2 subtiles per step: {2, 2, 0, -2, -2, -2, 0, 2}. Paired with gnDir8StepY. The eight
//  entries are E, SE, S, SW, W, NW, N, NE. Indexed by D2Common's #11053(dir & 0x3F), which folds a 64-direction into one of eight, so the pair is
//  how a fine direction becomes a coarse step.
inline const int32_t gnDir8StepX[8] = { 2, 2, 0, -2, -2, -2, 0, 2 };
static_assert(sizeof(gnDir8StepX) == 32, "gnDir8StepX: the item's 32 bytes in the .i64");

//  D2Client 0x6FB6E79C
//  The Y half of the pair: {0, 2, 2, 2, 0, -2, -2, -2}, in the same order and at 2 subtiles a step; see gnDir8StepX. The count is eight because
//  D2Common #11053 answers 0..7.
inline const int32_t gnDir8StepY[8] = { 0, 2, 2, 2, 0, -2, -2, -2 };
static_assert(sizeof(gnDir8StepY) == 32, "gnDir8StepY: the item's 32 bytes in the .i64");

//  D2Client 0x6FB6E7C0
//  X of 64 points on a circle of radius 30, one per 1/64 turn, with the same values as gnDir64CircleX: a private copy belonging to
//  SKILLS_CltDoFunc83 (Royal Strike's cltprgfunc2, Skills.txt Id 280, SKILL_ROYALSTRIKE). The handler walks it a step's number of entries at a time
//  (Royal Strike's step is prgcalc2, par6, 10 in stock: seven missiles, entries 0, 10 ... 60), seeding each missile with
//  SKILLS_MissileBuildWaveringPath, which builds its zig-zag path. Paired with gnRoyalStrikeCircleY, which begins exactly where this ends.
inline const int32_t gnRoyalStrikeCircleX[64] =
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
static_assert(sizeof(gnRoyalStrikeCircleX) == 256, "gnRoyalStrikeCircleX: the item's 256 bytes in the .i64");

//  D2Client 0x6FB6E8C0
//  Y of the same circle, with the same values as gnDir64CircleY. The twin of gnRoyalStrikeCircleX.
inline const int32_t gnRoyalStrikeCircleY[64] =
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
static_assert(sizeof(gnRoyalStrikeCircleY) == 256, "gnRoyalStrikeCircleY: the item's 256 bytes in the .i64");

//  D2Client 0x6FB778BC
//  15: SKILLS_MissileBuildWaveringPath flips the turn's sign and re-rolls the turn rate after each point whose index divides by it (the `idiv` at
//  0x6FAC087E), point 0 included, so the first swing is one point long and every later one 15. One reference, in that loop.
inline int32_t gnWaverFlipFrames = 15;
static_assert(sizeof(gnWaverFlipFrames) == 4, "gnWaverFlipFrames: the item's 4 bytes in the .i64");

//  D2Client 0x6FB778C0
//  2: the base turn per point of a wavering missile, in 64ths of a circle. The actual turn is `gnWaverBaseTurn + rand % 3`, so 2, 3 or 4, re-rolled
//  at each flip. Of its two references one is before the loop, for the first turn (0x6FAC0811), the other inside it, for the turn rolled again at
//  each flip (0x6FAC08B2).
inline int32_t gnWaverBaseTurn = 2;
static_assert(sizeof(gnWaverBaseTurn) == 4, "gnWaverBaseTurn: the item's 4 bytes in the .i64");

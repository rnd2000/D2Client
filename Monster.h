//  Monster.h -- the globals of Monster.cpp that are ours: the catapult missile ids, the blood spatter offsets, the client action table, the two
//  mode-description tables, the player slots and Kashya's unit. Address range 0x6FB70BA8..0x6FBBC22F.
//  Each is a C++17 `inline` variable, one object for every file that includes this header. The first five (0x6FB70BA8..0x6FB70CEF) lie in
//  .rdata and are `const`; the two mode tables are in .data; the last two lie past .data's raw data (it ends at 0x6FB9B000) and are zero at load.
//  Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB70BA8
//  The Missiles.txt ids the Act 5 catapults fire, by the Id column and not the row index: 406 catapultchargedball on, 410 catapult spike ball on,
//  415 catapult cold ball on, 418 catapult plague ball on, 421 catapult meteor ball on. Read by MONSTER_LinkOwnedUnits,
//  MONSTER_SpawnAttackMissile and MONSTER_DoClientSplEndEffect. The count 5 follows it (scnCatapultMissiles).
inline const int32_t gnCatapultMissiles[5] = { 406, 410, 415, 418, 421 };
static_assert(sizeof(gnCatapultMissiles) == 20, "gnCatapultMissiles: the item's 20 bytes in the .i64");

//  D2Client 0x6FB70BBC
//  The entry count of gnCatapultMissiles, right after the array. The name is the original's own, from its assert
//  `nMissileIndex < scnCatapultMissiles`; the `sc` prefix is Blizzard's for a static const.
inline const int32_t scnCatapultMissiles = 5;
static_assert(sizeof(scnCatapultMissiles) == 4, "scnCatapultMissiles: the item's 4 bytes in the .i64");

//  D2Client 0x6FB70BC0
//  X offsets for the eight blood spatters MONSTER_SpawnBlood drops around a hit unit, added to PATH_GetXPosition. Paired with
//  gnBloodSpawnOffsetY (2, 1, 0, -1, -2, -1, 0, 1): the four axis points are 2 away and the four diagonals (1, 1).
inline const int32_t gnBloodSpawnOffsetX[8] = { 0, -1, -2, -1, 0, 1, 2, 1 };
static_assert(sizeof(gnBloodSpawnOffsetX) == 32, "gnBloodSpawnOffsetX: the item's 32 bytes in the .i64");

//  D2Client 0x6FB70BE0
//  Y offsets for MONSTER_SpawnBlood's eight blood spatters, added to PATH_GetYPosition. The twin of gnBloodSpawnOffsetX.
inline const int32_t gnBloodSpawnOffsetY[8] = { 2, 1, 0, -1, -2, -1, 0, 1 };
static_assert(sizeof(gnBloodSpawnOffsetY) == 32, "gnBloodSpawnOffsetY: the item's 32 bytes in the .i64");

//  D2Client 0x6FB70C00
//  What animation mode each client action puts a monster into: 30 records of two dwords, indexed by nAction. Field +0 is the D2C_MonModes
//  passed to UNITMODE_SetAnimMode; field +4 non-zero means face the target first (1 for the attack, skill and cast modes, 0 for action 25).
//  Modes by action id:
//       0,1 WALK   2,3 16     4,5 CAST   6 GETHIT   7 NEUTRAL   8 DEATH
//       9 DEAD    10,11 ATTACK1  12,13 SKILL1  14,15 SKILL2  16,17 ATTACK2
//      18 BLOCK   19 16     20 KNOCKBACK  21,22 SEQUENCE  23,24 RUN
//      25 SKILL1  26,27 SKILL3  28,29 SKILL4
//  16 is a sentinel, not a mode: D2C_MonModes stops at MONMODE_RUN = 15, and the code guards on it (`if (mode < 16)`) before testing the
//  monstats2 bit as `gdwBitMasks[mode & 7] & nModeEnabledWhenMovingFlags[mode >> 3]`. So 16 means no mode of its own, for actions 2, 3 and 19.
inline const int32_t gMonActionModeTable[60] =
{
    2,   // [0]
    0,   // [1]
    2,   // [2]
    0,   // [3]
    16,   // [4]
    0,   // [5]
    16,   // [6]
    0,   // [7]
    7,   // [8]
    1,   // [9]
    7,   // [10]
    1,   // [11]
    3,   // [12]
    0,   // [13]
    1,   // [14]
    0,   // [15]
    0,   // [16]
    0,   // [17]
    12,   // [18]
    0,   // [19]
    4,   // [20]
    1,   // [21]
    4,   // [22]
    1,   // [23]
    8,   // [24]
    1,   // [25]
    8,   // [26]
    1,   // [27]
    9,   // [28]
    1,   // [29]
    9,   // [30]
    1,   // [31]
    5,   // [32]
    1,   // [33]
    5,   // [34]
    1,   // [35]
    6,   // [36]
    0,   // [37]
    16,   // [38]
    0,   // [39]
    13,   // [40]
    0,   // [41]
    14,   // [42]
    0,   // [43]
    14,   // [44]
    0,   // [45]
    15,   // [46]
    0,   // [47]
    15,   // [48]
    0,   // [49]
    8,   // [50]
    0,   // [51]
    10,   // [52]
    1,   // [53]
    10,   // [54]
    1,   // [55]
    11,   // [56]
    1,   // [57]
    11,   // [58]
    1,   // [59]
};
static_assert(sizeof(gMonActionModeTable) == 240, "gMonActionModeTable: the item's 240 bytes in the .i64");

//  D2Client 0x6FB91590
//  The client's per-mode animation policy for monsters: 16 records indexed by anim mode (read by MONSTER_UpdateAnimMode, acted on by
//  MONSTER_AdvanceAnimForMode). Each is { nPathStep, nAnimEnd, nSplEndTrigger }: whether the mode steps the unit along its path (1 = yes), how
//  the animation ends (0 loop, 1 end-of-cycle, 2 freeze on the last frame, 3 rewind and freeze), and what triggers
//  MONSTER_DoClientSplEndEffect (1 a path step, 2/3 the anim end, 4 both).
inline D2ClientMonModeDescStrc gMonModeDescs[16] =
{
    { .nAnimEnd = 2 },   // [0]
    {},   // [1]
    { .nPathStep = 1, .nSplEndTrigger = 1 },   // [2]
    { .nAnimEnd = 1, .nSplEndTrigger = 2 },   // [3]
    { .nPathStep = 2, .nAnimEnd = 1, .nSplEndTrigger = 3 },   // [4]
    { .nPathStep = 2, .nAnimEnd = 1, .nSplEndTrigger = 3 },   // [5]
    { .nAnimEnd = 1, .nSplEndTrigger = 2 },   // [6]
    { .nAnimEnd = 1, .nSplEndTrigger = 2 },   // [7]
    { .nAnimEnd = 1, .nSplEndTrigger = 2 },   // [8]
    { .nAnimEnd = 1, .nSplEndTrigger = 2 },   // [9]
    { .nAnimEnd = 2 },   // [10]
    {},   // [11]
    { .nAnimEnd = 2 },   // [12]
    { .nPathStep = 1, .nAnimEnd = 2, .nSplEndTrigger = 2 },   // [13]
    { .nPathStep = 2, .nAnimEnd = 1, .nSplEndTrigger = 2 },   // [14]
    { .nPathStep = 1, .nSplEndTrigger = 1 },   // [15]
};
static_assert(sizeof(gMonModeDescs) == 192, "gMonModeDescs: the item's 192 bytes in the .i64");

//  D2Client 0x6FB91650
//  The second D2ClientMonModeDescStrc[16], right after gMonModeDescs and the same shape. MONSTER_UpdateAnimMode picks this one when monstats2's
//  `nModeEnabledWhenMovingFlags` has the current mode's bit set: a monster whose animation for that mode may move.
//  Two rows are borrowed by name: entry 6 (BLOCK) by diablo and diabloclone in mode 11, entry 7 (CAST) by baalthrone, baalcrab and baalclone
//  in mode 10.
inline D2ClientMonModeDescStrc gMonModeDescsMoving[16] =
{
    { .nPathStep = 1, .nAnimEnd = 2 },   // [0]
    { .nAnimEnd = 1, .nSplEndTrigger = 2 },   // [1]
    { .nAnimEnd = 1, .nSplEndTrigger = 2 },   // [2]
    { .nAnimEnd = 1, .nSplEndTrigger = 2 },   // [3]
    { .nPathStep = 1, .nSplEndTrigger = 4 },   // [4]
    { .nPathStep = 1, .nSplEndTrigger = 4 },   // [5]
    { .nPathStep = 2, .nAnimEnd = 1, .nSplEndTrigger = 3 },   // [6]
    { .nPathStep = 1, .nSplEndTrigger = 4 },   // [7]
    { .nPathStep = 1, .nSplEndTrigger = 4 },   // [8]
    { .nPathStep = 1, .nSplEndTrigger = 4 },   // [9]
    { .nAnimEnd = 2 },   // [10]
    { .nPathStep = 1, .nAnimEnd = 2 },   // [11]
    { .nAnimEnd = 1, .nSplEndTrigger = 2 },   // [12]
    { .nAnimEnd = 1, .nSplEndTrigger = 2 },   // [13]
    { .nAnimEnd = 1, .nSplEndTrigger = 2 },   // [14]
    { .nAnimEnd = 1, .nSplEndTrigger = 2 },   // [15]
};
static_assert(sizeof(gMonModeDescsMoving) == 192, "gMonModeDescsMoving: the item's 192 bytes in the .i64");

//  D2Client 0x6FBBC20C
//  The eight player units the client knows about (the party size), maintained by MONSTER_RegisterPlayerSlot, MONSTER_UnregisterPlayerSlot and
//  MONSTER_ClearPlayerSlots; a slot is filled by PLAYER_InitClientUnit and emptied by PLAYER_FreeClientUnit. MONSTER_CritterTickAmbient scans
//  it for the nearest unit with a live anim mode and hands it to each critter's tick, so critters flee from the nearest player;
//  MONSTER_CritterTickBird reads CUNIT_GetPlayer() directly instead.
inline D2UnitStrc *gapClientPlayers[8];
static_assert(sizeof(gapClientPlayers) == 32, "gapClientPlayers: the item's 32 bytes in the .i64");

//  D2Client 0x6FBBC22C
//  The client's pointer to Kashya (monstats class 150). Written in exactly one place, MONSTER_LinkOwnedUnits' jump table, case 150, with the
//  unit being linked; nothing in the module reads it.
inline D2UnitStrc *gpKashyaUnit;
static_assert(sizeof(gpKashyaUnit) == 4, "gpKashyaUnit: the item's 4 bytes in the .i64");

//  Player.h -- the globals of Player.cpp that are ours: the anim-mode control table, the skill-use refusal sounds and their count, and the town
//  flag and the click throttle's state. Each is a C++17 inline variable, one object for every file that includes this header.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. The globals at 0x6FBBC234..0x6FBBC243 lie past the end
//  of .data's raw data (0x6FB9B000): the loader zero-fills them, and ours are zero-initialised.
//  Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB93CF8
//  Three dwords per anim mode, 20 modes: [0] moving/skill class, [1] how the sequence ends, [2] when the mode counts as finished. Read only by
//  PLAYER_AdvanceAnimMode.
inline int32_t gnAnimModeControl[20][3] =
{
    { 0, 2 },   // [0]
    {  },   // [1]
    { 1, 0, 1 },   // [2]
    { 1, 0, 1 },   // [3]
    { 0, 1, 2 },   // [4]
    {  },   // [5]
    { 1, 0, 1 },   // [6]
    { 2, 1, 3 },   // [7]
    { 2, 1, 3 },   // [8]
    { 0, 1, 2 },   // [9]
    { 2, 1, 3 },   // [10]
    { 2, 1, 3 },   // [11]
    { 2, 1, 3 },   // [12]
    { 0, 1, 2 },   // [13]
    { 2, 1, 3 },   // [14]
    { 2, 1, 3 },   // [15]
    { 2, 1, 3 },   // [16]
    { 0, 2 },   // [17]
    { 2, 1, 3 },   // [18]
    { 1, 3, 1 },   // [19]
};
static_assert(sizeof(gnAnimModeControl) == 240, "gnAnimModeControl: the item's 240 bytes in the .i64");

//  D2Client 0x6FB93DE8
//  The sound played when a skill cannot be used, one int per D2C_SkillUseStates (9 of them), read by PLAYER_CanUseSkill; 0 means silent.
//  NOMANA(1) = 21, NOQUANTITY(2), DISABLED(3) and SHAPERESTRICT(4) = 19, everything else 0. 19 and 21 are sound event ids for
//  UNITSND_PlayPlayerSoundEvent (the enum the server sends in packet 0x2C), not sounds.txt rows.
inline int32_t sgSnds[9] = { 0, 21, 19, 19, 19 };
static_assert(sizeof(sgSnds) == 36, "sgSnds: the item's 36 bytes in the .i64");

//  D2Client 0x6FB93E0C
//  The entry count of sgSnds, which PLAYER_CanUseSkill checks before indexing.
inline int32_t sgNumSnds = 9;
static_assert(sizeof(sgNumSnds) == 4, "sgNumSnds: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBC234
//  Whether the room a player unit was last initialised in is a town room (DUNGEON_IsRoomInTown). PLAYER_InitClientUnit writes it for every player
//  unit the client builds; PLAYER_AdvanceAnimMode reads and writes it for the local player on a frame its room has changed, normalised to 0 or 1.
inline int32_t gbLastRoomInTown;
static_assert(sizeof(gbLastRoomInTown) == 4, "gbLastRoomInTown: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBC238
//  GAME_GetFrameCount() at the last click command PLAYER_ClampClickTargetAndThrottle let through; a repeat is dropped while fewer than 7 frames
//  have passed. The throttle applies only to a PLAYER unit whose command carries bit 0x08 and whose anim mode is 2, 3 or 6; every other path
//  stamps the frame and allows the command.
inline uint32_t gnLastClickRepeatFrame;
static_assert(sizeof(gnLastClickRepeatFrame) == 4, "gnLastClickRepeatFrame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBC23C
//  Set by PLAYER_OnMouseClick when it acts on a click and cleared by PLAYER_ResetClickGuard: a held mouse button issues one order rather than
//  one per frame. A click at exactly (0, 0) bypasses it.
inline BOOL gbClickConsumed;
static_assert(sizeof(gbClickConsumed) == 4, "gbClickConsumed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBC240
//  PLAYER_ShouldHandleClick's phase across calls: 0 idle, 1 command with no target, 2 command with a target. Set to 0 by the cancel bit and to
//  1 or 2 by the new-command bit; the repeat bit only reads it. Only that function touches it.
inline int32_t gnClickPressPhase;
static_assert(sizeof(gnClickPressPhase) == 4, "gnClickPressPhase: the item's 4 bytes in the .i64");

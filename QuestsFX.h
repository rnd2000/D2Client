//  QuestsFX.h -- the globals of QuestsFX.cpp: the quest trigger handler table gQuestTriggerFuncs, the constant gfConst80, the counters of the Den of
//  Evil pulse and the Diablo tint, the watched unit, and the quest screen-fade, game-end and Worldstone-shake state (0x6FBAF948..0x6FBAF98C).
//  Each is a C++17 inline variable, so every file that includes this header shares one object.
//  The original clears the 11 dwords from gbQuestFxFadeActive with one `rep stosd`; ours are separate objects, so QuestsFX.cpp zeroes the thirteen
//  of them by name (QUESTSFX_ClearFadeState).
//  Include it after D2Client.h, which declares every function named here.
#pragma once

//  D2Client 0x6FB773B8
//  The quest trigger handlers by trigger id; QUESTSFX_FireTriggerById calls the slot if it is not empty. Twenty slots, ten live and ten empty.
inline void (__cdecl *gQuestTriggerFuncs[20])() =
{
    QUESTSFX_TriggerDenOfEvil,           //   0  0x6FAB9140
    QUESTSFX_TriggerCairnStones,         //   1  0x6FAB9000
    NULL,                               //   2
    QUESTSFX_TriggerHoradricStaff,       //   3  0x6FAB9150
    NULL,                               //   4
    NULL,                               //   5
    QUESTSFX_TriggerTaintedSun,          //   6  0x6FAB9280
    NULL,                               //   7
    NULL,                               //   8
    NULL,                               //   9
    NULL,                               //  10
    NULL,                               //  11
    QUESTSFX_TriggerDiabloAppears,       //  12  0x6FAB9330
    QUESTSFX_ArmTriggerTimer,            //  13  0x6FAB94D0
    QUESTSFX_TriggerHellforge,           //  14  0x6FAB93E0
    NULL,                               //  15
    QUESTSFX_TriggerAnya,                //  16  0x6FAB9570
    QUESTSFX_TriggerNihlathak,           //  17  0x6FAB9620
    NULL,                               //  18
    QUESTSFX_TriggerWorldstoneShake,     //  19  0x6FAB96F0
};
static_assert(sizeof(gQuestTriggerFuncs) == 0x6FB77408 - 0x6FB773B8,
              "gQuestTriggerFuncs: 20 slots, 0x6FB773B8 up to the nFadeOut string");

//  D2Client 0x6FB6E2D8
//  The constant 80.0, multiplied with FOG #10083's cosine in QUESTSFX_GetQuestLightOverride.
inline const float gfConst80 = 80.0f;
static_assert(sizeof(gfConst80) == 4, "gfConst80: the item's 4 bytes in the .i64");

//  D2Client 0x6FB773A8
//  Frame counter of the Den of Evil red light pulse (level 8); -1 means idle. QUESTSFX_TriggerDenOfEvil starts it, QUESTSFX_TickScreenFade runs it.
inline int32_t gnDenOfEvilPulseFrame = -1;
static_assert(sizeof(gnDenOfEvilPulseFrame) == 4, "gnDenOfEvilPulseFrame: the item's 4 bytes in the .i64");

//  D2Client 0x6FB773AC
//  Counter that ends the act-4 red tint; -1 means idle. QUESTSFX_ArmTriggerTimer arms it to 0, QUESTSFX_TickScreenFade counts it to 30.
inline int32_t gnDiabloAppearsTintFrame = -1;
static_assert(sizeof(gnDiabloAppearsTintFrame) == 4, "gnDiabloAppearsTintFrame: the item's 4 bytes in the .i64");

//  D2Client 0x6FB773B0
//  The unit id of the monster the game-end countdown watches, or -1 for none; set by QUESTSFX_SetWatchedUnit.
inline uint32_t gdwQuestFxWatchedUnitId = 0xFFFFFFFF;
static_assert(sizeof(gdwQuestFxWatchedUnitId) == 4, "gdwQuestFxWatchedUnitId: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF948
//  First dword of the 11-dword quest screen-fade state block (0x6FBAF948..0x6FBAF970), and the fade's active flag.
inline int32_t gbQuestFxFadeActive;
static_assert(sizeof(gbQuestFxFadeActive) == 4, "gbQuestFxFadeActive: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF94C
//  The faded light intensity QUESTSFX_TickScreenFade computes each frame; what QUESTSFX_GetQuestLightOverride returns for a level with no tint.
inline int32_t gnQuestFxIntensity;
static_assert(sizeof(gnQuestFxIntensity) == 4, "gnQuestFxIntensity: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF950
//  Red of the room colour QUESTSFX_TickScreenFade last fetched (an out-parameter of DLIGHTMAP_GetRoomLightColor); Green and Blue follow.
inline uint8_t gnQuestFxLightRed;
static_assert(sizeof(gnQuestFxLightRed) == 1, "gnQuestFxLightRed: the item's 1 bytes in the .i64");

//  D2Client 0x6FBAF951
//  Green of the room colour QUESTSFX_TickScreenFade last fetched.
inline uint8_t gnQuestFxLightGreen;
static_assert(sizeof(gnQuestFxLightGreen) == 1, "gnQuestFxLightGreen: the item's 1 bytes in the .i64");

//  D2Client 0x6FBAF952
//  Blue of the room colour QUESTSFX_TickScreenFade last fetched.
inline uint8_t gnQuestFxLightBlue;
static_assert(sizeof(gnQuestFxLightBlue) == 1, "gnQuestFxLightBlue: the item's 1 bytes in the .i64");

//  D2Client 0x6FBAF954
//  Length in frames of the fade-to-black phase, the first phase of the quest screen fade (the original's name, from the nFadeOut assert).
inline int32_t gnQuestFxFadeOutFrames;
static_assert(sizeof(gnQuestFxFadeOutFrames) == 4, "gnQuestFxFadeOutFrames: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF958
//  Length in frames of the final fade-back-in phase; a zero is replaced by 25 both when stored and when used.
inline int32_t gnQuestFxFadeInFrames;
static_assert(sizeof(gnQuestFxFadeInFrames) == 4, "gnQuestFxFadeInFrames: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF95C
//  Length in frames of the dark middle phase; stored, never read (the phase ends at gnQuestFxFadeTotalFrames less gnQuestFxFadeInFrames).
inline int32_t gnQuestFxDarkHoldFrames;
static_assert(sizeof(gnQuestFxDarkHoldFrames) == 4, "gnQuestFxDarkHoldFrames: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF960
//  Total length of the quest screen fade in frames: the sum of the out, dark-hold and in lengths.
inline int32_t gnQuestFxFadeTotalFrames;
static_assert(sizeof(gnQuestFxFadeTotalFrames) == 4, "gnQuestFxFadeTotalFrames: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF964
//  Frame counter of the quest screen fade, incremented once per QUESTSFX_TickScreenFade call.
inline int32_t gnQuestFxFadeFrame;
static_assert(sizeof(gnQuestFxFadeFrame) == 4, "gnQuestFxFadeFrame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF968
//  Phase index handed to FOG_10083_Cos_LUT: 0 -> 128 across the fade-out, 128 -> 0 across the fade-in (0 is full brightness, 128 black).
inline int32_t gnQuestFxFadeCosPhase;
static_assert(sizeof(gnQuestFxFadeCosPhase) == 4, "gnQuestFxFadeCosPhase: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF96C
//  The level id the quest screen fade was started on; the fade is cancelled if the player's level stops matching.
inline int32_t gnQuestFxFadeLevelId;
static_assert(sizeof(gnQuestFxFadeLevelId) == 4, "gnQuestFxFadeLevelId: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF970
//  Last dword of the fade state block; read as a "fade locked" guard by QUESTSFX_StartScreenFade, never set, so the guard always passes.
inline int32_t gbQuestFxFadeLocked;
static_assert(sizeof(gbQuestFxFadeLocked) == 4, "gbQuestFxFadeLocked: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF974
//  Bitmask of the quest triggers fired (`|= 1 << eTrigger` in QUESTSFX_FireTriggerById); nothing reads it.
inline uint32_t gdwQuestTriggersFired;
static_assert(sizeof(gdwQuestTriggersFired) == 4, "gdwQuestTriggersFired: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF978
//  Latches once the Den of Evil light effect has fired (the pulse reached 30); cleared by QUESTSFX_Reset.
inline int32_t gbDenOfEvilLightDone;
static_assert(sizeof(gbDenOfEvilLightDone) == 4, "gbDenOfEvilLightDone: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF97C
//  Set by QUESTSFX_TriggerDiabloAppears: the act-4 red tint (levels 107..108, intensity 160) is drawn while it is set.
inline int32_t gbDiabloAppearsTintActive;
static_assert(sizeof(gbDiabloAppearsTintActive) == 4, "gbDiabloAppearsTintActive: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF980
//  The sync-clock time, in seconds, at which the game ends; zero means no countdown is running.
inline int32_t gnGameEndDeadline;
static_assert(sizeof(gnGameEndDeadline) == 4, "gnGameEndDeadline: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF984
//  Set by quest trigger 19 (QUESTSFX_TriggerWorldstoneShake), cleared by QUESTSFX_Reset; allows the Worldstone shake in levels 131 and 132.
inline int32_t gbWorldstoneShakeArmed;
static_assert(sizeof(gbWorldstoneShakeArmed) == 4, "gbWorldstoneShakeArmed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF988
//  The once-per-visit latch of the Worldstone shake; set back to 0 when the player is outside levels 131..132. Not touched by QUESTSFX_Reset.
inline int32_t gbWorldstoneShakeSpawned;
static_assert(sizeof(gbWorldstoneShakeSpawned) == 4, "gbWorldstoneShakeSpawned: the item's 4 bytes in the .i64");

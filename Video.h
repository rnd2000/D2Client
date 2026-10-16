//  Video.h -- the globals of Video.cpp: gCinematics, gnCinematicCount and the five cinematic state flags (0x6FB742B8..0x6FB9A824). Each is a
//  C++17 inline variable, so every file that includes this header shares one object; the flags are zero in the file.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB742B8
//  The eight cinematics, `{ nVideoId, pszFileName, flags }`, 12 bytes each. bLocalized (+8): the file is under DATA\LOCAL\video\<language>;
//  bHasResolutionSuffix (+9): the name carries a `%s` for "640x292" or "640x146". Only `d2_promo.bik` has both clear.
inline D2CinematicEntry gCinematics[8] =
{
    { .pszFileName = "d2_promo.bik" },   // [0]
    { .nVideoId = 1, .pszFileName = "d2intro%s.bik", .bLocalized = 1, .bHasResolutionSuffix = 1 },   // [1]
    { .nVideoId = 2, .pszFileName = "ACT02START%s.BIK", .bLocalized = 1, .bHasResolutionSuffix = 1 },   // [2]
    { .nVideoId = 3, .pszFileName = "ACT03START%s.BIK", .bLocalized = 1, .bHasResolutionSuffix = 1 },   // [3]
    { .nVideoId = 4, .pszFileName = "ACT04START%s.BIK", .bLocalized = 1, .bHasResolutionSuffix = 1 },   // [4]
    { .nVideoId = 5, .pszFileName = "ACT04END%s.BIK", .bLocalized = 1, .bHasResolutionSuffix = 1 },   // [5]
    { .nVideoId = 6, .pszFileName = "D2X_INTRO_%s.BIK", .bLocalized = 1, .bHasResolutionSuffix = 1 },   // [6]
    { .nVideoId = 7, .pszFileName = "D2X_OUT_%s.BIK", .bLocalized = 1, .bHasResolutionSuffix = 1 },   // [7]
};
static_assert(sizeof(gCinematics) == 96, "gCinematics: the item's 96 bytes in the .i64");

//  D2Client 0x6FB74318
//  8, the `NumVideos` of VIDEO_PlayCinematic's `VideoId < NumVideos` assert; it sits right after gCinematics.
inline int32_t gnCinematicCount = 8;
static_assert(sizeof(gnCinematicCount) == 4, "gnCinematicCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9A820
//  The game was completed in a classic (non-expansion) game. Set by VIDEO_SetGameCompletedClassic, cleared by VIDEO_ResetCinematicState and
//  by VIDEO_PlayCinematic once it has acted on it. VIDEO_PlayCinematic reads it twice: it suppresses the per-frame MSG_SendPing6C callback,
//  and it makes the tail call GAME_LeaveGameAfterCinematic.
inline uint8_t gbGameCompletedClassic;
static_assert(sizeof(gbGameCompletedClassic) == 1, "gbGameCompletedClassic: the item's 1 bytes in the .i64");

//  D2Client 0x6FB9A821
//  Re-entrancy guard around the Smacker playback itself: raised just before D2GFX_WINDOW_OpenSmackCutscene, tested at the entry of
//  VIDEO_PlayCinematic. Narrower than gbCinematicPlaying.
inline uint8_t gbCinematicOnScreen;
static_assert(sizeof(gbCinematicOnScreen) == 1, "gbCinematicOnScreen: the item's 1 bytes in the .i64");

//  D2Client 0x6FB9A822
//  Which resolution variant of a cinematic to open: set means the `%s` in the gCinematics filename becomes "640x292", clear "640x146".
//  Written only by VIDEO_ResetCinematicState, as `FOG_10191() == 0` (Fog's CPU capability test: the half-height videos are the fallback).
inline uint8_t gbUseFullSizeCinematics;
static_assert(sizeof(gbUseFullSizeCinematics) == 1, "gbUseFullSizeCinematics: the item's 1 bytes in the .i64");

//  D2Client 0x6FB9A823
//  True for the whole of a VIDEO_PlayCinematic call, raised after the bounds assert and cleared on every exit but one: an entry while
//  gbCinematicOnScreen is set returns with it raised. Also cleared by VIDEO_ResetCinematicState; exposed through VIDEO_IsCinematicPlaying.
inline uint8_t gbCinematicPlaying;
static_assert(sizeof(gbCinematicPlaying) == 1, "gbCinematicPlaying: the item's 1 bytes in the .i64");

//  D2Client 0x6FB9A824
//  Set by VIDEO_SetActVIntroPending when Diablo falls in an expansion game (quest 23); cleared by VIDEO_ResetCinematicState and by
//  VIDEO_PlayCinematic. Read only there, where it makes the player play D2X_INTRO, record it and load act V's palette.
inline uint8_t gbActVIntroPending;
static_assert(sizeof(gbActVIntroPending) == 1, "gbActVIntroPending: the item's 1 bytes in the .i64");

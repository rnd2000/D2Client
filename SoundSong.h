//  SoundSong.h -- the globals of SoundSong.cpp's data that are ours: fourteen items at 0x6FBC1A6C..0x6FBC1AA3, the music (song, level and
//  ticks) and the two stings. They lie past the end of .data's raw data (0x6FB9B000): the loader zero-fills them, and ours are
//  zero-initialised. Each is a C++17 `inline` variable, one object for every file that includes this header. The comment line
//  "D2Client 0x6F..." above a global gives its address in the original. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FBC1A6C
//  The tick the player's level last changed, in SOUNDHDR_PumpPreload's counter; SOUNDSONG_UpdateMusic holds the song back until 75 ticks have
//  passed, but only while a song plays with the music on.
inline uint32_t gnLevelChangeTick;
static_assert(sizeof(gnLevelChangeTick) == 4, "gnLevelChangeTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1A70
//  Written with the current tick by SOUNDSONG_UpdateMusic and by SOUNDSONG_Init. Nothing reads it.
inline uint32_t gdwSoundMusicTickUnused;
static_assert(sizeof(gdwSoundMusicTickUnused) == 4, "gdwSoundMusicTickUnused: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1A74
//  The sounds.txt id of the song currently playing, 0 for none: taken from the sound environment record's nSong by SOUNDSONG_UpdateMusic, and 0
//  when that is 0 or outside [SOUNDHDR_GetEnvironLookupMin, Max]; also the index into the environ lookup, EnvironLookup[gnCurrentSongId - min],
//  that SOUNDSONG_ScheduleSting writes the song's resume state into.
inline D2C_Sounds gnCurrentSongId;
static_assert(sizeof(gnCurrentSongId) == 4, "gnCurrentSongId: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1A78
//  The level id the music was last chosen for; when SOUNDSONG_UpdateMusic sees a different one it records it and stamps gnLevelChangeTick.
inline int32_t gnSongLevelId;
static_assert(sizeof(gnSongLevelId) == 4, "gnSongLevelId: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1A7C
//  The tick the song last changed: stamped by SOUNDSONG_UpdateMusic when it takes a new song and by SOUNDSONG_ScheduleSting when it rewrites the
//  current song's environ-lookup state.
inline uint32_t gnSongChangeTick;
static_assert(sizeof(gnSongChangeTick) == 4, "gnSongChangeTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1A80
//  Set by SOUNDSONG_ScheduleSting and cleared by SOUNDSONG_UpdateMusic when the end frame passes; while it is set the background song is suppressed.
inline BOOL gbStingActive;
static_assert(sizeof(gbStingActive) == 4, "gbStingActive: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1A84
//  The sting's first sound, played with no unit at gnSting1Frame; also the id handed to SOUNDINST_StopSoundsById when the sting ends, and only
//  when the master and music volumes are both on.
inline D2C_Sounds gnSting1SoundId;
static_assert(sizeof(gnSting1SoundId) == 4, "gnSting1SoundId: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1A88
//  The frame gnSting1SoundId fires on: GAME_GetFrameCount() + nDelay1 at schedule time.
inline uint32_t gnSting1Frame;
static_assert(sizeof(gnSting1Frame) == 4, "gnSting1Frame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1A8C
//  Cleared by SOUNDSONG_UpdateMusic once gnSting1SoundId has fired, so it fires exactly once per sting.
inline BOOL gbSting1Pending;
static_assert(sizeof(gbSting1Pending) == 4, "gbSting1Pending: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1A90
//  The sting's second sound, played on the player unit and only when the player is not dead.
inline D2C_Sounds gnSting2SoundId;
static_assert(sizeof(gnSting2SoundId) == 4, "gnSting2SoundId: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1A94
//  The frame gnSting2SoundId fires on; UNITSND_PlayPlayerSoundEvent's two calls pass the delays 0 and 250, and 475 and 525.
inline uint32_t gnSting2Frame;
static_assert(sizeof(gnSting2Frame) == 4, "gnSting2Frame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1A98
//  Both the enable flag SOUNDSONG_ScheduleSting was given and the once-only latch SOUNDSONG_UpdateMusic clears; a sting with no second sound
//  has it 0.
inline BOOL gbSting2Pending;
static_assert(sizeof(gbSting2Pending) == 4, "gbSting2Pending: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1A9C
//  When the sting stops holding the music off: GAME_GetFrameCount() + nDuration from SOUNDSONG_ScheduleSting (SOUNDSONG_ScheduleSting2 writes
//  the frame plus 50 or gnSting2Frame plus 50). Reaching it clears gbStingActive, hands gnSting1SoundId to SOUNDINST_StopSoundsById and lets
//  SOUNDSONG_UpdateMusic start the song again.
inline uint32_t gnStingEndFrame;
static_assert(sizeof(gnStingEndFrame) == 4, "gnStingEndFrame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1AA0
//  The level id whose area-entry quest line has already been played: SOUNDSONG_UpdateMusic calls UNITSND_PlayAreaEntryQuestLine and stores
//  gnSongLevelId here once at least 0x3E ticks have passed since gnLevelChangeTick. Set by SOUNDSONG_Init as well.
inline int32_t gnAreaEntryQuestLineLevelId;
static_assert(sizeof(gnAreaEntryQuestLineLevelId) == 4, "gnAreaEntryQuestLineLevelId: the item's 4 bytes in the .i64");

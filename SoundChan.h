//  SoundChan.h -- the globals of SoundChan.cpp: the two float scales, the EAX environment last sent, the sixteen sound channels, the re-entry
//  guard, the system flag and the GUID mask the code-integrity probe reads. Address range 0x6FB70F50..0x6FBBD473.
//  Each is a C++17 inline variable, so every file that includes this header shares one object. The two scales are const (.rdata). The globals
//  from 0x6FBBD268 on lie past the end of .data's raw data (0x6FB9B000): the loader zero-fills them, and ours are zero-initialised.
//  The original ends the channel walks at the addresses of the three globals that follow the array (gnSoundChanReentryGuard, the end;
//  sgfSoundChannelsActive, the end + 4; gnMoveToUnitGuidOrMask, the end + 8), compared signed (jl) -- sixteen passes each. Ours are separate
//  objects, so SoundChan.cpp compares the record pointer with gaSoundChannels + 16, int-cast for the signed compare.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB70F50
//  0.001, read only by SOUNDCHAN_ApplyEaxEnvironment, which scales eight of the soundenviron.txt row's EAX columns by it.
inline const float gfEaxMilliScale = 0.0010000000474974513f;
static_assert(sizeof(gfEaxMilliScale) == 4, "gfEaxMilliScale: the item's 4 bytes in the .i64");

//  D2Client 0x6FB70F70
//  0.003125 (1/320), read only by SOUNDCHAN_ApplyVolumeAndPosition: the scale of the position coordinates sent to the driver.
inline const float gfSoundPositionScale = 0.0031250000465661287f;
static_assert(sizeof(gfSoundPositionScale) == 4, "gfSoundPositionScale: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9661C
//  The soundenviron.txt id SOUNDCHAN_ApplyEaxEnvironment last saw; the early-out that keeps the driver call from happening every frame.
//  Updated even when the driver call is skipped for want of hardware EAX.
inline int32_t gnCurrentEaxEnvironment = -1;
static_assert(sizeof(gnCurrentEaxEnvironment) == 4, "gnCurrentEaxEnvironment: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD268
//  The sound channels: 16 records of 32 bytes (hSound at +0, nKind at +4, pInstance at +8, then the tCache of what the channel last sent the driver).
inline D2SoundChannelStrc gaSoundChannels[16];
static_assert(sizeof(gaSoundChannels) == 512, "gaSoundChannels: the item's 512 bytes in the .i64");

//  D2Client 0x6FBBD468
//  A re-entry guard (InterlockedIncrement/Decrement) held across the whole of SOUNDCHAN_Toggle3dSound and SOUNDCHAN_ToggleEaxEffects.
inline LONG gnSoundChanReentryGuard;
static_assert(sizeof(gnSoundChanReentryGuard) == 4, "gnSoundChanReentryGuard: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD46C
//  Set once SOUNDCHAN_CreateChannels has built all sixteen channels, cleared on shutdown; the creating functions assert it is clear. The name is the
//  game's own, from the assert string "!sgfSoundChannelsActive".
inline int32_t sgfSoundChannelsActive;
static_assert(sizeof(sgfSoundChannelsActive) == 4, "sgfSoundChannelsActive: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD470
//  UNITMODE_StartMoveToUnit ORs every GUID it is passed into this; bit 14 (0x4000) triggers SOUNDCHAN_SendCodeIntegrityProbe, which clears the whole
//  global. Its address also ends SOUNDCHAN_StartInstance's walk of the channels (+8 per record); that loop never reads these bytes.
inline uint32_t gnMoveToUnitGuidOrMask;
static_assert(sizeof(gnMoveToUnitGuidOrMask) == 4, "gnMoveToUnitGuidOrMask: the item's 4 bytes in the .i64");

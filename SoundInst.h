//  SoundInst.h -- the globals of SoundInst.cpp's data that are ours: the float literals, the pause fade and the duck, the sound instance pool,
//  the list of the instances in use, the playback flag, the next handle, the volume ramp's tick and the chaos debug flag.
//  Each is a C++17 inline variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above a global
//  gives its address in the original. The original ends the free-slot walk of the instance pool at gpSoundInstances' address; ours counts
//  200 records (gaSoundInstances + 200).
//  Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB70F90
//  The negative half of the sound distance clamp to [-2000, +2000]; the positive half is gfSoundMaxDistance.
inline const float gfSoundMaxDistanceNeg = -2000.0f;
static_assert(sizeof(gfSoundMaxDistanceNeg) == 4, "gfSoundMaxDistanceNeg: the item's 4 bytes in the .i64");

//  D2Client 0x6FB70F94
//  640.0, added in SOUNDINST_SetPosition: a shift from centre-relative to left-relative coordinates.
inline const float gfConst640 = 640.0f;
static_assert(sizeof(gfConst640) == 4, "gfConst640: the item's 4 bytes in the .i64");

//  D2Client 0x6FB70F98
//  The pooled literal 0.5, read by PlaySound, SOUNDINST_GetUnitSoundPosition and SOUNDINST_ServiceInstances.
inline const float gfHalf = 0.5f;
static_assert(sizeof(gfHalf) == 4, "gfHalf: the item's 4 bytes in the .i64");

//  D2Client 0x6FB70F9C
//  -0.4, multiplied in SOUNDINST_GetUnitSoundPosition.
inline const float gfConstNegPoint4 = -0.4000000059604645f;
static_assert(sizeof(gfConstNegPoint4) == 4, "gfConstNegPoint4: the item's 4 bytes in the .i64");

//  D2Client 0x6FB70FA0
//  The pooled literal -2.0, multiplied against the client coordinates in SOUNDINST_GetUnitSoundPosition.
inline const float gfNegTwo = -2.0f;
static_assert(sizeof(gfNegTwo) == 4, "gfNegTwo: the item's 4 bytes in the .i64");

//  D2Client 0x6FB70FA4
//  0.05, the fade step SOUNDINST_ServiceInstances adds on one branch and subtracts on the other.
inline const float gfSoundFadeStep = 0.05000000074505806f;
static_assert(sizeof(gfSoundFadeStep) == 4, "gfSoundFadeStep: the item's 4 bytes in the .i64");

//  D2Client 0x6FB70FA8
//  490000.0, which is 700 x 700: the squared earshot distance SOUNDINST_IsUnitInEarshot compares against.
inline const float gfEarshotDistanceSq = 490000.0f;
static_assert(sizeof(gfEarshotDistanceSq) == 4, "gfEarshotDistanceSq: the item's 4 bytes in the .i64");

//  D2Client 0x6FB96A94
//  The pause fade, 0..100: volume * n / 100 unless the sound id is exempt; ramped by 5 a tick to 0 while the Esc menu or config is open offline.
inline int32_t gnSoundPauseFadePercent = 100;
static_assert(sizeof(gnSoundPauseFadePercent) == 4, "gnSoundPauseFadePercent: the item's 4 bytes in the .i64");

//  D2Client 0x6FB96A98
//  The second volume scale, applied after gnSoundPauseFadePercent and exempted by a flag in the sound's record at +82; ramped by 2 a tick, floor 70.
inline int32_t gnSoundDuckPercent = 100;
static_assert(sizeof(gnSoundDuckPercent) == 4, "gnSoundDuckPercent: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD4D0
//  The pool of 200 sound instance records of 89 bytes; PlaySound takes a record whose +0 is zero as the free slot.
inline D2SoundInstanceStrc gaSoundInstances[200];
static_assert(sizeof(gaSoundInstances) == 17800, "gaSoundInstances: the item's 17800 bytes in the .i64");

//  D2Client 0x6FBC1A58
//  Head of the list of the instances in use; each node is { pRecord, pNext }.
inline D2SoundListNodeStrc *gpSoundInstances;
static_assert(sizeof(gpSoundInstances) == 4, "gpSoundInstances: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1A5C
//  The playback switch: PlaySound returns 0 at once while it is clear. Set to 1 by SOUNDINST_ResetPlayback and SOUNDINST_FreeAllInstances.
inline int32_t gbSoundPlaybackEnabled;
static_assert(sizeof(gbSoundPlaybackEnabled) == 4, "gbSoundPlaybackEnabled: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1A60
//  The instance handle counter: PlaySound increments it and stores it in the new instance's +8; SOUNDINST_ResetPlayback resets it to 0.
inline int32_t gnNextSoundInstanceHandle;
static_assert(sizeof(gnNextSoundInstanceHandle) == 4, "gnNextSoundInstanceHandle: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1A64
//  The tick the volume ramp was last stepped at; SOUNDINST_ServiceInstances skips the ramp when its tick is <= this.
inline uint32_t gnSoundVolumeRampTick;
static_assert(sizeof(gnSoundVolumeRampTick) == 4, "gnSoundVolumeRampTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1A68
//  The soundchaosdebug chat mode, toggled by SOUNDINST_ToggleChaosDebug; while set, SOUNDINST_ServiceInstances plays random speech sounds.
inline int32_t gbSoundChaosDebug;
static_assert(sizeof(gbSoundChaosDebug) == 4, "gbSoundChaosDebug: the item's 4 bytes in the .i64");

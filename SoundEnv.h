//  SoundEnv.h -- the globals of SoundEnv.cpp's data: two float literals of SOUNDENV_UpdateEnvironment's weather volume, and the eight dwords of
//  the level, the ambience and weather instances and sounds, and the event sound and its delay. Address ranges 0x6FB70F74..0x6FB70F7B (.rdata)
//  and 0x6FBBD474..0x6FBBD493 (past .data's raw data, which ends at 0x6FB9B000: the loader zero-fills them, and ours are zero-initialised).
//  Each is a C++17 inline variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above a global
//  gives its address in the original. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB70F74
//  255.0, fmul'ed in SOUNDENV_UpdateEnvironment to scale a fraction to a byte range.
inline const float gfConst255 = 255.0f;
static_assert(sizeof(gfConst255) == 4, "gfConst255: the item's 4 bytes in the .i64");

//  D2Client 0x6FB70F78
//  0.0, a literal the FPU needs in memory because fcomp takes no immediate. Read by SOUNDINST_PlaySound, SOUNDINST_ServiceInstances and
//  SOUNDENV_UpdateEnvironment, mostly as a sign or zero test and three times as a value; written by nothing.
inline const float gfSoundZero = 0.0f;
static_assert(sizeof(gfSoundZero) == 4, "gfSoundZero: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD474
//  The level the footstep sounds were last preloaded for; SOUNDENV_UpdateEnvironment does the reference-count work only when
//  SOUNDHDR_GetPlayerLevelId differs from it.
inline int32_t gnLastSoundLevelId;
static_assert(sizeof(gnLastSoundLevelId) == 4, "gnLastSoundLevelId: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD478
//  The playing instance of gnCurrentAmbienceSound, kept so the old loop can be faded down when the environment changes. Cleared together with
//  gnCurrentAmbienceSound.
inline int32_t ghCurrentAmbienceInstance;
static_assert(sizeof(ghCurrentAmbienceInstance) == 4, "ghCurrentAmbienceInstance: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD47C
//  The playing instance of the weather loop, or 0 when there is none. SOUNDENV_UpdateEnvironment walks its volume toward the target by at most
//  6 a tick; when the volume it computed is 0 the instance is marked started and the handle cleared.
inline int32_t ghWeatherSoundInstance;
static_assert(sizeof(ghWeatherSoundInstance) == 4, "ghWeatherSoundInstance: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD480
//  The sounds.txt id of the ambient loop currently playing, or 0 for none. SOUNDENV_UpdateEnvironment compares the wanted ambience against it
//  to decide whether anything has to change, and whether the change is a cross-fade or a cut.
inline D2C_Sounds gnCurrentAmbienceSound;
static_assert(sizeof(gnCurrentAmbienceSound) == 4, "gnCurrentAmbienceSound: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD484
//  The rain loop's sound id (SOUND_SCENE_RAIN or SOUND_NONE) that SOUNDENV_UpdateEnvironment stored on the previous tick; the one read of it is
//  the sound id handed to SOUNDINST_PlaySound.
inline D2C_Sounds gnWeatherSoundId;
static_assert(sizeof(gnWeatherSoundId) == 4, "gnWeatherSoundId: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD488
//  The sounds.txt id of the environment's random one-shot event, the day or night event of the current record. When it changes, the delay and
//  the last-fired tick are both re-seeded so a new area does not inherit the old one's schedule.
inline D2C_Sounds gnCurrentEventSound;
static_assert(sizeof(gnCurrentEventSound) == 4, "gnCurrentEventSound: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD48C
//  How long until the next environment event sound, re-rolled after every one as nEventDelay plus SOUNDSONG_RandomOffset(nEventDelay / 3), a
//  signed value in [-x, x].
inline uint32_t gnEventSoundDelay;
static_assert(sizeof(gnEventSoundDelay) == 4, "gnEventSoundDelay: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD490
//  When the last environment event sound fired. On a change of event sound it is seeded as `now - SOUNDSONG_RollRandom(delay)` rather than
//  `now`, so entering a new area does not always start with a full silent delay.
inline uint32_t gdwLastEventSoundTick;
static_assert(sizeof(gdwLastEventSoundTick) == 4, "gdwLastEventSoundTick: the item's 4 bytes in the .i64");

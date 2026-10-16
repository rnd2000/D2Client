//  SoundHdr.h -- the globals of SoundHdr.cpp's data: five float literals (the distances and falloffs of SOUNDHDR_GetFalloffDistance and
//  SOUNDHDR_GetRangeParam), the two path prefixes `\SFX\` and `\MUSIC\`, and the fourteen dwords of sounds.txt and soundenviron.txt as loaded,
//  their lookup, the preload cursor and the wave cache.
//  Each is a C++17 inline variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above a global gives
//  its address in the original. The fourteen dwords lie past .data's raw data (it ends at 0x6FB9B000), so they are zero at load and ours are
//  zero-initialised.
//  Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB70F7C
//  60.0: the default arm of SOUNDHDR_GetRangeParam's switch (falloff 0 and 1, and anything else).
inline const float gfConst60 = 60.0f;
static_assert(sizeof(gfConst60) == 4, "gfConst60: the item's 4 bytes in the .i64");

//  D2Client 0x6FB70F80
//  2000.0, the sound system's distance limit: SOUNDINST_PlaySound, SOUNDINST_SetPosition and SOUNDINST_ServiceInstances fcomp against it (two
//  compares each), and SOUNDHDR_GetFalloffDistance and SOUNDHDR_GetRangeParam fld it.
inline const float gfSoundMaxDistance = 2000.0f;
static_assert(sizeof(gfSoundMaxDistance) == 4, "gfSoundMaxDistance: the item's 4 bytes in the .i64");

//  D2Client 0x6FB70F84
//  400.0: a case value of SOUNDHDR_GetFalloffDistance (falloff 0) and of SOUNDHDR_GetRangeParam (falloff 3).
inline const float gfConst400 = 400.0f;
static_assert(sizeof(gfConst400) == 4, "gfConst400: the item's 4 bytes in the .i64");

//  D2Client 0x6FB70F88
//  700.0: the default value of SOUNDHDR_GetFalloffDistance. Its square, 490000, is the constant SOUNDINST_IsUnitInEarshot compares against
//  (gfEarshotDistanceSq).
inline const float gfConst700 = 700.0f;
static_assert(sizeof(gfConst700) == 4, "gfConst700: the item's 4 bytes in the .i64");

//  D2Client 0x6FB70F8C
//  1500.0: a case value of SOUNDHDR_GetFalloffDistance (falloff 3).
inline const float gfConst1500 = 1500.0f;
static_assert(sizeof(gfConst1500) == 4, "gfConst1500: the item's 4 bytes in the .i64");

//  D2Client 0x6FB96A84
//  The path prefix `\SFX\`, which SOUNDHDR_BuildSoundPath and SOUNDHDR_LoadWaveData copy four bytes at a time (a dword and a word); 0x6FB96A88 is
//  the string's tail, not a global.
inline char gszSfxPathPrefix[6] = "\\SFX\\";
static_assert(sizeof(gszSfxPathPrefix) == 6, "gszSfxPathPrefix: the item's 6 bytes in the .i64");

//  D2Client 0x6FB96A8C
//  The path prefix `\MUSIC\`, the twin of gszSfxPathPrefix, copied the same way (two dwords); 0x6FB96A90 is the string's second half, not a global.
inline char gszMusicPathPrefix[8] = "\\MUSIC\\";
static_assert(sizeof(gszMusicPathPrefix) == 8, "gszMusicPathPrefix: the item's 8 bytes in the .i64");

//  D2Client 0x6FBBD494
//  A dword table allocated by SOUNDHDR_LoadSoundsTxt and zeroed by SOUNDSONG_Init, (gnSoundEnvironLookupMax - gnSoundEnvironLookupMin + 1) * 4
//  bytes; read through SOUNDHDR_GetEnvironLookup by SOUNDSONG_Init, SOUNDSONG_UpdateMusic and SOUNDSONG_ScheduleSting.
inline int32_t *gpSoundEnvironLookup;
static_assert(sizeof(gpSoundEnvironLookup) == 4, "gpSoundEnvironLookup: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD498
//  Base of the sound header array, 142 bytes per record; the reference count (dwRefCount) is at +138.
inline D2SoundsTxt *gpSoundHeaders;
static_assert(sizeof(gpSoundHeaders) == 4, "gpSoundHeaders: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD49C
//  Number of records in gpSoundHeaders. Not every index is checked against it: SOUNDHDR_ServiceAsyncLoads walks the records 1..2933 against a
//  constant.
inline int32_t gnSoundHeaderCount;
static_assert(sizeof(gnSoundHeaderCount) == 4, "gnSoundHeaderCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD4A0
//  Compiled soundenviron.txt, 88 bytes per record. Read through SOUNDHDR_GetEnvRecord; SOUNDHDR_LoadSoundsTxt tests and walks it directly and
//  SOUNDHDR_Shutdown frees it.
inline D2SoundEnvironTxt *gpSoundEnvironTxt;
static_assert(sizeof(gpSoundEnvironTxt) == 4, "gpSoundEnvironTxt: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD4A4
//  Number of records in gpSoundEnvironTxt; 50 in the stock data.
inline int32_t gnSoundEnvironTxtRecordCount;
static_assert(sizeof(gnSoundEnvironTxtRecordCount) == 4, "gnSoundEnvironTxtRecordCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD4A8
//  The smallest positive Song value (field +0) of the soundenviron.txt records, seeded with gnSoundHeaderCount; with gnSoundEnvironLookupMax it
//  sizes gpSoundEnvironLookup.
inline int32_t gnSoundEnvironLookupMin;
static_assert(sizeof(gnSoundEnvironLookupMin) == 4, "gnSoundEnvironLookupMin: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD4AC
//  The largest positive Song value of the soundenviron.txt records; the upper half of the pair described on gnSoundEnvironLookupMin.
inline int32_t gnSoundEnvironLookupMax;
static_assert(sizeof(gnSoundEnvironLookupMax) == 4, "gnSoundEnvironLookupMax: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD4B0
//  The head of D2Sound's list of loaded sound ids, a D2SoundListNodeStrc *; its address is passed as this to D2Sound #10059 and #10063. Used by
//  SOUNDHDR_LoadWaveData, SOUNDHDR_FreeSoundData, SOUNDHDR_Shutdown and SOUNDHDR_TouchCacheEntry.
inline D2SoundListNodeStrc *gSoundDataList;
static_assert(sizeof(gSoundDataList) == 4, "gSoundDataList: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD4B4
//  The tick SOUNDHDR_PumpPreload passes to its four callees; it advances by one per call and is cleared by SOUNDHDR_Init. SOUNDHDR_LoadWaveData
//  stamps it into a record's +7Ch as the load's time.
inline uint32_t gnSoundPreloadCursor;
static_assert(sizeof(gnSoundPreloadCursor) == 4, "gnSoundPreloadCursor: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD4B8
//  The tick SOUNDHDR_ServiceAsyncLoads last ran a full scan at; the next call does nothing until 0x19 ticks have passed. Zeroed by SOUNDHDR_Init.
inline uint32_t gdwSoundLastServiceTick;
static_assert(sizeof(gdwSoundLastServiceTick) == 4, "gdwSoundLastServiceTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD4BC
//  The tick SOUNDHDR_LoadWaveData last started a load at; SOUNDHDR_ServiceAsyncLoads gives up early when the cursor is within 0xFA ticks of it.
//  Zeroed by SOUNDHDR_Init.
inline uint32_t gdwSoundLastLoadTick;
static_assert(sizeof(gdwSoundLastLoadTick) == 4, "gdwSoundLastLoadTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD4C0
//  How many asynchronous wave loads are outstanding; SOUNDHDR_ServiceAsyncLoads starts no more once this reaches 15, and SOUNDHDR_LoadWaveData
//  moves it both ways. Cleared by SOUNDHDR_Init.
inline uint32_t gnSoundAsyncLoadsInFlight;
static_assert(sizeof(gnSoundAsyncLoadsInFlight) == 4, "gnSoundAsyncLoadsInFlight: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD4C4
//  How many bytes of wave data the sound cache may hold, set by SOUNDHDR_SetCacheBudget (from MEMPART_InitMemoryPartitions); SOUNDHDR_LoadWaveData
//  frees other sounds to stay inside it.
inline uint32_t gnSoundCacheBudget;
static_assert(sizeof(gnSoundCacheBudget) == 4, "gnSoundCacheBudget: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBD4C8
//  How many bytes of wave data the sound cache currently holds: added to by SOUNDHDR_LoadWaveData, lowered by SOUNDHDR_FreeSoundData and
//  SOUNDHDR_Shutdown, and compared against gnSoundCacheBudget.
inline uint32_t gnSoundCacheUsed;
static_assert(sizeof(gnSoundCacheUsed) == 4, "gnSoundCacheUsed: the item's 4 bytes in the .i64");

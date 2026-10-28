//  Gfx.h -- the globals of Gfx.cpp's data: the preload lists and their counts, the five acts' COF data files, the monster-mode token override
//  table, the two COF caches, the player and monster preload tables, the four hilight palette indices, the preload pass's state and the memory
//  counters. Each is a C++17 `inline` variable, one object for every file that includes this header. The globals from 0x6FBC1B08 on lie past
//  .data's raw data (which ends at 0x6FB9B000) and are zero at load.
//  Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB96E08
//  The 8 components the preloader fetches for every nearby unit, in priority order: 5, 6, 1, 2, 3, 7, 10, 4 (half of the 16 NUM_COMPONENTS; the
//  rest load on demand). The count follows at 0x6FB96E28. Walked by GFX_PreloadAllPlayerModes and the two preload sweeps.
inline D2Composits gPreloadComponents[8] =
{ COMPOSIT_RIGHTHAND, COMPOSIT_LEFTHAND, COMPOSIT_TORSO, COMPOSIT_LEGS, COMPOSIT_RIGHTARM, COMPOSIT_SHIELD, COMPOSIT_SPECIAL3, COMPOSIT_LEFTARM };
static_assert(sizeof(gPreloadComponents) == 32, "gPreloadComponents: the item's 32 bytes in the .i64");

//  D2Client 0x6FB96E28
//  The number of entries in gPreloadComponents; the loop bound of GFX_PreloadAllPlayerModes and the other preloaders.
inline int32_t gnPreloadComponentCount = 8;
static_assert(sizeof(gnPreloadComponentCount) == 4, "gnPreloadComponentCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FB96E2C
//  The 15 animation modes the preloader fetches for every nearby unit, in priority order (mode 12 is fetched before the idle and walk modes). The
//  count follows at 0x6FB96E68. Walked by GFX_PreloadAllAnimModes, GFX_PreloadVisibleUnits, GFX_PreloadOnLoadComplete and GFX_PreloadUnit.
inline int32_t gPreloadAnimModes[15] = { 12, 0, 1, 2, 15, 4, 3, 5, 7, 6, 13, 8, 9, 10, 11 };
static_assert(sizeof(gPreloadAnimModes) == 60, "gPreloadAnimModes: the item's 60 bytes in the .i64");

//  D2Client 0x6FB96E68
//  The number of entries in gPreloadAnimModes (15); the loop bound of GFX_AddMonsterCofFiles, GFX_PreloadUnit, GFX_PreloadAllAnimModes and the rest.
inline int32_t gnPreloadAnimModeCount = 15;
static_assert(sizeof(gnPreloadAnimModeCount) == 4, "gnPreloadAnimModeCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FB96E6C
//  The five per-act common COF tables GFX_LoadActCofData opens under `DATA\GLOBAL\`: cmncof_a1.d2, a2, a3, a4 and a6 (acts 1-4 and 6; act 5 has
//  none, and the index is a position in this table rather than an act number).
inline const char *gActCofDataFiles[5] = { "cmncof_a1.d2", "cmncof_a2.d2", "cmncof_a3.d2", "cmncof_a4.d2", "cmncof_a6.d2" };
static_assert(sizeof(gActCofDataFiles) == 20, "gActCofDataFiles: the item's 20 bytes in the .i64");

//  D2Client 0x6FB96E80
//  The monster-mode token override table read by GFX_BuildMonsterCofName: rows of { dwToken, nMonMode }, one row; its count is
//  gnMonModeOverrideCount.
inline D2ClientMonModeTokenOverrideStrc gMonModeTokenOverrides[1] = { { .dwToken = 0x20206867, .nMonMode = 13 } };
static_assert(sizeof(gMonModeTokenOverrides) == 8, "gMonModeTokenOverrides: the item's 8 bytes in the .i64");

//  D2Client 0x6FB96E88
//  The row count of gMonModeTokenOverrides (1), read by GFX_BuildMonsterCofName.
inline uint32_t gnMonModeOverrideCount = 1;
static_assert(sizeof(gnMonModeOverrideCount) == 4, "gnMonModeOverrideCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1B08
//  The COF hash table used for UNIT_PLAYER, one D2ClientCofCacheStrc (0x80C bytes): +0 bLoaded (nothing reads or writes it for this table),
//  +4 pBlock (the allocated block behind the table; GFX_FreeCelCache frees it, then clears the whole table), +8 nBlockSize (the block's byte
//  size, subtracted from gnCofMemoryUsed on teardown), +0xC pBuckets: 512 hash buckets (GFX_HashCofName's nine-bit fold), each the head of a
//  chain of 28-byte nodes { pNext, flags, nSize, name[8], pData }. Only entries whose flag bit 0 is set are freed, so the table can hold
//  borrowed entries it does not own. GFX_InitCelCache and GFX_FreeCelCache clear the whole cache from this address with `rep stosd` over 203h
//  dwords.
inline D2ClientCofCacheStrc gCofCachePlayer;
static_assert(sizeof(gCofCachePlayer) == 2060, "gCofCachePlayer: the item's 2060 bytes in the .i64");

//  D2Client 0x6FBC2318
//  Per (component, class) marker set when a cel could not be produced, indexed `4 * (nComponent + 40 * nClass)`. GFX_PreloadPlayerMode checks it
//  first and skips all further work when it is up; it writes 1 here after
//  `D2CMP_SPRITECACHE_SpriteFindAsynchSiblings(nType, nComponent, nClass)` returns 0, and clears it together with gaComponentCelQueued when
//  neither path succeeds.
//  The size is NOT ESTABLISHED: this table and gaComponentCelQueued sit 80 bytes apart while sharing an index that reaches 255, so they cannot both
//  be [7][40]. MEMPART_InitMemoryPartitions zeroes 0x460 bytes (280 ints) from here in one memset, which covers both and ends where
//  gCofCacheOther begins.
inline D2ClientModePreloadStrc gaPlayerPreloadTable[7];
static_assert(sizeof(gaPlayerPreloadTable) == 1120, "gaPlayerPreloadTable: the item's 1120 bytes in the .i64");

//  D2Client 0x6FBC2778
//  The COF hash table used for every unit type except UNIT_PLAYER, one D2ClientCofCacheStrc with the layout of gCofCachePlayer. bLoaded is the
//  load-once flag GFX_LoadActCofData reads as its guard and sets to 1 once it has loaded the act's `cmncof_aN.d2`. pBlock is freed by
//  GFX_FreeCofCache and on the GFX_FreeCelCache path; pBuckets is the array GFX_LoadCofFile chains into (`pTable[12 + 4*hash]`) and
//  GFX_AddMonsterCofFiles reads directly. The table ends where gpMonsterPreloadTable begins (0x6FBC2778 + 0x80C).
inline D2ClientCofCacheStrc gCofCacheOther;
static_assert(sizeof(gCofCacheOther) == 2060, "gCofCacheOther: the item's 2060 bytes in the .i64");

//  D2Client 0x6FBC2F84
//  One 160-byte record per monstats.txt row. Allocated by MEMPART_InitMemoryPartitions as FOG_AllocPool(0, 160 * nMonStatsTxtRecordCount) and zeroed;
//  freed and set back to 0 by GFX_FreeCelCache. Indexed through GFX_GetMonsterPreloadEntry, which multiplies by 160 with no bounds check.
inline D2ClientModePreloadStrc *gpMonsterPreloadTable;
static_assert(sizeof(gpMonsterPreloadTable) == 4, "gpMonsterPreloadTable: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC2F88
//  Palette index for green, built once as `D2Win_10034_MixRGB(0, 0xC0, 0)` by GFX_GetUnitHilightPaletteIndex behind gbPaletteIndicesInit. The default
//  tint: returned for any unit that is not a monster, for a monster with no maximum life, and for a monster at 66% life or above (`cmp eax, 42h`,
//  0x6FB59F06, sends only the values below 66 to yellow).
inline uint8_t gnPaletteIndexGreen;
static_assert(sizeof(gnPaletteIndexGreen) == 1, "gnPaletteIndexGreen: the item's 1 bytes in the .i64");

//  D2Client 0x6FBC2F89
//  Palette index for yellow, `D2Win_10034_MixRGB(0xE0, 0xE0, 0)`. Returned by GFX_GetUnitHilightPaletteIndex for a monster between 33% and 66% of
//  its maximum life.
inline uint8_t gnPaletteIndexYellow;
static_assert(sizeof(gnPaletteIndexYellow) == 1, "gnPaletteIndexYellow: the item's 1 bytes in the .i64");

//  D2Client 0x6FBC2F8A
//  Palette index for red, `D2Win_10034_MixRGB(0xE0, 0, 0)`. Returned by GFX_GetUnitHilightPaletteIndex for a monster below 33% of its maximum life.
inline uint8_t gnPaletteIndexRed;
static_assert(sizeof(gnPaletteIndexRed) == 1, "gnPaletteIndexRed: the item's 1 bytes in the .i64");

//  D2Client 0x6FBC2F8B
//  Palette index for blue, `D2Win_10034_MixRGB(0, 0, 0xC0)`. Returned by GFX_GetUnitHilightPaletteIndex for a monster whose monstats.txt
//  `nMonStatsFlags[1]` has bit 1 set, before the life bands are considered at all.
inline uint8_t gnPaletteIndexBlue;
static_assert(sizeof(gnPaletteIndexBlue) == 1, "gnPaletteIndexBlue: the item's 1 bytes in the .i64");

//  D2Client 0x6FBC2F8C
//  A GetTickCount stamp GFX_PreloadOnLoadComplete uses to throttle its second phase: it records the tick and then requires
//  `GetTickCount() >= gdwPreloadThrottleTick + 600` before doing the work again (600 ms), unless gbPreloadPassActive is up, in which case the
//  throttle is bypassed. MEMPART_InitMemoryPartitions zeroes it at startup.
inline uint32_t gdwPreloadThrottleTick;
static_assert(sizeof(gdwPreloadThrottleTick) == 4, "gdwPreloadThrottleTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC2F90
//  Printed by DRAW_GameDrawDebugOverlay as "GFX Memory->%i". Nothing in the module writes it (its only reference is the read in
//  GFX_GetGfxMemoryUsed), so the line always displays 0.
inline int32_t gnGfxMemoryUsed;
static_assert(sizeof(gnGfxMemoryUsed) == 4, "gnGfxMemoryUsed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC2F94
//  Running byte total of the COF and cel caches, printed by DRAW_GameDrawDebugOverlay as the first figure of "COF Memory->%i of %i".
//  GFX_LoadCofFile and GFX_LoadActCofData add to it; GFX_FreeCofCache and GFX_FreeCelCache subtract `28 + entry->nSize` per entry freed, then the
//  table's own size (GFX_FreeCelCache takes off both tables' sizes, gCofCachePlayer.nBlockSize and gCofCacheOther.nBlockSize).
inline int32_t gnCofMemoryUsed;
static_assert(sizeof(gnCofMemoryUsed) == 4, "gnCofMemoryUsed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC2F98
//  The ceiling the debug overlay prints after "of". MEMPART_InitMemoryPartitions sets it to a literal 0x200000 (2 MB) and nothing else writes it,
//  so despite that function's name it is a constant rather than anything derived from the machine.
inline int32_t gnCofMemoryBudget;
static_assert(sizeof(gnCofMemoryBudget) == 4, "gnCofMemoryBudget: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC2F9C
//  Set to 1 while GFX_PreloadOnLoadComplete is running its full pass and cleared at the end; nothing else writes it. It is read as a licence to
//  ignore the time budget: inside the pass, `if (!gbPreloadPassActive && D2CMP_SPRITECACHE_SpriteNumAsyncLoads() >= 0x1E) break` stops after
//  30 ms once the flag is down, and GFX_PreloadVisibleUnits checks it to keep out of the way while the bulk load is in progress.
inline int32_t gbPreloadPassActive;
static_assert(sizeof(gbPreloadPassActive) == 4, "gbPreloadPassActive: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC2FA0
//  One-shot guard for the four palette indices above. GFX_GetUnitHilightPaletteIndex mixes all four and sets this to 1 the first time it is called,
//  so the colours are built lazily: they need the palette to be loaded.
inline int32_t gbPaletteIndicesInit;
static_assert(sizeof(gbPaletteIndicesInit) == 4, "gbPaletteIndicesInit: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC2FA4
//  Set when the client must preload graphics because the character archive is not available locally: the return of D2Win ordinal 10185
//  (d2char.mpq in the install path -> 0; in the working dir -> 0; otherwise !FOG_UseDirect()), stored by GFX_InitCelCache (`mov ds:[6FBC2FA4h], eax`,
//  0x6FB5912F). So the character graphics are neither in an MPQ nor loose on disk -- the CD-install case. A plain gate in
//  GFX_PreloadVisibleUnits and GFX_PreloadOnLoadComplete, and the case-0 arm of GFXUTIL_ResolveAnimDirection's switch.
inline BOOL gbPreloadRequired;
static_assert(sizeof(gbPreloadRequired) == 4, "gbPreloadRequired: the item's 4 bytes in the .i64");

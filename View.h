//  View.h -- the globals of View.cpp's data block, read by View.cpp and ViewEffects.cpp: the Arcane Sanctuary starfield, the parallax, the screen
//  shake, the five gouraud tables and the two seed objects. Address range 0x6FB7712C..0x6FBAF8FC.
//  Each is a C++17 inline variable, so every file that includes this header shares one object. The globals from 0x6FBABA00 on lie past the end of
//  .data's raw data (0x6FB9B000): the loader zero-fills them, and ours are zero-initialised.
//  gpEnvSeed and gpParallaxCloudSeed are initialised in the image to point at a seed object each; the two seed objects (gEnvSeedStorage,
//  gParallaxCloudSeedStorage) are declared first, out of address order, because the pointers' initializers take their addresses.
//  gnScreenShakeOffsetX/Y are unsigned: they are only stored, shifted, added and copied, and they go negative, where a << 16 on a signed value is
//  undefined in C++17 and on an unsigned one defined.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after DLLmain.h and D2Client.h.
#pragma once

//  D2Client 0x6FBAF348
//  The cloud layer's seed, eight bytes of D2SeedStrc; VIEWEFFECTS_EnvDrawParallax reaches it through gpParallaxCloudSeed.
inline D2SeedStrc gParallaxCloudSeedStorage;
static_assert(sizeof(gParallaxCloudSeedStorage) == 8, "gParallaxCloudSeedStorage: the item's 8 bytes in the .i64");

//  D2Client 0x6FBAF37C
//  The environment seed, eight bytes of D2SeedStrc; VIEWEFFECTS_EnvUpdateViewAndLevelEffects and VIEWEFFECTS_EnvRespawnArcaneStar reach it
//  through gpEnvSeed.
inline D2SeedStrc gEnvSeedStorage;
static_assert(sizeof(gEnvSeedStorage) == 8, "gEnvSeedStorage: the item's 8 bytes in the .i64");

//  D2Client 0x6FB7712C
//  A fixed pointer to the environment RNG seed, never written; only VIEWEFFECTS_EnvRespawnArcaneStar and
//  VIEWEFFECTS_EnvUpdateViewAndLevelEffects read it. In the original it holds 0x6FBAF37C; ours holds gEnvSeedStorage's address.
inline D2SeedStrc *gpEnvSeed = &gEnvSeedStorage;
static_assert(sizeof(gpEnvSeed) == 4, "gpEnvSeed: the item's 4 bytes in the .i64");

//  D2Client 0x6FB77130
//  A fixed pointer to the RNG seed the cloud layer draws from, never written; only VIEWEFFECTS_EnvDrawParallax reads it. In the original it holds
//  0x6FBAF348; ours holds gParallaxCloudSeedStorage's address.
inline D2SeedStrc *gpParallaxCloudSeed = &gParallaxCloudSeedStorage;
static_assert(sizeof(gpParallaxCloudSeed) == 4, "gpParallaxCloudSeed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBABA00
//  The Y of each Arcane Sanctuary background star, set once per respawn and never moved; each star is drawn as a single pixel.
inline int32_t gnArcaneStarY[256];
static_assert(sizeof(gnArcaneStarY) == 1024, "gnArcaneStarY: the item's 1024 bytes in the .i64");

//  D2Client 0x6FBABE00
//  The X of each Arcane Sanctuary background star. It drifts left by gnArcaneStarVelX every 40 ms, and VIEWEFFECTS_EnvRespawnArcaneStar puts it
//  back just off the right edge when it passes 0.
inline int32_t gnArcaneStarX[256];
static_assert(sizeof(gnArcaneStarX) == 1024, "gnArcaneStarX: the item's 1024 bytes in the .i64");

//  D2Client 0x6FBAC200
//  The client X the parallax scroll is measured from. VIEWEFFECTS_EnvDrawParallax computes gnParallaxOffsetX = UNITS_GetClientCoordX(pUnit) -
//  gnParallaxOriginX. It is written twice at the top of that function and the second write always wins (the constant 10063), so the origin is
//  fixed; see gbParallaxOriginLatched and gbParallaxOriginForced.
inline int32_t gnParallaxOriginX;
static_assert(sizeof(gnParallaxOriginX) == 4, "gnParallaxOriginX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAC204
//  Screen-shake envelope ramp-down duration in ms.
inline uint32_t gnScreenShakeRampDownMs;
static_assert(sizeof(gnScreenShakeRampDownMs) == 4, "gnScreenShakeRampDownMs: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAC208
//  Screen-shake envelope ramp-up duration in ms, set by VIEWEFFECTS_StartScreenShake, read by VIEWEFFECTS_UpdateScreenShake.
inline uint32_t gnScreenShakeRampUpMs;
static_assert(sizeof(gnScreenShakeRampUpMs) == 4, "gnScreenShakeRampUpMs: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAC20C
//  Current frame's screen-shake view offset (Y), the other half of gnScreenShakeOffsetX. Its address is not adjacent to gnScreenShakeOffsetX and
//  gnScreenShakeMagnitude (0x6FBAF8xx), though it is used alongside them.
inline uint32_t gnScreenShakeOffsetY;
static_assert(sizeof(gnScreenShakeOffsetY) == 4, "gnScreenShakeOffsetY: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAC210
//  Handle of the looping screen-shake sound, sounds.txt 452 andariel_quake_loop. VIEWEFFECTS_UpdateScreenShakeSound owns the lifetime:
//  SOUNDINST_PlaySound(452) stores the handle here, SOUNDINST_SetVolumeByHandle rides the volume with the shake, and the global is cleared to 0
//  when the shake ends. VIEW_FreeGouraudTables clears it too.
inline int32_t ghScreenShakeSound;
static_assert(sizeof(ghScreenShakeSound) == 4, "ghScreenShakeSound: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAC214
//  SCALE_GetFactor() for the current frame, stored by VIEWEFFECTS_EnvUpdateViewAndLevelEffects on each call that gets past the view test and never
//  read.
inline int32_t gnViewScaleFactor;
static_assert(sizeof(gnViewScaleFactor) == 4, "gnViewScaleFactor: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAC218
//  X of the ten drifting clouds, in 1/16 pixel: VIEWEFFECTS_EnvDrawParallax shifts it right by 4 before drawing and adds gnParallaxCloudSpeedX[i]
//  (8..23) each tick. A cloud that passes the right edge is reset to 0xFFFFE900 (-5888, i.e. -368 pixels), so the clouds cycle left to right.
inline int32_t gnParallaxCloudX[10];
static_assert(sizeof(gnParallaxCloudX) == 40, "gnParallaxCloudX: the item's 40 bytes in the .i64");

//  D2Client 0x6FBAC240
//  D2Client's own copy of D2Win's shadow/light/gamma palette table, filled once by D2Win #10178 at the top of VIEW_AllocGouraudTables: 12544 bytes,
//  49 palette rows of 64 PALETTEENTRY (32 shadow, 16 light, 1 gamma). sgpGouraudTblX and gpSubtileGouraudTbl hold pointers to its 256-byte rows.
inline D2ShadowLightGammaPaletteStrc gShadowLightGammaPalette;
static_assert(sizeof(gShadowLightGammaPalette) == 12544, "gShadowLightGammaPalette: the item's 12544 bytes in the .i64");

//  D2Client 0x6FBAF340
//  Bit 0 is a one-shot latch: the first time VIEWEFFECTS_EnvDrawParallax runs it sets the bit and stores the player's X into gnParallaxOriginX; the
//  next statement overwrites that with the constant 10063 (see gbParallaxOriginForced), so the first value never survives a frame.
inline uint8_t gbParallaxOriginLatched;
static_assert(sizeof(gbParallaxOriginLatched) == 1, "gbParallaxOriginLatched: the item's 1 bytes in the .i64");

//  D2Client 0x6FBAF344
//  GetTickCount() timestamp at which VIEWEFFECTS_StartScreenShake records the shake as begun; VIEWEFFECTS_UpdateScreenShake computes elapsed time
//  against it every frame.
inline int32_t gdwScreenShakeStartTime;
static_assert(sizeof(gdwScreenShakeStartTime) == 4, "gdwScreenShakeStartTime: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF350
//  Y of the ten drifting clouds, in whole pixels (passed to the cel draw without a shift, unlike gnParallaxCloudX). Seeded as a remainder, div 250
//  on one path and div 300 on the other.
inline int32_t gnParallaxCloudY[10];
static_assert(sizeof(gnParallaxCloudY) == 40, "gnParallaxCloudY: the item's 40 bytes in the .i64");

//  D2Client 0x6FBAF378
//  GetTickCount of the last starfield step in the Arcane Sanctuary; the field advances when more than 0x28 (40) ms have passed, so the twinkle is on
//  a wall clock and not on the frame rate.
inline int32_t gnArcaneStarTick;
static_assert(sizeof(gnArcaneStarTick) == 4, "gnArcaneStarTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF388
//  Per-star colour index for the Arcane Sanctuary starfield, indexed by the same loop counter as the point coordinates and passed straight to
//  D2GFX_DrawLine.
inline uint8_t gnArcaneStarColors[256];
static_assert(sizeof(gnArcaneStarColors) == 256, "gnArcaneStarColors: the item's 256 bytes in the .i64");

//  D2Client 0x6FBAF488
//  Screen-shake envelope hold (full-magnitude) duration in ms; VIEWEFFECTS_StartScreenShake treats 0 here as 'do not start a shake at all'.
inline uint32_t gnScreenShakeHoldMs;
static_assert(sizeof(gnScreenShakeHoldMs) == 4, "gnScreenShakeHoldMs: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF490
//  Each star's horizontal velocity, -1 to -5 pixels per 40 ms tick (-1 - rand % 5). Added to gnArcaneStarX by
//  VIEWEFFECTS_EnvUpdateViewAndLevelEffects; a star whose X goes negative is handed to VIEWEFFECTS_EnvRespawnArcaneStar.
inline int32_t gnArcaneStarVelX[256];
static_assert(sizeof(gnArcaneStarVelX) == 1024, "gnArcaneStarVelX: the item's 1024 bytes in the .i64");

//  D2Client 0x6FBAF890
//  Per-cloud drift speed, added to gnParallaxCloudX each tick and so also in 1/16 pixel: (rand & 0xF) + 8, i.e. 8..23 sixteenths, drawn from the
//  seed at gpParallaxCloudSeed.
inline int32_t gnParallaxCloudSpeedX[10];
static_assert(sizeof(gnParallaxCloudSpeedX) == 40, "gnParallaxCloudSpeedX: the item's 40 bytes in the .i64");

//  D2Client 0x6FBAF8B8
//  The eight colours a star can be, built once by VIEWEFFECTS_EnvUpdateViewAndLevelEffects from D2Win_10034_MixRGB over a ramp and indexed by
//  VIEWEFFECTS_EnvRespawnArcaneStar with rand & 7.
inline char gArcaneStarPalette[8];
static_assert(sizeof(gArcaneStarPalette) == 8, "gArcaneStarPalette: the item's 8 bytes in the .i64");

//  D2Client 0x6FBAF8C0
//  The client frame VIEWEFFECTS_EnvUpdateViewAndLevelEffects last ran its per-frame block on: it calls GAME_GetFrameCount(), compares unsigned, and
//  stores the new frame here, so the block runs at most once per client frame. VIEW_Alloc resets it to 0.
inline int32_t gnViewEffectsLastFrame;
static_assert(sizeof(gnViewEffectsLastFrame) == 4, "gnViewEffectsLastFrame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF8C4
//  Peak screen-shake amplitude for the in-progress shake, set by VIEWEFFECTS_StartScreenShake with the three durations and the start time (and
//  only when the hold argument is not zero); consumed by VIEWEFFECTS_UpdateScreenShake's ramp-up/hold/ramp-down envelope to produce
//  gnScreenShakeOffsetX/Y each frame. Zeroed by VIEWEFFECTS_ResetScreenShake and by VIEWEFFECTS_UpdateScreenShake's expiry path.
inline uint32_t gnScreenShakeMagnitude;
static_assert(sizeof(gnScreenShakeMagnitude) == 4, "gnScreenShakeMagnitude: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF8C8
//  Current frame's screen-shake view offset (X), computed by VIEWEFFECTS_UpdateScreenShake from gnScreenShakeMagnitude's envelope; read via
//  VIEWEFFECTS_GetScreenShakeOffset.
inline uint32_t gnScreenShakeOffsetX;
static_assert(sizeof(gnScreenShakeOffsetX) == 4, "gnScreenShakeOffsetX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF8CC
//  36864 bytes (0x9000) of pointers into the 32-row shadow/light gamma palette, built by VIEW_AllocGouraudTables (named by its assert at
//  View.cpp:202): 32 x 32 x 9 dword entries, 8 interpolation steps plus one repeated slot. Allocated with VirtualAlloc PAGE_EXECUTE_READWRITE,
//  freed by VIEW_FreeGouraudTables; cached into the view object at +0x2C.
inline uint8_t **sgpGouraudTblX;
static_assert(sizeof(sgpGouraudTblX) == 4, "sgpGouraudTblX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF8D0
//  16387 bytes (0x4003): 32 x 32 x 16 one-byte shade indices plus a three-byte guard, built by VIEW_AllocGouraudTables (assert at View.cpp:203);
//  cached into the view object at +0x30.
inline uint8_t *sgpGouraudTblY;
static_assert(sizeof(sgpGouraudTblY) == 4, "sgpGouraudTblY: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF8D4
//  32771 bytes (0x8003): 32 x 32 x 32 one-byte shade indices plus a three-byte guard, built by VIEW_AllocGouraudTables (assert at View.cpp:204);
//  cached into the view object at +0x34. What distinguishes it from sgpGouraudTblX is not established, but TblX holds pointers and this holds
//  indices.
inline uint8_t *sgpGouraudTblXVal;
static_assert(sizeof(sgpGouraudTblXVal) == 4, "sgpGouraudTblXVal: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF8DC
//  131072 bytes (0x20000), built by VIEW_AllocGouraudTables (assert at View.cpp:205): per-subtile shading, 32 x 32 x 32 dword entries, each a pointer
//  into gShadowLightGammaPalette. Not cached into the view object, and the table is never read: it is built and freed unread.
inline uint8_t **gpSubtileGouraudTbl;
static_assert(sizeof(gpSubtileGouraudTbl) == 4, "gpSubtileGouraudTbl: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF8E0
//  40963 bytes (0xA003), built by VIEW_AllocGouraudTables (assert at View.cpp:206): the per-tile counterpart of gpSubtileGouraudTbl, 32 x 32 x 40
//  byte shade indices plus a three-byte guard. Never read.
inline uint8_t *gpTileGouraudTbl;
static_assert(sizeof(gpTileGouraudTbl) == 4, "gpTileGouraudTbl: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF8E4
//  Handle for data\global\ui\summit01, the mountain layer of the Arcane Sanctuary parallax. Loaded lazily by VIEWEFFECTS_EnvDrawParallax when null,
//  freed and nulled by VIEWEFFECTS_EnvUpdateViewAndLevelEffects when the player leaves the level and by VIEW_FreeGouraudTables on teardown.
//  Drawn as four cels side by side at y = 256 and again at y = 512, cel index (base + n) % 4 for the upper row and + 4 for the lower; a fifth cel is
//  drawn at x - 256 when the offset has scrolled far enough to expose the left edge.
inline D2CellFileStrc *gpImgParallaxSummit;
static_assert(sizeof(gpImgParallaxSummit) == 4, "gpImgParallaxSummit: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF8E8
//  Handle for data\global\ui\cloud01, the cloud layer of the parallax. Same lazy load and same two teardown paths as gpImgParallaxSummit.
inline D2CellFileStrc *gpImgParallaxCloud;
static_assert(sizeof(gpImgParallaxCloud) == 4, "gpImgParallaxCloud: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF8EC
//  Whether the 256 Arcane Sanctuary background stars have been scattered. VIEWEFFECTS_EnvUpdateViewAndLevelEffects reads it after confirming the
//  level is 0x4A (74, Act 2 - Arcane) and sets it to 1 once the seeding loop (VIEWEFFECTS_EnvRespawnArcaneStar(i, 0) for each star) has run.
inline int32_t gbArcaneStarsSeeded;
static_assert(sizeof(gbArcaneStarsSeeded) == 4, "gbArcaneStarsSeeded: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF8F0
//  Whether the ten clouds have been given starting positions and speeds; VIEWEFFECTS_EnvDrawParallax tests it before the seeding loop and sets it
//  to 1 after.
inline uint8_t gbParallaxCloudsSeeded;
static_assert(sizeof(gbParallaxCloudsSeeded) == 1, "gbParallaxCloudsSeeded: the item's 1 bytes in the .i64");

//  D2Client 0x6FBAF8F4
//  UNITS_GetClientCoordX(pUnit) - gnParallaxOriginX, recomputed every frame by VIEWEFFECTS_EnvDrawParallax: / 8 / 256 selects the starting cel (a
//  signed modulo 4), and -(value / 8 % 256) is the sub-tile scroll X the cels are drawn at. The / 8 is the parallax ratio: the backdrop moves one
//  pixel for every eight the player does.
inline int32_t gnParallaxOffsetX;
static_assert(sizeof(gnParallaxOffsetX) == 4, "gnParallaxOffsetX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF8F8
//  The second one-shot latch in VIEWEFFECTS_EnvDrawParallax: when clear it is set to 1 and gnParallaxOriginX is overwritten with the constant
//  10063. A plain byte tested for zero, not a bit test.
inline uint8_t gbParallaxOriginForced;
static_assert(sizeof(gbParallaxOriginForced) == 1, "gbParallaxOriginForced: the item's 1 bytes in the .i64");

//  D2Client 0x6FBAF8FC
//  Gates the only call to DLIGHTMAP_DebugDrawLightGrid, at the tail of VIEWEFFECTS_EnvUpdateViewAndLevelEffects. Nothing writes it, so the debug
//  overlay never draws in this build.
inline int32_t gbDebugDrawLightGrid;
static_assert(sizeof(gbDebugDrawLightGrid) == 4, "gbDebugDrawLightGrid: the item's 4 bytes in the .i64");

//  dLightMap.h -- the globals of dLightMap.cpp (address range 0x6FB74C88..0x6FBA7623): the light grids, the propagation state, the dynamic-light list
//  and its counters, the quality governor, the warp-flash deadline and the cached ambient light. Each is a C++17 `inline` variable, one object for
//  every file that includes this header. The comment line "D2Client 0x6F..." above a global gives its address in the original.
//  Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB74C88
//  1 in the image and never written: nonzero, the light map carries colour; zero, DLIGHTMAP_AddLightToGrid zeroes the colour bytes.
inline int32_t gbColoredLights = 1;
static_assert(sizeof(gbColoredLights) == 4, "gbColoredLights: the item's 4 bytes in the .i64");

//  D2Client 0x6FB74D50
//  The assert text of the owner-unit variable of DLIGHTMAP_ActCallback.
inline char gszAssertHSrcUnit[9] = "hSrcUnit";
static_assert(sizeof(gszAssertHSrcUnit) == 9, "gszAssertHSrcUnit: the item's 9 bytes in the .i64");

//  D2Client 0x6FB9A9D8
//  +1 or -1: the Y direction toward the light source; the counterpart of gnPropagateStepX.
inline int32_t gnPropagateStepY;
static_assert(sizeof(gnPropagateStepY) == 4, "gnPropagateStepY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9A9E0
//  int[64*64], indexed [x + (y << 6)]: the propagation's source grid, with the light always at cell (32, 32); gLightGrid is the working copy.
inline int32_t gStaticLightGrid[4096];
static_assert(sizeof(gStaticLightGrid) == 16384, "gStaticLightGrid: the item's 16384 bytes in the .i64");

//  D2Client 0x6FB9E9E0
//  The X of the ambient grid's top-left cell, in light cells (subtile >> 3); subtracted from a world position it gives a 0..47 grid index.
inline int32_t gnLightGridOriginX;
static_assert(sizeof(gnLightGridOriginX) == 4, "gnLightGridOriginX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9E9E4
//  The Y counterpart of gnLightGridOriginX.
inline int32_t gnLightGridOriginY;
static_assert(sizeof(gnLightGridOriginY) == 4, "gnLightGridOriginY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9E9E8
//  gnLightGridOriginX + 48 (the 48x48 window is centred on the player); the light functions use it in an overlap test closed at both ends.
inline int32_t gnLightGridEndX;
static_assert(sizeof(gnLightGridEndX) == 4, "gnLightGridEndX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9E9EC
//  gnLightGridOriginY + 48, the twin of gnLightGridEndX.
inline int32_t gnLightGridEndY;
static_assert(sizeof(gnLightGridEndY) == 4, "gnLightGridEndY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9E9F0
//  +1 or -1: the X direction toward the light source, set by DLIGHTMAP_SetupPropagation and used by DLIGHTMAP_PropagateCell.
inline int32_t gnPropagateStepX;
static_assert(sizeof(gnPropagateStepX) == 4, "gnPropagateStepX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9E9F8
//  uint32_t[64*64], indexed [x + (y << 6)]: the propagation's output grid; its first 1024 dwords are cleared before a rebuild.
inline uint32_t gLightGrid[4096];
static_assert(sizeof(gLightGrid) == 16384, "gLightGrid: the item's 16384 bytes in the .i64");

//  D2Client 0x6FBA29F8
//  int[256]: a 16.16 reciprocal table built by DLIGHTMAP_Init, [0] = 0 and [i] = 0x10000 / i; the colour blend divides with a multiply and >> 16.
inline int32_t gnReciprocal16[256];
static_assert(sizeof(gnReciprocal16) == 1024, "gnReciprocal16: the item's 1024 bytes in the .i64");

//  D2Client 0x6FBA2DF8
//  The 8-bit interpolation weight of the diagonal propagation modes 2 and 3, (minor << 8) / major; not read in modes 0, 1 and 4.
inline int32_t gnPropagateFrac;
static_assert(sizeof(gnPropagateFrac) == 4, "gnPropagateFrac: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA2E00
//  The 48x48 ambient light grid, 8 bytes per cell at 8 * (x + 48 * y) from gnLightGridOriginX/Y; in a cell +4 is the intensity, +5..+7 the colour.
inline D2LightGridCellStrc gPlayerLightGrid[2304];
static_assert(sizeof(gPlayerLightGrid) == 18432, "gPlayerLightGrid: the item's 18432 bytes in the .i64");

//  D2Client 0x6FBA7600
//  Which arm DLIGHTMAP_PropagateCell takes: 0 source cell, 1 horizontal, 4 vertical, 2 mostly horizontal, 3 mostly vertical.
inline int32_t gnPropagateMode;
static_assert(sizeof(gnPropagateMode) == 4, "gnPropagateMode: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7604
//  Head of the singly linked list of every light, chained through D2ClientLightStrc::pNext; DLIGHTMAP_AddLight pushes onto the front.
inline D2ClientLightStrc *gpDynamicLights;
static_assert(sizeof(gpDynamicLights) == 4, "gpDynamicLights: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7608
//  Running count of light records allocated by DLIGHTMAP_AddLight; never read.
inline int32_t gnDLightAllocCount;
static_assert(sizeof(gnDLightAllocCount) == 4, "gnDLightAllocCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA760C
//  Running count of light records freed by DLIGHTMAP_RemoveLight; never read.
inline int32_t gnDLightFreeCount;
static_assert(sizeof(gnDLightFreeCount) == 4, "gnDLightFreeCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7610
//  How many lights DLIGHTMAP_UpdateForPlayer applied this frame; nothing reads it afterwards.
inline int32_t gnActiveDynamicLights;
static_assert(sizeof(gnActiveDynamicLights) == 4, "gnActiveDynamicLights: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7614
//  The light-map quality level, 0, 1 or 2, written only by DLIGHTMAP_UpdateQualityLevel (a change at most every 2000 ms); 0 at load.
inline int32_t gnDLightQualityLevel;
static_assert(sizeof(gnDLightQualityLevel) == 4, "gnDLightQualityLevel: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7618
//  GetTickCount stamp of the last quality-level change; DLIGHTMAP_UpdateQualityLevel refuses another within 0x7D0 (2000 ms).
inline int32_t gnDLightQualityLastTick;
static_assert(sizeof(gnDLightQualityLastTick) == 4, "gnDLightQualityLastTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA761C
//  GetTickCount deadline of the post-warp screen flash, set by DLIGHTMAP_StartWarpFlash and cleared by DLIGHTMAP_CancelWarpFlash.
inline uint32_t gdwWarpFlashEndTime;
static_assert(sizeof(gdwWarpFlashEndTime) == 4, "gdwWarpFlashEndTime: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7620
//  The cached ambient light's green; the four bytes are stored Green, Blue, Red, Intensity and nothing reads them.
inline uint8_t gnAmbientLightGreen;
static_assert(sizeof(gnAmbientLightGreen) == 1, "gnAmbientLightGreen: the item's 1 bytes in the .i64");

//  D2Client 0x6FBA7621
//  The cached ambient light's blue.
inline uint8_t gnAmbientLightBlue;
static_assert(sizeof(gnAmbientLightBlue) == 1, "gnAmbientLightBlue: the item's 1 bytes in the .i64");

//  D2Client 0x6FBA7622
//  The cached ambient light's red.
inline uint8_t gnAmbientLightRed;
static_assert(sizeof(gnAmbientLightRed) == 1, "gnAmbientLightRed: the item's 1 bytes in the .i64");

//  D2Client 0x6FBA7623
//  The cached ambient light's intensity.
inline uint8_t gnAmbientLightIntensity;
static_assert(sizeof(gnAmbientLightIntensity) == 1, "gnAmbientLightIntensity: the item's 1 bytes in the .i64");

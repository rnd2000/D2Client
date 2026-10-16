//  Draw.h -- the globals of Draw.cpp: the renderer in use, the frame-rate overlay's counters and format strings, and the two .rdata floats the
//  overlay and the ESC-menu sliders read. Each is a C++17 inline variable, so every file that includes this header shares one object; the two
//  .rdata floats are const. The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB6E154
//  Exactly 1/1024 (2^-10), a division by 1024 written as a multiply; read by DRAW_GameDrawDebugOverlay alone, which prints byte counts as
//  kilobytes ("%.2fK").
inline const float gfOneOver1024 = 0.0009765625f;
static_assert(sizeof(gfOneOver1024) == 4, "gfOneOver1024: the item's 4 bytes in the .i64");

//  D2Client 0x6FB6E158
//  100.0. The ESC-menu slider handlers divide it by the slider's step count (fdivr), giving the percent per step; ESCMENU_ContrastRefresh does
//  the same 100 / steps; DRAW_GameDrawDebugOverlay multiplies a fraction by it to print a percentage.
inline const float gfPercentDivisor = 100.0f;
static_assert(sizeof(gfPercentDivisor) == 4, "gfPercentDivisor: the item's 4 bytes in the .i64");

//  D2Client 0x6FB74970
//  The renderer in use, a DisplayType: 1 GDI, 2 SOFTWARE, 3 DDRAW, 4 GLIDE, 5 OPENGL, 6 DIRECT3D, 7 RAVE. The command-line handlers write it
//  through DRAW_SetDisplayType* (GDI through DRAW_SelectGdiDisplay); DRAW_GetDisplayType and DRAW_IsDisplayType3D (>= DISPLAYTYPE_GLIDE) read it.
//  Values 2 and 3 have no command-line handler; the file holds 3, so a client given no renderer switch reports DirectDraw.
inline DisplayType gDisplayType = DISPLAYTYPE_DDRAW;
static_assert(sizeof(gDisplayType) == 4, "gDisplayType: the item's 4 bytes in the .i64");

//  D2Client 0x6FB74A70
//  The overlay's tile cache line, a sprintf format: "Tile Cache: (%.2fK) %3.2f%%, Misses: %d, %d percent utilized" and a newline. The line it
//  builds is then handed to D2GFX_OutputString as a format again.
inline char gszTileCacheDebugFmt[62] = "Tile Cache: (%.2fK) %3.2f%%, Misses: %d, %d percent utilized\012";
static_assert(sizeof(gszTileCacheDebugFmt) == 62, "gszTileCacheDebugFmt: the item's 62 bytes in the .i64");

//  D2Client 0x6FB74B9C
//  The FPS line's format. The second %d is GAME_GetRecentSkipRate's answer, not gnFpsSkipCounter.
inline char szFormat[16] = "%d FPS  %d Skip";
static_assert(sizeof(szFormat) == 16, "szFormat: the item's 16 bytes in the .i64");

//  D2Client 0x6FB9A968
//  Raised by DRAW_SelectGdiDisplay together with gDisplayType = DISPLAYTYPE_GDI; read by nothing.
inline BOOL gbAskedForGDI;
static_assert(sizeof(gbAskedForGDI) == 4, "gbAskedForGDI: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9A96C
//  Frames counted in the last completed 1000 ms window, the number printed as "%d FPS". Env.cpp also reads it and tests it against 10.
inline int32_t gnFramesPerSecond;
static_assert(sizeof(gnFramesPerSecond) == 4, "gnFramesPerSecond: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9A970
//  GetTickCount at the start of the current FPS window; the window closes when more than 0x3E8 (1000) ms have passed.
inline int32_t gnFpsWindowTick;
static_assert(sizeof(gnFpsWindowTick) == 4, "gnFpsWindowTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9A974
//  Frames drawn so far in the current 1000 ms window; copied into gnFramesPerSecond and zeroed when the window closes.
inline int32_t gnFpsFrameCounter;
static_assert(sizeof(gnFpsFrameCounter) == 4, "gnFpsFrameCounter: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9A978
//  Incremented with every frame of the FPS window and reset when the overlay is switched on; nothing else reads it. It is not the Skip figure
//  of the overlay, which is GAME_GetRecentSkipRate().
inline int32_t gnFpsSkipCounter;
static_assert(sizeof(gnFpsSkipCounter) == 4, "gnFpsSkipCounter: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9A97C
//  GetTickCount at which skip counting began: set the first frame after the overlay is enabled, which is also when gnFpsSkipCounter is cleared.
inline int32_t gnFpsSkipStartTick;
static_assert(sizeof(gnFpsSkipStartTick) == 4, "gnFpsSkipStartTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9A980
//  Whether the overlay is showing. Toggled by DRAW_GameToggleDebugOverlay (/framerate) and DRAW_ToggleDebugOverlay (/fps); DRAW_GameDrawDebugOverlay
//  draws nothing without it but keeps counting frames.
inline int32_t gbDebugOverlay;
static_assert(sizeof(gbDebugOverlay) == 4, "gbDebugOverlay: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9A988
//  Which page of the debug overlay is shown; the memory and cache block is drawn only at page 0. Both toggles reset it to 0 first;
//  DRAW_ToggleDebugOverlay (/fps) then leaves it at 1 on every path, DRAW_GameToggleDebugOverlay (/framerate) at 0.
inline int32_t gnDebugOverlayPage;
static_assert(sizeof(gnDebugOverlayPage) == 4, "gnDebugOverlayPage: the item's 4 bytes in the .i64");

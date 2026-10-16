//  scroll.h -- the globals of scroll.cpp: gnViewportOriginY, gnViewportOriginX, gnUiShiftOffsetX, gUiShiftScreen and gUiPlayerPosAdjust
//  (0x6FBAB9B8..0x6FBAB9C8). Each is a C++17 inline variable, so every file that includes this header shares one object. They lie past the
//  end of .data's raw data (0x6FB9B000): the loader zero-fills them, and ours are zero-initialised.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after DLLmain.h and D2Client.h.
#pragma once

//  D2Client 0x6FBAB9B8
//  Unadjusted Y origin of the world viewport; SCROLL_GetViewportOriginY returns it minus a literal 8.
inline int32_t gnViewportOriginY;
static_assert(sizeof(gnViewportOriginY) == 4, "gnViewportOriginY: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB9BC
//  Unadjusted X origin of the world viewport; SCROLL_GetViewportOriginX returns it minus gUiPlayerPosAdjust.
inline int32_t gnViewportOriginX;
static_assert(sizeof(gnViewportOriginX) == 4, "gnViewportOriginX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB9C0
//  Horizontal nudge recomputed every frame by SCROLL_UpdateViewportOrigin from gUiShiftScreen: 2 -> gnScreenWidth / 2 - 160, 3 -> 0,
//  anything else -> -160 (the default, written first). Three stores, no reader: nothing applies it.
inline int32_t gnUiShiftOffsetX;
static_assert(sizeof(gnUiShiftOffsetX) == 4, "gnUiShiftOffsetX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB9C4
//  The current D2C_UIpanels mode, stored by SCROLL_SetWindowMode as passed in. SCROLL_GetUiShiftScreen returns it, and
//  WNDPROC_SetResolutionMode feeds it back into SCROLL_SetWindowMode, which re-applies the panel shift after a resolution change.
inline D2C_UIpanels gUiShiftScreen;
static_assert(sizeof(gUiShiftScreen) == 4, "gUiShiftScreen: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB9C8
//  How far the view matrix is slid to keep the player visible past an open side panel: 0, or +/- (gnScreenWidth / 4). Negative for
//  PANEL_RIGHT, positive for PANEL_LEFT, zero for PANEL_NONE and PANEL_BOTH. Written only by SCROLL_SetWindowMode, which passes the same
//  value to D2GFX_SetViewMatrixAdjust.
inline int32_t gUiPlayerPosAdjust;
static_assert(sizeof(gUiPlayerPosAdjust) == 4, "gUiPlayerPosAdjust: the item's 4 bytes in the .i64");

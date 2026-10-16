//  Wall2.h -- the globals of Wall2.cpp's data, read and written by Wall2.cpp alone: the edge-test mode, the player's coord-list index and tile,
//  and the clip rect. Address range 0x6FB77264..0x6FBAF90C.
//  Each is a C++17 inline variable, so every file that includes this header shares one object. The globals from 0x6FBAF900 on lie past the end of
//  .data's raw data (0x6FB9B000): the loader zero-fills them, and ours are zero-initialised.
//  gnPlayerTileX and gnPlayerTileY are unsigned (each is stored by an unsigned divide by five); their readers compare them signed, and Wall2.cpp
//  casts at each read.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after DLLmain.h and D2Client.h.
#pragma once

//  D2Client 0x6FB77264
//  Selects which of WALL2_WallIsPlayerBehindTile's two implementations runs. It is 1 in the image and nothing writes it, so mode 0 -- the geometric
//  branch, a one-sided 20-subtile band keyed on the tile's nTileType -- is dead code.
inline int32_t gnWallEdgeTestMode = 1;
static_assert(sizeof(gnWallEdgeTestMode) == 4, "gnWallEdgeTestMode: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF900
//  The nIndex of the player's D2RoomCoordListStrc box, stored by WALL2_WallBuildSquareGrid from D2Common #10095 (an index, not a room);
//  WALL2_WallIsPlayerBehindTile compares it with a wall tile's box's nIndex (+0x28).
inline int32_t gnPlayerCoordListIndex;
static_assert(sizeof(gnPlayerCoordListIndex) == 4, "gnPlayerCoordListIndex: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF904
//  The player's subtile Y divided by 5; see gnPlayerTileX.
inline uint32_t gnPlayerTileY;
static_assert(sizeof(gnPlayerTileY) == 4, "gnPlayerTileY: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF908
//  The player's subtile X divided by 5 -- the tile the player stands on -- cached once per frame by WALL2_WallBuildSquareGrid with gnPlayerTileY.
inline uint32_t gnPlayerTileX;
static_assert(sizeof(gnPlayerTileX) == 4, "gnPlayerTileX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAF90C
//  Left, top, right, bottom, set once per frame by WALL2_WallBuildSquareGrid from the viewport: under perspective inflated by 48 pixels on the left,
//  top and right, otherwise the viewport exactly. It is written by four dword stores and never read; the rect the room callback culls with is the
//  argument block's own.
inline RECT gWallClipRect;
static_assert(sizeof(gWallClipRect) == 16, "gWallClipRect: the item's 16 bytes in the .i64");

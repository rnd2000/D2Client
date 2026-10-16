//  dWall.h -- the globals of dWall.cpp that are ours: the tile-light tables used by DWALL_EnvDrawRoomTilesLit and DWALL_DrawRoomWalls, and the
//  assert expression of DWALL_DrawRoomWalls. Each is a C++17 `inline` variable, one object for every file that includes this header.
//  Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB6E160
//  How many light samples a tile needs, indexed by the tile's light direction (the first field of the tile record the draw node points at,
//  D2CMP's D2TileLibraryEntryStrc nLightDirection; it also picks the row of the two offset tables below). Read by DWALL_EnvDrawRoomTilesLit
//  and DWALL_DrawRoomWalls; it bounds the loop that fills the per-corner light array (eight dwords on the stack). The counts are 0 or 6.
inline const int32_t gaTileLightCornerCount[10] = { 0, 6, 6, 6, 6, 6, 6, 6, 6, 6 };
static_assert(sizeof(gaTileLightCornerCount) == 40, "gaTileLightCornerCount: the item's 40 bytes in the .i64");

//  D2Client 0x6FB74D60
//  The (x, y) light sample offsets per light direction, six pairs each, multiplied by 8 and added to the tile's position before
//  DLIGHTMAP_GetLightAtPoint. Selected when bit 0 of the flag byte at record+0x24 is clear; gaTileLightOffsetsAlt is the other half.
//  What that bit distinguishes is NOT ESTABLISHED.
inline int32_t gaTileLightOffsets[10][6][2] =
{
    {  },   // [0]
    { { 2, 5 }, { 2, 3 }, { 2, 1 }, { 2, -1 }, { 2, -3 }, { 2, -5 } },   // [1]
    { { -5, 2 }, { -3, 2 }, { -1, 2 }, { 1, 2 }, { 3, 2 }, { 5, 2 } },   // [2]
    { { 2, 5 }, { 2, 3 }, { 2, 2 }, { 2, 2 }, { 3, 2 }, { 5, 2 } },   // [3]
    { { -5, 2 }, { -3, 2 }, { -1, 2 }, { 2, -1 }, { 2, -3 }, { 2, -5 } },   // [4]
    {  },   // [5]
    { { 2, 5 }, { 2, 3 }, { 2, 1 }, { 2, -1 }, { 2, -3 }, { 2, -5 } },   // [6]
    { { -5, 2 }, { -3, 2 }, { -1, 2 }, { 1, 2 }, { 3, 2 }, { 5, 2 } },   // [7]
    { { 2, 5 }, { 2, 3 }, { 2, 2 }, { 2, 2 }, { 3, 2 }, { 5, 2 } },   // [8]
    { { -5, 2 }, { -3, 2 }, { -1, 2 }, { 2, -1 }, { 2, -3 }, { 2, -5 } },   // [9]
};
static_assert(sizeof(gaTileLightOffsets) == 480, "gaTileLightOffsets: the item's 480 bytes in the .i64");

//  D2Client 0x6FB74F40
//  The alternate sample offsets, selected when bit 0 of the flag byte at record+0x24 is set; the same shape as gaTileLightOffsets.
inline int32_t gaTileLightOffsetsAlt[10][6][2] =
{
    {  },   // [0]
    { { -2, 5 }, { -2, 3 }, { -2, 1 }, { -2, -1 }, { -2, -3 }, { -2, -5 } },   // [1]
    { { -5, -2 }, { -3, -2 }, { -1, -2 }, { 1, -2 }, { 3, -2 }, { 5, -2 } },   // [2]
    { { -2, 5 }, { -2, 3 }, { -2, -2 }, { -2, -2 }, { 3, -2 }, { 5, -2 } },   // [3]
    { { -5, -2 }, { -3, -2 }, { -1, -2 }, { -2, -1 }, { -2, -3 }, { -2, -5 } },   // [4]
    {  },   // [5]
    { { -2, 5 }, { -2, 3 }, { -2, 1 }, { -2, -1 }, { -2, -3 }, { -2, -5 } },   // [6]
    { { -5, -2 }, { -3, -2 }, { -1, -2 }, { 1, -2 }, { 3, -2 }, { 5, -2 } },   // [7]
    { { -2, 5 }, { -2, 3 }, { -2, -2 }, { -2, -2 }, { 3, -2 }, { 5, -2 } },   // [8]
    { { -5, -2 }, { -3, -2 }, { -1, -2 }, { -2, -1 }, { -2, -3 }, { -2, -5 } },   // [9]
};
static_assert(sizeof(gaTileLightOffsetsAlt) == 480, "gaTileLightOffsetsAlt: the item's 480 bytes in the .i64");

//  D2Client 0x6FB75140
//  The expression of the assert in DWALL_DrawRoomWalls (line 243): its parameter fOnlyDrawShadowLayer is a boolean, checked against 0 and 1.
inline char gszAssertOnlyDrawShadowLayer[55] = "fOnlyDrawShadowLayer == 0 || fOnlyDrawShadowLayer == 1";
static_assert(sizeof(gszAssertOnlyDrawShadowLayer) == 55, "gszAssertOnlyDrawShadowLayer: the item's 55 bytes in the .i64");

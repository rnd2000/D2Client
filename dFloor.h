//  dFloor.h -- the globals of dFloor.cpp that are ours: the pooled literal gfConst1000 and the draw context gFloorDrawCtx. Each is a C++17
//  `inline` variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above a global gives its
//  address in the original. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB6E15C
//  The pooled literal 1000.0, read by DFLOOR_DrawRoomFloors and by SOUNDHDR_GetFalloffDistance (as a switch case value there).
inline const float gfConst1000 = 1000.0f;
static_assert(sizeof(gfConst1000) == 4, "gfConst1000: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9A990
//  dFloor.cpp's `this`, 72 bytes, zero-initialised: the settings block DFLOOR_DrawRoomFloors fills at entry and the file's other two drawing
//  entry points read; all three are __thiscall with it in ecx.
inline D2FloorDrawCtxStrc gFloorDrawCtx;
static_assert(sizeof(gFloorDrawCtx) == 72, "gFloorDrawCtx: the item's 72 bytes in the .i64");

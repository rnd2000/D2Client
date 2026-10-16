//  SkillsPal.h -- the globals of SkillsPal.cpp's data that are ours (0x6FB6EDC0 and 0x6FB6EDE0), read by SkillsPal.cpp alone: the eight offsets
//  SKILLS_CltDoFunc34 uses to place an effect around a unit. Each is a C++17 `inline` variable, one object for every file that includes this
//  header. The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after DLLmain.h and D2Client.h.
#pragma once

//  D2Client 0x6FB6EDC0
//  The X offsets 0, -1, -2, -1, 0, 1, 2, 1 that SKILLS_CltDoFunc34 adds to the unit's client X (with the Y table, the eight points of |x| + |y| = 2).
inline const int32_t gnCltEffect34OffsetX[8] = { 0, -1, -2, -1, 0, 1, 2, 1 };
static_assert(sizeof(gnCltEffect34OffsetX) == 32, "gnCltEffect34OffsetX: the item's 32 bytes in the .i64");

//  D2Client 0x6FB6EDE0
//  The Y offsets 2, 1, 0, -1, -2, -1, 0, 1, the twin of gnCltEffect34OffsetX.
inline const int32_t gnCltEffect34OffsetY[8] = { 2, 1, 0, -1, -2, -1, 0, 1 };
static_assert(sizeof(gnCltEffect34OffsetY) == 32, "gnCltEffect34OffsetY: the item's 32 bytes in the .i64");

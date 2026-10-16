//  SkillsBar.h -- the global of SkillsBar.cpp's data that is ours (0x6FB6E9C4), read by SkillsBar.cpp alone: the leap arc table. It is a C++17
//  `inline` variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above it gives its address in the
//  original. Include it after DLLmain.h and D2Client.h.
#pragma once

//  D2Client 0x6FB6E9C4
//  Leap launch-speed bands { maxDistance, slope, intercept }, read by SKILLS_StartLeapArc: speed = slope * distance + intercept, floored at 500.
inline const D2LeapArcBandStrc gLeapArcTable[2] =
{ { .nMaxDistance = 27, .nSlope = -270, .nIntercept = 8462 }, { .nMaxDistance = 36, .nSlope = -72, .nIntercept = 1224 } };
static_assert(sizeof(gLeapArcTable) == 24, "gLeapArcTable: the item's 24 bytes in the .i64");

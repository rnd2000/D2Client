//  SkillsEMon.h -- the globals of SkillsEMon.cpp's data that are ours, read by SkillsEMon.cpp alone: gnImpInfernoSpread (const int[6])
//  and the two sign tables of the eight unit directions, gnDir8SignX and gnDir8SignY (const int[8]), all in .rdata at
//  0x6FB6EA28..0x6FB6EA7F.
//  Each is a C++17 inline variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above a
//  global gives its address in the original.
//  Include it after DLLmain.h and D2Client.h.
#pragma once

//  D2Client 0x6FB6EA28
//  -2, -1, 0, 1, 2, 0: a velocity spread added to the missile's own missiles.txt nVel by SKILLS_CltDoFunc71 (Imp Inferno). The index is
//  the unit's next seed modulo 5, so only entries 0..4 are reachable; the trailing 0 is the sixth element.
inline const int32_t gnImpInfernoSpread[6] = { -2, -1, 0, 1, 2 };
static_assert(sizeof(gnImpInfernoSpread) == 24, "gnImpInfernoSpread: the item's 24 bytes in the .i64");

//  D2Client 0x6FB6EA40
//  X of eight unit directions, a sign table (the four axes first, then the four diagonals, at unit scale); with gnDir8SignY the pairs are
//  (0,1), (0,-1), (1,0), (-1,0), (1,1), (-1,-1), (-1,1), (1,-1). Read by SKILLS_CltStFunc49.
inline const int32_t gnDir8SignX[8] = { 0, 0, 1, -1, 1, -1, -1, 1 };
static_assert(sizeof(gnDir8SignX) == 32, "gnDir8SignX: the item's 32 bytes in the .i64");

//  D2Client 0x6FB6EA60
//  Y of the eight unit directions, the twin of gnDir8SignX.
inline const int32_t gnDir8SignY[8] = { 1, -1, 0, 0, 1, -1, 1, -1 };
static_assert(sizeof(gnDir8SignY) == 32, "gnDir8SignY: the item's 32 bytes in the .i64");

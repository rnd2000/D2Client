//  SkillsDru.h -- the globals of SkillsDru.cpp: gnDir8CircleX and gnDir8CircleY (0x6FB6E9E0, 0x6FB6EA00), the offsets of the eight points of an
//  octagon of radius 7, read by SKILLS_StatesSpawnHurricaneCltMissile. Each is a C++17 inline variable, so every file that includes this
//  header shares one object; both are const (.rdata).
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after DLLmain.h and D2Client.h.
#pragma once

//  D2Client 0x6FB6E9E0
//  The X of an octagon of radius 7: {7, 5, 0, -5, -7, -5, 0, 5}, index for index the same order of signs as gnDir8StepX but seven subtiles out
//  instead of two. Paired with gnDir8CircleY. One user: SKILLS_StatesSpawnHurricaneCltMissile, which indexes it with SEED_RollRandomNumber & 7.
inline const int32_t gnDir8CircleX[8] = { 7, 5, 0, -5, -7, -5, 0, 5 };
static_assert(sizeof(gnDir8CircleX) == 32, "gnDir8CircleX: the item's 32 bytes in the .i64");

//  D2Client 0x6FB6EA00
//  The Y half: {0, 5, 7, 5, 0, -5, -7, -5}. See gnDir8CircleX.
inline const int32_t gnDir8CircleY[8] = { 0, 5, 7, 5, 0, -5, -7, -5 };
static_assert(sizeof(gnDir8CircleY) == 32, "gnDir8CircleY: the item's 32 bytes in the .i64");

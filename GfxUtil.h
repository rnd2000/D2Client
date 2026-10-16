//  GfxUtil.h -- the globals of GfxUtil.cpp's data that are ours: the five token tables GFXUTIL_CacheTokens fills from the excel tables.
//  Each is a C++17 inline variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above a global
//  gives its address in the original.
//  Include it after D2Client.h.
#pragma once

//  D2Client 0x6FBC2FB0
//  The three-character animation-mode tokens of monmode.txt, one dword each, 16 entries, filled by GFXUTIL_CacheTokens.
inline int32_t gaMonModeTokens[16];
static_assert(sizeof(gaMonModeTokens) == 64, "gaMonModeTokens: the item's 64 bytes in the .i64");

//  D2Client 0x6FBC2FF0
//  The component tokens of composit.txt, one dword each, 16 entries, filled by GFXUTIL_CacheTokens.
inline int32_t gaCompositTokens[16];
static_assert(sizeof(gaCompositTokens) == 64, "gaCompositTokens: the item's 64 bytes in the .i64");

//  D2Client 0x6FBC3030
//  The animation-mode tokens of objmode.txt, one dword each, 8 entries, filled by GFXUTIL_CacheTokens.
inline int32_t gaObjModeTokens[8];
static_assert(sizeof(gaObjModeTokens) == 32, "gaObjModeTokens: the item's 32 bytes in the .i64");

//  D2Client 0x6FBC3050
//  The four-character token of each armtype.txt row, 3 entries, cached by GFXUTIL_CacheTokens at the start of every game.
inline int32_t gaArmTypeTokens[3];
static_assert(sizeof(gaArmTypeTokens) == 12, "gaArmTypeTokens: the item's 12 bytes in the .i64");

//  D2Client 0x6FBC3060
//  The animation-mode tokens of plrmode.txt, one dword each, 20 entries, filled by GFXUTIL_CacheTokens.
inline int32_t gaPlrModeTokens[20];
static_assert(sizeof(gaPlrModeTokens) == 80, "gaPlrModeTokens: the item's 80 bytes in the .i64");

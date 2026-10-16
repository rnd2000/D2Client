//  Item.h -- the globals of Item.cpp that are ours: the item cursors a skill takes and their count (0x6FB90C28..0x6FB90C33), and the weapon
//  classes cached for the two arms (0x6FBBC208..0x6FBBC20B, zero-initialised).
//  Each is a C++17 `inline` variable, so every file that includes this header shares one object. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB90C28
//  gaItemCursors: the item cursors a skill takes. Read-only; ITEM_OnCursorItemSkill reads it.
inline int32_t gaItemCursors[2] = { 0, 2 };
static_assert(sizeof(gaItemCursors) == 8, "gaItemCursors: the item's 8 bytes in the .i64");

//  D2Client 0x6FB90C30
//  gnItemCursorUIStates: the number of entries of gaItemCursors that ITEM_OnCursorItemSkill scans. Read-only.
inline int32_t gnItemCursorUIStates = 2;
static_assert(sizeof(gnItemCursorUIStates) == 4, "gnItemCursorUIStates: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBC208
//  The WeaponClass id of whatever is equipped in BODYLOC_RARM. Written by the equip, swap, unequip, weapon-switch and stat-update item messages
//  and by ITEM_CacheWeaponClassForBodyLoc and ITEM_ClearWeaponClassCache; nothing in D2Client reads it.
inline uint16_t gnWeaponClassRArm;
static_assert(sizeof(gnWeaponClassRArm) == 2, "gnWeaponClassRArm: the item's 2 bytes in the .i64");

//  D2Client 0x6FBBC20A
//  The WeaponClass id of whatever is equipped in BODYLOC_LARM, the other half of the pair: ITEM_CacheWeaponClassForBodyLoc routes body location 4
//  to gnWeaponClassRArm and 5 here. Written by the same call sites and, like gnWeaponClassRArm, never read in D2Client.
inline uint16_t gnWeaponClassLArm;
static_assert(sizeof(gnWeaponClassLArm) == 2, "gnWeaponClassLArm: the item's 2 bytes in the .i64");

//  Select.h -- the globals of Select.cpp: the enemy-probe tables, the character panel's reference monsters, the selection state and the click
//  region. Address range 0x6FB6E27C..0x6FBAB9F8. Each is a C++17 inline variable, so every file that includes this header shares one object.
//  The four probe tables are const (.rdata). The globals from 0x6FBAB9CC on lie past the end of .data's raw data (0x6FB9B000): the loader
//  zero-fills them, and ours are zero-initialised.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after DLLmain.h and D2Client.h:
//  gpLastSelectionRoom's type is D2Structs.h's, which DLLmain.h includes.
#pragma once

//  D2Client 0x6FB6E27C
//  The X of an octagon of reach 3 subtiles, {0, -2, -3, -2, 0, 2, 3, 2}, paired with gnDir8ProbeY, which begins where this ends.
inline const int32_t gnDir8ProbeX[8] = { 0, -2, -3, -2, 0, 2, 3, 2 };
static_assert(sizeof(gnDir8ProbeX) == 32, "gnDir8ProbeX: the item's 32 bytes in the .i64");

//  D2Client 0x6FB6E29C
//  The Y half of the reach-3 octagon, {3, 2, 0, -2, -3, -2, 0, 2}; ends where gnTargetProbeOrder begins.
inline const int32_t gnDir8ProbeY[8] = { 3, 2, 0, -2, -3, -2, 0, 2 };
static_assert(sizeof(gnDir8ProbeY) == 32, "gnDir8ProbeY: the item's 32 bytes in the .i64");

//  D2Client 0x6FB6E2BC
//  The probe order SELECT_SkillsFindEnemyInFrontOfUnit fans out from the facing: straight ahead, then one step each way, then two. Five entries.
inline const int32_t gnTargetProbeOrder[5] = { 0, -1, 1, -2, 2 };
static_assert(sizeof(gnTargetProbeOrder) == 20, "gnTargetProbeOrder: the item's 20 bytes in the .i64");

//  D2Client 0x6FB6E2D0
//  The eight bytes after gnTargetProbeOrder, {-3, 3}, read by nothing. The original's loop bound is this address; ours writes
//  gnTargetProbeOrder + 5.
inline const int32_t gnTargetProbeOrderEnd[2] = { -3, 3 };
static_assert(sizeof(gnTargetProbeOrderEnd) == 8, "gnTargetProbeOrderEnd: the item's 8 bytes in the .i64");

//  D2Client 0x6FB77080
//  Class id (monstats index) of the monster the character panel quotes for the player's chance to hit. Set by SELECT_ApplySelectedUnit, reset to
//  19 (MONSTER_FALLEN1) by SELECT_ResetRefMonsters.
inline int32_t gnToHitRefMonster = 19;
static_assert(sizeof(gnToHitRefMonster) == 4, "gnToHitRefMonster: the item's 4 bytes in the .i64");

//  D2Client 0x6FB77084
//  Class id of the monster the character panel quotes for the chance to be hit. Set under a narrower condition than gnToHitRefMonster (one extra
//  monstats2 flag bit must be clear); reset to 19 the same way.
inline int32_t gnToBeHitRefMonster = 19;
static_assert(sizeof(gnToBeHitRefMonster) == 4, "gnToBeHitRefMonster: the item's 4 bytes in the .i64");

//  D2Client 0x6FB77088
//  Selection priority by unit type, indexed by dwUnitType (UNIT_PLAYER..UNIT_TILE, six entries); SELECT_UpdateSelectedUnit prefers the higher.
inline int32_t gnUnitTypeSelectPriority[6] = { 0, 0, -6, 0, -4 };
static_assert(sizeof(gnUnitTypeSelectPriority) == 24, "gnUnitTypeSelectPriority: the item's 24 bytes in the .i64");

//  D2Client 0x6FBAB9CC
//  Type of the selected unit (a D2C_UnitTypes), paired with gnSelectedUnitGUID.
inline int32_t gnSelectedUnitType;
static_assert(sizeof(gnSelectedUnitType) == 4, "gnSelectedUnitType: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB9D0
//  GUID of the selected unit; the unit is re-found by type and GUID on every SELECT_GetSelectedUnit call, never held as a pointer.
inline int32_t gnSelectedUnitGUID;
static_assert(sizeof(gnSelectedUnitGUID) == 4, "gnSelectedUnitGUID: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB9D4
//  Left edge of the click region SELECT_SetClickRegion stores; while gbClickRegionValid is set, SELECT_UpdateSelectedUnit selects nothing under a
//  cursor inside the rectangle (all four edges exclusive).
inline int32_t gnClickRegionLeft;
static_assert(sizeof(gnClickRegionLeft) == 4, "gnClickRegionLeft: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB9D8
//  Top edge of the click region.
inline int32_t gnClickRegionTop;
static_assert(sizeof(gnClickRegionTop) == 4, "gnClickRegionTop: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB9DC
//  Right edge of the click region.
inline int32_t gnClickRegionRight;
static_assert(sizeof(gnClickRegionRight) == 4, "gnClickRegionRight: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB9E0
//  Bottom edge of the click region.
inline int32_t gnClickRegionBottom;
static_assert(sizeof(gnClickRegionBottom) == 4, "gnClickRegionBottom: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB9E4
//  Toggled by the /nopickup chat command; when set, clicking an item on the ground does not pick it up. SELECT_UpdateSelectedUnit enforces it.
inline int32_t gbNoPickup;
static_assert(sizeof(gbNoPickup) == 4, "gbNoPickup: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB9E8
//  Whether a unit is selected -- under the cursor, or chosen explicitly by SELECT_SetSelectedUnit. Cleared by SELECT_GetSelectedUnit when the
//  stored GUID no longer resolves or the unit no longer passes CUNIT_HitTestUnit(pUnit, 0, 0, 0).
inline int32_t gbHasSelectedUnit;
static_assert(sizeof(gbHasSelectedUnit) == 4, "gbHasSelectedUnit: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB9EC
//  A lock: while set, SELECT_ApplySelectedUnit and SELECT_UpdateSelectedUnit return at once. Set by SELECT_SetSelectedUnit once an explicit
//  selection is applied, and for NULL so that "nothing selected" sticks. Cleared by SELECT_UnlockSelection (input events), SELECT_ClearSelectedUnit,
//  SELECT_GetSelectedUnit, SELECT_SetSelectedUnit and SELECT_UpdateSelectedUnit.
inline int32_t gbSelectionLocked;
static_assert(sizeof(gbSelectionLocked) == 4, "gbSelectionLocked: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB9F0
//  Whether gnClickRegionLeft..gnClickRegionBottom describe a live region; SELECT_SetClickRegion sets it with a rect and clears it with NULL.
inline int32_t gbClickRegionValid;
static_assert(sizeof(gbClickRegionValid) == 4, "gbClickRegionValid: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB9F4
//  A latch: both of its stores write 1 and nothing clears it, so it is set on the first frame the player has a room (gpLastSelectionRoom starts
//  NULL) and stays set. The arm of SELECT_UpdateSelectedUnit that tests it for 0 and arms the interact throttle never runs.
inline int32_t gbSelectionRoomSeen;
static_assert(sizeof(gbSelectionRoomSeen) == 4, "gbSelectionRoomSeen: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB9F8
//  The player's room as of the last SELECT_UpdateSelectedUnit call that reaches the compare (the early gates return first); overwritten right after.
inline D2ActiveRoomStrc *gpLastSelectionRoom;
static_assert(sizeof(gpLastSelectionRoom) == 4, "gpLastSelectionRoom: the item's 4 bytes in the .i64");

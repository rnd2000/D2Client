//  CUnit.h -- the globals of CUnit.cpp that are ours: the skill select-proc table, the anim-mode sync classes, the unit destroy order, the client
//  unit id counter, the two unit hash tables, the local player's unit and the last period of day. Each is a C++17 inline variable, so every file
//  that includes this header shares one object. Include it after D2Client.h.
//  The two unit tables are [6][128], indexed [nType][nBucket]. DIFFERENCE: the original indexes them past their six rows for a type of 6 or more,
//  into the globals after them; ours would read past the array, which C++ leaves undefined. CUNIT_FindUnitByGuidAndType,
//  CUNIT_FindUnitByClientIdAndType, CUNIT_RemoveUnitById, CUNIT_RemoveUnitByClientId and CUNIT_RegisterUnit refuse a type of 6 or more
//  (a difference declared at each).
#pragma once

//  D2Client 0x6FB90900
//  The dispatch table indexed by skills.txt's SelectProc column: entry 0 is NULL, 1..6 are corpse tests. Count in gnSkillSelectProcCount.
inline int32_t (__fastcall *gpSkillSelectProcs[7])(D2UnitStrc *, D2UnitStrc *) =
{
    NULL,   // [0]
    &CUNIT_SkillsSelectProc1_CorpseExplosion,   // [1]
    &CUNIT_SkillsSelectProc2_RaiseSkeleton,   // [2]
    &CUNIT_SkillsSelectProc3_Revive,   // [3]
    &CUNIT_SkillsSelectProc4_FindPotion,   // [4]
    &CUNIT_SkillsSelectProc5_FindItem,   // [5]
    &CUNIT_SkillsSelectProc6_GrimWard,   // [6]
};
static_assert(sizeof(gpSkillSelectProcs) == 28, "gpSkillSelectProcs: the item's 28 bytes in the .i64");

//  D2Client 0x6FB2A6C0
//  How CUNIT_ResyncUnitFromServer reconciles a player's anim mode with the server's: indexed by the server's mode minus one, 18 entries.
inline const uint8_t gnAnimModeSyncClass[18] = { 0, 1, 2, 4, 0, 1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3 };
static_assert(sizeof(gnAnimModeSyncClass) == 18, "gnAnimModeSyncClass: the item's 18 bytes in the .i64");

//  D2Client 0x6FB70148
//  The order CUNIT_DestroyAllUnits walks the unit types in: monsters first, then players, objects, missiles, items, room tiles.
inline const int32_t gUnitTypeDestroyOrder[6] = { 1, 0, 2, 3, 4, 5 };
static_assert(sizeof(gUnitTypeDestroyOrder) == 24, "gUnitTypeDestroyOrder: the item's 24 bytes in the .i64");

//  D2Client 0x6FB908FC
//  The client's own unit-id allocator (CUNIT_CreateClientOnlyUnit); wraps at 0xFFFFFFFE back to 0, so 0xFFFFFFFF is never issued.
inline uint32_t gdwNextClientUnitId = 1;
static_assert(sizeof(gdwNextClientUnitId) == 4, "gdwNextClientUnitId: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9091C
//  The number of entries in gpSkillSelectProcs.
inline int32_t gnSkillSelectProcCount = 7;
static_assert(sizeof(gnSkillSelectProcCount) == 4, "gnSkillSelectProcCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBAA00
//  The unit hash table keyed by the server's id: [D2C_UnitTypes][dwUnitId & 0x7F], chained on pListNext, sorted by descending dwUnitId.
inline D2UnitStrc *gpUnitTableById[6][128];
static_assert(sizeof(gpUnitTableById) == 3072, "gpUnitTableById: the item's 3072 bytes in the .i64");

//  D2Client 0x6FBBB600
//  The same shape keyed by the client id, for the units the client invented (dwFlags bit 21, UNITFLAG_ISASYNC, set).
inline D2UnitStrc *gpUnitTableByClientId[6][128];
static_assert(sizeof(gpUnitTableByClientId) == 3072, "gpUnitTableByClientId: the item's 3072 bytes in the .i64");

//  D2Client 0x6FBBC200
//  The local player's unit, written only by CUNIT_SetPlayerUnit.
inline D2UnitStrc *gpPlayerUnit;
static_assert(sizeof(gpPlayerUnit) == 4, "gpPlayerUnit: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBC204
//  The period of day the client last acted on; CUNIT_RefreshObjectsOnDayNightChange does nothing unless it moved.
inline int32_t gnLastPeriodOfDay;
static_assert(sizeof(gnLastPeriodOfDay) == 4, "gnLastPeriodOfDay: the item's 4 bytes in the .i64");

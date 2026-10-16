#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "SkillsBar.h"

//  SkillsBar.cpp -- the Barbarian's client skill code of D2Client.dll 1.10: six `cltstfunc` start functions, eight `cltdofunc` do functions and
//  eight helpers (the leap and the charge, the collision footprint a leap lifts and puts back, the spin copies Whirlwind draws, and Whirlwind's
//  sound).
//  The original source file is Skills\SkillsBar.cpp (the asserts of four of its functions name it). Address range 0x6FAC0D30..0x6FAC2836.
//  The skills served (stock Skills.txt): cltstfunc 26 Find Potion and Grim Ward, 27 Double Swing, 28 Find Item, 29 Leap, 30 Leap Attack, 31
//  Whirlwind; cltdofunc 38 Find Potion, 39 Double Swing and Frenzy, 40 Find Item, 41 Grim Ward, 42 Double Throw, 43 Leap, 44 Leap Attack, 45
//  Whirlwind. Double Throw starts through cltstfunc 11; Frenzy has no cltstfunc.

static_assert(offsetof(D2SkillsTxt, dwCltCalc) == 0x114,
              "SKILLS_CltStFunc27: `mov edx, [eax+114h]` (0x6FAC0DC7), `cltcalc1`");
static_assert(MONSTATS2FLAGINDEX_SMALL == 10 && MONSTATS2FLAGINDEX_LARGE == 11,
              "SKILLS_CltDoFunc41: monstats2 byte 1 (`mov cl, [eax+5]`, 0x6FAC1265) against gdwBitMasks[3] "
              "(0x6FAC1270) and then [2] (0x6FAC127E)");
static_assert(MISSILE_CORPSEEXPLOSION == 115,
              "SKILLS_CltDoFunc41: `mov [esp+8Ch+var_4C], 73h` (0x6FAC12EF), the second spawn");


//1.10f: D2Client.0x6FAC0D30
//  `cltstfunc` 26, Find Potion or Grim Ward can start. It reads neither nSkillId nor nSkillLevel. Without a path target it answers the NULL itself;
//  otherwise D2Common's #11018 made 0 or 1 by neg / sbb / neg: a corpse the skill may use (a monster in mode MONMODE_DEAD, in no state of the
//  `udead` state mask, whose monstats2 row sets CORPSESEL and which UNITS_IsSoftMonster calls soft). The client tests no corpse state.
BOOL __fastcall SKILLS_CltStFunc26(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2UnitStrc *pTarget;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget)
        return FALSE;

    return D2COMMON_11018(pTarget) != 0;
}


//1.10f: D2Client.0x6FAC0D50
//  `cltdofunc` 38, Find Potion bleeds the corpse. Without a path target it answers 0; otherwise it sets UNITFLAG_SKSRVDOFUNC on a non-NULL unit,
//  bleeds five drops from the target (MONSTER_SpawnBlood, 5 in edx, bByChance 0) and answers 1. It does not ask whether the corpse can bleed; Find
//  Item's do (SKILLS_CltDoFunc40) does.
int32_t __fastcall SKILLS_CltDoFunc38(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2UnitStrc *pTarget;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget)
        return 0;

    //  BUG: this NULL test guards only the flag: pUnit has already gone to CUNIT_GetPathTargetUnit, which reads pUnit->pDynamicPath (test esi, esi
    //  at 0x6FAC0D60). UNREACHABLE: SKILLS_RunClientDoFunc, the one dispatcher of these slots, cannot pass NULL.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    MONSTER_SpawnBlood(pTarget, 5, 0);
    return 1;
}


//1.10f: D2Client.0x6FAC0D90
//  `cltstfunc` 27, Double Swing starts. It answers 0 without the skill's record; otherwise it hands the row's `cltcalc1` (mov edx, [eax+114h] at
//  0x6FAC0DC7; stock Double Swing's is `par5`), evaluated, to SKILLS_ApplyAttackRateStat (a temporary STAT_ATTACKRATE on the unit and its animation
//  rate recomputed) and answers 1. The original inlines DATATBLS_GetSkillsTxtRecordById here.
BOOL __fastcall SKILLS_CltStFunc27(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return FALSE;

    SKILLS_ApplyAttackRateStat(pUnit, D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[0], nSkillId, nSkillLevel));
    return TRUE;
}


//1.10f: D2Client.0x6FAC0DF0
//  `cltdofunc` 39, the swing of Double Swing and Frenzy; it answers 1 on every path. The swing's frame decides the target: dwSeqFrame with its low
//  byte cleared, compared signed against 600h (xor al, al / cmp eax, 600h / jge at 0x6FAC0DF8..0x6FAC0DFF), so frames 0..5 of the sequence are the
//  first swing and 6 on the second.
//    the first     the unit's hit-class sound (UNITSND_GetHitClassSound of D2Common's UNITS_GetHitClass), played with a delay of 4
//                  (SOUNDINST_PlaySound, stack arguments 4, 0, 0), and the path target;
//    the second    no sound, and the next unit after the path target by id within 3 (SKILLS_FindNextNearbyTargetById, mask 0, which that function
//                  turns into its default, and 0xFFFFFFFF as the id with no path target). The path target itself is not changed.
//  Either way, with a target and a dynamic path of the unit's own, the unit is turned to face the target's position (D2Common's PATH_FaceCoords).
//  The bytes carry that tail in each arm, the Y lookup twice and the X lookup's two exits shared; the C writes it once, after the arms. The
//  original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltDoFunc39(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2UnitStrc *pTarget;
    D2UnitStrc *pPathTarget;
    uint32_t dwAfterUnitId;
    int32_t nX;
    int32_t nY;

    if ((pUnit->dwSeqFrame & ~0xFF) < 0x600)
    {
        SOUNDINST_PlaySound(UNITSND_GetHitClassSound(pUnit, D2COMMON_UNITS_GetHitClass(pUnit)), pUnit, 4, 0, 0);
        pTarget = CUNIT_GetPathTargetUnit(pUnit);
    }
    else
    {
        pPathTarget = CUNIT_GetPathTargetUnit(pUnit);
        dwAfterUnitId = 0xFFFFFFFF;
        if (pPathTarget)
            dwAfterUnitId = pPathTarget->dwUnitId;

        pTarget = SKILLS_FindNextNearbyTargetById(pUnit, 0, 0, 3, (D2C_AuraFilters)0, dwAfterUnitId, NULL);
    }

    //  BUG: the unit's dynamic path is tested (0x6FAC0E33 and 0x6FAC0EC3, in both arms) after CUNIT_GetPathTargetUnit has asserted it and exited
    //  without one. UNREACHABLE: a unit without a path never gets here.
    if (!pTarget || !pUnit->pDynamicPath)
        return 1;

    nY = UNITS_GetYPosition(pTarget);

    nX = UNITS_GetXPosition(pTarget);

    D2COMMON_PATH_FaceCoords(pUnit->pDynamicPath, nX, nY, 0);
    return 1;
}


//1.10f: D2Client.0x6FAC0F40
//  `cltdofunc` 42, Double Throw throws. The item is D2Common's #10434 with its second argument 1 (0x6FAC0F46), the hand a skill uses, which for the
//  alternating weapon selection turns to the second hand from frame 6 on the client. Without one it answers 0. Then:
//    * ITEMS_CheckIfThrowable once, its answer not read;
//    * from frame 9 on (the same masked, signed compare, against 900h) UNITFLAG_SKSRVDOFUNC, the unit not tested for NULL here;
//    * ITEMS_CheckIfThrowable again, and an item it calls not throwable that carries STAT_ITEM_THROWABLE is refused (0x6FAC0F7E..0x6FAC0F8B); the
//      layer pushed is eax, which is 0 on that path;
//    * the item's missile, refused unless above 0 and below the missiles count (jle, jge);
//    * a throwing potion (ITEMTYPE_MISSILE_POTION) goes through MISSILE_ClientSpawnSkillEffect with no offset and no target point, whose NULL
//      answer the function answers 0 for; anything else is thrown at the target by SKILLS_MissileClientSpawnAimedAtTargetUnit, whose answer is not
//      read. The answer is 1.
int32_t __fastcall SKILLS_CltDoFunc42(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2UnitStrc *pItem;
    int32_t nMissileId;

    pItem = D2COMMON_10434(pUnit, TRUE);
    if (!pItem)
        return 0;

    D2COMMON_ITEMS_CheckIfThrowable(pItem);

    if ((pUnit->dwSeqFrame & ~0xFF) >= 0x900)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    //  BUG: an item that is not throwable but carries STAT_ITEM_THROWABLE is refused, where SKILLS_HasAmmoForShot accepts an item for either.
    //  LATENT: no stock row uses the `throw` property, the one that grants the stat.
    if (!D2COMMON_ITEMS_CheckIfThrowable(pItem) && D2COMMON_STATLIST_UnitGetItemStatOrSkillStatValue(pItem, STAT_ITEM_THROWABLE, 0))
    {
        return 0;
    }

    nMissileId = D2COMMON_ITEMS_GetMissileType(pItem);
    if (nMissileId <= 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_MISSILE_POTION))
    {
        if (!MISSILE_ClientSpawnSkillEffect(pUnit, nMissileId, 0, 0, 0, 0, (int32_t)nSkillId, nSkillLevel, 0))
            return 0;
    }
    else
    {
        SKILLS_MissileClientSpawnAimedAtTargetUnit(pUnit, (D2C_MissilesHcIdx)nMissileId, nSkillId, nSkillLevel);
    }

    return 1;
}


//1.10f: D2Client.0x6FAC1000
//  `cltstfunc` 28, Find Item can start: `cltstfunc` 26's body with #11019 in place of #11018 (0x6FAC100D). It reads neither nSkillId nor
//  nSkillLevel. #11019 is D2Common's SKILLS_CanUnitCorpseBeSelected: #11018's tests without the soft one.
BOOL __fastcall SKILLS_CltStFunc28(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2UnitStrc *pTarget;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget)
        return FALSE;

    return D2COMMON_11019(pTarget) != 0;
}


//1.10f: D2Client.0x6FAC1020
//  `cltdofunc` 40, Find Item bleeds a soft corpse: `cltdofunc` 38's body with one more refusal, a target that UNITS_IsSoftMonster (#10359) does not
//  call soft answers 0 (the NULL target and the refusal each answer the value in eax, 0).
int32_t __fastcall SKILLS_CltDoFunc40(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2UnitStrc *pTarget;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget)
        return 0;

    if (!D2COMMON_UNITS_IsSoftMonster(pTarget))
        return 0;

    //  BUG: this NULL test guards only the flag, as in `cltdofunc` 38 (test esi, esi at 0x6FAC1043). UNREACHABLE: SKILLS_RunClientDoFunc, the one
    //  dispatcher of these slots, cannot pass NULL.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    MONSTER_SpawnBlood(pTarget, 5, 0);
    return 1;
}


//1.10f: D2Client.0x6FAC1070
//  `cltdofunc` 41, Grim Ward rises from the corpse. It answers 0 without the skill's record, without a path target, or when the target is not soft
//  (UNITS_IsSoftMonster); then it sets UNITFLAG_SKSRVDOFUNC on a non-NULL unit and places the totem:
//    * the target's position, X then Y, is the spawn point;
//    * the room holding it, the target's room or one adjacent to it, and 0 without one;
//    * D2Common's COLLISION_GetFreeCoordinates moves the point to a free spot for a unit of size 2 against mask 1000h, neighbour rooms allowed, and
//      the answer is 0 when there is none;
//    * a target that is not a monster answers 0, only here, after the room and the spot (cmp dword ptr [ebx], 1 at 0x6FAC1205);
//    * the missile is the row's `cltmissilea`, `cltmissilec` for a monster whose monstats2 row sets LARGE, `cltmissileb` for one that sets SMALL
//      (stock Grim Ward: grimwardmediumstart, grimwardlargestart, grimwardsmallstart), and a missile outside the table answers 0;
//    * two client missiles at the spot, the totem and then MISSILE_CORPSEEXPLOSION, from one record: flags 2001h, the unit as owner, the skill and
//      its level. The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition, UNITS_GetYPosition, CLIENT_FindRoomContainingSubtile and
//  DATATBLS_GetMonStats2TxtRecord here.
int32_t __fastcall SKILLS_CltDoFunc41(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pTarget;
    D2ActiveRoomStrc *pRoom;
    D2MonStats2Txt *pMonStats2Txt;
    D2CoordStrc tSpawnPoint;
    D2MissileStrc tParams;
    int32_t nMissileId;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget || !D2COMMON_UNITS_IsSoftMonster(pTarget))
        return 0;

    //  BUG: this NULL test guards only the flag, as in `cltdofunc` 38 (test esi, esi at 0x6FAC10DC). UNREACHABLE: SKILLS_RunClientDoFunc, the one
    //  dispatcher of these slots, cannot pass NULL.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    tSpawnPoint.nX = UNITS_GetXPosition(pTarget);

    tSpawnPoint.nY = UNITS_GetYPosition(pTarget);

    pRoom = CLIENT_FindRoomContainingSubtile(D2COMMON_UNITS_GetRoom(pTarget), tSpawnPoint.nX, tSpawnPoint.nY);
    if (!pRoom)
        return 0;

    if (!D2COMMON_COLLISION_GetFreeCoordinates(pRoom, &tSpawnPoint, 2, 0x1000, TRUE))
        return 0;

    if (pTarget->dwUnitType != UNIT_MONSTER)
        return 0;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;

    pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pTarget->dwClassId);
    if (!pMonStats2Txt)
        return 0;

    if (pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_LARGE >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_LARGE & 7])
        nMissileId = (int16_t)pSkillsTxt->wCltMissileC;
    else if (pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_SMALL >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_SMALL & 7])
        nMissileId = (int16_t)pSkillsTxt->wCltMissileB;

    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    memset(&tParams, 0, sizeof(tParams));
    tParams.pOwner = pUnit;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_RANDOMDIR;
    tParams.nMissile = nMissileId;
    tParams.nX = tSpawnPoint.nX;
    tParams.nY = tSpawnPoint.nY;
    MISSILE_ClientSpawn(&tParams);

    tParams.nMissile = MISSILE_CORPSEEXPLOSION;
    MISSILE_ClientSpawn(&tParams);
    return 1;
}


static_assert(COLLIDE_MASK_MONSTER_PATH == 0x3C01 && COLLIDE_PLAYER == 0x80 && COLLIDE_MASK_PLAYER_FLYING == 0x804,
              "SKILLS_CltStFunc29: `push 3C01h` (0x6FAC14B3) and `push 80h` (0x6FAC165D); "
              "SKILLS_ResolveClientTargetPoint: `push 804h` (0x6FAC1950)");
//  The source path the original passes as __FILE__ to the asserting calls of this unit.
static const char szSkillsBarCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\Skills\\SkillsBar.cpp";


//1.10f: D2Client.0x6FAC1310
//  `cltstfunc` 29, Leap starts. It answers FALSE without a used skill and, for the local player only (CUNIT_GetPlayer), while the unit is in
//  STATE_UNINTERRUPTABLE or when the used skill is already this one with bit 0 of its flags set (SKILLS_GetSkillIdFromSkill with this file's path
//  and line 442; the 1101h SKILLS_StartChargeMove sets once the leap is moving). Then the landing point:
//    a monster     with a path target, the target's position reflected through the unit's (twice the target's X and Y less the unit's), a landing
//                  as far past the target as the unit stands before it; without one, SKILLS_GetPathTargetPos's point, FALSE when there is none, and
//                  param4 set to -1. Either way D2Common's COLLISION_GetFreeCoordinates moves the point to a free spot for the unit's size against
//                  COLLIDE_MASK_MONSTER_PATH, neighbour rooms not allowed, and the answer is FALSE when there is none;
//    otherwise     SKILLS_GetPathTargetPos's point, FALSE when there is none; FALSE without the skill's record, without the unit's room, or without
//                  a room holding the point from it (a call to CLIENT_FindRoomContainingSubtile); FALSE when that room is in town and the row does
//                  not set `InTown` (a byte test, mov dl, [edi+5] / test [ecx], dl; stock Leap does not); then SKILLS_ResolveClientTargetPoint, and
//                  FALSE when it finds nothing.
//  With the point, the room holding it is looked up again from the unit's room, and the unit's collision pattern is marked there with
//  COLLIDE_PLAYER, for a monster as well. The point goes into param1 and param2; a monster's path target, when it still has one, into param3 and
//  param4 as its type and GUID; the flags become 1080h; SKILLS_PostSkillStatWithRemoveCallback (Skills.cpp) posts the stat list whose expiry ends
//  the skill; the answer is TRUE.
//  The mark's room is not tested: D2Common's COLLISION_SetMaskWithPattern hands it only to lookups and setters that answer a NULL room first, so a
//  point in no room leaves the mark unmade.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition, DATATBLS_GetSkillsTxtRecordById and, at the second room lookup,
//  CLIENT_FindRoomContainingSubtile here.
BOOL __fastcall SKILLS_CltStFunc29(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pTarget;
    D2ActiveRoomStrc *pRoom;
    D2CoordStrc tSpawnPoint;
    D2C_UnitTypes nUnitType;
    BOOL bIsMonster;
    int32_t nX;
    int32_t nY;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return FALSE;

    if (pUnit == CUNIT_GetPlayer())
    {
        if (D2COMMON_STATES_CheckState(pUnit, STATE_UNINTERRUPTABLE))
            return FALSE;

        if (D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillsBarCpp, 442) == (int32_t)nSkillId && (D2COMMON_SKILLS_GetFlags(pSkill) & 1))
        {
            return FALSE;
        }
    }

    //  BUG: pUnit is tested for NULL (0x6FAC1384) after UNITS_GetUsedSkill has been handed it, and D2Common asserts `hUnit` and exits on NULL.
    //  UNREACHABLE: SKILLS_RunCltStFunc has read its path before the call as well.
    nUnitType = UNIT_TYPES_COUNT;
    if (pUnit)
        nUnitType = pUnit->dwUnitType;

    bIsMonster = nUnitType == UNIT_MONSTER;
    if (bIsMonster)
    {
        pTarget = CUNIT_GetPathTargetUnit(pUnit);
        if (pTarget)
        {
            nX = UNITS_GetXPosition(pTarget);
            nY = UNITS_GetYPosition(pTarget);
            nX = 2 * nX - UNITS_GetXPosition(pUnit);
            nY = 2 * nY - UNITS_GetYPosition(pUnit);
        }
        else
        {
            if (!SKILLS_GetPathTargetPos(pUnit, &nX, &nY))
                return FALSE;

            D2COMMON_SKILLS_SetParam4(pSkill, -1);
        }

        tSpawnPoint.nX = nX;
        tSpawnPoint.nY = nY;
        if (!D2COMMON_COLLISION_GetFreeCoordinates(D2COMMON_UNITS_GetRoom(pUnit), &tSpawnPoint, D2COMMON_UNITS_GetUnitSizeX(pUnit), COLLIDE_MASK_MONSTER_PATH, FALSE))
        {
            return FALSE;
        }

        nX = tSpawnPoint.nX;
        nY = tSpawnPoint.nY;
    }
    else
    {
        if (!SKILLS_GetPathTargetPos(pUnit, &nX, &nY))
            return FALSE;

        pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
        if (!pSkillsTxt)
            return FALSE;

        pRoom = D2COMMON_UNITS_GetRoom(pUnit);
        if (!pRoom)
            return FALSE;

        pRoom = CLIENT_FindRoomContainingSubtile(pRoom, nX, nY);
        if (!pRoom)
            return FALSE;

        if (!(((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_INTOWN >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_INTOWN & 7]) && D2COMMON_DUNGEON_IsRoomInTown(pRoom))
        {
            return FALSE;
        }

        if (!SKILLS_ResolveClientTargetPoint(pUnit, nSkillId, nSkillLevel, &nX, &nY))
            return FALSE;
    }

    pRoom = CLIENT_FindRoomContainingSubtile(D2COMMON_UNITS_GetRoom(pUnit), nX, nY);

    D2COMMON_COLLISION_SetMaskWithPattern(pRoom, nX, nY, (D2C_CollisionPattern)D2COMMON_PATH_GetUnitCollisionPattern(pUnit), COLLIDE_PLAYER);
    D2COMMON_SKILLS_SetParam1(pSkill, nX);
    D2COMMON_SKILLS_SetParam2(pSkill, nY);
    if (bIsMonster)
    {
        pTarget = CUNIT_GetPathTargetUnit(pUnit);
        if (pTarget)
        {
            D2COMMON_SKILLS_SetParam3(pSkill, pTarget->dwUnitType);
            D2COMMON_SKILLS_SetParam4(pSkill, pTarget->dwUnitId);
        }
    }

    D2COMMON_SKILLS_SetFlags(pSkill, 0x1080);
    SKILLS_PostSkillStatWithRemoveCallback(pUnit);
    return TRUE;
}


//1.10f: D2Client.0x6FAC16F0
//  Where a leap can land; its one caller is `cltstfunc` 29. It answers FALSE without the skill's record, and otherwise moves the point at *pnX,
//  *pnY:
//    * the reach is the row's `aurarangecalc` evaluated (+64h; stock Leap's is `dm12`). A point farther than that by D2Common's
//      UNITS_GetDistanceToCoordinates (a distance of 0 or 1 counted as 1) is pulled back along the line from the unit to the reach: each offset
//      times the reach, divided by the distance, signed (imul, cdq / idiv), and the unit's position added back. The Y offset is read before *pnX is
//      written;
//    * from that point, two side steps: the X step is the Y offset and the Y step the negated X offset, each held to -2..2, up to two subtiles
//      across the line of the leap;
//    * three tries: the point, then the point plus the side step, then the point less it. Each finds the room holding the try from the unit's room,
//      asks D2Common's COLLISION_GetFreeCoordinates for a free spot there for the unit's size against COLLIDE_MASK_PLAYER_PATH, neighbour rooms
//      allowed, and keeps the spot only if the unit's collision pattern meets nothing of that mask there (COLLISION_CheckAnyCollisionWithPattern),
//      the ray from the unit's position to it crosses no COLLIDE_MASK_PLAYER_FLYING (a door or a missile barrier; #11025, push 804h) and it lies
//      within the reach. The first spot kept is written back and the answer is TRUE;
//    * after the third failure a player hears sound event 13h (UNITSND_PlayPlayerSoundEvent), the "cannot do that" grunt, and the answer is FALSE.
//      The record's refusal plays nothing.
//  The distance is compared and divided signed here, and D2Common declares it `unsigned int`. Its body computes it signed and never below 0, so
//  either type reads the same value, and the C casts.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition, UNITS_GetYPosition and CLIENT_FindRoomContainingSubtile here.
BOOL __fastcall SKILLS_ResolveClientTargetPoint(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t *pnX, int32_t *pnY)
{
    D2SkillsTxt *pSkillsTxt;
    D2ActiveRoomStrc *pRoom;
    D2ActiveRoomStrc *pFreeRoom;
    D2CoordStrc tSpawnPoint;
    int32_t nUnitX;
    int32_t nUnitY;
    int32_t nRange;
    int32_t nDistance;
    int32_t nDeltaY;
    int32_t nSideStepX;
    int32_t nSideStepY;
    int32_t nTries;
    int32_t nX;
    int32_t nY;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return FALSE;

    nUnitX = UNITS_GetXPosition(pUnit);

    nUnitY = UNITS_GetYPosition(pUnit);

    nRange = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwAuraRangeCalc, nSkillId, nSkillLevel);

    nDistance = (int32_t)D2COMMON_UNITS_GetDistanceToCoordinates(pUnit, *pnX, *pnY);
    if (nDistance <= 1)
        nDistance = 1;

    if (nDistance > nRange)
    {
        nDeltaY = *pnY - nUnitY;
        *pnX = (*pnX - nUnitX) * nRange / nDistance + nUnitX;
        *pnY = nDeltaY * nRange / nDistance + nUnitY;
    }

    nY = *pnY;
    nSideStepX = nY - nUnitY;
    if (nSideStepX <= -2)
        nSideStepX = -2;
    else if (nSideStepX >= 2)
        nSideStepX = 2;

    nX = *pnX;
    nSideStepY = nUnitX - nX;
    if (nSideStepY <= -2)
        nSideStepY = -2;
    else if (nSideStepY >= 2)
        nSideStepY = 2;

    nTries = 3;
    for (;;)
    {
        tSpawnPoint.nX = nX;
        tSpawnPoint.nY = nY;

        pRoom = CLIENT_FindRoomContainingSubtile(D2COMMON_UNITS_GetRoom(pUnit), nX, nY);

        pFreeRoom = D2COMMON_COLLISION_GetFreeCoordinates(pRoom, &tSpawnPoint, D2COMMON_UNITS_GetUnitSizeX(pUnit), COLLIDE_MASK_PLAYER_PATH, TRUE);
        nX = tSpawnPoint.nX;
        nY = tSpawnPoint.nY;
        if (pFreeRoom && !D2COMMON_COLLISION_CheckAnyCollisionWithPattern(pFreeRoom, nX, nY, D2COMMON_PATH_GetUnitCollisionPattern(pUnit), COLLIDE_MASK_PLAYER_PATH) && D2COMMON_11025(nUnitX, nUnitY, nX, nY, pFreeRoom, COLLIDE_MASK_PLAYER_FLYING)
            && (int32_t)D2COMMON_UNITS_GetDistanceToCoordinates(pUnit, nX, nY) <= nRange)
        {
            *pnX = nX;
            *pnY = nY;
            return TRUE;
        }

        --nTries;
        if (nTries == 2)
        {
            nX = *pnX + nSideStepX;
            nY = *pnY + nSideStepY;
        }
        else if (nTries == 1)
        {
            nX = *pnX - nSideStepX;
            nY = *pnY - nSideStepY;
        }
        else if (nTries == 0)
        {
            break;
        }
        //  BUG: the count runs 3, 2, 1, 0 and nothing else writes it, so the original's arm for any other count (back to the room lookup without
        //  storing the point, a retry of the spot the last try left; test edx, edx / jnz at 0x6FAC19D6..0x6FAC19D8, to 0x6FAC1875) cannot run.
        //  UNREACHABLE: ours writes that arm as the loop going round again with the spot the last try left, which tSpawnPoint already holds.
    }

    if (pUnit->dwUnitType == UNIT_PLAYER)
        UNITSND_PlayPlayerSoundEvent(pUnit, 0x13u);

    return FALSE;
}


static_assert(offsetof(D2MonStatsTxt, nRun) == 0x34 && offsetof(D2CharStatsTxt, nRunSpeed) == 0x41,
              "SKILLS_StartChargeMove: `movsx eax, word ptr [eax+34h]` (0x6FAC1ABF), `mov ch, [eax+41h]` (0x6FAC1AF3)");
static_assert(offsetof(D2LeapArcBandStrc, nSlope) == 4 && offsetof(D2LeapArcBandStrc, nIntercept) == 8,
              "SKILLS_StartLeapArc: `(gLeapArcTable+4)[eax]` (0x6FAC1BB6), `(gLeapArcTable+8)[eax]` (0x6FAC1BBC)");
static_assert(PATHTYPE_LEAP == 9 && PATHTYPE_TOWARD == 2 && PATHTYPE_STRAIGHT == 7 && COLLIDE_MONSTER == 0x100,
              "`push 9` (0x6FAC1A7D); SKILLS_ApplyDefaultCollision: `push 2` (0x6FAC1D59), `push 7` (0x6FAC1D97), "
              "`push 100h` (0x6FAC1D2B)");
static_assert(MONSTER_SANDLEAPER1 == 78 && MONSTER_ANCIENTBARB1 == 540,
              "SKILLS_CltDoFuncLeap: `cmp ebx, 4Eh` (0x6FAC1F48), `cmp ebx, 21Ch` (0x6FAC1F54)");


//1.10f: D2Client.0x6FAC1A20
//  A leap's charge move. It answers FALSE without a dynamic path, or when param1 or param2, the landing point `cltstfunc` 29 wrote, is 0. Then it
//  sets the skill's flags to 1101h and the path's move-test and footprint masks to COLLIDE_NONE (the leaper collides with nothing), aims the path
//  at the point (#10170) as PATHTYPE_LEAP, and sets its velocity: a monster's monstats.txt `Run`, anything else's charstats.txt `RunVelocity`,
//  times 256 either way, with this file's path and line 567 (push 237h). The unit starts to move (PATH_StartUnitMove, bAllowInTown 0) and the
//  answer is TRUE. A monster without a monstats record gets FALSE only after the flags, the masks, the target and the type are set.
//  Its callers are `cltdofunc` 43 and 44, neither of which reads the answer. The original inlines DATATBLS_GetMonStatsTxtRecord and a charstats.txt
//  lookup here.
BOOL __fastcall SKILLS_StartChargeMove(D2UnitStrc *pUnit, D2SkillStrc *pSkill)
{
    D2DynamicPathStrc *pDynamicPath;
    D2MonStatsTxt *pMonStatsTxt;
    D2CharStatsTxt *pCharStatsTxt;
    int32_t nX;
    int32_t nY;
    int32_t nVelocity;

    pDynamicPath = pUnit->pDynamicPath;
    if (!pDynamicPath)
        return FALSE;

    nX = D2COMMON_SKILLS_GetParam1(pSkill);
    nY = D2COMMON_SKILLS_GetParam2(pSkill);
    if (!nX || !nY)
        return FALSE;

    D2COMMON_SKILLS_SetFlags(pSkill, 0x1101);
    D2COMMON_PATH_SetMoveTestCollisionMask(pDynamicPath, COLLIDE_NONE);
    D2COMMON_PATH_SetFootprintCollisionMask(pDynamicPath, COLLIDE_NONE);
    D2COMMON_10170_PathSetTargetPos(pDynamicPath, nX, nY);
    D2COMMON_PATH_SetType(pDynamicPath, PATHTYPE_LEAP);

    if (pUnit->dwUnitType == UNIT_MONSTER)
    {
        pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);
        if (!pMonStatsTxt)
            return FALSE;

        nVelocity = pMonStatsTxt->nRun << 8;
    }
    else
    {
        if (pUnit->dwClassId < 0 || pUnit->dwClassId >= sgptDataTables->nCharStatsTxtRecordCount)
            pCharStatsTxt = NULL;
        else
            pCharStatsTxt = &sgptDataTables->pCharStatsTxt[pUnit->dwClassId];

        //  BUG: the charstats lookup answers NULL for a class outside the table, and the answer is read at once (0x6FAC1AF3), a fault at 41h, where
        //  the monster arm tests its record (0x6FAC1ABB). LATENT: a player's class is always in charstats.txt.
        nVelocity = pCharStatsTxt->nRunSpeed << 8;
    }

    D2COMMON_PATH_SetVelocity(pDynamicPath, nVelocity, szSkillsBarCpp, 567);
    D2COMMON_PATH_StartUnitMove(pDynamicPath, pUnit, FALSE);
    return TRUE;
}


//1.10f: D2Client.0x6FAC1B30
//  A leap's arc; its callers are `cltdofunc` 43 and 44, right after the charge move. It does nothing without the skill, a dynamic path or a
//  velocity (the path's, shifted down 8 signed: sar esi, 8). Otherwise, with the distance to the point in param1 and param2 (at least 1; jge):
//    * the frames the arc takes, (distance << 16) / (velocity << 12), that is distance * 16 / velocity, divided signed (cdq / idiv), one less when
//      above 1;
//    * the launch speed from gLeapArcTable, two bands of { nMaxDistance, nSlope, nIntercept }: { 27, -270, 8462 } and { 36, -72, 1224 } in the
//      stock image. The scan tests band 0's bound only: it compares the band pointer with gLeapArcTable + 0Ch, signed (jl at 0x6FAC1BAE), the shape
//      a signed index loop over every band but the last compiles to, so the last band catches every longer leap and its own bound, 36, is never
//      read. A leap under 27 subtiles launches at 8462 - 270 * distance (8192 at 1, 1442 at 26), and any longer one at 1224 - 72 * distance, which
//      the floor of 500 replaces from 27 on;
//    * the unit's particles allocated, its vector set to that speed straight up (the Z negated), and the arc started with a peak height of 0 over
//      those frames.
void __fastcall SKILLS_StartLeapArc(D2UnitStrc *pUnit, D2SkillStrc *pSkill)
{
    D2DynamicPathStrc *pDynamicPath;
    int32_t nX;
    int32_t nY;
    int32_t nVelocity;
    int32_t nDistance;
    int32_t nDurationFrames;
    int32_t nBand;
    int32_t nLaunchSpeed;

    if (!pSkill)
        return;

    pDynamicPath = pUnit->pDynamicPath;
    if (!pDynamicPath)
        return;

    nX = D2COMMON_SKILLS_GetParam1(pSkill);
    nY = D2COMMON_SKILLS_GetParam2(pSkill);
    nVelocity = D2COMMON_PATH_GetVelocity(pDynamicPath) >> 8;
    if (!nVelocity)
        return;

    nDistance = (int32_t)D2COMMON_UNITS_GetDistanceToCoordinates(pUnit, nX, nY);
    if (nDistance < 1)
        nDistance = 1;

    nDurationFrames = (nDistance << 16) / (nVelocity << 12);
    if (nDurationFrames > 1)
        --nDurationFrames;

    for (nBand = 0; nBand < 1; ++nBand)
    {
        if (nDistance < gLeapArcTable[nBand].nMaxDistance)
            break;
    }

    nLaunchSpeed = gLeapArcTable[nBand].nSlope * nDistance + gLeapArcTable[nBand].nIntercept;
    if (nLaunchSpeed < 500)
        nLaunchSpeed = 500;

    PARTICLE_AllocUnitParticles(pUnit);
    PARTICLE_SetUnitGfxVector2(pUnit, 0, 0, -nLaunchSpeed, 0);
    PARTICLE_StartUnitArc(pUnit, 0, nDurationFrames);
}


//1.10f: D2Client.0x6FAC1C00
//  The collision a leap lifted, put back. Its callers end a Leap or a Leap Attack on the client: SCMD_HandleSrvA5 and
//  SKILLS_StatRemoveCallback_EndMoveSkill. The footprint mask a unit should have is COLLIDE_MONSTER for a monster and COLLIDE_PLAYER for anything
//  else (dec / neg / sbb / and / add); when D2Common's PATH_GetFootprintCollisionMask already answers that (it answers for a NULL path too),
//  nothing happens. Otherwise, at a point (the used skill's param1 and param2 when there is a used skill, the unit's own position when there is
//  not), that mask is cleared from the unit's collision pattern (COLLISION_ResetMaskWithPattern), the unit is warped there (CUNIT_WarpUnit) and
//  SKILLS_ApplyDefaultCollision restores the rest. The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall SKILLS_RestoreCollisionFootprint(D2UnitStrc *pUnit)
{
    D2SkillStrc *pUsedSkill;
    int32_t nMask;
    int32_t nX;
    int32_t nY;

    nMask = (pUnit->dwUnitType == UNIT_MONSTER) ? COLLIDE_MONSTER : COLLIDE_PLAYER;
    if (D2COMMON_PATH_GetFootprintCollisionMask(pUnit->pDynamicPath) == nMask)
        return;

    pUsedSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);

    if (pUsedSkill)
        nX = D2COMMON_SKILLS_GetParam1(pUsedSkill);
    else
        nX = UNITS_GetXPosition(pUnit);

    if (pUsedSkill)
        nY = D2COMMON_SKILLS_GetParam2(pUsedSkill);
    else
        nY = UNITS_GetYPosition(pUnit);

    D2COMMON_COLLISION_ResetMaskWithPattern(D2COMMON_UNITS_GetRoom(pUnit), nX, nY, (D2C_CollisionPattern)D2COMMON_PATH_GetUnitCollisionPattern(pUnit), (uint16_t)nMask);
    CUNIT_WarpUnit(pUnit, nX, nY, FALSE);
    SKILLS_ApplyDefaultCollision(pUnit, nX, nY);
}


//1.10f: D2Client.0x6FAC1CE0
//  The collision a unit walks with; its callers are SKILLS_RestoreCollisionFootprint and the landing of SKILLS_CltDoFuncLeap. Without a dynamic
//  path or a room it does nothing. Otherwise, by the unit's type:
//      a monster      COLLIDE_MONSTER   COLLIDE_MASK_MONSTER_PATH   PATHTYPE_TOWARD
//      anything else  COLLIDE_PLAYER    COLLIDE_MASK_PLAYER_PATH    PATHTYPE_STRAIGHT
//  the footprint mask is cleared from the unit's collision pattern at nX, nY when both are non-zero (COLLISION_ResetMaskWithPattern), set as the
//  path's footprint mask, the move-test mask set, and the path type; a dead unit (CUNIT_IsUnitDead) also gets D2Common's PATH_SetUnitDeadCollision
//  with bForGameLogic 0. These are the masks SKILLS_StartChargeMove zeroed.
//  BUG: a monster's leap marks its landing point with COLLIDE_PLAYER (`cltstfunc` 29, push 80h at 0x6FAC165D) where the landing clears
//  COLLIDE_MONSTER (0x6FAC1D2B), leaving COLLIDE_PLAYER behind. REACHABLE: stock sandleapers leap and ancientbarb3 leap-attacks.
void __fastcall SKILLS_ApplyDefaultCollision(D2UnitStrc *pUnit, int32_t nX, int32_t nY)
{
    D2DynamicPathStrc *pDynamicPath;
    D2ActiveRoomStrc *pRoom;
    D2C_UnitTypes nUnitType;

    //  BUG: the unit is tested for NULL (0x6FAC1CE9) for its type only, and its path is read at once (0x6FAC1CFD). UNREACHABLE: both callers have
    //  read through it.
    if (pUnit)
        nUnitType = pUnit->dwUnitType;
    else
        nUnitType = UNIT_TYPES_COUNT;

    pDynamicPath = pUnit->pDynamicPath;
    pRoom = D2COMMON_UNITS_GetRoom(pUnit);
    if (!pDynamicPath || !pRoom)
        return;

    if (nUnitType == UNIT_MONSTER)
    {
        if (nX && nY)
        {
            D2COMMON_COLLISION_ResetMaskWithPattern(pRoom, nX, nY, (D2C_CollisionPattern)D2COMMON_PATH_GetUnitCollisionPattern(pUnit), COLLIDE_MONSTER);
        }

        D2COMMON_PATH_SetFootprintCollisionMask(pDynamicPath, COLLIDE_MONSTER);
        D2COMMON_PATH_SetMoveTestCollisionMask(pDynamicPath, COLLIDE_MASK_MONSTER_PATH);
        D2COMMON_PATH_SetType(pDynamicPath, PATHTYPE_TOWARD);
    }
    else
    {
        if (nX && nY)
        {
            D2COMMON_COLLISION_ResetMaskWithPattern(pRoom, nX, nY, (D2C_CollisionPattern)D2COMMON_PATH_GetUnitCollisionPattern(pUnit), COLLIDE_PLAYER);
        }

        D2COMMON_PATH_SetFootprintCollisionMask(pDynamicPath, COLLIDE_PLAYER);
        D2COMMON_PATH_SetMoveTestCollisionMask(pDynamicPath, COLLIDE_MASK_PLAYER_PATH);
        D2COMMON_PATH_SetType(pDynamicPath, PATHTYPE_STRAIGHT);
    }

    if (CUNIT_IsUnitDead(pUnit))
        D2COMMON_PATH_SetUnitDeadCollision(pUnit, FALSE);
}


//1.10f: D2Client.0x6FAC1DC0
//  `cltdofunc` 43, Leap each frame; nSkillLevel is not read. It answers 0 without the unit's highest-level skill of this id (D2Common's __fastcall
//  SKILLS_GetHighestLevelSkillFromUnitAndId). Then, by that skill's flags (SKILLS_GetFlags):
//    bit 8 (100h)  the leap is moving: SKILLS_CltDoFuncLeap with the dust shown, and 0 while it answers 0 (in flight);
//    bit 7 (80h)   the first frame, `cltstfunc` 29's 1080h: the charge move and the arc, and 1. The charge sets 1101h, so every later frame takes
//                  bit 8;
//    otherwise     a unit that is not a monster gets flags 1000h, and 1.
//  A landing that answers 1 goes on to the bit-7 test, which 1101h fails: a player's flags become 1000h and later frames call nothing, a monster's
//  stay 1101h.
int32_t __fastcall SKILLS_CltDoFunc43(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;
    int32_t nFlags;

    pSkill = D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, nSkillId);
    if (!pSkill)
        return 0;

    nFlags = D2COMMON_SKILLS_GetFlags(pSkill);
    if ((nFlags & 0x100) && !SKILLS_CltDoFuncLeap(pUnit, pSkill, TRUE))
        return 0;

    if (nFlags & 0x80)
    {
        SKILLS_StartChargeMove(pUnit, pSkill);
        SKILLS_StartLeapArc(pUnit, pSkill);
        return 1;
    }

    //  BUG: pUnit is tested for NULL after SKILLS_GetHighestLevelSkillFromUnitAndId has been handed it, whose body reads pUnit->pSkills first
    //  (0x6FAC1E19). UNREACHABLE: pUnit has already been through D2Common.
    if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER)
        D2COMMON_SKILLS_SetFlags(pSkill, 0x1000);

    return 1;
}

static_assert(OVERLAY_DUST == 80, "SKILLS_CltDoFuncLeap: `mov edx, 50h` (0x6FAC1FE3)");


//1.10f: D2Client.0x6FAC1E40
//  Has the leap landed; its callers are `cltdofunc` 43 and 44, each passing TRUE for bShowDustOverlay. It is in no slot of gpSkillCltDoFuncs. With
//  the point in param1 and param2, the unit's graphic height (PARTICLE_GetUnitGfxCoordZ) and, for a monster, the base id of its class held to the
//  monstats count (-1 otherwise):
//    in flight     the height is not 0 and the unit is not at the point: the animation frame 10 for anything but a monster; for a monster 0, 8 for
//                  a sandleaper, and for an ancient barbarian 12 within 2 of the point and 10 farther (an unsigned compare, cmp eax, 2 / sbb); the
//                  frame counter moved to the next whole frame; FALSE;
//    landed        SKILLS_ApplyDefaultCollision at the point, the graphic offset zeroed, the `dust` overlay (overlay.txt 80) when asked, and the
//                  unit's running footstep; anything but a monster also plays `barbarian_leap_land` (sounds.txt 2517). A sandleaper whose
//                  param3/param4 target still exists then bounces: skill flags 1, animation frame 12, the next whole frame, #10212's move flags
//                  cleared, and a knock-back path (PATHTYPE_KNOCKBACK_CLIENT, distance 5) to 3 * the point - 2 * the target (twice the landing's
//                  distance from the target again, away from it) unless that lands in a town room. TRUE.
//  The original inlines MONSTER_GetBaseMonsterId, UNITS_GetXPosition and UNITS_GetYPosition here.
BOOL __fastcall SKILLS_CltDoFuncLeap(D2UnitStrc *pUnit, D2SkillStrc *pSkill, BOOL bShowDustOverlay)
{
    D2DynamicPathStrc *pDynamicPath;
    D2UnitStrc *pTarget;
    D2ActiveRoomStrc *pRoom;
    D2C_UnitTypes nUnitType;
    BOOL bIsMonster;
    int32_t nX;
    int32_t nY;
    int32_t nGfxZ;
    int32_t nBaseId;
    int32_t nFrame;
    int32_t nTargetType;
    uint32_t dwTargetGUID;
    int32_t nBounceX;
    int32_t nBounceY;

    nX = D2COMMON_SKILLS_GetParam1(pSkill);
    nY = D2COMMON_SKILLS_GetParam2(pSkill);
    nGfxZ = PARTICLE_GetUnitGfxCoordZ(pUnit);

    //  BUG: pUnit is tested for NULL for its type (0x6FAC1E67) and for the base id (0x6FAC1E84), and read unconditionally on both paths after it
    //  (the position read at 0x6FAC1EE0 and SKILLS_ApplyDefaultCollision's path read). UNREACHABLE: both callers, `cltdofunc` 43 and 44, have
    //  already passed pUnit to D2Common.
    nUnitType = UNIT_TYPES_COUNT;
    if (pUnit)
        nUnitType = pUnit->dwUnitType;

    bIsMonster = nUnitType == UNIT_MONSTER;

    nBaseId = -1;
    if (bIsMonster)
    {
        nBaseId = MONSTER_GetBaseMonsterId(pUnit);
        if (nBaseId < 0 || nBaseId >= sgptDataTables->nMonStatsTxtRecordCount)
            nBaseId = -1;
    }

    if (nGfxZ && (UNITS_GetXPosition(pUnit) != nX || UNITS_GetYPosition(pUnit) != nY))
    {
        if (bIsMonster)
        {
            nFrame = 0;
            if (nBaseId == MONSTER_SANDLEAPER1)
                nFrame = 8;
            else if (nBaseId == MONSTER_ANCIENTBARB1)
                nFrame = (D2COMMON_UNITS_GetDistanceToCoordinates(pUnit, nX, nY) < 2) ? 12 : 10;

            D2COMMON_UNITS_SetAnimationFrame(pUnit, nFrame);
        }
        else
        {
            D2COMMON_UNITS_SetAnimationFrame(pUnit, 10);
        }

        pUnit->dwFrameCountPrecise = (pUnit->dwFrameCountPrecise & 0xFFFFFF00) + 0x100;
        return FALSE;
    }

    SKILLS_ApplyDefaultCollision(pUnit, nX, nY);
    PARTICLE_SetUnitGfxCoords(pUnit, 0, 0, 0);
    if (bShowDustOverlay)
        GFX_AddOverlay(pUnit, OVERLAY_DUST, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);

    if (!bIsMonster)
    {
        SOUNDINST_PlaySound(UNITSND_GetFootstepSoundId(pUnit, TRUE), pUnit, 0, 0, 0);
        SOUNDINST_PlaySound(SOUND_BARBARIAN_LEAP_LAND, pUnit, 0, 0, 0);
        return TRUE;
    }

    SOUNDINST_PlaySound(UNITSND_GetFootstepSoundId(pUnit, TRUE), pUnit, 0, 0, 0);
    if (nBaseId != MONSTER_SANDLEAPER1)
        return TRUE;

    pDynamicPath = pUnit->pDynamicPath;
    if (!pDynamicPath)
        return TRUE;

    nTargetType = D2COMMON_SKILLS_GetParam3(pSkill);
    dwTargetGUID = (uint32_t)D2COMMON_SKILLS_GetParam4(pSkill);
    pTarget = CUNIT_FindUnitByGuidAndType(dwTargetGUID, (D2C_UnitTypes)nTargetType);
    if (!pTarget)
        return TRUE;

    D2COMMON_SKILLS_SetFlags(pSkill, 1);
    D2COMMON_UNITS_SetAnimationFrame(pUnit, 12);
    pUnit->dwFrameCountPrecise = (pUnit->dwFrameCountPrecise & 0xFFFFFF00) + 0x100;
    D2COMMON_10212_PATH_SetMoveFlags(pUnit, FALSE);
    D2COMMON_PATH_RecacheRoomIfNeeded(pDynamicPath);
    D2COMMON_PATH_SetNewDistance(pDynamicPath, 5);
    D2COMMON_PATH_SetStepNum(pDynamicPath, 1);
    D2COMMON_PATH_SetTargetUnit(pDynamicPath, NULL);

    nBounceX = 3 * nX - 2 * UNITS_GetXPosition(pTarget);
    nBounceY = 3 * nY - 2 * UNITS_GetYPosition(pTarget);

    pRoom = CLIENT_FindRoomContainingSubtile(D2COMMON_UNITS_GetRoom(pUnit), nBounceX, nBounceY);
    if (pRoom && D2COMMON_DUNGEON_IsRoomInTown(pRoom))
        return TRUE;

    D2COMMON_10170_PathSetTargetPos(pDynamicPath, nBounceX, nBounceY);
    D2COMMON_PATH_SetType(pDynamicPath, PATHTYPE_KNOCKBACK_CLIENT);
    D2COMMON_10190_PATH_SetDistance(pDynamicPath, 5);
    D2COMMON_PATH_StartUnitMove(pDynamicPath, pUnit, FALSE);
    return TRUE;
}


static_assert(offsetof(D2MonStatsTxt, nVelocity) == 0x32 && offsetof(D2CharStatsTxt, nWalkSpeed) == 0x40,
              "SKILLS_CltStFunc31: `movsx eax, word ptr [eax+32h]` (0x6FAC2647), `mov ch, [eax+40h]` (0x6FAC2611)");
static_assert(COLLIDE_MASK_PLAYER_WW == 0xC01 && (COLLIDE_WALL | COLLIDE_OBJECT) == 0x401,
              "SKILLS_CltStFunc31: `push 0C01h` (0x6FAC258F), `push 401h` (0x6FAC265F)");


//1.10f: D2Client.0x6FAC2180
//  `cltstfunc` 30, Leap Attack starts; nSkillLevel is not read. The answer stays `int`: on one path it is SKILLS_DispatchUnitAction22's. It answers
//  0 without the unit's highest-level skill of this id and, for the local player only (CUNIT_GetPlayer), while the unit is in STATE_UNINTERRUPTABLE
//  or when that skill is already this one with bit 0 of its flags set (this file's path and line 922, push 39Ah). Then D2Common's
//  SKILLS_CheckIfCanLeapTo (#11037) is asked for a landing point toward the path target, the point zeroed first:
//    refused       a path target in melee range (UNITS_IsInMeleeRange, bonus 0) gets SKILLS_DispatchUnitAction22 (an ordinary attack) and its
//                  answer; otherwise a unit that is not a monster hears sound event 13h, the "cannot do that" grunt, and the answer is 0;
//    granted       0 without the unit's room; else the unit's collision pattern marked COLLIDE_PLAYER at the point, through the unit's own room
//                  (where `cltstfunc` 29 looks up the room holding the point); the point in param1 and param2; the target's type and GUID in param3
//                  and param4, or UNIT_TYPES_COUNT alone in param3 without one (push 6); flags 1080h (0x6FAC22C2), which `cltdofunc` 44 takes as
//                  its first frame; SKILLS_PostSkillStatWithRemoveCallback; and 1.
int32_t __fastcall SKILLS_CltStFunc30(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;
    D2UnitStrc *pTarget;
    D2ActiveRoomStrc *pRoom;
    int32_t nX;
    int32_t nY;

    pSkill = D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, nSkillId);
    if (!pSkill)
        return 0;

    if (pUnit == CUNIT_GetPlayer())
    {
        if (D2COMMON_STATES_CheckState(pUnit, STATE_UNINTERRUPTABLE))
            return 0;

        if (D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillsBarCpp, 922) == (int32_t)nSkillId && (D2COMMON_SKILLS_GetFlags(pSkill) & 1))
        {
            return 0;
        }
    }

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    nX = 0;
    nY = 0;
    if (!D2COMMON_SKILLS_CheckIfCanLeapTo(pUnit, pTarget, &nX, &nY))
    {
        if (pTarget && D2COMMON_UNITS_IsInMeleeRange(pUnit, pTarget, 0))
            return SKILLS_DispatchUnitAction22(pUnit, pTarget);

        //  BUG: pUnit is tested for NULL (0x6FAC2238) after SKILLS_GetHighestLevelSkillFromUnitAndId has read pUnit->pSkills, and a NULL unit is
        //  the one that would hear the sound. UNREACHABLE: pUnit has already been through D2Common.
        if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER)
            UNITSND_PlayPlayerSoundEvent(pUnit, 0x13u);

        return 0;
    }

    pRoom = D2COMMON_UNITS_GetRoom(pUnit);
    if (!pRoom)
        return 0;

    D2COMMON_COLLISION_SetMaskWithPattern(pRoom, nX, nY, (D2C_CollisionPattern)D2COMMON_PATH_GetUnitCollisionPattern(pUnit), COLLIDE_PLAYER);
    D2COMMON_SKILLS_SetParam1(pSkill, nX);
    D2COMMON_SKILLS_SetParam2(pSkill, nY);
    if (pTarget)
    {
        D2COMMON_SKILLS_SetParam3(pSkill, pTarget->dwUnitType);
        D2COMMON_SKILLS_SetParam4(pSkill, pTarget->dwUnitId);
    }
    else
    {
        D2COMMON_SKILLS_SetParam3(pSkill, UNIT_TYPES_COUNT);
    }

    D2COMMON_SKILLS_SetFlags(pSkill, 0x1080);
    SKILLS_PostSkillStatWithRemoveCallback(pUnit);
    return 1;
}


//1.10f: D2Client.0x6FAC22F0
//  `cltdofunc` 44, Leap Attack each frame; nSkillLevel is not read. It answers 0 without the unit's highest-level skill of this id. Then, by that
//  skill's flags (SKILLS_GetFlags, kept whole in a stack slot):
//    bit 8 (100h)  the leap is moving: SKILLS_CltDoFuncLeap with the dust shown, and 0 while it answers 0 (in flight). Landed, a target:
//                  param3/param4's unit when param3 is not UNIT_TYPES_COUNT (CUNIT_FindUnitByGuidAndType), else SKILLS_FindNextNearbyTargetById
//                  around the unit, radius 3, mask 0, after id -1 (the smallest id in range, not the nearest). Found, the unit targets it (#10344),
//                  faces it (CUNIT_FaceUnitAtPathTarget, bSnap 0) and plays its hit class's sound, and the answer becomes 1; not found, the frame
//                  counter is set to 400h (frame 4);
//    bit 7 (80h)   the first frame, `cltstfunc` 30's 1080h: the charge move and the arc, and 1;
//    otherwise     flags 1000h for every unit, and the target answer, 1 only for a landing that found one.
//  `cltdofunc` 43, Leap's, is the same program without the target, and sets 1000h only for a unit that is not a monster. A landing's flags are the
//  charge's 1101h, which fail bit 7, so every landing ends at 1000h. The answer is SKILLS_RunClientDoFunc's "done", which plays `dosound`, the
//  target's sound and overlay, and a local player's delay.
//  BUG: the unit the client faces and hears need not be the one the server strikes: the server's SKILLS_FindLeapAttackTarget takes its own target
//  when in melee range, then param3/param4's unit, then a nearby search with mask 20003h, and writes each back into param3 and param4, where the
//  client searches radius 3 with mask 0 and writes none back. REACHABLE.
//  BUG: in flight the answer is 0 where the server's is 1. INERT.
int32_t __fastcall SKILLS_CltDoFunc44(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;
    D2UnitStrc *pTarget;
    int32_t nFlags;
    int32_t nTargetType;
    uint32_t dwTargetGUID;
    int32_t bFoundTarget;

    pSkill = D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, nSkillId);
    if (!pSkill)
        return 0;

    bFoundTarget = 0;
    nFlags = D2COMMON_SKILLS_GetFlags(pSkill);
    if (nFlags & 0x100)
    {
        if (!SKILLS_CltDoFuncLeap(pUnit, pSkill, TRUE))
            return 0;

        nTargetType = D2COMMON_SKILLS_GetParam3(pSkill);
        dwTargetGUID = (uint32_t)D2COMMON_SKILLS_GetParam4(pSkill);
        pTarget = NULL;
        if (nTargetType != UNIT_TYPES_COUNT)
            pTarget = CUNIT_FindUnitByGuidAndType(dwTargetGUID, (D2C_UnitTypes)nTargetType);

        if (!pTarget)
            pTarget = SKILLS_FindNextNearbyTargetById(pUnit, 0, 0, 3, (D2C_AuraFilters)0, 0xFFFFFFFF, NULL);

        if (pTarget)
        {
            D2COMMON_UNITS_SetTargetUnitForDynamicUnit(pUnit, pTarget);
            CUNIT_FaceUnitAtPathTarget(pUnit, FALSE);
            //  the hit class goes over in all of eax (mov edx, eax at 0x6FAC238A); the slot's parameter is a char
            SOUNDINST_PlaySound(UNITSND_GetHitClassSound(pUnit, D2COMMON_UNITS_GetHitClass(pUnit)), pUnit, 0, 0, 0);
            bFoundTarget = 1;
        }
        else
        {
            pUnit->dwFrameCountPrecise = 0x400;
        }
    }

    if (nFlags & 0x80)
    {
        SKILLS_StartChargeMove(pUnit, pSkill);
        SKILLS_StartLeapArc(pUnit, pSkill);
        return 1;
    }

    D2COMMON_SKILLS_SetFlags(pSkill, 0x1000);
    return bFoundTarget;
}


//1.10f: D2Client.0x6FAC23F0
//  Whirlwind's spin copy. Without a dynamic path it does nothing. Otherwise the path's direction (#10158, and eax, 0FFh) is turned by 21 of its 64
//  steps when bFirstThird is set and by 42 when it is not (a third and two thirds of a turn), taken modulo 64 signed (and eax, 8000003Fh / jns) and
//  set; the unit is drawn once (GFX_DrawUnitCels, its fifth argument 1 for a player, setz dl, and its sixth 0); and the direction is put back. The
//  position is the perspective one when D2Gfx has perspective on (SCALE_ScalePerspectivePosition of the unit's precise X and Y at height 96, plus
//  the viewport's origin) and D2Common's client coordinates otherwise.
//  Its callers draw it with bFirstThird 1 and 0, so with the unit's own sprite it is drawn at three turns a third apart. They call it for a unit
//  with a used skill, in PLRMODE_SEQUENCE or MONMODE_SEQUENCE, whose skills.txt `srvdofunc` is 76 (stock Whirlwind alone). `srvdofunc` 67, Charge
//  and SerpentCharge, goes to SKILLS_DrawMotionTrail instead, when bit 0 of the skill's flags is set: a skill gets the spin or the trail, never
//  both.
void __fastcall SKILLS_DrawSpinCopy(D2UnitStrc *pUnit, int32_t nDrawParam, BOOL bFirstThird)
{
    D2DynamicPathStrc *pDynamicPath;
    int32_t nSavedDirection;
    int32_t nRotated;
    int32_t nPrecisionX;
    int32_t nPrecisionY;
    int32_t nDrawX;
    int32_t nDrawY;

    pDynamicPath = pUnit->pDynamicPath;
    if (!pDynamicPath)
        return;

    nSavedDirection = D2COMMON_PATH_GetDirection(pDynamicPath);
    if (bFirstThird)
        nRotated = nSavedDirection + 21;
    else
        nRotated = nSavedDirection + 42;

    D2COMMON_PATH_SetDirection(pDynamicPath, (uint8_t)(nRotated % 64));
    if (D2GFX_CheckPerspective())
    {
        nPrecisionY = D2COMMON_UNITS_GetPrecisionY(pUnit);
        nPrecisionX = D2COMMON_UNITS_GetPrecisionX(pUnit);
        D2GFX_SCALE_ScalePerspectivePosition(nPrecisionX, nPrecisionY, 96, &nDrawX, &nDrawY, 0);
        nDrawX += SCROLL_GetViewportOriginX();
        nDrawY += SCROLL_GetViewportOriginY();
    }
    else
    {
        nDrawX = D2COMMON_UNITS_GetClientCoordX(pUnit);
        nDrawY = D2COMMON_UNITS_GetClientCoordY(pUnit);
    }

    GFX_DrawUnitCels(pUnit, nDrawParam, nDrawX, nDrawY, pUnit->dwUnitType == UNIT_PLAYER, 0);
    D2COMMON_PATH_SetDirection(pDynamicPath, (uint8_t)nSavedDirection);
}


//1.10f: D2Client.0x6FAC24C0
//  `cltstfunc` 31, Whirlwind starts; nSkillLevel is not read. It answers 0 for a NULL unit or one that is neither a player nor a monster; without
//  the unit's highest-level skill of this id or without its dynamic path; for the local player, when that skill is already this one with bit 0 of
//  its flags set (line 1113, push 459h) or while the unit is in STATE_UNINTERRUPTABLE (the two tests in the other order from `cltstfunc` 30's); and
//  without a path target position (SKILLS_GetPathTargetPos). A path target in melee range gets SKILLS_DispatchUnitAction22 and its answer, as in
//  30. Otherwise the whirl is laid out as a walk:
//    * the path's move-test mask set to COLLIDE_MASK_PLAYER_WW, its type to PATHTYPE_STRAIGHT, and D2Common's PATH_StartUnitMove (bAllowInTown 0)
//      asked for a path to the target position;
//    * the velocity, with this file's line 1161 or 1167: a player's charstats.txt `WalkVelocity` << 8 when its class is in the table (mov ch,
//      [eax+40h]), and none set when it is not; a monster's monstats.txt `Velocity` << 8 (movsx), its record read without a test;
//    * the move-test mask COLLIDE_WALL | COLLIDE_OBJECT (push 401h), the search's without COLLIDE_DOOR, for the walk;
//    * the path's last point marked with the unit's collision pattern and footprint mask, COLLIDE_PLAYER for a player and COLLIDE_MONSTER
//      otherwise, through the unit's room; flags 1; the point in param1 and param2; SKILLS_PostSkillStatWithRemoveCallback; and 1.
//  When PATH_StartUnitMove fails or the path has no point, the unit's default move-test mask is put back, COLLIDE_MASK_PLAYER_PATH or
//  COLLIDE_MASK_MONSTER_PATH, and the answer is 0. Both masks are selected without a branch (neg / sbb / and / add), as in SKILLS_CltDoFunc45.
//  The original inlines a charstats.txt lookup and DATATBLS_GetMonStatsTxtRecord here.
int32_t __fastcall SKILLS_CltStFunc31(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;
    D2DynamicPathStrc *pDynamicPath;
    D2UnitStrc *pTarget;
    D2MonStatsTxt *pMonStatsTxt;
    D2CharStatsTxt *pCharStatsTxt;
    D2PathPointStrc *pPathPoints;
    D2C_CollisionPattern nPattern;
    BOOL bIsPlayer;
    int32_t nMoveTestMask;
    int32_t nFootprintMask;
    int32_t nPathPoints;
    int32_t nX;
    int32_t nY;

    if (!pUnit)
        return 0;

    if (pUnit->dwUnitType != UNIT_PLAYER && pUnit->dwUnitType != UNIT_MONSTER)
        return 0;

    pSkill = D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, nSkillId);
    pDynamicPath = pUnit->pDynamicPath;
    if (!pSkill || !pDynamicPath)
        return 0;

    if (pUnit == CUNIT_GetPlayer())
    {
        if (D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillsBarCpp, 1113) == (int32_t)nSkillId && (D2COMMON_SKILLS_GetFlags(pSkill) & 1))
        {
            return 0;
        }

        if (D2COMMON_STATES_CheckState(pUnit, STATE_UNINTERRUPTABLE))
            return 0;
    }

    if (!SKILLS_GetPathTargetPos(pUnit, &nX, &nY))
        return 0;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (pTarget && D2COMMON_UNITS_IsInMeleeRange(pUnit, pTarget, 0))
        return SKILLS_DispatchUnitAction22(pUnit, pTarget);

    bIsPlayer = pUnit->dwUnitType == UNIT_PLAYER;
    nMoveTestMask = bIsPlayer ? COLLIDE_MASK_PLAYER_PATH : COLLIDE_MASK_MONSTER_PATH;
    nFootprintMask = bIsPlayer ? COLLIDE_PLAYER : COLLIDE_MONSTER;
    D2COMMON_PATH_SetMoveTestCollisionMask(pDynamicPath, COLLIDE_MASK_PLAYER_WW);
    D2COMMON_PATH_SetType(pDynamicPath, PATHTYPE_STRAIGHT);
    if (D2COMMON_PATH_StartUnitMove(pDynamicPath, pUnit, FALSE))
    {
        if (bIsPlayer)
        {
            if (pUnit->dwClassId < 0 || pUnit->dwClassId >= sgptDataTables->nCharStatsTxtRecordCount)
                pCharStatsTxt = NULL;
            else
                pCharStatsTxt = &sgptDataTables->pCharStatsTxt[pUnit->dwClassId];

            //  BUG: a player whose class is outside charstats.txt gets no velocity at all. LATENT: a player's class is always in charstats.txt.
            if (pCharStatsTxt)
                D2COMMON_PATH_SetVelocity(pDynamicPath, pCharStatsTxt->nWalkSpeed << 8, szSkillsBarCpp, 1161);
        }
        else
        {
            pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);

            //  BUG: the monster's monstats record is read without a test (0x6FAC2647), so a class outside the table would fault at 32h. LATENT.
            D2COMMON_PATH_SetVelocity(pDynamicPath, pMonStatsTxt->nVelocity << 8, szSkillsBarCpp, 1167);
        }

        D2COMMON_PATH_SetMoveTestCollisionMask(pDynamicPath, COLLIDE_WALL | COLLIDE_OBJECT);
        nPathPoints = D2COMMON_PATH_GetPathPoints(pDynamicPath, &pPathPoints);
        if (nPathPoints)
        {
            nX = pPathPoints[nPathPoints - 1].X;
            nY = pPathPoints[nPathPoints - 1].Y;
            nPattern = (D2C_CollisionPattern)D2COMMON_PATH_GetUnitCollisionPattern(pUnit);
            D2COMMON_COLLISION_SetMaskWithPattern(D2COMMON_UNITS_GetRoom(pUnit), nX, nY, nPattern, (uint16_t)nFootprintMask);
            D2COMMON_SKILLS_SetFlags(pSkill, 1);
            D2COMMON_SKILLS_SetParam1(pSkill, nX);
            D2COMMON_SKILLS_SetParam2(pSkill, nY);
            SKILLS_PostSkillStatWithRemoveCallback(pUnit);
            return 1;
        }
    }

    D2COMMON_PATH_SetMoveTestCollisionMask(pDynamicPath, nMoveTestMask);
    return 0;
}


//1.10f: D2Client.0x6FAC2700
//  `cltdofunc` 45, Whirlwind each frame; nSkillLevel is not read. The unit's path is read first; it answers 0 without the unit's highest-level
//  skill of this id or without the path. With the point from param1 and param2, by the skill's flags (and edx, 3 / cmp dl, 3 at 0x6FAC275D):
//    bits 0 and 1  the whirl is over (SKILLS_StopWhirlwindSound sets bit 1 on a skill with bit 0 when the server ends it): the unit's collision
//                  pattern cleared from the point with its footprint mask, COLLIDE_PLAYER for a player and COLLIDE_MONSTER otherwise; that mask set
//                  as the path's footprint mask, and the default move-test mask put back (COLLIDE_MASK_PLAYER_PATH or COLLIDE_MASK_MONSTER_PATH);
//                  flags 0; the unit's playing `barbarian_whirlwind` (sounds.txt 2531, mov edx, 9E3h) handed to SOUNDINST_MarkStarted, as the stop
//                  does; and 1;
//    bit 0 alone   animation frame 3 and the frame counter at 500h, and 1;
//    otherwise     1.
//  SOUNDINST_MarkStarted returns at once for a handle of 0 (test ecx, ecx), so the handle is passed on untested here and in the stop.
int32_t __fastcall SKILLS_CltDoFunc45(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2DynamicPathStrc *pDynamicPath;
    D2SkillStrc *pSkill;
    D2C_CollisionPattern nPattern;
    BOOL bIsPlayer;
    int32_t nFootprintMask;
    int32_t nX;
    int32_t nY;
    int32_t nFlags;

    pDynamicPath = pUnit->pDynamicPath;
    pSkill = D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, nSkillId);
    if (!pSkill || !pDynamicPath)
        return 0;

    bIsPlayer = pUnit->dwUnitType == UNIT_PLAYER;
    nFootprintMask = bIsPlayer ? COLLIDE_PLAYER : COLLIDE_MONSTER;
    nX = D2COMMON_SKILLS_GetParam1(pSkill);
    nY = D2COMMON_SKILLS_GetParam2(pSkill);
    nFlags = D2COMMON_SKILLS_GetFlags(pSkill);
    if ((nFlags & 3) == 3)
    {
        //  BUG: the end clears the mark at the point without testing it for 0, where the server's end tests it. LATENT.
        nPattern = (D2C_CollisionPattern)D2COMMON_PATH_GetUnitCollisionPattern(pUnit);
        D2COMMON_COLLISION_ResetMaskWithPattern(D2COMMON_UNITS_GetRoom(pUnit), nX, nY, nPattern, (uint16_t)nFootprintMask);
        D2COMMON_PATH_SetFootprintCollisionMask(pDynamicPath, nFootprintMask);
        D2COMMON_PATH_SetMoveTestCollisionMask(pDynamicPath, bIsPlayer ? COLLIDE_MASK_PLAYER_PATH : COLLIDE_MASK_MONSTER_PATH);
        D2COMMON_SKILLS_SetFlags(pSkill, 0);
        SOUNDINST_MarkStarted(UNITSND_FindPlayingSoundInGroup(pUnit, SOUND_BARBARIAN_WHIRLWIND));
        return 1;
    }

    //  BUG: a whirling monster's frame differs from the server's (3 at 500h against 0 at 400h). REACHABLE: stock ancientbarb1 whirls.
    //  BUG: the answer is 1 without bit 0, where the server's is 0. INERT.
    if (nFlags & 1)
    {
        D2COMMON_UNITS_SetAnimationFrame(pUnit, 3);
        pUnit->dwFrameCountPrecise = 0x500;
    }

    return 1;
}


//1.10f: D2Client.0x6FAC2800
//  Whirlwind ends on the client. Its callers end a skill whose `srvdofunc` is 76 (stock Whirlwind) on the client: SCMD_HandleSrvA5 and
//  SKILLS_StatRemoveCallback_EndMoveSkill. On the unit's highest-level skill of nSkillId, when bit 0 of its flags is set, it sets bit 1 (or al, 2),
//  the pair SKILLS_CltDoFunc45 takes as the whirl's end on its next frame; then, with the skill or without it, the unit's playing
//  `barbarian_whirlwind` (sounds.txt 2531, 9E3h) is handed to SOUNDINST_MarkStarted. It ends in a tail jump to it and takes no stack argument. The
//  C writes the whole flags dword the bytes push.
void __fastcall SKILLS_StopWhirlwindSound(D2UnitStrc *pUnit, D2C_Skills nSkillId)
{
    D2SkillStrc *pSkill;
    int32_t nFlags;

    pSkill = D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, nSkillId);
    if (pSkill)
    {
        nFlags = D2COMMON_SKILLS_GetFlags(pSkill);
        if (nFlags & 1)
            D2COMMON_SKILLS_SetFlags(pSkill, nFlags | 2);
    }

    SOUNDINST_MarkStarted(UNITSND_FindPlayingSoundInGroup(pUnit, SOUND_BARBARIAN_WHIRLWIND));
}

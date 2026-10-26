#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "SkillsAss.h"

//  SkillsAss.cpp -- the Assassin's client skill code of D2Client.dll 1.10: six `cltstfunc` start functions, six `cltdofunc` do functions, eight
//  `cltprgfunc` functions of the progressive (charge-up) skills, and six helpers. Address range 0x6FABF4B0..0x6FAC0D2E, 26 functions.
//  No assert in this unit names its source file, so the file name is ours.
//  The start and do functions are reached only through the slot tables gpSkillCltStFuncs and gpSkillCltDoFuncs: a `cltdofunc` slot N is
//  SKILLS_CltDoFuncN, and a `cltprgfunc` slot is SKILLS_CltDoFuncN too (9..15 and 83 here). A skill id is a D2C_Skills and a missile id a
//  D2C_MissilesHcIdx. The progressive rows are the six whose flag PROGRESSIVE is set, all the assassin's: a charge-up strike builds charges in its
//  aura state, and SKILLS_GetClientMissileForSkill and SKILLS_EvaluateProgressiveSkillCalc pick a missile and a calc by how many it holds.

static_assert(offsetof(D2SkillsTxt, dwPrgCalc) == 0x38 && sizeof(D2SkillsTxt::dwPrgCalc) == 12,
              "SKILLS_EvaluateProgressiveSkillCalc: `mov eax, [esi+38h]` (0x6FABF7BF) and "
              "`mov eax, [esi+eax*4+34h]` (0x6FABF80D), eax 1..3");
static_assert(offsetof(D2SkillsTxt, wCltMissileC) == 0xEE,
              "SKILLS_GetClientMissileForSkill: `movsx eax, word ptr [esi+eax*2+0E8h]` (0x6FABF715), eax "
              "up to 3 -- wCltMissile, A, B and C are four consecutive words");
static_assert(offsetof(D2SkillsTxt, wPrgOverlay) == 0x10C && offsetof(D2SkillsTxt, wPrgSound) == 0x10E,
              "SKILLS_StartChannelState: `mov ax, [ebx+10Ch]` (0x6FABF9B6), `mov ax, [ebx+10Eh]` "
              "(0x6FABF9E7)");


//1.10f: D2Client.0x6FABF4B0
//  `cltstfunc` 5: can Psychic Hammer or Dragon Flight start. `retn 4`, reading neither nSkillId nor nSkillLevel. It answers 1 when the unit has a
//  path target, neither stands in a town room, the target is a player or a monster, and CUNIT_IsUnitSelectableTarget accepts it; 0 otherwise. The
//  type test is two signed compares on dwUnitType (`test eax, eax / jl`, `cmp eax, 1 / jg`, 0x6FABF4E1..0x6FABF4E8) and the answer is made 0 or 1
//  by `neg / sbb / neg` (0x6FABF4F3).
BOOL __fastcall SKILLS_CltStFunc5(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2UnitStrc *pTarget;
    int32_t nTargetType;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget)
        return FALSE;

    if (D2COMMON_DUNGEON_IsRoomInTown(D2COMMON_UNITS_GetRoom(pUnit)))
        return FALSE;

    if (D2COMMON_DUNGEON_IsRoomInTown(D2COMMON_UNITS_GetRoom(pTarget)))
        return FALSE;

    nTargetType = (int32_t)pTarget->dwUnitType;
    if (nTargetType < (int32_t)UNIT_PLAYER || nTargetType > (int32_t)UNIT_MONSTER)
        return FALSE;

    return CUNIT_IsUnitSelectableTarget(pUnit, pTarget) != 0;
}


//1.10f: D2Client.0x6FABF510
//  `cltdofunc` 3: Psychic Hammer strikes. `retn 4`. With `cltstfunc` 5's test passed it sets UNITFLAG_SKSRVDOFUNC on a non-NULL unit (`or al, 40h`
//  on the whole dword read at 0x6FABF562 and stored back) and answers 1; otherwise 0. The client draws nothing here: the hammer's missile and its
//  damage are the server's. The original inlines SKILLS_CltStFunc5 here.
int32_t __fastcall SKILLS_CltDoFunc3(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    if (!SKILLS_CltStFunc5(pUnit, nSkillId, nSkillLevel))
        return 0;

    //  BUG: `test edi, edi` (0x6FABF55E) guards only the flag; pUnit has already gone to CUNIT_GetPathTargetUnit and to D2Common's UNITS_GetRoom.
    //  UNREACHABLE: SKILLS_RunClientDoFunc, the one dispatcher of this table's `cltdofunc` slots, cannot pass NULL.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    return 1;
}


//1.10f: D2Client.0x6FABF580
//  `cltstfunc` 6: Dragon Talon starts. `retn 4`. It answers 0 without the skill's record or a used skill, and 1 otherwise. With a path target
//  Param1 is the row's `calc1`, the number of kicks; without one it is 0. The target is looked up before the used skill (0x6FABF5BD, saved at
//  [esp+10h]) and tested after it. The original inlines DATATBLS_GetSkillsTxtRecordById.
BOOL __fastcall SKILLS_CltStFunc6(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pTarget;
    D2SkillStrc *pSkill;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return FALSE;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return FALSE;

    if (!pTarget)
    {
        D2COMMON_SKILLS_SetParam1(pSkill, 0);
        return TRUE;
    }

    D2COMMON_SKILLS_SetParam1(pSkill, D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel));
    return TRUE;
}


//1.10f: D2Client.0x6FABF620
//  `cltdofunc` 4: Dragon Talon kicks again. `retn 4`. Without a used skill or a path target it answers 0 (the NULL itself, 0x6FABF630 and
//  0x6FABF640); otherwise 1. On the way it takes one kick off Param1 (`dec eax / test eax, eax / jle`, signed) and while what is left is above 0 it
//  stores it back and calls SKILLS_ReduceFixedFieldByPercent with 100, which sets the animation's precise frame to 0: the kick plays again. When
//  the count reaches 0 nothing is stored and nothing replayed, the animation runs out, and the next cast starts from `cltstfunc` 6, which sets
//  Param1 afresh. So Param1 is never written 0 here; it is left at 1.
int32_t __fastcall SKILLS_CltDoFunc4(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;
    int32_t nPar1;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    if (!CUNIT_GetPathTargetUnit(pUnit))
        return 0;

    nPar1 = D2COMMON_SKILLS_GetParam1(pSkill) - 1;
    if (nPar1 > 0)
    {
        D2COMMON_SKILLS_SetParam1(pSkill, nPar1);
        SKILLS_ReduceFixedFieldByPercent(pUnit, 100);
    }

    return 1;
}


//1.10f: D2Client.0x6FABF670
//  The client missile for a skill, by its charges. pUnit in ecx and nSkillId in edx, a bare `retn`. -1 without the skill's record (`or eax,
//  0FFFFFFFFh`, 0x6FABF69D). Otherwise the row's `cltmissilea` unless every test of the progressive ladder passes: the row's flag PROGRESSIVE (byte
//  0 of dwFlags against gdwBitMasks[2]), its `aurastate` in [0, states.txt's count) and its `aurastat1` in [0, itemstatcost.txt's count) -- the
//  lower bounds on the 16-bit words (`test dx, dx / jl`), the upper on their sign extensions (`movsx`, `jge`) -- a stat list on the unit for that
//  state, and a value of that stat above 1. Then the value, held to 3, indexes the five words `cltmissile`..`cltmissiled` from `cltmissile`: 2
//  charges pick `cltmissileb`, 3 or more `cltmissilec`. Each word is sign-extended.
//  A value of 1 or less takes the arm that sets eax to 1 and indexes with it (`mov eax, 1` at 0x6FABF6FA, then the load at 0x6FABF700), which is
//  `cltmissilea` again; the C writes that index, as the bytes do. The original inlines DATATBLS_GetSkillsTxtRecordById.
int32_t __fastcall SKILLS_GetClientMissileForSkill(D2UnitStrc *pUnit, D2C_Skills nSkillId)
{
    D2SkillsTxt *pSkillsTxt;
    D2StatListStrc *pStatList;
    const int16_t *pnCltMissiles;
    int32_t nState;
    int32_t nCharges;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return -1;

    if (!((uint8_t)pSkillsTxt->dwFlags[0] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_PROGRESSIVE]))
        return (int16_t)pSkillsTxt->wCltMissileA;

    if (pSkillsTxt->nAuraState < 0)
        return (int16_t)pSkillsTxt->wCltMissileA;

    nState = pSkillsTxt->nAuraState;
    if (nState >= sgptDataTables->nStatesTxtRecordCount)
        return (int16_t)pSkillsTxt->wCltMissileA;

    if (pSkillsTxt->wAuraStat[0] < 0 || pSkillsTxt->wAuraStat[0] >= sgptDataTables->nItemStatCostTxtRecordCount)
        return (int16_t)pSkillsTxt->wCltMissileA;

    pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, (D2C_States)nState);
    if (!pStatList)
        return (int16_t)pSkillsTxt->wCltMissileA;

    nCharges = D2COMMON_STATLIST_GetStatValue(pStatList, (D2C_ItemStats)pSkillsTxt->wAuraStat[0], 0);
    if (nCharges <= 1)
        nCharges = 1;
    else if (nCharges >= 3)
        nCharges = 3;

    pnCltMissiles = (const int16_t *)((const char *)pSkillsTxt + offsetof(D2SkillsTxt, wCltMissile));
    return pnCltMissiles[nCharges];
}


//1.10f: D2Client.0x6FABF730
//  The calc of a progressive skill, by its charges. pUnit in ecx, nSkillId in edx, nSkillLevel on the stack, `retn 4`. 0 without the skill's
//  record; otherwise D2Common's SKILLS_EvaluateSkillFormula over one of the row's `prgcalc1..3`: `prgcalc1` unless every test of
//  SKILLS_GetClientMissileForSkill's ladder passes, and then the stat's value held to 3, less one -- 2 charges `prgcalc2`, 3 or more `prgcalc3`
//  (`mov eax, [esi+eax*4+34h]`, 0x6FABF80D; the 1-or-less arm sets eax to 1 first, 0x6FABF7E7). The original inlines
//  DATATBLS_GetSkillsTxtRecordById.
int32_t __fastcall SKILLS_EvaluateProgressiveSkillCalc(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2StatListStrc *pStatList;
    int32_t nState;
    int32_t nCharges;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    if (!((uint8_t)pSkillsTxt->dwFlags[0] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_PROGRESSIVE]))
        return D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwPrgCalc[0], nSkillId, nSkillLevel);

    if (pSkillsTxt->nAuraState < 0)
        return D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwPrgCalc[0], nSkillId, nSkillLevel);

    nState = pSkillsTxt->nAuraState;
    if (nState >= sgptDataTables->nStatesTxtRecordCount)
        return D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwPrgCalc[0], nSkillId, nSkillLevel);

    if (pSkillsTxt->wAuraStat[0] < 0 || pSkillsTxt->wAuraStat[0] >= sgptDataTables->nItemStatCostTxtRecordCount)
        return D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwPrgCalc[0], nSkillId, nSkillLevel);

    pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, (D2C_States)nState);
    if (!pStatList)
        return D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwPrgCalc[0], nSkillId, nSkillLevel);

    nCharges = D2COMMON_STATLIST_GetStatValue(pStatList, (D2C_ItemStats)pSkillsTxt->wAuraStat[0], 0);
    if (nCharges <= 1)
        nCharges = 1;
    else if (nCharges >= 3)
        nCharges = 3;

    return D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwPrgCalc[nCharges - 1], nSkillId, nSkillLevel);
}


//1.10f: D2Client.0x6FABF850
//  `cltdofunc` 5: Shock Field. `retn 4`. Without the skill's record it answers 0 and touches nothing. With it, UNITFLAG_SKSRVDOFUNC is set on a
//  non-NULL unit at once (0x6FABF897..0x6FABF89F), before anything else can refuse; then the missile is SKILLS_GetClientMissileForSkill's (below 0
//  answers 0, signed), the count SKILLS_EvaluateProgressiveSkillCalc's (0 or less answers 0), the radius the row's `aurarangecalc`, and
//  SKILLS_SpawnClientEffectScatter draws that many of the missile round the aim point. The answer is then 1. The original inlines
//  DATATBLS_GetSkillsTxtRecordById.
int32_t __fastcall SKILLS_CltDoFunc5(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    int32_t nCount;
    int32_t nRadius;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    //  BUG: `test edi, edi` (0x6FABF893) guards only the flag; pUnit goes on to the four calls after it untested.
    //  UNREACHABLE: SKILLS_RunClientDoFunc cannot pass NULL.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0)
        return 0;

    nCount = SKILLS_EvaluateProgressiveSkillCalc(pUnit, nSkillId, nSkillLevel);
    if (nCount <= 0)
        return 0;

    nRadius = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwAuraRangeCalc, nSkillId, nSkillLevel);
    SKILLS_SpawnClientEffectScatter(pUnit, (D2C_MissilesHcIdx)nMissileId, nCount, nRadius, nSkillId, nSkillLevel);
    return 1;
}


//1.10f: D2Client.0x6FABF910
//  `cltstfunc` 7: can Cloak of Shadows start. `retn 4`. 1 when the row's `aurastate` is in [0, states.txt's count) -- `test ax, ax / jl` on the
//  word, `movsx` and `jge` on its extension -- and the unit is not in that state (D2Common's STATES_CheckState, its answer turned into `== 0` by
//  `neg / sbb / inc`, 0x6FABF959..0x6FABF95E); 0 otherwise, and without the skill's record. The original inlines DATATBLS_GetSkillsTxtRecordById.
BOOL __fastcall SKILLS_CltStFunc7(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nState;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return FALSE;

    if (pSkillsTxt->nAuraState < 0)
        return FALSE;

    nState = pSkillsTxt->nAuraState;
    if (nState >= sgptDataTables->nStatesTxtRecordCount)
        return FALSE;

    return D2COMMON_STATES_CheckState(pUnit, (D2C_States)nState) == 0;
}


//1.10f: D2Client.0x6FABF970
//  The body of `cltstfunc` 8: Blade Fury's channel, begun or kept. `retn 4`, never reading nSkillLevel; reached only through SKILLS_CltStFunc8's
//  `jmp`. 0 without the skill's record. Then:
//    * when the unit is not in STATE_INFERNO, the channel's start cues: the row's `prgoverlay` through GFX_AddOverlay, kind 2, when it is in
//      (0, overlay.txt's count), and its `prgsound` through SOUNDINST_PlaySound with no unit, when it is above 0; both words signed (`jle`);
//    * when the unit already holds a STATE_INFERNO stat list, the channel goes on: the animation frame is set to the row's `seqinput` less one
//      (the byte zero-extended, 0x6FABFA8C), dwFrameCountPrecise to 800h, the list's expire frame to 7 (D2Common #10476), and the answer is 1;
//    * otherwise it begins: a stat list is allocated (filter 2, expire frame 14h, the unit's type and id, or 6 and -1 for no unit), the unit's
//      highest-level skill of this id gets Param1 0, the list is posted to the unit with ITEM_OnUninterruptableStateRemoved as its remove callback
//      and STATE_INFERNO as its state, the state is toggled on, and the answer is 1. A failed allocation, or no such skill, answers 0.
//  The original inlines DATATBLS_GetSkillsTxtRecordById.
BOOL __fastcall SKILLS_StartChannelState(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2StatListStrc *pChannelStats;
    D2StatListStrc *pAllocStats;
    D2SkillStrc *pSkill;
    D2C_UnitTypes dwUnitType;
    D2UnitGUID dwUnitId;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return FALSE;

    if (!D2COMMON_STATES_CheckState(pUnit, STATE_INFERNO))
    {
        if ((int16_t)pSkillsTxt->wPrgOverlay > 0 && (int16_t)pSkillsTxt->wPrgOverlay < sgptDataTables->nOverlayTxtRecordCount)
            GFX_AddOverlay(pUnit, (D2_OverlayIds)(int16_t)pSkillsTxt->wPrgOverlay, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);

        if ((int16_t)pSkillsTxt->wPrgSound > 0)
            SOUNDINST_PlaySound((D2C_Sounds)(int16_t)pSkillsTxt->wPrgSound, NULL, 0, 0, 0);
    }

    pChannelStats = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, STATE_INFERNO);
    if (pChannelStats)
    {
        D2COMMON_UNITS_SetAnimationFrame(pUnit, pSkillsTxt->nSeqInput - 1);
        pUnit->dwFrameCountPrecise = 0x800;
        D2COMMON_10476(pChannelStats, 7);
        return TRUE;
    }

    //  BUG: `test esi, esi` (0x6FABFA11) arms the no-unit type and id, after pUnit has gone to STATES_CheckState and
    //  STATLIST_GetStatListFromUnitAndState. UNREACHABLE: SKILLS_RunCltStFunc cannot pass NULL.
    if (pUnit)
    {
        dwUnitId = pUnit->dwUnitId;
        dwUnitType = pUnit->dwUnitType;
    }
    else
    {
        dwUnitId = (D2UnitGUID)-1;
        dwUnitType = UNIT_TYPES_COUNT;
    }

    pAllocStats = D2COMMON_STATLIST_AllocStatList(NULL, 2u, 0x14u, dwUnitType, dwUnitId);
    if (!pAllocStats)
        return FALSE;

    //  BUG: with no skill of this id on the unit the stat list just allocated is dropped: the refusal at 0x6FABFA4B returns without
    //  freeing or posting it. UNREACHABLE: SKILLS_RunCltStFunc gives the unit a skill of this id before any start function runs, or
    //  answers 0.
    pSkill = D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, nSkillId);
    if (!pSkill)
        return FALSE;

    D2COMMON_SKILLS_SetParam1(pSkill, 0);
    D2COMMON_10475_PostStatToStatList(pUnit, pAllocStats, TRUE);
    D2COMMON_STATLIST_SetStatRemoveCallback(pAllocStats, ITEM_OnUninterruptableStateRemoved);
    D2COMMON_STATLIST_SetState(pAllocStats, STATE_INFERNO);
    D2COMMON_STATES_ToggleState(pUnit, STATE_INFERNO, TRUE);
    return TRUE;
}


//1.10f: D2Client.0x6FABFAC0
//  `cltstfunc` 8: can Blade Fury start. 5 bytes, a `jmp` to SKILLS_StartChannelState with the three arguments where they arrived. Written as the
//  call; the compiler may keep it a tail jump.
BOOL __fastcall SKILLS_CltStFunc8(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    return SKILLS_StartChannelState(pUnit, nSkillId, nSkillLevel);
}


//1.10f: D2Client.0x6FABFAD0
//  `cltdofunc` 6: Blade Fury throws. `retn 4`. It answers 0 without the skill's record, without a missile from SKILLS_GetClientMissileForSkill
//  (below 0, `jge` at 0x6FABFB22), without the unit's skill of this id (the NULL itself, 0x6FABFB43), or when SKILLS_EvaluateProgressiveSkillCalc
//  answers 0 or less (`jg`, 0x6FABFB64); otherwise it clears UNITFLAG_SKSRVDOFUNC on a non-NULL unit (`and al, 0BFh` on the whole dword,
//  0x6FABFB76..0x6FABFB7E), takes the aim point, and answers 1.
//  Param1 is a countdown. While it is above 0 it is taken down by one and nothing is thrown (`test eax, eax / jg`, 0x6FABFBA8); at 0 or below, a
//  missile goes from the unit to the aim point -- the block cleared (17h dwords, `rep stosd`), flags 20h, owner and origin the unit, the missile,
//  the skill and its level, the target point -- is drawn 15 above the ground (PARTICLE_SetUnitGfxCoords, 0x6FABFBFD), and Param1 is reloaded with
//  the calc: Blade Fury's `prgcalc1` is `par4`, 5 in stock Skills.txt. So after a throw the next five calls count down and the sixth throws; the
//  two SetParam1 calls are one call site the two arms jump to (0x6FABFC11). In STATE_INFERNO, the channel SKILLS_StartChannelState keeps, the
//  animation is snapped as there: frame `seqinput` less one, precise frame 800h (0x6FABFC22..0x6FABFC33). The original inlines
//  DATATBLS_GetSkillsTxtRecordById.
int32_t __fastcall SKILLS_CltDoFunc6(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2SkillStrc *pSkill;
    D2UnitStrc *pMissile;
    D2MissileStrc tParams;
    int32_t nMissileId;
    int32_t nReload;
    int32_t nCountdown;
    int32_t nTargetX;
    int32_t nTargetY;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0)
        return 0;

    pSkill = D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, nSkillId);
    if (!pSkill)
        return 0;

    nReload = SKILLS_EvaluateProgressiveSkillCalc(pUnit, nSkillId, nSkillLevel);
    if (nReload <= 0)
        return 0;

    //  BUG: `test ebx, ebx` (0x6FABFB72) guards only the flag, pUnit having gone to three calls already. UNREACHABLE: SKILLS_RunClientDoFunc cannot
    //  pass NULL.
    if (pUnit)
        pUnit->dwFlags &= ~UNITFLAG_SKSRVDOFUNC;

    //  BUG: the refusal after SKILLS_GetPathTargetPos (0x6FABFB96) tests an answer that is always 1. UNREACHABLE:
    //  SKILLS_GetPathTargetPos answers 1 on every path.
    if (!SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY))
        return 0;

    nCountdown = D2COMMON_SKILLS_GetParam1(pSkill);
    if (nCountdown > 0)
    {
        D2COMMON_SKILLS_SetParam1(pSkill, nCountdown - 1);
    }
    else
    {
        memset(&tParams, 0, sizeof(tParams));
        tParams.nMissile = nMissileId;
        tParams.dwFlags = MISSILESPAWN_TARGETPOS;
        tParams.pOwner = pUnit;
        tParams.pOrigin = pUnit;
        tParams.nSkill = nSkillId;
        tParams.nSkillLevel = nSkillLevel;
        tParams.nTargetX = nTargetX;
        tParams.nTargetY = nTargetY;
        pMissile = MISSILE_ClientSpawn(&tParams);
        if (pMissile)
            PARTICLE_SetUnitGfxCoords(pMissile, 0, 0, 15);

        D2COMMON_SKILLS_SetParam1(pSkill, nReload);
    }

    if (D2COMMON_STATES_CheckState(pUnit, STATE_INFERNO))
    {
        D2COMMON_UNITS_SetAnimationFrame(pUnit, pSkillsTxt->nSeqInput - 1);
        pUnit->dwFrameCountPrecise = 0x800;
    }

    return 1;
}


//1.10f: D2Client.0x6FABFC50
//  `cltstfunc` 9: Dragon Tail starts. `retn 4`, always 1. It gives the unit the row's `par4` as a temporary attack rate
//  (SKILLS_ApplyAttackRateStat; Dragon Tail's is -40 in stock Skills.txt), or 0 without the skill's record; the bytes make the call in each arm
//  (0x6FABFC7D, 0x6FABFC93) and the C once, with the value chosen. The original inlines DATATBLS_GetSkillsTxtRecordById.
BOOL __fastcall SKILLS_CltStFunc9(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    SKILLS_ApplyAttackRateStat(pUnit, pSkillsTxt ? pSkillsTxt->dwParam[3] : 0);
    return TRUE;
}


//1.10f: D2Client.0x6FABFCB0
//  `cltdofunc` 7: Dragon Tail kicks. `retn 4`. Without a path target it answers 0 (the NULL itself, 0x6FABFCBF); without the skill's record, or
//  without a missile from SKILLS_GetClientMissileForSkill (below 0), 0; otherwise SKILLS_MissileClientSpawnOnTargetUnit sends that missile (Dragon
//  Tail's `cltmissilea`, `dragontail missile`) at the target, its answer unread, UNITFLAG_SKSRVDOFUNC is set on a non-NULL unit after the spawn
//  (0x6FABFD1A..0x6FABFD22, the whole dword), and the answer is 1. The record is looked up only to be tested. The original inlines
//  DATATBLS_GetSkillsTxtRecordById.
int32_t __fastcall SKILLS_CltDoFunc7(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    int32_t nMissileId;

    if (!CUNIT_GetPathTargetUnit(pUnit))
        return 0;

    if (!DATATBLS_GetSkillsTxtRecordById(nSkillId))
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0)
        return 0;

    SKILLS_MissileClientSpawnOnTargetUnit(pUnit, (D2C_MissilesHcIdx)nMissileId, nSkillId, nSkillLevel);

    //  BUG: `test edi, edi` (0x6FABFD16) guards only the flag, pUnit having gone to three calls already. UNREACHABLE: SKILLS_RunClientDoFunc cannot
    //  pass NULL.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    return 1;
}


//1.10f: D2Client.0x6FABFD40
//  `cltdofunc` 8: Mind Blast's stream. `retn 4`. It answers 0 without the skill's record, without the aim point, when the row's `cltmissilea` is
//  negative (the word sign-extended, `jge`, 0x6FABFDAB) or its `cltmissileb` is (a signed word compare with 0, 0x6FABFDB8); otherwise 1, whether or
//  not a missile came of it.
//  The row's `aurarangecalc` is evaluated first (Mind Blast's is `par7`) and kept in nSkillLevel's own stack slot (0x6FABFDEE, after the level was
//  loaded into ebp); then the block is cleared and filled -- flags 1, the owner, the aim point as the missile's position (nX, nY), the skill and
//  its level, `cltmissilea` (`mindblast center`) -- and MISSILE_ClientSpawn makes the head of the stream. On a spawned missile D2Common's
//  MISSILE_SetStreamMissile gets `cltmissileb` (`mindblast hit`, sign-extended) and MISSILE_SetStreamRange the evaluated range: the bytes push both
//  whole (0x6FABFE3C, 0x6FABFE47), and the callees take 16-bit values, so the C's casts to the declared widths hand them the same low half. The
//  original inlines DATATBLS_GetSkillsTxtRecordById.
int32_t __fastcall SKILLS_CltDoFunc8(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pMissile;
    D2MissileStrc tParams;
    int32_t nHeadMissileId;
    int32_t nStreamRange;
    int32_t nTargetX;
    int32_t nTargetY;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    //  BUG: the refusal after SKILLS_GetPathTargetPos (0x6FABFD93) tests an answer that is always 1. UNREACHABLE:
    //  SKILLS_GetPathTargetPos answers 1 on every path.
    if (!SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY))
        return 0;

    nHeadMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (nHeadMissileId < 0)
        return 0;

    if ((int16_t)pSkillsTxt->wCltMissileB < 0)
        return 0;

    nStreamRange = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwAuraRangeCalc, nSkillId, nSkillLevel);

    memset(&tParams, 0, sizeof(tParams));
    tParams.pOwner = pUnit;
    tParams.nY = nTargetY;
    tParams.dwFlags = MISSILESPAWN_STARTPOS;
    tParams.nX = nTargetX;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = nHeadMissileId;
    pMissile = MISSILE_ClientSpawn(&tParams);
    if (pMissile)
    {
        D2COMMON_MISSILE_SetStreamMissile(pMissile, pSkillsTxt->wCltMissileB);
        D2COMMON_MISSILE_SetStreamRange(pMissile, (int16_t)nStreamRange);
    }

    return 1;
}


//1.10f: D2Client.0x6FABFE60
//  `cltstfunc` 10, which no stock Skills.txt row names. `retn 4`, reading neither nSkillId nor nSkillLevel. 1 when the unit's path target is a
//  monster or a player (`cmp eax, 1 / jz`, then `test eax, eax / jz`, 0x6FABFE6E..0x6FABFE75, in that order), 0 for any other target, and the NULL
//  itself without one.
BOOL __fastcall SKILLS_CltStFunc10(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2UnitStrc *pTarget;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget)
        return FALSE;

    return pTarget->dwUnitType == UNIT_MONSTER || pTarget->dwUnitType == UNIT_PLAYER;
}


//1.10f: D2Client.0x6FABFE90
//  Missiles scattered over a disc. pUnit in ecx and nSeed in edx, `retn 18h` for the six on the stack; no answer. Its one caller is `cltdofunc` 9.
//  A seed of its own is started from nSeed (D2Common's SEED_InitLowSeed on a local D2SeedStrc), the block is cleared and filled once -- flags 1,
//  the owner, the missile, the skill and its level -- and the unit's room is taken. Then nRadius * nRadius attempts (`imul ebx, ebx / test / jle`,
//  0x6FABFEB0..0x6FABFEFB, and the count taken down to 0 by `dec / jnz`, 0x6FAC009A..0x6FAC009F), each:
//    * two rolls below 2 * nRadius, each subtracted from nRadius: an offset in (-nRadius, nRadius] on each axis;
//    * the point kept when X * X + Y * Y is at most nRadius * nRadius (`jg`, 0x6FABFFBC, signed), and written into the block's position;
//    * then spawned only when the unit has a room and the point lies in it -- X and Y from the room's subtile origin to its origin plus its width
//      and height, four signed compares (0x6FABFFED..0x6FAC0006) -- or in the first of the rooms D2Common's DUNGEON_GetAdjacentRoomsListFromRoom
//      lists for it that is not NULL and holds it; the list's count is compared UNSIGNED (`jbe`, 0x6FAC003B, and `jb`, 0x6FAC007B), and its two
//      out-parameters are the stack slots of nSkillId and nSkillLevel, set to 0 first (0x6FAC001F, 0x6FAC0026) -- the C gives them locals of their
//      own.
//  So about pi/4 of the nRadius * nRadius attempts land. The C computes the two squares and the attempt count in unsigned arithmetic, cast back for
//  the signed compares the bytes make: `imul` wraps where a signed C product would be undefined. No value a skill gives makes the difference show.
//  The original inlines SEED_RollLimitedRandomNumber for each of the two rolls.
void __fastcall SKILLS_ScatterClientMissilesInDisc(D2UnitStrc *pUnit, int32_t nSeed, D2C_MissilesHcIdx nMissileId, int32_t nCenterX, int32_t nCenterY, int32_t nRadius, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SeedStrc tSeed;
    D2MissileStrc tParams;
    D2ActiveRoomStrc *pUnitRoom;
    D2ActiveRoomStrc *pSpawnRoom;
    D2ActiveRoomStrc *pRoom;
    D2ActiveRoomStrc **ppRoomList;
    int32_t nNumRooms;
    uint32_t iRoom;
    int32_t nRadiusSquared;
    int32_t nAttemptsLeft;
    int32_t nSpan;
    int32_t nOffsetX;
    int32_t nOffsetY;
    int32_t nX;
    int32_t nY;

    D2COMMON_SEED_InitLowSeed(&tSeed, nSeed);
    nRadiusSquared = (int32_t)((uint32_t)nRadius * (uint32_t)nRadius);
    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS;
    tParams.pOwner = pUnit;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = nMissileId;
    pUnitRoom = D2COMMON_UNITS_GetRoom(pUnit);
    if (nRadiusSquared <= 0)
        return;

    nAttemptsLeft = nRadiusSquared;
    do
    {
        nSpan = (int32_t)(2u * (uint32_t)nRadius);

        nOffsetX = nRadius - SEED_RollLimitedRandomNumber(&tSeed, nSpan);

        nOffsetY = nRadius - SEED_RollLimitedRandomNumber(&tSeed, nSpan);

        if ((int32_t)((uint32_t)nOffsetY * (uint32_t)nOffsetY + (uint32_t)nOffsetX * (uint32_t)nOffsetX) > nRadiusSquared)
            continue;

        nX = nCenterX + nOffsetX;
        nY = nCenterY + nOffsetY;
        tParams.nX = nX;
        tParams.nY = nY;
        if (!pUnitRoom)
            continue;

        pSpawnRoom = NULL;
        if (nX >= pUnitRoom->tCoords.nSubtileX && nX < pUnitRoom->tCoords.nSubtileX + pUnitRoom->tCoords.nSubtileWidth && nY >= pUnitRoom->tCoords.nSubtileY && nY < pUnitRoom->tCoords.nSubtileY + pUnitRoom->tCoords.nSubtileHeight)
        {
            pSpawnRoom = pUnitRoom;
        }
        else
        {
            ppRoomList = NULL;
            nNumRooms = 0;
            D2COMMON_DUNGEON_GetAdjacentRoomsListFromRoom(pUnitRoom, &ppRoomList, &nNumRooms);
            for (iRoom = 0; iRoom < (uint32_t)nNumRooms; ++iRoom)
            {
                pRoom = ppRoomList[iRoom];
                if (pRoom && nX >= pRoom->tCoords.nSubtileX && nX < pRoom->tCoords.nSubtileX + pRoom->tCoords.nSubtileWidth && nY >= pRoom->tCoords.nSubtileY && nY < pRoom->tCoords.nSubtileY + pRoom->tCoords.nSubtileHeight)
                {
                    pSpawnRoom = ppRoomList[iRoom];
                    break;
                }
            }
        }

        if (pSpawnRoom)
            MISSILE_ClientSpawn(&tParams);
    } while (--nAttemptsLeft);
}


static_assert(PATHTYPE_CHARGEDBOLT == 10 && offsetof(D2PathPointStrc, Y) == 2,
              "SKILLS_MissileInitClientNovaBolt and SKILLS_MissileBuildWaveringPath: `push 0Ah` into PATH_SetType (0x6FAC04B9, "
              "0x6FAC0732); `mov [ecx+edi*4+2], dx` (0x6FAC0878)");


//1.10f: D2Client.0x6FAC00B0
//  `cltdofunc` 9: Fists of Fire and Blades of Ice release two or three charges (both skills' `cltprgfunc2` and `cltprgfunc3`). `retn 4`. It answers
//  0 without the skill's record or a missile (below 0, `jge` at 0x6FAC00FE); otherwise 1, after a disc of that missile around the target. The
//  radius is SKILLS_EvaluateProgressiveSkillCalc's -- `prgcalc2`/`prgcalc3` are `par1` and `par2`, 4 and 4 for Fists of Fire, 6 and 3 for Blades of
//  Ice in stock -- and the row's `aurarangecalc` only when that is 0 or less (`jg`, 0x6FAC011E). The centre is the path target's position, or
//  SKILLS_GetPathTargetPos' point without one, and its X is also the scatter's seed: the `mov edx, [esp+1Ch+pnX]` at 0x6FAC01A7 feeds nSeed and the
//  `push edx` at 0x6FAC01B3 nCenterX, so every client draws the same disc for the same target column. The original inlines
//  DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition.
int32_t __fastcall SKILLS_CltDoFunc9(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pTarget;
    int32_t nMissileId;
    int32_t nRadius;
    int32_t nTargetX;
    int32_t nTargetY;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0)
        return 0;

    nRadius = SKILLS_EvaluateProgressiveSkillCalc(pUnit, nSkillId, nSkillLevel);
    if (nRadius <= 0)
        nRadius = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwAuraRangeCalc, nSkillId, nSkillLevel);

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (pTarget)
    {
        nTargetX = UNITS_GetXPosition(pTarget);

        nTargetY = UNITS_GetYPosition(pTarget);
    }
    //  BUG: the refusal after SKILLS_GetPathTargetPos (0x6FAC01DD) tests an answer that is always 1. UNREACHABLE:
    //  SKILLS_GetPathTargetPos answers 1 on every path.
    else if (!SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY))
    {
        return 0;
    }

    SKILLS_ScatterClientMissilesInDisc(pUnit, nTargetX, (D2C_MissilesHcIdx)nMissileId, nTargetX, nTargetY, nRadius, nSkillId, nSkillLevel);
    return 1;
}


//1.10f: D2Client.0x6FAC01F0
//  `cltdofunc` 10: Claws of Thunder releases two charges (its `cltprgfunc2`). `retn 4`. Without the skill's record, a missile (below 0), or a path
//  target it answers 0 (without a target the NULL itself, 0x6FAC024E); otherwise a ring of the missile -- `cltmissileb`, `clawsofthundernova` --
//  goes out from the target through SKILLS_SpawnMissileRingFromUnit, with no second missile (-1), at missiles.txt's velocity for the level
//  (D2Common's DATATBLS_GetMissileVelocityFromMissilesTxt), and it answers 1. The record is looked up only to be tested.
//  It is also Nova's and Frost Nova's item effect: both rows carry `ItemCltEffect` 10, and SKILLS_RunClientDoFunc calls that slot in place of
//  `cltdofunc` when bUseTarget is set -- an item's skill cast by its attacker on a hit -- so such a nova goes out as this function's ring, from the
//  target, of the row's `cltmissilea` (`nova`, `frostnova`; neither row is progressive). The original inlines DATATBLS_GetSkillsTxtRecordById.
int32_t __fastcall SKILLS_CltDoFunc10(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2UnitStrc *pTarget;
    int32_t nMissileId;

    if (!DATATBLS_GetSkillsTxtRecordById(nSkillId))
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0)
        return 0;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget)
        return 0;

    SKILLS_SpawnMissileRingFromUnit(pUnit, pTarget, (D2C_MissilesHcIdx)nMissileId, (D2C_MissilesHcIdx)-1, nSkillId, nSkillLevel, D2COMMON_DATATBLS_GetMissileVelocityFromMissilesTxt(nMissileId, nSkillLevel));
    return 1;
}


//1.10f: D2Client.0x6FAC0280
//  `cltdofunc` 11: Claws of Thunder releases three charges (its `cltprgfunc3`). `retn 4`. Without the skill's record, a missile (below 0) or a path
//  target it answers 0 (without a target the NULL itself, 0x6FAC02EF); otherwise 1, after a nova from the target's position:
//    * the step is SKILLS_EvaluateProgressiveSkillCalc's -- `prgcalc3` is `par1`, 4 in stock -- or the row's `aurarangecalc` when that is 0 or
//      less (`jg`, 0x6FAC0310);
//    * a seed of its own is started from the caster's dwInitSeed (read at 0x6FAC032C, before the target's position), and one block is filled --
//      flags 3, the owner, the target's position as the missile's, the missile (`cltmissilec`, `clawsofthunderbolt`), the skill and its level, and
//      SKILLS_MissileInitClientNovaBolt as the block's initialiser;
//    * then every step-th of the 64 points of gnDir64CircleX/Y, a circle of radius 30, is a missile's target, with the next roll of the seed as
//      the initialiser's argument, and each missile's target X is set to the row's `par2` plus 1 (0 plus 1 without the record; Claws of Thunder's
//      `par2` is empty in stock).
//  The loop steps a BYTE offset -- `lea edi, [ecx*4]` (0x6FAC03E5), then `add esi, edi / cmp esi, 100h / jl` (0x6FAC0461..0x6FAC0469) -- and the C
//  does too, in unsigned arithmetic cast back for the signed compare. It is `cltdofunc` 83 (below) but for three instructions: the initialiser's
//  address and the two tables'; the original has no function the two share, so both bodies are written out. The original inlines
//  DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollRandomNumber.
int32_t __fastcall SKILLS_CltDoFunc11(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2SkillsTxt *pSkillsTxtAgain;
    D2UnitStrc *pTarget;
    D2UnitStrc *pMissile;
    D2SeedStrc tSeed;
    D2MissileStrc tParams;
    uint32_t dwInitSeed;
    int32_t nMissileId;
    int32_t nStep;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nOffset;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0)
        return 0;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget)
        return 0;

    nStep = SKILLS_EvaluateProgressiveSkillCalc(pUnit, nSkillId, nSkillLevel);
    if (nStep <= 0)
        nStep = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwAuraRangeCalc, nSkillId, nSkillLevel);

    dwInitSeed = pUnit->dwInitSeed;

    nTargetX = UNITS_GetXPosition(pTarget);

    nTargetY = UNITS_GetYPosition(pTarget);

    D2COMMON_SEED_InitLowSeed(&tSeed, (int32_t)dwInitSeed);
    memset(&tParams, 0, sizeof(tParams));
    tParams.nX = nTargetX;
    tParams.pOwner = pUnit;
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET;
    tParams.nY = nTargetY;
    tParams.nMissile = nMissileId;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.pInitFunc = SKILLS_MissileInitClientNovaBolt;

    //  BUG: a step of 0 or less never leaves the loop: the offset stays below 100h, or runs backwards through the memory before the table. LATENT:
    //  slot 11 is Claws of Thunder's `cltprgfunc3` alone, and SKILLS_ApplyClientSkillEffect sets its charges to 3 before the call, so the step is
    //  `par1`, 4 -- its `prgcalc1`, `prgcalc2` and `aurarangecalc` are empty, and any other count would take one of them.
    //  DIFFERENCE: the circle is ours (SkillsAss.h): a step of 0 reads entry 0 for ever, as the original did; a negative one walked back through the
    //  .rdata below the original's table, and would read below ours, outside the array, which C++ leaves undefined. Declared, not reproduced.
    for (nOffset = 0; nOffset < 0x100; nOffset = (int32_t)((uint32_t)nOffset + 4u * (uint32_t)nStep))
    {
        tParams.nTargetY = *(const int32_t *)((const char *)gnDir64CircleY + nOffset);
        tParams.nTargetX = *(const int32_t *)((const char *)gnDir64CircleX + nOffset);

        tParams.pInitArgs = (int32_t)(uint32_t)SEED_RollRandomNumber(&tSeed);

        pMissile = MISSILE_ClientSpawn(&tParams);

        pSkillsTxtAgain = DATATBLS_GetSkillsTxtRecordById(nSkillId);

        //  BUG: MISSILE_ClientSpawn's answer goes to D2COMMON_MISSILE_SetTargetX untested (0x6FAC045B). REFUTED: the callee tests
        //  the unit and returns (0x6FDBA494).
        D2COMMON_MISSILE_SetTargetX(pMissile, (pSkillsTxtAgain ? pSkillsTxtAgain->dwParam[1] : 0) + 1);
    }

    return 1;
}


//1.10f: D2Client.0x6FAC0480
//  The nova bolt's initialiser. pMissile in ecx and nLowSeed in edx, a bare `retn`; nothing without a missile. Installed by `cltdofunc` 11 as its
//  block's initialiser (the `offset` at 0x6FAC03DD is its one reference), which MISSILE_ClientSpawn calls with the block's argument. A missile
//  given 78 frames or more by missiles.txt is cut to 77, total and current (`cmp ebx, 4Eh / jl`, 0x6FAC0494, signed); the missile's own seed is set
//  to nLowSeed; and its path becomes a charged bolt's (PATHTYPE_CHARGEDBOLT, 10) of that many steps, and starts. D2Common's PATH_SetNewDistance
//  takes the count as a byte; the bytes push the whole register (0x6FAC04C1), and 77 or fewer fit.
void __fastcall SKILLS_MissileInitClientNovaBolt(D2UnitStrc *pMissile, int32_t nLowSeed)
{
    D2DynamicPathStrc *pPath;
    int32_t nFrames;

    if (!pMissile)
        return;

    nFrames = D2COMMON_MISSILE_GetTotalFrames(pMissile);
    if (nFrames >= 78)
    {
        nFrames = 77;
        D2COMMON_MISSILE_SetTotalFrames(pMissile, 77);
        D2COMMON_MISSILE_SetCurrentFrame(pMissile, 77);
    }

    pPath = pMissile->pDynamicPath;
    D2COMMON_SEED_InitLowSeed(&pMissile->pSeed, nLowSeed);
    D2COMMON_PATH_SetType(pPath, PATHTYPE_CHARGEDBOLT);
    D2COMMON_PATH_SetNewDistance(pPath, (uint8_t)nFrames);
    D2COMMON_PATH_StartUnitMove(pPath, pMissile, 0);
}


//1.10f: D2Client.0x6FAC04E0
//  `cltdofunc` 83: Royal Strike releases two charges (its `cltprgfunc2`). `retn 4`. `cltdofunc` 11 above, instruction for instruction, but for
//  three:
//  the initialiser is SKILLS_MissileBuildWaveringPath (0x6FAC063D) and the circle gnRoyalStrikeCircleX/Y (0x6FAC064E, 0x6FAC0654), a second copy of
//  gnDir64CircleX/Y with the same 64 values. Royal Strike's `prgcalc2` is `par6`, 10 in stock, so seven missiles, and its `cltmissileb`
//  `royalstrikechainlightning`; its `par2`, 6, gives each a target X of 7. The original inlines DATATBLS_GetSkillsTxtRecordById,
//  UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollRandomNumber.
int32_t __fastcall SKILLS_CltDoFunc83(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2SkillsTxt *pSkillsTxtAgain;
    D2UnitStrc *pTarget;
    D2UnitStrc *pMissile;
    D2SeedStrc tSeed;
    D2MissileStrc tParams;
    uint32_t dwInitSeed;
    int32_t nMissileId;
    int32_t nStep;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nOffset;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0)
        return 0;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget)
        return 0;

    nStep = SKILLS_EvaluateProgressiveSkillCalc(pUnit, nSkillId, nSkillLevel);
    if (nStep <= 0)
        nStep = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwAuraRangeCalc, nSkillId, nSkillLevel);

    dwInitSeed = pUnit->dwInitSeed;

    nTargetX = UNITS_GetXPosition(pTarget);

    nTargetY = UNITS_GetYPosition(pTarget);

    D2COMMON_SEED_InitLowSeed(&tSeed, (int32_t)dwInitSeed);
    memset(&tParams, 0, sizeof(tParams));
    tParams.nX = nTargetX;
    tParams.pOwner = pUnit;
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET;
    tParams.nY = nTargetY;
    tParams.nMissile = nMissileId;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.pInitFunc = SKILLS_MissileBuildWaveringPath;

    //  BUG: a step of 0 or less never leaves the loop, as in `cltdofunc` 11. LATENT: whichever calc the ladder picks for Royal
    //  Strike, 0 or less falls back to its `aurarangecalc`, `par1` = 8.
    //  DIFFERENCE: a negative step's reads below the table (SkillsAss.h) are declared, not reproduced, as in `cltdofunc` 11.
    for (nOffset = 0; nOffset < 0x100; nOffset = (int32_t)((uint32_t)nOffset + 4u * (uint32_t)nStep))
    {
        tParams.nTargetY = *(const int32_t *)((const char *)gnRoyalStrikeCircleY + nOffset);
        tParams.nTargetX = *(const int32_t *)((const char *)gnRoyalStrikeCircleX + nOffset);

        tParams.pInitArgs = (int32_t)(uint32_t)SEED_RollRandomNumber(&tSeed);

        pMissile = MISSILE_ClientSpawn(&tParams);

        pSkillsTxtAgain = DATATBLS_GetSkillsTxtRecordById(nSkillId);

        //  BUG: pMissile goes to D2COMMON_MISSILE_SetTargetX untested (0x6FAC06BB). REFUTED: the callee tests the unit and
        //  returns, as in `cltdofunc` 11.
        D2COMMON_MISSILE_SetTargetX(pMissile, (pSkillsTxtAgain ? pSkillsTxtAgain->dwParam[1] : 0) + 1);
    }

    return 1;
}


//1.10f: D2Client.0x6FAC06E0
//  The wavering path of Royal Strike's chain lightning. pMissile in ecx and nSeed in edx, a bare `retn`; nothing without a missile. Installed by
//  `cltdofunc` 83 as its block's initialiser (the `offset` at 0x6FAC063D is its one reference). It lays out the path point by point:
//    * the frames cut to 77 as SKILLS_MissileInitClientNovaBolt cuts them, the missile's seed set to nSeed, the path made a charged bolt's of that
//      many steps, and its point array fetched (D2Common's PATH_GetPathPoints);
//    * the start is the missile's own position and the first heading its direction to the path's first point (D2Common's
//      UNITS_GetDirectionToCoords, the answer's low byte, `and ebx, 0FFh`, 0x6FAC07D2);
//    * two rolls of the missile's seed: the first one's low bit picks the turn's sign, +1 or -1 (`and ecx, 1 / setnz dl / lea edx, [edx+edx-1]`,
//      0x6FAC07E2..0x6FAC07F1), the second's remainder by 3 (`div`, unsigned) plus gnWaverBaseTurn (2) the turn per point;
//    * then for each frame the heading turns by sign times turn, D2Common's #11053 maps it (`& 3Fh`, 64 directions) to one of 8, and the point
//      steps by gnDir8StepX/Y -- (2,0), (2,2), (0,2), (-2,2) and on round -- stored as two 16-bit halves (0x6FAC0868, 0x6FAC0878);
//    * after each point whose index divides by gnWaverFlipFrames (15) -- `cdq / idiv`, SIGNED, 0x6FAC087D -- the sign flips and the turn is rolled
//      again. Point 0 divides too, so the first swing is one point long and every later swing 15;
//    * the path's point count is set to the frames.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition, and SEED_RollRandomNumber and SEED_RollLimitedRandomNumber for the two rolls
//  before the loop (written out here; the first never stores the high half it hands the second in a register) and SEED_RollLimitedRandomNumber for
//  the re-roll at each flip.
void __fastcall SKILLS_MissileBuildWaveringPath(D2UnitStrc *pMissile, int32_t nSeed)
{
    D2DynamicPathStrc *pPath;
    D2PathPointStrc *pPathPoints;
    uint64_t lSeed;
    uint32_t nDir8;
    int32_t nFrames;
    int32_t nX;
    int32_t nY;
    int32_t nFirstX;
    int32_t nFirstY;
    int32_t nDirection;
    int32_t nTurnSign;
    int32_t nTurnStep;
    int32_t iPoint;

    if (!pMissile)
        return;

    nFrames = D2COMMON_MISSILE_GetTotalFrames(pMissile);
    if (nFrames >= 78)
    {
        nFrames = 77;
        D2COMMON_MISSILE_SetTotalFrames(pMissile, 77);
        D2COMMON_MISSILE_SetCurrentFrame(pMissile, 77);
    }

    pPath = pMissile->pDynamicPath;
    D2COMMON_SEED_InitLowSeed(&pMissile->pSeed, nSeed);
    D2COMMON_PATH_SetType(pPath, PATHTYPE_CHARGEDBOLT);
    D2COMMON_PATH_SetNewDistance(pPath, (uint8_t)nFrames);
    D2COMMON_PATH_GetPathPoints(pPath, &pPathPoints);

    nX = UNITS_GetXPosition(pMissile);

    nY = UNITS_GetYPosition(pMissile);

    nFirstY = D2COMMON_10176_PathGetFirstPointY(pMissile->pDynamicPath);
    nFirstX = D2COMMON_10175_PathGetFirstPointX(pMissile->pDynamicPath);
    nDirection = (uint8_t)D2COMMON_UNITS_GetDirectionToCoords(pMissile, nFirstX, nFirstY);

    lSeed = (uint64_t)pMissile->pSeed.nLowSeed * 0x6AC690C5u + pMissile->pSeed.nHighSeed;
    pMissile->pSeed.nLowSeed = (uint32_t)lSeed;
    nTurnSign = ((uint32_t)lSeed & 1u) ? 1 : -1;

    lSeed = (uint64_t)(uint32_t)lSeed * 0x6AC690C5u + (uint32_t)(lSeed >> 32);
    pMissile->pSeed.nHighSeed = (uint32_t)(lSeed >> 32);
    pMissile->pSeed.nLowSeed = (uint32_t)lSeed;
    nTurnStep = (int32_t)((uint32_t)lSeed % 3u) + gnWaverBaseTurn;

    for (iPoint = 0; iPoint < nFrames; ++iPoint)
    {
        nDirection += nTurnSign * nTurnStep;
        nDir8 = D2COMMON_11053((uint8_t)(nDirection & 0x3F));
        nX += gnDir8StepX[nDir8];
        pPathPoints[iPoint].X = (uint16_t)nX;
        nY += gnDir8StepY[nDir8];
        pPathPoints[iPoint].Y = (uint16_t)nY;

        if (iPoint % gnWaverFlipFrames == 0)
        {
            nTurnSign = -nTurnSign;

            nTurnStep = SEED_RollLimitedRandomNumber(&pMissile->pSeed, 3) + gnWaverBaseTurn;
        }
    }

    D2COMMON_PATH_SetNumberOfPathPoints(pPath, nFrames);
}


//1.10f: D2Client.0x6FAC08E0
//  `cltdofunc` 12: Tiger Strike releases a charge (all three of its `cltprgfunc1..3`). `retn 4`, nSkillLevel unread. It answers 0 without the
//  skill's record, and without a path target (the NULL itself, 0x6FAC0922); otherwise it puts the row's `prgoverlay` on the TARGET (GFX_AddOverlay,
//  kind 2, 0x6FAC09CC), plays its `prgsound` from the CASTER (0x6FAC09DB), and answers 1.
//  The charges pick both. The ladder is SKILLS_GetClientMissileForSkill's, written out here: the flag PROGRESSIVE (`test [ecx+8], al`, 0x6FAC0931),
//  `aurastate` and `aurastat1` in range -- 16-bit `jl` for the lower bounds, `movsx` and `jge` for the upper -- and a stat list on the caster for
//  that state. Then the stat's value held to 1..3, less one, is added to both words, each sign-extended; a test that fails takes the words as they
//  are (0x6FAC09B0), as one charge would. Stock Overlay.txt and Sounds.txt hold the three in consecutive rows -- `tigerstrike1..3`,
//  `assassin_chargeup_tiger_1..3` -- which is how one function serves three slots. The tables pointer is loaded again for the states count
//  (0x6FAC0942). The original inlines DATATBLS_GetSkillsTxtRecordById.
int32_t __fastcall SKILLS_CltDoFunc12(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pTarget;
    D2StatListStrc *pStatList;
    int32_t nState;
    int32_t nCharges;
    int32_t nOverlayId;
    int32_t nSoundId;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget)
        return 0;

    if (((uint8_t)pSkillsTxt->dwFlags[0] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_PROGRESSIVE]) && pSkillsTxt->nAuraState >= 0 && (nState = pSkillsTxt->nAuraState) < sgptDataTables->nStatesTxtRecordCount && pSkillsTxt->wAuraStat[0] >= 0
        && pSkillsTxt->wAuraStat[0] < sgptDataTables->nItemStatCostTxtRecordCount && (pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, (D2C_States)nState)) != NULL)
    {
        nCharges = D2COMMON_STATLIST_GetStatValue(pStatList, (D2C_ItemStats)pSkillsTxt->wAuraStat[0], 0);
        if (nCharges <= 1)
            nCharges = 1;
        else if (nCharges >= 3)
            nCharges = 3;

        nOverlayId = (int16_t)pSkillsTxt->wPrgOverlay + (nCharges - 1);
        nSoundId = (int16_t)pSkillsTxt->wPrgSound + (nCharges - 1);
    }
    else
    {
        nOverlayId = (int16_t)pSkillsTxt->wPrgOverlay;
        nSoundId = (int16_t)pSkillsTxt->wPrgSound;
    }

    GFX_AddOverlay(pTarget, (D2_OverlayIds)nOverlayId, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);
    SOUNDINST_PlaySound((D2C_Sounds)nSoundId, pUnit, 0, 0, 0);
    return 1;
}


//1.10f: D2Client.0x6FAC09F0
//  `cltdofunc` 13: Cobra Strike releases a charge (all three of its slots). `retn 4`, nSkillLevel unread. `cltdofunc` 12 without the target: 0
//  without the skill's record (0x6FAC0A1C), otherwise the overlay goes on the CASTER (0x6FAC0ABE) and the sound plays from it, and the answer is 1
//  -- with a path target or without one. The same ladder picks `cobrastrike1..3` and `assassin_chargeup_cobra_1..3`; here its two counts are read
//  off the tables pointer the lookup left in ecx (0x6FAC0A41, 0x6FAC0A52). The original inlines DATATBLS_GetSkillsTxtRecordById.
int32_t __fastcall SKILLS_CltDoFunc13(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2StatListStrc *pStatList;
    int32_t nState;
    int32_t nCharges;
    int32_t nOverlayId;
    int32_t nSoundId;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    if (((uint8_t)pSkillsTxt->dwFlags[0] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_PROGRESSIVE]) && pSkillsTxt->nAuraState >= 0 && (nState = pSkillsTxt->nAuraState) < sgptDataTables->nStatesTxtRecordCount && pSkillsTxt->wAuraStat[0] >= 0
        && pSkillsTxt->wAuraStat[0] < sgptDataTables->nItemStatCostTxtRecordCount && (pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, (D2C_States)nState)) != NULL)
    {
        nCharges = D2COMMON_STATLIST_GetStatValue(pStatList, (D2C_ItemStats)pSkillsTxt->wAuraStat[0], 0);
        if (nCharges <= 1)
            nCharges = 1;
        else if (nCharges >= 3)
            nCharges = 3;

        nOverlayId = (int16_t)pSkillsTxt->wPrgOverlay + (nCharges - 1);
        nSoundId = (int16_t)pSkillsTxt->wPrgSound + (nCharges - 1);
    }
    else
    {
        nOverlayId = (int16_t)pSkillsTxt->wPrgOverlay;
        nSoundId = (int16_t)pSkillsTxt->wPrgSound;
    }

    GFX_AddOverlay(pUnit, (D2_OverlayIds)nOverlayId, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);
    SOUNDINST_PlaySound((D2C_Sounds)nSoundId, pUnit, 0, 0, 0);
    return 1;
}


//1.10f: D2Client.0x6FAC0AE0
//  `cltdofunc` 14: Royal Strike releases one charge (its `cltprgfunc1`). `retn 4`. It answers 0 without the skill's record or a missile (below 0,
//  `jge` at 0x6FAC0B2A). Otherwise it calls SKILLS_GetPathTargetPos for the answer alone -- the point it writes is never read -- and
//  SKILLS_MissileClientSpawnOnTargetUnit starts the missile -- `cltmissilea`, `royalstrikemeteorcenter`, for one charge -- at the point the unit is
//  heading for, within 100 subtiles; that answer is not read, and this one is 1. The record is looked up only to be tested.
//  SKILLS_CltDoFunc7 makes the same spawn behind other tests; this one asks for no target unit and sets no flag. The original inlines
//  DATATBLS_GetSkillsTxtRecordById.
int32_t __fastcall SKILLS_CltDoFunc14(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    int32_t nMissileId;
    int32_t nTargetX;
    int32_t nTargetY;

    if (!DATATBLS_GetSkillsTxtRecordById(nSkillId))
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0)
        return 0;

    //  BUG: the refusal after SKILLS_GetPathTargetPos (0x6FAC0B49) tests an answer that is always 1. UNREACHABLE:
    //  SKILLS_GetPathTargetPos answers 1 on every path.
    if (!SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY))
        return 0;

    SKILLS_MissileClientSpawnOnTargetUnit(pUnit, (D2C_MissilesHcIdx)nMissileId, nSkillId, nSkillLevel);
    return 1;
}


//1.10f: D2Client.0x6FAC0B80
//  `cltdofunc` 15: Royal Strike releases three charges (its `cltprgfunc3`). `retn 4`. It answers 0 without the skill's record or a missile (below
//  0, `jge` at 0x6FAC0BCC); then comes SKILLS_GetPathTargetPos' point, and SKILLS_EvaluateProgressiveSkillCalc's count -- `prgcalc3`, `par5`, 16 in
//  stock -- refused on exactly 0 (`jnz`, 0x6FAC0C0B); a count below 0 answers 1 with nothing spawned (`jle`, 0x6FAC0C72). Otherwise a burst of the
//  missile -- `cltmissilec`, `royalstrikechaosice` -- goes out from the point:
//    * a seed of its own is started from the point's X (0x6FAC0C1F..0x6FAC0C2B), so every client throws the same burst at the same spot, and one
//      block is filled: flags 3, the point as the missile's position, the owner, the missile, the skill and its level;
//    * for each missile two rolls of that seed, each taken modulo 40 (`div`, unsigned) less 20, give the block's nTargetX and nTargetY, -20..19
//      each, and the pair (0, 0) is made (20, 0) (0x6FAC0CD7);
//    * a missile that spawns gets the seed's low half as its target X and the two offsets packed as its target Y, Y in the high 16 bits and X in
//      the low (0x6FAC0CFA..0x6FAC0D10). D2Common's MISSILE_SetTargetX and SetTargetY store them at +28h and +2Ch of the missile's data
//      (0x6FDBA4A8, 0x6FDBA4E8): two dwords a client missile carries for whatever its spawner puts there.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, and SEED_RollRandomNumber twice per missile (written out here; the first stores neither
//  half).
int32_t __fastcall SKILLS_CltDoFunc15(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2UnitStrc *pMissile;
    D2SeedStrc tSeed;
    D2MissileStrc tParams;
    uint64_t lSeed;
    int32_t nMissileId;
    int32_t nMissileCount;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nOffsetX;

    if (!DATATBLS_GetSkillsTxtRecordById(nSkillId))
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0)
        return 0;

    //  BUG: the refusal after SKILLS_GetPathTargetPos (0x6FAC0BEB) tests an answer that is always 1. UNREACHABLE:
    //  SKILLS_GetPathTargetPos answers 1 on every path.
    if (!SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY))
        return 0;

    nMissileCount = SKILLS_EvaluateProgressiveSkillCalc(pUnit, nSkillId, nSkillLevel);
    if (!nMissileCount)
        return 0;

    D2COMMON_SEED_InitLowSeed(&tSeed, nTargetX);
    memset(&tParams, 0, sizeof(tParams));
    tParams.nX = nTargetX;
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET;
    tParams.nY = nTargetY;
    tParams.pOwner = pUnit;
    tParams.nMissile = nMissileId;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;

    if (nMissileCount > 0)
    {
        do
        {
            lSeed = (uint64_t)tSeed.nLowSeed * 0x6AC690C5u + tSeed.nHighSeed;
            nOffsetX = (int32_t)((uint32_t)lSeed % 40u) - 20;

            lSeed = (uint64_t)(uint32_t)lSeed * 0x6AC690C5u + (uint32_t)(lSeed >> 32);
            tSeed.nHighSeed = (uint32_t)(lSeed >> 32);
            tSeed.nLowSeed = (uint32_t)lSeed;

            tParams.nTargetX = nOffsetX;
            tParams.nTargetY = (int32_t)((uint32_t)lSeed % 40u) - 20;
            if (!tParams.nTargetX && !tParams.nTargetY)
                tParams.nTargetX = 20;

            pMissile = MISSILE_ClientSpawn(&tParams);
            if (pMissile)
            {
                D2COMMON_MISSILE_SetTargetX(pMissile, (int32_t)D2COMMON_SEED_GetLowSeed(&tSeed));
                D2COMMON_MISSILE_SetTargetY(pMissile, (int32_t)((((uint32_t)tParams.nTargetY & 0xFFFFu) << 16) + ((uint32_t)tParams.nTargetX & 0xFFFFu)));
            }
        } while (--nMissileCount);
    }

    return 1;
}

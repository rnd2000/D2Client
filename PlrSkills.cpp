#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "PlrSkills.h"

//  PlrSkills.cpp -- from D2Client.dll 1.10, the original's UNIT\PlrSkills.cpp (named by its asserts), address range 0x6FB4B980..0x6FB4DAB1.
//  The effect a unit's used skill draws around it in sequence mode, the end of a state sent by the server, a state's stat written into its own
//  stat list, the two dispatchers that run a state's set and remove functions, and the client's answer to "can this skill be used".
//  The STATES_ functions that lie between them in the address range are written in States.cpp.

//  The path literal this unit passes as __FILE__ to its asserts.
static const char szPlrSkillsCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UNIT\\PlrSkills.cpp";

//  The members and numbers this file reads that no other file pins.
static_assert(offsetof(D2StatesTxt, wRemoveOverlay) == 0x0C && offsetof(D2StatesTxt, wOffSound) == 0x28,
              "PLRSKILLS_EndStateAndSkill: `mov ax, [ebp+0Ch]` (0x6FB4D6E6), `mov ax, [ebp+28h]` (0x6FB4D6B6)");
static_assert(STAT_ALIGNMENT == 0xAC && UNIT_ALIGNMENT_UNASSIGNED == 4 && ITEMSTATCOSTFLAGINDEX_UPDATEANIMRATE == 9,
              "PLRSKILLS_SetStateStat: `cmp edi, 0ACh` (0x6FB4D851), `mov dword ptr [esp+14h+nOldAlign], 4` "
              "(0x6FB4D859), byte +5 against gdwBitMasks[1] (0x6FB4D8F8..0x6FB4D900)");


//1.10f: D2Client.0x6FB4B980
//  Draws the used skill's effect before the unit. With a used skill and the unit in PLRMODE_SEQUENCE (a player) or MONMODE_SEQUENCE (a monster),
//  the skill's srvdofunc picks the effect: 67 (Charge, SerpentCharge) draws SKILLS_DrawMotionTrail when bit 0 of the skill's flags is set, 76
//  (Whirlwind) draws SKILLS_DrawSpinCopy, both with 1 as the last argument; anything else draws nothing, and any other unit type returns.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
//  The type and mode are read null-safe (pUnit ? type : 6, pUnit ? mode : 0), each mode test written into both arms, the shape of a pair of inlined
//  accessors. The NULL arms are dead: D2Common's UNITS_GetUsedSkill asserts the unit and exits on NULL before they can be reached.
//  Its twin PLRSKILLS_SkillsDrawUnitEffectInFront differs in the assert line (38 against 79) and in the last argument of both draws (1 against 0).
void __fastcall PLRSKILLS_SkillsDrawUnitEffectBehind(D2UnitStrc *pUnit, int32_t nDrawParam)
{
    D2SkillStrc *pUsedSkill;
    int32_t nUnitType;
    BOOL bInSequence;
    D2SkillsTxt *pSkillsTxt;
    int32_t nSrvDoFunc;

    pUsedSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pUsedSkill)
        return;

    nUnitType = pUnit ? (int32_t)pUnit->dwUnitType : UNIT_TYPES_COUNT;
    if (nUnitType == UNIT_PLAYER)
        bInSequence = (pUnit ? pUnit->dwAnimMode : 0) == PLRMODE_SEQUENCE;
    else if (nUnitType == UNIT_MONSTER)
        bInSequence = (pUnit ? pUnit->dwAnimMode : 0) == MONMODE_SEQUENCE;
    else
        return;

    if (!bInSequence)
        return;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(D2COMMON_SKILLS_GetSkillIdFromSkill(pUsedSkill, szPlrSkillsCpp, 38));
    if (!pSkillsTxt)
        return;

    nSrvDoFunc = (int16_t)pSkillsTxt->wSrvDoFunc;
    if (nSrvDoFunc == 67)
    {
        if (D2COMMON_SKILLS_GetFlags(pUsedSkill) & 1)
            SKILLS_DrawMotionTrail(pUnit, nDrawParam, 1);
    }
    else if (nSrvDoFunc == 76)
    {
        SKILLS_DrawSpinCopy(pUnit, nDrawParam, 1);
    }
}


//1.10f: D2Client.0x6FB4BA60
//  Draws the same effect after the unit: PLRSKILLS_SkillsDrawUnitEffectBehind with assert line 79 and 0 as both draws' last argument.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
void __fastcall PLRSKILLS_SkillsDrawUnitEffectInFront(D2UnitStrc *pUnit, int32_t nDrawParam)
{
    D2SkillStrc *pUsedSkill;
    int32_t nUnitType;
    BOOL bInSequence;
    D2SkillsTxt *pSkillsTxt;
    int32_t nSrvDoFunc;

    pUsedSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pUsedSkill)
        return;

    nUnitType = pUnit ? (int32_t)pUnit->dwUnitType : UNIT_TYPES_COUNT;
    if (nUnitType == UNIT_PLAYER)
        bInSequence = (pUnit ? pUnit->dwAnimMode : 0) == PLRMODE_SEQUENCE;
    else if (nUnitType == UNIT_MONSTER)
        bInSequence = (pUnit ? pUnit->dwAnimMode : 0) == MONMODE_SEQUENCE;
    else
        return;

    if (!bInSequence)
        return;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(D2COMMON_SKILLS_GetSkillIdFromSkill(pUsedSkill, szPlrSkillsCpp, 79));
    if (!pSkillsTxt)
        return;

    nSrvDoFunc = (int16_t)pSkillsTxt->wSrvDoFunc;
    if (nSrvDoFunc == 67)
    {
        if (D2COMMON_SKILLS_GetFlags(pUsedSkill) & 1)
            SKILLS_DrawMotionTrail(pUnit, nDrawParam, 0);
    }
    else if (nSrvDoFunc == 76)
    {
        SKILLS_DrawSpinCopy(pUnit, nDrawParam, 0);
    }
}


//1.10f: D2Client.0x6FB4D640
//  Handles a state turned off by the server. If the unit has the state and its stat list is the state's own, the list is expired and freed and the
//  row's offsound, when not negative, is played. Then always: the state is turned off; the colour is recomputed for a row with a colorshift
//  (STATES_RefreshUnitColorShift); the row's removeoverlay, when not negative, is moved to a random row of its group by the unit's seed and added as
//  OVERLAYTYPE_LOOPEX with -1 and -1; overlay1 and overlay2 are each taken off with their whole group; a transform row's graphics are restarted as
//  STATES_ToggleStateWithSound restarts them; and last, for every state, D2Common's UNITS_UpdateAnimRateAndVelocity is called.
//  The used skill is dropped only for a transform row, not with every state.
//  The original inlines DATATBLS_GetStatesTxtRecord, SEED_RollLimitedRandomNumber and the overlay-group removal here.
//  The compare with -1 after the `< 0` test of each overlay column tests an id that cannot be -1; it is an inlined helper's own test, kept as the
//  original has it.
void __fastcall PLRSKILLS_EndStateAndSkill(D2UnitStrc *pUnit, D2C_States nState)
{
    D2StatesTxt *pStatesTxt;
    D2StatListStrc *pStatList;
    int32_t nOverlayId;
    int32_t nCount;
    int32_t i;

    if (!pUnit)
        return;

    if ((int32_t)nState < 0 || (int32_t)nState >= sgptDataTables->nStatesTxtRecordCount)
        return;

    pStatesTxt = &sgptDataTables->pStatesTxt[nState];
    if (!pStatesTxt)
        return;

    if (D2COMMON_STATES_CheckState(pUnit, nState))
    {
        pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, nState);
        if (pStatList && (int32_t)nState == D2COMMON_STATLIST_GetState(pStatList))
        {
            D2COMMON_STATLIST_ExpireStatList(pUnit, pStatList);
            D2COMMON_STATLIST_FreeStatList(pStatList);
        }

        if ((int16_t)pStatesTxt->wOffSound >= 0)
            SOUNDINST_PlaySound((D2C_Sounds)(int16_t)pStatesTxt->wOffSound, pUnit, 0, 0, 0);
    }

    D2COMMON_STATES_ToggleState(pUnit, nState, FALSE);

    if (pStatesTxt->nColorShift)
        STATES_RefreshUnitColorShift(pUnit);

    if ((int16_t)pStatesTxt->wRemoveOverlay >= 0)
    {
        nOverlayId = (int16_t)pStatesTxt->wRemoveOverlay;
        if (nOverlayId != -1)
        {
            nOverlayId += SEED_RollLimitedRandomNumber(&pUnit->pSeed, (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlayId));
        }

        GFX_AddOverlay(pUnit, (D2_OverlayIds)nOverlayId, OVERLAYTYPE_LOOPEX, 0, -1, -1, 0, 0);
    }

    if ((int16_t)pStatesTxt->wOverlay[0] >= 0)
    {
        nOverlayId = (int16_t)pStatesTxt->wOverlay[0];
        if (nOverlayId != -1)
        {
            nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlayId);
            for (i = 0; i < nCount; ++i)
                GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(i + nOverlayId));
        }
    }

    if ((int16_t)pStatesTxt->wOverlay[1] >= 0)
    {
        nOverlayId = (int16_t)pStatesTxt->wOverlay[1];
        if (nOverlayId != -1)
        {
            nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlayId);
            for (i = 0; i < nCount; ++i)
                GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(i + nOverlayId));
        }
    }

    if (pStatesTxt->nStateFlags[0] & (uint8_t)gdwBitMasks[STATEMASK_TRANSFORM & 7])
    {
        GFX_ResetUnitGfx(pUnit);
        if (pUnit->dwUnitType == UNIT_PLAYER || pUnit->dwUnitType == UNIT_MONSTER)
            UNITMODE_SetAnimMode(pUnit, 1);
        GFX_RebuildUnitAnimForMode(pUnit, 1);
        D2COMMON_UNITS_SetAnimStartFrame(pUnit);
        pUnit->nSeqCurrentFramePrecise = 0;
        D2COMMON_UNITS_SetUsedSkill(pUnit, NULL);
    }

    D2COMMON_UNITS_UpdateAnimRateAndVelocity(pUnit, szPlrSkillsCpp, 1432);
}


//1.10f: D2Client.0x6FB4D830
//  Writes one stat of a state's own stat list, making the list when there is none; returns the list, which callers hand back for the
//  state's next stat.
//  No unit: NULL. Given no list, the unit's list for the state is fetched (and the stat's old value in it read, for any stat) or, when there is none,
//  allocated for the unit's type and id with no memory pool, given the state and posted to the unit. The value is then set. STAT_ALIGNMENT goes to
//  CUNIT_OnUnitAlignmentChanged with the old value, and any stat whose ItemStatCost row has UPDATEANIMRATE (bit 9) recomputes the unit's anim rate
//  and velocity. The original inlines the itemstatcost.txt record lookup here.
//  BUG: the old alignment is UNIT_ALIGNMENT_UNASSIGNED (4) unless the list was fetched here: a caller that passes the list it already has, or a
//  list made here, reports 4 as the old value, and a GOOD monster reported leaving GOOD from 4 keeps the room's ally count raised.
//  LATENT: the server writes STATE_ALIGNMENT's list with that one stat, so it is the first of its state and its list is fetched.
//  The old value is a dword slot passed as a byte; the 172 arm jumps past the lookup's `>= 0` test, which 172 cannot fail.
D2StatListStrc *__fastcall PLRSKILLS_SetStateStat(D2UnitStrc *pUnit, D2StatListStrc *pStatList, D2C_States nState, D2C_ItemStats nStatId, int32_t nValue, uint16_t nLayer)
{
    int32_t nOldAlign;
    D2ItemStatCostTxt *pItemStatCostTxt;

    if (!pUnit)
        return NULL;

    nOldAlign = 0;
    if (nStatId == STAT_ALIGNMENT)
        nOldAlign = UNIT_ALIGNMENT_UNASSIGNED;

    if (!pStatList)
    {
        pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, nState);
        if (pStatList)
        {
            nOldAlign = D2COMMON_STATLIST_GetStatValue(pStatList, nStatId, nLayer);
        }
        else
        {
            pStatList = D2COMMON_STATLIST_AllocStatList(NULL, 0, 0, pUnit->dwUnitType, pUnit->dwUnitId);
            D2COMMON_STATLIST_SetState(pStatList, nState);
            D2COMMON_10475_PostStatToStatList(pUnit, pStatList, TRUE);
        }
    }

    D2COMMON_STATLIST_SetStatIfListIsValid(pStatList, nStatId, nValue, nLayer);

    if (nStatId == STAT_ALIGNMENT)
        CUNIT_OnUnitAlignmentChanged(pUnit, (uint8_t)nOldAlign, (uint8_t)nValue);

    if (nStatId < 0 || nStatId >= sgptDataTables->nItemStatCostTxtRecordCount)
        return pStatList;

    pItemStatCostTxt = &sgptDataTables->pItemStatCostTxt[nStatId];
    if (pItemStatCostTxt && ((uint8_t)(pItemStatCostTxt->dwItemStatFlags >> 8) & (uint8_t)gdwBitMasks[ITEMSTATCOSTFLAGINDEX_UPDATEANIMRATE & 7]))
    {
        D2COMMON_UNITS_UpdateAnimRateAndVelocity(pUnit, szPlrSkillsCpp, 1486);
    }

    return pStatList;
}


//1.10f: D2Client.0x6FB4D920
//  Runs a state's setfunc and adds its client event. With a unit and its row, and unless the row's notondead finds the unit in its dead mode (the
//  same two tests as STATES_ToggleStateWithSound, but here they return), the row's setfunc, from 0 below 31 (signed), picks a slot of
//  gpStateSetFuncs, called as f(pUnit, nStateId) when not NULL. Then for a row whose cltevent is not negative and a unit with the state's stat
//  list, SKILLS_StatesAddCltEventFunc registers the event with the skill and level of that list (MODIFIERLIST_SKILL and _LEVEL, stats 350 and 351),
//  the row's clteventfunc, kind 1 and the state as the key. The original inlines DATATBLS_GetStatesTxtRecord here.
void __fastcall PLRSKILLS_StatesRunStateSetFunc(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2StatesTxt *pStatesTxt;
    int32_t nSetFunc;
    void (__fastcall *pfnSetFunc)(D2UnitStrc *, D2C_States);
    D2StatListStrc *pStatList;
    int32_t nSkillId;
    int32_t nSkillLevel;

    if (!pUnit)
        return;

    if ((int32_t)nStateId < 0 || (int32_t)nStateId >= sgptDataTables->nStatesTxtRecordCount)
        return;

    pStatesTxt = &sgptDataTables->pStatesTxt[nStateId];
    if (!pStatesTxt)
        return;

    if ((uint8_t)pStatesTxt->dwStateFlagsEx & (uint8_t)gdwBitMasks[STATEMASK_NOTONDEAD & 7])
    {
        if (pUnit->dwUnitType == UNIT_MONSTER && pUnit->dwAnimMode == MONMODE_DEAD)
            return;
        if (pUnit->dwUnitType == UNIT_PLAYER && pUnit->dwAnimMode == PLRMODE_DEAD)
            return;
    }

    nSetFunc = (int16_t)pStatesTxt->wSetFunc;
    if (nSetFunc >= 0 && nSetFunc < 0x1F)
    {
        pfnSetFunc = gpStateSetFuncs[nSetFunc];
        if (pfnSetFunc)
            pfnSetFunc(pUnit, nStateId);
    }

    if ((int16_t)pStatesTxt->wCltEvent < 0)
        return;

    pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, nStateId);
    if (!pStatList)
        return;

    nSkillId = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_MODIFIERLIST_SKILL, 0);
    nSkillLevel = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_MODIFIERLIST_LEVEL, 0);
    SKILLS_StatesAddCltEventFunc(pUnit, (int16_t)pStatesTxt->wCltEvent, (D2C_Skills)nSkillId, nSkillLevel, (int16_t)pStatesTxt->wCltEventFunc, 1, nStateId);
}


//1.10f: D2Client.0x6FB4DA00
//  Runs a state's remfunc and drops its client events. The mirror of PLRSKILLS_StatesRunStateSetFunc with no notondead test: the row's remfunc,
//  from 0 below 30 (signed), picks a slot of gpStateRemFuncs, called as f(pUnit, nStateId) when not NULL; then for a row whose cltevent is not
//  negative and a unit that still has the state's stat list, CUNITEVENT_RemoveEventsByKey is called with kind 1 and the state as the key.
//  The original inlines DATATBLS_GetStatesTxtRecord here.
void __fastcall PLRSKILLS_StatesRunStateRemFunc(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2StatesTxt *pStatesTxt;
    int32_t nRemFunc;
    void (__fastcall *pfnRemFunc)(D2UnitStrc *, D2C_States);

    if (!pUnit)
        return;

    if ((int32_t)nStateId < 0 || (int32_t)nStateId >= sgptDataTables->nStatesTxtRecordCount)
        return;

    pStatesTxt = &sgptDataTables->pStatesTxt[nStateId];
    if (!pStatesTxt)
        return;

    nRemFunc = (int16_t)pStatesTxt->wRemFunc;
    if (nRemFunc >= 0 && nRemFunc < 0x1E)
    {
        pfnRemFunc = gpStateRemFuncs[nRemFunc];
        if (pfnRemFunc)
            pfnRemFunc(pUnit, nStateId);
    }

    if ((int16_t)pStatesTxt->wCltEvent < 0)
        return;

    if (D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, nStateId))
        CUNITEVENT_RemoveEventsByKey(pUnit, 1, nStateId);
}


//1.10f: D2Client.0x6FB4DA80
//  The client's use state of a skill: D2Common's SKILLS_GetUseState, and for a skill it calls usable on the local player, SKILLUSTATE_BLOCKED while
//  the frame count is below the frame the client blocks skills until (an unsigned compare; the block frame is read first, then the frame count).
//  D2Common's own BLOCKED is the skill's delay; this is the client's frame block, set by Skills.cpp and cleared by
//  STATES_SetFunc19_RemFunc12_SkillDelay.
D2C_SkillUseStates __fastcall PLRSKILLS_SkillsGetUseStateForClient(D2UnitStrc *pUnit, D2SkillStrc *pSkill)
{
    int32_t nUseState;
    uint32_t nBlockUntilFrame;

    nUseState = D2COMMON_SKILLS_GetUseState(pUnit, pSkill);
    if (nUseState != SKILLUSTATE_USABLE)
        return (D2C_SkillUseStates)nUseState;

    if (pUnit == CUNIT_GetPlayer())
    {
        nBlockUntilFrame = (uint32_t)GAME_GetSkillBlockUntilFrame();
        if (GAME_GetFrameCount() < nBlockUntilFrame)
            return SKILLUSTATE_BLOCKED;
    }

    return SKILLUSTATE_USABLE;
}

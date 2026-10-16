#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "SkillsPal.h"

//  SkillsPal.cpp -- the paladin's client skill code of D2Client.dll 1.10: Sacrifice's blood, Blessed Hammer's and Fist of the Heavens' missiles,
//  the sanctuary aura's bolts, and Charge (its start, its do, its motion trail and its end) with the classless SerpentCharge row that borrows
//  Charge's slots.
//  The original source file is Skills\SkillsPal.cpp (SKILLS_CltStFunc25 passes its path). Address range 0x6FAC8EE0..0x6FAC9E7B.
//  The skills served: cltstfunc 25 Charge and SerpentCharge; cltdofunc 34 Sacrifice, 35 Blessed Hammer, 36 Fist of the Heavens, 37 Charge and
//  SerpentCharge; States.txt cltactivefunc 86 (the `sanctuary` state, the aura state of Sanctuary). SKILLS_CltDoFunc86 sits here between
//  SKILLS_CltDoFunc36 and SKILLS_DrawMotionTrail.

//  Blessed Hammer's path type is `push 0Eh` in the bytes (0x6FAC91BE) and a name here.
static_assert(PATHTYPE_BLESSEDHAMMER == 14,
              "SKILLS_CltDoFunc35 sets its missile's path type 14 (push 0Eh, 0x6FAC91BE)");


//1.10f: D2Client.0x6FAC8EE0
//  `cltdofunc` 34, the do of Sacrifice: its blood.
//    * The record, and `cltmissilea` (blood1), a signed word inside [0, missiles.txt's count), or the answer is 0. It is kept in a frame slot
//      (0x6FAC8F2F).
//    * UNITFLAG_SKSRVDOFUNC on a non-NULL unit.
//    * The path's target unit (CUNIT_GetPathTargetUnit) must exist, and the unit's dynamic path, or the answer is 0. The target is not read again:
//      the blood is drawn round the caster.
//    * A direction: with the path moving (D2Common's PATH_GetVelocity not 0), the unit's direction, a byte, shifted right 3 (and eax, 0FFh / shr
//      eax, 3); standing, a seed roll's low 3 bits. Its opposite, (nDirection + 4) % 8 signed (and ecx, 80000007h / jns), and a second roll's low
//      half unsigned mod 3 (div edi) pick the spot: (roll % 3 + opposite - 1) & 7, the opposite or a neighbour of it.
//    * The spot is the unit's client position plus gnCltEffect34OffsetX and Y at that index: the eight points (0, 2), (-1, 1), (-2, 0), (-1, -1),
//      (0, -2), (1, -1), (2, 0) and (1, 1) round the unit.
//    * `cltcalc1` (Sacrifice's 3) is evaluated, and a roll below it is added to the missile: blood1 (18), blood2 (19) or bigblood1 (20), stock
//      Missiles.txt. At 0 or below there is no roll and blood1 stays.
//    * MISSILE_ClientSpawnForOwner with the missile, the spot as its target, the skill and the level; its answer is not read. The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, SEED_RollRandomNumber, UNITS_GetXPosition, UNITS_GetYPosition and
//  SEED_RollLimitedRandomNumber here.
int32_t __fastcall SKILLS_CltDoFunc34(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    int32_t nDirection;
    int32_t nOppositeDir;
    int32_t nSpawnDir;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nMissileChoices;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: this NULL test guards only the flag; pUnit goes on untested into CUNIT_GetPathTargetUnit, which reads it (test esi, esi at 0x6FAC8F45).
    //  UNREACHABLE.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    if (!CUNIT_GetPathTargetUnit(pUnit))
        return 0;

    //  BUG: this refusal is dead: it comes after CUNIT_GetPathTargetUnit, which asserts the same path and exits on NULL (0x6FAC8F69..0x6FAC8F6B).
    //  UNREACHABLE.
    if (!pUnit->pDynamicPath)
        return 0;

    if (D2COMMON_PATH_GetVelocity(pUnit->pDynamicPath))
    {
        nDirection = D2COMMON_UNITS_GetDirection(pUnit) >> 3;
    }
    else
    {
        nDirection = (int32_t)((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) & 7);
    }

    nOppositeDir = (nDirection + 4) % 8;

    nSpawnDir = (int32_t)(((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 3 + nOppositeDir - 1) & 7);

    nTargetX = UNITS_GetXPosition(pUnit) + gnCltEffect34OffsetX[nSpawnDir];

    nTargetY = UNITS_GetYPosition(pUnit) + gnCltEffect34OffsetY[nSpawnDir];

    nMissileChoices = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[0], nSkillId, nSkillLevel);

    nMissileId += SEED_RollLimitedRandomNumber(&pUnit->pSeed, nMissileChoices);

    MISSILE_ClientSpawnForOwner(pUnit, nMissileId, nTargetX, nTargetY, nSkillId, nSkillLevel);
    return 1;
}


//1.10f: D2Client.0x6FAC90E0
//  `cltdofunc` 35, Blessed Hammer.
//    * The record is only tested; the missile comes from SKILLS_GetClientMissileForSkill (blessedhammer, the row's `cltmissilea`; the row is not
//      progressive), a signed value inside [0, missiles.txt's count), or the answer is 0.
//    * UNITFLAG_SKSRVDOFUNC on a non-NULL unit.
//    * SKILLS_GetPathTargetPos gives the point; its answer is tested (0x6FAC9167..0x6FAC9169).
//    * The block, cleared (rep stosd, 17h dwords), with flags 20h (the point is the target, where the missile goes, and it starts at the origin,
//      the caster, which is also its owner: MISSILE_ClientSpawn's test al, 20h), the skill, the level and the missile. No missile, or no dynamic
//      path on it, and the answer is 0.
//    * D2Common's PATH_SetType with PATHTYPE_BLESSEDHAMMER and PATH_StartUnitMove (bAllowInTown 0). The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
//  BUG: the client's do differs from the server's: its missile may be 0 where the server's must be above 0, it sets UNITFLAG_SKSRVDOFUNC before the
//  target point is found where the server sets it after, and it tests the missile's path before using it where the server does not. INERT.
int32_t __fastcall SKILLS_CltDoFunc35(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    int32_t nMissileId;
    int32_t nTargetX;
    int32_t nTargetY;
    D2UnitStrc *pMissile;
    D2DynamicPathStrc *pDynamicPath;
    D2MissileStrc tParams;

    if (!DATATBLS_GetSkillsTxtRecordById(nSkillId))
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: this NULL test guards only the flag; pUnit goes on untested into SKILLS_GetPathTargetPos and the block (test ebx, ebx at 0x6FAC9145).
    //  UNREACHABLE.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    //  BUG: this refusal is dead: SKILLS_GetPathTargetPos answers 1 on every path. UNREACHABLE.
    if (!SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY))
        return 0;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nTargetX = nTargetX;
    tParams.nTargetY = nTargetY;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = nMissileId;
    pMissile = MISSILE_ClientSpawn(&tParams);
    if (!pMissile)
        return 0;

    pDynamicPath = pMissile->pDynamicPath;
    if (!pDynamicPath)
        return 0;

    D2COMMON_PATH_SetType(pDynamicPath, PATHTYPE_BLESSEDHAMMER);
    D2COMMON_PATH_StartUnitMove(pDynamicPath, pMissile, 0);
    return 1;
}


//1.10f: D2Client.0x6FAC91F0
//  `cltdofunc` 36, Fist of the Heavens.
//    * The record is only tested; the missile is SKILLS_GetClientMissileForSkill's (fistoftheheavensdelay, the row's `cltmissilea`), a signed value
//      inside [0, missiles.txt's count), or the answer is 0.
//    * UNITFLAG_SKSRVDOFUNC on a non-NULL unit.
//    * The path's target unit, or the answer is 0 (xor eax, eax before the test; the jump lands past the epilogue's own xor).
//    * The block, cleared, with flags 1 (the missile starts at nX and nY, the target's client position), the owner (no origin), the skill, the
//      level and the missile. No missile, and the answer is 0.
//    * The target's dwUnitType and dwUnitId go into the missile's two target slots (D2Common's MISSILE_SetTargetX and SetTargetY), a unit handle
//      carried in a coordinate pair. The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
//  BUG: the missile may be 0 here where the server's must be above 0 (jg at 0x6FD1AAAB). INERT.
//  BUG: the client sets UNITFLAG_SKSRVDOFUNC where the server sets none, and puts no overlay on the target where the server puts the row's
//  `srvoverlay`. NOT ESTABLISHED.
int32_t __fastcall SKILLS_CltDoFunc36(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    int32_t nMissileId;
    D2UnitStrc *pTarget;
    D2UnitStrc *pMissile;
    D2MissileStrc tParams;

    if (!DATATBLS_GetSkillsTxtRecordById(nSkillId))
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: this NULL test guards only the flag; pUnit goes on untested into CUNIT_GetPathTargetUnit and the block (test ebx, ebx at 0x6FAC9259).
    //  UNREACHABLE.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget)
        return 0;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS;
    tParams.pOwner = pUnit;

    tParams.nX = UNITS_GetXPosition(pTarget);

    tParams.nY = UNITS_GetYPosition(pTarget);

    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = nMissileId;
    pMissile = MISSILE_ClientSpawn(&tParams);
    if (!pMissile)
        return 0;

    D2COMMON_MISSILE_SetTargetX(pMissile, pTarget->dwUnitType);
    D2COMMON_MISSILE_SetTargetY(pMissile, pTarget->dwUnitId);
    return 1;
}


//1.10f: D2Client.0x6FAC9350
//  `cltactivefunc` 86 of States.txt: `sanctuary` is its one row, the aura state of Sanctuary. The table's three-argument __fastcall shape, run by
//  SKILLS_StatesRunActiveStateFuncs with the skill and level of the state's own stat list.
//    * A unit with no room, or in a town room, gets nothing: 0.
//    * The record, and the missile from SKILLS_GetClientMissileForSkill (sanctuarybolt, Sanctuary's `cltmissilea`; the row is not progressive), a
//      signed value inside [0, missiles.txt's count), or 0.
//    * `aurarangecalc` (Sanctuary's ln12) evaluated at the level and at level 1; either at 0 or below, and the answer is 0.
//    * A count: the range squared over the level-1 range squared (the ratio of the two areas) over 1..18, a seed roll's low half unsigned mod 18
//      plus 1 (div ecx); both divisions are signed (cdq / idiv).
//    * The block, cleared, with flags 2001h (1: the missile starts at nX and nY; 2000h: MISSILE_ClientSpawn turns the new missile to a random one
//      of 64 directions from the missile's own seed, test ah, 20h), the owner, the skill, the level and the missile.
//    * Count times (dec / jnz, behind a jle on the count): two rolls below 2 * range + 1, each less the range, an offset in the square round the
//      caster. One whose squared length is signed-below the range squared (cmp / jge) spawns a missile at the caster's client position plus the
//      offset plus 2 on each axis, and a missile made is turned to one of 64 directions from the caster's seed (D2Common's PATH_SetDirection). The
//      answer is 1.
//  The new missile's path goes to PATH_SetDirection untested, where SKILLS_CltDoFunc35 tests the same field; D2Common's PATH_SetDirection tests it
//  itself.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, SEED_RollRandomNumber, UNITS_GetXPosition, UNITS_GetYPosition and
//  SEED_RollLimitedRandomNumber here.
int32_t __fastcall SKILLS_CltDoFunc86(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2ActiveRoomStrc *pRoom;
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    int32_t nRange;
    int32_t nRangeAtLevel1;
    int32_t nRangeSpan;
    int32_t nRangeSquared;
    int32_t nRangeRatio;
    int32_t nCount;
    int32_t nCasterX;
    int32_t nCasterY;
    int32_t nOffsetX;
    int32_t nOffsetY;
    D2UnitStrc *pMissile;
    D2MissileStrc tParams;

    pRoom = D2COMMON_UNITS_GetRoom(pUnit);
    if (!pRoom)
        return 0;

    if (D2COMMON_DUNGEON_IsRoomInTown(pRoom))
        return 0;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    nRange = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwAuraRangeCalc, nSkillId, nSkillLevel);
    if (nRange <= 0)
        return 0;

    nRangeAtLevel1 = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwAuraRangeCalc, nSkillId, 1);
    if (nRangeAtLevel1 <= 0)
        return 0;

    //  taken unsigned (lea ebp, [esi+esi+1] and the two imuls) so that no range overflows in C; the bits are the bytes'
    nRangeSpan = (int32_t)(2u * (uint32_t)nRange + 1u);
    nRangeSquared = (int32_t)((uint32_t)nRange * (uint32_t)nRange);
    nRangeRatio = nRangeSquared / (int32_t)((uint32_t)nRangeAtLevel1 * (uint32_t)nRangeAtLevel1);

    nCount = nRangeRatio / (int32_t)((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 18 + 1);

    nCasterX = UNITS_GetXPosition(pUnit);

    nCasterY = UNITS_GetYPosition(pUnit);

    memset(&tParams, 0, sizeof(tParams));
    tParams.nMissile = nMissileId;
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_RANDOMDIR;
    tParams.pOwner = pUnit;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;

    if (nCount > 0)
    {
        do
        {
            nOffsetX = SEED_RollLimitedRandomNumber(&pUnit->pSeed, nRangeSpan) - nRange;

            nOffsetY = SEED_RollLimitedRandomNumber(&pUnit->pSeed, nRangeSpan) - nRange;

            if ((int32_t)((uint32_t)nOffsetY * (uint32_t)nOffsetY + (uint32_t)nOffsetX * (uint32_t)nOffsetX) < nRangeSquared)
            {
                tParams.nX = nCasterX + nOffsetX + 2;
                tParams.nY = nCasterY + nOffsetY + 2;
                pMissile = MISSILE_ClientSpawn(&tParams);
                if (pMissile)
                {
                    //  BUG: each missile's direction is set twice, by the spawn from the missile's own seed (flag 2000h) and again here from the
                    //  caster's; the first is overwritten before anything draws it. INERT: sanctuarybolt has NumDirections 1 and no `Vel`.
                    D2COMMON_PATH_SetDirection(pMissile->pDynamicPath, (uint8_t)((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) & 0x3F));
                }
            }
        } while (--nCount);
    }

    return 1;
}


//  Charge keeps its state in the skill's own runtime fields (D2SkillStrc through D2Common's SKILLS_Get/SetFlags and Get/SetParam1..4), not in
//  Skills.txt:
//    flags bit 0     charging: the start sets 1001h, the do sets 1 when the run loops and 0 when it ends
//    flags bit 1     told to end: SKILLS_EndUninterruptableSkill sets it on a charging skill
//    Param1, Param2  the target's dwUnitType and dwUnitId, or UNIT_TYPES_COUNT and 0 for none (the start)
//    Param3          the motion trail's ghosts: 0 at the start, one more per do up to 2
//    Param4          7, or 15 for Duriel (the start); nothing in this unit reads it
//  SKILLS_DrawMotionTrail is drawn for a unit whose used skill's `srvdofunc` is 67 (Charge and SerpentCharge) while flags bit 0 is set, after the
//  unit's mode, PLRMODE_SEQUENCE or MONMODE_SEQUENCE; `srvdofunc` 76 (Whirlwind) goes to SKILLS_DrawSpinCopy instead.
//  The path literal SKILLS_CltStFunc25 passes as __FILE__.
static const char szSkillsPalCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\Skills\\SkillsPal.cpp";
static_assert(sizeof(szSkillsPalCpp) == 64 + 1,
              "the path at 0x6FB779E8, 64 characters (`cstr_at.py`, scratch)");
static_assert(offsetof(D2AnimSeqTxt, nEvent) == 5 && ANIMSEQ_EVENT_NONE == 0,
              "SKILLS_CltDoFunc37: `mov al, [eax+5]` (0x6FAC9BA6), tested for zero");
static_assert(MONSTER_REANIMATEDHORDE1 == 436 && MONSTER_SIEGEBEAST1 == 441,
              "SKILLS_CltDoFunc37: `sub eax, 1B4h` (0x6FAC9D0F), then `sub eax, 5` (0x6FAC9D1A)");


//1.10f: D2Client.0x6FAC9620
//  Charge's motion trail: pUnit in ecx, nDrawParam in edx (handed on to GFX_DrawUnitCels as its second argument) and bBehindUnit on the stack.
//  Called twice for each draw of a charging unit, before its sprite with bBehindUnit 1 and after it with 0.
//    * No used skill, nothing. The unit's client coordinates (D2Common's UNITS_GetClientCoordX/Y) and its velocity (PATH_GetClientCoordsVelocity,
//      both outputs zeroed first).
//    * The pass that does not match the direction draws nothing: the behind pass only while Y velocity >= 0 (jl), the front pass only while it is <
//      0 (jge). Moving down-screen the trail lies above the unit and is painted before it, moving up-screen below and after it.
//    * The step: the velocity scaled to 15 over the octagonal length (min + 2 * max) / 2 of its two absolute values, signed (cdq / sub / sar), 1
//      for 0; each component 15 * v / length signed (idiv).
//    * The frame: nSeqCurrentFramePrecise >> 8, signed (sar), saved. Param3 of the skill ghosts (0 to 2), each the unit one frame further back
//      (dec, and 8 added when it goes below 0: jns), written into nSeqCurrentFramePrecise and drawn with GFX_DrawUnitCels (bGhostly 1, bDrawCel 0).
//    * With perspective on (D2Gfx's CheckPerspective), each ghost's point is the unit's precise position through SCALE_ScalePerspectivePosition at
//      96 (push 60h), plus the viewport origin; otherwise its client coordinates. Each pass paints from the ghost farthest from the viewer to the
//      nearest: the behind pass (the trail up-screen) from Param3 steps out inwards, the front pass (the trail down-screen) from one step out
//      outwards.
//    * The frame is put back as the saved frame << 8, also when Param3 is 0 or less.
void __fastcall SKILLS_DrawMotionTrail(D2UnitStrc *pUnit, int32_t nDrawParam, int32_t bBehindUnit)
{
    D2SkillStrc *pSkill;
    int32_t nUnitX;
    int32_t nUnitY;
    int32_t nVelX;
    int32_t nVelY;
    int32_t nAbsVelX;
    int32_t nAbsVelY;
    int32_t nLength;
    int32_t nStepX;
    int32_t nStepY;
    int32_t bPerspective;
    int32_t nSavedFrame;
    int32_t nGhostFrame;
    int32_t nGhostsLeft;
    int32_t nNearOffsetX;
    int32_t nNearOffsetY;
    int32_t nFarOffsetX;
    int32_t nFarOffsetY;
    int32_t nStepXNeg;
    int32_t nStepYNeg;
    int32_t nPrecisionX;
    int32_t nPrecisionY;
    int32_t nXAdjust;
    int32_t nYAdjust;
    int32_t nScreenX;
    int32_t nScreenY;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return;

    nUnitX = D2COMMON_UNITS_GetClientCoordX(pUnit);
    nUnitY = D2COMMON_UNITS_GetClientCoordY(pUnit);
    nVelX = 0;
    nVelY = 0;
    D2COMMON_PATH_GetClientCoordsVelocity(pUnit, &nVelX, &nVelY);

    if (bBehindUnit)
    {
        if (nVelY < 0)
            return;
    }
    else if (nVelY >= 0)
    {
        return;
    }

    nAbsVelX = nVelX < 0 ? -nVelX : nVelX;
    nAbsVelY = nVelY < 0 ? -nVelY : nVelY;
    if (nAbsVelX <= nAbsVelY)
        nLength = (nAbsVelX + 2 * nAbsVelY) / 2;
    else
        nLength = (nAbsVelY + 2 * nAbsVelX) / 2;
    if (!nLength)
        nLength = 1;

    nStepX = 15 * nVelX / nLength;
    nStepY = 15 * nVelY / nLength;

    bPerspective = D2GFX_CheckPerspective();
    nSavedFrame = pUnit->nSeqCurrentFramePrecise >> 8;
    nGhostFrame = nSavedFrame;

    nGhostsLeft = D2COMMON_SKILLS_GetParam3(pSkill);
    if (nGhostsLeft > 0)
    {
        nStepYNeg = -nStepY;
        nStepXNeg = -nStepX;
        nFarOffsetX = nGhostsLeft * nStepX;
        nNearOffsetY = nStepY;
        nNearOffsetX = nStepX;
        nFarOffsetY = nGhostsLeft * nStepY;
        do
        {
            nXAdjust = 0;
            nYAdjust = 0;

            //  BUG: the frames count back from the first ghost painted, so the front pass puts the frame one back nearest the unit and the behind
            //  pass puts it farthest from it. REACHABLE: cosmetic.
            if (--nGhostFrame < 0)
                nGhostFrame += 8;
            pUnit->nSeqCurrentFramePrecise = nGhostFrame << 8;

            if (bPerspective)
            {
                //  the Y accessor is called first, as in the bytes (0x6FAC975E, 0x6FAC9765)
                nPrecisionY = D2COMMON_UNITS_GetPrecisionY(pUnit);
                nPrecisionX = D2COMMON_UNITS_GetPrecisionX(pUnit);
                D2GFX_SCALE_ScalePerspectivePosition(nPrecisionX, nPrecisionY, 96, &nXAdjust, &nYAdjust, 0);
                nXAdjust += SCROLL_GetViewportOriginX();
                nScreenY = SCROLL_GetViewportOriginY() + nYAdjust;
                nScreenX = nXAdjust;
            }
            else
            {
                nScreenX = nUnitX;
                nScreenY = nUnitY;
            }

            //  The point goes into the two adjust slots before the call (0x6FAC97B6, 0x6FAC97BE), which the next pass zeroes.
            if (bBehindUnit)
            {
                nXAdjust = nScreenX - nFarOffsetX;
                nYAdjust = nScreenY - nFarOffsetY;
            }
            else
            {
                nXAdjust = nScreenX - nNearOffsetX;
                nYAdjust = nScreenY - nNearOffsetY;
            }
            GFX_DrawUnitCels(pUnit, nDrawParam, nXAdjust, nYAdjust, 1, 0);

            nNearOffsetX += nStepX;
            nNearOffsetY += nStepY;
            nFarOffsetX += nStepXNeg;
            nFarOffsetY += nStepYNeg;
        } while (--nGhostsLeft);
    }

    //  BUG: the frame is put back as (nSeqCurrentFramePrecise >> 8) << 8, so the fraction of a frame (the precise field's low 8 bits) is cleared by
    //  every call that passes the direction test. REACHABLE; its cost is NOT ESTABLISHED.
    pUnit->nSeqCurrentFramePrecise = nSavedFrame << 8;
}


//1.10f: D2Client.0x6FAC9820
//  `cltstfunc` 25, Charge's start (Charge and SerpentCharge). It is the unit's one passer of the path literal, at lines 374 and 458.
//    * No used skill, and the answer is 0 (the NULL itself). The record, or 0. A charge the local player already runs (the used skill is this one,
//      the unit is CUNIT_GetPlayer's, flags bit 0 set) is not started again: 0.
//    * No dynamic path, and the answer is 0 (the NULL path).
//    * The path's target unit. A player already in melee range of it does not charge: the target goes to SKILLS_DispatchUnitAction22, whose answer
//      is this function's (0x6FAC98EF..0x6FAC98FB).
//    * The sound: a player's `dosound a` (a signed word above 0: test si, si / jle) or a monster's MONSTER_GetSkillSound above 0; one
//      SOUNDINST_PlaySound for both.
//    * Param1 and Param2 the target's type and id, or UNIT_TYPES_COUNT and 0 when there is none or it is STATE_UNINTERRUPTABLE. Param4 7.
//    * The base velocity: 100h; a player's charstats.txt `RunVelocity` << 8 (mov ch, [eax+41h]); any other unit's monstats.txt `Run` << 8, signed
//      (movsx), with Param4 15 for Duriel (class 211, cmp eax, 0D3h) and a path distance of 30 (PATH_SetNewDistance), the last two only with a
//      record.
//    * The percent: STAT_VELOCITYPERCENT, at least 50 (cmp ecx, 32h / jg), plus the row's `Param1` (Charge's 150).
//    * PATH_SetVelocity with base * percent / 100, PATH_StartUnitMove (bAllowInTown 0), CUNIT_FaceUnitAtPathTarget snapped when there is a target,
//      flags 1001h, Param3 0, and SKILLS_PostSkillStatWithRemoveCallback, whose SKILLS_StatRemoveCallback_EndMoveSkill runs
//      SKILLS_EndUninterruptableSkill for this row. The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, DATATBLS_GetMonStatsTxtRecord, a charstats.txt lookup and DATATBLS_ApplyRatio here.
//  BUG: the start differs from the server's: a monster in melee range charges here, where the server puts it into an attack mode and only a player
//  is sent to SKILLS_DispatchUnitAction22; a monster's path gets distance 30 with no clear here and 20 after PATH_ClearPoint2 there; the server has
//  no refusal of a running charge, no sound, no Param3 and no facing, and it toggles STATE_SKILL_MOVE where the client posts its stat. NOT
//  ESTABLISHED.
int32_t __fastcall SKILLS_CltStFunc25(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;
    D2SkillsTxt *pSkillsTxt;
    D2SkillsTxt *pSkillsTxt2;
    D2DynamicPathStrc *pDynamicPath;
    D2UnitStrc *pTarget;
    D2CharStatsTxt *pCharStatsTxt;
    D2MonStatsTxt *pMonStatsTxt;
    D2C_Sounds nSoundId;
    int32_t nBaseVelocity;
    int32_t nVelocityPercent;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    if (D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillsPalCpp, 374) == nSkillId && pUnit == CUNIT_GetPlayer() && (D2COMMON_SKILLS_GetFlags(pSkill) & 1))
        return 0;

    pDynamicPath = pUnit->pDynamicPath;
    if (!pDynamicPath)
        return 0;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (pUnit->dwUnitType == UNIT_PLAYER && pTarget && D2COMMON_UNITS_IsInMeleeRange(pUnit, pTarget, 0))
        return SKILLS_DispatchUnitAction22(pUnit, pTarget);

    //  One call site in the bytes (0x6FAC9929..0x6FAC9931), the two arms merged into it.
    if (pUnit->dwUnitType == UNIT_PLAYER)
    {
        if ((int16_t)pSkillsTxt->wDoSoundA > 0)
            SOUNDINST_PlaySound((D2C_Sounds)(int16_t)pSkillsTxt->wDoSoundA, pUnit, 0, 0, 0);
    }
    else if (pUnit->dwUnitType == UNIT_MONSTER)
    {
        nSoundId = MONSTER_GetSkillSound(pUnit, nSkillId);
        if (nSoundId > 0)
            SOUNDINST_PlaySound(nSoundId, pUnit, 0, 0, 0);
    }

    if (!pTarget || D2COMMON_STATES_CheckState(pTarget, STATE_UNINTERRUPTABLE))
    {
        D2COMMON_SKILLS_SetParam1(pSkill, UNIT_TYPES_COUNT);
        D2COMMON_SKILLS_SetParam2(pSkill, 0);
    }
    else
    {
        D2COMMON_SKILLS_SetParam1(pSkill, pTarget->dwUnitType);
        D2COMMON_SKILLS_SetParam2(pSkill, pTarget->dwUnitId);
    }

    nBaseVelocity = 0x100;
    D2COMMON_SKILLS_SetParam4(pSkill, 7);

    if (pUnit->dwUnitType != UNIT_PLAYER)
    {
        pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);
        if (pMonStatsTxt)
        {
            nBaseVelocity = pMonStatsTxt->nRun << 8;
            if (pUnit->dwClassId == MONSTER_DURIEL)
                D2COMMON_SKILLS_SetParam4(pSkill, 15);

            D2COMMON_PATH_SetNewDistance(pDynamicPath, 30);
        }
    }
    else
    {
        if (pUnit->dwClassId >= 0 && pUnit->dwClassId < sgptDataTables->nCharStatsTxtRecordCount)
        {
            pCharStatsTxt = &sgptDataTables->pCharStatsTxt[pUnit->dwClassId];
            if (pCharStatsTxt)
                nBaseVelocity = pCharStatsTxt->nRunSpeed << 8;
        }
    }

    nVelocityPercent = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_VELOCITYPERCENT, 0);
    if (nVelocityPercent <= 50)
        nVelocityPercent = 50;

    pSkillsTxt2 = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    nVelocityPercent += pSkillsTxt2 ? pSkillsTxt2->dwParam[0] : 0;

    D2COMMON_PATH_SetVelocity(pDynamicPath, DATATBLS_ApplyRatio(nBaseVelocity, nVelocityPercent, 100), szSkillsPalCpp, 458);
    D2COMMON_PATH_StartUnitMove(pDynamicPath, pUnit, 0);
    if (pTarget)
        CUNIT_FaceUnitAtPathTarget(pUnit, 1);

    D2COMMON_SKILLS_SetFlags(pSkill, 0x1001);
    D2COMMON_SKILLS_SetParam3(pSkill, 0);
    SKILLS_PostSkillStatWithRemoveCallback(pUnit);
    return 1;
}


//1.10f: D2Client.0x6FAC9B50
//  `cltdofunc` 37, Charge's do, run each frame of the charge's sequence.
//    * No used skill, and the answer is 0. The skill's flags, read once.
//    * The frame count: 15, or D2Common's DATATBLS_GetSeqFrameCount with a sequence record (DATATBLS_GetMonSeqTxtRecordFromUnit).
//    * The last run frame: 7, or with a sequence record and a count above 0, a walk that looks for a record whose event is ANIMSEQ_EVENT_NONE and
//      takes the frame before it.
//    * A player or monster at action frame 3 (cmp byte ptr [esi+4Eh], 3) plays its attack sound (UNITSND_PlayAttackSound) and the answer is 0.
//    * The target from Param1 and Param2, none for UNIT_TYPES_COUNT, through CUNIT_FindUnitByGuidAndType.
//    * Not charging (flags bit 0 clear): flags, Param1 and Param2 0; the frame count left, (count - (dwSeqFrame >> 8)) << 8, into
//      dwFrameCountPrecise; with no target the nearest player or monster within 3 (SKILLS_FindNextNearbyTargetById, mask 3, after id -1), none and
//      the answer is 0; the unit's path faces the target's client position (D2Common's PATH_FaceCoords), and 1.
//    * Charging, the target in melee range: flags 0, the animation to the frame after the last run frame, dwFrameCountPrecise the frame count << 8;
//      1.
//    * Charging and told to end (flags bit 1): the nearest player or monster within 3 after the target's id; one in melee range becomes the target
//      (flags 0, Param1 and Param2 its type and id), 1. Otherwise flags 0, and unless the unit is a monster whose base id
//      (MONSTER_GetBaseMonsterId, bounded) is reanimatedhorde1 or siegebeast1, dwFrameCountPrecise 100h (one frame); 1.
//    * Still charging: past the last run frame (dwSeqFrame >> 8 signed, jl) the animation goes back to frame 0 with the whole count and flags 1;
//      then UNITFLAG_SKSRVDOFUNC is cleared, Param3 grows by one up to 2 (cmp eax, 2 / jge), the motion trail's ghosts, and the answer is 1.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition and MONSTER_GetBaseMonsterId here.
//  BUG: a monster told to end is in range at 3 on the server and at 0 here. NOT ESTABLISHED.
int32_t __fastcall SKILLS_CltDoFunc37(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;
    int32_t nSkillFlags;
    int32_t nFrameCount;
    D2AnimSeqTxt *pSeqRecord;
    int32_t nSeqFrames;
    int32_t nFrame;
    int32_t nLastRunFrame;
    int32_t nTargetType;
    uint32_t dwTargetId;
    D2UnitStrc *pTarget;
    D2UnitStrc *pNearby;
    int32_t nBaseId;
    int32_t nGhosts;
    int32_t nX;
    int32_t nY;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    nSkillFlags = D2COMMON_SKILLS_GetFlags(pSkill);

    nFrameCount = 15;
    if (D2COMMON_DATATBLS_GetMonSeqTxtRecordFromUnit(pUnit))
        nFrameCount = D2COMMON_DATATBLS_GetSeqFrameCount(pUnit);

    nLastRunFrame = 7;
    pSeqRecord = D2COMMON_DATATBLS_GetMonSeqTxtRecordFromUnit(pUnit);
    if (pSeqRecord)
    {
        nSeqFrames = D2COMMON_DATATBLS_GetSeqFrameCount(pUnit);
        for (nFrame = 0; nFrame < nSeqFrames; ++nFrame)
        {
            //  BUG: the walk does not walk. The event byte is loaded once, before the loop (mov al, [eax+5] at 0x6FAC9BA6), and the back edge (jl
            //  at 0x6FAC9BB0) returns to its test (0x6FAC9BA9), not to the load, so the record pointer never moves: the answer is -1 when the
            //  sequence's first record has no event and 7 otherwise. REACHABLE: for Duriel, the reanimated horde and the siege beasts (7 where the
            //  walk would give 15, 10 and 13); INERT for the player's Charge and the claw vipers'.
            if (pSeqRecord->nEvent == ANIMSEQ_EVENT_NONE)
            {
                nLastRunFrame = nFrame - 1;
                break;
            }
        }
    }

    //  BUG: pUnit is tested for NULL (0x6FAC9BBA, and again at 0x6FAC9CC1) after D2Common's UNITS_GetUsedSkill has asserted it and exited on NULL.
    //  UNREACHABLE.
    if (pUnit && (pUnit->dwUnitType == UNIT_PLAYER || pUnit->dwUnitType == UNIT_MONSTER) && pUnit->nActionFrame == 3)
    {
        UNITSND_PlayAttackSound(pUnit, pUnit->dwAnimMode, 0);
        return 0;
    }

    pTarget = NULL;
    nTargetType = D2COMMON_SKILLS_GetParam1(pSkill);
    dwTargetId = D2COMMON_SKILLS_GetParam2(pSkill);
    if (nTargetType != UNIT_TYPES_COUNT)
        pTarget = CUNIT_FindUnitByGuidAndType(dwTargetId, (D2C_UnitTypes)nTargetType);

    if (!(nSkillFlags & 1))
    {
        D2COMMON_SKILLS_SetFlags(pSkill, 0);
        D2COMMON_SKILLS_SetParam1(pSkill, 0);
        D2COMMON_SKILLS_SetParam2(pSkill, 0);
        pUnit->dwFrameCountPrecise = (nFrameCount - (pUnit->dwSeqFrame >> 8)) << 8;

        if (!pTarget)
        {
            pTarget = SKILLS_FindNextNearbyTargetById(pUnit, 0, 0, 3, (D2C_AuraFilters)(AURAFILTER_FINDPLAYER | AURAFILTER_FINDMONSTER), 0xFFFFFFFF, NULL);
            if (!pTarget)
                return 0;
        }

        nY = UNITS_GetYPosition(pTarget);

        nX = UNITS_GetXPosition(pTarget);

        D2COMMON_PATH_FaceCoords(pUnit->pDynamicPath, nX, nY, 0);
        return 1;
    }

    if (pTarget && D2COMMON_UNITS_IsInMeleeRange(pUnit, pTarget, 0))
    {
        D2COMMON_SKILLS_SetFlags(pSkill, 0);
        D2COMMON_UNITS_SetAnimationFrame(pUnit, nLastRunFrame + 1);
        pUnit->dwFrameCountPrecise = nFrameCount << 8;
        return 1;
    }

    if (nSkillFlags & 2)
    {
        pNearby = SKILLS_FindNextNearbyTargetById(pUnit, 0, 0, 3, (D2C_AuraFilters)(AURAFILTER_FINDPLAYER | AURAFILTER_FINDMONSTER), dwTargetId, NULL);
        if (pNearby && D2COMMON_UNITS_IsInMeleeRange(pUnit, pNearby, 0))
        {
            D2COMMON_SKILLS_SetFlags(pSkill, 0);
            D2COMMON_SKILLS_SetParam1(pSkill, pNearby->dwUnitType);
            D2COMMON_SKILLS_SetParam2(pSkill, pNearby->dwUnitId);
            return 1;
        }

        D2COMMON_SKILLS_SetFlags(pSkill, 0);

        nBaseId = MONSTER_GetBaseMonsterId(pUnit);
        if (nBaseId < 0 || nBaseId >= sgptDataTables->nMonStatsTxtRecordCount)
            nBaseId = -1;

        //  BUG: the NULL arm of the second pUnit test (0x6FAC9CC1) would go on to this store into dwFrameCountPrecise (0x6FAC9D23). UNREACHABLE.
        if (nBaseId != MONSTER_REANIMATEDHORDE1 && nBaseId != MONSTER_SIEGEBEAST1)
            pUnit->dwFrameCountPrecise = 0x100;

        return 1;
    }

    if ((pUnit->dwSeqFrame >> 8) >= nLastRunFrame)
    {
        D2COMMON_UNITS_SetAnimationFrame(pUnit, 0);
        pUnit->dwFrameCountPrecise = nFrameCount << 8;
        D2COMMON_SKILLS_SetFlags(pSkill, 1);
    }

    pUnit->dwFlags &= ~(uint32_t)UNITFLAG_SKSRVDOFUNC;

    nGhosts = D2COMMON_SKILLS_GetParam3(pSkill);
    if (nGhosts < 2)
        ++nGhosts;
    D2COMMON_SKILLS_SetParam3(pSkill, nGhosts);
    return 1;
}


//1.10f: D2Client.0x6FAC9E50
//  Charge's end: pUnit in ecx and nSkillId in edx, no stack argument. The unit's highest-level copy of the skill (D2Common's
//  SKILLS_GetHighestLevelSkillFromUnitAndId, ecx and edx passed on as they came); a charging one (flags bit 0, test al, 1) gets bit 1 as well (or
//  al, 2 on the dword), which the next do reads as the charge told to end; and STATE_UNINTERRUPTABLE goes off (STATES_ToggleState).
//  Its callers end a skill whose `srvdofunc` is 67 (Charge and SerpentCharge): SCMD_HandleSrvA5, a skill's end from the server, and
//  SKILLS_StatRemoveCallback_EndMoveSkill, the stat SKILLS_PostSkillStatWithRemoveCallback posted running out. A NULL skill costs nothing:
//  D2Common's SKILLS_GetFlags answers 0 for it.
void __fastcall SKILLS_EndUninterruptableSkill(D2UnitStrc *pUnit, D2C_Skills nSkillId)
{
    D2SkillStrc *pSkill;
    int32_t nFlags;

    pSkill = D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, nSkillId);
    nFlags = D2COMMON_SKILLS_GetFlags(pSkill);
    if (nFlags & 1)
        D2COMMON_SKILLS_SetFlags(pSkill, nFlags | 2);

    D2COMMON_STATES_ToggleState(pUnit, STATE_UNINTERRUPTABLE, FALSE);
}

#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"

//  SkillsAma.cpp -- the Amazon's client skill code of D2Client.dll 1.10: five Skills.txt cltstfunc start functions (11, 12, 13, 14, 53) and
//  the seven cltdofunc do functions 16..22. Twelve functions, address range 0x6FABE2A0..0x6FABF4A0. The original's source file is
//  Skills\SkillsAma.cpp (cltdofunc 21 passes its path as __FILE__).
//  Every one is reached by one table slot and nothing else: the start functions from gpSkillCltStFuncs, the do functions from gpSkillCltDoFuncs.
//  Skills served. Start functions: cltstfunc 11 the Amazon's twelve bow and javelin shots (Fire Arrow .. Lightning Fury), the barbarian's
//  Double Throw and six monster shots (MonBow .. SuccubusBolt); 12 Jab; 13 Strafe; 14 Fend; 53 Zeal and Fury. Do functions: cltdofunc 16
//  Jab; 17 Multiple Shot, Teeth, PrimePoisonball, Shock Wave; 18 Guided Arrow, Bone Spirit, MonBoneSpirit; 19 Charged Strike; 20 Strafe;
//  21 Fend, Zeal, Fury; 22 Lightning Strike.
//  A start function answers whether the skill may start (SKILLS_RunCltStFunc reads the answer); the three that aim -- Strafe's, Fend's,
//  Zeal's -- also leave the plan in the used skill's three params for the do function to read: Param1 how many shots or hits, Param2 and
//  Param3 the first target's unit type and id. cltdofunc 20 and 21 read them back.
//  A skill id is a D2C_Skills and a missile id a D2C_MissilesHcIdx.

//  The D2C_AuraFilters bit 20000h, which the enum does not name: SKILLS_UnitMatchesTargetMask tests it before UNITS_IsInMeleeRange.
static const uint32_t nAuraFilterMeleeRange = 0x20000;
static const D2C_AuraFilters nShotTargetMask = (D2C_AuraFilters)(AURAFILTER_FINDPLAYER | AURAFILTER_FINDMONSTER);
static const D2C_AuraFilters nMeleeTargetMask = (D2C_AuraFilters)(AURAFILTER_FINDPLAYER | AURAFILTER_FINDMONSTER | nAuraFilterMeleeRange);


static_assert(offsetof(D2SkillsTxt, dwAuraRangeCalc) == 0x64,
              "SKILLS_CltStFunc13: `mov ecx, [ebx+64h]` (0x6FABE33E)");
static_assert(offsetof(D2SkillsTxt, wDoSoundA) == 0x102 && offsetof(D2SkillsTxt, wDoSoundB) == 0x104,
              "SKILLS_CltDoFunc16: `mov ax, [edi+102h]` (0x6FABE79E), `mov di, [edi+104h]` (0x6FABE7BF)");
static_assert(offsetof(D2SkillsTxt, wCltMissileA) == 0xEA && offsetof(D2SkillsTxt, wCltMissileB) == 0xEC
                  && offsetof(D2MissileStrc, nActivateFrame) == 0x44,
              "SKILLS_CltDoFunc17: `movsx ebp, word ptr [edi+0EAh]` (0x6FABE845), `mov ax, [edi+0ECh]` (0x6FABE85C), "
              "var_18, 44h into the block at var_5C (0x6FABE9B8)");
static_assert(WEAPONCLASS_BOW == 1 && nShotTargetMask == 3 && nMeleeTargetMask == 0x20003,
              "`cmp eax, 1` (0x6FABE857), `push 3` (0x6FABE36F), `push 20003h` (0x6FABE514)");


//1.10f: D2Client.0x6FABE2A0
//  Can a shot start (cltstfunc 11, the bow and javelin shots): SKILLS_HasAmmoForShot's answer made 0 or 1 (neg / sbb / neg). It reads neither
//  nSkillId nor nSkillLevel.
BOOL __fastcall SKILLS_CltStFunc11(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    return SKILLS_HasAmmoForShot(pUnit) != 0;
}


//1.10f: D2Client.0x6FABE2B0
//  Jab starts (cltstfunc 12). With a used skill its Param1 is set to 0 and the answer is 1; without one, 0.
BOOL __fastcall SKILLS_CltStFunc12(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return FALSE;

    D2COMMON_SKILLS_SetParam1(pSkill, 0);
    return TRUE;
}


//1.10f: D2Client.0x6FABE2D0
//  Strafe starts (cltstfunc 13). It answers 0 without ammo (SKILLS_HasAmmoForShot's own 0), without the skill's record or without a used
//  skill, and 1 otherwise. On the way it plans the volley:
//    * the search radius is the row's aurarangecalc (Strafe's is par5);
//    * with a path target, the count is SKILLS_CountNearbyTargets' answer within that radius, players and monsters; without one, the first
//      target SKILLS_FindNextNearbyTargetById finds there becomes the target and its count the count;
//    * Param1, the number of shots, is calc3 (the least) raised to the count and both held to calc1 (the most): three signed compares,
//      jle, jle, jge;
//    * with a target, Param2 and Param3 are its unit type and id, and the unit is turned to face it (D2Common's PATH_FaceCoords); with
//      none, Param2 is 1 (a monster) and Param3 -1.
//  The final count is stored back into the count's own slot before it is passed: a variable whose address went to
//  SKILLS_FindNextNearbyTargetById, so the compiler kept the store; the C does the same. The original inlines
//  DATATBLS_GetSkillsTxtRecordById, UNITS_GetYPosition and UNITS_GetXPosition here.
BOOL __fastcall SKILLS_CltStFunc13(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2SkillStrc *pSkill;
    D2UnitStrc *pTarget;
    int32_t nRadius;
    int32_t nNearbyCount;
    int32_t nShots;
    int32_t nPar1;
    int32_t nTargetX;
    int32_t nTargetY;

    if (!SKILLS_HasAmmoForShot(pUnit))
        return FALSE;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return FALSE;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return FALSE;

    nRadius = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwAuraRangeCalc, nSkillId, nSkillLevel);
    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    nNearbyCount = 0;
    if (pTarget)
        nNearbyCount = SKILLS_CountNearbyTargets(pUnit, 0, 0, nRadius, nShotTargetMask);
    else
        pTarget = SKILLS_FindNextNearbyTargetById(pUnit, 0, 0, nRadius, nShotTargetMask, (uint32_t)-1, &nNearbyCount);

    nShots = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[2], nSkillId, nSkillLevel);
    nPar1 = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);
    if (nShots > nPar1)
        nShots = nPar1;
    if (nNearbyCount > nShots)
        nShots = nNearbyCount;
    if (nShots < nPar1)
        nPar1 = nShots;

    nNearbyCount = nPar1;
    D2COMMON_SKILLS_SetParam1(pSkill, nPar1);

    if (!pTarget)
    {
        D2COMMON_SKILLS_SetParam2(pSkill, UNIT_MONSTER);
        D2COMMON_SKILLS_SetParam3(pSkill, -1);
        return TRUE;
    }

    D2COMMON_SKILLS_SetParam2(pSkill, pTarget->dwUnitType);
    D2COMMON_SKILLS_SetParam3(pSkill, pTarget->dwUnitId);

    nTargetY = UNITS_GetYPosition(pTarget);

    nTargetX = UNITS_GetXPosition(pTarget);

    D2COMMON_PATH_FaceCoords(pUnit->pDynamicPath, nTargetX, nTargetY, 0);
    return TRUE;
}


//1.10f: D2Client.0x6FABE490
//  Fend starts (cltstfunc 14). It answers 0 without the skill's record or a used skill, and 1 otherwise. The reach is
//  SKILLS_GetExtendedMeleeRange's, the mask players and monsters in melee range (20003h). With a path target the count is
//  SKILLS_CountNearbyTargets'; without one, the first target SKILLS_FindNextNearbyTargetById finds becomes the target, and when there is
//  none Param1 is 0 and the answer 1. Otherwise Param1 is the count held to calc1 (Fend's is 12; jl, signed, the held value stored back into
//  the count's slot), the unit turns to face the target, and Param2 and Param3 are the target's unit type and id. The original inlines
//  DATATBLS_GetSkillsTxtRecordById, UNITS_GetYPosition and UNITS_GetXPosition here.
BOOL __fastcall SKILLS_CltStFunc14(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2SkillStrc *pSkill;
    D2UnitStrc *pTarget;
    int32_t nReach;
    int32_t nTargetCount;
    int32_t nMaxTargets;
    int32_t nTargetX;
    int32_t nTargetY;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return FALSE;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return FALSE;

    nReach = SKILLS_GetExtendedMeleeRange(pUnit);
    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    nTargetCount = 0;
    if (pTarget)
    {
        nTargetCount = SKILLS_CountNearbyTargets(pUnit, 0, 0, nReach, nMeleeTargetMask);
    }
    else
    {
        pTarget = SKILLS_FindNextNearbyTargetById(pUnit, 0, 0, nReach, nMeleeTargetMask, (uint32_t)-1, &nTargetCount);
        if (!pTarget)
        {
            D2COMMON_SKILLS_SetParam1(pSkill, 0);
            return TRUE;
        }
    }

    nMaxTargets = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);
    if (nTargetCount >= nMaxTargets)
        nTargetCount = nMaxTargets;

    D2COMMON_SKILLS_SetParam1(pSkill, nTargetCount);

    nTargetY = UNITS_GetYPosition(pTarget);

    nTargetX = UNITS_GetXPosition(pTarget);

    D2COMMON_PATH_FaceCoords(pUnit->pDynamicPath, nTargetX, nTargetY, 0);
    D2COMMON_SKILLS_SetParam2(pSkill, pTarget->dwUnitType);
    D2COMMON_SKILLS_SetParam3(pSkill, pTarget->dwUnitId);
    return TRUE;
}


//1.10f: D2Client.0x6FABE610
//  Zeal and Fury start (cltstfunc 53). Fend's start without the count: SKILLS_FindNextNearbyTargetById is asked for no count (pnCount NULL),
//  Param1 is calc1 as it comes (Zeal's and Fury's: min(par5 + lvl - 1, par6)), and with no target at all Param1 is 0 and the answer 1. The
//  original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetYPosition and UNITS_GetXPosition here.
BOOL __fastcall SKILLS_CltStFunc53(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2SkillStrc *pSkill;
    D2UnitStrc *pTarget;
    int32_t nReach;
    int32_t nTargetX;
    int32_t nTargetY;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return FALSE;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return FALSE;

    nReach = SKILLS_GetExtendedMeleeRange(pUnit);
    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget)
    {
        pTarget = SKILLS_FindNextNearbyTargetById(pUnit, 0, 0, nReach, nMeleeTargetMask, (uint32_t)-1, NULL);
        if (!pTarget)
        {
            D2COMMON_SKILLS_SetParam1(pSkill, 0);
            return TRUE;
        }
    }

    D2COMMON_SKILLS_SetParam1(pSkill, D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel));

    nTargetY = UNITS_GetYPosition(pTarget);

    nTargetX = UNITS_GetXPosition(pTarget);

    D2COMMON_PATH_FaceCoords(pUnit->pDynamicPath, nTargetX, nTargetY, 0);
    D2COMMON_SKILLS_SetParam2(pSkill, pTarget->dwUnitType);
    D2COMMON_SKILLS_SetParam3(pSkill, pTarget->dwUnitId);
    return TRUE;
}


//1.10f: D2Client.0x6FABE760
//  Jab's sound (cltdofunc 16). On the unit's action frame 3 (a byte compare) it plays the skill's dosound a (wDoSoundA) for a player and
//  its dosound b (wDoSoundB) for a monster, each only when positive (Jab's are weapon_1ht_1 and weapon_2ht_1); the two words are read as
//  signed. It answers 0 only when the skill's record is missing on frame 3, and 1 otherwise. pUnit is read untested. The original inlines
//  DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc16(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;

    if (pUnit->nActionFrame != 3)
        return 1;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    if (pUnit->dwUnitType == UNIT_PLAYER && (int16_t)pSkillsTxt->wDoSoundA > 0)
        SOUNDINST_PlaySound((D2C_Sounds)(int16_t)pSkillsTxt->wDoSoundA, pUnit, 0, 0, 0);

    if (pUnit->dwUnitType == UNIT_MONSTER && (int16_t)pSkillsTxt->wDoSoundB > 0)
        SOUNDINST_PlaySound((D2C_Sounds)(int16_t)pSkillsTxt->wDoSoundB, pUnit, 0, 0, 0);

    return 1;
}


//1.10f: D2Client.0x6FABE7F0
//  The fan of missiles (cltdofunc 17): Multiple Shot, Teeth, PrimePoisonball and Shock Wave.
//  UNITFLAG_SKSRVDOFUNC is set on a non-NULL unit (the whole dword read, or al, 40h, stored). The missile is the row's cltmissilea, or its
//  cltmissileb when the weapon class is not a bow and cltmissileb is not negative (both words read signed); a missile outside
//  [0, missiles.txt's count) answers 0. calc1 is the number of missiles, and SKILLS_GetPathTargetPos the point aimed at.
//  The step between missiles is the vector from the unit to that point, grown to a least length and turned perpendicular
//  (CLIENT_GrowVectorToMinLength, SKILLS_ShrinkPerpendicularVector); the first missile aims at the point less half the fan, n times the step
//  over 2 (imul / cdq / sub eax, edx / sar eax, 1 / neg), a signed division truncated toward 0. The spawn record is cleared whole (rep stosd,
//  17h dwords) and given flags 820h, the unit as owner and origin, the skill, its level, the missile and calc2 as nActivateFrame, and each
//  missile takes the next point along the step. The answer is 1. The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition
//  and UNITS_GetYPosition here.
//  BUG: the NULL test on pUnit guards the flag only; D2Common's UNITS_GetWeaponClass, SKILLS_GetPathTargetPos and the inlined accessors take
//  pUnit untested. UNREACHABLE: only SKILLS_RunClientDoFunc reaches slot 17, and it cannot pass NULL.
int32_t __fastcall SKILLS_CltDoFunc17(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    int32_t nMissiles;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nStepX;
    int32_t nStepY;
    D2MissileStrc tParams;

    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (D2COMMON_UNITS_GetWeaponClass(pUnit) != WEAPONCLASS_BOW && (int16_t)pSkillsTxt->wCltMissileB >= 0)
        nMissileId = (int16_t)pSkillsTxt->wCltMissileB;

    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    nMissiles = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);

    //  BUG: the refusal after SKILLS_GetPathTargetPos cannot be taken: that function answers 1 on every path, so the jz never jumps.
    //  UNREACHABLE: the callee never answers 0.
    if (!SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY))
        return 0;

    nStepX = nTargetX - UNITS_GetXPosition(pUnit);

    nStepY = nTargetY - UNITS_GetYPosition(pUnit);

    SKILLS_GrowVectorToMinLength(&nStepX, &nStepY);
    SKILLS_ShrinkPerpendicularVector(&nStepX, &nStepY);

    nTargetX += -(nMissiles * nStepX / 2);
    nTargetY += -(nMissiles * nStepY / 2);

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETPOS | MISSILESPAWN_ACTIVATEFRAME;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = nMissileId;
    tParams.nActivateFrame = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[1], nSkillId, nSkillLevel);

    if (nMissiles > 0)
    {
        do
        {
            tParams.nTargetX = nTargetX;
            tParams.nTargetY = nTargetY;
            MISSILE_ClientSpawn(&tParams);
            nTargetX += nStepX;
            nTargetY += nStepY;
        }
        while (--nMissiles);
    }

    return 1;
}


//  Notes for the do functions 18..22. Four of them spawn a missile through MISSILE_ClientSpawn: Guided Arrow's homing shot (18), Charged
//  Strike's bolts (19), one arrow of Strafe's volley (20) and Lightning Strike's bolt (22); Fend's step (21) spawns nothing. 20 and 21 are the
//  second half of the plans cltstfunc 13, 14 and 53 leave in the used skill's params: Param1 is spent one shot or hit at a time, and Param2
//  and Param3 are moved on to the next target.
//  The next target can be the one just used: SKILLS_FindNextNearbyTargetById wraps, and its wrap takes an id at or below the one it is given
//  (SKILLS_NextTargetByIdVisitor), so the unit 20 and 21 pass as dwAfterUnitId is itself a candidate: the chain moves to another unit only
//  when one is in reach.
//  18, 19 and 22 take their missile from SKILLS_GetClientMissileForSkill (pUnit and nSkillId in ecx and edx): the row's cltmissilea, or for a
//  progressive skill with charges cltmissileb or cltmissilec, and -1 without the record. Each caller still makes its own range test on the
//  answer.

static_assert(offsetof(D2MissileStrc, pTarget) == 0x0C,
              "SKILLS_CltDoFunc18: `mov [esp+78h+var_50], ebp` into the block at var_5C (0x6FABEAEE)");
//  The path literal cltdofunc 21 passes as __FILE__. In the original it is one object, 0x6FB77878, 64 characters.
static const char szSkillsAmaCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\Skills\\SkillsAma.cpp";


//1.10f: D2Client.0x6FABEA20
//  The homing shot (cltdofunc 18): Guided Arrow, Bone Spirit and MonBoneSpirit.
//  The skill's record is looked up and only tested; then UNITFLAG_SKSRVDOFUNC is set on a non-NULL unit (or al, 40h on the whole dword),
//  SKILLS_GetPathTargetPos gives the point aimed at and SKILLS_GetClientMissileForSkill the missile, which must lie in [0, missiles.txt's
//  count). The spawn record is cleared whole (rep stosd, 17h dwords) and given flags 20h -- 420h when the unit has no path target -- the
//  unit as owner and origin, the path target (or NULL) as pTarget, the point, the skill, its level and the missile. A spawned missile gets
//  MISSILE_SetTargetX 1 with a target and 2 without, and MISSILE_SetTargetY the offset from the unit to the point packed in one dword: Y in
//  the high half, X in the low, each a 16-bit difference sign-extended (movsx edx, cx; movsx eax, di; shl edx, 10h; add edx, eax). The X
//  half is added, not ORed, so a negative X borrows one from the Y half; the C adds as the original does. The answer is 1 once past the
//  missile's range test. The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
//  BUG: the NULL test on pUnit guards the flag only; UNITS_GetPathTargetPos, SKILLS_GetClientMissileForSkill, CUNIT_GetPathTargetUnit and the
//  inlined accessors take pUnit untested. UNREACHABLE: only SKILLS_RunClientDoFunc reaches this slot, and it cannot pass NULL.
int32_t __fastcall SKILLS_CltDoFunc18(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2UnitStrc *pTarget;
    D2UnitStrc *pMissile;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nMissileId;
    int16_t nDeltaX;
    int16_t nDeltaY;
    D2MissileStrc tParams;

    if (!DATATBLS_GetSkillsTxtRecordById(nSkillId))
        return 0;

    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    //  BUG: the refusal after SKILLS_GetPathTargetPos cannot be taken (the jz): that function answers 1 on every path. UNREACHABLE: the callee
    //  never answers 0.
    if (!SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY))
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.pTarget = pTarget;
    tParams.nTargetX = nTargetX;
    tParams.nTargetY = nTargetY;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = nMissileId;
    if (!pTarget)
        tParams.dwFlags = MISSILESPAWN_TARGETPOS | MISSILESPAWN_FRAMESTOTARGET;

    pMissile = MISSILE_ClientSpawn(&tParams);
    if (pMissile)
    {
        D2COMMON_MISSILE_SetTargetX(pMissile, pTarget ? 1 : 2);

        nDeltaX = (int16_t)(nTargetX - UNITS_GetXPosition(pUnit));

        nDeltaY = (int16_t)(nTargetY - UNITS_GetYPosition(pUnit));

        D2COMMON_MISSILE_SetTargetY(pMissile, (int32_t)(((uint32_t)(int32_t)nDeltaY << 16) + (uint32_t)(int32_t)nDeltaX));
    }

    return 1;
}


//1.10f: D2Client.0x6FABEBC0
//  Charged Strike's bolts (cltdofunc 19). With the skill's record, a path target (CUNIT_GetPathTargetUnit; none answers 0) and a missile
//  from SKILLS_GetClientMissileForSkill in [0, missiles.txt's count), it spawns calc1 bolts -- Charged Strike's is par1 + lvl / par2 -- all
//  from the target's position toward the point mirrored through it: twice the target's X less the unit's (lea ecx, [edx+edx] / sub ecx, eax),
//  and the same for Y. The record is cleared whole and given flags 21h, the unit as owner and no origin, the skill, its level, the missile,
//  and SKILLS_MissileStartSeededPath as pInitFunc; each bolt takes its index as pInitArgs. The loop is skipped for a count not above 0
//  (test esi, esi / jle) and runs while the index is below it (jl), both signed. The answer is 1. It sets no flag. The original inlines
//  DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltDoFunc19(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pTarget;
    int32_t nMissileId;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nMirrorX;
    int32_t nUnitY;
    int32_t nMissiles;
    int32_t iSpawn;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget)
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    nTargetX = UNITS_GetXPosition(pTarget);

    nTargetY = UNITS_GetYPosition(pTarget);

    nMirrorX = 2 * nTargetX - UNITS_GetXPosition(pUnit);

    nUnitY = UNITS_GetYPosition(pUnit);

    nMissiles = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);

    memset(&tParams, 0, sizeof(tParams));
    tParams.nX = nTargetX;
    tParams.nY = nTargetY;
    tParams.nTargetX = nMirrorX;
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pUnit;
    tParams.nTargetY = 2 * nTargetY - nUnitY;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = nMissileId;
    tParams.pInitFunc = SKILLS_MissileStartSeededPath;

    for (iSpawn = 0; iSpawn < nMissiles; ++iSpawn)
    {
        tParams.pInitArgs = iSpawn;
        MISSILE_ClientSpawn(&tParams);
    }

    return 1;
}


//1.10f: D2Client.0x6FABEDC0
//  One arrow of Strafe's volley (cltdofunc 20). It answers 0 without the skill's record, a used skill or a Param1 above 0 (test eax, eax /
//  jle, signed). The radius is the row's aurarangecalc (Strafe's is par5, 35). The shot's target is the unit Param3 and Param2 name
//  (CUNIT_FindUnitByGuidAndType, the id first), or else the next unit after that id within the radius, players and monsters (3); when there
//  is one it becomes the unit's target (D2Common's UNITS_SetTargetUnitForDynamicUnit), and when there is none the arrow is fired all the
//  same. The point is SKILLS_GetPathTargetPos', the missile cltmissilea, or cltmissileb when the weapon class is not a bow and it is not
//  negative, as in SKILLS_CltDoFunc17; outside [0, missiles.txt's count) the answer is 0. The record is cleared whole and given flags 21h,
//  the unit as owner, its position, the point, the skill, its level and the missile.
//  Then Param1 goes down by one; at 0 or below the answer is 1. Otherwise the next unit after the shot's target (after id -1 when there was
//  none, or ebp, 0FFFFFFFFh) within the radius becomes the plan: none answers 0; one turns the unit to face it (D2Common's PATH_FaceCoords,
//  Y read first), and its unit type and id go into Param2 and Param3. Last, SKILLS_ReduceFixedFieldByPercent takes the row's par6
//  (dwParam[5], Strafe's 50) from the record looked up again, 0 without one, and the answer is 1. The original inlines
//  DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltDoFunc20(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2SkillStrc *pSkill;
    D2UnitStrc *pShotTarget;
    D2UnitStrc *pTargetUnit;
    uint32_t dwTargetGUID;
    uint32_t dwAfterUnitId;
    int32_t nShots;
    int32_t nShotsLeft;
    int32_t nRadius;
    int32_t nMissileId;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nTargetUnitX;
    int32_t nTargetUnitY;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    nShots = D2COMMON_SKILLS_GetParam1(pSkill);
    if (nShots <= 0)
        return 0;

    nRadius = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwAuraRangeCalc, nSkillId, nSkillLevel);

    dwTargetGUID = (uint32_t)D2COMMON_SKILLS_GetParam3(pSkill);
    pShotTarget = CUNIT_FindUnitByGuidAndType(dwTargetGUID, (D2C_UnitTypes)D2COMMON_SKILLS_GetParam2(pSkill));
    if (!pShotTarget)
        pShotTarget = SKILLS_FindNextNearbyTargetById(pUnit, 0, 0, nRadius, nShotTargetMask, dwTargetGUID, NULL);
    if (pShotTarget)
        D2COMMON_UNITS_SetTargetUnitForDynamicUnit(pUnit, pShotTarget);

    //  BUG: the refusal after SKILLS_GetPathTargetPos cannot be taken (the jz): that function answers 1 on every path. UNREACHABLE: the callee
    //  never answers 0, as in SKILLS_CltDoFunc17.
    if (!SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY))
        return 0;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (D2COMMON_UNITS_GetWeaponClass(pUnit) != WEAPONCLASS_BOW && (int16_t)pSkillsTxt->wCltMissileB >= 0)
        nMissileId = (int16_t)pSkillsTxt->wCltMissileB;

    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pUnit;

    tParams.nX = UNITS_GetXPosition(pUnit);

    tParams.nY = UNITS_GetYPosition(pUnit);

    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nTargetX = nTargetX;
    tParams.nMissile = nMissileId;
    tParams.nTargetY = nTargetY;
    MISSILE_ClientSpawn(&tParams);

    nShotsLeft = nShots - 1;
    D2COMMON_SKILLS_SetParam1(pSkill, nShotsLeft);
    if (nShotsLeft <= 0)
        return 1;

    dwAfterUnitId = pShotTarget ? pShotTarget->dwUnitId : (uint32_t)-1;
    pTargetUnit = SKILLS_FindNextNearbyTargetById(pUnit, 0, 0, nRadius, nShotTargetMask, dwAfterUnitId, NULL);
    if (!pTargetUnit)
        return 0;

    nTargetUnitY = UNITS_GetYPosition(pTargetUnit);

    nTargetUnitX = UNITS_GetXPosition(pTargetUnit);

    D2COMMON_PATH_FaceCoords(pUnit->pDynamicPath, nTargetUnitX, nTargetUnitY, 0);
    D2COMMON_SKILLS_SetParam2(pSkill, pTargetUnit->dwUnitType);
    D2COMMON_SKILLS_SetParam3(pSkill, pTargetUnit->dwUnitId);

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    SKILLS_ReduceFixedFieldByPercent(pUnit, pSkillsTxt ? pSkillsTxt->dwParam[5] : 0);
    return 1;
}


//1.10f: D2Client.0x6FABF0C0
//  One step of Fend, Zeal or Fury (cltdofunc 21); this file's one claimer of the path literal: it hands szSkillsAmaCpp and line 592
//  (push 250h) to D2Common's SKILLS_GetSkillIdFromSkill.
//  It answers 0 without the skill's record (looked up and only tested) or a used skill. The step's target is the unit Param3 and Param2 name
//  when it is in melee range (D2Common's UNITS_IsInMeleeRange, bonus 0), or else the next unit after that id within
//  SKILLS_GetExtendedMeleeRange's reach, players and monsters in melee range (20003h); none answers 0. Param1 goes down by one; at 0 or below
//  the answer is 1. Otherwise the next unit after the step's target becomes the plan: none answers 0; one turns the unit to face it (Y read
//  first), and its unit type and id go into Param2 and Param3. Then SKILLS_ReduceFixedFieldByPercent takes par2 (dwParam[1]; Fend's 60,
//  Zeal's and Fury's 100) from the record of the used skill's id, 0 without one; the unit's hit-class sound is played; the answer is 1.
//  The three push 0 before UNITSND_GetHitClassSound are SOUNDINST_PlaySound's: UNITSND_GetHitClassSound ends in a bare retn and nothing
//  cleans the stack after its call, while SOUNDINST_PlaySound ends in retn 0Ch. The original inlines DATATBLS_GetSkillsTxtRecordById,
//  UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltDoFunc21(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2SkillStrc *pSkill;
    D2UnitStrc *pShotTarget;
    D2UnitStrc *pTargetUnit;
    uint32_t dwTargetGUID;
    uint32_t dwAfterUnitId;
    int32_t nReach;
    int32_t nHitsLeft;
    int32_t nTargetUnitX;
    int32_t nTargetUnitY;
    int32_t nUsedSkillId;
    D2C_Sounds nSoundId;

    if (!DATATBLS_GetSkillsTxtRecordById(nSkillId))
        return 0;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    dwTargetGUID = (uint32_t)D2COMMON_SKILLS_GetParam3(pSkill);
    pShotTarget = CUNIT_FindUnitByGuidAndType(dwTargetGUID, (D2C_UnitTypes)D2COMMON_SKILLS_GetParam2(pSkill));
    nReach = SKILLS_GetExtendedMeleeRange(pUnit);
    if (!pShotTarget || !D2COMMON_UNITS_IsInMeleeRange(pUnit, pShotTarget, 0))
    {
        pShotTarget = SKILLS_FindNextNearbyTargetById(pUnit, 0, 0, nReach, nMeleeTargetMask, dwTargetGUID, NULL);
        if (!pShotTarget)
            return 0;
    }

    nHitsLeft = D2COMMON_SKILLS_GetParam1(pSkill) - 1;
    D2COMMON_SKILLS_SetParam1(pSkill, nHitsLeft);
    if (nHitsLeft <= 0)
        return 1;

    //  BUG: the step's target is never NULL where it is tested: both paths that reach the test have refused a NULL on the way (the stored
    //  target before UNITS_IsInMeleeRange, and the searched one), so the -1 arm (or esi, 0FFFFFFFFh) is dead. UNREACHABLE: pShotTarget is
    //  never NULL here.
    dwAfterUnitId = pShotTarget ? pShotTarget->dwUnitId : (uint32_t)-1;
    pTargetUnit = SKILLS_FindNextNearbyTargetById(pUnit, 0, 0, nReach, nMeleeTargetMask, dwAfterUnitId, NULL);
    if (!pTargetUnit)
        return 0;

    nTargetUnitY = UNITS_GetYPosition(pTargetUnit);

    nTargetUnitX = UNITS_GetXPosition(pTargetUnit);

    D2COMMON_PATH_FaceCoords(pUnit->pDynamicPath, nTargetUnitX, nTargetUnitY, 0);
    D2COMMON_SKILLS_SetParam2(pSkill, pTargetUnit->dwUnitType);
    D2COMMON_SKILLS_SetParam3(pSkill, pTargetUnit->dwUnitId);

    nUsedSkillId = D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillsAmaCpp, 592);

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nUsedSkillId);
    SKILLS_ReduceFixedFieldByPercent(pUnit, pSkillsTxt ? pSkillsTxt->dwParam[1] : 0);

    nSoundId = UNITSND_GetHitClassSound(pUnit, D2COMMON_UNITS_GetHitClass(pUnit));
    SOUNDINST_PlaySound(nSoundId, pUnit, 0, 0, 0);
    return 1;
}


//1.10f: D2Client.0x6FABF2A0
//  Lightning Strike's bolt (cltdofunc 22). It answers 0 without the skill's record, a missile from SKILLS_GetClientMissileForSkill in
//  [0, missiles.txt's count), a path target (the bolt's origin), or a unit found near that origin: the next after the origin's id within
//  calc1 (Lightning Strike's is 20) of the origin's position, players and monsters (3). Then calc2 is taken, the spawn record is cleared
//  whole and given flags 20h, the unit as owner, the path target as origin, the skill, its level, the missile, and the found unit's position
//  as the point; a spawned missile gets calc2 as its MISSILE_SetTargetX. The answer is 1.
//  The record's pointer and calc2's value share one stack slot in the original; the C has them as pSkillsTxtSaved and nCalc2. The original
//  inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltDoFunc22(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pOrigin;
    D2UnitStrc *pTargetUnit;
    D2UnitStrc *pMissile;
    uint32_t dwAfterUnitId;
    int32_t nMissileId;
    int32_t nRadius;
    int32_t nOriginX;
    int32_t nOriginY;
    int32_t nCalc2;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    pOrigin = CUNIT_GetPathTargetUnit(pUnit);
    if (!pOrigin)
        return 0;

    nRadius = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);
    dwAfterUnitId = pOrigin->dwUnitId;

    nOriginY = UNITS_GetYPosition(pOrigin);

    nOriginX = UNITS_GetXPosition(pOrigin);

    pTargetUnit = SKILLS_FindNextNearbyTargetById(pUnit, nOriginX, nOriginY, nRadius, nShotTargetMask, dwAfterUnitId, NULL);
    if (!pTargetUnit)
        return 0;

    nCalc2 = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[1], nSkillId, nSkillLevel);

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pOrigin;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = nMissileId;

    tParams.nTargetX = UNITS_GetXPosition(pTargetUnit);

    tParams.nTargetY = UNITS_GetYPosition(pTargetUnit);

    pMissile = MISSILE_ClientSpawn(&tParams);
    if (pMissile)
        D2COMMON_MISSILE_SetTargetX(pMissile, nCalc2);

    return 1;
}

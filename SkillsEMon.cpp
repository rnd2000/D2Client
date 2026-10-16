#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "SkillsEMon.h"

//  SkillsEMon.cpp -- the client skill code of the expansion's monsters: the classless Skills.txt rows that Lord of Destruction added
//  (Imp, Baal, the catapults, the sentries, the Siege Beast, the Death Maul, the vine creatures and Fenris) and the States.txt
//  cltactivefunc slots 88 (attached) and 89 (vine_beast).
//  The original source file is SKILLS\SkillsEMon.cpp (SKILLS_CltDoFunc89 passes its path as __FILE__). Address range
//  0x6FAC3310..0x6FAC51DF, 24 functions.
//  Every start and do function is reached by one table slot and nothing else: the cltstfunc slots from gpSkillCltStFuncs, the rest from
//  gpSkillCltDoFuncs. Slots 80 and 82 are named by no stock row.

static_assert(offsetof(D2MissilesTxt, nVel) == 0x9A && offsetof(D2MissilesTxt, nAnimLen) == 0x178,
              "SKILLS_CltDoFunc71: `mov al, [ecx+9Ah]` (0x6FAC3734), `mov ch, [eax+178h]` (0x6FAC369C)");
static_assert(MISSILE_BAAL_TAUNT_CONTROL == 0x222,
              "SKILLS_CltStFunc46: `mov edx, 222h` (0x6FAC394C), stock missiles.txt row 546");
static_assert(COMPOSIT_SPECIAL7 == 0x0E, "SKILLS_CltDoFunc74: `mov edx, 0Eh` (0x6FAC3D14)");


//1.10f: D2Client.0x6FAC3310
//  The Wake of Destruction sentry (cltdofunc 70). Answers 0 without the skill's record, or when SKILLS_GetClientMissileForSkill's missile
//  (the row's cltmissilea) lies outside [0, missiles.txt's count). Sets UNITFLAG_SKSRVDOFUNC on a non-NULL unit and makes the missile
//  from a cleared D2MissileStrc: flags 2, the unit as owner and origin, the skill and its level, and as the "target" the point minus the
//  unit's own position, a vector and not a point. The missile made, its target is set to that vector turned a quarter (MISSILE_SetTargetX
//  gets -dy, MISSILE_SetTargetY dx) and the answer is 1; a missile not made answers 0.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltDoFunc70(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    int32_t nMissileId;
    int32_t nPointX;
    int32_t nPointY;
    D2UnitStrc *pSpawned;
    D2MissileStrc tParams;

    if (!DATATBLS_GetSkillsTxtRecordById(nSkillId))
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: SKILLS_GetPathTargetPos answers 1 on every path, so this refusal never fires. UNREACHABLE: the test is dead.
    //  BUG: the server's twin refuses a point with a zero coordinate where the client's point never refuses. NOT ESTABLISHED: whether a drawn
    //  difference follows.
    if (!SKILLS_GetPathTargetPos(pUnit, &nPointX, &nPointY))
        return 0;

    //  BUG: this test guards only the flag; pUnit has already gone to SKILLS_GetPathTargetPos (which reads pUnit->pDynamicPath) and goes on
    //  untested into the inlined position accessors. UNREACHABLE: pUnit is never NULL here.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETOFFSET;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = nMissileId;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nSkill = nSkillId;

    tParams.nTargetX = nPointX - UNITS_GetXPosition(pUnit);

    tParams.nTargetY = nPointY - UNITS_GetYPosition(pUnit);

    pSpawned = MISSILE_ClientSpawn(&tParams);
    if (!pSpawned)
        return 0;

    D2COMMON_MISSILE_SetTargetX(pSpawned, -tParams.nTargetY);
    D2COMMON_MISSILE_SetTargetY(pSpawned, tParams.nTargetX);
    return 1;
}


//1.10f: D2Client.0x6FAC3480
//  Imp Inferno (cltdofunc 71): the flame, dropped from above the imp and fanned by the unit's seed.
//  The record, then cltmissilea (impinfernoflame1) as a signed word inside [0, missiles.txt's count) with its missiles.txt record, or the
//  answer is 0; cltmissileb (impinfernoflame2) is kept when in range and is -1 otherwise.
//  With a path target unit, the unit's path is aimed at that unit's position (its Y taken before its X) by D2Common's #10170 and the unit
//  faced to it at once (CUNIT_FaceUnitAtPathTarget, bSnap 1). Without one, UNITS_UpdateDirectionAndSpeed turns it to the point instead.
//  A cleared D2MissileStrc takes flags 25h, the unit as owner, its position, the skill and level, the point as target.
//  The height is cltcalc1 (8) plus the unit's graphic Z (PARTICLE_GetUnitGfxCoordZ, taken after the formula). The frame count is cltcalc2
//  plus the missile's Param2 (dwParam[1]). The animation length is taken as an 8.8 fixed-point value, and with a non-zero frame count
//  both it and the fall (minus twice the height) are divided by the count, signed; with a count of 0 neither is divided.
//  The missile is cltmissilea, or cltmissileb when that is above 0 (0 is refused as well as -1) and one roll of the unit's seed comes out
//  odd. A second roll picks the velocity: the missile's Vel plus gnImpInfernoSpread[roll % 5], unsigned.
//  The missile made: PATH_SetNewDistance takes the frame count as its byte, UNITS_SetAnimationSpeed the scaled length,
//  MISSILE_SetTotalFrames and MISSILE_SetCurrentFrame the count both (a whole life from its first frame, since the current frame counts
//  down); then PARTICLE_SetUnitGfxCoords lifts it to the height, PARTICLE_UpdateUnitParticle runs and PARTICLE_SetUnitGfxVector sets the
//  fall. The answer is 1 whether a missile was made or not.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetYPosition, UNITS_GetXPosition and SEED_RollRandomNumber here.
int32_t __fastcall SKILLS_CltDoFunc71(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileA;
    int32_t nMissileB;
    D2UnitStrc *pTarget;
    int32_t nTargetUnitX;
    int32_t nTargetUnitY;
    int32_t nPointX;
    int32_t nPointY;
    int32_t nZ;
    int32_t nZVelocity;
    int32_t nFrames;
    int32_t nAnimLenScaled;
    int32_t nAnimSpeed;
    D2UnitStrc *pSpawned;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileA = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileA < 0)
        return 0;

    if (nMissileA >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileA];
    if (!pMissilesTxt)
        return 0;

    nMissileB = (int16_t)pSkillsTxt->wCltMissileB;
    if (nMissileB < 0 || nMissileB >= sgptDataTables->nMissilesTxtRecordCount)
        nMissileB = -1;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (pTarget)
    {
        nTargetUnitY = UNITS_GetYPosition(pTarget);

        nTargetUnitX = UNITS_GetXPosition(pTarget);

        D2COMMON_10170_PathSetTargetPos(pUnit->pDynamicPath, nTargetUnitX, nTargetUnitY);
        CUNIT_FaceUnitAtPathTarget(pUnit, 1);
    }

    //  BUG: SKILLS_GetPathTargetPos answers 1 on every path, so this refusal never fires. UNREACHABLE: the test is dead.
    if (!SKILLS_GetPathTargetPos(pUnit, &nPointX, &nPointY))
        return 0;

    if (!pTarget)
        D2COMMON_UNITS_UpdateDirectionAndSpeed(pUnit, nPointX, nPointY);

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_VELOCITY | MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pUnit;

    tParams.nX = UNITS_GetXPosition(pUnit);

    tParams.nY = UNITS_GetYPosition(pUnit);

    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nTargetX = nPointX;
    tParams.nTargetY = nPointY;

    nZ = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[0], nSkillId, nSkillLevel);
    nZ += PARTICLE_GetUnitGfxCoordZ(pUnit);

    nFrames = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[1], nSkillId, nSkillLevel);
    nFrames += pMissilesTxt->dwParam[1];
    nAnimLenScaled = pMissilesTxt->nAnimLen << 8;
    nZVelocity = -2 * nZ;

    nAnimSpeed = nAnimLenScaled;
    if (nFrames)
    {
        nAnimSpeed = nAnimLenScaled / nFrames;
        nZVelocity /= nFrames;
    }

    tParams.nMissile = nMissileA;
    //  BUG: the client picks between cltmissilea and cltmissileb by the seed where the server's twin makes one missile. REACHABLE: the drawn
    //  flame is not the server's.
    if (nMissileB > 0)
    {
        if ((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) & 1)
            tParams.nMissile = nMissileB;
    }

    //  BUG: the velocity is Vel plus the spread where the server's is Vel plus VelLev times the level over 8. REACHABLE: stock
    //  impinfernoflame1 and 2 carry Vel 20 and no VelLev, so the server's flame flies at 20 and the client's at 18 to 22.
    tParams.nVelocity = pMissilesTxt->nVel + gnImpInfernoSpread[(uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 5];

    pSpawned = MISSILE_ClientSpawn(&tParams);
    if (pSpawned)
    {
        D2COMMON_PATH_SetNewDistance(pSpawned->pDynamicPath, (uint8_t)nFrames);
        D2COMMON_UNITS_SetAnimationSpeed(pSpawned, nAnimSpeed);
        D2COMMON_MISSILE_SetTotalFrames(pSpawned, nFrames);
        D2COMMON_MISSILE_SetCurrentFrame(pSpawned, nFrames);
        PARTICLE_SetUnitGfxCoords(pSpawned, 0, 0, nZ);
        PARTICLE_UpdateUnitParticle(pSpawned);
        PARTICLE_SetUnitGfxVector(pSpawned, 0, 0, nZVelocity, 1);
    }

    return 1;
}


//1.10f: D2Client.0x6FAC37C0
//  Imp Fireball (cltdofunc 72). The record, SKILLS_GetClientMissileForSkill's missile in range (impfireball) and
//  SKILLS_GetPathTargetPos's answer; then UNITFLAG_SKSRVDOFUNC on a non-NULL unit and MISSILE_ClientSpawnAtPathTarget with the missile,
//  the skill and its level. A missile made is lifted to cltcalc2 plus the unit's graphic Z (the formula first) and falls that height over
//  its frame count, signed, or over 2 when the count is 0 (a signed halving); then PARTICLE_SetUnitGfxCoords, PARTICLE_SetUnitGfxVector
//  with the step negated, then PARTICLE_UpdateUnitParticle, in that order (cltdofunc 71 updates the particle between the two). The answer
//  is 1, and 0 when no missile is made. cltcalc1 (40) is not read.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc72(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    int32_t nPointX;
    int32_t nPointY;
    D2UnitStrc *pMissile;
    int32_t nTotalFrames;
    int32_t nZ;
    int32_t nZStep;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: SKILLS_GetPathTargetPos answers 1 on every path, so this refusal never fires. UNREACHABLE: the test is dead.
    if (!SKILLS_GetPathTargetPos(pUnit, &nPointX, &nPointY))
        return 0;

    //  BUG: this test guards only the flag; pUnit has already gone to SKILLS_GetPathTargetPos. UNREACHABLE: pUnit is never NULL here.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    pMissile = MISSILE_ClientSpawnAtPathTarget(pUnit, nMissileId, nSkillId, nSkillLevel);
    if (!pMissile)
        return 0;

    nTotalFrames = D2COMMON_MISSILE_GetTotalFrames(pMissile);
    nZ = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[1], nSkillId, nSkillLevel);
    nZ += PARTICLE_GetUnitGfxCoordZ(pUnit);
    if (nTotalFrames)
        nZStep = nZ / nTotalFrames;
    else
        nZStep = nZ / 2;

    PARTICLE_SetUnitGfxCoords(pMissile, 0, 0, nZ);
    PARTICLE_SetUnitGfxVector(pMissile, 0, 0, -nZStep, 1);
    PARTICLE_UpdateUnitParticle(pMissile);
    return 1;
}


//1.10f: D2Client.0x6FAC38E0
//  Baal's taunt (cltstfunc 46), the start of Baal Taunt; reads nSkillLevel only to pass it on. The record, and
//  SKILLS_GetClientMissileForSkill's missile in range, or the answer is 0 -- but the missile spawned is not that one: it is the constant
//  MISSILE_BAAL_TAUNT_CONTROL, which is what the row's cltmissilea names in stock data. Before it, the row's prgsound
//  (monster_baal_taunt_1) plays when it is above 0 as a signed word, with no unit and three zero arguments.
//  SKILLS_MissileClientSpawnOnTargetUnit makes the missile and its answer is not read; the answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltStFunc46(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    if ((int16_t)pSkillsTxt->wPrgSound > 0)
        SOUNDINST_PlaySound((D2C_Sounds)(int16_t)pSkillsTxt->wPrgSound, NULL, 0, 0, 0);

    SKILLS_MissileClientSpawnOnTargetUnit(pUnit, MISSILE_BAAL_TAUNT_CONTROL, nSkillId, nSkillLevel);
    return 1;
}


//1.10f: D2Client.0x6FAC3970
//  The catapults (cltstfunc 47), the start of the five catapult rows. The record; SKILLS_GetClientMissileForSkill's missile in range,
//  with its missiles.txt record; then D2Common's #11066 (the monster's direction from its chain index, one of four unit vectors) into two
//  zeroed ints, and a vector of (0, 0) answers 0. The missile is made by SKILLS_MissileClientSpawnOnTargetUnit (a missile not made
//  answers 0) and thrown back along the vector: the missile's Range, a signed word raised to at least 1, times the vector, negated, is
//  its start offset and the vector itself its step, both converted by DUNGEON_GameSubtileToClientCoords. cltcalc1 (16 for every catapult)
//  is the fall per step, so PARTICLE_SetUnitGfxCoords places it at range times that height and PARTICLE_SetUnitGfxVector drops it by one
//  fall each step. PARTICLE_UpdateUnitParticle runs last, and the answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltStFunc47(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pMissile;
    int32_t nDirX;
    int32_t nDirY;
    int32_t nStartX;
    int32_t nStartY;
    int32_t nStepX;
    int32_t nStepY;
    int32_t nRange;
    int32_t nZSpeed;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0)
        return 0;

    if (nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt)
        return 0;

    nDirX = 0;
    nDirY = 0;
    D2COMMON_11066(pUnit, &nDirX, &nDirY);
    if (!nDirX && !nDirY)
        return 0;

    pMissile = SKILLS_MissileClientSpawnOnTargetUnit(pUnit, (D2C_MissilesHcIdx)nMissileId, nSkillId, nSkillLevel);
    if (!pMissile)
        return 0;

    nRange = (int16_t)pMissilesTxt->wRange;
    if (nRange < 1)
        nRange = 1;

    nZSpeed = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[0], nSkillId, nSkillLevel);
    nStartX = -(nRange * nDirX);
    nStartY = -(nRange * nDirY);
    D2COMMON_DUNGEON_GameSubtileToClientCoords(&nStartX, &nStartY);
    nStepX = nDirX;
    nStepY = nDirY;
    D2COMMON_DUNGEON_GameSubtileToClientCoords(&nStepX, &nStepY);
    PARTICLE_SetUnitGfxCoords(pMissile, nStartX, nStartY, nZSpeed * nRange);
    PARTICLE_SetUnitGfxVector(pMissile, nStepX, nStepY, -nZSpeed, 1);
    PARTICLE_UpdateUnitParticle(pMissile);
    return 1;
}


//1.10f: D2Client.0x6FAC3B00
//  The teleports' start (cltstfunc 48): Teleport 2, Baal Teleport, Imp Teleport and Baal Clone Teleport. The record, or the answer is 0;
//  then, when cltmissilea is a signed word inside [0, missiles.txt's count), MISSILE_ClientSpawnSimple makes it at the unit's own
//  position (Y taken before X) with the skill and its level. The spawn's answer is not read, and the answer is 1 either way. In stock
//  data only Imp Teleport and Baal Clone Teleport carry a cltmissilea; Teleport 2 and Baal Teleport leave it empty (-1).
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetYPosition and UNITS_GetXPosition here.
int32_t __fastcall SKILLS_CltStFunc48(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    int32_t nUnitX;
    int32_t nUnitY;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
    {
        nUnitY = UNITS_GetYPosition(pUnit);

        nUnitX = UNITS_GetXPosition(pUnit);

        MISSILE_ClientSpawnSimple(pUnit, nMissileId, nUnitX, nUnitY, nSkillId, nSkillLevel);
    }

    return 1;
}


//1.10f: D2Client.0x6FAC3BC0
//  The teleports' do (cltdofunc 73): Teleport 2's, Baal Teleport's and Baal Clone Teleport's. The record, or the answer is 0; cltmissileb
//  outside [0, missiles.txt's count) answers 1 with nothing made; otherwise MISSILE_ClientSpawnSimple makes it at the unit's position (Y
//  before X) at skill level 1, the literal and not nSkillLevel, and the answer is whether a missile was made. Stock: baalteleport for the
//  two Baal rows; Teleport 2's column is empty (-1), so its do makes nothing and answers 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetYPosition and UNITS_GetXPosition here.
BOOL __fastcall SKILLS_CltDoFunc73(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    int32_t nUnitX;
    int32_t nUnitY;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return FALSE;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileB;
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return TRUE;

    nUnitY = UNITS_GetYPosition(pUnit);

    nUnitX = UNITS_GetXPosition(pUnit);

    return MISSILE_ClientSpawnSimple(pUnit, nMissileId, nUnitX, nUnitY, nSkillId, 1) != 0;
}


//1.10f: D2Client.0x6FAC3C90
//  Imp Teleport's do (cltdofunc 74). The record, or the answer is 0; cltmissileb (imp teleport) outside [0, missiles.txt's count) answers
//  1 with nothing made. Otherwise SKILLS_MissileClientSpawnOnTargetUnit makes it at skill level 1, the literal, and a missile not made
//  answers 0 (that function's own 0, returned as it came). With a path target unit the missile's particles are allocated and it is hung
//  on that unit's COMPOSIT_SPECIAL7 cel: GFX_GetUnitCel's two offsets, the Y negated as a height, go to PARTICLE_SetUnitGfxCoords as X
//  and Z. The answer is 1. Imp Teleport's aurastate is attached, whose client function is SKILLS_CltDoFunc88.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc74(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    D2UnitStrc *pMissile;
    D2UnitStrc *pTarget;
    int32_t nOffsetX;
    int32_t nOffsetY;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileB;
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 1;

    pMissile = SKILLS_MissileClientSpawnOnTargetUnit(pUnit, (D2C_MissilesHcIdx)nMissileId, nSkillId, 1);
    if (!pMissile)
        return 0;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (pTarget)
    {
        PARTICLE_AllocUnitParticles(pMissile);
        GFX_GetUnitCel(pTarget, COMPOSIT_SPECIAL7, &nOffsetX, &nOffsetY);
        PARTICLE_SetUnitGfxCoords(pMissile, nOffsetX, 0, -nOffsetY);
    }

    return 1;
}


static_assert(UNITFLAGEX_NODRAW == 0x40000,
              "SKILLS_CltDoFunc76: `or eax, 40000h` (0x6FAC42BF), `or dword ptr [esi+0C8h], 40000h` (0x6FAC4397)");


//1.10f: D2Client.0x6FAC3D50
//  The vines' start (cltstfunc 49): Vine Attack, CorpseCycler and VineCycler. The record, or the answer is 0; then UNITFLAG_SKSRVDOFUNC
//  on a non-NULL unit, and two independent halves, each gated by its column being a signed word inside [0, missiles.txt's count).
//  cltmissilea (vine beast attack, all three rows) is made by MISSILE_ClientSpawnSimple at the unit's own position (Y before X); a
//  missile made is turned to the unit's direction (UNITS_GetDirection, then PATH_SetDirection on its path) and its path's rotate flag
//  cleared.
//  cltmissileb (vines for Vine Attack, recycler delay for the two cyclers) fans out from the point. cltcalc1 (min(12,ln12) for Vine
//  Attack, empty for the cyclers) is the count; a cleared D2MissileStrc takes flags 3, the unit as owner, the skill and level, the
//  missile and SKILLS_MissileStartSeededPath as its init callback, and SKILLS_GetPathTargetPos writes the point straight into its nX and
//  nY. Then missile i, for i in [0, count) (signed), gets i as the init argument and as its "target" gnDir8SignX/Y[i % 8], a signed
//  remainder, the same as C's for i >= 0. The answer is 1 once the record is found.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetYPosition and UNITS_GetXPosition here.
//  BUG: the direction is stored as a byte into a frame dword nothing has written yet and the whole dword is pushed to PATH_SetDirection,
//  so three stale bytes ride along. INERT: PATH_SetDirection reads the byte alone.
int32_t __fastcall SKILLS_CltStFunc49(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileA;
    int32_t nMissileB;
    int32_t nUnitX;
    int32_t nUnitY;
    D2UnitStrc *pMissile;
    D2DynamicPathStrc *pPath;
    uint8_t nDirection;
    int32_t nCount;
    int32_t i;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    //  BUG: this test guards only the flag; pUnit then goes untested into the position accessors of the first half and to D2Common's #11276
    //  and SKILLS_GetPathTargetPos in the second, so only a row with neither missile column would survive a NULL. UNREACHABLE: pUnit is never
    //  NULL here.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    nMissileA = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileA >= 0 && nMissileA < sgptDataTables->nMissilesTxtRecordCount)
    {
        nUnitY = UNITS_GetYPosition(pUnit);

        nUnitX = UNITS_GetXPosition(pUnit);

        pMissile = MISSILE_ClientSpawnSimple(pUnit, nMissileA, nUnitX, nUnitY, nSkillId, nSkillLevel);
        if (pMissile)
        {
            nDirection = D2COMMON_UNITS_GetDirection(pUnit);
            pPath = pMissile->pDynamicPath;
            D2COMMON_PATH_SetDirection(pPath, nDirection);
            D2COMMON_10203_PATH_SetRotateFlag(pPath, FALSE);
        }
    }

    nMissileB = (int16_t)pSkillsTxt->wCltMissileB;
    if (nMissileB >= 0 && nMissileB < sgptDataTables->nMissilesTxtRecordCount)
    {
        nCount = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[0], nSkillId, nSkillLevel);
        memset(&tParams, 0, sizeof(tParams));
        tParams.nSkillLevel = nSkillLevel;
        tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET;
        tParams.pOwner = pUnit;
        tParams.nSkill = nSkillId;
        tParams.nMissile = nMissileB;
        tParams.pInitFunc = SKILLS_MissileStartSeededPath;

        //  BUG: SKILLS_GetPathTargetPos answers 1 on every path, so this refusal never fires. UNREACHABLE: the test is dead.
        //  BUG: the server's Vine Attack makes calc1 (min(24,ln12), where the client's cltcalc1 is min(12,ln12)) missiles in four directions (i %
        //  4) where the client fans out in eight (i % 8), and refuses a point on a zero coordinate. REACHABLE: from the skill's level 3 the drawn
        //  vines fan out differently from the server's; the zero coordinate is NOT ESTABLISHED.
        if (SKILLS_GetPathTargetPos(pUnit, &tParams.nX, &tParams.nY))
        {
            for (i = 0; i < nCount; ++i)
            {
                tParams.pInitArgs = i;
                tParams.nTargetX = gnDir8SignX[i % 8];
                tParams.nTargetY = gnDir8SignY[i % 8];
                MISSILE_ClientSpawn(&tParams);
            }
        }
    }

    return 1;
}


//1.10f: D2Client.0x6FAC3F30
//  The Siege Beast's stomp (cltdofunc 75); reads no pUnit at all. It shakes the screen: VIEWEFFECTS_StartScreenShake with the row's
//  Param1 as the magnitude and Param2, Param3 and Param4 each times 1000 over 25 (a signed division) as the three times of the shake. The
//  record is looked up anew for each of the four, a missing one reading as 0. Stock Siege Beast Stomp: 8, 5, 20, 15, so magnitude 8, then
//  200, 800 and 600. The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc75(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMagnitude;
    int32_t nRampUpMs;
    int32_t nHoldMs;
    int32_t nRampDownMs;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    nMagnitude = pSkillsTxt ? pSkillsTxt->dwParam[0] : 0;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    nRampUpMs = 1000 * (pSkillsTxt ? pSkillsTxt->dwParam[1] : 0) / 25;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    nHoldMs = 1000 * (pSkillsTxt ? pSkillsTxt->dwParam[2] : 0) / 25;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    nRampDownMs = 1000 * (pSkillsTxt ? pSkillsTxt->dwParam[3] : 0) / 25;

    VIEWEFFECTS_StartScreenShake(nMagnitude, nRampUpMs, nHoldMs, nRampDownMs);
    return 1;
}


//1.10f: D2Client.0x6FAC4070
//  The Death Maul (cltdofunc 76): a trail of cltmissileb (death mauler trail) along the line to the point, then cltmissilea (death
//  mauler) on the target.
//  The record; cltmissileb a signed word inside [0, missiles.txt's count), with its missiles.txt record, or 0; then
//  SKILLS_GetPathTargetPos's point (its answer tested and dead, returned as it came). UNITS_GetDistanceToCoordinates to the point is
//  raised to at least 1 (a signed compare, though the export is declared unsigned); the unit's X and Y; and the two steps per subtile are
//  100 times the difference over the distance, signed.
//  The trail missile's Param1 (+38h) raised to at least 1 is the stride and its Param2 (+3Ch) raised to at least 1 the frame step. While
//  the walked distance (the stride, then the stride again each pass) is below the distance, a trail missile is made by
//  MISSILE_ClientSpawnSimple at the unit's position plus the accumulated step over 100 (signed; Y before X). A missile made is turned to
//  the unit's direction by D2Common's #10160, which reads the byte alone; its total and current frames are its own total plus the frame
//  offset, its activate frame its total minus it, and it is marked UNITFLAGEX_NODRAW. Each pass adds the frame step to the offset and
//  counts one spawn, made or not.
//  cltmissilea in range: SKILLS_MissileClientSpawnOnTargetUnit makes it (a missile not made answers that function's 0) and calc1, the
//  server's column (+138h, 96 for the row), is its animation speed when above 0; its frames are shifted by the spawn count times the
//  frame step, the same ladder at its end, and it is marked UNITFLAGEX_NODRAW too. The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltDoFunc76(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2MissilesTxt *pMissilesTxt;
    int32_t nMissileB;
    int32_t nMissileA;
    int32_t nPointX;
    int32_t nPointY;
    int32_t nDistance;
    int32_t nUnitX;
    int32_t nUnitY;
    int32_t nStepXScaled;
    int32_t nStepYScaled;
    int32_t nStride;
    int32_t nFrameStep;
    int32_t nWalked;
    int32_t nSpawnIndex;
    int32_t nFrameOffset;
    int32_t nStepX;
    int32_t nStepY;
    int32_t nAccX;
    int32_t nAccY;
    D2UnitStrc *pMissile;
    D2UnitStrc *pHead;
    int32_t nTotalFrames;
    int32_t nSpeed;
    int32_t nOffset;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileB = (int16_t)pSkillsTxt->wCltMissileB;
    if (nMissileB < 0)
        return 0;

    if (nMissileB >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileB];
    if (!pMissilesTxt)
        return 0;

    //  BUG: SKILLS_GetPathTargetPos answers 1 on every path, so this refusal never fires. UNREACHABLE: the test is dead.
    if (!SKILLS_GetPathTargetPos(pUnit, &nPointX, &nPointY))
        return 0;

    nDistance = (int32_t)D2COMMON_UNITS_GetDistanceToCoordinates(pUnit, nPointX, nPointY);
    if (nDistance < 1)
        nDistance = 1;

    nUnitX = UNITS_GetXPosition(pUnit);

    nUnitY = UNITS_GetYPosition(pUnit);

    nStepXScaled = 100 * (nPointX - nUnitX) / nDistance;
    nStepYScaled = 100 * (nPointY - nUnitY) / nDistance;
    nStride = pMissilesTxt->dwParam[0] >= 1 ? pMissilesTxt->dwParam[0] : 1;
    nFrameStep = pMissilesTxt->dwParam[1] >= 1 ? pMissilesTxt->dwParam[1] : 1;
    nSpawnIndex = 0;

    if (nStride < nDistance)
    {
        nFrameOffset = 0;
        nWalked = nStride;
        nStepX = nStride * nStepXScaled;
        nStepY = nStride * nStepYScaled;
        nAccX = nStepX;
        nAccY = nStepY;
        do
        {
            pMissile = MISSILE_ClientSpawnSimple(pUnit, nMissileB, nUnitX + nAccX / 100, nUnitY + nAccY / 100, nSkillId, nSkillLevel);
            if (pMissile)
            {
                D2COMMON_10160_PathUpdateDirection(pMissile->pDynamicPath, D2COMMON_UNITS_GetDirection(pUnit));
                nTotalFrames = D2COMMON_MISSILE_GetTotalFrames(pMissile);
                D2COMMON_MISSILE_SetTotalFrames(pMissile, nFrameOffset + nTotalFrames);
                D2COMMON_MISSILE_SetCurrentFrame(pMissile, nFrameOffset + nTotalFrames);
                D2COMMON_MISSILE_SetActivateFrame(pMissile, nTotalFrames - nFrameOffset);
                pMissile->dwFlagEx |= UNITFLAGEX_NODRAW;
            }

            nAccY += nStepY;
            nAccX += nStepX;
            nWalked += nStride;
            ++nSpawnIndex;
            nFrameOffset += nFrameStep;
        }
        while (nWalked < nDistance);
    }

    nMissileA = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileA >= 0 && nMissileA < sgptDataTables->nMissilesTxtRecordCount)
    {
        pHead = SKILLS_MissileClientSpawnOnTargetUnit(pUnit, (D2C_MissilesHcIdx)nMissileA, nSkillId, nSkillLevel);
        if (!pHead)
            return 0;

        nSpeed = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);
        if (nSpeed > 0)
            D2COMMON_UNITS_SetAnimationSpeed(pHead, nSpeed);

        nTotalFrames = D2COMMON_MISSILE_GetTotalFrames(pHead);
        nOffset = nSpawnIndex * nFrameStep;
        D2COMMON_MISSILE_SetTotalFrames(pHead, nOffset + nTotalFrames);
        D2COMMON_MISSILE_SetCurrentFrame(pHead, nOffset + nTotalFrames);
        D2COMMON_MISSILE_SetActivateFrame(pHead, nTotalFrames - nOffset);
        pHead->dwFlagEx |= UNITFLAGEX_NODRAW;
    }

    return 1;
}


//1.10f: D2Client.0x6FAC43B0
//  The inferno sentry's start (cltstfunc 52, mon inferno sentry). The record, and the unit's used skill (UNITS_GetUsedSkill), or the
//  answer is 0 (that function's own 0). calc2 (+13Ch, par1 plus the Wake of Fire Sentry's base level) raised to at least 1 plus the frame
//  count (GAME_GetFrameCount, called after the formula) becomes the skill's Param1, a deadline in frames, and the unit gets STATE_INFERNO
//  when it lacks it. The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltStFunc52(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2SkillStrc *pSkill;
    int32_t nDelay;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    nDelay = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[1], nSkillId, nSkillLevel);
    if (nDelay < 1)
        nDelay = 1;

    D2COMMON_SKILLS_SetParam1(pSkill, nDelay + GAME_GetFrameCount());
    if (!D2COMMON_STATES_CheckState(pUnit, STATE_INFERNO))
        D2COMMON_STATES_ToggleState(pUnit, STATE_INFERNO, TRUE);

    return 1;
}


//1.10f: D2Client.0x6FAC4450
//  The inferno sentry's do (cltdofunc 77). The record, and the used skill, or the answer is 0. When the frame count is past the skill's
//  Param1, the deadline cltstfunc 52 set (an unsigned compare, Param1 read first), STATE_INFERNO is taken off the unit and the answer is
//  0. Otherwise SKILLS_SpawnInfernoStream makes the stream (none made answers 0), whose particles are allocated, which is lifted to
//  cltcalc1 (-37 for the row) by PARTICLE_SetUnitGfxCoords and told to end its motion on rising to its target
//  (PARTICLE_SetUnitRisesToTarget, 1); UNITFLAG_SKSRVDOFUNC is cleared on a non-NULL unit, and the answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc77(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2SkillStrc *pSkill;
    uint32_t nDeadlineFrame;
    D2UnitStrc *pStream;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    nDeadlineFrame = (uint32_t)D2COMMON_SKILLS_GetParam1(pSkill);
    if (GAME_GetFrameCount() > nDeadlineFrame)
    {
        D2COMMON_STATES_ToggleState(pUnit, STATE_INFERNO, FALSE);
        return 0;
    }

    pStream = SKILLS_SpawnInfernoStream(pUnit, nSkillId, nSkillLevel);
    if (!pStream)
        return 0;

    PARTICLE_AllocUnitParticles(pStream);
    PARTICLE_SetUnitGfxCoords(pStream, 0, 0, D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[0], nSkillId, nSkillLevel));
    PARTICLE_SetUnitRisesToTarget(pStream, 1);

    //  BUG: this test guards only the flag; pUnit has already gone to UNITS_GetUsedSkill, SKILLS_SpawnInfernoStream and D2Common's #11276.
    //  UNREACHABLE: pUnit is never NULL here.
    if (pUnit)
        pUnit->dwFlags &= ~UNITFLAG_SKSRVDOFUNC;

    return 1;
}


//1.10f: D2Client.0x6FAC4540
//  The death sentry's start (cltstfunc 50, mon death sentry); reads neither nSkillId nor nSkillLevel, and no record. The unit's used
//  skill and its path target unit, or the answer is 0; then the target's X becomes the skill's Param1 and its Y Param2, and the answer is
//  1. These are the coordinates cltdofunc 78 draws at.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltStFunc50(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;
    D2UnitStrc *pTarget;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget)
        return 0;

    D2COMMON_SKILLS_SetParam1(pSkill, UNITS_GetXPosition(pTarget));

    D2COMMON_SKILLS_SetParam2(pSkill, UNITS_GetYPosition(pTarget));
    return 1;
}


//1.10f: D2Client.0x6FAC4600
//  The death sentry's do (cltdofunc 78). The record and the used skill, or 0; the skill's Param1 and Param2 (the target's position
//  cltstfunc 50 stored), or 0 when either is 0. Then cltmissilea (corpseexplosion) in range is made there by MISSILE_ClientSpawnSimple; a
//  missile not made answers 0, and one made plays the row's prgsound (necromancer_corpseexp_1) when above 0 as a signed word, from the
//  missile. cltmissileb (deathsentryexplode) in range is made there too, its answer not read. The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc78(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2SkillStrc *pSkill;
    int32_t nX;
    int32_t nY;
    int32_t nMissileId;
    D2UnitStrc *pMissile;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    nX = D2COMMON_SKILLS_GetParam1(pSkill);
    nY = D2COMMON_SKILLS_GetParam2(pSkill);
    if (!nX || !nY)
        return 0;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
    {
        pMissile = MISSILE_ClientSpawnSimple(pUnit, nMissileId, nX, nY, nSkillId, nSkillLevel);
        if (!pMissile)
            return 0;

        if ((int16_t)pSkillsTxt->wPrgSound > 0)
            SOUNDINST_PlaySound((D2C_Sounds)(int16_t)pSkillsTxt->wPrgSound, pMissile, 0, 0, 0);
    }

    nMissileId = (int16_t)pSkillsTxt->wCltMissileB;
    if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
        MISSILE_ClientSpawnSimple(pUnit, nMissileId, nX, nY, nSkillId, nSkillLevel);

    return 1;
}


//1.10f: D2Client.0x6FAC4710
//  Fenris's start (cltstfunc 51, fenris rage); reads neither nSkillId nor nSkillLevel. The used skill and the path target unit, and
//  D2Common's #11018 on the target (a corpse the skill may use), or the answer is 0; then the target's X, Y and class id become the
//  skill's Param1, Param2 and Param3, and the answer is 1.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltStFunc51(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;
    D2UnitStrc *pTarget;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget || !D2COMMON_11018(pTarget))
        return 0;

    D2COMMON_SKILLS_SetParam1(pSkill, UNITS_GetXPosition(pTarget));

    D2COMMON_SKILLS_SetParam2(pSkill, UNITS_GetYPosition(pTarget));
    D2COMMON_SKILLS_SetParam3(pSkill, (int32_t)pTarget->dwClassId);
    return 1;
}


//1.10f: D2Client.0x6FAC47C0
//  Fenris's do (cltdofunc 79). The record and the used skill, or 0; the skill's Param1 and Param2 (the corpse's position cltstfunc 51
//  stored), or 0 when either is 0. The unit's path is aimed there (D2Common's #10170), and cltmissilea (corpseexplosion) in range is made
//  there by MISSILE_ClientSpawnSimple, its answer not read. The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc79(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2SkillStrc *pSkill;
    int32_t nX;
    int32_t nY;
    int32_t nMissileId;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    nX = D2COMMON_SKILLS_GetParam1(pSkill);
    nY = D2COMMON_SKILLS_GetParam2(pSkill);
    if (!nX || !nY)
        return 0;

    D2COMMON_10170_PathSetTargetPos(pUnit->pDynamicPath, nX, nY);

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
        MISSILE_ClientSpawnSimple(pUnit, nMissileId, nX, nY, nSkillId, nSkillLevel);

    return 1;
}


static_assert(SKILL_BAALCORPSEEXPLODE == 285 && MISSILE_BAALCORPSEEXPLODEDELAY == 0x24B,
              "SKILLS_CltDoFunc82: `cmp dword ptr [eax+0BA0h], 11Dh` (0x6FAC4FF4), `mov edx, 24Bh` (0x6FAC5163)");


static_assert(MISSILE_VINE_BEAST_NEUTRAL == 0x1D4,
              "SKILLS_StatesMatchVineBeastNeutralMissile: `cmp ecx, 1D4h` (0x6FAC4BD1)");
static_assert(SKILL_VINEATTACK == 0x126,
              "SKILLS_CltDoFunc89: `mov edx, 126h` (0x6FAC48B4, 0x6FAC48DE), `push 126h` (0x6FAC48D3)");

//  The path SKILLS_CltDoFunc89 passes as __FILE__, twice.
static const char szSkillsEMonCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\SKILLS\\SkillsEMon.cpp";


//1.10f: D2Client.0x6FAC4870
//  States.txt cltactivefunc 88: attached, Imp Teleport's aurastate. Reads neither nSkillId nor nSkillLevel. The unit's owner
//  (CUNIT_GetOwnerUnit) or the answer is that function's 0; then D2Common's #10235, PATH_UpdateRiderPath, keeps the unit on its owner's
//  path, and the answer is 1.
int32_t __fastcall SKILLS_CltDoFunc88(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2UnitStrc *pOwner;

    pOwner = CUNIT_GetOwnerUnit(pUnit);
    if (!pOwner)
        return 0;

    D2COMMON_10235_PATH_UpdateRiderPath(pUnit, pOwner);
    return 1;
}


//1.10f: D2Client.0x6FAC4890
//  States.txt cltactivefunc 89: vine_beast, which has no server counterpart. It ignores both arguments and works from the unit's own Vine
//  Attack.
//  A unit that is not a monster answers 0. A monster without Vine Attack is given it at level 1 (SKILLS_AssignSkill) and asked again;
//  still none, the answer is 0. The skill's id (SKILLS_GetSkillIdFromSkill) and its level with bonuses stand in for the two arguments,
//  stored over their own frame slots. Param1 is a counter, taken and stored back one higher; Param2 and Param3 are the point of the last
//  vine.
//  The skill's record, or 0. The vine fires when the counter has reached cltcalc2 (35) or the unit is at least cltcalc2 from the last
//  point (the formula evaluated twice, once for each; the distance compared signed, though the export is declared unsigned).
//  In animation mode 1, when it fires, D2Common's #10407 looks in the unit's room at its position for a unit
//  SKILLS_StatesMatchVineBeastNeutralMissile accepts (a vine beast neutral missile) and MISSILE_ClientOnHit explodes it (no target, not
//  forced); the missile is cltmissiled. In mode 2 one roll of the unit's seed adds its low bit to cltmissilec (vine beast walk 1, then
//  the next row). Any other mode answers 1.
//  When it fires and the missile is above 0, the counter goes back to 0, Param2 and Param3 take the unit's position, and the missile is
//  made there (MISSILE_ClientSpawnSimple, Y before X, with the skill's own id and level); a missile made is turned to the unit's
//  direction, its rotate flag cleared, and it is set dead at once (CUNIT_SetUnitDead). The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetYPosition, UNITS_GetXPosition and SEED_RollRandomNumber here.
//  BUG: the direction goes out as a byte stored over the nSkillLevel slot and the whole dword is pushed, so the upper bytes are the skill
//  level's. INERT: PATH_SetDirection reads the byte alone.
int32_t __fastcall SKILLS_CltDoFunc89(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;
    int32_t nVineSkillId;
    int32_t nVineSkillLevel;
    int32_t nCounter;
    int32_t nLastX;
    int32_t nLastY;
    D2SkillsTxt *pSkillsTxt;
    BOOL bAtLimit;
    int32_t nRange;
    BOOL bFire;
    int32_t nMissileId;
    int32_t nUnitX;
    int32_t nUnitY;
    D2UnitStrc *pNeutral;
    D2UnitStrc *pMissile;
    D2DynamicPathStrc *pPath;
    uint8_t nDirection;

    if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER)
        return 0;

    pSkill = D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, SKILL_VINEATTACK);
    if (!pSkill)
    {
        D2COMMON_SKILLS_AssignSkill(pUnit, SKILL_VINEATTACK, 1, FALSE, szSkillsEMonCpp, 860);
        pSkill = D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, SKILL_VINEATTACK);
        if (!pSkill)
            return 0;
    }

    nVineSkillId = D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillsEMonCpp, 868);
    nVineSkillLevel = D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill, TRUE);
    nCounter = D2COMMON_SKILLS_GetParam1(pSkill);
    D2COMMON_SKILLS_SetParam1(pSkill, nCounter + 1);
    nLastX = D2COMMON_SKILLS_GetParam2(pSkill);
    nLastY = D2COMMON_SKILLS_GetParam3(pSkill);
    bAtLimit = FALSE;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nVineSkillId);
    if (!pSkillsTxt)
        return 0;

    if (nCounter >= D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[1], (D2C_Skills)nVineSkillId, nVineSkillLevel))
        bAtLimit = TRUE;

    nRange = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[1], (D2C_Skills)nVineSkillId, nVineSkillLevel);
    bFire = TRUE;
    if ((int32_t)D2COMMON_UNITS_GetDistanceToCoordinates(pUnit, nLastX, nLastY) < nRange)
        bFire = bAtLimit;

    if (pUnit->dwAnimMode == 1)
    {
        if (bFire)
        {
            nUnitY = UNITS_GetYPosition(pUnit);

            nUnitX = UNITS_GetXPosition(pUnit);

            pNeutral = D2COMMON_10407(D2COMMON_UNITS_GetRoom(pUnit), nUnitX, nUnitY, SKILLS_StatesMatchVineBeastNeutralMissile, NULL, 1);
            if (pNeutral)
                MISSILE_ClientOnHit(pNeutral, NULL, 0);
        }

        nMissileId = (int16_t)pSkillsTxt->wCltMissileD;
    }
    else if (pUnit->dwAnimMode == 2)
    {
        nMissileId = (int16_t)pSkillsTxt->wCltMissileC + (int32_t)((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) & 1);
    }
    else
    {
        return 1;
    }

    if (!bFire || nMissileId <= 0)
        return 1;

    D2COMMON_SKILLS_SetParam1(pSkill, 0);

    D2COMMON_SKILLS_SetParam2(pSkill, UNITS_GetXPosition(pUnit));

    D2COMMON_SKILLS_SetParam3(pSkill, UNITS_GetYPosition(pUnit));

    nUnitY = UNITS_GetYPosition(pUnit);

    nUnitX = UNITS_GetXPosition(pUnit);

    pMissile = MISSILE_ClientSpawnSimple(pUnit, nMissileId, nUnitX, nUnitY, nVineSkillId, nVineSkillLevel);
    if (pMissile)
    {
        nDirection = D2COMMON_UNITS_GetDirection(pUnit);
        pPath = pMissile->pDynamicPath;
        D2COMMON_PATH_SetDirection(pPath, nDirection);
        D2COMMON_10203_PATH_SetRotateFlag(pPath, FALSE);
        CUNIT_SetUnitDead(pMissile);
    }

    return 1;
}


//1.10f: D2Client.0x6FAC4BC0
//  A match callback for D2Common's #10407, which SKILLS_CltDoFunc89 hands its address; nothing else calls it. A unit that is a missile of
//  class vine beast neutral answers 1, anything else 0, NULL included. It reads only its first argument.
int32_t __fastcall SKILLS_StatesMatchVineBeastNeutralMissile(D2UnitStrc *pUnit, void *pUnused)
{
    if (!pUnit || pUnit->dwUnitType != UNIT_MISSILE)
        return 0;

    return pUnit->dwClassId == MISSILE_VINE_BEAST_NEUTRAL;
}


//1.10f: D2Client.0x6FAC4BE0
//  The cyclers' progressive (cltprgfunc1 93): CorpseCycler's and VineCycler's. The record, and cltmissileb (recycler delay) inside [0,
//  missiles.txt's count) (tested as a word first, then widened; the same sign either way), or the answer is 0; the unit's path target
//  unit, or 0. The target bleeds three drops (MONSTER_SpawnBlood, count 3, not by chance), and cltmissileb is made from a cleared
//  D2MissileStrc with the target as both owner and origin, the skill and its level, flags 0 and no point: it starts where the target is.
//  The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc93(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pTarget;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    if ((int16_t)pSkillsTxt->wCltMissileB < 0)
        return 0;

    if ((int16_t)pSkillsTxt->wCltMissileB >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget)
        return 0;

    MONSTER_SpawnBlood(pTarget, 3, 0);
    memset(&tParams, 0, sizeof(tParams));
    tParams.nMissile = (int16_t)pSkillsTxt->wCltMissileB;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nSkill = nSkillId;
    //  BUG: the client's missile is owned by the corpse (the target) where the server's is owned by the vine's master. INERT: by design; the
    //  client's own tick, MISSILE_CltDoFunc51, hides its owner, the corpse, and hands it on.
    tParams.pOwner = pTarget;
    tParams.pOrigin = pTarget;
    MISSILE_ClientSpawn(&tParams);
    return 1;
}


//1.10f: D2Client.0x6FAC4CA0
//  A wall across the aim: slot 80 of gpSkillCltDoFuncs, which no stock row names in any column that indexes the table. The record,
//  SKILLS_GetClientMissileForSkill's missile in range, and the point (SKILLS_GetPathTargetPos), or 0. calc1 (the server's column, +138h)
//  is the count. The vector from the unit to the point is grown to a minimum length and then shrunk perpendicular to itself
//  (SKILLS_GrowVectorToMinLength, SKILLS_ShrinkPerpendicularVector), and the point moved back along it by half the count (signed). calc2
//  (+13Ch) is the missiles' range. A cleared D2MissileStrc takes flags 20h, the unit as owner and origin, the skill, its level, the
//  missile and the range; then count missiles are made, each aimed at the point and the point stepped along the vector, and each missile
//  made gets its total frames as SKILLS_SetMissileAnimSpeedDivisor's divisor. Last, a unit in STATE_INFERNO is put back to animation
//  frame 8 with a frame count of 600h. The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltDoFunc80(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    int32_t nPointX;
    int32_t nPointY;
    int32_t nCount;
    int32_t nDirX;
    int32_t nDirY;
    int32_t nRange;
    D2UnitStrc *pSpawned;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: SKILLS_GetPathTargetPos answers 1 on every path, so this refusal never fires. UNREACHABLE: the test is dead.
    if (!SKILLS_GetPathTargetPos(pUnit, &nPointX, &nPointY))
        return 0;

    nCount = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);

    nDirX = nPointX - UNITS_GetXPosition(pUnit);

    nDirY = nPointY - UNITS_GetYPosition(pUnit);

    SKILLS_GrowVectorToMinLength(&nDirX, &nDirY);
    SKILLS_ShrinkPerpendicularVector(&nDirX, &nDirY);
    nPointX -= nCount * nDirX / 2;
    nPointY -= nCount * nDirY / 2;
    nRange = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[1], nSkillId, nSkillLevel);

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = nMissileId;
    tParams.nRange = nRange;
    if (nCount > 0)
    {
        do
        {
            tParams.nTargetY = nPointY;
            tParams.nTargetX = nPointX;
            pSpawned = MISSILE_ClientSpawn(&tParams);
            if (pSpawned)
                SKILLS_SetMissileAnimSpeedDivisor(pSpawned, D2COMMON_MISSILE_GetTotalFrames(pSpawned));

            nPointX += nDirX;
            nPointY += nDirY;
        }
        while (--nCount);
    }

    if (D2COMMON_STATES_CheckState(pUnit, STATE_INFERNO))
    {
        D2COMMON_UNITS_SetAnimationFrame(pUnit, 8);
        pUnit->dwFrameCountPrecise = 0x600;
    }

    return 1;
}


//1.10f: D2Client.0x6FAC4EC0
//  Baal's cold missiles (cltdofunc 81). The record, SKILLS_GetClientMissileForSkill's missile in range (baal cold maker), and the point,
//  or 0; SKILLS_MissileClientSpawnAimedAtTargetUnit makes the missile, and none made answers 0. Its target is then set to the
//  unit-to-point vector turned a quarter, as cltdofunc 70 does: MISSILE_SetTargetX gets the unit's Y minus the point's and
//  MISSILE_SetTargetY the point's X minus the unit's. The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltDoFunc81(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    int32_t nMissileId;
    int32_t nPointX;
    int32_t nPointY;
    D2UnitStrc *pMissile;
    int32_t nDeltaX;

    if (!DATATBLS_GetSkillsTxtRecordById(nSkillId))
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: SKILLS_GetPathTargetPos answers 1 on every path, so this refusal never fires. UNREACHABLE: the test is dead.
    if (!SKILLS_GetPathTargetPos(pUnit, &nPointX, &nPointY))
        return 0;

    pMissile = SKILLS_MissileClientSpawnAimedAtTargetUnit(pUnit, (D2C_MissilesHcIdx)nMissileId, nSkillId, nSkillLevel);
    if (!pMissile)
        return 0;

    nDeltaX = nPointX - UNITS_GetXPosition(pUnit);

    D2COMMON_MISSILE_SetTargetX(pMissile, UNITS_GetYPosition(pUnit) - nPointY);
    D2COMMON_MISSILE_SetTargetY(pMissile, nDeltaX);
    return 1;
}


//1.10f: D2Client.0x6FAC4FE0
//  Baal's corpse explosion, drawn: slot 82 of gpSkillCltDoFuncs. It is built for one row: it reads Skills.txt row 285, Baal Corpse
//  Explode, by constant, whatever skill it is handed, and takes the radius as that row's Param5 plus the level minus one times Param6 for
//  a level above 0, else 0 (40 at every level in stock data). Every unit D2Common's UNITFINDS_ search finds within it, flags 3002h around
//  the unit's position, gets a baalcorpseexplodedelay (MISSILE_BAALCORPSEEXPLODEDELAY) at its own position, Y before X, with the skill
//  and the level it was handed. A missile made has its total and current frames set to its own total plus a roll of the caster's seed,
//  unsigned % 20, minus 10 (-10 to +9 frames on its own life; the current frame counts down). The find data is freed, and the answer is
//  1.
//  BUG: no stock row reaches this slot: row 285's own client column is cltstfunc 82, which indexes gpSkillCltStFuncs, whose count is 70,
//  and SKILLS_RunCltStFunc calls a start function only below that count (for row 285 it calls none and goes on as after a start that
//  answered 1). UNREACHABLE: this slot of gpSkillCltDoFuncs is reached by no stock row.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollRandomNumber here.
int32_t __fastcall SKILLS_CltDoFunc82(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nSize;
    int32_t nUnitX;
    int32_t nUnitY;
    int32_t i;
    int32_t nFoundX;
    int32_t nFoundY;
    D2UnitStrc *pMissile;
    int32_t nFrames;
    D2UnitFindArgStrc tFindArg;
    D2UnitFindDataStrc tFindData;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(SKILL_BAALCORPSEEXPLODE);
    if (pSkillsTxt && nSkillLevel > 0)
        nSize = pSkillsTxt->dwParam[4] + (nSkillLevel - 1) * pSkillsTxt->dwParam[5];
    else
        nSize = 0;

    nUnitX = UNITS_GetXPosition(pUnit);

    nUnitY = UNITS_GetYPosition(pUnit);

    memset(&tFindArg, 0, sizeof(tFindArg));
    tFindArg.nFlags = 0x3002;
    tFindArg.pUnit = pUnit;
    tFindArg.nX = nUnitX;
    tFindArg.nY = nUnitY;
    tFindArg.nSize = nSize;
    D2COMMON_UNITFINDS_InitializeUnitFindData(NULL, &tFindData, D2COMMON_UNITS_GetRoom(pUnit), nUnitX, nUnitY, nSize, NULL, &tFindArg);
    tFindData.nFlags = tFindArg.nFlags;
    D2COMMON_UNITFINDS_FindAllMatchingUnitsInNeighboredRooms(&tFindData);

    for (i = 0; i < tFindData.nIndex; ++i)
    {
        nFoundY = UNITS_GetYPosition(tFindData.pUnitsArray[i]);

        nFoundX = UNITS_GetXPosition(tFindData.pUnitsArray[i]);

        pMissile = MISSILE_ClientSpawnSimple(pUnit, MISSILE_BAALCORPSEEXPLODEDELAY, nFoundX, nFoundY, nSkillId, nSkillLevel);
        if (pMissile)
        {
            nFrames = D2COMMON_MISSILE_GetTotalFrames(pMissile);

            nFrames += (int32_t)((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 20) - 10;
            D2COMMON_MISSILE_SetTotalFrames(pMissile, nFrames);
            D2COMMON_MISSILE_SetCurrentFrame(pMissile, nFrames);
        }
    }

    D2COMMON_UNITFINDS_FreeUnitFindData(&tFindData);
    return 1;
}

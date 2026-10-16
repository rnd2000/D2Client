#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "SkillsDru.h"

//  SkillsDru.cpp -- the Druid's client skill code of D2Client.dll 1.10: the shape-shift start function (cltstfunc 45), three do functions
//  (cltdofunc 67..69: Firestorm, Twister, Tornado) and the volley they share, the hurricane state's handler (States.txt cltactivefunc 90)
//  with its debris helper, and two cltprgfunc1 functions (91 Volcano, 92 Armageddon). Address range 0x6FAC2840..0x6FAC3304.
//  No function in this unit asserts, so the original's source file name is not known and the file name is ours.
//  Each start, do and progressive function is reached by one slot of gpSkillCltStFuncs (cltstfunc 45) or gpSkillCltDoFuncs (the rest), and
//  nothing else; the volley is reached from cltdofunc 67, 68 and 69 only. Skills served: cltstfunc 45 Wearwolf, Wearbear, Delerium Change;
//  cltdofunc 67 Firestorm, 68 Twister, 69 Tornado; cltprgfunc1 91 Volcano, 92 Armageddon and Diablogeddon; cltactivefunc 90 hurricane.
//  A skill id is a D2C_Skills and a missile id a D2C_MissilesHcIdx.

static_assert(offsetof(D2SkillsTxt, wCltOverlayA) == 0x110 && offsetof(D2SkillsTxt, wCltOverlayB) == 0x112,
              "SKILLS_CltStFunc45: `mov cx, [esi+110h]` (0x6FAC288C), `mov ax, [esi+112h]` (0x6FAC28A8)");


//1.10f: D2Client.0x6FAC2840
//  The start of Wearwolf, Wearbear and Delerium Change (cltstfunc 45). It reads nSkillLevel nowhere. The row's aurastate must lie in
//  [0, states.txt's count) and its cltoverlaya in [0, overlay.txt's count), or the answer is 0. The overlay is then cltoverlaya (wolf_into,
//  bear_into) unless cltoverlayb (wolf_undo, bear_undo) is in range too and the unit already has the state, when it is cltoverlayb;
//  cltoverlayb's range is tested before the state, so an out-of-range one never asks for the state. It is added with GFX_AddOverlay kind 2
//  and every other argument 0, and the answer is 1. All three columns are signed words against signed compares on the counts, hence the
//  int16_t casts. The original inlines DATATBLS_GetSkillsTxtRecordById here.
//  BUG: Delerium Change (row 350) carries neither overlay column, so this start answers 0 for it at the cltoverlaya test, and a start that
//  answers 0 makes SKILLS_RunCltStFunc clear the unit's used skill and, for a player or a monster, set mode 1. LATENT: stock 1.10 gives the
//  skill only as the Delirium runeword's gethit-skill, an item cast whose row does not ask for the start.
BOOL __fastcall SKILLS_CltStFunc45(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nState;
    int32_t nOverlayId;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return FALSE;

    if (pSkillsTxt->nAuraState < 0)
        return FALSE;

    nState = pSkillsTxt->nAuraState;
    if (nState >= sgptDataTables->nStatesTxtRecordCount)
        return FALSE;

    if ((int16_t)pSkillsTxt->wCltOverlayA < 0)
        return FALSE;

    nOverlayId = (int16_t)pSkillsTxt->wCltOverlayA;
    if (nOverlayId >= sgptDataTables->nOverlayTxtRecordCount)
        return FALSE;

    //  BUG: the choice of overlay tests the one state, where the server undoes the shape when the unit is in any state of the aura state's
    //  States.txt group (wolf, bear and delerium share group 3 with maul, feralrage and monsterset). So a werewolf casting Wearbear is turned
    //  back by the server while this function, finding no bear state, plays bear_into. REACHABLE: the start runs for every player starting the skill.
    if ((int16_t)pSkillsTxt->wCltOverlayB >= 0 && (int16_t)pSkillsTxt->wCltOverlayB < sgptDataTables->nOverlayTxtRecordCount && D2COMMON_STATES_CheckState(pUnit, (D2C_States)nState))
    {
        nOverlayId = (int16_t)pSkillsTxt->wCltOverlayB;
    }

    GFX_AddOverlay(pUnit, (D2_OverlayIds)nOverlayId, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);
    return TRUE;
}


//1.10f: D2Client.0x6FAC2900
//  A volley of seeded missiles, pUnit in ecx and nCount in edx, six stack arguments (retn 18h). Called by cltdofunc 67, 68 and 69 only:
//      caller               nAnimSpeed  bFirstAimedAtTargetUnit  bRandomFacing
//      SKILLS_CltDoFunc67   200h (512)  1                        0   Firestorm
//      SKILLS_CltDoFunc68   100h (256)  0                        1   Twister
//      SKILLS_CltDoFunc69   200h (512)  0                        1   Tornado
//  It asks SKILLS_GetPathTargetPos for the point and answers 0 when that answers 0 (it never does, below). With bFirstAimedAtTargetUnit one
//  of the count is spent first, on SKILLS_MissileClientSpawnAimedAtTargetUnit. The rest share one D2MissileStrc, cleared whole: flags 21h
//  (bit 0, start at nX/nY; bit 5, the target an absolute point), the unit as owner, its own position, the point, the skill, its level, the
//  missile, and SKILLS_MissileStartSeededPath as the init callback with the missile's index as its argument, so each seeded path differs.
//  bRandomFacing ORs 2000h into the flags, which makes MISSILE_ClientSpawn step the missile's seed and face its path seed & 3Fh; it also
//  clears the path's rotate flag on every missile the volley makes, the aimed one included. Every missile made gets nAnimSpeed. The loop's
//  count test and its back edge are signed, so a count the aimed missile took to 0 makes nothing more. The answer is 1. The original inlines
//  UNITS_GetXPosition and UNITS_GetYPosition here.
//  BUG: the volley fires at a point the server's volley refuses: the server's target-position helper answers 0 when either coordinate is 0
//  and no volley is fired, where the client's SKILLS_GetPathTargetPos falls back to the step in front of the unit and always answers 1.
//  NOT ESTABLISHED: it needs a point on world coordinate 0.
int32_t __fastcall SKILLS_SpawnSeededMissileVolley(D2UnitStrc *pUnit, int32_t nCount, D2C_MissilesHcIdx nMissileId, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nAnimSpeed, int32_t bFirstAimedAtTargetUnit, int32_t bRandomFacing)
{
    int32_t nTargetX;
    int32_t nTargetY;
    D2UnitStrc *pAimedMissile;
    D2UnitStrc *pSpawned;
    int32_t nSpawned;
    D2MissileStrc tParams;

    //  BUG: dead test: SKILLS_GetPathTargetPos answers 1 on every path. UNREACHABLE: the volley always goes out.
    if (!SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY))
        return 0;

    if (bFirstAimedAtTargetUnit)
    {
        --nCount;
        pAimedMissile = SKILLS_MissileClientSpawnAimedAtTargetUnit(pUnit, nMissileId, nSkillId, nSkillLevel);
        if (pAimedMissile)
        {
            if (bRandomFacing)
                D2COMMON_10203_PATH_SetRotateFlag(pAimedMissile->pDynamicPath, FALSE);

            D2COMMON_UNITS_SetAnimationSpeed(pAimedMissile, nAnimSpeed);
        }
    }

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pUnit;

    tParams.nX = UNITS_GetXPosition(pUnit);

    tParams.nY = UNITS_GetYPosition(pUnit);

    tParams.nSkill = nSkillId;
    tParams.nTargetX = nTargetX;
    tParams.nTargetY = nTargetY;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = nMissileId;
    tParams.pInitFunc = SKILLS_MissileStartSeededPath;
    if (bRandomFacing)
        tParams.dwFlags |= MISSILESPAWN_RANDOMDIR;

    for (nSpawned = 0; nSpawned < nCount; ++nSpawned)
    {
        tParams.pInitArgs = nSpawned;
        pSpawned = MISSILE_ClientSpawn(&tParams);
        if (pSpawned)
        {
            if (bRandomFacing)
                D2COMMON_10203_PATH_SetRotateFlag(pSpawned->pDynamicPath, FALSE);

            D2COMMON_UNITS_SetAnimationSpeed(pSpawned, nAnimSpeed);
        }
    }

    return 1;
}


//1.10f: D2Client.0x6FAC2A80
//  Firestorm (cltdofunc 67). It answers 0 without the skill's record, or when SKILLS_GetClientMissileForSkill's missile (cltmissilea,
//  firestormmaker) lies outside [0, missiles.txt's count). Then it sets UNITFLAG_SKSRVDOFUNC on a non-NULL unit, evaluates calc1 (Firestorm's
//  ln12) as the count, answers 0 for a count of 0 or less, and otherwise answers what SKILLS_SpawnSeededMissileVolley answers for the count
//  (always 1): animation speed 200h, the first missile aimed at the target unit, no random facing. The flag is set before the formula runs,
//  so it lands even when the count comes out 0. Twister and Tornado are this body with three constants changed, as separate functions in the
//  original. The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc67(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    int32_t nCount;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: the NULL test guards only the flag; pUnit then goes untested to D2Common's #11276 and to the volley, whose SKILLS_GetPathTargetPos
    //  reads pUnit->pDynamicPath. UNREACHABLE: only SKILLS_RunClientDoFunc reaches slots 67..69 and no caller can pass NULL.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    nCount = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);
    if (nCount <= 0)
        return 0;

    return SKILLS_SpawnSeededMissileVolley(pUnit, nCount, (D2C_MissilesHcIdx)nMissileId, nSkillId, nSkillLevel, 0x200, 1, 0);
}


//1.10f: D2Client.0x6FAC2B30
//  Twister (cltdofunc 68): cltdofunc 67's body instruction for instruction, the three pushes aside: animation speed 100h, no aimed missile,
//  random facing. Its cltmissilea is twister and its calc1 is par1 (3). The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc68(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    int32_t nCount;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: the NULL test guards only the flag, as in SKILLS_CltDoFunc67. UNREACHABLE: the same.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    nCount = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);
    if (nCount <= 0)
        return 0;

    return SKILLS_SpawnSeededMissileVolley(pUnit, nCount, (D2C_MissilesHcIdx)nMissileId, nSkillId, nSkillLevel, 0x100, 0, 1);
}


//1.10f: D2Client.0x6FAC2BE0
//  Tornado (cltdofunc 69): cltdofunc 67's body instruction for instruction, the three pushes aside: animation speed 200h, no aimed missile,
//  random facing -- Twister at twice the animation speed. Its cltmissilea is tornado and its calc1 is 1. The original inlines
//  DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc69(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    int32_t nCount;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: the NULL test guards only the flag, as in SKILLS_CltDoFunc67. UNREACHABLE: the same.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    nCount = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);
    if (nCount <= 0)
        return 0;

    return SKILLS_SpawnSeededMissileVolley(pUnit, nCount, (D2C_MissilesHcIdx)nMissileId, nSkillId, nSkillLevel, 0x200, 0, 1);
}


//1.10f: D2Client.0x6FAC2C90
//  The hurricane state's handler (States.txt cltactivefunc 90; no Skills.txt client column holds 90): the table's three-argument __fastcall
//  shape, run by SKILLS_StatesRunActiveStateFuncs with the skill and level of the state's own stat list.
//  A unit with no room, or in a town room, counts as in town. With a prgsound above 0 (a signed word; Hurricane's is druid_hurricane) the
//  sound of that group already playing on the unit is looked up (UNITSND_FindPlayingSoundInGroup): out of town, none playing starts it
//  (SOUNDINST_PlaySound); in town, one playing is detached from the unit with bForce 1 (SOUNDINST_DetachFromUnit). In town the answer is
//  then 0 and nothing is drawn. Out of town cltcalc1 is evaluated (Hurricane's 3) and, for each unit of it,
//  SKILLS_StatesSpawnHurricaneCltMissile runs three times (the count kept in nSkillLevel's stack slot). The answer is 1, also for a count of
//  0 or less. The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc90(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2ActiveRoomStrc *pRoom;
    BOOL bInTown;
    int32_t hPlayingSound;
    int32_t nBurstsLeft;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    pRoom = D2COMMON_UNITS_GetRoom(pUnit);
    bInTown = !pRoom || D2COMMON_DUNGEON_IsRoomInTown(pRoom);

    if ((int16_t)pSkillsTxt->wPrgSound > 0)
    {
        hPlayingSound = UNITSND_FindPlayingSoundInGroup(pUnit, (D2C_Sounds)(int16_t)pSkillsTxt->wPrgSound);
        if (!bInTown)
        {
            if (!hPlayingSound)
                SOUNDINST_PlaySound((D2C_Sounds)(int16_t)pSkillsTxt->wPrgSound, pUnit, 0, 0, 0);
        }
        else if (hPlayingSound)
        {
            SOUNDINST_DetachFromUnit(hPlayingSound, pUnit, 1);
        }
    }

    if (bInTown)
        return 0;

    nBurstsLeft = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[0], nSkillId, nSkillLevel);
    if (nBurstsLeft > 0)
    {
        //  BUG: three debris per unit of cltcalc1, where the row's own description column calls that value "debris/frame" (nine for the stock 3).
        //  INERT: the description column is not loaded, and this loop is the value's one reader.
        do
        {
            SKILLS_StatesSpawnHurricaneCltMissile(pUnit, pSkillsTxt, nSkillId, nSkillLevel);
            SKILLS_StatesSpawnHurricaneCltMissile(pUnit, pSkillsTxt, nSkillId, nSkillLevel);
            SKILLS_StatesSpawnHurricaneCltMissile(pUnit, pSkillsTxt, nSkillId, nSkillLevel);
        } while (--nBurstsLeft);
    }

    return 1;
}


//1.10f: D2Client.0x6FAC2D90
//  One piece of Hurricane debris, pUnit in ecx and pSkillsTxt in edx, retn 8 for nSkillId and nSkillLevel; its one caller is
//  SKILLS_CltDoFunc90, three calls.
//  Where: a roll of the unit's own seed (a call to SEED_RollRandomNumber, masked with 7) picks one of the eight points of gnDir8CircleX and
//  gnDir8CircleY, an octagon of radius 7 round the unit's position. Which missile: cltmissilea (Hurricane's hurricaneswoosh), and nothing at
//  all when it is negative; with cltmissileb (hurricanerock) set and cltmissilec (hurricanetree) not, a second roll's low bit keeps A (1) or
//  takes B (0); with both set, a roll below 3 takes B (0), C (1) or keeps A (2). The three words are tested signed: A after its movsx, B and
//  C in place.
//  How: the record is cleared and given flags 3 -- bit 0, start at nX/nY, and bit 1, the target an offset added to the start -- with the
//  octagon point as the start and (offsetY, -offsetX) as the offset: the radius turned a quarter turn, so the debris sets off along the
//  circle, not away from it. Then, if a missile was made: a height, a roll below cltcalc2 (Hurricane's 75, "height" in the row's description
//  column) and 0 for a formula of 0 or less, through PARTICLE_SetUnitGfxCoords(pDebris, 0, 0, height); and the unit's own position as the
//  missile's target (D2Common's MISSILE_SetTargetX and Y).
//  The low bit's roll is also stored into a stack slot nothing reads; the C has no local for it. The original inlines UNITS_GetXPosition and
//  UNITS_GetYPosition (all four uses) and SEED_RollLimitedRandomNumber (the bound 3 and the debris height).
void __fastcall SKILLS_StatesSpawnHurricaneCltMissile(D2UnitStrc *pUnit, D2SkillsTxt *pSkillsTxt, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SeedStrc *pSeed;
    int32_t nDirIdx;
    int32_t nUnitX;
    int32_t nUnitY;
    int32_t nOffsetX;
    int32_t nOffsetY;
    int32_t nSpawnX;
    int32_t nSpawnY;
    int32_t nCltMissileId;
    D2UnitStrc *pDebris;
    int32_t nMaxHeight;
    int32_t nGfxZ;
    D2MissileStrc tParams;

    pSeed = &pUnit->pSeed;

    //  a call, as the original has it; SEED_RollLimitedRandomNumber with 8 would give the same bits
    //  BUG: every piece rolls the unit's own seed up to three times (the point always, the pick when cltmissileb is set, the height when a
    //  missile was made and its formula is above 0), a seed no server function advances for Hurricane. INERT: nothing keeps the two copies of
    //  the seed equal, and no server function draws the debris.
    nDirIdx = (int32_t)((uint32_t)SEED_RollRandomNumber(pSeed) & 7);

    nUnitX = UNITS_GetXPosition(pUnit);

    nUnitY = UNITS_GetYPosition(pUnit);

    nOffsetX = gnDir8CircleX[nDirIdx];
    nOffsetY = gnDir8CircleY[nDirIdx];
    nSpawnY = nUnitY + nOffsetY;
    nSpawnX = nUnitX + nOffsetX;

    nCltMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (nCltMissileId < 0)
        return;

    if ((int16_t)pSkillsTxt->wCltMissileB >= 0)
    {
        if ((int16_t)pSkillsTxt->wCltMissileC >= 0)
        {
            switch (SEED_RollLimitedRandomNumber(pSeed, 3))
            {
            case 0:
                nCltMissileId = (int16_t)pSkillsTxt->wCltMissileB;
                break;
            case 1:
                nCltMissileId = (int16_t)pSkillsTxt->wCltMissileC;
                break;
            }
        }
        else if (!((uint32_t)SEED_RollRandomNumber(pSeed) & 1))
        {
            //  a call again, as for the point above
            nCltMissileId = (int16_t)pSkillsTxt->wCltMissileB;
        }
    }

    memset(&tParams, 0, sizeof(tParams));
    tParams.nMissile = nCltMissileId;
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET;
    tParams.nTargetX = nOffsetY;
    tParams.pOwner = pUnit;
    tParams.nX = nSpawnX;
    tParams.nY = nSpawnY;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nTargetY = -nOffsetX;
    //  BUG: the record's skill is not set: of its members only the missile, the flags, the offset, the owner, the start and the level are
    //  stored, so tParams.nSkill is the clear's 0, where the volley and cltprgfunc1 91 and 92 store theirs. INERT: Hurricane's stock debris
    //  rows name no client function to read it.
    pDebris = MISSILE_ClientSpawn(&tParams);
    if (!pDebris)
        return;

    nMaxHeight = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[1], nSkillId, nSkillLevel);

    nGfxZ = SEED_RollLimitedRandomNumber(pSeed, nMaxHeight);
    PARTICLE_SetUnitGfxCoords(pDebris, 0, 0, nGfxZ);

    D2COMMON_MISSILE_SetTargetX(pDebris, UNITS_GetXPosition(pUnit));

    D2COMMON_MISSILE_SetTargetY(pDebris, UNITS_GetYPosition(pUnit));
}


//1.10f: D2Client.0x6FAC3010
//  Armageddon's falling rock (cltprgfunc1 92; Armageddon's and the classless Diablogeddon's). It is run by packet 0xA3, through
//  SKILLS_ApplyClientSkillEffectNonProgressive: the server sends the caster, the skill, a point and a seed byte, and the handler makes the
//  point the caster's path target (D2Common's #10170, which also clears the path's target unit) and the byte its dwInitSeed for the length
//  of the call. So SKILLS_GetPathTargetPos answers the server's point: with no target unit it reads the path's tTargetCoord. For this
//  skill the server sends it from the armageddon state's srvactivefunc, with no target and a seed of 0.
//  Four formulas, each refusing with 0 when it comes out 0 or less: aurarangecalc (Armageddon's par3, 8), read and dropped -- a gate here,
//  the server's range -- and then cltcalc1..3, which Skills.txt's own description columns call frames, fall rate and slide rate (Armageddon's
//  25, 25 and 15). Frames times fall rate and frames times slide rate are the start's height and its X offset. The record is cleared and given
//  flags 1 (start at nX/nY), the unit as owner, the point, the skill and its level. cltmissilea (armageddontail) and then cltmissileb
//  (armageddonrock), each skipped when negative, are spawned from it, and each one made is displaced to (X offset, 0, height) by
//  PARTICLE_SetUnitGfxCoords and given the vector (-slide rate, 0, -fall rate), bScale 1, by PARTICLE_SetUnitGfxVector -- so a vector
//  stepped once a frame would bring it to the point in cltcalc1 frames; what bScale does is not read here. Neither id is bounded against
//  missiles.txt's count: MISSILE_ClientSpawn refuses one outside it. The answer is 1. The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc92(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nFrames;
    int32_t nFallRate;
    int32_t nSlideRate;
    int32_t nStartZ;
    int32_t nStartX;
    int32_t nX;
    int32_t nY;
    D2UnitStrc *pMissile;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    if (D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwAuraRangeCalc, nSkillId, nSkillLevel) <= 0)
        return 0;

    nFrames = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[0], nSkillId, nSkillLevel);
    if (nFrames <= 0)
        return 0;

    nFallRate = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[1], nSkillId, nSkillLevel);
    if (nFallRate <= 0)
        return 0;

    nSlideRate = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[2], nSkillId, nSkillLevel);
    if (nSlideRate <= 0)
        return 0;

    nStartZ = nFallRate * nFrames;
    nStartX = nSlideRate * nFrames;

    //  BUG: dead test: SKILLS_GetPathTargetPos answers 1 on every path. UNREACHABLE: the callee never answers 0, as in the volley.
    if (!SKILLS_GetPathTargetPos(pUnit, &nX, &nY))
        return 0;

    memset(&tParams, 0, sizeof(tParams));
    tParams.pOwner = pUnit;
    tParams.dwFlags = MISSILESPAWN_STARTPOS;
    tParams.nX = nX;
    tParams.nY = nY;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;

    if ((int16_t)pSkillsTxt->wCltMissileA >= 0)
    {
        tParams.nMissile = (int16_t)pSkillsTxt->wCltMissileA;
        pMissile = MISSILE_ClientSpawn(&tParams);
        if (pMissile)
        {
            PARTICLE_SetUnitGfxCoords(pMissile, nStartX, 0, nStartZ);
            PARTICLE_SetUnitGfxVector(pMissile, -nSlideRate, 0, -nFallRate, 1);
        }
    }

    if ((int16_t)pSkillsTxt->wCltMissileB >= 0)
    {
        tParams.nMissile = (int16_t)pSkillsTxt->wCltMissileB;
        pMissile = MISSILE_ClientSpawn(&tParams);
        if (pMissile)
        {
            PARTICLE_SetUnitGfxCoords(pMissile, nStartX, 0, nStartZ);
            PARTICLE_SetUnitGfxVector(pMissile, -nSlideRate, 0, -nFallRate, 1);
        }
    }

    return 1;
}


//1.10f: D2Client.0x6FAC3200
//  Volcano (cltprgfunc1 91), run by packet 0xA3 like SKILLS_CltDoFunc92. The server makes its srvmissilea (volcano) at the point it aims at,
//  gives it the low byte of a roll of the caster's seed as its target X and sends the point with that byte as the seed, which 0xA3's handler
//  puts into the caster's dwInitSeed for this call.
//  The point is SKILLS_GetPathTargetPos' and the record is cleared and given flags 1, the unit as owner, the point, the skill and its level.
//  cltmissilea (volcano overlay fire) and then cltmissileb (volcano) are spawned from it, each only when in [0, missiles.txt's count) -- here
//  the bounds are tested -- and the second, when made, gets the caster's dwInitSeed as its target X (D2Common's MISSILE_SetTargetX): the
//  server's byte, so the client's volcano and the server's carry the same one. The answer is 1. The original inlines
//  DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc91(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nX;
    int32_t nY;
    int32_t nCltMissileId;
    D2UnitStrc *pMissile;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    //  BUG: dead test: SKILLS_GetPathTargetPos answers 1 on every path. UNREACHABLE: the callee never answers 0, as in the volley.
    if (!SKILLS_GetPathTargetPos(pUnit, &nX, &nY))
        return 0;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS;
    tParams.pOwner = pUnit;
    tParams.nX = nX;
    tParams.nY = nY;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;

    nCltMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (nCltMissileId >= 0 && nCltMissileId < sgptDataTables->nMissilesTxtRecordCount)
    {
        tParams.nMissile = nCltMissileId;
        MISSILE_ClientSpawn(&tParams);
    }

    nCltMissileId = (int16_t)pSkillsTxt->wCltMissileB;
    if (nCltMissileId >= 0 && nCltMissileId < sgptDataTables->nMissilesTxtRecordCount)
    {
        tParams.nMissile = nCltMissileId;
        pMissile = MISSILE_ClientSpawn(&tParams);
        if (pMissile)
            D2COMMON_MISSILE_SetTargetX(pMissile, (int32_t)pUnit->dwInitSeed);
    }

    return 1;
}

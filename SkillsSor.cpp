#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"

//  SkillsSor.cpp -- the client skill code of the sorceress: Charged Bolt's seeded bolts, Inferno's stream and its start, a
//  Telekinesis-shaped start no row names, the novas, Fire Wall, Chain Lightning and its item effect, Meteor's and Blizzard's centres,
//  Hydra, Frozen Orb, Blaze's trail and Thunder Storm's charge, with the classless and the druid's rows that borrow the same slots, and
//  the States.txt handlers of Chilling Armor (clteventfunc 1) and Shiver Armor (cltactivefunc 87).
//  No string in the image names this unit's source file. Address range 0x6FAC9E80..0x6FACB661, 18 functions.
//  Every start and do function is reached by one table slot and nothing else: the cltstfunc slots from gpSkillCltStFuncs, the cltdofunc,
//  cltprgfunc, ItemCltEffect and cltactivefunc slots from gpSkillCltDoFuncs, the clteventfunc slot from gpStateCltEventFuncs.
static_assert(STATE_INFERNO == 12, "SKILLS_CltStFunc15 and SKILLS_SpawnInfernoStream: push 0Ch (0x6FACA083, 0x6FACA358)");
static_assert(offsetof(D2MissileStrc, nRange) == 0x4C,
              "SKILLS_SpawnInfernoStream: the lifetime at +4Ch (0x6FACA2FA); MISSILE_ClientSpawn reads [edx+4Ch] (0x6FB3E688)");


//1.10f: D2Client.0x6FAC9E80
//  Starts a missile on Charged Bolt's seeded path: MISSILE_ClientSpawn calls it as the block's pInitFunc with the block's pInitArgs,
//  which every caller sets to the missile's index in its volley. Nothing for a NULL missile. The missile's total frames
//  (MISSILE_GetTotalFrames) are compared as a signed int: at 78 or above, 77 becomes the total and the current frame. The unit's seed
//  gets the low half nSeed plus the path's first point X (D2Common #10175, SEED_InitLowSeed), so bolts of one volley take different,
//  reproducible paths. Then PATHTYPE_CHARGEDBOLT, the frame count as the new distance (a byte parameter), and PATH_StartUnitMove with
//  bAllowInTown 0.
//  The missile's path is handed on untested: MISSILE_ClientSpawn removes a new missile that has none before it calls pInitFunc.
void __fastcall SKILLS_MissileStartSeededPath(D2UnitStrc *pMissile, int32_t nSeed)
{
    int32_t nFrames;
    D2DynamicPathStrc *pPath;

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
    D2COMMON_SEED_InitLowSeed(&pMissile->pSeed, nSeed + D2COMMON_10175_PathGetFirstPointX(pPath));
    D2COMMON_PATH_SetType(pPath, PATHTYPE_CHARGEDBOLT);
    D2COMMON_PATH_SetNewDistance(pPath, (uint8_t)nFrames);
    D2COMMON_PATH_StartUnitMove(pPath, pMissile, 0);
}


//1.10f: D2Client.0x6FAC9EE0
//  Charged Bolt (cltdofunc 23): Charged Bolt, PrimeBolt, BoltSentry, ImpBolt. The record, and SKILLS_GetClientMissileForSkill's missile
//  (the row's cltmissilea; none of the four rows is progressive), a signed value inside [0, missiles.txt's count), or the answer is 0.
//  UNITFLAG_SKSRVDOFUNC on a non-NULL unit. The bolt count is calc1 (#11276 on dwCalc[0]). The block, cleared, takes flags 21h, the unit
//  as owner and no origin, the unit's client position as the start, the skill, the level, the missile and SKILLS_MissileStartSeededPath
//  as pInitFunc. SKILLS_GetPathTargetPos writes the point straight into the block's target and its answer is tested. Then calc1 bolts are
//  spawned by MISSILE_ClientSpawn, each with its index as pInitArgs (its seed); none for a count not above 0 (signed). The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
//  BUG: the server's twin sets UNITFLAG_SKSRVDOFUNC before the record, uses the progressive missile, and takes the target inside the loop
//  for every bolt, skipping a bolt whose target X or Y is 0, where the client takes it once before the loop and spawns every bolt.
//  NOT ESTABLISHED: whether the difference is observable.
int32_t __fastcall SKILLS_CltDoFunc23(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    int32_t nMissileCount;
    int32_t iSpawn;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: this test guards only the flag; pUnit has already gone to SKILLS_GetClientMissileForSkill and goes on untested to #11276 and the
    //  type read. UNREACHABLE: pUnit is never NULL here.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    nMissileCount = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pUnit;

    tParams.nX = UNITS_GetXPosition(pUnit);

    tParams.nY = UNITS_GetYPosition(pUnit);

    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = nMissileId;
    tParams.pInitFunc = SKILLS_MissileStartSeededPath;

    //  BUG: SKILLS_GetPathTargetPos answers 1 on every path, so this refusal never fires. UNREACHABLE: the test is dead.
    if (!SKILLS_GetPathTargetPos(pUnit, &tParams.nTargetX, &tParams.nTargetY))
        return 0;

    for (iSpawn = 0; iSpawn < nMissileCount; ++iSpawn)
    {
        tParams.pInitArgs = iSpawn;
        MISSILE_ClientSpawn(&tParams);
    }

    return 1;
}


//1.10f: D2Client.0x6FACA050
//  Can Inferno start (cltstfunc 15): Inferno's and Arctic Blast's start; never reads nSkillLevel. The shape of SKILLS_StartChannelState
//  (Blade Fury's) without its skill lookup. 0 without the skill's record. Then:
//  When the unit is not in STATE_INFERNO, the channel's start cues: the row's prgoverlay through GFX_AddOverlay, kind 2, when it is in
//  (0, overlay.txt's count), and its prgsound through SOUNDINST_PlaySound with no unit, when it is above 0; both words signed.
//  When the unit already holds a STATE_INFERNO stat list, the channel goes on: the animation frame is set to 8, dwFrameCountPrecise to
//  500h, the list's expire frame to 7 (D2Common #10476), and the answer is 1.
//  Otherwise it begins: a stat list is allocated (filter 2, expire frame 15h, the unit's type and id, or UNIT_TYPES_COUNT and -1 for no
//  unit), posted to the unit with ITEM_OnUninterruptableStateRemoved as its remove callback and STATE_INFERNO as its state, the state is
//  toggled on, and the answer is 1. A failed allocation answers 0, the NULL itself.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
//  BUG: the server's twin refuses unless the unit is already in STATE_INFERNO or has the mana for the row's startmana, gives a MONSTER's
//  used skill its Param1, times the channel by the game frame, hangs another remove callback and plays no cue. NOT ESTABLISHED: whether
//  the difference is observable.
int32_t __fastcall SKILLS_CltStFunc15(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2StatListStrc *pInfernoStats;
    D2StatListStrc *pAllocStats;
    D2C_UnitTypes dwUnitType;
    D2UnitGUID dwUnitId;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    if (!D2COMMON_STATES_CheckState(pUnit, STATE_INFERNO))
    {
        if ((int16_t)pSkillsTxt->wPrgOverlay > 0 && (int16_t)pSkillsTxt->wPrgOverlay < sgptDataTables->nOverlayTxtRecordCount)
            GFX_AddOverlay(pUnit, (D2_OverlayIds)(int16_t)pSkillsTxt->wPrgOverlay, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);

        if ((int16_t)pSkillsTxt->wPrgSound > 0)
            SOUNDINST_PlaySound((D2C_Sounds)(int16_t)pSkillsTxt->wPrgSound, NULL, 0, 0, 0);
    }

    pInfernoStats = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, STATE_INFERNO);
    if (pInfernoStats)
    {
        D2COMMON_UNITS_SetAnimationFrame(pUnit, 8);
        pUnit->dwFrameCountPrecise = 0x500;
        D2COMMON_10476(pInfernoStats, 7);
        return 1;
    }

    //  BUG: this arms the no-unit type and id (UNIT_TYPES_COUNT, -1) after pUnit has gone to STATES_CheckState and
    //  STATLIST_GetStatListFromUnitAndState, and SKILLS_RunCltStFunc cannot pass NULL. UNREACHABLE: pUnit is never NULL here.
    if (pUnit)
    {
        dwUnitId = pUnit->dwUnitId;
        dwUnitType = (D2C_UnitTypes)pUnit->dwUnitType;
    }
    else
    {
        dwUnitId = (D2UnitGUID)-1;
        dwUnitType = UNIT_TYPES_COUNT;
    }

    pAllocStats = D2COMMON_STATLIST_AllocStatList(NULL, 2u, 0x15u, dwUnitType, dwUnitId);
    if (!pAllocStats)
        return 0;

    D2COMMON_10475_PostStatToStatList(pUnit, pAllocStats, TRUE);
    D2COMMON_STATLIST_SetStatRemoveCallback(pAllocStats, ITEM_OnUninterruptableStateRemoved);
    D2COMMON_STATLIST_SetState(pAllocStats, STATE_INFERNO);
    D2COMMON_STATES_ToggleState(pUnit, STATE_INFERNO, TRUE);
    return 1;
}


//1.10f: D2Client.0x6FACA170
//  Sets a missile's animation speed to one pass over nDivisor frames: nothing unless pUnit is a missile and nDivisor is not 0; then
//  UNITS_SetAnimationSpeed gets missiles.txt's animlen in 1/256 units (the byte zero-extended into the second byte) divided by nDivisor,
//  signed. Called with the missile's total frames.
void __fastcall SKILLS_SetMissileAnimSpeedDivisor(D2UnitStrc *pUnit, int32_t nDivisor)
{
    D2MissilesTxt *pMissilesTxt;

    if (!pUnit || pUnit->dwUnitType != UNIT_MISSILE || !nDivisor)
        return;

    if (pUnit->dwClassId < 0 || pUnit->dwClassId >= sgptDataTables->nMissilesTxtRecordCount)
        return;

    pMissilesTxt = &sgptDataTables->pMissilesTxt[pUnit->dwClassId];
    if (!pMissilesTxt)
        return;

    D2COMMON_UNITS_SetAnimationSpeed(pUnit, (pMissilesTxt->nAnimLen << 8) / nDivisor);
}


//1.10f: D2Client.0x6FACA1D0
//  Inferno's stream; answers the missile (NULL included). Called by SKILLS_CltDoFunc24 (Inferno, Arctic Blast) and SKILLS_CltDoFunc77
//  (the inferno sentry).
//  The record; cltmissilea (infernoflame1, arcticblast1), a signed word inside [0, missiles.txt's count), or the answer is NULL; and
//  cltmissileb (infernoflame2, arcticblast2) when it too is inside (the count loaded once), else -1. SKILLS_GetPathTargetPos gives the
//  point and its answer is tested. With a cltmissileb, a seed roll's low bit picks it over cltmissilea; the bit is also kept in a frame
//  slot nothing reads. The lifetime is calc1, at least 1.
//  The block, cleared, takes flags 8020h (20h the point is the target; 8000h the lifetime in nRange replaces the one missiles.txt gives),
//  the unit as owner and origin, the skill, the level, the missile and the lifetime, spawned by MISSILE_ClientSpawn. A spawned missile
//  gets the animation speed that makes one pass span its lifetime, as SKILLS_SetMissileAnimSpeedDivisor computes it (written out here,
//  not called).
//  When the caster is in STATE_INFERNO, its own animation is held: with an animation sequence, frame 8 and dwFrameCountPrecise 500h, as
//  the start's kept channel; without one, nSeqCurrentFramePrecise loses its fraction and two whole frames.
//  The original inlines DATATBLS_GetSkillsTxtRecordById and SEED_RollRandomNumber here.
//  BUG: the server's twin spawns the stream only when the used skill's Param1 is not 0 (so the first do of a channel spawns nothing) and
//  then sets it to 1, has no second missile and no animation speed, and for a MONSTER writes nSeqCurrentFramePrecise B00h and schedules
//  the end of the animation, where the client holds the caster's pose as above. NOT ESTABLISHED: whether the difference is observable.
D2UnitStrc *__fastcall SKILLS_SpawnInfernoStream(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pStream;
    int32_t nMissileId;
    int32_t nMissileB;
    int32_t nCoinFlip;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nLifetimeFrames;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return NULL;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return NULL;

    nMissileB = (int16_t)pSkillsTxt->wCltMissileB;
    if (nMissileB < 0 || nMissileB >= sgptDataTables->nMissilesTxtRecordCount)
        nMissileB = -1;

    //  BUG: SKILLS_GetPathTargetPos answers 1 on every path, so this refusal never fires. UNREACHABLE: the test is dead.
    if (!SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY))
        return NULL;

    if (nMissileB >= 0)
    {
        nCoinFlip = (int32_t)((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) & 1);
        if (nCoinFlip)
            nMissileId = nMissileB;
    }

    nLifetimeFrames = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);
    if (nLifetimeFrames < 1)
        nLifetimeFrames = 1;

    memset(&tParams, 0, sizeof(tParams));
    tParams.nMissile = nMissileId;
    tParams.nTargetX = nTargetX;
    tParams.dwFlags = MISSILESPAWN_TARGETPOS | MISSILESPAWN_RANGE;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nTargetY = nTargetY;
    tParams.nRange = nLifetimeFrames;
    pStream = MISSILE_ClientSpawn(&tParams);

    //  the original tests the lifetime for 0; it is at least 1, so this cannot fire
    if (pStream && pStream->dwUnitType == UNIT_MISSILE && nLifetimeFrames != 0)
    {
        if (pStream->dwClassId >= 0 && pStream->dwClassId < sgptDataTables->nMissilesTxtRecordCount)
        {
            pMissilesTxt = &sgptDataTables->pMissilesTxt[pStream->dwClassId];
            if (pMissilesTxt)
                D2COMMON_UNITS_SetAnimationSpeed(pStream, (pMissilesTxt->nAnimLen << 8) / nLifetimeFrames);
        }
    }

    if (D2COMMON_STATES_CheckState(pUnit, STATE_INFERNO))
    {
        if (pUnit->pAnimSeq)
        {
            D2COMMON_UNITS_SetAnimationFrame(pUnit, 8);
            pUnit->dwFrameCountPrecise = 0x500;
            return pStream;
        }

        //  taken unsigned: the bits the original computes, with no signed overflow in C
        pUnit->nSeqCurrentFramePrecise = (int32_t)(((uint32_t)pUnit->nSeqCurrentFramePrecise & 0xFFFFFF00u) - 0x200u);
    }

    return pStream;
}


//1.10f: D2Client.0x6FACA3B0
//  Inferno (cltdofunc 24): Inferno's and Arctic Blast's do. Calls SKILLS_SpawnInfernoStream with its arguments passed through and answers
//  0 or 1.
BOOL __fastcall SKILLS_CltDoFunc24(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    return SKILLS_SpawnInfernoStream(pUnit, nSkillId, nSkillLevel) != NULL;
}


//1.10f: D2Client.0x6FACA3D0
//  Can Telekinesis start (cltstfunc 16). The body is the client's side of Telekinesis's server start.
//  The record, and the path's target unit (CUNIT_GetPathTargetUnit), or the answer is 0. The range is aurarangecalc (#11276 on
//  dwAuraRangeCalc), squared (a product taken unsigned, the bits the original computes and no signed overflow in C). The target's client
//  Y and X, then the unit's, go into PATH_ComputeSquaredDistance; above the squared range (a signed compare) the answer is 0.
//  A target whose type is a player or a monster (0 or 1, tested signed) must stand with the unit outside town: the unit's room, then the
//  target's, each must exist (UNITS_GetRoom) and not be in town (DUNGEON_IsRoomInTown), and CUNIT_IsUnitSelectableTarget must accept the
//  target; any failure answers 0. The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetYPosition and UNITS_GetXPosition here.
//  BUG: no stock row dispatches this slot (Telekinesis's row leaves cltstfunc empty, and no row's cltstfunc is 16). UNREACHABLE:
//  SKILLS_RunCltStFunc never reaches it in stock 1.10 data.
int32_t __fastcall SKILLS_CltStFunc16(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pTarget;
    D2ActiveRoomStrc *pRoom;
    int32_t nRange;
    int32_t nRangeSq;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nCasterX;
    int32_t nCasterY;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget)
        return 0;

    nRange = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwAuraRangeCalc, nSkillId, nSkillLevel);
    nRangeSq = (int32_t)((uint32_t)nRange * (uint32_t)nRange);

    nTargetY = UNITS_GetYPosition(pTarget);

    nTargetX = UNITS_GetXPosition(pTarget);

    nCasterY = UNITS_GetYPosition(pUnit);

    nCasterX = UNITS_GetXPosition(pUnit);

    if (D2COMMON_PATH_ComputeSquaredDistance(nCasterX, nCasterY, nTargetX, nTargetY) > nRangeSq)
        return 0;

    if ((int32_t)pTarget->dwUnitType >= 0 && (int32_t)pTarget->dwUnitType <= UNIT_MONSTER)
    {
        pRoom = D2COMMON_UNITS_GetRoom(pUnit);
        if (!pRoom || D2COMMON_DUNGEON_IsRoomInTown(pRoom))
            return 0;

        pRoom = D2COMMON_UNITS_GetRoom(pTarget);
        if (!pRoom || D2COMMON_DUNGEON_IsRoomInTown(pRoom))
            return 0;

        if (!CUNIT_IsUnitSelectableTarget(pUnit, pTarget))
            return 0;
    }

    return 1;
}


//1.10f: D2Client.0x6FACA570
//  A nova from the caster (cltdofunc 25): Frost Nova, Nova, Poison Nova, Howl, Shout, Battle Cry, Battle Orders, War Cry, Battle Command,
//  PrimeIceNova, DiabFire, Baal Nova, MephFrostNova, Trap Nova. The record, and SKILLS_GetClientMissileForSkill (the row's cltmissilea;
//  none of the rows is progressive), a signed value inside [0, missiles.txt's count), or the answer is 0. UNITFLAG_SKSRVDOFUNC on a
//  non-NULL unit. The velocity is missiles.txt's for the level (DATATBLS_GetMissileVelocityFromMissilesTxt) plus cltcalc1 (#11276 on
//  dwCltCalc[0], +114h): Howl's par1 * (lvl-1), the other rows' empty column. SKILLS_SpawnMissileRingFromUnit sends the ring out with the
//  unit as owner and as centre, no second missile (-1), the skill and the level. The answer is 1.
//  SKILLS_CltDoFunc10 does the same ring from the path target at missiles.txt's velocity alone, for Nova's and Frost Nova's
//  ItemCltEffect.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc25(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    int32_t nBaseVelocity;
    int32_t nSpeedBonus;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: this test guards only the flag; pUnit has already gone to SKILLS_GetClientMissileForSkill and goes on untested to #11276 and the
    //  ring. UNREACHABLE: pUnit is never NULL here.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    nBaseVelocity = D2COMMON_DATATBLS_GetMissileVelocityFromMissilesTxt(nMissileId, nSkillLevel);
    nSpeedBonus = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[0], nSkillId, nSkillLevel);
    SKILLS_SpawnMissileRingFromUnit(pUnit, pUnit, (D2C_MissilesHcIdx)nMissileId, (D2C_MissilesHcIdx)-1, nSkillId, nSkillLevel, nBaseVelocity + nSpeedBonus);
    return 1;
}


//1.10f: D2Client.0x6FACA630
//  Fire Wall (cltdofunc 26): Fire Wall, VampireFirewall, PrimeFirewall, CountessFirewall. It sets no UNITFLAG_SKSRVDOFUNC.
//  The record; cltmissilea (firewallmaker, vampirefirewallmaker, countessfirewallmaker), a signed word inside [0, missiles.txt's count),
//  with its missiles.txt row; and that row's SubMissile1 (firewall, vampirefirewall, countessfirewall), a signed word inside the same
//  range. Any of them missing answers 0. SKILLS_GetPathTargetPos gives the point and its answer is tested.
//  SKILLS_CanMissileLandAtAimPoint (whether the maker can land at the aim point) must pass, or the answer is 0.
//  The caster's room (UNITS_GetRoom), or 0; then the room holding the point, the caster's or an adjacent one, or NULL. Without the row's
//  INTOWN flag, that room in town answers 0.
//  The block, cleared, takes flags 21h (the maker starts at the point and heads for a target), the owner, the skill, the level and the
//  maker. With dX and dY the caster's client position less the point, two makers go out perpendicular to the caster's line, one toward
//  (point X - dY, point Y + dX) and one toward (point X + dY, point Y - dX); then the sub-missile, with flags 1 and no target, at the
//  point itself. MISSILE_ClientSpawn makes all three; its answers are not read. The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, CLIENT_FindRoomContainingSubtile, UNITS_GetXPosition and UNITS_GetYPosition
//  here.
int32_t __fastcall SKILLS_CltDoFunc26(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2MissilesTxt *pMissilesTxt;
    D2ActiveRoomStrc *pRoom;
    int32_t nMissileA;
    int32_t nSubMissile;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nDeltaX;
    int32_t nDeltaY;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileA = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileA < 0 || nMissileA >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileA];
    if (!pMissilesTxt)
        return 0;

    nSubMissile = (int16_t)pMissilesTxt->wSubMissile[0];
    if (nSubMissile < 0 || nSubMissile >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: SKILLS_GetPathTargetPos answers 1 on every path, so this refusal never fires. UNREACHABLE: the test is dead.
    if (!SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY))
        return 0;

    if (!SKILLS_CanMissileLandAtAimPoint(pUnit, (D2C_MissilesHcIdx)nMissileA))
        return 0;

    pRoom = D2COMMON_UNITS_GetRoom(pUnit);
    if (!pRoom)
        return 0;

    pRoom = CLIENT_FindRoomContainingSubtile(pRoom, nTargetX, nTargetY);

    //  BUG: a point in no room the search finds reaches DUNGEON_IsRoomInTown as NULL, and D2Common answers 0 for NULL, so the wall is cast
    //  there as out of town (SKILLS_CltStFunc17 refuses the same NULL). INERT: the server does the same.
    if (!(((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_INTOWN >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_INTOWN & 7]) && D2COMMON_DUNGEON_IsRoomInTown(pRoom))
        return 0;

    memset(&tParams, 0, sizeof(tParams));
    tParams.nY = nTargetY;
    tParams.nSkillLevel = nSkillLevel;
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pUnit;
    tParams.nX = nTargetX;
    tParams.nSkill = nSkillId;
    tParams.nMissile = nMissileA;

    nDeltaX = UNITS_GetXPosition(pUnit) - nTargetX;

    nDeltaY = UNITS_GetYPosition(pUnit) - nTargetY;

    tParams.nTargetY = nTargetY + nDeltaX;
    tParams.nTargetX = nTargetX - nDeltaY;
    MISSILE_ClientSpawn(&tParams);

    tParams.nTargetX = nTargetX + nDeltaY;
    tParams.nTargetY = nTargetY - nDeltaX;
    MISSILE_ClientSpawn(&tParams);

    tParams.nMissile = nSubMissile;
    tParams.nTargetX = 0;
    tParams.nTargetY = 0;
    tParams.dwFlags = MISSILESPAWN_STARTPOS;
    MISSILE_ClientSpawn(&tParams);
    return 1;
}


//1.10f: D2Client.0x6FACA900
//  Chain Lightning (cltdofunc 27). The record, and SKILLS_GetClientMissileForSkill's missile (chainlightning), a signed value inside [0,
//  missiles.txt's count), or the answer is 0. UNITFLAG_SKSRVDOFUNC on a non-NULL unit. calc1 (#11276 on dwCalc[0]) is kept.
//  SKILLS_MissileClientSpawnAimedAtTargetUnit makes the bolt; none answers 0. Its target X gets calc1 (MISSILE_SetTargetX): a count, not
//  a coordinate. The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc27(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pMissile;
    int32_t nMissileId;
    int32_t nCalcValue;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: this test guards only the flag; pUnit has already gone to SKILLS_GetClientMissileForSkill and goes on untested to #11276 and the
    //  spawn. UNREACHABLE: pUnit is never NULL here.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    nCalcValue = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);
    pMissile = SKILLS_MissileClientSpawnAimedAtTargetUnit(pUnit, (D2C_MissilesHcIdx)nMissileId, nSkillId, nSkillLevel);
    if (!pMissile)
        return 0;

    D2COMMON_MISSILE_SetTargetX(pMissile, nCalcValue);
    return 1;
}


//1.10f: D2Client.0x6FACA9C0
//  Chain Lightning from an item: slot 96 of gpSkillCltDoFuncs, Chain Lightning's ItemCltEffect (no other column names 96);
//  SKILLS_RunClientDoFunc runs it in place of cltdofunc 27 for an item's cast with a target.
//  The record and the missile, as cltdofunc 27; UNITFLAG_SKSRVDOFUNC on a non-NULL unit; calc1 is kept. With a path target unit
//  (CUNIT_GetPathTargetUnit), the offset from the caster to it (its client X less the caster's, its Y less the caster's); without one, 0
//  and 0. The block, cleared, takes flags 21h and the caster as owner; the start is the caster's client position plus the offset (the
//  target's own position when there is one), the skill, the missile and the level. SKILLS_GetPathTargetPos writes the point into the
//  block's target; its answer is tested, and a point with X or Y 0 answers 0. MISSILE_ClientSpawn makes the bolt; none answers 0. Its
//  target X gets calc1, as in cltdofunc 27. The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltDoFunc96(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pTarget;
    D2UnitStrc *pMissile;
    int32_t nMissileId;
    int32_t nCalcValue;
    int32_t nOffsetX;
    int32_t nOffsetY;
    int32_t nTargetPos;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: this test guards only the flag; pUnit has already gone to SKILLS_GetClientMissileForSkill and goes on untested to #11276,
    //  CUNIT_GetPathTargetUnit and the position reads. UNREACHABLE: pUnit is never NULL here.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    nCalcValue = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);

    nOffsetX = 0;
    nOffsetY = 0;
    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (pTarget)
    {
        nTargetPos = UNITS_GetXPosition(pTarget);

        nOffsetX = nTargetPos - UNITS_GetXPosition(pUnit);

        nTargetPos = UNITS_GetYPosition(pTarget);

        nOffsetY = nTargetPos - UNITS_GetYPosition(pUnit);
    }

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pUnit;

    tParams.nX = UNITS_GetXPosition(pUnit) + nOffsetX;

    tParams.nY = UNITS_GetYPosition(pUnit) + nOffsetY;

    tParams.nSkill = nSkillId;
    tParams.nMissile = nMissileId;
    tParams.nSkillLevel = nSkillLevel;

    //  BUG: SKILLS_GetPathTargetPos answers 1 on every path, so this refusal never fires. UNREACHABLE: the test is dead.
    if (!SKILLS_GetPathTargetPos(pUnit, &tParams.nTargetX, &tParams.nTargetY))
        return 0;

    if (!tParams.nTargetX || !tParams.nTargetY)
        return 0;

    pMissile = MISSILE_ClientSpawn(&tParams);
    if (!pMissile)
        return 0;

    D2COMMON_MISSILE_SetTargetX(pMissile, nCalcValue);
    return 1;
}


//1.10f: D2Client.0x6FACAC10
//  Meteor's and Blizzard's centre (cltdofunc 28): Meteor, Blizzard, VampireMeteor, MonBlizzard, Eruption. The record is only tested; the
//  missile is SKILLS_GetClientMissileForSkill's (meteorcenter, blizzardcenter, vampiremeteorcenter, monblizcenter, erruption center), a
//  signed value inside [0, missiles.txt's count); UNITFLAG_SKSRVDOFUNC on a non-NULL unit; SKILLS_CanMissileLandAtAimPoint must answer
//  TRUE; and the answer is whether SKILLS_MissileClientSpawnOnTargetUnit made the missile, made 0 or 1. Any refusal answers 0.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
BOOL __fastcall SKILLS_CltDoFunc28(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    int32_t nMissileId;

    if (!DATATBLS_GetSkillsTxtRecordById(nSkillId))
        return FALSE;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return FALSE;

    //  BUG: this test guards only the flag; pUnit has already gone to SKILLS_GetClientMissileForSkill and goes on untested to the launch test
    //  and the spawn. UNREACHABLE: pUnit is never NULL here.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    if (!SKILLS_CanMissileLandAtAimPoint(pUnit, (D2C_MissilesHcIdx)nMissileId))
        return FALSE;

    return SKILLS_MissileClientSpawnOnTargetUnit(pUnit, (D2C_MissilesHcIdx)nMissileId, nSkillId, nSkillLevel) != 0;
}


//1.10f: D2Client.0x6FACACA0
//  Can Hydra start (cltstfunc 17); never reads nSkillLevel. The caster's room (UNITS_GetRoom), or 0 (the NULL itself).
//  SKILLS_GetPathTargetPos gives the point and its answer is tested. The room holding the point, the caster's or an adjacent one, or 0.
//  Only then the record, or 0. The row's INTOWN flag answers 1; without it the room in town answers 0 and any other 1. Stock Hydra
//  carries no InTown, so a point in town refuses the cast.
//  The original inlines CLIENT_FindRoomContainingSubtile and DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltStFunc17(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2ActiveRoomStrc *pRoom;
    int32_t nTargetX;
    int32_t nTargetY;

    pRoom = D2COMMON_UNITS_GetRoom(pUnit);
    if (!pRoom)
        return 0;

    //  BUG: SKILLS_GetPathTargetPos answers 1 on every path, so this refusal never fires. UNREACHABLE: the test is dead.
    if (!SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY))
        return 0;

    pRoom = CLIENT_FindRoomContainingSubtile(pRoom, nTargetX, nTargetY);
    if (!pRoom)
        return 0;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    if (((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_INTOWN >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_INTOWN & 7])
        return 1;

    if (D2COMMON_DUNGEON_IsRoomInTown(pRoom))
        return 0;

    return 1;
}


//1.10f: D2Client.0x6FACADF0
//  Frozen Orb (cltdofunc 29). The record is only tested; the missile is SKILLS_GetClientMissileForSkill's (frozenorb), a signed value
//  inside [0, missiles.txt's count); UNITFLAG_SKSRVDOFUNC on a non-NULL unit, here on the whole dword where this unit's other functions
//  or a byte; and the answer is whether SKILLS_MissileClientSpawnAimedAtTargetUnit made the orb, made 0 or 1. Any refusal answers 0.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
BOOL __fastcall SKILLS_CltDoFunc29(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    int32_t nMissileId;

    if (!DATATBLS_GetSkillsTxtRecordById(nSkillId))
        return FALSE;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return FALSE;

    //  BUG: this test guards only the flag; pUnit has already gone to SKILLS_GetClientMissileForSkill and goes on untested to the spawn.
    //  UNREACHABLE: pUnit is never NULL here.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    return SKILLS_MissileClientSpawnAimedAtTargetUnit(pUnit, (D2C_MissilesHcIdx)nMissileId, nSkillId, nSkillLevel) != NULL;
}


static_assert(STATE_BLAZE == 13, "SKILLS_LayBlazeTrail: push 0Dh (0x6FACAE92)");


//1.10f: D2Client.0x6FACAE70
//  Blaze's trail: called by UNITMODE_StepUnitAlongPath for a unit in STATE_BLAZE; no row's. Nothing unless the unit has a dynamic path
//  and PATH_HasMovedThisFrame says it moved, and unless it holds a STATE_BLAZE stat list. The skill and the level come off that list
//  (STAT_MODIFIERLIST_SKILL and LEVEL). The skill must be above 0 and below skills.txt's count; a negative level becomes 1. A NULL record
//  ends it. cltmissilea (firewall for Blaze) is a signed word inside [0, missiles.txt's count), or nothing. Nothing when the unit's room
//  is in town (UNITS_GetRoom into DUNGEON_IsRoomInTown, the room untested: a NULL one is out of town there).
//  The block, cleared, takes flags 21h (the unit's client position as the start, and a target point flag 20h names but nothing writes,
//  the clear's (0, 0)), the owner, the skill, the level and cltmissilea. Blaze's three flames have no Vel, so the target is never used.
//  With cltmissileb signed-negative the flame is cltmissilea. With cltmissilec negative, one seed roll's low bit picks A on 1 and B on 0.
//  With both, the roll's low half, unsigned, mod 3 picks B on 0, C on 1 and A on 2. Blaze's are firewall, firesmall and firemedium.
//  MISSILE_ClientSpawn makes the flame; its answer is not read.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollRandomNumber here.
void __fastcall SKILLS_LayBlazeTrail(D2UnitStrc *pUnit)
{
    D2StatListStrc *pStateStats;
    D2SkillsTxt *pSkillsTxt;
    int32_t nSkillId;
    int32_t nSkillLevel;
    int32_t nUnitX;
    int32_t nUnitY;
    int32_t nPick;
    D2MissileStrc tParams;

    if (!pUnit->pDynamicPath || !D2COMMON_PATH_HasMovedThisFrame(pUnit->pDynamicPath))
        return;

    pStateStats = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, STATE_BLAZE);
    if (!pStateStats)
        return;

    nSkillId = D2COMMON_STATLIST_GetStatValue(pStateStats, STAT_MODIFIERLIST_SKILL, 0);
    nSkillLevel = D2COMMON_STATLIST_GetStatValue(pStateStats, STAT_MODIFIERLIST_LEVEL, 0);

    //  the skill id is tested in place: 0 is refused (DATATBLS_GetSkillsTxtRecordById would accept it), and a negative level is clamped to 1
    //  between the bound test and the stride
    if (nSkillId <= 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount)
        return;

    if (nSkillLevel < 0)
        nSkillLevel = 1;

    pSkillsTxt = &sgptDataTables->pSkillsTxt[nSkillId];
    if (!pSkillsTxt)
        return;

    if ((int16_t)pSkillsTxt->wCltMissileA < 0 || (int16_t)pSkillsTxt->wCltMissileA >= sgptDataTables->nMissilesTxtRecordCount)
        return;

    if (D2COMMON_DUNGEON_IsRoomInTown(D2COMMON_UNITS_GetRoom(pUnit)))
        return;

    nUnitX = UNITS_GetXPosition(pUnit);

    nUnitY = UNITS_GetYPosition(pUnit);

    memset(&tParams, 0, sizeof(tParams));
    tParams.nY = nUnitY;
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pUnit;
    tParams.nX = nUnitX;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = (int16_t)pSkillsTxt->wCltMissileA;

    //  BUG: cltmissileb and cltmissilec are tested for their sign alone, with no bound against missiles.txt's count where cltmissilea has
    //  one, before they go to MISSILE_ClientSpawn. LATENT: not reached with stock data.
    if ((int16_t)pSkillsTxt->wCltMissileB >= 0)
    {
        if ((int16_t)pSkillsTxt->wCltMissileC < 0)
        {
            nPick = (int32_t)((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) & 1);
            if (!nPick)
                tParams.nMissile = (int16_t)pSkillsTxt->wCltMissileB;
        }
        else
        {
            nPick = (int32_t)((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 3);
            if (nPick == 0)
            {
                tParams.nMissile = (int16_t)pSkillsTxt->wCltMissileB;
                MISSILE_ClientSpawn(&tParams);
                return;
            }

            if (nPick == 1)
                tParams.nMissile = (int16_t)pSkillsTxt->wCltMissileC;
        }
    }

    MISSILE_ClientSpawn(&tParams);
}


//1.10f: D2Client.0x6FACB070
//  Thunder Storm's strike (cltprgfunc1 84), Thunder Storm's and no other row's; SKILLS_ApplyClientSkillEffectNonProgressive runs a
//  non-progressive row's cltprgfunc1 when the server hands the client a skill effect on a target.
//  The record; cltmissilea (thunderstorm1), a signed word inside [0, missiles.txt's count); and the path's target unit
//  (CUNIT_GetPathTargetUnit). Any missing answers 0. The block, cleared, takes flags 1 (the bolt starts at the target's client position),
//  the unit as owner, the skill, the level and the missile, made by MISSILE_ClientSpawn. A bolt made is lifted 280 and falls 40 a frame,
//  seven frames to the ground (PARTICLE_SetUnitGfxCoords and PARTICLE_SetUnitGfxVector). Then ENV_TriggerLightningFlash with bPlayThunder
//  0, made or not. The row's prgoverlay (lightning) is added on the target when above 0, with no upper bound here (GFX_AddOverlay bounds
//  the id against overlay.txt's count itself). The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltDoFunc84(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pTarget;
    D2UnitStrc *pMissile;
    int32_t nMissileA;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileA = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileA < 0 || nMissileA >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget)
        return 0;

    //  BUG: this refuses a NULL pUnit after pUnit has gone to CUNIT_GetPathTargetUnit, which reads it. UNREACHABLE: pUnit is never NULL here.
    if (!pUnit)
        return 0;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS;
    tParams.pOwner = pUnit;

    tParams.nX = UNITS_GetXPosition(pTarget);

    tParams.nY = UNITS_GetYPosition(pTarget);

    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = nMissileA;
    pMissile = MISSILE_ClientSpawn(&tParams);
    if (pMissile)
    {
        PARTICLE_SetUnitGfxCoords(pMissile, 0, 0, 280);
        PARTICLE_SetUnitGfxVector(pMissile, 0, 0, -40, 1);
    }

    ENV_TriggerLightningFlash(0);

    if ((int16_t)pSkillsTxt->wPrgOverlay > 0)
        GFX_AddOverlay(pTarget, (D2_OverlayIds)(int16_t)pSkillsTxt->wPrgOverlay, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);

    return 1;
}


//  The chillingarmor handler tests the RETURNFIRE flag of missiles.txt: byte 1 of dwMissileFlags against gdwBitMasks[1].
static_assert(MISSILESFLAGINDEX_RETURNFIRE == 9,
              "SKILLS_StatesCltEventFunc1: byte 1 of dwMissileFlags against gdwBitMasks[1] (0x6FACB26E..0x6FACB276)");


//1.10f: D2Client.0x6FACB1F0
//  States.txt clteventfunc 1: slot 1 of gpStateCltEventFuncs, chillingarmor its one row (cltevent hitbymissile), the aura state of
//  Chilling Armor. nEvent in ecx, pUnit in edx, and on the stack pMissile, nUnused, nSkillId and nSkillLevel, as CUNITEVENT_FireEvent
//  calls every such handler. Chilling Armor's retaliation, drawn on the client:
//  Only event 0 (hitbymissile), with a unit and a missile; the missile's owner (CUNIT_GetOwnerUnit), which must be one
//  CUNIT_IsUnitSelectableTarget accepts from the unit. Anything else answers 0. The missile's missiles.txt row must carry RETURNFIRE, or
//  0. The record, and cltmissilea (chillingarmorbolt), a signed word inside [0, missiles.txt's count), or 0. The block, cleared, takes
//  flags 20h (the bolt starts at the origin, the unit, and flies at the owner's client position), the unit as owner and origin, the
//  skill, the level and the bolt, made by MISSILE_ClientSpawn (its answer not read). The row's cltoverlaya (chillarmor_hit) is added on
//  the unit through GFX_AddOverlay, kind 2, when it is inside [0, overlay.txt's count) (0 accepted). The answer is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_StatesCltEventFunc1(int32_t nEvent, D2UnitStrc *pUnit, D2UnitStrc *pMissile, int32_t nUnused, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2UnitStrc *pAttacker;
    D2MissilesTxt *pMissilesTxt;
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileA;
    D2MissileStrc tParams;

    if (nEvent)
        return 0;

    if (!pUnit)
        return 0;

    if (!pMissile)
        return 0;

    pAttacker = CUNIT_GetOwnerUnit(pMissile);
    if (!pAttacker || !CUNIT_IsUnitSelectableTarget(pUnit, pAttacker))
        return 0;

    if (pMissile->dwClassId < 0 || pMissile->dwClassId >= sgptDataTables->nMissilesTxtRecordCount)
        pMissilesTxt = NULL;
    else
        pMissilesTxt = &sgptDataTables->pMissilesTxt[pMissile->dwClassId];

    //  BUG: the RETURNFIRE test reads the row through NULL for a missile class outside the table, a fault at address 5. Reproduced. LATENT:
    //  not reached with stock data.
    if (!(((uint8_t *)&pMissilesTxt->dwMissileFlags)[MISSILESFLAGINDEX_RETURNFIRE >> 3] & (uint8_t)gdwBitMasks[MISSILESFLAGINDEX_RETURNFIRE & 7]))
        return 0;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileA = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileA < 0 || nMissileA >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    memset(&tParams, 0, sizeof(tParams));
    tParams.nSkill = nSkillId;
    tParams.dwFlags = MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = nMissileA;

    tParams.nTargetX = UNITS_GetXPosition(pAttacker);

    tParams.nTargetY = UNITS_GetYPosition(pAttacker);

    MISSILE_ClientSpawn(&tParams);

    if ((int16_t)pSkillsTxt->wCltOverlayA >= 0 && (int16_t)pSkillsTxt->wCltOverlayA < sgptDataTables->nOverlayTxtRecordCount)
        GFX_AddOverlay(pUnit, (D2_OverlayIds)(int16_t)pSkillsTxt->wCltOverlayA, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);

    return 1;
}


//1.10f: D2Client.0x6FACB3C0
//  States.txt cltactivefunc 87: shiverarmor its one row (no Skills.txt client column holds 87), the aura state of Shiver Armor; the
//  table's three-argument __fastcall shape, run by SKILLS_StatesRunActiveStateFuncs with the skill and level of the state's own stat
//  list. The armor's sparkles:
//  The record; the missile from SKILLS_GetClientMissileForSkill (sparkle, the row's cltmissilea, not progressive), a signed value inside
//  [0, missiles.txt's count); the unit's skill of this id (SKILLS_GetHighestLevelSkillFromUnitAndId); and cltcalc1, cltcalc2 and cltcalc3
//  (Shiver Armor's 10, 5 and 3), each above 0, signed. Anything missing answers 0. The skill's Param1 counts the calls: read, stored back
//  plus one, and unless the count before the step divides by cltcalc1 (signed) the answer is 1 with nothing drawn, so one sparkle every
//  tenth frame.
//  A point round the caster: its client X plus a roll below cltcalc2 * 2 + 1, less cltcalc2; then its Y the same way (within 5 either
//  side for Shiver Armor), each roll a limited seed roll (0 when the bound is not above 0). A third roll below cltcalc3, and
//  MISSILE_ClientSpawnSkillEffect with the unit, the missile, no spawn offset, the point as the target, the skill, the level and the roll
//  as its ninth argument, nGfxArg (the block's +24h, the arc its flag 100h draws, or 30 when it is 0). Its answer is not read. The answer
//  is 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition, UNITS_GetYPosition and SEED_RollLimitedRandomNumber here.
int32_t __fastcall SKILLS_CltDoFunc87(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2SkillStrc *pSkill;
    D2SeedStrc *pSeed;
    int32_t nMissileId;
    int32_t nPeriod;
    int32_t nSpread;
    int32_t nGfxRange;
    int32_t nCount;
    int32_t nRollRange;
    int32_t nCasterPos;
    int32_t nSpawnX;
    int32_t nSpawnY;
    int32_t nGfxArg;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = SKILLS_GetClientMissileForSkill(pUnit, nSkillId);
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    pSkill = D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, nSkillId);
    if (!pSkill)
        return 0;

    nPeriod = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[0], nSkillId, nSkillLevel);
    if (nPeriod <= 0)
        return 0;

    nSpread = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[1], nSkillId, nSkillLevel);
    if (nSpread <= 0)
        return 0;

    nGfxRange = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[2], nSkillId, nSkillLevel);
    if (nGfxRange <= 0)
        return 0;

    nCount = D2COMMON_SKILLS_GetParam1(pSkill);
    D2COMMON_SKILLS_SetParam1(pSkill, nCount + 1);
    if (nCount % nPeriod)
        return 1;

    //  BUG: the original tests the address of the seed, &pUnit->pSeed, for NULL; that can fire only for pUnit == (D2UnitStrc *)-32, and pUnit
    //  has been read by then. UNREACHABLE: pUnit is never NULL here.
    pSeed = &pUnit->pSeed;
    if (!pSeed)
        return 0;

    //  taken unsigned: the bits the original computes, with no signed overflow in C
    nRollRange = (int32_t)(2u * (uint32_t)nSpread + 1u);

    nCasterPos = UNITS_GetXPosition(pUnit);

    nSpawnX = SEED_RollLimitedRandomNumber(pSeed, nRollRange) - nSpread + nCasterPos;

    nCasterPos = UNITS_GetYPosition(pUnit);

    nSpawnY = SEED_RollLimitedRandomNumber(pSeed, nRollRange) - nSpread + nCasterPos;

    nGfxArg = SEED_RollLimitedRandomNumber(pSeed, nGfxRange);

    MISSILE_ClientSpawnSkillEffect(pUnit, nMissileId, 0, 0, nSpawnX, nSpawnY, nSkillId, nSkillLevel, nGfxArg);
    return 1;
}

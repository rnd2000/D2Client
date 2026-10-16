#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "SkillsMon.h"

//  SkillsMon.cpp -- the client skill code of the classic game's monsters, rewritten from D2Client.dll 1.10: the classless rows of Skills.txt from
//  before Lord of Destruction (the sand maggots, Andariel, the swarms, the nests, the submerging and resurrecting monsters, Diablo, the turrets and
//  towers, the mosquitoes, the regurgitators, the maggot queen) and the missile skills of the hirelings and the late monster casters.
//  Address range 0x6FAC51E0..0x6FAC7F85, 38 functions. The original's source file is Skills\SkillsMon.cpp (named by the path literal that
//  SKILLS_CltDoFunc59 passes).
//  Every start and do function is reached through one slot of the client dispatch tables (gpSkillCltStFuncs, gpSkillCltDoFuncs): SKILLS_CltStFuncN
//  is a skills.txt cltstfunc N and SKILLS_CltDoFuncN a cltdofunc N. The data tables it reads are in SkillsMon.h.

static_assert(COLLIDE_NO_PATH == 0x1000 && COLLIDE_MASK_PLAYER_PATH == 0x1C09,
              "SKILLS_CltStFunc32: `push 1000h` (0x6FAC5417), `push 1C09h` (0x6FAC5365)");
static_assert(COLLISION_PATTERN_SMALL_UNIT_PRESENCE == 1 && COLLISION_PATTERN_SMALL_NO_PRESENCE == 5,
              "SKILLS_CltStFunc32: `push 1` (0x6FAC541C); SKILLS_CltStFunc33: `push 5` (0x6FAC54B1)");


//1.10f: D2Client.0x6FAC51E0
//  MaggotEgg (cltdofunc 46): stops the egg's animation (dwSeqSpeed = 0) and returns 1. Reads neither nSkillId nor nSkillLevel.
int32_t __fastcall SKILLS_CltDoFunc46(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    pUnit->dwSeqSpeed = 0;
    return 1;
}


//1.10f: D2Client.0x6FAC51F0
//  MagottUp start (cltstfunc 32): the maggot surfaces. Returns 0 if the skills.txt record is missing. Sets UNITFLAG_TARGETABLE, _CANBEATTACKED and
//  _ISVALIDTARGET on the unit, spawns the dirt pile (cltmissilea; cltmissileb in Act II when that column is a valid missile id) at its position,
//  then moves the unit to a free coordinate (COLLISION_GetFreeCoordinates with COLLIDE_MASK_PLAYER_PATH, neighbour rooms allowed; returns 0 if there
//  is none or the move fails) and sets COLLIDE_NO_PATH (SMALL_UNIT_PRESENCE pattern) there. Returns 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltStFunc32(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    int32_t nUnitX;
    int32_t nUnitY;
    D2CoordStrc tSpawnPoint;
    int32_t nUnitSize;
    D2ActiveRoomStrc *pRoom;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    D2COMMON_UNITS_SetAnimStartFrame(pUnit);

    //  BUG: the NULL test of pUnit (0x6FAC5233) guards only the flags; pUnit goes on untested into the position accessors (0x6FAC529F). UNREACHABLE.
    //  BUG: the flags are set before the move and kept when the move fails and the start returns 0; the server sets them only after a successful
    //  move. NOT ESTABLISHED.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_TARGETABLE | UNITFLAG_CANBEATTACKED | UNITFLAG_ISVALIDTARGET;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileId > 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
    {
        if (D2COMMON_DRLG_GetActNoFromLevelId((D2C_Levels)D2COMMON_DUNGEON_GetLevelIdFromRoom(D2COMMON_UNITS_GetRoom(pUnit))) == 1 && (int16_t)pSkillsTxt->wCltMissileB > 0
            && (int16_t)pSkillsTxt->wCltMissileB < sgptDataTables->nMissilesTxtRecordCount)
        {
            nMissileId = (int16_t)pSkillsTxt->wCltMissileB;
        }

        nUnitY = UNITS_GetYPosition(pUnit);

        nUnitX = UNITS_GetXPosition(pUnit);

        MISSILE_ClientSpawnForOwner(pUnit, nMissileId, nUnitX, nUnitY, (int32_t)nSkillId, nSkillLevel);
    }

    tSpawnPoint.nX = UNITS_GetXPosition(pUnit);

    tSpawnPoint.nY = UNITS_GetYPosition(pUnit);

    nUnitSize = D2COMMON_UNITS_GetUnitSizeX(pUnit);
    pRoom = D2COMMON_COLLISION_GetFreeCoordinates(D2COMMON_UNITS_GetRoom(pUnit), &tSpawnPoint, nUnitSize, COLLIDE_MASK_PLAYER_PATH, TRUE);
    if (!pRoom)
        return 0;

    if (!pUnit->pDynamicPath)
        return 0;

    if (!D2COMMON_10231(pUnit->pDynamicPath, pUnit, pRoom, (uint16_t)tSpawnPoint.nX, (uint16_t)tSpawnPoint.nY))
        return 0;

    nUnitY = UNITS_GetYPosition(pUnit);

    nUnitX = UNITS_GetXPosition(pUnit);

    D2COMMON_COLLISION_SetMaskWithPattern(D2COMMON_UNITS_GetRoom(pUnit), nUnitX, nUnitY, COLLISION_PATTERN_SMALL_UNIT_PRESENCE, COLLIDE_NO_PATH);
    return 1;
}


//1.10f: D2Client.0x6FAC5440
//  MagottDown start (cltstfunc 33): the maggot burrows. Clears the three flags that cltstfunc 32 sets on a non-NULL unit, removes COLLIDE_NO_PATH
//  from the unit's position (SMALL_NO_PRESENCE pattern, the exact undo of cltstfunc 32) and returns 1. Reads neither nSkillId nor nSkillLevel.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltStFunc33(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    int32_t nUnitX;
    int32_t nUnitY;

    //  BUG: the NULL test of pUnit (0x6FAC5443) guards only the flags; pUnit goes on untested into the position accessors (0x6FAC5456). UNREACHABLE.
    if (pUnit)
        pUnit->dwFlags &= ~(UNITFLAG_TARGETABLE | UNITFLAG_CANBEATTACKED | UNITFLAG_ISVALIDTARGET);

    nUnitY = UNITS_GetYPosition(pUnit);

    nUnitX = UNITS_GetXPosition(pUnit);

    D2COMMON_COLLISION_ResetMaskWithPattern(D2COMMON_UNITS_GetRoom(pUnit), nUnitX, nUnitY, COLLISION_PATTERN_SMALL_NO_PRESENCE, COLLIDE_NO_PATH);
    return 1;
}


//1.10f: D2Client.0x6FAC54D0
//  Sinking (cltdofunc 47): MagottDown's do, and Submerge's through slot 52, which holds the same address. Calls UNITS_GetShiftedFrameMetrics, clears
//  the unit's nActionFrame byte and, with the frame number 0 or less (signed), stops its animation (dwSeqSpeed = 0). Returns 1.
int32_t __fastcall SKILLS_CltDoFunc47(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    int32_t nFrameNo;
    int32_t nFrameCount;

    D2COMMON_UNITS_GetShiftedFrameMetrics(pUnit, &nFrameNo, &nFrameCount);
    pUnit->nActionFrame = 0;
    if (nFrameNo <= 0)
        pUnit->dwSeqSpeed = 0;

    return 1;
}


//1.10f: D2Client.0x6FAC5510
//  AndrialSpray start (cltstfunc 34): takes the path target unit and the used skill (returns 0 if either is missing), stores the target's X and Y
//  as the skill's Param1 and Param2, the point cltdofunc 48 aims at, and returns 1. Reads neither nSkillId nor nSkillLevel.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltStFunc34(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2UnitStrc *pTarget;
    D2SkillStrc *pSkill;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget)
        return 0;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    D2COMMON_SKILLS_SetParam1(pSkill, UNITS_GetXPosition(pTarget));

    D2COMMON_SKILLS_SetParam2(pSkill, UNITS_GetYPosition(pTarget));
    return 1;
}


//1.10f: D2Client.0x6FAC55C0
//  AndrialSpray (cltdofunc 48): one missile per frame of the spray. Needs the skills.txt record, cltmissilea inside missiles.txt's range and the
//  used skill, else returns 0. The point is the skill's Param1/Param2 (set by cltstfunc 34), or SKILLS_GetPathTargetPos's when either is 0. The
//  direction to it (UNITS_GetDirectionToCoords, & 7) indexes gnCltEffect48Dirs; the missile's point steps from the unit's position once along that
//  code (D2Common #11055) and then along gnCltEffect48DirMatrix[9 * direction + step] unless that entry is 99; the step is the unit's shifted frame
//  number less 4, clamped to 0..8 (signed). MISSILE_ClientSpawnForOwner makes the missile toward that point; returns 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltDoFunc48(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    D2SkillStrc *pSkill;
    int32_t nPointX;
    int32_t nPointY;
    uint8_t nDirection;
    uint8_t nStepDir;
    int32_t nDeltaX;
    int32_t nDeltaY;
    int32_t nFrameNo;
    int32_t nFrameCount;
    int32_t nStep;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    nPointX = D2COMMON_SKILLS_GetParam1(pSkill);
    nPointY = D2COMMON_SKILLS_GetParam2(pSkill);
    if (!nPointX || !nPointY)
    {
        //  BUG: SKILLS_GetPathTargetPos's answer is tested (0x6FAC565C), but it answers 1 on every path, so the refusal is dead. UNREACHABLE.
        //  BUG: the server's fallback refuses a zero point where this one never does. NOT ESTABLISHED.
        if (!SKILLS_GetPathTargetPos(pUnit, &nPointX, &nPointY))
            return 0;
    }

    nDirection = (uint8_t)D2COMMON_UNITS_GetDirectionToCoords(pUnit, nPointX, nPointY);
    nDirection = D2COMMON_11053(nDirection) & 7;

    nPointX = UNITS_GetXPosition(pUnit);

    nPointY = UNITS_GetYPosition(pUnit);

    D2COMMON_11055(gnCltEffect48Dirs[nDirection], &nDeltaX, &nDeltaY);
    nPointX += nDeltaX;
    nPointY += nDeltaY;

    D2COMMON_UNITS_GetShiftedFrameMetrics(pUnit, &nFrameNo, &nFrameCount);
    nStep = nFrameNo - 4;
    if (nStep <= 0)
        nStep = 0;
    else if (nStep >= 8)
        nStep = 8;

    nStepDir = gnCltEffect48DirMatrix[9 * nDirection + nStep];
    if (nStepDir != 99)
    {
        D2COMMON_11055(nStepDir, &nDeltaX, &nDeltaY);
        nPointX += nDeltaX;
        nPointY += nDeltaY;
    }

    MISSILE_ClientSpawnForOwner(pUnit, nMissileId, nPointX, nPointY, (int32_t)nSkillId, nSkillLevel);
    return 1;
}


//1.10f: D2Client.0x6FAC57D0
//  Swarm Move start (cltstfunc 35): takes the path target unit and the used skill (returns 0 if either is missing or the unit has no dynamic path),
//  sets the path's new distance to 5 and starts it as type 2 (PATH_SetType, PATH_StartUnitMove; not in town), again as type 1 if that left no
//  points, and returns 0 if it still has none. Otherwise sets the skill's flags to 1 and returns 1. The point counts are compared signed.
int32_t __fastcall SKILLS_CltStFunc35(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;
    D2DynamicPathStrc *pPath;

    if (!CUNIT_GetPathTargetUnit(pUnit))
        return 0;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    pPath = pUnit->pDynamicPath;
    if (!pPath)
        return 0;

    D2COMMON_PATH_SetNewDistance(pPath, 5);
    D2COMMON_PATH_SetType(pPath, 2);
    D2COMMON_PATH_StartUnitMove(pPath, pUnit, FALSE);
    if (D2COMMON_PATH_GetNumberOfPathPoints(pPath) <= 0)
    {
        D2COMMON_PATH_SetType(pPath, 1);
        D2COMMON_PATH_StartUnitMove(pPath, pUnit, FALSE);
        if (D2COMMON_PATH_GetNumberOfPathPoints(pPath) <= 0)
            return 0;
    }

    D2COMMON_SKILLS_SetFlags(pSkill, 1);
    return 1;
}


//1.10f: D2Client.0x6FAC5860
//  Swarm Move (cltdofunc 49): needs the skills.txt record and the used skill, else returns 0. With bit 2 of the skill's flags set (the movement
//  code sets it when a sequence move ends) the flags are cleared and the animation jumps to frame calc2, otherwise to calc1 (the server's calc
//  columns), through UNITS_SetAnimationFrame. Returns 1. The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc49(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2SkillStrc *pSkill;
    int32_t nFrame;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    if (D2COMMON_SKILLS_GetFlags(pSkill) & 2)
    {
        D2COMMON_SKILLS_SetFlags(pSkill, 0);
        nFrame = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[1], nSkillId, nSkillLevel);
    }
    else
    {
        nFrame = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);
    }

    D2COMMON_UNITS_SetAnimationFrame(pUnit, nFrame);
    return 1;
}


//1.10f: D2Client.0x6FAC5900
//  Nest / EvilHutSpawner start (cltstfunc 36): needs the skills.txt record (tested, not read) and the used skill, else returns 0.
//  MONSTERS_GetSpawnMode_XY gives a spawn point; a negative result (signed) returns 0. The point becomes the skill's Param2 and Param3,
//  which cltdofunc 50 reads, and COLLIDE_MONSTER is set there (SMALL_UNIT_PRESENCE pattern) in the room CLIENT_FindRoomContainingSubtile
//  finds from the unit's. Returns 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById and CLIENT_FindRoomContainingSubtile here.
int32_t __fastcall SKILLS_CltStFunc36(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;
    int32_t nSpawnMode;
    int32_t nX;
    int32_t nY;
    D2ActiveRoomStrc *pRoom;

    if (!DATATBLS_GetSkillsTxtRecordById(nSkillId))
        return 0;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    if (D2COMMON_MONSTERS_GetSpawnMode_XY(pUnit, TRUE, (int32_t)nSkillId, nSkillLevel, &nSpawnMode, &nX, &nY) < 0)
        return 0;

    D2COMMON_SKILLS_SetParam2(pSkill, nX);
    D2COMMON_SKILLS_SetParam3(pSkill, nY);

    //  BUG: when the unit has no room, or no room near it holds the point, the pattern is set on a NULL room (0x6FAC5A20, 0x6FAC5A33). INERT:
    //  COLLISION_SetCollisionMask, which the pattern calls for each subtile, returns at once for a NULL room (D2Common 0x6FD434B7).
    pRoom = CLIENT_FindRoomContainingSubtile(D2COMMON_UNITS_GetRoom(pUnit), nX, nY);
    D2COMMON_COLLISION_SetMaskWithPattern(pRoom, nX, nY, COLLISION_PATTERN_SMALL_UNIT_PRESENCE, COLLIDE_MONSTER);
    return 1;
}


//1.10f: D2Client.0x6FAC5A50
//  Nest / EvilHutSpawner (cltdofunc 50): lifts the collision cltstfunc 36 set. Reads the used skill and its Param2/Param3 (the start's point) and
//  returns 0 if either param is 0; both are set back to 0 at once, so a second do frame finds nothing. Then COLLIDE_MONSTER comes off the point
//  (SMALL_UNIT_PRESENCE pattern) in the room CLIENT_FindRoomContainingSubtile finds from the unit's, or 0 is returned when there is none.
//  Returns 1. Reads neither nSkillId nor nSkillLevel. The original inlines CLIENT_FindRoomContainingSubtile here. The search's NULL answers share
//  this function's `return 0` (0x6FAC5B17), so the room NULL test after it (0x6FAC5B26) can only see a room already tested.
int32_t __fastcall SKILLS_CltDoFunc50(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;
    int32_t nX;
    int32_t nY;
    D2ActiveRoomStrc *pRoom;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    nX = D2COMMON_SKILLS_GetParam2(pSkill);
    nY = D2COMMON_SKILLS_GetParam3(pSkill);
    if (!nX || !nY)
        return 0;

    D2COMMON_SKILLS_SetParam2(pSkill, 0);
    D2COMMON_SKILLS_SetParam3(pSkill, 0);

    pRoom = CLIENT_FindRoomContainingSubtile(D2COMMON_UNITS_GetRoom(pUnit), nX, nY);
    if (!pRoom)
        return 0;

    D2COMMON_COLLISION_ResetMaskWithPattern(pRoom, nX, nY, COLLISION_PATTERN_SMALL_UNIT_PRESENCE, COLLIDE_MONSTER);
    return 1;
}


static_assert(offsetof(D2MissileStrc, nLoops) == 0x34,
              "SKILLS_CltDoFunc55: `mov [esp+70h+var_28], eax` (0x6FAC63B8), 0x34 into the block at var_5C");


//1.10f: D2Client.0x6FAC5B50
//  GargoyleTrap (cltdofunc 51): fires one missile (cltmissilea) toward the path target from a point beside the trap's own axis, and turns the trap
//  to face that point. Returns 0 without the skills.txt record, a valid cltmissilea or a path target unit; sets UNITFLAG_SKSRVDOFUNC. The point
//  takes the target's Y and an X that walks four steps from the unit's X toward the target's when |dx| < |dy| (signed), else the reverse. The
//  missile block starts one signed sixth of the offset (less one) from the unit and is made by MISSILE_ClientSpawn. The facing is the direction to
//  the point, & 0x3F, through gnDir64ToSector4 and gnSector4Direction, set on the unit's path with PATH_SetDirection. Returns 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltDoFunc51(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    D2UnitStrc *pTarget;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nUnitX;
    int32_t nUnitY;
    int32_t nDeltaX;
    int32_t nDeltaY;
    int32_t nNewX;
    int32_t nNewY;
    int32_t nSteps;
    int32_t nDirection;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: the NULL test of pUnit (0x6FAC5BB5) guards only the flag; pUnit goes on untested into CUNIT_GetPathTargetUnit and the position accessors
    //  (0x6FAC5C38). UNREACHABLE.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget)
        return 0;

    nTargetX = UNITS_GetXPosition(pTarget);

    nTargetY = UNITS_GetYPosition(pTarget);

    nUnitX = UNITS_GetXPosition(pUnit);

    nUnitY = UNITS_GetYPosition(pUnit);

    nNewX = nTargetX;
    nNewY = nTargetY;

    nDeltaX = nTargetX - nUnitX;
    if (nDeltaX < 0)
        nDeltaX = -nDeltaX;

    nDeltaY = nTargetY - nUnitY;
    if (nDeltaY < 0)
        nDeltaY = -nDeltaY;

    nSteps = 4;
    if (nDeltaX < nDeltaY)
    {
        nNewX = nUnitX;
        do
        {
            if (nNewX > nTargetX)
                --nNewX;
            else if (nNewX < nTargetX)
                ++nNewX;
        }
        while (--nSteps);
    }
    else
    {
        nNewY = nUnitY;
        do
        {
            if (nNewY > nTargetY)
                --nNewY;
            else if (nNewY < nTargetY)
                ++nNewY;
        }
        while (--nSteps);
    }

    memset(&tParams, 0, sizeof(tParams));
    tParams.pOwner = pUnit;
    tParams.nTargetX = nNewX - nUnitX;
    tParams.nTargetY = nNewY - nUnitY;
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET;
    tParams.nX = tParams.nTargetX / 6 + nUnitX - 1;
    tParams.nSkill = nSkillId;
    tParams.nY = tParams.nTargetY / 6 + nUnitY - 1;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = nMissileId;

    //  BUG: the client turns the trap here; the server's skill never does, but its AI turns the trap with other facings (31, 49, 0, 17). REACHABLE.
    nDirection = D2COMMON_UNITS_GetDirectionToCoords(pUnit, nNewX, nNewY);
    D2COMMON_PATH_SetDirection(pUnit->pDynamicPath, gnSector4Direction[gnDir64ToSector4[nDirection & 0x3F] & 3]);

    MISSILE_ClientSpawn(&tParams);
    return 1;
}


//1.10f: D2Client.0x6FAC5DC0
//  Submerge start (cltstfunc 37): clears UNITFLAG_TARGETABLE, _CANBEATTACKED and _ISVALIDTARGET on a non-NULL unit and returns 1; cltstfunc 33's
//  first line without its collision reset. Reads neither nSkillId nor nSkillLevel.
int32_t __fastcall SKILLS_CltStFunc37(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    if (pUnit)
        pUnit->dwFlags &= ~(UNITFLAG_TARGETABLE | UNITFLAG_CANBEATTACKED | UNITFLAG_ISVALIDTARGET);

    return 1;
}


//1.10f: D2Client.0x6FAC5DE0
//  FetishAura (cltdofunc 53): scatters the aura's cltmissilea (curseeffectred) over an area around the path target with MISSILE_SpawnOverArea.
//  Returns 0 without the skills.txt record or a valid cltmissilea; sets UNITFLAG_SKSRVDOFUNC. The point is SKILLS_GetPathTargetPos's; the radius is
//  aurarangecalc less 6, at least 1 (signed), and the one-in-N chance is cltcalc1. Returns 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc53(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    int32_t nX;
    int32_t nY;
    int32_t nRadius;
    int32_t nOneInChance;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: the NULL test of pUnit (0x6FAC5E41) guards only the flag; pUnit goes on untested into SKILLS_GetPathTargetPos and the two
    //  formulas. UNREACHABLE.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    //  BUG: SKILLS_GetPathTargetPos's answer is tested (0x6FAC5E63), but it answers 1 on every path, so the refusal is dead. UNREACHABLE.
    //  BUG: the server refuses a zero point where the client's fallback never does. NOT ESTABLISHED.
    if (!SKILLS_GetPathTargetPos(pUnit, &nX, &nY))
        return 0;

    nRadius = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwAuraRangeCalc, nSkillId, nSkillLevel) - 6;
    if (nRadius < 1)
        nRadius = 1;

    nOneInChance = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[0], nSkillId, nSkillLevel);
    MISSILE_SpawnOverArea(pUnit, nX, nY, nRadius, nMissileId, (int32_t)nSkillId, nSkillLevel, nOneInChance, 1);
    return 1;
}


//1.10f: D2Client.0x6FAC5ED0
//  FetishInferno / Baal Inferno / MegademonInferno / Horror Arctic Blast (cltdofunc 54): two client missiles per do toward the path target. Returns 0
//  without the skills.txt record or a valid cltmissilea. The unit faces the point (PATH_FaceCoords). Each of two passes rolls the unit's seed and,
//  when cltmissileb is above 0 (signed), a low bit of 1 picks cltmissileb over cltmissilea; SKILLS_MissileCltSpawnWithSkillRange spawns it with
//  gMissileVelJitter. Returns 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById and SEED_RollRandomNumber here.
int32_t __fastcall SKILLS_CltDoFunc54(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileA;
    int32_t nMissileB;
    int32_t nMissileId;
    int32_t nX;
    int32_t nY;
    int32_t nRemaining;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileA = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileA < 0 || nMissileA >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    nMissileB = (int16_t)pSkillsTxt->wCltMissileB;

    //  BUG: SKILLS_GetPathTargetPos's answer is tested (0x6FAC5F48), but it answers 1 on every path, so the refusal is dead. UNREACHABLE.
    //  BUG: the server refuses a zero point where the client's fallback never does. NOT ESTABLISHED.
    if (!SKILLS_GetPathTargetPos(pUnit, &nX, &nY))
        return 0;

    D2COMMON_PATH_FaceCoords(pUnit->pDynamicPath, nX, nY, 0);

    //  BUG: the client makes two missiles per do, with a seeded flag and a velocity jitter; the server makes one, at Vel plus VelLev with no jitter.
    //  REACHABLE.
    nRemaining = 2;
    do
    {
        nMissileId = nMissileA;
        if (nMissileB > 0)
        {
            if ((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) & 1)
                nMissileId = nMissileB;
        }

        SKILLS_MissileCltSpawnWithSkillRange(pUnit, (D2C_MissilesHcIdx)nMissileId, gMissileVelJitter, nX, nY, nSkillId, nSkillLevel);
    }
    while (--nRemaining);

    return 1;
}


//1.10f: D2Client.0x6FAC5FF0
//  Spawns one client missile with a skill's range: a __fastcall with ecx, edx and five stack arguments (retn 14h). Returns the missile, or NULL.
//  The block has flags 0x25 (0x4025 when the first roll of the unit's seed leaves its low two bits nonzero), the owner, the unit's position, the
//  target point, the skill and the level; the velocity is the missile's Vel plus pVelJitter[second roll % 5] (unsigned). After MISSILE_ClientSpawn
//  the range is the skill's calc1, or at 0 or less missiles.txt's Param2 plus the level less one, clamped to 1..255; it goes to
//  PATH_SetNewDistance, MISSILE_SetTotalFrames and MISSILE_SetCurrentFrame, and the animation speed is AnimLen << 8 over the range (signed).
//  The original inlines SEED_RollRandomNumber, UNITS_GetXPosition, UNITS_GetYPosition, DATATBLS_GetSkillsTxtRecordById and the two missiles.txt
//  record lookups here.
D2UnitStrc *__fastcall SKILLS_MissileCltSpawnWithSkillRange(D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId, const int32_t *pVelJitter, int32_t nTargetX, int32_t nTargetY, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2MissileStrc tParams;
    D2MissilesTxt *pMissilesTxt;
    D2UnitStrc *pSpawned;
    D2SkillsTxt *pSkillsTxt;
    D2MissilesTxt *pSpawnedMissilesTxt;
    int32_t nRange;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_VELOCITY | MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pUnit;

    if ((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) & 3)
        tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_VELOCITY | MISSILESPAWN_TARGETPOS | MISSILESPAWN_NOLIGHT;

    tParams.nMissile = nMissileId;

    tParams.nX = UNITS_GetXPosition(pUnit);

    tParams.nY = UNITS_GetYPosition(pUnit);

    tParams.nTargetX = nTargetX;
    tParams.nTargetY = nTargetY;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;

    if ((int32_t)nMissileId < 0 || (int32_t)nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return NULL;

    pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt)
        return NULL;

    tParams.nVelocity = pMissilesTxt->nVel + pVelJitter[(uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 5];

    pSpawned = MISSILE_ClientSpawn(&tParams);
    if (!pSpawned)
        return NULL;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return pSpawned;

    if (pSpawned->dwClassId < 0 || pSpawned->dwClassId >= sgptDataTables->nMissilesTxtRecordCount)
        return pSpawned;

    pSpawnedMissilesTxt = &sgptDataTables->pMissilesTxt[pSpawned->dwClassId];
    if (!pSpawnedMissilesTxt)
        return pSpawned;

    nRange = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);
    if (nRange <= 0)
        nRange = pSpawnedMissilesTxt->dwParam[1] + nSkillLevel - 1;

    if (nRange <= 1)
        nRange = 1;
    else if (nRange >= 255)
        nRange = 255;

    D2COMMON_PATH_SetNewDistance(pSpawned->pDynamicPath, (uint8_t)nRange);
    D2COMMON_UNITS_SetAnimationSpeed(pSpawned, (pSpawnedMissilesTxt->nAnimLen << 8) / nRange);
    D2COMMON_MISSILE_SetTotalFrames(pSpawned, nRange);
    D2COMMON_MISSILE_SetCurrentFrame(pSpawned, nRange);
    return pSpawned;
}


//1.10f: D2Client.0x6FAC6240
//  Emerge / Self-resurrect start (cltstfunc 38): sets UNITFLAG_TARGETABLE, _CANBEATTACKED and _ISVALIDTARGET on a non-NULL unit and returns 1.
//  Reads neither nSkillId nor nSkillLevel.
int32_t __fastcall SKILLS_CltStFunc38(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    //  BUG: for Self-resurrect the client sets the three flags that the server's start clears. REACHABLE.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_TARGETABLE | UNITFLAG_CANBEATTACKED | UNITFLAG_ISVALIDTARGET;

    return 1;
}


//1.10f: D2Client.0x6FAC6260
//  Resurrect / Resurrect2 start (cltstfunc 39): returns 0 without a path target unit; otherwise clears UNITFLAGEX_NODRAW on the caster (pUnit), not
//  the target, and returns 1. Reads neither nSkillId nor nSkillLevel.
int32_t __fastcall SKILLS_CltStFunc39(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    if (!CUNIT_GetPathTargetUnit(pUnit))
        return 0;

    //  BUG: the NULL test of pUnit (0x6FAC6270) comes after CUNIT_GetPathTargetUnit has read pUnit, so it is dead. UNREACHABLE.
    if (pUnit)
        pUnit->dwFlagEx &= ~UNITFLAGEX_NODRAW;

    return 1;
}


//1.10f: D2Client.0x6FAC6290
//  PrimePoisonNova (cltdofunc 55): a ring of cltmissilea (primepoisoncloud) around the unit, in two passes over the sixteen-point table pair
//  gnDir16SquareX/Y. Returns 0 without the skills.txt record, a valid cltmissilea or its missiles.txt record; sets UNITFLAG_SKSRVDOFUNC. The block
//  has flags 0x1F, the unit as owner and origin, the missile, the unit's position, the level, calc1 as the loops and missiles.txt's Param1 << 6 as
//  the velocity (no skill). The step is calc2, at least 1 (signed). First pass: the even points 0, 2 .. 14 (a byte offset stepped by 8 to 0x40),
//  one missile each. Second pass, only for a step above 1, at Param2 << 6: from point 1, the byte offset stepped by the step times four while it
//  is below 60 (signed); the C computes the offset in unsigned arithmetic, as the `add` does, and compares it signed, as the `jl` does.
//  MISSILE_ClientSpawn makes each missile. Returns 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, the missiles.txt record lookup, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltDoFunc55(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    D2MissilesTxt *pMissilesTxt;
    D2MissileStrc tParams;
    int32_t nStep;
    int32_t i;
    uint32_t nStride;
    int32_t nOffset;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;

    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt)
        return 0;

    //  BUG: the NULL test of pUnit (0x6FAC630D) guards only the flag; pUnit goes on untested into the position accessors (0x6FAC632D). UNREACHABLE.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET | MISSILESPAWN_VELOCITY | MISSILESPAWN_SUBLOOPS | MISSILESPAWN_VELOCITYSCALED;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nMissile = nMissileId;

    tParams.nX = UNITS_GetXPosition(pUnit);

    tParams.nY = UNITS_GetYPosition(pUnit);

    tParams.nSkillLevel = nSkillLevel;
    tParams.nLoops = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);
    tParams.nVelocity = pMissilesTxt->dwParam[0] << 6;

    nStep = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[1], nSkillId, nSkillLevel);
    if (nStep < 1)
        nStep = 1;

    for (i = 0; i < 16; i += 2)
    {
        tParams.nTargetX = gnDir16SquareX[i];
        tParams.nTargetY = gnDir16SquareY[i];
        MISSILE_ClientSpawn(&tParams);
    }

    tParams.nVelocity = pMissilesTxt->dwParam[1] << 6;
    if (nStep > 1)
    {
        //  BUG: a step at or above 2^29 wraps the stride (`shl edi, 2`), to 0 at 2^30, which never ends the second pass, and a wrapped
        //  offset reads off the tables. LATENT: the one stock row's calc2 is the constant 2.
        //  DIFFERENCE: the original reads a wrapped offset off its own .rdata around the tables; ours reads off these arrays, outside any
        //  object of ours, which C++ leaves undefined. Declared, not reproduced.
        nStride = (uint32_t)nStep << 2;
        for (nOffset = 0; nOffset < 60; nOffset = (int32_t)((uint32_t)nOffset + nStride))
        {
            tParams.nTargetY = *(const int32_t *)((const char *)(gnDir16SquareY + 1) + nOffset);
            tParams.nTargetX = *(const int32_t *)((const char *)(gnDir16SquareX + 1) + nOffset);
            MISSILE_ClientSpawn(&tParams);
        }
    }

    return 1;
}


//1.10f: D2Client.0x6FAC6460
//  DiabLight start (cltstfunc 40): needs the skills.txt record and the used skill, else returns 0. Evaluates cltcalc1 into the skill's Param1, the
//  counter that cltdofunc 56 reads and advances, and returns 1. The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltStFunc40(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2SkillStrc *pSkill;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    D2COMMON_SKILLS_SetParam1(pSkill, D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[0], nSkillId, nSkillLevel));
    return 1;
}


static_assert(offsetof(D2SkillsTxt, nMonAnim) == 0x11 && MONMODE_SEQUENCE == 14,
              "SKILLS_CltDoFunc56: `cmp byte ptr [edx+11h], 0Eh` (0x6FAC662B)");
static_assert(offsetof(D2MonStats2Txt, nInfernoAnim) == 0x109,
              "SKILLS_CltDoFunc56: `mov al, [edi+109h]` (0x6FAC6633)");
static_assert(STAT_VELOCITYPERCENT == 0x43, "SKILLS_CltDoFunc59: `push 43h` (0x6FAC6CA8)");
//  The path literal that SKILLS_CltDoFunc59 passes as __FILE__: the original's string at 0x6FB77958 (64 characters).
static const char szSkillsMonCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\Skills\\SkillsMon.cpp";


//1.10f: D2Client.0x6FAC64E0
//  DiabLight (cltdofunc 56): a client missile toward the path target, throttled by a counter, and the unit's animation set to the inferno's.
//  Returns 0 without the skills.txt record, a valid cltmissilea (diablight) or the used skill. The counter is the skill's Param1 plus one against
//  cltcalc1 (signed): below it the count is stored and the result is 0, otherwise Param1 goes back to 0 and the missile is made (every do frame
//  for the stock row's 0). The unit faces the path target point (PATH_FaceCoords) and SKILLS_MissileCltSpawnWithSkillRange makes the missile
//  with gnDiabLightVelJitter. Animation: for a non-monster, UNITS_SetAnimationFrame(8) and a frame count of 0x500; for a monster, its MonStats2
//  row (returns 1 if there is none) and, when the skill's monanim is the sequence mode, the frame InfernoAnim less one and a count of InfernoAnim
//  plus one in 24.8, otherwise the current frame at InfernoAnim less one frame. Returns 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById and DATATBLS_GetMonStats2TxtRecord here.
int32_t __fastcall SKILLS_CltDoFunc56(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    D2SkillStrc *pSkill;
    int32_t nCalcCap;
    int32_t nPar1;
    int32_t nX;
    int32_t nY;
    D2MonStats2Txt *pMonStats2Txt;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    nCalcCap = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCltCalc[0], nSkillId, nSkillLevel);
    nPar1 = D2COMMON_SKILLS_GetParam1(pSkill) + 1;
    if (nPar1 < nCalcCap)
    {
        D2COMMON_SKILLS_SetParam1(pSkill, nPar1);
        return 0;
    }

    D2COMMON_SKILLS_SetParam1(pSkill, 0);

    //  BUG: SKILLS_GetPathTargetPos's answer is tested (0x6FAC6585), but it answers 1 on every path, so the refusal is dead. UNREACHABLE.
    //  BUG: the server refuses a zero point where the client's fallback never does. NOT ESTABLISHED.
    if (!SKILLS_GetPathTargetPos(pUnit, &nX, &nY))
        return 0;

    D2COMMON_PATH_FaceCoords(pUnit->pDynamicPath, nX, nY, 0);

    //  BUG: the client's missile has a seeded flag and a velocity jitter; the server's has flags 0x25 at Vel plus VelLev with no jitter. REACHABLE.
    SKILLS_MissileCltSpawnWithSkillRange(pUnit, (D2C_MissilesHcIdx)nMissileId, gnDiabLightVelJitter, nX, nY, nSkillId, nSkillLevel);

    if (pUnit->dwUnitType != UNIT_MONSTER)
    {
        D2COMMON_UNITS_SetAnimationFrame(pUnit, 8);
        pUnit->dwFrameCountPrecise = 0x500;
        return 1;
    }

    pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pUnit->dwClassId);
    if (!pMonStats2Txt)
        return 1;

    if (pSkillsTxt->nMonAnim == MONMODE_SEQUENCE)
    {
        D2COMMON_UNITS_SetAnimationFrame(pUnit, pMonStats2Txt->nInfernoAnim - 1);
        pUnit->dwFrameCountPrecise = (pMonStats2Txt->nInfernoAnim + 1) << 8;
    }
    else
    {
        //  BUG: the client's current frame is InfernoAnim << 8 less one frame; the server's is InfernoAnim << 8. REACHABLE.
        pUnit->nSeqCurrentFramePrecise = (pMonStats2Txt->nInfernoAnim << 8) - 0x100;
    }

    return 1;
}


//1.10f: D2Client.0x6FAC66A0
//  FingerMageSpider (cltdofunc 57): one missile from the unit toward its target or its path's first point. Returns 0 without the skills.txt record,
//  a valid cltmissilea (fingermagespider) and its missiles.txt record; sets UNITFLAG_SKSRVDOFUNC. The block has flags 3, the unit as owner and
//  origin, the missile, the unit's position, the skill, the level and calc1 as the loops. With a path target unit, the unit faces it (its Y read
//  before its X, PATH_FaceCoords) and the block's target is the target's position less the unit's; without one, the target is the path's first
//  point (D2Common #10175, #10176) less the unit's position, through UNITS_GetXPosition and UNITS_GetYPosition called out of line (#10175 and
//  #10176 read the path untested, but CUNIT_GetPathTargetUnit has already asserted it). After MISSILE_ClientSpawn a missile made gets
//  MISSILE_SetTargetX with the SKILL LEVEL. Returns 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, the missiles.txt record lookup, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltDoFunc57(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    D2MissilesTxt *pMissilesTxt;
    D2MissileStrc tParams;
    D2UnitStrc *pTarget;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nUnitX;
    int32_t nUnitY;
    D2UnitStrc *pMissile;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;

    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt)
        return 0;

    //  BUG: the NULL test of pUnit (0x6FAC6719) guards only the flag; pUnit goes on untested into the position accessors (0x6FAC6739). UNREACHABLE.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nMissile = nMissileId;

    tParams.nX = UNITS_GetXPosition(pUnit);

    tParams.nY = UNITS_GetYPosition(pUnit);

    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nLoops = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (pTarget)
    {
        nTargetY = UNITS_GetYPosition(pTarget);

        nTargetX = UNITS_GetXPosition(pTarget);

        D2COMMON_PATH_FaceCoords(pUnit->pDynamicPath, nTargetX, nTargetY, 0);

        nTargetX = UNITS_GetXPosition(pTarget);

        nUnitX = UNITS_GetXPosition(pUnit);
        tParams.nTargetX = nTargetX - nUnitX;

        nTargetY = UNITS_GetYPosition(pTarget);

        nUnitY = UNITS_GetYPosition(pUnit);
    }
    else
    {
        nTargetX = D2COMMON_10175_PathGetFirstPointX(pUnit->pDynamicPath);
        nUnitX = UNITS_GetXPosition(pUnit);
        tParams.nTargetX = nTargetX - nUnitX;

        nTargetY = D2COMMON_10176_PathGetFirstPointY(pUnit->pDynamicPath);
        nUnitY = UNITS_GetYPosition(pUnit);
    }

    tParams.nTargetY = nTargetY - nUnitY;

    pMissile = MISSILE_ClientSpawn(&tParams);
    if (pMissile)
    {
        //  BUG: the client writes the skill level into its missile's target X; the server's equivalent does not. REACHABLE.
        D2COMMON_MISSILE_SetTargetX(pMissile, nSkillLevel);
    }

    return 1;
}


//1.10f: D2Client.0x6FAC6960
//  DiabWall (cltdofunc 58): a row of client missiles toward the path target, each started by SKILLS_MissileStartSeededPathOrStall. Returns 0
//  without the skills.txt record or a valid cltmissilea (diabwallmaker). Sets UNITFLAG_SKSRVDOFUNC with one `or dword ptr [esi+0C4h], 40h`, where
//  its siblings read, `or al` and store. The count is calc1. The block has flags 0x21, the owner, the unit's position, the missile and the level
//  (no skill), with SKILLS_MissileStartSeededPathOrStall as the start; SKILLS_GetPathTargetPos writes the point into the block's target. One
//  missile per count (signed), each with its index as the start's argument. Returns 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltDoFunc58(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    int32_t nCount;
    D2MissileStrc tParams;
    int32_t i;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: the NULL test of pUnit (0x6FAC69BB) guards only the flag; pUnit goes on untested into the formula and the position
    //  accessors (0x6FAC69E9). UNREACHABLE.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    nCount = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pUnit;

    tParams.nX = UNITS_GetXPosition(pUnit);

    tParams.nY = UNITS_GetYPosition(pUnit);

    tParams.nMissile = nMissileId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.pInitFunc = SKILLS_MissileStartSeededPathOrStall;

    //  BUG: SKILLS_GetPathTargetPos's answer is tested (0x6FAC6A75), but it answers 1 on every path, so the refusal is dead. UNREACHABLE.
    //  BUG: the server takes its point from D2GAME_GetXAndYFromTargetUnit_6FD14020 and refuses a zero one; the client never does. NOT ESTABLISHED.
    if (!SKILLS_GetPathTargetPos(pUnit, &tParams.nTargetX, &tParams.nTargetY))
        return 0;

    for (i = 0; i < nCount; ++i)
    {
        tParams.pInitArgs = i;
        MISSILE_ClientSpawn(&tParams);
    }

    return 1;
}


//1.10f: D2Client.0x6FAC6AB0
//  The start of DiabWall's flame missiles, called by MISSILE_ClientSpawn through the block's pInitFunc with the flame's index; a bare retn. For a
//  missile: its frame count capped at 77 (signed), total and current frames set to 77; its seed set to the index plus D2Common #10175 (the X of
//  its path's first point); one roll of that seed, unsigned % 100, and at 20 or more (signed) a type-10 path of the frame count, started
//  (PATH_SetType, PATH_SetNewDistance, PATH_StartUnitMove); below 20 the flame stands where it was made. The frame count lives in a dword slot
//  (the one the prologue's `push ecx` made); PATH_SetNewDistance keeps its low byte and caps it at 77 itself.
//  The server's SKILLS_MissileInit_DiabWall is this function instruction for instruction.
//  The original inlines SEED_RollRandomNumber here.
void __fastcall SKILLS_MissileStartSeededPathOrStall(D2UnitStrc *pMissile, int32_t nSeed)
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

    //  BUG: the missile's path goes untested into D2Common #10175, which reads it (0x6FDA9DE6). UNREACHABLE.
    D2COMMON_SEED_InitLowSeed(&pMissile->pSeed, nSeed + D2COMMON_10175_PathGetFirstPointX(pPath));

    if ((int32_t)((uint32_t)SEED_RollRandomNumber(&pMissile->pSeed) % 100) >= 20)
    {
        D2COMMON_PATH_SetType(pPath, 10);
        D2COMMON_PATH_SetNewDistance(pPath, (uint8_t)nFrames);
        D2COMMON_PATH_StartUnitMove(pPath, pMissile, FALSE);
    }
}


//1.10f: D2Client.0x6FAC6B50
//  DiabRun start (cltstfunc 41): returns 0 without the used skill; otherwise sets the skill's flags to 0 and returns 1. Reads neither nSkillId
//  nor nSkillLevel.
int32_t __fastcall SKILLS_CltStFunc41(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;

    //  BUG: the client keeps its path's target unit, which cltdofunc 59 tests for adjacency; the server's start clears it (PATH_SetTargetUnit,
    //  0x6FD094FD) and aims the path at the target's position. REACHABLE.
    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    D2COMMON_SKILLS_SetFlags(pSkill, 0);
    return 1;
}


//1.10f: D2Client.0x6FAC6B70
//  DiabRun (cltdofunc 59): a charge scripted by the frame, from the row's Param1..6 (8, 14, 5, 13, 16 and 6 in stock). Returns 0 without the
//  skills.txt record or the used skill. With the unit's shifted frame number past Param4 (signed), or with no dynamic path, returns 0. If the path's
//  target unit is within #10399 distance 1 (signed), or bit 2 of the skill's flags is set, the charge stops: flags 0, frame count Param1 << 8,
//  frame Param2, returns 1. At frame Param3 the charge starts: calc1 (20 in stock) << 8, at least 256 (signed), scaled by the unit's
//  STAT_VELOCITYPERCENT over 100 (DATATBLS_ApplyRatio), is the path's velocity (PATH_SetVelocity, with this file's literal and line 1126), the path
//  is started as type 1, the flags go to 1 and the result is 1. At frame Param4 the frame count goes to Param5 << 8 and the frame to Param6.
//  Returns 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById and DATATBLS_ApplyRatio here.
int32_t __fastcall SKILLS_CltDoFunc59(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2SkillStrc *pSkill;
    int32_t nFrameNo;
    int32_t nFrameCount;
    D2DynamicPathStrc *pPath;
    D2UnitStrc *pTarget;
    int32_t nBaseVelocity;
    int32_t nVelocityPercent;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    D2COMMON_UNITS_GetShiftedFrameMetrics(pUnit, &nFrameNo, &nFrameCount);

    //  BUG: the client returns 0 past Param4; the server has no such refusal. REACHABLE.
    if (nFrameNo > pSkillsTxt->dwParam[3])
        return 0;

    pPath = pUnit->pDynamicPath;
    if (!pPath)
        return 0;

    //  BUG: the client stops beside the target unit; the server has no such stop. REACHABLE.
    pTarget = D2COMMON_PATH_GetTargetUnit(pPath);
    if (pTarget && D2COMMON_10399(pTarget, pUnit) <= 1)
    {
        D2COMMON_SKILLS_SetFlags(pSkill, 0);
        pUnit->dwFrameCountPrecise = pSkillsTxt->dwParam[0] << 8;
        D2COMMON_UNITS_SetAnimationFrame(pUnit, pSkillsTxt->dwParam[1]);
        return 1;
    }

    if (D2COMMON_SKILLS_GetFlags(pSkill) & 2)
    {
        D2COMMON_SKILLS_SetFlags(pSkill, 0);
        pUnit->dwFrameCountPrecise = pSkillsTxt->dwParam[0] << 8;
        D2COMMON_UNITS_SetAnimationFrame(pUnit, pSkillsTxt->dwParam[1]);
        return 1;
    }

    if (nFrameNo == pSkillsTxt->dwParam[2])
    {
        nBaseVelocity = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel) << 8;
        if (nBaseVelocity <= 256)
            nBaseVelocity = 256;

        nVelocityPercent = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_VELOCITYPERCENT, 0);

        D2COMMON_PATH_SetVelocity(pPath, DATATBLS_ApplyRatio(nBaseVelocity, nVelocityPercent, 100), szSkillsMonCpp, 1126);
        D2COMMON_PATH_SetType(pPath, 1);
        D2COMMON_PATH_StartUnitMove(pPath, pUnit, FALSE);
        D2COMMON_SKILLS_SetFlags(pSkill, 1);
        return 1;
    }

    if (nFrameNo == pSkillsTxt->dwParam[3])
    {
        pUnit->dwFrameCountPrecise = pSkillsTxt->dwParam[4] << 8;
        D2COMMON_UNITS_SetAnimationFrame(pUnit, pSkillsTxt->dwParam[5]);
    }

    return 1;
}


//1.10f: D2Client.0x6FAC6DB0
//  DesertTurret (cltdofunc 60): a row of cltmissilea (desertfireball) across the line to the path target, centred on the target. Returns 0 without
//  the skills.txt record or a valid cltmissilea; sets UNITFLAG_SKSRVDOFUNC. The point is SKILLS_GetPathTargetPos's; the delta is the point less the
//  unit's position, and a zero delta returns 0. The step is chosen by eight cases on the delta's signs and comes to X = -2 * sign(dy) and
//  Y = 2 * sign(dx), perpendicular to the delta, so the row crosses the line to the target. The count is calc1 (3 in stock); the first offset is
//  the delta less the step times half the count (the half signed). The block is cleared once with flags 3; then for a count above 0 (signed), one
//  missile a pass: the owner, the offset as the target, a start a sixth of it from the unit (`imul` by 2AAAAAABh with the sign added back, one per
//  axis), the level and the missile (no skill), and the offset stepped. Returns 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltDoFunc60(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nUnitX;
    int32_t nUnitY;
    int32_t nDeltaX;
    int32_t nDeltaY;
    int32_t nDirX;
    int32_t nDirY;
    int32_t nCount;
    int32_t nHalf;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: the NULL test of pUnit (0x6FAC6E1D) guards only the flag; pUnit goes on untested into SKILLS_GetPathTargetPos and the position accessors
    //  (0x6FAC6E47). UNREACHABLE.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    //  BUG: SKILLS_GetPathTargetPos's answer is tested (0x6FAC6E3F), but it answers 1 on every path, so the refusal is dead. UNREACHABLE.
    //  BUG: the server takes its point from D2GAME_GetXAndYFromTargetUnit_6FD14020 and refuses a zero one; the client never does. NOT ESTABLISHED.
    if (!SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY))
        return 0;

    nUnitX = UNITS_GetXPosition(pUnit);

    nUnitY = UNITS_GetYPosition(pUnit);

    nDeltaX = nTargetX - nUnitX;
    nDeltaY = nTargetY - nUnitY;
    if (!nDeltaX && !nDeltaY)
        return 0;

    nDirX = 0;
    nDirY = 0;
    if (nDeltaX < 0 && nDeltaY < 0)
    {
        nDirX = 2;
        nDirY = -2;
    }
    else if (nDeltaX == 0 && nDeltaY < 0)
    {
        nDirX = 2;
    }
    else if (nDeltaX > 0 && nDeltaY < 0)
    {
        nDirX = 2;
        nDirY = 2;
    }
    else if (nDeltaX > 0 && nDeltaY == 0)
    {
        nDirY = 2;
    }
    else if (nDeltaX > 0 && nDeltaY > 0)
    {
        nDirX = -2;
        nDirY = 2;
    }
    else if (nDeltaX == 0 && nDeltaY > 0)
    {
        nDirX = -2;
    }
    else if (nDeltaX < 0 && nDeltaY > 0)
    {
        nDirX = -2;
        nDirY = -2;
    }
    else if (nDeltaX < 0 && nDeltaY == 0)
    {
        nDirY = -2;
    }

    nCount = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);
    nHalf = nCount / 2;
    nDeltaX -= nHalf * nDirX;
    nDeltaY -= nHalf * nDirY;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET;
    if (nCount > 0)
    {
        do
        {
            //  BUG: the server's Y start is taken from the X offset's sixth; the client's is taken from the Y offset. REACHABLE.
            tParams.pOwner = pUnit;
            tParams.nTargetX = nDeltaX;
            tParams.nX = nDeltaX / 6 + nUnitX;
            tParams.nY = nDeltaY / 6 + nUnitY;
            tParams.nTargetY = nDeltaY;
            tParams.nSkillLevel = nSkillLevel;
            tParams.nMissile = nMissileId;
            MISSILE_ClientSpawn(&tParams);

            nDeltaY += nDirY;
            nDeltaX += nDirX;
        }
        while (--nCount);
    }

    return 1;
}


//1.10f: D2Client.0x6FAC7030
//  ArcaneTower (cltdofunc 61): a ring of cltmissilea (lightningtowernova) that falls from above. Returns 0 without the skills.txt record, a valid
//  cltmissilea or a path target unit (tested, not read); sets UNITFLAG_SKSRVDOFUNC. The velocity is D2Common's
//  DATATBLS_GetMissileVelocityFromMissilesTxt for the missile and the level. The block has flags 3 (OR'd with 4 once the position is in), the
//  unit's position, the owner, the missile, the skill, the level and the velocity. 64 missiles, one per point of gnArcaneTowerCircleX/Y (a byte
//  offset stepped by 4 below 100h). Each missile made gets PARTICLE_SetUnitParticleFlag, PARTICLE_SetUnitRisesToTarget(0), a graphic vector of Z -6
//  (PARTICLE_SetUnitGfxVector, scaled), a zero particle vector and a graphic Z of 120: it starts 120 up and falls 6 a frame. Returns 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltDoFunc61(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    int32_t nVelocity;
    D2MissileStrc tParams;
    int32_t i;
    D2UnitStrc *pMissile;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: the NULL test of pUnit (0x6FAC708A) guards only the flag; pUnit goes on untested into CUNIT_GetPathTargetUnit and the position accessors
    //  (0x6FAC70C8). UNREACHABLE.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    if (!CUNIT_GetPathTargetUnit(pUnit))
        return 0;

    nVelocity = D2COMMON_DATATBLS_GetMissileVelocityFromMissilesTxt(nMissileId, nSkillLevel);

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET;

    tParams.nX = UNITS_GetXPosition(pUnit);

    tParams.nY = UNITS_GetYPosition(pUnit);

    tParams.dwFlags |= MISSILESPAWN_VELOCITY;
    tParams.pOwner = pUnit;
    tParams.nMissile = nMissileId;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nVelocity = nVelocity;

    for (i = 0; i < 64; ++i)
    {
        tParams.nTargetY = gnArcaneTowerCircleY[i];
        tParams.nTargetX = gnArcaneTowerCircleX[i];

        pMissile = MISSILE_ClientSpawn(&tParams);
        if (pMissile)
        {
            PARTICLE_SetUnitParticleFlag(pMissile);
            PARTICLE_SetUnitRisesToTarget(pMissile, 0);
            PARTICLE_SetUnitGfxVector(pMissile, 0, 0, -6, 1);
            PARTICLE_SetUnitParticleVector(pMissile, 0, 0, 0, 0);
            PARTICLE_SetUnitGfxCoords(pMissile, 0, 0, 120);
        }
    }

    return 1;
}


//1.10f: D2Client.0x6FAC71E0
//  Mosquito start (cltstfunc 42): needs the skills.txt record, the used skill and the path target unit, else returns 0. Then a seed of its own
//  (SEED_InitSeed, the low half set to the target's id plus the unit's, -1 for no unit) and a count: calc1 (4 in stock) plus a roll below calc2
//  less calc1 (7 - 4), at least 1 (signed), goes into Param1; the target's id and type go into Param2 and Param3. cltdofunc 62 counts Param1
//  down. Returns 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById and SEED_RollLimitedRandomNumber here.
int32_t __fastcall SKILLS_CltStFunc42(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2SkillStrc *pSkill;
    D2UnitStrc *pTarget;
    D2SeedStrc tSeed;
    uint32_t dwUnitId;
    int32_t nMin;
    int32_t nSpan;
    int32_t nPar1;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pTarget)
        return 0;

    D2COMMON_SEED_InitSeed(&tSeed);

    //  BUG: the NULL test of pUnit (0x6FAC725B) comes after CUNIT_GetPathTargetUnit has read pUnit, so it is dead. UNREACHABLE.
    dwUnitId = pUnit ? pUnit->dwUnitId : (uint32_t)-1;
    D2COMMON_SEED_InitLowSeed(&tSeed, (int32_t)(pTarget->dwUnitId + dwUnitId));

    nMin = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);
    nSpan = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[1], nSkillId, nSkillLevel) - nMin;

    nPar1 = SEED_RollLimitedRandomNumber(&tSeed, nSpan) + nMin;
    if (nPar1 < 1)
        nPar1 = 1;

    D2COMMON_SKILLS_SetParam1(pSkill, nPar1);
    D2COMMON_SKILLS_SetParam2(pSkill, (int32_t)pTarget->dwUnitId);
    D2COMMON_SKILLS_SetParam3(pSkill, (int32_t)pTarget->dwUnitType);
    return 1;
}


//1.10f: D2Client.0x6FAC7330
//  Mosquito (cltdofunc 62): the bites that drain, counted down on the start's Param1. Returns 0 without the skills.txt record or the used skill.
//  The target is found by Param2 (id) and Param3 (type) with CUNIT_FindUnitByGuidAndType (Param3 read first); with none, Param1 goes to 0 and the
//  result is 0. A target not within UNITS_IsInMeleeRange (bonus 0) returns that call's 0. Param1 less one; at 0 or less (signed) the result is 0,
//  otherwise it is stored, the animation goes back to frame Param1 of the row (12) and the frame count to its whole frame plus one, and the result
//  is 1. The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc62(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2SkillStrc *pSkill;
    D2C_UnitTypes nTargetType;
    D2UnitStrc *pTarget;
    int32_t nPar1;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return 0;

    nTargetType = (D2C_UnitTypes)D2COMMON_SKILLS_GetParam3(pSkill);
    pTarget = CUNIT_FindUnitByGuidAndType((uint32_t)D2COMMON_SKILLS_GetParam2(pSkill), nTargetType);
    if (!pTarget)
    {
        //  BUG: the client zeroes the count when the target is gone; the server leaves Param1 as it is. NOT ESTABLISHED.
        D2COMMON_SKILLS_SetParam1(pSkill, 0);
        return 0;
    }

    if (!D2COMMON_UNITS_IsInMeleeRange(pUnit, pTarget, 0))
        return 0;

    nPar1 = D2COMMON_SKILLS_GetParam1(pSkill) - 1;
    if (nPar1 <= 0)
        return 0;

    D2COMMON_SKILLS_SetParam1(pSkill, nPar1);
    D2COMMON_UNITS_SetAnimationFrame(pUnit, pSkillsTxt->dwParam[0]);
    pUnit->dwFrameCountPrecise = (pUnit->dwFrameCountPrecise & ~0xFF) + 0x100;
    return 1;
}


//1.10f: D2Client.0x6FAC7400
//  A start no stock row names (cltstfunc 43): returns 0 without the skills.txt record or a valid cltmissilea; otherwise calls
//  SKILLS_MissileClientSpawnAimedAtTargetUnit with the missile, the skill and the level (ecx still pUnit, edx the missile; its result is not read)
//  and returns 1.
int32_t __fastcall SKILLS_CltStFunc43(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    SKILLS_MissileClientSpawnAimedAtTargetUnit(pUnit, (D2C_MissilesHcIdx)nMissileId, nSkillId, nSkillLevel);
    return 1;
}


//1.10f: D2Client.0x6FAC7460
//  RegurgitatorEat (cltdofunc 63): two scatters of missiles around the unit. Returns 0 without the skills.txt record or a valid cltmissilea
//  (corpseexplosion); sets UNITFLAG_SKSRVDOFUNC. The block has flags 0x2021, the unit as owner and origin, its position, the missile, the skill and
//  the level. SKILLS_SpawnScatteredMissiles with jitter 2 and count 1; then, with a valid cltmissileb (bigblood1), the same block with that
//  missile and jitter 4, count 5. Returns 1 either way. The position accessors' copies each reload the missile into edx after their D2Common
//  call: the register kept across the call, nothing of the C's. The original inlines DATATBLS_GetSkillsTxtRecordById, UNITS_GetXPosition and
//  UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltDoFunc63(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileId;
    int32_t nMissileB;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: the NULL test of pUnit (0x6FAC74BD) guards only the flag; pUnit goes on untested into the position accessors (0x6FAC74DD). UNREACHABLE.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS | MISSILESPAWN_RANDOMDIR;
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;

    tParams.nX = UNITS_GetXPosition(pUnit);

    tParams.nY = UNITS_GetYPosition(pUnit);

    tParams.nMissile = nMissileId;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    SKILLS_SpawnScatteredMissiles(pUnit, &tParams, 2, 1);

    nMissileB = (int16_t)pSkillsTxt->wCltMissileB;
    if (nMissileB >= 0 && nMissileB < sgptDataTables->nMissilesTxtRecordCount)
    {
        tParams.nMissile = nMissileB;
        SKILLS_SpawnScatteredMissiles(pUnit, &tParams, 4, 5);
    }

    return 1;
}


//1.10f: D2Client.0x6FAC75C0
//  QueenDeath (cltdofunc 64): the maggot queen's death, with no guard at all. Takes the unit's skill of this id
//  (SKILLS_GetHighestLevelSkillFromUnitAndId), its Param1 and the whole frame of the unit's 24.8 counter (signed `sar 8`):
//    frame 6    frame 7 and a frame count of 0x3200 (50 frames)
//    frame 22   Param1 below 4 (signed): Param1 plus one, the frame held until it reaches 4; then frame 40 and a count of 0x1100 (17 frames)
//    frame 30   UNITMODE_SetAnimMode(MONMODE_DEAD) and CUNIT_SetUnitDead
//  Returns 1 on every path.
int32_t __fastcall SKILLS_CltDoFunc64(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillStrc *pSkill;
    int32_t nPar1;
    int32_t nFrameNo;

    //  BUG: the skill goes untested into D2Common's SKILLS_GetParam1, which asserts it, so a unit without the skill would fail the assert.
    //  NOT ESTABLISHED.
    pSkill = D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, nSkillId);
    nPar1 = D2COMMON_SKILLS_GetParam1(pSkill);
    nFrameNo = pUnit->nSeqCurrentFramePrecise >> 8;

    if (nFrameNo == 6)
    {
        D2COMMON_UNITS_SetAnimationFrame(pUnit, 7);
        pUnit->dwFrameCountPrecise = 0x3200;
        return 1;
    }

    if (nFrameNo == 22)
    {
        if (nPar1 < 4)
        {
            D2COMMON_SKILLS_SetParam1(pSkill, nPar1 + 1);
            return 1;
        }

        D2COMMON_UNITS_SetAnimationFrame(pUnit, 40);
        pUnit->dwFrameCountPrecise = 0x1100;
        return 1;
    }

    if (nFrameNo == 30)
    {
        UNITMODE_SetAnimMode(pUnit, MONMODE_DEAD);
        CUNIT_SetUnitDead(pUnit);
    }

    return 1;
}


static_assert(MISSILE_CURSECENTER == 0xBF,
              "SKILLS_CltDoFunc65: `mov [esp+78h+var_4C], 0BFh` (0x6FAC7721), stock missiles.txt Id 191");
static_assert(MISSILE_HELLMETEORLAUNCH1 == 0x14D && MISSILE_HELLMETEORLAUNCH2 == 0x14E && MISSILE_HELLMETEORUP == 0x14F
                  && MISSILE_HELLMETEORDOWN == 0x150 && MISSILE_HELLMETEORBALL == 0x151,
              "SKILLS_CltStFunc44: `mov edx, 14Dh..151h` (0x6FAC777F..0x6FAC7939), stock missiles.txt Id 333..337");
static_assert(COMPOSIT_SPECIAL3 == 10 && COMPOSIT_SPECIAL4 == 11,
              "SKILLS_CltDoFunc94/95: `mov bl, [ebp+0Eh]` / `[ebp+0Fh]` (0x6FAC7A5E, 0x6FAC7B7E), nComponent at +4");
static_assert(MISSILE_BOLT == 0x1F, "SKILLS_CltDoFunc66: `cmp eax, 1Fh` (0x6FAC7C8C), stock missiles.txt Id 31");
static_assert(SKILL_SPIDERLAY == 0xAD && MISSILE_SPIDERGOOLAY == 0x8F,
              "SKILLS_LaySpiderGooTrail: `mov edx, 0ADh` (0x6FAC7D8A), `mov [esp+88h+var_4C], 8Fh` (0x6FAC7F3E)");


//1.10f: D2Client.0x6FAC7650
//  Slot 65 of gpSkillCltDoFuncs, which no stock row names: a curse centre that streams. Returns 0 without the skills.txt record or a valid
//  cltmissileb, or when SKILLS_GetPathTargetPos fails. Sets UNITFLAG_SKSRVDOFUNC. Evaluates aurarangecalc, kept in the nSkillLevel argument's own
//  slot while ebp holds the level for the block. The block is cleared, with flags 1, the owner, the point as the origin, the skill, the level
//  and missile 191 (MISSILE_CURSECENTER). With no missile returns 0; otherwise MISSILE_SetStreamMissile with cltmissileb, MISSILE_SetStreamRange
//  with the range, and 1.
//  Both setters read a WORD off the stack (D2Common 0x6FDBA2C4, 0x6FDBA314) and store it at +4 and +6 of the missile's data, so the full dwords
//  pushed here reach them as the uint16_t and __int16 their prototypes take; the C casts.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_CltDoFunc65(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nStreamMissile;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nStreamRange;
    D2UnitStrc *pMissile;
    D2MissileStrc tParams;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nStreamMissile = (int16_t)pSkillsTxt->wCltMissileB;
    if (nStreamMissile < 0 || nStreamMissile >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    //  BUG: SKILLS_GetPathTargetPos's answer is tested (0x6FAC76BD), but it answers 1 on every path, so the refusal is dead. UNREACHABLE.
    if (!SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY))
        return 0;

    //  BUG: the NULL test of pUnit (0x6FAC76C5) comes after pUnit has gone to SKILLS_GetPathTargetPos, and guards only the flag. UNREACHABLE.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    nStreamRange = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwAuraRangeCalc, nSkillId, nSkillLevel);

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS;
    tParams.pOwner = pUnit;
    tParams.nX = nTargetX;
    tParams.nY = nTargetY;
    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = MISSILE_CURSECENTER;
    pMissile = MISSILE_ClientSpawn(&tParams);
    if (!pMissile)
        return 0;

    D2COMMON_MISSILE_SetStreamMissile(pMissile, (uint16_t)nStreamMissile);
    D2COMMON_MISSILE_SetStreamRange(pMissile, (int16_t)nStreamRange);
    return 1;
}


//1.10f: D2Client.0x6FAC7770
//  HellMeteor start (cltstfunc 44): six missiles, no record read. SKILLS_MissileClientSpawnOnTargetUnit starts hellmeteordown (336) and
//  hellmeteorball (337) at the point the unit is heading for, if it is within 100 subtiles (else it returns 0). Then MISSILE_ClientSpawnSimple at
//  the unit's own position, the position read afresh with Y before X: hellmeteorup (335), hellmeteorball (337), hellmeteorlaunch2 (334) and
//  hellmeteorlaunch1 (333). Then, each under a test of its own: the up and the ball at the unit get PARTICLE_SetUnitGfxVector (0, 0, 15, bScale 1);
//  the down and the ball at the target get PARTICLE_SetUnitGfxCoords (0, 0, 5625) and a vector of (0, 0, -25, bScale 1). Both launches' animation
//  speed is set to 128, under one test, of hellmeteorlaunch2. Returns 1. The up missile's result is kept in the nSkillLevel argument's slot,
//  the level having been loaded into ebp at the start; the C keeps a local.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_CltStFunc44(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2UnitStrc *pDownAtTarget;
    D2UnitStrc *pBallAtTarget;
    D2UnitStrc *pUpAtUnit;
    D2UnitStrc *pBallAtUnit;
    D2UnitStrc *pLaunch2;
    D2UnitStrc *pLaunch1;
    int32_t nUnitX;
    int32_t nUnitY;

    pDownAtTarget = SKILLS_MissileClientSpawnOnTargetUnit(pUnit, MISSILE_HELLMETEORDOWN, nSkillId, nSkillLevel);
    pBallAtTarget = SKILLS_MissileClientSpawnOnTargetUnit(pUnit, MISSILE_HELLMETEORBALL, nSkillId, nSkillLevel);

    nUnitY = UNITS_GetYPosition(pUnit);

    nUnitX = UNITS_GetXPosition(pUnit);

    pUpAtUnit = MISSILE_ClientSpawnSimple(pUnit, MISSILE_HELLMETEORUP, nUnitX, nUnitY, nSkillId, nSkillLevel);

    nUnitY = UNITS_GetYPosition(pUnit);

    nUnitX = UNITS_GetXPosition(pUnit);

    pBallAtUnit = MISSILE_ClientSpawnSimple(pUnit, MISSILE_HELLMETEORBALL, nUnitX, nUnitY, nSkillId, nSkillLevel);

    nUnitY = UNITS_GetYPosition(pUnit);

    nUnitX = UNITS_GetXPosition(pUnit);

    pLaunch2 = MISSILE_ClientSpawnSimple(pUnit, MISSILE_HELLMETEORLAUNCH2, nUnitX, nUnitY, nSkillId, nSkillLevel);

    nUnitY = UNITS_GetYPosition(pUnit);

    nUnitX = UNITS_GetXPosition(pUnit);

    pLaunch1 = MISSILE_ClientSpawnSimple(pUnit, MISSILE_HELLMETEORLAUNCH1, nUnitX, nUnitY, nSkillId, nSkillLevel);

    if (pUpAtUnit)
        PARTICLE_SetUnitGfxVector(pUpAtUnit, 0, 0, 15, 1);

    if (pBallAtUnit)
        PARTICLE_SetUnitGfxVector(pBallAtUnit, 0, 0, 15, 1);

    if (pDownAtTarget)
    {
        PARTICLE_SetUnitGfxCoords(pDownAtTarget, 0, 0, 5625);
        PARTICLE_SetUnitGfxVector(pDownAtTarget, 0, 0, -25, 1);
    }

    if (pBallAtTarget)
    {
        PARTICLE_SetUnitGfxCoords(pBallAtTarget, 0, 0, 5625);
        PARTICLE_SetUnitGfxVector(pBallAtTarget, 0, 0, -25, 1);
    }

    //  BUG: the test (0x6FAC79BD) is of launch2 (334), but it then hands launch1 (333, never tested) to UNITS_SetAnimationSpeed, whose stores to
    //  [ecx+4Ch] have no test of the unit before them (D2Common 0x6FDBF8DE..0x6FDBF900); a NULL launch1 beside a live launch2 would write at 0x4C.
    //  Reproduced. NOT ESTABLISHED.
    if (pLaunch2)
    {
        D2COMMON_UNITS_SetAnimationSpeed(pLaunch1, 128);
        D2COMMON_UNITS_SetAnimationSpeed(pLaunch2, 128);
    }

    return 1;
}


//1.10f: D2Client.0x6FAC79F0
//  DoomKnightMissile (cltdofunc 94): returns 0 without the skills.txt record or a valid cltmissilea (undeadmissile1 in stock). For a monster (a
//  non-NULL unit of type 1 with monster data) the missile is cltmissilea plus its COMPOSIT_SPECIAL3 component (byte 10 of nComponent, zero
//  extended); for anything else plus 0. The sum is range-checked in missiles.txt (count first, then sign) and its record fetched, else 0.
//  Sets UNITFLAG_SKSRVDOFUNC. If the skill's `lob` flag is set (bit 1 of dwFlags[0], tested against the low byte of gdwBitMasks[1]), calls
//  MISSILE_ClientSpawnSkillEffect with no offset and a target of 0, 0; otherwise SKILLS_MissileClientSpawnAimedAtTargetUnit. Returns 1.
//  The original inlines DATATBLS_GetSkillsTxtRecordById and the missiles.txt range test here.
int32_t __fastcall SKILLS_CltDoFunc94(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileBase;
    int32_t nMissileCount;
    D2MonsterDataStrc *pMonsterData;
    uint8_t nMissileVariant;
    int32_t nMissileId;
    D2MissilesTxt *pMissilesTxt;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileBase = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileBase < 0)
        return 0;

    nMissileCount = sgptDataTables->nMissilesTxtRecordCount;
    if (nMissileBase >= nMissileCount)
        return 0;

    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && (pMonsterData = pUnit->pMonsterData) != NULL)
        nMissileVariant = pMonsterData->nComponent[COMPOSIT_SPECIAL3];
    else
        nMissileVariant = 0;

    nMissileId = nMissileBase + nMissileVariant;

    if (nMissileId >= nMissileCount || nMissileId < 0)
        return 0;

    pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt)
        return 0;

    //  BUG: the NULL tests of pUnit guard the component read and the flag; pUnit goes on untested into either spawn. UNREACHABLE.
    //  BUG: the server refuses a unit with no target (SUNIT_GetTargetUnit, 0x6FD0A625); the client spawns toward its path target without asking.
    //  NOT ESTABLISHED.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    if ((uint8_t)pSkillsTxt->dwFlags[0] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_LOB])
    {
        MISSILE_ClientSpawnSkillEffect(pUnit, nMissileId, 0, 0, 0, 0, nSkillId, nSkillLevel, 0);
        return 1;
    }

    SKILLS_MissileClientSpawnAimedAtTargetUnit(pUnit, (D2C_MissilesHcIdx)nMissileId, nSkillId, nSkillLevel);
    return 1;
}


//1.10f: D2Client.0x6FAC7B10
//  NecromageMissile (cltdofunc 95): SKILLS_CltDoFunc94 with the component COMPOSIT_SPECIAL4 (byte 11 of nComponent) in place of SPECIAL3;
//  cltmissilea is necromage1 in stock.
int32_t __fastcall SKILLS_CltDoFunc95(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nMissileBase;
    int32_t nMissileCount;
    D2MonsterDataStrc *pMonsterData;
    uint8_t nMissileVariant;
    int32_t nMissileId;
    D2MissilesTxt *pMissilesTxt;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    nMissileBase = (int16_t)pSkillsTxt->wCltMissileA;
    if (nMissileBase < 0)
        return 0;

    nMissileCount = sgptDataTables->nMissilesTxtRecordCount;
    if (nMissileBase >= nMissileCount)
        return 0;

    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && (pMonsterData = pUnit->pMonsterData) != NULL)
        nMissileVariant = pMonsterData->nComponent[COMPOSIT_SPECIAL4];
    else
        nMissileVariant = 0;

    nMissileId = nMissileBase + nMissileVariant;

    if (nMissileId >= nMissileCount || nMissileId < 0)
        return 0;

    pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt)
        return 0;

    //  BUG: as in 94, the NULL tests of pUnit guard the component read and the flag. UNREACHABLE.
    //  BUG: as in 94, the server refuses a unit with no target and the client does not. NOT ESTABLISHED.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    if ((uint8_t)pSkillsTxt->dwFlags[0] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_LOB])
    {
        MISSILE_ClientSpawnSkillEffect(pUnit, nMissileId, 0, 0, 0, 0, nSkillId, nSkillLevel, 0);
        return 1;
    }

    SKILLS_MissileClientSpawnAimedAtTargetUnit(pUnit, (D2C_MissilesHcIdx)nMissileId, nSkillId, nSkillLevel);
    return 1;
}


//1.10f: D2Client.0x6FAC7C30
//  MissileSkill1 / HireableMissile / RogueMissile (cltdofunc 66): the weapon's missile first, the row's second. Returns 0 without the skills.txt
//  record; sets UNITFLAG_SKSRVDOFUNC. Calls D2Common #11039 with the ADDRESS of the nSkillLevel argument. It answers 0 for a NULL unit; for a
//  non-player, 27 (MISSILE_MAGICARROW) while it holds STAT_ITEM_MAGICARROW, else 41 (MISSILE_EXPLODINGARROW) while it holds
//  STAT_ITEM_EXPLOSIVEARROW, else 0; for a player it asks UNITS_GetWeaponClass: WEAPONCLASS_BOW gives the same three answers, WEAPONCLASS_XBW the
//  same with 31 (MISSILE_BOLT) in place of the 0, any other class -1. Each stat answer writes the stat's value through the pointer, over the skill
//  level. An answer other than 0 and 31 is the missile: inside missiles.txt with its record it is spawned by
//  SKILLS_MissileClientSpawnAimedAtTargetUnit and the result is 1; outside it (-1 included) the result is 0, with no fallback. For 0 or 31 the row's
//  cltmissilea (rogue1 for RogueMissile; the other two rows carry none) is used if inside missiles.txt with its record, else 0: then the skill's
//  `lob` as in 94 (MISSILE_ClientSpawnSkillEffect), or the same spawn as the weapon's. Returns 1. Every spawn takes the level back from the
//  argument's slot after #11039, so an arrow stat's value is the level the missile goes out with; the C passes &nSkillLevel and reads nSkillLevel
//  after. The original inlines DATATBLS_GetSkillsTxtRecordById and the missiles.txt range tests here.
int32_t __fastcall SKILLS_CltDoFunc66(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nWeaponMissileId;
    int32_t nMissileId;
    D2MissilesTxt *pMissilesTxt;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    //  BUG: the NULL test of pUnit guards only the flag; pUnit goes on into #11039, which answers 0 for it, and so into a spawn. UNREACHABLE.
    //  BUG: as in 94, the server refuses a unit with no target (SUNIT_GetTargetUnit, 0x6FD0A8A2) and the client does not. NOT ESTABLISHED.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    nWeaponMissileId = D2COMMON_11039_CheckWeaponIsMissileBased(pUnit, &nSkillLevel);
    if (nWeaponMissileId && nWeaponMissileId != MISSILE_BOLT)
    {
        if (nWeaponMissileId < 0 || nWeaponMissileId >= sgptDataTables->nMissilesTxtRecordCount)
            return 0;

        pMissilesTxt = &sgptDataTables->pMissilesTxt[nWeaponMissileId];
        if (!pMissilesTxt)
            return 0;

        SKILLS_MissileClientSpawnAimedAtTargetUnit(pUnit, (D2C_MissilesHcIdx)nWeaponMissileId, nSkillId, nSkillLevel);
        return 1;
    }

    nMissileId = (int16_t)pSkillsTxt->wCltMissileA;

    if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        return 0;

    pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt)
        return 0;

    if ((uint8_t)pSkillsTxt->dwFlags[0] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_LOB])
    {
        MISSILE_ClientSpawnSkillEffect(pUnit, nMissileId, 0, 0, 0, 0, nSkillId, nSkillLevel, 0);
        return 1;
    }

    //  The bytes share the weapon arm's spawn and its `return 1` (0x6FAC7CCF).
    SKILLS_MissileClientSpawnAimedAtTargetUnit(pUnit, (D2C_MissilesHcIdx)nMissileId, nSkillId, nSkillLevel);
    return 1;
}


//1.10f: D2Client.0x6FAC7D60
//  SPIDERLAY, the spider's trail (a bare retn for the one register argument; UNITMODE_StepUnitAlongPath's one call): lays one web behind a spider
//  that has moved. Returns NULL with no dynamic path or when PATH_HasMovedThisFrame is 0. Takes the level of the unit's SKILL_SPIDERLAY
//  (SKILLS_GetHighestLevelSkillFromUnitAndId, SKILLS_GetSkillLevel with the bonus), its room and its position. The step: the path's new direction
//  collapsed by D2Common #11053 to one of eight sectors picks a direction code from a table built on the stack, {10, 8, 22, 20, 18, 16, 14, 12},
//  and #11055 turns the code into a delta: sectors 0..7 give (-2,-2), (0,-2), (2,-2), (2,0), (2,2), (0,2), (-2,2) and (-2,0). The web is the
//  position plus the delta, behind the spider. The room is the unit's if the web lies inside it, else the first of
//  DUNGEON_GetAdjacentRoomsListFromRoom (count compared unsigned) it lies inside, else NULL; a room in town (DUNGEON_IsRoomInTown) returns NULL,
//  but a web point in no room reaches it as NULL, which answers 0, so the web is laid. The block is cleared, with flags 25h, the web as the
//  origin, the web plus twice the delta as the target, velocity 3, the level and missile 143 (MISSILE_SPIDERGOOLAY), no skill. Sound 1830
//  (spider_web_1) plays on the unit, and the missile is the result.
//  The code picked out of the stack table is written back over its first byte and pushed as a dword with the table's next three bytes above it
//  (#11055 keeps the low byte); the same dword is then zeroed and handed to DUNGEON_GetAdjacentRoomsListFromRoom as the list pointer: the C keeps
//  a table and a pointer, and writes the code back as the bytes do. The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
D2UnitStrc *__fastcall SKILLS_LaySpiderGooTrail(D2UnitStrc *pUnit)
{
    D2DynamicPathStrc *pDynamicPath;
    D2SkillStrc *pSpiderLaySkill;
    int32_t nSkillLevel;
    D2ActiveRoomStrc *pRoom;
    D2ActiveRoomStrc **ppRoomList;
    D2ActiveRoomStrc *pAdjacentRoom;
    int32_t nRoomCount;
    uint32_t iRoom;
    int32_t nUnitX;
    int32_t nUnitY;
    uint8_t aDirections[8] = {10, 8, 22, 20, 18, 16, 14, 12};
    int32_t nDeltaX;
    int32_t nDeltaY;
    int32_t nWebX;
    int32_t nWebY;
    D2UnitStrc *pMissile;
    D2MissileStrc tParams;

    pDynamicPath = pUnit->pDynamicPath;
    if (!pDynamicPath || !D2COMMON_PATH_HasMovedThisFrame(pDynamicPath))
        return NULL;

    pSpiderLaySkill = D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, SKILL_SPIDERLAY);
    nSkillLevel = D2COMMON_SKILLS_GetSkillLevel(pUnit, pSpiderLaySkill, TRUE);
    pRoom = D2COMMON_UNITS_GetRoom(pUnit);

    nUnitX = UNITS_GetXPosition(pUnit);

    nUnitY = UNITS_GetYPosition(pUnit);

    aDirections[0] = aDirections[D2COMMON_11053(D2COMMON_PATH_GetNewDirection(pDynamicPath))];
    D2COMMON_11055(aDirections[0], &nDeltaX, &nDeltaY);

    nWebX = nUnitX + nDeltaX;
    nWebY = nUnitY + nDeltaY;

    if (pRoom && !(nWebX >= pRoom->tCoords.nSubtileX && nWebX < pRoom->tCoords.nSubtileX + pRoom->tCoords.nSubtileWidth && nWebY >= pRoom->tCoords.nSubtileY && nWebY < pRoom->tCoords.nSubtileY + pRoom->tCoords.nSubtileHeight))
    {
        ppRoomList = NULL;
        nRoomCount = 0;
        D2COMMON_DUNGEON_GetAdjacentRoomsListFromRoom(pRoom, &ppRoomList, &nRoomCount);

        pRoom = NULL;
        for (iRoom = 0; iRoom < (uint32_t)nRoomCount; ++iRoom)
        {
            pAdjacentRoom = ppRoomList[iRoom];
            if (pAdjacentRoom && nWebX >= pAdjacentRoom->tCoords.nSubtileX && nWebX < pAdjacentRoom->tCoords.nSubtileX + pAdjacentRoom->tCoords.nSubtileWidth && nWebY >= pAdjacentRoom->tCoords.nSubtileY
                && nWebY < pAdjacentRoom->tCoords.nSubtileY + pAdjacentRoom->tCoords.nSubtileHeight)
            {
                pRoom = ppRoomList[iRoom];
                break;
            }
        }
    }

    //  BUG: the client lays no web in a room in town; the server has no town test and lays webs there. NOT ESTABLISHED.
    if (D2COMMON_DUNGEON_IsRoomInTown(pRoom))
        return NULL;

    //  BUG: the client's block has flags 25h, no skill and a sound; the server's has flags 21h, names SKILL_SPIDERLAY as the skill and
    //  plays no sound. REACHABLE.
    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_VELOCITY | MISSILESPAWN_TARGETPOS;
    tParams.nTargetX = nWebX + 2 * nDeltaX;
    tParams.nTargetY = nWebY + 2 * nDeltaY;
    tParams.pOwner = pUnit;
    tParams.nX = nWebX;
    tParams.nY = nWebY;
    tParams.nVelocity = 3;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = MISSILE_SPIDERGOOLAY;
    pMissile = MISSILE_ClientSpawn(&tParams);

    SOUNDINST_PlaySound(SOUND_SPIDER_WEB_1, pUnit, 0, 0, 0);
    return pMissile;
}

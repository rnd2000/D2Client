#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "SoundChan.h"
#include "UnitMode.h"

//  UnitMode.cpp -- the original's UNIT\UnitMode.cpp, rewritten from D2Client.dll 1.10 (image base 0x6FAA0000): a unit's path and position held
//  against the server's (its path target, its room, the moves it starts, the corrections it takes), the client command a click becomes,
//  the client action dispatched by unit type, and the animation mode a unit ends in.
//  Address range 0x6FB4DEC0..0x6FB4F495. The source file name comes from the original's asserts.

//  The path literal this unit passes as __FILE__, one object in the original, at 0x6FB94090.
static const char szUnitModeCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UNIT\\UnitMode.cpp";

//  The members and numbers this file reads that no other file pins.
static_assert(offsetof(D2UnitStrc, nLastClickY) == 0x8E,
              "UNITMODE_SyncUnitPosition: `mov [esi+8Eh], di` (0x6FB4DFC8)");
static_assert(offsetof(D2MonStats2Txt, nModeFlags) == 0xF0 && MONMODE_WALK == 2,
              "UNITMODE_MoveUnitToCoords: `mov dl, [eax+0F0h]` against gdwBitMasks[2] (0x6FB4E45F, 0x6FB4E46A)");
static_assert(UNITFLAG_ISASYNC == 1u << 21,
              "UNITMODE_StartMoveToLocation: `shr eax, 15h / and eax, 1` (0x6FB4DF79)");
static_assert(UNITFLAG_ROOMFREED == 1u << 23 && UNITFLAGEX_REQUESTUPDATE == 1u << 5,
              "UNITMODE_UpdateUnit: `shr eax, 17h / and eax, 1` (0x6FB4E6B7), `shr ecx, 5 / and ecx, 1` "
              "(0x6FB4E6C9), `and ecx, 0FF7FFFFFh` (0x6FB4E6EE), `and al, 0DFh` (0x6FB4E6F4)");
static_assert(MONMODE_GETHIT == 3 && MONMODE_ATTACK2 == 5 && MONMODE_BLOCK == 6 && MONMODE_SKILL4 == 11,
              "UNITMODE_SyncUnitPosition: `sub eax, 3 / cmp eax, 8`, cases 3-5 and 6-11 (0x6FB4E0B1)");
static_assert(STATE_SPIDERLAY == 22,
              "UNITMODE_StepUnitAlongPath: `push 16h` (0x6FB4E668)");
static_assert(OBJECT_COMPELLING_ORB == 404 && OBJECT_HELLFORGE == 376 && SKILL_KICK == 1,
              "UNITMODE_InteractWithUnit: `cmp eax, 194h` (0x6FB4E8D0), `cmp eax, 178h` (0x6FB4E947), "
              "`mov edx, 1` (0x6FB4E8B7)");
static_assert(SKILLSFLAGINDEX_INTERRUPT == 31,
              "UNITMODE_PerformClientCommand: `mov cl, [eax+7]` against gdwBitMasks[7] (0x6FB4EAE8, 0x6FB4EAEB)");
static_assert(MONMODE_KNOCKBACK == 13,
              "UNITMODE_PlayHitOverlayForMode: `cmp edx, 0Dh` (0x6FB4F271)");
//  D2C_ClientActions values, spelled by name where the original carries the literal.
static_assert(CLTACTION_WALKTOUNIT == 0 && CLTACTION_WALKTOXY == 1 && CLTACTION_INTERACT == 2
                  && CLTACTION_NEUTRAL == 7 && CLTACTION_SKILL_XY == 21 && CLTACTION_SKILL_UNIT == 22
                  && CLTACTION_RUNTOXY == 23 && CLTACTION_RUNTOUNIT == 24 && CLTACTION_SKILL1MODE == 25,
              "the client action literals: gnClientCmdClass' actions 0, 1, 2, 21..24 (PerformClientCommand), "
              "`mov ecx, 7` (0x6FB5DA5B), `mov ecx, 16h` / `15h` (0x6FAB380E, 0x6FAB3871), "
              "`mov ecx, 19h` (0x6FB4CD45), `sub ecx, 2` (ITEM_DoClientAction)");


//1.10f: D2Client.0x6FB4DEC0
//  Drops a path target that no longer resolves. With a target unit on the path, the (type, GUID) the path keeps beside the pointer is looked up
//  again in the client's unit table (CUNIT_FindUnitByGuidAndType); when the answer is not the pointer the path holds, the target is cleared,
//  PATH_SetTargetUnit(path, NULL). The path's pointer is asked for twice, and nothing between the two can change it.
//  Both asserts exit: "hUnit" at UnitMode.cpp:38 and "hPath" at :42. The push of edi before the PATH_GetTargetTypeAndGUID call is a register
//  save, not an argument: that function is __stdcall over three.
void __fastcall UNITMODE_ClearStaleTargetUnit(D2UnitStrc *pUnit)
{
    D2DynamicPathStrc *hPath;
    D2UnitStrc *pFound;
    int32_t nTargetType;
    D2UnitGUID nTargetGUID;

    if (!pUnit)
    {
        FOG_DisplayAssert("hUnit", szUnitModeCpp, 38);
        exit(-1);
    }

    hPath = pUnit->pDynamicPath;
    if (!hPath)
    {
        FOG_DisplayAssert("hPath", szUnitModeCpp, 42);
        exit(-1);
    }

    if (!D2COMMON_PATH_GetTargetUnit(hPath))
        return;

    D2COMMON_PATH_GetTargetTypeAndGUID(hPath, &nTargetType, &nTargetGUID);
    pFound = CUNIT_FindUnitByGuidAndType(nTargetGUID, (D2C_UnitTypes)nTargetType);
    if (pFound != D2COMMON_PATH_GetTargetUnit(hPath))
        D2COMMON_PATH_SetTargetUnit(hPath, NULL);
}


//1.10f: D2Client.0x6FB4DF50
//  Re-caches the path's room (PATH_RecacheRoomIfNeeded) on the unit's dynamic path, with neither the unit nor the path tested.
//  The one caller is MONSTER_DoClientAction, and a monster holds its path from MONSTER_InitClientUnit to its free, so the missing tests are
//  not a defect.
void __fastcall UNITMODE_RecacheUnitRoom(D2UnitStrc *pUnit)
{
    D2COMMON_PATH_RecacheRoomIfNeeded(pUnit->pDynamicPath);
}


//1.10f: D2Client.0x6FB4DF60
//  Starts a walk to a point: the path's target is set to (nX, nY) (PATH_SetTargetPos) and PATH_StartUnitMove plans the route; its answer, whether
//  a route was made, is this function's. nY is the one stack argument.
//  PATH_StartUnitMove's third argument (bAllowInTown) is bit 21 of the unit's flags, (dwFlags >> 21) & 1, a logical shift: UNITFLAG_ISASYNC.
int32_t __fastcall UNITMODE_StartMoveToLocation(D2UnitStrc *pUnit, int32_t nX, int32_t nY)
{
    D2DynamicPathStrc *pPath;

    pPath = pUnit->pDynamicPath;
    D2COMMON_10170_PathSetTargetPos(pPath, nX, nY);
    return D2COMMON_PATH_StartUnitMove(pPath, pUnit, (pUnit->dwFlags >> 21) & 1);
}


//1.10f: D2Client.0x6FB4DF90
//  Holds a unit's position against the server's point (nX, nY). Six parameters: four on the stack behind the two registers. The original inlines
//  UNITS_GetXPosition, UNITS_GetYPosition and CLIENT_FindRoomContainingSubtile here.
//
//  A zero coordinate (16-bit compare) or a dead unit returns at once. Otherwise (nX, nY) is recorded as the unit's last click (words at +8Ch and
//  +8Eh), the unit's room is taken, and its current game position is read. Then the tolerance, in subtiles:
//
//      nToleranceMode 1                      10
//      nToleranceMode 2                      0
//      otherwise, the local player           ((latency + 50) >> 7), plus 3 in PLRMODE_NEUTRAL, 7 in PLRMODE_RUN, 5 in any other
//      otherwise, a monster                  5 in GETHIT, ATTACK1, ATTACK2; 7 in BLOCK, CAST, SKILL1..4; 15 in any other mode
//      otherwise                             15
//
//  The latency is GAME_GetNetworkLatency's, and the sum is shifted as unsigned. The monster modes go through a byte table and a two-slot jump
//  table, a switch with nine case labels.
//
//  A difference on either axis past the tolerance (absolute value, compared signed) asks for a warp. It is called off when nToleranceMode is 0,
//  nNextX is positive (signed), the server's point is within ten subtiles (squared distance under 100) and the point (nNextX, nNextY) is nearer the
//  unit than the server's point is (PATH_ComputeSquaredDistance twice, both compares signed). The callers that pass a next point are
//  SCMD_HandleSrv18_PlayerAttributes, SCMD_HandleSrv95_LifeManaUpdate and SCMD_HandleSrv96_WalkVerify, all for the local player, and theirs is the
//  server's point plus the step the packet carries: when the server's unit, one step on, comes nearer to where the client has it, the two are
//  converging and nothing is corrected.
//
//  Within the tolerance the unit warps only when neither end is on the screen: both coordinates differ from the current ones (either equal is
//  enough to stay), and GFXUTIL_IsUnitCelOnScreen answers 0 for the current client position and for the server's point converted to client
//  coordinates. The original keeps that converted pair in the dead nNextX and nToleranceMode argument slots; the C gives them locals of their own.
//  The flag is re-read on the way out and is always 0 there, so that test only ever returns; the C writes the flag as the original keeps it.
//
//  The warp. The destination room is the unit's own room or an adjacent one holding the server's point, else the room found over the whole act
//  (DUNGEON_FindRoomBySubtileCoordinates), else nothing is done. Then three arms:
//
//    * The local player is not moved. It sends its own current position to the server as client packet 0x5F (MSG_SendCmdXY, Y read before X) and
//      the server decides. The room found is used only as the gate.
//    * Another player is moved: PLAYER_SetUnitRoomAndPos.
//    * Any other unit goes through D2Common #10230: the path moved to the point in that room. Its second argument, the unit, is not read.
//  BUG: no unit type is tested on the third arm; for an object, an item or a tile +2Ch is the static path. UNREACHABLE: on the stock server only
//  players (rows 0x0F, 0x10, 0x16) and monsters (0x68, 0x6A, 0x6B, 0x6D) get here, so the arm is a monster's.
void __fastcall UNITMODE_SyncUnitPosition(D2UnitStrc *pUnit, uint16_t nX, uint16_t nY, uint32_t nToleranceMode, int32_t nNextX, int32_t nNextY)
{
    D2ActiveRoomStrc *pRoom;
    D2ActiveRoomStrc *pDestRoom;
    int32_t nCurX;
    int32_t nCurY;
    int32_t nTolerance;
    int32_t nDelta;
    int32_t nDistSquared;
    int32_t nClientX;
    int32_t nClientY;
    int32_t nNewClientX;
    int32_t nNewClientY;
    int32_t nPlayerY;
    BOOL bWarp;

    nTolerance = 0;
    if (!nX || !nY || CUNIT_IsUnitDead(pUnit))
        return;

    pUnit->nLastClickX = nX;
    pUnit->nLastClickY = nY;
    pRoom = D2COMMON_UNITS_GetRoom(pUnit);
    bWarp = FALSE;

    nCurX = UNITS_GetXPosition(pUnit);

    nCurY = UNITS_GetYPosition(pUnit);

    if (nToleranceMode == 1)
    {
        nTolerance = 10;
    }
    else if (nToleranceMode != 2)
    {
        if (pUnit == CUNIT_GetPlayer())
        {
            if (pUnit->dwAnimMode == PLRMODE_NEUTRAL)
                nTolerance = (((uint32_t)GAME_GetNetworkLatency() + 50) >> 7) + 3;
            else if (pUnit->dwAnimMode == PLRMODE_RUN)
                nTolerance = (((uint32_t)GAME_GetNetworkLatency() + 50) >> 7) + 7;
            else
                nTolerance = (((uint32_t)GAME_GetNetworkLatency() + 50) >> 7) + 5;
        }
        else
        {
            nTolerance = 15;
            if (pUnit->dwUnitType == UNIT_MONSTER)
            {
                switch (pUnit->dwAnimMode)
                {
                case MONMODE_GETHIT:
                case MONMODE_ATTACK1:
                case MONMODE_ATTACK2:
                    nTolerance = 5;
                    break;

                case MONMODE_BLOCK:
                case MONMODE_CAST:
                case MONMODE_SKILL1:
                case MONMODE_SKILL2:
                case MONMODE_SKILL3:
                case MONMODE_SKILL4:
                    nTolerance = 7;
                    break;

                default:
                    break;
                }
            }
        }
    }

    nDelta = (int32_t)nX - nCurX;
    if (nDelta < 0)
        nDelta = -nDelta;
    if (nDelta > nTolerance)
        bWarp = TRUE;

    nDelta = (int32_t)nY - nCurY;
    if (nDelta < 0)
        nDelta = -nDelta;
    if (nDelta > nTolerance)
        bWarp = TRUE;

    if (bWarp && !nToleranceMode && nNextX > 0)
    {
        nDistSquared = D2COMMON_PATH_ComputeSquaredDistance(nCurX, nCurY, nX, nY);
        if (nDistSquared < 100 && D2COMMON_PATH_ComputeSquaredDistance(nCurX, nCurY, nNextX, nNextY) < nDistSquared)
            bWarp = FALSE;
    }

    if (!bWarp)
    {
        nClientX = D2COMMON_UNITS_GetClientCoordX(pUnit);
        nClientY = D2COMMON_UNITS_GetClientCoordY(pUnit);
        nNewClientX = nX;
        nNewClientY = nY;
        D2COMMON_DUNGEON_GameSubtileToClientCoords(&nNewClientX, &nNewClientY);
        if (nX != nCurX && nY != nCurY && !GFXUTIL_IsUnitCelOnScreen(pUnit, nClientX, nClientY) && !GFXUTIL_IsUnitCelOnScreen(pUnit, nNewClientX, nNewClientY))
        {
            bWarp = TRUE;
        }
    }

    if (!bWarp)
        return;

    pDestRoom = CLIENT_FindRoomContainingSubtile(pRoom, nX, nY);
    if (!pDestRoom)
    {
        pDestRoom = D2COMMON_DUNGEON_FindRoomBySubtileCoordinates(gpAct, nX, nY);
        if (!pDestRoom)
            return;
    }

    if (pUnit == CUNIT_GetPlayer())
    {
        nPlayerY = UNITS_GetYPosition(pUnit);

        MSG_SendCmdXY(0x5F, (int16_t)UNITS_GetXPosition(pUnit), (int16_t)nPlayerY);
        return;
    }

    if (pUnit->dwUnitType != UNIT_PLAYER)
        D2COMMON_10230(pUnit->pDynamicPath, (int32_t)pUnit, pDestRoom, nX, (int16_t)nY);
    else
        PLAYER_SetUnitRoomAndPos(pUnit, pDestRoom, nX, (int16_t)nY);
}


//1.10f: D2Client.0x6FB4E360
//  Sends a unit that is more than one subtile from (nX, nY) there. Its one caller is PLAYER_DoClientAction's action 7 (stop, back to the neutral
//  mode), which passes the action's (nX, nY) and, as bSnap, whether the unit was dead before that action made it alive again. The original inlines
//  UNITS_GetXPosition, UNITS_GetYPosition, DATATBLS_GetMonStats2TxtRecord and UNITMODE_DispatchActionByUnitType here.
//
//  The per-axis distance is abs(n - current) > 1 on a game coordinate, so the unit is one subtile. A monster must also have a walk: monstats2's
//  mode flags (nModeFlags +0F0h, a bit array over the modes in D2C_MonModes order) tested at MONMODE_WALK. When either test fails, the path's
//  pathing flag is cleared, PATH_SetPathingFlag(path, FALSE), and nothing else happens.
//
//  Otherwise the unit is given client action 1 (walk to a point) through the dispatch by unit type, with a parameter block of seven dwords:
//
//      [0] nX   [1] nY   [3] 1, for a monster only   [4] 100   [5] 15
//
//  [2] and [6] are never written, and neither is [3] for any other type; no unwritten slot is read on this path. The monster's [3] = 1 is the
//  original storing nAction's register, which is 1 on every path to that store.
//
//  A player that is not the local one gets action 23 (run to a point) instead, or, with bSnap set, no action at all: UNITMODE_SyncUnitPosition
//  with nToleranceMode 2, whose tolerance is 0, so any difference warps.
void __fastcall UNITMODE_MoveUnitToCoords(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t bSnap)
{
    D2MonStats2Txt *pMonStats2TxtRecord;
    int32_t aActionParams[7];
    D2C_ClientActions nAction;
    int32_t nDelta;
    BOOL bMovedFar;

    bMovedFar = FALSE;

    nDelta = nX - UNITS_GetXPosition(pUnit);
    if (nDelta < 0)
        nDelta = -nDelta;
    if (nDelta > 1)
        bMovedFar = TRUE;

    nDelta = nY - UNITS_GetYPosition(pUnit);
    if (nDelta < 0)
        nDelta = -nDelta;
    if (nDelta > 1)
        bMovedFar = TRUE;

    if (pUnit->dwUnitType == UNIT_MONSTER)
    {
        pMonStats2TxtRecord = DATATBLS_GetMonStats2TxtRecord(pUnit->dwClassId);
        if (!pMonStats2TxtRecord || !(gdwBitMasks[MONMODE_WALK & 7] & pMonStats2TxtRecord->nModeFlags[MONMODE_WALK >> 3]))
        {
            D2COMMON_10208_PathSetPathingFlag(pUnit->pDynamicPath, FALSE);
            return;
        }
    }

    if (!bMovedFar)
    {
        D2COMMON_10208_PathSetPathingFlag(pUnit->pDynamicPath, FALSE);
        return;
    }

    aActionParams[0] = nX;
    aActionParams[1] = nY;
    nAction = CLTACTION_WALKTOXY;
    if (pUnit->dwUnitType == UNIT_PLAYER)
    {
        if (pUnit != CUNIT_GetPlayer())
        {
            if (bSnap)
            {
                UNITMODE_SyncUnitPosition(pUnit, (uint16_t)nX, (uint16_t)nY, 2, 0, 0);
                return;
            }
            nAction = CLTACTION_RUNTOXY;
        }
    }
    else if (pUnit->dwUnitType == UNIT_MONSTER)
    {
        aActionParams[3] = 1;
    }
    aActionParams[4] = 100;
    aActionParams[5] = 15;

    UNITMODE_DispatchActionByUnitType(nAction, pUnit, aActionParams, FALSE);
}


//1.10f: D2Client.0x6FB4E580
//  Starts a walk to a unit. The target is looked up in the client's unit table by (GUID, type) and set as the path's target whether it was found
//  or not (a NULL clears the old one); only a found one has the route planned, PATH_StartUnitMove with bAllowInTown 0; the answer is 0 otherwise.
//  nTargetGUID is the one stack argument; nTargetType stays in edx from entry to the lookup. The push of edi is a register save.
//  gnMoveToUnitGuidOrMask is ORed with every GUID asked for, found or not; SOUNDCHAN_SendCodeIntegrityProbe reads it and clears it.
int32_t __fastcall UNITMODE_StartMoveToUnit(D2UnitStrc *pUnit, int32_t nTargetType, int32_t nTargetGUID)
{
    D2DynamicPathStrc *pPath;
    D2UnitStrc *pTarget;

    pPath = pUnit->pDynamicPath;
    pTarget = CUNIT_FindUnitByGuidAndType((uint32_t)nTargetGUID, (D2C_UnitTypes)nTargetType);
    gnMoveToUnitGuidOrMask |= (uint32_t)nTargetGUID;
    D2COMMON_PATH_SetTargetUnit(pPath, pTarget);
    if (!pTarget)
        return 0;

    return D2COMMON_PATH_StartUnitMove(pPath, pUnit, 0);
}


//1.10f: D2Client.0x6FB4E5D0
//  One frame of a unit's walk. The stale path target is dropped (UNITMODE_ClearStaleTargetUnit, inlined whole here); a unit in the blaze state
//  lays its fire trail (SKILLS_LayBlazeTrail) and one in spiderlay its goo (SKILLS_LaySpiderGooTrail), STATES_CheckState on 13 and 22, both
//  checked every frame; then D2Common #10226 steps the path, its second argument UNRESOLVED_sub_6FAABFE0's answer, which is always 0.
//  The answer is the step's negation, neg / sbb / inc: TRUE when #10226 answers 0.
BOOL __fastcall UNITMODE_StepUnitAlongPath(D2UnitStrc *pUnit)
{
    UNITMODE_ClearStaleTargetUnit(pUnit);

    if (D2COMMON_STATES_CheckState(pUnit, STATE_BLAZE))
        SKILLS_LayBlazeTrail(pUnit);

    if (D2COMMON_STATES_CheckState(pUnit, STATE_SPIDERLAY))
        SKILLS_LaySpiderGooTrail(pUnit);

    return !D2COMMON_10226(pUnit, UNRESOLVED_sub_6FAABFE0());
}


//1.10f: D2Client.0x6FB4E6A0
//  One frame of one unit. Its one caller is CUNIT_UpdateAllUnits.
//
//  A unit that is not the local player, with UNITFLAG_ROOMFREED (bit 23 of its flags) set, gets no update. With UNITFLAGEX_REQUESTUPDATE (bit 5
//  of its extra flags) also set, the client sends packet 0x4B, (type, id), asking the server to refresh the unit, and clears both bits. Then, if the
//  unit is an async one (UNITFLAG_ISASYNC, bit 21), it is removed by its client id, CUNIT_RemoveUnitByClientId, a tail jmp. The two tests are
//  written as the original takes them, a shift and `and 1`, like the async one; the two clears by name.
//  D2Common's DUNGEON_FreeRoom sets both bits on the units of a room it frees, so this arm is a server unit left in a freed room asking the
//  server for itself again, and an async one going.
//
//  Every other unit: its stat lists' expiry (D2Common #10516), its particle (PARTICLE_UpdateUnitParticle), its overlays (GFX_TickUnitOverlays),
//  then the per-type advance -- PLAYER_AdvanceAnimMode, MONSTER_UpdateAnimMode, OBJECT_AdvanceAnim, MISSILE_ClientUpdate, ITEM_OnUnitModeUpdate
//  -- and last the unit's queued packets, SCMD_ProcessUnitPacketQueue, a tail jmp, on the unit looked up again by the id and type read BEFORE
//  the advance, which can free it: by client id for an async unit (CUNIT_FindUnitByClientIdAndType), by GUID otherwise.
//
//  The id, type and async bit are read null-safe, `pUnit ? ... : -1`, 6, 0, the shape of inlined accessors written in place.
//  BUG: the NULL arm of those reads is dead: the unit goes untested to STATLIST_UpdateStatListsExpiration and to GFX_TickUnitOverlays first, and
//  the type is read untested right after. UNREACHABLE: no caller passes NULL; CUNIT_UpdateAllUnits calls it eight times, each inside a
//  `while (pUnit)` walk.
void __fastcall UNITMODE_UpdateUnit(D2UnitStrc *pUnit)
{
    D2UnitStrc *pFound;
    int32_t nUnitId;
    D2C_UnitTypes nUnitType;
    uint32_t bIsAsync;

    if (CUNIT_GetPlayer() != pUnit && pUnit && ((pUnit->dwFlags >> 23) & 1))
    {
        if ((pUnit->dwFlagEx >> 5) & 1)
        {
            MSG_SendCmdUnit(0x4B, (int32_t)pUnit->dwUnitType, (int32_t)pUnit->dwUnitId);
            pUnit->dwFlags &= ~UNITFLAG_ROOMFREED;
            pUnit->dwFlagEx &= ~UNITFLAGEX_REQUESTUPDATE;
        }

        if ((pUnit->dwFlags >> 21) & 1)
            CUNIT_RemoveUnitByClientId((int32_t)pUnit->dwUnitId, pUnit->dwUnitType);
        return;
    }

    D2COMMON_STATLIST_UpdateStatListsExpiration(pUnit, 0);
    PARTICLE_UpdateUnitParticle(pUnit);

    if (pUnit)
    {
        nUnitId = (int32_t)pUnit->dwUnitId;
        nUnitType = pUnit->dwUnitType;
        bIsAsync = (pUnit->dwFlags >> 21) & 1;
    }
    else
    {
        nUnitId = -1;
        nUnitType = UNIT_TYPES_COUNT;
        bIsAsync = 0;
    }

    GFX_TickUnitOverlays(pUnit);
    switch (pUnit->dwUnitType)
    {
    case UNIT_PLAYER:
        PLAYER_AdvanceAnimMode(pUnit);
        break;

    case UNIT_MONSTER:
        MONSTER_UpdateAnimMode(pUnit);
        break;

    case UNIT_OBJECT:
        OBJECT_AdvanceAnim(pUnit);
        break;

    case UNIT_MISSILE:
        MISSILE_ClientUpdate(pUnit);
        break;

    case UNIT_ITEM:
        ITEM_OnUnitModeUpdate(pUnit);
        break;

    default:
        break;
    }

    if (bIsAsync)
        pFound = CUNIT_FindUnitByClientIdAndType(nUnitId, nUnitType);
    else
        pFound = CUNIT_FindUnitByGuidAndType((uint32_t)nUnitId, nUnitType);
    if (pFound)
        SCMD_ProcessUnitPacketQueue(pFound);
}


//1.10f: D2Client.0x6FB4E7E0
//  Interacts with a unit, client action 2. Both arguments are in registers; nUnitType is a WORD (and ebp, 0FFFFh at 0x6FB4E7F8). The original
//  inlines UNITS_GetXPosition and UNITS_GetYPosition here. The target is looked up by (GUID, type); not found, nothing is done.
//  BUG: the local player is used untested. UNREACHABLE: the callers (the cube panel's three handlers and PLAYER_DoClientAction's action 2,
//  which only PerformClientCommand's command 19 makes) run with the local player there.
//  Otherwise, by the type ASKED FOR (the masked register, not the found unit's field):
//
//      PLAYER    the local player turned toward it (UNITS_UpdateDirectionAndSpeed, D2Common #10413), then packet 0x13
//      MONSTER   packet 0x13 at most once per 200 ms, the clock kept in the TARGET's dwTickCount (+0D4h; unsigned compare); no turn
//      OBJECT    the local player's own path target cleared; then an object without UNITFLAG_CANBEATTACKED gets packet 0x13 only; one
//                with it is turned toward and struck -- see below -- then packet 0x13
//      ITEM      packet 0x16 with the item's GUID and whether the inventory is open, UI_GetUiWindowState(UI_INVENTORY) masked to a byte
//      MISSILE, and any type past ITEM
//                packet 0x13
//
//  An object that can be attacked is kicked: the skill block is zeroed (seven dwords), and [0] is SKILL_KICK's id through the player's
//  highest-level Kick. Two quest objects change that. The Compelling Orb (404), in OBJMODE_NEUTRAL, wants 'qf2 ' in the left hand (Khalim's Will)
//  and is then struck with SKILL_ATTACK; without it the player plays sound event 0x13 and nothing is sent. The Hellforge (376) in OBJMODE_NEUTRAL
//  sends packet 0x13 for type 2 and returns; in OBJMODE_OPENED it wants 'hfh ' (the Hellforge Hammer), the same way. The orb in another mode and
//  the forge in any other are kicked. The block goes to SKILLS_CltStartSkillOnUnit with [1] -1 (the owner GUID), [2] UNIT_OBJECT and [3] the
//  object's GUID, stored in the order [3], [1], [2]; its answer is discarded and packet 0x13 follows.
//  The __LINE__s 531, 542 and 564 are SKILLS_GetSkillIdFromSkill's assert arguments, one per call; the original pushes 542's and 564's before
//  one shared tail, a merge of two calls that the C writes out.
void __fastcall UNITMODE_InteractWithUnit(uint16_t nUnitType, int32_t nUnitGUID)
{
    D2UnitStrc *pPlayer;
    D2UnitStrc *pTarget;
    D2UnitStrc *pWeapon;
    uint32_t dwNow;
    int32_t nType;
    int32_t nY;
    int32_t aSkillParams[7];

    pPlayer = CUNIT_GetPlayer();
    nType = nUnitType;
    pTarget = CUNIT_FindUnitByGuidAndType((uint32_t)nUnitGUID, (D2C_UnitTypes)nType);
    if (!pTarget)
        return;

    switch (nType)
    {
    case UNIT_PLAYER:
        nY = UNITS_GetYPosition(pTarget);

        D2COMMON_UNITS_UpdateDirectionAndSpeed(pPlayer, UNITS_GetXPosition(pTarget), nY);
        break;

    case UNIT_MONSTER:
        dwNow = GetTickCount();
        if (dwNow - pTarget->dwTickCount < 200)
            return;
        pTarget->dwTickCount = dwNow;
        MSG_SendCmdUnit(0x13, nType, nUnitGUID);
        return;

    case UNIT_OBJECT:
        D2COMMON_PATH_SetTargetUnit(pPlayer->pDynamicPath, NULL);
        if (!((pTarget->dwFlags >> 2) & 1))
            break;

        nY = UNITS_GetYPosition(pTarget);

        D2COMMON_UNITS_UpdateDirectionAndSpeed(pPlayer, UNITS_GetXPosition(pTarget), nY);

        memset(aSkillParams, 0, sizeof(aSkillParams));
        aSkillParams[0] = D2COMMON_SKILLS_GetSkillIdFromSkill(D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pPlayer, SKILL_KICK), szUnitModeCpp, 531);

        if (pTarget->dwClassId == OBJECT_COMPELLING_ORB)
        {
            if (pTarget->dwAnimMode == OBJMODE_NEUTRAL)
            {
                pWeapon = D2COMMON_INVENTORY_GetLeftHandWeapon(pPlayer->pInventory);
                if (!pWeapon || D2COMMON_ITEMS_GetBaseCode(pWeapon) != 0x20326671u)    /* 'qf2 ' */
                {
                    UNITSND_PlayPlayerSoundEvent(pPlayer, 0x13u);
                    return;
                }
                aSkillParams[0] = D2COMMON_SKILLS_GetSkillIdFromSkill(D2COMMON_SKILLS_GetSkillById(pPlayer, SKILL_ATTACK, (D2UnitGUID)-1), szUnitModeCpp, 542);
            }
        }
        else if (pTarget->dwClassId == OBJECT_HELLFORGE)
        {
            if (pTarget->dwAnimMode == OBJMODE_NEUTRAL)
            {
                MSG_SendCmdUnit(0x13, UNIT_OBJECT, nUnitGUID);
                return;
            }
            if (pTarget->dwAnimMode == OBJMODE_OPENED)
            {
                pWeapon = D2COMMON_INVENTORY_GetLeftHandWeapon(pPlayer->pInventory);
                if (!pWeapon || D2COMMON_ITEMS_GetBaseCode(pWeapon) != 0x20686668u)    /* 'hfh ' */
                {
                    UNITSND_PlayPlayerSoundEvent(pPlayer, 0x13u);
                    return;
                }
                aSkillParams[0] = D2COMMON_SKILLS_GetSkillIdFromSkill(D2COMMON_SKILLS_GetSkillById(pPlayer, SKILL_ATTACK, (D2UnitGUID)-1), szUnitModeCpp, 564);
            }
        }

        aSkillParams[3] = nUnitGUID;
        aSkillParams[1] = -1;
        aSkillParams[2] = UNIT_OBJECT;
        SKILLS_CltStartSkillOnUnit(pPlayer, aSkillParams);
        break;

    case UNIT_ITEM:
        MSG_SendCmdUnitParam(0x16, UNIT_ITEM, nUnitGUID, (uint8_t)UI_GetUiWindowState(UI_INVENTORY));
        return;

    default:
        break;
    }

    MSG_SendCmdUnit(0x13, nType, nUnitGUID);
}


//1.10f: D2Client.0x6FB4EA90
//  Turns a click into a client command. nClientCmd (1..19) is the client packet it becomes; the third and fourth arguments are a point for the
//  location commands and a (type, GUID) for the unit ones. Three stack arguments behind the two registers; the third, nUnused, is never read.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, PLAYER_GetQueuedCommand (twice), UNITMODE_DispatchActionByUnitType and
//  PLAYER_ClearQueuedCommand here.
//
//  A command during a skill that cannot be interrupted is dropped: with a used skill whose Skills.txt row has `interrupt` CLEAR
//  (SKILLSFLAGINDEX_INTERRUPT, byte +7 against gdwBitMasks[7]), nothing happens unless the unit stands in mode 1 or 5, PLRMODE_NEUTRAL or
//  PLRMODE_TOWNNEUTRAL. With no used skill, a row that cannot be found, or an interruptible skill, the command goes ahead.
//
//  The command's action is a `switch (nClientCmd)` the compiler built as a byte index table (gnClientCmdClass) and a ten-slot jump table:
//
//      1 Walk -> action 1        2 WalkToEntity -> 0
//      3 Run -> 23               4 RunToEntity -> 24
//      5, 8 -> 21, left skill    6, 7, 9, 10 -> 22, left skill
//      12, 15 -> 21, right       13, 14, 16, 17 -> 22, right
//      19 Interact -> 2          11, 18, anything else: assert "0", :919
//
//  The block is the int[7] of the dispatch (UNITMODE_MoveUnitToCoords): zeroed, [0] and [1] the two arguments; a skill command overwrites [0] with
//  the skill's id and sets [1] its owner GUID and [2], [3] the two arguments again (the original stores [2], [3], [1] in that order after both
//  calls: scheduling, written in the slots' order). A walk or run to a point is dropped when a queued interact is waiting (the player data's
//  +154h, the queued command, equal to 19). An interact with an ITEM that is not UNITFLAG_TARGETABLE (bit 1) is dropped too.
//
//  Then the dispatch, and for a PLAYER its answer decides: 0 skips the rest but the queue clear. Otherwise a skill command bumps the unit's
//  STAT_PIERCE_IDX and every command but 19 is sent: MSG_SendCmdUnit for the unit commands, MSG_SendCmdXY otherwise. 19 is not sent here:
//  UNITMODE_InteractWithUnit sends it. Last, the queued command is cleared.
//
//  The pierce bump and the send are byte-table switches too: the pierce bump's table has 13 bytes, 11 alone on the default slot; the send's has
//  17 bytes and THREE slots, two of them to the same MSG_SendCmdXY (1 and 3 on slot 0, the unit commands on slot 1, 5, 8, 11, 12 and 15 on
//  slot 2, the default) -- a source that named 1 and 3 as cases of their own and left the rest to its default; the C writes it so.
//  The unit's NULL tests after UNITS_GetUsedSkill are dead: D2Common's UNITS_GetUsedSkill asserts the unit and exits.
void __fastcall UNITMODE_PerformClientCommand(int32_t nClientCmd, D2UnitStrc *pUnit, int32_t nTargetTypeOrX, int32_t nTargetGuidOrY, int32_t nUnused)
{
    D2SkillStrc *pUsedSkill;
    D2SkillsTxt *pUsedSkillsTxt;
    D2UnitStrc *pItem;
    int32_t aActionParams[7];
    D2C_ClientActions nAction;
    BOOL bSend;

    pUsedSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (pUsedSkill)
    {
        pUsedSkillsTxt = DATATBLS_GetSkillsTxtRecordById(D2COMMON_SKILLS_GetSkillIdFromSkill(pUsedSkill, szUnitModeCpp, 785));
        if (pUsedSkillsTxt && !(((uint8_t *)pUsedSkillsTxt->dwFlags)[SKILLSFLAGINDEX_INTERRUPT >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_INTERRUPT & 7]))
        {
            if (!pUnit || (pUnit->dwAnimMode != PLRMODE_NEUTRAL && pUnit->dwAnimMode != PLRMODE_TOWNNEUTRAL))
                return;
        }
    }

    memset(aActionParams, 0, sizeof(aActionParams));
    aActionParams[0] = nTargetTypeOrX;
    aActionParams[1] = nTargetGuidOrY;

    switch (nClientCmd)
    {
    case 1:
        nAction = CLTACTION_WALKTOXY;
        if (PLAYER_GetQueuedCommand(pUnit) == 19)
            return;
        break;

    case 3:
        nAction = CLTACTION_RUNTOXY;
        if (PLAYER_GetQueuedCommand(pUnit) == 19)
            return;
        break;

    case 2:
        nAction = CLTACTION_WALKTOUNIT;
        break;

    case 4:
        nAction = CLTACTION_RUNTOUNIT;
        break;

    case 5:
    case 8:
        aActionParams[0] = D2COMMON_SKILLS_GetSkillIdFromSkill(D2COMMON_UNITS_GetLeftSkill(pUnit), szUnitModeCpp, 881);
        aActionParams[1] = (int32_t)D2COMMON_SKILLS_GetOwnerGUIDFromSkill(D2COMMON_UNITS_GetLeftSkill(pUnit));
        aActionParams[2] = nTargetTypeOrX;
        aActionParams[3] = nTargetGuidOrY;
        nAction = CLTACTION_SKILL_XY;
        break;

    case 6:
    case 7:
    case 9:
    case 10:
        aActionParams[0] = D2COMMON_SKILLS_GetSkillIdFromSkill(D2COMMON_UNITS_GetLeftSkill(pUnit), szUnitModeCpp, 891);
        aActionParams[1] = (int32_t)D2COMMON_SKILLS_GetOwnerGUIDFromSkill(D2COMMON_UNITS_GetLeftSkill(pUnit));
        aActionParams[2] = nTargetTypeOrX;
        aActionParams[3] = nTargetGuidOrY;
        nAction = CLTACTION_SKILL_UNIT;
        break;

    case 12:
    case 15:
        aActionParams[0] = D2COMMON_SKILLS_GetSkillIdFromSkill(D2COMMON_UNITS_GetRightSkill(pUnit), szUnitModeCpp, 899);
        aActionParams[1] = (int32_t)D2COMMON_SKILLS_GetOwnerGUIDFromSkill(D2COMMON_UNITS_GetRightSkill(pUnit));
        aActionParams[2] = nTargetTypeOrX;
        aActionParams[3] = nTargetGuidOrY;
        nAction = CLTACTION_SKILL_XY;
        break;

    case 13:
    case 14:
    case 16:
    case 17:
        aActionParams[0] = D2COMMON_SKILLS_GetSkillIdFromSkill(D2COMMON_UNITS_GetRightSkill(pUnit), szUnitModeCpp, 909);
        aActionParams[1] = (int32_t)D2COMMON_SKILLS_GetOwnerGUIDFromSkill(D2COMMON_UNITS_GetRightSkill(pUnit));
        aActionParams[2] = nTargetTypeOrX;
        aActionParams[3] = nTargetGuidOrY;
        nAction = CLTACTION_SKILL_UNIT;
        break;

    case 19:
        if (nTargetTypeOrX == UNIT_ITEM)
        {
            pItem = CUNIT_FindUnitByGuidAndType((uint32_t)nTargetGuidOrY, UNIT_ITEM);
            if (pItem && pItem->dwUnitType == UNIT_ITEM && !((pItem->dwFlags >> 1) & 1))
                return;
        }
        nAction = CLTACTION_INTERACT;
        break;

    default:
        FOG_DisplayAssert("0", szUnitModeCpp, 919);
        exit(-1);
    }

    bSend = TRUE;
    switch (pUnit->dwUnitType)
    {
    case UNIT_PLAYER:
        bSend = PLAYER_DoClientAction(nAction, pUnit, aActionParams, FALSE) != 0;
        break;

    case UNIT_MONSTER:
        MONSTER_DoClientAction(nAction, pUnit, aActionParams);
        break;

    case UNIT_OBJECT:
        OBJECT_DoClientAction(nAction, pUnit, aActionParams);
        break;

    case UNIT_ITEM:
        ITEM_DoClientAction(nAction, pUnit, aActionParams[0], aActionParams[1]);
        break;

    default:
        break;
    }

    if (bSend)
    {
        switch (nClientCmd)
        {
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
            D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_PIERCE_IDX, D2COMMON_STATLIST_GetUnitBaseStat(pUnit, STAT_PIERCE_IDX, 0) + 1, 0);
            break;

        default:
            break;
        }

        if (nClientCmd != 19)
        {
            switch (nClientCmd)
            {
            case 1:
            case 3:
                MSG_SendCmdXY((uint8_t)nClientCmd, (int16_t)nTargetTypeOrX, (int16_t)nTargetGuidOrY);
                break;

            case 2:
            case 4:
            case 6:
            case 7:
            case 9:
            case 10:
            case 13:
            case 14:
            case 16:
            case 17:
                MSG_SendCmdUnit((uint8_t)nClientCmd, nTargetTypeOrX, nTargetGuidOrY);
                break;

            default:
                MSG_SendCmdXY((uint8_t)nClientCmd, (int16_t)nTargetTypeOrX, (int16_t)nTargetGuidOrY);
                break;
            }
        }
    }

    if (pUnit->dwUnitType == UNIT_PLAYER && pUnit->pPlayerData)
    {
        pUnit->pPlayerData->unk0xA8[42] = 0;
        pUnit->pPlayerData->unk0xA8[43] = 0;
        pUnit->pPlayerData->unk0xA8[44] = 0;
        pUnit->pPlayerData->unk0xA8[45] = 0;
    }
}


//1.10f: D2Client.0x6FB4EE00
//  Runs client action nAction (a D2C_ClientActions value) by unit type. The block and bInterruptKnockback are on the stack. PLAYER_DoClientAction
//  for a player, a tail jmp that hands on both stack arguments; MONSTER_ and OBJECT_DoClientAction the block; ITEM_DoClientAction (Item.cpp)
//  its [0] and [1] by value, [1] read first. A missile or any type past ITEM does nothing. void: the other exits load `mov eax, 1`, the register
//  of a void; no caller reads it, and PerformClientCommand's inlined copy reads the PLAYER answer itself.
//
//  The ids are read off the consumers' switches: PLAYER_DoClientAction takes 0, 1, 2, 6..9 and 18..25, MONSTER_DoClientAction 0, 1, 4..24 and
//  26..29, OBJECT_DoClientAction 3 and 21, ITEM_DoClientAction 2. By the unit's mode, a point form and a target form each: WALK 1 / 0, RUN 23 / 24,
//  CAST 4 / 5, ATTACK1 11 / 10, ATTACK2 17 / 16, SKILL1..4 12 / 13, 14 / 15, 26 / 27, 28 / 29, a player's attack or skill 21 / 22. A number
//  keeps one name where the unit types read it differently: 2 is a player's interaction and an item's mode, 21 a skill at a point and an
//  object operated.
void __fastcall UNITMODE_DispatchActionByUnitType(D2C_ClientActions nAction, D2UnitStrc *pUnit, void *pActionParams, BOOL bInterruptKnockback)
{
    switch (pUnit->dwUnitType)
    {
    case UNIT_PLAYER:
        PLAYER_DoClientAction(nAction, pUnit, (int32_t *)pActionParams, bInterruptKnockback);
        return;

    case UNIT_MONSTER:
        MONSTER_DoClientAction(nAction, pUnit, (int32_t *)pActionParams);
        return;

    case UNIT_OBJECT:
        OBJECT_DoClientAction(nAction, pUnit, (int32_t *)pActionParams);
        return;

    case UNIT_ITEM:
        ITEM_DoClientAction(nAction, pUnit, ((int32_t *)pActionParams)[0], ((int32_t *)pActionParams)[1]);
        return;

    default:
        return;
    }
}


//1.10f: D2Client.0x6FB4EE70
//  Runs the player's queued command. Its one caller is GAME_UpdateFrame. The queue is four dwords of the player data, unk0xA8[42..45]
//  (+150h..+15Ch): pending, the command, the target's type, its GUID. The original inlines PLAYER_GetQueuedCommand, PLAYER_GetQueuedTargetType,
//  PLAYER_GetQueuedTargetGuid and PLAYER_ClearQueuedCommand here, each read behind its own test of the unit's type though the function has
//  already returned for any other type.
//
//  The target is looked up, the queue cleared, and a queued interact with a UNIT_TILE throttles the next interaction 500 ticks on
//  (GAME_SetInteractThrottle and GAME_GetFrameTick). With no target or no command nothing more happens. Then by the queued type:
//
//      PLAYER    dead: the command re-issued with type 0 if this player may loot the corpse (ROSTER_CanUnitLootFrom), else the selection
//                cleared; alive: it must be UNITFLAG_TARGETABLE (bit 1)
//      MONSTER   dead: the command must be a skill command, 5..11 the left skill and 12..18 the right, whose Skills.txt row has `targetcorpse`
//                (SKILLSFLAGINDEX_TARGETCORPSE, byte +7 against gdwBitMasks[0]); alive: TARGETABLE
//      OBJECT    MONSTER_TestFlag(target, 2)
//      any other MONSTER_TestFlag(target, 2), and no walk
//
//  A player, a monster or an object the unit must first walk to (UNITMODE_MustMoveToTarget) is walked to with the command queued again
//  (UNITMODE_MoveToTargetAndQueueCommand); otherwise the command is re-issued, UNITMODE_PerformClientCommand. Every refusal clears the
//  selection, SELECT_SetSelectedUnit(NULL), a tail jmp.
void __fastcall UNITMODE_RunQueuedCommand(D2UnitStrc *pUnit)
{
    D2PlayerDataStrc *pPlayerData;
    D2UnitStrc *pTarget;
    D2SkillStrc *pQueuedSkill;
    D2SkillsTxt *pSkillsTxt;
    int32_t nUnitType;
    int32_t nQueuedCmd;
    int32_t nQueuedTargetType;
    uint32_t nQueuedTargetGUID;

    nQueuedTargetGUID = 0;
    if (!pUnit)
        return;

    nUnitType = pUnit->dwUnitType;
    if (nUnitType != UNIT_PLAYER)
        return;

    pPlayerData = pUnit->pPlayerData;
    if (!pPlayerData || !pPlayerData->unk0xA8[42])
        return;

    nQueuedCmd = PLAYER_GetQueuedCommand(pUnit);

    nQueuedTargetType = PLAYER_GetQueuedTargetType(pUnit);

    nQueuedTargetGUID = (uint32_t)PLAYER_GetQueuedTargetGuid(pUnit);

    pTarget = CUNIT_FindUnitByGuidAndType(nQueuedTargetGUID, (D2C_UnitTypes)nQueuedTargetType);

    PLAYER_ClearQueuedCommand(pUnit);

    if (nQueuedCmd == 0x13 && nQueuedTargetType == UNIT_TILE)
        GAME_SetInteractThrottle(GAME_GetFrameTick() + 500);

    if (!pTarget || !nQueuedCmd)
        return;

    switch (nQueuedTargetType)
    {
    case UNIT_PLAYER:
        if (CUNIT_IsUnitDead(pTarget))
        {
            if (!ROSTER_CanUnitLootFrom(pUnit->dwUnitId, nQueuedTargetGUID))
            {
                SELECT_SetSelectedUnit(NULL);
                return;
            }
            UNITMODE_PerformClientCommand(nQueuedCmd, pUnit, UNIT_PLAYER, (int32_t)nQueuedTargetGUID, 0);
            return;
        }

        if (!((pTarget->dwFlags >> 1) & 1))
        {
            SELECT_SetSelectedUnit(NULL);
            return;
        }

        if (UNITMODE_MustMoveToTarget(pUnit, pTarget, nQueuedCmd))
        {
            UNITMODE_MoveToTargetAndQueueCommand(pUnit, nQueuedCmd, UNIT_PLAYER, (int32_t)nQueuedTargetGUID);
            return;
        }
        break;

    case UNIT_MONSTER:
        if (CUNIT_IsUnitDead(pTarget))
        {
            switch (nQueuedCmd)
            {
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
                pQueuedSkill = D2COMMON_UNITS_GetLeftSkill(pUnit);
                break;

            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
                pQueuedSkill = D2COMMON_UNITS_GetRightSkill(pUnit);
                break;

            default:
                SELECT_SetSelectedUnit(NULL);
                return;
            }

            if (!pQueuedSkill)
            {
                SELECT_SetSelectedUnit(NULL);
                return;
            }

            pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(D2COMMON_SKILLS_GetSkillIdFromSkill(pQueuedSkill, szUnitModeCpp, 1172));
            if (!pSkillsTxt)
            {
                SELECT_SetSelectedUnit(NULL);
                return;
            }

            if (!(((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_TARGETCORPSE >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_TARGETCORPSE & 7]))
            {
                SELECT_SetSelectedUnit(NULL);
                return;
            }
        }
        else if (!((pTarget->dwFlags >> 1) & 1))
        {
            SELECT_SetSelectedUnit(NULL);
            return;
        }

        if (UNITMODE_MustMoveToTarget(pUnit, pTarget, nQueuedCmd))
        {
            UNITMODE_MoveToTargetAndQueueCommand(pUnit, nQueuedCmd, UNIT_MONSTER, (int32_t)nQueuedTargetGUID);
            return;
        }
        break;

    case UNIT_OBJECT:
        if (!MONSTER_TestFlag(pTarget, UNITFLAG_TARGETABLE))
        {
            SELECT_SetSelectedUnit(NULL);
            return;
        }

        if (UNITMODE_MustMoveToTarget(pUnit, pTarget, nQueuedCmd))
        {
            UNITMODE_MoveToTargetAndQueueCommand(pUnit, nQueuedCmd, UNIT_OBJECT, (int32_t)nQueuedTargetGUID);
            return;
        }
        break;

    default:
        if (!MONSTER_TestFlag(pTarget, UNITFLAG_TARGETABLE))
        {
            SELECT_SetSelectedUnit(NULL);
            return;
        }
        break;
    }

    UNITMODE_PerformClientCommand(nQueuedCmd, pUnit, nQueuedTargetType, (int32_t)nQueuedTargetGUID, 0);
}


//1.10f: D2Client.0x6FB4F120
//  Whether the unit must walk before the command can run. TRUE sends RunQueuedCommand, its one caller, to walk to the target with the command
//  queued again. nClientCmd is the one stack argument.
//
//  Commands 6 and 13 (LeftSkillOnUnit and RightSkillOnUnit, none of their shift or hold variants) look at the skill first: when its Skills.txt
//  row has `srvdofunc` 19 (cmp word ptr [eax+2Eh], 13h), the answer is PLAYER_IsUnitInAuraRange's, turned to 0 or 1 (neg / sbb / neg). Stock
//  Skills.txt gives srvdofunc 19 to Inferno (41) and Arctic Blast (230), both ranged by `aurarangecalc`. The range comes back in nRange, and the
//  distance is written into the stack slot of nClientCmd itself (lea eax, [esp+10h+nClientCmd]), which the C passes as &nClientCmd.
//  BUG: that arm answers backwards. PLAYER_IsUnitInAuraRange answers `nDistance <= nRange`, TRUE when the target is IN range, and TRUE here
//  means "walk first": for these two skills a target in range is walked to and one out of range is cast at, the reverse of the two arms below.
//  Reproduced. REACHABLE: the queued 6 or 13 gets here with one of the two skills behind it when the walk after PLAYER_DispatchSkillClick's own
//  queue is not sent (no route, or PerformClientCommand's interrupt gate), when the skill is changed after that function's other queue is armed,
//  and from this function's own TRUE, whose walk (UNITMODE_MoveToTargetAndQueueCommand) queues the command again for the next tick.
//
//  Otherwise: an OBJECT must be in interact range (D2Common #11307) and have nothing between (UNITS_TestCollisionBetweenInteractingUnits,
//  COLLIDE_DOOR | COLLIDE_MISSILE_BARRIER, `push 804h`), or the unit walks; the answer when neither holds is the collision test's 0, left in eax.
//  Any other target is the melee-range test negated, its range bonus whether the TARGET is moving (UNITS_IsInMovingModeEx), neg / sbb / inc.
BOOL __fastcall UNITMODE_MustMoveToTarget(D2UnitStrc *pUnit, D2UnitStrc *pTarget, int32_t nClientCmd)
{
    D2SkillStrc *pCmdSkill;
    D2SkillsTxt *pSkillsTxt;
    int32_t nRange;

    if (nClientCmd == 6)
        pCmdSkill = D2COMMON_UNITS_GetLeftSkill(pUnit);
    else if (nClientCmd == 13)
        pCmdSkill = D2COMMON_UNITS_GetRightSkill(pUnit);
    else
        pCmdSkill = NULL;

    if (pCmdSkill)
    {
        pSkillsTxt = D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill(pCmdSkill);
        if (pSkillsTxt && pSkillsTxt->wSrvDoFunc == 19)
            return PLAYER_IsUnitInAuraRange(pCmdSkill, pTarget, &nRange, &nClientCmd) != 0;
    }

    if (pTarget && pTarget->dwUnitType == UNIT_OBJECT)
    {
        if (!D2COMMON_UNITS_IsObjectInInteractRange(pUnit, pTarget))
            return TRUE;
        if (D2COMMON_UNITS_TestCollisionBetweenInteractingUnits(pUnit, pTarget, COLLIDE_DOOR | COLLIDE_MISSILE_BARRIER))
            return TRUE;
        return FALSE;
    }

    return !D2COMMON_UNITS_IsInMeleeRange(pUnit, pTarget, D2COMMON_UNITS_IsInMovingModeEx(pTarget));
}


//1.10f: D2Client.0x6FB4F1D0
//  Walks to a target with the command queued. The walk is a client command of its own through UNITMODE_PerformClientCommand -- 4, RunToEntity,
//  when bit 3 of GAME_GetMovementFlags is set (the Run key or the run lock), else 2, WalkToEntity -- and then the queue in the player data is
//  armed: pending 1, the command, the target's type and GUID (+150h..+15Ch, each store re-reading pPlayerData). The original inlines
//  PLAYER_SetQueuedCommand here. RunQueuedCommand, its one caller, reads the queue back on a later frame.
void __fastcall UNITMODE_MoveToTargetAndQueueCommand(D2UnitStrc *pUnit, int32_t nQueuedCmd, int32_t nTargetType, int32_t nTargetGUID)
{
    UNITMODE_PerformClientCommand((GAME_GetMovementFlags() & 8) ? 4 : 2, pUnit, nTargetType, nTargetGUID, 0);

    PLAYER_SetQueuedCommand(nQueuedCmd, pUnit, nTargetType, (D2UnitGUID)nTargetGUID);
}


//1.10f: D2Client.0x6FB4F240
//  Plays a hit's overlay on a hit mode. The hit modes: PLRMODE_DEATH, GETHIT and KNOCKBACK (0, 4, 19) for a player, MONMODE_DEATH, GETHIT and
//  KNOCKBACK (0, 3, 13) for a monster, none for any other type; the type is read NULL-safe (`pUnit ? type : 6`). UNITMODE_FlushServerMemoryProbe
//  runs next, hit mode or not. Then, in a hit mode, when dwLastHitClass (+0B0h) has both a high nibble and a low one (and ecx, 0F0h; test al, 0Fh),
//  the high nibble is looked up in the seven pairs of gnHitClassOverlay and its overlay added once (GFX_AddOverlay, OVERLAYTYPE_ONCE). The walk
//  does not stop at a match: every pair is compared and the last match wins; the keys are distinct, so it is the only match.
void __fastcall UNITMODE_PlayHitOverlayForMode(D2UnitStrc *pUnit, int32_t nMode)
{
    int32_t nUnitType;
    BOOL bHitMode;
    uint32_t nLastHitClass;
    int32_t nOverlayId;
    int32_t i;

    nUnitType = pUnit ? (int32_t)pUnit->dwUnitType : UNIT_TYPES_COUNT;
    bHitMode = FALSE;
    if (nUnitType == UNIT_PLAYER)
    {
        if (nMode == PLRMODE_DEATH || nMode == PLRMODE_GETHIT || nMode == PLRMODE_KNOCKBACK)
            bHitMode = TRUE;
    }
    else if (nUnitType == UNIT_MONSTER)
    {
        if (nMode == MONMODE_DEATH || nMode == MONMODE_GETHIT || nMode == MONMODE_KNOCKBACK)
            bHitMode = TRUE;
    }

    UNITMODE_FlushServerMemoryProbe();

    if (!bHitMode)
        return;

    nLastHitClass = pUnit->dwLastHitClass;
    if (!(nLastHitClass & 0xF0) || !(nLastHitClass & 0x0F))
        return;

    nOverlayId = -1;
    for (i = 0; i < 7; ++i)
    {
        if (gnHitClassOverlay[i][0] == (int32_t)(nLastHitClass & 0xF0))
            nOverlayId = gnHitClassOverlay[i][1];
    }

    if (nOverlayId != -1)
        GFX_AddOverlay(pUnit, (D2_OverlayIds)nOverlayId, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);
}


//  The server's address obfuscation, and the client's answer's: the low word kept, the high word XORed with it. The original computes it in place
//  four times with no function at an address; the macro spells the one expression.
#define UNITMODE_PROBE_OBFUSCATE(n) (((n) & 0xFFFF) | ((((uint32_t)(n) >> 16) ^ ((n) & 0xFFFF)) << 16))

//1.10f: D2Client.0x6FB4F2D0
//  Answers the server's memory probe. This is the unit's one function with an SEH frame. Its one caller is UNITMODE_PlayHitOverlayForMode, so it
//  runs on the next mode change after the probe arrives.
//
//  SCmd.cpp's packet 0xAD handler parks two obfuscated dwords in gnServerProbeAddr1 and gnServerProbeAddr2. With either set, each is turned into
//  an address (the macro above; the original re-reads the high word from the global, where the C reads the dword once), both are cleared, and --
//  only in a Battle.net game, gnGameType GAMETYPE_BNET_BETA or GAMETYPE_BNET -- the two dwords AT those addresses are read inside `__try`: a NULL
//  address answers 0 without the read, and a read that faults answers 0x5A5A5A5A (the two `__except` blocks, their filters `mov eax, 1`,
//  EXCEPTION_EXECUTE_HANDLER). Both answers go back obfuscated the same way as client packet 0x64 through MSG_SendCmdUnit.
//  BUG: the server names two addresses in the client's process and reads back what is there; in this rewrite that is what the original
//  D2Client.dll mapped at its own base still holds there, not what the rewrite keeps in its own storage. INERT: against any server but Battle.net's.
void __cdecl UNITMODE_FlushServerMemoryProbe()
{
    uint32_t *pProbe1;
    uint32_t *pProbe2;
    uint32_t nAddr;
    uint32_t nProbe1;
    uint32_t nProbe2;

    if (!gnServerProbeAddr1 && !gnServerProbeAddr2)
        return;

    nAddr = gnServerProbeAddr1;
    pProbe1 = (uint32_t *)UNITMODE_PROBE_OBFUSCATE(nAddr);
    nAddr = gnServerProbeAddr2;
    pProbe2 = (uint32_t *)UNITMODE_PROBE_OBFUSCATE(nAddr);
    gnServerProbeAddr1 = 0;
    gnServerProbeAddr2 = 0;

    if (gnGameType != GAMETYPE_BNET_BETA && gnGameType != GAMETYPE_BNET)
        return;

    if (pProbe1)
    {
        __try
        {
            nProbe1 = *pProbe1;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            nProbe1 = 0x5A5A5A5A;
        }
    }
    else
    {
        nProbe1 = 0;
    }

    if (pProbe2)
    {
        __try
        {
            nProbe2 = *pProbe2;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            nProbe2 = 0x5A5A5A5A;
        }
    }
    else
    {
        nProbe2 = 0;
    }

    MSG_SendCmdUnit(0x64, (int32_t)UNITMODE_PROBE_OBFUSCATE(nProbe1), (int32_t)UNITMODE_PROBE_OBFUSCATE(nProbe2));
}


//1.10f: D2Client.0x6FB4F410
//  Sets a unit's animation mode. MEMPART_Nullsub1 is called first, as the original does. UNITFLAG_SKSRVDOFUNC (bit 6) is cleared on a unit that
//  is not NULL (`and al, 0BFh` on the dword read, stored whole), and D2Common's UNITS_ChangeAnimMode (#10348) gets the unit untested; it tests
//  it itself, NULL and UNIT_TILE to one exit, so a NULL unit changes nothing. Only a mode that changed rebuilds the unit's animation
//  (GFX_RebuildUnitAnimForMode), plays the hit overlay and flushes the probe (UNITMODE_PlayHitOverlayForMode) and plays the mode's sound
//  (UNITSND_PlayModeSound, bModeGiven 0).
void __fastcall UNITMODE_SetAnimMode(D2UnitStrc *pUnit, int32_t nMode)
{
    MEMPART_Nullsub1();
    if (pUnit)
        pUnit->dwFlags &= ~(uint32_t)UNITFLAG_SKSRVDOFUNC;

    if (!D2COMMON_UNITS_ChangeAnimMode(pUnit, nMode))
        return;

    GFX_RebuildUnitAnimForMode(pUnit, 0);
    UNITMODE_PlayHitOverlayForMode(pUnit, nMode);
    UNITSND_PlayModeSound(pUnit, nMode, 0);
}


//1.10f: D2Client.0x6FB4F460
//  Sets a unit's animation mode without the overlay and the sound: the same MEMPART_Nullsub1, the same flag clear and the same ChangeAnimMode as
//  UNITMODE_SetAnimMode, the rebuild a tail jmp. Its one caller is MONSTER_DoClientAction.
void __fastcall UNITMODE_SetAnimModeSilent(D2UnitStrc *pUnit, int32_t nMode)
{
    MEMPART_Nullsub1();
    if (pUnit)
        pUnit->dwFlags &= ~(uint32_t)UNITFLAG_SKSRVDOFUNC;

    if (D2COMMON_UNITS_ChangeAnimMode(pUnit, nMode))
        GFX_RebuildUnitAnimForMode(pUnit, 0);
}

#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "CoreArchive.h"
#include "Select.h"

//  Select.cpp -- the unit selection of D2Client.dll 1.10: the unit under the cursor, picked once a frame, and the lock that keeps it.
//  The original source file is GAME\Select.cpp (three FOG_DisplayWarning calls push its path). Address range 0x6FAB5A20..0x6FAB6AB9.
//
//  The selection is kept in globals:
//      gbHasSelectedUnit   a unit is selected, and gnSelectedUnitType and gnSelectedUnitGUID name it -- a type and an id, never a pointer,
//                          so a freed unit cannot be reached through the selection
//      gbSelectionLocked   while set, SELECT_ApplySelectedUnit and SELECT_UpdateSelectedUnit return at once; the window handlers clear it
//                          (SELECT_UnlockSelection)
//  The file also holds the character panel's two reference monsters (gnToHitRefMonster, gnToBeHitRefMonster) and the skill-target helpers
//  (SELECT_SkillsResolveClientTarget and the enemy probe).

//  The source file, as the warning at 0x6FAB5B39 hands it to FOG_DisplayWarning.
static const char szSourceFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\GAME\\Select.cpp";

//  The call the warning quotes; UnitTestSelect is CUNIT_HitTestUnit.
static const char szUnitTestSelect[] = "UnitTestSelect (hUnit, 0, 0, FALSE)";

//  The door throttle's debug line.
static const char szMsgAt[] = "msg at %d\n";

//  The flag indices SELECT_ApplySelectedUnit's monster arm carries as literals: monstats bit 8 (NPC), the alignment, monstats2 bits 9 and 17.
static_assert(MONSTATSFLAGINDEX_NPC == 8 && MONSTATS2FLAGINDEX_INERT == 17
              && MONALIGN_GOOD == 1,
              "SELECT_ApplySelectedUnit's monstats bit 8, monstats2 bit 17 "
              "and alignment 1");
static_assert(MONSTER_FALLEN1 == 19,
              "the reference monsters' initial value in the image");
static_assert(OBJMODE_SPECIAL2 == 4,
              "`cmp dword ptr [esi+10h], 4` at 0x6FAB5C30");

//1.10f: D2Client.0x6FAB5A20
//  Returns the selected unit, or NULL; a stale selection is dropped on the way.
//  The selection is re-found from its type and id on every call, then asked CUNIT_HitTestUnit(pUnit, 0, 0, 0) again -- with bTestGeometry zero
//  that is "may this unit be selected at all", not "is it under the cursor". Either failing clears the lock and the selection, in that order.
D2UnitStrc *__cdecl SELECT_GetSelectedUnit()
{
    D2UnitStrc *pSelected;

    if (gbHasSelectedUnit)
    {
        pSelected = CUNIT_FindUnitByGuidAndType((uint32_t)gnSelectedUnitGUID, (D2C_UnitTypes)gnSelectedUnitType);
        if (pSelected && CUNIT_HitTestUnit(pSelected, 0, 0, 0))
            return pSelected;

        gbSelectionLocked = 0;
        gbHasSelectedUnit = 0;
    }
    return NULL;
}

//1.10f: D2Client.0x6FAB5A70
//  The character panel's to-hit reference monster: a monstats class id, 19 (MONSTER_FALLEN1) after SELECT_ResetRefMonsters, and whatever
//  qualifying monster SELECT_ApplySelectedUnit last selected.
int32_t __cdecl SELECT_GetToHitRefMonster()
{
    return gnToHitRefMonster;
}

//1.10f: D2Client.0x6FAB5A80
//  The to-be-hit reference monster, the twin of SELECT_GetToHitRefMonster.
int32_t __cdecl SELECT_GetToBeHitRefMonster()
{
    return gnToBeHitRefMonster;
}

//1.10f: D2Client.0x6FAB5A90
//  Selects pUnit and locks the selection; with NULL, locks "nothing".
//      a selectable unit    unlock, apply, lock
//      one that is not      unlock, clear the selection
//      NULL                 clear the selection, lock
//  The unlock is stored after the test's call and before its branch, on both arms; SELECT_ApplySelectedUnit returns at once while the lock
//  is set, so it has to come first.
void __fastcall SELECT_SetSelectedUnit(D2UnitStrc *pUnit)
{
    BOOL bSelectable;

    if (pUnit)
    {
        bSelectable = CUNIT_HitTestUnit(pUnit, 0, 0, 0);
        gbSelectionLocked = 0;
        if (bSelectable)
        {
            SELECT_ApplySelectedUnit(pUnit);
            gbSelectionLocked = 1;
        }
        else
        {
            gbHasSelectedUnit = 0;
        }
    }
    else
    {
        gbHasSelectedUnit = 0;
        gbSelectionLocked = 1;
    }
}

//1.10f: D2Client.0x6FAB5AE0
//  Makes pUnit the selection, or with NULL drops it. __fastcall: the callers load ecx and push nothing.
//  1. Nothing while the lock is set.
//  2. If a popup is up and belongs to pUnit, re-point it and stop; else drop the popup's owner -- for NULL too.
//  3. pUnit is checked with CUNIT_HitTestUnit(pUnit, 0, 0, 0); a failure is only a warning (FOG_DisplayWarning) with no exit after it,
//     so the function goes on and selects the unit anyway.
//  4. bAlreadySelected: pUnit is the selection already (type and id). Otherwise, if the old selection is a TILE, its warp room is
//     deselected -- found again by id, D2Common #10092, the view marked.
//  5. NULL clears gbHasSelectedUnit and returns. A unit becomes the selection, and then by type:
//        UNIT_MONSTER  a new one may become the character panel's reference
//        UNIT_OBJECT   a door not in mode 4 tells the server, at most every 500 ms per door
//        UNIT_TILE     a new one has its warp room selected, #10091
//  The reference monsters: a monster qualifies when its monstats row is not an NPC (bit 8), is not MONALIGN_GOOD, and its monstats2 row has
//  isAtt (bit 9); it becomes the to-hit reference, and the to-be-hit reference as well unless monstats2 has inert (bit 17). The two stores
//  are in the original's order, to-be-hit first.
//  The door message is packet 0x3D, {u8 0x3D, u32 guid}, sent through MSG_SendServerItemMessage. The throttle keeps the send time in the
//  door's own dwTickCount, and every send also writes "msg at %d\n" with the tick to the debugger (OutputDebugStringA) -- a development
//  line left in the shipped client. The compare is unsigned, so a tick count that wraps still waits 500 ms.
//  The original inlines DATATBLS_GetMonStatsTxtRecord and DATATBLS_GetMonStats2TxtRecord here.
void __fastcall SELECT_ApplySelectedUnit(D2UnitStrc *pUnit)
{
    BOOL bAlreadySelected;
    D2UnitStrc *pOldTile;
    D2C_Levels nOldLevelId;
    D2C_Levels nLevelId;
    int32_t nClassId;
    int32_t nMonStatsEx;
    D2MonStatsTxt *pMonStatsTxt;
    D2MonStats2Txt *pMonStats2Txt;
    uint32_t dwNow;
    char szMsg[512];

    if (gbSelectionLocked)
        return;

    //  BUG: TEXT_GetPopupSourceUnit answers NULL when the popup has no source unit or it is gone, so with pUnit NULL an active popup takes this
    //  arm: TEXT_PopupSetOwnerUnit(NULL) clears the owner, and the return skips the clear of gbHasSelectedUnit the NULL call was made for.
    //  REACHABLE: the quest log's replayed speech is such a popup (it never names a source), so a selection made outside the gates survives
    //  them while it is up.
    if (TEXT_IsPopupActive() && TEXT_GetPopupSourceUnit() == pUnit)
    {
        TEXT_PopupSetOwnerUnit(pUnit);
        return;
    }

    TEXT_PopupClearOwnerUnit();

    bAlreadySelected = FALSE;
    if (pUnit)
    {
        if (!CUNIT_HitTestUnit(pUnit, 0, 0, 0))
        {
            FOG_DisplayWarning(szUnitTestSelect, szSourceFile, 79);
        }

        if (gbHasSelectedUnit && pUnit->dwUnitType == gnSelectedUnitType && pUnit->dwUnitId == gnSelectedUnitGUID)
        {
            bAlreadySelected = TRUE;
        }
    }

    //  The old selection's tile, deselected. For a non-NULL pUnit with no selection the original jumps straight past this; the test of
    //  gbHasSelectedUnit here gives the same answer on that path.
    if (!bAlreadySelected && gbHasSelectedUnit && gnSelectedUnitType == UNIT_TILE)
    {
        pOldTile = CUNIT_FindUnitByGuidAndType((uint32_t)gnSelectedUnitGUID, UNIT_TILE);
        if (pOldTile)
        {
            //  The level id is loaded and pushed before UNITS_GetRoom is called.
            //  BUG: the room goes to D2Common untested, here and in the UNIT_TILE arm, and both exports assert it and exit. LATENT: a live
            //  tile always carries the room it was created in.
            nOldLevelId = (D2C_Levels)pOldTile->dwClassId;
            D2COMMON_DUNGEON_UpdateWarpRoomDeselect(D2COMMON_UNITS_GetRoom(pOldTile), nOldLevelId);
            VIEW_MarkDirty(gpView);
        }
    }

    if (!pUnit)
    {
        gbHasSelectedUnit = 0;
        return;
    }

    gnSelectedUnitType = pUnit->dwUnitType;
    gnSelectedUnitGUID = pUnit->dwUnitId;
    gbHasSelectedUnit = 1;

    switch (gnSelectedUnitType)
    {
    case UNIT_MONSTER:
        if (bAlreadySelected)
            break;

        nClassId = pUnit->dwClassId;

        pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(nClassId);
        if (!pMonStatsTxt)
            break;

        if (nClassId < 0 || nClassId >= sgptDataTables->nMonStatsTxtRecordCount)
            break;
        nMonStatsEx = (int16_t)sgptDataTables->pMonStatsTxt[nClassId].wMonStatsEx;
        if (nMonStatsEx < 0 || nMonStatsEx >= sgptDataTables->nMonStats2TxtRecordCount)
            break;
        pMonStats2Txt = &sgptDataTables->pMonStats2Txt[nMonStatsEx];
        if (!pMonStats2Txt)
            break;

        if (pMonStatsTxt->nMonStatsFlags[MONSTATSFLAGINDEX_NPC >> 3] & (uint8_t)gdwBitMasks[MONSTATSFLAGINDEX_NPC & 7])
            break;
        if (pMonStatsTxt->nAlign == MONALIGN_GOOD)
            break;
        if (!(pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_ISATT >> 3] & (uint8_t)gdwBitMasks[MONSTATS2FLAGINDEX_ISATT & 7]))
            break;

        if (!(pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_INERT >> 3] & (uint8_t)gdwBitMasks[MONSTATS2FLAGINDEX_INERT & 7]))
            gnToBeHitRefMonster = nClassId;
        gnToHitRefMonster = nClassId;
        break;

    case UNIT_OBJECT:
        if (!D2COMMON_UNITS_IsDoor(pUnit))
            break;
        if (pUnit->dwAnimMode == OBJMODE_SPECIAL2)
            break;

        dwNow = GetTickCount();
        if (dwNow - pUnit->dwTickCount <= 500)
            break;

        sprintf(szMsg, szMsgAt, (int32_t)dwNow);
        OutputDebugStringA(szMsg);
        pUnit->dwTickCount = dwNow;
        MSG_SendServerItemMessage(0x3D, gnSelectedUnitGUID);
        break;

    case UNIT_TILE:
        if (bAlreadySelected)
            break;

        nLevelId = (D2C_Levels)pUnit->dwClassId;
        D2COMMON_DUNGEON_UpdateWarpRoomSelect(D2COMMON_UNITS_GetRoom(pUnit), nLevelId);
        VIEW_MarkDirty(gpView);
        break;
    }
}

//1.10f: D2Client.0x6FAB5D40
//  Clears the selection and its lock.
//  void: eax is left 0 by the `xor eax, eax` kept for the two stores, and no caller reads it.
void __cdecl SELECT_ClearSelectedUnit()
{
    gbSelectionLocked = 0;
    gbHasSelectedUnit = 0;
}

//1.10f: D2Client.0x6FAB5D50
//  Releases the lock. The window message handlers call it (the mouse buttons' press and release, GAME_WindowOnActivateApp), so any click,
//  or losing focus, lets the next frame's scan pick a selection again.
void __cdecl SELECT_UnlockSelection()
{
    gbSelectionLocked = 0;
}

//  A door's axis: the objects.txt Orientation byte.
static_assert(offsetof(D2ObjectsTxt, nOrientation) == 0x13C,
              "objects.txt's Orientation, read as a byte by the door arms");

//1.10f: D2Client.0x6FAB5D60
//  Picks the unit under the cursor, once a frame. Nothing while the lock is set. Each of these clears the selection
//  (SELECT_ApplySelectedUnit(NULL)): both side panels open; the cursor below gnScreenHeight - 49; the cursor inside the click region
//  SELECT_SetClickRegion keeps, all four edges exclusive; the cursor on the right half with PANEL_RIGHT open or on the left half with
//  PANEL_LEFT. Every compare is signed.
//  The cursor is then undone out of the perspective view with the same arithmetic SCROLL_ScreenToGameCoords makes, and moved into client
//  space through the adjusted viewport getters -- so, unlike that function, Y includes GetViewportOriginY's -8.
//  The old selection is re-found by type and id. If it can no longer be selected at all it is cleared, lock and flag; if it is still under
//  the cursor (and is not an item under /nopickup) it is simply applied again and the scan is skipped. Otherwise it is remembered, and in
//  the scan below it wins its room outright if it is hit and not dead -- the break ends that room's walk, not the scan, so a later room
//  can still replace it.
//  The scan: the player's room and its neighbours (D2Common #10035), every unit in each, skipping missiles, the player, and anything
//  CUNIT_HitTestUnit (geometry on) does not put under the cursor. A room's unit list that links back to the unit just seen ends the room,
//  and more than 30000 units in one room ends it with a warning, "Far too many units on screen"; both guard against a corrupt list.
//  BUG: missiles are skipped before either guard and are not counted, so a missile linked to itself would hold the loop forever. LATENT:
//  D2Common refuses the one insert that makes such a link (UNITROOM_AddUnitToRoomEx's assert), so it needs a list corrupted some other way.
//  The best unit is kept by priority -- gnUnitTypeSelectPriority, {0, 0, -6, 0, -4, 0} by unit type -- with a dead player raised to 1 and a
//  dead monster lowered to -8, and a monster in STATE_ATTACHED skipped. Higher wins; a tie goes to the unit strictly nearer the player in
//  client space.
//  Doors break that rule, and not symmetrically:
//      the CANDIDATE is a closed door   it competes on priority only if it lies strictly between the best unit and the player on the
//                                       door's axis; otherwise it is skipped
//      the BEST is a closed door        it is kept only if it lies strictly between the candidate and the player; otherwise the
//                                       candidate replaces it WITHOUT a priority compare
//  "Closed" is dwAnimMode 0; a door in any other mode is skipped as a candidate and, as the best, makes every non-object candidate be
//  skipped. The axis is objects.txt's Orientation (1: Y, else X), and every one of those coordinate compares is unsigned on D2CoordStrc's
//  int fields -- written with casts.
//  A warp under a walking player: a tile whose select box holds both the player and the cursor, while the player is moving and the left
//  button is down, is skipped while the interact throttle is pending, and the throttle is pushed 500 ms on each such frame.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __cdecl SELECT_UpdateSelectedUnit()
{
    D2C_UIpanels nPanel;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nHalfWidth;
    int32_t nScreenX;
    int32_t nScreenY;
    D2UnitStrc *pOldSelection;
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    int32_t nPlayerX;
    int32_t nPlayerY;
    D2ActiveRoomStrc **ppRoomList;
    int32_t nNumRooms;
    int32_t nRoom;
    D2UnitStrc *pBest;
    D2UnitStrc *pUnit;
    D2UnitStrc *pPrevUnit;
    D2UnitStrc *pSomePlayer;
    int32_t nUnitCount;
    int32_t nType;
    int32_t nBestType;
    int32_t nPriority;
    int32_t nBestPriority;
    uint32_t dwThrottle;
    D2ObjectsTxt *pObjectsTxt;
    D2CoordStrc tPlayer;
    D2CoordStrc tUnit;
    D2CoordStrc tBest;
    BOOL bBetween;
    int32_t nDxUnit;
    int32_t nDyUnit;
    int32_t nDxBest;
    int32_t nDyBest;
    int32_t nCoord;

    if (gbSelectionLocked)
        return;

    nPanel = SCROLL_GetUiShiftScreen();
    if (nPanel == PANEL_BOTH)
    {
        SELECT_ApplySelectedUnit(NULL);
        return;
    }

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    if (nMouseY > gnScreenHeight - 49)
    {
        SELECT_ApplySelectedUnit(NULL);
        return;
    }

    if (gbClickRegionValid && nMouseX > gnClickRegionLeft && nMouseX < gnClickRegionRight && nMouseY > gnClickRegionTop && nMouseY < gnClickRegionBottom)
    {
        SELECT_ApplySelectedUnit(NULL);
        return;
    }

    if (nPanel == PANEL_RIGHT)
    {
        if (nMouseX >= gnScreenWidth / 2)
        {
            SELECT_ApplySelectedUnit(NULL);
            return;
        }
    }
    else if (nPanel == PANEL_LEFT)
    {
        if (nMouseX < gnScreenWidth / 2)
        {
            SELECT_ApplySelectedUnit(NULL);
            return;
        }
    }

    if (D2GFX_CheckPerspective())
    {
        nMouseY = nMouseY + nMouseY / -40;
        nHalfWidth = gnScreenWidth / 2;
        nMouseX += (40 - nMouseY / 4) * (nMouseX - nHalfWidth) / nHalfWidth;
    }

    nScreenX = nMouseX + SCROLL_GetViewportOriginX();
    nScreenY = nMouseY + SCROLL_GetViewportOriginY();

    pOldSelection = NULL;
    if (gbHasSelectedUnit)
    {
        pUnit = CUNIT_FindUnitByGuidAndType((uint32_t)gnSelectedUnitGUID, (D2C_UnitTypes)gnSelectedUnitType);
        if (pUnit && CUNIT_HitTestUnit(pUnit, 0, 0, 0))
        {
            pOldSelection = pUnit;
            if (CUNIT_HitTestUnit(pUnit, nScreenX, nScreenY, 1) && !(pUnit->dwUnitType == UNIT_ITEM && gbNoPickup))
            {
                SELECT_ApplySelectedUnit(pUnit);
                return;
            }
        }
        else
        {
            gbSelectionLocked = 0;
            gbHasSelectedUnit = 0;
        }
    }

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;

    pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
    if (!pRoom)
        return;

    if (gpLastSelectionRoom != pRoom)
        gbSelectionRoomSeen = 1;
    gpLastSelectionRoom = pRoom;

    nPlayerX = UNITS_GetXPosition(pPlayer);

    nPlayerY = UNITS_GetYPosition(pPlayer);

    D2COMMON_DUNGEON_GameSubtileToClientCoords(&nPlayerX, &nPlayerY);

    pBest = NULL;
    D2COMMON_DUNGEON_GetAdjacentRoomsListFromRoom(pRoom, &ppRoomList, &nNumRooms);

    for (nRoom = 0; (uint32_t)nRoom < (uint32_t)nNumRooms; ++nRoom)
    {
        pPrevUnit = NULL;
        nUnitCount = 0;

        for (pUnit = ppRoomList[nRoom]->pUnitFirst; pUnit; pUnit = pUnit->pRoomNext)
        {
            //  The NULL-safe type read, p ? p->dwUnitType : 6; the loop has already proved the unit, so the NULL arm is dead.
            nType = pUnit ? (int32_t)pUnit->dwUnitType : UNIT_TYPES_COUNT;
            if (nType == UNIT_MISSILE)
                continue;

            if (pPrevUnit == pUnit)
                break;
            pPrevUnit = pUnit;

            if (nUnitCount++ > 30000)
            {
                FOG_DisplayWarning("Far too many units on screen", szSourceFile, 536);
                break;
            }

            if (pUnit == pPlayer)
                continue;

            //  The same NULL-safe read again; its NULL arm would skip this and the item test.
            if ((pUnit ? (int32_t)pUnit->dwUnitType : UNIT_TYPES_COUNT) == UNIT_TILE && CUNIT_IsPointInWarpSelectBox(pUnit, nPlayerX, nPlayerY) && CUNIT_IsPointInWarpSelectBox(pUnit, nScreenX, nScreenY) && D2COMMON_UNITS_IsInMovingMode(pPlayer)
                && GAME_GetLeftButtonDown())
            {
                dwThrottle = GAME_GetInteractThrottle();
                if (GAME_GetFrameTick() < dwThrottle)
                {
                    GAME_SetInteractThrottle(GAME_GetFrameTick() + 500);
                    continue;
                }

                //  BUG: the arm below cannot run: gbSelectionRoomSeen is only ever written with 1, and the first frame with a room already
                //  stores it, because gpLastSelectionRoom starts NULL. UNREACHABLE: reproduced as the original has it.
                if (!gbSelectionRoomSeen)
                {
                    GAME_SetInteractThrottle(GAME_GetFrameTick() + 500);
                    gbSelectionRoomSeen = 1;
                }
            }

            if (pUnit->dwUnitType == UNIT_ITEM && gbNoPickup)
                continue;

            if (!CUNIT_HitTestUnit(pUnit, nScreenX, nScreenY, 1))
                continue;

            //  The old selection, hit and alive, wins the room.
            if (pOldSelection == pUnit && !CUNIT_IsUnitDead(pUnit))
            {
                pBest = pUnit;
                break;
            }

            if (!pBest)
            {
                pBest = pUnit;
                continue;
            }

            nBestType = (int32_t)pBest->dwUnitType;
            nPriority = gnUnitTypeSelectPriority[nType];
            nBestPriority = gnUnitTypeSelectPriority[nBestType];

            if (nType == UNIT_PLAYER)
            {
                if (CUNIT_IsUnitDead(pUnit))
                    nPriority = 1;
            }
            else if (nType == UNIT_MONSTER)
            {
                if (D2COMMON_MONSTERS_IsDead(pUnit))
                    nPriority = -8;
                if (D2COMMON_STATES_CheckState(pUnit, STATE_ATTACHED))
                    continue;
            }

            if (nBestType == UNIT_MONSTER && D2COMMON_MONSTERS_IsDead(pBest))
                nBestPriority = -8;

            if (nType == UNIT_OBJECT && nBestType != UNIT_OBJECT)
            {
                //  The candidate is an object, the best is not.
                if (!D2COMMON_UNITS_IsDoor(pUnit))
                {
                    if (nBestPriority < nPriority)
                        pBest = pUnit;
                    continue;
                }

                //  A NULL-safe mode read; closed only.
                if ((pUnit ? (int32_t)pUnit->dwAnimMode : 0) != 0)
                    continue;

                pObjectsTxt = D2COMMON_UNITS_GetObjectTxtRecordFromObject(pUnit);
                pSomePlayer = CUNIT_GetPlayer();
                if (pSomePlayer)
                {
                    D2COMMON_UNITS_GetCoords(pSomePlayer, &tPlayer);
                }
                else
                {
                    tPlayer.nX = 0;
                    tPlayer.nY = 0;
                }
                D2COMMON_UNITS_GetCoords(pUnit, &tUnit);
                D2COMMON_UNITS_GetCoords(pBest, &tBest);

                //  Is the door strictly between the best unit and the player? Unsigned compares.
                if (pObjectsTxt->nOrientation == 1)
                {
                    bBetween = ((uint32_t)tBest.nY > (uint32_t)tUnit.nY && (uint32_t)tUnit.nY > (uint32_t)tPlayer.nY) || ((uint32_t)tBest.nY < (uint32_t)tUnit.nY && (uint32_t)tUnit.nY < (uint32_t)tPlayer.nY);
                }
                else
                {
                    bBetween = ((uint32_t)tBest.nX < (uint32_t)tUnit.nX && (uint32_t)tUnit.nX < (uint32_t)tPlayer.nX) || ((uint32_t)tBest.nX > (uint32_t)tUnit.nX && (uint32_t)tUnit.nX > (uint32_t)tPlayer.nX);
                }

                if (bBetween && nBestPriority < nPriority)
                    pBest = pUnit;
                continue;
            }

            if (nType != UNIT_OBJECT && nBestType == UNIT_OBJECT)
            {
                //  The best is an object, the candidate is not.
                if (!D2COMMON_UNITS_IsDoor(pBest))
                {
                    if (nBestPriority < nPriority)
                        pBest = pUnit;
                    continue;
                }

                //  No NULL test here: pBest was tested earlier.
                if (pBest->dwAnimMode != 0)
                    continue;

                pSomePlayer = CUNIT_GetPlayer();
                if (pSomePlayer)
                {
                    D2COMMON_UNITS_GetCoords(pSomePlayer, &tPlayer);
                }
                else
                {
                    tPlayer.nX = 0;
                    tPlayer.nY = 0;
                }
                D2COMMON_UNITS_GetCoords(pUnit, &tUnit);
                D2COMMON_UNITS_GetCoords(pBest, &tBest);

                //  Is the best door strictly between the candidate and the player? The record is fetched after the coordinates here, before
                //  them in the other arm.
                if (D2COMMON_UNITS_GetObjectTxtRecordFromObject(pBest)->nOrientation == 1)
                {
                    bBetween = ((uint32_t)tBest.nY > (uint32_t)tUnit.nY && (uint32_t)tBest.nY < (uint32_t)tPlayer.nY) || ((uint32_t)tBest.nY < (uint32_t)tUnit.nY && (uint32_t)tBest.nY > (uint32_t)tPlayer.nY);
                }
                else
                {
                    bBetween = ((uint32_t)tBest.nX < (uint32_t)tUnit.nX && (uint32_t)tBest.nX > (uint32_t)tPlayer.nX) || ((uint32_t)tBest.nX > (uint32_t)tUnit.nX && (uint32_t)tBest.nX < (uint32_t)tPlayer.nX);
                }

                if (!bBetween)
                    pBest = pUnit;
                continue;
            }

            //  Both objects, or neither: priority, then distance.
            if (nBestPriority != nPriority)
            {
                if (nBestPriority < nPriority)
                    pBest = pUnit;
                continue;
            }

            //  Eight D2Common calls, in the original's order; the squares are taken in unsigned arithmetic, as imul wraps, and compared signed.
            nCoord = D2COMMON_UNITS_GetClientCoordX(pUnit);
            nDxUnit = D2COMMON_UNITS_GetClientCoordX(pPlayer) - nCoord;
            nCoord = D2COMMON_UNITS_GetClientCoordY(pUnit);
            nDyUnit = D2COMMON_UNITS_GetClientCoordY(pPlayer) - nCoord;
            nCoord = D2COMMON_UNITS_GetClientCoordX(pBest);
            nDxBest = D2COMMON_UNITS_GetClientCoordX(pPlayer) - nCoord;
            nCoord = D2COMMON_UNITS_GetClientCoordY(pBest);
            nDyBest = D2COMMON_UNITS_GetClientCoordY(pPlayer) - nCoord;

            if ((int32_t)((uint32_t)nDyUnit * (uint32_t)nDyUnit + (uint32_t)nDxUnit * (uint32_t)nDxUnit) < (int32_t)((uint32_t)nDyBest * (uint32_t)nDyBest + (uint32_t)nDxBest * (uint32_t)nDxBest))
            {
                pBest = pUnit;
            }
        }
    }

    SELECT_ApplySelectedUnit(pBest);
}

//1.10f: D2Client.0x6FAB6430
//  Writes a pet's display state: the dword 8 bytes into the record ROSTERPETS_GetPetInfo finds, which UIPANELS_DrawInGamePanels reads as
//  "draw the mercenary portrait" when it is 1.
//  pOwner is tested and not otherwise used, as in the original.
//  BUG: ROSTERPETS_GetPetInfo answers NULL for a pet not in its list, and the store goes through it untested. LATENT: the two PLAYER_
//  callers read the same record untested before they call, and HIREABLES_HirIconsOnF11 passes the monster whose GUID is 0xFFFFFFFF.
void __fastcall SELECT_RosterPetsSetHirelingDisplayState(D2UnitStrc *pOwner, D2UnitStrc *pPet, int32_t nState)
{
    if (pOwner && pPet)
        ROSTERPETS_GetPetInfo(pPet->dwUnitId)->nHoverTextState = nState;
}

//1.10f: D2Client.0x6FAB6450
//  Sets or clears the click region -- the rectangle over which SELECT_UpdateSelectedUnit selects nothing, stored as left, top, right,
//  bottom in memory order and compared exclusively there.
//  void: eax is pRect[2], the last load, on the set path and never written on the clear path, and no caller reads it.
void __fastcall SELECT_SetClickRegion(const int32_t *pRect)
{
    if (pRect)
    {
        gnClickRegionLeft = pRect[0];
        gnClickRegionTop = pRect[1];
        gnClickRegionRight = pRect[2];
        gnClickRegionBottom = pRect[3];
        gbClickRegionValid = 1;
        return;
    }

    gbClickRegionValid = 0;
}

//  The skills.txt flags and the skill id SELECT_SkillsResolveClientTarget reads as literals: bit 24, bits 21..23 of dwFlags[0], and skill 90.
static_assert(SKILLSFLAGINDEX_SEARCHENEMYXY == 21
                  && SKILLSFLAGINDEX_SEARCHENEMYNEAR == 22
                  && SKILLSFLAGINDEX_SEARCHOPENXY == 23
                  && SKILLSFLAGINDEX_TARGETCORPSE == 24,
              "the skills.txt flag bits the target resolver tests");
static_assert(SKILL_IRONGOLEM == 90, "`cmp eax, 5Ah` at 0x6FAB66AE");
static_assert(sizeof(D2UnitFindArgStrc) == 14 * 4,
              "`mov ecx, 0Eh / rep stosd` clears it at 0x6FAB66D7");

//1.10f: D2Client.0x6FAB6490
//  Returns what a skill cast is aimed at -- a unit, or NULL -- and always a point, corrected through pnX/pnY. pnX, pnY, nCastFlags and
//  bAllowSearch are on the stack (retn 10h).
//  A dead target: a dead player is returned as it is. Any other dead target is kept only if the skill has TARGETCORPSE; otherwise the
//  selection is dropped (SELECT_ApplySelectedUnit(NULL)), the corpse's position becomes the point, and the target is NULL. For that case,
//  and for no target at all, a point exactly on the player is moved in front of it (SKILLS_GetPosInFrontOfUnit). A live target -- or a
//  corpse the skill wants -- skips that check.
//  The gates: cast flags 8 and 1 without 0x20 while the player is moving, or any non-zero use state from
//  PLRSKILLS_SkillsGetUseStateForClient, return the target unchanged.
//  The three searches, all from one byte of the flags:
//      SEARCHENEMYNEAR   the nearest unit to the point, within a distance of 10 (strictly), that CUNIT_HitTestUnit allows (bTestGeometry 0)
//                        -- found with D2Common's unit finder over the player's room and neighbours, size 9, mask 0x3002 with
//                        TARGETCORPSE, else 0x2003. Iron Golem (skill 90) searches with mask 0x20 and SELECT_SkillsIsIronGolemTarget,
//                        and a metal item already targeted is kept without a search
//      SEARCHENEMYXY     with no target and bAllowSearch, whatever SELECT_SkillsFindEnemyInFrontOfUnit finds
//      SEARCHOPENXY      the target's position (or the point) moved to a free subtile within 7 when collision mask 1 blocks it;
//                        then the answer is NULL -- aimed at ground
//  The finder's distance is an unsigned int, compared signed -- cast here. COLLISION_CheckMask's answer is 16 bits.
//  The original's one stack slot holds both the SEARCHOPENXY point and, on the SEARCHENEMYNEAR path, the best unit so far; they are two
//  variables here. The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
D2UnitStrc *__fastcall SELECT_SkillsResolveClientTarget(D2SkillStrc *pSkill, D2UnitStrc *pTargetUnit, int32_t *pnX, int32_t *pnY, uint8_t nCastFlags, int32_t bAllowSearch)
{
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pPlayer;
    D2UnitStrc *pResult;
    D2UnitStrc *pCandidate;
    D2ActiveRoomStrc *pRoom;
    UNITFINDTEST pfnFindTest;
    D2UnitFindArgStrc tFindArg;
    D2UnitFindDataStrc tFindData;
    D2CoordStrc tPoint;
    uint8_t nSearchFlags;
    int32_t nFindFlags;
    int32_t nPlayerX;
    int32_t nPlayerY;
    int32_t nBestDistance;
    int32_t nDistance;
    int32_t nIndex;

    pSkillsTxt = D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill(pSkill);
    pPlayer = CUNIT_GetPlayer();

    if (pTargetUnit && pTargetUnit->dwUnitType == UNIT_PLAYER && CUNIT_IsUnitDead(pTargetUnit))
        return pTargetUnit;

    if (!pTargetUnit || (!(((const uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_TARGETCORPSE >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_TARGETCORPSE & 7]) && CUNIT_IsUnitDead(pTargetUnit)))
    {
        if (pTargetUnit)
        {
            SELECT_ApplySelectedUnit(NULL);

            *pnX = UNITS_GetXPosition(pTargetUnit);

            *pnY = UNITS_GetYPosition(pTargetUnit);
            pTargetUnit = NULL;
        }

        nPlayerX = UNITS_GetXPosition(pPlayer);

        if (*pnX == nPlayerX)
        {
            nPlayerY = UNITS_GetYPosition(pPlayer);

            if (*pnY == nPlayerY)
                SKILLS_GetPosInFrontOfUnit(pPlayer, pnX, pnY);
        }
    }

    if ((nCastFlags & 8) && (nCastFlags & 1) && !(nCastFlags & 0x20) && D2COMMON_UNITS_IsInMovingMode(pPlayer))
        return pTargetUnit;

    if (PLRSKILLS_SkillsGetUseStateForClient(pPlayer, pSkill))
        return pTargetUnit;

    nSearchFlags = ((const uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_SEARCHENEMYNEAR >> 3];

    if (!(nSearchFlags & gdwBitMasks[SKILLSFLAGINDEX_SEARCHENEMYNEAR & 7]))
    {
        if (!pTargetUnit && (nSearchFlags & gdwBitMasks[SKILLSFLAGINDEX_SEARCHENEMYXY & 7]) && bAllowSearch)
            return SELECT_SkillsFindEnemyInFrontOfUnit(pPlayer, pSkillsTxt);

        if (!(nSearchFlags & gdwBitMasks[SKILLSFLAGINDEX_SEARCHOPENXY & 7]))
            return pTargetUnit;

        if (pTargetUnit)
        {
            tPoint.nX = UNITS_GetXPosition(pTargetUnit);
            tPoint.nY = UNITS_GetYPosition(pTargetUnit);
        }
        else
        {
            tPoint.nX = *pnX;
            tPoint.nY = *pnY;
        }

        pRoom = CUNIT_GetPlayerRoom();
        if (!pRoom)
            return pTargetUnit;
        if (!D2COMMON_COLLISION_CheckMask(pRoom, tPoint.nX, tPoint.nY, 1))
            return pTargetUnit;
        if (!D2COMMON_COLLISION_GetFreeCoordinatesWithMaxDistance(pRoom, &tPoint, 1, 1, FALSE, 7))
            return pTargetUnit;

        *pnX = tPoint.nX;
        *pnY = tPoint.nY;
        return NULL;
    }

    //  SEARCHENEMYNEAR.
    pResult = pTargetUnit;
    if (pTargetUnit)
    {
        if ((((const uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_TARGETCORPSE >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_TARGETCORPSE & 7]) && CUNIT_IsUnitDead(pTargetUnit))
            return pTargetUnit;

        *pnX = UNITS_GetXPosition(pTargetUnit);
        *pnY = UNITS_GetYPosition(pTargetUnit);
    }

    //  0x3002 with TARGETCORPSE, else 0x2003.
    nFindFlags = (((const uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_TARGETCORPSE >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_TARGETCORPSE & 7]) ? 0x3002 : 0x2003;
    pfnFindTest = NULL;

    if (D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSourceFile, 769) == SKILL_IRONGOLEM)
    {
        if (pTargetUnit && pTargetUnit->dwUnitType == UNIT_ITEM && D2COMMON_ITEMS_IsMetalItem(pTargetUnit->dwClassId))
            return pTargetUnit;

        nFindFlags = 0x20;
        pfnFindTest = SELECT_SkillsIsIronGolemTarget;
    }

    memset(&tFindArg, 0, sizeof(tFindArg));
    tFindArg.nFlags = nFindFlags;
    tFindArg.pUnit = CUNIT_GetPlayer();
    tFindArg.nX = *pnX;
    tFindArg.nY = *pnY;
    tFindArg.nSize = 9;
    pRoom = CUNIT_GetPlayerRoom();
    D2COMMON_UNITFINDS_InitializeUnitFindData(NULL, &tFindData, pRoom, tFindArg.nX, tFindArg.nY, 9, pfnFindTest, &tFindArg);
    tFindData.nFlags = nFindFlags;
    D2COMMON_UNITFINDS_FindAllMatchingUnitsInNeighboredRooms(&tFindData);

    nBestDistance = 10;
    for (nIndex = 0; nIndex < tFindData.nIndex; ++nIndex)
    {
        pCandidate = tFindData.pUnitsArray[nIndex];
        nDistance = (int32_t)D2COMMON_UNITS_GetDistanceToCoordinates(pCandidate, *pnX, *pnY);
        if (CUNIT_HitTestUnit(pCandidate, 0, 0, 0) && nDistance < nBestDistance)
        {
            pResult = pCandidate;
            nBestDistance = nDistance;
        }
    }

    D2COMMON_UNITFINDS_FreeUnitFindData(&tFindData);
    return pResult;
}

//1.10f: D2Client.0x6FAB6860
//  Iron Golem's find test: 1 for a non-NULL item whose class ItemsIsMetalItem accepts, else 0. D2Common calls it as a UNITFINDTEST; its
//  second parameter (edx) is never read here.
BOOL __fastcall SELECT_SkillsIsIronGolemTarget(D2UnitStrc *pUnit, D2UnitFindArgStrc *pUnitFindArg)
{
    return pUnit && pUnit->dwUnitType == UNIT_ITEM && D2COMMON_ITEMS_IsMetalItem(pUnit->dwClassId);
}

//  The flags SELECT_SkillsFindEnemyInFrontOfUnit tests as literals: skills.txt bit 20 and monstats2 bit 6.
static_assert(SKILLSFLAGINDEX_TARGETABLEONLY == 20
                  && MONSTATS2FLAGINDEX_SHIFTSEL == 6,
              "the skills.txt and monstats2 bits the enemy probe tests");

//1.10f: D2Client.0x6FAB6880
//  Returns the first enemy in front of pUnit, which becomes its target -- or NULL. SELECT_SkillsResolveClientTarget calls it for a skill
//  with SEARCHENEMYXY and without SEARCHENEMYNEAR, with no target and bAllowSearch, and returns the answer as its own.
//  Five probes: the facing is UNITS_GetNewDirection's low byte shifted right by 3 -- 64 directions to eight. The walk is over
//  gnTargetProbeOrder, {0, -1, 1, -2, 2}: each entry is added to the facing and masked with 7, so the probes run straight ahead, then one
//  step to either side, then two, and never behind. Each direction indexes an octagon of reach 3, gnDir8ProbeX {0, -2, -3, -2, 0, 2, 3, 2}
//  and gnDir8ProbeY {3, 2, 0, -2, -3, -2, 0, 2}. The subtile must pass COLLISION_CheckMaskWithSize(pRoom, x, y, 1, 0x180) before D2Common's
//  #10407 is asked for a unit there, with SELECT_UnitFindIsMonster as its test.
//  BUG: pRoom goes to both untested. INERT: for a NULL room the first answers 0x27 and the second 0, so every probe finds nothing.
//  The walk is a pointer stepped by 4 and compared signed (jl) against the address five entries past the table's start (written
//  gnTargetProbeOrder + 5); the dword at that address (-3) is not an entry.
//  The unit must have UNITFLAG_CANBEATTACKED. Then any of three is enough: the skill lacks TARGETABLEONLY (bit 20); the unit is a monster
//  whose monstats2 row has SHIFTSEL (bit 6); CUNIT_IsUnitSelectableTarget(pUnit, pTarget) agrees. The unit becomes pUnit's target through
//  UNITS_SetTargetUnitForDynamicUnit before it is returned. A monster whose class or monstats2 row is out of range, or whose row pointer
//  is NULL, falls through to the selectable test.
//  The flag bit's result is stored twice in the original and never read; it is not a variable here.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition and DATATBLS_GetMonStats2TxtRecord here.
D2UnitStrc *__fastcall SELECT_SkillsFindEnemyInFrontOfUnit(D2UnitStrc *pUnit, D2SkillsTxt *pSkillsTxt)
{
    D2ActiveRoomStrc *pRoom;
    D2UnitStrc *pTarget;
    D2MonStats2Txt *pMonStats2Txt;
    const int32_t *pProbe;
    int32_t nFacing;
    int32_t nDir;
    int32_t nX;
    int32_t nY;
    int32_t nProbeX;
    int32_t nProbeY;
    BOOL bAccept;

    nFacing = (int32_t)((uint32_t)(uint8_t)D2COMMON_UNITS_GetNewDirection(pUnit) >> 3);
    pRoom = D2COMMON_UNITS_GetRoom(pUnit);

    pProbe = gnTargetProbeOrder;
    do
    {
        nDir = (*pProbe + nFacing) & 7;

        nX = UNITS_GetXPosition(pUnit);
        nProbeX = nX + gnDir8ProbeX[nDir];

        nY = UNITS_GetYPosition(pUnit);
        nProbeY = nY + gnDir8ProbeY[nDir];

        if (D2COMMON_COLLISION_CheckMaskWithSize(pRoom, nProbeX, nProbeY, 1, 0x180))
        {
            pTarget = D2COMMON_10407(pRoom, nProbeX, nProbeY, SELECT_UnitFindIsMonster, NULL, 1);
            if (pTarget && (pTarget->dwFlags & UNITFLAG_CANBEATTACKED))
            {
                bAccept = FALSE;
                if (!(((const uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_TARGETABLEONLY >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_TARGETABLEONLY & 7]))
                {
                    bAccept = TRUE;
                }
                else
                {
                    if (pTarget->dwUnitType == UNIT_MONSTER)
                    {
                        pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pTarget->dwClassId);
                        if (pMonStats2Txt && (pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_SHIFTSEL >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_SHIFTSEL & 7]))
                        {
                            bAccept = TRUE;
                        }
                    }

                    if (!bAccept && CUNIT_IsUnitSelectableTarget(pUnit, pTarget))
                        bAccept = TRUE;
                }

                if (bAccept)
                {
                    D2COMMON_UNITS_SetTargetUnitForDynamicUnit(pUnit, pTarget);
                    return pTarget;
                }
            }
        }

        ++pProbe;
    } while ((int32_t)pProbe < (int32_t)(gnTargetProbeOrder + 5));

    return NULL;
}

//1.10f: D2Client.0x6FAB6A30
//  The unit test SELECT_SkillsFindEnemyInFrontOfUnit hands D2Common's #10407: tests for a monster. #10407 calls it as
//  int (__fastcall *)(D2UnitStrc *, void *) with its fifth argument, which the probe passes as NULL; edx is never read here.
int32_t __fastcall SELECT_UnitFindIsMonster(D2UnitStrc *pUnit, void *pContext)
{
    return pUnit && pUnit->dwUnitType == UNIT_MONSTER;
}

//1.10f: D2Client.0x6FAB6A50
//  Resets both reference monsters to 19 (MONSTER_FALLEN1), their initial value. void: the original leaves 19 in eax, which its one caller
//  (GAME_WindowOnClose) does not read.
void __cdecl SELECT_ResetRefMonsters()
{
    gnToHitRefMonster = MONSTER_FALLEN1;
    gnToBeHitRefMonster = MONSTER_FALLEN1;
}

//  UNITS_GetXPosition and UNITS_GetYPosition (0x6FAB6A60, 0x6FAB6A90) are in Shared.cpp.

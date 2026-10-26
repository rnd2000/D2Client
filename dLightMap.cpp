#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "dLightMap.h"
#include "CoreArchive.h"

//  dLightMap.cpp -- the dynamic light map of D2Client.dll 1.10: the lifecycle of a light node (add, resize, recolour, release, remove), the 48x48
//  ambient light grid centred on the player, the per-frame light update with its shadow propagation, static-light baking and the warp flash.
//  The original source file is Draw\dLightMap.cpp (the asserts in this unit name it). Address range 0x6FAA4280..0x6FAA633F; globals: dLightMap.h.
//  The functions from 0x6FAA60B0 on name no source file; they are placed here by position. CLIENT_FindRoomContainingSubtile (0x6FAA6150, between
//  DLIGHTMAP_DebugDrawLightGrid and DLIGHTMAP_StartWarpFlash) is written in Shared.cpp.
static const char szSourceFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\Draw\\dLightMap.cpp";

//  The assert texts this unit pushes. szAssertPtLightNTypeStatic is the only place the original's own name for nType 2 survives.
static const char szAssertPtLight[] = "ptLight";
static const char szAssertPtCurr[] = "ptCurr";
static const char szAssertPtLightNTypeStatic[] = "ptLight->nType != LIGHT_TYPE_STATIC";

//  The assert texts of the static-map functions: DLIGHTMAP_BuildStaticMap needs a room to bake against, refuses to redo a valid bake and needs its
//  buffer; DLIGHTMAP_ApplyLight needs the same buffer and a finished bake. The callback's hSrcUnit text is gszAssertHSrcUnit in dLightMap.h.
static const char szAssertPtLightPnStaticMap[] = "ptLight->pnStaticMap";
static const char szAssertNotPtLightFStaticValid[] = "!ptLight->fStaticValid";
static const char szAssertHRoom[] = "hRoom";
static const char szAssertPtLightFStaticValid[] = "ptLight->fStaticValid";

//  A static light is the one kind that bakes a map: DLIGHTMAP_AddLight allocates it, DLIGHTMAP_SetRadius frees it and DLIGHTMAP_ReleaseLight
//  refuses it. The other kinds, 0 and 1, have no name here.
static const int32_t LIGHT_TYPE_STATIC = 2;

//  Member offsets of D2ClientLightStrc as DLIGHTMAP_AddLight stores them: one FOG_Alloc(52, ...) and a store to every member, all in 0x00..0x34.
static_assert(offsetof(D2ClientLightStrc, dwUnitType) == 0x00
                  && offsetof(D2ClientLightStrc, dwUnitId) == 0x04
                  && offsetof(D2ClientLightStrc, bUnitIsAsync) == 0x08,
              "the owner triple: type, id and the id-space discriminant");
static_assert(offsetof(D2ClientLightStrc, nType) == 0x0C
                  && offsetof(D2ClientLightStrc, nSubtileX) == 0x10
                  && offsetof(D2ClientLightStrc, nSubtileY) == 0x14,
              "the kind and the subtile centre AddLight stores");
static_assert(offsetof(D2ClientLightStrc, nRadius) == 0x18
                  && offsetof(D2ClientLightStrc, nTargetRadius) == 0x1C,
              "live radius, then target -- GetRadius returns +0x18");
static_assert(offsetof(D2ClientLightStrc, bOrphaned) == 0x20
                  && offsetof(D2ClientLightStrc, nIntensity) == 0x24
                  && offsetof(D2ClientLightStrc, nRed) == 0x25
                  && offsetof(D2ClientLightStrc, nGreen) == 0x26
                  && offsetof(D2ClientLightStrc, nBlue) == 0x27,
              "ReleaseLight's flag, then the four colour bytes");
static_assert(offsetof(D2ClientLightStrc, pNext) == 0x28
                  && offsetof(D2ClientLightStrc, fStaticValid) == 0x2C
                  && offsetof(D2ClientLightStrc, pnStaticMap) == 0x30,
              "the list link and the static map's validity pair");

//  Layout of the room-walk structs and the sight mask.
static_assert(offsetof(D2ActiveRoomStrc, tCoords) == 0
                  && offsetof(D2DrlgCoordsStrc, nSubtileX) == 0
                  && offsetof(D2DrlgCoordsStrc, nSubtileY) == 4
                  && offsetof(D2DrlgCoordsStrc, nSubtileWidth) == 8
                  && offsetof(D2DrlgCoordsStrc, nSubtileHeight) == 0x0C,
              "the room's coordinates are its head, and the four subtile fields");
static_assert(offsetof(D2LightGridCellStrc, nLit) == 0
                  && offsetof(D2LightGridCellStrc, nIntensity) == 4
                  && offsetof(D2LightGridCellStrc, nRed) == 5
                  && offsetof(D2LightGridCellStrc, nGreen) == 6
                  && offsetof(D2LightGridCellStrc, nBlue) == 7,
              "the ambient grid cell: a dword flag, then the four colour bytes");
static_assert((COLLIDE_VISIBLE | COLLIDE_BLANK) == 0x22,
              "the light grid's blocking test -- 0x22 at 6FAA4C70");

//1.10f: D2Client.0x6FAA4280
//  Creates a light: allocates a D2ClientLightStrc (52 bytes), fills its sixteen members and pushes it on the front of gpDynamicLights. Returns NULL
//  when the radius is not positive. Every coordinate is in eighths of a subtile: (precision >> 13) + 4 is the subtile centre, and >> 3 turns one
//  into a subtile, the light grid's cell. nRadiusSubtiles (in subtiles, clamped to 18) is stored as 8 * subtiles in both nRadius and nTargetRadius,
//  so a new light does not animate until DLIGHTMAP_SetTargetRadius moves the target.
//  bUnitIsAsync is bit 21 of the owner's flags (UNITFLAG_ISASYNC); it selects the id space dwUnitId lives in for DLIGHTMAP_Update's lookup.
//  A static light (nType 2) also allocates pnStaticMap, 4 * (2r + 1) * (2r + 1) bytes with r the radius in subtiles, one dword per cell of the
//  square it covers; every other kind carries NULL there. A light with no owner unit stores dwUnitType 6 (UNIT_TYPES_COUNT) and dwUnitId -1.
//  The return value is used: it is the light pointer the owner keeps at +0x64.
D2ClientLightStrc *__fastcall DLIGHTMAP_AddLight(D2UnitStrc *pUnit, int32_t nType, int32_t nRadiusSubtiles, uint8_t nIntensity, uint8_t nRed, uint8_t nGreen, uint8_t nBlue)
{
    D2ClientLightStrc *pLight;
    D2C_UnitTypes dwUnitType;
    uint32_t dwUnitId;
    int32_t nRadiusClamped;

    if (nRadiusSubtiles <= 0)
        return NULL;

    nRadiusClamped = nRadiusSubtiles;
    if (nRadiusSubtiles >= 18)
        nRadiusClamped = 18;

    ++gnDLightAllocCount;

    pLight = (D2ClientLightStrc *)FOG_Alloc(52, szSourceFile, 255, 0);

    dwUnitType = UNIT_TYPES_COUNT;
    if (pUnit)
        dwUnitType = pUnit->dwUnitType;
    pLight->dwUnitType = dwUnitType;

    if (pUnit)
        dwUnitId = pUnit->dwUnitId;
    else
        dwUnitId = (uint32_t)-1;
    pLight->dwUnitId = dwUnitId;

    //  bit 21 of the owner's flags (UNITFLAG_ISASYNC)
    pLight->bUnitIsAsync = pUnit != NULL && ((pUnit->dwFlags >> 21) & 1) != 0;

    //  BUG: pUnit is tested for NULL above and is handed to D2COMMON_UNITS_GetPrecisionX regardless, and the stock D2Common asserts on a NULL unit
    //  and exits. LATENT: no caller reaches it, all of them pass a unit they have already used, so the no-owner arms never complete.
    //  The shifts are logical here (shr eax, 0Dh at 0x6FAA4305 and 0x6FAA4314), where DLIGHTMAP_Update's are arithmetic (sar at 0x6FAA4D49 and
    //  0x6FAA4D4C), so the value is unsigned; no coordinate reaches bit 31.
    pLight->nSubtileX = (int32_t)(((uint32_t)D2COMMON_UNITS_GetPrecisionX(pUnit) >> 13) + 4);
    pLight->nSubtileY = (int32_t)(((uint32_t)D2COMMON_UNITS_GetPrecisionY(pUnit) >> 13) + 4);

    pLight->nType = nType;
    pLight->nIntensity = nIntensity;
    pLight->nRed = nRed;
    pLight->nGreen = nGreen;
    pLight->nBlue = nBlue;

    pLight->nRadius = 8 * nRadiusClamped;
    pLight->nTargetRadius = 8 * nRadiusClamped;

    pLight->bOrphaned = 0;
    pLight->fStaticValid = 0;
    pLight->pNext = gpDynamicLights;

    //  a static light bakes its map; every other kind carries the NULL
    if (nType == LIGHT_TYPE_STATIC)
        pLight->pnStaticMap = (int32_t *)FOG_Alloc(4 * (2 * nRadiusClamped + 1) * (2 * nRadiusClamped + 1), szSourceFile, 278, 0);
    else
        pLight->pnStaticMap = NULL;

    gpDynamicLights = pLight;

    return pLight;
}

//1.10f: D2Client.0x6FAA43A0
//  Moves the target radius only (nTargetRadius, +0x1C): clamped to 18 subtiles and stored as 8 * subtiles. The live nRadius is left alone;
//  DLIGHTMAP_Update walks it toward the target one subtile (8 eighths) per frame, so this moves a flickering torch or a pulsing skill effect.
//  A non-positive radius is ignored whole, assert and all. void: the original leaves 8 * subtiles in eax (the lea of the store); nothing reads it.
void __fastcall DLIGHTMAP_SetTargetRadius(D2ClientLightStrc *ptLight, int32_t nRadiusSubtiles)
{
    if (nRadiusSubtiles > 0)
    {
        if (nRadiusSubtiles >= 18)
            nRadiusSubtiles = 18;
        if (!ptLight)
        {
            FOG_DisplayAssert(szAssertPtLight, szSourceFile, 305);
            exit(-1);
        }
        ptLight->nTargetRadius = 8 * nRadiusSubtiles;
    }
}

//1.10f: D2Client.0x6FAA43E0
//  Moves both radii at once, an instant resize rather than an animated one; for a static light it also throws away what was baked (fStaticValid =
//  0, pnStaticMap freed), which DLIGHTMAP_SetTargetRadius does not. The original clamps the radius twice, before the assert (0x6FAA43E7) and after
//  it (0x6FAA4413); one clamp is written, since nothing between the two can change its value.
void __fastcall DLIGHTMAP_SetRadius(D2ClientLightStrc *ptLight, int32_t nRadiusSubtiles)
{
    if (nRadiusSubtiles > 0)
    {
        if (nRadiusSubtiles >= 18)
            nRadiusSubtiles = 18;
        if (!ptLight)
        {
            FOG_DisplayAssert(szAssertPtLight, szSourceFile, 321);
            exit(-1);
        }
        ptLight->nRadius = 8 * nRadiusSubtiles;
        ptLight->nTargetRadius = 8 * nRadiusSubtiles;
        if (ptLight->nType == LIGHT_TYPE_STATIC)
        {
            ptLight->fStaticValid = 0;
            if (ptLight->pnStaticMap)
                FOG_Free(ptLight->pnStaticMap, szSourceFile, 330, 0);
            ptLight->pnStaticMap = NULL;
        }
    }
}

//1.10f: D2Client.0x6FAA4460
//  Returns the live radius (nRadius, not nTargetRadius) back in subtiles: nRadius >> 3, an arithmetic shift (sar at 0x6FAA4485).
int32_t __fastcall DLIGHTMAP_GetRadius(D2ClientLightStrc *ptLight)
{
    if (!ptLight)
    {
        FOG_DisplayAssert(szAssertPtLight, szSourceFile, 341);
        exit(-1);
    }
    return ptLight->nRadius >> 3;
}

//1.10f: D2Client.0x6FAA4490
//  Sets nRed, nGreen and nBlue (+0x25..+0x27) and leaves nIntensity (+0x24) alone.
//  void: the original's eax at the exit is a partial-width leftover (mov al, [esp+nGreen]), the upper three bytes whatever the caller left.
void __fastcall DLIGHTMAP_SetColor(D2ClientLightStrc *ptLight, uint8_t nRed, uint8_t nGreen, uint8_t nBlue)
{
    if (!ptLight)
    {
        FOG_DisplayAssert(szAssertPtLight, szSourceFile, 359);
        exit(-1);
    }
    ptLight->nRed = nRed;
    ptLight->nGreen = nGreen;
    ptLight->nBlue = nBlue;
}

//1.10f: D2Client.0x6FAA44D0
//  Unlinks a light from gpDynamicLights and frees it: pnStaticMap first, then the node; gnDLightFreeCount is bumped last. The walk keeps the
//  predecessor to splice. The assert is ptCurr and fires when the light is not in the list at all, and also when the list is empty.
//  The original tests the head for NULL once (test eax, eax at 6FAA44E2) and then loops find-first (cmp eax, esi at 6FAA44E6 before the next node's
//  test at 6FAA44EF), which is the rotated form of the while (ptCurr && ptCurr != pLight) below.
void __fastcall DLIGHTMAP_RemoveLight(D2ClientLightStrc *pLight)
{
    D2ClientLightStrc *ptCurr;
    D2ClientLightStrc *pPrev;

    if (!pLight)
        return;

    ptCurr = gpDynamicLights;
    pPrev = NULL;
    while (ptCurr && ptCurr != pLight)
    {
        pPrev = ptCurr;
        ptCurr = ptCurr->pNext;
    }
    if (!ptCurr)
    {
        FOG_DisplayAssert(szAssertPtCurr, szSourceFile, 378);
        exit(-1);
    }

    if (pPrev)
        pPrev->pNext = ptCurr->pNext;
    else
        gpDynamicLights = ptCurr->pNext;

    if (pLight->pnStaticMap)
        FOG_Free(pLight->pnStaticMap, szSourceFile, 385, 0);

    FOG_Free(pLight, szSourceFile, 387, 0);
    ++gnDLightFreeCount;
}

//1.10f: D2Client.0x6FAA4560
//  Orphans a light: sets bOrphaned (+0x20) and nTargetRadius to 0 in the same breath, before checking the kind. DLIGHTMAP_Update then shrinks it
//  one subtile a frame and removes it once bOrphaned && nRadius <= 0, so a light can outlive its owner. The assert ptLight->nType !=
//  LIGHT_TYPE_STATIC (checked on the path where nType is 2, 0x6FAA458C) comes after both stores in the bytes too: a static light that reaches here
//  is left orphaned and then dies at the assert.
void __fastcall DLIGHTMAP_ReleaseLight(D2ClientLightStrc *ptLight)
{
    int32_t nType;

    if (!ptLight)
    {
        FOG_DisplayAssert(szAssertPtLight, szSourceFile, 397);
        exit(-1);
    }

    nType = ptLight->nType;
    ptLight->bOrphaned = 1;
    ptLight->nTargetRadius = 0;

    if (nType == LIGHT_TYPE_STATIC)
    {
        FOG_DisplayAssert(szAssertPtLightNTypeStatic, szSourceFile, 400);
        exit(-1);
    }
}

//1.10f: D2Client.0x6FAA45C0
//  The room's ambient colour through a three-level fallback, each level consulted only when the one before it left the colour black:
//      1. QUESTSFX_GetQuestLightOverride(pRoom, ...), skipped entirely when bUseRoomOverride is 0; its return is the intensity;
//      2. DUNGEON_GetRGB_IntensityFromRoom(pRoom, ...);
//      3. ENVIRONMENT_GetLightColorFromAct(gpAct, ...) plus ENVIRONMENT_GetIntensityFromAct(gpAct), the act's time-of-day light.
//  So an unlit room inherits the act's environment colour rather than going dark. The intensity local stands for the argument slot of
//  bUseRoomOverride: the flag is dead by the time DUNGEON_GetRGB_IntensityFromRoom runs and the compiler reuses its home.
//  void: eax is left holding pnBlue, and its one caller does not read it.
void __fastcall DLIGHTMAP_GetRoomLightColor(D2ActiveRoomStrc *pRoom, int32_t *pnIntensity, uint8_t *pnRed, uint8_t *pnGreen, uint8_t *pnBlue, int32_t bUseRoomOverride)
{
    uint8_t nRed = 0;
    uint8_t nGreen = 0;
    uint8_t nBlue = 0;
    uint8_t nIntensity;
    int32_t nIntensityFromAct;

    if (!bUseRoomOverride || (nIntensityFromAct = QUESTSFX_GetQuestLightOverride(pRoom, &nRed, &nGreen, &nBlue), !nRed && !nGreen && !nBlue))
    {
        D2COMMON_DUNGEON_GetRGB_IntensityFromRoom(pRoom, &nIntensity, &nRed, &nGreen, &nBlue);
        nIntensityFromAct = nIntensity;
        if (!nRed && !nGreen && !nBlue)
        {
            D2COMMON_ENVIRONMENT_GetLightColorFromAct(gpAct, &nRed, &nGreen, &nBlue);
            nIntensityFromAct = D2COMMON_ENVIRONMENT_GetIntensityFromAct(gpAct);
        }
    }
    *pnIntensity = nIntensityFromAct;
    *pnRed = nRed;
    *pnGreen = nGreen;
    *pnBlue = nBlue;
}

//1.10f: D2Client.0x6FAA46A0
//  Zeroes the ambient light grid: a tail jump (jmp at 6FAA46A5) into DLIGHTMAP_RebuildAmbientLight(1), where 1 means clear and 0 means rebuild. It
//  is written as a statement, not a return: both functions are void and the one caller discards the value.
void __cdecl DLIGHTMAP_ClearAmbientLight()
{
    DLIGHTMAP_RebuildAmbientLight(1);
}

//1.10f: D2Client.0x6FAA46B0
//  Rebuilds the 48x48 ambient grid, centred on the player (DLIGHTMAP_UpdateForPlayer sets the window just before the call). Two passes: the whole
//  grid takes the player's own room's colour, then each adjacent room paints its own colour over its own rectangle, clipped to the grid. bClear, or
//  no player, or no room, zeroes the grid instead; DLIGHTMAP_ClearAmbientLight passes 1 and the per-frame path passes 0.
//  The colour comes from the same three-level chain as DLIGHTMAP_GetRoomLightColor, written out in place without bUseRoomOverride (the compiler
//  packed the scratch bytes differently). The rectangles come from DUNGEON_GetRoomCoordinates in grid units, subtile - origin; the subtraction is
//  signed, so gnLightGridOriginX/Y are int and the four clip compares (0x6FAA4901..0x6FAA495A) are jg/jl.
//  void: neither caller wants the value left in eax.
void __fastcall DLIGHTMAP_RebuildAmbientLight(int32_t bClear)
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pPlayerRoom;
    D2ActiveRoomStrc *pRoom;
    D2ActiveRoomStrc **ppRoomList;
    int32_t nNumRooms;
    uint8_t nRed;
    uint8_t nGreen;
    uint8_t nBlue;
    uint8_t nIntensity;
    int32_t nIntensityFromAct;
    D2DrlgCoordsStrc coords;
    D2LightGridCellStrc *pGridRow;
    D2LightGridCellStrc *pGridCell;
    int32_t i;
    int32_t nRows;
    int32_t iRoom;
    int32_t nCellX0;
    int32_t nCellX1;
    int32_t nCellY0;
    int32_t nCellY1;
    int32_t nCellWidth;
    uint32_t dwPackedCell;

    pPlayer = CUNIT_GetPlayer();
    pPlayerRoom = pPlayer ? D2COMMON_UNITS_GetRoom(pPlayer) : NULL;

    if (!pPlayerRoom || bClear)
    {
        //  no player, no room, or asked to clear: the whole grid goes black
        for (pGridRow = gPlayerLightGrid; (int32_t)pGridRow <= (int32_t)&gPlayerLightGrid[2256]; pGridRow += 48)
        {
            for (i = 0; i < 48; ++i)
            {
                pGridRow[i].nLit = 0;
                pGridRow[i].nIntensity = 0;
                pGridRow[i].nRed = 0;
                pGridRow[i].nGreen = 0;
                pGridRow[i].nBlue = 0;
            }
        }
        return;
    }

    D2COMMON_DUNGEON_GetAdjacentRoomsListFromRoom(pPlayerRoom, &ppRoomList, &nNumRooms);

    //  the player's room's colour, through the three-level fallback
    nRed = 0;
    nGreen = 0;
    nBlue = 0;
    nIntensityFromAct = QUESTSFX_GetQuestLightOverride(pPlayerRoom, &nRed, &nGreen, &nBlue);
    if (!nRed && !nGreen && !nBlue)
    {
        D2COMMON_DUNGEON_GetRGB_IntensityFromRoom(pPlayerRoom, &nIntensity, &nRed, &nGreen, &nBlue);
        nIntensityFromAct = nIntensity;
        if (!nRed && !nGreen && !nBlue)
        {
            D2COMMON_ENVIRONMENT_GetLightColorFromAct(gpAct, &nRed, &nGreen, &nBlue);
            nIntensityFromAct = D2COMMON_ENVIRONMENT_GetIntensityFromAct(gpAct);
        }
    }

    //  BUG: the four bytes are packed into one dword with the intensity OR-ed in unmasked (or ecx, eax at 0x6FAA47B0, eax the full return of
    //  whichever call answered) and split back out with shr 8/16/24, so an intensity bit above bit 7 lands in red, green or blue. LATENT: every
    //  source that answers with a non-black colour stays inside 0..255 (the DUNGEON arm masks with and eax, 0FFh).
    dwPackedCell = ((uint32_t)nBlue << 24 | (uint32_t)nGreen << 16 | (uint32_t)nRed << 8) | (uint32_t)nIntensityFromAct;

    //  the whole grid takes it; the adjacent rooms paint over it below
    for (pGridRow = gPlayerLightGrid; (int32_t)pGridRow <= (int32_t)&gPlayerLightGrid[2256]; pGridRow += 48)
    {
        for (i = 0; i < 48; ++i)
        {
            pGridRow[i].nLit = 0;
            pGridRow[i].nIntensity = (uint8_t)dwPackedCell;
            pGridRow[i].nRed = (uint8_t)(dwPackedCell >> 8);
            pGridRow[i].nGreen = (uint8_t)(dwPackedCell >> 16);
            pGridRow[i].nBlue = (uint8_t)(dwPackedCell >> 24);
        }
    }

    for (iRoom = 0; iRoom < nNumRooms; ++iRoom)
    {
        pRoom = ppRoomList[iRoom];
        if (pRoom == pPlayerRoom)
            continue;

        //  the room's colour, the same chain again
        nRed = 0;
        nGreen = 0;
        nBlue = 0;
        nIntensityFromAct = QUESTSFX_GetQuestLightOverride(pRoom, &nRed, &nGreen, &nBlue);
        if (!nRed && !nGreen && !nBlue)
        {
            D2COMMON_DUNGEON_GetRGB_IntensityFromRoom(pRoom, &nIntensity, &nRed, &nGreen, &nBlue);
            nIntensityFromAct = nIntensity;
            if (!nRed && !nGreen && !nBlue)
            {
                D2COMMON_ENVIRONMENT_GetLightColorFromAct(gpAct, &nRed, &nGreen, &nBlue);
                nIntensityFromAct = D2COMMON_ENVIRONMENT_GetIntensityFromAct(gpAct);
            }
        }

        //  the same unmasked packing as above, at 0x6FAA48E2
        dwPackedCell = ((uint32_t)nBlue << 24 | (uint32_t)nGreen << 16 | (uint32_t)nRed << 8) | (uint32_t)nIntensityFromAct;

        //  its rectangle, in grid units, clipped to the 48x48 window
        D2COMMON_DUNGEON_GetRoomCoordinates(ppRoomList[iRoom], &coords);
        nCellX0 = coords.nSubtileX - gnLightGridOriginX;
        nCellX1 = coords.nSubtileX + coords.nSubtileWidth - gnLightGridOriginX;
        if (nCellX0 > 48 || nCellX1 < 0)
            continue;
        nCellY0 = coords.nSubtileY - gnLightGridOriginY;
        nCellY1 = coords.nSubtileY + coords.nSubtileHeight - gnLightGridOriginY;
        if (nCellY0 > 48 || nCellY1 < 0)
            continue;
        if (nCellX0 < 0)
            nCellX0 = 0;
        if (nCellX1 >= 48)
            nCellX1 = 47;
        if (nCellY0 < 0)
            nCellY0 = 0;
        if (nCellY1 >= 48)
            nCellY1 = 47;
        if (nCellY0 > nCellY1)
            continue;

        pGridRow = &gPlayerLightGrid[48 * nCellY0 + nCellX0];
        for (nRows = nCellY1 - nCellY0 + 1; nRows > 0; --nRows)
        {
            if (nCellX0 <= nCellX1)
            {
                nCellWidth = nCellX1 - nCellX0 + 1;
                pGridCell = pGridRow;
                while (nCellWidth)
                {
                    pGridCell->nLit = 0;
                    pGridCell->nIntensity = (uint8_t)dwPackedCell;
                    pGridCell->nRed = (uint8_t)(dwPackedCell >> 8);
                    pGridCell->nGreen = (uint8_t)(dwPackedCell >> 16);
                    pGridCell->nBlue = (uint8_t)(dwPackedCell >> 24);
                    ++pGridCell;
                    --nCellWidth;
                }
            }
            pGridRow += 48;
        }
    }
}

//1.10f: D2Client.0x6FAA4A30
//  One-time init, from GAME_PreloadActData. It registers DLIGHTMAP_ActCallback with DUNGEON_SetActCallbackFunc, so the dungeon tells the light map
//  when rooms change, and builds gnReciprocal16, a 16.16 fixed-point reciprocal table: [0] = 0 and [i] = 0x10000 / i for i = 1..255, so that the
//  light falloff divides with a multiply and a >> 16.
//  In the bytes the loop does inc ecx before the store and indexes with the already incremented value, so the element for i lands at
//  0x6FBA29F4 + 4 * (i + 1); the array base is 0x6FBA29F8 and 0x6FBA29F4 is only the displacement.
//  void: eax is left holding the last quotient, and its one caller does not read it.
void __cdecl DLIGHTMAP_Init()
{
    int32_t i;
    int32_t nResult;

    D2COMMON_DUNGEON_SetActCallbackFunc(gpAct, DLIGHTMAP_ActCallback);

    gnReciprocal16[0] = 0;
    nResult = 0;
    for (i = 1; i < 256; ++i)
    {
        nResult = 0x10000 / i;
        gnReciprocal16[i] = nResult;
    }

}

//1.10f: D2Client.0x6FAA4A70
//  Picks the light-map quality level, 0, 1 or 2: stores it in gnDLightQualityLevel and returns it; DLIGHTMAP_UpdateForPlayer hands it to every
//  DLIGHTMAP_Update of the same frame. It starts at 2 and steps down, driven by the measured frame rate rather than by any setting:
//      D2GFX_CheckLowQuality    forces 1, or 0 outright when MISSILE_GetLightQuality is false, which skips the rate test entirely;
//      GAME_GetRecentTickRate   <= 9 -> 0
//                               9 < rate <= 12 -> 1
//                               12 < rate <= 15 -> 1, unless a level >= 1 is already stored, which is returned as it is (early exit 0x6FAA4AE1)
//                               rate > 15 -> keeps what the tests above set
//  Then the 2000 ms gate: a change is refused when GetTickCount() - gnDLightQualityLastTick <= 0x7D0 and the stored level is returned instead. A
//  candidate the gate refuses is not remembered; the next call recomputes from scratch.
//  The level is spent in DLIGHTMAP_Update: for an nType 0 light, above 1 takes the inline shadow walk and 0..1 takes DLIGHTMAP_ApplyLightSource; a
//  static light bakes and spends its map at every level and an nType 1 light goes to ApplyLightSource at every level. ApplyLightSource applies its
//  per-unit-type radius clamps only at 0.
int32_t __cdecl DLIGHTMAP_UpdateQualityLevel()
{
    uint32_t dwTickCount;
    int32_t nLevel;
    int32_t nRecentTickRate;

    nLevel = 2;
    if (D2GFX_CheckLowQuality())
    {
        if (!MISSILE_GetLightQuality())
            goto no_light_quality;
        nLevel = 1;
    }
    nRecentTickRate = GAME_GetRecentTickRate();
    if (nRecentTickRate > 9)
    {
        if (nRecentTickRate > 12)
        {
            if (nRecentTickRate > 15)
                goto apply_gate;
            if (gnDLightQualityLevel >= 1)
                return gnDLightQualityLevel;
        }
        nLevel = 1;
        goto apply_gate;
    }
no_light_quality:
    nLevel = 0;
apply_gate:
    if (nLevel != gnDLightQualityLevel)
    {
        dwTickCount = GetTickCount();
        if (dwTickCount - gnDLightQualityLastTick <= 0x7D0)
            return gnDLightQualityLevel;
        gnDLightQualityLastTick = dwTickCount;
        gnDLightQualityLevel = nLevel;
    }
    return nLevel;
}

//1.10f: D2Client.0x6FAA4AF0
//  The per-frame entry, called at most once a frame from VIEWEFFECTS_EnvUpdateViewAndLevelEffects. Four steps:
//      the player's position sets the grid window (origin = position - 24, end = position + 24, so the 48x48 grid is centred on the player);
//      DLIGHTMAP_RebuildAmbientLight(0);
//      the sight pass: for each of the 2304 cells COLLISION_CheckMask is asked for 0x22 (COLLIDE_VISIBLE | COLLIDE_BLANK) at the cell's world
//      subtile and nLit = 1 is set on a hit; the room is carried across cells and re-found only when a cell leaves it, through the adjacent-room
//      list, starting from the player's room each time the search fails;
//      DLIGHTMAP_UpdateQualityLevel(), then every light in gpDynamicLights, counted into gnActiveDynamicLights on the way.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here: one discriminant selects the arm, and +0x2C is the union it is read
//  through, a static path for UNIT_OBJECT / UNIT_ITEM / UNIT_TILE (tGameCoords.nX at +0x0C, .nY at +0x10) and a dynamic one for everything else.
//  The room walk is not a copy of CLIENT_FindRoomContainingSubtile: its contains test and adjacent-list search have the same shape, but that
//  function's failure arm returns NULL while this site re-fetches the player's room with UNITS_GetRoom.
void __cdecl DLIGHTMAP_UpdateForPlayer()
{
    D2UnitStrc *pPlayer;
    D2UnitStrc *pUnit;
    D2ActiveRoomStrc *pRoom;
    D2ActiveRoomStrc *pRoomFound;
    D2ActiveRoomStrc *pCandidate;
    D2ActiveRoomStrc **ppRoomList;
    D2ClientLightStrc *pLight;
    D2ClientLightStrc *pNext;
    int32_t nNumRooms;
    uint32_t i;
    int32_t nPlayerX;
    int32_t nPlayerY;
    int32_t nGridX;
    int32_t nGridY;
    int32_t nCellOffset;
    int32_t nSubtileX;
    int32_t nSubtileY;
    int32_t nQualityLevel;

    pPlayer = CUNIT_GetPlayer();

    nPlayerX = UNITS_GetXPosition(pPlayer);

    nPlayerY = UNITS_GetYPosition(pPlayer);

    gnLightGridOriginX = nPlayerX - 24;
    gnLightGridOriginY = nPlayerY - 24;
    gnLightGridEndX = nPlayerX - 24 + 48;
    gnLightGridEndY = nPlayerY - 24 + 48;

    DLIGHTMAP_RebuildAmbientLight(0);

    pUnit = CUNIT_GetPlayer();
    pRoom = pUnit ? D2COMMON_UNITS_GetRoom(pUnit) : NULL;
    if (pRoom)
    {
        nCellOffset = 0;
        for (nGridY = 0; nGridY < 48; ++nGridY)
        {
            for (nGridX = 0; nGridX < 48; ++nGridX)
            {
                nSubtileX = gnLightGridOriginX + nGridX;
                nSubtileY = gnLightGridOriginY + nGridY;

                if (pRoom == NULL || nSubtileX < pRoom->tCoords.nSubtileX || nSubtileX >= pRoom->tCoords.nSubtileX + pRoom->tCoords.nSubtileWidth || nSubtileY < pRoom->tCoords.nSubtileY || nSubtileY >= pRoom->tCoords.nSubtileY
                                        + pRoom->tCoords.nSubtileHeight)
                {
                    pRoomFound = NULL;
                    if (pRoom)
                    {
                        ppRoomList = NULL;
                        nNumRooms = 0;
                        D2COMMON_DUNGEON_GetAdjacentRoomsListFromRoom(pRoom, &ppRoomList, &nNumRooms);
                        for (i = 0; i < (uint32_t)nNumRooms && pRoomFound == NULL; ++i)
                        {
                            pCandidate = ppRoomList[i];
                            if (pCandidate && nSubtileX >= pCandidate->tCoords.nSubtileX && nSubtileX < pCandidate->tCoords.nSubtileX + pCandidate->tCoords.nSubtileWidth && nSubtileY >= pCandidate->tCoords.nSubtileY
                                && nSubtileY < pCandidate->tCoords.nSubtileY + pCandidate->tCoords.nSubtileHeight)
                            {
                                pRoomFound = pCandidate;
                            }
                        }
                    }
                    pRoom = pRoomFound ? pRoomFound : D2COMMON_UNITS_GetRoom(pUnit);
                }

                //  0x22 == COLLIDE_VISIBLE | COLLIDE_BLANK, asserted above
                if (D2COMMON_COLLISION_CheckMask(pRoom, gnLightGridOriginX + nGridX, gnLightGridOriginY + nGridY, (COLLIDE_VISIBLE | COLLIDE_BLANK)))
                {
                    gPlayerLightGrid[nGridX + nCellOffset].nLit = 1;
                }
            }
            nCellOffset += 48;
        }
    }

    nQualityLevel = DLIGHTMAP_UpdateQualityLevel();

    gnActiveDynamicLights = 0;
    pLight = gpDynamicLights;
    while (pLight)
    {
        ++gnActiveDynamicLights;
        pNext = pLight->pNext;
        DLIGHTMAP_Update(pLight, nQualityLevel);
        pLight = pNext;
    }
}

//1.10f: D2Client.0x6FAA4D00
//  The per-light frame update, the largest function in the file: called once per light by DLIGHTMAP_UpdateForPlayer's walk of gpDynamicLights with
//  the frame's quality level from DLIGHTMAP_UpdateQualityLevel.
//      1. The owner is re-read -- skipped when bOrphaned, when dwUnitType is UNIT_TYPES_COUNT, or when the unit is not found -- by
//         CUNIT_FindUnitByClientIdAndType when bUnitIsAsync and by CUNIT_FindUnitByGuidAndType otherwise; nSubtileX/Y become (precision >> 13) + 4,
//         the same subtile centre DLIGHTMAP_AddLight stored.
//      2. nRadius steps 8 (one subtile, the radius being in eighths) toward nTargetRadius, down when the target is equal or lower
//         (target <= radius, 0x6FAA4D94); a static light drops its baked map first (fStaticValid = 0, pnStaticMap freed and NULLed).
//      3. 256 or more clamps to 248 (0x6FAA4DAA).
//      4. bOrphaned && nRadius <= 0 removes the light outright (the tail jump at 0x6FAA4DC4).
//      5. The dispatch, which is what the quality level is for:
//         nType 0 and quality above 1    the shadowed falloff, inline below;
//         nType 2 (static), any quality  allocate pnStaticMap when absent (4 * (2 * (nRadius >> 3) + 1)^2 bytes), build it
//                                        when !fStaticValid, apply it through DLIGHTMAP_ApplyLight;
//         everything else                DLIGHTMAP_ApplyLightSource, which takes the quality level.
//  Only nType 2 has a name of its own (LIGHT_TYPE_STATIC); 0 and 1 stay literals.
//  The inline shadow path:
//      - a window test sends the light home when its span lies off the ambient grid: the four compares at 0x6FAA4E25..0x6FAA4E65 against
//        gnLightGridOriginX/Y and EndX/Y;
//      - gLightGrid is cleared, the first 0x1000 bytes of its 0x4000, as the original does;
//      - DLIGHTMAP_SeedStaticGridFromAmbient writes the light's own square of gStaticLightGrid, a different global from the gLightGrid
//        cleared above and read below, from the ambient grid;
//      - the ring walk: nRowIdx runs 2..nRadiusCells and each step of the inner loop makes sixteen calls, DLIGHTMAP_SetupPropagation at one
//        of eight coordinates around the light's cell, then DLIGHTMAP_PropagateCell along the matching row or column. Four of the eight
//        coordinates march 8 (one cell) per inner step and four per ring, all of them outward;
//      - the falloff proper: over the (nCellSpan + 1)-square window at nCellOriginX/Y, every grid cell below 16 adds
//        ((8 - (value >> 1)) * (nIntensityPerCell * (nRadius - da) >> 16)) >> 3 through DLIGHTMAP_AddLightToGrid at the cell's world position,
//        da from DFLOOR_APPROX_Distance.
//  The window base is &gLightGrid[65 * nGridRow] (0x6FAA50A9) with nGridRow = 32 - (nRadius >> 3); 65 dwords once and 64 per row after is the
//  original's layout. The walk's last index is 65 * (32 - (nRadius >> 3) + nCellSpan).
//  Two tests the original makes are dead and are not written: nGridH > 32 (nGridH starts at 30 and only falls) and nRadiusCells2 < 0 (nRadiusCells2
//  is (2 * nRadius) >> 3 with nRadius positive here).
void __fastcall DLIGHTMAP_Update(D2ClientLightStrc *pLight, int32_t nQualityLevel)
{
    D2UnitStrc *pUnit;
    D2C_UnitTypes nUnitType;
    int32_t nType;
    int32_t nRadius;
    int32_t nCellOriginX;
    int32_t nCellOriginY;
    int32_t nCellSpan;
    int32_t nRadiusCells;
    int32_t nCellX;
    int32_t nCellY;
    int32_t nRowIdx;
    int32_t nGridH;
    int32_t nGridW1;
    int32_t nGridW2;
    int32_t nX;
    int32_t nX3;
    int32_t nX4;
    int32_t nX5;
    int32_t nY;
    int32_t nY2;
    int32_t nY3;
    int32_t nY4;
    int32_t nIntensityPerCell;
    int32_t nGridRow;
    int32_t nCellCount;
    int32_t nRowCount;
    int32_t nColCount;
    int32_t nRowCellY;
    int32_t nColCellX;
    int32_t nDeltaY;
    int32_t nDX;
    int32_t nDist;
    int32_t nCellIntensity;
    int32_t nDiameterCells;
    uint32_t *pGridRowBase;
    uint32_t *pGridBase;
    int32_t nGridValue;

    if (!pLight->bOrphaned)
    {
        nUnitType = pLight->dwUnitType;
        if (nUnitType != UNIT_TYPES_COUNT)
        {
            pUnit = pLight->bUnitIsAsync ? CUNIT_FindUnitByClientIdAndType(pLight->dwUnitId, nUnitType)
                        : CUNIT_FindUnitByGuidAndType(pLight->dwUnitId, nUnitType);
            if (pUnit)
            {
                pLight->nSubtileX = (D2COMMON_UNITS_GetPrecisionX(pUnit) >> 13) + 4;
                pLight->nSubtileY = (D2COMMON_UNITS_GetPrecisionY(pUnit) >> 13) + 4;
            }
        }
    }

    if (pLight->nTargetRadius != pLight->nRadius)
    {
        if (pLight->nType == LIGHT_TYPE_STATIC)
        {
            pLight->fStaticValid = 0;
            if (pLight->pnStaticMap)
                FOG_Free(pLight->pnStaticMap, szSourceFile, 1228, 0);
            pLight->pnStaticMap = NULL;
        }
        nRadius = pLight->nRadius;
        if (pLight->nTargetRadius <= nRadius)
            nRadius -= 8;
        else
            nRadius += 8;
        pLight->nRadius = nRadius;
    }

    if (pLight->nRadius >= 256)
        pLight->nRadius = 248;

    if (pLight->bOrphaned && pLight->nRadius <= 0)
    {
        DLIGHTMAP_RemoveLight(pLight);
        return;
    }

    nType = pLight->nType;
    if (nType == 0 && nQualityLevel > 1)
    {
        //  the shadow path, for a type-0 light above quality 1; the clamp above has left nRadius in 1..255 and both bounds are still written out,
        //  as the bytes carry them
        nRadius = pLight->nRadius;
        if (nRadius > 0 && nRadius < 256)
        {
            nCellOriginX = pLight->nSubtileX - (pLight->nSubtileX & 7) - nRadius;
            nCellOriginY = pLight->nSubtileY - (pLight->nSubtileY & 7) - nRadius;
            nCellSpan = (2 * nRadius) >> 3;
            if ((nCellOriginX >> 3) <= gnLightGridEndX && (nCellOriginX >> 3) + nCellSpan + 1 >= gnLightGridOriginX && (nCellOriginY >> 3) <= gnLightGridEndY && (nCellOriginY >> 3) + nCellSpan + 1 >= gnLightGridOriginY)
            {
                memset(gLightGrid, 0, 0x1000u);

                nRadiusCells = pLight->nRadius >> 3;
                nCellX = pLight->nSubtileX >> 3;
                nCellY = pLight->nSubtileY >> 3;

                DLIGHTMAP_SeedStaticGridFromAmbient(pLight);

                nRowIdx = 2;
                if (nRadiusCells >= 2)
                {
                    nGridH = 30;
                    nX4 = 8 * nCellX + 20;
                    nY4 = 8 * nCellY + 20;
                    nY = 8 * nCellY - 12;
                    nX5 = 8 * nCellX - 12;
                    do
                    {
                        nGridW1 = 32;
                        nGridW2 = 32;
                        nY2 = 8 * nCellY + 4;
                        nX3 = 8 * nCellX + 4;
                        nY3 = nY2;
                        nX = nX3;
                        do
                        {
                            DLIGHTMAP_SetupPropagation(nX, nY, pLight);
                            DLIGHTMAP_PropagateCell(nGridW2, nGridH);
                            DLIGHTMAP_SetupPropagation(nX3, nY, pLight);
                            DLIGHTMAP_PropagateCell(nGridW1, nGridH);
                            DLIGHTMAP_SetupPropagation(nX, nY4, pLight);
                            DLIGHTMAP_PropagateCell(nGridW2, nRowIdx + 32);
                            DLIGHTMAP_SetupPropagation(nX3, nY4, pLight);
                            DLIGHTMAP_PropagateCell(nGridW1, nRowIdx + 32);
                            DLIGHTMAP_SetupPropagation(nX4, nY3, pLight);
                            DLIGHTMAP_PropagateCell(nRowIdx + 32, nGridW2);
                            DLIGHTMAP_SetupPropagation(nX4, nY2, pLight);
                            DLIGHTMAP_PropagateCell(nRowIdx + 32, nGridW1);
                            DLIGHTMAP_SetupPropagation(nX5, nY3, pLight);
                            DLIGHTMAP_PropagateCell(nGridH, nGridW2);
                            DLIGHTMAP_SetupPropagation(nX5, nY2, pLight);
                            DLIGHTMAP_PropagateCell(nGridH, nGridW1);
                            nX3 += 8;
                            nY2 += 8;
                            --nGridW2;
                            ++nGridW1;
                            nX -= 8;
                            nY3 -= 8;
                        } while (nGridW1 - 32 <= nRowIdx);
                        nX4 += 8;
                        nY4 += 8;
                        --nGridH;
                        nX5 -= 8;
                        nY -= 8;
                    } while (++nRowIdx <= nRadiusCells);
                }

                nIntensityPerCell = (pLight->nIntensity << 16) / pLight->nRadius;
                nGridRow = 32 - (pLight->nRadius >> 3);
                nCellCount = nCellSpan + 1;
                //  Two pointers, not one: the cursor walks a row and each row restarts it
                //  from a separate row base, 64 dwords on (0x6FAA513A..0x6FAA5150).
                pGridRowBase = &gLightGrid[65 * nGridRow];
                nRowCount = nCellCount;
                nRowCellY = nCellOriginY;
                do
                {
                    nDeltaY = pLight->nSubtileY - nRowCellY;
                    if (nDeltaY < 0)
                        nDeltaY = -nDeltaY;
                    nColCount = nCellCount;
                    nColCellX = nCellOriginX;
                    pGridBase = pGridRowBase;
                    do
                    {
                        nGridValue = (int32_t)*pGridBase;
                        if (nGridValue < 16)
                        {
                            nDX = pLight->nSubtileX - nColCellX;
                            if (nDX < 0)
                                nDX = -nDX;
                            nDist = (int32_t)DFLOOR_APPROX_Distance(nDX, nDeltaY);
                            nCellIntensity = ((8 - (nGridValue >> 1)) * ((nIntensityPerCell * (pLight->nRadius - nDist)) >> 16)) >> 3;
                            if (nCellIntensity > 0)
                                DLIGHTMAP_AddLightToGrid(nColCellX, nRowCellY, nCellIntensity, pLight->nRed, pLight->nGreen, pLight->nBlue);
                        }
                        ++pGridBase;
                        nColCellX += 8;
                    } while (--nColCount);
                    pGridRowBase += 64;
                    nRowCellY += 8;
                } while (--nRowCount);
            }
        }
    }
    else if (nType == LIGHT_TYPE_STATIC)
    {
        if (!pLight->fStaticValid)
        {
            if (!pLight->pnStaticMap)
            {
                nDiameterCells = 2 * (pLight->nRadius >> 3) + 1;
                pLight->pnStaticMap = (int32_t *)FOG_Alloc(4 * nDiameterCells * nDiameterCells, szSourceFile, 1265, 0);
            }
            DLIGHTMAP_BuildStaticMap(pLight);
        }
        DLIGHTMAP_ApplyLight(pLight);
    }
    else
    {
        DLIGHTMAP_ApplyLightSource(pLight, nQualityLevel);
    }
}

//1.10f: D2Client.0x6FAA51D0
//  Accumulates one light into one ambient cell. The world subtile (nX, nY) becomes the cell (nX >> 3) - gnLightGridOriginX, bounded 0..47 on both
//  axes; out of range returns having done nothing. void: no caller reads the return.
//  Intensity adds and saturates at 255. The colour is a weighted average of the new contribution and what the cell already held, weighted by their
//  intensities, which is where gnReciprocal16 earns its place:
//
//      channel = (gnReciprocal16[total] * (nIntensity * colourNew + nOldIntensity * colourOld)) >> 16
//
//  gnReciprocal16[i] is 65536 / i, so that is (...) / total done with a multiply, and the index is the saturated total; each channel then clamps to
//  255 on its own.
//  BUG: the weight and the index come from two different values: the blend multiplies by the original nIntensity (the register keeps it while the
//  saturated copy goes to a stack slot) and divides through the reciprocal of the saturated sum. Once the total passes 255 the average is scaled by
//  total / 255, up to twice, before each channel's own clamp, so overlapping lights wash their colour out toward white. REACHABLE: the index has to
//  stay inside the 256-entry table, and nothing scaled the weights to match.
//  gbColoredLights decides whether there is colour: nonzero, the three channels blend as above; zero, the intensity is stored and all three colour
//  bytes are zeroed.
void __fastcall DLIGHTMAP_AddLightToGrid(int32_t nX, int32_t nY, int32_t nIntensity, uint8_t nRed, uint8_t nGreen, uint8_t nBlue)
{
    int32_t nCellX;
    int32_t nCellY;
    D2LightGridCellStrc *pCell;
    int32_t nOldIntensity;
    int32_t nNewIntensity;
    int32_t nReciprocal;
    int32_t nNewRed;
    int32_t nNewGreen;
    int32_t nNewBlue;

    nCellX = (nX >> 3) - gnLightGridOriginX;
    if (nCellX >= 0 && nCellX < 0x30)
    {
        nCellY = (nY >> 3) - gnLightGridOriginY;
        if (nCellY >= 0 && nCellY < 0x30)
        {
            pCell = &gPlayerLightGrid[nCellX + 48 * nCellY];
            nOldIntensity = pCell->nIntensity;
            nNewIntensity = nOldIntensity + nIntensity;
            if (nNewIntensity >= 255)
                nNewIntensity = 255;
            //  BUG: gbColoredLights is 1 in the image and nothing in the module writes it, so the else arm below is dead. UNREACHABLE: a stock
            //  client never clears it; reproduced whole.
            if (gbColoredLights)
            {
                //  The three >> 16 are logical (shr eax, 10h at 0x6FAA526C, shr edx at 0x6FAA5295, shr ecx at 0x6FAA52BF), so the product is taken
                //  unsigned; it never reaches bit 31 (257 * 255 * 510 at the most).
                nReciprocal = gnReciprocal16[nNewIntensity];
                nNewRed = (int32_t)((uint32_t)(nReciprocal * (nIntensity * nRed + nOldIntensity * pCell->nRed)) >> 16);
                if (nNewRed > 0xFF)
                    nNewRed = 0xFF;
                nNewGreen = (int32_t)((uint32_t)(nReciprocal * (nIntensity * nGreen + nOldIntensity * pCell->nGreen)) >> 16);
                if (nNewGreen > 0xFF)
                    nNewGreen = 0xFF;
                nNewBlue = (int32_t)((uint32_t)(nReciprocal * (nIntensity * nBlue + nOldIntensity * pCell->nBlue)) >> 16);
                if (nNewBlue > 0xFF)
                    nNewBlue = 0xFF;
                pCell->nRed = nNewRed;
                pCell->nIntensity = nNewIntensity;
                pCell->nGreen = nNewGreen;
                pCell->nBlue = nNewBlue;
            }
            else
            {
                pCell->nIntensity = nNewIntensity;
                pCell->nRed = 0;
                pCell->nGreen = 0;
                pCell->nBlue = 0;
            }
        }
    }
}

//1.10f: D2Client.0x6FAA5300
//  The cheap light path, "the plain radial falloff": reached from DLIGHTMAP_Update for every light that is not a type-0 light above quality 1 and
//  not a static one; it lands in DLIGHTMAP_AddLightToGrid once per cell of the light's square.
//  Quality 0 is not merely cheaper, it is per-unit-type capped. With nQualityLevel 0 the switch runs on dwUnitType, not on a light kind:
//
//      UNIT_PLAYER (0)   when the light's dwUnitId differs from the player's (no player counts as -1) the radius is capped at 16; the local
//                        player's is left alone;
//      UNIT_MONSTER (1)  radius forced to 8;
//      UNIT_MISSILE (3)  return without drawing anything;
//      everything else   keep the stored radius.
//
//  Above quality 0 the whole switch is skipped.
//  The falloff is an octagon. Both distances are axis differences and the pair (983, 407) is the integer stand-in for sqrt:
//
//      if (|dx| < |dy|) { max = 983*|dy|; min = 407*|dx|; }
//      else             { max = 983*|dx|; min = 407*|dy|; }
//      intensity = (nFalloffStep * (nRadius - ((min + max) >> 10))) >> 16
//
//  nFalloffStep is (nIntensity << 16) / nRadius, which makes the brightness fall linearly to zero at the radius. Non-positive intensities are
//  dropped.
//  The radius read is nRadius (+0x18), the live one, and the window test is the same four compares against gnLightGridOrigin/EndX/Y that
//  DLIGHTMAP_Update's inline path runs.
void __fastcall DLIGHTMAP_ApplyLightSource(D2ClientLightStrc *pLight, int32_t nQualityLevel)
{
    D2UnitStrc *pPlayer;
    uint32_t dwUnitId;
    int32_t nRadius;
    int32_t nStartX;
    int32_t nStartY;
    int32_t nSpan;
    int32_t nX;
    int32_t nY;
    int32_t nColCount;
    int32_t nRowCount;
    int32_t nFalloffStep;
    int32_t nDistX;
    int32_t nDistY;
    int32_t nWeightedMax;
    int32_t nWeightedMin;
    int32_t nIntensity;

    nRadius = pLight->nRadius;
    if (nRadius <= 0 || nRadius >= 256)
        return;

    if (nQualityLevel == 0)
    {
        if (pLight->dwUnitType == UNIT_PLAYER)
        {
            pPlayer = CUNIT_GetPlayer();
            if (pPlayer)
                dwUnitId = pPlayer->dwUnitId;
            else
                dwUnitId = (uint32_t)-1;
            if (pLight->dwUnitId != dwUnitId && nRadius >= 16)
                nRadius = 16;
        }
        else if (pLight->dwUnitType == UNIT_MONSTER)
        {
            nRadius = 8;
        }
        else if (pLight->dwUnitType == UNIT_MISSILE)
        {
            return;
        }
    }

    nStartX = pLight->nSubtileX - (pLight->nSubtileX & 7) - nRadius;
    nStartY = pLight->nSubtileY - (pLight->nSubtileY & 7) - nRadius;
    nSpan = (2 * nRadius) >> 3;
    if ((nStartX >> 3) <= gnLightGridEndX && (nStartX >> 3) + nSpan + 1 >= gnLightGridOriginX && (nStartY >> 3) <= gnLightGridEndY && (nStartY >> 3) + nSpan + 1 >= gnLightGridOriginY)
    {
        nFalloffStep = (pLight->nIntensity << 16) / nRadius;
        if (nSpan < 0)
        {
            return;
        }
        nRowCount = nSpan + 1;
        nY = nStartY;
        do
        {
            nDistY = pLight->nSubtileY - nY;
            if (nDistY < 0)
                nDistY = -nDistY;
            nX = nStartX;
            nColCount = nSpan + 1;
            do
            {
                nDistX = pLight->nSubtileX - nX;
                if (nDistX < 0)
                    nDistX = -nDistX;
                if (nDistX < nDistY)
                {
                    nWeightedMax = 983 * nDistY;
                    nWeightedMin = 407 * nDistX;
                }
                else
                {
                    nWeightedMax = 983 * nDistX;
                    nWeightedMin = 407 * nDistY;
                }
                //  the octagon's >> 10 is logical (shr ecx, 0Ah at 0x6FAA546D), the step's >> 16 arithmetic (sar at 0x6FAA5477)
                nIntensity = (nFalloffStep * (nRadius - (int32_t)((uint32_t)(nWeightedMin + nWeightedMax) >> 10))) >> 16;
                if (nIntensity > 0)
                    DLIGHTMAP_AddLightToGrid(nX, nY, nIntensity, pLight->nRed, pLight->nGreen, pLight->nBlue);
                nX += 8;
            } while (--nColCount);
            nY += 8;
        } while (--nRowCount);
    }
}

//1.10f: D2Client.0x6FAA54D0
//  Seeds gStaticLightGrid (not the gLightGrid that DLIGHTMAP_Update's inline path clears and then reads) from the ambient grid; run by that path
//  between its grid clear and the ring walk. Both globals are indexed 65 * (32 - (nRadius >> 3)) dwords to the first row and 64 more per row, which
//  is the indexing the ring walk and the apply loop share.
//  The square is (nSpan + 1) cells on a side with nSpan = (2 * nRadius) >> 3, and each cell is filled from the 48x48 ambient grid gPlayerLightGrid:
//  one .nLit lookup per cell at 48 * nGridY + nGridX after the same origin subtraction DLIGHTMAP_AddLightToGrid uses, with 1 substituted wherever
//  the cell falls outside 0..47. What is stored is 16 * nLitValue.
//  void: the register the original leaves in
//  eax carries a grid index with its low byte overwritten by the 1 of the bounds test, and the sole caller never reads it.
void __fastcall DLIGHTMAP_SeedStaticGridFromAmbient(D2ClientLightStrc *pLight)
{
    int32_t nRadius;
    int32_t nStartX;
    int32_t nSubtileX;
    int32_t nSubtileY;
    int32_t nSpan;
    int32_t nColCount;
    int32_t nRowCount;
    int32_t nGridX;
    int32_t nGridY;
    uint8_t nLitValue;
    int32_t *pnRow;
    int32_t *pnCell;

    nRadius = pLight->nRadius;
    nStartX = pLight->nSubtileX - (pLight->nSubtileX & 7) - nRadius;
    nSpan = (2 * nRadius) >> 3;
    nSubtileY = pLight->nSubtileY - (pLight->nSubtileY & 7) - nRadius;
    if (nSpan < 0)
    {
        return;
    }
    nRowCount = nSpan + 1;
    pnRow = &gStaticLightGrid[65 * (32 - (nRadius >> 3))];
    do
    {
        nSubtileX = nStartX;
        pnCell = pnRow;
        nColCount = nSpan + 1;
        do
        {
            nGridX = (nSubtileX >> 3) - gnLightGridOriginX;
            if (nGridX >= 0 && nGridX < 48)
            {
                nGridY = (nSubtileY >> 3) - gnLightGridOriginY;
                if (nGridY >= 0 && nGridY < 48)
                    nLitValue = gPlayerLightGrid[48 * nGridY + nGridX].nLit;
                else
                    nLitValue = 1;
            }
            else
            {
                nLitValue = 1;
            }
            *pnCell = 16 * nLitValue;
            ++pnCell;
            nSubtileX += 8;
        } while (--nColCount);
        pnRow += 64;
        nSubtileY += 8;
    } while (--nRowCount);
}

//1.10f: D2Client.0x6FAA55D0
//  A Bresenham setup, one octant at a time: classifies the direction from cell (nX, nY) toward the light at pLight->nSubtileX/Y and leaves it in
//  four globals for DLIGHTMAP_PropagateCell, which is always called immediately after; the two are one operation split in two.
//
//      gnPropagateMode   0  the cell is the source, nothing to do
//                        1  purely horizontal (dy == 0)
//                        4  purely vertical   (dx == 0)
//                        2  x-major, gnPropagateFrac = (|dy| << 8) / |dx|
//                        3  y-major, gnPropagateFrac = (|dx| << 8) / |dy|
//
//      gnPropagateStepX  +1 or -1, the sign of dx
//      gnPropagateStepY  +1 or -1, the sign of dy
//
//  Modes 2 and 3 carry the 8.8 interpolation weight; 1 and 4 do not, so a diagonal ray blends two neighbours while an axial one copies one.
//  One harmless oddity, reproduced below: a pure-vertical ray still takes the arm that stores gnPropagateStepX = 1, because the dx == 0 case jumps
//  into it; mode 4 never reads that global.
//  void: eax is left holding the 8.8 interpolation fraction, or the -1 sentinel for modes 0, 1 and 4, and no caller reads it.
void __fastcall DLIGHTMAP_SetupPropagation(int32_t nX, int32_t nY, D2ClientLightStrc *pLight)
{
    int32_t nAbsDeltaX;
    int32_t nAbsDeltaY;
    int32_t nStepFrac;

    nAbsDeltaX = pLight->nSubtileX - nX;
    nAbsDeltaY = pLight->nSubtileY - nY;
    nStepFrac = -1;

    if (nAbsDeltaX == 0 && nAbsDeltaY == 0)
    {
        gnPropagateMode = 0;
        return;
    }

    if (nAbsDeltaX >= 0)
    {
        gnPropagateStepX = 1;
    }
    else
    {
        gnPropagateStepX = -1;
        nAbsDeltaX = -nAbsDeltaX;
    }

    if (nAbsDeltaY >= 0)
    {
        gnPropagateStepY = 1;
    }
    else
    {
        gnPropagateStepY = -1;
        nAbsDeltaY = -nAbsDeltaY;
    }

    if (nAbsDeltaX != 0)
    {
        if (nAbsDeltaY != 0)
        {
            if (nAbsDeltaX < nAbsDeltaY)
            {
                gnPropagateMode = 3;
                nStepFrac = (nAbsDeltaX << 8) / nAbsDeltaY;
            }
            else
            {
                gnPropagateMode = 2;
                nStepFrac = (nAbsDeltaY << 8) / nAbsDeltaX;
            }
            gnPropagateFrac = nStepFrac;
        }
        else
        {
            gnPropagateMode = 1;
        }
    }
    else
    {
        gnPropagateMode = 4;
    }

}

//1.10f: D2Client.0x6FAA5680
//  Propagates into one cell, the second half of the pair: a switch on gnPropagateMode. Every neighbour read is gStaticLightGrid[i] ? ... :
//  gLightGrid[i] (the seeded static grid wins wherever it is nonzero) and the result is written to gLightGrid[nX + (nY << 6)]. So the 64-wide pair
//  is a source and a working copy; this function only reads the static grid, so what was laid down survives every pass.
//
//      1  copy the neighbour at +gnPropagateStepX
//      4  copy the neighbour at +gnPropagateStepY
//      2  blend the X neighbour with the X+Y one, by gnPropagateFrac: (diagonal * frac + direct * (256 - frac)) >> 8
//      3  the same blend along Y
//      0  nothing to do, the cell is the source
//
//  Both grids are indexed [nX + (nY << 6)]; the << 6 fixes the 64-dword stride. The 256 is the 1.0 of the 8.8 fixed-point fraction, so a diagonal
//  ray steps Bresenham-style between its two axial neighbours. The setup's four globals are globals rather than an out-parameter, which is why the
//  pair must never be interleaved with another propagation.
void __fastcall DLIGHTMAP_PropagateCell(int32_t nX, int32_t nY)
{
    int32_t nRowBase;
    int32_t nSrcIdx;
    int32_t nSrcValue;
    int32_t nSrc2Idx;
    int32_t nSrc2Value;

    switch (gnPropagateMode)
    {
        case 1:
            nRowBase = nY << 6;
            nSrcIdx = nRowBase + nX + gnPropagateStepX;
            nSrcValue = gStaticLightGrid[nSrcIdx];
            if (!nSrcValue)
                nSrcValue = gLightGrid[nSrcIdx];
            gLightGrid[nRowBase + nX] = nSrcValue;
            break;

        case 2:
            nRowBase = nY << 6;
            nSrcIdx = nRowBase + nX + gnPropagateStepX;
            nSrcValue = gStaticLightGrid[nSrcIdx];
            if (!nSrcValue)
                nSrcValue = gLightGrid[nSrcIdx];
            nSrc2Idx = ((nY + gnPropagateStepY) << 6) + nX + gnPropagateStepX;
            nSrc2Value = gStaticLightGrid[nSrc2Idx];
            if (!nSrc2Value)
                nSrc2Value = gLightGrid[nSrc2Idx];
            gLightGrid[nRowBase + nX] = (nSrc2Value * gnPropagateFrac + nSrcValue * (256 - gnPropagateFrac)) >> 8;
            break;

        case 3:
            nSrcIdx = ((nY + gnPropagateStepY) << 6) + nX;
            nSrcValue = gStaticLightGrid[nSrcIdx];
            if (!nSrcValue)
                nSrcValue = gLightGrid[nSrcIdx];
            nSrc2Idx = ((nY + gnPropagateStepY) << 6) + nX + gnPropagateStepX;
            nSrc2Value = gStaticLightGrid[nSrc2Idx];
            if (!nSrc2Value)
                nSrc2Value = gLightGrid[nSrc2Idx];
            gLightGrid[(nY << 6) + nX] = (nSrc2Value * gnPropagateFrac + nSrcValue * (256 - gnPropagateFrac)) >> 8;
            break;

        case 4:
            nSrcIdx = ((nY + gnPropagateStepY) << 6) + nX;
            nSrcValue = gStaticLightGrid[nSrcIdx];
            if (!nSrcValue)
                nSrcValue = gLightGrid[nSrcIdx];
            gLightGrid[(nY << 6) + nX] = nSrcValue;
            break;
    }
}

//1.10f: D2Client.0x6FAA57E0
//  Bakes a static light's map, the nType 2 worker: run by DLIGHTMAP_Update only while fStaticValid is 0, and what it produces is what
//  DLIGHTMAP_ApplyLight spends. Where DLIGHTMAP_SeedStaticGridFromAmbient fills gStaticLightGrid from the ambient grid, this fills it from
//  collision: 64 dwords per row, 64 rows, each from D2COMMON_COLLISION_CheckMask at i + X - 32 across and row + Y - 32 down from the owner's
//  position, 16 where the mask fires and 0 where it does not.
//  Then the same ring walk as DLIGHTMAP_Update's inline path (the SetupPropagation/PropagateCell pairs, sixteen calls per ring cell), and the
//  square of (nRadiusCells2 + 1) dwords at row 32 - (nRadius >> 3) of gLightGrid, 65-wide stride, is copied out flat into pLight->pnStaticMap, 64
//  more per row, before fStaticValid = 1 closes the bake.
//  The three asserts are the contract: hRoom for a room to bake against, !ptLight->fStaticValid so it refuses to redo work already done, and
//  ptLight->pnStaticMap for the buffer it is about to fill. The owner lookup is the same bUnitIsAsync fork as DLIGHTMAP_Update's, except that
//  UNIT_TYPES_COUNT returns here instead of being skipped.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here, the same union-pointer arm as DLIGHTMAP_UpdateForPlayer's. The ring walk's
//  nGridH <= 32 guard and the copy-out's nRadiusCells2 >= 0 test are dead for the reasons DLIGHTMAP_Update gives, and neither is written.
void __fastcall DLIGHTMAP_BuildStaticMap(D2ClientLightStrc *pLight)
{
    int32_t nRadius;
    int32_t nRadiusCells2;
    int32_t nRadiusCells;
    int32_t nCellX;
    int32_t nCellY;
    D2C_UnitTypes nUnitType;
    D2UnitStrc *pUnit;
    D2ActiveRoomStrc *pRoom;
    int32_t nUnitX;
    int32_t nUnitY;
    int32_t nColIdx;
    uint16_t nCollisionMask;
    int32_t i;
    int32_t *pStaticGrid;
    int32_t nRowIdx;
    int32_t nGridH;
    int32_t nX4;
    int32_t nY4;
    int32_t nY;
    int32_t nX5;
    int32_t nGridW1;
    int32_t nGridW2;
    int32_t nY2;
    int32_t nX3;
    int32_t nY3;
    int32_t nX;
    int32_t nSpan;
    int32_t nColCount;
    int32_t *pnDest;
    uint32_t *pGridBase;
    uint32_t *pGridCursor;

    nRadius = pLight->nRadius;
    if (nRadius <= 0 || nRadius >= 256)
        return;

    nRadiusCells2 = (2 * nRadius) >> 3;
    memset(gLightGrid, 0, 0x1000u);

    nUnitType = pLight->dwUnitType;
    nRadiusCells = pLight->nRadius >> 3;
    nCellX = pLight->nSubtileX >> 3;
    nCellY = pLight->nSubtileY >> 3;
    if (nUnitType == UNIT_TYPES_COUNT)
        return;

    pUnit = pLight->bUnitIsAsync ? CUNIT_FindUnitByClientIdAndType(pLight->dwUnitId, nUnitType)
                : CUNIT_FindUnitByGuidAndType(pLight->dwUnitId, nUnitType);
    if (!pUnit)
        return;

    pRoom = D2COMMON_UNITS_GetRoom(pUnit);
    if (!pRoom)
    {
        FOG_DisplayAssert(szAssertHRoom, szSourceFile, 1044);
        exit(-1);
    }

    nUnitX = UNITS_GetXPosition(pUnit);

    nUnitY = UNITS_GetYPosition(pUnit);

    //  The collision fill: row-major over the whole 64x64 static grid.
    //  DIFFERENCE: the original's bound is the address of gnLightGridOriginX, the dword just past gStaticLightGrid (cmp ebp, 6FB9E9E0h at
    //  0x6FAA5937, a signed jl at 0x6FAA5941). Ours is gStaticLightGrid's own end, both sides taken as int: the two are separate objects here, and
    //  a pointer into one compared with the other's address is no bound C++ defines. The same value, the same 4096 cells.
    nColIdx = 0;
    pStaticGrid = gStaticLightGrid;
    do
    {
        for (i = 0; i < 64; ++i)
        {
            nCollisionMask = D2COMMON_COLLISION_CheckMask(pRoom, i + nUnitX - 32, nColIdx + nUnitY - 32, (COLLIDE_VISIBLE | COLLIDE_BLANK));
            *pStaticGrid++ = nCollisionMask != 0 ? 16 : 0;
        }
        ++nColIdx;
    } while ((int32_t)pStaticGrid < (int32_t)(gStaticLightGrid + 4096));

    //  The ring walk, pair for pair the one DLIGHTMAP_Update's inline path runs.
    nRowIdx = 2;
    if (nRadiusCells >= 2)
    {
        nGridH = 30;
        nX4 = 8 * nCellX + 20;
        nY4 = 8 * nCellY + 20;
        nY = 8 * nCellY - 12;
        nX5 = 8 * nCellX - 12;
        do
        {
            nGridW1 = 32;
            nGridW2 = 32;
            nY2 = 8 * nCellY + 4;
            nX3 = 8 * nCellX + 4;
            nY3 = nY2;
            nX = nX3;
            do
            {
                DLIGHTMAP_SetupPropagation(nX, nY, pLight);
                DLIGHTMAP_PropagateCell(nGridW2, nGridH);
                DLIGHTMAP_SetupPropagation(nX3, nY, pLight);
                DLIGHTMAP_PropagateCell(nGridW1, nGridH);
                DLIGHTMAP_SetupPropagation(nX, nY4, pLight);
                DLIGHTMAP_PropagateCell(nGridW2, nRowIdx + 32);
                DLIGHTMAP_SetupPropagation(nX3, nY4, pLight);
                DLIGHTMAP_PropagateCell(nGridW1, nRowIdx + 32);
                DLIGHTMAP_SetupPropagation(nX4, nY3, pLight);
                DLIGHTMAP_PropagateCell(nRowIdx + 32, nGridW2);
                DLIGHTMAP_SetupPropagation(nX4, nY2, pLight);
                DLIGHTMAP_PropagateCell(nRowIdx + 32, nGridW1);
                DLIGHTMAP_SetupPropagation(nX5, nY3, pLight);
                DLIGHTMAP_PropagateCell(nGridH, nGridW2);
                DLIGHTMAP_SetupPropagation(nX5, nY2, pLight);
                DLIGHTMAP_PropagateCell(nGridH, nGridW1);
                nX3 += 8;
                nY2 += 8;
                --nGridW2;
                ++nGridW1;
                nX -= 8;
                nY3 -= 8;
            } while (nGridW1 - 32 <= nRowIdx);
            nX4 += 8;
            nY4 += 8;
            --nGridH;
            nX5 -= 8;
            nY -= 8;
        } while (++nRowIdx <= nRadiusCells);
    }

    if (pLight->fStaticValid)
    {
        FOG_DisplayAssert(szAssertNotPtLightFStaticValid, szSourceFile, 1108);
        exit(-1);
    }
    pnDest = pLight->pnStaticMap;
    if (!pnDest)
    {
        FOG_DisplayAssert(szAssertPtLightPnStaticMap, szSourceFile, 1109);
        exit(-1);
    }

    nSpan = nRadiusCells2 + 1;
    pGridBase = &gLightGrid[65 * (32 - (pLight->nRadius >> 3))];
    do
    {
        pGridCursor = pGridBase;
        nColCount = nRadiusCells2 + 1;
        do
        {
            *pnDest++ = (int32_t)*pGridCursor++;
        } while (--nColCount);
        pGridBase += 64;
    } while (--nSpan);

    pLight->fStaticValid = 1;
}

//1.10f: D2Client.0x6FAA5BC0
//  Applies a light through its baked map, the static-light path: DLIGHTMAP_Update sends every nType 2 light here at any quality level
//  (0x6FAA516A..0x6FAA51AC), while DLIGHTMAP_ApplyLightSource takes nType 1 at every level and nType 0 at 0..1. The falloff is
//  DLIGHTMAP_ApplyLightSource's (the octagonal distance with the (983, 407) weights inline, the (nIntensity << 16) / nRadius step computed once)
//  with one more factor from the map:
//
//      nIntensity = (((nFalloffStep * (nRadius - (nDistance >> 10))) >> 16) * (8 - (v >> 1))) >> 3;
//
//  and a short circuit before all of it: a cell whose map value is >= 16 is skipped outright. That is how a wall shadows: the bake puts 16 into
//  blocked cells and propagation spreads it.
//  The radius bounds and the four window compares run first, so an off-screen light costs those and nothing else, and only then do the two asserts
//  fire, pnStaticMap and fStaticValid. The map is walked flat across the rows of the (nSpan + 1) square, never reset per row.
void __fastcall DLIGHTMAP_ApplyLight(D2ClientLightStrc *pLight)
{
    int32_t nRadius;
    int32_t nStartX;
    int32_t nStartY;
    int32_t nSpan;
    int32_t nY;
    int32_t nRowCount;
    int32_t nColCount;
    int32_t nX;
    int32_t nFalloffStep;
    int32_t *pnStaticMap;
    int32_t nDistX;
    int32_t nDistY;
    int32_t nWeightedMax;
    int32_t nWeightedMin;
    int32_t nIntensity;

    nRadius = pLight->nRadius;
    if (nRadius <= 0 || nRadius >= 256)
        return;

    nStartX = pLight->nSubtileX - (pLight->nSubtileX & 7) - nRadius;
    nStartY = pLight->nSubtileY - (pLight->nSubtileY & 7) - nRadius;
    nSpan = (2 * nRadius) >> 3;
    if ((nStartX >> 3) <= gnLightGridEndX && (nStartX >> 3) + nSpan + 1 >= gnLightGridOriginX && (nStartY >> 3) <= gnLightGridEndY && (nStartY >> 3) + nSpan + 1 >= gnLightGridOriginY)
    {
        if (!pLight->pnStaticMap)
        {
            FOG_DisplayAssert(szAssertPtLightPnStaticMap, szSourceFile, 1150);
            exit(-1);
        }
        if (!pLight->fStaticValid)
        {
            FOG_DisplayAssert(szAssertPtLightFStaticValid, szSourceFile, 1151);
            exit(-1);
        }

        pnStaticMap = pLight->pnStaticMap;
        nFalloffStep = (pLight->nIntensity << 16) / nRadius;
        if (nSpan < 0)
        {
            return;
        }
        nRowCount = nSpan + 1;
        nY = nStartY;
        do
        {
            nDistY = pLight->nSubtileY - nY;
            if (nDistY < 0)
                nDistY = -nDistY;
            nX = nStartX;
            nColCount = nSpan + 1;
            do
            {
                if (*pnStaticMap < 16)
                {
                    nDistX = pLight->nSubtileX - nX;
                    if (nDistX < 0)
                        nDistX = -nDistX;
                    if (nDistX < nDistY)
                    {
                        nWeightedMax = 983 * nDistY;
                        nWeightedMin = 407 * nDistX;
                    }
                    else
                    {
                        nWeightedMax = 983 * nDistX;
                        nWeightedMin = 407 * nDistY;
                    }
                    //  the same logical shift as DLIGHTMAP_ApplyLightSource's octagon (shr edx, 0Ah at 0x6FAA5D54)
                    nIntensity = (((nFalloffStep * (nRadius - (int32_t)((uint32_t)(nWeightedMin + nWeightedMax) >> 10))) >> 16) * (8 - (*pnStaticMap >> 1))) >> 3;
                    if (nIntensity > 0)
                        DLIGHTMAP_AddLightToGrid(nX, nY, nIntensity, pLight->nRed, pLight->nGreen, pLight->nBlue);
                }
                ++pnStaticMap;
                nX += 8;
            } while (--nColCount);
            nY += 8;
        } while (--nRowCount);
    }
}

//1.10f: D2Client.0x6FAA5DC0
//  Frees every light, the game's teardown call (GAME_AppModeGame, once, at shutdown). Walks gpDynamicLights; each node's pnStaticMap is freed first
//  and the node after it, and the head is nulled after the loop, not inside it.
void __cdecl DLIGHTMAP_FreeAll()
{
    D2ClientLightStrc *pLight;
    D2ClientLightStrc *pNext;
    int32_t *pnStaticMap;

    pLight = gpDynamicLights;
    if (pLight)
    {
        do
        {
            pnStaticMap = pLight->pnStaticMap;
            pNext = pLight->pNext;
            if (pnStaticMap)
                FOG_Free(pnStaticMap, szSourceFile, 1405, 0);
            FOG_Free(pLight, szSourceFile, 1407, 0);
            pLight = pNext;
        } while (pNext);
    }
    gpDynamicLights = NULL;
}

//1.10f: D2Client.0x6FAA5E10
//  The room-change callback, registered by DLIGHTMAP_Init through DUNGEON_SetActCallbackFunc and called when the dungeon changes a room. Every
//  static light (nType 2) whose baked map the change might invalidate gets fStaticValid = 0, so the next DLIGHTMAP_Update bakes it again.
//  The decision: the light's owner is found -- and unlike DLIGHTMAP_Update, whose lookup skips what it cannot find, both a UNIT_TYPES_COUNT type
//  and a failed lookup take the hSrcUnit assert (gszAssertHSrcUnit). The owner's room is taken; if it is the room that changed, the light
//  is left alone. Otherwise four points are tested against the changed room and its adjacent list: (X + r, Y), (X - r, Y), (X, Y + r) and
//  (X, Y - r), with r = nRadius >> 3 read once. If the changed room owns any of them, the light is invalidated.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here, and CLIENT_FindRoomContainingSubtile in the first two probes (it calls it
//  for the other two); ours calls it for all four.
void __fastcall DLIGHTMAP_ActCallback(D2ActiveRoomStrc *pRoom)
{
    D2ClientLightStrc *pLight;
    D2C_UnitTypes nUnitType;
    D2UnitStrc *pUnit;
    D2ActiveRoomStrc *pUnitRoom;
    D2ActiveRoomStrc *pRoomFound;
    D2ActiveRoomStrc *pRoomFound2;
    int32_t nX;
    int32_t nY;
    int32_t nRadiusCells;

    pLight = gpDynamicLights;
    while (pLight)
    {
        if (pLight->nType == LIGHT_TYPE_STATIC)
        {
            nUnitType = pLight->dwUnitType;
            pUnit = NULL;
            if (nUnitType != UNIT_TYPES_COUNT)
            {
                pUnit = pLight->bUnitIsAsync ? CUNIT_FindUnitByClientIdAndType(pLight->dwUnitId, nUnitType)
                            : CUNIT_FindUnitByGuidAndType(pLight->dwUnitId, nUnitType);
            }
            if (!pUnit)
            {
                FOG_DisplayAssert(gszAssertHSrcUnit, szSourceFile, 1425);
                exit(-1);
            }

            pUnitRoom = D2COMMON_UNITS_GetRoom(pUnit);
            if (pUnitRoom != pRoom)
            {
                nX = UNITS_GetXPosition(pUnit);

                nY = UNITS_GetYPosition(pUnit);

                nRadiusCells = pLight->nRadius >> 3;

                pRoomFound = CLIENT_FindRoomContainingSubtile(pUnitRoom, nX + nRadiusCells, nY);

                if (pRoomFound == pRoom)
                {
                    pLight->fStaticValid = 0;
                }
                else
                {
                    pRoomFound2 = CLIENT_FindRoomContainingSubtile(pUnitRoom, nX - nRadiusCells, nY);

                    if (pRoomFound2 == pRoom || CLIENT_FindRoomContainingSubtile(pUnitRoom, nX, nY + nRadiusCells) == pRoom || CLIENT_FindRoomContainingSubtile(pUnitRoom, nX, nY - nRadiusCells) == pRoom)
                    {
                        pLight->fStaticValid = 0;
                    }
                }
            }
        }
        pLight = pLight->pNext;
    }
}


//1.10f: D2Client.0x6FAA60B0
//  Reads the light at one point out of the player grid. (nX, nY) are in eighths of a subtile, and sar 3 makes them the grid's subtile cells, each
//  clamped into 0..47 rather than rejected (jge/jle, signed). The intensity is stored first and always; the three colour bytes only with
//  gbColoredLights set.
//  void: eax at the exit is either the cell's address (the lea at 0x6FAA6103) or the blue byte, and all call sites discard it.
void __fastcall DLIGHTMAP_GetLightAtPoint(int32_t nX, int32_t nY, uint8_t *pnIntensity, uint8_t *pnRed, uint8_t *pnGreen, uint8_t *pnBlue)
{
    int32_t nGridX;
    int32_t nGridY;
    D2LightGridCellStrc *pCell;

    nGridX = (nX >> 3) - gnLightGridOriginX;
    nGridY = (nY >> 3) - gnLightGridOriginY;

    if (nGridX < 0)
        nGridX = 0;
    else if (nGridX > 47)
        nGridX = 47;

    if (nGridY < 0)
        nGridY = 0;
    else if (nGridY > 47)
        nGridY = 47;

    pCell = &gPlayerLightGrid[nGridX + 48 * nGridY];
    *pnIntensity = pCell->nIntensity;
    if (gbColoredLights)
    {
        *pnRed = pCell->nRed;
        *pnGreen = pCell->nGreen;
        *pnBlue = pCell->nBlue;
    }
}

//1.10f: D2Client.0x6FAA6130
//  The light-grid debug view: D2Gfx #10070 is handed both grids and the subtile centre of the first light in gpDynamicLights, the list's head, read
//  once (0x6FAA6130) and dereferenced untested (+0x14, then +0x10). Its one caller gates it on gbDebugDrawLightGrid.
//  BUG: with the flag set and no light in the list, the first read faults at 0x14. Reproduced.
void __cdecl DLIGHTMAP_DebugDrawLightGrid()
{
    D2ClientLightStrc *pHead;

    pHead = gpDynamicLights;
    D2GFX_DebugDraw(gLightGrid, (uint32_t *)gPlayerLightGrid, pHead->nSubtileX, pHead->nSubtileY);
}


//1.10f: D2Client.0x6FAA61F0
//  Arms the warp flash for 500 ms. void: the original leaves the sum in eax, and the one caller, CUNIT_WarpUnit, writes eax before it reads it.
void __cdecl DLIGHTMAP_StartWarpFlash()
{
    gdwWarpFlashEndTime = GetTickCount() + 500;
}

//1.10f: D2Client.0x6FAA6210
//  The warp flash, called once a frame from VIEWEFFECTS_EnvUpdateViewAndLevelEffects' tail jump: an alpha box over the play area, down to 47 pixels
//  above the bottom, that fades out over the 500 ms DLIGHTMAP_StartWarpFlash armed and clears the deadline once it has passed. The compare is
//  unsigned (jbe 0x6FAA622A) and re-reads the global after GetTickCount.
//  The alpha is remaining * 255 / 500, a signed 32-bit divide: shl 8 and sub make the product (0x6FAA6277, 0x6FAA627A), imul 10624DD3h, sar edx, 5
//  and the sign fix-up divide it by 500 (0x6FAA6272..0x6FAA6286). D2Gfx #10056 takes the alpha as a byte.
//  The side panels trim it: PANEL_RIGHT stops it half a screen short of the right edge, PANEL_LEFT starts it at half the width, and PANEL_NONE or
//  PANEL_BOTH leave it full width (dec eax / jz, dec eax / jnz, 0x6FAA624E..0x6FAA6252). Half a width is the signed / 2 (cdq, sub, sar).
void __cdecl DLIGHTMAP_DrawWarpFlash()
{
    uint32_t dwTickCount;
    uint32_t dwRemaining;
    int32_t nXStart;
    int32_t nXEnd;
    int32_t nYEnd;

    nXStart = 0;
    if (!gdwWarpFlashEndTime)
        return;

    dwTickCount = GetTickCount();
    if (dwTickCount > gdwWarpFlashEndTime)
    {
        gdwWarpFlashEndTime = 0;
        return;
    }

    dwRemaining = gdwWarpFlashEndTime - dwTickCount;
    nXEnd = gnScreenWidth;
    nYEnd = gnScreenHeight - 47;

    switch (SCROLL_GetUiShiftScreen())
    {
    case PANEL_RIGHT:
        nXEnd += -(gnScreenWidth / 2);
        break;

    case PANEL_LEFT:
        nXStart = gnScreenWidth / 2;
        break;

    default:
        break;
    }

    D2GFX_DrawBoxAlpha(nXStart, 0, nXEnd, nYEnd, 0, (uint8_t)((int32_t)(dwRemaining * 255) / 500));
}

//1.10f: D2Client.0x6FAA62A0
//  Drops a pending flash; called before a cinematic plays.
void __cdecl DLIGHTMAP_CancelWarpFlash()
{
    gdwWarpFlashEndTime = 0;
}

//1.10f: D2Client.0x6FAA62B0
//  The ambient light cache: the act's colour and intensity, then the player's room's when the room's colour is not black, red, green and blue
//  tested in that order (0x6FAA6315..0x6FAA631F). The room's four bytes come back in the one stack dword the push ecx reserves; D2Common writes all
//  four unconditionally, so four locals of their own compute the same and read nothing undefined. Nothing in D2Client reads the four globals, so
//  this is a dead store once a frame, from VIEWEFFECTS_EnvUpdateViewAndLevelEffects.
void __cdecl DLIGHTMAP_EnvUpdateAmbientLightCache()
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    uint8_t nRoomIntensity;
    uint8_t nRoomRed;
    uint8_t nRoomGreen;
    uint8_t nRoomBlue;

    D2COMMON_ENVIRONMENT_GetLightColorFromAct(gpAct, &gnAmbientLightRed, &gnAmbientLightGreen, &gnAmbientLightBlue);
    gnAmbientLightIntensity = (uint8_t)D2COMMON_ENVIRONMENT_GetIntensityFromAct(gpAct);

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;

    pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
    if (!pRoom)
        return;

    D2COMMON_DUNGEON_GetRGB_IntensityFromRoom(pRoom, &nRoomIntensity, &nRoomRed, &nRoomGreen, &nRoomBlue);
    if (nRoomRed || nRoomGreen || nRoomBlue)
    {
        gnAmbientLightIntensity = nRoomIntensity;
        gnAmbientLightRed = nRoomRed;
        gnAmbientLightGreen = nRoomGreen;
        gnAmbientLightBlue = nRoomBlue;
    }
}

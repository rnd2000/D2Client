#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "dFloor.h"
#include "CoreArchive.h"

//  dFloor.cpp -- the floor-tile pass of the isometric renderer: the room floors and their edge tiles, the view-grid floors with their fades,
//  and the light grids (8x8 and 6x6 vertex colours) the tiles are lit with. The original source file is Draw\dFloor.cpp (the assert of
//  DFLOOR_DrawEdgeTile names it). Address range 0x6FAA2E30..0x6FAA4277.
//  The unit opens with the compiler's static initialiser of gFloorDrawCtx, which is not rewritten: the object is ours (dFloor.h) and is
//  zero-initialised. DFLOOR_APPROX_Distance, the last function, is placed here only by position.
//  A floor tile is five subtiles on a side, so its lighting is a 6x6 grid of D2ClientFloorVertexStrc -- intensity, red, green, blue, then the
//  screen position, twelve bytes each. The light comes from DLIGHTMAP_GetLightAtPoint at eight times the subtile coordinate or, with the flat
//  ambient, from the act's own intensity and colour. "Corners only" fills the four vertices at (0,0), (5,0), (0,5) and (5,5) and steps over
//  the rest.

//  The source path DFLOOR_DrawEdgeTile passes to its assert.
static const char szSourceFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\Draw\\dFloor.cpp";

//  The draw context: 72 bytes; its screen offsets are at +0x3C and +0x40.
static_assert(sizeof(D2FloorDrawCtxStrc) == 72
              && offsetof(D2FloorDrawCtxStrc, nScreenOffsetX) == 0x3C
              && offsetof(D2FloorDrawCtxStrc, nScreenOffsetY) == 0x40,
              "gFloorDrawCtx: 72 bytes, screen offsets at +0x3C/+0x40");
//  The vertex: four colour bytes, then x and y; twelve bytes.
static_assert(sizeof(D2ClientFloorVertexStrc) == 12
              && offsetof(D2ClientFloorVertexStrc, nRed) == 1
              && offsetof(D2ClientFloorVertexStrc, nGreen) == 2
              && offsetof(D2ClientFloorVertexStrc, nBlue) == 3
              && offsetof(D2ClientFloorVertexStrc, nScreenX) == 4
              && offsetof(D2ClientFloorVertexStrc, nScreenY) == 8,
              "D2ClientFloorVertexStrc: 12 bytes");

//1.10f: D2Client.0x6FAA2E60
//  Draws the room floors with the file's one context, gFloorDrawCtx, which DFLOOR_DrawRoomFloors takes in ecx (__thiscall).
void __fastcall DFLOOR_DrawFloors(D2ClientViewStrc *pView)
{
    DFLOOR_DrawRoomFloors(&gFloorDrawCtx, 0, pView);
}

//  Layout checks for DFLOOR_DrawFloorTile: the tile record, its library entry (flags, type) and the view's clip rect.
static_assert(offsetof(D2DrlgTileDataStrc, dwFlags) == 0x14
              && offsetof(D2DrlgTileDataStrc, pTile) == 0x18,
              "the tile record's flags and library entry");
static_assert(offsetof(D2TileLibraryEntryStrc, nFlags) == 6
              && offsetof(D2TileLibraryEntryStrc, nType) == 0x14,
              "the library entry's flags word and type");
static_assert(offsetof(D2ClientViewStrc, rcClip) == 0x14,
              "the view's clip rect, the ptRect assert's +0x14");

//1.10f: D2Client.0x6FAA2E70
//  Draws one floor tile. __thiscall over a `retn 18h`: pCtx in ecx and six on the stack, written __fastcall with an unread edx, as
//  DFLOOR_DrawEdgeTile is.
//  Early outs: a tile flagged 0x408 is skipped; so is one whose screen position is outside the view's clip rect (signed compares against
//  left, right, top, bottom), and one whose library entry has a non-zero nType.
//  BUG: the `ptRect` assert (line 118) tests an address -- `mov eax, [ebx+24h] / add eax, 14h / jnz`, pView plus the clip rect's offset -- which
//  is zero only for a pView of -20. LATENT: both passes dereference the view before they reach this. The C compares the integer, as the bytes do.
//  The light, by renderer: below DISPLAYTYPE_GLIDE the tile is lit by an 8x8 grid; at GLIDE and above by the 6x6 vertex grid, through D2Gfx's
//  perspective when the context says so. A library entry with nFlags 0x100 is drawn white (every vertex 0xFF, 0xFF, 0xFF, 0xFF) through the
//  same geometry the grid functions compute: the 8x8 is just the colours; the perspective 6x6 is DFLOOR_BuildTileLightGrid's walk with a
//  projection bias of 0, the flat 6x6 DFLOOR_FillEdgeLightGrid6x6's diamond. They are written out here as the bytes have them and not called:
//  the colour differs, and the perspective walk reads the corners flag once where the flat one reads it from the context at every vertex.
//  "Corners only" is the context's bLowQuality in every case.
//  D2Gfx #10079's ninth argument is bFloorFilter: the tile is drawn with bilinear texture filtering (D2Direct3D and D2Glide pick linear or
//  point filtering by it; D2DDraw never reads it). The calls pass DFLOOR_DrawRoomFloors' filter flag, 1 (DFLOOR_DrawEdgeTile) and 0
//  (DFLOOR_DrawViewGridFloors).
//  Splashes and bubbles: for an entry with nFlags 2 the context's seed at +0x1C is stepped (the original inlines SEED_RollRandomNumber here) and
//  the new low half's remainder by 1000 (`div ecx`, UNSIGNED) is compared SIGNED (`jge`) with the context's +0x10 (a splash, while bSplashWindow
//  is set) and with 100 (a bubble, while bBubbleWindow is set). DFLOOR_DrawRoomFloors fills those fields and pSplashSeed.
//  Then the tile is marked drawn (dwFlags | 0x20000) and, while the context collects edges, its world position widens the context's bounds
//  (signed compares).
void __fastcall DFLOOR_DrawFloorTile(D2FloorDrawCtxStrc *pCtx, int32_t nEdxUnused, D2DrlgTileDataStrc *pTile, BOOL bFloorFilter, int32_t nXPos, int32_t nYPos, int32_t nWorldX, int32_t nWorldY)
{
    D2TileLibraryEntryStrc *pTileEntry;
    D2ClientFloorVertexStrc *pVertex;
    int32_t nRand;
    int32_t bCornersOnly;
    int32_t nXShift;
    int32_t nOriginX;
    int32_t nRow;
    int32_t nCol;
    D2ClientFloorVertexStrc pVertices[36];
    D2ClientFloorVertexStrc pLight[64];

    if (pTile->dwFlags & 0x408)
        return;

    if ((UINT_PTR)pCtx->pView + offsetof(D2ClientViewStrc, rcClip) == 0)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "ptRect", (uint32_t)(UINT_PTR)szSourceFile, 118);
        exit(-1);
    }
    if (nXPos < pCtx->pView->rcClip.left || nXPos >= pCtx->pView->rcClip.right || nYPos < pCtx->pView->rcClip.top || nYPos >= pCtx->pView->rcClip.bottom)
        return;

    pTileEntry = pTile->pTile;
    if (pTileEntry->nType)
        return;

    if (DRAW_GetDisplayType() < DISPLAYTYPE_GLIDE)
    {
        if (pTileEntry->nFlags & 0x100)
        {
            pVertex = pLight;
            for (nRow = 0; nRow < 8; ++nRow)
            {
                for (nCol = 0; nCol < 8; ++nCol)
                {
                    pVertex->nIntensity = 0xFF;
                    pVertex->nRed = 0xFF;
                    pVertex->nBlue = 0xFF;
                    pVertex->nGreen = 0xFF;
                    ++pVertex;
                }
            }
        }
        else
        {
            DFLOOR_FillLightGrid8x8(pLight, nWorldX, nWorldY, 0);
        }
        D2GFX_FloorTileDraw(pTile->pTile, (D2GfxLightExStrc *)pLight, nXPos, nYPos, nWorldX, nWorldY, 0xFF, pCtx->nUiShiftScreen, bFloorFilter);
    }
    else
    {
        if (pCtx->bPerspective)
        {
            if (pTileEntry->nFlags & 0x100)
            {
                bCornersOnly = pCtx->bLowQuality;
                nXShift = 0;
                if (pCtx->nUiShiftScreen == 1)
                    nXShift = -(gnScreenWidth / 4);
                else if (pCtx->nUiShiftScreen == 2)
                    nXShift = gnScreenWidth / 4;

                pVertex = pVertices;
                for (nRow = 0; nRow < 6; ++nRow)
                {
                    for (nCol = 0; nCol < 6; ++nCol)
                    {
                        if (!bCornersOnly || ((nCol == 0 || nCol == 5) && (nRow == 0 || nRow == 5)))
                        {
                            D2GFX_SCALE_AdjustPerspectivePosition((int32_t)((uint32_t)(nWorldX + nCol) << 16), (int32_t)((uint32_t)(nRow + nWorldY) << 16), 0, &pVertex->nScreenX, &pVertex->nScreenY);
                            pVertex->nScreenX += nXShift;
                            pVertex->nIntensity = 0xFF;
                            pVertex->nRed = 0xFF;
                            pVertex->nBlue = 0xFF;
                            pVertex->nGreen = 0xFF;
                        }
                        ++pVertex;
                    }
                }
            }
            else
            {
                DFLOOR_BuildTileLightGrid(pVertices, nWorldX, nWorldY, 0, pCtx->nUiShiftScreen, pCtx->bLowQuality);
            }
        }
        else
        {
            if (pTileEntry->nFlags & 0x100)
            {
                nOriginX = nXPos;
                if (pCtx->nUiShiftScreen == 1)
                    nOriginX = nXPos - gnScreenWidth / 4;
                else if (pCtx->nUiShiftScreen == 2)
                    nOriginX = gnScreenWidth / 4 + nXPos;

                pVertex = pVertices;
                for (nRow = 0; nRow < 6; ++nRow)
                {
                    for (nCol = 0; nCol < 6; ++nCol)
                    {
                        if (!pCtx->bLowQuality || ((nCol == 0 || nCol == 5) && (nRow == 0 || nRow == 5)))
                        {
                            pVertex->nIntensity = 0xFF;
                            pVertex->nRed = 0xFF;
                            pVertex->nBlue = 0xFF;
                            pVertex->nScreenX = nOriginX + 16 * (nCol - nRow);
                            pVertex->nGreen = 0xFF;
                            pVertex->nScreenY = nYPos + 8 * (nCol + nRow);
                        }
                        ++pVertex;
                    }
                }
            }
            else
            {
                DFLOOR_FillEdgeLightGrid6x6(pVertices, nXPos, nYPos, nWorldX, nWorldY, 0, pCtx->nUiShiftScreen, pCtx->bLowQuality);
            }
        }
        D2GFX_FloorTileDraw(pTile->pTile, (D2GfxLightExStrc *)pVertices, nXPos, nYPos, nWorldX, nWorldY, 0xFF, pCtx->nUiShiftScreen, bFloorFilter);
    }

    if (pTileEntry->nFlags & 2)
    {
        nRand = (int32_t)((uint32_t)SEED_RollRandomNumber(pCtx->pSplashSeed) % 1000);
        if (pCtx->bSplashWindow && nRand < pCtx->nWeatherIntensity)
        {
            ENV_SpawnSplash(nXPos, nYPos, (int32_t)((uint32_t)nWorldX << 16), (int32_t)((uint32_t)nWorldY << 16));
        }
        if (pCtx->bBubbleWindow && nRand < 100)
        {
            ENV_SpawnBubble(nXPos, nYPos, (int32_t)((uint32_t)nWorldX << 16), (int32_t)((uint32_t)nWorldY << 16));
        }
    }

    pTile->dwFlags |= 0x20000;
    if (pCtx->bDrawEdges)
    {
        if (nWorldX < pCtx->nMinX)
            pCtx->nMinX = nWorldX;
        if (nWorldX > pCtx->nMaxX)
            pCtx->nMaxX = nWorldX;
        if (nWorldY < pCtx->nMinY)
            pCtx->nMinY = nWorldY;
        if (nWorldY > pCtx->nMaxY)
            pCtx->nMaxY = nWorldY;
    }
}

//1.10f: D2Client.0x6FAA3290
//  Draws one tile of a level's edge, with the act's own tile data. The subtile goes to screen through D2Common's GameSubtileToClientCoords with
//  y + 5 in and 40 added after, then the context's offsets; gpAct is asserted (line 223) and read once, the same value handed to
//  DUNGEON_GetTileDataFromAct; DFLOOR_DrawFloorTile draws it with bFloorFilter 1, bilinear-filtered under Glide and Direct3D.
//  __thiscall over `retn 8`: pCtx in ecx and both coordinates on the stack. MSVC does not accept __thiscall on a free function, so it is
//  written __fastcall with an edx parameter that is never read -- the same ABI.
void __fastcall DFLOOR_DrawEdgeTile(D2FloorDrawCtxStrc *pCtx, int32_t nEdxUnused, int32_t nWorldX, int32_t nWorldY)
{
    int32_t nX;
    int32_t nY;
    D2DrlgTileDataStrc *pTile;

    nX = nWorldX;
    nY = nWorldY + 5;
    D2COMMON_DUNGEON_GameSubtileToClientCoords(&nX, &nY);
    nY += 40;
    if (!gpAct)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "ghAct", (uint32_t)(UINT_PTR)szSourceFile, 223);
        exit(-1);
    }

    nX += pCtx->nScreenOffsetX;
    nY += pCtx->nScreenOffsetY;
    pTile = D2COMMON_DUNGEON_GetTileDataFromAct(gpAct);
    if (pTile)
        DFLOOR_DrawFloorTile(pCtx, 0, pTile, 1, nX, nY, nWorldX, nWorldY);
}

//  Layout checks for DFLOOR_DrawRoomFloors: the context's fields, the levels record's DrawEdges byte and floor filter, the 48-byte tile
//  records and the player's seed.
static_assert(offsetof(D2FloorDrawCtxStrc, bPerspective) == 4
              && offsetof(D2FloorDrawCtxStrc, bSplashWindow) == 8
              && offsetof(D2FloorDrawCtxStrc, bBubbleWindow) == 0xC
              && offsetof(D2FloorDrawCtxStrc, nWeatherIntensity) == 0x10
              && offsetof(D2FloorDrawCtxStrc, nLastSplashFrame) == 0x14
              && offsetof(D2FloorDrawCtxStrc, nLastBubbleFrame) == 0x18
              && offsetof(D2FloorDrawCtxStrc, pSplashSeed) == 0x1C
              && offsetof(D2FloorDrawCtxStrc, nUiShiftScreen) == 0x20
              && offsetof(D2FloorDrawCtxStrc, pView) == 0x24
              && offsetof(D2FloorDrawCtxStrc, bDrawEdges) == 0x28
              && offsetof(D2FloorDrawCtxStrc, nMinX) == 0x2C
              && offsetof(D2FloorDrawCtxStrc, nMaxX) == 0x30
              && offsetof(D2FloorDrawCtxStrc, nMinY) == 0x34
              && offsetof(D2FloorDrawCtxStrc, nMaxY) == 0x38,
              "gFloorDrawCtx's fields, as the room pass fills them");
static_assert(offsetof(D2LevelsTxt, nDrawEdges) == 9
              && offsetof(D2LevelsTxt, dwFloorFilter) == 0x214,
              "levels.txt DrawEdges and the floor filter");
static_assert(sizeof(D2DrlgTileDataStrc) == 0x30
              && offsetof(D2DrlgTileDataStrc, nPosX) == 8
              && offsetof(D2DrlgTileDataStrc, nPosY) == 0xC,
              "a tile record: 48 bytes, its tile position at +8/+0xC");
static_assert(offsetof(D2UnitStrc, pSeed) == 0x20,
              "the unit's seed, the splash roll's");

//1.10f: D2Client.0x6FAA3330
//  The room pass: fills the file's context, draws the floors of the player's room and its neighbours, then -- in a level that draws its edges --
//  the edge tiles beyond the bounds the floors reached. __thiscall over `retn 4`, written __fastcall with an unread edx.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition and DFLOOR_DrawEdgeTile here; the C calls DFLOOR_DrawEdgeTile for six of the
//  eight copies, and the top and bottom sides' second copy stays written out, its subtile y + 5 taken from the copy before it.
//  The context: D2Gfx's low-quality and perspective flags; the weather's particle intensity times gfConst1000 (1000.0f), truncated by `__ftol`;
//  the UI shift and the view. DrawEdges is levels.txt's byte for the player's level, and only with no panel open at 800x600 (`cmp eax, 2`).
//  DIFFERENCE: the original's x87 holds the particle product at extended precision where this build rounds it to double first, which differs
//  for the rare value whose extended product sits just under an integer that double rounds up to.
//  BUG: the levels record is read untested there and asserted (`ptLvlData`, line 304) only further down. LATENT: the level is the player's
//  room's. The bounds are seeded inverted, 0x7FFFFFFF and 0x80000000, and the screen offsets are the view origin's (80 - x, -80 - y).
//  The context also holds the splash and bubble clocks. nLastSplashFrame (+0x14) and nLastBubbleFrame (+0x18) hold the frame of the last
//  splash and bubble window, compared UNSIGNED (`jbe`) against the frame count plus 3 and plus 25; bSplashWindow (+0x08) and bBubbleWindow
//  (+0x0C) are the flags DFLOOR_DrawFloorTile reads -- a splash window opens every fourth frame while it rains, a bubble window every
//  twenty-sixth on a muddy level; and pSplashSeed (+0x1C) is set to &pPlayer->pSeed, so the splash roll steps the local player's own seed,
//  once per qualifying tile per frame. The assert on the player has the text "0" (line 286).
//  The floors: every adjacent room (signed count), layers 1 and 2, each floor tile whose `(dwFlags >> 14) & 7` is the layer, drawn at the
//  context offsets plus its draw point, world position the room's subtile origin plus five times its tile position. Layer 1 of a level whose
//  levels.txt FloorFilter is non-zero is drawn bilinear-filtered; layer 2 never is.
//  The edges: each of the four sides is tested against its own margin (signed): the min sides, left and top, against 33 with perspective and
//  30 without; the max sides, right and bottom, against 23 and 25. A side that fires snaps the bound one tile outward and down to a multiple
//  of five (a signed divide) and draws three edge tiles stepping away from the level: two through the body of DFLOOR_DrawEdgeTile, the third
//  by calling it.
//  BUG: the four differences wrap as the registers do: with no floor tile drawn, max - player wraps positive in the bytes (0x80000000 minus a
//  position) and the max sides do not fire, while the min sides do, around 2147483640. LATENT: the player's own floor always widens the bounds
//  first.
void __fastcall DFLOOR_DrawRoomFloors(D2FloorDrawCtxStrc *pCtx, int32_t nEdxUnused, D2ClientViewStrc *pView)
{
    uint32_t nFrame;
    int32_t nUiShiftScreen;
    int32_t bDrawEdges;
    D2ActiveRoomStrc *pRoom;
    D2UnitStrc *pPlayer;
    D2LevelsTxt *pLevelsTxt;
    D2ActiveRoomStrc **ppRoomList;
    int32_t nNumRooms;
    int32_t iRoom;
    D2DrlgCoordsStrc tCoords;
    uint32_t nLayer;
    int32_t bUnfiltered;
    BOOL bFloorFilter;
    D2DrlgTileDataStrc *pTile;
    int32_t nFloorCount;
    int32_t nMinSideMargin;
    int32_t nMaxSideMargin;
    int32_t nPlayerX;
    int32_t nPlayerY;
    int32_t nWorldX;
    int32_t nWorldY;
    int32_t nEdgeX;
    int32_t nEdgeY;
    int32_t nX;
    int32_t nY;
    D2DrlgTileDataStrc *pEdgeTile;

    pCtx->bLowQuality = D2GFX_CheckLowQuality();
    pCtx->bPerspective = D2GFX_CheckPerspective();
    nFrame = GAME_GetFrameCount();
    pCtx->bSplashWindow = 0;
    pCtx->bBubbleWindow = 0;
    pCtx->nWeatherIntensity = (int32_t)(int64_t)(ENV_GetWeatherParticleIntensity() * (double)gfConst1000);
    nUiShiftScreen = SCROLL_GetUiShiftScreen();
    pCtx->nUiShiftScreen = nUiShiftScreen;
    pCtx->pView = pView;

    bDrawEdges = FALSE;
    pRoom = CUNIT_GetPlayerRoom();
    if (pRoom && D2COMMON_DATATBLS_GetLevelsTxtRecord(D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom))->nDrawEdges && nUiShiftScreen == 0 && D2GFX_GetResolutionMode() == D2GAMERES_800x600)
    {
        bDrawEdges = TRUE;
    }
    pCtx->bDrawEdges = bDrawEdges;

    pCtx->nMaxX = (int32_t)0x80000000;
    pCtx->nMaxY = (int32_t)0x80000000;
    pCtx->nMinX = 0x7FFFFFFF;
    pCtx->nMinY = 0x7FFFFFFF;
    pCtx->nScreenOffsetX = 80 - pCtx->pView->nOriginX;
    pCtx->nScreenOffsetY = -80 - pCtx->pView->nOriginY;

    if (nFrame > (uint32_t)pCtx->nLastSplashFrame + 3)
    {
        pCtx->nLastSplashFrame = (int32_t)nFrame;
        if (pCtx->nWeatherIntensity)
            pCtx->bSplashWindow = 1;
    }
    if (ENV_IsPlayerLevelMuddy() && nFrame > (uint32_t)pCtx->nLastBubbleFrame + 25)
    {
        pCtx->nLastBubbleFrame = (int32_t)nFrame;
        pCtx->bBubbleWindow = 1;
    }

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "0", (uint32_t)(UINT_PTR)szSourceFile, 286);
        exit(-1);
    }
    pCtx->pSplashSeed = &pPlayer->pSeed;

    pRoom = CUNIT_GetPlayerRoom();
    if (!pRoom)
        return;

    D2COMMON_DUNGEON_GetAdjacentRoomsListFromRoom(pRoom, &ppRoomList, &nNumRooms);
    pLevelsTxt = D2COMMON_DATATBLS_GetLevelsTxtRecord(D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom));
    if (!pLevelsTxt)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "ptLvlData", (uint32_t)(UINT_PTR)szSourceFile, 304);
        exit(-1);
    }

    bUnfiltered = pLevelsTxt->dwFloorFilter == 0;
    for (iRoom = 0; iRoom < nNumRooms; ++iRoom)
    {
        D2COMMON_DUNGEON_GetRoomCoordinates(ppRoomList[iRoom], &tCoords);
        for (nLayer = 1; nLayer < 3; ++nLayer)
        {
            bFloorFilter = !bUnfiltered && nLayer == 1;
            pTile = D2COMMON_DUNGEON_GetFloorTilesFromRoom(ppRoomList[iRoom], &nFloorCount);
            for (; nFloorCount; --nFloorCount, ++pTile)
            {
                if (((pTile->dwFlags >> 14) & 7) == nLayer)
                {
                    DFLOOR_DrawFloorTile(pCtx, 0, pTile, bFloorFilter, pCtx->nScreenOffsetX + pTile->nDrawX, pCtx->nScreenOffsetY + pTile->nDrawY, tCoords.nSubtileX + 5 * pTile->nPosX, tCoords.nSubtileY + 5 * pTile->nPosY);
                }
            }
        }
    }

    if (!pCtx->bDrawEdges)
        return;

    nMinSideMargin = pCtx->bPerspective ? 33 : 30;
    nMaxSideMargin = 2 * (pCtx->bPerspective == 0) + 23;
    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;

    nPlayerX = UNITS_GetXPosition(pPlayer);

    nPlayerY = UNITS_GetYPosition(pPlayer);

    //  left: the min x side, three tiles stepping up in y
    if ((int32_t)((uint32_t)nPlayerX - (uint32_t)pCtx->nMinX) < nMinSideMargin)
    {
        nWorldX = 5 * ((pCtx->nMinX - 5) / 5);
        nWorldY = 5 * (nPlayerY / 5);

        DFLOOR_DrawEdgeTile(pCtx, 0, nWorldX, nWorldY);

        DFLOOR_DrawEdgeTile(pCtx, 0, nWorldX, nWorldY - 5);

        DFLOOR_DrawEdgeTile(pCtx, 0, nWorldX, nWorldY - 10);
    }

    //  right: the max x side, three tiles stepping down in y
    if ((int32_t)((uint32_t)pCtx->nMaxX - (uint32_t)nPlayerX) < nMaxSideMargin)
    {
        nWorldX = 5 * ((pCtx->nMaxX + 5) / 5);
        nWorldY = 5 * (nPlayerY / 5);

        DFLOOR_DrawEdgeTile(pCtx, 0, nWorldX, nWorldY);

        DFLOOR_DrawEdgeTile(pCtx, 0, nWorldX, nWorldY + 5);

        DFLOOR_DrawEdgeTile(pCtx, 0, nWorldX, nWorldY + 10);
    }

    //  top: the min y side, three tiles stepping left in x
    if ((int32_t)((uint32_t)nPlayerY - (uint32_t)pCtx->nMinY) < nMinSideMargin)
    {
        nWorldX = 5 * (nPlayerX / 5);
        nWorldY = 5 * ((pCtx->nMinY - 5) / 5);

        DFLOOR_DrawEdgeTile(pCtx, 0, nWorldX, nWorldY);

        nEdgeX = nWorldX - 5;
        nEdgeY = nWorldY;
        nX = nEdgeX;
        nY = nEdgeY + 5;
        D2COMMON_DUNGEON_GameSubtileToClientCoords(&nX, &nY);
        nY += 40;
        if (!gpAct)
        {
            FOG_DisplayAssert((uint32_t)(UINT_PTR) "ghAct", (uint32_t)(UINT_PTR)szSourceFile, 223);
            exit(-1);
        }
        nX += pCtx->nScreenOffsetX;
        nY += pCtx->nScreenOffsetY;
        pEdgeTile = D2COMMON_DUNGEON_GetTileDataFromAct(gpAct);
        if (pEdgeTile)
            DFLOOR_DrawFloorTile(pCtx, 0, pEdgeTile, 1, nX, nY, nEdgeX, nEdgeY);

        DFLOOR_DrawEdgeTile(pCtx, 0, nWorldX - 10, nWorldY);
    }

    //  bottom: the max y side, three tiles stepping right in x
    if ((int32_t)((uint32_t)pCtx->nMaxY - (uint32_t)nPlayerY) < nMaxSideMargin)
    {
        nWorldX = 5 * (nPlayerX / 5);
        nWorldY = 5 * ((pCtx->nMaxY + 5) / 5);

        DFLOOR_DrawEdgeTile(pCtx, 0, nWorldX, nWorldY);

        nEdgeX = nWorldX + 5;
        nEdgeY = nWorldY;
        nX = nEdgeX;
        nY = nEdgeY + 5;
        D2COMMON_DUNGEON_GameSubtileToClientCoords(&nX, &nY);
        nY += 40;
        if (!gpAct)
        {
            FOG_DisplayAssert((uint32_t)(UINT_PTR) "ghAct", (uint32_t)(UINT_PTR)szSourceFile, 223);
            exit(-1);
        }
        nX += pCtx->nScreenOffsetX;
        nY += pCtx->nScreenOffsetY;
        pEdgeTile = D2COMMON_DUNGEON_GetTileDataFromAct(gpAct);
        if (pEdgeTile)
            DFLOOR_DrawFloorTile(pCtx, 0, pEdgeTile, 1, nX, nY, nEdgeX, nEdgeY);

        DFLOOR_DrawEdgeTile(pCtx, 0, nWorldX + 10, nWorldY);
    }
}

//  Layout checks for DFLOOR_DrawViewGridFloors: the view's square grid and its nodes (36-byte squares, the floor list at +0x1C) and the
//  tile's fade fields at +0x24..+0x2C.
static_assert(offsetof(D2ClientViewStrc, pSquares) == 0xEAA8
              && offsetof(D2ClientViewStrc, nGridSide) == 0xEAB0
              && offsetof(D2ClientViewStrc, nOriginX) == 0x24
              && offsetof(D2ClientViewStrc, nOriginY) == 0x28,
              "the view's grid and origin");
static_assert(sizeof(D2ClientWallSquareStrc) == 36
              && offsetof(D2ClientWallSquareStrc, pFloorList) == 0x1C,
              "a grid square: 36 bytes, the floor list at +0x1C");
static_assert(offsetof(D2ClientDrawNodeStrc, nX) == 4
              && offsetof(D2ClientDrawNodeStrc, nY) == 8
              && offsetof(D2ClientDrawNodeStrc, pObject) == 0xC
              && offsetof(D2ClientDrawNodeStrc, pNext) == 0x10,
              "a draw node");
static_assert(offsetof(D2DrlgTileDataStrc, dwFadeState) == 0x24
              && offsetof(D2DrlgTileDataStrc, nAlpha) == 0x28
              && offsetof(D2DrlgTileDataStrc, nAlphaFrom) == 0x29
              && offsetof(D2DrlgTileDataStrc, nAlphaTo) == 0x2A
              && offsetof(D2DrlgTileDataStrc, dwFadeEndTick) == 0x2C,
              "a tile's fade");

//1.10f: D2Client.0x6FAA3B90
//  The other floor pass: every floor node of the view's square grid, in four layer passes, with each tile's fade advanced before it is drawn.
//  Layers: pass n (1..4, unsigned compare) draws the tiles whose flags hold ALL of `n << 14` -- so pass 3's mask is 0xC000, both of passes 1's
//  and 2's bits, and pass 4's is 0x10000.
//  Fade: a tile with dwFadeState bit 1 fades its alpha toward nAlphaTo. Below GLIDE (the display type is read once at entry) or at or past
//  dwFadeEndTick (UNSIGNED compare) it snaps and clears the bit; otherwise alpha = from + (now - end + 500) * (to - from) / 500, a 500 ms fade
//  ending at dwFadeEndTick. The product is taken in unsigned arithmetic, as the register wraps, and divided as an int (signed divide by 500).
//  Either way an alpha of zero sets dwFlags bit 10 (0x400) and a non-zero one clears it -- half of the 0x408 both passes skip. The fade-state
//  store is the whole dword with bit 1 cleared.
//  Draw: the position is the tile's draw point plus (80 - nOriginX, -80 - nOriginY), its y raised by the library entry's roof height; it is
//  clipped against the view's rect with the same `ptRect` address assert as DFLOOR_DrawFloorTile (see the BUG note there). The subtile is the
//  node's position `sar 3`. The roof height doubles as the flat-ambient flag: it is handed to all three light-grid functions where they take
//  bFlatAmbient or nBais, so a raised tile is lit flat. The renderer is asked again per tile (DRAW_GetDisplayType), beside the entry-time copy
//  the fade used. Perspective and low quality are D2Gfx's own flags, read once. D2Gfx #10079 gets the tile's alpha and 0 for bFloorFilter: a
//  view-grid tile is never filtered. The tile is then marked drawn, dwFlags | 0x20000.
void __fastcall DFLOOR_DrawViewGridFloors(D2ClientViewStrc *pView)
{
    uint32_t dwNow;
    int32_t nDisplayType;
    int32_t bPerspective;
    int32_t bCornersOnly;
    int32_t nOriginLeft;
    int32_t nOriginTop;
    int32_t nSquareCount;
    int32_t nSquaresLeft;
    int32_t nScreenPanels;
    uint32_t nLayer;
    uint32_t dwLayerMask;
    D2ClientWallSquareStrc *pSquare;
    D2ClientDrawNodeStrc *pNode;
    D2DrlgTileDataStrc *pTileData;
    D2TileLibraryEntryStrc *pTileEntry;
    uint32_t dwFlags;
    uint32_t dwFadeState;
    int32_t nXPos;
    int32_t nYPos;
    int32_t nSubtileX;
    int32_t nSubtileY;
    D2ClientFloorVertexStrc pLight[36];
    D2ClientFloorVertexStrc pGrid[64];

    dwNow = GetTickCount();
    nDisplayType = DRAW_GetDisplayType();
    bPerspective = D2GFX_CheckPerspective();
    bCornersOnly = D2GFX_CheckLowQuality();
    nOriginLeft = 80 - pView->nOriginX;
    nOriginTop = -80 - pView->nOriginY;
    nSquareCount = pView->nGridSide * pView->nGridSide;
    nScreenPanels = SCROLL_GetUiShiftScreen();

    for (nLayer = 1; nLayer <= 4; ++nLayer)
    {
        dwLayerMask = nLayer << 14;
        pSquare = pView->pSquares;
        for (nSquaresLeft = nSquareCount; nSquaresLeft; --nSquaresLeft)
        {
            for (pNode = pSquare->pFloorList; pNode; pNode = pNode->pNext)
            {
                pTileData = (D2DrlgTileDataStrc *)pNode->pObject;
                dwFlags = pTileData->dwFlags;
                if ((dwFlags & dwLayerMask) != dwLayerMask)
                    continue;

                dwFadeState = pTileData->dwFadeState;
                if (dwFadeState & 2)
                {
                    if (nDisplayType < DISPLAYTYPE_GLIDE || dwNow >= pTileData->dwFadeEndTick)
                    {
                        pTileData->nAlpha = pTileData->nAlphaTo;
                        pTileData->dwFadeState = dwFadeState & ~2u;
                    }
                    else
                    {
                        pTileData->nAlpha = (uint8_t)(pTileData->nAlphaFrom + (int32_t)((uint32_t)(dwNow - pTileData->dwFadeEndTick + 500) * (uint32_t)(pTileData->nAlphaTo - pTileData->nAlphaFrom)) / 500);
                    }

                    if (pTileData->nAlpha)
                        pTileData->dwFlags = dwFlags & ~0x400u;
                    else
                        pTileData->dwFlags = dwFlags | 0x400;
                }

                if (pTileData->dwFlags & 0x408)
                    continue;

                pTileEntry = pTileData->pTile;
                nXPos = pTileData->nDrawX + nOriginLeft;
                nYPos = pTileData->nDrawY - pTileEntry->nRoofHeight + nOriginTop;
                if ((UINT_PTR)pView + offsetof(D2ClientViewStrc, rcClip) == 0)
                {
                    FOG_DisplayAssert((uint32_t)(UINT_PTR) "ptRect", (uint32_t)(UINT_PTR)szSourceFile, 118);
                    exit(-1);
                }
                if (nXPos < pView->rcClip.left || nXPos >= pView->rcClip.right || nYPos < pView->rcClip.top || nYPos >= pView->rcClip.bottom)
                    continue;

                nSubtileX = pNode->nX >> 3;
                nSubtileY = pNode->nY >> 3;
                if (DRAW_GetDisplayType() >= DISPLAYTYPE_GLIDE)
                {
                    if (bPerspective)
                    {
                        DFLOOR_BuildTileLightGrid(pLight, nSubtileX, nSubtileY, pTileEntry->nRoofHeight, nScreenPanels, bCornersOnly);
                    }
                    else
                    {
                        DFLOOR_FillEdgeLightGrid6x6(pLight, nXPos, nYPos, nSubtileX, nSubtileY, pTileEntry->nRoofHeight, nScreenPanels, bCornersOnly);
                    }
                    D2GFX_FloorTileDraw(pTileData->pTile, (D2GfxLightExStrc *)pLight, nXPos, nYPos, nSubtileX, nSubtileY, pTileData->nAlpha, nScreenPanels, FALSE);
                }
                else
                {
                    DFLOOR_FillLightGrid8x8(pGrid, nSubtileX, nSubtileY, pTileEntry->nRoofHeight);
                    D2GFX_FloorTileDraw(pTileData->pTile, (D2GfxLightExStrc *)pGrid, nXPos, nYPos, nSubtileX, nSubtileY, pTileData->nAlpha, nScreenPanels, FALSE);
                }
                pTileData->dwFlags |= 0x20000;
            }
            ++pSquare;
        }
    }
}


//1.10f: D2Client.0x6FAA3E70
//  Fills an 8x8 block of vertex colours, sixty-four in a row: with the flat ambient every one gets the act's intensity and colour; otherwise
//  each is DLIGHTMAP_GetLightAtPoint's, from (8 * nSubtileX - 8, 8 * nSubtileY - 8) in steps of eight. Only the colour bytes are written; the
//  twelve-byte stride is D2ClientFloorVertexStrc's.
//  The three light-grid fillers (this one, DFLOOR_FillEdgeLightGrid6x6, DFLOOR_BuildTileLightGrid) are void: no caller reads their eax.
//  The panel shift: side 1 moves the grid left by a quarter of the screen, side 2 right, as the half-screen panels do -- a signed quarter,
//  which is what `/ 4` on an int computes.
void __fastcall DFLOOR_FillLightGrid8x8(D2ClientFloorVertexStrc *pVertices, int32_t nSubtileX, int32_t nSubtileY, int32_t bFlatAmbient)
{
    uint8_t nIntensity;
    uint8_t nRed;
    uint8_t nGreen;
    uint8_t nBlue;
    int32_t nRow;
    int32_t nCol;
    int32_t nX;
    int32_t nY;

    if (bFlatAmbient)
    {
        nIntensity = (uint8_t)D2COMMON_ENVIRONMENT_GetIntensityFromAct(gpAct);
        D2COMMON_ENVIRONMENT_GetLightColorFromAct(gpAct, &nRed, &nGreen, &nBlue);
        //  Two count-down loops on one cursor.
        for (nRow = 0; nRow < 8; ++nRow)
        {
            for (nCol = 0; nCol < 8; ++nCol)
            {
                pVertices->nIntensity = nIntensity;
                pVertices->nRed = nRed;
                pVertices->nBlue = nBlue;
                pVertices->nGreen = nGreen;
                ++pVertices;
            }
        }
        return;
    }

    //  The row's first x is reloaded at every row; the cursor runs on through all sixty-four.
    nY = 8 * nSubtileY - 8;
    for (nRow = 0; nRow < 8; ++nRow)
    {
        nX = 8 * nSubtileX - 8;
        for (nCol = 0; nCol < 8; ++nCol)
        {
            DLIGHTMAP_GetLightAtPoint(nX, nY, &pVertices->nIntensity, &pVertices->nRed, &pVertices->nGreen, &pVertices->nBlue);
            ++pVertices;
            nX += 8;
        }
        nY += 8;
    }
}

//1.10f: D2Client.0x6FAA3F40
//  Fills a 6x6 grid laid out as an isometric diamond from the screen position (nScreenX, nScreenY): column c, row r at x + 16 * (c - r),
//  y + 8 * (c + r), the panel shift added to x first; colours flat or DLIGHTMAP_GetLightAtPoint's at (8 * (c + nSubtileX), 8 * (nSubtileY + r)).
//  The last parameter is the corners-only flag, the same four tests as in DFLOOR_BuildTileLightGrid.
void __fastcall DFLOOR_FillEdgeLightGrid6x6(D2ClientFloorVertexStrc *pVertices, int32_t nScreenX, int32_t nScreenY, int32_t nSubtileX, int32_t nSubtileY, int32_t bFlatAmbient, int32_t nSideShift, int32_t bCornersOnly)
{
    uint8_t nIntensity;
    uint8_t nRed;
    uint8_t nGreen;
    uint8_t nBlue;
    int32_t nRow;
    int32_t nCol;
    int32_t nY;

    nIntensity = (uint8_t)D2COMMON_ENVIRONMENT_GetIntensityFromAct(gpAct);
    if (nSideShift == 1)
        nScreenX += -(gnScreenWidth / 4);
    else if (nSideShift == 2)
        nScreenX += gnScreenWidth / 4;
    D2COMMON_ENVIRONMENT_GetLightColorFromAct(gpAct, &nRed, &nGreen, &nBlue);

    nY = 8 * nSubtileY;
    for (nRow = 0; nRow < 6; ++nRow)
    {
        for (nCol = 0; nCol < 6; ++nCol)
        {
            if (!bCornersOnly || ((nCol == 0 || nCol == 5) && (nRow == 0 || nRow == 5)))
            {
                pVertices->nScreenX = nScreenX + 16 * (nCol - nRow);
                pVertices->nScreenY = nScreenY + 8 * (nCol + nRow);
                if (bFlatAmbient)
                {
                    pVertices->nIntensity = nIntensity;
                    pVertices->nRed = nRed;
                    pVertices->nBlue = nBlue;
                    pVertices->nGreen = nGreen;
                }
                else
                {
                    DLIGHTMAP_GetLightAtPoint(8 * (nCol + nSubtileX), nY, &pVertices->nIntensity, &pVertices->nRed, &pVertices->nGreen, &pVertices->nBlue);
                }
            }
            ++pVertices;
        }
        nY += 8;
    }
}

//1.10f: D2Client.0x6FAA4060
//  Fills a tile's 6x6 grid through D2Gfx's perspective: each vertex's position is SCALE_AdjustPerspectivePosition's for ((nTileX + c) << 16,
//  (nTileY + r) << 16) plus the panel shift. nBais decides the colour as well as the projection: non-zero, it is handed to the projection and
//  every vertex gets the flat ambient; zero, the projection gets 0 and each vertex is lit by DLIGHTMAP_GetLightAtPoint at
//  (8 * (nTileX + c), 8 * (nTileY + r)) -- nTileX and nTileY are in subtiles. The shift left by 16 of the sum is taken unsigned in the C, so a
//  coordinate past 32767 wraps as the register does instead of being undefined.
void __fastcall DFLOOR_BuildTileLightGrid(D2ClientFloorVertexStrc *pVertices, int32_t nTileX, int32_t nTileY, int32_t nBais, int32_t nHalfShift, int32_t bCornersOnly)
{
    uint8_t nIntensity;
    uint8_t nRed;
    uint8_t nGreen;
    uint8_t nBlue;
    int32_t nScreenXShift;
    int32_t nRow;
    int32_t nCol;

    nIntensity = (uint8_t)D2COMMON_ENVIRONMENT_GetIntensityFromAct(gpAct);
    nScreenXShift = 0;
    if (nHalfShift == 1)
        nScreenXShift = -(gnScreenWidth / 4);
    else if (nHalfShift == 2)
        nScreenXShift = gnScreenWidth / 4;
    D2COMMON_ENVIRONMENT_GetLightColorFromAct(gpAct, &nRed, &nGreen, &nBlue);

    if (nBais)
    {
        for (nRow = 0; nRow < 6; ++nRow)
        {
            for (nCol = 0; nCol < 6; ++nCol)
            {
                if (!bCornersOnly || ((nCol == 0 || nCol == 5) && (nRow == 0 || nRow == 5)))
                {
                    D2GFX_SCALE_AdjustPerspectivePosition((int32_t)((uint32_t)(nTileX + nCol) << 16), (int32_t)((uint32_t)(nRow + nTileY) << 16), nBais, &pVertices->nScreenX, &pVertices->nScreenY);
                    pVertices->nScreenX += nScreenXShift;
                    pVertices->nIntensity = nIntensity;
                    pVertices->nRed = nRed;
                    pVertices->nBlue = nBlue;
                    pVertices->nGreen = nGreen;
                }
                ++pVertices;
            }
        }
        return;
    }

    for (nRow = 0; nRow < 6; ++nRow)
    {
        for (nCol = 0; nCol < 6; ++nCol)
        {
            if (!bCornersOnly || ((nCol == 0 || nCol == 5) && (nRow == 0 || nRow == 5)))
            {
                D2GFX_SCALE_AdjustPerspectivePosition((int32_t)((uint32_t)(nTileX + nCol) << 16), (int32_t)((uint32_t)(nRow + nTileY) << 16), 0, &pVertices->nScreenX, &pVertices->nScreenY);
                pVertices->nScreenX += nScreenXShift;
                DLIGHTMAP_GetLightAtPoint(8 * (nTileX + nCol), 8 * (nRow + nTileY), &pVertices->nIntensity, &pVertices->nRed, &pVertices->nGreen, &pVertices->nBlue);
            }
            ++pVertices;
        }
    }
}

//1.10f: D2Client.0x6FAA4210
//  Alpha-max-plus-beta-min: (983 * max + 407 * min) >> 10 over the two absolute differences (the lea chains multiply by exactly 983 and 407).
//  The absolute values are `cdq / xor / sub`, compared SIGNED (`jl`); the sum is shifted UNSIGNED (`shr eax, 0Ah`). Written in unsigned
//  arithmetic, so INT_MIN, whose absolute value is itself, and a sum past 2^31 behave as the registers do rather than being undefined; the
//  compare casts back to int as `jl` reads it.
//  Placed in this file only by position: it follows the last light-grid function and precedes dLightMap.cpp's first, and nothing here calls it.
uint32_t __fastcall DFLOOR_APPROX_Distance(int32_t nDX, int32_t nDY)
{
    uint32_t nSignX;
    uint32_t nSignY;
    uint32_t nAbsDX;
    uint32_t nAbsDY;

    nSignX = (uint32_t)(nDX >> 31);
    nSignY = (uint32_t)(nDY >> 31);
    nAbsDX = ((uint32_t)nDX ^ nSignX) - nSignX;
    nAbsDY = ((uint32_t)nDY ^ nSignY) - nSignY;
    if ((int32_t)nAbsDX < (int32_t)nAbsDY)
        return (407 * nAbsDX + 983 * nAbsDY) >> 10;
    return (983 * nAbsDX + 407 * nAbsDY) >> 10;
}

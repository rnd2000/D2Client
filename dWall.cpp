#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "dWall.h"

//  dWall.cpp -- the draw-list readers of D2Client.dll 1.10: the four passes that walk the per-square draw lists Wall2.cpp fills
//  (WALL2_WallBuildSquareGrid sizes an `nGridSide * nGridSide` grid of 36-byte D2ClientWallSquareStrc, WALL2_WallAddRoomToSquareGrid links a node
//  into one of each square's five lists), one layer per pass.
//  The original source file is Draw\dWall.cpp (the asserts of DWALL_DrawRoomWalls at lines 243 and 249 name it). Address range
//  0x6FAA6340..0x6FAA6981. Only DWALL_DrawRoomWalls is named by the asserts; the other three are placed here by address contiguity.

//  The source file, as both asserts hand it to FOG_DisplayAssert (0x6FB75178).
static const char szSourceFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\Draw\\dWall.cpp";

//  Packs one corner's light into the dword the tile renderer takes: intensity, red, green and blue from the low byte up. The original builds it
//  through one 16-bit temporary holding blue and green; written flat here.
static uint32_t DWALL_PackLight(uint8_t nIntensity, uint8_t nRed, uint8_t nGreen, uint8_t nBlue)
{
    uint16_t nBlueGreen;

    nBlueGreen = (uint16_t)((nBlue << 8) | nGreen);

    return nIntensity | ((nRed | (nBlueGreen << 8)) << 8);
}

//  Samples the light at each of a tile's corners. The corner set comes out of one of two tables, gaTileLightOffsets and gaTileLightOffsetsAlt,
//  int[10][6][2]: ten light directions, six corners, an (x, y) pair, 48 bytes per direction. The original repeats this at two sites,
//  DWALL_DrawRoomWalls and DWALL_EnvDrawRoomTilesLit. Direction 0 has a count of 0 and all-zero offsets; direction 5 has a count of 6 and
//  all-zero offsets; directions 6..9 repeat 1..4.
//  The zero guard is branchless: `nIntensity &= (nIntensity == 0) - 1` keeps the byte unless it is zero, which stays zero; reproduced as written.
//  BUG: nothing in D2Client bounds the direction, the tile record's first field (D2CMP's D2TileLibraryEntryStrc nLightDirection, +0x00): it is
//  read and used as the index at once (0x6FAA650F in DWALL_DrawRoomWalls, 0x6FAA670D in DWALL_EnvDrawRoomTilesLit). A direction of 10 or more
//  reads past the tables -- into the next item in the original, past our arrays here (undefined in C++). The stock tiles hold 1..9 only;
//  data outside that set is where the two programs could differ.
static void DWALL_GatherTileCornerLight(const D2ClientDrawNodeStrc *pNode, const D2DrlgTileDataStrc *pTileData, uint32_t *pLight)
{
    int32_t nLightDirection;
    uint32_t nCornerCount;
    const int32_t (*pLightOffsets)[2];
    uint32_t nCornerIdx;
    uint8_t nIntensity;
    uint8_t nRed;
    uint8_t nGreen;
    uint8_t nBlue;

    nLightDirection = pTileData->pTile->nLightDirection;
    nCornerCount = (uint32_t)gaTileLightCornerCount[nLightDirection];

    //  Bit 0 of the fade state picks the mirrored corner set (gaTileLightOffsetsAlt).
    if ((pTileData->nFadeFlags & 1) != 0)
        pLightOffsets = gaTileLightOffsetsAlt[nLightDirection];
    else
        pLightOffsets = gaTileLightOffsets[nLightDirection];

    for (nCornerIdx = 0; nCornerIdx < nCornerCount; ++nCornerIdx)
    {
        DLIGHTMAP_GetLightAtPoint(pNode->nX + 8 * pLightOffsets[nCornerIdx][0], pNode->nY + 8 * pLightOffsets[nCornerIdx][1], &nIntensity, &nRed, &nGreen, &nBlue);

        nIntensity &= (uint8_t)((nIntensity == 0) - 1);

        pLight[nCornerIdx] = DWALL_PackLight(nIntensity, nRed, nGreen, nBlue);
    }
}

//  Draws one unit node under the light at its own position. The node's stored coordinates are used directly -- no perspective branch and no view
//  offset, unlike the tile paths below. The original repeats this at two sites, DWALL_EnvDrawCellUnits and the default arm of
//  DWALL_EnvDrawCellShadowsAndUnits.
static void DWALL_DrawUnitNodeLit(const D2ClientDrawNodeStrc *pNode)
{
    uint8_t nIntensity;
    uint8_t nRed;
    uint8_t nGreen;
    uint8_t nBlue;

    DLIGHTMAP_GetLightAtPoint(pNode->nX, pNode->nY, &nIntensity, &nRed, &nGreen, &nBlue);

    UNITGFX_GfxDrawUnit((D2UnitStrc *)pNode->pObject, DWALL_PackLight(nIntensity, nRed, nGreen, nBlue), 0);
}

//  The layout the four passes walk: every value is a displacement or a stride read off the original's instructions.
static_assert(sizeof(D2ClientWallSquareStrc) == 36,
              "the grid stride: DWALL_EnvDrawRoomTilesLit walks it as +9 dwords");
static_assert(offsetof(D2ClientWallSquareStrc, pShadowList) == 0x10,
              "DWALL_EnvDrawCellShadowsAndUnits takes [ebp+10h]");
static_assert(offsetof(D2ClientWallSquareStrc, pWallList) == 0x14,
              "DWALL_DrawRoomWalls takes [ecx+14h]; both callers test it first");
static_assert(offsetof(D2ClientWallSquareStrc, pUnitList) == 0x18,
              "DWALL_EnvDrawCellUnits takes [edi+18h]");
static_assert(offsetof(D2ClientWallSquareStrc, pTileList) == 0x20,
              "DWALL_EnvDrawRoomTilesLit starts at pSquares+20h");

static_assert(sizeof(D2ClientDrawNodeStrc) == 20,
              "20-byte nodes: the pool is pDrawNodes[3000] at view+0x3C");
static_assert(offsetof(D2ClientDrawNodeStrc, dwFlags) == 0x00,
              "the shadow pass dispatches on [esi]");
static_assert(offsetof(D2ClientDrawNodeStrc, nX) == 0x04,
              "DLIGHTMAP_GetLightAtPoint takes [esi+4]");
static_assert(offsetof(D2ClientDrawNodeStrc, nY) == 0x08,
              "DLIGHTMAP_GetLightAtPoint takes [esi+8]");
static_assert(offsetof(D2ClientDrawNodeStrc, pObject) == 0x0C,
              "the tile record or the unit, depending on dwFlags");
static_assert(offsetof(D2ClientDrawNodeStrc, pNext) == 0x10,
              "every list in this file walks [esi+10h]");

//  The tile record, and the five fields the fade reads (+0x28..+0x2A are the fade's current, start and target alpha).
static_assert(offsetof(D2DrlgTileDataStrc, nDrawX) == 0x00,
              "the flat renderer's X, offset by the view");
static_assert(offsetof(D2DrlgTileDataStrc, nDrawY) == 0x04,
              "the flat renderer's Y");
static_assert(offsetof(D2DrlgTileDataStrc, dwFlags) == 0x14,
              "the 0x408 / 0x1C000 filter and the 0x20000 drawn mark");
static_assert(offsetof(D2DrlgTileDataStrc, pTile) == 0x18,
              "handed straight to the three TILE_ entry points");
static_assert(offsetof(D2DrlgTileDataStrc, nTileType) == 0x1C,
              "the assert at dWall.cpp:249 compares it against 15");
static_assert(offsetof(D2DrlgTileDataStrc, nFadeFlags) == 0x24,
              "bit 0 mirrored corners, bit 1 fading, bit 2 timed expiry");
static_assert(offsetof(D2DrlgTileDataStrc, nAlpha) == 0x28,
              "compared against 0xFF to pick Lit over Trans");
static_assert(offsetof(D2DrlgTileDataStrc, nFadeStartAlpha) == 0x29,
              "the ramp's start");
static_assert(offsetof(D2DrlgTileDataStrc, nFadeEndAlpha) == 0x2A,
              "the ramp's target");
static_assert(offsetof(D2DrlgTileDataStrc, nFadeEndTick) == 0x2C,
              "the deadline both bit 1 and bit 2 test against");

//  >= DISPLAYTYPE_GLIDE (4) is the 3D-accelerated set.
static_assert(DISPLAYTYPE_GLIDE == 4,
              "cmp esi, 4 at 0x6FAA6412 is >= DISPLAYTYPE_GLIDE");
static_assert(DISPLAYTYPE_DDRAW == 3,
              "and DDRAW is the last mode that snaps instead of fading");

//1.10f: D2Client.0x6FAA6340
//  The simpler of the two per-cell unit passes: walks the whole draw grid and, per square, draws the wall list first and then every node on the
//  unit list (every node on pUnitList is a unit, so there is no branch). It is the only one of the three passes
//  VIEWEFFECTS_EnvUpdateViewAndLevelEffects runs unconditionally; the other two are gated on bits of pView->dwLayerMask.
//  The original multiplies nGridSide * nGridSide twice (a test, then the square count) and does not read pView->nSquareCount (+0xEAAC).
void __fastcall DWALL_EnvDrawCellUnits(D2ClientViewStrc *pView)
{
    D2ClientWallSquareStrc *pSquare;
    int32_t nSquareCount;
    D2ClientDrawNodeStrc *pNode;

    pSquare = pView->pSquares;

    if (pView->nGridSide * pView->nGridSide)
    {
        nSquareCount = pView->nGridSide * pView->nGridSide;

        do
        {
            if (pSquare->pWallList)
                DWALL_DrawRoomWalls(pSquare, 0, pView);

            for (pNode = pSquare->pUnitList; pNode; pNode = pNode->pNext)
                DWALL_DrawUnitNodeLit(pNode);

            ++pSquare;
            --nSquareCount;
        }
        while (nSquareCount);
    }
}

//1.10f: D2Client.0x6FAA63E0
//  Draws one square's wall tiles and runs the wall-transparency fade while doing it. The parameter names are the original's: the asserts at
//  Draw\dWall.cpp:243 (`fOnlyDrawShadowLayer == 0 || fOnlyDrawShadowLayer == 1`) and :249 (`ptTile->eTileType!= BASE_ROOF`).
//  Called twice, both from this file: DWALL_EnvDrawCellUnits passes 0 and calls it before its list, DWALL_EnvDrawCellShadowsAndUnits passes 1
//  and calls it after.
//  fOnlyDrawShadowLayer is copied into a local on entry and the per-tile filter compares the copy (an `unsigned int` comparison).
//  The fade ramp is 500 ms: the original divides by a magic multiply (6FAA6492..6FAA64A1: imul by 274877907, high dword, sar 5), and
//  2^37 / 274877907 = 500.
//  The `+ 500` is not rounding: this arm runs only while dwTickCount < pTileData->nFadeEndTick, so dwTickCount - nFadeEndTick is the negative
//  time still to run; adding the ramp length gives the time already elapsed, and elapsed / 500 scales the alpha interval. The `+ (n < 0)` beside
//  it is the compiler's round-toward-zero correction for a signed divide.
//  The gate `DRAW_GetDisplayType() >= 4` is `>= DISPLAYTYPE_GLIDE`: the smooth ramp runs only under 3D acceleration, every other mode snaps
//  straight to the end value. It is not a call of DRAW_IsDisplayType3D: the original calls DRAW_GetDisplayType at 6FAA6401 and compares (signed
//  `setnl`, 6FAA6419), so the predicate is spelled out.
//  nFadeFlags (+0x24) carries the fade's own bits -- 1 mirrored corner set, 2 fading, 4 a timed expiry; dwFlags (+0x14) the tile's, of which
//  this function touches 0x400 (alpha is zero), 8 (the expiry fired), 0x408 and 0x1C000 (the filter) and 0x20000 (drawn this frame).
//  The original writes the flags byte-wide (`and al, 0FDh`, `or al, 8`); ours writes the dword -- every bit touched lies in that byte, so the
//  value is the same. `&= ~0x400u` is the original's bit 2 of byte 1.
void __fastcall DWALL_DrawRoomWalls(D2ClientWallSquareStrc *pRoom, uint32_t fOnlyDrawShadowLayer, D2ClientViewStrc *pView)
{
    int32_t nScreenPanels;
    int32_t bPerspective;
    int32_t eDisplayType;
    uint32_t dwTickCount;
    BOOL bIs3DAccelerated;
    uint32_t nOnlyShadowCopy;
    D2ClientDrawNodeStrc *pNode;
    D2DrlgTileDataStrc *pTileData;
    uint32_t nFadeFlags;
    uint32_t nFadeEndTick;
    int32_t nScaled;
    uint8_t nNewAlpha;
    uint32_t dwTileFlags;
    uint8_t nAlphaNow;
    int32_t nXPos;
    int32_t nYPos;
    int32_t nDrawResult;
    uint32_t pLight[8];

    nOnlyShadowCopy = fOnlyDrawShadowLayer;
    nScreenPanels = SCROLL_GetUiShiftScreen();
    bPerspective = D2GFX_CheckPerspective();
    eDisplayType = DRAW_GetDisplayType();
    dwTickCount = GetTickCount();
    bIs3DAccelerated = eDisplayType >= DISPLAYTYPE_GLIDE;

    if (fOnlyDrawShadowLayer >= 2)
    {
        //  The expression is pushed from gszAssertOnlyDrawShadowLayer (0x6FB75140).
        FOG_DisplayAssert(gszAssertOnlyDrawShadowLayer, szSourceFile, 243);
        exit(-1);
    }

    for (pNode = pRoom->pWallList; pNode; pNode = pNode->pNext)
    {
        pTileData = (D2DrlgTileDataStrc *)pNode->pObject;

        if (pTileData->nTileType == 15)
        {
            FOG_DisplayAssert("ptTile->eTileType!= BASE_ROOF", szSourceFile, 249);
            exit(-1);
        }

        //  Bit 1: a fade is running. Interpolate under acceleration, snap otherwise; snapping also clears the bit, so the fade ends.
        nFadeFlags = pTileData->nFadeFlags;
        if ((nFadeFlags & 2) != 0)
        {
            nFadeEndTick = pTileData->nFadeEndTick;

            if (bIs3DAccelerated && dwTickCount < nFadeEndTick)
            {
                nScaled = (int32_t)(((int64_t)274877907 * (int32_t)((dwTickCount - nFadeEndTick + 500) * (pTileData->nFadeEndAlpha - pTileData->nFadeStartAlpha))) >> 32) >> 5;

                nNewAlpha = (uint8_t)(pTileData->nFadeStartAlpha + (nScaled < 0) + nScaled);
            }
            else
            {
                nNewAlpha = pTileData->nFadeEndAlpha;
                nFadeFlags &= ~2u;
                pTileData->nFadeFlags = nFadeFlags;
            }

            pTileData->nAlpha = nNewAlpha;

            //  0x400 in the tile flags mirrors "the tile is now invisible".
            dwTileFlags = pTileData->dwFlags;
            if (nNewAlpha)
                dwTileFlags &= ~0x400u;
            else
                dwTileFlags |= 0x400u;
            pTileData->dwFlags = dwTileFlags;
        }

        //  Bit 2: a timed expiry, independent of the ramp above and tested against the same deadline field.
        nFadeFlags = pTileData->nFadeFlags;
        if ((nFadeFlags & 4) != 0 && dwTickCount >= pTileData->nFadeEndTick)
        {
            nFadeFlags &= ~4u;
            pTileData->nFadeFlags = nFadeFlags;

            dwTileFlags = pTileData->dwFlags;
            dwTileFlags |= 8u;
            pTileData->dwFlags = dwTileFlags;
        }

        //  The filter: 0x408 must be clear, and the caller's layer selector must match whether any of 0x1C000 is set --
        //  so the shadow pass (1) takes the tiles with those bits and the unit pass (0) the rest.
        dwTileFlags = pTileData->dwFlags;
        if ((dwTileFlags & 0x408) == 0 && nOnlyShadowCopy != ((dwTileFlags & 0x1C000) != 0))
        {
            DWALL_GatherTileCornerLight(pNode, pTileData, pLight);

            nAlphaNow = pTileData->nAlpha;

            //  Perspective takes the node's coordinates; the flat renderer takes the tile record's and offsets them by the view.
            if (bPerspective)
            {
                nXPos = pNode->nX;
                nYPos = pNode->nY;
            }
            else
            {
                nXPos = pTileData->nDrawX + pView->rcView.left - pView->nOriginX;
                nYPos = pView->rcView.top + pTileData->nDrawY - pView->nOriginY;
            }

            if (nAlphaNow == 0xFF)
                nDrawResult = D2GFX_TILE_TileDrawLit(pTileData->pTile, nXPos, nYPos, (D2GfxLightStrc *)pLight, nScreenPanels);
            else
                nDrawResult = D2GFX_TILE_TileDrawTrans(pTileData->pTile, nXPos, nYPos, (D2GfxLightStrc *)pLight, nScreenPanels, nAlphaNow);

            if (nDrawResult)
                pTileData->dwFlags |= 0x20000u;
        }
    }
}

//1.10f: D2Client.0x6FAA6670
//  Draws the tile layer of every square, per-corner lit: the pass over pTileList (+0x20), the fifth of the square's five list heads.
//  SCROLL_GetUiShiftScreen, D2GFX_CheckPerspective and GetTickCount are called inside the grid loop, once per square that has a tile list, where
//  DWALL_DrawRoomWalls calls each once for the whole function.
//  There is no alpha ramp: only the timed expiry (bit 2) is run; the fade interpolation (bit 1) of DWALL_DrawRoomWalls has no counterpart in this
//  pass, so a tile on the tile list is drawn at whatever nAlpha the wall pass last left on the record.
//  The original walks the grid as a dword pointer (`&pSquares->pTileList`, 9 dwords per square); written here as `++pSquare` on the typed pointer.
void __fastcall DWALL_EnvDrawRoomTilesLit(D2ClientViewStrc *pView)
{
    int32_t nSquareCount;
    D2ClientWallSquareStrc *pSquare;
    int32_t nSquareIdx;
    int32_t nScreenPanels;
    int32_t bPerspective;
    uint32_t dwTickCount;
    D2ClientDrawNodeStrc *pNode;
    D2DrlgTileDataStrc *pTileData;
    uint32_t nFadeFlags;
    uint32_t dwTileFlags;
    uint8_t nAlphaNow;
    int32_t nXPos;
    int32_t nYPos;
    int32_t nDrawResult;
    uint32_t pLight[8];

    nSquareCount = pView->nGridSide * pView->nGridSide;
    pSquare = pView->pSquares;

    if (nSquareCount)
    {
        nSquareIdx = nSquareCount;

        do
        {
            if (pSquare->pTileList)
            {
                nScreenPanels = SCROLL_GetUiShiftScreen();
                bPerspective = D2GFX_CheckPerspective();
                dwTickCount = GetTickCount();

                for (pNode = pSquare->pTileList; pNode; pNode = pNode->pNext)
                {
                    pTileData = (D2DrlgTileDataStrc *)pNode->pObject;

                    nFadeFlags = pTileData->nFadeFlags;
                    if ((nFadeFlags & 4) != 0 && dwTickCount >= pTileData->nFadeEndTick)
                    {
                        nFadeFlags &= ~4u;
                        pTileData->nFadeFlags = nFadeFlags;

                        dwTileFlags = pTileData->dwFlags;
                        dwTileFlags |= 8u;
                        pTileData->dwFlags = dwTileFlags;
                    }

                    //  The same 0x408 gate as the wall pass, but the second half is an unconditional "has one of 0x1C000" rather than a
                    //  comparison against a caller's selector -- this pass takes no layer argument.
                    dwTileFlags = pTileData->dwFlags;
                    if ((dwTileFlags & 0x408) == 0 && (dwTileFlags & 0x1C000) != 0)
                    {
                        DWALL_GatherTileCornerLight(pNode, pTileData, pLight);

                        nAlphaNow = pTileData->nAlpha;

                        if (bPerspective)
                        {
                            nXPos = pNode->nX;
                            nYPos = pNode->nY;
                        }
                        else
                        {
                            nXPos = pTileData->nDrawX + pView->rcView.left - pView->nOriginX;
                            nYPos = pView->rcView.top + pTileData->nDrawY - pView->nOriginY;
                        }

                        if (nAlphaNow == 0xFF)
                            nDrawResult = D2GFX_TILE_TileDrawLit(pTileData->pTile, nXPos, nYPos, (D2GfxLightStrc *)pLight, nScreenPanels);
                        else
                            nDrawResult = D2GFX_TILE_TileDrawTrans(pTileData->pTile, nXPos, nYPos, (D2GfxLightStrc *)pLight, nScreenPanels, nAlphaNow);

                        if (nDrawResult)
                            pTileData->dwFlags |= 0x20000u;
                    }
                }
            }

            ++pSquare;
            --nSquareIdx;
        }
        while (nSquareIdx);
    }
}

//1.10f: D2Client.0x6FAA6860
//  The richer of the two per-cell passes: walks pShadowList (+0x10) and dispatches on the node's own flag bits, then draws the wall list -- after
//  the list, where DWALL_EnvDrawCellUnits draws it before; that ordering is the layering.
//      dwFlags & 1  -> TILE_ShadowTileDraw, so pObject is a tile record
//      dwFlags & 2  -> GFX_DrawUnitShadow, so pObject is a unit
//      neither      -> the ordinary lit unit draw
//  The outer test is `(dwFlags & 3) != 0` and the two arms inside it are tested in the order 1 then 2, so a node with both bits takes the tile arm.
//  GFX_DrawUnitShadow takes one argument, the unit (`int __fastcall(D2UnitStrc *)`).
//  D2GFX_CheckPerspective is called per node, and only inside the `& 3` branch; the other two passes hoist it.
void __fastcall DWALL_EnvDrawCellShadowsAndUnits(D2ClientViewStrc *pView)
{
    D2ClientWallSquareStrc *pSquare;
    int32_t nSquareCount;
    int32_t nScreenPanels;
    int32_t nSquaresLeft;
    D2ClientDrawNodeStrc *pNode;
    int32_t bPerspective;
    D2DrlgTileDataStrc *pTileData;

    pSquare = pView->pSquares;
    nSquareCount = pView->nGridSide * pView->nGridSide;
    nScreenPanels = SCROLL_GetUiShiftScreen();

    if (nSquareCount)
    {
        nSquaresLeft = nSquareCount;

        do
        {
            for (pNode = pSquare->pShadowList; pNode; pNode = pNode->pNext)
            {
                if ((pNode->dwFlags & 3) != 0)
                {
                    bPerspective = D2GFX_CheckPerspective();

                    if ((pNode->dwFlags & 1) != 0)
                    {
                        pTileData = (D2DrlgTileDataStrc *)pNode->pObject;

                        if (bPerspective == 0)
                            D2GFX_TILE_ShadowTileDraw(pTileData->pTile, pTileData->nDrawX + pView->rcView.left - pView->nOriginX, pView->rcView.top + pTileData->nDrawY - pView->nOriginY, 4, nScreenPanels);
                        else
                            D2GFX_TILE_ShadowTileDraw(pTileData->pTile, pNode->nX, pNode->nY, 4, nScreenPanels);
                    }
                    else if ((pNode->dwFlags & 2) != 0)
                    {
                        GFX_DrawUnitShadow((D2UnitStrc *)pNode->pObject);
                    }
                }
                else
                {
                    DWALL_DrawUnitNodeLit(pNode);
                }
            }

            if (pSquare->pWallList)
                DWALL_DrawRoomWalls(pSquare, 1u, pView);

            ++pSquare;
            --nSquaresLeft;
        }
        while (nSquaresLeft);
    }
}

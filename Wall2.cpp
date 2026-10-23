#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Wall2.h"

//  Wall2.cpp -- the per-frame draw grid of D2Client.dll 1.10: the grid is built for the viewport (WALL2_WallBuildSquareGrid), filled room by room
//  with wall, roof and unit draw nodes (WALL2_WallAddRoomToSquareGrid), and released with the view. Address range 0x6FAB7F00..0x6FAB8DF7.
//  The original source file is GAME\Wall2.cpp (WALL2_WallFreeSquares hands its path to FOG_Free at line 85).
//  The unit holds seven functions: WALL2_WallInitSquares, WALL2_WallFreeSquares, WALL2_WallBuildSquareGrid, WALL2_WallAddRoomToSquareGrid,
//  WALL2_WallIsPlayerBehindTile, WALL2_WallAppendTileToShadowList and WALL2_WallAppendUnitToShadowList, plus three static helpers for idioms the
//  original inlines.
//  WALL2_WallAddRoomToSquareGrid is reached as a function pointer through DUNGEON_CallRoomCallback; the (ROOMCALLBACKFN) cast at the bottom of
//  WALL2_WallBuildSquareGrid names ours.

//  The source file, as WALL2_WallFreeSquares and WALL2_WallBuildSquareGrid hand it to FOG_Free and FOG_DisplayAssert.
static const char szSourceFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\GAME\\Wall2.cpp";

//1.10f: D2Client.0x6FAB7F00
//  Zeroes the draw-grid fields of a freshly allocated view object: nGridSide, nDrawNodesUsed, dwLayerMask and pSquares. It is the constructor half
//  of a pair: WALL2_WallFreeSquares frees pSquares and then clears the same four fields in the same order. The caller is VIEW_Alloc, right after
//  FOG_Alloc(0xEABC), which does not zero; this is what makes the object safe to hand to WALL2_WallBuildSquareGrid.
//  nSquareCount (+0xEAAC) is left as allocator garbage, and WALL2_WallFreeSquares leaves it too. WALL2_WallBuildSquareGrid tests the pointer
//  before the count, and the reuse path that reads the count is only reached once the pointer is non-null; here the pointer is zero, so the
//  garbage count is never acted on. Reproduced: this function clears four fields, not five.
//  void: the body is one xor eax, eax reused as the source of all four stores, so eax leaves holding zero, and the caller does not read it.

//  The four field offsets.
static_assert(offsetof(D2ClientViewStrc, nGridSide) == 0xEAB0,
              "WALL2_WallInitSquares writes [ecx+0EAB0h] first");
static_assert(offsetof(D2ClientViewStrc, nDrawNodesUsed) == 0xEA9C,
              "WALL2_WallInitSquares writes [ecx+0EA9Ch] second");
static_assert(offsetof(D2ClientViewStrc, dwLayerMask) == 0x38,
              "WALL2_WallInitSquares writes [ecx+38h] third");
static_assert(offsetof(D2ClientViewStrc, pSquares) == 0xEAA8,
              "WALL2_WallInitSquares writes [ecx+0EAA8h] fourth");
static_assert(offsetof(D2ClientViewStrc, nSquareCount) == 0xEAAC,
              "and leaves nSquareCount, the field between the last two, alone");

void __fastcall WALL2_WallInitSquares(D2ClientViewStrc *pView)
{
    pView->nGridSide = 0;
    pView->nDrawNodesUsed = 0;
    pView->dwLayerMask = 0;
    pView->pSquares = nullptr;
}

//1.10f: D2Client.0x6FAB7F20
//  Releases the draw grid (FOG_Free at Wall2.cpp:85) and clears the same four fields WALL2_WallInitSquares sets, in the same order: pSquares,
//  nGridSide, nDrawNodesUsed, dwLayerMask. The caller is VIEW_Free.
//  nSquareCount is left set, which is safe: WALL2_WallBuildSquareGrid decides whether it can reuse an allocation from the count, but it tests the
//  pointer first and this function nulls it, so a stale count is never consulted.
void __fastcall WALL2_WallFreeSquares(D2ClientViewStrc *pView)
{
    if (pView->pSquares)
        FOG_Free(pView->pSquares, szSourceFile, 85, 0);

    pView->pSquares = nullptr;
    pView->nGridSide = 0;
    pView->nDrawNodesUsed = 0;
    pView->dwLayerMask = 0;
}

//1.10f: D2Client.0x6FAB7F60
//  Builds the per-frame draw grid, then hands it to WALL2_WallAddRoomToSquareGrid for every room the player's room touches. The one caller is
//  VIEW_Draw. The assert at Wall2.cpp:828 spells the parameter `pWall->pSquares != NULL`, so the original's identifier for pView is pWall.
//  The grid is the bounding square of the viewport diamond:
//      nTilesAcross = (rcView.right  - rcView.left) / 160 + 11
//      nTilesDown   = (rcView.bottom - rcView.top ) /  80 + 12
//      nGridSide    = nTilesAcross + nTilesDown - 1
//      nSquareCount = nGridSide * nGridSide         36 bytes each
//  160 x 80 is D2's tile size; the side is `across + down - 1` because the visible region is a diamond. Both divisions are signed, written as
//  `/ 160` and `/ 80` on ints. The compiler folded the `+12 - 1` of nGridSide into a single lea.
//  The allocation is reused when pSquares is non-null and the count is unchanged; otherwise it is freed (behind the free's own null check) and
//  allocated again. The original tests pSquares twice, the second being that null check, and both jz share the alloc as their target: a
//  short-circuited `||` whose body opens with a guarded free. MSVC 2022 folds the redundant test away.
//  The cull rect under perspective is 48 pixels wider on three sides: -48 on top and left, +48 on right; the bottom is left alone in both arms.
//  This is a different rect from VIEW_SetRect's pView->rcClip (-80, a 47-pixel bottom inset): it is in game coordinates, offset by the view
//  origin, and it is what the callback culls rooms and tiles against.
//  The original keeps four stack locals (Args, rcClip, pRoom, bSkipUnits1) that are one D2ClientWallRoomArgsStrc: the clip rect is filled in
//  place inside the argument block and then copied to the global, and the last call reads its own first argument back out of the block.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition for the player's position here.
//  The divide by 5 (five subtiles to a tile) is unsigned: mul and shr, not imul and sar. UNITS_GetXPosition returns int, so the cast below is
//  load-bearing: without it MSVC emits the signed sequence, which differs for a negative coordinate. gnPlayerTileX and gnPlayerTileY are unsigned.
//  The arguments are evaluated right to left: the Y copy is computed before the X copy, in one nested expression (UNITS_GetRoom, then D2Common
//  #10095); the three are pure reads, so the order is not observable. #10095 answers an index, not a room: the nIndex of the D2RoomCoordListStrc
//  box that holds the point, stored in gnPlayerCoordListIndex for WALL2_WallIsPlayerBehindTile.
//  The last line hands DUNGEON_CallRoomCallback the address of WALL2_WallAddRoomToSquareGrid (ours).
//  bSkipUnits is passed through untouched and is always 0 in the shipped client (VIEW_Draw's second parameter), so the callback's unit arm,
//  `if (!pArgs->bSkipUnits)`, is always taken.

//  The argument block: the original keeps its four members as one stack block.
static_assert(sizeof(D2ClientWallRoomArgsStrc) == 0x1C,
              "the block ebp-1Ch..ebp-01h that lea at 0x6FAB826D passes whole");
static_assert(offsetof(D2ClientWallRoomArgsStrc, pView) == 0
              && offsetof(D2ClientWallRoomArgsStrc, rcClip) == 4
              && offsetof(D2ClientWallRoomArgsStrc, pPlayerRoom) == 0x14
              && offsetof(D2ClientWallRoomArgsStrc, bSkipUnits) == 0x18,
              "Args, rcClip, pRoom and bSkipUnits1 at ebp-1Ch/-18h/-08h/-04h");

//  The square: its 36 bytes are the allocation's element size and the seeding loop's stride.
static_assert(sizeof(D2ClientWallSquareStrc) == 36,
              "FOG_Alloc asks for 36 * nSquareCount");
static_assert(offsetof(D2ClientWallSquareStrc, nGameX) == 8
              && offsetof(D2ClientWallSquareStrc, nGameY) == 0xC,
              "the seeding loop writes [ecx-1Ch] and [ecx-18h] after +24h");

//  The three view fields this function is the only writer of.
static_assert(offsetof(D2ClientViewStrc, nTilesAcross) == 0xEAB4
              && offsetof(D2ClientViewStrc, nTilesDown) == 0xEAB8,
              "stored at LABEL_5 from ebp and the spilled nTilesDownSaved");
static_assert(offsetof(D2ClientViewStrc, nGridOriginX) == 0xEAA0
              && offsetof(D2ClientViewStrc, nGridOriginY) == 0xEAA4,
              "written from the game coords biased by -3 and by -nTilesAcross");

void __fastcall WALL2_WallBuildSquareGrid(D2ClientViewStrc *pView, int32_t bSkipUnits)
{
    D2ClientWallRoomArgsStrc args;
    D2ClientWallSquareStrc *pSquare;
    D2UnitStrc *pPlayer;
    int32_t nTilesAcross;
    int32_t nTilesDown;
    int32_t nGridSide;
    int32_t nSquareCount;
    int32_t nGameX;
    int32_t nGameY;
    uint32_t nRow;
    uint32_t nCol;

    nTilesAcross = (pView->rcView.right - pView->rcView.left) / 160 + 11;
    nTilesDown = (pView->rcView.bottom - pView->rcView.top) / 80 + 12;
    nGridSide = nTilesAcross + nTilesDown - 1;
    nSquareCount = nGridSide * nGridSide;

    if (!pView->pSquares || nSquareCount != pView->nSquareCount)
    {
        if (pView->pSquares)
            FOG_Free(pView->pSquares, szSourceFile, 58, 0);

        pView->pSquares = (D2ClientWallSquareStrc *)FOG_Alloc(sizeof(D2ClientWallSquareStrc) * nSquareCount, szSourceFile, 61, 0);
        pView->nSquareCount = nSquareCount;
    }

    pView->nTilesDown = nTilesDown;
    pView->nTilesAcross = nTilesAcross;
    pView->nGridSide = nGridSide;

    args.pView = pView;
    args.bSkipUnits = bSkipUnits;

    if (D2GFX_CheckPerspective())
        SetRect(&args.rcClip, pView->nOriginX - 48, pView->nOriginY - 48, pView->nOriginX - pView->rcView.left + pView->rcView.right + 48, pView->nOriginY + pView->rcView.bottom - pView->rcView.top);
    else
        SetRect(&args.rcClip, pView->nOriginX, pView->nOriginY, pView->nOriginX - pView->rcView.left + pView->rcView.right, pView->nOriginY + pView->rcView.bottom - pView->rcView.top);

    gWallClipRect = args.rcClip;

    pView->nDrawNodesUsed = 0;
    pView->dwLayerMask = 0;

    if (!pView->pSquares)
    {
        FOG_DisplayAssert("pWall->pSquares != NULL", szSourceFile, 828);
        exit(-1);
    }

    memset(pView->pSquares, 0, sizeof(D2ClientWallSquareStrc) * pView->nSquareCount);

    D2COMMON_DUNGEON_ClientTileDrawPositionToGameCoords(pView->nOriginX, pView->nOriginY, &nGameX, &nGameY);

    nGameX -= 3;
    nGameY -= pView->nTilesAcross;
    pView->nGridOriginX = nGameX;
    pView->nGridOriginY = nGameY;

    pSquare = pView->pSquares;
    for (nRow = 0; nRow < (uint32_t)pView->nGridSide; ++nRow)
    {
        for (nCol = 0; nCol < (uint32_t)pView->nGridSide; ++nCol)
        {
            pSquare->nGameX = nGameX + nCol;
            pSquare->nGameY = nGameY + nRow;
            ++pSquare;
        }
    }

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        FOG_DisplayAssert("hUnit", szSourceFile, 849);
        exit(-1);
    }

    args.pPlayerRoom = CUNIT_GetPlayerRoom();

    gnPlayerTileX = (uint32_t)UNITS_GetXPosition(pPlayer) / 5;
    gnPlayerTileY = (uint32_t)UNITS_GetYPosition(pPlayer) / 5;

    gnPlayerCoordListIndex = D2COMMON_10095(D2COMMON_UNITS_GetRoom(pPlayer), UNITS_GetXPosition(pPlayer), UNITS_GetYPosition(pPlayer));

    D2COMMON_DUNGEON_CallRoomCallback(args.pPlayerRoom, (ROOMCALLBACKFN)WALL2_WallAddRoomToSquareGrid, &args);
}

//  Three static helpers for idioms WALL2_WallAddRoomToSquareGrid inlines in the original (five, three and two times); none has an out-of-line
//  copy. They are written as functions; if the original spelled any of them as a macro the behaviour is the same.
//    WALL_AllocDrawNode        takes the next node from the view's pool (five sites)
//    WALL_InsertNodeByLayer    inserts a node into a square's list by layer (three sites, sharing one merged tail)
//    WALL_AppendNode           appends a node at the tail (two sites, the unit and its shadow)

//  Takes the next draw node out of the view's pool.
//  BUG: the cursor nDrawNodesUsed is incremented and stored before the bound (3000, 0xBB8) is tested, at all five sites. Past 3000 the writes are
//  suppressed and the count is not, so nDrawNodesUsed is a number of attempts rather than a number of valid nodes. LATENT: nothing reads the
//  count except the allocator's own bound test, which uses the pre-increment value and rejects correctly; the consumers (DWALL_DrawRoomWalls,
//  DWALL_EnvDrawCellUnits, DWALL_EnvDrawRoomTilesLit, DWALL_EnvDrawCellShadowsAndUnits) walk the per-square linked lists, and a node is linked
//  only where the bound test passed. Past 3000 nothing is linked and nothing is read; the count runs away and is zeroed on the next rebuild.
//  The original's base register is pView + 0x38 throughout, which is why the pool resolves there as &pViewAt38[5 * n + 1]; that bias is codegen
//  and the C indexes the member.
static D2ClientDrawNodeStrc *WALL_AllocDrawNode(D2ClientViewStrc *pView)
{
    int32_t nNodeIndex;

    nNodeIndex = pView->nDrawNodesUsed;
    pView->nDrawNodesUsed = nNodeIndex + 1;

    if ((uint32_t)nNodeIndex >= 3000)
        return nullptr;

    return &pView->pDrawNodes[nNodeIndex];
}

//  Inserts a node into a square's list, ascending by its three-bit layer: (pTileRecord->dwFlags >> 14) & 7, read off the tile record the node
//  points at.
//  BUG: the last node in the list is never compared. The loop advances on pNode->pNext and compares against pNode, so it stops one node early.
//  When pNode is the last element pNext is already NULL, the loop body never runs for it, and the shared tail appends unconditionally: a list of
//  exactly one node is never compared at all -- the new node is appended whatever its layer. With N nodes the first N-1 are ordered and the last
//  two can be inverted. REACHABLE: it is a draw-order defect, not a crash -- two tiles in one square whose layers differ can be drawn in the wrong
//  order when the second lands last. Reproduced.
static void WALL_InsertNodeByLayer(D2ClientDrawNodeStrc **ppHead, D2ClientDrawNodeStrc *pNewNode)
{
    D2ClientDrawNodeStrc *pNode;
    D2ClientDrawNodeStrc *pNext;
    D2ClientDrawNodeStrc *pPrev;
    uint32_t nNewLayer;

    pNode = *ppHead;
    if (!pNode)
    {
        *ppHead = pNewNode;
        return;
    }

    pPrev = nullptr;
    nNewLayer = (((uint32_t *)pNewNode->pObject)[5] >> 14) & 7;

    for (pNext = pNode->pNext; pNext; pNext = pNode->pNext)
    {
        if (nNewLayer < ((((uint32_t *)pNode->pObject)[5] >> 14) & 7))
        {
            pNewNode->pNext = pNode;
            if (pPrev)
                pPrev->pNext = pNewNode;
            else
                *ppHead = pNewNode;
            break;
        }
        pPrev = pNode;
        pNode = pNext;
    }

    //  The shared tail the compiler merged out of all three call sites. It is a no-op after a successful mid-list insert, because pNode is then
    //  not the last node; it is the whole operation when the loop fell off the end or never ran.
    if (!pNode->pNext)
        pNode->pNext = pNewNode;
}

//  Appends at the tail, unsorted: what the unit and shadow lists get.
static void WALL_AppendNode(D2ClientDrawNodeStrc **ppHead, D2ClientDrawNodeStrc *pNode)
{
    D2ClientDrawNodeStrc *pTail;
    D2ClientDrawNodeStrc *pNext;

    pTail = *ppHead;
    if (!pTail)
    {
        *ppHead = pNode;
        return;
    }

    for (pNext = pTail->pNext; pNext; pNext = pNext->pNext)
        pTail = pNext;

    pTail->pNext = pNode;
}

//1.10f: D2Client.0x6FAB8290
//  Fills the draw grid for one room: the per-room half of the build, called once per room by DUNGEON_CallRoomCallback out of
//  WALL2_WallBuildSquareGrid. Three passes over the room -- wall tiles, roof tiles, units -- each turning a position into grid coordinates,
//  bounds-checking it, and linking a node into one of the square's five lists. It returns 1 unconditionally, which for a ROOMCALLBACKFN is "keep
//  going". The original inlines WALL_AllocDrawNode, WALL_InsertNodeByLayer and WALL_AppendNode here.
//  The second parameter is the argument block (D2ClientWallRoomArgsStrc), not the view: it has rcClip at +4 where the view has rcView, and
//  bSkipUnits at +0x18. bSkipUnits is always 0 in the shipped client, so the unit pass always runs; the skip path is reachable code that
//  nothing reaches.
//  The unit pass carries two asserts, Wall2.cpp:768 (lTileX) and :769 (lTileY); their strings give the original's identifiers: the parameter is
//  pWall and nGridSide is dwSquareSide. The wall and roof passes do the same bounds check and carry no assert.
//  The redundant `pUnit && ...` test in the unit pass is kept because the original has it (pUnit is the loop variable, which the loop already tests).
//  The three tile arms:
//      nTileType == 15            -> pFloorList (+0x1C), layer bit gdwBitMasks[1]
//      nTileType < 15 or > 19     -> pWallList  (+0x14), no layer bit; this is the arm that drives the wall fade
//      nTileType 16..19           -> pTileList  (+0x20), layer bit gdwBitMasks[0]
//  Arms two and three also set `pSquare->field_00 = 4` when the type is neither 0 nor 13 and flag 4 is clear; arm one never does. field_00 is
//  zeroed for every tile that lands in the square, not once per square, so a square with several tiles is cleared several times and the last
//  tile wins. It has one writer and no reader anywhere in the module; field_04 is touched by nothing but WALL2_WallBuildSquareGrid's memset of
//  the grid. Both keep their placeholder names: no reader says what they were for.
//  A tile whose dwFlags has none of 0x1C000 goes to WALL2_WallAppendTileToShadowList -- the square's shadow list -- instead of any of these three.
//  The wall fade: WALL2_WallIsPlayerBehindTile decides the direction. Fading out sets nAlphaFrom = 0xFF, nAlphaTo = 0x80 and `dwFadeState |= 3`
//  (full width); fading back sets 0x80 / 0xFF and `dwFadeState = (dwFadeState & ~1) | 2`, a byte-wide pair (`and al, 0FEh` then `or al, 2` at
//  0x6FAB8663), so the upper 24 bits ride through.
//  The deadline is GetTickCount() + 500, prorated when a fade is interrupted: `+ 500 * (nAlpha - 255) / 127` outward and
//  `+ 500 * (128 - nAlpha) / 127` back, both signed divisions, both negative in the interrupted case, so a reversal finishes in the time the
//  remaining distance deserves. 0xFF to 0x80 is the range and 127 is the divisor.
//  The fade runs only when a node was actually allocated: the bound check jumps past it, so a full pool freezes wall transparency as well as
//  dropping the tile.
//  The wall pass culls on `nDrawY - pTile->nRoofHeight` and the roof pass on `nDrawY` alone; not symmetric, reproduced as found.

//  The record the sort key comes from, and the pool the nodes come from.
static_assert(offsetof(D2DrlgTileDataStrc, dwFlags) == 0x14,
              "the layer is (pObject->dwFlags >> 14) & 7, read at +0x14");
static_assert(sizeof(D2DrlgTileDataStrc) == 48,
              "the tile loops step by 12 dwords");
static_assert(offsetof(D2TileLibraryEntryStrc, nRoofHeight) == 4,
              "the wall pass culls on nDrawY minus the u16 at +4");
static_assert(sizeof(D2ClientDrawNodeStrc) == 20
              && offsetof(D2ClientViewStrc, pDrawNodes) == 0x3C,
              "&pViewAt38[5*n + 1] is 0x38 + 4 + 20n, so &pDrawNodes[n]");
static_assert(offsetof(D2ClientWallSquareStrc, pShadowList) == 0x10
              && offsetof(D2ClientWallSquareStrc, pWallList) == 0x14
              && offsetof(D2ClientWallSquareStrc, pUnitList) == 0x18
              && offsetof(D2ClientWallSquareStrc, pFloorList) == 0x1C
              && offsetof(D2ClientWallSquareStrc, pTileList) == 0x20,
              "the five lists the three arms and the unit pass write");

int32_t __stdcall WALL2_WallAddRoomToSquareGrid(D2ActiveRoomStrc *pRoom, D2ClientWallRoomArgsStrc *pArgs)
{
    D2ClientViewStrc *pView;
    uint32_t *pContext;
    D2DrlgTileDataStrc *pTiles;
    D2DrlgTileDataStrc *pTile;
    D2ClientWallSquareStrc *pSquare;
    D2ClientDrawNodeStrc *pNewNode;
    D2UnitStrc *pUnit;
    D2DrlgCoordsStrc RoomCoords;
    RECT rcRoom;
    int32_t nTileCount;
    uint32_t nTileIndex;
    int32_t nTileRadius;
    int32_t nTileTop;
    int32_t nGameX;
    int32_t nGameY;
    int32_t nLocalX;
    int32_t nLocalY;
    int32_t nGridSide;
    int32_t nTileType;
    uint32_t dwTileFlags;
    uint32_t dwFadeDeadline;
    uint32_t dwFadeState;
    uint8_t nAlpha;

    pView = pArgs->pView;

    //  The biased base the original keeps in ebp. The two shadow-list appenders take it as their first argument (WALL2_WallAppendTileToShadowList
    //  says how they reach the view).
    pContext = (uint32_t *)&pView->dwLayerMask;

    D2COMMON_10052(pRoom, &rcRoom);

    if (rcRoom.left > pArgs->rcClip.right || rcRoom.top > pArgs->rcClip.bottom + 400 || rcRoom.right < pArgs->rcClip.left - 200 || rcRoom.bottom < pArgs->rcClip.top)
    {
        return 1;
    }

    //  ---- pass 1: the wall tiles -----------------------------------------
    pTiles = D2COMMON_DUNGEON_GetWallTilesFromRoom(pRoom, &nTileCount);
    nTileIndex = 0;
    if (nTileCount)
    {
        pTile = pTiles;
        do
        {
            nTileRadius = D2CMP_10084((uint32_t)(UINT_PTR)pTile->pTile);
            D2COMMON_DUNGEON_ClientTileDrawPositionToGameCoords(pTile->nDrawX, pTile->nDrawY, &nGameX, &nGameY);
            --nGameY;

            if (pTile->nDrawX <= pArgs->rcClip.right && pTile->nDrawX + 160 >= pArgs->rcClip.left)
            {
                nTileTop = pTile->nDrawY - pTile->pTile->nRoofHeight;
                if (nTileTop + nTileRadius <= pArgs->rcClip.bottom && nTileTop - nTileRadius >= pArgs->rcClip.top)
                {
                    nLocalX = nGameX - pView->nGridOriginX;
                    nLocalY = nGameY - pView->nGridOriginY;
                    if (nLocalX >= 0 && nLocalY >= 0)
                    {
                        nGridSide = pView->nGridSide;
                        if (nLocalX < nGridSide && nLocalY < nGridSide)
                        {
                            pSquare = &pView->pSquares[nLocalX + nLocalY * nGridSide];
                            pSquare->field_00 = 0;

                            dwTileFlags = pTile->dwFlags;
                            nTileType = pTile->nTileType;

                            if ((dwTileFlags & 0x1C000) == 0)
                            {
                                WALL2_WallAppendTileToShadowList(pContext, pSquare, pTile, pRoom);
                            }
                            else if (nTileType == 15)
                            {
                                pNewNode = WALL_AllocDrawNode(pView);
                                if (pNewNode)
                                {
                                    pNewNode->pObject = pTile;
                                    pNewNode->dwFlags = 1;
                                    pNewNode->pNext = nullptr;
                                    D2COMMON_DUNGEON_GetRoomCoordinates(pRoom, &RoomCoords);
                                    pNewNode->nX = 8 * (RoomCoords.nSubtileX + 5 * pTile->nPosX);
                                    pNewNode->nY = 8 * (RoomCoords.nSubtileY + 5 * pTile->nPosY);
                                    pView->dwLayerMask |= gdwBitMasks[1];
                                    WALL_InsertNodeByLayer(&pSquare->pFloorList, pNewNode);
                                }
                            }
                            else if (nTileType < 15 || nTileType > 19)
                            {
                                if (nTileType && nTileType != 13 && (dwTileFlags & 4) == 0)
                                {
                                    pSquare->field_00 = 4;
                                }

                                pNewNode = WALL_AllocDrawNode(pView);
                                if (pNewNode)
                                {
                                    dwFadeDeadline = GetTickCount() + 500;
                                    pNewNode->dwFlags = 1;
                                    pNewNode->pObject = pTile;
                                    pNewNode->pNext = nullptr;

                                    if ((pTile->dwFlags & 4) == 0)
                                    {
                                        if (WALL2_WallIsPlayerBehindTile(pRoom, pTile))
                                        {
                                            dwFadeState = pTile->dwFadeState;
                                            if ((dwFadeState & 1) == 0)
                                            {
                                                nAlpha = pTile->nAlpha;
                                                pTile->nAlphaFrom = 0xFF;
                                                pTile->nAlphaTo = 0x80;
                                                if (nAlpha == 0xFF)
                                                    pTile->dwFadeEndTick = dwFadeDeadline;
                                                else
                                                    pTile->dwFadeEndTick = dwFadeDeadline + 500 * (nAlpha - 255) / 127;
                                                pTile->dwFadeState = dwFadeState | 3;
                                            }
                                        }
                                        else
                                        {
                                            dwFadeState = pTile->dwFadeState;
                                            if ((dwFadeState & 1) != 0)
                                            {
                                                nAlpha = pTile->nAlpha;
                                                pTile->nAlphaFrom = 0x80;
                                                pTile->nAlphaTo = 0xFF;
                                                if (nAlpha == 0x80)
                                                    pTile->dwFadeEndTick = dwFadeDeadline;
                                                else
                                                    pTile->dwFadeEndTick = dwFadeDeadline + 500 * (128 - nAlpha) / 127;
                                                pTile->dwFadeState = (dwFadeState & ~1u) | 2;
                                            }
                                        }
                                    }

                                    D2COMMON_DUNGEON_GetRoomCoordinates(pRoom, &RoomCoords);
                                    pNewNode->nX = 8 * (RoomCoords.nSubtileX + 5 * pTile->nPosX);
                                    pNewNode->nY = 8 * (RoomCoords.nSubtileY + 5 * pTile->nPosY);
                                    WALL_InsertNodeByLayer(&pSquare->pWallList, pNewNode);
                                }
                            }
                            else
                            {
                                if (nTileType && nTileType != 13 && (dwTileFlags & 4) == 0)
                                {
                                    pSquare->field_00 = 4;
                                }

                                pNewNode = WALL_AllocDrawNode(pView);
                                if (pNewNode)
                                {
                                    pNewNode->pObject = pTile;
                                    pNewNode->dwFlags = 1;
                                    pNewNode->pNext = nullptr;
                                    D2COMMON_DUNGEON_GetRoomCoordinates(pRoom, &RoomCoords);
                                    pNewNode->nX = 8 * (RoomCoords.nSubtileX + 5 * pTile->nPosX);
                                    pNewNode->nY = 8 * (RoomCoords.nSubtileY + 5 * pTile->nPosY);
                                    pView->dwLayerMask |= gdwBitMasks[0];
                                    WALL_InsertNodeByLayer(&pSquare->pTileList, pNewNode);
                                }
                            }
                        }
                    }
                }
            }

            ++pTile;
            ++nTileIndex;
        }
        while (nTileIndex < (uint32_t)nTileCount);
    }

    //  ---- pass 2: the roof tiles -----------------------------------------
    //
    //  Same walk, but every roof tile goes to WALL2_WallAppendTileToShadowList -- the square's shadow list, drawn as a shadow tile -- and the
    //  vertical cull uses nDrawY itself where the wall pass subtracted the tile's roof height.
    pTiles = D2COMMON_DUNGEON_GetRoofTilesFromRoom(pRoom, &nTileCount);
    nTileIndex = 0;
    if (nTileCount)
    {
        pTile = pTiles;
        do
        {
            nTileRadius = D2CMP_10084((uint32_t)(UINT_PTR)pTile->pTile);
            D2COMMON_DUNGEON_ClientTileDrawPositionToGameCoords(pTile->nDrawX, pTile->nDrawY, &nGameX, &nGameY);
            --nGameY;

            if (pTile->nDrawX <= pArgs->rcClip.right && pTile->nDrawX + 160 >= pArgs->rcClip.left)
            {
                if (pTile->nDrawY + nTileRadius <= pArgs->rcClip.bottom && pTile->nDrawY - nTileRadius >= pArgs->rcClip.top)
                {
                    nLocalX = nGameX - pView->nGridOriginX;
                    nLocalY = nGameY - pView->nGridOriginY;
                    if (nLocalX >= 0 && nLocalY >= 0)
                    {
                        nGridSide = pView->nGridSide;
                        if (nLocalX < nGridSide && nLocalY < nGridSide)
                        {
                            WALL2_WallAppendTileToShadowList(pContext, &pView->pSquares[nLocalX + nLocalY * nGridSide], pTile, pRoom);
                        }
                    }
                }
            }

            ++pTile;
            ++nTileIndex;
        }
        while (nTileIndex < (uint32_t)nTileCount);
    }

    //  ---- pass 3: the units ----------------------------------------------
    //
    //  Two different coordinate pairs in one loop: the grid position comes from D2Common's UNITS_GetClientCoordX/Y (#10333/#10334), the node
    //  position from D2Client's own UNITS_GetXPosition and UNITS_GetYPosition. They are not interchangeable.
    if (!pArgs->bSkipUnits)
    {
        for (pUnit = D2COMMON_DUNGEON_GetFirstUnitInRoom(pRoom); pUnit; pUnit = pUnit->pRoomNext)
        {
            nGameX = D2COMMON_UNITS_GetClientCoordX(pUnit);
            nGameY = D2COMMON_UNITS_GetClientCoordY(pUnit);
            D2COMMON_DUNGEON_ClientTileDrawPositionToGameCoords(nGameX, nGameY, &nGameX, &nGameY);

            nLocalX = nGameX - pView->nGridOriginX;
            nLocalY = nGameY - pView->nGridOriginY;
            if (nLocalX < 0 || nLocalY < 0)
                continue;

            nGridSide = pView->nGridSide;
            if (nLocalX >= nGridSide || nLocalY >= nGridSide)
                continue;

            //  Both asserts are unreachable: the enclosing ifs already exclude their conditions. Reproduced as written, with both `< 0` halves.
            if (nLocalX < 0 || nLocalX >= nGridSide)
            {
                FOG_DisplayAssert("lTileX >= 0 && lTileX < (LONG) ( pWall->dwSquareSide )", szSourceFile, 768);
                exit(-1);
            }
            if (nLocalY < 0 || nLocalY >= nGridSide)
            {
                FOG_DisplayAssert("lTileY >= 0 && lTileY < (LONG) ( pWall->dwSquareSide )", szSourceFile, 769);
                exit(-1);
            }

            pSquare = &pView->pSquares[nLocalX + nLocalY * nGridSide];

            if (CUNIT_ShouldDrawUnitUnder(pUnit) || (pUnit && ((pUnit->dwFlags >> 20) & 1)))
            {
                WALL2_WallAppendUnitToShadowList(pContext, pSquare, pUnit);
                continue;
            }

            pNewNode = WALL_AllocDrawNode(pView);
            if (pNewNode)
            {
                pNewNode->dwFlags = 0;
                pNewNode->pObject = pUnit;
                pNewNode->pNext = nullptr;
                pNewNode->nX = 8 * UNITS_GetXPosition(pUnit);
                pNewNode->nY = 8 * UNITS_GetYPosition(pUnit);
                WALL_AppendNode(&pSquare->pUnitList, pNewNode);
            }

            if (pUnit && ((pUnit->dwFlagEx >> 7) & 1))
            {
                pNewNode = WALL_AllocDrawNode(pView);
                if (pNewNode)
                {
                    pNewNode->dwFlags = 2;
                    pNewNode->pObject = pUnit;
                    pNewNode->pNext = nullptr;
                    pNewNode->nX = 8 * UNITS_GetXPosition(pUnit);
                    pNewNode->nY = 8 * UNITS_GetYPosition(pUnit);
                    pView->dwLayerMask |= gdwBitMasks[3];
                    WALL_AppendNode(&pSquare->pShadowList, pNewNode);
                }
            }
        }
    }

    return 1;
}

//  The tile fields WALL2_WallIsPlayerBehindTile reads that no other file pins, and the box its live branch compares against (D2RoomCoordListStrc).
static_assert(offsetof(D2DrlgTileDataStrc, unk0x10) == 0x10
              && offsetof(D2DrlgTileDataStrc, nTileType) == 0x1C,
              "mov esi, [esi+10h] at 0x6FAB8B60; mov eax, [esi+1Ch] at 0x6FAB8BE9");
static_assert(offsetof(D2RoomCoordListStrc, pBox) == 0
              && offsetof(D2DrlgCoordStrc, nPosX) == 0 && offsetof(D2DrlgCoordStrc, nPosY) == 4
              && offsetof(D2RoomCoordListStrc, nIndex) == 0x28,
              "the box's [esi] and [esi+4] at 0x6FAB8B82/0x6FAB8B99, its index [esi+28h] at 0x6FAB8B6B");

//1.10f: D2Client.0x6FAB8B50
//  Whether the player is behind this wall tile: the question WALL2_WallAddRoomToSquareGrid's wall pass asks of a wall tile (a type below 15 or above
//  19, dwFlags bit 2 clear) to decide which way it fades: TRUE fades it out to alpha 0x80, FALSE back to 0xFF.
//  Two branches on gnWallEdgeTestMode, and only one runs: it is .data, 1 in the image, read once at the top of this function and written by
//  nothing. So mode 1 is the program and mode 0 is dead code, written because the original has it.
//  The live branch compares the player with the tile's coord-list box. The tile's +0x10 is filled by D2Common from the room's pIndexY grid, whose
//  cells are D2RoomCoordListStrc pointers (boxes of tiles); nIndex is at +0x28. The player's own box's index is what WALL2_WallBuildSquareGrid
//  stores in gnPlayerCoordListIndex from D2Common #10095. So: no box, or the player's own -> FALSE; otherwise TRUE when the player's tile is left
//  of or above the box's corner -- `jge` at 0x6FAB8B86 on X, `setl` at 0x6FAB8BA2 on Y, both signed. Behind the wall of another area, in
//  isometric terms.
//  The dead branch is the tile's position in subtiles (the room's origin plus five times its nPosX / nPosY) against the player's tile times five,
//  in a band of twenty subtiles short of it, and then the tile's nTileType decides:
//      X band  types 1 4 5 7 8 10 12  -> TRUE
//              types 2 3 6 9 11, and anything outside 1..12 -> the Y test
//      Y band  types 2 3 6 7 9 11 12  -> TRUE
//              types 4 5 8 10, and anything outside 2..12   -> FALSE
//  gnPlayerTileX and gnPlayerTileY are unsigned; every compare here is signed (`jge`, `jle`, `setl`), so each read is cast to int. The two fives
//  (`lea r, [r+r*4]`) are taken on the unsigned value first, the same bits without a signed overflow in the C. The index tests are unsigned
//  (`cmp edi, 0Bh / ja`, `cmp eax, 0Ah / ja`), which a C switch is.
BOOL __fastcall WALL2_WallIsPlayerBehindTile(D2ActiveRoomStrc *pRoom, D2DrlgTileDataStrc *pTile)
{
    D2RoomCoordListStrc *pCoordList;
    D2DrlgCoordsStrc tCoords;
    int32_t nTileSubX;
    int32_t nTileSubY;
    int32_t nPlayerSubX;
    int32_t nPlayerSubY;

    if (gnWallEdgeTestMode)
    {
        pCoordList = (D2RoomCoordListStrc *)(UINT_PTR)pTile->unk0x10;
        if (!pCoordList || pCoordList->nIndex == gnPlayerCoordListIndex)
            return FALSE;
        if ((int32_t)gnPlayerTileX < pCoordList->pBox[0].nPosX)
            return TRUE;
        return (int32_t)gnPlayerTileY < pCoordList->pBox[0].nPosY;
    }

    D2COMMON_DUNGEON_GetRoomCoordinates(pRoom, &tCoords);
    nTileSubX = tCoords.nSubtileX + 5 * pTile->nPosX;
    nTileSubY = tCoords.nSubtileY + 5 * pTile->nPosY;
    nPlayerSubX = (int32_t)(5 * gnPlayerTileX);
    nPlayerSubY = (int32_t)(5 * gnPlayerTileY);

    if (nPlayerSubX < nTileSubX && nPlayerSubX > nTileSubX - 20)
    {
        switch (pTile->nTileType)
        {
        case 1:
        case 4:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return TRUE;
        }
    }

    if (nPlayerSubY < nTileSubY && nPlayerSubY > nTileSubY - 20)
    {
        switch (pTile->nTileType)
        {
        case 2:
        case 3:
        case 6:
        case 7:
        case 9:
        case 11:
        case 12:
            return TRUE;
        }
    }

    return FALSE;
}

//  pContext is &pView->dwLayerMask, and the two appenders below reach the view through it, from edi in one and ebx in the other: [reg+0EA64h] is
//  nDrawNodesUsed and [reg+edx*4+4], with edx five times the index, is pDrawNodes[index].
static_assert(offsetof(D2ClientViewStrc, nDrawNodesUsed)
                  - offsetof(D2ClientViewStrc, dwLayerMask) == 0xEA64
              && offsetof(D2ClientViewStrc, pDrawNodes)
                  - offsetof(D2ClientViewStrc, dwLayerMask) == 4,
              "mov eax, [edi+0EA64h] at 0x6FAB8C6A; lea esi, [edi+edx*4+4] at 0x6FAB8C94");

//1.10f: D2Client.0x6FAB8C60
//  Puts one tile on a square's shadow list: a node of kind 1, a tile record, at the tail of pSquare->pShadowList (+0x10), and gdwBitMasks[3] into
//  the view's layer mask. Its reader is dWall.cpp's DWALL_EnvDrawCellShadowsAndUnits, which draws a kind-1 node with D2Gfx's TILE_ShadowTileDraw.
//  It is called for a wall tile with none of the layer bits 0x1C000 and for every tile of the room's third tile array (the roofs array,
//  DUNGEON_GetRoofTilesFromRoom, which this client draws as shadows).
//  pContext is &pView->dwLayerMask, the base WALL2_WallAddRoomToSquareGrid keeps in ebp, and the body reaches the view through it: [edi+0EA64h] is
//  nDrawNodesUsed and `lea esi, [edi+edx*4+4]`, edx five times the index, is &pDrawNodes[index]. Ours is written through the view that contains it
//  -- the same addresses.
//  The node comes from the pool as WALL_AllocDrawNode takes one: the count read, stored back incremented, then the unsigned bound, so past 3000
//  only the count moves. Written in place, as the original has it. The stores are in the original's order: the kind, pObject, pNext; the
//  position, in eighths of a subtile (`shl eax, 3`) from the room's origin and the tile's position; the layer bit; then the tail link, found by
//  walking pNext (+0x10).
void __fastcall WALL2_WallAppendTileToShadowList(uint32_t *pContext, D2ClientWallSquareStrc *pSquare, D2DrlgTileDataStrc *pTile, D2ActiveRoomStrc *pRoom)
{
    D2ClientViewStrc *pView;
    uint32_t nNodeIndex;
    D2ClientDrawNodeStrc *pNode;
    D2ClientDrawNodeStrc *pTail;
    D2ClientDrawNodeStrc *pNext;
    D2DrlgCoordsStrc tCoords;

    pView = (D2ClientViewStrc *)((char *)pContext - offsetof(D2ClientViewStrc, dwLayerMask));

    nNodeIndex = (uint32_t)pView->nDrawNodesUsed;
    pView->nDrawNodesUsed = (int32_t)(nNodeIndex + 1);
    if (nNodeIndex >= 3000)
        return;

    pNode = &pView->pDrawNodes[nNodeIndex];
    pNode->dwFlags = 1;
    pNode->pObject = pTile;
    pNode->pNext = nullptr;
    D2COMMON_DUNGEON_GetRoomCoordinates(pRoom, &tCoords);
    pNode->nX = 8 * (tCoords.nSubtileX + 5 * pTile->nPosX);
    pNode->nY = 8 * (tCoords.nSubtileY + 5 * pTile->nPosY);
    pView->dwLayerMask |= gdwBitMasks[3];

    pTail = pSquare->pShadowList;
    if (!pTail)
    {
        pSquare->pShadowList = pNode;
        return;
    }
    for (pNext = pTail->pNext; pNext; pNext = pNext->pNext)
        pTail = pNext;
    pTail->pNext = pNode;
}

//1.10f: D2Client.0x6FAB8D20
//  Puts one unit on a square's shadow list: WALL2_WallAppendTileToShadowList's twin with a node of kind 0, the unit, and its position from the
//  unit itself. The same reader draws a kind-0 node with DWALL_DrawUnitNodeLit, the ordinary lit unit draw, in the shadow pass before the walls.
//  It is called from WALL2_WallAddRoomToSquareGrid's unit pass for a unit that CUNIT_ShouldDrawUnitUnder claims or whose dwFlags bit 20 is set --
//  one arm each in the original, one `if (A || B)` in the C there.
//  The context and the node are as in its twin. The position is UNITS_GetXPosition and UNITS_GetYPosition, X first, stored in eighths
//  (`lea ecx, [eax*8]` for X, `shl eax, 3` for Y). The tail walk tests the next pointer before it moves, the same list end its twin reaches.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall WALL2_WallAppendUnitToShadowList(uint32_t *pContext, D2ClientWallSquareStrc *pSquare, D2UnitStrc *pUnit)
{
    D2ClientViewStrc *pView;
    uint32_t nNodeIndex;
    D2ClientDrawNodeStrc *pNode;
    D2ClientDrawNodeStrc *pTail;
    int32_t nX;
    int32_t nY;

    pView = (D2ClientViewStrc *)((char *)pContext - offsetof(D2ClientViewStrc, dwLayerMask));

    nNodeIndex = (uint32_t)pView->nDrawNodesUsed;
    pView->nDrawNodesUsed = (int32_t)(nNodeIndex + 1);
    if (nNodeIndex >= 3000)
        return;

    pNode = &pView->pDrawNodes[nNodeIndex];
    pNode->dwFlags = 0;
    pNode->pObject = pUnit;
    pNode->pNext = nullptr;

    nX = UNITS_GetXPosition(pUnit);
    pNode->nX = 8 * nX;

    nY = UNITS_GetYPosition(pUnit);
    pNode->nY = 8 * nY;

    pView->dwLayerMask |= gdwBitMasks[3];

    pTail = pSquare->pShadowList;
    if (!pTail)
    {
        pSquare->pShadowList = pNode;
        return;
    }
    while (pTail->pNext)
        pTail = pTail->pNext;
    pTail->pNext = pNode;
}

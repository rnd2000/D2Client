#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "CoreArchive.h"
#include "View.h"

//  View.cpp -- the viewport of D2Client.dll 1.10: the five Gouraud-shading lookup tables (built once per game, released on leaving it), the
//  one viewport object (allocate, free, rect, origin, redraw bit) and VIEW_Draw. The functions run from 0x6FAB6AC0 (VIEW_AllocGouraudTables) to
//  0x6FAB7200 (VIEW_Draw). The original source file is GAME\View.cpp: VIEW_Alloc hands that path to three asserts and a FOG_Alloc.

//  The source file, as VIEW_Alloc's three asserts hand it over.
static const char szSourceFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\GAME\\View.cpp";

//1.10f: D2Client.0x6FAB6AC0
//  Builds the five Gouraud-shading lookup tables, once per game. It refuses to run twice: five asserts in the negative (!sgpGouraudTblX ..,
//  View.cpp:202-206) mean none of the five may exist yet; VIEW_FreeGouraudTables asserts the same five in the positive. The caller is
//  GAME_AppModeGame.
//  Two of the five hold pointers and three hold bytes (allocation size over entry count is the element width):
//      sgpGouraudTblX       0x9000  / (32*32*9)  = 4   pointer
//      sgpGouraudTblY       0x4003  / (32*32*16) = 1   byte + 3
//      sgpGouraudTblXVal    0x8003  / (32*32*32) = 1   byte + 3
//      gpSubtileGouraudTbl  0x20000 / (32*32*32) = 4   pointer
//      gpTileGouraudTbl     0xA003  / (32*32*40) = 1   byte + 3
//  A pointer entry is &gShadowLightGammaPalette + 256 * shade, a pointer to a palette row; the byte tables never mention the palette. The
//  three-byte tail sits on exactly the three byte tables.
//  The five loops are one nest, differing only in the inner count, the element and the allocation:
//      for nRow  in 0..31:   nBase = nRow*0x10000, nSpan = -nRow*0x10000
//        for nCol in 0..31:  nDelta = nSpan + nCol*0x10000 = (nCol-nRow)<<16
//          for i   in 0..N-1: emit(nAcc >> 16), nAcc += nDelta / N
//  so entry (nRow, nCol, i) is nRow + i*(nCol - nRow)/N -- a linear ramp in 16.16 fixed point from shade nRow toward shade nCol in N steps.
//  The shade never leaves [0, 31], which is the 32 rows of the shadow palette, so the pointer tables cannot run off it.
//  Every division is signed and nDelta goes negative whenever nCol < nRow, so the truncation direction matters: `/ 8` is C's own signed
//  division by 8.
//  The ninth entry of table one repeats the eighth, not the endpoint: the byte computed in the last inner iteration is still live when the
//  extra store runs. 32 x 32 x 9 pointers = 0x9000 exactly, so the ninth is inside the allocation.
//  Five separate nests are written, not one helper: the two shapes differ in what they store (a dword pointer built from the shade, versus the
//  shade byte itself), and five separate nests is what the original contains.
//  gpSubtileGouraudTbl (0x20000 bytes) and gpTileGouraudTbl (0xA003) are built here, freed in VIEW_FreeGouraudTables, and read by nothing;
//  the other three are cached into the view object by VIEW_Alloc (+0x2C/+0x30/+0x34). So 168 KB is computed and released unread on every
//  game, as in the original.
//  The tables are allocated with CLIENT_VirtualAlloc: VirtualAlloc(NULL, dwSize, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE). Nothing here is
//  executed; 0x40 is what the original passes.

//  The palette base (gShadowLightGammaPalette) and its row stride, 256 bytes (shl edx, 8).
static_assert(sizeof(PALETTEENTRY) * 64 == 256,
              "one shadowsPalette row is the 256-byte stride the tables step by");
static_assert(offsetof(D2ShadowLightGammaPaletteStrc, shadowsPalette) == 0,
              "shade 0 is the buffer's own first byte, so base + 256*shade is row shade");

void __cdecl VIEW_AllocGouraudTables()
{
    uint8_t **pTblPtr;      // cursor over a table of palette-row pointers
    uint8_t *pTblByte;      // cursor over a table of shade indices
    uint8_t *pPalette;      // gShadowLightGammaPalette as bytes
    int32_t nBase;              // the row's start shade, 16.16
    int32_t nSpan;              // the row's start delta, 16.16 -- always -nBase
    int32_t nDelta;             // this column's ramp, 16.16
    int32_t nAcc;               // the ramp accumulator, 16.16
    int32_t nCol;
    int32_t i;
    uint8_t nShade;         // the last shade of the inner ramp, kept live for
                            // table one's repeated ninth entry

    if (sgpGouraudTblX)
    {
        FOG_DisplayAssert("!sgpGouraudTblX", szSourceFile, 202);
        exit(-1);
    }
    if (sgpGouraudTblY)
    {
        FOG_DisplayAssert("!sgpGouraudTblY", szSourceFile, 203);
        exit(-1);
    }
    if (sgpGouraudTblXVal)
    {
        FOG_DisplayAssert("!sgpGouraudTblXVal", szSourceFile, 204);
        exit(-1);
    }
    if (gpSubtileGouraudTbl)
    {
        FOG_DisplayAssert("!gpSubtileGouraudTbl", szSourceFile, 205);
        exit(-1);
    }
    if (gpTileGouraudTbl)
    {
        FOG_DisplayAssert("!gpTileGouraudTbl", szSourceFile, 206);
        exit(-1);
    }

    D2WIN_10178(&gShadowLightGammaPalette);
    pPalette = (uint8_t *)&gShadowLightGammaPalette;

    //  1 -- sgpGouraudTblX: 32 x 32 x (8 + 1) palette-row pointers, 0x9000.
    pTblPtr = (uint8_t **)CLIENT_VirtualAlloc(0x9000);
    sgpGouraudTblX = pTblPtr;
    nBase = 0;
    nSpan = 0;
    do
    {
        nDelta = nSpan;
        nCol = 32;
        do
        {
            nAcc = nBase;
            i = 8;
            do
            {
                nShade = (uint8_t)(nAcc / 0x10000);
                *pTblPtr++ = &pPalette[256 * nShade];
                nAcc += nDelta / 8;
            }
            while (--i);
            *pTblPtr++ = &pPalette[256 * nShade];   // the ninth, repeating
            nDelta += 0x10000;
        }
        while (--nCol);
        nBase += 0x10000;
        nSpan -= 0x10000;
    }
    while ((uint32_t)nBase < 0x200000);

    //  2 -- sgpGouraudTblY: 32 x 32 x 16 shade bytes plus three, 0x4003.
    pTblByte = (uint8_t *)CLIENT_VirtualAlloc(0x4003);
    sgpGouraudTblY = pTblByte;
    nBase = 0;
    nSpan = 0;
    do
    {
        nDelta = nSpan;
        nCol = 32;
        do
        {
            nAcc = nBase;
            i = 16;
            do
            {
                *pTblByte++ = (uint8_t)(nAcc / 0x10000);
                nAcc += nDelta / 16;
            }
            while (--i);
            nDelta += 0x10000;
        }
        while (--nCol);
        nBase += 0x10000;
        nSpan -= 0x10000;
    }
    while ((uint32_t)nBase < 0x200000);

    //  3 -- sgpGouraudTblXVal: 32 x 32 x 32 shade bytes plus three, 0x8003.
    pTblByte = (uint8_t *)CLIENT_VirtualAlloc(0x8003);
    sgpGouraudTblXVal = pTblByte;
    nBase = 0;
    nSpan = 0;
    do
    {
        nDelta = nSpan;
        nCol = 32;
        do
        {
            nAcc = nBase;
            i = 32;
            do
            {
                *pTblByte++ = (uint8_t)(nAcc / 0x10000);
                nAcc += nDelta / 32;
            }
            while (--i);
            nDelta += 0x10000;
        }
        while (--nCol);
        nBase += 0x10000;
        nSpan -= 0x10000;
    }
    while ((uint32_t)nBase < 0x200000);

    //  4 -- gpSubtileGouraudTbl: 32 x 32 x 32 palette-row pointers, 0x20000,
    //  and no ninth entry: this one fills its allocation exactly.
    pTblPtr = (uint8_t **)CLIENT_VirtualAlloc(0x20000);
    gpSubtileGouraudTbl = pTblPtr;
    nBase = 0;
    nSpan = 0;
    do
    {
        nDelta = nSpan;
        nCol = 32;
        do
        {
            nAcc = nBase;
            i = 32;
            do
            {
                *pTblPtr++ = &pPalette[256 * (uint8_t)(nAcc / 0x10000)];
                nAcc += nDelta / 32;
            }
            while (--i);
            nDelta += 0x10000;
        }
        while (--nCol);
        nBase += 0x10000;
        nSpan -= 0x10000;
    }
    while ((uint32_t)nBase < 0x200000);

    //  5 -- gpTileGouraudTbl: 32 x 32 x 40 shade bytes plus three, 0xA003.
    //  Forty is not a power of two, so this is the one ramp whose step is a real signed division.
    pTblByte = (uint8_t *)CLIENT_VirtualAlloc(0xA003);
    gpTileGouraudTbl = pTblByte;
    nBase = 0;
    nSpan = 0;
    do
    {
        nDelta = nSpan;
        nCol = 32;
        do
        {
            nAcc = nBase;
            i = 40;
            do
            {
                *pTblByte++ = (uint8_t)(nAcc / 0x10000);
                nAcc += nDelta / 40;
            }
            while (--i);
            nDelta += 0x10000;
        }
        while (--nCol);
        nBase += 0x10000;
        nSpan -= 0x10000;
    }
    while ((uint32_t)nBase < 0x200000);
}

//1.10f: D2Client.0x6FAB6EB0
//  Releases the five tables and the two parallax images: the matched half of VIEW_AllocGouraudTables, run on leaving a game (callers
//  GAME_AppModeGame and GAME_AppModeCloseWindow).
//  The whole body hangs on sgpGouraudTblX alone. Inside, the other four are asserted non-null (View.cpp:251-254) and then tested non-null
//  again before each free; the second test is dead after the assert and is reproduced rather than folded away. sgpGouraudTblX itself is freed
//  without a null test, because the enclosing if already is one.
//  ghScreenShakeSound = 0 sits between the table stores and the image unloads: a handle drop with no call to the sound layer. The original
//  schedules that store between the summit compare and its jz; the source order is kept.
void __cdecl VIEW_FreeGouraudTables()
{
    if (sgpGouraudTblX)
    {
        if (!sgpGouraudTblY)
        {
            FOG_DisplayAssert("sgpGouraudTblY", szSourceFile, 251);
            exit(-1);
        }
        if (!sgpGouraudTblXVal)
        {
            FOG_DisplayAssert("sgpGouraudTblXVal", szSourceFile, 252);
            exit(-1);
        }
        if (!gpSubtileGouraudTbl)
        {
            FOG_DisplayAssert("gpSubtileGouraudTbl", szSourceFile, 253);
            exit(-1);
        }
        if (!gpTileGouraudTbl)
        {
            FOG_DisplayAssert("gpTileGouraudTbl", szSourceFile, 254);
            exit(-1);
        }

        CLIENT_VirtualFree(sgpGouraudTblX);
        if (sgpGouraudTblY)
            CLIENT_VirtualFree(sgpGouraudTblY);
        if (sgpGouraudTblXVal)
            CLIENT_VirtualFree(sgpGouraudTblXVal);
        if (gpSubtileGouraudTbl)
            CLIENT_VirtualFree(gpSubtileGouraudTbl);
        if (gpTileGouraudTbl)
            CLIENT_VirtualFree(gpTileGouraudTbl);

        sgpGouraudTblX = nullptr;
        sgpGouraudTblY = nullptr;
        sgpGouraudTblXVal = nullptr;
        gpSubtileGouraudTbl = nullptr;
        gpTileGouraudTbl = nullptr;
        ghScreenShakeSound = 0;

        if (gpImgParallaxSummit)
        {
            ARCHIVE_CellFileFree(gpImgParallaxSummit);
            gpImgParallaxSummit = nullptr;
        }
        if (gpImgParallaxCloud)
        {
            ARCHIVE_CellFileFree(gpImgParallaxCloud);
            gpImgParallaxCloud = nullptr;
        }
    }
}

//1.10f: D2Client.0x6FAB6FF0
//  Allocates the client's one viewport (60092 bytes, View.cpp:285) and returns it. The only caller is GAME_InitGame, which allocates it once and
//  keeps it in gpView for the life of the process: a second game reuses the same object, because the call is guarded by `if (!gpView)`.
//  The size is written as sizeof(D2ClientViewStrc) rather than the literal 0xEABC, so the struct and the allocation cannot drift apart; the
//  assertion beside it pins that sizeof to the instruction.
//  The three asserts say the gouraud tables must already exist, so VIEW_AllocGouraudTables runs first.
//  rcView is zeroed here and stays empty until VIEW_SetRect runs, which is what VIEW_Draw's `right || bottom` test is checking.
//  WALL2_WallInitSquares then clears the four draw-grid fields, because FOG_Alloc does not zero.
//  `pView->dwFlags |= 1;` is a dword load, a byte OR and a dword store in the original (or al, 1), and one `or dword ptr` here; that difference
//  is codegen, not meaning.

//  0xEABC, the allocation size, and the struct that has to fill it.
static_assert(sizeof(D2ClientViewStrc) == 0xEABC,
              "VIEW_Alloc asks FOG_Alloc for 0EABCh bytes");
static_assert(offsetof(D2ClientViewStrc, rcView) == 4
              && offsetof(D2ClientViewStrc, pGouraudTblX) == 0x2C
              && offsetof(D2ClientViewStrc, pGouraudTblY) == 0x30
              && offsetof(D2ClientViewStrc, pGouraudTblXVal) == 0x34,
              "the four members VIEW_Alloc writes, at the displacements it uses");

D2ClientViewStrc *__cdecl VIEW_Alloc()
{
    D2ClientViewStrc *pView;

    if (!sgpGouraudTblX)
    {
        FOG_DisplayAssert("sgpGouraudTblX", szSourceFile, 281);
        exit(-1);
    }
    if (!sgpGouraudTblY)
    {
        FOG_DisplayAssert("sgpGouraudTblY", szSourceFile, 282);
        exit(-1);
    }
    if (!sgpGouraudTblXVal)
    {
        FOG_DisplayAssert("sgpGouraudTblXVal", szSourceFile, 283);
        exit(-1);
    }

    pView = (D2ClientViewStrc *)FOG_Alloc(sizeof(D2ClientViewStrc), szSourceFile, 285, 0);

    SetRect(&pView->rcView, 0, 0, 0, 0);
    pView->dwFlags = 0;
    pView->pGouraudTblX = sgpGouraudTblX;
    pView->pGouraudTblY = sgpGouraudTblY;
    pView->pGouraudTblXVal = sgpGouraudTblXVal;

    MEMPART_Nullsub1();
    WALL2_WallInitSquares(pView);

    pView->dwFlags |= 1;
    gnViewEffectsLastFrame = 0;
    return pView;
}

//1.10f: D2Client.0x6FAB70D0
//  Tears the viewport down: releases the draw grid, then the object (View.cpp:321). The caller, GAME_AppModeGame, nulls gpView afterwards.
//  void: the original leaves FOG_Free's answer in eax, and its one call site discards it.
void __fastcall VIEW_Free(D2ClientViewStrc *pView)
{
    WALL2_WallFreeSquares(pView);
    FOG_Free(pView, szSourceFile, 321, 0);
}

//1.10f: D2Client.0x6FAB70F0
//  Raises the redraw bit: bit 0 of dwFlags, which VIEW_Alloc, VIEW_SetRect and VIEW_SetOrigin all set and VIEW_Draw clears. Seven bytes, and the
//  whole function. void: no caller reads the original's eax.
void __fastcall VIEW_MarkDirty(D2ClientViewStrc *pView)
{
    pView->dwFlags |= 1;
}

//1.10f: D2Client.0x6FAB7100
//  Sets the viewport rect, recomputes the cull rect and marks the view dirty. A null pView is silently ignored, where VIEW_Draw asserts.
//  The cull rect is not derived from the view rect: it is always (-80, -80, gnScreenWidth + 80, gnScreenHeight - 47) -- an 80-pixel margin
//  outside the screen on three sides, and 47 pixels cut off the bottom for the control panel. Perspective then widens it further:
//  top -= 30, left -= 45, right += 30.
//  The original hoists one subtraction above two stores (scheduling); the code is three compound assignments in the order the stores happen:
//  top, left, right. D2GFX_CheckPerspective is called before the second SetRect and its answer held in a local; that order is kept.
void __fastcall VIEW_SetRect(D2ClientViewStrc *pView, int32_t xLeft, int32_t yTop, int32_t xRight, int32_t yBottom)
{
    int32_t bPerspective;

    if (pView)
    {
        SetRect(&pView->rcView, xLeft, yTop, xRight, yBottom);
        bPerspective = D2GFX_CheckPerspective();
        SetRect(&pView->rcClip, -80, -80, gnScreenWidth + 80, gnScreenHeight - 47);
        if (bPerspective)
        {
            pView->rcClip.top -= 30;
            pView->rcClip.left -= 45;
            pView->rcClip.right += 30;
        }
        pView->dwFlags |= 1;
    }
}

//1.10f: D2Client.0x6FAB7180
//  Copies the viewport rect (absolute edges) out into four ints. Callers pair it with VIEW_GetOrigin to turn a world position into a screen
//  position, converting to width and height themselves:
//      VIEW_GetRect(gpView, &l, &t, &r, &b);
//      VIEW_GetOrigin(gpView, &x, &y);
//      SetRect(&rc, x, y, x + r - l, b + y - t);
void __fastcall VIEW_GetRect(D2ClientViewStrc *pView, int32_t *pnLeft, int32_t *pnTop, int32_t *pnRight, int32_t *pnBottom)
{
    *pnLeft = pView->rcView.left;
    *pnTop = pView->rcView.top;
    *pnRight = pView->rcView.right;
    *pnBottom = pView->rcView.bottom;
}

//1.10f: D2Client.0x6FAB71B0
//  Moves the viewport, and marks it dirty only if it actually moved: the compare against the stored pair is what stops a stationary camera
//  forcing a redraw every frame. GAME_InitGame places the view at (0, 0) once, and GAME_DrawGameScene updates it every frame; VIEW_GetOrigin is
//  the reader.
//  The original interleaves the three stores (nOriginY, the hoisted flags load, nOriginX, then the flags store). Ours writes them in the order
//  Y, X, flags: where the bytes differ only by instruction scheduling the source shape is followed.
//  void: eax is written three times, each consumed by the instruction after it; on the unchanged path the original leaves nY in eax, an
//  argument handed straight back, and both call sites discard it.
static_assert(offsetof(D2ClientViewStrc, nOriginX) == 0x24,
              "VIEW_SetOrigin reads [ecx+24h] as the view's X origin");
static_assert(offsetof(D2ClientViewStrc, nOriginY) == 0x28,
              "VIEW_SetOrigin reads [ecx+28h] as the view's Y origin");
static_assert(offsetof(D2ClientViewStrc, dwFlags) == 0,
              "VIEW_SetOrigin ORs bit 0 of [ecx], the first member");

void __fastcall VIEW_SetOrigin(D2ClientViewStrc *pView, int32_t nX, int32_t nY)
{
    if (pView->nOriginX != nX || pView->nOriginY != nY)
    {
        pView->nOriginY = nY;
        pView->nOriginX = nX;
        pView->dwFlags |= 1;
    }
}

//1.10f: D2Client.0x6FAB71E0
//  Reads the viewport origin back out: the reader half of VIEW_SetOrigin and the companion of VIEW_GetRect. void: the original leaves nX in eax
//  because it was the last value loaded, and no caller reads it.
void __fastcall VIEW_GetOrigin(D2ClientViewStrc *pView, int32_t *pnX, int32_t *pnY)
{
    *pnX = pView->nOriginX;
    *pnY = pView->nOriginY;
}

//1.10f: D2Client.0x6FAB7200
//  Draws the viewport, then clears the redraw bit. The one caller, GAME_DrawGameScene, passes bSkipUnits = 0.
//  Three guards:
//    1. pView must be non-null, asserted at View.cpp:477 with the operand spelled `hView` (VIEW_SetRect ignores a null instead).
//    2. the view rect must be non-empty -- `right || bottom`, which is false from VIEW_Alloc's SetRect(0,0,0,0) until VIEW_SetRect has run once.
//    3. SCROLL_GetUiShiftScreen() != 3: with both side panels open the world is entirely covered, so it is not drawn at all.
//  BUG: the dirty bit is cleared even when the draw is skipped for reason 3 -- the clear is outside that `if` and inside the rect test -- so a
//  frame spent entirely behind two panels still consumes the redraw request. Reproduced, not corrected: it is only observable if a panel closes
//  in the same frame, and the next mouse move or camera step raises the bit again.
//  The clear is `&= ~1`: in the original `and al, 0FEh` on a dword load and store, which clears bit 0 and nothing else.
//  void: the original's char was the last temporary written to eax; its one call site discards it.
void __fastcall VIEW_Draw(D2ClientViewStrc *pView, int32_t bSkipUnits)
{
    if (!pView)
    {
        FOG_DisplayAssert("hView", szSourceFile, 477);
        exit(-1);
    }

    if (pView->rcView.right || pView->rcView.bottom)
    {
        if (SCROLL_GetUiShiftScreen() != 3)
            WALL2_WallBuildSquareGrid(pView, bSkipUnits);
        pView->dwFlags &= ~1;
    }
}

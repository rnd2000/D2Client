#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "CoreArchive.h"
#include "Cursor.h"

//  Cursor.cpp -- the mouse cursor of D2Client.dll 1.10: loading the seven cursor cels, the mouse-move and button handlers, the cursor's
//  animation state machine, drawing it, and moving the pointer. The original's source file is D2Client\ENGINE\Cursor.cpp (the path its
//  asserts push). Address range 0x6FB57330..0x6FB57CC5; CRT_StaticInit23 (0x6FB57CA0), a compiler-made static initialiser, lies inside it
//  and is not rewritten. The cursor's globals are in Cursor.h.

//  The table has NUM_CURSORS = 7 records and each record is 7 dwords (28 bytes); the index arithmetic 7 * nCursorType counts the second seven.
static_assert(NUM_CURSORS == 7,
              "the original compares against the literal 7 twice in one "
              "function -- `cmp dword ptr gnCursors, 7` at 0x6FB579A0 and "
              "`cmp ecx, 7` at 0x6FB579C7 -- so the name has to mean 7");
static_assert(sizeof(CursorDescStrc) == 28
                  && offsetof(CursorDescStrc, bAnimated) == 0x00
                  && offsetof(CursorDescStrc, bLoops) == 0x04
                  && offsetof(CursorDescStrc, nFrameCount) == 0x08
                  && offsetof(CursorDescStrc, nFrameAdvance) == 0x0C
                  && offsetof(CursorDescStrc, bCanPress) == 0x10
                  && offsetof(CursorDescStrc, pDrawFunction) == 0x14
                  && offsetof(CursorDescStrc, szName) == 0x18,
              "seven consecutive DWORDS: the emitted code reaches bLoops as "
              "`gCursorDescs + 4` and bCanPress as `+ 0x10` at a 28-byte "
              "stride, and this file is where both were named");


//  The source path the asserts of this unit push.
static const char szSourceFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\ENGINE\\Cursor.cpp";

//  The data root every cursor path is built on.
static const char aDataGlobal[] = "DATA\\GLOBAL";


//1.10f: D2Client.0x6FB57330
//  The mouse moved. Caches the position; on a hardware-accelerated surface it clamps the position into the screen and pushes it back to
//  Windows (CURSOR_SetCursorPos), and only when a coordinate was out of range -- the position is corrected, never confirmed. With an error
//  dialog up (FOG_IsHandlingError) the cursor is left where Windows put it.
//  The original inlines CURSOR_GetCursorFrameDuration here: the state transition ends with gnCursorFrame = duration of CURSOR_OHand - 256.
//  void: the original leaves whatever is in eax, which nothing reads; the answer travels in pMsg->bReturn and pMsg->lResult, both set to 0.
//  BUG: gnCursorFrame is written 0 and then written again after an assert that exits, so the first write is dead. LATENT: no path observes
//  the zero.
void __stdcall CURSOR_OnMouseMove(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nLeftInEax;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t bClamped;

    gnMouseX = pMsg->nXpos;
    gnMouseY = pMsg->nYpos;

    if (D2GFX_HardwareAcceleratedRenderMode() && !FOG_IsHandlingError())
    {
        nMouseX = gnMouseX;
        bClamped = 0;

        if (nMouseX > gnScreenWidth - 1)
        {
            nMouseX = gnScreenWidth - 1;
            bClamped = 1;
            gnMouseX = nMouseX;
        }

        nMouseY = gnMouseY;

        if (nMouseY > gnScreenHeight - 1)
        {
            nMouseY = gnScreenHeight - 1;
            gnMouseY = nMouseY;
            bClamped = 1;
        }

        if (bClamped)
        {
            nLeftInEax = SetCursorPos(nMouseX, nMouseY);
            gnMouseX = nMouseX;
            gnMouseY = nMouseY;
            return;
        }
    }

    D2GFX_WINDOW_ShowCursor(1);
    gnCursorLastPositionUpdateTickCount = GetTickCount();

    nLeftInEax = gnCursorState;

    if (nLeftInEax == CURSORSTATE_HandOpened || nLeftInEax == CURSORSTATE_HandOpening)
    {
        nLeftInEax = gnCursors;
        gnCursorState = CURSORSTATE_HandClosing;
        gnCursorType = CURSOR_OHand;
        gnCursorFrame = 0;
        gnCursorFrame = CURSOR_GetCursorFrameDuration(CURSOR_OHand) - 256;
    }

    pMsg->bReturn = 0;
    pMsg->lResult = 0;

}


//1.10f: D2Client.0x6FB57450
//  The mouse moved outside the client area. Over the title bar (hit-test code 2, HTCAPTION, in pMsg->wParam) the cursor is hidden, so the
//  game's cursor does not draw on top of the frame; every other non-client area is ignored.
void __stdcall CURSOR_OnNonClientMouseMove(SMSGHANDLER_PARAMS *pMsg)
{
    if (pMsg->wParam == HTCAPTION)
        D2GFX_WINDOW_ShowCursor(0);

    pMsg->bReturn = 0;
    pMsg->lResult = 0;
}


//1.10f: D2Client.0x6FB57480
//  A button went down. Caches the position, restarts the idle timer and, if the cursor has a pressed animation (bCanPress: 1 for the six
//  animated cursors, 0 for Buysell), starts it. Already pressed is a no-op apart from bReturn = 0, so holding the button does not restart the
//  animation; the position and the tick are cached before that test. lResult is not written, unlike the neighbouring handlers.
void __stdcall CURSOR_UpdateCursorPosInGame(SMSGHANDLER_PARAMS *pMsg)
{
    gnMouseX = pMsg->nXpos;
    gnMouseY = pMsg->nYpos;
    gnCursorLastPositionUpdateTickCount = GetTickCount();

    if (gnCursorState != CURSORSTATE_ButtonPressed)
    {
        if (gCursorDescs[gnCursorType].bCanPress)
        {
            gnCursorState = CURSORSTATE_ButtonPressed;
            gnCursorType = CURSOR_PPress;
            gnCursorFrame = 0;
        }
    }

    pMsg->bReturn = 0;
}


//1.10f: D2Client.0x6FB57500
//  The button came up. Caches the position, restarts the idle timer and, if the cursor was pressed, puts it back to the resting rotate.
//  It does not consult bCanPress: releasing always resets, so a cursor whose bCanPress is 0 can never be in the state this leaves.
void __stdcall CURSOR_OnPanelMouseUp(SMSGHANDLER_PARAMS *pMsg)
{
    gnMouseX = pMsg->nXpos;
    gnMouseY = pMsg->nYpos;
    gnCursorLastPositionUpdateTickCount = GetTickCount();

    if (gnCursorState == CURSORSTATE_ButtonPressed)
    {
        gnCursorType = CURSOR_PRotate;
        gnCursorState = CURSORSTATE_Default;
        gnCursorFrame = 0;
    }

    pMsg->bReturn = 0;
}


//1.10f: D2Client.0x6FB57560
//  Puts an item up as the buy/sell cursor (server packet 0x3F): the held item, CURSOR_Buysell, the frame straight from the packet, and state
//  CURSORSTATE_ItemSelected. void: eax is left at 6 (the type and the state are both 6), which nothing reads. nFrame is the packet's cursor
//  byte and is stored raw in gnCursorFrame, as in CURSOR_SetCursorBuySell.
void __fastcall CURSOR_SetBuysellCursor(D2UnitStrc *pUnit, int32_t nFrame)
{
    gpCursorHeldUnit = pUnit;
    gnCursorType = CURSOR_Buysell;
    gnCursorFrame = nFrame;
    gnCursorState = CURSORSTATE_ItemSelected;
}


//1.10f: D2Client.0x6FB57580
//  Shows one of the buy/sell cursors: drops what was on the cursor, switches to CURSOR_Buysell and stores nFrame in gnCursorFrame raw.
//  Here gnCursorFrame is not 8.8 fixed point (CURSOR_DrawCursorBuySell reads it raw), so the one global carries two encodings, selected by
//  gnCursorType; the values stored are D2C_CursorBuySellFrame. The state (bAltFrame != 0) + 7 is the buy/sell cursor's two variants.
void __fastcall CURSOR_SetCursorBuySell(int32_t nFrame, int32_t bAltFrame)
{
    gpCursorHeldUnit = NULL;
    gnCursorType = CURSOR_Buysell;
    gnCursorFrame = nFrame;
    gnCursorState = (D2C_CursorState)((bAltFrame != 0) + 7);
}


//1.10f: D2Client.0x6FB575B0
//  Puts an item on the cursor, or takes it off with NULL. Both go through CURSOR_PRotate with frame 0; the state is HandOpened when something
//  is held and Default when nothing is. The return value is a leftover (eax is zeroed first and nothing assigns it).
int32_t __fastcall CURSOR_SetHeldUnit(D2UnitStrc *pUnit)
{
    int32_t nLeftInEax;

    nLeftInEax = 0;

    gpCursorHeldUnit = pUnit;
    gnCursorFrame = 0;
    gnCursorType = CURSOR_PRotate;
    gnCursorState = CURSORSTATE_Default;

    if (pUnit)
        gnCursorState = CURSORSTATE_HandOpened;

    return nLeftInEax;
}


//1.10f: D2Client.0x6FB575E0
//  Returns the item on the cursor.
D2UnitStrc *__cdecl CURSOR_GetHeldUnit()
{
    return gpCursorHeldUnit;
}


//1.10f: D2Client.0x6FB575F0
//  Loads the seven cursors and hooks the three messages. The loop walks gCursorDescs, stepping the szName of each of its seven descriptors;
//  the bound is an address in the original, NUM_CURSORS the source-level reading.
//  The store to gwCursorWriteOnly (ax = 0, at 0x6FB57673) has no reader in the module; it is kept because the original makes it.
//  The mouse starts at the centre of the screen and the idle timer starts now, so the cursor does not at once fall to the idle hand.
void __fastcall CURSOR_Init(int32_t nClickOffsetX)
{
    CHAR szPath[80];
    int32_t nCursor;

    gnCursorClickOffsetX = nClickOffsetX;

    WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gCursorMessageHandlers, 3);

    for (nCursor = 0; nCursor < NUM_CURSORS; ++nCursor)
    {
        wsprintfA(szPath, "%s\\UI\\CURSOR\\%s", aDataGlobal, gCursorDescs[nCursor].szName);
        gpCursorImages[nCursor] = ARCHIVE_CellFileLoad(szPath, 0);
    }

    gbCursorsLoaded = 1;
    gnCursorState = CURSORSTATE_Default;
    gnCursorType = CURSOR_PRotate;
    gnCursorFrame = 0;
    gwCursorWriteOnly = 0;
    gnCursorLastPositionUpdateTickCount = GetTickCount();
    gnMouseX = gnScreenWidth / 2;
    gnMouseY = gnScreenHeight / 2;
}


//1.10f: D2Client.0x6FB576B0
//  Drops the cursors and unhooks; the mirror of CURSOR_Init. gbCursorsLoaded is cleared first, before anything is freed, so a draw racing the
//  teardown finds the flag already down. The loop bound is &gnCursorType, the end of the seven gpCursorImages pointers. Each slot is
//  null-checked before ARCHIVE_CellFileFree and nulled after, so calling this twice is harmless.
void __cdecl CURSOR_UnloadCursors()
{
    int32_t nCursor;

    WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gCursorMessageHandlers, 3);

    gbCursorsLoaded = 0;

    for (nCursor = 0; nCursor < NUM_CURSORS; ++nCursor)
    {
        if (gpCursorImages[nCursor])
        {
            ARCHIVE_CellFileFree(gpCursorImages[nCursor]);
            gpCursorImages[nCursor] = 0;
        }
    }
}


//1.10f: D2Client.0x6FB57700
//  Draws the cursor and advances its animation. Reached through gCursorDescs[type].pDrawFunction for six of the seven cursor types
//  (CURSOR_DrawCursorDefault tail-jumps here). __fastcall like CURSOR_DrawCursorBuySell, the convention of pDrawFunction.
//  gnCursorFrame is 8.8 fixed point: the cel index is gnCursorFrame >> 8 and nFrameAdvance is added in the same units.
//  The clamp is asymmetric: y goes to [0, gnScreenHeight - 1], x to [gnCursorClickOffsetX, gnScreenWidth - gnCursorClickOffsetX - 1],
//  because x has had gnCursorClickOffsetX added before the clamp.
//  On CURSORSTATE_Default the advance is rolled with SEED_RollRandomNumber(&pPlayer->pSeed): when (roll & 0x3F) < 0x10 (16 of 64) the frame
//  advances by 32, otherwise not. With CUNIT_GetPlayer() NULL the roll is skipped and the cursor freezes. Every other state advances by
//  gCursorDescs[type].nFrameAdvance unconditionally.
//  The original inlines CURSOR_GetCursorFrameDuration at the frame-overflow test and also calls it three statements later; both are written as
//  calls. After the loop test fails and the cursor does not loop, the state must be 2 or 5: anything else hits FOG_DisplayAssert("0", ..., 487)
//  and exits.
//  The gotos are the original's graph: idle_check is entered by falling out of the state machine and through idle_guard, which tests the
//  condition the first entry already knows to be true.
void __fastcall CURSOR_DrawCurrent()
{
    D2GfxDataStrc tCelContext;
    D2UnitStrc *pPlayer;
    int32_t nXPos;
    int32_t nYPos;
    uint8_t nSeedByte;
    int32_t nCursorType;
    int32_t nFrameOwner;

    nXPos = gnCursorClickOffsetX + gnMouseX;
    nYPos = gnMouseY;

    if (nYPos < 0)
        nYPos = 0;
    else if (nYPos > gnScreenHeight - 1)
        nYPos = gnScreenHeight - 1;

    if (nXPos < gnCursorClickOffsetX)
        nXPos = gnCursorClickOffsetX;
    else if (nXPos > gnScreenWidth - gnCursorClickOffsetX - 1)
        nXPos = gnScreenWidth - gnCursorClickOffsetX - 1;

    memset(&tCelContext, 0, sizeof(tCelContext));
    tCelContext.pCellFile = gpCursorImages[gnCursorType];
    tCelContext.nDirection = 0;
    tCelContext.nFrame = gnCursorFrame >> 8;

    D2GFX_TEXTURE_CelDraw(&tCelContext, nXPos, nYPos, 0xFFFFFFFF, 5, NULL);

    if (gnCursors != NUM_CURSORS)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "NUM_CURSORS == nCursorTableSize", (uint32_t)(UINT_PTR)szSourceFile, 132);
        exit(-1);
    }

    nCursorType = gnCursorType;

    //  SIGNED, as the bytes compare it: cmp ecx, esi / jl (0x6FB577B4..0x6FB577BC, esi = 7).
    if (nCursorType >= (int32_t)NUM_CURSORS)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "Type < NUM_CURSORS", (uint32_t)(UINT_PTR)szSourceFile, 133);
        exit(-1);
    }

    if (!gCursorDescs[gnCursorType].bAnimated)
        goto idle_guard;

    if (gnCursorState == CURSORSTATE_HandClosing)
    {
        gnCursorFrame -= 64;

        if (gnCursorFrame >= 0)
            return;

        if (gCursorDescs[gnCursorType].bLoops)
            goto wrap_frame;

        goto to_default;
    }

    pPlayer = CUNIT_GetPlayer();

    if (!pPlayer)
        goto idle_guard;

    if (gnCursorState == CURSORSTATE_Default)
    {
        nSeedByte = (uint8_t)SEED_RollRandomNumber(&pPlayer->pSeed);
        nFrameOwner = gnCursorType;

        if ((nSeedByte & 0x3F) >= 0x10)
            goto check_overflow;

        gnCursorFrame = gnCursorFrame + 32;
    }
    else
    {
        nFrameOwner = gnCursorType;
        gnCursorFrame = (int32_t)gCursorDescs[gnCursorType].nFrameAdvance + gnCursorFrame;
    }

check_overflow:
    //  The original inlines CURSOR_GetCursorFrameDuration here; written as the call.
    if (gnCursorFrame < CURSOR_GetCursorFrameDuration(nFrameOwner))
        goto idle_guard;

    if (CURSOR_GetCursorLoops(nFrameOwner))
    {
        nCursorType = gnCursorType;
wrap_frame:
        gnCursorFrame -= CURSOR_GetCursorFrameDuration(nCursorType);
        goto idle_guard;
    }

    if (gnCursorState == CURSORSTATE_HandOpening)
    {
        gnCursorType = CURSOR_ORotate;
        gnCursorFrame = 0;
        gnCursorState = CURSORSTATE_HandOpened;
        return;
    }

    if (gnCursorState != CURSORSTATE_ButtonPressed)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "0", (uint32_t)(UINT_PTR)szSourceFile, 487);
        exit(-1);
    }

to_default:
    gnCursorState = CURSORSTATE_Default;
    gnCursorType = CURSOR_PRotate;
    gnCursorFrame = 0;
    goto idle_check;

idle_guard:
    if (gnCursorState != CURSORSTATE_Default)
        return;

idle_check:
    //  Five seconds after the last move the cursor goes back to the opening hand.
    //  BUG: the compare is unsigned (add ecx, 1388h / cmp ecx, eax / jnb at 0x6FB578FA) and not wrap-safe: within 5000 ms of GetTickCount's
    //  49.7-day rollover the sum wraps to a small number and the test reads true with no time elapsed. LATENT: the cursor drops to the opening
    //  hand for that window once every 49.7 days of uptime, and nothing else depends on it. The wrap-safe spelling is now - stored > 5000.
    if (gnCursorLastPositionUpdateTickCount + 5000 < GetTickCount())
    {
        gnCursorFrame = 0;
        gnCursorState = CURSORSTATE_HandOpening;
        gnCursorType = CURSOR_OHand;
    }
}


//1.10f: D2Client.0x6FB579A0
//  Does this cursor loop? Returns gCursorDescs[nCursorType].bLoops, behind two asserts (NUM_CURSORS == nCursorTableSize, and
//  Type < NUM_CURSORS), both exit(-1). The index arithmetic 7 * nCursorType is in dwords: 28 bytes, sizeof(CursorDescStrc).
//  The bound test is signed (cmp ecx, 7 / jl at 0x6FB579C7).
int32_t __fastcall CURSOR_GetCursorLoops(int32_t nCursorType)
{
    if (gnCursors != NUM_CURSORS)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "NUM_CURSORS == nCursorTableSize", (uint32_t)(UINT_PTR)szSourceFile, 143);
        exit(-1);
    }

    if (nCursorType >= (int32_t)NUM_CURSORS)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "Type < NUM_CURSORS", (uint32_t)(UINT_PTR)szSourceFile, 144);
        exit(-1);
    }

    return (int32_t)gCursorDescs[nCursorType].bLoops;
}


//1.10f: D2Client.0x6FB57A00
//  Returns how long one frame of this cursor lasts: gCursorDescs[nCursorType].nFrameCount << 8, in 1/256ths of a frame count, behind the same
//  two asserts as CURSOR_GetCursorLoops. The shift is fixed point: CURSOR_OnMouseMove subtracts a whole 256 from the result to start the
//  closing animation one frame in.
int32_t __fastcall CURSOR_GetCursorFrameDuration(int32_t nCursorType)
{
    if (gnCursors != NUM_CURSORS)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "NUM_CURSORS == nCursorTableSize", (uint32_t)(UINT_PTR)szSourceFile, 154);
        exit(-1);
    }

    if (nCursorType >= (int32_t)NUM_CURSORS)        //  SIGNED: cmp ecx, 7 / jl at 0x6FB57A27
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "Type < NUM_CURSORS", (uint32_t)(UINT_PTR)szSourceFile, 155);
        exit(-1);
    }

    return (int32_t)(gCursorDescs[nCursorType].nFrameCount << 8);
}


//1.10f: D2Client.0x6FB57A60
//  Draws the buy/sell cursor. Row 6 of gCursorDescs points its pDrawFunction here; every other cursor type goes to CURSOR_DrawCurrent.
//  __fastcall like CURSOR_DrawCurrent, the convention of pDrawFunction. The frame is raw (pData.field_08 = gnCursorFrame, no shift): what
//  CURSOR_SetCursorBuySell stored, a D2C_CursorBuySellFrame, so this cursor does not animate. x is offset by gnCursorClickOffsetX as
//  everywhere, y by a literal 33 rather than by half the cel height.
void __fastcall CURSOR_DrawCursorBuySell()
{
    D2GfxDataStrc tCelContext;

    memset(&tCelContext, 0, sizeof(tCelContext));
    tCelContext.pCellFile = gpCursorImages[CURSOR_Buysell];
    tCelContext.nFrame = gnCursorFrame;
    tCelContext.nDirection = 0;

    D2GFX_TEXTURE_CelDraw(&tCelContext, gnMouseX + gnCursorClickOffsetX, gnMouseY + 33, 0xFFFFFFFF, 5, NULL);
}

//1.10f: D2Client.0x6FB57AC0
//  Draws the cursor, or the item's own graphic when an item is on it, centred on the mouse and offset by gnCursorClickOffsetX horizontally.
//  Gated by gbCursorsLoaded (nothing is drawn before CURSOR_Init has run), by an item on the cursor (gpCursorHeldUnit, dwUnitType == UNIT_ITEM)
//  and by gnCursorState <= 5. Both state compares are signed (jge), so the test casts to int32_t.
//  Without an item, control leaves by a tail jump (jmp dword ptr [ecx*4 + 6FB96B6Ch] at 0x6FB57BAD, after the stack is unwound at 0x6FB57BAA)
//  to gCursorDescs[gnCursorType].pDrawFunction(). The table has seven rows: rows 0..5 CURSOR_DrawCurrent, row 6 CURSOR_DrawCursorBuySell;
//  what follows is not a row. The two cel calls are in the binary's order, D2CMP_CelGetHeight first, then D2CMP_CelGetWidth.
//  BUG: gnCursorType is not range-checked here, unlike CURSOR_GetCursorLoops and CURSOR_GetCursorFrameDuration: a value of 7 or more jumps
//  through whatever follows the table (0xFFFFFFFF in the next row). NOT ESTABLISHED: this runs every frame and the game does not fall over,
//  so no writer is seen to reach 7, but that is an observation and not a proof. The writers are CURSOR_Init, CURSOR_SetCursorBuySell,
//  CURSOR_SetHeldUnit, CURSOR_OnMouseMove, CURSOR_UpdateCursorPosInGame and CURSOR_OnPanelMouseUp.
//  BUG: the state test is two compares and the second is dead: cmp eax, 6 / jge (0x6FB57AEF), then cmp eax, 8 / jge (0x6FB57AF8), which
//  cannot be taken because eax < 6 there. LATENT: it costs one compare per frame. Not written here: C compiles it away.
void __cdecl CURSOR_DrawCursorDefault()
{
    D2GfxDataStrc tCelContext;
    int32_t nClass;
    uint32_t nCellHeight;
    uint32_t nCellWidth;
    int32_t nDrawY;

    if (!gbCursorsLoaded)
        return;

    if (gpCursorHeldUnit && gpCursorHeldUnit->dwUnitType == UNIT_ITEM && (int32_t)gnCursorState <= 5)
    {
        memset(&tCelContext, 0, sizeof(tCelContext));

        if (GFXUTIL_FillGfxData(&tCelContext, gpCursorHeldUnit, 0, 0, 0, &nClass, 1, -1) && D2CMP_SpriteValidate(&tCelContext, 0, 1))
        {
            if (!tCelContext.pCurrentCell)
            {
                FOG_DisplayAssert((uint32_t)(UINT_PTR) "tCelContext.hCel", (uint32_t)(UINT_PTR)szSourceFile, 629);
                exit(-1);
            }

            nCellHeight = (uint32_t)D2CMP_CelGetHeight(tCelContext.pCurrentCell);
            nDrawY = gnMouseY - (int32_t)(nCellHeight >> 1);

            nCellWidth = (uint32_t)D2CMP_CelGetWidth(tCelContext.pCurrentCell);

            GFX_DrawInventoryItem(gpCursorHeldUnit, gnMouseX + gnCursorClickOffsetX - (int32_t)(nCellWidth >> 1), nDrawY);
        }
    }
    else
    {
        gCursorDescs[gnCursorType].pDrawFunction();
    }
}


//1.10f: D2Client.0x6FB57BC0
//  Returns the mouse X (one load). No arguments, so the convention is not observable; the same holds for CURSOR_GetMouseY.
int32_t __fastcall CURSOR_GetMouseX()
{
    return gnMouseX;
}


//1.10f: D2Client.0x6FB57BD0
//  Returns the mouse Y.
int32_t __fastcall CURSOR_GetMouseY()
{
    return gnMouseY;
}


//1.10f: D2Client.0x6FB57BE0
//  Puts the pointer somewhere, in client coordinates. Full screen: passed to Windows as is. Windowed: lifted into screen space with
//  x + rect.left + GetSystemMetrics(SM_CXFIXEDFRAME) and y + rect.top + GetSystemMetrics(SM_CYCAPTION); the rect is the outer window
//  rectangle, so the border and the title bar are added by hand rather than by ClientToScreen (the game's window is not resizable).
//  Both asserts (:775, :776) are on the input only: a negative x or y halts the client, an x past the right edge does not.
//  The cached gnMouseY is written first, then gnMouseX, both with the client coordinates.
void __fastcall CURSOR_SetCursorPos(int32_t nX, int32_t nY)
{
    RECT rcWindow;
    int32_t nScreenY;

    if (nX < 0)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "nX >= 0", (uint32_t)(UINT_PTR)szSourceFile, 775);
        exit(-1);
    }

    if (nY < 0)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "nY >= 0", (uint32_t)(UINT_PTR)szSourceFile, 776);
        exit(-1);
    }

    if (D2GFX_WINDOW_IsFullScreen())
    {
        SetCursorPos(nX, nY);
    }
    else
    {
        GetWindowRect(D2GFX_WINDOW_GetWindow(), &rcWindow);
        nScreenY = nY + rcWindow.top + GetSystemMetrics(SM_CYCAPTION);
        SetCursorPos(nX + rcWindow.left + GetSystemMetrics(SM_CXFIXEDFRAME), nScreenY);
    }

    gnMouseY = nY;
    gnMouseX = nX;
}


//1.10f: D2Client.0x6FB57CC0
//  Returns the cursor state (one load). No arguments, so the convention is not observable.
int32_t __fastcall CURSOR_GetCursorState()
{
    return (int32_t)gnCursorState;
}

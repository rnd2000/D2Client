#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "WNDPROC.h"
#include "WINMAIN.h"
#include "CoreArchive.h"
#include "Game.h"

//  Game.cpp -- the game application mode of D2Client.dll 1.10: the per-game initialisation (GAME_InitGame), the frame update and draw
//  (GAME_UpdateFrame, GAME_DrawGameScene), the game-window message handlers, the Startup, Game and CloseWindow application modes, the
//  launch-parameter appliers, the client's state accessors and mouse-handler pair, and the container (Storm's hash table over SGAMEDATA) behind
//  gGameData. The original source file is GAME\Game.cpp (the asserts in this unit name it, e.g. the operand at 0x6FAAB971 in GAME_AppModeGame).
//  Address range 0x6FAA8970 (GAME_GameDataDestructTable) .. GAME_ListNodeToLink at 0x6FAAD050.

//  The original's source path, as GAME_AppModeGame hands it to FOG_Free (0x6FB75BDC).
static const char szSourceFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\GAME\\Game.cpp";

//  Handler-table extents, from the binary's own literals.
static const size_t nGameWindowHandlers = 10;
static const size_t nBetaSlidesHandlers = 7;
static const size_t nVanquishedHandlers = 9;

//  gGameData is Storm's TSHashTable over SGAMEDATA records. The compiler-emitted initialiser and atexit destructor of the original's global are not
//  rewritten: ours builds gGameData through sgGameDataLifetime (below) and tears it down with GAME_GameDataDestructTable, and the list helpers
//  only that initialiser reaches are not rewritten either.
//
//  Storm's allocation tags for the container's two record kinds: the RTTI type-descriptor names at 0x6FB75B28 and 0x6FB75B40, which the template
//  hands to SMemFree as its "file".
static const char szSGameDataTag[] = ".?AUSGAMEDATA@@";
static const char szSGameDataChunkTag[] = ".?AV?$TSHashObjectChunk@USGAMEDATA@@VHASHKEY_NONE@@@@";

//  The offsets the destructor below reads.
static_assert(offsetof(SGameDataTableStrc, tNodeList) == 0x04,
              "`lea ebx, [edi+4]` at 0x6FAA8A80");
static_assert(offsetof(SGameDataTableStrc, nFullnessIndicator) == 0x10,
              "`mov [edi+10h], ebx` at 0x6FAA8A7D");
static_assert(offsetof(SGameDataTableStrc, tBuckets) == 0x14,
              "`lea ecx, [edi+14h]` at 0x6FAA8AC3");
static_assert(offsetof(SGameDataTableStrc, tReuseList) == 0x28,
              "`lea ebp, [edi+28h]` at 0x6FAA8A30");
static_assert(offsetof(SGameDataTableStrc, tChunkList) == 0x38,
              "`lea ebp, [edi+38h]` at 0x6FAA89E9");
static_assert(offsetof(SGameDataTableStrc, tLock) == 0x50,
              "`lea eax, [edi+50h]` at 0x6FAA8992");

//1.10f: D2Client.0x6FAA8970
//  The destructor of gGameData's table (Storm's TSHashTable over SGAMEDATA, the RTTI names above): the lock deleted, every chunk's record array
//  freed and the chunk returned to Storm, every record destroyed and returned, the node, chunk and record lists unlinked, the buckets' storage
//  freed. Run on our gGameData at our DLL's exit by sgGameDataLifetime, and by GAME_SGameDataVecDelDtor2 (the live table's deleting destructor).
//  The original inlines GAME_SGameDataDestructor here.
//  A TSLink is Storm's {m_prevlink, m_next}: pPrevLink is the previous link, nNext the next object when positive and ~(the terminator's link)
//  when not, so a list's head is its terminator's nNext and a walk ends when that is <= 0.
//  `__thiscall` cannot be written on a free function; with `this` its only argument, `__fastcall` has the same ABI. The original's fs:0 exception
//  frame and unwind states are not reproduced: nothing here can throw.
void __fastcall GAME_GameDataDestructTable(SGameDataTableStrc *pTable)
{
    DeleteCriticalSection(&pTable->tLock);

    GAME_SGameDataDestructor(pTable);
}

//  gGameData's construction -- ours, not a function of the original. Our compiler's initialisation of sgGameDataLifetime builds the table in the
//  state the original's constructor chain leaves it in, with the same calls outside the module (Storm's allocation, then InitializeCriticalSection),
//  and its destructor is GAME_GameDataDestructTable. The state:
//
//      +0x00 pVtbl               gVtblSGameData2 (the last of four vtable stores: gVtblSGameData0, 1, 3 and 2)
//      +0x04 tNodeList           link offset 12, empty
//      +0x10 nFullnessIndicator  0
//      +0x14 tBuckets            four 12-byte lists from Storm, each with link offset 4 and empty; nCapacity and nCount 4, nGrowQuantum 0
//      +0x24 nBucketMask         3
//      +0x28 tReuseList          link offset 4, empty
//      +0x34 nChunkSize          16
//      +0x38 tChunkList          link offset 16, empty
//      +0x44 unk0x44             never written: zero, as the object is at load
//      +0x48 dwCookie            ~this & 0x0FFFFFFF
//      +0x4C unk0x4C             0
//      +0x50 tLock               InitializeCriticalSection
//
//  An empty list's terminator links to itself: pPrevLink its own address, nNext that address complemented. The link offsets are written once, as
//  their end state; nothing reads the object before the constructor returns.
//
//  The bucket array is one STORM_SMemReAlloc(NULL, 48, the tag, -2, 0x10) (four 12-byte lists) and, only if that answers NULL,
//  STORM_SMemAlloc(48, the tag, -2, 0). Storm 1.10's SMemReAlloc hands a NULL pointer straight to SMemAlloc with the same flags, so the first
//  call is the one that allocates.
//  DIFFERENCE: if both allocations answer NULL the original faults on a store through NULL in its list constructor (0x6FAAD217); ours tests the
//  two answers as the original does but does not reproduce that store, which is undefined in C++. LATENT: whether Storm's allocator can answer
//  NULL at our load was not looked for.
//
//  The original constructor's exception frame and unwind states are not reproduced: nothing here throws.
static_assert(offsetof(SGameDataTableStrc, nChunkSize) == 0x34,
              "`mov dword ptr [esi+34h], 10h` at 0x6FAAC3FF");
static_assert(offsetof(SGameDataTableStrc, dwCookie) == 0x48,
              "`mov [esi+48h], eax` at 0x6FAAC41E");
static_assert(offsetof(SGameDataTableStrc, unk0x4C) == 0x4C,
              "`mov dword ptr [esi+4Ch], 0` at 0x6FAAC421");

static void GAME_GameDataInitEmptyList(TSExplicitList *pList, int32_t nLinkOffset)
{
    pList->nLinkOffset = nLinkOffset;
    pList->tSentinel.pPrevLink = &pList->tSentinel;
    pList->tSentinel.nNext = ~(int32_t)&pList->tSentinel;
}

static void GAME_GameDataConstruct(SGameDataTableStrc *pTable)
{
    TSExplicitList *pLists;
    uint32_t iBucket;

    GAME_GameDataInitEmptyList(&pTable->tNodeList, 12);
    pTable->nFullnessIndicator = 0;
    pTable->nBucketMask = 3;

    pTable->tBuckets.nGrowQuantum = 0;
    pTable->tBuckets.nCapacity = 4;
    pLists = (TSExplicitList *)STORM_SMemReAlloc(NULL, 4 * sizeof(TSExplicitList), szTSExplicitListSGameDataRawName, -2, 0x10);
    if (!pLists)
        pLists = (TSExplicitList *)STORM_SMemAlloc(4 * sizeof(TSExplicitList), szTSExplicitListSGameDataRawName, -2, 0);
    pTable->tBuckets.pLists = pLists;
    pTable->tBuckets.nCount = 4;
    for (iBucket = 0; iBucket <= pTable->nBucketMask; ++iBucket)
        GAME_GameDataInitEmptyList(&pTable->tBuckets.pLists[iBucket], 4);

    GAME_GameDataInitEmptyList(&pTable->tReuseList, 4);
    pTable->nChunkSize = 16;
    GAME_GameDataInitEmptyList(&pTable->tChunkList, 16);
    pTable->dwCookie = ~(uint32_t)pTable & 0x0FFFFFFF;
    pTable->unk0x4C = 0;
    InitializeCriticalSection(&pTable->tLock);
    pTable->pVtbl = &gVtblSGameData2;
}

//  The object of ours whose initialisation builds gGameData and whose destruction tears it down (gGameData is zero when it runs).
struct GameDataLifetime
{
    GameDataLifetime() { GAME_GameDataConstruct(&gGameData); }
    ~GameDataLifetime() { GAME_GameDataDestructTable(&gGameData); }
};
static GameDataLifetime sgGameDataLifetime;

//1.10f: D2Client.0x6FAA8B10
//  Stores the Run key state in gbRunKeyHeld.
void __fastcall GAME_SetRunKeyHeld(int32_t bHeld)
{
    gbRunKeyHeld = bHeld;
}

//1.10f: D2Client.0x6FAA8B20
//  Stores gbRunToggled, the run/walk lock.
void __fastcall GAME_SetRunToggled(int32_t bRun)
{
    gbRunToggled = bRun;
}

//1.10f: D2Client.0x6FAA8B30
//  Returns gbRunToggled.
int32_t __cdecl GAME_GetRunToggled()
{
    return gbRunToggled;
}

//1.10f: D2Client.0x6FAA8B40
//  Stores gbStandStillKeyHeld.
void __fastcall GAME_SetStandStillKeyHeld(int32_t bHeld)
{
    gbStandStillKeyHeld = bHeld;
}

//1.10f: D2Client.0x6FAA8B50
//  Bit 3 when the Run key is held or the run/walk lock is on, bit 2 when Stand Still is held. The original sets bit 3 by `mov eax, 8` and then again
//  by `or al, 8`; both are written.
uint8_t __cdecl GAME_GetMovementFlags()
{
    uint8_t nFlags = 0;

    if (gbRunKeyHeld)
        nFlags = 8;
    if (gbRunToggled)
        nFlags |= 8;
    if (gbStandStillKeyHeld)
        nFlags |= 4;
    return nFlags;
}

//1.10f: D2Client.0x6FAA8B80
//  Sets gbSuppressHeldClick.
void __cdecl GAME_SuppressHeldClick()
{
    gbSuppressHeldClick = 1;
}

//1.10f: D2Client.0x6FAA8B90
//  Returns gnLeftClickModifierFlags.
int32_t __cdecl GAME_GetLeftClickModifierFlags()
{
    return gnLeftClickModifierFlags;
}

//1.10f: D2Client.0x6FAA8BA0
//  Returns gnRightClickModifierFlags.
int32_t __cdecl GAME_GetRightClickModifierFlags()
{
    return gnRightClickModifierFlags;
}

//1.10f: D2Client.0x6FAA8BB0
//  Returns gbLeftButtonDown.
uint32_t __cdecl GAME_GetLeftButtonDown()
{
    return gbLeftButtonDown;
}

//  ---- the game window's handlers ----
//
//  Thirteen message handlers, all `void __stdcall(SMSGHANDLER_PARAMS *)`, reached only through the handler tables GAME_AppModeGame and the mouse pair
//  register. The shared ending, `pMsg->bReturn = 1; pMsg->lResult = 0;` and STORM_SMsgBreakHandlerChain, is the "consumed" idiom; where a handler
//  does less, its note says so.

//1.10f: D2Client.0x6FAA8BC0
//  WM_CLOSE. The Run key is let go first. Until gbDisconnecting is set, the close is refused: the leave request goes to the server
//  (MSG_Send_D2CLTSYS_ENDGAME, id 0x68), gbLeaveGameSent is raised, and the message reports handled (bReturn = 1, lResult = 0) without breaking
//  the chain. Once disconnecting, the UI and the reference monsters are reset and the message goes on with bReturn = 0.
void __stdcall GAME_WindowOnClose(SMSGHANDLER_PARAMS *pMsg)
{
    gbRunKeyHeld = 0;

    if (gbDisconnecting)
    {
        UI_ResetUiSubsystems();
        SELECT_ResetRefMonsters();
        pMsg->bReturn = 0;
    }
    else
    {
        MSG_Send_D2CLTSYS_ENDGAME();
        gbLeaveGameSent = 1;
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
    }
}

//1.10f: D2Client.0x6FAA8C10
//  WM_LBUTTONDOWN in the world, the layer under the panels. The modifier word is set to 4, the held-click suppression cleared and the selection
//  unlocked; in a game the button is marked down and, for a player unit, PLAYER_OnMouseClick's answer replaces the mark. The message position is
//  the two words of lParam, zero-extended. The movement flags are GAME_GetMovementFlags' tests done on a dword.
void __stdcall GAME_WindowOnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    D2UnitStrc *pPlayer;
    int32_t nFlags;

    gnLeftClickModifierFlags = 4;
    gbSuppressHeldClick = 0;
    SELECT_UnlockSelection();

    if (gbInGame)
    {
        gbLeftButtonDown = 1;
        pPlayer = CUNIT_GetPlayer();

        nFlags = 0;
        if (gbRunKeyHeld)
            nFlags = 8;
        if (gbRunToggled)
            nFlags |= 8;
        if (gbStandStillKeyHeld)
            nFlags |= 4;

        if (pPlayer && pPlayer->dwUnitType == UNIT_PLAYER)
        {
            const int32_t bDown = PLAYER_OnMouseClick(0, pMsg->nXpos, pMsg->nYpos, nFlags);
            gbLeftButtonDown = bDown;
        }

        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
    }
}

//1.10f: D2Client.0x6FAA8CB0
//  Returns whether gbLeftClickIssuedThisFrame is set.
BOOL __cdecl GAME_GetLeftClickIssuedThisFrame()
{
    return gbLeftClickIssuedThisFrame != 0;
}

//1.10f: D2Client.0x6FAA8CC0
//  WM_LBUTTONUP in the world. The modifier word becomes 16; the button, this frame's issued click and the held-click suppression are cleared and
//  the selection unlocked; in a game, for a player unit, the release goes to PLAYER_OnMouseClick at the cursor's position, not the message's, and its
//  answer is dropped. The message is not consumed: no bReturn, no break of the chain. The cursor is read Y first and X second (the order the
//  arguments are pushed), hence two statements. The original inlines GAME_GetMovementFlags here.
void __stdcall GAME_WindowOnLButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    D2UnitStrc *pPlayer;
    int32_t nFlags;
    int32_t nX;
    int32_t nY;

    gnLeftClickModifierFlags = 16;
    gbLeftButtonDown = 0;
    gbLeftClickIssuedThisFrame = 0;
    gbSuppressHeldClick = 0;
    SELECT_UnlockSelection();

    if (!gbInGame)
        return;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer || pPlayer->dwUnitType != UNIT_PLAYER)
        return;

    nFlags = GAME_GetMovementFlags();

    nY = CURSOR_GetMouseY();
    nX = CURSOR_GetMouseX();
    PLAYER_OnMouseClick(2, nX, nY, nFlags);
}

//1.10f: D2Client.0x6FAA8D50
//  WM_RBUTTONDOWN in the world, with two refusals first. Nothing happens in a game with both panels shifted in (SCROLL_GetUiShiftScreen() == 3) or
//  outside one. With shift 2 a click in the left half (0 < x < width / 2, 0 < y < height - 47) and with shift 1 a click in the right half
//  (width / 2 < x < width, 0 < y < gnViewportBottom) is swallowed with bReturn = 0 and the chain broken; the shift is asked again for each test.
//  Otherwise as WM_LBUTTONDOWN, with click event 3 and the right-hand globals. width / 2 is a signed division and every bound a signed compare.
void __stdcall GAME_WindowOnRButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    BOOL bInLeftHalf;
    BOOL bInRightHalf;
    D2UnitStrc *pPlayer;
    int32_t nFlags;
    int32_t nX;
    int32_t nY;

    bInRightHalf = FALSE;
    gnRightClickModifierFlags = 4;
    gbSuppressHeldClick = 0;
    SELECT_UnlockSelection();

    if (!gbInGame || SCROLL_GetUiShiftScreen() == 3)
        return;

    nX = pMsg->nXpos;
    nY = pMsg->nYpos;
    bInLeftHalf = FALSE;
    if (SCROLL_GetUiShiftScreen() == 2 && nX > 0 && nX < gnScreenWidth / 2 && nY > 0 && nY < gnScreenHeight - 47)
    {
        bInLeftHalf = TRUE;
    }
    if (SCROLL_GetUiShiftScreen() == 1 && nX > gnScreenWidth / 2 && nX < gnScreenWidth && nY > 0 && nY < gnViewportBottom)
    {
        bInRightHalf = TRUE;
    }

    if (bInLeftHalf || bInRightHalf)
    {
        pMsg->bReturn = 0;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        return;
    }

    gbRightButtonDown = 1;
    pPlayer = CUNIT_GetPlayer();

    nFlags = 0;
    if (gbRunKeyHeld)
        nFlags = 8;
    if (gbRunToggled)
        nFlags |= 8;
    if (gbStandStillKeyHeld)
        nFlags |= 4;

    if (pPlayer && pPlayer->dwUnitType == UNIT_PLAYER)
    {
        const int32_t bDown = PLAYER_OnMouseClick(3, pMsg->nXpos, pMsg->nYpos, nFlags);
        gbRightButtonDown = bDown;
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAA8E90
//  WM_RBUTTONUP in the world. The right-hand globals are released as WM_LBUTTONUP releases the left-hand ones, but this frame's issued right
//  click is SET, not cleared. In a game, for a player unit, PLAYER_OnMouseClick with click event 5 at the message's position, its answer stored in
//  gbRightButtonDown; consumed in a game either way.
//  BUG: the flags passed are not the movement flags the other three clicks pass but whatever eax holds: the player pointer CUNIT_GetPlayer returned
//  with its low half replaced by LOWORD(wParam), the MK_* keys (0x6FAA8ED9 mov ax, [esi+8]; 0x6FAA8EE3 push eax). INERT: PLAYER_OnMouseClick reads
//  the passed value once, as a byte tested for 8 and 4, so the pointer's half is never read. Written as the bytes compute it: the pointer's high
//  half, the word's low half.
void __stdcall GAME_WindowOnRButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    D2UnitStrc *pPlayer;
    int32_t nFlags;

    gnRightClickModifierFlags = 16;
    gbRightButtonDown = 0;
    gbRightClickIssuedThisFrame = 1;
    gbSuppressHeldClick = 0;
    SELECT_UnlockSelection();

    if (!gbInGame)
        return;

    pPlayer = CUNIT_GetPlayer();
    if (pPlayer && pPlayer->dwUnitType == UNIT_PLAYER)
    {
        nFlags = (int32_t)(((UINT_PTR)pPlayer & 0xFFFF0000u) | LOWORD(pMsg->wParam));
        const int32_t bDown = PLAYER_OnMouseClick(5, pMsg->nXpos, pMsg->nYpos, nFlags);
        gbRightButtonDown = bDown;
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAA8F20
//  WM_MOUSEWHEEL, from gWheelNativeHandler and gWheelWin95Handler. The delta is the signed high word of wParam, added to gnMouseWheelAccum; at a
//  magnitude of 120 or more the accumulator divided by 120 (signed, truncating toward zero) goes to CMDTBL_CommandHandleMouseWheel and is reset.
//  The accumulator cannot reach INT_MIN, where abs() is undefined: it is under 120 in magnitude before each add and one add is at most 32768.
//  BUG: on the Windows 95 path the handler misreads even the right message: the MSWheel driver's roll message carries its rotation in the whole
//  wParam, and the high word of +120 is 0, of -120 is -1.
void __stdcall GAME_WindowOnMouseWheel(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nAccum;

    if (!gbInGame)
        return;

    nAccum = gnMouseWheelAccum + (int16_t)HIWORD(pMsg->wParam);
    gnMouseWheelAccum = nAccum;
    if (abs(nAccum) >= 120)
    {
        CMDTBL_CommandHandleMouseWheel(nAccum / 120);
        gnMouseWheelAccum = 0;
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAA8F90
//  WM_MBUTTONUP, from gMButtonHandlers. The chain is broken by a tail jmp (0x6FAA8FB4), the same call as the C's.
void __stdcall GAME_WindowOnMButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    if (gbInGame)
    {
        CMDTBL_CallMouse3Lift();
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
    }
}

//1.10f: D2Client.0x6FAA8FC0
//  WM_MBUTTONDOWN, the twin of the handler above with another callee.
void __stdcall GAME_WindowOnMButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    if (gbInGame)
    {
        CMDTBL_CallMouse3Down();
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
    }
}

//1.10f: D2Client.0x6FAA8FF0
//  WM_XBUTTONDOWN, from gXButtonHandlers. The button is the high word of wParam, compared as a word: 1 is the fourth mouse button, 2 the fifth;
//  any other is consumed with no call.
void __stdcall GAME_WindowOnXButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    uint16_t nButton;

    if (gbInGame)
    {
        nButton = HIWORD(pMsg->wParam);
        if (nButton == 1)
            CMDTBL_CommandHandleMouse4Down();
        else if (nButton == 2)
            CMDTBL_CommandHandleMouse5Down();
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
    }
}

//1.10f: D2Client.0x6FAA9040
//  WM_XBUTTONUP, the twin of the handler above.
void __stdcall GAME_WindowOnXButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    uint16_t nButton;

    if (gbInGame)
    {
        nButton = HIWORD(pMsg->wParam);
        if (nButton == 1)
            CMDTBL_CommandHandleMouse4Lift();
        else if (nButton == 2)
            CMDTBL_CommandHandleMouse5Lift();
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
    }
}

//1.10f: D2Client.0x6FAA9090
//  WM_SYSCOMMAND. The low word of wParam, masked with 0xFFF0 and compared as a word: SC_TASKLIST (0xF130, the Start menu's Ctrl+Esc) and
//  SC_KEYMENU (0xF100, the menu bar's Alt) are always consumed, SC_MOVE (0xF010) only while the window is full screen; the rest go on to Windows.
void __stdcall GAME_WindowOnSysCommand(SMSGHANDLER_PARAMS *pMsg)
{
    const uint16_t nCommand = (uint16_t)(LOWORD(pMsg->wParam) & 0xFFF0);

    if (nCommand == SC_TASKLIST || nCommand == SC_KEYMENU || (nCommand == SC_MOVE && D2GFX_WINDOW_IsFullScreen()))
    {
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
    }
}

//1.10f: D2Client.0x6FAA90E0
//  Consume and stop: VK_LWIN, VK_RWIN and VK_APPS in all three of the file's tables, so the Windows keys cannot pull the Start menu over the game.
//  No retn: the chain break is a tail jmp (0x6FAA90F6).
void __stdcall GAME_WindowConsumeMessage(SMSGHANDLER_PARAMS *pMsg)
{
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAA9100
//  WM_ACTIVATEAPP, in all three of the file's tables. gbAppActive takes wParam, the pump's cue to sleep in the background. On activation the Hold Alt
//  mode is turned off, Stand Still let go, the selection cleared and D2Win's #10176 called with 0. On deactivation the Run key is let go unless it
//  is still down (bit 15 of GetAsyncKeyState's answer), and each mouse button still down is released. The message goes on with bReturn = 0 on both
//  paths. The original inlines GAME_WindowOnLButtonUp here.
void __stdcall GAME_WindowOnActivateApp(SMSGHANDLER_PARAMS *pMsg)
{
    gbAppActive = (int32_t)pMsg->wParam;

    if (pMsg->wParam)
    {
        UI_SetUIToggle(UI_HOLDALT, UI_TURNOFF, 0);
        gbStandStillKeyHeld = 0;
        SELECT_ClearSelectedUnit();
        D2WIN_10176(0);
        pMsg->bReturn = 0;
        return;
    }

    if (CMDTBL_ControlsIsActionBound(HOTKEY_RUN, TRUE))
    {
        if (!(GetAsyncKeyState((uint16_t)CMDTBL_ControlsFindBoundKey(HOTKEY_RUN, TRUE)) & 0x8000))
        {
            gbRunKeyHeld = 0;
        }
    }

    if (gnLeftClickModifierFlags != 16)
    {
        GAME_WindowOnLButtonUp(pMsg);
    }

    //  The first five statements of GAME_WindowOnRButtonUp.
    if (gnRightClickModifierFlags != 16)
    {
        gnRightClickModifierFlags = 16;
        gbRightButtonDown = 0;
        gbRightClickIssuedThisFrame = 1;
        gbSuppressHeldClick = 0;
        SELECT_UnlockSelection();
    }

    pMsg->bReturn = 0;
}

//1.10f: D2Client.0x6FAA9230
//  The Clear Screen key, reached through its five-byte thunk j_GAME_CmdTblActionClearScreen (0x6FAD3E50) in the key table. In a game the panels
//  are closed without the automap; if none was open, the automap's centre offsets are recomputed and the panels closed again with the automap.
//  void: eax is the answer of the tail jump at 0x6FAA9257, or gbInGame's 0 outside a game, and no caller reads it.
void __stdcall GAME_CmdTblActionClearScreen()
{
    int32_t nResult = gbInGame;

    if (nResult)
    {
        nResult = UI_CloseAllPanels(0, 1);
        if (!nResult)
        {
            AUTOMAP_RecomputeCenterOffsets(0);
            UI_CloseAllPanels(1, 0);
        }
    }
}

//1.10f: D2Client.0x6FAA9260
//  With an act loaded, D2Common's environment is re-initialised for it and the player's room (index 5, no ticks, the eclipse flag set); without
//  one, gbActEnvironmentDirty is raised, which GAME_PreloadActData reads once the act is allocated. gpAct is read again after CUNIT_GetPlayerRoom.
void __cdecl GAME_EnvReapplyActEnvironment()
{
    D2ActiveRoomStrc *pRoom;

    if (gpAct)
    {
        pRoom = CUNIT_GetPlayerRoom();
        D2COMMON_ENVIRONMENT_InitializeEnvironment(gpAct, pRoom, 5, 0, TRUE);
    }
    else
    {
        gbActEnvironmentDirty = 1;
    }
}

//  The two tiles GAME_PreloadActData loads by name.
static const char szWarpTile[] = "data\\global\\tiles\\act1\\barracks\\warp.dt1";
static const char szBlankTile[] = "data\\global\\tiles\\act1\\outdoors\\blank.dt1";

//1.10f: D2Client.0x6FAA9290
//  The act, loaded from the seed packet (SCMD_HandleSrv03_GameSeed's call). Every level type of the act (36 asked, 32 files each) has its files
//  preloaded through D2CMP's #10095, a name starting with '0' or empty skipped; then two tile files by name. The old act is freed and the new one
//  allocated with this module's automap callbacks; the environment is set up now if GAME_EnvReapplyActEnvironment found no act to set it up in,
//  but only for act 1 (cmp bl, 1); the loading screen is told the act; the client block's two seeds are stored; the automap colours, the light map
//  and the palette indices are reset; the town level is remembered; the act's COF data is loaded; the seed is traced; the screen is cleared; the
//  catch-up deadline is pushed ten seconds out. `retn 8`. The original inlines GAME_ExtendCatchUpDeadline here.
//
//  The act+1 byte is stored as a byte (0x6FAA929F) and pushed as a dword (0x6FAA92A5): D2Common reads only the byte, so this is the char ABI and
//  not a defect. D2Common answers with all of eax (`mov eax, 1` / `xor eax, eax`), hence `test eax, eax` over a bool. gbActEnvironmentDirty is a
//  byte, read and stored as one. The client block's two dwords are zeroed and then written, the block's pointer re-read for each.
void __fastcall GAME_PreloadActData(uint8_t nAct, uint32_t dwGameSeed, int32_t nObjectSeed, int32_t nTownLevelId)
{
    D2TileLibraryHashStrc *pTileHash;
    char szTrace[40];
    char szFile[60];
    const uint8_t nActPlusOne = (uint8_t)(nAct + 1);

    for (int32_t nLevelType = 0; nLevelType < 36; ++nLevelType)
    {
        if (!D2COMMON_DATATBLS_CheckActInLevelTypesTxt(nLevelType, nActPlusOne))
            continue;
        for (int32_t nFile = 0; nFile < 32; ++nFile)
        {
            D2COMMON_DATATBLS_GetFileNameFromLevelTypeAndFileId(nLevelType, nFile, szFile);
            if (szFile[0] != '0' && szFile[0])
                D2CMP_10095(&pTileHash, szFile);
        }
    }

    D2CMP_10095(&pTileHash, szWarpTile);
    D2CMP_10095(&pTileHash, szBlankTile);

    if (gpAct)
        D2COMMON_DUNGEON_FreeAct(gpAct);
    D2DrlgActStrc *pAct = D2COMMON_DUNGEON_AllocAct(nAct, dwGameSeed, TRUE, NULL, gnDifficulty, NULL, nTownLevelId, AUTOMAP_RebuildAutomapLayer, AUTOMAP_AddTown);
    gpAct = pAct;

    if (gbActEnvironmentDirty && nAct == 1 && pAct)
    {
        D2ActiveRoomStrc *pRoom = CUNIT_GetPlayerRoom();
        D2COMMON_ENVIRONMENT_InitializeEnvironment(gpAct, pRoom, 5, 0, TRUE);
    }
    gbActEnvironmentDirty = 0;

    UI_SetLoadScreenAct(nAct);
    UI_DrawLoadingScreen(1);

    ((uint32_t *)gpClientGameBlock)[0] = 0;
    ((uint32_t *)gpClientGameBlock)[1] = 0;
    ((uint32_t *)gpClientGameBlock)[0] = dwGameSeed;
    ((uint32_t *)gpClientGameBlock)[1] = (uint32_t)nObjectSeed;

    AUTOMAP_InitSymbolColors();
    DLIGHTMAP_Init();
    UI_CachePaletteIndices();
    gnPreloadedLevelId = nTownLevelId;
    GFX_FreeCofCache();
    GFX_LoadActCofData(nAct);

    sprintf(szTrace, "Seed: %d", dwGameSeed);
    FOG_Trace(szTrace);
    gnForceClearFrames = 1;

    GAME_ExtendCatchUpDeadline(10000);
}

//1.10f: D2Client.0x6FAA9420
//  D2Gfx's WINDOW_Destroy, then its Release. `retn 0Ch`: three stack arguments, none read (the handler slot's
//  signature), named by position. GAME_InitGame installs it with FAULT_SetAssertHandler, whose slot nothing calls, so it never runs.
//  void: eax is left holding the answer of D2Gfx's Release, and no caller reads it.
void __stdcall GAME_TeardownGraphics(int32_t nArg1, int32_t nArg2, int32_t nArg3)
{
    D2GFX_WINDOW_Destroy();
    D2GFX_Release();
}

//1.10f: D2Client.0x6FAA9430
//  Stores gbLeaveGameSent.
void __fastcall GAME_SetLeaveGameSent(int32_t bSent)
{
    gbLeaveGameSent = bSent;
}

//1.10f: D2Client.0x6FAA9440
//  Returns gbLeaveGameSent.
int32_t __cdecl GAME_GetLeaveGameSent()
{
    return gbLeaveGameSent;
}

//  Zeroes the 128 bytes from 0x6FBA7750 that GAME_InitGame and GAME_AppModeGame clear with one `rep stosd` (32 dwords): the 24 globals from
//  gnActiveMessageCallbackIndex to gnSkillBlockUntilFrame, and the dword at 0x6FBA7790, which nothing references. Each global is an object of its
//  own here, so a memset from the first would write past it.
//  DIFFERENCE: ours zeroes the 24 globals by name, in address order, and does not write the unreferenced dword at 0x6FBA7790. A helper of ours.
static inline void GAME_ClearClientGlobalData()
{
    gnActiveMessageCallbackIndex = 0;
    gpfnActiveMessageCallback = NULL;
    ghClientPoolSystem = 0;
    gdwFrameTick = 0;
    gdwLastGameTickTime = 0;
    gnTotalGameTicks = 0;
    gnClientFrame = 0;
    gnNextPingTick = 0;
    gnPingSentTick = 0;
    gnNetworkLatency = 0;
    gnRecentTickRate = 0;
    gnFrameCountInStatsWindow = 0;
    gnRecentSkipRate = 0;
    gnSkippedTickCountInStatsWindow = 0;
    gdwFrameStatsWindowStart = 0;
    gdwCatchUpResetDeadline = 0;
    UNRESOLVED_dword_6FBA7794 = 0;
    gnInteractThrottleTick = 0;
    gnLatencySampleCount = 0;
    gnNetworkLatencyRunningMean = 0;
    memset(gPongPacketBody, 0, sizeof(gPongPacketBody));
    gbExpansionGame = 0;
    gbLadderGame = 0;
    gnSkillBlockUntilFrame = 0;
}

//1.10f: D2Client.0x6FAA9450
//  Everything a game needs before its first frame; GAME_AppModeGame calls it once, after SOUNDHDR_Init and before the message pump.
//  Zero the 128-byte client block, clear the six flags a previous game left set, make the pool system, read Color.txt, allocate the view, read the
//  resolution, put the loading screen up, tell the server we are opening the character, install the graphics-teardown handler, prime the palettes,
//  take the two timestamps, install the draw callback, and reset the units, the lightmap, the environment and the chat history.
//  The draw callback (`mov gpfnActiveMessageCallback, offset GAME_DrawGameScene`, 0x6FAA9593) is what makes our GAME_DrawGameScene reachable from
//  GAME_UpdateFrame.
//  ARCHIVE_OpenFileOrTrace's third parameter is an out-handle (the address of a local that ARCHIVE_ReadFile then uses as the handle), not a mode.
//  The FOG_Alloc line number 1339 is the operand.
void __cdecl GAME_InitGame()
{
    //  The 128-byte client block, zeroed by name.
    GAME_ClearClientGlobalData();

    gnDisconnectCountdown = 0;
    gbGameTeardownActive = 0;
    gbD2xOutCinematicPending = 0;
    gbVanquishedDismissed = 0;
    gbGameExitPending = 0;
    gbStandStillKeyHeld = 0;

    FOG_CreateNewPoolSystem(&ghClientPoolSystem, "Client Pool System", 0x40000, 0x1000);
    CUNIT_SetPlayerUnit(NULL);

    //  One byte, and only the character '1' turns green blood on. The flag is never cleared here, so once green blood is on a later game skips the
    //  file; a missing file or any other byte leaves it 0 and every game reads the file again.
    if (!gbUseGreenBlood)
    {
        void *hColorFile;
        if (ARCHIVE_OpenFileOrTrace(0, "Data\\Local\\Color.txt", &hColorFile, 1))
        {
            char nColorDigit;
            ARCHIVE_ReadFile(NULL, hColorFile, &nColorDigit, 1);
            ARCHIVE_CloseFile(NULL, hColorFile);
            if (nColorDigit == '1')
                gbUseGreenBlood = 1;
        }
    }

    //  The view survives between games; only the first one allocates it.
    if (!gpView)
        gpView = VIEW_Alloc();

    MINIPANEL_LoadResolutionFromRegistry();
    UI_LoadScreenBegin();
    UI_DrawLoadingScreen(1);

    //  The four networked types that join or host an open game; the binary tests 7, 9, 6, 8 in that order.
    if (gnGameType == GAMETYPE_OBNET_JOIN || gnGameType == GAMETYPE_LAN_JOIN || gnGameType == GAMETYPE_OBNET_HOST || gnGameType == GAMETYPE_LAN_HOST)
    {
        MSG_Send_D2CLTSYS_OPENCHAR(1);
    }

    //  Nothing calls the slot it fills.
    FAULT_SetAssertHandler((void *)GAME_TeardownGraphics);
    PALSHIFT_Init();
    D2CMP_10096();
    MEMPART_InitMemoryPartitions(ghClientPoolSystem, 1);

    //  A 0-or-1 store (neg / sbb / neg), not the raw return value.
    gbDisconnecting = (MSG_IsJoinGameLoopDone() != 0);

    gnConnectionTimestamp = GetTickCount();
    gnGameStartTick = GetTickCount();

    //  The draw callback.
    gpfnActiveMessageCallback = GAME_DrawGameScene;

    gnClientFrame = 0;
    gnTotalGameTicks = 0;

    VIEW_SetOrigin(gpView, 0, 0);
    SCROLL_SetWindowMode(PANEL_NONE);
    gnSkillBlockUntilFrame = 0;

    MONSTER_ClearPlayerSlots();
    MEMPART_Nullsub1();
    QUESTSFX_Reset();

    //  Eight bytes at Game.cpp line 1339; GAME_AppModeGame frees the same pointer at line 1422.
    gpClientGameBlock = FOG_Alloc(8, szSourceFile, 1339, 0);

    CUNIT_ClearUnitTables();
    DLIGHTMAP_ClearAmbientLight();
    ENV_Init();
    CHAT_ResetHistory();

    gbLeaveGameSent = 0;
    gbInGame = 0;
}

//1.10f: D2Client.0x6FAA9610
//  Five bytes, `jmp ENV_UnloadImages`; the result is not used.
void __cdecl j_ENV_UnloadImages()
{
    ENV_UnloadImages();
}


//1.10f: D2Client.0x6FAA9620
//  Chooses the callback GAME_UpdateFrame's early exit calls (`(*gpfnActiveMessageCallback)(0)`): slot nIndex of gafnMessageCallbacks. Nothing
//  bounds nIndex. No caller uses the pointer left in eax.
void __fastcall GAME_SetActiveMessageCallback(int32_t nIndex)
{
    const D2ClientMessageCallbackFn pfnCallback = gafnMessageCallbacks[nIndex];
    gnActiveMessageCallbackIndex = nIndex;
    gpfnActiveMessageCallback = pfnCallback;
}

//1.10f: D2Client.0x6FAA9640
//  The draw half of a frame, everything GAME_UpdateFrame does not do. It is never called by name: GAME_InitGame installs it in
//  gpfnActiveMessageCallback, and the two `(*gpfnActiveMessageCallback)(0)` in GAME_UpdateFrame are the draw.
//  Centres the view on the player, shakes it, updates the weather and the ambient light, then, if D2GFX lets us start a frame, draws the world, the
//  panels, the cursor and the debug overlay, and ends the scene.
//  D2GFX_StartDraw's first argument is the level's dwBlankScreen itself (0x6FAA979A mov eax, [eax+218h]; 0x6FAA97A6 push eax), or 0 when there is no
//  room, not a boolean. One register carries the room pointer and then dwBlankScreen; it is written here as an int that starts at 0.
//  MEMPART_Nullsub1 is called in the original with 10 in ecx and edx (0x6FAA97E3); its body is a bare retn, so it is called with no argument here.
//  FOG_DisplayAssert("ptLevelData", <this file>, 1491) then exit(-1), and the "global static overwrite error" trace at line 1713, are the original's;
//  the line numbers are the operands (0x5D3 and 0x6B1).
//  Two early returns where the original nests: a void function's early return is the fall-through to the epilogue, so the control flow is the same.
void __fastcall GAME_DrawGameScene(int32_t bNoDefaultCursor)
{
    UI_UnloadLoadingScreen();

    //  Nothing is drawn while the window is inactive, the same gate GAME_UpdateFrame opens with.
    if (D2GFX_WINDOW_GetState())
        return;

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();

    int32_t pnLeft, pnTop, pnRight, pnBottom;
    VIEW_GetRect(gpView, &pnLeft, &pnTop, &pnRight, &pnBottom);

    //  Centre the viewport on the player. `cdq / sub eax, edx / sar eax, 1` is a signed divide by two rounding toward zero, which C's `/ 2` does
    //  on a signed int.
    int32_t nX = D2COMMON_UNITS_GetClientCoordX(pPlayer) - (pnRight - pnLeft) / 2;
    int32_t nY = D2COMMON_UNITS_GetClientCoordY(pPlayer) - (pnBottom - pnTop) / 2;
    VIEWEFFECTS_UpdateScreenShake(&nX, &nY);

    //  -1 in both is the "no previous frame" sentinel, so the first frame after a load produces no scroll delta. Either one not being -1 is enough,
    //  hence `||`.
    if (gnPrevViewOffsetX != -1 || gnPrevViewOffsetY != -1)
    {
        gnViewScrollDeltaX = nX - gnPrevViewOffsetX;
        gnViewScrollDeltaY = nY - gnPrevViewOffsetY;
    }
    gnPrevViewOffsetX = nX;
    gnPrevViewOffsetY = nY;

    VIEW_SetOrigin(gpView, nX, nY);
    VIEW_Draw(gpView, 0);

    ENV_UpdateWeather();

    //  Three one-byte out-parameters, each read back as a byte out of its own dword slot (`and eax, 0FFh` three times at 0x6FAA9749).
    uint8_t pRed[4], pGreen[4], pBlue[4];
    D2COMMON_ENVIRONMENT_GetLightColorFromAct(gpAct, pRed, pGreen, pBlue);
    D2GFX_SetAmbientColor(pRed[0], pGreen[0], pBlue[0]);

    //  0 when there is no room; otherwise the level's own BlankScreen value, passed raw.
    int32_t nBlankScreen = 0;
    D2ActiveRoomStrc *pRoom = CUNIT_GetPlayerRoom();
    if (pRoom)
    {
        D2LevelsTxt *pLevel = D2COMMON_DATATBLS_GetLevelsTxtRecord(D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom));
        if (!pLevel)
        {
            FOG_DisplayAssert("ptLevelData", szSourceFile, 1491);
            exit(-1);
        }
        nBlankScreen = (int32_t)pLevel->dwBlankScreen;
    }

    if (!D2GFX_StartDraw(nBlankScreen, 0, 0, 0))
        return;

    D2WIN_10179();
    SCROLL_UpdateViewportOrigin();

    //  UI shift state 3 suppresses the world background (the full-screen panels case).
    if (SCROLL_GetUiShiftScreen() != 3)
        VIEWEFFECTS_EnvUpdateViewAndLevelEffects(gpView);

    UIPANELS_DrawInGamePanels(gpView);
    D2WIN_10019();

    //  Called with 10 in ecx and edx in the original; the callee is empty.
    MEMPART_Nullsub1();

    if (!bNoDefaultCursor)
        CURSOR_DrawCursorDefault();

    DRAW_GameDrawDebugOverlay();

    //  A countdown of frames to force-clear. It decrements every such frame and complains once when it goes negative: the decremented value is
    //  stored before the test, and the message is the original's own wording.
    if (gnForceClearFrames)
    {
        D2GFX_ClearScreen(0);
        if (--gnForceClearFrames < 0)
        {
            gnForceClearFrames = 0;
            FOG_Trace("global static overwrite error at line %d of file %s\n", 1713, szSourceFile);
        }
    }

    //  A tail jmp in the original (0x6FAA983F), which is what a void function's last call compiles to.
    D2GFX_EndScene();
}

//1.10f: D2Client.0x6FAA9850
//  Starts the client's disconnect / exit sequence with a D2JOINERR_ message id, and does nothing at all if a shutdown is already running.
//  GAME_UpdateFrame calls it twice and the original also inlines it twice there.
//  An id of 29 or more becomes 9 (D2JOINERR_unknown_failure), so the valid range is [0, 28], the 29 members of the enum; the extent is asserted so
//  that a regenerated D2Constants.h that loses or gains a member breaks the build. gnDisconnectMessageStrings (0x6FB758E0) is 29 16-bit tbl indices.
//  BUG: the clamp (`cmp ecx, 1Dh / jl`, signed) checks one end only, so a negative nMessageId is stored unclamped and would index
//  gnDisconnectMessageStrings out of bounds in GAME_DrawDisconnectMessage. LATENT: every caller passes a constant in [0, 28].
//  The global is stored (0x6FAA986E) before the clamp is tested (0x6FAA9874), so it transiently holds the out-of-range value; nothing observes that.
static_assert(D2JOINERR_COUNT == 29,
              "GAME_BeginDisconnect clamps at 29: `cmp ecx, 1Dh` at 0x6FAA986B");
static_assert(D2JOINERR_unknown_failure == 9,
              "GAME_BeginDisconnect falls back to 9 at 0x6FAA9876");

void __fastcall GAME_BeginDisconnect(D2C_JoinErrors nMessageId)
{
    //  Three guards, in the binary's order: do nothing if a countdown is already running, if the end-of-game sequence is pending, or if the
    //  teardown is active.
    if (!gnDisconnectCountdown && !gbEndOfGamePending && !gbGameTeardownActive)
    {
        gnDisconnectMessage = nMessageId;
        if (nMessageId >= D2JOINERR_COUNT)
            gnDisconnectMessage = D2JOINERR_unknown_failure;   // 5372

        UI_CloseAllPanels(1, 0);
        gnDisconnectCountdown = 600;
        gbDisconnecting = 1;
        gbInGame = 0;
        gbConnectedToGame = 0;
        WINMAIN_SetAppModeContinue(0);
    }
}

//1.10f: D2Client.0x6FAA98C0
//  The disconnect screen. GAME_AppModeGame hands it to WNDPROC_WindowRunMessagePump once GAME_BeginDisconnect has set gnDisconnectCountdown to 600;
//  each pass of the pump draws the message and counts down, and at zero it unloads the loading screen and answers 1, which ends the pump. The
//  pump's idle counter arrives in nUnused and is never read. `push ecx` at 0x6FAA98C0 is the slot D2WIN_10199 writes the line count into.
//  The message is gnDisconnectMessageStrings[gnDisconnectMessage], a table of 29 tbl indices, with one override: for an expansion character
//  (CLIENTSAVEFLAG_EXPANSION in the launch config's save flags, `test byte ptr [ecx+1EAh], 20h`, 0x6FAA9919) message 20 becomes tbl 21794 and 21
//  becomes 21793 ("must kill Baal"), where the table holds 5364 and 5363 ("must kill Diablo").
//  Layout: font 3 (D2FONT_FONT42) and one line at y 200 while the text is narrower than gnViewportRight - 40 (a signed `jge`, 0x6FAA9956).
//  Otherwise font 2 (D2FONT_FONT30), split at 600 by D2WIN_10199, each line at 2 * nLine * D2WIN_10125() + 200, twice the line height apart. Every
//  x is (gnViewportRight - width) >> 1 by `sar` with no `cdq` (0x6FAA9963, 0x6FAA99AC), a shift rather than a `/ 2`; the line index and count
//  compare unsigned (`jbe` 0x6FAA9997, `jb` 0x6FAA99CF).
int32_t __stdcall GAME_DrawDisconnectMessage(int32_t nUnused)
{
    UNREFERENCED_PARAMETER(nUnused);
    int32_t nLineCount;

    if (!gnDisconnectCountdown)
        return 0;

    if (!D2GFX_WINDOW_GetState() && D2GFX_StartDraw(1, 0, 0, 0))
    {
        D2GFX_ClearScreen(0);
        D2WIN_10127_SetFont(D2FONT_FONT42);
        const Unicode *wszText = D2LANG_GetStringFromTblIndex((D2C_StringIndices)gnDisconnectMessageStrings[gnDisconnectMessage]);
        if (gpLaunchConfig->unpackedCTemp.nCharacterSaveFlags & CLIENTSAVEFLAG_EXPANSION)
        {
            if (gnDisconnectMessage == D2JOINERR_NightmareNotUnlocked)
                wszText = D2LANG_GetStringFromTblIndex(STR_IDX_21794_x);
            else if (gnDisconnectMessage == D2JOINERR_HellNotUnlocked)
                wszText = D2LANG_GetStringFromTblIndex(STR_IDX_21793_x);
        }

        const int32_t nTextWidth = D2WIN_10121_GetTextWidth(wszText);
        if (nTextWidth >= gnViewportRight - 40)
        {
            D2WIN_10127_SetFont(D2FONT_FONT30);
            D2SplittedTextStrc *pSplitText = D2WIN_10199(wszText, &nLineCount, 600);
            D2SplittedTextStrc *pLine = pSplitText;
            for (uint32_t nLine = 0; nLine < (uint32_t)nLineCount; ++nLine)
            {
                const int32_t nX = (gnViewportRight - D2WIN_10121_GetTextWidth(pLine->wszLine)) >> 1;
                D2WIN_10117_DrawText(pLine->wszLine, nX, 2 * (int32_t)nLine * D2WIN_10125() + 200, 0, 0);
                pLine = pLine->pNextLine;
            }
            D2WIN_10200(pSplitText);
        }
        else
        {
            D2WIN_10117_DrawText(wszText, (gnViewportRight - nTextWidth) >> 1, 200, 0, 0);
        }
        D2WIN_10019();
        D2GFX_EndScene();
    }

    if (--gnDisconnectCountdown == 0)
    {
        UI_UnloadLoadingScreen();
        return 1;
    }
    return 0;
}

//1.10f: D2Client.0x6FAA9A10
//  One ping at most every 5000 ms: due when gnNextPingTick is below now (an unsigned compare, `jnb` at 0x6FAA9A20, though the global is typed
//  int), and the send stamps gnPingSentTick for GAME_OnPongUpdateLatency.
//  DIFFERENCE: between the two stores the original loads the launch config's save-flags byte into cl (0x6FAA9A38) and never uses it; ours omits
//  the dead load.
void __cdecl GAME_SendPingIfDue()
{
    const uint32_t dwNow = GetTickCount();
    if ((uint32_t)gnNextPingTick < dwNow)
    {
        MSG_SendPing6C();
        gnPingSentTick = (int32_t)dwNow;
        gnNextPingTick = (int32_t)(dwNow + 5000);
    }
}

//1.10f: D2Client.0x6FAA9A50
//  Server packet 0x8F's body (the 32 bytes after its id) is copied a dword at a time into gPongPacketBody, and element [4] of it (0x6FBA77B4) is
//  overwritten with GetTickCount(). Then the latency: a second GetTickCount() minus gnPingSentTick, into gnNetworkLatency; and for the first ten
//  samples (an unsigned count, `jnb` 0x6FAA9A8D) the running mean (n * mean + x) / (n + 1), an unsigned `div` (0x6FAA9A9D); after ten the mean
//  never moves again. The packet's dwords are read unaligned; memcpy is that load, defined. void: the caller does not use the mean left in eax.
//  DIFFERENCE: the original's copy loop is a do-while bounded by the address of the next global (`cmp eax, offset gbExpansionGame; jl`, 0x6FAA9A60,
//  a signed compare of two addresses; 0x6FBA77C4 - 0x6FBA77A4 is 32, eight dwords). Two objects of ours are ordered by nothing C++ promises, so the
//  loop ends at the array's own end, gPongPacketBody + 8, compared signed as the `jl` is.
void __fastcall GAME_OnPongUpdateLatency(const uint8_t *pPacket)
{
    uint32_t *pDest = gPongPacketBody;
    const uint8_t *pSrc = pPacket + 1;
    do
    {
        uint32_t dwWord;
        memcpy(&dwWord, pSrc, sizeof(dwWord));
        pSrc += sizeof(dwWord);
        *pDest++ = dwWord;
    } while ((intptr_t)pDest < (intptr_t)(gPongPacketBody + 8));

    gPongPacketBody[4] = GetTickCount();
    const uint32_t dwLatency = GetTickCount() - (uint32_t)gnPingSentTick;
    gnNetworkLatency = (int32_t)dwLatency;
    const uint32_t nSamples = gnLatencySampleCount;
    if (nSamples < 10)
    {
        const uint32_t nMeanLatencyMs = (nSamples * gnNetworkLatencyRunningMean + dwLatency) / (nSamples + 1);
        gnLatencySampleCount = nSamples + 1;
        gnNetworkLatencyRunningMean = nMeanLatencyMs;
    }
}

//1.10f: D2Client.0x6FAA9AB0
void __fastcall GAME_SetSkillBlockUntilFrame(int32_t nFrame)
{
    gnSkillBlockUntilFrame = nFrame;
}

//1.10f: D2Client.0x6FAA9AC0
int32_t __cdecl GAME_GetSkillBlockUntilFrame()
{
    return gnSkillBlockUntilFrame;
}

//1.10f: D2Client.0x6FAA9AD0
int32_t __cdecl GAME_GetNetworkLatency()
{
    return gnNetworkLatency;
}

//1.10f: D2Client.0x6FAA9AE0
int32_t __cdecl GAME_GetNetworkLatencyRunningMean()
{
    return (int32_t)gnNetworkLatencyRunningMean;
}

//1.10f: Inlined in D2Client.0x6FAA9AF0 GAME_UpdateFrame, TWICE
//  The tail both held-mouse-button blocks share: assemble the movement modifiers out of three globals, and if the control unit is a player, hand the
//  click to PLAYER_OnMouseClick at the current cursor position. The original has two copies of it in GAME_UpdateFrame (0x6FAA9BC1 and 0x6FAA9C23),
//  differing only in the click event (1 and 4).
//  The two callers differ and are left as written: the left button is also gated on !gbSuppressHeldClick (0x6FAA9BB3) and the right one is not
//  (0x6FAA9C1D).
static void CLIENT_IssueHeldMouseClick(int32_t nClickEvent)
{
    D2UnitStrc *pPlayer = CUNIT_GetPlayer();

    //  `mov ecx, esi` then `or ecx, esi`, with esi = 8: an assignment and then an OR of the same bit, written as the compiler emitted it.
    int32_t nFlags = 0;
    if (gbRunKeyHeld)
        nFlags = 8;
    if (gbRunToggled)
        nFlags |= 8;
    if (gbStandStillKeyHeld)
        nFlags |= 4;

    if (pPlayer && pPlayer->dwUnitType == UNIT_PLAYER)
    {
        //  The pushes read nFlags, then Y; X goes in edx and the mask in ecx.
        const int32_t nY = CURSOR_GetMouseY();
        const int32_t nX = CURSOR_GetMouseX();
        PLAYER_OnMouseClick(nClickEvent, nX, nY, nFlags);
    }
}

//1.10f: Inlined in D2Client.0x6FAA9AF0 GAME_UpdateFrame, TWICE
//  Is the key bound to this hotkey currently held down? The original has two copies of it in GAME_UpdateFrame (0x6FAA9C6B primary, 0x6FAA9CA7
//  secondary), differing only in bPrimary.
//  It asks twice: CMDTBL_ControlsIsActionBound (0x6FAD4F00) is itself `CMDTBL_ControlsFindBoundKey(...) != 0xFFFF`, so the original looks the key up,
//  and then looks it up again to use it. Reproduced.
static BOOL CONTROLS_IsHotkeyDown(D2HotkeyIndices eCmd, BOOL bPrimary)
{
    if (!CMDTBL_ControlsIsActionBound(eCmd, bPrimary))
        return FALSE;

    const uint16_t nKey = (uint16_t)CMDTBL_ControlsFindBoundKey(eCmd, bPrimary);
    if (nKey == 0xFFFF)
        return FALSE;

    //  `test ah, ah` / `js`: bit 15 of the SHORT, the key-is-down bit.
    return (GetAsyncKeyState(nKey) & 0x8000) != 0;
}

//1.10f: Inlined in D2Client.0x6FAA9AF0 GAME_UpdateFrame, TWICE
//1.13c: D2Client.0x6FAF40C0
//  Walks the act's active rooms and gives each one that has not been visited its first-time population: critters, preset units, quest effects, then
//  the seen bit. The original has two copies of it in GAME_UpdateFrame (0x6FAAA05C and 0x6FAAA193).
static void CLIENT_PopulateRooms()
{
    for (D2ActiveRoomStrc *pRoom = D2COMMON_DUNGEON_GetRoomFromAct(gpAct); pRoom != NULL; pRoom = pRoom->pRoomNext)
    {
        if ((pRoom->dwFlags & 1) == 0)
        {
            ENV_SpawnRoomCritters(pRoom);
            CUNIT_CreatePresetUnitsInRoom(pRoom);
            QUESTSFX_OnRoomActivated(pRoom);
            pRoom->dwFlags |= 1;
        }
    }
}

//1.00 : D2Client.0x100105D0
//1.10f: Inlined in D2Client.0x6FAA9AF0 GAME_UpdateFrame, TWICE
//1.13c: D2Client.0x6FAF4A80
//  One game-logic tick: everything that advances the world and nothing that draws it. The original has two copies of it in GAME_UpdateFrame
//  (0x6FAAA04C and 0x6FAAA17C), the same calls in the same order, one in the singleplayer arm and one in the networked arm.
static void CLIENT_MainUpdate()
{
    gdwFrameTick = GetTickCount();

    CLIENT_PopulateRooms();
    CUNIT_UpdateAllUnits();
    D2COMMON_DUNGEON_AnimateTiles(CUNIT_GetPlayerRoom());
    QUESTSFX_TickScreenFade();
    GFX_PreloadVisibleUnits(1);

    if (D2COMMON_ENVIRONMENT_UpdatePeriodOfDay(gpAct, CUNIT_GetPlayerRoom()))
    {
        CUNIT_RefreshObjectsOnDayNightChange();
    }

    D2COMMON_DRLGACTIVATE_Update(D2COMMON_DUNGEON_GetDrlgFromAct(gpAct));
    AUTOMAP_Tick();

    //  Every thirteenth tick. `xor edx, edx; div ecx` is an unsigned divide, which is what gnClientFrame's type gives.
    if ((++gnClientFrame % 13) == 0)
        D2COMMON_DUNGEON_UpdateAndFreeInactiveRooms(gpAct);
}

//1.10f: D2Client.0x6FAA9AF0
//  The body of a frame: the idle callback GAME_AppModeGame hands to WNDPROC_WindowRunMessagePump. Its parameter is the pump's iteration counter,
//  which this build never reads. A non-zero return ends the pump; this function returns non-zero on exactly one path, when gbDisconnecting is set
//  at the bottom.
//  In order: two early exits for single-player; held mouse buttons turned into clicks (which is why holding the button keeps you walking); the
//  stand-still key polled with GetAsyncKeyState; the 5 s keep-alive ping; the ~3 s frame-statistics window; the fixed-timestep accumulator that
//  decides whether a game-logic tick runs this frame; the local server pumped for single-player; both packet queues drained; the network-loss and
//  timeout checks; one game-logic tick; the draw callback; and the teardown that stops the open-server thread.
//  The original inlines GAME_BeginDisconnect(7) twice (0x6FAA9EE1 and 0x6FAA9F4C; it also calls GAME_BeginDisconnect(6) out of line twice), and
//  CLIENT_MainUpdate, CLIENT_PopulateRooms, CLIENT_IssueHeldMouseClick and CONTROLS_IsHotkeyDown twice each.
//  The original stores gbStandStillKeyHeld = 1 (0x6FAA9CD3) before the `js` that decides whether the key is down and overwrites it with 0 when it is
//  not (0x6FAA9CDB); the if / else-if / else written here has the same net effect, and nothing observes the transient 1.
//  The second WINDOW_GetState: the original re-reads gbInGame at 0x6FAAA28B and, when the tick block cleared it, falls into the same
//  WINDOW_GetState at 0x6FAAA31B; written here as two consecutive `if`s.
//
//  C4018 is switched off for this function on purpose. Four comparisons mix a DWORD tick delta with an int, and the implicit conversion to unsigned
//  is the program: it puts `jnb` / `jb` / `ja` where the original has them. The four are gnNextPingTick against the ping's GetTickCount, and
//  nSinceLastTick against gnGameTickIntervalMs three times (`>=`, `>`, and `>= 2 *`). Every other comparison here is int against int, signed.
#pragma warning(push)
#pragma warning(disable : 4018)
int32_t __stdcall GAME_UpdateFrame(int32_t nIdleIteration)
{
    //  DIFFERENCE: zeroed here, where the original's stack bytes are whatever was there. SCMD_RouteServerPackets reads a unit header from bytes 1..5
    //  of every one-byte packet 0x6E..0x72; after a longer packet of the same frame they are its leftovers, in ours as in the original, but for the
    //  first packet of a frame the original's are indeterminate, which C++ does not let us read. Ours reads zeros there.
    uint8_t pBuffer[512] = {};

    //  The pump's iteration counter. Never read.
    (void)nIdleIteration;

    int32_t bTickRan = 0;
    int32_t bDidMainUpdate = 0;

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();

    //  The two single-player types, written by name.
    if (gnGameType == GAMETYPE_SINGLEPLAYER || gnGameType == GAMETYPE_SINGLEPLAYER_UNCAPPED)
    {
        //  The client does nothing at all while the window is inactive. It only timestamps, so the accumulator does not try to catch up the whole
        //  idle period on the way back.
        if (D2GFX_WINDOW_GetState())
        {
            gdwLastGameTickTime = GetTickCount();
            return 0;
        }

        //  Esc menu or the configuration panel open, with a player in a room: draw and pump sound, but run no game logic. This is what makes
        //  single-player pause.
        if ((UI_GetUiWindowStateRaw(UI_ESCMENU) || UI_GetUiWindowStateRaw(UI_CONFIG)) && pPlayer && CUNIT_GetPlayerRoom())
        {
            gdwLastGameTickTime = GetTickCount();
            (gpfnActiveMessageCallback)(0);
            SOUNDHDR_PumpPreload();
            return 0;
        }
    }

    RECORD_ReadNextForFrame();

    //  Held mouse buttons become clicks here, not in the window procedure.
    if (gbLeftButtonDown)
    {
        gnLeftClickModifierFlags = 8;
        if (gbInGame && !gbSuppressHeldClick)
        {
            gbLeftClickIssuedThisFrame = 1;
            CLIENT_IssueHeldMouseClick(1);
        }
    }

    if (gbRightButtonDown)
    {
        gnRightClickModifierFlags = 8;
        if (gbInGame)
        {
            gbRightClickIssuedThisFrame = 1;
            CLIENT_IssueHeldMouseClick(4);
        }
    }

    if (CONTROLS_IsHotkeyDown(HOTKEY_STANDSTILL, TRUE))
        gbStandStillKeyHeld = 1;
    else if (CONTROLS_IsHotkeyDown(HOTKEY_STANDSTILL, FALSE))
        gbStandStillKeyHeld = 1;
    else
        gbStandStillKeyHeld = 0;

    //  The keep-alive, every five seconds. gnNextPingTick is an int and the comparison is `jnb`, so it is unsigned, which is what C does with an
    //  int against a DWORD, hence no cast here.
    const uint32_t dwPingNow = GetTickCount();
    if (gnNextPingTick < dwPingNow)
    {
        MSG_SendPing6C();
        gnPingSentTick = dwPingNow;
        gnNextPingTick = dwPingNow + 5000;
        //  DIFFERENCE: the original loads gpLaunchConfig's save-flags byte here (0x6FAA9D0A) and never uses it; ours omits the dead load.
    }

    const uint32_t dwNow = GetTickCount();
    uint32_t dwLastTick = gdwLastGameTickTime;
    gdwFrameTick = dwNow;
    if (!gdwLastGameTickTime)
    {
        //  First frame: back-date one interval so the very next tick runs.
        dwLastTick = dwNow - gnGameTickIntervalMs;
        gdwLastGameTickTime = dwLastTick;
    }

    //  The ~3 s statistics window. Both folds are `imul` with 0x55555556, the signed divide-by-three idiom, which types these four counters as int
    //  rather than unsigned.
    if (dwNow - gdwFrameStatsWindowStart > 3000)
    {
        gdwFrameStatsWindowStart = dwNow;
        gnRecentTickRate = gnFrameCountInStatsWindow / 3;
        gnFrameCountInStatsWindow = 0;
        const int32_t nSkipRate = gnSkippedTickCountInStatsWindow / 3;
        gnSkippedTickCountInStatsWindow = 0;
        gnRecentSkipRate = nSkipRate;
    }

    //  The fixed-timestep accumulator. gnGameTickIntervalMs is 40 ms, so 25 game-logic ticks a second.
    uint32_t nSinceLastTick = dwNow - dwLastTick;
    if (nSinceLastTick >= gnGameTickIntervalMs)
    {
        gdwLastGameTickTime = dwNow;
        bTickRan = 1;

        //  The hard catch-up reset: too far behind, so throw the backlog away and restart the statistics at the nominal 25 ticks/sec rather than
        //  trying to run the missed ticks.
        if (dwNow < gdwCatchUpResetDeadline)
        {
            const int32_t nDelta = nSinceLastTick - gnGameTickIntervalMs;
            int32_t nBackdate = gnGameTickIntervalMs - 1;
            if (nDelta < nBackdate)
                nBackdate = nDelta;

            gnFramesSinceLastTick = 0;
            nSinceLastTick = gnGameTickIntervalMs;
            gdwLastGameTickTime = dwNow - nBackdate;
            gdwFrameStatsWindowStart = dwNow;
            gnRecentTickRate = 25;
            gnRecentSkipRate = 0;
            gnFrameCountInStatsWindow = 0;
            gnSkippedTickCountInStatsWindow = 0;
        }

        if (gbInGame)
            UNITMODE_RunQueuedCommand(pPlayer);
    }

    //  The local server, for the two single-player types. The result is that of a D2Game ordinal and is named only for where it came from.
    int32_t nGamesUpdateResult = 0;
    if (gnGameType == GAMETYPE_SINGLEPLAYER)
    {
        D2GAME_GAME_ProcessNetworkMessages();
        nGamesUpdateResult = D2GAME_GAME_UpdateGamesProgress(1);
        if (nGamesUpdateResult)
            D2GAME_GAME_UpdateClients(1, 0);
    }
    else if (gnGameType == GAMETYPE_SINGLEPLAYER_UNCAPPED)
    {
        D2GAME_GAME_ProcessNetworkMessages();
        nGamesUpdateResult = D2GAME_GAME_UpdateGamesProgress(1);
        //  The uncapped arm calls this unconditionally; the capped one does not.
        D2GAME_GAME_UpdateClients(0, 0);
    }

    //  Both packet queues, drained to empty. The Sleep(0) is in the original.
    int32_t nPacketLen;
    do
    {
        nPacketLen = D2NET_CLIENT_DequeueSystemPacket(pBuffer, sizeof(pBuffer));
        SCMD_SaveCharacterFile(pBuffer, nPacketLen);
        Sleep(0);
    } while (nPacketLen != -1);

    do
    {
        nPacketLen = D2NET_CLIENT_DequeueGamePacket(pBuffer, sizeof(pBuffer));
        SCMD_RouteServerPackets(pBuffer, nPacketLen);
        Sleep(0);
    } while (nPacketLen != -1);

    CHAT_Update();
    D2MCPCLIENT_10003();

    //  The network-loss and timeout checks, for every game type that has a server on the other end.
    if (gnGameType != GAMETYPE_SINGLEPLAYER && gnGameType != GAMETYPE_SINGLEPLAYER_UNCAPPED)
    {
        const uint32_t dwNetNow = GetTickCount();

        //  The network is gone.
        if (!D2NET_CLIENT_IsConnected() && !gbDisconnecting && gbInGame)
            GAME_BeginDisconnect(D2JOINERR_Your_connection_has_been_interrupted);

        //  Three minutes since the connection timestamp with an exit pending.
        if (gbGameExitPending && dwNetNow - gnConnectionTimestamp > 180000)
        {
            GAME_BeginDisconnect(D2JOINERR_Your_connection_has_been_interrupted);
        }

        //  Thirty seconds and still not in a game.
        if (dwNetNow - gnGameStartTick > 30000 && !gbInGame && !gbDisconnecting && !gbGameExitPending)
        {
            GAME_BeginDisconnect(D2JOINERR_failed_to_join_game_6);
        }

        //  Never connected, on a join type. The two game-type tests are in the binary's order: 9 first, then 7.
        if (!gbConnectedToGame && !gbInGame && !gbDisconnecting && !gbGameExitPending && (gnGameType == GAMETYPE_LAN_JOIN || gnGameType == GAMETYPE_OBNET_JOIN))
        {
            GAME_BeginDisconnect(D2JOINERR_failed_to_join_game_6);
        }
    }

    //  Two separate reads of gbInGame, as the binary does: the block below can clear it, and 0x6FAAA28B loads it again.
    if (gbInGame)
    {
        if (gnGameType == GAMETYPE_SINGLEPLAYER || gnGameType == GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            //  Single-player follows the server: a tick runs when the local game made progress, not when the wall clock says so.
            if (nGamesUpdateResult)
            {
                CLIENT_MainUpdate();
                bDidMainUpdate = 1;

                if (nSinceLastTick > gnGameTickIntervalMs)
                {
                    if (nSinceLastTick >= 2 * gnGameTickIntervalMs)
                    {
                        //  More than two intervals behind: allow one skipped frame and back-date by a whole interval less one.
                        gnFramesSinceLastTick = (gnFramesSinceLastTick <= 1);
                        gdwLastGameTickTime += 1 - gnGameTickIntervalMs;
                    }
                    else
                    {
                        gnFramesSinceLastTick = 0;
                        gdwLastGameTickTime += gnGameTickIntervalMs - nSinceLastTick;
                    }
                }
                else
                {
                    gnFramesSinceLastTick = 0;
                }
            }
        }
        else
        {
            //  Networked: a tick runs when the accumulator says one is due.
            if (bTickRan)
            {
                CLIENT_MainUpdate();
                bDidMainUpdate = 1;
                gdwLastGameTickTime += gnGameTickIntervalMs - nSinceLastTick;

                //  How many frames may be dropped before the next draw. The divide is unsigned (`xor edx, edx; div ecx`); every comparison after it
                //  is signed.
                const int32_t nTicksDue = nSinceLastTick / gnGameTickIntervalMs;
                int32_t nFrameBudget = (gnRecentTickRate < 7);
                if (nTicksDue > 1)
                    ++nFrameBudget;
                if (nTicksDue > 9)
                    ++nFrameBudget;
                if (nTicksDue > 19)
                    ++nFrameBudget;
                if (nTicksDue > 29)
                    ++nFrameBudget;
                if (gnRecentTickRate > 28)
                    nFrameBudget = 0;

                //  Over budget: reset the counter so this frame draws. Under budget the counter is left alone (`jle` past the store, 0x6FAAA14C),
                //  not set to 1.
                if (gnFramesSinceLastTick > nFrameBudget)
                    gnFramesSinceLastTick = 0;
            }
            else
            {
                //  The same `jle` at 0x6FAAA152, reached from 0x6FAAA167.
                if (gnRecentTickRate > 22)
                    gnFramesSinceLastTick = 0;
            }
        }
    }

    if (gbInGame)
    {
        PLAYER_ResetClickGuard();
        SELECT_UpdateSelectedUnit();

        if (D2SOUND_10032_IsStreamSamplePlaying())
        {
            D2SOUND_10029_FadeOutMasterVolume(180);
            D2SOUND_10031_StopStreamSample();
            D2SOUND_10027_ApplyMasterVolume(255);
        }

        if (gnFramesSinceLastTick)
        {
            //  This frame is dropped.
            if (bTickRan)
                ++gnSkippedTickCountInStatsWindow;
        }
        else if (CUNIT_GetPlayerRoom())
        {
            //  The draw. gpfnActiveMessageCallback is GAME_DrawGameScene (0x6FAA9640) while a game is running.
            (gpfnActiveMessageCallback)(0);
            ++gnTotalGameTicks;
            ++gnFrameCountInStatsWindow;
        }

        if (bDidMainUpdate)
            SOUNDHDR_PumpPreload();

        ++gnFramesSinceLastTick;
    }
    else
    {
        D2GFX_WINDOW_GetState();
    }

    //  The ordinary frame ends here, and a zero return keeps the pump running.
    if (!gbDisconnecting)
        return 0;

    //  Teardown: the one path that stops WNDPROC_WindowRunMessagePump.
    D2WIN_10177(0);
    UI_CloseAllPanels(1, 0);

    eD2GameTypes nGameType = gnGameType;
    if (nGameType == GAMETYPE_SINGLEPLAYER || nGameType == GAMETYPE_SINGLEPLAYER_UNCAPPED)
    {
        D2GAME_GAME_CloseAllGames();
        //  Re-read, because the binary re-reads: closing the games can change the type through the messages it processes.
        nGameType = gnGameType;
    }

    if (nGameType == GAMETYPE_OBNET_HOST || nGameType == GAMETYPE_LAN_HOST)
    {
        //  Stop the open-server thread GAME_AppModeGame started, and wait up to ten seconds for it. As there, EnterCriticalSection goes through the
        //  .idata slot and LeaveCriticalSection through its thunk.
        EnterCriticalSection(&gGameLoopCritSect);
        gbGameThreadStop = 1;
        LeaveCriticalSection(&gGameLoopCritSect);
        WaitForSingleObject(ghGameThread, 10000);
        ghGameThread = NULL;
    }

    return 1;
}
#pragma warning(pop)

//  The D2ConfigStrc fields GAME_ServerThreadProc hands Battle.net, at the offsets its `lea`s use, and its flag bytes' loads.
static_assert(offsetof(D2ConfigStrc, szGamePassword) == 0x23F
                  && offsetof(D2ConfigStrc, szGameStatstring) == 0x257
                  && offsetof(D2ConfigStrc, unpackedCTemp.nCharacterSaveFlags) == 0x1EA,
              "D2ConfigStrc: the fields the open-game refresh reads");

//1.10f: D2Client.0x6FAAA3B0
//  The open-game server thread. GAME_AppModeGame starts it with CreateThread for an open Battle.net or LAN host (GAMETYPE_OBNET_HOST,
//  GAMETYPE_LAN_HOST) and runs D2Game in it. Each pass takes gGameLoopCritSect only to read the stop and enable flags; when enabled it runs D2Game's
//  message processing and progress, and the clients' update when progress reported one.
//  bUpdated outlives the pass: it is set only while the loop is enabled, and a disabled pass sleeps 30 ms or 10 ms on the value the last enabled
//  pass left (ebp, set at 0x6FAAA3F2 and tested at 0x6FAAA403).
//  After a 30 ms pass it refreshes the open game on Battle.net, through the interface's function table (a struct of function pointers), whenever
//  the player count crosses gnMaxPlayerCountForGameRefresh (4 in the image, read once per pass; signed compares, `jge` 0x6FAAA432 and 0x6FAAA447,
//  `jl` 0x6FAAA436): down across it, slot +0x34 with the game's name, password and statstring and a flags byte, `__fastcall (char *, char *,
//  const char *, int)` (two registers and two pushes, 0x6FAAA47D..0x6FAAA48E); up across it, slot +0x90 with nothing, `__stdcall ()`. The flags:
//  bit 0 nArenaDifficulty == 1, bit 1 == 2, bit 2 CLIENTSAVEFLAG_HARDCORE, bit 3 CLIENTSAVEFLAG_EXPANSION, in the low byte of the character's
//  save flags.
DWORD __stdcall GAME_ServerThreadProc(LPVOID lpThreadParameter)
{
    UNREFERENCED_PARAMETER(lpThreadParameter);
    int32_t nPrevPlayerCount = 0;
    int32_t bUpdated = 0;

    for (;;)
    {
        EnterCriticalSection(&gGameLoopCritSect);
        const int32_t bStop = gbGameThreadStop;
        const int32_t bEnabled = gbGameLoopEnabled;
        LeaveCriticalSection(&gGameLoopCritSect);
        if (bStop)
            break;

        if (bEnabled)
        {
            D2GAME_GAME_ProcessNetworkMessages();
            bUpdated = D2GAME_GAME_UpdateGamesProgress(1);
            if (bUpdated)
                D2GAME_GAME_UpdateClients(0, 0);
        }

        if (!bUpdated)
        {
            Sleep(10);
            continue;
        }

        Sleep(30);
        struct BnClientInterface *pBnClient = WINMAIN_GetBnClientInterface();
        if (!pBnClient)
            continue;
        const int32_t nPlayerCount = D2GAME_GAME_GetPlayerUnitsCount(1);
        if (nPlayerCount == nPrevPlayerCount)
            continue;

        const int32_t nThreshold = gnMaxPlayerCountForGameRefresh;
        if (nPrevPlayerCount < nThreshold)
        {
            if (nPlayerCount >= nThreshold)
                (*(int32_t (__stdcall **)(void))((char *)pBnClient + 0x90))();
        }
        else if (nPlayerCount < nThreshold)
        {
            D2ConfigStrc *pConfig = gpLaunchConfig;
            const uint8_t nArenaDifficulty = pConfig->nArenaDifficulty;
            int32_t nGameFlags = nArenaDifficulty == 1;
            if (nArenaDifficulty == 2)
                nGameFlags |= 2;
            const uint16_t nSaveFlags = pConfig->unpackedCTemp.nCharacterSaveFlags;
            if (nSaveFlags & CLIENTSAVEFLAG_HARDCORE)
                nGameFlags |= 4;
            if (nSaveFlags & CLIENTSAVEFLAG_EXPANSION)
                nGameFlags |= 8;
            (*(int32_t (__fastcall **)(char *, char *, const char *, int32_t))((char *)pBnClient + 0x34))(pConfig->szGameName, pConfig->szGamePassword, pConfig->szGameStatstring, nGameFlags);
        }
        nPrevPlayerCount = nPlayerCount;
    }

    D2GAME_GAME_CloseAllGames();
    D2GAME_GAME_UpdateClients(0, 1);
    DeleteCriticalSection(&gGameLoopCritSect);
    OutputDebugStringA("Open server thread shut down\n");
    return 1;
}

//1.10f: D2Client.0x6FAAA4E0
//  The first node the dispatch tree reaches: WINMAIN_D2ClientEntrypoint hardcodes APPMODE_STARTUP and calls gafnAppModeEntryPoints[3].
//  The branch is always taken: gnAppModeAfterStartup is initialised to 5 (APPMODE_JOIN_GAME) in .data and nothing writes it, so this function
//  always enqueues APPMODE_GAME and always returns APPMODE_JOIN_GAME.
//  The original reloads the global after the call and so has two return statements; written as one.
D2C_AppMode __fastcall GAME_AppModeStartup()
{
    if (gnAppModeAfterStartup == APPMODE_JOIN_GAME)
    {
        WINMAIN_AppModeEnqueue(APPMODE_GAME);
    }

    return gnAppModeAfterStartup;
}

//  The beta slide show. GAME_AppModeGame runs GAME_BetaSlidesTick in the message pump with gBetaSlidesMessageHandlers registered, seven records:
//  WM_LBUTTONDOWN and WM_RBUTTONDOWN to GAME_BetaSlidesOnButtonDown, WM_KEYDOWN to GAME_BetaSlidesOnKeyDown, WM_ACTIVATEAPP to
//  GAME_WindowOnActivateApp, and VK_LWIN, VK_RWIN and VK_APPS to GAME_WindowConsumeMessage. The slides are gBetaScreens, ten 13-byte records,
//  "screen01".."screen10", each shown for ten seconds or until a click or a fresh key.
//  gnBetaScreenIndex is one byte: the bytes read and write its low byte alone (`inc byte ptr`, 0x6FAAA53D; `and eax, 0FFh` before every compare).
//  Promoted to int, it compares signed against gnBetaScreenCount as the `jge`/`jl` do.

//1.10f: D2Client.0x6FAAA500
//  A mouse button: next slide, the message consumed, and a tail jmp into STORM_SMsgBreakHandlerChain (0x6FAAA524).
void __stdcall GAME_BetaSlidesOnButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    ++gnBetaScreenIndex;
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAAA530
//  A key: the same, but only when lParam's bit 30, WM_KEYDOWN's previous key state, is clear (`test dword ptr [eax+0Ch], 40000000h`, 0x6FAAA534),
//  so auto-repeat does not skip slides.
void __stdcall GAME_BetaSlidesOnKeyDown(SMSGHANDLER_PARAMS *pMsg)
{
    if (!(pMsg->lParam & 0x40000000))
        ++gnBetaScreenIndex;
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAAA560
//  One pass of the pump. While the window is active and a frame can be started, the current slide is loaded on first sight
//  ("DATA\GLOBAL\ui\betascreens\<name>"; the load stamps the record's dwStartTick and loads the palette of its act through D2Win #10177) and drawn
//  as six 256-wide cels, frames 0..5 at (0,256) (256,256) (512,256) (0,480) (256,480) (512,480). If StartDraw fails the pass answers 0 at once,
//  before the timer.
//  Then the timer: a slide older than 10000 ms (`jbe` 0x6FAAA73C, unsigned) or an index already past the count advances the index; past the last
//  slide every loaded image is unloaded (the count re-read after each ARCHIVE_CellFileFree, 0x6FAAA777), the continue flag cleared and 1 answered,
//  which ends the pump. Index against count compares are signed (`jge` 0x6FAAA5A6 and 0x6FAAA71A, `jl` 0x6FAAA75F); the record index is read
//  afresh at each use, as the bytes reload it.
//  The record's byte at +8 is nPaletteAct: D2Win's #10177 takes an act (act + 1 into "palette\act%d"). All ten records hold 0, act 1's palette.
int32_t __stdcall GAME_BetaSlidesTick(int32_t nUnused)
{
    UNREFERENCED_PARAMETER(nUnused);
    D2BetaScreenStrc *pScreens = gBetaScreens;

    if (!D2GFX_WINDOW_GetState())
    {
        if (!D2GFX_StartDraw(1, 0, 0, 0))
            return 0;

        if (gnBetaScreenIndex < gnBetaScreenCount)
        {
            if (!pScreens[gnBetaScreenIndex].hImage)
            {
                char szPath[MAX_PATH];
                wsprintfA(szPath, "%s\\ui\\betascreens\\%s", "DATA\\GLOBAL", pScreens[gnBetaScreenIndex].szName);
                D2CellFileStrc *const hImage = ARCHIVE_CellFileLoad(szPath, 0);
                pScreens[gnBetaScreenIndex].hImage = hImage;
                D2BetaScreenStrc *pScreen = &pScreens[gnBetaScreenIndex];
                pScreen->dwStartTick = (int32_t)GetTickCount();
                D2WIN_10177(pScreen->nPaletteAct);
            }

            D2GfxDataStrc tCelData;
            memset(&tCelData, 0, sizeof(tCelData));
            tCelData.pCellFile = pScreens[gnBetaScreenIndex].hImage;
            tCelData.nDirection = 0;
            tCelData.nFrame = 0;
            D2GFX_TEXTURE_CelDraw(&tCelData, 0, 256, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 1;
            D2GFX_TEXTURE_CelDraw(&tCelData, 256, 256, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 2;
            D2GFX_TEXTURE_CelDraw(&tCelData, 512, 256, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 3;
            D2GFX_TEXTURE_CelDraw(&tCelData, 0, 480, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 4;
            D2GFX_TEXTURE_CelDraw(&tCelData, 256, 480, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 5;
            D2GFX_TEXTURE_CelDraw(&tCelData, 512, 480, 0xFFFFFFFF, 5, NULL);
        }
        D2GFX_EndScene();
    }

    int32_t nCount = gnBetaScreenCount;
    if (gnBetaScreenIndex < nCount)
    {
        if (GetTickCount() <= (uint32_t)pScreens[gnBetaScreenIndex].dwStartTick + 10000)
            return 0;
        nCount = gnBetaScreenCount;
    }

    ++gnBetaScreenIndex;
    if (gnBetaScreenIndex < nCount)
        return 0;

    for (int32_t i = 0; i < nCount; ++i)
    {
        if (pScreens[i].hImage)
        {
            ARCHIVE_CellFileFree(pScreens[i].hImage);
            nCount = gnBetaScreenCount;
            pScreens[i].hImage = NULL;
            pScreens[i].dwStartTick = 0;
        }
    }

    FOG_Trace("slide show end %d\n", GetTickCount());
    WINMAIN_SetAppModeContinue(FALSE);
    return 1;
}

//  The Diablo-vanquished screen. GAME_AppModeGame runs GAME_ShowDiabloVanquishedMessage in the message pump with gVanquishedMessageHandlers
//  registered, nine records: WM_LBUTTONDOWN and WM_RBUTTONDOWN to GAME_VanquishedOnButtonDown, WM_LBUTTONUP and WM_RBUTTONUP to
//  GAME_VanquishedOnButtonUp, WM_KEYDOWN to GAME_VanquishedOnKeyDown, and the slide show's last four: WM_ACTIVATEAPP to GAME_WindowOnActivateApp,
//  VK_LWIN, VK_RWIN and VK_APPS to GAME_WindowConsumeMessage.
//  The OK button is 96 by 32: its left edge at gnScreenWidth / 2 - 48, a signed division (`cdq; sub eax, edx; sar esi, 1`), and its bottom edge at
//  gnScreenHeight - 80. The hit test is inclusive on all four sides, signed (`jl`/`jg`, 0x6FAAA7F8..0x6FAAA80A), and reads the mouse x before the y.

//1.10f: D2Client.0x6FAAA7D0
//  A press: on the button it arms gbVanquishedButtonHeld; anywhere it clears the four click flags and consumes the message.
void __stdcall GAME_VanquishedOnButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    const int32_t nButtonBottom = gnScreenHeight - 80;
    const int32_t nButtonLeft = gnScreenWidth / 2 - 48;
    const int32_t nMouseX = CURSOR_GetMouseX();
    const int32_t nMouseY = CURSOR_GetMouseY();
    if (nMouseX >= nButtonLeft && nMouseX <= nButtonLeft + 96 && nMouseY >= nButtonBottom - 32 && nMouseY <= nButtonBottom)
    {
        gbVanquishedButtonHeld = 1;
    }
    gbLeftButtonDown = 0;
    gbRightButtonDown = 0;
    gbLeftClickIssuedThisFrame = 0;
    gbRightClickIssuedThisFrame = 0;
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAAA880
//  A release: the held state ends wherever it happens (0x6FAAA89A, before the mouse is read); on the button it dismisses the screen and leaves the
//  four click flags as they are, only a release off the button clears them. Both paths store lResult before bReturn.
void __stdcall GAME_VanquishedOnButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    const int32_t nButtonBottom = gnScreenHeight - 80;
    gbVanquishedButtonHeld = 0;
    const int32_t nButtonLeft = gnScreenWidth / 2 - 48;
    const int32_t nMouseX = CURSOR_GetMouseX();
    const int32_t nMouseY = CURSOR_GetMouseY();
    if (nMouseX >= nButtonLeft && nMouseX <= nButtonLeft + 96 && nMouseY >= nButtonBottom - 32 && nMouseY <= nButtonBottom)
    {
        gbVanquishedDismissed = 1;
    }
    else
    {
        gbLeftButtonDown = 0;
        gbRightButtonDown = 0;
        gbLeftClickIssuedThisFrame = 0;
        gbRightClickIssuedThisFrame = 0;
    }
    pMsg->lResult = 0;
    pMsg->bReturn = 1;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAAA920
//  Any key dismisses the screen, but not an auto-repeat: lParam's bit 30, the previous key state (0x6FAAA929).
void __stdcall GAME_VanquishedOnKeyDown(SMSGHANDLER_PARAMS *pMsg)
{
    if (!(pMsg->lParam & 0x40000000))
        gbVanquishedDismissed = 1;
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//  The compiler's `eh vector constructor iterator` (CRT_VectorConstructorIterator, 0x6FAAA950, ??_H) is not rewritten; its call sites are
//  reproduced by the declaration of the array, never by a call.

//1.10f: D2Client.0x6FAAA980
//  The screen, one pass of the pump per call; the pump's idle count in nIdlePass is never read. While the window is active and a frame starts:
//    - the backdrop is loaded on first sight, "endgame" for a classic game and "endgame2" for an expansion one, from "DATA\GLOBAL\ui\menu\", and
//      D2Win #10028 loads the palette of the same name ("DATA\GLOBAL\Palette\<name>\pal.dat" and "pal.pl2"); the OK button, "ui\menu\endgameok",
//      on first sight too;
//    - D2Win #10179 runs between filling the cel record and drawing it;
//    - the backdrop is twelve 256-wide cels in an expansion game, rows at y 256, 512 and 600, or eight in a classic one: x 0 and 256 with rows at
//      256 and gnScreenHeight - 1, and x gnScreenWidth / 2 and 256 past it with rows at 256 and gnScreenHeight;
//    - in font 3, colour 4, each line centred on gnScreenWidth / 2 by half its own width: "Congratulations!" (tbl 5098) at y 62, 100 in an
//      expansion game; "You Have Vanquished Diablo" (5099) at 107, classic only; "Will Henceforth Be Known As" (5100) at 232 or 272; the
//      character's name at 187; the title and the name at 277 or 357;
//    - the OK button at (gnScreenWidth / 2 - 48, gnScreenHeight - 80), frame 1 while held, and "OK" (5102) in font 1, colour 0, shifted 2 left and
//      2 down while held; then the cursor.
//  Every halving is a signed division, `cdq; sub; sar`, written `/ 2`.
//  Each fixed line is fetched twice: copied with Unicode::strcpy into a local only to be measured, then fetched again for the draw; the local is
//  not what is drawn (5098 at 0x6FAAAD78 and 0x6FAAADB9). The name and the title draw their locals.
//  The title comes from D2Win #10161 with the class, a progression value and the character's hardcore and expansion flags: by difficulty 4, 8 or
//  12, or 5, 10 or 15 for an expansion character (the classic and expansion encodings of "the last act of Normal / Nightmare / Hell completed").
//  Difficulty 3 and up draws no title. The expansion flag is #10161's fourth parameter (nExpansionFlag): D2Win takes the 5/10/15 arm when it is set
//  and hands it on to #10160, where it picks the Lord of Destruction titles. The title is copied, the name appended (strcat's inline form) and the
//  pair converted to Unicode, 64 wide.
//  The Unicode arrays: the original constructs a 100-element Unicode array six times (five times over one stack slot, before each measured line,
//  and once over the name's) through D2Lang's `??_FUnicode@@QAEXXZ`. Each is a block-scoped `Unicode ...[100];` here, which makes the compiler
//  construct it: the declaration is the reproduction, never a call. The first slot also holds the backdrop's name as bytes and the name's slot the
//  backdrop's path: separate buffers here. The title and name buffers are 100 bytes, the distance between their slots.
//  Once gbVanquishedDismissed is set both images are unloaded, the continue flag cleared and 1 answered, which ends the pump; this is checked even
//  while the window is inactive and nothing is drawn.
int32_t __stdcall GAME_ShowDiabloVanquishedMessage(int32_t nIdlePass)
{
    UNREFERENCED_PARAMETER(nIdlePass);

    if (!D2GFX_WINDOW_GetState())
    {
        if (!D2GFX_StartDraw(1, 0, 0, 0))
            return 0;

        if (!gpImgVanquishedScreen)
        {
            char szImage[16];
            strcpy(szImage, gbExpansionGame ? "endgame2" : "endgame");
            char szPath[MAX_PATH];
            sprintf(szPath, "%s\\ui\\menu\\%s", "DATA\\GLOBAL", szImage);
            gpImgVanquishedScreen = ARCHIVE_CellFileLoad(szPath, 0);
            char szPalDat[MAX_PATH];
            char szPalPl2[MAX_PATH];
            sprintf(szPalDat, "%s\\Palette\\%s\\pal.dat", "DATA\\GLOBAL", szImage);
            sprintf(szPalPl2, "%s\\Palette\\%s\\pal.pl2", "DATA\\GLOBAL", szImage);
            D2WIN_10028(szPalDat, szPalPl2);
        }
        if (!gpImgVanquishedOkButton)
        {
            char szPath[MAX_PATH];
            sprintf(szPath, "%s\\ui\\menu\\endgameok", "DATA\\GLOBAL");
            gpImgVanquishedOkButton = ARCHIVE_CellFileLoad(szPath, 0);
        }

        D2GfxDataStrc tCelData;
        memset(&tCelData, 0, sizeof(tCelData));
        tCelData.nDirection = 0;
        tCelData.pCellFile = gpImgVanquishedScreen;
        D2WIN_10179();
        tCelData.nFrame = 0;
        if (gbExpansionGame)
        {
            D2GFX_TEXTURE_CelDraw(&tCelData, 0, 256, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 1;
            D2GFX_TEXTURE_CelDraw(&tCelData, 256, 256, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 2;
            D2GFX_TEXTURE_CelDraw(&tCelData, 512, 256, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 3;
            D2GFX_TEXTURE_CelDraw(&tCelData, 768, 256, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 4;
            D2GFX_TEXTURE_CelDraw(&tCelData, 0, 512, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 5;
            D2GFX_TEXTURE_CelDraw(&tCelData, 256, 512, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 6;
            D2GFX_TEXTURE_CelDraw(&tCelData, 512, 512, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 7;
            D2GFX_TEXTURE_CelDraw(&tCelData, 768, 512, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 8;
            D2GFX_TEXTURE_CelDraw(&tCelData, 0, 600, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 9;
            D2GFX_TEXTURE_CelDraw(&tCelData, 256, 600, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 10;
            D2GFX_TEXTURE_CelDraw(&tCelData, 512, 600, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 11;
            D2GFX_TEXTURE_CelDraw(&tCelData, 768, 600, 0xFFFFFFFF, 5, NULL);
        }
        else
        {
            D2GFX_TEXTURE_CelDraw(&tCelData, 0, 256, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 1;
            D2GFX_TEXTURE_CelDraw(&tCelData, 256, 256, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 2;
            D2GFX_TEXTURE_CelDraw(&tCelData, 0, gnScreenHeight - 1, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 3;
            D2GFX_TEXTURE_CelDraw(&tCelData, 256, gnScreenHeight - 1, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 4;
            D2GFX_TEXTURE_CelDraw(&tCelData, gnScreenWidth / 2, 256, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 5;
            D2GFX_TEXTURE_CelDraw(&tCelData, gnScreenWidth / 2 + 256, 256, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 6;
            D2GFX_TEXTURE_CelDraw(&tCelData, gnScreenWidth / 2, gnScreenHeight, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 7;
            D2GFX_TEXTURE_CelDraw(&tCelData, gnScreenWidth / 2 + 256, gnScreenHeight, 0xFFFFFFFF, 5, NULL);
        }

        D2WIN_10127_SetFont(D2FONT_FONT42);
        {
            const int32_t nMidX = gnScreenWidth / 2;
            Unicode wszLine[100];
            Unicode::strcpy(wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_5098_Killdiablo1));
            const int32_t nX = nMidX - D2WIN_10121_GetTextWidth(wszLine) / 2;
            const int32_t nY = gbExpansionGame ? 100 : 62;
            D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_5098_Killdiablo1), nX, nY, 4, 0);
        }
        if (!gbExpansionGame)
        {
            const int32_t nMidX = gnScreenWidth / 2;
            Unicode wszLine[100];
            Unicode::strcpy(wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_5099_KillDiablo2));
            const int32_t nX = nMidX - D2WIN_10121_GetTextWidth(wszLine) / 2;
            D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_5099_KillDiablo2), nX, 107, 4, 0);
        }
        {
            const int32_t nY = gbExpansionGame ? 272 : 232;
            const int32_t nMidX = gnScreenWidth / 2;
            Unicode wszLine[100];
            Unicode::strcpy(wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_5100_KillDiablo3));
            const int32_t nX = nMidX - D2WIN_10121_GetTextWidth(wszLine) / 2;
            D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_5100_KillDiablo3), nX, nY, 4, 0);
        }

        D2ConfigStrc *pConfig = WINMAIN_GetConfig();
        char szCharName[100];
        {
            Unicode wszName[100];
            strcpy(szCharName, pConfig->szName);
            Unicode::toUnicode(wszName, szCharName, 64);
            const int32_t nHalfWidth = D2WIN_10121_GetTextWidth(wszName) / 2;
            D2WIN_10117_DrawText(wszName, gnScreenWidth / 2 - nHalfWidth, 187, 4, 0);
        }

        {
            const int32_t nDifficulty = gnDifficulty;
            const uint16_t nSaveFlags = pConfig->unpackedCTemp.nCharacterSaveFlags;
            const uint8_t nPlayerClass = gnPlayerClass;
            const int32_t nHardcoreFlag = nSaveFlags & CLIENTSAVEFLAG_HARDCORE;
            const int32_t nExpansionFlag = nSaveFlags & CLIENTSAVEFLAG_EXPANSION;
            Unicode wszTitle[100];
            const char *szTitle = NULL;
            switch (nDifficulty)
            {
            case 0:
                szTitle = D2WIN_10161(nPlayerClass, (nExpansionFlag != 0) + 4, nHardcoreFlag, nExpansionFlag);
                break;
            case 1:
                szTitle = D2WIN_10161(nPlayerClass, 2 * (nExpansionFlag != 0) + 8, nHardcoreFlag, nExpansionFlag);
                break;
            case 2:
                szTitle = D2WIN_10161(nPlayerClass, nExpansionFlag ? 15 : 12, nHardcoreFlag, nExpansionFlag);
                break;
            }
            if (szTitle)
            {
                char szTitleAndName[100];
                strcpy(szTitleAndName, szTitle);
                strcat(szTitleAndName, szCharName);
                Unicode::toUnicode(wszTitle, szTitleAndName, 64);
                const int32_t nHalfWidth = D2WIN_10121_GetTextWidth(wszTitle) / 2;
                const int32_t nX = gnScreenWidth / 2 - nHalfWidth;
                const int32_t nY = gbExpansionGame ? 357 : 277;
                D2WIN_10117_DrawText(wszTitle, nX, nY, 4, 0);
            }
        }

        D2WIN_10127_SetFont(D2FONT_FONT16);
        memset(&tCelData, 0, sizeof(tCelData));
        tCelData.pCellFile = gpImgVanquishedOkButton;
        tCelData.nFrame = gbVanquishedButtonHeld != 0;
        tCelData.nDirection = 0;
        D2GFX_TEXTURE_CelDraw(&tCelData, gnScreenWidth / 2 - 48, gnScreenHeight - 80, 0xFFFFFFFF, 5, NULL);
        {
            const int32_t nMidX = gnScreenWidth / 2;
            Unicode wszOk[100];
            Unicode::strcpy(wszOk, D2LANG_GetStringFromTblIndex(STR_IDX_5102_OK));
            const int32_t nTextLeft = nMidX - D2WIN_10121_GetTextWidth(wszOk) / 2;
            const int32_t nPress = gbVanquishedButtonHeld ? 2 : 0;
            D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_5102_OK), nTextLeft - nPress, gnScreenHeight + nPress - 88, 0, 0);
        }
        CURSOR_DrawCursorDefault();
        D2GFX_EndScene();
    }

    if (!gbVanquishedDismissed)
        return 0;
    if (gpImgVanquishedScreen)
    {
        ARCHIVE_CellFileFree(gpImgVanquishedScreen);
        gpImgVanquishedScreen = NULL;
    }
    if (gpImgVanquishedOkButton)
    {
        ARCHIVE_CellFileFree(gpImgVanquishedOkButton);
        gpImgVanquishedOkButton = NULL;
    }
    WINMAIN_SetAppModeContinue(FALSE);
    return 1;
}

//1.10f: D2Client.0x6FAAB1F0
//  Raises gbEndOfGamePending and writes (difficulty + 1) << 10 into the launch config's character save flags, over the progression field
//  (CLIENTSAVEFLAG_CHARACTER_PROGRESSION_MASK: bits 8..12, cleared by `and edx, 0E0FFh`, 0x6FAAB212). For difficulties 0..2 only bits 10 and 11
//  land, so the progression becomes (difficulty + 1) * 4, that is 4, 8 or 12, "act 4 of Normal / Nightmare / Hell completed" in the classic
//  encoding; 8 and 12 are the values GAME_AppModeGame's end-of-game test compares the field against. Called from the quest 23 handler in a game that
//  is not an expansion game, beside VIDEO_SetGameCompletedClassic (Diablo killed).
//  The arithmetic is the bytes': `movzx cx, byte ptr gnDifficulty`, `inc ecx`, `shl ecx, 0Ah`, and only cx stored: the sum is taken on the byte,
//  shifted, truncated to 16 bits. void: the caller does not use the pointer left in eax.
void __cdecl GAME_MarkClassicCompletion()
{
    const uint16_t nProgression = (uint16_t)((gnDifficulty + 1) << 10);
    D2ConfigStrc *pConfig = gpLaunchConfig;
    gbEndOfGamePending = 1;
    pConfig->unpackedCTemp.nCharacterSaveFlags = (uint16_t)(nProgression | (pConfig->unpackedCTemp.nCharacterSaveFlags & 0xE0FF));
}

//  `and edx, 0E0FFh` is the progression field cleared, and nothing else.
static_assert((0xFFFF & ~CLIENTSAVEFLAG_CHARACTER_PROGRESSION_MASK) == 0xE0FF,
              "D2ClientSaveFlags: the mask at 0x6FAAB212");

//1.10f: D2Client.0x6FAAB230
void __cdecl GAME_SetGameTeardownActive()
{
    gbGameTeardownActive = 1;
}

//1.10f: D2Client.0x6FAAB240
//  `mov gbDisconnecting, 1; retn`. void: no caller uses eax.
void __cdecl GAME_SetDisconnecting()
{
    gbDisconnecting = 1;
}

//  The four OS predicates. The original inlines each in both halves of the mouse-handler pair at the end of this file; three are called through
//  these out-of-line copies, and GAME_NeedsWin95WheelWorkaround's two copies stay written in place. All four gate on gbOsVersionKnown, whose one
//  writer stores 1 in GAME_AppModeGame once GetVersionExA has filled gOsVersionInfo.

//1.10f: D2Client.0x6FAAB250
//  Windows 95 exactly, with a wheel: platform 1 and minor version 0 (`setz al`, 0x6FAAB282).
//  DIFFERENCE: on NT the original loads dwMajorVersion and then answers 0 (0x6FAAB270, 0x6FAAB275); ours omits that dead load.
BOOL __cdecl GAME_NeedsWin95WheelWorkaround()
{
    return GetSystemMetrics(SM_MOUSEWHEELPRESENT) && gbOsVersionKnown && gOsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS && gOsVersionInfo.dwMinorVersion == 0;
}

//1.10f: D2Client.0x6FAAB290
//  A wheel, and an OS that sends WM_MOUSEWHEEL itself: platform 1 with a non-zero minor version (Windows 98 or ME, `setnz al`, 0x6FAAB2CF), or NT
//  with major version 4 or 5.
int32_t __cdecl GAME_HasNativeMouseWheel()
{
    if (GetSystemMetrics(SM_MOUSEWHEELPRESENT) && gbOsVersionKnown)
    {
        const OSVERSIONINFOA *pVersion = &gOsVersionInfo;
        if (pVersion->dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
            return pVersion->dwMinorVersion != 0;
        if (pVersion->dwPlatformId == VER_PLATFORM_WIN32_NT && (pVersion->dwMajorVersion == 4 || pVersion->dwMajorVersion == 5))
            return 1;
    }
    return 0;
}

//1.10f: D2Client.0x6FAAB2E0
//  Any platform-1 Windows, or NT 4, 2000 or XP. No wheel test.
BOOL __cdecl GAME_IsSupportedWindows()
{
    const OSVERSIONINFOA *pVersion = &gOsVersionInfo;
    if (!gbOsVersionKnown)
        return FALSE;
    if (pVersion->dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
        return TRUE;
    if (pVersion->dwPlatformId != VER_PLATFORM_WIN32_NT)
        return FALSE;
    return pVersion->dwMajorVersion == 4 || pVersion->dwMajorVersion == 5;
}

//1.10f: D2Client.0x6FAAB320
//  NT with major version 5, Windows 2000 or XP. The original compares major 4 first and fails on it (0x6FAAB339); that compare decides nothing.
BOOL __cdecl GAME_IsWin2000OrXp()
{
    return gbOsVersionKnown && gOsVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT && gOsVersionInfo.dwMajorVersion == 5;
}

//1.10f: D2Client.0x6FAAB350
void __fastcall GAME_SetD2xOutCinematicPending(BOOL bPending)
{
    gbD2xOutCinematicPending = bPending;
}

//1.10f: D2Client.0x6FAAB360
struct BnClientInterface *__cdecl GAME_GetComInterface()
{
    return gpComInterface;
}

//  Each table's extent is the count GAME_AppModeGame passes with it, and its size the item's size: 120, 84 and 108 bytes.
static_assert(sizeof(gGameWindowMessageHandlers) == 120,
              "gGameWindowMessageHandlers: 120 bytes, 0x6FB75818");
static_assert(ARRAYSIZE(gGameWindowMessageHandlers) == nGameWindowHandlers,
              "gGameWindowMessageHandlers: 10 records");
static_assert(sizeof(gBetaSlidesMessageHandlers) == 84,
              "gBetaSlidesMessageHandlers: 84 bytes, 0x6FB75920");
static_assert(ARRAYSIZE(gBetaSlidesMessageHandlers) == nBetaSlidesHandlers,
              "gBetaSlidesMessageHandlers: 7 records");
static_assert(sizeof(gVanquishedMessageHandlers) == 108,
              "gVanquishedMessageHandlers: 108 bytes, 0x6FB75978");
static_assert(ARRAYSIZE(gVanquishedMessageHandlers) == nVanquishedHandlers,
              "gVanquishedMessageHandlers: 9 records");

//1.10f: D2Client.0x6FAAB370
//  gafnAppModeEntryPoints[APPMODE_GAME], the node GAME_AppModeStartup enqueues, and the one that runs the game:
//  WNDPROC_WindowRunMessagePump(GAME_UpdateFrame). The original inlines GAME_UnregisterMouseInputHandlers four times in this function.
D2C_AppMode __fastcall GAME_AppModeGame()
{
    MSG Msg;

    if (!gnDisconnectCountdown)
        WINMAIN_SetAppModeContinue(TRUE);

    const uint32_t nEnterBegin = GetTickCount();
    FOG_Trace("[D2CLIENT]  Start entering at %d\n", nEnterBegin);

    gOsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    //  GetVersionExA is deprecated since Windows 8.1 and this project builds warnings as errors. It is not replaced: the point of the call is the OS
    //  version the original saw, and every mouse-handler predicate below branches on the dwPlatformId / dwMajorVersion it fills in.
#pragma warning(push)
#pragma warning(disable : 4996)
    if (GetVersionExA(&gOsVersionInfo))
        gbOsVersionKnown = 1;
#pragma warning(pop)

    VIEW_AllocGouraudTables();
    VIDEO_ResetCinematicState();
    WNDPROC_PushStormMessageState();
    CURSOR_Init(0);
    WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gGameWindowMessageHandlers, nGameWindowHandlers);
    GAME_RegisterMouseInputHandlers();

    if (gnGameType == GAMETYPE_BNET || gnGameType == GAMETYPE_OBNET_JOIN || gnGameType == GAMETYPE_LAN_JOIN)
    {
        MSG_Send_D2CLTSYS_JOINGAME(gnJoinGameId, gnJoinTokenId);
    }
    else
    {
        ARENA_GetArenaFlags();
        MSG_Send_D2CLTSYS_NEWGAME(szJoinGameName);

        if (gnGameType == GAMETYPE_OBNET_HOST || gnGameType == GAMETYPE_LAN_HOST)
        {
            gbGameThreadStop = 0;
            gbGameLoopEnabled = 0;
            InitializeCriticalSection(&gGameLoopCritSect);
            ghGameThread = CreateThread(NULL, 0, GAME_ServerThreadProc, NULL, 0, &gnGameThreadId);
        }
    }

    SOUNDHDR_Init();
    GAME_InitGame();

    if (RECORD_GetRecordingRequested() || RECORD_GetPlaybackRequested())
    {
        while (PeekMessageA(&Msg, NULL, 0, 0, PM_REMOVE))
            ;

        if (RECORD_GetPlaybackRequested())
            RECORD_SetRecordPlaybackLoaded();
        else if (RECORD_GetRecordingRequested())
            RECORD_InstallSaveOnCleanup();
    }

    FOG_SetMemoryPoolSystemMode(0, 0);

    if (gnGameType == GAMETYPE_OBNET_HOST || gnGameType == GAMETYPE_LAN_HOST)
    {
        //  EnterCriticalSection goes through the .idata slot (0x6FB6DF30), an indirect call, while LeaveCriticalSection below goes through its thunk.
        EnterCriticalSection(&gGameLoopCritSect);
        gbGameLoopEnabled = 1;
        LeaveCriticalSection(&gGameLoopCritSect);
    }

    const uint32_t nEnterElapsed = GetTickCount() - nEnterBegin;
    FOG_Trace("[D2CLIENT]  Finish entering at %d.  Elapsed: %d\n", GetTickCount(), nEnterElapsed);

    //  The main game loop.
    WNDPROC_WindowRunMessagePump(GAME_UpdateFrame);

    const uint32_t nCloseBegin = GetTickCount();
    FOG_Trace("[D2CLIENT]  Start closing at %d.\n", nCloseBegin);

    D2SOUND_10029_FadeOutMasterVolume(180);
    SOUNDHDR_StopAllPending();

    if (gnDisconnectCountdown && !gbEndOfGamePending && !gbGameTeardownActive)
    {
        WNDPROC_WindowRunMessagePump(GAME_DrawDisconnectMessage);
    }

    if (RECORD_GetRecordingRequested())
        RECORD_SaveToFile();

    if (gbBetaSlidesRegistered)
    {
        FOG_Trace("slide show start %d\n", GetTickCount());
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gGameWindowMessageHandlers, nGameWindowHandlers);
        GAME_UnregisterMouseInputHandlers();
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gBetaSlidesMessageHandlers, nBetaSlidesHandlers);
        WNDPROC_WindowRunMessagePump(GAME_BetaSlidesTick);
    }
    else if (gbEndOfGamePending)
    {
        VIDEO_PlayCinematic(5);
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gGameWindowMessageHandlers, nGameWindowHandlers);
        GAME_UnregisterMouseInputHandlers();
        WNDPROC_SetResolutionMode(D2GAMERES_640x480);
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gVanquishedMessageHandlers, nVanquishedHandlers);
        WNDPROC_WindowRunMessagePump(GAME_ShowDiabloVanquishedMessage);
    }
    else if (gbD2xOutCinematicPending)
    {
        VIDEO_PlayCinematic(7);
        gbGameTeardownActive = 1;
    }

    //  The block below is reached from all three arms above, including the ones that ran the slide show or the end-of-game cinematic: in the binary
    //  both of those end in a jump to it. bShowEndGameMenu is set to 1 here, not before the chain.
    //  gbGameTeardownActive is set independently of the two flags above (GAME_SetGameTeardownActive writes it, GAME_MarkClassicCompletion writes
    //  gbEndOfGamePending), so "slides or end-of-game AND teardown" is reachable, and in that state this block still writes nCharacterSaveFlags and
    //  shows the second vanquished screen.
    BOOL bShowEndGameMenu = TRUE;

    if (gbGameTeardownActive)
    {
        //  The difficulty / save-flag test, written as the branch lattice means rather than as its gotos read. gnDifficulty is compared as a byte in
        //  the binary.
        //
        //      difficulty 0    menu skipped when the field is 8 or 12
        //      difficulty 1    menu skipped when the field is 12
        //      difficulty 2+   menu always shown
        const int32_t nSaveFlagsHi = HIBYTE(gpLaunchConfig->unpackedCTemp.nCharacterSaveFlags) & 0x1F;
        const uint8_t nDifficulty = gnDifficulty;

        if (nDifficulty == 0)
            bShowEndGameMenu = (nSaveFlagsHi != 8 && nSaveFlagsHi != 12);
        else if (nDifficulty == 1)
            bShowEndGameMenu = (nSaveFlagsHi != 12);

        //  BUG: the field is the character's progression, bits 8..12, and this writes (difficulty << 2) + 5 into it: 5, 9 or 13. The expansion
        //  encoding of the last act completed is 5, 10 and 15, the classic one 4, 8 and 12. NOT ESTABLISHED: whether anything shows it.
        gpLaunchConfig->unpackedCTemp.nCharacterSaveFlags = (uint16_t)((gpLaunchConfig->unpackedCTemp.nCharacterSaveFlags & 0xE0FF) | ((nDifficulty << 10) + 1280));

        if (bShowEndGameMenu)
        {
            WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gGameWindowMessageHandlers, nGameWindowHandlers);
            GAME_UnregisterMouseInputHandlers();
            WNDPROC_SetResolutionMode(D2GAMERES_800x600);
            WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gVanquishedMessageHandlers, nVanquishedHandlers);
            WNDPROC_WindowRunMessagePump(GAME_ShowDiabloVanquishedMessage);
        }
    }

    FOG_Trace("game close start %d\n", GetTickCount());

    ENV_UnloadImages();
    PALSHIFT_Shutdown();
    CUNIT_DestroyAllUnits();

    if (gpAct)
        D2COMMON_DUNGEON_FreeAct(gpAct);
    gpAct = 0;

    D2CMP_SpriteFreeAsyncLoads();
    D2CMP_TileFreeAsyncLoads();
    MEMPART_DestroyCaches(FALSE);
    VIEW_Free(gpView);
    gpView = NULL;
    UI_ShutdownUiSubsystems();
    MEMPART_Nullsub1();
    DLIGHTMAP_FreeAll();
    ENV_Shutdown();
    FOG_DestroyMemoryPoolSystem(ghClientPoolSystem);

    //  The 128-byte client block (0x6FBA7750), zeroed by name.
    GAME_ClearClientGlobalData();

    FOG_Free(gpClientGameBlock, szSourceFile, 1422, 0);

    FOG_Trace("game close end %d\n", GetTickCount());

    SOUNDHDR_Shutdown();
    VIEW_FreeGouraudTables();
    CURSOR_UnloadCursors();
    D2WIN_10006_ClearDrawCaches();
    WINMAIN_AppModeEnqueue(APPMODE_CLOSEWINDOW);

    if (gbBetaSlidesRegistered)
    {
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gBetaSlidesMessageHandlers, nBetaSlidesHandlers);
        gbBetaSlidesRegistered = 0;
    }
    else if (gbEndOfGamePending)
    {
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gVanquishedMessageHandlers, nVanquishedHandlers);
        gbEndOfGamePending = 0;
    }
    else if (bShowEndGameMenu && gbGameTeardownActive)
    {
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gVanquishedMessageHandlers, nVanquishedHandlers);
        gbGameTeardownActive = 0;
    }
    else
    {
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gGameWindowMessageHandlers, nGameWindowHandlers);
        GAME_UnregisterMouseInputHandlers();
    }

    const uint32_t nCloseElapsed = GetTickCount() - nCloseBegin;
    FOG_Trace("[D2CLIENT]  Finish closing at %d.  Elapsed: %d\n", GetTickCount(), nCloseElapsed);

    if (gnGameType == GAMETYPE_BNET_BETA || gnGameType == GAMETYPE_OBNET_HOST || gnGameType == GAMETYPE_BNET || gnGameType == GAMETYPE_OBNET_JOIN)
    {
        //  The Battle.net interface's slot +0xC4, in a struct of function pointers (the leave-game call).
        if (gpComInterface)
            (*(void (**)(void))((char *)gpComInterface + 196))();
    }

    return APPMODE_LEAVE_GAME;
}

//1.10f: D2Client.0x6FAABB40
//  gafnAppModeEntryPoints[APPMODE_CLOSEWINDOW], the exact undo of GAME_AppModeGame's opening: that function allocates the gouraud tables, loads the
//  cursors and pushes the Storm message state; this releases them, plus the record buffers and the two roster lists. Returning APPMODE_SHOULDQUIT
//  is what ends WINMAIN_D2ClientEntrypoint's dispatch loop; it is the only handler that does.
D2C_AppMode __fastcall GAME_AppModeCloseWindow()
{
    VIEW_FreeGouraudTables();
    CURSOR_UnloadCursors();
    RECORD_FreeBufferList();
    ROSTER_FreeAll();
    ROSTERPETS_FreeAll();
    WNDPROC_PopStormMessageState();

    return APPMODE_SHOULDQUIT;
}

//  The launch-parameter appliers, 0x6FAABB70..0x6FAABF10: the handlers and the loop that runs them. Game.exe hands WINMAIN_D2ClientEntrypoint the
//  D2ConfigStrc it built from the command line; that hands it to GAME_ApplyLaunchParams, which walks gLaunchParamHandlers (48 slots) and calls every
//  non-null one with the block. All but two read a field of it, or several, and copy it into a global of the client's or hand it to the module that
//  owns it; slot 35's handler and GAME_NullFunc read nothing. Every handler returns void: the loop discards eax (`call eax` at 0x6FAABEFC).
//  The "0" test that opens four of them is `repe cmpsb` over two bytes against the literal "0" at 0x6FB74C04, MSVC's inline strcmp against a
//  one-character string, written here as the strcmp it is.
//  The D2ConfigStrc fields this block reads, at the offsets its instructions use. The struct is packed, which puts dwGamma at +0x0B and dwSeed at
//  +0x215 (unaligned dword loads, 0x6FAABD80 and 0x6FAABDC0).
static_assert(offsetof(D2ConfigStrc, bWindow) == 0x04
                  && offsetof(D2ConfigStrc, b3DFX) == 0x05
                  && offsetof(D2ConfigStrc, bOpenGL) == 0x06
                  && offsetof(D2ConfigStrc, bRave) == 0x07
                  && offsetof(D2ConfigStrc, bD3D) == 0x08
                  && offsetof(D2ConfigStrc, dwGamma) == 0x0B
                  && offsetof(D2ConfigStrc, bVSync) == 0x0F,
              "D2ConfigStrc: the display switches (0x6FAABD30..0x6FAABD90)");
static_assert(offsetof(D2ConfigStrc, dwGameType) == 0x14
                  && offsetof(D2ConfigStrc, wJoinID) == 0x18
                  && offsetof(D2ConfigStrc, szBattleNetIP) == 0x4A
                  && offsetof(D2ConfigStrc, szMCPIP) == 0x62
                  && offsetof(D2ConfigStrc, nTokenId) == 0x223,
              "D2ConfigStrc: the join fields (0x6FAABBF0..0x6FAABCA5)");
static_assert(offsetof(D2ConfigStrc, bAmazon) == 0x80
                  && offsetof(D2ConfigStrc, bPaladin) == 0x81
                  && offsetof(D2ConfigStrc, bSorceress) == 0x82
                  && offsetof(D2ConfigStrc, bNecromancer) == 0x83
                  && offsetof(D2ConfigStrc, bBarbarian) == 0x84
                  && offsetof(D2ConfigStrc, bDruid) == 0x85
                  && offsetof(D2ConfigStrc, bAssassin) == 0x86,
              "D2ConfigStrc: the seven class switches, in the order they are declared");
static_assert(offsetof(D2ConfigStrc, szName) == 0xB8
                  && offsetof(D2ConfigStrc, szRealm) == 0xD0
                  && offsetof(D2ConfigStrc, unpackedCTemp) == 0x1E8
                  && offsetof(D2ConfigStrc, dwAct) == 0x1FA
                  && offsetof(D2ConfigStrc, bNoCompress) == 0x201
                  && offsetof(D2ConfigStrc, bTxt) == 0x210
                  && offsetof(D2ConfigStrc, bLog) == 0x211
                  && offsetof(D2ConfigStrc, bNoSave) == 0x214
                  && offsetof(D2ConfigStrc, dwSeed) == 0x215
                  && offsetof(D2ConfigStrc, pComInterface) == 0x21F,
              "D2ConfigStrc: the remaining fields the appliers read");

//  The class ids the original stores as literals (0x6FAABCBA..0x6FAABD20), spelled by name. The switches are declared amazon, paladin, sorceress,
//  necromancer, barbarian, druid, assassin; the ids are not in that order.
static_assert(PCLASS_AMAZON == 0 && PCLASS_PALADIN == 3 && PCLASS_SORCERESS == 1
                  && PCLASS_NECROMANCER == 2 && PCLASS_BARBARIAN == 4
                  && PCLASS_DRUID == 5 && PCLASS_ASSASSIN == 6,
              "D2C_PlayerClasses: the seven stores of GAME_ApplyParamPlayerClass");

//  `cmp eax, 6` at 0x6FAABB9A, which the assert's own text spells.
static_assert(NUM_ACTS + 1 == 6, "D2C_Acts: GAME_ApplyParamStartAct's bound");

//1.10f: D2Client.0x6FAABB70
//  Slot 33. The switch turns saving off: `xor ecx, ecx` and a tail jmp into D2Game's PLRSAVE_EnableSaveFileWriting (0x6FAABB7A..0x6FAABB7C).
void __fastcall GAME_ApplyParamDisableSaving(D2ConfigStrc *pConfig)
{
    if (pConfig->bNoSave)
        D2GAME_10036_PLRSAVE_EnableSaveFileWriting(0);
}

//1.10f: D2Client.0x6FAABB90
//  Slot 30. dwAct is 1-based and D2Game's act is 0-based. The two asserts are the operands 0xD90 and 0xD91 (lines 3472 and 3473), and both tests are
//  signed (`jl` at 0x6FAABB9D, `jg` at 0x6FAABBBF): a negative act fails the second assert, not the first. So the bound is compared as an int.
void __fastcall GAME_ApplyParamStartAct(D2ConfigStrc *pConfig)
{
    if (pConfig->dwAct)
    {
        if (!(pConfig->dwAct < (int32_t)NUM_ACTS + 1))
        {
            FOG_DisplayAssert("ptParams->nAct < NUM_ACTS + 1", szSourceFile, 3472);
            exit(-1);
        }
        if (!(pConfig->dwAct > 0))
        {
            FOG_DisplayAssert("ptParams->nAct > 0", szSourceFile, 3473);
            exit(-1);
        }
        D2GAME_GAME_SetGlobalAct((D2C_Acts)(pConfig->dwAct - 1));
    }
}

//1.10f: D2Client.0x6FAABBF0
//  Slots 9 to 13 and 44: one handler six times. The server address is copied only when it is not "0"; the game name, the game type, the join id and
//  the join token are copied unconditionally. The join id is a WORD both ways (`mov dx, [ebx+18h]` / `mov word ptr gnJoinGameId, dx`,
//  0x6FAABC2C..0x6FAABC30).
//  BUG: the game name's copy is bounded only by the source's terminator.
void __fastcall GAME_ApplyParamJoinInfo(D2ConfigStrc *pConfig)
{
    if (strcmp(pConfig->szServerIP, "0"))
        lstrcpyA(gszServerIP, pConfig->szServerIP);
    lstrcpyA(szJoinGameName, pConfig->szGameName);
    gnGameType = pConfig->dwGameType;
    gnJoinGameId = pConfig->wJoinID;
    gnJoinTokenId = pConfig->nTokenId;
}

//1.10f: D2Client.0x6FAABC50
//  Slot 14, the Battle.net address.
//  BUG: the test reads szBattleNetIP (`lea esi, [eax+4Ah]`, 0x6FAABC59) and the copy takes szServerIP (`add eax, 32h`, 0x6FAABC69), so the
//  Battle.net address is never copied: when one is given, gszBattleNetIP receives whatever the game server's field holds, the buffer
//  MSG_AppModeJoinGame hands to D2Net's CLIENT_Initialize for GAMETYPE_BNET_INTERNAL. NOT ESTABLISHED: whether the game can show it;
//  gszBattleNetIP is read only by MSG_AppModeJoinGame's game-type-4 arm (0x6FAADD50), and the launcher that fills the config block is outside this
//  module.
void __fastcall GAME_ApplyParamBattleNetIp(D2ConfigStrc *pConfig)
{
    if (strcmp(pConfig->szBattleNetIP, "0"))
        lstrcpyA(gszBattleNetIP, pConfig->szServerIP);
}

//1.10f: D2Client.0x6FAABC80
//  Slot 15, the realm server's (MCP) address: the field it tests is the field it copies, unlike the slot before it. Nothing in D2Client reads what
//  it writes: gszMCPIP's one reference is this push (0x6FAABC9A).
void __fastcall GAME_ApplyParamRealmIp(D2ConfigStrc *pConfig)
{
    if (strcmp(pConfig->szMCPIP, "0"))
        lstrcpyA(gszMCPIP, pConfig->szMCPIP);
}

//1.10f: D2Client.0x6FAABCB0
//  Slots 16 to 20. Seven byte stores, one per switch, in the order the switches are declared; they are sequential tests, not a chain of else, so
//  when several are set the last one wins.
void __fastcall GAME_ApplyParamPlayerClass(D2ConfigStrc *pConfig)
{
    if (pConfig->bAmazon)
        gnPlayerClass = PCLASS_AMAZON;
    if (pConfig->bPaladin)
        gnPlayerClass = PCLASS_PALADIN;
    if (pConfig->bSorceress)
        gnPlayerClass = PCLASS_SORCERESS;
    if (pConfig->bNecromancer)
        gnPlayerClass = PCLASS_NECROMANCER;
    if (pConfig->bBarbarian)
        gnPlayerClass = PCLASS_BARBARIAN;
    if (pConfig->bDruid)
        gnPlayerClass = PCLASS_DRUID;
    if (pConfig->bAssassin)
        gnPlayerClass = PCLASS_ASSASSIN;
}

//1.10f: D2Client.0x6FAABD30
//  Slot 0. A tail jmp into DRAW_SelectGdiDisplay (0x6FAA2870), a function of its own that stores DISPLAYTYPE_GDI and raises gbAskedForGDI.
void __fastcall GAME_ApplyParamGDI(D2ConfigStrc *pConfig)
{
    if (pConfig->bWindow)
        DRAW_SelectGdiDisplay();
}

//1.10f: D2Client.0x6FAABD40
//  Slot 1.
void __fastcall GAME_ApplyParamGlide(D2ConfigStrc *pConfig)
{
    if (pConfig->b3DFX)
        DRAW_SetDisplayTypeGlide();
}

//1.10f: D2Client.0x6FAABD50
//  Slot 2.
void __fastcall GAME_ApplyParamOpenGL(D2ConfigStrc *pConfig)
{
    if (pConfig->bOpenGL)
        DRAW_SetDisplayTypeOpenGL();
}

//1.10f: D2Client.0x6FAABD60
//  Slot 3, and it reads +8 while slot 4 reads +7: the table runs Direct3D before Rave, the struct declares them the other way round.
void __fastcall GAME_ApplyParamDirect3D(D2ConfigStrc *pConfig)
{
    if (pConfig->bD3D)
        DRAW_SetDisplayTypeDirect3D();
}

//1.10f: D2Client.0x6FAABD70
//  Slot 4.
void __fastcall GAME_ApplyParamRave(D2ConfigStrc *pConfig)
{
    if (pConfig->bRave)
        DRAW_SetDisplayTypeRave();
}

//1.10f: D2Client.0x6FAABD80
//  Slot 6. The dword is loaded once, into ecx, and is both the test and the argument (`mov ecx, [ecx+0Bh]`, 0x6FAABD80).
void __fastcall GAME_ApplyParamGamma(D2ConfigStrc *pConfig)
{
    if (pConfig->dwGamma)
        DRAW_SetGamma(pConfig->dwGamma);
}

//1.10f: D2Client.0x6FAABD90
//  Slot 7.
void __fastcall GAME_ApplyParamVSync(D2ConfigStrc *pConfig)
{
    if (pConfig->bVSync)
        D2GFX_EnableVSync();
}

//1.10f: D2Client.0x6FAABDA0
//  Slot 31. `mov ecx, 1` and a tail jmp into D2Common #11102 (0x6FAABDAA).
void __fastcall GAME_ApplyParamEnableLog(D2ConfigStrc *pConfig)
{
    if (pConfig->bLog)
        D2COMMON_LOG_11102((void *)1);
}

//1.10f: D2Client.0x6FAABDC0
//  Slot 34.
void __fastcall GAME_ApplyParamInitSeed(D2ConfigStrc *pConfig)
{
    if (pConfig->dwSeed)
        D2GAME_GAME_SetInitSeed(pConfig->dwSeed);
}

//1.10f: D2Client.0x6FAABDE0
//  Slot 22. gLocalPlayerName is cleared by four dword stores (0x6FAABE01..0x6FAABE13) and then strncpy'd for strlen + 1 characters, at most 16,
//  compared signed (`cmp eax, 10h / jle`, 0x6FAABE1B). The four stores are written as the memset they compile from.
//  BUG: a name of 16 characters or more is copied as exactly 16 with no terminator, into a 16-byte buffer whose readers use it as a string. The
//  four bytes after the buffer (0x6FBA7820..0x6FBA7823) lie past .data's raw size and nothing refers to them, so a 16-character name reads as a
//  terminated string of 16; the damage is the length, one more than the field's 15: both join packets copy it unbounded into a 16-byte szClientName,
//  so its terminator lands one byte past the field. NOT ESTABLISHED: whether a name that long reaches szName, which the launcher fills.
void __fastcall GAME_ApplyParamCharacterName(D2ConfigStrc *pConfig)
{
    int32_t nCount;

    if (strcmp(pConfig->szName, "0"))
    {
        memset(gLocalPlayerName, 0, 16);
        nCount = (int32_t)strlen(pConfig->szName) + 1;
        if (nCount > 16)
            nCount = 16;
        strncpy(gLocalPlayerName, pConfig->szName, nCount);
    }
}

//1.10f: D2Client.0x6FAABE40
//  Slot 23, with no test of its own: SStrCopy of at most 32 characters (`push 20h`, 0x6FAABE46) from the config's realm field into gszRealmName.
void __fastcall GAME_ApplyParamRealm(D2ConfigStrc *pConfig)
{
    STORM_SStrCopy(gszRealmName, pConfig->szRealm, 32);
}

//1.10f: D2Client.0x6FAABE60
//  Slot 35, and it reads nothing: `xor ecx, ecx` and a tail jmp into ARENA_SetArenaFlag0 (0x6FAABE60..0x6FAABE62), whose zero arm sets bit 0 of
//  gdwArenaFlags (0x6FAA69D4). The table is static data, so this runs on every start.
void __fastcall GAME_ApplyParamSetArenaFlag0(D2ConfigStrc *pConfig)
{
    ARENA_SetArenaFlag0(0);
}

//1.10f: D2Client.0x6FAABE70
//  Slot 24. The byte at +0x1E8 zero-extended (`xor eax, eax / mov al, [ecx+1E8h]`) and tail-jumped into ARENA_SetArenaPlayerClass (0x6FAABE7A).
void __fastcall GAME_ApplyParamArenaPlayerClass(D2ConfigStrc *pConfig)
{
    ARENA_SetArenaPlayerClass(pConfig->unpackedCTemp.nUnk);
}

//1.10f: D2Client.0x6FAABE80
//  Slot 43.
void __fastcall GAME_ApplyParamComInterface(D2ConfigStrc *pConfig)
{
    if (pConfig->pComInterface)
        gpComInterface = pConfig->pComInterface;
}

//1.10f: D2Client.0x6FAABE90
//  One `retn`, and the do-nothing entry of more tables than this one: slots of gLaunchParamHandlers, the pfProcessCmd and pfProcessUnit slots of
//  SCmd.cpp's gSCmdTable, and CUnit.cpp's CUNIT_ForEachUnitInTable walks. Every one of those slots is `__fastcall` with its one or two arguments in
//  ecx and edx and nothing on the stack, so a callee that purges nothing serves them all; __cdecl is the convention that says "purges nothing".
void __cdecl GAME_NullFunc()
{
}

//1.10f: D2Client.0x6FAABEA0
//  Slot 46. The byte is zero-extended (`and eax, 0FFh`, 0x6FAABEAA) and pushed: D2CMP's SetCompressedDataMode takes a dword on the stack.
void __fastcall GAME_ApplyParamCompressedData(D2ConfigStrc *pConfig)
{
    if (pConfig->bNoCompress)
        D2CMP_SetCompressedDataMode(pConfig->bNoCompress);
}

//1.10f: D2Client.0x6FAABEC0
//  Slot 47. Unconditional, and inverted: the -txt switch SET passes 0 (`setz al`, 0x6FAABECA).
void __fastcall GAME_ApplyParamCompileTxt(D2ConfigStrc *pConfig)
{
    D2COMMON_DATATBLS_ToggleCompileTxtFlag(pConfig->bTxt == 0);
}


//1.10f: D2Client.0x6FAABEE0
//  The loop. The slot is loaded once per pass, tested and called with pConfig reloaded into ecx (`mov ecx, edi`, 0x6FAABEFA); the index is unsigned
//  (`jb`, 0x6FAABF07).
BOOL __fastcall GAME_ApplyLaunchParams(D2ConfigStrc *pConfig)
{
    gbLaunchParamsOk = TRUE;
    for (uint32_t i = 0; i < 48; ++i)
    {
        LaunchParamFn pfnHandler = gLaunchParamHandlers[i];
        if (pfnHandler)
            pfnHandler(pConfig);
    }
    return gbLaunchParamsOk;
}

//  The client's state accessors and its mouse-handler pair, 0x6FAABF20..0x6FAAC310: getters of one global each, setters, GAME_IsLadderGame,
//  GAME_ExtendCatchUpDeadline, the two resets of the mouse-click state, the pair that registers and unregisters the wheel and extra-button
//  handlers, and GAME_LeaveGameAfterCinematic.
//
//  The Windows constants the pair's tests spell, at the values the bytes compare: `push 4Bh` before each GetSystemMetrics (0x6FAAC0A7, 0x6FAAC1B7),
//  and the `dec eax / jz`, `dec eax / jnz` pairs that test dwPlatformId against 1 and 2 (0x6FAAC0C1..0x6FAAC0C5).
static_assert(SM_MOUSEWHEELPRESENT == 0x4B,
              "GetSystemMetrics index: `push 4Bh` at 0x6FAAC0A7");
static_assert(VER_PLATFORM_WIN32_WINDOWS == 1 && VER_PLATFORM_WIN32_NT == 2,
              "dwPlatformId: the dec/jz pairs at 0x6FAAC0C1..0x6FAAC0C5");

//  0x6FB7581C is gGameWindowMessageHandlers + 4 and 0x6FB758D0 is gWheelWin95Handler + 4: the same field of two different tables, which is the
//  whole of the unregister half's defect.
static_assert(offsetof(D2WinCommandRegisterStrc, nMessageOrKey) == 4,
              "D2WinCommandRegisterStrc: nMessageOrKey at +4");

//1.10f: D2Client.0x6FAABF20
//  Returns gbDisconnecting.
int32_t __cdecl GAME_IsDisconnecting()
{
    return gbDisconnecting;
}

//1.10f: D2Client.0x6FAABF30
//  Both modifier-flag words to 16 (the right one first, as the bytes store them), the four button flags to zero, and a tail jmp into
//  SELECT_ClearSelectedUnit (0x6FAABF55). void: no caller reads eax.
void __cdecl GAME_ResetClickState()
{
    gnRightClickModifierFlags = 16;
    gnLeftClickModifierFlags = 16;
    gbLeftButtonDown = 0;
    gbRightButtonDown = 0;
    gbLeftClickIssuedThisFrame = 0;
    gbRightClickIssuedThisFrame = 0;
    SELECT_ClearSelectedUnit();
}

//1.10f: D2Client.0x6FAABF60
//  GAME_ResetClickState's four button stores, in the same order, and nothing else. void: the zero in eax is the `xor eax, eax` made for the stores.
void __cdecl GAME_ResetMouseButtonState()
{
    gbLeftButtonDown = 0;
    gbRightButtonDown = 0;
    gbLeftClickIssuedThisFrame = 0;
    gbRightClickIssuedThisFrame = 0;
}

//1.10f: D2Client.0x6FAABF80
//  `mov eax, gnClientFrame / retn`: the frame counter, unsigned. Its callers store it, add to it or compare it unsigned; the exceptions compare it
//  signed: RECORD_ReadNextForFrame (the assert text casts the getter to int) and PANEL_GetSmoothedStatValue (a difference kept in an int).
uint32_t __cdecl GAME_GetFrameCount()
{
    return gnClientFrame;
}

//1.10f: D2Client.0x6FAABF90
uint32_t __cdecl GAME_GetFrameTick()
{
    return gdwFrameTick;
}

//1.10f: D2Client.0x6FAABFA0
uint32_t __cdecl GAME_GetInteractThrottle()
{
    return gnInteractThrottleTick;
}

//1.10f: D2Client.0x6FAABFB0
//  Stores gnInteractThrottleTick.
void __fastcall GAME_SetInteractThrottle(uint32_t dwTick)
{
    gnInteractThrottleTick = dwTick;
}

//1.10f: D2Client.0x6FAABFC0
int32_t __cdecl GAME_GetRecentTickRate()
{
    return gnRecentTickRate;
}

//1.10f: D2Client.0x6FAABFD0
int32_t __cdecl GAME_GetRecentSkipRate()
{
    return gnRecentSkipRate;
}

//1.10f: D2Client.0x6FAABFE0
//  `mov eax, dword ptr [0x6FBA7794]; retn`. The dword is in .bss and the only writes to it are the two `rep stosd` over the 128-byte client block
//  (0x6FAA9477, 0x6FAAB963), which write 0, so the answer is always 0 and its callers MISSILE_CltDoFunc1 and UNITMODE_StepUnitAlongPath always take
//  the branch 0 picks. UNRESOLVED_dword_6FBA7794 names the address and claims nothing about the value.
int32_t __cdecl UNRESOLVED_sub_6FAABFE0()
{
    return UNRESOLVED_dword_6FBA7794;
}

//1.10f: D2Client.0x6FAABFF0
eD2GameTypes __cdecl GAME_GetGameType()
{
    return gnGameType;
}

//1.10f: D2Client.0x6FAAC000
//  GetTickCount() + nExtraMs, kept only if it is later than the deadline already set: `jbe` at 0x6FAAC014 is unsigned, so the compare is DWORD
//  against DWORD. void: no caller reads the sum left in eax.
void __fastcall GAME_ExtendCatchUpDeadline(int32_t nExtraMs)
{
    const uint32_t dwDeadline = GetTickCount() + (uint32_t)nExtraMs;
    if (dwDeadline > gdwCatchUpResetDeadline)
        gdwCatchUpResetDeadline = dwDeadline;
}

//1.10f: D2Client.0x6FAAC020
//  Green blood is on (set by GAME_InitGame): PALSHIFT_GetUnitShiftTable takes the answer (0x6FAAE46D) to draw a LocalBlood 2 monster through its
//  green table.
int32_t __cdecl GAME_IsGreenBloodEnabled()
{
    return gbUseGreenBlood;
}

//1.10f: D2Client.0x6FAAC030
//  `mov byte ptr gnDifficulty, cl`: one byte, and the three above it are left as they are.
void __fastcall GAME_SetDifficulty(D2C_Difficulties nDifficulty)
{
    gnDifficulty = (uint8_t)nDifficulty;
}

//1.10f: D2Client.0x6FAAC040
//  `and ecx, 0FFh` before the store (0x6FAAC040): the callee widens its byte, which is what a uint8_t parameter assigned to a dword compiles to.
void __fastcall GAME_SetExpansionGame(uint8_t bExpansion)
{
    gbExpansionGame = bExpansion;
}

//1.10f: D2Client.0x6FAAC050
//  The whole ecx, unmasked (0x6FAAC050).
void __fastcall GAME_SetLadderGame(int32_t bLadder)
{
    gbLadderGame = bLadder;
}

//1.10f: D2Client.0x6FAAC060
//  1 unless the game type is 2 or 3 (GAMETYPE_BNET_BETA, GAMETYPE_BNET), and gbLadderGame inside that range. Both bounds are signed (`jl` at
//  0x6FAAC068, `jle` at 0x6FAAC072), so the compare is spelled on int32_t: a negative type would otherwise fail the upper bound instead of the
//  lower. Both return 1, so no value tells the two apart; the bytes' compare is written anyway. The original loads gbLadderGame between the two
//  tests (0x6FAAC06D), for nothing when the second fails.
int32_t __cdecl GAME_IsLadderGame()
{
    const int32_t nGameType = (int32_t)gnGameType;
    if (nGameType < (int32_t)GAMETYPE_BNET_BETA || nGameType > (int32_t)GAMETYPE_BNET)
        return 1;
    return gbLadderGame;
}

//1.10f: D2Client.0x6FAAC080
int32_t __cdecl GAME_IsExpansionGame()
{
    return gbExpansionGame;
}

//1.10f: D2Client.0x6FAAC090
//  `mov al, byte ptr gnDifficulty; retn`: al only, the rest of eax is the caller's. Read byte-wide as GAME_SetDifficulty writes it. The callers
//  widen al with `and eax, 0FFh` or test it unsigned (`jbe` at 0x6FB224E3), never `movsx`, so the type is unsigned.
uint8_t __cdecl GAME_GetDifficulty()
{
    return gnDifficulty;
}

//1.10f: D2Client.0x6FAAC0A0
//  The register half's mirror (below), with WNDPROC_UnregisterMessageHandlers and without the Win95 handshake: no RegisterWindowMessageA,
//  FindWindowA or SendMessageA here, so the id the register half obtained stays in gWheelWin95Handler[0].nMessageOrKey. Its out-of-line caller is
//  CONFIG_SuspendKeyCommands; the original also inlines it four times in GAME_AppModeGame.
//  The four blocks are the four OS predicates, inlined, each answer taken as a branch where the body returns it:
//      GAME_HasNativeMouseWheel        -> gWheelNativeHandler
//      GAME_IsSupportedWindows         -> gMButtonHandlers
//      GAME_IsWin2000OrXp              -> gXButtonHandlers
//      GAME_NeedsWin95WheelWorkaround  -> gWheelWin95Handler, but only when gGameWindowMessageHandlers[0]'s message is non-zero (the defect below)
//  DIFFERENCE: when a wheel is present and gbOsVersionKnown is 0, the original's first block jumps straight to the fourth (the `jz` at 0x6FAAC0B6 to
//  0x6FAAC160), skipping the second and third, which test the same flag and fail; ours calls those two predicates, which make the tests and fail on
//  them. GetSystemMetrics is called twice either way.
void __cdecl GAME_UnregisterMouseInputHandlers()
{
    const OSVERSIONINFOA *pVersion = &gOsVersionInfo;

    if (GAME_HasNativeMouseWheel())
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gWheelNativeHandler, 1);

    if (GAME_IsSupportedWindows())
    {
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gMButtonHandlers, 2);
    }

    if (GAME_IsWin2000OrXp())
    {
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gXButtonHandlers, 2);
    }

    if (GetSystemMetrics(SM_MOUSEWHEELPRESENT) && gbOsVersionKnown && pVersion->dwPlatformId == VER_PLATFORM_WIN32_WINDOWS && pVersion->dwMinorVersion == 0)
    {
        //  BUG: the guard reads the dword at 0x6FB7581C (operand bytes `a1 1c 58 b7 6f` at 0x6FAAC18C), which is
        //  gGameWindowMessageHandlers[0].nMessageOrKey: WM_LBUTTONDOWN (0x201) in the image, written by nothing. The id the register half keeps is
        //  gWheelWin95Handler[0].nMessageOrKey, 0x6FB758D0, stored at 0x6FAAC2D3. So the test is always true: wherever this block runs the table is
        //  unregistered whether or not it was ever registered, with the message still 0 when the driver was absent. INERT: Storm's
        //  SEvtUnregisterHandler (0x6FFC33C0) removes only an entry whose id and handler both match, and the register half registers this table only
        //  after storing a non-zero id, so an unregister with the id still 0 finds nothing, and one with the id set does what the intended test would
        //  have done. The inlined copies in GAME_AppModeGame carry the same read (0x6FAAB627, 0x6FAAB719, 0x6FAAB895, 0x6FAABAD9).
        if (gGameWindowMessageHandlers[0].nMessageOrKey)
            WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gWheelWin95Handler, 1);
    }
}

//1.10f: D2Client.0x6FAAC1B0
//  Registers the handlers the running Windows can deliver. The four OS predicates are inlined as in the unregister half (the same comparison and the
//  same difference, the `jz` at 0x6FAAC1C6 to 0x6FAAC270). Called by GAME_AppModeGame once the game starts and by CONFIG_SuspendKeyCommands after
//  its unregistering twin.
//  The Windows 95 block is the MSWheel driver's handshake. Windows 95 has no WM_MOUSEWHEEL; the driver runs a hidden window, class "MouseZ" and
//  title "Magellan MSWHEEL", and answers a registered message. The id is stored into the table only when the window exists, the id is non-zero and
//  the driver answers non-zero, tested in that order, hwnd before id (0x6FAAC2B9, 0x6FAAC2BD, 0x6FAAC2CD); the store comes before the
//  D2GFX_WINDOW_GetWindow call (0x6FAAC2D3, then 0x6FAAC2D9).
//  BUG: the message it registers is the wrong one. This function registers MSH_WHEELSUPPORT_MSG, the message to query for wheel support, and asks
//  the driver with it, correctly, and then hands the same id to GAME_WindowOnMouseWheel as the message to handle: a message sent to the driver's
//  window, not by it. The DLL holds no "MSWHEEL_ROLLMSG", so nothing in this module can register the roll message by name. NOT ESTABLISHED:
//  whether the block runs at all. Its first test is GetSystemMetrics(SM_MOUSEWHEELPRESENT) and it wants Windows 95 exactly, and the Windows SDK
//  defines SM_MOUSEWHEELPRESENT (75) only for targets newer than Windows 95. If Windows 95 answers 0 for metric 75, this block and its
//  unregistering twin are dead code; if it answers non-zero with the driver loaded, the handler waits for a message the driver is sent, not one
//  it sends. Only Windows 95 settles which.
void __cdecl GAME_RegisterMouseInputHandlers()
{
    const OSVERSIONINFOA *pVersion = &gOsVersionInfo;

    if (GAME_HasNativeMouseWheel())
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gWheelNativeHandler, 1);

    if (GAME_IsSupportedWindows())
    {
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gMButtonHandlers, 2);
    }

    if (GAME_IsWin2000OrXp())
    {
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gXButtonHandlers, 2);
    }

    if (GetSystemMetrics(SM_MOUSEWHEELPRESENT) && gbOsVersionKnown && pVersion->dwPlatformId == VER_PLATFORM_WIN32_WINDOWS && pVersion->dwMinorVersion == 0)
    {
        const uint32_t uWheelMessage = RegisterWindowMessageA("MSH_WHEELSUPPORT_MSG");
        const HWND hWheelWindow = FindWindowA("MouseZ", "Magellan MSWHEEL");
        if (hWheelWindow && uWheelMessage && SendMessageA(hWheelWindow, uWheelMessage, 0, 0))
        {
            gWheelWin95Handler[0].nMessageOrKey = uWheelMessage;
            WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gWheelWin95Handler, 1);
        }
    }
}

//1.10f: D2Client.0x6FAAC2F0
//  The leave that follows the ending cinematic. The continue flag goes first (`xor ecx, ecx`, 0x6FAAC2F0), then gbLeaveGameSent, the one-byte leave
//  packet, and gbDisconnecting last.
void __cdecl GAME_LeaveGameAfterCinematic()
{
    WINMAIN_SetAppModeContinue(FALSE);
    gbLeaveGameSent = 1;
    MSG_Send_D2CLTSYS_ENDGAME();
    gbDisconnecting = 1;
}

//  gGameData's container, 0x6FAAC320..0x6FAAD240: Storm's list and hash templates instantiated over SGAMEDATA. gGameData (0x6FBA7828, 104 bytes) is
//  Storm's TSExportTableSyncReuse over SGAMEDATA records, the RTTI name this code frees records with. The table is handed to D2Game's
//  GAME_InitGameDataTable (the reference at 0x6FAADDAB), so D2Game's own copies of these templates fill it; D2Client's are its constructor, its
//  destructors and the virtual slots its vtables carry. The table handed over is ours, built by sgGameDataLifetime; the virtual slots D2Game calls
//  on it (InternalNew at D2Game 0x6FC3B630) are the functions below, through gVtblSGameData2.
//
//  A TSLink is {pPrevLink, nNext}: the previous link, and the next object or, when nNext is negative, ~(the address of the list's terminator link),
//  which ends a walk. A TSExplicitList is {nLinkOffset, tSentinel}: where the link sits inside each object the list strings, and its terminator,
//  whose nNext is the head and whose pPrevLink is the tail's link. A node leaves a list by handing its pPrevLink to the next link and its nNext to
//  the previous one, and a list is empty when the terminator's nNext is not positive. The sign carries the encoding, so it holds only while every
//  object lies below 0x80000000, as in a process that is not large-address-aware.
//
//  A member that takes `this` alone is written `__fastcall` (the same register); one with stack arguments is `__fastcall` with an unused edx.
//
//  One SGAMEDATA record: the stride of GAME_SGameDataFreeRecordArray's walk (`add ebp, 1DE8h` at 0x6FAACF60).
static const uint32_t nSGameDataRecordSize = 7656;
//  The deleting destructors' free tag (0x6FB740E0).
static const char szDelete[] = "delete";
//  The offsets the list code below reads.
static_assert(offsetof(TSLink, pPrevLink) == 0 && offsetof(TSLink, nNext) == 4,
              "GAME_ListUnlinkNode: `mov esi, [ecx]` 0x6FAAD011, "
              "`mov edx, [ecx+4]` 0x6FAAD017");
static_assert(offsetof(TSExplicitList, nLinkOffset) == 0
              && offsetof(TSExplicitList, tSentinel) == 4,
              "GAME_ListGetLinkSlot: `mov eax, [ecx]` 0x6FAACE48, "
              "`lea eax, [ecx+4]` 0x6FAACE4F");
static_assert(offsetof(TSExplicitListArray, nCapacity) == 0
              && offsetof(TSExplicitListArray, nCount) == 4
              && offsetof(TSExplicitListArray, pLists) == 8,
              "GAME_SGameDataListDestroyAll, tBuckets at +14h: `mov [esi+14h], ebp` "
              "0x6FAAC766, `mov [esi+18h], ebp` 0x6FAAC769, `mov [esi+1Ch], ebp` "
              "0x6FAAC76C");
static_assert(offsetof(SGameDataChunkStrc, nCount) == 4
              && offsetof(SGameDataChunkStrc, pRecords) == 8
              && offsetof(SGameDataChunkStrc, tLink) == 0x10,
              "GAME_SGameDataFreeRecordArray: `mov eax, [ebx+4]` 0x6FAACF16, "
              "`mov esi, [ebx+8]` 0x6FAACF21, `mov esi, [ebx+10h]` 0x6FAACEE6");
static_assert(offsetof(SGameDataNodeStrc, tBucketLink) == 4
              && offsetof(SGameDataNodeStrc, tNodeLink) == 0xC,
              "GAME_SGameDataFreeRecordArray: `lea ecx, [esi+0Ch]` 0x6FAACF26, "
              "`add esi, 4` 0x6FAACF31");
static_assert(offsetof(SGameDataTableStrc, nBucketMask) == 0x24,
              "GAME_SGameDataListDestroyAll: `mov [esi+24h], ebp` 0x6FAAC72B");

//1.10f: D2Client.0x6FAAC450
//  Slot 0 of all four vtables (0x6FB6E23C, 0x6FB6E24C, 0x6FB6E25C, 0x6FB6E26C), TSHashTable::InternalDelete: the record's destructor,
//  GAME_SGameDataUnlinkNode, then the record freed under its RTTI tag. A member: GAME_SGameDataHashDestroyAll calls it through the vtable with the
//  table in ecx and the record pushed (0x6FAAC4AA..0x6FAAC4AD), and it reads only the record (`mov esi, [esp+4+...]`, 0x6FAAC451; `retn 4`).
void __fastcall GAME_SGameDataFreeNode(SGameDataTableStrc *pTable, int32_t nEdxUnused, SGameDataNodeStrc *pNode)
{
    GAME_SGameDataUnlinkNode(pNode);
    STORM_SMemFree(pNode, szSGameDataTag, -2, 0);
}

//1.10f: D2Client.0x6FAAC470
//  Slot 3 of gVtblSGameData1, 2 and 3 (0x6FB6E258, 0x6FB6E268, 0x6FB6E278): TSHashTableReuse::Destroy, Clear() and then Destructor(). Clear() is
//  TSHashTable::InternalClear(0): +0x10 cleared, the node list's nodes unlinked, and every bucket's records deleted through slot 0 of the table's
//  own vtable (`call dword ptr [edx]`, 0x6FAAC4AD), which in the live object, carrying gVtblSGameData2, is GAME_SGameDataFreeNode. Destructor()
//  returns every chunk (GAME_SGameDataFreeRecordArray, the chunk's tag) and every record on the reuse list (GAME_SGameDataUnlinkNode, the record's
//  tag); the bytes do not reset the chunk size (+0x34) to 16.
//  Every walk re-reads its head each pass (`jle`); the bucket walk re-reads pLists, the vtable and nCount, compares unsigned (`jb`, 0x6FAAC4BA) and
//  steps a byte offset (edi) by 12 beside its index, so the index is written. The node list's nodes go through a call to GAME_ListUnlinkNode each.
void __fastcall GAME_SGameDataHashDestroyAll(SGameDataTableStrc *pTable)
{
    int32_t nHead;
    uint32_t iBucket;

    pTable->nFullnessIndicator = 0;
    while ((nHead = pTable->tNodeList.tSentinel.nNext) > 0)
        GAME_ListUnlinkNode((TSLink *)(pTable->tNodeList.nLinkOffset + nHead));

    for (iBucket = 0; iBucket < pTable->tBuckets.nCount; ++iBucket)
    {
        while ((nHead = pTable->tBuckets.pLists[iBucket].tSentinel.nNext) > 0)
            (*(void(__thiscall **)(SGameDataTableStrc *, SGameDataNodeStrc *)) pTable->pVtbl)(pTable, (SGameDataNodeStrc *)nHead);
    }

    for (;;)
    {
        nHead = pTable->tChunkList.tSentinel.nNext;
        if (nHead <= 0)
            break;
        GAME_SGameDataFreeRecordArray((SGameDataChunkStrc *)nHead);
        STORM_SMemFree((void *)nHead, szSGameDataChunkTag, -2, 0);
    }

    for (;;)
    {
        nHead = pTable->tReuseList.tSentinel.nNext;
        if (nHead <= 0)
            break;
        GAME_SGameDataUnlinkNode((SGameDataNodeStrc *)nHead);
        STORM_SMemFree((void *)nHead, szSGameDataTag, -2, 0);
    }
}

//1.10f: D2Client.0x6FAAC500
//  Slot 1 of all four vtables (0x6FB6E240, 0x6FB6E250, 0x6FB6E260, 0x6FB6E270), TSHashTable::InternalNew: `listptr->NewNode(1, extrabytes, flags)`,
//  GAME_SGameDataAllocAndLinkNode with mode 1 (`push 1`, 0x6FAAC50E), the new record at the list's head. A member like slot 0: `this` in ecx,
//  unread, the three arguments on the stack (`retn 0Ch`). Nothing in D2Client calls it.
SGameDataNodeStrc *__fastcall GAME_SGameDataAllocNode(SGameDataTableStrc *pTable, int32_t nEdxUnused, TSExplicitList *pList, int32_t nExtraBytes, int32_t nAllocFlags)
{
    return GAME_SGameDataAllocAndLinkNode(pList, 0, 1, nExtraBytes, nAllocFlags);
}

//1.10f: D2Client.0x6FAAC520
//  ~TSHashTable, the destructor GAME_SGameDataVecDelDtor0 calls: gVtblSGameData0 stored, InternalClear(1) (+0x10 cleared, the node list's nodes
//  unlinked through GAME_ListGetLinkSlot and GAME_ListUnlinkNode, every bucket's nodes unlinked), then the members in reverse: the bucket array's
//  destructor and the node list's ~TSList. It takes the table, though its name says a list.
//  The original inlines GAME_ListUnlinkNode, GAME_SGameDataHashUnlinkListsAndFree (with one unwind-state store, 0x6FAAC5D8) and the list
//  terminator's unlink here.
//  Reached by nothing on a normal path: its other references are unwind-funclet jumps, and GAME_SGameDataVecDelDtor0 is itself unreached. Every
//  walk re-reads its head (`jle`); the bucket walks re-read pLists and nCount and compare unsigned (`jb`, 0x6FAAC5CF, 0x6FAAC5F4), a byte offset
//  stepped by 12 beside the index, written as the index. A bucket head whose link is not linked is skipped without being removed (`jz` back,
//  0x6FAAC599), and the node list's likewise (0x6FAAC62E), as in GAME_ListDestruct.
void __fastcall GAME_SGameDataListDestruct(SGameDataTableStrc *pTable)
{
    TSExplicitList *pNodeList;
    uint32_t iBucket;
    TSExplicitList *pBucket;
    int32_t nHead;
    TSLink *pLink;
    TSLink *pSentinel;

    pTable->pVtbl = &gVtblSGameData0;
    pTable->nFullnessIndicator = 0;
    pNodeList = &pTable->tNodeList;
    while (pNodeList->tSentinel.nNext > 0)
        GAME_ListUnlinkNode(GAME_ListGetLinkSlot(pNodeList, 0, (char *)pNodeList->tSentinel.nNext));

    for (iBucket = 0; iBucket < pTable->tBuckets.nCount; ++iBucket)
    {
        for (;;)
        {
            pBucket = &pTable->tBuckets.pLists[iBucket];
            nHead = pBucket->tSentinel.nNext;
            if (nHead <= 0)
                break;
            pLink = (TSLink *)(pBucket->nLinkOffset + nHead);
            GAME_ListUnlinkNode(pLink);
        }
    }

    GAME_SGameDataHashUnlinkListsAndFree(&pTable->tBuckets);

    pSentinel = &pNodeList->tSentinel;
    while ((nHead = pSentinel->nNext) > 0)
    {
        pLink = (TSLink *)(pNodeList->nLinkOffset + nHead);
        if (!pLink->pPrevLink)
            continue;
        GAME_ListNextLink(pLink, 0, -1)->pPrevLink = pLink->pPrevLink;
        pLink->pPrevLink->nNext = pLink->nNext;
        pLink->pPrevLink = NULL;
        pLink->nNext = 0;
    }

    GAME_ListUnlinkNode(pSentinel);
}

//1.10f: D2Client.0x6FAAC6A0
//  Slot 3 of gVtblSGameData0 (0x6FB6E248): TSHashTable::Destroy. InternalClear(1): +0x10 cleared, the node list's nodes unlinked by a call to
//  GAME_ListUnlinkNode each, every bucket's nodes unlinked (the original inlines GAME_ListUnlinkNode there, its tail duplicated into both arms,
//  0x6FAAC6EB and 0x6FAAC701), then +0x10 cleared again, the mask stored, and the bucket array's Clear(): every list destructed through
//  GAME_ListVecDelDtor with flag 0, the storage freed, and capacity, count and storage zeroed (0x6FAAC766..0x6FAAC76C); the grow quantum is kept.
//  BUG: the template's Destroy stores -1 in the mask, which its Initialized() reads as "never initialised"; the bytes store 0 (`mov [esi+24h], ebp`,
//  0x6FAAC72B), so by that test a table destroyed here counts as initialised with no buckets left. UNREACHABLE: nothing reaches it; gVtblSGameData0
//  is in the object only between two stores of the constructor (0x6FAAC366, 0x6FAAC3F9) and after the last vtable store of each destructor, and
//  neither stretch makes a virtual call. Written as the bytes have it.
void __fastcall GAME_SGameDataListDestroyAll(SGameDataTableStrc *pTable)
{
    int32_t nHead;
    uint32_t iBucket;
    TSExplicitList *pBucket;
    TSLink *pLink;
    TSExplicitList *pLists;

    pTable->nFullnessIndicator = 0;
    while ((nHead = pTable->tNodeList.tSentinel.nNext) > 0)
        GAME_ListUnlinkNode((TSLink *)(pTable->tNodeList.nLinkOffset + nHead));

    for (iBucket = 0; iBucket < pTable->tBuckets.nCount; ++iBucket)
    {
        for (;;)
        {
            pBucket = &pTable->tBuckets.pLists[iBucket];
            nHead = pBucket->tSentinel.nNext;
            if (nHead <= 0)
                break;
            pLink = (TSLink *)(pBucket->nLinkOffset + nHead);
            GAME_ListUnlinkNode(pLink);
        }
    }

    pTable->nFullnessIndicator = 0;
    //  The template stores -1; see the note above.
    pTable->nBucketMask = 0;

    for (iBucket = 0; iBucket < pTable->tBuckets.nCount; ++iBucket)
        GAME_ListVecDelDtor(&pTable->tBuckets.pLists[iBucket], 0, 0);
    pLists = pTable->tBuckets.pLists;
    if (pLists)
        STORM_SMemFree(pLists, szTSExplicitListSGameDataRawName, -2, 0);
    pTable->tBuckets.nCapacity = 0;
    pTable->tBuckets.nCount = 0;
    pTable->tBuckets.pLists = NULL;
}

//1.10f: D2Client.0x6FAAC780
//  The deleting destructor in slot 2 of gVtblSGameData2 (0x6FB6E264), the live table's: GAME_GameDataDestructTable, then the storage freed under
//  "delete" when bit 0 of the flag asks for it (`test [esp+4+arg_0], 1`, 0x6FAAC788). Returns the table. Nothing in D2Client calls it, and gGameData
//  is a static object that no delete may take.
SGameDataTableStrc *__fastcall GAME_SGameDataVecDelDtor2(SGameDataTableStrc *pTable, int32_t nEdxUnused, uint8_t bDelete)
{
    GAME_GameDataDestructTable(pTable);
    if ((bDelete & 1) != 0 && pTable)
        STORM_SMemFree(pTable, szDelete, -1, 0);
    return pTable;
}

//1.10f: D2Client.0x6FAAC7B0
//  The record's deleting destructor: ~TSHashObject (GAME_SGameDataUnlinkNode), then the record freed under "delete" when bit 0 of the flag asks for
//  it (`test [esp+4+bDelete], 1`, 0x6FAAC7B8). Its two callers, GAME_GameDataDestructTable (0x6FAA89D4) and GAME_SGameDataDestructor (0x6FAACD08),
//  pass 0 and free the record themselves under its RTTI tag.
SGameDataNodeStrc *__fastcall GAME_SGameDataNodeVecDelDtor(SGameDataNodeStrc *pNode, int32_t nEdxUnused, uint8_t bDelete)
{
    GAME_SGameDataUnlinkNode(pNode);
    if ((bDelete & 1) != 0 && pNode)
        STORM_SMemFree(pNode, szDelete, -1, 0);
    return pNode;
}

//1.10f: D2Client.0x6FAAC7E0
//  The deleting destructor in slot 2 of gVtblSGameData1 (0x6FB6E254): GAME_SGameDataDestructHashTable, then the storage freed under "delete" on
//  bit 0 (`test [esp+4+arg_0], 1`, 0x6FAAC7E8). Returns the table. Unreached: gVtblSGameData1 is in the object only while it is built (0x6FAAC3F9 to
//  0x6FAAC418) and at the head of the three destructors that store it (0x6FAA899C, 0x6FAAC832, 0x6FAACCD0), where no virtual call is made.
SGameDataTableStrc *__fastcall GAME_SGameDataVecDelDtor1(SGameDataTableStrc *pTable, int32_t nEdxUnused, uint8_t bDelete)
{
    GAME_SGameDataDestructHashTable(pTable);
    if ((bDelete & 1) != 0 && pTable)
        STORM_SMemFree(pTable, szDelete, -1, 0);
    return pTable;
}

//1.10f: D2Client.0x6FAAC810
//  ~TSHashTableReuse, the destructor GAME_SGameDataVecDelDtor1 calls, with ~TSHashTable written out under it: GAME_SGameDataDestructor's body,
//  access for access, but for the reuse list's records, whose destructor is a call to GAME_SGameDataUnlinkNode (0x6FAAC869) where
//  GAME_SGameDataDestructor calls GAME_SGameDataNodeVecDelDtor with flag 0 (the same ~TSHashObject either way). The walks and the two list
//  teardowns are as GAME_SGameDataDestructor describes; unreached, as GAME_SGameDataVecDelDtor1 is.
void __fastcall GAME_SGameDataDestructHashTable(SGameDataTableStrc *pTable)
{
    int32_t nHead;
    TSLink *pSentinel;
    TSExplicitList *pBucket;
    uint32_t iBucket;

    pTable->pVtbl = &gVtblSGameData1;

    for (;;)
    {
        nHead = pTable->tChunkList.tSentinel.nNext;
        if (nHead <= 0)
            break;
        GAME_SGameDataFreeRecordArray((SGameDataChunkStrc *)nHead);
        STORM_SMemFree((void *)nHead, szSGameDataChunkTag, -2, 0);
    }

    for (;;)
    {
        nHead = pTable->tReuseList.tSentinel.nNext;
        if (nHead <= 0)
            break;
        GAME_SGameDataUnlinkNode((SGameDataNodeStrc *)nHead);
        STORM_SMemFree((void *)nHead, szSGameDataTag, -2, 0);
    }

    pSentinel = &pTable->tChunkList.tSentinel;
    while ((nHead = pSentinel->nNext) > 0)
        GAME_ListUnlinkNode((TSLink *)(pTable->tChunkList.nLinkOffset + nHead));
    if (pSentinel->pPrevLink)
    {
        GAME_ListNextLink(pSentinel, 0, -1)->pPrevLink = pSentinel->pPrevLink;
        pSentinel->pPrevLink->nNext = pSentinel->nNext;
        pSentinel->pPrevLink = NULL;
        pSentinel->nNext = 0;
    }

    pSentinel = &pTable->tReuseList.tSentinel;
    while ((nHead = pSentinel->nNext) > 0)
        GAME_ListUnlinkNode((TSLink *)(pTable->tReuseList.nLinkOffset + nHead));
    if (pSentinel->pPrevLink)
    {
        GAME_ListNextLink(pSentinel, 0, -1)->pPrevLink = pSentinel->pPrevLink;
        pSentinel->pPrevLink->nNext = pSentinel->nNext;
        pSentinel->pPrevLink = NULL;
        pSentinel->nNext = 0;
    }

    pTable->pVtbl = &gVtblSGameData0;
    pTable->nFullnessIndicator = 0;
    GAME_ListUnlinkAllNodes(&pTable->tNodeList);

    for (iBucket = 0; iBucket < pTable->tBuckets.nCount; ++iBucket)
    {
        for (;;)
        {
            pBucket = &pTable->tBuckets.pLists[iBucket];
            if (pBucket->tSentinel.nNext <= 0)
                break;
            GAME_ListUnlinkNode(GAME_ListGetLinkSlot(pBucket, 0, (char *)pBucket->tSentinel.nNext));
        }
    }

    GAME_SGameDataHashUnlinkListsAndFree(&pTable->tBuckets);
    GAME_ListUnlinkAllNodes(&pTable->tNodeList);
    GAME_ListUnlinkNode(&pTable->tNodeList.tSentinel);
}

//1.10f: D2Client.0x6FAAC9A0
//  The deleting destructor in slot 2 of gVtblSGameData0 (0x6FB6E244): GAME_SGameDataListDestruct, then the storage freed under "delete" on bit 0
//  (`test [esp+4+arg_0], 1`, 0x6FAAC9A8). Returns the table. Unreached, for the reason GAME_SGameDataListDestroyAll gives.
SGameDataTableStrc *__fastcall GAME_SGameDataVecDelDtor0(SGameDataTableStrc *pTable, int32_t nEdxUnused, uint8_t bDelete)
{
    GAME_SGameDataListDestruct(pTable);
    if ((bDelete & 1) != 0 && pTable)
        STORM_SMemFree(pTable, szDelete, -1, 0);
    return pTable;
}

//1.10f: D2Client.0x6FAAC9D0
//  The bucket array's destructor, ~TSFixedArray over TSExplicitList: every list destructed, then the storage freed under the list's RTTI tag
//  (0x6FAACA5B). Each list's ~TSList is written out around calls: the heads unlinked through GAME_ListNodeToLink and GAME_ListUnlinkNode while the
//  terminator's nNext is positive (`jle`, 0x6FAACA14), then the terminator's own link through GAME_ListUnlinkNode.
//  GAME_SGameDataHashUnlinkListsAndFree, below, is the same destructor with the list's left a call to GAME_ListDestruct.
//  Nothing calls it: its references are jumps from unwind funclets (0x6FB6C0E5 .. 0x6FB6C22F), the bucket array at +0x14 destroyed when a C++
//  exception leaves a constructor or destructor of the table, an unwind the C does not have.
//  The walk re-reads pLists every pass (0x6FAAC9FB) and compares its index unsigned against nCount re-read (`jb`, 0x6FAACA46). The bytes step a
//  byte offset (ebp) by 12 beside the index, sizeof(TSExplicitList), so the index is written.
void __fastcall GAME_SGameDataHashUnlinkNodesAndFree(TSExplicitListArray *pArray)
{
    uint32_t iBucket;
    TSExplicitList *pBucket;
    int32_t nHead;
    TSExplicitList *pLists;

    for (iBucket = 0; iBucket < pArray->nCount; ++iBucket)
    {
        pBucket = &pArray->pLists[iBucket];
        while ((nHead = pBucket->tSentinel.nNext) > 0)
            GAME_ListUnlinkNode(GAME_ListNodeToLink((char *)nHead, pBucket->nLinkOffset));
        GAME_ListUnlinkNode(&pBucket->tSentinel);
    }

    pLists = pArray->pLists;
    if (pLists)
        STORM_SMemFree(pLists, szTSExplicitListSGameDataRawName, -2, 0);
}

//1.10f: D2Client.0x6FAACA80
//  ~TSHashObject: its two TSLink members destroyed in reverse order, tNodeLink (+0x0C) and then tBucketLink (+0x04), each by TSLink's destructor,
//  which unlinks. The original inlines GAME_ListUnlinkNode for both, the second without the push and pop of a scratch register that the first and
//  GAME_ListUnlinkNode carry (0x6FAACA96, 0x6FAACA9E; 0x6FAAD022, 0x6FAAD02A). The record is not freed.
void __fastcall GAME_SGameDataUnlinkNode(SGameDataNodeStrc *pNode)
{
    GAME_ListUnlinkNode(&pNode->tNodeLink);

    GAME_ListUnlinkNode(&pNode->tBucketLink);
}

//1.10f: D2Client.0x6FAACAF0
//  TSList::NewNode, with TSList::LinkNode under it: SMemAlloc of the record's 7656 bytes plus nExtraBytes under the record's RTTI tag, bit 3 of the
//  flags forced on (`or al, 8`, 0x6FAACAFD), which makes Storm's allocator zero-fill (SMemZero), so the whole record comes back zeroed; then
//  SGAMEDATA's constructor, the TSHashObject's two TSLinks cleared (0x6FAACB1A..0x6FAACB23); then, for a non-zero eLinkMode, the node linked
//  against the terminator (LinkNode with no existing node). Its caller, GAME_SGameDataAllocNode (slot 1 of all four vtables), passes 1.
//  The original inlines GAME_ListUnlinkNode and GAME_ListNextLink here.
//  eLinkMode 1 links after the terminator, at the head: the new link takes the terminator as its previous link and the old head as its next, and
//  the old head's link takes it as its previous. Any other non-zero mode links before the terminator, at the tail (`dec esi / jz`, 0x6FAACB6F); the
//  bytes have no third case.
//  BUG: with no node (the allocation-failure arm) the terminator is unlinked from its own list and linked back as if it were the node; its links
//  are zero after the unlink, so the head insert stores through address nLinkOffset (0x6FAACBB4; 4 for a bucket) and the tail insert reads address 4
//  (0x6FAACB76), an access violation, not a NULL return. UNREACHABLE: Storm's SMemAlloc never returns NULL; its failure (Storm 0x6FFCB4E4) calls
//  Storm 0x6FFCB280, which is SErrDisplayError and then ExitProcess(1). Written as the bytes have it.
SGameDataNodeStrc *__fastcall GAME_SGameDataAllocAndLinkNode(TSExplicitList *pList, int32_t nEdxUnused, int32_t eLinkMode, int32_t nExtraBytes, int32_t nAllocFlags)
{
    SGameDataNodeStrc *pNode;
    TSLink *pLink;
    TSLink *pSentinel;
    TSLink *pTailLink;

    pNode = (SGameDataNodeStrc *)STORM_SMemAlloc(nExtraBytes + nSGameDataRecordSize, szSGameDataTag, -2, nAllocFlags | 8);
    if (pNode)
    {
        pNode->tBucketLink.pPrevLink = NULL;
        pNode->tBucketLink.nNext = 0;
        pNode->tNodeLink.pPrevLink = NULL;
        pNode->tNodeLink.nNext = 0;
    }
    else
    {
        //  `xor eax, eax` at 0x6FAACB28.
        pNode = NULL;
    }

    if (!eLinkMode)
        return pNode;

    //  NULL takes the terminator itself: the arm noted above.
    if (pNode)
        pLink = (TSLink *)((char *)pNode + pList->nLinkOffset);
    else
        pLink = &pList->tSentinel;

    GAME_ListUnlinkNode(pLink);

    pSentinel = &pList->tSentinel;
    if (eLinkMode == 1)
    {
        pLink->pPrevLink = pSentinel;
        pLink->nNext = pSentinel->nNext;

        GAME_ListNextLink(pSentinel, 0, pList->nLinkOffset)->pPrevLink = pLink;
        pSentinel->nNext = (int32_t)pNode;
    }
    else
    {
        pTailLink = pSentinel->pPrevLink;
        pLink->pPrevLink = pTailLink;
        pLink->nNext = pTailLink->nNext;
        pTailLink->nNext = (int32_t)pNode;
        pSentinel->pPrevLink = pLink;
    }
    return pNode;
}

//1.10f: D2Client.0x6FAACBC0
//  The bucket array's destructor again (GAME_SGameDataHashUnlinkNodesAndFree, above), each list's destructor this time a call to GAME_ListDestruct
//  (0x6FAACBD5), then the storage freed under the list's tag. It re-reads pLists and nCount every pass and compares unsigned (`jb`, 0x6FAACBE3); the
//  bytes step a byte offset (ebx) by 12 beside the index (edi), so the index is written. Called by the table's destructors for the bucket array at
//  +0x14.
void __fastcall GAME_SGameDataHashUnlinkListsAndFree(TSExplicitListArray *pArray)
{
    uint32_t iBucket;
    TSExplicitList *pLists;

    for (iBucket = 0; iBucket < pArray->nCount; ++iBucket)
        GAME_ListDestruct(&pArray->pLists[iBucket]);

    pLists = pArray->pLists;
    if (pLists)
        STORM_SMemFree(pLists, szTSExplicitListSGameDataRawName, -2, 0);
}

//1.10f: D2Client.0x6FAACC00
//  ~TSList: every node unlinked, then the terminator's own link (TSLink's destructor unlinks, and the terminator is a TSLink member). The original
//  inlines both halves, GAME_ListUnlinkAllNodes and GAME_ListUnlinkNode. GAME_ListVecDelDtor, the deleting destructor, calls it (0x6FAACEB3), and
//  CRT_LISTARRAY_SetCount runs it on every list a shrink drops (0x6FAAD1E2).
//  A head whose link is not linked is skipped without being removed (`jz` back to the head's re-read, 0x6FAACC15), so the walk would spin on it; a
//  consistent list never has one, its head is linked by definition.
void __fastcall GAME_ListDestruct(TSExplicitList *pList)
{
    TSLink *pTerminator;

    pTerminator = &pList->tSentinel;

    GAME_ListUnlinkAllNodes(pList);

    GAME_ListUnlinkNode(pTerminator);
}

//1.10f: D2Client.0x6FAACC80
//  The deleting destructor in slot 2 of gVtblSGameData3 (0x6FB6E274), as GAME_ListVecDelDtor is the list's: GAME_SGameDataDestructor, then the
//  storage freed under "delete" when bit 0 of the flag asks for it (`test [esp+4+arg_0], 1`, 0x6FAACC88). Returns the table.
//  Nothing reaches it: each of the four vtables carries its own deleting destructor in slot 2 (GAME_SGameDataVecDelDtor0..3), and gVtblSGameData3
//  is referenced once, by the store at 0x6FAAC418 in the constructor, which 0x6FAAC432 overwrites with gVtblSGameData2 one InitializeCriticalSection
//  later.
SGameDataTableStrc *__fastcall GAME_SGameDataVecDelDtor3(SGameDataTableStrc *pTable, int32_t nEdxUnused, uint8_t bDelete)
{
    GAME_SGameDataDestructor(pTable);
    if ((bDelete & 1) != 0 && pTable)
        STORM_SMemFree(pTable, szDelete, -1, 0);
    return pTable;
}

//1.10f: D2Client.0x6FAACCB0
//  ~TSExportTableSimpleReuse, the destructor gVtblSGameData3's deleting destructor calls, with the destructors under it written out. Its first store
//  is gVtblSGameData1 (0x6FAACCD0), TSHashTableReuse's table; there is no store of gVtblSGameData3 in it.
//  ~TSHashTableReuse: Destructor(), which clears the chunk list (each chunk destructed by GAME_SGameDataFreeRecordArray and returned under its tag)
//  and then tReuseList (each record's destructor run, GAME_SGameDataNodeVecDelDtor with flag 0, and the record returned). Then its two lists in
//  reverse, tChunkList's nodes unlinked and its terminator, then tReuseList's. The bytes do not reset the chunk size (+0x34) to 16.
//  ~TSHashTable, from the store of gVtblSGameData0 (0x6FAACD8F): InternalClear(1) (+0x10 cleared, the node list's nodes unlinked by
//  GAME_ListUnlinkAllNodes, every bucket's nodes unlinked), then its members in reverse, the bucket array (GAME_SGameDataHashUnlinkListsAndFree)
//  and the node list, its nodes and its terminator.
//  Every walk re-reads its head each pass, `jle` on the signed head; the bucket walk re-reads pLists each pass and its bound is `jb` on nCount
//  re-read (0x6FAACDD7), with a byte offset (edi) stepped by 12 beside the index (ebp), so the index is written. GAME_GameDataDestructTable carries
//  this whole body inline after its DeleteCriticalSection; in the C it calls this function there, so this runs whenever that one does. In the binary
//  this function is reached only through GAME_SGameDataVecDelDtor3, so nothing reaches it either.
void __fastcall GAME_SGameDataDestructor(SGameDataTableStrc *pTable)
{
    int32_t nHead;
    TSLink *pSentinel;
    TSExplicitList *pBucket;
    uint32_t iBucket;

    pTable->pVtbl = &gVtblSGameData1;

    for (;;)
    {
        nHead = pTable->tChunkList.tSentinel.nNext;
        if (nHead <= 0)
            break;
        GAME_SGameDataFreeRecordArray((SGameDataChunkStrc *)nHead);
        STORM_SMemFree((void *)nHead, szSGameDataChunkTag, -2, 0);
    }

    for (;;)
    {
        nHead = pTable->tReuseList.tSentinel.nNext;
        if (nHead <= 0)
            break;
        GAME_SGameDataNodeVecDelDtor((SGameDataNodeStrc *)nHead, 0, 0);
        STORM_SMemFree((void *)nHead, szSGameDataTag, -2, 0);
    }

    pSentinel = &pTable->tChunkList.tSentinel;
    while ((nHead = pSentinel->nNext) > 0)
        GAME_ListUnlinkNode((TSLink *)(pTable->tChunkList.nLinkOffset + nHead));
    if (pSentinel->pPrevLink)
    {
        GAME_ListNextLink(pSentinel, 0, -1)->pPrevLink = pSentinel->pPrevLink;
        pSentinel->pPrevLink->nNext = pSentinel->nNext;
        pSentinel->pPrevLink = NULL;
        pSentinel->nNext = 0;
    }

    pSentinel = &pTable->tReuseList.tSentinel;
    while ((nHead = pSentinel->nNext) > 0)
        GAME_ListUnlinkNode((TSLink *)(pTable->tReuseList.nLinkOffset + nHead));
    if (pSentinel->pPrevLink)
    {
        GAME_ListNextLink(pSentinel, 0, -1)->pPrevLink = pSentinel->pPrevLink;
        pSentinel->pPrevLink->nNext = pSentinel->nNext;
        pSentinel->pPrevLink = NULL;
        pSentinel->nNext = 0;
    }

    pTable->pVtbl = &gVtblSGameData0;
    pTable->nFullnessIndicator = 0;
    GAME_ListUnlinkAllNodes(&pTable->tNodeList);

    for (iBucket = 0; iBucket < pTable->tBuckets.nCount; ++iBucket)
    {
        for (;;)
        {
            pBucket = &pTable->tBuckets.pLists[iBucket];
            if (pBucket->tSentinel.nNext <= 0)
                break;
            GAME_ListUnlinkNode(GAME_ListGetLinkSlot(pBucket, 0, (char *)pBucket->tSentinel.nNext));
        }
    }

    GAME_SGameDataHashUnlinkListsAndFree(&pTable->tBuckets);
    GAME_ListUnlinkAllNodes(&pTable->tNodeList);
    GAME_ListUnlinkNode(&pTable->tNodeList.tSentinel);
}

//1.10f: D2Client.0x6FAACE10
//  TSLink::NextLink: the link after this one. A negative nNext is the end of the list, turned back into the terminator's link (`not eax`,
//  0x6FAACE17). Otherwise it is the next object plus the link offset, and an offset passed negative is derived from the link itself: this link
//  minus where the previous link says its object starts (0x6FAACE24..0x6FAACE2A). Every caller passes -1 and stores through the result.
//  The end test is `jge` (0x6FAACE15), so a zero nNext takes the offset arm here, as it does in every unlink the image writes out.
TSLink *__fastcall GAME_ListNextLink(TSLink *pLink, int32_t nEdxUnused, int32_t nLinkOffset)
{
    int32_t nNext;
    int32_t nOffset;

    nNext = pLink->nNext;
    if (nNext < 0)
        return (TSLink *)~nNext;
    nOffset = nLinkOffset;
    if (nOffset < 0)
        nOffset = (int32_t)((uintptr_t)pLink - pLink->pPrevLink->nNext);
    return (TSLink *)((uintptr_t)nNext + nOffset);
}

//1.10f: D2Client.0x6FAACE40
//  TSList::Link: a node's link, the node plus the list's link offset, or the terminator for NULL, so an insert or an unlink needs no case for the
//  list's ends.
TSLink *__fastcall GAME_ListGetLinkSlot(TSExplicitList *pList, int32_t nEdxUnused, char *pNode)
{
    if (pNode)
        return (TSLink *)(pNode + pList->nLinkOffset);
    return &pList->tSentinel;
}

//1.10f: D2Client.0x6FAACE60
//  TSList::UnlinkAll: while the terminator's nNext (the head) is positive, the head's link is found (the head plus the link offset, 0x6FAACE6A) and
//  unlinked; the terminator itself stays linked. The original inlines the unlink (GAME_ListUnlinkNode); GAME_ListDestruct and
//  CRT_LIST_SetLinkOffset carry the same loop inline, and the C's GAME_ListDestruct calls this function there.
void __fastcall GAME_ListUnlinkAllNodes(TSExplicitList *pList)
{
    int32_t nHead;
    TSLink *pLink;

    while ((nHead = pList->tSentinel.nNext) > 0)
    {
        pLink = (TSLink *)(pList->nLinkOffset + nHead);
        GAME_ListUnlinkNode(pLink);
    }
}

//1.10f: D2Client.0x6FAACEB0
//  The deleting destructor of a TSExplicitList: GAME_ListDestruct, then the storage freed when bit 0 of the flag asks for it (`test [esp+4+
//  bDelete], 1`, 0x6FAACEB8), under the literal "delete". Every caller passes 0, so in this image it only ever destructs.
void *__fastcall GAME_ListVecDelDtor(TSExplicitList *pList, int32_t nEdxUnused, uint8_t bDelete)
{
    GAME_ListDestruct(pList);
    if ((bDelete & 1) != 0 && pList)
        STORM_SMemFree(pList, szDelete, -1, 0);
    return pList;
}

//1.10f: D2Client.0x6FAACEE0
//  The destructor of a TSHashObjectChunk<SGAMEDATA, HASHKEY_NONE>, {TSGrowableArray<T> m_array; TSLink m_link}, destroyed in reverse: the chunk's
//  own link first (0x6FAACEE6..0x6FAACF13; the original inlines GAME_ListUnlinkNode), then the array: every record's ~TSHashObject, which unlinks
//  tNodeLink (+0x0C, a call, 0x6FAACF29) and then tBucketLink (+0x04, written out around a call to GAME_ListNextLink, 0x6FAACF3C), and last the
//  records' storage, freed under the record's own RTTI tag (0x6FAACF7F).
//  The records are 7656 bytes apart: the walk keeps a byte offset (ebp) beside its index (a stack slot) and re-reads the storage and the count every
//  pass, the count compared unsigned (`jb`, 0x6FAACF6C). The template's array destructor checks every index against the count; this one does not.
//  Callers hand it a chunk taken off tChunkList and free the chunk themselves afterwards.
//  SGameDataChunkStrc is typed from these reads and the chunk list's link offset of 16 (`mov dword ptr [ebx], 10h` at 0x6FAAC3E5). NOT ESTABLISHED:
//  its size of 24 bytes is the template's; D2Client allocates no chunk, so nothing here proves it.
void __fastcall GAME_SGameDataFreeRecordArray(SGameDataChunkStrc *pChunk)
{
    uint32_t iRecord;
    uint32_t nRecordOffset;
    char *pRecord;
    TSLink *pSlotLink;
    TSLink *pSlotPrevLink;
    char *pRecords;

    GAME_ListUnlinkNode(&pChunk->tLink);
    nRecordOffset = 0;

    for (iRecord = 0; iRecord < pChunk->nCount; ++iRecord)
    {
        pRecord = pChunk->pRecords + nRecordOffset;
        GAME_ListUnlinkNode(&((SGameDataNodeStrc *)pRecord)->tNodeLink);

        //  tBucketLink: TSLink::Unlink with its NextLink(-1) left a call.
        pSlotLink = &((SGameDataNodeStrc *)pRecord)->tBucketLink;
        pSlotPrevLink = pSlotLink->pPrevLink;
        if (pSlotPrevLink)
        {
            GAME_ListNextLink(pSlotLink, 0, -1)->pPrevLink = pSlotPrevLink;
            pSlotLink->pPrevLink->nNext = pSlotLink->nNext;
            pSlotLink->pPrevLink = NULL;
            pSlotLink->nNext = 0;
        }
        nRecordOffset += nSGameDataRecordSize;
    }

    pRecords = pChunk->pRecords;
    if (pRecords)
        STORM_SMemFree(pRecords, szSGameDataTag, -2, 0);
}

//1.10f: D2Client.0x6FAAD010
//  TSLink::Unlink with NextLink(-1) folded in: nothing for a link that is not linked (pPrevLink NULL, `jz` at 0x6FAAD015); otherwise the next link
//  (~nNext at the end of the list, else this link moved from its own object to the next one) takes this link's pPrevLink (0x6FAAD02D), the
//  previous link takes its nNext (0x6FAAD034), and both are cleared, so a second unlink does nothing. The end test is `jge` (0x6FAAD01C), as in
//  GAME_ListNextLink.
void __fastcall GAME_ListUnlinkNode(TSLink *pLink)
{
    TSLink *pPrevLink;
    int32_t nNext;
    TSLink *pNextLink;

    pPrevLink = pLink->pPrevLink;
    if (!pPrevLink)
        return;
    nNext = pLink->nNext;
    if (nNext >= 0)
        pNextLink = (TSLink *)((uintptr_t)pLink - pPrevLink->nNext + nNext);
    else
        pNextLink = (TSLink *)~nNext;
    pNextLink->pPrevLink = pPrevLink;
    pLink->pPrevLink->nNext = pLink->nNext;
    pLink->pPrevLink = NULL;
    pLink->nNext = 0;
}

//1.10f: D2Client.0x6FAAD050
//  TSGetExplicitLink::Link: a node's link is the node plus the list's link offset, `lea eax, [ecx+edx]`, the whole function.
TSLink *__fastcall GAME_ListNodeToLink(char *pNode, int32_t nLinkOffset)
{
    return (TSLink *)(pNode + nLinkOffset);
}

#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "WNDPROC.h"
#include "WINMAIN.h"
#include "Lng.h"
#include "CoreArchive.h"

//  WNDPROC.cpp -- the client's window procedure and message pump, Storm's message-handler registration helpers, and the six handlers of the
//  table sgWndProcHandlers. The original source file is CORE\WNDPROC.CPP (the asserts of the two registrars name it).
//  Address range 0x6FAA23B0..0x6FAA285B. gbAppActive and sgWndProcHandlers are ours, in WNDPROC.h.
static_assert(sizeof(D2WindowProcCallbackStrc) == 12
                  && offsetof(D2WindowProcCallbackStrc, nType) == 0x00
                  && offsetof(D2WindowProcCallbackStrc, uMessage) == 0x04
                  && offsetof(D2WindowProcCallbackStrc, pfCallback) == 0x08,
              "the registration record this file walks: kind, message, "
              "handler, and a 12-byte stride");
//  D2WindowProcCallbackStrc and D2WinCommandRegisterStrc describe the same 12-byte record; the handler-table walk reads D2WinCommandRegisterStrc.
static_assert(offsetof(D2WinCommandRegisterStrc, nKind) == 0x00
                  && offsetof(D2WinCommandRegisterStrc, pfnHandler) == 0x08,
              "the record the walk and WNDPROC.h's table read: kind first, "
              "the handler last");
static_assert(GAMETYPE_OBNET_HOST == 6 && GAMETYPE_LAN_HOST == 8,
              "the two game types the window proc distinguishes. This enum "
              "is the one that falsified a NAME once: `dwGameType == 6` had "
              "been read as an expansion test and is the open-Battle.net "
              "host arm");


//  The source path the registrars pass to their asserts.
static const char szSourceFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\CORE\\WNDPROC.CPP";

//  Storm's handler signature is void (__stdcall *)(SMSGHANDLER_PARAMS *). WM_USER is the literal pushed by WNDPROC_RedirectToUser.
static_assert(WM_USER == 0x400, "`push 400h` at 0x6FAA252B");

//1.10f: D2Client.0x6FAA23B0
//  Switches the client to a resolution mode; nothing if D2Gfx already reports it. void: the original leaves the current mode in eax on the
//  early out, and no caller reads it. Only modes 0 and 2 set the size; any other mode keeps gnScreenWidth and gnScreenHeight and still re-lays
//  everything below. gnViewportRight and gnDrawMaxWidth take the width, gnViewportBottom the height less 40. nMode goes to MEMPART_Nullsub1 in
//  ecx (an empty function, so it is called bare) and to UI_NpcMenuCloseHireListDialog, whose parameter is never read; it is passed anyway.
void __fastcall WNDPROC_SetResolutionMode(D2GameResolutionMode nMode)
{
    int32_t nWidth;

    if (nMode == D2GFX_GetResolutionMode())
        return;

    switch (nMode)
    {
    case D2GAMERES_640x480:
        nWidth = 640;
        gnScreenHeight = 480;
        gnScreenWidth = nWidth;
        gbHighResolution = 0;
        break;

    case D2GAMERES_800x600:
        nWidth = 800;
        gnScreenHeight = 600;
        gnScreenWidth = nWidth;
        gbHighResolution = 1;
        break;

    default:
        nWidth = gnScreenWidth;
        break;
    }

    gnViewportRight = nWidth;
    gnViewportBottom = gnScreenHeight - 40;
    gnDrawMaxWidth = nWidth;
    D2WIN_10005(nMode);
    SCROLL_SetWindowMode(SCROLL_GetUiShiftScreen());
    AUTOMAP_RecomputeCenterOffsets(1);
    MEMPART_Nullsub1();
    UI_NpcMenuCloseHireListDialog((int32_t)nMode);
}

//  The six handlers below are the six records of sgWndProcHandlers (WNDPROC.h): WM_DESTROY, WM_ACTIVATEAPP, WM_NCHITTEST, the command ids
//  40100 and 4001, and WM_USER. Nothing registers the table (no code or data refers to it), so in a stock client these six never run: UNREACHABLE.

//1.10f: D2Client.0x6FAA2460
//  WM_DESTROY: quit the message loop, claim the message, answer 0.
void __stdcall WNDPROC_OnDestroy(SMSGHANDLER_PARAMS *pMsg)
{
    PostQuitMessage(0);
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
}

//1.10f: D2Client.0x6FAA2480
//  WM_ACTIVATEAPP: gbAppActive = (wParam != 0), and the message is NOT claimed -- bReturn is set to 0 and lResult left alone.
void __stdcall WNDPROC_OnActivateApp(SMSGHANDLER_PARAMS *pMsg)
{
    gbAppActive = pMsg->wParam != 0;
    pMsg->bReturn = 0;
}

//1.10f: D2Client.0x6FAA24A0
//  WM_NCHITTEST: DefWindowProcA's answer, claimed.
void __stdcall WNDPROC_OnDefault(SMSGHANDLER_PARAMS *pMsg)
{
    pMsg->bReturn = 1;
    pMsg->lResult = DefWindowProcA(pMsg->hWnd, pMsg->uMessage, pMsg->wParam, pMsg->lParam);
}

//1.10f: D2Client.0x6FAA24D0
//  Command 40100: send WM_CLOSE to the same window, claim it, answer 0.
void __stdcall WNDPROC_RedirectToClose(SMSGHANDLER_PARAMS *pMsg)
{
    SendMessageA(pMsg->hWnd, WM_CLOSE, 0, 0);
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
}

//1.10f: D2Client.0x6FAA2500
//  WM_USER: claimed and answered 0, nothing else.
void __stdcall WNDPROC_OnIgnoredMessage(SMSGHANDLER_PARAMS *pMsg)
{
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
}

//1.10f: D2Client.0x6FAA2520
//  Command 4001: send WM_USER to the same window -- which the last record would swallow, had the table been registered -- claim it, answer 0.
void __stdcall WNDPROC_RedirectToUser(SMSGHANDLER_PARAMS *pMsg)
{
    SendMessageA(pMsg->hWnd, WM_USER, 0, 0);
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
}

//1.10f: D2Client.0x6FAA2550
//  The client's window procedure; every Windows message the game sees arrives here. WINMAIN_D2ClientEntrypoint hands its address to D2Win,
//  which is its only reference.
//  RECORD_LogEvent(Msg, wParam, lParam) runs first and unconditionally, so the demo recorder sees even the messages nothing else handles. Then
//  Storm's handler-table dispatch; its (bHandled, lResult) pair is returned only when bHandled is set. Anything not claimed goes to
//  DefWindowProcA. While a recorded playback is loaded only message 0x10 (WM_CLOSE) is dispatched at all, so live input cannot disturb a playback
//  but the window can still be closed. The two epilogues in the bytes are the compiler merging two returns.
LRESULT __stdcall WNDPROC_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    BOOL bReturn = FALSE;
    LRESULT lResult = 0;

    RECORD_LogEvent(Msg, wParam, lParam);

    if ((!RECORD_GetRecordPlaybackLoaded() || Msg == WM_CLOSE) && STORM_SMsgDispatchMessage(hWnd, Msg, (uint32_t)wParam, (int32_t)lParam, &bReturn, &lResult) && bReturn)
    {
        return lResult;
    }
    else
    {
        return DefWindowProcA(hWnd, Msg, wParam, lParam);
    }
}

//1.10f: D2Client.0x6FAA25D0
//  The message pump every screen is built on: it runs until the idle callback pfnIdle (int (__stdcall *)(int)) says stop.
//  Per iteration: PeekMessageA without removing. If a message is waiting, GetMessageA takes it, it runs past the accelerator tables, and it is
//  translated and dispatched. If none is waiting, pfnIdle is called with a counter that increments every idle pass, and a non-zero return
//  leaves the pump and becomes this function's return value. A WM_QUIT (GetMessageA returning 0) ends the loop and the return is 1.
//  BUG: neither accelerator table swallows the message. A successful translation jumps (0x6FAA262D) to the TranslateMessage / DispatchMessageA
//  pair instead of past it, so a key an accelerator handles is also delivered as an ordinary key message and both handlers run. Reproduced.
//  The tables are tried in order: ghAccelTable first, hAccTableAlt only if the first is absent or did not translate.
//  The sleep condition reads backwards and is correct: if (!gbAppActive && gnGameType != 6 && gnGameType != 8) Sleep(10). gbAppActive is
//  WM_ACTIVATEAPP's wParam (TRUE on activation), so the pump yields its timeslice only while the client is in the background; the two
//  open-host game types never yield, even unfocused, as they run a server thread.
//  The idle-exit path (0x6FAA26A1) tests bNotQuit (`test edi, edi / jnz`) and would return 1 when it is clear. UNREACHABLE: bNotQuit is only
//  assigned inside the PeekMessageA branch and is non-zero on every path to that test, so the code returns nIdleReturn bare.
int32_t __fastcall WNDPROC_WindowRunMessagePump(int32_t(__stdcall* pfnIdle)(int32_t))
{
    MSG Msg;

    int32_t nIdleTick = 0;
    BOOL bNotQuit = TRUE;

    do
    {
        if (PeekMessageA(&Msg, NULL, 0, 0, PM_NOREMOVE))
        {
            //  BUG: GetMessageA also returns -1 on error, which the loop treats as "keep going" because it tests for zero. Reproduced.
            bNotQuit = GetMessageA(&Msg, NULL, 0, 0);
            if (bNotQuit)
            {
                //  Each table is loaded into a register once, before WINDOW_GetWindow is called; held in a local here for the same reason.
                const HACCEL hAccelTable = ghAccelTable;
                if (!hAccelTable || !TranslateAcceleratorA(D2GFX_WINDOW_GetWindow(), hAccelTable, &Msg))
                {
                    const HACCEL hAccelTableAlt = hAccTableAlt;
                    if (hAccelTableAlt)
                        TranslateAcceleratorA(D2GFX_WINDOW_GetWindow(), hAccelTableAlt, &Msg);
                }
            }

            //  Unconditional, even when an accelerator translated the message (see the BUG note in the head).
            TranslateMessage(&Msg);
            DispatchMessageA(&Msg);
        }
        else
        {
            if (pfnIdle)
            {
                //  The counter is post-incremented: the first idle pass is handed 0.
                const int32_t nIdleReturn = pfnIdle(nIdleTick++);
                if (nIdleReturn)
                    return nIdleReturn;
            }

            if (!gbAppActive && gnGameType != GAMETYPE_OBNET_HOST && gnGameType != GAMETYPE_LAN_HOST)
            {
                Sleep(10);
            }
        }
    } while (bNotQuit);

    return 1;
}

//  Registers one handler with Storm, switching on nKind (D2WindowProcCallbackType): 0 message, 1 command, 2 key-up, 3 key-down. The original
//  has no such function: the compiler inlined it into WNDPROC_RegisterMessageHandlers (its assert is WNDPROC.CPP:314), and it is written out
//  as a helper here. Two of the four arms are dead in D2Client: no handler table holds a record of kind 1 or 2. The arms are written anyway,
//  as in the binary.
static void RegisterMessageHandler(HWND hWnd, D2WindowProcCallbackType nKind, uint32_t uMessage, SMSGHANDLER pfnHandler)
{
    switch (nKind)
    {
    case D2_WINPROC_MESSAGE:
        if (uMessage == WM_COMMAND)
        {
            FOG_DisplayAssert("id != WM_COMMAND", szSourceFile, 314);
            exit(-1);
        }
        STORM_SMsgRegisterMessage(hWnd, uMessage, pfnHandler);
        break;

    case D2_WINPROC_COMMAND:
        STORM_SMsgRegisterCommand(hWnd, uMessage, pfnHandler);
        break;

    case D2_WINPROC_KEYUP:
        STORM_SMsgRegisterKeyUp(hWnd, uMessage, pfnHandler);
        break;

    case D2_WINPROC_KEYDOWN:
        STORM_SMsgRegisterKeyDown(hWnd, uMessage, pfnHandler);
        break;

    //  Kinds above 3 land here, and the default does nothing -- not even the assert the message arm gets.
    default:
        break;
    }
}

//1.10f: D2Client.0x6FAA26C0
//  Registers a table of message handlers with Storm, one record at a time. The original inlines the singular registrar (RegisterMessageHandler)
//  in the loop. A record is 12 bytes, {nKind, nMessageOrKey, pfnHandler}; the original walks it with an interior pointer (stride 0Ch), which
//  is indexed as an array here. nCallbacks is unsigned: the guard is `test ebp, ebp / jbe` and the loop's compare is `jb`.
void __fastcall WNDPROC_RegisterMessageHandlers(HWND hWnd, const D2WinCommandRegisterStrc* pCallbacks, size_t nCallbacks)
{
    if (!hWnd)
    {
        FOG_DisplayAssert("hWnd", szSourceFile, 377);
        exit(-1);
    }

    for (size_t iCallback = 0; iCallback < nCallbacks; iCallback++)
    {
        RegisterMessageHandler(hWnd, pCallbacks[iCallback].nKind, pCallbacks[iCallback].nMessageOrKey, pCallbacks[iCallback].pfnHandler);
    }
}

//  The mirror of RegisterMessageHandler, kind for kind; the compiler inlined it into WNDPROC_UnregisterMessageHandlers (its assert is
//  WNDPROC.CPP:347).
static void UnregisterMessageHandler(HWND hWnd, D2WindowProcCallbackType nKind, uint32_t uMessage, SMSGHANDLER pfnHandler)
{
    switch (nKind)
    {
    case D2_WINPROC_MESSAGE:
        if (uMessage == WM_COMMAND)
        {
            FOG_DisplayAssert("id != WM_COMMAND", szSourceFile, 347);
            exit(-1);
        }
        STORM_SMsgUnregisterMessage(hWnd, uMessage, pfnHandler);
        break;

    case D2_WINPROC_COMMAND:
        STORM_SMsgUnregisterCommand(hWnd, uMessage, pfnHandler);
        break;

    case D2_WINPROC_KEYUP:
        STORM_SMsgUnregisterKeyUp(hWnd, uMessage, pfnHandler);
        break;

    case D2_WINPROC_KEYDOWN:
        STORM_SMsgUnregisterKeyDown(hWnd, uMessage, pfnHandler);
        break;

    default:
        break;
    }
}

//1.10f: D2Client.0x6FAA2780
//  The mirror of WNDPROC_RegisterMessageHandlers, arm for arm. Its `retn 4` is the arithmetic for the prototype: two register arguments and
//  one on the stack, i.e. __fastcall with three parameters. void: the eax it leaves is SMsgUnregisterMessage's value, not a return.
void __fastcall WNDPROC_UnregisterMessageHandlers(HWND hWnd, const D2WinCommandRegisterStrc* pCallbacks, size_t nCallbacks)
{
    if (!hWnd)
    {
        FOG_DisplayAssert("hWnd", szSourceFile, 396);
        exit(-1);
    }

    for (size_t iCallback = 0; iCallback < nCallbacks; iCallback++)
    {
        UnregisterMessageHandler(hWnd, pCallbacks[iCallback].nKind, pCallbacks[iCallback].nMessageOrKey, pCallbacks[iCallback].pfnHandler);
    }
}

//1.10f: D2Client.0x6FAA2840
//  Saves Storm's message-registration state for the client's main window, so the handlers registered during a game can be dropped wholesale at
//  the end of it; the matching pop is WNDPROC_PopStormMessageState. The function is `call WINDOW_GetWindow / push eax / call
//  SMsgPushRegisterState / retn`, which purges nothing, so __cdecl is a declaration and not observable.
void __cdecl WNDPROC_PushStormMessageState()
{
    STORM_SMsgPushRegisterState(D2GFX_WINDOW_GetWindow());
}

//1.10f: D2Client.0x6FAA2850
//  The counterpart of WNDPROC_PushStormMessageState: pops Storm's message-registration state.
void __cdecl WNDPROC_PopStormMessageState()
{
    STORM_SMsgPopRegisterState(D2GFX_WINDOW_GetWindow());
}

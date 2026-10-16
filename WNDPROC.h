//  WNDPROC.h -- the globals of WNDPROC.cpp that are ours: gbAppActive and the handler table sgWndProcHandlers. Each is a C++17 `inline`
//  variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above a global gives its address in
//  the original. Include it after D2Client.h, which declares the six handlers.
#pragma once

//  D2Client 0x6FB747C0
//  Non-zero while the client has the foreground: GAME_WindowOnActivateApp stores WM_ACTIVATEAPP's wParam in it and WNDPROC_OnActivateApp stores
//  0 or 1; WNDPROC_WindowRunMessagePump sleeps on an idle pass only while it is 0. The file holds 1.
inline int32_t gbAppActive = 1;
static_assert(sizeof(gbAppActive) == 4, "gbAppActive: the item's 4 bytes in the .i64");

//  D2Client 0x6FB747C8
//  WNDPROC.CPP's handler table, six records {nKind, nMessageOrKey, pfnHandler}; dwType 1 is a WM_COMMAND id, not a message. Nothing registers it.
inline D2WinCommandRegisterStrc sgWndProcHandlers[6] =
{
    { D2_WINPROC_MESSAGE, 0x002, &WNDPROC_OnDestroy },
    { D2_WINPROC_MESSAGE, 0x01C, &WNDPROC_OnActivateApp },
    { D2_WINPROC_MESSAGE, 0x084, &WNDPROC_OnDefault },
    { D2_WINPROC_COMMAND, 0x9CA4, &WNDPROC_RedirectToClose },
    { D2_WINPROC_COMMAND, 0xFA1, &WNDPROC_RedirectToUser },
    { D2_WINPROC_MESSAGE, 0x400, &WNDPROC_OnIgnoredMessage },
};
static_assert(sizeof(sgWndProcHandlers) == 72, "sgWndProcHandlers: the item's 72 bytes in the .i64");

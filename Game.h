//  Game.h -- the globals of Game.cpp: the game-window and mouse message tables, the launch-parameter table, the client's state, the Windows
//  version block, the strings, and gGameData with its container's vtables. Address range 0x6FB6E23C..0x6FBA79EC.
//  Each is a C++17 `inline` variable, so every file that includes this header shares one object. The globals from 0x6FBA7748 on lie past .data's
//  raw data and are zero at load.
//  gGameData is built and torn down by Game.cpp's sgGameDataLifetime. MSG_InitGame hands gGameData and gGameList to D2Game's GAME_InitGameDataTable,
//  which keeps the first and only tests the second; D2Game then calls the vtable's slots on the table.
//  Include it after D2Client.h, which declares every function named here.
#pragma once

//  D2Client 0x6FB75450
//  The three callbacks GAME_SetActiveMessageCallback hands out: GAME_DrawGameScene (0x6FAA9640), INSERTDISC_DrawInsertDiscPromptScene (0x6FADC180)
//  and INSERTDISC_DrawInsertCinematicsDiscPromptScene (0x6FADC970). GAME_UpdateFrame calls the installed one through gpfnActiveMessageCallback with
//  ecx = 0 and nothing on the stack, which the `__cdecl` with no argument of the other two accepts as well, hence their casts.
//  BUG: slots 1 and 2 are never called. UNREACHABLE: the Show functions install them only around their nested pump, when GAME_UpdateFrame, the one
//  caller of gpfnActiveMessageCallback, does not run.
inline D2ClientMessageCallbackFn gafnMessageCallbacks[3] =
{
    GAME_DrawGameScene,
    (D2ClientMessageCallbackFn)INSERTDISC_DrawInsertDiscPromptScene,
    (D2ClientMessageCallbackFn)INSERTDISC_DrawInsertCinematicsDiscPromptScene,
};
static_assert(sizeof(gafnMessageCallbacks) == 12,
              "gafnMessageCallbacks: 3 slots, 12 bytes, 0x6FB75450");

//  D2Client 0x6FB75818
//  gGameWindowMessageHandlers: ten records of 12 bytes (every one of the five call sites pushes 10, and 10 x 12 bytes ends at 0x6FB75890 where
//  gWheelNativeHandler begins), registered by GAME_AppModeGame:
//
//      WM_LBUTTONDOWN/UP  GAME_WindowOnLButtonDown / OnLButtonUp
//      WM_RBUTTONDOWN/UP  GAME_WindowOnRButtonDown / OnRButtonUp
//      VK_LWIN, VK_RWIN, VK_APPS   GAME_WindowConsumeMessage
//      WM_SYSCOMMAND      GAME_WindowOnSysCommand
//      WM_CLOSE           GAME_WindowOnClose
//      WM_ACTIVATEAPP     GAME_WindowOnActivateApp
//
//  Field +4 of record 0 is the WM_LBUTTONDOWN message, which nothing writes; GAME_UnregisterMouseInputHandlers reads it as its guard (the defect
//  noted there). The sizes and counts of this table and the next two are asserted in Game.cpp.
inline D2WinCommandRegisterStrc gGameWindowMessageHandlers[10] =
{
    { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, GAME_WindowOnLButtonDown },
    { D2_WINPROC_MESSAGE, WM_LBUTTONUP, GAME_WindowOnLButtonUp },
    { D2_WINPROC_MESSAGE, WM_RBUTTONDOWN, GAME_WindowOnRButtonDown },
    { D2_WINPROC_MESSAGE, WM_RBUTTONUP, GAME_WindowOnRButtonUp },
    { D2_WINPROC_KEYDOWN, VK_LWIN, GAME_WindowConsumeMessage },
    { D2_WINPROC_KEYDOWN, VK_RWIN, GAME_WindowConsumeMessage },
    { D2_WINPROC_KEYDOWN, VK_APPS, GAME_WindowConsumeMessage },
    { D2_WINPROC_MESSAGE, WM_SYSCOMMAND, GAME_WindowOnSysCommand },
    { D2_WINPROC_MESSAGE, WM_CLOSE, GAME_WindowOnClose },
    { D2_WINPROC_MESSAGE, WM_ACTIVATEAPP, GAME_WindowOnActivateApp },
};

//  D2Client 0x6FB75920
//  The message table GAME_AppModeGame registers for the beta slide show: seven records of 12 bytes. WM_LBUTTONDOWN and WM_RBUTTONDOWN go to
//  GAME_BetaSlidesOnButtonDown, WM_KEYDOWN to GAME_BetaSlidesOnKeyDown, message 0x1C to GAME_WindowOnActivateApp, and the three Windows keys
//  (0x5B..0x5D) to GAME_WindowConsumeMessage. 7 * 12 = 84 stops four bytes short of gVanquishedMessageHandlers.
inline D2WinCommandRegisterStrc gBetaSlidesMessageHandlers[7] =
{
    { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, GAME_BetaSlidesOnButtonDown },
    { D2_WINPROC_MESSAGE, WM_RBUTTONDOWN, GAME_BetaSlidesOnButtonDown },
    { D2_WINPROC_MESSAGE, WM_KEYDOWN, GAME_BetaSlidesOnKeyDown },
    { D2_WINPROC_MESSAGE, WM_ACTIVATEAPP, GAME_WindowOnActivateApp },
    { D2_WINPROC_KEYDOWN, VK_LWIN, GAME_WindowConsumeMessage },
    { D2_WINPROC_KEYDOWN, VK_RWIN, GAME_WindowConsumeMessage },
    { D2_WINPROC_KEYDOWN, VK_APPS, GAME_WindowConsumeMessage },
};

//  D2Client 0x6FB75978
//  The message table GAME_AppModeGame registers for the Diablo-vanquished screen: nine records of 12 bytes. The two button-downs go to
//  GAME_VanquishedOnButtonDown, the two button-ups to GAME_VanquishedOnButtonUp, WM_KEYDOWN to GAME_VanquishedOnKeyDown, message 0x1C to
//  GAME_WindowOnActivateApp, and 0x5B..0x5D to GAME_WindowConsumeMessage. 9 * 12 = 108 ends exactly on gbLaunchParamsOk. One of the two
//  registrations first calls WNDPROC_SetResolutionMode(D2GAMERES_640x480): the screen is drawn at low resolution.
inline D2WinCommandRegisterStrc gVanquishedMessageHandlers[9] =
{
    { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, GAME_VanquishedOnButtonDown },
    { D2_WINPROC_MESSAGE, WM_RBUTTONDOWN, GAME_VanquishedOnButtonDown },
    { D2_WINPROC_MESSAGE, WM_LBUTTONUP, GAME_VanquishedOnButtonUp },
    { D2_WINPROC_MESSAGE, WM_RBUTTONUP, GAME_VanquishedOnButtonUp },
    { D2_WINPROC_MESSAGE, WM_KEYDOWN, GAME_VanquishedOnKeyDown },
    { D2_WINPROC_MESSAGE, WM_ACTIVATEAPP, GAME_WindowOnActivateApp },
    { D2_WINPROC_KEYDOWN, VK_LWIN, GAME_WindowConsumeMessage },
    { D2_WINPROC_KEYDOWN, VK_RWIN, GAME_WindowConsumeMessage },
    { D2_WINPROC_KEYDOWN, VK_APPS, GAME_WindowConsumeMessage },
};

//  D2Client 0x6FB759E8
//  gLaunchParamHandlers: the command-line handler table walked by GAME_ApplyLaunchParams, of the slot type LaunchParamFn. 48 slots: two (40 and 45)
//  are null and the loop skips them, fifteen are GAME_NullFunc (5, 8, 21, 25-29, 32, 36-39, 41, 42); six slots share GAME_ApplyParamJoinInfo and
//  five share GAME_ApplyParamPlayerClass. The loop's bound is `cmp esi, 0C0h` (0x6FAABF01), 192 bytes.
//  The slot index follows the parameter block's field order fairly closely (+4, +5, +6, +8, +7, ..., +506, +529, +532, +533) but not exactly:
//  slots 3/4 are swapped, and slots 46/47 read +513 and +528 after slot 43 has read +543.
typedef void (__fastcall *LaunchParamFn)(D2ConfigStrc *pConfig);

inline void (__fastcall *gLaunchParamHandlers[48])(D2ConfigStrc *pConfig) =
{
    /*  0 */ GAME_ApplyParamGDI,
    /*  1 */ GAME_ApplyParamGlide,
    /*  2 */ GAME_ApplyParamOpenGL,
    /*  3 */ GAME_ApplyParamDirect3D,
    /*  4 */ GAME_ApplyParamRave,
    /*  5 */ (LaunchParamFn)GAME_NullFunc,
    /*  6 */ GAME_ApplyParamGamma,
    /*  7 */ GAME_ApplyParamVSync,
    /*  8 */ (LaunchParamFn)GAME_NullFunc,
    /*  9 */ GAME_ApplyParamJoinInfo,
    /* 10 */ GAME_ApplyParamJoinInfo,
    /* 11 */ GAME_ApplyParamJoinInfo,
    /* 12 */ GAME_ApplyParamJoinInfo,
    /* 13 */ GAME_ApplyParamJoinInfo,
    /* 14 */ GAME_ApplyParamBattleNetIp,
    /* 15 */ GAME_ApplyParamRealmIp,
    /* 16 */ GAME_ApplyParamPlayerClass,
    /* 17 */ GAME_ApplyParamPlayerClass,
    /* 18 */ GAME_ApplyParamPlayerClass,
    /* 19 */ GAME_ApplyParamPlayerClass,
    /* 20 */ GAME_ApplyParamPlayerClass,
    /* 21 */ (LaunchParamFn)GAME_NullFunc,
    /* 22 */ GAME_ApplyParamCharacterName,
    /* 23 */ GAME_ApplyParamRealm,
    /* 24 */ GAME_ApplyParamArenaPlayerClass,
    /* 25 */ (LaunchParamFn)GAME_NullFunc,
    /* 26 */ (LaunchParamFn)GAME_NullFunc,
    /* 27 */ (LaunchParamFn)GAME_NullFunc,
    /* 28 */ (LaunchParamFn)GAME_NullFunc,
    /* 29 */ (LaunchParamFn)GAME_NullFunc,
    /* 30 */ GAME_ApplyParamStartAct,
    /* 31 */ GAME_ApplyParamEnableLog,
    /* 32 */ (LaunchParamFn)GAME_NullFunc,
    /* 33 */ GAME_ApplyParamDisableSaving,
    /* 34 */ GAME_ApplyParamInitSeed,
    /* 35 */ GAME_ApplyParamSetArenaFlag0,
    /* 36 */ (LaunchParamFn)GAME_NullFunc,
    /* 37 */ (LaunchParamFn)GAME_NullFunc,
    /* 38 */ (LaunchParamFn)GAME_NullFunc,
    /* 39 */ (LaunchParamFn)GAME_NullFunc,
    /* 40 */ NULL,
    /* 41 */ (LaunchParamFn)GAME_NullFunc,
    /* 42 */ (LaunchParamFn)GAME_NullFunc,
    /* 43 */ GAME_ApplyParamComInterface,
    /* 44 */ GAME_ApplyParamJoinInfo,
    /* 45 */ NULL,
    /* 46 */ GAME_ApplyParamCompressedData,
    /* 47 */ GAME_ApplyParamCompileTxt,
};

static_assert(sizeof(gLaunchParamHandlers) == 0xC0,
              "gLaunchParamHandlers: `cmp esi, 0C0h` at 0x6FAABF01");

//  D2Client 0x6FB75890
//  One record: WM_MOUSEWHEEL -> GAME_WindowOnMouseWheel. Registered by GAME_RegisterMouseInputHandlers only when a wheel is present and the OS
//  delivers WM_MOUSEWHEEL natively (Win98/ME or NT4/2000/XP, not Windows 95).
inline D2WinCommandRegisterStrc gWheelNativeHandler = { D2_WINPROC_MESSAGE, WM_MOUSEWHEEL, &GAME_WindowOnMouseWheel };
static_assert(sizeof(gWheelNativeHandler) == 12, "gWheelNativeHandler: the item's 12 bytes in the .i64");

//  D2Client 0x6FB7589C
//  Two records: WM_MBUTTONUP -> GAME_WindowOnMButtonUp, WM_MBUTTONDOWN -> GAME_WindowOnMButtonDown, in that order. Registered on Win9x or
//  NT4/2000/XP.
inline D2WinCommandRegisterStrc gMButtonHandlers[2] =
{ { D2_WINPROC_MESSAGE, WM_MBUTTONUP, &GAME_WindowOnMButtonUp }, { D2_WINPROC_MESSAGE, WM_MBUTTONDOWN, &GAME_WindowOnMButtonDown } };
static_assert(sizeof(gMButtonHandlers) == 24, "gMButtonHandlers: the item's 24 bytes in the .i64");

//  D2Client 0x6FB758B4
//  Two records: 0x20B (WM_XBUTTONDOWN) -> GAME_WindowOnXButtonDown and 0x20C (WM_XBUTTONUP) -> GAME_WindowOnXButtonUp. Registered on 2000/XP only,
//  because the X buttons arrived with Windows 2000 (the same gate the Configure Controls screen uses to decide whether an X button can be bound).
inline D2WinCommandRegisterStrc gXButtonHandlers[2] =
{ { D2_WINPROC_MESSAGE, 0x20B, &GAME_WindowOnXButtonDown }, { D2_WINPROC_MESSAGE, 0x20C, &GAME_WindowOnXButtonUp } };
static_assert(sizeof(gXButtonHandlers) == 24, "gXButtonHandlers: the item's 24 bytes in the .i64");

//  D2Client 0x6FB758CC
//  One record, for the Windows 95 wheel, with the same GAME_WindowOnMouseWheel as gWheelNativeHandler. Its message id is patched at runtime and is
//  0 in the binary: GAME_RegisterMouseInputHandlers writes RegisterWindowMessageA("MSH_WHEELSUPPORT_MSG") into [0].nMessageOrKey before registering
//  the table, so `msg 0x000` in a dump of it is a hole, not a default. That id is the message an application sends to the MSWheel window to ask
//  whether wheel support is on; rolls come as MSWHEEL_ROLLMSG, never registered here.
inline D2WinCommandRegisterStrc gWheelWin95Handler[1] = { { D2_WINPROC_MESSAGE, 0x000, &GAME_WindowOnMouseWheel } };
static_assert(sizeof(gWheelWin95Handler) == 12, "gWheelWin95Handler: the item's 12 bytes in the .i64");

//  D2Client 0x6FB7545C
//  The app mode GAME_AppModeStartup hands back, and the only thing it decides. Initialised to 5 (APPMODE_JOIN_GAME) in .data and written nowhere,
//  so the branch against 5 in GAME_AppModeStartup is always taken.
inline D2C_AppMode gnAppModeAfterStartup = APPMODE_JOIN_GAME;
static_assert(sizeof(gnAppModeAfterStartup) == 4, "gnAppModeAfterStartup: the item's 4 bytes in the .i64");

//  D2Client 0x6FB75460
//  The fixed-timestep game-logic tick interval in milliseconds: 40 (25 ticks/sec). GAME_UpdateFrame compares the time since gdwLastGameTickTime
//  against it and advances gdwLastGameTickTime by it per tick. Signed: `cmp edx, eax / jge` at 0x6FAA9DB9 compares (nSinceLastTick - this) against
//  (this - 1) signed; the other comparisons against it are unsigned because the other operand is a DWORD tick delta.
inline int32_t gnGameTickIntervalMs = 40;
static_assert(sizeof(gnGameTickIntervalMs) == 4, "gnGameTickIntervalMs: the item's 4 bytes in the .i64");

//  D2Client 0x6FB75468
//  A hardcoded Battle.net address.
inline char gszServerIP[264] = "207.82.87.243";
static_assert(sizeof(gszServerIP) == 264, "gszServerIP: the item's 264 bytes in the .i64");

//  D2Client 0x6FB75570
inline char gszBattleNetIP[264] = "207.82.87.139";
static_assert(sizeof(gszBattleNetIP) == 264, "gszBattleNetIP: the item's 264 bytes in the .i64");

//  D2Client 0x6FB75678
inline char gszMCPIP[264] = "207.82.87.133";
static_assert(sizeof(gszMCPIP) == 264, "gszMCPIP: the item's 264 bytes in the .i64");

//  D2Client 0x6FB75780
//  The beta slide show's screen list: 10 records of 13 bytes, `{ int dwStartTick; void *hImage; uint8 nPaletteAct; char *szName; }` (4 + 4 + 1 + 4,
//  unaligned, which is why every reference multiplies by 13). The names are screen01 through screen10, loaded from DATA\GLOBAL\ui\betascreens\.
//  The first two fields are scratch: GAME_BetaSlidesTick fills them on first sight of a screen and clears both when the show ends. The byte at +8
//  goes to D2Win #10177, which takes an act. gnBetaScreenCount follows after two padding bytes.
//  szName is `const char *`: nothing writes through it and ours point at literals (the original's strings lie in .data, writable but never written).
inline D2BetaScreenStrc gBetaScreens[10] =
{
    { .szName = "screen01" },   // [0]
    { .szName = "screen02" },   // [1]
    { .szName = "screen03" },   // [2]
    { .szName = "screen04" },   // [3]
    { .szName = "screen05" },   // [4]
    { .szName = "screen06" },   // [5]
    { .szName = "screen07" },   // [6]
    { .szName = "screen08" },   // [7]
    { .szName = "screen09" },   // [8]
    { .szName = "screen10" },   // [9]
};
static_assert(sizeof(gBetaScreens) == 130, "gBetaScreens: the item's 130 bytes in the .i64");

//  D2Client 0x6FB75804
//  10, the number of gBetaScreens records. Read by GAME_BetaSlidesTick to bound the index before drawing, to decide the show has ended, and to bound
//  the unload loop.
inline int32_t gnBetaScreenCount = 10;
static_assert(sizeof(gnBetaScreenCount) == 4, "gnBetaScreenCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7580C
//  The modifier flag word GAME_UpdateFrame passes as PLAYER_OnMouseClick's 4th argument for a held left button: set to 8 whenever the button is
//  held (regardless of gbInGame); a click is only issued if gbInGame and no blocking condition applies. Counterpart: gnRightClickModifierFlags.
inline int32_t gnLeftClickModifierFlags = 16;
static_assert(sizeof(gnLeftClickModifierFlags) == 4, "gnLeftClickModifierFlags: the item's 4 bytes in the .i64");

//  D2Client 0x6FB75810
//  The right-button counterpart to gnLeftClickModifierFlags: set to 8 in GAME_UpdateFrame whenever gbRightButtonDown is set.
inline int32_t gnRightClickModifierFlags = 16;
static_assert(sizeof(gnRightClickModifierFlags) == 4, "gnRightClickModifierFlags: the item's 4 bytes in the .i64");

//  D2Client 0x6FB75814
//  How many more frames must be drawn with a full screen clear before incremental drawing resumes. GAME_DrawGameScene reads it and, when non-zero,
//  calls D2GFX_ClearScreen(0) and decrements (`dec eax; mov; jns`). A zero count skips the clear (`test` / `je`, 0x6FAA9802), so a count of N clears
//  N frames. GAME_PreloadActData sets it to 1 when an act is loaded. Initialised to 1 in the image, so the very first scene drawn is a full clear.
//  The 0 store at 0x6FAA9829 follows `jns` (0x6FAA9818) only when the decrement went negative, beside FOG_Trace's "global static overwrite error"
//  trace (line 1713).
inline int32_t gnForceClearFrames = 1;
static_assert(sizeof(gnForceClearFrames) == 4, "gnForceClearFrames: the item's 4 bytes in the .i64");

//  D2Client 0x6FB758D8
//  The view's X offset as of the previous frame. GAME_DrawGameScene loads it, subtracts it from this frame's X to get gnViewScrollDeltaX, and then
//  stores the new X here. Both this and its Y twin are compared against -1 before the subtraction, so the first frame after a reset produces no
//  delta.
inline int32_t gnPrevViewOffsetX = -1;
static_assert(sizeof(gnPrevViewOffsetX) == 4, "gnPrevViewOffsetX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB758DC
//  The view's Y offset as of the previous frame; the twin of gnPrevViewOffsetX (the difference is gnViewScrollDeltaY).
inline int32_t gnPrevViewOffsetY = -1;
static_assert(sizeof(gnPrevViewOffsetY) == 4, "gnPrevViewOffsetY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB758E0
//  The tbl index per disconnect/refusal message id, indexed by gnDisconnectMessage (D2C_JoinErrors), one 16-bit entry per member (the bytes read
//  `f5 14 f6 14` = 5365, 5366). The count of 29 is what makes GAME_BeginDisconnect's `>= 29 -> 9` clamp a bounds check. Entries 25 and 26 are 10101
//  and 10102, patchstring.txt (index - 10000) rather than string.txt. Ids 20 and 21 are additionally overridden at the draw site with the expansion
//  ladder strings.
inline uint16_t gnDisconnectMessageStrings[29] =
{
    5365,   // [0]
    5366,   // [1]
    5367,   // [2]
    5368,   // [3]
    5369,   // [4]
    5371,   // [5]
    5370,   // [6]
    5357,   // [7]
    5358,   // [8]
    5372,   // [9]
    5373,   // [10]
    5374,   // [11]
    5375,   // [12]
    5376,   // [13]
    5377,   // [14]
    5378,   // [15]
    5379,   // [16]
    5380,   // [17]
    5381,   // [18]
    5360,   // [19]
    5364,   // [20]
    5363,   // [21]
    5362,   // [22]
    5361,   // [23]
    5359,   // [24]
    10101,   // [25]
    10102,   // [26]
    5370,   // [27]
    5371,   // [28]
};
static_assert(sizeof(gnDisconnectMessageStrings) == 58, "gnDisconnectMessageStrings: the item's 58 bytes in the .i64");

//  D2Client 0x6FB7591C
//  The player count at which GAME_ServerThreadProc refreshes the open game on Battle.net: 4. It compares D2Game's player count
//  (D2GAME_GAME_GetPlayerUnitsCount) with this both ways (`jge` 0x6FAAA432, `jl` 0x6FAAA436, `jge` 0x6FAAA447).
inline int32_t gnMaxPlayerCountForGameRefresh = 4;
static_assert(sizeof(gnMaxPlayerCountForGameRefresh) == 4, "gnMaxPlayerCountForGameRefresh: the item's 4 bytes in the .i64");

//  D2Client 0x6FB759E4
//  Set to 1 by GAME_ApplyLaunchParams before it runs the table and returned afterwards, so a handler could refuse the command line. None of the 48
//  handlers writes it, so the answer is always TRUE.
inline BOOL gbLaunchParamsOk = 1;
static_assert(sizeof(gbLaunchParamsOk) == 4, "gbLaunchParamsOk: the item's 4 bytes in the .i64");

//  D2Client 0x6FB75C18
inline char pszFileName[21] = "Data\\Local\\Color.txt";
static_assert(sizeof(pszFileName) == 21, "pszFileName: the item's 21 bytes in the .i64");

//  D2Client 0x6FB75C88
inline CHAR OutputString[30] = "Open server thread shut down\012";
static_assert(sizeof(OutputString) == 30, "OutputString: the item's 30 bytes in the .i64");

//  D2Client 0x6FB75E64
inline CHAR ClassName[7] = "MouseZ";
static_assert(sizeof(ClassName) == 7, "ClassName: the item's 7 bytes in the .i64");

//  D2Client 0x6FB75E6C
inline CHAR WindowName[17] = "Magellan MSWHEEL";
static_assert(sizeof(WindowName) == 17, "WindowName: the item's 17 bytes in the .i64");

//  D2Client 0x6FB75E80
inline CHAR String[21] = "MSH_WHEELSUPPORT_MSG";
static_assert(sizeof(String) == 21, "String: the item's 21 bytes in the .i64");

//  D2Client 0x6FBA7748
//  The game id from the launch parameters, a 16-bit one: GAME_ApplyParamJoinInfo writes it with `mov word ptr`, not a dword store. GAME_AppModeGame
//  reads it as nGameId when joining. The twin of gnJoinTokenId.
inline int16_t gnJoinGameId;
static_assert(sizeof(gnJoinGameId) == 2, "gnJoinGameId: the item's 2 bytes in the .i64");

//  D2Client 0x6FBA774C
//  The game thread's id, filled in by CreateThread's lpThreadId out-parameter; its only reference is that call in GAME_AppModeGame, so nothing
//  reads it. The game-thread group: ghGameThread (the handle), gbGameThreadStop (the exit signal), gbGameLoopEnabled (the run permission) and this.
inline DWORD gnGameThreadId;
static_assert(sizeof(gnGameThreadId) == 4, "gnGameThreadId: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7750
//  The index into gafnMessageCallbacks that GAME_SetActiveMessageCallback last selected; gpfnActiveMessageCallback is the resolved pointer.
inline int32_t gnActiveMessageCallbackIndex;
static_assert(sizeof(gnActiveMessageCallbackIndex) == 4, "gnActiveMessageCallbackIndex: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7754
//  The active 'show message' callback GAME_SetActiveMessageCallback last resolved from gafnMessageCallbacks[gnActiveMessageCallbackIndex].
//  GAME_UpdateFrame invokes it directly on one of its early-exit paths.
inline D2ClientMessageCallbackFn gpfnActiveMessageCallback;
static_assert(sizeof(gpfnActiveMessageCallback) == 4, "gpfnActiveMessageCallback: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7758
//  The client's Fog memory-pool handle. GAME_InitGame passes its address to FOG_CreateNewPoolSystem alongside the name "Client Pool System"; Fog's
//  pool constructor takes the handle as an out-parameter. Read back by GAME_InitGame and GAME_AppModeGame. The `rep stosd` over the 128-byte client
//  block also writes it (GAME_InitGame before the pool is made, GAME_AppModeGame after it is destroyed); GAME_ClearClientGlobalData zeroes the
//  region's globals by name.
inline int32_t ghClientPoolSystem;
static_assert(sizeof(ghClientPoolSystem) == 4, "ghClientPoolSystem: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA775C
//  GetTickCount, sampled once per frame by GAME_UpdateFrame and read everywhere else through GAME_GetFrameTick. It is what makes the game's `+ 500`
//  and `+ 250` deadlines milliseconds rather than frames.
inline uint32_t gdwFrameTick;
static_assert(sizeof(gdwFrameTick) == 4, "gdwFrameTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7760
//  The fixed-timestep accumulator's timestamp: the GetTickCount() value the last game-logic tick was considered to have happened at.
//  GAME_UpdateFrame advances it by gnGameTickIntervalMs per tick (with special-cased catch-up when real elapsed time is far ahead), and resets it
//  to GetTickCount() outright on the early-exit paths (window inactive, or a blocking UI panel open).
inline uint32_t gdwLastGameTickTime;
static_assert(sizeof(gdwLastGameTickTime) == 4, "gdwLastGameTickTime: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7764
//  Total game-logic ticks since the game started. GAME_UpdateFrame increments it in the same two instructions as gnFrameCountInStatsWindow (`inc ecx;
//  inc eax`), which is the same count reset with the stats window; GAME_InitGame zeroes it. Not gnClientFrame, which sits four bytes later.
inline int32_t gnTotalGameTicks;
static_assert(sizeof(gnTotalGameTicks) == 4, "gnTotalGameTicks: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7768
//  The client frame counter, incremented once per game loop iteration; every 13th frame triggers D2COMMON_DUNGEON_UpdateAndFreeInactiveRooms.
//  Unsigned: its `div`s are unsigned (0x6FAAA0EC and 0x6FAAA224).
inline uint32_t gnClientFrame;
static_assert(sizeof(gnClientFrame) == 4, "gnClientFrame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA776C
//  The GetTickCount deadline for the next keepalive ping. GAME_SendPingIfDue compares GetTickCount against it with `jnb`, calls MSG_SendPing6C when
//  due, and stores tick + 0x1388 (5000 ms) back here. GAME_UpdateFrame does the same test a second time on its own path. Zero in the image, so the
//  first frame sends immediately.
inline int32_t gnNextPingTick;
static_assert(sizeof(gnNextPingTick) == 4, "gnNextPingTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7770
//  GetTickCount at the moment the last keepalive ping went out. GAME_SendPingIfDue stamps it right after MSG_SendPing6C, and
//  GAME_OnPongUpdateLatency subtracts it from a second GetTickCount to get the round trip (the pong's arrival tick goes into gPongPacketBody[4],
//  0x6FAA9A70). GAME_UpdateFrame stamps it too, on its own send path.
inline int32_t gnPingSentTick;
static_assert(sizeof(gnPingSentTick) == 4, "gnPingSentTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7774
//  The measured round-trip latency in milliseconds, returned by GAME_GetNetworkLatency and printed as "Ping: %d" by DRAW_GameDrawDebugOverlay.
//  It also scales the position-drift tolerance in UNITMODE_SyncUnitPosition.
inline int32_t gnNetworkLatency;
static_assert(sizeof(gnNetworkLatency) == 4, "gnNetworkLatency: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7778
//  gnFrameCountInStatsWindow/3 as of the last ~3-second window boundary, a smoothed recent tick rate. Read by GAME_UpdateFrame's catch-up heuristic
//  (thresholds at <7, >28, <=22, <=7) to decide how aggressively to skip game-logic ticks when running behind, and force-set to 25 (the nominal
//  ticks/sec) on a hard catch-up reset. Signed: it is the destination of a signed divide by three (`imul 0x55555556` at 0x6FAA9D58) and every
//  comparison against it is signed (`jge` at 0x6FAAA12A, `jle` at 0x6FAAA148 and 0x6FAAA16E).
inline int32_t gnRecentTickRate;
static_assert(sizeof(gnRecentTickRate) == 4, "gnRecentTickRate: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA777C
//  The raw count of game-logic ticks processed since gdwFrameStatsWindowStart was last reset; folded into gnRecentTickRate (/3, the signed idiom
//  `imul 0x55555556` at 0x6FAA9D58) at each ~3-second window boundary, then zeroed.
inline int32_t gnFrameCountInStatsWindow;
static_assert(sizeof(gnFrameCountInStatsWindow) == 4, "gnFrameCountInStatsWindow: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7780
//  gnSkippedTickCountInStatsWindow/3 as of the last ~3-second window boundary, the skipped-tick counterpart to gnRecentTickRate. It receives the
//  signed quotient of `imul 0x55555556` at 0x6FAA9D78.
inline int32_t gnRecentSkipRate;
static_assert(sizeof(gnRecentSkipRate) == 4, "gnRecentSkipRate: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7784
//  The raw count of frames where a game-logic tick was due but skipped (gnFramesSinceLastTick nonzero) since gdwFrameStatsWindowStart was last
//  reset; folded into gnRecentSkipRate (/3, the signed idiom `imul 0x55555556` at 0x6FAA9D78) at each ~3-second window boundary, then zeroed.
inline int32_t gnSkippedTickCountInStatsWindow;
static_assert(sizeof(gnSkippedTickCountInStatsWindow) == 4, "gnSkippedTickCountInStatsWindow: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7788
//  The timestamp GAME_UpdateFrame resets every ~3000ms (0xBB8), each time folding gnFrameCountInStatsWindow and gnSkippedTickCountInStatsWindow
//  (both /3, both then reset to 0) into gnRecentTickRate/gnRecentSkipRate: a rolling ~3-second window for the tick/skip rate the frame-pacing
//  catch-up logic reads.
inline uint32_t gdwFrameStatsWindowStart;
static_assert(sizeof(gdwFrameStatsWindowStart) == 4, "gdwFrameStatsWindowStart: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA778C
//  The load/transition grace-period deadline. GAME_ExtendCatchUpDeadline pushes it forward by an explicit millisecond amount whenever a load or
//  transition starts (SCMD_HandleSrv15_ReassignPlayer, UI_LoadScreenBegin, UI_UnloadLoadingScreen), and GAME_UpdateFrame compares
//  `GetTickCount() < gdwCatchUpResetDeadline` to gate its hard fixed-timestep catch-up reset, which re-bases gdwLastGameTickTime, zeroes
//  gnFramesSinceLastTick and forces gnRecentTickRate to 25. So time spent loading is not replayed afterwards as a burst of game-logic ticks.
inline uint32_t gdwCatchUpResetDeadline;
static_assert(sizeof(gdwCatchUpResetDeadline) == 4, "gdwCatchUpResetDeadline: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7794
//  Read by UNRESOLVED_sub_6FAABFE0 alone (`mov eax, dword ptr [0x6FBA7794]`). It lies in .bss, and the only writes are the two `rep stosd` that clear
//  0x6FBA7750..0x6FBA77CF (0x6FAA9477, 0x6FAAB963), which write 0, so the value is always 0 and the getter's two callers, MISSILE_CltDoFunc1 and
//  UNITMODE_StepUnitAlongPath, always take the branch 0 picks. The name claims nothing about the value.
inline int32_t UNRESOLVED_dword_6FBA7794;
static_assert(sizeof(UNRESOLVED_dword_6FBA7794) == 4, "UNRESOLVED_dword_6FBA7794: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7798
//  The tick before which the next interaction is refused. Reached only through GAME_GetInteractThrottle and GAME_SetInteractThrottle. All four
//  arming sites use `GAME_GetFrameTick() + 500`. Its two testers are SELECT_UpdateSelectedUnit and PLAYER_InteractOrApproachTarget's unit type 5
//  (warp) case, which is why clicking stairs or a waypoint repeatedly does nothing until half a second has passed. UNITMODE_RunQueuedCommand
//  re-arms it for the same target type when it runs a queued interact.
inline uint32_t gnInteractThrottleTick;
static_assert(sizeof(gnInteractThrottleTick) == 4, "gnInteractThrottleTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA779C
//  The sample count for gnNetworkLatencyRunningMean's incremental mean, capped at 10 by GAME_OnPongUpdateLatency (once 10 samples are averaged,
//  later pongs stop updating the mean).
inline uint32_t gnLatencySampleCount;
static_assert(sizeof(gnLatencySampleCount) == 4, "gnLatencySampleCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA77A0
//  The running mean of the first 10 GAME_OnPongUpdateLatency samples; gnLatencySampleCount is the sample count that gates it. Read back by the getter
//  GAME_GetNetworkLatencyRunningMean, which MSG_SendPing6C calls (0x6FAAD3DE).
inline uint32_t gnNetworkLatencyRunningMean;
static_assert(sizeof(gnNetworkLatencyRunningMean) == 4, "gnNetworkLatencyRunningMean: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA77A4
//  Where GAME_OnPongUpdateLatency copies the body of the pong packet: eight dwords, 32 bytes, from `pPacket + 1`, the +1 skipping the packet id
//  byte. Element [4] (0x6FBA77B4) is then overwritten with GetTickCount at the moment the pong arrived, so only seven of the eight dwords survive as
//  packet data. The latency itself is computed from a second GetTickCount call two instructions later, so that element is a record of arrival
//  rather than an input to the arithmetic; nothing else reads it.
inline uint32_t gPongPacketBody[8];
static_assert(sizeof(gPongPacketBody) == 32, "gPongPacketBody: the item's 32 bytes in the .i64");

//  D2Client 0x6FBA77C4
//  Is the joined game an expansion game: byte +6 of server packet 0x01. Distinct from FOG_IsExpansion, which is about the running client.
inline BOOL gbExpansionGame;
static_assert(sizeof(gbExpansionGame) == 4, "gbExpansionGame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA77C8
//  The ladder flag, byte +7 of server packet 0x01. Read through GAME_IsLadderGame, which only trusts it when gnGameType is 2 or 3.
inline BOOL gbLadderGame;
static_assert(sizeof(gbLadderGame) == 4, "gbLadderGame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA77CC
//  The frame number until which the local player's skills report SKILLUSTATE_BLOCKED (see PLRSKILLS_SkillsGetUseStateForClient).
inline int32_t gnSkillBlockUntilFrame;
static_assert(sizeof(gnSkillBlockUntilFrame) == 4, "gnSkillBlockUntilFrame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA77D0
//  GetTickCount at the moment server packet 0x05 arrived; the game loop disconnects when it is more than 0x2BF20 ms (3 minutes) old.
inline uint32_t gnConnectionTimestamp;
static_assert(sizeof(gnConnectionTimestamp) == 4, "gnConnectionTimestamp: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA77D4
//  A CRITICAL_SECTION (24 bytes). It serialises state between the game loop and the worker thread: the users are GAME_UpdateFrame, GAME_AppModeGame
//  and GAME_ServerThreadProc (the thread entry point). GAME_UpdateFrame's disconnect path takes it, sets a flag, releases it and then
//  WaitForSingleObject's on the thread handle. GAME_AppModeGame initialises it (InitializeCriticalSection, 0x6FAAB43A) before the thread is created,
//  and GAME_ServerThreadProc deletes it (DeleteCriticalSection, 0x6FAAA4B8) as the thread ends; the other references are Enter/LeaveCriticalSection.
inline struct _RTL_CRITICAL_SECTION gGameLoopCritSect;
static_assert(sizeof(gGameLoopCritSect) == 24, "gGameLoopCritSect: the item's 24 bytes in the .i64");

//  D2Client 0x6FBA77EC
//  The message id shown by the disconnect sequence (D2C_JoinErrors, 29 members), clamped to 9 when the caller passes 29 or more.
inline D2C_JoinErrors gnDisconnectMessage;
static_assert(sizeof(gnDisconnectMessage) == 4, "gnDisconnectMessage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA77F0
//  The 32 characters of the config's szRealm (+0xD0), copied by GAME_ApplyParamRealm. AUTOMAP_SaveExploredMap and CMDTBL_OpenPlayerKeyFile both test
//  its first byte and, when it is non-empty, use it as a string while building a file name, so it is a subdirectory of the save path.
inline char gszRealmName[32];
static_assert(sizeof(gszRealmName) == 32, "gszRealmName: the item's 32 bytes in the .i64");

//  D2Client 0x6FBA7810
//  The local player's character name, taken from the launch parameters by GAME_ApplyParamCharacterName. 16 bytes: the D2 character-name field, 15
//  characters and a terminator. The parser clears the buffer with four stores of a zeroed eax (`mov <here>, eax` then 0x6FBA7814, 0x6FBA7818,
//  0x6FBA781C).
inline char gLocalPlayerName[16];
static_assert(sizeof(gLocalPlayerName) == 16, "gLocalPlayerName: the item's 16 bytes in the .i64");

//  D2Client 0x6FBA7824
//  The handle of the game thread, the one GAME_AppModeGame creates with `CreateThread(0, 0, GAME_ServerThreadProc, 0, 0, ...)`. GAME_UpdateFrame
//  does `WaitForSingleObject(ghGameThread, 0x2710)` (10 seconds) and then clears the global to 0: the shutdown path gives the game thread ten
//  seconds to finish before giving up on it.
inline HANDLE ghGameThread;
static_assert(sizeof(ghGameThread) == 4, "ghGameThread: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7890
//  The join token from the launch parameters. GAME_ApplyParamJoinInfo parses it out and stores it here; GAME_AppModeGame reads it and hands it on as
//  nTokenId when joining. Its partner is gnJoinGameId.
inline int32_t gnJoinTokenId;
static_assert(sizeof(gnJoinTokenId) == 4, "gnJoinTokenId: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7894
inline uint8_t gnPlayerClass;
static_assert(sizeof(gnPlayerClass) == 1, "gnPlayerClass: the item's 1 bytes in the .i64");

//  D2Client 0x6FBA7898
//  The game name to join, taken from the launch parameters. GAME_ApplyParamJoinInfo `lstrcpyA`s it out of the parameter block at +0x1A;
//  GAME_AppModeGame passes it to MSG_Send_D2CLTSYS_NEWGAME as szName, which builds client packet 0x66 with a 16-byte szGameName at +1. The buffer
//  here is 24 bytes, so the copy is bounded by the source rather than by the destination. Empty until a join is parsed.
inline char szJoinGameName[24];
static_assert(sizeof(szJoinGameName) == 24, "szJoinGameName: the item's 24 bytes in the .i64");

//  D2Client 0x6FBA78B0
//  The OSVERSIONINFOA that GAME_AppModeGame fills: it stores 0x94 into dwOSVersionInfoSize at 0x6FAAB3A4 and passes the address to GetVersionExA
//  (0x6FAAB3AE, 0x6FAAB3B3).
inline struct _OSVERSIONINFOA gOsVersionInfo;
static_assert(sizeof(gOsVersionInfo) == 148, "gOsVersionInfo: the item's 148 bytes in the .i64");

//  D2Client 0x6FBA7944
//  The Battle.net interface pointer: a copy of the launch config's pComInterface (+0x21F) that GAME_ApplyParamComInterface, its only writer, stores
//  only when that is not NULL (0x6FAABE80). Read by GAME_AppModeGame directly (LeaveGame, +0xC4) and through the six-byte getter
//  GAME_GetComInterface (GetVersion +0x5C, GetBuild +0xB0). Every reader calls through it, and the offsets are its BnClientInterface's.
inline struct BnClientInterface *gpComInterface;
static_assert(sizeof(gpComInterface) == 4, "gpComInterface: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7948
//  GetTickCount at the moment GAME_InitGame ran, taken by the second of two back-to-back GetTickCount calls, the first of which stamps
//  gnConnectionTimestamp. GAME_UpdateFrame uses it as a 30-second deadline (`sub edi, <here>; cmp edi, 7530h; jbe skip`), and past that it checks
//  gbInGame and disconnects if the game never went live: the load watchdog. Distinct from gnConnectionTimestamp, which is stamped on packet 0x05
//  and carries a three-minute deadline.
inline int32_t gnGameStartTick;
static_assert(sizeof(gnGameStartTick) == 4, "gnGameStartTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA794C
//  Green blood is on. GAME_InitGame tests it and, when clear, opens Data\Local\Color.txt through ARCHIVE_OpenFileOrTrace (0x6FB6B91C) with
//  bQuietIfMissing = 1; it is set to 1 only when the file's first byte is '1' (`cmp [esp+10h+pBuffer], 31h`, 0x6FAA94E8). A missing file or any
//  other byte leaves it 0, and every game reads the file again. Read outside that function only through GAME_IsGreenBloodEnabled.
inline int32_t gbUseGreenBlood;
static_assert(sizeof(gbUseGreenBlood) == 4, "gbUseGreenBlood: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7950
//  Whether the beta-slides message handlers are currently registered. GAME_AppModeGame tests it right after WINMAIN_AppModeEnqueue and, when set,
//  calls `WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(7), gBetaSlidesMessageHandlers)` before clearing it.
//  Nothing sets it: its three references are the two reads and this clear, and it lies past .data's raw data, 0 at load, so the slide show it gates
//  does not run in a stock 1.10 client unless something stores to it through a pointer (not looked for).
inline int32_t gbBetaSlidesRegistered;
static_assert(sizeof(gbBetaSlidesRegistered) == 4, "gbBetaSlidesRegistered: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7954
//  An end-of-game sequence is pending: do not start anything else. GAME_AppModeGame, on seeing it set, plays VIDEO_PlayCinematic(5) (ACT04END%s.BIK,
//  the classic ending) and tears the window down. In GAME_BeginDisconnect it is one of three guards (`!gnDisconnectCountdown &&
//  !gbEndOfGamePending && !gbGameTeardownActive`) that make the function a no-op, and GAME_UpdateFrame applies the same triple twice. Set by
//  GAME_MarkClassicCompletion alongside writing the new difficulty into the launch config.
inline BOOL gbEndOfGamePending;
static_assert(sizeof(gbEndOfGamePending) == 4, "gbEndOfGamePending: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7958
//  Set when the player dismisses the Diablo-vanquished screen: by GAME_VanquishedOnButtonUp on a release inside the button, or by
//  GAME_VanquishedOnKeyDown on any non-repeat key. GAME_ShowDiabloVanquishedMessage is its only reader and ends the message pump on it. Cleared by
//  GAME_InitGame.
inline BOOL gbVanquishedDismissed;
static_assert(sizeof(gbVanquishedDismissed) == 4, "gbVanquishedDismissed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA795C
//  The current game difficulty: 0 Normal, 1 Nightmare, 2 Hell (D2C_Difficulties at its setter). GAME_SetDifficulty writes only its low byte, from
//  SCMD_HandleSrv01_GameFlags' packet byte +1; GAME_GetDifficulty returns it as an unsigned byte. Also read by GAME_AppModeGame,
//  GAME_PreloadActData and GAME_ShowDiabloVanquishedMessage.
inline uint8_t gnDifficulty;
static_assert(sizeof(gnDifficulty) == 1, "gnDifficulty: the item's 1 bytes in the .i64");

//  D2Client 0x6FBA7960
//  The game type the launch configuration selected, copied out of D2ConfigStrc::dwGameType by GAME_ApplyParamJoinInfo and read through
//  GAME_GetGameType (0x6FAABFF0). The enum's extent is the compiler's: WINMAIN_D2ClientEntrypoint guards its switch on this value with
//  `cmp eax, 9` over a dense byte table, so 0..9. What the module does with it: 0 and 1 start a local server, 6 and 8 host, 3, 7 and 9 join, 4 is
//  the only arm that uses the hardcoded IP, and 2 and 3 are the realm types. 5 never appears anywhere.
inline eD2GameTypes gnGameType;
static_assert(sizeof(gnGameType) == 4, "gnGameType: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7968
//  The connection-level companion to gbInGame: at every site that clears either flag the two are written in the same two instructions
//  (`mov gbInGame, eax; mov <here>, eax` in GAME_UpdateFrame, `mov gbInGame, edi; mov <here>, edi` in UI_SetUIToggle's leave path). gbInGame is
//  raised by SCMD_HandleSrv04_LoadComplete (the level has loaded); this one is raised by SCMD_SaveCharacterFile, which sets 1 on its success path
//  and 0 twenty bytes later on its failure path, so it goes true earlier, at connection rather than at load.
//  MSG_JoinGameMessageLoop's tail is `mov ecx, <here>; test ecx, ecx; setnz al`: the loop reports success by returning this flag as a boolean.
//  MSG_LeaveGameMessageLoop reads it the same way.
inline int32_t gbConnectedToGame;
static_assert(sizeof(gbConnectedToGame) == 4, "gbConnectedToGame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA796C The game is live and accepting input. SCMD_HandleSrv04_LoadComplete sets it to 1; SCMD_HandleSrv05_UnloadComplete,
//  MSG_Send_D2CLTSYS_ENDGAME, GAME_BeginDisconnect, UI_SetUIToggle's leave path, both disconnect paths in GAME_UpdateFrame and GAME_InitGame's init
//  clear it. Every window-message handler gates on it (OnLButtonDown/Up, OnRButtonDown/Up, OnMouseWheel, OnXButtonDown/Up, OnActivateApp), as do
//  PANEL_GameOnMouseMove, GAME_CmdTblActionClearScreen and SOUNDINST_ServiceInstances. In SCMD_HandleSrv06_GameExit it is the realm-side counterpart
//  of gbLeaveGameSent: on game types 2 and 3, still being in game when packet 0x06 arrives means GAME_BeginDisconnect(7).
inline BOOL gbInGame;
static_assert(sizeof(gbInGame) == 4, "gbInGame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7970
//  A disconnect/exit sequence is running. Set by GAME_BeginDisconnect and the packet 0x06 handler; read through GAME_IsDisconnecting.
inline BOOL gbDisconnecting;
static_assert(sizeof(gbDisconnecting) == 4, "gbDisconnecting: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7974 The server has told us to leave. SCMD_HandleSrv05_UnloadComplete sets it to 1 and, in the same two instructions, stamps
//  gnConnectionTimestamp with GetTickCount (the start of a three-minute disconnect deadline). SCMD_HandleSrv04_LoadComplete sets `gbInGame = 1` and
//  clears this to 0 three instructions later. GAME_InitGame clears it at startup. Read three times by GAME_UpdateFrame (the game loop's exit test)
//  and by UI_LoadScreenBegin and UI_DrawLoadingScreen, which is how a pending exit suppresses the loading screen.
inline int32_t gbGameExitPending;
static_assert(sizeof(gbGameExitPending) == 4, "gbGameExitPending: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7978
//  Play the expansion's ending cutscene when the game is left. GAME_InitGame initialises it, GAME_SetD2xOutCinematicPending writes it (armed by
//  QUESTLOG_QuestOnQuestStatePacket, disarmed by VIDEO_PlayCinematic itself), and GAME_AppModeGame is the only reader: when set it calls
//  VIDEO_PlayCinematic(7) (`D2X_OUT%s.BIK` in gCinematics) and then raises gbGameTeardownActive.
inline BOOL gbD2xOutCinematicPending;
static_assert(sizeof(gbD2xOutCinematicPending) == 4, "gbD2xOutCinematicPending: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA797C
//  A cinematic or game teardown is already running: the twin guard to gbEndOfGamePending, which appears beside it in every guard test
//  (GAME_BeginDisconnect once, GAME_UpdateFrame twice). Raised by GAME_SetGameTeardownActive (sole caller VIDEO_PlayCinematic, so it goes up while a
//  cinematic plays) and by GAME_AppModeGame, and cleared by GAME_InitGame when a new game starts. gbEndOfGamePending is instead raised by
//  GAME_MarkClassicCompletion to request the ending.
inline BOOL gbGameTeardownActive;
static_assert(sizeof(gbGameTeardownActive) == 4, "gbGameTeardownActive: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7984
//  The client's current act, the D2ActStrc that D2COMMON_DUNGEON_FreeAct releases. GAME_AppModeGame creates and frees it around a game:
//  `if (gpAct) D2COMMON_DUNGEON_FreeAct(gpAct); gpAct = NULL;`.
inline D2DrlgActStrc *gpAct;
static_assert(sizeof(gpAct) == 4, "gpAct: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7988
//  An 8-byte block allocated by GAME_InitGame (`FOG_Alloc(8, <Game.cpp>, 1339, 0)`, 1339 being the source line), on the same run of statements that
//  clears the unit hash tables and the ambient light grid and calls ENV_Init, so it is part of the per-game setup rather than per-act. Every read
//  dereferences it (`mov ecx, <here>; mov edx, [ecx]`). The readers are AUTOMAP_LoadCelData, AUTOMAP_RebuildDrawLists, AUTOMAP_SaveExploredMap,
//  GAME_PreloadActData (which zeroes and then writes its two dwords, the seeds) and GAME_AppModeGame (which frees it).
//  NOT ESTABLISHED: what else the block contains; it is allocated once per game and shared between the automap and the act preloader.
inline void *gpClientGameBlock;
static_assert(sizeof(gpClientGameBlock) == 4, "gpClientGameBlock: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA798C
//  The level id GAME_PreloadActData last preloaded data for (its single writer). Read by UI_LoadScreenBegin and UI_UnloadLoadingScreen, the two ends
//  of the loading screen, which makes it the 'what have we already got in memory' marker rather than the current level.
inline int32_t gnPreloadedLevelId;
static_assert(sizeof(gnPreloadedLevelId) == 4, "gnPreloadedLevelId: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7990
//  The client's one viewport record, created in GAME_InitGame and released by VIEW_Free in GAME_AppModeGame alongside gpAct. The functions that
//  read it go through the VIEW_* accessors rather than touching fields.
inline D2ClientViewStrc *gpView;
static_assert(sizeof(gpView) == 4, "gpView: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7994
//  Which beta screen is showing. Only the low byte is used: the bytes read it either as a byte (`mov dl, byte ptr`, 0x6FAAA500; `mov cl, byte ptr`,
//  0x6FAAA743) or as the whole dword masked (`and edx, 0FFh`, 0x6FAAA591 and on), and every write is a byte store; the upper three bytes are 0 at
//  load. So the index wraps at 256 rather than growing. Advanced by GAME_BetaSlidesTick on the 10-second timer and by both input handlers; never
//  bounds-checked at the point of increment, because GAME_BetaSlidesTick tests it against gnBetaScreenCount at the top of the next frame.
inline uint8_t gnBetaScreenIndex;
static_assert(sizeof(gnBetaScreenIndex) == 1, "gnBetaScreenIndex: the item's 1 bytes in the .i64");

//  D2Client 0x6FBA7998
//  An 'input is up' latch: GAME_AppModeGame stores 1 (0x6FAAB3BD) once GetVersionExA has filled gOsVersionInfo, and it is never cleared. All four OS
//  predicates (GAME_HasNativeMouseWheel, GAME_NeedsWin95WheelWorkaround, GAME_IsSupportedWindows, GAME_IsWin2000OrXp) gate on it, so it gates every
//  wheel and extra-button registration.
inline BOOL gbOsVersionKnown;
static_assert(sizeof(gbOsVersionKnown) == 4, "gbOsVersionKnown: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA799C
//  The frames left in the disconnect sequence; GAME_BeginDisconnect sets it to 600 and refuses to start while it is non-zero.
inline int32_t gnDisconnectCountdown;
static_assert(sizeof(gnDisconnectCountdown) == 4, "gnDisconnectCountdown: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA79A0
//  The stop signal for the game thread. GAME_UpdateFrame sets it to 1 and then waits on ghGameThread with a ten-second timeout; GAME_AppModeGame
//  clears it when the thread is created. GAME_ServerThreadProc, the thread body, is the only reader. The shutdown handshake: raise this, wait ten
//  seconds, clear the handle.
inline int32_t gbGameThreadStop;
static_assert(sizeof(gbGameThreadStop) == 4, "gbGameThreadStop: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA79A4
//  'The game thread may run a frame.' GAME_AppModeGame clears it before creating the thread and sets it to 1 afterwards, both inside
//  gGameLoopCritSect; GAME_ServerThreadProc reads it under the same critical section, immediately after reading gbGameThreadStop, and skips its work
//  when it is 0 (stop is tested first and exits the loop). ghGameThread, gbGameThreadStop and this are one handshake, all touched inside the same
//  critical section.
inline int32_t gbGameLoopEnabled;
static_assert(sizeof(gbGameLoopEnabled) == 4, "gbGameLoopEnabled: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA79A8
//  Set to 1 by GAME_WindowOnLButtonDown and to 0 by GAME_WindowOnLButtonUp. GAME_WindowOnActivateApp also writes it (losing focus drops the button
//  state), and GAME_ResetMouseButtonState clears it with its three neighbours.
inline BOOL gbLeftButtonDown;
static_assert(sizeof(gbLeftButtonDown) == 4, "gbLeftButtonDown: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA79AC
//  The right mouse button held state, the counterpart to gbLeftButtonDown: GAME_UpdateFrame turns a held right button into a repeated
//  PLAYER_OnMouseClick(4, ...) each frame the same way it does for the left button (mouse code 1).
inline BOOL gbRightButtonDown;
static_assert(sizeof(gbRightButtonDown) == 4, "gbRightButtonDown: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA79B0
//  Blocks GAME_UpdateFrame's held-mouse-button auto-repeat: while set, a held button does not turn into a PLAYER_OnMouseClick that frame, so the
//  click is swallowed rather than repeated. Raised by GAME_SuppressHeldClick and cleared by every mouse transition (GAME_WindowOnLButtonDown/Up,
//  GAME_WindowOnRButtonDown/Up) and by GAME_WindowOnActivateApp, so the next genuine press always gets through and regaining focus never leaves
//  the suppression stuck on.
inline BOOL gbSuppressHeldClick;
static_assert(sizeof(gbSuppressHeldClick) == 4, "gbSuppressHeldClick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA79B4
//  The Run key is currently held. The second of the two sources GAME_GetMovementFlags ORs into movement-flag bit 3, alongside gbRunToggled, so the
//  player runs if either the key is down or the lock is on. Cleared by CMDTBL_ActionRunReleased through GAME_SetRunKeyHeld(0), and by
//  GAME_WindowOnClose and GAME_WindowOnActivateApp (losing focus drops the key).
inline BOOL gbRunKeyHeld;
static_assert(sizeof(gbRunKeyHeld) == 4, "gbRunKeyHeld: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA79B8
//  The run/walk lock: nonzero means the player runs without holding the Run key. GAME_GetMovementFlags ORs it into bit 3 of the movement flags, the
//  bit UNITMODE_MoveToTargetAndQueueCommand uses to pick command 4 (run to entity) over command 2 (walk). CMDTBL_ActionToggleRunWalk (the bindable
//  'Toggle Run/Walk' command) does nothing but `x = (x == 0)`, and PANEL_Draw800BorderFrame reads it to draw the control panel's run/walk indicator.
inline BOOL gbRunToggled;
static_assert(sizeof(gbRunToggled) == 4, "gbRunToggled: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA79BC
//  Whether the bound Stand Still hotkey (primary or secondary binding, HOTKEY_STANDSTILL) is currently held down (via GetAsyncKeyState), recomputed
//  once per GAME_UpdateFrame call. Contributes the value 4 (`or ecx, 4`, 0x6FAA9BE6) to the modifier flag word PLAYER_OnMouseClick receives.
inline BOOL gbStandStillKeyHeld;
static_assert(sizeof(gbStandStillKeyHeld) == 4, "gbStandStillKeyHeld: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA79C4
//  Set by GAME_UpdateFrame when it issues a PLAYER_OnMouseClick(1, ...) for a held left button this frame. Its one reader is
//  GAME_GetLeftClickIssuedThisFrame (0x6FAA8CB0); GAME_VanquishedOnButtonDown/Up, GAME_ResetMouseButtonState and GAME_ResetClickState only store
//  to it. Companion to gbRightClickIssuedThisFrame.
inline BOOL gbLeftClickIssuedThisFrame;
static_assert(sizeof(gbLeftClickIssuedThisFrame) == 4, "gbLeftClickIssuedThisFrame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA79C8
//  Set by GAME_UpdateFrame when it turns a held right button into a PLAYER_OnMouseClick(4, ...) this frame, the mirror of
//  gbLeftClickIssuedThisFrame. Also raised by GAME_WindowOnRButtonUp and GAME_WindowOnActivateApp, and cleared by GAME_ResetMouseButtonState,
//  GAME_ResetClickState and the two GAME_Vanquished button handlers. Nothing reads it: its references are stores.
inline BOOL gbRightClickIssuedThisFrame;
static_assert(sizeof(gbRightClickIssuedThisFrame) == 4, "gbRightClickIssuedThisFrame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA79CC
//  The accumulator for mouse-wheel notches. GAME_WindowOnMouseWheel reads it, adds the incoming delta, stores it back, and clears it to 0 once a
//  whole notch has been consumed (the usual WHEEL_DELTA accumulation). All its references are inside that one handler.
inline int32_t gnMouseWheelAccum;
static_assert(sizeof(gnMouseWheelAccum) == 4, "gnMouseWheelAccum: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA79D0
//  'A quest state change has invalidated the act's environment.' The setter is GAME_EnvReapplyActEnvironment (0x6FAA9260), which
//  QUESTLOG_QuestOnQuestStatePacket calls: with an act it re-initialises the environment at once (D2COMMON_ENVIRONMENT_InitializeEnvironment,
//  0x6FAA927B) and returns, the flag untouched; with none it only sets the flag (0x6FAA9281), and GAME_PreloadActData re-initialises when it next
//  loads act index 1 (`cmp bl, 1`, 0x6FAA934B), clearing it either way. A byte, not a dword: the read is `mov cl, <here>`.
inline uint8_t gbActEnvironmentDirty;
static_assert(sizeof(gbActEnvironmentDirty) == 1, "gbActEnvironmentDirty: the item's 1 bytes in the .i64");

//  D2Client 0x6FBA79D4 Set when this client asked to leave the game (always alongside a clt 0x68) and zeroed by GAME_InitGame when a new game starts.
//  Written by GAME_SetLeaveGameSent's callers and inline by GAME_WindowOnClose and GAME_LeaveGameAfterCinematic. Read in exactly one place:
//  SCMD_HandleSrv06_GameExit, on game types 7 and 9, calls GAME_BeginDisconnect(8) only when this is clear, so the flag separates "the server dropped
//  us" from "we asked to go" and stops a voluntary exit from being reported as a disconnect.
inline BOOL gbLeaveGameSent;
static_assert(sizeof(gbLeaveGameSent) == 4, "gbLeaveGameSent: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA79D8
//  How far the view scrolled in X since the last frame: `thisFrameX - gnPrevViewOffsetX`, computed in GAME_DrawGameScene. Read with its Y twin by
//  ENV_UpdateBubbles and ENV_UpdateWeather, which shift their particles by the scroll so weather does not slide across the world as the camera
//  moves.
inline int32_t gnViewScrollDeltaX;
static_assert(sizeof(gnViewScrollDeltaX) == 4, "gnViewScrollDeltaX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA79DC
//  How far the view scrolled in Y since the last frame: `thisFrameY - gnPrevViewOffsetY`. Read by ENV_UpdateBubbles and ENV_UpdateWeather; see
//  gnViewScrollDeltaX.
inline int32_t gnViewScrollDeltaY;
static_assert(sizeof(gnViewScrollDeltaY) == 4, "gnViewScrollDeltaY: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA79E0
//  Incremented every GAME_UpdateFrame call while gbInGame, reset to 0 whenever a game-logic tick actually ran (or on a hard catch-up reset);
//  compared against a lag-severity threshold to decide whether the current frame should skip drawing-adjacent bookkeeping
//  (gnSkippedTickCountInStatsWindow increments only while this is nonzero). Signed: `cmp gnFramesSinceLastTick, ecx / jle` at 0x6FAAA14C and
//  `cmp esi, edi / setle dl` at 0x6FAAA272 are both signed.
inline int32_t gnFramesSinceLastTick;
static_assert(sizeof(gnFramesSinceLastTick) == 4, "gnFramesSinceLastTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA79E4
//  Whether the Diablo-vanquished screen's button is currently held down: set by GAME_VanquishedOnButtonDown inside the rectangle, cleared
//  unconditionally by GAME_VanquishedOnButtonUp. Read only by GAME_ShowDiabloVanquishedMessage, which uses it to draw the pressed state. It is not
//  cleared when the press lands outside the button: the down handler's else-branch leaves it alone.
inline BOOL gbVanquishedButtonHeld;
static_assert(sizeof(gbVanquishedButtonHeld) == 4, "gbVanquishedButtonHeld: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA79E8
//  The backdrop of the Diablo-vanquished end-of-game screen, loaded by GAME_ShowDiabloVanquishedMessage from `DATA\GLOBAL\ui\menu\%s`, where %s is
//  `endgame2` when the joined game is an expansion game (gbExpansionGame) and `endgame` when it is not. The function loads it, draws it under the
//  message, and unloads both this and gpImgVanquishedOkButton before returning, clearing both globals; the screen exists only for the length of its
//  own message pump, which ends when gbVanquishedDismissed goes true.
inline D2CellFileStrc *gpImgVanquishedScreen;
static_assert(sizeof(gpImgVanquishedScreen) == 4, "gpImgVanquishedScreen: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA79EC
//  The OK button of the Diablo-vanquished screen: `DATA\GLOBAL\ui\menu\endgameok`, loaded and freed in the same breath as gpImgVanquishedScreen. The
//  two loads are preceded by `%s\Palette\%s\pal.dat` and `%s\Palette\%s\pal.pl2` going into D2WIN_10028, so the screen installs its own palette
//  before drawing.
inline D2CellFileStrc *gpImgVanquishedOkButton;
static_assert(sizeof(gpImgVanquishedOkButton) == 4, "gpImgVanquishedOkButton: the item's 4 bytes in the .i64");

//  gGameData's container: its three vtables of ours, the bucket array's Storm tag, the table itself and gGameList.

//  D2Client 0x6FB6E23C
//  The first of four 4-slot vtables that CRT_GAMEDATA_ConstructTable assigns in succession as the constructor chain climbs the SGAMEDATA
//  container's class hierarchy. Slots: 0 FreeNode, 1 AllocNode, 2 vector deleting destructor, 3 destroy-all. The four share one shape,
//  SGameDataTableVtbl (Storm's TSHashTable virtuals: InternalDelete, InternalNew, the deleting destructor, Destroy), each slot typed by its
//  function's prototype. Slots 0 and 1 are identical across all four; slot 2 differs in all four, and slot 3 is GAME_SGameDataHashDestroyAll in
//  vtables 1, 2 and 3 and GAME_SGameDataListDestroyAll here.
//  References: stores into a table's pVtbl by the original constructor chain (0x6FAAC366) and by four destructors of ours (0x6FAA8A77, 0x6FAAC542,
//  0x6FAAC90C, 0x6FAACD8F); no compare.
inline const SGameDataTableVtbl gVtblSGameData0 =
{
    .pfnInternalDelete = &GAME_SGameDataFreeNode,
    .pfnInternalNew = &GAME_SGameDataAllocNode,
    .pfnDeletingDtor = &GAME_SGameDataVecDelDtor0,
    .pfnDestroy = &GAME_SGameDataListDestroyAll,
};
static_assert(sizeof(gVtblSGameData0) == 16, "gVtblSGameData0: the item's 16 bytes in the .i64");

//  D2Client 0x6FB6E24C
//  Vtable 1 of four; see gVtblSGameData0. References: the constructor chain's store (0x6FAAC3F9) and three of our destructors' (0x6FAA899C,
//  0x6FAAC832, 0x6FAACCD0).
inline const SGameDataTableVtbl gVtblSGameData1 =
{
    .pfnInternalDelete = &GAME_SGameDataFreeNode,
    .pfnInternalNew = &GAME_SGameDataAllocNode,
    .pfnDeletingDtor = &GAME_SGameDataVecDelDtor1,
    .pfnDestroy = &GAME_SGameDataHashDestroyAll,
};
static_assert(sizeof(gVtblSGameData1) == 16, "gVtblSGameData1: the item's 16 bytes in the .i64");

//  D2Client 0x6FB6E25C
//  Vtable 2 of four; see gVtblSGameData0. The live table's vtable: the last of the constructor chain's four stores (0x6FAAC432), so a table nothing
//  has begun to destroy carries this one; our construction installs it. D2Game calls its slots on gGameData: InternalNew at D2Game 0x6FC3B630
//  (`call dword ptr [eax+4]`, in the table's New), so a game D2Game makes is allocated by GAME_SGameDataAllocNode. gVtblSGameData3 (0x6FB6E26C)
//  stays the original's: its one store, 0x6FAAC418, is overwritten by this one's with no call between them that reads it, and our construction
//  does not make it.
inline const SGameDataTableVtbl gVtblSGameData2 =
{
    .pfnInternalDelete = &GAME_SGameDataFreeNode,
    .pfnInternalNew = &GAME_SGameDataAllocNode,
    .pfnDeletingDtor = &GAME_SGameDataVecDelDtor2,
    .pfnDestroy = &GAME_SGameDataHashDestroyAll,
};
static_assert(sizeof(gVtblSGameData2) == 16, "gVtblSGameData2: the item's 16 bytes in the .i64");

//  D2Client 0x6FB75EA0
//  The name field of an RTTI type descriptor, `.?AV?$TSExplicitList@USGAMEDATA@@$0?CCCCCCCD@@@`: 47 characters and the NUL. The template hands
//  typeid(T).raw_name() to Storm as the "file" of its allocations, so this is the tag of the SMem calls in the list code
//  (GAME_SGameDataHashUnlinkListsAndFree, GAME_SGameDataHashUnlinkNodesAndFree, GAME_SGameDataListDestroyAll, GAME_SGameDataListDestruct, and
//  our construction's two allocations).
inline char szTSExplicitListSGameDataRawName[48] = ".?AV?$TSExplicitList@USGAMEDATA@@$0?CCCCCCCD@@@";
static_assert(sizeof(szTSExplicitListSGameDataRawName) == 48, "szTSExplicitListSGameDataRawName: the item's 48 bytes in the .i64");

//  D2Client 0x6FBA7828
//  The client's game-data object, 104 bytes, not a pointer: every reference takes its address. A static C++ object, zero here as the original's is
//  at load, built by Game.cpp's sgGameDataLifetime in our DLL's static initialisation and torn down by it at our DLL's exit. Its type
//  SGameDataTableStrc is the layout every function reads it through; MSG_InitGame passes it to D2Game's GAME_InitGameDataTable, which keeps the
//  pointer (D2Game 0x6FC358A6).
inline SGameDataTableStrc gGameData;
static_assert(sizeof(gGameData) == 104, "gGameData: the item's 104 bytes in the .i64");

//  D2Client 0x6FBA7980
//  The game-list buffer MSG_InitGame hands to D2Game's GAME_InitGameDataTable (the call pushes &gGameData and &<here> adjacently). One byte, with no
//  extent. The callee (D2Game 0x6FC35880) keeps pGameDataTbl (0x6FC358A6) and only tests pGameList (`mov eax, [esp+pGameList]` / `test eax, eax` /
//  `jnz` to its `retn 8`, 0x6FC358AB..0x6FC358B1, else FOG_DisplayAssert("phGameList") and exit); it neither writes through it nor keeps it, so the
//  buffer is filled by nothing and its address is the whole of its use.
inline uint8_t gGameList;
static_assert(sizeof(gGameList) == 1, "gGameList: the item's 1 bytes in the .i64");

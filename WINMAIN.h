//  WINMAIN.h -- the globals of WINMAIN.cpp's data that are ours: the app-mode dispatch table and queue, the command-line tables, the launch
//  configuration and the other state of the entry point. Each is a C++17 `inline` variable, one object for every file that includes this header.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. gafnCommandsToRegister and gaCommandLineOptions lie in
//  .rdata and are const. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB74538
//  Decides what WINMAIN_D2ClientEntrypoint returns: set -> APPMODE_CONTINUE (0); clear -> it switches on pConfig->dwGameType and returns
//  APPMODE_STARTUP (3) for the four Battle.net game types or APPMODE_CLOSEWINDOW (4) for single player and LAN. Initialised to 1 and written
//  afterwards only through WINMAIN_SetAppModeContinue.
inline BOOL gbAppModeContinue = 1;
static_assert(sizeof(gbAppModeContinue) == 4, "gbAppModeContinue: the item's 4 bytes in the .i64");

//  D2Client 0x6FB6E0C8
//  The startup command registration list: ONE entry, LNG_RegisterCmdLineArguments (the loop in WINMAIN_D2ClientEntrypoint stops at 4 bytes). It
//  is called with nothing pushed and its answer tested.
inline int32_t (__cdecl *const gafnCommandsToRegister[1])() = { LNG_RegisterCmdLineArguments };

//  D2Client 0x6FB6E0D0
//  SCmdArg[8], the command-line switch table, registered by `SCmdRegisterArgList(gaCommandLineOptions, 8)` in WINMAIN_D2ClientEntrypoint. A
//  16-byte record is {dword, dword always 0xFFFFFFFF, char *name, handler}; dword0 is 0x00020000 for the two switches that take a value ("set",
//  "mode") and 0 for the rest. Storm reads each row's four dwords and never keeps the table's address.
//      [0] "?"     WINMAIN_CmdArgDisplayHelp        [4] "mode"  WINMAIN_CmdArgMode
//      [1] "h"     WINMAIN_CmdArgDisplayHelp        [5] "c"     WINMAIN_CmdArgPreCache
//      [2] "help"  WINMAIN_CmdArgDisplayHelp        [6] "r"     WINMAIN_CmdArgRecord
//      [3] "set"   WINMAIN_CmdArgSet                [7] "p"     WINMAIN_CmdArgPlay
inline const SCmdArg gaCommandLineOptions[8] =
{
    { 0x00000000, 0xFFFFFFFF, (char *)"?",    WINMAIN_CmdArgDisplayHelp },
    { 0x00000000, 0xFFFFFFFF, (char *)"h",    WINMAIN_CmdArgDisplayHelp },
    { 0x00000000, 0xFFFFFFFF, (char *)"help", WINMAIN_CmdArgDisplayHelp },
    { 0x00020000, 0xFFFFFFFF, (char *)"set",  WINMAIN_CmdArgSet },
    { 0x00020000, 0xFFFFFFFF, (char *)"mode", WINMAIN_CmdArgMode },
    { 0x00000000, 0xFFFFFFFF, (char *)"c",    WINMAIN_CmdArgPreCache },
    { 0x00000000, 0xFFFFFFFF, (char *)"r",    WINMAIN_CmdArgRecord },
    { 0x00000000, 0xFFFFFFFF, (char *)"p",    WINMAIN_CmdArgPlay },
};

//  D2Client 0x6FB744EC
//  The app-mode dispatch table: one entry point per D2C_AppMode, indexed directly by the mode: [2] GAME_AppModeGame, [3] GAME_AppModeStartup,
//  [4] GAME_AppModeCloseWindow, [5] MSG_AppModeJoinGame, [6] MSG_AppModeLeaveGame. [0] and [1] are null because CONTINUE and SHOULDQUIT are
//  answers a handler returns, never modes to dispatch to (the loop's own two asserts refuse both).
//  Seven entries, bounded by gpLaunchParamCategories at the next address. That bounds the dispatchable modes and not the enum: the guard after
//  the call is `cmp eax, 0Ch` under the assert `gnAppMode < APPMODE_LAST`, so D2C_AppMode runs to 12, and gpLaunchParamCategories names four modes
//  with no entry here ('Tyler' 7, 'Path' 8, 'Test' 9, 'Composite' 10). Safe rather than an overrun: the loop starts at a hardcoded
//  APPMODE_STARTUP and gnAppMode has no reader.
inline D2C_AppMode (__fastcall *gafnAppModeEntryPoints[7])() =
{
    NULL,                   // [0] APPMODE_CONTINUE
    NULL,                   // [1] APPMODE_SHOULDQUIT
    GAME_AppModeGame,           // [2]
    GAME_AppModeStartup,        // [3]
    GAME_AppModeCloseWindow,    // [4]
    MSG_AppModeJoinGame,       // [5]
    MSG_AppModeLeaveGame,      // [6]
};

//  The extent is the binary's.
static_assert(sizeof(gafnAppModeEntryPoints) / sizeof(gafnAppModeEntryPoints[0]) == 7,
              "gafnAppModeEntryPoints is 28 bytes at 0x6FB744EC; index 7 is gpLaunchParamCategories");

//  D2Client 0x6FB6E0C0
//  A pointer to a string, not a string: the caption of the MessageBoxA of the command-line help paths. In the image it holds the pooled
//  "Diablo II" of szKeyName (Lng.h); here it holds an equal literal of its own, so this header needs no other. Never written.
inline const LPCSTR gpszMsgBoxCaption = "Diablo II";
static_assert(sizeof(gpszMsgBoxCaption) == 4, "gpszMsgBoxCaption: the item's 4 bytes in the .i64");

//  D2Client 0x6FB6E150
//  A pointer to a string, not a string: the command-line help text, which begins "Available command line options:", passed as MessageBoxA's
//  lpText by the same four functions. Never written. The text is split into adjacent literals at its line breaks to keep the lines short; the
//  bytes are the image's.
inline const LPCSTR gpszCmdLineHelpText =
    "Available command line options:\012"
    "  -set[default command line]\011\011Set new default command line string\012"
    "  -lng[lngfile]\011\011\011Use \"lngfile\" for language resources\012"
    "  -s[name]\011\011\011\011Attach to server \"name\"\012"
    "  -mode[mode]\011\011\011Start in [mode], where mode is\012"
    "  -c\011\011\011\011Precache\012"
    "  \011\011\011\011game\012"
    "  \011\011\011\011Tyler\012"
    "  \011\011\011\011Path\012"
    "  \011\011\011\011Composite\012";
static_assert(sizeof(gpszCmdLineHelpText) == 4, "gpszCmdLineHelpText: the item's 4 bytes in the .i64");

//  D2Client 0x6FB744D8
//  The current app mode: WINMAIN_D2ClientEntrypoint's two asserts spell it out (`gnAppMode < APPMODE_LAST` and `gnAppMode > APPMODE_CONTINUE`) and
//  both guards test the value stored here one instruction earlier. WINMAIN_CmdArgMode records a gpLaunchParamCategories index here, which IS the
//  app mode, and the entry point writes it twice. Three writers, zero readers: `-mode Path` lands a valid mode here and nothing consults it.
inline D2C_AppMode gnAppMode = APPMODE_STARTUP;
static_assert(sizeof(gnAppMode) == 4, "gnAppMode: the item's 4 bytes in the .i64");

//  D2Client 0x6FB74508
//  The `-mode` switch's category names, eleven slots, five populated, and they are app modes at their own indices: [2] "Game", [7] "Tyler",
//  [8] "Path", [9] "Test", [10] "Composite" (APPMODE_GAME 2, _TYLER 7, _PATH 8, _TEST 9, _COMPOSITE 10). The six NULLs are the modes with no
//  command-line name. WINMAIN_CmdArgMode walks all 11, skips a NULL and writes the matched index into gnAppMode. It also bounds
//  gafnAppModeEntryPoints at seven entries by owning its index 7.
inline LPCSTR gpLaunchParamCategories[11] = { NULL, NULL, "Game", NULL, NULL, NULL, NULL, "Tyler", "Path", "Test", "Composite" };
static_assert(sizeof(gpLaunchParamCategories) == 44, "gpLaunchParamCategories: the item's 44 bytes in the .i64");

//  D2Client 0x6FB9A828
//  A single read-only zero byte shared as an empty string: callers read the byte (`buf[0] = gszEmptyString`), take its address as an empty
//  string, or copy it as a string (TEXT_HandleSystemMessage). Nothing writes it.
inline char gszEmptyString;
static_assert(sizeof(gszEmptyString) == 1, "gszEmptyString: the item's 1 bytes in the .i64");

//  D2Client 0x6FB9A830
//  The startup path buffer, MAX_PATH (260 bytes); WINMAIN_D2ClientEntrypoint passes GetModuleFileNameA a length of 260.
inline char Filename[260];
static_assert(sizeof(Filename) == 260, "Filename: the item's 260 bytes in the .i64");

//  D2Client 0x6FB9A934
//  The pending app-mode queue WINMAIN_AppModeEnqueue pushes onto, with sgnLastAppmode as its cursor. Five entries, named by the game's own assert:
//  `sgnLastAppmode < APPMODE_STACK_SIZE` against 5 before writing. It starts where Filename ends, and its 20 bytes run to gpLaunchConfig.
//  The dequeue only shifts three of the five: slot [4] is written by WINMAIN_AppModeEnqueue when the count is 4 and read by nothing.
//  Unreachable, though -- the queue never gets deeper than 1; the proof is on WINMAIN_AppModeEnqueue.
inline D2C_AppMode sgaAppModeStack[5];
static_assert(sizeof(sgaAppModeStack) == 20, "sgaAppModeStack: the item's 20 bytes in the .i64");

//  D2Client 0x6FB9A948
//  The launch-configuration block D2Launch passes to WINMAIN_D2ClientEntrypoint: written once, there, and the backing store for the whole
//  WINMAIN_GetConfig* family. D2ConfigStrc is a packed struct (967 bytes).
inline D2ConfigStrc *gpLaunchConfig;
static_assert(sizeof(gpLaunchConfig) == 4, "gpLaunchConfig: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9A94C
//  The client DLL's own HINSTANCE, stored by WINMAIN_D2ClientEntrypoint at startup and read back later in the same function. It is module-level
//  state kept for the entry point's own use, not the process-wide instance handle.
inline HINSTANCE ghClientInstance;
static_assert(sizeof(ghClientInstance) == 4, "ghClientInstance: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9A950
//  The value WINMAIN_D2ClientEntrypoint last returned, saved on the way out, each write immediately before a `retn 8`. Written and never read.
//  The values are 0, 3 and 4: 3 is the arm for dwGameType 2, 3, 6 and 7, and 4 the arm for dwGameType 0, 1, 4, 5, 8 and 9 and every type above 9.
//  Distinct from gbAppModeContinue, which decides what the entrypoint returns; this is the answer after the decision.
inline int32_t gnEntrypointReturnCode;
static_assert(sizeof(gnEntrypointReturnCode) == 4, "gnEntrypointReturnCode: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9A954
//  The second accelerator table, `LoadAcceleratorsA(hInstance, MAKEINTRESOURCE(0x65))` in WINMAIN_D2ClientEntrypoint. WNDPROC_WindowRunMessagePump
//  tries ghAccelTable first and reaches this one only when the first is NULL or fails to translate the message.
inline HACCEL hAccTableAlt;
static_assert(sizeof(hAccTableAlt) == 4, "hAccTableAlt: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9A958
//  One-shot guard so the command-line help box appears at most once per process: `if (!gbCommandLineHelpShown) { gbCommandLineHelpShown = 1; ...
//  show the box... }` in WINMAIN_CmdArgDisplayHelp, WINMAIN_CmdArgMode, WINMAIN_SCmdDisplayHelp and WINMAIN_SCmdDisplayHelpOnExtraArg. Nothing
//  clears it.
inline BOOL gbCommandLineHelpShown;
static_assert(sizeof(gbCommandLineHelpShown) == 4, "gbCommandLineHelpShown: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9A95C
//  The cursor into sgaAppModeStack: how many app modes are queued and not yet taken. Signed (the bound check is a signed compare). Only
//  WINMAIN_AppModeEnqueue and WINMAIN_AppModeDequeue touch it. Its value never exceeds 1: each mode that enqueues returns the mode that dequeues,
//  so the dispatch loop drives it 0, 1, 0, 1, 0.
inline int32_t sgnLastAppmode;
static_assert(sizeof(sgnLastAppmode) == 4, "sgnLastAppmode: the item's 4 bytes in the .i64");

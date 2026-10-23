#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "Lng.h"
#include "WINMAIN.h"

//  WINMAIN.cpp -- the entry point of D2Client.dll 1.10 and its launch plumbing: WINMAIN_D2ClientEntrypoint with its app-mode dispatch loop and the
//  app-mode queue, the command-line switch handlers, the two SCmdProcess callbacks, and the accessors of the launch configuration (D2ConfigStrc).
//  The original source file is CORE\WINMAIN.CPP (WINMAIN_D2ClientEntrypoint's two asserts name it). Address range 0x6FAA1D00..0x6FAA23A2.

//  The assert file string, as the binary pushes it (0x6FB746F8).
static const char szAssertFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\CORE\\WINMAIN.CPP";

//  gafnCommandsToRegister has ONE slot. The original's loop steps its index by 4 bytes and stops at 4, so it runs exactly once; written as an
//  element count.
static const int32_t nCommandsToRegister = 1;

//  gaCommandLineOptions is SCmdArg[8], 128 bytes; the 8 is the binary's literal `push 8`, and it agrees with the declared extent.
static const int32_t nCommandLineOptions = 8;

//  gLocalPlayerName is char[16]; the binary loads 15.
static const uint32_t nLocalPlayerNameMax = 16 - 1;

//  The app-mode queue (sgaAppModeStack and its cursor sgnLastAppmode) holds APPMODE_STACK_SIZE modes, from the game's own assert text
//  `sgnLastAppmode < APPMODE_STACK_SIZE`, guarded by `cmp eax, 5 / jl`. There is an off-by-one in it; see WINMAIN_AppModeEnqueue.
//  Deliberately not a member of D2C_AppMode: APPMODE_JOIN_GAME is already 5, and an enumerator sharing that value would let a real mode print as
//  a stack depth.
static const int32_t APPMODE_STACK_SIZE = 5;

//  sgaAppModeStack (20 bytes) and sgnLastAppmode (the cursor; signed, the bound check is a signed compare) are declared in WINMAIN.h. The extent
//  is the binary's, and it is compiled rather than trusted.
static_assert(sizeof(sgaAppModeStack) / sizeof(sgaAppModeStack[0]) == APPMODE_STACK_SIZE,
              "sgaAppModeStack has APPMODE_STACK_SIZE slots at 0x6FB9A934; gpLaunchConfig owns 0x6FB9A948");

//1.10f: D2Client.0x6FAA1D00
//  Pushes one app mode onto the pending queue. Its callers are GAME_AppModeStartup (APPMODE_GAME) and GAME_AppModeGame (APPMODE_CLOSEWINDOW).
//  BUG: this function admits a count of 4 and writes slot [4], while WINMAIN_AppModeDequeue shifts only three ([0]=[1], [1]=[2], [2]=[3]), so slot
//  [4] is never carried down: queue five and the fifth is stranded while the fourth is served twice. UNREACHABLE: the queue never gets deeper
//  than one -- each mode that pushes returns the very mode that pops (GAME_AppModeStartup returns APPMODE_JOIN_GAME on every path, GAME_AppModeGame
//  has one return and its single enqueue sits on the teardown run every returning path takes), and WINMAIN_D2ClientEntrypoint's loop runs
//  STARTUP -> JOIN_GAME -> GAME -> LEAVE_GAME -> CLOSEWINDOW with the depth going 0, 1, 0, 1, 0. Slots [1] through [4] are never live.
void __fastcall WINMAIN_AppModeEnqueue(D2C_AppMode nAppMode)
{
    if (sgnLastAppmode >= APPMODE_STACK_SIZE)
    {
        FOG_DisplayAssert("sgnLastAppmode < APPMODE_STACK_SIZE", szAssertFile, 166);
        exit(-1);
    }

    sgaAppModeStack[sgnLastAppmode] = nAppMode;
    sgnLastAppmode++;
}

//1.10f: D2Client.0x6FAA1D40
//  Takes the front of the queue, shifts the rest down and returns what was taken. It is reached by tail jump from MSG_AppModeJoinGame and
//  MSG_AppModeLeaveGame, so the popped mode becomes their return and goes straight back to the dispatch loop.
//  BUG: the shift stops one slot short of the array: three moves over a five-slot array. UNREACHABLE: see WINMAIN_AppModeEnqueue.
//  The bytes schedule the moves as `ecx=[1]; eax=[0]; edx=[2]; [0]=ecx; ecx=[3]; [1]=edx; [2]=ecx`, which MSVC emits both for three written-out
//  assignments and for a three-iteration loop it unrolled. Written out, since a loop here would have to name a bound, and the only bound this
//  module names is the 5 that would move four.
D2C_AppMode __cdecl WINMAIN_AppModeDequeue()
{
    if (!sgnLastAppmode)
    {
        FOG_DisplayAssert("sgnLastAppmode", szAssertFile, 176);
        exit(-1);
    }

    const D2C_AppMode nAppMode = sgaAppModeStack[0];

    sgaAppModeStack[0] = sgaAppModeStack[1];
    sgaAppModeStack[1] = sgaAppModeStack[2];
    sgaAppModeStack[2] = sgaAppModeStack[3];

    sgnLastAppmode--;
    return nAppMode;
}

//  The command-line switches: six handlers Storm calls back through gaCommandLineOptions, the eight-row table WINMAIN_D2ClientEntrypoint registers
//  (`SCmdRegisterArgList(..., 8)`), and the table itself, below them.
//  Every handler has the table's signature, `int __stdcall(void *pParam, int nUnused)`, because a function stored in a table is called with the
//  table's arguments, whatever its body reads. All six end `retn 8`. WINMAIN_CmdArgSet and the -mode handler read one field of the first
//  argument, at +0x18, and hand it on as a string -- the switch's value; the other four read neither argument, and none reads the second.
//  The help box is one block, written out four times in this file: in WINMAIN_CmdArgDisplayHelp, in the -mode handler's no-match arm, and in the
//  two SCmdProcess callbacks after WINMAIN_D2ClientEntrypoint. Each runs the same instructions: the latch gbCommandLineHelpShown tested and
//  raised, GetUserNameA into a 16-byte buffer with a length of 15, SStrCmp against "rseis", and a MessageBoxA of the help text captioned
//  "Diablo II" when the name differs.
//  BUG: GetUserNameA's answer is not tested at any of the four. A name longer than 14 characters fails the call and leaves the buffer as the stack
//  held it, and SStrCmp then compares that with "rseis". INERT: the buffer's one reader is that compare, and on bytes that are not the name it
//  answers "different" -- the answer the long name itself would get -- unless those six stack bytes happen to spell "rseis" and its NUL.

//  The one field of Storm's argument record that anything here reads: the switch's value string, at +0x18 in both readers.
static const size_t nArgValueOffset = 0x18;

//  The login the help box spares, as one object.
static const char szHelpExemptUser[] = "rseis";

//  WINMAIN_CmdArgPreCache's region (96 MB) and stride (4 KB), the bytes' immediates.
static const UINT_PTR nPreCacheBytes = 0x6000000;
static const UINT_PTR nPreCachePageSize = 0x1000;

static_assert(nPreCacheBytes == 0x6000 * nPreCachePageSize,
              "-c touches 0x6000 pages of 4 KB -- D2MOO's 0x6000 * nPageSize");

//1.10f: D2Client.0x6FAA1DA0
//  -?, -h and -help: the help box, once, and 0 on every path. The original inlines DisplayHelp here.
int32_t __stdcall WINMAIN_CmdArgDisplayHelp(void *pParam, int32_t nUnused)
{
    DWORD nBufferSize;
    CHAR szUserName[16];

    if (!gbCommandLineHelpShown)
    {
        gbCommandLineHelpShown = 1;
        nBufferSize = 15;
        GetUserNameA(szUserName, &nBufferSize);
        if (STORM_SStrCmp(szUserName, szHelpExemptUser, 0x7FFFFFFF))
            MessageBoxA(NULL, gpszCmdLineHelpText, gpszMsgBoxCaption, MB_OK);
    }

    return 0;
}

//1.10f: D2Client.0x6FAA1E10
//  -c: allocates 96 MB, writes a zero into the first byte of every 4 KB page of it, frees it, and answers 1. Presumably it commits the pages up
//  front, so that the game's later allocations do not fault them in one by one.
//  The first address and the end are computed once (`lea ecx, [eax+6000000h]`) and both compares are unsigned (jnb, jb); written with integer
//  addresses so that the arithmetic stays defined.
//  BUG: nothing tests FOG_Alloc's answer. UNREACHABLE: FOG_Alloc returns NULL only from its allocator's pool arm, and a request this size never
//  takes it -- its size class (bsr(size - 1) + 1 - 4) is 23, the global pool has 15 classes, and a class at or past the count goes to malloc,
//  whose failure halts (FOG_DisplayHalt and _exit(-1)).
int32_t __stdcall WINMAIN_CmdArgPreCache(void *pParam, int32_t nUnused)
{
    UINT_PTR nCache;
    UINT_PTR nEnd;
    UINT_PTR nPage;

    nCache = (UINT_PTR)FOG_Alloc((uint32_t)nPreCacheBytes, szAssertFile, 285, 0);
    nEnd = nCache + nPreCacheBytes;
    for (nPage = nCache; nPage < nEnd; nPage += nPreCachePageSize) *(char *)nPage = 0;

    FOG_Free((void *)nCache, szAssertFile, 288, 0);
    return 1;
}

//1.10f: D2Client.0x6FAA1E60
//  -r: starts recording the demo, and answers 1. Three instructions.
int32_t __stdcall WINMAIN_CmdArgRecord(void *pParam, int32_t nUnused)
{
    RECORD_RequestRecording();
    return 1;
}

//1.10f: D2Client.0x6FAA1E70
//  -p: plays the recorded demo back, and answers 1.
int32_t __stdcall WINMAIN_CmdArgPlay(void *pParam, int32_t nUnused)
{
    RECORD_Play();
    return 1;
}

//1.10f: D2Client.0x6FAA1E80
//  -set: saves the switch's value as the default command line and answers what SRegSaveString answers (the call is the last thing before
//  `retn 8`). The value comes back at the next start: WINMAIN_D2ClientEntrypoint loads it with SRegLoadString under the same two names.
int32_t __stdcall WINMAIN_CmdArgSet(void *pParam, int32_t nUnused)
{
    return STORM_SRegSaveString(szKeyName, "Default Command Line", 0, *(const char **)((const char *)pParam + nArgValueOffset));
}

//1.10f: D2Client.0x6FAA1EA0
//  -mode: finds the switch's value among the app-mode names, case-insensitively, and stores the index as gnAppMode; with no match, the help box.
//  The original inlines DisplayHelp here.
//  The names are gpLaunchParamCategories, eleven slots indexed by D2C_AppMode: [2] "Game", [7] "Tyler", [8] "Path", [9] "Test", [10] "Composite",
//  the other six NULL. The loop skips a NULL (`test eax, eax / jz`) and its bound is the literal 0Bh, unsigned (`jb`). The value is re-read from
//  the record on every pass. _strcmpi is the CRT's: the original's is linked in, ours is today's, and for these ASCII names the two agree.
//  The store does nothing that lasts: WINMAIN_D2ClientEntrypoint writes APPMODE_STARTUP over gnAppMode before its first dispatch, so a matched
//  -mode is overwritten -- the modes it names are a feature compiled out.
//  The bytes keep the GetUserNameA length in the parameter's own slot (the compiler's slot sharing); the C keeps a local.
int32_t __stdcall WINMAIN_CmdArgMode(void *pParam, int32_t nUnused)
{
    uint32_t nMode;
    const char *szName;
    DWORD nBufferSize;
    CHAR szUserName[16];

    for (nMode = 0; nMode < 11; ++nMode)
    {
        szName = gpLaunchParamCategories[nMode];
        if (szName && !_strcmpi(szName, *(const char **)((const char *)pParam + nArgValueOffset)))
        {
            gnAppMode = (D2C_AppMode)nMode;
            return 1;
        }
    }

    if (!gbCommandLineHelpShown)
    {
        gbCommandLineHelpShown = 1;
        nBufferSize = 15;
        GetUserNameA(szUserName, &nBufferSize);
        if (STORM_SStrCmp(szUserName, szHelpExemptUser, 0x7FFFFFFF))
            MessageBoxA(NULL, gpszCmdLineHelpText, gpszMsgBoxCaption, MB_OK);
    }

    return 0;
}

static_assert(sizeof(gafnCommandsToRegister) / sizeof(gafnCommandsToRegister[0]) == nCommandsToRegister,
              "gafnCommandsToRegister is 4 bytes at 0x6FB6E0C8, one row; the loop's bound is `cmp esi, 4` at 0x6FAA20B4");


static_assert(sizeof(gaCommandLineOptions) / sizeof(gaCommandLineOptions[0])
                  == nCommandLineOptions
              && sizeof(gaCommandLineOptions) == 128,
              "gaCommandLineOptions is SCmdArg[8], 128 bytes at 0x6FB6E0D0; "
              "gpszCmdLineHelpText follows at 0x6FB6E150");
static_assert(offsetof(SCmdArg, argString) == 0x08
                  && offsetof(SCmdArg, callback) == 0x0C,
              "a row is {dword0, dword4, name, handler}, 16 bytes");

//1.10f: D2Client.0x6FAA1F50
//  Ordinal export #10003, by ordinal only (D2Client.def): sends the game's own window WM_ACTIVATEAPP with wParam FALSE ("being deactivated") when
//  D2Gfx has a window, and then D2Gfx's WINDOW_ShowAll, by tail jump. Nothing in D2Client calls it; its callers are in other modules.
//  The message is the one Windows sends an application losing the foreground, and this module's handlers treat it so: WNDPROC_OnActivateApp and
//  GAME_WindowOnActivateApp clear gbAppActive, so the pump yields its timeslice, and the second lets the Run key go unless it is still held and
//  releases each mouse button still down. D2Gfx's WINDOW_ShowAll (#10040) then shows every window it keeps a placement for with its saved show
//  command, in full screen only -- the call D2Gfx's own pause makes after minimizing its window.
//  extern "C", like QueryInterface, so that the .def can export it by this name at 10003.
extern "C" void __cdecl WINMAIN_DeactivateApp()
{
    HWND hWnd;

    hWnd = D2GFX_WINDOW_GetWindow();
    if (hWnd)
        SendMessageA(hWnd, WM_ACTIVATEAPP, FALSE, 0);
    D2GFX_WINDOW_ShowAll();
}

//  The launch configuration's accessors: six readers and one writer of the D2ConfigStrc that D2Launch hands WINMAIN_D2ClientEntrypoint, which
//  stores it in gpLaunchConfig.

//1.10f: D2Client.0x6FAA1F70
//  The configuration itself. Called by GAME_ShowDiabloVanquishedMessage.
D2ConfigStrc *__cdecl WINMAIN_GetConfig()
{
    return gpLaunchConfig;
}

//1.10f: D2Client.0x6FAA1F80
//  The BnClient interface D2Launch may have filled in, at +0x21F, with no test of the configuration pointer. Each caller tests the answer for
//  zero right after the call.
struct BnClientInterface *__cdecl WINMAIN_GetBnClientInterface()
{
    return gpLaunchConfig->pComInterface;
}

//  The next three are one shape: NULL when there is no configuration or the string is empty, else the string. `repne scasb` computes the length
//  and `cmp ecx, 1 / jb` tests it ("at least one character", unsigned), and the configuration pointer is loaded once.

//1.10f: D2Client.0x6FAA1F90
//  The game's name, +0x1A. Called by AUTOMAP_DrawGameInfo.
char *__cdecl WINMAIN_GetConfigGameName()
{
    D2ConfigStrc *pConfig = gpLaunchConfig;

    if (pConfig && strlen(pConfig->szGameName))
        return pConfig->szGameName;
    return NULL;
}

//1.10f: D2Client.0x6FAA1FC0
//  The game's password, +0x23F. Called by AUTOMAP_DrawGameInfo.
char *__cdecl WINMAIN_GetConfigGamePassword()
{
    D2ConfigStrc *pConfig = gpLaunchConfig;

    if (pConfig && strlen(pConfig->szGamePassword))
        return pConfig->szGamePassword;
    return NULL;
}

//1.10f: D2Client.0x6FAA1FF0
//  The server address, +0x32. Called by AUTOMAP_DrawGameInfo, which uses it only to decide whether there is anything to draw.
char *__cdecl WINMAIN_GetConfigServerIP()
{
    D2ConfigStrc *pConfig = gpLaunchConfig;

    if (pConfig && strlen(pConfig->szServerIP))
        return pConfig->szServerIP;
    return NULL;
}

//1.10f: D2Client.0x6FAA2020
//  The hardcore bit of the character's save flags. The bytes read ONE byte of the word, `mov al, [eax+1EAh]`, into a register still holding the
//  pointer's upper bytes, and `and eax, 4` clears those: the answer is 4 or 0, and the C reads the same byte. Each caller tests it for zero.
int32_t __cdecl WINMAIN_IsHardcoreGame()
{
    return LOBYTE(gpLaunchConfig->unpackedCTemp.nCharacterSaveFlags)
           & CLIENTSAVEFLAG_HARDCORE;
}

//1.10f: D2Client.0x6FAA2030
//  Marks the character dead: `or byte ptr [eax+1EAh], 8`, a byte read-modify-write, written as one. void: the configuration pointer is left in
//  eax and nothing reads it; the one caller, UI_SetUIToggle's hardcore death path, calls WINMAIN_SetAppModeContinue next.
void __cdecl WINMAIN_MarkCharacterDead()
{
    D2ConfigStrc *pConfig = gpLaunchConfig;

    *(uint8_t *)&pConfig->unpackedCTemp.nCharacterSaveFlags |= CLIENTSAVEFLAG_DEAD;
}

//1.10f: D2Client.0x6FAA2040
//  Sets gbAppModeContinue, the flag that decides what WINMAIN_D2ClientEntrypoint returns once the dispatch loop has ended: 0 when it is set, and
//  otherwise 3 or 4 by game type. Seven bytes: `mov gbAppModeContinue, ecx / retn`.
void __fastcall WINMAIN_SetAppModeContinue(BOOL bContinue)
{
    gbAppModeContinue = bContinue;
}


//1.10f: D2Client.0x6FAA2050
int32_t __stdcall WINMAIN_D2ClientEntrypoint(HMODULE hModule, D2ConfigStrc* pConfig)
{
    char szDefaultCommandLine[260];
    DWORD nPlayerNameLength;
    char szLocale[4];

    gpLaunchConfig = pConfig;

    FOG_10144(0);
    FOG_SetMemoryPoolSystemMode(0, TRUE);

    ghClientInstance = hModule;
    GetModuleFileNameA(hModule, Filename, 260);

    STORM_SCmdRegisterArgList(gaCommandLineOptions, nCommandLineOptions);
    for (int32_t i = 0; i < nCommandsToRegister; i++)
    {
        if (!gafnCommandsToRegister[i]())
            return 0;
    }

    //  One argument in ecx and nothing pushed: __fastcall.
    if (!GAME_ApplyLaunchParams(pConfig))
        return 0;

    if (STORM_SRegLoadString(szKeyName, "Default Command Line", 0, szDefaultCommandLine, sizeof(szDefaultCommandLine)))
    {
        //  Two different callbacks: the binary pushes WINMAIN_SCmdDisplayHelp FIRST, as the fourth argument, Storm's error callback, and
        //  WINMAIN_SCmdDisplayHelpOnExtraArg after it, as the third, its callback for an extra argument.
        STORM_SCmdProcess(szDefaultCommandLine, 0, WINMAIN_SCmdDisplayHelpOnExtraArg, WINMAIN_SCmdDisplayHelp);
    }

    nPlayerNameLength = nLocalPlayerNameMax;
    if (!gLocalPlayerName[0] && !GetUserNameA(gLocalPlayerName, &nPlayerNameLength))
    {
        wsprintfA(gLocalPlayerName, "%s", "noname");
    }

    if (!LNG_Initialize())
        return 0;

    D2WIN_10015();
    //  D2Win stores this pointer and calls it.
    D2WIN_10014_SetWndProc(WNDPROC_WndProc);
    hAccTableAlt = LoadAcceleratorsA(ghClientInstance, MAKEINTRESOURCEA(101));

    const int32_t bIsExpansion = FOG_IsExpansion();
    if (!D2LANG_10000((uint32_t)(UINT_PTR)D2WIN_GetArchive(), 0, bIsExpansion))
    {
        return 0;
    }

    D2LANG_10006((uint32_t)(UINT_PTR)szLocale, 0);
    FOG_Trace("%s is locale\n", szLocale);

    //  szLocale and nPlayerNameLength share one stack slot in the binary ([esp+3Ch] holds both), which is the compiler folding two dead locals
    //  together, not one variable with two roles. They are separate here because they were separate in the source.
    D2COMMON_DATATBLS_LoadAllTxts(D2WIN_GetArchive(), 0, pConfig->dwGameType == GAMETYPE_OBNET_HOST);

    gbAppModeContinue = 1;
    gnAppMode = APPMODE_STARTUP;

    D2C_AppMode nAppMode = APPMODE_STARTUP;
    do
    {
        //  The table is typed here, so the call needs no cast; it is the same __fastcall indirect call through a seven-entry table.
        nAppMode = gafnAppModeEntryPoints[nAppMode]();
        gnAppMode = nAppMode;

        //  APPMODE_LAST is 12 and the guard admits 1..11, while gafnAppModeEntryPoints has only seven entries. That is not a latent overrun: no
        //  handler returns 7..11 and nothing else feeds the loop. It is a feature compiled out with its constants left behind --
        //  gpLaunchParamCategories still names Tyler, Path, Test and Composite, and `mov eax, 3` overwrites whatever -mode selected.
        if (nAppMode >= APPMODE_LAST)
        {
            FOG_DisplayAssert("gnAppMode < APPMODE_LAST", szAssertFile, 662);
            exit(-1);
        }
        if (nAppMode <= APPMODE_CONTINUE)
        {
            FOG_DisplayAssert("gnAppMode > APPMODE_CONTINUE", szAssertFile, 663);
            exit(-1);
        }
    } while (nAppMode != APPMODE_SHOULDQUIT);

    D2WIN_10016();
    D2COMMON_DATATBLS_UnloadAllBins();
    D2LANG_10001();
    LNG_Shutdown();
    D2COMMON_LOG_11105();
    MSG_FreeGame();

    if (gbAppModeContinue)
    {
        gnEntrypointReturnCode = 0;
        return 0;
    }

    switch (pConfig->dwGameType)
    {
    case GAMETYPE_BNET_BETA:
    case GAMETYPE_BNET:
    case GAMETYPE_OBNET_HOST:
    case GAMETYPE_OBNET_JOIN:
        gnEntrypointReturnCode = 3;
        return 3;
    default:
        gnEntrypointReturnCode = 4;
        return 4;
    }
}

//  The two SCmdProcess callbacks. WINMAIN_D2ClientEntrypoint hands both to SCmdProcess with the default command line from the registry. Storm's
//  contract is `BOOL (__stdcall *)(int)`, `retn 4` in both, and the argument is read by neither. The image has two bodies at two addresses, the
//  same help-box block in each, and they differ in what they return.
//  Their slots are Storm's two callbacks. The third argument is the callback for an extra argument -- a token that is neither a switch nor a
//  switch's value: Storm's per-token handler pushes the token, calls it and returns its answer, and a 0 answer ends the parse; so its int is the
//  token's address. The fourth is the error callback: Storm calls it with a record {code, argument, message}, and SCmdProcess hands it its
//  fourth argument.

//1.10f: D2Client.0x6FAA22D0
//  SCmdProcess's third argument, the extra-argument callback: the help box, once, and 0 (`xor eax, eax`, the join of every path), which stops the
//  parse at the first stray word. The same instructions as WINMAIN_CmdArgDisplayHelp but for the one-argument `retn 4`. The original inlines
//  DisplayHelp here.
int32_t __stdcall WINMAIN_SCmdDisplayHelpOnExtraArg(int32_t nCmdArg)
{
    DWORD nBufferSize;
    CHAR szUserName[16];

    if (!gbCommandLineHelpShown)
    {
        gbCommandLineHelpShown = 1;
        nBufferSize = 15;
        GetUserNameA(szUserName, &nBufferSize);
        if (STORM_SStrCmp(szUserName, szHelpExemptUser, 0x7FFFFFFF))
            MessageBoxA(NULL, gpszCmdLineHelpText, gpszMsgBoxCaption, MB_OK);
    }

    return 0;
}

//1.10f: D2Client.0x6FAA2340
//  SCmdProcess's fourth argument, the error callback: the same block, and NO `xor eax, eax` before `retn 4`. So it answers whatever eax holds at
//  the exit: the latch's value when the box has been shown before; SStrCmp's 0 when the login is "rseis"; MessageBoxA's answer otherwise. The shape
//  is consistent with a source that falls off the end of a BOOL function; the C returns each value explicitly, which is the same program and a
//  defined one. The original inlines DisplayHelp here.
int32_t __stdcall WINMAIN_SCmdDisplayHelp(int32_t nCmdArg)
{
    int32_t nResult;
    DWORD nBufferSize;
    CHAR szUserName[16];

    nResult = gbCommandLineHelpShown;
    if (!nResult)
    {
        gbCommandLineHelpShown = 1;
        nBufferSize = 15;
        GetUserNameA(szUserName, &nBufferSize);
        nResult = STORM_SStrCmp(szUserName, szHelpExemptUser, 0x7FFFFFFF);
        if (nResult)
            nResult = MessageBoxA(NULL, gpszCmdLineHelpText, gpszMsgBoxCaption, MB_OK);
    }

    return nResult;
}

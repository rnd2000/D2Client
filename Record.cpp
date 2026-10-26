#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "Record.h"

//  Record.cpp -- the demo recorder and player of D2Client.dll 1.10, driven by the command-line switches `-r` (WINMAIN_CmdArgRecord) and `-p`
//  (WINMAIN_CmdArgPlay). Twelve functions, address range 0x6FAAE5B0..0x6FAAEC7D; the 288 bytes between RECORD_LogEvent and RECORD_SaveToFile
//  are LogEvent's own switch data and hold no code. The original source file is GAME\Record.cpp (its asserts name it).
//
//  Recording: `-r` raises gbRecordingRequested. GAME_AppModeGame, after GAME_InitGame, calls RECORD_InstallSaveOnCleanup; from then on
//  WNDPROC_WndProc hands every window message to RECORD_LogEvent, which keeps ten kinds of them, stamped with the frame, in a chain of
//  500-record holders. When the game loop ends GAME_AppModeGame calls RECORD_SaveToFile, which writes the chain to `Record.dr1` behind a
//  20-byte header.
//  Playback: `-p` is RECORD_Play. It raises gbPlaybackRequested, reads `Record.dr1` back into the same chain and hands the header's seed to
//  D2GAME_GAME_SetInitSeed. GAME_AppModeGame then calls RECORD_SetRecordPlaybackLoaded; from then on GAME_UpdateFrame calls
//  RECORD_ReadNextForFrame each frame, which dispatches every record whose frame has come, and WNDPROC_WndProc passes real input (WM_CLOSE
//  excepted) to DefWindowProcA instead of the game.
//
//  The four flags:
//      gbRecordingRequested   set by `-r` unless a `-p` is pending, cleared by `-p`; it asks for a RECORDING
//      gbPlaybackRequested    set by `-p`, cleared by `-r` and by a header that is not version 1; it asks for a PLAYBACK
//      gbRecordSaveOnCleanupInstalled  set by RECORD_InstallSaveOnCleanup; RECORD_LogEvent logs nothing while it is clear
//      gbRecordPlaybackLoaded set by RECORD_SetRecordPlaybackLoaded, cleared by a bad header and by the end frame; it gates
//                             RECORD_ReadNextForFrame and WNDPROC_WndProc's input filter
//  Each switch cancels the other's pending request rather than toggling its own.
//
//  The original's assert names: `sgptHolderCurr` is gpRecordBlock, `sgtReadFileHeader` is gRecordFileHeader (`.nHolders` is its +4) and
//  RECORDS_PER_HOLDER is 500. The allocation of a new holder is source line 53 of the original and is written in place at three sites: twice
//  in RECORD_LogEvent and once in RECORD_Play.

//  The original's source file, as every FOG_Alloc, FOG_Free and FOG_DisplayAssert of the unit names it.
static const char szSourceFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\GAME\\Record.cpp";

//  The original's name for the bound its asserts spell out.
static const int32_t RECORDS_PER_HOLDER = 500;

//  Layout checks: the members and extents the unit's instructions address.
static_assert(RECORDS_PER_HOLDER * sizeof(D2ClientRecordStrc) == 0x1F40,
              "500 records end where nRecordWriteCurr begins");
static_assert(offsetof(D2ClientRecordHolderStrc, nRecordWriteCurr) == 0x1F40,
              "+1F40h at 0x6FAAE5FA, 0x6FAAE63B and 0x6FAAE86D");
static_assert(offsetof(D2ClientRecordHolderStrc, nRecordReadCurr) == 0x1F44,
              "[eax+1F44h] at 0x6FAAEB7B and 0x6FAAEBFB");
static_assert(offsetof(D2ClientRecordHolderStrc, ptNext) == 0x1F48,
              "[ecx+1F48h] at 0x6FAAE85D, [esi+1F48h] at 0x6FAAE8E9");
static_assert(offsetof(D2ClientRecordStrc, nMsg) == 4
                  && offsetof(D2ClientRecordStrc, nWParam) == 8
                  && offsetof(D2ClientRecordStrc, nLParam) == 0xC,
              "[esi+4] edi, [esi+8] ebx, [esi+0Ch] lParam, 0x6FAAE65A..0x6FAAE660");
static_assert(offsetof(D2ClientRecordFileHeaderStrc, nHolders) == 4
                  && offsetof(D2ClientRecordFileHeaderStrc, nRecords) == 8
                  && offsetof(D2ClientRecordFileHeaderStrc, nEndFrame) == 0xC
                  && offsetof(D2ClientRecordFileHeaderStrc, nInitSeed) == 0x10,
              "0x6FBA9A24, 0x6FBA9A28, 0x6FBA9A2C and 0x6FBA9A30 as read");
static_assert(0x109A20 + sizeof(D2ClientRecordFileHeaderStrc) == 0x109A34,
              "the file header ends where gpfnPreviousCleanupHandler begins");
static_assert(0x109A38 + sizeof(D2ClientRecordHolderStrc) == 0x10B984,
              "gRecordHolderFirst ends where gbRecordingRequested begins");

//  The ten messages RECORD_LogEvent keeps, by their Windows names.
static_assert(WM_CLOSE == 0x10 && WM_KEYDOWN == 0x100 && WM_KEYUP == 0x101
                  && WM_CHAR == 0x102 && WM_COMMAND == 0x111
                  && WM_MOUSEMOVE == 0x200 && WM_LBUTTONDOWN == 0x201
                  && WM_LBUTTONUP == 0x202 && WM_RBUTTONDOWN == 0x204
                  && WM_RBUTTONUP == 0x205,
              "the ten ids the jump tables send to the two append arms");


//1.10f: D2Client.0x6FAAE5B0
//  Appends one window message to the recording. The one caller is WNDPROC_WndProc, for every message the window gets, before anything else
//  sees it. Nothing happens unless gbRecordSaveOnCleanupInstalled is set.
//  Ten ids are kept: 0x10, 0x100..0x102, 0x111, 0x200..0x202, 0x204 and 0x205. Every other id is dropped, WM_LBUTTONDBLCLK (0x203) among
//  them. The id is stored as it came; the `- 10h` and `- 200h` in the bytes are the switch indexing its tables.
//  The WM_KEYDOWN arm repeats the other arm's append: the same test, the same new holder, the same record stores, differing only in
//  scheduling (the new holder's two counters are zeroed in opposite orders). The C keeps both arms and each store order. The original
//  inlines the new-holder allocation (FOG_Alloc, both counters zeroed, linked after gpRecordBlock) in each arm.
//  A full holder is `nRecordWriteCurr >= 500`, a signed compare.
//  BUG: FOG_Alloc's answer is used untested. LATENT: only a failed allocation reaches it, and FOG_Alloc then reports through its error
//  manager and hands the NULL back unless the executable's error callback ends the process.
void __fastcall RECORD_LogEvent(uint32_t nEventId, WPARAM wParam, LPARAM lParam)
{
    D2ClientRecordHolderStrc *pHolder;
    D2ClientRecordStrc *pRecord;

    if (!gbRecordSaveOnCleanupInstalled)
        return;

    switch (nEventId)
    {
    case WM_KEYDOWN:
        pHolder = gpRecordBlock;
        if (pHolder->nRecordWriteCurr >= RECORDS_PER_HOLDER)
        {
            pHolder = (D2ClientRecordHolderStrc *)FOG_Alloc(sizeof(D2ClientRecordHolderStrc), szSourceFile, 53, 0);
            pHolder->nRecordWriteCurr = 0;
            pHolder->nRecordReadCurr = 0;
            gpRecordBlock->ptNext = pHolder;
            pHolder->ptNext = NULL;
            gpRecordBlock = pHolder;
        }

        pRecord = &pHolder->tRecords[pHolder->nRecordWriteCurr++];
        pRecord->nFrame = GAME_GetFrameCount();
        pRecord->nLParam = (int32_t)lParam;
        pRecord->nMsg = nEventId;
        pRecord->nWParam = wParam;
        return;

    case WM_CLOSE:
    case WM_KEYUP:
    case WM_CHAR:
    case WM_COMMAND:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        pHolder = gpRecordBlock;
        if (pHolder->nRecordWriteCurr >= RECORDS_PER_HOLDER)
        {
            pHolder = (D2ClientRecordHolderStrc *)FOG_Alloc(sizeof(D2ClientRecordHolderStrc), szSourceFile, 53, 0);
            pHolder->nRecordReadCurr = 0;
            pHolder->nRecordWriteCurr = 0;
            gpRecordBlock->ptNext = pHolder;
            pHolder->ptNext = NULL;
            gpRecordBlock = pHolder;
        }

        pRecord = &pHolder->tRecords[pHolder->nRecordWriteCurr++];
        pRecord->nFrame = GAME_GetFrameCount();
        pRecord->nLParam = (int32_t)lParam;
        pRecord->nMsg = nEventId;
        pRecord->nWParam = wParam;
        return;

    default:
        return;
    }
}


//1.10f: D2Client.0x6FAAE810
//  Writes the recording to `Record.dr1`: a dword 20 (the size of what follows), the 20-byte header, then every holder's used records back
//  to back from the static first one, `16 * nRecordWriteCurr` bytes each. The header is nVersion 1, nHolders (counted by the walk),
//  nRecords (the last holder's count plus 500 per holder before it), nEndFrame GAME_GetFrameCount() and nInitSeed
//  D2COMMON_DUNGEON_GetInitSeedFromAct(gpAct); it is a stack copy of the D2ClientRecordFileHeaderStrc that RECORD_Play reads into
//  gRecordFileHeader. nRecords is 32-bit lea arithmetic in the original; `int` computes the same, because nHolders counts 8012-byte holders
//  and 500 * (2^32 / 8012) is under 2^31.
//  Callers: GAME_AppModeGame after the game loop when gbRecordingRequested is set, and RECORD_CleanupHandler, which nothing calls.
//  CreateFileA's answer is not tested; WriteFile and CloseHandle are handed whatever it returned.
//  void: the body ends in `call ds:CloseHandle` and the original returns its eax; neither call site reads it.
void __cdecl RECORD_SaveToFile()
{
    HANDLE hFile;
    D2ClientRecordHolderStrc *pHolder;
    D2ClientRecordFileHeaderStrc tHeader;
    int32_t nHeaderSize;
    DWORD nWritten;

    hFile = CreateFileA(FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    tHeader.nVersion = 1;
    tHeader.nHolders = 1;
    pHolder = &gRecordHolderFirst;
    while (pHolder->ptNext)
    {
        pHolder = pHolder->ptNext;
        ++tHeader.nHolders;
    }

    tHeader.nRecords = pHolder->nRecordWriteCurr + RECORDS_PER_HOLDER * tHeader.nHolders - RECORDS_PER_HOLDER;
    tHeader.nEndFrame = GAME_GetFrameCount();
    tHeader.nInitSeed = D2COMMON_DUNGEON_GetInitSeedFromAct(gpAct);

    nHeaderSize = sizeof(D2ClientRecordFileHeaderStrc);
    WriteFile(hFile, &nHeaderSize, 4, &nWritten, NULL);
    WriteFile(hFile, &tHeader, sizeof(D2ClientRecordFileHeaderStrc), &nWritten, NULL);

    pHolder = &gRecordHolderFirst;
    do
    {
        WriteFile(hFile, pHolder, (uint32_t)pHolder->nRecordWriteCurr << 4, &nWritten, NULL);
        pHolder = pHolder->ptNext;
    } while (pHolder);

    CloseHandle(hFile);
}


//1.10f: D2Client.0x6FAAE910
//  Frees every holder but the first, which is static. The one caller is GAME_AppModeCloseWindow, on the way out of the program. The walk
//  starts at `gRecordHolderFirst.ptNext` and the next pointer is read before each FOG_Free.
//  Neither `gRecordHolderFirst.ptNext` nor gpRecordBlock is reset, so either can be left naming a freed holder; the one caller is the window's
//  close, after the last game.
void __cdecl RECORD_FreeBufferList()
{
    D2ClientRecordHolderStrc *pHolder;
    D2ClientRecordHolderStrc *pFree;

    pHolder = gRecordHolderFirst.ptNext;
    while (pHolder)
    {
        pFree = pHolder;
        pHolder = pHolder->ptNext;
        FOG_Free(pFree, szSourceFile, 288, 0);
    }
}


//1.10f: D2Client.0x6FAAE940
//  `-p`: loads `Record.dr1` for playback. The one caller is WINMAIN_CmdArgPlay, so this runs while the command line is parsed, before any
//  game. If a `-r` is pending it is cancelled and nothing is loaded. Otherwise gbPlaybackRequested is raised, the dword before the header is
//  read and ignored, and the header lands on gRecordFileHeader. A header whose version is not 1 clears gbPlaybackRequested and
//  gbRecordPlaybackLoaded; either way the file is closed and D2Game's init seed is set from the header (outside the `if`). The original
//  inlines the new-holder allocation for each holder after the first. The compares are signed throughout (`jle` on nHolders, `jg` on
//  nRecords, `jl` on the loop).
//  BUG: CreateFileA's answer is not tested. With no file both reads fail and the header keeps what it held, zero-filled on a first load, so
//  version 0 and the seed handed to D2Game is 0, not D2Game's "no seed", which is -1. REACHABLE: `-p` with no demo file creates every
//  later single-player game from seed 0.
//  BUG: a negative nRecords (only in a file the game did not write) passes the signed clamp to 500, and ReadFile is asked for
//  `(DWORD)nRecords << 4` bytes, computed below as the original computes it. LATENT: the game never writes a negative count. Whether the read
//  then writes past the holder was not looked for; if it does, the original's runs on into the four flags that follow gRecordHolderFirst
//  and beyond, ours into whatever the linker placed after that object.
void __cdecl RECORD_Play()
{
    HANDLE hFile;
    D2ClientRecordHolderStrc *pHolder;
    int32_t iHolder;
    int32_t nEntries;
    int32_t nHeaderSize;
    DWORD nRead;

    if (gbRecordingRequested)
    {
        gbRecordingRequested = 0;
        return;
    }

    gbPlaybackRequested = 1;
    hFile = CreateFileA(FileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ReadFile(hFile, &nHeaderSize, 4, &nRead, NULL);
    ReadFile(hFile, &gRecordFileHeader, sizeof(D2ClientRecordFileHeaderStrc), &nRead, NULL);

    if (gRecordFileHeader.nVersion == 1)
    {
        for (iHolder = 0; iHolder < gRecordFileHeader.nHolders; ++iHolder)
        {
            if (iHolder)
            {
                pHolder = (D2ClientRecordHolderStrc *)FOG_Alloc(sizeof(D2ClientRecordHolderStrc), szSourceFile, 53, 0);
                pHolder->nRecordWriteCurr = 0;
                pHolder->nRecordReadCurr = 0;
                gpRecordBlock->ptNext = pHolder;
                pHolder->ptNext = NULL;
                gpRecordBlock = pHolder;
            }

            //  BUG: both stores of the count go to gRecordHolderFirst.nRecordWriteCurr, the static holder's, whichever holder this pass
            //  is filling, and nRecords, the whole file's count, is read afresh every pass and never reduced by what a holder took. For a
            //  file of more than 500 records every holder is asked for 500, the last one included (ReadFile's short read at the end is not
            //  looked at), the first holder's count is 500 and every later holder keeps the 0 the new-holder code gave it, so
            //  RECORD_ReadNextForFrame's end test never matches in the last holder and the reader walks on into slots the file never
            //  filled. REACHABLE: a `-r` session of more than 500 kept messages, played with `-p`.
            //  A file of up to 500 records is one holder and is read right. The assert below is reached only in the `<= 500` arm, so it
            //  stops a file that claims at most 500 records and more than one holder.
            nEntries = gRecordFileHeader.nRecords;
            if (nEntries > RECORDS_PER_HOLDER)
            {
                nEntries = RECORDS_PER_HOLDER;
                gRecordHolderFirst.nRecordWriteCurr = RECORDS_PER_HOLDER;
            }
            else
            {
                gRecordHolderFirst.nRecordWriteCurr = nEntries;
                if (iHolder + 1 != gRecordFileHeader.nHolders)
                {
                    FOG_DisplayAssert("i + 1 == sgtReadFileHeader.nHolders", szSourceFile, 256);
                    exit(-1);
                }
            }

            ReadFile(hFile, gpRecordBlock, (uint32_t)nEntries << 4, &nRead, NULL);
        }

        gpRecordBlock = &gRecordHolderFirst;
    }
    else
    {
        gbPlaybackRequested = 0;
        gbRecordPlaybackLoaded = 0;
    }

    CloseHandle(hFile);
    D2GAME_GAME_SetInitSeed(gRecordFileHeader.nInitSeed);
}


//1.10f: D2Client.0x6FAAEAA0
//  Returns gbRecordingRequested: whether `-r` asked for a recording. Called by GAME_AppModeGame.
int32_t __cdecl RECORD_GetRecordingRequested()
{
    return gbRecordingRequested;
}


//1.10f: D2Client.0x6FAAEAB0
//  Saves the recording, then chains to the handler that was installed before (gpfnPreviousCleanupHandler), if any.
//  RECORD_InstallSaveOnCleanup puts this in the one slot FAULT_SetAssertHandler keeps, and nothing calls that slot (gpfnFaultAssertHandler is
//  read and written only by the installer), so this never runs: the recording is saved by GAME_AppModeGame's direct call alone.
//  The previous handler is entered by `jmp` (0x6FAAEABE) with this call's three arguments still on the stack. It is GAME_TeardownGraphics,
//  installed by GAME_InitGame. What the three arguments mean is not known, so they are named by position.
//  void: eax is the previous handler's own answer (entered by a tail jump), or the null pointer's 0 with none, and no caller reads it.
void __stdcall RECORD_CleanupHandler(int32_t nArg1, int32_t nArg2, int32_t nArg3)
{
    RECORD_SaveToFile();

    if (gpfnPreviousCleanupHandler)
        gpfnPreviousCleanupHandler(nArg1, nArg2, nArg3);
}


//1.10f: D2Client.0x6FAAEAD0
//  Turns the recorder on: raises gbRecordSaveOnCleanupInstalled, which RECORD_LogEvent tests, and installs RECORD_CleanupHandler, keeping the
//  handler it displaces in gpfnPreviousCleanupHandler. The one caller is GAME_AppModeGame, when a recording was asked for and no playback was.
//  FAULT_SetAssertHandler answers `void *`, hence the cast.
//  void: eax is the displaced handler, stored and left; the one call site does not read it.
void __cdecl RECORD_InstallSaveOnCleanup()
{
    gbRecordSaveOnCleanupInstalled = 1;
    gpfnPreviousCleanupHandler = (int32_t (__stdcall *)(int32_t, int32_t, int32_t))FAULT_SetAssertHandler((void *)RECORD_CleanupHandler);
}


//1.10f: D2Client.0x6FAAEAF0
//  `gbRecordPlaybackLoaded = 1;` -- starts the playback RECORD_Play loaded. Called by GAME_AppModeGame when gbPlaybackRequested is set.
void __cdecl RECORD_SetRecordPlaybackLoaded()
{
    gbRecordPlaybackLoaded = 1;
}


//1.10f: D2Client.0x6FAAEB00
//  Returns gbPlaybackRequested: whether `-p` asked for a playback. Called by GAME_AppModeGame.
int32_t __cdecl RECORD_GetPlaybackRequested()
{
    return gbPlaybackRequested;
}


//1.10f: D2Client.0x6FAAEB10
//  Returns gbRecordPlaybackLoaded. WNDPROC_WndProc reads it: while a playback runs, only WM_CLOSE of the real messages reaches the game.
int32_t __cdecl RECORD_GetRecordPlaybackLoaded()
{
    return gbRecordPlaybackLoaded;
}


//1.10f: D2Client.0x6FAAEB20
//  `-r`: asks for a recording, or cancels a pending playback. If gbPlaybackRequested is set (a `-p` came first) it is cleared and nothing
//  else happens; otherwise gbRecordingRequested is raised. The one caller is WINMAIN_CmdArgRecord.
void __cdecl RECORD_RequestRecording()
{
    if (gbPlaybackRequested)
    {
        gbPlaybackRequested = 0;
        return;
    }

    gbRecordingRequested = 1;
}


//1.10f: D2Client.0x6FAAEB40
//  Replays every record whose frame has come. The one caller is GAME_UpdateFrame, every frame. Nothing happens unless
//  gbRecordPlaybackLoaded is set and gpRecordBlock is not NULL.
//  At the recording's end frame the playback is switched off. Otherwise, from the current holder's read cursor (asserted under 500), each
//  record with `nFrame <= now` is dispatched through SMsgDispatchMessage to the game window, as if the window procedure had it: the cursor
//  steps on, and at 500 the reader moves to the next holder. It stops at a record from a later frame, at the end of the data (no next
//  holder and the write count reached) or at a NULL holder. Every compare is signed (`jl` on the end frame and on the cursor, `jg` and
//  `jle` on the record's frame).
//  The original inlines the frame assert `ptRecord->nFrame == (int)GameGetFrame ()` (source line 298) in the dispatch step. It stops the game
//  on any record from an earlier frame than now: the loop takes `nFrame <= now`, the assert demands `==`, so a record left behind by a
//  frame is fatal, not skipped.
//  BUG: when the last holder's cursor reaches 500 before the end test has matched, gpRecordBlock becomes NULL; from then on the first test
//  returns before the end-frame test, so gbRecordPlaybackLoaded is never cleared and WNDPROC_WndProc keeps the real input from the game.
//  REACHABLE: with a chain RECORD_Play built right the end test matches first; RECORD_Play's multi-holder count is what makes it miss, and
//  the slots the reader then walks into were never filled from the file: one whose frame is at or below now either fails the frame assert
//  or is dispatched.
void __cdecl RECORD_ReadNextForFrame()
{
    D2ClientRecordHolderStrc *pHolder;
    D2ClientRecordStrc *pRecord;
    int32_t nFrame;
    int32_t nReadCurr;
    BOOL bReturn;
    LRESULT lResult;

    if (!gbRecordPlaybackLoaded || !gpRecordBlock)
        return;

    //  The frame count is taken signed here (`jl` against the header's end frame, `jg` against each record's), as the original's own assert
    //  text says: "ptRecord->nFrame == (int)GameGetFrame ()". GAME_GetFrameCount answers `unsigned int`, so the casts are written where the
    //  original wrote them.
    nFrame = (int32_t)GAME_GetFrameCount();
    if (nFrame >= gRecordFileHeader.nEndFrame)
    {
        gbRecordPlaybackLoaded = 0;
        return;
    }

    pHolder = gpRecordBlock;
    nReadCurr = pHolder->nRecordReadCurr;
    if (nReadCurr >= RECORDS_PER_HOLDER)
    {
        FOG_DisplayAssert("sgptHolderCurr->nRecordReadCurr < RECORDS_PER_HOLDER", szSourceFile, 393);
        exit(-1);
    }

    if (!pHolder->ptNext && pHolder->nRecordWriteCurr == nReadCurr)
        return;

    pRecord = &pHolder->tRecords[nReadCurr];
    while (pRecord->nFrame <= nFrame)
    {
        bReturn = FALSE;
        lResult = 0;

        if (pRecord->nFrame != (int32_t)GAME_GetFrameCount())
        {
            FOG_DisplayAssert("ptRecord->nFrame == (int)GameGetFrame ()", szSourceFile, 298);
            exit(-1);
        }
        STORM_SMsgDispatchMessage(D2GFX_WINDOW_GetWindow(), pRecord->nMsg, pRecord->nWParam, pRecord->nLParam, &bReturn, &lResult);

        ++gpRecordBlock->nRecordReadCurr;
        pHolder = gpRecordBlock;
        if (!pHolder->ptNext && pHolder->nRecordWriteCurr == pHolder->nRecordReadCurr)
        {
            return;
        }
        if (pHolder->nRecordReadCurr >= RECORDS_PER_HOLDER)
        {
            pHolder = pHolder->ptNext;
            gpRecordBlock = pHolder;
        }
        if (!pHolder)
            return;

        pRecord = &pHolder->tRecords[pHolder->nRecordReadCurr];
    }
}

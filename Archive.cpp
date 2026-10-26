#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"

//  Archive.cpp -- the file-loading helpers of D2Client.dll 1.10: five functions, 0x6FB6B91C..0x6FB6BB35, which open, close, size, read and load a
//  file through Fog. All five asserts push the path D2Hell\SRC\Archive.cpp, so the original's source file is D2Hell's, compiled into D2Client
//  rather than written for it. It is not D2Client's own CORE\ARCHIVE.CPP (0x6FB740A0), a different unit that holds ARCHIVE_CellFileLoad.
//  Everything after a successful open is fatal on failure, and the message names the file.

//  The one path literal pushed by all five asserts: in the original it is __FILE__, one object at 0x6FB9A5FC.
static const char szArchiveCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Hell\\SRC\\Archive.cpp";

//  260 is MAX_PATH: ARCHIVE_GetFileSize and ARCHIVE_ReadFile each hold one 260-byte buffer, pushed as SFileGetFileName's nMaxLen (104h).
static_assert(260 == 0x104,
              "Archive.cpp's szFileName buffers are 0x104 bytes -- the frames "
              "of ARCHIVE_GetFileSize and ARCHIVE_ReadFile are that and "
              "nothing else");


//1.10f: D2Client.0x6FB6B91C
//  Opens a file or says so: FOG_FOpenFile with a diagnostic. Returns 1 on success; on failure it traces "Error opening file: %s" and returns 0.
//  The last parameter suppresses the trace only for a missing file: the test is `!bQuietIfMissing || GetLastError() != 2` (2 is
//  ERROR_FILE_NOT_FOUND), so a caller that expects an absent file (PALSHIFT_EnsureMonsterPalette) stays quiet about that one case and still
//  reports a bad path or a denied read.
//  ecx (hArchive) is never read: the file name arrives in edx, so parameter one exists only to put the second there; every caller passes the
//  archive handle into it. The third parameter is an out-handle: the opened file's handle, which the callers pass on to ARCHIVE_ReadFile.
int32_t __fastcall ARCHIVE_OpenFileOrTrace(void *hArchive, const char *pszFileName, void **phFile, int32_t bQuietIfMissing)
{
    uint32_t dwLastError;

    UNREFERENCED_PARAMETER(hArchive);

    if (FOG_FOpenFile(pszFileName, phFile))
        return 1;

    dwLastError = GetLastError();
    if (!bQuietIfMissing || dwLastError != 2)
        FOG_Trace("Error opening file: %s", pszFileName);

    return 0;
}


//1.10f: D2Client.0x6FB6B958
//  Closes a file: FOG_FCloseFile behind an hFile assert at Archive.cpp:73. hArchive is not used here or anywhere in this file (the
//  callers pass 0): Fog resolves the archive from the open handle.
//  void: the tail jump to FOG_FCloseFile leaves its answer in eax, and no caller reads it.
void __fastcall ARCHIVE_CloseFile(void *hArchive, void *hFile)
{
    UNREFERENCED_PARAMETER(hArchive);

    if (!hFile)
    {
        FOG_DisplayAssert("hFile", szArchiveCpp, 73);
        exit(-1);
    }

    FOG_FCloseFile(hFile);
}


//1.10f: D2Client.0x6FB6B97E
//  Returns the size of the file, and a zero-length answer kills the process. FOG_FGetFileSize returns the low dword of a 64-bit size and writes
//  the high dword through pdwSizeHigh. Zero is its failure signal, so the failure arm recovers the name with SFileGetFileName into a 260-byte
//  buffer, reports it through FOG_DisplayError(3, szName, ...) at :95, and exits; a genuinely empty file would not survive this path either.
//  BUG: the sole caller, ARCHIVE_LoadFile, never reads the high dword, so a file of 4 GB or more would load short and silently. LATENT: an MPQ
//  member cannot be that large.
uint32_t __fastcall ARCHIVE_GetFileSize(void *hArchive, void *hFile, uint32_t *pdwSizeHigh)
{
    uint32_t dwFileSize;
    char szFileName[260];

    UNREFERENCED_PARAMETER(hArchive);

    if (!hFile)
    {
        FOG_DisplayAssert("hFile", szArchiveCpp, 89);
        exit(-1);
    }

    dwFileSize = FOG_FGetFileSize(hFile, pdwSizeHigh);
    if (!dwFileSize)
    {
        STORM_SFileGetFileName(hFile, szFileName, 260);
        FOG_DisplayError(3, szFileName, szArchiveCpp, 95);
        exit(-1);
    }

    return dwFileSize;
}


//1.10f: D2Client.0x6FB6B9ED
//  Reads exactly dwBytesToRead bytes or dies. Three fatal checks and no failure return: hFile at :135, a failed read at :143 and a short read at
//  :146, whose assert text is "dwBytesToRead == dwBytes". So the callers know the exact size in advance and never check (PALSHIFT_Init reads
//  0x1E00 into gRandTransformTables and 0x100 into gpGreenBloodRemap straight into fixed arrays).
//  The byte count is passed to FOG_FReadFile by value and the same stack slot is its out-parameter, so the original keeps the requested count in
//  a register (edi) and compares against it afterwards:
//      dwBytesRequested = dwBytesToRead;
//      FOG_FReadFile(hFile, pBuffer, dwBytesToRead, &dwBytesToRead, 0, 0, 0);
//      if (dwBytesRequested != dwBytesToRead) -> assert
//  bReadOk asks whether the read happened, the compare asks whether it was complete.
//  void: eax is the last call's leftover, and no caller reads it.
void __fastcall ARCHIVE_ReadFile(void *hArchive, void *hFile, void *pBuffer, uint32_t dwBytesToRead)
{
    uint32_t dwBytesRequested;
    int32_t bReadOk;
    char szFileName[260];

    UNREFERENCED_PARAMETER(hArchive);

    if (!hFile)
    {
        FOG_DisplayAssert("hFile", szArchiveCpp, 135);
        exit(-1);
    }

    dwBytesRequested = dwBytesToRead;
    bReadOk = FOG_FReadFile(hFile, pBuffer, dwBytesToRead, &dwBytesToRead, 0, 0, 0);
    if (!bReadOk)
    {
        STORM_SFileGetFileName(hFile, szFileName, 260);
        FOG_DisplayError(3, szFileName, szArchiveCpp, 143);
        exit(-1);
    }

    if (dwBytesRequested != dwBytesToRead)
    {
        FOG_DisplayAssert("dwBytesToRead == dwBytes", szArchiveCpp, 146);
        exit(-1);
    }

}


//1.10f: D2Client.0x6FB6BA94
//  Opens, sizes, allocates, reads and closes: the client's whole "give me this file in memory" path, and the only function in this file that
//  fails softly: a file that will not open returns NULL. Everything after the open is fatal, because it goes through the two functions above.
//  The buffer is 800 bytes (0x320) larger than the file: FOG_Alloc(size + 0x320). Nothing here touches the slack; it belongs to whatever parses
//  the file afterwards. Arguments 4 and 5 of FOG_Alloc are the caller's __FILE__ and __LINE__, so a leak reports the call site rather than this
//  function. pdwFileSize may be NULL.
//  The original inlines ARCHIVE_CloseFile here (its assert carries ARCHIVE_CloseFile's own line, 73) and ARCHIVE_OpenFileOrTrace (the open arm
//  leaves a GetLastError call whose result is dropped, because with bQuietIfMissing = 0 the comparison with 2 folds away and a call to an import
//  stays). The second inline is inferred from that residue; it has no assert line to fingerprint. What goes into nUnused is a choice, not a
//  measurement: the argument leaves no trace in the bytes, and hArchive is written because every real call site passes it.
static_assert(800 == 0x320,
              "ARCHIVE_LoadFile over-allocates by 0x320 -- `lea ecx, "
              "[esi+320h]` at 0x6FB6BADB is FOG_Alloc's size argument");

void *__fastcall ARCHIVE_LoadFile(void *hArchive, const char *szFilename, uint32_t *pdwFileSize, const char *szSrcFile, int32_t nSrcLine)
{
    uint32_t dwFileSize;
    void *pBuffer;
    uint32_t dwSizeHigh;
    void *hFile;

    if (!ARCHIVE_OpenFileOrTrace(hArchive, szFilename, &hFile, 0))
        return NULL;

    dwFileSize = ARCHIVE_GetFileSize(hArchive, hFile, &dwSizeHigh);

    pBuffer = FOG_Alloc(dwFileSize + 800, szSrcFile, nSrcLine, 0);
    if (!pBuffer)
        return NULL;

    ARCHIVE_ReadFile(hArchive, hFile, pBuffer, dwFileSize);
    ARCHIVE_CloseFile(hArchive, hFile);

    if (pdwFileSize)
        *pdwFileSize = dwFileSize;

    return pBuffer;
}

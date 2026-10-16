//  Record.h -- the globals of Record.cpp: the demo recorder's file name, file header, holder chain and the four flags that drive it.
//  Address range 0x6FB76048..0x6FBAB990. Each is a C++17 `inline` variable, one object for every file that includes this header.
//  gpRecordBlock and FileName hold values in the image (gpRecordBlock the address of gRecordHolderFirst, FileName "Record.dr1") and are
//  initialised to the same; the other seven lie past .data's raw data and are zero at load. gpRecordBlock is written after
//  gRecordHolderFirst, out of address order, because its initializer takes that object's address.
//  Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB76088
//  The demo file name `Record.dr1`: ten characters and the NUL. RECORD_SaveToFile and RECORD_Play pass it to CreateFileA.
inline CHAR FileName[11] = "Record.dr1";
static_assert(sizeof(FileName) == 11, "FileName: the item's 11 bytes in the .i64");

//  D2Client 0x6FBA9A20
//  The 20-byte header of a recording file (`sgtReadFileHeader` in the original's asserts). RECORD_Play reads it here in one ReadFile and
//  RECORD_SaveToFile writes the same 20 bytes from its stack. +0 nVersion (1, or RECORD_Play clears gbPlaybackRequested and
//  gbRecordPlaybackLoaded), +4 nHolders (the number of 500-record holders), +8 nRecords, +12 nEndFrame, +16 nInitSeed.
inline D2ClientRecordFileHeaderStrc gRecordFileHeader;
static_assert(sizeof(gRecordFileHeader) == 20, "gRecordFileHeader: the item's 20 bytes in the .i64");

//  D2Client 0x6FBA9A34
//  The cleanup handler that was installed before RECORD_InstallSaveOnCleanup ran, kept so RECORD_CleanupHandler can chain to it.
inline int32_t (__stdcall *gpfnPreviousCleanupHandler)(int32_t, int32_t, int32_t);
static_assert(sizeof(gpfnPreviousCleanupHandler) == 4, "gpfnPreviousCleanupHandler: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA9A38
//  The statically allocated first record holder (8012 bytes). gpRecordBlock points at it in the image, so the chain always has a head without
//  allocating one. Its nRecordWriteCurr (+0x1F40) and ptNext (+0x1F48) are reached through this object. RECORD_Play reads at most 500 records
//  (8000 of its 8012 bytes) into it.
inline D2ClientRecordHolderStrc gRecordHolderFirst;
static_assert(sizeof(gRecordHolderFirst) == 8012, "gRecordHolderFirst: the item's 8012 bytes in the .i64");

//  D2Client 0x6FB76048
//  The record block currently being filled or played back (`sgptHolderCurr` in the original's asserts). Initialised in the image to
//  gRecordHolderFirst. A block is 0x1F4C = 8012 bytes, allocated with FOG_Alloc by RECORD_LogEvent and by RECORD_Play (for each holder after
//  the first of the file it loads) and chained through +0x1F48. +0x1F40 is the write count and +0x1F44 the read cursor; both stop at 500.
//  Each of the 500 entries is 16 bytes (8012 minus the three header dwords is 8000).
inline D2ClientRecordHolderStrc *gpRecordBlock = &gRecordHolderFirst;
static_assert(sizeof(gpRecordBlock) == 4, "gpRecordBlock: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB984
//  Set by the `-r` switch (RECORD_RequestRecording) unless a `-p` is pending; RECORD_Play clears it. When it is set GAME_AppModeGame turns the
//  recorder on (RECORD_InstallSaveOnCleanup) and writes Record.dr1 after the game loop (RECORD_SaveToFile).
inline int32_t gbRecordingRequested;
static_assert(sizeof(gbRecordingRequested) == 4, "gbRecordingRequested: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB988
//  Set to 1 by RECORD_InstallSaveOnCleanup. RECORD_LogEvent logs nothing while it is 0, so it is the switch that turns event recording on.
inline int32_t gbRecordSaveOnCleanupInstalled;
static_assert(sizeof(gbRecordSaveOnCleanupInstalled) == 4, "gbRecordSaveOnCleanupInstalled: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB98C
//  Set to 1 by RECORD_Play (the `-p` switch) before it opens Record.dr1, and cleared when the header's version is not 1; RECORD_RequestRecording
//  clears a pending one. When it is set GAME_AppModeGame starts the playback through RECORD_SetRecordPlaybackLoaded.
inline int32_t gbPlaybackRequested;
static_assert(sizeof(gbPlaybackRequested) == 4, "gbPlaybackRequested: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB990
//  Set by RECORD_SetRecordPlaybackLoaded: a playback is running. RECORD_Play clears it when the header's version is not 1 and
//  RECORD_ReadNextForFrame at the recording's end frame. RECORD_ReadNextForFrame replays while it is set, and WNDPROC_WndProc keeps real
//  input from the game while it is set.
inline int32_t gbRecordPlaybackLoaded;
static_assert(sizeof(gbRecordPlaybackLoaded) == 4, "gbRecordPlaybackLoaded: the item's 4 bytes in the .i64");

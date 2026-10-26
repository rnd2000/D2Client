#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "dFloor.h"
#include "SoundHdr.h"

//  SoundHdr.cpp -- the sound headers of D2Client.dll 1.10: sounds.txt and soundenviron.txt compiled and held (gpSoundHeaders, 142 bytes a row;
//  gpSoundEnvironTxt, 88), the accessors every other sound unit reads them through, each sound's reference count, and the loading, caching and
//  freeing of its wave data.
//  The original source file is Sound\SoundHdr.cpp (the asserts name it; the first is SoundHdr.cpp:252 in SOUNDHDR_LoadSoundsTxt). Address range
//  0x6FB53C00..0x6FB55518, 27 functions, from SOUNDHDR_GetRecord to SOUNDHDR_TouchCacheEntry. Its .bss is the fifteen dwords from
//  gpSoundEnvironLookup (0x6FBBD494) to gnSoundCacheUsed (0x6FBBD4C8).

static const char szSoundHdrCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\Sound\\SoundHdr.cpp";

static_assert(offsetof(D2SoundsTxt, nGroupSize) == 0x3D && offsetof(D2SoundsTxt, nGroupBaseIndex) == 0x60,
              "the group fields of the sounds.txt record: `mov al, [eax+3Dh]` (SOUNDHDR_GetGroupSize 0x6FB53C83), "
              "`mov eax, [eax+60h]` (SOUNDHDR_GetGroupBaseIndex 0x6FB53C51)");
static_assert(offsetof(D2LevelsTxt, dwSoundEnv) == 0x21C,
              "levels.txt SoundEnv, read as a byte: `mov cl, [eax+21Ch]` (SOUNDHDR_GetCurrentSoundEnvironId 0x6FB53D18)");


//1.10f: D2Client.0x6FB53C00
//  A sound id's sounds.txt row, or NULL: no table, or an id outside [0, gnSoundHeaderCount), signed. The row is 142 bytes. The sound units'
//  common door; the original inlines it in many callers.
D2SoundsTxt *__fastcall SOUNDHDR_GetRecord(D2C_Sounds nSoundId)
{
    if (gpSoundHeaders && (int32_t)nSoundId >= 0 && (int32_t)nSoundId < gnSoundHeaderCount)
        return &gpSoundHeaders[(int32_t)nSoundId];
    return NULL;
}


//1.10f: D2Client.0x6FB53C30
//  The first id of a sound's group: the row's nGroupBaseIndex (+60h), which SOUNDHDR_LoadSoundsTxt fills in its second pass; a group's head and
//  each row inside it get the head's id, any other row its own. No table answers 0. The original inlines SOUNDHDR_GetRecord here.
D2C_Sounds __fastcall SOUNDHDR_GetGroupBaseIndex(D2C_Sounds nSoundId)
{
    if (!gpSoundHeaders)
        return SOUND_NONE;

    //  BUG: an id outside the table reads the field off NULL: the inlined SOUNDHDR_GetRecord's answer is used untested, so the out-of-range arm is
    //  xor eax, eax / mov eax, [eax+60h] (0x6FB53C55) and faults at address 0x60. LATENT. Reproduced by the call through SOUNDHDR_GetRecord, which
    //  answers NULL there.
    return (D2C_Sounds)SOUNDHDR_GetRecord(nSoundId)->nGroupBaseIndex;
}


//1.10f: D2Client.0x6FB53C60
//  A sound's group size: the row's nGroupSize (+3Dh), sounds.txt's Group Size on a group's head and 1 on every other row (the loader's second
//  pass). No table answers 0. A byte: both callers take it with `and eax, 0FFh`. The original inlines SOUNDHDR_GetRecord here.
uint8_t __fastcall SOUNDHDR_GetGroupSize(D2C_Sounds nSoundId)
{
    if (!gpSoundHeaders)
        return 0;

    //  BUG: the out-of-range arm reads +3Dh off NULL (xor eax, eax / mov al, [eax+3Dh], 0x6FB53C87) and faults at 0x3D, as in
    //  SOUNDHDR_GetGroupBaseIndex. LATENT.
    return SOUNDHDR_GetRecord(nSoundId)->nGroupSize;
}


//1.10f: D2Client.0x6FB53C90
//  Does a sound loop: the row's bLoop (+3Eh), sounds.txt's Loop, widened to an int. No table answers 0. The one caller is
//  UNITSND_UpdateUnitModeSound. The original inlines SOUNDHDR_GetRecord here.
int32_t __fastcall SOUNDHDR_IsLooping(D2C_Sounds nSoundId)
{
    if (!gpSoundHeaders)
        return 0;

    //  BUG: the out-of-range arm reads +3Eh off NULL (xor eax, eax / xor ecx, ecx / mov cl, [eax+3Eh], 0x6FB53CBA) and faults at 0x3E, as in
    //  SOUNDHDR_GetGroupBaseIndex. LATENT.
    return SOUNDHDR_GetRecord(nSoundId)->bLoop;
}


//1.10f: D2Client.0x6FB53CD0
//  The level the player stands in, or 0: no player, or no room.
int32_t __cdecl SOUNDHDR_GetPlayerLevelId()
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return 0;

    pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
    if (!pRoom)
        return 0;

    return D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);
}


//1.10f: D2Client.0x6FB53CF0
//  The soundenviron.txt row of the player's level: levels.txt's SoundEnv, read as a byte (xor ecx, ecx / mov cl, [eax+21Ch] / mov eax, ecx) out of
//  the dword D2LevelsTxt holds it in. 0 for no player, no room or level 0. The `mov eax, ecx` hands back the whole register, zero above the byte,
//  so the int return stands.
//  BUG: the levels.txt row is read untested: D2Common #10631 answers NULL for a level id outside [0, nLevelsTxtRecordCount), and the
//  mov cl, [eax+21Ch] at 0x6FB53D18 follows the call with no test. LATENT: the id comes from the room the player is in.
int32_t __cdecl SOUNDHDR_GetCurrentSoundEnvironId()
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    int32_t nLevelId;

    pPlayer = CUNIT_GetPlayer();
    if (pPlayer)
    {
        pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
        if (pRoom)
        {
            nLevelId = D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);
            if (nLevelId)
                return (uint8_t)D2COMMON_DATATBLS_GetLevelsTxtRecord(nLevelId)->dwSoundEnv;
        }
    }
    return 0;
}


//1.10f: D2Client.0x6FB53D30
//  A soundenviron.txt row, or NULL: no table, or an id outside [0, gnSoundEnvironTxtRecordCount), signed as GetRecord's. The row is 88 bytes.
D2SoundEnvironTxt *__fastcall SOUNDHDR_GetEnvRecord(int32_t nSoundEnvId)
{
    if (gpSoundEnvironTxt && nSoundEnvId >= 0 && nSoundEnvId < gnSoundEnvironTxtRecordCount)
        return &gpSoundEnvironTxt[nSoundEnvId];
    return NULL;
}


//1.10f: D2Client.0x6FB53D60
//  Returns gnSoundEnvironLookupMin. SOUNDHDR_LoadSoundsTxt sets the value and SOUNDHDR_Shutdown clears it.
int32_t __cdecl SOUNDHDR_GetEnvironLookupMin()
{
    return gnSoundEnvironLookupMin;
}


//1.10f: D2Client.0x6FB53D70
//  Returns gnSoundEnvironLookupMax, GetEnvironLookupMin's twin, set and cleared by the same two.
int32_t __cdecl SOUNDHDR_GetEnvironLookupMax()
{
    return gnSoundEnvironLookupMax;
}


//1.10f: D2Client.0x6FB53D80
//  Returns gpSoundEnvironLookup: the table of ints SOUNDHDR_LoadSoundsTxt allocates, one per song id from gnSoundEnvironLookupMin to Max.
//  SOUNDSONG_UpdateMusic reads and writes it as [eax+esi*4] with esi the song id less the minimum, so it answers int *.
int32_t *__cdecl SOUNDHDR_GetEnvironLookup()
{
    return gpSoundEnvironLookup;
}


//1.10f: D2Client.0x6FB53D90
//  Brings the sound system up when D2Sound has a DirectSound object (D2Sound #10002 nonzero), and not at all otherwise: the two tables loaded, the
//  sixteen channels created, the playback reset, the song state reset (SOUNDSONG_Init), the level sound state reset
//  (UNITSND_ResetLevelSoundState), the master volume set to 255, and the four preload and throttle globals cleared.
void __cdecl SOUNDHDR_Init()
{
    if (!D2SOUND_10002_GetDirectSound())
        return;

    SOUNDHDR_LoadSoundsTxt();
    SOUNDCHAN_CreateSystem();
    SOUNDINST_ResetPlayback();
    SOUNDSONG_Init();
    UNITSND_ResetLevelSoundState();
    D2SOUND_10027_ApplyMasterVolume(255);

    gnSoundPreloadCursor = 0;
    gdwSoundLastServiceTick = 0;
    gdwSoundLastLoadTick = 0;
    gnSoundAsyncLoadsInFlight = 0;
}


static_assert(TXTFIELD_NONE == 0 && TXTFIELD_ASCII == 1 && TXTFIELD_DWORD == 2 && TXTFIELD_WORD == 3 && TXTFIELD_BYTE == 4
              && offsetof(D2BinFieldStrc, nFieldType) == 0x04 && offsetof(D2BinFieldStrc, nFieldLength) == 0x08
              && offsetof(D2BinFieldStrc, nFieldOffset) == 0x0C && offsetof(D2BinFieldStrc, pLinkField) == 0x10,
              "a column descriptor as SOUNDHDR_LoadSoundsTxt stores the first: \"Filename\", 1, 3Bh, 0, 0 at "
              "[esp+4CCh+var_2E4] .. [esp+4CCh+var_2D4] (0x6FB53E0E..0x6FB53E36); the types 2, 3, 4 and 0 the rows' own");
static_assert(offsetof(D2SoundsTxt, nFadeIn) == 0x3F && offsetof(D2SoundsTxt, nFadeOut) == 0x40
              && offsetof(D2SoundsTxt, nDeferInst) == 0x41 && offsetof(D2SoundsTxt, nStopInst) == 0x42
              && offsetof(D2SoundsTxt, nDuration) == 0x43 && offsetof(D2SoundsTxt, nCompound) == 0x45
              && offsetof(D2SoundsTxt, dwFalloff) == 0x47 && offsetof(D2SoundsTxt, bCache) == 0x4C
              && offsetof(D2SoundsTxt, nPriority) == 0x4E && offsetof(D2SoundsTxt, bTracking) == 0x51
              && offsetof(D2SoundsTxt, nBlock2) == 0x58 && offsetof(D2SoundsTxt, nBlock3) == 0x5C
              && offsetof(D2SoundsTxt, nCacheStamp) == 0x7C && offsetof(D2SoundsTxt, hAsyncData) == 0x82,
              "sounds.txt's descriptor offsets 3Fh .. 5Ch (0x6FB53ED7 .. 0x6FB541D2) not pinned in SoundChan.cpp, and "
              "SOUNDHDR_Shutdown's [esi+7Ch], [esi+82h] (0x6FB5491F, 0x6FB54919)");
static_assert(offsetof(D2SoundEnvironTxt, nSong) == 0x00 && offsetof(D2SoundEnvironTxt, bIndoors) == 0x18
              && offsetof(D2SoundEnvironTxt, nEaxEnvDiff) == 0x2C && offsetof(D2SoundEnvironTxt, nEaxRoomVol) == 0x30
              && offsetof(D2SoundEnvironTxt, nEaxRoomHF) == 0x34 && offsetof(D2SoundEnvironTxt, nEaxDecayTime) == 0x38
              && offsetof(D2SoundEnvironTxt, nEaxDecayHF) == 0x3C && offsetof(D2SoundEnvironTxt, nEaxReflect) == 0x40
              && offsetof(D2SoundEnvironTxt, nEaxReflectDelay) == 0x44 && offsetof(D2SoundEnvironTxt, nEaxReverb) == 0x48
              && offsetof(D2SoundEnvironTxt, nEaxRevDelay) == 0x4C && offsetof(D2SoundEnvironTxt, nEaxRoomRoll) == 0x50,
              "soundenviron.txt's descriptor offsets not pinned in SoundChan.cpp or SoundEnv.cpp: 0, 18h, 2Ch .. 50h "
              "(0x6FB54217 .. 0x6FB54525)");


//1.10f: D2Client.0x6FB53DE0
//  Loads sounds.txt and soundenviron.txt and fills in sounds.txt's groups. Each table is the .txt itself under DATA\GLOBAL\EXCEL, read through
//  ARCHIVE_LoadFile, parsed (Fog #10208), counted (Fog #10210), allocated whole and zeroed (FOG_Alloc, an inlined memset) and filled by Fog #10207
//  from a column descriptor array built on the stack, {name, type, length, offset, link}: sounds.txt's 23 columns and soundenviron.txt's 22, each
//  closed by an "end" row. The two arrays are locals in the original (stored one dword at a time), so the C keeps them local and initialised.
//  Both tables must be unloaded: "sgptSoundHeaderTable == NULL" and "sgptSoundEnvironTable == NULL" (SoundHdr.cpp:252, :253), the original's names
//  for gpSoundHeaders and gpSoundEnvironTxt.
//  The second pass runs over sounds.txt's rows from 1 (row 0 is no sound, and keeps the zeroes of the allocation): nGroupBaseIndex is the current
//  group's head for a row inside [head, head + size) and the row's own id otherwise; a row whose Group Size is nonzero opens a new group, and one
//  whose Group Size is 0 is given 1, so a group's members end with a size of 1 and their head's id (SoundEnv.cpp's footstep clear depends on that);
//  nBlockCount is how many of Block 1, 2, 3 come before the first -1. The head and size are signed; the block counter is a byte compared unsigned.
//  The song table: gnSoundEnvironLookupMin and Max are the smallest and largest soundenviron.txt Song above 0 (signed), Min starting from the header
//  count and Max from 0, and gpSoundEnvironLookup is Max - Min + 1 zeroed ints; SOUNDSONG_UpdateMusic keeps one value per song id there. A table
//  with none would make the size (0 - count + 1) * 4, which wraps; no stock row is that table.
//  BUG: the song table is never freed: the global's one write is the store here, SOUNDHDR_Shutdown frees both tables and zeroes Min and Max but
//  leaves this block, and the next SOUNDHDR_Init's load allocates another over the pointer. REACHABLE: 112 bytes a game with the stock data.
void __cdecl SOUNDHDR_LoadSoundsTxt()
{
    D2BinFieldStrc aSoundEnvironFields[] =
    {
        { "Song",              TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nSong),            NULL },
        { "Day Ambience",      TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nDayAmbience),     NULL },
        { "Night Ambience",    TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nNightAmbience),   NULL },
        { "Day Event",         TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nDayEvent),        NULL },
        { "Night Event",       TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nNightEvent),      NULL },
        { "Event Delay",       TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nEventDelay),      NULL },
        { "Indoors",           TXTFIELD_BYTE,  0, offsetof(D2SoundEnvironTxt, bIndoors),         NULL },
        { "Material 1",        TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nMaterial1),       NULL },
        { "Material 2",        TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nMaterial2),       NULL },
        { "EAX Environ",       TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nEaxEnviron),      NULL },
        { "EAX Env Size",      TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nEaxEnvSize),      NULL },
        { "EAX Env Diff",      TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nEaxEnvDiff),      NULL },
        { "EAX Room Vol",      TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nEaxRoomVol),      NULL },
        { "EAX Room HF",       TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nEaxRoomHF),       NULL },
        { "EAX Decay Time",    TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nEaxDecayTime),    NULL },
        { "EAX Decay HF",      TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nEaxDecayHF),      NULL },
        { "EAX Reflect",       TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nEaxReflect),      NULL },
        { "EAX Reflect Delay", TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nEaxReflectDelay), NULL },
        { "EAX Reverb",        TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nEaxReverb),       NULL },
        { "EAX Rev Delay",     TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nEaxRevDelay),     NULL },
        { "EAX Room Roll",     TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nEaxRoomRoll),     NULL },
        { "EAX Air Absorb",    TXTFIELD_DWORD, 0, offsetof(D2SoundEnvironTxt, nEaxAirAbsorb),    NULL },
        { "end",               TXTFIELD_NONE,  0, 0,                                              NULL },
    };
    D2BinFieldStrc aSoundsTxtFields[] =
    {
        { "Filename",   TXTFIELD_ASCII, 59, offsetof(D2SoundsTxt, szFileName), NULL },
        { "Volume",     TXTFIELD_BYTE,  0,  offsetof(D2SoundsTxt, nVolume),    NULL },
        { "Group Size", TXTFIELD_BYTE,  0,  offsetof(D2SoundsTxt, nGroupSize), NULL },
        { "Loop",       TXTFIELD_BYTE,  0,  offsetof(D2SoundsTxt, bLoop),      NULL },
        { "Fade In",    TXTFIELD_BYTE,  0,  offsetof(D2SoundsTxt, nFadeIn),    NULL },
        { "Fade Out",   TXTFIELD_BYTE,  0,  offsetof(D2SoundsTxt, nFadeOut),   NULL },
        { "Defer Inst", TXTFIELD_BYTE,  0,  offsetof(D2SoundsTxt, nDeferInst), NULL },
        { "Stop Inst",  TXTFIELD_BYTE,  0,  offsetof(D2SoundsTxt, nStopInst),  NULL },
        { "Duration",   TXTFIELD_WORD,  0,  offsetof(D2SoundsTxt, nDuration),  NULL },
        { "Compound",   TXTFIELD_WORD,  0,  offsetof(D2SoundsTxt, nCompound),  NULL },
        { "Falloff",    TXTFIELD_DWORD, 0,  offsetof(D2SoundsTxt, dwFalloff),  NULL },
        { "Reverb",     TXTFIELD_BYTE,  0,  offsetof(D2SoundsTxt, nReverb),    NULL },
        { "Cache",      TXTFIELD_BYTE,  0,  offsetof(D2SoundsTxt, bCache),     NULL },
        { "Async Only", TXTFIELD_BYTE,  0,  offsetof(D2SoundsTxt, bAsyncOnly), NULL },
        { "Priority",   TXTFIELD_BYTE,  0,  offsetof(D2SoundsTxt, nPriority),  NULL },
        { "Stream",     TXTFIELD_BYTE,  0,  offsetof(D2SoundsTxt, bStream),    NULL },
        { "Stereo",     TXTFIELD_BYTE,  0,  offsetof(D2SoundsTxt, bStereo),    NULL },
        { "Tracking",   TXTFIELD_BYTE,  0,  offsetof(D2SoundsTxt, bTracking),  NULL },
        { "Solo",       TXTFIELD_BYTE,  0,  offsetof(D2SoundsTxt, bSolo),      NULL },
        { "Music Vol",  TXTFIELD_BYTE,  0,  offsetof(D2SoundsTxt, nMusicVol),  NULL },
        { "Block 1",    TXTFIELD_DWORD, 0,  offsetof(D2SoundsTxt, nBlock1),    NULL },
        { "Block 2",    TXTFIELD_DWORD, 0,  offsetof(D2SoundsTxt, nBlock2),    NULL },
        { "Block 3",    TXTFIELD_DWORD, 0,  offsetof(D2SoundsTxt, nBlock3),    NULL },
        { "end",        TXTFIELD_NONE,  0,  0,                                 NULL },
    };
    char szFilename[MAX_PATH];
    uint32_t dwFileSize;
    void *pFileData;
    D2BinFileStrc *pBinFile;
    D2SoundsTxt *pRecord;
    D2SoundEnvironTxt *pEnvironRec;
    int32_t nSoundId;
    int32_t nGroupBase;
    int32_t nGroupSize;
    uint8_t nBlockCount;
    int32_t nRowsLeft;
    int32_t nSong;
    int32_t nLookupMin;
    int32_t nLookupMax;
    uint32_t nLookupBytes;

    if (gpSoundHeaders)
    {
        FOG_DisplayAssert("sgptSoundHeaderTable == NULL", szSoundHdrCpp, 252);
        exit(-1);
    }
    if (gpSoundEnvironTxt)
    {
        FOG_DisplayAssert("sgptSoundEnvironTable == NULL", szSoundHdrCpp, 253);
        exit(-1);
    }

    //  sounds.txt.
    wsprintfA(szFilename, "%s\\sounds.txt", "DATA\\GLOBAL\\EXCEL");
    pFileData = ARCHIVE_LoadFile(D2WIN_GetArchive(), szFilename, &dwFileSize, szSoundHdrCpp, 257);
    pBinFile = FOG_CreateBinFile(pFileData, dwFileSize);
    gnSoundHeaderCount = FOG_GetRecordCountFromBinFile(pBinFile);
    gpSoundHeaders = (D2SoundsTxt *)FOG_Alloc(sizeof(D2SoundsTxt) * gnSoundHeaderCount, szSoundHdrCpp, 262, 0);
    memset(gpSoundHeaders, 0, sizeof(D2SoundsTxt) * gnSoundHeaderCount);
    FOG_10207(pBinFile, aSoundsTxtFields, gpSoundHeaders, gnSoundHeaderCount, sizeof(D2SoundsTxt));
    FOG_FreeBinFile(pBinFile);

    //  soundenviron.txt.
    wsprintfA(szFilename, "%s\\soundenviron.txt", "DATA\\GLOBAL\\EXCEL");
    pFileData = ARCHIVE_LoadFile(D2WIN_GetArchive(), szFilename, &dwFileSize, szSoundHdrCpp, 269);
    pBinFile = FOG_CreateBinFile(pFileData, dwFileSize);
    gnSoundEnvironTxtRecordCount = FOG_GetRecordCountFromBinFile(pBinFile);
    gpSoundEnvironTxt = (D2SoundEnvironTxt *)FOG_Alloc(sizeof(D2SoundEnvironTxt) * gnSoundEnvironTxtRecordCount, szSoundHdrCpp, 274, 0);
    memset(gpSoundEnvironTxt, 0, sizeof(D2SoundEnvironTxt) * gnSoundEnvironTxtRecordCount);
    FOG_10207(pBinFile, aSoundEnvironFields, gpSoundEnvironTxt, gnSoundEnvironTxtRecordCount, sizeof(D2SoundEnvironTxt));
    FOG_FreeBinFile(pBinFile);

    //  The second pass over sounds.txt.
    nGroupSize = 1;
    nGroupBase = 0;
    for (nSoundId = 1; nSoundId < gnSoundHeaderCount; ++nSoundId)
    {
        pRecord = &gpSoundHeaders[nSoundId];

        if (nGroupBase && nSoundId >= nGroupBase && nSoundId < nGroupSize + nGroupBase)
            pRecord->nGroupBaseIndex = nGroupBase;
        else
            pRecord->nGroupBaseIndex = nSoundId;

        if (pRecord->nGroupSize)
        {
            nGroupBase = nSoundId;
            nGroupSize = pRecord->nGroupSize;
        }
        else
        {
            pRecord->nGroupSize = 1;
        }

        //  Block 1, 2 and 3 are walked as an array, [esi+ebp*4+54h]; the C steps through the record's own bytes.
        for (nBlockCount = 0; nBlockCount < 3; ++nBlockCount)
        {
            if (*(int32_t *)((char *)pRecord + offsetof(D2SoundsTxt, nBlock1) + sizeof(int32_t) * nBlockCount) == -1)
                break;
        }
        pRecord->nBlockCount = nBlockCount;
    }

    //  The song table.
    nLookupMin = gnSoundHeaderCount;
    nLookupMax = 0;
    pEnvironRec = gpSoundEnvironTxt;
    gnSoundEnvironLookupMin = nLookupMin;
    gnSoundEnvironLookupMax = nLookupMax;
    for (nRowsLeft = gnSoundEnvironTxtRecordCount; nRowsLeft > 0; --nRowsLeft, ++pEnvironRec)
    {
        nSong = pEnvironRec->nSong;
        if (nSong > 0)
        {
            if (nSong < nLookupMin)
            {
                nLookupMin = nSong;
                gnSoundEnvironLookupMin = nLookupMin;
            }
            if (nSong > nLookupMax)
            {
                nLookupMax = nSong;
                gnSoundEnvironLookupMax = nLookupMax;
            }
        }
    }

    nLookupBytes = sizeof(int32_t) * (nLookupMax - nLookupMin + 1);
    gpSoundEnvironLookup = (int32_t *)FOG_Alloc(nLookupBytes, szSoundHdrCpp, 327, 0);
    memset(gpSoundEnvironLookup, 0, nLookupBytes);
}


//1.10f: D2Client.0x6FB54860
//  Shuts the sound system down, when D2Sound has a DirectSound object: every channel stopped and detached, every instance stopped and freed, the
//  channels shut down, soundenviron.txt freed (SoundHdr.cpp:385), then every row's wave data freed by SOUNDHDR_FreeSoundData's body, inlined here
//  (the cache's count lowered by the row's size, its node taken off gSoundDataList with D2Sound #10059, its async handle freed or its block,
//  SoundHdr.cpp:626), and sounds.txt (SoundHdr.cpp:398); the song table's Min and Max are zeroed, its block is not (see
//  SOUNDHDR_LoadSoundsTxt). Row 0 is never visited: its data, if any, goes with the table.
//  A row still playing is an error: "!SoundChannelFind(eSound)" (SoundHdr.cpp:601) around SOUNDCHAN_FindOldestPlaying, the original's
//  SoundChannelFind.
void __cdecl SOUNDHDR_Shutdown()
{
    int32_t nSoundId;

    if (!D2SOUND_10002_GetDirectSound())
        return;

    SOUNDCHAN_StopAllAndDetach();
    SOUNDINST_StopAllInstances();
    SOUNDINST_FreeAllInstances();
    SOUNDCHAN_ShutdownSystem();

    if (gpSoundEnvironTxt)
    {
        FOG_Free(gpSoundEnvironTxt, szSoundHdrCpp, 385, 0);
        gpSoundEnvironTxt = NULL;
        gnSoundEnvironTxtRecordCount = 0;
    }

    if (gpSoundHeaders)
    {
        for (nSoundId = 1; nSoundId < gnSoundHeaderCount; ++nSoundId)
        {
            SOUNDHDR_FreeSoundData((D2C_Sounds)nSoundId);
        }

        FOG_Free(gpSoundHeaders, szSoundHdrCpp, 398, 0);
        gpSoundHeaders = NULL;
        gnSoundHeaderCount = 0;
    }

    gnSoundEnvironLookupMin = 0;
    gnSoundEnvironLookupMax = 0;
}


//1.10f: D2Client.0x6FB549C0
//  A sound's group taken or released: nDelta added to the dwRefCount of every row of the id's group, from the row's nGroupBaseIndex, nGroupSize
//  rows, both re-read each pass, so a group's head takes or releases all its members, and a member id (whose base is its head and whose size the
//  loader set to 1) only its head. Sound 0 and an unknown id do nothing, and so does a release (-1) of a row whose own count is 0; a member at 0
//  released asserts, "ptHeader->dwRefCount > 0 || nDelta != -1" (SoundHdr.cpp:476). The cursor and the bound are signed, so the base is an int.
//  The original inlines SOUNDHDR_GetRecord here.
void __fastcall SOUNDHDR_AdjustRefCount(D2C_Sounds nSoundId, int32_t nDelta)
{
    D2SoundsTxt *ptHeader;
    D2SoundsTxt *pMember;
    int32_t nMemberId;

    if (!nSoundId)
        return;

    ptHeader = SOUNDHDR_GetRecord(nSoundId);
    if (!ptHeader)
        return;

    if (ptHeader->dwRefCount > 0 || nDelta != -1)
    {
        for (nMemberId = ptHeader->nGroupBaseIndex; nMemberId < ptHeader->nGroupBaseIndex + ptHeader->nGroupSize; ++nMemberId)
        {
            //  BUG: a group running past the table reads a member off NULL: each member is SOUNDHDR_GetRecord inlined and its NULL read
            //  through (xor eax, eax at 0x6FB54A3C, then mov esi, [eax+8Ah] at 0x6FB54A3E), faulting at 0x8A for a member id at or past
            //  gnSoundHeaderCount. LATENT.
            pMember = SOUNDHDR_GetRecord((D2C_Sounds)nMemberId);
            if (!(pMember->dwRefCount > 0 || nDelta != -1))
            {
                FOG_DisplayAssert("ptHeader->dwRefCount > 0 || nDelta != -1", szSoundHdrCpp, 476);
                exit(-1);
            }
            pMember->dwRefCount += nDelta;
        }
    }
}


//1.10f: D2Client.0x6FB54A90
//  A sound's group's counts zeroed: dwRefCount 0 on every row from the id's nGroupBaseIndex, nGroupSize rows. AdjustRefCount's walk without the
//  release test and the assert. Called by SOUNDENV_UpdateEnvironment's footstep clear. The original inlines SOUNDHDR_GetRecord here.
void __fastcall SOUNDHDR_ClearRefCount(D2C_Sounds nSoundId)
{
    D2SoundsTxt *pRecord;
    int32_t nMemberId;

    if (!nSoundId)
        return;

    pRecord = SOUNDHDR_GetRecord(nSoundId);
    if (!pRecord)
        return;

    for (nMemberId = pRecord->nGroupBaseIndex; nMemberId < pRecord->nGroupBaseIndex + pRecord->nGroupSize; ++nMemberId)
    {
        //  BUG: the member past the table is written off NULL (xor ecx, ecx at 0x6FB54AF0, mov dword ptr [ecx+8Ah], 0 at 0x6FB54AF2), as in
        //  SOUNDHDR_AdjustRefCount. LATENT.
        SOUNDHDR_GetRecord((D2C_Sounds)nMemberId)->dwRefCount = 0;
    }
}


//1.10f: D2Client.0x6FB54B20
//  Services the asynchronous sound loads, from SOUNDHDR_PumpPreload with its counter as dwTickCount. A tick of 0 is the forced pass, with no
//  throttle at all; any other tick runs only 25 after the last service (unsigned). Each row from 1 to 2933 (a constant bound, 142 * 2933 as the
//  row offset, not gnSoundHeaderCount) that has no data (eDataState 0) and is referenced or cached is loaded (SOUNDHDR_LoadWaveData(id, forced, 1)),
//  unless a throttled pass already has 15 loads in flight or loaded within 250 ticks; a throttled row skips the completion test below as well.
//  A row still loading (state 1) whose Fog async data reports loaded takes its buffer and size, becomes state 2 and is prepared
//  (SOUNDCHAN_PrepareWaveBuffer), one load fewer in flight. The pass stamps gdwSoundLastServiceTick with the tick it began at.
//  The completion test carries an assert of SoundHdr.cpp:64, "ptHeader->eDataState == ESOUNDDATASTATE_ASYNCLOADING". The line runs backwards
//  against the file, so the block is a helper of the source that the original compiled in; no function has its body, so it is written in place.
//  Its assert cannot fire: the test it repeats is the `if` just above it.
//  BUG: rows 2934 and above are never serviced: a referenced or cached sound past 2933 is never loaded by this pass, and one whose async load
//  starts elsewhere is never completed here; and a sounds.txt of fewer than 2934 rows is walked past its end. LATENT: every stock row from 2934 on
//  is streamed.
//  void: the al that LoadWaveData or the others leave behind is not an answer; the one caller discards it.
void __fastcall SOUNDHDR_ServiceAsyncLoads(uint32_t dwTickCount)
{
    D2SoundsTxt *ptHeader;
    BOOL bForceLoad;
    int32_t nSoundId;

    if (!gpSoundHeaders)
        return;

    bForceLoad = dwTickCount == 0;
    if (dwTickCount && dwTickCount - gdwSoundLastServiceTick < 25)
        return;

    for (nSoundId = 1; nSoundId <= 2933; ++nSoundId)
    {
        ptHeader = &gpSoundHeaders[nSoundId];

        if (!ptHeader->eDataState && (ptHeader->dwRefCount || ptHeader->bCache))
        {
            if (!bForceLoad && (gnSoundAsyncLoadsInFlight >= 15 || dwTickCount - gdwSoundLastLoadTick < 250))
                continue;
            SOUNDHDR_LoadWaveData((D2C_Sounds)nSoundId, bForceLoad, 1);
        }

        if (ptHeader->eDataState == 1 && FOG_AsyncDataTestLoaded(ptHeader->hAsyncData))
        {
            if (ptHeader->eDataState != 1)
            {
                FOG_DisplayAssert("ptHeader->eDataState == ESOUNDDATASTATE_ASYNCLOADING", szSoundHdrCpp, 64);
                exit(-1);
            }
            ptHeader->pWaveData = FOG_AsyncDataGetBuffer(ptHeader->hAsyncData);
            ptHeader->nWaveSize = FOG_AsyncDataGetBufferSize(ptHeader->hAsyncData);
            ptHeader->eDataState = 2;
            SOUNDCHAN_PrepareWaveBuffer(ptHeader);
            --gnSoundAsyncLoadsInFlight;
        }
    }

    gdwSoundLastServiceTick = dwTickCount;
}


//1.10f: D2Client.0x6FB54C50
//  Returns gnSoundPreloadCursor, the counter SOUNDHDR_PumpPreload advances once a pump, which its callers use as a clock. unsigned int: the counter
//  is a DWORD and every caller consumes it unsigned.
uint32_t __cdecl SOUNDHDR_GetPreloadCursor()
{
    return gnSoundPreloadCursor;
}


//1.10f: D2Client.0x6FB54C60
//  One pump of the sound system, while its channels are up: the environment (SOUNDENV_UpdateEnvironment), the music (SOUNDSONG_UpdateMusic), the
//  async loads (SOUNDHDR_ServiceAsyncLoads) and the instances (SOUNDINST_ServiceInstances), each handed gnSoundPreloadCursor, then the finished
//  channels let go (SOUNDCHAN_ReapFinished); then the counter one up and Sleep(0).
void __cdecl SOUNDHDR_PumpPreload()
{
    if (!SOUNDCHAN_IsSystemReady())
        return;

    SOUNDENV_UpdateEnvironment(gnSoundPreloadCursor);
    SOUNDSONG_UpdateMusic(gnSoundPreloadCursor);
    SOUNDHDR_ServiceAsyncLoads(gnSoundPreloadCursor);
    SOUNDINST_ServiceInstances(gnSoundPreloadCursor);
    SOUNDCHAN_ReapFinished();
    ++gnSoundPreloadCursor;
    Sleep(0);
}


//1.10f: D2Client.0x6FB54CB0
//  Every channel and every instance stopped: SOUNDCHAN_StopAllAndDetach, then a tail jump to SOUNDINST_StopAllInstances.
void __cdecl SOUNDHDR_StopAllPending()
{
    SOUNDCHAN_StopAllAndDetach();
    SOUNDINST_StopAllInstances();
}


//1.10f: D2Client.0x6FB54CC0
//  One sound's data freed: when its row is loaded, the assert that nothing still plays it, "!SoundChannelFind(eSound)" (SoundHdr.cpp:601), the
//  cache's count lowered by its size, its node taken off gSoundDataList (D2Sound #10059), the cache stamp cleared, its async data freed through Fog
//  or its block (SoundHdr.cpp:626), the data pointer and the state cleared. SOUNDHDR_Shutdown's loop holds this whole body inlined. void: the one
//  caller discards eax. The original inlines SOUNDHDR_GetRecord here.
void __fastcall SOUNDHDR_FreeSoundData(D2C_Sounds nSoundId)
{
    D2SoundsTxt *ptHeader;

    //  BUG: an unknown id reads its row off NULL: the inlined SOUNDHDR_GetRecord's NULL is read through (xor esi, esi at 0x6FB54CE9, then
    //  cmp [esi+86h], ebx at 0x6FB54CEB), faulting at 0x86. LATENT.
    ptHeader = SOUNDHDR_GetRecord(nSoundId);
    if (!ptHeader->eDataState)
        return;

    if (SOUNDCHAN_FindOldestPlaying(nSoundId))
    {
        FOG_DisplayAssert("!SoundChannelFind(eSound)", szSoundHdrCpp, 601);
        exit(-1);
    }

    gnSoundCacheUsed -= ptHeader->nWaveSize;
    D2SOUND_10059_ListRemove(&gSoundDataList, nSoundId);
    ptHeader->nCacheStamp = 0;
    if (ptHeader->hAsyncData)
    {
        FOG_AsyncDataFree(ptHeader->hAsyncData);
        ptHeader->hAsyncData = 0;
    }
    else
    {
        FOG_Free(ptHeader->pWaveData, szSoundHdrCpp, 626, 0);
    }
    ptHeader->pWaveData = NULL;
    ptHeader->eDataState = 0;
}


static_assert(offsetof(D2SoundsTxt, dwRefCount) == 0x8A && offsetof(D2SoundListNodeStrc, pNext) == 4,
              "the eviction walk: `mov edx, [eax+8Ah]` (0x6FB55013) and `mov edi, [edi+4]` (0x6FB5504F), the node "
              "D2Sound #10063 allocates 8 bytes for (`mov ecx, 8`, D2Sound 0x6F985262)");
static_assert(SOUND_AMAZON_CANTCARRY_1 == 2934 && SOUND_MONSTER_NIHLATHAK_TAUNT_4 == 4656,
              "the DATA\\LOCAL range: `cmp ecx, 0B76h` / `jl`, `cmp ecx, 1230h` / `jg` (0x6FB553D8, 0x6FB553E0)");


//1.10f: D2Client.0x6FB54D90
//  One sound's wave data loaded into the cache. Called by SOUNDHDR_ServiceAsyncLoads (bServicePass 1, bForceLoad on its forced pass) and
//  SOUNDCHAN_StartInstance (bServicePass 0, bForceLoad when the record is not bAsyncOnly). void: neither caller uses eax.
//  Nothing is done for a row whose file failed to open before (bLoadFailed, +81h) or that holds its data (eDataState 2). A row still loading (1)
//  is completed only when forced: its Fog async data's buffer and size taken, the state 2, the buffer prepared (SOUNDCHAN_PrepareWaveBuffer), one
//  load fewer in flight; this is the block ServiceAsyncLoads writes in place, here without its line-64 assert. Unforced, the service pass finishes
//  it. A state other than 0, 1 and 2 returns.
//  The two tests of state 1 are the source's own pair: after cmp eax, 1 / jnz and test esi, esi / jz (0x6FB54E06..0x6FB54E0D), the second test
//  repeats both (cmp eax, eax / jnz and test esi, esi / jz, 0x6FB54E13..0x6FB54E19) and can be false on no path.
//  A row with no data (0) has its path built (SOUNDHDR_BuildSoundPath) and its file opened through Fog to measure it: an open that fails marks
//  the row bLoadFailed for good. The size is then fitted into the cache by two unsigned tests (jbe, 0x6FB54FBC and 0x6FB54FD4): the cache used
//  plus the size within gnSoundCacheBudget, or else the used within the budget less the size; the second differs from the first only when the size
//  passes the budget, where the subtraction wraps and lets the load through. Past both, the cache is emptied of other sounds until the used is
//  within that headroom: each pass walks gSoundDataList, D2Sound's list of the loaded ids, least recently touched first, and frees the first that
//  no channel plays (SOUNDCHAN_FindOldestPlaying, SOUNDHDR_FreeSoundData). The first pass skips a row still referenced (dwRefCount) or cached
//  (bCache); the second does not and runs only when forced, after the first whether or not the first freed one; the service pass skips a row
//  touched within 750 ticks of gnSoundPreloadCursor (unsigned). When a round frees nothing the load is abandoned and gdwSoundLastLoadTick takes the
//  cursor read on entry; ServiceAsyncLoads' 250-tick throttle reads it.
//  The load: forced, synchronously from the archive (ARCHIVE_LoadFile, SoundHdr.cpp:769) into pWaveData and nWaveSize, with the assert
//  "ptHeader->pbWaveData && ptHeader->dwWaveSize" (:770) on either missing; unforced, a Fog async load of the whole file (:795), state 1, one more
//  in flight; a failed one leaves the row in state 0 for the next pass. Either way the size is added to gnSoundCacheUsed and the id moved to the
//  list's tail with a fresh stamp (SOUNDHDR_TouchCacheEntry).
//  The original inlines SOUNDHDR_GetRecord, SOUNDHDR_BuildSoundPath and SOUNDHDR_TouchCacheEntry (twice) here.
void __fastcall SOUNDHDR_LoadWaveData(D2C_Sounds nSoundId, int32_t bForceLoad, int32_t bServicePass)
{
    D2SoundsTxt *ptHeader;
    D2SoundsTxt *pCandidate;
    D2SoundListNodeStrc *pNode;
    uint32_t dwCursorAtEntry;
    uint32_t dwCursor;
    uint32_t nFileSize;
    uint32_t nHeadroom;
    void *hFile;
    BOOL bEvicted;
    int32_t nPass;
    int32_t nCandidateId;
    char szFilename[80];

    //  BUG: an unknown id reads its row off NULL: the inlined SOUNDHDR_GetRecord's NULL is read through (mov al, [ebp+81h], 0x6FB54DE9). LATENT.
    ptHeader = SOUNDHDR_GetRecord(nSoundId);
    dwCursorAtEntry = gnSoundPreloadCursor;

    if (ptHeader->bLoadFailed || ptHeader->eDataState == 2)
        return;

    if (ptHeader->eDataState == 1 && !bForceLoad)
        return;
    if (ptHeader->eDataState == 1 && bForceLoad)
    {
        ptHeader->pWaveData = FOG_AsyncDataGetBuffer(ptHeader->hAsyncData);
        ptHeader->nWaveSize = FOG_AsyncDataGetBufferSize(ptHeader->hAsyncData);
        ptHeader->eDataState = 2;
        SOUNDCHAN_PrepareWaveBuffer(ptHeader);
        --gnSoundAsyncLoadsInFlight;
        return;
    }

    if (ptHeader->eDataState)
        return;

    SOUNDHDR_BuildSoundPath(nSoundId, szFilename);

    if (!FOG_FOpenFile(szFilename, &hFile))
    {
        ptHeader->bLoadFailed = 1;
        return;
    }
    nFileSize = FOG_FGetFileSize(hFile, 0);
    FOG_FCloseFile(hFile);

    if (gnSoundCacheUsed + nFileSize > gnSoundCacheBudget)
    {
        nHeadroom = gnSoundCacheBudget - nFileSize;
        dwCursor = gnSoundPreloadCursor;
        if (gnSoundCacheUsed > nHeadroom)
        {
            do
            {
                bEvicted = FALSE;
                for (nPass = 0; nPass <= 1; ++nPass)
                {
                    for (pNode = gSoundDataList; pNode; pNode = pNode->pNext)
                    {
                        nCandidateId = pNode->nValue;
                        //  BUG: each eviction candidate's row is read untested (mov edx, [eax+8Ah], 0x6FB55013). LATENT: every id on the list was
                        //  pushed by a load of a row that existed.
                        pCandidate = SOUNDHDR_GetRecord((D2C_Sounds)nCandidateId);
                        if (!nPass && (pCandidate->dwRefCount || pCandidate->bCache))
                            continue;
                        if (bServicePass && dwCursor - pCandidate->nCacheStamp < 750)
                            continue;
                        if (SOUNDCHAN_FindOldestPlaying((D2C_Sounds)nCandidateId))
                            continue;
                        SOUNDHDR_FreeSoundData((D2C_Sounds)nCandidateId);
                        bEvicted = TRUE;
                        break;
                    }
                    if (!bForceLoad)
                        break;
                }
                if (!bEvicted)
                {
                    gdwSoundLastLoadTick = dwCursorAtEntry;
                    return;
                }
            } while (gnSoundCacheUsed > nHeadroom);
        }
    }

    if (bForceLoad)
    {
        ptHeader->pWaveData = ARCHIVE_LoadFile(D2WIN_GetArchive(), szFilename, &ptHeader->nWaveSize, szSoundHdrCpp, 769);
        if (!ptHeader->pWaveData || !ptHeader->nWaveSize)
        {
            FOG_DisplayAssert("ptHeader->pbWaveData && ptHeader->dwWaveSize", szSoundHdrCpp, 770);
            exit(-1);
        }
        gnSoundCacheUsed += ptHeader->nWaveSize;
        SOUNDHDR_TouchCacheEntry(nSoundId);
        ptHeader->eDataState = 2;
        SOUNDCHAN_PrepareWaveBuffer(ptHeader);
        return;
    }

    ptHeader->hAsyncData = FOG_AsyncDataLoadFileEx(0, szFilename, 1, 0, -1, 0, 0, -2, szSoundHdrCpp, 795);
    if (!ptHeader->hAsyncData)
        return;
    ptHeader->nWaveSize = FOG_AsyncDataGetBufferSize(ptHeader->hAsyncData);
    gnSoundCacheUsed += ptHeader->nWaveSize;
    SOUNDHDR_TouchCacheEntry(nSoundId);
    ptHeader->eDataState = 1;
    ++gnSoundAsyncLoadsInFlight;
}


//1.10f: D2Client.0x6FB55230
//  The distance a sound plays at full volume, by its sounds.txt Falloff (dwFalloff, +47h): 2 -> 200, 3 -> 400, 4 -> 2000, anything else -> 60.
//  SOUNDCHAN_ApplyVolumeAndPosition fades the volume linearly from this distance to SOUNDHDR_GetFalloffDistance's. The answer is in st0 (fld of a
//  float constant: 200.0, 400.0, 2000.0, 60.0 in .rdata). Falloff 0 and 1 share the default here; GetFalloffDistance gives 0 a case of its own.
//  The original inlines SOUNDHDR_GetRecord here.
double __fastcall SOUNDHDR_GetRangeParam(D2C_Sounds nSoundId)
{
    //  BUG: an unknown id reads +47h off NULL: the inlined SOUNDHDR_GetRecord's NULL is read through (xor eax, eax at 0x6FB55252, mov eax, [eax+47h]
    //  at 0x6FB55254). LATENT.
    switch (SOUNDHDR_GetRecord(nSoundId)->dwFalloff)
    {
    case 2:
        return 200.0f;
    case 3:
        return 400.0f;
    case 4:
        return 2000.0f;
    default:
        return 60.0f;
    }
}


//1.10f: D2Client.0x6FB552A0
//  The distance at which a sound fades out, by the same Falloff: 0 -> 400, 2 -> 1000, 3 -> 1500, 4 -> 2000, anything else (1 among them) -> 700,
//  each a fld of a float constant. Only falloff 4 gives one number in both functions, 2000, so ApplyVolumeAndPosition's fade,
//  (falloff - distance) * volume / (falloff - range), divides by a zero span for a falloff-4 sound past 2000 (SoundChan.cpp takes the 0 without
//  dividing). Such a sound reaches the mixer on an axis: SOUNDINST_ServiceInstances' falloff test reads a distance taken from the position held to
//  +/-2000. The original inlines SOUNDHDR_GetRecord here.
double __fastcall SOUNDHDR_GetFalloffDistance(D2C_Sounds nSoundId)
{
    //  BUG: the same NULL read for an unknown id (xor eax, eax at 0x6FB552C2, mov eax, [eax+47h] at 0x6FB552C4). LATENT.
    switch (SOUNDHDR_GetRecord(nSoundId)->dwFalloff)
    {
    case 0:
        return 400.0f;
    case 2:
        //  gfConst1000 is a .rdata float (fld dword ptr, 0x6FB552DA); the other four arms fld other .rdata floats, written as literals here.
        return gfConst1000;
    case 3:
        return 1500.0f;
    case 4:
        return 2000.0f;
    default:
        return 700.0f;
    }
}


//1.10f: D2Client.0x6FB55310
//  A sound of the id's group, picked at random, avoiding the ones played last. With no table the id comes back, and with a group of 0 or 1
//  (nGroupSize, +3Dh, unsigned) too. Otherwise the id plus SOUNDSONG_RollRandom(nGroupSize) (the player's seed stepped and its low dword taken
//  modulo the size, or and with the size less one for a power of two, so 0 .. size - 1), drawn again while it equals one of the record's
//  nRecentPicks, the ids SOUNDCHAN_StartInstance last played from this record (+64h the latest, +68h the one before). How many are avoided: the
//  size less one, at most two; and for a group of 2 or 3 one fewer on a roll of 0 in size + 1, so a group of 2 avoids nothing one time in three
//  and a group of 3 only the latest one time in four. The draw is over the whole group; two is the number of recent picks compared.
//  The answer is a sound id, D2C_Sounds. The original inlines SOUNDHDR_GetRecord here.
D2C_Sounds __fastcall SOUNDHDR_PickRandomInGroup(D2C_Sounds nSoundId)
{
    D2SoundsTxt *ptHeader;
    D2C_Sounds nPicked;
    int32_t nRecentToAvoid;
    int32_t nIndex;

    if (!gpSoundHeaders)
        return nSoundId;

    //  BUG: an unknown id with the table present reads +3Dh off NULL (xor ebx, ebx at 0x6FB5533B, mov cl, [ebx+3Dh] at 0x6FB5533D). LATENT.
    ptHeader = SOUNDHDR_GetRecord(nSoundId);
    if (ptHeader->nGroupSize <= 1)
        return nSoundId;

    nRecentToAvoid = ptHeader->nGroupSize - 1;
    if (nRecentToAvoid >= 2)
        nRecentToAvoid = 2;
    if (ptHeader->nGroupSize <= 3 && !SOUNDSONG_RollRandom(ptHeader->nGroupSize + 1))
        --nRecentToAvoid;

    for (;;)
    {
        nPicked = (D2C_Sounds)(nSoundId + SOUNDSONG_RollRandom(ptHeader->nGroupSize));
        nIndex = 0;
        if (nRecentToAvoid <= 0)
            break;
        while (nPicked != ptHeader->nRecentPicks[nIndex])
        {
            if (++nIndex >= nRecentToAvoid)
                return nPicked;
        }
    }
    return nPicked;
}


//1.10f: D2Client.0x6FB553A0
//  Sets gnSoundCacheBudget. Called by MEMPART_InitMemoryPartitions with a tenth of the usable memory, held between 3 and 5 MB. The parameter is
//  unsigned: LoadWaveData compares the budget unsigned (jbe).
void __fastcall SOUNDHDR_SetCacheBudget(uint32_t nBytes)
{
    gnSoundCacheBudget = nBytes;
}


//1.10f: D2Client.0x6FB553B0
//  A sound's path: DATA\LOCAL for ids 2934..4656 (SOUND_AMAZON_CANTCARRY_1 to SOUND_MONSTER_NIHLATHAK_TAUNT_4, speech by the two names) and
//  DATA\GLOBAL otherwise (signed compares), then \MUSIC\ for 4657..4698 (SOUND_MUSIC_CAVES to SOUND_MUSIC_QUEST_SHENK) and \SFX\ otherwise, then
//  the record's file name. The assert "strlen(szFilename) + strlen(ptHeader->szFilename) < MAX_SOUND_PATH" (SoundHdr.cpp:939) bounds the two at 80
//  (cmp esi, 50h / jb). "\SFX\" is copied as a dword and a word and "\MUSIC\" as two dwords. The original inlines SOUNDHDR_GetRecord here.
void __fastcall SOUNDHDR_BuildSoundPath(D2C_Sounds nSoundId, char *pszPath)
{
    D2SoundsTxt *ptHeader;

    //  BUG: an unknown id measures its name off NULL: the inlined SOUNDHDR_GetRecord's NULL arm (xor ebp, ebp at 0x6FB553D6) is handed to the
    //  repne scasb at 0x6FB5545F, which reads from address 0. LATENT.
    ptHeader = SOUNDHDR_GetRecord(nSoundId);

    if ((int32_t)nSoundId < (int32_t)SOUND_AMAZON_CANTCARRY_1 || (int32_t)nSoundId > (int32_t)SOUND_MONSTER_NIHLATHAK_TAUNT_4)
        strcpy(pszPath, "DATA\\GLOBAL");
    else
        strcpy(pszPath, "DATA\\LOCAL");

    if ((int32_t)nSoundId < (int32_t)SOUND_MUSIC_CAVES || (int32_t)nSoundId > (int32_t)SOUND_MUSIC_QUEST_SHENK)
        strcat(pszPath, "\\SFX\\");
    else
        strcat(pszPath, "\\MUSIC\\");

    if (strlen(pszPath) + strlen(ptHeader->szFileName) >= 80)
    {
        FOG_DisplayAssert("strlen(szFilename) + strlen(ptHeader->szFilename) < MAX_SOUND_PATH", szSoundHdrCpp, 939);
        exit(-1);
    }
    strcat(pszPath, ptHeader->szFileName);
}


//1.10f: D2Client.0x6FB554D0
//  A cached sound touched: its row stamped with gnSoundPreloadCursor and its id moved to the tail of gSoundDataList (taken off with D2Sound #10059
//  and put back at the end with #10063, a tail jump), so LoadWaveData's eviction, walking from the head, reaches it last. The original inlines
//  SOUNDHDR_GetRecord here.
void __fastcall SOUNDHDR_TouchCacheEntry(D2C_Sounds nSoundId)
{
    //  BUG: an unknown id writes the stamp off NULL (xor eax, eax at 0x6FB554F5, mov [eax+7Ch], edx at 0x6FB55502). LATENT.
    SOUNDHDR_GetRecord(nSoundId)->nCacheStamp = gnSoundPreloadCursor;
    D2SOUND_10059_ListRemove(&gSoundDataList, nSoundId);
    D2SOUND_10063_ListPushBack(&gSoundDataList, nSoundId);
}

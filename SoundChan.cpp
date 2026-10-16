#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "SoundInst.h"
#include "SoundChan.h"

//  SOUNDCHAN_PrepareWaveBuffer reads a WAVEFORMATEX. The SDK declares it in mmeapi.h, which only mmsystem.h includes, and DLLmain.h defines
//  WIN32_LEAN_AND_MEAN, so windows.h does not pull mmsystem.h in. Nothing here calls winmm, so no library.
#include <mmsystem.h>

//  SoundChan.cpp -- the sound channels of D2Client.dll 1.10: the sixteen channels, a sound instance started on a free channel of the kind its
//  record wants (or on the weakest one of that kind), its volume, pan, 3D position and reverb mixed down every time something moved, the channels
//  stopped, detached, created and shut down, 3D sound and the EAX effects toggled, and the EAX listener set from a soundenviron.txt row.
//  The original source file is Sound\SoundChan.cpp (the asserts at lines 86, 248 and 537 name it). Address range 0x6FB52330..0x6FB537F1.
//
//  The channel walks end at the address of the next global, compared signed: a cursor over the channels stops at gnSoundChanReentryGuard, one
//  over their nKind at sgfSoundChannelsActive, and one over their pInstance members at gnMoveToUnitGuidOrMask -- sixteen records each way.
//  Ours compares the record pointer with gaSoundChannels + 16, int-cast for the signed compare (SoundChan.h).
//
//  A channel's kind (nKind): bit 0 the record is stereo; bit 1 a 3D channel (D2Sound's mode 1); bits 1 and 2 a 3D channel with EAX (mode 2).
//  A channel's tCache holds what it last sent the driver -- the position, the mixed volume and the reverb mix -- so an unchanged instance costs
//  nothing.

static const char szSoundChanCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\Sound\\SoundChan.cpp";

static_assert(offsetof(D2SoundChannelStrc, hSound) == 0x00 && offsetof(D2SoundChannelStrc, nKind) == 0x04
              && offsetof(D2SoundChannelStrc, pInstance) == 0x08 && offsetof(D2SoundChannelStrc, tCache) == 0x0C
              && sizeof(D2SoundChannelStrc) == 0x20,
              "the channel: `mov ecx, [esi]`, `cmp [esi+4], ebx`, `mov eax, [esi+8]`, `lea ecx, [esi+0Ch]`, `add esi, 20h` "
              "(SOUNDCHAN_StartInstance 0x6FB525FC, 0x6FB525F7, 0x6FB525F0, 0x6FB5271E, 0x6FB5260B)");
static_assert(offsetof(D2SoundChannelCacheStrc, fPosX) == 0x00 && offsetof(D2SoundChannelCacheStrc, nVolume) == 0x0C
              && offsetof(D2SoundChannelCacheStrc, fReverbMix) == 0x10,
              "the channel's cache at +0Ch: `mov [ebx+0Ch], eax`, `mov [ebx+18h], ebp`, `mov [ebx+1Ch], ecx` "
              "(SOUNDCHAN_ApplyVolumeAndPosition 0x6FB52CCB, 0x6FB52CD7, 0x6FB52CE0)");
static_assert(offsetof(D2SoundInstanceStrc, nSoundId) == 0x04 && offsetof(D2SoundInstanceStrc, fPosX) == 0x0C
              && offsetof(D2SoundInstanceStrc, nBaseVolume) == 0x1C && offsetof(D2SoundInstanceStrc, fReverbMix) == 0x20
              && offsetof(D2SoundInstanceStrc, nStartBlock) == 0x24 && offsetof(D2SoundInstanceStrc, ptChannel) == 0x2C
              && offsetof(D2SoundInstanceStrc, eState) == 0x30 && offsetof(D2SoundInstanceStrc, dwStartTick) == 0x34
              && offsetof(D2SoundInstanceStrc, nPlayFlags) == 0x38 && offsetof(D2SoundInstanceStrc, nPriority) == 0x3C
              && offsetof(D2SoundInstanceStrc, bStarted) == 0x3D && offsetof(D2SoundInstanceStrc, nSavedStreamPos) == 0x41,
              "the instance, packed: [esi+4], [edi+0Ch], [edi+1Ch], [edi+20h], [edi+24h], [eax+2Ch], [eax+30h], [eax+34h], "
              "[esi+38h], [eax+3Ch], [eax+3Dh] (a dword), [edx+41h] (a dword) -- SOUNDCHAN_StartInstance and ApplyVolumeAndPosition");
static_assert(offsetof(D2SoundsTxt, nVolume) == 0x3C && offsetof(D2SoundsTxt, bLoop) == 0x3E
              && offsetof(D2SoundsTxt, nReverb) == 0x4B && offsetof(D2SoundsTxt, bAsyncOnly) == 0x4D
              && offsetof(D2SoundsTxt, bStream) == 0x4F && offsetof(D2SoundsTxt, bStereo) == 0x50
              && offsetof(D2SoundsTxt, bSolo) == 0x52 && offsetof(D2SoundsTxt, nMusicVol) == 0x53
              && offsetof(D2SoundsTxt, nBlock1) == 0x54 && offsetof(D2SoundsTxt, nRecentPicks) == 0x64
              && sizeof(((D2SoundsTxt *)0)->nRecentPicks) == 8 && offsetof(D2SoundsTxt, pWaveData) == 0x6C
              && offsetof(D2SoundsTxt, nWaveSize) == 0x70 && offsetof(D2SoundsTxt, pPcmData) == 0x74
              && offsetof(D2SoundsTxt, nPcmSize) == 0x78 && offsetof(D2SoundsTxt, nBlockCount) == 0x80
              && offsetof(D2SoundsTxt, bLoadFailed) == 0x81 && offsetof(D2SoundsTxt, eDataState) == 0x86,
              "the sounds.txt record, packed: +3Ch +3Eh +4Bh +4Dh +4Fh +50h +52h +53h +54h +64h +68h +6Ch +70h +74h +78h +80h "
              "+81h +86h, as SOUNDCHAN_PrepareWaveBuffer, StartInstance and ApplyVolumeAndPosition read them");
static_assert(offsetof(D2SoundEnvironTxt, nEaxEnviron) == 0x24 && offsetof(D2SoundEnvironTxt, nEaxEnvSize) == 0x28
              && offsetof(D2SoundEnvironTxt, nEaxAirAbsorb) == 0x54,
              "soundenviron.txt's EAX columns, +24h to +54h: SOUNDCHAN_ApplyEaxEnvironment 0x6FB52400, 0x6FB523FA, 0x6FB5247C");
static_assert(sizeof(WAVEFORMATEX) == 18 && offsetof(WAVEFORMATEX, nChannels) == 2 && offsetof(WAVEFORMATEX, nSamplesPerSec) == 4
              && offsetof(WAVEFORMATEX, wBitsPerSample) == 14,
              "SOUNDCHAN_PrepareWaveBuffer's format block at [esp+8]: `cmp word ptr [esp+1Ch+pWaveFormatEx+2], 2`, "
              "`cmp [esp+1Ch+var_10], 5622h`, `cmp [esp+1Ch+var_6], 10h` (0x6FB52388, 0x6FB5237E, 0x6FB52376)");
static_assert(SOUND_CURSOR_PASS == 1 && SOUND_CURSOR_REPAIR_ITEM == 15 && SOUND_SCENE_CATACOMBS == 52
              && SOUND_SCENE_WILDERNESS_NIGHT == 71 && SOUND_MUSIC_CAVES == 4657 && SOUND_MUSIC_QUEST_SHENK == 4698,
              "the pause fade's three exempt ranges: `cmp ecx, 1231h` .. `cmp ecx, 0Fh` (0x6FB52A5D..0x6FB52A7C)");

//  The probe's fold: the low word kept, the high word replaced by the two words xored. The same expression as UnitMode.cpp's
//  UNITMODE_PROBE_OBFUSCATE, which is file-local there.
#define SOUNDCHAN_FOLD(n) (((n) & 0xFFFF) | ((((uint32_t)(n) >> 16) ^ ((n) & 0xFFFF)) << 16))

//  The pause fade's exempt ids: the music, the scenes and the cursor sounds -- three signed range tests. A helper of ours; the original tests
//  the three ranges in line.
static inline BOOL SOUNDCHAN_IsFadeExempt(int32_t nSoundId)
{
    if (nSoundId >= (int32_t)SOUND_MUSIC_CAVES && nSoundId <= (int32_t)SOUND_MUSIC_QUEST_SHENK)
        return TRUE;
    if (nSoundId >= (int32_t)SOUND_SCENE_CATACOMBS && nSoundId <= (int32_t)SOUND_SCENE_WILDERNESS_NIGHT)
        return TRUE;
    if (nSoundId >= (int32_t)SOUND_CURSOR_PASS && nSoundId <= (int32_t)SOUND_CURSOR_REPAIR_ITEM)
        return TRUE;
    return FALSE;
}

//  A float's bits as the int an import is declared to take: D2SOUND_10017_SetSample3dPosition takes three int coordinates, and the original
//  pushes the stored floats as dwords. A helper of ours.
static inline int32_t SOUNDCHAN_FloatBits(float fValue)
{
    int32_t nBits;

    memcpy(&nBits, &fValue, sizeof(nBits));
    return nBits;
}


//1.10f: D2Client.0x6FB52330
//  Parses a loaded sound's wave data and keeps it only at 16 bits and 22050 Hz. The record must be loaded (assert
//  "ptHeader->eDataState == ESOUNDDATASTATE_LOADED", SoundChan.cpp:86), and D2Sound #10056 parses its RIFF image (pWaveData, nWaveSize) into a PCM
//  pointer, a byte count and a WAVEFORMATEX. A parse that succeeds with wBitsPerSample 16 and nSamplesPerSec 22050 stores the PCM, its size and
//  bStereo = nChannels == 2 and answers 1; anything else sets bLoadFailed (+81h) and answers 0.
int32_t __fastcall SOUNDCHAN_PrepareWaveBuffer(D2SoundsTxt *ptHeader)
{
    uint32_t dwPcmSize;
    WAVEFORMATEX tFormat;
    void *pPcmData;

    if (ptHeader->eDataState != 2)
    {
        FOG_DisplayAssert((uint32_t)"ptHeader->eDataState == ESOUNDDATASTATE_LOADED", (uint32_t)szSoundChanCpp, 86);
        exit(-1);
    }

    dwPcmSize = ptHeader->nWaveSize;
    pPcmData = D2SOUND_10056_ParseWaveData(ptHeader->pWaveData, &dwPcmSize, &tFormat);
    if (pPcmData && tFormat.wBitsPerSample == 16 && tFormat.nSamplesPerSec == 22050)
    {
        ptHeader->pPcmData = pPcmData;
        ptHeader->nPcmSize = dwPcmSize;
        ptHeader->bStereo = tFormat.nChannels == 2;
        return 1;
    }

    ptHeader->bLoadFailed = 1;
    return 0;
}


//1.10f: D2Client.0x6FB523C0
//  Pushes a soundenviron.txt row at the EAX listener, once per change of environment: an id equal to gnCurrentEaxEnvironment returns at once.
//  Otherwise the row is fetched (SOUNDHDR_GetEnvRecord, always), and only in D2Sound's mode 2 or for environment 0, and only when #10004 allows it,
//  the row's thirteen EAX columns go to #10006 as thirteen dwords: five copied as they are (Environ, Room Vol, Room HF, Reflect, Reverb), eight
//  converted and scaled by gfEaxMilliScale, 0.001f. The id is stored whether or not the driver was reached.
//  #10006 is declared SetListenerPosition in D2Imports.h; it actually hands the block to the EAX listener property set (IKsPropertySet::Set).
//  The product: fild loads the int exactly and fmul takes the float constant, so the x87 product rounded at 53 bits and stored as a float is
//  the double product rounded to float here.
void __fastcall SOUNDCHAN_ApplyEaxEnvironment(int32_t nEnvironmentId)
{
    D2SoundEnvironTxt *pEnvRecord;
    float afProperties[13];

    if (nEnvironmentId == gnCurrentEaxEnvironment)
        return;

    pEnvRecord = SOUNDHDR_GetEnvRecord(nEnvironmentId);
    if ((D2SOUND_10044_GetSoundMixerSetting() == 2 || !nEnvironmentId) && D2SOUND_10004_Is3dListenerAllowed())
    {
        *(int32_t *)&afProperties[0] = pEnvRecord->nEaxEnviron;
        afProperties[1] = (float)((double)pEnvRecord->nEaxEnvSize * gfEaxMilliScale);
        afProperties[2] = (float)((double)pEnvRecord->nEaxEnvDiff * gfEaxMilliScale);
        *(int32_t *)&afProperties[3] = pEnvRecord->nEaxRoomVol;
        *(int32_t *)&afProperties[4] = pEnvRecord->nEaxRoomHF;
        afProperties[5] = (float)((double)pEnvRecord->nEaxDecayTime * gfEaxMilliScale);
        afProperties[6] = (float)((double)pEnvRecord->nEaxDecayHF * gfEaxMilliScale);
        *(int32_t *)&afProperties[7] = pEnvRecord->nEaxReflect;
        afProperties[8] = (float)((double)pEnvRecord->nEaxReflectDelay * gfEaxMilliScale);
        *(int32_t *)&afProperties[9] = pEnvRecord->nEaxReverb;
        afProperties[10] = (float)((double)pEnvRecord->nEaxRevDelay * gfEaxMilliScale);
        afProperties[11] = (float)((double)pEnvRecord->nEaxRoomRoll * gfEaxMilliScale);
        afProperties[12] = (float)((double)pEnvRecord->nEaxAirAbsorb * gfEaxMilliScale);
        D2SOUND_10006_SetListenerPosition(afProperties);
    }

    gnCurrentEaxEnvironment = nEnvironmentId;
}


//1.10f: D2Client.0x6FB524A0
//  Starts a sound instance on a channel, or refuses (NULL); the one caller stores the answer in the instance's ptChannel. In order:
//  * The record of the id asked for is kept; unless bit 0 of the instance's nPlayFlags (a dword, tested here by its low byte) is set, the id is
//    re-rolled within its group (SOUNDHDR_PickRandomInGroup) and written back.
//  * The channel already playing the same id with the oldest dwStartTick is found (SOUNDCHAN_FindOldestPlaying); if its instance has not started
//    (+3Dh, a dword), started less than two ticks before this one (unsigned) and has a priority at least this one's (unsigned bytes), the answer
//    is NULL -- two copies of one sound do not start on the same tick.
//  * The ASKED record's two last-played slots shift (nRecentPicks[2], which SOUNDHDR_PickRandomInGroup avoids); SOUNDCHAN_SendCodeIntegrityProbe
//    runs.
//  * A record that is not streamed and not loaded is loaded (SOUNDHDR_LoadWaveData, bForceLoad = !bAsyncOnly); still not loaded, or
//    bLoadFailed: NULL.
//  * The kind is bStereo, and for a mono record 2 in mode 1, 6 in mode 2. A free channel of exactly that kind whose sample is not playing is
//    taken; failing one, the channel of that kind holding the instance SOUNDINST_ComparePriority ranks lowest (distance and age counted), if this
//    instance outranks it (neither counted) -- that channel stopped and detached first. None: NULL.
//  * The sample reset; for a sound not streamed, its loop flag (#10019), for a one-block loop the loop start (nBlock1 * 2), its wave data
//    (#10010 -- failing: NULL, the channel not yet taken).
//  * The channel takes the instance; on an EAX channel the sample's EAX level is 1.0 for a record with Reverb, 0.0 without; the cache is cleared
//    with the volume at -1, so the mix below is sent whatever it is.
//  * Played -- #10011 for a sound in memory, #10025 streamed from its path (SOUNDHDR_BuildSoundPath into an 80-byte buffer) from nStartBlock. A
//    play that fails gives the channel up and answers NULL; else the channel.
//  #10025's second parameter is int nFileId in D2Imports.h; the original hands it the path buffer -- the cast below is that.
//  The original inlines SOUNDCHAN_FindOldestPlaying and SOUNDCHAN_StopAndDetach here.
D2SoundChannelStrc *__fastcall SOUNDCHAN_StartInstance(D2SoundInstanceStrc *pSoundInstance)
{
    D2SoundsTxt *pAskedRecord;
    D2SoundsTxt *pRecord;
    D2SoundChannelStrc *pChannel;
    D2SoundChannelStrc *pCandidate;
    D2SoundChannelStrc *pDuplicate;
    D2SoundInstanceStrc *pOther;
    uint32_t nKind;
    float fEaxLevel;
    char szPath[80];

    pAskedRecord = SOUNDHDR_GetRecord(pSoundInstance->nSoundId);
    if (!(pSoundInstance->nPlayFlags & 1))
        pSoundInstance->nSoundId = SOUNDHDR_PickRandomInGroup(pSoundInstance->nSoundId);

    pDuplicate = SOUNDCHAN_FindOldestPlaying(pSoundInstance->nSoundId);
    if (pDuplicate)
    {
        pOther = pDuplicate->pInstance;
        if (!pOther->bStarted && pSoundInstance->dwStartTick - pOther->dwStartTick < 2 && pOther->nPriority >= pSoundInstance->nPriority)
            return NULL;
    }

    pRecord = SOUNDHDR_GetRecord(pSoundInstance->nSoundId);
    pAskedRecord->nRecentPicks[1] = pAskedRecord->nRecentPicks[0];
    pAskedRecord->nRecentPicks[0] = pSoundInstance->nSoundId;
    SOUNDCHAN_SendCodeIntegrityProbe();

    if (!pRecord->bStream && pRecord->eDataState != 2)
    {
        SOUNDHDR_LoadWaveData(pSoundInstance->nSoundId, pRecord->bAsyncOnly == 0, 0);
        if (pRecord->eDataState != 2)
            return NULL;
    }

    if (pRecord->bLoadFailed)
        return NULL;

    nKind = pRecord->bStereo ? 1 : 0;
    if (!(nKind & 1))
    {
        if (D2SOUND_10044_GetSoundMixerSetting() == 1)
            nKind |= 2;
        if (D2SOUND_10044_GetSoundMixerSetting() == 2)
            nKind |= 6;
    }

    for (pChannel = gaSoundChannels; (int32_t)pChannel < (int32_t)(gaSoundChannels + 16); ++pChannel)
    {
        if (!pChannel->pInstance && pChannel->nKind == (int32_t)nKind && !D2SOUND_10013_IsSampleStarted(pChannel->hSound))
            goto have_channel;
    }

    pChannel = NULL;
    for (pCandidate = gaSoundChannels; (int32_t)pCandidate < (int32_t)(gaSoundChannels + 16); ++pCandidate)
    {
        if (pCandidate->nKind != (int32_t)nKind || !pCandidate->pInstance)
            continue;

        if (!pChannel || SOUNDINST_ComparePriority(pChannel->pInstance, pCandidate->pInstance, 1, 1))
            pChannel = pCandidate;
    }

    if (!pChannel || !SOUNDINST_ComparePriority(pSoundInstance, pChannel->pInstance, 0, 0))
        return NULL;

    SOUNDCHAN_StopAndDetach(pChannel);

have_channel:
    if (!pChannel)
        return NULL;

    D2SOUND_10009_ResetSample(pChannel->hSound);
    if (!pRecord->bStream)
    {
        D2SOUND_10019(pChannel->hSound, pRecord->bLoop);
        if (pRecord->nBlockCount == 1)
            D2SOUND_10020_SetSampleLoopBlockStart(pChannel->hSound, pRecord->nBlock1 * 2);
        if (!D2SOUND_10010_SetSampleWaveData(pChannel->hSound, pRecord->pWaveData, pRecord->nWaveSize))
            return NULL;
    }

    pChannel->pInstance = pSoundInstance;
    if (nKind & 4)
    {
        fEaxLevel = 1.0f;
        if (!pRecord->nReverb)
            fEaxLevel = 0.0f;
        D2SOUND_10022_SetSampleEaxLevel(pChannel->hSound, fEaxLevel);
    }

    pChannel->tCache.nVolume = -1;
    pChannel->tCache.fReverbMix = 0.0f;
    pChannel->tCache.fPosX = 0.0f;
    pChannel->tCache.fPosY = 0.0f;
    pChannel->tCache.fPosZ = 0.0f;
    SOUNDCHAN_ApplyVolumeAndPosition(pChannel);

    if (!pRecord->bStream)
    {
        if (!D2SOUND_10011_PlaySampleLooping(pChannel->hSound))
        {
            pChannel->pInstance = NULL;
            return NULL;
        }
    }
    else
    {
        SOUNDHDR_BuildSoundPath(pSoundInstance->nSoundId, szPath);
        if (!D2SOUND_10025_PlaySampleStream(pChannel->hSound, (int32_t)szPath, pSoundInstance->nStartBlock, pRecord->bLoop))
        {
            pChannel->pInstance = NULL;
            return NULL;
        }
    }

    return pChannel;
}


//1.10f: D2Client.0x6FB527A0
//  The code-integrity probe, triggered by the player's clicks. When bit 14 of gnMoveToUnitGuidOrMask is set -- the global UNITMODE_StartMoveToUnit
//  ORs every target GUID into -- the global is cleared, and in a Battle.net game (gnGameType GAMETYPE_BNET_BETA or GAMETYPE_BNET) the first dword
//  at four function pointers is read: the two D2Net exports #10039 and #10040 return, and the two D2Win exports #10206 and #10207. Each pair is
//  read inside __try, and a read that faults gives 0x12345678 and 0x90ABCDEF for the pair. The four go to the server folded (SOUNDCHAN_FOLD) as
//  client packet 0x65 through MSG_SendCmd4Dwords. The one caller is SOUNDCHAN_StartInstance, so a probe waits for the next sound to start.
//  Here the four pointers are D2Net's and D2Win's own, which this DLL does not replace, so the server reads what it would read from the original.
void __cdecl SOUNDCHAN_SendCodeIntegrityProbe()
{
    uint32_t nNet1;   //  D2NET_10039's
    uint32_t nNet2;   //  D2NET_10040's
    uint32_t nWin1;   //  D2WIN_10206's
    uint32_t nWin2;   //  D2WIN_10207's

    if (!(gnMoveToUnitGuidOrMask & 0x4000))
        return;

    gnMoveToUnitGuidOrMask = 0;
    if (gnGameType != GAMETYPE_BNET_BETA && gnGameType != GAMETYPE_BNET)
        return;

    __try
    {
        nNet1 = *(uint32_t *)D2NET_10039();
        nNet2 = *(uint32_t *)D2NET_10040();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        nNet1 = 0x12345678;
        nNet2 = 0x90ABCDEF;
    }

    __try
    {
        nWin1 = *(uint32_t *)D2WIN_10206();
        nWin2 = *(uint32_t *)D2WIN_10207();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        nWin1 = 0x12345678;
        nWin2 = 0x90ABCDEF;
    }

    MSG_SendCmd4Dwords(0x65, (int32_t)SOUNDCHAN_FOLD(nNet1), (int32_t)SOUNDCHAN_FOLD(nNet2), (int32_t)SOUNDCHAN_FOLD(nWin1), (int32_t)SOUNDCHAN_FOLD(nWin2));
}


//1.10f: D2Client.0x6FB528D0
//  Lets a channel's instance go, the sample left as it is: the instance's eState set to 2 and its ptChannel cleared, then the channel's
//  pInstance. Nothing for a channel with no instance.
void __fastcall SOUNDCHAN_DetachInstance(D2SoundChannelStrc *pChannel)
{
    if (!pChannel->pInstance)
        return;

    pChannel->pInstance->eState = 2;
    pChannel->pInstance->ptChannel = NULL;
    pChannel->pInstance = NULL;
}


//1.10f: D2Client.0x6FB528F0
//  Stops a channel and lets its instance go: with a sample, the instance's stream position is saved first (#10018 into nSavedStreamPos, +41h, a
//  dword) so a streamed sound can resume, then the sample is stopped (#10012); then the detach. The original inlines SOUNDCHAN_DetachInstance here.
void __fastcall SOUNDCHAN_StopAndDetach(D2SoundChannelStrc *pChannel)
{
    if (pChannel->hSound)
    {
        if (pChannel->pInstance)
            pChannel->pInstance->nSavedStreamPos = D2SOUND_10018_GetSampleStreamPosition(pChannel->hSound);
        D2SOUND_10012_StopSample(pChannel->hSound);
    }

    SOUNDCHAN_DetachInstance(pChannel);
}


//1.10f: D2Client.0x6FB52940
//  Stops and detaches every channel, with the master volume at 0 for the sweep and 0FFh after it (#10027). Nothing without a DirectSound
//  object (#10002). The original inlines SOUNDCHAN_StopAndDetach here.
void __cdecl SOUNDCHAN_StopAllAndDetach()
{
    D2SoundChannelStrc *pChannel;

    if (!D2SOUND_10002_GetDirectSound())
        return;

    D2SOUND_10027_ApplyMasterVolume(0);
    for (pChannel = gaSoundChannels; (int32_t)pChannel < (int32_t)(gaSoundChannels + 16); ++pChannel)
    {
        SOUNDCHAN_StopAndDetach(pChannel);
    }

    D2SOUND_10027_ApplyMasterVolume(0xFF);
}


//1.10f: D2Client.0x6FB529B0
//  Sends one channel's mix to the driver. The channel must hold an instance (assert "ptChannel->ptInst", SoundChan.cpp:537). From the
//  instance's record:
//
//    the base volume (nBaseVolume, +1Ch)
//      x the music volume / 100, for a record with MusicVol
//      x the master volume / 100
//      x gnSoundPauseFadePercent / 100 while it is not 100, for every id but the music (SOUND_MUSIC_CAVES..SOUND_MUSIC_QUEST_SHENK), the
//        scenes (SOUND_SCENE_CATACOMBS..SOUND_SCENE_WILDERNESS_NIGHT) and the cursor sounds (SOUND_CURSOR_PASS..SOUND_CURSOR_REPAIR_ITEM)
//      x gnSoundDuckPercent / 100 while it is not 100, for a record without Solo
//      x with the channels active, in mode 1 or 2 and a positional bias not 50: bias' = (2 * bias - 100) / 3; for a positive bias and a
//        sample, (255 - 255 * bias' / 50) / 255; for a negative bias on a 3D channel, (255 * bias' / 50 + 255) / 255
//
//  is the mixed volume. Every division is signed: the products are imul and divided as ints.
//
//  If the mixed volume, the reverb mix (a float compare) and the position (three dwords, compared bit for bit) all equal the channel's cache,
//  nothing is sent. Otherwise the distance from the listener (SOUNDSONG_Distance of x and y) attenuates linearly from the record's range
//  (SOUNDHDR_GetRangeParam) to its falloff distance (SOUNDHDR_GetFalloffDistance) when it is past the range; the record's Volume (+3Ch) / 255
//  scales it; the position is scaled by gfSoundPositionScale, 1/320. A channel of kind 0 takes its pan and a volume factor from #10057 (the
//  factor from the distance of the three coordinates on a decibel curve, the pan from x); a 3D channel takes the scaled position (#10017); every
//  channel its reverb mix and volume; the cache takes the position, the mixed volume -- before the distance -- and the reverb mix.
//
//  The floats: the range and the falloff are stored as floats; the distance is used as SOUNDSONG_Distance leaves it (a double); the attenuation
//  is ((falloff - distance) * volume) / (falloff - range), truncated to 64 bits and taken low, which is (int)(__int64) here; under 53-bit
//  precision control that is the double arithmetic below. The three positions are a float times a float rounded once to float, the same in SSE2.
//
//  #10057 writes both its outputs and reads neither. #10017 is __thiscall with three stack arguments, written as __fastcall with an unused second
//  parameter (D2Imports.h); the original leaves the scaled z in edx, which nothing reads, and ours passes 0. Its three coordinates are floats in
//  D2Sound and int in D2Imports.h, so the floats' bits are passed.
void __fastcall SOUNDCHAN_ApplyVolumeAndPosition(D2SoundChannelStrc *ptChannel)
{
    D2SoundInstanceStrc *pInstance;
    D2SoundsTxt *pRecord;
    float fRange;
    float fFalloff;
    float fX;
    float fY;
    float fZ;
    double dDistance;
    int32_t nVolume;
    int32_t nMixed;
    int32_t nSoundId;
    int32_t nMixerSetting;
    int32_t nBias;
    int32_t nAttenuated;
    int32_t nFinal;
    int32_t nPositionalVolume;
    int32_t nPositionalPan;

    pInstance = ptChannel->pInstance;
    if (!pInstance)
    {
        FOG_DisplayAssert((uint32_t)"ptChannel->ptInst", (uint32_t)szSoundChanCpp, 537);
        exit(-1);
    }

    pRecord = SOUNDHDR_GetRecord(pInstance->nSoundId);
    fRange = (float)SOUNDHDR_GetRangeParam(pInstance->nSoundId);
    fFalloff = (float)SOUNDHDR_GetFalloffDistance(pInstance->nSoundId);

    nVolume = pInstance->nBaseVolume;
    if (pRecord->nMusicVol)
        nVolume = D2SOUND_10048_GetMusicVolumeSetting() * nVolume / 100;
    nMixed = D2SOUND_10046_GetMasterVolumeSetting() * nVolume / 100;

    if (gnSoundPauseFadePercent != 100)
    {
        nSoundId = pInstance->nSoundId;
        if (!SOUNDCHAN_IsFadeExempt(nSoundId))
            nMixed = gnSoundPauseFadePercent * nMixed / 100;
    }

    if (gnSoundDuckPercent != 100 && !pRecord->bSolo)
        nMixed = gnSoundDuckPercent * nMixed / 100;

    if (sgfSoundChannelsActive)
    {
        nMixerSetting = D2SOUND_10044_GetSoundMixerSetting();
        if ((nMixerSetting == 1 || nMixerSetting == 2) && D2SOUND_10050_GetPositionalBiasSetting() != 50)
        {
            nBias = (2 * D2SOUND_10050_GetPositionalBiasSetting() - 100) / 3;
            if (ptChannel->hSound && nBias > 0)
                nMixed = (255 - 255 * nBias / 50) * nMixed / 255;
            if ((ptChannel->nKind & 2) && nBias < 0)
                nMixed = (255 * nBias / 50 + 255) * nMixed / 255;
        }
    }

    if (nMixed == ptChannel->tCache.nVolume && pInstance->fReverbMix == ptChannel->tCache.fReverbMix)
    {
        if (!memcmp(&pInstance->fPosX, &ptChannel->tCache.fPosX, 3 * sizeof(float)))
            return;
    }

    nAttenuated = nMixed;
    dDistance = SOUNDSONG_Distance(pInstance->fPosX, pInstance->fPosY);
    if (dDistance > (double)fRange)
    {
        //  BUG: a Falloff 4 row has range and falloff both 2000, and SOUNDINST_ServiceInstances lets such a sound reach here past 2000 on an axis
        //  (its falloff test reads a distance taken from the position held to +/-2000, this one the position's own), so the original divides by
        //  a zero span (fdivp, 0x6FB52BF8) and __ftol (0x6FB60A20) stores the infinity or NaN as the integer indefinite, low dword 0.
        //  NOT ESTABLISHED: the value is the fade's own 0.
        //  DIFFERENCE: C++ leaves that division and that conversion undefined, so the C takes the 0 without dividing -- the original's value for
        //  every operand, by a test the original does not make.
        if ((double)fFalloff == (double)fRange)
            nAttenuated = 0;
        else
            nAttenuated = (int32_t)(int64_t)(((double)fFalloff - dDistance) * (double)nMixed / ((double)fFalloff - (double)fRange));
    }

    nFinal = (int32_t)pRecord->nVolume * nAttenuated / 255;
    fX = pInstance->fPosX * gfSoundPositionScale;
    fY = pInstance->fPosY * gfSoundPositionScale;
    fZ = pInstance->fPosZ * gfSoundPositionScale;

    if (!ptChannel->nKind)
    {
        D2SOUND_10057_ComputeVolumeAndPan(&nPositionalVolume, &nPositionalPan, fX, fY, fZ);
        D2SOUND_10016_SetSamplePan(ptChannel->hSound, nPositionalPan);
        nFinal = nPositionalVolume * nFinal / 255;
    }

    if (ptChannel->nKind & 2)
        D2SOUND_10017_SetSample3dPosition(ptChannel->hSound, 0, SOUNDCHAN_FloatBits(fX), SOUNDCHAN_FloatBits(fY), SOUNDCHAN_FloatBits(fZ));

    D2SOUND_10021_SetSampleReverbMix(ptChannel->hSound, pInstance->fReverbMix);
    D2SOUND_10014_SetSampleVolume(ptChannel->hSound, nFinal);

    ptChannel->tCache.fPosX = pInstance->fPosX;
    ptChannel->tCache.fPosY = pInstance->fPosY;
    ptChannel->tCache.nVolume = nMixed;
    ptChannel->tCache.fPosZ = pInstance->fPosZ;
    ptChannel->tCache.fReverbMix = pInstance->fReverbMix;
}


//1.10f: D2Client.0x6FB52CF0
//  Returns whether the channels are up (sgfSoundChannelsActive).
int32_t __cdecl SOUNDCHAN_IsSystemReady()
{
    return sgfSoundChannelsActive;
}


//1.10f: D2Client.0x6FB52D00
//  The same six bytes again, sixteen bytes on: a second definition of SOUNDCHAN_IsSystemReady that the linker did not fold.
int32_t __cdecl SOUNDCHAN_IsSystemReady_2()
{
    return sgfSoundChannelsActive;
}


//1.10f: D2Client.0x6FB52D10
//  Lets the finished channels go, from SOUNDHDR_PumpPreload. Each channel holding an instance whose record does not loop (bLoop, +3Eh) and whose
//  sample is not playing (#10013) is let go: a streamed sample is stopped and detached, its position saved first; any other is only detached.
//  A looping sound is never reaped here. The original inlines SOUNDCHAN_StopAndDetach and SOUNDCHAN_DetachInstance here.
//  BUG: SOUNDHDR_GetRecord answers NULL for an id outside sounds.txt, and bLoop is read with nothing between: a fault at 3Eh.
//  SOUNDCHAN_StartInstance and SOUNDCHAN_ApplyVolumeAndPosition read their records the same way. LATENT: an instance's id is
//  SOUNDINST_PlaySound's, which reads that id's record untested before it makes the instance, or a pick inside the id's group, and no stock
//  group runs past the table.
void __cdecl SOUNDCHAN_ReapFinished()
{
    D2SoundChannelStrc *pChannel;

    for (pChannel = gaSoundChannels; (int32_t)pChannel < (int32_t)(gaSoundChannels + 16); ++pChannel)
    {
        if (!pChannel->pInstance)
            continue;
        if (SOUNDHDR_GetRecord(pChannel->pInstance->nSoundId)->bLoop)
            continue;
        if (D2SOUND_10013_IsSampleStarted(pChannel->hSound))
            continue;

        if (D2SOUND_10023_IsSampleStreaming(pChannel->hSound))
        {
            SOUNDCHAN_StopAndDetach(pChannel);
        }
        else
        {
            SOUNDCHAN_DetachInstance(pChannel);
        }
    }
}


//1.10f: D2Client.0x6FB52DA0
//  Returns a channel's stream position: its sample handed to #10018. The parameter is the channel, its hSound at +0.
uint32_t __fastcall SOUNDCHAN_GetStreamPosition(D2SoundChannelStrc *pChannel)
{
    return D2SOUND_10018_GetSampleStreamPosition(pChannel->hSound);
}


//1.10f: D2Client.0x6FB52DB0
//  Brings the channels up in D2Sound's mode, or in mode 0. The one caller is SOUNDHDR_Init. The mode is #10044's; mode 1 needs #10003 and
//  mode 2 needs #10004 to allow it. Allowed (or any other mode): the channels are shut down and created in that mode, and on success the mode
//  is written back if #10044 now answers another. Not allowed, or the creation failed: shut down again and created in mode 0 -- every channel
//  but the first four of kind 0 -- and on success the mode is set to 0 unless it is.
//  The original inlines SOUNDCHAN_ShutdownSystem twice and SOUNDCHAN_CreateChannels(0) once here.
void __cdecl SOUNDCHAN_CreateSystem()
{
    int32_t nMode;

    nMode = D2SOUND_10044_GetSoundMixerSetting();
    if (nMode == 1)
    {
        if (!D2SOUND_10003_Is3dSampleAllowed())
            goto mode_zero;
    }
    else if (nMode == 2)
    {
        if (!D2SOUND_10004_Is3dListenerAllowed())
            goto mode_zero;
    }

    SOUNDCHAN_ShutdownSystem();
    if (SOUNDCHAN_CreateChannels(nMode))
    {
        if (D2SOUND_10044_GetSoundMixerSetting() != nMode)
            D2SOUND_10045_SetSoundMixerSetting(nMode);
        return;
    }

mode_zero:
    SOUNDCHAN_ShutdownSystem();

    if (SOUNDCHAN_CreateChannels(0))
    {
        if (D2SOUND_10044_GetSoundMixerSetting())
            D2SOUND_10045_SetSoundMixerSetting(0);
    }
}


//1.10f: D2Client.0x6FB52F90
//  The shutdown, defined a second time: the same body as SOUNDCHAN_ShutdownSystem, named after its twin. Entered by SOUNDCHAN_CreateSystem's
//  mode-0 creation, and by Toggle3dSound and ToggleEaxEffects on their creation's failure. The original inlines SOUNDCHAN_StopAllAndDetach here.
void __cdecl SOUNDCHAN_ShutdownSystem_2()
{
    D2SoundChannelStrc *pChannel;

    if (!sgfSoundChannelsActive)
        return;

    SOUNDCHAN_StopAllAndDetach();

    for (pChannel = gaSoundChannels; (int32_t)pChannel < (int32_t)(gaSoundChannels + 16); ++pChannel)
    {
        D2SOUND_10008_FreeSample(pChannel->hSound);
        pChannel->hSound = NULL;
    }

    sgfSoundChannelsActive = 0;
}


//1.10f: D2Client.0x6FB53030
//  Creates the sixteen channels, one D2Sound sample each (#10007 with the channel's kind). No DirectSound object: FALSE at once. The channels must
//  not be up (assert "!sgfSoundChannelsActive", SoundChan.cpp:248). The first four channels are kind 1 whatever the mode; the rest 2 in mode 1,
//  6 in mode 2, 0 otherwise -- the kind stored 0 first and then its value. All sixteen made: soundenviron row 0 at the EAX listener, the channels
//  up, TRUE. A sample that cannot be made: everything shut down again, FALSE.
//  #10007 takes the kind in cl and masks it bit by bit, so the char D2Imports.h declares carries all of it.
//  The original inlines SOUNDCHAN_ShutdownSystem here.
BOOL __fastcall SOUNDCHAN_CreateChannels(int32_t nKindMode)
{
    D2SoundChannelStrc *pChannel;

    if (!D2SOUND_10002_GetDirectSound())
        return FALSE;

    if (sgfSoundChannelsActive)
    {
        FOG_DisplayAssert((uint32_t)"!sgfSoundChannelsActive", (uint32_t)szSoundChanCpp, 248);
        exit(-1);
    }

    for (pChannel = gaSoundChannels; (int32_t)pChannel < (int32_t)(gaSoundChannels + 16); ++pChannel)
    {
        pChannel->nKind = 0;
        if ((int32_t)&pChannel->nKind < (int32_t)&gaSoundChannels[4].nKind)
            pChannel->nKind = 1;
        else if (nKindMode == 1)
            pChannel->nKind = 2;
        else
            pChannel->nKind = nKindMode == 2 ? 6 : 0;

        pChannel->hSound = (void *)D2SOUND_10007_AllocSample((char)pChannel->nKind);
        if (!pChannel->hSound)
        {
            SOUNDCHAN_ShutdownSystem();
            return FALSE;
        }
    }

    SOUNDCHAN_ApplyEaxEnvironment(0);
    sgfSoundChannelsActive = 1;
    return TRUE;
}


//1.10f: D2Client.0x6FB53170
//  Shuts the channels down: with the channels up, every channel is stopped and detached under a muted master volume, every sample freed (#10008)
//  and its handle cleared, and the channels marked down. The one caller is SOUNDHDR_Shutdown. The original inlines SOUNDCHAN_StopAllAndDetach here.
void __cdecl SOUNDCHAN_ShutdownSystem()
{
    D2SoundChannelStrc *pChannel;

    if (!sgfSoundChannelsActive)
        return;

    SOUNDCHAN_StopAllAndDetach();

    for (pChannel = gaSoundChannels; (int32_t)pChannel < (int32_t)(gaSoundChannels + 16); ++pChannel)
    {
        D2SOUND_10008_FreeSample(pChannel->hSound);
        pChannel->hSound = NULL;
    }

    sgfSoundChannelsActive = 0;
}


//1.10f: D2Client.0x6FB53210
//  Returns #10003 when the channels are up, 0 otherwise -- a tail jmp to it. The ESC menu's 3D Sound row reads it.
int32_t __cdecl SOUNDCHAN_Get3DEnabled()
{
    if (!sgfSoundChannelsActive)
        return 0;

    return D2SOUND_10003_Is3dSampleAllowed();
}


//1.10f: D2Client.0x6FB53230
//  Returns #10004 when the channels are up, 0 otherwise: SOUNDCHAN_Get3DEnabled with the other ordinal, for the EAX row.
int32_t __cdecl SOUNDCHAN_GetEaxEnabled()
{
    if (!sgfSoundChannelsActive)
        return 0;

    return D2SOUND_10004_Is3dListenerAllowed();
}


//1.10f: D2Client.0x6FB53250
//  Whether 3D sound is on: the channels up and D2Sound in mode 1 or 2.
BOOL __cdecl SOUNDCHAN_Is3DEnabled()
{
    int32_t nMode;

    if (!sgfSoundChannelsActive)
        return FALSE;

    nMode = D2SOUND_10044_GetSoundMixerSetting();
    return nMode == 1 || nMode == 2;
}


//1.10f: D2Client.0x6FB53280
//  Whether the EAX effects are on: the channels up and D2Sound in mode 2.
BOOL __cdecl SOUNDCHAN_IsEaxEnabled()
{
    if (!sgfSoundChannelsActive)
        return FALSE;

    return D2SOUND_10044_GetSoundMixerSetting() == 2;
}


//1.10f: D2Client.0x6FB532A0
//  Handles a change of the ESC menu's 3D Sound row: the channels are rebuilt in the other of mode 0 and mode 1, falling back to the mode they
//  were in. Nothing while the channels are down or a rebuild is running (gnSoundChanReentryGuard, InterlockedIncrement and -Decrement around the
//  whole); the master volume is faded out over 180 ms first (#10029) and set to 0FFh after. The new mode is old == 0. An attempt in a mode is:
//  that mode allowed (#10003 for 1, #10004 for 2), the channels shut down and created in it, the setting written back if #10044 answers another.
//  The first attempt that works answers 1; the fallback answers 0 whether or not it worked.
//  The attempt has no out-of-line body: the same allowed-shutdown-create-reconcile sequence stands twice here, twice in ToggleEaxEffects and twice
//  in CreateSystem, written in place each time. The new mode is 0 or 1, so the first attempt's == 2 arm cannot hold; it is written as the
//  original tests it.
//  The original inlines SOUNDCHAN_ShutdownSystem twice and SOUNDCHAN_CreateChannels once here.
int32_t __cdecl SOUNDCHAN_Toggle3dSound()
{
    int32_t bRebuilt;
    int32_t nOldMode;
    int32_t nNewMode;

    bRebuilt = 0;
    if (!sgfSoundChannelsActive || gnSoundChanReentryGuard)
        return 0;

    InterlockedIncrement(&gnSoundChanReentryGuard);
    nOldMode = D2SOUND_10044_GetSoundMixerSetting();
    nNewMode = nOldMode == 0;
    D2SOUND_10029_FadeOutMasterVolume(180);

    if (nNewMode == 1)
    {
        if (!D2SOUND_10003_Is3dSampleAllowed())
            goto old_mode;
    }
    else if (nNewMode == 2)
    {
        if (!D2SOUND_10004_Is3dListenerAllowed())
            goto old_mode;
    }

    SOUNDCHAN_ShutdownSystem();
    if (SOUNDCHAN_CreateChannels(nNewMode))
    {
        if (D2SOUND_10044_GetSoundMixerSetting() != nNewMode)
            D2SOUND_10045_SetSoundMixerSetting(nNewMode);
        bRebuilt = 1;
        goto done;
    }

old_mode:
    if (nOldMode == 1)
    {
        if (!D2SOUND_10003_Is3dSampleAllowed())
            goto done;
    }
    else if (nOldMode == 2)
    {
        if (!D2SOUND_10004_Is3dListenerAllowed())
            goto done;
    }

    SOUNDCHAN_ShutdownSystem();

    if (SOUNDCHAN_CreateChannels(nOldMode))
    {
        if (D2SOUND_10044_GetSoundMixerSetting() != nOldMode)
            D2SOUND_10045_SetSoundMixerSetting(nOldMode);
    }

done:
    D2SOUND_10027_ApplyMasterVolume(0xFF);
    InterlockedDecrement(&gnSoundChanReentryGuard);
    return bRebuilt;
}


//1.10f: D2Client.0x6FB53510
//  Handles a change of the ESC menu's EAX row: SOUNDCHAN_Toggle3dSound with another new mode -- 2 unless the old one is 2, then 1 -- and
//  otherwise the same sequence. The original inlines SOUNDCHAN_ShutdownSystem twice and SOUNDCHAN_CreateChannels once here.
int32_t __cdecl SOUNDCHAN_ToggleEaxEffects()
{
    int32_t bRebuilt;
    int32_t nOldMode;
    int32_t nNewMode;

    bRebuilt = 0;
    if (!sgfSoundChannelsActive || gnSoundChanReentryGuard)
        return 0;

    InterlockedIncrement(&gnSoundChanReentryGuard);
    nOldMode = D2SOUND_10044_GetSoundMixerSetting();
    nNewMode = (nOldMode != 2) + 1;
    D2SOUND_10029_FadeOutMasterVolume(180);

    if (nNewMode == 1)
    {
        if (!D2SOUND_10003_Is3dSampleAllowed())
            goto old_mode;
    }
    else if (nNewMode == 2)
    {
        if (!D2SOUND_10004_Is3dListenerAllowed())
            goto old_mode;
    }

    SOUNDCHAN_ShutdownSystem();
    if (SOUNDCHAN_CreateChannels(nNewMode))
    {
        if (D2SOUND_10044_GetSoundMixerSetting() != nNewMode)
            D2SOUND_10045_SetSoundMixerSetting(nNewMode);
        bRebuilt = 1;
        goto done;
    }

old_mode:
    if (nOldMode == 1)
    {
        if (!D2SOUND_10003_Is3dSampleAllowed())
            goto done;
    }
    else if (nOldMode == 2)
    {
        if (!D2SOUND_10004_Is3dListenerAllowed())
            goto done;
    }

    SOUNDCHAN_ShutdownSystem();

    if (SOUNDCHAN_CreateChannels(nOldMode))
    {
        if (D2SOUND_10044_GetSoundMixerSetting() != nOldMode)
            D2SOUND_10045_SetSoundMixerSetting(nOldMode);
    }

done:
    D2SOUND_10027_ApplyMasterVolume(0xFF);
    InterlockedDecrement(&gnSoundChanReentryGuard);
    return bRebuilt;
}


//1.10f: D2Client.0x6FB53780
//  Returns the channel playing a sound id that started first: among the channels whose sample is playing (#10013) and whose instance has the id,
//  the one with the smallest dwStartTick (unsigned); NULL with the channels down or none found. It returns the record's base (the cursor less
//  eight). The original's name for it is SoundChannelFind (SoundHdr.cpp's assert "!SoundChannelFind(eSound)").
D2SoundChannelStrc *__fastcall SOUNDCHAN_FindOldestPlaying(D2C_Sounds nSoundId)
{
    D2SoundChannelStrc *pChannel;
    D2SoundChannelStrc *pOldest;
    D2SoundInstanceStrc *pInstance;

    if (!sgfSoundChannelsActive)
        return NULL;

    pOldest = NULL;
    for (pChannel = gaSoundChannels; (int32_t)pChannel < (int32_t)(gaSoundChannels + 16); ++pChannel)
    {
        if (!D2SOUND_10013_IsSampleStarted(pChannel->hSound))
            continue;

        pInstance = pChannel->pInstance;
        if (!pInstance || pInstance->nSoundId != nSoundId)
            continue;

        if (!pOldest || pInstance->dwStartTick < pOldest->pInstance->dwStartTick)
            pOldest = pChannel;
    }

    return pOldest;
}


//1.10f: D2Client.0x6FB537E0
//  Whether a channel's sample buffer is playing: #10064 for a channel with a sample, 0 for none or no sample -- a tail jmp. The parameter is the
//  channel, its sample at +0.
int32_t __fastcall SOUNDCHAN_IsPlaybackObjectActive(D2SoundChannelStrc *pChannel)
{
    if (!pChannel || !pChannel->hSound)
        return 0;

    return D2SOUND_10064_IsSampleBufferPlaying(pChannel->hSound);
}

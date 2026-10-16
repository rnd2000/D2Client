#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "SoundSong.h"

#include <math.h>
#include <string.h>

//  SoundSong.cpp -- the music of the sound area: the song of the player's sound environment, chosen once a pump and started, held while a
//  sting plays, and resumed from a block boundary of its sounds.txt row; the stings themselves; and three small families of helpers the other
//  sound units call -- rolls on the player's own seed, the two distances, a distance on a ring.
//  No assert in the unit names its source file, so the file name is ours. Address range 0x6FB56D40..0x6FB57325. Its data is fourteen dwords
//  past .data's raw size, zero at load, at 0x6FBC1A6C..0x6FBC1AA0 (SoundSong.h).
//  The song table, SOUNDHDR_GetEnvironLookup(), is one dword per song id, from SOUNDHDR_GetEnvironLookupMin to Max, indexed by the id less the
//  minimum: the block a song starts from when it is started again (PlaySound's nStartBlock). Init zeroes it, UpdateMusic keeps the block
//  boundary ahead of the song's play position in it, and ScheduleSting sets it from the row before a sting takes the music over.


//1.10f: D2Client.0x6FB56D40
//  Clears the music state: gnLevelChangeTick, gdwSoundMusicTickUnused, gnCurrentSongId, gnSongLevelId, gnSongChangeTick, gbStingActive and
//  gnAreaEntryQuestLineLevelId are set to 0 and the song table is zeroed, 4 * (max - min + 1) bytes (a memset; the size is taken in the order
//  minimum, maximum, table). The sting's other seven globals are left as they are: UpdateMusic and ScheduleSting2 read them only while
//  gbStingActive is set, and ScheduleSting writes all seven when it sets it. void: both callers discard eax.
void __stdcall SOUNDSONG_Init()
{
    int32_t nLookupCount;

    gnLevelChangeTick = 0;
    gdwSoundMusicTickUnused = 0;
    gnCurrentSongId = SOUND_NONE;
    gnSongLevelId = 0;
    gnSongChangeTick = 0;
    gbStingActive = 0;
    gnAreaEntryQuestLineLevelId = 0;

    nLookupCount = 1 - SOUNDHDR_GetEnvironLookupMin();
    nLookupCount += SOUNDHDR_GetEnvironLookupMax();
    memset(SOUNDHDR_GetEnvironLookup(), 0, (uint32_t)nLookupCount * 4);
}


//1.10f: D2Client.0x6FB56DA0
//  The music, once a pump: SOUNDHDR_PumpPreload's one call, nTick the preload cursor (gnSoundPreloadCursor), the clock
//  SOUNDINST_ServiceInstances counts in too. Nothing while the player is on no level (SOUNDHDR_GetPlayerLevelId 0). In order:
//  * a new level noted: its id kept (gnSongLevelId) and the tick it changed (gnLevelChangeTick);
//  * the wanted song: the current sound environment row's nSong, 0 with no row or outside SOUNDHDR_GetEnvironLookupMin..Max (signed compares);
//  * the switch: the playing song (gnCurrentSongId) becomes the wanted one, and the tick is noted (gnSongChangeTick), when they differ and no
//    song plays, or the sound is off (D2Sound's master or music volume 0), or 75 ticks have passed since the level changed (unsigned compare)
//    -- so crossing into a level does not switch the music at once;
//  * the music released (SOUNDINST_MarkAllEnvironStarted) while the sound is off, and again while no song is chosen;
//  * the sting (gbStingActive, ScheduleSting's): its first sound played on no unit once the frame count reaches its frame, its second on the
//    player once its frame comes, unless the player is dead (CUNIT_IsUnitDead); and while the frame count is short of the sting's end the
//    function RETURNS here. At the end the sting is cleared and, with the sound on, its first sound released (SOUNDINST_StopSoundsById).
//    The frame count is compared unsigned in all three tests;
//  * the start: a chosen song with no instance (SOUNDINST_GetHandleForSoundId 0), the sound on, is started on no unit from the block the song
//    table keeps for it, after the music still playing is released -- nPlayFlags 2 (bit 1, the start without the row's fade-in) when no music
//    is live and the block is 0, and 0 otherwise; the tick goes into gdwSoundMusicTickUnused, which nothing reads;
//  * the area's quest line (UNITSND_PlayAreaEntryQuestLine) played once a level, 62 ticks after the level changed (unsigned compare);
//  * the resume block: more than 125 ticks after the song last changed, its instance active, the play position (SOUNDINST_GetPlayPosition)
//    mapped to the block boundary ahead of it -- Block 1 from 0, Block 2 from Block 1, Block 3 from Block 2, three separate signed tests in
//    that order, and 0 before 0 or from Block 3 on -- and kept in the song table, the tick noted again. A song stopped and started again
//    starts from the boundary after the last one it had reached.
//  BUG: the sounds.txt row is read untested (`mov ecx, [edi+54h]`, 0x6FB56FE9, after SOUNDHDR_GetRecord at 0x6FB56FB9). LATENT: the row is read
//  off ids that are bounded below only.
//  BUG: the player is not tested before CUNIT_IsUnitDead (0x6FB56EC8), which reads its dwFlags without a test. LATENT.
void __fastcall SOUNDSONG_UpdateMusic(uint32_t nTick)
{
    int32_t nLevelId;
    D2C_Sounds nWantedSong;
    D2SoundEnvironTxt *pEnvRecord;
    BOOL bSoundEnabled;
    BOOL bMaySwitch;
    D2UnitStrc *pPlayer;
    int32_t nSlot;
    int32_t nStartBlock;
    int32_t nPlayFlags;
    D2SoundsTxt *pRecord;
    int32_t hSong;
    int32_t nPosition;
    int32_t nResumeBlock;

    nLevelId = SOUNDHDR_GetPlayerLevelId();
    if (!nLevelId)
        return;
    if (nLevelId != gnSongLevelId)
    {
        gnSongLevelId = nLevelId;
        gnLevelChangeTick = nTick;
    }

    nWantedSong = SOUND_NONE;
    pEnvRecord = SOUNDHDR_GetEnvRecord(SOUNDHDR_GetCurrentSoundEnvironId());
    if (pEnvRecord)
    {
        nWantedSong = pEnvRecord->nSong;
        if (nWantedSong && (nWantedSong < SOUNDHDR_GetEnvironLookupMin() || nWantedSong > SOUNDHDR_GetEnvironLookupMax()))
            nWantedSong = SOUND_NONE;
    }

    bSoundEnabled = D2SOUND_10046_GetMasterVolumeSetting() && D2SOUND_10048_GetMusicVolumeSetting();
    bMaySwitch = !gnCurrentSongId;
    if (!bSoundEnabled)
        bMaySwitch = TRUE;
    if (nTick - gnLevelChangeTick >= 75)
        bMaySwitch = TRUE;
    if (gnCurrentSongId != nWantedSong && bMaySwitch)
    {
        gnCurrentSongId = nWantedSong;
        gnSongChangeTick = nTick;
    }

    if (!bSoundEnabled)
        SOUNDINST_MarkAllEnvironStarted();
    if (!gnCurrentSongId)
        SOUNDINST_MarkAllEnvironStarted();

    if (gbStingActive)
    {
        if (gbSting1Pending && GAME_GetFrameCount() >= gnSting1Frame)
        {
            SOUNDINST_PlaySound(gnSting1SoundId, NULL, 0, 0, 0);
            gbSting1Pending = 0;
        }
        if (gbSting2Pending && GAME_GetFrameCount() >= gnSting2Frame)
        {
            pPlayer = CUNIT_GetPlayer();
            if (!CUNIT_IsUnitDead(pPlayer))
                SOUNDINST_PlaySound(gnSting2SoundId, pPlayer, 0, 0, 0);
            gbSting2Pending = 0;
        }
        if (GAME_GetFrameCount() < gnStingEndFrame)
            return;
        gbStingActive = 0;
        if (bSoundEnabled)
            SOUNDINST_StopSoundsById(gnSting1SoundId);
    }

    if (gnCurrentSongId && bSoundEnabled && !SOUNDINST_GetHandleForSoundId(gnCurrentSongId))
    {
        nSlot = gnCurrentSongId - SOUNDHDR_GetEnvironLookupMin();
        nStartBlock = SOUNDHDR_GetEnvironLookup()[nSlot];
        if (SOUNDINST_IsMusicPlaying() || nStartBlock)
            nPlayFlags = 0;
        else
            nPlayFlags = 2;
        SOUNDINST_MarkAllEnvironStarted();
        SOUNDINST_PlaySound(gnCurrentSongId, NULL, 0, nPlayFlags, nStartBlock);
        gdwSoundMusicTickUnused = nTick;
    }

    if (gnSongLevelId != gnAreaEntryQuestLineLevelId && nTick - gnLevelChangeTick >= 62)
    {
        UNITSND_PlayAreaEntryQuestLine();
        gnAreaEntryQuestLineLevelId = gnSongLevelId;
    }

    if (!gnCurrentSongId || nTick <= gnSongChangeTick + 125)
        return;
    pRecord = SOUNDHDR_GetRecord(gnCurrentSongId);
    hSong = SOUNDINST_GetHandleForSoundId(gnCurrentSongId);
    if (!hSong || !SOUNDINST_IsInstanceActive(hSong))
        return;

    nPosition = SOUNDINST_GetPlayPosition(hSong);
    nResumeBlock = 0;
    if (nPosition >= 0 && nPosition < pRecord->nBlock1)
        nResumeBlock = pRecord->nBlock1;
    if (nPosition >= pRecord->nBlock1 && nPosition < pRecord->nBlock2)
        nResumeBlock = pRecord->nBlock2;
    if (nPosition >= pRecord->nBlock2 && nPosition < pRecord->nBlock3)
        nResumeBlock = pRecord->nBlock3;
    nSlot = gnCurrentSongId - SOUNDHDR_GetEnvironLookupMin();
    SOUNDHDR_GetEnvironLookup()[nSlot] = nResumeBlock;
    gnSongChangeTick = nTick;
}


//1.10f: D2Client.0x6FB57030
//  Schedules a sting: first, when a song plays (its instance active), the song table's slot for it is set to the row's Block nSongStateIndex
//  (Block 1 at +54h) or to 0 for an index of 0, and the tick noted (gnSongChangeTick, the preload cursor). Then the music is released
//  (SOUNDINST_MarkAllEnvironStarted) and the sting set: nSound1 at the frame count plus nDelay1, nSound2 at the frame count plus nDelay2 and
//  pending only when bPlaySound2 is set, its end at the frame count plus nDuration -- GAME_GetFrameCount asked three times, as the bytes ask it.
//  UpdateMusic plays the two and holds the song until the end.
//  __fastcall with five arguments on the stack (`retn 14h`). The index is 0 or 1 at every call, so no other block is read; the slot is
//  addressed as the bytes address it, from Block 1, which the C writes from the member's offset.
//  The return is the end frame, eax as the bytes leave it. No caller was found that reads it; the `int` is kept because the tail-call chain
//  was not followed to every end.
//  BUG: the sounds.txt row is read untested (`mov esi, [esi+eax*4+50h]`, 0x6FB5706A, after SOUNDHDR_GetRecord at 0x6FB57041). LATENT: the row is
//  read off an id that is bounded below only.
int32_t __fastcall SOUNDSONG_ScheduleSting(D2C_Sounds nSound1, int32_t nDelay1, int32_t nDuration, int32_t nSongStateIndex, D2C_Sounds nSound2, int32_t nDelay2, int32_t bPlaySound2)
{
    D2SoundsTxt *pRecord;
    int32_t hSong;
    int32_t nResumeBlock;
    int32_t nSlot;
    int32_t nFrame;

    if (gnCurrentSongId)
    {
        pRecord = SOUNDHDR_GetRecord(gnCurrentSongId);
        hSong = SOUNDINST_GetHandleForSoundId(gnCurrentSongId);
        if (hSong && SOUNDINST_IsInstanceActive(hSong))
        {
            if (nSongStateIndex)
                nResumeBlock = *(int32_t *)((char *)pRecord + offsetof(D2SoundsTxt, nBlock1) + sizeof(int32_t) * (nSongStateIndex - 1));
            else
                nResumeBlock = 0;
            nSlot = gnCurrentSongId - SOUNDHDR_GetEnvironLookupMin();
            SOUNDHDR_GetEnvironLookup()[nSlot] = nResumeBlock;
            gnSongChangeTick = SOUNDHDR_GetPreloadCursor();
        }
    }

    SOUNDINST_MarkAllEnvironStarted();
    gnSting1SoundId = nSound1;
    gbStingActive = 1;
    nFrame = GAME_GetFrameCount();
    gbSting1Pending = 1;
    gnSting1Frame = nFrame + nDelay1;
    gnSting2SoundId = nSound2;
    nFrame = GAME_GetFrameCount();
    gbSting2Pending = bPlaySound2;
    gnSting2Frame = nFrame + nDelay2;
    nFrame = GAME_GetFrameCount() + nDuration;
    gnStingEndFrame = nFrame;
    return nFrame;
}


//1.10f: D2Client.0x6FB57100
//  Arms or disarms the second sting, while a sting is active and only then: with nPending set, pending at the frame count plus nDelayFrames;
//  either way the sting's end is 50 frames after the second sound's frame -- disarming keeps that tail rather than cutting it. nPending is
//  stored whole in both arms. void: nothing reads the end frame the original leaves in eax.
void __fastcall SOUNDSONG_ScheduleSting2(int32_t nDelayFrames, int32_t nPending)
{
    int32_t nFrame;

    if (!gbStingActive)
        return;
    if (nPending)
    {
        nFrame = GAME_GetFrameCount() + nDelayFrames;
        gbSting2Pending = nPending;
        gnSting2Frame = nFrame;
        gnStingEndFrame = nFrame + 50;
        return;
    }
    gbSting2Pending = nPending;
    gnStingEndFrame = gnSting2Frame + 50;
}


//1.10f: D2Client.0x6FB57150
//  A roll in [0, nRange) on the player's seed, 0 when nRange is not positive. The original inlines SEED_RollLimitedRandomNumber here (the
//  seed is at +20h of CUNIT_GetPlayer()'s unit, its arithmetic folded into the exits); the seed's address is taken before the test
//  (`add ecx, 20h`) and read only past it. The answer is the body's int, in [0, nRange), returned as unsigned.
//  BUG: the player is not tested (CUNIT_GetPlayer, 0x6FB57155): with no player a roll of a positive range reads the seed at 20h. NOT
//  ESTABLISHED. The same in RollRandomInRange and RandomOffset.
uint32_t __fastcall SOUNDSONG_RollRandom(int32_t nRange)
{
    D2UnitStrc *pPlayer;

    pPlayer = CUNIT_GetPlayer();
    return SEED_RollLimitedRandomNumber(&pPlayer->pSeed, nRange);
}


//1.10f: D2Client.0x6FB571B0
//  A roll in [nMin, nMax] on the player's seed: nMin plus a roll over the span nMax - nMin + 1, and nMin itself for an empty span. The original
//  inlines SEED_RollLimitedRandomNumber here.
int32_t __fastcall SOUNDSONG_RollRandomInRange(int32_t nMin, int32_t nMax)
{
    int32_t nSpan;
    D2UnitStrc *pPlayer;

    nSpan = nMax - nMin + 1;
    pPlayer = CUNIT_GetPlayer();
    return nMin + SEED_RollLimitedRandomNumber(&pPlayer->pSeed, nSpan);
}


//1.10f: D2Client.0x6FB57220
//  A roll in [-nRange, nRange] on the player's seed: a roll over 2 * nRange + 1 less nRange, and -nRange for a span that is not positive. The
//  original inlines SEED_RollLimitedRandomNumber here; the span is odd for every nRange, so its power-of-two arm runs only for a span of 1,
//  where it masks with 0.
int32_t __fastcall SOUNDSONG_RandomOffset(int32_t nRange)
{
    int32_t nSpan;
    D2UnitStrc *pPlayer;

    nSpan = 2 * nRange + 1;
    pPlayer = CUNIT_GetPlayer();
    return SEED_RollLimitedRandomNumber(&pPlayer->pSeed, nSpan) - nRange;
}


//1.10f: D2Client.0x6FB57290
//  The distance of (x, y) from the origin, sqrt(x * x + y * y), returned in st0 (`retn 8`). The products are exact in a double as in the x87's
//  extended format; the sum and the root are rounded to double here and to extended there.
double __stdcall SOUNDSONG_Distance(float x, float y)
{
    return sqrt((double)x * x + (double)y * y);
}


//1.10f: D2Client.0x6FB572B0
//  The squared distance of (x, y) from the origin, x * x + y * y: Distance without its `fsqrt`; returned in st0.
double __stdcall SOUNDSONG_DistanceSquared(float x, float y)
{
    return (double)x * x + (double)y * y;
}


//1.10f: D2Client.0x6FB572D0
//  The distance between nA and nB on a ring of nModulus: both reduced -- masked when nModulus is a power of two or 0 (`test edi, edx / jnz`),
//  else by the signed remainder (`cdq / idiv`), which is negative for a negative operand -- and the least of three differences: |b - a|,
//  |b - a - nModulus| and |a - b - nModulus|, each made positive on its sign (`jns / neg`). The least is taken as the bytes take it, the lesser
//  of the first two worked out twice, as a min macro nested in itself would expand; written so. A modulus of 0 takes the mask arm (`-1 & 0` is
//  0) and divides by nothing. `retn 4`: nModulus on the stack.
//  BUG: the bytes' `neg` wraps where the C's `-` is undefined: a difference of -2^31. LATENT.
int32_t __fastcall SOUNDSONG_GetCircularDistance(int32_t nA, int32_t nB, int32_t nModulus)
{
    int32_t nAMod;
    int32_t nBMod;
    int32_t nDelta;
    int32_t nDirect;
    int32_t nWrapA;
    int32_t nWrapB;
    int32_t nLesser;

    if ((nModulus - 1) & nModulus)
    {
        nAMod = nA % nModulus;
        nBMod = nB % nModulus;
    }
    else
    {
        nAMod = nA & (nModulus - 1);
        nBMod = nB & (nModulus - 1);
    }

    nDelta = nBMod - nAMod;
    nDirect = nDelta < 0 ? -nDelta : nDelta;
    nWrapA = nDelta - nModulus;
    if (nWrapA < 0)
        nWrapA = -nWrapA;
    nWrapB = nAMod - nBMod - nModulus;
    if (nWrapB < 0)
        nWrapB = -nWrapB;

    nLesser = nDirect < nWrapA ? nDirect : nWrapA;
    if (nLesser < nWrapB)
        return nDirect < nWrapA ? nDirect : nWrapA;
    return nWrapB;
}

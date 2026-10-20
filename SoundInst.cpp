#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "SoundInst.h"

//  SoundInst.cpp -- the sound instances of D2Client.dll 1.10: one play of one sounds.txt row, from SOUNDINST_PlaySound until the mixer lets it go.
//  The original source file is Sound\SoundInst.cpp (the asserts in this unit name it). Address range 0x6FB55520..0x6FB56D3D.
//  The instances are a pool of 200 records of 89 bytes (gaSoundInstances, packed); the ones in use are linked on gpSoundInstances, newest first,
//  a D2Sound list (#10058 pushes at the front, #10059 takes off) whose nValue is the instance's address. Each instance is named by a handle, an
//  int counted up from gnNextSoundInstanceHandle, compared and never dereferenced.
//  Every function that takes a handle opens with the same walk: the handle tested for 0, gpSoundInstances for NULL, then the nodes until one's
//  instance carries the handle at +8. It is written out in place in each, as nothing in the bytes shows whether it was a function or a macro.

static const char szSoundInstCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\Sound\\SoundInst.cpp";

static_assert(offsetof(D2SoundInstanceStrc, hInstance) == 0x08 && offsetof(D2SoundInstanceStrc, fDistanceSquared) == 0x18
              && offsetof(D2SoundInstanceStrc, pUnitList) == 0x28 && offsetof(D2SoundInstanceStrc, bFading) == 0x45
              && offsetof(D2SoundInstanceStrc, nFadeToVolume) == 0x4D,
              "the instance: `cmp [edx+8], ecx` (0x6FB5552F), `fstp dword ptr [esi+18h]` (0x6FB5580E), `lea ebp, [esi+28h]` "
              "(0x6FB55606), `mov eax, [esi+45h]` (0x6FB55620), `mov eax, [esi+4Dh]` (0x6FB55627)");
static_assert(SOUND_OBJECT_RIVER == 2599 && SOUND_EVENT_THUNDER_1 == 202,
              "`cmp esi, 0A27h` (0x6FB55B04) and `cmp dword ptr [eax+60h], 0CAh` (0x6FB55A2B)");
static_assert(offsetof(D2SoundInstanceStrc, nFadeFromVolume) == 0x49 && offsetof(D2SoundInstanceStrc, dwFadeStartTick) == 0x51
              && offsetof(D2SoundInstanceStrc, dwFadeEndTick) == 0x55,
              "UpdateForUnit's fade: `mov [esi+49h], ecx` (0x6FB55D33), `mov [esi+51h], eax` (0x6FB55D36), "
              "`mov [esi+55h], eax` (0x6FB55D3E)");
static_assert(SOUND_EVENT_ARCANE_1 == 72 && SOUND_EVENT_WILD_NIGHT_5 == 201,
              "StopEventSounds: `cmp ecx, 48h` (0x6FB55F1A), `cmp ecx, 0C9h` (0x6FB55F1F)");


//1.10f: D2Client.0x6FB55520
//  Whether a handle's instance is in state 1: found by the walk, eState == 1 as 0 or 1; 0 for a handle of 0, an empty list or a handle not found.
int32_t __fastcall SOUNDINST_IsInstanceActive(int32_t hInstance)
{
    D2SoundListNodeStrc *pNode;
    D2SoundInstanceStrc *pInstance;

    if (!hInstance)
        return 0;
    for (pNode = gpSoundInstances; pNode; pNode = pNode->pNext)
    {
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (pInstance->hInstance == hInstance)
            return pInstance->eState == 1;
    }
    return 0;
}


//1.10f: D2Client.0x6FB55550
//  Whether a handle's instance is still sounding: found, with a channel (ptChannel, +2Ch), in state 1, and the channel's playback object active
//  (SOUNDCHAN_IsPlaybackObjectActive): 1, else 0. The original inlines SOUNDINST_IsInstanceActive here.
int32_t __fastcall SOUNDINST_IsInstanceStillPlaying(int32_t hInstance)
{
    D2SoundListNodeStrc *pNode;
    D2SoundInstanceStrc *pInstance;
    D2SoundChannelStrc *pChannel;

    if (!hInstance)
        return 0;
    for (pNode = gpSoundInstances; ; pNode = pNode->pNext)
    {
        if (!pNode)
            return 0;
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (pInstance->hInstance == hInstance)
            break;
    }

    pChannel = pInstance->ptChannel;
    if (!pChannel)
        return 0;
    if (!SOUNDINST_IsInstanceActive(hInstance))
        return 0;
    if (!SOUNDCHAN_IsPlaybackObjectActive(pChannel))
        return 0;
    return 1;
}


//1.10f: D2Client.0x6FB555C0
//  A unit lets go of an instance. The assert "hInst && hUnit" (SoundInst.cpp:206) on either missing; the handle is unlinked from the unit
//  (UNITSND_UnlinkSoundFromUnit), and the instance takes the unit off its own unit list (D2Sound #10059 on pUnitList, +28h). When no unit is
//  left on it and either bForce is set or the sound would not stop by itself (it loops and is not fading to 0), the instance is released as
//  SOUNDINST_MarkStarted releases one: bStarted set, and in state 1 with a fade-out (nFadeOut) faded to 0 over it. The original inlines
//  SOUNDINST_MarkStarted's release here.
//  BUG: the instance's sounds.txt row is read without a NULL test, twice. LATENT: an instance's id is one PlaySound accepted.
void __fastcall SOUNDINST_DetachFromUnit(int32_t hInstance, D2UnitStrc *pUnit, int32_t bForce)
{
    D2SoundListNodeStrc *pNode;
    D2SoundInstanceStrc *pInstance;
    D2SoundsTxt *pRecord;
    BOOL bStopsByItself;

    if (!hInstance || !pUnit)
    {
        FOG_DisplayAssert((uint32_t)"hInst && hUnit", (uint32_t)szSoundInstCpp, 206);
        exit(-1);
    }

    UNITSND_UnlinkSoundFromUnit(pUnit, hInstance);
    for (pNode = gpSoundInstances; ; pNode = pNode->pNext)
    {
        if (!pNode)
            return;
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (pInstance->hInstance == hInstance)
            break;
    }

    pRecord = SOUNDHDR_GetRecord(pInstance->nSoundId);
    D2SOUND_10059_ListRemove(&pInstance->pUnitList, (int32_t)(UINT_PTR)pUnit);
    bStopsByItself = !pRecord->bLoop || (pInstance->bFading && !pInstance->nFadeToVolume);
    if (pInstance->pUnitList)
        return;
    if (!bForce && bStopsByItself)
        return;

    pRecord = SOUNDHDR_GetRecord(pInstance->nSoundId);
    pInstance->bStarted = 1;
    if (pRecord->nFadeOut && pInstance->eState == 1)
        SOUNDINST_UpdateForUnit(pInstance->hInstance, 0, 0, pRecord->nFadeOut);
}


//1.10f: D2Client.0x6FB556A0
//  Releases an instance: found by its handle, bStarted set (+3Dh), and when it is in state 1 and its row fades out (nFadeOut, +40h), faded to 0
//  over that many ticks from now (SOUNDINST_UpdateForUnit(h, 0, 0, nFadeOut)). The name says "started" and the body ends a sound.
//  BUG: the row is read without a NULL test. LATENT: an instance's id is one PlaySound accepted.
void __fastcall SOUNDINST_MarkStarted(int32_t hInstance)
{
    D2SoundListNodeStrc *pNode;
    D2SoundInstanceStrc *pInstance;
    D2SoundsTxt *pRecord;

    if (!hInstance)
        return;
    for (pNode = gpSoundInstances; ; pNode = pNode->pNext)
    {
        if (!pNode)
            return;
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (pInstance->hInstance == hInstance)
            break;
    }

    pRecord = SOUNDHDR_GetRecord(pInstance->nSoundId);
    pInstance->bStarted = 1;
    if (pRecord->nFadeOut && pInstance->eState == 1)
        SOUNDINST_UpdateForUnit(pInstance->hInstance, 0, 0, pRecord->nFadeOut);
}


//1.10f: D2Client.0x6FB556F0
//  The handle of the newest instance playing a sound id (the first on the list, which PlaySound fills from the front), or 0. The id is not tested.
int32_t __fastcall SOUNDINST_GetHandleForSoundId(D2C_Sounds nSoundId)
{
    D2SoundListNodeStrc *pNode;
    D2SoundInstanceStrc *pInstance;

    for (pNode = gpSoundInstances; pNode; pNode = pNode->pNext)
    {
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (pInstance->nSoundId == nSoundId)
            return pInstance->hInstance;
    }
    return 0;
}


//1.10f: D2Client.0x6FB55710
//  Places an instance: found by its handle, its position set to (x, y, z + 640), each converted to a float, z through an add of 640.0f before
//  the one rounding, and its squared distance from the listener taken from x and y, each held to [-2000, 2000] (SOUNDSONG_DistanceSquared).
//  The hold is two x87 tests per axis, which !(f > -2000.0f) and !(f < 2000.0f) reproduce exactly, a NaN included.
//  retn 8: x in edx, y and z on the stack.
void __fastcall SOUNDINST_SetPosition(int32_t hInstance, int32_t x, int32_t y, int32_t z)
{
    D2SoundListNodeStrc *pNode;
    D2SoundInstanceStrc *pInstance;
    float fX;
    float fY;
    float fClampedX;
    float fClampedY;

    if (!hInstance)
        return;
    for (pNode = gpSoundInstances; ; pNode = pNode->pNext)
    {
        if (!pNode)
            return;
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (pInstance->hInstance == hInstance)
            break;
    }

    fX = (float)x;
    pInstance->fPosX = fX;
    fY = (float)y;
    pInstance->fPosY = fY;
    pInstance->fPosZ = (float)((double)z + 640.0f);
    fClampedX = !(fX > -2000.0f) ? -2000.0f : (!(fX < 2000.0f) ? 2000.0f : fX);
    fClampedY = !(fY > -2000.0f) ? -2000.0f : (!(fY < 2000.0f) ? 2000.0f : fY);
    pInstance->fDistanceSquared = (float)SOUNDSONG_DistanceSquared(fClampedX, fClampedY);
}


//1.10f: D2Client.0x6FB55820
//  Starts a sound: the handle of its instance, or 0 when playback is off (gbSoundPlaybackEnabled), for an id of 0 or below (signed) and for a
//  row of volume 0 (nVolume, +3Ch).
//  A row with a Compound value (+45h, a word) may join an instance already there: the first on the list whose id is in the same group
//  (SOUNDHDR_GetGroupBaseIndex against the row's nGroupBaseIndex), not yet released (bStarted), and either Compound negative (any age) or begun
//  within Compound ticks (SOUNDHDR_GetPreloadCursor() - dwStartTick, unsigned). That instance is returned again, with the unit linked to it.
//  Otherwise the first free record of the pool (nInUse 0; none returns 0; the bound is compared signed) is zeroed whole, marked in use, pushed
//  on the list and given the next handle, the flags, the id, the start tick (the preload cursor plus nDelayFrames), nStartBlock, the base
//  volume 255 and the row's priority (+4Eh). With no unit it sits at (0, 0, 320) with the squared distance of (0, 0) and no reverb; a unit's
//  sound sits where SOUNDINST_GetUnitSoundPosition puts it, held to +/-2000 for the distance as SetPosition holds it, and takes reverb 0.5, or 0
//  when its row's Falloff is 4. For the thunder group (nGroupBaseIndex 202, SOUND_EVENT_THUNDER_1) the reverb is only indoors
//  (soundenviron.txt bIndoors of the current environment, none counting as outdoors); otherwise it is when UNITS_TestCollisionWithUnit(player,
//  unit, 2) answers non-zero (mask 2, COLLIDE_VISIBLE): a sound behind a wall echoes. A row not found takes that last test too.
//  Then, on both paths: priority +80 when the unit is the player -- a byte, wrapping past 255 -- and also on every join of a compound instance
//  and when there is no unit and no player; the unit is pushed on the instance's unit list and the handle linked to the unit
//  (UNITSND_LinkSoundToUnit). The original inlines a source's reverb share here and in SOUNDINST_ServiceInstances.
//  BUG: the id is bounded below only and its row is read without a NULL test (SOUNDHDR_GetRecord answers NULL for an id past sounds.txt).
//  LATENT: the read fails only for an id past sounds.txt. The reverb lookup alone tests its answer.
int32_t __fastcall SOUNDINST_PlaySound(D2C_Sounds nSoundId, D2UnitStrc *pUnit, int32_t nDelayFrames, int32_t nPlayFlags, int32_t nStartBlock)
{
    D2SoundsTxt *pRecord;
    D2SoundsTxt *pGroupRecord;
    D2SoundsTxt *pReverbRecord;
    D2SoundEnvironTxt *pEnv;
    D2SoundInstanceStrc *pInstance;
    D2SoundInstanceStrc *pJoined;
    D2SoundListNodeStrc *pNode;
    uint32_t nPreloadCursor;
    BOOL bOld;
    BOOL bReverb;
    float tPos[3];
    float fClampedX;
    float fClampedY;

    if (!gbSoundPlaybackEnabled)
        return 0;
    if ((int32_t)nSoundId <= 0)
        return 0;

    pRecord = SOUNDHDR_GetRecord(nSoundId);
    nPreloadCursor = SOUNDHDR_GetPreloadCursor();
    if (!pRecord->nVolume)
        return 0;

    pJoined = NULL;
    if (pRecord->nCompound)
    {
        pGroupRecord = SOUNDHDR_GetRecord(nSoundId);
        for (pNode = gpSoundInstances; pNode; pNode = pNode->pNext)
        {
            pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
            if (SOUNDHDR_GetGroupBaseIndex(pInstance->nSoundId) != pGroupRecord->nGroupBaseIndex || pInstance->bStarted)
                continue;
            bOld = (int16_t)pGroupRecord->nCompound >= 0 && SOUNDHDR_GetPreloadCursor() - pInstance->dwStartTick > (uint32_t)(int32_t)(int16_t)pGroupRecord->nCompound;
            if (!bOld)
            {
                pJoined = pInstance;
                break;
            }
        }
    }

    if (pJoined)
    {
        pInstance = pJoined;
    }
    else
    {
        for (pInstance = gaSoundInstances; pInstance->nInUse; )
        {
            ++pInstance;
            //  the bound is the end of the pool; the original spells it as gpSoundInstances' address
            if ((int32_t)(UINT_PTR)pInstance >= (int32_t)(UINT_PTR)(gaSoundInstances + 200))
                return 0;
        }

        memset(pInstance, 0, sizeof(D2SoundInstanceStrc));
        pInstance->nInUse = 1;
        D2SOUND_10058_ListPushFront(&gpSoundInstances, (int32_t)(UINT_PTR)pInstance);
        pInstance->hInstance = ++gnNextSoundInstanceHandle;
        pInstance->nPlayFlags = nPlayFlags;
        pInstance->nSoundId = nSoundId;
        pInstance->dwStartTick = nPreloadCursor + nDelayFrames;
        pInstance->nStartBlock = nStartBlock;
        pInstance->nBaseVolume = 255;
        pInstance->nPriority = pRecord->nPriority;

        if (!pUnit)
        {
            tPos[0] = 0.0f;
            tPos[1] = 0.0f;
            tPos[2] = 320.0f;
            pInstance->fPosX = tPos[0];
            pInstance->fPosY = tPos[1];
            pInstance->fPosZ = tPos[2];
            pInstance->fDistanceSquared = (float)SOUNDSONG_DistanceSquared(0.0f, 0.0f);
        }
        else
        {
            SOUNDINST_GetUnitSoundPosition(pUnit, nSoundId, tPos);
            pInstance->fPosX = tPos[0];
            pInstance->fPosY = tPos[1];
            pInstance->fPosZ = tPos[2];
            fClampedX = !(tPos[0] > -2000.0f) ? -2000.0f : (!(tPos[0] < 2000.0f) ? 2000.0f : tPos[0]);
            fClampedY = !(tPos[1] > -2000.0f) ? -2000.0f : (!(tPos[1] < 2000.0f) ? 2000.0f : tPos[1]);
            pInstance->fDistanceSquared = (float)SOUNDSONG_DistanceSquared(fClampedX, fClampedY);

            pReverbRecord = SOUNDHDR_GetRecord(nSoundId);
            if (pReverbRecord && pReverbRecord->dwFalloff == 4)
            {
                bReverb = FALSE;
            }
            else if (pReverbRecord && pReverbRecord->nGroupBaseIndex == SOUND_EVENT_THUNDER_1)
            {
                pEnv = SOUNDHDR_GetEnvRecord(SOUNDHDR_GetCurrentSoundEnvironId());
                bReverb = pEnv && pEnv->bIndoors;
            }
            else
            {
                bReverb = D2COMMON_UNITS_TestCollisionWithUnit(CUNIT_GetPlayer(), pUnit, COLLIDE_VISIBLE) != 0;
            }
            pInstance->fReverbMix = bReverb ? 0.5f : 0.0f;
        }
    }

    if (pUnit == CUNIT_GetPlayer())
        pInstance->nPriority += 80;
    if (pUnit)
    {
        D2SOUND_10058_ListPushFront(&pInstance->pUnitList, (int32_t)(UINT_PTR)pUnit);
        UNITSND_LinkSoundToUnit(pUnit, pInstance->hInstance);
    }
    return pInstance->hInstance;
}


//1.10f: D2Client.0x6FB55AE0
//  Where a unit's sound sits, relative to the player, into pOut[3]: x is the unit's client x less the player's, y twice the difference of the
//  y's, z 640. The river (SOUND_OBJECT_RIVER, 2599) is placed on its line instead: with the player's client coordinates (pX, pY) and the river
//  object's (tX, tY), in the x87 order:
//      a     = pX * -2.0f
//      proj  = ((a - tX * 0.5f) + pY - tY) * -0.4f
//      x     = _ftol(proj)
//      y     = _ftol((a - proj * -2.0f) + pY)
//  The constants are floats, promoted where the x87 loads them; _ftol truncates and its low dword is kept, which (int)(__int64) spells for every
//  input. retn 4: pOut on the stack.
void __fastcall SOUNDINST_GetUnitSoundPosition(D2UnitStrc *pUnit, D2C_Sounds nSoundId, float *pOut)
{
    D2CoordStrc tPlayerCoords;
    D2CoordStrc tUnitCoords;
    double dPlayerXScaled;
    double dProjected;
    int32_t nX;
    int32_t nY;
    int32_t nDeltaY;

    D2COMMON_UNITS_GetClientCoords(CUNIT_GetPlayer(), &tPlayerCoords);
    D2COMMON_UNITS_GetClientCoords(pUnit, &tUnitCoords);
    if (nSoundId == SOUND_OBJECT_RIVER)
    {
        dPlayerXScaled = (double)tPlayerCoords.nX * -2.0f;
        dProjected = (dPlayerXScaled - (double)tUnitCoords.nX * 0.5f + (double)tPlayerCoords.nY - (double)tUnitCoords.nY) * -0.4f;
        nX = (int32_t)(int64_t)dProjected;
        nY = (int32_t)(int64_t)(dPlayerXScaled - dProjected * -2.0f + (double)tPlayerCoords.nY);
    }
    else
    {
        nY = tUnitCoords.nY;
        nX = tUnitCoords.nX;
    }

    nDeltaY = nY - tPlayerCoords.nY;
    pOut[2] = 640.0f;
    pOut[0] = (float)(nX - tPlayerCoords.nX);
    pOut[1] = (float)((double)nDeltaY + (double)nDeltaY);
}


//1.10f: D2Client.0x6FB55B90
//  A handle's instance's base volume (nBaseVolume, +1Ch), or 0.
int32_t __fastcall SOUNDINST_GetInstanceVolume(int32_t hInstance)
{
    D2SoundListNodeStrc *pNode;
    D2SoundInstanceStrc *pInstance;

    if (!hInstance)
        return 0;
    for (pNode = gpSoundInstances; pNode; pNode = pNode->pNext)
    {
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (pInstance->hInstance == hInstance)
            return pInstance->nBaseVolume;
    }
    return 0;
}


//1.10f: D2Client.0x6FB55BC0
//  Sets a handle's instance's base volume, unchecked.
void __fastcall SOUNDINST_SetVolumeByHandle(int32_t hInstance, int32_t nVolume)
{
    D2SoundListNodeStrc *pNode;
    D2SoundInstanceStrc *pInstance;

    if (!hInstance)
        return;
    for (pNode = gpSoundInstances; pNode; pNode = pNode->pNext)
    {
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (pInstance->hInstance == hInstance)
        {
            pInstance->nBaseVolume = nVolume;
            return;
        }
    }
}


//1.10f: D2Client.0x6FB55BF0
//  Sets an instance's volume, at once or as a fade: to nVolume, over nFadeTicks ticks of the preload cursor beginning dwFadeBeginDelay ticks from
//  now. In order: found by its handle, or nothing; nothing when two or more units hold the instance (one unit's fade does not reach a sound
//  another still holds); a fade to 255 lasts at least the row's Fade In (+3Fh), a fade to 0 at least its Fade Out (+40h), the larger kept,
//  unsigned, and any other volume takes nFadeTicks as given; while a fade runs (bFading), one to the volume already faded to is dropped unless it
//  would end sooner (unsigned ticks), and a fade to 0 is replaced by nothing but another 0; a volume of 0 marks the instance released (bStarted,
//  +3Dh). With ticks, the fade is set from the base volume, starting at the cursor plus the delay, to nVolume, ending nFadeTicks later, bFading
//  set; with none, a delay is the assert "!dwFadeBeginDelay" (SoundInst.cpp:604) and the base volume is set at once. No caller passes a delay.
//  The original inlines SOUNDINST_HasActiveChannel and SOUNDINST_SetVolumeByHandle here. retn 8: the two tick counts on the stack.
//  BUG: the row is read without a NULL test in the two arms that read it. LATENT: an instance's id is one PlaySound accepted.
void __fastcall SOUNDINST_UpdateForUnit(int32_t hInstance, int32_t nVolume, uint32_t dwFadeBeginDelay, uint32_t nFadeTicks)
{
    D2SoundListNodeStrc *pNode;
    D2SoundInstanceStrc *pInstance;
    D2SoundsTxt *pRecord;
    uint32_t dwFadeStart;

    if (!hInstance)
        return;
    for (pNode = gpSoundInstances; ; pNode = pNode->pNext)
    {
        if (!pNode)
            return;
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (pInstance->hInstance == hInstance)
            break;
    }

    if (SOUNDINST_HasActiveChannel(hInstance))
        return;

    pRecord = SOUNDHDR_GetRecord(pInstance->nSoundId);
    if (nVolume == 255)
    {
        if (pRecord->nFadeIn > nFadeTicks)
            nFadeTicks = pRecord->nFadeIn;
    }
    else if (!nVolume)
    {
        if (pRecord->nFadeOut > nFadeTicks)
            nFadeTicks = pRecord->nFadeOut;
    }

    if (pInstance->bFading)
    {
        if (nVolume == pInstance->nFadeToVolume && SOUNDHDR_GetPreloadCursor() + dwFadeBeginDelay + nFadeTicks >= pInstance->dwFadeEndTick)
            return;
        if (!pInstance->nFadeToVolume && nVolume)
            return;
    }
    if (!nVolume)
        pInstance->bStarted = 1;

    if (nFadeTicks)
    {
        dwFadeStart = SOUNDHDR_GetPreloadCursor() + dwFadeBeginDelay;
        pInstance->nFadeFromVolume = pInstance->nBaseVolume;
        pInstance->dwFadeStartTick = dwFadeStart;
        pInstance->nFadeToVolume = nVolume;
        pInstance->dwFadeEndTick = dwFadeStart + nFadeTicks;
        pInstance->bFading = 1;
        return;
    }

    if (dwFadeBeginDelay)
    {
        FOG_DisplayAssert((uint32_t)"!dwFadeBeginDelay", (uint32_t)szSoundInstCpp, 604);
        exit(-1);
    }
    SOUNDINST_SetVolumeByHandle(hInstance, nVolume);
}


//1.10f: D2Client.0x6FB55D60
//  Whether two or more units hold a handle's instance: found by its handle, its unit list (pUnitList, +28h) has a first node and that node a
//  next: 1, else 0. The name is misleading: nothing here reads a channel (ptChannel is +2Ch).
BOOL __fastcall SOUNDINST_HasActiveChannel(int32_t hInstance)
{
    D2SoundListNodeStrc *pNode;
    D2SoundListNodeStrc *pUnitList;

    if (!hInstance)
        return 0;
    for (pNode = gpSoundInstances; ; pNode = pNode->pNext)
    {
        if (!pNode)
            return 0;
        if (((D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue)->hInstance == hInstance)
            break;
    }

    pUnitList = ((D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue)->pUnitList;
    if (!pUnitList || !pUnitList->pNext)
        return 0;
    return 1;
}


//1.10f: D2Client.0x6FB55DA0
//  Releases every instance of one sound id, as SOUNDINST_MarkStarted releases one: bStarted set, and in state 1 with a row Fade Out, faded to 0
//  over it. The original inlines SOUNDINST_MarkStarted's release here.
//  BUG: the row is read without a NULL test. LATENT: an instance's id is one PlaySound accepted.
void __fastcall SOUNDINST_StopSoundsById(D2C_Sounds nSoundId)
{
    D2SoundListNodeStrc *pNode;
    D2SoundInstanceStrc *pInstance;
    D2SoundsTxt *pRecord;

    for (pNode = gpSoundInstances; pNode; pNode = pNode->pNext)
    {
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (pInstance->nSoundId != nSoundId)
            continue;

        pRecord = SOUNDHDR_GetRecord(pInstance->nSoundId);
        pInstance->bStarted = 1;
        if (pRecord->nFadeOut && pInstance->eState == 1)
            SOUNDINST_UpdateForUnit(pInstance->hInstance, 0, 0, pRecord->nFadeOut);
    }
}


//1.10f: D2Client.0x6FB55DF0
//  Releases every music instance, as SOUNDINST_MarkStarted releases one: the instances whose id lies between SOUNDHDR_GetEnvironLookupMin() and
//  ...Max() (signed), the bounds of the song ids soundenviron.txt names. The "Environ" of the name comes from those getters. The original inlines
//  SOUNDINST_MarkStarted's release here.
//  BUG: the row is read without a NULL test. LATENT: an instance's id is one PlaySound accepted.
void __cdecl SOUNDINST_MarkAllEnvironStarted()
{
    D2SoundListNodeStrc *pNode;
    D2SoundInstanceStrc *pInstance;
    D2SoundsTxt *pRecord;

    for (pNode = gpSoundInstances; pNode; pNode = pNode->pNext)
    {
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (pInstance->nSoundId < SOUNDHDR_GetEnvironLookupMin() || pInstance->nSoundId > SOUNDHDR_GetEnvironLookupMax())
            continue;

        pRecord = SOUNDHDR_GetRecord(pInstance->nSoundId);
        pInstance->bStarted = 1;
        if (pRecord->nFadeOut && pInstance->eState == 1)
            SOUNDINST_UpdateForUnit(pInstance->hInstance, 0, 0, pRecord->nFadeOut);
    }
}


//1.10f: D2Client.0x6FB55E60
//  Releases every scene sound but two groups, as SOUNDINST_MarkStarted releases one: an instance whose group (SOUNDHDR_GetGroupBaseIndex, asked
//  twice) is neither nKeepGroupA nor nKeepGroupB and whose id lies in SOUND_SCENE_CATACOMBS..SOUND_SCENE_WILDERNESS_NIGHT (52..71, signed).
//  The original inlines SOUNDINST_MarkStarted's release here.
//  BUG: the row is read without a NULL test. LATENT: an instance's id is one PlaySound accepted.
void __fastcall SOUNDINST_StopSceneSounds(int32_t nKeepGroupA, int32_t nKeepGroupB)
{
    D2SoundListNodeStrc *pNode;
    D2SoundInstanceStrc *pInstance;
    D2SoundsTxt *pRecord;

    for (pNode = gpSoundInstances; pNode; pNode = pNode->pNext)
    {
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (SOUNDHDR_GetGroupBaseIndex(pInstance->nSoundId) == nKeepGroupA)
            continue;
        if (SOUNDHDR_GetGroupBaseIndex(pInstance->nSoundId) == nKeepGroupB)
            continue;
        if (pInstance->nSoundId < SOUND_SCENE_CATACOMBS || pInstance->nSoundId > SOUND_SCENE_WILDERNESS_NIGHT)
            continue;

        pRecord = SOUNDHDR_GetRecord(pInstance->nSoundId);
        pInstance->bStarted = 1;
        if (pRecord->nFadeOut && pInstance->eState == 1)
            SOUNDINST_UpdateForUnit(pInstance->hInstance, 0, 0, pRecord->nFadeOut);
    }
}


//1.10f: D2Client.0x6FB55EE0
//  Releases every event sound but two groups: StopSceneSounds over SOUND_EVENT_ARCANE_1..SOUND_EVENT_WILD_NIGHT_5 (72..201). The original
//  inlines SOUNDINST_MarkStarted's release here.
//  BUG: the row is read without a NULL test. LATENT: an instance's id is one PlaySound accepted.
void __fastcall SOUNDINST_StopEventSounds(int32_t nKeepGroupA, int32_t nKeepGroupB)
{
    D2SoundListNodeStrc *pNode;
    D2SoundInstanceStrc *pInstance;
    D2SoundsTxt *pRecord;

    for (pNode = gpSoundInstances; pNode; pNode = pNode->pNext)
    {
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (SOUNDHDR_GetGroupBaseIndex(pInstance->nSoundId) == nKeepGroupA)
            continue;
        if (SOUNDHDR_GetGroupBaseIndex(pInstance->nSoundId) == nKeepGroupB)
            continue;
        if (pInstance->nSoundId < SOUND_EVENT_ARCANE_1 || pInstance->nSoundId > SOUND_EVENT_WILD_NIGHT_5)
            continue;

        pRecord = SOUNDHDR_GetRecord(pInstance->nSoundId);
        pInstance->bStarted = 1;
        if (pRecord->nFadeOut && pInstance->eState == 1)
            SOUNDINST_UpdateForUnit(pInstance->hInstance, 0, 0, pRecord->nFadeOut);
    }
}


//1.10f: D2Client.0x6FB55F60
//  Releases every speech sound: the instances whose id lies in SOUND_AMAZON_CANTCARRY_1..SOUND_MONSTER_NIHLATHAK_TAUNT_4 (2934..4656), the voice
//  block SOUNDHDR_BuildSoundPath reads. The original inlines SOUNDINST_MarkStarted's release here.
//  BUG: the row is read without a NULL test. LATENT: an instance's id is one PlaySound accepted.
void __cdecl SOUNDINST_StopSpeechSounds()
{
    D2SoundListNodeStrc *pNode;
    D2SoundInstanceStrc *pInstance;
    D2SoundsTxt *pRecord;

    for (pNode = gpSoundInstances; pNode; pNode = pNode->pNext)
    {
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (pInstance->nSoundId < SOUND_AMAZON_CANTCARRY_1 || pInstance->nSoundId > SOUND_MONSTER_NIHLATHAK_TAUNT_4)
            continue;

        pRecord = SOUNDHDR_GetRecord(pInstance->nSoundId);
        pInstance->bStarted = 1;
        if (pRecord->nFadeOut && pInstance->eState == 1)
            SOUNDINST_UpdateForUnit(pInstance->hInstance, 0, 0, pRecord->nFadeOut);
    }
}


//1.10f: D2Client.0x6FB55FC0
//  Releases every instance, with no test before the release. The original inlines SOUNDINST_MarkStarted's release here.
//  BUG: the row is read without a NULL test. LATENT: an instance's id is one PlaySound accepted.
void __cdecl SOUNDINST_StopAllInstances()
{
    D2SoundListNodeStrc *pNode;
    D2SoundInstanceStrc *pInstance;
    D2SoundsTxt *pRecord;

    for (pNode = gpSoundInstances; pNode; pNode = pNode->pNext)
    {
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;

        pRecord = SOUNDHDR_GetRecord(pInstance->nSoundId);
        pInstance->bStarted = 1;
        if (pRecord->nFadeOut && pInstance->eState == 1)
            SOUNDINST_UpdateForUnit(pInstance->hInstance, 0, 0, pRecord->nFadeOut);
    }
}


//1.10f: D2Client.0x6FB56010
//  Whether any speech sound is live: 1 for the first instance not in state 2 whose id lies in SOUND_AMAZON_CANTCARRY_1..
//  SOUND_MONSTER_NIHLATHAK_TAUNT_4 (signed), else 0.
int32_t __cdecl SOUNDINST_IsAnySpeechActive()
{
    D2SoundListNodeStrc *pNode;
    D2SoundInstanceStrc *pInstance;

    for (pNode = gpSoundInstances; pNode; pNode = pNode->pNext)
    {
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (pInstance->eState != 2 && pInstance->nSoundId >= SOUND_AMAZON_CANTCARRY_1 && pInstance->nSoundId <= SOUND_MONSTER_NIHLATHAK_TAUNT_4)
            return 1;
    }
    return 0;
}


//1.10f: D2Client.0x6FB56050
//  Whether any music is live: the same test over SOUND_MUSIC_CAVES..SOUND_MUSIC_QUEST_SHENK (4657..4698).
int32_t __cdecl SOUNDINST_IsMusicPlaying()
{
    D2SoundListNodeStrc *pNode;
    D2SoundInstanceStrc *pInstance;

    for (pNode = gpSoundInstances; pNode; pNode = pNode->pNext)
    {
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (pInstance->eState != 2 && pInstance->nSoundId >= SOUND_MUSIC_CAVES && pInstance->nSoundId <= SOUND_MUSIC_QUEST_SHENK)
            return 1;
    }
    return 0;
}


//1.10f: D2Client.0x6FB56090
//  A handle's instance's sound id (nSoundId, +4), or 0 (SOUND_NONE).
D2C_Sounds __fastcall SOUNDINST_GetSoundIdFromHandle(int32_t hInstance)
{
    D2SoundListNodeStrc *pNode;
    D2SoundInstanceStrc *pInstance;

    if (!hInstance)
        return SOUND_NONE;
    for (pNode = gpSoundInstances; pNode; pNode = pNode->pNext)
    {
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (pInstance->hInstance == hInstance)
            return pInstance->nSoundId;
    }
    return SOUND_NONE;
}


//1.10f: D2Client.0x6FB560C0
//  The sound heartbeat, once a pump (SOUNDHDR_PumpPreload), with nTick the preload cursor itself, the clock every start and fade tick here is
//  counted in. Nothing while playback is off. In order:
//  the chaos debug (gbSoundChaosDebug): every third preload cursor (unsigned div), a random speech line is played;
//  the sort: a bubble sort of the instance list in place, the values of two adjacent nodes swapped until a pass swaps none -- priority first, then
//  the squared distance (nearer first), then the start tick (newer first), then the record's address (higher first). Two distances that are equal
//  or unordered fall through to the tick, written < and >;
//  the stop: every released instance (bStarted) not fading to 0 is put in state 2 when it was in 0, and its channel stopped when in 1;
//  the pass over every instance, its row looked up first (music is inaudible while D2Sound's music volume is 0, everything while the master
//  volume is 0 or below): state 2 (the channel let it go) starts a looping row with no Duration, not released, over again and puts any other on
//  the reap list; with tracking (in game, the row's bTracking, units on the instance) the instance is placed at its nearest unit and its reverb
//  mix moved toward the mean share by at most 0.05 a pump (gfSoundFadeStep); a running fade is stepped (from + (to - from) * (tick - start) /
//  (end - start), signed; end == start would divide by zero, and nothing makes a fade of 0 ticks); state 0, due and of non-zero volume, is
//  started when audible and within the falloff, after two conflict tests (a Defer Inst or Stop Inst row against the oldest playing instance of its
//  group; a speech line on exactly one unit against any other speech line on that unit); state 1 has its channel stopped past the row's Duration or
//  when inaudible or beyond the falloff, otherwise its volume and position applied (SOUNDCHAN_ApplyVolumeAndPosition);
//  the sweep of the reap list: each instance taken off the list, off its units and its unit list freed, its channel stopped, its record freed;
//  the two ramps, once a tick past gnSoundVolumeRampTick (unsigned): the pause fade five points down to 0 while the Esc menu or the config panel
//  is open in a single-player game, back up to 100 otherwise; the duck two points down to 70 while a solo row plays (bSolo, +52h, not fading to
//  0), back up to 100 otherwise.
//  The original inlines SOUNDINST_ComparePriority (both tests on), a source's reverb share, an instance freed and a channel stopped (the assert
//  "!ptInst->hChannel" at SoundInst.cpp:147) here.
//  BUG: the row is read without a NULL test: the pass's own, the group scan's, the one looked up after a start and the solo scan's. LATENT: an
//  instance's id is one PlaySound accepted.
//  BUG: a start that fails after the fade-in zeroed the volume (StartInstance answers NULL) is reaped in the same pass when its row does not
//  loop, and otherwise left in state 0 at volume 0, which the pass never starts again unless something sets its volume. NOT ESTABLISHED: it is
//  not shown that the start can fail there.
//  BUG: the speech conflict test replaces the group conflict's answer, so a group conflict is dropped when the speech test finds none. INERT.
void __fastcall SOUNDINST_ServiceInstances(uint32_t nTick)
{
    D2SoundListNodeStrc *pNode;
    D2SoundListNodeStrc *pNext;
    D2SoundListNodeStrc *pUnitNode;
    D2SoundListNodeStrc *pScanNode;
    D2SoundInstanceStrc *pLeft;
    D2SoundInstanceStrc *pRight;
    D2SoundInstanceStrc *pInstance;
    D2SoundInstanceStrc *pScan;
    D2SoundInstanceStrc *pConflict;
    D2SoundsTxt *pRecord;
    D2SoundsTxt *pShareRecord;
    D2SoundEnvironTxt *pEnv;
    D2UnitStrc *pUnit;
    D2UnitStrc *pNearestUnit;
    D2UnitStrc *pSingleUnit;
    D2SoundListNodeStrc *pReapList;
    BOOL bSorted;
    BOOL bSwap;
    BOOL bMasterAudible;
    BOOL bAudible;
    BOOL bPositioned;
    BOOL bResumed;
    BOOL bMenuOpen;
    BOOL bAnySolo;
    int32_t nChaosSound;
    int32_t nGroupBaseIndex;
    int32_t nUnits;
    int32_t nRestoreVolume;
    int32_t nFadeIn;
    int32_t nPauseFade;
    int32_t nDuck;
    float tOut[3];
    float fShare;
    float fOcclusionSum;
    float fNearestDistanceSq;
    float fNearestX;
    float fNearestY;
    float fNearestZ;
    float fClampedX;
    float fClampedY;
    float fMean;
    float fFalloffSq;
    double dDistanceSq;
    double dMean;
    double dRamped;
    double dFalloff;

    if (!gbSoundPlaybackEnabled)
        return;

    pReapList = NULL;
    if (gbSoundChaosDebug && !(SOUNDHDR_GetPreloadCursor() % 3))
    {
        nChaosSound = SOUNDSONG_RollRandomInRange(SOUND_AMAZON_CANTCARRY_1, SOUND_MONSTER_NIHLATHAK_TAUNT_4);
        SOUNDINST_PlaySound((D2C_Sounds)nChaosSound, NULL, 0, 0, 0);
    }

    do
    {
        bSorted = TRUE;
        for (pNode = gpSoundInstances; pNode; pNode = pNode->pNext)
        {
            pNext = pNode->pNext;
            if (!pNext)
                continue;
            pLeft = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
            pRight = (D2SoundInstanceStrc *)(UINT_PTR)pNext->nValue;
            bSwap = SOUNDINST_ComparePriority(pRight, pLeft, 1, 1);
            if (bSwap)
            {
                pNode->nValue = (int32_t)(UINT_PTR)pRight;
                pNext->nValue = (int32_t)(UINT_PTR)pLeft;
                bSorted = FALSE;
            }
        }
    }
    while (!bSorted);

    for (pNode = gpSoundInstances; pNode; pNode = pNode->pNext)
    {
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (!pInstance->bStarted || (pInstance->bFading && !pInstance->nFadeToVolume))
            continue;
        if (!pInstance->eState)
            pInstance->eState = 2;
        if (pInstance->eState == 1)
        {
            SOUNDCHAN_StopAndDetach(pInstance->ptChannel);
            if (pInstance->ptChannel)
            {
                FOG_DisplayAssert((uint32_t)"!ptInst->hChannel", (uint32_t)szSoundInstCpp, 147);
                exit(-1);
            }
        }
    }

    bMasterAudible = D2SOUND_10046_GetMasterVolumeSetting() > 0;
    for (pNode = gpSoundInstances; pNode; pNode = pNode->pNext)
    {
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        pRecord = SOUNDHDR_GetRecord(pInstance->nSoundId);
        bAudible = bMasterAudible;
        if (pInstance->nSoundId >= SOUNDHDR_GetEnvironLookupMin() && pInstance->nSoundId <= SOUNDHDR_GetEnvironLookupMax() && !D2SOUND_10048_GetMusicVolumeSetting())
            bAudible = FALSE;

        if (pInstance->eState == 2)
        {
            if (pInstance->ptChannel)
            {
                FOG_DisplayAssert((uint32_t)"!ptInst->hChannel", (uint32_t)szSoundInstCpp, 841);
                exit(-1);
            }
            if (pRecord->bLoop && !pRecord->nDuration && !pInstance->bStarted)
            {
                if (pInstance->bFading)
                {
                    pInstance->bFading = 0;
                    pInstance->nBaseVolume = pInstance->nFadeToVolume;
                }
                pInstance->eState = 0;
            }
            else
            {
                D2SOUND_10058_ListPushFront(&pReapList, (int32_t)(UINT_PTR)pInstance);
            }
            continue;
        }

        if (gbInGame && pRecord->bTracking && pInstance->pUnitList)
        {
            tOut[0] = 0.0f;
            tOut[1] = 0.0f;
            tOut[2] = 0.0f;
            fNearestX = 0.0f;
            fNearestY = 0.0f;
            fNearestZ = 0.0f;
            pNearestUnit = NULL;
            fNearestDistanceSq = -1.0f;
            fOcclusionSum = 0.0f;
            nUnits = 0;
            for (pUnitNode = pInstance->pUnitList; pUnitNode; pUnitNode = pUnitNode->pNext)
            {
                pUnit = (D2UnitStrc *)(UINT_PTR)pUnitNode->nValue;
                if (!pUnit)
                {
                    FOG_DisplayAssert((uint32_t)"hUnit", (uint32_t)szSoundInstCpp, 876);
                    exit(-1);
                }
                SOUNDINST_GetUnitSoundPosition(pUnit, pInstance->nSoundId, tOut);
                dDistanceSq = SOUNDSONG_DistanceSquared(tOut[0], tOut[1]);
                if (!pNearestUnit || dDistanceSq < fNearestDistanceSq)
                {
                    fNearestX = tOut[0];
                    fNearestDistanceSq = (float)dDistanceSq;
                    fNearestY = tOut[1];
                    fNearestZ = tOut[2];
                    pNearestUnit = pUnit;
                }

                pShareRecord = SOUNDHDR_GetRecord(pInstance->nSoundId);
                if (pShareRecord && pShareRecord->dwFalloff == 4)
                {
                    fShare = 0.0f;
                }
                else if (pShareRecord && pShareRecord->nGroupBaseIndex == SOUND_EVENT_THUNDER_1)
                {
                    pEnv = SOUNDHDR_GetEnvRecord(SOUNDHDR_GetCurrentSoundEnvironId());
                    fShare = pEnv && pEnv->bIndoors ? 0.5f : 0.0f;
                }
                else
                {
                    fShare = D2COMMON_UNITS_TestCollisionWithUnit(CUNIT_GetPlayer(), pUnit, COLLIDE_VISIBLE) ? 0.5f : 0.0f;
                }
                ++nUnits;
                fOcclusionSum = fShare + fOcclusionSum;
            }

            pInstance->fPosX = fNearestX;
            pInstance->fPosY = fNearestY;
            pInstance->fPosZ = fNearestZ;
            fClampedX = !(fNearestX > -2000.0f) ? -2000.0f : (!(fNearestX < 2000.0f) ? 2000.0f : fNearestX);
            fClampedY = !(fNearestY > -2000.0f) ? -2000.0f : (!(fNearestY < 2000.0f) ? 2000.0f : fNearestY);
            pInstance->fDistanceSquared = (float)SOUNDSONG_DistanceSquared(fClampedX, fClampedY);

            //  The mean share in st0 (dMean) is compared with the mix; the stepped mix is compared with the mean stored as a float (fMean).
            dMean = (double)fOcclusionSum / (double)nUnits;
            fMean = (float)dMean;
            if (!(dMean > (double)pInstance->fReverbMix))
            {
                dRamped = (double)pInstance->fReverbMix - 0.05f;
                pInstance->fReverbMix = dRamped > (double)fMean ? (float)dRamped : fMean;
            }
            else
            {
                dRamped = (double)pInstance->fReverbMix + 0.05f;
                pInstance->fReverbMix = dRamped < (double)fMean ? (float)dRamped : fMean;
            }
        }

        if (pInstance->bFading)
        {
            if (nTick > pInstance->dwFadeEndTick)
            {
                pInstance->bFading = 0;
                pInstance->nBaseVolume = pInstance->nFadeToVolume;
            }
            else if (nTick < pInstance->dwFadeStartTick)
            {
                pInstance->nBaseVolume = pInstance->nFadeFromVolume;
            }
            else
            {
                pInstance->nBaseVolume = pInstance->nFadeFromVolume + (int32_t)((uint32_t)(pInstance->nFadeToVolume - pInstance->nFadeFromVolume) * (nTick - pInstance->dwFadeStartTick))
                      / (int32_t)(pInstance->dwFadeEndTick - pInstance->dwFadeStartTick);
            }
        }

        dFalloff = SOUNDHDR_GetFalloffDistance(pInstance->nSoundId);
        fFalloffSq = (float)(dFalloff * dFalloff);

        if (!pInstance->eState && nTick >= pInstance->dwStartTick && pInstance->nBaseVolume)
        {
            if (bAudible && pInstance->fDistanceSquared <= fFalloffSq)
            {
                if (pInstance->ptChannel)
                {
                    FOG_DisplayAssert((uint32_t)"!ptInst->hChannel", (uint32_t)szSoundInstCpp, 929);
                    exit(-1);
                }

                pSingleUnit = NULL;
                pUnitNode = pInstance->pUnitList;
                if (pUnitNode && !pUnitNode->pNext)
                    pSingleUnit = (D2UnitStrc *)(UINT_PTR)pUnitNode->nValue;
                pConflict = NULL;
                bPositioned = pInstance->fPosX != 0.0f || pInstance->fPosY != 0.0f || pInstance->fPosZ != 0.0f;

                if (pRecord->nDeferInst || pRecord->nStopInst)
                {
                    nGroupBaseIndex = pRecord->nGroupBaseIndex;
                    for (pScanNode = gpSoundInstances; pScanNode; pScanNode = pScanNode->pNext)
                    {
                        pScan = (D2SoundInstanceStrc *)(UINT_PTR)pScanNode->nValue;
                        if (SOUNDHDR_GetRecord(pScan->nSoundId)->nGroupBaseIndex != nGroupBaseIndex)
                            continue;
                        if (pScan == pInstance || pScan->eState != 1)
                            continue;
                        if (pSingleUnit && !D2SOUND_10060_ListContains(pScan->pUnitList, (int32_t)(UINT_PTR)pSingleUnit))
                            continue;
                        if (pConflict && !(pScan->dwStartTick < pConflict->dwStartTick))
                            continue;
                        pConflict = pScan;
                    }
                }

                if (pSingleUnit && pInstance->nSoundId >= SOUND_AMAZON_CANTCARRY_1 && pInstance->nSoundId <= SOUND_MONSTER_NIHLATHAK_TAUNT_4)
                {
                    pConflict = NULL;
                    for (pScanNode = gpSoundInstances; pScanNode; pScanNode = pScanNode->pNext)
                    {
                        pScan = (D2SoundInstanceStrc *)(UINT_PTR)pScanNode->nValue;
                        if (pScan == pInstance || pScan->eState != 1)
                            continue;
                        if (!D2SOUND_10060_ListContains(pScan->pUnitList, (int32_t)(UINT_PTR)pSingleUnit))
                            continue;
                        if (pScan->nSoundId >= SOUND_AMAZON_CANTCARRY_1 && pScan->nSoundId <= SOUND_MONSTER_NIHLATHAK_TAUNT_4)
                        {
                            pConflict = pScan;
                            break;
                        }
                    }
                }

                if (!(pRecord->nDeferInst && !pInstance->pUnitList && bPositioned) && pConflict)
                {
                    if (!pRecord->nStopInst)
                    {
                        pInstance->bStarted = 1;
                        D2SOUND_10058_ListPushFront(&pReapList, (int32_t)(UINT_PTR)pInstance);
                        continue;
                    }
                    pConflict->bStarted = 1;
                    D2SOUND_10058_ListPushFront(&pReapList, (int32_t)(UINT_PTR)pConflict);
                }

                bResumed = FALSE;
                if (pInstance->nSavedStreamPos)
                {
                    pInstance->nStartBlock = pInstance->nSavedStreamPos;
                    pInstance->nSavedStreamPos = 0;
                    bResumed = TRUE;
                }
                nRestoreVolume = 0;
                nFadeIn = 0;
                if (bResumed)
                    nFadeIn = 3;
                else if (pRecord->nFadeIn && !pInstance->bFading && !(pInstance->nPlayFlags & 2))
                    nFadeIn = pRecord->nFadeIn;
                if (nFadeIn)
                {
                    nRestoreVolume = pInstance->nBaseVolume;
                    pInstance->nBaseVolume = 0;
                }

                pInstance->ptChannel = SOUNDCHAN_StartInstance(pInstance);
                if (pInstance->ptChannel)
                {
                    pInstance->eState = 1;
                    if (nFadeIn)
                    {
                        pInstance->nFadeFromVolume = pInstance->nBaseVolume;
                        pInstance->nFadeToVolume = nRestoreVolume;
                        pInstance->dwFadeStartTick = nTick;
                        pInstance->dwFadeEndTick = nTick + nFadeIn;
                        pInstance->bFading = 1;
                    }
                }
                pRecord = SOUNDHDR_GetRecord(pInstance->nSoundId);
            }

            if (!pInstance->ptChannel && !pRecord->bLoop && !(pRecord->bAsyncOnly && pRecord->eDataState == 1))
                D2SOUND_10058_ListPushFront(&pReapList, (int32_t)(UINT_PTR)pInstance);
        }

        if (pInstance->eState == 1)
        {
            if (!pInstance->ptChannel)
            {
                FOG_DisplayAssert((uint32_t)"ptInst->hChannel", (uint32_t)szSoundInstCpp, 1011);
                exit(-1);
            }
            if (pRecord->nDuration && nTick - pInstance->dwStartTick > pRecord->nDuration)
            {
                SOUNDCHAN_StopAndDetach(pInstance->ptChannel);
                if (pInstance->ptChannel)
                {
                    FOG_DisplayAssert((uint32_t)"!ptInst->hChannel", (uint32_t)szSoundInstCpp, 147);
                    exit(-1);
                }
            }
            else if (bAudible && !(pInstance->fDistanceSquared > fFalloffSq))
            {
                SOUNDCHAN_ApplyVolumeAndPosition(pInstance->ptChannel);
                if (pRecord->eDataState == 2)
                    SOUNDHDR_TouchCacheEntry(pInstance->nSoundId);
            }
            else
            {
                SOUNDCHAN_StopAndDetach(pInstance->ptChannel);
                if (pInstance->ptChannel)
                {
                    FOG_DisplayAssert((uint32_t)"!ptInst->hChannel", (uint32_t)szSoundInstCpp, 147);
                    exit(-1);
                }
            }
        }
    }

    for (pNode = pReapList; pNode; pNode = pNode->pNext)
    {
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (!pInstance)
            continue;
        D2SOUND_10059_ListRemove(&gpSoundInstances, (int32_t)(UINT_PTR)pInstance);
        for (pUnitNode = pInstance->pUnitList; pUnitNode; pUnitNode = pUnitNode->pNext)
            UNITSND_UnlinkSoundFromUnit((D2UnitStrc *)(UINT_PTR)pUnitNode->nValue, pInstance->hInstance);
        D2SOUND_10062_ListFree(&pInstance->pUnitList);
        if (pInstance->eState == 1)
            SOUNDCHAN_StopAndDetach(pInstance->ptChannel);
        if (pInstance->ptChannel)
        {
            FOG_DisplayAssert((uint32_t)"!ptInst->hChannel", (uint32_t)szSoundInstCpp, 147);
            exit(-1);
        }
        pInstance->nInUse = 0;
    }
    D2SOUND_10062_ListFree(&pReapList);

    bMenuOpen = FALSE;
    if ((GAME_GetGameType() == GAMETYPE_SINGLEPLAYER || GAME_GetGameType() == GAMETYPE_SINGLEPLAYER_UNCAPPED) && (UI_GetUiWindowStateRaw(UI_ESCMENU) || UI_GetUiWindowStateRaw(UI_CONFIG)))
        bMenuOpen = TRUE;

    bAnySolo = FALSE;
    for (pNode = gpSoundInstances; pNode; pNode = pNode->pNext)
    {
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (!SOUNDHDR_GetRecord(pInstance->nSoundId)->bSolo)
            continue;
        if (!pInstance->bFading || pInstance->nFadeToVolume)
        {
            bAnySolo = TRUE;
            break;
        }
    }

    if (nTick > gnSoundVolumeRampTick)
    {
        if (bMenuOpen)
        {
            nPauseFade = gnSoundPauseFadePercent - 5;
            gnSoundPauseFadePercent = nPauseFade < 0 ? 0 : nPauseFade;
        }
        else
        {
            nPauseFade = gnSoundPauseFadePercent + 5;
            gnSoundPauseFadePercent = 100;
            if (nPauseFade <= 100)
                gnSoundPauseFadePercent = nPauseFade;
        }

        if (bAnySolo)
        {
            nDuck = gnSoundDuckPercent - 2;
            if (nDuck < 70)
            {
                gnSoundVolumeRampTick = nTick;
                gnSoundDuckPercent = 70;
                return;
            }
            gnSoundDuckPercent = nDuck;
        }
        else
        {
            nDuck = gnSoundDuckPercent + 2;
            gnSoundDuckPercent = 100;
            if (nDuck <= 100)
                gnSoundDuckPercent = nDuck;
        }
    }
    gnSoundVolumeRampTick = nTick;
}


//1.10f: D2Client.0x6FB56AA0
//  Brings playback up: gbSoundPlaybackEnabled set to 1 (PlaySound and ServiceInstances return at once while it is 0), gnNextSoundInstanceHandle
//  set back to 0, so the next handle PlaySound hands out is 1, and the ambience, weather and event state cleared (SOUNDENV_ResetEnvironmentState,
//  a tail jmp).
void __cdecl SOUNDINST_ResetPlayback()
{
    gbSoundPlaybackEnabled = 1;
    gnNextSoundInstanceHandle = 0;
    SOUNDENV_ResetEnvironmentState();
}


//1.10f: D2Client.0x6FB56AC0
//  Frees every instance, at shutdown: the ambience, weather and event state cleared first (SOUNDENV_ResetEnvironmentState), then the list's first
//  instance freed until the list is empty -- taken off gpSoundInstances (#10059), off each of its units (UNITSND_UnlinkSoundFromUnit with its
//  handle), its unit list freed (#10062), its channel stopped when it is in state 1, its pool record freed (nInUse 0) -- and
//  gbSoundPlaybackEnabled set to 1 at the end, the value SOUNDINST_ResetPlayback gives it too. The walk does not advance a cursor: each pass
//  re-reads the head, which #10059 has just moved on. The original inlines an instance freed and a channel stopped here.
//  void: eax is 0 at the one retn and the one caller discards it; __stdcall with no argument and a bare retn is the same code as __cdecl.
//  BUG: a node whose value is NULL is never passed: the loop's test jumps back with the head unchanged, so the walk spins on it forever, and in
//  C++ a loop that spins with no side effect is undefined. UNREACHABLE: nothing pushes a NULL value on gpSoundInstances, so the C stays as written.
void __stdcall SOUNDINST_FreeAllInstances()
{
    D2SoundListNodeStrc *pNode;
    D2SoundListNodeStrc *pUnitNode;
    D2SoundInstanceStrc *pInstance;

    SOUNDENV_ResetEnvironmentState();
    pNode = gpSoundInstances;
    while (pNode)
    {
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (!pInstance)
            continue;

        D2SOUND_10059_ListRemove(&gpSoundInstances, (int32_t)(UINT_PTR)pInstance);
        for (pUnitNode = pInstance->pUnitList; pUnitNode; pUnitNode = pUnitNode->pNext)
            UNITSND_UnlinkSoundFromUnit((D2UnitStrc *)(UINT_PTR)pUnitNode->nValue, pInstance->hInstance);
        D2SOUND_10062_ListFree(&pInstance->pUnitList);
        if (pInstance->eState == 1)
            SOUNDCHAN_StopAndDetach(pInstance->ptChannel);
        if (pInstance->ptChannel)
        {
            FOG_DisplayAssert((uint32_t)"!ptInst->hChannel", (uint32_t)szSoundInstCpp, 147);
            exit(-1);
        }
        pInstance->nInUse = 0;
        pNode = gpSoundInstances;
    }
    gbSoundPlaybackEnabled = 1;
}


//1.10f: D2Client.0x6FB56B60
//  A handle's play position: the instance found by the walk, its channel asserted ("ptInst->hChannel" at SoundInst.cpp:1114) and the channel's
//  stream position answered (SOUNDCHAN_GetStreamPosition, a tail jmp); 0 for a handle of 0, an empty list or a handle not found. The position is
//  D2Sound #10018's unsigned int, read here as the int the caller tests. The parameter is the handle, compared with the instance's +8 and nothing
//  else; the local pChannel holds the channel (+2Ch).
int32_t __fastcall SOUNDINST_GetPlayPosition(int32_t hInstance)
{
    D2SoundListNodeStrc *pNode;
    D2SoundInstanceStrc *pInstance;
    D2SoundChannelStrc *pChannel;

    if (!hInstance)
        return 0;
    for (pNode = gpSoundInstances; ; pNode = pNode->pNext)
    {
        if (!pNode)
            return 0;
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (pInstance->hInstance == hInstance)
            break;
    }

    pChannel = pInstance->ptChannel;
    if (!pChannel)
    {
        FOG_DisplayAssert((uint32_t)"ptInst->hChannel", (uint32_t)szSoundInstCpp, 1114);
        exit(-1);
    }
    return SOUNDCHAN_GetStreamPosition(pChannel);
}


//1.10f: D2Client.0x6FB56BB0
//  Which of two instances ranks higher: 1 when pA does, else 0, the first test that tells them apart deciding: the higher nPriority (a byte);
//  with bUseDistance, the smaller fDistanceSquared, the nearer; with bUseAge, the later dwStartTick; and last the higher address (pB below pA,
//  unsigned). Two distances equal or unordered fall through to the age: < and >, each false on an unordered pair, as the x87 tests are.
//  __fastcall, pA in ecx and pB in edx, the two flags on the stack (retn 8).
int32_t __fastcall SOUNDINST_ComparePriority(D2SoundInstanceStrc *pA, D2SoundInstanceStrc *pB, int32_t bUseDistance, int32_t bUseAge)
{
    if (pA->nPriority > pB->nPriority)
        return 1;
    if (pA->nPriority < pB->nPriority)
        return 0;
    if (bUseDistance)
    {
        if (pA->fDistanceSquared < pB->fDistanceSquared)
            return 1;
        if (pA->fDistanceSquared > pB->fDistanceSquared)
            return 0;
    }
    if (bUseAge)
    {
        if (pA->dwStartTick > pB->dwStartTick)
            return 1;
        if (pA->dwStartTick < pB->dwStartTick)
            return 0;
    }
    return (UINT_PTR)pB < (UINT_PTR)pA;
}


//1.10f: D2Client.0x6FB56C30
//  Whether a unit is within earshot: its sound position relative to the player (SOUNDINST_GetUnitSoundPosition with no sound id) handed to
//  SOUNDSONG_DistanceSquared, and the answer at most 490000, which is 700 squared, the falloff SOUNDHDR_GetFalloffDistance answers for a row whose
//  Falloff is none of 2, 3 and 4. The constant is gfEarshotDistanceSq, written here as the literal. Less or equal answers 1; greater or unordered 0.
//  DIFFERENCE: SOUNDSONG_DistanceSquared answers in st0, unrounded, where this C takes a double, so a distance within the last bits of the bound
//  can compare the other way.
int32_t __fastcall SOUNDINST_IsUnitInEarshot(D2UnitStrc *pUnit)
{
    float tPos[3];

    SOUNDINST_GetUnitSoundPosition(pUnit, SOUND_NONE, tPos);
    return SOUNDSONG_DistanceSquared(tPos[0], tPos[1]) <= 490000.0f;
}


//1.10f: D2Client.0x6FB56C70
//  Whether a speech line plays on a unit: 1 for the first instance in state 1 whose unit list holds pUnit (#10060) -- any instance, when pUnit
//  is NULL -- and whose id lies in SOUND_AMAZON_CANTCARRY_1..SOUND_MONSTER_NIHLATHAK_TAUNT_4 (signed); else 0. A NULL value on the list is passed
//  over. The original has one exit; the C returns each answer where it is known.
BOOL __fastcall SOUNDINST_IsSpeechPlayingForUnit(D2UnitStrc *pUnit)
{
    D2SoundListNodeStrc *pNode;
    D2SoundInstanceStrc *pInstance;

    for (pNode = gpSoundInstances; pNode; pNode = pNode->pNext)
    {
        pInstance = (D2SoundInstanceStrc *)(UINT_PTR)pNode->nValue;
        if (!pInstance || pInstance->eState != 1)
            continue;
        if (pUnit && !D2SOUND_10060_ListContains(pInstance->pUnitList, (int32_t)(UINT_PTR)pUnit))
            continue;
        if (pInstance->nSoundId >= SOUND_AMAZON_CANTCARRY_1 && pInstance->nSoundId <= SOUND_MONSTER_NIHLATHAK_TAUNT_4)
            return TRUE;
    }
    return FALSE;
}


//1.10f: D2Client.0x6FB56CD0
//  The soundchaosdebug chat command (CHAT_HandleInputKey): gbSoundChaosDebug flipped, and when that turns it off every speech sound is released
//  (SOUNDINST_StopSpeechSounds, which the original inlines here). While it is on, SOUNDINST_ServiceInstances plays a random speech line on every
//  third preload cursor. The new value is kept in a local because the original tests it in the register it then stores from. void: nothing reads
//  eax after the one call.
void __cdecl SOUNDINST_ToggleChaosDebug()
{
    int32_t bEnabled;

    bEnabled = gbSoundChaosDebug == 0;
    gbSoundChaosDebug = bEnabled;
    if (bEnabled)
        return;
    SOUNDINST_StopSpeechSounds();
}

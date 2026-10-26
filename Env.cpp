#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "Env.h"
#include "Draw.h"
#include "CoreArchive.h"


//  Env.cpp -- the weather of D2Client.dll 1.10: the rain, snow and mud-bubble particle pools, the per-frame weather tick, the lightning flash
//  and its thunderclap, and the drawing of the particles, splashes and bubbles.
//  The original source file is GAME\Env.cpp (the asserts in this unit name it). Address range 0x6FAA6A10..0x6FAA893D; the two out-of-line seed
//  functions at the end of it, SEED_RollRandomNumber and SEED_RollLimitedRandomNumber, are written in Shared.cpp.

//1.10f: D2Client.0x6FAA6A10
//  Creates the three slot pools the weather runs on and primes the wind; ENV_Shutdown is the other half. Each DATAARRAYS_Alloc is followed by
//  DATAARRAYS_Init. DATAARRAYS_Alloc(pszName, nSlotCount, nSlotSize) takes the name in ecx, the count in edx and the slot size on the stack.
//      ghEnvParticles   "Environment Particles"   256 slots x 40 bytes
//      ghEnvSplashes    "Environment Splashes"    512 slots x 24 bytes
//      ghEnvBubbles     "Environment Bubbles"     128 slots x 24 bytes
//  void: the original leaves eax at 170 from its last two stores; the one caller does not read it.
void __cdecl ENV_Init()
{
    ghEnvParticles = FOG_DATAARRAYS_Alloc("Environment Particles", 256, 40);
    FOG_DATAARRAYS_Init(ghEnvParticles);

    gnEnvTargetParticleCount = 0;

    ghEnvSplashes = FOG_DATAARRAYS_Alloc("Environment Splashes", 512, 24);
    FOG_DATAARRAYS_Init(ghEnvSplashes);

    ghEnvBubbles = FOG_DATAARRAYS_Alloc("Environment Bubbles", 128, 24);
    FOG_DATAARRAYS_Init(ghEnvBubbles);

    gbEnvThunderScheduled = 0;
    gnEnvWindTargetBase2 = 42;
    gnEnvWindTargetBase = 42;
    gbEnvLightningActive = 0;
    gnEnvLastWeatherFrame = 0;
    gnEnvWindTargetRange2 = 170;
    gnEnvWindTargetRange = 170;
}

//  The source file, as every assert in this unit hands it to FOG_DisplayAssert (0x6FB7522C).
static const char szSourceFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\GAME\\Env.cpp";

//  The data root the image paths are built on (0x6FB743EC); it is upper case in the original.
static const char aDataGlobal[] = "DATA\\GLOBAL";

//  Resets the particle pool and asserts it came back empty (Env.cpp:347). The original has this as an inline function: three copies in
//  ENV_ResetWeather.
static void ENV_ResetParticlePool()
{
    FOG_DATAARRAYS_Reset(ghEnvParticles);

    //  The original's assert calls this member nTotalCount (D2DataArrayStrc +0x114).
    if (ghEnvParticles->nUsedCount)
    {
        FOG_DisplayAssert("spParticleData->nTotalCount == 0", szSourceFile, 347);
        exit(-1);
    }
}

//  The level ids ENV_SetRainState switches on, as the numbers the original compares against.
static_assert(LEVEL_HARROGATH == 109, "cmp against 6Dh -- 'm'");
static_assert(LEVEL_BLOODYFOOTHILLS == 110, "6Eh -- 'n'");
static_assert(LEVEL_ID_ACT5_BARRICADE_1 == 111, "6Fh -- 'o'");
static_assert(LEVEL_ARREATPLATEAU == 112, "70h -- 'p'");
static_assert(LEVEL_TUNDRAWASTELANDS == 117, "75h -- 'u'");
static_assert(LEVEL_ROCKYSUMMIT == 120, "78h -- 'x'");
static_assert(LEVEL_NIHLATHAKSTEMPLE == 121, "79h -- 'y'");

//  The pool field the assert at Env.cpp:347 reads.
static_assert(offsetof(D2DataArrayStrc, nUsedCount) == 0x114,
              "ENV_ResetParticlePool tests [pool+114h] after the reset");
static_assert(offsetof(D2DataArrayStrc, pSlots) == 0x11C,
              "the splash and bubble spawners index off [pool+11Ch]");

//1.10f: D2Client.0x6FAA6AB0
//  Releases the three pools ENV_Init created. void: the original leaves in eax whatever the last DATAARRAYS_Free left there.
void __cdecl ENV_Shutdown()
{
    FOG_DATAARRAYS_Free(ghEnvParticles);
    FOG_DATAARRAYS_Free(ghEnvSplashes);
    FOG_DATAARRAYS_Free(ghEnvBubbles);
}

//1.10f: D2Client.0x6FAA6AE0
//  Sets the weather state, 0..3 (the original asserts eState < NUMBER_OF_RAIN_STATES at Env.cpp:298). State 2 is a level map: HARROGATH,
//  BLOODYFOOTHILLS, ACT5_BARRICADE_1 and ARREATPLATEAU roll wind 42..211 and particles 32..87; TUNDRAWASTELANDS rolls wind 170..225 and particles
//  32..143; ROCKYSUMMIT and NIHLATHAKSTEMPLE fix the wind at 28 and the count at 256; any other level keeps the current peak and turns lightning off
//  unless it is snowing.
//  gbEnvRainStateLocked (0x6FBA7720) is never written, so it is always 0 and its half of the gate always passes; the original tests it all the
//  same. gbEnvWeatherForced is written by ENV_ResetWeather and ENV_UpdateWeather and refuses state changes away from 2.
//  Both arms that roll a count write the same value into gnEnvPeakParticleCount and gnEnvTargetParticleCount; every roll uses the player's seed.
void __fastcall ENV_SetRainState(uint32_t eState)
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    D2C_Levels eLevelId;

    if (eState >= 4)
    {
        FOG_DisplayAssert("eState >= 0 && eState < NUMBER_OF_RAIN_STATES", szSourceFile, 298);
        exit(-1);
    }

    if ((gbEnvRainStateLocked && eState) || (gbEnvWeatherForced && eState != 2))
        return;

    gnEnvRainState = eState;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        FOG_DisplayAssert("hCtrlUnit", szSourceFile, 311);
        exit(-1);
    }

    switch (eState)
    {
    case 0:
        gnEnvTargetParticleCount = 0;
        break;

    case 1:
        ENV_ResetWeather(0);
        //  The remainder is of the low dword of the new seed, unsigned (xor edx, edx / div ecx at 0x6FAA6B89; the same at 0x6FAA6C00 and 0x6FAA6C53
        //  below). SEED_RollRandomNumber returns the whole 64-bit seed, so without the cast the % would be a signed 64-bit remainder.
        gnEnvPeakParticleCount = (int32_t)((uint32_t)SEED_RollRandomNumber(&pPlayer->pSeed) % 0xE0) + 32;
        break;

    case 2:
        pRoom = CUNIT_GetPlayerRoom();
        eLevelId = (D2C_Levels)D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);
        pPlayer = CUNIT_GetPlayer();

        switch (eLevelId)
        {
        case LEVEL_HARROGATH:
        case LEVEL_BLOODYFOOTHILLS:
        case LEVEL_ID_ACT5_BARRICADE_1:
        case LEVEL_ARREATPLATEAU:
            gnEnvWindTargetBase2 = 42;
            gnEnvWindTargetBase = 42;
            gnEnvWindTargetRange2 = 170;
            gnEnvWindTargetRange = 170;
            gnEnvPeakParticleCount = (int32_t)((uint32_t)SEED_RollRandomNumber(&pPlayer->pSeed) % 0x38) + 32;
            gnEnvTargetParticleCount = gnEnvPeakParticleCount;
            break;

        case LEVEL_TUNDRAWASTELANDS:
            gnEnvWindTargetBase = 170;
            gnEnvWindTargetRange = 56;
            gnEnvWindTargetBase2 = 28;
            gnEnvWindTargetRange2 = 56;
            gnEnvPeakParticleCount = (int32_t)((uint32_t)SEED_RollRandomNumber(&pPlayer->pSeed) % 0x70) + 32;
            gnEnvTargetParticleCount = gnEnvPeakParticleCount;
            break;

        case LEVEL_ROCKYSUMMIT:
        case LEVEL_NIHLATHAKSTEMPLE:
            //  No roll here: the summit and the temple entrance get the wind and the particle count as literals.
            gnEnvWindTargetBase = 28;
            gnEnvWindTargetRange = 28;
            gnEnvWindTargetBase2 = 28;
            gnEnvWindTargetRange2 = 28;
            gnEnvWindTarget = 28;
            gnEnvWind = 28;
            gnEnvPeakParticleCount = 256;
            gnEnvTargetParticleCount = 256;
            break;

        default:
            gnEnvTargetParticleCount = gnEnvPeakParticleCount;
            if (gbEnvWeatherIsSnow)
                break;
            //  The roll is discarded: the seed is advanced and the result thrown away, as in the original.
            SEED_RollRandomNumber(&pPlayer->pSeed);
            gbEnvLightningActive = 0;
            break;
        }
        break;

    case 3:
        gbEnvLightningActive = 0;
        break;

    default:
        //  Unreachable: eState is bounded 0..3 above and all four are handled.
        return;
    }
}

//1.10f: D2Client.0x6FAA6D40
//  Resets the weather for the current level and decides whether it is snow; bForce forces the switch to snow and the particle-pool reset,
//  regardless of the percentage roll. int: every exit is a deliberate xor eax, eax (0x6FAA702A), and ENV_OnLevelChange tail-jumps here.
//  Outside act 5 (DRLG_GetActNoFromLevelId != 4, zero-based) it never snows: it clears the forced flag, resets the particle pool if it was
//  snowing, clears gbEnvWeatherIsSnow and re-rolls the wind, then returns without consulting bForce. Inside act 5 the level decides a chance in
//  percent, the player's seed rolls against it, and roll % 100 >= chance means snow this visit. The four Harrogath-side levels roll against 25;
//  the summit and the temple entrance are forced, and are handed ENV_SetRainState(2); LEVEL_TUNDRAWASTELANDS and every level the switch does not
//  name keep the chance at 0, so it always snows there. Whether anything falls is decided by ENV_UpdateWeather, from levels.txt Rain.
//  The wind roll (SEED_RollLimitedRandomNumber(seed, 375) + 125, (seed, 1500) + 500 and SEED_RollRandomNumber % 0x47 + 92; 312 / 1250 / 127 with
//  a FOG_Trace of line 399 when there is no player) is written out in both arms, as in the original.
int32_t __fastcall ENV_ResetWeather(int32_t bForce)
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    D2C_Levels eLevelId;
    uint32_t nWind;
    int32_t bWeatherForced;
    int32_t nChancePercent;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return 0;

    pRoom = CUNIT_GetPlayerRoom();
    if (!pRoom)
        return 0;

    eLevelId = (D2C_Levels)D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);

    if (D2COMMON_DRLG_GetActNoFromLevelId(eLevelId) != 4)
    {
        //  Not act 5: it never snows here.
        if (gbEnvWeatherForced)
            gbEnvWeatherForced = 0;

        if (gbEnvWeatherIsSnow)
            ENV_ResetParticlePool();

        gbEnvWeatherIsSnow = 0;

        pPlayer = CUNIT_GetPlayer();
        if (pPlayer)
        {
            gnEnvWindChangeTimer = SEED_RollLimitedRandomNumber(&pPlayer->pSeed, 375) + 125;
            gnEnvThunderTimer = SEED_RollLimitedRandomNumber(&pPlayer->pSeed, 1500) + 500;
            gnEnvWind = (uint32_t)SEED_RollRandomNumber(&pPlayer->pSeed) % 0x47 + 92;
            gnEnvWindTarget = gnEnvWind;
        }
        else
        {
            FOG_Trace("Env problem with latency at line %d of file %s", 399, szSourceFile);
            gnEnvWind = 127;
            gnEnvWindTarget = 127;
            gnEnvWindChangeTimer = 312;
            gnEnvThunderTimer = 1250;
        }

        return 0;
    }

    pPlayer = CUNIT_GetPlayer();
    if (pPlayer)
    {
        gnEnvWindChangeTimer = SEED_RollLimitedRandomNumber(&pPlayer->pSeed, 375) + 125;
        gnEnvThunderTimer = SEED_RollLimitedRandomNumber(&pPlayer->pSeed, 1500) + 500;
        nWind = (uint32_t)SEED_RollRandomNumber(&pPlayer->pSeed) % 0x47 + 92;
    }
    else
    {
        FOG_Trace("Env problem with latency at line %d of file %s", 399, szSourceFile);
        gnEnvWindChangeTimer = 312;
        gnEnvThunderTimer = 1250;
        nWind = 127;
    }

    gnEnvWind = nWind;
    gnEnvWindTarget = nWind;

    bWeatherForced = 0;
    nChancePercent = 0;

    switch (eLevelId)
    {
    case LEVEL_HARROGATH:
    case LEVEL_BLOODYFOOTHILLS:
    case LEVEL_ID_ACT5_BARRICADE_1:
    case LEVEL_ARREATPLATEAU:
        gnEnvParticleFrameBias = 0;
        nChancePercent = 25;
        break;

    case LEVEL_ROCKYSUMMIT:
    case LEVEL_NIHLATHAKSTEMPLE:
        bWeatherForced = 1;
        gnEnvParticleFrameBias = 1;
        nChancePercent = 0;
        break;

    case LEVEL_TUNDRAWASTELANDS:
        //  The same two stores as the forced pair, without the force; the original reaches them by jumping into the middle of that arm.
        gnEnvParticleFrameBias = 1;
        nChancePercent = 0;
        break;

    default:
        break;
    }

    //  The roll is the player's, so a given character sees the same weather on a given level.
    if ((int32_t)((uint32_t)SEED_RollRandomNumber(&pPlayer->pSeed) % 0x64) >= nChancePercent || bForce)
    {
        gbEnvWeatherForced = bWeatherForced;

        if (bWeatherForced)
            ENV_SetRainState(2u);

        if (gbEnvLightningActive)
            gbEnvLightningActive = 0;

        if (gbEnvWeatherIsSnow != 1 || bForce)
            ENV_ResetParticlePool();

        gbEnvWeatherIsSnow = 1;
        return 0;
    }

    if (gbEnvWeatherForced)
        gbEnvWeatherForced = 0;

    if (gbEnvWeatherIsSnow)
        ENV_ResetParticlePool();

    gbEnvWeatherIsSnow = 0;
    return 0;
}

//1.10f: D2Client.0x6FAA7050
//  Builds the weather's colour ramps and loads its sprites. The original keeps the level id and the day-ramp counter in one stack slot; the
//  second act test reads the level id back before the colour loop overwrites it with 0, and the code here has two variables.
//  The two act tests are separate ifs on the same value, each tripling the third phase's minimum duration:
//      if (act == 2) gnEnvPhaseDurationMin[2] *= 3;
//      if (act == 4) gnEnvPhaseDurationMin[2] *= 3;
//  The loop runs twelve times (nTwilightStep < 480, stepping 40). The three rain ramps walk their own counters at 80, 40 and 24 per step, all
//  divided by 12 before use, and each mixes green 25, 10 or 5 higher than the other two channels, which tints the rain. The two snow ramps are
//  grey, nSnowStep + 120 and nSnowStep - 86 on all three channels, from the day counter. The second is the fallback ramp and starts negative,
//  -86 at step 0, which D2WIN_10034_MixRGB takes as a uint8_t, so it wraps to 170; the same expression is passed here.
//  Eight images: Rain1..4 and bubble1..4 under DATA\GLOBAL\UncompOverlays, each followed by D2CMP_CelFileGetCelsPerDirection to record its
//  frame count.
void __fastcall ENV_LoadImages(D2C_Levels nLevelId)
{
    D2UnitStrc *pPlayer;
    uint32_t nWind;
    int32_t nDayStep;
    int32_t nTwilightStep;
    int32_t nNightStep;
    int32_t nRamp;
    int32_t nSnowStep;
    int32_t i;
    uint8_t *pImageSize;
    CHAR szPath[260];

    ENV_ResetWeather(1);

    //  The wind is primed once per process, not once per level.
    if (!gbEnvWindInitDone)
    {
        pPlayer = CUNIT_GetPlayer();
        if (pPlayer)
        {
            gnEnvWindChangeTimer = (uint32_t)SEED_RollRandomNumber(&pPlayer->pSeed) % 0x177 + 125;
            gnEnvThunderTimer = (uint32_t)SEED_RollRandomNumber(&pPlayer->pSeed) % 0x5DC + 500;
            nWind = (uint32_t)SEED_RollRandomNumber(&pPlayer->pSeed) % 0x47 + 92;
        }
        else
        {
            FOG_Trace("Env problem with latency at line %d of file %s", 399, szSourceFile);
            gnEnvWindChangeTimer = 312;
            gnEnvThunderTimer = 1250;
            nWind = 127;
        }

        gnEnvWind = nWind;
        gnEnvWindTarget = nWind;
        gbEnvWindInitDone = 1;
    }

    gnEnvPhaseDurationMin[0] = 7500;
    gnEnvPhaseDurationMin[1] = 250;
    gnEnvPhaseDurationMin[2] = 3000;
    gnEnvPhaseDurationMin[3] = 125;

    if (D2COMMON_DRLG_GetActNoFromLevelId(nLevelId) == 2)
        gnEnvPhaseDurationMin[2] *= 3;
    if (D2COMMON_DRLG_GetActNoFromLevelId(nLevelId) == 4)
        gnEnvPhaseDurationMin[2] *= 3;

    gnEnvPhaseDurationRange[0] = 7500;
    gnEnvPhaseDurationRange[1] = 250;
    gnEnvPhaseDurationRange[2] = 3000;
    gnEnvPhaseDurationRange[3] = 50;

    nTwilightStep = 0;
    nNightStep = 0;
    nDayStep = 0;
    nRamp = 0;

    do
    {
        nSnowStep = nDayStep / 12;

        gEnvRainColoursDay[nRamp] = D2WIN_10034_MixRGB((uint8_t)(98 - nDayStep / 12), (uint8_t)(98 - nDayStep / 12 + 25), (uint8_t)(98 - nDayStep / 12));

        gEnvRainColoursTwilight[nRamp] = D2WIN_10034_MixRGB((uint8_t)(45 - nTwilightStep / 12), (uint8_t)(45 - nTwilightStep / 12 + 10), (uint8_t)(45 - nTwilightStep / 12));

        gEnvRainColoursNight[nRamp] = D2WIN_10034_MixRGB((uint8_t)(25 - nNightStep / 12), (uint8_t)(25 - nNightStep / 12 + 5), (uint8_t)(25 - nNightStep / 12));

        gEnvSnowColours[nRamp] = D2WIN_10034_MixRGB((uint8_t)(nSnowStep + 120), (uint8_t)(nSnowStep + 120), (uint8_t)(nSnowStep + 120));

        //  Starts at -86 and is taken as a uint8_t, so the first entry wraps.
        gEnvSnowColoursFallback[nRamp] = D2WIN_10034_MixRGB((uint8_t)(nSnowStep - 86), (uint8_t)(nSnowStep - 86), (uint8_t)(nSnowStep - 86));

        nTwilightStep += 40;
        ++nRamp;
        nNightStep += 24;
        nDayStep += 80;
    }
    while (nTwilightStep < 480);

    wsprintfA(szPath, "%s\\UncompOverlays\\Rain1", aDataGlobal);
    gpEnvRainImages[0] = ARCHIVE_CellFileLoadWithSize(szPath, (uint32_t *)&pImageSize, 0);
    wsprintfA(szPath, "%s\\UncompOverlays\\Rain2", aDataGlobal);
    gpEnvRainImages[1] = ARCHIVE_CellFileLoadWithSize(szPath, (uint32_t *)&pImageSize, 0);
    wsprintfA(szPath, "%s\\UncompOverlays\\Rain3", aDataGlobal);
    gpEnvRainImages[2] = ARCHIVE_CellFileLoadWithSize(szPath, (uint32_t *)&pImageSize, 0);
    wsprintfA(szPath, "%s\\UncompOverlays\\Rain4", aDataGlobal);
    gpEnvRainImages[3] = ARCHIVE_CellFileLoadWithSize(szPath, (uint32_t *)&pImageSize, 0);

    for (i = 0; i < 4; ++i)
        gnEnvRainFrameCounts[i] = D2CMP_CelFileGetCelsPerDirection(gpEnvRainImages[i]);

    wsprintfA(szPath, "%s\\UncompOverlays\\bubble1", aDataGlobal);
    gpEnvBubbleImages[0] = ARCHIVE_CellFileLoadWithSize(szPath, (uint32_t *)&pImageSize, 0);
    wsprintfA(szPath, "%s\\UncompOverlays\\bubble2", aDataGlobal);
    gpEnvBubbleImages[1] = ARCHIVE_CellFileLoadWithSize(szPath, (uint32_t *)&pImageSize, 0);
    wsprintfA(szPath, "%s\\UncompOverlays\\bubble3", aDataGlobal);
    gpEnvBubbleImages[2] = ARCHIVE_CellFileLoadWithSize(szPath, (uint32_t *)&pImageSize, 0);
    wsprintfA(szPath, "%s\\UncompOverlays\\bubble4", aDataGlobal);
    gpEnvBubbleImages[3] = ARCHIVE_CellFileLoadWithSize(szPath, (uint32_t *)&pImageSize, 0);

    for (i = 0; i < 4; ++i)
        gnEnvBubbleFrameCounts[i] = D2CMP_CelFileGetCelsPerDirection(gpEnvBubbleImages[i]);

    gflWeatherParticleIntensity = 0.0f;
}

//  SEED_RollLimitedRandomNumberInline is D2Common's seed inline, which the original compiles into each unit that uses it (defined in Shared.cpp).
//
//  The two record strides are the arithmetic the slot address is built from: pSlots + 24 * (handle >> 16) and pSlots + 40 * (handle >> 16).
static_assert(sizeof(D2ClientEnvSplashStrc) == 24,
              "24 * (handle >> 16) in both spawners");
static_assert(sizeof(D2ClientEnvParticleStrc) == 40,
              "40 * (handle >> 16) in ENV_SpawnParticle");
static_assert(offsetof(D2ClientEnvParticleStrc, nTransIndex) == 0x25,
              "gaEnvSnowTransByPeriod's byte lands here");
static_assert(offsetof(D2ClientEnvParticleStrc, nColour) == 0x24,
              "and the ramp entry here, one byte below it");
static_assert(offsetof(D2ClientEnvSplashStrc, nWhichRain) == 0x0C,
              "asserted 0..3 at Env.cpp:744");

//1.10f: D2Client.0x6FAA7590
//  Rolls one rain splash into a free slot. The caller, DFLOOR_DrawFloorTile, spawns splashes where the ground is drawn, not where the rain
//  particle is, hence the two coordinate pairs. The flat arm writes whole pixels with a triangular scatter:
//      u = roll(80);  half = 80 - 2 * |u - 40|;  x = nX + roll(2 * half) - half;  y = nY + u
//  The perspective arm writes nPerspX + (roll(5) << 16) on both axes, 16.16 fixed point: a 0..4 pixel jitter in the high half.
//  DATAARRAYS_AllocSlot returns -1 when the 512-slot pool is full, otherwise a handle whose high word is the slot index: the record is at
//  pSlots + 24 * (handle >> 16). nWhichRain is (roll(2)) + 2, asserted >= 0 && < 4 at Env.cpp:744; the assert cannot fire.
void __fastcall ENV_SpawnSplash(int32_t nX, int32_t nY, int32_t nPerspX, int32_t nPerspY)
{
    D2UnitStrc *pPlayer;
    int32_t nSlotHandle;
    D2ClientEnvSplashStrc *pSplash;
    uint32_t nBandRoll;
    int32_t nDistFromMid;
    int32_t nHalfWidth;
    int32_t nWhichRain;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        FOG_DisplayAssert("hCtrlUnit", szSourceFile, 720);
        exit(-1);
    }

    nSlotHandle = FOG_DATAARRAYS_AllocSlot(ghEnvSplashes);
    if (nSlotHandle == -1)
        return;

    pSplash = (D2ClientEnvSplashStrc *)((char *)ghEnvSplashes->pSlots + 24 * (nSlotHandle >> 16));

    if (!D2GFX_CheckPerspective())
    {
        nBandRoll = SEED_RollLimitedRandomNumberInline(&pPlayer->pSeed, 0x50);

        nDistFromMid = (int32_t)nBandRoll - 40;
        if (nDistFromMid < 0)
            nDistFromMid = -nDistFromMid;
        nHalfWidth = 80 - 2 * nDistFromMid;

        pSplash->nX = nX + (int32_t)SEED_RollLimitedRandomNumberInline(&pPlayer->pSeed, 2 * nHalfWidth)
                      - nHalfWidth;
        pSplash->nY = nY + (int32_t)nBandRoll;
    }
    else
    {
        //  16.16 fixed point: the jitter is in the high half.
        pSplash->nX = nPerspX + ((int32_t)SEED_RollLimitedRandomNumberInline(&pPlayer->pSeed, 5) << 16);
        pSplash->nY = nPerspY + ((int32_t)SEED_RollLimitedRandomNumberInline(&pPlayer->pSeed, 5) << 16);
    }

    nWhichRain = (int32_t)SEED_RollLimitedRandomNumberInline(&pPlayer->pSeed, 2) + 2;
    pSplash->nWhichRain = nWhichRain;

    if ((uint32_t)nWhichRain >= 4)
    {
        FOG_DisplayAssert("tSplash->nWhichRain >= 0 && tSplash->nWhichRain < 4", szSourceFile, 744);
        exit(-1);
    }

    pSplash->nFrame = 0;
    pSplash->nFrameDelay = 2;
}

//1.10f: D2Client.0x6FAA7760
//  Rolls one mud bubble, the near-twin of ENV_SpawnSplash: the same 24-byte record, pool shape and triangular scatter. The differences:
//      the assert line           762 here, 720 there
//      the vertical offset       nY + u - 30 here, nY + u there
//      the perspective jitter    (roll(5) - 2) << 16 here, centred on -2..+2; (roll(5) << 16) there, 0..+4
//      nWhichRain                the constant 2 here, with no roll and no assert; a rolled 2 or 3 there
void __fastcall ENV_SpawnBubble(int32_t nX, int32_t nY, int32_t nPerspX, int32_t nPerspY)
{
    D2UnitStrc *pPlayer;
    int32_t nSlotHandle;
    D2ClientEnvSplashStrc *pBubble;
    uint32_t nBandRoll;
    int32_t nDistFromMid;
    int32_t nHalfWidth;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        FOG_DisplayAssert("hCtrlUnit", szSourceFile, 762);
        exit(-1);
    }

    nSlotHandle = FOG_DATAARRAYS_AllocSlot(ghEnvBubbles);
    if (nSlotHandle == -1)
        return;

    pBubble = (D2ClientEnvSplashStrc *)((char *)ghEnvBubbles->pSlots + 24 * (nSlotHandle >> 16));

    if (!D2GFX_CheckPerspective())
    {
        nBandRoll = SEED_RollLimitedRandomNumberInline(&pPlayer->pSeed, 0x50);

        nDistFromMid = (int32_t)nBandRoll - 40;
        if (nDistFromMid < 0)
            nDistFromMid = -nDistFromMid;
        nHalfWidth = 80 - 2 * nDistFromMid;

        pBubble->nX = nX + (int32_t)SEED_RollLimitedRandomNumberInline(&pPlayer->pSeed, 2 * nHalfWidth)
                      - nHalfWidth;
        pBubble->nY = (int32_t)nBandRoll + nY - 30;
    }
    else
    {
        pBubble->nX = nPerspX + (((int32_t)SEED_RollLimitedRandomNumberInline(&pPlayer->pSeed, 5) - 2) << 16);
        pBubble->nY = nPerspY + (((int32_t)SEED_RollLimitedRandomNumberInline(&pPlayer->pSeed, 5) - 2) << 16);
    }

    pBubble->nFrame = 0;
    pBubble->nWhichRain = 2;
    pBubble->nFrameDelay = 2;
}

//1.10f: D2Client.0x6FAA7D70
//  Rolls one weather particle, rain or snow by gbEnvWeatherIsSnow: a 40-byte record in its own pool. The seed comes in as an argument, which no
//  other spawner takes. void: the original reuses eax for the allocator handle, the period of day, the transparency byte and the colour-ramp index.
//      x = roll(gnScreenWidth);  groundY = roll(gnScreenHeight - 87) + 40;  y = roll(groundY + 20) - 20 (may start above the screen);
//      sway = roll(512);  fDepth = (groundY - 40) / (screenHeight - 87), 0..1, used for both size and speed
//  Snow: speed 8 + 28 * fDepth, linger 1, trans -1; size 7 * depthPixels / (screenHeight - 87), plus one when gnEnvParticleFrameBias is set, a
//  shape index (the original asserts nSize < NUM_SNOW_SHAPES at Env.cpp:910, so 8 shapes); colour gEnvSnowColours or gEnvSnowColoursFallback by
//  display type, indexed by 12 * nSize / 8 and then halved or quartered by the period of day.
//  Rain: speed 15 + 15 * fDepth, linger 3, size 4 + 8 * fDepth (the length of the streak ENV_UpdateLightningAndThunder draws); colour one of the
//  three rain ramps by period of day, indexed by SEED_RollLimitedRandomNumber(seed, 12), a roll that advances the seed.
//  The original asserts nPeriodOfDay < NUM_ENVIRONMENTS at Env.cpp:852 (4 environments). CUNIT_GetPlayerRoom feeds DUNGEON_GetLevelIdFromRoom
//  and the level id is never used, the period of day comes from gpAct; both calls are in the original.
//  The fallback-colour test is an interval with a hole: if (type > 0 && (type <= 3 || type == 6)) bFallbackColours = 0; so GDI, SOFTWARE, DDRAW
//  and DIRECT3D get the true ramp and GLIDE, OPENGL and RAVE the fallback. The casts of gpAct to D2DrlgActStrc * are redundant.
void __fastcall ENV_SpawnParticle(D2SeedStrc *pSeed)
{
    int32_t nSlotHandle;
    D2ClientEnvParticleStrc *pParticle;
    int32_t nGroundY;
    int32_t nDepthPixels;
    int32_t nSizeFromDepth;
    uint32_t nSize;
    int32_t bFallbackColours;
    int32_t eDisplayType;
    int32_t nPeriodOfDay;
    int32_t nColourIdx;
    int32_t nRainColourIdx;
    float fDepth;

    nSlotHandle = FOG_DATAARRAYS_AllocSlot(ghEnvParticles);
    if (nSlotHandle == -1)
        return;

    pParticle = (D2ClientEnvParticleStrc *)((char *)ghEnvParticles->pSlots + 40 * (nSlotHandle >> 16));

    pParticle->bLanded = 0;
    pParticle->nX = (int32_t)SEED_RollLimitedRandomNumberInline(pSeed, gnScreenWidth);

    nGroundY = (int32_t)SEED_RollLimitedRandomNumberInline(pSeed, gnScreenHeight - 87) + 40;
    pParticle->nGroundY = nGroundY;

    //  Born up to twenty pixels above the top of the screen.
    pParticle->nY = (int32_t)SEED_RollLimitedRandomNumberInline(pSeed, nGroundY + 20) - 20;
    pParticle->nSwayPhase = (int32_t)SEED_RollLimitedRandomNumberInline(pSeed, 512);

    nDepthPixels = pParticle->nGroundY - 40;
    fDepth = (float)((double)nDepthPixels / (double)(gnScreenHeight - 87));

    if (gbEnvWeatherIsSnow)
    {
        pParticle->nTransIndex = (uint8_t)-1;

        //  The roll is discarded: the seed is advanced and nothing is kept.
        SEED_RollRandomNumber(pSeed);

        bFallbackColours = 1;
        pParticle->nLingerFrames = 1;
        //  The -28.0 is gfConstNeg28 (fmul at 0x6FAA7EBF), taken in double as the x87 product is.
        pParticle->nSpeed = 8 - (int32_t)(double)(fDepth * (double)gfConstNeg28);

        nSizeFromDepth = 7 * nDepthPixels / (gnScreenHeight - 87);
        pParticle->nSize = nSizeFromDepth;
        if (gnEnvParticleFrameBias)
            pParticle->nSize = nSizeFromDepth + 1;

        nSize = pParticle->nSize;
        if (nSize >= 8)
        {
            FOG_DisplayAssert("tParticle->nSize < NUM_SNOW_SHAPES && tParticle->nSize >= 0", szSourceFile, 910);
            exit(-1);
        }

        eDisplayType = D2GFX_GetDisplayType();
        if (eDisplayType > 0 && (eDisplayType <= 3 || eDisplayType == DISPLAYTYPE_DIRECT3D))
            bFallbackColours = 0;

        //  Both results are thrown away.
        D2COMMON_DUNGEON_GetLevelIdFromRoom(CUNIT_GetPlayerRoom());

        nPeriodOfDay = D2COMMON_ENVIRONMENT_GetPeriodOfDayFromAct((D2DrlgActStrc *)gpAct, nullptr);
        if ((uint32_t)nPeriodOfDay >= 4)
        {
            FOG_DisplayAssert("nPeriodOfDay >= 0 && nPeriodOfDay < NUM_ENVIRONMENTS", szSourceFile, 852);
            exit(-1);
        }

        pParticle->nTransIndex = gaEnvSnowTransByPeriod[2 * nPeriodOfDay];

        nColourIdx = 12 * (int32_t)nSize / 8;
        if (bFallbackColours)
        {
            pParticle->nColour = gEnvSnowColoursFallback[nColourIdx];
        }
        else
        {
            //  Day keeps the full index; the two twilights halve it and night
            //  quarters it, so a darker sky picks a darker entry of the ramp.
            if (nPeriodOfDay == 1 || nPeriodOfDay == 3)
                nColourIdx /= 2;
            else if (nPeriodOfDay == 2)
                nColourIdx /= 4;
            pParticle->nColour = gEnvSnowColours[nColourIdx];
        }
    }
    else
    {
        //  The ramp entry is a roll and advances the seed, like the snow's: D2Common's SEED_RollLimitedRandomNumber, out of line.
        nRainColourIdx = SEED_RollLimitedRandomNumber(pSeed, 12);

        switch (D2COMMON_ENVIRONMENT_GetPeriodOfDayFromAct((D2DrlgActStrc *)gpAct, nullptr))
        {
        case 0:
            pParticle->nTransIndex = 127;
            pParticle->nColour = gEnvRainColoursDay[nRainColourIdx];
            break;
        case 1:
        case 3:
            pParticle->nTransIndex = (uint8_t)-1;
            pParticle->nColour = gEnvRainColoursTwilight[nRainColourIdx];
            break;
        case 2:
            pParticle->nTransIndex = (uint8_t)-1;
            pParticle->nColour = gEnvRainColoursNight[nRainColourIdx];
            break;
        default:
            break;
        }

        //  -8.0 and -15.0 are gfConstNeg8 and gfConstNeg15 (0x6FAA806B, 0x6FAA807A); taken in double, as above.
        pParticle->nSize = 4 - (int32_t)(double)(fDepth * (double)gfConstNeg8);
        pParticle->nLingerFrames = 3;
        pParticle->nSpeed = 15 - (int32_t)(double)(fDepth * (double)gfConstNeg15);
    }
}

static_assert(offsetof(D2ClientEnvParticleStrc, bLanded) == 0x1C,
              "the weather tick and the lightning both branch on it");
static_assert(offsetof(D2ClientEnvParticleStrc, nLingerFrames) == 0x20,
              "a landed particle counts down here before its slot is freed");
static_assert(offsetof(D2ClientEnvSplashStrc, nFrame) == 0x10,
              "the splash and bubble loops advance this");
static_assert(offsetof(D2ClientEnvSplashStrc, nFrameDelay) == 0x14,
              "and reset it to 2 each time they do");

//1.10f: D2Client.0x6FAA7440
//  Releases the eight sprites ENV_LoadImages loaded. The original ends both loops by comparing the cursor with the address of the global that
//  follows the array (`< gEnvSnowColoursFallback` for the rain images, `< &gnEnvTargetParticleCount` for the bubbles), which is a loop of four.
//  The code here writes the counted loops, since the address form depends on the linker placing those two globals adjacent.
void __cdecl ENV_UnloadImages()
{
    int32_t i;

    for (i = 0; i < 4; ++i)
    {
        if (gpEnvRainImages[i])
        {
            ARCHIVE_CellFileFree(gpEnvRainImages[i]);
            gpEnvRainImages[i] = 0;
        }
    }

    for (i = 0; i < 4; ++i)
    {
        if (gpEnvBubbleImages[i])
        {
            ARCHIVE_CellFileFree(gpEnvBubbleImages[i]);
            gpEnvBubbleImages[i] = 0;
        }
    }
}

//1.10f: D2Client.0x6FAA7490
//  Drops the weather in flight when the player moves: empties the splash and bubble pools, not the particle pool (ENV_ResetWeather does that
//  one, and only outside act 5). The parameter feeds ENV_ResetWeather's bForce; it stays a pointer with a cast because the single caller loads
//  ecx from a stack slot, so the bytes cannot say pointer or int.
void __fastcall ENV_OnLevelChange(void *pParam)
{

    FOG_DATAARRAYS_Reset(ghEnvSplashes);
    FOG_DATAARRAYS_Reset(ghEnvBubbles);

    if (pParam)
        ENV_ResetWeather((int32_t)(UINT_PTR)pParam);
}


//1.10f: D2Client.0x6FAA74C0
//  Arms one lightning flash, and refuses if one is already in flight. The one caller, SKILLS_CltDoFunc84 (slot 84 of gpSkillCltDoFuncs, skills.txt
//  cltprgfunc1 of Thunder Storm), passes 0: it is the sorceress's Thunder Storm making the sky flash, and the thunderclap behind it is sound 202,
//  event_thunder_1. bPlayThunder is stored into gbEnvPlayThunder, the flag deciding whether the next flash gets a thunderclap, so the first flash
//  of every Thunder Storm is silent and the ones after it thunder (ENV_UpdateLightningAndThunder sets the flag to 1).
void __fastcall ENV_TriggerLightningFlash(int32_t bPlayThunder)
{
    if (!gbEnvLightningActive)
    {
        gbEnvThunderScheduled = 0;
        gbEnvLightningActive = 1;
        gnEnvLightningDelay = 1;
        gbEnvPlayThunder = bPlayThunder;
    }

}

//1.10f: D2Client.0x6FAA74F0
//  Advances every mud bubble one frame and frees the ones that have run out. The pool is walked by slot index up to nHighWater, and the free
//  takes iBubble << 16, the packed handle with the index in the high word. Without perspective every bubble is scrolled by the view delta;
//  with it the stored position is already in the moving frame and is left alone.
void __cdecl ENV_UpdateBubbles()
{
    int32_t bPerspective;
    int32_t iBubble;
    D2ClientEnvSplashStrc *pBubble;
    int32_t nDelayLeft;
    int32_t nNextFrame;

    bPerspective = D2GFX_CheckPerspective();

    iBubble = 0;
    for (pBubble = (D2ClientEnvSplashStrc *)ghEnvBubbles->pSlots; iBubble <= ghEnvBubbles->nHighWater; ++pBubble, ++iBubble)
    {
        if (!pBubble->wInUse)
            continue;

        if (!bPerspective)
        {
            pBubble->nX -= gnViewScrollDeltaX;
            pBubble->nY -= gnViewScrollDeltaY;
        }

        nDelayLeft = pBubble->nFrameDelay - 1;
        pBubble->nFrameDelay = nDelayLeft;
        if (nDelayLeft <= 0)
        {
            nNextFrame = pBubble->nFrame + 1;
            pBubble->nFrameDelay = 2;
            pBubble->nFrame = nNextFrame;
            if (nNextFrame >= gnEnvBubbleFrameCounts[pBubble->nWhichRain])
                FOG_DATAARRAYS_FreeSlot(ghEnvBubbles, iBubble << 16);
        }
    }
}

//1.10f: D2Client.0x6FAA7900
//  Does the player's level have Levels.txt Mud set; DFLOOR_DrawRoomFloors uses it to choose between ENV_SpawnSplash and ENV_SpawnBubble.
//  It fetches the Rain column too and throws the answer away: the original makes the call. ENV_UpdateLightningAndThunder and
//  ENV_ShouldDrawRain do the same with the Mud column.
int32_t __cdecl ENV_IsPlayerLevelMuddy()
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    int32_t nLevelId;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return 0;

    pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
    nLevelId = D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);

    //  Fetched and discarded.
    D2COMMON_DATATBLS_GetRainFromLevelsTxt(nLevelId);

    return D2COMMON_DATATBLS_GetMudFromLevelsTxt(nLevelId) != 0;
}

//1.10f: D2Client.0x6FAA80C0
//  Keeps the weather at its target, one step per frame.
//      top up   while (pool->nUsedCount < gnEnvTargetParticleCount) ENV_SpawnParticle(pSeed): the count is a target, not a fixed population
//      ease     the wind walks toward its target by 2 per frame, or by 1 while gbEnvWeatherIsSnow is set, and clamps
//      re-aim   when gnEnvWindChangeTimer reaches zero it rolls a new target and a new timer:
//                   snow  timer = roll(0x36B) + 250,  target = gnEnvWindTargetBase + roll(gnEnvWindTargetRange)
//                   rain  timer = roll(0x177) + 125,  target = roll(0x47) + 92
//  The thunder timer is decremented here too; reaching zero arms the next flash by setting gnEnvLightningDelay to 1.
void __fastcall ENV_UpdateWeatherIntensity(D2SeedStrc *pSeed)
{
    int32_t nWindStep;
    int32_t nWindPlusStep;

    while (ghEnvParticles->nUsedCount < gnEnvTargetParticleCount)
        ENV_SpawnParticle(pSeed);

    //  Two per frame normally, one while it snows.
    nWindStep = 2 - (gbEnvWeatherIsSnow != 0);

    //  The four compares below are signed (jge 0x6FAA8112, jle 0x6FAA812A, jl 0x6FAA8136 and jle 0x6FAA8118).
    if (gnEnvWindTarget >= gnEnvWind)
    {
        if (gnEnvWindTarget > gnEnvWind)
        {
            nWindPlusStep = gnEnvWind + nWindStep;
            gnEnvWind = gnEnvWindTarget;
            if (gnEnvWindTarget >= nWindPlusStep)
                gnEnvWind = nWindPlusStep;
        }
    }
    else if (gnEnvWindTarget <= gnEnvWind - nWindStep)
    {
        gnEnvWind -= nWindStep;
    }
    else
    {
        gnEnvWind = gnEnvWindTarget;
    }

    if (!--gnEnvWindChangeTimer)
    {
        if (gbEnvWeatherIsSnow)
        {
            gnEnvWindChangeTimer = SEED_RollLimitedRandomNumberInline(pSeed, 0x36B) + 250;
            gnEnvWindTarget = gnEnvWindTargetBase + SEED_RollLimitedRandomNumberInline(pSeed, gnEnvWindTargetRange);
        }
        else
        {
            gnEnvWindChangeTimer = SEED_RollLimitedRandomNumberInline(pSeed, 0x177) + 125;
            gnEnvWindTarget = SEED_RollLimitedRandomNumberInline(pSeed, 0x47) + 92;
        }
    }

    if (gbEnvLightningActive)
    {
        if (!--gnEnvThunderTimer)
            gnEnvLightningDelay = 1;
    }
}

//1.10f: D2Client.0x6FAA7940
//  The per-frame weather tick. It reads Rain and Mud for the player's level every frame and remembers both in gbEnvWasRaining and
//  gbEnvWasMuddy, so that on the frame the player leaves a rainy or muddy level it empties the pools that level was using: particles and
//  splashes for rain, bubbles for mud.
//  The phase machine: gnEnvPhaseFramesLeft counts down; at zero the state advances 0->1->2->3->0 and a new duration is drawn as
//  gnEnvPhaseDurationMin[state] + SEED_RollLimitedRandomNumber(seed, gnEnvPhaseDurationRange[state]). States 1 and 3 ramp the target particle
//  count across the phase, 1 up (peak * elapsed / total) and 3 down (peak * remaining / total), while 0 and 2 leave it where ENV_SetRainState put
//  it: the fade in and fade out of a shower. gflWeatherParticleIntensity is the target count over 256 and feeds the wind scaling next frame.
//  A landed particle sets bLanded and zeroes its speed; nLingerFrames then counts down and only at zero is the slot freed, and refilled at once
//  if the pool is under target. Snow lingers 1 frame, rain 3.
void __cdecl ENV_UpdateWeather()
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    int32_t nLevelId;
    BOOL bWeatherOn;
    int32_t bMuddy;
    int32_t bViewPerspective;
    uint32_t dwFrameCount;
    D2SeedStrc *pSeed;
    float fIntensityScale;
    float fStep;
    D2ClientEnvParticleStrc *pParticle;
    int32_t nSlot;
    int32_t nDriftX;
    int32_t nDriftY;
    int32_t nGroundY;
    int32_t nLingerFrames;
    double fSway;
    int32_t nParticleX;
    int32_t nScreenWidth;
    D2ClientEnvSplashStrc *pSplash;
    int32_t nSplashSlot;
    int32_t nDelayLeft;
    int32_t nNextFrame;
    uint32_t eRainState;
    uint32_t nPhaseLeft;
    uint32_t nNextTargetCount;

    pPlayer = CUNIT_GetPlayer();
    nSlot = 0;
    bWeatherOn = 0;
    bMuddy = 0;

    if (pPlayer)
    {
        pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
        nLevelId = D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);
        bWeatherOn = D2COMMON_DATATBLS_GetRainFromLevelsTxt(nLevelId) != 0;
        bMuddy = D2COMMON_DATATBLS_GetMudFromLevelsTxt(nLevelId) != 0;
    }

    //  The edge: the pools are emptied on the frame the level stops having weather, not while it has none.
    if (!pPlayer || !bWeatherOn)
    {
        if (gbEnvWasRaining)
        {
            if (ghEnvParticles->nUsedCount)
                FOG_DATAARRAYS_Reset(ghEnvParticles);
            if (ghEnvSplashes->nUsedCount)
                FOG_DATAARRAYS_Reset(ghEnvSplashes);
        }
    }

    if (!bMuddy && gbEnvWasMuddy && ghEnvBubbles->nUsedCount)
        FOG_DATAARRAYS_Reset(ghEnvBubbles);

    gbEnvWasRaining = bWeatherOn;
    gbEnvWasMuddy = bMuddy;

    dwFrameCount = GAME_GetFrameCount();

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        FOG_DisplayAssert("hCtrlUnit", szSourceFile, 1349);
        exit(-1);
    }

    pSeed = &pPlayer->pSeed;

    //  Everything below runs at most once per frame.
    if (dwFrameCount <= gnEnvLastWeatherFrame)
        return;

    gnEnvLastWeatherFrame = dwFrameCount;

    if (!bWeatherOn)
    {
        gflWeatherParticleIntensity = 0.0f;
        if (bMuddy)
            ENV_UpdateBubbles();
        return;
    }

    if (ghEnvParticles->nUsedCount)
    {
        if (gbEnvWeatherIsSnow)
        {
            //  Wind-driven: the scale follows the wind's own cosine, floored at a quarter so a crosswind never stops the fall entirely. The compare
            //  reads gfQuarter (fcomp at 0x6FAA7A60); the store is an immediate 0x3E800000 (0x6FAA7A6D) and keeps its literal.
            fIntensityScale = (float)fabs(FOG_10083_Cos_LUT(gnEnvWind));
            if (fIntensityScale < gfQuarter)
                fIntensityScale = 0.25f;
        }
        else
        {
            //  gfWeatherSmoothNew and gfWeatherSmoothOld (0x6FAA7A7D, 0x6FAA7A83) are the floats 0.15000000596046448 and 0.8500000238418579,
            //  and the x87 sum takes them exact; so does this one, in double.
            fIntensityScale = (float)(gflWeatherParticleIntensity * (double)gfWeatherSmoothNew + (double)gfWeatherSmoothOld);
        }

        pParticle = (D2ClientEnvParticleStrc *)ghEnvParticles->pSlots;

        if (ghEnvParticles->nHighWater >= 0)
        {
            do
            {
                if (pParticle->wInUse)
                {
                    fStep = (float)(int32_t)(int64_t)((double)pParticle->nSpeed * fIntensityScale);
                    nDriftX = (int32_t)(int64_t)(FOG_10083_Cos_LUT(gnEnvWind) * fStep);
                    nDriftY = (int32_t)(int64_t)(FOG_10084_Sin_LUT(gnEnvWind) * fStep);

                    nGroundY = pParticle->nGroundY;
                    nDriftY = nDriftY - gnViewScrollDeltaY + pParticle->nY;
                    pParticle->nY = nDriftY;
                    if (nDriftY > nGroundY)
                        pParticle->bLanded = 1;

                    if (pParticle->bLanded)
                    {
                        nLingerFrames = pParticle->nLingerFrames;
                        if (nLingerFrames)
                        {
                            nDriftX = 0;
                            pParticle->nSpeed = 0;
                            pParticle->nLingerFrames = nLingerFrames - 1;
                        }
                        else
                        {
                            FOG_DATAARRAYS_FreeSlot(ghEnvParticles, nSlot << 16);
                            if (ghEnvParticles->nUsedCount < gnEnvTargetParticleCount)
                                ENV_SpawnParticle(pSeed);
                        }
                    }

                    //  The slot may have been freed just above, so this is a re-test and not a redundant one.
                    if (pParticle->wInUse)
                    {
                        if (gbEnvWeatherIsSnow && !pParticle->bLanded)
                        {
                            //  Snow sways; the phase advances once per frame for the whole field, scaled into the 512-entry sine table.
                            fSway = FOG_10084_Sin_LUT(((uint16_t)pParticle->nSwayPhase + (uint16_t)((gnEnvParticleSwayPhase << 9) / 0x19u)) & 0x1FF);
                            nDriftX += (int32_t)(int64_t)(fSway + fSway);
                        }

                        nParticleX = pParticle->nX - gnViewScrollDeltaX + nDriftX;
                        pParticle->nX = nParticleX;

                        //  Wrap rather than clamp: the field is a torus, so a particle blown off one edge reappears on the other.
                        nScreenWidth = gnScreenWidth;
                        if (nParticleX >= nScreenWidth)
                            pParticle->nX = nParticleX - nScreenWidth;
                        if (pParticle->nX < 0)
                            pParticle->nX = nScreenWidth + pParticle->nX;
                    }
                }

                ++nSlot;
                ++pParticle;
            }
            while (nSlot <= ghEnvParticles->nHighWater);
        }

        ++gnEnvParticleSwayPhase;
    }

    bViewPerspective = D2GFX_CheckPerspective();

    nSplashSlot = 0;
    for (pSplash = (D2ClientEnvSplashStrc *)ghEnvSplashes->pSlots; nSplashSlot <= ghEnvSplashes->nHighWater; ++pSplash, ++nSplashSlot)
    {
        if (!pSplash->wInUse)
            continue;

        if (!bViewPerspective)
        {
            pSplash->nX -= gnViewScrollDeltaX;
            pSplash->nY -= gnViewScrollDeltaY;
        }

        nDelayLeft = pSplash->nFrameDelay - 1;
        pSplash->nFrameDelay = nDelayLeft;
        if (nDelayLeft <= 0)
        {
            nNextFrame = pSplash->nFrame + 1;
            pSplash->nFrameDelay = 2;
            pSplash->nFrame = nNextFrame;
            if (nNextFrame >= gnEnvRainFrameCounts[pSplash->nWhichRain])
                FOG_DATAARRAYS_FreeSlot(ghEnvSplashes, nSplashSlot << 16);
        }
    }

    eRainState = gnEnvRainState;

    //  A forced level is dragged back to state 2 every frame it drifts off it.
    if (gbEnvWeatherForced && gnEnvRainState != 2)
    {
        ENV_SetRainState(2u);
        ENV_UpdateWeatherIntensity(pSeed);
        if (bMuddy)
            ENV_UpdateBubbles();
        return;
    }

    if (!gnEnvPhaseFramesLeft)
    {
        eRainState = gnEnvRainState + 1;
        if (gnEnvRainState == 3)
            eRainState = 0;

        gnEnvPhaseFramesTotal = gnEnvPhaseDurationMin[eRainState] + SEED_RollLimitedRandomNumber(pSeed, gnEnvPhaseDurationRange[eRainState]);
        gnEnvPhaseFramesLeft = gnEnvPhaseFramesTotal;

        ENV_SetRainState(eRainState);
    }

    nPhaseLeft = gnEnvPhaseFramesLeft - 1;
    gnEnvPhaseFramesLeft = nPhaseLeft;

    if (eRainState == 1)
    {
        //  Fading in: elapsed over total.
        nNextTargetCount = gnEnvPeakParticleCount * (gnEnvPhaseFramesTotal - nPhaseLeft) / gnEnvPhaseFramesTotal;
        gnEnvTargetParticleCount = nNextTargetCount;
    }
    else if (eRainState == 3)
    {
        //  Fading out: remaining over total.
        nNextTargetCount = nPhaseLeft * gnEnvPeakParticleCount / gnEnvPhaseFramesTotal;
        gnEnvTargetParticleCount = nNextTargetCount;
    }

    gflWeatherParticleIntensity = (float)((double)gnEnvTargetParticleCount * gfOneOver256);  // gfOneOver256: `fmul dword ptr [6FB6E220h]`, 0x6FAA7D30
    ENV_UpdateWeatherIntensity(pSeed);

    if (bMuddy)
        ENV_UpdateBubbles();
}

//1.10f: D2Client.0x6FAA8240
//  Draws the lightning flash and fires the thunderclap. gbEnvPlayThunder is set by ENV_TriggerLightningFlash to its parameter and the only
//  caller passes 0: the first time a flash's countdown expires the branch sets it to 1 and plays nothing, and every flash after that plays
//  sound 202 (event_thunder_1) at a rolled volume of 25..74, positioned at a rolled offset of -200..199 on both axes.
//  The flash is a white box over the play area at draw mode 5, but only while gnFramesPerSecond >= 10; below ten frames a second it falls
//  through to the per-particle path instead. SCROLL_GetUiShiftScreen() - 1 selects which half of the screen the lightning is allowed into:
//  0 shrinks the right edge, 1 pushes the left edge in, anything else leaves the full width.
//  Particles are drawn only when it is raining, the pool is non-empty and D2GFX_CheckLowQuality is false, three ways:
//      snow (gbEnvWeatherIsSnow)   two segments out of gnSnowShapeSegmentOffsets, indexed 4 * nSize .. 4 * nSize + 7 (the table is int[36]:
//                                  eight shapes of four ints plus one pair over)
//      a landed particle           a single-pixel line, start and end the same
//      falling rain                a streak along the wind vector, its length clipped so it never draws past nGroundY, and drawn if either
//                                  end is on screen
void __cdecl ENV_UpdateLightningAndThunder()
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    int32_t nLevelId;
    BOOL bRaining;
    D2SeedStrc *pSeed;
    int32_t hSoundInstance;
    int32_t nThunderX;
    int32_t nXStart;
    int32_t nXEnd;
    int32_t nYEnd;
    int32_t nUiShiftMinus1;
    int32_t nClipLeft;
    int32_t nClipRight;
    int32_t nGroundY;
    D2ClientEnvParticleStrc *pParticle;
    int32_t iSlot;
    int32_t nX;
    int32_t nY;
    uint32_t nShapeIdx;
    int32_t nWindDx;
    int32_t nWindDy;
    int32_t nDeltaToGround;

    pPlayer = CUNIT_GetPlayer();
    bRaining = 0;
    if (pPlayer)
    {
        pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
        nLevelId = D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);
        bRaining = D2COMMON_DATATBLS_GetRainFromLevelsTxt(nLevelId) != 0;
        //  Fetched and discarded, the mirror of ENV_IsPlayerLevelMuddy.
        D2COMMON_DATATBLS_GetMudFromLevelsTxt(nLevelId);
    }

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        FOG_DisplayAssert("hCtrlUnit", szSourceFile, 1393);
        exit(-1);
    }

    pSeed = &pPlayer->pSeed;

    if (gbEnvLightningActive && gnEnvLightningDelay)
    {
        if (!--gnEnvLightningDelay)
        {
            if (gbEnvThunderScheduled)
            {
                gbEnvThunderScheduled = 0;
                gnEnvThunderTimer = SEED_RollLimitedRandomNumberInline(pSeed, 0x5DC) + 500;

                if (gbEnvPlayThunder)
                {
                    hSoundInstance = SOUNDINST_PlaySound(SOUND_EVENT_THUNDER_1, nullptr, (int32_t)SEED_RollLimitedRandomNumberInline(pSeed, 0x32) + 25, 0, 0);

                    if (hSoundInstance)
                    {
                        nThunderX = (int32_t)SEED_RollLimitedRandomNumberInline(pSeed, 0x190);
                        SOUNDINST_SetPosition(hSoundInstance, (int32_t)SEED_RollLimitedRandomNumberInline(pSeed, 0x190) - 200, nThunderX - 200, 0);
                    }
                }
                else
                {
                    //  The first flash after a trigger is silent and arms the next one.
                    gbEnvPlayThunder = 1;
                }
            }
            else
            {
                gnEnvThunderTimer = 3;
                gbEnvThunderScheduled = 1;
            }
        }

        if ((uint32_t)gnFramesPerSecond >= 10)
        {
            nXEnd = gnScreenWidth;
            nXStart = 0;
            nYEnd = gnScreenHeight - 47;

            nUiShiftMinus1 = SCROLL_GetUiShiftScreen() - 1;
            if (!nUiShiftMinus1)
                nXEnd += 2 * (gnScreenWidth / -4);
            else if (nUiShiftMinus1 == 1)
                nXStart = 2 * (gnScreenWidth / 4);

            D2GFX_DrawBox(nXStart, 0, nXEnd, nYEnd, 0xFFu, 5);
            return;
        }
    }

    if (!bRaining || !ghEnvParticles->nUsedCount || D2GFX_CheckLowQuality())
        return;

    nClipRight = gnScreenWidth;
    nGroundY = gnScreenHeight - 47;
    nClipLeft = 0;

    nUiShiftMinus1 = SCROLL_GetUiShiftScreen() - 1;
    if (!nUiShiftMinus1)
        nClipRight = gnScreenWidth + 2 * (gnScreenWidth / -4);
    else if (nUiShiftMinus1 == 1)
        nClipLeft = 2 * (gnScreenWidth / 4);

    iSlot = 0;
    pParticle = (D2ClientEnvParticleStrc *)ghEnvParticles->pSlots;

    if (ghEnvParticles->nHighWater < 0)
        return;

    do
    {
        if (pParticle->wInUse)
        {
            if (gbEnvWeatherIsSnow)
            {
                //  Snow: two segments from the shape table.
                nX = pParticle->nX;
                nY = pParticle->nY;
                nShapeIdx = pParticle->nSize;

                if (nX >= nClipLeft && nX < nClipRight && nY >= 0 && nY < nGroundY && nShapeIdx < 8)
                {
                    D2GFX_DrawLine(nX + gnSnowShapeSegmentOffsets[4 * nShapeIdx], nY + gnSnowShapeSegmentOffsets[4 * nShapeIdx + 1], nX + gnSnowShapeSegmentOffsets[4 * nShapeIdx + 2], nY + gnSnowShapeSegmentOffsets[4 * nShapeIdx + 3],
                        pParticle->nColour, pParticle->nTransIndex);
                    D2GFX_DrawLine(nX + gnSnowShapeSegmentOffsets[4 * nShapeIdx + 4], nY + gnSnowShapeSegmentOffsets[4 * nShapeIdx + 5], nX + gnSnowShapeSegmentOffsets[4 * nShapeIdx + 6], nY + gnSnowShapeSegmentOffsets[4 * nShapeIdx + 7],
                        pParticle->nColour, pParticle->nTransIndex);
                }
            }
            else if (pParticle->bLanded)
            {
                //  A landed drop is one pixel: start and end are the same.
                nX = pParticle->nX;
                nY = pParticle->nY;
                if (nX >= nClipLeft && nX < nClipRight && nY >= 0 && nY < nGroundY)
                    D2GFX_DrawLine(nX, nY, nX, nY, pParticle->nColour, pParticle->nTransIndex);
            }
            else
            {
                //  Falling: a streak along the wind, shortened so it stops at the ground rather than crossing it.
                nWindDx = (int32_t)(int64_t)(FOG_10083_Cos_LUT(gnEnvWind) * (double)pParticle->nSize);
                nWindDy = (int32_t)(int64_t)(FOG_10084_Sin_LUT(gnEnvWind) * (double)pParticle->nSize);

                nY = pParticle->nY;
                nDeltaToGround = pParticle->nGroundY - nY;
                if (nWindDy > nDeltaToGround)
                {
                    nWindDx = nWindDx * nDeltaToGround / nWindDy;
                    nWindDy = nDeltaToGround;
                }

                nX = pParticle->nX;

                //  Drawn if either end is inside the play area.
                if ((nX >= nClipLeft && nX < nClipRight && nY >= 0 && nY < nGroundY) || (nX + nWindDx >= nClipLeft && nX + nWindDx < nClipRight && nY + nWindDy >= 0 && nY + nWindDy < nGroundY))
                {
                    D2GFX_DrawLine(nX, pParticle->nY, nX + nWindDx, nY + nWindDy, pParticle->nColour, pParticle->nTransIndex);
                }
            }
        }

        ++iSlot;
        ++pParticle;
    }
    while (iSlot <= ghEnvParticles->nHighWater);
}

//1.10f: D2Client.0x6FAA8690
//  Draws one pool of splashes or bubbles; ENV_DrawSplashesAndBubbles hands over the pool and the matching four-image array.
//  Under perspective a particle is dropped (DATAARRAYS_FreeSlot) when any of four things is true: the position does not project, the projected
//  X falls outside the play area, the projected Y does, or the draw itself returns zero. The flat arm simply skips an off-screen particle and
//  keeps its slot, so an off-screen splash is transient under perspective and persistent without it.
//  pXAdjust starts as a copy of pArray and is then handed to SCALE_AdjustPerspectivePosition as an out-parameter; the array pointer is read back
//  out of it once, as *(pXAdjust + 272) (D2DataArrayStrc::nHighWater, 0x110), before the first call overwrites it. The code here keeps the two apart.
//  SCROLL_GetUiShiftScreen() - 1 narrows the play area and also slides it: nShiftX is added to every X.
void __fastcall ENV_DrawParticleArray(D2DataArrayStrc *pArray, D2CellFileStrc **pImageArray)
{
    int32_t bPerspective;
    int32_t nShiftX;
    int32_t nUiShiftMinus1;
    int32_t nMinX;
    int32_t nMaxX;
    int32_t nMaxY;
    D2ClientEnvSplashStrc *pSlot;
    int32_t nSlot;
    int32_t nXPos;
    int32_t nYPos;
    int32_t pXAdjust;
    int32_t pYAdjust;
    D2GfxDataStrc pData;

    bPerspective = D2GFX_CheckPerspective();

    nShiftX = 0;
    nMinX = 0;
    nMaxX = gnScreenWidth;
    nMaxY = gnScreenHeight - 47;

    nUiShiftMinus1 = SCROLL_GetUiShiftScreen() - 1;
    if (!nUiShiftMinus1)
    {
        nShiftX = gnScreenWidth / -4;
        nMaxX = gnScreenWidth + 2 * (gnScreenWidth / -4);
    }
    else if (nUiShiftMinus1 == 1)
    {
        nShiftX = gnScreenWidth / 4;
        nMinX = 2 * (gnScreenWidth / 4);
    }

    pSlot = (D2ClientEnvSplashStrc *)pArray->pSlots;
    memset(&pData, 0, sizeof(pData));

    if (pArray->nHighWater < 0)
        return;

    nSlot = 0;
    do
    {
        if (pSlot->wInUse)
        {
            if (bPerspective)
            {
                if (!D2GFX_SCALE_CheckPerspectivePosition(pSlot->nX, pSlot->nY))
                {
                    FOG_DATAARRAYS_FreeSlot(pArray, nSlot << 16);
                }
                else
                {
                    D2GFX_SCALE_AdjustPerspectivePosition(pSlot->nX, pSlot->nY, 0, &pXAdjust, &pYAdjust);
                    nXPos = nShiftX + pXAdjust;
                    pXAdjust += nShiftX;

                    if (pXAdjust < nMinX || nXPos >= nMaxX || pYAdjust < 0 || pYAdjust >= nMaxY)
                    {
                        FOG_DATAARRAYS_FreeSlot(pArray, nSlot << 16);
                    }
                    else
                    {
                        pData.pCellFile = pImageArray[pSlot->nWhichRain];
                        pData.nFrame = pSlot->nFrame;
                        //  A draw that reports nothing drawn frees the slot.
                        if (!D2GFX_TEXTURE_CelDraw(&pData, nXPos, pYAdjust, 0xFFFFFFFF, 3, nullptr))
                            FOG_DATAARRAYS_FreeSlot(pArray, nSlot << 16);
                    }
                }
            }
            else
            {
                nYPos = pSlot->nY;
                nXPos = nShiftX + pSlot->nX;
                if (nXPos >= nMinX && nXPos < nMaxX && nYPos >= 0 && nYPos < nMaxY)
                {
                    pData.pCellFile = pImageArray[pSlot->nWhichRain];
                    pData.nFrame = pSlot->nFrame;
                    D2GFX_TEXTURE_CelDraw(&pData, nXPos, nYPos, 0xFFFFFFFF, 3, nullptr);
                }
            }
        }

        ++pSlot;
        ++nSlot;
    }
    while (nSlot <= pArray->nHighWater);
}

//1.10f: D2Client.0x6FAA8830
//  Draws both small pools; each is skipped entirely when empty.
void __cdecl ENV_DrawSplashesAndBubbles()
{
    if (ghEnvSplashes->nUsedCount)
        ENV_DrawParticleArray(ghEnvSplashes, gpEnvRainImages);

    if (ghEnvBubbles->nUsedCount)
        ENV_DrawParticleArray(ghEnvBubbles, gpEnvBubbleImages);
}

//1.10f: D2Client.0x6FAA8870
//  Should the rain be drawn at all. The caller is SOUNDENV_UpdateEnvironment. gbEnvWeatherIsSnow short-circuits before the player is fetched:
//  while it snows every call gets 0 and levels.txt Rain is not consulted. The Mud lookup (DATATBLS_GetMudFromLevelsTxt on the same level id)
//  is dead here too, the answer is not read; ENV_IsPlayerLevelMuddy and ENV_UpdateLightningAndThunder fetch the column they do not use the same way.
BOOL __cdecl ENV_ShouldDrawRain()
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    int32_t nLevelId;
    BOOL bRain;

    if (gbEnvWeatherIsSnow)
        return 0;

    pPlayer = CUNIT_GetPlayer();
    bRain = 0;

    if (pPlayer)
    {
        pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
        nLevelId = D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);
        bRain = D2COMMON_DATATBLS_GetRainFromLevelsTxt(nLevelId) != 0;
        //  Fetched and discarded.
        D2COMMON_DATATBLS_GetMudFromLevelsTxt(nLevelId);
    }

    return bRain;
}

//1.10f: D2Client.0x6FAA88B0
//  Returns the weather intensity (one fld and a retn). Called by DFLOOR_DrawRoomFloors and SOUNDENV_UpdateEnvironment.
double __cdecl ENV_GetWeatherParticleIntensity()
{
    return gflWeatherParticleIntensity;
}


//1.10f: D2Client.0x6FAA88C0
//  Stores its argument in gbPlayerRoomLosDraw (mov gbPlayerRoomLosDraw, ecx / retn). The one caller, PLAYER_AdvanceAnimMode, hands it D2Common's
//  DUNGEON_CheckLOSDraw of the local player's room; nothing reads the global.
void __fastcall ENV_SetPlayerRoomLosDraw(int32_t bLosDraw)
{
    gbPlayerRoomLosDraw = bLosDraw;
}

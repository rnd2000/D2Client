#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "SoundEnv.h"

//  SoundEnv.cpp -- the sound environment of D2Client.dll 1.10, what the player's surroundings sound like, once a tick: the ambient loop of the
//  current soundenviron.txt row (day or night), the rain loop faded toward the weather's strength, the row's random event sound placed to one
//  side, and the footstep sounds of the row's two materials kept loaded while the player is on the level; and the reset of all of it.
//  No assert in this unit names the original's source file, so the file name is ours. Address range 0x6FB53800..0x6FB53BFF, two functions.
//  A playing instance is held by a handle, an int: SOUNDINST_PlaySound answers the instance's +8, and every function handed one walks
//  gpSoundInstances comparing it with that +8.

static_assert(offsetof(D2SoundEnvironTxt, nDayAmbience) == 0x04 && offsetof(D2SoundEnvironTxt, nNightAmbience) == 0x08
              && offsetof(D2SoundEnvironTxt, nDayEvent) == 0x0C && offsetof(D2SoundEnvironTxt, nNightEvent) == 0x10
              && offsetof(D2SoundEnvironTxt, nEventDelay) == 0x14 && offsetof(D2SoundEnvironTxt, nMaterial1) == 0x1C
              && offsetof(D2SoundEnvironTxt, nMaterial2) == 0x20,
              "soundenviron.txt as SOUNDENV_UpdateEnvironment reads it: [edi+4], [edi+8], [edi+0Ch], [edi+10h], [edi+14h], "
              "[edi+1Ch], [edi+20h] (0x6FB538D6, 0x6FB538CA, 0x6FB538D3, 0x6FB538C7, 0x6FB53AAD, 0x6FB53B77, 0x6FB53B7A)");
static_assert(SOUND_SCENE_RAIN == 0x40 && SOUND_LIGHT_WALK_DIRT_1 == 0xAA0 && SOUND_WALK_HOOF_4 == 0xB3D
              && SOUND_LIGHT_WALK_DIRT_1 - 4 == 0xA9C && SOUND_LIGHT_RUN_DIRT_1 - SOUND_LIGHT_WALK_DIRT_1 == 0x18
              && SOUND_MEDIUM_WALK_DIRT_1 - SOUND_LIGHT_WALK_DIRT_1 == 0x30,
              "SOUNDENV_UpdateEnvironment's sound ids: `and ebx, 40h` (0x6FB538E8), `mov esi, 0AA0h` / `cmp esi, 0B3Dh` "
              "(0x6FB53B7D, 0x6FB53B8C), `lea esi, ds:0A9Ch[ebx*4]` (0x6FB53B98), `lea ecx, [esi+18h]` (0x6FB53BB0), "
              "`add esi, 30h` (0x6FB53BBD)");


//1.10f: D2Client.0x6FB53800
//  Clears the ambience, weather and event state: the level the footsteps were loaded for, the event sound, its delay and its last tick, the
//  ambience and weather sounds, and each of the two playing instances, if there is one, which is handed to SOUNDINST_MarkStarted and forgotten.
//  SOUNDINST_MarkStarted is a release, whatever its name: it finds the instance by its handle and, when the instance's sounds.txt row has a
//  fade-out and the instance's state is 1, fades it to volume 0 over that many ticks.
void __cdecl SOUNDENV_ResetEnvironmentState()
{
    gnLastSoundLevelId = 0;
    gnCurrentEventSound = SOUND_NONE;
    gnEventSoundDelay = 0;
    gdwLastEventSoundTick = 0;

    if (ghCurrentAmbienceInstance)
    {
        SOUNDINST_MarkStarted(ghCurrentAmbienceInstance);
        ghCurrentAmbienceInstance = 0;
    }
    gnCurrentAmbienceSound = SOUND_NONE;

    if (ghWeatherSoundInstance)
    {
        SOUNDINST_MarkStarted(ghWeatherSoundInstance);
        ghWeatherSoundInstance = 0;
    }
    gnWeatherSoundId = SOUND_NONE;
}


//1.10f: D2Client.0x6FB53860
//  Runs once a tick; nTick is the sound pump's own counter. Does nothing before the sound channels are up or while the player is on no level.
//  Day or night: the act's cycle index from ENVCYCLE_MORNING to ENVCYCLE_AFTERNOON is day, anything else (sunrise, sunset, night) is night;
//  the compares are signed, so the index is cast to int. The level's soundenviron.txt row gives the wanted ambience and event, the night pair
//  or the day pair; no row, none.
//  Rain: nRainSound is SOUND_SCENE_RAIN while the rain is drawn and SOUND_NONE otherwise. Its volume is the weather's strength times 255,
//  computed only when the rain is drawn and the strength is not 0.0 (ENV_ShouldDrawRain is asked three times, the last inside a
//  `ShouldDrawRain() ? strength : 0.0` that can only take its first arm there). The strength is a double and 255.0 a float constant
//  (gfConst255); the product is truncated through __int64.
//  The EAX listener follows the row every tick (SOUNDCHAN_ApplyEaxEnvironment returns at once for the id it already has).
void __fastcall SOUNDENV_UpdateEnvironment(int32_t nTick)
{
    int32_t bNight;
    D2C_Sounds nWantedAmbience;
    D2C_Sounds nWantedEvent;
    int32_t nWeatherVolume;
    int32_t nCycle;
    int32_t nEnvId;
    int32_t nLevelId;
    D2SoundEnvironTxt *pEnv;
    D2C_Sounds nRainSound;
    BOOL bCrossFade;
    int32_t hAmbience;
    int32_t nCurrentVolume;
    int32_t nVolume;
    uint32_t nRoll;
    int32_t hEvent;
    int32_t nSign;
    int32_t nPanOffset;
    int32_t nPosY;
    int32_t nMaterial1;
    int32_t nMaterial2;
    int32_t nClearId;
    int32_t nSoundId;
    int32_t nPairsLeft;

    if (!SOUNDCHAN_IsSystemReady())
        return;

    bNight = 0;
    nLevelId = SOUNDHDR_GetPlayerLevelId();
    if (!nLevelId)
        return;

    nWantedAmbience = SOUND_NONE;
    nWantedEvent = SOUND_NONE;
    nWeatherVolume = 0;
    nCycle = D2COMMON_ENVIRONMENT_GetCycleIndexFromAct(gpAct);
    if (nCycle < (int32_t)ENVCYCLE_MORNING || nCycle > (int32_t)ENVCYCLE_AFTERNOON)
        bNight = 1;

    nEnvId = SOUNDHDR_GetCurrentSoundEnvironId();
    pEnv = SOUNDHDR_GetEnvRecord(nEnvId);
    if (pEnv)
    {
        if (bNight)
        {
            nWantedAmbience = pEnv->nNightAmbience;
            nWantedEvent = pEnv->nNightEvent;
        }
        else
        {
            nWantedAmbience = pEnv->nDayAmbience;
            nWantedEvent = pEnv->nDayEvent;
        }
    }

    nRainSound = ENV_ShouldDrawRain() ? SOUND_SCENE_RAIN : SOUND_NONE;
    if (ENV_ShouldDrawRain() && ENV_GetWeatherParticleIntensity() != gfSoundZero)
        nWeatherVolume = (int32_t)(int64_t)((ENV_ShouldDrawRain() ? ENV_GetWeatherParticleIntensity() : gfSoundZero) * gfConst255);

    //  The ambient loop, only when the wanted one is not the one playing. A change between the row's own day and night ambience cross-fades: the
    //  old instance fades to 0 over 250 ticks, the new one starts at volume 0 and fades to 255 over 250 (each at least: SOUNDINST_UpdateForUnit
    //  takes the row's Fade Out or Fade In when longer). Any other change cuts: SOUNDINST_StopSceneSounds(wanted ambience, rain sound) and
    //  StopEventSounds(wanted event, 0), and the new loop starts at its own volume. gnCurrentAmbienceSound is read again before the start, after
    //  the two calls that may have changed it. A wanted ambience of SOUND_NONE clears both globals and stops nothing itself.
    //  BUG: the row is tested once, by the `if (pEnv)` at 0x6FB538BF, which guards only the choice of ambience and event, yet
    //  SOUNDHDR_GetEnvRecord answers NULL for an id outside its table or with no table (0x6FB53D35..0x6FB53D4F). The cross-fade test reads [edi+4]
    //  (0x6FB5393B) whenever the wanted ambience (SOUND_NONE without a row) differs from the playing one, and the level change reads [edi+1Ch]
    //  (0x6FB53B77) on every change of level. LATENT: the id is levels.txt's SoundEnv, which is inside soundenviron.txt's table in every row of the
    //  stock install.
    if (nWantedAmbience != gnCurrentAmbienceSound)
    {
        bCrossFade = FALSE;
        if (nWantedAmbience == pEnv->nDayAmbience && gnCurrentAmbienceSound == pEnv->nNightAmbience)
            bCrossFade = TRUE;
        if (nWantedAmbience == pEnv->nNightAmbience && gnCurrentAmbienceSound == pEnv->nDayAmbience)
            bCrossFade = TRUE;

        if (bCrossFade)
        {
            SOUNDINST_UpdateForUnit(ghCurrentAmbienceInstance, 0, 0, 250);
        }
        else
        {
            SOUNDINST_StopSceneSounds(nWantedAmbience, nRainSound);
            SOUNDINST_StopEventSounds(nWantedEvent, 0);
        }

        if (nWantedAmbience != gnCurrentAmbienceSound)
        {
            if (nWantedAmbience)
            {
                hAmbience = SOUNDINST_PlaySound(nWantedAmbience, NULL, 0, 0, 0);
                ghCurrentAmbienceInstance = hAmbience;
                if (bCrossFade)
                {
                    SOUNDINST_SetVolumeByHandle(hAmbience, 0);
                    SOUNDINST_UpdateForUnit(ghCurrentAmbienceInstance, 255, 0, 250);
                }
                gnCurrentAmbienceSound = nWantedAmbience;
            }
            else
            {
                ghCurrentAmbienceInstance = 0;
                gnCurrentAmbienceSound = SOUND_NONE;
            }
        }
    }

    //  The rain loop. While the rain is drawn, a strength and no instance start one, of gnWeatherSoundId (the id the previous tick stored), not of
    //  nRainSound: on the first tick of rain the global is still SOUND_NONE and PlaySound is asked for sound 0, so the loop starts a tick late.
    //  The instance's volume then moves toward the target by at most 6 a tick (signed compares); reaching 0 releases it. While the rain is not
    //  drawn the instance is released untested (0x6FB53A72); SOUNDINST_MarkStarted tests its handle itself, so a 0 there does nothing.
    if (nRainSound)
    {
        if (nWeatherVolume && !ghWeatherSoundInstance)
        {
            ghWeatherSoundInstance = SOUNDINST_PlaySound(gnWeatherSoundId, NULL, 0, 0, 0);
            if (ghWeatherSoundInstance)
                SOUNDINST_SetVolumeByHandle(ghWeatherSoundInstance, nWeatherVolume);
        }

        if (ghWeatherSoundInstance)
        {
            nCurrentVolume = SOUNDINST_GetInstanceVolume(ghWeatherSoundInstance);
            if (nWeatherVolume > nCurrentVolume)
            {
                nVolume = nCurrentVolume + 6;
                if (nWeatherVolume < nVolume)
                    nVolume = nWeatherVolume;
            }
            else
            {
                nVolume = nCurrentVolume - 6;
                if (nWeatherVolume > nVolume)
                    nVolume = nWeatherVolume;
            }

            if (nVolume)
            {
                SOUNDINST_SetVolumeByHandle(ghWeatherSoundInstance, nVolume);
            }
            else
            {
                SOUNDINST_MarkStarted(ghWeatherSoundInstance);
                ghWeatherSoundInstance = 0;
            }
        }
        gnWeatherSoundId = nRainSound;
    }
    else
    {
        SOUNDINST_MarkStarted(ghWeatherSoundInstance);
        ghWeatherSoundInstance = 0;
        gnWeatherSoundId = SOUND_NONE;
    }

    SOUNDCHAN_ApplyEaxEnvironment(nEnvId);

    //  The event sound, when the row has one. A new event resets the schedule: the delay becomes nEventDelay plus
    //  SOUNDSONG_RandomOffset(nEventDelay / 3), a roll in [-nEventDelay / 3, nEventDelay / 3], and the last tick is set back by RollRandom(delay),
    //  a roll in [0, delay), so the first one plays at a random point of the first period. Once nTick - last >= delay (unsigned) the event plays at
    //  x = +/-(450..750) (RollRandom(2) picks the side, RollRandomInRange(450, 750) the distance), y = RandomOffset(100), z = 0 -- the three rolls
    //  in that order, each computed into a local to keep it -- and a new delay is rolled. When PlaySound answers 0 nothing is rolled or reset, and
    //  the next tick tries again. The rolls draw on the player's seed (CUNIT_GetPlayer, untested), which exists here: SOUNDHDR_GetPlayerLevelId
    //  answered a level.
    if (nWantedEvent)
    {
        if (nWantedEvent != gnCurrentEventSound)
        {
            gnEventSoundDelay = SOUNDSONG_RandomOffset(pEnv->nEventDelay / 3) + pEnv->nEventDelay;
            nRoll = SOUNDSONG_RollRandom(gnEventSoundDelay);
            gnCurrentEventSound = nWantedEvent;
            gdwLastEventSoundTick = nTick - nRoll;
        }

        if (nTick - gdwLastEventSoundTick >= gnEventSoundDelay)
        {
            hEvent = SOUNDINST_PlaySound(nWantedEvent, NULL, 0, 0, 0);
            if (hEvent)
            {
                nSign = SOUNDSONG_RollRandom(2) ? 1 : -1;
                nPanOffset = SOUNDSONG_RollRandomInRange(450, 750) * nSign;
                nPosY = SOUNDSONG_RandomOffset(100);
                SOUNDINST_SetPosition(hEvent, nPanOffset, nPosY, 0);
                gdwLastEventSoundTick = nTick;
                gnEventSoundDelay = SOUNDSONG_RandomOffset(pEnv->nEventDelay / 3) + pEnv->nEventDelay;
            }
        }
    }

    //  The footsteps, once per level. On a change of level every footstep group's reference count is cleared (SOUNDHDR_ClearRefCount for each id
    //  from SOUND_LIGHT_WALK_DIRT_1 below SOUND_WALK_HOOF_4 in steps of 4, forty calls) and the six groups of each of the row's two materials are
    //  taken again: material m (1 to 6: dirt, istone, ostone, sand, snow, wood) is the group at SOUND_LIGHT_WALK_DIRT_1 + 4 * (m - 1), and its
    //  light walk and run, medium walk and run, heavy walk and run are 24 apart. A material of 0 takes nothing.
    //  BUG: the step of 4 assumes every group has four rows, but after SOUND_WALK_FLESH_1 (2864) the groups have 3, 3 and 4 (sounds.txt Group
    //  Size: ginormous_low 2868, ginormous_high 2871, hoof 2874). The ids 2872 and 2876 are not heads: the loader gives a member its head's index
    //  at +60h and a group size of 1 at +3Dh (0x6FB54772..0x6FB5478D), so ClearRefCount(2872) clears 2871 alone and ClearRefCount(2876) 2874
    //  alone, and ginormous_high_2 and _3 and hoof_2 to _4 keep their counts. REACHABLE: it runs on every change of level; its cost is
    //  NOT ESTABLISHED.
    if (nLevelId != gnLastSoundLevelId)
    {
        nMaterial1 = pEnv->nMaterial1;
        nMaterial2 = pEnv->nMaterial2;

        for (nClearId = SOUND_LIGHT_WALK_DIRT_1; nClearId < (int32_t)SOUND_WALK_HOOF_4; nClearId += 4)
            SOUNDHDR_ClearRefCount((D2C_Sounds)nClearId);

        //  The two materials' blocks are the same code twice.
        if (nMaterial1)
        {
            nSoundId = SOUND_LIGHT_WALK_DIRT_1 + 4 * (nMaterial1 - 1);
            for (nPairsLeft = 3; nPairsLeft; --nPairsLeft)
            {
                SOUNDHDR_AdjustRefCount((D2C_Sounds)nSoundId, 1);
                SOUNDHDR_AdjustRefCount((D2C_Sounds)(nSoundId + (SOUND_LIGHT_RUN_DIRT_1 - SOUND_LIGHT_WALK_DIRT_1)), 1);
                nSoundId += SOUND_MEDIUM_WALK_DIRT_1 - SOUND_LIGHT_WALK_DIRT_1;
            }
        }

        if (nMaterial2)
        {
            nSoundId = SOUND_LIGHT_WALK_DIRT_1 + 4 * (nMaterial2 - 1);
            for (nPairsLeft = 3; nPairsLeft; --nPairsLeft)
            {
                SOUNDHDR_AdjustRefCount((D2C_Sounds)nSoundId, 1);
                SOUNDHDR_AdjustRefCount((D2C_Sounds)(nSoundId + (SOUND_LIGHT_RUN_DIRT_1 - SOUND_LIGHT_WALK_DIRT_1)), 1);
                nSoundId += SOUND_MEDIUM_WALK_DIRT_1 - SOUND_LIGHT_WALK_DIRT_1;
            }
        }

        gnLastSoundLevelId = nLevelId;
    }
}

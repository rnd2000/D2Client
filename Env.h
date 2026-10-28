//  Env.h -- the globals of Env.cpp: the weather's constants, colour tables, timers, pools and flags. Address range 0x6FB6E189..0x6FBA773C.
//  Each is a C++17 inline variable, so every file that includes this header shares one object. The nine .rdata items are const. The globals from
//  0x6FBA762C on lie past the end of .data's raw data (0x6FB9B000): the loader zero-fills them, and ours are zero-initialised.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB6E189
//  Snow particle transparency by period of day: ENV_SpawnParticle reads [2 * nPeriodOfDay] into D2ClientEnvParticleStrc::nTransIndex (day 200,
//  twilight 160, night 80, twilight 160). The odd bytes 64, 32 and 64 are read by nothing.
inline const uint8_t gaEnvSnowTransByPeriod[7] = { 200, 64, 160, 32, 80, 64, 160 };
static_assert(sizeof(gaEnvSnowTransByPeriod) == 7, "gaEnvSnowTransByPeriod: the item's 7 bytes in the .i64");

//  D2Client 0x6FB6E190
//  Nine rows of four ints, 144 bytes: {dxStart, dyStart, dxEnd, dyEnd}, small deltas. ENV_UpdateLightningAndThunder draws two segments per snow
//  particle, one of eight shapes; the second draw reads the next row.
inline const int32_t gnSnowShapeSegmentOffsets[36] =
{ 0, 0, 0, 1, -1, -1, 0, 0, 0, -1, 0, 1, 1, 0, -1, 1, 1, 0, 0, 1, 2, -1, -1, 1, 1, 1, -2, -1, 2, 1, -2, -1, 2, -1, -2, 2 };
static_assert(sizeof(gnSnowShapeSegmentOffsets) == 144, "gnSnowShapeSegmentOffsets: the item's 144 bytes in the .i64");

//  D2Client 0x6FB6E220
//  0.00390625, exactly 2^-8: multiplied in ENV_UpdateWeather, a division by 256 expressed as a multiply.
inline const float gfOneOver256 = 0.00390625f;
static_assert(sizeof(gfOneOver256) == 4, "gfOneOver256: the item's 4 bytes in the .i64");

//  D2Client 0x6FB6E224
//  0.85: ENV_UpdateWeather computes ratio * gfWeatherSmoothNew + this, mapping the particle ratio 0..1 onto 0.85..1.0 as the wind drift's scale.
inline const float gfWeatherSmoothOld = 0.8500000238418579f;
static_assert(sizeof(gfWeatherSmoothOld) == 4, "gfWeatherSmoothOld: the item's 4 bytes in the .i64");

//  D2Client 0x6FB6E228
//  0.15: the factor of ratio * 0.15 + 0.85; see gfWeatherSmoothOld.
inline const float gfWeatherSmoothNew = 0.15000000596046448f;
static_assert(sizeof(gfWeatherSmoothNew) == 4, "gfWeatherSmoothNew: the item's 4 bytes in the .i64");

//  D2Client 0x6FB6E22C
//  0.25: compared in ENV_UpdateWeather, a threshold rather than a scale.
inline const float gfQuarter = 0.25f;
static_assert(sizeof(gfQuarter) == 4, "gfQuarter: the item's 4 bytes in the .i64");

//  D2Client 0x6FB6E230
//  -15.0: one of three negative scales ENV_SpawnParticle multiplies by, with -8.0 and -28.0.
inline const float gfConstNeg15 = -15.0f;
static_assert(sizeof(gfConstNeg15) == 4, "gfConstNeg15: the item's 4 bytes in the .i64");

//  D2Client 0x6FB6E234
//  -8.0: the second of the three ENV_SpawnParticle scales.
inline const float gfConstNeg8 = -8.0f;
static_assert(sizeof(gfConstNeg8) == 4, "gfConstNeg8: the item's 4 bytes in the .i64");

//  D2Client 0x6FB6E238
//  -28.0: the third of the three ENV_SpawnParticle scales.
inline const float gfConstNeg28 = -28.0f;
static_assert(sizeof(gfConstNeg28) == 4, "gfConstNeg28: the item's 4 bytes in the .i64");

//  D2Client 0x6FB751C0
//  What ENV_TriggerLightningFlash was passed (its only caller passes 0). ENV_UpdateLightningAndThunder reads it: non-zero takes the
//  SOUNDINST_SetPosition path, positioning the thunderclap (sound 202, event_thunder_1) in 3D; zero skips that and stores 1 here, so the first
//  flash after each trigger is silent.
inline int32_t gbEnvPlayThunder = 1;
static_assert(sizeof(gbEnvPlayThunder) == 4, "gbEnvPlayThunder: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA762C
//  Rain particle colours for periods of day 1 and 3, the dawn and dusk pair: twelve palette indices built by ENV_LoadImages from base 45 (day 98,
//  night 25).
inline char gEnvRainColoursTwilight[12];
static_assert(sizeof(gEnvRainColoursTwilight) == 12, "gEnvRainColoursTwilight: the item's 12 bytes in the .i64");

//  D2Client 0x6FBA7638
//  The value gnEnvWind is walking toward; re-rolled from the player's seed by ENV_ResetWeather and by ENV_UpdateWeatherIntensity when its
//  countdown expires.
inline int32_t gnEnvWindTarget;
static_assert(sizeof(gnEnvWindTarget) == 4, "gnEnvWindTarget: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA763C
//  ENV_UpdateWeather's weather-particle intensity ratio, 0.0..1.0: gnEnvTargetParticleCount / 256. It scales the wind effect (ratio * 0.15 + 0.85),
//  is zeroed when the weather has no active seed, and is read back by ENV_GetWeatherParticleIntensity.
inline float gflWeatherParticleIntensity;
static_assert(sizeof(gflWeatherParticleIntensity) == 4, "gflWeatherParticleIntensity: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7640
//  Frames until the next thunder event, rolled 500..1999 from the player's seed; ENV_UpdateLightningAndThunder reloads it from the same range.
inline int32_t gnEnvThunderTimer;
static_assert(sizeof(gnEnvThunderTimer) == 4, "gnEnvThunderTimer: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7644
//  Current wind value, eased toward gnEnvWindTarget by 1 or 2 per frame in ENV_UpdateWeatherIntensity, clamped in both directions. ENV_ResetWeather
//  re-rolls it to 92..162; ENV_Init does not write it.
inline int32_t gnEnvWind;
static_assert(sizeof(gnEnvWind) == 4, "gnEnvWind: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7648
//  The four bubble cel files, loaded by ENV_LoadImages from `%s\UncompOverlays\bubble1` .. `bubble4` and unloaded by the second loop of
//  ENV_UnloadImages.
inline D2CellFileStrc *gpEnvBubbleImages[4];
static_assert(sizeof(gpEnvBubbleImages) == 16, "gpEnvBubbleImages: the item's 16 bytes in the .i64");

//  D2Client 0x6FBA7658
//  How many particles the weather should have: ENV_SetRainState rolls it from the player's seed (0 when the state is off) and
//  ENV_UpdateWeatherIntensity tops the pool up toward it. A target rather than a population.
inline int32_t gnEnvTargetParticleCount;
static_assert(sizeof(gnEnvTargetParticleCount) == 4, "gnEnvTargetParticleCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA765C
//  Frames until the next wind target is rolled: seeded 125..499 by ENV_ResetWeather and reloaded to 250..1124 by ENV_UpdateWeatherIntensity
//  each time it expires.
inline int32_t gnEnvWindChangeTimer;
static_assert(sizeof(gnEnvWindChangeTimer) == 4, "gnEnvWindChangeTimer: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7660
//  The random range of each weather phase's duration, paired with gnEnvPhaseDurationMin: the next duration is
//  SEED_RollLimitedRandomNumber(pSeed, gnEnvPhaseDurationRange[i]) + gnEnvPhaseDurationMin[i]. Set by ENV_LoadImages to {7500, 250, 3000, 50}.
inline int32_t gnEnvPhaseDurationRange[4];
static_assert(sizeof(gnEnvPhaseDurationRange) == 16, "gnEnvPhaseDurationRange: the item's 16 bytes in the .i64");

//  D2Client 0x6FBA7670
//  Rain particle colours for period of day 0: twelve palette indices built by ENV_LoadImages from base 98 (twilight 45, night 25).
//  ENV_SpawnParticle indexes the table of the period of day with a roll of 12, so each drop takes a random one of the twelve.
inline char gEnvRainColoursDay[12];
static_assert(sizeof(gEnvRainColoursDay) == 12, "gEnvRainColoursDay: the item's 12 bytes in the .i64");

//  D2Client 0x6FBA767C
//  Snow particle colours, indexed by the flake's depth band, (12 * nSize) / 8, then halved or quartered by the period of day: twelve palette
//  indices built by ENV_LoadImages. The table depends on the display type: ENV_SpawnParticle takes this one when D2GFX_GetDisplayType() is > 0
//  and (<= 3 or == 6), gEnvSnowColoursFallback otherwise.
inline char gEnvSnowColours[12];
static_assert(sizeof(gEnvSnowColours) == 12, "gEnvSnowColours: the item's 12 bytes in the .i64");

//  D2Client 0x6FBA7688
//  Frame countdown between the lightning flash and the thunderclap: armed to 1 by ENV_TriggerLightningFlash and decremented by
//  ENV_UpdateLightningAndThunder, which plays sound 202 (event_thunder_1) when it reaches zero and reloads the next interval to 500..1999.
inline int32_t gnEnvLightningDelay;
static_assert(sizeof(gnEnvLightningDelay) == 4, "gnEnvLightningDelay: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA768C
//  Rain particle colours for period of day 2, the darkest of the three: twelve palette indices built from base 25 (day 98, twilight 45).
inline char gEnvRainColoursNight[12];
static_assert(sizeof(gEnvRainColoursNight) == 12, "gEnvRainColoursNight: the item's 12 bytes in the .i64");

//  D2Client 0x6FBA7698
//  Frames per direction of each gpEnvRainImages entry: ENV_LoadImages fills it with D2CMP_CELCMP_CelFileGetCelsPerDirection and ENV_UpdateWeather
//  reads it to know when a raindrop's animation has run out.
inline int32_t gnEnvRainFrameCounts[4];
static_assert(sizeof(gnEnvRainFrameCounts) == 16, "gnEnvRainFrameCounts: the item's 16 bytes in the .i64");

//  D2Client 0x6FBA76A8
//  The full particle count for the current level and weather state, the value gnEnvTargetParticleCount ramps toward. ENV_SetRainState rolls it
//  per preset (32..255 for plain rain, 32..87 / 32..143 / a fixed 256 for the three act-5 presets); ENV_UpdateWeather scales it by the phase
//  progress to get the live target: up in state 1, down in state 3, unchanged in states 0 and 2. The quotients are unsigned (div at 0x6FAA7D08,
//  0x6FAA7D1B).
inline uint32_t gnEnvPeakParticleCount;
static_assert(sizeof(gnEnvPeakParticleCount) == 4, "gnEnvPeakParticleCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA76AC
//  A thunderclap is pending for the flash that just happened. Cleared by ENV_TriggerLightningFlash and zeroed by ENV_Init;
//  ENV_UpdateLightningAndThunder, seeing 0, sets gnEnvThunderTimer to 3 and raises this to 1, so the clap follows three frames behind the flash,
//  and clears it again once the interval to the next storm event has been drawn.
inline int32_t gbEnvThunderScheduled;
static_assert(sizeof(gbEnvThunderScheduled) == 4, "gbEnvThunderScheduled: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA76B0
//  Low end of the range a fresh wind target is rolled from: ENV_UpdateWeatherIntensity computes gnEnvWindTarget = gnEnvWindTargetBase +
//  roll(gnEnvWindTargetRange) while gbEnvWeatherIsSnow is set, and uses a hardcoded 92..162 otherwise. Set per level preset by ENV_SetRainState,
//  primed to 42 by ENV_Init.
inline int32_t gnEnvWindTargetBase;
static_assert(sizeof(gnEnvWindTargetBase) == 4, "gnEnvWindTargetBase: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA76B4
//  A second wind-target base, written in lockstep with gnEnvWindTargetBase by ENV_Init and every ENV_SetRainState preset, and never read anywhere
//  in the module (all four references are stores). It does not always equal gnEnvWindTargetBase: the level-117 preset sets that to 170 and this
//  to 28.
inline int32_t gnEnvWindTargetBase2;
static_assert(sizeof(gnEnvWindTargetBase2) == 4, "gnEnvWindTargetBase2: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA76B8
//  The four rain cel files, loaded by ENV_LoadImages from `%s\UncompOverlays\Rain1` .. `Rain4` under aDataGlobal and unloaded by ENV_UnloadImages.
inline D2CellFileStrc *gpEnvRainImages[4];
static_assert(sizeof(gpEnvRainImages) == 16, "gpEnvRainImages: the item's 16 bytes in the .i64");

//  D2Client 0x6FBA76C8
//  The other snow particle colour table, used when the display type is not in the set gEnvSnowColours covers: twelve palette indices built from
//  base -86 (against +120 for gEnvSnowColours).
inline char gEnvSnowColoursFallback[12];
static_assert(sizeof(gEnvSnowColoursFallback) == 12, "gEnvSnowColoursFallback: the item's 12 bytes in the .i64");

//  D2Client 0x6FBA76D4
//  Frames per direction of each gpEnvBubbleImages entry, filled by ENV_LoadImages as gnEnvRainFrameCounts is and compared against by
//  ENV_UpdateBubbles.
inline int32_t gnEnvBubbleFrameCounts[4];
static_assert(sizeof(gnEnvBubbleFrameCounts) == 16, "gnEnvBubbleFrameCounts: the item's 16 bytes in the .i64");

//  D2Client 0x6FBA76E4
//  The minimum duration of each weather phase, {7500, 250, 3000, 125}, added to the random draw over gnEnvPhaseDurationRange. Set by
//  ENV_LoadImages, which triples entry 2 when DRLG_GetActNoFromLevelId, counted from zero, is 2 or 4 (Act III and Act V).
inline int32_t gnEnvPhaseDurationMin[4];
static_assert(sizeof(gnEnvPhaseDurationMin) == 16, "gnEnvPhaseDurationMin: the item's 16 bytes in the .i64");

//  D2Client 0x6FBA76F4
//  Span of the range a fresh wind target is rolled from, the modulus in gnEnvWindTargetBase + roll(gnEnvWindTargetRange); see gnEnvWindTargetBase.
//  Primed to 170 by ENV_Init.
inline int32_t gnEnvWindTargetRange;
static_assert(sizeof(gnEnvWindTargetRange) == 4, "gnEnvWindTargetRange: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA76F8
//  The span half of the unread second wind range, partner to gnEnvWindTargetBase2: written by ENV_Init and every ENV_SetRainState preset, read by
//  nothing.
inline int32_t gnEnvWindTargetRange2;
static_assert(sizeof(gnEnvWindTargetRange2) == 4, "gnEnvWindTargetRange2: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA76FC
//  The splash pool, 512 slots of 24 bytes, created by ENV_Init. A pointer to the pool, not the pool itself.
inline D2DataArrayStrc *ghEnvSplashes;
static_assert(sizeof(ghEnvSplashes) == 4, "ghEnvSplashes: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7700
//  The bubble pool, 128 slots of 24 bytes, created by ENV_Init. A pointer to the pool, not the pool itself.
inline D2DataArrayStrc *ghEnvBubbles;
static_assert(sizeof(ghEnvBubbles) == 4, "ghEnvBubbles: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7704
//  The particle pool, 256 slots of 40 bytes, created by ENV_Init. A pointer to the pool, not the pool itself.
inline D2DataArrayStrc *ghEnvParticles;
static_assert(sizeof(ghEnvParticles) == 4, "ghEnvParticles: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7708
//  Set by ENV_TriggerLightningFlash and cleared by ENV_UpdateLightningAndThunder; while it is set a second flash is refused. The trigger's only
//  caller is slot 84 of gpSkillCltDoFuncs, the cltprgfunc1 of Thunder Storm.
inline BOOL gbEnvLightningActive;
static_assert(sizeof(gbEnvLightningActive) == 4, "gbEnvLightningActive: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA770C
//  The last GAME_GetFrameCount value ENV_UpdateWeather ran its per-frame work for: the body runs only when the frame count is above it, so calling
//  ENV_UpdateWeather twice in one frame does the work once. Zeroed by ENV_Init.
inline uint32_t gnEnvLastWeatherFrame;
static_assert(sizeof(gnEnvLastWeatherFrame) == 4, "gnEnvLastWeatherFrame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7710
//  Whether the local player's room is drawn with line-of-sight wall culling: DUNGEON_CheckLOSDraw(pRoom) for the room PLAYER_AdvanceAnimMode last
//  initialised the player into, stored through ENV_SetPlayerRoomLosDraw once per frame. Written, never read.
inline int32_t gbPlayerRoomLosDraw;
static_assert(sizeof(gbPlayerRoomLosDraw) == 4, "gbPlayerRoomLosDraw: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7714
//  1 means the weather is snow. Written only by ENV_ResetWeather (set in act 5 when roll % 100 >= nChancePercent or bForce). ENV_SpawnParticle's
//  arm for it asserts nSize < NUM_SNOW_SHAPES, ENV_UpdateWeather sways only those particles and ENV_UpdateLightningAndThunder draws them as
//  shapes; ENV_ShouldDrawRain returns 0 on it.
inline BOOL gbEnvWeatherIsSnow;
static_assert(sizeof(gbEnvWeatherIsSnow) == 4, "gbEnvWeatherIsSnow: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7718
//  Adds 1 to the frame index ENV_SpawnParticle stores at each new particle's +16, selecting between two particle graphics. ENV_ResetWeather sets
//  it per level: 0 for Bloody Foothills / Barricade 1 / Arreat Plateau, 1 for Tundra Wastelands / Rocky Summit / Nihlathak's Temple. The same
//  switch arms decide gbEnvWeatherForced and a 0-or-25 percent skip chance. Which graphic is which was not confirmed against the cell file.
inline int32_t gnEnvParticleFrameBias;
static_assert(sizeof(gnEnvParticleFrameBias) == 4, "gnEnvParticleFrameBias: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA771C
//  Set by ENV_ResetWeather for levels whose weather is mandatory rather than chance-rolled. ENV_UpdateWeather forces the state back to 2 whenever it
//  drifts, and ENV_SetRainState refuses any state change that is not a switch to 2 while it is set. ENV_ResetWeather clears it for every act-4
//  level and for levels that fail the roll.
inline BOOL gbEnvWeatherForced;
static_assert(sizeof(gbEnvWeatherForced) == 4, "gbEnvWeatherForced: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7720
//  Read by ENV_SetRainState as the first half of its gate (!gbEnvRainStateLocked || !eState), intended to refuse any state change other than a
//  switch to 0. Nothing in the module writes it, so it is always 0 and the gate always passes.
inline BOOL gbEnvRainStateLocked;
static_assert(sizeof(gbEnvRainStateLocked) == 4, "gbEnvRainStateLocked: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7724
//  The current weather state, 0..3, written only by ENV_SetRainState (its assert names the bound NUMBER_OF_RAIN_STATES, Env.cpp:298).
//  ENV_UpdateWeather advances it 0 -> 1 -> 2 -> 3 -> 0 when a phase runs out, ramps the particle target up in state 1, down in state 3 and holds
//  it in 0 and 2: 1 and 3 are the fades of a weather phase, 2 its full strength, the state the forced levels are held in.
inline uint32_t gnEnvRainState;
static_assert(sizeof(gnEnvRainState) == 4, "gnEnvRainState: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7728
//  One-shot guard around ENV_LoadImages' first-time wind and timer seeding (gnEnvWindChangeTimer, gnEnvThunderTimer and the initial
//  gnEnvWind / gnEnvWindTarget pair). Never cleared, so that block runs once per process rather than once per level.
inline BOOL gbEnvWindInitDone;
static_assert(sizeof(gbEnvWindInitDone) == 4, "gbEnvWindInitDone: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA772C
//  Free-running frame counter ENV_UpdateWeather increments once per weather tick and feeds to FOG_10084_Sin_LUT as (gnEnvParticleSwayPhase * 512)
//  / 25, added to each particle's own phase at +24 and masked to 0x1FF: the side-to-side sway of falling snow, one period per 25 frames.
inline uint32_t gnEnvParticleSwayPhase;
static_assert(sizeof(gnEnvParticleSwayPhase) == 4, "gnEnvParticleSwayPhase: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7730
//  Frames remaining in the current weather phase, decremented once per ENV_UpdateWeather tick. At 0 the state advances (wrapping at 3), a fresh
//  duration is rolled into gnEnvPhaseFramesTotal and copied here, and ENV_SetRainState is called for the new state.
inline uint32_t gnEnvPhaseFramesLeft;
static_assert(sizeof(gnEnvPhaseFramesLeft) == 4, "gnEnvPhaseFramesLeft: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7734
//  The full length of the current weather phase in frames, the value gnEnvPhaseFramesLeft was reloaded to; ENV_UpdateWeather uses
//  (total - left) / total to fade a phase in and left / total to fade it out.
inline uint32_t gnEnvPhaseFramesTotal;
static_assert(sizeof(gnEnvPhaseFramesTotal) == 4, "gnEnvPhaseFramesTotal: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7738
//  Levels.txt Rain for the player's level as of the previous frame. ENV_UpdateWeather compares it against this frame's value, so that leaving a
//  rainy level empties ghEnvParticles and ghEnvSplashes exactly once.
inline BOOL gbEnvWasRaining;
static_assert(sizeof(gbEnvWasRaining) == 4, "gbEnvWasRaining: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA773C
//  Levels.txt Mud for the player's level as of the previous frame; the bubble half of gbEnvWasRaining. Leaving a muddy level empties ghEnvBubbles
//  once.
inline BOOL gbEnvWasMuddy;
static_assert(sizeof(gbEnvWasMuddy) == 4, "gbEnvWasMuddy: the item's 4 bytes in the .i64");

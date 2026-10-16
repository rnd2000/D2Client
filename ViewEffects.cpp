#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "CoreArchive.h"
#include "View.h"

//  ViewEffects.cpp -- one frame of view maintenance and the level ambient effects of D2Client.dll 1.10: the Arcane Sanctuary starfield, the Rocky
//  Summit parallax, the screen shake, and then the world draw. VIEWEFFECTS_EnvUpdateViewAndLevelEffects is the driver of dWall.cpp: the sole
//  caller of DWALL_EnvDrawRoomTilesLit, DWALL_EnvDrawCellShadowsAndUnits and DWALL_EnvDrawCellUnits. Address range 0x6FAB7260..0x6FAB7EE0.
//  The file name is ours: the original's name for this translation unit is not in D2Client.dll (no function of the block carries an assert).

//  The two levels with a bespoke background: 74 "Act 2 - Arcane" (LevelName "Arcane Sanctuary", LEVEL_ARCANESANCTUARY) and 120
//  "Act 5 - Mountain Top" (LevelName "Rocky Summit", LEVEL_ROCKYSUMMIT).
static_assert(LEVEL_ARCANESANCTUARY == 74,
              "cmp eax, 4Ah at 0x6FAB7331 -- Levels.txt Id 74");
static_assert(LEVEL_ROCKYSUMMIT == 120,
              "cmp eax, 78h at 0x6FAB74D2 -- Levels.txt Id 120");

//  The star tables: gnArcaneStarColors is uint8_t[256] and gArcaneStarPalette is char[8].
static const int32_t NUM_ARCANE_STARS = 256;
static const int32_t ARCANE_PALETTE_ENTRIES = 8;    // the 0..1023 step-128 ramp
static const int32_t NUM_PARALLAX_CLOUDS = 10;      // three loops, i += 4 to 0x28

//  The three view fields this function reads.
static_assert(offsetof(D2ClientViewStrc, rcView) == 0x04,
              "the guard reads [ebp+0Ch] and [ebp+10h] -- right and bottom");
static_assert(offsetof(D2ClientViewStrc, dwLayerMask) == 0x38,
              "every draw gate tests [ebp+38h]");
static_assert(sizeof(RECT) == 16,
              "so rcView.right is +0x0C and rcView.bottom +0x10");

//1.10f: D2Client.0x6FAB7260
//  One frame of view maintenance and the level ambient effects, then the world draw. Nothing at all happens while both rcView.right and
//  rcView.bottom are zero: the guard is a real short-circuit `||`.
//  The screen shake is folded into the perspective scale: only under perspective, and only when gnScreenShakeMagnitude is set, the two pixel
//  offsets are shifted left 16 into the fixed point the dungeon coordinate converter takes, converted, and added to the player's precision
//  position. DUNGEON_ClientToGameSubtileCoords rewrites both in place, which is why they are addressable locals. The offsets are unsigned: the
//  only things done to them are `shl 16` and an `add`, which are bit-identical for signed and unsigned.
//  The original reuses the two addressable slots for the shake offsets and then for the red and green channels of the star palette
//  (nShakeXOrRed, nShakeYOrGreen); the source had separate variables, so separate variables are written here.
//  The palette loop draws three random channels. The first two are calls to SEED_RollRandomNumber; the third is that function's whole body
//  written out in place (a multiply by 6AC690C5h, the multiplier of D2's LCG, and an add), and all three are written as SEED_RollRandomNumber.
//  All three colour channels clamp to 255 identically.
//  The twinkle is on a 40 ms timer, not the frame rate: `if (GetTickCount() - gnArcaneStarTick > 0x28)`, an unsigned compare (`jbe`) that
//  survives the tick wrap. The stars are drawn every frame and moved at most every 40 ms.
//  It ends in a tail call (ENV_UpdateLightningAndThunder, then a jmp to DLIGHTMAP_DrawWarpFlash); the source's last statement is a plain call.
void __fastcall VIEWEFFECTS_EnvUpdateViewAndLevelEffects(D2ClientViewStrc *pView)
{
    D2UnitStrc *pPlayer;
    int32_t nPrecisionX;
    int32_t nPrecisionY;
    int32_t nShakeX;
    int32_t nShakeY;
    uint32_t nFrameCount;
    D2UnitStrc *pUnit;
    D2ActiveRoomStrc *pRoom;
    D2SeedStrc *pSeed;
    int32_t nRamp;
    int32_t nBrightness;
    int32_t nPaletteIdx;
    int32_t nRed;
    int32_t nGreen;
    int32_t nBlue;
    int32_t i;
    uint32_t dwTickCount;
    int32_t nStarVelX;
    int32_t nStarX;

    if (!pView->rcView.right && !pView->rcView.bottom)
        return;

    D2GFX_SCALE_UpdateFactor();
    gnViewScaleFactor = D2GFX_SCALE_GetFactor();

    if (D2GFX_CheckPerspective())
    {
        pPlayer = CUNIT_GetPlayer();
        nPrecisionX = D2COMMON_UNITS_GetPrecisionX(pPlayer);
        nPrecisionY = D2COMMON_UNITS_GetPrecisionY(pPlayer);

        nShakeX = 0;
        nShakeY = 0;

        if (gnScreenShakeMagnitude)
        {
            nShakeX = gnScreenShakeOffsetX << 16;
            nShakeY = gnScreenShakeOffsetY << 16;
            D2COMMON_DUNGEON_ClientToGameSubtileCoords(&nShakeX, &nShakeY);
            nPrecisionX += nShakeX;
            nPrecisionY += nShakeY;
        }

        D2GFX_SCALE_SetPerspectiveScale(nPrecisionX, nPrecisionY);
    }

    //  The light map is rebuilt at most once per frame even if this function is entered twice.
    //  The compare is unsigned in the original (`jbe`, not `jle`). GAME_GetFrameCount answers unsigned int, which is enough for it;
    //  gnViewEffectsLastFrame is int, and the cast on it keeps the compare unsigned without a signed/unsigned mismatch.
    nFrameCount = GAME_GetFrameCount();
    if (nFrameCount > (uint32_t)gnViewEffectsLastFrame)
    {
        gnViewEffectsLastFrame = nFrameCount;
        DLIGHTMAP_UpdateForPlayer();
    }

    DLIGHTMAP_EnvUpdateAmbientLightCache();

    pUnit = CUNIT_GetPlayer();
    pRoom = D2COMMON_UNITS_GetRoom(pUnit);

    if (D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom) == LEVEL_ARCANESANCTUARY)
    {
        if (!gbArcaneStarsSeeded)
        {
            pSeed = gpEnvSeed;

            D2COMMON_SEED_InitSeed(pSeed);
            D2COMMON_SEED_InitLowSeed(pSeed, D2COMMON_SEED_GetRandomValue(gdwScreenShakeStartTime));

            //  Eight palette entries over a brightness ramp. `nRamp / 7` is a magic-multiply divide in the original, so the eight brightnesses
            //  are not evenly spaced: 128, 146, 164, 182, 201, 219, 237, 256.
            nPaletteIdx = 0;
            nRamp = 0;
            do
            {
                nBrightness = nRamp / 7 + 128;

                nRed = (int32_t)(SEED_RollRandomNumber(pSeed) & 0x3F) + nBrightness - 32;
                if (nRed > 255)
                    nRed = 255;

                nGreen = (int32_t)(SEED_RollRandomNumber(pSeed) & 0x3F) + nBrightness - 32;
                if (nGreen > 255)
                    nGreen = 255;

                nBlue = (int32_t)(SEED_RollRandomNumber(pSeed) & 0x3F) + nBrightness - 32;
                if (nBlue > 255)
                    nBlue = 255;

                gArcaneStarPalette[nPaletteIdx] = D2WIN_10034_MixRGB((uint8_t)nRed, (uint8_t)nGreen, (uint8_t)nBlue);

                nRamp += 128;
                ++nPaletteIdx;
            }
            while (nRamp < 1024);

            for (i = 0; i < NUM_ARCANE_STARS; ++i)
                VIEWEFFECTS_EnvRespawnArcaneStar(i, 0);

            gbArcaneStarsSeeded = 1;
        }

        //  Every star is a DrawLine whose two ends are the same point -- a one-pixel line standing in for a point, at full alpha.
        for (i = 0; i < NUM_ARCANE_STARS; ++i)
            D2GFX_DrawLine(gnArcaneStarX[i], gnArcaneStarY[i], gnArcaneStarX[i], gnArcaneStarY[i], gnArcaneStarColors[i], 0xFF);

        dwTickCount = GetTickCount();
        if (dwTickCount - gnArcaneStarTick > 40)
        {
            for (i = 0; i < NUM_ARCANE_STARS; ++i)
            {
                nStarVelX = gnArcaneStarVelX[i];
                nStarX = gnArcaneStarX[i];
                gnArcaneStarX[i] = nStarVelX + nStarX;

                //  A signed test (`jns`). A star that has drifted off the left edge is respawned on the right.
                if (nStarVelX + nStarX < 0)
                    VIEWEFFECTS_EnvRespawnArcaneStar(i, 1);
            }

            gnArcaneStarTick = dwTickCount;
        }
    }

    //  The room is read again rather than cached across the block above.
    if (D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom) == LEVEL_ROCKYSUMMIT)
    {
        VIEWEFFECTS_EnvDrawParallax(pUnit);
    }
    else
    {
        //  Leaving the summit frees its two images, so the parallax is paid for only while it is on screen.
        if (gpImgParallaxSummit)
        {
            ARCHIVE_CellFileFree(gpImgParallaxSummit);
            gpImgParallaxSummit = nullptr;
        }
        if (gpImgParallaxCloud)
        {
            ARCHIVE_CellFileFree(gpImgParallaxCloud);
            gpImgParallaxCloud = nullptr;
        }
    }

    //  ---- the world draw, and the order is the layering ------------------
    //
    //  gdwBitMasks[n] is 1 << n; the layer mask lives in pView->dwLayerMask and WALL2_WallAddRoomToSquareGrid sets the bits, one per list it filled.
    if (pView->dwLayerMask & gdwBitMasks[0])
        DWALL_EnvDrawRoomTilesLit(pView);

    DFLOOR_DrawFloors(pView);
    ENV_DrawSplashesAndBubbles();

    if (gdwBitMasks[3] & pView->dwLayerMask)
        DWALL_EnvDrawCellShadowsAndUnits(pView);

    //  The only one of the three with no gate, matching DWALL_EnvDrawCellUnits being the pass that always runs.
    DWALL_EnvDrawCellUnits(pView);

    if (gdwBitMasks[1] & pView->dwLayerMask)
        DFLOOR_DrawViewGridFloors(pView);

    if (gbDebugDrawLightGrid)
        DLIGHTMAP_DebugDrawLightGrid();

    //  The perspective scale set at the top is scoped to this function.
    D2GFX_SCALE_SetDefaultFactor();

    ENV_UpdateLightningAndThunder();
    DLIGHTMAP_DrawWarpFlash();
}

//  The random rolls below use the inline SEED_RollLimitedRandomNumber (one definition in Shared.cpp, as SEED_RollLimitedRandomNumberInline): for
//  nMax > 0 it takes the roll % nMax when nMax is not a power of two and roll & (nMax - 1) when it is; 0 otherwise. The original shows the runtime
//  power-of-two test at the variable sites (nMax = gnViewportRight, gnViewportBottom) and a folded plain div or and at the constant ones
//  (nMax = 5, 8). SEED_RollRandomNumber is the LCG step.

//  VIEWEFFECTS_EnvDrawParallax clears a D2GfxDataStrc of 0x48 (72) bytes; this catches a regenerated struct of another size.
static_assert(sizeof(D2GfxDataStrc) == 72,
              "memset(&pData, 0, 0x48) at 0x6FAB7813");
static_assert(offsetof(D2GfxDataStrc, pCellFile) == 0x04,
              "VIEWEFFECTS_EnvDrawParallax puts the image handle here");
static_assert(offsetof(D2GfxDataStrc, nFrame) == 0x08,
              "and the frame index here");
static_assert(sizeof(D2SeedStrc) == 8,
              "the LCG writes both halves as one qword");
static_assert(offsetof(D2UnitStrc, pSeed) == 0x20,
              "VIEWEFFECTS_UpdateScreenShake rolls on &pPlayer->pSeed");

//1.10f: D2Client.0x6FAB7590
//  Places one of the 256 Arcane Sanctuary stars: position, leftward velocity and colour, all out of the environment seed.
//      bAtRightEdge == 0   x = roll(gnViewportRight)          initial scatter
//      bAtRightEdge != 0   x = roll(8) + gnViewportRight - 1  the respawn, just off the right edge
//      y        = roll(gnViewportBottom), either way
//      velocity = -1 - roll(5), so -1..-5 pixels per step
//      colour   = gArcaneStarPalette[roll(8)]
//  Four rolls per call, and the two X arms are mutually exclusive; each roll is the SEED_RollLimitedRandomNumber inline, which the original
//  inlines here. Called 256 times to seed the field, then once per star that has drifted off the left edge, from
//  VIEWEFFECTS_EnvUpdateViewAndLevelEffects.
void __fastcall VIEWEFFECTS_EnvRespawnArcaneStar(int32_t nIndex, int32_t bAtRightEdge)
{
    D2SeedStrc *pSeed;

    pSeed = gpEnvSeed;

    if (bAtRightEdge)
        gnArcaneStarX[nIndex] = (int32_t)SEED_RollLimitedRandomNumberInline(pSeed, 8) + gnViewportRight - 1;
    else
        gnArcaneStarX[nIndex] = (int32_t)SEED_RollLimitedRandomNumberInline(pSeed, gnViewportRight);

    gnArcaneStarY[nIndex] = (int32_t)SEED_RollLimitedRandomNumberInline(pSeed, gnViewportBottom);

    gnArcaneStarVelX[nIndex] = -1 - (int32_t)SEED_RollLimitedRandomNumberInline(pSeed, 5);

    gnArcaneStarColors[nIndex] = gArcaneStarPalette[SEED_RollLimitedRandomNumberInline(pSeed, 8)];
}

//  The two image paths of the parallax, char[24] each. Both live in .rdata and nothing writes them, so a private copy is behaviourally identical to
//  reaching the original's.
static const char aParallaxSummitPath[] = "data\\global\\ui\\summit01";
static const char aParallaxCloudPath[] = "data\\global\\ui\\cloud01";

//1.10f: D2Client.0x6FAB76F0
//  The Rocky Summit parallax: four tiled backdrop layers and ten drifting clouds. The caller, VIEWEFFECTS_EnvUpdateViewAndLevelEffects, calls it
//  on level 120 only.
//  Two one-shot latches, and the second overwrites the first every time:
//      if (!(gbParallaxOriginLatched & 1)) { gbParallaxOriginLatched |= 1; gnParallaxOriginX = UNITS_GetClientCoordX(pUnit); }
//      if (!gbParallaxOriginForced) { gbParallaxOriginForced = 1; gnParallaxOriginX = 10063; }
//  Both are first-call-only and both fire on the same first call, so the player's own X never survives and the parallax origin is the constant
//  10063 for the life of the process; on the second call onward neither block runs at all. Reproduced, not fixed: whether this is a bug is not
//  decidable (a hard-coded origin for a single fixed level is plausible), but the first assignment is dead.
//  The frame index is a signed modulo 4, not a flag test: `(x & 0x80000003)` followed by a `< 0` fix-up is MSVC's signed `x % 4`. It is not the
//  whole statement: after that `% 4` the original tests the remainder again (`jns` at 0x6FAB7786) and, when it is negative, adds 4 times the
//  iteration count at once (the closed form of `while (n < 0) n += 4;`), then a second `% 4` with its fix-up. So the index is always 0..3; a
//  single `% 4` would leave -1..-3 for an offset of -2048 or less whose quotient `offset / 8 / 256` is not a multiple of 4.
//  The divisions feeding it are the same idiom: `(x + (x >> 31 & 7)) >> 3` is `x / 8` and `(x + (x >> 31 & 255)) >> 8` is `x / 256`.
//  Four layers a quarter-cycle apart: the backdrop draws frames n, n+1, n+2, n+3 at x, x+256, x+512, x+768, then the same four plus 4 one row
//  down, and -- at 800x600 only -- the same four plus 8 one row further. Each row also draws the n+3 frame at x-256 when the scroll offset is
//  positive, which is the left-hand wrap.
//  The clouds seed to 250 and respawn to 300 (it looks like a typo and is reproduced): the initial seeding rolls each cloud's Y in [0, 250) and
//  the wrap-around respawn rolls it in [0, 300). Both use a constant nMax, so both are a plain `div` without the power-of-two test.
//  X is in sixteenths of a pixel throughout: seeded over `16 * gnViewportRight`, advanced by a speed of `roll(16) + 8`, wrapped past
//  `16 * (gnViewportRight + 368)` back to -5888, and divided by 16 only at the draw. That is why the wrap constant is not a screen coordinate.
void __fastcall VIEWEFFECTS_EnvDrawParallax(D2UnitStrc *pUnit)
{
    int32_t nFrameIndex;
    int32_t nFrame1;
    int32_t nFrame2;
    int32_t nFrame3;
    int32_t nXPos;
    D2CellFileStrc *pSummitImage;
    D2SeedStrc *pCloudSeed;
    int32_t i;
    int32_t nCloudXNext;
    int32_t nWrapX;
    int32_t nCloudX;
    D2GfxDataStrc pData;
    char pszName[260];

    if (gbDisconnecting)
        return;

    if ((gbParallaxOriginLatched & 1) == 0)
    {
        gbParallaxOriginLatched |= 1u;
        gnParallaxOriginX = D2COMMON_UNITS_GetClientCoordX(pUnit);
    }

    if (!gbParallaxOriginForced)
    {
        gbParallaxOriginForced = 1;
        gnParallaxOriginX = 10063;
    }

    gnParallaxOffsetX = D2COMMON_UNITS_GetClientCoordX(pUnit) - gnParallaxOriginX;

    nFrameIndex = (gnParallaxOffsetX / 8 / 256) % 4;
    while (nFrameIndex < 0)
        nFrameIndex += 4;
    nFrameIndex %= 4;
    nXPos = -(gnParallaxOffsetX / 8 % 256);

    pSummitImage = gpImgParallaxSummit;
    if (!pSummitImage)
    {
        strcpy(pszName, aParallaxSummitPath);
        pSummitImage = ARCHIVE_CellFileLoad(pszName, 0);
        gpImgParallaxSummit = pSummitImage;
    }

    if (!gpImgParallaxCloud)
    {
        strcpy(pszName, aParallaxCloudPath);
        gpImgParallaxCloud = ARCHIVE_CellFileLoad(pszName, 0);
        pSummitImage = gpImgParallaxSummit;
    }

    memset(&pData, 0, sizeof(pData));

    if (pSummitImage)
    {
        nFrame1 = (nFrameIndex + 1) % 4;
        nFrame2 = (nFrameIndex + 2) % 4;
        nFrame3 = (nFrameIndex + 3) % 4;

        pData.pCellFile = pSummitImage;

        //  Row one: the four quarter-cycle frames, then the left-hand wrap.
        pData.nFrame = nFrameIndex;
        D2GFX_TEXTURE_CelDraw(&pData, nXPos, 256, 0xFFFFFFFF, 5, nullptr);
        pData.nFrame = nFrame1;
        D2GFX_TEXTURE_CelDraw(&pData, nXPos + 256, 256, 0xFFFFFFFF, 5, nullptr);
        pData.nFrame = nFrame2;
        D2GFX_TEXTURE_CelDraw(&pData, nXPos + 512, 256, 0xFFFFFFFF, 5, nullptr);
        pData.nFrame = nFrame3;
        D2GFX_TEXTURE_CelDraw(&pData, nXPos + 768, 256, 0xFFFFFFFF, 5, nullptr);
        if (nXPos > 0)
        {
            pData.nFrame = nFrame3;
            D2GFX_TEXTURE_CelDraw(&pData, nXPos - 256, 256, 0xFFFFFFFF, 5, nullptr);
        }

        //  Row two: the same four, plus 4.
        pData.nFrame = nFrameIndex + 4;
        D2GFX_TEXTURE_CelDraw(&pData, nXPos, 512, 0xFFFFFFFF, 5, nullptr);
        pData.nFrame = nFrame1 + 4;
        D2GFX_TEXTURE_CelDraw(&pData, nXPos + 256, 512, 0xFFFFFFFF, 5, nullptr);
        pData.nFrame = nFrame2 + 4;
        D2GFX_TEXTURE_CelDraw(&pData, nXPos + 512, 512, 0xFFFFFFFF, 5, nullptr);
        pData.nFrame = nFrame3 + 4;
        D2GFX_TEXTURE_CelDraw(&pData, nXPos + 768, 512, 0xFFFFFFFF, 5, nullptr);
        if (nXPos > 0)
        {
            pData.nFrame = nFrame3 + 4;
            D2GFX_TEXTURE_CelDraw(&pData, nXPos - 256, 512, 0xFFFFFFFF, 5, nullptr);
        }

        //  Row three exists only at 800x600 -- the taller viewport is the only one with a third band of sky to fill.
        if (D2GFX_GetResolutionMode() == D2GAMERES_800x600)
        {
            pData.nFrame = nFrameIndex + 8;
            D2GFX_TEXTURE_CelDraw(&pData, nXPos, 768, 0xFFFFFFFF, 5, nullptr);
            pData.nFrame = nFrame1 + 8;
            D2GFX_TEXTURE_CelDraw(&pData, nXPos + 256, 768, 0xFFFFFFFF, 5, nullptr);
            pData.nFrame = nFrame2 + 8;
            D2GFX_TEXTURE_CelDraw(&pData, nXPos + 512, 768, 0xFFFFFFFF, 5, nullptr);
            pData.nFrame = nFrame3 + 8;
            D2GFX_TEXTURE_CelDraw(&pData, nXPos + 768, 768, 0xFFFFFFFF, 5, nullptr);
            if (nXPos > 0)
            {
                pData.nFrame = nFrame3 + 8;
                D2GFX_TEXTURE_CelDraw(&pData, nXPos - 256, 768, 0xFFFFFFFF, 5, nullptr);
            }
        }
    }

    if (!gpImgParallaxCloud)
        return;

    if (!gbParallaxCloudsSeeded)
    {
        pCloudSeed = gpParallaxCloudSeed;

        D2COMMON_SEED_InitSeed(pCloudSeed);
        D2COMMON_SEED_InitLowSeed(pCloudSeed, D2COMMON_SEED_GetRandomValue(GetTickCount()));

        for (i = 0; i < NUM_PARALLAX_CLOUDS; ++i)
        {
            gnParallaxCloudX[i] = (int32_t)SEED_RollLimitedRandomNumberInline(pCloudSeed, 16 * gnViewportRight);
            gnParallaxCloudY[i] = (int32_t)SEED_RollLimitedRandomNumberInline(pCloudSeed, 250);
            gnParallaxCloudSpeedX[i] = (int32_t)SEED_RollLimitedRandomNumberInline(pCloudSeed, 16) + 8;
        }

        gbParallaxCloudsSeeded = 1;
    }

    for (i = 0; i < NUM_PARALLAX_CLOUDS; ++i)
    {
        nCloudXNext = gnParallaxCloudSpeedX[i] + gnParallaxCloudX[i];
        nWrapX = 16 * (gnViewportRight + 368);
        gnParallaxCloudX[i] = nCloudXNext;

        if (nCloudXNext > nWrapX)
        {
            gnParallaxCloudX[i] = -5888;
            gnParallaxCloudY[i] = (int32_t)SEED_RollLimitedRandomNumberInline(gpParallaxCloudSeed, 300);
        }
    }

    for (i = 0; i < NUM_PARALLAX_CLOUDS; ++i)
    {
        nCloudX = gnParallaxCloudX[i] / 16;

        pData.pCellFile = gpImgParallaxCloud;
        pData.nFrame = 0;
        D2GFX_TEXTURE_CelDraw(&pData, nCloudX, gnParallaxCloudY[i], 0xDDDDDDDD, 3, nullptr);
        pData.nFrame = 1;
        D2GFX_TEXTURE_CelDraw(&pData, nCloudX + 256, gnParallaxCloudY[i], 0xDDDDDDDD, 3, nullptr);
    }
}

//1.10f: D2Client.0x6FAB7C50
//  One frame of screen shake, folded into the caller's camera offset. The caller, GAME_DrawGameScene, passes the two camera offsets by address.
//  The envelope is unsigned: ramp up, hold, ramp down. Every comparison against the elapsed time is `jbe` and the two ramp divisions are `div`,
//  not `idiv`, so the phase durations and the elapsed tick delta are unsigned in the original.
//  Past the whole envelope it stops the sound, zeroes the three state globals and returns without touching the caller's offsets.
//  The scatter is short by one on each axis: nShakeSpan = 2 * nMagnitude; offset = roll(nShakeSpan) - nMagnitude; so the offset lands in
//  -n .. +n-1, not the symmetric -n .. +n. Reproduced; whether it was meant is not decidable, and one pixel of bias on a shake is not something
//  a player could see.
//  The roll is the SEED_RollLimitedRandomNumber inline, here on the player's own seed rather than the environment's, so a screen shake advances
//  the player's RNG.
void __fastcall VIEWEFFECTS_UpdateScreenShake(int32_t *pViewOffsetX, int32_t *pViewOffsetY)
{
    uint32_t dwElapsed;
    int32_t nMagnitude;
    D2UnitStrc *pPlayer;
    int32_t nShakeSpan;

    dwElapsed = GetTickCount() - gdwScreenShakeStartTime;
    nMagnitude = 0;

    if (dwElapsed > gnScreenShakeRampUpMs + gnScreenShakeRampDownMs + gnScreenShakeHoldMs)
    {
        VIEWEFFECTS_UpdateScreenShakeSound(0);
        gnScreenShakeMagnitude = 0;
        gnScreenShakeOffsetX = 0;
        gnScreenShakeOffsetY = 0;
        return;
    }

    if (dwElapsed >= gnScreenShakeRampUpMs)
    {
        if (dwElapsed < gnScreenShakeHoldMs + gnScreenShakeRampUpMs)
        {
            //  The hold phase: full magnitude, no interpolation.
            nMagnitude = gnScreenShakeMagnitude;
        }
        else if (gnScreenShakeRampDownMs)
        {
            nMagnitude = gnScreenShakeMagnitude * (gnScreenShakeRampUpMs + gnScreenShakeHoldMs + gnScreenShakeRampDownMs - dwElapsed) / gnScreenShakeRampDownMs;
        }
    }
    else if (gnScreenShakeRampUpMs)
    {
        nMagnitude = dwElapsed * gnScreenShakeMagnitude / gnScreenShakeRampUpMs;
    }

    if (nMagnitude)
    {
        pPlayer = CUNIT_GetPlayer();
        nShakeSpan = 2 * nMagnitude;

        gnScreenShakeOffsetX = SEED_RollLimitedRandomNumberInline(&pPlayer->pSeed, nShakeSpan) - nMagnitude;
        gnScreenShakeOffsetY = SEED_RollLimitedRandomNumberInline(&pPlayer->pSeed, nShakeSpan) - nMagnitude;

        *pViewOffsetX += gnScreenShakeOffsetX;
        *pViewOffsetY += gnScreenShakeOffsetY;
    }

    VIEWEFFECTS_UpdateScreenShakeSound(nMagnitude);
}

//1.10f: D2Client.0x6FAB7DC0
//  Keeps the rumble sound in step with the shake. Sound id 452, started lazily and muted, then ramped toward the target volume by at most six per
//  call in either direction -- a smooth fade rather than a hard set.
//  The target is `255 * nMagnitude / 20` clamped to 0..255; the clamp is written as three branches rather than a min/max pair, with the same
//  division computed twice. Both slots are kept because both are in the original.
//  When the ramp arrives at zero it does not set the volume to zero: it calls SOUNDINST_MarkStarted and drops the handle, letting the instance
//  release itself.
//  The control flow is a knot of gotos in the original and is written here as the structure it implements; the double division is not tidied,
//  because that is arithmetic rather than shape.
void __fastcall VIEWEFFECTS_UpdateScreenShakeSound(int32_t nMagnitude)
{
    int32_t nTargetRaw;
    int32_t nTargetVolume;
    int32_t hSound;
    int32_t nCurrentVolume;
    int32_t nVolume;

    nTargetRaw = 255 * nMagnitude / 20;

    if (nTargetRaw <= 0)
        nTargetVolume = 0;
    else if (nTargetRaw < 255)
        nTargetVolume = 255 * nMagnitude / 20;   //  computed a second time
    else
        nTargetVolume = 255;

    if (nTargetVolume)
    {
        //  Lazily start the sound, muted, the first time a shake needs it.
        if (!ghScreenShakeSound)
        {
            hSound = SOUNDINST_PlaySound(SOUND_ANDARIEL_QUAKE_LOOP, nullptr, 0, 0, 0);
            ghScreenShakeSound = hSound;
            if (!hSound)
                return;
            SOUNDINST_SetVolumeByHandle(hSound, 0);
        }
    }

    hSound = ghScreenShakeSound;
    if (!hSound)
        return;

    nCurrentVolume = SOUNDINST_GetInstanceVolume(hSound);
    if (nTargetVolume == nCurrentVolume)
        return;

    if (nTargetVolume <= nCurrentVolume)
    {
        nVolume = nCurrentVolume - 6;
        if (nTargetVolume > nVolume)
            nVolume = nTargetVolume;
    }
    else
    {
        nVolume = nCurrentVolume + 6;
        if (nTargetVolume < nVolume)
            nVolume = nTargetVolume;
    }

    if (nVolume)
    {
        SOUNDINST_SetVolumeByHandle(ghScreenShakeSound, nVolume);
    }
    else
    {
        SOUNDINST_MarkStarted(ghScreenShakeSound);
        ghScreenShakeSound = 0;
    }
}

//1.10f: D2Client.0x6FAB7E70
//  Arms a screen shake. A zero hold is a no-op: the whole body is inside `if (nHoldMs)`, so a caller asking for ramp-up and ramp-down with no hold
//  gets nothing at all, not even a start time.
void __fastcall VIEWEFFECTS_StartScreenShake(int32_t nMagnitude, int32_t nRampUpMs, int32_t nHoldMs, int32_t nRampDownMs)
{
    if (nHoldMs)
    {
        gdwScreenShakeStartTime = GetTickCount();
        gnScreenShakeRampUpMs = nRampUpMs;
        gnScreenShakeHoldMs = nHoldMs;
        gnScreenShakeRampDownMs = nRampDownMs;
        gnScreenShakeMagnitude = nMagnitude;
    }
}

//1.10f: D2Client.0x6FAB7EB0
//  Reads the current shake offset, and says whether there is one. When the magnitude is zero it returns 0 and leaves both out-params untouched,
//  which is why the caller (SCROLL_UpdateViewportOrigin) must check the return rather than the values.
int32_t __fastcall VIEWEFFECTS_GetScreenShakeOffset(int32_t *pOffsetX, int32_t *pOffsetY)
{
    if (!gnScreenShakeMagnitude)
        return 0;

    *pOffsetX = gnScreenShakeOffsetX;
    *pOffsetY = gnScreenShakeOffsetY;
    return 1;
}

//1.10f: D2Client.0x6FAB7EE0
//  Cancels a shake in progress: the same three-way zero VIEWEFFECTS_UpdateScreenShake does on its own expiry path, but without stopping the sound.
void __cdecl VIEWEFFECTS_ResetScreenShake()
{
    gnScreenShakeMagnitude = 0;
    gnScreenShakeOffsetX = 0;
    gnScreenShakeOffsetY = 0;
}

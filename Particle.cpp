#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"

//  Particle.cpp -- the unit particles of D2Client.dll 1.10: a unit's particle block (a position, a velocity, an acceleration, a point the motion
//  ends at, the screen offsets the drawing adds, and a bounce or a countdown) allocated, freed, read and set, and stepped once a frame.
//  The original source file is ENGINE\Particle.cpp (its asserts name the path, ENGINE in capitals) and its assert text is "ptParticle".
//  Address range 0x6FB5FB10..0x6FB6019F.
//
//  The block, D2ClientParticleStrc, is 76 bytes and hangs at +30h of the unit's gfx info (D2ClientUnitGfxInfoStrc). GFX_GetUnitParticles answers
//  it, NULL for a unit with no gfx info, and GFX_SetUnitParticles stores into it, storing nothing for a unit with no gfx info. Its fields:
//      +00  dwFlags          the flags below
//      +04  nX nY nZ         the position, game pixels scaled by 2048 (<< 11)
//      +10  nVelX ..         the velocity, added to the position each frame
//      +1C  nAccelX ..       the acceleration, added to the velocity each frame
//      +28  nTargetX ..      where the motion ends, UNSCALED: the step compares it with the position >> 11 and snaps the position to it << 11;
//                            nTargetZ is also the floor a bounce reverses at
//      +34  nOffsetX ..      the screen offsets the drawing adds: X and Y are the position >> 11 through DUNGEON_GameToClientCoords, Z is -(nZ >> 11)
//      +40  nBouncesLeft     bounces before the motion ends
//      +44  nRestitution     the bounce's percentage: the Z velocity becomes nVelZ * nRestitution / -100
//      +48  nDurationFrames  frames a timed motion has left
//  The flags, named by their values:
//      1     the motion has ended; the step returns at once
//      2     timed: nDurationFrames counts down, and at 0 the 1 is set
//      4     bouncing off nTargetZ (PARTICLE_StartUnitBounce)
//      8     not read by the step; PARTICLE_SetUnitParticleFlag sets it as it clears the 1, and PARTICLE_IsUnitMotionActive reads it
//      10h   attached: positioned from the owner's cel, not from its own position (PARTICLE_SetUnitAttached)
//      20h   the arrival test reversed: the motion ends when the position is at or ABOVE the target on all three axes, else at or below
//  A shift of a negative value is defined here: the project compiles as C++20, where << is the value modulo 2^32 (the original's shl) and >> of a
//  negative int is arithmetic (its sar).

//  The path literal this unit passes and its assert text.
static const char szParticleCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\ENGINE\\Particle.cpp";

//  Layout checks for the block's fields the first sixteen functions read and write. Its size, 76, is asserted in D2Structs.h.
static_assert(offsetof(D2ClientParticleStrc, dwFlags) == 0 && offsetof(D2ClientParticleStrc, nX) == 4
              && offsetof(D2ClientParticleStrc, nY) == 8 && offsetof(D2ClientParticleStrc, nZ) == 0x0C,
              "PARTICLE_GetUnitGfxCoordX/Y/Z: `mov eax, [eax+4]` 0x6FB5FC0D, `[eax+8]` 0x6FB5FC2D, `[eax+0Ch]` 0x6FB5FC4D; "
              "PARTICLE_StartUnitBounce: `mov ecx, [eax]` 0x6FB5FDAD");
static_assert(offsetof(D2ClientParticleStrc, nVelX) == 0x10 && offsetof(D2ClientParticleStrc, nVelY) == 0x14
              && offsetof(D2ClientParticleStrc, nVelZ) == 0x18 && offsetof(D2ClientParticleStrc, nAccelX) == 0x1C
              && offsetof(D2ClientParticleStrc, nAccelY) == 0x20 && offsetof(D2ClientParticleStrc, nAccelZ) == 0x24,
              "PARTICLE_SetUnitGfxVector: `mov [eax+10h], ecx` 0x6FB5FCD0, `[eax+14h]` 0x6FB5FCDD, `[eax+18h]` 0x6FB5FCE0; "
              "PARTICLE_SetUnitGfxVector2: `[eax+1Ch]` 0x6FB5FD20, `[eax+20h]` 0x6FB5FD2D, `[eax+24h]` 0x6FB5FD30");
static_assert(offsetof(D2ClientParticleStrc, nTargetX) == 0x28 && offsetof(D2ClientParticleStrc, nTargetY) == 0x2C
              && offsetof(D2ClientParticleStrc, nTargetZ) == 0x30 && offsetof(D2ClientParticleStrc, nOffsetX) == 0x34
              && offsetof(D2ClientParticleStrc, nOffsetY) == 0x38 && offsetof(D2ClientParticleStrc, nOffsetZ) == 0x3C,
              "PARTICLE_SetUnitParticleVector: `mov [eax+28h], ecx` 0x6FB5FD70, `[eax+2Ch]` 0x6FB5FD7D, `[eax+30h]` 0x6FB5FD80; "
              "PARTICLE_GetUnitParticleOffsetX/Y/Z: `mov eax, [eax+34h]` 0x6FB5FBAD, `[eax+38h]` 0x6FB5FBCD, `[eax+3Ch]` 0x6FB5FBED");
static_assert(offsetof(D2ClientParticleStrc, nBouncesLeft) == 0x40 && offsetof(D2ClientParticleStrc, nRestitution) == 0x44,
              "PARTICLE_StartUnitBounce: `mov [eax+44h], edx` 0x6FB5FDB6, `mov [eax+40h], ecx` 0x6FB5FDBF");

//  Layout check: the countdown field.
static_assert(offsetof(D2ClientParticleStrc, nDurationFrames) == 0x48,
              "PARTICLE_UpdateUnitParticle: `mov ecx, [esi+48h]` 0x6FB5FE67; PARTICLE_StartUnitArc: `mov [ecx+48h], eax` 0x6FB6009D");


//1.10f: D2Client.0x6FB5FB10
//  Gives a unit its particle block, cleared: the one it has (a reset), or a new one -- FOG_Alloc(76, this file, 72, 0), asserting ptParticle at
//  line 74 and exiting if that fails -- then all 76 bytes zeroed (rep stosd, 19 dwords) and handed to GFX_SetUnitParticles. The fourth FOG_Alloc
//  argument is push eax, with eax the NULL GFX_GetUnitParticles just answered, so 0.
//  BUG: on a unit with no gfx info (a NULL unit, or one whose pGfxData is NULL) GFX_GetUnitParticles answers NULL, so a block is allocated and
//  cleared and GFX_SetUnitParticles stores it nowhere: the 76 bytes leak, and the caller's next PARTICLE_ call finds no block. LATENT: only a NULL
//  unit or a tile has no gfx record, and no caller passes either.
void __stdcall PARTICLE_AllocUnitParticles(D2UnitStrc *pUnit)
{
    D2ClientParticleStrc *pParticle;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (!pParticle)
    {
        pParticle = (D2ClientParticleStrc *)FOG_Alloc(sizeof(D2ClientParticleStrc), szParticleCpp, 72, 0);
        if (!pParticle)
        {
            FOG_DisplayAssert("ptParticle", szParticleCpp, 74);
            exit(-1);
        }
    }

    memset(pParticle, 0, sizeof(D2ClientParticleStrc));
    GFX_SetUnitParticles(pUnit, pParticle);
}


//1.10f: D2Client.0x6FB5FB70
//  Frees a unit's particle block and clears the pointer to it: FOG_Free(block, this file, 100, 0), then GFX_SetUnitParticles(pUnit, NULL). A unit
//  with no block is left alone.
void __stdcall PARTICLE_FreeUnitParticles(D2UnitStrc *pUnit)
{
    D2ClientParticleStrc *pParticle;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (pParticle)
    {
        FOG_Free(pParticle, szParticleCpp, 100, 0);
        GFX_SetUnitParticles(pUnit, NULL);
    }
}


//1.10f: D2Client.0x6FB5FBA0
//  The screen X offset the step last derived (+34h), or 0 with no block.
int32_t __stdcall PARTICLE_GetUnitParticleOffsetX(D2UnitStrc *pUnit)
{
    D2ClientParticleStrc *pParticle;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (!pParticle)
        return 0;

    return pParticle->nOffsetX;
}


//1.10f: D2Client.0x6FB5FBC0
//  The screen Y offset (+38h), or 0 with no block.
int32_t __stdcall PARTICLE_GetUnitParticleOffsetY(D2UnitStrc *pUnit)
{
    D2ClientParticleStrc *pParticle;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (!pParticle)
        return 0;

    return pParticle->nOffsetY;
}


//1.10f: D2Client.0x6FB5FBE0
//  The screen Z offset (+3Ch), -(nZ >> 11) as the step stores it, or 0 with no block.
int32_t __stdcall PARTICLE_GetUnitParticleOffsetZ(D2UnitStrc *pUnit)
{
    D2ClientParticleStrc *pParticle;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (!pParticle)
        return 0;

    return pParticle->nOffsetZ;
}


//1.10f: D2Client.0x6FB5FC00
//  The position's X in game pixels: nX sar 11, or 0 with no block.
int32_t __stdcall PARTICLE_GetUnitGfxCoordX(D2UnitStrc *pUnit)
{
    D2ClientParticleStrc *pParticle;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (!pParticle)
        return 0;

    return pParticle->nX >> 11;
}


//1.10f: D2Client.0x6FB5FC20
//  The position's Y in game pixels: nY sar 11, or 0.
int32_t __stdcall PARTICLE_GetUnitGfxCoordY(D2UnitStrc *pUnit)
{
    D2ClientParticleStrc *pParticle;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (!pParticle)
        return 0;

    return pParticle->nY >> 11;
}


//1.10f: D2Client.0x6FB5FC40
//  The position's height: nZ sar 11, or 0.
int32_t __stdcall PARTICLE_GetUnitGfxCoordZ(D2UnitStrc *pUnit)
{
    D2ClientParticleStrc *pParticle;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (!pParticle)
        return 0;

    return pParticle->nZ >> 11;
}


//1.10f: D2Client.0x6FB5FC60
//  The height unshifted: the same field as PARTICLE_GetUnitGfxCoordZ, nZ (+0Ch), as stored, or 0.
int32_t __stdcall PARTICLE_GetUnitGfxCoordZRaw(D2UnitStrc *pUnit)
{
    D2ClientParticleStrc *pParticle;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (!pParticle)
        return 0;

    return pParticle->nZ;
}


//1.10f: D2Client.0x6FB5FC80
//  Sets the position in game pixels, each shl 11, and answers the block -- NULL, and nothing stored, with none.
D2ClientParticleStrc *__stdcall PARTICLE_SetUnitGfxCoords(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nZ)
{
    D2ClientParticleStrc *pParticle;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (pParticle)
    {
        pParticle->nX = nX << 11;
        pParticle->nY = nY << 11;
        pParticle->nZ = nZ << 11;
    }

    return pParticle;
}


//1.10f: D2Client.0x6FB5FCB0
//  Sets the velocity (+10h..+18h): game pixels a frame, shl 11 each when bScale is set, stored as given when it is not. Answers the block, or NULL.
D2ClientParticleStrc *__stdcall PARTICLE_SetUnitGfxVector(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nZ, int32_t bScale)
{
    D2ClientParticleStrc *pParticle;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (pParticle)
    {
        if (bScale)
        {
            pParticle->nVelX = nX << 11;
            pParticle->nVelY = nY << 11;
            pParticle->nVelZ = nZ << 11;
        }
        else
        {
            pParticle->nVelX = nX;
            pParticle->nVelY = nY;
            pParticle->nVelZ = nZ;
        }
    }

    return pParticle;
}


//1.10f: D2Client.0x6FB5FD00
//  Sets the acceleration (+1Ch..+24h), the same shape as the velocity's setter; the step adds these three to the velocity each frame.
D2ClientParticleStrc *__stdcall PARTICLE_SetUnitGfxVector2(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nZ, int32_t bScale)
{
    D2ClientParticleStrc *pParticle;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (pParticle)
    {
        if (bScale)
        {
            pParticle->nAccelX = nX << 11;
            pParticle->nAccelY = nY << 11;
            pParticle->nAccelZ = nZ << 11;
        }
        else
        {
            pParticle->nAccelX = nX;
            pParticle->nAccelY = nY;
            pParticle->nAccelZ = nZ;
        }
    }

    return pParticle;
}


//1.10f: D2Client.0x6FB5FD50
//  Sets the target (+28h..+30h), the same shape again: shl 11 each when bFixedPoint is set, as given when not. The step keeps the target
//  UNSCALED, so 0 is the flag that fits it.
//  BUG (in a caller): MONSTER_EnvTickWorldstoneChips passes bFixedPoint 1 for a rising chip's 300 and a falling one's -100, which the step then
//  holds as 614400 and -204800 game pixels, 2048 times out. LATENT: with no arc and 60 frames a chip ends short of even the unshifted 300 or -100.
D2ClientParticleStrc *__stdcall PARTICLE_SetUnitParticleVector(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nZ, int32_t bFixedPoint)
{
    D2ClientParticleStrc *pParticle;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (pParticle)
    {
        if (bFixedPoint)
        {
            pParticle->nTargetX = nX << 11;
            pParticle->nTargetY = nY << 11;
            pParticle->nTargetZ = nZ << 11;
        }
        else
        {
            pParticle->nTargetX = nX;
            pParticle->nTargetY = nY;
            pParticle->nTargetZ = nZ;
        }
    }

    return pParticle;
}


//1.10f: D2Client.0x6FB5FDA0
//  Starts a bounce: the 4 flag set, the bounces left (+40h) and the restitution percentage (+44h) stored. The bytes store the restitution, then
//  the flags, then the count: the order is the compiler's.
D2ClientParticleStrc *__stdcall PARTICLE_StartUnitBounce(D2UnitStrc *pUnit, int32_t nBouncesLeft, int32_t nRestitution)
{
    D2ClientParticleStrc *pParticle;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (pParticle)
    {
        pParticle->dwFlags |= 4;
        pParticle->nBouncesLeft = nBouncesLeft;
        pParticle->nRestitution = nRestitution;
    }

    return pParticle;
}


//1.10f: D2Client.0x6FB5FDD0
//  The bounces left (+40h), or 0 with no block.
int32_t __stdcall PARTICLE_GetUnitBouncesLeft(D2UnitStrc *pUnit)
{
    D2ClientParticleStrc *pParticle;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (!pParticle)
        return 0;

    return pParticle->nBouncesLeft;
}


//1.10f: D2Client.0x6FB5FDF0
//  Ends the countdown: the 2 flag cleared, so nDurationFrames is no longer counted and the motion no longer ends on it. __fastcall with no load
//  of its own: ecx, the unit, goes straight on to GFX_GetUnitParticles, and the retn is bare.
D2ClientParticleStrc *__fastcall PARTICLE_ClearUnitTimedMotion(D2UnitStrc *pUnit)
{
    D2ClientParticleStrc *pParticle;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (pParticle)
        pParticle->dwFlags &= ~2u;

    return pParticle;
}


//1.10f: D2Client.0x6FB5FE00
//  The per-frame step. Nothing for a unit with no block, or for a motion that has ended (the 1). Otherwise each axis's position takes the velocity
//  and the velocity the acceleration, the position the OLD velocity. Then:
//  * timed (the 2): nDurationFrames counts down; at 0 the motion ends: the 1 set and nX and nY ZEROED, nZ kept.
//  * bouncing (the 4), the flags read again: when the height >> 11 is at or below nTargetZ, the Z velocity just computed becomes
//    nRestitution * nVelZ / -100 (imul by 0AE147AE1h, sar edx, 5, the sign added), and a bounce is spent, or with none left the motion ends.
//  * attached (the 10h): placed from its owner (CUNIT_GetOwnerUnit), nothing with no owner. A monster takes the owner's COMPOSIT_SPECIAL7 cel,
//    anything else COMPOSIT_SPECIAL1, and a missile SPECIAL7 too after two checks of the owner: a FOG_DisplayWarning, line 425,
//    "UnitGetType (hSourceUnit) == UNIT_MONSTER", which logs and goes on (a warning, not an assert), and a RETURN for an owner in MONMODE_DEATH or
//    MONMODE_DEAD, read whatever the owner is. That return skips the offsets below, as their own 10h test would. GFX_GetUnitCel writes the cel's
//    D2CMP offsets into two locals that start at 0: X becomes nOffsetX and Y nOffsetZ, +10 for a missile; the owner's own two offsets are added
//    (its block read twice, 0 with none); then nX and nY are set from nOffsetX and nOffsetY through DUNGEON_ClientToGameSubtileCoords (#10108,
//    sar 5 of 2y +/- x) and nZ to -(nOffsetZ << 11). The `pUnit ?` tests are dead, a NULL unit having no block and left at the top, and are
//    written as the bytes have them.
//  * otherwise the arrival: with no 20h, the motion ends when the position >> 11 is at or below the target on all three axes; with it, at or
//    above. The bytes test the 20h a second time after the first chain fails, which is the else if below. Arrived, the position is snapped to
//    the target << 11 and the 1 set.
//  Last, unless attached, the screen offsets: nX and nY >> 11 through DUNGEON_GameToClientCoords (#10112, sar 1 of x - y and sar 2 of x + y),
//  nOffsetZ -(nZ >> 11).
void __stdcall PARTICLE_UpdateUnitParticle(D2UnitStrc *pUnit)
{
    D2ClientParticleStrc *pParticle;
    D2ClientParticleStrc *pOwnerParticles;
    D2UnitStrc *pOwner;
    D2C_UnitTypes nUnitType;
    D2Composits nComponent;
    uint32_t dwFlags;
    uint32_t dwRising;
    int32_t nNewVelZ;
    int32_t nTargetZ;
    int32_t nCelOffsetX;
    int32_t nCelOffsetY;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (!pParticle)
        return;

    dwFlags = pParticle->dwFlags;
    if (dwFlags & 1)
        return;

    pParticle->nX += pParticle->nVelX;
    pParticle->nVelX += pParticle->nAccelX;
    pParticle->nY += pParticle->nVelY;
    pParticle->nVelY += pParticle->nAccelY;
    pParticle->nZ += pParticle->nVelZ;
    nNewVelZ = pParticle->nAccelZ + pParticle->nVelZ;
    pParticle->nVelZ = nNewVelZ;

    if (dwFlags & 2)
    {
        if (pParticle->nDurationFrames)
        {
            --pParticle->nDurationFrames;
        }
        else
        {
            pParticle->nX = 0;
            pParticle->dwFlags = dwFlags | 1;
            pParticle->nY = 0;
        }
    }

    dwFlags = pParticle->dwFlags;
    if (dwFlags & 4)
    {
        nTargetZ = pParticle->nTargetZ;
        if ((pParticle->nZ >> 11) <= nTargetZ)
        {
            //  BUG: the bounce's floor is stored into nZ unscaled, where the compare above shifts nZ down and the arrival snap below shifts the
            //  target up. LATENT: the two agree for a floor of 0, and the game's one bounce, the Druid's molten boulder, has a floor of 0.
            pParticle->nZ = nTargetZ;
            pParticle->nVelZ = pParticle->nRestitution * nNewVelZ / -100;
            if (pParticle->nBouncesLeft)
                --pParticle->nBouncesLeft;
            else
                pParticle->dwFlags = dwFlags | 1;
        }
    }
    else if (dwFlags & 0x10)
    {
        pOwner = CUNIT_GetOwnerUnit(pUnit);
        if (pOwner)
        {
            nCelOffsetX = 0;
            nCelOffsetY = 0;
            nComponent = COMPOSIT_SPECIAL1;
            nUnitType = pUnit ? pUnit->dwUnitType : UNIT_TYPES_COUNT;
            if (nUnitType == UNIT_MONSTER)
            {
                nComponent = COMPOSIT_SPECIAL7;
            }
            else if (nUnitType == UNIT_MISSILE)
            {
                if (pOwner->dwUnitType != UNIT_MONSTER)
                    FOG_DisplayWarning("UnitGetType (hSourceUnit) == UNIT_MONSTER", szParticleCpp, 425);

                if (pOwner->dwAnimMode == MONMODE_DEATH || pOwner->dwAnimMode == MONMODE_DEAD)
                    return;

                nComponent = COMPOSIT_SPECIAL7;
            }

            GFX_GetUnitCel(pOwner, nComponent, &nCelOffsetX, &nCelOffsetY);
            pParticle->nOffsetX = nCelOffsetX;
            pParticle->nOffsetZ = nCelOffsetY;
            if (pUnit && pUnit->dwUnitType == UNIT_MISSILE)
                pParticle->nOffsetZ = nCelOffsetY + 10;

            pOwnerParticles = GFX_GetUnitParticles(pOwner);
            pParticle->nOffsetX += pOwnerParticles ? pOwnerParticles->nOffsetX : 0;
            pOwnerParticles = GFX_GetUnitParticles(pOwner);
            pParticle->nOffsetZ += pOwnerParticles ? pOwnerParticles->nOffsetZ : 0;

            //  BUG: nX and nY are set to whole subtiles, unscaled, where every other writer keeps game pixels << 11, 65536 times apart.
            //  UNREACHABLE in effect: the step does not read them back for an attached block, and neither reader lets the value out
            //  (MISSILE_ClientOnHit's copy never runs for a ball, and GFX_DrawUnitCelShadow adds the subtiles and then draws nothing).
            pParticle->nX = pParticle->nOffsetX;
            pParticle->nY = pParticle->nOffsetY;
            D2COMMON_DUNGEON_ClientToGameSubtileCoords(&pParticle->nX, &pParticle->nY);
            pParticle->nZ = -(pParticle->nOffsetZ << 11);
        }
    }
    else
    {
        dwRising = dwFlags & 0x20;
        if (!dwRising && (pParticle->nX >> 11) <= pParticle->nTargetX && (pParticle->nY >> 11) <= pParticle->nTargetY && (pParticle->nZ >> 11) <= pParticle->nTargetZ)
        {
            pParticle->nX = pParticle->nTargetX << 11;
            pParticle->nY = pParticle->nTargetY << 11;
            pParticle->nZ = pParticle->nTargetZ << 11;
            pParticle->dwFlags = dwFlags | 1;
        }
        else if (dwRising && (pParticle->nX >> 11) >= pParticle->nTargetX && (pParticle->nY >> 11) >= pParticle->nTargetY && (pParticle->nZ >> 11) >= pParticle->nTargetZ)
        {
            pParticle->nX = pParticle->nTargetX << 11;
            pParticle->nY = pParticle->nTargetY << 11;
            pParticle->nZ = pParticle->nTargetZ << 11;
            pParticle->dwFlags = dwFlags | 1;
        }
    }

    if (!(pParticle->dwFlags & 0x10))
    {
        pParticle->nOffsetX = pParticle->nX >> 11;
        pParticle->nOffsetY = pParticle->nY >> 11;
        D2COMMON_DUNGEON_GameToClientCoords(&pParticle->nOffsetX, &pParticle->nOffsetY);
        pParticle->nOffsetZ = -(pParticle->nZ >> 11);
    }
}


//1.10f: D2Client.0x6FB60080
//  Starts a vertical arc: the 2 set; nDurationFrames, floored at 1 (signed); the position at (0, 0, nStartHeight << 11) and the X and Y
//  velocities 0; an acceleration of -4096 if it has none (-2 game pixels a frame per frame); and the Z velocity solved so the arc comes down where
//  it started from 0: (-z0 - a*n*n/2) / n, both divisions signed. By the step's order (the position takes the old velocity, and the countdown ends
//  the motion on its n+1th step) an arc from 0 lands exactly at 0 on its last step, and one from z0 at -z0/n, the integer divisions aside.
//  nStartHeight is the start height: the body stores it as the position's Z, and with a positive velocity the arc rises above it. void: eax
//  leaves holding the Z velocity and no caller reads it.
void __fastcall PARTICLE_StartUnitArc(D2UnitStrc *pUnit, int32_t nStartHeight, int32_t nDurationFrames)
{
    D2ClientParticleStrc *pParticle;
    int32_t nAccelZ;
    int32_t nZStart;
    int32_t nDuration;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (!pParticle)
        return;

    pParticle->dwFlags |= 2;
    pParticle->nDurationFrames = nDurationFrames;
    if (nDurationFrames < 1)
        pParticle->nDurationFrames = 1;

    nAccelZ = pParticle->nAccelZ;
    pParticle->nX = 0;
    nZStart = nStartHeight << 11;
    pParticle->nY = 0;
    pParticle->nZ = nZStart;
    if (!nAccelZ)
        pParticle->nAccelZ = -4096;

    nDuration = pParticle->nDurationFrames;
    pParticle->nVelX = 0;
    pParticle->nVelY = 0;
    pParticle->nVelZ = (-nZStart - pParticle->nAccelZ * nDuration * nDuration / 2) / nDuration;
}


//1.10f: D2Client.0x6FB600F0
//  Sets or clears the 10h, attached, by bAttached, and answers the block. __fastcall: the flag is kept in esi across GFX_GetUnitParticles.
D2ClientParticleStrc *__fastcall PARTICLE_SetUnitAttached(D2UnitStrc *pUnit, int32_t bAttached)
{
    D2ClientParticleStrc *pParticle;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (pParticle)
    {
        if (bAttached)
            pParticle->dwFlags |= 0x10;
        else
            pParticle->dwFlags &= ~0x10u;
    }

    return pParticle;
}


//1.10f: D2Client.0x6FB60110
//  Ends the motion: the 1 set.
D2ClientParticleStrc *__stdcall PARTICLE_StopUnitMotion(D2UnitStrc *pUnit)
{
    D2ClientParticleStrc *pParticle;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (pParticle)
        pParticle->dwFlags |= 1;

    return pParticle;
}


//1.10f: D2Client.0x6FB60130
//  Restarts the motion and sets the 8: the 1 cleared and the 8 set (and ecx, 0FFFFFFFEh / or ecx, 8). What the 8 means is open: the step never
//  reads it.
D2ClientParticleStrc *__stdcall PARTICLE_SetUnitParticleFlag(D2UnitStrc *pUnit)
{
    D2ClientParticleStrc *pParticle;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (pParticle)
        pParticle->dwFlags = (pParticle->dwFlags & ~1u) | 8;

    return pParticle;
}


//1.10f: D2Client.0x6FB60150
//  FALSE only for a block whose motion has ended without the 8; TRUE for no block, a running motion, or an ended one with the 8 set.
BOOL __stdcall PARTICLE_IsUnitMotionActive(D2UnitStrc *pUnit)
{
    D2ClientParticleStrc *pParticle;
    uint32_t dwFlags;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (pParticle)
    {
        dwFlags = pParticle->dwFlags;
        if ((dwFlags & 1) && !(dwFlags & 8))
            return FALSE;
    }

    return TRUE;
}


//1.10f: D2Client.0x6FB60180
//  Sets or clears the 20h by bRising, and answers the block: with it the step's arrival test is "at or above the target", without it "at or
//  below".
D2ClientParticleStrc *__fastcall PARTICLE_SetUnitRisesToTarget(D2UnitStrc *pUnit, int32_t bRising)
{
    D2ClientParticleStrc *pParticle;

    pParticle = GFX_GetUnitParticles(pUnit);
    if (pParticle)
    {
        if (bRising)
            pParticle->dwFlags |= 0x20;
        else
            pParticle->dwFlags &= ~0x20u;
    }

    return pParticle;
}

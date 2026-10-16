#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"

//  UnitGfx.cpp -- the unit graphics of D2Client.dll 1.10: UNITGFX_UnitInitGfx, which allocates a new unit's graphics record, and
//  UNITGFX_GfxDrawUnit, the per-unit draw entry point (what is skipped, the line-of-sight flag, where on screen, and which draw path).
//  The original source file is UNIT\UnitGfx.cpp (UNITGFX_UnitInitGfx's asserts name it). Address range 0x6FB4DAC0..0x6FB4DEA2.

//  The path literal UNITGFX_UnitInitGfx passes (0x6FB94050).
static const char szUnitGfxCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UNIT\\UnitGfx.cpp";

//  The numbers UNITGFX_GfxDrawUnit reads.
static_assert(STATE_INVIS == 0x92 && UNITFLAG_AUTOMAP == 0x10000000 && COLLIDE_VISIBLE == 2,
              "UNITGFX_GfxDrawUnit: `push 92h` 0x6FB4DBB6, `or dword ptr [esi+0C4h], 10000000h` 0x6FB4DD7A, `push 2` 0x6FB4DD01");
static_assert(offsetof(D2MissilesTxt, wXoffset) == 0xA2 && offsetof(D2MissilesTxt, wYoffset) == 0xA4 && offsetof(D2MissilesTxt, wZoffset) == 0xA6,
              "UNITGFX_GfxDrawUnit: the `movsx` of +0A2h 0x6FB4DDE7, of +0A4h 0x6FB4DDD6, of +0A6h 0x6FB4DDCF");


//1.10f: D2Client.0x6FB4DAC0
//  A new unit's graphics record: GFX_AllocUnitGfxInfo into pGfxData (stored before the test, 0x6FB4DACA), `ptUnit->uGfx.hGFX` asserted at line 22
//  with the exit after it, then by type -- players and objects GFX_RebuildUnitAnimForMode with 0, monsters with 1 (both tail jumps); missiles and
//  items nothing; anything else FOG_DisplayHalt "[UnitInitGFX] Invalid unit type!" at line 40 and exit. The type switch is unsigned
//  (`cmp eax, 4` / `ja`, 0x6FB4DAEC). The assert names the slot `uGfx.hGFX`; pGfxData holds the client's D2ClientUnitGfxInfoStrc *.
//  __fastcall: edx is unused and the function ends in a bare `retn`. GFX_RebuildUnitAnimForMode takes only the unit as an argument; the 0 and 1
//  go in edx (`xor edx, edx` 0x6FB4DAFA, `mov edx, 1` 0x6FB4DB04).
void __fastcall UNITGFX_UnitInitGfx(D2UnitStrc *pUnit)
{
    D2ClientUnitGfxInfoStrc *pUnitGfxInfo;

    pUnitGfxInfo = GFX_AllocUnitGfxInfo();
    pUnit->pGfxData = pUnitGfxInfo;
    if (!pUnitGfxInfo)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "ptUnit->uGfx.hGFX", (uint32_t)(UINT_PTR)szUnitGfxCpp, 22);
        exit(-1);
    }

    switch (pUnit->dwUnitType)
    {
    case UNIT_PLAYER:
    case UNIT_OBJECT:
        GFX_RebuildUnitAnimForMode(pUnit, 0);
        return;

    case UNIT_MONSTER:
        GFX_RebuildUnitAnimForMode(pUnit, 1);
        return;

    case UNIT_MISSILE:
    case UNIT_ITEM:
        return;

    default:
        FOG_DisplayHalt("[UnitInitGFX] Invalid unit type!\n", szUnitGfxCpp, 40);
        exit(-1);
    }
}

//1.10f: D2Client.0x6FB4DB40
//  The per-unit draw, called by the two cell passes DWALL_EnvDrawCellUnits and DWALL_EnvDrawCellShadowsAndUnits and by GFX_DrawUnitCels (which
//  passes bForce 1). dwLight is the packed light the draw tints by; it is handed to every draw call in edx.
//
//  1. Skipped: without bForce, a unit with dwFlagEx bit 18 set (UNITFLAGEX_NODRAW; `shr eax, 12h` / `and eax, 1`, 0x6FB4DB61); always, a player in
//  PLRMODE_DEAD without STATE_PLAYERBODY; without bForce, a monster or an object (signed tests `jle` / `jg`, 0x6FB4DB9C, 0x6FB4DBA1) in
//  STATE_ATTACHED or STATE_INVIS. Then, under perspective, a unit D2GFX_SCALE_CheckPerspectivePosition puts outside the view. A NULL unit takes
//  none of the type tests and reaches UNITS_GetPrecisionX, which asserts `ptUnit` and exits, so the NULL arms below are dead.
//
//  2. The line-of-sight flag, UNITFLAGEX_ISINLOS. The local player: set directly (`or al, 80h` on the whole dword, 0x6FB4DC20). A player not in
//  PLRMODE_DEATH or _DEAD, a monster not in MONMODE_DEATH or _DEAD, a missile or an item: tested against the local player -- when the player's
//  room's level has `LOSDraw` (or the player has no room), UNITS_TestCollisionWithUnit with COLLIDE_VISIBLE. Every other unit, and a test that
//  finds no level flag or no collision: MONSTER_SetFlagEx(1).
//  A collision is not a fade, it is no draw: when the test hits, MONSTER_SetFlagEx(pUnit, UNITFLAGEX_ISINLOS, 0) and the function returns
//  (0x6FB4DD19..0x6FB4DD27), before any position is computed or any draw call made.
//
//  3. Where: under perspective, D2GFX_SCALE_ScalePerspectivePosition from the precise position, angle 96, with bOrder 1 for an object whose
//  objects.txt `OrderFlag` for its current mode is 2, then the viewport origin added to both. Otherwise D2Common's client coordinates. The
//  perspective arm reuses the bForce slot for the X (`lea edx, [esp+28h+pXAdjust]`, 0x6FB4DC93).
//
//  4. How: by type, with the local `pUnit ? type : 6` (0x6FB4DC4A): a player between PLRSKILLS_SkillsDrawUnitEffectBehind and _InFront; a monster
//  the same, flat when MONSTER_IsGhostly (normalised to 0 or 1, 0x6FB4DE0D..0x6FB4DE0F), the ghostly test made before the effect behind it; a
//  missile shifted by its missiles.txt `Xoffset` across and `Zoffset` + `Yoffset` down, each a signed WORD (`movsx`), after the record's bounds
//  test; anything else plain. A draw that answers nonzero sets UNITFLAG_AUTOMAP in dwFlags. Then, under perspective, the default scale factor back.
//  The missile arm's flag store jumps into the plain arm's tail past its `test esi, esi` (`jmp loc_6FB4DD74`, 0x6FB4DDFF) -- the unit is already
//  known not NULL there -- so its `&& pUnit` is the plain arm's, written in both.
void __fastcall UNITGFX_GfxDrawUnit(D2UnitStrc *pUnit, uint32_t dwLight, int32_t bForce)
{
    int32_t bPerspective;
    int32_t nXPos;
    int32_t nYPos;
    int32_t nX;
    int32_t nY;
    int32_t bLocalPlayer;
    int32_t bTestLos;
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    uint32_t dwUnitType;
    int32_t bOrder;
    D2ObjectsTxt *pObjectsTxt;
    int32_t nClassId;
    D2MissilesTxt *pMissilesTxt;
    int32_t bGhostly;

    if (!bForce && pUnit && ((pUnit->dwFlagEx >> 18) & 1))
        return;

    if (pUnit && pUnit->dwUnitType == UNIT_PLAYER && pUnit->dwAnimMode == PLRMODE_DEAD && !D2COMMON_STATES_CheckState(pUnit, STATE_PLAYERBODY))
        return;

    //  Both sides cast: D2C_UnitTypes is based on uint32_t, and an int compared with it is converted to unsigned; the bytes' `jle`/`jg` are signed.
    if (!bForce && pUnit && (int32_t)pUnit->dwUnitType > (int32_t)UNIT_PLAYER && (int32_t)pUnit->dwUnitType <= (int32_t)UNIT_OBJECT && (D2COMMON_STATES_CheckState(pUnit, STATE_ATTACHED) || D2COMMON_STATES_CheckState(pUnit, STATE_INVIS)))
        return;

    bPerspective = D2GFX_CheckPerspective();
    nXPos = D2COMMON_UNITS_GetPrecisionX(pUnit);
    nYPos = D2COMMON_UNITS_GetPrecisionY(pUnit);
    if (bPerspective && !D2GFX_SCALE_CheckPerspectivePosition(nXPos, nYPos))
        return;

    bLocalPlayer = FALSE;
    bTestLos = FALSE;
    pPlayer = NULL;
    if (pUnit)
    {
        switch (pUnit->dwUnitType)
        {
        case UNIT_PLAYER:
            pPlayer = CUNIT_GetPlayer();
            if (pPlayer == pUnit)
                bLocalPlayer = TRUE;
            else if (pUnit->dwAnimMode != PLRMODE_DEATH && pUnit->dwAnimMode != PLRMODE_DEAD)
                bTestLos = TRUE;
            break;

        case UNIT_MONSTER:
            if (pUnit->dwAnimMode == MONMODE_DEATH || pUnit->dwAnimMode == MONMODE_DEAD)
                break;
            //  a living monster is tested as a missile or an item is
            pPlayer = CUNIT_GetPlayer();
            bTestLos = TRUE;
            break;

        case UNIT_MISSILE:
        case UNIT_ITEM:
            pPlayer = CUNIT_GetPlayer();
            bTestLos = TRUE;
            break;
        }
    }

    if (bLocalPlayer)
    {
        pUnit->dwFlagEx |= UNITFLAGEX_ISINLOS;
    }
    else
    {
        if (bTestLos && pPlayer)
        {
            pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
            if (!pRoom || D2COMMON_DRLG_GetLOSDrawFromLevelId((D2C_Levels)D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom)))
            {
                if (D2COMMON_UNITS_TestCollisionWithUnit(pPlayer, pUnit, COLLIDE_VISIBLE))
                {
                    MONSTER_SetFlagEx(pUnit, UNITFLAGEX_ISINLOS, 0);
                    return;
                }
            }
        }
        MONSTER_SetFlagEx(pUnit, UNITFLAGEX_ISINLOS, 1);
    }

    dwUnitType = pUnit ? pUnit->dwUnitType : UNIT_TYPES_COUNT;

    if (bPerspective)
    {
        bOrder = 0;
        if (pUnit && pUnit->dwUnitType == UNIT_OBJECT)
        {
            pObjectsTxt = D2COMMON_DATATBLS_GetObjectsTxtRecord(pUnit->dwClassId);
            //  BUG: the objects.txt record is read without a NULL test. UNREACHABLE: D2Common's getter asserts both bounds and exits out of range.
            if (pObjectsTxt->nOrderFlag[D2COMMON_UNITS_GetAnimOrSeqMode(pUnit)] == 2)
                bOrder = 1;
        }
        D2GFX_SCALE_ScalePerspectivePosition(nXPos, nYPos, 96, &nX, &nY, bOrder);
        nX += SCROLL_GetViewportOriginX();
        nY += SCROLL_GetViewportOriginY();
    }
    else
    {
        nX = D2COMMON_UNITS_GetClientCoordX(pUnit);
        nY = D2COMMON_UNITS_GetClientCoordY(pUnit);
    }

    switch (dwUnitType)
    {
    case UNIT_PLAYER:
        PLRSKILLS_SkillsDrawUnitEffectBehind(pUnit, dwLight);
        if (GFX_DrawUnitCels(pUnit, dwLight, nX, nY, 0, 1) && pUnit)
            pUnit->dwFlags |= UNITFLAG_AUTOMAP;
        PLRSKILLS_SkillsDrawUnitEffectInFront(pUnit, dwLight);
        break;

    case UNIT_MONSTER:
        bGhostly = 0;
        if (MONSTER_IsGhostly(pUnit))
            bGhostly = 1;
        PLRSKILLS_SkillsDrawUnitEffectBehind(pUnit, dwLight);
        if (GFX_DrawUnitCels(pUnit, dwLight, nX, nY, bGhostly, 1) && pUnit)
            pUnit->dwFlags |= UNITFLAG_AUTOMAP;
        PLRSKILLS_SkillsDrawUnitEffectInFront(pUnit, dwLight);
        break;

    case UNIT_MISSILE:
        if (pUnit)
        {
            nClassId = pUnit->dwClassId;
            if (nClassId >= 0 && nClassId < sgptDataTables->nMissilesTxtRecordCount)
            {
                pMissilesTxt = &sgptDataTables->pMissilesTxt[nClassId];
                if (pMissilesTxt && GFX_DrawUnitCels(pUnit, dwLight, nX + (int16_t)pMissilesTxt->wXoffset, nY + (int16_t)pMissilesTxt->wZoffset + (int16_t)pMissilesTxt->wYoffset, 0, 1) && pUnit)
                    pUnit->dwFlags |= UNITFLAG_AUTOMAP;
            }
        }
        break;

    default:
        if (GFX_DrawUnitCels(pUnit, dwLight, nX, nY, 0, 1) && pUnit)
            pUnit->dwFlags |= UNITFLAG_AUTOMAP;
        break;
    }

    if (bPerspective)
        D2GFX_SCALE_SetDefaultFactorPerspectiveFactor();
}

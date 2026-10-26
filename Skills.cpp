#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Skills.h"

//  Skills.cpp -- the core of the client's skill code in D2Client.dll 1.10: the Skills.txt `cltstfunc` and `cltdofunc` dispatchers, the casts the
//  server replays to the client, the target searches, and the missile spawners the class files share.
//  The original source file is C:\projects\D2\head\Diablo2\Source\D2Client\Skills\Skills.cpp (the asserts in this unit name it).
//  Address range 0x6FABA5E0..0x6FABE29F, 49 functions, including the state functions SKILLS_CltDoFunc85, SKILLS_StatesRunActiveStateFuncs and
//  SKILLS_StatesAddCltEventFunc.
//  Every nSkillId is typed D2C_Skills and every missile id D2C_MissilesHcIdx.

static_assert(offsetof(D2SkillsTxt, wItemCastSound) == 0x122
                  && offsetof(D2SkillsTxt, wItemCastOverlay) == 0x124,
              "SKILLS_ClientCastAtTarget: `mov ax, [eax+122h]` and `mov ax, [ecx+124h]`");
static_assert(offsetof(D2DataTablesStrc, nOverlayTxtRecordCount) == 0xBC0,
              "SKILLS_ClientCastAtTarget: `cmp edx, [ecx+0BC0h]`");
static_assert(SKILLSFLAGINDEX2_ITEMCLTCHECKSTART == 2 && STATE_CORPSE_NOSELECT == 118
                  && UNITFLAG_SKSRVDOFUNC == 0x40,
              "SKILLS_ClientCastAtTarget: gdwBitMasks[2] (`[ecx+8]`), `push 76h`, `or al, 40h`");
//  The path literal this file passes as __FILE__ (in the original one object, 0x6FB777BC).
static const char szSkillsCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\Skills\\Skills.cpp";


//1.10f: D2Client.0x6FABA5E0
//  states.txt cltactivefunc 85, its one row being openwounds: slot 85 of gpSkillCltDoFuncs, 47 bytes. It reads neither nSkillId nor nSkillLevel.
//  A bleeding unit spills one blood splash at most every six client frames: the frame count is compared (unsigned, strict) with
//  gnOpenWoundsNextFrame and, once past it, the mark is moved five frames on and MONSTER_SpawnBlood(pUnit, 1, 0) is called.
//  The next-frame mark is one global, not a field of the unit: every bleeding unit on screen shares one budget, so with two of them only
//  whichever reaches the handler first after the mark expires spills blood that frame.
int32_t __fastcall SKILLS_CltDoFunc85(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    uint32_t nFrame;

    nFrame = GAME_GetFrameCount();
    if (nFrame > gnOpenWoundsNextFrame)
    {
        gnOpenWoundsNextFrame = nFrame + 5;
        MONSTER_SpawnBlood(pUnit, 1, 0);
    }
    return 1;
}


//1.10f: D2Client.0x6FABA610
//  Replays a cast the server reported and leaves the unit as it was. pUnit is in ecx, nSkillId in edx, the five other arguments are on the stack. Its
//  callers are SKILLS_ClientCastAtUnit and SKILLS_ClientCastAtPosition, called by SCMD_HandleSrv99_CastAtUnit and SCMD_HandleSrv9A_CastAtXY (the
//  server's 0x99/0x9A are the 0x4C/0x4D "unit uses a skill" packets with their flag set). The third argument is the skill level. The shape is save,
//  point, run, put back. The unit's path target, its path's first point and bit 6 of dwFlags (UNITFLAG_SKSRVDOFUNC) are saved; the unit is pointed at
//  pTarget or at (nX, nY); the effect runs; and on every way out the target (the saved unit if there was one, else the saved point) and the bit are
//  put back. The bit is written back through the whole dword read fresh (0x6FABA7B8). The gate is ItemCltCheckStart (Skills.txt flag 34,
//  SKILLSFLAG2_ITEMCLTCHECKSTART), not a corpse flag: `mov al, [edx+8] / mov dl, [ecx+8] / test dl, al` tests the low byte of dwFlags[1] against
//  gdwBitMasks[2]. Only with it set does the client run the skill's start function (SKILLS_RunCltStFuncSwapped, owner GUID -1) first, and a start
//  that answers 0 cancels the cast and returns 0. Around that start, a dead target that carries STATE_CORPSE_NOSELECT has the state switched off, so
//  that the start can accept the corpse, and switched on again after. BUG: pUnit->pDynamicPath goes to D2Common untested (#10175, #10176 and #10170,
//  from 0x6FABA660) for whatever unit type the packet named; for a static unit the slot holds its static path. LATENT: the server sends 0x99 and 0x9A
//  only for item-skill casts, whose server side has already passed the same caster's pDynamicPath to #10175 and #10176. The cues are the ItemCast
//  columns wItemCastSound and wItemCastOverlay, each read as a signed word (`test ax, ax / jl`, then `movsx`), the overlay bounded by
//  nOverlayTxtRecordCount with `jge`. The fourth argument of SKILLS_RunClientDoFunc is nParam zero-extended (`and edx, 0FFFFh`); the callers push the
//  whole dword of their word slot, so the mask is what makes it a word. The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall SKILLS_ClientCastAtTarget(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, D2UnitStrc *pTarget, int32_t nX, int32_t nY, uint16_t nParam)
{
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pSavedTarget;
    int32_t nSavedX;
    int32_t nSavedY;
    uint32_t bSavedFlag;
    BOOL bLiftedNoSelect;
    BOOL bStarted;
    int16_t nItemCastSound;
    int16_t nItemCastOverlay;
    int32_t nResult;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return 0;

    pSavedTarget = CUNIT_GetPathTargetUnit(pUnit);
    nSavedX = D2COMMON_10175_PathGetFirstPointX(pUnit->pDynamicPath);
    nSavedY = D2COMMON_10176_PathGetFirstPointY(pUnit->pDynamicPath);
    bSavedFlag = (pUnit->dwFlags >> 6) & 1;

    if (pTarget)
        D2COMMON_UNITS_SetTargetUnitForDynamicUnit(pUnit, pTarget);
    else
        D2COMMON_10170_PathSetTargetPos(pUnit->pDynamicPath, nX, nY);

    bStarted = TRUE;
    if ((uint8_t)pSkillsTxt->dwFlags[1] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX2_ITEMCLTCHECKSTART])
    {
        bLiftedNoSelect = FALSE;
        if (pTarget && CUNIT_IsUnitDead(pTarget) && D2COMMON_STATES_CheckState(pTarget, STATE_CORPSE_NOSELECT))
        {
            bLiftedNoSelect = TRUE;
            D2COMMON_STATES_ToggleState(pTarget, STATE_CORPSE_NOSELECT, FALSE);
        }

        if (SKILLS_RunCltStFuncSwapped(pUnit, nSkillId, nSkillLevel, (D2UnitGUID)-1))
        {
            if (bLiftedNoSelect)
                D2COMMON_STATES_ToggleState(pTarget, STATE_CORPSE_NOSELECT, TRUE);
        }
        else
        {
            //  BUG: when the start answers 0 the state is not switched back on: the jz at 0x6FABA706 leaves past the second STATES_ToggleState
            //  (0x6FABA710..0x6FABA715), the path target and the flag are restored, and the corpse keeps STATE_CORPSE_NOSELECT off.
            //  NOT ESTABLISHED: in stock 1.10 the gate is set only on Corpse Explosion and Poison Explosion, whose start (cltstfunc 21) can answer 0,
            //  e.g. for a target D2Common #11021 does not accept as a selectable corpse; whether the server replays either skill onto such a corpse
            //  is not settled.
            bStarted = FALSE;
        }
    }

    if (bStarted)
    {
        nItemCastSound = (int16_t)pSkillsTxt->wItemCastSound;
        if (nItemCastSound >= 0)
            SOUNDINST_PlaySound((D2C_Sounds)nItemCastSound, pUnit, 0, 0, 0);

        nItemCastOverlay = (int16_t)pSkillsTxt->wItemCastOverlay;
        if (nItemCastOverlay >= 0 && nItemCastOverlay < sgptDataTables->nOverlayTxtRecordCount)
            GFX_AddOverlay(pUnit, (D2_OverlayIds)nItemCastOverlay, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);

        nResult = SKILLS_RunClientDoFunc(pUnit, nSkillId, nSkillLevel, nParam);
    }
    else
    {
        nResult = 0;
    }

    if (pSavedTarget)
        D2COMMON_UNITS_SetTargetUnitForDynamicUnit(pUnit, pSavedTarget);
    else
        D2COMMON_10170_PathSetTargetPos(pUnit->pDynamicPath, nSavedX, nSavedY);

    if (bSavedFlag)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;
    else
        pUnit->dwFlags &= ~(uint32_t)UNITFLAG_SKSRVDOFUNC;

    return nResult;
}


//1.10f: D2Client.0x6FABA7F0
//  The cast at a unit: SCMD_HandleSrv99_CastAtUnit's one call. A NULL unit or a NULL target answers 0 and casts nothing; otherwise it calls
//  SKILLS_ClientCastAtTarget with no coordinates. nParam goes on as the whole dword of its slot (0x6FABA7FD), which the callee masks to a word.
int32_t __fastcall SKILLS_ClientCastAtUnit(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, D2UnitStrc *pTarget, uint16_t nParam)
{
    if (pUnit && pTarget)
        return SKILLS_ClientCastAtTarget(pUnit, nSkillId, nSkillLevel, pTarget, 0, 0, nParam);

    return 0;
}


//1.10f: D2Client.0x6FABA820
//  The cast at a point: SCMD_HandleSrv9A_CastAtXY's one call. The guard is `nX && nY`: a point with either coordinate 0 is refused, as a NULL unit
//  is, and answers 0.
int32_t __fastcall SKILLS_ClientCastAtPosition(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nX, int32_t nY, uint16_t nParam)
{
    if (pUnit && nX && nY)
        return SKILLS_ClientCastAtTarget(pUnit, nSkillId, nSkillLevel, NULL, nX, nY, nParam);

    return 0;
}


//1.10f: D2Client.0x6FABA860
//  One step in front of the unit: the unit's position plus a step in the direction it faces. The direction is UNITS_GetDirection's byte, shifted
//  logically by 3 (`shr eax, 3`, 0x6FABA878) to one of eight, and it indexes two eight-entry tables the function builds on its own stack:
//
//      direction   0   1   2   3   4   5   6   7
//      step X      0  -1  -2  -1   0   1   2   1
//      step Y      2   1   0  -1  -2  -1   0   1
//
//  BUG: the index is not bounded. A direction of PATH_NB_DIRECTIONS (64) or more gives an index of 8 or more: the X read runs into the Y table
//  (the two are adjacent, 20h apart on the frame) and the Y read past it. LATENT: every store into a dynamic path's direction is taken modulo
//  PATH_NB_DIRECTIONS, a unit with no path answers direction 0 and a static path's direction is 0, so the index stays in 0..7.
//  void: the original leaves eax holding the Y read before the step is added.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall SKILLS_GetPosInFrontOfUnit(D2UnitStrc *pUnit, int32_t *pnX, int32_t *pnY)
{
    int32_t nDirOffsetX[8] = {0, -1, -2, -1, 0, 1, 2, 1};
    int32_t nDirOffsetY[8] = {2, 1, 0, -1, -2, -1, 0, 1};
    uint32_t nDirection;

    nDirection = (uint32_t)D2COMMON_UNITS_GetDirection(pUnit) >> 3;

    *pnX = UNITS_GetXPosition(pUnit) + nDirOffsetX[nDirection];

    *pnY = UNITS_GetYPosition(pUnit) + nDirOffsetY[nDirection];
}


//1.10f: D2Client.0x6FABA950
//  Where the unit is heading: the position of its path's target unit if it has one; otherwise its path's first point; and if either coordinate
//  came back 0, one step in front of the unit. It answers 1 on every path (`mov eax, 1` at the early exit, 0x6FABAB09, and ebx, loaded with 1
//  for the step table, 0x6FABAA33, at the other three). After the first two tries the test reads *pnX and *pnY back from memory, as the C does.
//  BUG: the first point's two reads take the path from one load (0x6FABA9D3) and hand it to D2Common untested, as SKILLS_ClientCastAtTarget does.
//  NOT ESTABLISHED: every site in this unit, called or inlined, hands it a skill's caster or a missile, units with a dynamic path, but another
//  caller, PLAYER_SetUnitRoomAndPos (0x6FB4B68F), lets a unit through by its animation mode (2, 3 or 6) and not by its type.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition and SKILLS_GetPosInFrontOfUnit here.
int32_t __fastcall SKILLS_GetPathTargetPos(D2UnitStrc *pUnit, int32_t *pnX, int32_t *pnY)
{
    D2UnitStrc *pTargetUnit;
    D2DynamicPathStrc *pPath;

    pTargetUnit = CUNIT_GetPathTargetUnit(pUnit);
    if (pTargetUnit)
    {
        *pnX = UNITS_GetXPosition(pTargetUnit);

        *pnY = UNITS_GetYPosition(pTargetUnit);
    }
    else
    {
        pPath = pUnit->pDynamicPath;
        *pnX = D2COMMON_10175_PathGetFirstPointX(pPath);
        *pnY = D2COMMON_10176_PathGetFirstPointY(pPath);
    }

    if (*pnX && *pnY)
        return 1;

    SKILLS_GetPosInFrontOfUnit(pUnit, pnX, pnY);
    return 1;
}


//1.10f: D2Client.0x6FABAB20
//  Grows a short vector in place; both pointers arrive in registers. With the squared length under 4 the vector is multiplied by four
//  (`lea eax, [esi*4]`, `shl eax, 2`) and the length recomputed; with it then under 16 it is doubled. So every vector but (0, 0) leaves with a
//  squared length of 16 or more. The compares are signed (`jge`).
//  Inside the first step *pY is read after *pX is stored, and *pX read back after *pY is (0x6FABAB3E, 0x6FABAB45); the C keeps both reads, which
//  differ from the held values only if the two pointers alias. The second step reads *pY again for the same reason (0x6FABAB5B).
void __fastcall SKILLS_GrowVectorToMinLength(int32_t *pX, int32_t *pY)
{
    int32_t nX;
    int32_t nY;
    int32_t nSqrLen;

    nX = *pX;
    nSqrLen = *pY * *pY + nX * nX;
    if (nSqrLen < 4)
    {
        *pX = nX * 4;
        nY = *pY * 4;
        *pY = nY;
        nX = *pX;
        nSqrLen = nY * nY + nX * nX;
    }

    if (nSqrLen < 16)
    {
        *pX = nX * 2;
        *pY = *pY * 2;
    }
}


//1.10f: D2Client.0x6FABAB70
//  Turns a vector a quarter and shrinks it: (x, y) becomes (y, -x), then both halve (signed, toward zero: `cdq / sub / sar`) while the squared
//  length is over 3 (`jg`). Both results leave through the pointers, *pX first (0x6FABABAD).
void __fastcall SKILLS_ShrinkPerpendicularVector(int32_t *pX, int32_t *pY)
{
    int32_t nPerpX;
    int32_t nPerpY;

    nPerpY = -*pX;
    nPerpX = *pY;
    while (nPerpY * nPerpY + nPerpX * nPerpX > 3)
    {
        nPerpX /= 2;
        nPerpY /= 2;
    }

    *pX = nPerpX;
    *pY = nPerpY;
}


//1.10f: D2Client.0x6FABABC0
//  A missile aimed where the unit is heading. pUnit is in ecx and nMissileId in edx; nSkillId and nSkillLevel are on the stack. The point is
//  SKILLS_GetPathTargetPos's; with either coordinate still 0 nothing spawns and the answer is NULL. Otherwise MISSILE_ClientSpawnForOwner takes
//  the point as the missile's target, where SKILLS_MissileClientSpawnOnTargetUnit takes the same point as its origin.
//  The original inlines SKILLS_GetPathTargetPos here.
D2UnitStrc *__fastcall SKILLS_MissileClientSpawnAimedAtTargetUnit(D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    int32_t nX;
    int32_t nY;

    SKILLS_GetPathTargetPos(pUnit, &nX, &nY);

    if (nX && nY)
        return MISSILE_ClientSpawnForOwner(pUnit, nMissileId, nX, nY, nSkillId, nSkillLevel);

    return NULL;
}


//1.10f: D2Client.0x6FABAD50
//  A missile started where the unit is heading, if it is near: the same point, but as the missile's origin (MISSILE_ClientSpawnSimple), and only
//  within a squared distance of 10000 (100 subtiles; `cmp eax, 2710h / jg`, 0x6FABAE6E) of the unit; farther, or with either coordinate 0,
//  nothing spawns and the answer is 0. The distance takes the unit's Y before its X (0x6FABAE0F, 0x6FABAE3B), as the C does, and
//  PATH_ComputeSquaredDistance gets (unit X, unit Y, point X, point Y).
//  Here the point's fallback calls SKILLS_GetPosInFrontOfUnit (0x6FABADF2) and reads both results back from the frame (0x6FABADF7, 0x6FABADFB).
//  The answer is a unit: MISSILE_ClientSpawnSimple's (0x6FABAE87), the missile or NULL (which that function answers for a NULL pUnit), or 0 on
//  the two refusals (0x6FABAE99).
//  The original inlines SKILLS_GetPathTargetPos, UNITS_GetYPosition and UNITS_GetXPosition here.
D2UnitStrc *__fastcall SKILLS_MissileClientSpawnOnTargetUnit(D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    int32_t nPointX;
    int32_t nPointY;
    int32_t nUnitX;
    int32_t nUnitY;

    SKILLS_GetPathTargetPos(pUnit, &nPointX, &nPointY);

    if (!nPointX || !nPointY)
        return NULL;

    nUnitY = UNITS_GetYPosition(pUnit);

    nUnitX = UNITS_GetXPosition(pUnit);

    if (D2COMMON_PATH_ComputeSquaredDistance(nUnitX, nUnitY, nPointX, nPointY) > 10000)
        return NULL;

    return MISSILE_ClientSpawnSimple(pUnit, nMissileId, nPointX, nPointY, nSkillId, nSkillLevel);
}


static_assert(STAT_ITEM_MAGICARROW == 0x9D && STAT_ITEM_THROWABLE == 0x7D,
              "SKILLS_HasAmmoForShot: `push 9Dh` (0x6FABAF0E), `push 7Dh` (0x6FABAF5D, 0x6FABAF9F)");
static_assert(PLRMODE_DEATH == 0 && PLRMODE_DEAD == 0x11 && MONMODE_DEATH == 0 && MONMODE_DEAD == 0x0C,
              "SKILLS_UnitMatchesTargetMask: `cmp eax, 11h` and `cmp eax, 0Ch`, each beside "
              "`test eax, eax`");
static_assert(offsetof(D2MissilesTxt, dwMissileFlags) == 4 && MISSILESFLAGINDEX_EXPLOSION == 1,
              "SKILLS_UnitMatchesTargetMask: `mov al, [eax+4]` / `test [ecx+4], al` (0x6FABB33F)");
static_assert(MONSTATSFLAGINDEX_NPC == 8 && MONSTATSFLAGINDEX_NOAURA == 27,
              "`mov dl, [eax+0Dh]` / `test [eax], dl` (0x6FABB41A); "
              "`mov cl, [eax+0Fh]` / `and ecx, [edx+0Ch]` (0x6FABB7E6)");
static_assert(UNITFLAG_ISVALIDTARGET == 0x08 && STATE_JUSTHIT == 0x56 && COLLIDE_MISSILE_BARRIER == 4,
              "SKILLS_UnitMatchesTargetMask: `shr eax, 3` (0x6FABB37C), `push 56h` (0x6FABB460), "
              "`push 4` (0x6FABB553)");
static_assert(offsetof(D2ClientNearbyTargetCtxStrc, pUnit) == 0
                  && offsetof(D2ClientNearbyTargetCtxStrc, nAccepted) == 4
                  && offsetof(D2ClientNearbyTargetCtxStrc, pUserData) == 8,
              "SKILLS_ForEachNearbyTarget: var_2C, var_28, var_24 (0x6FABB661..0x6FABB669); "
              "`mov eax, [ecx+8]` in both visitors");
static_assert(offsetof(D2ClientTargetCycleStrc, pBest) == 0x00
                  && offsetof(D2ClientTargetCycleStrc, dwBestId) == 0x04
                  && offsetof(D2ClientTargetCycleStrc, pWrapped) == 0x08
                  && offsetof(D2ClientTargetCycleStrc, dwWrappedId) == 0x0C
                  && offsetof(D2ClientTargetCycleStrc, dwAfterId) == 0x10
                  && offsetof(D2ClientTargetCycleStrc, nSeen) == 0x14,
              "SKILLS_NextTargetByIdVisitor: [eax] .. [eax+14h], 0x6FABB920..0x6FABB94F");
//  The D2C_AuraFilters masks used below; the enum has no member for 20000h, the melee-range bit.
static const uint32_t nAuraFilterMeleeRange = 0x20000;
static const uint32_t nDefaultTargetMask = AURAFILTER_FINDPLAYER | AURAFILTER_FINDMONSTER | AURAFILTER_FINDISATT | AURAFILTER_IGNINTOWN | AURAFILTER_FINDISSEL;
static const uint32_t nNextTargetForcedMask = AURAFILTER_FINDISATT | AURAFILTER_USELOS | AURAFILTER_IGNINTOWN2 | AURAFILTER_IGNALLY;
static const uint32_t nCountTargetsForcedMask = nDefaultTargetMask | nNextTargetForcedMask;
static_assert(nDefaultTargetMask == 0x583 && nNextTargetForcedMask == 0xA280
                  && nCountTargetsForcedMask == 0xA783,
              "`mov [esp+54h+nMask], 583h` (0x6FABB641), `mov eax, 583h` (0x6FABB8C7), "
              "`or eax, 0A280h` (0x6FABB8D4), `or eax, 0A783h` (0x6FABB84C)");


//1.10f: D2Client.0x6FABAEB0
//  Can the unit shoot. pUnit is in ecx. NULL answers 0; a unit that is not a player answers 1 (`cmp dword ptr [esi], 0`); so does a player for
//  whom D2Common #10434 answers NULL. #10434 takes the equipped weapon (INVENTORY_GetEquippedWeapon) and, for a unit that can dual-wield, picks
//  between the hands by the used skill's `weapsel`.
//  ebp starts at 1 and is cleared when ITEMS_GetAmmoType answers non-zero (the slot holds bNoAmmoRequired). With an ammo type, a used skill of id 0
//  (SKILL_ATTACK) on a weapon carrying STAT_ITEM_MAGICARROW answers 1 before any ammo is looked for. No used skill at all passes the same test:
//  SKILLS_GetSkillIdFromSkill is given the file and line (szSkillsCpp and 182) to trace for a NULL skill, and answers 0.
//  Then the hands, right before left, and the first item that passes answers ITEMS_CanItemBeUsedForThrowSkill. The item must be stackable,
//  throwable or carry STAT_ITEM_THROWABLE, and with no ammo type it must be the weapon itself (`test ebp, ebp / jz` past `cmp edi, ebx`,
//  0x6FABAF40; 0x6FABAF82 the same). So a weapon with an ammo type is served by any such item in either hand (its quiver), and one without
//  shoots only when it is a throwing stack itself.
//  Every stat read takes layer 0, pushed from a register just tested zero (`push eax` at 0x6FABAF0D, 0x6FABAF5C, 0x6FABAF9E).
BOOL __fastcall SKILLS_HasAmmoForShot(D2UnitStrc *pUnit)
{
    D2UnitStrc *pWeapon;
    BOOL bNoAmmoRequired;
    D2InventoryStrc *pInventory;
    D2UnitStrc *pRightHandItem;
    D2UnitStrc *pLeftHandItem;

    if (!pUnit)
        return FALSE;

    if (pUnit->dwUnitType != UNIT_PLAYER)
        return TRUE;

    pWeapon = D2COMMON_10434(pUnit, FALSE);
    bNoAmmoRequired = TRUE;
    if (!pWeapon)
        return bNoAmmoRequired;

    if (D2COMMON_ITEMS_GetAmmoType(pWeapon))
    {
        bNoAmmoRequired = FALSE;
        if ((D2C_Skills)D2COMMON_SKILLS_GetSkillIdFromSkill(D2COMMON_UNITS_GetUsedSkill(pUnit), szSkillsCpp, 182) == SKILL_ATTACK && D2COMMON_STATLIST_UnitGetItemStatOrSkillStatValue(pWeapon, STAT_ITEM_MAGICARROW, 0))
        {
            return TRUE;
        }
    }

    pInventory = pUnit->pInventory;
    if (!pInventory)
        return FALSE;

    pRightHandItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RARM);
    if (pRightHandItem && (!bNoAmmoRequired || pRightHandItem == pWeapon) && (D2COMMON_ITEMS_CheckIfStackable(pRightHandItem) || D2COMMON_ITEMS_CheckIfThrowable(pRightHandItem)
            || D2COMMON_STATLIST_UnitGetItemStatOrSkillStatValue(pRightHandItem, STAT_ITEM_THROWABLE, 0)))
    {
        return D2COMMON_ITEMS_CanItemBeUsedForThrowSkill(pRightHandItem);
    }

    pLeftHandItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_LARM);
    if (pLeftHandItem && (!bNoAmmoRequired || pLeftHandItem == pWeapon) && (D2COMMON_ITEMS_CheckIfStackable(pLeftHandItem) || D2COMMON_ITEMS_CheckIfThrowable(pLeftHandItem)
            || D2COMMON_STATLIST_UnitGetItemStatOrSkillStatValue(pLeftHandItem, STAT_ITEM_THROWABLE, 0)))
    {
        return D2COMMON_ITEMS_CanItemBeUsedForThrowSkill(pLeftHandItem);
    }

    return FALSE;
}


//1.10f: D2Client.0x6FABAFC0
//  A client missile from the unit to a point. nMissileId is in ecx and pUnit in edx; the six other arguments are on the stack.
//  The spawn record is cleared whole (`rep stosd`, 17h dwords) and given flags 21h, the owner, the unit's position plus the spawn offset, the
//  skill, its level and the missile. In MISSILE_ClientSpawn flag 1 is the start at nX/nY and 20h the target an absolute point.
//  The target is (nTargetX, nTargetY) when both are non-zero. Otherwise it is SKILLS_GetPathTargetPos's point, written straight into the record;
//  and when either coordinate is still 0 after that, nothing spawns and the answer is NULL. The last test takes X from ecx and Y from eax, the
//  values just stored (0x6FABB1F1..0x6FABB1F7).
//  BUG: pUnit is read untested from the first instruction on (`mov eax, [esi]`, 0x6FABAFDD). UNREACHABLE: its one caller,
//  SKILLS_RunClientDoFunc, cannot pass NULL.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition and SKILLS_GetPathTargetPos here.
D2UnitStrc *__fastcall SKILLS_SpawnCltMissileStraight(D2C_MissilesHcIdx nMissileId, D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nSpawnOffsetX, int32_t nSpawnOffsetY, int32_t nTargetX, int32_t nTargetY)
{
    D2MissileStrc tParams;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pUnit;

    tParams.nX = UNITS_GetXPosition(pUnit) + nSpawnOffsetX;

    tParams.nY = UNITS_GetYPosition(pUnit) + nSpawnOffsetY;

    tParams.nSkill = nSkillId;
    tParams.nSkillLevel = nSkillLevel;
    tParams.nMissile = nMissileId;

    if (nTargetX && nTargetY)
    {
        tParams.nTargetX = nTargetX;
        tParams.nTargetY = nTargetY;
    }
    else
    {
        SKILLS_GetPathTargetPos(pUnit, &tParams.nTargetX, &tParams.nTargetY);
    }

    if (tParams.nTargetX && tParams.nTargetY)
        return MISSILE_ClientSpawn(&tParams);

    return NULL;
}


//1.10f: D2Client.0x6FABB220
//  Does the target pass the filter. pUnit is in ecx, pTarget in edx, nMask is a D2C_AuraFilters. It is the client's twin of the server's aura
//  filter.
//  By type (a jump table on dwUnitType, five cases): a player or a monster must be neither DEAD nor DEATH, or with AURAFILTER_FINDCORPSE exactly
//  DEAD (PLRMODE_DEAD 17, MONMODE_DEAD 12); a DEATH mode (0) passes neither way. A monster then meets FINDUNDEAD, IGNBOSS and IGNPRIME. A
//  missile's missiles.txt row must not carry EXPLOSION. An object or an item needs only its FIND bit.
//  Then, each only with its bit: the target's dwFlags must carry UNITFLAG_CANBEATTACKED (FINDISATT) and UNITFLAG_ISVALIDTARGET (FINDISSEL); its
//  room must not be in town (IGNINTOWN); it must be an ally (FINDALLY); CUNIT_IsUnitSelectableTarget must accept it (IGNALLY); it must be in melee
//  range (20000h); it must not be in STATE_JUSTHIT (IGNJUSTHIT); and with USELOS both units need a room and D2Common's ray from the unit to the
//  target must meet no COLLIDE_MISSILE_BARRIER.
//  AURAFILTER_FINDALLY asks for an ally: a monster target stands for its roster owner (ROSTERPETS_GetOwnerId; one with no owner is refused), and
//  ROSTER_UnitsShareParty must put that id in a party with pUnit's, or with -1 for a NULL pUnit. A monster must then not be an NPC (monstats.txt
//  flag 8).
//  The two dwFlags tests shift the bit down and park it in nMask's argument slot (0x6FABB361; 0x6FABB37C the same with 3), a slot nothing reads
//  again; the C tests the bit.
//  BUG: pUnit is tested for NULL on the FINDALLY arm only; CUNIT_IsUnitSelectableTarget and the USELOS arm hand it to UNITS_GetRoom untested.
//  UNREACHABLE: the one caller returns before its scan when pUnit is NULL (0x6FABB5A5).
//  The original inlines DATATBLS_GetMonStatsTxtRecord here, and the missiles.txt record lookup is written in place.
BOOL __fastcall SKILLS_UnitMatchesTargetMask(D2UnitStrc *pUnit, D2UnitStrc *pTarget, D2C_AuraFilters nMask)
{
    int32_t nMissileId;
    D2MissilesTxt *pMissilesTxt;
    D2MonStatsTxt *pMonStatsTxt;
    D2UnitGUID dwAllyId;
    D2ActiveRoomStrc *pSourceRoom;
    D2CoordStrc tFrom;
    D2CoordStrc tTo;

    if (!pTarget)
        return FALSE;

    switch (pTarget->dwUnitType)
    {
    case UNIT_PLAYER:
        if (!(nMask & AURAFILTER_FINDPLAYER))
            return FALSE;

        if (nMask & AURAFILTER_FINDCORPSE)
        {
            if (pTarget->dwAnimMode != PLRMODE_DEAD)
                return FALSE;
        }
        else if (pTarget->dwAnimMode == PLRMODE_DEAD || pTarget->dwAnimMode == PLRMODE_DEATH)
        {
            return FALSE;
        }
        break;

    case UNIT_MONSTER:
        if (!(nMask & AURAFILTER_FINDMONSTER))
            return FALSE;

        if (nMask & AURAFILTER_FINDCORPSE)
        {
            if (pTarget->dwAnimMode != MONMODE_DEAD)
                return FALSE;
        }
        else if (pTarget->dwAnimMode == MONMODE_DEAD || pTarget->dwAnimMode == MONMODE_DEATH)
        {
            return FALSE;
        }

        if ((nMask & AURAFILTER_FINDUNDEAD) && !D2COMMON_MONSTERS_IsUndead(pTarget))
            return FALSE;

        if ((nMask & AURAFILTER_IGNBOSS) && D2COMMON_MONSTERS_IsBoss(NULL, pTarget))
            return FALSE;

        if ((nMask & AURAFILTER_IGNPRIME) && D2COMMON_MONSTERS_IsPrimeEvil(pTarget))
            return FALSE;
        break;

    case UNIT_OBJECT:
        if (!(nMask & AURAFILTER_FINDOBJECT))
            return FALSE;
        break;

    case UNIT_MISSILE:
        if (!(nMask & AURAFILTER_FINDMISSILE))
            return FALSE;

        nMissileId = pTarget->dwClassId;
        if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
            return FALSE;

        pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
        if (!pMissilesTxt)
            return FALSE;

        //  A byte against a byte: `mov al, [eax+4] / test [ecx+4], al`.
        if ((uint8_t)pMissilesTxt->dwMissileFlags & (uint8_t)gdwBitMasks[MISSILESFLAGINDEX_EXPLOSION])
        {
            return FALSE;
        }
        break;

    case UNIT_ITEM:
        if (!(nMask & AURAFILTER_FINDITEM))
            return FALSE;
        break;

    default:
        return FALSE;
    }

    if ((nMask & AURAFILTER_FINDISATT) && !(pTarget->dwFlags & UNITFLAG_CANBEATTACKED))
        return FALSE;

    if ((nMask & AURAFILTER_FINDISSEL) && !(pTarget->dwFlags & UNITFLAG_ISVALIDTARGET))
        return FALSE;

    if ((nMask & AURAFILTER_IGNINTOWN) && D2COMMON_DUNGEON_IsRoomInTown(D2COMMON_UNITS_GetRoom(pTarget)))
    {
        return FALSE;
    }

    if (nMask & AURAFILTER_FINDALLY)
    {
        dwAllyId = pTarget->dwUnitId;
        if (pTarget->dwUnitType == UNIT_MONSTER)
        {
            dwAllyId = (D2UnitGUID)ROSTERPETS_GetOwnerId(pTarget->dwUnitId);
            if (dwAllyId == (D2UnitGUID)-1)
                return FALSE;
        }

        if (!ROSTER_UnitsShareParty(pUnit ? pUnit->dwUnitId : (D2UnitGUID)-1, dwAllyId))
            return FALSE;

        if (pTarget->dwUnitType == UNIT_MONSTER)
        {
            pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pTarget->dwClassId);
            if (!pMonStatsTxt)
                return FALSE;

            //  A byte against a byte: `mov dl, [eax+0Dh] / test [eax], dl`.
            if (pMonStatsTxt->nMonStatsFlags[MONSTATSFLAGINDEX_NPC >> 3] & (uint8_t)gdwBitMasks[MONSTATSFLAGINDEX_NPC & 7])
            {
                return FALSE;
            }
        }
    }

    if ((nMask & AURAFILTER_IGNALLY) && !CUNIT_IsUnitSelectableTarget(pUnit, pTarget))
        return FALSE;

    if ((nMask & nAuraFilterMeleeRange) && !D2COMMON_UNITS_IsInMeleeRange(pUnit, pTarget, 0))
        return FALSE;

    if ((nMask & AURAFILTER_IGNJUSTHIT) && D2COMMON_STATES_CheckState(pTarget, STATE_JUSTHIT))
        return FALSE;

    if (!(nMask & AURAFILTER_USELOS))
        return TRUE;

    pSourceRoom = D2COMMON_UNITS_GetRoom(pUnit);
    if (!pSourceRoom || !D2COMMON_UNITS_GetRoom(pTarget))
        return FALSE;

    tFrom.nX = UNITS_GetXPosition(pUnit);

    tFrom.nY = UNITS_GetYPosition(pUnit);

    tTo.nX = UNITS_GetXPosition(pTarget);

    tTo.nY = UNITS_GetYPosition(pTarget);

    if (D2COMMON_COLLISION_RayTrace(pSourceRoom, &tFrom, &tTo, COLLIDE_MISSILE_BARRIER))
        return FALSE;

    return TRUE;
}


//1.10f: D2Client.0x6FABB590
//  Visits every target near a point. pUnit is in ecx and nX in edx; the six other arguments are on the stack. Nothing is done without a unit, a
//  visitor, a non-zero radius and the unit's room. The centre is (nX, nY), or the unit's own position when either is 0; a mask of 0 becomes
//  nDefaultTargetMask, 583h. The squared radius is taken once, and D2Common's list of rooms adjacent to the unit's is walked.
//  Per room: with AURAFILTER_IGNINTOWN2 a room in town is skipped (the room-level twin of the filter's per-unit IGNINTOWN). Then comes a
//  bounding-box test on the room's subtile rectangle (see the BUG at the test).
//  Per unit, in the room's list order: the next link is read first (0x6FABB718), the scanning unit is skipped, the candidate's Y is read before
//  its X, and the candidate must lie within the squared radius (`cmp [esp+54h+var_34], eax / jl`, signed), pass SKILLS_UnitMatchesTargetMask,
//  and, with bSkipNoAuraMonsters, not be a monster whose monstats.txt row carries NOAURA (flag 27). That flag byte is zero-extended and ANDed
//  with the whole of gdwBitMasks[3] (`xor ecx, ecx / mov cl, [eax+0Fh] / and ecx, [edx+0Ch]`), where SKILLS_UnitMatchesTargetMask tests a byte
//  against a byte; the C writes each as its bytes do. A monster whose row is not found is visited. A visitor's non-zero answer counts one in
//  tCtx.nAccepted, which lives on this function's stack, so only a visitor reads it: MISSILE_SpawnOnUnitCallback compares it with the dword at
//  +10h of its user data (0x6FB3774C).
//  The context's first field, pUnit, is written here (0x6FABB661) and read by none of the three visitors.
//  The room counter is unsigned against the count (`jbe` at 0x6FABB682, `jb` at 0x6FABB821) and lives between rooms in nY's argument slot, which
//  nothing reads once the centre is taken; `nMask & 2000h` is parked the same way in pUserData's slot once the context holds pUserData.
//  The original inlines UNITS_GetXPosition, UNITS_GetYPosition and DATATBLS_GetMonStatsTxtRecord here.
void __fastcall SKILLS_ForEachNearbyTarget(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nRadius, D2C_AuraFilters nMask, int32_t(__fastcall *pfnVisit)(D2ClientNearbyTargetCtxStrc *pCtx, D2UnitStrc *pTarget),
                                           void *pUserData, BOOL bSkipNoAuraMonsters)
{
    D2ActiveRoomStrc *pSourceRoom;
    D2ActiveRoomStrc **ppRoomList;
    int32_t nNumRooms;
    uint32_t iRoom;
    D2DrlgCoordsStrc tCoords;
    D2ClientNearbyTargetCtxStrc tCtx;
    int32_t nCentreX;
    int32_t nCentreY;
    int32_t nRadiusSquared;
    D2UnitStrc *pCandidate;
    D2UnitStrc *pNext;
    int32_t nCandidateX;
    int32_t nCandidateY;
    D2MonStatsTxt *pMonStatsTxt;

    if (!pUnit || !pfnVisit || !nRadius)
        return;

    pSourceRoom = D2COMMON_UNITS_GetRoom(pUnit);
    if (!pSourceRoom)
        return;

    if (nX && nY)
    {
        nCentreX = nX;
        nCentreY = nY;
    }
    else
    {
        nCentreX = UNITS_GetXPosition(pUnit);

        nCentreY = UNITS_GetYPosition(pUnit);
    }

    if (!nMask)
        nMask = (D2C_AuraFilters)nDefaultTargetMask;

    nRadiusSquared = nRadius * nRadius;
    tCtx.pUnit = pUnit;
    tCtx.pUserData = pUserData;
    tCtx.nAccepted = 0;

    D2COMMON_DUNGEON_GetAdjacentRoomsListFromRoom(pSourceRoom, &ppRoomList, &nNumRooms);

    for (iRoom = 0; iRoom < (uint32_t)nNumRooms; ++iRoom)
    {
        if ((nMask & AURAFILTER_IGNINTOWN2) && D2COMMON_DUNGEON_IsRoomInTown(ppRoomList[iRoom]))
            continue;

        D2COMMON_DUNGEON_GetRoomCoordinates(ppRoomList[iRoom], &tCoords);

        //  BUG: `||` where an overlap test needs `&&`, both axes: an axis skips the room only when the centre is more than nRadius before the
        //  rectangle's start AND more than nRadius past its end (`jge` past the second compare, then `jg`: 0x6FABB6D1..0x6FABB6DF,
        //  0x6FABB6EC..0x6FABB6FC), which no rectangle of non-negative width allows. INERT: the test passes every room, and every unit of every
        //  room is then held to the squared radius before anything else is asked of it, so the per-unit test decides alone; it costs only the walk
        //  of rooms the distance test empties.
        if (!(nCentreX + nRadius >= tCoords.nSubtileX || nCentreX - nRadius <= tCoords.nSubtileX + tCoords.nSubtileWidth))
        {
            continue;
        }

        if (!(nRadius + nCentreY >= tCoords.nSubtileY || nCentreY - nRadius <= tCoords.nSubtileY + tCoords.nSubtileHeight))
        {
            continue;
        }

        for (pCandidate = ppRoomList[iRoom]->pUnitFirst; pCandidate; pCandidate = pNext)
        {
            pNext = pCandidate->pRoomNext;
            if (pCandidate == pUnit)
                continue;

            nCandidateY = UNITS_GetYPosition(pCandidate);

            nCandidateX = UNITS_GetXPosition(pCandidate);

            if (nRadiusSquared < D2COMMON_PATH_ComputeSquaredDistance(nCentreX, nCentreY, nCandidateX, nCandidateY))
            {
                continue;
            }

            if (!SKILLS_UnitMatchesTargetMask(pUnit, pCandidate, nMask))
                continue;

            if (bSkipNoAuraMonsters && pCandidate->dwUnitType == UNIT_MONSTER)
            {
                pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pCandidate->dwClassId);
                if (pMonStatsTxt && (pMonStatsTxt->nMonStatsFlags[MONSTATSFLAGINDEX_NOAURA >> 3] & gdwBitMasks[MONSTATSFLAGINDEX_NOAURA & 7]))
                {
                    continue;
                }
            }

            if (pfnVisit(&tCtx, pCandidate))
                ++tCtx.nAccepted;
        }
    }
}


//1.10f: D2Client.0x6FABB840
//  Counts the targets near a point. pUnit and nX are passed through in ecx and edx. The scan with SKILLS_VisitCountUnits, a counter on this
//  function's stack as pUserData, and nMask ORed with nCountTargetsForcedMask, 0A783h.
//  0A783h is the scan's default 583h with SKILLS_FindNextNearbyTargetById's forced 0A280h, so for a zero mask the two scan the same units; for a
//  non-zero one this forces 503h that FindNext does not (FINDPLAYER, FINDMONSTER, IGNINTOWN, FINDISSEL).
int32_t __fastcall SKILLS_CountNearbyTargets(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nRadius, D2C_AuraFilters nMask)
{
    int32_t nCount;

    nCount = 0;
    SKILLS_ForEachNearbyTarget(pUnit, nX, nY, nRadius, (D2C_AuraFilters)(nMask | nCountTargetsForcedMask), SKILLS_VisitCountUnits, &nCount, FALSE);
    return nCount;
}


//1.10f: D2Client.0x6FABB880
//  The counting visitor: one more in the int its user data points at, and 1. pTarget arrives in edx and is not read; it is in the prototype
//  because the scan calls every visitor through one type.
int32_t __fastcall SKILLS_VisitCountUnits(D2ClientNearbyTargetCtxStrc *pCtx, D2UnitStrc *pTarget)
{
    UNREFERENCED_PARAMETER(pTarget);

    ++*(int32_t *)pCtx->pUserData;
    return 1;
}


//1.10f: D2Client.0x6FABB890
//  The next target after an id, wrapping. pUnit and nX are passed through in ecx and edx. The scan with SKILLS_NextTargetByIdVisitor over a
//  D2ClientTargetCycleStrc on this function's stack, seeded {NULL, 0xFFFFFFFF, NULL, dwAfterUnitId, dwAfterUnitId, 0}. A zero mask becomes
//  nDefaultTargetMask here, before nNextTargetForcedMask, 0A280h, is ORed in, so the scan's own default never applies to this caller.
//  The answer is the unit with the smallest id above dwAfterUnitId, else the one with the smallest id at or below it (the wrap), else NULL;
//  pnCount, when given, receives every unit the visitor saw. The order is by unit id, not by distance; 0xFFFFFFFF as dwAfterUnitId asks for the
//  smallest id of all.
D2UnitStrc *__fastcall SKILLS_FindNextNearbyTargetById(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nRadius, D2C_AuraFilters nMask, uint32_t dwAfterUnitId, int32_t *pnCount)
{
    D2ClientTargetCycleStrc tCycle;
    D2UnitStrc *pChosen;

    tCycle.pBest = NULL;
    tCycle.dwBestId = 0xFFFFFFFF;
    tCycle.pWrapped = NULL;
    tCycle.dwWrappedId = dwAfterUnitId;
    tCycle.dwAfterId = dwAfterUnitId;
    tCycle.nSeen = 0;

    if (!nMask)
        nMask = (D2C_AuraFilters)nDefaultTargetMask;

    SKILLS_ForEachNearbyTarget(pUnit, nX, nY, nRadius, (D2C_AuraFilters)(nMask | nNextTargetForcedMask), SKILLS_NextTargetByIdVisitor, &tCycle, FALSE);

    pChosen = tCycle.pBest;
    if (!pChosen)
        pChosen = tCycle.pWrapped;

    if (pnCount)
        *pnCount = tCycle.nSeen;

    return pChosen;
}


//1.10f: D2Client.0x6FABB910
//  The visitor that keeps the next id. The block is the context's user data (`mov eax, [ecx+8]`). A unit whose id lies strictly between
//  dwAfterId and dwBestId becomes the best; any other becomes the wrap candidate when its id is at or below dwWrappedId. Every unit counts one in
//  nSeen, and the answer is always 1. The compares are unsigned (`jbe`, `jnb`, `ja`). A NULL pTarget would read as id 0xFFFFFFFF; the scan never
//  passes one.
int32_t __fastcall SKILLS_NextTargetByIdVisitor(D2ClientNearbyTargetCtxStrc *pCtx, D2UnitStrc *pTarget)
{
    D2ClientTargetCycleStrc *pCycle;
    uint32_t dwUnitId;

    pCycle = (D2ClientTargetCycleStrc *)pCtx->pUserData;
    dwUnitId = pTarget ? pTarget->dwUnitId : 0xFFFFFFFF;

    if (dwUnitId > pCycle->dwAfterId && dwUnitId < pCycle->dwBestId)
    {
        pCycle->dwBestId = dwUnitId;
        pCycle->pBest = pTarget;
        ++pCycle->nSeen;
        return 1;
    }

    if (dwUnitId <= pCycle->dwWrappedId)
    {
        pCycle->dwWrappedId = dwUnitId;
        pCycle->pWrapped = pTarget;
    }

    ++pCycle->nSeen;
    return 1;
}


//  Stock Skills.txt: cltstfunc 1 Attack, 2 Throw and Left Hand Throw, 3 Unsummon, 4 Left Hand Swing; cltdofunc 1 Attack and Left Hand Swing,
//  2 Throw and Left Hand Throw.

static_assert(ITEMTYPE_MISSILE_POTION == 0x26,
              "`push 26h` (0x6FABB988), `cmp eax, 26h` (0x6FABBCB9)");
static_assert(PETTYPEFLAGINDEX_UNSUMMON == 3 && offsetof(D2PetTypeTxt, dwPetFlags) == 4,
              "SKILLS_CltStFunc3: `mov al, [edx+0Ch]` / `mov dl, [ecx+4]` (0x6FABBA62)");


//1.10f: D2Client.0x6FABB960
//  `cltstfunc` 1, can attack start; it reads neither nSkillId nor nSkillLevel. With a bow or crossbow (COMPOSIT_IsWeaponBowOrXBow) the answer is
//  SKILLS_HasAmmoForShot's, made 0 or 1 (`jnz` to `mov eax, 1`, 0x6FABB976). Otherwise the attack starts unless the weapon D2Common #10434
//  answers is a throwing potion (ITEMTYPE_MISSILE_POTION, through ITEMS_CheckItemTypeId); no weapon starts it too.
BOOL __fastcall SKILLS_CltStFunc1(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2UnitStrc *pWeapon;

    if (D2COMMON_COMPOSIT_IsWeaponBowOrXBow(pUnit))
    {
        if (SKILLS_HasAmmoForShot(pUnit))
            return TRUE;

        return FALSE;
    }

    pWeapon = D2COMMON_10434(pUnit, FALSE);
    if (pWeapon && D2COMMON_ITEMS_CheckItemTypeId(pWeapon, ITEMTYPE_MISSILE_POTION))
        return FALSE;

    return TRUE;
}


//1.10f: D2Client.0x6FABB9B0
//  `cltstfunc` 2, can throw start; it reads neither nSkillId nor nSkillLevel. SKILLS_HasAmmoForShot's 0 is answered as it is; past it, 1 only
//  when D2Common #10434 has a weapon (`neg / sbb / neg`, 0x6FABB9C8). The weapon test is there because SKILLS_HasAmmoForShot answers 1 for a player
//  with no weapon.
BOOL __fastcall SKILLS_CltStFunc2(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    if (!SKILLS_HasAmmoForShot(pUnit))
        return FALSE;

    return D2COMMON_10434(pUnit, FALSE) != NULL;
}


//1.10f: D2Client.0x6FABB9E0
//  `cltstfunc` 3, can unsummon start; it reads neither nSkillId nor nSkillLevel. The unit's path target must be a monster whose roster owner is
//  the unit itself (ROSTERPETS_GetOwnerId against pUnit's id, or -1 for a NULL pUnit) and whose pet type carries UNSUMMON in pettype.txt; the
//  answer is that flag test made 0 or 1 (`and al, dl / neg / sbb / neg`, 0x6FABBA68). The flag is tested on the low byte against the low byte
//  (`mov al, [edx+0Ch] / mov dl, [ecx+4] / and al, dl`); flag 3 lies in that byte. The pettype.txt record lookup is written in place.
BOOL __fastcall SKILLS_CltStFunc3(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2UnitStrc *pPathTarget;
    int32_t nPetType;
    D2PetTypeTxt *pPetTypeTxt;

    pPathTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (!pPathTarget)
        return FALSE;

    if (pPathTarget->dwUnitType != UNIT_MONSTER)
        return FALSE;

    //  BUG: pUnit is tested for NULL only after CUNIT_GetPathTargetUnit has dereferenced it, so the -1 arm (0x6FABBA0C) runs only if that call
    //  survives a NULL unit, which it does not. UNREACHABLE: gpSkillCltStFuncs is read by SKILLS_RunCltStFunc alone (0x6FABC41D), which has read
    //  pUnit->pDynamicPath before.
    if ((D2UnitGUID)ROSTERPETS_GetOwnerId(pPathTarget->dwUnitId) != (pUnit ? pUnit->dwUnitId : (D2UnitGUID)-1))
    {
        return FALSE;
    }

    nPetType = ROSTERPETS_GetPetType(pPathTarget->dwUnitId);

    if (nPetType < 0 || nPetType >= sgptDataTables->nPetTypeTxtRecordCount)
        return FALSE;

    pPetTypeTxt = &sgptDataTables->pPetTypeTxt[nPetType];
    if (!pPetTypeTxt)
        return FALSE;

    return ((uint8_t)gdwBitMasks[PETTYPEFLAGINDEX_UNSUMMON] & (uint8_t)pPetTypeTxt->dwPetFlags) != 0;
}


//1.10f: D2Client.0x6FABBA80
//  `cltstfunc` 4, Left Hand Swing always starts: `mov eax, 1 / retn 4`.
BOOL __fastcall SKILLS_CltStFunc4(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    return TRUE;
}


//1.10f: D2Client.0x6FABBA90
//  `cltdofunc` 1, the attack: Attack and Left Hand Swing. gpSkillCltDoFuncs' three-argument shape.
//  UNITFLAG_SKSRVDOFUNC is set on a non-NULL unit (the whole dword read, `or al, 40h`, stored). Without a bow or crossbow nothing is spawned: the
//  path target is fetched and handed with pUnit to MEMPART_Nullsub1, whose body is `retn` (ecx = pUnit, edx = the target, 0x6FABBC4D..0x6FABBC58),
//  and the answer is 1.
//  With one, D2Common #11039 names the missile, and its second argument is the address of this function's own nSkillLevel slot: `lea eax,
//  [esp+5Ch]` (8D 44 24 5C, 0x6FABBABC) with esp 58h below the entry, the first stack argument, read back at 0x6FABBAD4. For a player #11039
//  writes a magic arrow's level there, so the level a magic arrow carries replaces the skill level; -1 means no missile, and then the answer is 1
//  and nothing spawns. Otherwise the point is SKILLS_GetPathTargetPos's and, with both coordinates non-zero, MISSILE_ClientSpawnForOwner fires
//  at it; the answer is always 1.
//  BUG: the NULL test on pUnit guards the flag only; COMPOSIT_IsWeaponBowOrXBow and CUNIT_GetPathTargetUnit take it untested. UNREACHABLE: in
//  stock 1.10 slots 1 and 2 of gpSkillCltDoFuncs are indexed by Skills.txt `cltdofunc` alone, so only SKILLS_RunClientDoFunc reaches them, and
//  its two callers cannot pass NULL.
//  The original inlines SKILLS_GetPathTargetPos here.
int32_t __fastcall SKILLS_CltDoFunc1(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    int32_t nMissileId;
    int32_t nTargetX;
    int32_t nTargetY;

    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    if (!D2COMMON_COMPOSIT_IsWeaponBowOrXBow(pUnit))
    {
        CUNIT_GetPathTargetUnit(pUnit);
        MEMPART_Nullsub1();
        return 1;
    }

    nMissileId = D2COMMON_11039_CheckWeaponIsMissileBased(pUnit, &nSkillLevel);
    if (nMissileId == -1)
        return 1;

    SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY);

    if (nTargetX && nTargetY)
        MISSILE_ClientSpawnForOwner(pUnit, nMissileId, nTargetX, nTargetY, nSkillId, nSkillLevel);

    return 1;
}


//1.10f: D2Client.0x6FABBC70
//  `cltdofunc` 2, the throw: Throw and Left Hand Throw; nSkillLevel is not read.
//  UNITFLAG_SKSRVDOFUNC is set on a non-NULL unit, as in the attack. Without a weapon from D2Common #10434 the answer is 0; otherwise the missile
//  is the weapon's (ITEMS_GetMissileType), the point is SKILLS_GetPathTargetPos's, and the skill level passed on is 1 in both arms (`push 1`,
//  0x6FABBDC1 and 0x6FABBE92). A throwing potion goes to MISSILE_ClientSpawnSkillEffect with no spawn offset and a last argument of 0, and with
//  no test of the point for zero; the other arm tests both coordinates before MISSILE_ClientSpawnForOwner. The answer is 1 either way.
//  BUG: as in the attack, the NULL test on pUnit guards the flag only. UNREACHABLE, as there.
//  The original inlines SKILLS_GetPathTargetPos in both arms.
int32_t __fastcall SKILLS_CltDoFunc2(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    D2UnitStrc *pWeapon;
    int32_t nMissileId;
    int32_t nTargetX;
    int32_t nTargetY;

    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

    pWeapon = D2COMMON_10434(pUnit, FALSE);
    if (!pWeapon)
        return 0;

    nMissileId = D2COMMON_ITEMS_GetMissileType(pWeapon);
    if (D2COMMON_ITEMS_GetItemType(pWeapon) == ITEMTYPE_MISSILE_POTION)
    {
        SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY);

        MISSILE_ClientSpawnSkillEffect(pUnit, nMissileId, 0, 0, nTargetX, nTargetY, nSkillId, 1, 0);
        return 1;
    }

    SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY);

    if (nTargetX && nTargetY)
        MISSILE_ClientSpawnForOwner(pUnit, nMissileId, nTargetX, nTargetY, nSkillId, 1);

    return 1;
}


//1.10f: D2Client.0x6FABBEB0
//  SKILLS_RunCltStFunc with its last two arguments swapped: pUnit and nSkillId pass through in ecx and edx; nSkillLevel is pushed first, so it
//  lands last, and nOwnerGUID third (0x6FABBEB0..0x6FABBEB9). SKILLS_RunCltStFunc takes (pUnit, nSkillId, nOwnerGUID, nSkillLevel). Its one
//  caller is SKILLS_ClientCastAtTarget.
int32_t __fastcall SKILLS_RunCltStFuncSwapped(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, D2UnitGUID nOwnerGUID)
{
    return SKILLS_RunCltStFunc(pUnit, nSkillId, nOwnerGUID, nSkillLevel);
}


//  The client dispatch tables gpSkillCltStFuncs (70 slots) and gpSkillCltDoFuncs (130 slots) are in Skills.h. gpSkillCltStFuncs is indexed by
//  Skills.txt `cltstfunc` and read by SKILLS_RunCltStFunc alone; gpSkillCltDoFuncs by `cltdofunc`, `cltprgfunc1..3`, `ItemCltEffect` and
//  States.txt `cltactivefunc`, and read by SKILLS_RunClientDoFunc, the two halves of packet 0xA3 and SKILLS_StatesRunActiveStateFuncs. Slot N holds
//  SKILLS_CltStFuncN or SKILLS_CltDoFuncN (1..53 and 1..96); every other slot is NULL. One function sits in two slots: 0x6FAC54D0 is slot 47 and
//  slot 52 of gpSkillCltDoFuncs, so SKILLS_CltDoFunc47 stands in both. The bounds are the counts the bytes read, gnSkillCltStFuncCount (70) and
//  gnSkillCltDoFuncCount (130).
//
//  A slot of gpSkillCltDoFuncs by an index its caller has not bounded: ours inside the table, NULL outside it.
//  BUG: the PrgStack loop of SKILLS_ApplyClientSkillEffect indexes the table with an unbounded charge count and reads past its end. LATENT.
//  DIFFERENCE: the original reads its own .data at the same offset and calls what it finds there; ours answers an empty slot, which both callers
//  test: the PrgStack loop calls nothing, the last arm marks UNITFLAG_SKSRVDOFUNC.
static SKILLS_CltFuncFn SKILLS_GetCltDoFuncOrNull(int32_t nSlot)
{
    if (nSlot >= 0 && nSlot < 130)
        return gpSkillCltDoFuncs[nSlot];

    return NULL;
}


static_assert(offsetof(D2SkillsTxt, nLineOfSight) == 0x18F && offsetof(D2SkillsTxt, wCltStFunc) == 0xF2
                  && offsetof(D2SkillsTxt, wCltMissile) == 0xE8,
              "SKILLS_RunCltStFunc: `mov cl, [eax+18Fh]` (0x6FABBF19), `mov ax, [ebp+0F2h]` (0x6FABC3FC), "
              "`mov ax, [ebp+0E8h]` (0x6FABC5EB)");
static_assert(offsetof(D2SkillsTxt, wStSound) == 0xFC && offsetof(D2SkillsTxt, nStSoundClass) == 0xFE
                  && offsetof(D2SkillsTxt, wCastOverlay) == 0x106,
              "SKILLS_RunCltStFunc: `mov di, [ebp+0FCh]` (0x6FABC475), `mov di, [ebp+0FEh]` (0x6FABC521), "
              "`movsx edi, word ptr [ebp+106h]` (0x6FABC594)");
static_assert(offsetof(D2MissilesTxt, nLight) == 0x130 && offsetof(D2MissilesTxt, nRGB) == 0x132
                  && offsetof(D2MonStats2Txt, nHeight) == 0x0B,
              "SKILLS_RunCltStFunc: `mov al, [edi+130h]` (0x6FABC62E), `mov cl, [edi+132h]` (0x6FABC644), "
              "`mov al, [eax+0Bh]` (0x6FABC351)");
static_assert(SKILLSLOS_MISSILE_BARRIER == 1 && SKILLSLOS_PLAYER_PATH == 2 && SKILLSLOS_PLAYER_MONSTER == 3
                  && SKILLSLOS_PLAYER_FLYING == 4 && SKILLSLOS_RADIAL_BARRIER == 5
                  && COLLIDE_MASK_PLAYER_PATH == 0x1C09 && (COLLIDE_PLAYER | COLLIDE_MONSTER) == 0x180
                  && (COLLIDE_DOOR | COLLIDE_MISSILE_BARRIER) == 0x804
                  && (COLLIDE_DOOR | COLLIDE_MISSILE_BARRIER | COLLIDE_WALL) == 0x805,
              "the LineOfSight switch: `dec ecx / cmp ecx, 4` and its five masks (0x6FABBFCC..0x6FABBFF9)");
static_assert(SKILLSFLAGINDEX_REPEAT == 11 && SKILLSFLAGINDEX_STSUCCESSONLY == 12
                  && SKILLSFLAGINDEX_STSOUNDDELAY == 13 && SKILLSFLAGINDEX_WEAPONSND == 14
                  && STATE_INFERNO == 0x0C,
              "byte 1 of dwFlags against gdwBitMasks[3], [4], [5], [6] (0x6FABC3DC, 0x6FABC468, 0x6FABC49C, "
              "0x6FABC4DE); `push 0Ch` into STATES_CheckState (0x6FABC3C7)");


//1.10f: D2Client.0x6FABBED0
//  Starts a skill on the client: the Skills.txt `cltstfunc` dispatch, and around it everything else the client does when a unit begins one.
//  pUnit is in ecx and nSkillId in edx; nOwnerGUID and nSkillLevel are on the stack. It answers 0 when the skill cannot start, and otherwise the
//  start function's answer, 1 when the skill has none. In order:
//
//    1  the record (DATATBLS_GetSkillsTxtRecordById); none, 0.
//    2  LineOfSight. With the column set, the point the unit heads to (SKILLS_GetPathTargetPos) must be reachable from the unit through D2Common
//       #11026 under the column's collision mask; a column above 5 answers 0 outright.
//    3  a corpse gate: with a path target, a SearchEnemyXY skill that is not also TargetCorpse is refused on a dead monster (MONMODE_DEAD).
//    4  the skill itself. The unit's used skill is kept when its id is nSkillId (SKILLS_GetSkillIdFromSkill, line 1284); otherwise the skill of
//       that id and owner is taken, and when the unit has none it is added and assigned at nSkillLevel capped by the row's wMaxLvl (20 when the
//       column is 0 or less), the excess going to D2Common #11030. The local player's new unowned skill gets SKILLS_SetFlags 8. A skill that is
//       still missing answers 0; the one found becomes the used skill.
//    5  an item's skill (nOwnerGUID not -1) must answer SKILLS_GetSkillInfo and have charges left. SKILLS_GetSkillInfo writes the skill's own
//       level into the third out slot, and the slot passed is this function's nSkillLevel argument (`lea ecx, [esp+40h+nSkillLevel]`,
//       0x6FABC175): the charge's level replaces the one the caller gave.
//    6  the level again: the local player's is SKILLS_GetSkillLevel with its bonus; any other unit's skill is reassigned at nSkillLevel with
//       the same cap and #11030 (lines 1338 and 1343).
//    7  an ally gate: with a path target, a skill that is not TargetAlly is refused on a unit CUNIT_IsSelectableAsAllyTarget accepts.
//    8  the town gate: a skill that is not InTown, in a town room, drops the used skill, hands UNITMODE_DispatchActionByUnitType action 7 a
//       zeroed seven-dword block and bInterruptKnockback FALSE, plays the local player's sound event 0x18 (`<class>_not_in_town`) and answers 0.
//    9  the animation mode: SKILLS_GetSkillModeFromUnit's, except for a player's Attack (see the code). A mode of 0 becomes mode 1, neutral in
//       both PLRMODE_ and MONMODE_, for a player or a monster, unless the unit is in STATE_INFERNO and the skill is Repeat, when the mode is
//       left alone.
//    10 the start function: `cltstfunc` through gpSkillCltStFuncs, when the column is in [0, gnSkillCltStFuncCount) and the slot is not NULL;
//       else the answer stays 1. A start that answers 0 drops the used skill, puts a player or a monster back in mode 1, and, for a
//       StSuccessOnly skill, ends here with 0.
//    11 the start sounds: `stsound` (the item's `ItemCastSound` for an item's skill, when positive), the hit class's weapon sound for a
//       WeaponSnd skill, and `stsoundclass` for a player of the skill's class when no item sound was taken. An StSoundDelay skill delays the
//       first and the third by UNITSND_GetHitClassSoundDelay less 3, clamped at 0; the weapon sound is delayed always, by the same delay
//       without the 3.
//    12 the cast overlay: `castoverlay` (the item's `ItemCastOverlay` for an item's skill, when positive), added with kind 2 unless the unit
//       has it.
//    13 the cast light, when the answer is not 0: `cltmissile`'s missiles.txt row, when its Light is not 0, gives a light of radius 1 grown
//       toward Light, at intensity 255 in the row's colour; D2Common #10958 (SKILLS_SwapCastLight) stores it in the unit's skill list and hands
//       back the one stored before, which is removed.
//
//  The two caps of the level (the count test, wMaxLvl through the stride kept in ebp, 20) are two hand-written sites in the original, not an
//  inlined function, and are written in place.
//  The original inlines DATATBLS_GetSkillsTxtRecordById (twice), SKILLS_GetPathTargetPos and DATATBLS_GetMonStats2TxtRecord here; the
//  missiles.txt record lookup is written in place.
int32_t __fastcall SKILLS_RunCltStFunc(D2UnitStrc *pUnit, D2C_Skills nSkillId, D2UnitGUID nOwnerGUID, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;
    D2SkillsTxt *pTargetSkillsTxt;
    D2UnitStrc *pTarget;
    D2SkillStrc *pSkill;
    int32_t nPointX;
    int32_t nPointY;
    uint16_t nColMask;
    int32_t nMaxLvl;
    D2UnitGUID nOwnerOut;
    int32_t nSkillIdOut;
    int32_t nCharges;
    int32_t aActionParams[7];
    int32_t nMode;
    BOOL bMayFlip;
    D2MonStats2Txt *pMonStats2Txt;
    uint8_t nHeight;
    D2UnitStrc *pWeapon;
    int32_t nBodyLoc;
    BOOL bLeftHand;
    int16_t nCltStFunc;
    SKILLS_CltFuncFn pfnCltStFunc;
    int32_t nResult;
    int16_t nSoundId;
    D2C_Sounds nHitSound;
    int32_t nDelay;
    int32_t nOverlayId;
    int16_t nCltMissile;
    D2MissilesTxt *pMissilesTxt;
    D2ClientLightStrc *pLight;
    D2ClientLightStrc *pOldLight;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById((int32_t)nSkillId);
    if (!pSkillsTxt)
        return 0;

    if (pSkillsTxt->nLineOfSight != SKILLSLOS_NONE)
    {
        SKILLS_GetPathTargetPos(pUnit, &nPointX, &nPointY);

        switch (pSkillsTxt->nLineOfSight)
        {
        case SKILLSLOS_MISSILE_BARRIER:
            nColMask = COLLIDE_MISSILE_BARRIER;
            break;
        case SKILLSLOS_PLAYER_PATH:
            nColMask = COLLIDE_MASK_PLAYER_PATH;
            break;
        case SKILLSLOS_PLAYER_MONSTER:
            nColMask = (uint16_t)(COLLIDE_PLAYER | COLLIDE_MONSTER);
            break;
        case SKILLSLOS_PLAYER_FLYING:
            nColMask = (uint16_t)(COLLIDE_DOOR | COLLIDE_MISSILE_BARRIER);
            break;
        case SKILLSLOS_RADIAL_BARRIER:
            nColMask = (uint16_t)(COLLIDE_DOOR | COLLIDE_MISSILE_BARRIER | COLLIDE_WALL);
            break;
        default:
            return 0;
        }

        if (!D2COMMON_SKILLS_HasLineOfSightToUnit(nPointX, nPointY, pUnit, nColMask))
            return 0;
    }

    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (pTarget)
    {
        pTargetSkillsTxt = DATATBLS_GetSkillsTxtRecordById((int32_t)nSkillId);
        if (!pTargetSkillsTxt)
            return 0;

        //  Bytes against bytes: `mov dl, [ecx+6] / test [edi+14h], dl`, then
        //  `mov al, [ecx+7] / mov cl, [edi] / test cl, al` (0x6FABC046,
        //  0x6FABC059).
        if ((((uint8_t *)pTargetSkillsTxt->dwFlags)[SKILLSFLAGINDEX_SEARCHENEMYXY >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_SEARCHENEMYXY & 7]) && pTarget->dwUnitType == UNIT_MONSTER && pTarget->dwAnimMode == MONMODE_DEAD
            && !(((uint8_t *)pTargetSkillsTxt->dwFlags)[SKILLSFLAGINDEX_TARGETCORPSE >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_TARGETCORPSE & 7]))
        {
            return 0;
        }
    }

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill || D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillsCpp, 1284) != (int32_t)nSkillId)
    {
        pSkill = D2COMMON_SKILLS_GetSkillById(pUnit, nSkillId, nOwnerGUID);
        if (!pSkill)
        {
            D2COMMON_SKILLS_AddSkill(pUnit, nSkillId);
            pSkill = D2COMMON_SKILLS_GetSkillById(pUnit, nSkillId, nOwnerGUID);
            if (pSkill && pUnit == CUNIT_GetPlayer() && nOwnerGUID == (D2UnitGUID)-1)
                D2COMMON_SKILLS_SetFlags(pSkill, 8);

            //  The first cap: the count read fresh, wMaxLvl through the stride in ebp (`movsx edi, word ptr [ecx+ebp+12Ch]`, 0x6FABC0FA), no test of
            //  the row's address.
            if ((int32_t)nSkillId >= sgptDataTables->nSkillsTxtRecordCount || (nMaxLvl = (int16_t)sgptDataTables->pSkillsTxt[nSkillId].wMaxLvl) <= 0)
            {
                nMaxLvl = 20;
            }

            if (nSkillLevel > nMaxLvl)
            {
                D2COMMON_SKILLS_AssignSkill(pUnit, nSkillId, nMaxLvl, FALSE, szSkillsCpp, 1299);
                D2COMMON_SKILLS_SetSkillLevelBonus(pUnit, (int32_t)nSkillId, nSkillLevel - nMaxLvl);
            }
            else
            {
                D2COMMON_SKILLS_AssignSkill(pUnit, nSkillId, nSkillLevel, FALSE, szSkillsCpp, 1304);
            }

            if (!pSkill)
                return 0;
        }

        D2COMMON_UNITS_SetUsedSkill(pUnit, pSkill);
    }

    if (nOwnerGUID != (D2UnitGUID)-1)
    {
        //  The third out slot is this function's own nSkillLevel.
        if (!D2COMMON_SKILLS_GetSkillInfo(pSkill, &nOwnerOut, &nSkillIdOut, &nSkillLevel, &nCharges))
            return 0;

        if (!nCharges)
            return 0;
    }

    if (pUnit == CUNIT_GetPlayer())
    {
        nSkillLevel = D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill, TRUE);
    }
    else
    {
        //  The second cap, 0x6FABC1B0..0x6FABC1D6.
        if ((int32_t)nSkillId >= sgptDataTables->nSkillsTxtRecordCount || (nMaxLvl = (int16_t)sgptDataTables->pSkillsTxt[nSkillId].wMaxLvl) <= 0)
        {
            nMaxLvl = 20;
        }

        if (nSkillLevel > nMaxLvl)
        {
            D2COMMON_SKILLS_AssignSkill(pUnit, nSkillId, nMaxLvl, FALSE, szSkillsCpp, 1338);
            D2COMMON_SKILLS_SetSkillLevelBonus(pUnit, (int32_t)nSkillId, nSkillLevel - nMaxLvl);
        }
        else
        {
            D2COMMON_SKILLS_AssignSkill(pUnit, nSkillId, nSkillLevel, FALSE, szSkillsCpp, 1343);
        }
    }

    //  From here the row is the first lookup's: `mov ebp, [esp+40h+...]`
    //  (0x6FABC239) reloads it into ebp for every flag test below.
    pTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (pTarget && !(((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_TARGETALLY >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_TARGETALLY & 7]) && CUNIT_IsSelectableAsAllyTarget(pUnit, pTarget))
    {
        return 0;
    }

    if (!(((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_INTOWN >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_INTOWN & 7]) && D2COMMON_DUNGEON_IsRoomInTown(D2COMMON_UNITS_GetRoom(pUnit)))
    {
        D2COMMON_UNITS_SetUsedSkill(pUnit, NULL);
        memset(aActionParams, 0, sizeof(aActionParams));
        UNITMODE_DispatchActionByUnitType(CLTACTION_NEUTRAL, pUnit, aActionParams, FALSE);
        if (pUnit == CUNIT_GetPlayer())
            UNITSND_PlayPlayerSoundEvent(pUnit, 0x18u);

        return 0;
    }

    //  BUG: pUnit is tested for NULL (0x6FABC2CD, 0x6FABC3E1, 0x6FABC444, 0x6FABC53C) only after CUNIT_GetPathTargetUnit has read pUnit->pDynamicPath
    //  (0x6FABC01B) on every path. UNREACHABLE: every path to the four tests passes 0x6FABC01B, where a NULL unit faults first.
    //  The type is compared as a BYTE: `cmp byte ptr [esi], 0` (0x6FABC2D5).
    if (!pUnit || (uint8_t)pUnit->dwUnitType != UNIT_PLAYER || nSkillId != SKILL_ATTACK)
    {
        nMode = D2COMMON_SKILLS_GetSkillModeFromUnit(pUnit, pSkill);
    }
    else
    {
        //  A player's Attack: the first attack mode, rolled to the second
        //  one time in two (the seed's low six bits under 32, `jge` signed)
        //  when nothing forbids it. A monster target of height 2 forces the
        //  second, of height 1 forbids the roll; a bow or crossbow forces the
        //  first; and with no equipped weapon the first is taken, even over
        //  height 2.
        nMode = PLRMODE_ATTACK1;
        bMayFlip = TRUE;
        pTarget = CUNIT_GetPathTargetUnit(pUnit);
        if (pTarget && pTarget->dwUnitType == UNIT_MONSTER)
        {
            pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pTarget->dwClassId);

            //  BUG: the MonStats2 row is read with no NULL test (`mov al, [eax+0Bh]`, 0x6FABC351): a monster whose class or wMonStatsEx is out of
            //  range reads address 0x0B. LATENT: every stock monstats.txt row with an hcIdx names a MonStats2.txt row in MonStatsEx.
            //  One read, two compares.
            nHeight = pMonStats2Txt->nHeight;
            if (nHeight == 2)
            {
                nMode = PLRMODE_ATTACK2;
                bMayFlip = FALSE;
            }
            if (nHeight == 1)
                bMayFlip = FALSE;
        }

        if (D2COMMON_COMPOSIT_IsWeaponBowOrXBow(pUnit))
        {
            nMode = PLRMODE_ATTACK1;
            bMayFlip = FALSE;
        }

        if (!D2COMMON_INVENTORY_GetEquippedWeapon(pUnit->pInventory, &pWeapon, &nBodyLoc, &bLeftHand))
        {
            nMode = PLRMODE_ATTACK1;
        }
        else if (bMayFlip && (int32_t)((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) & 0x3F) < 0x20)
        {
            nMode = PLRMODE_ATTACK2;
        }
    }

    if (nMode)
    {
        UNITMODE_SetAnimMode(pUnit, nMode);
    }
    else if (!(D2COMMON_STATES_CheckState(pUnit, STATE_INFERNO) && (((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_REPEAT >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_REPEAT & 7]))
             && pUnit && (pUnit->dwUnitType == UNIT_PLAYER || pUnit->dwUnitType == UNIT_MONSTER))
    {
        //  Mode 1, `mov edx, 1` (0x6FABC3F0): PLRMODE_NEUTRAL and MONMODE_NEUTRAL.
        UNITMODE_SetAnimMode(pUnit, 1);
    }

    //  The column is a SIGNED word (`test ax, ax / jl`, then `movsx`), and its
    //  bound a signed compare (`jge`) against the count.
    nResult = 1;
    nCltStFunc = (int16_t)pSkillsTxt->wCltStFunc;
    if (nCltStFunc >= 0 && nCltStFunc < gnSkillCltStFuncCount)
    {
        pfnCltStFunc = gpSkillCltStFuncs[nCltStFunc];
        if (pfnCltStFunc)
        {
            nResult = pfnCltStFunc(pUnit, nSkillId, nSkillLevel);
            if (!nResult)
            {
                D2COMMON_UNITS_SetUsedSkill(pUnit, NULL);
                if (pUnit && (pUnit->dwUnitType == UNIT_PLAYER || pUnit->dwUnitType == UNIT_MONSTER))
                    UNITMODE_SetAnimMode(pUnit, 1);

                if (((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_STSUCCESSONLY >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_STSUCCESSONLY & 7])
                {
                    return nResult;
                }
            }
        }
    }

    //  Each sound column is a signed word (`test di, di / jle`, `movsx ecx,
    //  di`); the delay less 3 is clamped on its sign (`sub eax, 3 / sets /
    //  dec / and`), the weapon sound's delay on its own (`test / setl`).
    nSoundId = pSkillsTxt->wStSound;
    if (nOwnerGUID != (D2UnitGUID)-1 && (int16_t)pSkillsTxt->wItemCastSound > 0)
        nSoundId = (int16_t)pSkillsTxt->wItemCastSound;

    if (nSoundId > 0)
    {
        if (((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_STSOUNDDELAY >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_STSOUNDDELAY & 7])
        {
            nDelay = (int32_t)(UNITSND_GetHitClassSoundDelay(pUnit, D2COMMON_UNITS_GetHitClass(pUnit)) - 3u);
            if (nDelay < 0)
                nDelay = 0;

            SOUNDINST_PlaySound((D2C_Sounds)nSoundId, pUnit, nDelay, 0, 0);
        }
        else
        {
            SOUNDINST_PlaySound((D2C_Sounds)nSoundId, pUnit, 0, 0, 0);
        }
    }

    if (((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_WEAPONSND >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_WEAPONSND & 7])
    {
        nHitSound = UNITSND_GetHitClassSound(pUnit, D2COMMON_UNITS_GetHitClass(pUnit));
        if (nHitSound)
        {
            nDelay = (int32_t)UNITSND_GetHitClassSoundDelay(pUnit, D2COMMON_UNITS_GetHitClass(pUnit));
            if (nDelay < 0)
                nDelay = 0;

            SOUNDINST_PlaySound(nHitSound, pUnit, nDelay, 0, 0);
        }
    }

    //  The class's own line, `cmp dword ptr [esi], 0` and the class byte
    //  zero-extended (`xor edx, edx / mov dl, [ebp+0Ch]`, 0x6FABC548).
    nSoundId = (int16_t)pSkillsTxt->nStSoundClass;
    if ((nOwnerGUID == (D2UnitGUID)-1 || (int16_t)pSkillsTxt->wItemCastSound <= 0) && nSoundId > 0 && pUnit && pUnit->dwUnitType == UNIT_PLAYER && pUnit->dwClassId == (uint8_t)pSkillsTxt->nCharClass)
    {
        if (((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_STSOUNDDELAY >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_STSOUNDDELAY & 7])
        {
            nDelay = (int32_t)(UNITSND_GetHitClassSoundDelay(pUnit, D2COMMON_UNITS_GetHitClass(pUnit)) - 3u);
            if (nDelay < 0)
                nDelay = 0;

            SOUNDINST_PlaySound((D2C_Sounds)nSoundId, pUnit, nDelay, 0, 0);
        }
        else
        {
            SOUNDINST_PlaySound((D2C_Sounds)nSoundId, pUnit, 0, 0, 0);
        }
    }

    nOverlayId = (int16_t)pSkillsTxt->wCastOverlay;
    if (nOwnerGUID != (D2UnitGUID)-1 && (int16_t)pSkillsTxt->wItemCastOverlay > 0)
        nOverlayId = (int16_t)pSkillsTxt->wItemCastOverlay;

    if (nOverlayId > 0 && nOverlayId < sgptDataTables->nOverlayTxtRecordCount && !GFX_HasOverlay(pUnit, nOverlayId))
    {
        GFX_AddOverlay(pUnit, (D2_OverlayIds)nOverlayId, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);
    }

    if (nResult)
    {
        nCltMissile = (int16_t)pSkillsTxt->wCltMissile;
        if (nCltMissile > 0)
        {
            if (nCltMissile < 0 || nCltMissile >= sgptDataTables->nMissilesTxtRecordCount)
                pMissilesTxt = NULL;
            else
                pMissilesTxt = &sgptDataTables->pMissilesTxt[nCltMissile];

            //  BUG: the missiles.txt row is read with no NULL test (`mov al, [edi+130h]`, 0x6FABC62E): a column at or past the table's count
            //  reads address 0x130. LATENT: every `cltmissile` in stock Skills.txt names a Missiles.txt row.
            if (pMissilesTxt->nLight)
            {
                //  Each colour is pushed as a whole register of which only the low byte was loaded (`mov cl, [edi+132h] / push ecx`, 0x6FABC644);
                //  DLIGHTMAP_AddLight stores each into a byte, so the upper bytes are not a value.
                pLight = DLIGHTMAP_AddLight(pUnit, 1, 1, 0xFF, pMissilesTxt->nRGB[0], pMissilesTxt->nRGB[1], pMissilesTxt->nRGB[2]);
                DLIGHTMAP_SetTargetRadius(pLight, pMissilesTxt->nLight);
                pOldLight = (D2ClientLightStrc *)D2COMMON_SKILLS_SwapCastLight(pUnit, pLight);
                if (pOldLight)
                    DLIGHTMAP_RemoveLight(pOldLight);
            }
        }
    }

    return nResult;
}


static_assert(offsetof(D2SkillsTxt, wCltDoFunc) == 0xF4 && offsetof(D2SkillsTxt, wItemCltEffect) == 0x16C
                  && offsetof(D2SkillsTxt, dwDelay) == 0x190,
              "SKILLS_RunClientDoFunc: `mov ax, [ecx+0F4h]` (0x6FABC6F6), `mov cx, [ecx+16Ch]` (0x6FABC8DB), "
              "`mov ecx, [edi+190h]` (0x6FABC9CA)");
static_assert(offsetof(D2SkillsTxt, wDoSound) == 0x100 && offsetof(D2SkillsTxt, wTgtOverlay) == 0x108
                  && offsetof(D2SkillsTxt, wTgtSound) == 0x10A,
              "SKILLS_RunClientDoFunc: `mov ax, [edi+100h]` (0x6FABC94E); the sound at +10Ah (0x6FABC96A) and "
              "the overlay at +108h (0x6FABC993) -- the overlay's column lies first");
static_assert(SKILLSFLAGINDEX_LOB == 1 && SKILL_LEFTHANDSWING == 5,
              "gdwBitMasks[1] against byte 0 of dwFlags (`test [edx+4], al`, 0x6FABC860); "
              "`mov dword ptr [ebp+0], 5` (0x6FABD3C2) and `mov edx, 5` (0x6FABD3CE)");


//1.10f: D2Client.0x6FABC6B0
//  The client's "do" of a skill. pUnit is in ecx and nSkillId in edx; nSkillLevel and bUseTarget are on the stack. It answers 1 when something was
//  done and 0 otherwise. Two callers: SKILLS_ClientCastAtTarget, which reads the answer and passes its word argument nParam as bUseTarget, and
//  SKILLS_RunUsedSkillClientDoFunc, which passes 0. That word comes from the server's 0x99 and 0x9A, which come only from the server's unit
//  message queue; the word is the last argument of its item-skill casts: 1 for the attacker's cast of an on-hit skill, 0 elsewhere. So
//  bUseTarget, and with it the `ItemCltEffect` substitution below, can be 1 in play. In order:
//
//    1  the record (DATATBLS_GetSkillsTxtRecordById) and the `cltdofunc` column bounded against gnSkillCltDoFuncCount: no record, or a column
//       out of range, answers 0 before anything is done.
//    2  the missile, when `cltmissile` is positive. With bUseTarget it is aimed through the point the unit heads to
//       (SKILLS_GetPathTargetPos): the spawn offset is the point less the unit, so the missile appears AT the point, and its target is the
//       point plus that offset, twice as far from the unit. When either coordinate of the point is 0 the point itself goes as the offset and the
//       target stays 0. Without bUseTarget offset and target are 0: the missile starts at the unit, and the spawner aims it at the path target
//       itself. A Lob skill is spawned by MISSILE_ClientSpawnSkillEffect, every other by SKILLS_SpawnCltMissileStraight. A missile that spawns
//       counts as done; either way the unit gets UNITFLAG_SKSRVDOFUNC.
//    3  the do function: `cltdofunc` through gpSkillCltDoFuncs, with bUseTarget the `ItemCltEffect` column instead when it is above 1. A do
//       function that answers non-zero counts as done; a column out of range or an empty slot counts as done too, and sets
//       UNITFLAG_SKSRVDOFUNC.
//    4  the cast light SKILLS_RunCltStFunc stored in the skill list is taken back with D2Common #10958 and removed, done or not.
//    5  when something was done: `dosound` on the unit; `tgtsound` and `tgtoverlay` (kind 2) on the path target; and for the local player a
//       positive `delay` formula, capped at 12, blocks skills until that many frames from now, unless the skill's `anim` is PLRMODE_SEQUENCE
//       and dwSeqFrame has a bit set above its low byte.
//
//  The original inlines DATATBLS_GetSkillsTxtRecordById, SKILLS_GetPathTargetPos, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_RunClientDoFunc(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t bUseTarget)
{
    D2SkillsTxt *pSkillsTxt;
    BOOL bHandled;
    int32_t nSpawnOffsetX;
    int32_t nSpawnOffsetY;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nPointX;
    int32_t nPointY;
    D2UnitStrc *pSpawned;
    int32_t nCltDoFunc;
    SKILLS_CltFuncFn pfnCltDoFunc;
    D2ClientLightStrc *pLight;
    D2UnitStrc *pSoundTarget;
    D2UnitStrc *pOverlayTarget;
    int32_t nSkillDelay;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById((int32_t)nSkillId);
    if (!pSkillsTxt)
        return 0;

    //  The column is a SIGNED word (`cmp ax, si / jl`, then `movsx`), bound
    //  by a signed compare (`jge`) against the count beside the table.
    if ((int16_t)pSkillsTxt->wCltDoFunc < 0 || (int16_t)pSkillsTxt->wCltDoFunc >= gnSkillCltDoFuncCount)
    {
        return 0;
    }

    bHandled = FALSE;
    if ((int16_t)pSkillsTxt->wCltMissile > 0)
    {
        nSpawnOffsetX = 0;
        nSpawnOffsetY = 0;
        nTargetX = 0;
        nTargetY = 0;
        if (bUseTarget)
        {
            SKILLS_GetPathTargetPos(pUnit, &nPointX, &nPointY);
            nSpawnOffsetX = nPointX;
            nSpawnOffsetY = nPointY;
            if (nSpawnOffsetX && nSpawnOffsetY)
            {
                //  The target starts as the point (`mov ebp, edi`, 0x6FABC7EB; 0x6FABC7F0 for Y), the offset is the point less the unit, and the
                //  target is the point plus the offset.
                nTargetX = nSpawnOffsetX;
                nTargetY = nSpawnOffsetY;

                nSpawnOffsetX -= UNITS_GetXPosition(pUnit);

                nSpawnOffsetY -= UNITS_GetYPosition(pUnit);

                nTargetX += nSpawnOffsetX;
                nTargetY += nSpawnOffsetY;
            }
        }

        //  Byte 0 of dwFlags against gdwBitMasks[1] (`mov al, [ecx+4] / test
        //  [edx+4], al`, 0x6FABC85D): the Lob flag.
        if (((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_LOB >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_LOB & 7])
        {
            pSpawned = MISSILE_ClientSpawnSkillEffect(pUnit, (int16_t)pSkillsTxt->wCltMissile, nSpawnOffsetX, nSpawnOffsetY, nTargetX, nTargetY, (int32_t)nSkillId, nSkillLevel, 0);
        }
        else
        {
            pSpawned = SKILLS_SpawnCltMissileStraight((D2C_MissilesHcIdx)(int16_t)pSkillsTxt->wCltMissile, pUnit, nSkillId, nSkillLevel, nSpawnOffsetX, nSpawnOffsetY, nTargetX, nTargetY);
        }

        if (pSpawned)
            bHandled = TRUE;

        //  BUG: the first NULL test of pUnit (0x6FABC8B9; the second is 0x6FABC911): pUnit has gone untested to CUNIT_GetPathTargetUnit
        //  (with bUseTarget) and to either spawner, or with no missile to the do function. UNREACHABLE: neither caller can pass NULL
        //  (SKILLS_RunUsedSkillClientDoFunc hands pUnit to UNITS_GetUsedSkill first, SKILLS_ClientCastAtTarget to CUNIT_GetPathTargetUnit at
        //  0x6FABA657, which reads its pDynamicPath).
        //  The bit goes through the whole dword (`or al, 40h`, 0x6FABC8C4).
        if (pUnit)
            pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;
    }

    //  Both columns are signed words (`movsx`; `cmp cx, 1 / jle`); the bound is checked again on the column actually taken.
    nCltDoFunc = (int16_t)pSkillsTxt->wCltDoFunc;
    if (bUseTarget && (int16_t)pSkillsTxt->wItemCltEffect > 1)
        nCltDoFunc = (int16_t)pSkillsTxt->wItemCltEffect;

    if (nCltDoFunc >= 0 && nCltDoFunc < gnSkillCltDoFuncCount && (pfnCltDoFunc = gpSkillCltDoFuncs[nCltDoFunc]) != NULL)
    {
        if (pfnCltDoFunc(pUnit, nSkillId, nSkillLevel))
            bHandled = TRUE;
    }
    else
    {
        //  BUG: the second NULL test of pUnit, as above.
        if (pUnit)
            pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;

        bHandled = TRUE;
    }

    pLight = (D2ClientLightStrc *)D2COMMON_SKILLS_SwapCastLight(pUnit, NULL);
    if (pLight)
        DLIGHTMAP_RemoveLight(pLight);

    if (!bHandled)
        return 0;

    //  Each column is a signed word (`test ax, ax / jle`; `cmp word ptr
    //  [edi+10Ah], 0 / jle`), sign-extended where it is passed.
    if ((int16_t)pSkillsTxt->wDoSound > 0)
        SOUNDINST_PlaySound((D2C_Sounds)(int16_t)pSkillsTxt->wDoSound, pUnit, 0, 0, 0);

    if ((int16_t)pSkillsTxt->wTgtSound > 0)
    {
        pSoundTarget = CUNIT_GetPathTargetUnit(pUnit);
        if (pSoundTarget)
            SOUNDINST_PlaySound((D2C_Sounds)(int16_t)pSkillsTxt->wTgtSound, pSoundTarget, 0, 0, 0);
    }

    //  No bound on the overlay and no GFX_HasOverlay test, where SKILLS_RunCltStFunc's cast overlay has both.
    if ((int16_t)pSkillsTxt->wTgtOverlay > 0)
    {
        pOverlayTarget = CUNIT_GetPathTargetUnit(pUnit);
        if (pOverlayTarget)
            GFX_AddOverlay(pOverlayTarget, (D2_OverlayIds)(int16_t)pSkillsTxt->wTgtOverlay, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);
    }

    //  The delay's tests are signed (`jle` twice); `anim` is compared as a byte (`cmp byte ptr [edi+10h], 12h`) and dwSeqFrame tested whole
    //  (`test dword ptr [ebx+38h], 0FFFFFF00h`).
    nSkillDelay = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwDelay, nSkillId, nSkillLevel);
    if (nSkillDelay > 0 && CUNIT_GetPlayer() == pUnit)
    {
        if (nSkillDelay > 12)
            nSkillDelay = 12;

        if (pSkillsTxt->nAnim != PLRMODE_SEQUENCE || !(pUnit->dwSeqFrame & 0xFFFFFF00))
            GAME_SetSkillBlockUntilFrame(GAME_GetFrameCount() + nSkillDelay);
    }

    return 1;
}


//1.10f: D2Client.0x6FABCA30
//  The "do" of the unit's used skill. pUnit is in ecx. The used skill's id (line 1687) and its level with bonus go to SKILLS_RunClientDoFunc with
//  bUseTarget 0; with no used skill nothing is done.
//  void: eax at the exit is the used skill only where that is NULL, and otherwise SKILLS_RunClientDoFunc's answer; no caller reads it.
void __fastcall SKILLS_RunUsedSkillClientDoFunc(D2UnitStrc *pUnit)
{
    D2SkillStrc *pSkill;
    D2C_Skills nSkillId;
    int32_t nSkillLevel;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return;

    nSkillId = (D2C_Skills)D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillsCpp, 1687);
    nSkillLevel = D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill, TRUE);
    SKILLS_RunClientDoFunc(pUnit, nSkillId, nSkillLevel, FALSE);
}


//1.10f: D2Client.0x6FABD2A0
//  Starts a skill on a unit, for a client action: pUnit is in ecx and pParams in edx. pParams is the client action's parameter block, read here as
//
//    [0] nSkillId       0 is SKILL_ATTACK, which the swap of hands can turn into 5, SKILL_LEFTHANDSWING
//    [1] nOwnerGUID     SKILLS_RunCltStFunc's third argument
//    [2] the target's unit type, [3] its GUID
//    [4] nSkillLevel    SKILLS_RunCltStFunc's fourth
//
//  A unit that can dual wield and is not a monster first has its hand swapped (SKILLS_CltAlternateDualWieldHand). The target is looked up by
//  GUID and type; found, it becomes the dynamic target, the unit is turned toward it over time (CUNIT_FaceUnitAtPathTarget, bSnap 0) and the
//  start's answer is this function's. Not found: UNITMODE_DispatchActionByUnitType gets action 7 with no block and bInterruptKnockback FALSE,
//  D2Common #11100 (LOG_11100) logs "StartsStartSkillID - Invalid Target" at line 1986 (the string at 0x6FB777FC), and the answer is 0.
//  BUG: pUnit is tested for NULL (0x6FABD2B0) only after UNITS_CanDualWield has taken it, and is then handed on untested to
//  UNITS_SetTargetUnitForDynamicUnit or UNITMODE_DispatchActionByUnitType. UNREACHABLE: the test sits behind `&&` on UNITS_CanDualWield, which
//  answers FALSE for NULL, so its NULL arm cannot run.
int32_t __fastcall SKILLS_CltStartSkillOnUnit(D2UnitStrc *pUnit, int32_t *pParams)
{
    D2UnitStrc *pTarget;

    if (D2COMMON_UNITS_CanDualWield(pUnit) && (!pUnit || pUnit->dwUnitType != UNIT_MONSTER))
        SKILLS_CltAlternateDualWieldHand(pUnit, pParams);

    pTarget = CUNIT_FindUnitByGuidAndType((uint32_t)pParams[3], (D2C_UnitTypes)pParams[2]);
    if (!pTarget)
    {
        UNITMODE_DispatchActionByUnitType(CLTACTION_NEUTRAL, pUnit, NULL, FALSE);
        D2COMMON_LOG_11100(1, 2, 4, 0x40, szSkillsCpp, 1986, "StartsStartSkillID - Invalid Target");
        return 0;
    }

    D2COMMON_UNITS_SetTargetUnitForDynamicUnit(pUnit, pTarget);
    CUNIT_FaceUnitAtPathTarget(pUnit, 0);
    return SKILLS_RunCltStFunc(pUnit, (D2C_Skills)pParams[0], (D2UnitGUID)pParams[1], pParams[4]);
}


//1.10f: D2Client.0x6FABD330
//  Swaps the hand a dual wielder swings with: pUnit is in ecx and the action block in edx. Only for Attack (pParams[0] 0), and only when both
//  hands hold an ITEMTYPE_WEAPON; a hand whose item cannot be equipped, or is a throwing potion (ITEMTYPE_MISSILE_POTION), counts as empty. The
//  latch is the Attack skill's own Param4: set, it is cleared and Attack becomes the used skill; clear, it is set to 5, the block's skill becomes
//  SKILL_LEFTHANDSWING and so does the used skill. So consecutive attacks alternate hands. The value 5 of the latch is written as 5: nothing but
//  its non-zero is read (`test eax, eax`, 0x6FABD3BB).
void __fastcall SKILLS_CltAlternateDualWieldHand(D2UnitStrc *pUnit, int32_t *pParams)
{
    D2InventoryStrc *pInventory;
    D2UnitStrc *pRightItem;
    D2UnitStrc *pLeftItem;
    D2SkillStrc *pAttackSkill;

    if (pParams[0] != (int32_t)SKILL_ATTACK)
        return;

    pInventory = pUnit->pInventory;
    pRightItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RARM);
    if (pRightItem && (!D2COMMON_ITEMS_CanBeEquipped(pRightItem) || D2COMMON_ITEMS_GetItemType(pRightItem) == ITEMTYPE_MISSILE_POTION))
    {
        pRightItem = NULL;
    }

    pLeftItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_LARM);
    if (pLeftItem && (!D2COMMON_ITEMS_CanBeEquipped(pLeftItem) || D2COMMON_ITEMS_GetItemType(pLeftItem) == ITEMTYPE_MISSILE_POTION))
    {
        pLeftItem = NULL;
    }

    if (!D2COMMON_ITEMS_CheckItemTypeId(pRightItem, ITEMTYPE_WEAPON) || !D2COMMON_ITEMS_CheckItemTypeId(pLeftItem, ITEMTYPE_WEAPON))
    {
        return;
    }

    //  BUG: the Attack skill goes to SKILLS_GetParam4 and SKILLS_SetParam4 untested, and pUnit->pInventory is read untested at 0x6FABD341.
    //  LATENT: both callers test UNITS_CanDualWield first, which answers FALSE for NULL and passes only barbarians, assassins, Shadow Warriors and
    //  Shadow Masters; a player is given Attack when its skill list is made, and both shadows carry Attack in monstats.txt's Skill7 (that the
    //  client's copy of a shadow is given its monstats skills was not checked).
    pAttackSkill = D2COMMON_SKILLS_GetSkillById(pUnit, SKILL_ATTACK, (D2UnitGUID)-1);
    if (D2COMMON_SKILLS_GetParam4(pAttackSkill))
    {
        D2COMMON_SKILLS_SetParam4(pAttackSkill, 0);
        D2COMMON_UNITS_SetUsedSkill(pUnit, pAttackSkill);
    }
    else
    {
        //  The block's store sits between SetParam4's pushes and its call
        //  (0x6FABD3C2); scheduling.
        pParams[0] = SKILL_LEFTHANDSWING;
        D2COMMON_SKILLS_SetParam4(pAttackSkill, 5);
        D2COMMON_UNITS_SetUsedSkill(pUnit, D2COMMON_SKILLS_GetSkillById(pUnit, SKILL_LEFTHANDSWING, (D2UnitGUID)-1));
    }
}


//1.10f: D2Client.0x6FABD400
//  Starts a skill at a point, for a client action: pUnit is in ecx and pParams in edx. The same block as the start on a unit, with [2] and [3]
//  the point's X and Y. A unit that can dual wield has its hand swapped, a monster too, where the start on a unit spares it. The path's target
//  becomes the point, the unit's direction and speed are updated toward it, the dynamic target is dropped, and the start's answer is this
//  function's.
//  One monster family is turned an eighth of a turn. For a monster whose monstats.txt `Code` is "PB" (dwCode 0x20204250, `P`, `B` and two
//  spaces, little-endian), the new direction plus 56, modulo the 64 directions, minus eight, is written back. In the stock monstats.txt four
//  rows carry that code: turret1, turret2, turret3 and firetower.
//  The modulo is a signed `% 64` on an int (`and eax, 8000003Fh / jns / dec / or eax, 0FFFFFFC0h / inc`, 0x6FABD484), whose fix-up cannot run:
//  the direction is zero-extended from a byte (`and eax, 0FFh`) and 56 added. The result is stored as a byte into the slot of this function's
//  `push ecx` (0x6FABD400), pUnit's, and the whole dword is loaded (0x6FABD497) and pushed: D2Common #10160 takes a uint8_t, so pUnit's three
//  upper bytes are not a value.
//  pUnit is read untested from 0x6FABD422 on (`mov eax, [esi+2Ch]`).
//  The original inlines DATATBLS_GetMonStatsTxtRecord here.
int32_t __fastcall SKILLS_CltStartSkillAtCoords(D2UnitStrc *pUnit, int32_t *pParams)
{
    int32_t nX;
    int32_t nY;
    D2MonStatsTxt *pMonStatsTxt;
    uint8_t nDirection;

    nX = pParams[2];
    nY = pParams[3];
    if (D2COMMON_UNITS_CanDualWield(pUnit))
        SKILLS_CltAlternateDualWieldHand(pUnit, pParams);

    D2COMMON_10170_PathSetTargetPos(pUnit->pDynamicPath, nX, nY);
    D2COMMON_UNITS_UpdateDirectionAndSpeed(pUnit, nX, nY);
    if (pUnit->dwUnitType == UNIT_MONSTER)
    {
        pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);
        if (pMonStatsTxt && pMonStatsTxt->dwCode == 0x20204250)
        {
            nDirection = (uint8_t)((D2COMMON_PATH_GetNewDirection(pUnit->pDynamicPath) + 56) % 64);
            D2COMMON_10160_PathUpdateDirection(pUnit->pDynamicPath, nDirection);
        }
    }

    D2COMMON_UNITS_SetTargetUnitForDynamicUnit(pUnit, NULL);
    return SKILLS_RunCltStFunc(pUnit, (D2C_Skills)pParams[0], (D2UnitGUID)pParams[1], pParams[4]);
}


//  Server packet 0xA3 has two halves, SKILLS_ApplyClientSkillEffectNonProgressive and SKILLS_ApplyClientSkillEffect (the progressive one).
//  SCMD_HandleSrvA3_SkillEffect picks one by the skill's PROGRESSIVE flag and hands both the same seven arguments, the last the packet's byte at +1.
//  The six PROGRESSIVE rows of the stock Skills.txt are all the assassin's. The seed: with nX set and nY 0 both halves store nX into the unit's
//  dwInitSeed for the length of the effect, and the non-progressive half, given a target or a point, stores the byte there first.

static_assert(offsetof(D2SkillsTxt, nAuraState) == 0x80 && offsetof(D2SkillsTxt, wCltPrgFunc) == 0xF6
                  && sizeof(D2SkillsTxt::wCltPrgFunc) == 6
                  && offsetof(D2SkillsTxt, wCltPrgFunc) + sizeof(D2SkillsTxt::wCltPrgFunc)
                         == offsetof(D2SkillsTxt, wStSound),
              "SKILLS_ApplyClientSkillEffect: `mov cx, [ebx+80h]` (0x6FABCDC1), `lea ebx, [edx+0F6h]` "
              "(0x6FABCF61) and `mov ax, [ebx+ecx*2+0F6h]` (0x6FABCFCA) -- the fourth column is wStSound");
static_assert(offsetof(D2DataTablesStrc, nStatesTxtRecordCount) == 0xC4 && SKILLSFLAGINDEX_PRGSTACK == 7,
              "SKILLS_ApplyClientSkillEffect: `mov edx, [eax+0C4h]` (0x6FABCDD1); gdwBitMasks[7] "
              "(`mov dl, [ecx+1Ch]`, 0x6FABCF45)");


//1.10f: D2Client.0x6FABCA70
//  Packet 0xA3, the non-progressive half: pUnit is in ecx and pTarget in edx, the five other arguments on the stack. The shape is save, point,
//  run, put back, as in SKILLS_ClientCastAtTarget:
//
//    1  no unit, or no record (DATATBLS_GetSkillsTxtRecordById): nothing. The unit's dwInitSeed is saved, and the point it heads to is taken
//       (SKILLS_GetPathTargetPos).
//    2  the unit is pointed: at pTarget as its dynamic target, or, with none and both coordinates set, at (nX, nY); either way dwInitSeed becomes
//       nSeed. Then, nX set and nY 0, dwInitSeed becomes nX.
//    3  the first `cltprgfunc` column through gpSkillCltDoFuncs, bounded against gnSkillCltDoFuncCount; when it answers non-zero, `dosound` on
//       the unit and `tgtsound` and `tgtoverlay` on the path target, as in SKILLS_RunClientDoFunc.
//    4  put back: the dynamic target read just before step 2; with none, the path target set to step 1's point; and dwInitSeed.
//
//  The byte arrives with a pointer's upper bytes: SCMD_HandleSrvA3_SkillEffect loads it with `mov al, [esi+1]` (0x6FAB2DFD) into an eax whose upper
//  three bytes are the Skills.txt row's address, and this function masks it (`and ecx, 0FFh` at 0x6FABCC40, `and eax, 0FFh` at 0x6FABCC62), so the
//  parameter is a byte. The original inlines DATATBLS_GetSkillsTxtRecordById and SKILLS_GetPathTargetPos here.
void __fastcall SKILLS_ApplyClientSkillEffectNonProgressive(D2UnitStrc *pUnit, D2UnitStrc *pTarget, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nX, int32_t nY, uint8_t nSeed)
{
    D2SkillsTxt *pSkillsTxt;
    uint32_t dwSavedSeed;
    int32_t nPointX;
    int32_t nPointY;
    D2UnitStrc *pSavedTarget;
    int16_t nCltPrgFunc;
    SKILLS_CltFuncFn pfnCltPrgFunc;
    D2UnitStrc *pSoundTarget;
    D2UnitStrc *pOverlayTarget;

    if (!pUnit)
        return;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById((int32_t)nSkillId);
    if (!pSkillsTxt)
        return;

    dwSavedSeed = pUnit->dwInitSeed;

    SKILLS_GetPathTargetPos(pUnit, &nPointX, &nPointY);

    pSavedTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (pTarget)
    {
        D2COMMON_UNITS_SetTargetUnitForDynamicUnit(pUnit, pTarget);
        pUnit->dwInitSeed = nSeed;
    }
    else if (nX && nY)
    {
        D2COMMON_10170_PathSetTargetPos(pUnit->pDynamicPath, nX, nY);
        pUnit->dwInitSeed = nSeed;
    }

    //  With neither a target nor nX the bytes jump past this test to the
    //  column (0x6FABCC4D); it is false there anyway.
    if (nX && !nY)
        pUnit->dwInitSeed = (uint32_t)nX;

    //  The column is a signed word (`test ax, ax / jl`, then `movsx`),
    //  bounded by a signed compare (`jge`).
    nCltPrgFunc = (int16_t)pSkillsTxt->wCltPrgFunc[0];
    if (nCltPrgFunc >= 0 && nCltPrgFunc < gnSkillCltDoFuncCount)
    {
        pfnCltPrgFunc = gpSkillCltDoFuncs[nCltPrgFunc];
        if (pfnCltPrgFunc && pfnCltPrgFunc(pUnit, nSkillId, nSkillLevel))
        {
            if ((int16_t)pSkillsTxt->wDoSound > 0)
                SOUNDINST_PlaySound((D2C_Sounds)(int16_t)pSkillsTxt->wDoSound, pUnit, 0, 0, 0);

            if ((int16_t)pSkillsTxt->wTgtSound > 0)
            {
                pSoundTarget = CUNIT_GetPathTargetUnit(pUnit);
                if (pSoundTarget)
                    SOUNDINST_PlaySound((D2C_Sounds)(int16_t)pSkillsTxt->wTgtSound, pSoundTarget, 0, 0, 0);
            }

            if ((int16_t)pSkillsTxt->wTgtOverlay > 0)
            {
                pOverlayTarget = CUNIT_GetPathTargetUnit(pUnit);
                if (pOverlayTarget)
                    GFX_AddOverlay(pOverlayTarget, (D2_OverlayIds)(int16_t)pSkillsTxt->wTgtOverlay, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);
            }
        }
    }

    D2COMMON_UNITS_SetTargetUnitForDynamicUnit(pUnit, pSavedTarget);
    if (!pSavedTarget)
        D2COMMON_10170_PathSetTargetPos(pUnit->pDynamicPath, nPointX, nPointY);

    pUnit->dwInitSeed = dwSavedSeed;
}


//1.10f: D2Client.0x6FABCD70
//  Packet 0xA3, the progressive half: pUnit is in ecx and pTarget in edx, the five other arguments on the stack. nCharges is the charge count the
//  server released: it is the aura stat's value, and nCharges - 1 picks the `cltprgfunc` column. In order:
//
//    1  no unit, no record, an `aurastate` outside states.txt or an `aurastat1` outside itemstatcost.txt: nothing.
//    2  the unit's stat list of the aura state; when it has none, one is allocated (no pool, no filter, no timeout, the unit's type and GUID),
//       given the state, posted to the unit (D2Common #10475, reset flag 1) and given the aura stat at nCharges.
//    3  the point the unit heads to (SKILLS_GetPathTargetPos); the unit pointed at pTarget or at (nX, nY) with both set; dwInitSeed saved, and nX
//       stored there when nY is 0 (nSeed is not).
//    4  a PrgStack skill replays the lower charges first: for each column below nCharges - 1 whose gpSkillCltDoFuncs slot is not NULL, the aura
//       stat is set to that charge and the function called; then the stat is set to nCharges.
//    5  the column of nCharges - 1, when it is in [0, the count): its `cltmissile`, when positive (a Lob skill through
//       MISSILE_ClientSpawnSkillEffect at the point, offset 0, whatever the point is; any other through MISSILE_ClientSpawnForOwner when both
//       coordinates are set), then UNITFLAG_SKSRVDOFUNC; then its function: an empty slot sets UNITFLAG_SKSRVDOFUNC, a non-zero answer plays
//       `dosound`, `tgtsound` and `tgtoverlay` as the other half does.
//    6  put back: the saved dynamic target, and the path target set to step 3's point whether or not there was one (the other half does it only
//       with none); dwInitSeed only when step 3 overwrote it.
//
//  BUG: nCharges indexes the three `cltprgfunc` columns with no bound, and the PrgStack columns index gpSkillCltDoFuncs with no bound. LATENT: the
//  one sender of a progressive skill clamps the count to 1..3, and the three stock PrgStack rows (Fists of Fire, Claws of Thunder, Blades of Ice)
//  carry `cltprgfunc` values 9, 10 and 11 or none. nCharges comes from the packet as 0..255 (SCMD_HandleSrvA3_SkillEffect zero-extends it, `xor eax,
//  eax / mov al, [esi+1]`, 0x6FAB2DDD); nCharges 0 reads the column at -1, wCltDoFunc at +0F4h, and 4 reads wStSound at +0FCh, each as a table index.
//  The C reads the columns as the bytes do, a word at +0F6h plus twice the index from the row, so an index past the three stays a defined read of the
//  table. DIFFERENCE: the original indexes gpSkillCltDoFuncs with the value read, past the table's end if need be; ours goes through
//  SKILLS_GetCltDoFuncOrNull, which answers NULL there. pUnit's dwFlags are written with no NULL test, and need none: the entry has tested it
//  (0x6FABCD78). The original inlines DATATBLS_GetSkillsTxtRecordById and SKILLS_GetPathTargetPos (three times) here.
void __fastcall SKILLS_ApplyClientSkillEffect(D2UnitStrc *pUnit, D2UnitStrc *pTarget, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nX, int32_t nY, int32_t nCharges)
{
    D2SkillsTxt *pSkillsTxt;
    const char *pCltPrgFuncs;
    D2StatListStrc *pStatList;
    int32_t nPointX;
    int32_t nPointY;
    D2UnitStrc *pSavedTarget;
    uint32_t dwSavedSeed;
    int32_t nChargeIndex;
    int32_t iCharge;
    int16_t nCltPrgFunc;
    int32_t nMissileX;
    int32_t nMissileY;
    SKILLS_CltFuncFn pfnCltPrgFunc;
    D2UnitStrc *pSoundTarget;
    D2UnitStrc *pOverlayTarget;

    if (!pUnit)
        return;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById((int32_t)nSkillId);
    if (!pSkillsTxt)
        return;

    //  Both columns are signed words (`test cx, cx / jl`, `test dx, dx /
    //  jl`), bounded with `jge`.
    if (pSkillsTxt->nAuraState < 0 || pSkillsTxt->nAuraState >= sgptDataTables->nStatesTxtRecordCount)
        return;

    if (pSkillsTxt->wAuraStat[0] < 0 || pSkillsTxt->wAuraStat[0] >= sgptDataTables->nItemStatCostTxtRecordCount)
        return;

    pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, (D2C_States)pSkillsTxt->nAuraState);
    if (!pStatList)
    {
        pStatList = D2COMMON_STATLIST_AllocStatList(NULL, 0, 0, pUnit->dwUnitType, pUnit->dwUnitId);
        if (!pStatList)
            return;

        D2COMMON_STATLIST_SetState(pStatList, (D2C_States)pSkillsTxt->nAuraState);
        D2COMMON_10475_PostStatToStatList(pUnit, pStatList, TRUE);
        D2COMMON_STATLIST_SetStatIfListIsValid(pStatList, (D2C_ItemStats)pSkillsTxt->wAuraStat[0], nCharges, 0);
    }

    SKILLS_GetPathTargetPos(pUnit, &nPointX, &nPointY);

    pSavedTarget = CUNIT_GetPathTargetUnit(pUnit);
    if (pTarget)
        D2COMMON_UNITS_SetTargetUnitForDynamicUnit(pUnit, pTarget);
    else if (nX && nY)
        D2COMMON_10170_PathSetTargetPos(pUnit->pDynamicPath, nX, nY);

    dwSavedSeed = pUnit->dwInitSeed;
    if (nX && !nY)
        pUnit->dwInitSeed = (uint32_t)nX;

    //  BUG: no bound on nChargeIndex (LATENT, see the head). The columns are read by their offset from the row (`[ebx+ecx*2+0F6h]`, the loop's
    //  `lea ebx, [edx+0F6h]` stepped by 2), which keeps an index past the three a read of the table and not of an array past its end.
    nChargeIndex = nCharges - 1;
    pCltPrgFuncs = (const char *)pSkillsTxt + offsetof(D2SkillsTxt, wCltPrgFunc);

    //  Byte 0 of dwFlags against gdwBitMasks[7] (`mov dl, [ecx+1Ch] / test
    //  al, dl`, 0x6FABCF45): PrgStack.
    if (((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_PRGSTACK >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_PRGSTACK & 7])
    {
        //  `test eax, eax / jle`, then `jl` on the counter: signed. The column and its slot are read twice, as the bytes read them (`movsx eax,
        //  word ptr [ebx]` at 0x6FABCF67 and 0x6FABCF8A; the slot tested at 0x6FABCF6A and called through at 0x6FABCF98). BUG: no bound on the
        //  column, as in the head; both reads go through SKILLS_GetCltDoFuncOrNull.
        for (iCharge = 0; iCharge < nChargeIndex; ++iCharge)
        {
            if (SKILLS_GetCltDoFuncOrNull(*(const int16_t *)(pCltPrgFuncs + 2 * iCharge)))
            {
                D2COMMON_STATLIST_SetStatIfListIsValid(pStatList, (D2C_ItemStats)pSkillsTxt->wAuraStat[0], iCharge + 1, 0);
                SKILLS_GetCltDoFuncOrNull(*(const int16_t *)(pCltPrgFuncs + 2 * iCharge))(pUnit, nSkillId, nSkillLevel);
            }
        }

        D2COMMON_STATLIST_SetStatIfListIsValid(pStatList, (D2C_ItemStats)pSkillsTxt->wAuraStat[0], nChargeIndex + 1, 0);
    }

    nCltPrgFunc = *(const int16_t *)(pCltPrgFuncs + 2 * nChargeIndex);
    if (nCltPrgFunc >= 0 && nCltPrgFunc < gnSkillCltDoFuncCount)
    {
        if ((int16_t)pSkillsTxt->wCltMissile > 0)
        {
            //  Byte 0 of dwFlags against gdwBitMasks[1] (0x6FABD003): the Lob flag, as in SKILLS_RunClientDoFunc.
            if (((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_LOB >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_LOB & 7])
            {
                SKILLS_GetPathTargetPos(pUnit, &nMissileX, &nMissileY);
                MISSILE_ClientSpawnSkillEffect(pUnit, (int16_t)pSkillsTxt->wCltMissile, 0, 0, nMissileX, nMissileY, (int32_t)nSkillId, nSkillLevel, 0);
            }
            else
            {
                SKILLS_GetPathTargetPos(pUnit, &nMissileX, &nMissileY);
                if (nMissileX && nMissileY)
                {
                    MISSILE_ClientSpawnForOwner(pUnit, (int16_t)pSkillsTxt->wCltMissile, nMissileX, nMissileY, (int32_t)nSkillId, nSkillLevel);
                }
            }

            //  The bit goes through the whole dword (`or al, 40h`, 0x6FABD1A5).
            pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;
        }

        //  The column is read again (`movsx ecx, word ptr [ebx+eax*2+0F6h]`, 0x6FABD1B1), the bound not tested again, so it goes through
        //  SKILLS_GetCltDoFuncOrNull, which answers our slot for the value just tested.
        pfnCltPrgFunc = SKILLS_GetCltDoFuncOrNull(*(const int16_t *)(pCltPrgFuncs + 2 * nChargeIndex));
        if (!pfnCltPrgFunc)
        {
            pUnit->dwFlags |= UNITFLAG_SKSRVDOFUNC;
        }
        else if (pfnCltPrgFunc(pUnit, nSkillId, nSkillLevel))
        {
            if ((int16_t)pSkillsTxt->wDoSound > 0)
                SOUNDINST_PlaySound((D2C_Sounds)(int16_t)pSkillsTxt->wDoSound, pUnit, 0, 0, 0);

            if ((int16_t)pSkillsTxt->wTgtSound > 0)
            {
                pSoundTarget = CUNIT_GetPathTargetUnit(pUnit);
                if (pSoundTarget)
                    SOUNDINST_PlaySound((D2C_Sounds)(int16_t)pSkillsTxt->wTgtSound, pSoundTarget, 0, 0, 0);
            }

            if ((int16_t)pSkillsTxt->wTgtOverlay > 0)
            {
                pOverlayTarget = CUNIT_GetPathTargetUnit(pUnit);
                if (pOverlayTarget)
                    GFX_AddOverlay(pOverlayTarget, (D2_OverlayIds)(int16_t)pSkillsTxt->wTgtOverlay, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);
            }
        }
    }

    D2COMMON_UNITS_SetTargetUnitForDynamicUnit(pUnit, pSavedTarget);
    D2COMMON_10170_PathSetTargetPos(pUnit->pDynamicPath, nPointX, nPointY);
    if (nX && !nY)
        pUnit->dwInitSeed = dwSavedSeed;
}


static_assert(offsetof(D2MissileStrc, nVelocity) == 0x28,
              "SKILLS_SpawnMissileRingFromUnit: the velocity's store (0x6FABD582), IDA's [esp+0C4h+var_90], 28h into "
              "the block at [esp+0C4h+pParams]");
static_assert(MISSILE_DIABFIRE == 176, "SKILLS_SpawnMissileRing: `mov ebx, 0B0h` (0x6FABD647)");


//1.10f: D2Client.0x6FABD4D0
//  A ring of 64 missiles around a unit: pOwner is in ecx and pUnit in edx, the five other arguments on the stack. It fills a D2MissileStrc on
//  the stack (zeroed with `rep stosd`, flags 3, the centre at pUnit's client position, the owner, the missile, the skill and its level and, with
//  a velocity given, that velocity and flag bit 2) and hands it to SKILLS_SpawnMissileRing. With nAltMissileId not -1, a copy of the block
//  (`rep movsd`) naming that missile goes with it as the second block.
//  The first argument is the owner unit, not a GUID: it is stored into the block's pOwner (0x6FABD570), which SKILLS_SpawnMissileRing
//  dereferences for the owner's seed, and each of the four call sites loads it from a unit pointer.
//  Every caller passes -1 for nAltMissileId (`push 0FFFFFFFFh` at all four sites), so no stock call builds the second block, and the seeded mix
//  in SKILLS_SpawnMissileRing never runs in 1.10.
//  BUG: pUnit is read with no test (`mov eax, [esi]`, 0x6FABD4EA, as the inlined UNITS_GetXPosition reads it). UNREACHABLE: the one caller that
//  could send NULL, MONUNIQUE_MonUModCltElementalDeath (its `jz` at 0x6FB456F8 goes to this call), is run only by
//  MONUNIQUE_MonUModRunCltFuncsForUnit, which returns for a NULL unit before its loop (0x6FB461C7); the test at 0x6FB456ED is the inlined mode
//  accessor's own.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall SKILLS_SpawnMissileRingFromUnit(D2UnitStrc *pOwner, D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId, D2C_MissilesHcIdx nAltMissileId, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nVelocity)
{
    D2MissileStrc tParams;
    D2MissileStrc tAltParams;
    D2MissileStrc *pAltParams;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETOFFSET;

    tParams.nX = UNITS_GetXPosition(pUnit);

    tParams.nY = UNITS_GetYPosition(pUnit);

    tParams.nSkill = nSkillId;
    tParams.pOwner = pOwner;
    tParams.nMissile = nMissileId;
    tParams.nSkillLevel = nSkillLevel;
    if (nVelocity)
    {
        tParams.nVelocity = nVelocity;
        tParams.dwFlags |= MISSILESPAWN_VELOCITY;
    }

    //  `cmp eax, 0FFFFFFFFh / jnz` (0x6FABD594): -1 is "no second missile".
    if ((int32_t)nAltMissileId == -1)
    {
        pAltParams = NULL;
    }
    else
    {
        tAltParams = tParams;
        tAltParams.nMissile = nAltMissileId;
        pAltParams = &tAltParams;
    }

    SKILLS_SpawnMissileRing(&tParams, pAltParams);
}


//1.10f: D2Client.0x6FABD5D0
//  The ring: pParams is in ecx and pAltParams in edx. Sixty-four spawns through MISSILE_ClientSpawn, each with the block's nTargetX and nTargetY
//  set to the next pair of gnRingOffsetX and gnRingOffsetY (the points of a circle of radius 30, from (30, 0) round).
//  With a second block, each step rolls the owner's seed (pParams->pOwner's pSeed, `add ecx, 20h` at 0x6FABD5E9) and spawns the second block when
//  the roll's low dword is a multiple of 6 (`xor edx, edx / div ecx`, unsigned, 0x6FABD5FE..0x6FABD605), about one step in six. No stock call
//  passes one.
//  Without one, a diabfire ring (MISSILE_DIABFIRE) flips bit 4000h of the block's flags before every spawn (`xor dword ptr [esi], 4000h`,
//  0x6FABD665), so the bit alternates from one missile to the next; the block SKILLS_SpawnMissileRingFromUnit builds starts with it clear, so the
//  first missile and every other one after it carry it.
//  The original inlines SEED_RollRandomNumber here.
//  void: eax is left holding the last spawn, which no caller reads.
void __fastcall SKILLS_SpawnMissileRing(D2MissileStrc *pParams, D2MissileStrc *pAltParams)
{
    D2MissileStrc *pSpawnParams;
    int32_t iRingStep;

    //  Both loops are do-while in the bytes, the bound tested after each
    //  spawn (`cmp edi, 100h / jl`, 0x6FABD639 and 0x6FABD675, edi stepped by
    //  4 through the two tables).
    iRingStep = 0;
    if (pAltParams)
    {
        do
        {
            if ((uint32_t)SEED_RollRandomNumber(&pParams->pOwner->pSeed) % 6)
            {
                pParams->nTargetX = gnRingOffsetX[iRingStep];
                pParams->nTargetY = gnRingOffsetY[iRingStep];
                pSpawnParams = pParams;
            }
            else
            {
                pAltParams->nTargetX = gnRingOffsetX[iRingStep];
                pAltParams->nTargetY = gnRingOffsetY[iRingStep];
                pSpawnParams = pAltParams;
            }

            //  One call for both arms, the block in ecx (0x6FABD631).
            MISSILE_ClientSpawn(pSpawnParams);
            ++iRingStep;
        } while (iRingStep < 64);
    }
    else
    {
        do
        {
            pParams->nTargetX = gnRingOffsetX[iRingStep];
            pParams->nTargetY = gnRingOffsetY[iRingStep];
            if (pParams->nMissile == (int32_t)MISSILE_DIABFIRE)
                pParams->dwFlags ^= MISSILESPAWN_NOLIGHT;

            MISSILE_ClientSpawn(pParams);
            ++iRingStep;
        } while (iRingStep < 64);
    }

}


//1.10f: D2Client.0x6FABD690
//  Up to 64 missiles from one point, all round: nMissiles is in ecx and pParams in edx. The caller's block is spawned again and again with a new
//  nTargetX and nTargetY: first (14, -14); then, ring by ring, seven rings of four quadrants with two missiles each (the ring's two values,
//  gnMissileArrayRingA and gnMissileArrayRingB, times the quadrant's signs, gnMissileArrayQuadrantX and gnMissileArrayQuadrantY, once as (A, B) and
//  once swapped) and one missile on an axis, gnMissileArrayAxisX and gnMissileArrayAxisY. Every offset is between 19.7 and 20.4 long.
//  The count is asserted first: `(nMissiles - 1) / 5 <= MISSILEARRAY_TABLE_LENGTH`, compiled as `> 7` on a signed division (`imul` by 66666667h,
//  `sar edx, 1`, the sign bit added; 0x6FABD693..0x6FABD6AC), line 2120 of this file (`push 848h`); 41 or more ends the process. The axis tables
//  hold eight pairs and the loop reads seven: the eighth, (14, -14), is the first missile's, which the code writes as two immediates (0x6FABD6CE,
//  0x6FABD6D5).
//  void: eax is the first spawn's unit, or the last axis missile's, and no caller reads it.
void __fastcall SKILLS_SpawnMissileArray(int32_t nMissiles, D2MissileStrc *pParams)
{
    int32_t nRemaining;
    int32_t nRing;
    int32_t nQuadrant;

    nRemaining = nMissiles - 1;
    if (nRemaining / 5 > 7)
    {
        FOG_DisplayAssert("(nMissiles - 1) / 5 <= MISSILEARRAY_TABLE_LENGTH", szSkillsCpp, 2120);
        exit(-1);
    }

    pParams->nTargetX = 14;
    pParams->nTargetY = -14;
    MISSILE_ClientSpawn(pParams);

    //  The ring bound is tested after each ring (`cmp ebx, 1Ch / jl`,
    //  0x6FABD767), the quadrant bound after each quadrant (`cmp edi, 10h /
    //  jl`, 0x6FABD745); the count before each.
    for (nRing = 0; nRing < 7; ++nRing)
    {
        //  BUG: the walk ends only at a count of exactly 0, where the quadrant test is `<= 0`. The ring loop leaves on `test ebp, ebp / jz`
        //  (0x6FABD6E3), the quadrant loop on `test ebp, ebp / jle` (0x6FABD6ED). A quadrant spends two and the axis one, so the count is 0 at a
        //  ring's top only when nMissiles - 1 is a multiple of nine; any other count goes below 0 inside a ring, the quadrant loop stops, and every
        //  ring after it still lays its axis missile. Of the counts the assert lets through, only 1, 10, 19, 28 and 37 lay as many missiles as
        //  asked: 2 lays 10, 40 lays 44, 0 lays 8. LATENT: no stock caller asks for another count: the only stock missile of handler 46 has a
        //  count of 1 (floored), and handler 51's two missiles are named by no column of Skills.txt or Missiles.txt.
        if (nRemaining == 0)
            break;

        for (nQuadrant = 0; nQuadrant < 4; ++nQuadrant)
        {
            if (nRemaining <= 0)
                break;

            pParams->nTargetX = gnMissileArrayQuadrantX[nQuadrant] * gnMissileArrayRingA[nRing];
            pParams->nTargetY = gnMissileArrayQuadrantY[nQuadrant] * gnMissileArrayRingB[nRing];
            MISSILE_ClientSpawn(pParams);

            pParams->nTargetX = gnMissileArrayQuadrantX[nQuadrant] * gnMissileArrayRingB[nRing];
            pParams->nTargetY = gnMissileArrayQuadrantY[nQuadrant] * gnMissileArrayRingA[nRing];
            MISSILE_ClientSpawn(pParams);

            nRemaining -= 2;
        }

        pParams->nTargetX = gnMissileArrayAxisX[nRing];
        pParams->nTargetY = gnMissileArrayAxisY[nRing];
        MISSILE_ClientSpawn(pParams);
        --nRemaining;
    }

}


static_assert(offsetof(D2DataTablesStrc, pStatesTxt) == 0xBC && offsetof(D2DataTablesStrc, pActionStates) == 0x184
                  && offsetof(D2DataTablesStrc, nActionStates) == 0x188,
              "SKILLS_StatesRunActiveStateFuncs: `mov ecx, [ecx+0BCh]` (0x6FABD7EA), `mov ecx, [eax+184h]` (0x6FABD7B5), "
              "`cmp [eax+188h], di` (0x6FABD7A6)");
static_assert(sizeof(((D2DataTablesStrc *)0)->nActionStates) == 2,
              "a WORD, not the int D2MOO declares: `cmp [eax+188h], di` (0x6FABD7A6), `movsx edx, word ptr "
              "[eax+188h]` (0x6FABD87A); D2Common's `inc word ptr` (0x6FD4F3F9) -- TODO.md T104");
static_assert(offsetof(D2StatesTxt, wCltActiveFunc) == 0x34 && STAT_MODIFIERLIST_SKILL == 0x15E
                  && STAT_MODIFIERLIST_LEVEL == 0x15F,
              "SKILLS_StatesRunActiveStateFuncs: `mov ax, [ebp+34h]` (0x6FABD7FD), `push 15Eh` (0x6FABD837), `push 15Fh` "
              "(0x6FABD846)");


//1.10f: D2Client.0x6FABD780
//  Runs the client function of every active state on a unit: pUnit is in ecx. D2Common's list of the active states (the states.txt rows with the
//  STATEMASK_ACTIVE bit, collected into pActionStates and nActionStates) is walked, and each state the unit has, with an in-range
//  `cltactivefunc` whose gpSkillCltDoFuncs slot is not NULL, has that function called with the skill and the level of the state's own stat list:
//  MODIFIERLIST_SKILL and MODIFIERLIST_LEVEL, stats 350 and 351, or 0 and 1 with no list. The function's answer is not read: eax is next written
//  at 0x6FABD878 (`mov eax, [ecx]`), and nothing between reads it. Both callers, MONSTER_UpdateAnimMode and PLAYER_AdvanceAnimMode, have just
//  tested STATES_CheckStateMaskActiveOnUnit themselves.
//  nActionStates is a WORD in 1.10: the client compares it as one (`cmp [eax+188h], di`, 0x6FABD7A6; `movsx edx, word ptr [eax+188h]`,
//  0x6FABD87A), and D2Common's loader writes it as one.
//  The tables pointer and the count are read again after every pass (0x6FABD86D..0x6FABD87A), the list at the top of the next (0x6FABD7B5). The
//  state id is a signed word (`movsx`, 0x6FABD7BB); the column is a signed word too (`test ax, ax / jl`, then `movsx`), read once for the tests
//  (0x6FABD7FD) and again for the call (0x6FABD853), and its slot likewise (0x6FABD813, 0x6FABD85E).
//  The original inlines DATATBLS_GetStatesTxtRecord here.
void __fastcall SKILLS_StatesRunActiveStateFuncs(D2UnitStrc *pUnit)
{
    int32_t iState;
    int32_t nState;
    D2StatesTxt *pStatesTxt;
    int16_t nCltActiveFunc;
    D2StatListStrc *pStatList;
    int32_t nSkillId;
    int32_t nSkillLevel;

    if (!D2COMMON_STATES_CheckStateMaskActiveOnUnit(pUnit))
        return;

    //  The count as a signed word: `cmp [eax+188h], di / jle` before the first pass, `movsx edx, word ptr [eax+188h] / cmp edi, edx / jl` after
    //  each.
    for (iState = 0; iState < sgptDataTables->nActionStates; ++iState)
    {
        nState = sgptDataTables->pActionStates[iState];
        if (!D2COMMON_STATES_CheckState(pUnit, (D2C_States)nState))
            continue;

        if (nState < 0 || nState >= sgptDataTables->nStatesTxtRecordCount)
            continue;

        pStatesTxt = &sgptDataTables->pStatesTxt[nState];
        if (!pStatesTxt)
            continue;

        //  `jge` against the count: signed.
        nCltActiveFunc = (int16_t)pStatesTxt->wCltActiveFunc;
        if (nCltActiveFunc < 0 || nCltActiveFunc >= gnSkillCltDoFuncCount || !gpSkillCltDoFuncs[nCltActiveFunc])
        {
            continue;
        }

        nSkillId = 0;
        nSkillLevel = 1;
        pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, (D2C_States)nState);
        if (pStatList)
        {
            nSkillId = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_MODIFIERLIST_SKILL, 0);
            nSkillLevel = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_MODIFIERLIST_LEVEL, 0);
        }

        //  The column is read again, as the bytes read it (0x6FABD853); it was bounded above on the same field of a table nothing writes after load.
        gpSkillCltDoFuncs[(int16_t)pStatesTxt->wCltActiveFunc](pUnit, (D2C_Skills)nSkillId, nSkillLevel);
    }
}


static_assert(offsetof(D2MissileStrc, pInitFunc) == 0x54 && offsetof(D2MissileStrc, pInitArgs) == 0x58,
              "SKILLS_SpawnScatteredMissiles: `mov dword ptr [edi+54h], offset SKILLS_SetUnitDirection` (0x6FABDA18), "
              "`mov [edi+58h], eax` (0x6FABDA1F); MISSILE_ClientSpawn calls [ebx+54h] with [ebx+58h] (0x6FB3EA86)");


//1.10f: D2Client.0x6FABD8A0
//  nCount missiles round a point, each on a diagonal: pUnit is in ecx and pParams in edx; nJitter and nCount are on the stack. The block is the
//  caller's, its nX and nY the base point. One roll of the unit's seed picks the first corner (`and eax, 3`, 0x6FABD8C1); then, for each missile,
//  the corner's offset (0 is (-16, -16), 1 (-16, 16), 2 (16, 16), 3 (16, -16)), the corner advanced `(n + 1) % 4` (a signed remainder,
//  0x6FABD936..0x6FABD943), the spawn point rolled within nJitter of the base on each axis (two limited rolls of 2 * nJitter, less nJitter), the
//  target at the spawn point plus the corner's offset, and SKILLS_SetUnitDirection with 16 times the corner as the block's init callback and
//  argument, which MISSILE_ClientSpawn calls on the new missile (0x6FB3EA86..0x6FB3EA92). So a burst of N rolls the unit's seed 2N + 1 times,
//  with nJitter above 0.
//  void: the original leaves the counter in eax, and no caller reads it.
//  The original inlines SEED_RollRandomNumber and SEED_RollLimitedRandomNumber here.
void __fastcall SKILLS_SpawnScatteredMissiles(D2UnitStrc *pUnit, D2MissileStrc *pParams, int32_t nJitter, int32_t nCount)
{
    int32_t nDirection;
    int32_t nBaseX;
    int32_t nBaseY;
    int32_t nJitterRange;
    int32_t nRemaining;
    int32_t nFacingX;
    int32_t nFacingY;
    int32_t nSpawnY;

    nDirection = (int32_t)((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) & 3);
    nBaseX = pParams->nX;
    nBaseY = pParams->nY;
    if (nCount <= 0)
        return;

    nRemaining = nCount;
    nJitterRange = 2 * nJitter;
    do
    {
        switch (nDirection)
        {
        case 0:
            nFacingX = -16;
            nFacingY = -16;
            break;
        case 1:
            nFacingX = -16;
            nFacingY = 16;
            break;
        case 2:
            nFacingX = 16;
            nFacingY = 16;
            break;
        case 3:
            nFacingX = 16;
            nFacingY = -16;
            break;
        default:
            nFacingX = 0;
            nFacingY = 0;
            break;
        }

        nDirection = (nDirection + 1) % 4;

        pParams->nX = SEED_RollLimitedRandomNumber(&pUnit->pSeed, nJitterRange) - nJitter + nBaseX;

        nSpawnY = nBaseY + (SEED_RollLimitedRandomNumber(&pUnit->pSeed, nJitterRange) - nJitter);

        //  nX is read back (0x6FABD9F1), nY is the value just computed.
        pParams->nY = nSpawnY;
        pParams->nTargetY = nSpawnY + nFacingY;
        pParams->nTargetX = pParams->nX + nFacingX;

        //  BUG: the facing and the offset come from two different corners: the corner is advanced (0x6FABD936) before it is multiplied by 16
        //  (`mov eax, ebp / shl eax, 4`, 0x6FABDA0E..0x6FABDA15, ebp reloaded from the advanced value at 0x6FABD98F and 0x6FABD9E9), so the missile
        //  sent along corner d is given the facing 16 * ((d + 1) % 4). The four offsets point at directions 32, 15, 0 and 47, and the facings given
        //  are 16, 32, 48 and 0: no corner matches. NOT ESTABLISHED: it runs on every call, but whether a wrong facing shows depends on the missiles
        //  the callers put in the block, on how many directions each draws, and on whether anything re-aims a missile after MISSILE_ClientSpawn's
        //  init callback; none of the three was checked.
        pParams->pInitFunc = SKILLS_SetUnitDirection;
        pParams->pInitArgs = nDirection << 4;
        MISSILE_ClientSpawn(pParams);
    } while (--nRemaining);
}


//1.10f: D2Client.0x6FABDA50
//  The init callback: face the new missile. pUnit is in ecx and nDirection in edx: D2Common's PATH_SetDirection on the unit's dynamic path, which
//  takes the direction as a byte.
//  It is called through a D2MissileStrc's pInitFunc (MISSILE_ClientSpawn hands it the new missile and the block's pInitArgs, 0x6FB3EA86..
//  0x6FB3EA92); SKILLS_SpawnScatteredMissiles is the only function that stores it (0x6FABDA18).
//  The parameter is typed to the callback's contract, `void (__fastcall *)(D2UnitStrc *, int)`: the body pushes edx whole (0x6FABDA53) and
//  D2Common reads the byte, so the C narrows at the call.
void __fastcall SKILLS_SetUnitDirection(D2UnitStrc *pUnit, int32_t nDirection)
{
    D2COMMON_PATH_SetDirection(pUnit->pDynamicPath, (uint8_t)nDirection);
}


//1.10f: D2Client.0x6FABDA60
//  Swing at a unit in melee range: pUnit is in ecx and pTarget in edx; always 1. A seven-int client-action block, zeroed (`rep stosd`) and filled
//  as server packet 0x4C's handler fills its own: slot 0 the skill, SKILL_ATTACK; 1 -1; 2 and 3 the target's unit type and GUID, UNIT_TYPES_COUNT
//  and -1 with no target; 4 the level, 1; then UNITMODE_DispatchActionByUnitType with action 22, a skill on a unit, which
//  SKILLS_CltStartSkillOnUnit starts. The fourth argument of that dispatch is the literal 1, bInterruptKnockback.
//  The second argument is the target unit: the body reads `[edx]` and `[edx+0Ch]` (0x6FABDA89, 0x6FABDA98), a unit's dwUnitType and dwUnitId, and
//  each of the three callers passes the unit D2COMMON_UNITS_IsInMeleeRange has just accepted (0x6FAC221C, 0x6FAC2568, 0x6FAC98E0). Those callers
//  are SKILLS_CltStFunc30, 31 and 25, and each returns this function's answer, so the return stays int.
//  The client builds the no-target block, which none of its three callers reaches: each tests the unit before (0x6FAC2215, 0x6FAC2560,
//  0x6FAC98D8).
int32_t __fastcall SKILLS_DispatchUnitAction22(D2UnitStrc *pUnit, D2UnitStrc *pTarget)
{
    int32_t pActionParams[7];

    memset(pActionParams, 0, sizeof(pActionParams));
    pActionParams[0] = SKILL_ATTACK;
    pActionParams[1] = -1;
    pActionParams[2] = pTarget ? (int32_t)pTarget->dwUnitType : (int32_t)UNIT_TYPES_COUNT;
    pActionParams[3] = pTarget ? (int32_t)pTarget->dwUnitId : -1;
    pActionParams[4] = 1;
    UNITMODE_DispatchActionByUnitType(CLTACTION_SKILL_UNIT, pUnit, pActionParams, TRUE);
    return 1;
}


//1.10f: D2Client.0x6FABDAD0
//  Slows the current animation frame by a percentage: pUnit is in ecx and nPercent in edx. nSeqCurrentFramePrecise is the frame in 8.8 fixed
//  point: its whole part (`sar eax, 8`) times 100 - nPercent, divided by 100 (`imul` by 51EB851Fh, `sar edx, 5`, the sign bit added: a signed
//  division), shifted back (`shl edx, 8`); the fraction is dropped.
//  void: the original leaves the sign bit of the division in eax (`shr eax, 1Fh`, 0x6FABDAEE), and no caller reads it. The shift is written on an
//  unsigned value, so a negative quotient stays defined C; the bits are `shl`'s.
void __fastcall SKILLS_ReduceFixedFieldByPercent(D2UnitStrc *pUnit, int32_t nPercent)
{
    int32_t nScaled;

    nScaled = (pUnit->nSeqCurrentFramePrecise >> 8) * (100 - nPercent);
    pUnit->nSeqCurrentFramePrecise = (int32_t)((uint32_t)(nScaled / 100) << 8);
}


//1.10f: D2Client.0x6FABDB00
//  nCount client effects scattered round the aim point: pUnit is in ecx and nMissileId in edx; nCount, nRadius, nSkillId and nSkillLevel are on
//  the stack. Always 1. The aim point is the one SKILLS_GetPathTargetPos finds; the unit's low seed is set to its X (D2Common's
//  SEED_InitLowSeed), so every client draws the same scatter for the same aim; then for each effect a point within nRadius of the aim on each axis
//  (two limited rolls of 2 * nRadius, less nRadius), and MISSILE_ClientSpawnSkillEffect there when its squared distance from pUnit is 4 or more
//  (`cmp eax, 4 / jl`, 0x6FABDCCE).
//  Its callers pass a missile as pUnit (MISSILE_CltHitFunc41 and 47) or a unit (SKILLS_CltDoFunc5); the `int` return stays because the body
//  writes the 1 itself (0x6FABDD06). nCount's own argument slot holds the unit's Y from the first effect on (0x6FABDC8E).
//  The original inlines SKILLS_GetPathTargetPos, SEED_RollLimitedRandomNumber, UNITS_GetXPosition and UNITS_GetYPosition here.
int32_t __fastcall SKILLS_SpawnClientEffectScatter(D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId, int32_t nCount, int32_t nRadius, D2C_Skills nSkillId, int32_t nSkillLevel)
{
    int32_t nAimX;
    int32_t nAimY;
    int32_t nSpread;
    int32_t nRemaining;
    int32_t nScatterX;
    int32_t nScatterY;

    SKILLS_GetPathTargetPos(pUnit, &nAimX, &nAimY);

    D2COMMON_SEED_InitLowSeed(&pUnit->pSeed, nAimX);
    if (nCount <= 0)
        return 1;

    nRemaining = nCount;
    nSpread = 2 * nRadius;
    do
    {
        nScatterX = SEED_RollLimitedRandomNumber(&pUnit->pSeed, nSpread) - nRadius + nAimX;

        nScatterY = SEED_RollLimitedRandomNumber(&pUnit->pSeed, nSpread) - nRadius + nAimY;

        if (D2COMMON_PATH_ComputeSquaredDistance(UNITS_GetXPosition(pUnit), UNITS_GetYPosition(pUnit), nScatterX, nScatterY) >= 4)
        {
            MISSILE_ClientSpawnSkillEffect(pUnit, (int32_t)nMissileId, 0, 0, nScatterX, nScatterY, (int32_t)nSkillId, nSkillLevel, 0);
        }
    } while (--nRemaining);

    return 1;
}


static_assert(offsetof(D2MissilesTxt, nSize) == 0x18A && COLLIDE_WALL == 1,
              "SKILLS_CanMissileLandAtAimPoint: `mov cl, [edx+18Ah]` (0x6FABE1BE), `push 5` (0x6FABE1BC) -- "
              "COLLIDE_WALL | COLLIDE_MISSILE_BARRIER");
static_assert((STATLIST_NEWLENGTH | STATLIST_TEMPONLY) == 6 && STATLIST_TEMPONLY == 4 && STAT_ATTACKRATE == 0x44,
              "`push 6` (0x6FABDD3B); `push 4` (0x6FABE21E) and `push 44h` (0x6FABE239)");


//1.10f: D2Client.0x6FABDD20
//  A stat list whose expiry ends the skill: pUnit is in ecx. D2Common's STATLIST_AllocStatList with STATLIST_NEWLENGTH | STATLIST_TEMPONLY and 50
//  as its expire frame, the unit's type and GUID (UNIT_TYPES_COUNT and -1 with no unit), then SKILLS_StatRemoveCallback_EndMoveSkill (below) as the
//  list's remove callback, and the list posted to the unit (D2Common #10475, reset flag 1).
//  Its callers are the client starts of Charge and SerpentCharge (SKILLS_CltStFunc25), Leap (29), Leap Attack (30) and Whirlwind (31).
void __fastcall SKILLS_PostSkillStatWithRemoveCallback(D2UnitStrc *pUnit)
{
    D2StatListStrc *pStatList;

    pStatList = D2COMMON_STATLIST_AllocStatList(NULL, STATLIST_NEWLENGTH | STATLIST_TEMPONLY, 50, pUnit ? pUnit->dwUnitType : UNIT_TYPES_COUNT, pUnit ? pUnit->dwUnitId : (D2UnitGUID)-1);
    if (!pStatList)
        return;

    //  The callback D2Common will call is the SKILLS_StatRemoveCallback_EndMoveSkill written below.
    D2COMMON_STATLIST_SetStatRemoveCallback(pStatList, SKILLS_StatRemoveCallback_EndMoveSkill);

    //  BUG: the no-unit arm ends in a fault: the NULL test (0x6FABDD24) only picks the owner key, and pUnit then goes to #10475 untested
    //  (0x6FABDD58), which reads pUnit->pStatListEx first. UNREACHABLE: the four callers are `cltstfunc` handlers, which only SKILLS_RunCltStFunc
    //  calls, and only after it has read pUnit->pDynamicPath (0x6FABC01B).
    D2COMMON_10475_PostStatToStatList(pUnit, pStatList, TRUE);
}


//1.10f: D2Client.0x6FABDD70
//  The remove callback: end the movement skill. It is D2Common's StatListRemoveCallback: pUnit is in ecx, nState in edx and pStatList on the stack
//  (`retn 4`); it reads neither of the last two. With a used skill, the unit not in STATE_SKILL_MOVE and bit 1 of the skill's flags (`test al, 2`,
//  0x6FABDD96) clear, the skill's `srvdofunc` picks the end:
//
//    67  Charge, SerpentCharge  SKILLS_EndUninterruptableSkill
//    76  Whirlwind              SKILLS_StopWhirlwindSound
//    77  Leap                   SKILLS_RestoreCollisionFootprint
//    78  Leap Attack            SKILLS_RestoreCollisionFootprint
//
//  any other value does nothing: a jump table indexed by a byte table over 67..78 (0x6FABDDD8..0x6FABDDE8).
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
void __fastcall SKILLS_StatRemoveCallback_EndMoveSkill(D2UnitStrc *pUnit, int32_t nState, D2StatListStrc *pStatList)
{
    D2SkillStrc *pUsedSkill;
    int32_t nSkillId;
    D2SkillsTxt *pSkillsTxt;

    pUsedSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pUsedSkill)
        return;

    if (D2COMMON_STATES_CheckState(pUnit, STATE_SKILL_MOVE))
        return;

    if (D2COMMON_SKILLS_GetFlags(pUsedSkill) & 2)
        return;

    nSkillId = D2COMMON_SKILLS_GetSkillIdFromSkill(pUsedSkill, szSkillsCpp, 2364);

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return;

    //  `movsx` of the column, then `add eax, -67 / cmp eax, 0Bh / ja`: a
    //  signed word, 67..78 through the tables.
    switch ((int16_t)pSkillsTxt->wSrvDoFunc)
    {
    case 67:
        SKILLS_EndUninterruptableSkill(pUnit, (D2C_Skills)nSkillId);
        break;
    case 76:
        SKILLS_StopWhirlwindSound(pUnit, (D2C_Skills)nSkillId);
        break;
    case 77:
    case 78:
        SKILLS_RestoreCollisionFootprint(pUnit);
        break;
    default:
        break;
    }
}


//1.10f: D2Client.0x6FABDE30
//  Adds levels to a page of the unit's skills: pUnit is in ecx and nDelta in edx; nElement and nSkillPage are on the stack. Server packet 0x93's
//  handler, SCMD_HandleSrv93_SkillPageLevels, is its one caller, with a signed delta, the element and the page from the packet. Three asserts first
//  ("hUnit" at 2409, "nDelta != 0" at 2410, "hSkillInfo" (the unit's skill list) at 2412), then every skill of the list that has a level above 0, is
//  enhanceable, has the element when one is given (Skills.txt `EType`), is on the page (its skilldesc.txt row's page is nSkillPage + 1, or nSkillPage
//  is 4, every page) and is the unit's own (owner GUID -1) gets nDelta added through D2Common #11031, which adds it to the skill's level bonus
//  (SKILLS_AddSkillLevelBonus); one left at level 0 is removed. Then the passives are refreshed. BUG: the walk stops advancing at six of its exits,
//  and loops forever. The next skill is taken at the top (0x6FABDEBE) and moved into ebx only at 0x6FABDFCB; the id's two bounds, the skills.txt
//  row's NULL test, the skilldesc id's two bounds and its row's NULL test all jump to 0x6FABDFCF, past that move, where `test ebx, ebx / jnz` goes
//  round again with the same skill (0x6FABDEDB, 0x6FABDEEE, 0x6FABDF07, 0x6FABDF16, 0x6FABDF22, 0x6FABDF38). So one skill in the list whose id is
//  outside skills.txt or whose `skilldesc` is empty or out of range hangs the client. The C keeps the shape with `continue` in a do-while, which goes
//  to the loop's test with the skill unchanged. LATENT: SCMD_HandleSrv93_SkillPageLevels looks the unit up as a player only, and a player's list
//  holds its class's skills and the skills of its items, all of which name a skilldesc row in the stock data. The original inlines
//  DATATBLS_GetSkillsTxtRecordById here, and the skilldesc.txt record lookup is written in place.
void __fastcall SKILLS_AddLevelsToSkillPage(D2UnitStrc *pUnit, int32_t nDelta, int32_t nElement, uint8_t nSkillPage)
{
    D2SkillStrc *pSkill;
    D2SkillStrc *pNextSkill;
    int32_t nSkillId;
    D2SkillsTxt *pSkillsTxt;
    int32_t nSkillDesc;
    D2SkillDescTxt *pSkillDesc;
    D2SkillStrc *pOwnSkill;

    if (!pUnit)
    {
        FOG_DisplayAssert("hUnit", szSkillsCpp, 2409);
        exit(-1);
    }

    if (!nDelta)
    {
        FOG_DisplayAssert("nDelta != 0", szSkillsCpp, 2410);
        exit(-1);
    }

    if (!pUnit->pSkills)
    {
        FOG_DisplayAssert("hSkillInfo", szSkillsCpp, 2412);
        exit(-1);
    }

    pSkill = D2COMMON_SKILLS_GetFirstSkillFromSkillList(pUnit->pSkills);
    if (pSkill)
    {
        do
        {
            pNextSkill = D2COMMON_SKILLS_GetNextSkill(pSkill);
            nSkillId = D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillsCpp, 2419);

            pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
            if (!pSkillsTxt)
                continue;   //  BUG: pSkill is not advanced (see the head)

            nSkillDesc = (int16_t)pSkillsTxt->wSkillDesc;
            if (nSkillDesc < 0 || nSkillDesc >= sgptDataTables->nSkillDescTxtRecordCount)
                continue;   //  BUG: pSkill is not advanced

            pSkillDesc = &sgptDataTables->pSkillDescTxt[nSkillDesc];
            if (!pSkillDesc)
                continue;   //  BUG: pSkill is not advanced

            //  The level `jle` (signed); the element a byte zero-extended
            //  against the whole int; the page `cmp al, 4`, then the row's page
            //  sign-extended (`movsx ecx, byte ptr [edi+2]`) against the byte
            //  plus 1.
            if (D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill, TRUE) > 0 && D2COMMON_SKILLS_IsEnhanceable((D2C_Skills)nSkillId) && (!nElement || (int32_t)pSkillsTxt->nEType == nElement)
                && (nSkillPage == 4 || (int32_t)(int8_t)pSkillDesc->nSkillPage == (int32_t)nSkillPage + 1) && D2COMMON_SKILLS_GetOwnerGUIDFromSkill(pSkill) == (D2UnitGUID)-1)
            {
                D2COMMON_SKILLS_AddSkillLevelBonus(pUnit, nSkillId, nDelta);
                pOwnSkill = D2COMMON_SKILLS_GetSkillById(pUnit, (D2C_Skills)nSkillId, (D2UnitGUID)-1);
                if (pOwnSkill && !D2COMMON_SKILLS_GetSkillLevel(pUnit, pOwnSkill, TRUE))
                    D2COMMON_SKILLS_RemoveSkill(pUnit, (D2C_Skills)nSkillId, szSkillsCpp, 2464);
            }

            pSkill = pNextSkill;
        } while (pSkill);
    }

    D2COMMON_SKILLS_RefreshPassiveSkills(pUnit);
}


//1.10f: D2Client.0x6FABDFF0
//  Can the skill's missile land at the aim point: pUnit is in ecx and nMissileId in edx. The missiles.txt row is asserted ("ptStats" at 2494)
//  after the unit's room is fetched, and with no room the answer is FALSE. The point is the one SKILLS_GetPathTargetPos finds; with both
//  coordinates set, TRUE when D2Common's COLLISION_CheckMaskWithSizeXY finds none of COLLIDE_WALL and COLLIDE_MISSILE_BARRIER in a square of the
//  row's `size` there; its WORD answer is tested (`neg ax / sbb eax, eax / inc eax`, 0x6FABE1D2).
//  Its callers are SKILLS_CltDoFunc26 and 28 (Skills.txt `cltdofunc` 26 and 28: Fire Wall, and Meteor, Blizzard and Eruption, with their monster
//  copies); both read the answer.
//  The original inlines SKILLS_GetPathTargetPos here, and the missiles.txt record lookup is written in place.
BOOL __fastcall SKILLS_CanMissileLandAtAimPoint(D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId)
{
    D2MissilesTxt *ptStats;
    D2ActiveRoomStrc *pRoom;
    int32_t nPointX;
    int32_t nPointY;

    if ((int32_t)nMissileId < 0 || (int32_t)nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        ptStats = NULL;
    else
        ptStats = &sgptDataTables->pMissilesTxt[nMissileId];

    pRoom = D2COMMON_UNITS_GetRoom(pUnit);
    if (!ptStats)
    {
        FOG_DisplayAssert("ptStats", szSkillsCpp, 2494);
        exit(-1);
    }

    if (!pRoom)
        return FALSE;

    SKILLS_GetPathTargetPos(pUnit, &nPointX, &nPointY);
    if (!nPointX || !nPointY)
        return FALSE;

    return D2COMMON_COLLISION_CheckMaskWithSizeXY(pRoom, nPointX, nPointY, ptStats->nSize, ptStats->nSize, COLLIDE_WALL | COLLIDE_MISSILE_BARRIER) == 0;
}


//1.10f: D2Client.0x6FABE1F0
//  The melee reach and 4 more: pUnit is in ecx; D2Common's UNITS_GetMeleeRange plus 4. Its callers are SKILLS_CltStFunc14 and 53 and
//  SKILLS_CltDoFunc21, and all three read the answer.
int32_t __fastcall SKILLS_GetExtendedMeleeRange(D2UnitStrc *pUnit)
{
    return D2COMMON_UNITS_GetMeleeRange(pUnit) + 4;
}


//1.10f: D2Client.0x6FABE200
//  A temporary attack rate: pUnit is in ecx and nValue in edx. A stat list with STATLIST_TEMPONLY and no expire frame, keyed on the unit
//  (UNIT_TYPES_COUNT and -1 with none), posted to the unit, given STAT_ATTACKRATE at nValue, and the unit's animation rate and velocity recomputed
//  (D2Common's UNITS_UpdateAnimRateAndVelocity, line 2532). Its callers are SKILLS_CltStFunc9 (twice) and 27.
void __fastcall SKILLS_ApplyAttackRateStat(D2UnitStrc *pUnit, int32_t nValue)
{
    D2StatListStrc *pStatList;

    pStatList = D2COMMON_STATLIST_AllocStatList(NULL, STATLIST_TEMPONLY, 0, pUnit ? pUnit->dwUnitType : UNIT_TYPES_COUNT, pUnit ? pUnit->dwUnitId : (D2UnitGUID)-1);
    if (!pStatList)
        return;

    //  BUG: the no-unit arm ends in a fault, as in SKILLS_PostSkillStatWithRemoveCallback: pUnit goes to #10475 untested (0x6FABE231), which reads it
    //  before UNITS_UpdateAnimRateAndVelocity would trace a NULL unit. UNREACHABLE: the callers are `cltstfunc` 9 and 27.
    D2COMMON_10475_PostStatToStatList(pUnit, pStatList, TRUE);
    D2COMMON_STATLIST_SetStatIfListIsValid(pStatList, STAT_ATTACKRATE, nValue, 0);
    D2COMMON_UNITS_UpdateAnimRateAndVelocity(pUnit, szSkillsCpp, 2532);
}


//1.10f: D2Client.0x6FABE260
//  Registers a state's client event handler on a unit: pUnit is in ecx and nCltEvent in edx; nSkillId, nSkillLevel, nCltEventFunc, nKind and
//  nParam are on the stack. nCltEventFunc picks a slot of gpStateCltEventFuncs (two dwords, the States.txt `clteventfunc` table); in range and not
//  NULL, the handler is added to the unit's event list by CUNITEVENT_AddEvent with every other argument as it came, and its answer is this
//  function's; else NULL. The bound is two signed compares (`test eax, eax / jl`, `cmp eax, 2 / jge`, 0x6FABE264..0x6FABE26B).
//  Its one caller is PLRSKILLS_StatesRunStateSetFunc (0x6FB4D9F2), which tail-returns the answer from a function of its own typed void, so nothing
//  reads it; the return stays, because the body writes the NULL itself (0x6FABE297).
D2UnitEventStrc *__fastcall SKILLS_StatesAddCltEventFunc(D2UnitStrc *pUnit, int32_t nCltEvent, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nCltEventFunc, int32_t nKind, int32_t nParam)
{
    void *pfnHandler;

    if (nCltEventFunc < 0 || nCltEventFunc >= 2)
        return NULL;

    pfnHandler = (void *)gpStateCltEventFuncs[nCltEventFunc];
    if (!pfnHandler)
        return NULL;

    return CUNITEVENT_AddEvent(pUnit, nCltEvent, (int32_t)nSkillId, nSkillLevel, pfnHandler, nKind, nParam);
}

#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "Player.h"

//  Player.cpp -- a player unit on the client: its light and its stat callback, its construction and its teardown, a frame of its animation
//  mode, the client action it takes, the click that becomes one, and the command it queues.
//  The original source file is UNIT\Player.cpp (the asserts in this unit name it). Address range 0x6FB48570..0x6FB4B7BA.
//  PLAYER_SetQueuedCommand and PLAYER_GetQueuedCommand are written in Shared.cpp; the original inlines them here and in UnitMode.cpp.

//  The path literal this unit passes as __FILE__, one object in the original, 0x6FB93E10.
static const char szPlayerCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UNIT\\Player.cpp";

//  Layout checks for the members and numbers this file reads.
static_assert(offsetof(D2MonStats2Txt, nLight) == 0x11E,
              "PLAYER_UpdateLightRadius: `mov al, [esi+11Eh]` (0x6FB485FF)");
static_assert(offsetof(D2DataTablesStrc, nStuff) == 0xC6C && offsetof(D2DataTablesStrc, nShiftedStuff) == 0xC70,
              "PLAYER_OnStatValueChanged: `mov ecx, [edx+0C6Ch]` (0x6FB487A3), `mov ebx, [edx+0C70h]` (0x6FB487A9)");
static_assert(STAT_ITEM_LIGHTRADIUS == 89 && STAT_ITEM_LIGHTCOLOR == 90 && STAT_ITEM_NONCLASSSKILL == 97
                  && STAT_ITEM_SINGLESKILL == 107,
              "PLAYER_OnStatValueChanged: the switch from 59h (`lea ecx, [ebp-59h]`, 0x6FB4864B), cases 89, 90, 97 "
              "and 107; `cmp ebp, 61h` (0x6FB48753)");
static_assert(STAT_OTHER_ANIMRATE == 69 && STATE_DOPPLEZON == 63 && COLLIDE_CORPSE == 0x8000,
              "PLAYER_InitClientUnit: `push 45h` (0x6FB48910), `push 3Fh` (0x6FB48A2E), `push 8000h` (0x6FB4895C)");
static_assert((UNITFLAG_TARGETABLE | UNITFLAG_CANBEATTACKED | UNITFLAG_ISVALIDTARGET) == 0xE
                  && UNITFLAG_DRAWSHADOW == 0x20,
              "PLAYER_InitClientUnit: `and edx, 0FFFFFFDFh / or edx, 0Eh` (0x6FB48934, 0x6FB48937)");
static_assert(UNITFLAG_SQGFXCHANGE == 1u << 14 && CLTACTION_GETHIT == 6,
              "PLAYER_AdvanceAnimMode: `shr edx, 0Eh / and edx, 1` (0x6FB48D2E), `mov ecx, 6` (0x6FB48E04)");
static_assert(CLTACTION_DEATH == 8 && CLTACTION_DEAD == 9 && CLTACTION_BLOCK == 18 && CLTACTION_HIT == 19
                  && CLTACTION_KNOCKBACK == 20,
              "PLAYER_DoClientAction: the switch's index bytes at 0x6FB49798 send 8, 9, 18, 19 and 20 to slots 5..9 "
              "(0x6FB4952E, 0x6FB49634, 0x6FB494DC, 0x6FB49445, 0x6FB49488); `cmp edi, 13h` (0x6FB490B7)");
static_assert(STATE_PLAYERBODY == 7 && SKILLSFLAGINDEX_ATTACKNOMANA == 28,
              "PLAYER_DoClientAction: `push 7` (0x6FB495B7, 0x6FB4965D); PLAYER_CanUseSkill: skills.txt byte "
              "4 + (28 >> 3) (`mov al, [eax+7]`, 0x6FB49823) against gdwBitMasks[28 & 7] (`test [ecx+10h], al`)");
static_assert(SKILLUSTATE_SHAPERESTRICT == 4 && SKILLUSTATE_PASSIVE == 5,
              "PLAYER_CanUseSkill: `cmp esi, 4` (0x6FB49818), `cmp esi, 5` (0x6FB49841)");
static_assert(offsetof(D2ClientClickStrc, dwClickFlags) == 0x00 && offsetof(D2ClientClickStrc, pUnit) == 0x04
                  && offsetof(D2ClientClickStrc, pTarget) == 0x08 && offsetof(D2ClientClickStrc, nX) == 0x0C
                  && offsetof(D2ClientClickStrc, nY) == 0x10 && offsetof(D2ClientClickStrc, nMoveToPointCmd) == 0x14
                  && offsetof(D2ClientClickStrc, nMoveToUnitCmd) == 0x18 && offsetof(D2ClientClickStrc, pSkill) == 0x1C,
              "PLAYER_DispatchClickAction: `test byte ptr [esi], 2` (0x6FB49E0D), `mov ecx, [esi+4]` (0x6FB49DFC), "
              "`mov [esi+8], eax` (0x6FB49F16), `lea ebx, [esi+0Ch]` (0x6FB49F00), `lea edi, [esi+10h]` (0x6FB49EFC), "
              "`mov dword ptr [esi+14h], 3` (0x6FB49E72), `mov dword ptr [esi+18h], 4` (0x6FB49E79), "
              "`mov eax, [esi+1Ch]` (0x6FB49DF9)");
static_assert(offsetof(D2SkillsTxt, nRange) == 0x14,
              "PLAYER_OnMouseClick: `movsx eax, byte ptr [eax+14h]` (0x6FB49CEC)");
static_assert(STATE_JUST_PORTALED == 102 && OBJECT_TOWN_PORTAL == 59,
              "PLAYER_DispatchClickAction: `push 66h` (0x6FB4A25C), `cmp eax, 3Bh` (0x6FB4A257)");
static_assert(SKILLSFLAGINDEX_TARGETITEM == 27 && MONSTATSFLAGINDEX_INTERACT == 9,
              "PLAYER_DispatchClickAction: skills.txt byte 4 + (27 >> 3) against gdwBitMasks[27 & 7] (`mov cl, [ecx+7]` / "
              "`test [edx+0Ch], cl`, 0x6FB49FB0); monstats.txt byte 0Ch + (9 >> 3) against gdwBitMasks[9 & 7] "
              "(`mov al, [eax+0Dh]` / `test [ecx+4], al`, 0x6FB4A52D)");
static_assert(offsetof(D2GSPacketSrv74, bIsCorpse) == 1 && offsetof(D2GSPacketSrv74, nOwnerGUID) == 2
                  && offsetof(D2GSPacketSrv74, nBodyGUID) == 6,
              "PLAYER_AssignCorpse: `mov al, [esi+1]` (0x6FB4B3C9), `mov ecx, [esi+2]` (0x6FB4B3D4), "
              "`mov ecx, [esi+6]` (0x6FB4B409)");
static_assert(COMPOSIT_LEFTHAND == 6,
              "PLAYER_AssignCorpse: `mov byte ptr [esp+20h+var_C], 6` (0x6FB4B568)");


//1.10f: D2Client.0x6FB48570
//  Recomputes a unit's light radius and hands it to its light (a tail jump into DLIGHTMAP_SetRadius). A player or a monster with a light gets
//  its base radius plus nRadiusDelta, floored at 0 (signed); a NULL unit, any other type or a unit with no light returns. The base radius is 13
//  for a player and, for a monster, the larger of MONSTERS_GetMaximalLightRadius (a byte) and its monstats2 row's Light (a byte), compared signed
//  once zero-extended; the delta is added after the maximum. A monster whose class or monstats2 row is out of range keeps its radius.
//  The light is the client arm of D2UnitStrc's union at +64h, pLight. The original inlines DATATBLS_GetMonStats2TxtRecord here.
void __fastcall PLAYER_UpdateLightRadius(D2UnitStrc *pUnit, int32_t nRadiusDelta)
{
    D2ClientLightStrc *pLight;
    D2MonStats2Txt *pMonStats2;
    int32_t nRadius;

    if (!pUnit)
        return;

    if (pUnit->dwUnitType != UNIT_PLAYER && pUnit->dwUnitType != UNIT_MONSTER)
        return;

    pLight = pUnit->pLight;
    if (!pLight)
        return;

    if (pUnit->dwUnitType == UNIT_PLAYER)
    {
        nRadius = 13;
    }
    else
    {
        pMonStats2 = DATATBLS_GetMonStats2TxtRecord(pUnit->dwClassId);
        if (!pMonStats2)
            return;

        nRadius = D2COMMON_MONSTERS_GetMaximalLightRadius(pUnit);
        if (nRadius < pMonStats2->nLight)
            nRadius = pMonStats2->nLight;
    }

    nRadius += nRadiusDelta;
    if (nRadius <= 0)
        nRadius = 0;

    DLIGHTMAP_SetRadius(pLight, nRadius);
}

//1.10f: D2Client.0x6FB48630
//  The player's stat callback. D2Common calls it on a change to a stat list PLAYER_InitClientUnit allocated; nothing in D2Client calls it.
//  __fastcall with pGame in ecx, pOwner in edx and four stack arguments (pOther, nLayer_StatId, nPreviousValue, nNewValue). pGame and
//  nPreviousValue are never read. The stat is the high word of nLayer_StatId, taken unsigned; five stats have an arm, every other returns:
//
//    STAT_ITEM_LIGHTRADIUS  PLAYER_UpdateLightRadius(pOwner, nNewValue), for any unit type -- the callee tests it.
//    STAT_ITEM_LIGHTCOLOR   a player or a monster with a light: the colour from nNewValue, red its bits 16..23, green 8..15 and blue 0..7, or
//                           white (FFh each) when it is 0. DLIGHTMAP_SetColor takes three bytes, so the C passes the bytes.
//    STAT_ITEM_NONCLASSSKILL and STAT_ITEM_SINGLESKILL
//                           a player only: the skill is the layer (low word). With nNewValue 0, a skill the unit has whose base level
//                           (bBonus 0) is 0 or less is removed; with any other value, a skill it lacks is assigned at level 0 -- for
//                           NONCLASSSKILL always, for SINGLESKILL only when skills.txt's charclass is the unit's class (signed compare).
//    STAT_ITEM_CHARGED_SKILL
//                           a player only, with pOther (the item carrying the charges): the skill is the layer shifted right by
//                           sgptDataTables->nStuff and the level the layer masked by nShiftedStuff; the charges are pOther's own value of
//                           the stat on that layer, as a byte. D2Common #10954 is told the skill has those charges when they are more
//                           than 0 and pOther's stat list belongs to pOwner, and to free it (0 charges, bFreeMemory 1) otherwise -- both
//                           with pOther's dwUnitId.
//
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
void __fastcall PLAYER_OnStatValueChanged(struct D2GameStrc *pGame, D2UnitStrc *pOwner, D2UnitStrc *pOther, int32_t nLayer_StatId, int32_t nPreviousValue, int32_t nNewValue)
{
    D2ClientLightStrc *pLight;
    D2SkillStrc *pSkill;
    D2SkillsTxt *pSkillsTxt;
    uint32_t nStatId;
    int32_t nLayer;
    int32_t nChargedSkillId;
    int32_t nSkillLevel;
    int32_t nCharges;
    uint8_t nRed;
    uint8_t nGreen;
    uint8_t nBlue;
    D2UnitGUID nOtherGUID;

    if (!pOwner)
        return;

    nStatId = (uint32_t)nLayer_StatId >> 16;
    switch (nStatId)
    {
    case STAT_ITEM_LIGHTRADIUS:
        PLAYER_UpdateLightRadius(pOwner, nNewValue);
        return;

    case STAT_ITEM_LIGHTCOLOR:
        if (pOwner->dwUnitType != UNIT_PLAYER && pOwner->dwUnitType != UNIT_MONSTER)
            return;

        pLight = pOwner->pLight;
        if (!pLight)
            return;

        nRed = 0xFF;
        nGreen = 0xFF;
        nBlue = 0xFF;
        if (nNewValue)
        {
            nRed = (uint8_t)(nNewValue >> 16);
            nGreen = (uint8_t)(nNewValue >> 8);
            nBlue = (uint8_t)nNewValue;
        }
        DLIGHTMAP_SetColor(pLight, nRed, nGreen, nBlue);
        return;

    case STAT_ITEM_NONCLASSSKILL:
    case STAT_ITEM_SINGLESKILL:
        if (pOwner->dwUnitType != UNIT_PLAYER)
            return;

        nLayer = nLayer_StatId & 0xFFFF;
        pSkill = D2COMMON_SKILLS_GetSkillById(pOwner, (D2C_Skills)nLayer, (D2UnitGUID)-1);
        if (!nNewValue)
        {
            if (pSkill && D2COMMON_SKILLS_GetSkillLevel(pOwner, pSkill, FALSE) <= 0)
                D2COMMON_SKILLS_RemoveSkill(pOwner, (D2C_Skills)nLayer, szPlayerCpp, 221);
            return;
        }

        if (pSkill)
            return;

        pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nLayer);
        if (nStatId == STAT_ITEM_NONCLASSSKILL || (pSkillsTxt && pSkillsTxt->nCharClass == pOwner->dwClassId))
            D2COMMON_SKILLS_AssignSkill(pOwner, (D2C_Skills)nLayer, 0, FALSE, szPlayerCpp, 232);
        return;

    case STAT_ITEM_CHARGED_SKILL:
        if (pOwner->dwUnitType != UNIT_PLAYER)
            return;

        nLayer = nLayer_StatId & 0xFFFF;
        nChargedSkillId = nLayer >> sgptDataTables->nStuff;
        nSkillLevel = sgptDataTables->nShiftedStuff & nLayer;
        if (!pOther)
            return;

        nOtherGUID = pOther->dwUnitId;
        nCharges = (uint8_t)D2COMMON_STATLIST_UnitGetStatValue(pOther, (D2C_ItemStats)nStatId, (uint16_t)nLayer);
        if (nCharges > 0 && D2COMMON_STATLIST_GetOwner(pOther, NULL) == pOwner)
            D2COMMON_10954(pOwner, nOtherGUID, nChargedSkillId, nSkillLevel, nCharges, FALSE);
        else
            D2COMMON_10954(pOwner, nOtherGUID, nChargedSkillId, nSkillLevel, 0, TRUE);
        return;

    default:
        return;
    }
}

//1.10f: D2Client.0x6FB488A0
//  Builds a player unit on the client: pPlayer in ecx, pRoom in edx, nX and nY on the stack. Its one caller is CUNIT_InitUnit's UNIT_PLAYER arm.
//  In order:
//
//    * its slot in gapClientPlayers (MONSTER_RegisterPlayerSlot), a dynamic path at (nX, nY) in pRoom, an inventory only when it has none, a
//      skill list, and a stat list whose callback is PLAYER_OnStatValueChanged;
//    * STAT_ATTACKRATE, STAT_VELOCITYPERCENT and STAT_OTHER_ANIMRATE at 100 each;
//    * gbLastRoomInTown from DUNGEON_IsRoomInTown(pRoom), stored as the call answers it -- PLAYER_AdvanceAnimMode, which compares against it,
//      stores it normalised to 0 or 1;
//    * the mode PLRMODE_TOWNNEUTRAL, and in dwFlags UNITFLAG_DRAWSHADOW cleared and TARGETABLE, CANBEATTACKED and ISVALIDTARGET set; a unit
//      whose mode is then DEAD or DEATH loses its collision footprint and gets pattern COLLISION_PATTERN_SMALL_NO_PRESENCE with the footprint
//      mask COLLIDE_CORPSE;
//    * a light, radius 13, white at full intensity, type 1 for any player but the local one and 0 for the local player or while there is
//      none, stored in pLight, and the unit's start light set to the light's radius;
//    * its player data (UNITS_AllocPlayerData);
//    * a random facing, seed & 3Fh, for every player but the local one;
//    * the decoy rebuild, below.
//
//  The light is always added: the test before it asks D2Common #10083, whose both arms end in #10018, which answers 0 for any room. The call
//  stays, as the original makes it.
//
//  The decoy rebuild is this function's tail and PLAYER_FreeClientUnit's: with a local player in a room, every monster in the rooms adjacent to
//  the local player's that carries STATE_DOPPLEZON (the Amazon's Decoy, drawn in its caster's appearance) has its animation rebuilt
//  (GFX_RebuildUnitAnimForMode, 0). The original writes the block out in place in both functions. The room walk's count is compared unsigned.
//  The compiler keeps the list and its count in the argument slots nX and nY, both dead by then; the C gives them locals of their own.
//  The original inlines SEED_RollRandomNumber here.
void __fastcall PLAYER_InitClientUnit(D2UnitStrc *pPlayer, D2ActiveRoomStrc *pRoom, int32_t nX, int32_t nY)
{
    D2UnitStrc *pLocalPlayer;
    D2ClientLightStrc *pLight;
    D2ActiveRoomStrc *pPlayerRoom;
    D2ActiveRoomStrc **ppRoomList;
    D2UnitStrc *pRoomUnit;
    int32_t nNumRooms;
    uint32_t nRoom;

    MONSTER_RegisterPlayerSlot(pPlayer);
    D2COMMON_PATH_AllocDynamicPath(NULL, pRoom, nX, nY, pPlayer, TRUE);
    if (!pPlayer->pInventory)
        pPlayer->pInventory = D2COMMON_INVENTORY_AllocInventory(NULL, pPlayer);
    pPlayer->pSkills = D2COMMON_SKILLS_AllocSkillList(NULL);
    D2COMMON_SKILLS_InitSkillList(pPlayer);
    D2COMMON_STATLIST_AllocStatListEx(pPlayer, 0, PLAYER_OnStatValueChanged, NULL);
    D2COMMON_STATLIST_SetUnitStat(pPlayer, STAT_ATTACKRATE, 100, 0);
    D2COMMON_STATLIST_SetUnitStat(pPlayer, STAT_VELOCITYPERCENT, 100, 0);
    D2COMMON_STATLIST_SetUnitStat(pPlayer, STAT_OTHER_ANIMRATE, 100, 0);
    //  BUG: gbLastRoomInTown is written for every player the client builds, not only the local one, so PLAYER_AdvanceAnimMode can compare
    //  another player's flag. REACHABLE: the flag is then another player's when PLAYER_AdvanceAnimMode reads it.
    gbLastRoomInTown = D2COMMON_DUNGEON_IsRoomInTown(pRoom);
    D2COMMON_UNITS_ChangeAnimMode(pPlayer, PLRMODE_TOWNNEUTRAL);

    pPlayer->dwFlags = (pPlayer->dwFlags & ~(uint32_t)UNITFLAG_DRAWSHADOW) | UNITFLAG_TARGETABLE | UNITFLAG_CANBEATTACKED | UNITFLAG_ISVALIDTARGET;
    if (pPlayer->dwAnimMode == PLRMODE_DEAD || pPlayer->dwAnimMode == PLRMODE_DEATH)
    {
        D2COMMON_PATH_RemoveCollisionFootprintForUnit(pPlayer, TRUE);
        D2COMMON_PATH_SetUnitCollisionPattern(pPlayer, COLLISION_PATTERN_SMALL_NO_PRESENCE);
        D2COMMON_PATH_SetFootprintCollisionMask(pPlayer->pDynamicPath, COLLIDE_CORPSE);
    }

    pLocalPlayer = CUNIT_GetPlayer();
    if (!D2COMMON_10083_Return0(pRoom))
    {
        pLight = DLIGHTMAP_AddLight(pPlayer, (pLocalPlayer && pPlayer != pLocalPlayer) ? 1 : 0, 13, 255, 255, 255, 255);
        if (pLight)
        {
            pPlayer->pLight = pLight;
            CUNIT_SetUnitStartLight(pPlayer, DLIGHTMAP_GetRadius(pLight));
        }
    }

    D2COMMON_UNITS_AllocPlayerData(pPlayer);
    if (pPlayer != pLocalPlayer)
    {
        D2COMMON_PATH_SetDirection(pPlayer->pDynamicPath, (uint8_t)(SEED_RollRandomNumber(&pPlayer->pSeed) & 0x3F));
    }

    if (!CUNIT_GetPlayer())
        return;

    pPlayerRoom = CUNIT_GetPlayerRoom();
    if (!pPlayerRoom)
        return;

    D2COMMON_DUNGEON_GetAdjacentRoomsListFromRoom(pPlayerRoom, &ppRoomList, &nNumRooms);
    for (nRoom = 0; nRoom < (uint32_t)nNumRooms; ++nRoom)
    {
        for (pRoomUnit = ppRoomList[nRoom]->pUnitFirst; pRoomUnit; pRoomUnit = pRoomUnit->pRoomNext)
        {
            if (pRoomUnit->dwUnitType == UNIT_MONSTER && D2COMMON_STATES_CheckState(pRoomUnit, STATE_DOPPLEZON))
                GFX_RebuildUnitAnimForMode(pRoomUnit, 0);
        }
    }
}

//1.10f: D2Client.0x6FB48A60
//  Frees a player unit on the client; pPlayer in ecx. Its one caller is CUNIT_DestroyUnit's UNIT_PLAYER arm. In order: its slot in
//  gapClientPlayers (MONSTER_UnregisterPlayerSlot), its gfx info, and -- only while there is an act (gpAct) -- D2Common #10228 and
//  UNITROOM_RemoveUnitFromRoom; its dynamic path, freed and cleared; its inventory's items (CUNIT_FreeUnitInventory, bKeepItemsHeldElsewhere 0),
//  then the inventory itself, cleared; the skill list's cast light, taken with D2Common #10958 (SKILLS_SwapCastLight, NULL in its place) and
//  removed when there was one; CUNIT_GfxFreeUnitLightMap; the local player pointer cleared when this is the local player
//  (CUNIT_SetPlayerUnit(NULL)); the player data; and last the decoy rebuild, PLAYER_InitClientUnit's tail -- which, with the local player just
//  freed, finds no local player and does nothing.
//  BUG: the removal of the cast light cannot run from the one caller: CUNIT_DestroyUnit takes it with the same #10958 call, and removes it,
//  before it dispatches on the type, so this #10958 answers NULL. UNREACHABLE: reproduced.
void __fastcall PLAYER_FreeClientUnit(D2UnitStrc *pPlayer)
{
    D2ClientLightStrc *pCastLight;
    D2ActiveRoomStrc *pPlayerRoom;
    D2ActiveRoomStrc **ppRoomList;
    D2UnitStrc *pRoomUnit;
    int32_t nNumRooms;
    uint32_t nRoom;

    MONSTER_UnregisterPlayerSlot(pPlayer);
    GFX_FreeUnitGfxInfo(pPlayer);
    if (gpAct)
    {
        D2COMMON_10228(pPlayer);
        D2COMMON_UNITROOM_RemoveUnitFromRoom(pPlayer);
    }

    D2COMMON_PATH_FreeDynamicPath(NULL, pPlayer->pDynamicPath);
    pPlayer->pDynamicPath = NULL;
    CUNIT_FreeUnitInventory(pPlayer, 0);
    D2COMMON_INVENTORY_FreeInventory(pPlayer->pInventory);
    pPlayer->pInventory = NULL;

    pCastLight = (D2ClientLightStrc *)D2COMMON_SKILLS_SwapCastLight(pPlayer, NULL);
    if (pCastLight)
        DLIGHTMAP_RemoveLight(pCastLight);

    CUNIT_GfxFreeUnitLightMap(pPlayer);
    if (pPlayer == CUNIT_GetPlayer())
        CUNIT_SetPlayerUnit(NULL);
    D2COMMON_UNITS_FreePlayerData(NULL, pPlayer);

    if (!CUNIT_GetPlayer())
        return;

    pPlayerRoom = CUNIT_GetPlayerRoom();
    if (!pPlayerRoom)
        return;

    D2COMMON_DUNGEON_GetAdjacentRoomsListFromRoom(pPlayerRoom, &ppRoomList, &nNumRooms);
    for (nRoom = 0; nRoom < (uint32_t)nNumRooms; ++nRoom)
    {
        for (pRoomUnit = ppRoomList[nRoom]->pUnitFirst; pRoomUnit; pRoomUnit = pRoomUnit->pRoomNext)
        {
            if (pRoomUnit->dwUnitType == UNIT_MONSTER && D2COMMON_STATES_CheckState(pRoomUnit, STATE_DOPPLEZON))
                GFX_RebuildUnitAnimForMode(pRoomUnit, 0);
        }
    }
}

//  The row of gnAnimModeControl for a player's mode: Player.h's copy for the twenty modes it holds, and a row of zeros for any other.
//  DIFFERENCE: the original reads its own .data at the same offset ([ecx*4+6FB93CF8h] with ecx = 3 * mode and no bound, 0x6FB48B79); ours
//  reads [0] 0, which is neither 1 nor 2, [1] 0 for the switch and [2] 0 as the mode's end. A mode of 20 or more is UNREACHABLE.
static const int32_t sgnAnimModeControlNone[3] = { 0, 0, 0 };

static const int32_t *PLAYER_GetAnimModeControlRow(uint32_t dwAnimMode)
{
    if (dwAnimMode < 20)
        return gnAnimModeControl[dwAnimMode];

    return sgnAnimModeControlNone;
}

//1.10f: D2Client.0x6FB48B60
//  One frame of a player's animation mode. The path pointer is read once, at entry; the knockback's two path calls and PATH_AdjustDirection
//  take that copy, while the stamina arm and the room block read the field again. The original inlines UNITS_GetXPosition,
//  UNITS_GetYPosition and PLAYER_GetQueuedCommand here.
//
//  gnAnimModeControl holds three ints for each of the 20 player modes, and this function is its only reader. Its rows (the last column's
//  0 = never):
//
//      mode           col 0  col 1  col 2     mode           col 0  col 1  col 2
//      0  DEATH         0      2      0       10 CAST          2      1      3
//      1  NEUTRAL       0      0      0       11 THROW         2      1      3
//      2  WALK          1      0      1       12 KICK          2      1      3
//      3  RUN           1      0      1       13 SPECIAL1      0      1      2
//      4  GETHIT        0      1      2       14 SPECIAL2      2      1      3
//      5  TOWNNEUTRAL   0      0      0       15 SPECIAL3      2      1      3
//      6  TOWNWALK      1      0      1       16 SPECIAL4      2      1      3
//      7  ATTACK1       2      1      3       17 DEAD          0      2      0
//      8  ATTACK2       2      1      3       18 SEQUENCE      2      1      3
//      9  BLOCK         0      1      2       19 KNOCKBACK     1      3      1
//
//  The original indexes the table by dwAnimMode times three with no bound (0x6FB48B79); see PLAYER_GetAnimModeControlRow above. A player's mode
//  of 20 or more is UNREACHABLE with the stock server and data.
//
//  Column 0 = 1, a moving mode: the unit steps along its path (UNITMODE_StepUnitAlongPath, whose answer "the path is done" is kept as
//  bModeFinished); the local player also updates the level's pops at its position (DUNGEON_UpdatePops); and every player plays its footstep
//  (UNITSND_PlayFootstep).
//  Column 0 = 2, a skill mode, with a used skill: if bit 0 of the skill's flags is set -- a skill that moves the unit -- the unit steps along
//  its path, and when the path is done bit 1 is set and the skill's client do-func runs (SKILLS_RunUsedSkillClientDoFunc); the local player
//  updates its pops. Then, unless the do-func has just run, it runs on action frames 1, 2 and 3 while UNITFLAG_SKSRVDOFUNC is clear.
//  Column 1, how the animation advances (D2Common #10373, which clears nActionFrame and adds the speed to the frame): 0 advances it every frame;
//  1 advances it until the end of the frame cycle, then holds; 2 parks it at the end on the last frame (nSeqCurrentFramePrecise =
//  dwFrameCountPrecise - 256) with wAnimSpeed 0; 3, KNOCKBACK's alone, parks it on frame 1 (256) with wAnimSpeed 0.
//  Column 2, when the mode ends: 1, when the path is done; 2 and 3, at the end of the frame cycle -- the two are one test here.
//
//  A mode that ends: KNOCKBACK issues action 6 (CLTACTION_GETHIT) at the unit's own position -- the two coordinate copies read the path's
//  current point -- after the saved path is re-cached and reset to its previous type, with bInterruptKnockback TRUE. Any other mode: a player
//  with a queued command sets its pending flag, unk0xA8[42], for UNITMODE_RunQueuedCommand to take up; anything else issues CLTACTION_NEUTRAL,
//  bInterruptKnockback TRUE.
//
//  And, every frame: a unit whose room has changed has the flag cleared, and the local player, in a room, takes the level's portal flags, the
//  tile animation between the path's next room and its own, the NPC menu's abort on leaving town (NPCMENU_AbortOnPlayerAction, when the last
//  room was in town and this one is not), gbLastRoomInTown, the room's LOS-draw flag and the Worldstone's shake. The town test is normalised to
//  0 or 1 before the compare. Then PATH_AdjustDirection on the saved path; a RUNNING local player with no stamina left is sent walking to its
//  path's target -- a unit, whose type and id go with CLTACTION_WALKTOUNIT (UNIT_TYPES_COUNT and -1 when CUNIT_GetPathTargetUnit answers NULL),
//  or the path's first point with CLTACTION_WALKTOXY; the active states' funcs when the unit's state mask says so; and a unit standing (NEUTRAL
//  or TOWNNEUTRAL) on a cell its collision pattern cannot hold under COLLIDE_MASK_PLAYER_WW is warped to the nearest free coordinates.
void __fastcall PLAYER_AdvanceAnimMode(D2UnitStrc *pUnit)
{
    D2DynamicPathStrc *pPath;
    D2DynamicPathStrc *pPathNow;
    D2PlayerDataStrc *pPlayerData;
    D2SkillStrc *pSkill;
    D2UnitStrc *pTarget;
    D2ActiveRoomStrc *pRoom;
    D2ActiveRoomStrc *pNextRoom;
    D2ActiveRoomStrc *pRoomNow;
    D2C_ClientActions nAction;
    D2CoordStrc tCoord;
    BOOL bModeFinished;
    BOOL bFinished;
    int32_t bSkillActive;
    int32_t nSkillFlags;
    int32_t nModeEnd;
    int32_t nX;
    int32_t nY;
    int32_t nLevelId;
    int32_t nPortalFlags;
    int32_t bInTown;
    int32_t aActionParams[7];

    bModeFinished = FALSE;
    pPath = pUnit->pDynamicPath;

    if (PLAYER_GetAnimModeControlRow(pUnit->dwAnimMode)[0] == 1)
    {
        bModeFinished = UNITMODE_StepUnitAlongPath(pUnit);
        if (pUnit == CUNIT_GetPlayer())
        {
            nX = UNITS_GetXPosition(pUnit);
            nY = UNITS_GetYPosition(pUnit);
            pRoom = D2COMMON_UNITS_GetRoom(pUnit);
            D2COMMON_DUNGEON_UpdatePops(pRoom, nX, nY, D2COMMON_UNITS_IsCurrentRoomInvalid(pUnit));
        }
        UNITSND_PlayFootstep(pUnit);
    }

    if (PLAYER_GetAnimModeControlRow(pUnit->dwAnimMode)[0] == 2)
    {
        pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
        if (pSkill)
        {
            nSkillFlags = D2COMMON_SKILLS_GetFlags(pSkill);
            bSkillActive = 1;
            if (nSkillFlags & 1)
            {
                if (UNITMODE_StepUnitAlongPath(pUnit))
                {
                    D2COMMON_SKILLS_SetFlags(pSkill, nSkillFlags | 2);
                    SKILLS_RunUsedSkillClientDoFunc(pUnit);
                    bSkillActive = 0;
                }

                if (pUnit == CUNIT_GetPlayer())
                {
                    nX = UNITS_GetXPosition(pUnit);
                    nY = UNITS_GetYPosition(pUnit);
                    pRoom = D2COMMON_UNITS_GetRoom(pUnit);
                    D2COMMON_DUNGEON_UpdatePops(pRoom, nX, nY, D2COMMON_UNITS_IsCurrentRoomInvalid(pUnit));
                }
            }

            if (bSkillActive && !(pUnit->dwFlags & UNITFLAG_SKSRVDOFUNC) && (pUnit->nActionFrame == 1 || pUnit->nActionFrame == 2 || pUnit->nActionFrame == 3))
            {
                SKILLS_RunUsedSkillClientDoFunc(pUnit);
            }
        }
    }

    switch (PLAYER_GetAnimModeControlRow(pUnit->dwAnimMode)[1])
    {
    case 0:
        D2COMMON_UNITS_StopSequence(pUnit);
        break;

    case 1:
        if (!D2COMMON_UNITS_IsAtEndOfFrameCycle(pUnit))
            D2COMMON_UNITS_StopSequence(pUnit);
        break;

    case 2:
        if (D2COMMON_UNITS_IsAtEndOfFrameCycle(pUnit))
        {
            pUnit->nSeqCurrentFramePrecise = pUnit->dwFrameCountPrecise - 256;
            pUnit->wAnimSpeed = 0;
        }
        else
        {
            D2COMMON_UNITS_StopSequence(pUnit);
        }
        break;

    case 3:
        pUnit->nSeqCurrentFramePrecise = 256;
        pUnit->wAnimSpeed = 0;
        break;

    default:
        break;
    }

    if (pUnit->dwAnimMode == PLRMODE_SEQUENCE && (pUnit->dwFlags & UNITFLAG_SQGFXCHANGE))
        GFX_RebuildUnitAnimForMode(pUnit, 0);

    nModeEnd = PLAYER_GetAnimModeControlRow(pUnit->dwAnimMode)[2];
    if (nModeEnd == 1)
        bFinished = bModeFinished;
    else if (nModeEnd > 1 && nModeEnd <= 3)
        bFinished = D2COMMON_UNITS_IsAtEndOfFrameCycle(pUnit);
    else
        bFinished = FALSE;

    if (bFinished)
    {
        if (pUnit->dwAnimMode == PLRMODE_KNOCKBACK)
        {
            memset(aActionParams, 0, sizeof(aActionParams));

            aActionParams[0] = UNITS_GetXPosition(pUnit);

            aActionParams[1] = UNITS_GetYPosition(pUnit);

            D2COMMON_PATH_RecacheRoomIfNeeded(pPath);
            D2COMMON_PATH_ResetToPreviousType(pPath);
            PLAYER_DoClientAction(CLTACTION_GETHIT, pUnit, aActionParams, TRUE);
        }
        else
        {
            pPlayerData = pUnit->pPlayerData;
            if (pUnit->dwUnitType == UNIT_PLAYER && pPlayerData && pPlayerData->unk0xA8[43])
            {
                if (pUnit->dwUnitType == UNIT_PLAYER)
                    pPlayerData->unk0xA8[42] = 1;
            }
            else
            {
                PLAYER_DoClientAction(CLTACTION_NEUTRAL, pUnit, NULL, TRUE);
            }
        }
    }

    if (D2COMMON_UNITS_IsCurrentRoomInvalid(pUnit))
    {
        D2COMMON_UNITS_SetCurrentRoomInvalid(pUnit, 0);
        if (pUnit == CUNIT_GetPlayer())
        {
            pRoom = D2COMMON_UNITS_GetRoom(pUnit);
            if (pRoom)
            {
                nLevelId = D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);
                nPortalFlags = D2COMMON_UNITS_GetPlayerPortalFlags(pUnit);
                D2COMMON_UNITS_SetPlayerPortalFlags(pUnit, nPortalFlags | D2COMMON_DUNGEON_GetPortalFlagFromLevelId(nLevelId));

                pRoomNow = D2COMMON_UNITS_GetRoom(pUnit);
                pNextRoom = D2COMMON_PATH_GetNextRoom(pUnit->pDynamicPath);
                D2COMMON_DUNGEON_InitRoomTileAnimation(gpAct, pNextRoom, pRoomNow);

                bInTown = D2COMMON_DUNGEON_IsRoomInTown(pRoom) != 0;
                //  The flag can be another player's: PLAYER_InitClientUnit writes it for every player it builds.
                if (gbLastRoomInTown != bInTown && !bInTown)
                    NPCMENU_AbortOnPlayerAction(pUnit);
                gbLastRoomInTown = bInTown;

                ENV_SetPlayerRoomLosDraw(D2COMMON_DUNGEON_CheckLOSDraw(pRoom));
                QUESTSFX_SpawnWorldstoneShake(pRoom);
            }
        }
    }

    D2COMMON_10193_PATH_AdjustDirection(pPath);

    if (pUnit->dwAnimMode == PLRMODE_RUN && pUnit == CUNIT_GetPlayer() && !D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_STAMINA, 0))
    {
        memset(aActionParams, 0, sizeof(aActionParams));
        pPathNow = pUnit->pDynamicPath;
        pTarget = CUNIT_GetPathTargetUnit(pUnit);
        if (D2COMMON_PATH_GetTargetUnit(pPathNow))
        {
            aActionParams[0] = pTarget ? (int32_t)pTarget->dwUnitType : UNIT_TYPES_COUNT;
            aActionParams[1] = pTarget ? (int32_t)pTarget->dwUnitId : -1;
            nAction = CLTACTION_WALKTOUNIT;
        }
        else
        {
            aActionParams[0] = D2COMMON_10175_PathGetFirstPointX(pPathNow);
            aActionParams[1] = D2COMMON_10176_PathGetFirstPointY(pPathNow);
            nAction = CLTACTION_WALKTOXY;
        }
        PLAYER_DoClientAction(nAction, pUnit, aActionParams, FALSE);
    }

    if (D2COMMON_STATES_CheckStateMaskActiveOnUnit(pUnit))
        SKILLS_StatesRunActiveStateFuncs(pUnit);

    if (pUnit->dwAnimMode != PLRMODE_NEUTRAL && pUnit->dwAnimMode != PLRMODE_TOWNNEUTRAL)
        return;

    tCoord.nX = UNITS_GetXPosition(pUnit);

    tCoord.nY = UNITS_GetYPosition(pUnit);

    pRoom = D2COMMON_UNITS_GetRoom(pUnit);
    if (!pRoom)
        return;

    if (D2COMMON_COLLISION_CheckMaskWithPattern(pRoom, tCoord.nX, tCoord.nY, (D2C_CollisionPattern)D2COMMON_PATH_GetUnitCollisionPattern(pUnit), COLLIDE_MASK_PLAYER_WW))
    {
        D2COMMON_COLLISION_GetFreeCoordinates(pRoom, &tCoord, D2COMMON_UNITS_GetUnitSizeX(pUnit), COLLIDE_MASK_PLAYER_WW, TRUE);
        CUNIT_WarpUnit(pUnit, tCoord.nX, tCoord.nY, 0);
    }
}

//1.10f: D2Client.0x6FB49090
//  A player takes a client action: the PLAYER arm of the client action dispatch. nAction in ecx, pUnit in edx, pParams and bInterruptKnockback
//  on the stack. The answer is TRUE when the action was taken, FALSE on the refusal below. The original inlines UNITS_GetXPosition and
//  PLAYER_ClearQueuedCommand here.
//
//  Before the switch, in order:
//    * the skill list's cast light is taken (D2Common's SKILLS_SwapCastLight, NULL in its place) and removed when there was one;
//    * unless the action is CLTACTION_HIT, the used skill is ended: one whose flags have bit 0 -- the skill that moves the unit -- gets bit 1
//      and its client do-func (SKILLS_RunUsedSkillClientDoFunc); any other runs the do-func unless the unit is the local player or has
//      UNITFLAG_SKSRVDOFUNC; then the used skill is cleared. The bytes also store the flag bit into a frame slot (0x6FB490F3) that nothing
//      reads, which the C does not keep;
//    * a unit in PLRMODE_KNOCKBACK takes the action only when bInterruptKnockback is set -- the path's room re-cached first -- and otherwise
//      the call answers TRUE at once;
//    * the path is reset to its previous type, and the mode pair chosen: TOWNNEUTRAL and TOWNWALK in a town room, NEUTRAL and WALK outside.
//
//  The switch: 26 ids through a byte table onto 16 slots, unsigned. Ids 3..5 and 10..17 -- the monster's and the object's -- are the default,
//  FOG_DisplayAssert("0", line 1074) and exit. The arms:
//
//    WALKTOUNIT, WALKTOXY   the move started: the walk mode, TRUE
//    RUNTOXY, RUNTOUNIT     the move started: PLRMODE_RUN -- in town too
//    INTERACT               UNITMODE_InteractWithUnit(the type's low word, the GUID), TRUE
//    GETHIT                 the hit class kept, PLRMODE_GETHIT, the position synced within tolerance 1, TRUE
//    NEUTRAL                alive again, TARGETABLE, the neutral mode, and with a block the unit moved to its point, snapped when it had
//                           been dead; TRUE
//    DEATH                  the local player's panels closed; the hit class; the selection dropped when it is this unit; the position synced
//                           exactly; PLRMODE_DEATH; the dead state (below); the queued command cleared; TRUE
//    DEAD                   PLRMODE_DEAD and the dead state, TRUE
//    BLOCK                  the hit class; PLRMODE_BLOCK with a shield, or when the weapon class is WEAPONCLASS_HT2, two claws; the position
//                           synced within tolerance 1; TRUE
//    HIT                    the hit class; the overlay of PLRMODE_KNOCKBACK's hit; the mode sound, 3 for a shapeshifted player and 19 for any
//                           other; the position synced; TRUE. The mode is not changed
//    KNOCKBACK              the hit class; the saved path re-cached and made PATHTYPE_KNOCKBACK_CLIENT, distance 5, target the block's
//                           point, and started (bAllowInTown FALSE); PLRMODE_KNOCKBACK; TRUE
//    SKILL_XY, SKILL_UNIT   the skill by the block's id and owner GUID; one PLAYER_CanUseSkill refuses is the refusal below; otherwise the
//                           id of the skill it answers with -- Attack, when it swapped -- is written back into the block, and the GUID made
//                           -1 when that id changed; the skill started (SKILLS_CltStartSkillAtCoords, OnUnit): TRUE
//    SKILL1MODE             PLRMODE_SPECIAL1, the position synced, TRUE
//
//  The refusal -- a move not started, a skill not started or refused: the neutral mode, and for the local player its position synced exactly at
//  its own coordinates when both are nonzero; FALSE. Six arms reach it; the bytes carry it four times, the C reaches it by `break`.
//
//  The dead state is one block written twice in the original, in the DEATH and DEAD arms: the weapon class cache cleared, the unit's collision
//  made a corpse's (bForGameLogic FALSE), the unit marked dead, CANBEATTACKED cleared, TARGETABLE cleared unless it is a player's body
//  (STATE_PLAYERBODY), its animation list freed but its death nodes, every loaded-mode flag set to 0, and STAT_HITPOINTS 0.
//
//  BUG: a NULL pUnit would run the used skill's do-func (test esi, esi at 0x6FB490E3). UNREACHABLE: D2Common's UNITS_GetUsedSkill, just
//  before, asserts hUnit and exits.
//  BUG: a command of the local player's own that arrives in a knockback is not carried out but answers TRUE, and UNITMODE_PerformClientCommand
//  sends the command to the server when the answer is TRUE. REACHABLE: every such caller passes bInterruptKnockback FALSE.
//  BUG: the weapon class is written into pParams' own slot (0x6FB494FD) and COMPOSIT_GetWeaponClassId leaves it unwritten on one path -- a
//  right-arm item that is not an item -- where the compare then reads the pointer. The C starts its local with that pointer.
BOOL __fastcall PLAYER_DoClientAction(D2C_ClientActions nAction, D2UnitStrc *pUnit, int32_t *pParams, BOOL bInterruptKnockback)
{
    D2ClientLightStrc *pCastLight;
    D2UnitStrc *pPlayer;
    D2SkillStrc *pUsedSkill;
    D2SkillStrc *pSkill;
    D2DynamicPathStrc *pPath;
    int32_t nSkillFlags;
    int32_t nNeutralMode;
    int32_t nWalkMode;
    int32_t nSkillId;
    int32_t nOldSkillId;
    int32_t nWeaponClass;
    int32_t nX;
    int32_t nY;
    BOOL bWasDead;

    pCastLight = (D2ClientLightStrc *)D2COMMON_SKILLS_SwapCastLight(pUnit, NULL);
    if (pCastLight)
        DLIGHTMAP_RemoveLight(pCastLight);

    pPlayer = CUNIT_GetPlayer();
    if (nAction != CLTACTION_HIT)
    {
        pUsedSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
        if (pUsedSkill)
        {
            nSkillFlags = D2COMMON_SKILLS_GetFlags(pUsedSkill);
            if (nSkillFlags & 1)
            {
                D2COMMON_SKILLS_SetFlags(pUsedSkill, nSkillFlags | 2);
                SKILLS_RunUsedSkillClientDoFunc(pUnit);
            }
            else if (pUnit != pPlayer && (!pUnit || !(pUnit->dwFlags & UNITFLAG_SKSRVDOFUNC)))
            {
                SKILLS_RunUsedSkillClientDoFunc(pUnit);
            }
        }
        D2COMMON_UNITS_SetUsedSkill(pUnit, NULL);
    }

    pPath = pUnit->pDynamicPath;
    if (pUnit->dwAnimMode == PLRMODE_KNOCKBACK)
    {
        if (!bInterruptKnockback)
            return TRUE;
        D2COMMON_PATH_RecacheRoomIfNeeded(pPath);
    }
    D2COMMON_PATH_ResetToPreviousType(pPath);

    if (D2COMMON_DUNGEON_IsRoomInTown(D2COMMON_UNITS_GetRoom(pUnit)))
    {
        nNeutralMode = PLRMODE_TOWNNEUTRAL;
        nWalkMode = PLRMODE_TOWNWALK;
    }
    else
    {
        nNeutralMode = PLRMODE_NEUTRAL;
        nWalkMode = PLRMODE_WALK;
    }

    switch (nAction)
    {
    case CLTACTION_WALKTOUNIT:
        if (UNITMODE_StartMoveToUnit(pUnit, pParams[0], pParams[1]))
        {
            UNITMODE_SetAnimMode(pUnit, nWalkMode);
            return TRUE;
        }
        break;

    case CLTACTION_WALKTOXY:
        if (UNITMODE_StartMoveToLocation(pUnit, pParams[0], pParams[1]))
        {
            UNITMODE_SetAnimMode(pUnit, nWalkMode);
            return TRUE;
        }
        break;

    case CLTACTION_RUNTOXY:
        if (UNITMODE_StartMoveToLocation(pUnit, pParams[0], pParams[1]))
        {
            UNITMODE_SetAnimMode(pUnit, PLRMODE_RUN);
            return TRUE;
        }
        break;

    case CLTACTION_RUNTOUNIT:
        if (UNITMODE_StartMoveToUnit(pUnit, pParams[0], pParams[1]))
        {
            UNITMODE_SetAnimMode(pUnit, PLRMODE_RUN);
            return TRUE;
        }
        break;

    case CLTACTION_INTERACT:
        UNITMODE_InteractWithUnit((uint16_t)pParams[0], pParams[1]);
        return TRUE;

    case CLTACTION_GETHIT:
        pUnit->dwLastHitClass = pParams[2];
        UNITMODE_SetAnimMode(pUnit, PLRMODE_GETHIT);
        UNITMODE_SyncUnitPosition(pUnit, (uint16_t)pParams[0], (uint16_t)pParams[1], 1, 0, 0);
        return TRUE;

    case CLTACTION_NEUTRAL:
        bWasDead = CUNIT_IsUnitDead(pUnit);
        CUNIT_SetUnitAlive(pUnit);
        pUnit->dwFlags |= UNITFLAG_TARGETABLE;
        UNITMODE_SetAnimMode(pUnit, nNeutralMode);
        if (pParams)
            UNITMODE_MoveUnitToCoords(pUnit, pParams[0], pParams[1], bWasDead);
        return TRUE;

    case CLTACTION_DEATH:
        if (CUNIT_GetPlayer() == pUnit)
            UI_CloseAllPanels(TRUE, 0);

        pUnit->dwLastHitClass = pParams[2];
        if (SELECT_GetSelectedUnit() == pUnit)
        {
            //  8 is a mouse button held this frame; GAME_ResetClickState ends in SELECT_ClearSelectedUnit itself.
            if ((GAME_GetLeftClickModifierFlags() & 8) || (GAME_GetRightClickModifierFlags() & 8))
            {
                GAME_ResetClickState();
                SELECT_SetSelectedUnit(NULL);
            }
            else
            {
                SELECT_ClearSelectedUnit();
            }
        }

        UNITMODE_SyncUnitPosition(pUnit, (uint16_t)pParams[0], (uint16_t)pParams[1], 0, 0, 0);
        UNITMODE_SetAnimMode(pUnit, PLRMODE_DEATH);

        ITEM_ClearWeaponClassCache();
        D2COMMON_PATH_SetUnitDeadCollision(pUnit, FALSE);
        CUNIT_SetUnitDead(pUnit);
        pUnit->dwFlags &= ~(uint32_t)UNITFLAG_CANBEATTACKED;
        if (!D2COMMON_STATES_CheckState(pUnit, STATE_PLAYERBODY))
            pUnit->dwFlags &= ~(uint32_t)UNITFLAG_TARGETABLE;
        GFX_FreeUnitAnimListButDeath(pUnit);
        GFX_SetAllUnitModesLoaded(pUnit, 0);
        D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_HITPOINTS, 0, 0);

        if (pUnit->dwUnitType == UNIT_PLAYER && pUnit->pPlayerData)
        {
            pUnit->pPlayerData->unk0xA8[42] = 0;
            pUnit->pPlayerData->unk0xA8[43] = 0;
            pUnit->pPlayerData->unk0xA8[44] = 0;
            pUnit->pPlayerData->unk0xA8[45] = 0;
        }
        return TRUE;

    case CLTACTION_DEAD:
        UNITMODE_SetAnimMode(pUnit, PLRMODE_DEAD);

        ITEM_ClearWeaponClassCache();
        D2COMMON_PATH_SetUnitDeadCollision(pUnit, FALSE);
        CUNIT_SetUnitDead(pUnit);
        pUnit->dwFlags &= ~(uint32_t)UNITFLAG_CANBEATTACKED;
        if (!D2COMMON_STATES_CheckState(pUnit, STATE_PLAYERBODY))
            pUnit->dwFlags &= ~(uint32_t)UNITFLAG_TARGETABLE;
        GFX_FreeUnitAnimListButDeath(pUnit);
        GFX_SetAllUnitModesLoaded(pUnit, 0);
        D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_HITPOINTS, 0, 0);
        return TRUE;

    case CLTACTION_BLOCK:
        pUnit->dwLastHitClass = pParams[2];
        nWeaponClass = (int32_t)(intptr_t)pParams;
        if (D2COMMON_INVENTORY_GetEquippedShield(pUnit->pInventory, NULL) || (D2COMMON_COMPOSIT_GetWeaponClassId(pUnit, pUnit->pInventory, &nWeaponClass, -1, TRUE), nWeaponClass == WEAPONCLASS_HT2))
        {
            UNITMODE_SetAnimMode(pUnit, PLRMODE_BLOCK);
        }
        UNITMODE_SyncUnitPosition(pUnit, (uint16_t)pParams[0], (uint16_t)pParams[1], 1, 0, 0);
        return TRUE;

    case CLTACTION_HIT:
        pUnit->dwLastHitClass = pParams[2];
        UNITMODE_PlayHitOverlayForMode(pUnit, PLRMODE_KNOCKBACK);
        UNITSND_PlayModeSound(pUnit, D2COMMON_STATES_IsUnitShapeShifted(pUnit) ? 3 : 19, TRUE);
        UNITMODE_SyncUnitPosition(pUnit, (uint16_t)pParams[0], (uint16_t)pParams[1], 1, 0, 0);
        return TRUE;

    case CLTACTION_KNOCKBACK:
        pUnit->dwLastHitClass = pParams[2];
        D2COMMON_PATH_RecacheRoomIfNeeded(pPath);
        D2COMMON_PATH_SetType(pPath, PATHTYPE_KNOCKBACK_CLIENT);
        D2COMMON_10190_PATH_SetDistance(pPath, 5);
        D2COMMON_10170_PathSetTargetPos(pPath, pParams[0], pParams[1]);
        D2COMMON_PATH_StartUnitMove(pPath, pUnit, FALSE);
        UNITMODE_SetAnimMode(pUnit, PLRMODE_KNOCKBACK);
        return TRUE;

    case CLTACTION_SKILL_XY:
        pSkill = D2COMMON_SKILLS_GetSkillById(pUnit, (D2C_Skills)pParams[0], (D2UnitGUID)pParams[1]);
        if (pSkill)
        {
            if (!PLAYER_CanUseSkill(pUnit, &pSkill, NULL))
                break;

            nOldSkillId = pParams[0];
            nSkillId = D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szPlayerCpp, 812);
            pParams[0] = nSkillId;
            if (nSkillId != nOldSkillId)
                pParams[1] = -1;
        }
        if (SKILLS_CltStartSkillAtCoords(pUnit, pParams))
            return TRUE;
        break;

    case CLTACTION_SKILL_UNIT:
        pSkill = D2COMMON_SKILLS_GetSkillById(pUnit, (D2C_Skills)pParams[0], (D2UnitGUID)pParams[1]);
        if (pSkill)
        {
            if (!PLAYER_CanUseSkill(pUnit, &pSkill, NULL))
                break;

            nOldSkillId = pParams[0];
            nSkillId = D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szPlayerCpp, 812);
            pParams[0] = nSkillId;
            if (nSkillId != nOldSkillId)
                pParams[1] = -1;
        }
        if (SKILLS_CltStartSkillOnUnit(pUnit, pParams))
            return TRUE;
        break;

    case CLTACTION_SKILL1MODE:
        UNITMODE_SetAnimMode(pUnit, PLRMODE_SPECIAL1);
        UNITMODE_SyncUnitPosition(pUnit, (uint16_t)pParams[0], (uint16_t)pParams[1], 0, 0, 0);
        return TRUE;

    default:
        FOG_DisplayAssert("0", szPlayerCpp, 1074);
        exit(-1);
    }

    UNITMODE_SetAnimMode(pUnit, nNeutralMode);
    if (pUnit == CUNIT_GetPlayer())
    {
        nX = UNITS_GetXPosition(pUnit);
        nY = UNITS_GetYPosition(pUnit);
        if (nX && nY)
            UNITMODE_SyncUnitPosition(pUnit, (uint16_t)nX, (uint16_t)nY, 0, 0, 0);
    }
    return FALSE;
}

//1.10f: D2Client.0x6FB497C0
//  May the local player use this skill? pUnit in ecx, ppSkill in edx, pnUseState on the stack.
//
//  Any unit but the local player: TRUE at once. No skill, or a skill with no skills.txt row: FALSE. Otherwise the skill's use state
//  (PLRSKILLS_SkillsGetUseStateForClient) goes to *pnUseState when asked for -- the state of the skill as given, before any swap -- and:
//
//    * NOMANA, NOQUANTITY or SHAPERESTRICT (signed compares) on a skill whose skills.txt attacknomana is set -- byte 7 against gdwBitMasks[4]
//      -- SWAPS the skill: *ppSkill becomes the unit's highest-level Attack (D2Common #10950, SKILL_ATTACK) and the state is taken again;
//    * USABLE or PASSIVE answers TRUE;
//    * any other state plays its refusal sound and answers FALSE: the word at sgSnds[state], a player sound event, played when nonzero -- 21 for
//      NOMANA, 19 for NOQUANTITY, DISABLED and SHAPERESTRICT, silence for the rest. The index is asserted below sgNumSnds first
//      (sgNumSnds > eStatus, line 715) and compared signed.
//
//  The assert bounds the index from above only; the original's sgSnds read below 0 is the DIFFERENCE below.
BOOL __fastcall PLAYER_CanUseSkill(D2UnitStrc *pUnit, D2SkillStrc **ppSkill, int32_t *pnUseState)
{
    D2SkillsTxt *pSkillsTxt;
    D2SkillStrc *pAttackSkill;
    int32_t nUseState;
    uint16_t nSoundEvent;

    if (pUnit != CUNIT_GetPlayer())
        return TRUE;

    if (!ppSkill || !*ppSkill)
        return FALSE;

    nUseState = PLRSKILLS_SkillsGetUseStateForClient(pUnit, *ppSkill);
    if (pnUseState)
        *pnUseState = nUseState;

    pSkillsTxt = D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill(*ppSkill);
    if (!pSkillsTxt)
        return FALSE;

    if (nUseState > (int32_t)SKILLUSTATE_USABLE && (nUseState <= (int32_t)SKILLUSTATE_NOQUANTITY || nUseState == (int32_t)SKILLUSTATE_SHAPERESTRICT) && (((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_ATTACKNOMANA >> 3]
            & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_ATTACKNOMANA & 7]))
    {
        pAttackSkill = D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pUnit, SKILL_ATTACK);
        *ppSkill = pAttackSkill;
        nUseState = PLRSKILLS_SkillsGetUseStateForClient(pUnit, pAttackSkill);
    }

    if (nUseState == (int32_t)SKILLUSTATE_PASSIVE || nUseState == (int32_t)SKILLUSTATE_USABLE)
        return TRUE;

    if (sgNumSnds <= nUseState)
    {
        FOG_DisplayAssert("sgNumSnds > eStatus", szPlayerCpp, 715);
        exit(-1);
    }

    //  DIFFERENCE: below 0 the original reads its .data before sgSnds; ours plays no sound. A negative state is UNREACHABLE.
    nSoundEvent = (uint16_t)(nUseState >= 0 ? sgSnds[nUseState] : 0);
    if (nSoundEvent)
        UNITSND_PlayPlayerSoundEvent(pUnit, nSoundEvent);
    return FALSE;
}

//1.10f: D2Client.0x6FB498A0
//  Is a unit within a skill's aura range of the local player? pSkill in ecx, pTarget in edx, pnRange and pnDistance on the stack. Its one
//  caller is UNITMODE_MustMoveToTarget, for a skill whose srvdofunc is 19; the original inlines it in PLAYER_DispatchSkillClick.
//
//  No local player, skill, target or skills.txt row: FALSE, nothing written. Otherwise every number is the LOCAL player's, whoever owns the
//  skill: its level with bonuses (bBonus TRUE), skills.txt aurarangecalc evaluated for it into *pnRange, and its distance to pTarget into
//  *pnDistance. The answer is `distance <= *pnRange`, signed, with *pnRange read back from memory after *pnDistance is written -- the C does
//  the same.
BOOL __fastcall PLAYER_IsUnitInAuraRange(D2SkillStrc *pSkill, D2UnitStrc *pTarget, int32_t *pnRange, int32_t *pnDistance)
{
    D2UnitStrc *pPlayer;
    D2SkillsTxt *pSkillsTxt;
    int32_t nDistance;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer || !pSkill || !pTarget)
        return FALSE;

    pSkillsTxt = D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill(pSkill);
    if (!pSkillsTxt)
        return FALSE;

    *pnRange = D2COMMON_SKILLS_EvaluateSkillFormula(pPlayer, pSkillsTxt->dwAuraRangeCalc, (D2C_Skills)pSkillsTxt->nSkillId, D2COMMON_SKILLS_GetSkillLevel(pPlayer, pSkill, TRUE));
    nDistance = D2COMMON_UNITS_GetDistanceToOtherUnit(pPlayer, pTarget);
    *pnDistance = nDistance;
    return nDistance <= *pnRange;
}

//1.10f: D2Client.0x6FB49910
//  Clears gbClickConsumed. Its one caller is GAME_UpdateFrame, once a frame in a game.
void __cdecl PLAYER_ResetClickGuard()
{
    gbClickConsumed = 0;
}

//1.10f: D2Client.0x6FB49920
//  The local player's mouse click in the world: nClickEvent in ecx, nX in edx, nY and nMoveFlags on the stack. Its callers are in Game.cpp. It
//  builds the click record, D2ClientClickStrc, on its own frame, and its answer is what those callers keep as gbLeftButtonDown or
//  gbRightButtonDown. The original inlines UNITS_GetXPosition, UNITS_GetYPosition and PLAYER_ReleaseClickAndResetHireling here.
//
//  nClickEvent is a button and a phase. Each arm ORs one of six values into the click flags: 0 left press 5, 1 left held 9, 2 left release 11h,
//  3 right press 6, 4 right held 0Ah, 5 right release 12h -- bit 0 the left button, bit 1 the right, bit 2 press, bit 3 held, bit 4 release.
//  The bound is unsigned: any other value answers 1 once the record is built, and no arm runs.
//
//  Before the switch, in order. The ESC menu open answers 0. A second click in one frame answers 0: gbClickConsumed, cleared every frame by
//  PLAYER_ResetClickGuard, stops it -- unless the click is at (0, 0), which passes. The flag is set and the position turned into a game subtile
//  in place (SCROLL_ScreenToGameCoords). The record is cleared and its flag word stored 0 once more; the local player goes in, the selected
//  unit as the target, the converted position as the point -- replaced by the target's own position when it is an object or an item.
//  nMoveFlags is read once, as a byte: 8, with any stamina left, sets 40h, RUN; 4 sets 20h, STAND STILL. The callers build it from gbRunKeyHeld
//  and gbRunToggled (8) and gbStandStillKeyHeld (4).
//
//  The arms. A press (0, 3) selects the target or clears the selection. Arms 0, 1 and 3 ask PLAYER_ShouldHandleClick first and answer 1 when it
//  says no; arms 2 and 5 reach it through PLAYER_ReleaseClickAndResetHireling, which drops its answer. Arms 0, 1 and 3 then take the button's
//  skill and ask CUNIT_CanPlayerIssueCommand:
//
//    0  it may: 80h, dispatch, 1. It may not: the cursor's state work -- 6 uses the held item, 8 sends item message 76 with -1 -- and an item
//       still on the cursor goes out in item message 23 with its id and the answer is 0; with none, 1.
//    1  it may not: 1. The walk velocity is the player's walk speed (UNITS_GetRunAndWalkSpeedForPlayer) shifted to 8.8 and scaled by
//       STAT_VELOCITYPERCENT over 100, signed. The speed CHANGED when the path moves at that velocity and the click runs, or at another and it
//       walks. With a target: one that is not the path's target unit (CUNIT_GetPathTargetUnit), or a changed speed, becomes the path's target
//       (PATH_SetTargetUnit) and sets 80h; either way, dispatch. With none: STAND STILL dispatches; no queued command
//       (PLAYER_GetQueuedCommand) sets 80h and dispatches; a queued one answers 1.
//    2  clears the selection, runs PLAYER_ReleaseClickAndResetHireling's instructions and PLAYER_ResumeQueuedMove; 0.
//    3  it may not: 1. A skill whose skills.txt range is 1 sets 80h unless the click runs; dispatch, 1.
//    4  0Ah, and PLAYER_IssueRightSkillCommand's answer.
//    5  clears the selection, 12h, and PLAYER_ReleaseClickAndResetHireling's answer.
//
//  Every dispatch is PLAYER_DispatchClickAction, and answers 1. Bit 80h is set here and in PLAYER_IssueRightSkillCommand and read nowhere.
int32_t __fastcall PLAYER_OnMouseClick(int32_t nClickEvent, int32_t nX, int32_t nY, int32_t nMoveFlags)
{
    D2ClientClickStrc tClick;
    D2UnitStrc *pCursorItem;
    D2DynamicPathStrc *pPath;
    D2SkillsTxt *pSkillsTxt;
    BOOL bSpeedChanged;
    uint8_t nMoveFlagsByte;
    int32_t nWalkSpeed;
    int32_t nRunSpeed;
    int32_t nWalkVelocity;

    if (UI_GetUiWindowState(UI_ESCMENU))
        return 0;

    if (gbClickConsumed && (nX || nY))
        return 0;

    gbClickConsumed = 1;
    SCROLL_ScreenToGameCoords(&nX, &nY);

    memset(&tClick, 0, sizeof(tClick));
    tClick.dwClickFlags = 0;
    tClick.pUnit = CUNIT_GetPlayer();
    tClick.pTarget = SELECT_GetSelectedUnit();
    tClick.nX = nX;
    tClick.nY = nY;
    if (tClick.pTarget && (tClick.pTarget->dwUnitType == UNIT_OBJECT || tClick.pTarget->dwUnitType == UNIT_ITEM))
    {
        tClick.nX = UNITS_GetXPosition(tClick.pTarget);

        tClick.nY = UNITS_GetYPosition(tClick.pTarget);
    }

    nMoveFlagsByte = (uint8_t)nMoveFlags;
    if ((nMoveFlagsByte & 8) && D2COMMON_STATLIST_UnitGetStatValue(tClick.pUnit, STAT_STAMINA, 0))
        tClick.dwClickFlags |= 0x40;
    if (nMoveFlagsByte & 4)
        tClick.dwClickFlags |= 0x20;

    switch (nClickEvent)
    {
    case 0:
        if (tClick.pTarget)
            SELECT_SetSelectedUnit(tClick.pTarget);
        else
            SELECT_ClearSelectedUnit();

        tClick.dwClickFlags |= 5;
        if (!PLAYER_ShouldHandleClick(&tClick))
            return 1;

        tClick.pSkill = D2COMMON_UNITS_GetLeftSkill(tClick.pUnit);
        if (CUNIT_CanPlayerIssueCommand(tClick.pUnit, tClick.pSkill))
            goto DISPATCH_NOW;

        if (CURSOR_GetCursorState() == 6)
            UI_CursorSendUseHeldItem();
        else if (CURSOR_GetCursorState() == 8)
            MSG_SendServerItemMessage(76, -1);

        pCursorItem = D2COMMON_INVENTORY_GetCursorItem(tClick.pUnit->pInventory);
        if (!pCursorItem)
            return 1;

        MSG_SendServerItemMessage(23, (int32_t)pCursorItem->dwUnitId);
        return 0;

    case 1:
        tClick.dwClickFlags |= 9;
        if (!PLAYER_ShouldHandleClick(&tClick))
            return 1;

        tClick.pSkill = D2COMMON_UNITS_GetLeftSkill(tClick.pUnit);
        if (!CUNIT_CanPlayerIssueCommand(tClick.pUnit, tClick.pSkill))
            return 1;

        //  BUG: the player is tested for NULL (0x6FB49B50) and then its path is read untested (0x6FB49B80). UNREACHABLE: the test cannot be
        //  taken, since CUNIT_CanPlayerIssueCommand, called first, reads pInventory off the unit.
        //  The original lets the walk speed land in nMoveFlags' own stack slot (0x6FB49B61), which nothing reads again; the C gives it a local.
        D2COMMON_UNITS_GetRunAndWalkSpeedForPlayer((int32_t)(intptr_t)tClick.pUnit, tClick.pUnit ? tClick.pUnit->dwClassId : -1, &nWalkSpeed, &nRunSpeed);
        nWalkSpeed <<= 8;
        pPath = tClick.pUnit->pDynamicPath;
        nWalkVelocity = nWalkSpeed * D2COMMON_STATLIST_UnitGetStatValue(tClick.pUnit, STAT_VELOCITYPERCENT, 0) / 100;
        if (D2COMMON_PATH_GetVelocity(pPath) == nWalkVelocity)
            bSpeedChanged = (tClick.dwClickFlags & 0x40) != 0;
        else
            bSpeedChanged = (tClick.dwClickFlags & 0x40) == 0;

        if (tClick.pTarget)
        {
            if (tClick.pTarget != CUNIT_GetPathTargetUnit(tClick.pUnit) || bSpeedChanged)
            {
                D2COMMON_PATH_SetTargetUnit(pPath, tClick.pTarget);
                goto DISPATCH_NOW;
            }
            goto DISPATCH;
        }

        if (tClick.dwClickFlags & 0x20)
            goto DISPATCH;

        if (!PLAYER_GetQueuedCommand(tClick.pUnit))
            goto DISPATCH_NOW;

        return 1;

    case 2:
        SELECT_ClearSelectedUnit();
        tClick.dwClickFlags |= 0x11;

        PLAYER_ReleaseClickAndResetHireling(&tClick);
        PLAYER_ResumeQueuedMove(&tClick);
        return 0;

    case 3:
        if (tClick.pTarget)
            SELECT_SetSelectedUnit(tClick.pTarget);
        else
            SELECT_ClearSelectedUnit();

        tClick.dwClickFlags |= 6;
        if (!PLAYER_ShouldHandleClick(&tClick))
            return 1;

        tClick.pSkill = D2COMMON_UNITS_GetRightSkill(tClick.pUnit);
        if (!CUNIT_CanPlayerIssueCommand(tClick.pUnit, tClick.pSkill))
            return 1;

        //  BUG: the skills.txt row is read untested at +14h; its lookup answers NULL for an id out of range. UNREACHABLE.
        pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(D2COMMON_SKILLS_GetSkillIdFromSkill(tClick.pSkill, szPlayerCpp, 2525));
        if ((int8_t)pSkillsTxt->nRange == 1 && !(tClick.dwClickFlags & 0x40))
            goto DISPATCH_NOW;
        goto DISPATCH;

    case 4:
        tClick.dwClickFlags |= 0x0A;
        return PLAYER_IssueRightSkillCommand(&tClick);

    case 5:
        SELECT_ClearSelectedUnit();
        tClick.dwClickFlags |= 0x12;
        return PLAYER_ReleaseClickAndResetHireling(&tClick);

    default:
        return 1;
    }

DISPATCH_NOW:
    tClick.dwClickFlags |= 0x80;
DISPATCH:
    PLAYER_DispatchClickAction(&tClick);
    return 1;
}

//1.10f: D2Client.0x6FB49D70
//  A released left button carries the move on to the release point: pClick in ecx. Its one caller is PLAYER_OnMouseClick's release arm. A
//  PLAYER unit whose path has no target unit, in mode WALK or TOWNWALK, sends the click's point as command 1, Walk; in RUN, as 3, Run; any other
//  mode, a NULL unit or another type, nothing. The point is the record's: the release position as PLAYER_OnMouseClick converted it.
//  The original inlines PLAYER_SendMoveToReachablePoint here.
void __fastcall PLAYER_ResumeQueuedMove(D2ClientClickStrc *pClick)
{
    D2UnitStrc *pUnit;
    int32_t nClientCmd;

    pUnit = pClick->pUnit;
    if (!pUnit || pUnit->dwUnitType != UNIT_PLAYER || CUNIT_GetPathTargetUnit(pUnit))
        return;

    switch (pUnit->dwAnimMode)
    {
    case PLRMODE_WALK:
    case PLRMODE_TOWNWALK:
        nClientCmd = 1;
        break;

    case PLRMODE_RUN:
        nClientCmd = 3;
        break;

    default:
        return;
    }

    PLAYER_SendMoveToReachablePoint(nClientCmd, pUnit, pClick->nX, pClick->nY);
}

//1.10f: D2Client.0x6FB49DF0
//  What a click does: pClick in ecx. Two callers, PLAYER_OnMouseClick and PLAYER_IssueRightSkillCommand; neither reads an answer. The original
//  inlines PLAYER_GetQueuedCommand, PLAYER_SendMoveToReachablePoint, PLAYER_SetQueuedCommand, UNITS_GetYPosition,
//  DATATBLS_GetSkillsTxtRecordById and DATATBLS_GetMonStatsTxtRecord in it.
//
//  The cast gate. A right click (2) with a skill whose range class (D2Common #11016) is not 1 returns when the skill has no skills.txt row;
//  plays player sound event 18h and returns when the skill lacks InTown and the player's room is in town; and returns for a passive skill. The
//  room is fetched before the flag is tested.
//
//  The move commands go into the record for everything below: running (40h) moves to a point with command 3 and to a unit with 4, else 1 and 2.
//
//  A queued interaction is dropped. A player with a queued command (PLAYER_GetQueuedCommand) has it cleared (PLAYER_ClearQueuedCommand) when
//  it is not 19, Interact, and when it is 19 and its target -- the queued type and GUID -- still exists; a 19 whose target is gone stays.
//
//  No skill: return. SELECT_SkillsResolveClientTarget then picks the target and corrects the point in the record, searching on STAND STILL
//  (20h) or on a right click with no target. The cursor's state work follows, as in PLAYER_OnMouseClick's press, and the click goes on.
//
//  The judgement, on a copy of the flags and the range class taken again. No skills.txt row, or no target: MOVE OR CAST. A right click with a
//  non-melee skill takes an object or an item only with TargetItem (bItemTargetAllowed), else MOVE OR CAST; a tile (a warp) sets 100h in the
//  copy and goes to MOVE OR CAST -- a store nothing reads after it. A dead monster for a skill without TargetCorpse: MOVE OR CAST. Else the
//  target. The row is NULL at 0x6FB49F7C only for a NULL skill, which has returned (0x6FB49EDB), so that test's arm is dead; the C keeps it.
//
//  MOVE OR CAST. With STAND STILL, or a right click with a non-melee skill: PLAYER_CanUseSkill -- a refusal returns, and its use state lands in
//  the slot the range class had -- and the click goes out as a cast (PLAYER_SendClickCommand, bOnUnit FALSE) with the selection cleared,
//  unless the skill is now Attack, its state was NOMANA and there is no STAND STILL: PLAYER_CanUseSkill swapped a skill with no mana for Attack,
//  and the click walks instead. Otherwise it walks: PLAYER_ClampClickTargetAndThrottle gates it, the move is PLAYER_SendMoveToReachablePoint's,
//  and the selection is cleared.
//
//  The target. Its type and GUID are kept. An object, item or tile target not allowed above, on a right click with a non-melee skill, returns
//  -- the type test UNITS_GetXPosition opens with, signed. A skill with no mode for this unit (SKILLS_GetSkillModeFromUnit 0): STAND STILL
//  returns, else INTERACT OR APPROACH. A TargetItem skill goes out as a cast on the unit (bOnUnit TRUE) when PLAYER_CanUseSkill allows, and
//  returns. Then a target that can be attacked (UNITFLAG_CANBEATTACKED) and -- for a skill with neither TargetPet nor TargetAlly -- is
//  selectable (CUNIT_IsUnitSelectableTarget) is taken by its type:
//
//    OBJECT, without STAND STILL: found again by GUID (none: return); an object with no objects.txt row asserts "ptObjectData" (line 1220) and
//      exits; a town portal cannot be operated while the local player is STATE_JUST_PORTALED. In interact range and not blocked
//      (COLLIDE_MASK_PLAYER_FLYING): command 19 on it, if it can be operated. Else a walk to it and 19 queued on it (PLAYER_SetQueuedCommand)
//      if it can be operated.
//    PLAYER, both it and the player in town: close -- D2Common's #10399 at most 2, compared UNSIGNED -- and not blocked, command 19; else a
//      move to it (nMoveToUnitCmd) and 19 queued (PLAYER_SetQueuedCommand). Otherwise PLAYER_DispatchSkillClick.
//    any other type: in town only with an InTown skill (no skill: return), then PLAYER_DispatchSkillClick; out of town,
//      PLAYER_DispatchSkillClick.
//
//  A target that is not taken: STAND STILL casts (PLAYER_CanUseSkill, then PLAYER_SendClickCommand, FALSE) and returns; else INTERACT OR
//  APPROACH: any type but a monster goes to PLAYER_InteractOrApproachTarget, and a monster only on a press (4) -- there too when its
//  monstats.txt row has interact, else a move to it (nMoveToUnitCmd).
//
//  BUG: the InTown row of the any-type arm is read at +5 untested (the lookup answers NULL for an id out of range). UNREACHABLE.
void __fastcall PLAYER_DispatchClickAction(D2ClientClickStrc *pClick)
{
    D2UnitStrc *pUnit;
    D2UnitStrc *pTarget;
    D2UnitStrc *pObject;
    D2ActiveRoomStrc *pRoom;
    D2SkillsTxt *pSkillsTxt;
    D2SkillsTxt *pModeSkillsTxt;
    D2MonStatsTxt *pMonStatsTxt;
    D2C_UnitTypes nTargetType;
    uint32_t dwTargetGuid;
    uint32_t bAllowSearch;
    uint32_t bSelectable;
    int32_t nRange;
    int32_t nRangeOrUseState;
    int32_t nClickFlags;
    int32_t nQueuedCmd;
    int32_t nQueuedType;
    int32_t nQueuedGuid;
    int32_t nTargetX;
    int32_t nTargetY;
    BOOL bItemTargetAllowed;
    BOOL bCanOperate;

    nRange = D2COMMON_11016(pClick->pUnit, pClick->pSkill);
    if (pClick->pSkill && (pClick->dwClickFlags & 2) && nRange != 1)
    {
        pSkillsTxt = D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill(pClick->pSkill);
        if (!pSkillsTxt)
            return;

        pRoom = D2COMMON_UNITS_GetRoom(pClick->pUnit);
        if (!(((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_INTOWN >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_INTOWN & 7]) && D2COMMON_DUNGEON_IsRoomInTown(pRoom))
        {
            UNITSND_PlayPlayerSoundEvent(pClick->pUnit, 0x18);
            return;
        }

        if (((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_PASSIVE >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_PASSIVE & 7])
        {
            return;
        }
    }

    if (pClick->dwClickFlags & 0x40)
    {
        pClick->nMoveToPointCmd = 3;
        pClick->nMoveToUnitCmd = 4;
    }
    else
    {
        pClick->nMoveToPointCmd = 1;
        pClick->nMoveToUnitCmd = 2;
    }

    nQueuedCmd = PLAYER_GetQueuedCommand(pClick->pUnit);
    if (nQueuedCmd)
    {
        if (nQueuedCmd != 19)
        {
            PLAYER_ClearQueuedCommand(pClick->pUnit);
        }
        else
        {
            nQueuedType = PLAYER_GetQueuedTargetType(pClick->pUnit);
            nQueuedGuid = PLAYER_GetQueuedTargetGuid(pClick->pUnit);
            if (CUNIT_FindUnitByGuidAndType((uint32_t)nQueuedGuid, (D2C_UnitTypes)nQueuedType))
                PLAYER_ClearQueuedCommand(pClick->pUnit);
        }
    }

    if (!pClick->pSkill)
        return;

    bAllowSearch = ((uint32_t)pClick->dwClickFlags >> 5) & 1;
    if ((pClick->dwClickFlags & 2) && !pClick->pTarget)
        bAllowSearch = 1;
    pClick->pTarget = SELECT_SkillsResolveClientTarget(pClick->pSkill, pClick->pTarget, &pClick->nX, &pClick->nY, (uint8_t)pClick->dwClickFlags, (int32_t)bAllowSearch);

    if (CURSOR_GetCursorState() == 6)
        UI_CursorSendUseHeldItem();
    else if (CURSOR_GetCursorState() == 8)
        MSG_SendServerItemMessage(76, -1);

    pUnit = pClick->pUnit;
    pTarget = pClick->pTarget;
    nClickFlags = pClick->dwClickFlags;
    bItemTargetAllowed = FALSE;
    nRangeOrUseState = D2COMMON_11016(pUnit, pClick->pSkill);
    pSkillsTxt = NULL;
    if (pClick->pSkill)
    {
        pSkillsTxt = D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill(pClick->pSkill);
        if (!pSkillsTxt)
            goto MOVE_OR_CAST;
    }

    if (!pTarget)
        goto MOVE_OR_CAST;

    if (!pSkillsTxt)
        goto TARGET;

    if ((pClick->dwClickFlags & 2) && nRangeOrUseState != 1)
    {
        if (pTarget->dwUnitType == UNIT_OBJECT || pTarget->dwUnitType == UNIT_ITEM)
        {
            if (!(((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_TARGETITEM >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_TARGETITEM & 7]))
            {
                goto MOVE_OR_CAST;
            }
            bItemTargetAllowed = TRUE;
        }
        else if (pTarget->dwUnitType == UNIT_TILE)
        {
            //  `or ah, 1` (0x6FB49FA1): nothing reads 100h after it.
            nClickFlags |= 0x100;
            goto MOVE_OR_CAST;
        }
    }

    if (pTarget->dwUnitType == UNIT_MONSTER && CUNIT_IsUnitDead(pTarget) && !(((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_TARGETCORPSE >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_TARGETCORPSE & 7]))
    {
        goto MOVE_OR_CAST;
    }
    goto TARGET;

MOVE_OR_CAST:
    if ((nClickFlags & 0x20) || ((nClickFlags & 2) && nRangeOrUseState != 1))
    {
        nRangeOrUseState = 0;
        if (!PLAYER_CanUseSkill(pClick->pUnit, &pClick->pSkill, &nRangeOrUseState))
            return;

        if (!pClick->pSkill || (int32_t)D2COMMON_SKILLS_GetSkillIdFromSkill(pClick->pSkill, szPlayerCpp, 1967) != (int32_t)SKILL_ATTACK || nRangeOrUseState != (int32_t)SKILLUSTATE_NOMANA || (nClickFlags & 0x20))
        {
            PLAYER_SendClickCommand(pClick, FALSE);
            SELECT_SetSelectedUnit(NULL);
            return;
        }

        if (!PLAYER_ClampClickTargetAndThrottle(pClick->pUnit, pClick))
            return;

        PLAYER_SendMoveToReachablePoint(pClick->nMoveToPointCmd, pUnit, pClick->nX, pClick->nY);
        SELECT_SetSelectedUnit(NULL);
        return;
    }

    if (!PLAYER_ClampClickTargetAndThrottle(pClick->pUnit, pClick))
        return;

    PLAYER_SendMoveToReachablePoint(pClick->nMoveToPointCmd, pUnit, pClick->nX, pClick->nY);
    SELECT_SetSelectedUnit(NULL);
    return;

TARGET:
    nTargetType = pTarget->dwUnitType;
    dwTargetGuid = pTarget->dwUnitId;
    if (!bItemTargetAllowed && (nTargetType == UNIT_OBJECT || ((int32_t)nTargetType > (int32_t)UNIT_MISSILE && (int32_t)nTargetType <= (int32_t)UNIT_TILE)) && (nClickFlags & 2) && nRangeOrUseState != 1)
    {
        return;
    }

    if (!D2COMMON_SKILLS_GetSkillModeFromUnit(pUnit, pClick->pSkill))
    {
        if (nClickFlags & 0x20)
            return;
        goto INTERACT_OR_APPROACH;
    }

    if (pClick->pSkill)
    {
        pModeSkillsTxt = D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill(pClick->pSkill);
        if (pModeSkillsTxt && (((uint8_t *)pModeSkillsTxt->dwFlags)[SKILLSFLAGINDEX_TARGETITEM >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_TARGETITEM & 7]))
        {
            if (PLAYER_CanUseSkill(pClick->pUnit, &pClick->pSkill, NULL))
                PLAYER_SendClickCommand(pClick, TRUE);
            return;
        }
    }

    bSelectable = (pTarget->dwFlags >> 2) & 1;
    if (bSelectable)
    {
        //  A dword test of the byte (`xor ecx, ecx / mov cl`, `test [edx+4], ecx`, 0x6FB4A1CC..0x6FB4A1DC). The row is not NULL here: only the
        //  dead arm above reaches the target without one.
        if (!(((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_TARGETPET >> 3] & gdwBitMasks[SKILLSFLAGINDEX_TARGETPET & 7]) && !(((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_TARGETALLY >> 3] & gdwBitMasks[SKILLSFLAGINDEX_TARGETALLY & 7]))
        {
            bSelectable = (uint32_t)CUNIT_IsUnitSelectableTarget(pUnit, pTarget);
        }

        if (bSelectable)
        {
            if (nTargetType == UNIT_OBJECT)
            {
                if (nClickFlags & 0x20)
                    return;

                pObject = CUNIT_FindUnitByGuidAndType(dwTargetGuid, UNIT_OBJECT);
                if (!pObject)
                    return;

                if (!D2COMMON_DATATBLS_GetObjectsTxtRecord(pObject->dwClassId))
                {
                    FOG_DisplayAssert("ptObjectData", szPlayerCpp, 1220);
                    exit(-1);
                }

                bCanOperate = TRUE;
                if (pObject->dwClassId == OBJECT_TOWN_PORTAL && D2COMMON_STATES_CheckState(CUNIT_GetPlayer(), STATE_JUST_PORTALED))
                {
                    bCanOperate = FALSE;
                }

                if (D2COMMON_UNITS_IsObjectInInteractRange(pClick->pUnit, pClick->pTarget) && !D2COMMON_UNITS_TestCollisionBetweenInteractingUnits(pClick->pUnit, pClick->pTarget, COLLIDE_MASK_PLAYER_FLYING))
                {
                    if (bCanOperate)
                        UNITMODE_PerformClientCommand(19, pClick->pUnit, UNIT_OBJECT, (int32_t)dwTargetGuid, 0);
                    return;
                }

                nTargetY = UNITS_GetYPosition(pClick->pTarget);
                nTargetX = UNITS_GetXPosition(pClick->pTarget);
                PLAYER_SendMoveToReachablePoint(pClick->nMoveToPointCmd, pClick->pUnit, nTargetX, nTargetY);
                if (bCanOperate)
                    PLAYER_SetQueuedCommand(19, pClick->pUnit, UNIT_OBJECT, dwTargetGuid);
                return;
            }

            if (nTargetType != UNIT_PLAYER)
            {
                if (D2COMMON_DUNGEON_IsRoomInTown(D2COMMON_UNITS_GetRoom(pTarget)))
                {
                    if (!pClick->pSkill)
                        return;

                    pModeSkillsTxt = DATATBLS_GetSkillsTxtRecordById(D2COMMON_SKILLS_GetSkillIdFromSkill(pClick->pSkill, szPlayerCpp, 1882));
                    if (!(((uint8_t *)pModeSkillsTxt->dwFlags)[SKILLSFLAGINDEX_INTOWN >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_INTOWN & 7]))
                    {
                        return;
                    }
                }

                PLAYER_DispatchSkillClick(pClick, nTargetType, dwTargetGuid);
                return;
            }

            if (D2COMMON_DUNGEON_IsRoomInTown(D2COMMON_UNITS_GetRoom(pTarget)) && D2COMMON_DUNGEON_IsRoomInTown(D2COMMON_UNITS_GetRoom(pUnit)))
            {
                //  BUG: #10399 answers -1 for units that overlap, which this unsigned compare takes as far (PLAYER_InteractOrApproachTarget compares
                //  it signed). REACHABLE.
                if ((uint32_t)D2COMMON_10399(pClick->pTarget, pClick->pUnit) <= 2 && !D2COMMON_UNITS_TestCollisionBetweenInteractingUnits(pClick->pTarget, pClick->pUnit, COLLIDE_MASK_PLAYER_FLYING))
                {
                    UNITMODE_PerformClientCommand(19, pClick->pUnit, UNIT_PLAYER, (int32_t)dwTargetGuid, 0);
                    return;
                }

                UNITMODE_PerformClientCommand(pClick->nMoveToUnitCmd, pClick->pUnit, UNIT_PLAYER, (int32_t)dwTargetGuid, 0);

                PLAYER_SetQueuedCommand(19, pClick->pUnit, UNIT_PLAYER, dwTargetGuid);
                return;
            }

            PLAYER_DispatchSkillClick(pClick, UNIT_PLAYER, dwTargetGuid);
            return;
        }
    }

    if (nClickFlags & 0x20)
    {
        if (PLAYER_CanUseSkill(pClick->pUnit, &pClick->pSkill, NULL))
            PLAYER_SendClickCommand(pClick, FALSE);
        return;
    }

INTERACT_OR_APPROACH:
    if (nTargetType != UNIT_MONSTER)
    {
        PLAYER_InteractOrApproachTarget(pClick, (int32_t)nTargetType, (int32_t)dwTargetGuid);
        return;
    }

    if (!(nClickFlags & 4))
        return;

    pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pTarget->dwClassId);
    if (pMonStatsTxt && (pMonStatsTxt->nMonStatsFlags[MONSTATSFLAGINDEX_INTERACT >> 3] & (uint8_t)gdwBitMasks[MONSTATSFLAGINDEX_INTERACT & 7]))
    {
        PLAYER_InteractOrApproachTarget(pClick, UNIT_MONSTER, (int32_t)dwTargetGuid);
        return;
    }

    UNITMODE_PerformClientCommand(pClick->nMoveToUnitCmd, pUnit, UNIT_MONSTER, (int32_t)dwTargetGuid, 0);
}

//1.10f: D2Client.0x6FB4A570
//  A click sent as a skill command: pClick in ecx, bOnUnit in edx. Nothing happens with the cursor below the play area -- CURSOR_GetMouseY()
//  above UI_GetPlayAreaBottom(), the screen height less 49, signed.
//
//  bOnUnit exactly 1: the command goes at the record's target, its type and GUID -- UNIT_TYPES_COUNT and -1 with no target; any other value: at
//  the record's point. The left button (1) and the right (2) are tested apart and both fire when both are set, the flag word read again for the
//  right. The commands, from UNITMODE_PerformClientCommand's table:
//
//                         plain   stand still   held   stand still, held
//      left, on a unit      6          7          9           10
//      right, on a unit    13         14         16           17
//      left, at a point     5          5          8            8
//      right, at a point   12         12         15           15
//
//  STAND STILL (20h) is read only on a unit.
void __fastcall PLAYER_SendClickCommand(D2ClientClickStrc *pClick, int32_t bOnUnit)
{
    D2UnitStrc *pTarget;
    int32_t nPlayAreaBottom;
    int32_t nTargetType;
    int32_t nTargetGuid;
    int32_t nClientCmd;

    nPlayAreaBottom = UI_GetPlayAreaBottom();
    if (CURSOR_GetMouseY() > nPlayAreaBottom)
        return;

    if (bOnUnit == 1)
    {
        pTarget = pClick->pTarget;
        nTargetType = UNIT_TYPES_COUNT;
        if (pTarget)
        {
            nTargetType = pTarget->dwUnitType;
            nTargetGuid = (int32_t)pTarget->dwUnitId;
        }
        else
        {
            nTargetGuid = -1;
        }

        if (pClick->dwClickFlags & 1)
        {
            if (pClick->dwClickFlags & 0x20)
                nClientCmd = (pClick->dwClickFlags & 8) ? 10 : 7;
            else
                nClientCmd = (pClick->dwClickFlags & 8) ? 9 : 6;
            UNITMODE_PerformClientCommand(nClientCmd, pClick->pUnit, nTargetType, nTargetGuid, 0);
        }

        if (pClick->dwClickFlags & 2)
        {
            if (pClick->dwClickFlags & 0x20)
                nClientCmd = (pClick->dwClickFlags & 8) ? 17 : 14;
            else
                nClientCmd = (pClick->dwClickFlags & 8) ? 16 : 13;
            UNITMODE_PerformClientCommand(nClientCmd, pClick->pUnit, nTargetType, nTargetGuid, 0);
        }
        return;
    }

    if (pClick->dwClickFlags & 1)
        UNITMODE_PerformClientCommand((pClick->dwClickFlags & 8) ? 8 : 5, pClick->pUnit, pClick->nX, pClick->nY, 0);

    if (pClick->dwClickFlags & 2)
        UNITMODE_PerformClientCommand((pClick->dwClickFlags & 8) ? 15 : 12, pClick->pUnit, pClick->nX, pClick->nY, 0);
}

//1.10f: D2Client.0x6FB4A6B0
//  A move to a point, sent as where the path can get: nClientCmd in ecx, pUnit in edx, nX and nY on the stack. The point becomes the unit's
//  path target; when PATH_StartUnitMove finds a route, the command goes out with the path's last point (D2Common #10177, #10178) rather than
//  the point asked for -- a click into a wall sends the reachable spot before it -- and with no route nothing is sent. The original inlines it
//  in PLAYER_ResumeQueuedMove, PLAYER_DispatchClickAction (twice) and PLAYER_InteractOrApproachTarget. The body leaves 1 in eax after a route and
//  PATH_StartUnitMove's 0 without; nothing reads it, so the function is void.
void __fastcall PLAYER_SendMoveToReachablePoint(int32_t nClientCmd, D2UnitStrc *pUnit, int32_t nX, int32_t nY)
{
    D2DynamicPathStrc *pPath;
    int32_t nLastX;
    int32_t nLastY;

    pPath = pUnit->pDynamicPath;
    D2COMMON_10170_PathSetTargetPos(pPath, nX, nY);
    if (!D2COMMON_PATH_StartUnitMove(pPath, pUnit, FALSE))
        return;

    nLastX = D2COMMON_10177_PATH_GetLastPointX(pPath);
    nLastY = D2COMMON_10178_PATH_GetLastPointY(pPath);
    UNITMODE_PerformClientCommand(nClientCmd, pUnit, nLastX, nLastY, 0);
}

//1.10f: D2Client.0x6FB4A710
//  A skill click on a unit: cast now, or walk into range and queue the cast. pClick in ecx, nTargetType in edx, dwTargetGuid on the stack. The
//  original inlines PLAYER_IsUnitInAuraRange and PLAYER_SetQueuedCommand here.
//
//  PLAYER_CanUseSkill first -- a refusal returns, and it may swap the record's skill for Attack -- then the skill's range class, D2Common's
//  #11016: skills.txt range, whose codes are 0 none, 1 h2h, 2 rng, 3 both, 4 loc, both resolved by the weapon. The target is found again by GUID
//  and type (none: return). In melee range -- UNITS_IsInMeleeRange of the record's unit and target, with the found unit's moving mode as the
//  bonus -- the click is cast on the unit (PLAYER_SendClickCommand, TRUE).
//
//  A skill whose srvdofunc is 19 (+2Eh): the target and the unit are asserted, "ptMouse->hSelected" (line 1327) and "ptMouse->hUnit" (1328).
//  STAND STILL casts on the unit, and so does a target within the skill's aura range of the local player (PLAYER_IsUnitInAuraRange). Otherwise
//  the cast is queued -- command 6 for the left button, 13 for the right, both when both are set (PLAYER_SetQueuedCommand) -- the distance past
//  the range is asserted positive ("nDR > 0", 1352) and the distance nonzero ("nDist", 1353), and the player walks to the point that far short
//  of the target, interpolated from its own position:
//
//      x = playerX + (targetX - playerX) * (nDistance - nRange) / nDistance
//
//  and y alike, both signed, y first; PLAYER_SendMoveToReachablePoint sends it.
//
//  Any other skill: range class 1 (h2h) or 4 (loc) walks to the unit (the record's nMoveToUnitCmd) and queues the cast as above (6, 13) -- with
//  STAND STILL it is cast at the point instead (bOnUnit FALSE); any other class is cast on the unit.
void __fastcall PLAYER_DispatchSkillClick(D2ClientClickStrc *pClick, D2C_UnitTypes nTargetType, uint32_t dwTargetGuid)
{
    D2UnitStrc *pFound;
    D2UnitStrc *pTarget;
    D2SkillsTxt *pSkillsTxt;
    int32_t nRangeClass;
    int32_t nRangeBonus;
    int32_t nRange;
    int32_t nDistance;
    int32_t nApproach;
    int32_t nPlayerX;
    int32_t nPlayerY;
    int32_t nDeltaX;
    int32_t nTargetY;
    int32_t nX;
    int32_t nY;

    if (!PLAYER_CanUseSkill(pClick->pUnit, &pClick->pSkill, NULL))
        return;

    nRangeClass = D2COMMON_11016(pClick->pUnit, pClick->pSkill);
    pFound = CUNIT_FindUnitByGuidAndType(dwTargetGuid, nTargetType);
    if (!pFound)
        return;

    nRangeBonus = D2COMMON_UNITS_IsInMovingModeEx(pFound);
    if (D2COMMON_UNITS_IsInMeleeRange(pClick->pUnit, pClick->pTarget, nRangeBonus))
    {
        PLAYER_SendClickCommand(pClick, TRUE);
        return;
    }

    if (pClick->pSkill)
    {
        pSkillsTxt = D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill(pClick->pSkill);
        if (pSkillsTxt && pSkillsTxt->wSrvDoFunc == 19)
        {
            nDistance = 0;
            nRange = 0;
            pTarget = pClick->pTarget;
            if (!pTarget)
            {
                FOG_DisplayAssert("ptMouse->hSelected", szPlayerCpp, 1327);
                exit(-1);
            }
            if (!pClick->pUnit)
            {
                FOG_DisplayAssert("ptMouse->hUnit", szPlayerCpp, 1328);
                exit(-1);
            }

            if (pClick->dwClickFlags & 0x20)
            {
                PLAYER_SendClickCommand(pClick, TRUE);
                return;
            }

            if (PLAYER_IsUnitInAuraRange(pClick->pSkill, pTarget, &nRange, &nDistance))
            {
                PLAYER_SendClickCommand(pClick, TRUE);
                return;
            }

            //  BUG: the cast is queued (calls at 0x6FB4A8A3, 0x6FB4A8BF) before the walk below, and the walk goes through
            //  UNITMODE_PerformClientCommand, whose last act clears the queue, so the queue is gone once a route is found. REACHABLE.
            if (pClick->dwClickFlags & 1)
                PLAYER_SetQueuedCommand(6, pClick->pUnit, (int32_t)nTargetType, dwTargetGuid);
            if (pClick->dwClickFlags & 2)
                PLAYER_SetQueuedCommand(13, pClick->pUnit, (int32_t)nTargetType, dwTargetGuid);

            //  BUG: the asserts "nDR > 0" and "nDist" are reproduced. UNREACHABLE: "nDR > 0" can fail only without a local player (the aura test
            //  then writes nothing, both values are 0); "nDist" only for a target at distance 0 whose range formula came out negative.
            nApproach = nDistance - nRange;
            if (nApproach <= 0)
            {
                FOG_DisplayAssert("nDR > 0", szPlayerCpp, 1352);
                exit(-1);
            }
            if (!nDistance)
            {
                FOG_DisplayAssert("nDist", szPlayerCpp, 1353);
                exit(-1);
            }

            nPlayerX = UNITS_GetXPosition(pClick->pUnit);
            nPlayerY = UNITS_GetYPosition(pClick->pUnit);
            nDeltaX = UNITS_GetXPosition(pClick->pTarget) - nPlayerX;
            nTargetY = UNITS_GetYPosition(pClick->pTarget);
            nY = nPlayerY + (nTargetY - nPlayerY) * nApproach / nDistance;
            nX = nPlayerX + nDeltaX * nApproach / nDistance;
            PLAYER_SendMoveToReachablePoint(pClick->nMoveToPointCmd, pClick->pUnit, nX, nY);
            return;
        }
    }

    if (nRangeClass != 1 && nRangeClass != 4)
    {
        PLAYER_SendClickCommand(pClick, TRUE);
        return;
    }

    if (pClick->dwClickFlags & 0x20)
    {
        PLAYER_SendClickCommand(pClick, FALSE);
        return;
    }

    UNITMODE_PerformClientCommand(pClick->nMoveToUnitCmd, pClick->pUnit, (int32_t)nTargetType, (int32_t)dwTargetGuid, 0);

    if (pClick->dwClickFlags & 1)
        PLAYER_SetQueuedCommand(6, pClick->pUnit, (int32_t)nTargetType, dwTargetGuid);

    if (pClick->dwClickFlags & 2)
        PLAYER_SetQueuedCommand(13, pClick->pUnit, (int32_t)nTargetType, dwTargetGuid);
}

//1.10f: D2Client.0x6FB4AA50
//  Interact with the clicked unit, or walk to it and queue the interaction: pClick in ecx, nTargetType in edx, nTargetGuid on the stack. The
//  interaction is command 19, Interact; the walk is the record's nMoveToUnitCmd; the queue is 19 on the same type and GUID
//  (PLAYER_SetQueuedCommand). The original inlines PLAYER_SetQueuedCommand, PLAYER_GetQueuedCommand, PLAYER_SendMoveToReachablePoint,
//  UNITS_GetXPosition, UNITS_GetYPosition, DATATBLS_GetMonStatsTxtRecord and NPCMENU_MonsterSetDataFlag here. The switch is on the record's
//  target's type (unsigned bound):
//
//    PLAYER   D2Common's #10399 at most 4 and not blocked (COLLIDE_MASK_PLAYER_FLYING): interact; else walk and queue.
//    OBJECT   not on a held click (8). Its objects.txt row is looked up and the answer dropped. A town portal cannot be operated while the
//             local player is STATE_JUST_PORTALED. In interact range (UNITS_IsObjectInInteractRange) and not blocked: if it can be operated,
//             interact and suppress the held click (GAME_SuppressHeldClick). Else -- unless this is the click already queued, a 19 on the same
//             type and GUID, which returns -- walk to its position (the one arm that sends a point) and, if it can be operated, queue and
//             suppress.
//    ITEM     not on a held click. #10399 at most 4 and not blocked: interact and suppress. Else, unless already queued: walk, queue, suppress.
//    TILE     (a warp) once per 500 ms: GAME_GetFrameTick answers the frame's GetTickCount(), compared unsigned, and the throttle is re-armed
//             at +500 after either arm. #10399 at most 4: interact; else walk and queue.
//    MONSTER  an interact distance of 2 when the monstats.txt row has interact, else 5; then the common tail.
//    any other type, and no target: 5, and the common tail.
//
//  The common tail. A monster whose row has both interact and npc -- a townsperson -- is stopped (D2Common #10153 on its path), its type, GUID
//  and position go to the server in client packet 59h (MSG_SendCmd4Dwords), bit 0 is set in the +28h word of its monster data
//  (NPCMENU_MonsterSetDataFlag) and its mode is MONMODE_NEUTRAL. Then #10399 within the distance: interact; else walk and queue. Neither the
//  tail nor the PLAYER and TILE arms suppress the held click.
//
//  #10399 is compared signed here and unsigned in PLAYER_DispatchClickAction's player arm: its -1 is close here and far there.
//
//  BUG: with no target, #10399 is handed the NULL target (0x6FB4B001). UNREACHABLE: both callers pass a resolved target.
void __fastcall PLAYER_InteractOrApproachTarget(D2ClientClickStrc *pClick, int32_t nTargetType, int32_t nTargetGuid)
{
    D2UnitStrc *pLocalPlayer;
    D2UnitStrc *pTarget;
    D2UnitStrc *pQueuePlayer;
    D2MonStatsTxt *pMonStatsTxt;
    uint32_t nThrottleTick;
    int32_t nMaxInteractDist;
    int32_t nQueuedType;
    int32_t nQueuedGuid;
    int32_t nTargetX;
    int32_t nTargetY;
    BOOL bCanOperate;

    pLocalPlayer = CUNIT_GetPlayer();
    pTarget = pClick->pTarget;
    if (!pTarget)
    {
        nMaxInteractDist = 5;
    }
    else
    {
        switch (pTarget->dwUnitType)
        {
        case UNIT_PLAYER:
            if (D2COMMON_10399(pTarget, pClick->pUnit) <= 4 && !D2COMMON_UNITS_TestCollisionBetweenInteractingUnits(pClick->pUnit, pClick->pTarget, COLLIDE_MASK_PLAYER_FLYING))
            {
                UNITMODE_PerformClientCommand(19, pClick->pUnit, nTargetType, nTargetGuid, 0);
                return;
            }

            UNITMODE_PerformClientCommand(pClick->nMoveToUnitCmd, pClick->pUnit, nTargetType, nTargetGuid, 0);

            PLAYER_SetQueuedCommand(19, pClick->pUnit, nTargetType, (D2UnitGUID)nTargetGuid);
            return;

        case UNIT_MONSTER:
            pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pTarget->dwClassId);
            if (pMonStatsTxt && (pMonStatsTxt->nMonStatsFlags[MONSTATSFLAGINDEX_INTERACT >> 3] & (uint8_t)gdwBitMasks[MONSTATSFLAGINDEX_INTERACT & 7]))
            {
                nMaxInteractDist = 2;
            }
            else
            {
                nMaxInteractDist = 5;
            }
            break;

        case UNIT_OBJECT:
            if (pClick->dwClickFlags & 8)
                return;

            D2COMMON_DATATBLS_GetObjectsTxtRecord(pTarget ? pTarget->dwClassId : -1);
            bCanOperate = TRUE;
            if (pClick->pTarget && pClick->pTarget->dwClassId == OBJECT_TOWN_PORTAL && D2COMMON_STATES_CheckState(pLocalPlayer, STATE_JUST_PORTALED))
            {
                bCanOperate = FALSE;
            }

            if (D2COMMON_UNITS_IsObjectInInteractRange(pClick->pUnit, pClick->pTarget) && !D2COMMON_UNITS_TestCollisionBetweenInteractingUnits(pClick->pUnit, pClick->pTarget, COLLIDE_MASK_PLAYER_FLYING))
            {
                if (!bCanOperate)
                    return;
                UNITMODE_PerformClientCommand(19, pClick->pUnit, nTargetType, nTargetGuid, 0);
                GAME_SuppressHeldClick();
                return;
            }

            pQueuePlayer = CUNIT_GetPlayer();
            if (PLAYER_GetQueuedCommand(pQueuePlayer) == 19)
            {
                nQueuedType = PLAYER_GetQueuedTargetType(pQueuePlayer);
                nQueuedGuid = PLAYER_GetQueuedTargetGuid(pQueuePlayer);
                if (nQueuedType == nTargetType && nQueuedGuid == nTargetGuid)
                    return;
            }

            nTargetY = UNITS_GetYPosition(pClick->pTarget);

            nTargetX = UNITS_GetXPosition(pClick->pTarget);

            PLAYER_SendMoveToReachablePoint(pClick->nMoveToPointCmd, pClick->pUnit, nTargetX, nTargetY);

            //  The frame's copy of bCanOperate (0x6FB4ABF6), the same value.
            if (!bCanOperate)
                return;

            PLAYER_SetQueuedCommand(19, pClick->pUnit, nTargetType, (D2UnitGUID)nTargetGuid);
            GAME_SuppressHeldClick();
            return;

        case UNIT_ITEM:
            if (pClick->dwClickFlags & 8)
                return;

            if (D2COMMON_10399(pTarget, pClick->pUnit) <= 4 && !D2COMMON_UNITS_TestCollisionBetweenInteractingUnits(pClick->pUnit, pClick->pTarget, COLLIDE_MASK_PLAYER_FLYING))
            {
                UNITMODE_PerformClientCommand(19, pClick->pUnit, nTargetType, nTargetGuid, 0);
                GAME_SuppressHeldClick();
                return;
            }

            pQueuePlayer = CUNIT_GetPlayer();
            if (PLAYER_GetQueuedCommand(pQueuePlayer) == 19)
            {
                nQueuedType = PLAYER_GetQueuedTargetType(pQueuePlayer);
                nQueuedGuid = PLAYER_GetQueuedTargetGuid(pQueuePlayer);
                if (nQueuedType == nTargetType && nQueuedGuid == nTargetGuid)
                    return;
            }

            UNITMODE_PerformClientCommand(pClick->nMoveToUnitCmd, pClick->pUnit, nTargetType, nTargetGuid, 0);

            PLAYER_SetQueuedCommand(19, pClick->pUnit, nTargetType, (D2UnitGUID)nTargetGuid);
            GAME_SuppressHeldClick();
            return;

        case UNIT_TILE:
            nThrottleTick = GAME_GetInteractThrottle();
            if (GAME_GetFrameTick() < nThrottleTick)
                return;

            if (D2COMMON_10399(pClick->pTarget, pClick->pUnit) <= 4)
            {
                UNITMODE_PerformClientCommand(19, pClick->pUnit, nTargetType, nTargetGuid, 0);
            }
            else
            {
                UNITMODE_PerformClientCommand(pClick->nMoveToUnitCmd, pClick->pUnit, nTargetType, nTargetGuid, 0);

                PLAYER_SetQueuedCommand(19, pClick->pUnit, nTargetType, (D2UnitGUID)nTargetGuid);
            }
            GAME_SetInteractThrottle(GAME_GetFrameTick() + 500);
            return;

        default:
            nMaxInteractDist = 5;
            break;
        }
    }

    //  The target tested twice more before the lookup (0x6FB4AED8, 0x6FB4AEE9), the second one dead.
    if (pTarget && pTarget->dwUnitType == UNIT_MONSTER)
    {
        pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pTarget->dwClassId);
        if (pMonStatsTxt && ((uint32_t)pMonStatsTxt->nMonStatsFlags[MONSTATSFLAGINDEX_INTERACT >> 3] & gdwBitMasks[MONSTATSFLAGINDEX_INTERACT & 7]) && ((uint32_t)pMonStatsTxt->nMonStatsFlags[MONSTATSFLAGINDEX_NPC >> 3]
                & gdwBitMasks[MONSTATSFLAGINDEX_NPC & 7]))
        {
            if (pTarget->pDynamicPath)
                D2COMMON_10153(pTarget->pDynamicPath);

            nTargetY = UNITS_GetYPosition(pClick->pTarget);

            nTargetX = UNITS_GetXPosition(pClick->pTarget);

            MSG_SendCmd4Dwords(0x59, pClick->pTarget ? (int32_t)pClick->pTarget->dwUnitType : UNIT_TYPES_COUNT, pClick->pTarget ? (int32_t)pClick->pTarget->dwUnitId : -1, nTargetX, nTargetY);

            NPCMENU_MonsterSetDataFlag(pClick->pTarget, 1, TRUE);
            UNITMODE_SetAnimMode(pClick->pTarget, MONMODE_NEUTRAL);
        }
    }

    if (D2COMMON_10399(pClick->pTarget, pClick->pUnit) <= nMaxInteractDist)
    {
        UNITMODE_PerformClientCommand(19, pClick->pUnit, nTargetType, nTargetGuid, 0);
        return;
    }

    UNITMODE_PerformClientCommand(pClick->nMoveToUnitCmd, pClick->pUnit, nTargetType, nTargetGuid, 0);

    PLAYER_SetQueuedCommand(19, pClick->pUnit, nTargetType, (D2UnitGUID)nTargetGuid);
}

//1.10f: D2Client.0x6FB4B0A0
//  A click's point pushed out to a minimum range, and a held move throttled: pUnit in ecx, pClick in edx. Two calls, both
//  PLAYER_DispatchClickAction's, each in front of a move that 0 does not send. The original inlines UNITS_GetXPosition and UNITS_GetYPosition
//  here.
//
//  The offset from the unit's position to the record's point is taken signed. 256 or more on either axis answers 0, and so does the point
//  under the unit, both offsets under 1. The distance is DFLOOR_APPROX_Distance's, floored at 1. The minimum range comes from the flag word: 3;
//  5 with both press (4) and RUN (40h); else held (8), 4 -- or 5 with RUN, the bit taken out of the low byte with shr; else release (10h), 0. A
//  distance under it (signed) scales both offsets by range / distance on the x87 stack, each product truncated by __ftol: a point too close is
//  moved OUT to the range. The point is written back whether it moved or not (the offsets added back, then both stored).
//
//  The throttle: the frame count (GAME_GetFrameCount) is stamped into gnLastClickRepeatFrame and the answer is 1 -- except for a PLAYER on a
//  held click in mode 2, 3 or 6 (PLRMODE_WALK, RUN, TOWNWALK; unsigned compares) fewer than 7 frames after the last stamp, the difference taken
//  unsigned, which answers 0 and stamps nothing. A held click while standing is never throttled.
int32_t __fastcall PLAYER_ClampClickTargetAndThrottle(D2UnitStrc *pUnit, D2ClientClickStrc *pClick)
{
    uint32_t nClickFlags;
    int32_t nUnitX;
    int32_t nUnitY;
    int32_t nDX;
    int32_t nDY;
    int32_t nDistance;
    int32_t nMinRange;
    double fScale;
    uint32_t nFrame;

    nUnitX = UNITS_GetXPosition(pUnit);

    nUnitY = UNITS_GetYPosition(pUnit);

    nDX = pClick->nX - nUnitX;
    nDY = pClick->nY - nUnitY;
    if ((nDX < 0 ? -nDX : nDX) >= 256)
        return 0;
    if ((nDY < 0 ? -nDY : nDY) >= 256)
        return 0;
    if ((nDX < 0 ? -nDX : nDX) < 1 && (nDY < 0 ? -nDY : nDY) < 1)
        return 0;

    nDistance = (int32_t)DFLOOR_APPROX_Distance(nDX, nDY);
    if (nDistance <= 1)
        nDistance = 1;

    nClickFlags = pClick->dwClickFlags;
    nMinRange = 3;
    if ((nClickFlags & 4) && (nClickFlags & 0x40))
        nMinRange = 5;
    else if (nClickFlags & 8)
        nMinRange = (int32_t)((((nClickFlags & 0xFF) >> 6) & 1) | 4);
    else if (nClickFlags & 0x10)
        nMinRange = 0;

    if (nDistance < nMinRange)
    {
        fScale = (double)nMinRange / (double)nDistance;
        nDX = (int32_t)((double)nDX * fScale);
        nDY = (int32_t)((double)nDY * fScale);
    }

    pClick->nX = nDX + nUnitX;
    pClick->nY = nDY + nUnitY;

    nFrame = GAME_GetFrameCount();
    if (pUnit->dwUnitType == UNIT_PLAYER && (pClick->dwClickFlags & 8) && pUnit->dwAnimMode >= PLRMODE_WALK && (pUnit->dwAnimMode <= PLRMODE_RUN || pUnit->dwAnimMode == PLRMODE_TOWNWALK) && nFrame - gnLastClickRepeatFrame < 7)
    {
        return 0;
    }

    gnLastClickRepeatFrame = nFrame;
    return 1;
}

//1.10f: D2Client.0x6FB4B260
//  May this click go on: pClick in ecx -- the gate of the button's press, hold and release. PLAYER_OnMouseClick puts it in front of arms 0, 1
//  and 3 and PLAYER_IssueRightSkillCommand in front of arm 4; the two releases reach it through PLAYER_ReleaseClickAndResetHireling, which
//  drops the answer. It keeps the phase of the press in gnClickPressPhase, which nothing else touches:
//
//    release (10h)  the phase back to 0; FALSE
//    press (4)      the phase 1, or 2 when the record has a target; TRUE
//    neither        TRUE
//    held (8)       FALSE with no press recorded; TRUE with no target; with one, FALSE unless it is a player, a monster or a tile (type 0, 1
//                   or 5, the first test signed), then FALSE while the clicking unit moves (mode 2, 3 or 6), TRUE when it stands or there
//                   is none.
//
//  Only a nonzero phase is ever tested: 1 and 2 are told apart by nothing. So a held button repeats a click only inside a press, on a unit of
//  those three types, and while the player stands.
BOOL __fastcall PLAYER_ShouldHandleClick(D2ClientClickStrc *pClick)
{
    uint32_t nClickFlags;
    int32_t nTargetType;
    uint32_t nAnimMode;

    nClickFlags = pClick->dwClickFlags;
    if (nClickFlags & 0x10)
    {
        gnClickPressPhase = 0;
        return FALSE;
    }

    if (nClickFlags & 4)
    {
        gnClickPressPhase = (pClick->pTarget != NULL) + 1;
        return TRUE;
    }

    if (!(nClickFlags & 8))
        return TRUE;

    if (!gnClickPressPhase)
        return FALSE;

    if (!pClick->pTarget)
        return TRUE;

    nTargetType = (int32_t)pClick->pTarget->dwUnitType;
    //  Signed (`jl` / `jle`, 0x6FB4B2A8, 0x6FB4B2AD), so the enumerators are cast: D2C_UnitTypes' base is uint32_t.
    if (nTargetType < 0 || (nTargetType > (int32_t)UNIT_MONSTER && nTargetType != (int32_t)UNIT_TILE))
        return FALSE;

    if (!pClick->pUnit)
        return TRUE;

    nAnimMode = pClick->pUnit->dwAnimMode;
    if (nAnimMode == PLRMODE_WALK || nAnimMode == PLRMODE_RUN || nAnimMode == PLRMODE_TOWNWALK)
        return FALSE;
    return TRUE;
}

//1.10f: D2Client.0x6FB4B2D0
//  The right button held: pClick in ecx -- arm 4 of PLAYER_OnMouseClick (0Ah, right and held), whose answer it becomes; it answers 1 on every
//  path. PLAYER_ShouldHandleClick first, then the right skill into the record (+1Ch) and CUNIT_CanPlayerIssueCommand; a skill whose skills.txt
//  range is 1 sets 80h unless the click runs (40h), and the click is dispatched (PLAYER_DispatchClickAction). Arm 3's steps, with arm 3's 80h
//  test; 80h has no reader. The original inlines DATATBLS_GetSkillsTxtRecordById here.
int32_t __fastcall PLAYER_IssueRightSkillCommand(D2ClientClickStrc *pClick)
{
    D2SkillsTxt *pSkillsTxt;

    if (!PLAYER_ShouldHandleClick(pClick))
        return 1;

    pClick->pSkill = D2COMMON_UNITS_GetRightSkill(pClick->pUnit);
    if (!CUNIT_CanPlayerIssueCommand(pClick->pUnit, pClick->pSkill))
        return 1;

    //  BUG: the row is read at +14h untested (movsx, 0x6FB4B332) after a lookup that answers NULL for an id out of range -- as in arm 3 of
    //  PLAYER_OnMouseClick. UNREACHABLE.
    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(D2COMMON_SKILLS_GetSkillIdFromSkill(pClick->pSkill, szPlayerCpp, 2557));
    if ((int8_t)pSkillsTxt->nRange == 1 && !(pClick->dwClickFlags & 0x40))
        pClick->dwClickFlags |= 0x80;

    PLAYER_DispatchClickAction(pClick);
    return 1;
}

//1.10f: D2Client.0x6FB4B360
//  A button released: pClick in ecx; answers 0. One call, arm 5 of PLAYER_OnMouseClick (12h), whose answer it becomes; the original inlines it
//  in arm 2 (11h). Both pass a release, so PLAYER_ShouldHandleClick takes its first arm -- the press phase back to 0 -- and its answer is
//  dropped. Then the clicking unit's hireling, pet kind 7 (ROSTERPETS_GetAssociatedUnitGUID, bIncludeInactive 0), found as a monster: a
//  display state of 2 is set back to 0 (SELECT_RosterPetsSetHirelingDisplayState).
int32_t __fastcall PLAYER_ReleaseClickAndResetHireling(D2ClientClickStrc *pClick)
{
    int32_t nHirelingGuid;
    D2UnitStrc *pHireling;

    PLAYER_ShouldHandleClick(pClick);
    nHirelingGuid = ROSTERPETS_GetAssociatedUnitGUID(pClick->pUnit, 7, 0);
    if (nHirelingGuid != -1)
    {
        pHireling = CUNIT_FindUnitByGuidAndType((uint32_t)nHirelingGuid, UNIT_MONSTER);
        //  BUG: ROSTERPETS_GetPetInfo's record is read untested (0x6FB4B399), and the display state is only ever 0 or 1, so the test for 2 is
        //  never true. UNREACHABLE.
        if (pHireling && ROSTERPETS_GetPetInfo((uint32_t)nHirelingGuid)->nDisplayState == 2)
            SELECT_RosterPetsSetHirelingDisplayState(pClick->pUnit, pHireling, 0);
    }
    return 0;
}

//1.10f: D2Client.0x6FB4B3C0
//  A player's body assigned: the owner dies, the body takes its place, the equipment leaves the owner. ptCmd in ecx, server packet 74h. One call,
//  SCmd.cpp's SCMD_HandleSrv74_AssignCorpse, with its twelve-byte stack copy. The original inlines UNITS_GetYPosition and UNITS_GetXPosition here.
//
//  The packet: +2 is the player whose id the body's inventory keeps as its owner (INVENTORY_GetOwnerId), +6 the body, and +1 the body's
//  UNITFLAGEX_ISCORPSE (bIsCorpse; the members are bIsCorpse, nOwnerGUID and nBodyGUID). The body is a unit of its own.
//
//  A clear flag returns. The owner is found as a PLAYER (none: return) and, unless CUNIT_IsUnitDead says so already, set to mode 0, PLRMODE_DEATH
//  (UNITMODE_SetAnimMode). For the LOCAL player only, the body is found as a PLAYER too (none: return -- before the equipment), its path given
//  move-test collision mask 0 and collision pattern 5 (COLLISION_PATTERN_SMALL_NO_PRESENCE), placed at the owner's position in the owner's
//  room by D2Common #10229, which is asserted, "fRet" (line 2746), and turned to the owner's direction.
//
//  Then, for EVERY owner, its inventory is walked -- the next item read before the current one is removed -- and each item on node page 3,
//  NODEPAGE_EQUIP, is taken off the OWNER: INVENTORY_UnitIsItem, the weapon GUID updated (#10290), every loaded-mode flag of the owner cleared
//  (GFX_ClearUnitModesLoaded, which the item's component only gates: below 16), the removal asserted ("hRemove", 2760; "hRemove == hItem",
//  2761), the item detached from the owner (ITEM_DetachItemFromUnit), D2Common's #10254 on its body location, the owner's animation rebuilt,
//  and the item unit removed from the client. The component passed is the item's own, except for an item in either arm (body location 5 or 4, a
//  byte) while both arms hold an item, neither two-handed, and it is not INVENTORY_GetLeftHandWeapon's: then COMPOSIT_LEFTHAND.
void __fastcall PLAYER_AssignCorpse(D2GSPacketSrv74 *ptCmd)
{
    D2UnitStrc *pOwner;
    D2UnitStrc *pBody;
    D2DynamicPathStrc *pBodyPath;
    D2InventoryStrc *pInventory;
    D2UnitStrc *pItem;
    D2UnitStrc *pNextItem;
    D2UnitStrc *pInvItem;
    D2UnitStrc *pRightArmItem;
    D2UnitStrc *pLeftArmItem;
    D2UnitStrc *pRemoved;
    uint8_t nBodyLoc;
    uint8_t nComponent;
    BOOL bLeftHand;
    int32_t nOwnerX;
    int32_t nOwnerY;

    if (!ptCmd->bIsCorpse)
        return;

    pOwner = CUNIT_FindUnitByGuidAndType((uint32_t)ptCmd->nOwnerGUID, UNIT_PLAYER);
    if (!pOwner)
        return;

    if (!CUNIT_IsUnitDead(pOwner))
        UNITMODE_SetAnimMode(pOwner, PLRMODE_DEATH);

    if (pOwner == CUNIT_GetPlayer())
    {
        pBody = CUNIT_FindUnitByGuidAndType((uint32_t)ptCmd->nBodyGUID, UNIT_PLAYER);
        if (!pBody)
            return;

        pBodyPath = pBody->pDynamicPath;
        D2COMMON_PATH_SetMoveTestCollisionMask(pBodyPath, 0);
        D2COMMON_PATH_SetUnitCollisionPattern(pBody, COLLISION_PATTERN_SMALL_NO_PRESENCE);

        nOwnerY = UNITS_GetYPosition(pOwner);

        nOwnerX = UNITS_GetXPosition(pOwner);

        if (!D2COMMON_10229(pBodyPath, pBody, D2COMMON_UNITS_GetRoom(pOwner), (uint32_t)nOwnerX, (uint32_t)nOwnerY))
        {
            FOG_DisplayAssert("fRet", szPlayerCpp, 2746);
            exit(-1);
        }

        D2COMMON_PATH_SetDirection(pBody->pDynamicPath, D2COMMON_PATH_GetDirection(pOwner->pDynamicPath));
    }

    pInventory = pOwner->pInventory;
    pItem = D2COMMON_INVENTORY_GetFirstItem(pInventory);
    while (pItem)
    {
        pNextItem = D2COMMON_INVENTORY_GetNextItem(pItem);
        if (D2COMMON_INVENTORY_GetItemNodePage(pItem) == NODEPAGE_EQUIP)
        {
            pInvItem = D2COMMON_INVENTORY_UnitIsItem(pItem);
            D2COMMON_INVENTORY_UpdateWeaponGUIDOnRemoval(pInventory, pInvItem);

            bLeftHand = FALSE;
            nBodyLoc = D2COMMON_ITEMS_GetBodyLocation(pInvItem);
            if (nBodyLoc == BODYLOC_LARM || nBodyLoc == BODYLOC_RARM)
            {
                pRightArmItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pOwner->pInventory, BODYLOC_RARM);
                pLeftArmItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pOwner->pInventory, BODYLOC_LARM);
                if (pRightArmItem && pLeftArmItem && !D2COMMON_ITEMS_CheckWeaponIfTwoHanded(pRightArmItem) && !D2COMMON_ITEMS_CheckWeaponIfTwoHanded(pLeftArmItem) && pInvItem != D2COMMON_INVENTORY_GetLeftHandWeapon(pOwner->pInventory))
                {
                    bLeftHand = TRUE;
                }
            }

            if (bLeftHand)
                nComponent = COMPOSIT_LEFTHAND;
            else
                nComponent = D2COMMON_ITEMS_GetComponent(pInvItem);
            GFX_ClearUnitModesLoaded(pOwner, (D2Composits)nComponent);

            pRemoved = D2COMMON_INVENTORY_RemoveItemFromInventory(pInventory, pInvItem);
            if (!pRemoved)
            {
                FOG_DisplayAssert("hRemove", szPlayerCpp, 2760);
                exit(-1);
            }
            if (pRemoved != pInvItem)
            {
                FOG_DisplayAssert("hRemove == hItem", szPlayerCpp, 2761);
                exit(-1);
            }

            ITEM_DetachItemFromUnit(pInvItem, pOwner, 0, 0);
            D2COMMON_10254_Return0((int32_t)(intptr_t)pInventory, D2COMMON_ITEMS_GetBodyLocation(pInvItem));
            GFX_RebuildUnitAnimForMode(pOwner, 0);

            //  BUG: pInvItem is tested for NULL only here (0x6FB4B5C5), after D2Common's #10290, ITEMS_GetBodyLocation (twice), ITEMS_GetComponent
            //  on one path, RemoveItemFromInventory and ITEM_DetachItemFromUnit have been handed it. UNREACHABLE: INVENTORY_UnitIsItem answers
            //  NULL only for a unit that is not an item.
            if (pInvItem)
                CUNIT_RemoveUnitById((int32_t)pInvItem->dwUnitId, pInvItem->dwUnitType);
            else
                CUNIT_RemoveUnitById(-1, UNIT_TYPES_COUNT);
        }
        pItem = pNextItem;
    }
}

//1.10f: D2Client.0x6FB4B640
//  Another player put into a room at a point, its move resumed: pUnit in ecx, pRoom in edx, nX and nY on the stack. One call,
//  UNITMODE_SyncUnitPosition's, for a player that is not the local one.
//
//  A unit in mode 2, 3 or 6 -- the mode read once and compared signed -- is moving, and its path's target unit is kept (CUNIT_GetPathTargetUnit),
//  or with none its target point (SKILLS_GetPathTargetPos). The path read at entry is handed to D2Common #10230 with the room and the point, and
//  the move is issued again through PLAYER_DoClientAction (bInterruptKnockback FALSE) with a seven-dword parameter block, zeroed: the target's
//  type and GUID with CLTACTION_WALKTOUNIT, or CLTACTION_RUNTOUNIT in mode 3; the point with CLTACTION_WALKTOXY, or CLTACTION_RUNTOXY in mode 3.
//  Modes 2 and 6 both walk.
//
//  nY is __int16: the original forwards the caller's whole stack dword (0x6FB4B694), and #10230, which reads it, keeps 16 bits. Ours forwards
//  those 16 bits sign-extended.
void __fastcall PLAYER_SetUnitRoomAndPos(D2UnitStrc *pUnit, D2ActiveRoomStrc *pRoom, int32_t nX, int16_t nY)
{
    BOOL bWasMoving;
    int32_t nAnimMode;
    D2DynamicPathStrc *pPath;
    D2UnitStrc *pPathTarget;
    D2C_ClientActions nAction;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t aActionParams[7];

    bWasMoving = FALSE;
    nAnimMode = (int32_t)pUnit->dwAnimMode;
    pPath = pUnit->pDynamicPath;
    pPathTarget = NULL;
    nTargetX = 0;
    nTargetY = 0;
    //  Signed, so the enumerators are cast: D2C_PlayerModes' base is uint32_t.
    if (nAnimMode >= (int32_t)PLRMODE_WALK && (nAnimMode <= (int32_t)PLRMODE_RUN || nAnimMode == (int32_t)PLRMODE_TOWNWALK))
    {
        bWasMoving = TRUE;
        pPathTarget = CUNIT_GetPathTargetUnit(pUnit);
        if (!pPathTarget)
            SKILLS_GetPathTargetPos(pUnit, &nTargetX, &nTargetY);
    }

    D2COMMON_10230(pPath, (int32_t)(intptr_t)pUnit, pRoom, (uint32_t)nX, nY);
    if (!bWasMoving)
        return;

    memset(aActionParams, 0, sizeof(aActionParams));
    if (pPathTarget)
    {
        aActionParams[0] = (int32_t)pPathTarget->dwUnitType;
        aActionParams[1] = (int32_t)pPathTarget->dwUnitId;
        nAction = (nAnimMode != (int32_t)PLRMODE_RUN) ? CLTACTION_WALKTOUNIT : CLTACTION_RUNTOUNIT;
    }
    else
    {
        aActionParams[0] = nTargetX;
        aActionParams[1] = nTargetY;
        nAction = (nAnimMode != (int32_t)PLRMODE_RUN) ? CLTACTION_WALKTOXY : CLTACTION_RUNTOXY;
    }
    PLAYER_DoClientAction(nAction, pUnit, aActionParams, FALSE);
}


//1.10f: D2Client.0x6FB4B780
//  The queued command's target type: pUnit in ecx -- unk0xA8[44] of a PLAYER's data (+158h), read through one load of pPlayerData; 0 for NULL,
//  any other type or no data. With PLAYER_GetQueuedTargetGuid below it answers "is this the click already queued"; the original inlines it in
//  UNITMODE_RunQueuedCommand.
int32_t __fastcall PLAYER_GetQueuedTargetType(D2UnitStrc *pUnit)
{
    D2PlayerDataStrc *pPlayerData;

    if (pUnit && pUnit->dwUnitType == UNIT_PLAYER && (pPlayerData = pUnit->pPlayerData) != NULL)
        return (int32_t)pPlayerData->unk0xA8[44];
    return 0;
}

//1.10f: D2Client.0x6FB4B7A0
//  The queued command's target GUID: pUnit in ecx -- unk0xA8[45], +15Ch -- the same shape, callers and inlined copy as
//  PLAYER_GetQueuedTargetType above.
int32_t __fastcall PLAYER_GetQueuedTargetGuid(D2UnitStrc *pUnit)
{
    D2PlayerDataStrc *pPlayerData;

    if (pUnit && pUnit->dwUnitType == UNIT_PLAYER && (pPlayerData = pUnit->pPlayerData) != NULL)
        return (int32_t)pPlayerData->unk0xA8[45];
    return 0;
}

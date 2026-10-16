#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"

//  States.cpp -- the States.txt state handlers of D2Client.dll 1.10: the `setfunc` and `remfunc` functions of gpStateSetFuncs and gpStateRemFuncs
//  (STATES_SetFuncN and STATES_RemFuncN, N the table slot, with a purpose after the number; a function in both tables carries both numbers), and
//  the functions beside them that apply a state's overlays or colour shift and turn a state on from the server.
//  These bytes are the original's UNIT\PlrSkills.cpp (the path string at 0x6FB93FD8 is pushed by several of them); the original has no States.cpp,
//  so the position of a function here says nothing about its layout. Address range 0x6FB4BB40..0x6FB4D510.
//  Both tables are run as f(pUnit, nStateId), pUnit in ecx and the state in edx, by PLRSKILLS_StatesRunStateSetFunc and
//  PLRSKILLS_StatesRunStateRemFunc. The set table's slots 1..19 are filled, the rem table's slots 1..12.

//  The original's source path literal for UNIT\PlrSkills.cpp (0x6FB93FD8), passed as __FILE__ by the functions below.
static const char szPlrSkillsCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UNIT\\PlrSkills.cpp";

static_assert(offsetof(D2StatesTxt, wOverlay) == 0x02 && offsetof(D2StatesTxt, wCastOverlay) == 0x0A
                  && offsetof(D2StatesTxt, wPrgOverlay) == 0x0E && offsetof(D2StatesTxt, nStateFlags) == 0x10
                  && offsetof(D2StatesTxt, wStat) == 0x18,
              "the setfunc and remfunc handlers: `mov cx, [eax+2]`..`[eax+8]` (0x6FB4BFAB..0x6FB4BFF3), `mov cx, "
              "[eax+0Ah]` (0x6FB4BF9A), `mov ax, [esi+0Eh]` (0x6FB4BB83), `mov dl, [esi+10h]` (0x6FB4BB7B), `mov ax, "
              "[esi+18h]` (0x6FB4BB97)");
static_assert(offsetof(D2DataTablesStrc, pSkillsTxt) == 0xB98 && offsetof(D2SkillsTxt, dwAuraLenCalc) == 0x60,
              "STATES_SetFunc2_CloakOfShadows: `mov ecx, [ecx+0B98h]` (0x6FB4BCD9), `mov edx, [ecx+60h]` (0x6FB4BCEE)");
static_assert(STAT_BONEARMOR == 0x84 && STAT_BONEARMORMAX == 0x85 && STAT_SOURCE_UNIT_TYPE == 0x161
                  && STAT_SOURCE_UNIT_ID == 0x162 && STAT_SHORTPARAM1 == 0x163,
              "`push 84h`, `push 85h` (0x6FB4C206, 0x6FB4C215), `push 161h`, `push 162h` (0x6FB4BE34, 0x6FB4BE48), "
              "`push 163h` (0x6FB4BE99, 0x6FB4C28D)");
static_assert(MONSTER_BAALTHRONE == 543 && MONMODE_SKILL1 == 8 && OVERLAYTYPE_BONEARMOR == 7,
              "`cmp ebx, 21Fh` (0x6FB4BEC8), `mov edx, 8` (0x6FB4BEB7), `push 7` (0x6FB4C301)");


//1.10f: D2Client.0x6FB4BB40
//  States.txt setfunc 1 (gpStateSetFuncs[1]): the six `progressive_*` rows (damage, steal, other, fire, cold, lightning). The original inlines
//  DATATBLS_GetStatesTxtRecord here.
//  The charge level as an overlay. For a state with the `pgsv` flag, a `pgsvoverlay` above 0 and below the overlay count, and a `stat` from 0 up to
//  the ItemStatCost count inclusive: the three overlays from `pgsvoverlay` on are taken off the unit, and the one the stat's value picks, 1..3,
//  goes back on as a looping overlay (kind 3). The stock rows name progressive_<x>_1 there, and Overlay.txt holds _1, _2 and _3 of each
//  consecutively.
//  Every range test is signed: the id, the overlay, the stat and the level. The column is read again for every removal and for the add, as the C
//  reads it.
void __fastcall STATES_SetFunc1_Progressive(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2StatesTxt *pStatesTxt;
    int32_t i;
    int32_t nLevel;

    if ((int32_t)nStateId < 0 || (int32_t)nStateId >= sgptDataTables->nStatesTxtRecordCount)
        return;

    pStatesTxt = &sgptDataTables->pStatesTxt[nStateId];
    if (!pStatesTxt)
        return;

    //  `pgsv`: bit 4 of the first flag byte.
    if (!(pStatesTxt->nStateFlags[0] & (uint8_t)gdwBitMasks[4]))
        return;

    if ((int16_t)pStatesTxt->wPrgOverlay <= 0 || (int16_t)pStatesTxt->wPrgOverlay >= sgptDataTables->nOverlayTxtRecordCount)
    {
        return;
    }

    if ((int16_t)pStatesTxt->wStat < 0 || (int16_t)pStatesTxt->wStat > sgptDataTables->nItemStatCostTxtRecordCount)
        return;

    for (i = 0; i < 3; ++i)
        GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)((int16_t)pStatesTxt->wPrgOverlay + i));

    nLevel = D2COMMON_STATLIST_UnitGetStatValue(pUnit, (D2C_ItemStats)(int16_t)pStatesTxt->wStat, 0);
    if (nLevel > 0 && nLevel <= 3)
        GFX_AddOverlay(pUnit, (D2_OverlayIds)((int16_t)pStatesTxt->wPrgOverlay + nLevel - 1), OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
}


//1.10f: D2Client.0x6FB4BC00
//  States.txt remfunc 1 (gpStateRemFuncs[1]): the same six `progressive_*` rows. It is STATES_SetFunc1_Progressive's first half: the same four
//  tests (`pgsv`, the overlay above 0 and below the count, the stat from 0 to the count inclusive) and the three overlays from `pgsvoverlay` on
//  taken off. The original inlines DATATBLS_GetStatesTxtRecord here.
//  The stat is tested and never read: a row whose `stat` is out of range keeps its overlays. The six stock rows all name a stat, so it does not
//  bite there.
void __fastcall STATES_RemFunc1_Progressive(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2StatesTxt *pStatesTxt;
    int32_t i;

    if ((int32_t)nStateId < 0 || (int32_t)nStateId >= sgptDataTables->nStatesTxtRecordCount)
        return;

    pStatesTxt = &sgptDataTables->pStatesTxt[nStateId];
    if (!pStatesTxt)
        return;

    if (!(pStatesTxt->nStateFlags[0] & (uint8_t)gdwBitMasks[4]))
        return;

    if ((int16_t)pStatesTxt->wPrgOverlay <= 0 || (int16_t)pStatesTxt->wPrgOverlay >= sgptDataTables->nOverlayTxtRecordCount)
    {
        return;
    }

    if ((int16_t)pStatesTxt->wStat < 0 || (int16_t)pStatesTxt->wStat > sgptDataTables->nItemStatCostTxtRecordCount)
        return;

    for (i = 0; i < 3; ++i)
        GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)((int16_t)pStatesTxt->wPrgOverlay + i));
}


//1.10f: D2Client.0x6FB4BC80
//  States.txt setfunc 2: `cloak_of_shadows`. Cloak of shadows darkens the screen for the state's duration. With a room for the unit, the skill and
//  level the state carries (MODIFIERLIST_SKILL and MODIFIERLIST_LEVEL of its own stat list, stats 350 and 351) give the skill's `auralencalc`
//  (+60h) evaluated at that level, the duration in frames, and QUESTSFX_StartScreenFade is started with 5 frames to fade out, three fifths of the
//  duration held dark and two fifths to fade in, and the level of the unit's room. Both fifths are signed divisions of 3*len and 2*len (C's `/ 5`
//  on int). The original inlines DATATBLS_GetSkillsTxtRecordById here.
void __fastcall STATES_SetFunc2_CloakOfShadows(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2ActiveRoomStrc *pRoom;
    D2StatListStrc *pStatList;
    int32_t nSkillId;
    int32_t nSkillLevel;
    D2SkillsTxt *pSkillsTxt;
    int32_t nAuraLen;
    int32_t nLevelId;

    pRoom = D2COMMON_UNITS_GetRoom(pUnit);
    if (!pRoom)
        return;

    pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, nStateId);
    if (!pStatList)
        return;

    nSkillId = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_MODIFIERLIST_SKILL, 0);
    nSkillLevel = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_MODIFIERLIST_LEVEL, 0);

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return;

    nAuraLen = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwAuraLenCalc, (D2C_Skills)nSkillId, nSkillLevel);
    nLevelId = D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);
    QUESTSFX_StartScreenFade(5, 3 * nAuraLen / 5, 2 * nAuraLen / 5, nLevelId, 0);
}


//1.10f: D2Client.0x6FB4BD40
//  States.txt setfunc 3: `blaze`. Another unit's skill, synced from the state. Nothing is done for the local player. For any other unit the state's
//  stat list gives the skill and level it was applied with (MODIFIERLIST_SKILL, MODIFIERLIST_LEVEL), and the level the unit has in that skill now
//  is read: SKILLS_GetSkillById with owner GUID -1, then SKILLS_GetSkillLevel with bBonus 1, 0 with no skill. When the two differ the skill is
//  assigned again at the state's level, capped at skills.txt `maxlvl` (a signed word; 20 when the id is out of range or the column is 0 or less),
//  and whatever lies above the cap goes to SKILLS_SetSkillLevelBonus. The original inlines a skills.txt `maxlvl` read here.
//  The two SKILLS_AssignSkill calls pass the original's source path and lines 283 and 288 (push 11Bh, push 120h); the capped arm is the first in
//  the source, as the C writes it.
void __fastcall STATES_SetFunc3_Blaze(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2StatListStrc *pStatList;
    int32_t nSkillId;
    int32_t nSkillLevel;
    D2SkillStrc *pSkill;
    int32_t nAssignedSkillLevel;
    int32_t nMaxLevel;

    if (pUnit == CUNIT_GetPlayer())
        return;

    pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, nStateId);
    if (!pStatList)
        return;

    nSkillId = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_MODIFIERLIST_SKILL, 0);
    nSkillLevel = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_MODIFIERLIST_LEVEL, 0);

    pSkill = D2COMMON_SKILLS_GetSkillById(pUnit, (D2C_Skills)nSkillId, (D2UnitGUID)-1);
    nAssignedSkillLevel = pSkill ? D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill, TRUE) : 0;
    if (nAssignedSkillLevel == nSkillLevel)
        return;

    if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount || (nMaxLevel = (int16_t)sgptDataTables->pSkillsTxt[nSkillId].wMaxLvl) <= 0)
    {
        nMaxLevel = 20;
    }

    if (nSkillLevel > nMaxLevel)
    {
        D2COMMON_SKILLS_AssignSkill(pUnit, (D2C_Skills)nSkillId, nMaxLevel, FALSE, szPlrSkillsCpp, 283);
        D2COMMON_SKILLS_SetSkillLevelBonus(pUnit, nSkillId, nSkillLevel - nMaxLevel);
        return;
    }

    D2COMMON_SKILLS_AssignSkill(pUnit, (D2C_Skills)nSkillId, nSkillLevel, FALSE, szPlrSkillsCpp, 288);
}


//1.10f: D2Client.0x6FB4BE20
//  States.txt setfunc 4: `sourceunit`. The owner, back from the state. The state's stat list gives STAT_SOURCE_UNIT_TYPE (353), taken unsigned
//  below UNIT_TYPES_COUNT (6), and STAT_SOURCE_UNIT_ID (354), and D2Common's UNITS_StoreOwnerTypeAndGUID files them as the unit's owner. The rem
//  function beside it, STATES_RemFunc2_SourceUnit, clears the owner.
void __fastcall STATES_SetFunc4_SourceUnit(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2StatListStrc *pStatList;
    uint32_t nOwnerType;

    pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, nStateId);
    if (!pStatList)
        return;

    nOwnerType = (uint32_t)D2COMMON_STATLIST_GetStatValue(pStatList, STAT_SOURCE_UNIT_TYPE, 0);
    if (nOwnerType >= UNIT_TYPES_COUNT)
        return;

    D2COMMON_UNITS_StoreOwnerTypeAndGUID(pUnit, (int32_t)nOwnerType, (D2UnitGUID)D2COMMON_STATLIST_GetStatValue(pStatList, STAT_SOURCE_UNIT_ID, 0));
}


//1.10f: D2Client.0x6FB4BE60
//  States.txt remfunc 2: `sourceunit`. Calls D2Common's UNITS_StoreOwner with no owner (push 0). The state id the table passes in edx is not read.
void __fastcall STATES_RemFunc2_SourceUnit(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2COMMON_UNITS_StoreOwner(pUnit, NULL);
}


//1.10f: D2Client.0x6FB4BE70
//  States.txt setfunc 5: `changeclass`. A monster changes class. Only for a monster, with the state's stat list: STAT_SHORTPARAM1 (355) is the new
//  monstats class, MONSTER_ChangeMonsterClass makes the change, and the anim mode becomes MONMODE_SKILL1 (8) if it was that before the change (read
//  ahead of the call) and MONMODE_NEUTRAL (1) otherwise.
//  Then, for MONSTER_BAALTHRONE (543) only, the unit turns to face a point ten subtiles along Y from where it stands:
//  PATH_FaceCoords(pPath, x, y + 10, 1), with the path read before the two coordinate copies. The original inlines UNITS_GetYPosition and
//  UNITS_GetXPosition here.
void __fastcall STATES_SetFunc5_ChangeClass(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2StatListStrc *pStatList;
    int32_t nNewClassId;
    int32_t nAnimMode;
    D2DynamicPathStrc *pPath;
    int32_t nY;
    int32_t nX;

    if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER)
        return;

    pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, nStateId);
    if (!pStatList)
        return;

    nNewClassId = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_SHORTPARAM1, 0);
    nAnimMode = (int32_t)pUnit->dwAnimMode;
    MONSTER_ChangeMonsterClass(pUnit, (D2C_MonsterIds)nNewClassId);
    UNITMODE_SetAnimMode(pUnit, nAnimMode == MONMODE_SKILL1 ? MONMODE_SKILL1 : MONMODE_NEUTRAL);

    if (nNewClassId != MONSTER_BAALTHRONE)
        return;

    //  BUG: the arm for MONSTER_BAALTHRONE is dead code. UNREACHABLE: no stock server change turns a monster into MONSTER_BAALTHRONE.
    pPath = pUnit->pDynamicPath;

    nY = UNITS_GetYPosition(pUnit);

    nX = UNITS_GetXPosition(pUnit);
    D2COMMON_PATH_FaceCoords(pPath, nX, nY + 10, 1);
}


//1.10f: D2Client.0x6FB4BF60
//  States.txt remfunc 3: `bonearmor`; also called by STATES_SetFunc6_BoneArmor before it puts the layers back. Every overlay the row names comes
//  off: `castoverlay` and `overlay1..4`, which stock States.txt fills with bonearmor_cast, _front, _back, _null1 and _null2. Each column is a
//  signed word taken as -1 when negative, all five read first. For each that is not -1, the group of D2Common's DATATBLS_Get1OfNFromOverlayTxt(id)
//  rows from the id on is handed to GFX_RemoveOverlay one row at a time: three times over for the cast, front and back ids, once for the two
//  nulls, eleven sweeps, the count fetched again for each. The original inlines DATATBLS_GetStatesTxtRecord and an overlay-group removal (each
//  sweep) here.
//  The count is a signed int in every loop, where Get1OfNFromOverlayTxt is declared uint32_t; the C casts. The front id is spilled to the frame
//  because the three cast sweeps use its register.
void __fastcall STATES_RemFunc3_BoneArmor(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2StatesTxt *pStatesTxt;
    int32_t nCastOverlayId;
    int32_t nOverlay0Id;
    int32_t nOverlay1Id;
    int32_t nOverlay2Id;
    int32_t nOverlay3Id;
    int32_t nCount;
    int32_t i;

    if ((int32_t)nStateId < 0 || (int32_t)nStateId >= sgptDataTables->nStatesTxtRecordCount)
        return;

    pStatesTxt = &sgptDataTables->pStatesTxt[nStateId];
    if (!pStatesTxt)
        return;

    nCastOverlayId = (int16_t)pStatesTxt->wCastOverlay < 0 ? -1 : (int16_t)pStatesTxt->wCastOverlay;
    nOverlay0Id = (int16_t)pStatesTxt->wOverlay[0] < 0 ? -1 : (int16_t)pStatesTxt->wOverlay[0];
    nOverlay1Id = (int16_t)pStatesTxt->wOverlay[1] < 0 ? -1 : (int16_t)pStatesTxt->wOverlay[1];
    nOverlay2Id = (int16_t)pStatesTxt->wOverlay[2] < 0 ? -1 : (int16_t)pStatesTxt->wOverlay[2];
    nOverlay3Id = (int16_t)pStatesTxt->wOverlay[3] < 0 ? -1 : (int16_t)pStatesTxt->wOverlay[3];

    if (nCastOverlayId != -1)
    {
        nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nCastOverlayId);
        for (i = 0; i < nCount; ++i)
            GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nCastOverlayId + i));

        nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nCastOverlayId);
        for (i = 0; i < nCount; ++i)
            GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nCastOverlayId + i));

        nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nCastOverlayId);
        for (i = 0; i < nCount; ++i)
            GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nCastOverlayId + i));
    }

    if (nOverlay0Id != -1)
    {
        nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay0Id);
        for (i = 0; i < nCount; ++i)
            GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay0Id + i));

        nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay0Id);
        for (i = 0; i < nCount; ++i)
            GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay0Id + i));

        nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay0Id);
        for (i = 0; i < nCount; ++i)
            GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay0Id + i));
    }

    if (nOverlay1Id != -1)
    {
        nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay1Id);
        for (i = 0; i < nCount; ++i)
            GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay1Id + i));

        nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay1Id);
        for (i = 0; i < nCount; ++i)
            GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay1Id + i));

        nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay1Id);
        for (i = 0; i < nCount; ++i)
            GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay1Id + i));
    }

    if (nOverlay2Id != -1)
    {
        nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay2Id);
        for (i = 0; i < nCount; ++i)
            GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay2Id + i));
    }

    if (nOverlay3Id != -1)
    {
        nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay3Id);
        for (i = 0; i < nCount; ++i)
            GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay3Id + i));
    }
}


//1.10f: D2Client.0x6FB4C1B0
//  States.txt setfunc 6: `bonearmor`. Bone armor's layers from what is left of it. Off the state's own stat list, STAT_BONEARMOR (132) against
//  STAT_BONEARMORMAX (133): three layers at two thirds or more, two from one third, one below. `nMax / 3` is a signed division and two thirds is
//  twice that (lea eax, [edx+edx]), not 2 * nMax / 3. The old count is read from STAT_SHORTPARAM1 (355) and the new one stored there
//  (SetStatIfListIsValid) before either is compared.
//  More layers than before: STATES_RemFunc3_BoneArmor takes every overlay of the row off, then null1 (`overlay3`) goes on as OVERLAYTYPE_BONEARMOR
//  (7) for three layers, null2 (`overlay4`) as the same for two or more, and the cast overlay as OVERLAYTYPE_LOOPEX (4) with front and back
//  (`overlay1`, `overlay2`) in GFX_AddOverlay's nTargetOrStateId and nX slots; all three of those ids are tested for -1 first. Fewer: the front
//  and back groups are swept off once per layer lost (a do-while; the difference is at least 1 there). The same number: nothing.
//  The original inlines DATATBLS_GetStatesTxtRecord and an overlay-group removal here.
void __fastcall STATES_SetFunc6_BoneArmor(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2StatesTxt *pStatesTxt;
    D2StatListStrc *pStatList;
    int32_t nBoneArmor;
    int32_t nBoneArmorMax;
    int32_t nCastOverlayId;
    int32_t nOverlay0Id;
    int32_t nOverlay1Id;
    int32_t nOverlay2Id;
    int32_t nOverlay3Id;
    int32_t nOldStackCount;
    int32_t nStackCount;
    int32_t nRemoveCount;
    int32_t nCount;
    int32_t i;

    if ((int32_t)nStateId < 0 || (int32_t)nStateId >= sgptDataTables->nStatesTxtRecordCount)
        return;

    pStatesTxt = &sgptDataTables->pStatesTxt[nStateId];
    if (!pStatesTxt)
        return;

    pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, nStateId);
    if (!pStatList)
        return;

    nBoneArmor = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_BONEARMOR, 0);
    nBoneArmorMax = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_BONEARMORMAX, 0);

    nCastOverlayId = (int16_t)pStatesTxt->wCastOverlay < 0 ? -1 : (int16_t)pStatesTxt->wCastOverlay;
    nOverlay0Id = (int16_t)pStatesTxt->wOverlay[0] < 0 ? -1 : (int16_t)pStatesTxt->wOverlay[0];
    nOverlay1Id = (int16_t)pStatesTxt->wOverlay[1] < 0 ? -1 : (int16_t)pStatesTxt->wOverlay[1];
    nOverlay2Id = (int16_t)pStatesTxt->wOverlay[2] < 0 ? -1 : (int16_t)pStatesTxt->wOverlay[2];
    nOverlay3Id = (int16_t)pStatesTxt->wOverlay[3] < 0 ? -1 : (int16_t)pStatesTxt->wOverlay[3];

    nOldStackCount = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_SHORTPARAM1, 0);

    nStackCount = 3;
    if (nBoneArmor < nBoneArmorMax / 3)
        nStackCount = 2;
    if (nBoneArmor < 2 * (nBoneArmorMax / 3))
        --nStackCount;

    D2COMMON_STATLIST_SetStatIfListIsValid(pStatList, STAT_SHORTPARAM1, nStackCount, 0);

    if (nStackCount > nOldStackCount)
    {
        STATES_RemFunc3_BoneArmor(pUnit, nStateId);

        if (nOverlay2Id != -1 && nStackCount >= 3)
            GFX_AddOverlay(pUnit, (D2_OverlayIds)nOverlay2Id, OVERLAYTYPE_BONEARMOR, 0, 0, 0, 0, 0);

        if (nOverlay3Id != -1 && nStackCount >= 2)
            GFX_AddOverlay(pUnit, (D2_OverlayIds)nOverlay3Id, OVERLAYTYPE_BONEARMOR, 0, 0, 0, 0, 0);

        if (nCastOverlayId != -1 && nOverlay0Id != -1 && nOverlay1Id != -1)
            GFX_AddOverlay(pUnit, (D2_OverlayIds)nCastOverlayId, OVERLAYTYPE_LOOPEX, 0, nOverlay0Id, nOverlay1Id, 0, 0);
    }
    else if (nStackCount < nOldStackCount)
    {
        nRemoveCount = nOldStackCount - nStackCount;
        do
        {
            if (nOverlay0Id != -1)
            {
                nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay0Id);
                for (i = 0; i < nCount; ++i)
                    GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay0Id + i));
            }

            if (nOverlay1Id != -1)
            {
                nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay1Id);
                for (i = 0; i < nCount; ++i)
                    GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay1Id + i));
            }
        }
        while (--nRemoveCount);
    }
}


//1.10f: D2Client.0x6FB4C3D0
//  States.txt remfunc 5: `cyclonearmor`; its other caller is STATES_SetFunc8_CycloneArmor. Each armor layer's front and back come off. `overlay1..3`
//  hold cyclonearmor1front, 2front and 3front in stock States.txt, and in stock Overlay.txt each is followed by its `back` row. Each column is a
//  signed word taken as -1 when negative; for each that is not -1, the group of Get1OfNFromOverlayTxt(id) rows from the id on and then the group
//  from id + 1 are handed to GFX_RemoveOverlay, as in STATES_RemFunc3_BoneArmor. The original inlines DATATBLS_GetStatesTxtRecord and an
//  overlay-group removal (each sweep) here.
//  The removal has a -1 test of its own: after the front's group, id + 1 is tested against -1 before the back's (lea edi, [ebp+1] / cmp edi,
//  0FFFFFFFFh / jz at 0x6FB4C475..0x6FB4C47B, and again at 0x6FB4C4CE and 0x6FB4C527). id + 1 is -1 only for an id of -2, which the column read
//  cannot give: it is what an inlined remover testing its own argument leaves when called with id and with id + 1 inside the caller's
//  `if (id != -1)`. The C keeps the second test.
void __fastcall STATES_RemFunc5_CycloneArmor(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2StatesTxt *pStatesTxt;
    int32_t nOverlay0Id;
    int32_t nOverlay1Id;
    int32_t nOverlay2Id;
    int32_t nCount;
    int32_t i;

    if ((int32_t)nStateId < 0 || (int32_t)nStateId >= sgptDataTables->nStatesTxtRecordCount)
        return;

    pStatesTxt = &sgptDataTables->pStatesTxt[nStateId];
    if (!pStatesTxt)
        return;

    nOverlay0Id = (int16_t)pStatesTxt->wOverlay[0] < 0 ? -1 : (int16_t)pStatesTxt->wOverlay[0];
    nOverlay1Id = (int16_t)pStatesTxt->wOverlay[1] < 0 ? -1 : (int16_t)pStatesTxt->wOverlay[1];
    nOverlay2Id = (int16_t)pStatesTxt->wOverlay[2] < 0 ? -1 : (int16_t)pStatesTxt->wOverlay[2];

    if (nOverlay0Id != -1)
    {
        nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay0Id);
        for (i = 0; i < nCount; ++i)
            GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay0Id + i));

        if (nOverlay0Id + 1 != -1)
        {
            nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay0Id + 1);
            for (i = 0; i < nCount; ++i)
                GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay0Id + 1 + i));
        }
    }

    if (nOverlay1Id != -1)
    {
        nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay1Id);
        for (i = 0; i < nCount; ++i)
            GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay1Id + i));

        if (nOverlay1Id + 1 != -1)
        {
            nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay1Id + 1);
            for (i = 0; i < nCount; ++i)
                GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay1Id + 1 + i));
        }
    }

    if (nOverlay2Id != -1)
    {
        nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay2Id);
        for (i = 0; i < nCount; ++i)
            GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay2Id + i));

        if (nOverlay2Id + 1 != -1)
        {
            nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay2Id + 1);
            for (i = 0; i < nCount; ++i)
                GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay2Id + 1 + i));
        }
    }
}


static_assert(offsetof(D2StatesTxt, wSkill) == 0x38 && STAT_SKILL_FRENZY == 0xA9 && SKILL_NEST == 0xA7
                  && UNITFLAG_DRAWSHADOW == 0x20 && UNITFLAG_ISREVIVE == 0x80000000u,
              "`mov ax, [esi+38h]` (0x6FB4C98A), `push 0A9h` (0x6FB4CDF6), `cmp eax, 0A7h` (0x6FB4CA91), `or al, 20h` "
              "(0x6FB4CA35, 0x6FB4CC3B), `or dword ptr [esi+0C4h], 80000000h` (0x6FB4CD81)");


//1.10f: D2Client.0x6FB4C560
//  States.txt setfunc 8: `cyclonearmor`. Cyclone armor's tiers from what is left of it: STATES_SetFunc6_BoneArmor over again, on the same two stats
//  (STAT_BONEARMOR against STAT_BONEARMORMAX, push 84h and push 85h; the row's `stat` column is empty): three tiers at two thirds or more, two from
//  one third, one below, `nMax / 3` signed and two thirds twice that; the old tier read from STAT_SHORTPARAM1 and the new one stored there before
//  either is compared. `overlay1..3` are cyclonearmor1front, 2front and 3front, each followed by its `back` row.
//  More tiers than before: STATES_RemFunc5_CycloneArmor takes every front and back off, then the third tier's pair goes on as looping overlays
//  (kind 3) at three, the second's at two or more, the first's at one or more; each id tested for -1 first, and each pair the id and id + 1.
//  The same tier or fewer (the jle takes the equal case too) runs three removals, which keep the overlay-group remover's own -1 test with no -1
//  test around them at all. Two of their tests are not what the rest of the function tests (see the notes at them).
//  The original inlines DATATBLS_GetStatesTxtRecord and an overlay-group removal here.
void __fastcall STATES_SetFunc8_CycloneArmor(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2StatesTxt *pStatesTxt;
    D2StatListStrc *pStatList;
    int32_t nAbsorb;
    int32_t nAbsorbMax;
    int32_t nOverlay0Id;
    int32_t nOverlay1Id;
    int32_t nOverlay2Id;
    int32_t nOldTier;
    int32_t nTier;
    int32_t nCount;
    int32_t i;

    if ((int32_t)nStateId < 0 || (int32_t)nStateId >= sgptDataTables->nStatesTxtRecordCount)
        return;

    pStatesTxt = &sgptDataTables->pStatesTxt[nStateId];
    if (!pStatesTxt)
        return;

    pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, nStateId);
    if (!pStatList)
        return;

    nAbsorb = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_BONEARMOR, 0);
    nAbsorbMax = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_BONEARMORMAX, 0);

    nOverlay0Id = (int16_t)pStatesTxt->wOverlay[0] < 0 ? -1 : (int16_t)pStatesTxt->wOverlay[0];
    nOverlay1Id = (int16_t)pStatesTxt->wOverlay[1] < 0 ? -1 : (int16_t)pStatesTxt->wOverlay[1];
    nOverlay2Id = (int16_t)pStatesTxt->wOverlay[2] < 0 ? -1 : (int16_t)pStatesTxt->wOverlay[2];

    nOldTier = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_SHORTPARAM1, 0);

    nTier = 3;
    if (nAbsorb < nAbsorbMax / 3)
        nTier = 2;
    if (nAbsorb < 2 * (nAbsorbMax / 3))
        --nTier;

    D2COMMON_STATLIST_SetStatIfListIsValid(pStatList, STAT_SHORTPARAM1, nTier, 0);

    if (nTier > nOldTier)
    {
        STATES_RemFunc5_CycloneArmor(pUnit, nStateId);

        if (nOverlay2Id != -1 && nTier >= 3)
        {
            GFX_AddOverlay(pUnit, (D2_OverlayIds)nOverlay2Id, OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
            GFX_AddOverlay(pUnit, (D2_OverlayIds)(nOverlay2Id + 1), OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
        }

        if (nOverlay1Id != -1 && nTier >= 2)
        {
            GFX_AddOverlay(pUnit, (D2_OverlayIds)nOverlay1Id, OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
            GFX_AddOverlay(pUnit, (D2_OverlayIds)(nOverlay1Id + 1), OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
        }

        if (nOverlay0Id != -1 && nTier >= 1)
        {
            GFX_AddOverlay(pUnit, (D2_OverlayIds)nOverlay0Id, OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
            GFX_AddOverlay(pUnit, (D2_OverlayIds)(nOverlay0Id + 1), OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
        }
        return;
    }

    //  BUG: the ids are tested against 0, not -1 (test ebx, ebx / jz at 0x6FB4C737 and 0x6FB4C797), where the add half tests -1.
    //  With an empty column (-1) the test passes, the remover's own -1 test then skips the id's group, and id + 1 is 0, so overlay 0's
    //  group comes off the unit. LATENT: the three stock columns are 208, 210 and 212.
    if (nOverlay2Id != 0 && nTier <= 2)
    {
        if (nOverlay2Id != -1)
        {
            nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay2Id);
            for (i = 0; i < nCount; ++i)
                GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay2Id + i));
        }

        if (nOverlay2Id + 1 != -1)
        {
            nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay2Id + 1);
            for (i = 0; i < nCount; ++i)
                GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay2Id + 1 + i));
        }
    }

    //  BUG: tested against 0, not -1 (see above). LATENT.
    if (nOverlay1Id != 0 && nTier <= 1)
    {
        if (nOverlay1Id != -1)
        {
            nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay1Id);
            for (i = 0; i < nCount; ++i)
                GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay1Id + i));
        }

        if (nOverlay1Id + 1 != -1)
        {
            nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay1Id + 1);
            for (i = 0; i < nCount; ++i)
                GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay1Id + 1 + i));
        }
    }

    //  BUG: the first tier's test compares the overlay id with 1, not the tier: mov ebp, [esp+28h+var_10] loads the first id over the
    //  tier (0x6FB4C7F3), then test ebp, ebp / jz and cmp ebp, 1 / jg (0x6FB4C7F7..0x6FB4C7FE). The block runs only for an id of 1 or a
    //  negative one, never for stock's 208. INERT: the tier it was meant to test is at least 1, so the first tier's pair would stay on
    //  in either reading until the state comes off.
    if (nOverlay0Id != 0 && nOverlay0Id <= 1)
    {
        if (nOverlay0Id != -1)
        {
            nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay0Id);
            for (i = 0; i < nCount; ++i)
                GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay0Id + i));
        }

        if (nOverlay0Id + 1 != -1)
        {
            nCount = (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay0Id + 1);
            for (i = 0; i < nCount; ++i)
                GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nOverlay0Id + 1 + i));
        }
    }
}


//1.10f: D2Client.0x6FB4C860
//  States.txt setfunc 7: `hurricane`. The skill's looping sound starts: the skill the state carries (MODIFIERLIST_SKILL), its skills.txt `prgsound`
//  above 0 (a signed word), and SOUNDINST_PlaySound(that group, pUnit, 0, 0, 0). STATES_RemFunc4_Hurricane detaches it again; the state's
//  `cltactivefunc`, SKILLS_CltDoFunc90, plays or detaches the same group by whether the unit is in town. The original inlines
//  DATATBLS_GetSkillsTxtRecordById here.
//  void: eax holds whatever SOUNDINST_PlaySound or a lookup left, and the dispatcher reads nothing back.
void __fastcall STATES_SetFunc7_Hurricane(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2StatListStrc *pStatList;
    int32_t nSkillId;
    D2SkillsTxt *pSkillsTxt;

    pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, nStateId);
    if (!pStatList)
        return;

    nSkillId = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_MODIFIERLIST_SKILL, 0);

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return;

    if ((int16_t)pSkillsTxt->wPrgSound > 0)
        SOUNDINST_PlaySound((D2C_Sounds)(int16_t)pSkillsTxt->wPrgSound, pUnit, 0, 0, 0);
}


//1.10f: D2Client.0x6FB4C8D0
//  States.txt remfunc 4: `hurricane`. The skill's looping sound stops following the unit. The same lookup as STATES_SetFunc7_Hurricane up to the
//  `prgsound` test; then the instance of that group playing on the unit is looked up (UNITSND_FindPlayingSoundInGroup) and detached from it with
//  bForce 1 (SOUNDINST_DetachFromUnit). Nothing is stored, so a state removed twice finds nothing the second time. The original inlines
//  DATATBLS_GetSkillsTxtRecordById here.
void __fastcall STATES_RemFunc4_Hurricane(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2StatListStrc *pStatList;
    int32_t nSkillId;
    D2SkillsTxt *pSkillsTxt;
    int32_t hPlayingSound;

    pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, nStateId);
    if (!pStatList)
        return;

    nSkillId = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_MODIFIERLIST_SKILL, 0);

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return;

    if ((int16_t)pSkillsTxt->wPrgSound <= 0)
        return;

    hPlayingSound = UNITSND_FindPlayingSoundInGroup(pUnit, (D2C_Sounds)(int16_t)pSkillsTxt->wPrgSound);
    if (hPlayingSound)
        SOUNDINST_DetachFromUnit(hPlayingSound, pUnit, 1);
}


//1.10f: D2Client.0x6FB4C940
//  States.txt setfunc 9 and remfunc 6: `shrine_skill`. Calls D2Common's SKILLS_RefreshPassiveSkills(pUnit): the skill shrine's bonus levels are
//  recounted when the state comes on and when it goes off. The state id the tables pass in edx is not read. The tables' slots are typed
//  void __fastcall(D2UnitStrc *pUnit, D2C_States nStateId): both dispatchers load edx.
void __fastcall STATES_SetFunc9_RemFunc6_ShrineSkill(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2COMMON_SKILLS_RefreshPassiveSkills(pUnit);
}


//1.10f: D2Client.0x6FB4C950
//  States.txt setfunc 10: `redeemed`. A redeemed corpse sends its missile and goes out of sight. The state row's `skill` (Redemption in the stock
//  row) and that skill's `cltmissilea` (redemption), both signed words tested against their tables' counts, fill a spawn block zeroed whole
//  (rep stosd, 17h dwords): owner and origin the unit, the missile, the skill and level 1, and nothing in the flag word. MISSILE_ClientSpawn spawns
//  it. Then, the unit tested only now, UNITFLAGEX_NODRAW and UNITFLAG_DRAWSHADOW are set, the pair STATES_SetFunc12_Invis sets. The skill id is
//  bounded twice: by the site's own compare (cmp eax, edx / jge) and by the lookup's tests. The original inlines DATATBLS_GetStatesTxtRecord and
//  DATATBLS_GetSkillsTxtRecordById here.
void __fastcall STATES_SetFunc10_Redeemed(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2StatesTxt *pStatesTxt;
    D2SkillsTxt *pSkillsTxt;
    D2MissileStrc tParams;

    if ((int32_t)nStateId < 0 || (int32_t)nStateId >= sgptDataTables->nStatesTxtRecordCount)
        return;

    pStatesTxt = &sgptDataTables->pStatesTxt[nStateId];
    if (!pStatesTxt)
        return;

    if ((int16_t)pStatesTxt->wSkill < 0 || (int16_t)pStatesTxt->wSkill >= sgptDataTables->nSkillsTxtRecordCount)
        return;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById((int16_t)pStatesTxt->wSkill);
    if (!pSkillsTxt)
        return;

    if ((int16_t)pSkillsTxt->wCltMissileA < 0 || (int16_t)pSkillsTxt->wCltMissileA >= sgptDataTables->nMissilesTxtRecordCount)
    {
        return;
    }

    memset(&tParams, 0, sizeof(tParams));
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nMissile = (int16_t)pSkillsTxt->wCltMissileA;
    tParams.nSkill = (int16_t)pStatesTxt->wSkill;
    tParams.nSkillLevel = 1;
    MISSILE_ClientSpawn(&tParams);

    if (pUnit)
    {
        pUnit->dwFlagEx |= UNITFLAGEX_NODRAW;
        pUnit->dwFlags |= UNITFLAG_DRAWSHADOW;
    }
}


//1.10f: D2Client.0x6FB4CA50
//  States.txt remfunc 7: `uninterruptable`. A nest's spawn spot is freed. Only for a monster whose skill in use is SKILL_NEST (0A7h; its id read
//  through SKILLS_GetSkillIdFromSkill with the original's source path and line 759, push 2F7h) and whose skill Param2 or Param3 is set: both are
//  cleared, and COLLIDE_MONSTER is reset around that subtile with COLLISION_PATTERN_SMALL_UNIT_PRESENCE, in the room that holds it.
//  The room is searched for twice. The first search, from the unit's room, only gates: finding nothing ends the function, and the room it finds is
//  dropped. The second repeats it from the unit's room again, and its answer, NULL included, is the reset's room. The original inlines
//  CLIENT_FindRoomContainingSubtile for both.
void __fastcall STATES_RemFunc7_Uninterruptable(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2SkillStrc *pSkill;
    int32_t nX;
    int32_t nY;

    if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER)
        return;

    pSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
    if (!pSkill)
        return;

    if (D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szPlrSkillsCpp, 759) != SKILL_NEST)
        return;

    nX = D2COMMON_SKILLS_GetParam2(pSkill);
    nY = D2COMMON_SKILLS_GetParam3(pSkill);
    if (!nX && !nY)
        return;

    D2COMMON_SKILLS_SetParam2(pSkill, 0);
    D2COMMON_SKILLS_SetParam3(pSkill, 0);

    if (!CLIENT_FindRoomContainingSubtile(D2COMMON_UNITS_GetRoom(pUnit), nX, nY))
        return;

    D2COMMON_COLLISION_ResetMaskWithPattern(CLIENT_FindRoomContainingSubtile(D2COMMON_UNITS_GetRoom(pUnit), nX, nY), nX, nY, COLLISION_PATTERN_SMALL_UNIT_PRESENCE, COLLIDE_MONSTER);
}


//1.10f: D2Client.0x6FB4CC10
//  States.txt setfunc 11: `vine_beast`. With a unit, UNITFLAGEX_NODRAW is set.
void __fastcall STATES_SetFunc11_VineBeast(D2UnitStrc *pUnit, D2C_States nStateId)
{
    if (pUnit)
        pUnit->dwFlagEx |= UNITFLAGEX_NODRAW;
}


//1.10f: D2Client.0x6FB4CC20
//  States.txt setfunc 12: `invis`. With a unit, UNITFLAGEX_NODRAW set and then UNITFLAG_DRAWSHADOW (a byte OR on the word read and written whole).
//  STATES_RemFunc8_Invis clears the same two.
void __fastcall STATES_SetFunc12_Invis(D2UnitStrc *pUnit, D2C_States nStateId)
{
    if (pUnit)
    {
        pUnit->dwFlagEx |= UNITFLAGEX_NODRAW;
        pUnit->dwFlags |= UNITFLAG_DRAWSHADOW;
    }
}


//1.10f: D2Client.0x6FB4CC50
//  States.txt remfunc 8: `invis`. With a unit, UNITFLAGEX_NODRAW cleared and then UNITFLAG_DRAWSHADOW (a byte AND on the word read and written
//  whole): STATES_SetFunc12_Invis's two, taken off.
void __fastcall STATES_RemFunc8_Invis(D2UnitStrc *pUnit, D2C_States nStateId)
{
    if (pUnit)
    {
        pUnit->dwFlagEx &= ~UNITFLAGEX_NODRAW;
        pUnit->dwFlags &= ~UNITFLAG_DRAWSHADOW;
    }
}


//1.10f: D2Client.0x6FB4CC80
//  States.txt setfunc 13: `attached`. PARTICLE_AllocUnitParticles(pUnit), then a tail jump to PARTICLE_SetUnitAttached(pUnit, 1).
void __fastcall STATES_SetFunc13_Attached(D2UnitStrc *pUnit, D2C_States nStateId)
{
    PARTICLE_AllocUnitParticles(pUnit);
    PARTICLE_SetUnitAttached(pUnit, 1);
}


//1.10f: D2Client.0x6FB4CCA0
//  States.txt remfunc 9: `attached`. PARTICLE_SetUnitAttached(pUnit, 0), then PARTICLE_SetUnitGfxCoords(pUnit, 0, 0, 0), whose answer is not read.
void __fastcall STATES_RemFunc9_Attached(D2UnitStrc *pUnit, D2C_States nStateId)
{
    PARTICLE_SetUnitAttached(pUnit, 0);
    PARTICLE_SetUnitGfxCoords(pUnit, 0, 0, 0);
}


//1.10f: D2Client.0x6FB4CCC0
//  States.txt remfunc 10: `pregnant`. A `jmp MONUNIQUE_MonUModPlayPregnantBurst` with both registers untouched, so that function runs with this
//  one's pUnit and nStateId.
void __fastcall STATES_RemFunc10_Pregnant(D2UnitStrc *pUnit, D2C_States nStateId)
{
    MONUNIQUE_MonUModPlayPregnantBurst(pUnit, nStateId);
}


//1.10f: D2Client.0x6FB4CCD0
//  States.txt setfunc 14: `wolf` and `bear`. The shapeshift, at the unit's own position: a seven-dword block zeroed (rep stosd) gets the unit's
//  position in its first two, client action 25 is dispatched with it (UNITMODE_DispatchActionByUnitType), and the path's direction is set to 0
//  (D2Common's #10160). The original inlines UNITS_GetXPosition and UNITS_GetYPosition here.
void __fastcall STATES_SetFunc14_Shapeshift(D2UnitStrc *pUnit, D2C_States nStateId)
{
    int32_t anActionParams[7];

    memset(anActionParams, 0, sizeof(anActionParams));

    anActionParams[0] = UNITS_GetXPosition(pUnit);

    anActionParams[1] = UNITS_GetYPosition(pUnit);

    UNITMODE_DispatchActionByUnitType(CLTACTION_SKILL1MODE, pUnit, anActionParams, FALSE);
    D2COMMON_10160_PathUpdateDirection(pUnit->pDynamicPath, 0);
}


//1.10f: D2Client.0x6FB4CD70
//  States.txt setfunc 15: `revive`. The anim mode set to 1 (UNITMODE_SetAnimMode), UNITFLAG_ISREVIVE set with a unit, and D2Common's #10214, which
//  re-derives the dynamic path's collision pattern from the unit's size and re-registers its footprint when the unit has a room.
void __fastcall STATES_SetFunc15_Revive(D2UnitStrc *pUnit, D2C_States nStateId)
{
    UNITMODE_SetAnimMode(pUnit, 1);

    //  BUG: the unit is tested between two uses: UNITMODE_SetAnimMode gets it first (0x6FB4CD78) and D2Common's #10214 after, both
    //  untested, and only the flag store is behind `test esi, esi` (0x6FB4CD7D). UNREACHABLE: the dispatcher has tested the unit already
    //  (0x6FB4D926), so the test cannot fail.
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_ISREVIVE;

    D2COMMON_10214(pUnit);
}


//1.10f: D2Client.0x6FB4CDA0
//  States.txt setfunc 16: `maul` and `feralrage`. A charge-up's charges as one overlay of a row. The charges are the state's STAT_SKILL_FRENZY
//  (push 0A9h); the most there can be is the skill's `calc2` (dwCalc[1], +13Ch) evaluated at the state's skill and level (above 0). `overlay1..2` and
//  `overlay3..4` each name a range of consecutive overlays: maul1..maul5 and maul1back..maul5back for Maul, feralrage1..5 and their backs for Feral
//  Rage. For each range, the one overlay at (charges - 1) * count / most, a signed division (cdq / idiv), goes on if it is not on already
//  (GFX_HasOverlay), and every other one of the range comes off.
//  Both the most and the charges are clamped to the first range's count (0x6FB4CE7E..0x6FB4CE8A) before either range is tested, and the second
//  range uses them as clamped. The original inlines DATATBLS_GetStatesTxtRecord and DATATBLS_GetSkillsTxtRecordById here.
void __fastcall STATES_SetFunc16_SkillCharges(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2StatesTxt *pStatesTxt;
    D2StatListStrc *pStatList;
    int32_t nCharges;
    int32_t nSkillId;
    int32_t nSkillLevel;
    D2SkillsTxt *pSkillsTxt;
    int32_t nMaxCharges;
    int32_t nFirst;
    int32_t nLast;
    int32_t nCount;
    int32_t nShown;
    int32_t nOverlayId;
    int32_t i;

    if ((int32_t)nStateId < 0 || (int32_t)nStateId >= sgptDataTables->nStatesTxtRecordCount)
        return;

    pStatesTxt = &sgptDataTables->pStatesTxt[nStateId];
    if (!pStatesTxt)
        return;

    pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, nStateId);
    if (!pStatList)
        return;

    nCharges = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_SKILL_FRENZY, 0);
    nSkillId = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_MODIFIERLIST_SKILL, 0);
    nSkillLevel = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_MODIFIERLIST_LEVEL, 0);

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return;

    nMaxCharges = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[1], (D2C_Skills)nSkillId, nSkillLevel);
    if (nMaxCharges <= 0)
        return;

    nFirst = (int16_t)pStatesTxt->wOverlay[0];
    nLast = (int16_t)pStatesTxt->wOverlay[1];
    nCount = nLast - nFirst + 1;
    if (nMaxCharges >= nCount)
        nMaxCharges = nCount;
    if (nCharges >= nCount)
        nCharges = nCount;

    if (nFirst >= 0 && nLast >= nFirst)
    {
        nShown = (nCharges - 1) * nCount / nMaxCharges;
        for (i = 0, nOverlayId = nFirst; i < nCount; ++i, ++nOverlayId)
        {
            if (i == nShown)
            {
                if (!GFX_HasOverlay(pUnit, nOverlayId))
                    GFX_AddOverlay(pUnit, (D2_OverlayIds)nOverlayId, OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
            }
            else
            {
                GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)nOverlayId);
            }
        }
    }

    nFirst = (int16_t)pStatesTxt->wOverlay[2];
    nLast = (int16_t)pStatesTxt->wOverlay[3];
    if (nFirst < 0 || nLast < nFirst)
        return;

    nCount = nLast - nFirst + 1;

    //  BUG: with `overlay2` one below `overlay1` the first count is 0, so the most is clamped to 0, the first range is skipped as empty, and this
    //  division by zero faults (#DE at 0x6FB4CF0A). LATENT: stock's two rows give five and five.
    nShown = (nCharges - 1) * nCount / nMaxCharges;
    for (i = 0, nOverlayId = nFirst; i < nCount; ++i, ++nOverlayId)
    {
        if (i == nShown)
        {
            if (!GFX_HasOverlay(pUnit, nOverlayId))
                GFX_AddOverlay(pUnit, (D2_OverlayIds)nOverlayId, OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
        }
        else
        {
            GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)nOverlayId);
        }
    }
}


//  The states.txt members read below, and the four flag bits tested by name. D2C_StateMasks numbers a flag as the loader places its bit (byte n >> 3
//  of the flags at +10h, bit n & 7), so bits 35, 36 and 39 are the low byte of dwStateFlagsEx (+14h) against gdwBitMasks[3], [4] and [7], and bit 3
//  the first byte of the flags against gdwBitMasks[3].
static_assert(offsetof(D2StatesTxt, dwStateFlagsEx) == 0x14 && offsetof(D2StatesTxt, wSetFunc) == 0x1A
                  && offsetof(D2StatesTxt, wRemFunc) == 0x1C && offsetof(D2StatesTxt, nColorPri) == 0x20
                  && offsetof(D2StatesTxt, nColorShift) == 0x21 && offsetof(D2StatesTxt, nLightRGB) == 0x22
                  && offsetof(D2StatesTxt, wOnSound) == 0x26 && offsetof(D2StatesTxt, wCltEvent) == 0x30
                  && offsetof(D2StatesTxt, wCltEventFunc) == 0x32 && offsetof(D2StatesTxt, wMissile) == 0x3A,
              "`mov cl, [eax+14h]` (0x6FB4D167), `mov ax, [edi+1Ah]` (0x6FB4D989), `mov ax, [edi+1Ch]` "
              "(0x6FB4DA31), `mov al, [edi+20h]` (0x6FB4D561), `cmp byte ptr [esi+21h], 68h` (0x6FB4D5B2), `mov dl, "
              "[esi+22h]` (0x6FB4D5F2), `mov ax, [ebx+26h]` (0x6FB4D447), `cmp word ptr [edi+30h], 0` (0x6FB4D9AC), "
              "`movsx ecx, word ptr [edi+32h]` (0x6FB4D9DE), `mov ax, [edx+3Ah]` (0x6FB4CFF1)");
static_assert(STATEMASK_TRANSFORM == 3 && STATEMASK_NOOVERLAYS == 35 && STATEMASK_NOCLEAR == 36
                  && STATEMASK_NOTONDEAD == 39 && OVERLAYTYPE_AURA == 8 && OVERLAYTYPE_AURAEX == 9,
              "`test [edx+0Ch], cl` on +10h (0x6FB4D4C0) and on +14h (0x6FB4D16A), `test [edx+10h], cl` "
              "(0x6FB4D43A), `test [edx+1Ch], cl` (0x6FB4D3EA), `push 8` (0x6FB4D349), `push 9` (0x6FB4D30B)");


//1.10f: D2Client.0x6FB4CF50
//  States.txt remfunc 11: `maul` and `feralrage`, the two rows STATES_SetFunc16_SkillCharges serves. The charge overlays come off whole:
//  `overlay1..2` and `overlay3..4` are the two inclusive ranges SetFunc16 shows one overlay of, and every overlay of each is removed. A range is
//  skipped when its first id is negative or above its last, and walked with jle; all signed, each column a movsx. Nothing is clamped and nothing
//  divided. The original inlines DATATBLS_GetStatesTxtRecord here.
void __fastcall STATES_RemFunc11_SkillCharges(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2StatesTxt *pStatesTxt;
    int32_t nFirst;
    int32_t nLast;

    if ((int32_t)nStateId < 0 || (int32_t)nStateId >= sgptDataTables->nStatesTxtRecordCount)
        return;

    pStatesTxt = &sgptDataTables->pStatesTxt[nStateId];
    if (!pStatesTxt)
        return;

    nFirst = (int16_t)pStatesTxt->wOverlay[0];
    nLast = (int16_t)pStatesTxt->wOverlay[1];
    if (nFirst >= 0 && nFirst <= nLast)
    {
        do
            GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nFirst++));
        while (nFirst <= nLast);
    }

    nFirst = (int16_t)pStatesTxt->wOverlay[2];
    nLast = (int16_t)pStatesTxt->wOverlay[3];
    if (nFirst >= 0 && nFirst <= nLast)
    {
        do
            GFX_RemoveOverlay(pUnit, 0, (D2_OverlayIds)(nFirst++));
        while (nFirst <= nLast);
    }
}


//1.10f: D2Client.0x6FB4CFC0
//  States.txt setfunc 17: `restinpeace`. The state's missile, from the unit. With a `missile` from 0 below the missiles.txt count, a D2MissileStrc
//  zeroed whole (rep stosd of 17h dwords) takes the unit as owner and origin, the missile, the row's `skill` (0 when the column is negative) and
//  level 1, and goes to MISSILE_ClientSpawn, whose answer is not read.
//  The row also sets `notondead`, which keeps PLRSKILLS_StatesRunStateSetFunc from calling this for a monster in MONMODE_DEAD or a player in
//  PLRMODE_DEAD. The original inlines DATATBLS_GetStatesTxtRecord here.
void __fastcall STATES_SetFunc17_RestInPeace(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2StatesTxt *pStatesTxt;
    D2MissileStrc tParams;

    if ((int32_t)nStateId < 0 || (int32_t)nStateId >= sgptDataTables->nStatesTxtRecordCount)
        return;

    pStatesTxt = &sgptDataTables->pStatesTxt[nStateId];
    if (!pStatesTxt)
        return;

    if ((int16_t)pStatesTxt->wMissile < 0 || (int16_t)pStatesTxt->wMissile >= sgptDataTables->nMissilesTxtRecordCount)
        return;

    memset(&tParams, 0, sizeof(tParams));
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nMissile = (int16_t)pStatesTxt->wMissile;
    tParams.nSkill = (int16_t)pStatesTxt->wSkill < 0 ? 0 : (int16_t)pStatesTxt->wSkill;
    tParams.nSkillLevel = 1;
    MISSILE_ClientSpawn(&tParams);
}


//1.10f: D2Client.0x6FB4D060
//  States.txt setfunc 18: `corpseexp`. Blood, then STATES_SetFunc17_RestInPeace's missile. MONSTER_SpawnBlood(pUnit, 5, 0) runs before the state is
//  looked up, so for any id; the rest is SetFunc17's body, the same tests and stores. The row also sets `notondead`. The original inlines
//  DATATBLS_GetStatesTxtRecord here.
void __fastcall STATES_SetFunc18_CorpseExp(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2StatesTxt *pStatesTxt;
    D2MissileStrc tParams;

    MONSTER_SpawnBlood(pUnit, 5, 0);

    if ((int32_t)nStateId < 0 || (int32_t)nStateId >= sgptDataTables->nStatesTxtRecordCount)
        return;

    pStatesTxt = &sgptDataTables->pStatesTxt[nStateId];
    if (!pStatesTxt)
        return;

    if ((int16_t)pStatesTxt->wMissile < 0 || (int16_t)pStatesTxt->wMissile >= sgptDataTables->nMissilesTxtRecordCount)
        return;

    memset(&tParams, 0, sizeof(tParams));
    tParams.pOwner = pUnit;
    tParams.pOrigin = pUnit;
    tParams.nMissile = (int16_t)pStatesTxt->wMissile;
    tParams.nSkill = (int16_t)pStatesTxt->wSkill < 0 ? 0 : (int16_t)pStatesTxt->wSkill;
    tParams.nSkillLevel = 1;
    MISSILE_ClientSpawn(&tParams);
}


//1.10f: D2Client.0x6FB4D110
//  States.txt setfunc 19 and remfunc 12: `skilldelay`. `xor ecx, ecx / jmp GAME_SetSkillBlockUntilFrame`: a tail call with frame 0, so the frame
//  until which the client blocks a skill is cleared as the state comes on and as it goes off. Neither argument the tables pass is read. The tables'
//  slots are typed void __fastcall(D2UnitStrc *pUnit, D2C_States nStateId).
void __fastcall STATES_SetFunc19_RemFunc12_SkillDelay(D2UnitStrc *pUnit, D2C_States nStateId)
{
    GAME_SetSkillBlockUntilFrame(0);
}


//1.10f: D2Client.0x6FB4D120
//  A state's overlays as it comes on. Called by STATES_ToggleStateWithSound. Nothing for a `nooverlays` row (stock's bonearmor, maul, feralrage and
//  cyclonearmor, whose set functions place their own). Otherwise three ids, `castoverlay` (+0Ah), `overlay1` and `overlay2` (+2, +4), each -1 when
//  negative, are each moved to a random row of its group: D2Common's DATATBLS_Get1OfNFromOverlayTxt answers how many consecutive rows the overlay
//  has, and the unit's own seed picks one. Whether the state is an aura is asked before the rolls (D2Common's STATES_CheckStateMaskAuraByStateId).
//  An aura drops its cast overlay when another aura is on: the state is turned off, D2Common's STATES_CheckStateMaskAuraOnUnit asked, and the
//  state turned back on, so the answer is about the unit's other auras. Then either the cast overlay alone, carrying the other two, or each of the
//  other two on its own (o1 and o2 are `overlay1` and `overlay2`, nOverlay0Id and nOverlay1Id below):
//      cast    aura      (cast, OVERLAYTYPE_AURAEX 9, NULL, o1, o2, state, 0)
//              no aura   (cast, OVERLAYTYPE_LOOPEX 4, NULL, o1, o2, 0, 0)
//      none    aura      each of o1, o2: (id, OVERLAYTYPE_AURA 8, NULL, state, 0, 0, 0)
//              no aura   each of o1, o2: (id, OVERLAYTYPE_LOOP 3, NULL, 0, 0, 0, 0)
//  GFX_AddOverlay's nTargetOrStateId, nX and nY are named for other kinds; for 4 and 9 they carry the two overlays and the state.
//  The original inlines DATATBLS_GetStatesTxtRecord and SEED_RollLimitedRandomNumber here.
void __fastcall STATES_ApplyStateOverlays(D2UnitStrc *pUnit, D2C_States nStateId)
{
    D2StatesTxt *pStatesTxt;
    int32_t nCastOverlayId;
    int32_t nOverlay0Id;
    int32_t nOverlay1Id;
    BOOL bIsAura;

    if ((int32_t)nStateId < 0 || (int32_t)nStateId >= sgptDataTables->nStatesTxtRecordCount)
        return;

    pStatesTxt = &sgptDataTables->pStatesTxt[nStateId];
    if (!pStatesTxt)
        return;

    if ((uint8_t)pStatesTxt->dwStateFlagsEx & (uint8_t)gdwBitMasks[STATEMASK_NOOVERLAYS & 7])
        return;

    nCastOverlayId = (int16_t)pStatesTxt->wCastOverlay < 0 ? -1 : (int16_t)pStatesTxt->wCastOverlay;
    nOverlay0Id = (int16_t)pStatesTxt->wOverlay[0] < 0 ? -1 : (int16_t)pStatesTxt->wOverlay[0];
    nOverlay1Id = (int16_t)pStatesTxt->wOverlay[1] < 0 ? -1 : (int16_t)pStatesTxt->wOverlay[1];

    bIsAura = D2COMMON_STATES_CheckStateMaskAuraByStateId(nStateId);

    if (nCastOverlayId != -1)
        nCastOverlayId += SEED_RollLimitedRandomNumber(&pUnit->pSeed, (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nCastOverlayId));

    if (nOverlay0Id != -1)
        nOverlay0Id += SEED_RollLimitedRandomNumber(&pUnit->pSeed, (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay0Id));

    if (nOverlay1Id != -1)
        nOverlay1Id += SEED_RollLimitedRandomNumber(&pUnit->pSeed, (int32_t)D2COMMON_DATATBLS_Get1OfNFromOverlayTxt(nOverlay1Id));

    if (bIsAura)
    {
        D2COMMON_STATES_ToggleState(pUnit, nStateId, FALSE);
        if (D2COMMON_STATES_CheckStateMaskAuraOnUnit(pUnit))
            nCastOverlayId = -1;
        D2COMMON_STATES_ToggleState(pUnit, nStateId, TRUE);
    }

    if (nCastOverlayId != -1)
    {
        if (bIsAura)
            GFX_AddOverlay(pUnit, (D2_OverlayIds)nCastOverlayId, OVERLAYTYPE_AURAEX, 0, nOverlay0Id, nOverlay1Id, nStateId, 0);
        else
            GFX_AddOverlay(pUnit, (D2_OverlayIds)nCastOverlayId, OVERLAYTYPE_LOOPEX, 0, nOverlay0Id, nOverlay1Id, 0, 0);
        return;
    }

    if (nOverlay0Id != -1)
    {
        if (bIsAura)
            GFX_AddOverlay(pUnit, (D2_OverlayIds)nOverlay0Id, OVERLAYTYPE_AURA, 0, nStateId, 0, 0, 0);
        else
            GFX_AddOverlay(pUnit, (D2_OverlayIds)nOverlay0Id, OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
    }

    if (nOverlay1Id != -1)
    {
        if (bIsAura)
            GFX_AddOverlay(pUnit, (D2_OverlayIds)nOverlay1Id, OVERLAYTYPE_AURA, 0, nStateId, 0, 0, 0);
        else
            GFX_AddOverlay(pUnit, (D2_OverlayIds)nOverlay1Id, OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
    }
}


//1.10f: D2Client.0x6FB4D3A0
//  A state on from the server, with its client side. Called by the handlers of packets 0xA7, 0xA8 and 0xAA, each of which runs
//  PLRSKILLS_StatesRunStateSetFunc after it. With a unit (tested first) and its row:
//    * `notondead`: a monster in MONMODE_DEAD or a player in PLRMODE_DEAD has the state turned on and nothing else;
//    * the unit has the state already: its stat list is fetched and, unless the row has `noclear`, emptied (D2Common's STATLIST_RemoveAllStats,
//      which tests for NULL itself);
//    * it has not: the row's `onsound`, when not negative, is played on the unit (SOUNDINST_PlaySound).
//  Then always: the state on; the colour recomputed for a row with a `colorshift` (STATES_RefreshUnitColorShift); STATES_ApplyStateOverlays; for
//  a unit that is dead (CUNIT_IsUnitDead) under a state that hides it (D2Common's STATES_CheckStateMaskHideOnUnit), UNITFLAGEX_NODRAW set, and
//  UNITFLAG_TARGETABLE cleared and UNITFLAG_DRAWSHADOW set in the low byte of dwFlags, read and written whole; and for a `transform` row the
//  unit's graphics restarted: GFX_ResetUnitGfx, anim mode 1 (the neutral mode of both) for a player or a monster, GFX_RebuildUnitAnimForMode,
//  D2Common's UNITS_SetAnimStartFrame, the precise frame (+44h) zeroed and the used skill cleared.
//  The original inlines DATATBLS_GetStatesTxtRecord here.
void __fastcall STATES_ToggleStateWithSound(D2UnitStrc *pUnit, D2C_States nState)
{
    D2StatesTxt *pStatesTxt;
    D2StatListStrc *pStatList;

    if (!pUnit)
        return;

    if ((int32_t)nState < 0 || (int32_t)nState >= sgptDataTables->nStatesTxtRecordCount)
        return;

    pStatesTxt = &sgptDataTables->pStatesTxt[nState];
    if (!pStatesTxt)
        return;

    if ((uint8_t)pStatesTxt->dwStateFlagsEx & (uint8_t)gdwBitMasks[STATEMASK_NOTONDEAD & 7])
    {
        if (pUnit->dwUnitType == UNIT_MONSTER && pUnit->dwAnimMode == MONMODE_DEAD)
        {
            //  The bytes push eax, which holds the unit type, 1 here; written 1.
            D2COMMON_STATES_ToggleState(pUnit, nState, TRUE);
            return;
        }

        if (pUnit->dwUnitType == UNIT_PLAYER && pUnit->dwAnimMode == PLRMODE_DEAD)
        {
            D2COMMON_STATES_ToggleState(pUnit, nState, TRUE);
            return;
        }
    }

    if (D2COMMON_STATES_CheckState(pUnit, nState))
    {
        pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, nState);
        if (!((uint8_t)pStatesTxt->dwStateFlagsEx & (uint8_t)gdwBitMasks[STATEMASK_NOCLEAR & 7]))
            D2COMMON_STATLIST_RemoveAllStats(pStatList);
    }
    else if ((int16_t)pStatesTxt->wOnSound >= 0)
    {
        SOUNDINST_PlaySound((D2C_Sounds)(int16_t)pStatesTxt->wOnSound, pUnit, 0, 0, 0);
    }

    D2COMMON_STATES_ToggleState(pUnit, nState, TRUE);

    if (pStatesTxt->nColorShift)
        STATES_RefreshUnitColorShift(pUnit);

    STATES_ApplyStateOverlays(pUnit, nState);

    if (CUNIT_IsUnitDead(pUnit) && D2COMMON_STATES_CheckStateMaskHideOnUnit(pUnit))
    {
        pUnit->dwFlagEx |= UNITFLAGEX_NODRAW;
        pUnit->dwFlags = (pUnit->dwFlags & ~UNITFLAG_TARGETABLE) | UNITFLAG_DRAWSHADOW;
    }

    if (pStatesTxt->nStateFlags[0] & (uint8_t)gdwBitMasks[STATEMASK_TRANSFORM & 7])
    {
        GFX_ResetUnitGfx(pUnit);
        if (pUnit->dwUnitType == UNIT_PLAYER || pUnit->dwUnitType == UNIT_MONSTER)
            UNITMODE_SetAnimMode(pUnit, 1);
        GFX_RebuildUnitAnimForMode(pUnit, 1);
        D2COMMON_UNITS_SetAnimStartFrame(pUnit);
        pUnit->nSeqCurrentFramePrecise = 0;
        D2COMMON_UNITS_SetUsedSkill(pUnit, NULL);
    }
}


//1.10f: D2Client.0x6FB4D510
//  The unit's colour from its states. Called by STATES_ToggleStateWithSound and PLRSKILLS_EndStateAndSkill, each only for a row with a
//  `colorshift`. Every states.txt row in id order, the count re-read each pass: of the states the unit has, the first with the highest `colorpri`
//  wins, a strict `>` on an unsigned byte starting from 0, so a priority of 0 never wins and a tie keeps the lower id. The lookup inside the walk
//  makes its `>= 0` test on the scaled offset.
//  No winner, or state 0 winning (test ebx, ebx / jle), zeroes the palette index (+6Ch), and the local player's light, if it has one (+64h), goes
//  back to white (DLIGHTMAP_SetColor). A winner's `colorshift` is the palette index and its `light-r/g/b` (+22h..+24h) the local player's light,
//  except that `colorshift` 104 on the local player under a hardware renderer (D2Gfx #10006) gives index 0. A winner the second lookup refuses
//  leaves the palette index as it was.
//  __fastcall(D2UnitStrc *) stands for the original's __thiscall: one argument, in ecx either way. The original inlines
//  DATATBLS_GetStatesTxtRecord here.
void __fastcall STATES_RefreshUnitColorShift(D2UnitStrc *pUnit)
{
    D2StatesTxt *pStatesTxt;
    int32_t nBestState;
    uint8_t nBestColorPri;
    int32_t nState;

    nBestState = -1;
    nBestColorPri = 0;
    for (nState = 0; nState < sgptDataTables->nStatesTxtRecordCount; ++nState)
    {
        if (nState < 0 || nState >= sgptDataTables->nStatesTxtRecordCount)
            continue;

        pStatesTxt = &sgptDataTables->pStatesTxt[nState];
        if (pStatesTxt && D2COMMON_STATES_CheckState(pUnit, (D2C_States)nState) && pStatesTxt->nColorPri > nBestColorPri)
        {
            nBestState = nState;
            nBestColorPri = pStatesTxt->nColorPri;
        }
    }

    //  BUG: a state 0 winner counts as none (jle where the none is -1). INERT: stock's row 0, `none`, has no `colorpri`, so it cannot win.
    if (nBestState <= 0)
    {
        pUnit->nPaletteIndex = 0;
        if (pUnit == CUNIT_GetPlayer() && pUnit->pLight)
            DLIGHTMAP_SetColor(pUnit->pLight, 0xFF, 0xFF, 0xFF);
        return;
    }

    if (nBestState < 0 || nBestState >= sgptDataTables->nStatesTxtRecordCount)
        return;

    pStatesTxt = &sgptDataTables->pStatesTxt[nBestState];
    if (!pStatesTxt)
        return;

    if (pStatesTxt->nColorShift == 104 && pUnit == CUNIT_GetPlayer() && D2GFX_HardwareAcceleratedRenderMode_10006())
        pUnit->nPaletteIndex = 0;
    else
        pUnit->nPaletteIndex = pStatesTxt->nColorShift;

    if (pUnit == CUNIT_GetPlayer() && pUnit->pLight)
        DLIGHTMAP_SetColor(pUnit->pLight, pStatesTxt->nLightRGB[0], pStatesTxt->nLightRGB[1], pStatesTxt->nLightRGB[2]);
}

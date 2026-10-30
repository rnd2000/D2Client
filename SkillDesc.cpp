#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "WINMAIN.h"
#include "CoreArchive.h"
#include "ui.h"
#include "SkillDesc.h"

//  SkillDesc.cpp -- the skill descriptions of D2Client.dll 1.10: the skilldesc.txt descline dispatcher (SKILLDESC_BuildDescLine) with its text
//  formatters, the tooltip builders (current/next level, requirements, finishing move), the descdam and descatt handlers that compute and
//  draw the skill panel's damage and attack-rating figures, and the panel drawer SKILLDESC_DrawSkillInfo with its callers.
//  The original source file is UI\SkillDesc.cpp (the assert string at 0x6FB8CE90 names it). Address range 0x6FB0A4A0..0x6FB16300.

//  Layout and value checks for the skilldesc.txt and skills.txt offsets this file indexes.

static_assert(offsetof(D2SkillDescTxt, wStrMana) == 0x10
                  && offsetof(D2SkillDescTxt, wDescMissile) == 0x3C
                  && offsetof(D2SkillDescTxt, nDescLine) == 0x42
                  && offsetof(D2SkillDescTxt, wDescTextA) == 0x54
                  && offsetof(D2SkillDescTxt, wDescTextB) == 0x76
                  && offsetof(D2SkillDescTxt, dwDescCalcA) == 0x98
                  && offsetof(D2SkillDescTxt, dwDescCalcB) == 0xDC,
              "the seven skilldesc.txt columns this file indexes, at the "
              "offsets the cases were read from");

//  Data-table offsets this file reads.
static_assert(offsetof(D2DataTablesStrc, pSkillDescTxt) == 0xB8C
                  && offsetof(D2DataTablesStrc, nSkillDescTxtRecordCount) == 0xB94
                  && offsetof(D2DataTablesStrc, pMonStatsTxt) == 0xA78
                  && offsetof(D2DataTablesStrc, nMonStatsTxtRecordCount) == 0xA80
                  && offsetof(D2DataTablesStrc, pMissilesTxt) == 0xB64
                  && offsetof(D2DataTablesStrc, nMissilesTxtRecordCount) == 0xB6C,
              "the records and counts this file bounds-checks, at the "
              "displacements the guards load");

static_assert(offsetof(D2SkillsTxt, wAuraStat) == 0x54
                  && offsetof(D2SkillsTxt, dwAuraStatCalc) == 0x68
                  && offsetof(D2SkillsTxt, nPassiveStat) == 0x98
                  && offsetof(D2SkillsTxt, dwPassiveCalc) == 0xA4
                  && offsetof(D2SkillsTxt, wSummon) == 0xBC
                  && offsetof(D2SkillsTxt, wMinMana) == 0x186
                  && offsetof(D2SkillsTxt, nManaShift) == 0x188
                  && offsetof(D2SkillsTxt, wMana) == 0x18A
                  && offsetof(D2SkillsTxt, wLevelMana) == 0x18C
                  && offsetof(D2SkillsTxt, wSkillDesc) == 0x194
                  && offsetof(D2SkillsTxt, nEType) == 0x1DC,
              "the skills.txt columns: the aura and passive pairs (stat "
              "followed by its calc), the mana pair with its shift, the "
              "summon id, the element and the skilldesc link");

static_assert(offsetof(D2MissilesTxt, wSubMissile) == 0x18
                  && offsetof(D2MissilesTxt, dwParam) == 0x38
                  && offsetof(D2MissilesTxt, wRange) == 0x96
                  && offsetof(D2MissilesTxt, wLevRange) == 0x98,
              "the missiles.txt columns desclines 0x17..0x1D read");

static_assert(offsetof(D2MonStatsInitStrc, nMinHP) == 0x00
                  && offsetof(D2MonStatsInitStrc, nMaxHP) == 0x04
                  && offsetof(D2MonStatsInitStrc, nA1MinD) == 0x14
                  && offsetof(D2MonStatsInitStrc, nA1MaxD) == 0x18,
              "the calculated-stats block the summon cases read");

static_assert(offsetof(D2MonStatsTxt, nLevel) == 0xAA
                  && offsetof(D2DifficultyLevelsTxt, dwAiCurseDiv) == 0x1C,
              "the monster level this file indexes by difficulty, and the "
              "per-difficulty AI-curse divisor it divides by");

//  Values the logic compares against: the 'dummy' sentinel string, the damage and element labels, the punctuation strings and two item stats.
static_assert(STR_IDX_5382_dummy == 5382,
              "the no-string sentinel; every guard compares against it");
static_assert(STR_IDX_3998_newline == 3998 && STR_IDX_3995_space == 3995
                  && STR_IDX_3997_colon == 3997 && STR_IDX_4000_slash == 4000,
              "the four punctuation strings the tails append");
static_assert(STR_IDX_4256_StrSkill4 == 4256 && STR_IDX_4257_StrSkill5 == 4257
                  && STR_IDX_4258_StrSkill6 == 4258 && STR_IDX_4259_StrSkill7 == 4259
                  && STR_IDX_4260_StrSkill8 == 4260 && STR_IDX_4291_StrSkill39 == 4291
                  && STR_IDX_3524_ModStr2z == 3524,
              "the damage labels: 4256 Damage, and what the six-way element "
              "switch maps the ElemTypes ids to -- 1 Fire 4257, 2 Lightning "
              "4259, 3 Magic 4291, 4 Cold 4258, 5 Poison 4260, 6 Life Drain "
              "3524 (0x6FB13FDC..0x6FB14009)");
static_assert(STAT_DAMAGEPERCENT == 25 && STAT_ITEM_NORMALDAMAGE == 111,
              "the two stats the aura and passive walks test -- "
              "damagepercent and item_normaldamage");
static_assert(MONSTER_FIREGOLEM == 292,
              "the id descline 0x29 validates before the stat block");
static_assert(DEFAULT_FRAMES_PER_SECOND == 25,
              "the /25 of every frames-to-seconds line");

//  skills.txt and skilldesc.txt columns the two tooltip builders bound-check and index.
static_assert(offsetof(D2SkillsTxt, nCharClass) == 0x0C
                  && offsetof(D2SkillsTxt, wMaxLvl) == 0x12C
                  && offsetof(D2SkillsTxt, wReqLevel) == 0x174,
              "the class, level cap and required level of a skills.txt row");
static_assert(offsetof(D2SkillDescTxt, wStrName) == 0x08
                  && offsetof(D2SkillDescTxt, wStrLong) == 0x0C,
              "the skilldesc.txt name and long string");

//  tbl string ids the two tooltip builders fetch; the digit suffixes in the names are the tbl's own.
static_assert(STR_IDX_4187_skilldesc3 == 4187 && STR_IDX_4252_StrSkill0 == 4252
                  && STR_IDX_4253_StrSkill1 == 4253 && STR_IDX_4254_StrSkill2 == 4254
                  && STR_IDX_4267_StrSkill15 == 4267 && STR_IDX_4268_StrSkill16 == 4268
                  && STR_IDX_4269_StrSkill17 == 4269 && STR_IDX_4369_StrSkill116 == 4369
                  && STR_IDX_4370_StrSkill117 == 4370 && STR_IDX_4371_StrSkill118 == 4371
                  && STR_IDX_4372_StrSkill119 == 4372,
              "Required Level, the two level labels, the singular/plural "
              "second pair, and the four not-learned / item suffixes");


//  The charge-up block of D2SkillDescTxt (three parallel arrays) and the skills.txt flag index the finishing-move gate tests.
static_assert(offsetof(D2SkillDescTxt, nPrgDamElem) == 0x20
                  && offsetof(D2SkillDescTxt, dwProgDmgMin) == 0x24
                  && offsetof(D2SkillDescTxt, dwProgDmgMax) == 0x30,
              "the charge-up element byte and its two calc arrays");
static_assert(SKILLSFLAGINDEX_FINISHING == 3,
              "gdwBitMasks[3] at 0x6FB0B447 and 0x6FB0BAC8 -- the flag that "
              "gates the charge-up bonus and the charge-up elemental damage");

//  The two mastery stats of the Fire/Lightning arms and the four ui_color_t values of the elemental line's colour remap.
static_assert(STAT_PASSIVE_FIRE_MASTERY == 329
                  && STAT_PASSIVE_LTNG_MASTERY == 330,
              "the mastery stats of the Fire and Lightning arms");
static_assert(UI_COLOR_RED == 1 && UI_COLOR_YELLOW == 9
                  && UI_COLOR_BLUE == 3 && UI_COLOR_LIGHT_GREEN == 2,
              "the four colours the element remap writes, out of ui_color_t");

//  The srcdam byte of the dual-wield scaling, the two fonts of its fit test and the state, item type and stats of the throwing-potion arm.
static_assert(offsetof(D2SkillsTxt, nSrcDam) == 0x1A5,
              "mov dl, [esi+1A5h] at 0x6FB0C480 -- srcdam, the 128ths scale");
static_assert(D2FONT_FONT16 == 1 && D2FONT_FONT6 == 6,
              "the two fonts the fit test chooses between -- 1 when the text "
              "fits, 6 and a one-pixel nudge when it does not");
static_assert(STATE_THROWINGMASTERY == 78
                  && ITEMTYPE_MISSILE_POTION == 38
                  && STAT_ITEM_THROW_MINDAMAGE == 0x9F
                  && STAT_ITEM_THROW_MAXDAMAGE == 0xA0,
              "the throwing-weapon path: the state the mastery is gated on, "
              "the item type, and the two throw stats");
static_assert(UI_COLOR_DARK_GOLD == 4 && UI_COLOR_WHITE == 0,
              "the potion arm's lightning colour -- 4, not the 9 the rest of "
              "the panel uses -- and the untouched default of a magic potion");

//  The item type and body slots of the weapon walker, the language band that picks the first line's offset, and the attack-rating stats.
static_assert(ITEMTYPE_WEAPON == 45 && BODYLOC_RARM == 4 && BODYLOC_LARM == 5,
              "the wielded-weapon test and the two hand slots the other hand "
              "is computed from");
static_assert(LANGUAGE_JAPANESE == 6 && LANGUAGE_CHINESETWN == 9,
              "the CJK band whose line-1 offset is -7 instead of -6");
static_assert(STAT_ITEM_TOHIT_PERCENT == 0x77 && STAT_PROGRESSIVE_TOHIT == 0x145,
              "the to-hit percentage and the progressive to-hit the finishing "
              "skills add");

//  __FILE__ as the descdam handlers pass it to the assert helpers; the line numbers in those calls are the original's own.
static const char szSkillDescCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UI\\SkillDesc.cpp";

//  The two damage-calc columns the descdam handlers evaluate, and the skills.txt fields and kick stat SKILLDESC_DamFunc2 reads.
static_assert(offsetof(D2SkillDescTxt, dwDamCalc) == 0x18,
              "`[esi+18h]` and `[esi+1Ch]` at 0x6FB0C314 / 0x6FB0C334 -- the "
              "two dwDamCalc columns of the skilldesc record");
static_assert(offsetof(D2SkillsTxt, nToHitShift) == 0x1A4
                  && offsetof(D2SkillsTxt, dwMinDam) == 0x1A8,
              "`mov cl, [eax+1A4h]` and `mov esi, [eax+1A8h]` at 0x6FB0CD5A / "
              "0x6FB0CD63 -- the shift and the kick base of DamFunc2");
static_assert(STAT_ITEM_KICKDAMAGE == 0x89,
              "`push 89h` at 0x6FB0CD6E -- the stat the kick damage is added "
              "to");

//  The damage-percent stats SKILLDESC_DamFunc21 folds into damagepercent.
static_assert(STAT_ITEM_MINDAMAGE_PERCENT == 0x12
                  && STAT_ITEM_MAXDAMAGE_PERCENT == 0x11,
              "`push 12h` at 0x6FB0CF6E and `push 11h` at 0x6FB0CF7E -- the "
              "two percent stats the throw line adds to damagepercent");

//  The formula column SKILLDESC_DamFunc13 evaluates and the low bound of SKILLDESC_DamFunc8's skill-id guard.
static_assert(offsetof(D2SkillsTxt, dwCalc) == 0x138,
              "`mov ecx, [edi+138h]` at 0x6FB0E7C2 -- dwCalc[0] of "
              "DamFunc13");
static_assert(SKILL_ATTACK == 0,
              "`test eax, eax` / `jl` at 0x6FB0E824/0x6FB0E826 -- "
              "DamFunc8's skill id must not be negative");

//  Fields and stats read by SKILLDESC_DamFunc9, DamFunc10 and DamFunc11, and the weapon-damage stats the shield line's dual-wield test uses.
static_assert(offsetof(D2SkillsTxt, dwMaxDam) == 0x1AC,
              "`mov ebx, [esi+1ACh]` at 0x6FB0EAD3 -- dwMaxDam, shifted by "
              "nToHitShift beside dwMinDam");
static_assert(offsetof(D2SkillsTxt, dwParam) == 0x148,
              "`mov esi, [eax+154h]` / `add esi, [eax+150h]` at "
              "0x6FB0EE2A / 0x6FB0EE34 -- dwParam[3] and [2] of the shield "
              "bonus");
static_assert(STAT_PASSIVE_COLD_MASTERY == 0x14B,
              "`push 14Bh` at 0x6FB0F3DB -- the cold mastery, beside the "
              "fire and lightning pair batch 4b asserted");
static_assert(SKILL_HOLYSHIELD == 0x75,
              "`mov edx, 75h` at 0x6FB0ED9D -- the skill the shield line "
              "adds; its STATE_HOLYSHIELD is already pinned in char.cpp");
static_assert(STAT_MINDAMAGE == 0x15 && STAT_MAXDAMAGE == 0x16
                  && STAT_SECONDARY_MINDAMAGE == 0x17
                  && STAT_SECONDARY_MAXDAMAGE == 0x18,
              "the four stats pushed at 0x6FB0F172, 0x6FB0F17B, "
              "0x6FB0F180 and 0x6FB0F189");

//  Fields, values and strings of the tooltip drawers' guard chains, the two dispatch tables' bounds and the finishing-move tooltip.
static_assert(offsetof(D2SkillsTxt, wSkillDesc) == 0x194,
              "`movsx eax, word ptr [eax+194h]` at 0x6FB159F5, 0x6FB15B7D "
              "and 0x6FB1626C -- the skilldesc.txt row a skills.txt row "
              "points at");
static_assert(offsetof(D2SkillDescTxt, wStrShort) == 0x0A
                  && offsetof(D2SkillDescTxt, wStrAlt) == 0x0E
                  && offsetof(D2SkillDescTxt, wDescDam) == 0x12
                  && offsetof(D2SkillDescTxt, wDescAtt) == 0x14,
              "the finishing move's short string, the title, and the two "
              "dispatch columns -- `[eax+0Ah]` at 0x6FB15A21, `[eax+0Eh]` at "
              "0x6FB15C1F, `[ecx+12h]` at 0x6FB15CBE and `[ecx+14h]` at "
              "0x6FB15CD8");
static_assert(PCLASS_ASSASSIN == 6 && offsetof(D2UnitStrc, dwClassId) == 0x04
                  && UNIT_PLAYER == 0,
              "`cmp dword ptr [eax], 0` / `cmp dword ptr [eax+4], 6` at "
              "0x6FB159B5 / 0x6FB159BA -- the finishing-move clause, skill 0 "
              "of an Assassin");
static_assert(SKILLUSTATE_USABLE == 0 && SKILLUSTATE_NOMANA == 1
                  && SKILLUSTATE_AURA == 6 && SKILLUSTATE_BLOCKED == 8,
              "the four states that let both dispatches run -- the switch "
              "behind `cmp eax, 8; ja` at 0x6FB15C9D and the case table at "
              "0x6FB16184");
static_assert(STR_IDX_22306_Eskillfinishmove == 22306
                  && STR_IDX_4061_strchrskm == 4061
                  && STR_IDX_4063_strchratr == 4063
                  && STR_IDX_4065_strchrrat == 4065,
              "the Assassin's closing line and the two attack lines -- "
              "`mov ecx, 5722h` at 0x6FB159C0, `mov ecx, 0FDDh` at 0x6FB15D00, "
              "`0FDFh` at 0x6FB15E19 and `0FE1h` at 0x6FB15E34");
static_assert(D2FONT_FONT8 == 0,
              "`cmp esi, 3E8h` at 0x6FB16124 and `xor ecx, ecx` at "
              "0x6FB16133 -- SetFont(nSingleValue < 1000 ? FONT16 : FONT8), "
              "FONT16 pinned in batch 4c");


//1.10f: D2Client.0x6FB0A4A0
//  descatt handler 1: entry 1 of gpSkillDescAttFuncs, the 18-entry table indexed by the descatt column, dispatched by SKILLDESC_DrawSkillInfo.
//  One tail call: SKILLDESC_ComputeAttackRating with the left-hand weapon (or NULL), whose answer lands in the A pair. The B pair is untouched,
//  which is how SKILLDESC_DrawSkillInfo sees there is no second figure. void: every descatt handler drops the return; pSkillsTxt is not read.
void __fastcall SKILLDESC_AttFunc1(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnAttackRatingA, int32_t *pnColorA, int32_t *pnAttackRatingB, int32_t *pnColorB)
{
    D2UnitStrc *pLeftHandWeapon;

    pLeftHandWeapon = NULL;
    if (pUnit->pInventory)
    {
        pLeftHandWeapon = D2COMMON_INVENTORY_GetLeftHandWeapon(pUnit->pInventory);
    }
    *pnAttackRatingA = 0;
    *pnColorA = 0;
    SKILLDESC_ComputeAttackRating(pUnit, pnAttackRatingA, pnColorA, nSkillLevel, pSkill, pLeftHandWeapon);
}


//1.10f: D2Client.0x6FB0A4E0
//  The attack rating of the skill panel, the counterpart of SKILLDESC_AddWeaponPhysDamage on the damage side (same out-parameters and colour).
//      base    = UNITS_GetAttackRate(pUnit)
//      weapon  = pWeapon, else INVENTORY_GetLeftHandWeapon(pUnit->pInventory)
//      percent = STAT_ITEM_TOHIT_PERCENT + SKILLS_GetWeaponMasteryBonus(pUnit, weapon, pSkill, 0)
//                + SKILLS_GetToHitFactor(pUnit, nSkillId, nSkillLevel)
//                + STAT_PROGRESSIVE_TOHIT, only for a finishing skill (dwFlags[0] & gdwBitMasks[3]); it is summed into nPercent directly
//      result  = base + percent * base / 100
//  A throwing potion in hand (ITEMTYPE_MISSILE_POTION) zeroes both the rating and the colour. The mastery type is 0 (melee to-hit) where the
//  damage side passes 1, and the mastery is added unconditionally (SKILLDESC_AttFunc3 gates it on STATE_THROWINGMASTERY). The state
//  overrides come last: AttBlue forces colour 3, then AttRed forces 1, so red wins.
int32_t __fastcall SKILLDESC_ComputeAttackRating(D2UnitStrc *pUnit, int32_t *pnAttackRating, int32_t *pnColor, int32_t nSkillLevel, D2SkillStrc *pSkill, D2UnitStrc *pWeapon)
{
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pWeaponUsed;
    int32_t nWeaponMasteryBonus;
    int32_t nToHitSubtotal;
    int32_t nPercent;
    int32_t nProgressiveToHit;

    if (!pUnit)
    {
        return 0;
    }
    if (!pSkill)
    {
        return 0;
    }
    pSkillsTxt = D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill(pSkill);
    if (!pSkillsTxt)
    {
        return 0;
    }
    *pnColor = 0;
    *pnAttackRating = D2COMMON_UNITS_GetAttackRate(pUnit);
    pWeaponUsed = pWeapon;
    if ((pWeapon || (pUnit->pInventory && (pWeaponUsed = D2COMMON_INVENTORY_GetLeftHandWeapon(pUnit->pInventory)) != NULL)) && D2COMMON_ITEMS_GetItemType(pWeaponUsed) == ITEMTYPE_MISSILE_POTION)
    {
        *pnAttackRating = 0;
        *pnColor = 0;
    }
    nWeaponMasteryBonus = D2COMMON_SKILLS_GetWeaponMasteryBonus(pUnit, pWeaponUsed, pSkill, 0);
    nToHitSubtotal = D2COMMON_STATLIST_UnitGetItemStatOrSkillStatValue(pUnit, STAT_ITEM_TOHIT_PERCENT, 0) + nWeaponMasteryBonus;
    nPercent = D2COMMON_SKILLS_GetToHitFactor(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel) + nToHitSubtotal;
    if ((((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_FINISHING >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_FINISHING & 7]) != 0)
    {
        nProgressiveToHit = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_PROGRESSIVE_TOHIT, 0);
        if (nProgressiveToHit)
        {
            nPercent += nProgressiveToHit;
            *pnColor = 3;
        }
    }
    *pnAttackRating += nPercent * *pnAttackRating / 100;
    if (D2COMMON_STATES_CheckStateMaskAttBlueOnUnit(pUnit))
    {
        *pnColor = 3;
    }
    if (D2COMMON_STATES_CheckStateMaskAttRedOnUnit(pUnit))
    {
        *pnColor = 1;
    }
    return 1;
}


//1.10f: D2Client.0x6FB0A610
//  descatt handler 5. The one AttFunc that merges the weapon's statlist into the unit before it measures, and takes it back out afterwards:
//      owner = STATLIST_GetOwner(weapon, &bStatNotDynamic)
//      STATLIST_MergeStatLists(pUnit, weapon, 0)
//      rating = UNITS_GetAttackRate(pUnit)
//      owner ? MergeStatLists(pUnit, weapon, bStatNotDynamic) : ExpireUnitStatlist(pUnit, weapon)
//  With no weapon in hand the rating is the unit's own. The rest is SKILLDESC_ComputeAttackRating's arithmetic without the weapon parameter (the
//  same to-hit percentage, the same finishing gate with colour 3, the same two state overrides). pSkill is only tested; the to-hit factor
//  uses pSkillsTxt's nSkillId.
void __fastcall SKILLDESC_AttFunc5(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnAttackRatingA, int32_t *pnColorA, int32_t *pnAttackRatingB, int32_t *pnColorB)
{
    D2UnitStrc *pLeftHandWeapon;
    int32_t *pnAttackRatingOut;
    D2UnitStrc *pOwner;
    BOOL bStatNotDynamic;
    int32_t nToHitSubtotal;
    int32_t nPercent;
    int32_t nProgressiveToHit;

    pLeftHandWeapon = NULL;
    if (pUnit->pInventory)
    {
        pLeftHandWeapon = D2COMMON_INVENTORY_GetLeftHandWeapon(pUnit->pInventory);
    }
    pnAttackRatingOut = pnAttackRatingA;
    *pnAttackRatingA = 0;
    *pnColorA = 0;
    if (!pSkill)
    {
        return;
    }
    if (pLeftHandWeapon)
    {
        bStatNotDynamic = 0;
        pOwner = D2COMMON_STATLIST_GetOwner(pLeftHandWeapon, &bStatNotDynamic);
        D2COMMON_STATLIST_MergeStatLists(pUnit, pLeftHandWeapon, 0);
        *pnAttackRatingOut = D2COMMON_UNITS_GetAttackRate(pUnit);
        if (pOwner)
        {
            D2COMMON_STATLIST_MergeStatLists(pUnit, pLeftHandWeapon, bStatNotDynamic);
        }
        else
        {
            D2COMMON_STATLIST_ExpireUnitStatlist(pUnit, pLeftHandWeapon);
        }
    }
    else
    {
        *pnAttackRatingOut = D2COMMON_UNITS_GetAttackRate(pUnit);
    }
    nToHitSubtotal = D2COMMON_STATLIST_UnitGetItemStatOrSkillStatValue(pUnit, STAT_ITEM_TOHIT_PERCENT, 0);
    nPercent = D2COMMON_SKILLS_GetToHitFactor(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel) + nToHitSubtotal;
    if ((((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_FINISHING >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_FINISHING & 7]) != 0)
    {
        nProgressiveToHit = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_PROGRESSIVE_TOHIT, 0);
        if (nProgressiveToHit)
        {
            nPercent += nProgressiveToHit;
            *pnColorA = 3;
        }
    }
    *pnAttackRatingOut += nPercent * *pnAttackRatingOut / 100;
    if (D2COMMON_STATES_CheckStateMaskAttBlueOnUnit(pUnit))
    {
        *pnColorA = 3;
    }
    if (D2COMMON_STATES_CheckStateMaskAttRedOnUnit(pUnit))
    {
        *pnColorA = 1;
    }
}


//1.10f: D2Client.0x6FB0A730
//  descatt handler 2. Both exits are tail calls: the walker when the character can dual wield, the single-weapon routine when it cannot
//  (SKILLDESC_AttFunc1 is the unconditional single-weapon version). The walk's pSkill is the skill, and nExtraB is the level.
void __fastcall SKILLDESC_AttFunc2(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnAttackRatingA, int32_t *pnColorA, int32_t *pnAttackRatingB, int32_t *pnColorB)
{
    D2UnitStrc *pLeftHandWeapon;

    if (D2COMMON_UNITS_CanDualWield(pUnit))
    {
        SKILLDESC_ForEachWieldedWeapon(pUnit, pnAttackRatingA, pnColorA, 0, pnAttackRatingB, pnColorB, 0, pSkill, SKILLDESC_AttackRatingCallback, 0, nSkillLevel);
    }
    else
    {
        pLeftHandWeapon = NULL;
        if (pUnit->pInventory)
        {
            pLeftHandWeapon = D2COMMON_INVENTORY_GetLeftHandWeapon(pUnit->pInventory);
        }
        *pnAttackRatingA = 0;
        *pnColorA = 0;
        SKILLDESC_ComputeAttackRating(pUnit, pnAttackRatingA, pnColorA, nSkillLevel, pSkill, pLeftHandWeapon);
    }
}


//1.10f: D2Client.0x6FB0A7B0
//  The nine-argument adapter: five slots dropped, the rest forwarded to SKILLDESC_ComputeAttackRating. It is only ever a callback (handed to the
//  walker by SKILLDESC_AttFunc2 and SKILLDESC_AttFunc4); SKILLDESC_AddTotalDamageCallback is the other implementation of the same signature.
//  void: no caller reads what the original leaves in eax.
void __fastcall SKILLDESC_AttackRatingCallback(D2UnitStrc *pUnit, int32_t *pnValue, int32_t *pnColor, int32_t nUnusedA, int32_t nUnusedB, int32_t nSkillLevel, D2SkillStrc *pSkill, int32_t nUnusedC, D2UnitStrc *pWeapon)
{
    SKILLDESC_ComputeAttackRating(pUnit, pnValue, pnColor, nSkillLevel, pSkill, pWeapon);
}


//1.10f: D2Client.0x6FB0A7D0
//  The dual-wield walker: runs a per-weapon computation once or twice so the panel can show two figures side by side.
//      !CanDualWield                                  -> return 0, nothing written
//      both hands equippable, neither a throwing potion -> the callback twice, B then A
//      otherwise                                      -> one call, A, no weapon
//  The two-handed route goes through SKILLDESC_PrepareOffHandWeapon for the B pair and SKILLDESC_PrepareMainHandWeapon for A, and zeroes the
//  pair whose helper returned NULL. The single-hand route merges the left-hand weapon's statlist with mode 1 when it is equippable and calls
//  the callback with no weapon. pfnCompute is SKILLDESC_AttackRatingCallback, SKILLDESC_AddTotalDamageCallback or SKILLDESC_ComputeThrowDamage; it
//  receives (pUnit, pair, nArg, nExtraA, nExtraB, pSkill, 0, weapon), nExtraA and nExtraB carrying the caller's own meaning. A throwing potion does
//  not count as a wielded weapon. bCanDualWield holds UNITS_CanDualWield; the last return is reached only when the unit cannot dual wield.
int32_t __fastcall SKILLDESC_ForEachWieldedWeapon(D2UnitStrc *pUnit, int32_t *pnValueA, int32_t *pnColorA, int32_t *pnArgA, int32_t *pnValueB, int32_t *pnColorB, int32_t *pnArgB, D2SkillStrc *pSkill, void *pfnCompute, int32_t nExtraA, int32_t nExtraB)
{
    D2InventoryStrc *pInventory;
    int32_t bCanDualWield;
    D2UnitStrc *pRightHand;
    D2UnitStrc *pLeftHand;
    D2UnitStrc *pOffHandWeapon;
    D2UnitStrc *pMainHandWeapon;
    D2UnitStrc *pLeftHandWeapon;

    pInventory = pUnit->pInventory;
    if (!pInventory)
    {
        return 0;
    }
    bCanDualWield = D2COMMON_UNITS_CanDualWield(pUnit);
    if (!bCanDualWield)
    {
        return bCanDualWield;
    }

    pRightHand = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RARM);
    if (!D2COMMON_ITEMS_CanBeEquipped(pRightHand) || D2COMMON_ITEMS_CheckItemTypeId(pRightHand, ITEMTYPE_MISSILE_POTION))
    {
        pRightHand = NULL;
    }
    pLeftHand = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_LARM);
    if (!D2COMMON_ITEMS_CanBeEquipped(pLeftHand) || D2COMMON_ITEMS_CheckItemTypeId(pLeftHand, ITEMTYPE_MISSILE_POTION))
    {
        pLeftHand = NULL;
    }
    if (pRightHand && pLeftHand)
    {
        //  B first (the off hand)
        pOffHandWeapon = SKILLDESC_PrepareOffHandWeapon(pUnit);
        if (pOffHandWeapon)
        {
            ((void(__fastcall *)(D2UnitStrc *, int32_t *, int32_t *, int32_t *, int32_t, int32_t, D2SkillStrc *, int32_t, D2UnitStrc *))pfnCompute)(pUnit, pnValueB, pnColorB, pnArgB, nExtraA, nExtraB, pSkill, 0, pOffHandWeapon);
        }
        else
        {
            *pnValueB = 0;
            *pnColorB = 0;
        }
        pMainHandWeapon = SKILLDESC_PrepareMainHandWeapon(pUnit);
        if (pMainHandWeapon)
        {
            ((void(__fastcall *)(D2UnitStrc *, int32_t *, int32_t *, int32_t *, int32_t, int32_t, D2SkillStrc *, int32_t, D2UnitStrc *))pfnCompute)(pUnit, pnValueA, pnColorA, pnArgA, nExtraA, nExtraB, pSkill, 0, pMainHandWeapon);
        }
        else
        {
            *pnValueA = 0;
            *pnColorA = 0;
        }
        return 1;
    }

    pLeftHandWeapon = D2COMMON_INVENTORY_GetLeftHandWeapon(pInventory);
    if (pLeftHandWeapon && D2COMMON_ITEMS_CanBeEquipped(pLeftHandWeapon))
    {
        D2COMMON_STATLIST_MergeStatLists(pUnit, pLeftHandWeapon, 1);
    }
    ((void(__fastcall *)(D2UnitStrc *, int32_t *, int32_t *, int32_t *, int32_t, int32_t, D2SkillStrc *, int32_t, D2UnitStrc *))pfnCompute)(pUnit, pnValueA, pnColorA, pnArgA, nExtraA, nExtraB, pSkill, 0, 0);
    return 1;
}


//1.10f: D2Client.0x6FB0A950
//  Returns the other hand's weapon and merges the two statlists so a per-hand figure can be computed against it; NULL when the character is not
//  dual wielding, holds no weapon, or the other hand is not an equippable weapon. The other hand is (nBodyLoc == BODYLOC_RARM) + BODYLOC_RARM.
//  Merge order: the primary first with mode 0, then the other hand with mode 1; SKILLDESC_PrepareMainHandWeapon merges them the other way round.
//  Takes one argument (retn 0).
D2UnitStrc *__fastcall SKILLDESC_PrepareOffHandWeapon(D2UnitStrc *pUnit)
{
    D2InventoryStrc *pInventory;
    D2UnitStrc *pPrimary;
    uint8_t nBodyLocation;
    D2UnitStrc *pOther;

    if (!D2COMMON_UNITS_CanDualWield(pUnit))
    {
        return NULL;
    }
    pInventory = pUnit->pInventory;
    if (!pInventory)
    {
        return NULL;
    }
    pPrimary = D2COMMON_INVENTORY_GetLeftHandWeapon(pUnit->pInventory);
    if (!pPrimary)
    {
        return NULL;
    }
    nBodyLocation = D2COMMON_ITEMS_GetBodyLocation(pPrimary);
    pOther = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, (D2C_PlayerBodyLocs)((nBodyLocation == BODYLOC_RARM) + BODYLOC_RARM));
    if (!pOther || pOther == pPrimary || !D2COMMON_ITEMS_CheckItemTypeId(pOther, ITEMTYPE_WEAPON) || !D2COMMON_ITEMS_CanBeEquipped(pPrimary) || !D2COMMON_ITEMS_CanBeEquipped(pOther))
    {
        return NULL;
    }
    if (D2COMMON_STATLIST_GetOwner(pPrimary, NULL))
    {
        D2COMMON_STATLIST_MergeStatLists(pUnit, pPrimary, 0);
    }
    if (D2COMMON_STATLIST_GetOwner(pOther, NULL))
    {
        D2COMMON_STATLIST_MergeStatLists(pUnit, pOther, 1);
    }
    return pOther;
}


//1.10f: D2Client.0x6FB0AA00
//  The mirror of SKILLDESC_PrepareOffHandWeapon: the primary weapon comes back, with the other hand merged first with mode 0 and the primary
//  second with mode 1. More forgiving than its twin: where the off-hand version returns NULL as soon as anything is missing, this one falls
//  through to merging just the primary (mode 1) and returning it. The two-hand merge happens only when both hold weapons and both pass
//  ITEMS_CanBeEquipped. Takes one argument (retn 0).
D2UnitStrc *__fastcall SKILLDESC_PrepareMainHandWeapon(D2UnitStrc *pUnit)
{
    D2InventoryStrc *pInventory;
    D2UnitStrc *pPrimary;
    uint8_t nBodyLocation;
    D2UnitStrc *pOther;
    BOOL bOtherIsWeapon;

    if (!D2COMMON_UNITS_CanDualWield(pUnit))
    {
        return NULL;
    }
    pInventory = pUnit->pInventory;
    if (!pInventory)
    {
        return NULL;
    }
    pPrimary = D2COMMON_INVENTORY_GetLeftHandWeapon(pUnit->pInventory);
    if (!pPrimary)
    {
        return NULL;
    }
    nBodyLocation = D2COMMON_ITEMS_GetBodyLocation(pPrimary);
    pOther = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, (D2C_PlayerBodyLocs)((nBodyLocation == BODYLOC_RARM) + BODYLOC_RARM));
    if (pOther && pOther != pPrimary)
    {
        bOtherIsWeapon = D2COMMON_ITEMS_CheckItemTypeId(pOther, ITEMTYPE_WEAPON);
        if (bOtherIsWeapon && D2COMMON_ITEMS_CanBeEquipped(pPrimary) && D2COMMON_ITEMS_CanBeEquipped(pOther))
        {
            D2COMMON_STATLIST_MergeStatLists(pUnit, pOther, 0);
            D2COMMON_STATLIST_MergeStatLists(pUnit, pPrimary, 1);
            return pPrimary;
        }
    }

    //  the fall-through every other path takes: the primary alone, mode 1
    if (D2COMMON_ITEMS_CanBeEquipped(pPrimary))
    {
        D2COMMON_STATLIST_MergeStatLists(pUnit, pPrimary, 1);
    }
    return pPrimary;
}


//1.10f: D2Client.0x6FB0AAB0
//  descatt handler 3. The attack rating without the charge-up bonus: no dwFlags[0] & gdwBitMasks[3] test and no STAT_PROGRESSIVE_TOHIT, where
//  SKILLDESC_ComputeAttackRating and SKILLDESC_AttFunc5 add it for a finishing skill. Its mastery is gated the other way: it adds
//  SKILLS_GetWeaponMasteryBonus(pUnit, weapon, pSkill, 0) only while the unit has STATE_THROWINGMASTERY. A throwing potion zeroes the rating and
//  the colour. The colour is re-zeroed after the rating is computed, then the two state overrides run, AttBlue first and AttRed last.
void __fastcall SKILLDESC_AttFunc3(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnAttackRatingA, int32_t *pnColorA, int32_t *pnAttackRatingB, int32_t *pnColorB)
{
    D2UnitStrc *pLeftHandWeapon;
    D2SkillsTxt *pSkillsTxtFromSkill;
    int32_t nWeaponMasteryBonus;
    int32_t nToHitSubtotal;

    *pnAttackRatingA = 0;
    *pnColorA = 0;
    pLeftHandWeapon = NULL;
    if (!pSkill)
    {
        return;
    }
    pSkillsTxtFromSkill = D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill(pSkill);
    if (!pSkillsTxtFromSkill)
    {
        return;
    }
    *pnAttackRatingA = D2COMMON_UNITS_GetAttackRate(pUnit);
    if (pUnit->pInventory)
    {
        pLeftHandWeapon = D2COMMON_INVENTORY_GetLeftHandWeapon(pUnit->pInventory);
        if (pLeftHandWeapon)
        {
            if (D2COMMON_ITEMS_GetItemType(pLeftHandWeapon) == ITEMTYPE_MISSILE_POTION)
            {
                *pnAttackRatingA = 0;
                *pnColorA = 0;
            }
        }
    }
    nWeaponMasteryBonus = 0;
    if (D2COMMON_STATES_CheckState(pUnit, STATE_THROWINGMASTERY))
    {
        nWeaponMasteryBonus = D2COMMON_SKILLS_GetWeaponMasteryBonus(pUnit, pLeftHandWeapon, pSkill, 0);
    }
    nToHitSubtotal = D2COMMON_STATLIST_UnitGetItemStatOrSkillStatValue(pUnit, STAT_ITEM_TOHIT_PERCENT, 0) + nWeaponMasteryBonus;
    *pnAttackRatingA += (D2COMMON_SKILLS_GetToHitFactor(pUnit, (D2C_Skills)pSkillsTxtFromSkill->nSkillId, nSkillLevel) + nToHitSubtotal) * *pnAttackRatingA / 100;
    *pnColorA = 0;
    if (D2COMMON_STATES_CheckStateMaskAttBlueOnUnit(pUnit))
    {
        *pnColorA = 3;
    }
    if (D2COMMON_STATES_CheckStateMaskAttRedOnUnit(pUnit))
    {
        *pnColorA = 1;
    }
}


//1.10f: D2Client.0x6FB0ABC0
//  descatt handler 4. The walker, then the B pair collapsed into A and cleared: the panel ends up with one figure from the other hand
//  (SKILLDESC_PrepareOffHandWeapon runs first, into B) and no second line.
void __fastcall SKILLDESC_AttFunc4(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnAttackRatingA, int32_t *pnColorA, int32_t *pnAttackRatingB, int32_t *pnColorB)
{
    SKILLDESC_ForEachWieldedWeapon(pUnit, pnAttackRatingA, pnColorA, 0, pnAttackRatingB, pnColorB, 0, pSkill, SKILLDESC_AttackRatingCallback, 0, nSkillLevel);
    *pnAttackRatingA = *pnAttackRatingB;
    *pnColorA = *pnColorB;
    *pnAttackRatingB = 0;
    *pnColorB = 0;
}


//1.10f: D2Client.0x6FB0AC10
//  Draws the skill panel's two damage lines, one above the other, in font 6, each in its own colour.
//      line 1 at nY + gnSkillDescLine1YOffset   -7 for CJK, -6 otherwise
//      line 2 at nY + gnSkillDescLine2YOffset   read, never written here
//  The first offset is computed once and cached behind gbSkillDescLineOffsetsReady: STRTABLE_GetLanguage() in 6..9 (Japanese, Korean, Chinese
//  simplified and traditional) gets -7, everything else -6. The -7 is written inside the test before the upper bound is compared, so the effect is
//  the closed band 6..9; the language is fetched twice. Both tests are signed (jl at 0x6FB0AC33, jle at 0x6FB0AC47), hence the casts.
void __fastcall SKILLDESC_DrawTwoLines(Unicode *pwszLine1, Unicode *pwszLine2, int32_t nX, int32_t nY, int32_t nRight, int32_t nColor1, int32_t nColor2)
{
    D2C_Language nLanguage;

    D2WIN_10127_SetFont(D2FONT_FONT6);
    if (!gbSkillDescLineOffsetsReady)
    {
        if ((int32_t)D2LANG_STRTABLE_GetLanguage() < (int32_t)LANGUAGE_JAPANESE || (nLanguage = D2LANG_STRTABLE_GetLanguage(), gnSkillDescLine1YOffset = -7, (int32_t)nLanguage > (int32_t)LANGUAGE_CHINESETWN))
        {
            gnSkillDescLine1YOffset = -6;
        }
        gbSkillDescLineOffsetsReady = 1;
    }
    CHAR_DrawTextCentered(gnBorderXDifference + nX, gnSkillDescLine1YOffset + gnScreenHeight + gnBorderYDifference + nY - 480, gnBorderXDifference + nRight, pwszLine1, nColor1);
    CHAR_DrawTextCentered(gnBorderXDifference + nX, gnScreenHeight + gnBorderYDifference + gnSkillDescLine2YOffset + nY - 480, gnBorderXDifference + nRight, pwszLine2, nColor2);
}


//1.10f: D2Client.0x6FB0ACE0
//  descdam handler 6: entry 6 of gpSkillDescDamFuncs (36 entries), dispatched by SKILLDESC_DrawSkillInfo. The one handler that colours its figure
//  by the skill's own nEType and pairs it with the weapon's figure, the same two-figure shape SKILLDESC_DrawDualWieldDamage draws, computed
//  here directly rather than through the walker. The original inlines SKILLDESC_FormatRangeOrValue in the two-line draw.
//      weapon side  AddWeaponPhysDamage + AddItemElemDamage into (nWeaponMinRaw, nWeaponMaxRaw, nWeaponColor), then both ends scaled by srcdam
//                   in 128ths, skipped entirely when srcdam is 0
//      skill side   min/max ElemDamage and PHYS damage off the skill, each >> 8, accumulated and finally folded through SKILLDESC_AddElemDamageByType
//  The element colour map is the panel's, not the throwing potion's: 1 -> 1 RED, 2 -> 9 YELLOW, 3 and 4 -> 3 BLUE, 5 -> 2 LIGHT_GREEN, anything
//  else -> 0 WHITE (lightning is 9 here where SKILLDESC_ComputeThrowDamage writes 4). The three arms, in test order, each ends in a draw:
//      weapon side all zero -> the skill's line, in the nEType colour
//      skill side all zero  -> the weapon's line, in the colour AddWeaponPhysDamage produced
//      otherwise            -> both, weapon first, through SKILLDESC_DrawTwoLines
//  Each single line passes through the family's fit test and its own `if (min >= max) max = min + 1;` clamp.
void __fastcall SKILLDESC_DamFunc6(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    int32_t nWeaponMinRaw;
    int32_t nWeaponMaxRaw;
    int32_t nWeaponMin;
    int32_t nWeaponMax;
    int32_t nWeaponColor;
    int32_t nSrcDam;
    int32_t nSrcDamBase;
    int32_t nSkillMin;
    int32_t nSkillMax;
    int32_t nEType;
    int32_t nDrawColor;
    int32_t nMinClamped;
    int32_t nMaxClamped;
    int32_t nWidth;
    int32_t nHeight;
    int32_t nTextY;
    int32_t nRectTopY;
    int32_t nRightCopy;
    int32_t nLeftCopy;
    Unicode wszText[32];
    Unicode wszWeapon[32];

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }

    nWeaponMin = 0;
    nSrcDam = pSkillsTxt->nSrcDam;
    nWeaponMax = 0;
    nSrcDamBase = nSrcDam;
    nWeaponColor = 0;
    if (nSrcDam)
    {
        nWeaponMinRaw = 0;
        nWeaponMaxRaw = 0;
        SKILLDESC_AddWeaponPhysDamage(pUnit, &nWeaponMinRaw, &nWeaponMaxRaw, &nWeaponColor, 0, 0, pSkill, 128, NULL, 0);
        SKILLDESC_AddItemElemDamage(pUnit, &nWeaponMinRaw, &nWeaponMaxRaw, &nWeaponColor, 0, pSkill);
        nWeaponMin = nSrcDamBase * nWeaponMinRaw / 128;
        nWeaponMax = nSrcDamBase * nWeaponMaxRaw / 128;
    }

    nSkillMin = 0;
    nEType = pSkillsTxt->nEType;
    nSkillMax = 0;
    switch (nEType)
    {
    case 1:
        nDrawColor = 1;
        break;
    case 2:
        nDrawColor = 9;
        break;
    case 3:
    case 4:
        nDrawColor = 3;
        break;
    case 5:
        nDrawColor = 2;
        break;
    default:
        nDrawColor = 0;
        break;
    }
    nSkillMin += D2COMMON_SKILLS_GetMinPhysDamage(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel, 0) >> 8;
    nSkillMax += D2COMMON_SKILLS_GetMaxPhysDamage(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel, 0) >> 8;
    nSkillMin += D2COMMON_SKILLS_GetMinElemDamage(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel, 1) >> 8;
    nSkillMax += D2COMMON_SKILLS_GetMaxElemDamage(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel, 1) >> 8;
    SKILLDESC_AddElemDamageByType(pUnit, &nSkillMin, &nSkillMax, nEType);

    //  weapon side empty: the skill's own line, in the nEType colour
    if (!nWeaponMin && !nWeaponMax)
    {
        nMaxClamped = nSkillMax;
        nTextY = nY;
        nMinClamped = nSkillMin;
        if (nSkillMin >= nSkillMax)
        {
            nMaxClamped = nSkillMin + 1;
        }
        SKILLDESC_FormatRangeOrValue(wszText, nMinClamped, nMaxClamped, 0);
        nWidth = 0;
        nHeight = 0;
        D2WIN_10131_GetTextDimensions(wszText, &nWidth, &nHeight);
        nRightCopy = nRight;
        nLeftCopy = nLeft;
        nWidth = 11 * nWidth / 7;
        if (nWidth <= nRight - nLeft)
        {
            D2WIN_10127_SetFont(D2FONT_FONT16);
            nRectTopY = nTextY;
        }
        else
        {
            D2WIN_10127_SetFont(D2FONT_FONT6);
            nRectTopY = nY - 1;
        }
        CHAR_DrawTextCentered(gnBorderXDifference + nLeftCopy, gnScreenHeight + gnBorderYDifference + nRectTopY - 480, gnBorderXDifference + nRightCopy, wszText, nDrawColor);
        return;
    }

    //  skill side empty: the weapon's line, in the weapon's own colour
    if (!nSkillMin && !nSkillMax)
    {
        nRectTopY = nY;
        nDrawColor = nWeaponColor;
        nMaxClamped = nWeaponMax;
        if (nWeaponMin >= nWeaponMax)
        {
            nMaxClamped = nWeaponMin + 1;
        }
        SKILLDESC_FormatRangeOrValue(wszText, nWeaponMin, nMaxClamped, 0);
        nWidth = 0;
        nHeight = 0;
        D2WIN_10131_GetTextDimensions(wszText, &nWidth, &nHeight);
        nRightCopy = nRight;
        nLeftCopy = nLeft;
        nWidth = 11 * nWidth / 7;
        if (nWidth <= nRight - nLeft)
        {
            D2WIN_10127_SetFont(D2FONT_FONT16);
        }
        else
        {
            D2WIN_10127_SetFont(D2FONT_FONT6);
            nRectTopY = nY - 1;
        }
        CHAR_DrawTextCentered(gnBorderXDifference + nLeftCopy, gnScreenHeight + gnBorderYDifference + nRectTopY - 480, gnBorderXDifference + nRightCopy, wszText, nDrawColor);
        return;
    }

    //  both sides: the weapon is line 1 and the skill is line 2
    SKILLDESC_FormatRangeOrValue(wszWeapon, nWeaponMin, nWeaponMax, 0);
    SKILLDESC_FormatRangeOrValue(wszText, nSkillMin, nSkillMax, 0);
    SKILLDESC_DrawTwoLines(wszWeapon, wszText, nLeft, nY, nRight, nWeaponColor, nDrawColor);
}


//1.10f: D2Client.0x6FB0B0F0
//  "%d", or "+%d" when bSigned is set and the value is not negative (a negative value keeps its own minus sign). Called by the dispatcher and
//  by some descdam handlers.
int32_t __fastcall SKILLDESC_FormatInt(struct Unicode *pwszOut, int32_t nValue, int32_t bSigned)
{
    char src[32];

    if (bSigned && nValue >= 0)
    {
        wsprintfA(src, "+%d", nValue);
    }
    else
    {
        wsprintfA(src, "%d", nValue);
    }
    Unicode::toUnicode(pwszOut, src, 32);
    return 1;
}


//1.10f: D2Client.0x6FB0B140
//  A damage range as text, "min-max", for the description lines.
//      bAllowK, nMin >= 10000   "%s%dK-%dK", both bounds (x + 500) / 1000   (0x6FB0B1B1..0x6FB0B202)
//      bAllowK, nMax >= 10000   "%s%d-%dK", nMin as it is, nMax divided     (0x6FB0B16D..0x6FB0B1A3)
//      otherwise                "+%d-%d" when bSigned, else "%d-%d"
//  %s is gszPlus ("+") when bSigned and gszEmptyString otherwise, chosen in each K arm on its own. The tests are signed (jge / jl against 2710h,
//  0x6FB0B160, 0x6FB0B167), and so is the division (imul by 10624DD3h, sar edx, 6, the sign bit added: truncation towards zero). The text is
//  built by wsprintfA into 32 bytes and widened by Unicode::toUnicode with a count of 32; the longest the formats can make is 26.
//  void: eax is left holding what toUnicode returns, wszOut, and no caller reads it.
void __fastcall SKILLDESC_FormatDamageRange(struct Unicode *wszOut, int32_t nMin, int32_t nMax, int32_t bSigned, int32_t bAllowK)
{
    const char *pszSignPrefix;
    char szText[32];

    if (bAllowK && nMin >= 10000)
    {
        pszSignPrefix = bSigned ? gszPlus : &gszEmptyString;
        wsprintfA(szText, "%s%dK-%dK", pszSignPrefix, (nMin + 500) / 1000, (nMax + 500) / 1000);
    }
    else if (bAllowK && nMax >= 10000)
    {
        pszSignPrefix = bSigned ? gszPlus : &gszEmptyString;
        wsprintfA(szText, "%s%d-%dK", pszSignPrefix, nMin, (nMax + 500) / 1000);
    }
    else if (bSigned)
    {
        wsprintfA(szText, "+%d-%d", nMin, nMax);
    }
    else
    {
        wsprintfA(szText, "%d-%d", nMin, nMax);
    }
    Unicode::toUnicode(wszOut, szText, 32);
}


//1.10f: D2Client.0x6FB0B250
//  Writes "min-max" through SKILLDESC_FormatDamageRange, or a single "%d" / "+%d" when the two ends are equal. The two arms differ in more than
//  the format: the range arm passes nAllowK = 1, so a large range gets the thousands forms, while the collapsed single value never does. Same
//  sign rule as SKILLDESC_FormatInt. The original inlines SKILLDESC_FormatInt in the single-value arm.
int32_t __fastcall SKILLDESC_FormatRangeOrValue(struct Unicode *pwszOut, int32_t nMin, int32_t nMax, int32_t bSigned)
{
    if (nMin == nMax)
    {
        SKILLDESC_FormatInt(pwszOut, nMin, bSigned);
        return 1;
    }
    SKILLDESC_FormatDamageRange(pwszOut, nMin, nMax, bSigned, 1);
    return 1;
}


//1.10f: D2Client.0x6FB0B2C0
//  The physical half of the skill panel's damage figure: the weapon's damage scaled by the skill's srcdam, with every percentage the character
//  carries applied on top. It is the shared "what would this skill hit for" routine of the descdam handlers and the description builder.
//      weapon   = pWeapon, else INVENTORY_GetLeftHandWeapon(pUnit->pInventory)
//      min/max  = bUseItemOwnStats ? the item's own mindamage/maxdamage : wield type 2 ? SECONDARY_MIN/MAXDAMAGE : MIN/MAXDAMAGE
//      bare-handed: the unit's MIN/MAXDAMAGE + 1 and + 2
//      scale    = nSrcDamOverride, else pSkillsTxt->nSrcDam, applied as `value * nSrcDamPct / 128` and skipped when it is 128
//      clamps   min at >= 1 and max at >= 2 before accumulating
//      percent  = STAT_DAMAGEPERCENT + nExtraDamagePercent + SKILLS_GetWeaponMasteryBonus(pUnit, weapon, pSkill, 1) + the charge-up bonus
//                 + strbonus * STRENGTH / 100 and dexbonus * DEXTERITY / 100 (with no weapon, simply + STRENGTH)
//      clamped at >= -90, so damage can fall to a tenth but no further
//      final    *pnMin = nFlatBonus + nNormalDamage + *pnMin * (STAT_ITEM_MINDAMAGE_PERCENT + percent + 100) / 100
//  nSrcDamPct is a fraction of the weapon's damage in 128ths: 128 is 100 per cent and is the "leave it alone" test. The trailing +100 in the
//  factor is real (lea ecx, [ecx+edi+64h] at 0x6FB0B524, then the 51EB851Fh / sar edx, 5 divide by 100). pnColor is the family's ui_color_t
//  out-parameter: the charge-up branch sets 3 (blue), then the two states override it, 3 for DamBlue and 1 (red) for DamRed, so red wins. The
//  charge-up bonus is gated on dwFlags[0] & gdwBitMasks[3] (SKILLSFLAGINDEX_FINISHING, the Assassin finishing moves). bUseItemOwnStats reads
//  the item's own stats through D2Common_11248 rather than the unit's, which is what lets a dual-wielded off hand read its own record.
int32_t __fastcall SKILLDESC_AddWeaponPhysDamage(D2UnitStrc *pUnit, int32_t *pnMin, int32_t *pnMax, int32_t *pnColor, int32_t nExtraDamagePercent, int32_t nFlatBonus, D2SkillStrc *pSkill, int32_t nSrcDamOverride,
                                             D2UnitStrc *pWeapon, int32_t bUseItemOwnStats)
{
    D2InventoryStrc *pInventory;
    D2UnitStrc *pWeaponUsed;
    D2SkillsTxt *pSkillsTxt;
    int32_t nMinDamage;
    int32_t nMaxDamage;
    int32_t nSrcDamPct;
    int32_t nDamagePercentBase;
    int32_t nDamagePercent;
    int32_t nItemMinDamagePercent;
    int32_t nItemMaxDamagePercent;
    int32_t nNormalDamage;
    int32_t nChargeUpPercent;
    int32_t nStrengthBonus;
    int32_t nDexBonus;

    if (!pSkill)
    {
        return 0;
    }
    pSkillsTxt = D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill(pSkill);
    if (!pSkillsTxt)
    {
        return 0;
    }
    pInventory = pUnit->pInventory;
    if (!pInventory)
    {
        return 0;
    }

    pWeaponUsed = pWeapon;
    if (pWeapon || (pWeaponUsed = D2COMMON_INVENTORY_GetLeftHandWeapon(pInventory)) != NULL)
    {
        D2COMMON_DATATBLS_GetItemsTxtRecord(pWeaponUsed->dwClassId);
        if (bUseItemOwnStats)
        {
            nMinDamage = D2COMMON_11248(pUnit, pWeaponUsed, STAT_MINDAMAGE);
            nMaxDamage = D2COMMON_11248(pUnit, pWeaponUsed, STAT_MAXDAMAGE);
        }
        else if (D2COMMON_INVENTORY_GetWieldType(pUnit, pInventory) == 2)
        {
            nMinDamage = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_SECONDARY_MINDAMAGE, 0);
            nMaxDamage = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_SECONDARY_MAXDAMAGE, 0);
        }
        else
        {
            nMinDamage = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_MINDAMAGE, 0);
            nMaxDamage = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_MAXDAMAGE, 0);
        }
    }
    else
    {
        nMinDamage = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_MINDAMAGE, 0) + 1;
        nMaxDamage = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_MAXDAMAGE, 0) + 2;
    }

    nSrcDamPct = nSrcDamOverride;
    if (!nSrcDamOverride)
    {
        nSrcDamPct = pSkillsTxt->nSrcDam;
    }
    if (nSrcDamPct != 128)
    {
        nMinDamage = nSrcDamPct * nMinDamage / 128;
        nMaxDamage = nSrcDamPct * nMaxDamage / 128;
    }
    if (nSrcDamPct)
    {
        if (nMinDamage < 1)
        {
            nMinDamage = 1;
        }
        *pnMin += nMinDamage;
        if (nMaxDamage < 2)
        {
            nMaxDamage = 2;
        }
        *pnMax += nMaxDamage;
    }

    nDamagePercentBase = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_DAMAGEPERCENT, 0) + nExtraDamagePercent;
    nDamagePercent = D2COMMON_SKILLS_GetWeaponMasteryBonus(pUnit, pWeaponUsed, pSkill, 1) + nDamagePercentBase;
    nItemMinDamagePercent = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_ITEM_MINDAMAGE_PERCENT, 0);
    nItemMaxDamagePercent = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_ITEM_MAXDAMAGE_PERCENT, 0);

    if ((((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_FINISHING >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_FINISHING & 7]) != 0)
    {
        nChargeUpPercent = SKILLDESC_GetChargeUpDamagePercent();
        if (nChargeUpPercent)
        {
            nDamagePercent += nChargeUpPercent;
            *pnColor = 3;
        }
    }

    nNormalDamage = 0;
    if (pWeaponUsed)
    {
        nStrengthBonus = D2COMMON_ITEMS_GetStrengthBonus(pWeaponUsed);
        if (nStrengthBonus)
        {
            nDamagePercent += nStrengthBonus * D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_STRENGTH, 0) / 100;
        }
        nDexBonus = D2COMMON_ITEMS_GetDexBonus(pWeaponUsed);
        if (nDexBonus)
        {
            nDamagePercent += nDexBonus * D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_DEXTERITY, 0) / 100;
        }
        nNormalDamage = D2COMMON_STATLIST_UnitGetItemStatOrSkillStatValue(pWeaponUsed, STAT_ITEM_NORMALDAMAGE, 0);
    }
    else
    {
        nDamagePercent += D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_STRENGTH, 0);
    }

    if (D2COMMON_STATES_CheckStateMaskDamBlueOnUnit(pUnit))
    {
        *pnColor = 3;
    }
    if (D2COMMON_STATES_CheckStateMaskDamRedOnUnit(pUnit))
    {
        *pnColor = 1;
    }
    if (nDamagePercent <= -90)
    {
        nDamagePercent = -90;
    }

    *pnMin = nFlatBonus + nNormalDamage + *pnMin * (nItemMinDamagePercent + nDamagePercent + 100) / 100;
    *pnMax = nFlatBonus + nNormalDamage + *pnMax * (nItemMaxDamagePercent + nDamagePercent + 100) / 100;
    return 1;
}


//1.10f: D2Client.0x6FB0B580
//  The damage-percent bonus the local player's active charge-ups contribute, summed. Takes no arguments; the unit comes from CUNIT_GetPlayer().
//  Walks sgptDataTables->pProgressiveStates (progressive states.txt) and, for each state the player has:
//      STAT_MODIFIERLIST_SKILL (350)   the skill that granted the charge
//      STAT_MODIFIERLIST_LEVEL (351)   that skill's level
//      pSkillsTxt->wAuraStat[0]        the stat the charge itself carries
//      counted only when pSkillsTxt->nPrgDamage == 1  (skills.txt prgdam)
//      nBonus += chargeStat * (dwParam[0] + (nLevel - 1) * dwParam[1])
//  The same skills.txt record is looked up twice (pSkillsTxt for nPrgDamage and wAuraStat[0], then again, with the bounds check repeated, for
//  dwParam), and the loop counter is mirrored in nIndex; both as found. nProgressiveStates is read as a word (movsx ecx, word ptr [eax+170h]).
//  The original inlines DATATBLS_GetSkillsTxtRecordById and SKILLDESC_GetLinearParamByLevel here.
int32_t __cdecl SKILLDESC_GetChargeUpDamagePercent(void)
{
    D2UnitStrc *pPlayer;
    D2DataTablesStrc *pDataTables;
    D2StatListStrc *pStatList;
    D2SkillsTxt *pSkillsTxt;
    D2SkillsTxt *pSkillsTxt2;
    int32_t nBonus;
    int32_t iState;
    int32_t nIndex;
    int32_t nChargeSkillId;
    int32_t nChargeLevel;
    int32_t nStatValue;
    int32_t nPerCharge;
    int16_t nStatId;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        return 0;
    }
    nBonus = 0;
    iState = 0;
    nIndex = 0;
    pDataTables = sgptDataTables;
    if (sgptDataTables->nProgressiveStates > 0)
    {
        while (1)
        {
            D2C_States nState = (D2C_States)pDataTables->pProgressiveStates[iState];

            if (D2COMMON_STATES_CheckState(pPlayer, nState))
            {
                pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pPlayer, nState);
                if (pStatList)
                {
                    nChargeSkillId = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_MODIFIERLIST_SKILL, 0);
                    if (nChargeSkillId > 0)
                    {
                        pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nChargeSkillId);
                        if (pSkillsTxt)
                        {
                            nChargeLevel = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_MODIFIERLIST_LEVEL, 0);
                            if (nChargeLevel > 0 && pSkillsTxt->nPrgDamage == 1)
                            {
                                nStatId = pSkillsTxt->wAuraStat[0];
                                if (nStatId >= 0 && nStatId < sgptDataTables->nItemStatCostTxtRecordCount)
                                {
                                    nStatValue = D2COMMON_STATLIST_GetStatValue(pStatList, (D2C_ItemStats)nStatId, 0);
                                    if (nStatValue > 0)
                                    {
                                        if (nChargeSkillId < sgptDataTables->nSkillsTxtRecordCount && (pSkillsTxt2 = &sgptDataTables->pSkillsTxt[nChargeSkillId]) != NULL)
                                        {
                                            nPerCharge = pSkillsTxt2->dwParam[0] + (nChargeLevel - 1) * pSkillsTxt2->dwParam[1];
                                        }
                                        else
                                        {
                                            nPerCharge = 0;
                                        }
                                        nBonus += nStatValue * nPerCharge;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            iState = nIndex + 1;
            pDataTables = sgptDataTables;
            nIndex = iState;
            if (iState >= sgptDataTables->nProgressiveStates)
            {
                break;
            }
        }
    }
    return nBonus;
}


//1.10f: D2Client.0x6FB0B6F0
//  The elemental half of the panel's damage figure: adds the elemental damage the character's stats carry (items, auras, charms) to a running
//  min/max pair, and reports the colour the total should be drawn in. The original inlines DATATBLS_ApplyRatio in the four scalings.
//  Driven by a four-row table, gnElemDamageStats (three stat ids per row: min, max, mastery), with a parallel colour array gnElemDamageColors:
//      firemindam  48 / firemaxdam  49 / passive_fire_mastery 329 -> 1 red
//      lightmindam 50 / lightmaxdam 51 / passive_ltng_mastery 330 -> 9 yellow
//      coldmindam  54 / coldmaxdam  55 / passive_cold_mastery 331 -> 3 blue
//      magicmindam 52 / magicmaxdam 53 / STAT_INVALID (-1)      -> 3 blue
//  The mastery is applied through DATATBLS_ApplyRatio(value, mastery, 100): the four scalings in this body (min and max in the loop, min and
//  max of the poison) are copies of its four-way split, and two of the arms divide before multiplying, so a plain 64-bit multiply would differ.
//  Poison is handled after the loop because it needs its duration: STAT_SKILL_POISON_OVERRIDE_LENGTH, or STAT_POISONLENGTH / max(1,
//  STAT_POISON_COUNT), and the damage becomes (damage * length) >> 8, the per-frame value turned into a total; its colour is 2 (light green).
//  After the elements, a skill flagged SKILLSFLAGINDEX_FINISHING gets the charge-up damage through SKILLDESC_AddChargeUpElemDamage, and the two
//  state checks override the colour last, DamBlue 3 then DamRed 1. The last block is a display guard, not arithmetic: when the max came out
//  above zero, `if (*pnMin < 1) *pnMin = 1; if (*pnMax <= *pnMin) *pnMax = *pnMin + 1;`, so a panel line never reads 0-0 and never shows a max
//  below its min; the damage the game deals is untouched. pUnused is accepted and never read.
//  void: no caller reads what the original leaves in eax.
void __fastcall SKILLDESC_AddItemElemDamage(D2UnitStrc *pUnit, int32_t *pnMin, int32_t *pnMax, int32_t *pnColor, D2UnitStrc *pUnused, D2SkillStrc *pSkill)
{
    D2SkillsTxt *pSkillsTxtRec;
    UINT_PTR nStatRow;   // integer cursor; see the DIFFERENCE note below
    const int32_t *pnColorRow;
    int32_t nMin;
    int32_t nMax;
    int32_t nMastery;
    int32_t nPoisonMin;
    int32_t nPoisonMax;
    int32_t nPoisonMastery;
    int32_t nPoisonLength;
    int32_t nPoisonCount;
    int32_t nMinFloored;
    int32_t nMaxFloored;

    //  DIFFERENCE: the original ends the stat walk at an address in the next global (the stats start at gnElemDamageStats + 2 and step 12 bytes
    //  four times, so the bound is +14 dwords, &gnElemDamageColors[2]). Ours uses an integer cursor bounded from gnElemDamageStats and compared
    //  signed as the bytes compare it, read only inside the table, because a pointer there would leave the stat table. The colour pointer ends
    //  one past gnElemDamageColors.
    pnColorRow = gnElemDamageColors;
    nStatRow = (UINT_PTR)&gnElemDamageStats[2];
    do
    {
        nMin = D2COMMON_STATLIST_UnitGetStatValue(pUnit, (D2C_ItemStats)((const int32_t *)nStatRow)[-2], 0);
        nMax = D2COMMON_STATLIST_UnitGetStatValue(pUnit, (D2C_ItemStats)((const int32_t *)nStatRow)[-1], 0);
        if (*(const int32_t *)nStatRow != STAT_INVALID)
        {
            nMastery = D2COMMON_STATLIST_UnitGetStatValue(pUnit, (D2C_ItemStats)*(const int32_t *)nStatRow, 0);
            if (nMastery)
            {
                nMin += DATATBLS_ApplyRatio(nMin, nMastery, 100);
                nMax += DATATBLS_ApplyRatio(nMax, nMastery, 100);
            }
        }
        if (nMin >= nMax)
        {
            nMin = nMax;
        }
        *pnMin += nMin;
        *pnMax += nMax;
        if (nMin || nMax)
        {
            *pnColor = *pnColorRow;
        }
        nStatRow += 3 * sizeof(int32_t);
        ++pnColorRow;
    }
    while ((int32_t)nStatRow < (int32_t)((UINT_PTR)gnElemDamageStats + 14 * sizeof(int32_t)));

    nPoisonMin = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_POISONMINDAM, 0);
    nPoisonMax = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_POISONMAXDAM, 0);
    if (nPoisonMax)
    {
        nPoisonMastery = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_PASSIVE_POIS_MASTERY, 0);
        if (nPoisonMastery)
        {
            nPoisonMin += DATATBLS_ApplyRatio(nPoisonMin, nPoisonMastery, 100);
            nPoisonMax += DATATBLS_ApplyRatio(nPoisonMax, nPoisonMastery, 100);
        }
        nPoisonLength = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_SKILL_POISON_OVERRIDE_LENGTH, 0);
        if (nPoisonLength <= 0)
        {
            nPoisonCount = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_POISON_COUNT, 0);
            if (nPoisonCount <= 1)
            {
                nPoisonCount = 1;
            }
            nPoisonLength = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_POISONLENGTH, 0) / nPoisonCount;
        }
        *pnColor = 2;
        *pnMin += (nPoisonMin * nPoisonLength) >> 8;
        *pnMax += (nPoisonMax * nPoisonLength) >> 8;
    }

    if (*pnMax > 0)
    {
        nMinFloored = *pnMin;
        if (*pnMin < 1)
        {
            nMinFloored = 1;
        }
        *pnMin = nMinFloored;
        nMaxFloored = nMinFloored + 1;
        if (nMaxFloored <= *pnMax)
        {
            nMaxFloored = *pnMax;
        }
        *pnMax = nMaxFloored;
    }

    if (pSkill)
    {
        pSkillsTxtRec = D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill(pSkill);
        if (pSkillsTxtRec)
        {
            if ((((uint8_t *)pSkillsTxtRec->dwFlags)[SKILLSFLAGINDEX_FINISHING >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_FINISHING & 7]) != 0)
            {
                SKILLDESC_AddChargeUpElemDamage(pUnit, pnMin, pnMax, pnColor);
            }
        }
    }

    if (D2COMMON_STATES_CheckStateMaskDamBlueOnUnit(pUnit))
    {
        *pnColor = 3;
    }
    if (D2COMMON_STATES_CheckStateMaskDamRedOnUnit(pUnit))
    {
        *pnColor = 1;
    }
}


//1.10f: D2Client.0x6FB0BB10
//  The elemental twin of SKILLDESC_GetChargeUpDamagePercent: the same walk over sgptDataTables->pProgressiveStates with the same
//  MODIFIERLIST_SKILL / MODIFIERLIST_LEVEL pair, but it goes on to the charge-up skill's own skilldesc record for the element and adds
//  elemental damage to the running min/max instead of a percentage. It takes the unit as an argument rather than asking CUNIT_GetPlayer. Callers:
//  SKILLDESC_AddItemElemDamage (gated on SKILLSFLAGINDEX_FINISHING) and the kick line SKILLDESC_DamFunc15 (not gated). The original inlines
//  DATATBLS_GetSkillsTxtRecordById here.
//  The charge level picks all three values at once; the index is the charge's own stat value clamped to 1..3:
//      iCalc          = clamp(chargeStat, 1, 3)
//      min  calc      dwProgDmgMin[iCalc - 1]   through SKILLS_EvaluateSkillDescFormula
//      max  calc      dwProgDmgMax[iCalc - 1]   likewise
//      element byte   nPrgDamElem[iCalc - 1]
//  The three fields are consecutive in D2SkillDescTxt (nPrgDamElem at +0x20, dwProgDmgMin at +0x24, dwProgDmgMax at +0x30), which is why the
//  bytes read a dword at +0x20 + 4 * iCalc. When the skilldesc record cannot be resolved the arm falls back to the D2Common damage getters
//  (>> 8), the way the percentage twin carries on without a record. The element byte is mapped to a colour and only written when it is in
//  1..0xC: 1 -> 1, 2 -> 9, 3 and 4 -> 3, 5 -> 2, anything else -> 0. Returns 1 on every path, including the one that adds nothing.
int32_t __fastcall SKILLDESC_AddChargeUpElemDamage(D2UnitStrc *pUnit, int32_t *pnMin, int32_t *pnMax, int32_t *pnColor)
{
    D2DataTablesStrc *pDataTables;
    D2StatListStrc *pStatList;
    D2SkillsTxt *pSkillsTxt;
    D2SkillsTxt *pSkillsTxtByStat;
    D2SkillDescTxt *pSkillDescTxt;
    int32_t nChargeSkillId;
    int32_t nChargeLevel;
    int32_t nStatId;
    int32_t nStatValue;
    int32_t iCalc;
    int32_t iElem;
    int32_t nLineCount;
    int32_t nIndex;
    int32_t nElemTypeByte;
    int32_t wSkillDesc;

    nLineCount = 0;
    nIndex = 0;
    pDataTables = sgptDataTables;
    if (sgptDataTables->nProgressiveStates > 0)
    {
        while (1)
        {
            D2C_States nState = (D2C_States)pDataTables->pProgressiveStates[nLineCount];

            if (D2COMMON_STATES_CheckState(pUnit, nState))
            {
                pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, nState);
                if (pStatList)
                {
                    nChargeSkillId = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_MODIFIERLIST_SKILL, 0);
                    //  a signed `> 0` (test edi, edi / jle at 0x6FB0BB8B); `> SKILL_ATTACK` would promote the id to the enum's unsigned type
                    //  and let a negative one through
                    if (nChargeSkillId > 0)
                    {
                        pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nChargeSkillId);
                        if (pSkillsTxt)
                        {
                            nChargeLevel = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_MODIFIERLIST_LEVEL, 0);
                            if (nChargeLevel > 0)
                            {
                                nStatId = pSkillsTxt->wAuraStat[0];
                                if (nStatId >= 0 && nStatId < sgptDataTables->nItemStatCostTxtRecordCount)
                                {
                                    nStatValue = D2COMMON_STATLIST_GetStatValue(pStatList, (D2C_ItemStats)nStatId, 0);
                                    if (nStatValue > 0)
                                    {
                                        if (nChargeSkillId < sgptDataTables->nSkillsTxtRecordCount && (pSkillsTxtByStat = &sgptDataTables->pSkillsTxt[nChargeSkillId]) != NULL && (wSkillDesc = (int16_t)pSkillsTxtByStat->wSkillDesc) >= 0 && wSkillDesc
                                               < sgptDataTables->nSkillDescTxtRecordCount && (pSkillDescTxt = &sgptDataTables->pSkillDescTxt[wSkillDesc]) != NULL)
                                        {
                                            if (nStatValue <= 1)
                                            {
                                                iCalc = 1;
                                            }
                                            else
                                            {
                                                iCalc = nStatValue;
                                                if (nStatValue >= 3)
                                                {
                                                    iCalc = 3;
                                                }
                                            }
                                            *pnMin += D2COMMON_SKILLS_EvaluateSkillDescFormula(pUnit, pSkillDescTxt->dwProgDmgMin[iCalc - 1], (D2C_Skills)nChargeSkillId, nChargeLevel);
                                            *pnMax += D2COMMON_SKILLS_EvaluateSkillDescFormula(pUnit, pSkillDescTxt->dwProgDmgMax[iCalc - 1], (D2C_Skills)nChargeSkillId, nChargeLevel);
                                            nElemTypeByte = pSkillDescTxt->nPrgDamElem[iCalc - 1];
                                            if (nElemTypeByte && nElemTypeByte < 0xD)
                                            {
                                                switch (nElemTypeByte)
                                                {
                                                case 1:
                                                    iElem = 1;
                                                    break;
                                                case 2:
                                                    iElem = 9;
                                                    break;
                                                case 3:
                                                case 4:
                                                    iElem = 3;
                                                    break;
                                                case 5:
                                                    iElem = 2;
                                                    break;
                                                default:
                                                    iElem = 0;
                                                    break;
                                                }
                                                *pnColor = iElem;
                                            }
                                        }
                                        else
                                        {
                                            *pnMin += D2COMMON_SKILLS_GetMinElemDamage(pUnit, (D2C_Skills)nChargeSkillId, nChargeLevel, 1) >> 8;
                                            *pnMax += D2COMMON_SKILLS_GetMaxElemDamage(pUnit, (D2C_Skills)nChargeSkillId, nChargeLevel, 1) >> 8;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            nLineCount = nIndex + 1;
            pDataTables = sgptDataTables;
            nIndex = nLineCount;
            if (nLineCount >= sgptDataTables->nProgressiveStates)
            {
                break;
            }
        }
    }
    return 1;
}


//1.10f: D2Client.0x6FB0BD60
//  Adds the character's elemental damage stats for one element to a running min/max pair; the single-element counterpart of
//  SKILLDESC_AddItemElemDamage, with the same stat pairs and no mastery. A switch on nEType: 1 Fire -> firemindam/firemaxdam, 2 Lightning ->
//  lightmindam/lightmaxdam, 3 Magic -> magicmindam/magicmaxdam, 4 Cold and 0xC (a second cold id, as found) -> coldmindam/coldmaxdam, 5 Poison:
//      nPoisonLength = STAT_SKILL_POISON_OVERRIDE_LENGTH
//      if that is <= 0:  nPoisonLength = STAT_POISONLENGTH / max(STAT_POISON_COUNT, 1)
//      *pnMin += (nPoisonLength * STAT_POISONMINDAM) >> 8
//      *pnMax += (nPoisonLength * STAT_POISONMAXDAM) >> 8
//  The poison stats are damage per frame in 8.8 fixed point, so what is added is the total over the duration.
//  void: eax is the last maximum read, or nEType - 1 on the default arm, and no caller reads it.
void __fastcall SKILLDESC_AddElemDamageByType(D2UnitStrc *pUnit, int32_t *pnMin, int32_t *pnMax, uint8_t nEType)
{
    int32_t nPoisonLength;
    int32_t nPoisonCount;
    int32_t nPoisonMinDam;
    int32_t nMaxDam;

    switch (nEType)
    {
    case 1:
        *pnMin += D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_FIREMINDAM, 0);
        nMaxDam = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_FIREMAXDAM, 0);
        *pnMax += nMaxDam;
        return;
    case 2:
        *pnMin += D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_LIGHTMINDAM, 0);
        nMaxDam = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_LIGHTMAXDAM, 0);
        *pnMax += nMaxDam;
        return;
    case 3:
        *pnMin += D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_MAGICMINDAM, 0);
        nMaxDam = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_MAGICMAXDAM, 0);
        *pnMax += nMaxDam;
        return;
    case 4:
    case 0xC:
        *pnMin += D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_COLDMINDAM, 0);
        nMaxDam = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_COLDMAXDAM, 0);
        *pnMax += nMaxDam;
        return;
    case 5:
        nPoisonLength = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_SKILL_POISON_OVERRIDE_LENGTH, 0);
        if (nPoisonLength <= 0)
        {
            nPoisonCount = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_POISON_COUNT, 0);
            if (nPoisonCount <= 1)
            {
                nPoisonCount = 1;
            }
            nPoisonLength = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_POISONLENGTH, 0) / nPoisonCount;
        }
        nPoisonMinDam = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_POISONMINDAM, 0);
        nMaxDam = nPoisonLength * D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_POISONMAXDAM, 0);
        *pnMin += (nPoisonLength * nPoisonMinDam) >> 8;
        nMaxDam >>= 8;
        *pnMax += nMaxDam;
        return;
    default:
        return;
    }
}


//1.10f: D2Client.0x6FB0BEC0
//  descdam handler 7: entry 7 of gpSkillDescDamFuncs. The common body of the whole family: skills.txt by id, then its wSkillDesc into
//  skilldesc.txt, then dwDamCalc[0] and [1] through the assert helper (line numbers 2444/2445), fed to SKILLDESC_AddWeaponPhysDamage as the
//  percent and the flat bonus; add the item's elemental damage, scale both ends by srcdam (skipped when it is 0x80, the 128ths "leave it
//  alone"), then add the four skill getters, each >> 8 (Get{Min,Max}{Phys,Elem}Damage), and draw one centred line in the colour the damage
//  adders produced. The original inlines DATATBLS_GetSkillsTxtRecordById, DATATBLS_ApplyRatio and SKILLDESC_FormatRangeOrValue here.
//  The two scalings are DATATBLS_ApplyRatio(value, srcdam, 128) with a dead arm: the srcdam > 0x10000 pair (cmp esi, 10000h at 0x6FB0C023 and
//  0x6FB0C095) can never be entered by a byte, and the call reproduces it. The single-value arm is dead too: `if (min >= max) max = min + 1;`
//  runs before the format test. nDrawColor is the colour out-parameter of AddWeaponPhysDamage, not a width.
void __fastcall SKILLDESC_DamFunc7(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    int32_t nSkillId;
    D2SkillsTxt *pSkillsTxtRec;
    int32_t wSkillDesc;
    D2SkillDescTxt *pSkillDescRec;
    D2C_Skills nSkillIdFromSkill;
    int32_t nPercentBonus;
    int32_t nFlatBonus;
    uint8_t nSrcDam;
    int32_t nMin;
    int32_t nMax;
    int32_t nMinBeforeClamp;
    int32_t nDrawColor;
    int32_t nYCopy;
    int32_t nWidth;
    int32_t nHeight;
    Unicode wszOut[32];

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    nSkillId = pSkillsTxt->nSkillId;
    pSkillsTxtRec = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxtRec)
    {
        return;
    }
    wSkillDesc = (int16_t)pSkillsTxtRec->wSkillDesc;
    if (wSkillDesc < 0 || wSkillDesc >= sgptDataTables->nSkillDescTxtRecordCount)
    {
        return;
    }
    pSkillDescRec = &sgptDataTables->pSkillDescTxt[wSkillDesc];
    if (!pSkillDescRec)
    {
        return;
    }

    nMin = 0;
    nMax = 0;
    nDrawColor = 0;
    nSkillIdFromSkill = (D2C_Skills)D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillDescCpp, 2444);
    nPercentBonus = D2COMMON_SKILLS_EvaluateSkillDescFormula(pUnit, pSkillDescRec->dwDamCalc[0], nSkillIdFromSkill, nSkillLevel);
    nSkillIdFromSkill = (D2C_Skills)D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillDescCpp, 2445);
    nFlatBonus = D2COMMON_SKILLS_EvaluateSkillDescFormula(pUnit, pSkillDescRec->dwDamCalc[1], nSkillIdFromSkill, nSkillLevel);
    SKILLDESC_AddWeaponPhysDamage(pUnit, &nMin, &nMax, &nDrawColor, nPercentBonus, nFlatBonus, pSkill, 128, NULL, 0);
    SKILLDESC_AddItemElemDamage(pUnit, &nMin, &nMax, &nDrawColor, 0, pSkill);

    nSrcDam = pSkillsTxt->nSrcDam;
    if (nSrcDam != 0x80)
    {
        nMin = DATATBLS_ApplyRatio(nMin, nSrcDam, 128);
        nMax = DATATBLS_ApplyRatio(nMax, nSrcDam, 128);
    }

    nMin += D2COMMON_SKILLS_GetMinPhysDamage(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel, 0) >> 8;
    nMax += D2COMMON_SKILLS_GetMaxPhysDamage(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel, 0) >> 8;
    nMin += D2COMMON_SKILLS_GetMinElemDamage(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel, 1) >> 8;
    nMax += D2COMMON_SKILLS_GetMaxElemDamage(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel, 1) >> 8;

    nYCopy = nY;
    nMinBeforeClamp = nMin;
    if (nMin >= nMax)
    {
        nMax = nMin + 1;
    }
    SKILLDESC_FormatRangeOrValue(wszOut, nMinBeforeClamp, nMax, 0);
    nWidth = 0;
    nHeight = 0;
    D2WIN_10131_GetTextDimensions(wszOut, &nWidth, &nHeight);
    nWidth = 11 * nWidth / 7;
    if (nWidth <= nRight - nLeft)
    {
        D2WIN_10127_SetFont(D2FONT_FONT16);
    }
    else
    {
        D2WIN_10127_SetFont(D2FONT_FONT6);
        nYCopy = nY - 1;
    }
    CHAR_DrawTextCentered(gnBorderXDifference + nLeft, gnScreenHeight + gnBorderYDifference + nYCopy - 480, gnBorderXDifference + nRight, wszOut, nDrawColor);
}


//1.10f: D2Client.0x6FB0C270
//  descdam handler 1: entry 1 of gpSkillDescDamFuncs; stock skilldesc.txt gives it attack, whirlwind and maul, the plain weapon attacks.
//  Same guard chain as SKILLDESC_DamFunc7 (skills.txt by id, then wSkillDesc into skilldesc.txt), then:
//      CanDualWield -> evaluate dwDamCalc[0] and [1] (lines 2488/2489) and hand SKILLDESC_AddTotalDamageCallback to
//                      SKILLDESC_DrawDualWieldDamage with bAddElemDamage = 1, so the weapon's elemental damage is folded into the line
//      otherwise    -> a tail call to SKILLDESC_DamFunc7
//  SKILLDESC_DamFunc20 is this function with the 1 replaced by 0 (and other assert line numbers); descdam 20 is the Assassin's fists of fire /
//  claws of thunder / blades of ice, and 0 keeps the weapon's own elements out of the line. The callback is passed as a function pointer and
//  nExtraA / nExtraB are the evaluated dwDamCalc[0] and [1]. The original inlines DATATBLS_GetSkillsTxtRecordById here.
void __fastcall SKILLDESC_DamFunc1(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    int32_t nSkillId;
    D2SkillsTxt *pSkillsTxtRec;
    int32_t wSkillDesc;
    D2SkillDescTxt *pSkillDescRec;
    D2C_Skills nSkillIdFromSkill;
    int32_t nExtraA;
    int32_t nExtraB;

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    nSkillId = pSkillsTxt->nSkillId;
    pSkillsTxtRec = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxtRec)
    {
        return;
    }
    wSkillDesc = (int16_t)pSkillsTxtRec->wSkillDesc;
    if (wSkillDesc < 0 || wSkillDesc >= sgptDataTables->nSkillDescTxtRecordCount)
    {
        return;
    }
    pSkillDescRec = &sgptDataTables->pSkillDescTxt[wSkillDesc];
    if (!pSkillDescRec)
    {
        return;
    }

    if (!D2COMMON_UNITS_CanDualWield(pUnit))
    {
        SKILLDESC_DamFunc7(pUnit, pSkill, pSkillsTxt, nSkillLevel, nLeft, nY, nRight);
        return;
    }
    nSkillIdFromSkill = (D2C_Skills)D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillDescCpp, 2488);
    nExtraA = D2COMMON_SKILLS_EvaluateSkillDescFormula(pUnit, pSkillDescRec->dwDamCalc[0], nSkillIdFromSkill, nSkillLevel);
    nSkillIdFromSkill = (D2C_Skills)D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillDescCpp, 2489);
    nExtraB = D2COMMON_SKILLS_EvaluateSkillDescFormula(pUnit, pSkillDescRec->dwDamCalc[1], nSkillIdFromSkill, nSkillLevel);
    SKILLDESC_DrawDualWieldDamage(pUnit, pSkill, pSkillsTxt, nSkillLevel, SKILLDESC_AddTotalDamageCallback, nExtraA, nExtraB, nLeft, nY, nRight, 1);
}


//1.10f: D2Client.0x6FB0C3A0
//  Physical plus elemental in one call: SKILLDESC_AddWeaponPhysDamage followed by SKILLDESC_AddItemElemDamage, accumulating into the same
//  min/max pair and colour. One of the three implementations
//  of the shared nine-argument callback (see SKILLDESC_ForEachWieldedWeapon), only ever taken as a function pointer. bUseItemOwnStats is 0, so
//  the physical half reads the unit's damage stats rather than the item's, and pWeapon is handed on to the elemental half's unused fifth parameter.
//  void: eax is left holding what SKILLDESC_AddItemElemDamage leaves, 1 on every path, and no caller reads it.
void __fastcall SKILLDESC_AddTotalDamageCallback(D2UnitStrc *pUnit, int32_t *pnMin, int32_t *pnMax, int32_t *pnColor, int32_t nExtraDamagePercent, int32_t nFlatBonus, D2SkillStrc *pSkill, int32_t nSrcDamOverride, D2UnitStrc *pWeapon)
{
    SKILLDESC_AddWeaponPhysDamage(pUnit, pnMin, pnMax, pnColor, nExtraDamagePercent, nFlatBonus, pSkill, nSrcDamOverride, pWeapon, 0);
    SKILLDESC_AddItemElemDamage(pUnit, pnMin, pnMax, pnColor, pWeapon, pSkill);
}


//1.10f: D2Client.0x6FB0C400
//  Draws the dual-wield damage figure: one centred line when only one hand came back with a pair, two through SKILLDESC_DrawTwoLines when both
//  did. The walk is SKILLDESC_ForEachWieldedWeapon with the caller's own pfnCompute and its two extras; the arms below only scale what it
//  produced. The original inlines SKILLDESC_FormatRangeOrValue in the two-line path.
//  Every non-zero pair is scaled as the whole panel scales weapon damage: value * pSkillsTxt->nSrcDam / 128 (nSrcDam is the byte at +0x1A5,
//  mov dl, [esi+1A5h] at 0x6FB0C480). The elemental total SKILLDESC_AddElemDamageAndColor produced is added to both hands, and bAddElemDamage
//  decides whether that elemental half runs at all.
//  The walker's out-parameters are two (min, max) pairs plus one colour apiece: pnValueA / pnColorA are hand A's ends and nMin / nMax hand B's,
//  while the two (int)& arguments, both pre-set to the elemental colour, are the callbacks' own colour slots. The three arms, in test order:
//      hand A (pnValueA, pnColorA) all zero  -> draw hand B alone
//      hand B (nMin, nMax) all zero          -> draw hand A alone
//      otherwise                             -> draw both, line 1 and 2
//  An absent hand is exactly (0, 0), so a character with one weapon gets a single line. Hand B's line takes nArgB (the elemental colour the
//  callback may overwrite) and hand A's takes pnColor (the one it may also have replaced).
//  The single-value arm of both one-line paths is dead, as found: each clamps `if (min >= max) max = min + 1;` before its format test (lea ebx,
//  [edi+1] at 0x6FB0C579, cmp edi, ebx at 0x6FB0C593). The two-line path does not clamp, and there pnValueA == pnColorA and nMin == nMax really
//  do collapse to a plain "%d". The clamp stays where the original put it.
void __fastcall SKILLDESC_DrawDualWieldDamage(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, void *pfnCompute, int32_t nExtraA, int32_t nExtraB, int32_t nX, int32_t nY, int32_t nRight, int32_t bAddElemDamage)
{
    int32_t nSrcDam;
    int32_t nValueACopy;
    int32_t nColorACopy;
    int32_t nMin2;
    int32_t nMax2;
    int32_t nMin3;
    int32_t nMax3;
    int32_t nYSaved;
    int32_t nColor2;
    int32_t nWidth;
    int32_t nHeight;
    int32_t nRightCopy;
    int32_t nXCopy;
    int32_t nYCopy;
    int32_t pnValueA;
    int32_t pnColorA;
    int32_t pnMinTotal;
    int32_t pnMaxTotal;
    int32_t pnColor;
    int32_t nArgB;
    int32_t nMin;
    int32_t nMax;
    int32_t nColor;
    Unicode wszSrc[32];
    Unicode wszDest[32];
    Unicode wszOut[32];

    if (!pSkill || !pSkillsTxt || !pfnCompute)
    {
        return;
    }

    pnMinTotal = 0;
    pnMaxTotal = 0;
    pnColor = 0;
    nArgB = 0;
    if (bAddElemDamage)
    {
        SKILLDESC_AddElemDamageAndColor(pUnit, pSkillsTxt, nSkillLevel, &pnMinTotal, &pnMaxTotal, &pnColor);
    }
    nArgB = pnColor;
    nSrcDam = pSkillsTxt->nSrcDam;
    pnValueA = 0;
    pnColorA = 0;
    nMin = 0;
    nMax = 0;
    SKILLDESC_ForEachWieldedWeapon(pUnit, &pnValueA, &pnColorA, &pnColor, &nMin, &nMax, &nArgB, pSkill, pfnCompute, nExtraA, nExtraB);
    nColorACopy = pnColorA;
    nValueACopy = pnValueA;
    if (pnValueA && pnColorA)
    {
        nValueACopy = nSrcDam * pnValueA / 128 + pnMinTotal;
        pnValueA = nValueACopy;
        nColorACopy = pnMaxTotal + nSrcDam * pnColorA / 128;
        pnColorA = nColorACopy;
    }
    nMin2 = nMin;
    nMax2 = nMax;
    if (nMin && nMax)
    {
        nMin2 = nSrcDam * nMin / 128 + pnMinTotal;
        nMin = nMin2;
        nMax2 = pnMaxTotal + nSrcDam * nMax / 128;
        nMax = nMax2;
    }

    //  hand A empty: one line for hand B, in the colour of hand B
    if (!nValueACopy && !nColorACopy)
    {
        nYSaved = nY;
        nColor = nArgB;
        if (nMin2 >= nMax2)
        {
            nMax2 = nMin2 + 1;
        }
        SKILLDESC_FormatRangeOrValue(wszSrc, nMin2, nMax2, 0);
        nWidth = 0;
        nHeight = 0;
        D2WIN_10131_GetTextDimensions(wszSrc, &nWidth, &nHeight);
        nRightCopy = nRight;
        nXCopy = nX;
        nWidth = 11 * nWidth / 7;
        if (nWidth <= nRight - nX)
        {
            D2WIN_10127_SetFont(D2FONT_FONT16);
            nYCopy = nYSaved;
        }
        else
        {
            D2WIN_10127_SetFont(D2FONT_FONT6);
            nYCopy = nY - 1;
        }
        nColor2 = nColor;
        CHAR_DrawTextCentered(gnBorderXDifference + nXCopy, gnScreenHeight + gnBorderYDifference + nYCopy - 480, gnBorderXDifference + nRightCopy, wszSrc, nColor2);
        return;
    }

    //  hand B empty: one line for hand A, in the colour of hand A
    if (!nMin2 && !nMax2)
    {
        nYSaved = nY;
        nHeight = pnColor;
        nMax3 = nColorACopy;
        nMin3 = nValueACopy;
        if (nValueACopy >= nColorACopy)
        {
            nMax3 = nValueACopy + 1;
        }
        SKILLDESC_FormatRangeOrValue(wszSrc, nMin3, nMax3, 0);
        nWidth = 0;
        nColor = 0;
        D2WIN_10131_GetTextDimensions(wszSrc, &nWidth, &nColor);
        nRightCopy = nRight;
        nXCopy = nX;
        nWidth = 11 * nWidth / 7;
        if (nWidth <= nRight - nX)
        {
            D2WIN_10127_SetFont(D2FONT_FONT16);
            nYCopy = nYSaved;
            nColor2 = nHeight;
        }
        else
        {
            D2WIN_10127_SetFont(D2FONT_FONT6);
            nColor2 = nHeight;
            nYCopy = nY - 1;
        }
        CHAR_DrawTextCentered(gnBorderXDifference + nXCopy, gnScreenHeight + gnBorderYDifference + nYCopy - 480, gnBorderXDifference + nRightCopy, wszSrc, nColor2);
        return;
    }

    //  both hands: line 1 is hand A in `pnColor`, line 2 hand B in `nArgB`
    SKILLDESC_FormatRangeOrValue(wszOut, pnValueA, pnColorA, 0);
    SKILLDESC_FormatRangeOrValue(wszDest, nMin, nMax, 0);
    SKILLDESC_DrawTwoLines(wszOut, wszDest, nX, nY, nRight, pnColor, nArgB);
}


//1.10f: D2Client.0x6FB0C840
//  Folds a skill's elemental damage into a running min/max total and reports the colour the total should be drawn in; the shared "and now the
//  elemental part" step of the panel. pnColor is a ui_color_t, not an element id. The remap of pSkillsTxt->nEType onto the game's text colours:
//      1 Fire      -> 1  UI_COLOR_RED
//      2 Lightning -> 9  UI_COLOR_YELLOW
//      3 Magic     -> 3  UI_COLOR_BLUE
//      4 Cold      -> 3  UI_COLOR_BLUE
//      5 Poison    -> 2  UI_COLOR_LIGHT_GREEN
//  and anything else leaves *pnColor untouched. Poison is scaled by its duration: both halves are multiplied by SKILLS_GetElementalLength
//  before the >> 8, because poison damage in the txt is per frame and the panel shows the total. The poison branch also writes *pnColor = 2
//  before the switch reaches the same answer (redundant, as found). Both getters take the last argument 1, so Fire, Lightning, Cold and Poison
//  mastery are already in the numbers when they are added.
//  void: eax is the maximum elemental damage the skill contributes (also stored in *pnMaxTotal), and no caller reads it.
void __fastcall SKILLDESC_AddElemDamageAndColor(D2UnitStrc *pUnit, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnMinTotal, int32_t *pnMaxTotal, int32_t *pnColor)
{
    int32_t nMinElem;
    int32_t nMaxElem;
    int32_t nElemLength;

    if (pSkillsTxt->nEType == 5)
    {
        nElemLength = D2COMMON_SKILLS_GetElementalLength(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel, 1);
        *pnColor = UI_COLOR_LIGHT_GREEN;
        nMinElem = (D2COMMON_SKILLS_GetMinElemDamage(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel, 1) * nElemLength) >> 8;
        nMaxElem = (nElemLength * D2COMMON_SKILLS_GetMaxElemDamage(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel, 1)) >> 8;
    }
    else
    {
        nMinElem = D2COMMON_SKILLS_GetMinElemDamage(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel, 1) >> 8;
        nMaxElem = D2COMMON_SKILLS_GetMaxElemDamage(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel, 1) >> 8;
    }

    if (pnColor)
    {
        switch (pSkillsTxt->nEType)
        {
        case 1:
            *pnColor = UI_COLOR_RED;
            break;
        case 2:
            *pnColor = UI_COLOR_YELLOW;
            break;
        case 3:
        case 4:
            *pnColor = UI_COLOR_BLUE;
            break;
        case 5:
            *pnColor = UI_COLOR_LIGHT_GREEN;
            break;
        default:
            break;
        }
    }
    *pnMinTotal += nMinElem;
    *pnMaxTotal += nMaxElem;
}


//1.10f: D2Client.0x6FB0C930
//  descdam handler 20: SKILLDESC_DamFunc1 with bAddElemDamage = 0 where DamFunc1 passes 1 (the rest of the difference is the assert helper's
//  line numbers, 2525/2526 against 2488/2489). Stock skilldesc.txt gives it fists of fire, claws of thunder and blades of ice, the Assassin's
//  elemental charge-up finishers, so the panel keeps the weapon's own elemental damage out of their line. The non-dual-wield arm is the same
//  tail call to SKILLDESC_DamFunc7. The original inlines DATATBLS_GetSkillsTxtRecordById here.
void __fastcall SKILLDESC_DamFunc20(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    int32_t nSkillId;
    D2SkillsTxt *pSkillsTxtRec;
    int32_t wSkillDesc;
    D2SkillDescTxt *pSkillDescRec;
    D2C_Skills nSkillIdFromSkill;
    int32_t nExtraA;
    int32_t nExtraB;

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    nSkillId = pSkillsTxt->nSkillId;
    pSkillsTxtRec = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxtRec)
    {
        return;
    }
    wSkillDesc = (int16_t)pSkillsTxtRec->wSkillDesc;
    if (wSkillDesc < 0 || wSkillDesc >= sgptDataTables->nSkillDescTxtRecordCount)
    {
        return;
    }
    pSkillDescRec = &sgptDataTables->pSkillDescTxt[wSkillDesc];
    if (!pSkillDescRec)
    {
        return;
    }

    if (!D2COMMON_UNITS_CanDualWield(pUnit))
    {
        SKILLDESC_DamFunc7(pUnit, pSkill, pSkillsTxt, nSkillLevel, nLeft, nY, nRight);
        return;
    }
    nSkillIdFromSkill = (D2C_Skills)D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillDescCpp, 2525);
    nExtraA = D2COMMON_SKILLS_EvaluateSkillDescFormula(pUnit, pSkillDescRec->dwDamCalc[0], nSkillIdFromSkill, nSkillLevel);
    nSkillIdFromSkill = (D2C_Skills)D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillDescCpp, 2526);
    nExtraB = D2COMMON_SKILLS_EvaluateSkillDescFormula(pUnit, pSkillDescRec->dwDamCalc[1], nSkillIdFromSkill, nSkillLevel);
    SKILLDESC_DrawDualWieldDamage(pUnit, pSkill, pSkillsTxt, nSkillLevel, SKILLDESC_AddTotalDamageCallback, nExtraA, nExtraB, nLeft, nY, nRight, 0);
}


//1.10f: D2Client.0x6FB0CA60
//  descdam handler 19: entry 19 of gpSkillDescDamFuncs, the largest of the three and the one whose two arms differ most:
//      CanDualWield -> the SKILLDESC_DamFunc1 pair of formula evaluations (lines 2562/2563) and DrawDualWieldDamage with bAddElemDamage = 1
//      otherwise    -> the single-hand arm written out in place, and it is not SKILLDESC_DamFunc7: the same two evaluations (lines 2572/2573),
//                      AddWeaponPhysDamage with nSrcDamOverride 0 and AddItemElemDamage, then the draw, with none of DamFunc7's four
//                      SKILLS_Get*Damage >> 8 adds and no post-scaling by srcdam
//  pSkillsTxtRec re-derives the record the function was already given: it reads nSkillId out of the pSkillsTxt parameter and indexes
//  sgptDataTables->pSkillsTxt with it, arriving back at the same row. The clamp and the fit test are the family's, and the equal arm is dead
//  for the same reason (`if (min >= max) max = min + 1;` runs first). The original inlines DATATBLS_GetSkillsTxtRecordById and
//  SKILLDESC_FormatRangeOrValue here.
void __fastcall SKILLDESC_DamFunc19(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    int32_t nSkillId;
    D2SkillsTxt *pSkillsTxtRec;
    int32_t wSkillDesc;
    D2SkillDescTxt *pSkillDescRec;
    D2C_Skills nSkillIdFromSkill;
    int32_t nExtraA;
    int32_t nExtraB;
    int32_t nMin;
    int32_t nMax;
    int32_t nMaxClamped;
    int32_t nDrawColor;
    int32_t nTextY;
    int32_t nWidth;
    int32_t nHeight;
    Unicode wszOut[32];

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    nSkillId = pSkillsTxt->nSkillId;
    pSkillsTxtRec = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxtRec)
    {
        return;
    }
    wSkillDesc = (int16_t)pSkillsTxtRec->wSkillDesc;
    if (wSkillDesc < 0 || wSkillDesc >= sgptDataTables->nSkillDescTxtRecordCount)
    {
        return;
    }
    pSkillDescRec = &sgptDataTables->pSkillDescTxt[wSkillDesc];
    if (!pSkillDescRec)
    {
        return;
    }

    if (D2COMMON_UNITS_CanDualWield(pUnit))
    {
        nSkillIdFromSkill = (D2C_Skills)D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillDescCpp, 2562);
        nExtraA = D2COMMON_SKILLS_EvaluateSkillDescFormula(pUnit, pSkillDescRec->dwDamCalc[0], nSkillIdFromSkill, nSkillLevel);
        nSkillIdFromSkill = (D2C_Skills)D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillDescCpp, 2563);
        nExtraB = D2COMMON_SKILLS_EvaluateSkillDescFormula(pUnit, pSkillDescRec->dwDamCalc[1], nSkillIdFromSkill, nSkillLevel);
        SKILLDESC_DrawDualWieldDamage(pUnit, pSkill, pSkillsTxt, nSkillLevel, SKILLDESC_AddTotalDamageCallback, nExtraA, nExtraB, nLeft, nY, nRight, 1);
        return;
    }

    nMin = 0;
    nMax = 0;
    nDrawColor = 0;
    nSkillIdFromSkill = (D2C_Skills)D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillDescCpp, 2572);
    nExtraA = D2COMMON_SKILLS_EvaluateSkillDescFormula(pUnit, pSkillDescRec->dwDamCalc[0], nSkillIdFromSkill, nSkillLevel);
    nSkillIdFromSkill = (D2C_Skills)D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillDescCpp, 2573);
    nExtraB = D2COMMON_SKILLS_EvaluateSkillDescFormula(pUnit, pSkillDescRec->dwDamCalc[1], nSkillIdFromSkill, nSkillLevel);
    SKILLDESC_AddWeaponPhysDamage(pUnit, &nMin, &nMax, &nDrawColor, nExtraA, nExtraB, pSkill, 0, NULL, 0);
    SKILLDESC_AddItemElemDamage(pUnit, &nMin, &nMax, &nDrawColor, 0, pSkill);

    nTextY = nY;
    nMaxClamped = nMax;
    if (nMin >= nMax)
    {
        nMaxClamped = nMin + 1;
    }
    SKILLDESC_FormatRangeOrValue(wszOut, nMin, nMaxClamped, 0);
    nWidth = 0;
    nHeight = 0;
    D2WIN_10131_GetTextDimensions(wszOut, &nWidth, &nHeight);
    nWidth = 11 * nWidth / 7;
    if (nWidth <= nRight - nLeft)
    {
        D2WIN_10127_SetFont(D2FONT_FONT16);
    }
    else
    {
        D2WIN_10127_SetFont(D2FONT_FONT6);
        nTextY = nY - 1;
    }
    CHAR_DrawTextCentered(gnBorderXDifference + nLeft, gnScreenHeight + gnBorderYDifference + nTextY - 480, gnBorderXDifference + nRight, wszOut, nDrawColor);
}


//1.10f: D2Client.0x6FB0CD40
//  descdam handler 2: entry 2 of gpSkillDescDamFuncs. The kick line: skills.txt dwMinDam shifted by nToHitShift - 8, plus STAT_ITEM_KICKDAMAGE,
//  printed as a range of width one. nToHitShift carries a bias here and not in SKILLDESC_DamFunc9, which shifts dwMinDam << nToHitShift with
//  no adjustment (eight is the fractional width of 8.8, so one of the two produces a whole number and the other a fixed-point one).
//  DIFFERENCE: the shift count is masked with & 31 as the x86 shl masks it (sub ecx, 8 / shl esi, cl at 0x6FB0CD6B, 0x6FB0CD74); C++ leaves
//  a negative count undefined, and any skills.txt HitShift below 8 would be one. Stock data gives descdam 2 to Kick alone, HitShift 8.
//  The single-value arm is dead, and this is the clearest instance: the test is `if (nMin == nMin + 1)` (lea ebp, [esi+1] at 0x6FB0CD91,
//  cmp esi, ebp at 0x6FB0CDA2), so the line always goes through SKILLDESC_FormatDamageRange and a kick always prints n-(n+1).
//  Reproduced as found. The original inlines SKILLDESC_FormatRangeOrValue here.
void __fastcall SKILLDESC_DamFunc2(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    uint32_t nMinDamShifted;
    int32_t nMin;
    int32_t nYCopy;
    int32_t nWidth;
    int32_t nHeight;
    int32_t nRightCopy;
    int32_t nLeftCopy;
    Unicode wszOut[32];

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    nMinDamShifted = pSkillsTxt->dwMinDam << ((pSkillsTxt->nToHitShift - 8) & 31);
    nMin = D2COMMON_STATLIST_UnitGetItemStatOrSkillStatValue(pUnit, STAT_ITEM_KICKDAMAGE, 0) + nMinDamShifted;
    nYCopy = nY;
    SKILLDESC_FormatRangeOrValue(wszOut, nMin, nMin + 1, 0);
    nWidth = 0;
    nHeight = 0;
    D2WIN_10131_GetTextDimensions(wszOut, &nWidth, &nHeight);
    nRightCopy = nRight;
    nLeftCopy = nLeft;
    nWidth = 11 * nWidth / 7;
    if (nWidth <= nRight - nLeft)
    {
        D2WIN_10127_SetFont(D2FONT_FONT16);
    }
    else
    {
        D2WIN_10127_SetFont(D2FONT_FONT6);
        nYCopy = nY - 1;
    }
    CHAR_DrawTextCentered(gnBorderXDifference + nLeftCopy, gnScreenHeight + gnBorderYDifference + nYCopy - 480, gnBorderXDifference + nRightCopy, wszOut, 0);
}


//1.10f: D2Client.0x6FB0CE90
//  descdam handler 21: entry 21 of gpSkillDescDamFuncs. The throwing-weapon damage line, drawn by the handler itself. It does not call
//  SKILLDESC_ComputeThrowDamage; the throwing-weapon stat block is written out a second time here, and this copy folds the weapon's own
//  strength and dexterity bonuses into damagepercent as a pre-percent. The original inlines DATATBLS_ApplyRatio here.
//      guard       pSkill, pSkillsTxt, pUnit->pInventory (+0x60), the left-hand weapon, ITEMS_CheckIfThrowable on it
//      pre-percent ITEMS_GetStrengthBonus * STAT_STRENGTH / 100 and ITEMS_GetDexBonus * STAT_DEXTERITY / 100
//      damage      STAT_ITEM_THROW_MINDAMAGE / ...MAXDAMAGE, scaled by `stat + stat * (damagepercent + STAT_ITEM_MIN/MAXDAMAGE_PERCENT) / 100`,
//                  with damagepercent clamped at >= -90 (cmp esi, 0FFFFFFA6h at 0x6FB0CF86)
//      mastery     only under STATE_THROWINGMASTERY (push 4Eh at 0x6FB0CFAB), added as DATATBLS_ApplyRatio(scaled, bonus, 100)
//      elem (unit) SKILLDESC_AddElemDamageByType against the unit's own elemental stats
//      srcdam      both ends scaled by pSkillsTxt->nSrcDam (mov dl, [ebx+1A5h] at 0x6FB0D14E) as DATATBLS_ApplyRatio(value, nSrcDam, 128)
//      elem (skill) SKILLDESC_AddElemDamageAndColor, which also sets the colour; being after the srcdam scaling, the skill's own elemental
//                  damage is not scaled by it
//  The scaling reads max(max, min) (cmp edi, ecx / jge at 0x6FB0D140, the same accumulation the callback does). The final clamp is the family's
//  `if (nMin >= nMax) nMax = nMin + 1` (lea esi, [edi+1] at 0x6FB0D25F); unlike its siblings the format that follows is a call to
//  SKILLDESC_FormatRangeOrValue (0x6FB0D283), not an inlined copy.
void __fastcall SKILLDESC_DamFunc21(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    D2InventoryStrc *pInventory;
    D2UnitStrc *pWeapon;
    int32_t nExtraPercent;
    int16_t nStrengthBonus;
    int16_t nDexBonus;
    int32_t nThrowMin;
    int32_t nThrowMax;
    int32_t nDamagePercent;
    int32_t nMinPercent;
    int32_t nMaxPercent;
    int32_t nMinScaled;
    int32_t nMaxScaled;
    int32_t nMasteryBonus;
    int32_t nMinTotal;
    int32_t nMaxTotal;
    int32_t nMaxAccum;
    int32_t nSrcDam;
    int32_t nMaxClamped;
    int32_t nWidth;
    int32_t nHeight;
    int32_t nTextY;
    int32_t nDrawColor;
    Unicode wszText[32];

    nExtraPercent = 0;
    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    nDrawColor = 0;
    pInventory = pUnit->pInventory;
    if (!pInventory)
    {
        return;
    }
    pWeapon = D2COMMON_INVENTORY_GetLeftHandWeapon(pInventory);
    if (!pWeapon)
    {
        return;
    }
    if (!D2COMMON_ITEMS_CheckIfThrowable(pWeapon))
    {
        return;
    }
    nStrengthBonus = D2COMMON_ITEMS_GetStrengthBonus(pWeapon);
    if (nStrengthBonus)
    {
        nExtraPercent = nStrengthBonus * D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_STRENGTH, 0) / 100;
    }
    nDexBonus = D2COMMON_ITEMS_GetDexBonus(pWeapon);
    if (nDexBonus)
    {
        nExtraPercent += nDexBonus * D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_DEXTERITY, 0) / 100;
    }
    nThrowMin = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_ITEM_THROW_MINDAMAGE, 0);
    nThrowMax = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_ITEM_THROW_MAXDAMAGE, 0);
    nDamagePercent = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_DAMAGEPERCENT, 0) + nExtraPercent;
    nMinPercent = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_ITEM_MINDAMAGE_PERCENT, 0);
    nMaxPercent = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_ITEM_MAXDAMAGE_PERCENT, 0);
    if (nDamagePercent <= -90)
    {
        nDamagePercent = -90;
    }
    nMinScaled = nThrowMin + nThrowMin * (nDamagePercent + nMinPercent) / 100;
    nMaxScaled = nThrowMax + nThrowMax * (nDamagePercent + nMaxPercent) / 100;
    nMasteryBonus = 0;
    if (D2COMMON_STATES_CheckState(pUnit, STATE_THROWINGMASTERY))
    {
        nMasteryBonus = D2COMMON_SKILLS_GetWeaponMasteryBonus(pUnit, pWeapon, pSkill, 1);
    }
    nMinTotal = nMinScaled + DATATBLS_ApplyRatio(nMinScaled, nMasteryBonus, 100);
    nMaxTotal = nMaxScaled + DATATBLS_ApplyRatio(nMaxScaled, nMasteryBonus, 100);
    SKILLDESC_AddElemDamageByType(pUnit, &nMinTotal, &nMaxTotal, pSkillsTxt->nEType);
    nMaxAccum = nMaxTotal;
    if (nMaxTotal < nMinTotal)
    {
        nMaxAccum = nMinTotal;
    }
    nSrcDam = pSkillsTxt->nSrcDam;
    nMinTotal = DATATBLS_ApplyRatio(nMinTotal, nSrcDam, 128);
    nMaxTotal = DATATBLS_ApplyRatio(nMaxAccum, nSrcDam, 128);
    SKILLDESC_AddElemDamageAndColor(pUnit, pSkillsTxt, nSkillLevel, &nMinTotal, &nMaxTotal, &nDrawColor);
    nMaxClamped = nMaxTotal;
    if (nMinTotal >= nMaxTotal)
    {
        nMaxClamped = nMinTotal + 1;
    }
    SKILLDESC_FormatRangeOrValue(wszText, nMinTotal, nMaxClamped, 0);
    nWidth = 0;
    nHeight = 0;
    D2WIN_10131_GetTextDimensions(wszText, &nWidth, &nHeight);
    nTextY = nY;
    nWidth = 11 * nWidth / 7;
    if (nWidth <= nRight - nLeft)
    {
        D2WIN_10127_SetFont(D2FONT_FONT16);
    }
    else
    {
        D2WIN_10127_SetFont(D2FONT_FONT6);
        nTextY = nY - 1;
    }
    CHAR_DrawTextCentered(gnBorderXDifference + nLeft, gnScreenHeight + gnBorderYDifference + nTextY - 480, gnBorderXDifference + nRight, wszText, nDrawColor);
}


//1.10f: D2Client.0x6FB0D330
//  descdam handler 3: entry 3 of gpSkillDescDamFuncs. The throw damage through the nine-argument callback, then both ends scaled by the skill's
//  srcdam byte as a 128ths fraction:
//      SKILLDESC_ComputeThrowDamage(pUnit, &min, &max, &color, 0, 0, pSkill, 0, NULL)
//      min = srcdam * min / 128;   max = srcdam * max / 128;
//      SKILLDESC_AddElemDamageAndColor(pUnit, pSkillsTxt, nSkillLevel, &min, &max, &color)
//  The scaling is a plain signed /128 (imul eax, esi, then cdq / and edx, 7Fh / sar eax, 7 at 0x6FB0D387), not DATATBLS_ApplyRatio.
//  The single-value arm is dead: the clamp `if (nMin >= nMax) nMax = nMin + 1` runs first (lea edi, [esi+1] at 0x6FB0D3E8) and the equality
//  test after it compares the same two registers (cmp esi, edi at 0x6FB0D403), so a lone number renders as n-(n+1). Reproduced as found.
//  The original inlines SKILLDESC_FormatRangeOrValue here.
void __fastcall SKILLDESC_DamFunc3(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    int32_t nSrcDam;
    int32_t nThrowMin;
    int32_t nThrowMax;
    int32_t nMaxClamped;
    int32_t nWidth;
    int32_t nHeight;
    int32_t nTextY;
    int32_t nDrawColor;
    Unicode wszText[32];

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    nSrcDam = pSkillsTxt->nSrcDam;
    nDrawColor = 0;
    SKILLDESC_ComputeThrowDamage(pUnit, &nThrowMin, &nThrowMax, &nDrawColor, 0, 0, pSkill, 0, NULL);
    nThrowMin = nSrcDam * nThrowMin / 128;
    nThrowMax = nSrcDam * nThrowMax / 128;
    SKILLDESC_AddElemDamageAndColor(pUnit, pSkillsTxt, nSkillLevel, &nThrowMin, &nThrowMax, &nDrawColor);
    nMaxClamped = nThrowMax;
    if (nThrowMin >= nThrowMax)
    {
        nMaxClamped = nThrowMin + 1;
    }
    SKILLDESC_FormatRangeOrValue(wszText, nThrowMin, nMaxClamped, 0);
    nWidth = 0;
    nHeight = 0;
    D2WIN_10131_GetTextDimensions(wszText, &nWidth, &nHeight);
    nTextY = nY;
    nWidth = 11 * nWidth / 7;
    if (nWidth <= nRight - nLeft)
    {
        D2WIN_10127_SetFont(D2FONT_FONT16);
    }
    else
    {
        D2WIN_10127_SetFont(D2FONT_FONT6);
        nTextY = nY - 1;
    }
    CHAR_DrawTextCentered(gnBorderXDifference + nLeft, gnScreenHeight + gnBorderYDifference + nTextY - 480, gnBorderXDifference + nRight, wszText, nDrawColor);
}


//1.10f: D2Client.0x6FB0D4F0
//  Damage for a thrown weapon: the third implementation of the shared nine-argument callback, the one SKILLDESC_DamFunc13 hands to
//  SKILLDESC_DrawDualWieldDamage as the walker's pfnCompute, and called directly by three other descdam handlers. Two separate arms, chosen by
//  ITEMS_CheckIfThrowable and then ITEMS_CheckItemTypeId(ITEMTYPE_MISSILE_POTION). The original inlines DATATBLS_ApplyRatio here.
//  A throwing potion takes its damage from the missile, not from the item: MISSILE_GetMin/MaxDamage plus MISSILE_GetMin/MaxElemDamage, and the
//  colour from MISSILE_GetElemTypeFromMissileId. Two things here disagree with the rest of the panel, and both are in the binary:
//      element 1 Fire      -> 1 UI_COLOR_RED        (agrees)
//      element 2 Lightning -> 4 UI_COLOR_DARK_GOLD  (everywhere else 9 YELLOW)
//      element 3 Magic     -> falls to default, colour stays 0 UI_COLOR_WHITE (everywhere else 3 BLUE)
//      element 4 Cold      -> 3 UI_COLOR_BLUE       (agrees)
//      element 5 Poison    -> 2 UI_COLOR_LIGHT_GREEN (agrees)
//  and the poison arm divides where the rest of the panel multiplies: both ends by MISSILE_GetElementalLength(...) / 25 (frames to seconds),
//  clamped to at least 1, so a throwing potion shows poison damage per second while SKILLDESC_AddElemDamageAndColor shows a total.
//  A throwing weapon uses the item stats instead: STAT_ITEM_THROW_MINDAMAGE / MAXDAMAGE, plus the item's strbonus and dexbonus against STRENGTH
//  and DEXTERITY, plus STAT_DAMAGEPERCENT (clamped at >= -90 like the melee routine), then STAT_ITEM_MIN/MAXDAMAGE_PERCENT. The mastery is added
//  only while the unit has STATE_THROWINGMASTERY, and nExtraDamagePercent plus SKILLS_GetWeaponMasteryBonus together are what the percentage
//  multiply below applies. The max half clamps the maximum up to the minimum (`if (nMaxAfterBonus < *pnMin) *pnMax = *pnMin;`).
//  Both out-parameters are zeroed on entry. result is a multi-role slot (pSkill, the inventory pointer, the left-hand weapon through the guard
//  chain, the throwable test, and in the potion arm the switch selector itself); the `result = (int)pnColor` in case 1 is a register reuse.
//  void: eax is the `result` slot: the switch selector, or a stack address for lightning and max/ticks for poison; no caller reads it.
void __fastcall SKILLDESC_ComputeThrowDamage(D2UnitStrc *pUnit, int32_t *pnMin, int32_t *pnMax, int32_t *pnColor, int32_t nExtraDamagePercent, int32_t nUnusedA, D2SkillStrc *pSkill, int32_t nUnusedB, D2UnitStrc *pWeapon)
{
    int32_t result;
    D2UnitStrc *pWeaponUsed;
    int32_t nMissileId;
    int32_t nMinElem;
    int32_t nMaxElem;
    int32_t nMinTotal;
    int32_t nMaxTotal;
    int32_t nPoisonTicks;
    int32_t nMinScaled;
    int32_t nMinOut;
    int32_t nMaxOut;
    int32_t nStatBonus;
    int16_t nStrengthBonus;
    int16_t nDexBonus;
    int32_t nDamagePercent;
    int32_t nMinDamagePercent;
    int32_t nMaxDamagePercent;
    int32_t nExtraPercent;
    int32_t nMin;
    int32_t nMinBonus;
    int32_t nMaxMasked;
    int32_t nMaxBonus;
    int32_t nMaxAfterBonus;
    int32_t nElemMin;
    int32_t nElemMax;

    *pnMin = 0;
    *pnMax = 0;
    result = (int32_t)pSkill;
    if (!pSkill)
    {
        return;
    }

    //  the guard chain: skill, inventory, left hand -- each fetched, tested, and returned as the failure value
    pWeaponUsed = pWeapon;
    if (!pWeapon)
    {
        result = (int32_t)pUnit->pInventory;
        if (!result)
        {
            return;
        }
        result = (int32_t)D2COMMON_INVENTORY_GetLeftHandWeapon(pUnit->pInventory);
        pWeapon = (D2UnitStrc *)result;
        if (!result)
        {
            return;
        }
        pWeaponUsed = (D2UnitStrc *)result;
    }

    result = D2COMMON_ITEMS_CheckIfThrowable(pWeaponUsed);
    if (!result)
    {
        return;
    }

    if (D2COMMON_ITEMS_CheckItemTypeId(pWeaponUsed, ITEMTYPE_MISSILE_POTION))
    {
        nMissileId = D2COMMON_ITEMS_GetMissileType(pWeaponUsed);
        *pnColor = UI_COLOR_WHITE;
        nMinElem = D2COMMON_MISSILE_GetMinElemDamage(NULL, pUnit, nMissileId, 1);
        nMinTotal = D2COMMON_MISSILE_GetMinDamage(NULL, pUnit, nMissileId, 1) + nMinElem;
        nMaxElem = D2COMMON_MISSILE_GetMaxElemDamage(NULL, pUnit, nMissileId, 1);
        nMaxTotal = D2COMMON_MISSILE_GetMaxDamage(NULL, pUnit, nMissileId, 1) + nMaxElem;
        result = D2COMMON_MISSILE_GetElemTypeFromMissileId(nMissileId) - 1;
        nMinScaled = nMinTotal;
        switch (result)
        {
        case 0:                             // Fire
            *pnColor = UI_COLOR_RED;
            break;
        case 1:                             // Lightning: 4, everywhere else 9
            result = (int32_t)pnColor;
            *pnColor = UI_COLOR_DARK_GOLD;
            break;
        case 3:                             // Cold
            *pnColor = UI_COLOR_BLUE;
            break;
        case 4:                             // Poison: per second, not total
            *pnColor = UI_COLOR_LIGHT_GREEN;
            nPoisonTicks = D2COMMON_MISSILE_GetElementalLength(0, pUnit, nMissileId, 1) / 25;
            if (nPoisonTicks <= 0)
            {
                nPoisonTicks = 1;
            }
            nMinScaled = nMinTotal / nPoisonTicks;
            result = nMaxTotal / nPoisonTicks;
            nMaxTotal /= nPoisonTicks;
            break;
        default:                            // Magic stays UI_COLOR_WHITE
            break;
        }
        nMinOut = nMinScaled >> 8;
        nMaxOut = nMaxTotal >> 8;
        if (nMaxOut <= nMinOut)
        {
            nMaxOut = nMinOut;
        }
        *pnMin = nMinOut;
        *pnMax = nMaxOut;
        return;
    }

    nStatBonus = 0;
    nStrengthBonus = D2COMMON_ITEMS_GetStrengthBonus(pWeaponUsed);
    if (nStrengthBonus)
    {
        nStatBonus = nStrengthBonus * D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_STRENGTH, 0) / 100;
    }
    nDexBonus = D2COMMON_ITEMS_GetDexBonus(pWeaponUsed);
    if (nDexBonus)
    {
        nStatBonus += nDexBonus * D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_DEXTERITY, 0) / 100;
    }
    *pnMin = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_ITEM_THROW_MINDAMAGE, 0);
    *pnMax = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_ITEM_THROW_MAXDAMAGE, 0);
    nDamagePercent = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_DAMAGEPERCENT, 0) + nStatBonus;
    nMinDamagePercent = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_ITEM_MINDAMAGE_PERCENT, 0);
    nMaxDamagePercent = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_ITEM_MAXDAMAGE_PERCENT, 0);
    if (nDamagePercent <= -90)
    {
        nDamagePercent = -90;
    }
    *pnMin += *pnMin * (nDamagePercent + nMinDamagePercent) / 100;
    *pnMax += *pnMax * (nMaxDamagePercent + nDamagePercent) / 100;
    nExtraPercent = nExtraDamagePercent;
    if (D2COMMON_STATES_CheckState(pUnit, STATE_THROWINGMASTERY))
    {
        nExtraPercent = nExtraDamagePercent + D2COMMON_SKILLS_GetWeaponMasteryBonus(pUnit, pWeapon, pSkill, 1);
    }
    nMin = *pnMin;
    nMinBonus = DATATBLS_ApplyRatio(nMin, nExtraPercent, 100);
    *pnMin = nMinBonus + nMin;
    nMaxMasked = *pnMax;
    nMaxBonus = DATATBLS_ApplyRatio(nMaxMasked, nExtraPercent, 100);
    nMaxAfterBonus = nMaxBonus + *pnMax;
    *pnMax = nMaxAfterBonus;
    if (nMaxAfterBonus < *pnMin)
    {
        *pnMax = *pnMin;
    }
    nElemMin = 0;
    nElemMax = 0;
    SKILLDESC_AddItemElemDamage(pUnit, &nElemMin, &nElemMax, pnColor, pWeapon, pSkill);
    result = nElemMax;
    if (nElemMax < nElemMin)
    {
        result = nElemMin;
    }
    *pnMin += nElemMin;
    *pnMax += result;
}


//1.10f: D2Client.0x6FB0D920
//  descdam handler 4: entry 4 of gpSkillDescDamFuncs. The off-hand line of the dual-wield pair: SKILLDESC_PrepareOffHandWeapon makes the off
//  hand current, the nine-argument callback runs against that weapon (passed in, so no srcdam scaling happens inside it and none is applied
//  here, which is the difference from descdam 3), SKILLDESC_PrepareMainHandWeapon puts the pair back, and the answer is formatted and drawn
//  as descdam 3 draws it. The single-value arm is dead again (lea ebx, [esi+1] at 0x6FB0D99C clamps, cmp esi, ebx at 0x6FB0D9B7 tests the
//  same two registers). The original inlines SKILLDESC_FormatRangeOrValue here.
void __fastcall SKILLDESC_DamFunc4(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    D2UnitStrc *pWeapon;
    int32_t nThrowMin;
    int32_t nThrowMax;
    int32_t nDrawColor;
    int32_t nMaxClamped;
    int32_t nWidth;
    int32_t nHeight;
    int32_t nTextY;
    Unicode wszText[32];

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    nThrowMin = 0;
    nThrowMax = 0;
    nDrawColor = 0;
    pWeapon = SKILLDESC_PrepareOffHandWeapon(pUnit);
    if (!pWeapon)
    {
        return;
    }
    SKILLDESC_ComputeThrowDamage(pUnit, &nThrowMin, &nThrowMax, &nDrawColor, 0, 0, pSkill, 0, pWeapon);
    SKILLDESC_PrepareMainHandWeapon(pUnit);
    nMaxClamped = nThrowMax;
    if (nThrowMin >= nThrowMax)
    {
        nMaxClamped = nThrowMin + 1;
    }
    SKILLDESC_FormatRangeOrValue(wszText, nThrowMin, nMaxClamped, 0);
    nWidth = 0;
    nHeight = 0;
    D2WIN_10131_GetTextDimensions(wszText, &nWidth, &nHeight);
    nTextY = nY;
    nWidth = 11 * nWidth / 7;
    if (nWidth <= nRight - nLeft)
    {
        D2WIN_10127_SetFont(D2FONT_FONT16);
    }
    else
    {
        D2WIN_10127_SetFont(D2FONT_FONT6);
        nTextY = nY - 1;
    }
    CHAR_DrawTextCentered(gnBorderXDifference + nLeft, gnScreenHeight + gnBorderYDifference + nTextY - 480, gnBorderXDifference + nRight, wszText, nDrawColor);
}


//1.10f: D2Client.0x6FB0DAB0
//  descdam handler 22: entry 22 of gpSkillDescDamFuncs, the one handler of the family that draws two lines. Both ranges are built first and
//  the presence of each decides the shape:
//      physical  SKILLDESC_ComputeThrowDamage, then both ends scaled by srcdam /128 through DATATBLS_ApplyRatio
//      elemental SKILLDESC_AddElemDamageAndColor, which also sets the colour
//      both ends   two lines, SKILLDESC_DrawTwoLines
//      phys only   one line, clamped, the throw colour (nMin||nMax and no elemental)
//      elem only   one line, clamped, the elemental colour (neither physical end)
//  The single-value arm is live here and dead in every clamped sibling, and it is the two-line arm that keeps it: both ranges are formatted
//  there with no clamp before either, so nMin == nMax is reachable on both lines and a single value prints one number. The two one-line arms
//  clamp first (lea edi, [ebx+1] at 0x6FB0DC4D, lea esi, [ebx+1] at 0x6FB0DD4F) and print n-(n+1), as the siblings do. The original inlines
//  DATATBLS_ApplyRatio and SKILLDESC_FormatRangeOrValue here.
void __fastcall SKILLDESC_DamFunc22(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    int32_t nSrcDam;
    int32_t nThrowMin;
    int32_t nThrowMax;
    int32_t nThrowColor;
    int32_t nElemMin;
    int32_t nElemMax;
    int32_t nElemColor;
    int32_t nMaxClamped;
    int32_t nWidth;
    int32_t nHeight;
    int32_t nTextY;
    Unicode wszText[32];
    Unicode wszElem[32];

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    nSrcDam = pSkillsTxt->nSrcDam;
    nThrowMin = 0;
    nThrowMax = 0;
    nThrowColor = 0;
    SKILLDESC_ComputeThrowDamage(pUnit, &nThrowMin, &nThrowMax, &nThrowColor, 0, 0, pSkill, 0, NULL);
    nThrowMin = DATATBLS_ApplyRatio(nThrowMin, nSrcDam, 128);
    nThrowMax = DATATBLS_ApplyRatio(nThrowMax, nSrcDam, 128);
    nElemMin = 0;
    nElemMax = 0;
    nElemColor = 0;
    SKILLDESC_AddElemDamageAndColor(pUnit, pSkillsTxt, nSkillLevel, &nElemMin, &nElemMax, &nElemColor);
    if (nThrowMin || nThrowMax)
    {
        if (nElemMin || nElemMax)
        {
            //  both kinds of damage: the physical line over the elemental
            SKILLDESC_FormatRangeOrValue(wszText, nThrowMin, nThrowMax, 0);
            SKILLDESC_FormatRangeOrValue(wszElem, nElemMin, nElemMax, 0);
            SKILLDESC_DrawTwoLines(wszText, wszElem, nLeft, nY, nRight, nThrowColor, nElemColor);
        }
        else
        {
            //  physical only
            nMaxClamped = nThrowMax;
            if (nThrowMin >= nThrowMax)
            {
                nMaxClamped = nThrowMin + 1;
            }
            SKILLDESC_FormatRangeOrValue(wszText, nThrowMin, nMaxClamped, 0);
            nWidth = 0;
            nHeight = 0;
            D2WIN_10131_GetTextDimensions(wszText, &nWidth, &nHeight);
            nTextY = nY;
            nWidth = 11 * nWidth / 7;
            if (nWidth <= nRight - nLeft)
            {
                D2WIN_10127_SetFont(D2FONT_FONT16);
            }
            else
            {
                D2WIN_10127_SetFont(D2FONT_FONT6);
                nTextY = nY - 1;
            }
            CHAR_DrawTextCentered(gnBorderXDifference + nLeft, gnScreenHeight + gnBorderYDifference + nTextY - 480, gnBorderXDifference + nRight, wszText, nThrowColor);
        }
    }
    else
    {
        //  elemental only
        nMaxClamped = nElemMax;
        if (nElemMin >= nElemMax)
        {
            nMaxClamped = nElemMin + 1;
        }
        SKILLDESC_FormatRangeOrValue(wszText, nElemMin, nMaxClamped, 0);
        nWidth = 0;
        nHeight = 0;
        D2WIN_10131_GetTextDimensions(wszText, &nWidth, &nHeight);
        nTextY = nY;
        nWidth = 11 * nWidth / 7;
        if (nWidth <= nRight - nLeft)
        {
            D2WIN_10127_SetFont(D2FONT_FONT16);
        }
        else
        {
            D2WIN_10127_SetFont(D2FONT_FONT6);
            nTextY = nY - 1;
        }
        CHAR_DrawTextCentered(gnBorderXDifference + nLeft, gnScreenHeight + gnBorderYDifference + nTextY - 480, gnBorderXDifference + nRight, wszText, nElemColor);
    }
}


//1.10f: D2Client.0x6FB0DEC0
//  descdam handler 5: entry 5 of gpSkillDescDamFuncs. The element-coloured line. The colour is chosen from nEType before any damage is
//  computed, a five-way jump through switch (nEType - 1), and it is the same map SKILLDESC_AddElemDamageAndColor carries:
//      1 Fire -> 1 RED        2 Lightning -> 9 YELLOW
//      3 Magic, 4 Cold -> 3 BLUE   5 Poison -> 2 LIGHT_GREEN
//      anything else -> 0 WHITE
//  Then, when srcdam is nonzero, the physical half is built from the item with the callback's own scale switched off (nSrcDamOverride = 128,
//  the helper's "leave it" argument) and scaled here instead: SKILLDESC_AddWeaponPhysDamage then SKILLDESC_AddItemElemDamage, then both ends
//  by srcdam as a plain /128 (cdq / and edx, 7Fh / sar eax, 7 at 0x6FB0DF7F). The skill's own damage is added on top: SKILLS_GetMin/MaxPhysDamage
//  >> 8 plus the two srcdam-scaled halves, then the elemental pair with the >> 8 shared between the arms, except poison (cmp al, 5 at
//  0x6FB0DFD7), where both ends are multiplied by SKILLS_GetElementalLength before the shift, as AddElemDamageAndColor's poison arm does.
//  The clamp closes it; the equality arm is dead again (lea edi, [ebx+1] at 0x6FB0E063, cmp ebx, edi at 0x6FB0E07D). The original inlines
//  SKILLDESC_FormatRangeOrValue here.
void __fastcall SKILLDESC_DamFunc5(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    int32_t nSrcDam;
    int32_t nElemColor;
    int32_t nSrcBonusMin;
    int32_t nSrcBonusMax;
    int32_t nPhysMin;
    int32_t nPhysMax;
    int32_t nElemLength;
    int32_t nElemMin;
    int32_t nElemMax;
    int32_t nMin;
    int32_t nMax;
    int32_t nWidth;
    int32_t nHeight;
    int32_t nTextY;
    int32_t nDrawColor;
    D2C_Skills nSkillId;
    Unicode wszText[32];

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    nSrcBonusMin = 0;
    nSrcBonusMax = 0;
    nSrcDam = pSkillsTxt->nSrcDam;
    switch (pSkillsTxt->nEType - 1)
    {
    case 0:
        nElemColor = UI_COLOR_RED;
        break;
    case 1:
        nElemColor = UI_COLOR_YELLOW;
        break;
    case 2:
    case 3:
        nElemColor = UI_COLOR_BLUE;
        break;
    case 4:
        nElemColor = UI_COLOR_LIGHT_GREEN;
        break;
    default:
        nElemColor = UI_COLOR_WHITE;
        break;
    }
    nDrawColor = nElemColor;
    if (nSrcDam)
    {
        nPhysMin = 0;
        nPhysMax = 0;
        SKILLDESC_AddWeaponPhysDamage(pUnit, &nPhysMin, &nPhysMax, &nDrawColor, 0, 0, pSkill, 128, NULL, 0);
        SKILLDESC_AddItemElemDamage(pUnit, &nPhysMin, &nPhysMax, &nDrawColor, 0, pSkill);
        nSrcBonusMin = nSrcDam * nPhysMin / 128;
        nSrcBonusMax = nSrcDam * nPhysMax / 128;
    }
    nPhysMin = (D2COMMON_SKILLS_GetMinPhysDamage(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel, 0) >> 8) + nSrcBonusMin;
    nPhysMax = (D2COMMON_SKILLS_GetMaxPhysDamage(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel, 0) >> 8) + nSrcBonusMax;
    nSkillId = (D2C_Skills)pSkillsTxt->nSkillId;
    if (pSkillsTxt->nEType == 5)
    {
        nElemLength = D2COMMON_SKILLS_GetElementalLength(pUnit, nSkillId, nSkillLevel, 1);
        nElemMin = (nElemLength * D2COMMON_SKILLS_GetMinElemDamage(pUnit, nSkillId, nSkillLevel, 1)) >> 8;
        nElemMax = nElemLength * D2COMMON_SKILLS_GetMaxElemDamage(pUnit, nSkillId, nSkillLevel, 1);
    }
    else
    {
        nElemMin = D2COMMON_SKILLS_GetMinElemDamage(pUnit, nSkillId, nSkillLevel, 1) >> 8;
        nElemMax = D2COMMON_SKILLS_GetMaxElemDamage(pUnit, nSkillId, nSkillLevel, 1);
    }
    nMin = nElemMin + nPhysMin;
    nMax = (nElemMax >> 8) + nPhysMax;
    nTextY = nY;
    if (nMin >= nMax)
    {
        nMax = nMin + 1;
    }
    SKILLDESC_FormatRangeOrValue(wszText, nMin, nMax, 0);
    nWidth = 0;
    nHeight = 0;
    D2WIN_10131_GetTextDimensions(wszText, &nWidth, &nHeight);
    nWidth = 11 * nWidth / 7;
    if (nWidth <= nRight - nLeft)
    {
        D2WIN_10127_SetFont(D2FONT_FONT16);
    }
    else
    {
        D2WIN_10127_SetFont(D2FONT_FONT6);
        nTextY = nY - 1;
    }
    CHAR_DrawTextCentered(gnBorderXDifference + nLeft, gnScreenHeight + gnBorderYDifference + nTextY - 480, gnBorderXDifference + nRight, wszText, nDrawColor);
}


//1.10f: D2Client.0x6FB0E180
//  descdam handler 24: entry 24 of gpSkillDescDamFuncs. The element-coloured line written out a second time: SKILLDESC_DamFunc5 is its twin,
//  and the two differ only in where the srcdam scaling meets the two physical getters. Here the getters come first and the scaled halves are
//  added to them inside the `if (srcdam)` block; there the scaled halves are computed first and the getters added after. Same numbers.
//  The rest is the twin's round: the colour chosen from nEType before any damage (the same 1 / 9 / 3 / 2 / 0 map), the physical half built
//  from the item with nSrcDamOverride 128 through AddWeaponPhysDamage and AddItemElemDamage, the poison arm multiplying both ends by
//  SKILLS_GetElementalLength before the >> 8 (cmp al, 5 at 0x6FB0E29F). The single-value arm is dead: the clamp runs first (lea ebx, [ebp+1] at
//  0x6FB0E320) and the following test compares the same two registers (cmp ebp, ebx at 0x6FB0E33A). The original inlines
//  SKILLDESC_FormatRangeOrValue here.
void __fastcall SKILLDESC_DamFunc24(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    int32_t nSrcDam;
    int32_t nElemColor;
    int32_t nMinPhysDamage;
    int32_t nMaxPhysDamage;
    int32_t nPhysMin;
    int32_t nPhysMax;
    int32_t nElemLength;
    int32_t nElemMin;
    int32_t nElemMax;
    int32_t nMin;
    int32_t nMax;
    int32_t nMaxClamped;
    int32_t nWidth;
    int32_t nHeight;
    int32_t nTextY;
    int32_t nDrawColor;
    D2C_Skills nSkillId;
    Unicode wszText[32];

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    nSrcDam = pSkillsTxt->nSrcDam;
    switch (pSkillsTxt->nEType)
    {
    case 1:
        nElemColor = UI_COLOR_RED;
        break;
    case 2:
        nElemColor = UI_COLOR_YELLOW;
        break;
    case 3:
    case 4:
        nElemColor = UI_COLOR_BLUE;
        break;
    case 5:
        nElemColor = UI_COLOR_LIGHT_GREEN;
        break;
    default:
        nElemColor = UI_COLOR_WHITE;
        break;
    }
    nDrawColor = nElemColor;
    nMinPhysDamage = D2COMMON_SKILLS_GetMinPhysDamage(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel, 0) >> 8;
    nMaxPhysDamage = D2COMMON_SKILLS_GetMaxPhysDamage(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel, 0) >> 8;
    if (nSrcDam)
    {
        nPhysMin = 0;
        nPhysMax = 0;
        SKILLDESC_AddWeaponPhysDamage(pUnit, &nPhysMin, &nPhysMax, &nDrawColor, 0, 0, pSkill, 128, NULL, 0);
        SKILLDESC_AddItemElemDamage(pUnit, &nPhysMin, &nPhysMax, &nDrawColor, 0, pSkill);
        nMinPhysDamage += nSrcDam * nPhysMin / 128;
        nMaxPhysDamage += nSrcDam * nPhysMax / 128;
    }
    nSkillId = (D2C_Skills)pSkillsTxt->nSkillId;
    if (pSkillsTxt->nEType == 5)
    {
        nElemLength = D2COMMON_SKILLS_GetElementalLength(pUnit, nSkillId, nSkillLevel, 1);
        nElemMin = (nElemLength * D2COMMON_SKILLS_GetMinElemDamage(pUnit, nSkillId, nSkillLevel, 1)) >> 8;
        nElemMax = nElemLength * D2COMMON_SKILLS_GetMaxElemDamage(pUnit, nSkillId, nSkillLevel, 1);
    }
    else
    {
        nElemMin = D2COMMON_SKILLS_GetMinElemDamage(pUnit, nSkillId, nSkillLevel, 1) >> 8;
        nElemMax = D2COMMON_SKILLS_GetMaxElemDamage(pUnit, nSkillId, nSkillLevel, 1);
    }
    nMin = nElemMin + nMinPhysDamage;
    nMax = (nElemMax >> 8) + nMaxPhysDamage;
    nTextY = nY;
    nMaxClamped = nMax;
    if (nMin >= nMax)
    {
        nMaxClamped = nMin + 1;
    }
    SKILLDESC_FormatRangeOrValue(wszText, nMin, nMaxClamped, 0);
    nWidth = 0;
    nHeight = 0;
    D2WIN_10131_GetTextDimensions(wszText, &nWidth, &nHeight);
    nWidth = 11 * nWidth / 7;
    if (nWidth <= nRight - nLeft)
    {
        D2WIN_10127_SetFont(D2FONT_FONT16);
    }
    else
    {
        D2WIN_10127_SetFont(D2FONT_FONT6);
        nTextY = nY - 1;
    }
    CHAR_DrawTextCentered(gnBorderXDifference + nLeft, gnScreenHeight + gnBorderYDifference + nTextY - 480, gnBorderXDifference + nRight, wszText, nDrawColor);
}


//1.10f: D2Client.0x6FB0E440
//  descdam handler 23: entry 23 of gpSkillDescDamFuncs; in stock data its only user is rabies. The three-way line: the physical side comes from
//  SKILLDESC_AddWeaponPhysDamage with nSrcDamOverride 128 plus SKILLDESC_AddItemElemDamage, the elemental side from
//  SKILLDESC_AddElemDamageAndColor, and the arms are
//      physical empty  the elemental line, clamped, in the elemental colour
//      elemental empty the physical line, clamped, in the weapon's colour
//      both            two lines through SKILLDESC_DrawTwoLines
//  The two one-line arms jump to one CHAR_DrawTextCentered (nDrawLeft, nDrawRight, nDrawY and the colour are set just before the jump); the C
//  keeps the label, as the dispatcher's shared tails do. The format rule of the family, spelled out by this function alone: the two one-line
//  arms clamp before formatting (lea edi, [ebx+1] at 0x6FB0E504 and lea edi, [eax+1] at 0x6FB0E60C) and call SKILLDESC_FormatRangeOrValue out
//  of line, so their equality arms are dead; the two-line arm does not clamp, and its two inlined copies of that helper keep live equality
//  arms, printing a single value for either side.
void __fastcall SKILLDESC_DamFunc23(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    int32_t nPhysMin;
    int32_t nPhysMax;
    int32_t nPhysColor;
    int32_t nElemMin;
    int32_t nElemMax;
    int32_t nElemColor;
    int32_t nMaxClamped;
    int32_t nDrawLeft;
    int32_t nDrawRight;
    int32_t nDrawY;
    int32_t nDrawColor;
    int32_t nWidth;
    int32_t nHeight;
    Unicode wszText[32];
    Unicode wszPhys[32];
    Unicode wszElem[32];

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    nPhysMin = 0;
    nPhysMax = 0;
    nPhysColor = 0;
    SKILLDESC_AddWeaponPhysDamage(pUnit, &nPhysMin, &nPhysMax, &nPhysColor, 0, 0, pSkill, 128, NULL, 0);
    SKILLDESC_AddItemElemDamage(pUnit, &nPhysMin, &nPhysMax, &nPhysColor, 0, pSkill);
    nElemMin = 0;
    nElemMax = 0;
    nElemColor = 0;
    SKILLDESC_AddElemDamageAndColor(pUnit, pSkillsTxt, nSkillLevel, &nElemMin, &nElemMax, &nElemColor);
    if (!nPhysMin && !nPhysMax)
    {
        //  no physical damage: the elemental line
        nDrawRight = nRight;
        nDrawLeft = nLeft;
        nMaxClamped = nElemMax;
        if (nElemMin >= nElemMax)
        {
            nMaxClamped = nElemMin + 1;
        }
        SKILLDESC_FormatRangeOrValue(wszText, nElemMin, nMaxClamped, 0);
        nWidth = 0;
        nHeight = 0;
        D2WIN_10131_GetTextDimensions(wszText, &nWidth, &nHeight);
        nWidth = 11 * nWidth / 7;
        nDrawY = nY;
        if (nWidth <= nRight - nLeft)
        {
            D2WIN_10127_SetFont(D2FONT_FONT16);
        }
        else
        {
            D2WIN_10127_SetFont(D2FONT_FONT6);
            nDrawY = nY - 1;
        }
        nDrawColor = nElemColor;
        goto DRAW_ONE_LINE;
    }
    if (!nElemMin && !nElemMax)
    {
        //  no elemental damage: the physical line
        nDrawRight = nRight;
        nDrawLeft = nLeft;
        nMaxClamped = nPhysMax;
        if (nPhysMin >= nPhysMax)
        {
            nMaxClamped = nPhysMin + 1;
        }
        SKILLDESC_FormatRangeOrValue(wszText, nPhysMin, nMaxClamped, 0);
        nWidth = 0;
        nHeight = 0;
        D2WIN_10131_GetTextDimensions(wszText, &nWidth, &nHeight);
        nWidth = 11 * nWidth / 7;
        nDrawY = nY;
        if (nWidth <= nRight - nLeft)
        {
            D2WIN_10127_SetFont(D2FONT_FONT16);
        }
        else
        {
            D2WIN_10127_SetFont(D2FONT_FONT6);
            nDrawY = nY - 1;
        }
        nDrawColor = nPhysColor;
        goto DRAW_ONE_LINE;
    }
    //  both: the physical line over the elemental one, unclamped formats
    SKILLDESC_FormatRangeOrValue(wszPhys, nPhysMin, nPhysMax, 0);
    SKILLDESC_FormatRangeOrValue(wszElem, nElemMin, nElemMax, 0);
    SKILLDESC_DrawTwoLines(wszPhys, wszElem, nLeft, nY, nRight, nPhysColor, nElemColor);
    return;

//  shared tail of the two one-line arms
DRAW_ONE_LINE:
    CHAR_DrawTextCentered(gnBorderXDifference + nDrawLeft, gnScreenHeight + gnBorderYDifference + nDrawY - 480, gnBorderXDifference + nDrawRight, wszText, nDrawColor);
}


//1.10f: D2Client.0x6FB0E7B0
//  descdam handler 13: entry 13 of gpSkillDescDamFuncs, the smallest of the family. One formula out of the skill's own dwCalc[0] (through
//  SKILLS_EvaluateSkillFormula, the plain evaluator, not the assert-carrying one the DamFunc7 family uses), then the walker with
//  SKILLDESC_ComputeThrowDamage as its pfnCompute, nExtraB 0 and bAddElemDamage 1, and the skill in the walker's pSkill. It is the only caller
//  of SKILLDESC_DrawDualWieldDamage that hands it the throwing-damage callback (DamFunc1, DamFunc20 and
//  DamFunc19 pass SKILLDESC_AddTotalDamageCallback), so descdam 13 is the thrown-weapon line of the dual-wield pair.
void __fastcall SKILLDESC_DamFunc13(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    int32_t nExtraA;

    nExtraA = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel);
    SKILLDESC_DrawDualWieldDamage(pUnit, pSkill, pSkillsTxt, nSkillLevel, SKILLDESC_ComputeThrowDamage, nExtraA, 0, nLeft, nY, nRight, 1);
}


//1.10f: D2Client.0x6FB0E800
//  descdam handler 8: entry 8 of gpSkillDescDamFuncs. Its own walk of the two tables, guard by guard: skills.txt by id (nSkillId >=
//  SKILL_ATTACK, then the record count), the record's wSkillDesc into skilldesc.txt by its count, both records null-checked as found. Then the
//  elemental pair at the skill's level, the five-way colour switch, and the two formulas off the skilldesc record's dwDamCalc[0] and [1], each
//  through SKILLS_GetSkillIdFromSkill with the __FILE__ and lines 3114 / 3119, and each floored at 1 (`if (!nDamCalc) nDamCalc = 1;`):
//      nMin = (25 * elemMin * calcA / calcB) >> 8
//      nMax = (25 * elemMax * calcA / calcB) >> 8
//  The 25 is DEFAULT_FRAMES_PER_SECOND (the per-frame to per-second factor) and the division is the signed idiv at 0x6FB0E964. The clamp closes
//  it, equality arm dead (lea esi, [edi+1] at 0x6FB0E988, cmp edi, esi at 0x6FB0E9A2). The original inlines DATATBLS_GetSkillsTxtRecordById
//  and SKILLDESC_FormatRangeOrValue here.
void __fastcall SKILLDESC_DamFunc8(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    D2C_Skills nSkillId;
    D2SkillsTxt *pSkillsTxtRec;
    int32_t wSkillDesc;
    D2SkillDescTxt *pSkillDescRec;
    D2C_Skills nSkillIdFromSkill;
    int32_t nDamCalcA;
    int32_t nDamCalcB;
    int32_t nElemMin;
    int32_t nElemMax;
    int32_t nMin;
    int32_t nMax;
    int32_t nMaxClamped;
    int32_t nWidth;
    int32_t nHeight;
    int32_t nTextY;
    int32_t nElemColor;
    Unicode wszText[32];

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    nSkillId = (D2C_Skills)pSkillsTxt->nSkillId;
    pSkillsTxtRec = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxtRec)
    {
        return;
    }
    wSkillDesc = (int16_t)pSkillsTxtRec->wSkillDesc;
    if (wSkillDesc < 0 || wSkillDesc >= sgptDataTables->nSkillDescTxtRecordCount)
    {
        return;
    }
    pSkillDescRec = &sgptDataTables->pSkillDescTxt[wSkillDesc];
    if (!pSkillDescRec)
    {
        return;
    }
    nElemMin = D2COMMON_SKILLS_GetMinElemDamage(pUnit, nSkillId, nSkillLevel, 1);
    nElemMax = D2COMMON_SKILLS_GetMaxElemDamage(pUnit, nSkillId, nSkillLevel, 1);
    switch (pSkillsTxt->nEType)
    {
    case 1:
        nElemColor = UI_COLOR_RED;
        break;
    case 2:
        nElemColor = UI_COLOR_YELLOW;
        break;
    case 3:
    case 4:
        nElemColor = UI_COLOR_BLUE;
        break;
    case 5:
        nElemColor = UI_COLOR_LIGHT_GREEN;
        break;
    default:
        nElemColor = UI_COLOR_WHITE;
        break;
    }
    nSkillIdFromSkill = (D2C_Skills)D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillDescCpp, 3114);
    nDamCalcA = D2COMMON_SKILLS_EvaluateSkillDescFormula(pUnit, pSkillDescRec->dwDamCalc[0], nSkillIdFromSkill, nSkillLevel);
    if (!nDamCalcA)
    {
        nDamCalcA = 1;
    }
    nSkillIdFromSkill = (D2C_Skills)D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillDescCpp, 3119);
    nDamCalcB = D2COMMON_SKILLS_EvaluateSkillDescFormula(pUnit, pSkillDescRec->dwDamCalc[1], nSkillIdFromSkill, nSkillLevel);
    if (!nDamCalcB)
    {
        nDamCalcB = 1;
    }
    nTextY = nY;
    nMin = (25 * nElemMin * nDamCalcA / nDamCalcB) >> 8;
    nMax = (25 * nElemMax * nDamCalcA / nDamCalcB) >> 8;
    nMaxClamped = nMax;
    if (nMin >= nMax)
    {
        nMaxClamped = nMin + 1;
    }
    SKILLDESC_FormatRangeOrValue(wszText, nMin, nMaxClamped, 0);
    nWidth = 0;
    nHeight = 0;
    D2WIN_10131_GetTextDimensions(wszText, &nWidth, &nHeight);
    nWidth = 11 * nWidth / 7;
    if (nWidth <= nRight - nLeft)
    {
        D2WIN_10127_SetFont(D2FONT_FONT16);
    }
    else
    {
        D2WIN_10127_SetFont(D2FONT_FONT6);
        nTextY = nY - 1;
    }
    CHAR_DrawTextCentered(gnBorderXDifference + nLeft, gnScreenHeight + gnBorderYDifference + nTextY - 480, gnBorderXDifference + nRight, wszText, nElemColor);
}


//1.10f: D2Client.0x6FB0EAB0
//  descdam handler 9: entry 9 of gpSkillDescDamFuncs. Not a kick line: stock data gives it to blaze, fire wall and firestorm, whose skills.txt
//  MinDam and MaxDam are empty and whose HitShift is 4, 4 and 2. It is SKILLDESC_DamFunc2's shift without the bias: dwMinDam << nToHitShift and
//  dwMaxDam << nToHitShift (shl edi, cl at 0x6FB0EAF0, shl ebx, cl at 0x6FB0EAF2, cl out of the byte at +0x1A4), where DamFunc2 shifts by
//  nToHitShift - 8.
//  DIFFERENCE: the shift count is masked with & 31 as the machine masks it, as in DamFunc2; C++ leaves a count of 32 or more undefined, which
//  the byte allows. The stock counts are all below 32 and the values shifted are zero.
//  Both shifted values are added to the skill's elemental pair first (GetMin/MaxElemDamage plus the shifted physical), and the sum is scaled by
//  75/256 (lea x75 at 0x6FB0EB57, sar 8), about 0.293. The rest is the family's: the colour chosen from nEType first (1 / 9 / 3 / 2 / 0), and
//  under srcdam the weapon's physical pair through AddWeaponPhysDamage (128 override) and AddItemElemDamage, both scaled by srcdam /128 and
//  added. The single-value arm is dead: clamped at 0x6FB0EC15, cmp edi, ebx at 0x6FB0EC2F always jumps. The original inlines
//  SKILLDESC_FormatRangeOrValue here.
void __fastcall SKILLDESC_DamFunc9(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    uint8_t nToHitShift;
    uint32_t nMinDamShifted;
    uint32_t nMaxDamShifted;
    int32_t nMinElemDamage;
    int32_t nMaxElemDamage;
    int32_t nSrcDam;
    int32_t nElemColor;
    int32_t nMin;
    int32_t nMax;
    int32_t nMaxClamped;
    int32_t nWeaponMin;
    int32_t nWeaponMax;
    int32_t nWidth;
    int32_t nHeight;
    int32_t nTextY;
    int32_t nDrawColor;
    Unicode wszText[32];

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    nToHitShift = pSkillsTxt->nToHitShift;
    nMinDamShifted = pSkillsTxt->dwMinDam << (nToHitShift & 31);
    nMaxDamShifted = pSkillsTxt->dwMaxDam << (nToHitShift & 31);
    nMinElemDamage = D2COMMON_SKILLS_GetMinElemDamage(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel, 1) + nMinDamShifted;
    nMaxElemDamage = D2COMMON_SKILLS_GetMaxElemDamage(pUnit, (D2C_Skills)pSkillsTxt->nSkillId, nSkillLevel, 1) + nMaxDamShifted;
    nSrcDam = pSkillsTxt->nSrcDam;
    switch (pSkillsTxt->nEType)
    {
    case 1:
        nElemColor = UI_COLOR_RED;
        break;
    case 2:
        nElemColor = UI_COLOR_YELLOW;
        break;
    case 3:
    case 4:
        nElemColor = UI_COLOR_BLUE;
        break;
    case 5:
        nElemColor = UI_COLOR_LIGHT_GREEN;
        break;
    default:
        nElemColor = UI_COLOR_WHITE;
        break;
    }
    nDrawColor = nElemColor;
    nMin = (75 * nMinElemDamage) >> 8;
    nMax = (75 * nMaxElemDamage) >> 8;
    if (nSrcDam)
    {
        nWeaponMin = 0;
        nWeaponMax = 0;
        SKILLDESC_AddWeaponPhysDamage(pUnit, &nWeaponMin, &nWeaponMax, &nDrawColor, 0, 0, pSkill, 128, NULL, 0);
        SKILLDESC_AddItemElemDamage(pUnit, &nWeaponMin, &nWeaponMax, &nDrawColor, 0, pSkill);
        nMin += nSrcDam * nWeaponMin / 128;
        nMax += nSrcDam * nWeaponMax / 128;
    }
    nTextY = nY;
    nMaxClamped = nMax;
    if (nMin >= nMax)
    {
        nMaxClamped = nMin + 1;
    }
    SKILLDESC_FormatRangeOrValue(wszText, nMin, nMaxClamped, 0);
    nWidth = 0;
    nHeight = 0;
    D2WIN_10131_GetTextDimensions(wszText, &nWidth, &nHeight);
    nWidth = 11 * nWidth / 7;
    if (nWidth <= nRight - nLeft)
    {
        D2WIN_10127_SetFont(D2FONT_FONT16);
    }
    else
    {
        D2WIN_10127_SetFont(D2FONT_FONT6);
        nTextY = nY - 1;
    }
    CHAR_DrawTextCentered(gnBorderXDifference + nLeft, gnScreenHeight + gnBorderYDifference + nTextY - 480, gnBorderXDifference + nRight, wszText, nDrawColor);
}


//1.10f: D2Client.0x6FB0ED40
//  descdam handler 10: entry 10 of gpSkillDescDamFuncs. The shield line. The guard includes INVENTORY_GetEquippedShield(pUnit->pInventory,
//  &pShield), and the damage starts from the shield's own items.txt row (nMinDam/nMaxDam at +0xFE/+0xFF, the byte pair), with a NULL shield
//  taking item id -1 (or eax, 0FFFFFFFFh at 0x6FB0ED78), which the table lookup is simply handed.
//  Holy Shield adds flat damage while it holds: SKILLS_GetSkillById for SKILL_HOLYSHIELD (mov edx, 75h at 0x6FB0ED9D) with owner -1, then, only
//  while STATES_CheckState(pUnit, STATE_HOLYSHIELD) (push 65h at 0x6FB0EDAF), the skill's own level with the bonus argument 1 and
//  GetMin/MaxPhysDamage >> 8 added to the shield's pair. The bonus percent comes from the skill's own dwParam[2] + (nSkillLevel - 1) *
//  dwParam[3] (+0x150 / +0x154) when the level is positive, strbonus * STRENGTH / 100 and dexbonus * DEXTERITY / 100 off the shield, and
//  STAT_DAMAGEPERCENT (clamped at >= -90 like the rest of the panel). The colour is the state masks': blue for DamBlue overriding to red for
//  DamRed (STATES_CheckStateMaskDam*OnUnit), left 0 otherwise, so a cursed shield's line is red. The clamp before the format is the family's,
//  its equality arm dead (lea esi, [ebx+1] at 0x6FB0EF65, cmp ebx, esi at 0x6FB0EF7F). The original inlines
//  DATATBLS_GetSkillsTxtRecordById and SKILLDESC_FormatRangeOrValue here.
void __fastcall SKILLDESC_DamFunc10(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    D2UnitStrc *pShield;
    int32_t nItemId;
    D2ItemsTxt *pItemsTxtRec;
    int32_t nMinDam;
    int32_t nMaxDam;
    D2SkillStrc *pHolyShieldSkill;
    int32_t nHolyShieldLevel;
    int32_t nDamageBonus;
    int16_t nStrengthBonus;
    int16_t nDexBonus;
    int32_t nMinPercent;
    int32_t nMaxPercent;
    int32_t nDamagePercent;
    int32_t nMin;
    int32_t nMax;
    int32_t nMaxClamped;
    int32_t nWidth;
    int32_t nHeight;
    int32_t nTextY;
    int32_t nDrawColor;
    D2C_Skills nSkillId;
    Unicode wszText[32];

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    if (!D2COMMON_INVENTORY_GetEquippedShield(pUnit->pInventory, &pShield))
    {
        return;
    }
    nItemId = pShield ? pShield->dwClassId : -1;
    pItemsTxtRec = D2COMMON_DATATBLS_GetItemsTxtRecord(nItemId);
    nMinDam = pItemsTxtRec->nMinDam;
    nMaxDam = pItemsTxtRec->nMaxDam;
    pHolyShieldSkill = D2COMMON_SKILLS_GetSkillById(pUnit, SKILL_HOLYSHIELD, -1);
    if (pHolyShieldSkill && D2COMMON_STATES_CheckState(pUnit, STATE_HOLYSHIELD))
    {
        nHolyShieldLevel = D2COMMON_SKILLS_GetSkillLevel(pUnit, pHolyShieldSkill, 1);
        nMinDam += D2COMMON_SKILLS_GetMinPhysDamage(pUnit, SKILL_HOLYSHIELD, nHolyShieldLevel, 1) >> 8;
        nMaxDam += D2COMMON_SKILLS_GetMaxPhysDamage(pUnit, SKILL_HOLYSHIELD, nHolyShieldLevel, 1) >> 8;
    }
    nDamageBonus = 0;
    nDrawColor = 0;
    nSkillId = (D2C_Skills)pSkillsTxt->nSkillId;
    D2SkillsTxt *pSkillsTxtRec = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (pSkillsTxtRec)
    {
        if (nSkillLevel > 0)
        {
            nDamageBonus = pSkillsTxtRec->dwParam[2] + (nSkillLevel - 1) * pSkillsTxtRec->dwParam[3];
        }
    }
    nStrengthBonus = D2COMMON_ITEMS_GetStrengthBonus(pShield);
    if (nStrengthBonus)
    {
        nDamageBonus += nStrengthBonus * D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_STRENGTH, 0) / 100;
    }
    nDexBonus = D2COMMON_ITEMS_GetDexBonus(pShield);
    if (nDexBonus)
    {
        nDamageBonus += nDexBonus * D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_DEXTERITY, 0) / 100;
    }
    nDamagePercent = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_DAMAGEPERCENT, 0) + nDamageBonus;
    nMinPercent = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_ITEM_MINDAMAGE_PERCENT, 0);
    nMaxPercent = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_ITEM_MAXDAMAGE_PERCENT, 0);
    if (D2COMMON_STATES_CheckStateMaskDamBlueOnUnit(pUnit))
    {
        nDrawColor = UI_COLOR_BLUE;
    }
    if (D2COMMON_STATES_CheckStateMaskDamRedOnUnit(pUnit))
    {
        nDrawColor = UI_COLOR_RED;
    }
    if (nDamagePercent <= -90)
    {
        nDamagePercent = -90;
    }
    //  BUG: the item damage percents are added to the product instead of joining nDamagePercent; the sibling handlers take
    //  nMinDam * (nDamagePercent + nMinPercent) / 100 (imul ecx, ebx at 0x6FB0EF1D, add ecx, edx at 0x6FB0EF23). REACHABLE: the two stats come from
    //  whatever a Paladin wears, and stock data puts them on a Smite shield itself, Alma Negra; a hundred per cent moves the line by one point.
    nMin = (nMinPercent + nMinDam * nDamagePercent) / 100 + nMinDam;
    nTextY = nY;
    nMax = nMaxDam + (nMaxPercent + nMaxDam * nDamagePercent) / 100;
    nMaxClamped = nMax;
    if (nMin >= nMax)
    {
        nMaxClamped = nMin + 1;
    }
    SKILLDESC_FormatRangeOrValue(wszText, nMin, nMaxClamped, 0);
    nWidth = 0;
    nHeight = 0;
    D2WIN_10131_GetTextDimensions(wszText, &nWidth, &nHeight);
    nWidth = 11 * nWidth / 7;
    if (nWidth <= nRight - nLeft)
    {
        D2WIN_10127_SetFont(D2FONT_FONT16);
    }
    else
    {
        D2WIN_10127_SetFont(D2FONT_FONT6);
        nTextY = nY - 1;
    }
    CHAR_DrawTextCentered(gnBorderXDifference + nLeft, gnScreenHeight + gnBorderYDifference + nTextY - 480, gnBorderXDifference + nRight, wszText, nDrawColor);
}


//1.10f: D2Client.0x6FB0F080
//  descdam handler 11: entry 11 of gpSkillDescDamFuncs, the largest of the one-line handlers. Three formulas, dwCalc[0], [1] and [2] (+0x138,
//  +0x13C, +0x140, each through SKILLS_EvaluateSkillFormula), each boosted by its own mastery stat and each then scaling the weapon's damage:
//      nFire  = calc[0];  if nFire and FIRE mastery:  nFire += DATATBLS_ApplyRatio(nFire, fireMastery, 100)
//      nCold  = calc[1];  ... COLD mastery ...
//      nLtng  = calc[2];  ... LTNG mastery ...
//      min = weaponMin + DATATBLS_ApplyRatio(minDam, nFire, 100) + DATATBLS_ApplyRatio(minDam, nCold, 100) + DATATBLS_ApplyRatio(minDam, nLtng, 100)
//      max = the same against maxDam and the scaled weapon max
//  The mastery is added, not substituted, and only when both the calc and the mastery stat are nonzero; the scalings themselves run
//  unconditionally, so a zero calc leaves the weapon's own damage showing. The mastery bonus is applied to the calc before the calc scales
//  anything. The original inlines DATATBLS_ApplyRatio in five of the nine multiplications (the fire and cold mastery boosts, both fire
//  scalings and the cold minimum); the other four are calls.
//  The base damage is the character's own: STAT_MINDAMAGE/STAT_MAXDAMAGE, or the SECONDARY pair when D2COMMON_10434(pUnit, 1) (an unresolved
//  ordinal) and the wield type is 2; clamped min <= 1 -> 1, max <= 2 -> 2, and max up to min. The weapon half with srcdam comes first, through
//  AddWeaponPhysDamage (128 override) and AddItemElemDamage, both scaled by srcdam /128. Drawn by SKILLDESC_DrawDamageValue, and in blue when
//  no element chose a colour: pnColor 0 is replaced by 3 before the call (mov eax, 3 at 0x6FB0F59B), the only handler that paints a
//  colourless line.
void __fastcall SKILLDESC_DamFunc11(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    int32_t nSrcDam;
    int32_t nElemColor;
    int32_t nWeaponMin;
    int32_t nWeaponMax;
    int32_t nMinFromWeapon;
    int32_t nMaxFromWeapon;
    int32_t nMinDamage;
    int32_t nMaxDamage;
    D2C_Skills nSkillId;
    int32_t nFireValue;
    int32_t nFireMastery;
    int32_t nColdValue;
    int32_t nColdMastery;
    int32_t nLtngValue;
    int32_t nLtngMastery;
    int32_t nMinTotal;
    int32_t nMaxTotal;
    int32_t nDrawColor;

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    nSrcDam = pSkillsTxt->nSrcDam;
    nMinFromWeapon = 0;
    nMaxFromWeapon = 0;
    switch (pSkillsTxt->nEType - 1)
    {
    case 0:
        nElemColor = UI_COLOR_RED;
        break;
    case 1:
        nElemColor = UI_COLOR_YELLOW;
        break;
    case 2:
    case 3:
        nElemColor = UI_COLOR_BLUE;
        break;
    case 4:
        nElemColor = UI_COLOR_LIGHT_GREEN;
        break;
    default:
        nElemColor = UI_COLOR_WHITE;
        break;
    }
    nDrawColor = nElemColor;
    if (nSrcDam)
    {
        nWeaponMin = 0;
        nWeaponMax = 0;
        SKILLDESC_AddWeaponPhysDamage(pUnit, &nWeaponMin, &nWeaponMax, &nDrawColor, 0, 0, pSkill, 128, NULL, 0);
        SKILLDESC_AddItemElemDamage(pUnit, &nWeaponMin, &nWeaponMax, &nDrawColor, 0, pSkill);
        nMinFromWeapon = nSrcDam * nWeaponMin / 128;
        nMaxFromWeapon = nSrcDam * nWeaponMax / 128;
    }
    if (D2COMMON_10434(pUnit, 1) && D2COMMON_INVENTORY_GetWieldType(pUnit, pUnit->pInventory) == 2)
    {
        nMinDamage = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_SECONDARY_MINDAMAGE, 0);
        nMaxDamage = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_SECONDARY_MAXDAMAGE, 0);
    }
    else
    {
        nMinDamage = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_MINDAMAGE, 0);
        nMaxDamage = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_MAXDAMAGE, 0);
    }
    if (nMinDamage <= 1)
    {
        nMinDamage = 1;
    }
    if (nMaxDamage <= 2)
    {
        nMaxDamage = 2;
    }
    if (nMaxDamage < nMinDamage)
    {
        nMaxDamage = nMinDamage;
    }
    nSkillId = (D2C_Skills)pSkillsTxt->nSkillId;
    nFireValue = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], nSkillId, nSkillLevel);
    if (nFireValue)
    {
        nFireMastery = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_PASSIVE_FIRE_MASTERY, 0);
        if (nFireMastery)
        {
            nFireValue += DATATBLS_ApplyRatio(nFireValue, nFireMastery, 100);
        }
    }
    nMinTotal = DATATBLS_ApplyRatio(nMinDamage, nFireValue, 100) + nMinFromWeapon;
    nMaxTotal = DATATBLS_ApplyRatio(nMaxDamage, nFireValue, 100) + nMaxFromWeapon;
    nColdValue = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[1], nSkillId, nSkillLevel);
    if (nColdValue)
    {
        nColdMastery = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_PASSIVE_COLD_MASTERY, 0);
        if (nColdMastery)
        {
            nColdValue += DATATBLS_ApplyRatio(nColdValue, nColdMastery, 100);
        }
    }
    nMinTotal += DATATBLS_ApplyRatio(nMinDamage, nColdValue, 100);
    nMaxTotal += DATATBLS_ApplyRatio(nMaxDamage, nColdValue, 100);
    nLtngValue = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[2], nSkillId, nSkillLevel);
    if (nLtngValue)
    {
        nLtngMastery = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_PASSIVE_LTNG_MASTERY, 0);
        if (nLtngMastery)
        {
            nLtngValue += DATATBLS_ApplyRatio(nLtngValue, nLtngMastery, 100);
        }
    }
    nMinTotal += DATATBLS_ApplyRatio(nMinDamage, nLtngValue, 100);
    nMaxTotal += DATATBLS_ApplyRatio(nMaxDamage, nLtngValue, 100);
    if (!nDrawColor)
    {
        nDrawColor = UI_COLOR_BLUE;
    }
    SKILLDESC_DrawDamageValue(pUnit, nMinTotal, nMaxTotal, nDrawColor, nLeft, nY, nRight);
}


//1.10f: D2Client.0x6FB0F5E0
//  Formats one damage value and draws it centred: D2WIN_10127_SetFont(1) when the text fits in nRight - nLeft after the family's
//  11 * width / 7 scaling, font 6 and a one-pixel y nudge when it does not. The third argument is the maximum and the fourth the colour; the
//  fourth goes straight into CHAR_DrawTextCentered's colour slot. The width comes back in the nMax parameter, which is why the function keeps
//  nMax1 and nLeftSaved copies of two others: `nMax = 0; D2WIN_10131_GetTextDimensions(wszOut, &nMax, &nTextHeight);` then
//  `nMax = 11 * nMax / 7;` and the fit test. nTextHeight is written and never read. The equality arm is dead here too: `if (nMin >= nMax)
//  nMaxClamped = nMin + 1;` runs before the format test (lea ebp, [edi+1] at 0x6FB0F5F0, cmp edi, ebp at 0x6FB0F60D), so a single value
//  prints as the range n-(n+1). The original inlines SKILLDESC_FormatRangeOrValue here.
void __fastcall SKILLDESC_DrawDamageValue(D2UnitStrc *pUnused, int32_t nMin, int32_t nMax, int32_t nColor, int32_t nLeft, int32_t nY, int32_t nRight)
{
    int32_t nMaxClamped;
    int32_t nTextHeight;
    int32_t nMax1;
    int32_t nLeftSaved;
    Unicode wszOut[32];

    nMaxClamped = nMax;
    if (nMin >= nMax)
    {
        nMaxClamped = nMin + 1;
    }
    SKILLDESC_FormatRangeOrValue(wszOut, nMin, nMaxClamped, 0);
    nMax = 0;
    nTextHeight = 0;
    D2WIN_10131_GetTextDimensions(wszOut, &nMax, &nTextHeight);
    nMax1 = nRight;
    nLeftSaved = nLeft;
    nMax = 11 * nMax / 7;
    if (nMax <= nRight - nLeft)
    {
        D2WIN_10127_SetFont(D2FONT_FONT16);
    }
    else
    {
        D2WIN_10127_SetFont(D2FONT_FONT6);
        --nY;
    }
    CHAR_DrawTextCentered(gnBorderXDifference + nLeftSaved, gnScreenHeight + gnBorderYDifference + nY - 480, gnBorderXDifference + nMax1, wszOut, nColor);
}


//1.10f: D2Client.0x6FB0F700
//  descdam handler 12: entry 12 of gpSkillDescDamFuncs. The elemental pair under a percentage, and the percentage is the only thing the two
//  arms differ in: an expansion game takes SKILLS_GetConcentrationDamageBonus for the skill itself, otherwise STAT_DAMAGEPERCENT clamped at
//  >= -90 (cmp ecx, 0FFFFFFA6h at 0x6FB0F8D4). Both scalings are the plain `value + value * pct / 100` (one imul against the /100 constant,
//  no arm tests), not the four-armed DATATBLS_ApplyRatio. The arms also swap the damage call's 5th and 6th out-slots, and each writes and reads
//  its own, so the two collapse into one body here. The tail is the family's fit-or-shrink with the clamp before the format, so the equality
//  arm is dead: a single value prints as n-(n+1). The original inlines SKILLDESC_FormatRangeOrValue here.
void __fastcall SKILLDESC_DamFunc12(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    D2SkillsTxt *pSkillsTxtRec;
    int32_t nElemMin;
    int32_t nElemMax;
    int32_t nElemColor;
    int32_t nPercent;
    int32_t nTextY;
    int32_t nMaxClamped;
    int32_t nRightCopy;
    int32_t nLeftCopy;
    int32_t nWidth;
    int32_t nHeight;
    Unicode wszOut[32];

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    pSkillsTxtRec = pSkillsTxt;
    nElemMin = 0;
    nElemMax = 0;
    nElemColor = 0;
    if (GAME_IsExpansionGame())
    {
        SKILLDESC_AddElemDamageAndColor(pUnit, pSkillsTxtRec, nSkillLevel, &nElemMin, &nElemMax, &nElemColor);
        nPercent = D2COMMON_SKILLS_GetConcentrationDamageBonus(pUnit, (D2C_Skills)pSkillsTxtRec->nSkillId);
    }
    else
    {
        SKILLDESC_AddElemDamageAndColor(pUnit, pSkillsTxtRec, nSkillLevel, &nElemMin, &nElemMax, &nElemColor);
        nPercent = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_DAMAGEPERCENT, 0);
        if (nPercent <= -90)
        {
            nPercent = -90;
        }
    }
    nElemMin += nElemMin * nPercent / 100;
    nElemMax += nElemMax * nPercent / 100;
    nTextY = nY;
    nMaxClamped = nElemMax;
    if (nElemMin >= nElemMax)
    {
        nMaxClamped = nElemMin + 1;
    }
    SKILLDESC_FormatRangeOrValue(wszOut, nElemMin, nMaxClamped, 0);
    nWidth = 0;
    nHeight = 0;
    D2WIN_10131_GetTextDimensions(wszOut, &nWidth, &nHeight);
    nRightCopy = nRight;
    nLeftCopy = nLeft;
    nWidth = 11 * nWidth / 7;
    if (nWidth <= nRight - nLeft)
    {
        D2WIN_10127_SetFont(D2FONT_FONT16);
    }
    else
    {
        D2WIN_10127_SetFont(D2FONT_FONT6);
        nTextY = nY - 1;
    }
    CHAR_DrawTextCentered(gnBorderXDifference + nLeftCopy, gnScreenHeight + gnBorderYDifference + nTextY - 480, gnBorderXDifference + nRightCopy, wszOut, nElemColor);
}


//1.10f: D2Client.0x6FB0FA50
//  descdam handler 14: entry 14 of gpSkillDescDamFuncs. The weapon's pair through SKILLDESC_AddWeaponPhysDamage with no extra percent, no flat
//  bonus, no weapon and nSrcDamOverride 0, which is not "off": 0 makes the helper take the skill's own srcdam, and 128 leaves the pair alone.
//  Stock data gives descdam 14 to hunger alone, whose SrcDam is 128, so nothing is scaled there. Then both ends are scaled by dwParam[4]
//  (+0x158, mov ecx, [eax+158h] at 0x6FB0FAD5; hunger's Param5 is -75) of the record the function looks up for itself (the skill id from
//  SKILLS_GetSkillIdFromSkill, the bounds check, the row), with zero when the lookup fails. The scalings are the plain
//  `value + value * pct / 100`, not DATATBLS_ApplyRatio. Then SKILLDESC_AddItemElemDamage over the same pair and colour, the family's clamp,
//  fit-or-shrink and draw, the equality arm dead behind the clamp as in DamFunc12. The original inlines DATATBLS_GetSkillsTxtRecordById and
//  SKILLDESC_FormatRangeOrValue here.
void __fastcall SKILLDESC_DamFunc14(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    D2SkillsTxt *pSkillsTxtRec;
    D2C_Skills nSkillIdFromSkill;
    int32_t nMin;
    int32_t nMax;
    int32_t nBonusPercent;
    int32_t nColor;
    int32_t nTextY;
    int32_t nMaxClamped;
    int32_t nWidth;
    int32_t nHeight;
    Unicode wszOut[32];

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    nMin = 0;
    nMax = 0;
    nColor = 0;
    SKILLDESC_AddWeaponPhysDamage(pUnit, &nMin, &nMax, &nColor, 0, 0, pSkill, 0, NULL, 0);
    nSkillIdFromSkill = (D2C_Skills)D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillDescCpp, 3392);
    nBonusPercent = 0;
    pSkillsTxtRec = DATATBLS_GetSkillsTxtRecordById(nSkillIdFromSkill);
    if (pSkillsTxtRec)
    {
        nBonusPercent = pSkillsTxtRec->dwParam[4];
    }
    nMin += nMin * nBonusPercent / 100;
    nMax += nMax * nBonusPercent / 100;
    SKILLDESC_AddItemElemDamage(pUnit, &nMin, &nMax, &nColor, 0, pSkill);
    nTextY = nY;
    nMaxClamped = nMax;
    if (nMin >= nMax)
    {
        nMaxClamped = nMin + 1;
    }
    SKILLDESC_FormatRangeOrValue(wszOut, nMin, nMaxClamped, 0);
    nWidth = 0;
    nHeight = 0;
    D2WIN_10131_GetTextDimensions(wszOut, &nWidth, &nHeight);
    nWidth = 11 * nWidth / 7;
    if (nWidth <= nRight - nLeft)
    {
        D2WIN_10127_SetFont(D2FONT_FONT16);
    }
    else
    {
        D2WIN_10127_SetFont(D2FONT_FONT6);
        nTextY = nY - 1;
    }
    CHAR_DrawTextCentered(gnBorderXDifference + nLeft, gnScreenHeight + gnBorderYDifference + nTextY - 480, gnBorderXDifference + nRight, wszOut, nColor);
}


//1.10f: D2Client.0x6FB0FC60
//  descdam handler 15: entry 15 of gpSkillDescDamFuncs. The kick line: SKILLS_GetMin/MaxPhysDamage with the bonus argument (>> 8, a4 = 1), then
//  SKILLS_CalculateKickDamage, and all four ends scaled by dwParam[0] + (level - 1) * dwParam[1] (the record's first two columns, [ecx+148h] /
//  [ecx+14Ch] at 0x6FB0FCEB..0x6FB0FCF5) plus the charge-up percent; each scaling is DATATBLS_ApplyRatio(value, pct, 100), and the colour goes
//  blue whenever the total percent is nonzero. The kick's percent is the by-ref slot the kick call may rewrite: the two kick scalings read it
//  back after CalculateKickDamage (reloaded at 0x6FB0FE62), while the physical pair used the original value. The totals are kick + physical,
//  the charge-up pair on top through SKILLDESC_AddChargeUpElemDamage, all drawn by SKILLDESC_DrawDamageValue, which owns the clamp and the
//  format. The original inlines SKILLDESC_GetLinearParamByLevel and DATATBLS_ApplyRatio here.
void __fastcall SKILLDESC_DamFunc15(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    int32_t nSkillId;
    int32_t nChargeUpPercent;
    int32_t nBasePercent;
    int32_t nTotalPercent;
    int32_t nPhysMin;
    int32_t nPhysMax;
    int32_t nKickMin;
    int32_t nKickMax;
    int32_t nKickPercent;
    int32_t nElemMin;
    int32_t nElemMax;
    int32_t nColor;
    int32_t nMinTotal;
    int32_t nMaxTotal;

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    nSkillId = pSkillsTxt->nSkillId;
    nColor = 0;
    nPhysMin = D2COMMON_SKILLS_GetMinPhysDamage(pUnit, (D2C_Skills)nSkillId, nSkillLevel, 1) >> 8;
    nPhysMax = D2COMMON_SKILLS_GetMaxPhysDamage(pUnit, (D2C_Skills)nSkillId, nSkillLevel, 1) >> 8;
    nChargeUpPercent = SKILLDESC_GetChargeUpDamagePercent();
    nBasePercent = SKILLDESC_GetLinearParamByLevel(nSkillId, nSkillLevel);
    nTotalPercent = nBasePercent + nChargeUpPercent;
    if (nTotalPercent)
    {
        nColor = UI_COLOR_BLUE;
    }
    nPhysMin += DATATBLS_ApplyRatio(nPhysMin, nTotalPercent, 100);
    nPhysMax += DATATBLS_ApplyRatio(nPhysMax, nTotalPercent, 100);
    nKickMin = 0;
    nKickMax = 0;
    nKickPercent = nTotalPercent;
    D2COMMON_SKILLS_CalculateKickDamage(pUnit, &nKickMin, &nKickMax, &nKickPercent);
    nKickMin += DATATBLS_ApplyRatio(nKickMin, nKickPercent, 100);
    nKickMax += DATATBLS_ApplyRatio(nKickMax, nKickPercent, 100);
    nMinTotal = nKickMin + nPhysMin;
    nMaxTotal = nKickMax + nPhysMax;
    nElemMin = 0;
    nElemMax = 0;
    SKILLDESC_AddChargeUpElemDamage(pUnit, &nElemMin, &nElemMax, &nColor);
    nMinTotal += nElemMin;
    nMaxTotal += nElemMax;
    SKILLDESC_DrawDamageValue(pUnit, nMinTotal, nMaxTotal, nColor, nLeft, nY, nRight);
}


//1.10f: D2Client.0x6FB10010
//  descdam handler 16: entry 16 of gpSkillDescDamFuncs. The colour is chosen from nEType before any damage, the same five-way map DamFunc5 and
//  SKILLDESC_AddElemDamageAndColor carry (1 red, 2 yellow, 3 and 4 blue, 5 light green, anything else white). The physical pair comes from
//  SKILLS_GetMin/MaxPhysDamage with a4 = 0 (DamFunc15's calls pass 1), then DATATBLS_ApplyRatio(value, pct, 100) four times (the physical and
//  the kick pairs against the charge-up percent, the kick percent read back after SKILLS_CalculateKickDamage as in DamFunc15), then two calls to
//  the same helper (0x6FB10358 / 0x6FB1036F) whose percentage is dwCalc[0] through SKILLS_EvaluateSkillFormula and whose value is the running
//  total. With an inventory, the off-hand block: the left-hand weapon and the item in the other hand ((nBodyLocation == BODYLOC_RARM) +
//  BODYLOC_RARM, the same computed slot the two weapon preparations use), each merged with STATLIST_MergeStatLists(..., 0) while it is an
//  equippable weapon with an owner, the weapon merged back with mode 1 after SKILLDESC_AddItemElemDamage; that call sits inside the inventory
//  guard, so a unit with no inventory gets no elemental item damage either. Drawn as one line by CHAR_DrawTextCentered with the family's
//  fit-or-shrink; the clamp before the SKILLDESC_FormatRangeOrValue call keeps an n-(n+1) range. The original inlines DATATBLS_ApplyRatio
//  in the four scalings.
void __fastcall SKILLDESC_DamFunc16(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    D2InventoryStrc *pInventory;
    D2UnitStrc *pWeapon;
    D2UnitStrc *pOtherWeapon;
    uint8_t nBodyLocation;
    int32_t nEType;
    int32_t nSkillId;
    int32_t nColor;
    int32_t nPhysMin;
    int32_t nPhysMax;
    int32_t nChargeUpPercent;
    int32_t nKickMin;
    int32_t nKickMax;
    int32_t nKickPercent;
    int32_t nMinTotal;
    int32_t nMaxTotal;
    int32_t nFormula;
    int32_t nMaxClamped;
    int32_t nTextY;
    int32_t nWidth;
    int32_t nHeight;
    Unicode wszOut[32];

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    nEType = pSkillsTxt->nEType;
    nSkillId = pSkillsTxt->nSkillId;
    switch (nEType)
    {
    case 1:                             // Fire
        nColor = UI_COLOR_RED;
        break;
    case 2:                             // Lightning
        nColor = UI_COLOR_YELLOW;
        break;
    case 3:                             // Magic
    case 4:                             // Cold
        nColor = UI_COLOR_BLUE;
        break;
    case 5:                             // Poison
        nColor = UI_COLOR_LIGHT_GREEN;
        break;
    default:
        nColor = UI_COLOR_WHITE;
        break;
    }
    nPhysMin = D2COMMON_SKILLS_GetMinPhysDamage(pUnit, (D2C_Skills)nSkillId, nSkillLevel, 0) >> 8;
    nPhysMax = D2COMMON_SKILLS_GetMaxPhysDamage(pUnit, (D2C_Skills)nSkillId, nSkillLevel, 0) >> 8;
    nChargeUpPercent = SKILLDESC_GetChargeUpDamagePercent();
    nPhysMin += DATATBLS_ApplyRatio(nPhysMin, nChargeUpPercent, 100);
    nPhysMax += DATATBLS_ApplyRatio(nPhysMax, nChargeUpPercent, 100);
    nKickMin = 0;
    nKickMax = 0;
    nKickPercent = nChargeUpPercent;
    D2COMMON_SKILLS_CalculateKickDamage(pUnit, &nKickMin, &nKickMax, &nKickPercent);
    nKickMin += DATATBLS_ApplyRatio(nKickMin, nKickPercent, 100);
    nKickMax += DATATBLS_ApplyRatio(nKickMax, nKickPercent, 100);
    nMinTotal = nKickMin + nPhysMin;
    nMaxTotal = nKickMax + nPhysMax;
    nFormula = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, pSkillsTxt->dwCalc[0], (D2C_Skills)nSkillId, nSkillLevel);
    nMinTotal += DATATBLS_ApplyRatio(nMinTotal, nFormula, 100);
    nMaxTotal += DATATBLS_ApplyRatio(nMaxTotal, nFormula, 100);
    pInventory = pUnit->pInventory;
    if (pInventory)
    {
        pWeapon = D2COMMON_INVENTORY_GetLeftHandWeapon(pInventory);
        pOtherWeapon = NULL;
        if (pWeapon)
        {
            nBodyLocation = D2COMMON_ITEMS_GetBodyLocation(pWeapon);
            pOtherWeapon = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, (D2C_PlayerBodyLocs)((nBodyLocation == BODYLOC_RARM) + BODYLOC_RARM));
        }
        if (D2COMMON_ITEMS_CheckItemTypeId(pWeapon, ITEMTYPE_WEAPON) && D2COMMON_ITEMS_CanBeEquipped(pWeapon) && D2COMMON_STATLIST_GetOwner(pWeapon, NULL))
        {
            D2COMMON_STATLIST_MergeStatLists(pUnit, pWeapon, 0);
        }
        else
        {
            pWeapon = NULL;
        }
        if (pOtherWeapon && pOtherWeapon != pWeapon && D2COMMON_ITEMS_CheckItemTypeId(pOtherWeapon, ITEMTYPE_WEAPON) && D2COMMON_ITEMS_CanBeEquipped(pOtherWeapon) && D2COMMON_STATLIST_GetOwner(pOtherWeapon, NULL))
        {
            D2COMMON_STATLIST_MergeStatLists(pUnit, pOtherWeapon, 0);
        }
        SKILLDESC_AddItemElemDamage(pUnit, &nMinTotal, &nMaxTotal, &nColor, 0, pSkill);
        if (pWeapon)
        {
            D2COMMON_STATLIST_MergeStatLists(pUnit, pWeapon, 1);
        }
    }
    nTextY = nY;
    nMaxClamped = nMaxTotal;
    if (nMinTotal >= nMaxTotal)
    {
        nMaxClamped = nMinTotal + 1;
    }
    SKILLDESC_FormatRangeOrValue(wszOut, nMinTotal, nMaxClamped, 0);
    nWidth = 0;
    nHeight = 0;
    D2WIN_10131_GetTextDimensions(wszOut, &nWidth, &nHeight);
    nWidth = 11 * nWidth / 7;
    if (nWidth <= nRight - nLeft)
    {
        D2WIN_10127_SetFont(D2FONT_FONT16);
    }
    else
    {
        D2WIN_10127_SetFont(D2FONT_FONT6);
        nTextY = nY - 1;
    }
    CHAR_DrawTextCentered(gnBorderXDifference + nLeft, gnScreenHeight + gnBorderYDifference + nTextY - 480, gnBorderXDifference + nRight, wszOut, nColor);
}


//1.10f: D2Client.0x6FB10540
//  descdam handler 17: entry 17 of gpSkillDescDamFuncs. Its own skills.txt -> skilldesc.txt walk (wSkillDesc at +0x194, the row's dwDamCalc[0]
//  and [1] at +0x18/+0x1C through SKILLS_EvaluateSkillDescFormula), then the whole physical pipeline at nSrcDamOverride 128 (the value
//  SKILLDESC_AddWeaponPhysDamage reads as "leave it", so the weapon's pair is not scaled by srcdam) with dwDamCalc[0] as the percentage and
//  dwDamCalc[1] as the flat bonus, and the elemental pair after it. Then the family's three-way draw:
//      physical pair empty   the elemental pair alone, clamped
//      elemental pair empty  the physical pair alone, clamped
//      both pairs present    two lines through DrawTwoLines, each formatted unclamped
//  The two single-line arms format through SKILLDESC_FormatRangeOrValue behind their clamps, so their equality arms are dead; the two-line arm
//  tests nMin == nMax itself and its single-value arms are live, the same split as DamFunc22. The elemental colour is the one
//  SKILLDESC_AddElemDamageAndColor leaves in its colour slot. The original inlines DATATBLS_GetSkillsTxtRecordById and
//  SKILLDESC_FormatRangeOrValue here.
void __fastcall SKILLDESC_DamFunc17(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    D2SkillsTxt *pSkillsTxtRec;
    D2SkillDescTxt *pSkillDescRec;
    int32_t nSkillId;
    int32_t nSkillDesc;
    int32_t nDamCalcA;
    int32_t nDamCalcB;
    int32_t nMin;
    int32_t nMax;
    int32_t nPhysColor;
    int32_t nElemMin;
    int32_t nElemMax;
    int32_t nElemColor;
    int32_t nTextY;
    int32_t nMaxClamped;
    int32_t nWidth;
    int32_t nHeight;
    Unicode wszText[32];
    Unicode wszOut[32];

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    nSkillId = pSkillsTxt->nSkillId;
    if ((pSkillsTxtRec = DATATBLS_GetSkillsTxtRecordById(nSkillId)) != NULL && (nSkillDesc = (int16_t)pSkillsTxtRec->wSkillDesc) >= 0 && nSkillDesc < sgptDataTables->nSkillDescTxtRecordCount
        && (pSkillDescRec = &sgptDataTables->pSkillDescTxt[nSkillDesc]) != NULL)
    {
        nMin = 0;
        nMax = 0;
        nPhysColor = 0;
        nDamCalcA = D2COMMON_SKILLS_EvaluateSkillDescFormula(pUnit, pSkillDescRec->dwDamCalc[0], (D2C_Skills)nSkillId, nSkillLevel);
        nDamCalcB = D2COMMON_SKILLS_EvaluateSkillDescFormula(pUnit, pSkillDescRec->dwDamCalc[1], (D2C_Skills)nSkillId, nSkillLevel);
        SKILLDESC_AddWeaponPhysDamage(pUnit, &nMin, &nMax, &nPhysColor, nDamCalcA, nDamCalcB, pSkill, 128, NULL, 0);
        SKILLDESC_AddItemElemDamage(pUnit, &nMin, &nMax, &nPhysColor, 0, pSkill);
        nElemMin = 0;
        nElemMax = 0;
        nElemColor = 0;
        SKILLDESC_AddElemDamageAndColor(pUnit, pSkillsTxt, nSkillLevel, &nElemMin, &nElemMax, &nElemColor);
        if (!nMin && !nMax)
        {
            //  the physical pair is empty: the elemental pair alone, in the elemental colour
            nTextY = nY;
            nMaxClamped = nElemMax;
            if (nElemMin >= nElemMax)
            {
                nMaxClamped = nElemMin + 1;
            }
            SKILLDESC_FormatRangeOrValue(wszText, nElemMin, nMaxClamped, 0);
            nWidth = 0;
            nHeight = 0;
            D2WIN_10131_GetTextDimensions(wszText, &nWidth, &nHeight);
            nWidth = 11 * nWidth / 7;
            if (nWidth <= nRight - nLeft)
            {
                D2WIN_10127_SetFont(D2FONT_FONT16);
            }
            else
            {
                D2WIN_10127_SetFont(D2FONT_FONT6);
                nTextY = nY - 1;
            }
            CHAR_DrawTextCentered(gnBorderXDifference + nLeft, gnScreenHeight + gnBorderYDifference + nTextY - 480, gnBorderXDifference + nRight, wszText, nElemColor);
            return;
        }
        if (!nElemMin && !nElemMax)
        {
            //  the elemental pair is empty: the physical pair alone, in the pair's own colour
            nTextY = nY;
            nMaxClamped = nMax;
            if (nMin >= nMax)
            {
                nMaxClamped = nMin + 1;
            }
            SKILLDESC_FormatRangeOrValue(wszText, nMin, nMaxClamped, 0);
            nWidth = 0;
            nHeight = 0;
            D2WIN_10131_GetTextDimensions(wszText, &nWidth, &nHeight);
            nWidth = 11 * nWidth / 7;
            if (nWidth <= nRight - nLeft)
            {
                D2WIN_10127_SetFont(D2FONT_FONT16);
            }
            else
            {
                D2WIN_10127_SetFont(D2FONT_FONT6);
                nTextY = nY - 1;
            }
            CHAR_DrawTextCentered(gnBorderXDifference + nLeft, gnScreenHeight + gnBorderYDifference + nTextY - 480, gnBorderXDifference + nRight, wszText, nPhysColor);
            return;
        }
        //  both pairs: the physical line over the elemental
        SKILLDESC_FormatRangeOrValue(wszOut, nMin, nMax, 0);
        SKILLDESC_FormatRangeOrValue(wszText, nElemMin, nElemMax, 0);
        SKILLDESC_DrawTwoLines(wszOut, wszText, nLeft, nY, nRight, nPhysColor, nElemColor);
    }
}


//1.10f: D2Client.0x6FB108F0
//  descdam handler 18: entry 18 of gpSkillDescDamFuncs. DamFunc17's guard without any of its work (the skill's record, its wSkillDesc, the
//  skilldesc row, each checked and then discarded) and SKILLDESC_DamFunc7 called with the original seven arguments. So this entry is DamFunc7
//  for skills that have a skilldesc row, and a no-op for the rest. The original inlines DATATBLS_GetSkillsTxtRecordById here.
void __fastcall SKILLDESC_DamFunc18(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight)
{
    D2SkillsTxt *pSkillsTxtRec;
    int32_t nSkillId;
    int32_t nSkillDesc;

    if (!pSkill || !pSkillsTxt)
    {
        return;
    }
    nSkillId = pSkillsTxt->nSkillId;
    if ((pSkillsTxtRec = DATATBLS_GetSkillsTxtRecordById(nSkillId)) != NULL && (nSkillDesc = (int16_t)pSkillsTxtRec->wSkillDesc) >= 0 && nSkillDesc < sgptDataTables->nSkillDescTxtRecordCount && &sgptDataTables->pSkillDescTxt[nSkillDesc] != NULL)
    {
        SKILLDESC_DamFunc7(pUnit, pSkill, pSkillsTxt, nSkillLevel, nLeft, nY, nRight);
    }
}


//1.10f: D2Client.0x6FB10970
//  The skilldesc.txt descline dispatcher: appends one line of a skill's description to pwszText and returns whether it recognised the line
//  type. A 75-case switch on the line's descline value (1..0x4B; everything else returns 0), not a function-pointer table like
//  gpSkillDescDamFuncs / gpSkillDescAttFuncs. The arguments come from the D2SkillDescTxt record at
//  &sgptDataTables->pSkillDescTxt[pSkillsTxt->wSkillDesc]:
//      nDescLine[nLine]   +0x42   the switch value (the desclineN column)
//      wDescTextA[nLine]  +0x54   desctextaN, the line's label/pre-text
//      wDescTextB[nLine]  +0x76   desctextbN, the line's suffix/post-text
//      dwDescCalcA[nLine] +0x98   desccalcaN, through SKILLS_EvaluateSkillDescFormula
//      dwDescCalcB[nLine] +0xDC   desccalcbN, likewise
//  nLine indexes all seventeen line slots as one array: 0..5 are descline1..6, 6..9 are dsc2line1..4, 10..16 are dsc3line1..7, exactly the
//  ranges the callers walk. 5382 ("dummy", STR_IDX_5382_dummy) is the no-string sentinel: almost every case tests a text index against it
//  before appending, so a cell left as dummy prints nothing.
//  The return value says "recognised", not "printed": most "nothing to show" arms return 1 (RETURN_ONE is the shared tail; descline 1 with
//  zero mana still returns 1). Only the default case, the seven tests at the top (0x6FB10984..0x6FB109F5) and a handful of explicit arms
//  return 0. The callers use the result only to decide whether to insert a blank separator line, so an unknown descline value is what
//  suppresses the separator. nCharges goes straight to SKILLDESC_FormatChargesText on descline 1: an item-granted skill shows "(Charges: N)"
//  instead of its mana cost. nColor is a colour code fed to UI_AddColorCodeToWstring on desclines 0x12, 0x19, 0x28 and 0x2E (the level builder
//  passes 0, the requirement builder 1, the finishing-move builder its own nTextColor). Descline 1 is the mana formula:
//      nMana = max(wMinMana << 8, (wMana + wLevelMana * (nSkillLevel-1)) << nManaShift)
//  Element ids (pSkillsTxt->nEType): 1 Fire, 2 Lightning, 3 Magic, 4 Cold, 5 Poison, 6 Life Drain; descline 0x0B prints a length only for 4
//  and 5. Descline 0x1D's range is (wRange + (nSkillLevel - 1) * wLevRange) * 2 / 3, MSVC's signed division by 3 (magic 0x55555556 after
//  add ecx, ecx, at 0x6FB11B26..0x6FB11B53). The shared tails are labels, exactly as the compiler merged them.
//  The three buffers are the original's three stack slots, declared once: wszScratch is the 32-Unicode formatting scratch (the slot the summon
//  cases use as a D2MonStatsInitStrc: two locals over one address), wszBuild the 256-Unicode line builder and wszTemp the 100-Unicode sprintf
//  target that descline 0x35 also draws from. The original reuses two more stack slots for non-overlapping roles (the pSkillDesc slot also
//  holds the max elemental damage on 0x16, the percent on 0x22/0x27 and the colour out-parameter on 0x30; the B text index slot holds the
//  running minimum on 0x22); the C declares the roles apart.
//  DIFFERENCE: the original constructs each Unicode buffer inside the case that uses it; ours construct at function entry, which is what the
//  declaration does and writes the same zeros into the same dead stack before any read.
//  DIFFERENCE: the compiler emitted the "append newline and return 1" tail twice (0x6FB12D4A for the suffix paths, 0x6FB12DED after
//  APPEND_TAIL_TEXT); ours shares one copy.
//  The original inlines DATATBLS_GetSkillsTxtRecordById, DATATBLS_GetMonStatsTxtRecord, SKILLDESC_DrawQuantityHint and the missiles.txt
//  record lookup in the cases below.
int32_t __fastcall SKILLDESC_BuildDescLine(D2UnitStrc *pPlayer, struct Unicode *pwszText, D2SkillsTxt *pSkillsTxt, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nCharges, int32_t nLine, int32_t nColor)
{
    Unicode wszScratch[32];
    Unicode wszBuild[256];
    Unicode wszTemp[100];
    D2MonStatsInitStrc tSummon;

    D2SkillsTxt *pSkillsTxtById;
    D2SkillDescTxt *pSkillDescTxt;
    D2SkillDescTxt *pSkillDescByIdx;
    D2MonStatsTxt *pMonStatsTxtById;
    D2MonStatsTxt *pSummonMonStats;
    D2MissilesTxt *pMissileRec;
    D2DifficultyLevelsTxt *pDifficultyLevels;

    const Unicode *pTailText;
    int32_t nSuffixIdx;
    int32_t nStrSkill36Idx;

    int32_t wSkillDesc;
    int32_t nCalcA;
    int32_t nCalcB;
    D2C_StringIndices wDescTextA;
    D2C_StringIndices nDescTextB;

    D2C_StringIndices nStrIdx;
    int32_t nManaShift;
    int32_t wLevelMana;
    int32_t nManaCost;
    int32_t nSkillDescIdx;
    int32_t nToHitFactor;
    int32_t nMinElemDamage;
    int32_t nMaxElemDamage;
    int32_t nElemLength;
    int32_t nElemType;
    int32_t nSummonLife;
    int32_t nSeconds;
    int32_t nMinFrac;
    int32_t nTenthsOfSecond;
    int32_t nMissileId;
    int32_t nSubMissileId;
    int32_t nMonsterId;
    int32_t nRange;
    int32_t nMissileParam0;
    int32_t nFraction;
    int32_t nWhole;
    int32_t nQty;
    int32_t nMin;
    int32_t nMax;
    int32_t nPercent;
    int32_t nLevel;
    int32_t nGameType;
    int32_t nDifficulty;
    int32_t nFrames;
    int32_t nStatBonus;
    int32_t nValue;
    uint32_t *pStatCalc;
    int16_t *pStat;
    int32_t i;

    pTailText = NULL;
    nSuffixIdx = 0;
    nStrSkill36Idx = 0;

    if (!pSkillsTxt)
    {
        return 0;
    }
    pSkillsTxtById = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxtById)
    {
        return 0;
    }
    wSkillDesc = (int16_t)pSkillsTxtById->wSkillDesc;
    if (wSkillDesc < 0)
    {
        return 0;
    }
    if (wSkillDesc >= sgptDataTables->nSkillDescTxtRecordCount)
    {
        return 0;
    }
    pSkillDescTxt = &sgptDataTables->pSkillDescTxt[wSkillDesc];
    if (!pSkillDescTxt)
    {
        return 0;
    }

    nCalcA = D2COMMON_SKILLS_EvaluateSkillDescFormula(pPlayer, pSkillDescTxt->dwDescCalcA[nLine], nSkillId, nSkillLevel);
    nCalcB = D2COMMON_SKILLS_EvaluateSkillDescFormula(pPlayer, pSkillDescTxt->dwDescCalcB[nLine], nSkillId, nSkillLevel);
    wDescTextA = (D2C_StringIndices)pSkillDescTxt->wDescTextA[nLine];
    nDescTextB = (D2C_StringIndices)pSkillDescTxt->wDescTextB[nLine];

    switch (pSkillDescTxt->nDescLine[nLine])
    {
    case 1:     //  the mana cost, or the charges when an item grants it
        if (!pSkillsTxt->wMana && !pSkillsTxt->wLevelMana)
        {
            goto RETURN_ONE;
        }
        if (nCharges)
        {
            SKILLDESC_FormatChargesText(pwszText, nCharges);
            return 1;
        }
        pSkillsTxtById = DATATBLS_GetSkillsTxtRecordById(nSkillId);
        if (!pSkillsTxtById)
        {
            goto RETURN_ONE;
        }
        nSkillDescIdx = (int16_t)pSkillsTxtById->wSkillDesc;
        if (nSkillDescIdx < 0)
        {
            goto RETURN_ONE;
        }
        if (nSkillDescIdx >= sgptDataTables->nSkillDescTxtRecordCount)
        {
            goto RETURN_ONE;
        }
        pSkillDescByIdx = &sgptDataTables->pSkillDescTxt[nSkillDescIdx];
        if (!pSkillDescByIdx)
        {
            goto RETURN_ONE;
        }

        nStrIdx = (D2C_StringIndices)pSkillDescByIdx->wStrMana;

        //  nManaShift is read as a word over the pad byte after it (the threshold test takes the whole word), and the shift takes cl, which the
        //  machine masks to five bits (shl ebp, cl at 0x6FB10B55, shl edi, cl at 0x6FB10B6B).
        //  DIFFERENCE: the C shifts by nManaShift & 31, the same count and one C++ defines; stock data's ManaShift never passes 8.
        nManaShift = *(uint16_t *)&pSkillsTxt->nManaShift;
        wLevelMana = pSkillsTxt->wLevelMana;
        nManaCost = (pSkillsTxt->wMana + wLevelMana * (nSkillLevel - 1)) << (nManaShift & 31);
        if ((int16_t)pSkillsTxt->wMinMana << 8 > nManaCost)
        {
            nManaCost = (int16_t)pSkillsTxt->wMinMana << 8;
        }
        if (nManaShift == 8 || (wLevelMana << (nManaShift & 31)) > 256 || nManaCost > 6400)
        {
            SKILLDESC_AppendTextThenValue(pwszText, nStrIdx, nManaCost >> 8, 0);
            return 1;
        }
        if (nCalcA > 10)
        {
            SKILLDESC_AppendFixedPointValue(pwszText, nStrIdx, STR_IDX_5382_dummy, nManaCost, nCalcA, 0);
        }
        else
        {
            SKILLDESC_AppendIntAndFraction(pwszText, nStrIdx, nManaCost / 256, 10 * (nManaCost % 256) / 256, 0);
        }
        return 1;

    case 2:     //  "+value" then the A text, then B
        if (!nCalcA)
        {
            goto RETURN_ONE;
        }
        SKILLDESC_FormatInt(wszScratch, nCalcA, 1);
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        Unicode::strcat(pwszText, wszScratch);
        pTailText = D2LANG_GetStringFromTblIndex(nDescTextB);
        goto APPEND_TAIL_TEXT;

    case 3:     //  the same with the value unsigned
        if (!nCalcA)
        {
            goto RETURN_ONE;
        }
        SKILLDESC_FormatInt(wszScratch, nCalcA, 0);
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        Unicode::strcat(pwszText, wszScratch);
        pTailText = D2LANG_GetStringFromTblIndex(nDescTextB);
        goto APPEND_TAIL_TEXT;

    case 4:
        if (!nCalcA)
        {
            goto RETURN_ONE;
        }
        SKILLDESC_FormatInt(wszScratch, nCalcA, 1);
        goto APPEND_A_THEN_SCRATCH;

    case 5:
        if (!nCalcA)
        {
            goto RETURN_ONE;
        }
        SKILLDESC_FormatInt(wszScratch, nCalcA, 0);
        goto APPEND_A_THEN_SCRATCH;

    case 6:
        if (!nCalcA)
        {
            goto RETURN_ONE;
        }
        SKILLDESC_FormatInt(wszScratch, nCalcA, 1);
        goto APPEND_SCRATCH_THEN_SUFFIX_A;

    case 7:
        if (!nCalcA)
        {
            goto RETURN_ONE;
        }
        SKILLDESC_FormatInt(wszScratch, nCalcA, 0);
        goto APPEND_SCRATCH_THEN_SUFFIX_A;

    case 8:     //  the to-hit factor between two fixed labels
        if (wDescTextA != STR_IDX_5382_dummy)
        {
            Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        }
        if (nDescTextB != STR_IDX_5382_dummy)
        {
            Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nDescTextB));
        }
        nToHitFactor = D2COMMON_SKILLS_GetToHitFactor(pPlayer, nSkillId, nSkillLevel);
        if (!nToHitFactor)
        {
            goto RETURN_ONE;
        }
        SKILLDESC_AppendValueBetweenTexts(pwszText, STR_IDX_4262_StrSkill10, STR_IDX_4275_StrSkill23, nToHitFactor, 1);
        return 1;

    case 9:     //  the physical damage line
        if (wDescTextA != STR_IDX_5382_dummy)
        {
            Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        }
        if (nDescTextB != STR_IDX_5382_dummy)
        {
            Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nDescTextB));
        }
        SKILLDESC_AppendPhysDamageLine(pPlayer, pwszText, nSkillId, pSkillsTxt, nSkillLevel, nCalcA, nCalcB);
        return 1;

    case 0xA:   //  the elemental damage line, via D2Common's own numbers
        if (wDescTextA != STR_IDX_5382_dummy)
        {
            Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        }
        if (nDescTextB != STR_IDX_5382_dummy)
        {
            Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nDescTextB));
        }
        nMinElemDamage = D2COMMON_SKILLS_GetMinElemDamage(pPlayer, nSkillId, nSkillLevel, 1);
        nMaxElemDamage = D2COMMON_SKILLS_GetMaxElemDamage(pPlayer, nSkillId, nSkillLevel, 1);
        if (!nMinElemDamage && !nMaxElemDamage)
        {
            goto RETURN_ONE;
        }
        SKILLDESC_AppendElemDamageLine(pwszText, nMinElemDamage >> 8, nMaxElemDamage >> 8, pSkillsTxt->nEType, 0);
        return 1;

    case 0xB:   //  the duration, and only Cold and Poison have one
        if (wDescTextA != STR_IDX_5382_dummy)
        {
            Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        }
        if (nDescTextB != STR_IDX_5382_dummy)
        {
            Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nDescTextB));
        }
        nElemLength = D2COMMON_SKILLS_GetElementalLength(pPlayer, nSkillId, nSkillLevel, 0);
        if (!nElemLength)
        {
            goto RETURN_ONE;
        }
        if (pSkillsTxt->nEType == 4)
        {
            SKILLDESC_AppendFramesAsSeconds(pwszText, STR_IDX_4265_StrSkill13, nElemLength, 0);
            return 1;
        }
        if (pSkillsTxt->nEType != 5)
        {
            goto RETURN_ONE;
        }
        SKILLDESC_AppendFramesAsSeconds(pwszText, STR_IDX_4266_StrSkill14, nElemLength, 0);
        return 1;

    case 0xC:   //  the A text is the label the helper receives
        if (nDescTextB != STR_IDX_5382_dummy)
        {
            Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nDescTextB));
        }
        SKILLDESC_AppendFramesAsSeconds(pwszText, wDescTextA, nCalcA, 0);
        return 1;

    case 0xD:   //  the summoned monster's life
        {
            int32_t wSummon;

            wSummon = (int16_t)pSkillsTxt->wSummon;
            pMonStatsTxtById = DATATBLS_GetMonStatsTxtRecord(wSummon);
            if (!pMonStatsTxtById)
            {
                goto RETURN_ONE;
            }
            nDifficulty = GAME_GetDifficulty();
            memset(&tSummon, 0, sizeof(tSummon));
            nLevel = (int16_t)pMonStatsTxtById->nLevel[nDifficulty];
            nGameType = GAME_IsLadderGame();
            if (!D2COMMON_DATATBLS_CalculateMonsterStatsByLevel(wSummon, nGameType, nDifficulty, nLevel, 1, &tSummon))
            {
                goto RETURN_ONE;
            }
            nSummonLife = nCalcB + nCalcA * ((tSummon.nMinHP + tSummon.nMaxHP) / 2) / 100 + (tSummon.nMinHP + tSummon.nMaxHP) / 2;
            if (!nSummonLife)
            {
                goto RETURN_ONE;
            }
            SKILLDESC_AppendTextThenValue(pwszText, STR_IDX_4294_StrSkill42, nSummonLife, 0);
            return 1;
        }

    case 0xE:   //  the length line and the per-frame damage it repeats
        if (wDescTextA != STR_IDX_5382_dummy)
        {
            Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        }
        if (nDescTextB != STR_IDX_5382_dummy)
        {
            Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nDescTextB));
        }
        nFrames = D2COMMON_SKILLS_GetElementalLength(pPlayer, nSkillId, nSkillLevel, 0);
        nMin = D2COMMON_SKILLS_GetMinElemDamage(pPlayer, nSkillId, nSkillLevel, 1);
        nMax = (nFrames * D2COMMON_SKILLS_GetMaxElemDamage(pPlayer, nSkillId, nSkillLevel, 1)) >> 8;
        SKILLDESC_AppendFramesAsSeconds(pwszText, STR_IDX_11022_StrSkill63Patch, nFrames, 0);
        SKILLDESC_AppendElemDamageLine(pwszText, (nFrames * nMin) >> 8, nMax, pSkillsTxt->nEType, 0);
        return 1;

    case 0xF:   //  "A: B"
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3997_colon));
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        pTailText = D2LANG_GetStringFromTblIndex(nDescTextB);
        goto APPEND_TAIL_TEXT;

    case 0x10:  //  the seconds range: /25 on each end, two formatter arms
        nSeconds = nCalcA / 25;
        nMinFrac = 10 * (nCalcA % 25) / 25;
        nTenthsOfSecond = 10 * (nCalcB % 25);
        if (nSeconds)
        {
            if (nMinFrac)
            {
                goto FORMAT_FRAC_RANGE;
            }
        }
        else
        {
            if (nMinFrac)
            {
                goto FORMAT_FRAC_RANGE;
            }
            if (!(nCalcB / 25))
            {
                goto RETURN_ONE;
            }
        }
        if (!(nTenthsOfSecond / 25))
        {
            SKILLDESC_FormatDamageRange(wszScratch, nSeconds, nCalcB / 25, 0, 0);
            goto APPEND_SECONDS_TAIL;
        }
    FORMAT_FRAC_RANGE:
        SKILLDESC_FormatFracRange(wszScratch, nSeconds, nMinFrac, nCalcB / 25, nTenthsOfSecond / 25, 0);
    APPEND_SECONDS_TAIL:
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_4272_StrSkill20));
        Unicode::strcat(pwszText, wszScratch);
        pTailText = D2LANG_GetStringFromTblIndex(STR_IDX_4268_StrSkill16);
        goto APPEND_TAIL_TEXT;

    case 0x11:  //  the A-B range, or the single value B when equal
        if (nDescTextB != STR_IDX_5382_dummy)
        {
            Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nDescTextB));
        }
        if (nCalcA == nCalcB)
        {
            SKILLDESC_AppendValueBetweenTexts(pwszText, wDescTextA, STR_IDX_4286_StrSkill34, nCalcA, 0);
            return 1;
        }
        SKILLDESC_FormatDamageRange(wszScratch, nCalcA, nCalcB, 0, 0);
        if (wDescTextA != STR_IDX_5382_dummy)
        {
            Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        }
        Unicode::strcat(pwszText, wszScratch);
        pTailText = D2LANG_GetStringFromTblIndex(STR_IDX_4286_StrSkill34);
        goto APPEND_TAIL_TEXT;

    case 0x12:  //  the coloured A line and a coloured newline
        Unicode::strcpy(wszBuild, D2LANG_GetStringFromTblIndex(wDescTextA));
        if (nCalcA > 0)
        {
            UI_AddColorCodeToWstring((wchar_t *)wszBuild, nCalcA);
        }
        Unicode::strcat(pwszText, wszBuild);
        Unicode::strcpy(wszBuild, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
        UI_AddColorCodeToWstring((wchar_t *)wszBuild, nColor);
        Unicode::strcat(pwszText, wszBuild);
        return 1;

    case 0x13:  //  20/3 into a one-decimal value, clamped up to 1
        if (nDescTextB != STR_IDX_5382_dummy)
        {
            Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nDescTextB));
        }
        nFraction = 20 * nCalcA / 3 % 10;
        nWhole = 20 * nCalcA / 3 / 10;
        if (nWhole < 1)
        {
            nWhole = 1;
        }
        SKILLDESC_FormatFixedPointOrInt(wszScratch, nWhole, nFraction, 0);
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        Unicode::strcat(pwszText, wszScratch);
        nStrSkill36Idx = (nWhole != 1) ? STR_IDX_4278_StrSkill26 : STR_IDX_4288_StrSkill36;
        pTailText = D2LANG_GetStringFromTblIndex((D2C_StringIndices)nStrSkill36Idx);
        goto APPEND_TAIL_TEXT;

    case 0x14:
        if (!nCalcA)
        {
            goto RETURN_ONE;
        }
        SKILLDESC_FormatInt(wszScratch, nCalcA, 1);
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        Unicode::strcat(pwszText, wszScratch);
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_4275_StrSkill23));
        pTailText = D2LANG_GetStringFromTblIndex(nDescTextB);
        goto APPEND_TAIL_TEXT;

    case 0x15:
        if (!nCalcA)
        {
            goto RETURN_ONE;
        }
        SKILLDESC_FormatInt(wszScratch, nCalcA, 0);
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        Unicode::strcat(pwszText, wszScratch);
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_4275_StrSkill23));
        pTailText = D2LANG_GetStringFromTblIndex(nDescTextB);
        goto APPEND_TAIL_TEXT;

    case 0x16:  //  the skill's missile damage per second, masteries folded in HERE
        {
            //  the original reuses the pSkillDesc slot for the max value
            nMissileId = (int16_t)pSkillDescTxt->wDescMissile[0];
            nMin = 75 * D2COMMON_MISSILE_GetMinElemDamage(NULL, pPlayer, nMissileId, nSkillLevel);
            nMax = 75 * D2COMMON_MISSILE_GetMaxElemDamage(NULL, pPlayer, nMissileId, nSkillLevel);
            SKILLDESC_ApplyFireMasteryToRange(pSkillsTxt, &nMin, &nMax);
            SKILLDESC_ApplyLightningMasteryToRange(pSkillsTxt, &nMin, &nMax);
            nMin >>= 8;
            nMax >>= 8;
            switch (D2COMMON_MISSILE_GetElemTypeFromMissileId(nMissileId))
            {
            case 2:  nStrIdx = STR_IDX_4259_StrSkill7;  break;
            case 3:  nStrIdx = STR_IDX_4291_StrSkill39; break;
            case 4:  nStrIdx = STR_IDX_4258_StrSkill6;  break;
            case 5:  nStrIdx = STR_IDX_4260_StrSkill8;  break;
            case 6:  nStrIdx = STR_IDX_3524_ModStr2z;   break;
            default: nStrIdx = STR_IDX_4257_StrSkill5;  break;
            }
            SKILLDESC_AppendAveragePerSecondLine(pwszText, nMin, nMax, nStrIdx);
            return 1;
        }

    case 0x17:  //  the missile's own range, in seconds
        nMissileId = (int16_t)pSkillDescTxt->wDescMissile[0];
        if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        {
            return 0;
        }
        pMissileRec = &sgptDataTables->pMissilesTxt[nMissileId];
        goto APPEND_MISSILE_RANGE;

    case 0x18:
        if (nDescTextB != STR_IDX_5382_dummy)
        {
            Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nDescTextB));
        }
        nMinElemDamage = D2COMMON_SKILLS_GetMinElemDamage(pPlayer, nSkillId, nSkillLevel, 1);
        nMaxElemDamage = D2COMMON_SKILLS_GetMaxElemDamage(pPlayer, nSkillId, nSkillLevel, 1);
        if (!nMinElemDamage && !nMaxElemDamage)
        {
            goto RETURN_ONE;
        }
        SKILLDESC_AppendElemDamageLine(pwszText, nMinElemDamage >> 8, nMaxElemDamage >> 8, pSkillsTxt->nEType, wDescTextA);
        return 1;

    case 0x19:
        Unicode::strcpy(wszBuild, D2LANG_GetStringFromTblIndex(wDescTextA));
        if (nDescTextB != STR_IDX_5382_dummy)
        {
            Unicode::strcat(wszBuild, D2LANG_GetStringFromTblIndex(nDescTextB));
        }
        if (nCalcA > 0)
        {
            UI_AddColorCodeToWstring((wchar_t *)wszBuild, nCalcA);
        }
        Unicode::strcat(pwszText, wszBuild);
        Unicode::strcpy(wszBuild, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
        goto APPEND_COLOR_AND_BUILD;

    case 0x1A:  //  25/256 into a per-second pair, labelled by element
        if (wDescTextA != STR_IDX_5382_dummy)
        {
            Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        }
        if (nDescTextB != STR_IDX_5382_dummy)
        {
            Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nDescTextB));
        }
        nMin = (25 * D2COMMON_SKILLS_GetMinElemDamage(pPlayer, nSkillId, nSkillLevel, 1)) >> 8;
        nMax = (25 * D2COMMON_SKILLS_GetMaxElemDamage(pPlayer, nSkillId, nSkillLevel, 1)) >> 8;
        switch (pSkillsTxt->nEType)
        {
        case 2:  SKILLDESC_AppendAveragePerSecondLine(pwszText, nMin, nMax, STR_IDX_4259_StrSkill7);  break;
        case 3:  SKILLDESC_AppendAveragePerSecondLine(pwszText, nMin, nMax, STR_IDX_4291_StrSkill39); break;
        case 4:  SKILLDESC_AppendAveragePerSecondLine(pwszText, nMin, nMax, STR_IDX_4258_StrSkill6);  break;
        case 5:  SKILLDESC_AppendAveragePerSecondLine(pwszText, nMin, nMax, STR_IDX_4260_StrSkill8);  break;
        case 6:  SKILLDESC_AppendAveragePerSecondLine(pwszText, nMin, nMax, STR_IDX_3524_ModStr2z);   break;
        default: SKILLDESC_AppendAveragePerSecondLine(pwszText, nMin, nMax, STR_IDX_4257_StrSkill5);  break;
        }
        return 1;

    case 0x1B:  //  the skill's own numbers at 75/256, Fire's label
        if (wDescTextA != STR_IDX_5382_dummy)
        {
            Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        }
        if (nDescTextB != STR_IDX_5382_dummy)
        {
            Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nDescTextB));
        }
        nMin = (75 * D2COMMON_SKILLS_GetMinElemDamage(pPlayer, nSkillId, nSkillLevel, 1)) >> 8;
        nMax = (75 * D2COMMON_SKILLS_GetMaxElemDamage(pPlayer, nSkillId, nSkillLevel, 1)) >> 8;
        SKILLDESC_AppendAveragePerSecondLine(pwszText, nMin, nMax, STR_IDX_4257_StrSkill5);
        return 1;

    case 0x1C:  //  the missile's dwParam[0] as 20/3, "StrSkill18" prefix
        nMissileId = (int16_t)pSkillDescTxt->wDescMissile[0];
        if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        {
            return 0;
        }
        pMissileRec = &sgptDataTables->pMissilesTxt[nMissileId];
        if (!pMissileRec)
        {
            return 0;
        }
        nMissileParam0 = pMissileRec->dwParam[0];
        nFraction = 20 * nMissileParam0 / 3 % 10;
        nWhole = 20 * nMissileParam0 / 3 / 10;
        if (nWhole < 1)
        {
            nWhole = 1;
        }
        SKILLDESC_FormatFixedPointOrInt(wszScratch, nWhole, nFraction, 0);
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_4270_StrSkill18));
        Unicode::strcat(pwszText, wszScratch);
        nSuffixIdx = (nWhole != 1) ? STR_IDX_4278_StrSkill26 : STR_IDX_4288_StrSkill36;
        goto APPEND_SUFFIX_AND_NEWLINE;

    case 0x1D:  // the missile's range ramp times 2/3
        nMissileId = (int16_t)pSkillDescTxt->wDescMissile[0];
        if (nMissileId < 0 || nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        {
            return 0;
        }
        pMissileRec = &sgptDataTables->pMissilesTxt[nMissileId];
        if (!pMissileRec)
        {
            return 0;
        }
        nRange = ((int16_t)pMissileRec->wRange + (nSkillLevel - 1) * (int16_t)pMissileRec->wLevRange) * 2 / 3;
        if (!nRange)
        {
            goto RETURN_ONE;
        }
        SKILLDESC_FormatInt(wszScratch, nRange, 0);
        Unicode::strcat(pwszText, wszScratch);
        pTailText = D2LANG_GetStringFromTblIndex(STR_IDX_4278_StrSkill26);
        goto APPEND_TAIL_TEXT;

    case 0x1E:  //  the sub-missile's range ramp, same tail
        nMissileId = (int16_t)pSkillDescTxt->wDescMissile[0];
        if (nMissileId < 0)
        {
            return 0;
        }
        if (nMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        {
            return 0;
        }
        pMissileRec = &sgptDataTables->pMissilesTxt[nMissileId];
        if (!pMissileRec)
        {
            return 0;
        }
        nSubMissileId = (int16_t)pMissileRec->wSubMissile[0];
        if (nSubMissileId < 0 || nSubMissileId >= sgptDataTables->nMissilesTxtRecordCount)
        {
            return 0;
        }
        pMissileRec = &sgptDataTables->pMissilesTxt[nSubMissileId];
        goto APPEND_MISSILE_RANGE;

    case 0x1F:  //  the AI-curse duration, divided by the difficulty's own divisor
        nDifficulty = GAME_GetDifficulty();
        pDifficultyLevels = D2COMMON_DATATBLS_GetDifficultyLevelsTxtRecord(nDifficulty);
        nFrames = nCalcA;
        //  `test ecx, ecx` / `jle` (0x6FB11C55) and `idiv ecx` (0x6FB11C5A): the divisor is tested and divided signed; the struct carries it as
        //  unsigned int, hence the cast.
        if ((int32_t)pDifficultyLevels->dwAiCurseDiv > 0)
        {
            nFrames = nCalcA / (int32_t)pDifficultyLevels->dwAiCurseDiv;
        }
        SKILLDESC_AppendFramesAsSeconds(pwszText, wDescTextA, nFrames, 0);
        return 1;

    case 0x20:
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        if (!nCalcA)
        {
            goto RETURN_ONE;
        }
        SKILLDESC_FormatInt(wszScratch, nCalcA, 1);
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nDescTextB));
        Unicode::strcat(pwszText, wszScratch);
        pTailText = D2LANG_GetStringFromTblIndex(STR_IDX_4275_StrSkill23);
        goto APPEND_TAIL_TEXT;

    case 0x21:
        if (nSkillLevel <= 1)
        {
            goto RETURN_ONE;
        }
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        pTailText = D2LANG_GetStringFromTblIndex(nDescTextB);
        goto APPEND_TAIL_TEXT;

    case 0x22:  //  the summon numbers with the aura/passive percent folded in
        {
            int32_t wSummon;

            wSummon = (int16_t)pSkillsTxt->wSummon;
            pSummonMonStats = DATATBLS_GetMonStatsTxtRecord(wSummon);
            if (!pSummonMonStats)
            {
                goto RETURN_ONE;
            }
            memset(&tSummon, 0, sizeof(tSummon));
            //  GAME_GetDifficulty is called twice, for the level's index (0x6FB11D75) and again for the argument (0x6FB11D88); the C makes both calls
            nLevel = (int16_t)pSummonMonStats->nLevel[GAME_GetDifficulty()];
            nDifficulty = GAME_GetDifficulty();
            nGameType = GAME_IsLadderGame();
            D2COMMON_DATATBLS_CalculateMonsterStatsByLevel((int16_t)pSkillsTxt->wSummon, nGameType, nDifficulty, nLevel, 8, &tSummon);
            nPercent = 0;
            nMin = tSummon.nA1MinD;
            nMax = tSummon.nA1MaxD;
            pStatCalc = (uint32_t *)pSkillsTxt->dwAuraStatCalc;
            pStat = pSkillsTxt->wAuraStat;
            for (i = 0; i < 6; ++i)
            {
                if (pStat[i] == STAT_DAMAGEPERCENT)
                {
                    nPercent += D2COMMON_SKILLS_EvaluateSkillFormula(pPlayer, pStatCalc[i], nSkillId, nSkillLevel);
                }
                if (pStat[i] == STAT_ITEM_NORMALDAMAGE)
                {
                    nValue = D2COMMON_SKILLS_EvaluateSkillFormula(pPlayer, pStatCalc[i], nSkillId, nSkillLevel);
                    nMin += nValue;
                    nMax += nValue;
                }
            }
            pStatCalc = pSkillsTxt->dwPassiveCalc;
            pStat = pSkillsTxt->nPassiveStat;
            for (i = 0; i < 5; ++i)
            {
                if (pStat[i] == STAT_DAMAGEPERCENT)
                {
                    nPercent += D2COMMON_SKILLS_EvaluateSkillFormula(pPlayer, pStatCalc[i], nSkillId, nSkillLevel);
                }
                if (pStat[i] == STAT_ITEM_NORMALDAMAGE)
                {
                    nValue = D2COMMON_SKILLS_EvaluateSkillFormula(pPlayer, pStatCalc[i], nSkillId, nSkillLevel);
                    nMin += nValue;
                    nMax += nValue;
                }
            }
            if (nPercent)
            {
                if (nPercent <= -90)
                {
                    nPercent = -90;
                }
                nMin += DATATBLS_ApplyRatio(nMin, nPercent, 100);
                nMax += DATATBLS_ApplyRatio(nMax, nPercent, 100);
            }
            SKILLDESC_AppendElemDamageLine(pwszText, nMin, nMax, 0, STR_IDX_4256_StrSkill4);
            return 1;
        }

    case 0x23:  //  "A: <damage range>"
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3997_colon));
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        SKILLDESC_FormatDamageRange(wszScratch, nCalcA, nCalcB, 0, 0);
        pTailText = wszScratch;
        goto APPEND_TAIL_TEXT;

    case 0x24:  //  the value then B -- or A when the value is exactly one
        if (!nCalcA)
        {
            goto RETURN_ONE;
        }
        SKILLDESC_FormatInt(wszScratch, nCalcA, 0);
        Unicode::strcat(pwszText, wszScratch);
        nSuffixIdx = (nCalcA != 1) ? nDescTextB : wDescTextA;
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex((D2C_StringIndices)nSuffixIdx));
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
        return 1;

    case 0x25:  //  10/3 into a one-decimal value, clamped up to 1
        nFraction = 10 * nCalcA / 3 % 10;
        nWhole = 10 * nCalcA / 3 / 10;
        if (nWhole < 1)
        {
            nWhole = 1;
        }
        SKILLDESC_FormatFixedPointOrInt(wszScratch, nWhole, nFraction, 0);
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        Unicode::strcat(pwszText, wszScratch);
        nStrSkill36Idx = (nWhole != 1) ? STR_IDX_4278_StrSkill26 : STR_IDX_4288_StrSkill36;
        pTailText = D2LANG_GetStringFromTblIndex((D2C_StringIndices)nStrSkill36Idx);
        goto APPEND_TAIL_TEXT;

    case 0x26:
        if (nCalcA == nCalcB)
        {
            goto APPEND_RANGE_EQUAL;
        }
        SKILLDESC_FormatDamageRange(wszScratch, nCalcA, nCalcB, 0, 0);
        goto APPEND_RANGE_TAIL;

    case 0x27:  //  the summon numbers with only the "damagepercent" bonus
        {
            int32_t wSummon;

            wSummon = (int16_t)pSkillsTxt->wSummon;
            pMonStatsTxtById = DATATBLS_GetMonStatsTxtRecord(wSummon);
            if (!pMonStatsTxtById)
            {
                goto RETURN_ONE;
            }
            memset(&tSummon, 0, sizeof(tSummon));
            //  two GAME_GetDifficulty calls here too (0x6FB1214D, 0x6FB12160), as in case 0x22
            nLevel = (int16_t)pMonStatsTxtById->nLevel[GAME_GetDifficulty()];
            nDifficulty = GAME_GetDifficulty();
            nGameType = GAME_IsLadderGame();
            nMonsterId = (int16_t)pSkillsTxt->wSummon;
            D2COMMON_DATATBLS_CalculateMonsterStatsByLevel(nMonsterId, nGameType, nDifficulty, nLevel, 8, &tSummon);
            nStatBonus = 0;
            pStatCalc = (uint32_t *)pSkillsTxt->dwAuraStatCalc;
            pStat = pSkillsTxt->wAuraStat;
            for (i = 0; i < 6; ++i)
            {
                if (pStat[i] == STAT_DAMAGEPERCENT)
                {
                    nStatBonus += D2COMMON_SKILLS_EvaluateSkillFormula(pPlayer, pStatCalc[i], nSkillId, nSkillLevel);
                }
            }
            pStatCalc = pSkillsTxt->dwPassiveCalc;
            pStat = pSkillsTxt->nPassiveStat;
            for (i = 0; i < 5; ++i)
            {
                if (pStat[i] == STAT_DAMAGEPERCENT)
                {
                    nStatBonus += D2COMMON_SKILLS_EvaluateSkillFormula(pPlayer, pStatCalc[i], nSkillId, nSkillLevel);
                }
            }
            nMin = tSummon.nA1MinD;
            nMax = tSummon.nA1MaxD;
            if (nStatBonus)
            {
                if (nStatBonus <= -90)
                {
                    nStatBonus = -90;
                }
                nMin += DATATBLS_ApplyRatio(nMin, nStatBonus, 100);
                nMax += DATATBLS_ApplyRatio(nMax, nStatBonus, 100);
            }
            SKILLDESC_AppendElemDamageLine(pwszText, nMin, nMax, 0, STR_IDX_4256_StrSkill4);
            return 1;
        }

    case 0x28:
        if (wDescTextA == STR_IDX_5382_dummy)
        {
            return 0;
        }
        goto SPRINTF_A_AND_B;

    case 0x29:  //  the Fire Golem block: validated id, its own A1 damage plus nCalcA/B
        {
            int32_t wSummon;

            wSummon = (int16_t)pSkillsTxt->wSummon;
            pMonStatsTxtById = DATATBLS_GetMonStatsTxtRecord(wSummon);
            if (!pMonStatsTxtById)
            {
                goto RETURN_ONE;
            }
            nDifficulty = GAME_GetDifficulty();
            memset(&tSummon, 0, sizeof(tSummon));
            nLevel = (int16_t)pMonStatsTxtById->nLevel[nDifficulty];
            nGameType = GAME_IsLadderGame();
            nMonsterId = SKILLDESC_ValidateMonsterId(MONSTER_FIREGOLEM);
            D2COMMON_DATATBLS_CalculateMonsterStatsByLevel(nMonsterId, nGameType, nDifficulty, nLevel, 8, &tSummon);
            SKILLDESC_AppendElemDamageLine(pwszText, tSummon.nA1MinD + nCalcA, nCalcB + tSummon.nA1MaxD, pSkillsTxt->nEType, 0);
            return 1;
        }

    case 0x2A:  //  "A: <fixed point> B"
        if (wDescTextA == STR_IDX_5382_dummy || (!nCalcA && !nCalcB))
        {
            goto RETURN_ONE;
        }
        SKILLDESC_FormatFixedPoint(wszScratch, nCalcA, nCalcB, 1);
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3997_colon));
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        Unicode::strcat(pwszText, wszScratch);
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        pTailText = D2LANG_GetStringFromTblIndex(nDescTextB);
        goto APPEND_TAIL_TEXT;

    case 0x2B:
        SKILLDESC_AppendScaledDamageRange(pwszText, wDescTextA, nDescTextB, nCalcA, nCalcB, 10);
        return 1;

    case 0x2C:
        SKILLDESC_AppendScaledDamageRange(pwszText, wDescTextA, nDescTextB, nCalcA, nCalcB, 100);
        return 1;

    case 0x2D:
        if (nSkillLevel <= 1)
        {
            goto RETURN_ONE;
        }
        nValue = SKILLDESC_GetLinearParamByLevel(nSkillId, nSkillLevel - 1);
        SKILLDESC_AppendValueThenText(pwszText, STR_IDX_4309_StrSkill55, nValue, 0);
        pTailText = D2LANG_GetStringFromTblIndex(STR_IDX_4365_StrSkill112);
        goto APPEND_TAIL_TEXT;

    case 0x2E:
        if (!nCalcB || wDescTextA == STR_IDX_5382_dummy)
        {
            return 0;
        }
        goto SPRINTF_A_AND_B;

    case 0x2F:
        if (nCalcA == nCalcB)
        {
            SKILLDESC_AppendTextThenValue(pwszText, wDescTextA, nCalcA, 0);
            return 1;
        }
        SKILLDESC_FormatDamageRange(wszScratch, nCalcA, nCalcB, 0, 0);
        goto APPEND_A_THEN_SCRATCH;

    case 0x30:  //  the melee block: four builders in an expansion game, one in classic
        if (GAME_IsExpansionGame())
        {
            SKILLDESC_AppendElemLengthLine(pPlayer, pwszText, nSkillId, pSkillsTxt, nSkillLevel);
            SKILLDESC_AppendSkillElemDamageLine(pPlayer, pwszText, nSkillId, pSkillsTxt, nSkillLevel, 0);
            SKILLDESC_AppendPhysDamageLine(pPlayer, pwszText, nSkillId, pSkillsTxt, nSkillLevel, 0, 0);
            SKILLDESC_AppendToHitLine(pPlayer, pwszText, nSkillId, pSkillsTxt, nSkillLevel);
        }
        else
        {
            int32_t nMinTotal;
            int32_t nMaxTotal;
            int32_t nColorOut;

            nMinTotal = 0;
            nMaxTotal = 0;
            nColorOut = 0;
            SKILLDESC_AddElemDamageAndColor(pPlayer, pSkillsTxt, nSkillLevel, &nMinTotal, &nMaxTotal, &nColorOut);
            SKILLDESC_AppendDamageRangeLine(pwszText, STR_IDX_4256_StrSkill4, nMinTotal, nMaxTotal, 0, 0);
        }
        return 1;

    case 0x31:
        nMin = D2COMMON_SKILLS_GetMinPhysDamage(pPlayer, nSkillId, nSkillLevel, 1) >> 8;
        nMax = D2COMMON_SKILLS_GetMaxPhysDamage(pPlayer, nSkillId, nSkillLevel, 1) >> 8;
        if (nMin == nMax)
        {
            SKILLDESC_AppendTextThenValue(pwszText, wDescTextA, nMin, 1);
            return 1;
        }
        SKILLDESC_FormatDamageRange(wszScratch, nMin, nMax, 1, 0);
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        pTailText = wszScratch;
        goto APPEND_TAIL_TEXT;

    case 0x32:  //  the missile's damage, its element from the missile
        nMissileId = (int16_t)pSkillDescTxt->wDescMissile[0];
        nMin = D2COMMON_MISSILE_GetMinElemDamage(NULL, pPlayer, nMissileId, nSkillLevel) >> 8;
        nMax = D2COMMON_MISSILE_GetMaxElemDamage(NULL, pPlayer, nMissileId, nSkillLevel) >> 8;
        if (!nMin && !nMax)
        {
            goto RETURN_ONE;
        }
        nElemType = D2COMMON_MISSILE_GetElemTypeFromMissileId(nMissileId);
        SKILLDESC_AppendElemDamageLine(pwszText, nMin, nMax, nElemType, wDescTextA);
        return 1;

    case 0x33:  //  the A text as a format with nCalcA -- or the dummy line
        if (wDescTextA == STR_IDX_5382_dummy)
        {
            SKILLDESC_AppendTextLine(pwszText, STR_IDX_5382_dummy);
        }
        else
        {
            Unicode::sprintf(256, wszBuild, D2LANG_GetStringFromTblIndex(wDescTextA), nCalcA);
            Unicode::strcat(wszBuild, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
            Unicode::strcat(pwszText, wszBuild);
        }
        return 1;

    case 0x34:
        if (nCalcA == nCalcB)
        {
            goto APPEND_RANGE_EQUAL;
        }
        SKILLDESC_FormatDamageRange(wszScratch, nCalcA, nCalcB, 1, 0);
        goto APPEND_RANGE_TAIL;

    case 0x35:  //  the scroll-book hint, drawn straight to the screen
        nQty = SPELLSEL_SkillDescGetSkillQuantity_Always0(pPlayer, nSkillId);
        SKILLDESC_DrawQuantityHint(nQty, wDescTextA);
        return 1;

    case 0x36:  // DIFFERENCE: the original has three identical bodies for 0x36..0x38; ours has one
    case 0x37:
    case 0x38:
        nQty = SPELLSEL_SkillDescGetSkillQuantity_Always0(pPlayer, nSkillId);
        SKILLDESC_DrawQuantityHint(nQty, wDescTextA);
        return 1;

    case 0x39:
        SKILLDESC_AppendFramesAsSeconds(pwszText, wDescTextA, nCalcA, 1);
        return 1;

    case 0x3A:
        SKILLDESC_AppendRangeBetweenTexts(pwszText, wDescTextA, nDescTextB, nCalcA, nCalcB, 1);
        return 1;

    case 0x3B:
        if (nDescTextB != STR_IDX_5382_dummy)
        {
            Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nDescTextB));
        }
        SKILLDESC_AppendElemDamageLine(pwszText, nCalcA, nCalcB, pSkillsTxt->nEType, wDescTextA);
        return 1;

    case 0x3C:
        if (!nCalcB)
        {
            nCalcB = 10;
        }
        SKILLDESC_AppendFixedPointValue(pwszText, wDescTextA, nDescTextB, nCalcA, nCalcB, 1);
        return 1;

    case 0x3D:
        if (!nCalcB)
        {
            nCalcB = 10;
        }
        SKILLDESC_AppendFixedPointValue(pwszText, wDescTextA, nDescTextB, nCalcA, nCalcB, 0);
        return 1;

    case 0x3E:
        SKILLDESC_AppendRangeBetweenTexts(pwszText, wDescTextA, nDescTextB, nCalcA, nCalcB, 0);
        return 1;

    case 0x3F:
        SKILLDESC_AppendValueThenTextLine(pwszText, wDescTextA, nDescTextB, nCalcA, 1, 1);
        return 1;

    case 0x40:
        SKILLDESC_AppendValueOverValueLine(pwszText, wDescTextA, nDescTextB, nCalcA, nCalcB, 1);
        return 1;

    case 0x41:
        SKILLDESC_AppendTextPairLine(pwszText, wDescTextA, nDescTextB);
        return 1;

    case 0x42:
        SKILLDESC_AppendSprintfLine(pwszText, wDescTextA, nCalcA);
        return 1;

    case 0x43:
        SKILLDESC_AppendValueThenTextLine(pwszText, wDescTextA, nDescTextB, nCalcA, 1, 0);
        return 1;

    case 0x44:
        SKILLDESC_AppendValueThenTextPair(pwszText, wDescTextA, nDescTextB, nCalcA, 0);
        return 1;

    case 0x45:
        SKILLDESC_AppendTextThenValueLine(pwszText, wDescTextA, nDescTextB, nCalcA, 0, 0);
        return 1;

    case 0x46:
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        SKILLDESC_AppendTextThenValue(pwszText, nDescTextB, nCalcA, 1);
        return 1;

    case 0x47:
        if (wDescTextA == STR_IDX_5382_dummy || nDescTextB == STR_IDX_5382_dummy)
        {
            return 0;
        }
        Unicode::sprintf(100, wszTemp, D2LANG_GetStringFromTblIndex(nDescTextB), nCalcA);
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3997_colon));
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        pTailText = wszTemp;
        goto APPEND_TAIL_TEXT;

    case 0x48:
        if (!nCalcA || nCalcB <= 0)
        {
            return 0;
        }
        SKILLDESC_FormatInt(wszScratch, nCalcA, 1);
        goto APPEND_SLASH_TAIL;

    case 0x49:
        if (!nCalcA || nCalcB <= 0)
        {
            return 0;
        }
        SKILLDESC_FormatInt(wszScratch, nCalcA, 0);
    APPEND_SLASH_TAIL:
        Unicode::strcat(pwszText, wszScratch);
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_4000_slash));
        SKILLDESC_FormatInt(wszScratch, nCalcB, 0);
        Unicode::strcat(pwszText, wszScratch);
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        nSuffixIdx = (int16_t)wDescTextA;
        goto APPEND_SUFFIX_AND_NEWLINE;

    case 0x4A:
        if (wDescTextA == STR_IDX_5382_dummy)
        {
            return 0;
        }
        Unicode::sprintf(100, wszTemp, D2LANG_GetStringFromTblIndex(wDescTextA), nCalcA);
        goto APPEND_TEMP;

    case 0x4B:
        if (wDescTextA == STR_IDX_5382_dummy)
        {
            return 0;
        }
        Unicode::sprintf(100, wszTemp, D2LANG_GetStringFromTblIndex(wDescTextA), nCalcA, nCalcB);
    APPEND_TEMP:
        pTailText = wszTemp;
        goto APPEND_TAIL_TEXT;

    default:
        return 0;
    }

//  shared tails

RETURN_ONE:
    return 1;

APPEND_NEWLINE_AND_RETURN:
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    return 1;

APPEND_SUFFIX_AND_NEWLINE:
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex((D2C_StringIndices)nSuffixIdx));
    goto APPEND_NEWLINE_AND_RETURN;

APPEND_TAIL_TEXT:
    Unicode::strcat(pwszText, pTailText);
    goto APPEND_NEWLINE_AND_RETURN;

APPEND_A_THEN_SCRATCH:
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
    pTailText = wszScratch;
    goto APPEND_TAIL_TEXT;

APPEND_SCRATCH_THEN_SUFFIX_A:
    Unicode::strcat(pwszText, wszScratch);
    nSuffixIdx = (int16_t)wDescTextA;
    goto APPEND_SUFFIX_AND_NEWLINE;

APPEND_RANGE_EQUAL:
    SKILLDESC_AppendValueBetweenTexts(pwszText, wDescTextA, nDescTextB, nCalcA, 0);
    return 1;

APPEND_RANGE_TAIL:
    if (wDescTextA != STR_IDX_5382_dummy)
    {
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wDescTextA));
    }
    Unicode::strcat(pwszText, wszScratch);
    nSuffixIdx = (int16_t)nDescTextB;
    if (nDescTextB != STR_IDX_5382_dummy)
    {
        goto APPEND_SUFFIX_AND_NEWLINE;
    }
    goto APPEND_NEWLINE_AND_RETURN;

APPEND_MISSILE_RANGE:
    if (!pMissileRec)
    {
        return 0;
    }
    SKILLDESC_AppendFramesAsSeconds(pwszText, wDescTextA, (int16_t)pMissileRec->wRange + nSkillLevel * (int16_t)pMissileRec->wLevRange, 0);
    return 1;

SPRINTF_A_AND_B:
    Unicode::sprintf(256, wszBuild, D2LANG_GetStringFromTblIndex(wDescTextA), D2LANG_GetStringFromTblIndex(nDescTextB));
    if (nCalcA > 0)
    {
        UI_AddColorCodeToWstring((wchar_t *)wszBuild, nCalcA);
    }
    Unicode::strcat(pwszText, wszBuild);
    Unicode::strcpy(wszBuild, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));

APPEND_COLOR_AND_BUILD:
    UI_AddColorCodeToWstring((wchar_t *)wszBuild, nColor);
    Unicode::strcat(pwszText, wszBuild);
    return 1;
}


//1.10f: D2Client.0x6FB12F80
//  If the skill's element is Fire (pSkillsTxt->nEType == 1), scales *pnMin and *pnMax by the local player's STAT_PASSIVE_FIRE_MASTERY percentage;
//  nothing happens for any other element. The unit comes from CUNIT_GetPlayer() rather than from an argument, so this is deliberately the local
//  player's mastery (a description panel, not a damage calculation for an arbitrary unit). Its one caller is descline 0x16, whose numbers come
//  from MISSILE_GetMin/MaxElemDamage, raw damage. This pair is the client re-deriving the missile's mastery for display and it differs from the
//  game's own: it covers only Fire and Lightning (no cold or poison arm), it does not check the missile's ApplyMastery flag, and it keys on the
//  skill's element while descline 0x16 labels the line from MISSILE_GetElemTypeFromMissileId. The number can therefore disagree with the damage
//  actually dealt. It does not generalise: every line that goes through SKILLS_GetMin/MaxElemDamage with the last argument 1 gets all four
//  masteries from D2Common. Each half is DATATBLS_ApplyRatio(value, pct, 100), which the original inlines here.
//  nScaled first holds what the original's eax holds at that point; nothing reads it.
void __fastcall SKILLDESC_ApplyFireMasteryToRange(D2SkillsTxt *pSkillsTxt, int32_t *pnMin, int32_t *pnMax)
{
    D2UnitStrc *pPlayer;
    int32_t nScaled;
    int32_t nMasteryPct;
    int32_t nMin;
    int32_t nMax;

    pPlayer = CUNIT_GetPlayer();
    nScaled = (int32_t)pPlayer;
    if (pSkillsTxt && pSkillsTxt->nEType == 1)
    {
        nMasteryPct = D2COMMON_STATLIST_UnitGetStatValue(pPlayer, STAT_PASSIVE_FIRE_MASTERY, 0);
        nScaled = nMasteryPct;
        if (nMasteryPct)
        {
            nMin = *pnMin;
            nScaled = DATATBLS_ApplyRatio(nMin, nMasteryPct, 100);
            *pnMin = nScaled + nMin;
            nMax = *pnMax;
            nScaled = DATATBLS_ApplyRatio(nMax, nMasteryPct, 100);
            *pnMax = nScaled + nMax;
        }
    }
}


//1.10f: D2Client.0x6FB13120
//  The Lightning arm: identical in shape to SKILLDESC_ApplyFireMasteryToRange, differing only in the element tested and the stat read. The two
//  are the whole of this pair; there is no cold or poison arm. The original inlines DATATBLS_ApplyRatio here.
//  void: no caller reads what the original leaves in eax.
void __fastcall SKILLDESC_ApplyLightningMasteryToRange(D2SkillsTxt *pSkillsTxt, int32_t *pnMin, int32_t *pnMax)
{
    D2UnitStrc *pPlayer;
    int32_t nScaled;
    int32_t nMasteryPct;
    int32_t nMin;
    int32_t nMax;

    pPlayer = CUNIT_GetPlayer();
    nScaled = (int32_t)pPlayer;
    if (pSkillsTxt && pSkillsTxt->nEType == 2)
    {
        nMasteryPct = D2COMMON_STATLIST_UnitGetStatValue(pPlayer, STAT_PASSIVE_LTNG_MASTERY, 0);
        nScaled = nMasteryPct;
        if (nMasteryPct)
        {
            nMin = *pnMin;
            nScaled = DATATBLS_ApplyRatio(nMin, nMasteryPct, 100);
            *pnMin = nScaled + nMin;
            nMax = *pnMax;
            nScaled = DATATBLS_ApplyRatio(nMax, nMasteryPct, 100);
            *pnMax = nScaled + nMax;
        }
    }
}


//1.10f: D2Client.0x6FB132C0
//  "<whole>.<fraction>", with a leading '+' when bSigned is set and nWhole is not negative. The fractional part is always printed, zero or not;
//  SKILLDESC_FormatFixedPointOrInt is the version that drops it. The decimal point is a literal '.' in the format string, not a tbl lookup, so
//  unlike the ':' and ' ' separators of the Append* family it does not follow the locale (FormatFracRange's '-' is literal for the same reason).
int32_t __fastcall SKILLDESC_FormatFixedPoint(struct Unicode *pwszOut, int32_t nWhole, int32_t nFraction, int32_t bSigned)
{
    char src[32];

    if (bSigned && nWhole >= 0)
    {
        wsprintfA(src, "+%d.%d", nWhole, nFraction);
    }
    else
    {
        wsprintfA(src, "%d.%d", nWhole, nFraction);
    }
    Unicode::toUnicode(pwszOut, src, 32);
    return 1;
}


//1.10f: D2Client.0x6FB13320
//  The forgiving version of the formatter above, with two rules it does not have: a zero fraction is dropped rather than printed, and both parts
//  zero writes nothing and returns 0, which lets a caller use the return value to decide whether the line is worth emitting. A non-zero fraction
//  with a zero whole still prints as "0.<fraction>". The sign keys on the whole part, so a value of -0.5 arriving as (0, 5) would print "+0.5".
//  result is nFraction on the way in and the constant zero on the way out: the zero exit is reached only when the test on nWhole failed and
//  nFraction is not positive either, so the branch has proved the register zero. The original inlines SKILLDESC_FormatInt here.
int32_t __fastcall SKILLDESC_FormatFixedPointOrInt(struct Unicode *pwszOut, int32_t nWhole, int32_t nFraction, int32_t bSigned)
{
    int32_t result;
    char src[32];

    result = nFraction;
    if (nWhole)
    {
        if (!nFraction)
        {
            SKILLDESC_FormatInt(pwszOut, nWhole, bSigned);
            return 1;
        }
    }
    else if (!nFraction)
    {
        return result;
    }
    if (bSigned && nWhole >= 0)
    {
        wsprintfA(src, "+%d.%d", nWhole, nFraction);
    }
    else
    {
        wsprintfA(src, "%d.%d", nWhole, nFraction);
    }

    Unicode::toUnicode(pwszOut, src, 32);
    return 1;
}


//1.10f: D2Client.0x6FB133C0
//  "<min int>.<min frac>-<max int>.<max frac>", or the same with a leading '+'. The halves arrive already split; the caller does the /25 or the
//  /256 itself. The sign test is bSigned alone: unlike the rest of the family there is no nMinInt >= 0 beside it.
//  void: eax is Unicode::toUnicode's own answer, passed straight on, and its one caller ignores it.
void __fastcall SKILLDESC_FormatFracRange(struct Unicode *pwszOut, int32_t nMinInt, int32_t nMinFrac, int32_t nMaxInt, int32_t nMaxFrac, int32_t bSigned)
{
    char src[32];

    if (bSigned)
    {
        wsprintfA(src, "+%d.%d-%d.%d", nMinInt, nMinFrac, nMaxInt, nMaxFrac);
    }
    else
    {
        wsprintfA(src, "%d.%d-%d.%d", nMinInt, nMinFrac, nMaxInt, nMaxFrac);
    }
    Unicode::toUnicode(pwszOut, src, 32);
}


//1.10f: D2Client.0x6FB13410
//  "<tbl><N[.N]> second[s]\n" for a duration given in frames. The divisor is 25, the game's frames per second, and one decimal of the remainder:
//      nWholeSeconds = nFrames / 25;
//      nTenths       = 10 * (nFrames % 25) / 25;
//  so every "Duration:" and "Length:" line of a skill description is a frame count divided here, not a number skills.txt already holds in
//  seconds. Singular/plural is chosen with the test
//      if ( nWholeSeconds != 1 || (nUnitStrIdx = STR_IDX_4267_StrSkill15, nTenths) )
//          nUnitStrIdx = STR_IDX_4268_StrSkill16;
//  so the singular " second" is taken only for a flat "1"; "1.5" takes the plural. Returns 0 without touching the text when the value rounds to
//  zero, which is how a line with no duration disappears entirely. The original inlines SKILLDESC_FormatInt here.
int32_t __fastcall SKILLDESC_AppendFramesAsSeconds(struct Unicode *pwszText, D2C_StringIndices nStrIdx, int32_t nFrames, int32_t bSigned)
{
    Unicode dest[32];
    char src[32];

    D2C_StringIndices wStrIdx;
    D2C_StringIndices nUnitStrIdx;
    int32_t nWholeSeconds;
    int32_t nTenthsScaled;
    int32_t nTenths;
    const Unicode *pwszLabel;
    const Unicode *wszUnit;

    wStrIdx = nStrIdx;
    nWholeSeconds = nFrames / 25;
    nTenthsScaled = 10 * (nFrames % 25);
    nTenths = nTenthsScaled / 25;
    if (!nWholeSeconds && !nTenths)
    {
        return 0;
    }
    if (nWholeSeconds && !nTenths)
    {
        SKILLDESC_FormatInt(dest, nWholeSeconds, bSigned);
    }
    else
    {
        if (bSigned && nWholeSeconds >= 0)
        {
            wsprintfA(src, "+%d.%d", nWholeSeconds, nTenths);
        }
        else
        {
            wsprintfA(src, "%d.%d", nWholeSeconds, nTenths);
        }
        Unicode::toUnicode(dest, src, 32);
    }
    pwszLabel = D2LANG_GetStringFromTblIndex(wStrIdx);
    Unicode::strcat(pwszText, pwszLabel);
    Unicode::strcat(pwszText, dest);
    if (nWholeSeconds != 1 || (nUnitStrIdx = STR_IDX_4267_StrSkill15, nTenths))
    {
        nUnitStrIdx = STR_IDX_4268_StrSkill16;
    }
    wszUnit = D2LANG_GetStringFromTblIndex(nUnitStrIdx);
    Unicode::strcat(pwszText, wszUnit);
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    return 1;
}


//1.10f: D2Client.0x6FB13550
//  "<tbl A><min.frac-max.frac><tbl B>\n" for a damage range held in 8.8 fixed point, with nFracScale (10 or 100) fixing the precision shown:
//      nMinFraction = nFracScale * (nMin % 256) / 256;
//  When both fractions round away the whole numbers go to SKILLDESC_FormatDamageRange instead of being printed as "<min>.0", so a range with no
//  fractional part is formatted the way every other damage range is. Either tbl index may be 5382 ("dummy"), in which case that side is skipped.
//  Returns 0, printing nothing, when the whole range is zero. The early return is `return nMaxFraction`, which the branch has proved zero: it is
//  reached only when both whole parts and both fractions are zero, and the same slot carries the max fraction the rest of the time.
int32_t __fastcall SKILLDESC_AppendScaledDamageRange(struct Unicode *pwszText, D2C_StringIndices nStrIdxA, D2C_StringIndices nStrIdxB, int32_t nMin, int32_t nMax, int32_t nFracScale)
{
    Unicode wszOut[32];
    char src[32];

    D2C_StringIndices wStrIdxA;
    int32_t nMinFraction;
    int32_t nMaxFraction;

    wStrIdxA = nStrIdxA;
    nMinFraction = nFracScale * (nMin % 256) / 256;
    nMaxFraction = nFracScale * (nMax % 256) / 256;
    if (!(nMin / 256) && !(nMax / 256) && !nMinFraction && !nMaxFraction)
    {
        return nMaxFraction;
    }
    if (!nMinFraction && !nMaxFraction && ((nMin / 256) || (nMax / 256)))
    {
        SKILLDESC_FormatDamageRange(wszOut, nMin / 256, nMax / 256, 0, 0);
    }
    else
    {
        wsprintfA(src, "%d.%d-%d.%d", nMin / 256, nMinFraction, nMax / 256, nMaxFraction);
        Unicode::toUnicode(wszOut, src, 32);
    }
    if (wStrIdxA != STR_IDX_5382_dummy)
    {
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wStrIdxA));
    }
    Unicode::strcat(pwszText, wszOut);
    if (nStrIdxB != STR_IDX_5382_dummy)
    {
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nStrIdxB));
    }
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    return 1;
}


//1.10f: D2Client.0x6FB136B0
//  "<tbl nStrIdx>\n". Its one caller, descline 0x33, uses it when the format string is the sentinel 5382 ("dummy") and Unicode::sprintf would
//  have nothing to substitute into.
int32_t __fastcall SKILLDESC_AppendTextLine(struct Unicode *pwszText, D2C_StringIndices nStrIdx)
{
    const Unicode *pwszLabel;
    const Unicode *wszNewline;

    pwszLabel = D2LANG_GetStringFromTblIndex(nStrIdx);
    Unicode::strcat(pwszText, pwszLabel);
    wszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
    Unicode::strcat(pwszText, wszNewline);
    return 1;
}


//1.10f: D2Client.0x6FB136E0
//  "<tbl nStrIdx><value>\n". Returns 0 and prints nothing when nValue is zero, which is how a skill with no bonus in some slot drops the whole
//  line rather than printing "Radius: 0". result is nValue on the way in and the constant zero on the way out, and `return result` is reached
//  only when nValue is zero: the register the branch has just tested, not a variable. AppendValueThenText and AppendIntAndFraction are the same
//  function with the pieces in a different order. The original inlines SKILLDESC_FormatInt here.
int32_t __fastcall SKILLDESC_AppendTextThenValue(struct Unicode *pwszText, D2C_StringIndices nStrIdx, int32_t nValue, int32_t bSigned)
{
    Unicode dest[32];

    D2C_StringIndices wStrIdx;
    int32_t result;
    const Unicode *pwszLabel;
    const Unicode *wszNewline;

    wStrIdx = nStrIdx;
    result = nValue;
    if (nValue)
    {
        SKILLDESC_FormatInt(dest, nValue, bSigned);
        pwszLabel = D2LANG_GetStringFromTblIndex(wStrIdx);
        Unicode::strcat(pwszText, pwszLabel);
        Unicode::strcat(pwszText, dest);
        wszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
        Unicode::strcat(pwszText, wszNewline);
        return 1;
    }
    return result;
}


//1.10f: D2Client.0x6FB13790
//  "<tbl A><N[.N]><tbl B>\n" for a value in 8.8 fixed point, nFracScale deciding the fractional precision:
//      nFraction = nFracScale * (nValue % 256) / 256;
//      nWhole    = nValue / 256;
//  The four wsprintfA arms are the cross product of has-a-fraction and signed-and-not-negative. A value that is non-zero but rounds away prints
//  the labels only: with nWhole and nFraction both zero both formatting blocks are skipped and the empty dest is concatenated. Returns 0, before
//  any of that, only when nValue itself is zero. Either index may be 5382 and is then skipped; the first test compares wStrIdxA, the
//  sign-truncated copy made once at the top, the second tests (_WORD)nStrIdxB inline. The original inlines SKILLDESC_FormatInt here.
int32_t __fastcall SKILLDESC_AppendFixedPointValue(struct Unicode *pwszText, D2C_StringIndices nStrIdxA, D2C_StringIndices nStrIdxB, int32_t nValue, int32_t nFracScale, int32_t bSigned)
{
    Unicode dest[32];
    char src[32];

    D2C_StringIndices wStrIdxA;
    int32_t nFraction;
    int32_t nWhole;
    const Unicode *pwszSuffix;

    wStrIdxA = nStrIdxA;
    if (!nValue)
    {
        return 0;
    }
    nFraction = nFracScale * (nValue % 256) / 256;
    nWhole = nValue / 256;
    if (nWhole || nFraction)
    {
        if (nFraction)
        {
            if (bSigned && nWhole >= 0)
            {
                wsprintfA(src, "+%d.%d", nWhole, nFraction);
            }
            else
            {
                wsprintfA(src, "%d.%d", nWhole, nFraction);
            }
            Unicode::toUnicode(dest, src, 32);
        }
        else
        {
            SKILLDESC_FormatInt(dest, nWhole, bSigned);
        }
    }
    if (wStrIdxA != STR_IDX_5382_dummy)
    {
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wStrIdxA));
    }
    Unicode::strcat(pwszText, dest);
    if (nStrIdxB != STR_IDX_5382_dummy)
    {
        pwszSuffix = D2LANG_GetStringFromTblIndex(nStrIdxB);
        Unicode::strcat(pwszText, pwszSuffix);
    }
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    return 1;
}


//1.10f: D2Client.0x6FB138E0
//  "<tbl nStrIdx><nInt[.nFrac]>\n", dropping the ".nFrac" when the fraction is zero. Returns 0 only when the integer part is zero and the
//  fraction is not positive, so a negative fraction with a zero integer part prints nothing either. Its one caller is descline 1, the mana cost
//  line, when the cost is small enough to be worth a decimal (mana is 8.8 fixed point there, and the caller does the split). The original
//  inlines SKILLDESC_FormatInt here.
int32_t __fastcall SKILLDESC_AppendIntAndFraction(struct Unicode *pwszText, D2C_StringIndices nStrIdx, int32_t nInt, int32_t nFrac, int32_t bSigned)
{
    Unicode dest[32];
    char src[32];

    D2C_StringIndices wStrIdx;
    int32_t result;
    const Unicode *pwszLabel;
    const Unicode *wszNewline;

    wStrIdx = nStrIdx;
    result = nInt;
    if (!nInt && !(nFrac > 0))
    {
        return result;
    }
    if (nInt && !nFrac)
    {
        SKILLDESC_FormatInt(dest, nInt, bSigned);
    }
    else
    {
        if (bSigned && nInt >= 0)
        {
            wsprintfA(src, "+%d.%d", nInt, nFrac);
        }
        else
        {
            wsprintfA(src, "%d.%d", nInt, nFrac);
        }
        Unicode::toUnicode(dest, src, 32);
    }
    pwszLabel = D2LANG_GetStringFromTblIndex(wStrIdx);
    Unicode::strcat(pwszText, pwszLabel);
    Unicode::strcat(pwszText, dest);
    wszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
    Unicode::strcat(pwszText, wszNewline);
    return 1;
}


//1.10f: D2Client.0x6FB139E0
//  "<value><tbl nStrIdx>\n", the mirror of SKILLDESC_AppendTextThenValue, for lines that read "25 percent damage returned" rather than
//  "Radius: 25". Returns 0 when nValue is zero. The order of the two strcat calls is the only thing separating it from its mirror: the bare
//  Unicode::strcat of the formatted number comes before the tbl lookup here and after it there. Its one caller is descline 0x2D, tbl 4309
//  " percent damage returned". The original inlines SKILLDESC_FormatInt here.
int32_t __fastcall SKILLDESC_AppendValueThenText(struct Unicode *pwszText, D2C_StringIndices nStrIdx, int32_t nValue, int32_t bSigned)
{
    Unicode dest[32];

    D2C_StringIndices wStrIdx;
    int32_t result;
    const Unicode *pwszLabel;
    const Unicode *wszNewline;

    wStrIdx = nStrIdx;
    result = nValue;
    if (nValue)
    {
        SKILLDESC_FormatInt(dest, nValue, bSigned);
        Unicode::strcat(pwszText, dest);
        pwszLabel = D2LANG_GetStringFromTblIndex(wStrIdx);
        Unicode::strcat(pwszText, pwszLabel);
        wszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
        Unicode::strcat(pwszText, wszNewline);
        return 1;
    }
    return result;
}


//1.10f: D2Client.0x6FB13A90
//  "<value><tbl A><tbl B>\n". Returns 0 when nValue is zero. Its one caller, descline 0x44, wraps the value in two strings the way a range's two
//  labels do, but as a plain concatenation, with no tbl index tested against 5382 and no SKILLDESC_FormatDamageRange. wStrIdxA is the first
//  index narrowed to 16 bits once at the top; the second index is cast at its own call. The original inlines SKILLDESC_FormatInt here.
int32_t __fastcall SKILLDESC_AppendValueThenTextPair(struct Unicode *pwszText, D2C_StringIndices nStrIdxA, D2C_StringIndices nStrIdxB, int32_t nValue, int32_t bSigned)
{
    Unicode dest[32];

    D2C_StringIndices wStrIdxA;
    int32_t result;
    const Unicode *pwszTextA;
    const Unicode *pwszTextB;
    const Unicode *pwszNewline;

    wStrIdxA = nStrIdxA;
    result = nValue;
    if (nValue)
    {
        SKILLDESC_FormatInt(dest, nValue, bSigned);
        Unicode::strcat(pwszText, dest);
        pwszTextA = D2LANG_GetStringFromTblIndex(wStrIdxA);
        Unicode::strcat(pwszText, pwszTextA);
        pwszTextB = D2LANG_GetStringFromTblIndex(nStrIdxB);
        Unicode::strcat(pwszText, pwszTextB);
        pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
        Unicode::strcat(pwszText, pwszNewline);
        return 1;
    }
    return result;
}


//1.10f: D2Client.0x6FB13B50
//  "<tbl A><value><tbl B>\n", the same three pieces as SKILLDESC_AppendValueThenTextPair in a different order. Returns 0 when nValue is zero.
//  Descline 8 passes 4262 "To Attack Rating: " and 4275 " percent" around the value SKILLS_GetToHitFactor returns. The original inlines
//  SKILLDESC_FormatInt here.
int32_t __fastcall SKILLDESC_AppendValueBetweenTexts(struct Unicode *pwszText, D2C_StringIndices nStrIdxA, D2C_StringIndices nStrIdxB, int32_t nValue, int32_t bSigned)
{
    Unicode dest[32];

    D2C_StringIndices wStrIdxA;
    int32_t result;
    const Unicode *pwszTextA;
    const Unicode *pwszTextB;
    const Unicode *pwszNewline;

    wStrIdxA = nStrIdxA;
    result = nValue;
    if (nValue)
    {
        SKILLDESC_FormatInt(dest, nValue, bSigned);
        pwszTextA = D2LANG_GetStringFromTblIndex(wStrIdxA);
        Unicode::strcat(pwszText, pwszTextA);
        Unicode::strcat(pwszText, dest);
        pwszTextB = D2LANG_GetStringFromTblIndex(nStrIdxB);
        Unicode::strcat(pwszText, pwszTextB);
        pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
        Unicode::strcat(pwszText, pwszNewline);
        return 1;
    }
    return result;
}


//1.10f: D2Client.0x6FB13C10
//  "<tbl A><value><tbl B>\n" when nMin == nMax, but "<tbl A><tbl B><min-max>\n" for a range: that arm appends B before the formatted pair
//  (0x6FB13CF4, then 0x6FB13D06). Returns 0 when the collapsed value is zero; a genuine range is always printed. The two arms format through
//  different helpers: a collapsed value goes through wsprintfA("%d") with no sign, a range through SKILLDESC_FormatDamageRange with bSigned,
//  so a one-value line loses the '+' that the same numbers keep as a range. Its two callers, desclines 0x3A and 0x3E, differ only in bSigned.
//  DIFFERENCE: both buffers are declared here and both are constructed at entry; the original constructs only the one its arm uses, and the
//  zeros land in the same dead stack.
//  DIFFERENCE: the original loads the Unicode::strcat address into a register once and calls through it (call edi); ours calls it directly.
int32_t __fastcall SKILLDESC_AppendRangeBetweenTexts(struct Unicode *pwszText, D2C_StringIndices nStrIdxA, D2C_StringIndices nStrIdxB, int32_t nMin, int32_t nMax, int32_t bSigned)
{
    Unicode dest[32];
    Unicode wszOut[32];
    char src[32];

    D2C_StringIndices wStrIdxA;
    const Unicode *pwszValue;

    wStrIdxA = nStrIdxA;
    if (nMin == nMax)
    {
        if (!nMin)
        {
            return 0;
        }
        wsprintfA(src, "%d", nMin);
        Unicode::toUnicode(dest, src, 32);
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wStrIdxA));
        Unicode::strcat(pwszText, dest);
        pwszValue = D2LANG_GetStringFromTblIndex(nStrIdxB);
    }
    else
    {
        SKILLDESC_FormatDamageRange(wszOut, nMin, nMax, bSigned, 0);
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wStrIdxA));
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nStrIdxB));
        pwszValue = wszOut;
    }
    Unicode::strcat(pwszText, pwszValue);
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    return 1;
}


//1.10f: D2Client.0x6FB13D30
//  "<tbl nStrIdx><min-max>[ ]\n", the classic-only physical damage line. Returns 0 when nMin == nMax == 0. Its one caller, descline 0x30, uses it
//  in the non-expansion arm; in an expansion game that descline calls four separate builders instead. Same two arms as
//  SKILLDESC_AppendRangeBetweenTexts, with two differences: the collapsed value keeps the '+' when bSigned allows it, and the range arm can
//  append tbl 3995, a single space, before the value, which is what bTrailingSpace is for; the equal arm ignores that flag. The tbl index is
//  spilled once at 0x6FB13D4A and read back at 0x6FB13DC0.
//  DIFFERENCE: the original loads the Unicode::strcat address into a register once and calls through it (call edi); ours calls it directly.
//  The original inlines SKILLDESC_FormatInt here.
int32_t __fastcall SKILLDESC_AppendDamageRangeLine(struct Unicode *pwszText, D2C_StringIndices nStrIdx, int32_t nMin, int32_t nMax, int32_t bSigned, int32_t bTrailingSpace)
{
    Unicode dest[32];
    Unicode wszOut[32];

    D2C_StringIndices wLabelStrIdx;
    const Unicode *pwszValue;
    const Unicode *wszSpace;

    wLabelStrIdx = nStrIdx;
    if (nMin == nMax)
    {
        if (!nMin)
        {
            return 0;
        }
        SKILLDESC_FormatInt(dest, nMin, bSigned);
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wLabelStrIdx));
        pwszValue = dest;
    }
    else
    {
        SKILLDESC_FormatDamageRange(wszOut, nMin, nMax, bSigned, 0);
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wLabelStrIdx));
        if (bTrailingSpace)
        {
            wszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
            Unicode::strcat(pwszText, wszSpace);
        }
        pwszValue = wszOut;
    }
    Unicode::strcat(pwszText, pwszValue);
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    return 1;
}


static_assert(STR_IDX_21248_ModStre10a == 0x5300 && STR_IDX_21252_ModStre10e == 0x5304
              && STR_IDX_21253_ModStre10f == 0x5305,
              "the charges line: mov ecx, 5300h / 5304h / 5305h (0x6FB13E79..0x6FB13E93)");


//1.10f: D2Client.0x6FB13E70
//  "(N Charges)" and a newline, appended to wszOut: tbl 21252 '(', the number, tbl 3995 ' ', tbl 21248 "Charges", tbl 21253 ')' and tbl 3998.
//  Its one call is SKILLDESC_BuildDescLine's descline 1. The four strings are fetched first, in the order 21248, 21252, 21253, 3995
//  (0x6FB13E79..0x6FB13EAA), and the newline only after the fifth strcat (0x6FB13F1C); a fetch is a call into D2Lang, so the order is kept.
//  The number is "%d" through wsprintfA into 32 bytes, widened with a count of 32 into a 16-Unicode local.
//  BUG: the widening count is twice the buffer. LATENT: "%d" makes at most eleven characters and a terminator.
//  Returns 1 (mov eax, 1, 0x6FB13F2C); the one caller discards it.
int32_t __fastcall SKILLDESC_FormatChargesText(struct Unicode *wszOut, int32_t nCharges)
{
    const Unicode *pwszCharges = D2LANG_GetStringFromTblIndex(STR_IDX_21248_ModStre10a);
    const Unicode *pwszOpenParen = D2LANG_GetStringFromTblIndex(STR_IDX_21252_ModStre10e);
    const Unicode *pwszCloseParen = D2LANG_GetStringFromTblIndex(STR_IDX_21253_ModStre10f);
    const Unicode *pwszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);

    Unicode wszNumber[16];
    char szNumber[32];

    wsprintfA(szNumber, "%d", nCharges);
    Unicode::toUnicode(wszNumber, szNumber, 32);
    Unicode::strcat(wszOut, pwszOpenParen);
    Unicode::strcat(wszOut, wszNumber);
    Unicode::strcat(wszOut, pwszSpace);
    Unicode::strcat(wszOut, pwszCharges);
    Unicode::strcat(wszOut, pwszCloseParen);
    Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    return 1;
}


//1.10f: D2Client.0x6FB13F40
//  "Cold Length: N seconds" / "Poison Length: N seconds": tbl 4265 for element 4 (Cold), tbl 4266 for element 5 (Poison), the frame count from
//  SKILLS_GetElementalLength divided by 25 inside SKILLDESC_AppendFramesAsSeconds. Returns 0 for every other element, which is why no other
//  element shows a duration. One of the four builders descline 0x30 calls in an expansion game (the classic arm takes a different path); it is
//  the same logic descline 0x0B has inline. It passes a zero bApplyMastery to the length getter, which ignores the flag.
int32_t __fastcall SKILLDESC_AppendElemLengthLine(D2UnitStrc *pUnit, struct Unicode *pwszText, D2C_Skills nSkillId, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel)
{
    int32_t nElemLength;

    nElemLength = D2COMMON_SKILLS_GetElementalLength(pUnit, nSkillId, nSkillLevel, 0);
    if (nElemLength)
    {
        if (pSkillsTxt->nEType == 4)
        {
            return SKILLDESC_AppendFramesAsSeconds(pwszText, STR_IDX_4265_StrSkill13, nElemLength, 0);
        }
        if (pSkillsTxt->nEType == 5)
        {
            return SKILLDESC_AppendFramesAsSeconds(pwszText, STR_IDX_4266_StrSkill14, nElemLength, 0);
        }
    }
    return 0;
}


//1.10f: D2Client.0x6FB13FA0
//  The elemental damage line. nStrIdxOverride, when non-zero, is the label; when it is zero the label comes from nEType, in the stock
//  ElemTypes.txt row order:
//      1 Fire       -> 4257 StrSkill5  'Fire Damage: '
//      2 Lightning  -> 4259 StrSkill7  'Lightning Damage: '
//      3 Magic      -> 4291 StrSkill39 'Magic Damage: '
//      4 Cold       -> 4258 StrSkill6  'Cold Damage: '
//      5 Poison     -> 4260 StrSkill8  'Poison Damage: '
//      6 Life Drain -> 3524 ModStr2z   'Life stolen per hit'
//  Anything else, including 0 ("None"), returns 0 and prints nothing; the same six-way mapping is written inline on desclines 0x16 and 0x1A.
//  The collapse test is not nMin == nMax: it is `nMin ? (nMin == nMax) : (nMax == 0)`, so a line with a zero min and a non-zero max prints as a
//  range, while 0-0 collapses to a single 0. result is the literal zero of the switch's default arm: the return says "was this an element type
//  I know".
int32_t __fastcall SKILLDESC_AppendElemDamageLine(struct Unicode *pwszText, int32_t nMin, int32_t nMax, int32_t nEType, uint16_t nStrIdxOverride)
{
    Unicode wszOut[32];
    char src[32];

    D2C_StringIndices nStrIdx;
    int32_t bSingleValue;
    int32_t result;
    const Unicode *pwszLabel;

    nStrIdx = (D2C_StringIndices)nStrIdxOverride;
    if (!nStrIdxOverride)
    {
        switch (nEType)
        {
        case 1: nStrIdx = STR_IDX_4257_StrSkill5;  break;
        case 2: nStrIdx = STR_IDX_4259_StrSkill7;  break;
        case 3: nStrIdx = STR_IDX_4291_StrSkill39; break;
        case 4: nStrIdx = STR_IDX_4258_StrSkill6;  break;
        case 5: nStrIdx = STR_IDX_4260_StrSkill8;  break;
        case 6: nStrIdx = STR_IDX_3524_ModStr2z;   break;
        default:
            result = 0;
            return result;
        }
    }
    if (nMin)
    {
        bSingleValue = (nMin == nMax);
    }
    else
    {
        bSingleValue = (nMax == 0);
    }
    if (bSingleValue)
    {
        wsprintfA(src, "%d", nMin);
        Unicode::toUnicode(wszOut, src, 32);
    }
    else
    {
        SKILLDESC_FormatDamageRange(wszOut, nMin, nMax, 0, 0);
    }
    pwszLabel = D2LANG_GetStringFromTblIndex(nStrIdx);
    Unicode::strcat(pwszText, pwszLabel);
    Unicode::strcat(pwszText, wszOut);
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    return 1;
}


//1.10f: D2Client.0x6FB140B0
//  The same elemental damage line, computed from the skill rather than from a pair already in hand; the twin of
//  SKILLDESC_AppendElemDamageLine, with the same six-way element-to-label mapping, the same override escape and the same collapse rule. It
//  asks D2Common for damage with mastery: both getters take the last argument 1, the flag that makes D2Common add
//  SKILLS_CalculateMasteryBonus, so this line already covers Fire, Lightning, Cold (and Freeze) and Poison mastery without the client doing
//  anything, unlike the missile path where the mastery helpers are folded in by hand (descline 0x16, SKILLDESC_ApplyFireMasteryToRange).
int32_t __fastcall SKILLDESC_AppendSkillElemDamageLine(D2UnitStrc *pUnit, struct Unicode *pwszText, D2C_Skills nSkillId, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, uint16_t nStrIdxOverride)
{
    Unicode wszOut[32];
    char src[32];

    D2C_StringIndices wStrIdx;
    int32_t nEType;
    int32_t nMin;
    int32_t nMax;
    int32_t bNoDamage;
    const Unicode *pwszLabel;

    nMin = D2COMMON_SKILLS_GetMinElemDamage(pUnit, nSkillId, nSkillLevel, 1);
    nMax = D2COMMON_SKILLS_GetMaxElemDamage(pUnit, nSkillId, nSkillLevel, 1);
    if (!nMin && !nMax)
    {
        return 0;
    }
    nMin >>= 8;
    nMax >>= 8;
    wStrIdx = (D2C_StringIndices)nStrIdxOverride;
    nEType = pSkillsTxt->nEType;
    if (!nStrIdxOverride)
    {
        switch (nEType)
        {
        case 1: wStrIdx = STR_IDX_4257_StrSkill5;  break;
        case 2: wStrIdx = STR_IDX_4259_StrSkill7;  break;
        case 3: wStrIdx = STR_IDX_4291_StrSkill39; break;
        case 4: wStrIdx = STR_IDX_4258_StrSkill6;  break;
        case 5: wStrIdx = STR_IDX_4260_StrSkill8;  break;
        case 6: wStrIdx = STR_IDX_3524_ModStr2z;   break;
        default:
            return 0;
        }
    }
    if (nMin)
    {
        bNoDamage = (nMin == nMax);
    }
    else
    {
        bNoDamage = (nMax == 0);
    }
    if (bNoDamage)
    {
        wsprintfA(src, "%d", nMin);
        Unicode::toUnicode(wszOut, src, 32);
    }
    else
    {
        SKILLDESC_FormatDamageRange(wszOut, nMin, nMax, 0, 0);
    }
    pwszLabel = D2LANG_GetStringFromTblIndex(wStrIdx);
    Unicode::strcat(pwszText, pwszLabel);
    Unicode::strcat(pwszText, wszOut);
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    return 1;
}


//1.10f: D2Client.0x6FB14240
//  "Damage: <min-max>\n" (tbl 4256 StrSkill4) from SKILLS_GetMinPhysDamage / SKILLS_GetMaxPhysDamage, each >> 8, then scaled by nPercentBonus
//  percent and offset by nFlatBonus. Returns 0 when both ends come out zero. The original inlines DATATBLS_ApplyRatio(value, nPercentBonus, 100)
//  and SKILLDESC_FormatInt here; two of the helper's arms divide before multiplying and therefore truncate, so it is not a 64-bit multiply in
//  disguise and must not be collapsed into one. pSkillsTxt is accepted and never read; the record is reached through nSkillId instead.
//  Sign quirk: a collapsed single value prints "+%d" for anything non-negative and "%d" only when negative, so "Damage: +25" is normal here
//  while the range form has no sign at all. Descline 9 passes nCalcA and nCalcB; descline 0x30's expansion arm passes 0 and 0.
int32_t __fastcall SKILLDESC_AppendPhysDamageLine(D2UnitStrc *pUnit, struct Unicode *pwszText, D2C_Skills nSkillId, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nPercentBonus, int32_t nFlatBonus)
{
    Unicode wszOut[32];

    int32_t nMin;
    int32_t nMax;

    nMin = D2COMMON_SKILLS_GetMinPhysDamage(pUnit, nSkillId, nSkillLevel, 0) >> 8;
    nMax = D2COMMON_SKILLS_GetMaxPhysDamage(pUnit, nSkillId, nSkillLevel, 0) >> 8;
    nMin = nMin + DATATBLS_ApplyRatio(nMin, nPercentBonus, 100) + nFlatBonus;
    nMax = nMax + DATATBLS_ApplyRatio(nMax, nPercentBonus, 100) + nFlatBonus;
    if (!nMin && !nMax)
    {
        return 0;
    }
    if (nMin && nMin == nMax)
    {
        SKILLDESC_FormatInt(wszOut, nMin, 1);
    }
    else
    {
        SKILLDESC_FormatDamageRange(wszOut, nMin, nMax, 0, 0);
    }
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_4256_StrSkill4));
    Unicode::strcat(pwszText, wszOut);
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    return 1;
}


//1.10f: D2Client.0x6FB14470
//  "To Attack Rating: +N percent": tbl 4262 StrSkill10 and 4275 StrSkill23 around SKILLS_GetToHitFactor. Returns 0 and prints nothing when the
//  factor is zero. pSkillsTxt is accepted and never read; the record is reached through nSkillId. The same sign quirk as the other single-value
//  lines: "+%d" for anything non-negative, "%d" only when negative, so the '+' is not evidence of a bonus. One of the four builders descline 0x30
//  calls in an expansion game; descline 8 reaches the same text through SKILLDESC_AppendValueBetweenTexts with the same two tbl indices.
//  result and nToHitFactor are one value split across eax and ebp: the first is the call's answer, the second the copy that is printed, and
//  `return result` is the zero-factor path. The original inlines SKILLDESC_FormatInt here.
int32_t __fastcall SKILLDESC_AppendToHitLine(D2UnitStrc *pUnit, struct Unicode *pwszText, D2C_Skills nSkillId, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel)
{
    Unicode dest[32];

    int32_t result;
    int32_t nToHitFactor;
    const Unicode *pwszLabel;

    result = D2COMMON_SKILLS_GetToHitFactor(pUnit, nSkillId, nSkillLevel);
    nToHitFactor = result;
    if (!result)
    {
        return result;
    }
    SKILLDESC_FormatInt(dest, nToHitFactor, 1);
    pwszLabel = D2LANG_GetStringFromTblIndex(STR_IDX_4262_StrSkill10);
    Unicode::strcat(pwszText, pwszLabel);
    Unicode::strcat(pwszText, dest);
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_4275_StrSkill23));
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    return 1;
}


//1.10f: D2Client.0x6FB14530
//  "Average <tbl nStrIdx><min-max> per second": tbl 4322 StrSkill68 in front and 4286 StrSkill34 behind, collapsing to a single number when
//  nMin == nMax. Returns 0 only when both ends are zero. Descline 0x16, the missile damage-per-second line, is one of its callers; it is what
//  the pair of mastery helpers feeds. The middle Unicode::strcat with no tbl lookup in front of it is the formatted number, so the sentence is
//  'Average ', the caller's label, the number, ' per second'. result starts as nMax, which makes `return result` the both-ends-zero exit; every
//  path that prints returns a literal 1.
int32_t __fastcall SKILLDESC_AppendAveragePerSecondLine(struct Unicode *pwszText, int32_t nMin, int32_t nMax, D2C_StringIndices nStrIdx)
{
    Unicode wszOut[32];
    char src[32];

    int32_t result;

    result = nMax;
    if (nMin)
    {
        if (nMin == nMax)
        {
            wsprintfA(src, "%d", nMin);
            Unicode::toUnicode(wszOut, src, 32);
        }
        else
        {
            SKILLDESC_FormatDamageRange(wszOut, nMin, nMax, 0, 0);
        }
    }
    else
    {
        if (!nMax)
        {
            return result;
        }
        SKILLDESC_FormatDamageRange(wszOut, nMin, nMax, 0, 0);
    }
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_4322_StrSkill68));
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nStrIdx));
    Unicode::strcat(pwszText, wszOut);
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_4286_StrSkill34));
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    return 1;
}


//1.10f: D2Client.0x6FB14600
//  One description line: `<label>: <value>[%] <suffix>\n`. The whole `<label>: ` block is skipped when nStrIdxLabel is 5382 ("dummy"), the
//  sentinel skilldesc.txt leaves in a desctexta/desctextb cell that was not filled in; every function in this family guards on it, and
//  SKILLDESC_AppendTextPairLine is the variant that bails out entirely. Returns 0 and writes nothing when nValue is zero, so an empty modifier
//  does not leave a blank line in the tooltip. bSigned prints "+%d" for anything non-negative and "%d" for a negative, the family's usual
//  convention, so a leading '+' is not evidence of a bonus. This and SKILLDESC_AppendTextThenValueLine are mirror images with the same six
//  arguments: here the number precedes the caller's text, there the text precedes the number. The strcat order is: label, 3997, 3995, the number,
//  [4001 percent,] 3995, the suffix, 3998. The original inlines SKILLDESC_FormatInt here.
int32_t __fastcall SKILLDESC_AppendValueThenTextLine(struct Unicode *pwszText, D2C_StringIndices nStrIdxLabel, D2C_StringIndices nStrIdxSuffix, int32_t nValue, int32_t bSigned, int32_t bPercent)
{
    Unicode dest[32];

    D2C_StringIndices wLabelStrIdx;

    wLabelStrIdx = nStrIdxLabel;
    if (!nValue)
    {
        return 0;
    }
    SKILLDESC_FormatInt(dest, nValue, bSigned);
    if (wLabelStrIdx != STR_IDX_5382_dummy)
    {
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wLabelStrIdx));
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3997_colon));
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
    }
    Unicode::strcat(pwszText, dest);
    if (bPercent)
    {
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_4001_percent));
    }
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nStrIdxSuffix));
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    return 1;
}


//1.10f: D2Client.0x6FB14710
//  One description line of the form `<label>: <value>/<max> <suffix>\n`. Nothing is written when nStrIdxLabel is the 5382 "dummy" sentinel (the
//  parameter is int16_t, the truncation the sentinel test is made in), when nValue is zero, or when nMax is not positive. The sign convention is
//  the family's: "+%d" only when bSigned is set and nValue is non-negative, and only on the first of the two numbers; the second always uses the
//  plain form. Both numbers are printed, from the two separate stack arguments. The original inlines SKILLDESC_FormatInt here.
int32_t __fastcall SKILLDESC_AppendValueOverValueLine(struct Unicode *pwszText, D2C_StringIndices nStrIdxLabel, D2C_StringIndices nStrIdxSuffix, int32_t nValue, int32_t nMax, int32_t bSigned)
{
    Unicode dest[32];
    char src[32];

    if (nStrIdxLabel == STR_IDX_5382_dummy || !nValue || nMax <= 0)
    {
        return 0;
    }
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nStrIdxLabel));
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3997_colon));
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
    SKILLDESC_FormatInt(dest, nValue, bSigned);
    Unicode::strcat(pwszText, dest);
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_4000_slash));
    wsprintfA(src, "%d", nMax);
    Unicode::toUnicode(dest, src, 32);
    Unicode::strcat(pwszText, dest);
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nStrIdxSuffix));
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    return 1;
}


//1.10f: D2Client.0x6FB14860
//  The simplest line in the family: `<label>: <text>\n`, two tbl indices and nothing else. Returns 0 and writes nothing at all when either index
//  is the 5382 "dummy" sentinel (unlike SKILLDESC_AppendValueThenTextLine, which merely omits the label). The 32-Unicode buffer is constructed
//  (0x6FB1487F..0x6FB14894) and never written or read again; the source declared one it did not use, and it is declared, and therefore
//  constructed, as found.
int32_t __fastcall SKILLDESC_AppendTextPairLine(struct Unicode *pwszText, D2C_StringIndices nStrIdxLabel, D2C_StringIndices nStrIdxText)
{
    Unicode dest[32];

    D2C_StringIndices wLabelStrIdx;

    wLabelStrIdx = nStrIdxLabel;
    if (nStrIdxLabel == STR_IDX_5382_dummy || nStrIdxText == STR_IDX_5382_dummy)
    {
        return 0;
    }
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wLabelStrIdx));
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3997_colon));
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nStrIdxText));
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    return 1;
}


//1.10f: D2Client.0x6FB14910
//  `<label>: <text> <value>[%]\n`: the same six arguments as SKILLDESC_AppendValueThenTextLine with the text and the number swapped, and the
//  guards in the other order: this one tests the label first and the value second, and both return 0. The original inlines
//  SKILLDESC_FormatInt here.
int32_t __fastcall SKILLDESC_AppendTextThenValueLine(struct Unicode *pwszText, D2C_StringIndices nStrIdxLabel, D2C_StringIndices nStrIdxText, int32_t nValue, int32_t bSigned, int32_t bPercent)
{
    Unicode dest[32];

    D2C_StringIndices wLabelStrIdx;

    wLabelStrIdx = nStrIdxLabel;
    if (nStrIdxLabel == STR_IDX_5382_dummy)
    {
        return 0;
    }
    if (!nValue)
    {
        return 0;
    }
    SKILLDESC_FormatInt(dest, nValue, bSigned);
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(wLabelStrIdx));
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3997_colon));
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(nStrIdxText));
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
    Unicode::strcat(pwszText, dest);
    if (bPercent)
    {
        Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_4001_percent));
    }
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    return 1;
}


//1.10f: D2Client.0x6FB14A30
//  The one member of the family that lets the tbl string be the format: it fetches nStrIdxFormat and hands it to Unicode::sprintf with a
//  256-wide stack buffer and a single integer argument, then appends a newline. Guards on the 5382 "dummy" sentinel like the others.
//  D2Lang's Unicode::sprintf consumes an argument for %%, so a tbl format containing %% would read past the single argument. The call is
//  bounded: the buffer is 256 Unicode and the length is passed.
int32_t __fastcall SKILLDESC_AppendSprintfLine(struct Unicode *pwszText, D2C_StringIndices nStrIdxFormat, int32_t nValue)
{
    Unicode src[256];

    D2C_StringIndices nStrIdx;

    nStrIdx = nStrIdxFormat;
    if (nStrIdxFormat == STR_IDX_5382_dummy)
    {
        return 0;
    }
    Unicode::sprintf(256, src, D2LANG_GetStringFromTblIndex(nStrIdx), nValue);
    Unicode::strcat(pwszText, src);
    Unicode::strcat(pwszText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    return 1;
}


//1.10f: D2Client.0x6FB14AC0
//  Returns nMonsterId when it is a valid row of monstats.txt and -1 otherwise. The upper bound is nMonStatsTxtRecordCount and the lower bound
//  is MONSTER_SKELETON1 (0, the first row); the lower-bound test is signed (test eax, eax / jl at 0x6FB14AC2). Its one caller, descline 0x29,
//  hands it the constant MONSTER_FIREGOLEM (mov ecx, 124h at 0x6FB12328) before computing that monster's stats; descline 0x27 does not call it.
int32_t __fastcall SKILLDESC_ValidateMonsterId(D2C_MonsterIds nMonsterId)
{
    int32_t result;

    result = nMonsterId;
    if ((int32_t)nMonsterId < (int32_t)MONSTER_SKELETON1 || (int32_t)nMonsterId >= sgptDataTables->nMonStatsTxtRecordCount)
    {
        return -1;
    }
    return result;
}


//1.10f: D2Client.0x6FB14B10
//  Draws the two-line framed hint in the bottom right corner: tbl 4184 'Quantity : %d' with nQuantity, a newline, then whatever tbl string the
//  caller names, so the second line is the caller's own label and this function owns only the quantity line and the position. The position is
//  measured from the corner (gnScreenWidth - 128, gnScreenHeight - 52), so it follows the resolution. The fifth argument to
//  D2Win_10129_DrawFramedText, nAlign, is 1 where the level builder passes 0; the colour is 0 in both. The original has descline 0x35 draw
//  the same hint with this body inlined (0x6FB128BC..0x6FB1292D) while 0x36..0x38 call it.
void __fastcall SKILLDESC_DrawQuantityHint(int32_t nQuantity, D2C_StringIndices nStringId)
{
    Unicode wszText[100];

    D2C_StringIndices nStrIdx;

    nStrIdx = nStringId;
    Unicode::sprintf(100, wszText, D2LANG_GetStringFromTblIndex(STR_IDX_4184_scrollbooktext), nQuantity);
    Unicode::strcat(wszText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    Unicode::strcat(wszText, D2LANG_GetStringFromTblIndex(nStrIdx));
    D2WIN_10129_DrawFramedText(wszText, gnScreenWidth - 128, gnScreenHeight - 52, 0, 1);
}


//1.10f: D2Client.0x6FB14BB0
//  `dwParam[0] + (nSkillLevel - 1) * dwParam[1]`: the base-plus-per-level value skills.txt keeps in Param1 and Param2, evaluated for one level.
//  Level 1 gives the base alone, and a level of zero or less returns 0 rather than extrapolating backwards. Returns 0 for a negative or
//  out-of-range id and for a NULL record too, so a caller cannot tell "no such skill" from "the value is zero".
int32_t __fastcall SKILLDESC_GetLinearParamByLevel(int32_t nSkillId, int32_t nSkillLevel)
{
    D2SkillsTxt *pSkillsTxt;

    if (nSkillId < 0)
    {
        return 0;
    }
    if (nSkillId >= sgptDataTables->nSkillsTxtRecordCount)
    {
        return 0;
    }
    pSkillsTxt = &sgptDataTables->pSkillsTxt[nSkillId];
    if (!pSkillsTxt)
    {
        return 0;
    }
    if (nSkillLevel > 0)
    {
        return pSkillsTxt->dwParam[0] + (nSkillLevel - 1) * pSkillsTxt->dwParam[1];
    }
    return 0;
}


//1.10f: D2Client.0x6FB14C00
//  The current/next level block of a skill tooltip: three passes over the dispatcher's line ranges at the next level (nNextLevel =
//  nSkillLevel + 1: dsc3, descline, dsc2), one more over descline at nSkillLevel itself with the "Current Skill Level: N" label (tbl 4254), and
//  the skill's name, coloured with code 2. The tooltip describes two levels: the fourth pass is labelled with tbl 4253 "Next Level", or tbl
//  4269 "First Level" when the skill is still at zero, and each label goes after its own lines, not before them. Two strings mark a skill whose
//  every level comes from an item: tbl 4370 " (item)" goes after the current-level number and tbl 4371 "You have been granted this skill by an
//  item" after the name. Drawn by D2Win_10129_DrawFramedText to the left of the icon (nIconLeft - 24), unlike its requirement twin, which
//  passes the x straight through. Its only caller is SKILLSTREE_DrawSkillIcon, which passes nCharges = 0 always.
//  The separator rule: four flags name the dispatcher's line ranges (16..10 dsc3line1..7: bAnyDsc3Line; 0..5 descline1..6: bAnyDescLine, and
//  bAnyNextLevelLine for the second pass over the same range; 6..9 dsc2line1..4: bAnyDsc2Line). Each is set when any line of its range
//  printed and read once afterwards to decide whether to append a blank line, which is what the dispatcher's return value exists for.
//  bNoBaseLevel is set as a side effect of a short circuit; it is 1 exactly when a skill's level comes entirely from item bonuses:
//      if ( !pSkill || !D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill, 1)
//           || (bNoBaseLevel = 1, D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill, 0)) )
//          bNoBaseLevel = 0;
//  The scan at the bottom is a one-way latch: gbSkillsTxtScanDone gates a walk of the player's class rows looking for any skill whose base level
//  is above zero; it is set only when one is found, and when none is, the tooltip gets tbl 4372 "You have not learned this skill yet" and the
//  global stays clear, so the scan repeats on every tooltip until the character has learned something. Both builders do it, against their own
//  local copy of the count. Nothing clears the global.
//  The original shares one stack slot between bAtMaxLevel, bAnyDescLine, bAnyNextLevelLine and bAnyDsc2Line; they are declared apart here, and
//  bAnyDsc3Line keeps its own. wszText is an array of 1024 Unicode, not of wchar_t: the constructor loop covers all 1024 elements, which is
//  what the declaration does, and the casts to wchar_t * at the two calls that take one are kept.
//  DIFFERENCE: the original loads the Unicode::strcat address into a register once and calls through it (call edi); ours calls it directly.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
void __fastcall SKILLDESC_SkillsBuildSkillLevelText(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nCharges, int32_t nIconLeft, int32_t nIconBottom)
{
    Unicode pText[1024];
    Unicode dest[32];
    Unicode wszText[1024];
    char src[32];

    D2SkillsTxt *pSkillsTxt;
    D2SkillsTxt *pSkillsTxtRec;
    D2SkillsTxt *pSkillsTxtById;
    D2SkillDescTxt *pSkillDescRec;
    D2SkillStrc *pSkill;
    D2SkillStrc *pSkill2;
    D2SkillStrc *pSkill3;

    D2C_StringIndices wStrLong;
    D2C_StringIndices wStrName;
    D2C_Skills nScanSkillId;
    int32_t nSkillDescIdx;
    int32_t nLine;
    int32_t i;
    int32_t nNextLevel;
    int32_t nSkillsTxtRecordCount;
    int32_t nSkillIdx;
    int32_t wMaxLvl;
    int32_t bAtMaxLevel;
    int32_t bAnyDsc3Line;
    int32_t bAnyDescLine;
    int32_t bAnyNextLevelLine;
    int32_t bAnyDsc2Line;
    int32_t bNoBaseLevel;
    int32_t pWidth;
    int32_t pHeight;

    if (pUnit)
    {
        pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
        if (pSkillsTxt)
        {
            pSkill = D2COMMON_SKILLS_GetSkillById(pUnit, nSkillId, 0xFFFFFFFF);
            if (!pSkill || !D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill, 1) || (bNoBaseLevel = 1, D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill, 0)))
            {
                bNoBaseLevel = 0;
            }
            Unicode::strcpy(pText, D2LANG_GetStringFromTblIndex(STR_IDX_4252_StrSkill0));

            //  bAtMaxLevel is 1 only when the skill exists and its level has reached the cap, the cap being 20 when the record has none to offer
            bAtMaxLevel = 0;
            pSkill2 = D2COMMON_SKILLS_GetSkillById(pUnit, nSkillId, 0xFFFFFFFF);
            if (pSkill2)
            {
                wMaxLvl = 20;
                if ((int32_t)nSkillId < sgptDataTables->nSkillsTxtRecordCount)
                {
                    wMaxLvl = (int16_t)sgptDataTables->pSkillsTxt[nSkillId].wMaxLvl;
                    if (wMaxLvl <= 0)
                    {
                        wMaxLvl = 20;
                    }
                }
                if (D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill2, 0) >= wMaxLvl)
                {
                    bAtMaxLevel = 1;
                }
            }

            //  the dsc3 lines, at the next level
            bAnyDsc3Line = 0;
            nLine = 16;
            nNextLevel = nSkillLevel + 1;
            do
            {
                if (SKILLDESC_BuildDescLine(pUnit, pText, pSkillsTxt, nSkillId, nNextLevel, nCharges, nLine, 0))
                {
                    bAnyDsc3Line = 1;
                }
                --nLine;
            }
            while (nLine >= 10);
            if (bAnyDsc3Line)
            {
                Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
            }

            //  the descline lines, at the next level, labelled for it, unless the skill is already at its cap (no next level to describe)
            if (!bAtMaxLevel)
            {
                bAnyDescLine = 0;
                for (nLine = 0; nLine < 6; ++nLine)
                {
                    if (SKILLDESC_BuildDescLine(pUnit, pText, pSkillsTxt, nSkillId, nNextLevel, nCharges, nLine, 0))
                    {
                        bAnyDescLine = 1;
                    }
                }
                if (bAnyDescLine)
                {
                    if (nSkillLevel)
                    {
                        Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_4253_StrSkill1));
                    }
                    else
                    {
                        Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_4269_StrSkill17));
                    }
                    Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
                    Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
                }
            }

            //  the same lines again at the level actually held, with the number printed
            if (nSkillLevel > 0)
            {
                bAnyNextLevelLine = 0;
                for (nLine = 0; nLine < 6; ++nLine)
                {
                    if (SKILLDESC_BuildDescLine(pUnit, pText, pSkillsTxt, nSkillId, nSkillLevel, nCharges, nLine, 0))
                    {
                        bAnyNextLevelLine = 1;
                    }
                }
                if (bAnyNextLevelLine)
                {
                    wsprintfA(src, "%d", nSkillLevel);
                    Unicode::toUnicode(dest, src, 32);
                    Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_4254_StrSkill2));
                    Unicode::strcat(pText, dest);
                    if (bNoBaseLevel)
                    {
                        Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_4370_StrSkill117));
                    }
                    Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
                    Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
                }
            }

            //  the dsc2 lines, at the next level
            bAnyDsc2Line = 0;
            for (i = 6; i < 10; ++i)
            {
                if (SKILLDESC_BuildDescLine(pUnit, pText, pSkillsTxt, nSkillId, nNextLevel, nCharges, i, 0))
                {
                    bAnyDsc2Line = 1;
                }
            }
            if (bAnyDsc2Line)
            {
                Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
            }

            //  the skill's own long string, or the dummy sentinel's
            if ((int32_t)nSkillId < sgptDataTables->nSkillsTxtRecordCount && (pSkillsTxtRec = &sgptDataTables->pSkillsTxt[nSkillId]) != NULL && (nSkillDescIdx = (int16_t)pSkillsTxtRec->wSkillDesc) >= 0
                && nSkillDescIdx < sgptDataTables->nSkillDescTxtRecordCount && (pSkillDescRec = &sgptDataTables->pSkillDescTxt[nSkillDescIdx]) != NULL)
            {
                wStrLong = (D2C_StringIndices)pSkillDescRec->wStrLong;
            }
            else
            {
                wStrLong = STR_IDX_5382_dummy;
            }
            Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(wStrLong));
            Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));

            //  and the skill's name, coloured
            if ((int32_t)nSkillId < sgptDataTables->nSkillsTxtRecordCount && (pSkillsTxtRec = &sgptDataTables->pSkillsTxt[nSkillId]) != NULL && (nSkillDescIdx = (int16_t)pSkillsTxtRec->wSkillDesc) >= 0
                && nSkillDescIdx < sgptDataTables->nSkillDescTxtRecordCount && (pSkillDescRec = &sgptDataTables->pSkillDescTxt[nSkillDescIdx]) != NULL)
            {
                wStrName = (D2C_StringIndices)pSkillDescRec->wStrName;
            }
            else
            {
                wStrName = STR_IDX_5382_dummy;
            }
            Unicode::strcpy(wszText, D2LANG_GetStringFromTblIndex(wStrName));
            UI_AddColorCodeToWstring((wchar_t *)wszText, 2u);
            Unicode::strcat(pText, wszText);

            if (bNoBaseLevel)
            {
                Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
                Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
                Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_4371_StrSkill118));
            }

            //  the one-way latch: walk the class rows for any skill with a base level, and if the walk runs out, say so without setting the
            //  latch, so the next tooltip scans again
            if (!gbSkillsTxtScanDone)
            {
                nScanSkillId = SKILL_ATTACK;
                nSkillsTxtRecordCount = sgptDataTables->nSkillsTxtRecordCount;
                if (nSkillsTxtRecordCount <= 0)
                {
                    goto APPEND_NOT_LEARNED;
                }
                nSkillIdx = 0;
                for (;;)
                {
                    if (nSkillIdx >= 0 && (int32_t)nScanSkillId < nSkillsTxtRecordCount)
                    {
                        pSkillsTxtById = &sgptDataTables->pSkillsTxt[nSkillIdx];
                        if (pSkillsTxtById)
                        {
                            //  BYTE against BYTE (mov dl,[eax+0Ch] / cmp dl,cl, 0x6FB15146..0x6FB15149): the char field sign-extended against a
                            //  zero-extended byte would call a record's -1 (0xFF) unequal to a class byte of 255; the original calls them equal
                            if ((uint8_t)pSkillsTxtById->nCharClass == (uint8_t)pUnit->dwClassId)
                            {
                                pSkill3 = D2COMMON_SKILLS_GetSkillById(pUnit, nScanSkillId, 0xFFFFFFFF);
                                if (pSkill3)
                                {
                                    if (D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill3, 0))
                                    {
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    nScanSkillId = (D2C_Skills)(nScanSkillId + 1);
                    ++nSkillIdx;
                    nSkillsTxtRecordCount = sgptDataTables->nSkillsTxtRecordCount;
                    if ((int32_t)nScanSkillId >= nSkillsTxtRecordCount)
                    {
                        goto APPEND_NOT_LEARNED;
                    }
                }
                gbSkillsTxtScanDone = 1;
                goto SCAN_TAIL;

            APPEND_NOT_LEARNED:
                Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
                Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
                Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_4372_StrSkill119));
            SCAN_TAIL:;
            }

            D2WIN_10131_GetTextDimensions(pText, &pWidth, &pHeight);
            D2WIN_10129_DrawFramedText(pText, nIconLeft - 24, pHeight + nIconBottom + 3, 0, 0);
        }
    }
}


//1.10f: D2Client.0x6FB15220
//  The requirement block of a skill tooltip: the same three description-line ranges as its twin, in the same order (the dsc3 range at
//  nSkillLevel + 1 and the other two at max(nSkillLevel, 1)), then either "Maximum Level Reached" or "Required Level : N", the skill's long
//  string, its name and the "not learned" line. Where the twin runs all three ranges at the next level and repeats 0..5 at the current one, this
//  one has that single next-level range; and the x it draws at is the caller's, not the icon's minus 24. The parameters are the level, the
//  charges, and the two coordinates D2Win_10129_DrawFramedText takes, in that order, as the level builder's. The flags, the two levels and the
//  scan latch work as in SKILLDESC_SkillsBuildSkillLevelText.
//  The labels differ from the twin's: this one labels the 0..5 range with "Current Skill Level: N" when N > 0 and with "First Level" when the
//  caller passed 0 or less, and the level that goes to the dispatcher is max(nSkillLevel, 1) in both that range and the 6..9 one, while the
//  dsc3 range runs at nSkillLevel + 1; the requirement block has no "next level" to describe. "Maximum Level Reached" is also the gate on the
//  required level: when the skill exists and its level is at the cap, the "Required Level" line is not printed at all, and when the value is
//  zero it is skipped too, and the newline after it goes with it.
//  DIFFERENCE: the original loads the Unicode::strcat address into a register once and calls through it (call edi); ours calls it directly.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
void __fastcall SKILLDESC_SkillsBuildSkillRequirementText(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nCharges, int32_t nIconLeft, int32_t nIconBottom)
{
    Unicode pText[1024];
    Unicode dest[32];
    Unicode wszConverted[32];
    Unicode wszText[256];
    char src[32];

    D2SkillsTxt *pSkillsTxt;
    D2SkillsTxt *pReqSkillsTxt1;
    D2SkillsTxt *pReqSkillsTxt2;
    D2SkillsTxt *pReqSkillsTxt3;
    D2SkillDescTxt *pReqSkillDesc1;
    D2SkillDescTxt *pReqSkillDesc2;
    D2SkillStrc *pSkill;
    D2SkillStrc *pSkill2;
    D2SkillStrc *pSkill3;

    D2C_StringIndices wStrLong;
    D2C_StringIndices wStrName;
    D2C_Skills nScanSkillId;
    int32_t wSkillDesc;
    int32_t wReqSkillDesc;
    int32_t nSkillsTxtRecordCount;
    int32_t nSkillIdx;
    int32_t bAnyDsc3Line;
    int32_t bAnyDescLine;
    int32_t bAnyDsc2Line;
    int32_t nLineLevel;
    int32_t i;
    int32_t j;
    int32_t k;
    int32_t wMaxLvl;
    int32_t nReqLevel;
    int32_t bNoBaseLevel;
    int32_t pWidth;
    int32_t pHeight;

    bAnyDsc3Line = 0;
    if (!pUnit)
    {
        return;
    }
    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
    {
        return;
    }
    //  the same skill is fetched twice: the first object is kept for the level-cap test below while the second answers for bNoBaseLevel;
    //  the original really does call GetSkillById twice
    pSkill = D2COMMON_SKILLS_GetSkillById(pUnit, nSkillId, 0xFFFFFFFF);
    pSkill2 = D2COMMON_SKILLS_GetSkillById(pUnit, nSkillId, 0xFFFFFFFF);
    if (!pSkill2 || !D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill2, 1) || (bNoBaseLevel = 1, D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill2, 0)))
    {
        bNoBaseLevel = 0;
    }
    Unicode::strcpy(pText, D2LANG_GetStringFromTblIndex(STR_IDX_4252_StrSkill0));

    for (i = 16; i >= 10; --i)
    {
        if (SKILLDESC_BuildDescLine(pUnit, pText, pSkillsTxt, nSkillId, nSkillLevel + 1, nCharges, i, 1))
        {
            bAnyDsc3Line = 1;
        }
    }
    if (bAnyDsc3Line)
    {
        Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    }

    bAnyDescLine = 0;
    for (j = 0; j < 6; ++j)
    {
        nLineLevel = nSkillLevel;
        if (nLineLevel <= 0)
        {
            nLineLevel = 1;
        }
        if (SKILLDESC_BuildDescLine(pUnit, pText, pSkillsTxt, nSkillId, nLineLevel, nCharges, j, 1))
        {
            bAnyDescLine = 1;
        }
    }
    if (bAnyDescLine)
    {
        if (nSkillLevel > 0)
        {
            wsprintfA(src, "%d", nSkillLevel);
            Unicode::toUnicode(dest, src, 32);
            Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_4254_StrSkill2));
            Unicode::strcat(pText, dest);
        }
        else
        {
            Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_4269_StrSkill17));
        }
        if (bNoBaseLevel)
        {
            Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_4370_StrSkill117));
        }
        Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
        Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    }

    bAnyDsc2Line = 0;
    for (k = 6; k < 10; ++k)
    {
        nLineLevel = nSkillLevel;
        if (nLineLevel <= 0)
        {
            nLineLevel = 1;
        }
        if (SKILLDESC_BuildDescLine(pUnit, pText, pSkillsTxt, nSkillId, nLineLevel, nCharges, k, 1))
        {
            bAnyDsc2Line = 1;
        }
    }
    if (bAnyDsc2Line)
    {
        Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    }

    //  at the cap: the note, and no required level; otherwise the level required at this skill level, or, when that comes out zero, not
    //  even the newline, which is why the newline sits inside the branch
    if (pSkill)
    {
        wMaxLvl = 20;
        if ((int32_t)nSkillId < sgptDataTables->nSkillsTxtRecordCount)
        {
            wMaxLvl = (int16_t)sgptDataTables->pSkillsTxt[nSkillId].wMaxLvl;
            if (wMaxLvl <= 0)
            {
                wMaxLvl = 20;
            }
        }
        if (D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill, 0) >= wMaxLvl)
        {
            Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_4369_StrSkill116));
            Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
            goto SHORT_STRING;
        }
    }
    nReqLevel = nSkillLevel + (int16_t)pSkillsTxt->wReqLevel;
    if (nReqLevel)
    {
        wsprintfA(src, "%d", nReqLevel);
        Unicode::toUnicode(wszConverted, src, 32);
        Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_4187_skilldesc3));
        Unicode::strcat(pText, wszConverted);
        Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    }

    //  the long string, or the dummy sentinel's
SHORT_STRING:
    if ((int32_t)nSkillId < sgptDataTables->nSkillsTxtRecordCount && (pReqSkillsTxt1 = &sgptDataTables->pSkillsTxt[nSkillId]) != NULL && (wSkillDesc = (int16_t)pReqSkillsTxt1->wSkillDesc) >= 0 && wSkillDesc < sgptDataTables->nSkillDescTxtRecordCount
        && (pReqSkillDesc1 = &sgptDataTables->pSkillDescTxt[wSkillDesc]) != NULL)
    {
        wStrLong = (D2C_StringIndices)pReqSkillDesc1->wStrLong;
    }
    else
    {
        wStrLong = STR_IDX_5382_dummy;
    }
    Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(wStrLong));
    Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));

    //  and the name, coloured
    if ((int32_t)nSkillId < sgptDataTables->nSkillsTxtRecordCount && (pReqSkillsTxt2 = &sgptDataTables->pSkillsTxt[nSkillId]) != NULL && (wReqSkillDesc = (int16_t)pReqSkillsTxt2->wSkillDesc) >= 0
        && wReqSkillDesc < sgptDataTables->nSkillDescTxtRecordCount && (pReqSkillDesc2 = &sgptDataTables->pSkillDescTxt[wReqSkillDesc]) != NULL)
    {
        wStrName = (D2C_StringIndices)pReqSkillDesc2->wStrName;
    }
    else
    {
        wStrName = STR_IDX_5382_dummy;
    }
    Unicode::strcpy(wszText, D2LANG_GetStringFromTblIndex(wStrName));
    UI_AddColorCodeToWstring((wchar_t *)wszText, 2u);
    Unicode::strcat(pText, wszText);

    if (bNoBaseLevel)
    {
        Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
        Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
        Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_4371_StrSkill118));
    }

    if (!gbSkillsTxtScanDone)
    {
        nScanSkillId = SKILL_ATTACK;
        nSkillsTxtRecordCount = sgptDataTables->nSkillsTxtRecordCount;
        if (nSkillsTxtRecordCount <= 0)
        {
            goto APPEND_NOT_LEARNED;
        }
        nSkillIdx = 0;
        for (;;)
        {
            if (nSkillIdx >= 0 && (int32_t)nScanSkillId < nSkillsTxtRecordCount)
            {
                pReqSkillsTxt3 = &sgptDataTables->pSkillsTxt[nSkillIdx];
                if (pReqSkillsTxt3)
                {
                    //  BYTE against BYTE (mov dl,[eax+0Ch] / cmp dl,cl, 0x6FB15770..0x6FB15773), as in SKILLDESC_SkillsBuildSkillLevelText
                    if ((uint8_t)pReqSkillsTxt3->nCharClass == (uint8_t)pUnit->dwClassId)
                    {
                        pSkill3 = D2COMMON_SKILLS_GetSkillById(pUnit, nScanSkillId, 0xFFFFFFFF);
                        if (pSkill3)
                        {
                            if (D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill3, 0))
                            {
                                break;
                            }
                        }
                    }
                }
            }
            nScanSkillId = (D2C_Skills)(nScanSkillId + 1);
            ++nSkillIdx;
            nSkillsTxtRecordCount = sgptDataTables->nSkillsTxtRecordCount;
            if ((int32_t)nScanSkillId >= nSkillsTxtRecordCount)
            {
                goto APPEND_NOT_LEARNED;
            }
        }
        gbSkillsTxtScanDone = 1;
        goto SCAN_TAIL;

    APPEND_NOT_LEARNED:
        Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
        Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
        Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_4372_StrSkill119));
    SCAN_TAIL:;
    }

    D2WIN_10131_GetTextDimensions(pText, &pWidth, &pHeight);
    D2WIN_10129_DrawFramedText(pText, nIconLeft, pHeight + nIconBottom + 3, 1, 0);
}


//1.10f: D2Client.0x6FB15850
//  The finishing-move tooltip text. Where the two tooltip-block builders draw the level and requirement blocks of an ordinary skill, this one
//  builds the text of a finishing move: the ten descline lines the dispatcher produces, then tbl 4254 and the number ("Current Skill Level: N",
//  the label the two block builders use), the record's short string and the record's name, all appended into one buffer and drawn by
//  D2Win_10129_DrawFramedText at the coordinates the caller passes. Both callers are in the spell-selector panel and hand it the player from
//  CUNIT_GetPlayer and a skill id read as a word.
//  The 5th argument is a colour, not a flag: it goes to the dispatcher's nColor and to D2Win_10129_DrawFramedText's nTextColor, the slot the two
//  block builders pass 0 and 1 in. The Assassin clause shows what the function serves: !nSkillId and a class-6 player (PCLASS_ASSASSIN) append
//  tbl 22306; skill 0 is the finishing move every Assassin starts with, and the line names it.
//  bHasFinishingMove is the OR of the ten dispatcher answers: the level/charges line goes in only when at least one line printed, and nothing
//  ever clears the flag. The two record lookups at the bottom are the same guard chain written out twice (skills.txt -> wSkillDesc ->
//  skilldesc.txt, then wStrShort (+0x0A) and wStrName (+0x08), tbl 5382 when the chain fails), as in the level and requirement builders.
//  The original inlines DATATBLS_GetSkillsTxtRecordById here.
void __fastcall SKILLDESC_SkillsBuildFinishingMoveText(D2UnitStrc *pUnit, int32_t nSkillId, int32_t nSkillLevel, int32_t nCharges, int32_t nTextColor, int32_t nX, int32_t nY)
{
    Unicode pText[1024];
    Unicode dest[32];
    char src[32];

    D2SkillsTxt *pSkillsTxt;
    D2SkillsTxt *pSkillsTxt1;
    D2SkillsTxt *pSkillsTxt2;
    D2SkillDescTxt *pSkillDescTxt1;
    D2SkillDescTxt *pSkillDescTxt2;
    D2C_StringIndices wStrShort;
    D2C_StringIndices wStrName;
    int32_t bHasFinishingMove;
    int32_t nLine;
    int32_t nSkillId1;
    int32_t pHeight;
    int32_t pWidth;
    int32_t wSkillDesc;
    int32_t nSkillDesc2;

    if (!pUnit)
    {
        return;
    }
    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    nSkillId1 = nSkillId;
    if (!pSkillsTxt)
    {
        return;
    }

    Unicode::strcpy(pText, D2LANG_GetStringFromTblIndex(STR_IDX_4252_StrSkill0));
    nLine = 0;
    bHasFinishingMove = 0;
    do
    {
        if (SKILLDESC_BuildDescLine(pUnit, pText, pSkillsTxt, (D2C_Skills)nSkillId, nSkillLevel, nCharges, nLine, nTextColor))
        {
            bHasFinishingMove = 1;
        }
        ++nLine;
    } while (nLine < 10);

    if (bHasFinishingMove)
    {
        wsprintfA(src, "%d", nSkillLevel);
        Unicode::toUnicode(dest, src, 32);
        Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_4254_StrSkill2));
        Unicode::strcat(pText, dest);
        Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
        Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    }

    if (!nSkillId && pUnit->dwUnitType == UNIT_PLAYER && pUnit->dwClassId == PCLASS_ASSASSIN)
    {
        Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_22306_Eskillfinishmove));
    }

    //  the short string, then a newline
    if (nSkillId < sgptDataTables->nSkillsTxtRecordCount && (pSkillsTxt1 = &sgptDataTables->pSkillsTxt[nSkillId1]) != NULL && (wSkillDesc = (int16_t)pSkillsTxt1->wSkillDesc, wSkillDesc >= 0) && wSkillDesc < sgptDataTables->nSkillDescTxtRecordCount
        && (pSkillDescTxt1 = &sgptDataTables->pSkillDescTxt[wSkillDesc]) != NULL)
    {
        wStrShort = (D2C_StringIndices)pSkillDescTxt1->wStrShort;
    }
    else
    {
        wStrShort = STR_IDX_5382_dummy;
    }
    Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(wStrShort));
    Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));

    //  and the name, with no newline after it
    if (nSkillId < sgptDataTables->nSkillsTxtRecordCount && (pSkillsTxt2 = &sgptDataTables->pSkillsTxt[nSkillId1]) != NULL && (nSkillDesc2 = (int16_t)pSkillsTxt2->wSkillDesc, nSkillDesc2 >= 0) && nSkillDesc2 < sgptDataTables->nSkillDescTxtRecordCount
        && (pSkillDescTxt2 = &sgptDataTables->pSkillDescTxt[nSkillDesc2]) != NULL)
    {
        wStrName = (D2C_StringIndices)pSkillDescTxt2->wStrName;
    }
    else
    {
        wStrName = STR_IDX_5382_dummy;
    }
    Unicode::strcat(pText, D2LANG_GetStringFromTblIndex(wStrName));

    D2WIN_10131_GetTextDimensions(pText, &pWidth, &pHeight);
    D2WIN_10129_DrawFramedText(pText, nX, pHeight + nY + 3, nTextColor, 0);
}


//  Accessors of the descdam and descatt tables (gpSkillDescDamFuncs, 36 entries; gpSkillDescAttFuncs, 18 entries).
//  BUG: the callers bound the index by 0x90 and 0x48, the tables' sizes in bytes over 36 and 18 entries, so an index the bound lets through can
//  lie past the table. LATENT: stock skilldesc.txt stays inside.
//  DIFFERENCE: inside the table these answer our row; past the end the bytes read the original's .data at the same offset and call what they
//  find, while ours answers NULL, so each caller's NULL test takes its no-function arm (bHasDamFunc or bHasAttFunc 0;
//  SKILLDESC_GetAttackRatingForHand's 0). The bytes read the table twice, the test and the call, and so does the C.
using SKILLDESC_DamFuncFn = void (__fastcall *)(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
using SKILLDESC_AttFuncFn = void (__fastcall *)(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnAttackRatingA, int32_t *pnColorA, int32_t *pnAttackRatingB, int32_t *pnColorB);

static SKILLDESC_DamFuncFn SKILLDESC_GetDamFuncOrNull(uint32_t nIndex)
{
    if (nIndex < 36)
        return gpSkillDescDamFuncs[nIndex];

    return NULL;
}

static SKILLDESC_AttFuncFn SKILLDESC_GetAttFuncOrNull(uint32_t nIndex)
{
    if (nIndex < 18)
        return gpSkillDescAttFuncs[nIndex];

    return NULL;
}


//1.10f: D2Client.0x6FB15B00
//  The skill description panel of one mouse button. The title first, then both skilldesc.txt function columns dispatched:
//      gpSkillDescDamFuncs[pSkillDesc->wDescDam]   (+0x12, descdam)
//      gpSkillDescAttFuncs[pSkillDesc->wDescAtt]   (+0x14, descatt)
//  nLayoutRow is the panel slot, 0 for the left button's block and 6 for the right's, and every row this function touches is an offset from it:
//  the title at the row itself, the "Damage" label one row down, the damage handler's own coordinates two down, the attack line three down and
//  the value row five down. SKILLDESC_DrawBothSkillInfos supplies both slots. The descdam / descatt bounds are the loose ones described above.
//  The attack path, in order:
//  - PLRSKILLS_SkillsGetUseStateForClient gates both dispatches: only the four states 0 (usable), 1 (no mana), 6 (aura) and 8 (blocked) let
//    them run.
//  - the att handler fills four outputs and the sixth is a colour (the handlers write 3 or 1 into it). nAttackRatingA/nAttackRatingB are the
//    two figures, nColorA/nColorB their colours.
//  - the line is Unicode::sprintf(200, ...) of tbl 4063/4065, "%s\n..." with the uppercased title as the %s, and `pText[199].ch = 0` pins a
//    terminator the constructor never wrote, because the sprintf has no bound of its own. CHAR_TextCutNextLine then splits the line at its
//    newline.
//  - BUG: the else arm draws wszText, the title, not the sprintf output. LATENT: with the stock table both formats carry the newline ("%s\nAttack
//    Rating" and "%s\nRating", lines 4064 and 4066 of the tbl), so the arm never runs; a modded table without it would draw the title twice.
//  - the two-figure arm writes both numbers with wsprintfA "%d" and draws them at the row's nY offset by the two line offsets of the shared CJK
//    latch, offset-for-offset as SKILLDESC_DrawTwoLines. The one-figure arm draws nAttackRatingA in nColorA's colour, FONT16 below 1000 and
//    FONT8 at or above.
//  The original uses one stack slot for the skill id (stored 0x6FB15B44, read 0x6FB15BD9, tested 0x6FB15E0F for the 4063/4065 choice), then the
//  att handler's colour (0x6FB15FD3), then the single figure (0x6FB160C5); they are declared apart here as SkillIdFromSkill, nLineColor and
//  nSingleValue. The language compares here and in SKILLDESC_DrawBothSkillInfos are signed in the original (jl and jle); every value
//  STRTABLE_GetLanguage can return is 0..12, where signed and unsigned agree, so no cast is written. The original inlines
//  DATATBLS_GetSkillsTxtRecordById and the skilldesc.txt record lookup here.
void __fastcall SKILLDESC_DrawSkillInfo(D2UnitStrc *pUnit, D2SkillStrc *pSkill, int32_t nLayoutRow)
{
    Unicode wszText[32];
    Unicode pText[200];
    Unicode dest[32];
    Unicode wszValue[32];
    char src[32];

    D2UnitStrc *pUnita;
    D2SkillsTxt *pSkillsTxt;
    D2SkillsTxt *pSkillsTxtById;
    D2SkillsTxt *pSkillsTxtRec;
    D2SkillDescTxt *pSkillDescSlot;
    D2SkillDescTxt *pSkillDescRec;
    const Unicode *wszStrchratr;
    Unicode *pwszSecondLine;
    D2C_StringIndices wStrAlt;
    D2C_Language nLanguage;
    int32_t SkillIdFromSkill;
    int32_t SkillLevel;
    int32_t wSkillDesc;
    int32_t wSkillDesc2;
    int32_t nLineColor;
    int32_t nSingleValue;
    int32_t bHasDamFunc;
    int32_t bHasAttFunc;
    int32_t nAttackRatingA;
    int32_t nColorA;
    int32_t nAttackRatingB;
    int32_t nColorB;
    int32_t nRow3;
    int32_t nRow5;
    int32_t nColor;
    int32_t nLayoutIdx;
    int32_t nLeftRow5_1;
    int32_t nYRow5_1;
    int32_t nRightRow5_1;
    int32_t nLeftRow5_2;
    int32_t nYRow5_2;
    int32_t nRightRow5_2;

    pUnita = pUnit;
    pSkillsTxt = D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill(pSkill);
    if (!pSkillsTxt)
    {
        return;
    }
    SkillLevel = D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill, 1);
    SkillIdFromSkill = D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szSkillDescCpp, 5512);
    pSkillsTxtById = DATATBLS_GetSkillsTxtRecordById(SkillIdFromSkill);
    if (!pSkillsTxtById)
    {
        return;
    }
    wSkillDesc = (int16_t)pSkillsTxtById->wSkillDesc;
    if (wSkillDesc < 0 || wSkillDesc >= sgptDataTables->nSkillDescTxtRecordCount)
    {
        return;
    }
    pSkillDescSlot = &sgptDataTables->pSkillDescTxt[wSkillDesc];
    if (!pSkillDescSlot)
    {
        return;
    }

    D2WIN_10127_SetFont(D2FONT_FONT6);

    //  the title, uppercased
    if (SkillIdFromSkill < sgptDataTables->nSkillsTxtRecordCount && (pSkillsTxtRec = &sgptDataTables->pSkillsTxt[SkillIdFromSkill]) != NULL && (wSkillDesc2 = (int16_t)pSkillsTxtRec->wSkillDesc, wSkillDesc2 >= 0)
        && wSkillDesc2 < sgptDataTables->nSkillDescTxtRecordCount && (pSkillDescRec = &sgptDataTables->pSkillDescTxt[wSkillDesc2]) != NULL)
    {
        wStrAlt = (D2C_StringIndices)pSkillDescRec->wStrAlt;
    }
    else
    {
        wStrAlt = STR_IDX_5382_dummy;
    }
    Unicode::strcpy(wszText, D2LANG_GetStringFromTblIndex(wStrAlt));
    UI_TextToUpperInPlace(wszText);
    CHAR_DrawTextCentered(gnBorderXDifference + gSkillDescPanelLayout[3 * nLayoutRow], gnBorderYDifference + gSkillDescPanelLayout[3 * nLayoutRow + 1] + gnScreenHeight - 480, gnBorderXDifference + gSkillDescPanelLayout[3 * nLayoutRow + 2],
        wszText, 0);

    bHasDamFunc = 0;
    bHasAttFunc = 0;
    switch (PLRSKILLS_SkillsGetUseStateForClient(pUnita, pSkill))
    {
    case SKILLUSTATE_USABLE:
    case SKILLUSTATE_NOMANA:
    case SKILLUSTATE_AURA:
    case SKILLUSTATE_BLOCKED:
        bHasDamFunc = 1;
        bHasAttFunc = 1;
        break;
    default:
        break;
    }
    if (pSkillDescSlot->wDescDam >= 0x90u || !SKILLDESC_GetDamFuncOrNull(pSkillDescSlot->wDescDam))
    {
        bHasDamFunc = 0;
    }
    if (pSkillDescSlot->wDescAtt >= 0x48u || !SKILLDESC_GetAttFuncOrNull(pSkillDescSlot->wDescAtt))
    {
        bHasAttFunc = 0;
    }

    //  the damage block: the label at row nLayoutRow + 1, the handler called with the coordinates of row nLayoutRow + 2
    if (bHasDamFunc)
    {
        CHAR_DrawTextCentered(gnBorderXDifference + gSkillDescPanelLayout[3 * nLayoutRow + 3], gnBorderYDifference + gSkillDescPanelLayout[3 * nLayoutRow + 4] + gnScreenHeight - 480, gnBorderXDifference + gSkillDescPanelLayout[3 * nLayoutRow + 5],
            D2LANG_GetStringFromTblIndex(STR_IDX_4061_strchrskm), 0);
        SKILLDESC_GetDamFuncOrNull(pSkillDescSlot->wDescDam)(pUnita, pSkill, pSkillsTxt, SkillLevel, gSkillDescPanelLayout[3 * nLayoutRow + 6], gSkillDescPanelLayout[3 * nLayoutRow + 7], gSkillDescPanelLayout[3 * nLayoutRow + 8]);
    }

    if (bHasAttFunc)
    {
        nAttackRatingA = 0;
        nAttackRatingB = 0;
        nColorA = 0;
        nColorB = 0;
        SKILLDESC_GetAttFuncOrNull(pSkillDescSlot->wDescAtt)(pUnita, pSkill, pSkillsTxt, SkillLevel, &nAttackRatingA, &nColorA, &nAttackRatingB, &nColorB);

        //  the attack line at row nLayoutRow + 3, split in two when it has a newline
        nRow3 = nLayoutRow + 3;
        if (nAttackRatingA || nAttackRatingB)
        {
            D2WIN_10127_SetFont(D2FONT_FONT6);
            if (SkillIdFromSkill)
            {
                wszStrchratr = D2LANG_GetStringFromTblIndex(STR_IDX_4063_strchratr);
            }
            else
            {
                wszStrchratr = D2LANG_GetStringFromTblIndex(STR_IDX_4065_strchrrat);
            }
            Unicode::sprintf(200, pText, wszStrchratr, wszText);
            pText[199].ch = 0;
            if (CHAR_TextCutNextLine(pText, &pwszSecondLine))
            {
                CHAR_DrawTextCentered(gnBorderXDifference + gSkillDescPanelLayout[3 * nRow3], gnBorderYDifference + gSkillDescPanelLayout[3 * nRow3 + 1] + gnScreenHeight - 484, gnBorderXDifference + gSkillDescPanelLayout[3 * nRow3 + 2], pText, 0);
                CHAR_DrawTextCentered(gnBorderXDifference + gSkillDescPanelLayout[3 * nRow3], gnBorderYDifference + gSkillDescPanelLayout[3 * nRow3 + 1] + gnScreenHeight - 476, gnBorderXDifference + gSkillDescPanelLayout[3 * nRow3 + 2],
                    pwszSecondLine, 0);
            }
            else
            {
                CHAR_DrawTextCentered(gnBorderXDifference + gSkillDescPanelLayout[3 * nRow3], gnBorderYDifference + gSkillDescPanelLayout[3 * nRow3 + 1] + gnScreenHeight - 480, gnBorderXDifference + gSkillDescPanelLayout[3 * nRow3 + 2], wszText, 0);
            }
        }

        //  the value row at nLayoutRow + 5: two numbers with their own colours, or one
        nRow5 = nLayoutRow + 5;
        if (nAttackRatingB)
        {
            wsprintfA(src, "%d", nAttackRatingA);
            Unicode::toUnicode(dest, src, 32);
            wsprintfA(src, "%d", nAttackRatingB);
            Unicode::toUnicode(wszValue, src, 32);
            nColor = nColorB;
            nLineColor = nColorA;
            nLayoutIdx = 3 * nRow5;
            nRightRow5_1 = gSkillDescPanelLayout[nLayoutIdx + 2];
            nYRow5_1 = gSkillDescPanelLayout[nLayoutIdx + 1];
            nLeftRow5_1 = gSkillDescPanelLayout[nLayoutIdx];
            D2WIN_10127_SetFont(D2FONT_FONT6);
            if (!gbSkillDescLineOffsetsReady)
            {
                if (D2LANG_STRTABLE_GetLanguage() < LANGUAGE_JAPANESE || (nLanguage = D2LANG_STRTABLE_GetLanguage(), gnSkillDescLine1YOffset = -7, nLanguage > LANGUAGE_CHINESETWN))
                {
                    gnSkillDescLine1YOffset = -6;
                }
                gbSkillDescLineOffsetsReady = 1;
            }
            CHAR_DrawTextCentered(gnBorderXDifference + nLeftRow5_1, gnBorderYDifference + nYRow5_1 + gnSkillDescLine1YOffset + gnScreenHeight - 480, nRightRow5_1 + gnBorderXDifference, dest, nLineColor);
            CHAR_DrawTextCentered(gnBorderXDifference + nLeftRow5_1, gnBorderYDifference + gnSkillDescLine2YOffset + nYRow5_1 + gnScreenHeight - 480, gnBorderXDifference + nRightRow5_1, wszValue, nColor);
        }
        else if (nAttackRatingA)
        {
            nSingleValue = nAttackRatingA;
            nRightRow5_2 = gSkillDescPanelLayout[3 * nRow5 + 2];
            nYRow5_2 = gSkillDescPanelLayout[3 * nRow5 + 1];
            nLeftRow5_2 = gSkillDescPanelLayout[3 * nRow5];
            wsprintfA(src, "%d", nSingleValue);
            Unicode::toUnicode(wszValue, src, 32);
            D2WIN_10127_SetFont(nSingleValue < 1000 ? D2FONT_FONT16 : D2FONT_FONT8);
            CHAR_DrawTextCentered(gnBorderXDifference + nLeftRow5_2, gnScreenHeight + gnBorderYDifference + nYRow5_2 - 480, gnBorderXDifference + nRightRow5_2, wszValue, nColorA);
        }
    }
}


//1.10f: D2Client.0x6FB16190
//  Draws both mouse buttons' skill panels: UNITS_GetLeftSkill into SKILLDESC_DrawSkillInfo(pUnit, skill, 0) and UNITS_GetRightSkill into the
//  same with 6, the two panel slots of the layout table. gbSkillDescPanelNudged is the one-time latch in front of the CJK fixup: for languages
//  6..9 (Japanese through ChineseTwn) the nY of row 0 and of row 6 (layout entries [1] and [19], the first row of each button's block) is
//  decremented once per session, and the latch is set even when no nudge happened. The same band picks SKILLDESC_DrawTwoLines' -7 line offset
//  and the localised inventory artwork.
void __fastcall SKILLDESC_DrawBothSkillInfos(D2UnitStrc *pUnit)
{
    D2SkillStrc *LeftSkill;
    D2SkillStrc *RightSkill;

    if (!gbSkillDescPanelNudged)
    {
        if (D2LANG_STRTABLE_GetLanguage() >= LANGUAGE_JAPANESE && D2LANG_STRTABLE_GetLanguage() <= LANGUAGE_CHINESETWN)
        {
            --gSkillDescPanelLayout[1];
            --gSkillDescPanelLayout[19];
        }
        gbSkillDescPanelNudged = 1;
    }
    if (pUnit)
    {
        LeftSkill = D2COMMON_UNITS_GetLeftSkill(pUnit);
        SKILLDESC_DrawSkillInfo(pUnit, LeftSkill, 0);
        RightSkill = D2COMMON_UNITS_GetRightSkill(pUnit);
        SKILLDESC_DrawSkillInfo(pUnit, RightSkill, 6);
    }
}


//1.10f: D2Client.0x6FB16200
//  The attack rating the character panel shows for one hand: the hand's skill -> skills.txt -> wSkillDesc -> skilldesc.txt, then the row's
//  descatt handler through gpSkillDescAttFuncs, bounded at 0x48 and null-checked: the same dispatch SKILLDESC_DrawSkillInfo makes, reduced to its
//  first output. result holds SKILLS_GetSkillsTxtRecordFromSkill's answer, copied into pSkillsTxt, and the null path returns it (0). The sixth
//  argument is the colour slot: the caller passes an int (nColorA) and the handlers write 3 (blue) or 1 (red) into it. nAttackRatingB and
//  nColorB are written and ignored. The original inlines DATATBLS_GetSkillsTxtRecordById and the skilldesc.txt record lookup here.
int32_t __fastcall SKILLDESC_GetAttackRatingForHand(D2UnitStrc *pUnit, int32_t bLeftHand)
{
    int32_t result;
    D2SkillStrc *LeftSkill;
    D2SkillStrc *pSkill;
    D2SkillsTxt *pSkillsTxt;
    int32_t nSkillId;
    D2SkillsTxt *pSkillsTxtRec;
    int32_t wSkillDesc;
    D2SkillDescTxt *pSkillDescTxt;
    int32_t SkillLevel;
    int32_t nAttackRating;
    int32_t nAttackRatingB;
    int32_t nColorB;
    int32_t nColorA;

    if (!pUnit)
    {
        return 0;
    }
    if (bLeftHand)
    {
        LeftSkill = D2COMMON_UNITS_GetLeftSkill(pUnit);
    }
    else
    {
        LeftSkill = D2COMMON_UNITS_GetRightSkill(pUnit);
    }
    pSkill = LeftSkill;
    pSkillsTxt = D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill(LeftSkill);
    result = (int32_t)(INT_PTR)pSkillsTxt;
    if (!result)
    {
        return result;
    }
    nSkillId = *(int16_t *)result;
    if ((pSkillsTxtRec = DATATBLS_GetSkillsTxtRecordById(nSkillId)) != NULL && (wSkillDesc = (int16_t)pSkillsTxtRec->wSkillDesc, wSkillDesc >= 0) && wSkillDesc < sgptDataTables->nSkillDescTxtRecordCount
        && (pSkillDescTxt = &sgptDataTables->pSkillDescTxt[wSkillDesc]) != NULL)
    {
        nAttackRating = 0;
        nAttackRatingB = 0;
        if (pSkillDescTxt->wDescAtt < 0x48u && SKILLDESC_GetAttFuncOrNull(pSkillDescTxt->wDescAtt))
        {
            SkillLevel = D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill, 1);
            SKILLDESC_GetAttFuncOrNull(pSkillDescTxt->wDescAtt)(pUnit, pSkill, pSkillsTxt, SkillLevel, &nAttackRating, &nColorA, &nAttackRatingB, &nColorB);
        }
        return nAttackRating;
    }
    return 0;
}

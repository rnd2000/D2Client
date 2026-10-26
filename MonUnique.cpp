#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "chat.h"
#include "MonUnique.h"

//  MonUnique.cpp -- the boss modifiers (monumod.txt) of D2Client.dll 1.10: the name a boss gets, the handlers gpMonUModHandlers holds and the walk
//  that runs them, the client's own hooks (gpMonUModCltFuncs, 43 rows of five columns) with their dispatcher and the ring of eight missiles two
//  of them draw, and the label of a boss's enchantments for the hover text.
//  The original source file is UNIT\MonUnique.cpp: MONUNIQUE_ModTeleport hands its path to D2COMMON_SKILLS_AssignSkill as __FILE__, with line 337.
//  Address range 0x6FB44A70 (MONUNIQUE_BuildMonsterNameText) to MONUNIQUE_MonUModBuildMonsterModifierLabel at 0x6FB462C0.
//
//  gwszNameSeparator is built in the original by a static initialiser for `Unicode gwszNameSeparator[2]` (two constructions), which is not
//  rewritten; the global is in MonUnique.h. It is written by MONUNIQUE_BuildMonsterNameText and read twice by
//  MONUNIQUE_MonUModBuildMonsterModifierLabel.
//
//  A handler's third argument is bUnique: the monster's type flag 8 (MONTYPEFLAG_UNIQUE), which MONUNIQUE_ApplyModifiers passes. A handler
//  given 0 does nothing but MONUNIQUE_ModDurielDead, which never reads it. The second is D2C_MonUMods, the monumod.txt ids; many names read
//  nothing like the stock table's keys, so the C writes the stock key and the id beside each.
//  The client hooks' third argument is bUnique too: the dispatcher pushes `(byte [pMonsterData+16h] >> 3) & 1` with the modifier byte in edx.
//  The dispatcher returns before any hook for a NULL unit, a unit that is not a monster and a monster with no data, so every hook runs on a
//  monster with its data (column 4, whose ecx is a missile, excepted) and their own tests of pUnit cannot fail.
//
//  The read-only tables gpMonUModHandlers, gnMonUModOrder, gaChampionUModNames, gpMonUModCltFuncs and gMonUModLabelStrings are ours, in
//  MonUnique.h; nothing writes them, and every live slot of the two function tables is a function of this file.
//
//  The path literal the unit passes as __FILE__ (0x6FB91C10).
static const char szMonUniqueCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UNIT\\MonUnique.cpp";

static_assert(offsetof(D2MonsterDataStrc, dwDurielFlag) == 0x18
                  && offsetof(D2SuperUniquesTxt, wNameStr) == 0x02
                  && offsetof(D2ChampionUModNameStrc, nUMod) == 0
                  && offsetof(D2ChampionUModNameStrc, nNameStr) == 0x04,
              "MonUnique.cpp: `mov dword ptr [ecx+18h], 0` (0x6FB44E60); `mov ax, [eax+2]` "
              "(0x6FB44B1C); `mov eax, ds:gaChampionUModNames[ebx*8]` (0x6FB44B53), `mov cx, "
              "word ptr ds:(gaChampionUModNames+4)[ebx*8]` (0x6FB44B9A)");
static_assert(MONSTER_DURIEL == 211 && MONSTER_IZUAL == 256 && SKILL_MONTELEPORT == 184
                  && STR_IDX_3220_Champion == 3220 && STR_IDX_11084_champghostlyX == 11084
                  && STR_IDX_11085_champfanaticX == 11085 && STR_IDX_11086_champpossessedX == 11086
                  && STR_IDX_11087_champberserkX == 11087 && STR_IDX_11072_x_FMT_0_1 == 11072
                  && STR_IDX_1721_Monster1Format == 1721 && STR_IDX_1722_Monster2Format == 1722
                  && STR_IDX_3111_Bloodraven == 3111 && STR_IDX_1014_Izual == 1014
                  && STR_IDX_3021_Andariel == 3021 && STR_IDX_3054_Duriel == 3054
                  && STR_IDX_3060_Diablo == 3060 && STR_IDX_3061_Baal == 3061
                  && STR_IDX_3062_Mephisto == 3062,
              "MonUnique.cpp batch 1: `sub edi, 37h` from 9Ch (0x6FB44C0F), `sub edi, 100h` "
              "(0x6FB44C37), `push 0B8h` (0x6FB44E89); the tbl indices 0C94h..2B4Fh in "
              "gaChampionUModNames, `mov ecx, 2B40h` (0x6FB44BA8), 6B9h, 6BAh, 0C27h, 3F6h, "
              "0BCDh, 0BEEh, 0BF4h, 0BF5h, 0BF6h");
static_assert(MONUMOD_RNDNAME == 1 && MONUMOD_HPMULTIPLY == 2 && MONUMOD_LIGHT == 3
                  && MONUMOD_LEVELADD == 4 && MONUMOD_MAGICRESISTANT == 8 && MONUMOD_FIRECHANT == 9
                  && MONUMOD_WORMDEATH == 11 && MONUMOD_BRAVENDEATH == 12 && MONUMOD_CHAMPMODS == 16
                  && MONUMOD_LIGHTCHANT == 17 && MONUMOD_COLDCHANT == 18 && MONUMOD_POISONFIELD == 23
                  && MONUMOD_MANABURN == 25 && MONUMOD_TELEHEAL == 26 && MONUMOD_SPECTRALHIT == 27
                  && MONUMOD_STONESKIN == 28 && MONUMOD_CHAMPSTONED == 36 && MONUMOD_CHAMPSTATS == 37
                  && MONUMOD_CHAMPCURSEIMMUNE == 38 && MONUMOD_CHAMPSTATS2 == 39,
              "MonUnique.cpp batch 1: gnMonUModOrder's dwords 1..4, gpMonUModHandlers' slots, "
              "gaChampionUModNames' ids 10h, 24h..27h; `cmp [esp+274h+var_264], 0Ch` (0x6FB44BE8), "
              "`cmp edx, 1Ch` (0x6FB44EC9) and the `cmp ecx, N` chain (0x6FB44F86..0x6FB45123)");


//1.10f: D2Client.0x6FB44A70
//  Builds a boss's name and hands it to D2Common's MONSTERS_SetMonsterNameInMonsterData. Seven slots of gpMonUModHandlers hold it -- monumod.txt
//  rndname (1), bloodraven (12), champion (16) and ghostly, fanatic, possessed, berserk (36..39) -- and nothing calls it.
//  The original inlines the name-seed getter, MONSTER_TestTypeFlag, MONSTER_GetBossHcIdx, DATATBLS_GetMonStatsTxtRecord and
//  SEED_RollLimitedRandomNumber here. In the bytes' order:
//
//    * bUnique 0 returns at once, before the buffer is built.
//    * The separator: gwszNameSeparator is refilled from the ASCII " " at gszSpace on every call -- copied into an eight-byte local by the
//      inlined strcpy, then Unicode::toUnicode with a count of 2. MONUNIQUE_MonUModBuildMonsterModifierLabel reads it.
//    * A superunique (type flag 2): its superuniques.txt row's name, or "an evil force" (STR_IDX_5382_dummy) for a row whose name is 0; no row
//      returns with nothing set.
//    * A champion (MONSTER_IsChampion): "<modifier> <monster>" through the format 11072, '%0 %1' -- the modifier's string from
//      gaChampionUModNames, the monster's from its monstats.txt row. The search compares the first FOUR of the five pairs (`cmp ebx, 4 / jl`);
//      a miss leaves the index at 4, the fifth pair, berserk. MONUNIQUE_ApplyModifiers calls this function first with id 1 (its fixed walk),
//      which misses, and then once for each modifier the monster carries; the last of those to find its id names it.
//    * Otherwise the unit's seed is reset to the name seed (D2COMMON_SEED_InitLowSeed) and: Blood Raven for id 12; a fixed name for seven
//      classes, MONSTER_ANDARIEL, MONSTER_DURIEL, MONSTER_MEPHISTO, MONSTER_DIABLO, MONSTER_IZUAL, MONSTER_DIABLOCLONE (Diablo's name) and
//      MONSTER_BAALCRAB (Baal's); and for every other class "<prefix> <suffix>" (Monster1Format), redone as "<prefix> <suffix> <appellation>"
//      (Monster2Format) on a roll under 50 of 100. The rolls are D2Common's and draw on the unit's seed, so their order is the bytes': suffix,
//      then prefix, and appellation, suffix, prefix -- the C takes each into a local in that order rather than leaving it to the evaluation
//      order of an argument list.
//
//  The class switch is MSVC's split on 243 (`cmp edi, 0F3h / jg / jz`) with a chain of `sub` on each side: a switch in the source.
//
//  BUG: the champion's monstats.txt lookup answers NULL for a class out of range and the name is read off it at once (`mov cx, [eax+6]`,
//  0x6FB44B8F). UNREACHABLE: the class is the unit's own, bounded where the unit is made.
//  BUG: a NULL pMonster passes every test above and reaches D2COMMON_SEED_InitLowSeed with &pMonster->pSeed, 0x20 (`lea ebx, [esi+20h]`,
//  0x6FB44ACA). UNREACHABLE: MONUNIQUE_ApplyModifiers alone runs the handlers and returns for a NULL unit before any of them.
void __fastcall MONUNIQUE_BuildMonsterNameText(D2UnitStrc *pMonster, D2C_MonUMods nModId, int32_t bUnique)
{
    uint16_t wNameSeed;
    D2SuperUniquesTxt *pSuperUniquesTxt;
    D2MonStatsTxt *pMonStatsTxt;
    const Unicode *wszFormat;
    const Unicode *wszMonster;
    const Unicode *wszModifier;
    const Unicode *wszPrefix;
    const Unicode *wszSuffix;
    const Unicode *wszAppellation;
    D2C_StringIndices nNameStr;
    int32_t nClassId;
    int32_t i;

    if (!bUnique)
        return;

    //  300 constructions (`mov ebx, 12Ch`), after the test.
    Unicode wszName[300];
    char szSpace[8];

    strcpy(szSpace, gszSpace);
    Unicode::toUnicode(gwszNameSeparator, szSpace, 2);

    if (pMonster && pMonster->dwUnitType == UNIT_MONSTER && pMonster->pMonsterData)
        wNameSeed = pMonster->pMonsterData->wNameSeed;
    else
        wNameSeed = 0;

    if (MONSTER_TestTypeFlag(pMonster, MONTYPEFLAG_SUPERUNIQUE))
    {
        pSuperUniquesTxt = D2COMMON_DATATBLS_GetSuperUniquesTxtRecord(MONSTER_GetBossHcIdx(pMonster));
        if (!pSuperUniquesTxt)
            return;

        nNameStr = pSuperUniquesTxt->wNameStr ? (D2C_StringIndices)pSuperUniquesTxt->wNameStr : STR_IDX_5382_dummy;
    }
    else
    {
        nClassId = pMonster ? pMonster->dwClassId : -1;

        if (MONSTER_IsChampion(pMonster))
        {
            for (i = 0; i < 4; ++i)
            {
                if (gaChampionUModNames[i].nUMod == nModId)
                    break;
            }

            pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(nClassId);
            //  BUG: read with no NULL test (0x6FB44B8F).
            wszMonster = D2LANG_GetStringFromTblIndex((D2C_StringIndices)pMonStatsTxt->wNameStr);
            wszModifier = D2LANG_GetStringFromTblIndex(gaChampionUModNames[i].nNameStr);
            wszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_11072_x_FMT_0_1);
            LANG_FormatString(wszName, wszFormat, wszModifier, wszMonster, (const Unicode *)NULL);
            D2COMMON_MONSTERS_SetMonsterNameInMonsterData(pMonster, wszName);
            return;
        }

        //  BUG: for a NULL pMonster this address is 0x20.
        D2COMMON_SEED_InitLowSeed(&pMonster->pSeed, wNameSeed);

        if (nModId == MONUMOD_BRAVENDEATH)                      //  12, monumod.txt bloodraven
        {
            nNameStr = STR_IDX_3111_Bloodraven;
        }
        else
        {
            switch (nClassId)
            {
            case MONSTER_ANDARIEL:
                nNameStr = STR_IDX_3021_Andariel;
                break;
            case MONSTER_DURIEL:
                nNameStr = STR_IDX_3054_Duriel;
                break;
            case MONSTER_MEPHISTO:
                nNameStr = STR_IDX_3062_Mephisto;
                break;
            case MONSTER_DIABLO:
            case MONSTER_DIABLOCLONE:
                nNameStr = STR_IDX_3060_Diablo;
                break;
            case MONSTER_IZUAL:
                nNameStr = STR_IDX_1014_Izual;
                break;
            case MONSTER_BAALCRAB:
                nNameStr = STR_IDX_3061_Baal;
                break;
            default:
                wszSuffix = D2COMMON_DATATBLS_RollRandomUniqueSuffixString(pMonster);
                wszPrefix = D2COMMON_DATATBLS_RollRandomUniquePrefixString(pMonster);
                wszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_1721_Monster1Format);
                LANG_FormatString(wszName, wszFormat, wszPrefix, wszSuffix, (const Unicode *)NULL);

                if (SEED_RollLimitedRandomNumber(&pMonster->pSeed, 100) < 50)
                {
                    wszAppellation = D2COMMON_DATATBLS_RollRandomUniqueAppellationString(pMonster);
                    wszSuffix = D2COMMON_DATATBLS_RollRandomUniqueSuffixString(pMonster);
                    wszPrefix = D2COMMON_DATATBLS_RollRandomUniquePrefixString(pMonster);
                    wszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_1722_Monster2Format);
                    LANG_FormatString(wszName, wszFormat, wszPrefix, wszSuffix, wszAppellation, (const Unicode *)NULL);
                }

                D2COMMON_MONSTERS_SetMonsterNameInMonsterData(pMonster, wszName);
                return;
            }
        }
    }

    //  The fixed names' one tail.
    D2COMMON_MONSTERS_SetMonsterNameInMonsterData(pMonster, D2LANG_GetStringFromTblIndex(nNameStr));
}


//1.10f: D2Client.0x6FB44D20
//  A boss's light -- gpMonUModHandlers' slot 3, monumod.txt light, which MONUNIQUE_ApplyModifiers' fixed walk runs on every boss with a
//  modifier. nModId is not read. The unit's light is dropped (CUNIT_GfxFreeUnitLightMap) and a new one added: DLIGHTMAP_AddLight with type 0,
//  radius 7 and intensity 255, stored in the unit's pLight (+0x64), its radius handed to CUNIT_SetUnitStartLight.
//  The original inlines MONSTER_TestTypeFlag, SEED_RollRandomNumber (three times, chained) and DATATBLS_GetMonStats2TxtRecord here.
//
//  The colour: a champion's is random -- three rolls of the unit's own seed, whose low bytes are blue, green and red in that order (pushed edi,
//  ecx, eax). Every other boss takes its monstats2.txt row's Light R, G and B -- the colour MONSTER_UpdateClientLight gives an ordinary
//  monster, with radius 7 in place of the row's own. The three bytes are pushed through dl, cl and dl, the upper bytes of edx still those of the
//  data tables' pointer and of ecx those of the monstats2 array's; DLIGHTMAP_AddLight stores each into a byte of D2ClientLightStrc
//  (+0x25..+0x27), so they are not values, and the C passes the bytes.
//
//  BUG: CUNIT_GfxFreeUnitLightMap reads pMonster->pLight with no test (0x6FB296E3), and it runs before this function's own NULL test, so a NULL
//  unit faults there and the test cannot see one. UNREACHABLE: MONUNIQUE_ApplyModifiers returns for a NULL unit before any handler.
void __fastcall MONUNIQUE_ModLight(D2UnitStrc *pMonster, D2C_MonUMods nModId, int32_t bUnique)
{
    D2ClientLightStrc *pLight;
    D2MonStats2Txt *pMonStats2Txt;
    int32_t nRed;
    int32_t nGreen;
    int32_t nBlue;

    UNREFERENCED_PARAMETER(nModId);

    if (!bUnique)
        return;

    CUNIT_GfxFreeUnitLightMap(pMonster);
    //  The NULL test comes too late: a NULL unit has already faulted in CUNIT_GfxFreeUnitLightMap.
    if (!pMonster)
        return;

    if (MONSTER_TestTypeFlag(pMonster, MONTYPEFLAG_CHAMPION))
    {
        nBlue = (uint8_t)SEED_RollRandomNumber(&pMonster->pSeed);
        nGreen = (uint8_t)SEED_RollRandomNumber(&pMonster->pSeed);
        nRed = (uint8_t)SEED_RollRandomNumber(&pMonster->pSeed);
        pLight = DLIGHTMAP_AddLight(pMonster, 0, 7, 255, nRed, nGreen, nBlue);
    }
    else
    {
        pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pMonster->dwClassId);
        if (!pMonStats2Txt)
            return;

        pLight = DLIGHTMAP_AddLight(pMonster, 0, 7, 255, pMonStats2Txt->nLight_R, pMonStats2Txt->nLight_G, pMonStats2Txt->nLight_B);
    }

    pMonster->pLight = pLight;
    CUNIT_SetUnitStartLight(pMonster, DLIGHTMAP_GetRadius(pLight));
}


//1.10f: D2Client.0x6FB44E50
//  monumod.txt durieldead, slot 11: clears the monster data's dwDurielFlag (+0x18, `mov dword ptr [ecx+18h], 0`). Neither nModId nor bUnique
//  is read -- the one handler that runs for a monster without the unique flag -- and `retn 4` takes bUnique off the stack all the same, the
//  table's ABI.
void __fastcall MONUNIQUE_ModDurielDead(D2UnitStrc *pMonster, D2C_MonUMods nModId, int32_t bUnique)
{
    UNREFERENCED_PARAMETER(nModId);
    UNREFERENCED_PARAMETER(bUnique);

    if (pMonster && pMonster->dwUnitType == UNIT_MONSTER && pMonster->pMonsterData)
        pMonster->pMonsterData->dwDurielFlag = 0;
}


//1.10f: D2Client.0x6FB44E70
//  monumod.txt teleport, slot 26: MonTeleport (SKILL_MONTELEPORT, 184) at level 1, put in skill mode 4 (MONMODE_ATTACK1). AssignSkill's
//  bRemove, 1, is read only for a level of 0. nModId is not read.
void __fastcall MONUNIQUE_ModTeleport(D2UnitStrc *pMonster, D2C_MonUMods nModId, int32_t bUnique)
{
    UNREFERENCED_PARAMETER(nModId);

    if (bUnique)
    {
        D2COMMON_SKILLS_AssignSkill(pMonster, SKILL_MONTELEPORT, 1, TRUE, szMonUniqueCpp, 337);
        D2COMMON_SKILLS_SetSkillMode(D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pMonster, SKILL_MONTELEPORT), (int32_t)MONMODE_ATTACK1);
    }
}


//1.10f: D2Client.0x6FB44EB0
//  The resistance modifiers, eight slots: monumod.txt resist (8), fire (9), lightning (17), cold (18), poisonhit (23), manahit (25),
//  spectralhit (27) and stoneskin (28). Stone skin doubles the armour class first, before the count below, so it does so whatever the count is.
//  Then the six resistances are read -- fire, lightning, cold, poison, damage, magic -- and those at 100 or more counted; with two or more
//  nothing else is done. Otherwise, by id: resist adds 40 to cold, fire and lightning in that order, each only while it is under 100, stopping
//  when the count reaches two; spectralhit adds 20 the same way, each only while under 75; fire, lightning, cold and poisonhit add 75 to their
//  own, manahit 20 to magic, stone skin 50 to damage. Every compare is signed (`jl`, `jge`).
//
//  The count lives in bUnique's stack slot (`mov [esp+28h+bUnique], 0`). After the third addition of resist and of spectralhit the original
//  still counts (`cmp edi, 64h / jl`, then `inc eax / mov [esp+10h+bUnique], eax`), a store nothing reads; the C keeps it. The cases are
//  tested in the order 8, 9, 18, 17, 23, 25, 27, 28 (a `cmp ecx, N / jnz` chain), which is the C's.
void __fastcall MONUNIQUE_ModResists(D2UnitStrc *pMonster, D2C_MonUMods nModId, int32_t bUnique)
{
    int32_t nFireResist;
    int32_t nLightResist;
    int32_t nColdResist;
    int32_t nPoisonResist;
    int32_t nDamageResist;
    int32_t nMagicResist;
    int32_t nNewResist;
    int32_t nImmunityCount;

    if (!bUnique)
        return;

    if (nModId == MONUMOD_STONESKIN)                            //  28 stoneskin
    {
        D2COMMON_STATLIST_SetUnitStat(pMonster, STAT_ARMORCLASS, 2 * D2COMMON_STATLIST_UnitGetStatValue(pMonster, STAT_ARMORCLASS, 0), 0);
    }

    nImmunityCount = 0;
    nFireResist = D2COMMON_STATLIST_UnitGetStatValue(pMonster, STAT_FIRERESIST, 0);
    nLightResist = D2COMMON_STATLIST_UnitGetStatValue(pMonster, STAT_LIGHTRESIST, 0);
    nColdResist = D2COMMON_STATLIST_UnitGetStatValue(pMonster, STAT_COLDRESIST, 0);
    nPoisonResist = D2COMMON_STATLIST_UnitGetStatValue(pMonster, STAT_POISONRESIST, 0);
    nDamageResist = D2COMMON_STATLIST_UnitGetStatValue(pMonster, STAT_DAMAGERESIST, 0);
    nMagicResist = D2COMMON_STATLIST_UnitGetStatValue(pMonster, STAT_MAGICRESIST, 0);

    if (nFireResist >= 100)
        ++nImmunityCount;
    if (nLightResist >= 100)
        ++nImmunityCount;
    if (nColdResist >= 100)
        ++nImmunityCount;
    if (nPoisonResist >= 100)
        ++nImmunityCount;
    if (nDamageResist >= 100)
        ++nImmunityCount;
    if (nMagicResist >= 100)
        ++nImmunityCount;

    if (nImmunityCount >= 2)
        return;

    switch (nModId)
    {
    case MONUMOD_MAGICRESISTANT:                                //  8 resist
        if (nColdResist < 100)
        {
            nNewResist = nColdResist + 40;
            D2COMMON_STATLIST_SetUnitStat(pMonster, STAT_COLDRESIST, nNewResist, 0);
            if (nNewResist >= 100)
                ++nImmunityCount;
        }
        if (nImmunityCount >= 2)
            break;
        if (nFireResist < 100)
        {
            nNewResist = nFireResist + 40;
            D2COMMON_STATLIST_SetUnitStat(pMonster, STAT_FIRERESIST, nNewResist, 0);
            if (nNewResist >= 100)
                ++nImmunityCount;
        }
        if (nImmunityCount >= 2)
            break;
        if (nLightResist < 100)
        {
            nNewResist = nLightResist + 40;
            D2COMMON_STATLIST_SetUnitStat(pMonster, STAT_LIGHTRESIST, nNewResist, 0);
            if (nNewResist >= 100)
                ++nImmunityCount;                               //  read by nothing
        }
        break;

    case MONUMOD_FIRECHANT:                                     //  9 fire
        D2COMMON_STATLIST_SetUnitStat(pMonster, STAT_FIRERESIST, nFireResist + 75, 0);
        break;

    case MONUMOD_COLDCHANT:                                     //  18 cold
        D2COMMON_STATLIST_SetUnitStat(pMonster, STAT_COLDRESIST, nColdResist + 75, 0);
        break;

    case MONUMOD_LIGHTCHANT:                                    //  17 lightning
        D2COMMON_STATLIST_SetUnitStat(pMonster, STAT_LIGHTRESIST, nLightResist + 75, 0);
        break;

    case MONUMOD_POISONFIELD:                                   //  23 poisonhit
        D2COMMON_STATLIST_SetUnitStat(pMonster, STAT_POISONRESIST, nPoisonResist + 75, 0);
        break;

    case MONUMOD_MANABURN:                                      //  25 manahit
        D2COMMON_STATLIST_SetUnitStat(pMonster, STAT_MAGICRESIST, nMagicResist + 20, 0);
        break;

    case MONUMOD_SPECTRALHIT:                                   //  27 spectralhit
        if (nColdResist < 75)
        {
            nNewResist = nColdResist + 20;
            D2COMMON_STATLIST_SetUnitStat(pMonster, STAT_COLDRESIST, nNewResist, 0);
            if (nNewResist >= 100)
                ++nImmunityCount;
        }
        if (nImmunityCount >= 2)
            break;
        if (nFireResist < 75)
        {
            nNewResist = nFireResist + 20;
            D2COMMON_STATLIST_SetUnitStat(pMonster, STAT_FIRERESIST, nNewResist, 0);
            if (nNewResist >= 100)
                ++nImmunityCount;
        }
        if (nImmunityCount >= 2)
            break;
        if (nLightResist < 75)
        {
            nNewResist = nLightResist + 20;
            D2COMMON_STATLIST_SetUnitStat(pMonster, STAT_LIGHTRESIST, nNewResist, 0);
            if (nNewResist >= 100)
                ++nImmunityCount;                               //  read by nothing
        }
        break;

    case MONUMOD_STONESKIN:                                     //  28 stoneskin
        D2COMMON_STATLIST_SetUnitStat(pMonster, STAT_DAMAGERESIST, nDamageResist + 50, 0);
        break;
    }
}


//1.10f: D2Client.0x6FB45140
//  Runs a boss's modifiers: for a monster with at least one monumod.txt id in its data, every handler gpMonUModHandlers holds for the ids of
//  gnMonUModOrder, then for each of the first nine bytes of its id array -- each called (pMonster, the id, bUnique), bUnique its type flag 8.
//  A monster whose first id is 0 gets nothing, the fixed walk included. The original inlines MONSTER_GetMonUmodArray and
//  MONSTER_TestTypeFlag here.
//
//  The walk does not stop at a zero id: the bytes test the first byte once (`cmp byte ptr [ebp+0], 0`) and walk all nine (`cmp esi, 9 / jl`),
//  a 0 id landing on the empty slot 0.
//  The fixed walk steps a pointer and compares it `jl` -- signed -- with the address after the table: an int index strength-reduced. The C
//  keeps the index.
//
//  BUG: a byte of the id array is not bounded: it indexes the 43 slots with nothing between (`xor edx, edx / mov dl, [esi+ebp]`,
//  0x6FB451A2..0x6FB451A7), so an id of 43 or more reads past the table, into the original's next data. LATENT: the ids come from packets 0xAC
//  and 0x57 alone, and a stock server assigns none above 42.
//  DIFFERENCE: ours finds no handler for an id past the table, where the original reads the memory after it.
void __fastcall MONUNIQUE_ApplyModifiers(D2UnitStrc *pMonster)
{
    uint8_t *pUMods;
    void (__fastcall *pfnHandler)(D2UnitStrc *, D2C_MonUMods, int32_t);
    int32_t bUnique;
    int32_t nUMod;
    int32_t i;

    pUMods = MONSTER_GetMonUmodArray(pMonster);
    if (!pUMods || !pUMods[0])
        return;

    bUnique = MONSTER_TestTypeFlag(pMonster, MONTYPEFLAG_UNIQUE);

    for (i = 0; i < 4; ++i)
    {
        pfnHandler = gpMonUModHandlers[gnMonUModOrder[i]];
        if (pfnHandler)
            pfnHandler(pMonster, gnMonUModOrder[i], bUnique);
    }

    for (i = 0; i < 9; ++i)
    {
        nUMod = pUMods[i];
        //  DIFFERENCE: unbounded in the original; past the table ours finds no handler.
        pfnHandler = nUMod < 43 ? gpMonUModHandlers[nUMod] : NULL;
        if (pfnHandler)
            pfnHandler(pMonster, (D2C_MonUMods)nUMod, bUnique);
    }
}


//  Pinned numbers: the missiles.txt ids, the overlays, the classes of MONUNIQUE_MonUModCltQuestComplete's switch not pinned elsewhere and the
//  two modifier ids MONUNIQUE_MonUModCltElementalDeath compares.
static_assert(MISSILE_FIREEXPLOSION == 82 && MISSILE_MONSTERCORPSEEXPLODE == 117 && MISSILE_NOVA == 90
                  && MISSILE_FROSTNOVA == 119 && MISSILE_COLDUNIQUE == 194 && MISSILE_CORPSEPOISONCLOUD == 155
                  && MISSILE_LIGHTUNIQUE == 195 && MISSILE_QUEENPOISONCLOUD == 321,
              "MonUnique.cpp batch 2: `mov edx, 52h` (0x6FB45279), `mov edx, 75h` (0x6FB453B2), `push 5Ah` "
              "(0x6FB456FA, 0x6FB45715), `push 77h` (0x6FB457CD), `push 0C2h` (0x6FB457E8), `mov edx, 9Bh` "
              "(0x6FB457AE), `cmp esi, 0C3h` (0x6FB4597D), `cmp esi, 141h` (0x6FB45A22)");
static_assert(OVERLAY_DURIELDEAD_FWD == 0x34 && OVERLAY_DURIELDEAD_REAR == 0x9C && MONSTER_CORRUPTROGUE3 == 45
                  && MONSTER_RADAMENT == 229 && MONSTER_BLOODRAVEN == 267 && MONSTER_COMPELLINGORB == 366
                  && MONSTER_OVERSEER1 == 479 && MONSTER_ANCIENTBARB2 == 541 && MONSTER_ANCIENTBARB3 == 542
                  && MONUMOD_POISDEATH == 10 && MONUMOD_NOVADEATH == 42,
              "MonUnique.cpp batch 2: `mov edx, 34h` (0x6FB4584C), `mov edx, 9Ch` (0x6FB45864); `sub eax, 2Dh` "
              "(0x6FB454BC) and the indirect table's case 229, `sub eax, 10Bh` (0x6FB45567), `sub eax, 52h` "
              "(0x6FB45573), `cmp eax, 1DFh` (0x6FB4555E), `cmp eax, 21Eh` (0x6FB455BA); `cmp edi, 0Ah` "
              "(0x6FB45746), `cmp edi, 2Ah` (0x6FB456E8)");


//1.10f: D2Client.0x6FB451D0
//  monumod.txt goboom (31), gpMonUModCltFuncs column 2 -- the column MONSTER_UpdateAnimMode runs after MONSTER_SpawnAttackMissile. On the
//  fourth frame of the death mode it draws five fire explosions (missiles.txt fireexplosion, 82) through MISSILE_ClientSpawnSimple, skill 0
//  level 1, on five points -- (x, y), (x+1, y+1), (x+1, y-1), (x-1, y+1), (x-1, y-1), in that order -- and plays sound 2458 (sounds.txt
//  necromancer_corpseexp_1). nModId is not read. The original inlines UNITS_GetXPosition here.
//
//  The tests in the bytes' order: bUnique; the frame, from D2Common's #10379 UNITS_GetShiftedFrameMetrics; `pUnit && dwAnimMode` returns --
//  written as the mode read, `pUnit ? pUnit->dwAnimMode : 0`, the inlined mode accessor -- and a frame other than 4 returns. #10379 asserts
//  its unit and exits, so the NULL test after it cannot see a NULL, and the dispatcher never passes one.
void __fastcall MONUNIQUE_MonUModCltGoBoom(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique)
{
    int32_t nFrame;
    int32_t nFrameCount;
    int32_t nX;
    int32_t nY;

    UNREFERENCED_PARAMETER(nModId);

    if (!bUnique)
        return;

    D2COMMON_UNITS_GetShiftedFrameMetrics(pUnit, &nFrame, &nFrameCount);
    if ((pUnit ? pUnit->dwAnimMode : 0) != MONMODE_DEATH)
        return;

    if (nFrame != 4)
        return;

    nX = UNITS_GetXPosition(pUnit);
    nY = UNITS_GetYPosition(pUnit);

    MISSILE_ClientSpawnSimple(pUnit, MISSILE_FIREEXPLOSION, nX, nY, SKILL_ATTACK, 1);
    MISSILE_ClientSpawnSimple(pUnit, MISSILE_FIREEXPLOSION, nX + 1, nY + 1, SKILL_ATTACK, 1);
    MISSILE_ClientSpawnSimple(pUnit, MISSILE_FIREEXPLOSION, nX + 1, nY - 1, SKILL_ATTACK, 1);
    MISSILE_ClientSpawnSimple(pUnit, MISSILE_FIREEXPLOSION, nX - 1, nY + 1, SKILL_ATTACK, 1);
    MISSILE_ClientSpawnSimple(pUnit, MISSILE_FIREEXPLOSION, nX - 1, nY - 1, SKILL_ATTACK, 1);
    SOUNDINST_PlaySound(SOUND_NECROMANCER_CORPSEEXP_1, pUnit, 0, 0, 0);                   //  sounds.txt necromancer_corpseexp_1
}


//1.10f: D2Client.0x6FB45300
//  monumod.txt fire (9), column 2: MONUNIQUE_MonUModCltGoBoom with a quincunx of monstercorpseexplode (117) before its fireexplosion (82) one
//  -- the same five points in the same order for each -- and the same sound. The tests are GoBoom's, with edi zeroed first and compared where
//  GoBoom tests (`xor edi, edi`, `cmp eax, edi`). nModId is not read. The original inlines UNITS_GetXPosition here.
void __fastcall MONUNIQUE_MonUModCltFire(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique)
{
    int32_t nFrame;
    int32_t nFrameCount;
    int32_t nX;
    int32_t nY;

    UNREFERENCED_PARAMETER(nModId);

    if (!bUnique)
        return;

    D2COMMON_UNITS_GetShiftedFrameMetrics(pUnit, &nFrame, &nFrameCount);
    if ((pUnit ? pUnit->dwAnimMode : 0) != MONMODE_DEATH)
        return;

    if (nFrame != 4)
        return;

    nX = UNITS_GetXPosition(pUnit);
    nY = UNITS_GetYPosition(pUnit);

    MISSILE_ClientSpawnSimple(pUnit, MISSILE_MONSTERCORPSEEXPLODE, nX, nY, SKILL_ATTACK, 1);
    MISSILE_ClientSpawnSimple(pUnit, MISSILE_MONSTERCORPSEEXPLODE, nX + 1, nY + 1, SKILL_ATTACK, 1);
    MISSILE_ClientSpawnSimple(pUnit, MISSILE_MONSTERCORPSEEXPLODE, nX + 1, nY - 1, SKILL_ATTACK, 1);
    MISSILE_ClientSpawnSimple(pUnit, MISSILE_MONSTERCORPSEEXPLODE, nX - 1, nY + 1, SKILL_ATTACK, 1);
    MISSILE_ClientSpawnSimple(pUnit, MISSILE_MONSTERCORPSEEXPLODE, nX - 1, nY - 1, SKILL_ATTACK, 1);
    MISSILE_ClientSpawnSimple(pUnit, MISSILE_FIREEXPLOSION, nX, nY, SKILL_ATTACK, 1);
    MISSILE_ClientSpawnSimple(pUnit, MISSILE_FIREEXPLOSION, nX + 1, nY + 1, SKILL_ATTACK, 1);
    MISSILE_ClientSpawnSimple(pUnit, MISSILE_FIREEXPLOSION, nX + 1, nY - 1, SKILL_ATTACK, 1);
    MISSILE_ClientSpawnSimple(pUnit, MISSILE_FIREEXPLOSION, nX - 1, nY + 1, SKILL_ATTACK, 1);
    MISSILE_ClientSpawnSimple(pUnit, MISSILE_FIREEXPLOSION, nX - 1, nY - 1, SKILL_ATTACK, 1);
    SOUNDINST_PlaySound(SOUND_NECROMANCER_CORPSEEXP_1, pUnit, 0, 0, 0);                   //  sounds.txt necromancer_corpseexp_1
}


//1.10f: D2Client.0x6FB454A0
//  monumod.txt questcomplete (22), column 1 -- the column MONSTER_DoClientAction runs after its velocity block, whatever the action. A switch
//  on the class, an arm per quest monster, each calling the quest effect in the death mode: the Countess's tower chest (45, corruptrogue3, her
//  base class), Andariel (156), Duriel (211), Radament (229), Mephisto (242), Diablo (243, QUESTSFX_SetWatchedUnit), Izual (256), Blood Raven
//  (267), the Compelling Orb (366), the Overseer (479), the three Ancients (540..542) and Baal (544, baalcrab) -- and the Maggot Queen (284),
//  whose arm keys on MONMODE_SEQUENCE, 14 (`cmp dword ptr [ecx+10h], 0Eh`), not on the death mode. Neither nModId nor bUnique is read. Each arm
//  ends `call / retn 4` with the unit still in ecx; QUESTSFX_OnBaalDeath takes nothing.
//
//  MSVC's switch: split on 256 (`cmp eax, 100h / ja / jz`), a byte table for 45..243 (`ja` past 0C6h), a `sub` chain for 267, 284 and 366,
//  479 compared alone, the Ancients a `jb`/`jbe` range and 544 compared alone -- every compare unsigned, on the unsigned class. The C writes
//  the switch.
void __fastcall MONUNIQUE_MonUModCltQuestComplete(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique)
{
    UNREFERENCED_PARAMETER(nModId);
    UNREFERENCED_PARAMETER(bUnique);

    if (!pUnit)
        return;

    switch (pUnit->dwClassId)
    {
    case MONSTER_CORRUPTROGUE3:
        if (pUnit->dwAnimMode == MONMODE_DEATH)
            QUESTSFX_SpawnTowerChestEffect(pUnit);
        break;
    case MONSTER_ANDARIEL:
        if (pUnit->dwAnimMode == MONMODE_DEATH)
            QUESTSFX_OnAndarielDeath(pUnit);
        break;
    case MONSTER_DURIEL:
        if (pUnit->dwAnimMode == MONMODE_DEATH)
            QUESTSFX_OnDurielDeath(pUnit);
        break;
    case MONSTER_RADAMENT:
        if (pUnit->dwAnimMode == MONMODE_DEATH)
            QUESTSFX_OnRadamentDeath(pUnit);
        break;
    case MONSTER_MEPHISTO:
        if (pUnit->dwAnimMode == MONMODE_DEATH)
            QUESTSFX_OnMephistoDeath(pUnit);
        break;
    case MONSTER_DIABLO:
        if (pUnit->dwAnimMode == MONMODE_DEATH)
            QUESTSFX_SetWatchedUnit(pUnit);
        break;
    case MONSTER_IZUAL:
        if (pUnit->dwAnimMode == MONMODE_DEATH)
            QUESTSFX_OnIzualDeath(pUnit);
        break;
    case MONSTER_BLOODRAVEN:
        if (pUnit->dwAnimMode == MONMODE_DEATH)
            QUESTSFX_OnBloodRavenDeath(pUnit);
        break;
    case MONSTER_MAGGOTQUEEN1:
        if (pUnit->dwAnimMode == MONMODE_SEQUENCE)
            QUESTSFX_OnMaggotQueenDeath(pUnit);
        break;
    case MONSTER_COMPELLINGORB:
        if (pUnit->dwAnimMode == MONMODE_DEATH)
            QUESTSFX_OnCompellingOrbDeath(pUnit);
        break;
    case MONSTER_OVERSEER1:
        if (pUnit->dwAnimMode == MONMODE_DEATH)
            QUESTSFX_OnOverseerDeath(pUnit);
        break;
    case MONSTER_ANCIENTBARB1:
    case MONSTER_ANCIENTBARB2:
    case MONSTER_ANCIENTBARB3:
        if (pUnit->dwAnimMode == MONMODE_DEATH)
            QUESTSFX_OnAncientDeath(pUnit);
        break;
    case MONSTER_BAALCRAB:
        if (pUnit->dwAnimMode == MONMODE_DEATH)
            QUESTSFX_OnBaalDeath();
        break;
    }
}


//1.10f: D2Client.0x6FB456D0
//  Three rows, column 2: monumod.txt poisondead (10), cold (18) and lightningdeath (42), told apart by nModId (MONUMOD_POISDEATH,
//  MONUMOD_COLDCHANT, MONUMOD_NOVADEATH). The original inlines UNITS_GetYPosition here. In the bytes' order:
//    * bUnique is read once: 0 returns for cold and nothing else (`test eax, eax / jnz`, `cmp edi, 12h / jz`).
//    * lightningdeath, before the frame is read and on `!pUnit || dwAnimMode == 0` alone: a ring of nova (90) at nova's own velocity
//      (D2COMMON_DATATBLS_GetMissileVelocityFromMissilesTxt, called first) at the monster's STAT_LEVEL, through
//      SKILLS_SpawnMissileRingFromUnit with no alternate missile and skill 0 -- with no frame test, on every run of the column in the death
//      mode.
//    * then the frame, into bUnique's own slot (`lea ecx, [esp+0Ch+bUnique]`; the argument is dead by then), and on frame 4 of the death mode:
//      poisondead spawns corpsepoisoncloud (155) at the unit through MISSILE_ClientSpawnForOwner, skill 0, the monster's level -- its Y read
//      before its X, as the C reads them; cold a ring of coldunique (194) at frostnova's (119) velocity, at the monster's level.
//  The NULL tests after #10379 cannot see a NULL, since #10379 asserts its unit. Both of the client's rings take the full STAT_LEVEL, and its
//  nova ignores bUnique.
void __fastcall MONUNIQUE_MonUModCltElementalDeath(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique)
{
    int32_t nFrame;
    int32_t nFrameCount;
    int32_t nVelocity;
    int32_t nLevel;
    int32_t nX;
    int32_t nY;

    if (!bUnique && nModId == MONUMOD_COLDCHANT)
        return;

    if (nModId == MONUMOD_NOVADEATH && (pUnit ? pUnit->dwAnimMode : 0) == MONMODE_DEATH)
    {
        nVelocity = D2COMMON_DATATBLS_GetMissileVelocityFromMissilesTxt(MISSILE_NOVA, 0);
        nLevel = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_LEVEL, 0);
        SKILLS_SpawnMissileRingFromUnit(pUnit, pUnit, MISSILE_NOVA, (D2C_MissilesHcIdx)-1, SKILL_ATTACK, nLevel, nVelocity);
    }

    D2COMMON_UNITS_GetShiftedFrameMetrics(pUnit, &nFrame, &nFrameCount);
    if ((pUnit ? pUnit->dwAnimMode : 0) != MONMODE_DEATH)
        return;

    if (nFrame != 4)
        return;

    if (nModId == MONUMOD_POISDEATH)
    {
        nY = UNITS_GetYPosition(pUnit);
        nX = UNITS_GetXPosition(pUnit);
        nLevel = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_LEVEL, 0);
        MISSILE_ClientSpawnForOwner(pUnit, MISSILE_CORPSEPOISONCLOUD, nX, nY, SKILL_ATTACK, nLevel);
        return;
    }

    if (nModId == MONUMOD_COLDCHANT)
    {
        nVelocity = D2COMMON_DATATBLS_GetMissileVelocityFromMissilesTxt(MISSILE_FROSTNOVA, 0);
        nLevel = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_LEVEL, 0);
        SKILLS_SpawnMissileRingFromUnit(pUnit, pUnit, MISSILE_COLDUNIQUE, (D2C_MissilesHcIdx)-1, SKILL_ATTACK, nLevel, nVelocity);
    }
}


//1.10f: D2Client.0x6FB45800
//  monumod.txt durieldead (11), column 2 -- Duriel's corpse. The unit is tested as a monster with data first, as MONUNIQUE_ModDurielDead's is.
//  The original inlines SEED_RollRandomNumber and UNITS_GetXPosition here.
//    * In the death mode, on frame 13: every overlay off (GFX_RemoveAllOverlays) and the two halves of Duriel's death on,
//      OVERLAY_DURIELDEAD_FWD (52) and OVERLAY_DURIELDEAD_REAR (156), both OVERLAYTYPE_STOP, then the unit's nActionFrame cleared
//      (`mov byte ptr [esi+4Eh], 0`).
//    * In the corpse mode, MONMODE_DEAD (12): one roll of the unit's seed, and on a remainder of 0 of 100 -- one roll in a hundred -- while the
//      monster data's dwDurielFlag is under 100, a maggot (monstats.txt maggot, 227) through UNITS_FindFreeSpotForMonster with 4, and
//      dwDurielFlag += 4: 25 maggots from 0, the value MONUNIQUE_ModDurielDead sets. The bound is compared `jge`, signed, on a field
//      D2Structs.h types unsigned: the C casts.
//  Neither nModId nor bUnique is read.
//
//  Before the maggot an X and a Y are read and thrown away -- the calls of D2COMMON_PATH_GetXPosition and _GetYPosition stay, their answers
//  unused, and UNITS_FindFreeSpotForMonster takes no coordinate.
//  The roll is an unsigned 32-bit remainder of the low half (`mov ecx, 64h`, `xor edx, edx`, `div ecx`), so the C casts the 64-bit answer
//  before the `%`.
void __fastcall MONUNIQUE_MonUModCltDurielDead(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique)
{
    D2MonsterDataStrc *pMonsterData;
    int32_t nFrame;
    int32_t nFrameCount;

    UNREFERENCED_PARAMETER(nModId);
    UNREFERENCED_PARAMETER(bUnique);

    if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER)
        return;

    pMonsterData = pUnit->pMonsterData;
    if (!pMonsterData)
        return;

    if (pUnit->dwAnimMode == MONMODE_DEATH)
    {
        D2COMMON_UNITS_GetShiftedFrameMetrics(pUnit, &nFrame, &nFrameCount);
        if (nFrame != 13)
            return;

        GFX_RemoveAllOverlays(pUnit);
        GFX_AddOverlay(pUnit, OVERLAY_DURIELDEAD_FWD, OVERLAYTYPE_STOP, 0, 0, 0, 0, 0);
        GFX_AddOverlay(pUnit, OVERLAY_DURIELDEAD_REAR, OVERLAYTYPE_STOP, 0, 0, 0, 0, 0);
        pUnit->nActionFrame = 0;
        return;
    }

    if (pUnit->dwAnimMode != MONMODE_DEAD)
        return;

    if ((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 100 != 0)
        return;

    if ((int32_t)pMonsterData->dwDurielFlag >= 100)
        return;

    UNITS_GetXPosition(pUnit);
    UNITS_GetYPosition(pUnit);
    UNITS_FindFreeSpotForMonster(pUnit, MONSTER_MAGGOT, 4);
    pMonsterData->dwDurielFlag += 4;
}


//1.10f: D2Client.0x6FB45910
//  Two rows: monumod.txt lightning (17) in column 2 and poisonhit (23) in column 0 -- the column MONSTER_DoClientAction runs when an action
//  other than a hit or a skill brings a parameter block. nModId is not read. On bUnique: the frame -- into bUnique's own slot, dead by then
//  (`lea ecx, [esp+8+bUnique]`) -- then, for a unit (the test cannot fail) in MONMODE_GETHIT (3) on frame 2, the ring
//  MONUNIQUE_MonUModSpawnAuraMissileRing of lightunique (195) at half the monster's STAT_LEVEL, an arithmetic shift (`sar eax, 1`).
//
//  BUG: the client's slot for row 23 column 0 holds this function: a poison-hit monster's hook draws lightunique, and only for a monster
//  carrying the type bit 0x100, which the server sets in the lightning hook alone -- so it draws nothing for a monster that is poison-hit and
//  not lightning-enchanted, where the server's row 23 column 0 casts a ring of eight queenpoisoncloud (321). REACHABLE: monumod.txt gives
//  poisonhit no pick weight, but SuperUniques.txt's Baal Subject 2 carries it as Mod1, alone, so the client runs this hook for a stock boss and
//  draws nothing where the server casts its cloud. Whether column 0 runs while that boss is in the get-hit mode was not traced.
void __fastcall MONUNIQUE_MonUModCltGetHitOverlay(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique)
{
    int32_t nFrame;
    int32_t nFrameCount;

    UNREFERENCED_PARAMETER(nModId);

    if (!bUnique)
        return;

    D2COMMON_UNITS_GetShiftedFrameMetrics(pUnit, &nFrame, &nFrameCount);
    if (!pUnit)
        return;

    //  BUG: this function is also the hook of row 23 column 0 (poisonhit).
    if (pUnit->dwAnimMode != MONMODE_GETHIT || nFrame != 2)
        return;

    MONUNIQUE_MonUModSpawnAuraMissileRing(pUnit, MISSILE_LIGHTUNIQUE, D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_LEVEL, 0) >> 1);
}


//1.10f: D2Client.0x6FB45960
//  A ring of eight: nMissileId twice at each of the four points one subtile from the unit -- (x, y-1), (x+1, y), (x, y+1), (x-1, y), the
//  offsets {0, 1, 0, -1} and {-1, 0, 1, 0} stored as locals -- the two of a point told apart by pInitArgs 0 and 1 (the argument of pInitFunc).
//  Three callers: MONUNIQUE_MonUModCltGetHitOverlay and MONUNIQUE_MonUModCltLightning with lightunique (195, `mov edx, 0C3h`) and
//  MONUNIQUE_MonUModCltScarab with buglightning (225, `mov edx, 0E1h`). The original inlines MONSTER_TestTypeFlag, UNITS_GetXPosition and
//  UNITS_GetYPosition (twice) here.
//    * A level of 0 becomes 1; nothing else is clamped. The callers halve the level with a bare `sar`, which rounds a negative level down.
//    * Lightunique is drawn only for a monster carrying type bit 0x100 -- +0x17's bit 0, which MONSTER_DoClientAction sets and clears from a bit
//      of the server's parameter block (MONSTER_SetAuraRingEnabled). The server's MONSTERUNIQUE_CastLightUniqueMissile sets that bit when it
//      fires its own ring and clears it when it is throttled -- fewer than ten game frames since the last, the frame kept in +0x18. So the
//      client draws the bolts the server has fired. Every other id is drawn.
//    * The parameters: D2MissileStrc zeroed whole (`rep stosd`, 17h dwords, 92 bytes), MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS, the
//      owner, the unit's coordinates as the start, skill 0, the level, the missile, and SKILLS_MissileStartSeededPath as pInitFunc for every id
//      but queenpoisoncloud (321) -- an arm no caller reaches.
//    * Each point reads the unit's coordinates again; the point walk steps a byte offset by 4 up to 16 (`add eax, 4 / cmp eax, 10h / jl`), an
//      int index strength-reduced, which the C keeps as the index; the two spawns per point compare `jl` 2.
//
//  BUG: a NULL unit with an id other than 195 is read at once (0x6FB459BA, the first coordinate). UNREACHABLE: the three callers are hooks of
//  gpMonUModCltFuncs, which MONUNIQUE_MonUModRunCltFuncsForUnit alone calls, and it returns for a NULL unit.
//  The arm for 321 (queenpoisoncloud) is UNREACHABLE: the three callers pass 195 and 225.
void __fastcall MONUNIQUE_MonUModSpawnAuraMissileRing(D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId, int32_t nSkillLevel)
{
    D2MissileStrc tParams;
    int32_t aRingOffsetX[4];
    int32_t aRingOffsetY[4];
    int32_t nLevel;
    int32_t nX;
    int32_t nY;
    int32_t i;
    int32_t j;

    nLevel = nSkillLevel;
    if (!nLevel)
        nLevel = 1;

    if (nMissileId == MISSILE_LIGHTUNIQUE && !MONSTER_TestTypeFlag(pUnit, MONTYPEFLAG_LIGHTNINGUNIQUE))
        return;

    memset(&tParams, 0, sizeof(tParams));
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS;
    tParams.pOwner = pUnit;
            //  BUG: for a NULL pUnit the first coordinate is read here (0x6FB459BA).
    tParams.nX = UNITS_GetXPosition(pUnit);
    tParams.nY = UNITS_GetYPosition(pUnit);
    tParams.nSkill = 0;
    tParams.nSkillLevel = nLevel;
    tParams.nMissile = nMissileId;
    if (nMissileId != MISSILE_QUEENPOISONCLOUD)
        tParams.pInitFunc = SKILLS_MissileStartSeededPath;

    aRingOffsetX[0] = 0;
    aRingOffsetX[1] = 1;
    aRingOffsetX[2] = 0;
    aRingOffsetX[3] = -1;
    aRingOffsetY[0] = -1;
    aRingOffsetY[1] = 0;
    aRingOffsetY[2] = 1;
    aRingOffsetY[3] = 0;

    for (i = 0; i < 4; ++i)
    {
        nX = UNITS_GetXPosition(pUnit) + aRingOffsetX[i];
        nY = UNITS_GetYPosition(pUnit) + aRingOffsetY[i];

        for (j = 0; j < 2; ++j)
        {
            tParams.pInitArgs = j;
            tParams.nTargetX = nX;
            tParams.nTargetY = nY;
            MISSILE_ClientSpawn(&tParams);
        }
    }
}


//1.10f: D2Client.0x6FB45B20
//  monumod.txt lightning (17), column 3 -- the column MONSTER_DoClientAction runs after a hit, with the mode just set to MONMODE_GETHIT, and
//  after a knockback. On bUnique, for a unit not in the get-hit mode: the lightunique ring at half the monster's STAT_LEVEL (`sar eax, 1`) --
//  which MONUNIQUE_MonUModCltGetHitOverlay draws in that mode, on its frame 2. nModId is not read. A NULL unit would pass the test and reach
//  D2Common's STATLIST_UnitGetStatValue, which answers 0 for NULL; the dispatcher never passes one.
void __fastcall MONUNIQUE_MonUModCltLightning(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique)
{
    UNREFERENCED_PARAMETER(nModId);

    if (!bUnique)
        return;

    if ((pUnit ? pUnit->dwAnimMode : 0) == MONMODE_GETHIT)
        return;

    MONUNIQUE_MonUModSpawnAuraMissileRing(pUnit, MISSILE_LIGHTUNIQUE, D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_LEVEL, 0) >> 1);
}


//  Pinned numbers: the missiles.txt ids, the nomultishot flag's bit, and the monumod.txt rows gpMonUModCltFuncs fills.
static_assert(MISSILE_BUGLIGHTNING == 225 && MISSILE_SUICIDECORPSEEXPLODE == 426
                  && MISSILE_SUICIDEFIREEXPLODE == 427 && MISSILE_SUICIDEICEEXPLODE == 428
                  && MISSILE_PAIN_WORM_APPEAR == 545 && MISSILESFLAGINDEX_NOMULTISHOT == 12,
              "MonUnique.cpp batch 3: `mov edx, 0E1h` (0x6FB45B97), `mov edx, 1AAh` (0x6FB46057), `mov edx, 1ABh` "
              "(0x6FB45F71), `mov edx, 1ACh` (0x6FB45FE9), `mov edx, 221h` (0x6FB460DF, 0x6FB4617F); byte 1 of "
              "dwMissileFlags (`mov cl, [eax+5]`, 0x6FB45C32) against gdwBitMasks[4] (`test [edx+10h], cl`, 0x6FB45C35)");
static_assert(MONUMOD_CHARGEDBOLTS == 20 && MONUMOD_QUESTMOD == 22 && MONUMOD_MULTISHOT == 29
                  && MONUMOD_CORPSEBOOMDEATH == 31 && MONUMOD_FIREBOOMDEATH == 32 && MONUMOD_FREEZINDEATH == 33
                  && MONUMOD_PAINWORMDEATH == 40,
              "MonUnique.cpp batch 3: gpMonUModCltFuncs' rows scarab, questcomplete, multishot, goboom, "
              "firespike_explode, suicideminion_explode and worms_on_death (excel.py monumod.txt)");


//1.10f: D2Client.0x6FB45B60
//  monumod.txt scarab (20), column 2. The frame first, from D2Common's #10379; then, for a unit in MONMODE_GETHIT or MONMODE_DEATH (`cmp eax,
//  3 / jz`, `test eax, eax / jnz`) on its frame 2, the ring MONUNIQUE_MonUModSpawnAuraMissileRing of buglightning (225) at the monster's full
//  STAT_LEVEL -- no shift, where the lightunique callers halve it. Neither nModId nor bUnique is read: nothing in the body touches the stack
//  argument `retn 4` purges. The NULL test after #10379 cannot see a NULL.
void __fastcall MONUNIQUE_MonUModCltScarab(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique)
{
    int32_t nFrame;
    int32_t nFrameCount;

    UNREFERENCED_PARAMETER(nModId);
    UNREFERENCED_PARAMETER(bUnique);

    D2COMMON_UNITS_GetShiftedFrameMetrics(pUnit, &nFrame, &nFrameCount);
    if (pUnit && pUnit->dwAnimMode != MONMODE_GETHIT && pUnit->dwAnimMode != MONMODE_DEATH)
        return;

    if (nFrame != 2)
        return;

    MONUNIQUE_MonUModSpawnAuraMissileRing(pUnit, MISSILE_BUGLIGHTNING, D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_LEVEL, 0));
}


//1.10f: D2Client.0x6FB45BB0
//  monumod.txt multishot (29), column 4 -- the column MISSILE_ClientSpawn runs at its end, through MONUNIQUE_MonUModRunCltFuncsOnMissileSpawn,
//  for a missile with an owner: ecx is the MISSILE, and bUnique its owner's type flag 8. A boss with the modifier fires two more of each
//  missile, one either side of its target. The original inlines the missiles.txt record lookup, MONSTER_TestTypeFlag, MONSTER_SetTypeFlag
//  (twice), UNITS_GetXPosition and UNITS_GetYPosition (twice) here. In the bytes' order:
//    * bUnique, a missile, its owner (CUNIT_GetOwnerUnit), its missiles.txt row, and the row's nomultishot flag clear: bit 12,
//      MISSILESFLAGINDEX_NOMULTISHOT, read as byte 1 of dwMissileFlags against gdwBitMasks[4] (`mov cl, [eax+5] / test [edx+10h], cl`);
//    * the owner's type bit 0x80 clear (MONTYPEFLAG_MULTISHOT). It is set around the two spawns below, whose own column-4 run stops on it, so
//      the two make no more;
//    * the target: the owner's path target's coordinates, or with none the target of the MISSILE's own path -- D2Common #10175 and #10176 read
//      the words at +0x10 and +0x12, tTargetCoord, whatever "PathGetFirstPoint" says. The path is read before CUNIT_GetPathTargetUnit is
//      called, and it is not NULL: MISSILE_ClientSpawn removes a missile with no path before it runs this column;
//    * a seed, built and then never read: SEED_InitSeed, SEED_InitLowSeed with 'SEIS' (0x53454953), its high half the owner's dwUnitId;
//    * the direction, owner minus target, each axis cut to -1, 0 or 1 (a sign function), and both 0 for the mummies' missiles, mummy1..mummy4
//      (63..66, `jl` and `jg`) -- theirs land on the target;
//    * 0x80 on, two MISSILE_ClientSpawnForOwner of the same missile at (x - dy, y + dx) and (x + dy, y - dx) -- the target moved one step
//      across the line from the owner, either way -- each with the skill and the level the missile carries (D2Common #11127 and #11120, the
//      level asked first), and 0x80 off.
//  nModId is not read.
void __fastcall MONUNIQUE_MonUModCltMultishotSpawn(D2UnitStrc *pMissile, D2C_MonUMods nModId, int32_t bUnique)
{
    D2UnitStrc *pOwner;
    D2UnitStrc *pTarget;
    D2MissilesTxt *pMissilesTxt;
    D2DynamicPathStrc *pMissilePath;
    D2SeedStrc tSeed;
    int32_t nMissileId;
    int32_t nTargetX;
    int32_t nTargetY;
    int32_t nDirX;
    int32_t nDirY;
    int32_t nSkillLevel;
    int32_t nSkillId;

    UNREFERENCED_PARAMETER(nModId);

    if (!bUnique || !pMissile || pMissile->dwUnitType != UNIT_MISSILE)
        return;

    nMissileId = pMissile->dwClassId;
    pOwner = CUNIT_GetOwnerUnit(pMissile);
    if (!pOwner)
        return;

    pMissilesTxt = NULL;
    if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
        pMissilesTxt = &sgptDataTables->pMissilesTxt[nMissileId];
    if (!pMissilesTxt)
        return;

    if ((((const uint8_t *)&pMissilesTxt->dwMissileFlags)[MISSILESFLAGINDEX_NOMULTISHOT >> 3] & (uint8_t)gdwBitMasks[MISSILESFLAGINDEX_NOMULTISHOT & 7]) != 0)
        return;

    if (MONSTER_TestTypeFlag(pOwner, MONTYPEFLAG_MULTISHOT))
        return;

    pMissilePath = pMissile->pDynamicPath;
    pTarget = CUNIT_GetPathTargetUnit(pOwner);
    if (pTarget)
    {
        nTargetX = UNITS_GetXPosition(pTarget);
        nTargetY = UNITS_GetYPosition(pTarget);
    }
    else
    {
        nTargetX = D2COMMON_10175_PathGetFirstPointX(pMissilePath);
        nTargetY = D2COMMON_10176_PathGetFirstPointY(pMissilePath);
    }

    //  Read by nothing after this.
    D2COMMON_SEED_InitSeed(&tSeed);
    D2COMMON_SEED_InitLowSeed(&tSeed, 0x53454953);              //  'SEIS'
    tSeed.nHighSeed = pOwner->dwUnitId;

    nDirX = UNITS_GetXPosition(pOwner) - nTargetX;
    nDirY = UNITS_GetYPosition(pOwner) - nTargetY;
    if (nDirX < 0)
        nDirX = -1;
    else if (nDirX > 0)
        nDirX = 1;

    if (nDirY < 0)
        nDirY = -1;
    else if (nDirY > 0)
        nDirY = 1;

    if (nMissileId >= MISSILE_MUMMY1 && nMissileId <= MISSILE_MUMMY4)
    {
        nDirY = 0;
        nDirX = 0;
    }

    MONSTER_SetTypeFlag(pOwner, MONTYPEFLAG_MULTISHOT, TRUE);

    nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    MISSILE_ClientSpawnForOwner(pOwner, nMissileId, nTargetX - nDirY, nTargetY + nDirX, nSkillId, nSkillLevel);

    nSkillLevel = (int32_t)D2COMMON_MISSILE_GetLevel(pMissile);
    nSkillId = D2COMMON_MISSILE_GetSkill(pMissile);
    MISSILE_ClientSpawnForOwner(pOwner, nMissileId, nTargetX + nDirY, nTargetY - nDirX, nSkillId, nSkillLevel);

    MONSTER_SetTypeFlag(pOwner, MONTYPEFLAG_MULTISHOT, FALSE);
}


//1.10f: D2Client.0x6FB45E10
//  monumod.txt firespike_explode (32), column 2. On frame 4 of the death mode, one explodingarrowexp (42, `mov edx, 2Ah`) at the unit through
//  MISSILE_ClientSpawnForOwner, skill 0, level 1 -- the unit's Y read before its X, as the C reads them. Neither nModId nor bUnique is read;
//  the NULL test after #10379 cannot see a NULL. The original inlines UNITS_GetYPosition here.
void __fastcall MONUNIQUE_MonUModCltFireSpikeExplode(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique)
{
    int32_t nFrame;
    int32_t nFrameCount;
    int32_t nX;
    int32_t nY;

    UNREFERENCED_PARAMETER(nModId);
    UNREFERENCED_PARAMETER(bUnique);

    D2COMMON_UNITS_GetShiftedFrameMetrics(pUnit, &nFrame, &nFrameCount);
    if ((pUnit ? pUnit->dwAnimMode : 0) != MONMODE_DEATH)
        return;

    if (nFrame != 4)
        return;

    nY = UNITS_GetYPosition(pUnit);
    nX = UNITS_GetXPosition(pUnit);
    MISSILE_ClientSpawnForOwner(pUnit, MISSILE_EXPLODINGARROWEXP, nX, nY, SKILL_ATTACK, 1);
}


//1.10f: D2Client.0x6FB45EB0
//  monumod.txt suicideminion_explode (33), column 2. First the monster's monstats.txt row and its first AI parameter for the difficulty,
//  wAiParam[0][GAME_GetDifficulty()], sign-extended (`movsx edi, word ptr [edi+eax*2+56h]`); then, on frame 4 of the death mode, explosions at
//  the unit through MISSILE_ClientSpawnForOwner, skill 0, level 1, each unit's Y read before its X:
//    * a parameter of 1: suicidefireexplode (427);
//    * 2 or more (`cmp edi, 2 / jl`, signed): suicideiceexplode (428);
//    * then, after either and alone for a parameter under 1: suicidecorpseexplode (426).
//  The two arms meet at one call, each loading its missile id into edx on the way -- `mov edx, 1ABh` on the first arm's three paths,
//  `mov edx, 1ACh` on the second's: the compiler's merge of two calls the C writes apart. Neither nModId nor bUnique is read.
//  The original inlines DATATBLS_GetMonStatsTxtRecord and UNITS_GetYPosition here.
//
//  BUG: the row is read with no NULL test. For a NULL unit or a class outside monstats.txt the lookup answers NULL and the read faults at
//  0x56 + 2 * the difficulty (0x6FB45EF7), before #10379's assert, which would have stopped a NULL unit. UNREACHABLE: the class is the unit's
//  own, bounded where the unit is made, and the dispatcher never passes a NULL unit.
void __fastcall MONUNIQUE_MonUModCltSuicideMinionExplode(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique)
{
    D2MonStatsTxt *pMonStatsTxt;
    int32_t nAiParam0;
    int32_t nFrame;
    int32_t nFrameCount;
    int32_t nX;
    int32_t nY;

    UNREFERENCED_PARAMETER(nModId);
    UNREFERENCED_PARAMETER(bUnique);

    pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pUnit ? pUnit->dwClassId : -1);
            //  BUG: pMonStatsTxt is read with no NULL test (0x6FB45EF7).
    nAiParam0 = pMonStatsTxt->wAiParam[0][GAME_GetDifficulty()];

    D2COMMON_UNITS_GetShiftedFrameMetrics(pUnit, &nFrame, &nFrameCount);
    if ((pUnit ? pUnit->dwAnimMode : 0) != MONMODE_DEATH)
        return;

    if (nFrame != 4)
        return;

    if (nAiParam0 == 1)
    {
        nY = UNITS_GetYPosition(pUnit);
        nX = UNITS_GetXPosition(pUnit);
        MISSILE_ClientSpawnForOwner(pUnit, MISSILE_SUICIDEFIREEXPLODE, nX, nY, SKILL_ATTACK, 1);
    }
    else if (nAiParam0 >= 2)
    {
        nY = UNITS_GetYPosition(pUnit);
        nX = UNITS_GetXPosition(pUnit);
        MISSILE_ClientSpawnForOwner(pUnit, MISSILE_SUICIDEICEEXPLODE, nX, nY, SKILL_ATTACK, 1);
    }

    nY = UNITS_GetYPosition(pUnit);
    nX = UNITS_GetXPosition(pUnit);
    MISSILE_ClientSpawnForOwner(pUnit, MISSILE_SUICIDECORPSEEXPLODE, nX, nY, SKILL_ATTACK, 1);
}


//1.10f: D2Client.0x6FB46070
//  states.txt pregnant (110), its remfunc 10: STATES_RemFunc10_Pregnant is a `jmp` here. At the unit, monstercorpseexplode (117) and pain worm
//  appear (545) through MISSILE_ClientSpawnSimple, skill 0, level 1, then sound 2458 (sounds.txt necromancer_corpseexp_1) -- X read before Y
//  here. nStateId is not read; the bare `retn` takes nothing off the stack. Nothing tests the unit: it is read at once.
//  The original inlines UNITS_GetXPosition here.
//  MONUNIQUE_MonUModCltWormsOnDeath is this body with `retn 4`; the two are written as two definitions.
void __fastcall MONUNIQUE_MonUModPlayPregnantBurst(D2UnitStrc *pUnit, D2C_States nStateId)
{
    int32_t nX;
    int32_t nY;

    UNREFERENCED_PARAMETER(nStateId);

    nX = UNITS_GetXPosition(pUnit);
    nY = UNITS_GetYPosition(pUnit);
    MISSILE_ClientSpawnSimple(pUnit, MISSILE_MONSTERCORPSEEXPLODE, nX, nY, SKILL_ATTACK, 1);
    MISSILE_ClientSpawnSimple(pUnit, MISSILE_PAIN_WORM_APPEAR, nX, nY, SKILL_ATTACK, 1);
    SOUNDINST_PlaySound(SOUND_NECROMANCER_CORPSEEXP_1, pUnit, 0, 0, 0);                   //  sounds.txt necromancer_corpseexp_1
}


//1.10f: D2Client.0x6FB46110
//  monumod.txt worms_on_death (40), column 2: MONUNIQUE_MonUModPlayPregnantBurst's body as a hook -- the two missiles and the sound at the
//  unit. Neither nModId nor bUnique is read, and nothing else is tested: no mode, no frame. MONSTER_UpdateAnimMode calls column 2 with no mode
//  test of its own, after MONSTER_SpawnAttackMissile, so a monster carrying the modifier bursts on every pass through that call, in any mode.
//  The original inlines UNITS_GetXPosition here.
//
//  BUG: the client's hook has no mode test where the server's has one, a divergence from the server. UNREACHABLE: no stock monster carries
//  the modifier -- no superuniques.txt Mod column names 40, monumod.txt gives it no pick weight, and the server never assigns it.
void __fastcall MONUNIQUE_MonUModCltWormsOnDeath(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique)
{
    int32_t nX;
    int32_t nY;

    UNREFERENCED_PARAMETER(nModId);
    UNREFERENCED_PARAMETER(bUnique);

    //  BUG: no mode or frame test.
    nX = UNITS_GetXPosition(pUnit);
    nY = UNITS_GetYPosition(pUnit);
    MISSILE_ClientSpawnSimple(pUnit, MISSILE_MONSTERCORPSEEXPLODE, nX, nY, SKILL_ATTACK, 1);
    MISSILE_ClientSpawnSimple(pUnit, MISSILE_PAIN_WORM_APPEAR, nX, nY, SKILL_ATTACK, 1);
    SOUNDINST_PlaySound(SOUND_NECROMANCER_CORPSEEXP_1, pUnit, 0, 0, 0);                   //  sounds.txt necromancer_corpseexp_1
}


//1.10f: D2Client.0x6FB461B0
//  Column 0 of gpMonUModCltFuncs: `push 0 / xor edx, edx / call` -- the dispatcher with pMissile NULL. Called by MONSTER_DoClientAction; the
//  column's one live slot is row 23, poisonhit, MONUNIQUE_MonUModCltGetHitOverlay.
void __fastcall MONUNIQUE_MonUModRunCltFuncsCol0(D2UnitStrc *pUnit)
{
    MONUNIQUE_MonUModRunCltFuncsForUnit(pUnit, NULL, 0);
}


//1.10f: D2Client.0x6FB461C0
//  Runs one column of a boss's hooks: for a monster with data, the hook gpMonUModCltFuncs holds in nColumn for each of the nine bytes of its id
//  array, called (the unit, the id, bUnique) -- or for column 4 (pMissile, the id, bUnique) -- bUnique the monster's type flag 8 either way.
//  In the bytes' order: the unit tested NULL, a monster and with data, and the array's address tested; bUnique; then the walk for columns 0..3
//  or, on `cmp ebp, 4 / jz`, the walk for column 4. Each steps `inc esi / cmp esi, 9 / jl`, the C's int index. The column entry points pass
//  pMissile NULL, which the walk for 0..3 never reads. The original inlines MONSTER_GetMonUmodArray and MONSTER_TestTypeFlag here.
//
//  BUG: the first id is not tested alone. MONUNIQUE_ApplyModifiers returns for a boss whose first id is 0, and so does the server's twin of this
//  function; this one walks the nine whatever the first holds, a 0 landing on row 0, empty in all five columns. An id array whose first id is 0
//  and a later one not runs that hook here and not on the server. UNREACHABLE (a divergence from the server): the reader of packet 0xAC stops
//  at its first 0 into a zeroed buffer, and the server stores an id into its array's first empty slot. Whether the server ever clears an id was
//  not looked for.
//  BUG: a byte of the id array is not bounded, as in MONUNIQUE_ApplyModifiers: `mov dl, [esi+ebx]` indexes the 43 rows with nothing between,
//  so an id of 43 or more reads past the table, into the original's next data. LATENT, as there.
//  DIFFERENCE: ours finds no hook for an id past the table, where the original reads the memory after it.
void __fastcall MONUNIQUE_MonUModRunCltFuncsForUnit(D2UnitStrc *pUnit, D2UnitStrc *pMissile, int32_t nColumn)
{
    uint8_t *pUMods;
    void (__fastcall *pfnHook)(D2UnitStrc *, D2C_MonUMods, int32_t);
    int32_t bUnique;
    int32_t nUMod;
    int32_t i;

    pUMods = MONSTER_GetMonUmodArray(pUnit);
    if (!pUMods)
        return;

    bUnique = MONSTER_TestTypeFlag(pUnit, MONTYPEFLAG_UNIQUE);

    //  BUG: the first id is not tested alone.
    if (nColumn != 4)
    {
        for (i = 0; i < 9; ++i)
        {
            nUMod = pUMods[i];
            //  DIFFERENCE: unbounded in the original; past the table ours finds no hook.
            pfnHook = nUMod < 43 ? gpMonUModCltFuncs[nUMod][nColumn] : NULL;
            if (pfnHook)
                pfnHook(pUnit, (D2C_MonUMods)nUMod, bUnique);
        }
        return;
    }

    for (i = 0; i < 9; ++i)
    {
        nUMod = pUMods[i];
        //  DIFFERENCE: unbounded in the original; past the table ours finds no hook.
        pfnHook = nUMod < 43 ? gpMonUModCltFuncs[nUMod][4] : NULL;
        if (pfnHook)
            pfnHook(pMissile, (D2C_MonUMods)nUMod, bUnique);
    }
}


//1.10f: D2Client.0x6FB46270
//  Column 1 of gpMonUModCltFuncs, the dispatcher with pMissile NULL. Called by MONSTER_DoClientAction; the column's one live slot is row 22,
//  questcomplete, MONUNIQUE_MonUModCltQuestComplete.
void __fastcall MONUNIQUE_MonUModRunCltFuncsCol1(D2UnitStrc *pUnit)
{
    MONUNIQUE_MonUModRunCltFuncsForUnit(pUnit, NULL, 1);
}


//1.10f: D2Client.0x6FB46280
//  Column 2 of gpMonUModCltFuncs, the dispatcher with pMissile NULL. Called by MONSTER_UpdateAnimMode. The column holds eleven of the table's
//  fifteen live slots, rows 9, 10, 11, 17, 18, 20, 31, 32, 33, 40 and 42.
void __fastcall MONUNIQUE_MonUModRunCltFuncsCol2(D2UnitStrc *pUnit)
{
    MONUNIQUE_MonUModRunCltFuncsForUnit(pUnit, NULL, 2);
}


//1.10f: D2Client.0x6FB46290
//  Column 3 of gpMonUModCltFuncs, the dispatcher with pMissile NULL. Two call sites, both in MONSTER_DoClientAction; the column's one live slot
//  is row 17, lightning, MONUNIQUE_MonUModCltLightning.
void __fastcall MONUNIQUE_MonUModRunCltFuncsCol3(D2UnitStrc *pUnit)
{
    MONUNIQUE_MonUModRunCltFuncsForUnit(pUnit, NULL, 3);
}


//1.10f: D2Client.0x6FB462A0
//  Column 4 of gpMonUModCltFuncs, for a missile just made: a unit, a missile and a monster unit (`test ecx, ecx`, `test edx, edx`,
//  `cmp dword ptr [ecx], 1`), then the dispatcher with both, edx passed on untouched. Called by MISSILE_ClientSpawn with the missile's owner;
//  the column's one live slot is row 29, multishot, MONUNIQUE_MonUModCltMultishotSpawn.
void __fastcall MONUNIQUE_MonUModRunCltFuncsOnMissileSpawn(D2UnitStrc *pUnit, D2UnitStrc *pMissile)
{
    if (pUnit && pMissile && pUnit->dwUnitType == UNIT_MONSTER)
        MONUNIQUE_MonUModRunCltFuncsForUnit(pUnit, pMissile, 4);
}


//  The minion line's string; the label table's fourteen strings are pinned by the table below.
static_assert(STR_IDX_3221_minion == 3221,
              "MonUnique.cpp batch 4: `mov ecx, 0C95h` (0x6FB46337, 0x6FB46351)");


//1.10f: D2Client.0x6FB462C0
//  The first line of a boss's hover label, into pwszOut: "<prefix> Minion" for a minion, else its enchantments -- the label of each of the
//  nine bytes of its id array that gMonUModLabelStrings names, after the prefix, joined by gwszNameSeparator. Called by
//  UIPANELS_ShouldDrawNameLabel with gwszUnitLabelLine1, 256 (`push 100h`) and its own prefix. The original inlines MONSTER_GetMonUmodArray
//  here. In the bytes' order:
//
//    * pwszOut's first character is cleared before any test, so a NULL unit leaves an empty line.
//    * The id array: no unit, no monster or no data returns. MONSTER_IsMinion then picks the label.
//    * A minion: tbl nPrefixStr, tbl 3995 (" ") and tbl 3221 ("Minion"), or "Minion" alone without a prefix -- strcpy, then strcat; the font
//      is not touched on this arm.
//    * Anything else: font 6 for the measuring (restored on both ways out), the prefix appended with Unicode::strcat and counted -- its length
//      and its width, each from its own call of D2LANG_GetStringFromTblIndex, three in all -- and then the walk.
//
//  The prefix is tested as 16 bits, `cmp cx, bx` and `test si, si`: D2C_StringIndices is a uint16_t here, so `if (nPrefixStr)` is that test.
//
//  The walk (`inc ecx / cmp ecx, 9 / jl`) keeps its index in nPrefixStr's slot, `[esp+2Ch]`, once the prefix is read for the last time. An id
//  whose word is 0 is skipped with no test of the width. For one that is not:
//
//    * the separator, before every label after the first entry -- the prefix counts as one (nEntries starts at 1 with it) -- when one more
//      character fits, counted as one character and its width added;
//    * the label, when the count is under nCapacity - 1 and the count with the label's length still is: Unicode::strlen twice, the width, the
//      strcat, in that order;
//    * the width tested last, `cmp ebx, 1E0h / jg`: past 480 the walk stops. The label that crossed 480 is in the line by then, so the line can
//      be wider than 480 by its last label.
//
//  The table is read twice for an id whose label is tried, the test and the load; ours reads one element through one pointer twice. The
//  separator is the string MONUNIQUE_BuildMonsterNameText refills from " " on each of its calls.
//
//  BUG: the prefix is appended and counted with no test against nCapacity. UNREACHABLE in the languages measured: it needs 255 characters,
//  more than the stock tbl files make (Chinese and Korean not established).
//  BUG: a separator whose label then does not fit stays at the end of the line. UNREACHABLE in the languages measured, as above.
//  BUG: the id byte indexes the 44 words with nothing between (`mov al, [ecx+edx]` then `gMonUModLabelStrings[eax*2]`, 0x6FB463D2, 0x6FB463D9),
//  as the dispatcher's does. LATENT, with the dispatcher's.
//  DIFFERENCE: ours finds no label for an id past the table, where the original reads the word at the same index.
void __fastcall MONUNIQUE_MonUModBuildMonsterModifierLabel(D2UnitStrc *pUnit, Unicode *pwszOut, int32_t nCapacity, D2C_StringIndices nPrefixStr)
{
    uint8_t *pUMods;
    const D2C_StringIndices *pLabelStr;
    const Unicode *wszModLabel;
    enum Font nPrevFont;
    int32_t nTextWidth;
    int32_t nLength;
    int32_t nEntries;
    int32_t nUMod;
    int32_t i;

    nTextWidth = 0;
    pwszOut[0].ch = 0;

    pUMods = MONSTER_GetMonUmodArray(pUnit);
    if (!pUMods)
        return;

    if (MONSTER_IsMinion(pUnit))
    {
        if (nPrefixStr)
        {
            Unicode::strcpy(pwszOut, D2LANG_GetStringFromTblIndex(nPrefixStr));
            Unicode::strcat(pwszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
            Unicode::strcat(pwszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3221_minion));
        }
        else
        {
            Unicode::strcpy(pwszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3221_minion));
        }
        return;
    }

    nLength = 0;
    nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT6);
    nEntries = 0;
    if (nPrefixStr)
    {
        nEntries = 1;
        //  BUG: appended and counted with no test against nCapacity.
        Unicode::strcat(pwszOut, D2LANG_GetStringFromTblIndex(nPrefixStr));
        nLength = Unicode::strlen(D2LANG_GetStringFromTblIndex(nPrefixStr));
        nTextWidth = D2WIN_10121_GetTextWidth(D2LANG_GetStringFromTblIndex(nPrefixStr));
    }

    for (i = 0; i < 9; ++i)
    {
        nUMod = pUMods[i];
        //  DIFFERENCE: unbounded in the original; past the table ours finds no label.
        if (nUMod >= 44)
            continue;
        pLabelStr = &gMonUModLabelStrings[nUMod];
        if (!*pLabelStr)
            continue;

        //  BUG: left at the end when the label after it does not fit.
        if (nEntries && nLength + 1 < nCapacity - 1)
        {
            Unicode::strcat(pwszOut, gwszNameSeparator);
            ++nLength;
            nTextWidth += D2WIN_10121_GetTextWidth(gwszNameSeparator);
        }
        if (nLength < nCapacity - 1)
        {
            wszModLabel = D2LANG_GetStringFromTblIndex(*pLabelStr);
            if (nLength + Unicode::strlen(wszModLabel) < nCapacity - 1)
            {
                nLength += Unicode::strlen(wszModLabel);
                nTextWidth += D2WIN_10121_GetTextWidth(wszModLabel);
                Unicode::strcat(pwszOut, wszModLabel);
                ++nEntries;
            }
        }
        if (nTextWidth > 480)
            break;
    }

    D2WIN_10127_SetFont(nPrevFont);
}

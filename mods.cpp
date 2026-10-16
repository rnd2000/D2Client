#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "mods.h"

//  mods.cpp -- the item description's property text of D2Client.dll 1.10: nine functions, 0x6FAF12C0..0x6FAF389C, from
//  MODS_AppendBluntUndeadBonusText to MODS_FormatElementalDamageText (the blunt weapon's undead bonus, a rune's property set as text, the
//  socket filler's slot lines, the property list and its lines, the set bonuses, the damage lines).
//  The original source file is mods.cpp (two asserts in this unit name it).

//  The numbers the first five functions lean on that no other file pins: each is an immediate in the original.
static_assert(STATE_NONE == 0 && STATE_RUNEWORD == 171,
              "MODS_BuildStatsText's two states: push 0, push 0ABh (0x6FAF3191)");
static_assert(STATLIST_BASE == 0 && STATLIST_MAGIC == 0x40,
              "the stat-list flags: push 0 (0x6FAF3330), push 40h (0x6FAF318F)");
static_assert(PROPMODE_RUNE == 5,
              "ITEMMODS_AssignProperty's mode: push 5 (0x6FAF13D6)");
static_assert(STAT_VALUE == 0x47,
              "the set bonus's key stat: push 47h (0x6FAF341C)");
static_assert(STR_IDX_4002_plus == 0xFA2 && STR_IDX_4001_percent == 0xFA1
              && STR_IDX_3554_ModStr4f == 0xDE2 && STR_IDX_3852_KeyComma == 0xF0C,
              "the tbl strings: mov ecx, 0FA2h / 0FA1h / 0DE2h / 0F0Ch");
static_assert(offsetof(D2SetItemsTxt, nAddFunc) == 0x87,
              "setitems.txt add func: mov cl, [eax+87h] (0x6FAF32E9)");
static_assert(offsetof(D2SetItemsTxt, nSetId) == 0x2C
              && sizeof(((D2SetItemsTxt *)0)->nSetId) == 2,
              "setitems.txt set id: movsx ecx, word ptr [edi+2Ch] (0x6FAF3424)");


//1.10f: D2Client.0x6FAF12C0
//  Appends the line "+50% Damage to Undead" that every blunt weapon gets without carrying the stat: tbl 4002 '+', the number (50, an immediate,
//  printed with "%i" into ten bytes and widened with a count of nine), tbl 4001 '%', 3995 ' ', 3554 "Damage to Undead", 3998 (the newline).
//  The line is built in a 128-Unicode local and appended to wszOut with no bound. The two buffers are constructed in the original's order
//  (the line, the four tbl fetches, then the number), because each construction is a call into D2Lang.
//  pUnit and nMaxLen are not read: the caller loads both and `retn 4` keeps the stack slot. void: eax holds the last strcat's result, which
//  the one call site discards.
void __fastcall MODS_AppendBluntUndeadBonusText(D2UnitStrc *pUnit, Unicode *wszOut, int32_t nMaxLen)
{
    UNREFERENCED_PARAMETER(pUnit);
    UNREFERENCED_PARAMETER(nMaxLen);

    Unicode wszLine[128];

    const Unicode *pwszPlus = D2LANG_GetStringFromTblIndex(STR_IDX_4002_plus);
    const Unicode *pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
    const Unicode *pwszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
    const Unicode *pwszPercent = D2LANG_GetStringFromTblIndex(STR_IDX_4001_percent);

    Unicode wszValue[10];
    char szValue[10];

    STORM_SStrPrintf(szValue, 10, "%i", 50);
    Unicode::toUnicode(wszValue, szValue, 9);
    Unicode::strcat(wszLine, pwszPlus);
    Unicode::strcat(wszLine, wszValue);
    Unicode::strcat(wszLine, pwszPercent);
    Unicode::strcat(wszLine, pwszSpace);
    Unicode::strcat(wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_3554_ModStr4f));
    Unicode::strcat(wszLine, pwszNewline);
    Unicode::strcat(wszOut, wszLine);
}


//1.10f: D2Client.0x6FAF13C0
//  A rune's property set as text: MODS_BuildSocketFillerText's preview of what a rune grants in one kind of socket (its four calls are on the
//  rune arm). The set is assigned to the rune itself (ITEMMODS_AssignProperty with PROPMODE_RUNE and no parent unit), rendered through
//  MODS_AppendMagicModsText into a 1024-Unicode local and appended to wszOut; then the stat list the assignment made (STATLIST_MAGIC, no state)
//  is expired and freed, and a trailing newline is cut off wszOut.
//  The local is constructed after the assignment and its first character is zeroed a second time; both are kept. Unicode::strlen is called
//  twice, as the original does. void: eax held a pointer to wszOut's last character on one path and its length on the other; no call site reads it.
//  BUG: on an empty wszOut the original reads the word below it (`cmp word ptr [esi+eax*2-2], 0Ah`, 0x6FAF1452) and would clear it if it held
//  10. UNREACHABLE: each of the four calls follows a newline appended to wszOut.
void __fastcall MODS_AppendPropertySetText(D2UnitStrc *pUnit, D2GemsTxt *pGemsTxt, int32_t nPropSet, Unicode *wszOut, int32_t nMaxLen, const Unicode *wszSep)
{
    D2StatListStrc *pStatList;
    int32_t nLen;

    D2COMMON_ITEMMODS_AssignProperty(PROPMODE_RUNE, NULL, pUnit, pGemsTxt, nPropSet, 0);

    Unicode wszMods[1024];

    wszMods[0].ch = 0;
    MODS_AppendMagicModsText(pUnit, wszMods, nMaxLen, 1, wszSep);
    Unicode::strcat(wszOut, wszMods);

    pStatList = D2COMMON_STATLIST_GetStatListFromUnitStateOrFlag(pUnit, STATE_NONE, STATLIST_MAGIC);
    if (pStatList)
    {
        D2COMMON_STATLIST_ExpireStatList(pUnit, pStatList);
        D2COMMON_STATLIST_FreeStatList(pStatList);
    }

    nLen = Unicode::strlen(wszOut);
    //  DIFFERENCE: `nLen > 0` is ours: the original reads the word below an empty wszOut (see the head).
    if (nLen > 0 && wszOut[nLen - 1].ch == 10)
        wszOut[Unicode::strlen(wszOut) - 1].ch = 0;
}


//  The path this unit passes as __FILE__ and the three texts MODS_BuildSocketFillerText asserts.
static const char szModsCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UI\\mods.cpp";
static const char szAssertSocketFiller[] = "ITEMSIsA(hItem, ITEMTYPE_SOCKETFILLER_TYPE)";
static const char szPtItemStats[] = "ptItemStats";
static const char szPtGemMods[] = "ptGemMods";

static_assert(ITEMTYPE_SOCKET_FILLER == 53 && PROPMODE_GEM == 2,
              "the filler's type and the gem's mode: push 35h (0x6FAF14A5), push 2 (0x6FAF1593)");
static_assert(STR_IDX_11073_GemXp1 == 0x2B41 && STR_IDX_11074_GemXp2 == 0x2B42
              && STR_IDX_11075_GemXp3 == 0x2B43 && STR_IDX_11076_GemXp4 == 0x2B44,
              "the four slot labels: mov ecx, 2B41h .. 2B44h");


//1.10f: D2Client.0x6FAF1480
//  What a gem or a rune grants, slot by slot. wszOut is cleared and then, for a gem or a rune, gets a newline and four labelled blocks,
//  newline-separated, and two newlines at the end:
//
//      tbl 11074 GemXp2   "Shields:"   property set 2
//      tbl 11073 GemXp1   "Helms:"     property set 1
//      tbl 11076 GemXp4   "Armor:"     property set 1
//      tbl 11075 GemXp3   "Weapons:"   property set 0
//
//  Set 1 twice and the labels in the order 2, 1, 4, 3 are the original's; a loop would print something else. Each label is built in a
//  256-Unicode local (cleared, the label, tbl 3995 ' ') and handed on as the separator the magic-mods block puts before its last line.
//
//      a gem (ITEMTYPE_GEM)   the set is assigned here, with PROPMODE_GEM and no parent unit, rendered straight into wszOut by
//                             MODS_AppendMagicModsText with bAppendNewline 0 -- the mods joined by ", " -- and the stat list the
//                             assignment made freed; then a trailing newline cut
//      a rune (ITEMTYPE_RUNE) MODS_AppendPropertySetText, the same work with PROPMODE_RUNE, a 1024-Unicode buffer and bAppendNewline 1
//      anything else          nothing, and not the two closing newlines either: the `jz` at 0x6FAF17FB leaves for the exit past them, so
//                             wszOut stays empty
//
//  The four gem blocks are written out, as the original has them. Every block runs just after a newline was appended, so the trailing-newline
//  cut never reads below wszOut (which is why MODS_AppendPropertySetText's empty-buffer case is unreachable).
//  Three asserts, each FOG_DisplayAssert and `exit(-1)`: the item is a socket filler (line 538), it has an items.txt record (540), and its
//  gems.txt record exists (547 on the gem arm, 581 on the rune arm). The class id is `pItem ? dwClassId : -1`, a NULL test the first assert has
//  already made moot (ITEMS_CheckItemTypeId answers 0 for a NULL item). nMaxLen is passed on to both renderers, which do not read it.
void __fastcall MODS_BuildSocketFillerText(D2UnitStrc *pItem, Unicode *wszOut, int32_t nMaxLen)
{
    D2ItemsTxt *pItemsTxt;
    D2GemsTxt *pGemsTxt;
    D2StatListStrc *pStatList;

    Unicode wszLabel[256];

    if (!D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_SOCKET_FILLER))
    {
        FOG_DisplayAssert((uint32_t)szAssertSocketFiller, (uint32_t)szModsCpp, 538);
        exit(-1);
    }
    pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(pItem ? (int32_t)pItem->dwClassId : -1);
    if (!pItemsTxt)
    {
        FOG_DisplayAssert((uint32_t)szPtItemStats, (uint32_t)szModsCpp, 540);
        exit(-1);
    }
    wszOut[0].ch = 0;

    if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_GEM))
    {
        pGemsTxt = D2COMMON_DATATBLS_GetGemsTxtRecord(pItemsTxt->dwGemOffset);
        if (!pGemsTxt)
        {
            FOG_DisplayAssert((uint32_t)szPtGemMods, (uint32_t)szModsCpp, 547);
            exit(-1);
        }

        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
        wszLabel[0].ch = 0;
        Unicode::strcat(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_11074_GemXp2));
        Unicode::strcat(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        D2COMMON_ITEMMODS_AssignProperty(PROPMODE_GEM, NULL, pItem, pGemsTxt, 2, 0);
        MODS_AppendMagicModsText(pItem, wszOut, nMaxLen, 0, wszLabel);
        pStatList = D2COMMON_STATLIST_GetStatListFromUnitStateOrFlag(pItem, STATE_NONE, STATLIST_MAGIC);
        if (pStatList)
        {
            D2COMMON_STATLIST_ExpireStatList(pItem, pStatList);
            D2COMMON_STATLIST_FreeStatList(pStatList);
        }
        if (wszOut[Unicode::strlen(wszOut) - 1].ch == 10)
            wszOut[Unicode::strlen(wszOut) - 1].ch = 0;

        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
        wszLabel[0].ch = 0;
        Unicode::strcat(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_11073_GemXp1));
        Unicode::strcat(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        D2COMMON_ITEMMODS_AssignProperty(PROPMODE_GEM, NULL, pItem, pGemsTxt, 1, 0);
        MODS_AppendMagicModsText(pItem, wszOut, nMaxLen, 0, wszLabel);
        pStatList = D2COMMON_STATLIST_GetStatListFromUnitStateOrFlag(pItem, STATE_NONE, STATLIST_MAGIC);
        if (pStatList)
        {
            D2COMMON_STATLIST_ExpireStatList(pItem, pStatList);
            D2COMMON_STATLIST_FreeStatList(pStatList);
        }
        if (wszOut[Unicode::strlen(wszOut) - 1].ch == 10)
            wszOut[Unicode::strlen(wszOut) - 1].ch = 0;

        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
        wszLabel[0].ch = 0;
        Unicode::strcat(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_11076_GemXp4));
        Unicode::strcat(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        D2COMMON_ITEMMODS_AssignProperty(PROPMODE_GEM, NULL, pItem, pGemsTxt, 1, 0);
        MODS_AppendMagicModsText(pItem, wszOut, nMaxLen, 0, wszLabel);
        pStatList = D2COMMON_STATLIST_GetStatListFromUnitStateOrFlag(pItem, STATE_NONE, STATLIST_MAGIC);
        if (pStatList)
        {
            D2COMMON_STATLIST_ExpireStatList(pItem, pStatList);
            D2COMMON_STATLIST_FreeStatList(pStatList);
        }
        if (wszOut[Unicode::strlen(wszOut) - 1].ch == 10)
            wszOut[Unicode::strlen(wszOut) - 1].ch = 0;

        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
        wszLabel[0].ch = 0;
        Unicode::strcat(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_11075_GemXp3));
        Unicode::strcat(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        D2COMMON_ITEMMODS_AssignProperty(PROPMODE_GEM, NULL, pItem, pGemsTxt, 0, 0);
        MODS_AppendMagicModsText(pItem, wszOut, nMaxLen, 0, wszLabel);
        pStatList = D2COMMON_STATLIST_GetStatListFromUnitStateOrFlag(pItem, STATE_NONE, STATLIST_MAGIC);
        if (pStatList)
        {
            D2COMMON_STATLIST_ExpireStatList(pItem, pStatList);
            D2COMMON_STATLIST_FreeStatList(pStatList);
        }
        if (wszOut[Unicode::strlen(wszOut) - 1].ch == 10)
            wszOut[Unicode::strlen(wszOut) - 1].ch = 0;
    }
    else
    {
        if (!D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_RUNE))
            return;

        pGemsTxt = D2COMMON_DATATBLS_GetGemsTxtRecord(pItemsTxt->dwGemOffset);
        if (!pGemsTxt)
        {
            FOG_DisplayAssert((uint32_t)szPtGemMods, (uint32_t)szModsCpp, 581);
            exit(-1);
        }

        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
        wszLabel[0].ch = 0;
        Unicode::strcat(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_11074_GemXp2));
        Unicode::strcat(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        MODS_AppendPropertySetText(pItem, pGemsTxt, 2, wszOut, nMaxLen, wszLabel);

        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
        wszLabel[0].ch = 0;
        Unicode::strcat(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_11073_GemXp1));
        Unicode::strcat(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        MODS_AppendPropertySetText(pItem, pGemsTxt, 1, wszOut, nMaxLen, wszLabel);

        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
        wszLabel[0].ch = 0;
        Unicode::strcat(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_11076_GemXp4));
        Unicode::strcat(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        MODS_AppendPropertySetText(pItem, pGemsTxt, 1, wszOut, nMaxLen, wszLabel);

        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
        wszLabel[0].ch = 0;
        Unicode::strcat(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_11075_GemXp3));
        Unicode::strcat(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        MODS_AppendPropertySetText(pItem, pGemsTxt, 0, wszOut, nMaxLen, wszLabel);
    }

    Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
    Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
}


//  The elixir half's assert text and the numbers MODS_BuildStatsText leans on that no other file pins.
static const char szAssertZero[] = "0";

static_assert(ITEMTYPE_ELIXIR == 11 && ITEMTYPE_BLUNT == 57,
              "the two item types: cmp eax, 0Bh (0x6FAF1A12), push 39h (0x6FAF1D37)");
static_assert(STAT_ITEM_UNDEADDAMAGE_PERCENT == 122 && STAT_POISON_COUNT == 326
              && STAT_ITEM_INDESCTRUCTIBLE == 152,
              "push 7Ah (0x6FAF1D44), push 146h (0x6FAF1E97), push 98h (0x6FAF20D1)");
static_assert(STR_IDX_21240_ModStre9s == 0x52F8,
              "\"Indestructible\": mov ecx, 52F8h (0x6FAF20EA)");
static_assert(offsetof(D2DataTablesStrc, pStatsWithDescFunc) == 0xBD8
              && offsetof(D2DataTablesStrc, nStatsWithDescFunc) == 0xBDC,
              "the described stats: [eax+0BD8h] (0x6FAF1F62), [ecx+0BDCh] (0x6FAF1F52)");
static_assert(offsetof(D2ItemsTxt, nDurability) == 0x112
              && offsetof(D2ItemsTxt, nNoDurability) == 0x113,
              "items.txt durability, nodurability: [eax+112h] (0x6FAF20C5), [eax+113h] (0x6FAF20BB)");
static_assert(offsetof(D2ElixirStatLabelStrc, nAlwaysOne) == 0x4
              && offsetof(D2ElixirStatLabelStrc, wStrPositive) == 0x8
              && offsetof(D2ElixirStatLabelStrc, wStrNegative) == 0xA
              && offsetof(D2ElixirStatLabelStrc, nAlwaysTwo) == 0xC,
              "an elixir label: [edi+4] (0x6FAF1B56), cx [edi+8] / [edi+0Ah] (0x6FAF1B01, "
              "0x6FAF1B09), [edi+0Ch] (0x6FAF1B8E)");
static_assert(offsetof(D2StatStrc, nValue) == 4,
              "a copied stat's value: [esp+eax*8+...+nValue] (0x6FAF1FAC)");


//1.10f: D2Client.0x6FAF19C0
//  The property list: the builder every property text of the item description ends in. It appends to wszOut the text of pUnit's stat list
//  under nState and nFlag, merged with the list under nSourceState when that is set and with each socketed item's list under nState (nState,
//  not nSourceState). Called by MODS_AppendMagicModsText (STATLIST_MAGIC, STATE_RUNEWORD as the source state, the undead bonus allowed) and by
//  the two set appenders (one set-bonus state each, STATLIST_BASE, a newline after every line).
//  nMaxLen is not read: `retn 18h` keeps its slot. The frame is 5348 bytes (`mov eax, 14E4h / call __alloca_probe`), 4088 of them the copied stats.
//  BUG: the text is appended to wszOut with no bound, so the bound is the callers' buffers. NOT ESTABLISHED: whether stock data can fill them.
//
//  Two halves, on the item's type (`cmp eax, 0Bh`); the original lays the elixir half first:
//
//      ITEMTYPE_ELIXIR    a line per elixir label whose file index is the item's
//      anything else      the property list
//
//  The elixir half: gptElixirStatLabels is six 16-byte records and its count is compared unsigned. Every record whose file index matches
//  prints (the scan does not stop at the first). The value is always STAT_VALUE; a file index of 8 or 9, 6 or 7, 10 or 11 shifts it right by
//  8, arithmetic (`sar`), each pair its own test, so one shift at most. The label is the record's positive string for a positive value and
//  its negative one for a negative; a zero prints nothing. The line is the label, tbl 3995 ' ', the number (after tbl 4002 '+' when positive;
//  a negative number carries its own sign) and tbl 3998, so the stock line reads "Elixir of Strength +5".
//  Two asserts, FOG_DisplayAssert of "0" and `exit(-1)`: the record's +4 is 1 (line 321) before the number is signed, its +0xC is 2
//  (line 317) before the line is built. They belong to a helper the original inlines here, written in place.
//
//  The property list: pUnit's stat list under nState and nFlag, or nothing at all (the exit frees nothing, since nothing was made). A fresh
//  list from pUnit's pool, `pUnit ? ... : UNIT_TYPES_COUNT, -1` for a NULL unit, into which the lists above are merged. Then, in order:
//
//      the undead bonus   bAppendUndeadBonus, ITEMTYPE_BLUNT and no STAT_ITEM_UNDEADDAMAGE_PERCENT of its own: the line of
//                         MODS_AppendBluntUndeadBonusText, built in a 128-Unicode local constructed at the top
//      the damage pairs   the seven groups MODS_FormatElementalDamageText reads, filled from the stats gptClientStatDescs names: physical
//                         min and max fall back to the secondary pair when zero, and stat 59 also takes STAT_POISON_COUNT off the UNIT, not
//                         the merged list. A group whose min and max are both positive gets bPresent, the flag the damage lines test
//      the lines          each stat D2Common lists as described (pStatsWithDescFunc, sorted by descpriority when itemstatcost.txt loads),
//                         its id read signed (`movsx`) and bounded by the table; each layer of it D2COMMON_11269_CopyStats copies (511
//                         slots) with a non-zero value: the damage lines first, and when they decline, the ordinary line from
//                         MODS_FormatPropertyLine into a 128-Unicode local
//      "Indestructible"   an item whose items.txt has durability and not nodurability, with no STAT_ITEM_INDESCTRUCTIBLE and a max
//                         durability of 0: tbl 21240
//
//  bAppendNewline decides the joins: set, tbl 3998 after every line; clear, tbl 3852 ',' and tbl 3995 ' ' between lines. The layer is a
//  zero-extended word. The line local is built once and not cleared between stats; MODS_FormatPropertyLine empties it before anything else,
//  so no line carries over.
//  For stat 23 or 24 the line is built first; only then is the merged list asked for 21 or 22, and the line thrown away when that is non-zero.
//  The call stays where the original makes it.
//
//  gptClientStatDescs is a copy inside D2Client of itemstatcost.txt's description columns (the stat id, descfunc, descstrpos, descstrneg,
//  descval): 143 records of 16 bytes, the first with id -1, which the walk skips. Only the stat id is read, and only here.
//  ITEMS_GetItemType asserts its item and exits, so past the first call pUnit is not NULL and its later NULL tests cannot fail; kept, as the
//  original has them. The tail's untested items.txt record is the one that call asserted. STATLIST_AllocStatList writes into what it allocates,
//  so the merged list is not tested either. D2COMMON_11269_CopyStats takes a D2StatListExStrc * and tests the extended flag before it walks,
//  so the plain merged list is walked as a plain list -- the cast is the prototype's.
void __fastcall MODS_BuildStatsText(D2UnitStrc *pUnit, Unicode *wszOut, int32_t nMaxLen, int32_t bAppendUndeadBonus, D2C_States nState, D2C_States nSourceState, int32_t nFlag, int32_t bAppendNewline)
{
    D2StatListStrc *pStateStats;
    D2StatListStrc *pMergedStats;
    D2StatListStrc *pSourceStats;
    D2UnitStrc *pNode;
    D2UnitStrc *pSocketed;
    D2ClientStatDescStrc *pStatDesc;
    D2C_ItemStats nStatId;
    D2ClientDamageGroupStrc aDamageGroups[7];
    const Unicode *pwszLineEnd;
    const Unicode *pwszSpace;
    const Unicode *pwszComma;
    const Unicode *pwszIndestructible;
    D2ItemsTxt *pItemsTxt;
    D2StatStrc aStats[511];
    int32_t nDescStatId;
    int32_t nCopiedStats;
    int32_t nValue;
    int32_t nLayer;
    int32_t bAnyEmitted;
    int32_t iStatDesc;
    int32_t iDescStat;
    int32_t iCopied;
    int32_t i;

    UNREFERENCED_PARAMETER(nMaxLen);

    Unicode wszLine[128];
    Unicode wszUndead[128];

    if (D2COMMON_ITEMS_GetItemType(pUnit) == ITEMTYPE_ELIXIR)
    {
        D2ElixirStatLabelStrc *pLabel;
        const Unicode *pwszLabel;
        int32_t nLabelFileIndex;
        int32_t nStatValue;
        uint32_t iLabel;

        Unicode wszElixirLine[128];
        Unicode wszSignedValue[128];

        const Unicode *pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
        const Unicode *pwszElixirSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
        const Unicode *pwszPlus = D2LANG_GetStringFromTblIndex(STR_IDX_4002_plus);
        int32_t nItemFileIndex = D2COMMON_ITEMS_GetFileIndex(pUnit);

        pLabel = gptElixirStatLabels;
        for (iLabel = 0; iLabel < gnElixirStatLabelCount; ++iLabel, ++pLabel)
        {
            if (pLabel->nFileIndex != nItemFileIndex)
                continue;

            wszElixirLine[0].ch = 0;
            wszSignedValue[0].ch = 0;
            nStatValue = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_VALUE, 0);
            nLabelFileIndex = pLabel->nFileIndex;
            if (nLabelFileIndex == 8 || nLabelFileIndex == 9)
                nStatValue >>= 8;
            if (nLabelFileIndex == 6 || nLabelFileIndex == 7)
                nStatValue >>= 8;
            if (nLabelFileIndex == 10 || nLabelFileIndex == 11)
                nStatValue >>= 8;

            pwszLabel = NULL;
            if (nStatValue > 0)
                pwszLabel = D2LANG_GetStringFromTblIndex((D2C_StringIndices)pLabel->wStrPositive);
            else if (nStatValue < 0)
                pwszLabel = D2LANG_GetStringFromTblIndex((D2C_StringIndices)pLabel->wStrNegative);

            if (!nStatValue)
                continue;

            Unicode wszValue[10];
            char szValue[10];

            STORM_SStrPrintf(szValue, 10, "%i", nStatValue);
            Unicode::toUnicode(wszValue, szValue, 9);
            if (pLabel->nAlwaysOne != 1)
            {
                FOG_DisplayAssert((uint32_t)szAssertZero, (uint32_t)szModsCpp, 321);
                exit(-1);
            }
            if (nStatValue <= 0)
            {
                Unicode::strcpy(wszSignedValue, wszValue);
            }
            else
            {
                Unicode::strcpy(wszSignedValue, pwszPlus);
                Unicode::strcat(wszSignedValue, wszValue);
            }
            if (pLabel->nAlwaysTwo != 2)
            {
                FOG_DisplayAssert((uint32_t)szAssertZero, (uint32_t)szModsCpp, 317);
                exit(-1);
            }
            Unicode::strcpy(wszElixirLine, pwszLabel);
            Unicode::strcat(wszElixirLine, pwszElixirSpace);
            Unicode::strcat(wszElixirLine, wszSignedValue);
            Unicode::strcat(wszElixirLine, pwszNewline);
            Unicode::strcat(wszOut, wszElixirLine);
        }
        return;
    }

    pStateStats = D2COMMON_STATLIST_GetStatListFromUnitStateAndFlag(pUnit, nState, nFlag);
    if (!pStateStats)
        return;

    pMergedStats = D2COMMON_STATLIST_AllocStatList(pUnit ? pUnit->pMemoryPool : NULL, 0, 0, pUnit ? pUnit->dwUnitType : UNIT_TYPES_COUNT, pUnit ? (D2UnitGUID)pUnit->dwUnitId : (D2UnitGUID)-1);
    D2COMMON_STATLIST_MergeBaseStats(pMergedStats, pStateStats);
    if (nSourceState)
    {
        pSourceStats = D2COMMON_STATLIST_GetStatListFromUnitStateAndFlag(pUnit, nSourceState, nFlag);
        if (pSourceStats)
            D2COMMON_STATLIST_MergeBaseStats(pMergedStats, pSourceStats);
    }
    if (pUnit && pUnit->dwUnitType == UNIT_ITEM && pUnit->pInventory)
    {
        pNode = D2COMMON_INVENTORY_GetFirstItem(pUnit->pInventory);
        while (pNode)
        {
            pSocketed = D2COMMON_INVENTORY_UnitIsItem(pNode);
            if (!pSocketed)
                break;
            pNode = D2COMMON_INVENTORY_GetNextItem(pNode);
            pSourceStats = D2COMMON_STATLIST_GetStatListFromUnitStateAndFlag(pSocketed, nState, nFlag);
            if (pSourceStats)
                D2COMMON_STATLIST_MergeBaseStats(pMergedStats, pSourceStats);
        }
    }

    pwszLineEnd = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
    pwszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
    pwszComma = D2LANG_GetStringFromTblIndex(STR_IDX_3852_KeyComma);

    if (bAppendUndeadBonus && D2COMMON_ITEMS_CheckItemTypeId(pUnit, ITEMTYPE_BLUNT) && !D2COMMON_STATLIST_UnitGetItemStatOrSkillStatValue(pUnit, STAT_ITEM_UNDEADDAMAGE_PERCENT, 0))
    {
        MODS_AppendBluntUndeadBonusText(pUnit, wszUndead, 128);
        Unicode::strcat(wszOut, wszUndead);
    }

    memset(aDamageGroups, 0, sizeof(aDamageGroups));
    pStatDesc = gptClientStatDescs;
    for (iStatDesc = 0; iStatDesc < gnClientStatDescCount; ++iStatDesc, ++pStatDesc)
    {
        nStatId = pStatDesc->nStatId;
        if (nStatId == STAT_INVALID)
            continue;

        switch (nStatId)
        {
        case STAT_ITEM_MAXDAMAGE_PERCENT:
            aDamageGroups[1].nMax = D2COMMON_STATLIST_GetStatValue(pMergedStats, nStatId, 0);
            break;
        case STAT_ITEM_MINDAMAGE_PERCENT:
            aDamageGroups[1].nMin = D2COMMON_STATLIST_GetStatValue(pMergedStats, nStatId, 0);
            break;
        case STAT_MINDAMAGE:
            aDamageGroups[0].nMin = D2COMMON_STATLIST_GetStatValue(pMergedStats, nStatId, 0);
            if (!aDamageGroups[0].nMin)
                aDamageGroups[0].nMin = D2COMMON_STATLIST_GetStatValue(pMergedStats, STAT_SECONDARY_MINDAMAGE, 0);
            break;
        case STAT_MAXDAMAGE:
            aDamageGroups[0].nMax = D2COMMON_STATLIST_GetStatValue(pMergedStats, nStatId, 0);
            if (!aDamageGroups[0].nMax)
                aDamageGroups[0].nMax = D2COMMON_STATLIST_GetStatValue(pMergedStats, STAT_SECONDARY_MAXDAMAGE, 0);
            break;
        case STAT_FIREMINDAM:
            aDamageGroups[4].nMin = D2COMMON_STATLIST_GetStatValue(pMergedStats, nStatId, 0);
            break;
        case STAT_FIREMAXDAM:
            aDamageGroups[4].nMax = D2COMMON_STATLIST_GetStatValue(pMergedStats, nStatId, 0);
            break;
        case STAT_LIGHTMINDAM:
            aDamageGroups[3].nMin = D2COMMON_STATLIST_GetStatValue(pMergedStats, nStatId, 0);
            break;
        case STAT_LIGHTMAXDAM:
            aDamageGroups[3].nMax = D2COMMON_STATLIST_GetStatValue(pMergedStats, nStatId, 0);
            break;
        case STAT_MAGICMINDAM:
            aDamageGroups[6].nMin = D2COMMON_STATLIST_GetStatValue(pMergedStats, nStatId, 0);
            break;
        case STAT_MAGICMAXDAM:
            aDamageGroups[6].nMax = D2COMMON_STATLIST_GetStatValue(pMergedStats, nStatId, 0);
            break;
        case STAT_COLDMINDAM:
            aDamageGroups[2].nMin = D2COMMON_STATLIST_GetStatValue(pMergedStats, nStatId, 0);
            break;
        case STAT_COLDMAXDAM:
            aDamageGroups[2].nMax = D2COMMON_STATLIST_GetStatValue(pMergedStats, nStatId, 0);
            break;
        case STAT_POISONMINDAM:
            aDamageGroups[5].nMin = D2COMMON_STATLIST_GetStatValue(pMergedStats, nStatId, 0);
            break;
        case STAT_POISONMAXDAM:
            aDamageGroups[5].nMax = D2COMMON_STATLIST_GetStatValue(pMergedStats, nStatId, 0);
            break;
        case STAT_POISONLENGTH:
            aDamageGroups[5].nLengthFrames = D2COMMON_STATLIST_GetStatValue(pMergedStats, nStatId, 0);
            aDamageGroups[5].nPoisonCount = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_POISON_COUNT, 0);
            break;
        default:
            break;
        }
    }

    for (i = 0; i < 7; ++i)
    {
        if (aDamageGroups[i].nMin > 0 && aDamageGroups[i].nMax > 0)
            aDamageGroups[i].bPresent = 1;
    }

    bAnyEmitted = 0;
    for (iDescStat = 0; iDescStat < sgptDataTables->nStatsWithDescFunc; ++iDescStat)
    {
        nDescStatId = (int16_t)sgptDataTables->pStatsWithDescFunc[iDescStat];
        if (nDescStatId < 0 || nDescStatId >= sgptDataTables->nItemStatCostTxtRecordCount)
            continue;

        nCopiedStats = D2COMMON_11269_CopyStats((D2StatListExStrc *)pMergedStats, nDescStatId, aStats, 511);
        for (iCopied = 0; iCopied < nCopiedStats; ++iCopied)
        {
            nValue = aStats[iCopied].nValue;
            nLayer = aStats[iCopied].nLayer;
            if (!nValue)
                continue;
            if (MODS_FormatElementalDamageText(aDamageGroups, (D2C_ItemStats)nDescStatId, wszOut))
                continue;
            if (!MODS_FormatPropertyLine(pUnit, pMergedStats, (D2C_ItemStats)nDescStatId, nLayer, nValue, wszLine))
                continue;

            if (nDescStatId == STAT_SECONDARY_MINDAMAGE)
            {
                if (D2COMMON_STATLIST_GetStatValue(pMergedStats, STAT_MINDAMAGE, 0))
                    continue;
            }
            else if (nDescStatId == STAT_SECONDARY_MAXDAMAGE)
            {
                if (D2COMMON_STATLIST_GetStatValue(pMergedStats, STAT_MAXDAMAGE, 0))
                    continue;
            }

            if (bAppendNewline)
                Unicode::strcat(wszLine, pwszLineEnd);
            if (bAnyEmitted && !bAppendNewline)
            {
                Unicode::strcat(wszOut, pwszComma);
                Unicode::strcat(wszOut, pwszSpace);
            }
            bAnyEmitted = 1;
            Unicode::strcat(wszOut, wszLine);
        }
    }

    if (pUnit && pUnit->dwUnitType == UNIT_ITEM)
    {
        pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(pUnit->dwClassId);
        if (!pItemsTxt->nNoDurability && pItemsTxt->nDurability && D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_ITEM_INDESCTRUCTIBLE, 0) <= 0 && !D2COMMON_STATLIST_GetMaxDurabilityFromUnit(pUnit))
        {
            pwszIndestructible = D2LANG_GetStringFromTblIndex(STR_IDX_21240_ModStre9s);
            if (bAnyEmitted && !bAppendNewline)
            {
                Unicode::strcat(wszOut, pwszComma);
                Unicode::strcat(wszOut, pwszSpace);
            }
            Unicode::strcat(wszOut, pwszIndestructible);
            if (bAppendNewline)
                Unicode::strcat(wszOut, pwszLineEnd);
        }
    }

    D2COMMON_STATLIST_FreeStatList(pMergedStats);
}


//  The numbers MODS_FormatPropertyLine leans on that no other file pins: itemstatcost.txt's value and description columns, charstats.txt's
//  three skill strings, montype.txt's plural, the bound and the table descfunc 22 takes from two different files, the tbl strings the cases
//  load, and D2Common's "no class".
static_assert(offsetof(D2ItemStatCostTxt, nValShift) == 0x18
              && offsetof(D2ItemStatCostTxt, nOp) == 0x54
              && offsetof(D2ItemStatCostTxt, nOpParam) == 0x55
              && offsetof(D2ItemStatCostTxt, wOpBase) == 0x56,
              "the value's scale: [ebp+18h] (0x6FAF2264), [edi+54h] (0x6FAF2221), "
              "[edi+55h] (0x6FAF2269), ax [edi+56h] (0x6FAF2237)");
static_assert(offsetof(D2ItemStatCostTxt, nDescFunc) == 0x36
              && offsetof(D2ItemStatCostTxt, nDescVal) == 0x37
              && offsetof(D2ItemStatCostTxt, wDescStrPos) == 0x38
              && offsetof(D2ItemStatCostTxt, wDescStrNeg) == 0x3A
              && offsetof(D2ItemStatCostTxt, wDescStr2) == 0x3C
              && offsetof(D2ItemStatCostTxt, wDescGrp) == 0x3E,
              "descfunc .. descstr2: [eax+36h] .. [eax+3Ch] (0x6FAF23EB..0x6FAF23FB), "
              "cx [eax+38h] / [eax+3Ah] (0x6FAF2437, 0x6FAF243D); dgrp: [edi+3Eh] (0x6FAF22A4)");
static_assert(offsetof(D2ItemStatCostTxt, nDescGrpFunc) == 0x40
              && offsetof(D2ItemStatCostTxt, nDescGrpVal) == 0x41
              && offsetof(D2ItemStatCostTxt, wDescGrpStrPos) == 0x42
              && offsetof(D2ItemStatCostTxt, wDescGrpStrNeg) == 0x44
              && offsetof(D2ItemStatCostTxt, wDescGrpStr2) == 0x46,
              "the group's columns: [eax+40h] .. [eax+46h] (0x6FAF240C..0x6FAF242D)");
static_assert(offsetof(D2CharStatsTxt, wStrAllSkills) == 0x52
              && offsetof(D2CharStatsTxt, wStrSkillTab) == 0x54
              && offsetof(D2CharStatsTxt, wStrClassOnly) == 0x5A,
              "charstats.txt: cx [eax+52h] (0x6FAF2844), [ebx+ecx*2+54h] (0x6FAF28A9), "
              "[ebx+5Ah] (0x6FAF28CC)");
static_assert(offsetof(D2MonTypeTxt, wStrPlur) == 0x0A,
              "montype.txt strplur: cx [ebx+0Ah] (0x6FAF2BDC)");
static_assert(offsetof(D2DataTablesStrc, nMonPropTxtRecordCount) == 0xC34
              && offsetof(D2DataTablesStrc, pMonTypeTxt) == 0xC3C
              && offsetof(D2DataTablesStrc, nMonTypeTxtRecordCount) == 0xC40,
              "descfunc 22: cmp eax, [ecx+0C34h] (0x6FAF2B96), [ecx+0C3Ch] (0x6FAF2B9E); "
              "montype.txt's own count, +0C40h, is not read");
static_assert(STR_IDX_21241_ModStre9t == 0x52F9 && STR_IDX_21242_ModStre9u == 0x52FA
              && STR_IDX_21249_ModStre10b == 0x5301 && STR_IDX_4003_to == 0xFA3
              && STR_IDX_11091_increaseswithplaylevelX == 0x2B53,
              "mov ecx, 52F9h / 52FAh (0x6FAF2742, 0x6FAF2763), 5301h (0x6FAF2D2A), "
              "0FA3h (0x6FAF2E56), 2B53h (0x6FAF30AA)");
static_assert(PCLASS_INVALID == 7,
              "descfunc 27's 'no class': mov [...], 7 (0x6FAF2EE4); cmp eax, 7 (0x6FAF2F05)");


//1.10f: D2Client.0x6FAF21C0
//  One property line: a described stat's value turned into its line by the stat's itemstatcost.txt description columns. Called by
//  MODS_BuildStatsText for each layer of a described stat that has a value and is not a damage line, into its 128-Unicode line local.
//  Non-zero: the line is built and the caller joins it; zero: the stat prints nothing.
//  The line starts empty, always: `mov word ptr [esi], 0` (0x6FAF21E2) is the first thing done with wszOut, before the stat id is tested, so
//  the caller's line local, which is built once and never cleared, carries nothing from one stat into the next.
//
//  nLayer is the stat's parameter, not a layer: the second half of a stat's id. Here it is a class (13), a class and a skill tab (14: the class
//  `>> 3`, the tab `& 7`), a skill and a level packed as `skill << nStuff` plus `level & nShiftedStuff` (15, 24), a skill (16, 27, 28), a
//  monster type (22) and a monster class (23).
//
//  The value: nValue as given; a stat with op 2..5 (nOp compared unsigned) is scaled by the local player's op base stat, `(nValue * (base >>
//  the base's valshift)) >> op param`, through CUNIT_GetPlayer and not pUnit, untested (STATLIST_UnitGetStatValue answers 0 for a NULL unit);
//  then by the stat's own valshift. All three shifts are `sar`. Then a blunt weapon's STAT_ITEM_UNDEADDAMAGE_PERCENT gets 50 more, the bonus
//  MODS_AppendBluntUndeadBonusText prints for one that lacks the stat. An op base outside the table gives 0 and skips both the last shift and
//  the bonus. The value's computation is written twice, as the original has it.
//
//  The group: a stat with a descgroup is compared with every member of it, from the table's last row down, the count re-read at each step,
//  each member's value read from pStatList at parameter 0 and scaled as above. One differing value, or a row outside the table, and the stat
//  prints on its own. When all agree the group's columns are used, and only by the member the walk met last, the lowest-numbered: every other
//  member returns 0, so the group prints once.
//
//  The descriptor: descfunc, descval and descstr2 (or the group's three) and the string by the value's sign; a descfunc outside 1..28 returns
//  0 (`dec eax / cmp eax, 1Bh / ja`, unsigned). The number is "%i" into ten bytes, widened with a count of nine into a 10-Unicode buffer
//  constructed after the printf; then tbl 4001 '%', 3995 ' ' and 4002 '+' are loaded, in that order. descval picks the layout: 1 the number
//  first, 2 the string first, anything else the string alone -- or, in the cases that build the number in a line first (1, 6, 12, 13, 17, 18,
//  25, 26), no number and no string at all.
//
//      1, 6, 12   the signed number; 12 with a value of 1 leaves the number out, so the label prints alone
//      2, 7       the number and '%'
//      3, 9       the number
//      4, 8       the signed number and '%'; 20 and 21 negate the value, render it again and do the same
//      5, 10      the value * 100 / 128, divided signed (`cdq / and edx, 7Fh / sar eax, 7`), and '%'
//      11         self-repair: 2500 / value frames (`idiv`); over 30, tbl 21242 "Repairs %d durability in %d seconds" with 1 and
//                 (frames + 12) / 25; else tbl 21241 "Repairs %d durability per second" with 1; a value <= 0, tbl 21241 with 25
//      13         the signed number and the class's "all skills" string; a value of 0 returns 0
//      14         the class's tab string formatted with the value, ' ' and the class's "only" string
//      15         chance to cast: the record's positive string whatever the sign or the group (`mov cx, [edx+38h]`, 0x6FAF292A), formatted
//                 with the value, 0, the level and the skill's name -- the 0 is the argument "%%" takes (see below); a skill <= 0 or past
//                 skills.txt returns 0
//      16         an aura: the string formatted with the value and the skill's name, in that order; a NULL name returns 0, a test that
//                 cannot fire (see below)
//      17, 18     by time of day: the period (GetByTimeAdjustment writes the value's low two bits into it), named from
//                 gaPeriodOfDayStrIndices, tbl 3998, then the value now; with no act, its minimum. A negative value now prints no number
//                 unless the stat is negative too; 18 adds '%'. With descval 0 the period's name and the newline stay, alone
//      19         the string is the format, with the value
//      22         as 4, then ':', ' ' and a monster type's plural. The bound is MONPROP's record count (+0C34h) and the index MONTYPE's
//                 table (+0C3Ch); montype.txt's own count (+0C40h) is not read; out of the bound the first record is taken
//      23         as 2, then ' ' and a monster's name; out of range, 0
//      24         charges: "Level N <skill> " and the string formatted with the charges (the value's low byte) and the maximum (the value
//                 `sar` 8). The skill is not bounded here: SKILLS_GetSkillDescStringIndex answers tbl 5382 out of range
//      25, 26     negated after the number was rendered (`neg ebp`, 0x6FAF2DE7): only the sign moves, so a stored -5 prints "+-5" and 5
//                 prints "5". 13 and 25/26 share their layout's code in the original (0x6FAF2E16); written in each here
//      27         "+N to <skill> " and the skill's class's "only" string. The '+' is a strcat onto the emptied line;
//                 SKILLS_IsPlayerClassSkill answers `*pPlayerClass != 7`, so the `>= 7` after it cannot fail
//      28         "+N to <skill>", N capped at 3 when the skill is the class of pUnit if that is a player, of the local player otherwise
//
//  descfunc 6..10 and 21 then add ' ' and descstr2, or tbl 11091 "(Based on Character Level)" where descstr2 is tbl 5382 (`cmp bx, 1506h`).
//  BUG: a case that reaches the end returns 1 whatever it built: an empty line for descval 0 in 1, 6, 12, 13, 25 and 26, for 24's and 28's
//  exits and for 27's first three, a line without its monster for 22's NULL table. The caller joins an empty line like any other (a blank
//  line or a stray ", "). REACHABLE in the retail ESP client, through its three empty descfunc-15 strings; every other route UNREACHABLE with
//  stock data, closed by the data or by a test that cannot fire.
//
//  The formats come from the tables. D2Lang's Unicode::sprintf takes its arguments in order and knows four specifiers, each taking one
//  argument: %d, %u, %s and %%; anything else exits. So 15's literal 0 is what the "%%" of "%d%% Chance to cast level %d %s ..." takes.
//  D2LANG_GetStringFromTblIndex never answers NULL (its lookup takes string 500 for an index past the table and asserts a NULL slot), so the
//  NULL tests after it in 15, 16, 24, 27 and 28 cannot fire; kept, as the original has them.
//  BUG: the DEU, ESP and POL patchstring.tbl of 1.10's Patch_D2.mpq write 16's ModitemAura with the name first, and %s then takes the level
//  for a pointer, so the retail German, Spanish and Polish clients fault on an aura item's description. REACHABLE in the retail DEU, ESP
//  and POL clients of 1.10. The push order here is eng's.
//  BUG: the buffer is the caller's 128 Unicodes; the sprintfs are bounded at 256 (14, 15, 16, 19) or 128 (11, and 24's own line), the strcats
//  not at all. LATENT: the longest line the install's packs can make, every number at eleven characters, is 121 (ITA), under the 127 the
//  buffer holds.
//
//  pUnit is the unit MODS_BuildStatsText was given: an item, or the player when MODS_AppendStateModsText calls; the unit-type tests say so
//  (4 before each blunt bonus, 0 in 28).
BOOL __fastcall MODS_FormatPropertyLine(D2UnitStrc *pUnit, D2StatListStrc *pStatList, D2C_ItemStats nStatId, int32_t nLayer, int32_t nValue, Unicode *wszOut)
{
    D2ItemStatCostTxt *pStatCostTxt;
    D2ItemStatCostTxt *pStatCost;
    D2ItemStatCostTxt *pOpBase;
    D2ItemStatCostTxt *pOther;
    D2CharStatsTxt *pCharStats;
    D2MonTypeTxt *pMonType;
    D2MonStatsTxt *pMonStats;
    D2SkillsTxt *pSkillsTxt;
    D2UnitStrc *pOwner;
    const Unicode *pwszDesc;
    const Unicode *pwszPercent;
    const Unicode *pwszSpace;
    const Unicode *pwszPlus;
    const Unicode *pwszSkill;
    const Unicode *pwszTo;
    const Unicode *pwszSuffix;
    D2C_StringIndices nDescStr;
    D2C_StringIndices nDescStr2;
    int32_t nStatCostCount;
    int32_t nVal;
    int32_t nOtherVal;
    int32_t nOpBase;
    int32_t bScaled;
    int32_t bUseGroup;
    int32_t bLowestMember;
    int32_t nDescFunc;
    int32_t nDescVal;
    int32_t nSkillId;
    int32_t nSkillLevel;
    int32_t nClass;
    int32_t nTab;
    int32_t nFrames;
    int32_t nPeriod;
    int32_t nBaseTime;
    int32_t nPeriodIndex;
    int32_t nTimedValue;
    int32_t nTimedMin;
    int32_t nTimedMax;
    int32_t nCharges;
    int32_t nMaxCharges;
    int32_t nSkillClass;
    int32_t nOwnerClass;
    int32_t nShown;
    int32_t iOther;
    char szValue[10];

    wszOut[0].ch = 0;
    if (nStatId < 0)
        return FALSE;
    nStatCostCount = sgptDataTables->nItemStatCostTxtRecordCount;
    if (nStatId >= nStatCostCount)
        return FALSE;
    pStatCostTxt = sgptDataTables->pItemStatCostTxt;
    pStatCost = &pStatCostTxt[nStatId];
    if (!pStatCost)
        return FALSE;

    nVal = nValue;
    bScaled = 1;
    if (pStatCost->nOp >= 2 && pStatCost->nOp <= 5)
    {
        nOpBase = pStatCost->wOpBase;
        pOpBase = NULL;
        if (nOpBase >= 0 && nOpBase < nStatCostCount)
            pOpBase = &pStatCostTxt[nOpBase];
        if (!pOpBase)
        {
            nVal = 0;
            bScaled = 0;
        }
        else
        {
            nVal = (nValue * (D2COMMON_STATLIST_UnitGetStatValue(CUNIT_GetPlayer(), (D2C_ItemStats)nOpBase, 0) >> pOpBase->nValShift)) >> pStatCost->nOpParam;
        }
    }
    if (bScaled)
    {
        nVal >>= pStatCost->nValShift;
        if (nStatId == STAT_ITEM_UNDEADDAMAGE_PERCENT && pUnit && pUnit->dwUnitType == UNIT_ITEM && D2COMMON_ITEMS_CheckItemTypeId(pUnit, ITEMTYPE_BLUNT))
            nVal += 50;
    }

    bLowestMember = 1;
    bUseGroup = 0;
    if (pStatCost->wDescGrp)
    {
        bUseGroup = 1;
        for (iOther = sgptDataTables->nItemStatCostTxtRecordCount - 1; iOther >= 0; --iOther)
        {
            if (iOther >= sgptDataTables->nItemStatCostTxtRecordCount)
            {
                bUseGroup = 0;
                break;
            }
            pOther = &sgptDataTables->pItemStatCostTxt[iOther];
            if (!pOther)
            {
                bUseGroup = 0;
                break;
            }
            if (pStatCost->wDescGrp != pOther->wDescGrp)
                continue;

            bLowestMember = (iOther == nStatId);
            nOtherVal = D2COMMON_STATLIST_GetStatValue(pStatList, (D2C_ItemStats)iOther, 0);
            bScaled = 1;
            if (pOther->nOp >= 2 && pOther->nOp <= 5)
            {
                nOpBase = pOther->wOpBase;
                pOpBase = NULL;
                if (nOpBase >= 0 && nOpBase < sgptDataTables->nItemStatCostTxtRecordCount)
                    pOpBase = &sgptDataTables->pItemStatCostTxt[nOpBase];
                if (!pOpBase)
                {
                    nOtherVal = 0;
                    bScaled = 0;
                }
                else
                {
                    nOtherVal = (nOtherVal * (D2COMMON_STATLIST_UnitGetStatValue(CUNIT_GetPlayer(), (D2C_ItemStats)nOpBase, 0) >> pOpBase->nValShift)) >> pOther->nOpParam;
                }
            }
            if (bScaled)
            {
                nOtherVal >>= pOther->nValShift;
                if (iOther == STAT_ITEM_UNDEADDAMAGE_PERCENT && pUnit && pUnit->dwUnitType == UNIT_ITEM && D2COMMON_ITEMS_CheckItemTypeId(pUnit, ITEMTYPE_BLUNT))
                    nOtherVal += 50;
            }
            if (nVal != nOtherVal)
            {
                bUseGroup = 0;
                break;
            }
        }
        if (bUseGroup && !bLowestMember)
            return FALSE;
    }

    nDescFunc = pStatCost->nDescFunc;
    nDescVal = pStatCost->nDescVal;
    nDescStr2 = (D2C_StringIndices)pStatCost->wDescStr2;
    if (bUseGroup)
    {
        nDescFunc = pStatCost->nDescGrpFunc;
        nDescVal = pStatCost->nDescGrpVal;
        nDescStr2 = (D2C_StringIndices)pStatCost->wDescGrpStr2;
        nDescStr = (D2C_StringIndices)(nVal < 0 ? pStatCost->wDescGrpStrNeg : pStatCost->wDescGrpStrPos);
    }
    else
    {
        nDescStr = (D2C_StringIndices)(nVal < 0 ? pStatCost->wDescStrNeg : pStatCost->wDescStrPos);
    }
    pwszDesc = D2LANG_GetStringFromTblIndex(nDescStr);

    STORM_SStrPrintf(szValue, 10, "%i", nVal);

    Unicode wszValue[10];

    Unicode::toUnicode(wszValue, szValue, 9);
    pwszPercent = D2LANG_GetStringFromTblIndex(STR_IDX_4001_percent);
    pwszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
    pwszPlus = D2LANG_GetStringFromTblIndex(STR_IDX_4002_plus);

    switch (nDescFunc)
    {
    case 1:
    case 6:
    case 12:
    {
        Unicode wszLine[256];

        wszLine[0].ch = 0;
        if (nVal <= 0)
        {
            Unicode::strcpy(wszLine, wszValue);
        }
        else if (nDescFunc != 12 || nVal > 1)
        {
            Unicode::strcpy(wszLine, pwszPlus);
            Unicode::strcat(wszLine, wszValue);
        }
        if (nDescVal == 1)
        {
            Unicode::strcpy(wszOut, wszLine);
            Unicode::strcat(wszOut, pwszSpace);
            Unicode::strcat(wszOut, pwszDesc);
        }
        else if (nDescVal == 2)
        {
            Unicode::strcpy(wszOut, pwszDesc);
            Unicode::strcat(wszOut, pwszSpace);
            Unicode::strcat(wszOut, wszLine);
        }
        break;
    }

    case 2:
    case 7:
        if (nDescVal == 1)
        {
            Unicode::strcpy(wszOut, wszValue);
            Unicode::strcat(wszOut, pwszPercent);
            Unicode::strcat(wszOut, pwszSpace);
            Unicode::strcat(wszOut, pwszDesc);
        }
        else if (nDescVal == 2)
        {
            Unicode::strcpy(wszOut, pwszDesc);
            Unicode::strcat(wszOut, pwszSpace);
            Unicode::strcat(wszOut, wszValue);
            Unicode::strcat(wszOut, pwszPercent);
        }
        else
        {
            Unicode::strcpy(wszOut, pwszDesc);
        }
        break;

    case 3:
    case 9:
        if (nDescVal == 1)
        {
            Unicode::strcpy(wszOut, wszValue);
            Unicode::strcat(wszOut, pwszSpace);
            Unicode::strcat(wszOut, pwszDesc);
        }
        else if (nDescVal == 2)
        {
            Unicode::strcpy(wszOut, pwszDesc);
            Unicode::strcat(wszOut, pwszSpace);
            Unicode::strcat(wszOut, wszValue);
        }
        else
        {
            Unicode::strcpy(wszOut, pwszDesc);
        }
        break;

    case 20:
    case 21:
        nVal = -nVal;
        STORM_SStrPrintf(szValue, 10, "%i", nVal);
        Unicode::toUnicode(wszValue, szValue, 9);
        //  falls through: the code of cases 4 and 8 is entered from here
    case 4:
    case 8:
        if (nDescVal == 1)
        {
            if (nVal < 0)
            {
                wszOut[0].ch = 0;
                Unicode::strcat(wszOut, wszValue);
            }
            else
            {
                Unicode::strcpy(wszOut, pwszPlus);
                Unicode::strcat(wszOut, wszValue);
            }
            Unicode::strcat(wszOut, pwszPercent);
            Unicode::strcat(wszOut, pwszSpace);
            Unicode::strcat(wszOut, pwszDesc);
        }
        else if (nDescVal == 2)
        {
            Unicode::strcpy(wszOut, pwszDesc);
            Unicode::strcat(wszOut, pwszSpace);
            if (nVal >= 0)
                Unicode::strcat(wszOut, pwszPlus);
            Unicode::strcat(wszOut, wszValue);
            Unicode::strcat(wszOut, pwszPercent);
        }
        else
        {
            Unicode::strcpy(wszOut, pwszDesc);
        }
        break;

    case 5:
    case 10:
        STORM_SStrPrintf(szValue, 10, "%i", 100 * nVal / 128);
        Unicode::toUnicode(wszValue, szValue, 9);
        if (nDescVal == 1)
        {
            Unicode::strcpy(wszOut, wszValue);
            Unicode::strcat(wszOut, pwszPercent);
            Unicode::strcat(wszOut, pwszSpace);
            Unicode::strcat(wszOut, pwszDesc);
        }
        else if (nDescVal == 2)
        {
            Unicode::strcpy(wszOut, pwszDesc);
            Unicode::strcat(wszOut, pwszSpace);
            Unicode::strcat(wszOut, wszValue);
            Unicode::strcat(wszOut, pwszPercent);
        }
        else
        {
            Unicode::strcpy(wszOut, pwszDesc);
        }
        break;

    case 11:
        if (nVal <= 0)
        {
            Unicode::sprintf(128, wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_21241_ModStre9t), 25);
        }
        else
        {
            nFrames = 2500 / nVal;
            if (nFrames > 30)
                Unicode::sprintf(128, wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_21242_ModStre9u), 1, (nFrames + 12) / 25);
            else
                Unicode::sprintf(128, wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_21241_ModStre9t), 1);
        }
        break;

    case 13:
    {
        Unicode wszLine[256];

        wszLine[0].ch = 0;
        if (nVal > 0)
        {
            Unicode::strcpy(wszLine, pwszPlus);
            Unicode::strcat(wszLine, wszValue);
        }
        else if (nVal >= 0)
        {
            return FALSE;
        }
        else
        {
            Unicode::strcpy(wszLine, wszValue);
        }
        if (nLayer < 0 || nLayer >= sgptDataTables->nCharStatsTxtRecordCount)
            return FALSE;
        pCharStats = &sgptDataTables->pCharStatsTxt[nLayer];
        if (!pCharStats)
            return FALSE;
        pwszDesc = D2LANG_GetStringFromTblIndex((D2C_StringIndices)pCharStats->wStrAllSkills);
        if (nDescVal == 1)
        {
            Unicode::strcpy(wszOut, wszLine);
            Unicode::strcat(wszOut, pwszSpace);
            Unicode::strcat(wszOut, pwszDesc);
        }
        else if (nDescVal == 2)
        {
            Unicode::strcpy(wszOut, pwszDesc);
            Unicode::strcat(wszOut, pwszSpace);
            Unicode::strcat(wszOut, wszLine);
        }
        break;
    }

    case 14:
        nTab = nLayer & 7;
        nClass = nLayer >> 3;
        if (nClass < 0 || nClass >= sgptDataTables->nCharStatsTxtRecordCount)
            return FALSE;
        pCharStats = &sgptDataTables->pCharStatsTxt[nClass];
        if (!pCharStats)
            return FALSE;
        if (nTab < 0 || nTab >= 3)
            return FALSE;
        Unicode::sprintf(256, wszOut, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pCharStats->wStrSkillTab[nTab]), nVal);
        Unicode::strcat(wszOut, pwszSpace);
        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pCharStats->wStrClassOnly));
        break;

    case 15:
        nSkillId = nLayer >> sgptDataTables->nStuff;
        nSkillLevel = nLayer & sgptDataTables->nShiftedStuff;
        if (nSkillId <= 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount)
            return FALSE;
        pwszSkill = D2LANG_GetStringFromTblIndex((D2C_StringIndices) SKILLS_GetSkillDescStringIndex((D2C_Skills)nSkillId, 0));
        //  the 0 is the argument the format's "%%" takes (see the head)
        Unicode::sprintf(256, wszOut, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pStatCost->wDescStrPos), nVal, 0, nSkillLevel, pwszSkill);
        break;

    case 16:
        pwszSkill = D2LANG_GetStringFromTblIndex((D2C_StringIndices) SKILLS_GetSkillDescStringIndex((D2C_Skills)nLayer, 0));
        if (!pwszSkill)
            return FALSE;
        Unicode::sprintf(256, wszOut, pwszDesc, nVal, pwszSkill);
        break;

    case 17:
    case 18:
    {
        Unicode wszLine[256];

        wszLine[0].ch = 0;
        nPeriod = 0;
        nBaseTime = 0;
        if (gpAct)
            nPeriod = D2COMMON_ENVIRONMENT_GetPeriodOfDayFromAct(gpAct, &nBaseTime);
        nTimedValue = D2COMMON_ITEMMODS_GetByTimeAdjustment(nVal, nPeriod, nBaseTime, &nPeriod, &nTimedMin, &nTimedMax);
        if (!gpAct)
            nTimedValue = nTimedMin;
        if (nPeriod <= 0)
            nPeriodIndex = 0;
        else if (nPeriod >= 3)
            nPeriodIndex = 3;
        else
            nPeriodIndex = nPeriod;
        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(gaPeriodOfDayStrIndices[nPeriodIndex]));
        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
        STORM_SStrPrintf(szValue, 10, "%i", nTimedValue);
        Unicode::toUnicode(wszValue, szValue, 9);
        if (nTimedValue >= 0)
        {
            Unicode::strcpy(wszLine, pwszPlus);
            Unicode::strcat(wszLine, wszValue);
        }
        else if (nVal < 0)
        {
            Unicode::strcpy(wszLine, wszValue);
        }
        if (nDescFunc == 18)
            Unicode::strcat(wszLine, pwszPercent);
        if (nDescVal == 1)
        {
            Unicode::strcat(wszOut, wszLine);
            Unicode::strcat(wszOut, pwszSpace);
            Unicode::strcat(wszOut, pwszDesc);
        }
        else if (nDescVal == 2)
        {
            Unicode::strcat(wszOut, pwszDesc);
            Unicode::strcat(wszOut, pwszSpace);
            Unicode::strcat(wszOut, wszLine);
        }
        break;
    }

    case 19:
        Unicode::sprintf(256, wszOut, pwszDesc, nVal);
        break;

    case 22:
        if (nDescVal == 1)
        {
            if (nVal < 0)
                wszOut[0].ch = 0;
            else
                Unicode::strcpy(wszOut, pwszPlus);
            Unicode::strcat(wszOut, wszValue);
            Unicode::strcat(wszOut, pwszPercent);
            Unicode::strcat(wszOut, pwszSpace);
            Unicode::strcat(wszOut, pwszDesc);
        }
        else if (nDescVal == 2)
        {
            Unicode::strcpy(wszOut, pwszDesc);
            Unicode::strcat(wszOut, pwszSpace);
            if (nVal >= 0)
                Unicode::strcat(wszOut, pwszPlus);
            Unicode::strcat(wszOut, wszValue);
            Unicode::strcat(wszOut, pwszPercent);
        }
        else
        {
            Unicode::strcpy(wszOut, pwszDesc);
        }
        //  BUG: bounded by monprop.txt's count, indexing montype.txt's table. UNREACHABLE with stock data: no stock stat has descfunc 22.
        if (nLayer >= 0 && nLayer < sgptDataTables->nMonPropTxtRecordCount)
            pMonType = &sgptDataTables->pMonTypeTxt[nLayer];
        else
            pMonType = sgptDataTables->pMonTypeTxt;
        if (!pMonType)
            break;
        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3997_colon));
        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pMonType->wStrPlur));
        break;

    case 23:
        if (nLayer < 0 || nLayer >= sgptDataTables->nMonStatsTxtRecordCount)
            return FALSE;
        pMonStats = &sgptDataTables->pMonStatsTxt[nLayer];
        if (!pMonStats)
            return FALSE;
        if (nDescVal == 1)
        {
            Unicode::strcpy(wszOut, wszValue);
            Unicode::strcat(wszOut, pwszPercent);
            Unicode::strcat(wszOut, pwszSpace);
            Unicode::strcat(wszOut, pwszDesc);
        }
        else if (nDescVal == 2)
        {
            Unicode::strcpy(wszOut, pwszDesc);
            Unicode::strcat(wszOut, pwszSpace);
            Unicode::strcat(wszOut, wszValue);
            Unicode::strcat(wszOut, pwszPercent);
        }
        else
        {
            Unicode::strcpy(wszOut, pwszDesc);
        }
        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pMonStats->wNameStr));
        break;

    case 24:
    {
        nSkillId = nLayer >> sgptDataTables->nStuff;
        nSkillLevel = nLayer & sgptDataTables->nShiftedStuff;
        nMaxCharges = nVal >> 8;
        nCharges = nVal & 0xFF;
        pwszSkill = D2LANG_GetStringFromTblIndex((D2C_StringIndices) SKILLS_GetSkillDescStringIndex((D2C_Skills)nSkillId, 0));
        if (!pwszSkill)
            break;

        Unicode wszLine[256];

        wszLine[0].ch = 0;
        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_21249_ModStre10b));
        Unicode::strcat(wszOut, pwszSpace);
        STORM_SStrPrintf(szValue, 10, "%i", nSkillLevel);
        Unicode::toUnicode(wszValue, szValue, 9);
        Unicode::strcat(wszOut, wszValue);
        Unicode::strcat(wszOut, pwszSpace);
        Unicode::strcat(wszOut, pwszSkill);
        Unicode::strcat(wszOut, pwszSpace);
        Unicode::sprintf(128, wszLine, pwszDesc, nCharges, nMaxCharges);
        Unicode::strcat(wszOut, wszLine);
        break;
    }

    case 25:
    case 26:
    {
        Unicode wszLine[256];

        //  BUG: negated after the number was rendered (see the head). UNREACHABLE with stock data: no stock stat has descfunc 25 or 26.
        nVal = -nVal;
        wszLine[0].ch = 0;
        if (nVal > 0)
        {
            Unicode::strcpy(wszLine, pwszPlus);
            Unicode::strcat(wszLine, wszValue);
        }
        else
        {
            Unicode::strcpy(wszLine, wszValue);
        }
        if (nDescVal == 1)
        {
            Unicode::strcpy(wszOut, wszLine);
            Unicode::strcat(wszOut, pwszSpace);
            Unicode::strcat(wszOut, pwszDesc);
        }
        else if (nDescVal == 2)
        {
            Unicode::strcpy(wszOut, pwszDesc);
            Unicode::strcat(wszOut, pwszSpace);
            Unicode::strcat(wszOut, wszLine);
        }
        break;
    }

    case 27:
        pwszTo = D2LANG_GetStringFromTblIndex(STR_IDX_4003_to);
        if (!pwszTo)
            break;
        if (!nVal)
            break;
        pwszSkill = D2LANG_GetStringFromTblIndex((D2C_StringIndices) SKILLS_GetSkillDescStringIndex((D2C_Skills)nLayer, 0));
        if (!pwszSkill)
            break;
        if (nVal > 0)
        {
            Unicode::strcat(wszOut, pwszPlus);
            Unicode::strcat(wszOut, wszValue);
        }
        else
        {
            Unicode::strcpy(wszOut, wszValue);
        }
        Unicode::strcat(wszOut, pwszSpace);
        Unicode::strcat(wszOut, pwszTo);
        Unicode::strcat(wszOut, pwszSpace);
        Unicode::strcat(wszOut, pwszSkill);
        Unicode::strcat(wszOut, pwszSpace);
        nSkillClass = PCLASS_INVALID;
        if (!D2COMMON_SKILLS_IsPlayerClassSkill((D2C_Skills)nLayer, &nSkillClass))
            break;
        if (nSkillClass < 0 || nSkillClass >= (int32_t)NUMBER_OF_PLAYERCLASSES)
            break;
        if (nSkillClass >= sgptDataTables->nCharStatsTxtRecordCount)
            break;
        pCharStats = &sgptDataTables->pCharStatsTxt[nSkillClass];
        if (!pCharStats)
            break;
        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pCharStats->wStrClassOnly));
        break;

    case 28:
        nShown = nVal;
        if (!nVal)
            break;
        if (nLayer < 0)
            break;
        if (nLayer >= sgptDataTables->nSkillsTxtRecordCount)
            break;
        pSkillsTxt = &sgptDataTables->pSkillsTxt[nLayer];
        if (!pSkillsTxt)
            break;
        pOwner = pUnit;
        if (!pUnit || pUnit->dwUnitType != UNIT_PLAYER)
            pOwner = CUNIT_GetPlayer();
        nOwnerClass = pOwner ? (int32_t)pOwner->dwClassId : -1;
        if (pSkillsTxt->nCharClass == nOwnerClass && nVal > 3)
            nShown = 3;
        pwszTo = D2LANG_GetStringFromTblIndex(STR_IDX_4003_to);
        if (!pwszTo)
            break;
        pwszSkill = D2LANG_GetStringFromTblIndex((D2C_StringIndices) SKILLS_GetSkillDescStringIndex((D2C_Skills)nLayer, 0));
        if (!pwszSkill)
            break;
        STORM_SStrPrintf(szValue, 10, "%i", nShown);
        Unicode::toUnicode(wszValue, szValue, 9);
        if (nShown > 0)
        {
            Unicode::strcpy(wszOut, pwszPlus);
            Unicode::strcat(wszOut, wszValue);
        }
        else
        {
            Unicode::strcpy(wszOut, wszValue);
        }
        Unicode::strcat(wszOut, pwszSpace);
        Unicode::strcat(wszOut, pwszTo);
        Unicode::strcat(wszOut, pwszSpace);
        Unicode::strcat(wszOut, pwszSkill);
        break;

    default:
        return FALSE;
    }

    if (nDescFunc >= 6 && (nDescFunc <= 10 || nDescFunc == 21))
    {
        if (nDescStr2 == STR_IDX_5382_dummy)
        {
            pwszSuffix = D2LANG_GetStringFromTblIndex(STR_IDX_11091_increaseswithplaylevelX);
            Unicode::strcat(wszOut, pwszSpace);
            Unicode::strcat(wszOut, pwszSuffix);
        }
        else
        {
            Unicode::strcat(wszOut, pwszSpace);
            Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(nDescStr2));
        }
    }
    return TRUE;
}


//1.10f: D2Client.0x6FAF3160
//  The magic-mods block. MODS_BuildStatsText renders the item's STATLIST_MAGIC list, and its STATE_RUNEWORD list as the source state, into a
//  1024-Unicode local, with the blunt weapon's undead bonus allowed (`push 1`), and the text goes onto wszOut:
//
//      no wszSep, or nothing built     appended whole
//      wszSep, no newline to split at  wszSep, then the text
//      wszSep and a newline at nPos    the text up to nPos, tbl 3998, wszSep, the last line -- its own trailing newline cut -- tbl 3852 ',',
//                                      tbl 3998
//
//  The newline is searched for backwards from one short of the last character, and index 0 is never tested (`jle` / `jg`, signed); the "last
//  character" is taken at the length computed before the cut.
//  bAppendNewline is MODS_BuildStatsText's eighth argument, pushed straight through: set, the builder ends every line with tbl 3998; clear,
//  it joins the lines with tbl 3852 ',' and tbl 3995 ' ' instead.
//  nMaxLen is not read: the 1024 is an immediate (`push 400h`, 0x6FAF3199).
//  void: seven call sites discard eax, and the eighth is a tail jump from INV_DrawItemDescription, which is void.
void __fastcall MODS_AppendMagicModsText(D2UnitStrc *pUnit, Unicode *wszOut, int32_t nMaxLen, int32_t bAppendNewline, const Unicode *wszSep)
{
    const Unicode *pwszNewline;
    const Unicode *pwszComma;
    int32_t nLen;
    int32_t nPos;

    UNREFERENCED_PARAMETER(nMaxLen);

    Unicode wszMods[1024];

    wszMods[0].ch = 0;
    MODS_BuildStatsText(pUnit, wszMods, 1024, 1, STATE_NONE, STATE_RUNEWORD, STATLIST_MAGIC, bAppendNewline);
    if (!wszSep)
    {
        Unicode::strcat(wszOut, wszMods);
        return;
    }

    nLen = Unicode::strlen(wszMods);
    if (!nLen)
    {
        Unicode::strcat(wszOut, wszMods);
        return;
    }

    nPos = nLen - 2;
    while (nPos > 0 && wszMods[nPos].ch != 10)
        --nPos;

    if (nPos > 0)
    {
        pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
        pwszComma = D2LANG_GetStringFromTblIndex(STR_IDX_3852_KeyComma);
        wszMods[nPos].ch = 0;
        Unicode::strcat(wszOut, wszMods);
        Unicode::strcat(wszOut, pwszNewline);
        Unicode::strcat(wszOut, wszSep);
        if (wszMods[nLen - 1].ch == 10)
            wszMods[nLen - 1].ch = 0;
        Unicode::strcat(wszOut, &wszMods[nPos + 1]);
        Unicode::strcat(wszOut, pwszComma);
        Unicode::strcat(wszOut, pwszNewline);
        return;
    }

    Unicode::strcat(wszOut, wszSep);
    Unicode::strcat(wszOut, wszMods);
}


//1.10f: D2Client.0x6FAF32B0
//  The set item's own bonuses, by the pieces worn. For a set item (UNIT_ITEM, quality ITEMQUAL_SET, a setitems.txt record), the record's add
//  func (+0x87, a byte) chooses which of the six set-bonus states' lists MODS_BuildStatsText renders onto wszOut, each through the item's own
//  lists (the state as nState, no source state, STATLIST_BASE, a newline after each line):
//
//      1   by which pieces are worn: ITEMS_GetSetItemsMask(pPlayer, pItem, FALSE), bit i for the set's piece i; the item's own piece
//          (ITEMS_GetNoOfSetItemsFromItem) is skipped, and past it the state index is one lower -- six bits for five states
//      2   by how many are worn: the mask with bDontIgnoreInputItem TRUE through gnSixBitPopCount -- a mask of 64 or more counts as 0 --
//          and the first count-1 states
//      else nothing
//
//  gnSetBonusStates is STATE_ITEMSET1..6 (165..170) and gnSixBitPopCount the population count of 0..63. The mask test is `jb`, unsigned; the
//  loops and the own-slot test are signed (`jl`, `jle`).
//  pPlayer is not tested: ITEMS_GetSetItemsMask answers 0 for NULL, so no bonus is rendered.
void __fastcall MODS_AppendSetBonusText(D2UnitStrc *pPlayer, D2UnitStrc *pItem, Unicode *wszOut, int32_t nMaxLen)
{
    D2SetItemsTxt *pSetItemsTxt;
    uint32_t dwSetItemsMask;
    int32_t nPieces;
    int32_t nExtraPieces;
    int32_t nOwnSlot;
    int32_t nBonusIndex;
    int32_t i;

    if (!pItem || pItem->dwUnitType != UNIT_ITEM || D2COMMON_ITEMS_GetItemQuality(pItem) != ITEMQUAL_SET)
        return;

    pSetItemsTxt = D2COMMON_ITEMS_GetSetItemsTxtRecordFromItem(pItem);
    if (!pSetItemsTxt)
        return;

    switch (pSetItemsTxt->nAddFunc)
    {
    case 1:
        dwSetItemsMask = D2COMMON_ITEMS_GetSetItemsMask(pPlayer, pItem, FALSE);
        nOwnSlot = D2COMMON_ITEMS_GetNoOfSetItemsFromItem(pItem);
        for (i = 0; i < 6; ++i)
        {
            if (i == nOwnSlot)
                continue;
            nBonusIndex = i;
            if (i > nOwnSlot)
                --nBonusIndex;
            if ((1 << i) & dwSetItemsMask)
                MODS_BuildStatsText(pItem, wszOut, nMaxLen, 0, gnSetBonusStates[nBonusIndex], STATE_NONE, STATLIST_BASE, 1);
        }
        break;

    case 2:
        dwSetItemsMask = D2COMMON_ITEMS_GetSetItemsMask(pPlayer, pItem, TRUE);
        nPieces = dwSetItemsMask < 64 ? gnSixBitPopCount[dwSetItemsMask] : 0;
        nExtraPieces = nPieces - 1;
        for (i = 0; i < nExtraPieces; ++i)
            MODS_BuildStatsText(pItem, wszOut, nMaxLen, 0, gnSetBonusStates[i], STATE_NONE, STATLIST_BASE, 1);
        break;
    }
}


//1.10f: D2Client.0x6FAF33C0
//  The full set's bonus, one state. For a set item, the first of the six set-bonus states under which pUnit (the player) holds a stat list
//  whose STAT_VALUE equals the item's set (setitems.txt `set`, nSetId, +0x2C, a signed 16-bit `movsx`): that state's list is rendered onto
//  wszOut through pUnit's lists, as MODS_AppendSetBonusText renders its states. Nothing is rendered when no state matches.
//  One state, not every one that matches: the compare's `jz` leaves the loop, MODS_BuildStatsText is called once and the function returns.
//  pItem is not tested before ITEMS_GetItemQuality, which answers 2 (not a set item) for NULL. The original inlines
//  DATATBLS_GetSetItemsTxtRecord here.
void __fastcall MODS_AppendStateModsText(D2UnitStrc *pUnit, D2UnitStrc *pItem, Unicode *wszOut, int32_t nMaxLen)
{
    D2SetItemsTxt *pSetItemsTxt;
    D2StatListStrc *pStatList;
    int32_t i;

    if (D2COMMON_ITEMS_GetItemQuality(pItem) != ITEMQUAL_SET)
        return;

    pSetItemsTxt = DATATBLS_GetSetItemsTxtRecord(D2COMMON_ITEMS_GetFileIndex(pItem));
    if (!pSetItemsTxt)
        return;

    for (i = 0; i < 6; ++i)
    {
        pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, gnSetBonusStates[i]);
        if (pStatList && pSetItemsTxt->nSetId == D2COMMON_STATLIST_GetStatValue(pStatList, STAT_VALUE, 0))
        {
            MODS_BuildStatsText(pUnit, wszOut, nMaxLen, 0, gnSetBonusStates[i], STATE_NONE, STATLIST_BASE, 1);
            return;
        }
    }
}


//  The seven damage groups' fields at the offsets the switch reads, and the stat ids the jump table is indexed by, less 17.
static_assert(offsetof(D2ClientDamageGroupStrc, nMin) == 0x00
              && offsetof(D2ClientDamageGroupStrc, nMax) == 0x04
              && offsetof(D2ClientDamageGroupStrc, nLengthFrames) == 0x08
              && offsetof(D2ClientDamageGroupStrc, nPoisonCount) == 0x0C
              && offsetof(D2ClientDamageGroupStrc, bPresent) == 0x10
              && offsetof(D2ClientDamageGroupStrc, bIsRange) == 0x14,
              "D2ClientDamageGroupStrc: min, max, length, count, present, range");
static_assert(STAT_ITEM_MAXDAMAGE_PERCENT == 17 && STAT_ITEM_MINDAMAGE_PERCENT == 18
              && STAT_MINDAMAGE == 21 && STAT_MAXDAMAGE == 22
              && STAT_SECONDARY_MINDAMAGE == 23 && STAT_SECONDARY_MAXDAMAGE == 24,
              "the enhanced and physical damage cases, 17..24");
static_assert(STAT_FIREMINDAM == 48 && STAT_FIREMAXDAM == 49 && STAT_LIGHTMINDAM == 50
              && STAT_LIGHTMAXDAM == 51 && STAT_MAGICMINDAM == 52 && STAT_MAGICMAXDAM == 53
              && STAT_COLDMINDAM == 54 && STAT_COLDMAXDAM == 55 && STAT_POISONMINDAM == 57
              && STAT_POISONMAXDAM == 58 && STAT_POISONLENGTH == 59,
              "the elemental and poison cases, 48..59");
static_assert(STR_IDX_3612_strModFireDamage == 0xE1C
              && STR_IDX_3613_strModFireDamageRange == 0xE1D
              && STR_IDX_3614_strModColdDamage == 0xE1E
              && STR_IDX_3615_strModColdDamageRange == 0xE1F
              && STR_IDX_3616_strModLightningDamage == 0xE20
              && STR_IDX_3617_strModLightningDamageRange == 0xE21
              && STR_IDX_3618_strModMagicDamage == 0xE22
              && STR_IDX_3619_strModMagicDamageRange == 0xE23
              && STR_IDX_3620_strModPoisonDamage == 0xE24
              && STR_IDX_3621_strModPoisonDamageRange == 0xE25
              && STR_IDX_3623_strModMinDamageRange == 0xE27
              && STR_IDX_10023_strModEnhancedDamage == 0x2727,
              "the damage formats: mov ecx, 0E1Ch .. 0E27h, 2727h");


//1.10f: D2Client.0x6FAF3460
//  The damage lines. Called once, by MODS_BuildStatsText for each stat, which skips the ordinary property line when this answers non-zero.
//  pDamageGroups is the builder's seven 24-byte groups -- [0] physical, [1] enhanced damage, [2] cold, [3] lightning, [4] fire, [5] poison,
//  [6] magic -- which its pairing pass fills:
//
//      17, 49, 51, 53, 55,   the "max" half of a pair: nothing drawn, the group's bPresent answered, so the pair prints once, from its "min"
//      58, 59                stat
//      18                    "+N% Enhanced Damage" from group 1's nMin -- formatted before bPresent is tested; clear, 0 and no line
//      21, 22, 23            the physical range, "Adds %d-%d damage", drawn once: a set bIsRange means an earlier stat drew it, and the arm
//                            appends the empty line and answers 1; with nMin >= nMax both flags are cleared and 0 answered, and the
//                            ordinary lines print the stats
//      24                    1 once the range is drawn, else bPresent
//      48, 50, 52, 54        fire, lightning, magic, cold: with nMin < nMax a range from the ...Range format, else one value -- nMax, not
//                            nMin -- from the flat one
//      57                    poison, below
//      anything else         0 (19, 20, 25..47 and 56 go to the default through the jump table, the rest by the range test)
//
//  The line is built with Unicode::sprintf into a 256-Unicode local, which is constructed and then cleared whole (`rep stosd`); every arm
//  that draws ends in one shared tail, the append to pwszDest and 1 -- here the `break` out of the switch. The formats carry their own newline.
//  Poison (57): the count (+0x0C: stat 326, STAT_POISON_COUNT, which the builder reads off the unit into it) is made at least 1; the length
//  is divided by it (`idiv`) and written back; min and max are scaled by that many frames with rounding, `(x * frames + 128) >> 8`, the shifts
//  arithmetic (`sar`), and written back; the seconds are the frames over 25, a signed division by multiply (0x51EB851F). Then a range with
//  the seconds, or the one value -- nMax -- with them.
//  DIFFERENCE: the two products are taken in unsigned arithmetic and converted back. `imul` wraps; a signed overflow in C is undefined. The
//  bits are the same.
int32_t __fastcall MODS_FormatElementalDamageText(D2ClientDamageGroupStrc *pDamageGroups, D2C_ItemStats nStatId, Unicode *pwszDest)
{
    D2ClientDamageGroupStrc *pGroup;
    int32_t nFrames;
    int32_t nMin;
    int32_t nMax;
    int32_t nSeconds;

    Unicode wszLine[256];

    memset(wszLine, 0, sizeof(wszLine));

    switch (nStatId)
    {
    case STAT_ITEM_MAXDAMAGE_PERCENT:
        return pDamageGroups[1].bPresent;

    case STAT_ITEM_MINDAMAGE_PERCENT:
    {
        const Unicode *pwszPercent = D2LANG_GetStringFromTblIndex(STR_IDX_4001_percent);
        const Unicode *pwszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
        const Unicode *pwszPlus = D2LANG_GetStringFromTblIndex(STR_IDX_4002_plus);

        Unicode wszValue[10];
        char szValue[10];

        STORM_SStrPrintf(szValue, 10, "%i", pDamageGroups[1].nMin);
        Unicode::toUnicode(wszValue, szValue, 9);
        if (!pDamageGroups[1].bPresent)
            return 0;
        Unicode::strcpy(wszLine, pwszPlus);
        Unicode::strcat(wszLine, wszValue);
        Unicode::strcat(wszLine, pwszPercent);
        Unicode::strcat(wszLine, pwszSpace);
        Unicode::strcat(wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_10023_strModEnhancedDamage));
        break;
    }

    case STAT_MINDAMAGE:
    case STAT_MAXDAMAGE:
    case STAT_SECONDARY_MINDAMAGE:
        pGroup = &pDamageGroups[0];
        if (pGroup->bIsRange)
            break;
        if (!pGroup->bPresent)
            return 0;
        if (pGroup->nMin >= pGroup->nMax)
        {
            pGroup->bIsRange = 0;
            pGroup->bPresent = 0;
            return 0;
        }
        pGroup->bIsRange = 1;
        Unicode::sprintf(256, wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_3623_strModMinDamageRange), pGroup->nMin, pGroup->nMax);
        break;

    case STAT_SECONDARY_MAXDAMAGE:
        if (pDamageGroups[0].bIsRange)
            return 1;
        return pDamageGroups[0].bPresent;

    case STAT_FIREMINDAM:
        pGroup = &pDamageGroups[4];
        if (!pGroup->bPresent)
            return 0;
        if (pGroup->nMin < pGroup->nMax)
            Unicode::sprintf(256, wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_3613_strModFireDamageRange), pGroup->nMin, pGroup->nMax);
        else
            Unicode::sprintf(256, wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_3612_strModFireDamage), pGroup->nMax);
        break;

    case STAT_FIREMAXDAM:
        return pDamageGroups[4].bPresent;

    case STAT_LIGHTMINDAM:
        pGroup = &pDamageGroups[3];
        if (!pGroup->bPresent)
            return 0;
        if (pGroup->nMin < pGroup->nMax)
            Unicode::sprintf(256, wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_3617_strModLightningDamageRange), pGroup->nMin, pGroup->nMax);
        else
            Unicode::sprintf(256, wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_3616_strModLightningDamage), pGroup->nMax);
        break;

    case STAT_LIGHTMAXDAM:
        return pDamageGroups[3].bPresent;

    case STAT_MAGICMINDAM:
        pGroup = &pDamageGroups[6];
        if (!pGroup->bPresent)
            return 0;
        if (pGroup->nMin < pGroup->nMax)
            Unicode::sprintf(256, wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_3619_strModMagicDamageRange), pGroup->nMin, pGroup->nMax);
        else
            Unicode::sprintf(256, wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_3618_strModMagicDamage), pGroup->nMax);
        break;

    case STAT_MAGICMAXDAM:
        return pDamageGroups[6].bPresent;

    case STAT_COLDMINDAM:
        pGroup = &pDamageGroups[2];
        if (!pGroup->bPresent)
            return 0;
        if (pGroup->nMin < pGroup->nMax)
            Unicode::sprintf(256, wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_3615_strModColdDamageRange), pGroup->nMin, pGroup->nMax);
        else
            Unicode::sprintf(256, wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_3614_strModColdDamage), pGroup->nMax);
        break;

    case STAT_COLDMAXDAM:
        return pDamageGroups[2].bPresent;

    case STAT_POISONMINDAM:
        pGroup = &pDamageGroups[5];
        if (!pGroup->bPresent)
            return 0;
        if (pGroup->nPoisonCount <= 0)
            pGroup->nPoisonCount = 1;
        nFrames = pGroup->nLengthFrames / pGroup->nPoisonCount;
        pGroup->nLengthFrames = nFrames;
        //  DIFFERENCE: unsigned products (see the head)
        nMin = (int32_t)((uint32_t)pGroup->nMin * (uint32_t)nFrames + 128u) >> 8;
        nMax = (int32_t)((uint32_t)pGroup->nMax * (uint32_t)nFrames + 128u) >> 8;
        nSeconds = nFrames / 25;
        pGroup->nMin = nMin;
        pGroup->nMax = nMax;
        if (nMin < nMax)
            Unicode::sprintf(256, wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_3621_strModPoisonDamageRange), nMin, nMax, nSeconds);
        else
            Unicode::sprintf(256, wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_3620_strModPoisonDamage), nMax, nSeconds);
        break;

    case STAT_POISONMAXDAM:
    case STAT_POISONLENGTH:
        return pDamageGroups[5].bPresent;

    default:
        return 0;
    }

    Unicode::strcat(pwszDest, wszLine);
    return 1;
}

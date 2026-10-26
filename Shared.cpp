#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"

//  Shared.cpp -- not one of the original's files. It holds the functions that cannot be placed in a single module: the out-of-line bodies of
//  inline functions that the original inlines in several units (most of the functions here), and orphans that lie between two units with
//  nothing in the bytes to choose between them. Each function's head says which.


//1.10f: D2Client.0x6FAA6150
//  Returns the room that holds a subtile, searched outward from one nearby: pRoom itself if its bounds hold the point, else the first adjacent
//  room that does, else NULL -- and NULL for a NULL room. The original inlines this function in several units.
//  __fastcall over retn 4: nSubtileY is the one stack argument, and once it is in edi the compiler reuses its slot for the adjacent-room count
//  (0x6FAA6191); the C gives the count a local of its own. The bounds compare signed (jl/jge), the count unsigned (jbe 0x6FAA61A0, jb 0x6FAA61D3).
D2ActiveRoomStrc *__fastcall CLIENT_FindRoomContainingSubtile(D2ActiveRoomStrc *pRoom, int32_t nSubtileX, int32_t nSubtileY)
{
    D2ActiveRoomStrc **ppRoomList;
    D2ActiveRoomStrc *pCandidate;
    int32_t nNumRooms;
    uint32_t i;

    if (!pRoom)
        return NULL;

    if (nSubtileX >= pRoom->tCoords.nSubtileX && nSubtileX < pRoom->tCoords.nSubtileX + pRoom->tCoords.nSubtileWidth && nSubtileY >= pRoom->tCoords.nSubtileY && nSubtileY < pRoom->tCoords.nSubtileY + pRoom->tCoords.nSubtileHeight)
    {
        return pRoom;
    }

    ppRoomList = NULL;
    nNumRooms = 0;
    D2COMMON_DUNGEON_GetAdjacentRoomsListFromRoom(pRoom, &ppRoomList, &nNumRooms);
    for (i = 0; i < (uint32_t)nNumRooms; ++i)
    {
        pCandidate = ppRoomList[i];
        if (pCandidate && nSubtileX >= pCandidate->tCoords.nSubtileX && nSubtileX < pCandidate->tCoords.nSubtileX + pCandidate->tCoords.nSubtileWidth && nSubtileY >= pCandidate->tCoords.nSubtileY && nSubtileY < pCandidate->tCoords.nSubtileY
                               + pCandidate->tCoords.nSubtileHeight)
        {
            return pCandidate;
        }
    }
    return NULL;
}


//1.10f: D2Client.0x6FAA88D0
//  D2Common's SEED_RollRandomNumber, out of line: seed = 0x6AC690C5 * low + high, stored back whole and returned in edx:eax (uint64_t).
//  mul edx is the unsigned 32 x 32 -> 64 product, and add eax, esi / adc edx, edi, with edi zeroed, adds the high half zero-extended. The bytes
//  store the high half first (0x6FAA88E5, then 0x6FAA88E8): two stores to two dwords, written in that order. The seed is read at [ecx] and
//  [ecx+4], the struct's nLowSeed and nHighSeed. The original inlines it in many units.
uint64_t __fastcall SEED_RollRandomNumber(D2SeedStrc *pSeed)
{
    const uint64_t lSeed = (uint64_t)pSeed->nLowSeed * 0x6AC690C5u + pSeed->nHighSeed;
    pSeed->nHighSeed = (uint32_t)(lSeed >> 32);
    pSeed->nLowSeed = (uint32_t)lSeed;
    return lSeed;
}

//1.10f: D2Client.0x6FAA88F0
//  D2Common's SEED_RollLimitedRandomNumber, out of line, with SEED_RollRandomNumber folded in. A roll: both arms store the advanced seed back,
//  low half first (0x6FAA8919/0x6FAA891B and 0x6FAA8932/0x6FAA8938). The bound's test is jg (0x6FAA88F7), signed; the remainder is
//  xor edx, edx / div esi, unsigned, of the low half; the power-of-two arm masks the low half. The seed is read at [ecx] and [ecx+4].
//  The return is int: every test of the answer read in the image is signed (jl, jge). The original inlines this function in many units.
int32_t __fastcall SEED_RollLimitedRandomNumber(D2SeedStrc *pSeed, int32_t nMax)
{
    if (nMax <= 0)
        return 0;

    const uint64_t lSeed = (uint64_t)pSeed->nLowSeed * 0x6AC690C5u + pSeed->nHighSeed;
    pSeed->nLowSeed = (uint32_t)lSeed;
    pSeed->nHighSeed = (uint32_t)(lSeed >> 32);
    if ((nMax - 1) & nMax)
        return (int32_t)((uint32_t)lSeed % (uint32_t)nMax);
    return (int32_t)((uint32_t)lSeed & (uint32_t)(nMax - 1));
}

//  No address of its own: D2Common's SEED_RollLimitedRandomNumber as its header inline has it, transcribed unchanged but for the name, for the
//  sites where the original inlines it instead of calling 0x6FAA88F0 above. One definition is shared by every unit that uses it. The Inline
//  suffix keeps it apart from the out-of-line copy.
uint32_t __fastcall SEED_RollLimitedRandomNumberInline(D2SeedStrc *pSeed, int32_t nMax)
{
    if (nMax > 0)
    {
        if ((nMax - 1) & nMax)
            return (uint32_t)SEED_RollRandomNumber(pSeed) % nMax;
        else
            return (uint32_t)(SEED_RollRandomNumber(pSeed) & (nMax - 1));
    }

    return 0;
}


//1.10f: D2Client.0x6FAAE570
//  &sgptDataTables->pMonStatsTxt[nMonStatsId], or NULL for a negative id or one at or past the count (jl, jge, signed). D2Common's header
//  inline, emitted out of line; the original inlines it in many units.
D2MonStatsTxt *__fastcall DATATBLS_GetMonStatsTxtRecord(int32_t nMonStatsId)
{
    if (nMonStatsId >= 0 && nMonStatsId < sgptDataTables->nMonStatsTxtRecordCount)
    {
        return &sgptDataTables->pMonStatsTxt[nMonStatsId];
    }

    return NULL;
}


//1.10f: D2Client.0x6FAB6A60
//  UNITS_GetXPosition and UNITS_GetYPosition: a D2Common body, emitted out of line; the original writes it in place in most of its callers.
//  D2Common's UNITS_GetXPosition (0x6FD925E0) and UNITS_GetYPosition (0x6FD92610), both 41 bytes, are these two instruction for instruction.
//  Neither is exported, so D2Client compiled the same source. The value is a game (subtile) coordinate.
//  OBJECT, ITEM and TILE read pStaticPath->tGameCoords untested, and every other type -- an invalid one too -- goes to PATH_GetXPosition when
//  the dynamic path is not NULL. The type bounds are jle, signed (0x6FAB6A6A, 0x6FAB6A6F).
//  BUG: the static path is not tested for NULL. LATENT: CUNIT_InitUnit allocates one for exactly those three types.
int32_t __fastcall UNITS_GetXPosition(D2UnitStrc *pUnit)
{
    if (pUnit->dwUnitType == UNIT_OBJECT || ((int32_t)pUnit->dwUnitType > (int32_t)UNIT_MISSILE && (int32_t)pUnit->dwUnitType <= (int32_t)UNIT_TILE))
    {
        return pUnit->pStaticPath->tGameCoords.nX;
    }

    if (pUnit->pDynamicPath)
        return D2COMMON_PATH_GetXPosition(pUnit->pDynamicPath);

    return 0;
}

//1.10f: D2Client.0x6FAB6A90
int32_t __fastcall UNITS_GetYPosition(D2UnitStrc *pUnit)
{
    if (pUnit->dwUnitType == UNIT_OBJECT || ((int32_t)pUnit->dwUnitType > (int32_t)UNIT_MISSILE && (int32_t)pUnit->dwUnitType <= (int32_t)UNIT_TILE))
    {
        return pUnit->pStaticPath->tGameCoords.nY;
    }

    if (pUnit->pDynamicPath)
        return D2COMMON_PATH_GetYPosition(pUnit->pDynamicPath);

    return 0;
}


//1.10f: D2Client.0x6FACB670
//  SendMessageA(pMsg->hWnd, WM_CLOSE, 0, 0), then bReturn and lResult zeroed, in that order (0x6FACB684, 0x6FACB68B), so the dispatcher that
//  ran it would not take lResult as the message's answer. A Storm message handler (retn 4).
//  An orphan, and nothing reaches it: it lies between the skills area's last function and AUTOMAP_OnArrowDown, and its one reference is the
//  third dword of a 12-byte D2WinCommandRegisterStrc at 0x6FB77A2C -- { 0, WM_LBUTTONDOWN, this function } -- that no registrar is handed: a
//  close-on-click nobody wires up in the 1.10 client.
void __stdcall UI_SendWmCloseAndReset(SMSGHANDLER_PARAMS *pMsg)
{
    SendMessageA(pMsg->hWnd, WM_CLOSE, 0, 0);
    pMsg->bReturn = FALSE;
    pMsg->lResult = 0;
}


//1.10f: D2Client.0x6FADCF10
//  Replaces `%<nIndex>` in wszText by wszValue, in place, through a 400-character scratch buffer (sub esp, 324h: the digit's dword and 800
//  bytes). An orphan; its one caller is LANG_FormatString. __fastcall over retn 4: wszText in ecx, nIndex in edx, wszValue on the stack.
//  The scan covers the first strlen - 1 characters (signed jle 0x6FADCF4B and jl 0x6FADCF78), copying each into the scratch buffer until a '%'
//  is followed by the digit nIndex + '0' -- the next character zero-extended and compared as an int (0x6FADCF62..0x6FADCF68), so nIndex past 9
//  looks for ':' and beyond. No placeholder: it returns with wszText untouched. Found: the value is appended, then the text after the
//  placeholder -- one character later when the scratch buffer now ends in a space and the text after the placeholder begins with one
//  (0x6FADCFA4, 0x6FADCFB0), so a value that carries its own trailing space does not double the gap -- and the result is copied back.
//  BUG: the bytes test the word before the buffer's terminator ([esp+eax*2+12h], 0x6FADCFA4) with no test for a length of 0, and then read the
//  word below the buffer, which in the original's frame is the top half of the digit's dword, (nIndex + '0') >> 16, 0 for every nIndex from
//  -48 to 65487, so the test fails. REACHABLE, and the same answer: a magic item with no prefix hands INV_BuildItemNameText's emptied
//  wszPrefixPart as %0, which opens MagicFormat in the eng, CHI, JPN and KOR packs ("%0 %1 %2"), and nIndex 0's top half is 0.
//  DIFFERENCE: reading below an array is not C; this C tests nLen > 0 first, which fails the test the same way for every nIndex whose + '0'
//  does not have 0x20 in its top half.
//  BUG: the scratch buffer has no bound: the prefix, the value and the tail go into 400 characters through Unicode::strcat, and the result
//  back into the caller's buffer through Unicode::strcpy. LATENT: no stock format and its arguments come near 400.
void __fastcall LANG_SubstituteArg(Unicode *wszText, int32_t nIndex, const Unicode *wszValue)
{
    Unicode wszResult[400];
    const int32_t nDigit = nIndex + '0';
    const int32_t nLast = Unicode::strlen(wszText) - 1;
    int32_t nOffset = 0;
    if (nLast <= 0)
        return;

    for (;;)
    {
        const uint16_t ch = wszText[nOffset].ch;
        if (ch == '%' && wszText[nOffset + 1].ch == nDigit)
            break;
        wszResult[nOffset].ch = ch;
        if (++nOffset >= nLast)
            return;
    }

    Unicode::strcat(wszResult, wszValue);
    const int32_t nLen = Unicode::strlen(wszResult);
    const Unicode *pTail;
    if (nLen > 0 && wszResult[nLen - 1].ch == ' ' && wszText[nOffset + 2].ch == ' ')
        pTail = &wszText[nOffset + 3];
    else
        pTail = &wszText[nOffset + 2];
    Unicode::strcat(wszResult, pTail);
    Unicode::strcpy(wszText, wszResult);
}

//1.10f: D2Client.0x6FADCFE0
//  Formats an item's name: copies each argument into a slot of its own, applies the grammar directive the format may open with, then copies
//  the format's text out and substitutes %0, %1 ... from the slots. An orphan. __cdecl and variadic, the argument list ending in NULL: a bare
//  retn (0x6FADD2E1), and every call site cleans after itself (add esp, 10h, 14h or 18h).
//  D2Lang's calls, as their bytes do them: Unicode::strchr compares the sixteen-bit character (cmp dx, cx), so what rides above it in the
//  pushed dword is never read; Unicode::strlen answers 0 for NULL; Unicode::strstr answers NULL for a NULL either side; Unicode::strncpy
//  copies up to n characters and zero-fills the rest of n only when the source is shorter, with no terminator when it is not. Every Unicode
//  buffer declared below is one the bytes construct; the one-character needle and the "[ms]" tag are stored word by word and never
//  constructed, so they are uint16_t.
//  A NULL format returns before anything is constructed (0x6FADCFF7). Then ten slots of 200 characters are constructed (0x6FADCFFD..0x6FADD015)
//  and filled by Unicode::strcpy from the arguments, one slot each, until a NULL.
//  BUG: that copy has no bound: an eleventh argument goes past the tenth slot (in the original's frame, over the return address). LATENT: no
//  call site passes more than three arguments.
//  The directive. With a ':' in the format (Unicode::strchr), what precedes it is a directive and only what follows is text. The directive is
//  searched for 'a' and for 'n', each the first in the whole format and taken only when it lies before the ':' (jnb, 0x6FADD0B3 and
//  0x6FADD0D9); the character after each is the slot, ch - '0'. Then:
//      'n' alone   slot n loses its first four characters, unconditionally (Unicode::strcpy onto itself four on, 0x6FADD291)
//      'a' alone   in slot a, the text after the first "[ms]" up to the next '[' replaces the slot; no "[ms]", no change
//      both        the tag is slot n's first four characters when slot n opens with '[' -- cut from it -- and "[ms]" otherwise; then, only
//                  if slot a opens with '[', the text after that tag up to the next '[' replaces slot a
//  -- slot a a table of forms, "[ms]sword[fs]sable", slot n the word it must agree with, carrying its own tag. The run kept is strlen(after the
//  tag) - strlen(from the next '['), the second a strlen of NULL, 0, when there is none; it goes through Unicode::strncpy into a
//  100-character buffer (constructed on those two paths only, 0x6FADD178 and 0x6FADD1FC) and then back into the slot.
//  BUG: the slot digit is unchecked: a non-digit makes a slot outside the ten, and '/' makes -1, which reads as "absent"; and the run copied
//  into the 100-character buffer has no bound. LATENT: the only stock formats with a ':' have slot digits 0 and 1, never an 'n' without an
//  'a', and short runs.
//  The text. Unicode::strcpy(wszOut, after the ':' or the whole format), and LANG_SubstituteArg once per slot filled, %0 first.
//  BUG: wszOut is the caller's buffer and is unbounded here too. LATENT: no stock name nears the callers' buffers (INV_BuildItemNameText's
//  smallest nMaxLen is 128).
//  The needle's buffer carries 'a', then 'n', then the string "[" -- one frame slot for three uses (0x6FADD077, 0x6FADD0BE,
//  0x6FADD16A/0x6FADD1EE) -- and the ':' is stored for its strchr in the slot that then keeps the answer (0x6FADD059, 0x6FADD06D). Each strchr
//  is handed the character by value as the dword it lies in, of which strchr reads the low word; here each is handed a copy of the one word.
void __cdecl LANG_FormatString(Unicode *wszOut, const Unicode *wszFormat, ...)
{
    if (!wszFormat)
        return;

    Unicode wszArgs[10][200];
    int32_t nArgCount = 0;
    va_list args;
    va_start(args, wszFormat);
    const Unicode *pArg = va_arg(args, const Unicode *);
    if (pArg)
    {
        Unicode *pSlot = wszArgs[0];
        int32_t nArg = 0;
        do
        {
            ++nArg;
            Unicode::strcpy(pSlot, pArg);
            pSlot += 200;
            pArg = va_arg(args, const Unicode *);
        } while (pArg);
        nArgCount = nArg;
    }
    va_end(args);

    const uint16_t wchColon = ':';
    const Unicode *pColon = Unicode::strchr(wszFormat, *(const Unicode *)&wchColon);
    const Unicode *pText;
    if (!pColon)
    {
        pText = wszFormat;
    }
    else
    {
        uint16_t wszNeedle[2];      //  stored, never constructed
        uint16_t wszMsTag[5];       //  stored, never constructed: "[ms]"

        wszNeedle[0] = 'a';
        wszMsTag[0] = '[';
        wszMsTag[1] = 'm';
        wszMsTag[2] = 's';
        wszMsTag[3] = ']';
        wszMsTag[4] = 0;
        int32_t nSlotA = -1;
        const Unicode *pA = Unicode::strchr(wszFormat, *(const Unicode *)&wszNeedle[0]);
        if (pA && pA < pColon)
            nSlotA = pA[1].ch - '0';

        wszNeedle[0] = 'n';
        int32_t nSlotN = -1;
        const Unicode *pN = Unicode::strchr(wszFormat, *(const Unicode *)&wszNeedle[0]);
        if (pN && pN < pColon)
            nSlotN = pN[1].ch - '0';

        if (nSlotA == -1)
        {
            if (nSlotN != -1)
                Unicode::strcpy(wszArgs[nSlotN], &wszArgs[nSlotN][4]);
        }
        else if (nSlotN == -1)
        {
            wszNeedle[0] = '[';
            wszNeedle[1] = 0;
            Unicode *pSlotA = wszArgs[nSlotA];
            Unicode wszKept[100];
            const Unicode *pMatch = Unicode::strstr(pSlotA, (const Unicode *)wszMsTag);
            if (pMatch)
            {
                const Unicode *pAfter = pMatch + 4;
                const int32_t nAfterLen = Unicode::strlen(pAfter);
                Unicode::strncpy(wszKept, pAfter, nAfterLen - Unicode::strlen(Unicode::strstr(pAfter, (const Unicode *)wszNeedle)));
                Unicode::strcpy(pSlotA, wszKept);
            }
        }
        else
        {
            Unicode wszTag[5];
            if (wszArgs[nSlotN][0].ch == '[')
            {
                Unicode::strncpy(wszTag, wszArgs[nSlotN], 4);
                Unicode::strcpy(wszArgs[nSlotN], &wszArgs[nSlotN][4]);
            }
            else
            {
                Unicode::strcpy(wszTag, (const Unicode *)wszMsTag);
            }

            Unicode *pSlotA = wszArgs[nSlotA];
            if (pSlotA->ch == '[')
            {
                wszNeedle[0] = '[';
                wszNeedle[1] = 0;
                Unicode wszKept[100];
                const Unicode *pMatch = Unicode::strstr(pSlotA, wszTag);
                if (pMatch)
                {
                    const Unicode *pAfter = pMatch + 4;
                    const int32_t nAfterLen = Unicode::strlen(pAfter);
                    Unicode::strncpy(wszKept, pAfter, nAfterLen - Unicode::strlen(Unicode::strstr(pAfter, (const Unicode *)wszNeedle)));
                    Unicode::strcpy(pSlotA, wszKept);
                }
            }
        }
        pText = pColon + 1;
    }

    Unicode::strcpy(wszOut, pText);
    for (int32_t nArg = 0; nArg < nArgCount; ++nArg)
        LANG_SubstituteArg(wszOut, nArg, wszArgs[nArg]);
}


//1.10f: D2Client.0x6FAE5BD0
//  nValue * nMultiplier / nDivisor, whichever of four ways avoids overflowing 32 bits: ecx is nValue, edx is nMultiplier, the stack slot is
//  nDivisor. D2Common's header inline, emitted out of line; the original inlines it in more units.
//  The four branches are not redundant and must not be collapsed into one 64-bit multiply. Two of them divide before multiplying, which
//  truncates: with nValue=30, nMultiplier=0x20000, nDivisor=7 the shortcut gives 30 * (131072 / 7) = 30 * 18724 = 561720 against an exact
//  561737. The game's numbers depend on it.
//  BUG: the guard does not prove safety: nValue is bounded by 0x100000 (cmp ecx, 100000h, 0x6FAE5BE3) and nMultiplier by 0x10000
//  (cmp edi, 10000h, 0x6FAE5C01), and 0x100000 * 0x10000 is 2^36, so the "both small" fast path can still overflow a 32-bit multiply. It is a
//  heuristic about typical magnitudes.
int32_t __fastcall DATATBLS_ApplyRatio(int32_t nValue, int32_t nMultiplier, int32_t nDivisor)
{
    if (!nDivisor)
    {
        return 0;
    }

    if (nValue <= 0x100000)
    {
        if (nMultiplier <= 0x10000)
        {
            return nMultiplier * nValue / nDivisor;
        }
        if (nDivisor <= nMultiplier >> 4)
        {
            return nValue * (nMultiplier / nDivisor);
        }
    }
    else if (nDivisor <= nValue >> 4)
    {
        return nMultiplier * (nValue / nDivisor);
    }

    return (int32_t)((int64_t)nMultiplier * (int64_t)nValue / nDivisor);
}


//1.10f: D2Client.0x6FAE5C80
//  setitems.txt by row, bounds-checked: the same shape as INV_DataTblsGetSetsTxtRecord (0x6FAE5C50). D2Common's header inline, emitted out of
//  line; the original inlines it in several units. The static_assert below pins the two table fields it reads.
static_assert(offsetof(D2DataTablesStrc, pSetItemsTxt) == 0xC18 && offsetof(D2DataTablesStrc, nSetItemsTxtRecordCount) == 0xC1C,
              "DATATBLS_GetSetItemsTxtRecord: `mov edx, [edx+0C18h]` (0x6FAE5C93), `cmp ecx, [edx+0C1Ch]` (0x6FAE5C8B)");
D2SetItemsTxt *__fastcall DATATBLS_GetSetItemsTxtRecord(int32_t nFileIndex)
{
    if (nFileIndex < 0 || nFileIndex >= sgptDataTables->nSetItemsTxtRecordCount)
    {
        return NULL;
    }
    return &sgptDataTables->pSetItemsTxt[nFileIndex];
}


//1.10f: D2Client.0x6FAFBB10
//  Clears a player's queued command: the four dwords of the player's data at +0x150..+0x15C (unk0xA8[42..45]) are zeroed; for a NULL unit,
//  any other type or a player with no data, nothing. They are the queued command: PLAYER_SetQueuedCommand writes 1, the command and two
//  parameters into the same four, and PLAYER_GetQueuedCommand and PLAYER_GetQueuedTargetType answer [43] and [44].
//  The original re-loads pPlayerData before every store (four mov eax, [ecx+14h]) rather than keeping it in a register; written that way,
//  since the source said pUnit->pPlayerData->x = 0 four times. The original inlines this function in several units.
void __fastcall PLAYER_ClearQueuedCommand(D2UnitStrc *pPlayer)
{
    if (pPlayer && pPlayer->dwUnitType == UNIT_PLAYER && pPlayer->pPlayerData)
    {
        pPlayer->pPlayerData->unk0xA8[42] = 0;
        pPlayer->pPlayerData->unk0xA8[43] = 0;
        pPlayer->pPlayerData->unk0xA8[44] = 0;
        pPlayer->pPlayerData->unk0xA8[45] = 0;
    }
}


//1.10f: D2Client.0x6FB0A440
//  A skill's skilldesc.txt string as a tbl index: the skill's row, its skilldesc (wSkillDesc, +194h, a signed WORD -- movsx, 0x6FB0A469), that
//  row of skilldesc.txt, and the WORD nStringField places past wStrName (mov ax, [eax+edx*2+8], 0x6FB0A494): 0 the name, 1 the short name,
//  2 the long. A failed bound at any step answers STR_IDX_5382_dummy, "an evil force" (mov ax, 1506h, 0x6FB0A48E).
//  An orphan: it lies between the ground item labels (showitems.cpp) and SkillDesc.cpp. The original inlines
//  DATATBLS_GetSkillsTxtRecordById here; the skilldesc lookup is written in place.
int16_t __fastcall SKILLS_GetSkillDescStringIndex(D2C_Skills nSkillId, int32_t nStringField)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nSkillDesc;
    D2SkillDescTxt *pSkillDescTxt;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return STR_IDX_5382_dummy;

    nSkillDesc = (int16_t)pSkillsTxt->wSkillDesc;
    if (nSkillDesc < 0 || nSkillDesc >= sgptDataTables->nSkillDescTxtRecordCount)
        return STR_IDX_5382_dummy;

    pSkillDescTxt = &sgptDataTables->pSkillDescTxt[nSkillDesc];
    if (!pSkillDescTxt)
        return STR_IDX_5382_dummy;

    return (&pSkillDescTxt->wStrName)[nStringField];
}


//1.10f: D2Client.0x6FB14AE0
//  Bounds-checked accessor: the record, or NULL when the id is negative or past nSkillsTxtRecordCount. The client's own copy; D2Common exports a
//  similar one, and this is not a thunk to it. The original inlines it in many units.
D2SkillsTxt *__fastcall DATATBLS_GetSkillsTxtRecordById(int32_t nSkillId)
{
    if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount)
    {
        return NULL;
    }
    else
    {
        return &sgptDataTables->pSkillsTxt[nSkillId];
    }
}


//1.10f: D2Client.0x6FB247F0
//  A monstats.txt id to its monstats2 row. The indirection is the thing to remember: monstats2 is not indexed by class id. The walk is
//  monstats row, then its wMonStatsEx field, then the monstats2 row, and both hops are bounds-checked. The original inlines this function in
//  many units; MONSTER_SpawnBlood does the same walk inline rather than calling it.
D2MonStats2Txt *__fastcall DATATBLS_GetMonStats2TxtRecord(int32_t nMonStatsId)
{
    int32_t nMonStatsEx;

    if (nMonStatsId < 0)
        return NULL;
    if (nMonStatsId >= sgptDataTables->nMonStatsTxtRecordCount)
        return NULL;

    nMonStatsEx = (int16_t)sgptDataTables->pMonStatsTxt[nMonStatsId].wMonStatsEx;
    if (nMonStatsEx < 0 || nMonStatsEx >= sgptDataTables->nMonStats2TxtRecordCount)
        return NULL;

    return &sgptDataTables->pMonStats2Txt[nMonStatsEx];
}


//1.10f: D2Client.0x6FB2A6E0
//  A unit's party id, -1 for anything but a player: pUnit in ecx, a bare retn. NULL, or a unit whose dwUnitType is not UNIT_PLAYER
//  (cmp dword ptr [ecx], 0, 0x6FB2A6E4), answers -1 by or ax, 0FFFFh (0x6FB2A6EE) -- the low word only, as an int16_t answer needs; a player
//  goes on to ROSTER_FindPartyIdByUnit by a tail jmp (0x6FB2A6E9), which answers the entry's party id or -1.
//  An orphan: it lies between CUnit.cpp's last function and CUnitEvent.cpp's first.
int16_t __fastcall ROSTER_GetPartyIdForUnit(D2UnitStrc *pUnit)
{
    if (!pUnit || pUnit->dwUnitType != UNIT_PLAYER)
        return -1;
    return ROSTER_FindPartyIdByUnit(pUnit);
}


//  The table fields and constants the next two functions read at measured positions, pinned nowhere else.
static_assert(offsetof(D2MonStats2Txt, nSizeX) == 0x08 && COLLIDE_MASK_PLACEMENT == 0x3F11,
              "UNITS_FindFreeSpotForMonster: `movsx eax, byte ptr [edx+8]` (0x6FB3EF54), `push 3F11h` "
              "(0x6FB3EF4F); ENV_SpawnRoomCritters: `movsx edx, byte ptr [eax+8]` (0x6FB3F171), `push 3F11h` "
              "(0x6FB3F1F6)");
static_assert(offsetof(D2ActiveRoomStrc, pSeed) == 0x3C
                  && offsetof(D2LevelsTxt, wCMon) == 0xCC && offsetof(D2LevelsTxt, wCPct) == 0xD4
                  && offsetof(D2LevelsTxt, wCAmt) == 0xDC
                  && offsetof(D2MonStatsTxt, nMinGrp) == 0x2F && offsetof(D2MonStatsTxt, nMaxGrp) == 0x30,
              "ENV_SpawnRoomCritters: `add esi, 3Ch` (0x6FB3EFD5), `lea ebp, [eax+0CCh]` (0x6FB3EFDC), "
              "`movsx eax, word ptr [ebp+8]` (0x6FB3F01F), `mov cx, [ebp+10h]` (0x6FB3F0BD), `mov dl, "
              "[eax+2Fh]` (0x6FB3F06F), `mov bl, [eax+30h]` (0x6FB3F06C)");


//1.10f: D2Client.0x6FB3EDF0
//  Creates up to nCount client-only monsters of one class near a unit. __fastcall over retn 4, nCount the one stack argument. Each of up to
//  ten tries rolls a subtile within two of the unit's on both axes (% 5, then - 2, the unit's own seed), takes the room that holds it, and
//  where monstats2's SizeX fits under COLLIDE_MASK_PLACEMENT (0x3F11) makes the monster with CUNIT_CreateClientOnlyUnit and counts it off; the
//  walk ends when nCount reaches 0 (jle, signed, 0x6FB3EF80) or after the tenth try (jl, 0x6FB3EF8E).
//  An orphan, with ENV_SpawnRoomCritters: both lie between Missile.cpp's last function and MONSTER_ForEachUnitNearby.
//  The original inlines DATATBLS_GetMonStats2TxtRecord, SEED_RollRandomNumber (twice) and CLIENT_FindRoomContainingSubtile here.
//  The room is carried from try to try: pRoom starts as the unit's room, and each try replaces it with the room that holds the try's point --
//  the same one, an adjacent one, or NULL when no room in reach holds it. After a NULL every later try starts from NULL, so
//  CLIENT_FindRoomContainingSubtile answers NULL at once, and COLLISION_CheckMaskWithSize answers a NULL room non-zero in every arm of its
//  size switch, which blocks: nothing more is placed. A unit with no room places nothing at all.
//  BUG: once a try's room is NULL, every later try is blocked and nothing more is placed. NOT ESTABLISHED: a NULL needs a try's point in no
//  active room near the one it starts from, and whether a stock caller's unit stands within two subtiles of a level's edge or of an inactive
//  room was not looked for. The cost is cosmetic: fewer critters.
//  The rolls are unsigned div of the low half (0x6FB3EE9F, 0x6FB3EEC5), so the C casts SEED_RollRandomNumber's 64-bit answer before its %.
//  SizeX is read sign-extended from an unsigned byte field (movsx eax, byte ptr [edx+8], 0x6FB3EF54), and the C casts it.
void __fastcall UNITS_FindFreeSpotForMonster(D2UnitStrc *pUnit, int32_t nMonStatsId, int32_t nCount)
{
    D2ActiveRoomStrc *pRoom;
    D2MonStats2Txt *pMonStats2Txt;
    D2CoordStrc tCoord;
    int32_t nTries;
    int32_t nX;
    int32_t nY;

    pRoom = D2COMMON_UNITS_GetRoom(pUnit);

    pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(nMonStatsId);
    if (!pMonStats2Txt)
        return;

    D2COMMON_UNITS_GetCoords(pUnit, &tCoord);
    nTries = 0;
    do
    {
        nX = (int32_t)((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 5) + tCoord.nX - 2;
        nY = (int32_t)((uint32_t)SEED_RollRandomNumber(&pUnit->pSeed) % 5) + tCoord.nY - 2;

        pRoom = CLIENT_FindRoomContainingSubtile(pRoom, nX, nY);

        if (!D2COMMON_COLLISION_CheckMaskWithSize(pRoom, nX, nY, (int8_t)pMonStats2Txt->nSizeX, COLLIDE_MASK_PLACEMENT))
        {
            CUNIT_CreateClientOnlyUnit(nMonStatsId, nX, nY, UNIT_MONSTER, 0);
            --nCount;
        }
    } while (nCount > 0 && ++nTries < 10);
}


//1.10f: D2Client.0x6FB3EFB0
//  Spawns a room's critters from levels.txt alone: pRoom in ecx, a bare retn. The room's level record (DUNGEON_GetLevelIdFromRoom, 0
//  returning at once, then DATATBLS_GetLevelsTxtRecord) and its four critter columns: CMon1..4, the monstats.txt class (wCMon at +0xCC),
//  CPct1..4, the chance in hundredths (wCPct, +0xD4), CAmt1..4, a multiplier (wCAmt, +0xDC). In order, until a negative CMon: one roll of
//  the room's seed, % 100, under CPct; then a group of nMinGrp plus a roll below nMaxGrp - nMinGrp (the class's monstats.txt row), times CAmt
//  when CAmt is not 0; and for each of the group, up to ten tries at a random subtile of the room (a roll below its width - 1 and its
//  height - 1, from its corner) until monstats2's SizeX fits under COLLIDE_MASK_PLACEMENT, and the critter made there with
//  CUNIT_CreateClientOnlyUnit. A critter that finds no place in ten tries is not made. Everything comes from the room's seed, so every client
//  fills a room alike and the server says nothing.
//  An orphan, like UNITS_FindFreeSpotForMonster above. The original inlines SEED_RollRandomNumber, DATATBLS_GetMonStatsTxtRecord,
//  SEED_RollLimitedRandomNumber and DATATBLS_GetMonStats2TxtRecord here; the two rolls of a try are D2Common's seed inline, which calls
//  SEED_RollRandomNumber in the power-of-two arm (the x roll in the other arm too), written as SEED_RollLimitedRandomNumberInline.
//  The three columns are read signed: the bytes test CMon with jl (cmp word ptr [ebp+0], 0, 0x6FB3EFEE) and widen all three with movsx
//  (0x6FB3F01F, 0x6FB3F0C8, 0x6FB3F0DA); the C casts each to int16_t. The chance compare is unsigned (jnb, 0x6FB3F025): the roll's remainder
//  against CPct sign-extended, so a negative CPct would always pass; none is negative in the stock levels.txt.
//  BUG: only wCAmt[0] is ever written: the levels.txt field table gives camt1..4 one offset, 0xDC, so wCAmt[1..3] stay 0 and a CAmt can
//  multiply the first column's group alone. INERT: camt1..4 are empty in every stock row, so the multiplier never applies; the C reads
//  wCAmt[i] as the client does.
//  BUG: the class's monstats.txt row is read with no NULL test (mov bl, [eax+30h], 0x6FB3F06C, after the copy's NULL arm at 0x6FB3F05C).
//  LATENT: the stock levels.txt's CMon cells name ten classes, every one a monstats.txt Id.
//  The redundant test after the tries (cmp eax, 0Ah / jge, 0x6FB3F224, on a path where the count is below 10) is the source's `if` after its
//  loop, which the C writes.
void __fastcall ENV_SpawnRoomCritters(D2ActiveRoomStrc *pRoom)
{
    int32_t nLevelId;
    D2LevelsTxt *pLevelsTxt;
    D2SeedStrc *pSeed;
    D2MonStatsTxt *pMonStatsTxt;
    D2MonStats2Txt *pMonStats2Txt;
    D2DrlgCoordsStrc tCoords;
    int32_t i;
    int32_t nGroup;
    int32_t nClassId;
    int32_t nSpanX;
    int32_t nSpanY;
    int32_t nUnitSize;
    int32_t nAttempt;
    int32_t nX;
    int32_t nY;

    nLevelId = D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);
    if (!nLevelId)
        return;

    //  BUG: the level record is used with no NULL test (lea ebp, [eax+0CCh], 0x6FB3EFDC). UNREACHABLE: a room's level id is its level's.
    pLevelsTxt = D2COMMON_DATATBLS_GetLevelsTxtRecord(nLevelId);
    //  The room's NULL test for the seed comes after DUNGEON_GetLevelIdFromRoom has answered 0 for a NULL room, so its NULL arm is dead.
    //  UNREACHABLE.
    pSeed = pRoom ? &pRoom->pSeed : NULL;

    for (i = 0; i < 4; ++i)
    {
        if ((int16_t)pLevelsTxt->wCMon[i] < 0)
            break;

        if ((uint32_t)SEED_RollRandomNumber(pSeed) % 100 < (uint32_t)(int16_t)pLevelsTxt->wCPct[i])
        {
            pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord((int16_t)pLevelsTxt->wCMon[i]);

            nGroup = SEED_RollLimitedRandomNumber(pSeed, (int32_t)pMonStatsTxt->nMaxGrp - (int32_t)pMonStatsTxt->nMinGrp) + pMonStatsTxt->nMinGrp;
            if (pLevelsTxt->wCAmt[i])
                nGroup *= (int16_t)pLevelsTxt->wCAmt[i];

            if (nGroup > 0)
            {
                do
                {
                    nClassId = (int16_t)pLevelsTxt->wCMon[i];
                    D2COMMON_DUNGEON_GetRoomCoordinates(pRoom, &tCoords);
                    nSpanX = tCoords.nSubtileWidth - 1;
                    nSpanY = tCoords.nSubtileHeight - 1;

                    pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(nClassId);
                    if (pMonStats2Txt)
                    {
                        nUnitSize = (int8_t)pMonStats2Txt->nSizeX;
                        for (nAttempt = 0; nAttempt < 10; ++nAttempt)
                        {
                            nX = (int32_t)SEED_RollLimitedRandomNumberInline(pSeed, nSpanX) + tCoords.nSubtileX;
                            nY = (int32_t)SEED_RollLimitedRandomNumberInline(pSeed, nSpanY) + tCoords.nSubtileY;
                            if (!D2COMMON_COLLISION_CheckMaskWithSize(pRoom, nX, nY, nUnitSize, COLLIDE_MASK_PLACEMENT))
                                break;
                        }
                        if (nAttempt < 10)
                            CUNIT_CreateClientOnlyUnit(nClassId, nX, nY, UNIT_MONSTER, 0);
                    }
                } while (--nGroup);
            }
        }
    }
}


//1.10f: D2Client.0x6FB4B720
//  Queues a command on a player: nCmd in ecx, pPlayer in edx, the target's type and GUID on the stack (retn 8). For a player with player data:
//  1, the command, the type and the GUID into the four dwords at +150h..+15Ch (unk0xA8[42..45]), pPlayerData read again before every store as
//  in PLAYER_ClearQueuedCommand; anything else, nothing. +158h is the target's type and +15Ch its GUID, what PLAYER_GetQueuedTargetType and
//  PLAYER_GetQueuedTargetGuid read back. UNITMODE_RunQueuedCommand reads the group back on a later frame, and
//  UNITMODE_PerformClientCommand's last act clears it. The original inlines this function in more than one unit.
void __fastcall PLAYER_SetQueuedCommand(int32_t nCmd, D2UnitStrc *pPlayer, int32_t nTargetType, D2UnitGUID nTargetGUID)
{
    if (pPlayer && pPlayer->dwUnitType == UNIT_PLAYER && pPlayer->pPlayerData)
    {
        pPlayer->pPlayerData->unk0xA8[42] = 1;
        pPlayer->pPlayerData->unk0xA8[43] = (uint32_t)nCmd;
        pPlayer->pPlayerData->unk0xA8[44] = (uint32_t)nTargetType;
        pPlayer->pPlayerData->unk0xA8[45] = nTargetGUID;
    }
}


//1.10f: D2Client.0x6FB4B760
//  Returns the queued command: unk0xA8[43] of a player's data (+154h), read through one load of pPlayerData; 0 for NULL, any other type or no
//  data. The pending flag at +150h is not read. A bare retn, pPlayer in ecx. The original inlines it in more than one unit.
int32_t __fastcall PLAYER_GetQueuedCommand(D2UnitStrc *pPlayer)
{
    D2PlayerDataStrc *pPlayerData;

    if (pPlayer && pPlayer->dwUnitType == UNIT_PLAYER && (pPlayerData = pPlayer->pPlayerData) != NULL)
        return (int32_t)pPlayerData->unk0xA8[43];
    return 0;
}


//1.10f: D2Client.0x6FB522F0
//  A monster's base monster: monstats.txt's BaseId of the unit's class, read signed (movsx eax, word ptr [ecx+2], 0x6FB52327); the class id
//  itself when its row cannot be read -- a negative class or one at or past the count; -1 for a NULL unit or one that is not UNIT_MONSTER
//  (or eax, 0FFFFFFFFh, 0x6FB5232C). A bare retn, the unit in ecx. The original inlines DATATBLS_GetMonStatsTxtRecord here, and inlines this
//  function in more than one unit.
int32_t __fastcall MONSTER_GetBaseMonsterId(D2UnitStrc *pUnit)
{
    int32_t nClassId;
    D2MonStatsTxt *pMonStatsTxt;

    if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER)
        return -1;

    nClassId = pUnit->dwClassId;
    pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(nClassId);
    if (pMonStatsTxt)
        return pMonStatsTxt->nBaseId;
    return nClassId;
}


//  The Engine.h path GFXUTIL_GetEffectiveClassId hands #11013 (0x6FB96F24).
static const char szEngineH[] = "..\\D2Client/Engine.h";


//1.10f: D2Client.0x6FB5DD70
//  The class a unit is drawn as. A NULL unit is -1 (or eax, 0FFFFFFFFh, 0x6FB5DD94); a unit without dwFlagEx bit 3 -- UNITFLAGEX_ISSHAPESHIFTED
//  -- is its own dwClassId; a shapeshifted one is the class D2Common's #11013 ConvertMode puts in place of it, fed the unit's type, class and
//  anim or sequence mode. The assert path is Engine.h's, line 278 (push 116h, 0x6FB5DDB7): an inline of that header, emitted here out of
//  line. The bit is shifted down and masked, and the result stored into the slot that then takes the type (0x6FB5DD80..0x6FB5DD86); the store
//  is the compiler's reuse of a dead slot, and the test is what the C keeps. The original inlines this function in more than one unit.
int32_t __fastcall GFXUTIL_GetEffectiveClassId(D2UnitStrc *pUnit)
{
    int32_t nType;
    int32_t nClass;
    int32_t nMode;

    if (!pUnit)
        return -1;

    if (!((pUnit->dwFlagEx >> 3) & 1))
        return (int32_t)pUnit->dwClassId;

    nType = (int32_t)pUnit->dwUnitType;
    nClass = (int32_t)pUnit->dwClassId;
    nMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nMode, szEngineH, 278);
    return nClass;
}


//1.10f: D2Client.0x6FB5DDE0
//  The inventory a unit is drawn wearing. A shapeshifted unit (dwFlagEx bit 3) under a state whose mask 37 is set -- STATEMASK_BOSSINV,
//  states.txt's bossinv column -- is drawn in its owner's equipment, CUNIT_GetOwnerUnit's pInventory, and *pbFromOwner says TRUE; anything
//  else is drawn in its own, FALSE. Both stores are skipped for a NULL pbFromOwner. The original inlines this function in more than one unit.
//  BUG: the NULL test does not cover the last line. A NULL pUnit takes the jz at 0x6FB5DDE9 to the own-inventory arm, which reads [edi+60h]
//  with edi zero (0x6FB5DE2F): a fault at address 0x60. UNREACHABLE: the one call sits behind GFXUTIL_IsDrawableUnitType (0x6FB5A84F,
//  0x6FB5A856), FALSE for a NULL unit. Reproduced.
D2InventoryStrc *__fastcall GFXUTIL_GetUnitDisplayInventory(D2UnitStrc *pUnit, BOOL *pbFromOwner)
{
    D2UnitStrc *pOwner;

    if (pUnit && ((pUnit->dwFlagEx >> 3) & 1) && D2COMMON_STATES_CheckStateMaskOnUnit(pUnit, STATEMASK_BOSSINV))
    {
        pOwner = CUNIT_GetOwnerUnit(pUnit);
        if (pOwner)
        {
            if (pbFromOwner)
                *pbFromOwner = TRUE;

            return pOwner->pInventory;
        }
    }

    if (pbFromOwner)
        *pbFromOwner = FALSE;

    return pUnit->pInventory;
}


//1.10f: D2Client.0x6FB5FAB0
//  An overlay.txt record by id, or NULL below 0 or at the count (jl and jge, signed); 132-byte records (shl edx, 5 / add edx, ecx, then * 4).
//  The original inlines this function in more than one unit.
D2OverlayTxt *__fastcall GFXUTIL_GetOverlayTxtRecord(int32_t nOverlayId)
{
    if (nOverlayId < 0 || nOverlayId >= sgptDataTables->nOverlayTxtRecordCount)
        return NULL;

    return &sgptDataTables->pOverlayTxt[nOverlayId];
}


//1.10f: D2Client.0x6FB6BD9A
//  VirtualAlloc(NULL, dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE), the whole function (0x6FB6BD9A..0x6FB6BDA4). The pages are
//  executable, for the gouraud tables of VIEW_AllocGouraudTables. An orphan, with its twin below: the two lie after Fault.cpp's last function
//  and before the second run of import thunks.
LPVOID __fastcall CLIENT_VirtualAlloc(SIZE_T dwSize)
{
    return VirtualAlloc(NULL, dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
}

//1.10f: D2Client.0x6FB6BDAB
//  VirtualFree(lpAddress, 0, MEM_RELEASE), the whole function: it frees what CLIENT_VirtualAlloc gave (VIEW_FreeGouraudTables).
void __fastcall CLIENT_VirtualFree(LPVOID lpAddress)
{
    VirtualFree(lpAddress, 0, MEM_RELEASE);
}

#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"

//  PlayerList.cpp -- the per-player lists of the roster in D2Client.dll 1.10: what each roster player holds about the others -- loot, ignore,
//  squelch, hostile, the four D2C_RosterInfoFlags bits -- one twelve-byte D2RosterInfoStrc per other player.
//  The original source file is UNIT\PlayerList.cpp (the asserts of three of the functions name it). Address range 0x6FB4B7C0..0x6FB4B97B. The
//  roster entries hold these lists, but the assert path is this file's.
//  The four functions that look an entry up each walk the list for the target in place; no out-of-line body of the walk exists in the image.
//  The lookup of the player's list comes before the comparison of the two GUIDs.
//
//  The path literal the three allocating functions pass (0x6FB93EA0).
static const char szPlayerListCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UNIT\\PlayerList.cpp";

//  The three members every walk reads.
static_assert(offsetof(D2RosterInfoStrc, dwUnitId) == 0 && offsetof(D2RosterInfoStrc, dwPartyFlags) == 4 && offsetof(D2RosterInfoStrc, pNext) == 8,
              "PLAYERLIST_RosterSetInfoFlag: `cmp [eax], edi` 0x6FB4B8B9, `mov edx, [eax+4]` 0x6FB4B8D5, `mov eax, [eax+8]` 0x6FB4B8BD");


//1.10f: D2Client.0x6FB4B7C0
//  A list head for one roster player: FOG_Alloc of four bytes (line 37), `ptInfo` asserted at 39 with the exit after it, the head cleared to an
//  empty list. The head goes back in eax.
D2RosterInfoStrc **__cdecl PLAYERLIST_RosterAllocPlayerInfoList(void)
{
    D2RosterInfoStrc **ppInfoHead;

    ppInfoHead = (D2RosterInfoStrc **)FOG_Alloc(4, szPlayerListCpp, 37, 0);
    if (!ppInfoHead)
    {
        FOG_DisplayAssert("ptInfo", szPlayerListCpp, 39);
        exit(-1);
    }

    *ppInfoHead = NULL;
    return ppInfoHead;
}

//1.10f: D2Client.0x6FB4B800
//  Frees a list and its head: each entry with its next pointer read first (line 60), then the head (line 64). A NULL head frees nothing. void:
//  none of the three callers reads eax.
void __fastcall PLAYERLIST_RosterFreePlayerInfoList(D2RosterInfoStrc **ppInfoHead)
{
    D2RosterInfoStrc *pInfoEntry;
    D2RosterInfoStrc *pNext;

    if (!ppInfoHead)
        return;

    for (pInfoEntry = *ppInfoHead; pInfoEntry; pInfoEntry = pNext)
    {
        pNext = pInfoEntry->pNext;
        FOG_Free(pInfoEntry, szPlayerListCpp, 60, 0);
    }

    FOG_Free(ppInfoHead, szPlayerListCpp, 64, 0);
}

//1.10f: D2Client.0x6FB4B840
//  The entry dwPlayerGUID holds about dwTargetGUID, made if missing: the player's list is looked up, and when there is one and the two GUIDs
//  differ, looked up again (a second ROSTER_GetInfoListByUnitOrCorpse call with the same ecx) and walked for the target; a hit returns. A miss, or
//  a NULL from the second lookup, allocates twelve bytes (line 116), clears all three dwords, writes the target's GUID and links the entry at the
//  head of the first lookup's list.
//  void: eax at the common exit is whatever the last step left there (the walk's node, the new entry, NULL with no list, or the list's head slot
//  when the two GUIDs are the same), and no caller reads it.
//  BUG: the allocation is not tested: a FOG_Alloc that failed is written through at 0x6FB4B889. LATENT.
void __fastcall PLAYERLIST_RosterGetOrAddInfoEntry(uint32_t dwPlayerGUID, uint32_t dwTargetGUID)
{
    D2RosterInfoStrc **ppInfoHead;
    D2RosterInfoStrc **ppWalkHead;
    D2RosterInfoStrc *pInfoEntry;

    ppInfoHead = ROSTER_GetInfoListByUnitOrCorpse(dwPlayerGUID);
    if (!ppInfoHead || dwPlayerGUID == dwTargetGUID)
        return;

    ppWalkHead = ROSTER_GetInfoListByUnitOrCorpse(dwPlayerGUID);
    if (ppWalkHead)
    {
        for (pInfoEntry = *ppWalkHead; pInfoEntry; pInfoEntry = pInfoEntry->pNext)
        {
            if (pInfoEntry->dwUnitId == dwTargetGUID)
                return;
        }
    }

    pInfoEntry = (D2RosterInfoStrc *)FOG_Alloc(sizeof(D2RosterInfoStrc), szPlayerListCpp, 116, 0);
    //  BUG: FOG_Alloc's answer is not tested before the stores. LATENT.
    pInfoEntry->dwUnitId = 0;
    pInfoEntry->dwPartyFlags = 0;
    pInfoEntry->pNext = NULL;
    pInfoEntry->dwUnitId = dwTargetGUID;
    pInfoEntry->pNext = *ppInfoHead;
    *ppInfoHead = pInfoEntry;
}

//1.10f: D2Client.0x6FB4B8A0
//  Sets (bSet) or clears nFlag in the entry dwPlayerGUID holds about dwTargetGUID; does nothing when there is no list, no entry, or the GUIDs are
//  the same. It makes no entry, which is why its callers make one first with PLAYERLIST_RosterGetOrAddInfoEntry. nFlag and bSet are the two stack
//  arguments (retn 8).
void __fastcall PLAYERLIST_RosterSetInfoFlag(uint32_t dwPlayerGUID, uint32_t dwTargetGUID, int32_t nFlag, int32_t bSet)
{
    D2RosterInfoStrc **ppInfoHead;
    D2RosterInfoStrc *pInfoEntry;

    ppInfoHead = ROSTER_GetInfoListByUnitOrCorpse(dwPlayerGUID);
    if (!ppInfoHead || dwPlayerGUID == dwTargetGUID)
        return;

    for (pInfoEntry = *ppInfoHead; pInfoEntry; pInfoEntry = pInfoEntry->pNext)
    {
        if (pInfoEntry->dwUnitId == dwTargetGUID)
        {
            if (bSet)
                pInfoEntry->dwPartyFlags |= nFlag;
            else
                pInfoEntry->dwPartyFlags &= ~nFlag;
            return;
        }
    }
}

//1.10f: D2Client.0x6FB4B900
//  The entry's flags masked by nFlag (the masked word, not a BOOL), or 0 with no list, no entry or the same GUID twice. nFlag is the one stack
//  argument (retn 4).
int32_t __fastcall PLAYERLIST_RosterTestInfoFlag(uint32_t dwPlayerGUID, uint32_t dwTargetGUID, D2C_RosterInfoFlags nFlag)
{
    D2RosterInfoStrc **ppInfoHead;
    D2RosterInfoStrc *pInfoEntry;

    ppInfoHead = ROSTER_GetInfoListByUnitOrCorpse(dwPlayerGUID);
    if (!ppInfoHead || dwPlayerGUID == dwTargetGUID)
        return 0;

    for (pInfoEntry = *ppInfoHead; pInfoEntry; pInfoEntry = pInfoEntry->pNext)
    {
        if (pInfoEntry->dwUnitId == dwTargetGUID)
            return pInfoEntry->dwPartyFlags & nFlag;
    }

    return 0;
}

//1.10f: D2Client.0x6FB4B940
//  Replaces the low word of the entry's flags: a 16-bit store of 0 (0x6FB4B96D), then the 32-bit nFlags OR-ed in (0x6FB4B973), so bits 16..31 of
//  dwPartyFlags survive, and any of nFlags' own. The one caller masks what it passes to 16 bits, and the four flags are bits 0..3. Written as
//  the two stores the bytes make.
void __fastcall PLAYERLIST_RosterReplaceInfoFlags(uint32_t dwPlayerGUID, uint32_t dwTargetGUID, int32_t nFlags)
{
    D2RosterInfoStrc **ppInfoHead;
    D2RosterInfoStrc *pInfoEntry;

    ppInfoHead = ROSTER_GetInfoListByUnitOrCorpse(dwPlayerGUID);
    if (!ppInfoHead || dwPlayerGUID == dwTargetGUID)
        return;

    for (pInfoEntry = *ppInfoHead; pInfoEntry; pInfoEntry = pInfoEntry->pNext)
    {
        if (pInfoEntry->dwUnitId == dwTargetGUID)
        {
            *(uint16_t *)&pInfoEntry->dwPartyFlags = 0;
            pInfoEntry->dwPartyFlags |= nFlags;
            return;
        }
    }
}

#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Roster.h"

//  Roster.cpp -- the client's copy of the party roster of D2Client.dll 1.10: one D2RosterUnitStrc per player the server has announced (packet
//  0x5B adds, 0x5C removes, 0x75 and 0x8D update), kept on gpPartyEntries in the order the party panel draws it.
//  The original source file is GAME\Roster.cpp (the asserts in this unit name it). Address range 0x6FAAEC80..0x6FAB14BD; the last ten functions
//  (0x6FAB1200 on) carry no assert and are placed here by the globals they share.
//  Enums used here: D2C_RosterInfoFlags (LOOT 1, IGNORE 2, SQUELCH 4, HOSTILE 8), D2C_PartyButtons (the party-panel button at D2RosterUnitStrc
//  +0x30: INVITE 0, NONE 1, ACCEPT 2, LEAVE 3, CANCEL 4; the button is 1 after an accept and after a leave) and D2C_PartyCommands (the command
//  byte of client packets 0x5D and 0x5E).
//  Every function that changes an entry's party re-sorts the whole roster the same way (the rebuild); in the binary none of them calls a
//  function for it:
//      1  the local player's id, and the party of the entry holding it (as a unit or as a corpse): ROSTER_FindEntryByUnitOrCorpse's walk
//      2  every entry moved out of gpPartyEntries onto a D2RosterGroupStrc per party id (ROSTER_FindOrAddListNode,
//         ROSTER_PartyExtractPartyMembers)
//      3  each group's members sorted, by kill count in arena mode and by name otherwise, then the groups by party id (ROSTER_ResortAllGroups)
//      4  the local player's group and entry moved to the front (ROSTER_MoveListNodeAndEntryToFront)
//      5  the groups concatenated back into gpPartyEntries (5a, ROSTER_FlattenGroupsIntoEntryList) and freed (5b, ROSTER_FreeList)
//      6  when the local player is in no party, its entry moved to the front directly (ROSTER_MoveEntryToFront)
//      7  the party panel's rows rebuilt (PARTY_BuildRowList, Party.cpp)
//  Each caller inlines a different subset of these steps; the function heads say which.

//  The source file, as every Roster.cpp call hands it to Fog.
static const char szRosterCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\GAME\\Roster.cpp";

//  What this unit reads and writes, at the offsets its instructions use.
static_assert(offsetof(D2RosterUnitStrc, dwUnitId) == 0x10
              && offsetof(D2RosterUnitStrc, dwClassId) == 0x1C
              && offsetof(D2RosterUnitStrc, wLevel) == 0x20
              && offsetof(D2RosterUnitStrc, wPartyId) == 0x22
              && offsetof(D2RosterUnitStrc, eButton) == 0x30
              && offsetof(D2RosterUnitStrc, pRosterInfo) == 0x34
              && offsetof(D2RosterUnitStrc, pCorpse) == 0x38,
              "ROSTER_AddPlayer's stores: [esi+10h], [esi+1Ch], [esi+20h], "
              "[esi+22h], [esi+30h], [esi+34h]; the corpse walks' [ecx+38h]");
static_assert(offsetof(D2RosterUnitStrc, wGuildFlags) == 0x44
              && offsetof(D2RosterUnitStrc, szGuildTag) == 0x46
              && offsetof(D2RosterUnitStrc, szGuildName) == 0x4A
              && offsetof(D2RosterUnitStrc, szNameEx) == 0x66
              && offsetof(D2RosterUnitStrc, pNext) == 0x80,
              "ROSTER_AddPlayer: `mov [esi+44h], dx`, the copies to +46h and +4Ah, "
              "`lea eax, [esi+66h]`; every walk steps [reg+80h]");
static_assert(offsetof(D2RosterGroupStrc, wPartyId) == 0
              && offsetof(D2RosterGroupStrc, pMembers) == 4
              && offsetof(D2RosterGroupStrc, pNext) == 8,
              "the rebuild's `cmp [eax], di`, `mov ecx, [eax+4]`, `mov eax, [eax+8]`");
static_assert(offsetof(D2RosterCorpseStrc, dwCorpseId) == 0
              && offsetof(D2RosterCorpseStrc, pPrev) == 4,
              "ROSTER_EntryHasCorpse: `cmp [eax], edx`, `mov eax, [eax+4]`");
//  The kill count.
static_assert(offsetof(D2RosterUnitStrc, dwKillCount) == 0x18,
              "ROSTER_SortEntriesByKillCount: `mov ebp, [ebx+18h]` at 0x6FAAF6DC; "
              "ROSTER_OnPartyUpdate: `mov [esi+18h], edx` at 0x6FAAFB6E");
//  The enum values the original carries as literals, and the one its own assert names (ptRoster->eButton != PARTYBUTTON_NONE).
static_assert(PARTYBUTTON_NONE == 1,
              "`ptRoster->eButton != PARTYBUTTON_NONE`, 0x6FB7618C; "
              "ROSTER_EntryGetPartyButton answers `mov eax, 1` for NULL");

//1.10f: D2Client.0x6FAAEC80
//  Packet 0x5B: a player added to the roster. Its caller SCMD_HandleSrv5B_RosterAddPlayer hands on the packet's fields: +3 the GUID in ecx, +7 the
//  class zero-extended into edx, then +8 the name, the words at +0x18, +0x1A, +0x1C, +0x1E and +0x20, the guild tag at +0x22 and the guild name after
//  its terminator. `retn 20h`: eight stack arguments; nUnused is never read here. The server stores +0x1C and +0x1E as 0 and +0x20 as the guild
//  flags, so a new player's button is PARTYBUTTON_INVITE until a 0x75 says otherwise, and +0x44 holds the guild's flags. An entry already on the
//  roster is found by id or by name (the inlined strcmp at 0x6FAAECD0) and rewritten in place, id included. Otherwise the pending list -- entries
//  ROSTER_FindCorpseEntry made for a GUID it did not know, to hang a corpse on -- is searched by id only, and a hit is unlinked and reused with its
//  corpses; a miss allocates a zeroed entry. Either way the entry gets a fresh info list (ROSTER_FindCorpseEntry leaves +0x34 NULL, so nothing is
//  lost) and goes to the head of gpPartyEntries. Then the rebuild; the two paths differ in step 3 only, inlined on the first. The original inlines
//  ROSTER_FindEntryByUnitOrCorpse (step 1) on both paths and ROSTER_ResortAllGroups on the first.
void __fastcall ROSTER_AddPlayer(uint32_t dwUnitGUID, int32_t nClass, const char *szName, int16_t nLevel, int16_t nPartyId, int16_t nUnused, D2C_PartyButtons eButton, int16_t nGuildFlags, const char *szGuildTag, const char *szGuildName)
{
    D2RosterUnitStrc *pEntry;
    D2RosterUnitStrc *pPrev;
    D2RosterUnitStrc *pWalk;
    D2RosterGroupStrc *pGroup;
    D2UnitStrc *pPlayer;
    D2UnitGUID dwPlayerId;
    int16_t nMyPartyId;

    (void)nUnused;
    pPrev = NULL;
    if (dwUnitGUID == 0xFFFFFFFF)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "dwUnitId != INVALID_PLAYER_ID", (uint32_t)(UINT_PTR)szRosterCpp, 176);
        exit(-1);
    }

    for (pEntry = gpPartyEntries; pEntry; pEntry = pEntry->pNext)
    {
        if (pEntry->dwUnitId == dwUnitGUID || !strcmp(pEntry->szName, szName))
        {
            break;
        }
    }

    if (pEntry)
    {
        strcpy(pEntry->szName, szName);
        pEntry->wPartyId = nPartyId;
        pEntry->dwClassId = nClass;
        pEntry->wLevel = nLevel;
        pEntry->eButton = eButton;
        pEntry->dwUnitId = dwUnitGUID;
        pEntry->wGuildFlags = nGuildFlags;
        //  BUG: the guild tag and the guild name are copied with no bound into szGuildTag[4] and szGuildName[28]; past 27 characters the name
        //  would run into szNameEx and past 53 into pNext. LATENT, margin zero: the server's save loader zeroes the tag's fourth byte and the
        //  name's 28th, so a tag has at most three characters and a name at most 27.
        strcpy(pEntry->szGuildTag, szGuildTag);
        strcpy(pEntry->szGuildName, szGuildName);
        if (*szGuildTag)
        {
            sprintf(pEntry->szNameEx, "%s {%s}", szName, szGuildTag);
        }
        else
        {
            strcpy(pEntry->szNameEx, szName);
        }

        pPlayer = CUNIT_GetPlayer();
        dwPlayerId = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;

        nMyPartyId = -1;
        if (dwPlayerId != 0xFFFFFFFF)
        {
            for (pWalk = gpPartyEntries; pWalk; pWalk = pWalk->pNext)
            {
                if (pWalk->dwUnitId == dwPlayerId || ROSTER_EntryHasCorpse(pWalk, dwPlayerId))
                {
                    nMyPartyId = (int16_t)pWalk->wPartyId;
                    break;
                }
            }
        }

        while (gpPartyEntries)
        {
            pGroup = ROSTER_FindOrAddListNode((int16_t)gpPartyEntries->wPartyId);
            ROSTER_PartyExtractPartyMembers(pGroup);
        }

        for (pGroup = gpRosterList; pGroup; pGroup = pGroup->pNext)
        {
            if (ARENA_IsArenaMode())
            {
                pGroup->pMembers = ROSTER_SortEntriesByKillCount(pGroup->pMembers);
            }
            else
            {
                pGroup->pMembers = ROSTER_SortEntriesByName(pGroup->pMembers);
            }
        }
        ROSTER_SortList();

        ROSTER_MoveListNodeAndEntryToFront(dwPlayerId, nMyPartyId);
        ROSTER_FlattenGroupsIntoEntryList();
        ROSTER_FreeList();
        if (nMyPartyId == -1)
        {
            ROSTER_MoveEntryToFront(dwPlayerId);
        }
        PARTY_BuildRowList();
        return;
    }

    for (pEntry = gpPendingRosterEntries; pEntry; pEntry = pEntry->pNext)
    {
        if (pEntry->dwUnitId == dwUnitGUID)
        {
            break;
        }
        pPrev = pEntry;
    }

    if (pEntry)
    {
        if (pPrev)
        {
            pPrev->pNext = pEntry->pNext;
        }
        else
        {
            gpPendingRosterEntries = pEntry->pNext;
        }
        pEntry->pNext = NULL;
    }
    else
    {
        //  BUG: FOG_Alloc's answer is used untested (`rep stosd` over it, 0x21 dwords, 0x6FAAEEF0), where ROSTER_FindCorpseEntry asserts its own
        //  allocation of the same 0x84 bytes. LATENT: only a failed allocation reaches it.
        pEntry = (D2RosterUnitStrc *)(UINT_PTR)FOG_Alloc(sizeof(D2RosterUnitStrc), (uint32_t)(UINT_PTR)szRosterCpp, 213, 0);
        memset(pEntry, 0, sizeof(D2RosterUnitStrc));
    }

    pEntry->pRosterInfo = PLAYERLIST_RosterAllocPlayerInfoList();
    strcpy(pEntry->szName, szName);
    pEntry->dwClassId = nClass;
    pEntry->eButton = eButton;
    pEntry->wLevel = nLevel;
    pEntry->wPartyId = nPartyId;
    pEntry->wGuildFlags = nGuildFlags;
    pEntry->dwUnitId = dwUnitGUID;
    //  BUG: the same unbounded copy, LATENT as above.
    strcpy(pEntry->szGuildTag, szGuildTag);
    strcpy(pEntry->szGuildName, szGuildName);
    if (*szGuildTag)
    {
        sprintf(pEntry->szNameEx, "%s {%s}", szName, szGuildTag);
    }
    else
    {
        strcpy(pEntry->szNameEx, szName);
    }
    pEntry->pNext = gpPartyEntries;
    gpPartyEntries = pEntry;

    pPlayer = CUNIT_GetPlayer();
    dwPlayerId = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;

    nMyPartyId = -1;
    if (dwPlayerId != 0xFFFFFFFF)
    {
        for (pWalk = gpPartyEntries; pWalk; pWalk = pWalk->pNext)
        {
            if (pWalk->dwUnitId == dwPlayerId || ROSTER_EntryHasCorpse(pWalk, dwPlayerId))
            {
                nMyPartyId = (int16_t)pWalk->wPartyId;
                break;
            }
        }
    }

    while (gpPartyEntries)
    {
        pGroup = ROSTER_FindOrAddListNode((int16_t)gpPartyEntries->wPartyId);
        ROSTER_PartyExtractPartyMembers(pGroup);
    }
    ROSTER_ResortAllGroups();
    ROSTER_MoveListNodeAndEntryToFront(dwPlayerId, nMyPartyId);
    ROSTER_FlattenGroupsIntoEntryList();
    ROSTER_FreeList();
    if (nMyPartyId == -1)
    {
        ROSTER_MoveEntryToFront(dwPlayerId);
    }
    PARTY_BuildRowList();
}

//1.10f: D2Client.0x6FAAF050
//  Whether dwCorpseId is on the entry's corpse list (+0x38, chained through pPrev at +4). Called by ROSTER_AddPlayer (both paths) and
//  ROSTER_PartySendHostileToggle; the original inlines the other walks of this list.
int32_t __fastcall ROSTER_EntryHasCorpse(D2RosterUnitStrc *pRosterEntry, uint32_t dwCorpseId)
{
    D2RosterCorpseStrc *pCorpse;

    for (pCorpse = pRosterEntry->pCorpse; pCorpse; pCorpse = pCorpse->pPrev)
    {
        if (pCorpse->dwCorpseId == dwCorpseId)
        {
            return 1;
        }
    }
    return 0;
}

//1.10f: D2Client.0x6FAAF070
//  Packet 0x75: a player's party, level and button. Its caller is SCMD_HandleSrv75_PartyRosterUpdate. `retn 0Ch` is three stack arguments and the
//  body loads two: the first as a word (`mov ax, [esp+0Ch]`, 0x6FAAF0C5) and the third (`mov ecx, [esp+14h]`, 0x6FAAF0CA); the second, the packet's
//  +9, is never read. The update only happens for an entry matched by its own id. The search is ROSTER_FindEntryByUnitOrCorpse's, which also answers
//  the entry whose corpse list holds the GUID, and the corpse walk right after it returns on that same hit. So a corpse's GUID finds the owner and
//  changes nothing. Then the rebuild. BUG: an entry whose own id is also one of its corpses' ids would be skipped too. UNREACHABLE with a stock
//  server: every 0x8E pairs a player with a different unit, and a new unit takes the next GUID of its type. The original inlines
//  ROSTER_FindEntryByUnitOrCorpse, ROSTER_EntryHasCorpse, ROSTER_ResortAllGroups, ROSTER_MoveListNodeAndEntryToFront and
//  ROSTER_FlattenGroupsIntoEntryList here (rebuild steps 1, 3, 4 and 5a).
void __fastcall ROSTER_UpdatePartyState(D2UnitGUID dwUnitId, int16_t nPartyId, int16_t nLevel, int16_t nUnusedFlags, D2C_PartyButtons eButton)
{
    D2RosterUnitStrc *pEntry;
    D2RosterUnitStrc *pWalk;
    D2RosterGroupStrc *pGroup;
    D2UnitStrc *pPlayer;
    D2UnitGUID dwPlayerId;
    int16_t nMyPartyId;

    (void)nUnusedFlags;

    pEntry = ROSTER_FindEntryByUnitOrCorpse(dwUnitId);
    if (!pEntry)
    {
        return;
    }

    if (ROSTER_EntryHasCorpse(pEntry, dwUnitId))
    {
        return;
    }

    pEntry->wPartyId = nPartyId;
    pEntry->wLevel = nLevel;
    pEntry->eButton = eButton;
    ROSTERPETS_UpdateSkeletonPalettes();

    pPlayer = CUNIT_GetPlayer();
    dwPlayerId = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;

    nMyPartyId = -1;
    pWalk = ROSTER_FindEntryByUnitOrCorpse(dwPlayerId);
    if (pWalk)
    {
        nMyPartyId = (int16_t)pWalk->wPartyId;
    }

    while (gpPartyEntries)
    {
        pGroup = ROSTER_FindOrAddListNode((int16_t)gpPartyEntries->wPartyId);
        ROSTER_PartyExtractPartyMembers(pGroup);
    }

    for (pGroup = gpRosterList; pGroup; pGroup = pGroup->pNext)
    {
        if (ARENA_IsArenaMode())
        {
            pGroup->pMembers = ROSTER_SortEntriesByKillCount(pGroup->pMembers);
        }
        else
        {
            pGroup->pMembers = ROSTER_SortEntriesByName(pGroup->pMembers);
        }
    }
    ROSTER_SortList();

    ROSTER_MoveListNodeAndEntryToFront(dwPlayerId, nMyPartyId);

    ROSTER_FlattenGroupsIntoEntryList();

    ROSTER_FreeList();
    if (nMyPartyId == -1)
    {
        ROSTER_MoveEntryToFront(dwPlayerId);
    }
    PARTY_BuildRowList();
}

//1.10f: D2Client.0x6FAAF260
//  Packet 0x5C: a player removed from the roster. Its only code reference is SCMD_HandleSrv5C_RosterRemovePlayer's tail jump, and it ends in one of
//  its own, to PARTY_BuildRowList. An invalid GUID is a warning, not a halt (FOG_DisplayWarning, with no exit after it), and the function goes on:
//  the search then finds nothing (ROSTER_AddPlayer refuses that GUID) and the rebuild runs regardless. An entry that is found is unlinked and freed
//  with its info list and corpses. The original inlines the corpse-list free and the rebuild steps 1, 2 (ROSTER_FindOrAddListNode), 3, 4 and 5 here.
void __fastcall ROSTER_RemovePlayer(uint32_t dwPlayerGUID)
{
    D2RosterUnitStrc *pEntry;
    D2RosterUnitStrc *pPrev;
    D2RosterUnitStrc *pWalk;
    D2RosterCorpseStrc *pCorpse;
    D2RosterCorpseStrc *pPrevCorpse;
    D2RosterGroupStrc *pGroup;
    D2UnitStrc *pPlayer;
    D2UnitGUID dwPlayerId;
    int16_t nMyPartyId;
    int16_t nGroupId;

    pPrev = NULL;
    if (dwPlayerGUID == 0xFFFFFFFF)
    {
        FOG_DisplayWarning((uint32_t)(UINT_PTR) "dwUnitId != INVALID_PLAYER_ID", (uint32_t)(UINT_PTR)szRosterCpp, 286);
    }

    for (pEntry = gpPartyEntries; pEntry; pEntry = pEntry->pNext)
    {
        if (pEntry->dwUnitId == dwPlayerGUID)
        {
            break;
        }
        pPrev = pEntry;
    }

    if (pEntry)
    {
        if (pPrev)
        {
            pPrev->pNext = pEntry->pNext;
        }
        else
        {
            gpPartyEntries = pEntry->pNext;
        }

        PLAYERLIST_RosterFreePlayerInfoList(pEntry->pRosterInfo);
        pEntry->pRosterInfo = NULL;
        for (pCorpse = pEntry->pCorpse; pCorpse; pCorpse = pPrevCorpse)
        {
            pPrevCorpse = pCorpse->pPrev;
            FOG_Free((uint32_t)(UINT_PTR)pCorpse, (uint32_t)(UINT_PTR)szRosterCpp, 269, 0);
        }
        pEntry->pCorpse = NULL;

        FOG_Free((uint32_t)(UINT_PTR)pEntry, (uint32_t)(UINT_PTR)szRosterCpp, 302, 0);
    }

    pPlayer = CUNIT_GetPlayer();
    dwPlayerId = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;

    nMyPartyId = -1;
    pWalk = ROSTER_FindEntryByUnitOrCorpse(dwPlayerId);
    if (pWalk)
    {
        nMyPartyId = (int16_t)pWalk->wPartyId;
    }

    while (gpPartyEntries)
    {
        nGroupId = (int16_t)gpPartyEntries->wPartyId;
        pGroup = ROSTER_FindListNode(nGroupId);
        if (!pGroup)
        {
            pGroup = (D2RosterGroupStrc *)(UINT_PTR)FOG_Alloc(sizeof(D2RosterGroupStrc), (uint32_t)(UINT_PTR)szRosterCpp, 505, 0);
            memset(pGroup, 0, sizeof(D2RosterGroupStrc));
            pGroup->wPartyId = nGroupId;
            pGroup->pNext = gpRosterList;
            gpRosterList = pGroup;
        }
        ROSTER_PartyExtractPartyMembers(pGroup);
    }

    for (pGroup = gpRosterList; pGroup; pGroup = pGroup->pNext)
    {
        if (ARENA_IsArenaMode())
        {
            pGroup->pMembers = ROSTER_SortEntriesByKillCount(pGroup->pMembers);
        }
        else
        {
            pGroup->pMembers = ROSTER_SortEntriesByName(pGroup->pMembers);
        }
    }
    ROSTER_SortList();

    ROSTER_MoveListNodeAndEntryToFront(dwPlayerId, nMyPartyId);

    ROSTER_FlattenGroupsIntoEntryList();

    ROSTER_FreeList();

    if (nMyPartyId == -1)
    {
        ROSTER_MoveEntryToFront(dwPlayerId);
    }
    PARTY_BuildRowList();
}

//1.10f: D2Client.0x6FAAF4E0
//  Frees both lists -- the roster and the pending entries ROSTER_FindCorpseEntry parks there -- each entry with its info list and corpses, and
//  NULLs both heads. The two loops differ only in their list and the line that frees the entry. Its caller is GAME_AppModeCloseWindow. The
//  original inlines the corpse-list free in both loops.
void __cdecl ROSTER_FreeAll()
{
    D2RosterUnitStrc *pEntry;
    D2RosterUnitStrc *pNext;
    D2RosterCorpseStrc *pCorpse;
    D2RosterCorpseStrc *pPrevCorpse;

    for (pEntry = gpPartyEntries; pEntry; pEntry = pNext)
    {
        pNext = pEntry->pNext;
        PLAYERLIST_RosterFreePlayerInfoList(pEntry->pRosterInfo);
        pEntry->pRosterInfo = NULL;
        for (pCorpse = pEntry->pCorpse; pCorpse; pCorpse = pPrevCorpse)
        {
            pPrevCorpse = pCorpse->pPrev;
            FOG_Free((uint32_t)(UINT_PTR)pCorpse, (uint32_t)(UINT_PTR)szRosterCpp, 269, 0);
        }
        pEntry->pCorpse = NULL;
        FOG_Free((uint32_t)(UINT_PTR)pEntry, (uint32_t)(UINT_PTR)szRosterCpp, 330, 0);
    }
    gpPartyEntries = NULL;

    for (pEntry = gpPendingRosterEntries; pEntry; pEntry = pNext)
    {
        pNext = pEntry->pNext;
        PLAYERLIST_RosterFreePlayerInfoList(pEntry->pRosterInfo);
        pEntry->pRosterInfo = NULL;
        for (pCorpse = pEntry->pCorpse; pCorpse; pCorpse = pPrevCorpse)
        {
            pPrevCorpse = pCorpse->pPrev;
            FOG_Free((uint32_t)(UINT_PTR)pCorpse, (uint32_t)(UINT_PTR)szRosterCpp, 269, 0);
        }
        pEntry->pCorpse = NULL;
        FOG_Free((uint32_t)(UINT_PTR)pEntry, (uint32_t)(UINT_PTR)szRosterCpp, 342, 0);
    }
    gpPendingRosterEntries = NULL;
}

//1.10f: D2Client.0x6FAAF5B0
//  The roster's iterator: the entry after pEntry, or the head for NULL.
D2RosterUnitStrc *__fastcall ROSTER_PartyGetNextEntry(D2RosterUnitStrc *pEntry)
{
    if (pEntry)
    {
        return pEntry->pNext;
    }
    return gpPartyEntries;
}

//1.10f: D2Client.0x6FAAF5D0
//  The entry's display name, szNameEx at +0x66: "Name {Tag}", or the plain name when there is no tag (ROSTER_AddPlayer). A NULL entry is a
//  warning (no exit) and the function still answers NULL + 0x66 (`lea eax, [esi+66h]`, 0x6FAAF5EE). Written as the integer sum so that the NULL
//  case is the address the bytes compute and not undefined behaviour.
char *__fastcall ROSTER_GetDisplayName(D2RosterUnitStrc *pEntry)
{
    if (!pEntry)
    {
        FOG_DisplayWarning((uint32_t)(UINT_PTR) "ptRoster", (uint32_t)(UINT_PTR)szRosterCpp, 369);
    }
    return (char *)((UINT_PTR)pEntry + offsetof(D2RosterUnitStrc, szNameEx));
}

//1.10f: D2Client.0x6FAAF600
//  Rebuild step 3, by name: an insertion sort of one group's member chain, ascending by strcmp of the names, returning the new head. Every caller
//  stores the head straight back into the group (`mov [esi+4], eax`). The sort is stable: the scan moves past every entry whose name compares
//  greater than or equal to the new one's (`test edx, edx / jl`, 0x6FAAF65C..0x6FAAF65E), so equal names keep their arrival order. Each
//  entry's pNext is read before it is relinked (0x6FAAF623).
D2RosterUnitStrc *__fastcall ROSTER_SortEntriesByName(D2RosterUnitStrc *pList)
{
    D2RosterUnitStrc *pSorted;
    D2RosterUnitStrc *pEntry;
    D2RosterUnitStrc *pNextEntry;
    D2RosterUnitStrc *pScan;
    D2RosterUnitStrc *pInsertAfter;

    pSorted = NULL;
    for (pEntry = pList; pEntry; pEntry = pNextEntry)
    {
        pNextEntry = pEntry->pNext;
        pInsertAfter = NULL;
        for (pScan = pSorted; pScan; pScan = pScan->pNext)
        {
            if (strcmp(pEntry->szName, pScan->szName) < 0)
            {
                break;
            }
            pInsertAfter = pScan;
        }
        if (pInsertAfter)
        {
            pEntry->pNext = pInsertAfter->pNext;
            pInsertAfter->pNext = pEntry;
        }
        else
        {
            pEntry->pNext = pSorted;
            pSorted = pEntry;
        }
    }
    return pSorted;
}

//1.10f: D2Client.0x6FAAF6C0
//  Rebuild step 3 in arena mode: the same insertion sort by kill count, descending, and stable the same way -- the scan stops at the first entry
//  the new one beats. The compare is signed (`cmp ebp, [edx+18h] / jg`, 0x6FAAF6DF..0x6FAAF6E2) on an `unsigned int` member, so the C casts: a
//  kill count can be negative (ROSTER_OnPartyUpdate). The callers store the returned head as the name sort's do.
D2RosterUnitStrc *__fastcall ROSTER_SortEntriesByKillCount(D2RosterUnitStrc *pList)
{
    D2RosterUnitStrc *pSorted;
    D2RosterUnitStrc *pEntry;
    D2RosterUnitStrc *pNextEntry;
    D2RosterUnitStrc *pScan;
    D2RosterUnitStrc *pInsertAfter;

    pSorted = NULL;
    for (pEntry = pList; pEntry; pEntry = pNextEntry)
    {
        pNextEntry = pEntry->pNext;
        pInsertAfter = NULL;
        for (pScan = pSorted; pScan; pScan = pScan->pNext)
        {
            if ((int32_t)pEntry->dwKillCount > (int32_t)pScan->dwKillCount)
            {
                break;
            }
            pInsertAfter = pScan;
        }
        if (pInsertAfter)
        {
            pEntry->pNext = pInsertAfter->pNext;
            pInsertAfter->pNext = pEntry;
        }
        else
        {
            pEntry->pNext = pSorted;
            pSorted = pEntry;
        }
    }
    return pSorted;
}

//1.10f: D2Client.0x6FAAF720
//  The group for a party id on gpRosterList, or NULL. The ids compare as words (`cmp [eax], cx`, 0x6FAAF729): only the low 16 bits of the key
//  meet the unsigned member. 0xFFFF is an ordinary key here.
D2RosterGroupStrc *__fastcall ROSTER_FindListNode(int16_t nPartyId)
{
    D2RosterGroupStrc *pGroup;

    for (pGroup = gpRosterList; pGroup; pGroup = pGroup->pNext)
    {
        if ((int16_t)pGroup->wPartyId == nPartyId)
        {
            return pGroup;
        }
    }
    return NULL;
}

//1.10f: D2Client.0x6FAAF740
//  Rebuild step 2's group: the group for a party id, made when there is none -- a zeroed 12-byte D2RosterGroupStrc (FOG_Alloc) pushed at the
//  front of gpRosterList. Every caller hands the answer to ROSTER_PartyExtractPartyMembers. The original writes ROSTER_FindListNode's walk in its
//  own body, while every site that inlines this function calls ROSTER_FindListNode.
D2RosterGroupStrc *__fastcall ROSTER_FindOrAddListNode(int16_t nPartyId)
{
    D2RosterGroupStrc *pGroup;

    pGroup = ROSTER_FindListNode(nPartyId);
    if (pGroup)
    {
        return pGroup;
    }

    //  BUG: FOG_Alloc's answer is used untested (the three stores at 0x6FAAF773..0x6FAAF778), as at every other copy of this allocation in the
    //  unit. LATENT: only a failed allocation reaches it.
    pGroup = (D2RosterGroupStrc *)(UINT_PTR)FOG_Alloc(sizeof(D2RosterGroupStrc), (uint32_t)(UINT_PTR)szRosterCpp, 505, 0);
    memset(pGroup, 0, sizeof(D2RosterGroupStrc));
    pGroup->wPartyId = nPartyId;
    pGroup->pNext = gpRosterList;
    gpRosterList = pGroup;
    return pGroup;
}

//1.10f: D2Client.0x6FAAF790
//  Rebuild step 5b: every group node freed and the head NULLed. The members are not touched; step 5a has already moved them back onto
//  gpPartyEntries.
void __cdecl ROSTER_FreeList()
{
    D2RosterGroupStrc *pGroup;
    D2RosterGroupStrc *pNextGroup;

    for (pGroup = gpRosterList; pGroup; pGroup = pNextGroup)
    {
        pNextGroup = pGroup->pNext;
        FOG_Free((uint32_t)(UINT_PTR)pGroup, (uint32_t)(UINT_PTR)szRosterCpp, 556, 0);
    }
    gpRosterList = NULL;
}

//1.10f: D2Client.0x6FAAF7D0
//  Rebuild step 2's move: every entry of gpPartyEntries whose party id is the group's is unlinked and pushed at the front of the group's
//  members, so a group holds its members in the reverse of their roster order until step 3 sorts them. The ids compare as words (`cmp dx, [ecx]`,
//  0x6FAAF7E2). The trailing pointer does not move past an entry that was taken (0x6FAAF814 is the only write to it inside the loop).
void __fastcall ROSTER_PartyExtractPartyMembers(D2RosterGroupStrc *pGroup)
{
    D2RosterUnitStrc *pEntry;
    D2RosterUnitStrc *pPrev;
    D2RosterUnitStrc *pMoved;

    pPrev = NULL;
    pEntry = gpPartyEntries;
    while (pEntry)
    {
        if (pEntry->wPartyId == pGroup->wPartyId)
        {
            pMoved = pEntry;
            pEntry = pEntry->pNext;
            if (pPrev)
            {
                pPrev->pNext = pEntry;
            }
            else
            {
                gpPartyEntries = pEntry;
            }
            pMoved->pNext = pGroup->pMembers;
            pGroup->pMembers = pMoved;
        }
        else
        {
            pPrev = pEntry;
            pEntry = pEntry->pNext;
        }
    }
}

//1.10f: D2Client.0x6FAAF830
//  Rebuild step 3's second half: gpRosterList itself insertion-sorted ascending by party id, stable. The compare is an unsigned word compare
//  (`cmp bx, [eax] / jb`, 0x6FAAF84F..0x6FAAF852), so a group with no party (0xFFFF) sorts last. __stdcall with no arguments, a bare `retn`.
void __stdcall ROSTER_SortList()
{
    D2RosterGroupStrc *pSorted;
    D2RosterGroupStrc *pGroup;
    D2RosterGroupStrc *pNextGroup;
    D2RosterGroupStrc *pScan;
    D2RosterGroupStrc *pInsertAfter;

    pSorted = NULL;
    for (pGroup = gpRosterList; pGroup; pGroup = pNextGroup)
    {
        pNextGroup = pGroup->pNext;
        pInsertAfter = NULL;
        for (pScan = pSorted; pScan; pScan = pScan->pNext)
        {
            if (pGroup->wPartyId < pScan->wPartyId)
            {
                break;
            }
            pInsertAfter = pScan;
        }
        if (pInsertAfter)
        {
            pGroup->pNext = pInsertAfter->pNext;
            pInsertAfter->pNext = pGroup;
        }
        else
        {
            pGroup->pNext = pSorted;
            pSorted = pGroup;
        }
    }
    gpRosterList = pSorted;
}

//1.10f: D2Client.0x6FAAF890
//  Rebuild step 4: the local player's group moved to the front of gpRosterList, then the local player's entry to the front of that group. Each
//  move needs a predecessor: a node already at the head stays put. void: the body leaves the group, the list head or NULL in eax (the head, not
//  NULL, when the party id is 0xFFFF: `jz` at 0x6FAAF8A0 with eax = gpRosterList), but no caller reads it.
void __fastcall ROSTER_MoveListNodeAndEntryToFront(D2UnitGUID dwUnitId, int16_t nPartyId)
{
    D2RosterGroupStrc *pGroup;
    D2RosterGroupStrc *pPrevGroup;
    D2RosterUnitStrc *pMember;
    D2RosterUnitStrc *pPrevMember;

    pPrevGroup = NULL;
    pPrevMember = NULL;
    if (nPartyId == -1)
    {
        return;
    }
    for (pGroup = gpRosterList; pGroup; pGroup = pGroup->pNext)
    {
        if ((int16_t)pGroup->wPartyId == nPartyId)
        {
            break;
        }
        pPrevGroup = pGroup;
    }
    if (!pGroup)
    {
        return;
    }
    if (pPrevGroup)
    {
        pPrevGroup->pNext = pGroup->pNext;
        pGroup->pNext = gpRosterList;
        gpRosterList = pGroup;
    }
    for (pMember = pGroup->pMembers; pMember; pMember = pMember->pNext)
    {
        if (pMember->dwUnitId == dwUnitId)
        {
            break;
        }
        pPrevMember = pMember;
    }
    if (pMember && pPrevMember)
    {
        pPrevMember->pNext = pMember->pNext;
        pMember->pNext = pGroup->pMembers;
        pGroup->pMembers = pMember;
    }
}

//1.10f: D2Client.0x6FAAF900
//  Rebuild step 3, out of line: each group's members sorted -- by kill count in arena mode, by name otherwise, ARENA_IsArenaMode asked once per
//  group -- then the group list by party id. One caller, ROSTER_AddPlayer's second path (0x6FAAF008). The original writes
//  ROSTER_SortEntriesByKillCount, ROSTER_SortEntriesByName and ROSTER_SortList inline in its own body, while every site that inlines this
//  function calls the three.
void __fastcall ROSTER_ResortAllGroups()
{
    D2RosterGroupStrc *pGroup;
    D2RosterUnitStrc *pSorted;

    for (pGroup = gpRosterList; pGroup; pGroup = pGroup->pNext)
    {
        if (ARENA_IsArenaMode())
        {
            pSorted = ROSTER_SortEntriesByKillCount(pGroup->pMembers);
        }
        else
        {
            pSorted = ROSTER_SortEntriesByName(pGroup->pMembers);
        }
        pGroup->pMembers = pSorted;
    }

    ROSTER_SortList();
}

//1.10f: D2Client.0x6FAAFA90
//  Rebuild step 5a: the groups' member chains concatenated back into gpPartyEntries, in group order, each group's pMembers NULLed. The head is
//  written once, by the first group with members; an empty group leaves the tail where it was.
void __cdecl ROSTER_FlattenGroupsIntoEntryList()
{
    D2RosterGroupStrc *pGroup;
    D2RosterUnitStrc *pTail;
    D2RosterUnitStrc *pNext;

    pTail = NULL;
    for (pGroup = gpRosterList; pGroup; pGroup = pGroup->pNext)
    {
        if (pTail)
        {
            pTail->pNext = pGroup->pMembers;
        }
        else
        {
            gpPartyEntries = pGroup->pMembers;
            pTail = pGroup->pMembers;
        }
        pGroup->pMembers = NULL;
        if (pTail)
        {
            for (pNext = pTail->pNext; pNext; pNext = pTail->pNext)
            {
                pTail = pNext;
            }
        }
    }
}

//1.10f: D2Client.0x6FAAFAE0
//  Rebuild step 6: the entry for dwUnitId moved to the head of gpPartyEntries. Every caller runs it only when the local player is in no party.
//  The walk does not stop at the match. After the move the cursor steps to the moved entry's new pNext (the old head) with the moved entry as its
//  predecessor, so everything before the match is walked a second time; the entry itself is now behind the cursor and is not met again. A second
//  entry with the same id would be moved too.
void __fastcall ROSTER_MoveEntryToFront(D2UnitGUID dwUnitId)
{
    D2RosterUnitStrc *pEntry;
    D2RosterUnitStrc *pPrev;

    pPrev = NULL;
    for (pEntry = gpPartyEntries; pEntry; pEntry = pEntry->pNext)
    {
        if (pEntry->dwUnitId == dwUnitId && pPrev)
        {
            pPrev->pNext = pEntry->pNext;
            pEntry->pNext = gpPartyEntries;
            gpPartyEntries = pEntry;
        }
        pPrev = pEntry;
    }
}

//1.10f: D2Client.0x6FAAFB20
//  Packet 0x65: a player's kill count. Its caller SCMD_HandleSrv65_RosterUpdate passes the GUID in ecx and the packet's word at +5 in edx; only
//  its low 16 bits are read.
//  The entry is found by id or by one of its corpses and -- unlike ROSTER_UpdatePartyState, which walks the corpses a second time and gives up on
//  a corpse hit -- a corpse's GUID updates its owner. A GUID of -1 or one on no entry returns at once (0x6FAAFB52), and the roster is not rebuilt.
//  The count is a signed word, sign-extended by hand: `test dh, dh / jns`, then `and edx, 0FFFFh` and, for a set bit 15, `sub edx, 10000h`
//  (0x6FAAFB55..0x6FAAFB67). The C writes those operations; the value is (__int16)nKillCount's.
//  Then the rebuild, ending in a tail jump to PARTY_BuildRowList (0x6FAAFD22). The original inlines ROSTER_FindEntryByUnitOrCorpse,
//  ROSTER_FindOrAddListNode, ROSTER_ResortAllGroups, ROSTER_MoveListNodeAndEntryToFront and ROSTER_FlattenGroupsIntoEntryList here.
void __fastcall ROSTER_OnPartyUpdate(uint32_t dwUnitId, int32_t nKillCount)
{
    D2RosterUnitStrc *pEntry;
    D2RosterUnitStrc *pWalk;
    D2RosterGroupStrc *pGroup;
    D2UnitStrc *pPlayer;
    D2UnitGUID dwPlayerId;
    int16_t nMyPartyId;
    int16_t nGroupId;

    pEntry = ROSTER_FindEntryByUnitOrCorpse(dwUnitId);
    if (!pEntry)
    {
        return;
    }

    if (nKillCount & 0x8000)
    {
        pEntry->dwKillCount = (nKillCount & 0xFFFF) - 0x10000;
    }
    else
    {
        pEntry->dwKillCount = nKillCount & 0xFFFF;
    }

    pPlayer = CUNIT_GetPlayer();
    dwPlayerId = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;

    nMyPartyId = -1;
    pWalk = ROSTER_FindEntryByUnitOrCorpse(dwPlayerId);
    if (pWalk)
    {
        nMyPartyId = (int16_t)pWalk->wPartyId;
    }

    while (gpPartyEntries)
    {
        nGroupId = (int16_t)gpPartyEntries->wPartyId;
        pGroup = ROSTER_FindListNode(nGroupId);
        if (!pGroup)
        {
            pGroup = (D2RosterGroupStrc *)(UINT_PTR)FOG_Alloc(sizeof(D2RosterGroupStrc), (uint32_t)(UINT_PTR)szRosterCpp, 505, 0);
            memset(pGroup, 0, sizeof(D2RosterGroupStrc));
            pGroup->wPartyId = nGroupId;
            pGroup->pNext = gpRosterList;
            gpRosterList = pGroup;
        }
        ROSTER_PartyExtractPartyMembers(pGroup);
    }

    for (pGroup = gpRosterList; pGroup; pGroup = pGroup->pNext)
    {
        if (ARENA_IsArenaMode())
        {
            pGroup->pMembers = ROSTER_SortEntriesByKillCount(pGroup->pMembers);
        }
        else
        {
            pGroup->pMembers = ROSTER_SortEntriesByName(pGroup->pMembers);
        }
    }
    ROSTER_SortList();

    ROSTER_MoveListNodeAndEntryToFront(dwPlayerId, nMyPartyId);

    ROSTER_FlattenGroupsIntoEntryList();

    ROSTER_FreeList();
    if (nMyPartyId == -1)
    {
        ROSTER_MoveEntryToFront(dwPlayerId);
    }
    PARTY_BuildRowList();
}

//1.10f: D2Client.0x6FAAFD30
//  An entry's kill count (+0x18), 0 for NULL.
int32_t __fastcall ROSTER_EntryGetKillCount(D2RosterUnitStrc *pEntry)
{
    if (!pEntry)
    {
        return 0;
    }
    return pEntry->dwKillCount;
}

//1.10f: D2Client.0x6FAAFD40
//  An entry's level (+0x20, a word), 0 for NULL: `xor eax, eax` first, so the upper half of eax is 0 on both paths.
int32_t __fastcall ROSTER_EntryGetLevel(D2RosterUnitStrc *pEntry)
{
    if (!pEntry)
    {
        return 0;
    }
    return pEntry->wLevel;
}

//1.10f: D2Client.0x6FAAFD50
//  An entry's class (+0x1C), 0 for NULL -- which is the amazon's id, so a caller cannot tell the two apart.
int32_t __fastcall ROSTER_EntryGetClassId(D2RosterUnitStrc *pEntry)
{
    if (!pEntry)
    {
        return 0;
    }
    return pEntry->dwClassId;
}

//1.10f: D2Client.0x6FAAFD60
//  Rebuild step 1's search, out of line: the roster entry for a GUID -- the entry's own id, or one of its corpses' -- or NULL, and NULL for -1.
//  The rebuild's own copies of this walk are inlined in the original. The original inlines ROSTER_EntryHasCorpse here.
D2RosterUnitStrc *__fastcall ROSTER_FindEntryByUnitOrCorpse(D2UnitGUID dwUnitId)
{
    D2RosterUnitStrc *pEntry;

    if (dwUnitId == 0xFFFFFFFF)
    {
        return NULL;
    }
    for (pEntry = gpPartyEntries; pEntry; pEntry = pEntry->pNext)
    {
        if (pEntry->dwUnitId == dwUnitId)
        {
            return pEntry;
        }
        if (ROSTER_EntryHasCorpse(pEntry, dwUnitId))
        {
            return pEntry;
        }
    }
    return NULL;
}

//1.10f: D2Client.0x6FAAFDA0
//  The entry's party id, but only if someone else is in it: the entries sharing its party id are counted, the entry itself among them, and -1 is
//  the answer unless there are more than one (`cmp esi, 1 / jg`, signed, 0x6FAAFDC6). -1 too for a NULL entry or an empty roster. The id is
//  read at +0x22 (`mov ax, [ecx+22h]`, 0x6FAAFDB1) and compared as a word.
//  It returns a word: the body writes only ax (that load and `or ax, 0FFFFh`, 0x6FAAFDCB), so the upper half of eax is whatever the caller left
//  there, and every caller reads 16 bits.
int16_t __fastcall ROSTER_PartyGetSharedPartyId(D2RosterUnitStrc *pRosterEntry)
{
    D2RosterUnitStrc *pEntry;
    int32_t nMembers;
    int16_t nPartyId;

    nMembers = 0;
    pEntry = gpPartyEntries;
    if (!pRosterEntry || !pEntry)
    {
        return -1;
    }
    nPartyId = (int16_t)pRosterEntry->wPartyId;
    do
    {
        if ((int16_t)pEntry->wPartyId == nPartyId)
        {
            ++nMembers;
        }
        pEntry = pEntry->pNext;
    } while (pEntry);
    if (nMembers <= 1)
    {
        return -1;
    }
    return nPartyId;
}

//1.10f: D2Client.0x6FAAFDE0
//  A unit's party id through the roster -- found by its own GUID or one of its corpses' -- or -1, for a NULL unit too. A word, like the one
//  above: `or ax, 0FFFFh` and `mov ax, [ecx+22h]` (0x6FAAFE17, 0x6FAAFE1C). Its one reference is ROSTER_GetPartyIdForUnit's tail jump
//  (0x6FB2A6E9). The original inlines ROSTER_FindEntryByUnitOrCorpse here.
int16_t __fastcall ROSTER_FindPartyIdByUnit(D2UnitStrc *pUnit)
{
    D2UnitGUID dwUnitId;
    D2RosterUnitStrc *pEntry;

    if (!pUnit)
    {
        return -1;
    }
    dwUnitId = pUnit->dwUnitId;

    pEntry = ROSTER_FindEntryByUnitOrCorpse(dwUnitId);
    if (!pEntry)
    {
        return -1;
    }
    return (int16_t)pEntry->wPartyId;
}

//  The info flags the original passes as literals: `push 1`, `push 2` and `push 4` into PLAYERLIST_RosterTestInfoFlag and
//  PLAYERLIST_RosterSetInfoFlag.
static_assert(ROSTERINFOFLAG_LOOT == 1 && ROSTERINFOFLAG_IGNORE == 2
                  && ROSTERINFOFLAG_SQUELCH == 4,
              "`push 1`, `push 2`, `push 4` before PLAYERLIST_RosterTestInfoFlag and "
              "PLAYERLIST_RosterSetInfoFlag, 0x6FAAFE46..0x6FAB005A");
//  The packet commands sent below, each a `mov ecx, N` (or `xor ecx, ecx`) into a callee that stores cl, so the enum is one byte.
static_assert(PARTYCMD_TOGGLELOOT == 1 && PARTYCMD_TOGGLEIGNORE == 2
                  && PARTYCMD_TOGGLESQUELCH == 3 && PARTYCMD_TOGGLEHOSTILE == 4,
              "0x5D: `mov ecx, 1/2/3/4` at 0x6FAAFFA2, 0x6FAB0012, 0x6FAB0082, "
              "0x6FAB05C1");
static_assert(PARTYCMD_NONE == 0 && PARTYCMD_INVITE == 6 && PARTYCMD_CANCELINVITE == 7
                  && PARTYCMD_ACCEPTINVITE == 8 && PARTYCMD_LEAVEPARTY == 9
                  && sizeof(D2C_PartyCommands) == 1,
              "0x5E: `xor ecx, ecx` 0x6FAB0095, `mov ecx, 6/8/7/9` at 0x6FAB00B5, "
              "0x6FAB00DA, 0x6FAB00E4, 0x6FAB00EB and 0x6FAB052A");

//1.10f: D2Client.0x6FAAFE30
//  The four info-flag predicates (0x6FAAFE30..0x6FAAFF22): whether the local player's info list holds a flag for this entry --
//  PLAYERLIST_RosterTestInfoFlag with the local GUID first and the entry's second. A NULL entry answers 0. With no local player the GUID is -1,
//  and PLAYERLIST_RosterTestInfoFlag answers 0 for it: the info-list lookup it calls refuses -1 (0x6FAB0D89).
//  The answer is the mask, not a BOOL: PLAYERLIST_RosterTestInfoFlag ends in `and eax, ecx` (0x6FB4B933), so the loot, ignore and squelch
//  predicates answer 1, 2 and 4 for a set flag; only ROSTER_EntryIsHostile answers 1 (`mov eax, 1`, 0x6FAAFF18). Every caller tests the answer
//  for non-zero, so the difference is never seen.
//  The three that return the answer carry the call twice, once per CUNIT_GetPlayer outcome (0x6FAAFE4A and 0x6FAAFE5B here);
//  ROSTER_EntryIsHostile, which tests it, has one. The C writes one call with the GUID chosen before it: both copies pass the same arguments to
//  the same callee.
BOOL __fastcall ROSTER_EntryAllowsLoot(D2RosterUnitStrc *pEntry)
{
    D2UnitStrc *pPlayer;

    if (!pEntry)
    {
        return FALSE;
    }
    pPlayer = CUNIT_GetPlayer();
    return PLAYERLIST_RosterTestInfoFlag(pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF, pEntry->dwUnitId, ROSTERINFOFLAG_LOOT);
}

//1.10f: D2Client.0x6FAAFE70
//  ROSTERINFOFLAG_IGNORE (`push 2` at 0x6FAAFE86 and 0x6FAAFE97), as ROSTER_EntryAllowsLoot.
BOOL __fastcall ROSTER_EntryIsIgnored(D2RosterUnitStrc *pEntry)
{
    D2UnitStrc *pPlayer;

    if (!pEntry)
    {
        return FALSE;
    }
    pPlayer = CUNIT_GetPlayer();
    return PLAYERLIST_RosterTestInfoFlag(pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF, pEntry->dwUnitId, ROSTERINFOFLAG_IGNORE);
}

//1.10f: D2Client.0x6FAAFEB0
//  ROSTERINFOFLAG_SQUELCH (`push 4` at 0x6FAAFEC6 and 0x6FAAFED7), as ROSTER_EntryAllowsLoot.
BOOL __fastcall ROSTER_EntryIsSquelched(D2RosterUnitStrc *pEntry)
{
    D2UnitStrc *pPlayer;

    if (!pEntry)
    {
        return FALSE;
    }
    pPlayer = CUNIT_GetPlayer();
    return PLAYERLIST_RosterTestInfoFlag(pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF, pEntry->dwUnitId, ROSTERINFOFLAG_SQUELCH);
}

//1.10f: D2Client.0x6FAAFEF0
//  ROSTERINFOFLAG_HOSTILE (`push 8`, 0x6FAAFF0B), answered as 1 or 0. The raw flag; ROSTER_IsHostileTo is the test that also looks at the two
//  parties. __fastcall: with one argument the two conventions both put it in ecx, and the bare `retn` (0x6FAAFF1E) purges nothing.
BOOL __fastcall ROSTER_EntryIsHostile(D2RosterUnitStrc *pEntry)
{
    D2UnitStrc *pPlayer;

    if (!pEntry)
    {
        return FALSE;
    }
    pPlayer = CUNIT_GetPlayer();
    if (PLAYERLIST_RosterTestInfoFlag(pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF, pEntry->dwUnitId, ROSTERINFOFLAG_HOSTILE))
    {
        return TRUE;
    }
    return FALSE;
}

//1.10f: D2Client.0x6FAAFF30
//  The entry's party-panel button (+0x30), or PARTYBUTTON_NONE for NULL (`mov eax, 1`, 0x6FAAFF38). The value is one D2C_PartyButtons.
//  PARTY_BuildRowList puts it in the row's invite button, and PARTY_OnLButtonUp copies it back there after ROSTER_PartySendButtonCommand has
//  changed it.
D2C_PartyButtons __fastcall ROSTER_EntryGetPartyButton(D2RosterUnitStrc *pEntry)
{
    if (pEntry)
    {
        return pEntry->eButton;
    }
    return PARTYBUTTON_NONE;
}

//1.10f: D2Client.0x6FAAFF40
//  The three info-flag toggles (0x6FAAFF40..0x6FAB008E): the party panel's loot, ignore and squelch boxes (PARTY_OnLButtonUp, one call each).
//  The local player's info entry for the target is made if it is missing (PLAYERLIST_RosterGetOrAddInfoEntry), the flag is read and written
//  back inverted (`neg / sbb / inc` is `== 0`, 0x6FAAFF74..0x6FAAFF78) and the server is told with packet 0x5D. The flag sent is read again,
//  after a second CUNIT_GetPlayer, so what is sent is what was stored: PLAYERLIST_RosterSetInfoFlag writes nothing when the local GUID has no
//  info list or is the target's own, and PLAYERLIST_RosterGetOrAddInfoEntry makes no entry in either case.
//  Packet 0x5D's command and the flag are not the same number:
//      loot     flag 1   command 1   ROSTER_ToggleLootAndNotify
//      ignore   flag 2   command 2   ROSTER_ToggleIgnoreAndNotify
//      squelch  flag 4   command 3   ROSTER_ToggleSquelchAndNotify
//      hostile  flag 8   command 4   ROSTER_PartySendHostileToggle
//  The third argument of MSG_SendClt5D is PLAYERLIST_RosterTestInfoFlag's answer, the mask (1, 2, 4 or 8), but the packet carries 0 or 1:
//  MSG_SendClt5D stores a literal 1 for any non-zero bToggle. None of the three tells Battle.net about a squelch: PARTY_OnLButtonUp does, after
//  its call to ROSTER_ToggleSquelchAndNotify. The first half (one CUNIT_GetPlayer, PLAYERLIST_RosterGetOrAddInfoEntry, the read and the
//  inverted write) is ROSTER_PartySendHostileToggle's too.
void __fastcall ROSTER_ToggleLootAndNotify(D2RosterUnitStrc *pEntry)
{
    D2UnitStrc *pPlayer;
    D2UnitGUID dwPlayerGUID;
    int32_t bWasSet;

    if (!pEntry)
    {
        return;
    }
    pPlayer = CUNIT_GetPlayer();
    dwPlayerGUID = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;
    PLAYERLIST_RosterGetOrAddInfoEntry(dwPlayerGUID, pEntry->dwUnitId);
    bWasSet = PLAYERLIST_RosterTestInfoFlag(dwPlayerGUID, pEntry->dwUnitId, ROSTERINFOFLAG_LOOT);
    PLAYERLIST_RosterSetInfoFlag(dwPlayerGUID, pEntry->dwUnitId, ROSTERINFOFLAG_LOOT, bWasSet == 0);

    pPlayer = CUNIT_GetPlayer();
    MSG_SendClt5D(PARTYCMD_TOGGLELOOT, pEntry->dwUnitId, PLAYERLIST_RosterTestInfoFlag(pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF, pEntry->dwUnitId, ROSTERINFOFLAG_LOOT));
}

//1.10f: D2Client.0x6FAAFFB0
//  ROSTERINFOFLAG_IGNORE, packet 0x5D command 2 (`mov ecx, 2`, 0x6FAB0012).
void __fastcall ROSTER_ToggleIgnoreAndNotify(D2RosterUnitStrc *pEntry)
{
    D2UnitStrc *pPlayer;
    D2UnitGUID dwPlayerGUID;
    int32_t bWasSet;

    if (!pEntry)
    {
        return;
    }
    pPlayer = CUNIT_GetPlayer();
    dwPlayerGUID = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;
    PLAYERLIST_RosterGetOrAddInfoEntry(dwPlayerGUID, pEntry->dwUnitId);
    bWasSet = PLAYERLIST_RosterTestInfoFlag(dwPlayerGUID, pEntry->dwUnitId, ROSTERINFOFLAG_IGNORE);
    PLAYERLIST_RosterSetInfoFlag(dwPlayerGUID, pEntry->dwUnitId, ROSTERINFOFLAG_IGNORE, bWasSet == 0);

    pPlayer = CUNIT_GetPlayer();
    MSG_SendClt5D(PARTYCMD_TOGGLEIGNORE, pEntry->dwUnitId, PLAYERLIST_RosterTestInfoFlag(pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF, pEntry->dwUnitId, ROSTERINFOFLAG_IGNORE));
}

//1.10f: D2Client.0x6FAB0020
//  ROSTERINFOFLAG_SQUELCH, flag 4 (`push 4`, 0x6FAB005A) and packet 0x5D command 3 (`mov ecx, 3`, 0x6FAB0082).
void __fastcall ROSTER_ToggleSquelchAndNotify(D2RosterUnitStrc *pEntry)
{
    D2UnitStrc *pPlayer;
    D2UnitGUID dwPlayerGUID;
    int32_t bWasSet;

    if (!pEntry)
    {
        return;
    }
    pPlayer = CUNIT_GetPlayer();
    dwPlayerGUID = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;
    PLAYERLIST_RosterGetOrAddInfoEntry(dwPlayerGUID, pEntry->dwUnitId);
    bWasSet = PLAYERLIST_RosterTestInfoFlag(dwPlayerGUID, pEntry->dwUnitId, ROSTERINFOFLAG_SQUELCH);
    PLAYERLIST_RosterSetInfoFlag(dwPlayerGUID, pEntry->dwUnitId, ROSTERINFOFLAG_SQUELCH, bWasSet == 0);

    pPlayer = CUNIT_GetPlayer();
    MSG_SendClt5D(PARTYCMD_TOGGLESQUELCH, pEntry->dwUnitId, PLAYERLIST_RosterTestInfoFlag(pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF, pEntry->dwUnitId, ROSTERINFOFLAG_SQUELCH));
}

//1.10f: D2Client.0x6FAB0090
//  The party button, clicked (PARTY_OnLButtonUp): the entry's button picks the packet 0x5E command and what the button becomes, the server is
//  told, and the roster is rebuilt.
//      eButton   becomes   command
//      INVITE    CANCEL    6
//      NONE      --        --       assert `ptRoster->eButton != PARTYBUTTON_NONE`, then exit(-1)
//      ACCEPT    NONE      8
//      LEAVE     NONE      9
//      CANCEL    INVITE    7
//      above 4   unchanged 0        no command: the server has no handler for it
//  The jump table is at 0x6FAB0314 behind `cmp edx, 4 / ja` (0x6FAB00A2, unsigned). The command is built in ecx, zeroed at 0x6FAB0095 for the
//  default arm, and MSG_SendClt5E takes it in cl, a D2C_PartyCommands.
//  The original inlines rebuild steps 1, 2 (ROSTER_FindOrAddListNode), 3, 4, 5a, 5b and 6 here; it calls ROSTER_FindListNode,
//  ROSTER_PartyExtractPartyMembers, the three sorts and, as a tail jump, PARTY_BuildRowList (0x6FAB030C).
void __fastcall ROSTER_PartySendButtonCommand(D2RosterUnitStrc *pEntry)
{
    D2RosterUnitStrc *pWalk;
    D2RosterGroupStrc *pGroup;
    D2UnitStrc *pPlayer;
    D2UnitGUID dwPlayerId;
    int16_t nMyPartyId;
    int16_t nGroupId;
    D2C_PartyCommands nCommand;

    nCommand = PARTYCMD_NONE;
    if (!pEntry)
    {
        return;
    }
    switch (pEntry->eButton)
    {
    case PARTYBUTTON_INVITE:
        pEntry->eButton = PARTYBUTTON_CANCEL;
        nCommand = PARTYCMD_INVITE;
        break;

    case PARTYBUTTON_NONE:
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "ptRoster->eButton != PARTYBUTTON_NONE", (uint32_t)(UINT_PTR)szRosterCpp, 1213);
        exit(-1);

    case PARTYBUTTON_ACCEPT:
        nCommand = PARTYCMD_ACCEPTINVITE;
        pEntry->eButton = PARTYBUTTON_NONE;
        break;

    case PARTYBUTTON_LEAVE:
        nCommand = PARTYCMD_LEAVEPARTY;
        pEntry->eButton = PARTYBUTTON_NONE;
        break;

    case PARTYBUTTON_CANCEL:
        pEntry->eButton = PARTYBUTTON_INVITE;
        nCommand = PARTYCMD_CANCELINVITE;
        break;

    default:
        break;
    }
    MSG_SendClt5E(nCommand, pEntry->dwUnitId);

    pPlayer = CUNIT_GetPlayer();
    dwPlayerId = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;

    nMyPartyId = -1;
    pWalk = ROSTER_FindEntryByUnitOrCorpse(dwPlayerId);
    if (pWalk)
    {
        nMyPartyId = (int16_t)pWalk->wPartyId;
    }

    while (gpPartyEntries)
    {
        nGroupId = (int16_t)gpPartyEntries->wPartyId;
        pGroup = ROSTER_FindListNode(nGroupId);
        if (!pGroup)
        {
            pGroup = (D2RosterGroupStrc *)(UINT_PTR)FOG_Alloc(sizeof(D2RosterGroupStrc), (uint32_t)(UINT_PTR)szRosterCpp, 505, 0);
            memset(pGroup, 0, sizeof(D2RosterGroupStrc));
            pGroup->wPartyId = nGroupId;
            pGroup->pNext = gpRosterList;
            gpRosterList = pGroup;
        }
        ROSTER_PartyExtractPartyMembers(pGroup);
    }

    for (pGroup = gpRosterList; pGroup; pGroup = pGroup->pNext)
    {
        if (ARENA_IsArenaMode())
        {
            pGroup->pMembers = ROSTER_SortEntriesByKillCount(pGroup->pMembers);
        }
        else
        {
            pGroup->pMembers = ROSTER_SortEntriesByName(pGroup->pMembers);
        }
    }
    ROSTER_SortList();

    ROSTER_MoveListNodeAndEntryToFront(dwPlayerId, nMyPartyId);

    ROSTER_FlattenGroupsIntoEntryList();

    ROSTER_FreeList();

    if (nMyPartyId == -1)
    {
        ROSTER_MoveEntryToFront(dwPlayerId);
    }
    PARTY_BuildRowList();
}

//1.10f: D2Client.0x6FAB0330
//  The leave-party button (PARTY_OnLButtonUp). The local player's own entry leaves its party here first -- button PARTYBUTTON_NONE, party id
//  0xFFFF (0x6FAB03A7, 0x6FAB03AE) -- the roster is rebuilt, and only then is the server told: packet 0x5E, command 9, naming the entry found,
//  as a tail jump (0x6FAB0532). An entry found through one of its corpses names its owner.
//  The entry is found by the local GUID or a corpse's and must exist: `ptRoster` is asserted (0x6FAB0382), the original's own name for it. The
//  same lookup, the same two stores and the same rebuild close ROSTER_PartySendHostileToggle, asserting at the same source line, so the original
//  has them in a function both inline (the image holds no out-of-line copy of it).
//  The original inlines that lookup (its walk is ROSTER_FindEntryByUnitOrCorpse's) and rebuild steps 1, 3, 4 and 5a here, and calls
//  ROSTER_FindOrAddListNode, ROSTER_FreeList, ROSTER_MoveEntryToFront and PARTY_BuildRowList.
void __cdecl ROSTER_PartySendLeaveParty()
{
    D2RosterUnitStrc *ptRoster;
    D2RosterUnitStrc *pWalk;
    D2RosterGroupStrc *pGroup;
    D2UnitStrc *pPlayer;
    D2UnitGUID dwLocalId;
    D2UnitGUID dwPlayerId;
    int16_t nMyPartyId;

    pPlayer = CUNIT_GetPlayer();
    dwLocalId = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;

    ptRoster = ROSTER_FindEntryByUnitOrCorpse(dwLocalId);
    if (!ptRoster)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "ptRoster", (uint32_t)(UINT_PTR)szRosterCpp, 1251);
        exit(-1);
    }
    ptRoster->eButton = PARTYBUTTON_NONE;
    ptRoster->wPartyId = 0xFFFF;

    pPlayer = CUNIT_GetPlayer();
    dwPlayerId = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;

    nMyPartyId = -1;
    pWalk = ROSTER_FindEntryByUnitOrCorpse(dwPlayerId);
    if (pWalk)
    {
        nMyPartyId = (int16_t)pWalk->wPartyId;
    }

    while (gpPartyEntries)
    {
        pGroup = ROSTER_FindOrAddListNode((int16_t)gpPartyEntries->wPartyId);
        ROSTER_PartyExtractPartyMembers(pGroup);
    }

    for (pGroup = gpRosterList; pGroup; pGroup = pGroup->pNext)
    {
        if (ARENA_IsArenaMode())
        {
            pGroup->pMembers = ROSTER_SortEntriesByKillCount(pGroup->pMembers);
        }
        else
        {
            pGroup->pMembers = ROSTER_SortEntriesByName(pGroup->pMembers);
        }
    }
    ROSTER_SortList();

    ROSTER_MoveListNodeAndEntryToFront(dwPlayerId, nMyPartyId);

    ROSTER_FlattenGroupsIntoEntryList();

    ROSTER_FreeList();
    if (nMyPartyId == -1)
    {
        ROSTER_MoveEntryToFront(dwPlayerId);
    }
    PARTY_BuildRowList();
    MSG_SendClt5E(PARTYCMD_LEAVEPARTY, ptRoster->dwUnitId);
}

//1.10f: D2Client.0x6FAB0540
//  The hostile box (PARTY_OnLButtonUp). It toggles only when the target is already hostile or is of level 9 or more -- `cmp word ptr [edi+20h],
//  9 / jb` (0x6FAB056E), unsigned. Then it is ROSTER_ToggleLootAndNotify's shape for flag 8 and packet 0x5D command 4, with one difference:
//  the flag sent, and the one tested after it, are read with the local GUID taken at the top (ebx, from the call at 0x6FAB0545), not with a
//  fresh one.
//  A player who has just been made hostile and shares the local player's party forces the local player out of it, locally -- both entries'
//  party ids compared as words (0x6FAB064F) -- by the same lookup, stores and rebuild that open ROSTER_PartySendLeaveParty, but without the 0x5E
//  packet.
//  Two asserting lookups of the local entry, at the original's source lines 114 (0x6FAB0630) and 1251 (0x6FAB069A): 1251 is the one shared
//  with ROSTER_PartySendLeaveParty, and 114 is a lookup that no other function references. The original inlines both, with rebuild steps 1, 3
//  and 5a; its step 1 calls ROSTER_EntryHasCorpse (0x6FAB06EC), and it calls ROSTER_FindOrAddListNode, ROSTER_MoveListNodeAndEntryToFront
//  (0x6FAB0758), ROSTER_FreeList, ROSTER_MoveEntryToFront and, as a tail jump, PARTY_BuildRowList (0x6FAB07C1).
void __fastcall ROSTER_PartySendHostileToggle(D2RosterUnitStrc *pEntry)
{
    D2RosterUnitStrc *ptRoster;
    D2RosterUnitStrc *pWalk;
    D2RosterGroupStrc *pGroup;
    D2UnitStrc *pPlayer;
    D2UnitGUID dwSelfId;
    D2UnitGUID dwToggleId;
    D2UnitGUID dwLocalId;
    D2UnitGUID dwPlayerId;
    int16_t nMyPartyId;
    int32_t bWasHostile;

    pPlayer = CUNIT_GetPlayer();
    dwSelfId = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;
    if (!pEntry)
    {
        return;
    }
    if (!PLAYERLIST_RosterTestInfoFlag(dwSelfId, pEntry->dwUnitId, ROSTERINFOFLAG_HOSTILE) && pEntry->wLevel < 9)
    {
        return;
    }

    pPlayer = CUNIT_GetPlayer();
    dwToggleId = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;
    PLAYERLIST_RosterGetOrAddInfoEntry(dwToggleId, pEntry->dwUnitId);
    bWasHostile = PLAYERLIST_RosterTestInfoFlag(dwToggleId, pEntry->dwUnitId, ROSTERINFOFLAG_HOSTILE);
    PLAYERLIST_RosterSetInfoFlag(dwToggleId, pEntry->dwUnitId, ROSTERINFOFLAG_HOSTILE, bWasHostile == 0);
    MSG_SendClt5D(PARTYCMD_TOGGLEHOSTILE, pEntry->dwUnitId, PLAYERLIST_RosterTestInfoFlag(dwSelfId, pEntry->dwUnitId, ROSTERINFOFLAG_HOSTILE));

    if (!PLAYERLIST_RosterTestInfoFlag(dwSelfId, pEntry->dwUnitId, ROSTERINFOFLAG_HOSTILE))
    {
        return;
    }
    if (pEntry->wPartyId == 0xFFFF)
    {
        return;
    }

    pPlayer = CUNIT_GetPlayer();
    dwLocalId = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;

    ptRoster = ROSTER_FindEntryByUnitOrCorpse(dwLocalId);
    if (!ptRoster)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "ptRoster", (uint32_t)(UINT_PTR)szRosterCpp, 114);
        exit(-1);
    }
    if (ptRoster->wPartyId != pEntry->wPartyId)
    {
        return;
    }

    pPlayer = CUNIT_GetPlayer();
    dwLocalId = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;

    ptRoster = ROSTER_FindEntryByUnitOrCorpse(dwLocalId);
    if (!ptRoster)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "ptRoster", (uint32_t)(UINT_PTR)szRosterCpp, 1251);
        exit(-1);
    }
    ptRoster->wPartyId = 0xFFFF;
    ptRoster->eButton = PARTYBUTTON_NONE;

    pPlayer = CUNIT_GetPlayer();
    dwPlayerId = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;

    nMyPartyId = -1;
    if (dwPlayerId != 0xFFFFFFFF)
    {
        for (pWalk = gpPartyEntries; pWalk; pWalk = pWalk->pNext)
        {
            if (pWalk->dwUnitId == dwPlayerId || ROSTER_EntryHasCorpse(pWalk, dwPlayerId))
            {
                nMyPartyId = (int16_t)pWalk->wPartyId;
                break;
            }
        }
    }

    while (gpPartyEntries)
    {
        pGroup = ROSTER_FindOrAddListNode((int16_t)gpPartyEntries->wPartyId);
        ROSTER_PartyExtractPartyMembers(pGroup);
    }

    for (pGroup = gpRosterList; pGroup; pGroup = pGroup->pNext)
    {
        if (ARENA_IsArenaMode())
        {
            pGroup->pMembers = ROSTER_SortEntriesByKillCount(pGroup->pMembers);
        }
        else
        {
            pGroup->pMembers = ROSTER_SortEntriesByName(pGroup->pMembers);
        }
    }
    ROSTER_SortList();
    ROSTER_MoveListNodeAndEntryToFront(dwPlayerId, nMyPartyId);

    ROSTER_FlattenGroupsIntoEntryList();

    ROSTER_FreeList();
    if (nMyPartyId == -1)
    {
        ROSTER_MoveEntryToFront(dwPlayerId);
    }
    PARTY_BuildRowList();
}

//1.10f: D2Client.0x6FAB07D0
//  Whether dwPlayerGUID's entry counts dwTargetGUID's as hostile: their party ids differ, or both are 0xFFFF (`cmp ax, [ecx+22h]` then
//  `cmp ax, 0FFFFh`, 0x6FAB0840, 0x6FAB0846), and the player's info list holds ROSTERINFOFLAG_HOSTILE for the target. One direction only. An
//  entry missing on either side answers TRUE: an unknown player counts as hostile (0x6FAB0869).
//  Each GUID is found by id or by corpse, and the flag is then asked with the two entries' ids (0x6FAB0851, 0x6FAB0854), not the GUIDs passed:
//  a corpse's GUID is answered for its owner. The answer is normalised to 0 or 1 (`neg / sbb / neg`, 0x6FAB085E..0x6FAB0864).
//  The original inlines ROSTER_FindEntryByUnitOrCorpse (twice) here.
BOOL __fastcall ROSTER_IsHostileTo(D2UnitGUID dwPlayerGUID, D2UnitGUID dwTargetGUID)
{
    D2RosterUnitStrc *pPlayerEntry;
    D2RosterUnitStrc *pTargetEntry;
    D2RosterCorpseStrc *pCorpse;

    pPlayerEntry = ROSTER_FindEntryByUnitOrCorpse(dwPlayerGUID);
    pTargetEntry = NULL;
    if (dwTargetGUID != 0xFFFFFFFF)
    {
        for (pTargetEntry = gpPartyEntries; pTargetEntry; pTargetEntry = pTargetEntry->pNext)
        {
            if (pTargetEntry->dwUnitId == dwTargetGUID)
            {
                break;
            }
            for (pCorpse = pTargetEntry->pCorpse; pCorpse; pCorpse = pCorpse->pPrev)
            {
                if (pCorpse->dwCorpseId == dwTargetGUID)
                {
                    break;
                }
            }
            if (pCorpse)
            {
                break;
            }
        }
    }

    if (!pPlayerEntry || !pTargetEntry)
    {
        return TRUE;
    }
    if (pPlayerEntry->wPartyId == pTargetEntry->wPartyId && pPlayerEntry->wPartyId != 0xFFFF)
    {
        return FALSE;
    }
    return PLAYERLIST_RosterTestInfoFlag(pPlayerEntry->dwUnitId, pTargetEntry->dwUnitId, ROSTERINFOFLAG_HOSTILE) != 0;
}

//1.10f: D2Client.0x6FAB0870
//  An entry's GUID (+0x10), or -1 for NULL (`or eax, 0FFFFFFFFh`, 0x6FAB0878).
int32_t __fastcall ROSTER_GetEntryUnitId(D2RosterUnitStrc *pRosterEntry)
{
    if (pRosterEntry)
    {
        return pRosterEntry->dwUnitId;
    }
    return -1;
}

//  The entry fields used from here on.
static_assert(offsetof(D2RosterUnitStrc, dwPartyLife) == 0x14
                  && offsetof(D2RosterUnitStrc, dwLevelId) == 0x24
                  && offsetof(D2RosterUnitStrc, dwPosX) == 0x28
                  && offsetof(D2RosterUnitStrc, dwPosY) == 0x2C,
              "ROSTER_UpdateLifeAndLevel: `mov [esi+14h], edx`, `mov [esi+24h], eax` "
              "(0x6FAB08C9, 0x6FAB08CC); ROSTER_SetEntryPosition: `mov [esi+28h], edx`, "
              "`mov [esi+2Ch], eax` (0x6FAB0AC9, 0x6FAB0ACC)");
static_assert(offsetof(D2RosterUnitStrc, dwSrcPortalGUID) == 0x3C
                  && offsetof(D2RosterUnitStrc, dwDstPortalGUID) == 0x40,
              "ROSTER_SetEntryPortalGuids: `mov [esi+3Ch], edx`, `mov [esi+40h], eax` "
              "(0x6FAB0CE9, 0x6FAB0CEC); ROSTER_GetNameExByPortalGuid: `cmp [eax+3Ch], ecx`, "
              "`cmp [eax+40h], ecx` (0x6FAB0D09, 0x6FAB0D0E)");

//1.10f: D2Client.0x6FAB0880
//  Packet 0x7F: a party member's life and area. Its caller is SCMD_HandleSrv7F_PartyLifeUpdate. The entry is found by the GUID or by one of its
//  corpses; a miss returns before anything is written (0x6FAB08BA). dwPartyLife takes nLifePercentage and dwLevelId takes the word nLevelId,
//  zero-extended (`and eax, 0FFFFh`, 0x6FAB08C4). Then the whole roster is rebuilt. `retn 4` purges nLevelId, the one stack argument. The original
//  inlines ROSTER_FindEntryByUnitOrCorpse and rebuild steps 1, 2 (ROSTER_FindOrAddListNode), 3, 4 and 5a here, and calls ROSTER_FreeList,
//  ROSTER_MoveEntryToFront and PARTY_BuildRowList.
void __fastcall ROSTER_UpdateLifeAndLevel(int32_t dwUnitGUID, int32_t nLifePercentage, uint16_t nLevelId)
{
    D2RosterUnitStrc *pEntry;
    D2RosterUnitStrc *pWalk;
    D2RosterGroupStrc *pGroup;
    D2UnitStrc *pPlayer;
    D2UnitGUID dwPlayerId;
    int16_t nMyPartyId;
    int16_t nGroupId;

    pEntry = ROSTER_FindEntryByUnitOrCorpse((D2UnitGUID)dwUnitGUID);
    if (!pEntry)
    {
        return;
    }
    pEntry->dwPartyLife = nLifePercentage;
    pEntry->dwLevelId = nLevelId;

    pPlayer = CUNIT_GetPlayer();
    dwPlayerId = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;

    nMyPartyId = -1;
    pWalk = ROSTER_FindEntryByUnitOrCorpse(dwPlayerId);
    if (pWalk)
    {
        nMyPartyId = (int16_t)pWalk->wPartyId;
    }

    while (gpPartyEntries)
    {
        nGroupId = (int16_t)gpPartyEntries->wPartyId;
        pGroup = ROSTER_FindListNode(nGroupId);
        if (!pGroup)
        {
            pGroup = (D2RosterGroupStrc *)(UINT_PTR)FOG_Alloc(sizeof(D2RosterGroupStrc), (uint32_t)(UINT_PTR)szRosterCpp, 505, 0);
            memset(pGroup, 0, sizeof(D2RosterGroupStrc));
            pGroup->wPartyId = nGroupId;
            pGroup->pNext = gpRosterList;
            gpRosterList = pGroup;
        }
        ROSTER_PartyExtractPartyMembers(pGroup);
    }

    for (pGroup = gpRosterList; pGroup; pGroup = pGroup->pNext)
    {
        if (ARENA_IsArenaMode())
        {
            pGroup->pMembers = ROSTER_SortEntriesByKillCount(pGroup->pMembers);
        }
        else
        {
            pGroup->pMembers = ROSTER_SortEntriesByName(pGroup->pMembers);
        }
    }
    ROSTER_SortList();

    ROSTER_MoveListNodeAndEntryToFront(dwPlayerId, nMyPartyId);

    ROSTER_FlattenGroupsIntoEntryList();

    ROSTER_FreeList();
    if (nMyPartyId == -1)
    {
        ROSTER_MoveEntryToFront(dwPlayerId);
    }
    PARTY_BuildRowList();
}

//1.10f: D2Client.0x6FAB0A90
//  Packet 0x90: a party member's position, into dwPosX and dwPosY; AUTOMAP_DrawPartyMember reads the pair back through
//  ROSTER_GetEntryPosition. The entry is found by the GUID or a corpse's; a miss writes nothing. `retn 4` purges nY.
//  The original inlines ROSTER_FindEntryByUnitOrCorpse here.
void __fastcall ROSTER_SetEntryPosition(uint32_t dwUnitId, int32_t nX, int32_t nY)
{
    D2RosterUnitStrc *pEntry;

    pEntry = ROSTER_FindEntryByUnitOrCorpse(dwUnitId);
    if (!pEntry)
    {
        return;
    }
    pEntry->dwPosX = nX;
    pEntry->dwPosY = nY;
}

//1.10f: D2Client.0x6FAB0AE0
//  dwPartyLife for a GUID or a corpse's, 0 when there is no entry (0x6FAB0B10). The original inlines ROSTER_FindEntryByUnitOrCorpse here.
int32_t __fastcall ROSTER_GetPartyLife(uint32_t dwUnitId)
{
    D2RosterUnitStrc *pEntry;

    pEntry = ROSTER_FindEntryByUnitOrCorpse(dwUnitId);
    if (!pEntry)
    {
        return 0;
    }
    return pEntry->dwPartyLife;
}

//1.10f: D2Client.0x6FAB0B20
//  Whether the local player has not squelched this GUID: TRUE when its entry is missing (0x6FAB0B51) -- a GUID of -1, an empty roster, a
//  stranger -- and otherwise `setz` on PLAYERLIST_RosterTestInfoFlag's mask for ROSTERINFOFLAG_SQUELCH (0x6FAB0B77). The flag is asked with the
//  entry's id (0x6FAB0B69), so a corpse's GUID answers for its owner. With no local player the GUID is -1, which has no info list: the answer
//  is then TRUE. Its caller TEXT_HandleChatMessage drops a player's chat message when this is FALSE.
//  The original inlines ROSTER_FindEntryByUnitOrCorpse here.
BOOL __fastcall ROSTER_IsUnitNotSquelched(D2UnitGUID dwUnitId)
{
    D2RosterUnitStrc *pEntry;
    D2UnitStrc *pPlayer;

    pEntry = ROSTER_FindEntryByUnitOrCorpse(dwUnitId);
    if (!pEntry)
    {
        return TRUE;
    }
    pPlayer = CUNIT_GetPlayer();
    return PLAYERLIST_RosterTestInfoFlag(pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF, pEntry->dwUnitId, ROSTERINFOFLAG_SQUELCH) == 0;
}

//1.10f: D2Client.0x6FAB0B80
//  An entry's dwPosX and dwPosY into *pnX and *pnY, or 0 and 0 for NULL. `retn 4` purges pnY. void: eax holds dwPosX on one path (0x6FAB0B84)
//  and pnY on the other (0x6FAB0B95), and the caller reads neither.
void __fastcall ROSTER_GetEntryPosition(D2RosterUnitStrc *pRosterEntry, int32_t *pnX, int32_t *pnY)
{
    if (pRosterEntry)
    {
        *pnX = pRosterEntry->dwPosX;
        *pnY = pRosterEntry->dwPosY;
        return;
    }
    *pnX = 0;
    *pnY = 0;
}

//1.10f: D2Client.0x6FAB0BB0
//  An entry's dwLevelId, or 0 for NULL (0x6FAB0BB8).
int32_t __fastcall ROSTER_GetEntryLevelId(D2RosterUnitStrc *pRosterEntry)
{
    if (pRosterEntry)
    {
        return pRosterEntry->dwLevelId;
    }
    return 0;
}

//1.10f: D2Client.0x6FAB0BC0
//  A GUID's (or a corpse's) szNameEx, `lea eax, [edx+66h]` (0x6FAB0BF3), or NULL when there is no entry.
//  The original inlines ROSTER_FindEntryByUnitOrCorpse here.
char *__fastcall ROSTER_GetNameEx(uint32_t dwUnitId)
{
    D2RosterUnitStrc *pEntry;

    pEntry = ROSTER_FindEntryByUnitOrCorpse(dwUnitId);
    if (!pEntry)
    {
        return NULL;
    }
    return pEntry->szNameEx;
}

//1.10f: D2Client.0x6FAB0C00
//  A player's display name, not an entry: the roster's szNameEx when the unit (or a corpse of it) has an entry, else the player data's szName,
//  which is the D2PlayerDataStrc pointer itself (szName at offset 0). Three asserts (source lines 1489, 1490 and 1499; `push 5D1h`, `5D2h`,
//  `5DBh`): the unit, that it is a player (`cmp dword ptr [ecx], 0`, 0x6FAB0C23), and the player data.
//  The original inlines ROSTER_FindEntryByUnitOrCorpse here.
char *__fastcall ROSTER_GetPlayerDisplayName(D2UnitStrc *pUnit)
{
    D2RosterUnitStrc *pEntry;
    D2PlayerDataStrc *pPlayerData;
    D2UnitGUID dwUnitId;

    if (!pUnit)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "hPlayer", (uint32_t)(UINT_PTR)szRosterCpp, 1489);
        exit(-1);
    }
    if (pUnit->dwUnitType != UNIT_PLAYER)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "UnitGetType (hPlayer) == UNIT_PLAYER", (uint32_t)(UINT_PTR)szRosterCpp, 1490);
        exit(-1);
    }
    dwUnitId = pUnit->dwUnitId;

    pEntry = ROSTER_FindEntryByUnitOrCorpse(dwUnitId);
    if (pEntry)
    {
        return pEntry->szNameEx;
    }

    pPlayerData = D2COMMON_UNITS_GetPlayerData(pUnit);
    if (!pPlayerData)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "ptPlayerInfo", (uint32_t)(UINT_PTR)szRosterCpp, 1499);
        exit(-1);
    }
    return pPlayerData->szName;
}

//1.10f: D2Client.0x6FAB0CB0
//  Packet 0x82: the two GUIDs of a player's town portal, into dwSrcPortalGUID and dwDstPortalGUID. Its caller is SCMD_HandleSrv82_PortalOwner. The
//  entry is found by the owner's GUID or a corpse's; a miss writes nothing. `retn 4` purges dwDstPortalGUID. The original inlines
//  ROSTER_FindEntryByUnitOrCorpse here.
void __fastcall ROSTER_SetEntryPortalGuids(uint32_t dwUnitId, uint32_t dwSrcPortalGUID, uint32_t dwDstPortalGUID)
{
    D2RosterUnitStrc *pEntry;

    pEntry = ROSTER_FindEntryByUnitOrCorpse(dwUnitId);
    if (!pEntry)
    {
        return;
    }
    pEntry->dwSrcPortalGUID = dwSrcPortalGUID;
    pEntry->dwDstPortalGUID = dwDstPortalGUID;
}

//1.10f: D2Client.0x6FAB0D00
//  The owner's szNameEx for a portal GUID, matched at either end -- +0x3C, then +0x40 (0x6FAB0D09, 0x6FAB0D0E) -- or NULL. The one walk here
//  that is not the unit-or-corpse one, and the only one with no -1 test.
char *__fastcall ROSTER_GetNameExByPortalGuid(uint32_t dwPortalGUID)
{
    D2RosterUnitStrc *pEntry;

    for (pEntry = gpPartyEntries; pEntry; pEntry = pEntry->pNext)
    {
        if (pEntry->dwSrcPortalGUID == dwPortalGUID || pEntry->dwDstPortalGUID == dwPortalGUID)
        {
            return pEntry->szNameEx;
        }
    }
    return NULL;
}

//1.10f: D2Client.0x6FAB0D30
//  Whether another entry shares the unit's party: same party id, compared as words (`cmp [esi+22h], ax`, 0x6FAB0D53), and a different GUID
//  (0x6FAB0D59). A party id of -1 answers FALSE (0x6FAB0D49), and so does a NULL unit: ROSTER_GetPartyIdForUnit answers -1 for it before its
//  tail jump to ROSTER_FindPartyIdByUnit. The list head is read before that call (0x6FAB0D31), and the C keeps the order. The result is 0 or 1.
BOOL __fastcall ROSTER_HasOtherPartyMember(D2UnitStrc *pUnit)
{
    D2RosterUnitStrc *pEntry;
    D2UnitGUID dwUnitId;
    int16_t nPartyId;

    pEntry = gpPartyEntries;
    dwUnitId = pUnit ? pUnit->dwUnitId : 0xFFFFFFFF;
    nPartyId = ROSTER_GetPartyIdForUnit(pUnit);
    if (nPartyId == -1)
    {
        return FALSE;
    }
    for (; pEntry; pEntry = pEntry->pNext)
    {
        if ((int16_t)pEntry->wPartyId == nPartyId && pEntry->dwUnitId != dwUnitId)
        {
            return TRUE;
        }
    }
    return FALSE;
}

//1.10f: D2Client.0x6FAB0D80
//  A GUID's (or a corpse's) pRosterInfo, the list of info flags that player holds on the others, or NULL (0x6FAB0DB0). Its GUID test at
//  0x6FAB0D89 is what makes PLAYERLIST_RosterTestInfoFlag answer 0 for a GUID of -1. The original inlines ROSTER_FindEntryByUnitOrCorpse here.
D2RosterInfoStrc **__fastcall ROSTER_GetInfoListByUnitOrCorpse(uint32_t dwUnitId)
{
    D2RosterUnitStrc *pEntry;

    pEntry = ROSTER_FindEntryByUnitOrCorpse(dwUnitId);
    if (!pEntry)
    {
        return NULL;
    }
    return pEntry->pRosterInfo;
}

//1.10f: D2Client.0x6FAB0DC0
//  Packet 0x8D: a player's party. Its caller is SCMD_HandleSrv8D_SetPartyId. The entry is found by the GUID or by one of its corpses; a miss returns
//  before anything is written (0x6FAB0DF2). The found entry takes the word nPartyGUID (`mov [esi+22h], dx`, 0x6FAB0DF6) before step 1 looks up the
//  local player, so when the entry is the local player's own, the rebuild sorts by the new party. Both arguments are in registers and the `retn` is
//  bare. The rebuild is ROSTER_UpdateLifeAndLevel's, step for step (the original inlines ROSTER_FindEntryByUnitOrCorpse and steps 1, 2
//  (ROSTER_FindOrAddListNode), 3, 4 and 5a, and calls ROSTER_FreeList and ROSTER_MoveEntryToFront), except that PARTY_BuildRowList is a tail jump
//  here (0x6FAB0FAB).
void __fastcall ROSTER_SetPartyId(int32_t dwPlayerGUID, int16_t nPartyGUID)
{
    D2RosterUnitStrc *pEntry;
    D2RosterUnitStrc *pWalk;
    D2RosterGroupStrc *pGroup;
    D2UnitStrc *pPlayer;
    D2UnitGUID dwPlayerId;
    int16_t nMyPartyId;
    int16_t nGroupId;

    pEntry = ROSTER_FindEntryByUnitOrCorpse((D2UnitGUID)dwPlayerGUID);
    if (!pEntry)
    {
        return;
    }
    pEntry->wPartyId = nPartyGUID;

    pPlayer = CUNIT_GetPlayer();
    dwPlayerId = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;

    nMyPartyId = -1;
    pWalk = ROSTER_FindEntryByUnitOrCorpse(dwPlayerId);
    if (pWalk)
    {
        nMyPartyId = (int16_t)pWalk->wPartyId;
    }

    while (gpPartyEntries)
    {
        nGroupId = (int16_t)gpPartyEntries->wPartyId;
        pGroup = ROSTER_FindListNode(nGroupId);
        if (!pGroup)
        {
            pGroup = (D2RosterGroupStrc *)(UINT_PTR)FOG_Alloc(sizeof(D2RosterGroupStrc), (uint32_t)(UINT_PTR)szRosterCpp, 505, 0);
            memset(pGroup, 0, sizeof(D2RosterGroupStrc));
            pGroup->wPartyId = nGroupId;
            pGroup->pNext = gpRosterList;
            gpRosterList = pGroup;
        }
        ROSTER_PartyExtractPartyMembers(pGroup);
    }

    for (pGroup = gpRosterList; pGroup; pGroup = pGroup->pNext)
    {
        if (ARENA_IsArenaMode())
        {
            pGroup->pMembers = ROSTER_SortEntriesByKillCount(pGroup->pMembers);
        }
        else
        {
            pGroup->pMembers = ROSTER_SortEntriesByName(pGroup->pMembers);
        }
    }
    ROSTER_SortList();

    ROSTER_MoveListNodeAndEntryToFront(dwPlayerId, nMyPartyId);

    ROSTER_FlattenGroupsIntoEntryList();

    ROSTER_FreeList();
    if (nMyPartyId == -1)
    {
        ROSTER_MoveEntryToFront(dwPlayerId);
    }
    PARTY_BuildRowList();
}

//1.10f: D2Client.0x6FAB0FB0
//  Packet 0x8B: a player's party button. Its caller SCMD_HandleSrv8B_SetPartyButton reaches it by a tail jump (0x6FAB4376). The byte is zero-extended
//  into eButton (`and edx, 0FFh`, 0x6FAB0FE3), and the party panel's rows are rebuilt by a tail jump to PARTY_BuildRowList (0x6FAB0FED); nothing is
//  re-sorted. A miss does neither. The original inlines ROSTER_FindEntryByUnitOrCorpse here.
void __fastcall ROSTER_SetPartyButton(int32_t nUnitId, uint8_t eButton)
{
    D2RosterUnitStrc *pEntry;

    pEntry = ROSTER_FindEntryByUnitOrCorpse((D2UnitGUID)nUnitId);
    if (!pEntry)
    {
        return;
    }
    pEntry->eButton = (D2C_PartyButtons)eButton;
    PARTY_BuildRowList();
}

//1.10f: D2Client.0x6FAB1000
//  Packet 0x8C: the info flags dwPlayerGUID holds on dwTargetGUID, replaced by nFlags. Its caller is SCMD_HandleSrv8C_PlayerRelation. The lookup only
//  checks that dwPlayerGUID (or a corpse of it) is on the roster; the entry found is not used, and a miss does nothing (0x6FAB1036). Then
//  PLAYERLIST_RosterGetOrAddInfoEntry makes the record if it is missing, PLAYERLIST_RosterReplaceInfoFlags writes the word nFlags, zero-extended
//  (`and eax, 0FFFFh`, 0x6FAB104A), and the rows are rebuilt -- before the last step: when the target is the local player,
//  ROSTERPETS_UpdateRaisedUndead runs for dwPlayerGUID (0x6FAB1073). `retn 4` purges nFlags. The original inlines ROSTER_FindEntryByUnitOrCorpse
//  here.
void __fastcall ROSTER_ApplyInfoFlags(D2UnitGUID dwPlayerGUID, D2UnitGUID dwTargetGUID, uint16_t nFlags)
{
    D2RosterUnitStrc *pEntry;
    D2UnitStrc *pPlayer;

    pEntry = ROSTER_FindEntryByUnitOrCorpse(dwPlayerGUID);
    if (!pEntry)
    {
        return;
    }

    PLAYERLIST_RosterGetOrAddInfoEntry(dwPlayerGUID, dwTargetGUID);
    PLAYERLIST_RosterReplaceInfoFlags(dwPlayerGUID, dwTargetGUID, nFlags);
    PARTY_BuildRowList();

    pPlayer = CUNIT_GetPlayer();
    if (dwTargetGUID == (pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF))
    {
        ROSTERPETS_UpdateRaisedUndead(dwPlayerGUID);
    }
}

//1.10f: D2Client.0x6FAB1080
//  Packet 0x8E, type 1: a corpse for a player. Its caller SCMD_HandleSrv8E_RosterOnCorpseAssignment jumps here for a non-zero type. The
//  player's entry is found by its GUID or a corpse's on gpPartyEntries; when there is none, a new entry is made -- FOG_Alloc of 132 bytes,
//  asserted `ptRoster`, cleared by `rep stosd` of 0x21 dwords (0x6FAB10F6..0x6FAB10FF), given the GUID -- and put on gpPendingRosterEntries
//  (0x6FAB1104..0x6FAB110F), where ROSTER_AddPlayer takes it back when packet 0x5B announces the player. Then, unless the entry already carries
//  the corpse, a corpse record -- FOG_Alloc of 8, asserted `ptBody` -- is put at the head of its chain.
//  BUG: the pending list is not searched: the lookup walks gpPartyEntries only (0x6FAB1096..0x6FAB10B5), so a second corpse for a player who
//  is still pending makes a second pending entry, and ROSTER_AddPlayer takes the first it finds, the newest, since the pending list is
//  head-inserted. The older one, with its corpse, stays pending until ROSTER_FreeAll. LATENT, harmless if hit: a save holds at most one corpse,
//  and no lookup reads the pending list. Reproduced.
//  The original inlines ROSTER_FindEntryByUnitOrCorpse and ROSTER_EntryHasCorpse here.
void __fastcall ROSTER_FindCorpseEntry(uint32_t dwUnitId, uint32_t dwCorpseId)
{
    D2RosterUnitStrc *pEntry;
    D2RosterCorpseStrc *pCorpse;

    pEntry = ROSTER_FindEntryByUnitOrCorpse(dwUnitId);
    if (!pEntry)
    {
        pEntry = (D2RosterUnitStrc *)(UINT_PTR)FOG_Alloc(sizeof(D2RosterUnitStrc), (uint32_t)(UINT_PTR)szRosterCpp, 1660, 0);
        if (!pEntry)
        {
            FOG_DisplayAssert((uint32_t)(UINT_PTR) "ptRoster", (uint32_t)(UINT_PTR)szRosterCpp, 1661);
            exit(-1);
        }
        memset(pEntry, 0, sizeof(D2RosterUnitStrc));
        pEntry->dwUnitId = dwUnitId;
        pEntry->pNext = gpPendingRosterEntries;
        gpPendingRosterEntries = pEntry;
    }

    if (ROSTER_EntryHasCorpse(pEntry, dwCorpseId))
    {
        return;
    }

    pCorpse = (D2RosterCorpseStrc *)(UINT_PTR)FOG_Alloc(sizeof(D2RosterCorpseStrc), (uint32_t)(UINT_PTR)szRosterCpp, 1677, 0);
    if (!pCorpse)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR) "ptBody", (uint32_t)(UINT_PTR)szRosterCpp, 1678);
        exit(-1);
    }
    memset(pCorpse, 0, sizeof(D2RosterCorpseStrc));
    pCorpse->dwCorpseId = dwCorpseId;
    pCorpse->pPrev = pEntry->pCorpse;
    pEntry->pCorpse = pCorpse;
}

//1.10f: D2Client.0x6FAB1180
//  Packet 0x8E, type 0: a corpse unlinked from its owner's chain and freed (FOG_Free). SCMD_HandleSrv8E_RosterOnCorpseAssignment jumps here
//  for type 0. A missing entry or a missing corpse does nothing. The owner is looked up on gpPartyEntries only, so a corpse held by a pending
//  entry stays. The original inlines ROSTER_FindEntryByUnitOrCorpse here.
void __fastcall ROSTER_RemoveCorpseFromEntry(int32_t nUnitId, int32_t nCorpseId)
{
    D2RosterUnitStrc *pEntry;
    D2RosterCorpseStrc *pCorpse;
    D2RosterCorpseStrc *pPrevCorpse;

    pPrevCorpse = NULL;

    pEntry = ROSTER_FindEntryByUnitOrCorpse((D2UnitGUID)nUnitId);
    if (!pEntry)
    {
        return;
    }

    for (pCorpse = pEntry->pCorpse; pCorpse; pCorpse = pCorpse->pPrev)
    {
        if (pCorpse->dwCorpseId == (D2UnitGUID)nCorpseId)
        {
            break;
        }
        pPrevCorpse = pCorpse;
    }
    if (!pCorpse)
    {
        return;
    }
    if (pPrevCorpse)
    {
        pPrevCorpse->pPrev = pCorpse->pPrev;
    }
    else
    {
        pEntry->pCorpse = pCorpse->pPrev;
    }
    FOG_Free((uint32_t)(UINT_PTR)pCorpse, (uint32_t)(UINT_PTR)szRosterCpp, 1715, 0);
}

//  The functions from here to the end of the unit (0x6FAB1200..0x6FAB14BD) carry no assert and no source line, so nothing in them names a file;
//  they belong to this unit by the globals they share.

//1.10f: D2Client.0x6FAB1200
//  Whether dwLooterGUID may loot what dwOwnerGUID owns: FALSE when the owner (or a corpse of it) has no entry (0x6FAB1231), TRUE when the looter
//  is the entry found -- compared against the entry's own GUID (0x6FAB1235), so a looter asking about their own corpse gets TRUE -- and
//  otherwise the LOOT flag the owner holds on the looter, normalised to 1. The original inlines ROSTER_FindEntryByUnitOrCorpse here.
BOOL __fastcall ROSTER_CanUnitLootFrom(D2UnitGUID dwLooterGUID, D2UnitGUID dwOwnerGUID)
{
    D2RosterUnitStrc *pEntry;

    pEntry = ROSTER_FindEntryByUnitOrCorpse(dwOwnerGUID);
    if (!pEntry)
    {
        return FALSE;
    }
    if (dwLooterGUID == pEntry->dwUnitId)
    {
        return TRUE;
    }
    if (PLAYERLIST_RosterTestInfoFlag(pEntry->dwUnitId, dwLooterGUID, ROSTERINFOFLAG_LOOT))
    {
        return TRUE;
    }
    return FALSE;
}

//1.10f: D2Client.0x6FAB1260
//  Whether two GUIDs (or corpses) have entries in one party: their party ids equal as words (`cmp ax, [ecx+22h]`, 0x6FAB12D0) and not 0xFFFF
//  (0x6FAB12D6). A missing entry on either side answers FALSE, the opposite of ROSTER_IsHostileTo, whose two walks are the same.
//  The original inlines ROSTER_FindEntryByUnitOrCorpse (twice) here.
BOOL __fastcall ROSTER_UnitsShareParty(D2UnitGUID dwUnitIdA, D2UnitGUID dwUnitIdB)
{
    D2RosterUnitStrc *pEntryA;
    D2RosterUnitStrc *pEntryB;
    D2RosterCorpseStrc *pCorpse;

    pEntryA = ROSTER_FindEntryByUnitOrCorpse(dwUnitIdA);
    pEntryB = NULL;
    if (dwUnitIdB != 0xFFFFFFFF)
    {
        for (pEntryB = gpPartyEntries; pEntryB; pEntryB = pEntryB->pNext)
        {
            if (pEntryB->dwUnitId == dwUnitIdB)
            {
                break;
            }
            for (pCorpse = pEntryB->pCorpse; pCorpse; pCorpse = pCorpse->pPrev)
            {
                if (pCorpse->dwCorpseId == dwUnitIdB)
                {
                    break;
                }
            }
            if (pCorpse)
            {
                break;
            }
        }
    }

    if (pEntryA && pEntryB && pEntryA->wPartyId == pEntryB->wPartyId && pEntryA->wPartyId != 0xFFFF)
    {
        return TRUE;
    }
    return FALSE;
}

//1.10f: D2Client.0x6FAB12F0
//  Whether any entry holds ROSTERINFOFLAG_HOSTILE on the local player: the entry's flag about the player, not the player's about it. The list
//  head is read before the one call to CUNIT_GetPlayer (0x6FAB12F1), and the C keeps the order. With no local player the GUID is -1 and
//  nothing answers.
BOOL __cdecl ROSTER_IsAnyoneHostileToPlayer()
{
    D2RosterUnitStrc *pEntry;
    D2UnitStrc *pPlayer;
    D2UnitGUID dwPlayerId;

    pEntry = gpPartyEntries;
    pPlayer = CUNIT_GetPlayer();
    dwPlayerId = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;
    for (; pEntry; pEntry = pEntry->pNext)
    {
        if (PLAYERLIST_RosterTestInfoFlag(pEntry->dwUnitId, dwPlayerId, ROSTERINFOFLAG_HOSTILE))
        {
            return TRUE;
        }
    }
    return FALSE;
}

//1.10f: D2Client.0x6FAB1340
//  The four directed predicates (0x6FAB1340..0x6FAB13FD). Each calls CUNIT_GetPlayer first (0x6FAB1343 here) and only then tests the entry and
//  the player, answering FALSE for either NULL; the flag is normalised to 1. PLAYERLIST_RosterTestInfoFlag's first GUID is the list's owner and
//  its second the subject:
//      ROSTER_EntryIsHostileToPlayer   entry's list, the player    HOSTILE
//      ROSTER_PlayerIsHostileToEntry   player's list, the entry    HOSTILE
//      ROSTER_EntryAllowsPlayerLoot    entry's list, the player    LOOT
//      ROSTER_PlayerAllowsEntryLoot    player's list, the entry    LOOT
//  The second and fourth ask what ROSTER_EntryIsHostile and ROSTER_EntryAllowsLoot ask; those two ask with a GUID of -1 when there is no
//  player, these four do not ask at all, and PLAYERLIST_RosterTestInfoFlag answers 0 for -1 either way.
BOOL __fastcall ROSTER_EntryIsHostileToPlayer(D2RosterUnitStrc *pEntry)
{
    D2UnitStrc *pPlayer;

    pPlayer = CUNIT_GetPlayer();
    if (!pEntry || !pPlayer)
    {
        return FALSE;
    }
    if (PLAYERLIST_RosterTestInfoFlag(pEntry->dwUnitId, pPlayer->dwUnitId, ROSTERINFOFLAG_HOSTILE))
    {
        return TRUE;
    }
    return FALSE;
}

//1.10f: D2Client.0x6FAB1370
//  The player's HOSTILE flag on the entry, one of the four directed predicates.
BOOL __fastcall ROSTER_PlayerIsHostileToEntry(D2RosterUnitStrc *pEntry)
{
    D2UnitStrc *pPlayer;

    pPlayer = CUNIT_GetPlayer();
    if (!pEntry || !pPlayer)
    {
        return FALSE;
    }
    if (PLAYERLIST_RosterTestInfoFlag(pPlayer->dwUnitId, pEntry->dwUnitId, ROSTERINFOFLAG_HOSTILE))
    {
        return TRUE;
    }
    return FALSE;
}

//1.10f: D2Client.0x6FAB13A0
//  The entry's LOOT flag on the player, one of the four directed predicates.
BOOL __fastcall ROSTER_EntryAllowsPlayerLoot(D2RosterUnitStrc *pEntry)
{
    D2UnitStrc *pPlayer;

    pPlayer = CUNIT_GetPlayer();
    if (!pEntry || !pPlayer)
    {
        return FALSE;
    }
    if (PLAYERLIST_RosterTestInfoFlag(pEntry->dwUnitId, pPlayer->dwUnitId, ROSTERINFOFLAG_LOOT))
    {
        return TRUE;
    }
    return FALSE;
}

//1.10f: D2Client.0x6FAB13D0
//  The player's LOOT flag on the entry, one of the four directed predicates.
BOOL __fastcall ROSTER_PlayerAllowsEntryLoot(D2RosterUnitStrc *pEntry)
{
    D2UnitStrc *pPlayer;

    pPlayer = CUNIT_GetPlayer();
    if (!pEntry || !pPlayer)
    {
        return FALSE;
    }
    if (PLAYERLIST_RosterTestInfoFlag(pPlayer->dwUnitId, pEntry->dwUnitId, ROSTERINFOFLAG_LOOT))
    {
        return TRUE;
    }
    return FALSE;
}

//1.10f: D2Client.0x6FAB1400
//  FALSE for the amazon, the sorceress and the assassin, TRUE for every other class, for a NULL entry and for a negative class id. The
//  compares are signed -- `test eax, eax / jl` (0x6FAB1409) and `cmp eax, 1 / jle` (0x6FAB140E) -- on the unsigned dwClassId, so the C reads
//  it as an int. The result is 0 or 1.
BOOL __fastcall ROSTER_EntryIsMale(D2RosterUnitStrc *pRosterEntry)
{
    int32_t nClassId;

    if (!pRosterEntry)
    {
        return TRUE;
    }
    nClassId = (int32_t)pRosterEntry->dwClassId;
    if (nClassId < 0)
    {
        return TRUE;
    }
    if (nClassId <= PCLASS_SORCERESS || nClassId == PCLASS_ASSASSIN)
    {
        return FALSE;
    }
    return TRUE;
}

//1.10f: D2Client.0x6FAB1420
//  dwPartyLife from a byte, zero-extended (`and edx, 0FFh`, 0x6FAB1453), for a GUID or a corpse's; a miss writes nothing. Callers:
//  SCMD_HandleSrvAB_UnitHealth, which passes `100 * nLife / 128`, and SCMD_HandleSrv0D_ActionAtXY.
//  The original inlines ROSTER_FindEntryByUnitOrCorpse here.
void __fastcall ROSTER_SetPartyLife(uint32_t dwUnitId, uint8_t nLife)
{
    D2RosterUnitStrc *pEntry;

    pEntry = ROSTER_FindEntryByUnitOrCorpse(dwUnitId);
    if (!pEntry)
    {
        return;
    }
    pEntry->dwPartyLife = nLife;
}

//1.10f: D2Client.0x6FAB1460
//  ROSTER_GetPartyLife, except that the local player's own GUID answers 0: the lookup first, then CUNIT_GetPlayer (0x6FAB1496), then
//  `cmp esi, eax` (0x6FAB14AB). A corpse's GUID answers its owner's life; callers compute `(life << 7) / 100`.
//  The original inlines ROSTER_FindEntryByUnitOrCorpse here.
int32_t __fastcall ROSTER_GetOtherPlayerPartyLife(uint32_t dwUnitId)
{
    D2RosterUnitStrc *pEntry;
    D2UnitStrc *pPlayer;

    pEntry = ROSTER_FindEntryByUnitOrCorpse(dwUnitId);

    pPlayer = CUNIT_GetPlayer();
    if (dwUnitId == (pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF) || !pEntry)
    {
        return 0;
    }
    return pEntry->dwPartyLife;
}

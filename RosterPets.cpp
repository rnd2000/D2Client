#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "RosterPets.h"

//  RosterPets.cpp -- the client's list of pets in D2Client.dll 1.10: every summon and hireling the server has announced, the local player's
//  and other players', one 52-byte D2RosterPetStrc each on gpRosterPetList, a new record pushed on the head. Packets 0x7A and 0x81 add,
//  update and remove records, 0x7F updates a pet's life; CUNIT_CreateMonsterUnit consults the list when a monster unit arrives, and the
//  automap, the party panel, the hireling icons and the palette shifts ask it who owns a monster.
//  The original source file is GAME\RosterPets.cpp (its asserts name it). Address range 0x6FAB14C0..0x6FAB1CA2, 17 functions.

//  The source file name, as every RosterPets.cpp call hands it to Fog.
static const char szRosterPetsCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\GAME\\RosterPets.cpp";

//  The record members this unit reads and writes, at the offsets its instructions use.
static_assert(offsetof(D2RosterPetStrc, nMonStatsId) == 0
              && offsetof(D2RosterPetStrc, nPetTypeId) == 4,
              "the eviction's `cmp dword ptr [ecx], 7` reads +0; the pet type is "
              "stored at +4 (`mov [edx+4], ebp`) and tested there (`cmp dword ptr [esi+4], 7`)");
static_assert(offsetof(D2RosterPetStrc, dwUnitId) == 8
              && offsetof(D2RosterPetStrc, dwOwnerId) == 0xC
              && offsetof(D2RosterPetStrc, unk0x10) == 0x10
              && offsetof(D2RosterPetStrc, nLifePercent) == 0x1C
              && offsetof(D2RosterPetStrc, bInactive) == 0x20,
              "every walk's `cmp [reg+8], id`; the owner at [reg+0Ch]; UpdateLifeAndArea's "
              "[eax+10h] and [eax+1Ch]; `mov dword ptr [esi+20h], 1` at 0x6FAB17DB");
static_assert(offsetof(D2RosterPetStrc, tPetInfo) == 0x24
              && offsetof(D2RosterPetStrc, pNext) == 0x30,
              "OnPetPacket81's copy into [eax+24h], [eax+28h], [eax+2Ch]; "
              "GetPetInfo's `add eax, 24h`; every walk steps [reg+30h]");
static_assert(offsetof(D2ClientPetInfoStrc, nDisplayState) == 8,
              "SetHirelingDisplayState's `mov [eax+8], ecx` through GetPetInfo's "
              "answer, and OnPetPacket81's copy of the third dword into +0x2C");
static_assert(offsetof(D2DataTablesStrc, pPetTypeTxt) == 0xBE8
              && offsetof(D2DataTablesStrc, nPetTypeTxtRecordCount) == 0xBF0,
              "GetAutomapClass: `cmp ecx, [edx+0BF0h]`, `mov edi, [edx+0BE8h]`");
static_assert(PETTYPEFLAGINDEX_AUTOMAP == 4,
              "GetAutomapClass: `test [edx+10h], cl` -- gdwBitMasks[4] at 0x6FAB186C");

//1.10f: D2Client.0x6FAB14C0
//  Frees the whole list. The next pointer is read before each free, and the head is cleared once, after the walk.
void __cdecl ROSTERPETS_FreeAll()
{
    D2RosterPetStrc *pPet;
    D2RosterPetStrc *pNext;

    pPet = gpRosterPetList;
    if (pPet)
    {
        do
        {
            pNext = pPet->pNext;
            FOG_Free(pPet, szRosterPetsCpp, 47, 0);
            pPet = pNext;
        } while (pNext);
    }
    gpRosterPetList = NULL;
}

//1.10f: D2Client.0x6FAB14F0
//  Packet 0x7A, the announce-or-update arm, called with the packet's +9 unit id, +5 owner id, +2 pet type (a byte) and +3 monstats id (a word).
//  For a hireling the eviction runs first (the original inlines it here); then the record with this unit id is updated in place -- type,
//  owner, monstats id, and bInactive cleared -- or a zeroed record is allocated at full life (nLifePercent 100) and pushed on the head.
//  retn 8: two stack arguments. void: the one caller is a void handler and nothing reads eax. nPetTypeId is stored into +0x04; 7 is
//  PetType.txt's `hireable`.
void __fastcall ROSTERPETS_FindOrAddPet(uint32_t dwUnitId, uint32_t dwOwnerId, int32_t nPetTypeId, int32_t nMonStatsId)
{
    D2RosterPetStrc *pPet;
    D2RosterPetStrc *pWalk;
    D2RosterPetStrc *pPrev;
    D2RosterPetStrc *pNewPet;

    pPet = gpRosterPetList;
    if (nPetTypeId == 7)
    {
        pPrev = NULL;
        for (pWalk = pPet; pWalk; pPrev = pWalk, pWalk = pWalk->pNext)
        {
            //  BUG: the hireling eviction compares +0x00, nMonStatsId, with 7, where the pet type is stored at +0x04 (the unit's other tests of a
            //  record against 7 read +0x04). The same mistake is made in each of the three copies. REACHABLE: MonStats.txt's 7 is the Ghoul, so a
            //  Necromancer's revived Ghoul loses its record to the next hireling record the client receives; no hireling's class is 7, so the
            //  eviction it means never happens.
            if (pWalk->nMonStatsId == 7 && pWalk->dwUnitId != dwUnitId)
            {
                if (pPrev)
                {
                    pPrev->pNext = pWalk->pNext;
                }
                else
                {
                    gpRosterPetList = pWalk->pNext;
                }
                FOG_Free(pWalk, szRosterPetsCpp, 89, 0);
                pPet = gpRosterPetList;
                break;
            }
        }
    }

    for (; pPet; pPet = pPet->pNext)
    {
        if (pPet->dwUnitId == dwUnitId)
        {
            pPet->nPetTypeId = nPetTypeId;
            pPet->dwOwnerId = dwOwnerId;
            pPet->nMonStatsId = nMonStatsId;
            pPet->bInactive = 0;
            return;
        }
    }

    //  BUG: FOG_Alloc's answer is used untested. LATENT: only a failed 52-byte allocation reaches it.
    pNewPet = (D2RosterPetStrc *)(UINT_PTR)FOG_Alloc(sizeof(D2RosterPetStrc), szRosterPetsCpp, 122, 0);
    memset(pNewPet, 0, sizeof(D2RosterPetStrc));
    pNewPet->dwUnitId = dwUnitId;
    pNewPet->dwOwnerId = dwOwnerId;
    pNewPet->nPetTypeId = nPetTypeId;
    pNewPet->nMonStatsId = nMonStatsId;
    pNewPet->nLifePercent = 100;
    pNewPet->pNext = gpRosterPetList;
    gpRosterPetList = pNewPet;
}

//1.10f: D2Client.0x6FAB15D0
//  Packet 0x81: a pet record with its info block, called with the packet's +8 unit id, +4 owner id, +1 pet type, +2 monstats id and a
//  three-dword block built on the caller's stack: the packet's +0x0C and +0x10 and a literal 0. retn 0Ch: three stack arguments.
//  The original inlines ROSTERPETS_FindOrAddPet and the hireling eviction here. The body, in the order the bytes run it:
//
//    1  for a hireling, the eviction (the first of two copies here);
//    2  a walk for the unit id whose answer nothing reads: its cursor is overwritten on the hireling path and read by nothing on the other;
//    3  ROSTERPETS_FindOrAddPet, inlined whole: the eviction again, then the update or the allocation;
//    4  the record found again by unit id, asserting `ptPet` (line 149); it cannot fail, step 3 has just updated or pushed that record;
//    5  the unit id, owner and type stored again, and the block copied into +0x24..+0x2F;
//    6  for a hireling whose monster unit exists, the unit re-initialised from the block's seed and name: MONSTER_InitHireling,
//       CUNIT_SetUnitAlive, MONSTER_UpdateClientLight and D2Common's UNITS_ChangeAnimMode(pUnit, 1).
//
//  The block is a D2ClientPetInfoStrc {nSeed, wName, nDisplayState}: +0x24 is what ROSTERPETS_GetPetInfo hands out, and the display state is
//  written 0 on every packet (SCMD_HandleSrv81_AddPet stores a literal 0 there). Step 6 reads the name as the word at +4, the low half of the
//  second dword. The block parameter stays `unsigned int *`: the copy reads three whole dwords.
void __fastcall ROSTERPETS_OnPetPacket81(uint32_t dwUnitId, uint32_t dwOwnerId, int32_t nPetTypeId, uint32_t nMonStatsId, uint32_t *pPetInfo)
{
    D2RosterPetStrc *pHead;
    D2RosterPetStrc *pWalk;
    D2RosterPetStrc *pPrev;
    D2RosterPetStrc *pPet;
    D2RosterPetStrc *pNewPet;
    D2UnitStrc *pUnit;
    D2ClientHirelingInitStrc tHirelingInfo;

    pHead = gpRosterPetList;
    if (nPetTypeId == 7)
    {
        pPrev = NULL;
        for (pWalk = pHead; pWalk; pPrev = pWalk, pWalk = pWalk->pNext)
        {
            //  BUG: the eviction tests +0x00, nMonStatsId, where the type is at +0x04, as in ROSTERPETS_FindOrAddPet. REACHABLE: the same Ghoul case.
            if (pWalk->nMonStatsId == 7 && pWalk->dwUnitId != dwUnitId)
            {
                if (pPrev)
                {
                    pPrev->pNext = pWalk->pNext;
                }
                else
                {
                    gpRosterPetList = pWalk->pNext;
                }
                FOG_Free(pWalk, szRosterPetsCpp, 89, 0);
                pHead = gpRosterPetList;
                break;
            }
        }
    }

    for (pWalk = pHead; pWalk && pWalk->dwUnitId != dwUnitId; pWalk = pWalk->pNext)
    {
    }

    if (nPetTypeId == 7)
    {
        pPrev = NULL;
        for (pWalk = pHead; pWalk; pPrev = pWalk, pWalk = pWalk->pNext)
        {
            //  BUG: the eviction tests +0x00, nMonStatsId, where the type is at +0x04, as in ROSTERPETS_FindOrAddPet. REACHABLE: the same Ghoul case.
            if (pWalk->nMonStatsId == 7 && pWalk->dwUnitId != dwUnitId)
            {
                if (pPrev)
                {
                    pPrev->pNext = pWalk->pNext;
                }
                else
                {
                    gpRosterPetList = pWalk->pNext;
                }
                FOG_Free(pWalk, szRosterPetsCpp, 89, 0);
                pHead = gpRosterPetList;
                break;
            }
        }
    }
    for (pPet = pHead; pPet; pPet = pPet->pNext)
    {
        if (pPet->dwUnitId == dwUnitId)
        {
            break;
        }
    }
    if (pPet)
    {
        pPet->dwOwnerId = dwOwnerId;
        pPet->nPetTypeId = nPetTypeId;
        pPet->nMonStatsId = nMonStatsId;
        pPet->bInactive = 0;
    }
    else
    {
        //  BUG: FOG_Alloc's answer is used untested. LATENT: only a failed 52-byte allocation reaches it.
        pNewPet = (D2RosterPetStrc *)(UINT_PTR)FOG_Alloc(sizeof(D2RosterPetStrc), szRosterPetsCpp, 122, 0);
        memset(pNewPet, 0, sizeof(D2RosterPetStrc));
        pNewPet->dwUnitId = dwUnitId;
        pNewPet->dwOwnerId = dwOwnerId;
        pNewPet->nPetTypeId = nPetTypeId;
        pNewPet->nMonStatsId = nMonStatsId;
        pNewPet->nLifePercent = 100;
        pNewPet->pNext = gpRosterPetList;
        gpRosterPetList = pNewPet;
    }

    //  step 4: from the head again, whichever arm ran
    for (pPet = gpRosterPetList; ; pPet = pPet->pNext)
    {
        if (!pPet)
        {
            FOG_DisplayAssert("ptPet", szRosterPetsCpp, 149);
            exit(-1);
        }
        if (pPet->dwUnitId == dwUnitId)
        {
            break;
        }
    }

    pPet->dwUnitId = dwUnitId;
    pPet->dwOwnerId = dwOwnerId;
    pPet->nPetTypeId = nPetTypeId;
    //  three dword moves into +0x24, +0x28, +0x2C: the block's twelve bytes, copied whole into the record's D2ClientPetInfoStrc
    memcpy(&pPet->tPetInfo, pPetInfo, sizeof(pPet->tPetInfo));
    if (nPetTypeId == 7)
    {
        tHirelingInfo.wNameId = (uint16_t)pPetInfo[1];
        tHirelingInfo.nLowSeed = pPetInfo[0];
        pUnit = CUNIT_FindUnitByGuidAndType(dwUnitId, UNIT_MONSTER);
        if (pUnit)
        {
            MONSTER_InitHireling(pUnit, &tHirelingInfo);
            CUNIT_SetUnitAlive(pUnit);
            MONSTER_UpdateClientLight(pUnit);
            D2COMMON_UNITS_ChangeAnimMode(pUnit, 1);
        }
    }
}

//1.10f: D2Client.0x6FAB17A0
//  Packet 0x7A, the remove arm; SCMD_HandleSrv7A_RosterPetsOnPetPacket jumps here when the packet's byte +1 is zero. The local player's
//  hireling is kept and marked -- bInactive set to 1 -- and any other record with this unit id is unlinked and freed. No record, nothing.
//  This arm tests the pet type where it is, at +0x04, the field the eviction should have read.
//  bInactive means "this hireling's unit has gone, its record is kept"; its one reader is ROSTERPETS_GetAssociatedUnitGUID, which skips a
//  marked record unless bIncludeInactive.
void __fastcall ROSTERPETS_RemovePetOrMarkHireling(uint32_t dwUnitId)
{
    D2RosterPetStrc *pPet;
    D2RosterPetStrc *pPrev;
    D2UnitStrc *pPlayer;

    pPrev = NULL;
    for (pPet = gpRosterPetList; pPet; pPrev = pPet, pPet = pPet->pNext)
    {
        if (pPet->dwUnitId == dwUnitId)
        {
            break;
        }
    }
    if (!pPet)
    {
        return;
    }

    if (pPet->nPetTypeId == 7)
    {
        pPlayer = CUNIT_GetPlayer();
        if (pPet->dwOwnerId == (pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF))
        {
            pPet->bInactive = 1;
            return;
        }
    }

    if (pPrev)
    {
        pPrev->pNext = pPet->pNext;
    }
    else
    {
        gpRosterPetList = pPet->pNext;
    }
    FOG_Free(pPet, szRosterPetsCpp, 209, 0);
}

//1.10f: D2Client.0x6FAB1810
//  What the automap draws for a pet; the answer is switched on by AUTOMAP_DrawUnit:
//
//      0  no record, or the owner shares no party with the local player
//      1  the local player owns it
//      2  the owner shares the local player's party
//      3  the pet type is outside PetType.txt, or its `automap` column is clear
//
//  The column is PETTYPEFLAGINDEX_AUTOMAP, 4: the low byte of dwPetFlags against gdwBitMasks[4], written at that width below. The range test
//  is signed, and the row's address is then tested for NULL, which only an unloaded table and row 0 together could make -- written as the
//  bytes have it. The party ids compare as words.
int32_t __fastcall ROSTERPETS_GetAutomapClass(D2UnitGUID dwPetUnitId)
{
    D2RosterPetStrc *pPet;
    D2PetTypeTxt *pPetTypeTxt;
    D2UnitStrc *pPlayer;
    D2UnitGUID dwPlayerId;
    int32_t nPetTypeId;
    int16_t nPartyId;

    for (pPet = gpRosterPetList; pPet; pPet = pPet->pNext)
    {
        if (pPet->dwUnitId == dwPetUnitId)
        {
            break;
        }
    }
    if (!pPet)
    {
        return 0;
    }

    nPetTypeId = pPet->nPetTypeId;
    if (nPetTypeId < 0 || nPetTypeId >= sgptDataTables->nPetTypeTxtRecordCount)
    {
        return 3;
    }
    pPetTypeTxt = &sgptDataTables->pPetTypeTxt[nPetTypeId];
    if (!pPetTypeTxt)
    {
        return 3;
    }
    if (!((uint8_t)pPetTypeTxt->dwPetFlags & (uint8_t)gdwBitMasks[PETTYPEFLAGINDEX_AUTOMAP]))
    {
        return 3;
    }

    pPlayer = CUNIT_GetPlayer();
    dwPlayerId = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;
    if (pPet->dwOwnerId == dwPlayerId)
    {
        return 1;
    }

    nPartyId = ROSTER_PartyGetSharedPartyId(ROSTER_FindEntryByUnitOrCorpse(dwPlayerId));
    if (nPartyId == -1)
    {
        return 0;
    }
    if (nPartyId != ROSTER_PartyGetSharedPartyId(ROSTER_FindEntryByUnitOrCorpse(pPet->dwOwnerId)))
    {
        return 0;
    }
    return 2;
}

//1.10f: D2Client.0x6FAB18D0
//  The pet's info block: the address of the record's +0x24 (a D2ClientPetInfoStrc: the seed at +0, the name word at +4, the display state
//  at +8), NULL when no record has this unit id.
D2ClientPetInfoStrc *__fastcall ROSTERPETS_GetPetInfo(uint32_t dwPetUnitId)
{
    D2RosterPetStrc *pPet;

    for (pPet = gpRosterPetList; pPet; pPet = pPet->pNext)
    {
        if (pPet->dwUnitId == dwPetUnitId)
        {
            return &pPet->tPetInfo;
        }
    }
    return NULL;
}

//1.10f: D2Client.0x6FAB18F0
//  How many pets of one type a unit owns: the records whose nPetTypeId matches and whose owner is pOwner's id; a NULL owner counts as id -1.
int32_t __fastcall ROSTERPETS_CountByType(D2UnitStrc *pOwner, int32_t nPetTypeId)
{
    D2RosterPetStrc *pPet;
    uint32_t dwOwnerId;
    int32_t nCount;

    dwOwnerId = pOwner ? pOwner->dwUnitId : 0xFFFFFFFF;
    nCount = 0;
    for (pPet = gpRosterPetList; pPet; pPet = pPet->pNext)
    {
        if (pPet->nPetTypeId == nPetTypeId && pPet->dwOwnerId == dwOwnerId)
        {
            ++nCount;
        }
    }
    return nCount;
}

//1.10f: D2Client.0x6FAB1920
//  The unit id of a unit's pet of one type, -1 for none or a NULL unit: the first record from the head whose nPetTypeId is nKind and whose
//  owner is pUnit's id, passing over a marked one (bInactive) unless bIncludeInactive. retn 4. pUnit's id is read before the list is tested.
int32_t __fastcall ROSTERPETS_GetAssociatedUnitGUID(D2UnitStrc *pUnit, int32_t nKind, int32_t bIncludeInactive)
{
    D2RosterPetStrc *pPet;
    uint32_t dwOwnerId;

    if (!pUnit)
    {
        return -1;
    }
    dwOwnerId = pUnit->dwUnitId;
    for (pPet = gpRosterPetList; pPet; pPet = pPet->pNext)
    {
        if (pPet->nPetTypeId == nKind && pPet->dwOwnerId == dwOwnerId && (bIncludeInactive || !pPet->bInactive))
        {
            return pPet->dwUnitId;
        }
    }
    return -1;
}

//1.10f: D2Client.0x6FAB1970
//  Whether any record names this unit as its owner, 1 or 0; a NULL unit is id -1.
BOOL __fastcall ROSTERPETS_UnitHasPets(D2UnitStrc *pUnit)
{
    D2RosterPetStrc *pPet;
    uint32_t dwOwnerId;

    dwOwnerId = pUnit ? pUnit->dwUnitId : 0xFFFFFFFF;
    for (pPet = gpRosterPetList; pPet; pPet = pPet->pNext)
    {
        if (pPet->dwOwnerId == dwOwnerId)
        {
            return TRUE;
        }
    }
    return FALSE;
}

//1.10f: D2Client.0x6FAB19A0
//  Re-shifts the palette of every skeleton -- pet type 4, PetType.txt's `skeleton` (the skeleton mage is 5 and is not touched). For each
//  record of type 4 whose monster unit exists, the shift is:
//
//    0  with no local player;
//    otherwise, the record found again by the UNIT's id -- its own record --
//    1  when its owner is the local player, or ROSTER_IsHostileTo(me, owner) answers 0; else 0;
//    and with no record, 1 only when the player and the pet are both monsters and D2Common's STATLIST_AreUnitsAligned agrees; else 0.
//
//  Both shifts are set to it. A 0 does not end the walk: 0x6FAB1A3C zeroes esi and jumps to the common stores at 0x6FAB1A0E.
void __cdecl ROSTERPETS_UpdateSkeletonPalettes()
{
    D2RosterPetStrc *pEntry;
    D2RosterPetStrc *pSelf;
    D2UnitStrc *pPlayer;
    D2UnitStrc *pPetUnit;
    int32_t nShift;

    pEntry = gpRosterPetList;
    pPlayer = CUNIT_GetPlayer();
    for (; pEntry; pEntry = pEntry->pNext)
    {
        if (pEntry->nPetTypeId != 4)
        {
            continue;
        }
        pPetUnit = CUNIT_FindUnitByGuidAndType(pEntry->dwUnitId, UNIT_MONSTER);
        if (!pPetUnit)
        {
            continue;
        }

        for (pSelf = gpRosterPetList; pSelf; pSelf = pSelf->pNext)
        {
            if (pSelf->dwUnitId == pPetUnit->dwUnitId)
            {
                break;
            }
        }

        if (!pPlayer)
        {
            nShift = 0;
        }
        else if (pSelf)
        {
            if (pSelf->dwOwnerId == pPlayer->dwUnitId || !ROSTER_IsHostileTo(pPlayer->dwUnitId, pSelf->dwOwnerId))
            {
                nShift = 1;
            }
            else
            {
                nShift = 0;
            }
        }
        else if (pPlayer->dwUnitType == UNIT_MONSTER && pPetUnit->dwUnitType == UNIT_MONSTER && D2COMMON_STATLIST_AreUnitsAligned(pPlayer, pPetUnit))
        {
            nShift = 1;
        }
        else
        {
            nShift = 0;
        }

        CUNIT_SetUnitPaletteShift1(pPetUnit, nShift);
        CUNIT_SetUnitPaletteShift2(pPetUnit, nShift);
    }
}

//1.10f: D2Client.0x6FAB1A40
//  Packet 0x7F for a pet: its life percent into +0x1C and wArea into +0x10 (zero-extended, stored as a dword). Called by
//  SCMD_HandleSrv7F_PartyLifeUpdate when the packet's bIsPlayer is zero; the word is the packet's nLevelId. For a hireling whose monster unit exists,
//  the unit's hit points are set to that percent of its maximum. STATLIST_GetMaxLifeFromUnit takes one argument and STATLIST_SetUnitStat four: the
//  `push 0` at 0x6FAB1A86 is SetUnitStat's nLayer. The product is a signed wrapping imul, then a signed divide by 100 (0x51EB851F). It is taken
//  through unsigned below so that a product past INT_MAX wraps as the bytes do instead of being undefined.
void __fastcall ROSTERPETS_UpdateLifeAndArea(uint32_t dwPetUnitId, int32_t nLifePercent, uint16_t wArea)
{
    D2RosterPetStrc *pPet;
    D2UnitStrc *pUnit;
    int32_t nMaxLife;

    for (pPet = gpRosterPetList; pPet; pPet = pPet->pNext)
    {
        if (pPet->dwUnitId == dwPetUnitId)
        {
            break;
        }
    }
    if (!pPet)
    {
        return;
    }

    pPet->nLifePercent = nLifePercent;
    pPet->unk0x10 = wArea;
    if (pPet->nPetTypeId == 7)
    {
        pUnit = CUNIT_FindUnitByGuidAndType(dwPetUnitId, UNIT_MONSTER);
        if (pUnit)
        {
            nMaxLife = D2COMMON_STATLIST_GetMaxLifeFromUnit(pUnit);
            D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_HITPOINTS, (int32_t)((uint32_t)nMaxLife * (uint32_t)nLifePercent) / 100, 0);
        }
    }
}

//1.10f: D2Client.0x6FAB1AC0
//  A pet's life percent (+0x1C), 0 when no record has this unit id.
int32_t __fastcall ROSTERPETS_GetLifePercent(uint32_t dwPetUnitId)
{
    D2RosterPetStrc *pPet;

    for (pPet = gpRosterPetList; pPet; pPet = pPet->pNext)
    {
        if (pPet->dwUnitId == dwPetUnitId)
        {
            return pPet->nLifePercent;
        }
    }
    return 0;
}

//1.10f: D2Client.0x6FAB1AE0
//  A pet's monstats id (+0x00), 0 when no record has this unit id.
int32_t __fastcall ROSTERPETS_GetMonStatsId(uint32_t dwPetUnitId)
{
    D2RosterPetStrc *pPet;

    for (pPet = gpRosterPetList; pPet; pPet = pPet->pNext)
    {
        if (pPet->dwUnitId == dwPetUnitId)
        {
            return pPet->nMonStatsId;
        }
    }
    return 0;
}

//1.10f: D2Client.0x6FAB1B00
//  A pet's owner id (+0x0C), and -1 -- not 0, as its neighbours answer -- when no record has this unit id.
int32_t __fastcall ROSTERPETS_GetOwnerId(uint32_t dwPetUnitId)
{
    D2RosterPetStrc *pPet;

    for (pPet = gpRosterPetList; pPet; pPet = pPet->pNext)
    {
        if (pPet->dwUnitId == dwPetUnitId)
        {
            return pPet->dwOwnerId;
        }
    }
    return -1;
}

//1.10f: D2Client.0x6FAB1B20
//  A pet's type (+0x04), 0 when no record has this unit id (0 is PetType.txt's first row).
int32_t __fastcall ROSTERPETS_GetPetType(uint32_t dwPetUnitId)
{
    D2RosterPetStrc *pPet;

    for (pPet = gpRosterPetList; pPet; pPet = pPet->pNext)
    {
        if (pPet->dwUnitId == dwPetUnitId)
        {
            return pPet->nPetTypeId;
        }
    }
    return 0;
}

//1.10f: D2Client.0x6FAB1B40
//  Whether this monster's owner is hostile to the local player: 1 when the monster has no record (or pPet is NULL, id -1); otherwise
//  PLAYERLIST_RosterTestInfoFlag(owner, me, ROSTERINFOFLAG_HOSTILE). The local player must exist and have an id: the assert
//  `dwControlId != INVALID_PLAYER_ID` (line 549) exits. It writes nothing.
int32_t __fastcall ROSTERPETS_IsPetOwnerHostile(D2UnitStrc *pPet)
{
    D2RosterPetStrc *pRecord;
    D2UnitStrc *pPlayer;
    uint32_t dwPetId;
    uint32_t dwOwnerId;

    dwPetId = pPet ? pPet->dwUnitId : 0xFFFFFFFF;
    dwOwnerId = 0xFFFFFFFF;
    for (pRecord = gpRosterPetList; pRecord; pRecord = pRecord->pNext)
    {
        if (pRecord->dwUnitId == dwPetId)
        {
            dwOwnerId = pRecord->dwOwnerId;
            break;
        }
    }

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer || pPlayer->dwUnitId == 0xFFFFFFFF)
    {
        FOG_DisplayAssert("dwControlId != INVALID_PLAYER_ID", szRosterPetsCpp, 549);
        exit(-1);
    }
    if (dwOwnerId == 0xFFFFFFFF)
    {
        return 1;
    }
    return PLAYERLIST_RosterTestInfoFlag(dwOwnerId, pPlayer->dwUnitId, ROSTERINFOFLAG_HOSTILE);
}

//1.10f: D2Client.0x6FAB1BB0
//  Re-shifts a player's raised undead: the records owned by dwOwnerGuid whose monstats id is 363 or 364, MonStats.txt's `necroskeleton` and
//  `necromage` (signed compares). Called by ROSTER_ApplyInfoFlags when the flags it applies are toward the local player. For each whose
//  monster unit exists, the shift is 1 when its owner is not hostile to the local player and 0 otherwise; shift 1 takes it, shift 2 only
//  when it equalled shift 1 before (both read before either store). The original inlines ROSTERPETS_IsPetOwnerHostile here.
void __fastcall ROSTERPETS_UpdateRaisedUndead(uint32_t dwOwnerGuid)
{
    D2RosterPetStrc *pEntry;
    D2UnitStrc *pUnit;
    int32_t nOldShift2;
    int32_t nShift;

    for (pEntry = gpRosterPetList; pEntry; pEntry = pEntry->pNext)
    {
        if (pEntry->dwOwnerId != dwOwnerGuid || pEntry->nMonStatsId < 363 || pEntry->nMonStatsId > 364)
        {
            continue;
        }
        pUnit = CUNIT_FindUnitByGuidAndType(pEntry->dwUnitId, UNIT_MONSTER);
        if (!pUnit)
        {
            continue;
        }
        nOldShift2 = CUNIT_GetUnitPaletteShift2(pUnit);

        if (ROSTERPETS_IsPetOwnerHostile(pUnit))
        {
            nShift = 0;
        }
        else
        {
            nShift = 1;
        }

        if (nOldShift2 == CUNIT_GetUnitPaletteShift1(pUnit))
        {
            CUNIT_SetUnitPaletteShift2(pUnit, nShift);
        }
        CUNIT_SetUnitPaletteShift1(pUnit, nShift);
    }
}

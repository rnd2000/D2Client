//  SCmd.cpp -- every server packet the client receives: the router that walks the arriving bytes through the server-command table, the
//  handlers that table names, and the per-unit queue that holds a packet until its unit's own update applies it. The original source file is
//  D2Client\GAME\SCmd.cpp (the path of the asserts, szFile in SCmd.h). 122 functions over 0x6FAB1CB0..0x6FAB5750, from SCMD_SaveCharacterFile
//  to SCMD_ProcessUnitPacketQueue.
//  The router calls a handler with the packet pointer in ecx and nothing else (what sits in edx at the call is leftover), so a handler takes one
//  argument and a pfProcessUnit handler two (unit, packet). Several handlers copy the packet onto their own stack and pass the copy's address,
//  written as a struct copy so that MSVC chooses the moves.
//  Where the original passes a byte or a word in a register whose upper bits are dirty, the callee masks or never reads them, so ours passes
//  the byte or word and nothing downstream can tell.

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "WINMAIN.h"
#include "UnitMode.h"
#include "SCmd.h"
#include "Cursor.h"

#include <intrin.h>                 // _BitScanReverse, the `bsr` in 0xAC


//  NUM_SCMDS = 174: `cmp bl, 0AEh` at 0x6FAB5109 in the router and `cmp al, 0AEh` at 0x6FAB5677 in the drain. The table, gSCmdTable, is in
//  SCmd.h.
static const int32_t NUM_SCMDS = 174;

//  The original's assert strings, byte for byte. The two nCmdSize texts differ: the router's parenthesises the table's field.
static const char szAssertCmdBound[] = "bCmd < NUM_SCMDS";                    // 0x6FB76F7C
static const char szAssertRouteSize[] = "(ptMsgStruct->nCmdSize == -1) || (packetSize == (ptMsgStruct->nCmdSize))";  // 0x6FB76F30
static const char szAssertSlotSize[] = "nSize <= sgnMaxUnitPacketDataSize";   // 0x6FB76F0C
static const char szAssertQueueUsed[] = "ptPacket->nUsed <= ptPacket->nTotal"; // 0x6FB76FF8
static const char szAssertDrainSize[] = "(ptMsgStruct->nCmdSize == -1) || (packetSize == ptMsgStruct->nCmdSize)";  // 0x6FB76FB0
static const char szAssertUnitHandler[] = "ptMsgStruct->pfProcessUnit";       // 0x6FB76F90

static_assert(sizeof(szAssertCmdBound) == 16 + 1 && sizeof(szAssertRouteSize) == 72 + 1
              && sizeof(szAssertSlotSize) == 33 + 1 && sizeof(szAssertQueueUsed) == 35 + 1
              && sizeof(szAssertDrainSize) == 70 + 1 && sizeof(szAssertUnitHandler) == 26 + 1,
              "the six assert texts at 0x6FB76F0C..0x6FB76FF8, measured in the stock DLL");

static_assert(offsetof(D2SCmdStrc, nCmdSize) == 4 && offsetof(D2SCmdStrc, pfProcessUnit) == 8,
              "D2SCmdStrc: the router reads nCmdSize at +4 (0x6FAB5123) and pfProcessUnit "
              "at +8 (0x6FAB515D)");
static_assert(offsetof(D2PacketListStrc, nTotal) == 0 && offsetof(D2PacketListStrc, nUsed) == 4
              && offsetof(D2PacketListStrc, pPacketData) == 0x0C,
              "D2PacketListStrc: the drain's assert compares +4 with +0 (0x6FAB5628), and the "
              "allocator's buffer is +0x0C (0x6FAB5574)");


static_assert(sizeof(gSCmdTable) / sizeof(gSCmdTable[0]) == NUM_SCMDS,
              "gSCmdTable: 174 rows of 12 bytes at 0x6FB76270, szFileModeWriteBinary at 0x6FB76A98");


//1.10f: D2Client.0x6FAB54C0
//  Grows a unit's queue by five slots, allocating the queue on first use, and sizes a slot from the table the first time it runs: the largest
//  nCmdSize among the twenty queueing rows is 0x68's 21, so a slot is 26 bytes (sgnMaxUnitPacketDataSize, nCmdSize plus 5).
D2PacketListStrc *__fastcall SCMD_AllocUnitPacketQueue(D2UnitStrc *pUnit)
{
    //  The walk ends at &gSCmdTable[NUM_SCMDS].nCmdSize, 0x6FB76A9C, which is also szFile's address.
    int32_t nMaxPacketSize = sgnMaxUnitPacketDataSize;
    if (!nMaxPacketSize)
    {
        for (D2SCmdStrc *ptMsgStruct = gSCmdTable; ptMsgStruct < &gSCmdTable[NUM_SCMDS]; ++ptMsgStruct)
        {
            if (ptMsgStruct->pfProcessUnit && ptMsgStruct->nCmdSize > nMaxPacketSize)
                nMaxPacketSize = ptMsgStruct->nCmdSize;
        }
        sgnMaxUnitPacketDataSize = nMaxPacketSize + 5;
    }

    D2PacketListStrc *pQueue = CUNIT_GetUnitPacketQueue(pUnit);
    if (!pQueue)
    {
        pQueue = (D2PacketListStrc *)FOG_Alloc(sizeof(D2PacketListStrc), (uint32_t)(UINT_PTR)szFile, 4696, 0);
        pQueue->nTotal = 0;
        pQueue->nUsed = 0;
        pQueue->unk0x08 = 0;
        pQueue->pPacketData = NULL;
        CUNIT_SetUnitPacketQueue(pUnit, pQueue);
    }

    const int32_t nNewCapacity = pQueue->nTotal + 5;
    uint8_t *pNewBuffer = (uint8_t *)FOG_Alloc(nNewCapacity * sgnMaxUnitPacketDataSize, (uint32_t)(UINT_PTR)szFile, 4701, 0);
    memset(pNewBuffer, 0, nNewCapacity * sgnMaxUnitPacketDataSize);
    if (pQueue->pPacketData)
    {
        memcpy(pNewBuffer, pQueue->pPacketData, sgnMaxUnitPacketDataSize * pQueue->nTotal);
        FOG_Free((uint32_t)(UINT_PTR)pQueue->pPacketData, (uint32_t)(UINT_PTR)szFile, 4705, 0);
    }
    pQueue->nTotal = nNewCapacity;
    pQueue->pPacketData = pNewBuffer;
    return pQueue;
}

//  Appends a packet to a unit's queue. The queue grows when the slot after this one would be the last (`nUsed + 1 == nTotal`), so the last slot
//  of each allocation is never written before the next.
static void SCMD_AppendUnitPacket(D2UnitStrc *pUnit, uint8_t *pPacket, int32_t nSize)
{
    D2PacketListStrc *pQueue = CUNIT_GetUnitPacketQueue(pUnit);
    if (!pQueue || pQueue->nUsed + 1 == pQueue->nTotal)
        pQueue = SCMD_AllocUnitPacketQueue(pUnit);

    if (nSize > sgnMaxUnitPacketDataSize)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR)szAssertSlotSize, (uint32_t)(UINT_PTR)szFile, 4721);
        exit(-1);
    }

    memcpy(&pQueue->pPacketData[sgnMaxUnitPacketDataSize * pQueue->nUsed], pPacket, nSize);
    ++pQueue->nUsed;
}

//1.10f: D2Client.0x6FAB50B0
//  The router. Every server packet reaches D2Client through GAME_UpdateFrame, which drains D2Net's game-packet list into a 512-byte buffer and
//  hands each dequeue here, and once more with the -1 that ends the drain (the router's first test). The original inlines
//  SCMD_AppendUnitPacket here.
//  gnRoutePacketCalls, gnRoutedPacketBytes, gnOversizePacketBatches (a buffer over 500 bytes is only counted, nothing caps it) and
//  gnLastRoutedPacketId are touched by the router and nothing reads any of them.
void __fastcall SCMD_RouteServerPackets(uint8_t *pBuffer, int32_t nSize)
{
    if (nSize == -1)
        return;

    int32_t nRemaining = nSize;

    //  The tick is taken and never used, as in the original (0x6FAB50CC).
    GetTickCount();

    if (nSize > 500)
        ++gnOversizePacketBatches;

    while (nRemaining > 0)
    {
        const uint8_t bCmd = pBuffer[0];

        //  BUG: nSize, the whole buffer, is measured against every packet where D2Net's own callers pass the bytes left (`mov edx, edi` at
        //  0x6FAB50F0; edi is nSize, and the running count at [esp+1Ch] never reaches edx). The two are equal while D2Net's dequeue copies one packet
        //  per call.
        int32_t nSizeRead;
        if (!D2NET_SERVER_GetServerPacketSize((D2PacketBufferStrc *)pBuffer, nSize, &nSizeRead))
            return;

        //  A 16-bit length: every use is masked, `and <reg>, 0FFFFh`.
        const uint16_t packetSize = (uint16_t)nSizeRead;

        if (bCmd >= NUM_SCMDS)
        {
            FOG_DisplayAssert((uint32_t)(UINT_PTR)szAssertCmdBound, (uint32_t)(UINT_PTR)szFile, 5186);
            exit(-1);
        }

        D2SCmdStrc *ptMsgStruct = &gSCmdTable[bCmd];

        //  The size is tested twice, asserted at 0x6FAB5142 and then tested again at 0x6FAB5155 around everything that dispatches; the second test
        //  cannot fail once the first has passed.
        if (!((ptMsgStruct->nCmdSize == -1) || (packetSize == (ptMsgStruct->nCmdSize))))
        {
            FOG_DisplayAssert((uint32_t)(UINT_PTR)szAssertRouteSize, (uint32_t)(UINT_PTR)szFile, 5193);
            exit(-1);
        }

        if ((ptMsgStruct->nCmdSize == -1) || (packetSize == ptMsgStruct->nCmdSize))
        {
            if (ptMsgStruct->pfProcessUnit)
            {
                //  0x67..0x6D are monster-only forms with no type byte: the GUID is at +1 and the type is `mov edx, 1` (0x6FAB5177). Every other
                //  queueing id is read as type at +1, GUID at +2.
                //  BUG: 0x6E..0x72 are one byte long and each has a pfProcessUnit, so the router takes the unit type from byte 1 and the GUID from
                //  bytes 2..5, past the packet, and queues the one byte on whatever unit that finds, for the drain to hand to GAME_NullFunc.
                uint32_t dwUnitGUID;
                D2C_UnitTypes nUnitType;
                if (bCmd < 0x67 || bCmd > 0x6D)
                {
                    dwUnitGUID = *(uint32_t *)&pBuffer[2];
                    nUnitType = (D2C_UnitTypes)pBuffer[1];
                }
                else
                {
                    dwUnitGUID = *(uint32_t *)&pBuffer[1];
                    nUnitType = UNIT_MONSTER;
                }

                D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(dwUnitGUID, nUnitType);
                if (pUnit)
                    SCMD_AppendUnitPacket(pUnit, pBuffer, packetSize);
            }

            //  Four ids are read here and the results thrown away: a monster lookup for 0x0D and three Fog bit readers whose widths are the packets'
            //  layouts. Nothing survives any of the four; the calls are in the original, so they are here.
            switch (bCmd)
            {
            case 0x0D:
                if (pBuffer[1] == 1)
                    CUNIT_FindUnitByGuidAndType(*(uint32_t *)&pBuffer[2], UNIT_MONSTER);
                break;

            case 0x18:
            {
                D2BitBufferStrc tBits18;
                FOG_BITMANIP_Initialize(&tBits18, pBuffer, 15);
                FOG_BITMANIP_Read(&tBits18, 8);
                FOG_BITMANIP_Read(&tBits18, 15);
                FOG_BITMANIP_Read(&tBits18, 15);
                FOG_BITMANIP_Read(&tBits18, 15);
                FOG_BITMANIP_Read(&tBits18, 7);
                FOG_BITMANIP_Read(&tBits18, 7);
                FOG_BITMANIP_Read(&tBits18, 16);
                FOG_BITMANIP_Read(&tBits18, 16);
                FOG_BITMANIP_Read(&tBits18, 8);
                FOG_BITMANIP_Read(&tBits18, 8);
                break;
            }

            case 0x95:
            {
                D2BitBufferStrc tBits95;
                FOG_BITMANIP_Initialize(&tBits95, pBuffer, 13);
                FOG_BITMANIP_Read(&tBits95, 8);
                FOG_BITMANIP_Read(&tBits95, 15);
                FOG_BITMANIP_Read(&tBits95, 15);
                FOG_BITMANIP_Read(&tBits95, 15);
                FOG_BITMANIP_Read(&tBits95, 16);
                FOG_BITMANIP_Read(&tBits95, 16);
                FOG_BITMANIP_Read(&tBits95, 8);
                FOG_BITMANIP_Read(&tBits95, 8);
                break;
            }

            case 0x96:
            {
                D2BitBufferStrc tBits96;
                FOG_BITMANIP_Initialize(&tBits96, pBuffer, 9);
                FOG_BITMANIP_Read(&tBits96, 8);
                FOG_BITMANIP_Read(&tBits96, 15);
                FOG_BITMANIP_Read(&tBits96, 16);
                FOG_BITMANIP_Read(&tBits96, 16);
                FOG_BITMANIP_Read(&tBits96, 8);
                FOG_BITMANIP_Read(&tBits96, 8);
                break;
            }

            default:
                break;
            }

            if (ptMsgStruct->pfProcessCmd)
                ptMsgStruct->pfProcessCmd(pBuffer);
        }

        gnLastRoutedPacketId = bCmd;
        pBuffer += packetSize;
        gnRoutedPacketBytes += packetSize;
        nRemaining -= packetSize;
    }

    //  Not reached when a size cannot be read: that path returns above.
    ++gnRoutePacketCalls;
}

//1.10f: D2Client.0x6FAB55C0
//  Frees the unit's packet buffer, then the queue, then the unit's pointer to it.
void __fastcall SCMD_FreeUnitPacketQueue(D2UnitStrc *pUnit)
{
    D2PacketListStrc *pQueue = CUNIT_GetUnitPacketQueue(pUnit);
    if (pQueue)
    {
        FOG_Free((uint32_t)(UINT_PTR)pQueue->pPacketData, (uint32_t)(UINT_PTR)szFile, 5391, 0);
        FOG_Free((uint32_t)(UINT_PTR)pQueue, (uint32_t)(UINT_PTR)szFile, 5392, 0);
        CUNIT_SetUnitPacketQueue(pUnit, NULL);
    }
}

//1.10f: D2Client.0x6FAB5610
//  Applies a unit's queued packets through each row's pfProcessUnit, then clears the slots it used. The loop bound is re-read after every
//  handler (`mov eax, [ebx+4]` at 0x6FAB56B9).
void __fastcall SCMD_ProcessUnitPacketQueue(D2UnitStrc *pUnit)
{
    D2PacketListStrc *ptPacket = CUNIT_GetUnitPacketQueue(pUnit);
    if (!ptPacket)
        return;

    if (ptPacket->nUsed > ptPacket->nTotal)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR)szAssertQueueUsed, (uint32_t)(UINT_PTR)szFile, 5406);
        exit(-1);
    }

    for (int32_t iPacket = 0; iPacket < ptPacket->nUsed; ++iPacket)
    {
        uint8_t *pPacket = &ptPacket->pPacketData[iPacket * sgnMaxUnitPacketDataSize];

        //  BUG: when a queued packet's size cannot be read the drain returns (`jz` to 0x6FAB56E9 at 0x6FAB5673), past the clear and past
        //  `nUsed = 0`; the router keeps appending, and the queue grows by five slots each time it fills.
        int32_t nSizeRead;
        if (!D2NET_SERVER_GetServerPacketSize((D2PacketBufferStrc *)pPacket, sgnMaxUnitPacketDataSize, &nSizeRead))
            return;

        const uint8_t bCmd = pPacket[0];
        if (bCmd >= NUM_SCMDS)
        {
            FOG_DisplayAssert((uint32_t)(UINT_PTR)szAssertCmdBound, (uint32_t)(UINT_PTR)szFile, 5418);
            exit(-1);
        }

        D2SCmdStrc *ptMsgStruct = &gSCmdTable[bCmd];
        const uint16_t packetSize = (uint16_t)nSizeRead;
        if (!((ptMsgStruct->nCmdSize == -1) || (packetSize == ptMsgStruct->nCmdSize)))
        {
            FOG_DisplayAssert((uint32_t)(UINT_PTR)szAssertDrainSize, (uint32_t)(UINT_PTR)szFile, 5421);
            exit(-1);
        }

        if (!ptMsgStruct->pfProcessUnit)
        {
            FOG_DisplayAssert((uint32_t)(UINT_PTR)szAssertUnitHandler, (uint32_t)(UINT_PTR)szFile, 5423);
            exit(-1);
        }

        ptMsgStruct->pfProcessUnit(pUnit, pPacket);
    }

    if (ptPacket->nUsed)
        memset(ptPacket->pPacketData, 0, sgnMaxUnitPacketDataSize * ptPacket->nUsed);
    ptPacket->nUsed = 0;
}


//  NPC conversation packets: 0x28, 0x29, 0x2A, 0x42, 0x4E, 0x4F, 0x50, 0x62, 0x8A, 0x91 and 0x9B, each a few instructions that hand the packet
//  on to the NPC menu.

static_assert(offsetof(D2GSPacketSrv28, nUnitType) == 1 && offsetof(D2GSPacketSrv28, dwUnitGUID) == 2
              && offsetof(D2GSPacketSrv28, nAbortUnitType) == 6 && offsetof(D2GSPacketSrv28, pData) == 7,
              "D2GSPacketSrv28: `mov cl, [ecx+1]`, `mov edx, [ecx+2]`, `mov al, [ecx+6]`, "
              "`lea edx, [ecx+7]` at 0x6FAB2B80..0x6FAB2B8B");
static_assert(offsetof(D2GSPacketSrv29, pData) == 1,
              "D2GSPacketSrv29: `inc ecx` at 0x6FAB2BB0");
static_assert(offsetof(D2GSPacketSrv62, nUnitType) == 1 && offsetof(D2GSPacketSrv62, nUnitId) == 2,
              "D2GSPacketSrv62: `mov cl, [ecx+1]` and `mov edx, [ecx+2]` at 0x6FAB2BA0");
static_assert(offsetof(D2GSPacketSrv9B, wNameTblId) == 1 && offsetof(D2GSPacketSrv9B, nCost) == 3,
              "D2GSPacketSrv9B: `mov cx, [ecx+1]` and `mov dx, [ecx+3]` at 0x6FAB43D2");

//1.10f: D2Client.0x6FAB2B80
//  Packet 0x28: an NPC starts a conversation.
void __fastcall SCMD_HandleSrv28_NpcMenu(D2GSPacketSrv28 *pPacket)
{
    NPCMENU_BeginInteraction(pPacket->nUnitType, (int32_t)pPacket->dwUnitGUID, pPacket->pData, pPacket->nAbortUnitType);
}

//1.10f: D2Client.0x6FAB2BA0
//  Packet 0x62: the conversation stops. A tail jump.
void __fastcall SCMD_HandleSrv62_NpcInteractionStop(D2GSPacketSrv62 *pPacket)
{
    NPCMENU_OnInteractionStop(pPacket->nUnitType, pPacket->nUnitId);
}

//1.10f: D2Client.0x6FAB2BB0
//  Packet 0x29: the secondary quest record, from +1. A tail jump.
void __fastcall SCMD_HandleSrv29_QuestRecord(D2GSPacketSrv29 *pPacket)
{
    NPCMENU_QuestCopySecondaryQuestRecord(pPacket->pData);
}

//1.10f: D2Client.0x6FAB3990
//  Packet 0x2A: a transaction's answer, copied (15 bytes).
void __fastcall SCMD_HandleSrv2A_TransactionResult(D2GSPacketSrv2A *pPacket)
{
    D2GSPacketSrv2A tPacket = *pPacket;
    NPCMENU_OnTransactionResult(&tPacket);
}

//1.10f: D2Client.0x6FAB3B30
//  Packet 0x42, copied (6 bytes), handed to the item code that acknowledges an item action and drops the cursor item.
void __fastcall SCMD_HandleSrv42_ClearCursor(D2GSPacketSrv42 *pPacket)
{
    D2GSPacketSrv42 tPacket = *pPacket;
    ITEM_NpcMenuAckItemActionAndDropCursorItem(&tPacket);
}

//1.10f: D2Client.0x6FAB3C30
//  Packet 0x50, copied (15 bytes), handed to Quests.cpp's dispatch on the quest id.
void __fastcall SCMD_HandleSrv50_QuestSpecial(D2GSPacketSrv50 *pPacket)
{
    D2GSPacketSrv50 tPacket = *pPacket;
    QUESTS_DispatchQuestPacket(&tPacket);
}

//1.10f: D2Client.0x6FAB3C80
//  Packet 0x4F, header only: the hireling offers are cleared. A tail jump.
void __fastcall SCMD_HandleSrv4F_ClearHirelingOffers(D2GSPacketSrv4F *pPacket)
{
    NPCMENU_ClearHirelingOffers();
}

//1.10f: D2Client.0x6FAB3C90
//  Packet 0x4E: one hireling offer, copied (7 bytes).
void __fastcall SCMD_HandleSrv4E_AddHirelingOffer(D2GSPacketSrv4E *pPacket)
{
    D2GSPacketSrv4E tPacket = *pPacket;
    NPCMENU_AddHirelingOffer(&tPacket);
}

//1.10f: D2Client.0x6FAB3E40
//  Packet 0x91, copied (26 bytes, `rep movsd` of six and a `movsw`) and passed as the word pointer the callee declares.
void __fastcall SCMD_HandleSrv91_NpcIntros(D2GSPacketSrv91 *pPacket)
{
    D2GSPacketSrv91 tPacket = *pPacket;
    NPCMENU_MarkEntriesFromPacket91((uint16_t *)&tPacket);
}

//1.10f: D2Client.0x6FAB4350
//  Packet 0x8A, copied (6 bytes).
void __fastcall SCMD_HandleSrv8A_NpcAlert(D2GSPacketSrv8A *pPacket)
{
    D2GSPacketSrv8A tPacket = *pPacket;
    NPCMENU_ShowNpcAlert(&tPacket);
}

//1.10f: D2Client.0x6FAB43D0
//  Packet 0x9B: the resurrect offer. A tail jump. The name is loaded with `mov cx, [ecx+1]`, leaving the packet pointer's upper bits in ecx;
//  NPCMENU_SetResurrectMercOffer reads cx alone, so the zero-extended name ours passes is the same argument.
//  BUG: the server writes the cost as a 32-bit value at +3, but 0x9B passes its low word, zero-extended (`xor edx, edx / mov dx, [ecx+3]`,
//  0x6FAB43D0), and SetResurrectMercOffer stores that edx whole in gnMercResurrectCost: a cost of 65536 or more would be shown and charged
//  modulo 65536 on the client's side.
void __fastcall SCMD_HandleSrv9B_ResurrectMercOffer(D2GSPacketSrv9B *pPacket)
{
    NPCMENU_SetResurrectMercOffer((D2C_StringIndices)pPacket->wNameTblId, (uint16_t)pPacket->nCost);
}


//  Packets of the game's start and of the units that come and go: 0x01..0x0B, 0x51 and 0x52.
//  The original's assert string for the act pointer, byte for byte; the halt text szCouldNotGetActMsg is SCmd.h's.
static const char szAssertGhAct[] = "ghAct";                      // 0x6FB74BF0
static_assert(sizeof(szAssertGhAct) == 5 + 1, "the assert text at 0x6FB74BF0, measured in the DLL");

static_assert(offsetof(D2GSPacketSrv01, nDifficulty) == 1 && offsetof(D2GSPacketSrv01, dwArenaFlags) == 2
              && offsetof(D2GSPacketSrv01, nExpansion) == 6 && offsetof(D2GSPacketSrv01, nLadder) == 7,
              "D2GSPacketSrv01: [esi+1], [esi+2], [esi+6], [esi+7] at 0x6FAB2134..0x6FAB2149");
static_assert(offsetof(D2GSPacketSrv03, nAct) == 1 && offsetof(D2GSPacketSrv03, dwGameSeed) == 2
              && offsetof(D2GSPacketSrv03, nTownLevelId) == 6 && offsetof(D2GSPacketSrv03, nObjectSeed) == 8,
              "D2GSPacketSrv03: [ecx+1], [ecx+2], a word at [ecx+6], [ecx+8] at 0x6FAB2160..0x6FAB216E");
static_assert(offsetof(D2GSPacketSrv07, nX) == 1 && offsetof(D2GSPacketSrv07, nY) == 3
              && offsetof(D2GSPacketSrv07, nLevelId) == 5
              && offsetof(D2GSPacketSrv08, nX) == 1 && offsetof(D2GSPacketSrv08, nY) == 3
              && offsetof(D2GSPacketSrv08, nLevelId) == 5,
              "D2GSPacketSrv07/08: words at [ebx+1] and [ebx+3], a byte at [ebx+5]");
static_assert(offsetof(D2GSPacketSrv09, nUnitType) == 1 && offsetof(D2GSPacketSrv09, dwUnitGUID) == 2
              && offsetof(D2GSPacketSrv09, nClassId) == 6 && offsetof(D2GSPacketSrv09, nPosX) == 7
              && offsetof(D2GSPacketSrv09, nPosY) == 9,
              "D2GSPacketSrv09: [eax+1], [eax+2], [eax+6], words at [eax+7] and [eax+9]");
static_assert(offsetof(D2GSPacketSrv0A, nUnitType) == 1 && offsetof(D2GSPacketSrv0A, dwUnitGUID) == 2
              && offsetof(D2GSPacketSrv0B, nUnitType) == 1 && offsetof(D2GSPacketSrv0B, dwUnitGUID) == 2,
              "D2GSPacketSrv0A/0B: a type byte at +1, the GUID at +2");
static_assert(offsetof(D2GSPacketSrv51, nUnitType) == 1 && offsetof(D2GSPacketSrv51, dwUnitGUID) == 2
              && offsetof(D2GSPacketSrv51, nObject) == 6 && offsetof(D2GSPacketSrv51, nPosX) == 8
              && offsetof(D2GSPacketSrv51, nPosY) == 0x0A && offsetof(D2GSPacketSrv51, nObjectMode) == 0x0C
              && offsetof(D2GSPacketSrv51, nInteractType) == 0x0D,
              "D2GSPacketSrv51: the eight loads at 0x6FAB23D0..0x6FAB23F3");
static_assert(GAMETYPE_BNET_BETA == 2 && GAMETYPE_BNET == 3 && GAMETYPE_OBNET_JOIN == 7
              && GAMETYPE_LAN_JOIN == 9,
              "0x06: cmp eax, 7 / 9 / 2 / 3 at 0x6FAB2255..0x6FAB227C");
static_assert(D2JOINERR_Your_connection_has_been_interrupted == 7
              && D2JOINERR_The_Host_of_this_game_has_left == 8,
              "0x06: mov ecx, 8 at 0x6FAB2268 and mov ecx, 7 at 0x6FAB228A");

//1.10f: D2Client.0x6FAB2130
//  Packet 0x01: the game's difficulty, arena flags, expansion and ladder, then the interface is set up. Ends in a tail jump. The difficulty and
//  expansion bytes are passed with the packet pointer's bits above them in the original and the ladder byte zero-extended; the callees store or
//  mask the byte, so ours passes zero-extended bytes to all three.
void __fastcall SCMD_HandleSrv01_GameFlags(D2GSPacketSrv01 *pPacket)
{
    const uint32_t dwArenaFlags = pPacket->dwArenaFlags;
    GAME_SetDifficulty((D2C_Difficulties)pPacket->nDifficulty);
    GAME_SetExpansionGame(pPacket->nExpansion);
    GAME_SetLadderGame(pPacket->nLadder);
    ARENA_SetArenaFlags(dwArenaFlags);
    UI_InitUiSubsystems();
}

//1.10f: D2Client.0x6FAB2160
//  Packet 0x03: the act, the game and object seeds, the town.
void __fastcall SCMD_HandleSrv03_GameSeed(D2GSPacketSrv03 *pPacket)
{
    GAME_PreloadActData(pPacket->nAct, pPacket->dwGameSeed, (int32_t)pPacket->nObjectSeed, pPacket->nTownLevelId);
}

//1.10f: D2Client.0x6FAB2180
//  Packet 0x02, header only: the act is loaded, and the client answers with packet 0x6A. A tail jump.
void __fastcall SCMD_HandleSrv02_ActLoaded(D2GSPacketSrv02 *pPacket)
{
    MSG_Send_D2CLTSYS_JOINACT();
}

//1.10f: D2Client.0x6FAB2190
//  Packet 0x04, header only: loading is done.
void __fastcall SCMD_HandleSrv04_LoadComplete(D2GSPacketSrv04 *pPacket)
{
    gbInGame = 1;
    gbGameExitPending = 0;

    D2ActiveRoomStrc *pRoom = CUNIT_GetPlayerRoom();
    if (!pRoom)
    {
        FOG_DisplayHalt(szCouldNotGetActMsg, szFile, 1319);
        exit(-1);
    }

    //  the act goes in cl and GFX_PreloadOnLoadComplete never reads it
    GFX_PreloadOnLoadComplete(D2COMMON_DRLG_GetActNoFromLevelId((D2C_Levels)D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom)));

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    if (pPlayer && pPlayer->dwUnitType == UNIT_PLAYER && pPlayer->pPlayerData)
    {
        pPlayer->pPlayerData->unk0xA8[42] = 0;
        pPlayer->pPlayerData->unk0xA8[43] = 0;
        pPlayer->pPlayerData->unk0xA8[44] = 0;
        pPlayer->pPlayerData->unk0xA8[45] = 0;
    }
}

//1.10f: D2Client.0x6FAB2220
//  Packet 0x05, header only: the game is left.
void __fastcall SCMD_HandleSrv05_UnloadComplete(D2GSPacketSrv05 *pPacket)
{
    j_ENV_UnloadImages();
    FOG_10144(0);
    gbInGame = 0;
    gbGameExitPending = 1;
    gnConnectionTimestamp = GetTickCount();
}

//1.10f: D2Client.0x6FAB2250
//  Packet 0x06, header only: the other end is gone. gnGameType is read once and again only after the disconnect call it may make (0x6FAB2272).
void __fastcall SCMD_HandleSrv06_GameExit(D2GSPacketSrv06 *pPacket)
{
    if (gnGameType == GAMETYPE_OBNET_JOIN || gnGameType == GAMETYPE_LAN_JOIN)
    {
        if (!GAME_GetLeaveGameSent())
            GAME_BeginDisconnect(D2JOINERR_The_Host_of_this_game_has_left);
    }

    if ((gnGameType == GAMETYPE_BNET_BETA || gnGameType == GAMETYPE_BNET) && gbInGame)
    {
        GAME_BeginDisconnect(D2JOINERR_Your_connection_has_been_interrupted);
    }

    gbDisconnecting = 1;
}

//1.10f: D2Client.0x6FAB22A0
//  Packet 0x07: a map tile comes into sight. Both coordinates are loaded before the assert, which names the act pointer `ghAct`.
void __fastcall SCMD_HandleSrv07_MapReveal(D2GSPacketSrv07 *pPacket)
{
    const uint16_t nX = pPacket->nX;
    const uint16_t nY = pPacket->nY;
    if (!gpAct)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR)szAssertGhAct, (uint32_t)(UINT_PTR)szFile, 1418);
        exit(-1);
    }

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    D2ActiveRoomStrc *pRoom = pPlayer ? D2COMMON_UNITS_GetRoom(pPlayer) : NULL;
    D2COMMON_DUNGEON_SetClientIsInSight(gpAct, (D2C_Levels)pPacket->nLevelId, nX, nY, pRoom);
}

//1.10f: D2Client.0x6FAB2310
//  Packet 0x08: a map tile goes out of sight.
void __fastcall SCMD_HandleSrv08_MapHide(D2GSPacketSrv08 *pPacket)
{
    const uint16_t nX = pPacket->nX;
    const uint16_t nY = pPacket->nY;
    if (!gpAct)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR)szAssertGhAct, (uint32_t)(UINT_PTR)szFile, 1438);
        exit(-1);
    }

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    D2ActiveRoomStrc *pRoom = pPlayer ? D2COMMON_UNITS_GetRoom(pPlayer) : NULL;
    D2COMMON_DUNGEON_UnsetClientIsInSight(gpAct, (D2C_Levels)pPacket->nLevelId, nX, nY, pRoom);
}

//1.10f: D2Client.0x6FAB2380
//  Packet 0x09: a unit appears; a monster's graphics are preloaded by tail jump.
void __fastcall SCMD_HandleSrv09_SpawnUnit(D2GSPacketSrv09 *pPacket)
{
    D2UnitStrc *pUnit = CUNIT_CreateUnit(pPacket->dwUnitGUID, pPacket->nClassId, pPacket->nPosX, pPacket->nPosY, (D2C_UnitTypes)pPacket->nUnitType, 0);
    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER)
        GFX_PreloadUnit(pUnit);
}

//1.10f: D2Client.0x6FAB23D0
//  Packet 0x51: an object appears.
void __fastcall SCMD_HandleSrv51_SpawnObject(D2GSPacketSrv51 *pPacket)
{
    CUNIT_CreateObjectUnit(pPacket->dwUnitGUID, pPacket->nObject, pPacket->nPosX, pPacket->nPosY, (D2C_UnitTypes)pPacket->nUnitType, 0, pPacket->nObjectMode, pPacket->nInteractType);
}

//1.10f: D2Client.0x6FAB2400
//  Packet 0x52: the quest list. A tail jump.
void __fastcall SCMD_HandleSrv52_QuestList(D2GSPacketSrv52 *pPacket)
{
    QUESTLOG_ApplyQuestListPacket(pPacket);
}

//1.10f: D2Client.0x6FAB2410
//  Packet 0x0A: a unit is removed, unless it is the player's own hireling (pet type 7, pettype.txt's hireable). The removal is a tail jump.
void __fastcall SCMD_HandleSrv0A_RemoveUnit(D2GSPacketSrv0A *pPacket)
{
    const uint32_t dwUnitGUID = pPacket->dwUnitGUID;
    if (pPacket->nUnitType == UNIT_MONSTER && dwUnitGUID == (uint32_t)ROSTERPETS_GetAssociatedUnitGUID(CUNIT_GetPlayer(), 7, 1))
    {
        return;
    }
    CUNIT_RemoveUnitById((int32_t)dwUnitGUID, (D2C_UnitTypes)pPacket->nUnitType);
}

//1.10f: D2Client.0x6FAB2450
//  Packet 0x0B: this client's own unit is assigned. Ends in a tail jump to MEMPART_Nullsub1 (one `retn`), called through its slot.
void __fastcall SCMD_HandleSrv0B_SetPlayerUnit(D2GSPacketSrv0B *pPacket)
{
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(pPacket->dwUnitGUID, (D2C_UnitTypes)pPacket->nUnitType);
    if (pUnit)
        CUNIT_SetPlayerUnit(pUnit);
    MEMPART_Nullsub1();
}


//  The system queue and the pfProcessUnit handlers.
//  GAME_UpdateFrame drains D2Net's system packet list the way it drains the game list and hands every dequeue, the -1 that ends the drain
//  included, to SCMD_SaveCharacterFile (its first test is `nLen != -1`); MSG_JoinGameMessageLoop and MSG_LeaveGameMessageLoop call it too.
//  The pfProcessUnit handlers (rows 0x0C..0x10, 0x17, 0x4C, 0x4D and 0x67..0x6D) unpack the packet into a seven-int block and call
//  UNITMODE_DispatchActionByUnitType(nAction, pUnit, block, TRUE), which switches on the unit's type and forwards the fourth argument to
//  PLAYER_DoClientAction alone. The literal 1 is bInterruptKnockback: a player in a knockback takes the action only with it set.
//  0x0C, 0x4C and 0x4D clear the block first (`rep stosd`); the other twelve write some slots and leave the rest as the stack held them, and so
//  does ours.
//    0x0C..0x10  a unit type at +1, its GUID at +2, the action at +6
//    0x67..0x6D  monsters only: the GUID at +1 and the action at +5 (the router's own arm for this range finds the unit)
//    0x4C, 0x4D  a skill: action 22 on a unit, 21 at a point
//    0x17        SCMD_HandleSrv17_ApplyRemoteUnitAction
//  0x0D, 0x0F, 0x69 and 0x6B aim at a point, 0x0E, 0x10, 0x6A and 0x6C at a unit, and 0x0F, 0x10, 0x6B and 0x6C first put the unit at the
//  packet's position with UNITMODE_SyncUnitPosition (after unpacking in the first two, before in the second two). 0x67 and 0x68 carry the
//  monster's path type and velocity (a signed word, `movsx`) where 0x69..0x6C write the constants 2, 0 and 4; 0x68 also syncs.
//  The original's strings: szFileModeWriteBinary and szAssertHUnit are SCmd.h's.
static const char szAssertPlrName[] = "gszPlrName[0] != '\\0'";      // 0x6FB75EE8
static const char szSaveFileFormat[] = "%s%s.d2s";                  // 0x6FB75EDC
static_assert(sizeof(szAssertPlrName) == 21 + 1 && sizeof(szSaveFileFormat) == 8 + 1,
              "the two strings at 0x6FB75EE8 and 0x6FB75EDC, measured in the DLL");

static_assert(D2JOINERR_Bad_character_version == 0 && D2JOINERR_Unable_to_enter_game_generic == 5
              && D2JOINERR_unknown_failure == 9 && D2JOINERR_Bad_inventory_data == 10
              && D2JOINERR_dead_hardcore_character == 24 && D2JOINERR_NORMAL_CANT_JOIN_EXP_GAME == 25
              && D2JOINERR_Unable_to_enter_game == 28,
              "0xB3: xor ecx, ecx at 0x6FAB1EAB, mov ecx, 5 / 0Ah / 18h / 19h / 1Ch and the default's 9");
static_assert(STAT_PIERCE_IDX == 0x148, "0x6D: push 148h at 0x6FAB28F4 and 0x6FAB2901");
static_assert(UNIT_PLAYER == 0, "0x0D: test al, al on the unit type at 0x6FAB24EF");

static_assert(sizeof(D2GSPacketSrv0C) == 9 && sizeof(D2GSPacketSrv0D) == 13 && sizeof(D2GSPacketSrv0E) == 12
              && sizeof(D2GSPacketSrv0F) == 16 && sizeof(D2GSPacketSrv10) == 16
              && sizeof(D2GSPacketSrv4C) == 16 && sizeof(D2GSPacketSrv4D) == 17
              && sizeof(D2GSPacketSrv67) == 16 && sizeof(D2GSPacketSrv68) == 21
              && sizeof(D2GSPacketSrv69) == 12 && sizeof(D2GSPacketSrv6A) == 12
              && sizeof(D2GSPacketSrv6B) == 16 && sizeof(D2GSPacketSrv6C) == 16
              && sizeof(D2GSPacketSrv6D) == 10,
              "the fifteen rows' nCmdSize in gSCmdTable, 0x17's -1 aside");
static_assert(offsetof(D2GSPacketSrv0C, nAction) == 6 && offsetof(D2GSPacketSrv0C, nHitClass) == 7
              && offsetof(D2GSPacketSrv0C, nLife) == 8,
              "0x0C: [edx+7], [edx+8], [edx+6] at 0x6FAB2484..0x6FAB2496");
static_assert(offsetof(D2GSPacketSrv0D, nUnitType) == 1 && offsetof(D2GSPacketSrv0D, dwUnitGUID) == 2
              && offsetof(D2GSPacketSrv0D, nAction) == 6 && offsetof(D2GSPacketSrv0D, nX) == 7
              && offsetof(D2GSPacketSrv0D, nY) == 9 && offsetof(D2GSPacketSrv0D, nHitType) == 0x0B
              && offsetof(D2GSPacketSrv0D, nUnitLifePct) == 0x0C,
              "0x0D: words at [esi+7] and [esi+9], [esi+0Bh], [esi+2], [esi+6], [esi+1], [esi+0Ch]");
static_assert(offsetof(D2GSPacketSrv0E, nAction) == 6 && offsetof(D2GSPacketSrv0E, nTargetType) == 7
              && offsetof(D2GSPacketSrv0E, dwTargetGUID) == 8,
              "0x0E: [eax+7], [eax+6], [eax+8] at 0x6FAB251A..0x6FAB2524");
static_assert(offsetof(D2GSPacketSrv0F, nAction) == 6 && offsetof(D2GSPacketSrv0F, nDestPosX) == 7
              && offsetof(D2GSPacketSrv0F, nDestPosY) == 9 && offsetof(D2GSPacketSrv0F, unk0x00B) == 0x0B
              && offsetof(D2GSPacketSrv0F, nCurrentPosX) == 0x0C
              && offsetof(D2GSPacketSrv0F, nCurrentPosY) == 0x0E,
              "0x0F: [esi+7], [esi+9], [esi+0Bh], [esi+0Eh], [esi+0Ch], [esi+6]");
static_assert(offsetof(D2GSPacketSrv10, nAction) == 6 && offsetof(D2GSPacketSrv10, nTargetType) == 7
              && offsetof(D2GSPacketSrv10, dwTargetGUID) == 8 && offsetof(D2GSPacketSrv10, nCurrentPosX) == 0x0C
              && offsetof(D2GSPacketSrv10, nCurrentPosY) == 0x0E,
              "0x10: [esi+8], [esi+0Eh], [esi+7], [esi+0Ch], [esi+6]");
static_assert(offsetof(D2GSPacketSrv4C, nUnitType) == 1 && offsetof(D2GSPacketSrv4C, dwUnitGUID) == 2
              && offsetof(D2GSPacketSrv4C, nSkillId) == 6 && offsetof(D2GSPacketSrv4C, nSkillLevel) == 8
              && offsetof(D2GSPacketSrv4C, nTargetType) == 9 && offsetof(D2GSPacketSrv4C, dwTargetGUID) == 0x0A,
              "0x4C: a word at [edx+6], [edx+9], [edx+0Ah], [edx+8] at 0x6FAB37E8..0x6FAB3800");
static_assert(offsetof(D2GSPacketSrv4D, nUnitType) == 1 && offsetof(D2GSPacketSrv4D, dwUnitGUID) == 2
              && offsetof(D2GSPacketSrv4D, nSkillId) == 6 && offsetof(D2GSPacketSrv4D, nSkillLevel) == 0x0A
              && offsetof(D2GSPacketSrv4D, nX) == 0x0B && offsetof(D2GSPacketSrv4D, nY) == 0x0D,
              "0x4D: [edx+6], words at [edx+0Bh] and [edx+0Dh], [edx+0Ah] at 0x6FAB3848..0x6FAB3861");
static_assert(offsetof(D2GSPacketSrv67, nAction) == 5 && offsetof(D2GSPacketSrv67, nX) == 6
              && offsetof(D2GSPacketSrv67, nY) == 8 && offsetof(D2GSPacketSrv67, unk0x0A) == 0x0A
              && offsetof(D2GSPacketSrv67, unk0x0B) == 0x0B && offsetof(D2GSPacketSrv67, nPathType) == 0x0C
              && offsetof(D2GSPacketSrv67, nVelocity) == 0x0D && offsetof(D2GSPacketSrv67, unk0x0F) == 0x0F,
              "0x67: the eight loads at 0x6FAB25FA..0x6FAB2636");
static_assert(offsetof(D2GSPacketSrv68, nAction) == 5 && offsetof(D2GSPacketSrv68, nX) == 6
              && offsetof(D2GSPacketSrv68, nY) == 8 && offsetof(D2GSPacketSrv68, nTargetType) == 0x0A
              && offsetof(D2GSPacketSrv68, dwTargetGUID) == 0x0B && offsetof(D2GSPacketSrv68, unk0x0F) == 0x0F
              && offsetof(D2GSPacketSrv68, unk0x10) == 0x10 && offsetof(D2GSPacketSrv68, nPathType) == 0x11
              && offsetof(D2GSPacketSrv68, nVelocity) == 0x12 && offsetof(D2GSPacketSrv68, unk0x14) == 0x14,
              "0x68: the ten loads at 0x6FAB2659..0x6FAB26B1");
static_assert(offsetof(D2GSPacketSrv69, nUnitGUID) == 1 && offsetof(D2GSPacketSrv69, nAction) == 5
              && offsetof(D2GSPacketSrv69, nX) == 6 && offsetof(D2GSPacketSrv69, nY) == 8
              && offsetof(D2GSPacketSrv69, unk0x0A) == 0x0A && offsetof(D2GSPacketSrv69, unk0x0B) == 0x0B,
              "0x69: [eax+6], [eax+5], [eax+8], [eax+0Ah], [eax+0Bh] at 0x6FAB26DA..0x6FAB2704");
static_assert(offsetof(D2GSPacketSrv6A, nAction) == 5 && offsetof(D2GSPacketSrv6A, nTargetUnitType) == 6
              && offsetof(D2GSPacketSrv6A, nTargetUnitGuid) == 7 && offsetof(D2GSPacketSrv6A, nDirection) == 0x0B,
              "0x6A: [eax+6], [eax+5], [eax+7], [eax+0Bh] at 0x6FAB273A..0x6FAB274D");
static_assert(offsetof(D2GSPacketSrv6B, nUnitGUID) == 1 && offsetof(D2GSPacketSrv6B, nAction) == 5
              && offsetof(D2GSPacketSrv6B, nDestPosX) == 6 && offsetof(D2GSPacketSrv6B, nDestPosY) == 8
              && offsetof(D2GSPacketSrv6B, unk0x0A) == 0x0A && offsetof(D2GSPacketSrv6B, unk0x0B) == 0x0B
              && offsetof(D2GSPacketSrv6B, nCurrentPosX) == 0x0C && offsetof(D2GSPacketSrv6B, nCurrentPosY) == 0x0E,
              "0x6B: [esi+0Eh], [esi+0Ch], then [esi+6], [esi+8], [esi+0Bh], [esi+0Ah], [esi+5]");
static_assert(offsetof(D2GSPacketSrv6C, nUnitGUID) == 1 && offsetof(D2GSPacketSrv6C, nAction) == 5
              && offsetof(D2GSPacketSrv6C, nTargetType) == 6 && offsetof(D2GSPacketSrv6C, dwTargetGUID) == 7
              && offsetof(D2GSPacketSrv6C, unk0x0B) == 0x0B && offsetof(D2GSPacketSrv6C, nCurrentPosX) == 0x0C
              && offsetof(D2GSPacketSrv6C, nCurrentPosY) == 0x0E,
              "0x6C: [esi+0Eh], [esi+0Ch], then [esi+7], [esi+6], [esi+0Bh], [esi+5]");
static_assert(offsetof(D2GSPacketSrv6D, nUnitGUID) == 1 && offsetof(D2GSPacketSrv6D, nX) == 5
              && offsetof(D2GSPacketSrv6D, nY) == 7 && offsetof(D2GSPacketSrv6D, unk0x09) == 9,
              "0x6D: words at [eax+5] and [eax+7], a byte at [eax+9] at 0x6FAB2898..0x6FAB28BF");

//  The original inlines this into SCMD_SaveCharacterFile, starting at the player-name assert and ending at the close; each early return is the
//  binary's jump to the free.
static void SCMD_WriteCharacterFile()
{
    char szFilePath[500];
    char szSavePath[1024];

    if (!gLocalPlayerName[0])
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR)szAssertPlrName, (uint32_t)(UINT_PTR)szFile, 37);
        exit(-1);
    }

    const uint32_t nSize = (uint32_t)gnScmdBufferSize;
    const uint8_t *pData = (const uint8_t *)gpScmdBuffer;
    if (nSize < 8 || *(const uint32_t *)pData != 0xAA55AA55)
        return;

    if (*(const uint32_t *)(pData + 4) > 0x5B)
    {
        if (nSize < 0x14F)
            return;
        gpLaunchConfig->unpackedCTemp.nCharacterSaveFlags = *(const uint16_t *)(pData + 0x24);
    }
    else
    {
        if (nSize < 0x82)
            return;
        gpLaunchConfig->unpackedCTemp.nCharacterSaveFlags = *(const uint16_t *)(pData + 0x18);
    }

    //  BUG: the path is built into a 500-byte buffer (szFilePath, 500 bytes below szSavePath) by an unbounded sprintf from a save path fetched with a
    //  limit of 1024.
    FOG_GetSavePath((uint32_t)(UINT_PTR)szSavePath, 1024);
    sprintf(szFilePath, szSaveFileFormat, szSavePath, gLocalPlayerName);

    //  DIFFERENCE: the file goes through the C runtime's fopen, fwrite and fclose; the original calls its own VC6 runtime's (0x6FB60F4F, 0x6FB60F62,
    //  0x6FB60DE8), and the stream is opened and closed here.
    FILE *pStream = fopen(szFilePath, szFileModeWriteBinary);
    if (pStream)
    {
        fwrite(gpScmdBuffer, gnScmdBufferSize, 1, pStream);
        fclose(pStream);
    }
}

//1.10f: D2Client.0x6FAB1CB0
//  Every packet of D2Net's system queue, four ids and nothing else (the other two fall to the default):
//    0xAE  gbConnectedToGame = 1
//    0xAF  gbConnectedToGame = 0
//    0xB2  one piece of the character's .d2s file
//    0xB3  the server refuses the character: a dword reason at +1, turned into a D2JOINERR for GAME_BeginDisconnect. Reasons 1..6 give 0..5,
//          7..21 give 10..24, 23..26 give 25..28, and 22 and everything else give 9, "unknown failure", so 6, 7 and 8 (failed to join,
//          connection interrupted, host gone) never come from here.
//  0xB2 carries the chunk's length in byte +1, a restart flag in +2, the whole file's size as a dword at +3, and the chunk from +7. With no
//  buffer, or with the flag set (the old buffer freed first), a buffer of the whole size is allocated; the chunk is copied in at
//  gnScmdBufferSize, which then grows by the length; and only when it equals the size is the file written and the buffer freed.
//  What is written is checked first: under eight bytes, or without the magic 0xAA55AA55 at +0, nothing is written. Above version 0x5B the file
//  must hold 0x14F bytes and the word at +0x24 goes into gpLaunchConfig->unpackedCTemp.nCharacterSaveFlags; at 0x5B or below the file needs 0x82
//  bytes and the word comes from +0x18.
void __fastcall SCMD_SaveCharacterFile(uint8_t *pBuffer, int32_t nLen)
{
    if (nLen == -1)
        return;

    switch (pBuffer[0])
    {
    case 0xAE:
        gbConnectedToGame = 1;
        return;

    case 0xAF:
        gbConnectedToGame = 0;
        return;

    case 0xB2:
    {
        if (gpScmdBuffer && pBuffer[2])
        {
            FOG_Free((uint32_t)(UINT_PTR)gpScmdBuffer, (uint32_t)(UINT_PTR)szFile, 91, 0);
            gpScmdBuffer = NULL;
            gnScmdBufferSize = 0;
        }
        if (!gpScmdBuffer)
            gpScmdBuffer = (void *)(UINT_PTR)FOG_Alloc(*(uint32_t *)(pBuffer + 3), (uint32_t)(UINT_PTR)szFile, 98, 0);

        //  BUG: nothing compares the copy with the allocation. The buffer is the size of the chunk that allocated it; a later chunk without the
        //  restart flag is copied at gnScmdBufferSize whatever its own total says, and the only test is the equality after the copy. Chunks that pass
        //  the total write past the block, and a sum that steps over the total never equals it, so the buffer is then kept until a restart.
        uint8_t *pDest = (uint8_t *)gpScmdBuffer + gnScmdBufferSize;
        for (int32_t i = 0; i < pBuffer[1]; ++i)
            pDest[i] = pBuffer[7 + i];

        gnScmdBufferSize += pBuffer[1];
        if (*(uint32_t *)(pBuffer + 3) != (uint32_t)gnScmdBufferSize)
            return;

        SCMD_WriteCharacterFile();

        FOG_Free((uint32_t)(UINT_PTR)gpScmdBuffer, (uint32_t)(UINT_PTR)szFile, 113, 0);
        gpScmdBuffer = NULL;
        gnScmdBufferSize = 0;
        return;
    }

    case 0xB3:
        switch (*(int32_t *)(pBuffer + 1))
        {
        case 1:  GAME_BeginDisconnect(D2JOINERR_Bad_character_version); return;
        case 2:  GAME_BeginDisconnect(D2JOINERR_Bad_character_quest); return;
        case 3:  GAME_BeginDisconnect(D2JOINERR_Bad_character_waypoint); return;
        case 4:  GAME_BeginDisconnect(D2JOINERR_Bad_character_stats); return;
        case 5:  GAME_BeginDisconnect(D2JOINERR_Bad_character_skill); return;
        case 6:  GAME_BeginDisconnect(D2JOINERR_Unable_to_enter_game_generic); return;
        case 7:  GAME_BeginDisconnect(D2JOINERR_Bad_inventory_data); return;
        case 8:  GAME_BeginDisconnect(D2JOINERR_bad_dead_bodies); return;
        case 9:  GAME_BeginDisconnect(D2JOINERR_bad_header); return;
        case 10: GAME_BeginDisconnect(D2JOINERR_bad_hireables); return;
        case 11: GAME_BeginDisconnect(D2JOINERR_bad_intro_data); return;
        case 12: GAME_BeginDisconnect(D2JOINERR_bad_item); return;
        case 13: GAME_BeginDisconnect(D2JOINERR_bad_dead_body_item); return;
        case 14: GAME_BeginDisconnect(D2JOINERR_generic_bad_file); return;
        case 15: GAME_BeginDisconnect(D2JOINERR_Game_is_full); return;
        case 16: GAME_BeginDisconnect(D2JOINERR_Versions_do_not_match); return;
        case 17: GAME_BeginDisconnect(D2JOINERR_NightmareNotUnlocked); return;
        case 18: GAME_BeginDisconnect(D2JOINERR_HellNotUnlocked); return;
        case 19: GAME_BeginDisconnect(D2JOINERR_A_normal_character_cannot_join_hardcore); return;
        case 20: GAME_BeginDisconnect(D2JOINERR_A_hardcore_character_cannot_join_normal); return;
        case 21: GAME_BeginDisconnect(D2JOINERR_dead_hardcore_character); return;
        case 23: GAME_BeginDisconnect(D2JOINERR_NORMAL_CANT_JOIN_EXP_GAME); return;
        case 24: GAME_BeginDisconnect(D2JOINERR_EXP_CANT_JOIN_NORMAL_GAME); return;
        case 25: GAME_BeginDisconnect(D2JOINERR_failed_to_join_game_27); return;
        case 26: GAME_BeginDisconnect(D2JOINERR_Unable_to_enter_game); return;
        default: GAME_BeginDisconnect(D2JOINERR_unknown_failure); return;
        }

    default:
        return;
    }
}

//1.10f: D2Client.0x6FAB2470
//  Row 0x0C: an action with two loose bytes, the block cleared.
void __fastcall SCMD_HandleSrv0C_Action(D2UnitStrc *pUnit, D2GSPacketSrv0C *pPacket)
{
    int32_t pActionParams[7];
    memset(pActionParams, 0, sizeof(pActionParams));
    pActionParams[0] = pPacket->nHitClass;
    pActionParams[1] = pPacket->nLife;
    UNITMODE_DispatchActionByUnitType((D2C_ClientActions)pPacket->nAction,pUnit, pActionParams, TRUE);
}

//1.10f: D2Client.0x6FAB24B0
//  Row 0x0D: an action at a point, and a player's life for the party panel (ROSTER_SetPartyLife when the unit is a player, the GUID loaded
//  before the dispatch and the type after it).
void __fastcall SCMD_HandleSrv0D_ActionAtXY(D2UnitStrc *pUnit, D2GSPacketSrv0D *pPacket)
{
    int32_t pActionParams[7];
    pActionParams[0] = pPacket->nX;
    pActionParams[1] = pPacket->nY;
    pActionParams[2] = pPacket->nHitType;
    const uint32_t dwUnitGUID = pPacket->dwUnitGUID;
    UNITMODE_DispatchActionByUnitType((D2C_ClientActions)pPacket->nAction,pUnit, pActionParams, TRUE);
    if (pPacket->nUnitType == UNIT_PLAYER)
        ROSTER_SetPartyLife(dwUnitGUID, pPacket->nUnitLifePct);
}

//1.10f: D2Client.0x6FAB2510
//  Row 0x0E: an action on a unit.
void __fastcall SCMD_HandleSrv0E_ActionOnTarget(D2UnitStrc *pUnit, D2GSPacketSrv0E *pPacket)
{
    int32_t pActionParams[7];
    pActionParams[0] = pPacket->nTargetType;
    pActionParams[1] = pPacket->dwTargetGUID;
    UNITMODE_DispatchActionByUnitType((D2C_ClientActions)pPacket->nAction,pUnit, pActionParams, TRUE);
}

//1.10f: D2Client.0x6FAB2540
//  Row 0x0F: an action at a point, from the packet's position.
void __fastcall SCMD_HandleSrv0F_ActionAtXYFromPos(D2UnitStrc *pUnit, D2GSPacketSrv0F *pPacket)
{
    int32_t pActionParams[7];
    pActionParams[0] = pPacket->nDestPosX;
    pActionParams[1] = pPacket->nDestPosY;
    pActionParams[2] = pPacket->unk0x00B;
    UNITMODE_SyncUnitPosition(pUnit, pPacket->nCurrentPosX, pPacket->nCurrentPosY, 0, 0, 0);
    UNITMODE_DispatchActionByUnitType((D2C_ClientActions)pPacket->nAction,pUnit, pActionParams, TRUE);
}

//1.10f: D2Client.0x6FAB25A0
//  Row 0x10: an action on a unit, from the packet's position.
void __fastcall SCMD_HandleSrv10_ActionOnTargetFromPos(D2UnitStrc *pUnit, D2GSPacketSrv10 *pPacket)
{
    int32_t pActionParams[7];
    pActionParams[1] = pPacket->dwTargetGUID;
    pActionParams[0] = pPacket->nTargetType;
    UNITMODE_SyncUnitPosition(pUnit, pPacket->nCurrentPosX, pPacket->nCurrentPosY, 0, 0, 0);
    UNITMODE_DispatchActionByUnitType((D2C_ClientActions)pPacket->nAction,pUnit, pActionParams, TRUE);
}

//1.10f: D2Client.0x6FAB25F0
//  Row 0x67: a monster's action at a point, with its path type and velocity. Slot 6 is +0x0B, filled last.
void __fastcall SCMD_HandleSrv67_ActionAtXYPathed(D2UnitStrc *pUnit, D2GSPacketSrv67 *pPacket)
{
    int32_t pActionParams[7];
    pActionParams[0] = (uint16_t)pPacket->nX;
    pActionParams[1] = (uint16_t)pPacket->nY;
    pActionParams[2] = (uint8_t)pPacket->unk0x0A;
    pActionParams[3] = (uint8_t)pPacket->nPathType;
    pActionParams[4] = pPacket->nVelocity;
    pActionParams[5] = (uint8_t)pPacket->unk0x0F;
    pActionParams[6] = (uint8_t)pPacket->unk0x0B;
    UNITMODE_DispatchActionByUnitType((D2C_ClientActions)pPacket->nAction,pUnit, pActionParams, TRUE);
}

//1.10f: D2Client.0x6FAB2650
//  Row 0x68: a monster's action on a unit, with its path type and velocity, from the packet's position. Slot 6 is +0x10.
void __fastcall SCMD_HandleSrv68_ActionOnTargetPathedFromPos(D2UnitStrc *pUnit, D2GSPacketSrv68 *pPacket)
{
    int32_t pActionParams[7];
    pActionParams[0] = (uint8_t)pPacket->nTargetType;
    pActionParams[1] = pPacket->dwTargetGUID;
    pActionParams[2] = (uint8_t)pPacket->unk0x0F;
    pActionParams[3] = (uint8_t)pPacket->nPathType;
    pActionParams[4] = pPacket->nVelocity;
    pActionParams[5] = (uint8_t)pPacket->unk0x14;
    pActionParams[6] = (uint8_t)pPacket->unk0x10;
    UNITMODE_SyncUnitPosition(pUnit, (uint16_t)pPacket->nX, (uint16_t)pPacket->nY, 0, 0, 0);
    UNITMODE_DispatchActionByUnitType((D2C_ClientActions)pPacket->nAction,pUnit, pActionParams, TRUE);
}

//1.10f: D2Client.0x6FAB26D0
//  Row 0x69: a monster's action at a point.
void __fastcall SCMD_HandleSrv69_ActionAtXY(D2UnitStrc *pUnit, D2GSPacketSrv69 *pPacket)
{
    int32_t pActionParams[7];
    pActionParams[0] = pPacket->nX;
    pActionParams[1] = pPacket->nY;
    pActionParams[3] = 2;
    pActionParams[2] = pPacket->unk0x0A;
    pActionParams[4] = 0;
    pActionParams[6] = pPacket->unk0x0B;
    pActionParams[5] = 4;
    UNITMODE_DispatchActionByUnitType((D2C_ClientActions)pPacket->nAction,pUnit, pActionParams, TRUE);
}

//1.10f: D2Client.0x6FAB2730
//  Row 0x6A: a monster's action on a unit.
void __fastcall SCMD_HandleSrv6A_ActionOnTarget(D2UnitStrc *pUnit, D2GSPacketSrv6A *pPacket)
{
    int32_t pActionParams[7];
    pActionParams[0] = pPacket->nTargetUnitType;
    pActionParams[1] = pPacket->nTargetUnitGuid;
    pActionParams[2] = pPacket->nDirection;
    pActionParams[3] = 2;
    pActionParams[4] = 0;
    pActionParams[5] = 4;
    UNITMODE_DispatchActionByUnitType((D2C_ClientActions)pPacket->nAction,pUnit, pActionParams, TRUE);
}

//1.10f: D2Client.0x6FAB2790
//  Row 0x6B: a monster's action at a point, from the packet's position; the sync comes first.
void __fastcall SCMD_HandleSrv6B_ActionAtXYFromPos(D2UnitStrc *pUnit, D2GSPacketSrv6B *pPacket)
{
    int32_t pActionParams[7];
    UNITMODE_SyncUnitPosition(pUnit, pPacket->nCurrentPosX, pPacket->nCurrentPosY, 0, 0, 0);
    pActionParams[0] = pPacket->nDestPosX;
    pActionParams[1] = pPacket->nDestPosY;
    pActionParams[6] = pPacket->unk0x0B;
    pActionParams[2] = pPacket->unk0x0A;
    pActionParams[3] = 2;
    pActionParams[4] = 0;
    pActionParams[5] = 4;
    UNITMODE_DispatchActionByUnitType((D2C_ClientActions)pPacket->nAction,pUnit, pActionParams, TRUE);
}

//1.10f: D2Client.0x6FAB2810
//  Row 0x6C: a monster's action on a unit, from the packet's position; the sync comes first.
void __fastcall SCMD_HandleSrv6C_ActionOnTargetFromPos(D2UnitStrc *pUnit, D2GSPacketSrv6C *pPacket)
{
    int32_t pActionParams[7];
    UNITMODE_SyncUnitPosition(pUnit, pPacket->nCurrentPosX, pPacket->nCurrentPosY, 0, 0, 0);
    pActionParams[0] = pPacket->nTargetType;
    pActionParams[1] = pPacket->dwTargetGUID;
    pActionParams[2] = pPacket->unk0x0B;
    pActionParams[3] = 2;
    pActionParams[4] = 0;
    pActionParams[5] = 4;
    UNITMODE_DispatchActionByUnitType((D2C_ClientActions)pPacket->nAction,pUnit, pActionParams, TRUE);
}

//1.10f: D2Client.0x6FAB2880
//  Row 0x6D: action 7 at a point, one more pierce: both arguments are null-checked, and STAT_PIERCE_IDX is raised by one from its base value
//  between the sync and the dispatch.
void __fastcall SCMD_HandleSrv6D_PierceAtXY(D2UnitStrc *pUnit, D2GSPacketSrv6D *pPacket)
{
    if (!pUnit || !pPacket)
        return;

    int32_t pActionParams[7];
    const uint16_t nX = pPacket->nX;
    const uint16_t nY = pPacket->nY;
    pActionParams[0] = nX;
    pActionParams[1] = nY;
    pActionParams[2] = pPacket->unk0x09;
    pActionParams[3] = 2;
    pActionParams[4] = 0;
    pActionParams[5] = 4;
    pActionParams[6] = 0;
    UNITMODE_SyncUnitPosition(pUnit, nX, nY, 0, 0, 0);
    D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_PIERCE_IDX, D2COMMON_STATLIST_GetUnitBaseStat(pUnit, STAT_PIERCE_IDX, 0) + 1, 0);
    UNITMODE_DispatchActionByUnitType(CLTACTION_NEUTRAL, pUnit, pActionParams, TRUE);
}

//1.10f: D2Client.0x6FAB2A80
//  Row 0x17: another unit's action, and a read through two 16-bit values. It skips the local player, asserts the unit, takes the action from +7
//  and slot 0 from +6 shifted right once.
//  BUG: the words at +8 and +0x0A are loaded zero-extended and each is READ THROUGH AS AN ADDRESS (`mov cx, [eax]` at 0x6FAB2AC2,
//  `mov ax, [edx]` at 0x6FAB2ACA), for a two-word local whose address goes in slot 1. A value below 0x10000 is never a mapped address on
//  Windows, so the first read faults every time the handler runs. Reproduced as written.
void __fastcall SCMD_HandleSrv17_ApplyRemoteUnitAction(D2UnitStrc *pUnit, uint8_t *pPacket)
{
    if (pUnit == CUNIT_GetPlayer())
        return;

    if (!pUnit)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR)szAssertHUnit, (uint32_t)(UINT_PTR)szFile, 2113);
        exit(-1);
    }

    const uint16_t wFirstAddress = *(uint16_t *)(pPacket + 8);
    const uint16_t wSecondAddress = *(uint16_t *)(pPacket + 10);

    //  the fault, written as the original wrote it, deliberately
    uint16_t wDerefPair[2];
    wDerefPair[0] = *(uint16_t *)(UINT_PTR)wFirstAddress;
    wDerefPair[1] = *(uint16_t *)(UINT_PTR)wSecondAddress;

    int32_t pActionParams[7];
    pActionParams[0] = pPacket[6] >> 1;
    pActionParams[1] = (int32_t)(UINT_PTR)wDerefPair;
    UNITMODE_DispatchActionByUnitType((D2C_ClientActions)pPacket[7], pUnit, pActionParams, TRUE);
}

//1.10f: D2Client.0x6FAB37D0
//  Row 0x4C: a skill on a unit, action 22. The skill goes in slot 0, -1 in slot 1, the target in 2 and 3 and the level in 4.
void __fastcall SCMD_HandleSrv4C_UnitAction(D2UnitStrc *pUnit, D2GSPacketSrv4C *pPacket)
{
    if (!pUnit)
        return;

    int32_t pActionParams[7];
    memset(pActionParams, 0, sizeof(pActionParams));
    pActionParams[0] = pPacket->nSkillId;
    pActionParams[2] = pPacket->nTargetType;
    pActionParams[4] = pPacket->nSkillLevel;
    pActionParams[1] = -1;
    pActionParams[3] = pPacket->dwTargetGUID;
    UNITMODE_DispatchActionByUnitType(CLTACTION_SKILL_UNIT, pUnit, pActionParams, TRUE);
}

//1.10f: D2Client.0x6FAB3830
//  Row 0x4D: a skill at a point, action 21; the same slots as 0x4C.
void __fastcall SCMD_HandleSrv4D_UnitAction(D2UnitStrc *pUnit, D2GSPacketSrv4D *pPacket)
{
    if (!pUnit)
        return;

    int32_t pActionParams[7];
    memset(pActionParams, 0, sizeof(pActionParams));
    pActionParams[0] = pPacket->nSkillId;
    pActionParams[2] = pPacket->nX;
    pActionParams[4] = pPacket->nSkillLevel;
    pActionParams[1] = -1;
    pActionParams[3] = pPacket->nY;
    UNITMODE_DispatchActionByUnitType(CLTACTION_SKILL_XY, pUnit, pActionParams, TRUE);
}


//  Packets 0x11, 0x15, 0x16, 0x9E..0xA2, 0xA3, 0xA4 and 0xA5: overlays, warps, positions, a monster's stats and skill effects, and
//  SCMD_RemergeItemStatLists. The original's strings szIllegalPacketWarpXYMsg and szAssertNumUpdates are SCmd.h's.

static_assert(STAT_LEVEL == 0x0C, "0x9E..0xA2: cmp edi, 0Ch at 0x6FAB2CFE");
static_assert(STATE_SKILL_MOVE == 0x12, "0xA5: push 12h at 0x6FAB2E91, 0x6FAB2EA5, 0x6FAB2EB9");
static_assert(SKILLSFLAGINDEX_PROGRESSIVE == 2, "0xA3: test [ebx+8], al at 0x6FAB2DD8 -- gdwBitMasks[2]");
static_assert(UNIT_MONSTER == 1 && UNIT_ITEM == 4,
              "0x9E..0xA2: mov edx, 1 at 0x6FAB2CEE; the remerge: cmp dword ptr [esi], 4 at 0x6FAB2C34");

static_assert(offsetof(D2UnitStrc, dwUnitId) == 0x0C && offsetof(D2UnitStrc, nAct) == 0x18
              && offsetof(D2UnitStrc, pDrlgAct) == 0x1C && offsetof(D2UnitStrc, pDynamicPath) == 0x2C
              && offsetof(D2UnitStrc, pInventory) == 0x60 && offsetof(D2UnitStrc, pSkills) == 0xA8,
              "D2UnitStrc: [ebx+0Ch], [esi+18h], [esi+1Ch], [esi+2Ch], [ebx+60h], [ebx+0A8h]");
static_assert(offsetof(D2SkillListStrc, pLeftSkill) == 0x08 && offsetof(D2SkillListStrc, pRightSkill) == 0x0C
              && offsetof(D2SkillStrc, pSkillsTxt) == 0 && offsetof(D2SkillsTxt, nSkillId) == 0,
              "the remerge: [ecx+8], [ecx+0Ch], then movsx word ptr [eax] through the record");
static_assert(offsetof(D2SkillsTxt, dwFlags) == 4 && offsetof(D2SkillsTxt, wSrvDoFunc) == 0x2E,
              "0xA3: mov al, [eax+4] at 0x6FAB2DD5; 0xA5: movsx eax, word ptr [eax+2Eh] at 0x6FAB2E6D");

static_assert(sizeof(D2GSPacketSrv11) == 8 && sizeof(D2GSPacketSrv15) == 11
              && sizeof(D2GSPacketSrv9E) == 7 && sizeof(D2GSPacketSrv9F) == 8 && sizeof(D2GSPacketSrvA0) == 10
              && sizeof(D2GSPacketSrvA1) == 7 && sizeof(D2GSPacketSrvA2) == 8
              && sizeof(D2GSPacketSrvA3) == 24 && sizeof(D2GSPacketSrvA4) == 3 && sizeof(D2GSPacketSrvA5) == 8
              && sizeof(D2ClientUnitUpdateStrc) == 9,
              "the rows' nCmdSize in gSCmdTable, 0x16's -1 aside; 0x16's stride, add esi, 9 at 0x6FAB2B65");
static_assert(offsetof(D2GSPacketSrv11, nUnitType) == 1 && offsetof(D2GSPacketSrv11, dwUnitGUID) == 2
              && offsetof(D2GSPacketSrv11, nOverlayId) == 6,
              "0x11: [eax+2], [eax+1], [eax+6] at 0x6FAB2935..0x6FAB293B");
static_assert(offsetof(D2GSPacketSrv15, nUnitType) == 1 && offsetof(D2GSPacketSrv15, dwUnitGUID) == 2
              && offsetof(D2GSPacketSrv15, nPosX) == 6 && offsetof(D2GSPacketSrv15, nPosY) == 8
              && offsetof(D2GSPacketSrv15, unk0x00A) == 0x0A,
              "0x15: [edi+2], [edi+1], [edi+6], [edi+8], [edi+0Ah]");
static_assert(offsetof(D2GSPacketSrv16, nNumUpdates) == 3 && offsetof(D2GSPacketSrv16, unitUpdate) == 4
              && offsetof(D2ClientUnitUpdateStrc, nUnitType) == 0 && offsetof(D2ClientUnitUpdateStrc, nUnitGUID) == 1
              && offsetof(D2ClientUnitUpdateStrc, nX) == 5 && offsetof(D2ClientUnitUpdateStrc, nY) == 7,
              "0x16: [ecx+3], lea esi, [ecx+4]; [esi+1], [esi+5], [esi+7], [esi]");
static_assert(offsetof(D2GSPacketSrv9E, nStat) == 1 && offsetof(D2GSPacketSrv9E, dwGUID) == 2
              && offsetof(D2GSPacketSrv9E, nValue) == 6 && offsetof(D2GSPacketSrv9F, nValue) == 6
              && offsetof(D2GSPacketSrvA0, nValue) == 6 && offsetof(D2GSPacketSrvA1, nValue) == 6
              && offsetof(D2GSPacketSrvA2, nValue) == 6,
              "0x9E..0xA2: [ecx+1], [ecx+2], and the value at [ecx+6] as byte, word or dword");
static_assert(offsetof(D2GSPacketSrvA3, unk0x01) == 1 && offsetof(D2GSPacketSrvA3, nSkillId) == 2
              && offsetof(D2GSPacketSrvA3, nSkillLevel) == 4 && offsetof(D2GSPacketSrvA3, nUnitType) == 6
              && offsetof(D2GSPacketSrvA3, dwUnitGUID) == 7 && offsetof(D2GSPacketSrvA3, nTargetType) == 0x0B
              && offsetof(D2GSPacketSrvA3, dwTargetGUID) == 0x0C && offsetof(D2GSPacketSrvA3, nX) == 0x10
              && offsetof(D2GSPacketSrvA3, nY) == 0x14,
              "0xA3: the loads at 0x6FAB2D6B..0x6FAB2D9C and [esi+1] at 0x6FAB2DDF");
static_assert(offsetof(D2GSPacketSrvA4, nMonsterId) == 1, "0xA4: mov cx, [ecx+1] at 0x6FAB2EE5");
static_assert(offsetof(D2GSPacketSrvA5, nUnitType) == 1 && offsetof(D2GSPacketSrvA5, dwUnitGUID) == 2
              && offsetof(D2GSPacketSrvA5, nSkillId) == 6,
              "0xA5: [eax+2], [eax+1], [eax+6] at 0x6FAB2E26..0x6FAB2E2C");

//1.10f: D2Client.0x6FAB2930
//  Row 0x11: an overlay on a unit, the id bounded by overlay.txt's count.
void __fastcall SCMD_HandleSrv11_AddOverlay(D2GSPacketSrv11 *pPacket)
{
    const uint16_t nOverlayId = pPacket->nOverlayId;
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(pPacket->dwUnitGUID, (D2C_UnitTypes)pPacket->nUnitType);
    if (pUnit && (int32_t)nOverlayId < sgptDataTables->nOverlayTxtRecordCount)
        CUNIT_AddOverlay(pUnit, nOverlayId, 0);
}

//1.10f: D2Client.0x6FAB2970
//  Row 0x15: a unit warped: CUNIT_WarpUnit, its act and act pointer set again, its collision masks put back (0x1C09 for a player after D2Common's
//  10214, 0x100 for a live monster), and for the local player the catch-up deadline pushed 3000 ms out (a tail jump). Two halts: "Could not get
//  act!" when the unit has no room, and "Illegal packet in sSCmd_UnitWarpXY" when D2COMMON_10083_Return0 answers non-zero for the player's
//  room, which that import's name says it never does.
void __fastcall SCMD_HandleSrv15_ReassignPlayer(D2GSPacketSrv15 *pPacket)
{
    const uint16_t nPosX = pPacket->nPosX;
    const uint16_t nPosY = pPacket->nPosY;
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(pPacket->dwUnitGUID, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pUnit)
        return;

    CUNIT_WarpUnit(pUnit, nPosX, nPosY, pPacket->unk0x00A);

    D2ActiveRoomStrc *pRoom = D2COMMON_UNITS_GetRoom(pUnit);
    if (!pRoom)
    {
        FOG_DisplayHalt(szCouldNotGetActMsg, szFile, 1336);
        exit(-1);
    }
    pUnit->nAct = D2COMMON_DRLG_GetActNoFromLevelId((D2C_Levels)D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom));
    pUnit->pDrlgAct = gpAct;

    if (pUnit->pDynamicPath)
    {
        if (pUnit->dwUnitType == UNIT_PLAYER)
        {
            D2COMMON_10214(pUnit);
            D2COMMON_PATH_SetMoveTestCollisionMask(pUnit->pDynamicPath, 0x1C09);
        }
        else if (pUnit->dwUnitType == UNIT_MONSTER && !D2COMMON_MONSTERS_IsDead(pUnit))
        {
            D2COMMON_PATH_SetFootprintCollisionMask(pUnit->pDynamicPath, 0x100);
        }
    }

    if (pUnit == CUNIT_GetPlayer())
    {
        if (D2COMMON_10083_Return0(D2COMMON_UNITS_GetRoom(pUnit)))
        {
            FOG_DisplayHalt(szIllegalPacketWarpXYMsg, szFile, 2078);
            exit(-1);
        }
        GAME_ExtendCatchUpDeadline(3000);
    }
}

//1.10f: D2Client.0x6FAB2B00
//  Row 0x16: a list of unit positions, nine bytes a record from +4, each through UNITMODE_SyncUnitPosition; asserted non-empty. The count is
//  re-read from the packet on every pass, and the loop tests it once more after the assert (`test al, al / jbe` at 0x6FAB2B31) though nothing
//  between could change it.
void __fastcall SCMD_HandleSrv16_UnitsUpdate(D2GSPacketSrv16 *pPacket)
{
    if (!pPacket->nNumUpdates)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR)szAssertNumUpdates, (uint32_t)(UINT_PTR)szFile, 2133);
        exit(-1);
    }

    //  Walked by pointer from +4, as the bytes do: the count is the packet's own byte and can pass the struct's 55.
    D2ClientUnitUpdateStrc *pUpdate = (D2ClientUnitUpdateStrc *)((uint8_t *)pPacket + 4);
    for (int32_t nUpdateIndex = 0; nUpdateIndex < pPacket->nNumUpdates; ++nUpdateIndex, ++pUpdate)
    {
        const uint16_t nX = pUpdate->nX;
        const uint16_t nY = pUpdate->nY;
        D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType((uint32_t)pUpdate->nUnitGUID, (D2C_UnitTypes)pUpdate->nUnitType);
        if (pUnit)
            UNITMODE_SyncUnitPosition(pUnit, nX, nY, 0, 0, 0);
    }
}

//1.10f: D2Client.0x6FAB2BC0
//  Every item's stat list merged into the unit again, and both active skills put back. Called by the handlers of 0x19..0x1F and 0x20. It walks
//  the whole inventory, every entry INVENTORY_UnitIsItem answers with an item that STATLIST_GetOwner accepts, not only what is worn. A missing
//  hand keeps skill 0 for either side.
//  BUG: pUnit->pSkills is not tested before both hands are read (`mov ecx, [ebx+0A8h] / mov eax, [ecx+8]` at 0x6FAB2BDC); both callers pass the
//  local player, whose skill list exists.
void __fastcall SCMD_RemergeItemStatLists(D2UnitStrc *pUnit)
{
    if (!pUnit || !pUnit->pInventory)
        return;

    D2SkillListStrc *pSkills = pUnit->pSkills;
    const int32_t nLeftSkillId = pSkills->pLeftSkill ? pSkills->pLeftSkill->pSkillsTxt->nSkillId : 0;
    const int32_t nRightSkillId = pSkills->pRightSkill ? pSkills->pRightSkill->pSkillsTxt->nSkillId : 0;

    D2UnitStrc *pEntry = D2COMMON_INVENTORY_GetFirstItem(pUnit->pInventory);
    while (pEntry)
    {
        D2UnitStrc *pNext = D2COMMON_INVENTORY_GetNextItem(pEntry);
        D2UnitStrc *pItem = D2COMMON_INVENTORY_UnitIsItem(pEntry);
        BOOL bStatNotDynamic = FALSE;
        if (pItem && pItem->dwUnitType == UNIT_ITEM && D2COMMON_STATLIST_GetOwner(pItem, &bStatNotDynamic))
        {
            D2COMMON_STATLIST_ExpireUnitStatlist(pUnit, pItem);
            D2COMMON_STATLIST_MergeStatLists(pUnit, pItem, bStatNotDynamic);
        }
        pEntry = pNext;
    }

    D2COMMON_SKILLS_SetLeftActiveSkill(pUnit, (D2C_Skills)nLeftSkillId, (D2UnitGUID)-1);
    D2COMMON_SKILLS_SetRightActiveSkill(pUnit, (D2C_Skills)nRightSkillId, (D2UnitGUID)-1);
}

//1.10f: D2Client.0x6FAB2C90
//  Rows 0x9E..0xA2: a monster's stat, set or added. One function, five rows, a jump table on the id. All five carry the stat at +1 and a GUID
//  at +2 and differ in the value's width at +6 (a byte for 0x9E and 0xA1, a word for 0x9F and 0xA2, a dword for 0xA0) and in what is done with
//  it: 0x9E, 0x9F and 0xA0 set the stat, 0xA1 and 0xA2 add to it. The unit is always looked up as a monster (`mov edx, 1` at 0x6FAB2CEE), and a
//  change of STAT_LEVEL first refreshes the monster through ITEM_RefreshUnitFromMsg with a twelve-byte message made on the stack: 0x47 at +0, the
//  unit type at +1, the GUID at +3, and the other five bytes whatever the stack held, in ours too.
void __fastcall SCMD_HandleSrv9E_A2(uint8_t *pPacket)
{
    int32_t bIsDelta = 0;
    int32_t nValue;

    switch (pPacket[0])
    {
    case 0x9E:
        nValue = ((D2GSPacketSrv9E *)pPacket)->nValue;
        break;
    case 0x9F:
        nValue = ((D2GSPacketSrv9F *)pPacket)->nValue;
        break;
    case 0xA0:
        nValue = (int32_t)((D2GSPacketSrvA0 *)pPacket)->nValue;
        break;
    case 0xA1:
        bIsDelta = 1;
        nValue = ((D2GSPacketSrvA1 *)pPacket)->nValue;
        break;
    case 0xA2:
        bIsDelta = 1;
        nValue = ((D2GSPacketSrvA2 *)pPacket)->nValue;
        break;
    default:
        return;
    }
    const int32_t nStatId = ((D2GSPacketSrv9E *)pPacket)->nStat;

    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(((D2GSPacketSrv9E *)pPacket)->dwGUID, UNIT_MONSTER);
    if (!pUnit)
        return;

    if (nStatId == STAT_LEVEL)
    {
        //  bytes 2 and 7..11 are left as the stack held them, as in the original
        uint8_t pMsg[12];
        pMsg[1] = (uint8_t)pUnit->dwUnitType;
        pMsg[0] = 0x47;
        memcpy(&pMsg[3], &pUnit->dwUnitId, sizeof(pUnit->dwUnitId));
        ITEM_RefreshUnitFromMsg(pMsg);
    }

    if (bIsDelta)
        D2COMMON_STATLIST_AddUnitStat(pUnit, (D2C_ItemStats)nStatId, nValue, 0);
    else
        D2COMMON_STATLIST_SetUnitStat(pUnit, (D2C_ItemStats)nStatId, nValue, 0);
}

//1.10f: D2Client.0x6FAB2D60
//  Row 0xA3: a skill's effect from a unit, with a target unit or a point: skills.txt's PROGRESSIVE flag chooses SKILLS_ApplyClientSkillEffect
//  or its non-progressive twin, the same seven arguments to both. The skill id and level are read as signed words.
//  HARMLESS: the non-progressive arm loads the last argument with `mov al, [esi+1]` alone (0x6FAB2DFD), so the pushed dword carries the skills.txt
//  row pointer's upper 24 bits; the callee masks it (`and ecx, 0FFh`, 0x6FABCC40; `and eax, 0FFh`, 0x6FABCC62), and ours passes the byte.
void __fastcall SCMD_HandleSrvA3_SkillEffect(D2GSPacketSrvA3 *pPacket)
{
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(pPacket->dwUnitGUID, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pUnit)
        return;

    D2UnitStrc *pTarget = CUNIT_FindUnitByGuidAndType(pPacket->dwTargetGUID, (D2C_UnitTypes)pPacket->nTargetType);
    const int16_t nSkillLevel = (int16_t)pPacket->nSkillLevel;
    const int32_t nX = (int32_t)pPacket->nX;
    const int32_t nY = (int32_t)pPacket->nY;
    const int32_t nSkillId = (int16_t)pPacket->nSkillId;
    if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount)
        return;

    D2SkillsTxt *pSkillsTxt = &sgptDataTables->pSkillsTxt[nSkillId];
    if (!pSkillsTxt)
        return;

    if ((uint8_t)pSkillsTxt->dwFlags[0] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_PROGRESSIVE])
        SKILLS_ApplyClientSkillEffect(pUnit, pTarget, (D2C_Skills)nSkillId, nSkillLevel, nX, nY, pPacket->unk0x01);
    else
        SKILLS_ApplyClientSkillEffectNonProgressive(pUnit, pTarget, (D2C_Skills)nSkillId, nSkillLevel, nX, nY, pPacket->unk0x01);
}

//1.10f: D2Client.0x6FAB2E20
//  Row 0xA5: a skill ends, switched on skills.txt's srvdofunc: 67 ends an uninterruptable skill, 76 stops the whirlwind sound, 77 and 78 restore
//  the collision footprint; STATE_SKILL_MOVE is cleared on every path. The word id is masked and still tested negative (`and edx, 0FFFFh / jl`).
void __fastcall SCMD_HandleSrvA5_EndSkillMove(D2GSPacketSrvA5 *pPacket)
{
    const uint16_t nSkillId = pPacket->nSkillId;
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType((uint32_t)pPacket->dwUnitGUID, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pUnit)
        return;

    if ((int32_t)nSkillId < 0 || (int32_t)nSkillId >= sgptDataTables->nSkillsTxtRecordCount)
        return;

    D2SkillsTxt *pSkillsTxt = &sgptDataTables->pSkillsTxt[nSkillId];
    if (!pSkillsTxt)
        return;

    switch ((int16_t)pSkillsTxt->wSrvDoFunc)
    {
    case 67:
        SKILLS_EndUninterruptableSkill(pUnit, (D2C_Skills)nSkillId);
        D2COMMON_STATES_ToggleState(pUnit, STATE_SKILL_MOVE, FALSE);
        return;

    case 77:
    case 78:
        SKILLS_RestoreCollisionFootprint(pUnit);
        D2COMMON_STATES_ToggleState(pUnit, STATE_SKILL_MOVE, FALSE);
        return;

    case 76:
        SKILLS_StopWhirlwindSound(pUnit, (D2C_Skills)nSkillId);
        [[fallthrough]];
    default:
        D2COMMON_STATES_ToggleState(pUnit, STATE_SKILL_MOVE, FALSE);
        return;
    }
}

//1.10f: D2Client.0x6FAB2EE0
//  Row 0xA4: a monster's COF files added, the id bounded by monstats.txt's count. A tail jump.
void __fastcall SCMD_HandleSrvA4_PreloadMonsterCofs(D2GSPacketSrvA4 *pPacket)
{
    const int32_t nMonsterId = pPacket->nMonsterId;
    if (nMonsterId < sgptDataTables->nMonStatsTxtRecordCount)
        GFX_AddMonsterCofFiles(nMonsterId, 0);
}


//  Packets 0x18, 0x19..0x1F, 0x20, 0x95 and 0x96: the player's stats, and where the server says it is.

static_assert(STAT_STRENGTH == 0 && STAT_DEXTERITY == 2 && STAT_HITPOINTS == 6 && STAT_MANA == 8
              && STAT_STAMINA == 10 && STAT_EXPERIENCE == 13 && STAT_GOLD == 14
              && STAT_MANARECOVERY == 0x1A && STAT_HPREGEN == 0x4A && STAT_INVALID == -1,
              "or ebx, -1 at 0x6FAB2F1B; mov ebx, 0Eh / 0Dh; push 6 / 8 / 0Ah / 4Ah / 1Ah in 0x18; the "
              "follow-up's table at 0x6FAB3090 sends 0 and 2 to the refresh, 6 to the revive, 12 to the level");
static_assert(PLRMODE_DEAD == 17 && PLRMODE_TNEUTRAL == 5,
              "cmp dword ptr [ebp+10h], 11h at 0x6FAB2FFB; mov edx, 5 at 0x6FAB3001");
static_assert(offsetof(D2UnitStrc, dwAnimMode) == 0x10, "cmp dword ptr [ebp+10h], 11h at 0x6FAB2FFB");
static_assert(sizeof(D2GSPacketSrv18) == 15 && sizeof(D2GSPacketSrv19) == 2 && sizeof(D2GSPacketSrv1A) == 2
              && sizeof(D2GSPacketSrv1B) == 3 && sizeof(D2GSPacketSrv1C) == 5 && sizeof(D2GSPacketSrv1D) == 3
              && sizeof(D2GSPacketSrv1E) == 4 && sizeof(D2GSPacketSrv1F) == 6 && sizeof(D2GSPacketSrv20) == 10
              && sizeof(D2GSPacketSrv95) == 13 && sizeof(D2GSPacketSrv96) == 9,
              "the eleven rows' nCmdSize in gSCmdTable; push 0Fh / 0Dh / 9 before BITMANIP_Initialize");
static_assert(offsetof(D2GSPacketSrv20, dwUnitGUID) == 1 && offsetof(D2GSPacketSrv20, nStat) == 5
              && offsetof(D2GSPacketSrv20, nValue) == 6,
              "0x20: [esi+1], [esi+6], [esi+5] at 0x6FAB30AA..0x6FAB30C3");

//1.10f: D2Client.0x6FAB2F10
//  Rows 0x19..0x1F: the local player's gold, experience and stats, a jump table on the id; the local player is asserted.
//    0x19  STAT_GOLD        += the byte at +1
//    0x1A  STAT_EXPERIENCE  += the byte at +1
//    0x1B  STAT_EXPERIENCE  += the word at +1
//    0x1C  STAT_EXPERIENCE   = the dword at +1
//    0x1D  the stat at +1    = the byte at +2
//    0x1E  the stat at +1    = the word at +2
//    0x1F  the stat at +1    = the dword at +2
//  All are written with STATLIST_SetUnitStat, the increments by reading the stat first. The default arm cannot be taken (only these seven rows
//  name the function) and would set stat -1 to 0. The follow-up after the stat is the one of 0x20, written out in both as the original has it
//  twice.
void __fastcall SCMD_HandleSrv19_1F_AttributeUpdate(uint8_t *pPacket)
{
    int32_t nValue = 0;
    int32_t nStatId = STAT_INVALID;
    const uint8_t nPacketId = pPacket[0];

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR)szAssertHUnit, (uint32_t)(UINT_PTR)szFile, 2474);
        exit(-1);
    }

    switch (nPacketId)
    {
    case 0x19:
        nStatId = STAT_GOLD;
        nValue = D2COMMON_STATLIST_UnitGetStatValue(pPlayer, STAT_GOLD, 0) + pPacket[1];
        break;
    case 0x1A:
        nStatId = STAT_EXPERIENCE;
        nValue = D2COMMON_STATLIST_UnitGetStatValue(pPlayer, STAT_EXPERIENCE, 0) + pPacket[1];
        break;
    case 0x1B:
        nStatId = STAT_EXPERIENCE;
        nValue = D2COMMON_STATLIST_UnitGetStatValue(pPlayer, STAT_EXPERIENCE, 0) + *(uint16_t *)(pPacket + 1);
        break;
    case 0x1C:
        nValue = *(int32_t *)(pPacket + 1);
        nStatId = STAT_EXPERIENCE;
        break;
    case 0x1D:
        nValue = pPacket[2];
        nStatId = pPacket[1];
        break;
    case 0x1E:
        nValue = *(uint16_t *)(pPacket + 2);
        nStatId = pPacket[1];
        break;
    case 0x1F:
        nValue = *(int32_t *)(pPacket + 2);
        nStatId = pPacket[1];
        break;
    }

    D2COMMON_STATLIST_SetUnitStat(pPlayer, (D2C_ItemStats)nStatId, nValue, 0);

    switch (nStatId)
    {
    case STAT_STRENGTH:
    case STAT_DEXTERITY:
        j_ITEM_RefreshActiveItems(pPlayer);
        return;

    case STAT_HITPOINTS:
        if (nValue && pPlayer->dwAnimMode == PLRMODE_DEAD)
        {
            UNITMODE_SetAnimMode(pPlayer, PLRMODE_TNEUTRAL);
            CUNIT_SetUnitAlive(pPlayer);
        }
        return;

    case STAT_LEVEL:
    {
        D2UnitStrc *pLocalPlayer = CUNIT_GetPlayer();
        if (pLocalPlayer)
        {
            uint8_t pMsg[12];
            pMsg[1] = (uint8_t)pLocalPlayer->dwUnitType;
            pMsg[0] = 0x47;
            memcpy(&pMsg[3], &pLocalPlayer->dwUnitId, sizeof(pLocalPlayer->dwUnitId));
            ITEM_RefreshUnitFromMsg(pMsg);
        }
        if (pPlayer == CUNIT_GetPlayer())
        {
            SCMD_RemergeItemStatLists(pPlayer);
            j_ITEM_RefreshActiveItems(pPlayer);
        }
        return;
    }
    }
}

//1.10f: D2Client.0x6FAB30A0
//  Row 0x20: another player's stat. It finds a PLAYER by the GUID at +1 (`xor edx, edx` at 0x6FAB30A3) and sets the stat at +5 to the dword at
//  +6. Then the follow-up, the same as in 0x19..0x1F instruction for instruction: strength or dexterity refreshes the unit's active items; hit
//  points above zero on a unit in PLRMODE_DEAD put it in PLRMODE_TNEUTRAL and alive; STAT_LEVEL refreshes the local player through
//  ITEM_RefreshUnitFromMsg (the twelve-byte 0x47 message) and, when the unit is the local player, re-merges its item stat lists and refreshes
//  its items. In 0x19..0x1F the unit is the local player; in 0x20 it need not be.
void __fastcall SCMD_HandleSrv20_AttributeUpdate(D2GSPacketSrv20 *pPacket)
{
    const int32_t nValue = pPacket->nValue;
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(pPacket->dwUnitGUID, UNIT_PLAYER);
    if (!pUnit)
        return;

    const int32_t nStatId = pPacket->nStat;
    D2COMMON_STATLIST_SetUnitStat(pUnit, (D2C_ItemStats)nStatId, nValue, 0);

    switch (nStatId)
    {
    case STAT_STRENGTH:
    case STAT_DEXTERITY:
        j_ITEM_RefreshActiveItems(pUnit);
        return;

    case STAT_HITPOINTS:
        if (nValue && pUnit->dwAnimMode == PLRMODE_DEAD)
        {
            UNITMODE_SetAnimMode(pUnit, PLRMODE_TNEUTRAL);
            CUNIT_SetUnitAlive(pUnit);
        }
        return;

    case STAT_LEVEL:
    {
        //  BUG: the 0x47 message names the local player (`call CUNIT_GetPlayer` at 0x6FAB3108), not the unit: another player's level-up refreshes
        //  ours and not theirs.
        D2UnitStrc *pLocalPlayer = CUNIT_GetPlayer();
        if (pLocalPlayer)
        {
            uint8_t pMsg[12];
            pMsg[1] = (uint8_t)pLocalPlayer->dwUnitType;
            pMsg[0] = 0x47;
            memcpy(&pMsg[3], &pLocalPlayer->dwUnitId, sizeof(pLocalPlayer->dwUnitId));
            ITEM_RefreshUnitFromMsg(pMsg);
        }
        if (pUnit == CUNIT_GetPlayer())
        {
            SCMD_RemergeItemStatLists(pUnit);
            j_ITEM_RefreshActiveItems(pUnit);
        }
        return;
    }
    }
}

//1.10f: D2Client.0x6FAB3170
//  Row 0x18: the local player's life, mana, stamina, regeneration and position, bit-packed through Fog's bit reader over the declared 15 bytes:
//  8, 15 life, 15 mana, 15 stamina, 7 HP regen, 7 mana recovery, 16 X, 16 Y, 8, 8. Life, mana and stamina go into the stat list shifted left
//  by eight. The two trailing bytes become signed offsets, and UNITMODE_SyncUnitPosition gets the position and the position plus the offsets: it
//  does not snap the unit when it is already nearer the second point than the first. A dead player is revived on life above zero, as in
//  0x19..0x1F.
void __fastcall SCMD_HandleSrv18_PlayerAttributes(D2GSPacketSrv18 *pPacket)
{
    D2BitBufferStrc tBits;
    FOG_BITMANIP_Initialize(&tBits, &pPacket->nHeader, 15);
    FOG_BITMANIP_Read(&tBits, 8);
    const uint16_t nHitPoints = (uint16_t)FOG_BITMANIP_Read(&tBits, 15);
    const uint16_t nMana = (uint16_t)FOG_BITMANIP_Read(&tBits, 15);
    const uint16_t nStamina = (uint16_t)FOG_BITMANIP_Read(&tBits, 15);
    const uint8_t nHpRegen = (uint8_t)FOG_BITMANIP_Read(&tBits, 7);
    const uint8_t nManaRecovery = (uint8_t)FOG_BITMANIP_Read(&tBits, 7);
    const uint16_t nX = (uint16_t)FOG_BITMANIP_Read(&tBits, 16);
    const uint16_t nY = (uint16_t)FOG_BITMANIP_Read(&tBits, 16);
    const uint8_t nDeltaXRaw = (uint8_t)FOG_BITMANIP_Read(&tBits, 8);
    const uint8_t nDeltaYRaw = (uint8_t)FOG_BITMANIP_Read(&tBits, 8);

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;

    const int32_t nLife = nHitPoints << 8;
    D2COMMON_STATLIST_SetUnitStat(pPlayer, STAT_HITPOINTS, nLife, 0);
    D2COMMON_STATLIST_SetUnitStat(pPlayer, STAT_MANA, nMana << 8, 0);
    D2COMMON_STATLIST_SetUnitStat(pPlayer, STAT_STAMINA, nStamina << 8, 0);
    D2COMMON_STATLIST_SetUnitStat(pPlayer, STAT_HPREGEN, nHpRegen, 0);
    D2COMMON_STATLIST_SetUnitStat(pPlayer, STAT_MANARECOVERY, nManaRecovery, 0);

    //  0x80 stays +128: `cmp eax, 80h / jle`, then `sub eax, 100h` wraps only 0x81..0xFF, so the range is -127..+128. Whether the server ever sends
    //  0x80 is not established.
    int32_t nDeltaX = nDeltaXRaw;
    int32_t nDeltaY = nDeltaYRaw;
    if (nDeltaX > 0x80)
        nDeltaX -= 0x100;
    if (nDeltaY > 0x80)
        nDeltaY -= 0x100;
    UNITMODE_SyncUnitPosition(pPlayer, nX, nY, 0, (uint16_t)(nX + nDeltaX), (uint16_t)(nY + nDeltaY));

    if (nLife && pPlayer->dwAnimMode == PLRMODE_DEAD)
    {
        UNITMODE_SetAnimMode(pPlayer, PLRMODE_TNEUTRAL);
        CUNIT_SetUnitAlive(pPlayer);
    }
}

//1.10f: D2Client.0x6FAB3320
//  Row 0x95: the local player's life, mana, stamina and position, bit-packed over the declared 13 bytes: 8, 15 life, 15 mana, 15 stamina, 16 X,
//  16 Y, 8, 8. The offsets are -127..+128 as in 0x18, and a dead player is revived on life above zero.
void __fastcall SCMD_HandleSrv95_LifeManaUpdate(D2GSPacketSrv95 *pPacket)
{
    D2BitBufferStrc tBits;
    FOG_BITMANIP_Initialize(&tBits, &pPacket->nHeader, 13);
    FOG_BITMANIP_Read(&tBits, 8);
    const uint16_t nLifeRaw = (uint16_t)FOG_BITMANIP_Read(&tBits, 15);
    const uint16_t nMana = (uint16_t)FOG_BITMANIP_Read(&tBits, 15);
    const uint16_t nStamina = (uint16_t)FOG_BITMANIP_Read(&tBits, 15);
    const uint16_t nX = (uint16_t)FOG_BITMANIP_Read(&tBits, 16);
    const uint16_t nY = (uint16_t)FOG_BITMANIP_Read(&tBits, 16);
    const uint8_t nDeltaXRaw = (uint8_t)FOG_BITMANIP_Read(&tBits, 8);
    const uint8_t nDeltaYRaw = (uint8_t)FOG_BITMANIP_Read(&tBits, 8);

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;

    const int32_t nLife = nLifeRaw << 8;
    D2COMMON_STATLIST_SetUnitStat(pPlayer, STAT_HITPOINTS, nLife, 0);
    D2COMMON_STATLIST_SetUnitStat(pPlayer, STAT_MANA, nMana << 8, 0);
    D2COMMON_STATLIST_SetUnitStat(pPlayer, STAT_STAMINA, nStamina << 8, 0);

    int32_t nDeltaX = nDeltaXRaw;
    int32_t nDeltaY = nDeltaYRaw;
    if (nDeltaX > 0x80)
        nDeltaX -= 0x100;
    if (nDeltaY > 0x80)
        nDeltaY -= 0x100;
    UNITMODE_SyncUnitPosition(pPlayer, nX, nY, 0, (uint16_t)(nX + nDeltaX), (uint16_t)(nY + nDeltaY));

    if (nLife && pPlayer->dwAnimMode == PLRMODE_DEAD)
    {
        UNITMODE_SetAnimMode(pPlayer, PLRMODE_TNEUTRAL);
        CUNIT_SetUnitAlive(pPlayer);
    }
}

//1.10f: D2Client.0x6FAB3480
//  Row 0x96: the local player's stamina and position, bit-packed over the declared 9 bytes: 8, 15 stamina, 16 X, 16 Y, 8, 8. The offsets are
//  -127..+128 as in 0x18; it carries no life and does not revive.
void __fastcall SCMD_HandleSrv96_WalkVerify(D2GSPacketSrv96 *pPacket)
{
    D2BitBufferStrc tBits;
    FOG_BITMANIP_Initialize(&tBits, &pPacket->nHeader, 9);
    FOG_BITMANIP_Read(&tBits, 8);
    const uint16_t nStamina = (uint16_t)FOG_BITMANIP_Read(&tBits, 15);
    const uint16_t nX = (uint16_t)FOG_BITMANIP_Read(&tBits, 16);
    const uint16_t nY = (uint16_t)FOG_BITMANIP_Read(&tBits, 16);
    const uint8_t nDeltaXRaw = (uint8_t)FOG_BITMANIP_Read(&tBits, 8);
    const uint8_t nDeltaYRaw = (uint8_t)FOG_BITMANIP_Read(&tBits, 8);

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;

    D2COMMON_STATLIST_SetUnitStat(pPlayer, STAT_STAMINA, nStamina << 8, 0);

    int32_t nDeltaX = nDeltaXRaw;
    int32_t nDeltaY = nDeltaYRaw;
    if (nDeltaX > 0x80)
        nDeltaX -= 0x100;
    if (nDeltaY > 0x80)
        nDeltaY -= 0x100;
    UNITMODE_SyncUnitPosition(pPlayer, nX, nY, 0, (uint16_t)(nX + nDeltaX), (uint16_t)(nY + nDeltaY));
}


//  Packets 0x21, 0x22, 0x23, 0x26, 0x27, 0x2C, 0x5A, 0x76, 0x93, 0x94, 0x98, 0x99 and 0x9A: skills, casts, chat and sounds. szAssertHSkill is
//  SCmd.h's.

static_assert(sizeof(D2GSPacketSrv21) == 12 && sizeof(D2GSPacketSrv22) == 12 && sizeof(D2GSPacketSrv23) == 13
              && sizeof(D2GSPacketSrv27) == 40 && sizeof(D2GSPacketSrv2C) == 8 && sizeof(D2GSPacketSrv5A) == 40
              && sizeof(D2GSPacketSrv76) == 6 && sizeof(D2GSPacketSrv93) == 8 && sizeof(D2GSPacketSrv98) == 7
              && sizeof(D2GSPacketSrv99) == 16 && sizeof(D2GSPacketSrv9A) == 17,
              "the rows' nCmdSize in gSCmdTable; 0x26 and 0x94 are -1");
static_assert(sizeof(D2GSPacketSrv26) == 282 && sizeof(D2GSPacketSrv94__SkillInfo) == 3,
              "0x26: SStrCopy at +0x1A for 0x100 ends at 282; 0x94: add esi, 3 at 0x6FAB365A");
static_assert(offsetof(D2GSPacketSrv21, nUnitType) == 1 && offsetof(D2GSPacketSrv21, bRemove) == 2
              && offsetof(D2GSPacketSrv21, nUnitGUID) == 3 && offsetof(D2GSPacketSrv21, nSkillId) == 7
              && offsetof(D2GSPacketSrv21, nSkillLevel) == 9,
              "0x21: [esi+3], [esi+1], [esi+7], [esi+2], [esi+9]");
static_assert(offsetof(D2GSPacketSrv93, dwUnitGUID) == 1 && offsetof(D2GSPacketSrv93, nLevelDelta) == 5
              && offsetof(D2GSPacketSrv93, nElement) == 6 && offsetof(D2GSPacketSrv93, nSkillPage) == 7,
              "0x93: [eax+5], [eax+1], [eax+6], [eax+7]");
static_assert(offsetof(D2GSPacketSrv94, nSkills) == 1 && offsetof(D2GSPacketSrv94, dwUnitGUID) == 2
              && offsetof(D2GSPacketSrv94, Skills) == 6 && offsetof(D2GSPacketSrv94__SkillInfo, nSkill) == 0
              && offsetof(D2GSPacketSrv94__SkillInfo, nSkillLevel) == 2,
              "0x94: [eax+1], [eax+2], lea esi, [eax+6]; word [esi], byte [esi+2]");
static_assert(offsetof(D2GSPacketSrv22, nUnitGUID) == 3 && offsetof(D2GSPacketSrv22, nSkillId) == 7
              && offsetof(D2GSPacketSrv22, nQuantity) == 9 && offsetof(D2GSPacketSrv22, bIsCorpse) == 0x0B,
              "0x22: [esi+0Bh], [esi+3], [esi+7], [esi+9]");
static_assert(offsetof(D2GSPacketSrv23, dwUnitGUID) == 2 && offsetof(D2GSPacketSrv23, bLeftSkill) == 6
              && offsetof(D2GSPacketSrv23, nSkill) == 7 && offsetof(D2GSPacketSrv23, dwOwnerGUID) == 9,
              "0x23: [esi+2], [esi+7], [esi+9], [esi+6]");
static_assert(offsetof(D2GSPacketSrv98, dwUnitGUID) == 1 && offsetof(D2GSPacketSrv98, nMinionClassId) == 5,
              "MONSTER_SetMinionSpawnClassId: [esi+1] at 0x6FB44888, word [esi+5] at 0x6FB44894");
static_assert(offsetof(D2GSPacketSrv99, nUnitType) == 1 && offsetof(D2GSPacketSrv99, dwUnitGUID) == 2
              && offsetof(D2GSPacketSrv99, nSkillId) == 6 && offsetof(D2GSPacketSrv99, nSkillLevel) == 8
              && offsetof(D2GSPacketSrv99, nTargetType) == 9 && offsetof(D2GSPacketSrv99, dwTargetGUID) == 0x0A
              && offsetof(D2GSPacketSrv99, unk0x0E) == 0x0E,
              "0x99: [esi+2], [esi+1], [esi+0Ah], [esi+9], [esi+0Eh], [esi+8], [esi+6]");
static_assert(offsetof(D2GSPacketSrv9A, nUnitType) == 1 && offsetof(D2GSPacketSrv9A, dwUnitGUID) == 2
              && offsetof(D2GSPacketSrv9A, nSkillId) == 6 && offsetof(D2GSPacketSrv9A, nSkillLevel) == 0x0A
              && offsetof(D2GSPacketSrv9A, nX) == 0x0B && offsetof(D2GSPacketSrv9A, nY) == 0x0D
              && offsetof(D2GSPacketSrv9A, unk0x0F) == 0x0F,
              "0x9A: [esi+2], [esi+1], [esi+0Fh], [esi+0Dh], [esi+0Bh], [esi+0Ah], [esi+6]");
static_assert(offsetof(D2GSPacketSrv26, nMessageType) == 1 && offsetof(D2GSPacketSrv26, nLang) == 2
              && offsetof(D2GSPacketSrv26, nUnitType) == 3 && offsetof(D2GSPacketSrv26, dwUnitGUID) == 4
              && offsetof(D2GSPacketSrv26, nMessageColor) == 8 && offsetof(D2GSPacketSrv26, nNameColor) == 9
              && offsetof(D2GSPacketSrv26, szName) == 0x0A && offsetof(D2GSPacketSrv26, szMessage) == 0x1A,
              "0x26: the ten loads at 0x6FAB3896..0x6FAB38BC, lea esi, [ecx+0Ah], and +1Ah at 0x6FAB38E1");
static_assert(offsetof(D2GSPacketSrv2C, nUnitType) == 1 && offsetof(D2GSPacketSrv2C, dwUnitGUID) == 2
              && offsetof(D2GSPacketSrv2C, nSoundEvent) == 6,
              "0x2C: [ecx+6], [ecx+1], [ecx+2] at 0x6FAB39D0..0x6FAB39D7");

//1.10f: D2Client.0x6FAB3570
//  Row 0x21: a skill assigned to any unit (SKILLS_AssignSkill, the byte at +2 as bRemove). The tree's pending point is cleared by tail jump.
void __fastcall SCMD_HandleSrv21_UpdateSkills(D2GSPacketSrv21 *pPacket)
{
    const uint16_t nSkillId = pPacket->nSkillId;
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(pPacket->nUnitGUID, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pUnit)
        return;

    D2COMMON_SKILLS_AssignSkill(pUnit, (D2C_Skills)nSkillId, pPacket->nSkillLevel, pPacket->bRemove, szFile, 2700);
    SKILLSTREE_ClearSkillPointPending();
}

//1.10f: D2Client.0x6FAB35C0
//  Row 0x93: levels added to one page of a player's skills, filtered by element. The unit is looked up as a player whatever the packet says. The
//  page byte is stored over the frame slot `push ecx` made, so its dword is the packet pointer's upper bytes; SKILLS_AddLevelsToSkillPage tests
//  `cmp al, 4` and masks it (`and eax, 0FFh`, 0x6FABDF71). The delta is 0x80-positive as 0x18's offsets are (`cmp esi, 80h / jle` at
//  0x6FAB35CF): -127..+128.
void __fastcall SCMD_HandleSrv93_SkillPageLevels(D2GSPacketSrv93 *pPacket)
{
    int32_t nDelta = pPacket->nLevelDelta;
    if (nDelta > 0x80)
        nDelta -= 0x100;
    const int32_t nElement = pPacket->nElement;
    const uint8_t nSkillPage = pPacket->nSkillPage;

    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(pPacket->dwUnitGUID, UNIT_PLAYER);
    if (pUnit)
        SKILLS_AddLevelsToSkillPage(pUnit, nDelta, nElement, nSkillPage);
}

//1.10f: D2Client.0x6FAB3610
//  Row 0x94: a player's base skill levels, three bytes a record from +6. The unit is looked up as a player whatever the packet says.
void __fastcall SCMD_HandleSrv94_BaseSkillLevels(D2GSPacketSrv94 *pPacket)
{
    const uint8_t nSkills = pPacket->nSkills;
    D2GSPacketSrv94__SkillInfo *pSkillInfo = (D2GSPacketSrv94__SkillInfo *)((uint8_t *)pPacket + 6);

    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(pPacket->dwUnitGUID, UNIT_PLAYER);
    if (!pUnit)
        return;

    for (int32_t nLeft = nSkills; nLeft > 0; --nLeft, ++pSkillInfo)
    {
        D2COMMON_SKILLS_AssignSkill(pUnit, (D2C_Skills)pSkillInfo->nSkill, pSkillInfo->nSkillLevel, FALSE, szFile, 2743);
    }
}

//1.10f: D2Client.0x6FAB3670
//  Row 0x22: a player's skill quantity, unless the byte at +0x0B is set; the skill is asserted. The unit is looked up as a player whatever the
//  packet says; the unit type at +1 is carried and never read.
void __fastcall SCMD_HandleSrv22_SetSkill(D2GSPacketSrv22 *pPacket)
{
    if (pPacket->bIsCorpse)
        return;

    const uint16_t nSkillId = pPacket->nSkillId;
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType((uint32_t)pPacket->nUnitGUID, UNIT_PLAYER);
    if (!pUnit)
        return;

    D2SkillStrc *pSkill = D2COMMON_SKILLS_GetSkillById(pUnit, (D2C_Skills)nSkillId, (D2UnitGUID)-1);
    if (!pSkill)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR)szAssertHSkill, (uint32_t)(UINT_PTR)szFile, 2774);
        exit(-1);
    }
    D2COMMON_SKILLS_SetQuantity(pSkill, pPacket->nQuantity);
}

//1.10f: D2Client.0x6FAB36D0
//  Row 0x23: a player's left or right active skill and its owner GUID. The unit is looked up as a player; the unit type at +1 is never read.
void __fastcall SCMD_HandleSrv23_SetActiveSkill(D2GSPacketSrv23 *pPacket)
{
    const uint16_t nSkill = pPacket->nSkill;
    const D2UnitGUID dwOwnerGUID = pPacket->dwOwnerGUID;
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(pPacket->dwUnitGUID, UNIT_PLAYER);
    if (!pUnit)
        return;

    if (pPacket->bLeftSkill)
        D2COMMON_SKILLS_SetLeftActiveSkill(pUnit, (D2C_Skills)nSkill, dwOwnerGUID);
    else
        D2COMMON_SKILLS_SetRightActiveSkill(pUnit, (D2C_Skills)nSkill, dwOwnerGUID);
}

//1.10f: D2Client.0x6FAB3710
//  Row 0x98: the class an evil hut spawns, from a stack copy whose eighth byte is never written or read (MONSTER_SetMinionSpawnClassId reads
//  bytes 1..6).
void __fastcall SCMD_HandleSrv98_MinionSpawnClass(D2GSPacketSrv98 *pPacket)
{
    uint8_t tPacket[8];
    memcpy(tPacket, pPacket, sizeof(D2GSPacketSrv98));
    MONSTER_SetMinionSpawnClassId((const D2GSPacketSrv98 *)tPacket);
}

//1.10f: D2Client.0x6FAB3740
//  Row 0x99: a unit's skill cast at a unit. The word at +0x0E (the skill level) is loaded with `mov cx, [..]` after a call; the cast functions
//  forward the dword and SKILLS_ClientCastAtTarget masks it (`and edx, 0FFFFh`, 0x6FABA77B) before SKILLS_RunClientDoFunc tests it.
void __fastcall SCMD_HandleSrv99_CastAtUnit(D2GSPacketSrv99 *pPacket)
{
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(pPacket->dwUnitGUID, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pUnit)
        return;

    D2UnitStrc *pTarget = CUNIT_FindUnitByGuidAndType((uint32_t)pPacket->dwTargetGUID, (D2C_UnitTypes)pPacket->nTargetType);
    if (!pTarget)
        return;

    SKILLS_ClientCastAtUnit(pUnit, (D2C_Skills)pPacket->nSkillId, pPacket->nSkillLevel, pTarget, pPacket->unk0x0E);
}

//1.10f: D2Client.0x6FAB3790
//  Row 0x9A: a unit's skill cast at a point. The skill id is the low word of the dword at +6; the word at +0x0F is the skill level, handled as in
//  0x99.
void __fastcall SCMD_HandleSrv9A_CastAtXY(D2GSPacketSrv9A *pPacket)
{
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(pPacket->dwUnitGUID, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pUnit)
        return;

    SKILLS_ClientCastAtPosition(pUnit, (D2C_Skills)(uint16_t)pPacket->nSkillId, pPacket->nSkillLevel, pPacket->nX, pPacket->nY, pPacket->unk0x0F);
}

//1.10f: D2Client.0x6FAB3890
//  Row 0x26: a chat message, re-laid from the wire into the fixed layout. On the wire the name and the message follow nNameColor back to back,
//  each NUL-terminated. The handler copies the ten fixed bytes one by one into a 284-byte stack buffer, the name with SStrCopy at +0x0A for 16
//  and, past the name's SStrLen and its terminator, the message at +0x1A for 256 (D2GSPacketSrv26's fixed layout, 282 bytes), and hands the
//  buffer to TEXT_HandleChatMessage. Ours builds the same buffer and walks the wire by offset from the packet rather than through the struct's
//  16-character szName.
//  BUG: the message is found by the name's length alone, so a name the wire did not terminate carries the search past it.
void __fastcall SCMD_HandleSrv26_ChatMessage(D2GSPacketSrv26 *pPacket)
{
    uint8_t tChatMessage[284];
    tChatMessage[0] = pPacket->nHeader;
    tChatMessage[1] = pPacket->nMessageType;
    tChatMessage[2] = pPacket->nLang;
    tChatMessage[3] = pPacket->nUnitType;
    memcpy(&tChatMessage[4], &pPacket->dwUnitGUID, sizeof(pPacket->dwUnitGUID));
    tChatMessage[8] = pPacket->nMessageColor;
    tChatMessage[9] = pPacket->nNameColor;

    const char *szWireName = (const char *)pPacket + offsetof(D2GSPacketSrv26, szName);
    STORM_SStrCopy((char *)&tChatMessage[offsetof(D2GSPacketSrv26, szName)], szWireName, 16);
    const size_t nNameLen = STORM_SStrLen(szWireName);
    STORM_SStrCopy((char *)&tChatMessage[offsetof(D2GSPacketSrv26, szMessage)], szWireName + nNameLen + 1, 256);

    TEXT_HandleChatMessage((D2GSPacketSrv26 *)tChatMessage);
}

//1.10f: D2Client.0x6FAB3910
//  Row 0x76: a unit's hover text cleared, from a stack copy (TEXT_ChatClearUnitHoverText reads bytes 1..5).
void __fastcall SCMD_HandleSrv76_ClearUnitHoverText(D2GSPacketSrv76 *pPacket)
{
    D2GSPacketSrv76 tPacket;
    memcpy(&tPacket, pPacket, sizeof(tPacket));
    TEXT_ChatClearUnitHoverText(&tPacket);
}

//1.10f: D2Client.0x6FAB3930
//  Row 0x5A: a system message, from a 40-byte stack copy.
void __fastcall SCMD_HandleSrv5A_SystemMessage(D2GSPacketSrv5A *pPacket)
{
    D2GSPacketSrv5A tPacket;
    memcpy(&tPacket, pPacket, sizeof(tPacket));
    TEXT_HandleSystemMessage(&tPacket);
}

//1.10f: D2Client.0x6FAB3960
//  Row 0x27: a message list over a unit, from a 40-byte stack copy.
void __fastcall SCMD_HandleSrv27_OverheadText(D2GSPacketSrv27 *pPacket)
{
    D2GSPacketSrv27 tPacket;
    memcpy(&tPacket, pPacket, sizeof(tPacket));
    TEXT_OnMessageListPacket(&tPacket);
}

//1.10f: D2Client.0x6FAB39D0
//  Row 0x2C: a unit's sound event. The sound word is loaded with `mov ax, [..]` and UNITSND_PlayUnitSoundEvent keeps it in a 16-bit register
//  and switches on the word. A tail call's shape, a call and a retn.
void __fastcall SCMD_HandleSrv2C_PlayUnitSound(D2GSPacketSrv2C *pPacket)
{
    UNITSND_PlayUnitSoundEvent(pPacket->dwUnitGUID, pPacket->nUnitType, pPacket->nSoundEvent);
}


//  Packets 0x3E, 0x3F, 0x40, 0x47, 0x48, 0x53, 0x54, 0x57, 0x58, 0x59, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x63, 0x65 and 0x92: items,
//  environment, monsters, the roster and portals. szAssertUnitGetControlUnit is SCmd.h's.

static_assert(sizeof(D2GSPacketSrv3F) == 8 && sizeof(D2GSPacketSrv40) == 13 && sizeof(D2GSPacketSrv47) == 11
              && sizeof(D2GSPacketSrv48) == 11 && sizeof(D2GSPacketSrv53) == 10 && sizeof(D2GSPacketSrv54) == 3
              && sizeof(D2GSPacketSrv57) == 14 && sizeof(D2GSPacketSrv58) == 7 && sizeof(D2GSPacketSrv59) == 26
              && sizeof(D2GSPacketSrv5C) == 5 && sizeof(D2GSPacketSrv5D) == 6 && sizeof(D2GSPacketSrv5E) == 38
              && sizeof(D2GSPacketSrv5F) == 5 && sizeof(D2GSPacketSrv60) == 7 && sizeof(D2GSPacketSrv61) == 2
              && sizeof(D2GSPacketSrv63) == 21 && sizeof(D2GSPacketSrv65) == 7 && sizeof(D2GSPacketSrv92) == 6,
              "the rows' nCmdSize in gSCmdTable; 0x3E and 0x5B are -1");
static_assert(offsetof(D2GSPacketSrv3E, nSize) == 1 && offsetof(D2GSPacketSrv3E, pBuffer) == 2,
              "0x3E: mov al, [ecx+1] and add ecx, 2 at 0x6FAB39F5..0x6FAB39F8");
static_assert(offsetof(D2GSPacketSrv53, nIndex) == 1 && offsetof(D2GSPacketSrv53, nTicks) == 5
              && offsetof(D2GSPacketSrv53, bEclipse) == 9,
              "0x53: [esi+1], [esi+5], [esi+9]");
static_assert(offsetof(D2GSPacketSrv57, dwUnitGUID) == 1 && offsetof(D2GSPacketSrv57, nUnitType) == 5
              && offsetof(D2GSPacketSrv57, wNameSeed) == 6 && offsetof(D2GSPacketSrv57, wUmods) == 8
              && offsetof(D2GSPacketSrv57, wUmod2) == 0x0A && offsetof(D2GSPacketSrv57, bChampion) == 0x0C,
              "0x57: [eax+0Ah], [eax+1], [eax+8], [eax+0Ch], [eax+5], [eax+6]");
static_assert(offsetof(D2GSPacketSrv59, dwGUID) == 1 && offsetof(D2GSPacketSrv59, nClass) == 5
              && offsetof(D2GSPacketSrv59, szName) == 6 && offsetof(D2GSPacketSrv59, nPosX) == 0x16
              && offsetof(D2GSPacketSrv59, nPosY) == 0x18,
              "0x59: lea eax, [ecx+6], [ecx+18h], [ecx+16h], [ecx+5], [ecx+1]");
static_assert(offsetof(D2GSPacketSrv5B, nUnitGUID) == 3 && offsetof(D2GSPacketSrv5B, nClass) == 7
              && offsetof(D2GSPacketSrv5B, szName) == 8 && offsetof(D2GSPacketSrv5B, nLevel) == 0x18
              && offsetof(D2GSPacketSrv5B, nPartyId) == 0x1A && offsetof(D2GSPacketSrv5B, unk0x1C) == 0x1C
              && offsetof(D2GSPacketSrv5B, nPartyFlags) == 0x1E && offsetof(D2GSPacketSrv5B, unk0x20) == 0x20
              && offsetof(D2GSPacketSrv5B, unk0x22) == 0x22,
              "0x5B: the loads at 0x6FAB3DA7..0x6FAB3DDD");
static_assert(offsetof(D2GSPacketSrv5C, dwPlayerGUID) == 1 && offsetof(D2GSPacketSrv5F, dwPortalFlags) == 1
              && offsetof(D2GSPacketSrv65, dwUnitId) == 1 && offsetof(D2GSPacketSrv65, unk0x05) == 5
              && offsetof(D2GSPacketSrv92, nUnitType) == 1 && offsetof(D2GSPacketSrv92, dwUnitGUID) == 2,
              "0x5C [ecx+1]; 0x5F [ecx+1]; 0x65 [ecx+5], [ecx+1]; 0x92 [eax+2], [eax+1]");

//1.10f: D2Client.0x6FAB39F0
//  Row 0x3E: an item's stat, bit-packed with self-describing widths: a GUID of 8, 16 or 32 bits (one or two leading bits say which), a set
//  flag, a 9-bit stat, a value of 8, 16 or 32, a parameter of 8 or 16.
void __fastcall SCMD_HandleSrv3E_ItemStatUpdate(D2GSPacketSrv3E *pPacket)
{
    D2BitBufferStrc tBits;
    //  BUG: the bit buffer is sized as the packet's length byte minus 2 (`sub eax, 2` at 0x6FAB39FE) with no test: a length below 2 hands
    //  BITMANIP_Initialize a size near 4 GB.
    FOG_BITMANIP_Initialize(&tBits, pPacket->pBuffer, (size_t)(pPacket->nSize - 2));

    int32_t nWidth = 8;
    if (FOG_BITMANIP_Read(&tBits, 1))
        nWidth = FOG_BITMANIP_Read(&tBits, 1) ? 32 : 16;
    const uint32_t dwItemGUID = FOG_BITMANIP_Read(&tBits, nWidth);
    const uint8_t bSetStat = (uint8_t)FOG_BITMANIP_Read(&tBits, 1);
    const int32_t nStatId = (int32_t)FOG_BITMANIP_Read(&tBits, 9);

    nWidth = 8;
    if (FOG_BITMANIP_Read(&tBits, 1))
        nWidth = FOG_BITMANIP_Read(&tBits, 1) ? 32 : 16;
    const int32_t nValue = (int32_t)FOG_BITMANIP_Read(&tBits, nWidth);

    const int32_t nStatParam = (int32_t)(FOG_BITMANIP_Read(&tBits, 1) ? FOG_BITMANIP_Read(&tBits, 16) : FOG_BITMANIP_Read(&tBits, 8));
    ITEM_OnItemStatUpdate(dwItemGUID, bSetStat, nStatId, nValue, nStatParam);
}

//1.10f: D2Client.0x6FAB3AE0
//  Row 0x3F: a cursor item's skill, from a stack copy.
void __fastcall SCMD_HandleSrv3F_CursorItemSkill(D2GSPacketSrv3F *pPacket)
{
    D2GSPacketSrv3F tPacket;
    memcpy(&tPacket, pPacket, sizeof(tPacket));
    ITEM_OnCursorItemSkill(&tPacket);
}

//1.10f: D2Client.0x6FAB3B00
//  Row 0x40: an item flag, from a sixteen-byte stack copy.
void __fastcall SCMD_HandleSrv40_SetItemFlag(D2GSPacketSrv40 *pPacket)
{
    uint8_t tPacket[16];
    memcpy(tPacket, pPacket, sizeof(D2GSPacketSrv40));
    ITEM_SetItemFlagFromPacket(tPacket);
}

//1.10f: D2Client.0x6FAB3B50
//  Row 0x47: a unit's items refreshed, from a twelve-byte stack copy. The `mov edx, 1` before the call is dead in the callee, which starts with
//  `xor edx, edx` (0x6FB2C2B0) and reads bytes +1 and +3..+6 only.
void __fastcall SCMD_HandleSrv47_RefreshItem(D2GSPacketSrv47 *pPacket)
{
    uint8_t tMsg[12];
    memcpy(tMsg, pPacket, sizeof(D2GSPacketSrv47));
    ITEM_RefreshUnitFromMsg(tMsg);
}

//1.10f: D2Client.0x6FAB3B90
//  Row 0x48: the same through ITEM_RefreshUnitFromPacket.
void __fastcall SCMD_HandleSrv48_RefreshUnit(D2GSPacketSrv48 *pPacket)
{
    uint8_t tPacket[12];
    memcpy(tPacket, pPacket, sizeof(D2GSPacketSrv48));
    ITEM_RefreshUnitFromPacket(tPacket);
}

//1.10f: D2Client.0x6FAB3BC0
//  Row 0x53: the act's environment, only when the player is in the act the client holds: ENVIRONMENT_InitializeEnvironment with the index, the
//  ticks and the eclipse flag, objects refreshed for day and night, and the player's items refreshed.
//  BUG: the local player is dereferenced untested (`mov edx, [eax+1Ch]` at 0x6FAB3BD9, straight after CUNIT_GetPlayer) and tested only on the
//  way out. Reachable only if the server sends 0x53 before the player's unit exists.
void __fastcall SCMD_HandleSrv53_Environment(D2GSPacketSrv53 *pPacket)
{
    const int32_t nIndex = pPacket->nIndex;
    const int32_t nTicks = pPacket->nTicks;
    if (gpAct != CUNIT_GetPlayer()->pDrlgAct)
        return;

    const BOOL bEclipse = pPacket->bEclipse;
    D2ActiveRoomStrc *pRoom = CUNIT_GetPlayerRoom();
    D2COMMON_ENVIRONMENT_InitializeEnvironment(gpAct, pRoom, nIndex, nTicks, bEclipse);
    CUNIT_RefreshObjectsOnDayNightChange();

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    if (pPlayer)
    {
        uint8_t pMsg[12];
        pMsg[1] = (uint8_t)pPlayer->dwUnitType;
        pMsg[0] = 0x47;
        memcpy(&pMsg[3], &pPlayer->dwUnitId, sizeof(pPlayer->dwUnitId));
        ITEM_RefreshUnitFromMsg(pMsg);
    }
}

//1.10f: D2Client.0x6FAB3C70
//  Row 0x54: a jump to MEMPART_Nullsub1. Declared __fastcall here for a __thiscall with one argument in ecx.
void __fastcall j_MEMPART_Nullsub1(void *pPacket)
{
    MEMPART_Nullsub1();
}

//1.10f: D2Client.0x6FAB3CC0
//  Row 0x57: a monster's name seed, its three modifier bytes, champion and unique type flags, and MONUNIQUE_ApplyModifiers by tail jump, only
//  when the type byte at +5 is UNIT_MONSTER.
void __fastcall SCMD_HandleSrv57_MonsterEnchants(D2GSPacketSrv57 *pPacket)
{
    const uint16_t wUmod2 = pPacket->wUmod2;
    const uint32_t dwUnitGUID = pPacket->dwUnitGUID;
    const uint16_t wUmods = pPacket->wUmods;
    const uint16_t bChampion = pPacket->bChampion;
    const uint16_t wNameSeed = pPacket->wNameSeed;
    if (pPacket->nUnitType != UNIT_MONSTER)
        return;

    D2UnitStrc *pMonster = CUNIT_FindUnitByGuidAndType(dwUnitGUID, UNIT_MONSTER);
    if (!pMonster)
        return;

    MONSTER_SetNameSeed(pMonster, wNameSeed);
    uint8_t *pUmods = MONSTER_GetMonUmodArray(pMonster);
    pUmods[0] = (uint8_t)wUmods;
    pUmods[1] = (uint8_t)(wUmods >> 8);
    pUmods[2] = (uint8_t)wUmod2;
    if (bChampion)
        MONSTER_SetTypeFlag(pMonster, MONTYPEFLAG_CHAMPION, 1);
    MONSTER_SetTypeFlag(pMonster, MONTYPEFLAG_UNIQUE, 1);
    MONUNIQUE_ApplyModifiers(pMonster);
}

//1.10f: D2Client.0x6FAB3D50
//  Row 0x58: the anvil panel, from a stack copy.
void __fastcall SCMD_HandleSrv58_AnvilPanel(D2GSPacketSrv58 *pPacket)
{
    D2GSPacketSrv58 tPacket;
    memcpy(&tPacket, pPacket, sizeof(tPacket));
    USEITEM_AnvilOnPanelPacket(&tPacket);
}

//1.10f: D2Client.0x6FAB3D80
//  Row 0x59: another player created.
void __fastcall SCMD_HandleSrv59_AssignPlayer(D2GSPacketSrv59 *pPacket)
{
    CUNIT_CreatePlayerUnit(pPacket->dwGUID, (D2C_PlayerClasses)pPacket->nClass, pPacket->nPosX, pPacket->nPosY, pPacket->szName);
}

//1.10f: D2Client.0x6FAB3DA0
//  Row 0x5B: a player added to the roster; the guild tag at +0x22 and the guild name after its terminator. Three of ROSTER_AddPlayer's words are
//  pushed from registers holding other upper bits; the callee reads nLevel, nPartyId and the word at +0x20 as words and never reads the one at
//  +0x1C.
void __fastcall SCMD_HandleSrv5B_RosterAddPlayer(D2GSPacketSrv5B *pPacket)
{
    char *szGuildTag = (char *)pPacket + offsetof(D2GSPacketSrv5B, unk0x22);
    ROSTER_AddPlayer(pPacket->nUnitGUID, pPacket->nClass, pPacket->szName, (int16_t)pPacket->nLevel, (int16_t)pPacket->nPartyId, (int16_t)pPacket->unk0x1C, (D2C_PartyButtons)pPacket->nPartyFlags,
                     (int16_t)pPacket->unk0x20, szGuildTag, szGuildTag + strlen(szGuildTag) + 1);
}

//1.10f: D2Client.0x6FAB3DF0
//  Row 0x5C: a player removed from the roster. A tail jump.
void __fastcall SCMD_HandleSrv5C_RosterRemovePlayer(D2GSPacketSrv5C *pPacket)
{
    ROSTER_RemovePlayer(pPacket->dwPlayerGUID);
}

//1.10f: D2Client.0x6FAB3E00
//  Row 0x5D: a quest state, from a stack copy.
void __fastcall SCMD_HandleSrv5D_QuestState(D2GSPacketSrv5D *pPacket)
{
    D2GSPacketSrv5D tPacket;
    memcpy(&tPacket, pPacket, sizeof(tPacket));
    QUESTLOG_QuestOnQuestStatePacket(1, &tPacket);
}

//1.10f: D2Client.0x6FAB3E30
//  Row 0x5E: a jump to QUESTS_StoreSrv5EPayload.
void __fastcall SCMD_HandleSrv5E_QuestAvailability(D2GSPacketSrv5E *pPacket)
{
    QUESTS_StoreSrv5EPayload(pPacket);
}

//1.10f: D2Client.0x6FAB3E70
//  Row 0x92: a unit's items. A tail jump.
void __fastcall SCMD_HandleSrv92_RefreshInventory(D2GSPacketSrv92 *pPacket)
{
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(pPacket->dwUnitGUID, (D2C_UnitTypes)pPacket->nUnitType);
    if (pUnit)
        ITEM_OnPacket92(pUnit);
}

//1.10f: D2Client.0x6FAB3E90
//  Row 0x5F: the local player's portal flags.
void __fastcall SCMD_HandleSrv5F_PortalFlags(D2GSPacketSrv5F *pPacket)
{
    const int32_t nPortalFlags = (int32_t)pPacket->dwPortalFlags;
    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR)szAssertHUnit, (uint32_t)(UINT_PTR)szFile, 3349);
        exit(-1);
    }
    D2COMMON_UNITS_SetPlayerPortalFlags(pPlayer, nPortalFlags);
}

//1.10f: D2Client.0x6FAB3ED0
//  Row 0x60: a town portal's state, from a stack copy; the local player is asserted.
void __fastcall SCMD_HandleSrv60_TownPortalState(D2GSPacketSrv60 *pPacket)
{
    D2GSPacketSrv60 tPacket;
    memcpy(&tPacket, pPacket, sizeof(tPacket));
    if (!CUNIT_GetPlayer())
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR)szAssertUnitGetControlUnit, (uint32_t)(UINT_PTR)szFile, 3361);
        exit(-1);
    }
    OBJECT_ApplyPortalState(&tPacket);
}

//1.10f: D2Client.0x6FAB3F20
//  Row 0x61: the cinematic, then the resolution reloaded by tail jump.
void __fastcall SCMD_HandleSrv61_PlayCinematic(D2GSPacketSrv61 *pPacket)
{
    QUESTS_PlayCinematicFromPacket(pPacket);
    MINIPANEL_LoadResolutionFromRegistry();
}

//1.10f: D2Client.0x6FAB3F30
//  Row 0x63: the waypoint panel, from a 24-byte stack copy.
void __fastcall SCMD_HandleSrv63_WaypointMenu(D2GSPacketSrv63 *pPacket)
{
    uint8_t tPacket[24];
    memcpy(tPacket, pPacket, sizeof(D2GSPacketSrv63));
    WAYPOINT_OnOpenPacket((D2GSPacketSrv63 *)tPacket);
}

//1.10f: D2Client.0x6FAB3F60
//  Row 0x65: a party update. A tail jump. The word is passed in dx over the router's edx; ROSTER_OnPartyUpdate masks it (`and edx, 0FFFFh`) and
//  sign-extends by hand (0x6FAAFB55).
void __fastcall SCMD_HandleSrv65_RosterUpdate(D2GSPacketSrv65 *pPacket)
{
    ROSTER_OnPartyUpdate(pPacket->dwUnitId, pPacket->unk0x05);
}


//  Packets 0x73, 0x74, 0x75, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x81, 0x82, 0x89, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90 and
//  0x97: missiles, trade, pets, hotkeys and the roster.

static const char szAssertCmdWeaponSwap[] = "ptCmd->bCmd == SCMD_WEAPONSWAP";   // 0x6FB76B50
static_assert(sizeof(szAssertCmdWeaponSwap) == 30 + 1, "the assert text at 0x6FB76B50, measured in the DLL");

static_assert(sizeof(D2GSPacketSrv73) == 32 && sizeof(D2GSPacketSrv74) == 10 && sizeof(D2GSPacketSrv75) == 13
              && sizeof(D2GSPacketSrv77) == 2 && sizeof(D2GSPacketSrv78) == 21 && sizeof(D2GSPacketSrv79) == 6
              && sizeof(D2GSPacketSrv7A) == 13 && sizeof(D2GSPacketSrv7B) == 8 && sizeof(D2GSPacketSrv7C) == 6
              && sizeof(D2GSPacketSrv7D) == 18 && sizeof(D2GSPacketSrv7E) == 5 && sizeof(D2GSPacketSrv7F) == 10
              && sizeof(D2GSPacketSrv81) == 20 && sizeof(D2GSPacketSrv82) == 29 && sizeof(D2GSPacketSrv89) == 2
              && sizeof(D2GSPacketSrv8B) == 6 && sizeof(D2GSPacketSrv8C) == 11 && sizeof(D2GSPacketSrv8D) == 7
              && sizeof(D2GSPacketSrv8E) == 10 && sizeof(D2GSPacketSrv8F) == 33 && sizeof(D2GSPacketSrv90) == 13
              && sizeof(D2GSPacketSrv97) == 1,
              "the rows' nCmdSize in gSCmdTable; for 0x74, 0x78, 0x7C and 0x7D also the copies' lengths");
static_assert(offsetof(D2GSPacketSrv73, nMissileId) == 5 && offsetof(D2GSPacketSrv73, nX) == 7
              && offsetof(D2GSPacketSrv73, nY) == 0x0B && offsetof(D2GSPacketSrv73, nFirstX) == 0x0F
              && offsetof(D2GSPacketSrv73, nFirstY) == 0x13 && offsetof(D2GSPacketSrv73, nCurrentFrame) == 0x17
              && offsetof(D2GSPacketSrv73, nUnitType) == 0x19 && offsetof(D2GSPacketSrv73, nUnitGUID) == 0x1A
              && offsetof(D2GSPacketSrv73, nLevel) == 0x1E && offsetof(D2GSPacketSrv73, nPierce) == 0x1F,
              "0x73: the loads at 0x6FAB3F79..0x6FAB3F95, 0x6FAB3FBF, 0x6FAB401D and 0x6FAB4028");
static_assert(offsetof(D2MissileStrc, pOwner) == 4 && offsetof(D2MissileStrc, nMissile) == 0x10
              && offsetof(D2MissileStrc, nX) == 0x14 && offsetof(D2MissileStrc, nY) == 0x18
              && offsetof(D2MissileStrc, nTargetX) == 0x1C && offsetof(D2MissileStrc, nTargetY) == 0x20
              && offsetof(D2MissileStrc, nSkillLevel) == 0x30,
              "0x73: the stores into the block, 0x6FAB3FC2..0x6FAB4000");
static_assert(offsetof(D2GSPacketSrv75, nUnitGUID) == 1 && offsetof(D2GSPacketSrv75, nPartyId) == 5
              && offsetof(D2GSPacketSrv75, nLevel) == 7 && offsetof(D2GSPacketSrv75, nFlags) == 9
              && offsetof(D2GSPacketSrv75, unk0x0B) == 0x0B,
              "0x75: [ecx+9], [ecx+0Bh], [ecx+7], [ecx+5], [ecx+1]");
static_assert(offsetof(D2GSPacketSrv79, bSelf) == 1 && offsetof(D2GSPacketSrv79, dwGold) == 2,
              "0x79: mov dl, [ecx+1] and mov ecx, [ecx+2]");
static_assert(offsetof(D2GSPacketSrv7A, unk0x01) == 1 && offsetof(D2GSPacketSrv7A, unk0x02) == 2
              && offsetof(D2GSPacketSrv7A, unk0x03) == 3 && offsetof(D2GSPacketSrv7A, unk0x05) == 5
              && offsetof(D2GSPacketSrv7A, unk0x09) == 9,
              "0x7A: [eax+1], [eax+9], [eax+3], [eax+2], [eax+5]");
static_assert(offsetof(D2GSPacketSrv81, unk0x01) == 1 && offsetof(D2GSPacketSrv81, unk0x02) == 2
              && offsetof(D2GSPacketSrv81, unk0x04) == 4 && offsetof(D2GSPacketSrv81, unk0x08) == 8
              && offsetof(D2GSPacketSrv81, unk0x0C) == 0x0C && offsetof(D2GSPacketSrv81, unk0x10) == 0x10,
              "0x81: [ecx+0Ch], [ecx+10h], [ecx+2], [ecx+1], [ecx+4], [ecx+8]");
static_assert(offsetof(D2GSPacketSrv7B, nSlot) == 1 && offsetof(D2GSPacketSrv7B, nItemGUID) == 4,
              "0x7B: mov bl, [ecx+1] and mov edx, [ecx+4], the bit-field word at [ecx+2] between them");
static_assert(offsetof(D2GSPacketSrv7F, bIsPlayer) == 1 && offsetof(D2GSPacketSrv7F, nLifePercentage) == 2
              && offsetof(D2GSPacketSrv7F, nUnitGUID) == 4 && offsetof(D2GSPacketSrv7F, nLevelId) == 8,
              "0x7F: [eax+2], [eax+1], [eax+8], [eax+4]");
static_assert(offsetof(D2GSPacketSrv90, dwGUID) == 1 && offsetof(D2GSPacketSrv90, dwPosX) == 5
              && offsetof(D2GSPacketSrv90, dwPosY) == 9,
              "0x90: [ecx+9], [ecx+5], [ecx+1]");
static_assert(offsetof(D2GSPacketSrv82, dwOwnerGUID) == 1 && offsetof(D2GSPacketSrv82, szOwnerName) == 5
              && offsetof(D2GSPacketSrv82, dwPortalGUID) == 0x15,
              "0x82: [esi+15h], [esi+1], [esi+19h], lea edx, [esi+5]");
static_assert(offsetof(D2GSPacketSrv89, nEvent) == 1,
              "QUESTSFX_FireTrigger: mov al, [ecx+1] at 0x6FAB8F62 -- the prototype this batch corrected");
static_assert(offsetof(D2GSPacketSrv8B, nUnitGUID) == 1 && offsetof(D2GSPacketSrv8B, bParty) == 5
              && offsetof(D2GSPacketSrv8C, dwPlayerGUID) == 1 && offsetof(D2GSPacketSrv8C, nFlags) == 9
              && offsetof(D2GSPacketSrv8D, dwPlayerGUID) == 1 && offsetof(D2GSPacketSrv8D, nPartyGUID) == 5
              && offsetof(D2GSPacketSrv8E, nType) == 1 && offsetof(D2GSPacketSrv8E, dwPlayerGUID) == 2
              && offsetof(D2GSPacketSrv8E, dwCorpseGUID) == 6,
              "0x8B [ecx+5], [ecx+1]; 0x8C [ecx+9], [ecx+5], [ecx+1]; 0x8D [ecx+5], [ecx+1]; "
              "0x8E [eax+1], [eax+2], [eax+6]");

//1.10f: D2Client.0x6FAB3F70
//  Row 0x73: a missile: MISSILE_ClientSpawn over a zeroed 0x5C-byte block with the local player as owner and flags 0x40000001 (0x40000021, and
//  the first point as the target, when both its coordinates are non-zero), then the owner's type and GUID, the pierce count when the byte is
//  non-zero, the current frame, and a random animation frame rolled off the missile's own seed. The original inlines D2Common's
//  SEED_RollLimitedRandomNumber at the end, the LCG step included; ours calls SEED_RollLimitedRandomNumberInline (Shared.cpp).
//  DIFFERENCE: ours calls SEED_RollRandomNumber where the bytes inline the LCG step; it computes the same `lo * 0x6AC690C5 + hi` over the same
//  two dwords and stores both halves, the high one first where 0x73 stores the low one first.
void __fastcall SCMD_HandleSrv73_SpawnMissile(D2GSPacketSrv73 *pPacket)
{
    const int32_t nX = pPacket->nX;
    const int32_t nY = pPacket->nY;
    const int32_t nFirstY = pPacket->nFirstY;
    const int32_t nFirstX = pPacket->nFirstX;
    const uint16_t nMissileId = (uint16_t)pPacket->nMissileId;
    const uint16_t nCurrentFrame = (uint16_t)pPacket->nCurrentFrame;
    const int32_t nOwnerGUID = pPacket->nUnitGUID;

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    D2MissileStrc tParams;
    memset(&tParams, 0, sizeof(tParams));
    tParams.pOwner = pPlayer;
    tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_FROMSERVER;
    tParams.nX = nX;
    tParams.nY = nY;
    tParams.nSkillLevel = pPacket->nLevel;
    tParams.nMissile = nMissileId;
    if (nFirstX && nFirstY)
    {
        tParams.dwFlags = MISSILESPAWN_STARTPOS | MISSILESPAWN_TARGETPOS | MISSILESPAWN_FROMSERVER;
        tParams.nTargetX = nFirstX;
        tParams.nTargetY = nFirstY;
    }

    D2UnitStrc *pMissile = MISSILE_ClientSpawn(&tParams);
    if (!pMissile)
        return;

    D2COMMON_UNITS_StoreOwnerTypeAndGUID(pMissile, pPacket->nUnitType, (D2UnitGUID)nOwnerGUID);
    if (pPacket->nPierce)
        D2COMMON_STATLIST_SetUnitStat(pMissile, STAT_PIERCE_IDX, pPacket->nPierce, 0);
    D2COMMON_MISSILE_SetCurrentFrame(pMissile, nCurrentFrame);
    pMissile->nSeqCurrentFramePrecise = (int32_t)(SEED_RollLimitedRandomNumberInline(&pMissile->pSeed, pMissile->dwFrameCountPrecise >> 8) << 8);
}

//1.10f: D2Client.0x6FAB40C0
//  Row 0x74: PLAYER_AssignCorpse (Player.cpp), from a twelve-byte stack copy (the packet's 10 bytes rounded up to four).
void __fastcall SCMD_HandleSrv74_AssignCorpse(D2GSPacketSrv74 *pPacket)
{
    uint8_t tPacket[12];
    memcpy(tPacket, pPacket, sizeof(D2GSPacketSrv74));
    PLAYER_AssignCorpse((D2GSPacketSrv74 *)tPacket);
}

//1.10f: D2Client.0x6FAB40F0
//  Row 0x75: ROSTER_UpdatePartyState. Its fourth argument is never read: nFlags, pushed from the same edx as nPartyId, lands in the second stack
//  slot, and the callee never loads that slot. nPartyId is in dx over the router's edx; the callee's one use of edx before reloading it is
//  `mov [esi+22h], dx` (0x6FAAF0CF).
void __fastcall SCMD_HandleSrv75_PartyRosterUpdate(D2GSPacketSrv75 *pPacket)
{
    ROSTER_UpdatePartyState(pPacket->nUnitGUID, (int16_t)pPacket->nPartyId, (int16_t)pPacket->nLevel, (int16_t)pPacket->nFlags, (D2C_PartyButtons)pPacket->unk0x0B);
}

//1.10f: D2Client.0x6FAB4110
//  Row 0x77: a jump to trade.cpp's TRADE_HandleStateMessage.
void __fastcall SCMD_HandleSrv77_TradeState(D2GSPacketSrv77 *pPacket)
{
    TRADE_HandleStateMessage(&pPacket->nHeader);
}

//1.10f: D2Client.0x6FAB4120
//  Row 0x78: trade.cpp's TRADE_SetPartnerFromPacket78, from a 24-byte stack copy (the packet's 21 bytes rounded up to four).
void __fastcall SCMD_HandleSrv78_TradePartner(D2GSPacketSrv78 *pPacket)
{
    uint8_t tPacket[24];
    memcpy(tPacket, pPacket, sizeof(D2GSPacketSrv78));
    TRADE_SetPartnerFromPacket78((D2GSPacketSrv78 *)tPacket);
}

//1.10f: D2Client.0x6FAB4150
//  Row 0x79: a jump to trade.cpp's TRADE_SetGold; bSelf is passed in dl, which is all TRADE_SetGold tests.
void __fastcall SCMD_HandleSrv79_TradeGold(D2GSPacketSrv79 *pPacket)
{
    TRADE_SetGold((int32_t)pPacket->dwGold, (uint8_t)pPacket->bSelf);
}

//1.10f: D2Client.0x6FAB4160
//  Row 0x7A: ROSTERPETS_FindOrAddPet, or ROSTERPETS_RemovePetOrMarkHireling by jump when the byte at +1 is zero.
void __fastcall SCMD_HandleSrv7A_RosterPetsOnPetPacket(D2GSPacketSrv7A *pPacket)
{
    if (pPacket->unk0x01)
        ROSTERPETS_FindOrAddPet(pPacket->unk0x09, pPacket->unk0x05, pPacket->unk0x02, pPacket->unk0x03);
    else
        ROSTERPETS_RemovePetOrMarkHireling(pPacket->unk0x09);
}

//1.10f: D2Client.0x6FAB4190
//  Row 0x81: ROSTERPETS_OnPetPacket81, with the dwords at +0x0C and +0x10 and a zero as a three-dword array.
void __fastcall SCMD_HandleSrv81_AddPet(D2GSPacketSrv81 *pPacket)
{
    uint32_t tPetInfo[3];
    tPetInfo[0] = pPacket->unk0x0C;
    tPetInfo[1] = pPacket->unk0x10;
    tPetInfo[2] = 0;
    ROSTERPETS_OnPetPacket81(pPacket->unk0x08, pPacket->unk0x04, pPacket->unk0x01, pPacket->unk0x02, tPetInfo);
}

//1.10f: D2Client.0x6FAB41D0
//  Row 0x7B: a hotkey slot: the low twelve bits of the word at +2 are the skill, bit 15 the mode. Two defects of the original, below.
void __fastcall SCMD_HandleSrv7B_AssignSkillHotkey(D2GSPacketSrv7B *pPacket)
{
    //  the skill is the low twelve bits and the hand bit 15, read as the bytes read them: the word, then `and eax, 0FFFh` and `and esi, 8000h`;
    //  a bit-field's position is the compiler's to choose, the masks are not
    uint16_t wSkillAndHand;
    memcpy(&wSkillAndHand, (const uint8_t *)pPacket + 2, sizeof(wSkillAndHand));

    int32_t nSkillId = wSkillAndHand & 0x0FFF;
    //  BUG: `jle` -- only an id ABOVE nSkillsTxtRecordCount becomes -1, so an id equal to the record count is stored as it came.
    if (nSkillId > sgptDataTables->nSkillsTxtRecordCount)
        nSkillId = -1;

    //  BUG: the slot byte (`mov bl, [ecx+1]`) is not bounded. It is SPELLSEL_SetHotkeySlot's index into three arrays, each followed 64 bytes on by
    //  another item (gnSpellSelSkillIds by gnSpellSelOwnerGUIDs, that by the array at 0x6FBB9D70, and gnSpellSelHotkeyMode by gSpellSelPanelX), so a
    //  slot above 15 writes into them.
    //  DIFFERENCE: in ours the arrays are objects of their own, so a slot above 15 is an index past one of them.
    SPELLSEL_SetHotkeySlot(pPacket->nSlot, nSkillId, (wSkillAndHand & 0x8000) != 0, pPacket->nItemGUID);
}

//1.10f: D2Client.0x6FAB4220
//  Row 0x7C: ITEM_OnItemRefPacket7C, from an eight-byte stack copy (the packet's 6 bytes rounded up to four).
void __fastcall SCMD_HandleSrv7C_ReleaseItemLock(D2GSPacketSrv7C *pPacket)
{
    uint8_t tPacket[8];
    memcpy(tPacket, pPacket, sizeof(D2GSPacketSrv7C));
    ITEM_OnItemRefPacket7C((D2GSPacketSrv7C *)tPacket);
}

//1.10f: D2Client.0x6FAB4240
//  Row 0x7D: ITEM_OnItemPairPacket7D, from a twenty-byte stack copy (the packet's 18 bytes rounded up to four).
void __fastcall SCMD_HandleSrv7D_ItemBreakRepair(D2GSPacketSrv7D *pPacket)
{
    uint8_t tPacket[20];
    memcpy(tPacket, pPacket, sizeof(D2GSPacketSrv7D));
    ITEM_OnItemPairPacket7D((D2GSPacketSrv7D *)tPacket);
}

//1.10f: D2Client.0x6FAB4280
//  Row 0x7E: GFX_LoadActCofData by jump, for the act of the level the player stands in. Nothing of the packet is read. The act is passed in cl;
//  GFX_LoadActCofData's second instruction is `and ecx, 0FFh` (0x6FB5C9E6).
void __fastcall SCMD_HandleSrv7E_LoadActCofData(D2GSPacketSrv7E *pPacket)
{
    //  BUG: CUNIT_GetPlayer()'s answer reaches D2Common's UNITS_GetRoom untested, which asserts "ptUnit" and exits on a null unit. Reachable only if
    //  0x7E arrives before the player's unit exists.
    D2ActiveRoomStrc *pRoom = D2COMMON_UNITS_GetRoom(CUNIT_GetPlayer());
    const D2C_Levels nLevelId = (D2C_Levels)D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);
    GFX_LoadActCofData(D2COMMON_DRLG_GetActNoFromLevelId(nLevelId));
}

//1.10f: D2Client.0x6FAB42A0
//  Row 0x7F: a life percentage and a level id, to the roster, or to the pet list when the byte at +1 is zero. nLevelId is pushed from si over
//  the router's esi; both callees mask it (`and eax, 0FFFFh`, 0x6FAB08C4 and 0x6FAB1A65).
void __fastcall SCMD_HandleSrv7F_PartyLifeUpdate(D2GSPacketSrv7F *pPacket)
{
    if (pPacket->bIsPlayer)
        ROSTER_UpdateLifeAndLevel(pPacket->nUnitGUID, pPacket->nLifePercentage, pPacket->nLevelId);
    else
        ROSTERPETS_UpdateLifeAndArea((uint32_t)pPacket->nUnitGUID, pPacket->nLifePercentage, pPacket->nLevelId);
}

//1.10f: D2Client.0x6FAB42D0
//  Row 0x90: ROSTER_SetEntryPosition.
void __fastcall SCMD_HandleSrv90_PartyPosition(D2GSPacketSrv90 *pPacket)
{
    ROSTER_SetEntryPosition(pPacket->dwGUID, (int32_t)pPacket->dwPosX, (int32_t)pPacket->dwPosY);
}

//1.10f: D2Client.0x6FAB42E0
//  Row 0x82: the owner name on each portal object this client holds, and the roster's two portal GUIDs when the second is not -1, whether or not
//  either object was found.
void __fastcall SCMD_HandleSrv82_PortalOwner(D2GSPacketSrv82 *pPacket)
{
    const uint32_t dwPortalGUID0 = pPacket->dwPortalGUID[0];
    const uint32_t dwOwnerGUID = pPacket->dwOwnerGUID;
    const uint32_t dwPortalGUID1 = pPacket->dwPortalGUID[1];

    D2UnitStrc *pPortal = CUNIT_FindUnitByGuidAndType(dwPortalGUID0, UNIT_OBJECT);
    if (pPortal)
        OBJECT_SetPortalOwnerName(pPortal, pPacket->szOwnerName);

    if (dwPortalGUID1 == (uint32_t)-1)
        return;

    pPortal = CUNIT_FindUnitByGuidAndType(dwPortalGUID1, UNIT_OBJECT);
    if (pPortal)
        OBJECT_SetPortalOwnerName(pPortal, pPacket->szOwnerName);
    ROSTER_SetEntryPortalGuids(dwOwnerGUID, dwPortalGUID0, dwPortalGUID1);
}

//1.10f: D2Client.0x6FAB4340
//  Row 0x89: a jump to QUESTSFX_FireTrigger, which reads the byte at +1.
void __fastcall SCMD_HandleSrv89_QuestTrigger(D2GSPacketSrv89 *pPacket)
{
    QUESTSFX_FireTrigger(pPacket);
}

//1.10f: D2Client.0x6FAB4370
//  Row 0x8B: a jump to ROSTER_SetPartyButton; the byte at +5 is passed in dl and masked there (`and edx, 0FFh`, 0x6FAB0FE3).
void __fastcall SCMD_HandleSrv8B_SetPartyButton(D2GSPacketSrv8B *pPacket)
{
    ROSTER_SetPartyButton(pPacket->nUnitGUID, pPacket->bParty);
}

//1.10f: D2Client.0x6FAB4380
//  Row 0x8C: ROSTER_ApplyInfoFlags, the word pushed over a dirty eax; the callee masks it (`and eax, 0FFFFh`, 0x6FAB104A).
void __fastcall SCMD_HandleSrv8C_PlayerRelation(D2GSPacketSrv8C *pPacket)
{
    ROSTER_ApplyInfoFlags(pPacket->dwPlayerGUID[0], pPacket->dwPlayerGUID[1], pPacket->nFlags);
}

//1.10f: D2Client.0x6FAB43A0
//  Row 0x8D: a jump to ROSTER_SetPartyId; nPartyGUID is passed in dx.
void __fastcall SCMD_HandleSrv8D_SetPartyId(D2GSPacketSrv8D *pPacket)
{
    ROSTER_SetPartyId((int32_t)pPacket->dwPlayerGUID, (int16_t)pPacket->nPartyGUID);
}

//1.10f: D2Client.0x6FAB43B0
//  Row 0x8E: ROSTER_FindCorpseEntry, or ROSTER_RemoveCorpseFromEntry when the byte at +1 is zero; both by jump.
void __fastcall SCMD_HandleSrv8E_RosterOnCorpseAssignment(D2GSPacketSrv8E *pPacket)
{
    if (pPacket->nType)
        ROSTER_FindCorpseEntry(pPacket->dwPlayerGUID, pPacket->dwCorpseGUID);
    else
        ROSTER_RemoveCorpseFromEntry((int32_t)pPacket->dwPlayerGUID, (int32_t)pPacket->dwCorpseGUID);
}

//1.10f: D2Client.0x6FAB43E0
//  Row 0x97: the weapon swap, by jump to inv.cpp's. It asserts its own header, which cannot fire through the router, whose row is chosen by that
//  byte.
void __fastcall SCMD_HandleSrv97_WeaponSwitch(D2GSPacketSrv97 *pPacket)
{
    if (pPacket->nHeader != 0x97)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR)szAssertCmdWeaponSwap, (uint32_t)(UINT_PTR)szFile, 3910);
        exit(-1);
    }
    INV_ToggleWeaponSwapSet();
}

//1.10f: D2Client.0x6FAB4410
//  Row 0x8F: a jump to GAME_OnPongUpdateLatency.
void __fastcall SCMD_HandleSrv8F_Pong(D2GSPacketSrv8F *pPacket)
{
    GAME_OnPongUpdateLatency(&pPacket->nHeader);
}


//  Packets 0x9C, 0x9D, 0xA6, 0xA7, 0xA8 and 0xA9: item messages, rune words and states. gszAssertItemUnitExMsg is SCmd.h's. The four texts after
//  the warning string are the original's trace texts, in the image at 0x6FB76BC8, 0x6FB76C98, 0x6FB76C38 and 0x6FB76D08.
static const char szWarnItemExMsgs[] = "These should be scmd_itemex messages!";           // 0x6FB76B9C
static const char szTraceStateOnNoUnit[] = "WARNING: sSCmd_UnitStateOnEx, Attempt to change state on a unit the client doesn't have!  Type: %d, Id: %d\n";
static const char szTraceStateOnValNoUnit[] = "WARNING: sSCmd_UnitStateOnValEx, Attempt to change state on a unit the client doesn't have!  Type: %d, Id: %d\n";
static const char szTraceStateOnValBadStat[] = "WARNING: sSCmd_UnitStateOnValEx, Attempt to set an invalid stat!  Type: %d, Id: %d, Stat: %d\n";
static const char szTraceStateOffNoUnit[] = "WARNING: sSCmd_UnitStateOffEx, Attempt to change state on a unit the client doesn't have!  Type: %d, Id: %d\n";
static_assert(sizeof(szWarnItemExMsgs) == 37 + 1
              && sizeof(szTraceStateOnNoUnit) == 107 + 1 && sizeof(szTraceStateOnValNoUnit) == 110 + 1
              && sizeof(szTraceStateOnValBadStat) == 93 + 1 && sizeof(szTraceStateOffNoUnit) == 108 + 1,
              "the five texts at 0x6FB76B9C..0x6FB76D08, measured in the DLL");

static_assert(sizeof(D2GSPacketSrv9C) == 252 && offsetof(D2GSPacketSrv9C, nAction) == 1,
              "0x9C: mov ecx, 3Fh / rep movsd, and the subtype from [esp+104h+var_FB] at 0x6FAB4435");
static_assert(sizeof(D2GSPacketSrv9D) == 257 && offsetof(D2GSPacketSrv9D, nAction) == 1,
              "0x9D: mov ecx, 40h / rep movsd / movsb, and the subtype at 0x6FAB45A6");
static_assert(offsetof(D2GSPacketSrvA6, unk0x01) == 1 && offsetof(D2GSPacketSrvA6, nSize) == 2
              && offsetof(D2GSPacketSrvA6, nRecordId) == 4 && offsetof(D2GSPacketSrvA6, tRecord) == 6
              && sizeof(D2RunesTxt) == 0x48 * 4,
              "0xA6: [ecx+1], [ecx+4], lea eax, [ecx+6]; D2Net's word at +2; D2Common copies 0x48 dwords");
static_assert(offsetof(D2GSPacketSrvA7, nUnitType) == 1 && offsetof(D2GSPacketSrvA7, nUnitId) == 2
              && offsetof(D2GSPacketSrvA7, nState) == 6 && offsetof(D2GSPacketSrvA9, nUnitType) == 1
              && offsetof(D2GSPacketSrvA9, nUnitId) == 2 && offsetof(D2GSPacketSrvA9, nState) == 6,
              "0xA7 and 0xA9: [esi+1], [esi+2], mov dl, [esi+6]");
static_assert(offsetof(D2GSPacketSrvA8, nUnitType) == 1 && offsetof(D2GSPacketSrvA8, nUnitId) == 2
              && offsetof(D2GSPacketSrvA8, nSize) == 6 && offsetof(D2GSPacketSrvA8, nState) == 7
              && offsetof(D2GSPacketSrvA8, pStream) == 8,
              "0xA8: [ebx+1], [ebx+2], mov al, [ebx+6], mov dl, [ebx+7], lea ecx, [ebx+8]");
static_assert(offsetof(D2DataTablesStrc, pItemStatCostTxt) == 0xBCC
              && offsetof(D2DataTablesStrc, nItemStatCostTxtRecordCount) == 0xBD4,
              "0xA8: mov eax, [ecx+0BCCh] and cmp esi, [ecx+0BD4h]");
static_assert(offsetof(D2ItemStatCostTxt, dwItemStatFlags) == 4 && offsetof(D2ItemStatCostTxt, nSendBits) == 8
              && offsetof(D2ItemStatCostTxt, nSendParamBits) == 9 && ITEMSTATCOSTFLAGINDEX_SIGNED == 1,
              "0xA8: mov dl, [edi+4] against [gdwBitMasks+4], mov al, [edi+8], mov al, [edi+9]");

//1.10f: D2Client.0x6FAB4420
//  Row 0x9C: an item message. The packet is copied whole into 252 bytes, then a switch on its subtype at +1 into twelve ITEMS_ItemMsg calls (0x0B
//  and 0x0C share one). The eleven subtypes 0x9D takes (0x05..0x09, 0x11, 0x13..0x17) give the warning "These should be scmd_itemunitex
//  messages!". Above 0x17 it does nothing.
void __fastcall SCMD_HandleSrv9C_ItemDropPickup(D2GSPacketSrv9C *pPacket)
{
    //  BUG: 252 bytes (`mov ecx, 3Fh / rep movsd`) are copied whatever the packet's length; a shorter packet's copy takes the rest from the router's
    //  buffer, which is 512 bytes with the packet at its head, so the copy stays inside it.
    D2GSPacketSrv9C tPacket;
    memcpy(&tPacket, pPacket, sizeof(tPacket));

    switch (tPacket.nAction)
    {
    case 0x00:
        ITEM_ItemMsg00_PutOnGround(&tPacket);
        break;
    case 0x01:
        ITEM_ItemMsg01_ToCursor(&tPacket);
        break;
    case 0x02:
        ITEM_ItemMsg02_DropToGround(&tPacket);
        break;
    case 0x03:
        ITEM_ItemMsg03_OnGround(&tPacket);
        break;
    case 0x04:
        ITEM_ItemMsg04_ToStorePage(&tPacket);
        break;
    case 0x05:
    case 0x06:
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x11:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
        FOG_DisplayWarning((uint32_t)(UINT_PTR)gszAssertItemUnitExMsg, (uint32_t)(UINT_PTR)szFile, 3949);
        break;
    case 0x0A:
        ITEM_ItemMsg0A_UpdateItem(&tPacket);
        break;
    case 0x0B:
    case 0x0C:
        ITEM_ItemMsg0B0C_UpdateNpcStore(&tPacket);
        break;
    case 0x0D:
        ITEM_ItemMsg0D_SwapInContainer(&tPacket);
        break;
    case 0x0E:
        ITEM_ItemMsg0E_SetInvPage(&tPacket);
        break;
    case 0x0F:
        ITEM_ItemMsg0F_RemoveFromBelt(&tPacket);
        break;
    case 0x10:
        ITEM_ItemMsg10_SwapInBelt(&tPacket);
        break;
    case 0x12:
        ITEM_ItemMsg12_RecreateToCursor(&tPacket);
        break;
    default:
        break;
    }
}

//1.10f: D2Client.0x6FAB4590
//  Row 0x9D: an item message that also names a unit. The packet is copied into a 260-byte buffer (257 bytes), then a switch on its subtype into
//  eleven calls. The thirteen subtypes 0x9C takes give the warning "These should be scmd_itemex messages!". Above 0x17 it does nothing.
void __fastcall SCMD_HandleSrv9D_ItemEquip(D2GSPacketSrv9D *pPacket)
{
    //  BUG: 257 bytes (`mov ecx, 40h / rep movsd / movsb`) are copied whatever the packet's length, as in 0x9C. A buffer holding two packets, the
    //  second a 0x9D past offset 255, would read past the 512, which D2Net's one packet per dequeue rules out.
    uint8_t tBuffer[260];
    memcpy(tBuffer, pPacket, sizeof(D2GSPacketSrv9D));
    D2GSPacketSrv9D *ptPacket = (D2GSPacketSrv9D *)tBuffer;

    switch (ptPacket->nAction)
    {
    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x0A:
    case 0x0B:
    case 0x0C:
    case 0x0D:
    case 0x0E:
    case 0x0F:
    case 0x10:
    case 0x12:
        FOG_DisplayWarning((uint32_t)(UINT_PTR)szWarnItemExMsgs, (uint32_t)(UINT_PTR)szFile, 4023);
        break;
    case 0x05:
        ITEM_ItemMsg05_ToInventoryGrid(ptPacket);
        break;
    case 0x06:
        ITEM_ItemMsg06_InsertIntoBodyLoc(ptPacket);
        break;
    case 0x07:
        ITEM_ItemMsg07_SwapBodyLocItems(ptPacket);
        break;
    case 0x08:
        ITEM_ItemMsg08_RemoveFromBodyLoc(ptPacket);
        break;
    case 0x09:
        ITEM_ItemMsg09_EquipOnPlayer(ptPacket);
        break;
    case 0x11:
        ITEM_ItemMsg11_AutoUnequip(ptPacket);
        break;
    case 0x13:
        ITEM_ItemMsg13_ItemInSocket(ptPacket);
        break;
    case 0x14:
        ITEM_ItemMsg14_Unknown(ptPacket);
        break;
    case 0x15:
        ITEM_ItemMsg15_UpdateStats(ptPacket);
        break;
    case 0x16:
        ITEM_ItemMsg16_Unknown(ptPacket);
        break;
    case 0x17:
        ITEM_ItemMsg17_WeaponSwitch(ptPacket);
        break;
    default:
        break;
    }
}

//1.10f: D2Client.0x6FAB46E0
//  Row 0xA6: a runes.txt record added or changed, DATATBLS_AddOrChangeRunesTxtRecord with the word at +4 and the 288 bytes from +6, when the
//  byte at +1 is zero.
void __fastcall SCMD_HandleSrvA6_RunesRecord(D2GSPacketSrvA6 *pPacket)
{
    if (!pPacket->unk0x01)
        D2COMMON_DATATBLS_AddOrChangeRunesTxtRecord(pPacket->nRecordId, &pPacket->tRecord);
}

//1.10f: D2Client.0x6FAB4700
//  Row 0xA7: a state on: STATES_ToggleStateWithSound, then PLRSKILLS_StatesRunStateSetFunc by tail jump. A missing unit is a FOG_Trace
//  ("sSCmd_UnitStateOnEx") and a return.
void __fastcall SCMD_HandleSrvA7_StateOn(D2GSPacketSrvA7 *pPacket)
{
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType((uint32_t)pPacket->nUnitId, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pUnit)
    {
        FOG_Trace(szTraceStateOnNoUnit, pPacket->nUnitType, pPacket->nUnitId);
        return;
    }

    const D2C_States nState = (D2C_States)pPacket->nState;
    STATES_ToggleStateWithSound(pUnit, nState);
    PLRSKILLS_StatesRunStateSetFunc(pUnit, nState);
}

//1.10f: D2Client.0x6FAB4750
//  Row 0xA8: a state on with its stats: after the toggle, a bit stream of 9-bit stat ids ending at 511, each with a layer of nSendParamBits and a
//  value of nSendBits (signed when ItemStatCost's SIGNED flag is set and the width is below 32) into PLRSKILLS_SetStateStat. A missing unit, a
//  stat id out of range or no send bits is a FOG_Trace ("sSCmd_UnitStateOnValEx") and a return.
void __fastcall SCMD_HandleSrvA8_StateOnWithStats(D2GSPacketSrvA8 *pPacket)
{
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType((uint32_t)pPacket->nUnitId, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pUnit)
    {
        FOG_Trace(szTraceStateOnValNoUnit, pPacket->nUnitType, pPacket->nUnitId);
        return;
    }

    const int32_t nState = pPacket->nState;
    STATES_ToggleStateWithSound(pUnit, (D2C_States)nState);

    D2BitBufferStrc tBits;
    //  BUG: the bit stream is sized as the length byte minus 8 (`sub eax, 8` at 0x6FAB47AF) with no test, as in 0x3E: a length below 8 hands
    //  BITMANIP_Initialize a size near 4 GB.
    FOG_BITMANIP_Initialize(&tBits, pPacket->pStream, (size_t)(pPacket->nSize - 8));

    D2StatListStrc *pStatList = NULL;
    for (int32_t nStatId = (int32_t)FOG_BITMANIP_Read(&tBits, 9); nStatId < 511; nStatId = (int32_t)FOG_BITMANIP_Read(&tBits, 9))
    {
        D2ItemStatCostTxt *pStatCostTxt = NULL;
        if (nStatId < 0 || nStatId >= sgptDataTables->nItemStatCostTxtRecordCount || !(pStatCostTxt = &sgptDataTables->pItemStatCostTxt[nStatId]) || !pStatCostTxt->nSendBits)
        {
            FOG_Trace(szTraceStateOnValBadStat, pPacket->nUnitType, pPacket->nUnitId, nStatId);
            return;
        }

        int32_t nLayer = 0;
        if (pStatCostTxt->nSendParamBits)
            nLayer = FOG_BITMANIP_ReadSigned(&tBits, pStatCostTxt->nSendParamBits);

        int32_t nValue;
        if (pStatCostTxt->nSendBits < 32 && ((uint8_t)pStatCostTxt->dwItemStatFlags & (uint8_t)gdwBitMasks[ITEMSTATCOSTFLAGINDEX_SIGNED]))
            nValue = FOG_BITMANIP_ReadSigned(&tBits, pStatCostTxt->nSendBits);
        else
            nValue = (int32_t)FOG_BITMANIP_Read(&tBits, pStatCostTxt->nSendBits);

        //  DIFFERENCE: BITMANIP_ReadSigned answers an int, so a negative layer reaches PLRSKILLS_SetStateStat in the original with every upper bit
        //  set; ours passes the low word, the prototype's uint16_t. The difference is dead: the callee hands the slot only to D2Common's stat
        //  functions, which mask it (`and ecx, 0FFFFh`, `and eax, 0FFFFh`).
        pStatList = PLRSKILLS_SetStateStat(pUnit, pStatList, (D2C_States)nState, (D2C_ItemStats)nStatId, nValue, (uint16_t)nLayer);
    }

    PLRSKILLS_StatesRunStateSetFunc(pUnit, (D2C_States)nState);
}

//1.10f: D2Client.0x6FAB4900
//  Row 0xA9: a state off: PLRSKILLS_StatesRunStateRemFunc, then PLRSKILLS_EndStateAndSkill by tail jump. A missing unit is a FOG_Trace
//  ("sSCmd_UnitStateOffEx") and a return.
void __fastcall SCMD_HandleSrvA9_StateOff(D2GSPacketSrvA9 *pPacket)
{
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType((uint32_t)pPacket->nUnitId, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pUnit)
    {
        FOG_Trace(szTraceStateOffNoUnit, pPacket->nUnitType, pPacket->nUnitId);
        return;
    }

    const int32_t nState = pPacket->nState;
    PLRSKILLS_StatesRunStateRemFunc(pUnit, (D2C_States)nState);
    PLRSKILLS_EndStateAndSkill(pUnit, (D2C_States)nState);
}


//  Packets 0xAA, 0xAB, 0xAC and 0xAD: state lists, health and monsters. The four texts below are the original's trace texts, in the image at
//  0x6FB76DD8, 0x6FB76D78, 0x6FB76E48 and 0x6FB76EB8.

static const char szTraceStateAllNoUnit[] = "WARNING: sSCmd_UnitStateAllEx, Attempt to change state on a unit the client doesn't have!  Type: %d, Id: %d\n";
static const char szTraceStateAllBadStat[] = "WARNING: sSCmd_UnitStateAllEx, Attempt to set an invalid stat!  Type: %d, Id: %d, Stat: %d\n";
static const char szTraceHealthNoUnit[] = "WARNING: sSCmd_HealthUpdate, Attempt to update health on a unit the client doesn't have!  Type: %d, Id: %d\n";
static const char szTraceNewMonsterBadStat[] = "WARNING: sSCmd_NewMonsterEx, Attempt to set an invalid stat!  Class: %d, Stat: %d\n";
static_assert(sizeof(szTraceStateAllNoUnit) == 108 + 1 && sizeof(szTraceStateAllBadStat) == 91 + 1
              && sizeof(szTraceHealthNoUnit) == 107 + 1 && sizeof(szTraceNewMonsterBadStat) == 82 + 1,
              "the four texts at 0x6FB76D78..0x6FB76EB8, measured in the DLL");

static_assert(offsetof(D2GSPacketSrvAA, nUnitType) == 1 && offsetof(D2GSPacketSrvAA, nUnitId) == 2
              && offsetof(D2GSPacketSrvAA, nSize) == 6 && offsetof(D2GSPacketSrvAA, pStream) == 7,
              "0xAA: [esi+1], [esi+2], mov dl, [esi+6], add esi, 7");
static_assert(offsetof(D2GSPacketSrvAB, nUnitType) == 1 && offsetof(D2GSPacketSrvAB, dwUnitGUID) == 2
              && offsetof(D2GSPacketSrvAB, nLife) == 6,
              "0xAB: [esi+1], [esi+2], mov ah, [esi+6]");
static_assert(offsetof(D2GSPacketSrvAC, dwUnitGUID) == 1 && offsetof(D2GSPacketSrvAC, nUnitIndex) == 5
              && offsetof(D2GSPacketSrvAC, nPosX) == 7 && offsetof(D2GSPacketSrvAC, nPosY) == 9
              && offsetof(D2GSPacketSrvAC, nLifePct) == 0x0B && offsetof(D2GSPacketSrvAC, nPacketLength) == 0x0C
              && offsetof(D2GSPacketSrvAC, bitstream) == 0x0D,
              "0xAC: [ebp+1], [ebp+5], [ebp+7], [ebp+9], [ebp+0Bh], [ebp+0Ch], lea ecx, [ebp+0Dh]");
static_assert(offsetof(D2GSPacketSrvAD, unk0x01) == 1 && offsetof(D2GSPacketSrvAD, unk0x05) == 5,
              "0xAD: mov eax, [ecx+1] and mov ecx, [ecx+5]");
static_assert(offsetof(D2DataTablesStrc, pMonStatsTxt) == 0xA78
              && offsetof(D2DataTablesStrc, pMonStats2Txt) == 0xA90
              && offsetof(D2DataTablesStrc, nMonStats2TxtRecordCount) == 0xA98,
              "0xAC: [edx+0A78h], [edx+0A90h], [edx+0A98h]");
static_assert(offsetof(D2MonStatsTxt, wMonStatsEx) == 0x18 && sizeof(D2MonStatsTxt) == 53 * 8,
              "0xAC: movsx eax, word ptr [eax+ecx*8+18h], the stride built as 53 * 8");
static_assert(offsetof(D2MonStats2Txt, nComponentChoiceCounts) == 0x15 && sizeof(D2MonStats2Txt) == 77 * 4,
              "0xAC: mov al, [esi+edi+15h], the stride built as 77 * 4");
static_assert(STAT_MAXHP == 7, "0xAC: push 7 at 0x6FAB4EB2, beside STAT_HITPOINTS's 6");

//1.10f: D2Client.0x6FAB4950
//  Row 0xAA: a whole state list: 8-bit state ids ending at 255, each toggled with sound, each optionally followed by 0xA8's stat stream, each
//  closed with PLRSKILLS_StatesRunStateSetFunc. A missing unit is a FOG_Trace ("sSCmd_UnitStateAllEx") and a return.
void __fastcall SCMD_HandleSrvAA_AllStates(D2GSPacketSrvAA *pPacket)
{
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType((uint32_t)pPacket->nUnitId, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pUnit)
    {
        FOG_Trace(szTraceStateAllNoUnit, pPacket->nUnitType, pPacket->nUnitId);
        return;
    }

    D2BitBufferStrc tBits;
    //  BUG: the bit stream is sized as the length byte minus 7 (`sub edx, 7` at 0x6FAB499F) with no test: a length below 7 wraps.
    FOG_BITMANIP_Initialize(&tBits, pPacket->pStream, (size_t)(pPacket->nSize - 7));

    for (int32_t nState = (int32_t)FOG_BITMANIP_Read(&tBits, 8); nState < 255; nState = (int32_t)FOG_BITMANIP_Read(&tBits, 8))
    {
        STATES_ToggleStateWithSound(pUnit, (D2C_States)nState);

        if (FOG_BITMANIP_Read(&tBits, 1))
        {
            D2StatListStrc *pStatList = NULL;
            for (int32_t nStatId = (int32_t)FOG_BITMANIP_Read(&tBits, 9); nStatId < 511; nStatId = (int32_t)FOG_BITMANIP_Read(&tBits, 9))
            {
                D2ItemStatCostTxt *pStatCostTxt;
                if (nStatId < 0 || nStatId >= sgptDataTables->nItemStatCostTxtRecordCount || !(pStatCostTxt = &sgptDataTables->pItemStatCostTxt[nStatId]))
                {
                    FOG_Trace(szTraceStateAllBadStat, pPacket->nUnitType, pPacket->nUnitId, nStatId);
                    return;
                }

                //  BUG: a zero nSendBits prints 0xA8's text ("sSCmd_UnitStateOnValEx", 0x6FAB4B22), where the id-bound failure prints
                //  "sSCmd_UnitStateAllEx" (0x6FAB4AFE). Reproduced.
                if (!pStatCostTxt->nSendBits)
                {
                    FOG_Trace(szTraceStateOnValBadStat, pPacket->nUnitType, pPacket->nUnitId, nStatId);
                    return;
                }

                int32_t nLayer = 0;
                if (pStatCostTxt->nSendParamBits)
                    nLayer = FOG_BITMANIP_ReadSigned(&tBits, pStatCostTxt->nSendParamBits);

                int32_t nValue;
                if (pStatCostTxt->nSendBits < 32 && ((uint8_t)pStatCostTxt->dwItemStatFlags & (uint8_t)gdwBitMasks[ITEMSTATCOSTFLAGINDEX_SIGNED]))
                    nValue = FOG_BITMANIP_ReadSigned(&tBits, pStatCostTxt->nSendBits);
                else
                    nValue = (int32_t)FOG_BITMANIP_Read(&tBits, pStatCostTxt->nSendBits);

                pStatList = PLRSKILLS_SetStateStat(pUnit, pStatList, (D2C_States)nState, (D2C_ItemStats)nStatId, nValue, (uint16_t)nLayer);
            }
        }

        PLRSKILLS_StatesRunStateSetFunc(pUnit, (D2C_States)nState);
    }
}

//1.10f: D2Client.0x6FAB4B40
//  Row 0xAB: a health byte: for a UNIT_MONSTER it becomes STAT_HITPOINTS as `nLife << 8`, for anything else the party roster's percentage,
//  `100 * nLife / 128`, by tail jump. The local player and any unit with dwFlags bit 9 are skipped. A missing unit is a FOG_Trace
//  ("sSCmd_HealthUpdate").
void __fastcall SCMD_HandleSrvAB_UnitHealth(D2GSPacketSrvAB *pPacket)
{
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(pPacket->dwUnitGUID, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pUnit)
    {
        FOG_Trace(szTraceHealthNoUnit, pPacket->nUnitType, pPacket->dwUnitGUID);
        return;
    }

    if (pUnit == CUNIT_GetPlayer())
        return;
    if ((pUnit->dwFlags >> 9) & 1)
        return;

    if (pUnit->dwUnitType == UNIT_MONSTER)
        D2COMMON_STATLIST_SetUnitStat(pUnit, STAT_HITPOINTS, pPacket->nLife << 8, 0);
    else
        ROSTER_SetPartyLife(pPacket->dwUnitGUID, (uint8_t)(100 * pPacket->nLife / 128));
}

//1.10f: D2Client.0x6FAB4BD0
//  Row 0xAC: a monster, its components, its unique modifiers and its stats. A fixed head, then a bit stream: a 4-bit anim mode, a bit for
//  sixteen component choices, a bit for the unique block (five type-flag bits, a 16-bit superunique index when bit 2 is set, a 0-terminated run
//  of modifier bytes, a name seed, an optional dword), then CUNIT_CreateMonsterUnit, the life stats, an optional 31-bit owner GUID and an
//  optional stat stream ("sSCmd_NewMonsterEx" is the trace for a bad stat).
//  Each of the sixteen component choices is read at the width its slot's choice count needs: a count below 3 is one bit and anything else is
//  floor(log2(count - 1)) + 1; ours is `_BitScanReverse`, the original's `bsr`. The record is reached the long way and each step is a bound: the
//  class id against nMonStatsTxtRecordCount, then monstats.txt's wMonStatsEx (+0x18, stride 424) against nMonStats2TxtRecordCount, then
//  monstats2.txt's record (stride 308) and its sixteen counts at +0x15. Any failed bound returns: the monster is not created at all.
void __fastcall SCMD_HandleSrvAC_SpawnMonster(D2GSPacketSrvAC *pPacket)
{
    const uint32_t dwUnitGUID = pPacket->dwUnitGUID;
    const int16_t nMonsterClassId = (int16_t)pPacket->nUnitIndex;
    const uint16_t nPosX = pPacket->nPosX;
    const uint16_t nPosY = pPacket->nPosY;

    D2BitBufferStrc tBits;
    //  BUG: the bit stream is sized as the length byte minus 13 (`sub eax, 0Dh` at 0x6FAB4BFA) with no test: a length below 13 wraps.
    FOG_BITMANIP_Initialize(&tBits, pPacket->bitstream, (size_t)(pPacket->nPacketLength - 13));

    const uint8_t nAnimMode = (uint8_t)FOG_BITMANIP_Read(&tBits, 4);

    uint8_t tComponents[16];
    uint8_t *pComponents = NULL;
    if (FOG_BITMANIP_Read(&tBits, 1))
    {
        if (nMonsterClassId < 0 || nMonsterClassId >= sgptDataTables->nMonStatsTxtRecordCount)
            return;

        const int32_t nMonStatsEx = (int16_t)sgptDataTables->pMonStatsTxt[nMonsterClassId].wMonStatsEx;
        if (nMonStatsEx < 0 || nMonStatsEx >= sgptDataTables->nMonStats2TxtRecordCount)
            return;

        D2MonStats2Txt *pMonStats2Txt = &sgptDataTables->pMonStats2Txt[nMonStatsEx];
        if (!pMonStats2Txt)
            return;

        pComponents = tComponents;
        for (int32_t i = 0; i < 16; ++i)
        {
            const int32_t nChoiceCount = pMonStats2Txt->nComponentChoiceCounts[i];
            int32_t nBits = 1;
            if (nChoiceCount >= 3)
            {
                //  the dead arm of the width helper is the `? : 0`: the arm for count - 1 == 0 (0x6FAB4CC6) cannot be reached through the `jge 3`; it
                //  is what an inlined width helper leaves behind
                unsigned long nHighBit;     //  _BitScanReverse's own `unsigned long *`
                nBits = _BitScanReverse(&nHighBit, (uint32_t)(nChoiceCount - 1)) ? (int32_t)nHighBit + 1 : 0;
            }
            tComponents[i] = (uint8_t)FOG_BITMANIP_Read(&tBits, nBits);
        }
    }

    uint8_t tMonUmods[9];
    uint8_t *pMonUmods = NULL;
    uint16_t wNameSeed = 0;
    uint32_t dwHirelingOwnerGUID = (uint32_t)-1;
    int32_t nBossHcIdx = 0;
    int32_t nTypeFlags = 0;
    if (FOG_BITMANIP_Read(&tBits, 1))
    {
        nTypeFlags = FOG_BITMANIP_Read(&tBits, 1) ? 0x04 : 0;
        nTypeFlags |= FOG_BITMANIP_Read(&tBits, 1) ? 0x08 : 0;
        nTypeFlags |= FOG_BITMANIP_Read(&tBits, 1) ? 0x02 : 0;
        nTypeFlags |= FOG_BITMANIP_Read(&tBits, 1) ? 0x10 : 0;
        nTypeFlags |= FOG_BITMANIP_Read(&tBits, 1) ? 0x40 : 0;
        if (nTypeFlags & 2)
            nBossHcIdx = FOG_BITMANIP_ReadSigned(&tBits, 16);

        memset(tMonUmods, 0, sizeof(tMonUmods));
        pMonUmods = tMonUmods;

        uint8_t nMonUmod = (uint8_t)FOG_BITMANIP_Read(&tBits, 8);
        if (nMonUmod)
        {
            int32_t nWritten = 0;
            do
            {
                //  BUG: the run of modifier ids is stored with no bound into nine bytes cleared at [esp+38h] (`inc esi` and no limit,
                //  0x6FAB4DB7..0x6FAB4DCC), so a run longer than nine writes on into the frame.
                //  DIFFERENCE: ours stores the first nine and keeps reading the rest, which consumes the same bits and hands CUNIT_CreateMonsterUnit
                //  the same nine bytes. Identical for every run the buffer holds; different, and deliberately, past it, because writing past an array
                //  is undefined.
                if (nWritten < (int32_t)sizeof(tMonUmods))
                    tMonUmods[nWritten++] = nMonUmod;
                nMonUmod = (uint8_t)FOG_BITMANIP_Read(&tBits, 8);
            } while (nMonUmod);
        }

        //  BUG: sixteen bits are read and only the low eight kept (`call BITMANIP_Read(16)`, then `movzx si, al`, 0x6FAB4DE1).
        wNameSeed = (uint8_t)FOG_BITMANIP_Read(&tBits, 16);
        if (FOG_BITMANIP_Read(&tBits, 1))
            dwHirelingOwnerGUID = FOG_BITMANIP_Read(&tBits, 32);
    }

    int32_t tAssign[7];
    memset(tAssign, 0, sizeof(tAssign));
    tAssign[0] = (int32_t)dwUnitGUID;
    tAssign[1] = nMonsterClassId;
    tAssign[2] = nPosX;
    tAssign[3] = nPosY;
    tAssign[4] = nTypeFlags & 0xFFFF;
    tAssign[5] = (int32_t)dwHirelingOwnerGUID;
    tAssign[6] = nAnimMode;

    PALSHIFT_EnsureMonsterPalette((D2C_MonsterIds)nMonsterClassId);

    D2UnitStrc *pMonster = CUNIT_CreateMonsterUnit(tAssign, pComponents, pMonUmods, wNameSeed, nBossHcIdx, 0);
    if (!pMonster)
        return;

    GFX_PreloadUnit(pMonster);
    if (!D2COMMON_MONSTERS_GetHirelingTypeId(pMonster) || (int32_t)dwUnitGUID != ROSTERPETS_GetAssociatedUnitGUID(CUNIT_GetPlayer(), 7, 1))
    {
        D2COMMON_STATLIST_SetUnitStat(pMonster, STAT_MAXHP, 0x8000, 0);
        D2COMMON_STATLIST_SetUnitStat(pMonster, STAT_HITPOINTS, pPacket->nLifePct << 8, 0);
    }

    if (D2COMMON_MONSTERS_GetHirelingTypeId(pMonster) && nAnimMode == 1)
        pMonster->dwFlagEx &= ~0x40000u;

    //  BUG: STAT_PIERCE_IDX gets the two coordinates added together (`mov eax, [esp+24h] / mov ecx, [esp+28h] / add / and eax, 0FFFFh` at
    //  0x6FAB4EE5; those two slots hold nPosY and nPosX, the layer slot is not reused for the stat layer until 0x6FAB4F98).
    D2COMMON_STATLIST_SetUnitStat(pMonster, STAT_PIERCE_IDX, (uint16_t)(nPosY + nPosX), 0);

    if (FOG_BITMANIP_Read(&tBits, 1))
    {
        const D2UnitGUID dwOwnerGUID = FOG_BITMANIP_Read(&tBits, 31);
        D2COMMON_UNITS_StoreOwnerTypeAndGUID(pMonster, 0, dwOwnerGUID);
    }

    if (!FOG_BITMANIP_Read(&tBits, 1))
        return;

    D2StatListStrc *pStatList = NULL;
    for (int32_t nStatId = (int32_t)FOG_BITMANIP_Read(&tBits, 9); nStatId < 511; nStatId = (int32_t)FOG_BITMANIP_Read(&tBits, 9))
    {
        D2ItemStatCostTxt *pStatCostTxt;
        if (nStatId < 0 || nStatId >= sgptDataTables->nItemStatCostTxtRecordCount || !(pStatCostTxt = &sgptDataTables->pItemStatCostTxt[nStatId]) || !pStatCostTxt->nSendBits)
        {
            FOG_Trace(szTraceNewMonsterBadStat, (int16_t)pPacket->nUnitIndex, nStatId);
            return;
        }

        int32_t nLayer = 0;
        if (pStatCostTxt->nSendParamBits)
            nLayer = FOG_BITMANIP_ReadSigned(&tBits, pStatCostTxt->nSendParamBits);

        int32_t nValue;
        if (pStatCostTxt->nSendBits < 32 && ((uint8_t)pStatCostTxt->dwItemStatFlags & (uint8_t)gdwBitMasks[ITEMSTATCOSTFLAGINDEX_SIGNED]))
            nValue = FOG_BITMANIP_ReadSigned(&tBits, pStatCostTxt->nSendBits);
        else
            nValue = (int32_t)FOG_BITMANIP_Read(&tBits, pStatCostTxt->nSendBits);

        if (!pStatList)
        {
            pStatList = D2COMMON_STATLIST_GetStatListFromUnitStateAndFlag(pMonster, STATE_NONE, 0x40);
            if (!pStatList)
            {
                pStatList = D2COMMON_STATLIST_AllocStatList(NULL, 0x40, 0, (D2C_UnitTypes)pMonster->dwUnitType, pMonster->dwUnitId);
                D2COMMON_STATLIST_SetState(pStatList, STATE_NONE);
                D2COMMON_10475_PostStatToStatList(pMonster, pStatList, 1);
            }
        }

        if (pStatList)
            D2COMMON_STATLIST_SetStatIfListIsValid(pStatList, (D2C_ItemStats)nStatId, nValue, (uint16_t)nLayer);
    }
}

//1.10f: D2Client.0x6FAB5090
//  Row 0xAD: the server's two probe addresses, the dwords at +1 and +5, stored in gnServerProbeAddr1 and gnServerProbeAddr2 for
//  UNITMODE_FlushServerMemoryProbe to answer.
void __fastcall SCMD_HandleSrvAD_ServerMemoryProbe(D2GSPacketSrvAD *pPacket)
{
    gnServerProbeAddr1 = (uint32_t)pPacket->unk0x01;
    gnServerProbeAddr2 = (uint32_t)pPacket->unk0x05;
}

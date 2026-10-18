//  npcmenu.cpp -- the NPC conversation and transaction screens of D2Client.dll 1.10: the state of a conversation (who the NPC is, which dialog is
//  up, what was already said), the main menu and its quest unlocks, the speech popups and queue, the hire list, the transaction (price, wait,
//  answer, confirm dialog), the teardown, and the packet handlers that open and answer a conversation.
//  The original source file is UI\npcmenu.cpp (the asserts in this unit name it). Address range 0x6FAF3930..0x6FAFC232.

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "CoreArchive.h"
#include "npcmenu.h"

//  The original's __FILE__ for this unit, pushed by every assert and trace that names its file.
static const char szSourceFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UI\\npcmenu.cpp";
static_assert(sizeof(szSourceFile) == 58 + 1, "the path is 58 characters at 0x6FB7C510");

//  The data root NPCMENU_Init pushes in front of its two image paths.
static const char aDataGlobal[] = "DATA\\GLOBAL";

//  Three tables carry the screen's state: gNpcMenuEntries (46 records of 22 bytes), gNpcMenuDescriptors (48 records of 39 bytes) and the hireling
//  offers (10 records of 16 bytes). D2NpcMenuEntryStrc is packed (pSpeechLineTable sits at +5).
//  The seven hDialog* handles of sgUIState are one set: NPCMENU_IsAnyDialogOpen ORs all seven and NPCMENU_CloseAllDialogs closes all seven.
//
//  gbNpcVendorGamble is a flag, not a mode: it only ever holds 0 (buy) or 1 (gamble). Of its four readers, three turn a set flag into
//  nTransactionType 2 (gamble) for ITEMS_GetTransactionCost, and NPCMENU_IsVendorGamble returns it.
//
//  The asserts below pin the offsets the code leans on, not only the sizes.
static_assert(sizeof(D2NpcMenuEntryStrc) == 22, "the 22-byte stride; 46 x 22 ends at gnNpcMenuEntryCount");
static_assert(offsetof(D2NpcMenuEntryStrc, unk0x12) == 18, "packet 0x91 raises +18");
static_assert(offsetof(D2NpcMenuEntryStrc, bAborted) == 21, "BeginInteraction reads 22*i + 21");
static_assert(sizeof(D2NpcMenuDescStrc) == 39, "the 39-byte stride; 48 x 39 ends at gnNpcMenuDescriptorCount");
static_assert(sizeof(D2NpcHirelingOfferStrc) == 16, "the 16-byte row; 10 x 16 ends at gNpcUnitGUID");
static_assert(offsetof(D2NpcHirelingOfferStrc, bUsed) == 8, "the free-slot scan starts at +8");
static_assert(offsetof(D2DataTablesStrc, nMonStatsTxtRecordCount) == 0xA80, "cmp ecx, [eax+0A80h]");
static_assert(offsetof(D2UnitStrc, dwUnitId) == 0x0C, "SetInteractingNpc reads pNpc[3]");
static_assert(offsetof(D2PlayerDataStrc, unk0xA8) == 0xA8, "the cleared group is [42..45] = +0x150..+0x15C");
//  The quest unlocks store into record 11 at 429 + 4 (nRows), 429 + 10 (wTblIdx) and 429 + 22 (pfnHandler); 429 = 11 * 39.
static_assert(offsetof(D2NpcMenuDescStrc, nRows) == 4, "nRows+1ADh = 429 + 4");
static_assert(offsetof(D2NpcMenuDescStrc, wTblIdx) == 8, "wTblIdx+1AFh = 429 + 8 + 2*1");
static_assert(offsetof(D2NpcMenuDescStrc, pfnHandler) == 18, "pfnHandler+1B1h = 429 + 18 + 4*1");
static_assert(UI_NPCMENU == 8, "OnItemServiceClicked: mov ecx, 8 ahead of UI_SetUIToggle");
static_assert(UI_TURNOFF == 1, "OnItemServiceClicked: mov edx, 1 ahead of UI_SetUIToggle");
//  The travel handlers push 1, 28h and 4Bh as the destination level, and the teardown ORs 2 into the NPC's dwFlags.
static_assert(LEVEL_ROGUEENCAMPMENT == 1 && LEVEL_LUTGHOLEIN == 40
              && LEVEL_KURASTDOCKTOWN == 75,
              "the travel handlers: push 1 / push 28h / push 4Bh");
static_assert(UNITFLAG_TARGETABLE == 2, "the teardown: or al, 2 on dwFlags (+0xC4)");
//  AbortOnPlayerAction's cursor starts at gNpcMenuEntries+11h (bRerollLine) and takes lea ecx, [esi-11h] as the record.
static_assert(offsetof(D2NpcMenuEntryStrc, bRerollLine) == 0x11,
              "AbortOnPlayerAction: mov esi, offset gNpcMenuEntries+11h");
//  TickAndValidate tests the NPC's class against the literal 202h.
static_assert(MONSTER_NIHLATHAK == 514,
              "TickAndValidate: cmp dword ptr [eax+4], 202h");

//1.10f: D2Client.0x6FAFC130
//  Spelled __fastcall; the accessor takes no argument, so the convention makes no difference to the bytes.
BOOL __fastcall NPCMENU_IsVendorGamble(void)
{
    return sgUIState.bNpcVendorGamble;
}

//1.10f: D2Client.0x6FAF3950
int32_t __cdecl NPCMENU_NpcGetVendorClassId(void)
{
    return sgUIState.NpcClassId;
}

//1.10f: D2Client.0x6FAFBD20
D2BitBufferStrc *__cdecl NPCMENU_QuestGetQuestRecord(void)
{
    return sgUIState.pNpcQuestRecord;
}

//1.10f: D2Client.0x6FAFBD30
D2BitBufferStrc *__cdecl NPCMENU_QuestGetQuestRecordSecondary(void)
{
    return sgUIState.pQuestRecordSecondary;
}

//1.10f: D2Client.0x6FAFB6D0
//  True while sgUIState.CurrentState is not 0, the idle value every teardown path writes back.
BOOL __cdecl NPCMENU_IsUiStateActive(void)
{
    return sgUIState.CurrentState != 0;
}

//1.10f: D2Client.0x6FAF3930
D2UnitStrc *__cdecl NPCMENU_NPC_GetActiveNpcUnit(void)
{
    if (sgUIState.bNpcInteractionActive)
    {
        return CUNIT_FindUnitByGuidAndType((uint32_t)sgUIState.NpcUnitGUID, UNIT_MONSTER);
    }
    else
    {
        return NULL;
    }
}

//1.10f: D2Client.0x6FAFBB50
//  The "am I talking to an NPC, and which one" pair: the GUID goes out through the pointer and the flag comes back as the result.
int32_t __fastcall NPCMENU_GetInteractingNpc(D2UnitGUID *pNpcGUID)
{
    *pNpcGUID = (D2UnitGUID)sgUIState.NpcUnitGUID;
    return sgUIState.bNpcInteractionActive;
}

//1.10f: D2Client.0x6FAFC0B0
//  Finds the interacting NPC unit and discards it; what comes back is a flag, not the unit, whatever the name suggests. What its
//  CurrentState == 1 test means is not established. It is the guard that keeps the new-stat and new-skill buttons (NEWSTATS_OnLButtonDown,
//  NEWSTATS_NewSkillsOnLButtonDown) from acting while a conversation is open.
BOOL __cdecl NPCMENU_GetInteractingNpcUnit(void)
{
    return NPCMENU_NPC_GetActiveNpcUnit() != NULL && sgUIState.CurrentState == 1;
}

//1.10f: D2Client.0x6FAFC0E0
//  True when any NPC screen is up; the third right-hand hint is suppressed while a conversation window is open.
BOOL __cdecl NPCMENU_IsAnyDialogOpen(void)
{
    return sgUIState.hDialogConfirm != NULL || sgUIState.hDialogResults != NULL || sgUIState.hDialogWaiting != NULL || sgUIState.hDialogMainMenu != NULL || sgUIState.hDialogTalkMenu != NULL || sgUIState.hDialogHireList != NULL
        || sgUIState.hDialogInterrupt != NULL;
}

//1.10f: D2Client.0x6FAF3DC0
//  Records the NPC's unit id and class id and sets the active flag. The parameter is spelled int32_t *; indices 1 and 3 are D2UnitStrc's
//  dwClassId and dwUnitId.
//  BUG: passing NULL clears the flag only and leaves gNpcUnitGUID and gNpcClassId holding the previous NPC. LATENT: every reader tests the
//  flag before the GUID, so the stale GUID is never read.
void __fastcall NPCMENU_SetInteractingNpc(int32_t *pNpc)
{
    if (pNpc)
    {
        sgUIState.bNpcInteractionActive = 1;
        sgUIState.NpcUnitGUID = (D2UnitGUID)pNpc[3]; //  +0x0C  dwUnitId
        sgUIState.NpcClassId = pNpc[1];             //  +0x04  dwClassId
    }
    else
    {
        sgUIState.bNpcInteractionActive = 0;
    }
}

//1.10f: D2Client.0x6FAFA230
//  NPCMENU_SetUnitFlag sets or clears nMask in pUnit->dwFlags (+0xC4); NPCMENU_MonsterSetDataFlag does the same to the WORD at pMonsterData
//  +0x28. Both are __fastcall with the third argument (bSet) on the stack (retn 4) and do nothing for a null unit, which is why no caller checks.
//  D2MonsterDataStrc declares +0x28 as a pointer (D2AiControlStrc *), but the client uses it as a 16-bit flag word: the mask is always a
//  single bit, and a client-side monster carries no AI control block. The store is therefore written as a 16-bit access through a cast.
void __fastcall NPCMENU_SetUnitFlag(D2UnitStrc *pUnit, uint32_t nMask, int32_t bSet)
{
    if (pUnit)
    {
        if (bSet)
        {
            pUnit->dwFlags |= nMask;
        }
        else
        {
            pUnit->dwFlags &= ~nMask;
        }
    }
}

//1.10f: D2Client.0x6FAFB6E0
void __fastcall NPCMENU_MonsterSetDataFlag(D2UnitStrc *pUnit, int16_t wFlagMask, int32_t bSet)
{
    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData)
    {
        //  a word: or [eax+28h], dx / and [eax+28h], dx
        uint16_t *pwFlags = (uint16_t *)((char *)pUnit->pMonsterData + 0x28);
        if (bSet)
        {
            *pwFlags |= (uint16_t)wFlagMask;
        }
        else
        {
            *pwFlags &= (uint16_t)~wFlagMask;
        }
    }
}

//1.10f: D2Client.0x6FAF8940
//  gwNpcSpeechesPlayed is a ring of ten words, and the guard is 9: the three appenders (NPCMENU_BeginInteraction, NPCMENU_ChooseNextSpeech,
//  NPCMENU_OnInterruptNewsClicked) test cmp ax, 9 / jnb before writing, so only indices 0..8 are written and the tenth slot is unused.
//  The count is a uint16_t (every read is a zero-extended mov ax).
//
//  Returns TRUE when the line has NOT been said yet; both callers loop until it answers TRUE, and an empty ring answers TRUE for everything.
//  BUG: the count is read as a word and compared signed (test eax, eax / jle and cmp edx, eax / jl), so a count above 0x7FFF would end the
//  loop at once. UNREACHABLE: the guard keeps the count at nine.
bool __fastcall NPCMENU_SpeechNotYetPlayed(int16_t nTblStringId)
{
    int32_t i = 0;

    if (!sgUIState.nNpcSpeechesPlayed)
    {
        return true;
    }
    while (sgUIState.wNpcSpeechesPlayed[i] != nTblStringId)
    {
        if (++i >= (int32_t)(uint16_t)sgUIState.nNpcSpeechesPlayed)
        {
            return true;
        }
    }
    return false;
}

//1.10f: D2Client.0x6FAF8970
//  True for the thirteen NPCs that talk and never open the main menu: story and quest fixtures (caged Cain, the frozen Anya, the palace guards,
//  the Ancients' statues), none of them a vendor. NPCMENU_BeginInteraction consults it after the monstats interact flag and calls
//  NPCMENU_OpenMainMenu only when it answers false. The ids are monstats.txt hcIdx values, not row numbers; 527 is Anya while still frozen.
bool __fastcall NPCMENU_IsSpeechOnlyNpc(D2C_MonsterIds nMonsterClassId)
{
    return nMonsterClassId == MONSTER_CAIN1
        || nMonsterClassId == MONSTER_DREHYAICED
        || nMonsterClassId == MONSTER_ANCIENTSTATUE1
        || nMonsterClassId == MONSTER_ANCIENTSTATUE2
        || nMonsterClassId == MONSTER_ANCIENTSTATUE3
        || nMonsterClassId == MONSTER_TYRAEL3
        || nMonsterClassId == MONSTER_NAVI
        || nMonsterClassId == MONSTER_ACT2GUARD5
        || nMonsterClassId == MONSTER_TYRAEL1
        || nMonsterClassId == MONSTER_ACT2GUARD4
        || nMonsterClassId == MONSTER_ACT2GUARD2
        || nMonsterClassId == MONSTER_IZUALGHOST
        || nMonsterClassId == MONSTER_MALACHAI;
}

//1.10f: D2Client.0x6FAF8E40
//  Returns the index of the first entry holding gNpcClassId. The loop is the original's: a separate found flag is tested at the top of the
//  body, so the index stops advancing on the match and the value returned is the counter.
//  BUG: a miss falls through to return 0, the same as "found at index 0". The sole caller, NPCMENU_BeginInteraction, multiplies the answer by 22
//  and reads gNpcMenuEntries[i].bAborted at +21, so on a miss it reads entry 0's flag byte instead of failing. REACHABLE: act2guard4 (377) and
//  act2guard5 (378) are interactable classes that none of the 46 records holds; BeginInteraction then opens its interrupt dialog, where the 0
//  reads and clears Akara's flag, a wrong "conversation already aborted" answer, not a fault.
int32_t __cdecl NPCMENU_FindEntryIndexForNpc(void)
{
    int32_t nIndex;
    int32_t bFound;
    D2NpcMenuEntryStrc *pEntry;

    nIndex = 0;
    bFound = 0;
    if (gnNpcMenuEntryCount <= 0)
    {
        return 0;
    }
    pEntry = gNpcMenuEntries;
    do
    {
        if (bFound)
        {
            break;
        }
        if (pEntry->nMonsterClassId == (uint32_t)sgUIState.NpcClassId)
        {
            bFound = 1;
        }
        else
        {
            ++nIndex;
            ++pEntry;
        }
    }
    while (nIndex < gnNpcMenuEntryCount);
    if (bFound != 1)
    {
        return 0;
    }
    return nIndex;
}

//1.10f: D2Client.0x6FAF4A40
//  Re-rolls the greeting line of every entry in one pass, then sets the flag, which is why the flag is global rather than per entry: the lines
//  for every NPC in the act are chosen at the same moment. The sole caller is NPCMENU_PlayClassGreeting. void: the original leaves the loop
//  bound in eax and no caller reads it.
void __cdecl NPCMENU_RerollAllSpeechLines(void)
{
    int32_t i = 0;

    if (gnNpcMenuEntryCount > 0)
    {
        D2NpcMenuEntryStrc *pEntry = gNpcMenuEntries;
        do
        {
            NPCMENU_PickRandomSpeechLine(pEntry);
            ++i;
            ++pEntry;
        }
        while (i < gnNpcMenuEntryCount);
    }
    sgUIState.bNpcSpeechLinesRolled = 1;
}

//1.10f: D2Client.0x6FAF9010
//  The four row setters re-prime a descriptor record at the start of every conversation; their bodies are identical. Each is void: the
//  original leaves the record's byte offset (nRecord * 39, the struct's size) in eax and no caller reads it.
void __fastcall NPCMENU_SetRowCount2(int32_t nRecord)
{
    gNpcMenuDescriptors[nRecord].nRows = 2;
    gNpcMenuDescriptors[nRecord].wTblIdx[2] = 0;
    gNpcMenuDescriptors[nRecord].pfnHandler[2] = NULL;
}

//1.10f: D2Client.0x6FAF9040
void __fastcall NPCMENU_SetRowCount3(int32_t nRecord)
{
    gNpcMenuDescriptors[nRecord].nRows = 3;
    gNpcMenuDescriptors[nRecord].wTblIdx[3] = 0;
    gNpcMenuDescriptors[nRecord].pfnHandler[3] = NULL;
}

//1.10f: D2Client.0x6FAF9070
void __fastcall NPCMENU_SetRowCount4(int32_t nRecord)
{
    gNpcMenuDescriptors[nRecord].nRows = 4;
    gNpcMenuDescriptors[nRecord].wTblIdx[4] = 0;
    gNpcMenuDescriptors[nRecord].pfnHandler[4] = NULL;
}

//1.10f: D2Client.0x6FAF90A0
//  Gives a record a live Identify Items row: tbl 4020 NPCIdentify1, handler NPCMENU_OnIdentifyClicked, three rows. NPCMENU_BeginInteraction
//  calls it on records 16, 17, 18, 19 and 38 (the five Cains). The row comes off again in NPCMENU_ShowTransactionResult once there is
//  nothing left to identify, and NPCMENU_BuildMainMenu itself sets it to 2 or 3 (from ITEMS_GetNoOfUnidItems) on the record it builds.
//  NPCMENU_BuildMainMenu finds the identify row by its tbl id, 4020; NPCMENU_ToggleResurrectMercRow finds the hire and resurrect rows by
//  their handler address.
void __fastcall NPCMENU_EnableIdentifyRow(int32_t nRecord)
{
    gNpcMenuDescriptors[nRecord].wTblIdx[1] = STR_IDX_4020_NPCIdentify1;
    gNpcMenuDescriptors[nRecord].pfnHandler[1] = (void *)NPCMENU_OnIdentifyClicked;
    gNpcMenuDescriptors[nRecord].nRows = 3;
}

//1.10f: D2Client.0x6FAF63E0
//  The quest unlocks: eight functions that each give one NPC one extra main-menu row once a quest bit is set. gNpcMenuQuestUnlocks is eight
//  20-byte records, walked by NPCMENU_OpenMainMenu:
//      +0x00  nMonsterClassId  compared with the NPC's class
//      +0x04  nQuestStateFlag  } QUESTRECORD_GetQuestState(gpNpcQuestRecord, nQuest, nState)
//      +0x08  nQuestState      }
//      +0x0C  bWantState       which answer fires the row (1 in all eight, so every row appears when its quest bit is SET)
//      +0x10  pfnAddRow        called through the table
//  The records, read out of the stock D2Client.dll:
//      class           quest                   bit                    unlock
//      155 warriv1      6  QUESTSTATEFLAG_A1Q6   0 QFLAG_REWARDGRANTED  WarrivGoEast
//      210 meshif1     14  QUESTSTATEFLAG_A2Q6   0 QFLAG_REWARDGRANTED  MeshifSailEast
//      150 kashya       2  QUESTSTATEFLAG_A1Q2   0 QFLAG_REWARDGRANTED  KashyaHire
//      154 charsi       3  QUESTSTATEFLAG_A1Q3   1 QFLAG_REWARDPENDING  CharsiImbue
//      511 larzuk      35  QUESTSTATEFLAG_A5Q1   1 QFLAG_REWARDPENDING  LarzukSockets
//      512 drehya      38  QUESTSTATEFLAG_A5Q4   1 QFLAG_REWARDPENDING  AnyaPersonalize
//      515 qual-kehk   36  QUESTSTATEFLAG_A5Q2   0 QFLAG_REWARDGRANTED  QualKehkHire
//      367 tyrael2     28  QUESTSTATEFLAG_A4COMPLETED 0 REWARDGRANTED   TyraelTravel
//  The class ids are monstats hcIdx values. Tyrael's row waits for the act, not for a quest in it: 28 is QUESTSTATEFLAG_A4COMPLETED, while
//  the Diablo quest (A4Q2) is 26.
//
//  Each unlock makes three stores into gNpcMenuDescriptors, always in the order nRows, handler, tbl id:
//      unlock           record nRows row  tbl id                    handler
//      KashyaHire          11    3   [1]  3397  NPCMenuHire          DrawHireMercenaryList
//      WarrivGoEast        13    3   [1]  3382  WarrivMenu1b         OnGoEastClicked
//      MeshifSailEast      15    3   [1]  3384  MeshifMenuEast       OnSailEastClicked
//      CharsiImbue          3    4   [2]  4017  Upgrade              OnItemServiceClicked
//      LarzukSockets       39    4   [2]  22748 Addsocketsui         OnItemServiceClicked
//      AnyaPersonalize     41    5   [3]  22749 Personalizeui        OnItemServiceClicked
//      QualKehkHire        43    3   [1]  3397  NPCMenuHire          DrawHireMercenaryList
//      TyraelTravel        21    3   [1]  22738 Travel_To_Harrogath  OnTravelToHarrogathClicked
//
//  Each returns int 1 (mov eax, 1); the one dispatcher, NPCMENU_OpenMainMenu, discards it.
int32_t __cdecl NPCMENU_QuestUnlock_KashyaHire(void)
{
    gNpcMenuDescriptors[11].nRows = 3;
    gNpcMenuDescriptors[11].pfnHandler[1] = (void *)NPCMENU_DrawHireMercenaryList;
    gNpcMenuDescriptors[11].wTblIdx[1] = STR_IDX_3397_NPCMenuHire;
    return 1;
}

//1.10f: D2Client.0x6FAF6410
int32_t __cdecl NPCMENU_QuestUnlock_WarrivGoEast(void)
{
    gNpcMenuDescriptors[13].nRows = 3;
    gNpcMenuDescriptors[13].pfnHandler[1] = (void *)NPCMENU_OnGoEastClicked;
    gNpcMenuDescriptors[13].wTblIdx[1] = STR_IDX_3382_WarrivMenu1b;
    return 1;
}

//1.10f: D2Client.0x6FAF6440
int32_t __cdecl NPCMENU_QuestUnlock_MeshifSailEast(void)
{
    gNpcMenuDescriptors[15].nRows = 3;
    gNpcMenuDescriptors[15].pfnHandler[1] = (void *)NPCMENU_OnSailEastClicked;
    gNpcMenuDescriptors[15].wTblIdx[1] = STR_IDX_3384_MeshifMenuEast;
    return 1;
}

//1.10f: D2Client.0x6FAF6470
int32_t __cdecl NPCMENU_QuestUnlock_CharsiImbue(void)
{
    gNpcMenuDescriptors[3].nRows = 4;
    gNpcMenuDescriptors[3].pfnHandler[2] = (void *)NPCMENU_OnItemServiceClicked;
    gNpcMenuDescriptors[3].wTblIdx[2] = STR_IDX_4017_Upgrade;
    return 1;
}

//1.10f: D2Client.0x6FAF64A0
//  The handler behind Charsi's Upgrade, Larzuk's Add sockets and Anya's Personalize: it closes the menu and opens the anvil panel in mode 1 (the
//  locally entered one) for the item to be handed over; the panel's confirm button is what sends it, through NPCMENU_SendQuestItemAction.
//  It is the only writer of UI state 7.
//  The return 1 decides what the dialog does next: DIALOG_OnEnter arms the panel-toggle delay on exactly 1 (already armed here, so on Enter
//  it is set twice), and DIALOG_OnMouseUp on 0 clears pDialog->bLinePressActive through a pointer fetched before the call. This handler has
//  just freed that dialog (DIALOG_Close frees the head of the list), so an answer of 0 would store into freed memory; 1 returns without
//  touching it.
//  USEITEM_AnvilOpenPanelLocal puts its argument into a global nothing reads (gnAnvilNpcGUID), so the NPC's GUID passed here goes nowhere.
int32_t __cdecl NPCMENU_OnItemServiceClicked(void)
{
    if (sgUIState.hDialogMainMenu)
    {
        DIALOG_Close(sgUIState.hDialogMainMenu);
        sgUIState.hDialogMainMenu = NULL;
    }
    UI_SetUIToggle(UI_NPCMENU, UI_TURNOFF, 0);
    UI_ArmPanelToggleDelay();
    INV_ResetItemDescState();
    sgUIState.CurrentState = 7;
    sgUIState.Transaction = 0;
    USEITEM_AnvilOpenPanelLocal(sgUIState.NpcUnitGUID);
    return 1;
}

//1.10f: D2Client.0x6FAF6500
int32_t __cdecl NPCMENU_QuestUnlock_LarzukSockets(void)
{
    gNpcMenuDescriptors[39].nRows = 4;
    gNpcMenuDescriptors[39].pfnHandler[2] = (void *)NPCMENU_OnItemServiceClicked;
    gNpcMenuDescriptors[39].wTblIdx[2] = STR_IDX_22748_Addsocketsui;
    return 1;
}

//1.10f: D2Client.0x6FAF6530
int32_t __cdecl NPCMENU_QuestUnlock_AnyaPersonalize(void)
{
    gNpcMenuDescriptors[41].nRows = 5;
    gNpcMenuDescriptors[41].pfnHandler[3] = (void *)NPCMENU_OnItemServiceClicked;
    gNpcMenuDescriptors[41].wTblIdx[3] = STR_IDX_22749_Personalizeui;
    return 1;
}

//1.10f: D2Client.0x6FAF6560
int32_t __cdecl NPCMENU_QuestUnlock_QualKehkHire(void)
{
    gNpcMenuDescriptors[43].nRows = 3;
    gNpcMenuDescriptors[43].pfnHandler[1] = (void *)NPCMENU_DrawHireMercenaryList;
    gNpcMenuDescriptors[43].wTblIdx[1] = STR_IDX_3397_NPCMenuHire;
    return 1;
}

//1.10f: D2Client.0x6FAF6590
//  The one unlock with a condition of its own: the three stores sit behind GAME_IsExpansionGame(), so a classic game that completes act 4
//  gets no Harrogath row. mov eax, 1 is loaded above the jz, so both paths return 1.
int32_t __cdecl NPCMENU_QuestUnlock_TyraelTravel(void)
{
    if (GAME_IsExpansionGame())
    {
        gNpcMenuDescriptors[21].nRows = 3;
        gNpcMenuDescriptors[21].pfnHandler[1] = (void *)NPCMENU_OnTravelToHarrogathClicked;
        gNpcMenuDescriptors[21].wTblIdx[1] = STR_IDX_22738_Travel_To_Harrogath;
    }
    return 1;
}

//1.10f: D2Client.0x6FAF70A0
//  The other end of the item service: in mode 1 the anvil panel's confirm button hands this the item in its slot, and this sends client packet
//  0x38 (13 bytes) with nAction 0, the NPC's GUID and the item's GUID, after clearing gbWaitingForItemMsg. The server takes nAction 0 to its
//  per-class switch, where Charsi's arm checks the player's cursor item against the GUID sent here.
//  void: the original leaves the eax of MSG_SendCmdUnitParam, which its one caller, USEITEM_AnvilOnPanelClick, does not read.
void __fastcall NPCMENU_SendQuestItemAction(D2UnitGUID dwItemGUID)
{
    sgUIState.bWaitingForItemMsg = 0;
    MSG_SendCmdUnitParam(0x38, 0, (int32_t)sgUIState.NpcUnitGUID, (int32_t)dwItemGUID);
}

//1.10f: D2Client.0x6FAFBCC0
//  The hireling offer list: ten 16-byte rows, filled one server packet (0x4E) at a time; the run ends with a header-only 0x4F.
//  DIFFERENCE: the original ends each walk (AddHirelingOffer, ClearHirelingOffers, RefreshHireIconsForOffer) at the address of the next
//  global; the offers are a member of our struct, so each walk of ours counts the row it is on and stops at 10. The original's compares are signed.
//  BUG: a full list silently drops the new offer: AddHirelingOffer scans for a free row and returns when there is none, with no write and no
//  report. LATENT: the server sends one packet per candidate, and whether an eleventh can arrive is decided by D2Game, not by this module.
//  Reproduced.
void __fastcall NPCMENU_AddHirelingOffer(D2GSPacketSrv4E *pPacket)
{
    int32_t nRow;

    if (pPacket)
    {
        nRow = 0;
        while (sgUIState.NpcHirelingOffers[nRow].bUsed)
        {
            ++nRow;
            if (nRow >= 10)
            {
                return;                     //  the list is full; nothing written
            }
        }
        sgUIState.NpcHirelingOffers[nRow].wMercName = pPacket->nMercName;
        sgUIState.NpcHirelingOffers[nRow].nSeed = pPacket->nSeed;
        sgUIState.NpcHirelingOffers[nRow].bUsed = 1;
    }
}

//1.10f: D2Client.0x6FAFBD00
//  Empties the list by writing 0 to each row's bUsed field only, leaving wMercName and nSeed as they were. Every caller is an
//  interaction-teardown path.
void __cdecl NPCMENU_ClearHirelingOffers(void)
{
    int32_t nRow = 0;

    do
    {
        sgUIState.NpcHirelingOffers[nRow].bUsed = 0;
        ++nRow;
    }
    while (nRow < 10);
}

//1.10f: D2Client.0x6FAFBE30
//  Searches the offer list for wMercName, then calls HIREABLES_HireIconsResetAndShow.
//  BUG: the search is dead: it breaks on the match, but nothing reads the cursor afterwards, and HIREABLES_HireIconsResetAndShow takes no argument.
//  INERT: a match and a miss do exactly the same thing. The loop is kept as the original has it.
void __fastcall NPCMENU_RefreshHireIconsForOffer(int16_t wMercName)
{
    int32_t nRow = 0;

    do
    {
        if (sgUIState.NpcHirelingOffers[nRow].wMercName == wMercName)
        {
            break;
        }
        ++nRow;
    }
    while (nRow < 10);
    HIREABLES_HireIconsResetAndShow();
}

//1.10f: D2Client.0x6FAFBE50
//  A CUNIT_ForEachObjectUnit callback: raises a flag when an object is class 318 eunuch (objects.txt) in anim mode 2, one of the murdered harem
//  eunuchs in Lut Gholein's palace in its already-revealed pose. NPCMENU_ShowNpcAlert clears the flag, passes this in, and then reads the flag
//  as "has the player seen the murder scene".
void __fastcall NPCMENU_NoteOpenedEunuchObject(D2UnitStrc *pObject)
{
    if (pObject && pObject->dwClassId == 318 && pObject->dwAnimMode == 2)
    {
        gbFoundOpenedEunuch = 1;
    }
}

//1.10f: D2Client.0x6FAFC190
//  Records the resurrect offer's name and price, and for one tbl id turns five menu rows off.
//  The "no name" sentinel is 0xFFFF (cmp cx, 0FFFFh), the one the speech code tests throughout this unit.
//  The name store is sixteen bits into a four-byte slot (mov word ptr gwMercResurrectNameTblId, cx), so it is written as a partial store and
//  not as an enum assignment, which MSVC would widen to 32 bits.
//  DIFFERENCE: the original's fifth call is a tail jump that shares the epilogue; ours makes five ordinary calls, identical in effect.
void __fastcall NPCMENU_SetResurrectMercOffer(D2C_StringIndices wNameTblId, int32_t nCost)
{
    gwMercResurrectNameTblId = wNameTblId;
    gnMercResurrectCost = nCost;
    if ((uint16_t)wNameTblId == 0xFFFF)
    {
        NPCMENU_ToggleResurrectMercRow(11, 0);
        NPCMENU_ToggleResurrectMercRow(8, 0);
        NPCMENU_ToggleResurrectMercRow(24, 0);
        NPCMENU_ToggleResurrectMercRow(21, 0);
        NPCMENU_ToggleResurrectMercRow(43, 0);
    }
}

//1.10f: D2Client.0x6FAFC140
//  Packet 0x91 is a bulk "these NPCs are available" message: twelve 16-bit monster ids starting at +2. For each id that is a valid monstats
//  index, every entry of gNpcMenuEntries whose class id matches gets its byte at +0x12 (unk0x12) raised, so the flag is per menu entry, not per
//  NPC, and one id can light several entries.
//  The bounds test reads nMonStatsTxtRecordCount through three loads, because sgptDataTables is a slot holding a pointer that D2Common exports.
//  void: the original leaves the cursor in eax and the sole caller does not read it.
void __fastcall NPCMENU_MarkEntriesFromPacket91(uint16_t *pPacket)
{
    int32_t nWordsLeft = 12;
    uint16_t *pClassIds = pPacket + 1;      //  the ids start at +2

    do
    {
        if ((int32_t)*pClassIds < sgptDataTables->nMonStatsTxtRecordCount && gnNpcMenuEntryCount > 0)
        {
            D2NpcMenuEntryStrc *pEntry = gNpcMenuEntries;
            int32_t nEntriesLeft = gnNpcMenuEntryCount;
            do
            {
                if (pEntry->nMonsterClassId == *pClassIds)
                {
                    pEntry->unk0x12 = 1;
                }
                ++pEntry;
                --nEntriesLeft;
            }
            while (nEntriesLeft);
        }
        ++pClassIds;
        --nWordsLeft;
    }
    while (nWordsLeft);
}

//1.10f: D2Client.0x6FAFC1F0
//  Closes the hire-list dialog, destroys its control through the control's own vtable slot 0 with argument 1 (the vector deleting destructor),
//  nulls both handles and rebuilds the main menu.
//  The outer if is tested twice, at entry and again after DIALOG_Close, because DIALOG_Close can reach code that clears the handle; both tests
//  are in the bytes and both are written.
void __cdecl NPCMENU_TeardownHireList(void)
{
    if (sgUIState.pHireListControl)
    {
        if (sgUIState.hDialogHireList)
        {
            DIALOG_Close(sgUIState.hDialogHireList);
            if (sgUIState.pHireListControl)
            {
                (*(void(__thiscall **)(void *, int32_t)) *(uint32_t *)sgUIState.pHireListControl)(sgUIState.pHireListControl, 1);
            }
            sgUIState.pHireListControl = 0;
            sgUIState.hDialogHireList = NULL;
        }
        NPCMENU_BuildMainMenu();
    }
}

//1.10f: D2Client.0x6FAF7FE0
//  Copies 0x60 bytes of the packet into the second quest bit buffer. What that record is remains open; every read of it queries quest state
//  bit 13, while the primary record is queried for bits 0, 1 and 14.
void __fastcall NPCMENU_QuestCopySecondaryQuestRecord(uint8_t *pBuffer)
{
    D2COMMON_QUESTRECORD_CopyBufferToRecord(sgUIState.pQuestRecordSecondary, pBuffer, 0x60, 0);
}

//1.10f: D2Client.0x6FAF9E50
//  A one-instruction thunk (jmp INV_NpcShopRequestPageRebuild) and the only path by which ITEM_ItemMsg0B0C_UpdateNpcStore reaches that setter.
//  __fastcall with one register argument that the target does not read; void; nothing is purged.
void __fastcall j_INV_NpcShopRequestPageRebuild(D2UnitStrc *pNpc)
{
    INV_NpcShopRequestPageRebuild();
}

//1.10f: D2Client.0x6FAF78E0
//  The teardown: fourteen functions end a conversation with one block of code, which is NPCMENU_NpcCloseInteraction. It exists in the original as
//  three byte-identical out-of-line copies (NPCMENU_NpcCloseInteraction, _Dup1 and _Dup2), written here as three functions because their
//  callers reach different copies; none calls another. Which copy the source named, if it named one at all, is not in the bytes.
//  NPCMENU_NpcEndInteraction's middle is NPCMENU_NpcResyncVendorInventory's body, and that one's middle is NPCMENU_NpcFreeVendorInventory's.
//
//  NPCMENU_NpcFreeVendorInventory clears the ten hireling offers first, before the null test, so a NULL NPC still empties the offer list; then it
//  walks the NPC's inventory (the next item is fetched before the current one is touched, and a node's client unit is removed only when
//  INVENTORY_UnitIsItem says it is an item), and INVENTORY_RemoveInventoryItems empties the inventory. The original inlines
//  NPCMENU_ClearHirelingOffers here.
//  BUG: the last call, D2Common #10251, is a bare retn 4 (0x6FDA5250 in the stock D2Common); it is called anyway, with the inventory.
//  INERT: it does nothing, and nothing here frees the inventory object. Reproduced.
void __fastcall NPCMENU_NpcFreeVendorInventory(D2UnitStrc *pNpc)
{
    D2InventoryStrc *pInventory;
    D2UnitStrc *pItem;
    D2UnitStrc *pNextItem;
    D2UnitStrc *pItemUnit;

    NPCMENU_ClearHirelingOffers();
    if (pNpc)
    {
        pInventory = pNpc->pInventory;
        if (pInventory)
        {
            for (pItem = D2COMMON_INVENTORY_GetFirstItem(pInventory); pItem; pItem = pNextItem)
            {
                pNextItem = D2COMMON_INVENTORY_GetNextItem(pItem);
                pItemUnit = D2COMMON_INVENTORY_UnitIsItem(pItem);
                if (pItemUnit)
                {
                    CUNIT_RemoveUnitById((int32_t)pItemUnit->dwUnitId, pItemUnit->dwUnitType);
                }
            }
            D2COMMON_INVENTORY_RemoveInventoryItems(pInventory);
            D2COMMON_10251_Return((int32_t)pInventory);     //  a bare retn 4
        }
    }
}

//1.10f: D2Client.0x6FAF7950
//  The vendor-inventory drop reached by GUID, and the greeting sound stopped with it. The GUID check is FOG_Trace, not an assert: a mismatch is
//  logged and the function carries on with the GUID it was handed.
void __fastcall NPCMENU_NpcResyncVendorInventory(uint32_t dwNpcGUID)
{
    D2UnitStrc *pNpc;

    if (dwNpcGUID != (uint32_t)sgUIState.NpcUnitGUID)
    {
        FOG_Trace("mismatch npc ids\n");
    }
    pNpc = CUNIT_FindUnitByGuidAndType(dwNpcGUID, UNIT_MONSTER);
    if (pNpc)
    {
        NPCMENU_NpcFreeVendorInventory(pNpc);
    }
    if (sgUIState.bNpcGreetingIsOneShot)
    {
        SOUNDINST_MarkStarted(sgUIState.pNpcGreetingSound);
    }
    sgUIState.bNpcGreetingIsOneShot = 0;
    sgUIState.pNpcGreetingSound = 0;
}

//1.10f: D2Client.0x6FAFBB60
//  The data half of the teardown. In order: the player's four queued-command dwords cleared (PLAYER_ClearQueuedCommand; CUNIT_GetPlayer is asked
//  twice, once for the test and once for the argument); the vendor inventory dropped and the greeting stopped
//  (NPCMENU_NpcResyncVendorInventory); UNITFLAG_TARGETABLE raised on the NPC; client packet 0x30, which reads only the GUID at +5, so the 1 in
//  front of it is written and never read; the text messages cleared; the interaction flag dropped through NPCMENU_SetInteractingNpc(NULL), which
//  leaves the GUID stale; the NPC menu panel turned off; bit 0 of the NPC's monster-data word at +0x28 cleared; and the NPC back in anim mode 1.
//  The NPC is looked up once for the flag, and that pointer is reused across four calls (the 0x30 send, TEXT_ClearMessages,
//  NPCMENU_SetInteractingNpc and UI_SetUIToggle) before its data flag and anim mode are set. Whether any of the four can free the unit is not
//  established; the original does exactly this, and so does the C.
//  DIFFERENCE: the original's last statement is a tail jmp UNITMODE_SetAnimMode with the frame already torn down; ours is an ordinary call,
//  which computes the same.
void __fastcall NPCMENU_NpcEndInteraction(uint32_t dwNpcGUID)
{
    D2UnitStrc *pNpc;

    if (CUNIT_GetPlayer())
    {
        PLAYER_ClearQueuedCommand(CUNIT_GetPlayer());
    }
    NPCMENU_NpcResyncVendorInventory(dwNpcGUID);
    pNpc = CUNIT_FindUnitByGuidAndType(dwNpcGUID, UNIT_MONSTER);
    if (pNpc)
    {
        NPCMENU_SetUnitFlag(pNpc, UNITFLAG_TARGETABLE, TRUE);
    }
    MSG_SendCmdUnit(0x30, 1, (int32_t)dwNpcGUID);
    TEXT_ClearMessages();
    NPCMENU_SetInteractingNpc(NULL);
    UI_SetUIToggle(UI_NPCMENU, UI_TURNOFF, 0);
    if (pNpc)
    {
        NPCMENU_MonsterSetDataFlag(pNpc, 1, FALSE);
        UNITMODE_SetAnimMode(pNpc, 1);
    }
}

//1.10f: D2Client.0x6FAF3AD0
//  The whole teardown, in three copies: the UI half in front of NPCMENU_NpcEndInteraction and a second panel-off behind it. The second
//  UI_SetUIToggle finds the panel already off, and UI_SetUIToggle unregisters a panel's handlers only on a change of state, so it does not
//  unregister twice. Reproduced as a second call. void: the original's int is UI_SetUIToggle's eax, which no caller reads.
//  Callers: NPCMENU_NpcCloseInteraction -- the anvil's close paths; _Dup1 -- USEITEM_AnvilOnPanelPacket; _Dup2 -- NPCMENU_BeginInteraction
//  (twice) and NPCMENU_PlayClassGreeting.
void __cdecl NPCMENU_NpcCloseInteraction(void)
{
    sgUIState.CurrentState = 0;
    INV_ResetItemDescState();
    NPCMENU_NpcEndInteraction((uint32_t)sgUIState.NpcUnitGUID);
    UI_SetUIToggle(UI_NPCMENU, UI_TURNOFF, 0);
}

//1.10f: D2Client.0x6FAF3960
void __cdecl NPCMENU_NpcCloseInteraction_Dup1(void)
{
    sgUIState.CurrentState = 0;
    INV_ResetItemDescState();
    NPCMENU_NpcEndInteraction((uint32_t)sgUIState.NpcUnitGUID);
    UI_SetUIToggle(UI_NPCMENU, UI_TURNOFF, 0);
}

//1.10f: D2Client.0x6FAF7A60
void __cdecl NPCMENU_NpcCloseInteraction_Dup2(void)
{
    sgUIState.CurrentState = 0;
    INV_ResetItemDescState();
    NPCMENU_NpcEndInteraction((uint32_t)sgUIState.NpcUnitGUID);
    UI_SetUIToggle(UI_NPCMENU, UI_TURNOFF, 0);
}

//1.10f: D2Client.0x6FAF9E60
//  The shop panel off, then the whole teardown. It does not close the main-menu dialog the way NPCMENU_OnMainMenuCancel does.
void __cdecl NPCMENU_NpcShopCloseAndEndInteraction(void)
{
    UI_SetUIToggle(UI_NPCSHOP, UI_TURNOFF, 0);
    NPCMENU_NpcCloseInteraction();
}

//1.10f: D2Client.0x6FAF4110
//  The dialog callbacks. Each of the three dialogs is created with a dismiss callback (pfnCallback) and a click-away callback (pfnCallback2),
//  and each has a cancel row whose pfnOnClick is the dismiss callback again, so one function is reached three ways and the answer means
//  something on only two of them:
//      pfnCallback   DIALOG_OnCancelKey and DIALOG_OnEscape consume the key before calling it; DIALOG_OnMouseDown calls it only when a dialog
//                    has no pfnCallback2, and none of these three lacks one. Nothing reads the answer.
//      pfnCallback2  DIALOG_OnMouseDown: 0 returns without consuming; non-zero asks the NEWSTATS and NEWSKILLS buttons and consumes only over
//                    one of them.
//      pfnOnClick    DIALOG_OnMouseUp clears bLinePressActive through the dialog pointer it fetched before the call when the answer is 0;
//                    DIALOG_OnEnter arms the toggle delay only on exactly 1.
//  Every closing callback here answers 1 after DIALOG_Close has freed its dialog, so DIALOG_OnMouseUp never writes through the stale pointer.
//  That the answer decides it is established; that it is why they answer 1 is not.
//
//  OnTalkMenuCancel goes back instead of ending the conversation: it closes the talk menu, plays sound 1 and rebuilds the main menu, unless a
//  speech line is playing, when it does nothing and answers 0. As the dismiss callback that 0 is never read; as the talk menu's cancel row it
//  makes DIALOG_OnMouseUp clear bLinePressActive on the talk dialog, which is still alive on that path.
int32_t __cdecl NPCMENU_OnTalkMenuCancel(void)
{
    if (sgUIState.bNpcSpeechActive)
    {
        return 0;
    }
    if (sgUIState.hDialogTalkMenu)
    {
        DIALOG_Close(sgUIState.hDialogTalkMenu);
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
        sgUIState.hDialogTalkMenu = NULL;
        NPCMENU_BuildMainMenu();
    }
    return 1;
}

//1.10f: D2Client.0x6FAF4160
//  Clicking away from the talk menu ends the conversation, where escape only went back. While a speech line plays it answers 0, and
//  DIALOG_OnMouseDown then returns without consuming, so that click goes on down the handler chain. Where it lands is not established.
int32_t __cdecl NPCMENU_OnTalkMenuClickAway(void)
{
    if (sgUIState.bNpcSpeechActive)
    {
        return 0;
    }
    if (sgUIState.hDialogTalkMenu)
    {
        DIALOG_Close(sgUIState.hDialogTalkMenu);
        sgUIState.hDialogTalkMenu = NULL;
        NPCMENU_NpcCloseInteraction();
    }
    return 1;
}

//1.10f: D2Client.0x6FAF6BC0
//  The main menu's dismiss callback and cancel row; its twin below, OnMainMenuClickAway, is the click-away callback. Both close the menu and end
//  the conversation.
int32_t __cdecl NPCMENU_OnMainMenuCancel(void)
{
    if (sgUIState.hDialogMainMenu)
    {
        DIALOG_Close(sgUIState.hDialogMainMenu);
        sgUIState.hDialogMainMenu = NULL;
    }
    NPCMENU_NpcCloseInteraction();
    return 1;
}

//1.10f: D2Client.0x6FAF6D50
int32_t __cdecl NPCMENU_OnMainMenuClickAway(void)
{
    if (sgUIState.hDialogMainMenu)
    {
        DIALOG_Close(sgUIState.hDialogMainMenu);
        sgUIState.hDialogMainMenu = NULL;
    }
    NPCMENU_NpcCloseInteraction();
    return 1;
}

//1.10f: D2Client.0x6FAF93D0
//  The interrupt dialog's callbacks; the twin below is OnInterruptClickAway. Declining the news still acknowledges it: client packet 0x31 goes
//  out with the NPC's GUID and the speech's tbl id (nine bytes, the id zero-extended into a dword), but only when the dialog was still up.
//  The original schedules the store of NULL between the arguments and the call; nothing between them reads the handle, so it is written in
//  source order.
int32_t __cdecl NPCMENU_OnInterruptCancel(void)
{
    if (sgUIState.hDialogInterrupt)
    {
        DIALOG_Close(sgUIState.hDialogInterrupt);
        sgUIState.hDialogInterrupt = NULL;
        MSG_SendCmdUnit(0x31, (int32_t)sgUIState.NpcUnitGUID, sgUIState.wNpcSpeechTblId);
    }
    NPCMENU_NpcCloseInteraction();
    return 1;
}

//1.10f: D2Client.0x6FAF9570
int32_t __cdecl NPCMENU_OnInterruptClickAway(void)
{
    if (sgUIState.hDialogInterrupt)
    {
        DIALOG_Close(sgUIState.hDialogInterrupt);
        sgUIState.hDialogInterrupt = NULL;
        MSG_SendCmdUnit(0x31, (int32_t)sgUIState.NpcUnitGUID, sgUIState.wNpcSpeechTblId);
    }
    NPCMENU_NpcCloseInteraction();
    return 1;
}

//1.10f: D2Client.0x6FAF70C0
//  The five travel handlers close the main menu, run the whole teardown, then send client packet 0x38 with action 0, the NPC's GUID (re-read
//  from gNpcUnitGUID, not the teardown's saved copy) and a level id. They are five 407-byte functions that differ in one immediate byte:
//      OnGoEastClicked             warriv1    40  Lut Gholein
//      OnSailEastClicked           meshif1    75  Kurast Docks
//      OnTravelToHarrogathClicked  tyrael2   109  Harrogath
//      OnSailWestClicked           meshif2    40  Lut Gholein
//      OnTravelWestClicked         warriv2     1  Rogue Encampment
//  The level id is redundant: the server decides the destination from the NPC's monstats class (155 -> 40, 175 -> 1, 210 -> 75, 264 -> 40,
//  367 -> 109), and no travel arm reads the fourth field; only the item-service arms do, as the item GUID.
//  The client offers Tyrael's row once quest 28 (QUESTSTATEFLAG_A4COMPLETED) has its bit 0 set; the server's arm requires quest 26, the Diablo
//  quest, bit 0. Whether the two bits can ever disagree is not decided from inside D2Client.
int32_t __cdecl NPCMENU_OnGoEastClicked(void)
{
    if (sgUIState.hDialogMainMenu)
    {
        DIALOG_Close(sgUIState.hDialogMainMenu);
        sgUIState.hDialogMainMenu = NULL;
    }
    NPCMENU_NpcCloseInteraction();
    MSG_SendCmdUnitParam(0x38, 0, (int32_t)sgUIState.NpcUnitGUID, LEVEL_LUTGHOLEIN);
    return 1;
}

//1.10f: D2Client.0x6FAF7260
int32_t __cdecl NPCMENU_OnSailEastClicked(void)
{
    if (sgUIState.hDialogMainMenu)
    {
        DIALOG_Close(sgUIState.hDialogMainMenu);
        sgUIState.hDialogMainMenu = NULL;
    }
    NPCMENU_NpcCloseInteraction();
    MSG_SendCmdUnitParam(0x38, 0, (int32_t)sgUIState.NpcUnitGUID, LEVEL_KURASTDOCKTOWN);
    return 1;
}

//1.10f: D2Client.0x6FAF7400
int32_t __cdecl NPCMENU_OnTravelToHarrogathClicked(void)
{
    if (sgUIState.hDialogMainMenu)
    {
        DIALOG_Close(sgUIState.hDialogMainMenu);
        sgUIState.hDialogMainMenu = NULL;
    }
    NPCMENU_NpcCloseInteraction();
    MSG_SendCmdUnitParam(0x38, 0, (int32_t)sgUIState.NpcUnitGUID, LEVEL_HARROGATH);
    return 1;
}

//1.10f: D2Client.0x6FAF75A0
int32_t __cdecl NPCMENU_OnSailWestClicked(void)
{
    if (sgUIState.hDialogMainMenu)
    {
        DIALOG_Close(sgUIState.hDialogMainMenu);
        sgUIState.hDialogMainMenu = NULL;
    }
    NPCMENU_NpcCloseInteraction();
    MSG_SendCmdUnitParam(0x38, 0, (int32_t)sgUIState.NpcUnitGUID, LEVEL_LUTGHOLEIN);
    return 1;
}

//1.10f: D2Client.0x6FAF7740
int32_t __cdecl NPCMENU_OnTravelWestClicked(void)
{
    if (sgUIState.hDialogMainMenu)
    {
        DIALOG_Close(sgUIState.hDialogMainMenu);
        sgUIState.hDialogMainMenu = NULL;
    }
    NPCMENU_NpcCloseInteraction();
    MSG_SendCmdUnitParam(0x38, 0, (int32_t)sgUIState.NpcUnitGUID, LEVEL_ROGUEENCAMPMENT);
    return 1;
}

//1.10f: D2Client.0x6FAF5B80
//  The teardown's other forms: six functions carry the teardown with some helpers called, and they do not all carry the same function:
//      OnTradeClicked            NPCMENU_NpcCloseInteraction
//      OnInterruptNewsClicked    NPCMENU_NpcCloseInteraction
//      AbortOnPlayerAction       NPCMENU_NpcCloseInteraction, then gbNpcInteractionActive = 0 once more
//      OnGambleClicked           NPCMENU_NpcEndInteraction
//      OnResultContinueClicked   NPCMENU_NpcEndInteraction, then UI_NPCMENU off again
//      NPCMENU_EndInteraction    a trace, NPCMENU_NpcEndInteraction, then the NPC looked up and flagged once more
//  Each arm is written as calls; the differences between the arms are the original's.
//
//  OnTradeClicked and OnGambleClicked are the shop's two doors: trade and trade/repair on 16 NPCs, gamble on 6. With the NPC's unit present and
//  carrying an inventory they set state 3 and the gamble flag, turn the shop panel on, send client packet 0x38 with action 1 or 2 (the vendor and
//  gamble arms) and cache the player for NPCMENU_SendTransactionAndWait, which reads gpNpcTransactionPlayer->pInventory (mov eax, [eax+60h]).
//  A word global beside it is zeroed by both and read by nothing.
//  The two are not one body:
//    * only OnTradeClicked stops a one-shot greeting at entry, with an inner test on the sound handle that the teardown's own stop does not
//      have; the same seven instructions stand in NPCMENU_OpenMainMenu, and no out-of-line copy exists, so they are written out;
//    * the fallback arms differ: Trade's is NPCMENU_NpcCloseInteraction and Gamble's NPCMENU_NpcEndInteraction. Gamble's does not zero
//      sgUIState_CurrentState, does not call INV_ResetItemDescState, and turns UI_NPCMENU off once.
//  That difference runs through UI_UnregisterPanelHandlers: UI_NPCMENU going from on to off calls NPCMENU_EndInteractionByUiState, which
//  switches on sgUIState_CurrentState. Trade's arm has zeroed it by then, so that call only closes the dialogs unless a speech is active.
//  Gamble's has not: at state 1 that call runs its own full teardown, a second client packet 0x30 among it, and that is what finally zeroes
//  the state and resets the item description. Reproduced.
//  BUG: the Gamble fallback arm leaves the UI state alone, as described. NOT ESTABLISHED: the arm runs only when the NPC's unit is gone from
//  the client, or has no inventory, while its main menu is open, and whether that can happen is not decided.
//  The shop condition is the unit and its inventory, not the interaction flag, so it is not NPCMENU_NPC_GetActiveNpcUnit.
int32_t __cdecl NPCMENU_OnTradeClicked(void)
{
    D2UnitStrc *pNpc;

    if (sgUIState.bNpcGreetingIsOneShot)
    {
        if (sgUIState.pNpcGreetingSound)
        {
            SOUNDINST_MarkStarted(sgUIState.pNpcGreetingSound);
            sgUIState.pNpcGreetingSound = 0;
        }
        sgUIState.bNpcGreetingIsOneShot = 0;
    }
    if (sgUIState.hDialogMainMenu)
    {
        DIALOG_Close(sgUIState.hDialogMainMenu);
        sgUIState.hDialogMainMenu = NULL;
    }
    pNpc = CUNIT_FindUnitByGuidAndType((uint32_t)sgUIState.NpcUnitGUID, UNIT_MONSTER);
    if (pNpc && pNpc->pInventory)
    {
        sgUIState.CurrentState = 3;
        sgUIState.bNpcVendorGamble = FALSE;
        UI_SetUIToggle(UI_NPCSHOP, UI_TURNON, 0);
        MSG_SendCmdUnitParam(0x38, 1, (int32_t)sgUIState.NpcUnitGUID, 0);
        gpNpcTransactionPlayer = CUNIT_GetPlayer();
        UNRESOLVED_word_6FBB5DA4 = 0;
        return 1;
    }
    NPCMENU_NpcCloseInteraction();
    return 1;
}

//1.10f: D2Client.0x6FAF5D20
int32_t __cdecl NPCMENU_OnGambleClicked(void)
{
    D2UnitStrc *pNpc;

    if (sgUIState.hDialogMainMenu)
    {
        DIALOG_Close(sgUIState.hDialogMainMenu);
        sgUIState.hDialogMainMenu = NULL;
    }
    pNpc = CUNIT_FindUnitByGuidAndType((uint32_t)sgUIState.NpcUnitGUID, UNIT_MONSTER);
    if (pNpc && pNpc->pInventory)
    {
        sgUIState.CurrentState = 3;
        sgUIState.bNpcVendorGamble = TRUE;
        UI_SetUIToggle(UI_NPCSHOP, UI_TURNON, 0);
        MSG_SendCmdUnitParam(0x38, 2, (int32_t)sgUIState.NpcUnitGUID, 0);
        gpNpcTransactionPlayer = CUNIT_GetPlayer();
        UNRESOLVED_word_6FBB5DA4 = 0;
        return 1;
    }
    NPCMENU_NpcEndInteraction((uint32_t)sgUIState.NpcUnitGUID);
    return 1;
}

//1.10f: D2Client.0x6FAF9710
//  The news row of the interrupt dialog (its pfnOnClick). With the NPC present: its speech popup for gwNpcSpeechTblId, the id appended to the
//  speech ring under the guard of 9, then the state back to 0 when no next line is queued (0xFFFF), or gbNpcSpeechActive raised and
//  NPCMENU_ChooseNextSpeech handed to TEXT_SetCallback, and client packet 0x31 (the acknowledgement the two cancel callbacks send too).
//  Without it: NPCMENU_NpcCloseInteraction, as OnTradeClicked's arm. The original inlines NPCMENU_NpcCloseInteraction here and
//  NPCMENU_NPC_GetActiveNpcUnit twice (as the test, and as TEXT_ShowSpeechPopup's first argument with the xor eax, eax arm intact).
//  The tbl id goes over in dx alone (mov dx, word ptr gwNpcSpeechTblId): the high half of edx is whatever CUNIT_FindUnitByGuidAndType left
//  there. INERT: every reader of the id (TEXT_ShowSpeechPopup, D2Lang #10004, OBJECT_FindSpeechSoundByTblIndex) uses only its low word. The
//  call passes the word, which computes the same.
int32_t __cdecl NPCMENU_OnInterruptNewsClicked(void)
{
    if (sgUIState.hDialogInterrupt)
    {
        DIALOG_Close(sgUIState.hDialogInterrupt);
        sgUIState.hDialogInterrupt = NULL;
    }
    if (NPCMENU_NPC_GetActiveNpcUnit())
    {
        TEXT_ShowSpeechPopup(NPCMENU_NPC_GetActiveNpcUnit(), (int16_t)sgUIState.wNpcSpeechTblId, 0);
        if (sgUIState.nNpcSpeechesPlayed < 9)
        {
            sgUIState.wNpcSpeechesPlayed[sgUIState.nNpcSpeechesPlayed] = (int16_t)sgUIState.wNpcSpeechTblId;
            ++sgUIState.nNpcSpeechesPlayed;
        }
        if (sgUIState.wNpcNextSpeechTblId == 0xFFFF)
        {
            sgUIState.CurrentState = 0;
        }
        else
        {
            sgUIState.bNpcSpeechActive = 1;
            TEXT_SetCallback((void *)NPCMENU_ChooseNextSpeech, 0);
        }
        MSG_SendCmdUnit(0x31, (int32_t)sgUIState.NpcUnitGUID, sgUIState.wNpcSpeechTblId);
        return 1;
    }
    NPCMENU_NpcCloseInteraction();
    return 1;
}

//1.10f: D2Client.0x6FAF6EE0
//  The transaction-result dialog's dismiss callback and its Continue row. NPCMENU_OnTransactionResult and NPCMENU_ShowTransactionResult each
//  push it twice: as DIALOG_Create's pfnCallback (answer unread) and as DIALOG_AddLine's pfnOnClick (answer read). As the row's handler its 1 is
//  what DIALOG_OnEnter wants to arm the toggle delay, and it keeps DIALOG_OnMouseUp from writing bLinePressActive through the dialog pointer
//  it saved before the call, the dialog this function has just closed.
//  It clears the selection, plays sound 1, closes the dialog, and then:
//      state 12, transaction 5   NPCMENU_NpcEndInteraction, and UI_NPCMENU off a second time: the teardown without
//                                NPCMENU_NpcCloseInteraction's state reset and item-description reset
//      state 12, any other       the main menu rebuilt
//      any other state           the two HealPlayer asserts, npcmenu.cpp 3256 and 3257, then state 3 and the shop closed with the cursor
//                                put back
//  6 is the transaction HealPlayer and 8 the state UIStateHealPlayer, by their asserts; 12 and 5 have no name in the binary's strings.
//  The original inlines NPCMENU_NpcEndInteraction here. DIALOG_Close is handed the dialog with no test around the load. The state is loaded once,
//  after the close and before the handle is cleared, and compared with 12 and then with 8 from the same register; nothing runs between the
//  compares on the path that reaches the second, so reading the global again is the same value.
int32_t __cdecl NPCMENU_OnResultContinueClicked(void)
{
    SELECT_ClearSelectedUnit();
    SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
    DIALOG_Close(sgUIState.hDialogResults);
    sgUIState.hDialogResults = NULL;
    if (sgUIState.CurrentState == 12)
    {
        if (sgUIState.Transaction == 5)
        {
            NPCMENU_NpcEndInteraction((uint32_t)sgUIState.NpcUnitGUID);
            UI_SetUIToggle(UI_NPCMENU, UI_TURNOFF, 0);
            return 1;
        }
        NPCMENU_BuildMainMenu();
        return 1;
    }
    if (sgUIState.Transaction == 6)
    {
        FOG_DisplayAssert((uint32_t)"sgUIState.Transaction != HealPlayer", (uint32_t)szSourceFile, 3256);
        exit(-1);
    }
    if (sgUIState.CurrentState == 8)
    {
        FOG_DisplayAssert((uint32_t)"sgUIState.CurrentState != UIStateHealPlayer", (uint32_t)szSourceFile, 3257);
        exit(-1);
    }
    sgUIState.CurrentState = 3;
    INV_NpcShopCloseAndRestoreCursor();
    return 1;
}

//1.10f: D2Client.0x6FAF7D90
//  PLAYER_AdvanceAnimMode's hook into this unit, so it runs when the player starts to move, attack or die. For the local player only: every
//  menu entry is marked aborted (+0x15) and any entry waiting to re-roll its line (+0x11 at 1) has the flag cleared and the line re-rolled;
//  then, if a conversation is open and its unit still exists, NPCMENU_NpcCloseInteraction. The original inlines
//  NPCMENU_NPC_GetActiveNpcUnit and NPCMENU_NpcCloseInteraction here.
//  BUG: gbNpcInteractionActive is cleared twice, once inside the inlined NPCMENU_NpcEndInteraction (0x6FAF7EDC) and again after the second
//  UI_SetUIToggle (0x6FAF7F14). NOT ESTABLISHED: it reads as a redundant store, but the full reach of UI_SetUIToggle and UNITMODE_SetAnimMode
//  was not traced. The store is written; whether the source wrote it or NPCMENU_SetInteractingNpc(NULL) the bytes cannot say.
//  The assert names a variable the source had and the bytes do not keep: hCtrlUnit != NULL, npcmenu.cpp 3845, over a CUNIT_GetPlayer() whose
//  answer nothing else reads.
//  gnNpcMenuEntryCount is re-read on every pass (mov eax, <count> at the bottom of the loop), which is a do-while under a > 0 test, both as the
//  bytes have them.
void __fastcall NPCMENU_AbortOnPlayerAction(D2UnitStrc *pUnit)
{
    int32_t nEntry;
    D2NpcMenuEntryStrc *pEntry;

    if (pUnit == CUNIT_GetPlayer())
    {
        nEntry = 0;
        if (gnNpcMenuEntryCount > 0)
        {
            pEntry = gNpcMenuEntries;
            do
            {
                if (pEntry->bRerollLine == 1)
                {
                    pEntry->bRerollLine = 0;
                    if (!CUNIT_GetPlayer())
                    {
                        FOG_DisplayAssert((uint32_t)"hCtrlUnit != NULL", (uint32_t)szSourceFile, 3845);
                        exit(-1);
                    }
                    NPCMENU_PickRandomSpeechLine(pEntry);
                }
                pEntry->bAborted = 1;
                ++nEntry;
                ++pEntry;
            }
            while (nEntry < gnNpcMenuEntryCount);
        }
        if (NPCMENU_NPC_GetActiveNpcUnit())
        {
            NPCMENU_NpcCloseInteraction();
            sgUIState.bNpcInteractionActive = 0;
        }
    }
}

//1.10f: D2Client.0x6FAFBF80
//  The conversation's end from the text side (TEXT_OnPopupDismissed and TEXT_TickStaticInfoPanel call it): a trace when the GUID it is handed is
//  not the conversation's (the module's only push of " different ids at line %d file %s", with the original's own line, 5553), then
//  NPCMENU_NpcEndInteraction (inlined in the original), then the NPC looked up once more and UNITFLAG_TARGETABLE raised on it again.
//  BUG: the flag is raised twice, once inside the inlined NPCMENU_NpcEndInteraction and once after it (or dword ptr [eax+0C4h], 2 at
//  0x6FAFC0A8), with the lookup repeated. NOT ESTABLISHED: whether the second can find the bit cleared (NPCMENU_MonsterSetDataFlag and
//  UNITMODE_SetAnimMode run between). Reproduced.
//  The second raise has one null test, the lookup's, so it is written as NPCMENU_SetUnitFlag on the lookup's answer, whose own test is that
//  one, rather than as an if around a call, which would be two.
void __fastcall NPCMENU_EndInteraction(uint32_t dwNpcGUID)
{
    if (dwNpcGUID != (uint32_t)sgUIState.NpcUnitGUID)
    {
        FOG_Trace(" different ids at line %d file %s\n", 5553, szSourceFile);
    }
    NPCMENU_NpcEndInteraction(dwNpcGUID);
    NPCMENU_SetUnitFlag(CUNIT_FindUnitByGuidAndType(dwNpcGUID, UNIT_MONSTER), UNITFLAG_TARGETABLE, TRUE);
}

//1.10f: D2Client.0x6FAF7BD0
//  The lifecycle and the frame: NPCMENU_Init, NPCMENU_Shutdown and NPCMENU_Reset run under UI_InitUiSubsystems, UI_ShutdownUiSubsystems and
//  UI_ResetUiSubsystems; NPCMENU_TickAndValidate runs every frame UI_NPCMENU is on; NPCMENU_EndInteractionByUiState runs from
//  UI_UnregisterPanelHandlers when UI_NPCMENU or UI_NPCSHOP turns off; NPCMENU_CloseAllDialogs is reached through the tick and the teardown.
//  None of them registers a message handler.
//
//  NPCMENU_Init clears the two Nihlathak flags, calls GetTickCount and throws the answer away (a bare call), zeroes 0x15C bytes, allocates the
//  two quest records, zeroes the UI state, loads the buy/sell panel and the gold button, and zeroes four transaction globals that lie below the
//  zeroed block. The original's zeroing (mov ecx, 57h / rep stosd from gbNpcSkipMenuOnInteract, 0x6FBB5C38) covers 87 dwords: the speech state,
//  the interaction flag and GUID, the hireling offers, both quest record pointers, all seven dialog handles and gpHireListControl. So the two
//  flag stores in front of it, and the UI state store after the allocations, write bytes the zeroing has cleared or is about to. Reproduced.
//  DIFFERENCE: the original's 44 globals are the fields of one packed struct of ours, sgUIState (npcmenu.h, 348 bytes), and the clear is
//  memset(&sgUIState, 0, sizeof(sgUIState)), the same 348 bytes.
//  The gold button's store is scheduled after the four zero stores in the bytes; it is written after its load.
void __cdecl NPCMENU_Init(void)
{
    char szPath[260];

    sgUIState.bNihlathakHurryTimerArmed = 0;
    sgUIState.bNihlathakHurrySoundPlayed = 0;
    GetTickCount();
    memset(&sgUIState, 0, sizeof(sgUIState));
    sgUIState.pNpcQuestRecord = D2COMMON_QUESTRECORD_AllocRecord(NULL);
    sgUIState.pQuestRecordSecondary = D2COMMON_QUESTRECORD_AllocRecord(NULL);
    sgUIState.CurrentState = 0;
    sprintf(szPath, "%s\\ui\\panel\\buysell", aDataGlobal);
    gNpcBuySellImage = ARCHIVE_CellFileLoad(szPath, 0);
    sprintf(szPath, "%s\\ui\\menu\\goldbtn", aDataGlobal);
    gNpcGoldButtonImage = ARCHIVE_CellFileLoad(szPath, 0);
    gnItemActionBurstCount = 0;
    gbTransactionPending = 0;
    gLastTransactionTick = 0;
    gnLastItemActionTick = 0;
}

//1.10f: D2Client.0x6FAF7C90
//  Frees the two quest records, unloads the two images, destroys the hire-list control through its vtable slot 0 with 1 (the vector deleting
//  destructor, as NPCMENU_TeardownHireList does), and closes the seven dialogs in its own order: Confirm, Results, Waiting, MainMenu, HireList,
//  Interrupt, TalkMenu. NPCMENU_CloseAllDialogs closes them in another order, so one is not the other inlined. The two record pointers are
//  nulled after both frees; the bytes schedule the stores between the next loads.
void __cdecl NPCMENU_Shutdown(void)
{
    D2COMMON_QUESTRECORD_FreeRecord(NULL, sgUIState.pNpcQuestRecord);
    D2COMMON_QUESTRECORD_FreeRecord(NULL, sgUIState.pQuestRecordSecondary);
    sgUIState.pNpcQuestRecord = NULL;
    sgUIState.pQuestRecordSecondary = NULL;
    if (gNpcBuySellImage)
    {
        ARCHIVE_CellFileFree(gNpcBuySellImage);
        gNpcBuySellImage = NULL;
    }
    if (gNpcGoldButtonImage)
    {
        ARCHIVE_CellFileFree(gNpcGoldButtonImage);
        gNpcGoldButtonImage = NULL;
    }
    if (sgUIState.pHireListControl)
    {
        (*(void(__thiscall **)(void *, int32_t)) *(uint32_t *)sgUIState.pHireListControl)(sgUIState.pHireListControl, 1);
        sgUIState.pHireListControl = 0;
    }
    if (sgUIState.hDialogConfirm)
    {
        DIALOG_Close(sgUIState.hDialogConfirm);
        sgUIState.hDialogConfirm = NULL;
    }
    if (sgUIState.hDialogResults)
    {
        DIALOG_Close(sgUIState.hDialogResults);
        sgUIState.hDialogResults = NULL;
    }
    if (sgUIState.hDialogWaiting)
    {
        DIALOG_Close(sgUIState.hDialogWaiting);
        sgUIState.hDialogWaiting = NULL;
    }
    if (sgUIState.hDialogMainMenu)
    {
        DIALOG_Close(sgUIState.hDialogMainMenu);
        sgUIState.hDialogMainMenu = NULL;
    }
    if (sgUIState.hDialogHireList)
    {
        DIALOG_Close(sgUIState.hDialogHireList);
        sgUIState.hDialogHireList = NULL;
    }
    if (sgUIState.hDialogInterrupt)
    {
        DIALOG_Close(sgUIState.hDialogInterrupt);
        sgUIState.hDialogInterrupt = NULL;
    }
    if (sgUIState.hDialogTalkMenu)
    {
        DIALOG_Close(sgUIState.hDialogTalkMenu);
        sgUIState.hDialogTalkMenu = NULL;
    }
}

//1.10f: D2Client.0x6FAF6340
//  Closes the same seven dialogs in this order: TalkMenu, MainMenu, Interrupt, Confirm, Results, Waiting, HireList.
void __cdecl NPCMENU_CloseAllDialogs(void)
{
    if (sgUIState.hDialogTalkMenu)
    {
        DIALOG_Close(sgUIState.hDialogTalkMenu);
        sgUIState.hDialogTalkMenu = NULL;
    }
    if (sgUIState.hDialogMainMenu)
    {
        DIALOG_Close(sgUIState.hDialogMainMenu);
        sgUIState.hDialogMainMenu = NULL;
    }
    if (sgUIState.hDialogInterrupt)
    {
        DIALOG_Close(sgUIState.hDialogInterrupt);
        sgUIState.hDialogInterrupt = NULL;
    }
    if (sgUIState.hDialogConfirm)
    {
        DIALOG_Close(sgUIState.hDialogConfirm);
        sgUIState.hDialogConfirm = NULL;
    }
    if (sgUIState.hDialogResults)
    {
        DIALOG_Close(sgUIState.hDialogResults);
        sgUIState.hDialogResults = NULL;
    }
    if (sgUIState.hDialogWaiting)
    {
        DIALOG_Close(sgUIState.hDialogWaiting);
        sgUIState.hDialogWaiting = NULL;
    }
    if (sgUIState.hDialogHireList)
    {
        DIALOG_Close(sgUIState.hDialogHireList);
        sgUIState.hDialogHireList = NULL;
    }
}

//1.10f: D2Client.0x6FAFBD40
//  The resurrect offer's name back to the no-name sentinel (a word store into the four-byte slot, as NPCMENU_SetResurrectMercOffer writes it),
//  then Shutdown and Init, then gbNpcSpeechLinesRolled cleared (Init's zeroing has already cleared that byte: a redundant store, the original's),
//  then each menu entry's bRerollLine and unk0x12 forgotten. The count is read once, before the loop, which runs down to zero.
//  The original inlines NPCMENU_Init here (plus the extra clear of gbNpcSpeechLinesRolled after it).
void __cdecl NPCMENU_Reset(void)
{
    int32_t nEntriesLeft;
    D2NpcMenuEntryStrc *pEntry;

    gwMercResurrectNameTblId = (D2C_StringIndices)0xFFFF;
    NPCMENU_Shutdown();
    NPCMENU_Init();
    sgUIState.bNpcSpeechLinesRolled = 0;
    nEntriesLeft = gnNpcMenuEntryCount;
    if (nEntriesLeft > 0)
    {
        pEntry = gNpcMenuEntries;
        do
        {
            pEntry->bRerollLine = 0;
            pEntry->unk0x12 = 0;
            ++pEntry;
            --nEntriesLeft;
        }
        while (nEntriesLeft);
    }
}

//1.10f: D2Client.0x6FAF6070
//  Runs every frame UI_NPCMENU is on. First the conversation is validated: the player dead or missing, no active NPC, or the NPC's unit in anim
//  mode 12, and NPCMENU_CloseAllDialogs and NPCMENU_NpcCloseInteraction run. Then the first dialog that is up is drawn -- TalkMenu, MainMenu
//  (after the click state is reset), Results -- or, with the Waiting dialog up, which this function never draws: the Nihlathak line, the
//  item-action timer that restores the shop, and the transaction timeout that disconnects. Then Confirm, which closes itself when an item is
//  on the cursor or held, HireList and Interrupt.
//  The original inlines NPCMENU_NPC_GetActiveNpcUnit three times in the validity test and twice more in the Nihlathak block,
//  NPCMENU_CloseAllDialogs with NPCMENU_NpcCloseInteraction's four statements in the abort block, and SEED_RollRandomNumber in the seed roll.
//  The validity test, G being the active-NPC lookup, is  dead || (G() && (pNpc = G()) && pNpc->dwAnimMode == 12) || !G()  and that shape
//  reproduces every edge of the bytes, including the first G() failing on a clear flag going straight to the abort (the compiler threaded
//  that jump).
//  BUG: the Nihlathak line plays at once and the timer never decides when. With the Waiting dialog up and the NPC being Nihlathak (monstats
//  514), a first pass arms gnNihlathakHurryTick = GetTickCount() + 200 * (5 * (seed % 30) + 600), 120 000 to 149 000 ms ahead; then
//  cmp gnNihlathakHurryTick, eax / jbe skip (0x6FAF6215) skips only when the deadline has PASSED, so the line (sounds.txt row 4560,
//  nihlathak_hurryup) plays while the deadline is still ahead, on the very frame it was armed, and gbNihlathakHurrySoundPlayed stops it ever
//  playing again. Only a tick count wrapping inside the two minutes could delay it. Whether a delay was meant is not in the binary.
//  REACHABLE: opening the shop leaves UI_NPCMENU on and this tick running, so the first transaction a game makes with Nihlathak plays the line
//  at once, given one frame between the send and the answer. Reproduced.
//  The seed step's remainder is the low dword's, unsigned (xor edx, edx / div ecx).
//  The Waiting branch's two timers: outside a single-player game (GAME_GetGameType() non-zero; GAMETYPE_SINGLEPLAYER is 0), a set
//  gnNextItemActionTick that has passed closes the waiting dialog (tested again, though the branch is already inside that test), puts the
//  state back to 3 and restores the shop grids. Otherwise a passed gnTransactionTimeoutTick, which NPCMENU_SendTransactionAndWait sets 60 s
//  ahead, runs MSG_Send_D2CLTSYS_ENDGAME, WINMAIN_SetAppModeContinue(FALSE), GAME_SetLeaveGameSent(1) and GAME_SetDisconnecting.
void __cdecl NPCMENU_TickAndValidate(void)
{
    D2UnitStrc *pNpc;
    D2UnitStrc *pPlayer;
    uint32_t nRoll;

    if (CUNIT_IsPlayerDeadOrMissing() || (NPCMENU_NPC_GetActiveNpcUnit() && (pNpc = NPCMENU_NPC_GetActiveNpcUnit()) != NULL && pNpc->dwAnimMode == 12) || !NPCMENU_NPC_GetActiveNpcUnit())
    {
        NPCMENU_CloseAllDialogs();
        NPCMENU_NpcCloseInteraction();
    }

    if (sgUIState.hDialogTalkMenu)
    {
        DIALOG_Draw(sgUIState.hDialogTalkMenu);
        return;
    }
    if (sgUIState.hDialogMainMenu)
    {
        GAME_ResetClickState();
        DIALOG_Draw(sgUIState.hDialogMainMenu);
        return;
    }
    if (sgUIState.hDialogResults)
    {
        DIALOG_Draw(sgUIState.hDialogResults);
        return;
    }
    if (sgUIState.hDialogWaiting)
    {
        if (NPCMENU_NPC_GetActiveNpcUnit() && (pNpc = NPCMENU_NPC_GetActiveNpcUnit()) != NULL && pNpc->dwClassId == MONSTER_NIHLATHAK && !sgUIState.bNihlathakHurrySoundPlayed)
        {
            if (!sgUIState.bNihlathakHurryTimerArmed)
            {
                sgUIState.bNihlathakHurryTimerArmed = 1;
                pPlayer = CUNIT_GetPlayer();
                nRoll = (uint32_t)SEED_RollRandomNumber(&pPlayer->pSeed) % 30;
                sgUIState.nNihlathakHurryTick = GetTickCount() + 200 * (5 * nRoll + 600);
            }
            //  the deadline is still AHEAD, not passed; reproduced
            if (sgUIState.nNihlathakHurryTick > GetTickCount())
            {
                sgUIState.bNihlathakHurrySoundPlayed = 1;
                SOUNDINST_PlaySound(SOUND_NIHLATHAK_HURRYUP, NULL, 0, 0, 0);
            }
        }
        if (GAME_GetGameType() && sgUIState.nNextItemActionTick && sgUIState.nNextItemActionTick < GetTickCount())
        {
            if (sgUIState.hDialogWaiting)
            {
                DIALOG_Close(sgUIState.hDialogWaiting);
                sgUIState.hDialogWaiting = NULL;
            }
            sgUIState.CurrentState = 3;
            INV_ResetItemGrids();
            INV_NpcShopApplyTabSelection();
            SELECT_ClearSelectedUnit();
            return;
        }
        if (sgUIState.nTransactionTimeoutTick < GetTickCount())
        {
            MSG_Send_D2CLTSYS_ENDGAME();
            WINMAIN_SetAppModeContinue(FALSE);
            GAME_SetLeaveGameSent(1);
            GAME_SetDisconnecting();
        }
        return;
    }
    if (sgUIState.hDialogConfirm)
    {
        if (D2COMMON_INVENTORY_GetCursorItem(CUNIT_GetPlayer()->pInventory) || CURSOR_GetHeldUnit())
        {
            sgUIState.CurrentState = 3;
            INV_TradeRestoreHeldItem();
            gnLastItemActionTick = GetTickCount();
            ++gnItemActionBurstCount;
            DIALOG_Close(sgUIState.hDialogConfirm);
            sgUIState.hDialogConfirm = NULL;
        }
        else
        {
            DIALOG_Draw(sgUIState.hDialogConfirm);
        }
        return;
    }
    if (sgUIState.hDialogHireList)
    {
        DIALOG_Draw(sgUIState.hDialogHireList);
        return;
    }
    if (sgUIState.hDialogInterrupt)
    {
        DIALOG_Draw(sgUIState.hDialogInterrupt);
    }
}

//1.10f: D2Client.0x6FAF9FE0
//  What UI_UnregisterPanelHandlers runs when UI_NPCMENU or UI_NPCSHOP turns off, chosen by the UI state, loaded once and compared signed
//  (jl and jg), so it is read into an int here:
//      1       gbNpcSpeechActive cleared, then NPCMENU_NpcCloseInteraction
//      3..6    NPCMENU_NpcShopCloseAndEndInteraction, then INV_TradeResetPanelState
//      7       USEITEM_AnvilCloseOnInteractionStop
//      other   NPCMENU_NpcCloseInteraction only if a speech is active, clearing it
//  and every arm ends in NPCMENU_CloseAllDialogs and the state zeroed. The original inlines the teardowns: NPCMENU_NpcCloseInteraction in the
//  state-1 and speech arms (the compiler merged their tails from the 0x30 send onward) and NPCMENU_NpcShopCloseAndEndInteraction in the 3..6 arm.
//  The shop arm can re-enter: NPCMENU_NpcShopCloseAndEndInteraction turns UI_NPCSHOP off before it zeroes the state, and a turn-off that changes
//  the panel calls this function again with the state still 3..6. When the outer call came from UI_NPCMENU's turn-off with the shop still up,
//  the inner call runs the shop arm's teardown too. That is the original's call graph, reproduced by calling the same functions; what it costs
//  at run time (a second packet 0x30 among it) is not established.
void __cdecl NPCMENU_EndInteractionByUiState(void)
{
    int32_t nState = (int32_t)sgUIState.CurrentState;

    if (nState == 1)
    {
        sgUIState.bNpcSpeechActive = 0;
        NPCMENU_NpcCloseInteraction();
    }
    else if (nState >= 3 && nState <= 6)
    {
        NPCMENU_NpcShopCloseAndEndInteraction();
        INV_TradeResetPanelState();
    }
    else if (nState == 7)
    {
        USEITEM_AnvilCloseOnInteractionStop();
    }
    else if (sgUIState.bNpcSpeechActive)
    {
        sgUIState.bNpcSpeechActive = 0;
        NPCMENU_NpcCloseInteraction();
    }
    NPCMENU_CloseAllDialogs();
    sgUIState.CurrentState = 0;
}

//  The item-message acknowledgements: six functions that each settle a pending item message, and NPCMENU_ClearSelectionIfStoreItem. In UI state 5
//  NPCMENU_OnTransactionResult looks the transaction's item up by the GUID packet 0x2A carries, and sets gbWaitingForItemMsg instead of closing
//  the waiting dialog in two cases: result code 0 while the item is not yet on the client, result code 1 while it still is. The item message
//  that settles it then calls one of the six with that item's GUID. Each is named after its caller:
//      ITEM_ItemMsg05_ToInventoryGrid    -> NPCMENU_AckItemAction_ToInventoryGrid     result 1
//      ITEM_ItemMsg08_RemoveFromBodyLoc  -> NPCMENU_AckItemAction_RemoveFromBodyLoc   result 1
//      ITEM_ItemMsg06_InsertIntoBodyLoc  -> NPCMENU_AckItemAction_InsertIntoBodyLoc   result 0
//      ITEM_ItemMsg04_ToStorePage        -> NPCMENU_AckItemAction_ToStorePage         result 0
//      ITEM_ItemMsg0E_SetInvPage         -> NPCMENU_AckItemAction_SetInvPage          result 0
//      packet 0x42 (ITEM_NpcMenuAckItemActionAndDropCursorItem's tail jump) -> NPCMENU_AckItemMessage   result 1
//  Six copies, two routines: the result-1 set tests cmp gLastTransactionResult.nResult, 1 (an immediate), the result-0 set cmp ..., bl with ebx
//  zeroed at entry; within each set the bodies are the same instructions.
//  On a match, the throttle: now - gnLastItemActionTick <= 1000 (jbe, unsigned) ? now + 100 * (5 * min(gnItemActionBurstCount, 10) + 150) :
//  now + 100, stored into gnNextItemActionTick -- 15 000 ms plus 500 per counted action, capped at 20 000. gnLastItemActionTick and the increment
//  of gnItemActionBurstCount are written only on two abort arms: NPCMENU_SendTransactionAndWait's cancelled buy (the "buy cancelled for non
//  existant item?" trace) and NPCMENU_TickAndValidate's Confirm dialog closed by a cursor item; only NPCMENU_Init and NPCMENU_Reset clear either.
//  So the long lock follows an acknowledgement inside a second of an aborted action. The deadline is read by NPCMENU_TickAndValidate's waiting
//  branch, which acts on it only outside single player; in single player these six close the waiting dialog themselves.
//  All seven are void: the original leaves the eax of a tail call and no caller reads it. The FOG_Trace calls name line 4590 (0x11EE).

static_assert(offsetof(D2GSPacketSrv2A, nResult) == 0x02, "AckItemMessage: cmp gLastTransactionResult+2, 1 at 0x6FAFAAB0");
static_assert(offsetof(D2GSPacketSrv2A, dwItemGUID) == 0x07, "AckItemMessage: cmp ecx, [0x6FBB5D5A] at 0x6FAFAABD");
static_assert(GAMETYPE_SINGLEPLAYER == 0, "AckItemMessage: GAME_GetGameType / test eax, eax / jnz at 0x6FAFAB43");

//1.10f: D2Client.0x6FAFAAA0
void __fastcall NPCMENU_AckItemMessage(uint32_t dwItemGUID)
{
    uint32_t dwNow;
    uint32_t nBurst;

    if (sgUIState.bWaitingForItemMsg && sgUIState.LastTransactionResult.nResult == 1 && dwItemGUID == sgUIState.LastTransactionResult.dwItemGUID)
    {
        dwNow = GetTickCount();
        if (dwNow - gnLastItemActionTick <= 1000)
        {
            nBurst = gnItemActionBurstCount;
            if (nBurst >= 10)
                nBurst = 10;
            sgUIState.nNextItemActionTick = dwNow + 100 * (5 * nBurst + 150);
        }
        else
        {
            sgUIState.nNextItemActionTick = dwNow + 100;
        }
        sgUIState.bWaitingForItemMsg = 0;
        if (!gbTransactionPending)
            FOG_Trace("got overlapping item msgs at line %d of file %s\n", 4590, szSourceFile);
        gbTransactionPending = 0;
        gTransactionItemClassId = 0;
        gTransactionItemGUID = 0;
        if (GAME_GetGameType() == GAMETYPE_SINGLEPLAYER)
        {
            if (sgUIState.hDialogWaiting)
            {
                DIALOG_Close(sgUIState.hDialogWaiting);
                sgUIState.hDialogWaiting = NULL;
            }
            sgUIState.CurrentState = 3;
            INV_ResetItemGrids();
            INV_NpcShopApplyTabSelection();
            SELECT_ClearSelectedUnit();
        }
    }
}

//1.10f: D2Client.0x6FAFAB80
void __fastcall NPCMENU_AckItemAction_ToInventoryGrid(uint32_t dwItemGUID)
{
    uint32_t dwNow;
    uint32_t nBurst;

    if (sgUIState.bWaitingForItemMsg && sgUIState.LastTransactionResult.nResult == 1 && dwItemGUID == sgUIState.LastTransactionResult.dwItemGUID)
    {
        dwNow = GetTickCount();
        if (dwNow - gnLastItemActionTick <= 1000)
        {
            nBurst = gnItemActionBurstCount;
            if (nBurst >= 10)
                nBurst = 10;
            sgUIState.nNextItemActionTick = dwNow + 100 * (5 * nBurst + 150);
        }
        else
        {
            sgUIState.nNextItemActionTick = dwNow + 100;
        }
        sgUIState.bWaitingForItemMsg = 0;
        if (!gbTransactionPending)
            FOG_Trace("got overlapping item msgs at line %d of file %s\n", 4590, szSourceFile);
        gbTransactionPending = 0;
        gTransactionItemClassId = 0;
        gTransactionItemGUID = 0;
        if (GAME_GetGameType() == GAMETYPE_SINGLEPLAYER)
        {
            if (sgUIState.hDialogWaiting)
            {
                DIALOG_Close(sgUIState.hDialogWaiting);
                sgUIState.hDialogWaiting = NULL;
            }
            sgUIState.CurrentState = 3;
            INV_ResetItemGrids();
            INV_NpcShopApplyTabSelection();
            SELECT_ClearSelectedUnit();
        }
    }
}

//1.10f: D2Client.0x6FAFAC60
void __fastcall NPCMENU_AckItemAction_RemoveFromBodyLoc(uint32_t dwItemGUID)
{
    uint32_t dwNow;
    uint32_t nBurst;

    if (sgUIState.bWaitingForItemMsg && sgUIState.LastTransactionResult.nResult == 1 && dwItemGUID == sgUIState.LastTransactionResult.dwItemGUID)
    {
        dwNow = GetTickCount();
        if (dwNow - gnLastItemActionTick <= 1000)
        {
            nBurst = gnItemActionBurstCount;
            if (nBurst >= 10)
                nBurst = 10;
            sgUIState.nNextItemActionTick = dwNow + 100 * (5 * nBurst + 150);
        }
        else
        {
            sgUIState.nNextItemActionTick = dwNow + 100;
        }
        sgUIState.bWaitingForItemMsg = 0;
        if (!gbTransactionPending)
            FOG_Trace("got overlapping item msgs at line %d of file %s\n", 4590, szSourceFile);
        gbTransactionPending = 0;
        gTransactionItemClassId = 0;
        gTransactionItemGUID = 0;
        if (GAME_GetGameType() == GAMETYPE_SINGLEPLAYER)
        {
            if (sgUIState.hDialogWaiting)
            {
                DIALOG_Close(sgUIState.hDialogWaiting);
                sgUIState.hDialogWaiting = NULL;
            }
            sgUIState.CurrentState = 3;
            INV_ResetItemGrids();
            INV_NpcShopApplyTabSelection();
            SELECT_ClearSelectedUnit();
        }
    }
}

//1.10f: D2Client.0x6FAFAD40
//  The result-0 set: cmp gLastTransactionResult.nResult, bl with ebx zeroed.
void __fastcall NPCMENU_AckItemAction_InsertIntoBodyLoc(uint32_t dwItemGUID)
{
    uint32_t dwNow;
    uint32_t nBurst;

    if (sgUIState.bWaitingForItemMsg && sgUIState.LastTransactionResult.nResult == 0 && dwItemGUID == sgUIState.LastTransactionResult.dwItemGUID)
    {
        dwNow = GetTickCount();
        if (dwNow - gnLastItemActionTick <= 1000)
        {
            nBurst = gnItemActionBurstCount;
            if (nBurst >= 10)
                nBurst = 10;
            sgUIState.nNextItemActionTick = dwNow + 100 * (5 * nBurst + 150);
        }
        else
        {
            sgUIState.nNextItemActionTick = dwNow + 100;
        }
        sgUIState.bWaitingForItemMsg = 0;
        if (!gbTransactionPending)
            FOG_Trace("got overlapping item msgs at line %d of file %s\n", 4590, szSourceFile);
        gbTransactionPending = 0;
        gTransactionItemClassId = 0;
        gTransactionItemGUID = 0;
        if (GAME_GetGameType() == GAMETYPE_SINGLEPLAYER)
        {
            if (sgUIState.hDialogWaiting)
            {
                DIALOG_Close(sgUIState.hDialogWaiting);
                sgUIState.hDialogWaiting = NULL;
            }
            sgUIState.CurrentState = 3;
            INV_ResetItemGrids();
            INV_NpcShopApplyTabSelection();
            SELECT_ClearSelectedUnit();
        }
    }
}

//1.10f: D2Client.0x6FAFAE20
void __fastcall NPCMENU_AckItemAction_ToStorePage(uint32_t dwItemGUID)
{
    uint32_t dwNow;
    uint32_t nBurst;

    if (sgUIState.bWaitingForItemMsg && sgUIState.LastTransactionResult.nResult == 0 && dwItemGUID == sgUIState.LastTransactionResult.dwItemGUID)
    {
        dwNow = GetTickCount();
        if (dwNow - gnLastItemActionTick <= 1000)
        {
            nBurst = gnItemActionBurstCount;
            if (nBurst >= 10)
                nBurst = 10;
            sgUIState.nNextItemActionTick = dwNow + 100 * (5 * nBurst + 150);
        }
        else
        {
            sgUIState.nNextItemActionTick = dwNow + 100;
        }
        sgUIState.bWaitingForItemMsg = 0;
        if (!gbTransactionPending)
            FOG_Trace("got overlapping item msgs at line %d of file %s\n", 4590, szSourceFile);
        gbTransactionPending = 0;
        gTransactionItemClassId = 0;
        gTransactionItemGUID = 0;
        if (GAME_GetGameType() == GAMETYPE_SINGLEPLAYER)
        {
            if (sgUIState.hDialogWaiting)
            {
                DIALOG_Close(sgUIState.hDialogWaiting);
                sgUIState.hDialogWaiting = NULL;
            }
            sgUIState.CurrentState = 3;
            INV_ResetItemGrids();
            INV_NpcShopApplyTabSelection();
            SELECT_ClearSelectedUnit();
        }
    }
}

//1.10f: D2Client.0x6FAFAF00
void __fastcall NPCMENU_AckItemAction_SetInvPage(uint32_t dwItemGUID)
{
    uint32_t dwNow;
    uint32_t nBurst;

    if (sgUIState.bWaitingForItemMsg && sgUIState.LastTransactionResult.nResult == 0 && dwItemGUID == sgUIState.LastTransactionResult.dwItemGUID)
    {
        dwNow = GetTickCount();
        if (dwNow - gnLastItemActionTick <= 1000)
        {
            nBurst = gnItemActionBurstCount;
            if (nBurst >= 10)
                nBurst = 10;
            sgUIState.nNextItemActionTick = dwNow + 100 * (5 * nBurst + 150);
        }
        else
        {
            sgUIState.nNextItemActionTick = dwNow + 100;
        }
        sgUIState.bWaitingForItemMsg = 0;
        if (!gbTransactionPending)
            FOG_Trace("got overlapping item msgs at line %d of file %s\n", 4590, szSourceFile);
        gbTransactionPending = 0;
        gTransactionItemClassId = 0;
        gTransactionItemGUID = 0;
        if (GAME_GetGameType() == GAMETYPE_SINGLEPLAYER)
        {
            if (sgUIState.hDialogWaiting)
            {
                DIALOG_Close(sgUIState.hDialogWaiting);
                sgUIState.hDialogWaiting = NULL;
            }
            sgUIState.CurrentState = 3;
            INV_ResetItemGrids();
            INV_NpcShopApplyTabSelection();
            SELECT_ClearSelectedUnit();
        }
    }
}

//1.10f: D2Client.0x6FAFAFE0
//  Called from ITEM_ItemMsg0B0C_UpdateNpcStore right after CUNIT_RemoveUnitById has taken the store item away: when something is selected, a
//  conversation is on and the NPC's unit still exists, and the selection is that item (cmp dword ptr [esi], 4 -- UNIT_ITEM -- and
//  cmp [esi+0Ch], edi), the selection and the item description are dropped. The NPC lookup's answer is only tested. The selected unit is
//  fetched before the flag is read, as the bytes do.
void __fastcall NPCMENU_ClearSelectionIfStoreItem(D2UnitGUID dwItemGUID)
{
    D2UnitStrc *pSelected = SELECT_GetSelectedUnit();

    if (pSelected && NPCMENU_NPC_GetActiveNpcUnit() && pSelected->dwUnitType == UNIT_ITEM && pSelected->dwUnitId == dwItemGUID)
    {
        SELECT_ClearSelectedUnit();
        INV_ResetItemDescState();
    }
}

//  The speech popups. NPCMENU_BuildTalkMenu adds the talk menu's rows with these as pfnOnClick: 'introduction' (PlayClassGreeting, only while
//  the entry's bIntroHeard is clear), 'gossip' (ShowDescriptorSpeech, whenever the message list exists), one row per quest line
//  (OnTalkLineClicked), Jerhyn's 'about the merchants' and a Cain's 'Horadric Cube' row. Each hands TEXT_ShowSpeechPopup a tbl id, raises
//  gbNpcSpeechActive, arms TEXT_SetCallback with NPCMENU_OnSpeechDismissed and closes the talk menu. When the popup goes, OnSpeechDismissed
//  either shows the next line and re-arms itself or rebuilds the talk menu. gwNpcShownSpeechTblId is the id each store hands the popup next.
//  The original inlines NPCMENU_RerollAllSpeechLines in the re-roll blocks of ShowDescriptorSpeech and ShowSpeechLine (PlayClassGreeting calls
//  it), and the teardown of NPCMENU_NpcCloseInteraction in the NPC-gone arms of those two; the teardown is written as a call.
//  PickRandomSpeechLine copes with a zero line count (index 0, ten misses, 2) and returns 2; a quest-gated line whose state does not match goes
//  round the loop again. Its one special table is Jerhyn's (gNpcMenuEntries[15], class 201): gJerhynSpeechLines, ten lines.
//  BUG: PickRandomSpeechLine tests the player for its class id and then reads the same pointer's seed with no test when the line count is
//  positive (mov ebx, [esi+24h] at 0x6FAF44FA). Reproduced through SEED_RollRandomNumber, whose body that LCG is.
//  BUG: gNpcMenuEntries 6, 7 and 8 (classes 331 act2guard2, 266 navi, 408 malachai) carry nSpeechLineCount 0; the other 43 have 3 to 16.
//  ShowDescriptorSpeech divides by it (idiv dword ptr [esi+9] at 0x6FAF4495) and ShowSpeechLine would too (0x6FAF4C0A). LATENT: all three
//  have a Talk row (gNpcMenuDescriptors 32, 33 and 34) and BuildTalkMenu adds 'gossip' whenever the message list exists, but both calls of
//  NPCMENU_OpenMainMenu are behind NPCMENU_IsSpeechOnlyNpc, which answers true for all three, so their main menu never opens. Reproduced as a
//  % on the raw count.

static_assert(offsetof(D2NpcMenuEntryStrc, pSpeechLineTable) == 0x05, "ShowDescriptorSpeech: mov edx, [esi+5] at 0x6FAF449C");
static_assert(offsetof(D2NpcMenuEntryStrc, nSpeechLineCount) == 0x09, "ShowDescriptorSpeech: idiv dword ptr [esi+9] at 0x6FAF4495");
static_assert(offsetof(D2NpcMenuEntryStrc, nSelectedLine) == 0x0D, "ShowDescriptorSpeech: mov esi, [eax+0Dh] at 0x6FAF4309");
static_assert(offsetof(D2NpcMenuEntryStrc, bIntroHeard) == 0x14, "OnTalkLineClicked: gNpcMenuEntries+14h at 0x6FAF4C49");
static_assert(offsetof(D2NpcSpeechLineStrc, bHasQuestGate) == 0x02, "PickRandomSpeechLine: mov cl, [eax+2] at 0x6FAF4554");
static_assert(offsetof(D2NpcSpeechLineStrc, nRequiredState) == 0x03, "PickRandomSpeechLine: cmp eax, [ecx+edx+3] at 0x6FAF4578");
static_assert(offsetof(D2NpcSpeechLineStrc, nQuestStateFlag) == 0x07, "PickRandomSpeechLine: mov edx, [eax+7] at 0x6FAF455B");
static_assert(offsetof(D2NpcSpeechLineStrc, nClassFilter) == 0x0B, "PickRandomSpeechLine: mov ecx, [eax+0Bh] at 0x6FAF4548");
static_assert(STR_IDX_3381_TalkMenu == 0xD35, "OnSpeechDismissed: mov edx, 0D35h at 0x6FAF473F");
static_assert(STR_IDX_254_JerhynActIntroMoreGossip1 == 0xFE, "OnJerhynMoreGossipClicked: mov edx, 0FEh at 0x6FAF4D6C");
static_assert(STR_IDX_338_A2Q2EarlyReturnCubeCain == 0x152, "OnCainCubeGossipClicked: mov edx, 152h at 0x6FAF4DEC");

//1.10f: D2Client.0x6FAF44C0
void __fastcall NPCMENU_PickRandomSpeechLine(D2NpcMenuEntryStrc *pSpeechState)
{
    int32_t nTriesLeft = 10;
    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    int32_t nPlayerClassId;
    int32_t nLineCount;
    int32_t nLine;
    uint32_t nRoll;
    D2NpcSpeechLineStrc *pLine;

    nPlayerClassId = pPlayer ? (int32_t)pPlayer->dwClassId : -1;
    for (;;)
    {
        nLineCount = (int32_t)pSpeechState->nSpeechLineCount;
        --nTriesLeft;
        if (nLineCount > 0)
        {
            //  the original reads the seed without testing pPlayer
            nRoll = (uint32_t)SEED_RollRandomNumber(&pPlayer->pSeed);
            if ((nLineCount - 1) & nLineCount)
                nLine = (int32_t)(nRoll % (uint32_t)nLineCount);
            else
                nLine = (int32_t)(nRoll & (uint32_t)(nLineCount - 1));
        }
        else
        {
            nLine = 0;
        }
        pSpeechState->nSelectedLine = nLine;
        if (nLine >= 2)
        {
            pLine = &pSpeechState->pSpeechLineTable[nLine];
            if (pLine->nClassFilter == nPlayerClassId || pLine->nClassFilter == 7)
            {
                if (!pLine->bHasQuestGate)
                    return;
                if (D2COMMON_QUESTRECORD_GetQuestState(sgUIState.pNpcQuestRecord, pLine->nQuestStateFlag, 0) == pSpeechState->pSpeechLineTable[pSpeechState->nSelectedLine].nRequiredState)
                    break;
            }
        }
        if (!nTriesLeft)
        {
            pSpeechState->nSelectedLine = 2;
            return;
        }
    }
    if (pSpeechState->pSpeechLineTable == gJerhynSpeechLines && pSpeechState->pSpeechLineTable[pSpeechState->nSelectedLine].nTblStringId == 255 && D2COMMON_QUESTRECORD_GetQuestState(sgUIState.pQuestRecordSecondary, 12, 13) == 1)
    {
        pSpeechState->nSelectedLine = 2;
    }
}

//1.10f: D2Client.0x6FAF4300
//  The 'gossip' row. The selected line is read on entry and read again only if the lines have not been rolled yet; rolling is what picks it.
//  With the NPC gone, the whole teardown instead.
int32_t __cdecl NPCMENU_ShowDescriptorSpeech(void)
{
    int32_t nLine = (int32_t)(sgUIState.pNpcSpeechDesc)->nSelectedLine;
    D2UnitStrc *pNpc;

    if (NPCMENU_NPC_GetActiveNpcUnit())
    {
        sgUIState.bNpcSpeechActive = 1;
        TEXT_SetCallback((void *)NPCMENU_OnSpeechDismissed, 0);
        DIALOG_Close(sgUIState.hDialogTalkMenu);
        sgUIState.hDialogTalkMenu = NULL;
        (sgUIState.pNpcSpeechDesc)->bRerollLine = 1;
        if (!sgUIState.bNpcSpeechLinesRolled)
        {
            NPCMENU_RerollAllSpeechLines();
            nLine = (int32_t)(sgUIState.pNpcSpeechDesc)->nSelectedLine;
        }
        pNpc = NPCMENU_NPC_GetActiveNpcUnit();
        //  cdq / idiv dword ptr [esi+9]: a signed divide, and by zero for gNpcMenuEntries 6..8. Reproduced.
        TEXT_ShowSpeechPopup(pNpc, (int16_t)(sgUIState.pNpcSpeechDesc)->pSpeechLineTable[nLine % (int32_t)(sgUIState.pNpcSpeechDesc)->nSpeechLineCount].nTblStringId, 0);
        return 1;
    }
    NPCMENU_NpcCloseInteraction();
    return 1;
}

//1.10f: D2Client.0x6FAF4A70
//  The sole caller, PlayClassGreeting, passes 0 and FALSE (xor ecx, ecx / xor dl, dl at 0x6FAF4A29), so the bTakeLineFromDescriptor arm never
//  runs in the shipped game.
void __fastcall NPCMENU_ShowSpeechLine(int32_t nSpeechLine, uint8_t bTakeLineFromDescriptor)
{
    D2UnitStrc *pNpc;

    if (NPCMENU_NPC_GetActiveNpcUnit())
    {
        sgUIState.bNpcSpeechActive = 1;
        TEXT_SetCallback((void *)NPCMENU_OnSpeechDismissed, 0);
        DIALOG_Close(sgUIState.hDialogTalkMenu);
        sgUIState.hDialogTalkMenu = NULL;
        (sgUIState.pNpcSpeechDesc)->bRerollLine = 1;
        if (!sgUIState.bNpcSpeechLinesRolled)
        {
            NPCMENU_RerollAllSpeechLines();
            if (bTakeLineFromDescriptor)
                nSpeechLine = (int32_t)(sgUIState.pNpcSpeechDesc)->nSelectedLine;
        }
        pNpc = NPCMENU_NPC_GetActiveNpcUnit();
        TEXT_ShowSpeechPopup(pNpc, (int16_t)(sgUIState.pNpcSpeechDesc)->pSpeechLineTable[nSpeechLine % (int32_t)(sgUIState.pNpcSpeechDesc)->nSpeechLineCount].nTblStringId, 0);
        return;
    }
    NPCMENU_NpcCloseInteraction();
}

//1.10f: D2Client.0x6FAF4C30
//  A quest-line row. Two header rows above the quest lines once the introduction has been heard, three before (neg dl / sbb edx, edx /
//  add edx, 3). The selected line is asked for twice, before and after the NPC test, as the bytes do. The assert names line 1760.
int32_t __cdecl NPCMENU_OnTalkLineClicked(void)
{
    int32_t nSelected = DIALOG_GetSelectedLine(sgUIState.hDialogTalkMenu);
    int32_t nFirstTalkLine = 3 - (gNpcMenuEntries[sgUIState.nTalkMenuBaseEntryIdx].bIntroHeard != 0);
    int32_t nNode;
    D2TextHeaderStrc *pTextHeader;
    int16_t wTblStringId;
    D2UnitStrc *pNpc;

    if (nSelected < nFirstTalkLine || !NPCMENU_NPC_GetActiveNpcUnit())
    {
        return 0;
    }
    nNode = DIALOG_GetSelectedLine(sgUIState.hDialogTalkMenu) - nFirstTalkLine;
    pTextHeader = TEXT_GetMessageListHead();
    if (!pTextHeader)
    {
        FOG_DisplayAssert((uint32_t)"hTextList != NULL", (uint32_t)szSourceFile, 1760);
        exit(-1);
    }
    wTblStringId = D2COMMON_TEXT_GetStringIdOfMenuType2Node(pTextHeader, nNode);
    sgUIState.wNpcShownSpeechTblId = (uint16_t)wTblStringId;
    pNpc = NPCMENU_NPC_GetActiveNpcUnit();
    TEXT_ShowSpeechPopup(pNpc, wTblStringId, 0);
    sgUIState.bNpcSpeechActive = 1;
    TEXT_SetCallback((void *)NPCMENU_OnSpeechDismissed, 0);
    DIALOG_Close(sgUIState.hDialogTalkMenu);
    sgUIState.hDialogTalkMenu = NULL;
    return 1;
}

//1.10f: D2Client.0x6FAF4D30
//  Jerhyn's tour of Lut Gholein, tbl 254; its twin below is a Cain's word on the Horadric Cube, tbl 338. They differ in one instruction, the
//  tbl id.
int32_t __cdecl NPCMENU_OnJerhynMoreGossipClicked(void)
{
    D2UnitStrc *pNpc;

    if (NPCMENU_NPC_GetActiveNpcUnit())
    {
        pNpc = NPCMENU_NPC_GetActiveNpcUnit();
        TEXT_ShowSpeechPopup(pNpc, (int16_t)STR_IDX_254_JerhynActIntroMoreGossip1, 0);
        sgUIState.bNpcSpeechActive = 1;
        TEXT_SetCallback((void *)NPCMENU_OnSpeechDismissed, 0);
        DIALOG_Close(sgUIState.hDialogTalkMenu);
        sgUIState.hDialogTalkMenu = NULL;
    }
    return 1;
}

//1.10f: D2Client.0x6FAF4DB0
int32_t __cdecl NPCMENU_OnCainCubeGossipClicked(void)
{
    D2UnitStrc *pNpc;

    if (NPCMENU_NPC_GetActiveNpcUnit())
    {
        pNpc = NPCMENU_NPC_GetActiveNpcUnit();
        TEXT_ShowSpeechPopup(pNpc, (int16_t)STR_IDX_338_A2Q2EarlyReturnCubeCain, 0);
        sgUIState.bNpcSpeechActive = 1;
        TEXT_SetCallback((void *)NPCMENU_OnSpeechDismissed, 0);
        DIALOG_Close(sgUIState.hDialogTalkMenu);
        sgUIState.hDialogTalkMenu = NULL;
    }
    return 1;
}

//1.10f: D2Client.0x6FAF45E0
//  The chain. With a speech active: the follow-up line out of gNpcSpeechPairs if the line just shown opens a pair, else the first quest node
//  of type 1 or 2 that is neither that line nor one already played. Shown only if the NPC is still there; otherwise, and when nothing is left,
//  the talk menu comes back for the NPC's entry -- entry 0 on a miss. The original inlines NPCMENU_FindEntryIndexForNpc in that search and
//  NPCMENU_SpeechNotYetPlayed in the played scan. pArg goes on to TEXT_SetCallback as the context; the other two arguments are unread.
//  __fastcall with one stack argument (retn 4); the pair table is strided 8 bytes.
int32_t __fastcall NPCMENU_OnSpeechDismissed(void *pArg, int32_t bDismissed, int32_t bByMouse)
{
    uint16_t wShown;
    int16_t wNext;
    D2TextHeaderStrc *pTextHeader;
    int32_t nPair;
    int32_t nNode;
    int32_t nEntry;
    D2UnitStrc *pNpc;

    if (sgUIState.bNpcSpeechActive == 1)
    {
        wShown = sgUIState.wNpcShownSpeechTblId;
        for (nPair = 0; nPair < (int32_t)gnNpcSpeechPairCount; ++nPair)
        {
            if (gNpcSpeechPairs[nPair].wShownSpeechId == wShown)
            {
                wNext = (int16_t)gNpcSpeechPairs[nPair].wNextSpeechId;
                goto show_next;
            }
        }
        pTextHeader = TEXT_GetMessageListHead();
        nNode = 0;
        for (wNext = D2COMMON_TEXT_GetStringIdOfMenuType1Or2Node(pTextHeader, nNode); wNext != -1; wNext = D2COMMON_TEXT_GetStringIdOfMenuType1Or2Node(pTextHeader, ++nNode))
        {
            if (wNext != (int16_t)wShown && NPCMENU_SpeechNotYetPlayed(wNext))
                goto show_next;
        }
        goto rebuild_menu;

    show_next:
        if (wNext != -1 && NPCMENU_NPC_GetActiveNpcUnit())
        {
            sgUIState.wNpcShownSpeechTblId = (uint16_t)wNext;
            pNpc = NPCMENU_NPC_GetActiveNpcUnit();
            TEXT_ShowSpeechPopup(pNpc, wNext, 0);
            TEXT_SetCallback((void *)NPCMENU_OnSpeechDismissed, 0);
            return 1;
        }
    }

rebuild_menu:
    sgUIState.bNpcSpeechActive = 0;
    TEXT_SetCallback(NULL, (int32_t)pArg);
    sgUIState.CurrentState = 1;
    nEntry = NPCMENU_FindEntryIndexForNpc();
    sgUIState.pNpcSpeechDesc = &gNpcMenuEntries[nEntry];
    NPCMENU_BuildTalkMenu(0, STR_IDX_3381_TalkMenu, nEntry);
    return 1;
}

//  The talk menu and the speech queue. OnTalkClicked opens the talk menu for the NPC's gNpcMenuEntries record; its rows are the speech popups
//  plus PlayClassGreeting. ChooseNextSpeech is the other popup chain -- the one NPCMENU_BeginInteraction and the interrupt dialog's news row
//  arm -- which plays queued lines, sends client command 0x31 for each and, when they run out, opens the main menu or ends the conversation.
//  The original inlines NPCMENU_FindEntryIndexForNpc in OnTalkClicked's search (its NPC-gone arm is ShowSpeechLine's), NPCMENU_IsSpeechOnlyNpc in
//  ChooseNextSpeech's class chain, and NPCMENU_RerollAllSpeechLines in PlayClassGreeting's malah arm (its other arms call it). PlayClassGreeting's
//  four-statement teardowns are NPCMENU_NpcCloseInteraction's statements, and one arm calls NPCMENU_NpcCloseInteraction_Dup2 outright.
//  The follow-up search -- gNpcSpeechPairs first, then the first type 1 or 2 message node that is neither the key nor already played -- stands
//  in ChooseNextSpeech twice (keyed on gwNpcSpeechTblId and then on the line it has just shown) and in NPCMENU_BeginInteraction twice more.
//  The copies differ in nothing computed. No out-of-line copy exists, so it is written once as a static helper, NPCMENU_FindFollowUpSpeech.
//  OnSpeechDismissed's search answers the same question spelled differently (no second -1 test) and stays as read.
//  OnTalkClicked returns 0 or 1, and DIALOG_OnMouseUp reads the answer. BuildTalkMenu is __fastcall with its entry index on the stack
//  (retn 4); its first parameter is unread and both callers pass 0. PlayClassGreeting hard-codes three pairs -- 513 malah with a Barbarian (4)
//  line 15, 514 nihlathak with a Necromancer (2) line 11, 515 qual-kehk with a Paladin (3) line 10 -- then line 1 when the class matches the
//  entry's line 1 filter, else ShowSpeechLine(0, FALSE). Only the nihlathak arm inlines the NPC lookup for its popup; the rest call
//  NPCMENU_NPC_GetActiveNpcUnit. ChooseNextSpeech plays sounds 241 and 237 for lines 593 and 538 after the follow-up is stored, and walks
//  8-byte pair records.

static_assert(STR_IDX_3399_Intro == 0xD47, "BuildTalkMenu: mov ecx, 0D47h at 0x6FAF3FA5");
static_assert(STR_IDX_3395_NPCGossipMenu == 0xD43, "BuildTalkMenu: mov ecx, 0D43h at 0x6FAF3FCB");
static_assert(STR_IDX_3392_NPCTownMore0 == 0xD40, "BuildTalkMenu: mov ecx, 0D40h at 0x6FAF4032");
static_assert(STR_IDX_2231_box == 0x8B7, "BuildTalkMenu: mov ecx, 8B7h at 0x6FAF40C1");
static_assert(STR_IDX_3400_Back == 0xD48, "BuildTalkMenu: mov ecx, 0D48h at 0x6FAF40E5");
static_assert(MONSTER_JERHYN == 0xC9, "BuildTalkMenu: cmp eax, 0C9h at 0x6FAF401C");
static_assert(MONSTER_MALAH == 0x201, "PlayClassGreeting: sub eax, 201h at 0x6FAF479D");
static_assert(MONSTER_QUAL_KEHK == 515, "PlayClassGreeting: the second dec eax at 0x6FAF47AB");
static_assert(offsetof(D2NpcMenuDescStrc, bEnabled) == 0x26, "ChooseNextSpeech: gNpcMenuDescriptors+26h at 0x6FAF8D63");

//1.10f: D2Client.0x6FAF3C40
int32_t __cdecl NPCMENU_OnTalkClicked(void)
{
    int32_t nEntry;

    if (NPCMENU_NPC_GetActiveNpcUnit())
    {
        if (sgUIState.hDialogMainMenu)
        {
            DIALOG_Close(sgUIState.hDialogMainMenu);
            sgUIState.hDialogMainMenu = NULL;
        }
        sgUIState.CurrentState = 1;
        nEntry = NPCMENU_FindEntryIndexForNpc();
        sgUIState.pNpcSpeechDesc = &gNpcMenuEntries[nEntry];
        NPCMENU_BuildTalkMenu(0, STR_IDX_3381_TalkMenu, nEntry);
        return 1;
    }
    NPCMENU_NpcCloseInteraction();
    return 0;
}

//1.10f: D2Client.0x6FAF3DF0
//  Placed above the NPC like the main menu: 150 pixels up in view coordinates, clamped to y >= 20. The assert names line 1847. The Horadric
//  Cube row looks for base code 'box ' among everything the player's inventory holds, and only for the five Cains.
void __fastcall NPCMENU_BuildTalkMenu(int32_t nUnused, D2C_StringIndices nTitleTblIdx, int32_t nMenuEntryIdx)
{
    D2CoordStrc tClientCoords;
    int32_t nViewLeft;
    int32_t nViewTop;
    int32_t nViewRight;
    int32_t nViewBottom;
    int32_t nOriginX;
    int32_t nOriginY;
    RECT rcView;
    D2UnitStrc *pNpc;
    D2TextHeaderStrc *pTextHeader;
    int32_t nNodeCount;
    int32_t i;
    D2UnitStrc *pPlayer;
    D2UnitStrc *pUnit;
    D2UnitStrc *pItem;

    if (sgUIState.hDialogTalkMenu)
    {
        FOG_DisplayAssert((uint32_t)"sgUIState.hDialogTalkMenu == NULL", (uint32_t)szSourceFile, 1847);
        exit(-1);
    }
    tClientCoords.nX = 0;
    tClientCoords.nY = 0;
    if (NPCMENU_NPC_GetActiveNpcUnit())
    {
        pNpc = NPCMENU_NPC_GetActiveNpcUnit();
        D2COMMON_UNITS_GetClientCoords(pNpc, &tClientCoords);
        VIEW_GetRect(gpView, &nViewLeft, &nViewTop, &nViewRight, &nViewBottom);
        VIEW_GetOrigin(gpView, &nOriginX, &nOriginY);
        SetRect(&rcView, nOriginX, nOriginY, nViewRight - nViewLeft + nOriginX, nOriginY - nViewTop + nViewBottom);
        tClientCoords.nY += -150 - rcView.top;
        tClientCoords.nX -= rcView.left;
        if (tClientCoords.nY < 20)
            tClientCoords.nY = 20;
    }
    sgUIState.hDialogTalkMenu = DIALOG_Create(tClientCoords.nX, tClientCoords.nY, (DIALOGCALLBACK)NPCMENU_OnTalkMenuCancel, (DIALOGCALLBACK)NPCMENU_OnTalkMenuClickAway, 0, 0, 1, 0, 0, 0, 1);
    DIALOG_SetHighlight(sgUIState.hDialogTalkMenu, 1);
    DIALOG_AddLine(sgUIState.hDialogTalkMenu, D2LANG_GetStringFromTblIndex(nTitleTblIdx), 21, 0, 4, 1, NULL, 0);
    pTextHeader = TEXT_GetMessageListHead();
    if (pTextHeader)
    {
        nNodeCount = D2COMMON_TEXT_GetMenuType2NodeCount(pTextHeader);
        sgUIState.nTalkMenuBaseEntryIdx = nMenuEntryIdx;
        if (!gNpcMenuEntries[nMenuEntryIdx].bIntroHeard)
        {
            DIALOG_AddLine(sgUIState.hDialogTalkMenu, D2LANG_GetStringFromTblIndex(STR_IDX_3399_Intro), 15, 0, 0, 1, (DIALOGCALLBACK)NPCMENU_PlayClassGreeting, 1);
        }
        DIALOG_AddLine(sgUIState.hDialogTalkMenu, D2LANG_GetStringFromTblIndex(STR_IDX_3395_NPCGossipMenu), 15, 0, 0, 1, (DIALOGCALLBACK)NPCMENU_ShowDescriptorSpeech, 1);
        for (i = 0; i < nNodeCount; ++i)
        {
            DIALOG_AddLine(sgUIState.hDialogTalkMenu, TEXT_QuestGetQuestMenuString((uint16_t)D2COMMON_TEXT_GetStringIdOfMenuType2Node(pTextHeader, i)), 15, 0, 0, 1, (DIALOGCALLBACK)NPCMENU_OnTalkLineClicked, 1);
        }
        if (sgUIState.NpcClassId == MONSTER_JERHYN)
        {
            DIALOG_AddLine(sgUIState.hDialogTalkMenu, D2LANG_GetStringFromTblIndex(STR_IDX_3392_NPCTownMore0), 15, 0, 0, 1, (DIALOGCALLBACK)NPCMENU_OnJerhynMoreGossipClicked, 1);
        }
        if (sgUIState.NpcClassId == MONSTER_CAIN5 || sgUIState.NpcClassId == MONSTER_CAIN2 || sgUIState.NpcClassId == MONSTER_CAIN3 || sgUIState.NpcClassId == MONSTER_CAIN6 || sgUIState.NpcClassId == MONSTER_CAIN4)
        {
            pPlayer = CUNIT_GetPlayer();
            if (pPlayer && pPlayer->pInventory)
            {
                for (pUnit = D2COMMON_INVENTORY_GetFirstItem(pPlayer->pInventory); pUnit; pUnit = D2COMMON_INVENTORY_GetNextItem(pUnit))
                {
                    pItem = D2COMMON_INVENTORY_UnitIsItem(pUnit);
                    //  'box ' read as a dword: the Horadric Cube.
                    if (pItem && D2COMMON_ITEMS_GetBaseCode(pItem) == 0x20786F62)
                    {
                        DIALOG_AddLine(sgUIState.hDialogTalkMenu, D2LANG_GetStringFromTblIndex(STR_IDX_2231_box), 15, 0, 0, 1, (DIALOGCALLBACK)NPCMENU_OnCainCubeGossipClicked, 1);
                        break;
                    }
                }
            }
        }
    }
    DIALOG_AddLine(sgUIState.hDialogTalkMenu, D2LANG_GetStringFromTblIndex(STR_IDX_3400_Back), 15, 0, 0, 1, (DIALOGCALLBACK)NPCMENU_OnTalkMenuCancel, 1);
}

//1.10f: D2Client.0x6FAF4760
//  The 'introduction' row. The player's class is read before the NPC test; a class id of -1 without a player matches no arm.
int32_t __cdecl NPCMENU_PlayClassGreeting(void)
{
    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    int32_t nPlayerClass = pPlayer ? (int32_t)pPlayer->dwClassId : -1;
    D2UnitStrc *pNpc;
    D2NpcMenuEntryStrc *pDesc;
    D2NpcSpeechLineStrc *pTable;
    int32_t nLine;

    if ((pNpc = NPCMENU_NPC_GetActiveNpcUnit()) != NULL)
    {
        switch ((int32_t)pNpc->dwClassId)
        {
        case MONSTER_MALAH:
            if (nPlayerClass != 4)              //  Barbarian
                break;
            if (!NPCMENU_NPC_GetActiveNpcUnit())
            {
                NPCMENU_NpcCloseInteraction();
                return 1;
            }
            sgUIState.bNpcSpeechActive = 1;
            TEXT_SetCallback((void *)NPCMENU_OnSpeechDismissed, 0);
            DIALOG_Close(sgUIState.hDialogTalkMenu);
            sgUIState.hDialogTalkMenu = NULL;
            (sgUIState.pNpcSpeechDesc)->bRerollLine = 1;
            if (!sgUIState.bNpcSpeechLinesRolled)
                NPCMENU_RerollAllSpeechLines();
            nLine = 15;
            goto show_line;

        case MONSTER_NIHLATHAK:
            if (nPlayerClass != 2)              //  Necromancer
                break;
            if (!NPCMENU_NPC_GetActiveNpcUnit())
            {
                NPCMENU_NpcCloseInteraction();
                return 1;
            }
            sgUIState.bNpcSpeechActive = 1;
            TEXT_SetCallback((void *)NPCMENU_OnSpeechDismissed, 0);
            DIALOG_Close(sgUIState.hDialogTalkMenu);
            sgUIState.hDialogTalkMenu = NULL;
            (sgUIState.pNpcSpeechDesc)->bRerollLine = 1;
            if (!sgUIState.bNpcSpeechLinesRolled)
                NPCMENU_RerollAllSpeechLines();
            pNpc = NPCMENU_NPC_GetActiveNpcUnit();
            TEXT_ShowSpeechPopup(pNpc, (int16_t)(sgUIState.pNpcSpeechDesc)->pSpeechLineTable[11 % (int32_t)(sgUIState.pNpcSpeechDesc)->nSpeechLineCount].nTblStringId, 0);
            return 1;

        case MONSTER_QUAL_KEHK:
            if (nPlayerClass != 3)              //  Paladin
                break;
            if (!NPCMENU_NPC_GetActiveNpcUnit())
            {
                NPCMENU_NpcCloseInteraction();
                return 1;
            }
            sgUIState.bNpcSpeechActive = 1;
            TEXT_SetCallback((void *)NPCMENU_OnSpeechDismissed, 0);
            DIALOG_Close(sgUIState.hDialogTalkMenu);
            sgUIState.hDialogTalkMenu = NULL;
            (sgUIState.pNpcSpeechDesc)->bRerollLine = 1;
            if (!sgUIState.bNpcSpeechLinesRolled)
                NPCMENU_RerollAllSpeechLines();
            nLine = 10;
            goto show_line;
        }
    }

    if (nPlayerClass != (sgUIState.pNpcSpeechDesc)->pSpeechLineTable[1].nClassFilter)
    {
        NPCMENU_ShowSpeechLine(0, FALSE);
        return 1;
    }
    if (!NPCMENU_NPC_GetActiveNpcUnit())
    {
        NPCMENU_NpcCloseInteraction_Dup2();
        return 1;
    }
    sgUIState.bNpcSpeechActive = 1;
    TEXT_SetCallback((void *)NPCMENU_OnSpeechDismissed, 0);
    DIALOG_Close(sgUIState.hDialogTalkMenu);
    sgUIState.hDialogTalkMenu = NULL;
    (sgUIState.pNpcSpeechDesc)->bRerollLine = 1;
    if (!sgUIState.bNpcSpeechLinesRolled)
        NPCMENU_RerollAllSpeechLines();
    nLine = 1;

show_line:
    pDesc = sgUIState.pNpcSpeechDesc;
    nLine = nLine % (int32_t)pDesc->nSpeechLineCount;
    pTable = pDesc->pSpeechLineTable;
    pNpc = NPCMENU_NPC_GetActiveNpcUnit();
    TEXT_ShowSpeechPopup(pNpc, (int16_t)pTable[nLine].nTblStringId, 0);
    return 1;
}

//  No address: the follow-up search ChooseNextSpeech carries twice and NPCMENU_BeginInteraction twice more. The pair table first; then the
//  first type 1 or 2 node that is neither the key nor already played; else 0xFFFF.
static uint16_t NPCMENU_FindFollowUpSpeech(uint16_t wKey)
{
    int32_t nPair;
    int32_t nNode;
    D2TextHeaderStrc *pTextHeader;
    int16_t wId;

    for (nPair = 0; nPair < (int32_t)gnNpcSpeechPairCount; ++nPair)
    {
        if (gNpcSpeechPairs[nPair].wShownSpeechId == wKey)
            return gNpcSpeechPairs[nPair].wNextSpeechId;
    }
    pTextHeader = TEXT_GetMessageListHead();
    nNode = 0;
    for (wId = D2COMMON_TEXT_GetStringIdOfMenuType1Or2Node(pTextHeader, nNode); wId != -1; wId = D2COMMON_TEXT_GetStringIdOfMenuType1Or2Node(pTextHeader, ++nNode))
    {
        if (wId != (int16_t)wKey && wId != -1 && NPCMENU_SpeechNotYetPlayed(wId))
            return (uint16_t)wId;
    }
    return 0xFFFF;
}

//1.10f: D2Client.0x6FAF89E0
//  The queue. Choose the follow-up of the line that just played; sound for lines 593 and 538; then, with no speech active, the interrupt
//  dialog; with a follow-up and the NPC present, play it, record it, re-arm, send 0x31 and choose the next; otherwise open the main menu --
//  unless the popup went by mouse, the NPC is speech-only, or its descriptor is missing or disabled, in which case the conversation ends.
int32_t __fastcall NPCMENU_ChooseNextSpeech(int32_t nContext, int32_t bDismissed, int32_t bDismissedByMouse)
{
    D2UnitStrc *pNpc;
    int32_t nDescriptor;
    int32_t i;

    sgUIState.wNpcNextSpeechTblId = NPCMENU_FindFollowUpSpeech(sgUIState.wNpcSpeechTblId);
    if (sgUIState.wNpcSpeechTblId == 593)
        SOUNDINST_PlaySound(SOUND_ITEM_RING, NULL, 0, 0, 0);
    if (sgUIState.wNpcSpeechTblId == 538)
        SOUNDINST_PlaySound(SOUND_ITEM_POTION, NULL, 0, 0, 0);

    if (sgUIState.bNpcSpeechActive != 1)
    {
        sgUIState.bNpcSpeechActive = 0;
        TEXT_SetCallback(NULL, nContext);
        sgUIState.wNpcSpeechTblId = 0xFFFF;
        NPCMENU_BuildInterruptDialog();
        return 0;
    }

    if (sgUIState.wNpcNextSpeechTblId != 0xFFFF && NPCMENU_NPC_GetActiveNpcUnit())
    {
        sgUIState.wNpcShownSpeechTblId = sgUIState.wNpcNextSpeechTblId;
        pNpc = NPCMENU_NPC_GetActiveNpcUnit();
        TEXT_ShowSpeechPopup(pNpc, (int16_t)sgUIState.wNpcShownSpeechTblId, 1);
        if (sgUIState.nNpcSpeechesPlayed < 9)
        {
            sgUIState.wNpcSpeechesPlayed[sgUIState.nNpcSpeechesPlayed] = (int16_t)sgUIState.wNpcShownSpeechTblId;
            ++sgUIState.nNpcSpeechesPlayed;
        }
        TEXT_SetCallback((void *)NPCMENU_ChooseNextSpeech, 0);
        sgUIState.wNpcSpeechTblId = sgUIState.wNpcShownSpeechTblId;
        MSG_SendCmdUnit(0x31, (int32_t)sgUIState.NpcUnitGUID, (int32_t)sgUIState.wNpcShownSpeechTblId);
        sgUIState.wNpcNextSpeechTblId = NPCMENU_FindFollowUpSpeech(sgUIState.wNpcShownSpeechTblId);
        return 1;
    }

    sgUIState.bNpcSpeechActive = 0;
    TEXT_SetCallback(NULL, 0);
    if ((pNpc = NPCMENU_NPC_GetActiveNpcUnit()) != NULL)
    {
        nDescriptor = 0;
        for (i = 0; i < gnNpcMenuDescriptorCount; ++i)
        {
            if (gNpcMenuDescriptors[i].nMonsterId == (int32_t)pNpc->dwClassId)
            {
                nDescriptor = i;
                break;
            }
        }
    }
    else
    {
        nDescriptor = -1;
    }

    if ((bDismissed && bDismissedByMouse) || NPCMENU_IsSpeechOnlyNpc((D2C_MonsterIds)sgUIState.NpcClassId) || nDescriptor < 0 || !NPCMENU_NPC_GetActiveNpcUnit() || nDescriptor >= gnNpcMenuDescriptorCount || !gNpcMenuDescriptors[nDescriptor].bEnabled
        || (sgUIState.CurrentState = 1, !NPCMENU_NPC_GetActiveNpcUnit()))
    {
        NPCMENU_NpcCloseInteraction();
        return 0;
    }
    pNpc = NPCMENU_NPC_GetActiveNpcUnit();
    NPCMENU_OpenMainMenu(pNpc, 1);
    return 1;
}

//  The hire list. The 'hire' row opens a dialog with a list box in it: one row per offer the server has sent, "name - Lvl: n  Life: n  Def: n
//  Cost: n" in the first column and the hireling's description in the second. Clicking a row hires at once when the player has no mercenary,
//  living or waiting to be resurrected, and otherwise asks first through NPCMENU_BuildConfirmDialog.
//  The list box's four vtable slots used here: +00 LISTBOX_ProxyVecDelDtor (deleting destructor, 1 = free); +2C -> body +2C
//  LISTBOX_ImplGetSelection; +30 -> body +3C LISTBOX_ImplAddEntry; +34 LISTBOX_GetEntryUserData (entry[i].nUserData). AddEntry appends
//  (entries[wNumEntries], no ordering): six optional column pointers, the user data, a colour, bUseDefaultColor and a font.
//  BUG: the hire goes by the row's index, not by the offer number the row carries. INERT: the two are the same number exactly when the used
//  offers are a prefix of the ten, and they always are (NPCMENU_AddHirelingOffer takes the first free slot and every other writer of bUsed
//  clears all ten at once). DrawHireMercenaryList adds one row per used offer with the offer's number as its user data, or a single row with
//  user data 10 when there is none; OnHireListConfirmClicked bounds the selection (signed, < 10) and that row's user data (unsigned, < 10),
//  so the placeholder row (selection 0, user data 10) is turned away by the second bound.
//  The block "if the hire dialog is up: close it, free the control through slot 0 if there is one, null both" stands in all four hire-list
//  functions (twice in DrawHireMercenaryList, where both copies share the tail at 0x6FAF5010) and in NPCMENU_TeardownHireList. No function holds
//  it alone, so it is written out at each site as read.
//  DrawHireMercenaryList: wszFormat holds six words, the first stored by mov wszFormat.ch, 61h at 0x6FAF54AE: L"a0:%0", the format
//  LANG_FormatString is handed at 0x6FAF54E7 for the description. The gold sum is the "Your Gold: %d" title. The C declares wszFormat a plain
//  sixteen-bit array: the six words are stored with no construction loop, and a Unicode[6] would call D2Lang's constructor six times.
//  DIFFERENCE: the original's SMemAlloc + LISTBOX_Construct sits in an exception-handling state whose funclet frees the block if the
//  constructor throws; ours is allocate, test, construct, which differs only on a throw.
//  A failed MONSTERS_HirelingInit returns 0 from the middle of the build, with the dialog up, the rows so far in it and no SetSelectedLine.
//  As read. Name id 1057 is shown as tbl 11021, merca239 'Jarulf', the same substitution NPCMENU_BuildMainMenu makes for the resurrect row.
//  The cost's two trailing spaces go into the second column, ahead of the description; the other three pairs stay in the first.
//  DIFFERENCE: the original passes the offer's name id as mov ax, [eax] / push eax, so the dword's upper half is the upper half of the record's
//  address; ours passes the word zero-extended. Every reader takes sixteen bits (MONSTERS_GetActFromHirelingTxt declares uint16_t nNameId, and
//  DATATBLS_GetNextHirelingTxtRecordFromNameId reads mov dx, [esp+10h+nNameId]); the tbl id's mov cx, [ecx] is the same shape into D2Lang.

static_assert(STR_IDX_3364_ItemDesc1s == 0xD24, "DrawHireMercenaryList: mov ecx, 0D24h at 0x6FAF4F1B");
static_assert(STR_IDX_3365_ItemDesc1t == 0xD25, "DrawHireMercenaryList: mov ecx, 0D25h at 0x6FAF5570");
static_assert(STR_IDX_3366_HP == 0xD26, "DrawHireMercenaryList: mov ecx, 0D26h at 0x6FAF5287");
static_assert(STR_IDX_3367_AC == 0xD27, "DrawHireMercenaryList: mov ecx, 0D27h at 0x6FAF5329");
static_assert(STR_IDX_3368_Level == 0xD28, "DrawHireMercenaryList: mov ecx, 0D28h at 0x6FAF51E1");
static_assert(STR_IDX_3369_Cost == 0xD29, "DrawHireMercenaryList: mov ecx, 0D29h at 0x6FAF53CB");
static_assert(STR_IDX_3995_space == 0xF9B, "DrawHireMercenaryList: mov ecx, 0F9Bh at 0x6FAF515C");
static_assert(STR_IDX_3996_dash == 0xF9C, "DrawHireMercenaryList: mov ecx, 0F9Ch at 0x6FAF5171");
static_assert(STR_IDX_3997_colon == 0xF9D, "DrawHireMercenaryList: mov ecx, 0F9Dh at 0x6FAF5200");
static_assert(STR_IDX_11021_merca239 == 0x2B0D, "DrawHireMercenaryList: mov ecx, 2B0Dh at 0x6FAF5143");
static_assert(STR_IDX_3353_TransactionResults1 == 0xD19, "ConfirmHireMercenary: mov ecx, 0D19h at 0x6FAF5855");
static_assert(offsetof(D2NpcHirelingOfferStrc, nSeed) == 4, "DrawHireMercenaryList: mov ecx, [eax+4] at 0x6FAF5093");

//  The list box's slots, as byte offsets from the vtable pointer.
typedef void (__thiscall *HireListReleaseFn)(void *pThis, int32_t bFree);
typedef int32_t  (__thiscall *HireListGetSelectionFn)(void *pThis);
typedef void (__thiscall *HireListAddEntryFn)(void *pThis, const Unicode *pwsCol0, const Unicode *pwsCol1, const Unicode *pwsCol2, const Unicode *pwsCol3, const Unicode *pwsCol4, const Unicode *pwsCol5, int32_t nUserData,
                                              int32_t nColor, int32_t bUseDefaultColor, int32_t nFont);
typedef int32_t  (__thiscall *HireListGetUserDataFn)(void *pThis, int32_t nEntryIndex);

#define HIRELIST_VSLOT(fn, nByteOff) (*(fn *)((*(uint32_t *)(UINT_PTR)sgUIState.pHireListControl) + (nByteOff)))

//  Allocation tag for the list box: the original's own aNew.
static const char szNew[] = "new";

//1.10f: D2Client.0x6FAF4E30
int32_t __cdecl NPCMENU_DrawHireMercenaryList(void)
{
    void *pMemory;
    void *pControl;
    int32_t nGold;
    int32_t nRow;
    int32_t nAdded;
    int32_t nAct;
    int32_t nDifficulty;
    uint16_t wName;
    char szNumber[12];
    D2HirelingInitStrc tHirelingInit;

    if (sgUIState.hDialogMainMenu)
    {
        DIALOG_Close(sgUIState.hDialogMainMenu);
        sgUIState.hDialogMainMenu = NULL;
    }
    if (sgUIState.hDialogHireList)
    {
        DIALOG_Close(sgUIState.hDialogHireList);
        if (sgUIState.pHireListControl)
            HIRELIST_VSLOT(HireListReleaseFn, 0x00)(sgUIState.pHireListControl, 1);
        sgUIState.pHireListControl = 0;
        sgUIState.hDialogHireList = NULL;
        NPCMENU_BuildMainMenu();
        return 0;
    }

    sgUIState.hDialogHireList = DIALOG_Create((gnScreenWidth - 490) / 2, gnViewportBottom / 2 - 195, (DIALOGCALLBACK)NPCMENU_OnHireListBack, NULL, 0, 0, 0, 490, 350, 0, 1);
    DIALOG_SetHighlight(sgUIState.hDialogHireList, 1);

    Unicode wszText[300];
    nGold = D2COMMON_STATLIST_UnitGetStatValue(CUNIT_GetPlayer(), STAT_GOLDBANK, 0);
    nGold += D2COMMON_STATLIST_UnitGetStatValue(CUNIT_GetPlayer(), STAT_GOLD, 0);
    Unicode::sprintf(300, wszText, D2LANG_GetStringFromTblIndex(STR_IDX_3364_ItemDesc1s), nGold);
    DIALOG_AddLine(sgUIState.hDialogHireList, wszText, 21, 0, 4, 1, NULL, 0);
    DIALOG_AddLine(sgUIState.hDialogHireList, D2LANG_GetStringFromTblIndex(STR_IDX_3400_Back), 315, 0, 0, 1, (DIALOGCALLBACK)NPCMENU_OnHireListBack, 1);

    pMemory = STORM_SMemAlloc(12, szNew, -1, 0);
    pControl = pMemory ? LISTBOX_Construct(pMemory, 0, (gnScreenWidth - 490) / 2, gnViewportBottom / 2 - 160, 490, 280, 6, 0, 1, (void *)NPCMENU_OnHireListConfirmClicked, 0)
        : NULL;
    sgUIState.pHireListControl = pControl;
    if (!pControl)
    {
        if (sgUIState.hDialogHireList)
        {
            DIALOG_Close(sgUIState.hDialogHireList);
            if (sgUIState.pHireListControl)
                HIRELIST_VSLOT(HireListReleaseFn, 0x00)(sgUIState.pHireListControl, 1);
            sgUIState.pHireListControl = 0;
            sgUIState.hDialogHireList = NULL;
        }
        NPCMENU_BuildMainMenu();
        return 0;
    }
    DIALOG_AddControl(sgUIState.hDialogHireList, pControl, 35, 30);

    nAdded = 0;
    for (nRow = 0; nRow < 10; ++nRow)
    {
        if (!sgUIState.NpcHirelingOffers[nRow].bUsed)
            continue;

        nAct = D2COMMON_MONSTERS_GetActFromHirelingTxt(GAME_IsExpansionGame(), 0, (uint16_t)sgUIState.NpcHirelingOffers[nRow].wMercName);
        nDifficulty = (uint8_t)GAME_GetDifficulty();
        if (!D2COMMON_MONSTERS_HirelingInit(GAME_IsExpansionGame(), CUNIT_GetPlayer(), sgUIState.NpcHirelingOffers[nRow].nSeed, (D2C_Acts)nAct, (D2C_Difficulties)nDifficulty, &tHirelingInit))
        {
            return 0;
        }

        Unicode wszName[150];
        Unicode wszDetail[150];
        wName = (uint16_t)sgUIState.NpcHirelingOffers[nRow].wMercName;
        if (wName == 1057)
            wName = STR_IDX_11021_merca239;
        Unicode::strcpy(wszName, D2LANG_GetStringFromTblIndex((D2C_StringIndices)wName));
        Unicode::strcat(wszName, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        Unicode::strcat(wszName, D2LANG_GetStringFromTblIndex(STR_IDX_3996_dash));
        Unicode::strcat(wszName, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));

        Unicode wszLabel[20];
        Unicode wszNumber[10];

        Unicode::strncpy(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_3368_Level), 20);
        Unicode::strcat(wszName, wszLabel);
        Unicode::strcat(wszName, D2LANG_GetStringFromTblIndex(STR_IDX_3997_colon));
        Unicode::strcat(wszName, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        wsprintfA(szNumber, "%2u", tHirelingInit.nLevel);
        Unicode::win2Unicode(wszNumber, szNumber, 9);
        Unicode::strcat(wszName, wszNumber);
        Unicode::strcat(wszName, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        Unicode::strcat(wszName, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));

        Unicode::strncpy(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_3366_HP), 20);
        Unicode::strcat(wszName, wszLabel);
        Unicode::strcat(wszName, D2LANG_GetStringFromTblIndex(STR_IDX_3997_colon));
        Unicode::strcat(wszName, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        wsprintfA(szNumber, "%2u", tHirelingInit.nHitpoints);
        Unicode::win2Unicode(wszNumber, szNumber, 9);
        Unicode::strcat(wszName, wszNumber);
        Unicode::strcat(wszName, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        Unicode::strcat(wszName, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));

        Unicode::strncpy(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_3367_AC), 20);
        Unicode::strcat(wszName, wszLabel);
        Unicode::strcat(wszName, D2LANG_GetStringFromTblIndex(STR_IDX_3997_colon));
        Unicode::strcat(wszName, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        wsprintfA(szNumber, "%2u", tHirelingInit.nDefense);
        Unicode::win2Unicode(wszNumber, szNumber, 9);
        Unicode::strcat(wszName, wszNumber);
        Unicode::strcat(wszName, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        Unicode::strcat(wszName, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));

        Unicode::strncpy(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_3369_Cost), 20);
        Unicode::strcat(wszName, wszLabel);
        Unicode::strcat(wszName, D2LANG_GetStringFromTblIndex(STR_IDX_3997_colon));
        Unicode::strcat(wszName, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        wsprintfA(szNumber, "%2u", tHirelingInit.nGold);
        Unicode::win2Unicode(wszNumber, szNumber, 9);
        Unicode::strcat(wszName, wszNumber);
        Unicode::strcat(wszDetail, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
        Unicode::strcat(wszDetail, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));

        if (D2COMMON_MONSTERS_GetHirelingDescString(tHirelingInit.nHireDesc))
        {
            Unicode wszDesc[150];
            uint16_t wszFormat[6];      //  stored, never constructed

            wszFormat[0] = 'a';
            wszFormat[1] = '0';
            wszFormat[2] = ':';
            wszFormat[3] = '%';
            wszFormat[4] = '0';
            wszFormat[5] = 0;
            LANG_FormatString(wszDesc, (const Unicode *)wszFormat, D2COMMON_MONSTERS_GetHirelingDescString(tHirelingInit.nHireDesc), 0);
            Unicode::strcat(wszDetail, wszDesc);
        }

        HIRELIST_VSLOT(HireListAddEntryFn, 0x30)(sgUIState.pHireListControl, wszName, wszDetail, NULL, NULL, NULL, NULL, nRow, 0, 1, 14);
        ++nAdded;
    }

    if (!nAdded)
    {
        HIRELIST_VSLOT(HireListAddEntryFn, 0x30)(sgUIState.pHireListControl, D2LANG_GetStringFromTblIndex(STR_IDX_3365_ItemDesc1t), NULL, NULL, NULL, NULL, NULL, 10, 0, 1, 14);
    }
    DIALOG_SetSelectedLine(sgUIState.hDialogHireList, -1);
    return 1;
}

//1.10f: D2Client.0x6FAF55B0
//  The dialog's dismiss and its Back row both.
int32_t __stdcall NPCMENU_OnHireListBack(void)
{
    if (sgUIState.hDialogHireList)
    {
        DIALOG_Close(sgUIState.hDialogHireList);
        if (sgUIState.pHireListControl)
            HIRELIST_VSLOT(HireListReleaseFn, 0x00)(sgUIState.pHireListControl, 1);
        sgUIState.pHireListControl = 0;
        sgUIState.hDialogHireList = NULL;
    }
    NPCMENU_BuildMainMenu();
    return 1;
}

//1.10f: D2Client.0x6FAF55F0
//  The selection and the NPC's GUID are stored before either bound is tested. Pet type 7 is the mercenary.
int32_t __cdecl NPCMENU_OnHireListConfirmClicked(void)
{
    int32_t nSelection;

    SOUNDINST_PlaySound(SOUND_CURSOR_SELECT, NULL, 0, 0, 0);
    nSelection = HIRELIST_VSLOT(HireListGetSelectionFn, 0x2C)(sgUIState.pHireListControl);
    gnSelectedHirelingRow = nSelection;
    gnHireNpcUnitGUID = (int32_t)sgUIState.NpcUnitGUID;
    if (nSelection < 10 && (uint32_t)HIRELIST_VSLOT(HireListGetUserDataFn, 0x34)(sgUIState.pHireListControl, nSelection) < 10)
    {
        if (ROSTERPETS_GetAssociatedUnitGUID(CUNIT_GetPlayer(), 7, 0) == -1 && (!GAME_IsExpansionGame() || (!ROSTERPETS_CountByType(CUNIT_GetPlayer(), 7) && (uint16_t)gwMercResurrectNameTblId == 0xFFFF)))
        {
            NPCMENU_ConfirmHireMercenary();
            return 0;
        }
        if (sgUIState.hDialogHireList)
        {
            DIALOG_Close(sgUIState.hDialogHireList);
            if (sgUIState.pHireListControl)
                HIRELIST_VSLOT(HireListReleaseFn, 0x00)(sgUIState.pHireListControl, 1);
            sgUIState.pHireListControl = 0;
            sgUIState.hDialogHireList = NULL;
        }
        sgUIState.Transaction = 5;
        NPCMENU_BuildConfirmDialog();
    }
    return 0;
}

//1.10f: D2Client.0x6FAF56D0
//  Client command 0x36: the NPC, and the name id of the offer in the selected row. Puts the waiting box 150 pixels above the NPC, as
//  BuildTalkMenu places its menu, and arms the 60-second transaction timeout.
void __cdecl NPCMENU_ConfirmHireMercenary(void)
{
    D2CoordStrc tClientCoords;
    int32_t nViewLeft;
    int32_t nViewTop;
    int32_t nViewRight;
    int32_t nViewBottom;
    int32_t nOriginX;
    int32_t nOriginY;
    RECT rcView;
    D2UnitStrc *pNpc;

    MSG_SendCmdUnit(0x36, gnHireNpcUnitGUID, (uint16_t)sgUIState.NpcHirelingOffers[gnSelectedHirelingRow].wMercName);
    if (sgUIState.hDialogHireList)
    {
        DIALOG_Close(sgUIState.hDialogHireList);
        if (sgUIState.pHireListControl)
            HIRELIST_VSLOT(HireListReleaseFn, 0x00)(sgUIState.pHireListControl, 1);
        sgUIState.pHireListControl = 0;
        sgUIState.hDialogHireList = NULL;
    }
    sgUIState.CurrentState = 10;
    tClientCoords.nX = 0;
    tClientCoords.nY = 0;
    if (NPCMENU_NPC_GetActiveNpcUnit())
    {
        pNpc = NPCMENU_NPC_GetActiveNpcUnit();
        D2COMMON_UNITS_GetClientCoords(pNpc, &tClientCoords);
        VIEW_GetRect(gpView, &nViewLeft, &nViewTop, &nViewRight, &nViewBottom);
        VIEW_GetOrigin(gpView, &nOriginX, &nOriginY);
        SetRect(&rcView, nOriginX, nOriginY, nViewRight - nViewLeft + nOriginX, nOriginY - nViewTop + nViewBottom);
        tClientCoords.nY += -150 - rcView.top;
        tClientCoords.nX -= rcView.left;
        if (tClientCoords.nY < 20)
            tClientCoords.nY = 20;
    }
    sgUIState.hDialogWaiting = DIALOG_Create(tClientCoords.nX, tClientCoords.nY, (DIALOGCALLBACK)USEITEM_NullCallback, NULL, 0, 0, 1, 0, 0, 0, 1);
    sgUIState.nTransactionTimeoutTick = GetTickCount() + 60000;
    sgUIState.nNextItemActionTick = 0;
    DIALOG_SetHighlight(sgUIState.hDialogWaiting, 0);
    DIALOG_AddLine(sgUIState.hDialogWaiting, D2LANG_GetStringFromTblIndex(STR_IDX_3353_TransactionResults1), 15, 0, 0, 1, NULL, 0);
}

//  A descriptor's row k, addressed by byte offset from the record as the original does, so that the insert's slot -1 and slot 5, which the
//  shipped table never reaches, are defined C.
#define NPCDESC_HANDLER(pRec, k) (*(void **)((uint8_t *)(pRec) + 18 + 4 * (k)))
#define NPCDESC_TBLIDX(pRec, k)  (*(uint16_t *)((uint8_t *)(pRec) + 8 + 2 * (k)))

//1.10f: D2Client.0x6FAF9CB0
//  The resurrect row: handler NPCMENU_OnResurrectMercClicked, label 5383, which NPCMENU_BuildMainMenu replaces with "Resurrect %s: %d".
//  Inserted in front of the hire row, else in front of the last row. The descriptor stride is 39 bytes.
//  BUG: the insert writes one slot past the record's rows at nRows 0 or 5. UNREACHABLE: the shipped table never has such a record.
int32_t __fastcall NPCMENU_ToggleResurrectMercRow(int32_t nDescriptorIndex, int32_t bAdd)
{
    D2NpcMenuDescStrc *pRec;
    int32_t nRow;
    int32_t nHireRow;
    int32_t nLast;

    if (!FOG_IsExpansion() || !GAME_IsExpansionGame())
        return 0;
    pRec = &gNpcMenuDescriptors[nDescriptorIndex];

    if (bAdd)
    {
        for (nRow = 0; nRow < pRec->nRows; ++nRow)
        {
            if (NPCDESC_HANDLER(pRec, nRow) == (void *)NPCMENU_OnResurrectMercClicked)
                return 1;
        }
        for (nHireRow = 0; nHireRow < pRec->nRows; ++nHireRow)
        {
            if (NPCDESC_HANDLER(pRec, nHireRow) == (void *)NPCMENU_DrawHireMercenaryList)
            {
                for (nRow = pRec->nRows; nRow > nHireRow; --nRow)
                {
                    NPCDESC_HANDLER(pRec, nRow) = NPCDESC_HANDLER(pRec, nRow - 1);
                    NPCDESC_TBLIDX(pRec, nRow) = NPCDESC_TBLIDX(pRec, nRow - 1);
                }
                ++pRec->nRows;
                NPCDESC_HANDLER(pRec, nHireRow) = (void *)NPCMENU_OnResurrectMercClicked;
                NPCDESC_TBLIDX(pRec, nHireRow) = 5383;
                return 1;
            }
        }
        nLast = pRec->nRows - 1;
        NPCDESC_HANDLER(pRec, nLast + 1) = NPCDESC_HANDLER(pRec, nLast);
        NPCDESC_TBLIDX(pRec, nLast + 1) = NPCDESC_TBLIDX(pRec, nLast);
        ++pRec->nRows;
        NPCDESC_HANDLER(pRec, nLast) = (void *)NPCMENU_OnResurrectMercClicked;
        NPCDESC_TBLIDX(pRec, nLast) = 5383;
        return 1;
    }

    for (nRow = 0; nRow < pRec->nRows; ++nRow)
    {
        if (NPCDESC_HANDLER(pRec, nRow) == (void *)NPCMENU_OnResurrectMercClicked)
        {
            while (nRow < pRec->nRows - 1)
            {
                NPCDESC_HANDLER(pRec, nRow) = NPCDESC_HANDLER(pRec, nRow + 1);
                NPCDESC_TBLIDX(pRec, nRow) = NPCDESC_TBLIDX(pRec, nRow + 1);
                ++nRow;
            }
            --pRec->nRows;
            return 1;
        }
    }
    return 1;
}

//  The transaction's wait and answer. A transaction leaves the client in one of three ways -- a buy, sell or repair through
//  SendTransactionAndWait, "identify everything" through the Cains' row, a resurrection through the resurrect row -- and all three put the same
//  waiting box up above the NPC and arm the same sixty-second deadline, gnTransactionTimeoutTick, which NPCMENU_TickAndValidate enforces. The
//  server answers with packet 0x2A; OnTransactionResult reads it in UI state 5, a trade, and hands it to ShowTransactionResult in state 10,
//  the two menu rows.
//  The commands:
//      0x32 buy        MSG_SendCmd4Dwords  NPC, item, packed mode, cost
//      0x33 sell       MSG_SendCmd4Dwords  NPC, item, anim mode, cost
//      0x35 repair     MSG_SendCmd4Dwords  NPC, item, anim mode, durability -- 0x80000000 for repair-all
//      0x34 identify   MSG_SendServerItemMessage  NPC
//      0x62 resurrect  MSG_SendServerItemMessage  NPC
//  The buy's third dword is the item's anim mode in the high word, bit 1 for a gamble, and bit 31 when shift is held over an item whose
//  ItemsTxt nMultibuy byte is set. Shift is the second argument's MK_SHIFT: BeginTransaction's callers hand on the mouse message's wParam, or 0.
//  OnResurrectMercClicked is the resurrect row's handler (label 5383, which NPCMENU_BuildMainMenu turns into 'Resurrect %s: %d') and sends
//  packet 0x62. ShowTransactionResult sets five row counts to 2, which drops Identify Items, because the Cain records ship with 3 and
//  BuildMainMenu shows nRows - 1 rows before its 'cancel'.
//  Written out rather than as a call:
//   * the waiting box: the NPC position, DIALOG_Create with USEITEM_NullCallback, the deadline and tbl 3353 are the same
//     instructions in SendTransactionAndWait, OnIdentifyClicked, OnResurrectMercClicked and ConfirmHireMercenary, and nothing holds them alone.
//     The free-box assert beside them is in two of the four, at two lines (4352 and 2491), so it is not one function's either;
//   * the result strings: the switch from result code to tbl id is in both result functions, over two jump tables, with no copy of its own.
//  OnTransactionResult re-tests CurrentState == 5 after the switch and asserts "0" at line 4853 if it fails, then asserts the free result box at
//  4833, a lower line, after it. A line number running backwards inside one body is an inlined callee's; which callee is not established, and
//  the asserts are written in the order the bytes run. Code 0 waits for the item message when the item is not on the client yet, code 1 while it
//  still is; the item-message acknowledgements are the other end of those two returns.

static_assert(STR_IDX_3356_TransactionResults4 == 0xD1C, "OnTransactionResult: mov esi, 0D1Ch at 0x6FAF5A01");
static_assert(STR_IDX_3357_TransactionResults5 == 0xD1D, "OnTransactionResult: mov esi, 0D1Dh at 0x6FAF59F3");
static_assert(STR_IDX_3358_TransactionResults6 == 0xD1E, "OnTransactionResult: mov esi, 0D1Eh at 0x6FAF5A0F");
static_assert(STR_IDX_3360_TransactionResults8 == 0xD20, "OnTransactionResult: mov esi, 0D20h at 0x6FAF5A08");
static_assert(STR_IDX_3361_TransactionResults9 == 0xD21, "OnTransactionResult: mov esi, 0D21h at 0x6FAF5A16");
static_assert(STR_IDX_3362_TransactionResults10 == 0xD22, "OnTransactionResult: mov esi, 0D22h at 0x6FAF59FA");
static_assert(STR_IDX_3363_TransactionResults11 == 0xD23, "OnTransactionResult: mov esi, 0D23h at 0x6FAF59EC");
static_assert(STR_IDX_3403_Continue == 0xD4B, "OnTransactionResult: mov ecx, 0D4Bh at 0x6FAF5ADF");
static_assert(MONSTER_CAIN2 == 0xF4 && MONSTER_CAIN3 == 0xF5 && MONSTER_CAIN4 == 0xF6,
              "ShowTransactionResult: cmp eax, 0F4h / jb and cmp eax, 0F6h / jbe at 0x6FAFB18F and 0x6FAFB196");
static_assert(MONSTER_CAIN5 == 0x109 && MONSTER_CAIN6 == 0x208,
              "ShowTransactionResult: cmp eax, 109h at 0x6FAFB186, cmp eax, 208h at 0x6FAFB1A3");
static_assert(STAT_DURABILITY == 0x48, "SendTransactionAndWait: push 48h at 0x6FAFA89C");
static_assert(offsetof(D2ItemsTxt, nMultibuy) == 0x1A5, "SendTransactionAndWait: mov cl, [eax+1A5h] at 0x6FAFA838");
static_assert(MK_SHIFT == 4, "SendTransactionAndWait: test byte ptr [esp+40h+yTop], 4 at 0x6FAFA81B");

//1.10f: D2Client.0x6FAFA6E0
//  The command, then the box. The first argument (bSkippedVerify) does not decide whether anything is sent: every path past the two early
//  returns sends. It only picks INV_NpcShopEnterPanelMode6 (1, from BeginTransaction when no confirm dialog was shown) or
//  INV_NpcShopLeaveVerifyMode (0, from the dialog's Yes), which asserts inventory mode 5, BUYSELL_VERIFY. The second is nMouseKeyFlags.
//  BUG: the two NULL-item arms are dead. INERT: the only writers of gbTransactionItemIsPlayers are INV_NpcShopOnLButtonUp's 1 and 0 around its
//  repair-all call, so INV_GetTransactionItemIsPlayers answers 1 only inside a repair-all, and anywhere else a missing item has already left
//  through the "buy cancelled" trace. Transcribed with their guards.
//  The same flag gates the two-second repeat guard on gnLastTransactionSendTick, so only a repair-all is throttled, and a throttled one
//  returns with gbTransactionPending still set and sound 15 already played.
void __fastcall NPCMENU_SendTransactionAndWait(int32_t bSkippedVerify, int32_t nMouseKeyFlags)
{
    uint8_t nCmd;
    D2UnitStrc *pItem;
    uint32_t dwAnimMode;
    uint32_t dwPackedMode;
    D2ItemsTxt *pItemsTxt;
    D2CoordStrc tClientCoords;
    int32_t nViewLeft;
    int32_t nViewTop;
    int32_t nViewRight;
    int32_t nViewBottom;
    int32_t nOriginX;
    int32_t nOriginY;
    RECT rcView;
    D2UnitStrc *pNpc;

    if (!gbTransactionPending)
    {
        FOG_Trace("Error at line %d of file %s\n", 4187, szSourceFile);
        INV_LeavePanelMode5();
        return;
    }
    switch (sgUIState.Transaction)
    {
    case 1:
        nCmd = 0x32;
        break;
    case 2:
        nCmd = 0x33;
        break;
    case 3:
        nCmd = 0x35;
        SOUNDINST_PlaySound(SOUND_CURSOR_REPAIR_ITEM, NULL, 0, 0, 0);
        break;
    default:
        sgUIState.CurrentState = 3;
        INV_LeavePanelMode5();
        return;
    }

    pItem = NULL;
    dwAnimMode = 0;
    sgUIState.bWaitingForItemMsg = 0;
    if (INV_GetTransactionItemIsPlayers())
    {
        if (GetTickCount() - gnLastTransactionSendTick < 2000)
        {
            INV_LeavePanelMode5();
            return;
        }
        gnLastTransactionSendTick = GetTickCount();
    }
    else
    {
        pItem = CUNIT_FindUnitByGuidAndType(gTransactionItemGUID, UNIT_ITEM);
        if (!pItem)
        {
            sgUIState.CurrentState = 3;
            INV_TradeRestoreHeldItem();
            FOG_Trace("buy cancelled for non existant item?");
            gnLastItemActionTick = GetTickCount();
            ++gnItemActionBurstCount;
            gbTransactionPending = 0;
            return;
        }
        dwAnimMode = pItem->dwAnimMode;
        if (sgUIState.Transaction == 2)
        {
            sgUIState.bTransactionItemWasOnCursor = D2COMMON_INVENTORY_GetCursorItem(gpNpcTransactionPlayer->pInventory) == pItem;
        }
    }

    if (bSkippedVerify)
        INV_NpcShopEnterPanelMode6();
    else
        INV_NpcShopLeaveVerifyMode();
    sgUIState.CurrentState = 5;
    gLastTransactionTick = GetTickCount();

    if (nCmd == 0x32)
    {
        dwPackedMode = (dwAnimMode & 0xFFFF) << 16;
        if (sgUIState.bNpcVendorGamble)
            dwPackedMode |= 2;
        if (nMouseKeyFlags & MK_SHIFT)
        {
            pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(pItem ? (int32_t)pItem->dwClassId : -1);
            if (pItemsTxt && pItemsTxt->nMultibuy)
                dwPackedMode |= 0x80000000;
        }
        MSG_SendCmd4Dwords(0x32, (int32_t)sgUIState.NpcUnitGUID, (int32_t)gTransactionItemGUID, (int32_t)dwPackedMode, sgUIState.TransactionCost);
        sgUIState.nTransactionResultSoundId = ITEM_GetDropSoundNoFrame(pItem);
    }
    else if (nCmd != 0x35)
    {
        MSG_SendCmd4Dwords(nCmd, (int32_t)sgUIState.NpcUnitGUID, (int32_t)gTransactionItemGUID, (int32_t)(dwAnimMode & 0xFFFF), sgUIState.TransactionCost);
    }
    else if (INV_GetTransactionItemIsPlayers())
    {
        MSG_SendCmd4Dwords(0x35, (int32_t)sgUIState.NpcUnitGUID, (int32_t)gTransactionItemGUID, (int32_t)(dwAnimMode & 0xFFFF), (int32_t)0x80000000);
    }
    else if (pItem)
    {
        MSG_SendCmd4Dwords(0x35, (int32_t)sgUIState.NpcUnitGUID, (int32_t)gTransactionItemGUID, (int32_t)(dwAnimMode & 0xFFFF), D2COMMON_STATLIST_UnitGetStatValue(pItem, STAT_DURABILITY, 0));
    }
    else
    {
        MSG_SendCmd4Dwords(0x35, (int32_t)sgUIState.NpcUnitGUID, (int32_t)gTransactionItemGUID, (int32_t)(dwAnimMode & 0xFFFF), 0);
    }

    if (sgUIState.hDialogWaiting)
    {
        FOG_DisplayAssert((uint32_t)"sgUIState.hDialogWaiting == NULL", (uint32_t)szSourceFile, 4352);
        exit(-1);
    }
    tClientCoords.nX = 0;
    tClientCoords.nY = 0;
    if (NPCMENU_NPC_GetActiveNpcUnit())
    {
        pNpc = NPCMENU_NPC_GetActiveNpcUnit();
        D2COMMON_UNITS_GetClientCoords(pNpc, &tClientCoords);
        VIEW_GetRect(gpView, &nViewLeft, &nViewTop, &nViewRight, &nViewBottom);
        VIEW_GetOrigin(gpView, &nOriginX, &nOriginY);
        SetRect(&rcView, nOriginX, nOriginY, nViewRight - nViewLeft + nOriginX, nOriginY - nViewTop + nViewBottom);
        tClientCoords.nY += -150 - rcView.top;
        tClientCoords.nX -= rcView.left;
        if (tClientCoords.nY < 20)
            tClientCoords.nY = 20;
    }
    sgUIState.hDialogWaiting = DIALOG_Create(tClientCoords.nX, tClientCoords.nY, (DIALOGCALLBACK)USEITEM_NullCallback, NULL, 0, 0, 1, 0, 0, 0, 1);
    sgUIState.nTransactionTimeoutTick = GetTickCount() + 60000;
    sgUIState.nNextItemActionTick = 0;
    DIALOG_SetHighlight(sgUIState.hDialogWaiting, 0);
    DIALOG_AddLine(sgUIState.hDialogWaiting, D2LANG_GetStringFromTblIndex(STR_IDX_3353_TransactionResults1), 15, 0, 0, 1, NULL, 0);
}

//1.10f: D2Client.0x6FAF5EB0
//  The Cains' 'Identify Items' row: close the menu, put the box up, then state 10, transaction 4 and item message 0x34 with the NPC; the three
//  stores are made before the send (0x6FAF6043..0x6FAF6057).
int32_t __cdecl NPCMENU_OnIdentifyClicked(void)
{
    D2CoordStrc tClientCoords;
    int32_t nViewLeft;
    int32_t nViewTop;
    int32_t nViewRight;
    int32_t nViewBottom;
    int32_t nOriginX;
    int32_t nOriginY;
    RECT rcView;
    D2UnitStrc *pNpc;

    if (sgUIState.hDialogMainMenu)
    {
        DIALOG_Close(sgUIState.hDialogMainMenu);
        sgUIState.hDialogMainMenu = NULL;
    }
    if (sgUIState.hDialogWaiting)
    {
        FOG_DisplayAssert((uint32_t)"sgUIState.hDialogWaiting == NULL", (uint32_t)szSourceFile, 2491);
        exit(-1);
    }
    tClientCoords.nX = 0;
    tClientCoords.nY = 0;
    if (NPCMENU_NPC_GetActiveNpcUnit())
    {
        pNpc = NPCMENU_NPC_GetActiveNpcUnit();
        D2COMMON_UNITS_GetClientCoords(pNpc, &tClientCoords);
        VIEW_GetRect(gpView, &nViewLeft, &nViewTop, &nViewRight, &nViewBottom);
        VIEW_GetOrigin(gpView, &nOriginX, &nOriginY);
        SetRect(&rcView, nOriginX, nOriginY, nViewRight - nViewLeft + nOriginX, nOriginY - nViewTop + nViewBottom);
        tClientCoords.nY += -150 - rcView.top;
        tClientCoords.nX -= rcView.left;
        if (tClientCoords.nY < 20)
            tClientCoords.nY = 20;
    }
    sgUIState.hDialogWaiting = DIALOG_Create(tClientCoords.nX, tClientCoords.nY, (DIALOGCALLBACK)USEITEM_NullCallback, NULL, 0, 0, 1, 0, 0, 0, 1);
    sgUIState.nTransactionTimeoutTick = GetTickCount() + 60000;
    sgUIState.nNextItemActionTick = 0;
    DIALOG_SetHighlight(sgUIState.hDialogWaiting, 0);
    DIALOG_AddLine(sgUIState.hDialogWaiting, D2LANG_GetStringFromTblIndex(STR_IDX_3353_TransactionResults1), 15, 0, 0, 1, NULL, 0);
    sgUIState.CurrentState = 10;
    sgUIState.Transaction = 4;
    sgUIState.bWaitingForItemMsg = 0;
    MSG_SendServerItemMessage(0x34, (int32_t)sgUIState.NpcUnitGUID);
    return 1;
}

//1.10f: D2Client.0x6FAF8E80
//  The resurrect row: close the menu, item message 0x62 with the NPC, state 10, and the box, which this one opens without asserting it is free.
int32_t __cdecl NPCMENU_OnResurrectMercClicked(void)
{
    D2CoordStrc tClientCoords;
    int32_t nViewLeft;
    int32_t nViewTop;
    int32_t nViewRight;
    int32_t nViewBottom;
    int32_t nOriginX;
    int32_t nOriginY;
    RECT rcView;
    D2UnitStrc *pNpc;

    if (sgUIState.hDialogMainMenu)
    {
        DIALOG_Close(sgUIState.hDialogMainMenu);
        sgUIState.hDialogMainMenu = NULL;
    }
    MSG_SendServerItemMessage(0x62, (int32_t)sgUIState.NpcUnitGUID);
    sgUIState.CurrentState = 10;
    tClientCoords.nX = 0;
    tClientCoords.nY = 0;
    if (NPCMENU_NPC_GetActiveNpcUnit())
    {
        pNpc = NPCMENU_NPC_GetActiveNpcUnit();
        D2COMMON_UNITS_GetClientCoords(pNpc, &tClientCoords);
        VIEW_GetRect(gpView, &nViewLeft, &nViewTop, &nViewRight, &nViewBottom);
        VIEW_GetOrigin(gpView, &nOriginX, &nOriginY);
        SetRect(&rcView, nOriginX, nOriginY, nViewRight - nViewLeft + nOriginX, nOriginY - nViewTop + nViewBottom);
        tClientCoords.nY += -150 - rcView.top;
        tClientCoords.nX -= rcView.left;
        if (tClientCoords.nY < 20)
            tClientCoords.nY = 20;
    }
    sgUIState.hDialogWaiting = DIALOG_Create(tClientCoords.nX, tClientCoords.nY, (DIALOGCALLBACK)USEITEM_NullCallback, NULL, 0, 0, 1, 0, 0, 0, 1);
    sgUIState.nTransactionTimeoutTick = GetTickCount() + 60000;
    sgUIState.nNextItemActionTick = 0;
    DIALOG_SetHighlight(sgUIState.hDialogWaiting, 0);
    DIALOG_AddLine(sgUIState.hDialogWaiting, D2LANG_GetStringFromTblIndex(STR_IDX_3353_TransactionResults1), 15, 0, 0, 1, NULL, 0);
    return 1;
}

//1.10f: D2Client.0x6FAF5880
//  Packet 0x2A's body: keep the packet, refresh the cached gold, and act by UI state -- 5 here, 10 in ShowTransactionResult, and anything else
//  only a sound for result 1, read off the packet rather than the copy.
void __fastcall NPCMENU_OnTransactionResult(D2GSPacketSrv2A *pPacket)
{
    D2InventoryStrc *pInventory;
    D2C_StringIndices nResultTblIdx;
    D2UnitStrc *pCursorItem;
    int32_t nMouseY;

    sgUIState.LastTransactionResult = *pPacket;
    sgUIState.nCachedPlayerGold = D2COMMON_STATLIST_UnitGetStatValue(CUNIT_GetPlayer(), STAT_GOLD, 0);
    if (sgUIState.CurrentState == 5)
    {
        pInventory = CUNIT_GetPlayer()->pInventory;
        if (sgUIState.LastTransactionResult.nResult <= 6)
        {
            switch (sgUIState.LastTransactionResult.nResult)
            {
            case 0:
                SOUNDINST_PlaySound(sgUIState.nTransactionResultSoundId, NULL, 0, 0, 0);
                sgUIState.nTransactionResultSoundId = SOUND_NONE;
                if (!CUNIT_FindUnitByGuidAndType(sgUIState.LastTransactionResult.dwItemGUID, UNIT_ITEM))
                {
                    sgUIState.bWaitingForItemMsg = 1;
                    return;
                }
                break;
            case 1:
                SOUNDINST_PlaySound(SOUND_ITEM_GOLD, NULL, 0, 0, 0);
                if (CUNIT_FindUnitByGuidAndType(sgUIState.LastTransactionResult.dwItemGUID, UNIT_ITEM))
                {
                    sgUIState.bWaitingForItemMsg = 1;
                    return;
                }
                break;
            case 5:
                HIREABLES_HireIconsResetAndShow();
                break;
            }
            sgUIState.bWaitingForItemMsg = 0;
            if (sgUIState.hDialogWaiting)
            {
                DIALOG_Close(sgUIState.hDialogWaiting);
                sgUIState.hDialogWaiting = NULL;
            }
            SELECT_ClearSelectedUnit();
            gbTransactionPending = 0;
            gTransactionItemClassId = 0;
            gTransactionItemGUID = 0;
            sgUIState.CurrentState = 3;
            INV_ResetItemGrids();
            INV_NpcShopApplyTabSelection();
            return;
        }

        if (sgUIState.hDialogWaiting)
        {
            DIALOG_Close(sgUIState.hDialogWaiting);
            sgUIState.hDialogWaiting = NULL;
        }
        switch (sgUIState.LastTransactionResult.nResult)
        {
        case 7:
        case 8:
            nResultTblIdx = STR_IDX_3358_TransactionResults6;
            break;
        case 10:
            nResultTblIdx = STR_IDX_3356_TransactionResults4;
            break;
        case 11:
            nResultTblIdx = STR_IDX_3360_TransactionResults8;
            break;
        case 12:
            nResultTblIdx = STR_IDX_3362_TransactionResults10;
            break;
        case 15:
            nResultTblIdx = STR_IDX_3363_TransactionResults11;
            break;
        case 16:
            nResultTblIdx = STR_IDX_3357_TransactionResults5;
            break;
        default:
            nResultTblIdx = STR_IDX_3361_TransactionResults9;
            break;
        }
        if (sgUIState.CurrentState != 5)
        {
            FOG_DisplayAssert((uint32_t)"0", (uint32_t)szSourceFile, 4853);
            exit(-1);
        }
        sgUIState.CurrentState = 6;
        INV_NpcShopEnterPanelMode7();
        if (sgUIState.hDialogResults)
        {
            FOG_DisplayAssert((uint32_t)"sgUIState.hDialogResults == NULL", (uint32_t)szSourceFile, 4833);
            exit(-1);
        }
        if (sgUIState.Transaction == 2)
        {
            pCursorItem = D2COMMON_INVENTORY_GetCursorItem(pInventory);
            if (sgUIState.bTransactionItemWasOnCursor == 1 && pCursorItem && !CURSOR_GetHeldUnit())
                CURSOR_SetHeldUnit(pCursorItem);
        }
        nMouseY = CURSOR_GetMouseY();
        sgUIState.hDialogResults = DIALOG_Create(CURSOR_GetMouseX(), nMouseY, (DIALOGCALLBACK)NPCMENU_OnResultContinueClicked, NULL, 0, 1, 1, 0, 0, 0, 1);
        DIALOG_SetHighlight(sgUIState.hDialogResults, 1);
        DIALOG_AddLine(sgUIState.hDialogResults, D2LANG_GetStringFromTblIndex(nResultTblIdx), 21, 0, 1, 1, NULL, 0);
        DIALOG_AddLine(sgUIState.hDialogResults, D2LANG_GetStringFromTblIndex(STR_IDX_3403_Continue), 15, 0, 0, 1, (DIALOGCALLBACK)NPCMENU_OnResultContinueClicked, 1);
        return;
    }
    if (sgUIState.CurrentState == 10)
    {
        NPCMENU_ShowTransactionResult();
        return;
    }
    if (pPacket->nResult == 1)
        SOUNDINST_PlaySound(SOUND_ITEM_GOLD, NULL, 0, 0, 0);
}

//1.10f: D2Client.0x6FAFB030
//  The answer to the two menu rows. Result 3, 5 or 6 goes back to the main menu (5 by way of the hireling icons) and drops the Identify row
//  first when the NPC is a Cain; anything else is state 12 and the result box at the mouse.
void __cdecl NPCMENU_ShowTransactionResult(void)
{
    D2C_StringIndices nResultTblIdx;
    D2UnitStrc *pNpc;
    int32_t nMouseY;

    if (sgUIState.hDialogWaiting)
    {
        DIALOG_Close(sgUIState.hDialogWaiting);
        sgUIState.hDialogWaiting = NULL;
    }
    if (sgUIState.LastTransactionResult.nResult == 5)
    {
        HIREABLES_HireIconsResetAndShow();
    }
    else if (sgUIState.LastTransactionResult.nResult != 6 && sgUIState.LastTransactionResult.nResult != 3)
    {
        switch (sgUIState.LastTransactionResult.nResult)
        {
        case 7:
        case 8:
            nResultTblIdx = STR_IDX_3358_TransactionResults6;
            break;
        case 10:
            nResultTblIdx = STR_IDX_3356_TransactionResults4;
            break;
        case 11:
            nResultTblIdx = STR_IDX_3360_TransactionResults8;
            break;
        case 12:
            nResultTblIdx = STR_IDX_3362_TransactionResults10;
            break;
        case 15:
            nResultTblIdx = STR_IDX_3363_TransactionResults11;
            break;
        case 16:
            nResultTblIdx = STR_IDX_3357_TransactionResults5;
            break;
        default:
            nResultTblIdx = STR_IDX_3361_TransactionResults9;
            break;
        }
        sgUIState.CurrentState = 12;
        if (sgUIState.hDialogResults)
        {
            FOG_DisplayAssert((uint32_t)"sgUIState.hDialogResults == NULL", (uint32_t)szSourceFile, 4955);
            exit(-1);
        }
        nMouseY = CURSOR_GetMouseY();
        sgUIState.hDialogResults = DIALOG_Create(CURSOR_GetMouseX(), nMouseY, (DIALOGCALLBACK)NPCMENU_OnResultContinueClicked, NULL, 0, 1, 1, 0, 0, 0, 1);
        DIALOG_SetHighlight(sgUIState.hDialogResults, 1);
        DIALOG_AddLine(sgUIState.hDialogResults, D2LANG_GetStringFromTblIndex(nResultTblIdx), 21, 0, 1, 1, NULL, 0);
        DIALOG_AddLine(sgUIState.hDialogResults, D2LANG_GetStringFromTblIndex(STR_IDX_3403_Continue), 15, 0, 0, 1, (DIALOGCALLBACK)NPCMENU_OnResultContinueClicked, 1);
        return;
    }

    if ((pNpc = NPCMENU_NPC_GetActiveNpcUnit()) != NULL)
    {
        switch (pNpc->dwClassId)
        {
        case MONSTER_CAIN2:
        case MONSTER_CAIN3:
        case MONSTER_CAIN4:
        case MONSTER_CAIN5:
        case MONSTER_CAIN6:
            gNpcMenuDescriptors[16].nRows = 2;
            gNpcMenuDescriptors[17].nRows = 2;
            gNpcMenuDescriptors[18].nRows = 2;
            gNpcMenuDescriptors[19].nRows = 2;
            gNpcMenuDescriptors[38].nRows = 2;
            break;
        }
    }
    NPCMENU_BuildMainMenu();
}

//  The price, the transaction and the confirm dialog. A click on an item in a trade lands in BeginTransaction. It chooses the transaction by
//  the NPC's class, prices it, records it in gTransactionItemGUID, gTransactionItemClassId, gTransactionCost and gbTransactionPending, and
//  either sends through SendTransactionAndWait or opens the confirm dialog, whose Yes sends and whose No goes back. TransactionCost is the
//  price with its label, for that dialog, for two describers of a hovered item, for the shop's click handler and for the belt's hint.
//  BUG: the two class switches disagree.
//                BeginTransaction                  TransactionCost
//      sell      147 148 177 199 202 252          the same, and 246
//                254 255 405 512 513 514
//      repair    154 178 253 257 511              the same
//                (inventory mode 4, else they sell)
//      identify  244 245 246 265 520              244 245 265
//  These are gheed, akara, drognan, elzix, lysander, asheara, alkor, ormus, jamella, drehya, malah, nihlathak; charsi, fara, hratli, halbu,
//  larzuk; cain2 to cain6. The price sells at 246 (cain4) and has no case for 520 (cain6), so its default answers 0. Both switches are written
//  as the bytes have them.
//  BUG: both identify arms ask ITEMS_CheckItemFlag(pItem, IFLAG_IDENTIFIED, 2343, ...) == 1 (cmp eax, 1 at 0x6FAFA52B and 0x6FAFB2FA), and
//  D2Common's ITEMS_CheckItemFlag answers the masked flag word, 0x10 (and eax, ecx at 0x6FD9876B), so neither arm refuses an identified item.
//  REACHABLE in TransactionCost: while a Cain's menu is up, the belt's hint prices a hovered potion as an owned item and every one shows
//  'Identify cost: 100'. NOT ESTABLISHED in BeginTransaction's copy: its callers need the inventory, the stash, the cube or the shop open
//  beside a Cain.
//  Three helpers stand for code the original inlines (none has an out-of-line copy): ITEM_IsIdentified (both identify arms push line 2343; the
//  helper holds the call and the callers keep their == 1), ITEM_NeedsRepair (the repair test of both functions, including the gold test both
//  have already made; the original hands HasUsedCharges an argument slot of the caller as its BOOL, nMouseX's in BeginTransaction and pnCost's
//  in TransactionCost, both dead or copied by then, where here it is a local) and NPCMENU_GetActiveNpcClassId (NPCMENU_NPC_GetActiveNpcUnit()
//  ? dwClassId : -1, four copies in TransactionCost, each with the lookup's NULL arm folded into the -1). NPCMENU_NPC_GetActiveNpcUnit itself
//  is inlined six times across the two and is written as calls.

static_assert(MONSTER_GHEED == 0x93 && MONSTER_AKARA == 0x94 && MONSTER_DROGNAN == 0xB1
              && MONSTER_ELZIX == 0xC7 && MONSTER_LYSANDER == 0xCA,
              "the sell cases 147, 148, 177, 199 and 202 of the jump tables at 0x6FAFA3BF and 0x6FAFB2C1");
static_assert(MONSTER_ASHEARA == 0xFC && MONSTER_ALKOR == 0xFE && MONSTER_ORMUS == 0xFF
              && MONSTER_JAMELLA == 0x195 && MONSTER_DREHYA == 0x200,
              "cmp esi, 0FCh at 0x6FAFB29C, and cases 0FEh, 0FFh, 195h and 200h of TransactionCost's tables");
static_assert(MONSTER_CHARSI == 0x9A && MONSTER_FARA == 0xB2 && MONSTER_HRATLI == 0xFD
              && MONSTER_HALBU == 0x101 && MONSTER_LARZUK == 0x1FF,
              "BeginTransaction's repair-all tests: sub eax, 9Ah / sub eax, 18h / cmp edi, 0FDh / 101h / 1FFh at 0x6FAFA29B..0x6FAFA2BB");
static_assert(STR_IDX_3329_cost == 0xD01 && STR_IDX_3330_Repair == 0xD02
              && STR_IDX_3331_Sell == 0xD03 && STR_IDX_3332_Identify == 0xD04,
              "TransactionCost: mov ecx, 0D01h / 0D02h / 0D03h / 0D04h at 0x6FAFB52D, 0x6FAFB4A4, 0x6FAFB4D2, 0x6FAFB326");
static_assert(STR_IDX_4022_strCannotDoThisToUnknown == 0xFB6, "TransactionCost: mov ecx, 0FB6h at 0x6FAFB3AF");
static_assert(STR_IDX_3344_VerifyTransaction1 == 0xD10 && STR_IDX_3345_VerifyTransaction2 == 0xD11
              && STR_IDX_3346_VerifyTransaction3 == 0xD12,
              "BuildConfirmDialog: mov ecx, 0D10h / 0D11h / 0D12h at 0x6FAFB9AC, 0x6FAFB9C3, 0x6FAFB95B");
static_assert(STR_IDX_3347_VerifyTransaction4 == 0xD13 && STR_IDX_3348_VerifyTransaction5 == 0xD14
              && STR_IDX_3350_VerifyTransaction7 == 0xD16 && STR_IDX_3351_VerifyTransaction8 == 0xD17
              && STR_IDX_3352_VerifyTransaction9 == 0xD18,
              "BuildConfirmDialog: mov ecx, 0D13h / 0D14h / 0D16h / 0D17h / 0D18h at 0x6FAFB853, 0x6FAFB7A7, 0x6FAFB87B, 0x6FAFB874, 0x6FAFB8B4");
static_assert(TRANSACTIONTYPE_BUY == 0 && TRANSACTIONTYPE_SELL == 1 && TRANSACTIONTYPE_GAMBLE == 2
              && TRANSACTIONTYPE_REPAIR == 3,
              "BeginTransaction: xor eax, eax / mov eax, 2 at 0x6FAFA564 / 0x6FAFA574, push 1 at 0x6FAFA4CF, push 3 at 0x6FAFA49B");
static_assert(STAT_QUANTITY == 0x46, "ITEM_NeedsRepair: push 46h at 0x6FAFA462 and 0x6FAFB427");
static_assert(IFLAG_IDENTIFIED == 0x10 && ITEMTYPE_GOLD == 4,
              "push 10h at 0x6FAFA523; cmp eax, 4 after ITEMS_GetItemType at 0x6FAFA34E");
static_assert(QFLAG_REWARDGRANTED == 0, "TransactionCost: push 0 as the quest state at 0x6FAFB318");

//  Both identify arms' test (line 2343, pushed by both). It answers ITEMS_CheckItemFlag as it is, the masked flag word, and the callers
//  compare that with 1.
static BOOL ITEM_IsIdentified(D2UnitStrc *pItem)
{
    return D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_IDENTIFIED, 2343, szSourceFile);
}

//  Needs repair: durability below its maximum, a stack below its maximum, or used charges on an item without charged skills. The quantity is
//  read before the maximum stack, as both copies do.
static BOOL ITEM_NeedsRepair(D2UnitStrc *pItem)
{
    int32_t nMaxDurability;
    int32_t nQuantity;
    BOOL bHasChargedSkills;

    if (D2COMMON_ITEMS_GetItemType(pItem) == ITEMTYPE_GOLD || !D2COMMON_ITEMS_IsRepairable(pItem))
        return 0;
    if (D2COMMON_ITEMS_HasDurability(pItem))
    {
        nMaxDurability = D2COMMON_STATLIST_GetMaxDurabilityFromUnit(pItem);
        if (nMaxDurability && D2COMMON_STATLIST_UnitGetStatValue(pItem, STAT_DURABILITY, 0) < nMaxDurability)
            return 1;
    }
    if (D2COMMON_ITEMS_CheckIfStackable(pItem))
    {
        nQuantity = D2COMMON_STATLIST_UnitGetStatValue(pItem, STAT_QUANTITY, 0);
        if (nQuantity < D2COMMON_ITEMS_GetTotalMaxStack(pItem))
            return 1;
    }
    if (D2COMMON_ITEMS_HasUsedCharges(pItem, &bHasChargedSkills) && !bHasChargedSkills)
        return 1;
    return 0;
}

//  The interacting NPC's class, or -1.
static int32_t NPCMENU_GetActiveNpcClassId(void)
{
    D2UnitStrc *pNpc;

    pNpc = NPCMENU_NPC_GetActiveNpcUnit();
    return pNpc ? (int32_t)pNpc->dwClassId : -1;
}

//1.10f: D2Client.0x6FAFB200
//  The price into *pnCost, always, and the label with the price into wszOut when both fit in nSize. Not owned: a buy, or a gamble. Owned: what
//  the NPC's class does with it, or 0.
BOOL __fastcall NPCMENU_TransactionCost(D2UnitStrc *pItem, BOOL bIsOwned, int32_t *pnCost, Unicode *wszOut, int32_t nSize)
{
    int32_t nItemClassId;
    int32_t nNpcClassId;
    int32_t nVendorId;
    int32_t nCost;
    D2C_TransactionTypes nTransactionType;
    D2C_StringIndices nLabelTblIdx;
    D2C_Difficulties nDifficulty;
    D2BitBufferStrc *pQuestRecord;
    const Unicode *wszLabel;
    char szCost[20];

    *pnCost = 0;
    if (!pItem)
        return 0;
    if (D2COMMON_ITEMS_GetItemType(pItem) == ITEMTYPE_GOLD)
        return 0;

    if (!bIsOwned)
    {
        nTransactionType = TRANSACTIONTYPE_BUY;
        if (sgUIState.bNpcVendorGamble)
            nTransactionType = TRANSACTIONTYPE_GAMBLE;
        nVendorId = NPCMENU_GetActiveNpcClassId();
        pQuestRecord = sgUIState.pNpcQuestRecord;
        nDifficulty = (D2C_Difficulties)(uint8_t)GAME_GetDifficulty();
        nCost = D2COMMON_ITEMS_GetTransactionCost(CUNIT_GetPlayer(), pItem, nDifficulty, pQuestRecord, nVendorId, nTransactionType);
        nLabelTblIdx = STR_IDX_3329_cost;
    }
    else
    {
        nItemClassId = (int32_t)pItem->dwClassId;
        if (!NPCMENU_NPC_GetActiveNpcUnit())
            return 0;
        nNpcClassId = NPCMENU_GetActiveNpcClassId();
        //  the record is fetched and dropped
        D2COMMON_DATATBLS_GetItemsTxtRecord(nItemClassId);

        switch (nNpcClassId)
        {
        case MONSTER_CAIN2:
        case MONSTER_CAIN3:
        case MONSTER_CAIN5:
            //  never taken: ITEMS_CheckItemFlag answers 0x10, not 1
            if (ITEM_IsIdentified(pItem) == 1)
            {
                //  mov dword ptr [edx], 0: two characters
                wszOut[0].ch = 0;
                wszOut[1].ch = 0;
                return 1;
            }
            nCost = D2COMMON_QUESTRECORD_GetQuestState(sgUIState.pNpcQuestRecord, 4, QFLAG_REWARDGRANTED) ? 0 : 100;
            nLabelTblIdx = STR_IDX_3332_Identify;
            break;

        case MONSTER_CHARSI:
        case MONSTER_FARA:
        case MONSTER_HRATLI:
        case MONSTER_HALBU:
        case MONSTER_LARZUK:
            if (INV_IsInventoryMode4())
            {
                if (!D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_IDENTIFIED, 5059, szSourceFile))
                {
                    Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_4022_strCannotDoThisToUnknown));
                    UI_AddColorCodeToWstring((wchar_t *)wszOut, 1);
                    return 1;
                }
                if (!ITEM_NeedsRepair(pItem))
                    return 0;
                nVendorId = NPCMENU_GetActiveNpcClassId();
                pQuestRecord = sgUIState.pNpcQuestRecord;
                nDifficulty = (D2C_Difficulties)(uint8_t)GAME_GetDifficulty();
                nCost = D2COMMON_ITEMS_GetTransactionCost(CUNIT_GetPlayer(), pItem, nDifficulty, pQuestRecord, nVendorId, TRANSACTIONTYPE_REPAIR);
                nLabelTblIdx = STR_IDX_3330_Repair;
                break;
            }
            //  not in the repair mode: the smith buys, as the traders do (fall through)
        case MONSTER_GHEED:
        case MONSTER_AKARA:
        case MONSTER_DROGNAN:
        case MONSTER_ELZIX:
        case MONSTER_LYSANDER:
        case MONSTER_CAIN4:
        case MONSTER_ASHEARA:
        case MONSTER_ALKOR:
        case MONSTER_ORMUS:
        case MONSTER_JAMELLA:
        case MONSTER_DREHYA:
        case MONSTER_MALAH:
        case MONSTER_NIHLATHAK:
            if (!D2COMMON_ITEMS_IsNotQuestItem(pItem))
                return 0;
            nVendorId = NPCMENU_GetActiveNpcClassId();
            pQuestRecord = sgUIState.pNpcQuestRecord;
            nDifficulty = (D2C_Difficulties)(uint8_t)GAME_GetDifficulty();
            nCost = D2COMMON_ITEMS_GetTransactionCost(CUNIT_GetPlayer(), pItem, nDifficulty, pQuestRecord, nVendorId, TRANSACTIONTYPE_SELL);
            nLabelTblIdx = STR_IDX_3331_Sell;
            break;

        default:
            //  520, cain6, among them: the price has no case for it
            return 0;
        }
    }

    *pnCost = nCost;
    wszLabel = D2LANG_GetStringFromTblIndex(nLabelTblIdx);
    if (Unicode::strlen(wszLabel) + 10 > nSize)
        return 0;
    Unicode::strcpy(wszOut, wszLabel);
    {
        Unicode wszCost[20];

        wsprintfA(szCost, "%d", *pnCost);
        Unicode::win2Unicode(wszCost, szCost, 19);
        Unicode::strcat(wszOut, wszCost);
    }
    return 1;
}

//1.10f: D2Client.0x6FAFA260
//  Where a click on an item in a trade begins. nMouseX, nMouseY and nMouseKeyFlags are the click's position and the mouse message's wParam;
//  only a quick buy or sell sends at once. pPlayer is the NPC at two call sites (INV_ItemPanelOnRButtonDown and INV_NpcShopOnLButtonDown push
//  the unit they found by GUID) and the player at the other five; it is read only in the repair-all arm, whose caller passes the player.
//  The sell arm stores gTransactionCost before it asks ITEMS_IsNotQuestItem, so a refused quest item leaves its price behind with
//  gbTransactionPending unset; the identify arm stores no price and leaves the previous one. The repair-all arm answers 1 and does nothing when
//  INV_GetTransactionItemIsPlayers is 0, which it never is inside INV_NpcShopOnLButtonUp's call, the only one passing bRepairAll.
BOOL __fastcall NPCMENU_BeginTransaction(D2UnitStrc *pPlayer, D2UnitStrc *pItem, int32_t bPlayerOwnsItem, int32_t nMouseX, int32_t nMouseY, int32_t nMouseKeyFlags, int32_t bQuickTransaction, int32_t bRepairAll)
{
    D2UnitStrc *pNpc;
    int32_t nNpcClassId;
    D2C_TransactionTypes nTransactionType;
    D2C_Difficulties nDifficulty;
    D2BitBufferStrc *pQuestRecord;

    pNpc = NPCMENU_NPC_GetActiveNpcUnit();
    if (!pNpc)
        return 0;
    nNpcClassId = (int32_t)pNpc->dwClassId;

    if (bRepairAll)
    {
        switch (nNpcClassId)
        {
        case MONSTER_CHARSI:
        case MONSTER_FARA:
        case MONSTER_HRATLI:
        case MONSTER_HALBU:
        case MONSTER_LARZUK:
            break;
        default:
            return 0;
        }
        if (!INV_GetTransactionItemIsPlayers())
            return 1;
        pQuestRecord = sgUIState.pNpcQuestRecord;
        nDifficulty = (D2C_Difficulties)(uint8_t)GAME_GetDifficulty();
        sgUIState.TransactionCost = D2COMMON_ITEMS_GetAllRepairCosts(NULL, pPlayer, nNpcClassId, nDifficulty, pQuestRecord, NULL);
        sgUIState.Transaction = 3;
        gbTransactionPending = 1;
        gTransactionItemGUID = 0;
        SELECT_SetSelectedUnit(NULL);
        sgUIState.unk0xE3 = (uint32_t)nMouseX;
        sgUIState.unk0xE7 = (uint32_t)nMouseY;
        NPCMENU_SendTransactionAndWait(1, nMouseKeyFlags);
        return 1;
    }

    if (D2COMMON_ITEMS_GetItemType(pItem) == ITEMTYPE_GOLD)
        return 0;
    if (bQuickTransaction && GetTickCount() - (uint32_t)gLastTransactionTick < 500)
        return 0;
    //  the click's position, which nothing reads back
    sgUIState.unk0xE3 = (uint32_t)nMouseX;
    sgUIState.unk0xE7 = (uint32_t)nMouseY;

    if (bPlayerOwnsItem != 1)
    {
        if (D2COMMON_INVENTORY_GetCursorItem(CUNIT_GetPlayer()->pInventory))
            return 0;
        nTransactionType = TRANSACTIONTYPE_BUY;
        sgUIState.Transaction = 1;
        if (sgUIState.bNpcVendorGamble)
            nTransactionType = TRANSACTIONTYPE_GAMBLE;
        pQuestRecord = sgUIState.pNpcQuestRecord;
        nDifficulty = (D2C_Difficulties)(uint8_t)GAME_GetDifficulty();
        sgUIState.TransactionCost = D2COMMON_ITEMS_GetTransactionCost(CUNIT_GetPlayer(), pItem, nDifficulty, pQuestRecord, nNpcClassId, nTransactionType);
    }
    else
    {
        switch (nNpcClassId)
        {
        case MONSTER_CAIN2:
        case MONSTER_CAIN3:
        case MONSTER_CAIN4:
        case MONSTER_CAIN5:
        case MONSTER_CAIN6:
            //  never refuses: ITEMS_CheckItemFlag answers 0x10, not 1
            if (ITEM_IsIdentified(pItem) == 1)
                return 0;
            //  and no price: gTransactionCost keeps the last one
            sgUIState.Transaction = 4;
            break;

        case MONSTER_CHARSI:
        case MONSTER_FARA:
        case MONSTER_HRATLI:
        case MONSTER_HALBU:
        case MONSTER_LARZUK:
            if (INV_IsInventoryMode4())
            {
                if (!ITEM_NeedsRepair(pItem))
                    return 0;
                pQuestRecord = sgUIState.pNpcQuestRecord;
                sgUIState.Transaction = 3;
                nDifficulty = (D2C_Difficulties)(uint8_t)GAME_GetDifficulty();
                sgUIState.TransactionCost = D2COMMON_ITEMS_GetTransactionCost(CUNIT_GetPlayer(), pItem, nDifficulty, pQuestRecord, nNpcClassId, TRANSACTIONTYPE_REPAIR);
                break;
            }
            //  not in the repair mode: the smith buys, as the traders do (fall through)
        case MONSTER_GHEED:
        case MONSTER_AKARA:
        case MONSTER_DROGNAN:
        case MONSTER_ELZIX:
        case MONSTER_LYSANDER:
        case MONSTER_ASHEARA:
        case MONSTER_ALKOR:
        case MONSTER_ORMUS:
        case MONSTER_JAMELLA:
        case MONSTER_DREHYA:
        case MONSTER_MALAH:
        case MONSTER_NIHLATHAK:
            pQuestRecord = sgUIState.pNpcQuestRecord;
            nDifficulty = (D2C_Difficulties)(uint8_t)GAME_GetDifficulty();
            //  the price is stored before the quest-item refusal
            sgUIState.TransactionCost = D2COMMON_ITEMS_GetTransactionCost(CUNIT_GetPlayer(), pItem, nDifficulty, pQuestRecord, nNpcClassId, TRANSACTIONTYPE_SELL);
            if (!D2COMMON_ITEMS_IsNotQuestItem(pItem))
                return 0;
            sgUIState.Transaction = 2;
            break;

        default:
            return 0;
        }
    }

    gbTransactionPending = 1;
    gTransactionItemGUID = pItem ? pItem->dwUnitId : (uint32_t)-1;
    gTransactionItemClassId = pItem ? (int32_t)pItem->dwClassId : -1;
    SELECT_SetSelectedUnit(NULL);
    if ((sgUIState.Transaction == 2 || sgUIState.Transaction == 1) && bQuickTransaction)
    {
        NPCMENU_SendTransactionAndWait(1, nMouseKeyFlags);
        return 1;
    }
    SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
    sgUIState.CurrentState = 4;
    NPCMENU_BuildConfirmDialog();
    return 1;
}

//1.10f: D2Client.0x6FAFB710
//  The box at the mouse: a title by transaction, then -- for 1 to 4 -- the item's name, 'Gold:' and the price, and for every transaction Yes and
//  No. The hire, 5, has only its title before the two. It returns after its title when the transaction item has left the client: no Yes, no No,
//  and the box's cancel callback the only way out. The item's name is cut at its first newline. gwszNpcConfirmText gets TransactionCost's label
//  and number, and then the number alone over it; only pnCost's value was wanted.
void __cdecl NPCMENU_BuildConfirmDialog(void)
{
    BOOL bIsOwned;
    D2C_StringIndices nTitleTblIdx;
    D2UnitStrc *pItem;
    int32_t nMouseY;
    int32_t nLen;
    int32_t i;
    int32_t nCost;
    char szCost[12];

    if (sgUIState.hDialogConfirm)
    {
        FOG_DisplayAssert((uint32_t)"sgUIState.hDialogConfirm == NULL", (uint32_t)szSourceFile, 5219);
        exit(-1);
    }
    sgUIState.CurrentState = 4;
    nMouseY = CURSOR_GetMouseY();
    sgUIState.hDialogConfirm = DIALOG_Create(CURSOR_GetMouseX(), nMouseY, (DIALOGCALLBACK)NPCMENU_OnConfirmDialogDismissed, NULL, 0, 1, 1, 0, 0, 0, 1);
    DIALOG_SetHighlight(sgUIState.hDialogConfirm, 1);

    switch (sgUIState.Transaction)
    {
    case 1:
        DIALOG_AddLine(sgUIState.hDialogConfirm, D2LANG_GetStringFromTblIndex(STR_IDX_3348_VerifyTransaction5), 21, 0, 4, 1, NULL, 0);
        bIsOwned = 0;
        break;
    case 2:
        DIALOG_AddLine(sgUIState.hDialogConfirm, D2LANG_GetStringFromTblIndex(STR_IDX_3347_VerifyTransaction4), 21, 0, 4, 1, NULL, 0);
        bIsOwned = 1;
        break;
    case 3:
        nTitleTblIdx = STR_IDX_3351_VerifyTransaction8;
        goto ADD_OWNED_TITLE;
    case 4:
        nTitleTblIdx = STR_IDX_3350_VerifyTransaction7;
ADD_OWNED_TITLE:
        bIsOwned = 1;
        DIALOG_AddLine(sgUIState.hDialogConfirm, D2LANG_GetStringFromTblIndex(nTitleTblIdx), 21, 0, 4, 1, NULL, 0);
        break;
    case 5:
        DIALOG_AddLine(sgUIState.hDialogConfirm, D2LANG_GetStringFromTblIndex(STR_IDX_3352_VerifyTransaction9), 21, 0, 4, 1, NULL, 0);
        goto ADD_CHOICES;
    default:
        FOG_DisplayAssert((uint32_t)"0", (uint32_t)szSourceFile, 5272);
        exit(-1);
    }

    if (!gbTransactionPending)
        FOG_Trace("Error at line %d of file %s\n", 5280, szSourceFile);
    pItem = CUNIT_FindUnitByGuidAndType(gTransactionItemGUID, UNIT_ITEM);
    if (!pItem)
        return;
    {
        Unicode wszName[192];

        INV_BuildItemNameText(pItem, wszName, 191);
        nLen = Unicode::strlen(wszName);
        for (i = 0; i < nLen; ++i)
        {
            if (wszName[i].ch == 10)
            {
                wszName[i].ch = 0;
                break;
            }
        }
        DIALOG_AddLine(sgUIState.hDialogConfirm, wszName, 15, 0, 4, 1, (DIALOGCALLBACK)NPCMENU_OnGraphicActionVerified, 0);
    }
    gwszNpcConfirmText->ch = 0;
    NPCMENU_TransactionCost(pItem, bIsOwned, &nCost, gwszNpcConfirmText, 40);
    {
        Unicode wszCost[10];

        wsprintfA(szCost, "%d", nCost);
        Unicode::win2Unicode(wszCost, szCost, 9);
        Unicode::strcpy(gwszNpcConfirmText, wszCost);   //  at most nine characters and the NUL, into forty
    }
    DIALOG_AddLine(sgUIState.hDialogConfirm, D2LANG_GetStringFromTblIndex(STR_IDX_3346_VerifyTransaction3), 15, 0, 4, 1, (DIALOGCALLBACK)NPCMENU_OnGraphicActionVerified, 0);
    DIALOG_AddLine(sgUIState.hDialogConfirm, gwszNpcConfirmText, 15, 0, 4, 1, (DIALOGCALLBACK)NPCMENU_OnGraphicActionVerified, 0);
ADD_CHOICES:
    DIALOG_AddLine(sgUIState.hDialogConfirm, D2LANG_GetStringFromTblIndex(STR_IDX_3344_VerifyTransaction1), 15, 0, 0, 1, (DIALOGCALLBACK)NPCMENU_OnGraphicActionVerified, 1);
    DIALOG_AddLine(sgUIState.hDialogConfirm, D2LANG_GetStringFromTblIndex(STR_IDX_3345_VerifyTransaction2), 15, 0, 0, 1, (DIALOGCALLBACK)NPCMENU_OnConfirmDialogDismissed, 1);
}

//1.10f: D2Client.0x6FAFBA30
//  Yes: the hire, or the transaction without its verify step; otherwise only the selection cleared and an assert on a state already tested.
//  The assert re-tests the state its second arm has already tested; SELECT_ClearSelectedUnit sits between them, so it is transcribed rather
//  than folded. Answers 1 on every path (the dialog reads it as a pfnOnClick answer).
int32_t __cdecl NPCMENU_OnGraphicActionVerified(void)
{
    if (sgUIState.Transaction == 5)
    {
        if (sgUIState.hDialogConfirm)
        {
            DIALOG_Close(sgUIState.hDialogConfirm);
            sgUIState.hDialogConfirm = NULL;
        }
        NPCMENU_ConfirmHireMercenary();
        return 1;
    }
    if (sgUIState.CurrentState == 4)
    {
        if (sgUIState.hDialogConfirm)
        {
            DIALOG_Close(sgUIState.hDialogConfirm);
            sgUIState.hDialogConfirm = NULL;
        }
        NPCMENU_SendTransactionAndWait(0, 0);
        return 1;
    }
    SELECT_ClearSelectedUnit();
    if (sgUIState.CurrentState == 4)
    {
        FOG_DisplayAssert((uint32_t)"sgUIState.CurrentState != UIStateGraphicVerifyingAction", (uint32_t)szSourceFile, 5185);
        exit(-1);
    }
    return 1;
}

//1.10f: D2Client.0x6FAFBAC0
//  No, and the box's own cancel: back to the hire list for a hire, otherwise out of the verify mode to state 3.
int32_t __cdecl NPCMENU_OnConfirmDialogDismissed(void)
{
    DIALOG_Close(sgUIState.hDialogConfirm);
    sgUIState.hDialogConfirm = NULL;
    if (sgUIState.Transaction == 5)
    {
        NPCMENU_DrawHireMercenaryList();
        return 1;
    }
    INV_LeavePanelMode5();
    sgUIState.CurrentState = 3;
    SELECT_ClearSelectedUnit();
    return 1;
}

//  The main menu, its opener and the interrupt dialog.
//
//  BuildMainMenu finds the NPC's record in gNpcMenuDescriptors by class id (record 0, akara's (148), on a miss) and adds nRows - 1 rows from it,
//  the label out of wTblIdx[] and the handler out of pfnHandler[], then 'cancel'. The search has no out-of-line copy: the table's base is taken
//  here and in NPCMENU_ChooseNextSpeech and nowhere else, and ChooseNextSpeech's copy is written out in the same shape. The bound test is
//  index < 0 || index >= count with what the compiler knew folded in: the found path keeps its jl (0x6FAF68D0), the miss path, index 0, goes
//  straight to the jge. Three labels are replaced on the way:
//      5383  'Resurrect %s: %d' (22696) with the offer's name -- tbl gwMercResurrectNameTblId, or 11021 when that holds 1057 (cmp cx, 421h / je
//            past the lookup, 0x6FAF6953) -- and gnMercResurrectCost. NPCMENU_ToggleResurrectMercRow inserts the label, and OpenMainMenu calls it
//            for records 8, 11, 21, 24 and 43.
//      4020  'Identify Items: ' (4021) and 100 per unidentified item, or the plain label; with nothing unidentified the row takes itself off,
//            nRows = 2, which NPCMENU_BeginInteraction undoes at the next conversation through NPCMENU_EnableIdentifyRow.
//      3337  'Heal :' and UNITS_GetHealingCost floored at 1, below full life only.
//  BUG: the heal row (label 3337) cannot appear. UNREACHABLE: no shipped record carries 3337 and no writer of wTblIdx stores it. Reproduced whole,
//  including a store that cannot run: after the floor the bytes test the cost again and clear bSelectable if it is zero (0x6FAF6A35).
//  DIFFERENCE: the two costs go into two stack slots the view arithmetic has finished with in the original (the heal cost into the RECT, the
//  identify cost into the four view-rect ints); ours uses separate buffers, each the width of the slot.
//  BUG: the identify label and the identify price disagree. The label leaves the price off when quest 4 has QFLAG_REWARDGRANTED or
//  QFLAG_REWARDPENDING (0x6FAF6A5D, 0x6FAF6A70), while NPCMENU_TransactionCost prices identifying at 100 unless QFLAG_REWARDGRANTED is set. So
//  while only the pending bit is set, the menu row says 'Identify Items' and the price line the belt's hint reaches says 100. NOT ESTABLISHED:
//  whether a Cain can be talked to in that window, and which figure the server charges.
//
//  OpenMainMenu: the original inlines NPCMENU_NpcCloseInteraction (both refusals at the top), NPCMENU_SetInteractingNpc (gbNpcInteractionActive
//  = 1, the GUID and the class id, in that order; the refused toggle's gbNpcInteractionActive = 0 is SetInteractingNpc(NULL)),
//  NPCMENU_SetUnitFlag(pNpc, UNITFLAG_TARGETABLE, ...) (the two dwFlags stores), NPCMENU_QuestUnlock_KashyaHire (Kashya's level-8 block, so her
//  hire row comes on at character level 8 as well as on quest 2's reward) and NPCMENU_NpcResyncVendorInventory (the refused toggle's tail: the
//  trace, the lookup, NPCMENU_NpcFreeVendorInventory under the lookup's test, which is why the offers are cleared only when the unit is found,
//  and the greeting sound). The one-shot greeting stop in front of the resurrect toggles is the seven instructions OnTradeClicked has, written
//  out. The class id the searches key on is copied before the unlocks run.
//  DIFFERENCE: gNpcMenuQuestUnlocks (npcmenu.h), read by OpenMainMenu alone, is a table of ours holding the eight rows as the stock bytes ship
//  them (bWantState is 1 in all eight) with our unlock functions in pfnAddRow.
//  The greeting: after BuildMainMenu, and only for a fresh conversation, the NPC's entry in gNpcMenuEntries picks the speech kind. +0x12, raised
//  by packet 0x91 (NPCMENU_MarkEntriesFromPacket91), makes it a greeting, kind 2, and is cleared at once; +0x15 must then be 1 or nothing plays,
//  so a greeting owed to an entry whose +0x15 is not 1 is consumed unplayed. An NPC with no entry skips both and plays kind 0. A kind-2 line
//  keeps its sound handle, sends client packet 0x4D with the class id and arms the one-shot flag; any other kind drops the handle at once.
//
//  BuildInterruptDialog: NPCMENU_NpcCloseInteraction when the NPC is gone; otherwise the box above the NPC, the NPC's name lower-cased as its
//  title (Unicode::toLower inlined over the name through _toLowerTable), one of five news lines, 3386..3390, picked by SEED_RollRandomNumber
//  inlined modulo 5, and 'cancel', with the news line selected. The five ids are stored into the dead view-rect slots before the roll.

static_assert(MONSTER_ASHEARA == 0xFC && MONSTER_GREIZ == 0xC6 && MONSTER_QUAL_KEHK == 0x203
              && MONSTER_KASHYA == 0x96,
              "BuildMainMenu: cmp eax, 0FCh / 0C6h / 203h / 96h at 0x6FAF66C6..0x6FAF66DB");
static_assert(MONSTER_TYRAEL2 == 0x16F, "OpenMainMenu: cmp eax, 16Fh at 0x6FAF9AE6");
static_assert(STR_IDX_5383_Dummy == 0x1507 && STR_IDX_3337_NPCHeal == 0xD09
              && STR_IDX_4020_NPCIdentify1 == 0xFB4 && STR_IDX_4021_NPCIdentify2 == 0xFB5,
              "BuildMainMenu: cmp cx, 1507h / 0D09h / 0FB4h and mov ecx, 0FB5h");
static_assert(STR_IDX_11021_merca239 == 0x2B0D && STR_IDX_1057_merca239 == 0x421
              && STR_IDX_22696_hireresurrect2 == 0x58A8,
              "BuildMainMenu: mov ecx, 2B0Dh, cmp cx, 421h, mov ecx, 58A8h at 0x6FAF6943..0x6FAF6965");
static_assert(STR_IDX_4142_lowercasecancel == 0x102E,
              "the cancel rows: mov ecx, 102Eh at 0x6FAF6B8D and 0x6FAF9388");
static_assert(STR_IDX_3386_NPCMenuNews0 == 0xD3A && STR_IDX_3387_NPCMenuNews1 == 0xD3B
              && STR_IDX_3388_NPCMenuNews2 == 0xD3C && STR_IDX_3389_NPCMenuNews3 == 0xD3D
              && STR_IDX_3390_NPCMenuNews4 == 0xD3E,
              "BuildInterruptDialog: the five stores at 0x6FAF9316..0x6FAF9332");
static_assert(STAT_HITPOINTS == 6 && STAT_LEVEL == 0x0C && STAT_GOLD == 0x0E,
              "push 6 at 0x6FAF699A, push 0Ch at 0x6FAF9AA9, push 0Eh at 0x6FAF991E");
static_assert(UNITFLAG_TARGETABLE == 2, "OpenMainMenu: and edx, 0FFFFFFFDh / or edx, 2");
static_assert(QFLAG_REWARDPENDING == 1, "BuildMainMenu: push 1 as the quest state at 0x6FAF6A6B");
static_assert(offsetof(D2NpcMenuEntryStrc, unk0x12) == 0x12 && offsetof(D2NpcMenuEntryStrc, bAborted) == 0x15,
              "OpenMainMenu: gNpcMenuEntries+12h and +15h at 0x6FAF9C2C..0x6FAF9C4F");
static_assert(offsetof(D2NpcMenuQuestUnlockStrc, nQuestState) == 8
              && offsetof(D2NpcMenuQuestUnlockStrc, pfnAddRow) == 0x10,
              "OpenMainMenu: the cursor starts at +8 and calls dword ptr [edi+8]");


//1.10f: D2Client.0x6FAF65C0
void __cdecl NPCMENU_BuildMainMenu(void)
{
    D2UnitStrc *pNpc;
    D2UnitStrc *pPlayer;
    D2CoordStrc tClientCoords;
    int32_t nViewLeft;
    int32_t nViewTop;
    int32_t nViewRight;
    int32_t nViewBottom;
    int32_t nOriginX;
    int32_t nOriginY;
    RECT rcView;
    int32_t nDescriptor;
    int32_t i;
    D2NpcMenuDescStrc *pRec;
    int32_t nRow;
    int32_t bSelectable;
    const Unicode *wszMercName;
    uint32_t nHealCost;
    int32_t bShowPrice;
    int32_t nUnidItems;

    if (sgUIState.hDialogMainMenu)
    {
        FOG_DisplayAssert((uint32_t)"sgUIState.hDialogMainMenu == NULL", (uint32_t)szSourceFile, 3057);
        exit(-1);
    }
    if (!NPCMENU_NPC_GetActiveNpcUnit())
    {
        NPCMENU_NpcCloseInteraction();
        return;
    }

    //  The four hireling NPCs ask for their offer list as the menu opens.
    if (sgUIState.NpcClassId == MONSTER_ASHEARA || sgUIState.NpcClassId == MONSTER_GREIZ || sgUIState.NpcClassId == MONSTER_QUAL_KEHK || sgUIState.NpcClassId == MONSTER_KASHYA)
    {
        pPlayer = CUNIT_GetPlayer();
        MSG_SendCmdUnitParam(0x38, 3, (int32_t)sgUIState.NpcUnitGUID, pPlayer ? (int32_t)pPlayer->dwUnitId : -1);
    }

    tClientCoords.nX = 0;
    tClientCoords.nY = 0;
    if (NPCMENU_NPC_GetActiveNpcUnit())
    {
        pNpc = NPCMENU_NPC_GetActiveNpcUnit();
        D2COMMON_UNITS_GetClientCoords(pNpc, &tClientCoords);
        VIEW_GetRect(gpView, &nViewLeft, &nViewTop, &nViewRight, &nViewBottom);
        VIEW_GetOrigin(gpView, &nOriginX, &nOriginY);
        SetRect(&rcView, nOriginX, nOriginY, nViewRight - nViewLeft + nOriginX, nOriginY - nViewTop + nViewBottom);
        tClientCoords.nX -= rcView.left;
        tClientCoords.nY += -150 - rcView.top;
        if (tClientCoords.nY < 20)
            tClientCoords.nY = 20;
    }
    sgUIState.hDialogMainMenu = DIALOG_Create(tClientCoords.nX, tClientCoords.nY, (DIALOGCALLBACK)NPCMENU_OnMainMenuCancel, (DIALOGCALLBACK)NPCMENU_OnMainMenuClickAway, 0, 0, 1, 0, 0, 0, 1);
    DIALOG_SetHighlight(sgUIState.hDialogMainMenu, 1);
    {
        Unicode wszTitle[80];

        Unicode::strcpy(wszTitle, CUNIT_GetUnitName(NPCMENU_NPC_GetActiveNpcUnit()));
        Unicode::strlen(wszTitle);      //  the answer is dropped
        DIALOG_AddLine(sgUIState.hDialogMainMenu, wszTitle, 21, 0, 4, 1, NULL, 0);
    }

    pNpc = NPCMENU_NPC_GetActiveNpcUnit();
    if (!pNpc)
        return;

    nDescriptor = 0;
    for (i = 0; i < gnNpcMenuDescriptorCount; ++i)
    {
        if (gNpcMenuDescriptors[i].nMonsterId == (int32_t)pNpc->dwClassId)
        {
            nDescriptor = i;
            break;
        }
    }
    if (nDescriptor < 0 || nDescriptor >= gnNpcMenuDescriptorCount)
        return;

    sgUIState.CurrentState = 1;
    pRec = &gNpcMenuDescriptors[nDescriptor];
    for (nRow = 0; nRow < pRec->nRows - 1; ++nRow)
    {
        Unicode wszLine[512];

        bSelectable = 1;
        if (pRec->wTblIdx[nRow] == STR_IDX_5383_Dummy)
        {
            wszMercName = D2LANG_GetStringFromTblIndex(STR_IDX_11021_merca239);
            if ((uint16_t)gwMercResurrectNameTblId != STR_IDX_1057_merca239)
                wszMercName = D2LANG_GetStringFromTblIndex(gwMercResurrectNameTblId);
            Unicode::sprintf(512, wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_22696_hireresurrect2), wszMercName, (int32_t)gnMercResurrectCost);
        }
        else if (pRec->wTblIdx[nRow] == STR_IDX_3337_NPCHeal)
        {
            //  UNREACHABLE in 1.10: no record carries label 3337
            if (D2COMMON_STATLIST_UnitGetStatValue(CUNIT_GetPlayer(), STAT_HITPOINTS, 0) >= D2COMMON_STATLIST_GetMaxLifeFromUnit(CUNIT_GetPlayer()))
                continue;
            nHealCost = D2COMMON_UNITS_GetHealingCost(CUNIT_GetPlayer());
            if (!nHealCost)
                nHealCost = 1;
            Unicode::strcpy(wszLine, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pRec->wTblIdx[nRow]));
            {
                Unicode wszCost[10];
                char szCost[16];            //  the RECT's slot in the original

                wsprintfA(szCost, "%d", nHealCost);
                Unicode::win2Unicode(wszCost, szCost, 9);
                Unicode::strcat(wszLine, wszCost);
            }
            //  Cannot run: the cost was floored at 1 just above. The bytes test it anyway (0x6FAF6A35).
            if (!nHealCost)
                bSelectable = 0;
        }
        else if (pRec->wTblIdx[nRow] == STR_IDX_4020_NPCIdentify1)
        {
            bShowPrice = 0;
            if (!D2COMMON_QUESTRECORD_GetQuestState(sgUIState.pNpcQuestRecord, 4, QFLAG_REWARDGRANTED) && !D2COMMON_QUESTRECORD_GetQuestState(sgUIState.pNpcQuestRecord, 4, QFLAG_REWARDPENDING))
            {
                bShowPrice = 1;
            }
            nUnidItems = D2COMMON_ITEMS_GetNoOfUnidItems(CUNIT_GetPlayer());
            if (!nUnidItems)
            {
                pRec->nRows = 2;            //  the row takes itself off
                continue;
            }
            pRec->nRows = 3;
            if (bShowPrice)
            {
                Unicode::strcpy(wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_4021_NPCIdentify2));
                {
                    Unicode wszCost[10];
                    char szCost[16];        //  the four view-rect ints' slot

                    wsprintfA(szCost, "%d", 100 * nUnidItems);
                    Unicode::win2Unicode(wszCost, szCost, 9);
                    Unicode::strcat(wszLine, wszCost);
                }
            }
            else
            {
                Unicode::strcpy(wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_4020_NPCIdentify1));
            }
        }
        else
        {
            Unicode::strcpy(wszLine, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pRec->wTblIdx[nRow]));
        }
        DIALOG_AddLine(sgUIState.hDialogMainMenu, wszLine, 15, 0, 0, 1, (DIALOGCALLBACK)pRec->pfnHandler[nRow], bSelectable);
    }
    DIALOG_AddLine(sgUIState.hDialogMainMenu, D2LANG_GetStringFromTblIndex(STR_IDX_4142_lowercasecancel), 15, 0, 0, 1, (DIALOGCALLBACK)NPCMENU_OnMainMenuCancel, 1);
}

//1.10f: D2Client.0x6FAF98D0
void __fastcall NPCMENU_OpenMainMenu(D2UnitStrc *pNpc, int32_t bAlreadyInteracting)
{
    int32_t nClassId;
    D2BitBufferStrc *pQuestRecord;
    const D2NpcMenuQuestUnlockStrc *pUnlock;
    int32_t nUnlocksLeft;
    int32_t nEntry;
    int32_t nSpeechKind;
    D2C_Sounds nSoundId;

    GAME_ResetClickState();
    if (!pNpc)
    {
        NPCMENU_NpcCloseInteraction();
        return;
    }
    sgUIState.CurrentState = 1;
    sgUIState.Transaction = 0;
    sgUIState.nCachedPlayerGold = D2COMMON_STATLIST_UnitGetStatValue(CUNIT_GetPlayer(), STAT_GOLD, 0);
    UI_CloseAllPanels(0, 0);

    if (bAlreadyInteracting)
    {
        if (pNpc != NPCMENU_NPC_GetActiveNpcUnit())
            FOG_Trace("log at %d %s\n", 3433, szSourceFile);
    }
    else
    {
        if (pNpc == NPCMENU_NPC_GetActiveNpcUnit())
            return;
        if (NPCMENU_NPC_GetActiveNpcUnit())
        {
            NPCMENU_NpcCloseInteraction();
            return;
        }
    }

    NPCMENU_SetInteractingNpc((int32_t *)pNpc);
    NPCMENU_SetUnitFlag(pNpc, UNITFLAG_TARGETABLE, FALSE);
    nClassId = (int32_t)pNpc->dwClassId;

    pQuestRecord = sgUIState.pNpcQuestRecord;
    if (gnNpcMenuQuestUnlockCount > 0)
    {
        pUnlock = gNpcMenuQuestUnlocks;
        nUnlocksLeft = gnNpcMenuQuestUnlockCount;
        do
        {
            if (pUnlock->nMonsterClassId == nClassId)
            {
                if (pUnlock->bWantState ? D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, pUnlock->nQuestStateFlag, pUnlock->nQuestState) : !D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, pUnlock->nQuestStateFlag, pUnlock->nQuestState))
                {
                    pUnlock->pfnAddRow();
                }
            }
            ++pUnlock;
        }
        while (--nUnlocksLeft);
    }

    if (sgUIState.bNpcGreetingIsOneShot)
    {
        if (sgUIState.pNpcGreetingSound)
        {
            SOUNDINST_MarkStarted(sgUIState.pNpcGreetingSound);
            sgUIState.pNpcGreetingSound = 0;
        }
        sgUIState.bNpcGreetingIsOneShot = 0;
    }

    //  The resurrect row, for the four hireling NPCs and Tyrael in act 4.
    switch (pNpc->dwClassId)
    {
    case MONSTER_KASHYA:
        if (D2COMMON_STATLIST_UnitGetStatValue(CUNIT_GetPlayer(), STAT_LEVEL, 0) >= 8)
            NPCMENU_QuestUnlock_KashyaHire();
        NPCMENU_ToggleResurrectMercRow(11, (uint16_t)gwMercResurrectNameTblId != 0xFFFF);
        break;
    case MONSTER_GREIZ:
        NPCMENU_ToggleResurrectMercRow(8, (uint16_t)gwMercResurrectNameTblId != 0xFFFF);
        break;
    case MONSTER_ASHEARA:
        NPCMENU_ToggleResurrectMercRow(24, (uint16_t)gwMercResurrectNameTblId != 0xFFFF);
        break;
    case MONSTER_TYRAEL2:
        NPCMENU_ToggleResurrectMercRow(21, (uint16_t)gwMercResurrectNameTblId != 0xFFFF);
        break;
    case MONSTER_QUAL_KEHK:
        NPCMENU_ToggleResurrectMercRow(43, (uint16_t)gwMercResurrectNameTblId != 0xFFFF);
        break;
    default:
        break;
    }

    if (!UI_SetUIToggle(UI_NPCMENU, UI_TURNON, 0))
    {
        sgUIState.CurrentState = 0;
        NPCMENU_SetUnitFlag(pNpc, UNITFLAG_TARGETABLE, TRUE);
        MSG_SendCmdUnit(0x30, 1, (int32_t)pNpc->dwUnitId);
        NPCMENU_SetInteractingNpc(NULL);
        NPCMENU_NpcResyncVendorInventory(pNpc->dwUnitId);
        return;
    }

    NPCMENU_BuildMainMenu();
    if (bAlreadyInteracting)
        return;

    nSpeechKind = 0;
    for (nEntry = 0; nEntry < gnNpcMenuEntryCount; ++nEntry)
    {
        if ((int32_t)gNpcMenuEntries[nEntry].nMonsterClassId == nClassId)
        {
            if (gNpcMenuEntries[nEntry].unk0x12 == 1)
            {
                gNpcMenuEntries[nEntry].unk0x12 = 0;
                nSpeechKind = 2;
            }
            if (gNpcMenuEntries[nEntry].bAborted != 1)
                return;
            gNpcMenuEntries[nEntry].bAborted = 0;
            break;
        }
    }

    nSoundId = OBJECT_PickNpcSpeechSound(pNpc, nSpeechKind);
    if (!nSoundId)
        return;
    UNITSND_StopMonsterSkillSounds(pNpc);
    sgUIState.pNpcGreetingSound = SOUNDINST_PlaySound(nSoundId, CUNIT_GetPlayer(), 0, 1, 0);
    if (nSpeechKind != 2)
    {
        sgUIState.pNpcGreetingSound = 0;
        return;
    }
    MSG_SendCmdWord(0x4D, (int16_t)nClassId);
    sgUIState.bNpcGreetingIsOneShot = 1;
}

//1.10f: D2Client.0x6FAF90D0
void __cdecl NPCMENU_BuildInterruptDialog(void)
{
    D2UnitStrc *pNpc;
    D2UnitStrc *pPlayer;
    D2CoordStrc tClientCoords;
    int32_t nViewLeft;
    int32_t nViewTop;
    int32_t nViewRight;
    int32_t nViewBottom;
    int32_t nOriginX;
    int32_t nOriginY;
    RECT rcView;
    int32_t nNameLen;
    int32_t i;
    uint16_t ch;
    uint16_t wNewsTblIds[5];
    uint32_t nRoll;

    if (!NPCMENU_NPC_GetActiveNpcUnit())
    {
        NPCMENU_NpcCloseInteraction();
        return;
    }
    if (sgUIState.hDialogInterrupt)
    {
        FOG_DisplayAssert((uint32_t)"sgUIState.hDialogInterrupt == NULL", (uint32_t)szSourceFile, 3276);
        exit(-1);
    }

    tClientCoords.nX = 0;
    tClientCoords.nY = 0;
    if (NPCMENU_NPC_GetActiveNpcUnit())
    {
        pNpc = NPCMENU_NPC_GetActiveNpcUnit();
        D2COMMON_UNITS_GetClientCoords(pNpc, &tClientCoords);
        VIEW_GetRect(gpView, &nViewLeft, &nViewTop, &nViewRight, &nViewBottom);
        VIEW_GetOrigin(gpView, &nOriginX, &nOriginY);
        SetRect(&rcView, nOriginX, nOriginY, nViewRight - nViewLeft + nOriginX, nOriginY - nViewTop + nViewBottom);
        tClientCoords.nY += -150 - rcView.top;
        tClientCoords.nX -= rcView.left;
        if (tClientCoords.nY < 20)
            tClientCoords.nY = 20;
    }
    sgUIState.hDialogInterrupt = DIALOG_Create(tClientCoords.nX, tClientCoords.nY, (DIALOGCALLBACK)NPCMENU_OnInterruptCancel, (DIALOGCALLBACK)NPCMENU_OnInterruptClickAway, 0, 0, 1, 0, 0, 0, 1);
    DIALOG_SetHighlight(sgUIState.hDialogInterrupt, 1);
    {
        Unicode wszTitle[80];

        Unicode::strcpy(wszTitle, CUNIT_GetUnitName(NPCMENU_NPC_GetActiveNpcUnit()));
        nNameLen = Unicode::strlen(wszTitle);
        //  Unicode::toLower, inlined: only the Latin-1 range is mapped.
        for (i = 0; i < nNameLen; ++i)
        {
            ch = wszTitle[i].ch;
            if (ch < 0x100u)
                ch = UnicodeTables::toLowerTable()[wszTitle[i].ch];
            wszTitle[i].ch = ch;
        }
        DIALOG_AddLine(sgUIState.hDialogInterrupt, wszTitle, 21, 0, 4, 1, NULL, 0);
    }
    sgUIState.CurrentState = 1;

    wNewsTblIds[0] = STR_IDX_3386_NPCMenuNews0;
    wNewsTblIds[1] = STR_IDX_3387_NPCMenuNews1;
    wNewsTblIds[2] = STR_IDX_3388_NPCMenuNews2;
    wNewsTblIds[3] = STR_IDX_3389_NPCMenuNews3;
    wNewsTblIds[4] = STR_IDX_3390_NPCMenuNews4;
    pPlayer = CUNIT_GetPlayer();
    nRoll = (uint32_t)SEED_RollRandomNumber(&pPlayer->pSeed);
    DIALOG_AddLine(sgUIState.hDialogInterrupt, D2LANG_GetStringFromTblIndex((D2C_StringIndices)wNewsTblIds[nRoll % 5]), 15, 0, 0, 1, (DIALOGCALLBACK)NPCMENU_OnInterruptNewsClicked, 1);
    DIALOG_AddLine(sgUIState.hDialogInterrupt, D2LANG_GetStringFromTblIndex(STR_IDX_4142_lowercasecancel), 15, 0, 0, 1, (DIALOGCALLBACK)NPCMENU_OnInterruptCancel, 1);
    DIALOG_SetSelectedLine(sgUIState.hDialogInterrupt, 1);
}

//  The conversation's doors: the bodies of four server packets (0x28, 0x62, 0x8A and the quest ids 3 and 6 of 0x50) and the callback one of
//  them hands out. Each packet handler is the original's and a few bytes long, reached from the packet table by packet id.

static_assert(MONSTER_ANCIENTSTATUE1 == 0x219 && MONSTER_ANCIENTSTATUE2 == 0x21A
              && MONSTER_ANCIENTSTATUE3 == 0x21B && MONSTER_DREHYAICED == 0x20F,
              "BeginInteraction: cmp eax, 219h / 21Ah / 21Bh / 20Fh at 0x6FAF849F..0x6FAF84B4");
static_assert(MONSTER_ACT5POW == 0x216 && MONSTER_ACT2GUARD2 == 0x14B,
              "ShowNpcAlert: cmp dword ptr [esi+4], 216h / 14Bh at 0x6FAFBE88 and 0x6FAFBEFA");
static_assert(QFLAG_CUSTOM4 == 8, "ShowNpcAlert: push 8 as the quest state at 0x6FAFBF29");
static_assert(UNIT_OBJECT == 2, "OnInteractionStop: the jump table's cases 2, 4 and 6 at 0x6FAF802F");
static_assert(offsetof(D2GSPacketSrv8A, nUnitType) == 1 && offsetof(D2GSPacketSrv8A, dwUnitGUID) == 2,
              "ShowNpcAlert: mov dl, [ecx+1] / mov ecx, [ecx+2] at 0x6FAFBE73");
static_assert(offsetof(D2GSPacketSrv50, nQuestId) == 1,
              "OnNpcCmdPacket: cmp word ptr [esi+1], 3 at 0x6FAF7F65");
static_assert(offsetof(D2MonStatsTxt, nMonStatsFlags) == 0x0C,
              "[eax+0Dh] is nMonStatsFlags[1]: 0x6FAF7A36, 0x6FAF848B, 0x6FAF889F");
static_assert(OVERLAY_NPCALERT == 72,
              "NPCMENU_ClearNpcAlertOverlay's `push 48h` (0x6FAF7A46) and "
              "NPCMENU_ShowNpcAlert's `mov edx, 48h` (0x6FAFBEEE)");

//1.10f: D2Client.0x6FAF7A00
//  A CUNIT_ForEachMonsterUnit callback: overlay 72 off a monster whose monstats record has bit 8, npc (MONSTATSFLAGINDEX_NPC, byte 1 of the
//  flag dword at mask index 0).
void __fastcall NPCMENU_ClearNpcAlertOverlay(D2UnitStrc *pUnit)
{
    D2MonStatsTxt *pMonStatsTxt;

    if (pUnit)
    {
        pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);
        if (pMonStatsTxt && (gdwBitMasks[0] & pMonStatsTxt->nMonStatsFlags[1]))
            GFX_RemoveOverlay(pUnit, 0, OVERLAY_NPCALERT);
    }
}

//1.10f: D2Client.0x6FAF7F40
//  Packet 0x50, quest ids 3 and 6. On quest id 3 with a conversation live it raises gbNpcQuestMsgPending, tracing its own line first when the
//  flag is already up, and stores the id; on 6 it traces. The flag's only other reference is this function's own test, and the word has no
//  reader at all, and the server never sends either id: a message the client was ready for, written as the bytes have it. The original
//  inlines NPCMENU_NPC_GetActiveNpcUnit once here.
void __fastcall NPCMENU_OnNpcCmdPacket(D2GSPacketSrv50 *ptCmd)
{
    if (!ptCmd)
    {
        FOG_DisplayAssert((uint32_t)"ptCmd", (uint32_t)szSourceFile, 3872);
        exit(-1);
    }
    if (ptCmd->nQuestId == 3)
    {
        if (NPCMENU_NPC_GetActiveNpcUnit())
        {
            if (sgUIState.bNpcQuestMsgPending == 1)
                FOG_Trace("%d %s\n", 3884, szSourceFile);
            sgUIState.bNpcQuestMsgPending = 1;
            sgUIState.wNpcQuestMsgId = ptCmd->nQuestId;
        }
    }
    else
    {
        FOG_Trace("%d %s\n", 3893, szSourceFile);
    }
}

//1.10f: D2Client.0x6FAF8000
//  Packet 0x62, SCMD_INTERACTION_STOP. The monster arm is NPCMENU_EndInteractionByUiState with a trace in three of its arms: the shop arm is
//  NPCMENU_NpcShopCloseAndEndInteraction, its flag raised as or [esi+0C4h], ebx with ebx 2, and the two speech arms share one tail with each
//  string pushed before the jump. Unit types 2, 4 and 6 close every panel through a tail jump; 3, 5 and anything past 6 do nothing; 6 is never
//  looked up.
void __fastcall NPCMENU_OnInteractionStop(uint8_t nUnitType, int32_t dwGuid)
{
    D2UnitStrc *pUnit;
    int32_t nState;

    pUnit = NULL;
    if (nUnitType < 6)
        pUnit = CUNIT_FindUnitByGuidAndType((uint32_t)dwGuid, (D2C_UnitTypes)nUnitType);

    switch (nUnitType)
    {
    case UNIT_MONSTER:
        NPCMENU_SetUnitFlag(pUnit, UNITFLAG_TARGETABLE, TRUE);
        nState = (int32_t)sgUIState.CurrentState;
        if (nState == 1)
        {
            sgUIState.bNpcSpeechActive = 0;
            FOG_Trace("SCMD_INTERACTION_STOP while menus active");
            NPCMENU_NpcCloseInteraction();
        }
        else if (nState >= 3 && nState <= 6)
        {
            NPCMENU_NpcShopCloseAndEndInteraction();
            INV_TradeResetPanelState();
            FOG_Trace("SCMD_INTERACTION_STOP");
        }
        else if (nState == 7)
        {
            USEITEM_AnvilCloseOnInteractionStop();
        }
        else if (sgUIState.bNpcSpeechActive)
        {
            sgUIState.bNpcSpeechActive = 0;
            FOG_Trace("SCMD_INTERACTION_STOP while text active");
            NPCMENU_NpcCloseInteraction();
        }
        NPCMENU_CloseAllDialogs();
        sgUIState.CurrentState = 0;
        return;

    case UNIT_OBJECT:
    case UNIT_ITEM:
    case 6:     //  not a unit type, and not looked up above
        UI_CloseAllPanels(1, 0);
        return;

    default:
        return;
    }
}

//  No address in the original: BeginInteraction's interrupt arm from the state store on (0x6FAF8790..0x6FAF8888), a function of the source's
//  inlined once: the state to 1 and the transaction to 0, the gold cached, panels closed, an assert that no NPC is active
//  ("UINPCMenuGetActive() == NULL", line 3372), the NPC made active and untargetable, UI_NPCMENU toggled (NPCMENU_NpcCloseInteraction_Dup2 if
//  refused), NPCMENU_BuildInterruptDialog, and a kind-1 speech sound if the NPC's entry was aborted. The name is ours, and so is where it
//  starts: whether the NPCMENU_NPC_GetActiveNpcUnit test in front of it was the helper's or the caller's, the bytes cannot say.
static void NPCMENU_OpenInterruptMenu(D2UnitStrc *pNpc)
{
    int32_t nEntry;
    D2C_Sounds nSoundId;

    sgUIState.CurrentState = 1;
    sgUIState.Transaction = 0;
    sgUIState.nCachedPlayerGold = D2COMMON_STATLIST_UnitGetStatValue(CUNIT_GetPlayer(), STAT_GOLD, 0);
    UI_CloseAllPanels(0, 0);
    GAME_ResetClickState();
    if (NPCMENU_NPC_GetActiveNpcUnit())
    {
        FOG_DisplayAssert((uint32_t)"UINPCMenuGetActive() == NULL", (uint32_t)szSourceFile, 3372);
        exit(-1);
    }
    NPCMENU_SetInteractingNpc((int32_t *)pNpc);
    NPCMENU_SetUnitFlag(NPCMENU_NPC_GetActiveNpcUnit(), UNITFLAG_TARGETABLE, FALSE);
    if (!UI_SetUIToggle(UI_NPCMENU, UI_TOGGLE, 0))
    {
        NPCMENU_NpcCloseInteraction_Dup2();
        return;
    }
    NPCMENU_BuildInterruptDialog();

    //  BUG: on a miss this is entry 0, Akara's. REACHABLE: act2guard4 (377) and act2guard5 (378) have no entry in gNpcMenuEntries.
    nEntry = NPCMENU_FindEntryIndexForNpc();
    if (gNpcMenuEntries[nEntry].bAborted)
    {
        nSoundId = OBJECT_PickNpcSpeechSound(NPCMENU_NPC_GetActiveNpcUnit(), 1);
        if (nSoundId)
        {
            UNITSND_StopMonsterSkillSounds(pNpc);
            SOUNDINST_PlaySound(nSoundId, CUNIT_GetPlayer(), 0, 1, 0);
        }
        gNpcMenuEntries[nEntry].bAborted = 0;
    }
}

//1.10f: D2Client.0x6FAF8250
//  Packet 0x28. Type 6 carries only a quest record: 0x60 bytes into gpNpcQuestRecord.
//  A unit the client cannot find: the player's queued-command dwords cleared, client packet 0x30 with the GUID and the packet's byte +6 as its
//  unit type, and the vendor inventory resynced. The server reads only the GUID out of 0x30, so nAbortUnitType names where the byte goes and
//  no more.
//  Otherwise every monster loses the npcalert overlay (the callback above), the NPC is made targetable, the quest record is copied, the
//  descriptor table is re-primed -- records 11, 13, 15, 21 and 43 to two rows, 3 and 39 to three, 41 to four, 43 back to three while its row 1
//  is the resurrect row, record 21's row 1 emptied, the five Cains' identify rows put back -- and client packet 0x2F goes. Then the first arm
//  that matches:
//    * gbNpcSkipMenuOnInteract: the speech flag, the interacting NPC, the NPC untargetable. Dead in the shipped build: nothing sets the flag;
//    * an item on the cursor: the interaction set and ended at once;
//    * a menu node of type 1 or 2: its line plays -- the popup, the speech ring, the follow-up, packet 0x31, TEXT_SetCallback with
//      NPCMENU_ChooseNextSpeech;
//    * no node at all: an interactable NPC gets the main menu or, when it is speech-only, the interaction set and ended; an NPC that is not
//      interactable ends the last conversation's GUID (the second BUG note below);
//    * type 0 nodes only: the interrupt dialog, or NPCMENU_NpcCloseInteraction_Dup2 when a conversation is already live.
//  An interactable NPC turns to face the player, except the three Ancient statues and 527 drehya (iced); in the last two arms the player turns
//  to face the NPC, except 527, and D2Common #10153 runs on the NPC's dynamic path. gbNpcVendorGamble is cleared on the way out of every arm
//  after the cursor test except the speech-only one, whose UI_SetUIToggle is the cursor arm's own, and neither clears it.
//  The original inlines PLAYER_ClearQueuedCommand, NPCMENU_NpcResyncVendorInventory (with NPCMENU_NpcFreeVendorInventory called),
//  NPCMENU_SetInteractingNpc, NPCMENU_NpcEndInteraction (its helpers called), NPCMENU_SetUnitFlag, the follow-up search
//  (NPCMENU_FindFollowUpSpeech), DATATBLS_GetMonStatsTxtRecord and NPCMENU_NPC_GetActiveNpcUnit; all are written as calls.
//  BUG: the arm for an NPC that cannot be talked to ends the wrong conversation: with no node and no interact bit in the NPC's monstats record,
//  NPCMENU_NpcEndInteraction is handed gNpcUnitGUID (0x6FAF8913), which nothing on this path has written -- the last conversation's, left behind
//  by SetInteractingNpc(NULL). UNREACHABLE: the server sends 0x28 with a unit only for an NPC whose record carries the interact bit, the one
//  tested at 0x6FAF88A2.
//  BUG: the interrupt arm reads Akara's abort flag for the two palace guards. NPCMENU_FindEntryIndexForNpc answers 0 on a miss, and of the
//  monstats classes with interact set the two that none of the 46 records of gNpcMenuEntries holds are 377 act2guard4 and 378 act2guard5;
//  both reach this arm as a type 0 node, so the opener reads entry 0's flag, Akara's (148), and clears it. REACHABLE.
void __fastcall NPCMENU_BeginInteraction(uint8_t nUnitType, int32_t dwNpcGUID, uint8_t *pQuestData, uint8_t nAbortUnitType)
{
    D2UnitStrc *pNpc;
    D2TextHeaderStrc *pTextHeader;
    D2MonStatsTxt *pMonStatsTxt;
    uint16_t nSpeechTblId;
    uint16_t nNodeSpeechTblId;
    int32_t nClassId;

    if (nUnitType == 6)
    {
        D2COMMON_QUESTRECORD_CopyBufferToRecord(sgUIState.pNpcQuestRecord, pQuestData, 0x60, FALSE);
        return;
    }

    pNpc = CUNIT_FindUnitByGuidAndType((uint32_t)dwNpcGUID, (D2C_UnitTypes)nUnitType);
    if (!pNpc)
    {
        PLAYER_ClearQueuedCommand(CUNIT_GetPlayer());
        MSG_SendCmdUnit(0x30, nAbortUnitType, dwNpcGUID);
        NPCMENU_NpcResyncVendorInventory((uint32_t)dwNpcGUID);
        return;
    }

    CUNIT_ForEachMonsterUnit(NPCMENU_ClearNpcAlertOverlay);
    NPCMENU_SetUnitFlag(pNpc, UNITFLAG_TARGETABLE, TRUE);
    D2COMMON_QUESTRECORD_CopyBufferToRecord(sgUIState.pNpcQuestRecord, pQuestData, 0x60, FALSE);

    NPCMENU_SetRowCount2(11);
    NPCMENU_SetRowCount2(13);
    NPCMENU_SetRowCount2(15);
    NPCMENU_SetRowCount3(3);
    NPCMENU_SetRowCount2(43);
    if (gNpcMenuDescriptors[43].pfnHandler[1] == (void *)NPCMENU_OnResurrectMercClicked)
        gNpcMenuDescriptors[43].nRows = 3;
    NPCMENU_SetRowCount3(39);
    NPCMENU_SetRowCount4(41);
    NPCMENU_SetRowCount2(21);
    gNpcMenuDescriptors[21].wTblIdx[1] = 0;
    gNpcMenuDescriptors[21].pfnHandler[1] = NULL;
    NPCMENU_EnableIdentifyRow(16);
    NPCMENU_EnableIdentifyRow(17);
    NPCMENU_EnableIdentifyRow(18);
    NPCMENU_EnableIdentifyRow(19);
    NPCMENU_EnableIdentifyRow(38);

    MSG_SendCmdUnit(0x2F, nUnitType, dwNpcGUID);
    GAME_ResetClickState();

    if (sgUIState.bNpcSkipMenuOnInteract)
    {
        sgUIState.bNpcSpeechActive = 1;
        NPCMENU_SetInteractingNpc((int32_t *)pNpc);
        NPCMENU_SetUnitFlag(pNpc, UNITFLAG_TARGETABLE, FALSE);
        return;
    }

    pTextHeader = TEXT_GetMessageListHead();
    if (!pTextHeader)
    {
        FOG_DisplayAssert((uint32_t)"hTextList != NULL", (uint32_t)szSourceFile, 4016);
        exit(-1);
    }
    nSpeechTblId = (uint16_t)D2COMMON_TEXT_GetStringIdOfMenuType1Or2Node(pTextHeader, 0);

    pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pNpc->dwClassId);
    if (pMonStatsTxt && (gdwBitMasks[1] & pMonStatsTxt->nMonStatsFlags[1]))
    {
        nClassId = pNpc->dwClassId;
        if (nClassId != MONSTER_ANCIENTSTATUE1 && nClassId != MONSTER_ANCIENTSTATUE2 && nClassId != MONSTER_ANCIENTSTATUE3 && nClassId != MONSTER_DREHYAICED)
        {
            CUNIT_TurnUnitToFacePlayer(pNpc);
        }
    }

    if (D2COMMON_INVENTORY_GetCursorItem(CUNIT_GetPlayer()->pInventory))
    {
        NPCMENU_SetInteractingNpc((int32_t *)pNpc);
        NPCMENU_NpcEndInteraction(pNpc->dwUnitId);
        UI_SetUIToggle(UI_NPCMENU, UI_TURNOFF, 0);
        return;
    }

    sgUIState.nNpcSpeechesPlayed = 0;
    if (nSpeechTblId != 0xFFFF)
    {
        UI_CloseAllPanels(0, 0);
        sgUIState.wNpcSpeechTblId = nSpeechTblId;
        TEXT_ShowSpeechPopup(pNpc, (int16_t)nSpeechTblId, 1);
        if (sgUIState.nNpcSpeechesPlayed < 9)
        {
            sgUIState.wNpcSpeechesPlayed[sgUIState.nNpcSpeechesPlayed] = (int16_t)nSpeechTblId;
            ++sgUIState.nNpcSpeechesPlayed;
        }
        sgUIState.wNpcNextSpeechTblId = NPCMENU_FindFollowUpSpeech(nSpeechTblId);
        MSG_SendCmdUnit(0x31, (int32_t)pNpc->dwUnitId, nSpeechTblId);
        sgUIState.bNpcSpeechActive = 1;
        TEXT_SetCallback((void *)NPCMENU_ChooseNextSpeech, 0);
        NPCMENU_SetInteractingNpc((int32_t *)pNpc);
        sgUIState.bNpcVendorGamble = 0;
        return;
    }

    nNodeSpeechTblId = (uint16_t)D2COMMON_TEXT_GetStringIdOfMenuType0Or2Node(pTextHeader, 0);
    if (pNpc->dwClassId != MONSTER_DREHYAICED)
        CUNIT_TurnPlayerToFaceUnit(pNpc);
    if (pNpc->pDynamicPath)
        D2COMMON_10153(pNpc->pDynamicPath);

    if (nNodeSpeechTblId == 0xFFFF)
    {
        pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pNpc->dwClassId);
        if (pMonStatsTxt && (gdwBitMasks[1] & pMonStatsTxt->nMonStatsFlags[1]))
        {
            if (NPCMENU_IsSpeechOnlyNpc((D2C_MonsterIds)pNpc->dwClassId))
            {
                NPCMENU_SetInteractingNpc((int32_t *)pNpc);
                NPCMENU_NpcEndInteraction(pNpc->dwUnitId);
                UI_SetUIToggle(UI_NPCMENU, UI_TURNOFF, 0);
                return;
            }
            INV_ResetItemDescState();
            UI_CloseAllPanels(0, 0);
            NPCMENU_OpenMainMenu(pNpc, 0);
            sgUIState.bNpcVendorGamble = 0;
            return;
        }
        //  BUG: the last conversation's GUID, not this NPC's. UNREACHABLE: the server sends 0x28 with a unit only for an NPC with the interact bit.
        NPCMENU_NpcEndInteraction((uint32_t)sgUIState.NpcUnitGUID);
        UI_SetUIToggle(UI_NPCMENU, UI_TURNOFF, 0);
        sgUIState.bNpcVendorGamble = 0;
        return;
    }

    INV_ResetItemDescState();
    UI_CloseAllPanels(0, 0);
    sgUIState.wNpcSpeechTblId = nNodeSpeechTblId;
    sgUIState.wNpcNextSpeechTblId = NPCMENU_FindFollowUpSpeech(nNodeSpeechTblId);
    GAME_ResetClickState();
    if (NPCMENU_NPC_GetActiveNpcUnit())
        NPCMENU_NpcCloseInteraction_Dup2();
    else
        NPCMENU_OpenInterruptMenu(pNpc);
    sgUIState.bNpcVendorGamble = 0;
}

//1.10f: D2Client.0x6FAFBE70
//  Packet 0x8A. 534 act5pow, the caged soldiers of Act 5, get a line instead of the mark: 4607 wussie_help_me when
//  MONSTER_GetHirelingOwnerGUID answers -1, else 4603 wussie_cheer_1. Any other NPC but the one being talked to gets overlay 72, npcalert;
//  331 act2guard2, the palace guard, also plays 3983 guard_halt while no conversation is live, quest 12 (QUESTSTATEFLAG_A2Q4) has neither
//  QFLAG_CUSTOM4 nor QFLAG_REWARDPENDING, and NPCMENU_NoteOpenedEunuchObject finds no opened eunuch. The original inlines
//  NPCMENU_NPC_GetActiveNpcUnit twice here.
void __fastcall NPCMENU_ShowNpcAlert(D2GSPacketSrv8A *pPacket)
{
    D2UnitStrc *pUnit;

    pUnit = CUNIT_FindUnitByGuidAndType(pPacket->dwUnitGUID, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pUnit)
        return;

    if (pUnit->dwClassId == MONSTER_ACT5POW)
    {
        if (MONSTER_GetHirelingOwnerGUID(pUnit) == -1)
            SOUNDINST_PlaySound(SOUND_WUSSIE_HELP_ME, pUnit, 0, 0, 0);       //  wussie_help_me
        else
            SOUNDINST_PlaySound(SOUND_WUSSIE_CHEER_1, pUnit, 0, 0, 0);       //  wussie_cheer_1
        return;
    }
    if (pUnit == NPCMENU_NPC_GetActiveNpcUnit())
        return;

    GFX_AddOverlay(pUnit, OVERLAY_NPCALERT, OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
    if (pUnit->dwClassId == MONSTER_ACT2GUARD2 && !NPCMENU_NPC_GetActiveNpcUnit() && sgUIState.pNpcQuestRecord && !D2COMMON_QUESTRECORD_GetQuestState(sgUIState.pNpcQuestRecord, 12, QFLAG_CUSTOM4)
        && !D2COMMON_QUESTRECORD_GetQuestState(sgUIState.pNpcQuestRecord, 12, QFLAG_REWARDPENDING))
    {
        gbFoundOpenedEunuch = 0;
        CUNIT_ForEachObjectUnit(NPCMENU_NoteOpenedEunuchObject);
        if (!gbFoundOpenedEunuch)
            SOUNDINST_PlaySound(SOUND_GUARD_HALT, pUnit, 0, 0, 0);       //  guard_halt
    }
}

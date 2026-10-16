//  npcmenu.h -- the globals of npcmenu.cpp: the NPC menu descriptor, entry, speech-line and quest-unlock tables, the speech pairs, the transaction
//  and hire-list globals, and the state block sgUIState (one packed struct, D2NpcMenuUIStateStrc, 348 bytes).
//  Each is a C++17 inline variable, so every file that includes this header shares one object. Address range 0x6FB7A518..0x6FBB5DAC.
//  The 32 speech-line tables that gNpcMenuEntries points at (pSpeechLineTable) tile 0x6FB7A5C0..0x6FB7B978 with zero padding between them. Each is
//  D2NpcSpeechLineStrc[n], n being the largest nSpeechLineCount of the entries that point at it, named after the first NPC whose entry points at it.
//  Five are shared: gWarriv1SpeechLines (three entries with a count of 0), gCain1SpeechLines (cain5's too), gFaraSpeechLines (izualghost's and
//  halbu's), gTyrael1SpeechLines (tyrael2's, tyrael3's, jamella's and halbu's) and gDrehyaSpeechLines (drehyaiced's and the three ancient statues').
//  Include it after D2Client.h, which declares every function named here.
#pragma once

//  D2Client 0x6FB7BD70
//  The NPC interaction menu, one record per talkable NPC: 48 records of 39 bytes, ending at 0x6FB7C4C0 where gnNpcMenuDescriptorCount begins.
//    +0x00 int    nMonsterId     monstats.txt hcIdx
//    +0x04 int    nRows          buttons + 1
//    +0x08 uint16 wTblIdx[5]     button labels, tbl indices, 0-terminated
//    +0x12 void * pfnHandler[5]  one handler per label, same order
//    +0x26 uint8  bEnabled       0 for six: cain1, tyrael1, drehyaiced and the three ancientstatue rows
//  At most three slots are used. Eight button kinds, each with exactly one handler: 3381 talk, 3396 trade, 3334 trade/repair (same handler),
//  3398 gamble, 4020 Identify Items, 3397 hire, 3383 go west, 3385 sail west.
//  The handler slots are compared by address: NPCMENU_ToggleResurrectMercRow and NPCMENU_ShowTransactionResult look for
//  NPCMENU_OnResurrectMercClicked and NPCMENU_DrawHireMercenaryList in a record's pfnHandler.
inline D2NpcMenuDescStrc gNpcMenuDescriptors[48] =
{
    {   // [0]
        .nMonsterId = 148,
        .nRows = 3,
        .wTblIdx = { 3381, 3396, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnTradeClicked, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [1]
        .nMonsterId = 176,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [2]
        .nMonsterId = 146,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 0,
    },
    {   // [3]
        .nMonsterId = 154,
        .nRows = 3,
        .wTblIdx = { 3381, 3334, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnTradeClicked, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [4]
        .nMonsterId = 177,
        .nRows = 3,
        .wTblIdx = { 3381, 3396, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnTradeClicked, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [5]
        .nMonsterId = 199,
        .nRows = 4,
        .wTblIdx = { 3381, 3396, 3398, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnTradeClicked, (void *)&NPCMENU_OnGambleClicked, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [6]
        .nMonsterId = 200,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [7]
        .nMonsterId = 147,
        .nRows = 4,
        .wTblIdx = { 3381, 3396, 3398, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnTradeClicked, (void *)&NPCMENU_OnGambleClicked, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [8]
        .nMonsterId = 198,
        .nRows = 3,
        .wTblIdx = { 3381, 3397, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_DrawHireMercenaryList, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [9]
        .nMonsterId = 201,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [10]
        .nMonsterId = 202,
        .nRows = 3,
        .wTblIdx = { 3381, 3396, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnTradeClicked, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [11]
        .nMonsterId = 150,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [12]
        .nMonsterId = 178,
        .nRows = 3,
        .wTblIdx = { 3381, 3334, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnTradeClicked, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [13]
        .nMonsterId = 155,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [14]
        .nMonsterId = 175,
        .nRows = 3,
        .wTblIdx = { 3381, 3383, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnTravelWestClicked, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [15]
        .nMonsterId = 210,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [16]
        .nMonsterId = 244,
        .nRows = 3,
        .wTblIdx = { 3381, 4020, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnIdentifyClicked, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [17]
        .nMonsterId = 265,
        .nRows = 3,
        .wTblIdx = { 3381, 4020, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnIdentifyClicked, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [18]
        .nMonsterId = 245,
        .nRows = 3,
        .wTblIdx = { 3381, 4020, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnIdentifyClicked, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [19]
        .nMonsterId = 246,
        .nRows = 3,
        .wTblIdx = { 3381, 4020, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnIdentifyClicked, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [20]
        .nMonsterId = 251,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 0,
    },
    {   // [21]
        .nMonsterId = 367,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [22]
        .nMonsterId = 257,
        .nRows = 2,
        .wTblIdx = { 3334, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTradeClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [23]
        .nMonsterId = 405,
        .nRows = 3,
        .wTblIdx = { 3396, 3398, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTradeClicked, (void *)&NPCMENU_OnGambleClicked, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [24]
        .nMonsterId = 252,
        .nRows = 4,
        .wTblIdx = { 3381, 3397, 3396, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_DrawHireMercenaryList, (void *)&NPCMENU_OnTradeClicked, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [25]
        .nMonsterId = 253,
        .nRows = 3,
        .wTblIdx = { 3381, 3334, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnTradeClicked, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [26]
        .nMonsterId = 254,
        .nRows = 4,
        .wTblIdx = { 3381, 3396, 3398, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnTradeClicked, (void *)&NPCMENU_OnGambleClicked, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [27]
        .nMonsterId = 255,
        .nRows = 3,
        .wTblIdx = { 3381, 3396, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnTradeClicked, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [28]
        .nMonsterId = 406,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [29]
        .nMonsterId = 257,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [30]
        .nMonsterId = 264,
        .nRows = 3,
        .wTblIdx = { 3381, 3385, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnSailWestClicked, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [31]
        .nMonsterId = 297,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [32]
        .nMonsterId = 266,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [33]
        .nMonsterId = 331,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [34]
        .nMonsterId = 408,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [35]
        .nMonsterId = 378,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [36]
        .nMonsterId = 377,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [37]
        .nMonsterId = 521,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [38]
        .nMonsterId = 520,
        .nRows = 3,
        .wTblIdx = { 3381, 4020, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnIdentifyClicked, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [39]
        .nMonsterId = 511,
        .nRows = 3,
        .wTblIdx = { 3381, 3334, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnTradeClicked, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [40]
        .nMonsterId = 513,
        .nRows = 3,
        .wTblIdx = { 3381, 3396, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnTradeClicked, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [41]
        .nMonsterId = 512,
        .nRows = 4,
        .wTblIdx = { 3381, 3396, 3398, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnTradeClicked, (void *)&NPCMENU_OnGambleClicked, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [42]
        .nMonsterId = 514,
        .nRows = 3,
        .wTblIdx = { 3381, 3398, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, (void *)&NPCMENU_OnGambleClicked, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [43]
        .nMonsterId = 515,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 1,
    },
    {   // [44]
        .nMonsterId = 527,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 0,
    },
    {   // [45]
        .nMonsterId = 537,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 0,
    },
    {   // [46]
        .nMonsterId = 538,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 0,
    },
    {   // [47]
        .nMonsterId = 539,
        .nRows = 2,
        .wTblIdx = { 3381, 0, 0, 0, 0 },
        .pfnHandler = { (void *)&NPCMENU_OnTalkClicked, NULL, NULL, NULL, NULL },
        .bEnabled = 0,
    },
};
static_assert(sizeof(gNpcMenuDescriptors) == 1872, "gNpcMenuDescriptors: the item's 1872 bytes in the .i64");

//  D2Client 0x6FB7A518
//  8 records of 20 bytes, what turns quest progress into menu rows: { nMonsterClassId, nQuestStateFlag, nQuestState, bWantState, pfnAddRow }.
//  NPCMENU_OpenMainMenu walks it on every menu open and calls pfnAddRow for each record whose NPC matches and whose
//  QUESTRECORD_GetQuestState(pQuestRecord, nQuestStateFlag, nQuestState) equals bWantState. Count in gnNpcMenuQuestUnlockCount.
//  The complete set of quest-unlocked NPC options in the game. Class ids are monstats hcIdx values. Not const: it lives in writable .data.
inline D2NpcMenuQuestUnlockStrc gNpcMenuQuestUnlocks[8] =
{
    { MONSTER_WARRIV1,    QUESTSTATEFLAG_A1Q6, QFLAG_REWARDGRANTED, 1, NPCMENU_QuestUnlock_WarrivGoEast },
    { MONSTER_MESHIF1,   QUESTSTATEFLAG_A2Q6, QFLAG_REWARDGRANTED, 1, NPCMENU_QuestUnlock_MeshifSailEast },
    { MONSTER_KASHYA,     QUESTSTATEFLAG_A1Q2, QFLAG_REWARDGRANTED, 1, NPCMENU_QuestUnlock_KashyaHire },
    { MONSTER_CHARSI,     QUESTSTATEFLAG_A1Q3, QFLAG_REWARDPENDING, 1, NPCMENU_QuestUnlock_CharsiImbue },
    { MONSTER_LARZUK,    QUESTSTATEFLAG_A5Q1, QFLAG_REWARDPENDING, 1, NPCMENU_QuestUnlock_LarzukSockets },
    { MONSTER_DREHYA,    QUESTSTATEFLAG_A5Q4, QFLAG_REWARDPENDING, 1, NPCMENU_QuestUnlock_AnyaPersonalize },
    { MONSTER_QUAL_KEHK, QUESTSTATEFLAG_A5Q2, QFLAG_REWARDGRANTED, 1, NPCMENU_QuestUnlock_QualKehkHire },
    { MONSTER_TYRAEL2,   QUESTSTATEFLAG_A4COMPLETED, QFLAG_REWARDGRANTED, 1, NPCMENU_QuestUnlock_TyraelTravel },
};
static_assert(sizeof(gNpcMenuQuestUnlocks) == 8 * 20,
              "eight 20-byte records from 0x6FB7A518 end at gnNpcMenuQuestUnlockCount, 0x6FB7A5B8");

//  D2Client 0x6FB7A5B8
//  Number of live records in gNpcMenuQuestUnlocks.
inline int32_t gnNpcMenuQuestUnlockCount = 8;
static_assert(sizeof(gnNpcMenuQuestUnlockCount) == 4, "gnNpcMenuQuestUnlockCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7A5BC
//  The tbl string id of the dead mercenary's name, for the "Resurrect %s: %d" row of the NPC main menu. Written as a WORD by
//  NPCMENU_SetResurrectMercOffer from server packet 0x9B (the field at +1), and set to 0xFFFF by NPCMENU_Reset.
//  0xFFFF is the sentinel for "no dead mercenary" (cmp cx, 0FFFFh): NPCMENU_SetResurrectMercOffer compares the incoming word with it, and the
//  global is tested by NPCMENU_OnHireListConfirmClicked, which treats anything else as a reason to ask the player before hiring a replacement,
//  and by NPCMENU_OpenMainMenu. NPCMENU_BuildMainMenu skips the name lookup when the id is 0x421, falling back to tbl 11021.
inline D2C_StringIndices gwMercResurrectNameTblId = (D2C_StringIndices)-1;
static_assert(sizeof(gwMercResurrectNameTblId) == 2, "gwMercResurrectNameTblId: the item's 2 bytes in the .i64");

//  D2Client 0x6FB7A5C0
inline D2NpcSpeechLineStrc gKashyaSpeechLines[12] =
{
    { .nTblStringId = 24, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 25, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED },   // [1]
    { .nTblStringId = 26, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED },   // [2]
    { .nTblStringId = 27, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1Q4, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 28, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 29, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 30, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1Q3, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 31, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 32, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 33, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [9]
    { .nTblStringId = 34, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [10]
    { .nTblStringId = 35, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [11]
};
static_assert(sizeof(gKashyaSpeechLines) == 180, "gKashyaSpeechLines: the item's 180 bytes in the .i64");

//  D2Client 0x6FB7A678
inline D2NpcSpeechLineStrc gWarriv1SpeechLines[11] =
{
    { .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 1, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 3 },   // [1]
    { .nTblStringId = 2, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 3 },   // [2]
    { .nTblStringId = 3, .bHasQuestGate = 1, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1Q4, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 4, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 5, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 6, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 7, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 8, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 9, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1Q6, .nClassFilter = 7 },   // [9]
    { .nTblStringId = 10, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1Q4, .nClassFilter = 7 },   // [10]
};
static_assert(sizeof(gWarriv1SpeechLines) == 165, "gWarriv1SpeechLines: the item's 165 bytes in the .i64");

//  D2Client 0x6FB7A720
inline D2NpcSpeechLineStrc gGheedSpeechLines[9] =
{
    { .nTblStringId = 45, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 46, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 2 },   // [1]
    { .nTblStringId = 47, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 2 },   // [2]
    { .nTblStringId = 48, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1Q4, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 49, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 50, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 51, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 52, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1Q6, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 53, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [8]
};
static_assert(sizeof(gGheedSpeechLines) == 135, "gGheedSpeechLines: the item's 135 bytes in the .i64");

//  D2Client 0x6FB7A7A8
inline D2NpcSpeechLineStrc gCain1SpeechLines[7] =
{
    { .nTblStringId = 54, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 54, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [1]
    { .nTblStringId = 54, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [2]
    { .nTblStringId = 55, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 56, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 57, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 58, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [6]
};
static_assert(sizeof(gCain1SpeechLines) == 105, "gCain1SpeechLines: the item's 105 bytes in the .i64");

//  D2Client 0x6FB7A818
inline D2NpcSpeechLineStrc gCain2SpeechLines[7] =
{
    { .nTblStringId = 297, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 297, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [1]
    { .nTblStringId = 297, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [2]
    { .nTblStringId = 298, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 299, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 300, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 301, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [6]
};
static_assert(sizeof(gCain2SpeechLines) == 105, "gCain2SpeechLines: the item's 105 bytes in the .i64");

//  D2Client 0x6FB7A888
inline D2NpcSpeechLineStrc gCain3SpeechLines[8] =
{
    { .nTblStringId = 458, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 458, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [1]
    { .nTblStringId = 459, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [2]
    { .nTblStringId = 460, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 461, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 462, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 463, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 464, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [7]
};
static_assert(sizeof(gCain3SpeechLines) == 120, "gCain3SpeechLines: the item's 120 bytes in the .i64");

//  D2Client 0x6FB7A900
inline D2NpcSpeechLineStrc gCharsiSpeechLines[9] =
{
    { .nTblStringId = 36, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 37, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 4 },   // [1]
    { .nTblStringId = 38, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 4 },   // [2]
    { .nTblStringId = 39, .bHasQuestGate = 1, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1Q4, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 40, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 41, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 42, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 43, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 44, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [8]
};
static_assert(sizeof(gCharsiSpeechLines) == 135, "gCharsiSpeechLines: the item's 135 bytes in the .i64");

//  D2Client 0x6FB7A988
inline D2NpcSpeechLineStrc gAkaraSpeechLines[13] =
{
    { .nTblStringId = 11, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 12, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 1 },   // [1]
    { .nTblStringId = 13, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 1 },   // [2]
    { .nTblStringId = 14, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 15, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 16, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 17, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 18, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 19, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 20, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [9]
    { .nTblStringId = 21, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [10]
    { .nTblStringId = 22, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED, .nClassFilter = 7 },   // [11]
    { .nTblStringId = 23, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A1Q6, .nClassFilter = 7 },   // [12]
};
static_assert(sizeof(gAkaraSpeechLines) == 195, "gAkaraSpeechLines: the item's 195 bytes in the .i64");

//  D2Client 0x6FB7AA50
inline D2NpcSpeechLineStrc gAtmaSpeechLines[10] =
{
    { .nTblStringId = 221, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 221, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [1]
    { .nTblStringId = 222, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [2]
    { .nTblStringId = 223, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 224, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q3, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 225, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 226, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 227, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 228, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 229, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [9]
};
static_assert(sizeof(gAtmaSpeechLines) == 150, "gAtmaSpeechLines: the item's 150 bytes in the .i64");

//  D2Client 0x6FB7AAE8
inline D2NpcSpeechLineStrc gDrognanSpeechLines[12] =
{
    { .nTblStringId = 285, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 286, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 1 },   // [1]
    { .nTblStringId = 287, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 1 },   // [2]
    { .nTblStringId = 288, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 289, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 290, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q5, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 291, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 292, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 293, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 294, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q6, .nClassFilter = 7 },   // [9]
    { .nTblStringId = 295, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q3, .nClassFilter = 7 },   // [10]
    { .nTblStringId = 296, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [11]
};
static_assert(sizeof(gDrognanSpeechLines) == 180, "gDrognanSpeechLines: the item's 180 bytes in the .i64");

//  D2Client 0x6FB7ABA0
inline D2NpcSpeechLineStrc gGreizSpeechLines[14] =
{
    { .nTblStringId = 190, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 190, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [1]
    { .nTblStringId = 191, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [2]
    { .nTblStringId = 192, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q3, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 193, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 194, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 195, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 196, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 197, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 198, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [9]
    { .nTblStringId = 199, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q6, .nClassFilter = 7 },   // [10]
    { .nTblStringId = 200, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [11]
    { .nTblStringId = 201, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [12]
    { .nTblStringId = 202, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [13]
};
static_assert(sizeof(gGreizSpeechLines) == 210, "gGreizSpeechLines: the item's 210 bytes in the .i64");

//  D2Client 0x6FB7AC78
inline D2NpcSpeechLineStrc gElzixSpeechLines[12] =
{
    { .nTblStringId = 203, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 204, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 2 },   // [1]
    { .nTblStringId = 205, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 2 },   // [2]
    { .nTblStringId = 206, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 207, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 208, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q3, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 209, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q1, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 210, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q3, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 211, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 212, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [9]
    { .nTblStringId = 213, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [10]
    { .nTblStringId = 214, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [11]
};
static_assert(sizeof(gElzixSpeechLines) == 180, "gElzixSpeechLines: the item's 180 bytes in the .i64");

//  D2Client 0x6FB7AD30
inline D2NpcSpeechLineStrc gGeglashSpeechLines[11] =
{
    { .nTblStringId = 230, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 231, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 4 },   // [1]
    { .nTblStringId = 232, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 4 },   // [2]
    { .nTblStringId = 233, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q3, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 234, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 235, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 236, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 237, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 238, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q3, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 239, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [9]
    { .nTblStringId = 240, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [10]
};
static_assert(sizeof(gGeglashSpeechLines) == 165, "gGeglashSpeechLines: the item's 165 bytes in the .i64");

//  D2Client 0x6FB7ADD8
inline D2NpcSpeechLineStrc gJerhynSpeechLines[10] =
{
    { .nTblStringId = 253, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 253, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [1]
    { .nTblStringId = 255, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q1, .nClassFilter = 7 },   // [2]
    { .nTblStringId = 256, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q1, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 257, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 258, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q3, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 259, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 260, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 261, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q6, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 262, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [9]
};
static_assert(sizeof(gJerhynSpeechLines) == 150, "gJerhynSpeechLines: the item's 150 bytes in the .i64");

//  D2Client 0x6FB7AE70
inline D2NpcSpeechLineStrc gLysanderSpeechLines[12] =
{
    { .nTblStringId = 274, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 274, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [1]
    { .nTblStringId = 275, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [2]
    { .nTblStringId = 276, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 277, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 278, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 279, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q3, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 280, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 281, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 282, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [9]
    { .nTblStringId = 283, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q3, .nClassFilter = 7 },   // [10]
    { .nTblStringId = 284, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [11]
};
static_assert(sizeof(gLysanderSpeechLines) == 180, "gLysanderSpeechLines: the item's 180 bytes in the .i64");

//  D2Client 0x6FB7AF28
inline D2NpcSpeechLineStrc gFaraSpeechLines[11] =
{
    { .nTblStringId = 263, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 264, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 3 },   // [1]
    { .nTblStringId = 265, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 3 },   // [2]
    { .nTblStringId = 266, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 267, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q3, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 268, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 269, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 270, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 271, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 272, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [9]
    { .nTblStringId = 273, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [10]
};
static_assert(sizeof(gFaraSpeechLines) == 165, "gFaraSpeechLines: the item's 165 bytes in the .i64");

//  D2Client 0x6FB7AFD0
inline D2NpcSpeechLineStrc gMeshif1SpeechLines[12] =
{
    { .nTblStringId = 241, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 242, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED },   // [1]
    { .nTblStringId = 243, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED },   // [2]
    { .nTblStringId = 244, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q3, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 245, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 246, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q6, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 247, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 248, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 249, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 250, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [9]
    { .nTblStringId = 251, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [10]
    { .nTblStringId = 252, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [11]
};
static_assert(sizeof(gMeshif1SpeechLines) == 180, "gMeshif1SpeechLines: the item's 180 bytes in the .i64");

//  D2Client 0x6FB7B088
inline D2NpcSpeechLineStrc gWarriv2SpeechLines[7] =
{
    { .nTblStringId = 215, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 216, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [1]
    { .nTblStringId = 216, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [2]
    { .nTblStringId = 217, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 218, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2Q3, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 219, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 220, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED, .nClassFilter = 7 },   // [6]
};
static_assert(sizeof(gWarriv2SpeechLines) == 105, "gWarriv2SpeechLines: the item's 105 bytes in the .i64");

//  D2Client 0x6FB7B0F8
inline D2NpcSpeechLineStrc gMeshif2SpeechLines[11] =
{
    { .nTblStringId = 478, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 479, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 4 },   // [1]
    { .nTblStringId = 480, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 4 },   // [2]
    { .nTblStringId = 481, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 482, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 483, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 484, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 485, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 486, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 487, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [9]
    { .nTblStringId = 488, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [10]
};
static_assert(sizeof(gMeshif2SpeechLines) == 165, "gMeshif2SpeechLines: the item's 165 bytes in the .i64");

//  D2Client 0x6FB7B1A0
inline D2NpcSpeechLineStrc gHratliSpeechLines[12] =
{
    { .nTblStringId = 465, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 466, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 1 },   // [1]
    { .nTblStringId = 467, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 1 },   // [2]
    { .nTblStringId = 468, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3Q6, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 469, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 470, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 471, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 472, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 473, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 474, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [9]
    { .nTblStringId = 475, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [10]
    { .nTblStringId = 476, .bHasQuestGate = 1, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3Q7, .nClassFilter = 7 },   // [11]
};
static_assert(sizeof(gHratliSpeechLines) == 180, "gHratliSpeechLines: the item's 180 bytes in the .i64");

//  D2Client 0x6FB7B258
inline D2NpcSpeechLineStrc gAshearaSpeechLines[9] =
{
    { .nTblStringId = 490, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 491, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED },   // [1]
    { .nTblStringId = 492, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED },   // [2]
    { .nTblStringId = 493, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 494, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 495, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 496, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 497, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 498, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [8]
};
static_assert(sizeof(gAshearaSpeechLines) == 135, "gAshearaSpeechLines: the item's 135 bytes in the .i64");

//  D2Client 0x6FB7B2E0
inline D2NpcSpeechLineStrc gAlkorSpeechLines[11] =
{
    { .nTblStringId = 501, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 502, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 2 },   // [1]
    { .nTblStringId = 503, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 2 },   // [2]
    { .nTblStringId = 504, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 505, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 506, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 508, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 509, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 510, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 511, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [9]
    { .nTblStringId = 512, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [10]
};
static_assert(sizeof(gAlkorSpeechLines) == 165, "gAlkorSpeechLines: the item's 165 bytes in the .i64");

//  D2Client 0x6FB7B388
inline D2NpcSpeechLineStrc gOrmusSpeechLines[12] =
{
    { .nTblStringId = 514, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 515, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 3 },   // [1]
    { .nTblStringId = 516, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 3 },   // [2]
    { .nTblStringId = 518, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 519, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 520, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 521, .bHasQuestGate = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3Q6, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 522, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 523, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 524, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [9]
    { .nTblStringId = 525, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [10]
    { .nTblStringId = 526, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [11]
};
static_assert(sizeof(gOrmusSpeechLines) == 180, "gOrmusSpeechLines: the item's 180 bytes in the .i64");

//  D2Client 0x6FB7B43C
inline D2NpcSpeechLineStrc gTyrael1SpeechLines[3] =
{
    { .nTblStringId = 664, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 664, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [1]
    { .nTblStringId = 665, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [2]
};
static_assert(sizeof(gTyrael1SpeechLines) == 45, "gTyrael1SpeechLines: the item's 45 bytes in the .i64");

//  D2Client 0x6FB7B46C
inline D2NpcSpeechLineStrc gCain4SpeechLines[3] =
{
    { .nTblStringId = 666, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 666, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [1]
    { .nTblStringId = 667, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [2]
};
static_assert(sizeof(gCain4SpeechLines) == 45, "gCain4SpeechLines: the item's 45 bytes in the .i64");

//  D2Client 0x6FB7B4A0
inline D2NpcSpeechLineStrc gNatalyaSpeechLines[6] =
{
    { .nTblStringId = 453, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 453, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [1]
    { .nTblStringId = 454, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [2]
    { .nTblStringId = 455, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 456, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 457, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED, .nClassFilter = 7 },   // [5]
};
static_assert(sizeof(gNatalyaSpeechLines) == 90, "gNatalyaSpeechLines: the item's 90 bytes in the .i64");

//  D2Client 0x6FB7B500
inline D2NpcSpeechLineStrc gCain6SpeechLines[12] =
{
    { .nTblStringId = 20003, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 20003, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [1]
    { .nTblStringId = 20004, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [2]
    { .nTblStringId = 20005, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 20006, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 20007, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 20008, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 20009, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 20010, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 20011, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [9]
    { .nTblStringId = 20012, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [10]
    { .nTblStringId = 20013, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [11]
};
static_assert(sizeof(gCain6SpeechLines) == 180, "gCain6SpeechLines: the item's 180 bytes in the .i64");

//  D2Client 0x6FB7B5B8
inline D2NpcSpeechLineStrc gDrehyaSpeechLines[12] =
{
    { .nTblStringId = 20014, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 20014, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [1]
    { .nTblStringId = 20015, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [2]
    { .nTblStringId = 20016, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [3]
    { .nTblStringId = 20017, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 20018, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 20019, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 20020, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 20021, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 20022, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [9]
    { .nTblStringId = 20023, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [10]
    { .nTblStringId = 20024, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [11]
};
static_assert(sizeof(gDrehyaSpeechLines) == 180, "gDrehyaSpeechLines: the item's 180 bytes in the .i64");

//  D2Client 0x6FB7B670
inline D2NpcSpeechLineStrc gLarzukSpeechLines[12] =
{
    { .nTblStringId = 20025, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 20026, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED },   // [1]
    { .nTblStringId = 20027, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED },   // [2]
    { .nTblStringId = 20028, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED },   // [3]
    { .nTblStringId = 20029, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 20030, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 20031, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 20032, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 20033, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 20034, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [9]
    { .nTblStringId = 20035, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [10]
    { .nTblStringId = 20036, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [11]
};
static_assert(sizeof(gLarzukSpeechLines) == 180, "gLarzukSpeechLines: the item's 180 bytes in the .i64");

//  D2Client 0x6FB7B728
inline D2NpcSpeechLineStrc gMalahSpeechLines[16] =
{
    { .nTblStringId = 20037, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 20038, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 1 },   // [1]
    { .nTblStringId = 20040, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 1 },   // [2]
    { .nTblStringId = 20041, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 4 },   // [3]
    { .nTblStringId = 20042, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 20043, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 20044, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 20045, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 20046, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 20047, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [9]
    { .nTblStringId = 20048, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [10]
    { .nTblStringId = 20049, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [11]
    { .nTblStringId = 20050, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [12]
    { .nTblStringId = 20051, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [13]
    { .nTblStringId = 20052, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [14]
    { .nTblStringId = 20039, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 4 },   // [15]
};
static_assert(sizeof(gMalahSpeechLines) == 240, "gMalahSpeechLines: the item's 240 bytes in the .i64");

//  D2Client 0x6FB7B818
inline D2NpcSpeechLineStrc gNihlathakSpeechLines[12] =
{
    { .nTblStringId = 20053, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 20054, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 6 },   // [1]
    { .nTblStringId = 20056, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 6 },   // [2]
    { .nTblStringId = 20057, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 2 },   // [3]
    { .nTblStringId = 20058, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 20059, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 20060, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 20061, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 20062, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 20063, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [9]
    { .nTblStringId = 20064, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [10]
    { .nTblStringId = 20055, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 2 },   // [11]
};
static_assert(sizeof(gNihlathakSpeechLines) == 180, "gNihlathakSpeechLines: the item's 180 bytes in the .i64");

//  D2Client 0x6FB7B8D0
inline D2NpcSpeechLineStrc gQualKehkSpeechLines[11] =
{
    { .nTblStringId = 20065, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [0]
    { .nTblStringId = 20067, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 5 },   // [1]
    { .nTblStringId = 20068, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 3 },   // [2]
    { .nTblStringId = 20069, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 5 },   // [3]
    { .nTblStringId = 20070, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [4]
    { .nTblStringId = 20071, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [5]
    { .nTblStringId = 20072, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [6]
    { .nTblStringId = 20073, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [7]
    { .nTblStringId = 20074, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [8]
    { .nTblStringId = 20075, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 7 },   // [9]
    { .nTblStringId = 20066, .nRequiredState = 1, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED, .nClassFilter = 3 },   // [10]
};
static_assert(sizeof(gQualKehkSpeechLines) == 165, "gQualKehkSpeechLines: the item's 165 bytes in the .i64");

//  D2Client 0x6FB7B978
//  D2NpcMenuEntryStrc[46]: packed 22-byte records, one per NPC the player has spoken to, keyed by monster class id at +0x00;
//  gnNpcMenuEntryCount says how many are live.
inline D2NpcMenuEntryStrc gNpcMenuEntries[46] =
{
    { .nMonsterClassId = 148, .pSpeechLineTable = gAkaraSpeechLines, .nSpeechLineCount = 13, .bAborted = 1 },   // [0]
    { .nMonsterClassId = 154, .pSpeechLineTable = gCharsiSpeechLines, .nSpeechLineCount = 9, .bAborted = 1 },   // [1]
    { .nMonsterClassId = 146, .pSpeechLineTable = gCain1SpeechLines, .nSpeechLineCount = 7, .bIntroHeard = 1, .bAborted = 1 },   // [2]
    { .nMonsterClassId = 147, .pSpeechLineTable = gGheedSpeechLines, .nSpeechLineCount = 9, .bAborted = 1 },   // [3]
    { .nMonsterClassId = 150, .pSpeechLineTable = gKashyaSpeechLines, .nSpeechLineCount = 12, .bAborted = 1 },   // [4]
    { .nMonsterClassId = 155, .pSpeechLineTable = gWarriv1SpeechLines, .nSpeechLineCount = 11, .unk0x13 = 1, .bAborted = 1 },   // [5]
    { .nMonsterClassId = 331, .pSpeechLineTable = gWarriv1SpeechLines, .bAborted = 1 },   // [6]
    { .nMonsterClassId = 266, .pSpeechLineTable = gWarriv1SpeechLines, .bAborted = 1 },   // [7]
    { .nMonsterClassId = 408, .unk0x04 = 3, .pSpeechLineTable = gWarriv1SpeechLines, .bAborted = 1 },   // [8]
    {   // [9]
        .nMonsterClassId = 175,
        .unk0x04 = 1,
        .pSpeechLineTable = gWarriv2SpeechLines,
        .nSpeechLineCount = 7,
        .bIntroHeard = 1,
        .bAborted = 1,
    },
    { .nMonsterClassId = 176, .unk0x04 = 1, .pSpeechLineTable = gAtmaSpeechLines, .nSpeechLineCount = 10, .bIntroHeard = 1, .bAborted = 1 },   // [10]
    { .nMonsterClassId = 177, .unk0x04 = 1, .pSpeechLineTable = gDrognanSpeechLines, .nSpeechLineCount = 12, .bAborted = 1 },   // [11]
    { .nMonsterClassId = 199, .unk0x04 = 1, .pSpeechLineTable = gElzixSpeechLines, .nSpeechLineCount = 12, .bAborted = 1 },   // [12]
    { .nMonsterClassId = 200, .unk0x04 = 1, .pSpeechLineTable = gGeglashSpeechLines, .nSpeechLineCount = 11, .bAborted = 1 },   // [13]
    { .nMonsterClassId = 198, .unk0x04 = 1, .pSpeechLineTable = gGreizSpeechLines, .nSpeechLineCount = 14, .bAborted = 1 },   // [14]
    { .nMonsterClassId = 201, .unk0x04 = 1, .pSpeechLineTable = gJerhynSpeechLines, .nSpeechLineCount = 10, .bAborted = 1 },   // [15]
    { .nMonsterClassId = 202, .unk0x04 = 1, .pSpeechLineTable = gLysanderSpeechLines, .nSpeechLineCount = 12, .bAborted = 1 },   // [16]
    { .nMonsterClassId = 178, .unk0x04 = 1, .pSpeechLineTable = gFaraSpeechLines, .nSpeechLineCount = 11, .bAborted = 1 },   // [17]
    { .nMonsterClassId = 210, .unk0x04 = 1, .pSpeechLineTable = gMeshif1SpeechLines, .nSpeechLineCount = 12, .unk0x13 = 1, .bAborted = 1 },   // [18]
    { .nMonsterClassId = 265, .pSpeechLineTable = gCain1SpeechLines, .nSpeechLineCount = 7, .bIntroHeard = 1, .bAborted = 1 },   // [19]
    { .nMonsterClassId = 244, .unk0x04 = 1, .pSpeechLineTable = gCain2SpeechLines, .nSpeechLineCount = 7, .bIntroHeard = 1, .bAborted = 1 },   // [20]
    { .nMonsterClassId = 245, .unk0x04 = 2, .pSpeechLineTable = gCain3SpeechLines, .nSpeechLineCount = 8, .bAborted = 1 },   // [21]
    { .nMonsterClassId = 246, .unk0x04 = 3, .pSpeechLineTable = gCain4SpeechLines, .nSpeechLineCount = 3, .bAborted = 1 },   // [22]
    { .nMonsterClassId = 251, .unk0x04 = 1, .pSpeechLineTable = gTyrael1SpeechLines, .nSpeechLineCount = 3, .bAborted = 1 },   // [23]
    { .nMonsterClassId = 252, .unk0x04 = 2, .pSpeechLineTable = gAshearaSpeechLines, .nSpeechLineCount = 9, .bAborted = 1 },   // [24]
    { .nMonsterClassId = 253, .unk0x04 = 2, .pSpeechLineTable = gHratliSpeechLines, .nSpeechLineCount = 12, .bAborted = 1 },   // [25]
    { .nMonsterClassId = 254, .unk0x04 = 2, .pSpeechLineTable = gAlkorSpeechLines, .nSpeechLineCount = 11, .bAborted = 1 },   // [26]
    { .nMonsterClassId = 255, .unk0x04 = 2, .pSpeechLineTable = gOrmusSpeechLines, .nSpeechLineCount = 12, .bAborted = 1 },   // [27]
    { .nMonsterClassId = 406, .unk0x04 = 2, .pSpeechLineTable = gFaraSpeechLines, .nSpeechLineCount = 11, .bAborted = 1 },   // [28]
    { .nMonsterClassId = 257, .unk0x04 = 2, .pSpeechLineTable = gFaraSpeechLines, .nSpeechLineCount = 11, .bAborted = 1 },   // [29]
    { .nMonsterClassId = 264, .unk0x04 = 2, .pSpeechLineTable = gMeshif2SpeechLines, .nSpeechLineCount = 11, .bAborted = 1 },   // [30]
    { .nMonsterClassId = 297, .unk0x04 = 2, .pSpeechLineTable = gNatalyaSpeechLines, .nSpeechLineCount = 6, .bAborted = 1 },   // [31]
    { .nMonsterClassId = 367, .unk0x04 = 3, .pSpeechLineTable = gTyrael1SpeechLines, .nSpeechLineCount = 3, .unk0x13 = 1, .bAborted = 1 },   // [32]
    { .nMonsterClassId = 405, .unk0x04 = 3, .pSpeechLineTable = gTyrael1SpeechLines, .nSpeechLineCount = 3, .bAborted = 1 },   // [33]
    { .nMonsterClassId = 257, .unk0x04 = 3, .pSpeechLineTable = gTyrael1SpeechLines, .nSpeechLineCount = 3, .bAborted = 1 },   // [34]
    { .nMonsterClassId = 511, .unk0x04 = 4, .pSpeechLineTable = gLarzukSpeechLines, .nSpeechLineCount = 12, .bAborted = 1 },   // [35]
    { .nMonsterClassId = 512, .unk0x04 = 4, .pSpeechLineTable = gDrehyaSpeechLines, .nSpeechLineCount = 12, .bAborted = 1 },   // [36]
    { .nMonsterClassId = 513, .unk0x04 = 4, .pSpeechLineTable = gMalahSpeechLines, .nSpeechLineCount = 16, .bAborted = 1 },   // [37]
    { .nMonsterClassId = 514, .unk0x04 = 4, .pSpeechLineTable = gNihlathakSpeechLines, .nSpeechLineCount = 12, .bAborted = 1 },   // [38]
    { .nMonsterClassId = 515, .unk0x04 = 4, .pSpeechLineTable = gQualKehkSpeechLines, .nSpeechLineCount = 11, .bAborted = 1 },   // [39]
    { .nMonsterClassId = 521, .unk0x04 = 4, .pSpeechLineTable = gTyrael1SpeechLines, .nSpeechLineCount = 3, .unk0x13 = 1, .bAborted = 1 },   // [40]
    { .nMonsterClassId = 520, .unk0x04 = 4, .pSpeechLineTable = gCain6SpeechLines, .nSpeechLineCount = 12, .bAborted = 1 },   // [41]
    { .nMonsterClassId = 527, .unk0x04 = 4, .pSpeechLineTable = gDrehyaSpeechLines, .nSpeechLineCount = 12, .bAborted = 1 },   // [42]
    { .nMonsterClassId = 537, .unk0x04 = 4, .pSpeechLineTable = gDrehyaSpeechLines, .nSpeechLineCount = 12, .bAborted = 1 },   // [43]
    { .nMonsterClassId = 538, .unk0x04 = 4, .pSpeechLineTable = gDrehyaSpeechLines, .nSpeechLineCount = 12, .bAborted = 1 },   // [44]
    { .nMonsterClassId = 539, .unk0x04 = 4, .pSpeechLineTable = gDrehyaSpeechLines, .nSpeechLineCount = 12, .bAborted = 1 },   // [45]
};
static_assert(sizeof(gNpcMenuEntries) == 1012, "gNpcMenuEntries: the item's 1012 bytes in the .i64");

//  D2Client 0x6FB7BD6C
//  Number of records in gNpcMenuEntries (46); it sits in the dword directly after the array, so it can look like a 47th entry.
inline int32_t gnNpcMenuEntryCount = 46;
static_assert(sizeof(gnNpcMenuEntryCount) == 4, "gnNpcMenuEntryCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7C4C0
//  Number of records in gNpcMenuDescriptors (48); it sits in the dword directly after the array. NPCMENU_BuildMainMenu uses it to bound its linear
//  search for the NPC's record.
inline int32_t gnNpcMenuDescriptorCount = 48;
static_assert(sizeof(gnNpcMenuDescriptorCount) == 4, "gnNpcMenuDescriptorCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7C4C4
//  The NPC two-line speech table: 2 records of 8 bytes, { int nMonsterClassId; uint16 wShownSpeechId; uint16 wNextSpeechId; }, with its count in
//  gnNpcSpeechPairCount. Both records are monstats hcIdx 265 (cain5) and hold Deckard Cain's Act 1 Quest 4 rescue dialogue:
//      { 265, 123 A1Q4QuestSuccessfulCain,  126 A1Q4TragedyOfTristramCain }
//      { 265, 125 A1Q4RescuedByRoguesCain,  126 A1Q4TragedyOfTristramCain }
//  So whichever opening line applies, the long Tristram monologue follows: a first line and a follow-up. Read by NPCMENU_BeginInteraction,
//  NPCMENU_ChooseNextSpeech and NPCMENU_OnSpeechDismissed, all indexed [i*8]. The class id is an hcIdx, not a monstats row number.
inline D2NpcSpeechPairStrc gNpcSpeechPairs[2] =
{ { .nMonsterClassId = 265, .wShownSpeechId = 123, .wNextSpeechId = 126 }, { .nMonsterClassId = 265, .wShownSpeechId = 125, .wNextSpeechId = 126 } };
static_assert(sizeof(gNpcSpeechPairs) == 16, "gNpcSpeechPairs: the item's 16 bytes in the .i64");

//  D2Client 0x6FB7C4D4
//  The record count of gNpcSpeechPairs (2), immediately after the array.
inline int32_t gnNpcSpeechPairCount = 2;
static_assert(sizeof(gnNpcSpeechPairCount) == 4, "gnNpcSpeechPairCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5BC8
//  Index into the hireling offers of the row the player picked in the hire list; written by NPCMENU_OnHireListConfirmClicked and read by
//  NPCMENU_ConfirmHireMercenary, which uses it to fetch the wMercName it sends with client command 54. Nothing checks it against 10 at that use.
inline int32_t gnSelectedHirelingRow;
static_assert(sizeof(gnSelectedHirelingRow) == 4, "gnSelectedHirelingRow: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5BCC
//  data\global\ui\menu\goldbtn: the gold button on the trade panel. Loaded by NPCMENU_Init and NPCMENU_Reset, unloaded by NPCMENU_Shutdown.
inline D2CellFileStrc *gNpcGoldButtonImage;
static_assert(sizeof(gNpcGoldButtonImage) == 4, "gNpcGoldButtonImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5BD0
//  Set when NPCMENU_BeginTransaction accepts a click; cleared at init and reset, and by NPCMENU_SendTransactionAndWait only in its arm for an item
//  not found (the send path leaves it set). Read as the guard by NPCMENU_AckItemMessage, the five NPCMENU_AckItemAction_* handlers,
//  NPCMENU_BuildConfirmDialog and SendTransactionAndWait: it stops a second click, or an acknowledgement for an unrelated item, from being taken
//  as part of the transaction in flight.
inline uint8_t gbTransactionPending;
static_assert(sizeof(gbTransactionPending) == 1, "gbTransactionPending: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB5BD1
//  The dwClassId of the item in the pending transaction (or -1 when there is none), written alongside gTransactionItemGUID by
//  NPCMENU_BeginTransaction, NPCMENU_OnTransactionResult, NPCMENU_AckItemMessage and the five NPCMENU_AckItemAction_* handlers. Write-only: no
//  reader found.
inline int32_t gTransactionItemClassId;
static_assert(sizeof(gTransactionItemClassId) == 4, "gTransactionItemClassId: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5BD5
//  The GUID of the item the pending NPC transaction is about. Written by NPCMENU_BeginTransaction (the item's dwUnitId, or -1 when there is no
//  item; it also stores a literal 0 once), by NPCMENU_OnTransactionResult, NPCMENU_AckItemMessage and the five Ack handlers; read by
//  NPCMENU_SendTransactionAndWait, where it is the dwUnitGUID argument of the command actually sent, and by NPCMENU_BuildConfirmDialog.
inline uint32_t gTransactionItemGUID;
static_assert(sizeof(gTransactionItemGUID) == 4, "gTransactionItemGUID: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5BD9
//  GetTickCount(), written only in two abort arms: NPCMENU_SendTransactionAndWait's cancelled buy (the "buy cancelled for non existant item?"
//  trace) and NPCMENU_TickAndValidate's Confirm dialog closed by a cursor item. Zeroed by NPCMENU_Init and NPCMENU_Reset. Read only by the five
//  NPCMENU_AckItemAction_* handlers and NPCMENU_AckItemMessage, for one test: now - gnLastItemActionTick <= 1000 decides whether the next action
//  gets the 100 ms throttle or the 15-to-20-second one (see sgUIState.nNextItemActionTick).
inline uint32_t gnLastItemActionTick;
static_assert(sizeof(gnLastItemActionTick) == 4, "gnLastItemActionTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5BDD
//  How many item actions were aborted in quick succession: incremented in the same two abort arms as gnLastItemActionTick (by
//  NPCMENU_TickAndValidate and NPCMENU_SendTransactionAndWait); nothing decrements it, and only NPCMENU_Init and NPCMENU_Reset clear it. The
//  five NPCMENU_AckItemAction_* handlers and NPCMENU_AckItemMessage read it clamped to 10 into the throttle, 100 * (5 * min(count, 10) + 150) ms,
//  so the penalty saturates at 20 seconds.
inline uint32_t gnItemActionBurstCount;
static_assert(sizeof(gnItemActionBurstCount) == 4, "gnItemActionBurstCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5BE1
//  GetTickCount() at the moment NPCMENU_SendTransactionAndWait sent a command. NPCMENU_BeginTransaction reads it back and refuses a quick
//  transaction within 500 ms (0x1F4) of the last one, so a held shift-click cannot outrun the server. Cleared by NPCMENU_Init and NPCMENU_Reset.
inline uint32_t gLastTransactionTick;
static_assert(sizeof(gLastTransactionTick) == 4, "gLastTransactionTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5BE5
//  The buffer NPCMENU_BuildConfirmDialog composes its confirmation prompt in (forty Unicodes; ours is constructed by its declaration).
//  NPCMENU_TransactionCost appends to it (Unicode::strcat, UI_AddColorCodeToWstring) and BuildConfirmDialog copies the cost text into it.
//  It sits just below the NPC menu's state block, not inside it, so the memset in NPCMENU_Init does not reach it.
inline Unicode gwszNpcConfirmText[40];
static_assert(sizeof(gwszNpcConfirmText) == 80, "gwszNpcConfirmText: the item's 80 bytes in the .i64");

//  D2Client 0x6FBB5C38
//  The NPC menu's state block: 348 bytes (0x6FBB5C38..0x6FBB5D93), one packed struct whose four-byte fields sit at unaligned addresses. The
//  original clears all of it with one memset (mov ecx, 57h / rep stosd) in NPCMENU_Init and NPCMENU_Reset. The fields, by offset:
//    bNpcSkipMenuOnInteract  read in one place, NPCMENU_BeginInteraction (0x6FAF840E): when set it takes a short path (records the GUID and class
//                            id, sets the speech and interaction flags, clears UNITFLAG_TARGETABLE, returns). Nothing sets it: dead in the
//                            shipped build
//    bNpcSpeechActive        u8, set while an NPC speech line is playing, by every path that starts a line; cleared by NPCMENU_ChooseNextSpeech
//                            when the queue runs out and by NPCMENU_OnInteractionStop. Both talk-menu callbacks (NPCMENU_OnTalkMenuCancel and
//                            NPCMENU_OnTalkMenuClickAway) return 0 at once while it is set, so escape and click-away do nothing mid-line
//    bNpcSpeechLinesRolled   set by NPCMENU_RerollAllSpeechLines once it has run NPCMENU_PickRandomSpeechLine over every entry of gNpcMenuEntries,
//                            cleared by NPCMENU_Reset; read as "the greeting lines were already chosen for this session"
//    CurrentState            the UI state, named by the asserts sgUIState.CurrentState != UIStateHealPlayer and != UIStateGraphicVerifyingAction:
//                            4 = UIStateGraphicVerifyingAction, 8 = UIStateHealPlayer; 3 and 10 are also set in the NPC code
//    nCachedPlayerGold       the player's gold stat (stat 14, a literal push 0Eh), cached by NPCMENU_BeginInteraction, NPCMENU_OpenMainMenu and
//                            NPCMENU_OnTransactionResult; written and never read
//    wNpcSpeechTblId         u16, the tbl string id of the speech line being delivered; set by NPCMENU_BeginInteraction and
//                            NPCMENU_ChooseNextSpeech (which parks 0xFFFF in it when the queue empties); passed to TEXT_ShowSpeechPopup,
//                            appended to wNpcSpeechesPlayed, and sent to the server as the u16 message id of client command 49 (0x31)
//    wNpcNextSpeechTblId     u16, the next queued speech line, 0xFFFF when there is none; written by NPCMENU_BeginInteraction and
//                            NPCMENU_ChooseNextSpeech. NPCMENU_OnInterruptNewsClicked tests it against 0xFFFF to arm bNpcSpeechActive and the
//                            TEXT_SetCallback chain or to drop CurrentState back to 0; ChooseNextSpeech's 0xFFFF arm clears bNpcSpeechActive
//    wNpcShownSpeechTblId    u16, the tbl string id last handed to TEXT_ShowSpeechPopup, i.e. the line last shown; written (16-bit stores) by
//                            NPCMENU_ChooseNextSpeech, NPCMENU_OnSpeechDismissed and NPCMENU_OnTalkLineClicked, read by OnSpeechDismissed
//    pNpcSpeechDesc          the current NPC's row of gNpcMenuEntries (22-byte stride), set by NPCMENU_OnTalkClicked and
//                            NPCMENU_OnSpeechDismissed; read by NPCMENU_PlayClassGreeting, ShowDescriptorSpeech and ShowSpeechLine.
//                            +5 is the line table (15-byte records), +9 the line count (used as 15 * (n % count), so an index wraps),
//                            +17 a flag every greeting path sets before showing the popup; the player class is compared at the line table's +26
//    nTalkMenuBaseEntryIdx   written once by NPCMENU_BuildTalkMenu from its nMenuEntryIdx and read once by NPCMENU_OnTalkLineClicked, which uses
//                            it to select the entry's byte at +0x14 and so a header-row bias of 3 or 2; it carries the entry index from the
//                            build to the click
//    NpcHirelingOffers       ten 16-byte mercenary offers (0x6FBB5C55..0x6FBB5CF5); the WORD at +0, wMercName, is the merc's name id, which
//                            NPCMENU_ConfirmHireMercenary sends back in client command 54 (0x36 hire); server packet 0x4E fills a row. Written by
//                            NPCMENU_AddHirelingOffer, cleared by NPCMENU_ClearHirelingOffers and the vendor teardown, read by
//                            NPCMENU_DrawHireMercenaryList and ConfirmHireMercenary
//    NpcClassId              the monstats class id (hcIdx) of the NPC being interacted with, written by NPCMENU_SetInteractingNpc from
//                            pNpc->dwClassId alongside NpcUnitGUID; NPCMENU_OnTalkClicked searches gNpcMenuEntries for it. Left stale when the
//                            interaction flag is cleared, so test the flag before reading it
//    Transaction             the transaction kind, named by the assert sgUIState.Transaction != HealPlayer (preceded by a compare with 6, so
//                            HealPlayer = 6)
//    bNpcQuestMsgPending     written and read only by NPCMENU_OnNpcCmdPacket
//    wNpcQuestMsgId          written by NPCMENU_OnNpcCmdPacket and never read; it holds 0 before that store
//    bTransactionItemWasOnCursor  INVENTORY_GetCursorItem(pPlayer->pInventory) == pTransactionItem, captured by NPCMENU_SendTransactionAndWait
//                            as the command goes out and read by NPCMENU_OnTransactionResult only on the Transaction == 2 path; it cannot be
//                            recomputed when the reply arrives, since the server may have moved the item
//    pNpcQuestRecord         the result of QUESTRECORD_AllocRecord, called twice in a row by NPCMENU_Init; re-allocated by NPCMENU_Reset and
//                            cleared by NPCMENU_Shutdown; field +0xDB of the block
//    pQuestRecordSecondary   the client's second quest bit buffer, allocated and freed alongside pNpcQuestRecord and exposed by
//                            NPCMENU_QuestGetQuestRecordSecondary. What distinguishes it is not established; it is asked quest state bit 13
//                            (the primary record bits 0, 1 and 14) by NPCMENU_PickRandomSpeechLine and QUEST_ShouldPlayAreaEntrySpiel
//    unk0xE3, unk0xE7        a pair written twice by NPCMENU_BeginTransaction, each from one of that function's own arguments, and read nowhere
//    nTransactionTimeoutTick GetTickCount() + 0xEA60, a sixty-second deadline written wherever the waiting dialog opens
//                            (NPCMENU_OnResurrectMercClicked, SendTransactionAndWait, ConfirmHireMercenary, OnIdentifyClicked). Once it has
//                            passed, NPCMENU_TickAndValidate ends the game (MSG_Send_D2CLTSYS_ENDGAME, GAME_SetLeaveGameSent,
//                            GAME_SetDisconnecting), so a server that never answers costs the player a minute
//    nNextItemActionTick     the throttle deadline for the next item action, tested by NPCMENU_TickAndValidate. Set by the five
//                            NPCMENU_AckItemAction_* handlers and NPCMENU_AckItemMessage to now + 100, or, when the reply comes at most 1000 ms
//                            after an abort, to now + 100 * (5 * min(gnItemActionBurstCount, 10) + 150): 15 to 20 seconds. The four
//                            dialog-opening functions write it too
//    nTransactionResultSoundId  the sound to play when the transaction is answered: chosen by NPCMENU_SendTransactionAndWait as the command goes
//                            out, played and then cleared by NPCMENU_OnTransactionResult, so it fires once per transaction
//    hDialogConfirm          the transaction confirmation dialog (tbl VerifyTransaction1..9)
//    hDialogResults          the transaction result dialog (NPCMENU_OnTransactionResult)
//    hDialogWaiting          the waiting-for-confirmation dialog (NPCMENU_OnIdentifyClicked, NPCMENU_SendTransactionAndWait)
//    hDialogMainMenu         the main menu (NPCMENU_BuildMainMenu, NPCMENU_DrawHireMercenaryList)
//    hDialogTalkMenu         the talk menu (NPCMENU_BuildTalkMenu, tbl NPCGossipMenu)
//    hDialogHireList         the mercenary hire list, built by NPCMENU_DrawHireMercenaryList and closed by NPCMENU_TeardownHireList,
//                            NPCMENU_CloseAllDialogs and NPCMENU_Shutdown
//    hDialogInterrupt        the interrupt dialog (NPCMENU_BuildInterruptDialog)
//                            (the seven hDialog* handles are all ORed by NPCMENU_IsAnyDialogOpen)
//    TransactionCost         the price of the pending transaction in gold as the client computed it (ITEMS_GetTransactionCost, or
//                            ITEMS_GetAllRepairCosts for repair-all); written by NPCMENU_BeginTransaction, read by
//                            NPCMENU_SendTransactionAndWait, which sends it to the server with the command
//    pHireListControl        the list control inside the hire dialog, an object with a vtable released through slot 0 with a 1; created and
//                            driven by NPCMENU_DrawHireMercenaryList, released by NPCMENU_TeardownHireList and NPCMENU_Shutdown
//    LastTransactionResult   a verbatim copy of server packet 0x2A (15 bytes) saved by NPCMENU_OnTransactionResult; +0x02 is the result code and
//                            +0x07 an item GUID, looked up as UNIT_ITEM by the pending-message code
//    bWaitingForItemMsg      set while the client waits for the server to confirm an item action; NPCMENU_AckItemMessage and the five
//                            NPCMENU_AckItemAction_* handlers test it first and clear it. Set to 1 by NPCMENU_OnTransactionResult, written
//                            from a register by NPCMENU_SendTransactionAndWait and NPCMENU_OnIdentifyClicked, and cleared by
//                            NPCMENU_SendQuestItemAction before sending, so the imbue/socket/personalise path does not wait on it
//    wNpcSpeechesPlayed      ten words (20 bytes): the ring of tbl ids of speech lines already delivered in this conversation, so a line is not
//                            repeated. Only indices 0..8 are written, by NPCMENU_BeginInteraction, NPCMENU_ChooseNextSpeech and
//                            NPCMENU_OnInterruptNewsClicked under the guard nNpcSpeechesPlayed < 9; searched by NPCMENU_SpeechNotYetPlayed
//                            and NPCMENU_OnSpeechDismissed
//    nNpcSpeechesPlayed      u16, the number of valid entries in wNpcSpeechesPlayed: reset by NPCMENU_BeginInteraction at the start of a
//                            conversation and post-incremented on every append. The bound tested is 9 (cmp ax, 9 / jnb), not the array's length
//    bNpcVendorGamble        a boolean, not a mode: written only by the npcmenu functions (0 by NPCMENU_OnTradeClicked and six sites of
//                            NPCMENU_BeginInteraction, a literal 1 by NPCMENU_OnGambleClicked). Every reader but NPCMENU_IsVendorGamble maps a
//                            non-zero value to transaction type 2, the gamble arm of ITEMS_GetTransactionCost; gates the shortened item
//                            description
//    bNpcGreetingIsOneShot   set to 1 in NPCMENU_OpenMainMenu only on the branch where the NPC's gNpcMenuEntries record had its byte at +0x12
//                            equal to 1, a one-shot greeting that branch consumes by writing 0 back and which it announces to the server with
//                            client command 0x4D carrying the NPC class id. Its only use afterwards is as the gate on
//                            SOUNDINST_MarkStarted(pNpcGreetingSound) in every teardown; cleared alongside pNpcGreetingSound and by
//                            NPCMENU_NpcResyncVendorInventory
//    pNpcGreetingSound       the sound handle of the NPC's greeting speech, from PlaySound(nSoundId, pPlayer, 0, 1, 0) with the id from
//                            OBJECT_PickNpcSpeechSound(pNpc, nGreetingKind) in NPCMENU_OpenMainMenu; every teardown reads it and, if
//                            bNpcGreetingIsOneShot is set, passes it to SOUNDINST_MarkStarted before zeroing both
//    bNihlathakHurrySoundPlayed  one-shot latch: NPCMENU_TickAndValidate plays sounds.txt 4560 nihlathak_hurryup once, on the first frame a
//                            transaction with Nihlathak (monstats 514) waits, and sets this so it never fires again; zeroed by NPCMENU_Init
//                            and NPCMENU_Reset; it is the first thing tested, so once it is up the whole block is skipped
//    bNihlathakHurryTimerArmed  the other half of the latch: set the first time NPCMENU_TickAndValidate sees a transaction with him waiting,
//                            which is when nNihlathakHurryTick is computed; on a later tick with this set and the sound flag still clear the
//                            function jumps straight to the deadline comparison without recomputing it
//    nNihlathakHurryTick     the deadline for the hurry-up line, GetTickCount() + 1000 * (playerSeed % 30) + 120000, randomised from the player's
//                            own seed; the test in NPCMENU_TickAndValidate lets the line play at once (see the BUG note there)
inline D2NpcMenuUIStateStrc sgUIState;
static_assert(sizeof(sgUIState) == 348, "sgUIState: the item's 348 bytes in the .i64");

//  D2Client 0x6FBB5D94
//  A snapshot of the NPC's GUID taken by NPCMENU_OnHireListConfirmClicked when the player clicks a row of the hire list, and sent by
//  NPCMENU_ConfirmHireMercenary as the second field of client command 54 (0x36 hire mercenary).
inline int32_t gnHireNpcUnitGUID;
static_assert(sizeof(gnHireNpcUnitGUID) == 4, "gnHireNpcUnitGUID: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5D98
//  data\global\ui\panel\buysell: the trade panel's own artwork. Loaded by NPCMENU_Init and NPCMENU_Reset, unloaded by NPCMENU_Shutdown.
inline D2CellFileStrc *gNpcBuySellImage;
static_assert(sizeof(gNpcBuySellImage) == 4, "gNpcBuySellImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5D9C
//  The gold cost shown on the "resurrect mercenary" row of the NPC main menu. Written by NPCMENU_SetResurrectMercOffer from server packet 0x9B,
//  read once by NPCMENU_BuildMainMenu. That row is built rather than looked up: when a menu row's label id is 5383, BuildMainMenu formats tbl 22696
//  "Resurrect %s: %d" with the merc's name (gwMercResurrectNameTblId, or tbl 11021 as the default) and this number, so tbl 5383 is a placeholder
//  that never reaches the screen.
inline int32_t gnMercResurrectCost;
static_assert(sizeof(gnMercResurrectCost) == 4, "gnMercResurrectCost: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5DA0
//  CUNIT_GetPlayer() cached when NPCMENU_OnTradeClicked or NPCMENU_OnGambleClicked sends client packet 0x38, and read by
//  NPCMENU_SendTransactionAndWait, which reaches pInventory through it for the cursor-item test behind sgUIState.bTransactionItemWasOnCursor.
inline D2UnitStrc *gpNpcTransactionPlayer;
static_assert(sizeof(gpNpcTransactionPlayer) == 4, "gpNpcTransactionPlayer: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5DA4
//  Written and never read, by exactly the two functions that write gpNpcTransactionPlayer (NPCMENU_OnTradeClicked and NPCMENU_OnGambleClicked),
//  from a register holding zero on the trade path.
inline uint16_t UNRESOLVED_word_6FBB5DA4;
static_assert(sizeof(UNRESOLVED_word_6FBB5DA4) == 2, "UNRESOLVED_word_6FBB5DA4: the item's 2 bytes in the .i64");

//  D2Client 0x6FBB5DA8
//  GetTickCount() at the last transaction NPCMENU_SendTransactionAndWait sent; the same function reads it on the next call as
//  now - gnLastTransactionSendTick, a rate check independent of the item-action throttle in sgUIState.nNextItemActionTick.
inline uint32_t gnLastTransactionSendTick;
static_assert(sizeof(gnLastTransactionSendTick) == 4, "gnLastTransactionSendTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5DAC
//  Scratch flag for one search: NPCMENU_ShowNpcAlert clears it, walks every object unit through NPCMENU_NoteOpenedEunuchObject, and reads it back
//  as "is there an opened objects.txt 318 eunuch in this level" (the murdered harem eunuch in Lut Gholein's palace). If there is, the palace guard
//  (monstats 331 act2guard2) does not say guard_halt (sounds.txt 3983). Written by NoteOpenedEunuchObject and by ShowNpcAlert, which also reads it.
inline uint8_t gbFoundOpenedEunuch;
static_assert(sizeof(gbFoundOpenedEunuch) == 1, "gbFoundOpenedEunuch: the item's 1 bytes in the .i64");

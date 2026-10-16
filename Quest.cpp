#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "QuestLog.h"

//  Quest.cpp -- seven quest functions of D2Client.dll 1.10, 0x6FB06D80 to QUEST_SpawnZooChickens at 0x6FB06F20: the area-entry voice-line gate,
//  the preloads of Duriel's and the Ancient statues' COF files, and the Harrogath zoo (arming it, the armed flag, the chickens).
//  No assert in this unit names the original's source file, so the file name is ours.

//  The offsets and constants this file leans on.
static_assert(MONSTER_CHICKEN == 149,
              "QUEST_SpawnZooChickens: mov ecx, 95h");
static_assert(MONSTER_DURIEL == 211,
              "QUEST_PreloadDurielCof and the quest-13 alert arm: mov ecx, 0D3h");
//  In D2QuestLogQuestDefStrc, +8 (nQuestId) holds the D2Quests id and +0xC holds the QUESTSTATEFLAG index.
static_assert(offsetof(D2QuestLogQuestDefStrc, nQuestId) == 8,
              "the four walks compare THIS dword and step 16 bytes");
static_assert(offsetof(D2QuestLogQuestDefStrc, nQuestStateFlag) == 0xC
              && offsetof(D2QuestLogQuestDefStrc, nAct) == 3
              && offsetof(D2QuestLogQuestDefStrc, nSlotInPage) == 2,
              "the three fields read off the record a walk lands on");
static_assert(offsetof(D2QuestLogEntryStrc, nQuestStateFlag) == 1
              && offsetof(D2QuestLogEntryStrc, wszText) == 7
              && offsetof(D2QuestLogEntryStrc, nStateColour) == 0x265,
              "pInfo's three writes: the unaligned id, the 300-Unicode run, "
              "the colour tested against 5");


//1.10f: D2Client.0x6FB06D80
//  Tells whether the quest's area-entry voice line still wants to play. Its only caller is UNITSND_PlayAreaEntryQuestLine.
//  Four gates before the answer:
//      QUESTS_GetQuestState(0, nQuestKey)                  -- else FALSE
//      nQuestKey found in gQuestLogQuestDefs' nQuestId     -- else FALSE (41 records of 16 bytes; the index is unsigned, as the loop's jb is)
//      the record's nQuestStateFlag == 41                  -- else FALSE
//      record bits 13 (secondary record), 1, 0 and 14 (main record) all clear -- else FALSE
//  Then nQuestKey != 1 returns TRUE, and key 1 (the Den of Evil) has one more question: build the entry and ask whether its colour is under 5.
//  Of the table's two id columns, +8 is the D2Quests id (what packets 0x50 and 0x5D carry as their quest id; 1..36 with gaps, 37 on every blank
//  row) and +0xC is the QUESTSTATEFLAG index (0..40, equal to the record index on every row), the column QUESTLOG_BuildQuestDisplayInfo scans.
//  The test nQuestStateFlag == 41 cannot hold with the shipped data: 41 is the value the four intro flags share, so it reads as "this log row is
//  an act intro", but the +0xC column stops at 40 and the blank rows mark themselves with 37 at +8. So the jz at 0x6FB06DC1 is never taken in
//  stock 1.10. The C keeps the test, because the bytes have it and a mod's table could carry the value.
//  The original constructs pInfo.wszText[300] with Unicode's default constructor, once per element, before QUESTLOG_BuildQuestDisplayInfo fills
//  the record; ours writes the same 300 stores. pInfo.nQuestStateFlag is written at +1 (an unaligned int), and the answer is
//  pInfo.nStateColour (+0x265) < 5: the state colour is a small index, and 5 is the first "done" colour.
BOOL __fastcall QUEST_ShouldPlayAreaEntrySpiel(D2Quests nQuestKey)
{
    //  Unsigned, because the loop's back edge is jb.
    uint32_t iQuestDef;
    //  The walk over the +8 column; the `+= 4` below steps one 16-byte record, four of these four-byte elements.
    D2Quests *pDefQuestId;
    //  The QUESTSTATEFLAG index of the record found (+0xC), not a D2Quests id.
    D2QuestStateFlagIds nQuestStateFlag;
    D2BitBufferStrc *pQuestRecord;
    D2QuestLogEntryStrc pInfo;
    int32_t i;

    if (!QUESTS_GetQuestState(0, nQuestKey))
    {
        return FALSE;
    }

    iQuestDef = 0;
    pDefQuestId = &gQuestLogQuestDefs[0].nQuestId;
    while (*pDefQuestId != nQuestKey)
    {
        ++iQuestDef;
        pDefQuestId += 4;
        if (iQuestDef >= 0x29)
        {
            return FALSE;
        }
    }

    nQuestStateFlag = gQuestLogQuestDefs[iQuestDef].nQuestStateFlag;
    if (nQuestStateFlag == 41)
    {
        return FALSE;
    }

    if (!NPCMENU_QuestGetQuestRecordSecondary())
    {
        return FALSE;
    }
    pQuestRecord = NPCMENU_QuestGetQuestRecordSecondary();
    if (D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, nQuestStateFlag, 13))
    {
        return FALSE;
    }

    if (!NPCMENU_QuestGetQuestRecord())
    {
        return FALSE;
    }
    pQuestRecord = NPCMENU_QuestGetQuestRecord();
    if (D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, nQuestStateFlag, 1))
    {
        return FALSE;
    }
    pQuestRecord = NPCMENU_QuestGetQuestRecord();
    if (D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, nQuestStateFlag, 0))
    {
        return FALSE;
    }
    pQuestRecord = NPCMENU_QuestGetQuestRecord();
    if (D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, nQuestStateFlag, 14))
    {
        return FALSE;
    }

    if (nQuestKey != 1)
    {
        return TRUE;
    }

    //  The original calls Unicode's default-constructor closure once per element; it zeroes the 300 words the callee fills.
    for (i = 0; i < 300; ++i)
    {
        pInfo.wszText[i] = 0;
    }

    pInfo.nQuestStateFlag = nQuestStateFlag;
    QUESTLOG_BuildQuestDisplayInfo(&pInfo);
    return pInfo.nStateColour < 5u;
}


//1.10f: D2Client.0x6FB06E70
//  Preloads Duriel's COF files once, so the fight does not stall on disk when he spawns: GFX_AddMonsterCofFiles(211, 1) behind gbDurielCofLoaded.
//  The flag is set before the call, and the call is the function's tail (jmp). 211 is MONSTER_DURIEL. The only caller is
//  OBJECT_ProcessHoradricOrifice: putting the staff into the orifice arms the preload, one room before he appears.
void __cdecl QUEST_PreloadDurielCof(void)
{
    if (!gbDurielCofLoaded)
    {
        gbDurielCofLoaded = 1;
        GFX_AddMonsterCofFiles(MONSTER_DURIEL, 1);
    }
}


//1.10f: D2Client.0x6FB06E90
//  Preloads the three Ancient statues' COF files once (monster ids 537, 538, 539, behind gbAncientStatueCofLoaded), called from
//  OBJECT_ProcessAncientsAltar, so touching the altar arms them. The first two calls pass bValidateSprites 0 and the third is a tail jmp.
//  The ids are ancientstatue1, ancientstatue2 and ancientstatue3 as the game resolves them (by hcIdx); indexed by line they would read act5barb4,
//  ancientstatue1, ancientstatue2, because of monstats.txt's Expansion marker at row 410.
void __cdecl QUEST_PreloadAncientStatueCof(void)
{
    if (!gbAncientStatueCofLoaded)
    {
        gbAncientStatueCofLoaded = 1;
        GFX_AddMonsterCofFiles(MONSTER_ANCIENTSTATUE1, 0);
        GFX_AddMonsterCofFiles(MONSTER_ANCIENTSTATUE2, 0);
        GFX_AddMonsterCofFiles(MONSTER_ANCIENTSTATUE3, 0);
    }
}


//1.10f: D2Client.0x6FB06ED0
//  Arms the Harrogath zoo: the flag goes up and the packet's value is kept in gnZooMonsterId. It has no code caller: QUESTS_DispatchQuestPacket
//  tail-jumps here for case 36 (QUEST_A5Q6_BAAL) of server packet 0x50, and the value is the word at +3 of that payload. gnZooMonsterId is
//  write-only in stock 1.10: this store is its only reference.
void __fastcall QUEST_ArmZooEffect(int32_t nValue)
{
    gbZooArmed = 1;
    gnZooMonsterId = nValue;
}


//1.10f: D2Client.0x6FB06EE0
//  Returns gbZooArmed. Its sole caller OBJECT_ProcessZoo does nothing at all when it is clear.
uint8_t __cdecl QUEST_IsZooArmed(void)
{
    return gbZooArmed;
}


//1.10f: D2Client.0x6FB06EF0
//  gbZooChickensSpawned && CUNIT_FindUnitByClientIdAndType(gnZooChickenUnitId, UNIT_MONSTER).
//  The second half is what makes the spawn re-arm: the flag alone would latch forever, but the recorded unit id is looked up every time, so once
//  the chicken is gone OBJECT_ProcessZoo spawns again. Only the last chicken's id survives (QUEST_SpawnZooChickens overwrites the global on every
//  iteration), so this asks whether that one is still there, not whether any are.
bool __cdecl QUEST_ZooChickensPresent(void)
{
    return gbZooChickensSpawned && CUNIT_FindUnitByClientIdAndType(gnZooChickenUnitId, UNIT_MONSTER);
}


//1.10f: D2Client.0x6FB06F20
//  Spawns the zoo chickens when gbZooArmed is up: between two and four of them (count = (uint32)lSeed % 3 + 2), drawn from the player's own LCG
//  (the low seed times 0x6AC690C5 plus the high seed, stored back as one 64-bit lSeed). Its sole caller is OBJECT_ProcessZoo, from three call
//  sites with three different pairs of coordinates.
//  The counter is also the position offset, modulo two: every chicken goes to (i % 2 + nXPos, i % 2 + nYPos), so the chickens stack on exactly
//  two squares, (x, y) and (x+1, y+1), a diagonal rather than a spread.
//  149 is MONSTER_CHICKEN. The last two arguments of CUNIT_CreateClientOnlyUnit are UNIT_MONSTER (1) and the mode 1, both one, not zero.
//  Only the last chicken's id survives in gnZooChickenUnitId; gbZooChickensSpawned goes up on every iteration.
//  The player is not null-checked: the seed is read straight off the unit.
void __fastcall QUEST_SpawnZooChickens(int32_t nXPos, int32_t nYPos)
{
    D2UnitStrc *pPlayer;
    int64_t lSeed;
    int32_t iChicken;
    int32_t nChickenCount;
    D2UnitStrc *pChicken;
    int32_t dwUnitId;

    if (gbZooArmed)
    {
        pPlayer = CUNIT_GetPlayer();
        lSeed = pPlayer->pSeed.nHighSeed + 1791398085LL * pPlayer->pSeed.nLowSeed;
        pPlayer->pSeed.lSeed = lSeed;

        iChicken = 0;
        nChickenCount = (uint32_t)lSeed % 3 + 2;
        if (nChickenCount > 0)
        {
            do
            {
                pChicken = CUNIT_CreateClientOnlyUnit(MONSTER_CHICKEN, iChicken % 2 + nXPos, iChicken % 2 + nYPos, UNIT_MONSTER, 1);
                if (pChicken)
                {
                    dwUnitId = pChicken->dwUnitId;
                }
                else
                {
                    dwUnitId = -1;
                }
                ++iChicken;
                gnZooChickenUnitId = dwUnitId;
                gbZooChickensSpawned = 1;
            }
            while (iChicken < nChickenCount);
        }
    }
}

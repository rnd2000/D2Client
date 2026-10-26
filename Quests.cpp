#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Quests.h"

//  Quests.cpp -- the quest-state packets of D2Client.dll 1.10: the dispatcher of server packet 0x50 (one handler per quest), the store of the
//  quest-state bytes of packet 0x5E, the player of the cinematic of packet 0x61, and the two exports that read the stored bytes.
//  The original's source file is QUEST\Quests.cpp (the asserts of QUESTS_GetQuestState name it). Address range 0x6FAB8E00..0x6FAB8F5B.
//  QUESTS_AreStatesReceived and QUESTS_GetQuestState are D2Client.dll's export ordinals 10001 and 10002 (by ordinal only, in D2Client.def); they are
//  `extern "C"` so that the .def can export them under these names. The two globals they work on, gQuestStates and gbQuestStatesReceived,
//  are in Quests.h.

//  The source file, as QUESTS_GetQuestState hands it to FOG_DisplayAssert.
static const char szSourceFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\QUEST\\Quests.cpp";

static_assert(offsetof(D2GSPacketSrv50, nQuestId) == 1
              && offsetof(D2GSPacketSrv50, HirelingPayload) == 3
              && offsetof(D2GSPacketSrv50, A5Q6_BaalPayload) == 3,
              "QUESTS_DispatchQuestPacket: `mov ax, [ecx+1]` at 0x6FAB8E03, "
              "`mov cx, [ecx+3]` at 0x6FAB8E2C and `mov ax, [ecx+3]` at 0x6FAB8E21");
static_assert(offsetof(D2GSPacketSrv5E, nQuestIntroFlags) == 1
              && sizeof(((D2GSPacketSrv5E *)0)->nQuestIntroFlags) == 9 * 4 + 1,
              "QUESTS_StoreSrv5EPayload: `lea esi, [ecx+1]`, then `mov ecx, 9 / "
              "rep movsd / movsb` -- 37 bytes");
static_assert(offsetof(D2GSPacketSrv61, nCinematic) == 1,
              "QUESTS_PlayCinematicFromPacket: `mov al, [ecx+1]` at 0x6FAB8F52");

//1.10f: D2Client.0x6FAB8E00
//  PACKET 0x50, the quest-state message: switch on the quest id at +1 and hand the packet, or the word at +3, to the handler for that quest.
//  The id is zero-extended and less 1 bounds the switch, UNSIGNED (`lea esi, [eax-1] / cmp esi, 23h / ja`), so 0 and anything above 36 take the
//  default, which calls FOG_Trace; a byte table of 36 entries picks one of seven targets. Five of the six arms are one tail jump each; arm 23 (the
//  end of the game) sends client packet 0x68, then calls three setters, the last by tail jump. Arm 13 reaches TEXT_QuestOnCairnStoneOrderCmd, which
//  returns at once for any id but 4.
//  Arm 2 loads the word with `mov cx, [ecx+3]` (0x6FAB8E2C), leaving the packet pointer's top half in ecx; the callee takes an __int16 and compares
//  cx alone (`cmp [eax], cx`, 0x6FAFBE35), so that is the ABI and not a defect. Arm 36 zero-extends the word into QUEST_ArmZooEffect's int, as the
//  promotion of the uint16_t field does, and so does the default's `push eax` into FOG_Trace's %d.
void __fastcall QUESTS_DispatchQuestPacket(D2GSPacketSrv50 *pCmd)
{
    switch (pCmd->nQuestId)
    {
    case 1:
        QUESTLOG_QuestOnQuestLogStateCmd(&pCmd->nHeader);
        return;

    case 2:
        NPCMENU_RefreshHireIconsForOffer(pCmd->HirelingPayload.nHireling);
        return;

    case 3:
    case 6:
        NPCMENU_OnNpcCmdPacket(pCmd);
        return;

    case 4:
    case 13:
        TEXT_QuestOnCairnStoneOrderCmd(&pCmd->nHeader);
        return;

    case 23:
        MSG_Send_D2CLTSYS_ENDGAME();
        WINMAIN_SetAppModeContinue(FALSE);
        GAME_SetLeaveGameSent(1);
        GAME_SetDisconnecting();
        return;

    case 36:
        QUEST_ArmZooEffect(pCmd->A5Q6_BaalPayload.nZooMonsterId);
        return;

    default:
        FOG_Trace("Invalid %d Quest data\n", pCmd->nQuestId);
        return;
    }
}

//1.10f: D2Client.0x6FAB8EC0
//  PACKET 0x5E: keep its 37 bytes in gQuestStates and raise gbQuestStatesReceived. The body behind SCMD_HandleSrv5E_QuestAvailability, which jumps
//  here. The copy is a `rep movsd` of nine dwords and a `movsb`, then the store of 1. What the bytes are: see the BUG note of QUESTS_GetQuestState.
void __fastcall QUESTS_StoreSrv5EPayload(D2GSPacketSrv5E *pPacket)
{
    static_assert(sizeof(pPacket->nQuestIntroFlags) == sizeof(gQuestStates), "the copy fills gQuestStates exactly");
    memcpy(gQuestStates, pPacket->nQuestIntroFlags, sizeof(gQuestStates));
    gbQuestStatesReceived = 1;
}

//1.10f: D2Client.0x6FAB8EE0
//  EXPORT ORDINAL 10001: whether packet 0x5E has arrived. `mov eax, gbQuestStatesReceived / retn 4`: one stack argument, purged and never read.
//  Nothing in D2Client calls it; its only reference is the export table's slot.
extern "C" int32_t __stdcall QUESTS_AreStatesReceived(int32_t nUnused)
{
    (void)nUnused;
    return gbQuestStatesReceived;
}

//1.10f: D2Client.0x6FAB8EF0
//  EXPORT ORDINAL 10002: the byte packet 0x5E left for a quest, zero-extended. Two asserts, both fatal: no packet yet (line 94, "sgbInit", the
//  original's name for gbQuestStatesReceived) and an index of 37 or more (line 99, "idx < QUEST_NUMCLASS"). `retn 8`; the first argument is never
//  read, and every caller passes 0 in it.
//  BUG: the index bound is signed and one-sided (`cmp eax, 25h / jl`, 0x6FAB8F1B), so a negative index passes both asserts and reads below the array.
//  LATENT: every caller in D2Client passes a positive quest id.
//  BUG: the client indexes gQuestStates by quest id, but D2Game fills the packet by the row of its quest-init table (36 rows), and the
//  rows equal the quest ids for 0..13 and 29 only: for every other id from 14 up the client reads another quest's byte, and byte 36 is never written
//  by the server. REACHABLE: QUEST_ShouldPlayAreaEntrySpiel stops on a 0, and D2Game clears a row's byte for a quest the player has finished, so a
//  finished neighbour can silence an Act 5 line (a player who had done the Ancients but not Nihlathak gets no line in Nihlathak's Temple).
extern "C" int32_t __stdcall QUESTS_GetQuestState(int32_t nUnused, D2Quests nQuest)
{
    (void)nUnused;

    if (!gbQuestStatesReceived)
    {
        FOG_DisplayAssert("sgbInit", szSourceFile, 94);
        exit(-1);
    }
    //  The compare is SIGNED, as the original's `jl` (0x6FAB8F1B) is; D2Quests' base is uint32_t, so the cast keeps a negative id passing the assert.
    if ((int32_t)nQuest >= 37)
    {
        FOG_DisplayAssert("idx < QUEST_NUMCLASS", szSourceFile, 99);
        exit(-1);
    }
    //  DIFFERENCE: for a negative id, which no caller passes, the original read below its own array; ours would read below ours, outside any
    //  object of ours, which C++ leaves undefined.
    return (uint8_t)gQuestStates[nQuest];
}

//1.10f: D2Client.0x6FAB8F50
//  PACKET 0x61: play the cinematic whose id is the byte at +1. The id arrives zero-extended, 0..255, and VIDEO_PlayCinematic asserts it against the
//  cinematic count. The original is `xor eax, eax / mov al, [ecx+1] / mov ecx, eax / jmp VIDEO_PlayCinematic`. Its caller,
//  SCMD_HandleSrv61_PlayCinematic, reloads the resolution after it.
void __fastcall QUESTS_PlayCinematicFromPacket(D2GSPacketSrv61 *pPacket)
{
    VIDEO_PlayCinematic(pPacket->nCinematic);
}

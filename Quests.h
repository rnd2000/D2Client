//  Quests.h -- the globals of Quests.cpp that are ours: gQuestStates and gbQuestStatesReceived. Each is a C++17 `inline` variable, one object for
//  every file that includes this header. The comment line "D2Client 0x6F..." above a global gives its address in the original. Both lie past the
//  end of .data's raw data (0x6FB9B000): the loader zero-fills them, and ours are zero-initialised. Include it after DLLmain.h and D2Client.h.
#pragma once

//  D2Client 0x6FBAF91C
//  One byte of quest state per quest, 37 bytes: filled wholesale by QUESTS_StoreSrv5EPayload from server packet 0x5E (a 0x25-byte copy, no
//  interpretation) and read only through the exported QUESTS_GetQuestState, by quest id (0..36). The server fills it by the row of its quest-init
//  table, not by quest id (see the BUG note of QUESTS_GetQuestState).
//  The length is 37 from the code: the copy moves 0x25 bytes and QUESTS_GetQuestState asserts `idx < 37`. The three bytes up to
//  gbQuestStatesReceived (0x6FBAF941..0x6FBAF943) are not referenced.
inline char gQuestStates[37];
static_assert(sizeof(gQuestStates) == 37, "gQuestStates: the item's 37 bytes in the .i64");

//  D2Client 0x6FBAF944
//  Set to 1 by QUESTS_StoreSrv5EPayload when server packet 0x5E arrives. QUESTS_GetQuestState asserts on it ("sgbInit"), so reading a quest state
//  before the packet is a fatal assert, not a zero. QUESTS_AreStatesReceived (export #10001) returns it.
inline int32_t gbQuestStatesReceived;
static_assert(sizeof(gbQuestStatesReceived) == 4, "gbQuestStatesReceived: the item's 4 bytes in the .i64");

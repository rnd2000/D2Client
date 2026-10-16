//  Roster.h -- the globals of Roster.cpp that are ours: the party list, the pending list and the party roster list. All three start NULL.
//  Each is a C++17 `inline` variable, so every file that includes this header shares one object. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FBAB994
//  Head of the party list ROSTER_PartyGetNextEntry iterates (D2RosterUnitStrc: next at +0x80, key at +0x10, pRosterInfo at +0x34, pCorpse at
//  +0x38). Distinct from gpRosterList.
inline D2RosterUnitStrc * gpPartyEntries = NULL;
static_assert(sizeof(gpPartyEntries) == 4, "gpPartyEntries: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB998
//  Head of a second D2RosterUnitStrc list, chained through pNext (+0x80) like gpPartyEntries: entries the client had to invent before the player
//  they describe was on the roster. ROSTER_FindCorpseEntry creates one when a corpse is assigned to a GUID that gpPartyEntries does not know (a
//  zeroed 0x84-byte entry with dwUnitId set, pushed at the head); ROSTER_AddPlayer searches this list first when that player joins, unlinks the
//  entry and reuses it, so the corpse assignments made in the meantime survive; ROSTER_FreeAll frees the list and NULLs the head.
inline D2RosterUnitStrc * gpPendingRosterEntries = NULL;
static_assert(sizeof(gpPendingRosterEntries) == 4, "gpPendingRosterEntries: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB99C
//  Head of the party roster linked list (D2RosterGroupStrc, 12 bytes: wPartyId at +0x00, pMembers, a D2RosterUnitStrc chain, at +0x04, pNext at
//  +0x08), sorted by ROSTER_SortList. Scratch state, built and freed inside a single roster rebuild; ROSTER_AddPlayer and ROSTER_RemovePlayer
//  also use it.
inline D2RosterGroupStrc * gpRosterList = NULL;
static_assert(sizeof(gpRosterList) == 4, "gpRosterList: the item's 4 bytes in the .i64");

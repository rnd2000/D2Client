//  RosterPets.h -- the global of RosterPets.cpp: gpRosterPetList (0x6FBAB9A0). It is a C++17 inline variable, so every file that includes
//  this header shares one object; it starts NULL, in the zero-filled tail of .data.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FBAB9A0
//  Every summon and hireling the client knows about, its own and other players'; a new record is pushed on the head. Filled by server packets
//  0x7A and 0x81, consulted by CUNIT_CreateMonsterUnit when a monster unit arrives, freed wholesale by ROSTERPETS_FreeAll.
inline D2RosterPetStrc * gpRosterPetList = NULL;
static_assert(sizeof(gpRosterPetList) == 4, "gpRosterPetList: the item's 4 bytes in the .i64");

//  arena.h -- the globals of arena.cpp: gdwArenaFlags (0x6FB751B4) and gnArenaPlayerClass (0x6FB751B8). Each is a C++17 inline variable, so
//  every file that includes this header shares one object; both are initialised in the original's .data.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB751B4
//  The arena / game flags dword. SCMD_HandleSrv01_GameFlags takes the DWORD at offset +0x02 of server packet 0x01 and hands it to
//  ARENA_SetArenaFlags, which stores it whole. Only two bits are read in D2Client:
//      0x002  arena mode               -> ARENA_IsArenaMode
//      0x100  arena class forced       -> ARENA_IsArenaPlayerClassForced
//  Bit 0 is ORed in at startup by GAME_ApplyParamSetArenaFlag0 (param slot 35) and has no reader anywhere in D2Client; the first game-flags
//  packet overwrites it in any case. ARENA_GetArenaFlags loads the whole dword, but its one caller, GAME_AppModeGame, discards it.
inline uint32_t gdwArenaFlags = 4;
static_assert(sizeof(gdwArenaFlags) == 4, "gdwArenaFlags: the item's 4 bytes in the .i64");

//  D2Client 0x6FB751B8
//  The character class D2Client reports to the server in place of the player's own: set from a command-line parameter byte by
//  GAME_ApplyParamArenaPlayerClass (through ARENA_SetArenaPlayerClass) and read by MSG_Send_D2CLTSYS_NEWGAME and MSG_Send_D2CLTSYS_JOINGAME
//  (through ARENA_GetArenaPlayerClass), but only when ARENA_IsArenaPlayerClassForced is true.
inline int32_t gnArenaPlayerClass = 1;
static_assert(sizeof(gnArenaPlayerClass) == 4, "gnArenaPlayerClass: the item's 4 bytes in the .i64");

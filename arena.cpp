#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "arena.h"

//  arena.cpp -- the arena settings of D2Client.dll 1.10: the arena / game flags dword and the arena player class, with seven accessors.
//  Seven functions, 84 bytes, address range 0x6FAA6990..0x6FAA6A0B. No assert, FOG_Alloc line or path literal names this unit, so the file
//  name is ours (an unnamed unit is named after what it holds).
//  The unit's two globals sit in .data after dWall.cpp's __FILE__ string and before Env.cpp's gbEnvPlayThunder: gdwArenaFlags (0x6FB751B4,
//  initialised 4) and gnArenaPlayerClass (0x6FB751B8, initialised 1); every reference to either is in these seven functions.
//  Neither initial value survives the start. The launch-parameter table (Game.cpp) runs whole on every start: slot 24,
//  GAME_ApplyParamArenaPlayerClass, overwrites the 1 with the launch configuration's byte, and slot 35, GAME_ApplyParamSetArenaFlag0, ORs
//  bit 0 into the 4, leaving 5 until server packet 0x01 replaces the dword. D2Client tests only bits 0x02 and 0x100.
//  The masks are written as the bytes compute them: ARENA_IsArenaMode takes the low byte first (and eax, 0FFh), ARENA_IsArenaPlayerClassForced
//  does not.

//1.10f: D2Client.0x6FAA6990
//  Arena mode, bit 0x02: and eax, 0FFh / shr eax, 1 / and eax, 1. Called by ten roster and party functions and by UIPANELS_DrawInGamePanels,
//  which draws the local player's arena kill count when it is set.
BOOL __cdecl ARENA_IsArenaMode()
{
    return (gdwArenaFlags & 0xFF) >> 1 & 1;
}

//1.10f: D2Client.0x6FAA69A0
//  Returns the whole dword. The one caller, GAME_AppModeGame, leaves it in edx for MSG_Send_D2CLTSYS_NEWGAME, which never reads edx.
int32_t __cdecl ARENA_GetArenaFlags()
{
    return gdwArenaFlags;
}

//1.10f: D2Client.0x6FAA69B0
//  Stores server packet 0x01's dword at +0x02 (from SCMD_HandleSrv01_GameFlags): a store, not an OR, so bit 0 set at launch does not survive
//  the first game-flags packet.
void __fastcall ARENA_SetArenaFlags(uint32_t dwArenaFlags)
{
    gdwArenaFlags = dwArenaFlags;
}

//1.10f: D2Client.0x6FAA69C0
//  Bit 0, cleared for a non-zero argument and set for zero. The one caller, GAME_ApplyParamSetArenaFlag0, tail-jumps here with ecx cleared
//  (xor ecx, ecx), so only the set arm runs, and nothing in D2Client reads the bit.
void __fastcall ARENA_SetArenaFlag0(int32_t bClear)
{
    if (bClear)
        gdwArenaFlags &= ~1u;
    else
        gdwArenaFlags |= 1u;
}

//1.10f: D2Client.0x6FAA69E0
//  Stores the arena player class; called from GAME_ApplyParamArenaPlayerClass with the launch configuration's character-template byte.
void __fastcall ARENA_SetArenaPlayerClass(int32_t nPlayerClass)
{
    gnArenaPlayerClass = nPlayerClass;
}

//1.10f: D2Client.0x6FAA69F0
//  Returns the arena player class; MSG_Send_D2CLTSYS_NEWGAME and MSG_Send_D2CLTSYS_JOINGAME put it in the packet's class byte when
//  ARENA_IsArenaPlayerClassForced says so.
int32_t __cdecl ARENA_GetArenaPlayerClass()
{
    return gnArenaPlayerClass;
}

//1.10f: D2Client.0x6FAA6A00
//  Arena player class forced, bit 0x100: shr eax, 8 / and eax, 1, without ARENA_IsArenaMode's low-byte mask.
BOOL __cdecl ARENA_IsArenaPlayerClassForced()
{
    return gdwArenaFlags >> 8 & 1;
}

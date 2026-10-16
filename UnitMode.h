//  UnitMode.h -- the globals of UnitMode.cpp that are ours: C++17 inline variables, one object for every file that includes this header.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. gnHitClassOverlay lives in .rdata and is const.
//  gnServerProbeAddr1 and gnServerProbeAddr2 lie past the end of .data's raw data (0x6FB9B000): the loader zero-fills them, and ours are
//  zero-initialised.
//  Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB4ED9C
//  The index table MSVC built for UNITMODE_PerformClientCommand's two-level switch: client command id (1..19) -> command class, class 9
//  asserts. Const, read by nothing of ours (our switch builds its own table).
inline const uint8_t gnClientCmdClass[19] = { 0, 1, 2, 3, 4, 5, 5, 4, 5, 5, 9, 6, 7, 7, 6, 7, 7, 9, 8 };
static_assert(sizeof(gnClientCmdClass) == 19, "gnClientCmdClass: the item's 19 bytes in the .i64");

//  D2Client 0x6FB70EFC
//  7 pairs {dwLastHitClass & 0xF0, Overlay.txt row}, ending at the address 0x6FB70F38: 0x10->54 doubledamage1, 0x20->81 fire_hit, 0x30->36
//  ice_explode, 0x40->1 lightning, 0x70->147 bash, 0x80->21 hit_thorns, 0x90->113 sanctuaryknockback. Read by UNITMODE_PlayHitOverlayForMode.
inline const int32_t gnHitClassOverlay[7][2] =
{
    {0x20, 81}, {0x30, 36}, {0x40, 1}, {0x10, 54}, {0x70, 147}, {0x80, 21}, {0x90, 113},
};
static_assert(sizeof(gnHitClassOverlay) == 56, "gnHitClassOverlay: the item's 56 bytes in the .i64");

//  D2Client 0x6FBBC244
//  First obfuscated address from server packet 0xAD (SCMD_HandleSrvAD_ServerMemoryProbe, UNITMODE_FlushServerMemoryProbe).
inline uint32_t gnServerProbeAddr1;
static_assert(sizeof(gnServerProbeAddr1) == 4, "gnServerProbeAddr1: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBC248
//  Second obfuscated address from server packet 0xAD.
inline uint32_t gnServerProbeAddr2;
static_assert(sizeof(gnServerProbeAddr2) == 4, "gnServerProbeAddr2: the item's 4 bytes in the .i64");

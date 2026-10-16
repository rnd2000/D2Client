//  Msg.h -- the globals of Msg.cpp: the keepalive and last-send stamps, the last sent packet, the load-progress ratio, the join-loop flag,
//  the join and leave start ticks and the game-allocated flag. Address range 0x6FBA79F0..0x6FBA7C08. Each is a C++17 inline variable, one
//  object for every file that includes this header; all eight lie past .data's raw data and are zero at load.
//  Include it after D2Client.h.
#pragma once

//  D2Client 0x6FBA79F0
//  GetTickCount at the last keepalive actually sent by MSG_SendPing6C, which refuses to send again inside 0x1388 = 5000 ms (an unsigned compare).
//  GAME_SendPingIfDue checks gnNextPingTick before calling, so the limit exists in both caller and callee; for callers with no test of their
//  own this stamp is the only limit.
inline uint32_t gnLastPing6CTick;
static_assert(sizeof(gnLastPing6CTick) == 4, "gnLastPing6CTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA79F4
//  An 8.8 fixed-point load-progress ratio (current/total, `<<8` then divided), set by MSG_SetLoadProgressRatio and read by
//  MSG_GetLoadProgressRatio; MSG_Send_D2CLTSYS_OPENCHAR writes it too. Every value stored is 0 or a signed quotient, so it is an int.
inline int32_t gnLoadProgressRatio;
static_assert(sizeof(gnLoadProgressRatio) == 4, "gnLoadProgressRatio: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA79F8
//  A copy of the last packet sent to the server, kept so MSG_SendPacketToServer can suppress an identical resend: inside the window (50 ms
//  for packet ids 5-10 and 12-17, 200 ms for the rest, packet id 0x3A having an arm of its own that skips the window) the new packet is
//  compared with this buffer and not sent on a match; otherwise it is copied in and gnLastPacketSentTick is restamped. The length is
//  asserted under 512 at the function's head, so the string operations stay inside the 512 bytes.
inline uint8_t gLastSentPacket[512];
static_assert(sizeof(gLastSentPacket) == 512, "gLastSentPacket: the item's 512 bytes in the .i64");

//  D2Client 0x6FBA7BF8
//  GetTickCount at the last accepted send, used by MSG_SendPacketToServer's duplicate suppression (compared against 50 ms or 200 ms);
//  restamped after the copy into gLastSentPacket.
inline uint32_t gnLastPacketSentTick;
static_assert(sizeof(gnLastPacketSentTick) == 4, "gnLastPacketSentTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7BFC
//  Ends the join-game message pump. MSG_AppModeJoinGame clears it before entering the loop; MSG_JoinGameMessageLoop and the idle callback
//  MSG_JoinGameIdlePoll each set it to 1; MSG_IsJoinGameLoopDone, the loop's continue test, is the only reader.
inline int32_t gbJoinGameLoopDone;
static_assert(sizeof(gbJoinGameLoopDone) == 4, "gbJoinGameLoopDone: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7C00
//  GetTickCount() when the join began: stored by MSG_AppModeJoinGame and read by MSG_JoinGameMessageLoop for the join's ten-second timeout.
inline uint32_t gnGameJoinStartTickCount;
static_assert(sizeof(gnGameJoinStartTickCount) == 4, "gnGameJoinStartTickCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7C04
//  GetTickCount() when the leave began: stored by MSG_AppModeLeaveGame and read by MSG_LeaveGameMessageLoop for the leave's ten-second timeout.
inline uint32_t gnGameLeaveStartTickCount;
static_assert(sizeof(gnGameLeaveStartTickCount) == 4, "gnGameLeaveStartTickCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7C08
//  A game has been allocated and needs freeing. Set to 1 by MSG_InitGame (inlined in the original), which runs only for the game types that
//  bring up a server here -- single player, single player uncapped and the OBNET and LAN hosts. MSG_FreeGame tests it, clears it before the
//  teardown, and so frees once.
inline int32_t gbGameAllocated;
static_assert(sizeof(gbGameAllocated) == 4, "gbGameAllocated: the item's 4 bytes in the .i64");

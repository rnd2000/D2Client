//  trade.h -- the globals of trade.cpp: the two trade dialogs, the partner's name and GUID, the two gold amounts, the deadline, the state and
//  the flags (0x6FBB9F7C..0x6FBB9FBC). Each is a C++17 inline variable, so every file that includes this header shares one object. They lie
//  past the end of .data's raw data (0x6FB9B000): the loader zero-fills them, and ours are zero-initialised.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FBB9F7C
//  The "waiting for the other player" dialog (tbl 0xFCB, "asking other player to trade"), built by TRADE_HandleStateMessage with
//  TRADE_SendCancelEarlyStamp as its callback. TRADE_CloseWindow closes it before gpTradeConfirmDialog.
inline D2ClientDialogStrc *gpTradeWaitDialog;
static_assert(sizeof(gpTradeWaitDialog) == 4, "gpTradeWaitDialog: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9F80
//  The trading partner's name, widened from server packet 0x78's 16-byte szName by TRADE_SetPartnerFromPacket78. Fifteen characters: the
//  sixteenth is always the terminator.
inline Unicode gwszTradePartnerName[16];
static_assert(sizeof(gwszTradePartnerName) == 32, "gwszTradePartnerName: the item's 32 bytes in the .i64");

//  D2Client 0x6FBB9FA0
//  GetTickCount base for the trade timeout; -1 means no deadline is armed.
inline uint32_t gnTradeDeadline;
static_assert(sizeof(gnTradeDeadline) == 4, "gnTradeDeadline: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9FA4
//  The gold the other party has offered: stored by the packet 0x79 handler when bSelf is 0, reset to 0 by TRADE_OpenWindow and by
//  TRADE_HandleStateMessage's state 1.
inline int32_t gnTradeGoldOther;
static_assert(sizeof(gnTradeGoldOther) == 4, "gnTradeGoldOther: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9FA8
//  Set to 1 where a cancel enters trade state 7 (TRADE_SendCancelOnce and arms of TRADE_HandleStateMessage), cleared by TRADE_ResetState.
//  BUG: the original never reads it. INERT: nothing can observe what it says.
inline uint8_t gbTradeCancelSent;
static_assert(sizeof(gbTradeCancelSent) == 1, "gbTradeCancelSent: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB9FAC
//  The gold we offer, 32 bits; the trade packet carries it as two words, low first. Written by TRADE_SendGoldAmount after clamping to the
//  player's gold, by the packet 0x79 handler when bSelf is set, and zeroed by TRADE_OpenWindow.
inline int32_t gnTradeGoldSelf;
static_assert(sizeof(gnTradeGoldSelf) == 4, "gnTradeGoldSelf: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9FB0
//  The trading partner's player GUID, taken from server packet 0x78's dwPlayerGUID (+0x11) by TRADE_SetPartnerFromPacket78.
inline uint32_t gTradePartnerGUID;
static_assert(sizeof(gTradePartnerGUID) == 4, "gTradePartnerGUID: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9FB4
//  The "agree to trade?" prompt, built by TRADE_HandleStateMessage from tbl lines 0xFCA (agree to trade), 0xD49 (ok) and 0xD48 (cancel);
//  both callbacks are TRADE_SendCancel. Closed and set to NULL by TRADE_CloseWindow, TRADE_SendCancel and TRADE_SendAgreeToTrade.
inline D2ClientDialogStrc *gpTradeConfirmDialog;
static_assert(sizeof(gpTradeConfirmDialog) == 4, "gpTradeConfirmDialog: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9FB8
//  The trade state machine, 0..7 (see TRADE_HandleStateMessage). 3 is set when button 7 goes out (TRADE_SendGreenCheck); 7 is the waiting
//  state TRADE_TickTimeout gives thirty seconds.
inline int32_t gnTradeState;
static_assert(sizeof(gnTradeState) == 4, "gnTradeState: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9FBC
//  Set by TRADE_OpenWindow and cleared by TRADE_CloseWindow; written and never read inside this module.
inline uint8_t gbTradeWindowOpen;
static_assert(sizeof(gbTradeWindowOpen) == 1, "gbTradeWindowOpen: the item's 1 bytes in the .i64");

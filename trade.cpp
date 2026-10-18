#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "ui.h"
#include "trade.h"

//  trade.cpp -- the player-to-player trade window of D2Client.dll 1.10: the protocol state machine (gnTradeState), the hover labels and the
//  two trade dialogs, the gold amounts, the accept and cancel senders and the timeout of the waiting state.
//  The original source file is Ui\trade.cpp (the unit's asserts name it). Address range 0x6FB1F370..0x6FB20060, 19 functions.

//  Layout checks for the structures this file reads.
static_assert(offsetof(SMSGHANDLER_PARAMS, nXpos) == 0x0C
                  && offsetof(SMSGHANDLER_PARAMS, nYpos) == 0x0E,
              "the mouse position the trade panel's handlers hit-test");
static_assert(offsetof(SMSGHANDLER_PARAMS, bReturn) == 0x18
                  && offsetof(SMSGHANDLER_PARAMS, lResult) == 0x1C,
              "what a handler answers with; the handlers return void");
static_assert(offsetof(D2PlayerDataStrc, szName) == 0
                  && sizeof(((D2PlayerDataStrc *)0)->szName) == 16,
              "the trade partner's name is the FIRST field and is 16 bytes "
              "-- the capacity anything copying it is bounded by");
static_assert(offsetof(D2InventoryGridInfoStrc, nGridLeft) == 4,
              "the page-clear loop's end sentinel is the ADDRESS of this "
              "member, not of the grid -- reading the rendered line instead "
              "of the operand once made that search return zero");
static_assert(UI_MPTRADE == 23 && UI_INVENTORY == 1 && UI_CHATBOX == 5,
              "the three panel ids this unit toggles");
static_assert(UI_TURNON == 0 && UI_TURNOFF == 1 && UI_TOGGLE == 2,
              "UI_SetUIToggle's nUIState");
static_assert(UNIT_PLAYER == 0, "the only unit type this unit resolves");
static_assert(STR_IDX_3400_Back == 3400 && STR_IDX_3401_ok == 3401
                  && STR_IDX_4042_strTradeAgreeTo == 4042
                  && STR_IDX_4043_strWaitingForOtherPlayer == 4043
                  && STR_IDX_4131_strUiMenu2 == 4131,
              "the tbl indices this unit pushes; the name carries the "
              "number so a renumbered enum cannot pass");

//  The original's assert path and the one assert text it uses.
static const char szTradeCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\Ui\\trade.cpp";
static const char szWaitDialogNullAssert[] = "sghDialogWaitingForOther == NULL";

//1.10f: D2Client.0x6FB1F370
//  Resets gnTradeState and gbTradeCancelSent to zero. Runs on the way into a game (UI_InitUiForPlayerUnit, UI_InitUiSubsystems).
void __cdecl TRADE_ResetState()
{
    gnTradeState = 0;
    gbTradeCancelSent = 0;
}

//1.10f: D2Client.0x6FB1F380
//  A teardown call without an argument, for UI_ResetUiSubsystems and UI_ShutdownUiSubsystems.
void __cdecl TRADE_CloseOnShutdown()
{
    TRADE_CloseWindow(0);
}

//1.10f: D2Client.0x6FB1F390
//  Draws the hover labels of the trade panel, then watches the trade. Three hit rectangles, all 34 by 34, in panel space:
//
//      x 273..307, y 168..202   tbl 4131, over the other player's accept
//      x 221..255, y 383..417   tbl 4131, over your own
//      x 273..307, y 383..417   tbl 4130, over cancel
//
//  Each label is drawn to the left of its box (34, 34 and 10 pixels) and one pixel above it; the labels need gnTradeState > 2 and
//  UI_IsGoldDialogOpen() false. If the player is dead or missing, or no longer in a town, and the trade window is open, it cancels the trade.
//  Otherwise it draws the wait dialog in state 1 and the confirm dialog in state 2 -- the only place either is drawn.
//  The Y coordinates are `N - gnBorderYDifference`: the panel hangs from the top of the letterbox, where the X coordinates add.
void __cdecl TRADE_DrawButtonHints()
{
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nBorderX;
    int32_t nBorderY;
    const Unicode *pwszText;
    D2ActiveRoomStrc *pRoom;

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();

    if (gnTradeState > 2 && !UI_IsGoldDialogOpen())
    {
        nBorderX = gnBorderXDifference;
        nBorderY = gnBorderYDifference;

        if (nMouseX >= nBorderX + 273 && nMouseX <= nBorderX + 307 && nMouseY >= 168 - nBorderY && nMouseY <= 202 - nBorderY)
        {
            pwszText = D2LANG_GetStringFromTblIndex(STR_IDX_4131_strUiMenu2);
            D2WIN_10129_DrawFramedText(pwszText, nBorderX + 239, 167 - nBorderY, 0, 0);
            nBorderX = gnBorderXDifference;
            nBorderY = gnBorderYDifference;
        }

        if (nMouseX >= nBorderX + 221 && nMouseX <= nBorderX + 255 && nMouseY >= 383 - nBorderY && nMouseY <= 417 - nBorderY)
        {
            pwszText = D2LANG_GetStringFromTblIndex(STR_IDX_4131_strUiMenu2);
            D2WIN_10129_DrawFramedText(pwszText, nBorderX + 187, 382 - nBorderY, 0, 0);
            nBorderX = gnBorderXDifference;
            nBorderY = gnBorderYDifference;
        }

        if (nMouseX >= nBorderX + 273 && nMouseX <= nBorderX + 307 && nMouseY >= 383 - nBorderY && nMouseY <= 417 - nBorderY)
        {
            pwszText = D2LANG_GetStringFromTblIndex(STR_IDX_4130_strUiMenu1);
            D2WIN_10129_DrawFramedText(pwszText, nBorderX + 263, 382 - nBorderY, 0, 0);
        }
    }

    //  out of town, or dead: the trade cannot continue
    if (CUNIT_GetPlayer())
    {
        pRoom = D2COMMON_UNITS_GetRoom(CUNIT_GetPlayer());
        if (CUNIT_IsPlayerDeadOrMissing() || (pRoom && !D2COMMON_DUNGEON_IsTownLevelId((D2C_Levels)D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom))))
        {
            if (UI_GetUiWindowState(UI_MPTRADE))
            {
                MSG_SendCmd3Words(79, 2, 0, 0);
                UI_SetUIToggle(UI_MPTRADE, UI_TURNOFF, 0);
                INV_TradeClearAcceptBlocked();
                INV_ResetPanelState();
                gnTradeState = 0;
            }
            return;
        }
    }

    //  the two dialogs, and the only place either is drawn
    if ((gnTradeState < 3 || gnTradeState > 6) && gnTradeState)
    {
        if (gnTradeState == 1)
        {
            if (gpTradeWaitDialog)
            {
                DIALOG_Draw(gpTradeWaitDialog);
            }
            return;
        }
        if (gnTradeState == 2 && gpTradeConfirmDialog)
        {
            DIALOG_Draw(gpTradeConfirmDialog);
        }
    }
}

//1.10f: D2Client.0x6FB1F590
//  Sets the open flag and zeroes both gold amounts. Called from UI_RegisterPanelHandlers, so it runs when the panel's handlers go on rather
//  than when a trade begins; it does not touch gnTradeState.
void __cdecl TRADE_OpenWindow()
{
    gbTradeWindowOpen = 1;
    gnTradeGoldOther = 0;
    gnTradeGoldSelf = 0;
}

//1.10f: D2Client.0x6FB1F5B0
//  The counterpart: clears the flag, closes whichever dialog is up, and -- unless the state is already 7, the waiting state -- turns the
//  panel off, reopens the inventory if the caller asked for it and the player is alive, closes the chat box, and sends the cancel if any
//  trade was in progress. The dialog handle is loaded into ecx before each DIALOG_Close and the callee does not read it.
void __fastcall TRADE_CloseWindow(int32_t bReopenInventory)
{
    gbTradeWindowOpen = 0;

    if (gpTradeWaitDialog)
    {
        DIALOG_Close(gpTradeWaitDialog);
        gpTradeWaitDialog = NULL;
    }
    if (gpTradeConfirmDialog)
    {
        DIALOG_Close(gpTradeConfirmDialog);
        gpTradeConfirmDialog = NULL;
    }

    if (gnTradeState != 7)
    {
        if (UI_GetUiWindowState(UI_MPTRADE))
        {
            UI_SetUIToggle(UI_MPTRADE, UI_TURNOFF, 0);
            INV_TradeClearAcceptBlocked();
            INV_ResetPanelState();
            if (!CUNIT_IsPlayerDeadOrMissing() && bReopenInventory)
            {
                UI_SetUIToggle(UI_INVENTORY, UI_TOGGLE, 0);
            }
            if (UI_GetUiWindowState(UI_CHATBOX))
            {
                UI_SetUIToggle(UI_CHATBOX, UI_TURNOFF, 0);
            }
        }
        if (gnTradeState)
        {
            MSG_SendCmd3Words(79, 2, 0, 0);
            gnTradeState = 0;
        }
    }
}

//1.10f: D2Client.0x6FB1F680
//  True in states 1 and 2 only (asked, or asking); every state past 2 is a trade that has already opened both windows.
BOOL __cdecl TRADE_IsTradeInProgress()
{
    return gnTradeState == 1 || gnTradeState == 2;
}

//1.10f: D2Client.0x6FB1F6A0
//  Sends the cancel and, unless the state is already 7, enters the waiting state with the deadline stamped. void: the eax the original leaves
//  is whichever call ran last, and nothing reads it.
void __cdecl TRADE_SendCancelOnce()
{
    MSG_SendCmd3Words(79, 2, 0, 0);
    if (gnTradeState != 7)
    {
        gbTradeCancelSent = 1;
        gnTradeDeadline = GetTickCount();
        gnTradeState = 7;
    }
}

//1.10f: D2Client.0x6FB1F6E0
//  The protocol's state machine, on pMessage[1]; packet 0x77's handler (SCMD_HandleSrv77_TradeState) jumps into it. gnTradeState runs 0..7:
//
//      0   idle                     4,5,6  the two sides' ready flags
//      1   waiting for the other    3      both windows open, nobody ready
//      2   asked, dialog up         7      waiting, and the deadline is stamped with GetTickCount
//
//  Entering state 7 through a cancel stamps gnTradeDeadline; every other transition sets it to -1, so -1 means "not waiting", not "expired".
//  The recurring shape is the cancel-and-wait, which five arms share:
//
//      MSG_SendCmd3Words(79, 2, 0, 0);
//      if (gnTradeState != 7) { gbTradeCancelSent = 1;
//                               gnTradeDeadline = GetTickCount();
//                               gnTradeState = 7; }
//      FOG_Trace(... the state, clamped to 7 for the message only ...);
//
//  The clamp is the trace's, not the machine's: a state above the bound is reported as 7 and handled as itself.
//  The cases 0x10, 0x11 and 0x15 are tail jumps into inv.cpp in the original; here they call INV_StashOpenPanel, INV_StashTeardownPanel and
//  INV_CubeOpenPanel.
void __fastcall TRADE_HandleStateMessage(uint8_t *pMessage)
{
    const Unicode *pwszText;
    int32_t nTraceState;
    D2UnitStrc *pPlayer;
    D2UnitStrc *pPartner;

    switch (pMessage[1])
    {
    case 0:
        //  the other player asks
        if (gnTradeState)
        {
            MSG_SendCmd3Words(79, 2, 0, 0);
            nTraceState = gnTradeState;
            if (gnTradeState != 7)
            {
                gbTradeCancelSent = 1;
                gnTradeDeadline = GetTickCount();
                nTraceState = 7;
                gnTradeState = 7;
            }
            FOG_Trace("waiting for query response mode:%d at line %d of file %s\n", nTraceState, 424, szTradeCpp);
            return;
        }

        if (!UI_GetUiWindowState(UI_MPTRADE))
        {
            if (!UI_SetUIToggle(UI_MPTRADE, UI_TURNON, 0))
            {
                goto CANCEL_AND_WAIT;
            }
            INV_TradeClearAcceptBlocked();
            gnTradeDeadline = -1;
        }
        gnTradeState = 1;
        if (gpTradeWaitDialog)
        {
            FOG_DisplayAssert((uint32_t)szWaitDialogNullAssert, (uint32_t)szTradeCpp, 317);
            exit(-1);
        }
        gpTradeWaitDialog = DIALOG_Create(320, 100, (DIALOGCALLBACK)TRADE_SendCancelEarlyStamp, (DIALOGCALLBACK)TRADE_SendCancelEarlyStamp, 0, 1, 1, 0, 0, 0, 1);
        DIALOG_SetHighlight(gpTradeWaitDialog, 1);
        pwszText = D2LANG_GetStringFromTblIndex(STR_IDX_4043_strWaitingForOtherPlayer);
        DIALOG_AddLine(gpTradeWaitDialog, pwszText, 15, 0, 0, 1, NULL, 0);
        pwszText = D2LANG_GetStringFromTblIndex(STR_IDX_3400_Back);
        DIALOG_AddLine(gpTradeWaitDialog, pwszText, 15, 0, 0, 1, (DIALOGCALLBACK)TRADE_SendCancelEarlyStamp, 1);
        DIALOG_SetSelectedLine(gpTradeWaitDialog, -1);
        INV_TradeSetPanelMode(0);
        return;

    case 1:
        //  the other player answers, and we put the question
        if (gnTradeState)
        {
            MSG_SendCmd3Words(79, 2, 0, 0);
            nTraceState = gnTradeState;
            if (gnTradeState != 7)
            {
                gbTradeCancelSent = 1;
                gnTradeDeadline = GetTickCount();
                nTraceState = 7;
                gnTradeState = 7;
            }
            FOG_Trace(" mode:%d at line %d of file %s\n", nTraceState, 480, szTradeCpp);
            return;
        }

        if (D2COMMON_INVENTORY_GetCursorItem(CUNIT_GetPlayer()->pInventory) || DIALOG_GetNestDepth() || gpTradeConfirmDialog)
        {
            MSG_SendCmd3Words(79, 2, 0, 0);
            return;
        }

        gpTradeConfirmDialog = DIALOG_Create(320, 100, (DIALOGCALLBACK)TRADE_SendCancel, (DIALOGCALLBACK)TRADE_SendCancel, 0, 1, 1, 0, 0, 0, 1);
        DIALOG_SetHighlight(gpTradeConfirmDialog, 1);
        pwszText = D2LANG_GetStringFromTblIndex(STR_IDX_4042_strTradeAgreeTo);
        DIALOG_AddLine(gpTradeConfirmDialog, pwszText, 15, 0, 0, 1, NULL, 0);
        pwszText = D2LANG_GetStringFromTblIndex(STR_IDX_3401_ok);
        DIALOG_AddLine(gpTradeConfirmDialog, pwszText, 15, 0, 0, 1, (DIALOGCALLBACK)TRADE_SendAgreeToTrade, 1);
        pwszText = D2LANG_GetStringFromTblIndex(STR_IDX_3400_Back);
        DIALOG_AddLine(gpTradeConfirmDialog, pwszText, 15, 0, 0, 1, (DIALOGCALLBACK)TRADE_SendCancel, 1);
        DIALOG_SetSelectedLine(gpTradeConfirmDialog, -1);

        if (UI_GetUiWindowState(UI_CHATBOX))
        {
            MSG_SendCmd3Words(79, 2, 0, 0);
            return;
        }
        if (!UI_GetUiWindowState(UI_MPTRADE))
        {
            if (!UI_SetUIToggle(UI_MPTRADE, UI_TURNON, 0))
            {
CANCEL_AND_WAIT:
                MSG_SendCmd3Words(79, 2, 0, 0);
                if (gnTradeState != 7)
                {
                    gbTradeCancelSent = 1;
                    gnTradeDeadline = GetTickCount();
                    gnTradeState = 7;
                }
                return;
            }
            gnTradeDeadline = -1;
        }
        gnTradeState = 2;
        INV_TradeSetPanelMode(0);
        return;

    case 2:
        INV_UpdateToggleSounds(0, 0);
        TRADE_CloseWindow(0);
        return;

    case 5:
        //  the other side is ready
        if (UI_GetUiWindowStateRaw(UI_MPTRADE))
        {
            gnTradeDeadline = -1;
            INV_TradeSetPanelMode(1);
            switch (gnTradeState)
            {
            case 3:
                gnTradeState = 5;
                INV_UpdateToggleSounds(0, 1);
                break;
            case 4:
                gnTradeState = 6;
                INV_UpdateToggleSounds(1, 1);
                break;
            case 5:
                INV_UpdateToggleSounds(0, 1);
                //  the missing space after `recvd` is the original's
                FOG_Trace("other ready mode %d recvd%d at line %d of file %s\n", gnTradeState, pMessage[1], 509, szTradeCpp);
                break;
            case 6:
                FOG_Trace("other ready mode %d recvd %d at line %d of file %s\n", gnTradeState, pMessage[1], 512, szTradeCpp);
                break;
            case 7:
                return;
            default:
                MSG_SendCmd3Words(79, 2, 0, 0);
                nTraceState = gnTradeState;
                if (gnTradeState != 7)
                {
                    gbTradeCancelSent = 1;
                    gnTradeDeadline = GetTickCount();
                    nTraceState = 7;
                    gnTradeState = 7;
                }
                FOG_Trace("other ready mode %d recvd %d at line %d of file %s\n", nTraceState, pMessage[1], 516, szTradeCpp);
                break;
            }
        }
        return;

    case 6:
        //  the other side is NOT ready, and our gold goes out again
        if (UI_GetUiWindowStateRaw(UI_MPTRADE))
        {
            MSG_SendCmd3Words(79, 8, (int16_t)((uint32_t)gnTradeGoldSelf >> 16), (int16_t)gnTradeGoldSelf);
            gnTradeDeadline = -1;
            INV_TradeSetPanelMode(1);
            switch (gnTradeState)
            {
            case 1:
                gnTradeGoldOther = 0;
                INV_UpdateToggleSounds(0, 0);
                gnTradeState = 3;
                if (gpTradeWaitDialog)
                {
                    DIALOG_Close(gpTradeWaitDialog);
                    gpTradeWaitDialog = NULL;
                }
                break;
            case 3:
                INV_UpdateToggleSounds(0, 0);
                break;
            case 4:
            case 5:
            case 6:
                INV_UpdateToggleSounds(0, 0);
                gnTradeState = 3;
                break;
            case 7:
                gnTradeState = 3;
                INV_UpdateToggleSounds(0, 0);
                break;
            default:
                MSG_SendCmd3Words(79, 2, 0, 0);
                nTraceState = gnTradeState;
                if (gnTradeState != 7)
                {
                    gbTradeCancelSent = 1;
                    gnTradeDeadline = GetTickCount();
                    nTraceState = 7;
                    gnTradeState = 7;
                }
                FOG_Trace("other not ready state %d recvd %d at line %d of file %s\n", nTraceState, pMessage[1], 562, szTradeCpp);
                break;
            }
        }
        return;

    case 9:
        pPlayer = CUNIT_GetPlayer();
        UNITSND_PlayPlayerSoundEvent(pPlayer, 0x17);
        return;

    case 0xA:
        pPartner = CUNIT_FindUnitByGuidAndType(gTradePartnerGUID, UNIT_PLAYER);
        if (pPartner)
        {
            UNITSND_PlayPlayerSoundEvent(pPartner, 0x17);
        }
        return;

    case 0xC:
        gnTradeState = 0;
        TRADE_CloseWindow(0);
        return;

    case 0xD:
        gnTradeState = 0;
        TRADE_CloseWindow(1);
        return;

    case 0xE:
        INV_TradeSetAcceptBlocked();
        return;

    case 0xF:
        INV_TradeClearAcceptBlocked();
        return;

    //  cases 0x10, 0x11 and 0x15: INV_StashOpenPanel, INV_StashTeardownPanel and INV_CubeOpenPanel (tail jumps in the original)
    case 0x10:
        INV_StashOpenPanel();
        return;

    case 0x11:
        INV_StashTeardownPanel();
        return;

    case 0x15:
        INV_CubeOpenPanel();
        return;

    default:
        return;
    }
}

//1.10f: D2Client.0x6FB1FD60
//  The confirm dialog's `ok` line: sound, close the dialog, state 3, and the agreement goes out as subcommand 3.
int32_t __cdecl TRADE_SendAgreeToTrade()
{
    SOUNDINST_PlaySound(SOUND_CURSOR_SELECT, NULL, 0, 0, 0);
    DIALOG_Close(gpTradeConfirmDialog);
    gpTradeConfirmDialog = NULL;
    gnTradeState = 3;
    MSG_SendCmd3Words(79, 3, 0, 0);
    return 1;
}

//1.10f: D2Client.0x6FB1FDB0
//  The confirm dialog's cancel line and its outer callback both. It closes the dialog, enters the waiting state and sends the cancel; it
//  stamps the deadline after the send, where TRADE_SendCancelEarlyStamp stamps it before.
int32_t __cdecl TRADE_SendCancel()
{
    SOUNDINST_PlaySound(SOUND_CURSOR_SELECT, NULL, 0, 0, 0);
    DIALOG_Close(gpTradeConfirmDialog);
    gpTradeConfirmDialog = NULL;
    gnTradeState = 7;
    MSG_SendCmd3Words(79, 2, 0, 0);
    gnTradeDeadline = GetTickCount();
    return 1;
}

//1.10f: D2Client.0x6FB1FE10
//  The wait dialog's cancel. The order of its last two statements is the only thing that separates it from TRADE_SendCancel: the deadline is
//  stamped before the cancel goes out.
int32_t __cdecl TRADE_SendCancelEarlyStamp()
{
    SOUNDINST_PlaySound(SOUND_CURSOR_SELECT, NULL, 0, 0, 0);
    DIALOG_Close(gpTradeWaitDialog);
    gpTradeWaitDialog = NULL;
    gnTradeState = 7;
    gnTradeDeadline = GetTickCount();
    MSG_SendCmd3Words(79, 2, 0, 0);
    return 1;
}

//1.10f: D2Client.0x6FB1FE70
//  Packet 0x78's sixteen name bytes into a local, the last byte forced to NUL, then widened into gwszTradePartnerName; the GUID is kept for
//  the sound arm of the state machine. The copy is four dword moves, which is how the compiler spelled sixteen bytes.
void __fastcall TRADE_SetPartnerFromPacket78(D2GSPacketSrv78 *pPacket)
{
    char szName[16];

    *(uint32_t *)szName = *(uint32_t *)pPacket->szName;
    *(uint32_t *)&szName[4] = *(uint32_t *)&pPacket->szName[4];
    *(uint32_t *)&szName[8] = *(uint32_t *)&pPacket->szName[8];
    *(uint32_t *)&szName[12] = *(uint32_t *)&pPacket->szName[12];
    szName[15] = 0;
    Unicode::win2Unicode(gwszTradePartnerName, szName, 16);
    gTradePartnerGUID = pPacket->dwPlayerGUID;
}

//1.10f: D2Client.0x6FB1FEC0
Unicode *__cdecl TRADE_GetPartnerName()
{
    return gwszTradePartnerName;
}

//1.10f: D2Client.0x6FB1FED0
//  The accept: state 7 with no deadline -- the -1 goes in after the send, so this is the one wait TRADE_TickTimeout never expires.
//  void: its one caller has another call before anything reads eax.
void __cdecl TRADE_SendAccept()
{
    gnTradeState = 7;
    MSG_SendCmd3Words(79, 4, (int16_t)((uint32_t)gnTradeGoldSelf >> 16), (int16_t)gnTradeGoldSelf);
    gnTradeDeadline = -1;
}

//1.10f: D2Client.0x6FB1FF00
//  void: no call site reads the send's answer it leaves in eax.
void __cdecl TRADE_SendGreenCheck()
{
    MSG_SendCmd3Words(79, 7, 0, 0);
    gnTradeState = 3;
}

//1.10f: D2Client.0x6FB1FF20
//  The server sets one side's gold. Called by the packet handler SCMD_HandleSrv79_TradeGold, which passes bSelf (packet byte 1) and nGold (the
//  unaligned dword at byte 2) and validates neither, so a server can put any 32-bit value into either side's displayed gold.
void __fastcall TRADE_SetGold(int32_t nGold, uint8_t bSelf)
{
    if (bSelf)
    {
        gnTradeGoldSelf = nGold;
    }
    else
    {
        gnTradeGoldOther = nGold;
    }
}

//1.10f: D2Client.0x6FB1FF40
int32_t __cdecl TRADE_GetGoldOther()
{
    return gnTradeGoldOther;
}

//1.10f: D2Client.0x6FB1FF50
int32_t __cdecl TRADE_GetGoldSelf()
{
    return gnTradeGoldSelf;
}

//1.10f: D2Client.0x6FB1FF60
//  In the three ready states the amount is not sent at all -- the green check goes out instead and the state drops back to 3. Otherwise the
//  amount is clamped to the player's own gold, with an unsigned and strict compare: an amount equal to the stat takes the clamp arm, which
//  sends the same number by another route.
void __fastcall TRADE_SendGoldAmount(uint32_t nGold)
{
    uint32_t nGoldStat;

    if (gnTradeState == 4 || gnTradeState == 5 || gnTradeState == 6)
    {
        MSG_SendCmd3Words(79, 7, 0, 0);
        gnTradeState = 3;
        return;
    }

    nGoldStat = D2COMMON_STATLIST_UnitGetStatValue(CUNIT_GetPlayer(), STAT_GOLD, 0);
    if (nGold < nGoldStat)
    {
        gnTradeGoldSelf = nGold;
        MSG_SendCmd3Words(79, 8, (int16_t)(nGold >> 16), (int16_t)nGold);
    }
    else
    {
        gnTradeGoldSelf = nGoldStat;
        MSG_SendCmd3Words(79, 8, (int16_t)(nGoldStat >> 16), (int16_t)nGoldStat);
    }
}

//1.10f: D2Client.0x6FB1FFF0
//  Thirty seconds in state 7 and the client leaves the game: the panel is reset and closed, MSG_Send_D2CLTSYS_ENDGAME goes out, the app mode
//  is told not to continue and the disconnect is armed. A deadline of -1 never expires, which TRADE_SendAccept relies on.
//  Returns 1 while the state is 7 and 0 otherwise.
int32_t __cdecl TRADE_TickTimeout()
{
    if (gnTradeState != 7)
    {
        return 0;
    }
    if (gnTradeDeadline != -1 && GetTickCount() > gnTradeDeadline + 30000)
    {
        INV_TradeResetPanelState();
        gnTradeState = 0;
        TRADE_CloseWindow(0);
        MSG_Send_D2CLTSYS_ENDGAME();
        WINMAIN_SetAppModeContinue(0);
        GAME_SetLeaveGameSent(1);
        GAME_SetDisconnecting();
    }
    return 1;
}

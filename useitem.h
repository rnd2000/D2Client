//  useitem.h -- the globals of useitem.cpp: the anvil panel's message table, its two button records and their count, and the panel's mode,
//  state, GUIDs, dialog, click tick and the two images (0x6FB90660..0x6FBBA9A0).
//  Each is a C++17 inline variable, so every file that includes this header shares one object. The items from 0x6FBBA978 on lie past the end
//  of .data's raw data (0x6FB9B000): the loader zero-fills them, and ours are zero-initialised.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB906B8
//  The anvil panel's message table, 8 records, registered on the game window when the panel opens and unregistered by every path that closes
//  it: WM_LBUTTONUP and WM_RBUTTONUP USEITEM_AnvilOnButtonUp, WM_LBUTTONDOWN and WM_RBUTTONDOWN USEITEM_AnvilOnButtonDown, WM_MOUSEMOVE
//  USEITEM_AnvilOnMouseMove, VK_ESCAPE and VK_SPACE USEITEM_AnvilOnCloseKey (kind 3, a key binding rather than a window message), and
//  WM_CHAR INV_ConsumeMessage, which only swallows typing so it cannot reach the chat box.
inline D2WinCommandRegisterStrc gAnvilMsgHandlers[8] =
{
    { D2_WINPROC_MESSAGE,   WM_LBUTTONUP,               &USEITEM_AnvilOnButtonUp },
    { D2_WINPROC_MESSAGE,   WM_LBUTTONDOWN,             &USEITEM_AnvilOnButtonDown },
    { D2_WINPROC_MESSAGE,   WM_RBUTTONDOWN,             &USEITEM_AnvilOnButtonDown },
    { D2_WINPROC_MESSAGE,   WM_RBUTTONUP,               &USEITEM_AnvilOnButtonUp },
    { D2_WINPROC_MESSAGE,   WM_MOUSEMOVE,               &USEITEM_AnvilOnMouseMove },
    { D2_WINPROC_KEYDOWN,   VK_ESCAPE,                  &USEITEM_AnvilOnCloseKey },
    { D2_WINPROC_KEYDOWN,   VK_SPACE,                   &USEITEM_AnvilOnCloseKey },
    { D2_WINPROC_MESSAGE,   WM_CHAR,                    &INV_ConsumeMessage },
};
static_assert(sizeof(gAnvilMsgHandlers) == 96, "gAnvilMsgHandlers: the item's 96 bytes in the .i64");

//  D2Client 0x6FB90660
//  The panel's two button records, 40 bytes each, counted by gnPanelButtonCount (0x6FB90660 + 2 * 40 is 0x6FB906B0). Fields:
//  wLabelStrId +0x00, the hover label's tbl index (4017 Upgrade in record 0, 4143 in record 1; USEITEM_AnvilDrawPanel rewrites 4017 to 3401 ok in
//  mode 0, to 22748 Addsockets for vendor 511 and to 22749 Personalize for 512);
//  nX +0x02 and nY +0x06, from which the hover box and its text are placed; nCelX +0x0A and nCelY +0x0E, where the button's cel is drawn (each
//  rectangle's bottom-left corner); nLeft +0x12, nRight +0x16, nTop +0x1A and nBottom +0x1E, the 32 x 32 hit rectangle; nCelIndex +0x22
//  (16 and 10; the pressed frame is the next cel); bState +0x26, which picks the frame; bPressed +0x27, set on a mouse-down and read on the
//  mouse-up to pair a release with its own press.
inline D2AnvilPanelButtonStrc gAnvilPanelButtons[2] =
{
    {   // [0]
        .wLabelStrId = STR_IDX_4017_Upgrade,
        .nX = 122,
        .nY = 256,
        .nCelX = 122,
        .nCelY = 256,
        .nLeft = 122,
        .nRight = 154,
        .nTop = 224,
        .nBottom = 256,
        .nCelIndex = 16,
    },
    {   // [1]
        .wLabelStrId = STR_IDX_4143_close,
        .nX = 177,
        .nY = 256,
        .nCelX = 177,
        .nCelY = 256,
        .nLeft = 177,
        .nRight = 209,
        .nTop = 224,
        .nBottom = 256,
        .nCelIndex = 10,
    },
};
static_assert(sizeof(gAnvilPanelButtons) == 80, "gAnvilPanelButtons: the item's 80 bytes in the .i64");

//  D2Client 0x6FB906B0
//  The number of live entries in gAnvilPanelButtons.
inline int32_t gnPanelButtonCount = 2;
static_assert(sizeof(gnPanelButtonCount) == 4, "gnPanelButtonCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA978
//  Which way the panel was opened: 0 by the server, through USEITEM_AnvilOnPanelPacket sub-command 0 (gnAnvilInteractGUID names the unit);
//  1 locally, by USEITEM_AnvilOpenPanelLocal. Read by the close paths (tell the server, or call NPCMENU_NpcCloseInteraction), the button
//  handlers and the background cel in USEITEM_AnvilDrawPanel.
inline int32_t gnAnvilPanelMode;
static_assert(sizeof(gnAnvilPanelMode) == 4, "gnAnvilPanelMode: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA97C
//  The state of the panel: 0 closed, 1 the item is on the cursor, 2 the item is in the slot, 3 finished. The closing paths store 0 before the
//  swap helper (ANVIL_SwapSlotItem) tests for 1 and 2, so those branches never run there.
inline int32_t gnAnvilState;
static_assert(sizeof(gnAnvilState) == 4, "gnAnvilState: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA980
//  Write-only: USEITEM_AnvilInit clears it and USEITEM_AnvilOpenPanelLocal stores the NPC's GUID in it; nothing reads it (LATENT). Past .data's
//  raw data, so it starts at 0.
inline D2UnitGUID gnAnvilNpcGUID;
static_assert(sizeof(gnAnvilNpcGUID) == 4, "gnAnvilNpcGUID: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA984
//  GUID of the anvil being used, sent as nAnvilInteractGUID in client packet 0x44.
inline int32_t gnAnvilInteractGUID;
static_assert(sizeof(gnAnvilInteractGUID) == 4, "gnAnvilInteractGUID: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA988
//  The unit id of the item currently sitting on the anvil, or NULL when the slot is empty. Set by USEITEM_AnvilOnButtonDown when the item
//  moves from the cursor to the slot and cleared when it moves back.
inline D2UnitGUID gnAnvilSlotUnitId;
static_assert(sizeof(gnAnvilSlotUnitId) == 4, "gnAnvilSlotUnitId: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA98C
//  The panel's current dialog, from DIALOG_Create. USEITEM_AnvilOnPanelClick stores the handle after building the wait dialog (callback
//  USEITEM_NpcMenuWaitDialogRefuseClose); the close paths close it and clear it. A non-null value means a dialog is up and owned by this panel.
inline D2ClientDialogStrc *gpAnvilDialog;
static_assert(sizeof(gpAnvilDialog) == 4, "gpAnvilDialog: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA990
//  GetTickCount at the last accepted anvil click. USEITEM_AnvilOnButtonDown ignores anything within 400 ms of it, which stops a double-click
//  from toggling the item onto the anvil and straight back off.
inline uint32_t gdwAnvilLastClickTick;
static_assert(sizeof(gdwAnvilLastClickTick) == 4, "gdwAnvilLastClickTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA99C
//  The anvil panel image menu\\upgrade: loaded by USEITEM_AnvilInit, read by USEITEM_AnvilDrawPanel, released by USEITEM_AnvilShutdown.
inline D2CellFileStrc *gpImgAnvilUpgrade;
static_assert(sizeof(gpImgAnvilUpgrade) == 4, "gpImgAnvilUpgrade: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA9A0
//  The anvil panel image menu\\horadricback, the Horadric backdrop the panel draws on: loaded last in USEITEM_AnvilInit, read by
//  USEITEM_AnvilDrawPanel, released by USEITEM_AnvilShutdown.
inline D2CellFileStrc *gpImgAnvilBackground;
static_assert(sizeof(gpImgAnvilBackground) == 4, "gpImgAnvilBackground: the item's 4 bytes in the .i64");

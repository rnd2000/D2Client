//  config.h -- the globals of config.cpp, the Configure Controls screen: the capture tables, the three buttons, the two row lists and the list
//  pointer, the bind state, and the scroll and mouse state. Address range 0x6FB7C828..0x6FBB5E14. Each is a C++17 inline variable, one object for
//  every file that includes this header. The comment line "D2Client 0x6F..." above a global gives its address in the original.
//  gCfgCaptureMButtonHandler, gCfgCaptureXButtonHandler and gCfgCaptureWheelHandler are one record each, not an array of one. The globals from
//  0x6FBB5DB0 on lie past .data's raw data (which ends at 0x6FB9B000) and are zero at load, except gnCfgScrollThumbTop, which is initialised the
//  way the original's static initialiser does it (from gnViewportBottom), which is why this header includes CoreArchive.h.
//  Include it after D2Client.h.
#pragma once

#include "CoreArchive.h"

//  D2Client 0x6FB7CCF0
//  6 message records installed while the Configure Controls screen waits for a key: WM_LBUTTONDOWN and WM_RBUTTONDOWN to CONFIG_CaptureMouseButton,
//  WM_KEYDOWN/WM_SYSKEYDOWN to CONFIG_CaptureKeyDown, WM_KEYUP/WM_SYSKEYUP to CONFIG_CaptureKeyUp. Registered by CONFIG_OnEnterBeginCapture and
//  CONFIG_OnMouseUp, not by UI_RegisterPanelHandlers.
inline D2WinCommandRegisterStrc gCfgCaptureKeyHandlers[6] =
{
    { D2_WINPROC_MESSAGE,   WM_LBUTTONDOWN,             &CONFIG_CaptureMouseButton },
    { D2_WINPROC_MESSAGE,   WM_RBUTTONDOWN,             &CONFIG_CaptureMouseButton },
    { D2_WINPROC_MESSAGE,   WM_KEYDOWN,                 &CONFIG_CaptureKeyDown },
    { D2_WINPROC_MESSAGE,   WM_SYSKEYDOWN,              &CONFIG_CaptureKeyDown },
    { D2_WINPROC_MESSAGE,   WM_KEYUP,                   &CONFIG_CaptureKeyUp },
    { D2_WINPROC_MESSAGE,   WM_SYSKEYUP,                &CONFIG_CaptureKeyUp },
};
static_assert(sizeof(gCfgCaptureKeyHandlers) == 72, "gCfgCaptureKeyHandlers: the item's 72 bytes in the .i64");

//  D2Client 0x6FB7CD38
//  1 record: WM_MBUTTONDOWN -> CONFIG_CaptureMiddleButton. Registered only when GAME_IsSupportedWindows().
inline D2WinCommandRegisterStrc gCfgCaptureMButtonHandler =
{ D2_WINPROC_MESSAGE,   WM_MBUTTONDOWN,             &CONFIG_CaptureMiddleButton };
static_assert(sizeof(gCfgCaptureMButtonHandler) == 12, "gCfgCaptureMButtonHandler: the item's 12 bytes in the .i64");

//  D2Client 0x6FB7CD44
//  1 record: message 0x20B (WM_XBUTTONDOWN) -> CONFIG_CaptureXButton. Registered only when GAME_IsWin2000OrXp().
inline D2WinCommandRegisterStrc gCfgCaptureXButtonHandler =
{ D2_WINPROC_MESSAGE,   0x20B,                      &CONFIG_CaptureXButton };
static_assert(sizeof(gCfgCaptureXButtonHandler) == 12, "gCfgCaptureXButtonHandler: the item's 12 bytes in the .i64");

//  D2Client 0x6FB7CD50
//  1 record: WM_MOUSEWHEEL -> CONFIG_CaptureMouseWheel. Registered only when GAME_HasNativeMouseWheel().
inline D2WinCommandRegisterStrc gCfgCaptureWheelHandler =
{ D2_WINPROC_MESSAGE,   WM_MOUSEWHEEL,              &CONFIG_CaptureMouseWheel };
static_assert(sizeof(gCfgCaptureWheelHandler) == 12, "gCfgCaptureWheelHandler: the item's 12 bytes in the .i64");

//  D2Client 0x6FB7CC98
//  The three buttons at the foot of the Configure Controls screen: 3 records of 26 bytes.
//      +0x00 word   tbl index of the label
//      +0x02 dword  click handler
//      +0x06 dword  a small id (1, 3, 2)
//      +0x0A .. +0x16  left, right, top, bottom -- filled in by CONFIG_LayoutControlsButtons
//
//      [0] 3974 CfgCancel   CONFIG_OnCancelButton   drawn leftmost
//      [1] 3972 CfgDefault  CONFIG_OnDefaultButton
//      [2] 3973 CfgAccept   CONFIG_OnAcceptButton
inline D2ClientCfgControlsButtonStrc gCfgControlsButtons[3] =
{
    {   // [0]
        .nLabelStrId = STR_IDX_3974_CfgCancel,
        .pfnOnClick = &CONFIG_OnCancelButton,
        .nHoverColor = 1,
        .nLeft = 0,
        .nRight = 0,
        .nTop = 0,
        .nBottom = 0,
    },
    {   // [1]
        .nLabelStrId = STR_IDX_3972_CfgDefault,
        .pfnOnClick = &CONFIG_OnDefaultButton,
        .nHoverColor = 3,
        .nLeft = 0,
        .nRight = 0,
        .nTop = 0,
        .nBottom = 0,
    },
    {   // [2]
        .nLabelStrId = STR_IDX_3973_CfgAccept,
        .pfnOnClick = &CONFIG_OnAcceptButton,
        .nHoverColor = 2,
        .nLeft = 0,
        .nRight = 0,
        .nTop = 0,
        .nBottom = 0,
    },
};
static_assert(sizeof(gCfgControlsButtons) == 78, "gCfgControlsButtons: the item's 78 bytes in the .i64");

//  D2Client 0x6FB7C828
//  The Configure Controls screen's row list: 51 records of 10 bytes (D2CfgControlsRowStrc): +0 the key-binding command, +4 the tbl index of the
//  label (e.g. 3925 `CfgInventory`), +6 a dword that is zero in every row of both lists. A command of 57 (HOTKEY_MAX_COUNT) is a blank separator
//  row.
inline D2CfgControlsRowStrc gCfgControlsList[51] =
{
    { .nTblIndex = 3924 },   // [0]
    { .eCmd = HOTKEY_INVENTORY, .nTblIndex = 3925 },   // [1]
    { .eCmd = HOTKEY_PARTY, .nTblIndex = 3926 },   // [2]
    { .eCmd = HOTKEY_MSGLOG, .nTblIndex = 3927 },   // [3]
    { .eCmd = HOTKEY_QUESTS, .nTblIndex = 3928 },   // [4]
    { .eCmd = HOTKEY_HELP, .nTblIndex = 3933 },   // [5]
    { .eCmd = HOTKEY_MAX_COUNT },   // [6]
    { .eCmd = HOTKEY_SKILLTREE, .nTblIndex = 3934 },   // [7]
    { .eCmd = HOTKEY_SKILLSELECT, .nTblIndex = 10833 },   // [8]
    { .eCmd = HOTKEY_SKILL1, .nTblIndex = 3936 },   // [9]
    { .eCmd = HOTKEY_SKILL2, .nTblIndex = 3937 },   // [10]
    { .eCmd = HOTKEY_SKILL3, .nTblIndex = 3938 },   // [11]
    { .eCmd = HOTKEY_SKILL4, .nTblIndex = 3939 },   // [12]
    { .eCmd = HOTKEY_SKILL5, .nTblIndex = 3940 },   // [13]
    { .eCmd = HOTKEY_SKILL6, .nTblIndex = 3941 },   // [14]
    { .eCmd = HOTKEY_SKILL7, .nTblIndex = 3942 },   // [15]
    { .eCmd = HOTKEY_SKILL8, .nTblIndex = 3943 },   // [16]
    { .eCmd = HOTKEY_PREVSKILL, .nTblIndex = 3944 },   // [17]
    { .eCmd = HOTKEY_NEXTSKILL, .nTblIndex = 3945 },   // [18]
    { .eCmd = HOTKEY_MAX_COUNT },   // [19]
    { .eCmd = HOTKEY_SHOWBELT, .nTblIndex = 3946 },   // [20]
    { .eCmd = HOTKEY_POTION1, .nTblIndex = 3947 },   // [21]
    { .eCmd = HOTKEY_POTION2, .nTblIndex = 3948 },   // [22]
    { .eCmd = HOTKEY_POTION3, .nTblIndex = 3949 },   // [23]
    { .eCmd = HOTKEY_POTION4, .nTblIndex = 3950 },   // [24]
    { .eCmd = HOTKEY_MAX_COUNT },   // [25]
    { .eCmd = HOTKEY_CHAT, .nTblIndex = 3929 },   // [26]
    { .eCmd = HOTKEY_RUN, .nTblIndex = 3966 },   // [27]
    { .eCmd = HOTKEY_RUNWALK, .nTblIndex = 3967 },   // [28]
    { .eCmd = HOTKEY_STANDSTILL, .nTblIndex = 3968 },   // [29]
    { .eCmd = HOTKEY_SHOWITEMS, .nTblIndex = 3969 },   // [30]
    { .eCmd = HOTKEY_PORTRAITS, .nTblIndex = 3982 },   // [31]
    { .eCmd = HOTKEY_MAX_COUNT },   // [32]
    { .eCmd = HOTKEY_AUTOMAP, .nTblIndex = 3930 },   // [33]
    { .eCmd = HOTKEY_MINIMAP_POSITION, .nTblIndex = 3931 },   // [34]
    { .eCmd = HOTKEY_FADEAUTOMAP, .nTblIndex = 3983 },   // [35]
    { .eCmd = HOTKEY_PARTYONMAP, .nTblIndex = 3985 },   // [36]
    { .eCmd = HOTKEY_NAMESONMAP, .nTblIndex = 3984 },   // [37]
    { .eCmd = HOTKEY_MAX_COUNT },   // [38]
    { .eCmd = HOTKEY_SPEECH1, .nTblIndex = 3959 },   // [39]
    { .eCmd = HOTKEY_SPEECH2, .nTblIndex = 3960 },   // [40]
    { .eCmd = HOTKEY_SPEECH3, .nTblIndex = 3961 },   // [41]
    { .eCmd = HOTKEY_SPEECH4, .nTblIndex = 3962 },   // [42]
    { .eCmd = HOTKEY_SPEECH5, .nTblIndex = 3963 },   // [43]
    { .eCmd = HOTKEY_SPEECH6, .nTblIndex = 3964 },   // [44]
    { .eCmd = HOTKEY_SPEECH7, .nTblIndex = 3965 },   // [45]
    { .eCmd = HOTKEY_SPEECH8, .nTblIndex = 11083 },   // [46]
    { .eCmd = HOTKEY_MAX_COUNT },   // [47]
    { .eCmd = HOTKEY_SCREENSHOT, .nTblIndex = 3971 },   // [48]
    { .eCmd = HOTKEY_CLEARSCREEN, .nTblIndex = 3970 },   // [49]
    { .eCmd = HOTKEY_CLEARMESSAGES, .nTblIndex = 3981 },   // [50]
};
static_assert(sizeof(gCfgControlsList) == 510, "gCfgControlsList: the item's 510 bytes in the .i64");

//  D2Client 0x6FB7CA28
//  The expansion form of the row list: 62 records of 10 bytes, against gCfgControlsList's 51. CONFIG_SelectControlsList picks between them on
//  GAME_IsExpansionGame().
inline D2CfgControlsRowStrc gCfgControlsListExpansion[62] =
{
    { .nTblIndex = 3924 },   // [0]
    { .eCmd = HOTKEY_INVENTORY, .nTblIndex = 3925 },   // [1]
    { .eCmd = HOTKEY_PARTY, .nTblIndex = 3926 },   // [2]
    { .eCmd = HOTKEY_MERCINV, .nTblIndex = 22727 },   // [3]
    { .eCmd = HOTKEY_MSGLOG, .nTblIndex = 3927 },   // [4]
    { .eCmd = HOTKEY_QUESTS, .nTblIndex = 3928 },   // [5]
    { .eCmd = HOTKEY_HELP, .nTblIndex = 3933 },   // [6]
    { .eCmd = HOTKEY_MAX_COUNT },   // [7]
    { .eCmd = HOTKEY_SKILLTREE, .nTblIndex = 3934 },   // [8]
    { .eCmd = HOTKEY_SKILLSELECT, .nTblIndex = 10833 },   // [9]
    { .eCmd = HOTKEY_SKILL1, .nTblIndex = 3936 },   // [10]
    { .eCmd = HOTKEY_SKILL2, .nTblIndex = 3937 },   // [11]
    { .eCmd = HOTKEY_SKILL3, .nTblIndex = 3938 },   // [12]
    { .eCmd = HOTKEY_SKILL4, .nTblIndex = 3939 },   // [13]
    { .eCmd = HOTKEY_SKILL5, .nTblIndex = 3940 },   // [14]
    { .eCmd = HOTKEY_SKILL6, .nTblIndex = 3941 },   // [15]
    { .eCmd = HOTKEY_SKILL7, .nTblIndex = 3942 },   // [16]
    { .eCmd = HOTKEY_SKILL8, .nTblIndex = 3943 },   // [17]
    { .eCmd = HOTKEY_SKILL9, .nTblIndex = 22717 },   // [18]
    { .eCmd = HOTKEY_SKILL10, .nTblIndex = 22718 },   // [19]
    { .eCmd = HOTKEY_SKILL11, .nTblIndex = 22719 },   // [20]
    { .eCmd = HOTKEY_SKILL12, .nTblIndex = 22720 },   // [21]
    { .eCmd = HOTKEY_SKILL13, .nTblIndex = 22721 },   // [22]
    { .eCmd = HOTKEY_SKILL14, .nTblIndex = 22722 },   // [23]
    { .eCmd = HOTKEY_SKILL15, .nTblIndex = 22723 },   // [24]
    { .eCmd = HOTKEY_SKILL16, .nTblIndex = 22724 },   // [25]
    { .eCmd = HOTKEY_PREVSKILL, .nTblIndex = 3944 },   // [26]
    { .eCmd = HOTKEY_NEXTSKILL, .nTblIndex = 3945 },   // [27]
    { .eCmd = HOTKEY_MAX_COUNT },   // [28]
    { .eCmd = HOTKEY_SHOWBELT, .nTblIndex = 3946 },   // [29]
    { .eCmd = HOTKEY_POTION1, .nTblIndex = 3947 },   // [30]
    { .eCmd = HOTKEY_POTION2, .nTblIndex = 3948 },   // [31]
    { .eCmd = HOTKEY_POTION3, .nTblIndex = 3949 },   // [32]
    { .eCmd = HOTKEY_POTION4, .nTblIndex = 3950 },   // [33]
    { .eCmd = HOTKEY_SWITCHWEAP, .nTblIndex = 22726 },   // [34]
    { .eCmd = HOTKEY_MAX_COUNT },   // [35]
    { .eCmd = HOTKEY_CHAT, .nTblIndex = 3929 },   // [36]
    { .eCmd = HOTKEY_RUN, .nTblIndex = 3966 },   // [37]
    { .eCmd = HOTKEY_RUNWALK, .nTblIndex = 3967 },   // [38]
    { .eCmd = HOTKEY_STANDSTILL, .nTblIndex = 3968 },   // [39]
    { .eCmd = HOTKEY_SHOWITEMS, .nTblIndex = 3969 },   // [40]
    { .eCmd = HOTKEY_PORTRAITS, .nTblIndex = 3982 },   // [41]
    { .eCmd = HOTKEY_MAX_COUNT },   // [42]
    { .eCmd = HOTKEY_AUTOMAP, .nTblIndex = 3930 },   // [43]
    { .eCmd = HOTKEY_MINIMAP_POSITION, .nTblIndex = 3931 },   // [44]
    { .eCmd = HOTKEY_FADEAUTOMAP, .nTblIndex = 3983 },   // [45]
    { .eCmd = HOTKEY_PARTYONMAP, .nTblIndex = 3985 },   // [46]
    { .eCmd = HOTKEY_NAMESONMAP, .nTblIndex = 3984 },   // [47]
    { .eCmd = HOTKEY_MINIMAP, .nTblIndex = 22725 },   // [48]
    { .eCmd = HOTKEY_MAX_COUNT },   // [49]
    { .eCmd = HOTKEY_SPEECH1, .nTblIndex = 3959 },   // [50]
    { .eCmd = HOTKEY_SPEECH2, .nTblIndex = 3960 },   // [51]
    { .eCmd = HOTKEY_SPEECH3, .nTblIndex = 3961 },   // [52]
    { .eCmd = HOTKEY_SPEECH4, .nTblIndex = 3962 },   // [53]
    { .eCmd = HOTKEY_SPEECH5, .nTblIndex = 3963 },   // [54]
    { .eCmd = HOTKEY_SPEECH6, .nTblIndex = 3964 },   // [55]
    { .eCmd = HOTKEY_SPEECH7, .nTblIndex = 3965 },   // [56]
    { .eCmd = HOTKEY_SPEECH8, .nTblIndex = 11083 },   // [57]
    { .eCmd = HOTKEY_MAX_COUNT },   // [58]
    { .eCmd = HOTKEY_SCREENSHOT, .nTblIndex = 3971 },   // [59]
    { .eCmd = HOTKEY_CLEARSCREEN, .nTblIndex = 3970 },   // [60]
    { .eCmd = HOTKEY_CLEARMESSAGES, .nTblIndex = 3981 },   // [61]
};
static_assert(sizeof(gCfgControlsListExpansion) == 620, "gCfgControlsListExpansion: the item's 620 bytes in the .i64");

//  D2Client 0x6FB7CC94
//  Which row list the screen is showing: gCfgControlsList (51 rows, classic) or gCfgControlsListExpansion (62 rows). Set by
//  CONFIG_SelectControlsList and read as `gpCfgControlsList[gnCfgSelectedRow]`, whose first dword is the command id. It sits 4 bytes below
//  gCfgControlsButtons and is not part of it.
inline D2CfgControlsRowStrc *gpCfgControlsList = gCfgControlsList;
static_assert(sizeof(gpCfgControlsList) == 4, "gpCfgControlsList: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7CCE8
//  Which of a row's two binding slots is highlighted, and the bPrimary argument of the bind call: 1 is the primary binding, in the LEFT column, 0
//  the secondary, on the right. CONFIG_OnArrowLeft sets 0 -> 1 and CONFIG_OnArrowRight sets 1 -> 0.
inline int32_t gnCfgBindColumn = 1;
static_assert(sizeof(gnCfgBindColumn) == 4, "gnCfgBindColumn: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7CCEC
//  Armed: the screen acts on the next key event. CONFIG_OnEnterBeginCapture clears it the instant a capture begins, and the first capture handler
//  that sees it clear sets it again and does nothing, which swallows the release of the key that began the capture. CONFIG_CaptureKeyDown sets it
//  after an ordinary key and after a failed bind; only Escape and Enter, Delete, Backspace and Space clear it.
inline BOOL gbCfgBindArmed = 1;
static_assert(sizeof(gbCfgBindArmed) == 4, "gbCfgBindArmed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5DB0
//  Screen Y of the top of the controls list's scrollbar thumb. With gnCfgScrollThumbRows it defines the three regions of the scrollbar track that
//  CONFIG_UpdateControlsMouseState distinguishes: above the thumb, the thumb, and below it. Written by CONFIG_ScrollList, read by the panel draw.
//  Initialised as the original's static initialiser does: (gnViewportBottom - 420) / 2 + 47, a signed halving; gnViewportBottom is CoreArchive.h's,
//  defined first in every file that defines this one.
inline int32_t gnCfgScrollThumbTop = (gnViewportBottom - 420) / 2 + 47;
static_assert(sizeof(gnCfgScrollThumbTop) == 4, "gnCfgScrollThumbTop: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5DB4
//  62 for the expansion list, 51 for the classic one; written by CONFIG_SelectControlsList alongside the list pointer.
inline int32_t gnCfgControlsRowCount;
static_assert(sizeof(gnCfgControlsRowCount) == 4, "gnCfgControlsRowCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5DB8
//  The thumb's height in 12-pixel units: the thumb region is gnCfgScrollThumbTop .. gnCfgScrollThumbTop + 12 * gnCfgScrollThumbRows. Written by
//  CONFIG_SelectControlsList, read by CONFIG_ScrollList.
inline int32_t gnCfgScrollThumbRows;
static_assert(sizeof(gnCfgScrollThumbRows) == 4, "gnCfgScrollThumbRows: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5DBC
//  The last first visible row: the row count less the fifteen visible rows (47 for the expansion list, 36 for the classic one). Written by
//  CONFIG_SelectControlsList, read by the thumb drag in CONFIG_UpdateControlsMouseState.
inline int32_t gnCfgMaxFirstVisibleRow;
static_assert(sizeof(gnCfgMaxFirstVisibleRow) == 4, "gnCfgMaxFirstVisibleRow: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5DC0
//  Whether the mouse button is held on the controls screen. CONFIG_UpdateControlsMouseState copies it into gaCfgRegionPressed[0] and gates all of
//  its scrolling on it, and it is the value latched into gbCfgMouseWasDown when nothing is hit.
inline BOOL gbCfgMouseButtonDown;
static_assert(sizeof(gbCfgMouseButtonDown) == 4, "gbCfgMouseButtonDown: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5DC4
//  The previous frame's gbCfgMouseButtonDown, latched by CONFIG_UpdateControlsMouseState when the cursor is over none of the seven regions. The
//  whole body of that function runs only while this is clear, so once a press has landed outside everything the screen stops re-evaluating regions
//  until the button comes back up, which is what makes a drag stick to the region it started in.
inline BOOL gbCfgMouseWasDown;
static_assert(sizeof(gbCfgMouseWasDown) == 4, "gbCfgMouseWasDown: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5DC8
//  One flag per interactive region of the controls screen (seven), set and cleared by CONFIG_UpdateControlsMouseState and read by CONFIG_OnMouseUp;
//  element 0 is the list itself. The walks over it end at the address after its seven ints, gnCfgThumbDragStartTop's.
inline int32_t gaCfgRegionPressed[7];
static_assert(sizeof(gaCfgRegionPressed) == 28, "gaCfgRegionPressed: the item's 28 bytes in the .i64");

//  D2Client 0x6FBB5DE4
//  The thumb's top when a thumb drag starts; written and read by CONFIG_UpdateControlsMouseState. It sits right after gaCfgRegionPressed, so most
//  of its references are that array's end compare.
inline int32_t gnCfgThumbDragStartTop;
static_assert(sizeof(gnCfgThumbDragStartTop) == 4, "gnCfgThumbDragStartTop: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5DE8
//  The cursor's y recorded when a thumb drag starts; written and read by CONFIG_UpdateControlsMouseState.
inline int32_t gnCfgThumbDragStartY;
static_assert(sizeof(gnCfgThumbDragStartY) == 4, "gnCfgThumbDragStartY: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5DEC
//  Index of the row at the top of the controls list's fifteen-row window. Read by CONFIG_HitTestRow to turn a Y into a row, adjusted by
//  CONFIG_MoveSelection to keep the selection visible, and used by CONFIG_DrawKeyBindingsPanel and CONFIG_ScrollList. CONFIG_MoveSelection writes
//  it directly when scrolling up but goes through CONFIG_ScrollList(1) when scrolling down, so the two directions are not symmetric.
inline int32_t gnCfgFirstVisibleRow;
static_assert(sizeof(gnCfgFirstVisibleRow) == 4, "gnCfgFirstVisibleRow: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5DF0
//  The highlighted row of the Configure Controls list; the index into gpCfgControlsList. Zeroed by CONFIG_InitControlsScreen, written by
//  CONFIG_MoveSelection.
inline int32_t gnCfgSelectedRow;
static_assert(sizeof(gnCfgSelectedRow) == 4, "gnCfgSelectedRow: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5DF4
//  Which of the panel's three bottom buttons the cursor is over; 3 is the 'none' value, not -1. CONFIG_UpdateControlsMouseState stores 3 at entry
//  and overwrites it with the loop counter when one of CONFIG_HitTestButton's three rects matches; CONFIG_DrawKeyBindingsPanel compares its own
//  counter against it to pick the highlight colour, and CONFIG_OnMouseUp reads it to decide which button was released on.
inline int32_t gnCfgHoveredButton;
static_assert(sizeof(gnCfgHoveredButton) == 4, "gnCfgHoveredButton: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5DF8
//  Non-zero while the screen is swallowing input to bind it. Set by CONFIG_OnEnterBeginCapture and CONFIG_OnMouseUp, cleared by every capture
//  handler on the way out, including the Escape cancel.
inline BOOL gbCfgCapturingBind;
static_assert(sizeof(gbCfgCapturingBind) == 4, "gbCfgCapturingBind: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5DFC
//  data\global\UI\Menu\TextSlid, loaded by CONFIG_InitControlsScreen.
inline D2CellFileStrc *gpImgCfgTextSlider;
static_assert(sizeof(gpImgCfgTextSlider) == 4, "gpImgCfgTextSlider: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E00
//  The tick of the last auto-scroll step in the Configure Controls list, with a 50 ms repeat interval: each of the four scrollable regions in
//  CONFIG_UpdateControlsMouseState steps only when GetTickCount() is more than 50 above it.
inline uint32_t gnCfgScrollRepeatTick;
static_assert(sizeof(gnCfgScrollRepeatTick) == 4, "gnCfgScrollRepeatTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E04
//  The tbl index of the message shown when a key binding is refused. Five capture handlers (not CONFIG_CaptureMouseButton) store it, with the tick
//  in gnCfgBindErrorTick, when CMDTBL_BindKey returns false and leaves a non-zero index in its out-parameter; CONFIG_DrawKeyBindingsPanel then
//  draws it centred, through D2LANG_GetStringFromTblIndex, for as long as the stamp is fresh. Written from a WORD out-parameter; the draw loads a
//  dword whose upper half is zero.
inline D2C_StringIndices gnCfgBindErrorStrId;
static_assert(sizeof(gnCfgBindErrorStrId) == 2, "gnCfgBindErrorStrId: the item's 2 bytes in the .i64");

//  D2Client 0x6FBB5E08
//  When the binding-error message was raised, stored by the same five capture handlers beside the string id. CONFIG_DrawKeyBindingsPanel shows
//  gnCfgBindErrorStrId while `GetTickCount() < gnCfgBindErrorTick + 2000`, so the message lasts two seconds and nothing else clears it.
inline uint32_t gnCfgBindErrorTick;
static_assert(sizeof(gnCfgBindErrorTick) == 4, "gnCfgBindErrorTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E0C
//  A draw counter, incremented last by CONFIG_DrawKeyBindingsPanel and read by the blink, which skips the selected row's bind column on five drawn
//  frames in sixteen. It counts draws, not time.
inline uint32_t gnCfgFrameCounter;
static_assert(sizeof(gnCfgFrameCounter) == 4, "gnCfgFrameCounter: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E10
//  The cursor X the mouse model last ran for; with gnCfgLastMouseY it gates CONFIG_UpdateControlsMouseState to frames where the cursor moved or the
//  button is down.
inline int32_t gnCfgLastMouseX;
static_assert(sizeof(gnCfgLastMouseX) == 4, "gnCfgLastMouseX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E14
//  The cursor Y the mouse model last ran for; see gnCfgLastMouseX.
inline int32_t gnCfgLastMouseY;
static_assert(sizeof(gnCfgLastMouseY) == 4, "gnCfgLastMouseY: the item's 4 bytes in the .i64");

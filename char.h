//  char.h -- the globals of char.cpp that are ours: the UI button's two vtables, the character panel's tables (chance-to-hit boxes, button
//  images, labels, stat rows, stat buttons) and three single items. Each is a C++17 `inline` variable, one object for every file that includes
//  this header. The comment line "D2Client 0x6F..." above a global gives its address in the original. gCharPanelHiliteBoxes lies in .rdata and
//  is const. The items from 0x6FBB1A44 on lie past the end of .data's raw data (0x6FB9B000): the loader zero-fills them, and ours are
//  zero-initialised. Include it after D2Client.h, which declares every function named here.
#pragma once

//  D2Client 0x6FB6EE98
//  The vtable of the UI button control's body, 13 slots, installed by CHAR_UiButtonConstruct and reinstalled by CHAR_UiButtonDestruct.
//  Slots: 0 CHAR_UiButtonVecDelDtor, 1 CHAR_UiButtonDraw, 2 EDITBOX_DialogNullCallback, 3 CHAR_UiButtonOnMouseDown, 4 CHAR_UiButtonOnMouseUp,
//  5 EDITBOX_DialogNullCallback, 6 CHAR_UiButtonOnChar, 7-10 EDITBOX_DialogNullCallback, 11 EDITBOX_ReturnZeroCallback, 12 EDITBOX_UiCtrlSetFlag24.
//  Each slot is typed by its function; the __thiscall slots are written __fastcall with nEdxUnused.
inline const D2UiButtonImplVtbl gVtblUiButtonImpl =
{
    .pfnDeletingDtor = &CHAR_UiButtonVecDelDtor,
    .pfnDraw = &CHAR_UiButtonDraw,
    .pfnOnMouseWheel = &EDITBOX_DialogNullCallback,
    .pfnOnMouseDown = &CHAR_UiButtonOnMouseDown,
    .pfnOnMouseUp = &CHAR_UiButtonOnMouseUp,
    .pfnOnMouseMove = &EDITBOX_DialogNullCallback,
    .pfnOnChar = &CHAR_UiButtonOnChar,
    .pfnOnArrowUp = &EDITBOX_DialogNullCallback,
    .pfnOnArrowDown = &EDITBOX_DialogNullCallback,
    .pfnOnArrowRight = &EDITBOX_DialogNullCallback,
    .pfnOnArrowLeft = &EDITBOX_DialogNullCallback,
    .pfnGetValue = &EDITBOX_ReturnZeroCallback,
    .pfnSetFlag24 = &EDITBOX_UiCtrlSetFlag24,
};
static_assert(sizeof(gVtblUiButtonImpl) == 52, "gVtblUiButtonImpl: the item's 52 bytes in the .i64");

//  D2Client 0x6FB6EECC
//  The button proxy's vtable, 13 slots, installed by CHAR_UiButtonProxyConstruct and reinstalled by CHAR_UiButtonProxyDestruct. Slot 0 is its
//  own vector deleting destructor; slots 1-12 are the shared UICTRLPROXY_FwdSlot forwarders to body slots 1-12. Typed as the body's table is.
inline const D2UiButtonProxyVtbl gVtblUiButtonProxy =
{
    .pfnDeletingDtor = &CHAR_UiButtonProxyVecDelDtor,
    .pfnDraw = &LISTBOX_UiCtrlProxyFwdSlot1,
    .pfnOnMouseWheel = &LISTBOX_UiCtrlProxyFwdSlot2,
    .pfnOnMouseDown = &SPELLSEL_UiCtrlProxyFwdSlot3,
    .pfnOnMouseUp = &EDITBOX_UiCtrlProxyFwdSlot4,
    .pfnOnMouseMove = &EDITBOX_UiCtrlProxyFwdSlot5,
    .pfnOnChar = &EDITBOX_UiCtrlProxyFwdSlot6,
    .pfnOnArrowUp = &EDITBOX_UiCtrlProxyFwdSlot7,
    .pfnOnArrowDown = &EDITBOX_UiCtrlProxyFwdSlot8,
    .pfnOnArrowRight = &EDITBOX_UiCtrlProxyFwdSlot9,
    .pfnOnArrowLeft = &EDITBOX_UiCtrlProxyFwdSlot10,
    .pfnGetValue = &EDITBOX_UiCtrlProxyFwdSlot11,
    .pfnSetFlag24 = &EDITBOX_UiCtrlProxyFwdSlot12,
};
static_assert(sizeof(gVtblUiButtonProxy) == 52, "gVtblUiButtonProxy: the item's 52 bytes in the .i64");

//  D2Client 0x6FB6EF00
//  POINT[2][3], 48 bytes: two rows of three points of the chance-to-hit readout of CHAR_CharPanelDraw. [n][0] is the corner of a fixed
//  0x9B x 0x1E box (GFX_DrawFilledRect) behind the readout, [n][1] the position of the text tbl 0x103F "Average chance to hit", [n][2] that
//  of the text built by sprintf from tbl 0x2777. The points are offset by gnBorderXDifference / gnBorderYDifference, the y also by
//  `+ gnScreenHeight - 0x1E0`. Row 0 is (154,115) (159,128) (159,140); row 1 is (154,130) (159,143) (159,155).
inline const POINT gCharPanelHiliteBoxes[2][3] =
{
    { { .x = 154, .y = 115 }, { .x = 159, .y = 128 }, { .x = 159, .y = 140 } },
    { { .x = 154, .y = 130 }, { .x = 159, .y = 143 }, { .x = 159, .y = 155 } },
};
static_assert(sizeof(gCharPanelHiliteBoxes) == 48, "gCharPanelHiliteBoxes: the item's 48 bytes in the .i64");

//  D2Client 0x6FB78008
//  D2UiButtonImageStrc[2], {wFrame, wCaptionStrId}, indexed by a button's nImageIndex: {16, tbl 3401 'ok'} and {10, tbl 4142 'cancel'}.
//  CHAR_UiButtonConstruct reads wFrame and CHAR_UiButtonDraw wCaptionStrId.
inline D2UiButtonImageStrc gUiButtonImages[2] = { { .wFrame = 16, .wCaptionStrId = 3401 }, { .wFrame = 10, .wCaptionStrId = 4142 } };
static_assert(sizeof(gUiButtonImages) == 8, "gUiButtonImages: the item's 8 bytes in the .i64");

//  D2Client 0x6FB78010
//  D2CharPanelLabelStrc[15], the character panel's labels, 18 bytes each (the struct is #pragma pack(1): nTextWidth is at +14). nLeft and
//  nRight are offset by gnBorderXDifference, nY by gnBorderYDifference; wStringId is a tbl index for D2LANG_GetStringFromTblIndex; nTextWidth
//  is written at start-up (the Unicode::strlen of the resolved label), not constant data.
inline D2CharPanelLabelStrc gCharPanelLabels[15] =
{
    { .nLeft = 11, .nY = 44, .nRight = 52, .wStringId = 4057, .nTextWidth = 6 },
    { .nLeft = 65, .nY = 44, .nRight = 180, .wStringId = 4058, .nTextWidth = 11 },
    { .nLeft = 193, .nY = 44, .nRight = 308, .wStringId = 4059, .nTextWidth = 11 },
    { .nLeft = 10, .nY = 97, .nRight = 73, .wStringId = 4060, .nTextWidth = 9 },
    { .nLeft = 10, .nY = 160, .nRight = 73, .wStringId = 4062, .nTextWidth = 10 },
    { .nLeft = 174, .nY = 207, .nRight = 268, .wStringId = 4064, .nTextWidth = 8 },
    { .nLeft = 10, .nY = 245, .nRight = 73, .wStringId = 4066, .nTextWidth = 9 },
    { .nLeft = 174, .nY = 245, .nRight = 228, .wStringId = 4067, .nTextWidth = 8 },
    { .nLeft = 174, .nY = 269, .nRight = 228, .wStringId = 4068, .nTextWidth = 5 },
    { .nLeft = 10, .nY = 307, .nRight = 73, .wStringId = 4069, .nTextWidth = 7 },
    { .nLeft = 174, .nY = 307, .nRight = 228, .wStringId = 4070, .nTextWidth = 5 },
    { .nLeft = 190, .nY = 346, .nRight = 268, .wStringId = 4071, .nTextWidth = 5 },
    { .nLeft = 190, .nY = 370, .nRight = 268, .wStringId = 4072, .nTextWidth = 5 },
    { .nLeft = 190, .nY = 395, .nRight = 268, .wStringId = 4073, .nTextWidth = 10 },
    { .nLeft = 190, .nY = 419, .nRight = 268, .wStringId = 4074, .nTextWidth = 7 },
};
static_assert(sizeof(gCharPanelLabels) == 270, "gCharPanelLabels: the item's 270 bytes in the .i64");

//  D2Client 0x6FB78120
//  The character panel's stat rows, 18 records of 16 bytes: +0 nLeft (x of the label), +4 nY, +8 nRight (x of the value), +12 nStatId (a stat
//  id read by CHAR_CharPanelDraw). Rows 3, 4, 6 and 11 are strength, dexterity, vitality and energy down the left column; rows 14-17 are the four
//  resistances at the bottom right.
inline int32_t gCharPanelStatRows[72] =
{
    13,
    59,
    53,
    12,
    67,
    59,
    180,
    13,
    195,
    59,
    308,
    30,
    77,
    99,
    112,
    0,
    77,
    161,
    112,
    2,
    273,
    209,
    307,
    31,
    77,
    247,
    112,
    3,
    232,
    246,
    267,
    11,
    273,
    246,
    308,
    10,
    232,
    270,
    267,
    7,
    273,
    270,
    308,
    6,
    77,
    308,
    112,
    1,
    232,
    308,
    267,
    9,
    273,
    308,
    308,
    8,
    273,
    348,
    307,
    39,
    273,
    372,
    307,
    43,
    273,
    396,
    307,
    41,
    273,
    420,
    307,
    45,
};
static_assert(sizeof(gCharPanelStatRows) == 288, "gCharPanelStatRows: the item's 288 bytes in the .i64");

//  D2Client 0x6FB78240
//  The four stat-point buttons, 4 records of 14 bytes: +0 int nX (117), +4 int nY (105, 167, 253, 315), +8 int the pressed flag
//  (CHAR_StatsOnLButtonDown stores 1, CHAR_StatsOnLButtonUp clears it), +12 short nStatId (0, 2, 3, 1 -- strength, dexterity, vitality, energy).
//  CHAR_StatsOnLButtonUp sends the +12 word with MSG_SendCmdWord, command 0x3A (add a stat point): the count (at most 32 per click) in the high
//  byte, the stat id in the low. Its walk over the table ends at 0x6FB7827C, four bytes past the table's end.
inline uint8_t gCharPanelStatButtons[56] =
{
    0x75,
    0x00,
    0x00,
    0x00,
    0x69,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x75,
    0x00,
    0x00,
    0x00,
    0xA7,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x02,
    0x00,
    0x75,
    0x00,
    0x00,
    0x00,
    0xFD,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x03,
    0x00,
    0x75,
    0x00,
    0x00,
    0x00,
    0x3B,
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
};
static_assert(sizeof(gCharPanelStatButtons) == 56, "gCharPanelStatButtons: the item's 56 bytes in the .i64");

//  D2Client 0x6FBB1A44
//  Set to 1 while the gold-entry dialog is open (CHAR_MarkGoldDialogOpen, called from UI_OpenGoldEntryDialog) and cleared by
//  CHAR_MarkGoldDialogClosed; read by nothing. It duplicates `gpGoldDialog != NULL`, which UI_IsGoldDialogOpen tests.
inline BOOL gbGoldDialogOpenUnread;
static_assert(sizeof(gbGoldDialogOpenUnread) == 4, "gbGoldDialogOpenUnread: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1A4C
//  The stats button's pressed state: armed by CHAR_StatsOnLButtonDown, cleared by CHAR_StatsOnLButtonUp, read by CHAR_CharPanelDraw. It is not
//  gbNewStatsButtonDown, the mini panel's new-stats notification button.
inline BOOL gbStatsButtonDown;
static_assert(sizeof(gbStatsButtonDown) == 4, "gbStatsButtonDown: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1A50
//  Never written (it lies past .data's raw size, so it is 0 at run time). CHAR_CharPanelDraw reads it as the colour of the chance-to-hit
//  highlight box and of the defence tooltip.
inline uint32_t gnCharPanelHiliteColor;
static_assert(sizeof(gnCharPanelHiliteColor) == 4, "gnCharPanelHiliteColor: the item's 4 bytes in the .i64");

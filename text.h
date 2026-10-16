//  text.h -- the globals of text.cpp: the popup's message handler table, the hover-message placement tables, the quest-menu string table, the
//  Cairn Stone rune positions, the speech subtitle tables, the text state block, the popup state and the quest scroll images.
//  Each is a C++17 `inline` variable, so every file that includes this header shares one object. Address range 0x6FB8D220..0x6FBB9F78. The
//  state block and the popup state lie past .data's raw data (it ends at 0x6FB9B000) and are zero at load.
//  gnPopupX is declared with what the original's static initialiser (CRT_StaticInit16, 0x6FB1A7A0) computes at load from gnScreenWidth,
//  which is why this header includes CoreArchive.h.
//  Include it after D2Client.h, which declares every function named here.
#pragma once

#include "CoreArchive.h"

//  D2Client 0x6FB8D228
//  The popup's message table: 7 records of {dwType, nMessage, pfnHandler}, registered on the client window by
//  TEXT_PopupSetMessageHandlersActive and also by TEXT_ShowQuestScrollingText and TEXT_ShowSpeechPopup. WM_RBUTTONUP -> INV_ConsumeMessage;
//  WM_RBUTTONDOWN and WM_LBUTTONDOWN -> TEXT_PopupOnMouseDown; VK_ESCAPE and VK_SPACE -> TEXT_PopupOnDismissKey; WM_CHAR -> TEXT_PopupOnChar;
//  WM_SYSKEYDOWN -> TEXT_PopupOnSysKeyDown.
inline D2WinCommandRegisterStrc gPopupMessageHandlers[7] =
{
    { D2_WINPROC_MESSAGE,   WM_RBUTTONUP,               &INV_ConsumeMessage },
    { D2_WINPROC_MESSAGE,   WM_RBUTTONDOWN,             &TEXT_PopupOnMouseDown },
    { D2_WINPROC_MESSAGE,   WM_LBUTTONDOWN,             &TEXT_PopupOnMouseDown },
    { D2_WINPROC_KEYDOWN,   VK_ESCAPE,                  &TEXT_PopupOnDismissKey },
    { D2_WINPROC_KEYDOWN,   VK_SPACE,                   &TEXT_PopupOnDismissKey },
    { D2_WINPROC_MESSAGE,   WM_CHAR,                    &TEXT_PopupOnChar },
    { D2_WINPROC_MESSAGE,   WM_SYSKEYDOWN,              &TEXT_PopupOnSysKeyDown },
};
static_assert(sizeof(gPopupMessageHandlers) == 84, "gPopupMessageHandlers: the item's 84 bytes in the .i64");

//  D2Client 0x6FB8D220
//  Top edge of the speech popup, set to 12 by TEXT_ShowSpeechPopup and TEXT_ShowQuestScrollingText. The click region runs from here to
//  `gnPopupY + 182`, clamped to `gnScreenHeight - 53`. Read by TEXT_DrawPopup (six times).
inline int32_t gnPopupY = 12;
static_assert(sizeof(gnPopupY) == 4, "gnPopupY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB8D27C
//  int[8] of +/-1, { 1, 1, -1, 1, 1, -1, -1, -1 }: the signs TEXT_PlaceHoverMsg tries when a speech bubble does not fit where it wants to be.
//  The placement search is three nested loops: the outer steps nOffsetX from 50 to 350 in 50s (`cmp eax, 190h / jl`, so 400 is never taken),
//  the middle steps ebx from 35 to 245 in 35s, and the inner runs over these signs, testing a 400x280 rectangle at
//      left = rcSrc.left + nOffsetX * sign      top = rcSrc.top + ebx * sign
//  until one fits. gnHoverMsgOffsetSignCount is the bound.
//  NOT ESTABLISHED: the code has one indexed read of this table (`mov ecx, [gnHoverMsgOffsetSigns + ebp*4]`) and uses its value for both the X
//  and the Y term, so whether the eight entries are eight directions with a shared sign per axis, or four interleaved (X,Y) pairs, cannot be
//  told from that read.
inline int32_t gnHoverMsgOffsetSigns[8] = { 1, 1, -1, 1, 1, -1, -1, -1 };
static_assert(sizeof(gnHoverMsgOffsetSigns) == 32, "gnHoverMsgOffsetSigns: the item's 32 bytes in the .i64");

//  D2Client 0x6FB8D29C
//  8: the count of ints in gnHoverMsgOffsetSigns, the bound of the inner placement loop in TEXT_PlaceHoverMsg, loaded into a register before the
//  loop and compared each pass. A constant in .data rather than an immediate; both of its references are in that one function.
inline int32_t gnHoverMsgOffsetSignCount = 8;
static_assert(sizeof(gnHoverMsgOffsetSignCount) == 4, "gnHoverMsgOffsetSignCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FB8D2A0
//  Maps a quest message's string id to the tbl index of its NPC talk-menu line: 527 records of { uint16 nQuestStringId; uint16 nTblIdx }.
//  gnQuestMenuStringCount follows it in memory (0x6FB8D2A0 + 527 * 4 = 0x6FB8DADC). Read only by TEXT_QuestGetQuestMenuString, which splits the
//  scan at record 100. The records are not all sorted by key: [60] = 126 and [61] = 124, [250] = 435 and [251] = 431, [314] = 593 and
//  [315] = 588.
inline D2QuestMenuStringStrc gQuestMenuStrings[527] =
{
    { .nQuestStringId = 64, .nTblIdx = 3714 },   // [0]
    { .nQuestStringId = 65, .nTblIdx = 3714 },   // [1]
    { .nQuestStringId = 66, .nTblIdx = 3714 },   // [2]
    { .nQuestStringId = 67, .nTblIdx = 3714 },   // [3]
    { .nQuestStringId = 68, .nTblIdx = 3714 },   // [4]
    { .nQuestStringId = 69, .nTblIdx = 3714 },   // [5]
    { .nQuestStringId = 70, .nTblIdx = 3714 },   // [6]
    { .nQuestStringId = 71, .nTblIdx = 3714 },   // [7]
    { .nQuestStringId = 72, .nTblIdx = 3714 },   // [8]
    { .nQuestStringId = 73, .nTblIdx = 3714 },   // [9]
    { .nQuestStringId = 74, .nTblIdx = 3714 },   // [10]
    { .nQuestStringId = 75, .nTblIdx = 3714 },   // [11]
    { .nQuestStringId = 76, .nTblIdx = 3714 },   // [12]
    { .nQuestStringId = 77, .nTblIdx = 3714 },   // [13]
    { .nQuestStringId = 78, .nTblIdx = 3714 },   // [14]
    { .nQuestStringId = 79, .nTblIdx = 3714 },   // [15]
    { .nQuestStringId = 80, .nTblIdx = 3714 },   // [16]
    { .nQuestStringId = 81, .nTblIdx = 3715 },   // [17]
    { .nQuestStringId = 82, .nTblIdx = 3715 },   // [18]
    { .nQuestStringId = 83, .nTblIdx = 3715 },   // [19]
    { .nQuestStringId = 84, .nTblIdx = 3715 },   // [20]
    { .nQuestStringId = 85, .nTblIdx = 3715 },   // [21]
    { .nQuestStringId = 86, .nTblIdx = 3715 },   // [22]
    { .nQuestStringId = 87, .nTblIdx = 3715 },   // [23]
    { .nQuestStringId = 88, .nTblIdx = 3715 },   // [24]
    { .nQuestStringId = 89, .nTblIdx = 3715 },   // [25]
    { .nQuestStringId = 90, .nTblIdx = 3715 },   // [26]
    { .nQuestStringId = 91, .nTblIdx = 3715 },   // [27]
    { .nQuestStringId = 92, .nTblIdx = 3715 },   // [28]
    { .nQuestStringId = 93, .nTblIdx = 3715 },   // [29]
    { .nQuestStringId = 94, .nTblIdx = 3715 },   // [30]
    { .nQuestStringId = 95, .nTblIdx = 3715 },   // [31]
    { .nQuestStringId = 96, .nTblIdx = 3715 },   // [32]
    { .nQuestStringId = 97, .nTblIdx = 3717 },   // [33]
    { .nQuestStringId = 98, .nTblIdx = 3717 },   // [34]
    { .nQuestStringId = 99, .nTblIdx = 3717 },   // [35]
    { .nQuestStringId = 100, .nTblIdx = 3717 },   // [36]
    { .nQuestStringId = 101, .nTblIdx = 3717 },   // [37]
    { .nQuestStringId = 102, .nTblIdx = 3717 },   // [38]
    { .nQuestStringId = 103, .nTblIdx = 3717 },   // [39]
    { .nQuestStringId = 104, .nTblIdx = 3717 },   // [40]
    { .nQuestStringId = 105, .nTblIdx = 3717 },   // [41]
    { .nQuestStringId = 106, .nTblIdx = 3717 },   // [42]
    { .nQuestStringId = 107, .nTblIdx = 3717 },   // [43]
    { .nQuestStringId = 108, .nTblIdx = 3717 },   // [44]
    { .nQuestStringId = 109, .nTblIdx = 3717 },   // [45]
    { .nQuestStringId = 110, .nTblIdx = 3717 },   // [46]
    { .nQuestStringId = 111, .nTblIdx = 3717 },   // [47]
    { .nQuestStringId = 112, .nTblIdx = 3717 },   // [48]
    { .nQuestStringId = 113, .nTblIdx = 3717 },   // [49]
    { .nQuestStringId = 114, .nTblIdx = 3717 },   // [50]
    { .nQuestStringId = 115, .nTblIdx = 3717 },   // [51]
    { .nQuestStringId = 116, .nTblIdx = 3717 },   // [52]
    { .nQuestStringId = 117, .nTblIdx = 3717 },   // [53]
    { .nQuestStringId = 118, .nTblIdx = 3717 },   // [54]
    { .nQuestStringId = 119, .nTblIdx = 3717 },   // [55]
    { .nQuestStringId = 120, .nTblIdx = 3717 },   // [56]
    { .nQuestStringId = 121, .nTblIdx = 3717 },   // [57]
    { .nQuestStringId = 122, .nTblIdx = 3717 },   // [58]
    { .nQuestStringId = 123, .nTblIdx = 3717 },   // [59]
    { .nQuestStringId = 126, .nTblIdx = 3717 },   // [60]
    { .nQuestStringId = 124, .nTblIdx = 3717 },   // [61]
    { .nQuestStringId = 125, .nTblIdx = 3717 },   // [62]
    { .nQuestStringId = 127, .nTblIdx = 3718 },   // [63]
    { .nQuestStringId = 128, .nTblIdx = 3718 },   // [64]
    { .nQuestStringId = 129, .nTblIdx = 3718 },   // [65]
    { .nQuestStringId = 130, .nTblIdx = 3718 },   // [66]
    { .nQuestStringId = 131, .nTblIdx = 3718 },   // [67]
    { .nQuestStringId = 132, .nTblIdx = 3718 },   // [68]
    { .nQuestStringId = 133, .nTblIdx = 3718 },   // [69]
    { .nQuestStringId = 134, .nTblIdx = 3718 },   // [70]
    { .nQuestStringId = 135, .nTblIdx = 3718 },   // [71]
    { .nQuestStringId = 136, .nTblIdx = 3718 },   // [72]
    { .nQuestStringId = 137, .nTblIdx = 3718 },   // [73]
    { .nQuestStringId = 138, .nTblIdx = 3718 },   // [74]
    { .nQuestStringId = 139, .nTblIdx = 3718 },   // [75]
    { .nQuestStringId = 140, .nTblIdx = 3718 },   // [76]
    { .nQuestStringId = 141, .nTblIdx = 3718 },   // [77]
    { .nQuestStringId = 142, .nTblIdx = 3718 },   // [78]
    { .nQuestStringId = 143, .nTblIdx = 3718 },   // [79]
    { .nQuestStringId = 144, .nTblIdx = 3718 },   // [80]
    { .nQuestStringId = 145, .nTblIdx = 3718 },   // [81]
    { .nQuestStringId = 146, .nTblIdx = 3716 },   // [82]
    { .nQuestStringId = 147, .nTblIdx = 3716 },   // [83]
    { .nQuestStringId = 148, .nTblIdx = 3716 },   // [84]
    { .nQuestStringId = 149, .nTblIdx = 3716 },   // [85]
    { .nQuestStringId = 150, .nTblIdx = 3716 },   // [86]
    { .nQuestStringId = 151, .nTblIdx = 3716 },   // [87]
    { .nQuestStringId = 152, .nTblIdx = 3716 },   // [88]
    { .nQuestStringId = 153, .nTblIdx = 3716 },   // [89]
    { .nQuestStringId = 154, .nTblIdx = 3716 },   // [90]
    { .nQuestStringId = 155, .nTblIdx = 3716 },   // [91]
    { .nQuestStringId = 156, .nTblIdx = 3716 },   // [92]
    { .nQuestStringId = 157, .nTblIdx = 3716 },   // [93]
    { .nQuestStringId = 158, .nTblIdx = 3716 },   // [94]
    { .nQuestStringId = 159, .nTblIdx = 3716 },   // [95]
    { .nQuestStringId = 160, .nTblIdx = 3716 },   // [96]
    { .nQuestStringId = 161, .nTblIdx = 3716 },   // [97]
    { .nQuestStringId = 162, .nTblIdx = 3716 },   // [98]
    { .nQuestStringId = 163, .nTblIdx = 3716 },   // [99]
    { .nQuestStringId = 164, .nTblIdx = 3716 },   // [100]
    { .nQuestStringId = 165, .nTblIdx = 3716 },   // [101]
    { .nQuestStringId = 166, .nTblIdx = 3719 },   // [102]
    { .nQuestStringId = 167, .nTblIdx = 3719 },   // [103]
    { .nQuestStringId = 168, .nTblIdx = 3719 },   // [104]
    { .nQuestStringId = 169, .nTblIdx = 3719 },   // [105]
    { .nQuestStringId = 170, .nTblIdx = 3719 },   // [106]
    { .nQuestStringId = 171, .nTblIdx = 3719 },   // [107]
    { .nQuestStringId = 172, .nTblIdx = 3719 },   // [108]
    { .nQuestStringId = 173, .nTblIdx = 3719 },   // [109]
    { .nQuestStringId = 174, .nTblIdx = 3719 },   // [110]
    { .nQuestStringId = 175, .nTblIdx = 3719 },   // [111]
    { .nQuestStringId = 176, .nTblIdx = 3719 },   // [112]
    { .nQuestStringId = 177, .nTblIdx = 3719 },   // [113]
    { .nQuestStringId = 178, .nTblIdx = 3719 },   // [114]
    { .nQuestStringId = 179, .nTblIdx = 3719 },   // [115]
    { .nQuestStringId = 180, .nTblIdx = 3719 },   // [116]
    { .nQuestStringId = 181, .nTblIdx = 3719 },   // [117]
    { .nQuestStringId = 182, .nTblIdx = 3719 },   // [118]
    { .nQuestStringId = 183, .nTblIdx = 3719 },   // [119]
    { .nQuestStringId = 184, .nTblIdx = 3719 },   // [120]
    { .nQuestStringId = 304, .nTblIdx = 923 },   // [121]
    { .nQuestStringId = 305, .nTblIdx = 923 },   // [122]
    { .nQuestStringId = 306, .nTblIdx = 923 },   // [123]
    { .nQuestStringId = 307, .nTblIdx = 923 },   // [124]
    { .nQuestStringId = 308, .nTblIdx = 923 },   // [125]
    { .nQuestStringId = 309, .nTblIdx = 923 },   // [126]
    { .nQuestStringId = 310, .nTblIdx = 923 },   // [127]
    { .nQuestStringId = 311, .nTblIdx = 923 },   // [128]
    { .nQuestStringId = 312, .nTblIdx = 923 },   // [129]
    { .nQuestStringId = 313, .nTblIdx = 923 },   // [130]
    { .nQuestStringId = 314, .nTblIdx = 923 },   // [131]
    { .nQuestStringId = 315, .nTblIdx = 923 },   // [132]
    { .nQuestStringId = 316, .nTblIdx = 923 },   // [133]
    { .nQuestStringId = 317, .nTblIdx = 923 },   // [134]
    { .nQuestStringId = 318, .nTblIdx = 923 },   // [135]
    { .nQuestStringId = 319, .nTblIdx = 923 },   // [136]
    { .nQuestStringId = 320, .nTblIdx = 923 },   // [137]
    { .nQuestStringId = 321, .nTblIdx = 923 },   // [138]
    { .nQuestStringId = 322, .nTblIdx = 923 },   // [139]
    { .nQuestStringId = 323, .nTblIdx = 923 },   // [140]
    { .nQuestStringId = 324, .nTblIdx = 923 },   // [141]
    { .nQuestStringId = 325, .nTblIdx = 923 },   // [142]
    { .nQuestStringId = 326, .nTblIdx = 923 },   // [143]
    { .nQuestStringId = 327, .nTblIdx = 923 },   // [144]
    { .nQuestStringId = 328, .nTblIdx = 923 },   // [145]
    { .nQuestStringId = 329, .nTblIdx = 923 },   // [146]
    { .nQuestStringId = 330, .nTblIdx = 923 },   // [147]
    { .nQuestStringId = 331, .nTblIdx = 923 },   // [148]
    { .nQuestStringId = 332, .nTblIdx = 923 },   // [149]
    { .nQuestStringId = 333, .nTblIdx = 923 },   // [150]
    { .nQuestStringId = 334, .nTblIdx = 923 },   // [151]
    { .nQuestStringId = 335, .nTblIdx = 924 },   // [152]
    { .nQuestStringId = 336, .nTblIdx = 924 },   // [153]
    { .nQuestStringId = 337, .nTblIdx = 924 },   // [154]
    { .nQuestStringId = 338, .nTblIdx = 924 },   // [155]
    { .nQuestStringId = 339, .nTblIdx = 924 },   // [156]
    { .nQuestStringId = 340, .nTblIdx = 925 },   // [157]
    { .nQuestStringId = 341, .nTblIdx = 925 },   // [158]
    { .nQuestStringId = 342, .nTblIdx = 925 },   // [159]
    { .nQuestStringId = 343, .nTblIdx = 925 },   // [160]
    { .nQuestStringId = 344, .nTblIdx = 925 },   // [161]
    { .nQuestStringId = 345, .nTblIdx = 925 },   // [162]
    { .nQuestStringId = 346, .nTblIdx = 925 },   // [163]
    { .nQuestStringId = 347, .nTblIdx = 925 },   // [164]
    { .nQuestStringId = 348, .nTblIdx = 925 },   // [165]
    { .nQuestStringId = 349, .nTblIdx = 925 },   // [166]
    { .nQuestStringId = 350, .nTblIdx = 925 },   // [167]
    { .nQuestStringId = 351, .nTblIdx = 925 },   // [168]
    { .nQuestStringId = 352, .nTblIdx = 925 },   // [169]
    { .nQuestStringId = 353, .nTblIdx = 925 },   // [170]
    { .nQuestStringId = 354, .nTblIdx = 925 },   // [171]
    { .nQuestStringId = 355, .nTblIdx = 925 },   // [172]
    { .nQuestStringId = 356, .nTblIdx = 925 },   // [173]
    { .nQuestStringId = 357, .nTblIdx = 925 },   // [174]
    { .nQuestStringId = 358, .nTblIdx = 925 },   // [175]
    { .nQuestStringId = 359, .nTblIdx = 925 },   // [176]
    { .nQuestStringId = 360, .nTblIdx = 925 },   // [177]
    { .nQuestStringId = 361, .nTblIdx = 925 },   // [178]
    { .nQuestStringId = 362, .nTblIdx = 925 },   // [179]
    { .nQuestStringId = 363, .nTblIdx = 925 },   // [180]
    { .nQuestStringId = 364, .nTblIdx = 925 },   // [181]
    { .nQuestStringId = 365, .nTblIdx = 925 },   // [182]
    { .nQuestStringId = 366, .nTblIdx = 925 },   // [183]
    { .nQuestStringId = 367, .nTblIdx = 925 },   // [184]
    { .nQuestStringId = 368, .nTblIdx = 925 },   // [185]
    { .nQuestStringId = 369, .nTblIdx = 925 },   // [186]
    { .nQuestStringId = 370, .nTblIdx = 925 },   // [187]
    { .nQuestStringId = 371, .nTblIdx = 925 },   // [188]
    { .nQuestStringId = 372, .nTblIdx = 925 },   // [189]
    { .nQuestStringId = 373, .nTblIdx = 926 },   // [190]
    { .nQuestStringId = 374, .nTblIdx = 926 },   // [191]
    { .nQuestStringId = 375, .nTblIdx = 926 },   // [192]
    { .nQuestStringId = 376, .nTblIdx = 926 },   // [193]
    { .nQuestStringId = 377, .nTblIdx = 926 },   // [194]
    { .nQuestStringId = 378, .nTblIdx = 926 },   // [195]
    { .nQuestStringId = 379, .nTblIdx = 926 },   // [196]
    { .nQuestStringId = 380, .nTblIdx = 926 },   // [197]
    { .nQuestStringId = 381, .nTblIdx = 926 },   // [198]
    { .nQuestStringId = 382, .nTblIdx = 926 },   // [199]
    { .nQuestStringId = 383, .nTblIdx = 926 },   // [200]
    { .nQuestStringId = 384, .nTblIdx = 926 },   // [201]
    { .nQuestStringId = 385, .nTblIdx = 926 },   // [202]
    { .nQuestStringId = 386, .nTblIdx = 926 },   // [203]
    { .nQuestStringId = 387, .nTblIdx = 926 },   // [204]
    { .nQuestStringId = 388, .nTblIdx = 926 },   // [205]
    { .nQuestStringId = 389, .nTblIdx = 926 },   // [206]
    { .nQuestStringId = 390, .nTblIdx = 926 },   // [207]
    { .nQuestStringId = 391, .nTblIdx = 926 },   // [208]
    { .nQuestStringId = 392, .nTblIdx = 926 },   // [209]
    { .nQuestStringId = 393, .nTblIdx = 926 },   // [210]
    { .nQuestStringId = 394, .nTblIdx = 926 },   // [211]
    { .nQuestStringId = 395, .nTblIdx = 926 },   // [212]
    { .nQuestStringId = 397, .nTblIdx = 926 },   // [213]
    { .nQuestStringId = 398, .nTblIdx = 926 },   // [214]
    { .nQuestStringId = 399, .nTblIdx = 926 },   // [215]
    { .nQuestStringId = 400, .nTblIdx = 926 },   // [216]
    { .nQuestStringId = 401, .nTblIdx = 926 },   // [217]
    { .nQuestStringId = 402, .nTblIdx = 926 },   // [218]
    { .nQuestStringId = 403, .nTblIdx = 926 },   // [219]
    { .nQuestStringId = 404, .nTblIdx = 926 },   // [220]
    { .nQuestStringId = 405, .nTblIdx = 926 },   // [221]
    { .nQuestStringId = 406, .nTblIdx = 926 },   // [222]
    { .nQuestStringId = 407, .nTblIdx = 926 },   // [223]
    { .nQuestStringId = 408, .nTblIdx = 927 },   // [224]
    { .nQuestStringId = 409, .nTblIdx = 927 },   // [225]
    { .nQuestStringId = 410, .nTblIdx = 927 },   // [226]
    { .nQuestStringId = 411, .nTblIdx = 927 },   // [227]
    { .nQuestStringId = 412, .nTblIdx = 927 },   // [228]
    { .nQuestStringId = 413, .nTblIdx = 927 },   // [229]
    { .nQuestStringId = 414, .nTblIdx = 927 },   // [230]
    { .nQuestStringId = 415, .nTblIdx = 927 },   // [231]
    { .nQuestStringId = 416, .nTblIdx = 927 },   // [232]
    { .nQuestStringId = 417, .nTblIdx = 927 },   // [233]
    { .nQuestStringId = 418, .nTblIdx = 927 },   // [234]
    { .nQuestStringId = 419, .nTblIdx = 927 },   // [235]
    { .nQuestStringId = 420, .nTblIdx = 927 },   // [236]
    { .nQuestStringId = 421, .nTblIdx = 927 },   // [237]
    { .nQuestStringId = 422, .nTblIdx = 927 },   // [238]
    { .nQuestStringId = 423, .nTblIdx = 927 },   // [239]
    { .nQuestStringId = 424, .nTblIdx = 927 },   // [240]
    { .nQuestStringId = 425, .nTblIdx = 927 },   // [241]
    { .nQuestStringId = 426, .nTblIdx = 927 },   // [242]
    { .nQuestStringId = 427, .nTblIdx = 927 },   // [243]
    { .nQuestStringId = 428, .nTblIdx = 927 },   // [244]
    { .nQuestStringId = 429, .nTblIdx = 927 },   // [245]
    { .nQuestStringId = 430, .nTblIdx = 928 },   // [246]
    { .nQuestStringId = 432, .nTblIdx = 928 },   // [247]
    { .nQuestStringId = 433, .nTblIdx = 928 },   // [248]
    { .nQuestStringId = 434, .nTblIdx = 928 },   // [249]
    { .nQuestStringId = 435, .nTblIdx = 928 },   // [250]
    { .nQuestStringId = 431, .nTblIdx = 928 },   // [251]
    { .nQuestStringId = 436, .nTblIdx = 928 },   // [252]
    { .nQuestStringId = 437, .nTblIdx = 928 },   // [253]
    { .nQuestStringId = 438, .nTblIdx = 928 },   // [254]
    { .nQuestStringId = 439, .nTblIdx = 928 },   // [255]
    { .nQuestStringId = 440, .nTblIdx = 928 },   // [256]
    { .nQuestStringId = 441, .nTblIdx = 928 },   // [257]
    { .nQuestStringId = 442, .nTblIdx = 928 },   // [258]
    { .nQuestStringId = 443, .nTblIdx = 928 },   // [259]
    { .nQuestStringId = 444, .nTblIdx = 928 },   // [260]
    { .nQuestStringId = 445, .nTblIdx = 928 },   // [261]
    { .nQuestStringId = 446, .nTblIdx = 928 },   // [262]
    { .nQuestStringId = 447, .nTblIdx = 928 },   // [263]
    { .nQuestStringId = 448, .nTblIdx = 928 },   // [264]
    { .nQuestStringId = 449, .nTblIdx = 928 },   // [265]
    { .nQuestStringId = 450, .nTblIdx = 928 },   // [266]
    { .nQuestStringId = 451, .nTblIdx = 928 },   // [267]
    { .nQuestStringId = 452, .nTblIdx = 928 },   // [268]
    { .nQuestStringId = 549, .nTblIdx = 930 },   // [269]
    { .nQuestStringId = 550, .nTblIdx = 930 },   // [270]
    { .nQuestStringId = 551, .nTblIdx = 930 },   // [271]
    { .nQuestStringId = 552, .nTblIdx = 930 },   // [272]
    { .nQuestStringId = 553, .nTblIdx = 930 },   // [273]
    { .nQuestStringId = 554, .nTblIdx = 930 },   // [274]
    { .nQuestStringId = 555, .nTblIdx = 930 },   // [275]
    { .nQuestStringId = 556, .nTblIdx = 930 },   // [276]
    { .nQuestStringId = 557, .nTblIdx = 930 },   // [277]
    { .nQuestStringId = 558, .nTblIdx = 930 },   // [278]
    { .nQuestStringId = 559, .nTblIdx = 930 },   // [279]
    { .nQuestStringId = 560, .nTblIdx = 930 },   // [280]
    { .nQuestStringId = 561, .nTblIdx = 930 },   // [281]
    { .nQuestStringId = 562, .nTblIdx = 930 },   // [282]
    { .nQuestStringId = 563, .nTblIdx = 930 },   // [283]
    { .nQuestStringId = 564, .nTblIdx = 930 },   // [284]
    { .nQuestStringId = 565, .nTblIdx = 930 },   // [285]
    { .nQuestStringId = 566, .nTblIdx = 930 },   // [286]
    { .nQuestStringId = 567, .nTblIdx = 930 },   // [287]
    { .nQuestStringId = 568, .nTblIdx = 930 },   // [288]
    { .nQuestStringId = 569, .nTblIdx = 930 },   // [289]
    { .nQuestStringId = 570, .nTblIdx = 930 },   // [290]
    { .nQuestStringId = 543, .nTblIdx = 931 },   // [291]
    { .nQuestStringId = 545, .nTblIdx = 931 },   // [292]
    { .nQuestStringId = 544, .nTblIdx = 931 },   // [293]
    { .nQuestStringId = 546, .nTblIdx = 931 },   // [294]
    { .nQuestStringId = 547, .nTblIdx = 931 },   // [295]
    { .nQuestStringId = 548, .nTblIdx = 931 },   // [296]
    { .nQuestStringId = 571, .nTblIdx = 932 },   // [297]
    { .nQuestStringId = 572, .nTblIdx = 932 },   // [298]
    { .nQuestStringId = 573, .nTblIdx = 932 },   // [299]
    { .nQuestStringId = 574, .nTblIdx = 932 },   // [300]
    { .nQuestStringId = 575, .nTblIdx = 932 },   // [301]
    { .nQuestStringId = 576, .nTblIdx = 932 },   // [302]
    { .nQuestStringId = 577, .nTblIdx = 932 },   // [303]
    { .nQuestStringId = 578, .nTblIdx = 932 },   // [304]
    { .nQuestStringId = 579, .nTblIdx = 932 },   // [305]
    { .nQuestStringId = 580, .nTblIdx = 932 },   // [306]
    { .nQuestStringId = 581, .nTblIdx = 932 },   // [307]
    { .nQuestStringId = 582, .nTblIdx = 932 },   // [308]
    { .nQuestStringId = 583, .nTblIdx = 932 },   // [309]
    { .nQuestStringId = 584, .nTblIdx = 932 },   // [310]
    { .nQuestStringId = 585, .nTblIdx = 932 },   // [311]
    { .nQuestStringId = 586, .nTblIdx = 932 },   // [312]
    { .nQuestStringId = 587, .nTblIdx = 932 },   // [313]
    { .nQuestStringId = 593, .nTblIdx = 932 },   // [314]
    { .nQuestStringId = 588, .nTblIdx = 932 },   // [315]
    { .nQuestStringId = 589, .nTblIdx = 932 },   // [316]
    { .nQuestStringId = 590, .nTblIdx = 932 },   // [317]
    { .nQuestStringId = 591, .nTblIdx = 932 },   // [318]
    { .nQuestStringId = 592, .nTblIdx = 932 },   // [319]
    { .nQuestStringId = 527, .nTblIdx = 933 },   // [320]
    { .nQuestStringId = 528, .nTblIdx = 933 },   // [321]
    { .nQuestStringId = 529, .nTblIdx = 933 },   // [322]
    { .nQuestStringId = 530, .nTblIdx = 933 },   // [323]
    { .nQuestStringId = 531, .nTblIdx = 933 },   // [324]
    { .nQuestStringId = 532, .nTblIdx = 933 },   // [325]
    { .nQuestStringId = 533, .nTblIdx = 933 },   // [326]
    { .nQuestStringId = 534, .nTblIdx = 933 },   // [327]
    { .nQuestStringId = 535, .nTblIdx = 933 },   // [328]
    { .nQuestStringId = 536, .nTblIdx = 933 },   // [329]
    { .nQuestStringId = 537, .nTblIdx = 933 },   // [330]
    { .nQuestStringId = 538, .nTblIdx = 933 },   // [331]
    { .nQuestStringId = 539, .nTblIdx = 933 },   // [332]
    { .nQuestStringId = 540, .nTblIdx = 933 },   // [333]
    { .nQuestStringId = 541, .nTblIdx = 933 },   // [334]
    { .nQuestStringId = 542, .nTblIdx = 933 },   // [335]
    { .nQuestStringId = 594, .nTblIdx = 934 },   // [336]
    { .nQuestStringId = 595, .nTblIdx = 934 },   // [337]
    { .nQuestStringId = 596, .nTblIdx = 934 },   // [338]
    { .nQuestStringId = 597, .nTblIdx = 934 },   // [339]
    { .nQuestStringId = 598, .nTblIdx = 934 },   // [340]
    { .nQuestStringId = 599, .nTblIdx = 934 },   // [341]
    { .nQuestStringId = 600, .nTblIdx = 934 },   // [342]
    { .nQuestStringId = 601, .nTblIdx = 934 },   // [343]
    { .nQuestStringId = 602, .nTblIdx = 934 },   // [344]
    { .nQuestStringId = 603, .nTblIdx = 934 },   // [345]
    { .nQuestStringId = 604, .nTblIdx = 934 },   // [346]
    { .nQuestStringId = 605, .nTblIdx = 934 },   // [347]
    { .nQuestStringId = 606, .nTblIdx = 934 },   // [348]
    { .nQuestStringId = 607, .nTblIdx = 934 },   // [349]
    { .nQuestStringId = 608, .nTblIdx = 934 },   // [350]
    { .nQuestStringId = 609, .nTblIdx = 934 },   // [351]
    { .nQuestStringId = 610, .nTblIdx = 934 },   // [352]
    { .nQuestStringId = 611, .nTblIdx = 934 },   // [353]
    { .nQuestStringId = 612, .nTblIdx = 934 },   // [354]
    { .nQuestStringId = 613, .nTblIdx = 934 },   // [355]
    { .nQuestStringId = 614, .nTblIdx = 934 },   // [356]
    { .nQuestStringId = 615, .nTblIdx = 934 },   // [357]
    { .nQuestStringId = 616, .nTblIdx = 934 },   // [358]
    { .nQuestStringId = 617, .nTblIdx = 934 },   // [359]
    { .nQuestStringId = 618, .nTblIdx = 934 },   // [360]
    { .nQuestStringId = 619, .nTblIdx = 934 },   // [361]
    { .nQuestStringId = 620, .nTblIdx = 934 },   // [362]
    { .nQuestStringId = 621, .nTblIdx = 934 },   // [363]
    { .nQuestStringId = 622, .nTblIdx = 934 },   // [364]
    { .nQuestStringId = 623, .nTblIdx = 934 },   // [365]
    { .nQuestStringId = 624, .nTblIdx = 934 },   // [366]
    { .nQuestStringId = 625, .nTblIdx = 934 },   // [367]
    { .nQuestStringId = 626, .nTblIdx = 934 },   // [368]
    { .nQuestStringId = 627, .nTblIdx = 934 },   // [369]
    { .nQuestStringId = 628, .nTblIdx = 935 },   // [370]
    { .nQuestStringId = 629, .nTblIdx = 935 },   // [371]
    { .nQuestStringId = 630, .nTblIdx = 935 },   // [372]
    { .nQuestStringId = 631, .nTblIdx = 935 },   // [373]
    { .nQuestStringId = 632, .nTblIdx = 935 },   // [374]
    { .nQuestStringId = 633, .nTblIdx = 935 },   // [375]
    { .nQuestStringId = 634, .nTblIdx = 935 },   // [376]
    { .nQuestStringId = 635, .nTblIdx = 935 },   // [377]
    { .nQuestStringId = 637, .nTblIdx = 935 },   // [378]
    { .nQuestStringId = 638, .nTblIdx = 935 },   // [379]
    { .nQuestStringId = 639, .nTblIdx = 935 },   // [380]
    { .nQuestStringId = 640, .nTblIdx = 935 },   // [381]
    { .nQuestStringId = 641, .nTblIdx = 935 },   // [382]
    { .nQuestStringId = 642, .nTblIdx = 935 },   // [383]
    { .nQuestStringId = 643, .nTblIdx = 935 },   // [384]
    { .nQuestStringId = 644, .nTblIdx = 935 },   // [385]
    { .nQuestStringId = 645, .nTblIdx = 935 },   // [386]
    { .nQuestStringId = 646, .nTblIdx = 935 },   // [387]
    { .nQuestStringId = 647, .nTblIdx = 935 },   // [388]
    { .nQuestStringId = 648, .nTblIdx = 935 },   // [389]
    { .nQuestStringId = 649, .nTblIdx = 935 },   // [390]
    { .nQuestStringId = 650, .nTblIdx = 935 },   // [391]
    { .nQuestStringId = 651, .nTblIdx = 935 },   // [392]
    { .nQuestStringId = 652, .nTblIdx = 935 },   // [393]
    { .nQuestStringId = 653, .nTblIdx = 935 },   // [394]
    { .nQuestStringId = 654, .nTblIdx = 935 },   // [395]
    { .nQuestStringId = 655, .nTblIdx = 935 },   // [396]
    { .nQuestStringId = 656, .nTblIdx = 935 },   // [397]
    { .nQuestStringId = 657, .nTblIdx = 935 },   // [398]
    { .nQuestStringId = 658, .nTblIdx = 935 },   // [399]
    { .nQuestStringId = 659, .nTblIdx = 935 },   // [400]
    { .nQuestStringId = 660, .nTblIdx = 935 },   // [401]
    { .nQuestStringId = 661, .nTblIdx = 935 },   // [402]
    { .nQuestStringId = 662, .nTblIdx = 935 },   // [403]
    { .nQuestStringId = 663, .nTblIdx = 935 },   // [404]
    { .nQuestStringId = 670, .nTblIdx = 937 },   // [405]
    { .nQuestStringId = 671, .nTblIdx = 937 },   // [406]
    { .nQuestStringId = 672, .nTblIdx = 937 },   // [407]
    { .nQuestStringId = 673, .nTblIdx = 937 },   // [408]
    { .nQuestStringId = 674, .nTblIdx = 937 },   // [409]
    { .nQuestStringId = 675, .nTblIdx = 937 },   // [410]
    { .nQuestStringId = 676, .nTblIdx = 937 },   // [411]
    { .nQuestStringId = 677, .nTblIdx = 937 },   // [412]
    { .nQuestStringId = 681, .nTblIdx = 938 },   // [413]
    { .nQuestStringId = 682, .nTblIdx = 938 },   // [414]
    { .nQuestStringId = 683, .nTblIdx = 938 },   // [415]
    { .nQuestStringId = 684, .nTblIdx = 938 },   // [416]
    { .nQuestStringId = 685, .nTblIdx = 938 },   // [417]
    { .nQuestStringId = 678, .nTblIdx = 939 },   // [418]
    { .nQuestStringId = 679, .nTblIdx = 939 },   // [419]
    { .nQuestStringId = 680, .nTblIdx = 939 },   // [420]
    { .nQuestStringId = 20000, .nTblIdx = 938 },   // [421]
    { .nQuestStringId = 20001, .nTblIdx = 938 },   // [422]
    { .nQuestStringId = 20077, .nTblIdx = 22618 },   // [423]
    { .nQuestStringId = 20078, .nTblIdx = 22618 },   // [424]
    { .nQuestStringId = 20079, .nTblIdx = 22618 },   // [425]
    { .nQuestStringId = 20080, .nTblIdx = 22618 },   // [426]
    { .nQuestStringId = 20081, .nTblIdx = 22618 },   // [427]
    { .nQuestStringId = 20082, .nTblIdx = 22618 },   // [428]
    { .nQuestStringId = 20083, .nTblIdx = 22618 },   // [429]
    { .nQuestStringId = 20084, .nTblIdx = 22618 },   // [430]
    { .nQuestStringId = 20085, .nTblIdx = 22618 },   // [431]
    { .nQuestStringId = 20086, .nTblIdx = 22618 },   // [432]
    { .nQuestStringId = 20087, .nTblIdx = 22618 },   // [433]
    { .nQuestStringId = 20088, .nTblIdx = 22618 },   // [434]
    { .nQuestStringId = 20089, .nTblIdx = 22618 },   // [435]
    { .nQuestStringId = 20090, .nTblIdx = 22618 },   // [436]
    { .nQuestStringId = 20091, .nTblIdx = 22618 },   // [437]
    { .nQuestStringId = 20092, .nTblIdx = 22618 },   // [438]
    { .nQuestStringId = 20093, .nTblIdx = 22618 },   // [439]
    { .nQuestStringId = 20094, .nTblIdx = 22618 },   // [440]
    { .nQuestStringId = 20095, .nTblIdx = 22618 },   // [441]
    { .nQuestStringId = 20096, .nTblIdx = 22622 },   // [442]
    { .nQuestStringId = 20097, .nTblIdx = 22622 },   // [443]
    { .nQuestStringId = 20098, .nTblIdx = 22622 },   // [444]
    { .nQuestStringId = 20099, .nTblIdx = 22622 },   // [445]
    { .nQuestStringId = 20100, .nTblIdx = 22622 },   // [446]
    { .nQuestStringId = 20101, .nTblIdx = 22622 },   // [447]
    { .nQuestStringId = 20102, .nTblIdx = 22622 },   // [448]
    { .nQuestStringId = 20103, .nTblIdx = 22622 },   // [449]
    { .nQuestStringId = 20104, .nTblIdx = 22622 },   // [450]
    { .nQuestStringId = 20105, .nTblIdx = 22622 },   // [451]
    { .nQuestStringId = 20106, .nTblIdx = 22622 },   // [452]
    { .nQuestStringId = 20107, .nTblIdx = 22622 },   // [453]
    { .nQuestStringId = 20108, .nTblIdx = 22622 },   // [454]
    { .nQuestStringId = 20109, .nTblIdx = 22622 },   // [455]
    { .nQuestStringId = 20110, .nTblIdx = 22622 },   // [456]
    { .nQuestStringId = 20111, .nTblIdx = 22622 },   // [457]
    { .nQuestStringId = 20112, .nTblIdx = 22622 },   // [458]
    { .nQuestStringId = 20113, .nTblIdx = 22622 },   // [459]
    { .nQuestStringId = 20114, .nTblIdx = 22622 },   // [460]
    { .nQuestStringId = 20115, .nTblIdx = 22622 },   // [461]
    { .nQuestStringId = 20116, .nTblIdx = 22627 },   // [462]
    { .nQuestStringId = 20117, .nTblIdx = 22627 },   // [463]
    { .nQuestStringId = 20118, .nTblIdx = 22627 },   // [464]
    { .nQuestStringId = 20119, .nTblIdx = 22627 },   // [465]
    { .nQuestStringId = 20120, .nTblIdx = 22627 },   // [466]
    { .nQuestStringId = 20121, .nTblIdx = 22627 },   // [467]
    { .nQuestStringId = 20122, .nTblIdx = 22627 },   // [468]
    { .nQuestStringId = 20123, .nTblIdx = 22627 },   // [469]
    { .nQuestStringId = 20124, .nTblIdx = 22627 },   // [470]
    { .nQuestStringId = 20125, .nTblIdx = 22627 },   // [471]
    { .nQuestStringId = 20126, .nTblIdx = 22627 },   // [472]
    { .nQuestStringId = 20127, .nTblIdx = 22627 },   // [473]
    { .nQuestStringId = 20128, .nTblIdx = 22627 },   // [474]
    { .nQuestStringId = 20129, .nTblIdx = 22627 },   // [475]
    { .nQuestStringId = 20130, .nTblIdx = 22627 },   // [476]
    { .nQuestStringId = 20131, .nTblIdx = 22627 },   // [477]
    { .nQuestStringId = 20136, .nTblIdx = 22627 },   // [478]
    { .nQuestStringId = 20132, .nTblIdx = 22627 },   // [479]
    { .nQuestStringId = 20133, .nTblIdx = 22627 },   // [480]
    { .nQuestStringId = 20134, .nTblIdx = 22627 },   // [481]
    { .nQuestStringId = 20135, .nTblIdx = 22627 },   // [482]
    { .nQuestStringId = 20137, .nTblIdx = 22633 },   // [483]
    { .nQuestStringId = 20140, .nTblIdx = 22633 },   // [484]
    { .nQuestStringId = 20139, .nTblIdx = 22633 },   // [485]
    { .nQuestStringId = 20141, .nTblIdx = 22633 },   // [486]
    { .nQuestStringId = 20142, .nTblIdx = 22633 },   // [487]
    { .nQuestStringId = 20143, .nTblIdx = 22633 },   // [488]
    { .nQuestStringId = 20144, .nTblIdx = 22633 },   // [489]
    { .nQuestStringId = 20145, .nTblIdx = 22633 },   // [490]
    { .nQuestStringId = 20146, .nTblIdx = 22633 },   // [491]
    { .nQuestStringId = 20147, .nTblIdx = 22633 },   // [492]
    { .nQuestStringId = 20148, .nTblIdx = 22633 },   // [493]
    { .nQuestStringId = 20149, .nTblIdx = 22633 },   // [494]
    { .nQuestStringId = 20150, .nTblIdx = 22633 },   // [495]
    { .nQuestStringId = 20151, .nTblIdx = 22633 },   // [496]
    { .nQuestStringId = 20152, .nTblIdx = 22633 },   // [497]
    { .nQuestStringId = 20153, .nTblIdx = 22637 },   // [498]
    { .nQuestStringId = 20154, .nTblIdx = 22637 },   // [499]
    { .nQuestStringId = 20155, .nTblIdx = 22637 },   // [500]
    { .nQuestStringId = 20156, .nTblIdx = 22637 },   // [501]
    { .nQuestStringId = 20157, .nTblIdx = 22637 },   // [502]
    { .nQuestStringId = 20158, .nTblIdx = 22637 },   // [503]
    { .nQuestStringId = 20159, .nTblIdx = 22637 },   // [504]
    { .nQuestStringId = 20160, .nTblIdx = 22637 },   // [505]
    { .nQuestStringId = 20161, .nTblIdx = 22637 },   // [506]
    { .nQuestStringId = 20162, .nTblIdx = 22637 },   // [507]
    { .nQuestStringId = 20163, .nTblIdx = 22637 },   // [508]
    { .nQuestStringId = 20002, .nTblIdx = 22641 },   // [509]
    { .nQuestStringId = 20166, .nTblIdx = 22637 },   // [510]
    { .nQuestStringId = 20165, .nTblIdx = 22637 },   // [511]
    { .nQuestStringId = 20167, .nTblIdx = 22637 },   // [512]
    { .nQuestStringId = 20168, .nTblIdx = 22637 },   // [513]
    { .nQuestStringId = 20164, .nTblIdx = 22637 },   // [514]
    { .nQuestStringId = 20169, .nTblIdx = 22641 },   // [515]
    { .nQuestStringId = 20170, .nTblIdx = 22641 },   // [516]
    { .nQuestStringId = 20171, .nTblIdx = 22641 },   // [517]
    { .nQuestStringId = 20172, .nTblIdx = 22641 },   // [518]
    { .nQuestStringId = 20173, .nTblIdx = 22641 },   // [519]
    { .nQuestStringId = 20174, .nTblIdx = 22641 },   // [520]
    { .nQuestStringId = 20175, .nTblIdx = 22641 },   // [521]
    { .nQuestStringId = 20176, .nTblIdx = 22641 },   // [522]
    { .nQuestStringId = 20177, .nTblIdx = 22641 },   // [523]
    { .nQuestStringId = 20178, .nTblIdx = 22641 },   // [524]
    { .nQuestStringId = 20179, .nTblIdx = 22641 },   // [525]
    { .nQuestStringId = 20180, .nTblIdx = 22641 },   // [526]
};
static_assert(sizeof(gQuestMenuStrings) == 2108, "gQuestMenuStrings: the item's 2108 bytes in the .i64");

//  D2Client 0x6FB8DADC
//  Number of records in gQuestMenuStrings: 527. It sits in the dword immediately after the array, so a naive read makes it look like a 528th
//  entry.
inline int32_t gnQuestMenuStringCount = 527;
static_assert(sizeof(gnQuestMenuStringCount) == 4, "gnQuestMenuStringCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FB8DAE0
//  The game's own name, from its assert `w < arrsize(sgRunePos)` (D2Client/UI/text.cpp, line 5703).
//  { int nX; int nY; }[5]: the five screen positions of the Cairn Stones pentagon, (242,104) (255,222) (148,310) (47,222) (75,104). Indexed by
//  gwCairnStoneOrder[i], so the server decides which rune appears at which corner and the client only looks the position up. Both coordinates
//  go through the letterbox offsets before drawing (gnBorderXDifference + X and Y - gnBorderYDifference).
//  sgRuneOverlayPos at +0x28 is the second table of five pairs, same index, placing an animated cel over each rune.
inline D2CoordStrc sgRunePos[5] =
{ { .nX = 242, .nY = 104 }, { .nX = 255, .nY = 222 }, { .nX = 148, .nY = 310 }, { .nX = 47, .nY = 222 }, { .nX = 75, .nY = 104 } };
static_assert(sizeof(sgRunePos) == 40, "sgRunePos: the item's 40 bytes in the .i64");

//  D2Client 0x6FB8DB08
//  { int nX; int nY; }[5], +0x28 from sgRunePos and +0x28 before gnQuestScrollFrameThresholds: (303,161) (322,242) (254,290) (190,238) (211,162).
//  Indexed like sgRunePos. Where sgRunePos positions the stone cel, this positions a second cel taken from gIniScrollImage3 and drawn with mode 3,
//  whose frame is `frame >= 21 ? 0 : frame`: an animation over each rune. The name is descriptive; the game names only the first array in its
//  asserts.
inline D2CoordStrc sgRuneOverlayPos[5] =
{ { .nX = 303, .nY = 161 }, { .nX = 322, .nY = 242 }, { .nX = 254, .nY = 290 }, { .nX = 190, .nY = 238 }, { .nX = 211, .nY = 162 } };
static_assert(sizeof(sgRuneOverlayPos) == 40, "sgRuneOverlayPos: the item's 40 bytes in the .i64");

//  D2Client 0x6FB8DB30
//  Five frame thresholds, 0, 12, 24, 36, 48 (the bytes at 0x6FB8DB30): TEXT_DrawQuestScrollFrame compares its frame counter against them
//  (`cmp ebx, eax / jbe`) for nStone < 5. Each Cairn Stone starts twelve animation frames after the one before it, and a frame is 50 ms, so the
//  stones light 600 ms apart.
inline int32_t gnQuestScrollFrameThresholds[5] = { 0, 12, 24, 36, 48 };
static_assert(sizeof(gnQuestScrollFrameThresholds) == 20, "gnQuestScrollFrameThresholds: the item's 20 bytes in the .i64");

//  D2Client 0x6FB8DB48
inline D2ClientSpeechSubtitleStrc gSpeechSubtitlesAmazon[74] =
{
    { .nSoundId = 2953, .nStringId = 1069 },   // [0]
    { .nSoundId = 2954, .nStringId = 1070 },   // [1]
    { .nSoundId = 2937, .nStringId = 1072 },   // [2]
    { .nSoundId = 2938, .nStringId = 1071 },   // [3]
    { .nSoundId = 2939, .nStringId = 1073 },   // [4]
    { .nSoundId = 2940, .nStringId = 1074 },   // [5]
    { .nSoundId = 2941, .nStringId = 1075 },   // [6]
    { .nSoundId = 2942, .nStringId = 1076 },   // [7]
    { .nSoundId = 2943, .nStringId = 1078 },   // [8]
    { .nSoundId = 2944, .nStringId = 1077 },   // [9]
    { .nSoundId = 2945, .nStringId = 1080 },   // [10]
    { .nSoundId = 2946, .nStringId = 1079 },   // [11]
    { .nSoundId = 2947, .nStringId = 1082 },   // [12]
    { .nSoundId = 2948, .nStringId = 1081 },   // [13]
    { .nSoundId = 2949, .nStringId = 1083 },   // [14]
    { .nSoundId = 2950, .nStringId = 1084 },   // [15]
    { .nSoundId = 2957, .nStringId = 1085 },   // [16]
    { .nSoundId = 2958, .nStringId = 1086 },   // [17]
    { .nSoundId = 2936, .nStringId = 1087 },   // [18]
    { .nSoundId = 2934, .nStringId = 1089 },   // [19]
    { .nSoundId = 2935, .nStringId = 1088 },   // [20]
    { .nSoundId = 2959, .nStringId = 1091 },   // [21]
    { .nSoundId = 2960, .nStringId = 1090 },   // [22]
    { .nSoundId = 2955, .nStringId = 1094 },   // [23]
    { .nSoundId = 2956, .nStringId = 1093 },   // [24]
    { .nSoundId = 2975, .nStringId = 1095 },   // [25]
    { .nSoundId = 2969, .nStringId = 1100 },   // [26]
    { .nSoundId = 2966, .nStringId = 1105 },   // [27]
    { .nSoundId = 2971, .nStringId = 1110 },   // [28]
    { .nSoundId = 2972, .nStringId = 1115 },   // [29]
    { .nSoundId = 2970, .nStringId = 1120 },   // [30]
    { .nSoundId = 2968, .nStringId = 1130 },   // [31]
    { .nSoundId = 2963, .nStringId = 1135 },   // [32]
    { .nSoundId = 2962, .nStringId = 1140 },   // [33]
    { .nSoundId = 2973, .nStringId = 1145 },   // [34]
    { .nSoundId = 2967, .nStringId = 1150 },   // [35]
    { .nSoundId = 2974, .nStringId = 1155 },   // [36]
    { .nSoundId = 2976, .nStringId = 1160 },   // [37]
    { .nSoundId = 2964, .nStringId = 1165 },   // [38]
    { .nSoundId = 2965, .nStringId = 1170 },   // [39]
    { .nSoundId = 2961, .nStringId = 1175 },   // [40]
    { .nSoundId = 2985, .nStringId = 1180 },   // [41]
    { .nSoundId = 2978, .nStringId = 1185 },   // [42]
    { .nSoundId = 2977, .nStringId = 1190 },   // [43]
    { .nSoundId = 2984, .nStringId = 1195 },   // [44]
    { .nSoundId = 2980, .nStringId = 1200 },   // [45]
    { .nSoundId = 2983, .nStringId = 1205 },   // [46]
    { .nSoundId = 2986, .nStringId = 1210 },   // [47]
    { .nSoundId = 2979, .nStringId = 1215 },   // [48]
    { .nSoundId = 2987, .nStringId = 1220 },   // [49]
    { .nSoundId = 2988, .nStringId = 1225 },   // [50]
    { .nSoundId = 2982, .nStringId = 1230 },   // [51]
    { .nSoundId = 3001, .nStringId = 1236 },   // [52]
    { .nSoundId = 2995, .nStringId = 1241 },   // [53]
    { .nSoundId = 2996, .nStringId = 1246 },   // [54]
    { .nSoundId = 2998, .nStringId = 1251 },   // [55]
    { .nSoundId = 2991, .nStringId = 1256 },   // [56]
    { .nSoundId = 2993, .nStringId = 1261 },   // [57]
    { .nSoundId = 3000, .nStringId = 1266 },   // [58]
    { .nSoundId = 2997, .nStringId = 1271 },   // [59]
    { .nSoundId = 2992, .nStringId = 1276 },   // [60]
    { .nSoundId = 2999, .nStringId = 1281 },   // [61]
    { .nSoundId = 2994, .nStringId = 1286 },   // [62]
    { .nSoundId = 3002, .nStringId = 1291 },   // [63]
    { .nSoundId = 2951, .nStringId = 10916 },   // [64]
    { .nSoundId = 2952, .nStringId = 21894 },   // [65]
    { .nSoundId = 3004, .nStringId = 21986 },   // [66]
    { .nSoundId = 3005, .nStringId = 22007 },   // [67]
    { .nSoundId = 3006, .nStringId = 22021 },   // [68]
    { .nSoundId = 3007, .nStringId = 22028 },   // [69]
    { .nSoundId = 3008, .nStringId = 21993 },   // [70]
    { .nSoundId = 3009, .nStringId = 22000 },   // [71]
    { .nSoundId = 3010, .nStringId = 22014 },   // [72]
    { .nSoundId = 3011, .nStringId = 22035 },   // [73]
};
static_assert(sizeof(gSpeechSubtitlesAmazon) == 296, "gSpeechSubtitlesAmazon: the item's 296 bytes in the .i64");

//  D2Client 0x6FB8DC70
inline D2ClientSpeechSubtitleStrc gSpeechSubtitlesSorceress[74] =
{
    { .nSoundId = 3265, .nStringId = 1069 },   // [0]
    { .nSoundId = 3266, .nStringId = 1070 },   // [1]
    { .nSoundId = 3249, .nStringId = 1072 },   // [2]
    { .nSoundId = 3250, .nStringId = 1071 },   // [3]
    { .nSoundId = 3251, .nStringId = 1073 },   // [4]
    { .nSoundId = 3252, .nStringId = 1074 },   // [5]
    { .nSoundId = 3253, .nStringId = 1075 },   // [6]
    { .nSoundId = 3254, .nStringId = 1076 },   // [7]
    { .nSoundId = 3255, .nStringId = 1078 },   // [8]
    { .nSoundId = 3256, .nStringId = 1077 },   // [9]
    { .nSoundId = 3257, .nStringId = 1080 },   // [10]
    { .nSoundId = 3258, .nStringId = 1079 },   // [11]
    { .nSoundId = 3259, .nStringId = 1082 },   // [12]
    { .nSoundId = 3260, .nStringId = 1081 },   // [13]
    { .nSoundId = 3261, .nStringId = 1083 },   // [14]
    { .nSoundId = 3262, .nStringId = 1084 },   // [15]
    { .nSoundId = 3269, .nStringId = 1085 },   // [16]
    { .nSoundId = 3270, .nStringId = 1086 },   // [17]
    { .nSoundId = 3248, .nStringId = 1087 },   // [18]
    { .nSoundId = 3246, .nStringId = 1089 },   // [19]
    { .nSoundId = 3247, .nStringId = 1088 },   // [20]
    { .nSoundId = 3271, .nStringId = 1091 },   // [21]
    { .nSoundId = 3272, .nStringId = 1090 },   // [22]
    { .nSoundId = 3267, .nStringId = 1094 },   // [23]
    { .nSoundId = 3268, .nStringId = 1093 },   // [24]
    { .nSoundId = 3287, .nStringId = 1098 },   // [25]
    { .nSoundId = 3281, .nStringId = 1103 },   // [26]
    { .nSoundId = 3278, .nStringId = 1108 },   // [27]
    { .nSoundId = 3283, .nStringId = 1113 },   // [28]
    { .nSoundId = 3284, .nStringId = 1118 },   // [29]
    { .nSoundId = 3282, .nStringId = 1123 },   // [30]
    { .nSoundId = 3280, .nStringId = 1133 },   // [31]
    { .nSoundId = 3275, .nStringId = 1138 },   // [32]
    { .nSoundId = 3274, .nStringId = 1143 },   // [33]
    { .nSoundId = 3285, .nStringId = 1148 },   // [34]
    { .nSoundId = 3279, .nStringId = 1153 },   // [35]
    { .nSoundId = 3286, .nStringId = 1158 },   // [36]
    { .nSoundId = 3288, .nStringId = 1163 },   // [37]
    { .nSoundId = 3276, .nStringId = 1168 },   // [38]
    { .nSoundId = 3277, .nStringId = 1173 },   // [39]
    { .nSoundId = 3273, .nStringId = 1178 },   // [40]
    { .nSoundId = 3297, .nStringId = 1183 },   // [41]
    { .nSoundId = 3290, .nStringId = 1188 },   // [42]
    { .nSoundId = 3289, .nStringId = 1193 },   // [43]
    { .nSoundId = 3296, .nStringId = 1198 },   // [44]
    { .nSoundId = 3292, .nStringId = 1203 },   // [45]
    { .nSoundId = 3295, .nStringId = 1208 },   // [46]
    { .nSoundId = 3298, .nStringId = 1213 },   // [47]
    { .nSoundId = 3291, .nStringId = 1218 },   // [48]
    { .nSoundId = 3299, .nStringId = 1223 },   // [49]
    { .nSoundId = 3300, .nStringId = 1228 },   // [50]
    { .nSoundId = 3294, .nStringId = 1233 },   // [51]
    { .nSoundId = 3313, .nStringId = 1239 },   // [52]
    { .nSoundId = 3307, .nStringId = 1244 },   // [53]
    { .nSoundId = 3308, .nStringId = 1249 },   // [54]
    { .nSoundId = 3310, .nStringId = 1254 },   // [55]
    { .nSoundId = 3303, .nStringId = 1259 },   // [56]
    { .nSoundId = 3305, .nStringId = 1264 },   // [57]
    { .nSoundId = 3312, .nStringId = 1269 },   // [58]
    { .nSoundId = 3309, .nStringId = 1274 },   // [59]
    { .nSoundId = 3304, .nStringId = 1279 },   // [60]
    { .nSoundId = 3311, .nStringId = 1284 },   // [61]
    { .nSoundId = 3306, .nStringId = 1289 },   // [62]
    { .nSoundId = 3314, .nStringId = 1294 },   // [63]
    { .nSoundId = 3263, .nStringId = 10916 },   // [64]
    { .nSoundId = 3264, .nStringId = 21894 },   // [65]
    { .nSoundId = 3316, .nStringId = 21982 },   // [66]
    { .nSoundId = 3317, .nStringId = 22003 },   // [67]
    { .nSoundId = 3318, .nStringId = 22017 },   // [68]
    { .nSoundId = 3319, .nStringId = 22024 },   // [69]
    { .nSoundId = 3320, .nStringId = 21989 },   // [70]
    { .nSoundId = 3321, .nStringId = 21996 },   // [71]
    { .nSoundId = 3322, .nStringId = 22010 },   // [72]
    { .nSoundId = 3323, .nStringId = 22031 },   // [73]
};
static_assert(sizeof(gSpeechSubtitlesSorceress) == 296, "gSpeechSubtitlesSorceress: the item's 296 bytes in the .i64");

//  D2Client 0x6FB8DD98
inline D2ClientSpeechSubtitleStrc gSpeechSubtitlesNecromancer[74] =
{
    { .nSoundId = 3109, .nStringId = 1069 },   // [0]
    { .nSoundId = 3110, .nStringId = 1070 },   // [1]
    { .nSoundId = 3093, .nStringId = 1072 },   // [2]
    { .nSoundId = 3094, .nStringId = 1071 },   // [3]
    { .nSoundId = 3095, .nStringId = 1073 },   // [4]
    { .nSoundId = 3096, .nStringId = 1074 },   // [5]
    { .nSoundId = 3097, .nStringId = 1075 },   // [6]
    { .nSoundId = 3098, .nStringId = 1076 },   // [7]
    { .nSoundId = 3099, .nStringId = 1078 },   // [8]
    { .nSoundId = 3100, .nStringId = 1077 },   // [9]
    { .nSoundId = 3101, .nStringId = 1080 },   // [10]
    { .nSoundId = 3102, .nStringId = 1079 },   // [11]
    { .nSoundId = 3103, .nStringId = 1082 },   // [12]
    { .nSoundId = 3104, .nStringId = 1081 },   // [13]
    { .nSoundId = 3105, .nStringId = 1083 },   // [14]
    { .nSoundId = 3106, .nStringId = 1084 },   // [15]
    { .nSoundId = 3113, .nStringId = 1085 },   // [16]
    { .nSoundId = 3114, .nStringId = 1086 },   // [17]
    { .nSoundId = 3092, .nStringId = 1087 },   // [18]
    { .nSoundId = 3090, .nStringId = 1089 },   // [19]
    { .nSoundId = 3091, .nStringId = 1088 },   // [20]
    { .nSoundId = 3115, .nStringId = 1091 },   // [21]
    { .nSoundId = 3116, .nStringId = 1090 },   // [22]
    { .nSoundId = 3111, .nStringId = 1094 },   // [23]
    { .nSoundId = 3112, .nStringId = 1093 },   // [24]
    { .nSoundId = 3131, .nStringId = 1099 },   // [25]
    { .nSoundId = 3125, .nStringId = 1104 },   // [26]
    { .nSoundId = 3122, .nStringId = 1109 },   // [27]
    { .nSoundId = 3127, .nStringId = 1114 },   // [28]
    { .nSoundId = 3128, .nStringId = 1119 },   // [29]
    { .nSoundId = 3126, .nStringId = 1124 },   // [30]
    { .nSoundId = 3124, .nStringId = 1134 },   // [31]
    { .nSoundId = 3119, .nStringId = 1139 },   // [32]
    { .nSoundId = 3118, .nStringId = 1144 },   // [33]
    { .nSoundId = 3129, .nStringId = 1149 },   // [34]
    { .nSoundId = 3123, .nStringId = 1154 },   // [35]
    { .nSoundId = 3130, .nStringId = 1159 },   // [36]
    { .nSoundId = 3132, .nStringId = 1164 },   // [37]
    { .nSoundId = 3120, .nStringId = 1169 },   // [38]
    { .nSoundId = 3121, .nStringId = 1174 },   // [39]
    { .nSoundId = 3117, .nStringId = 1179 },   // [40]
    { .nSoundId = 3141, .nStringId = 1184 },   // [41]
    { .nSoundId = 3134, .nStringId = 1189 },   // [42]
    { .nSoundId = 3133, .nStringId = 1194 },   // [43]
    { .nSoundId = 3140, .nStringId = 1199 },   // [44]
    { .nSoundId = 3136, .nStringId = 1204 },   // [45]
    { .nSoundId = 3139, .nStringId = 1209 },   // [46]
    { .nSoundId = 3142, .nStringId = 1214 },   // [47]
    { .nSoundId = 3135, .nStringId = 1219 },   // [48]
    { .nSoundId = 3143, .nStringId = 1224 },   // [49]
    { .nSoundId = 3144, .nStringId = 1229 },   // [50]
    { .nSoundId = 3138, .nStringId = 1234 },   // [51]
    { .nSoundId = 3157, .nStringId = 1240 },   // [52]
    { .nSoundId = 3151, .nStringId = 1245 },   // [53]
    { .nSoundId = 3152, .nStringId = 1250 },   // [54]
    { .nSoundId = 3154, .nStringId = 1255 },   // [55]
    { .nSoundId = 3147, .nStringId = 1260 },   // [56]
    { .nSoundId = 3149, .nStringId = 1265 },   // [57]
    { .nSoundId = 3156, .nStringId = 1270 },   // [58]
    { .nSoundId = 3153, .nStringId = 1275 },   // [59]
    { .nSoundId = 3148, .nStringId = 1280 },   // [60]
    { .nSoundId = 3155, .nStringId = 1285 },   // [61]
    { .nSoundId = 3150, .nStringId = 1290 },   // [62]
    { .nSoundId = 3158, .nStringId = 1295 },   // [63]
    { .nSoundId = 3107, .nStringId = 10916 },   // [64]
    { .nSoundId = 3108, .nStringId = 21894 },   // [65]
    { .nSoundId = 3160, .nStringId = 21984 },   // [66]
    { .nSoundId = 3161, .nStringId = 22005 },   // [67]
    { .nSoundId = 3162, .nStringId = 22019 },   // [68]
    { .nSoundId = 3163, .nStringId = 22026 },   // [69]
    { .nSoundId = 3164, .nStringId = 21991 },   // [70]
    { .nSoundId = 3165, .nStringId = 21998 },   // [71]
    { .nSoundId = 3166, .nStringId = 22012 },   // [72]
    { .nSoundId = 3167, .nStringId = 22033 },   // [73]
};
static_assert(sizeof(gSpeechSubtitlesNecromancer) == 296, "gSpeechSubtitlesNecromancer: the item's 296 bytes in the .i64");

//  D2Client 0x6FB8DEC0
inline D2ClientSpeechSubtitleStrc gSpeechSubtitlesPaladin[74] =
{
    { .nSoundId = 3187, .nStringId = 1069 },   // [0]
    { .nSoundId = 3188, .nStringId = 1070 },   // [1]
    { .nSoundId = 3171, .nStringId = 1072 },   // [2]
    { .nSoundId = 3172, .nStringId = 1071 },   // [3]
    { .nSoundId = 3173, .nStringId = 1073 },   // [4]
    { .nSoundId = 3174, .nStringId = 1074 },   // [5]
    { .nSoundId = 3175, .nStringId = 1075 },   // [6]
    { .nSoundId = 3176, .nStringId = 1076 },   // [7]
    { .nSoundId = 3177, .nStringId = 1078 },   // [8]
    { .nSoundId = 3178, .nStringId = 1077 },   // [9]
    { .nSoundId = 3179, .nStringId = 1080 },   // [10]
    { .nSoundId = 3180, .nStringId = 1079 },   // [11]
    { .nSoundId = 3181, .nStringId = 1082 },   // [12]
    { .nSoundId = 3182, .nStringId = 1081 },   // [13]
    { .nSoundId = 3183, .nStringId = 1083 },   // [14]
    { .nSoundId = 3184, .nStringId = 1084 },   // [15]
    { .nSoundId = 3191, .nStringId = 1085 },   // [16]
    { .nSoundId = 3192, .nStringId = 1086 },   // [17]
    { .nSoundId = 3170, .nStringId = 1087 },   // [18]
    { .nSoundId = 3168, .nStringId = 1089 },   // [19]
    { .nSoundId = 3169, .nStringId = 1088 },   // [20]
    { .nSoundId = 3193, .nStringId = 1091 },   // [21]
    { .nSoundId = 3194, .nStringId = 1090 },   // [22]
    { .nSoundId = 3189, .nStringId = 1094 },   // [23]
    { .nSoundId = 3190, .nStringId = 1093 },   // [24]
    { .nSoundId = 3209, .nStringId = 1097 },   // [25]
    { .nSoundId = 3203, .nStringId = 1102 },   // [26]
    { .nSoundId = 3200, .nStringId = 1107 },   // [27]
    { .nSoundId = 3205, .nStringId = 1112 },   // [28]
    { .nSoundId = 3206, .nStringId = 1117 },   // [29]
    { .nSoundId = 3204, .nStringId = 1122 },   // [30]
    { .nSoundId = 3202, .nStringId = 1132 },   // [31]
    { .nSoundId = 3197, .nStringId = 1137 },   // [32]
    { .nSoundId = 3196, .nStringId = 1142 },   // [33]
    { .nSoundId = 3207, .nStringId = 1147 },   // [34]
    { .nSoundId = 3201, .nStringId = 1152 },   // [35]
    { .nSoundId = 3208, .nStringId = 1157 },   // [36]
    { .nSoundId = 3210, .nStringId = 1162 },   // [37]
    { .nSoundId = 3198, .nStringId = 1167 },   // [38]
    { .nSoundId = 3199, .nStringId = 1172 },   // [39]
    { .nSoundId = 3195, .nStringId = 1177 },   // [40]
    { .nSoundId = 3219, .nStringId = 1182 },   // [41]
    { .nSoundId = 3212, .nStringId = 1187 },   // [42]
    { .nSoundId = 3211, .nStringId = 1192 },   // [43]
    { .nSoundId = 3218, .nStringId = 1197 },   // [44]
    { .nSoundId = 3214, .nStringId = 1202 },   // [45]
    { .nSoundId = 3217, .nStringId = 1207 },   // [46]
    { .nSoundId = 3220, .nStringId = 1212 },   // [47]
    { .nSoundId = 3213, .nStringId = 1217 },   // [48]
    { .nSoundId = 3221, .nStringId = 1222 },   // [49]
    { .nSoundId = 3222, .nStringId = 1227 },   // [50]
    { .nSoundId = 3216, .nStringId = 1232 },   // [51]
    { .nSoundId = 3235, .nStringId = 1238 },   // [52]
    { .nSoundId = 3229, .nStringId = 1243 },   // [53]
    { .nSoundId = 3230, .nStringId = 1248 },   // [54]
    { .nSoundId = 3232, .nStringId = 1253 },   // [55]
    { .nSoundId = 3225, .nStringId = 1258 },   // [56]
    { .nSoundId = 3227, .nStringId = 1263 },   // [57]
    { .nSoundId = 3234, .nStringId = 1268 },   // [58]
    { .nSoundId = 3231, .nStringId = 1273 },   // [59]
    { .nSoundId = 3226, .nStringId = 1278 },   // [60]
    { .nSoundId = 3233, .nStringId = 1283 },   // [61]
    { .nSoundId = 3228, .nStringId = 1288 },   // [62]
    { .nSoundId = 3236, .nStringId = 1293 },   // [63]
    { .nSoundId = 3185, .nStringId = 10916 },   // [64]
    { .nSoundId = 3186, .nStringId = 21894 },   // [65]
    { .nSoundId = 3238, .nStringId = 21985 },   // [66]
    { .nSoundId = 3239, .nStringId = 22006 },   // [67]
    { .nSoundId = 3240, .nStringId = 22020 },   // [68]
    { .nSoundId = 3241, .nStringId = 22027 },   // [69]
    { .nSoundId = 3242, .nStringId = 21992 },   // [70]
    { .nSoundId = 3243, .nStringId = 21999 },   // [71]
    { .nSoundId = 3244, .nStringId = 22013 },   // [72]
    { .nSoundId = 3245, .nStringId = 22034 },   // [73]
};
static_assert(sizeof(gSpeechSubtitlesPaladin) == 296, "gSpeechSubtitlesPaladin: the item's 296 bytes in the .i64");

//  D2Client 0x6FB8DFE8
inline D2ClientSpeechSubtitleStrc gSpeechSubtitlesBarbarian[74] =
{
    { .nSoundId = 3031, .nStringId = 1069 },   // [0]
    { .nSoundId = 3032, .nStringId = 1070 },   // [1]
    { .nSoundId = 3015, .nStringId = 1072 },   // [2]
    { .nSoundId = 3016, .nStringId = 1071 },   // [3]
    { .nSoundId = 3017, .nStringId = 1073 },   // [4]
    { .nSoundId = 3018, .nStringId = 1074 },   // [5]
    { .nSoundId = 3019, .nStringId = 1075 },   // [6]
    { .nSoundId = 3020, .nStringId = 1076 },   // [7]
    { .nSoundId = 3021, .nStringId = 1078 },   // [8]
    { .nSoundId = 3022, .nStringId = 1077 },   // [9]
    { .nSoundId = 3023, .nStringId = 1080 },   // [10]
    { .nSoundId = 3024, .nStringId = 1079 },   // [11]
    { .nSoundId = 3025, .nStringId = 1082 },   // [12]
    { .nSoundId = 3026, .nStringId = 1081 },   // [13]
    { .nSoundId = 3027, .nStringId = 1083 },   // [14]
    { .nSoundId = 3028, .nStringId = 1084 },   // [15]
    { .nSoundId = 3035, .nStringId = 1085 },   // [16]
    { .nSoundId = 3036, .nStringId = 1086 },   // [17]
    { .nSoundId = 3014, .nStringId = 1087 },   // [18]
    { .nSoundId = 3012, .nStringId = 1089 },   // [19]
    { .nSoundId = 3013, .nStringId = 1088 },   // [20]
    { .nSoundId = 3037, .nStringId = 1091 },   // [21]
    { .nSoundId = 3038, .nStringId = 1090 },   // [22]
    { .nSoundId = 3033, .nStringId = 1094 },   // [23]
    { .nSoundId = 3034, .nStringId = 1093 },   // [24]
    { .nSoundId = 3053, .nStringId = 1096 },   // [25]
    { .nSoundId = 3047, .nStringId = 1101 },   // [26]
    { .nSoundId = 3044, .nStringId = 1106 },   // [27]
    { .nSoundId = 3049, .nStringId = 1111 },   // [28]
    { .nSoundId = 3050, .nStringId = 1116 },   // [29]
    { .nSoundId = 3048, .nStringId = 1121 },   // [30]
    { .nSoundId = 3046, .nStringId = 1131 },   // [31]
    { .nSoundId = 3041, .nStringId = 1136 },   // [32]
    { .nSoundId = 3040, .nStringId = 1141 },   // [33]
    { .nSoundId = 3051, .nStringId = 1146 },   // [34]
    { .nSoundId = 3045, .nStringId = 1151 },   // [35]
    { .nSoundId = 3052, .nStringId = 1156 },   // [36]
    { .nSoundId = 3054, .nStringId = 1161 },   // [37]
    { .nSoundId = 3042, .nStringId = 1166 },   // [38]
    { .nSoundId = 3043, .nStringId = 1171 },   // [39]
    { .nSoundId = 3039, .nStringId = 1176 },   // [40]
    { .nSoundId = 3063, .nStringId = 1181 },   // [41]
    { .nSoundId = 3056, .nStringId = 1186 },   // [42]
    { .nSoundId = 3055, .nStringId = 1191 },   // [43]
    { .nSoundId = 3062, .nStringId = 1196 },   // [44]
    { .nSoundId = 3058, .nStringId = 1201 },   // [45]
    { .nSoundId = 3061, .nStringId = 1206 },   // [46]
    { .nSoundId = 3064, .nStringId = 1211 },   // [47]
    { .nSoundId = 3057, .nStringId = 1216 },   // [48]
    { .nSoundId = 3065, .nStringId = 1221 },   // [49]
    { .nSoundId = 3066, .nStringId = 1226 },   // [50]
    { .nSoundId = 3060, .nStringId = 1231 },   // [51]
    { .nSoundId = 3079, .nStringId = 1237 },   // [52]
    { .nSoundId = 3073, .nStringId = 1242 },   // [53]
    { .nSoundId = 3074, .nStringId = 1247 },   // [54]
    { .nSoundId = 3076, .nStringId = 1252 },   // [55]
    { .nSoundId = 3069, .nStringId = 1257 },   // [56]
    { .nSoundId = 3071, .nStringId = 1262 },   // [57]
    { .nSoundId = 3078, .nStringId = 1267 },   // [58]
    { .nSoundId = 3075, .nStringId = 1272 },   // [59]
    { .nSoundId = 3070, .nStringId = 1277 },   // [60]
    { .nSoundId = 3077, .nStringId = 1282 },   // [61]
    { .nSoundId = 3072, .nStringId = 1287 },   // [62]
    { .nSoundId = 3080, .nStringId = 1292 },   // [63]
    { .nSoundId = 3029, .nStringId = 10916 },   // [64]
    { .nSoundId = 3030, .nStringId = 21894 },   // [65]
    { .nSoundId = 3082, .nStringId = 21983 },   // [66]
    { .nSoundId = 3083, .nStringId = 22004 },   // [67]
    { .nSoundId = 3084, .nStringId = 22018 },   // [68]
    { .nSoundId = 3085, .nStringId = 22025 },   // [69]
    { .nSoundId = 3086, .nStringId = 21990 },   // [70]
    { .nSoundId = 3087, .nStringId = 21997 },   // [71]
    { .nSoundId = 3088, .nStringId = 22011 },   // [72]
    { .nSoundId = 3089, .nStringId = 22032 },   // [73]
};
static_assert(sizeof(gSpeechSubtitlesBarbarian) == 296, "gSpeechSubtitlesBarbarian: the item's 296 bytes in the .i64");

//  D2Client 0x6FB8E110
inline D2ClientSpeechSubtitleStrc gSpeechSubtitlesDruid[74] =
{
    { .nSoundId = 3421, .nStringId = 1069 },   // [0]
    { .nSoundId = 3422, .nStringId = 1070 },   // [1]
    { .nSoundId = 3405, .nStringId = 1072 },   // [2]
    { .nSoundId = 3406, .nStringId = 1071 },   // [3]
    { .nSoundId = 3407, .nStringId = 1073 },   // [4]
    { .nSoundId = 3408, .nStringId = 1074 },   // [5]
    { .nSoundId = 3409, .nStringId = 1075 },   // [6]
    { .nSoundId = 3410, .nStringId = 1076 },   // [7]
    { .nSoundId = 3411, .nStringId = 1078 },   // [8]
    { .nSoundId = 3412, .nStringId = 1077 },   // [9]
    { .nSoundId = 3413, .nStringId = 1080 },   // [10]
    { .nSoundId = 3414, .nStringId = 1079 },   // [11]
    { .nSoundId = 3415, .nStringId = 1082 },   // [12]
    { .nSoundId = 3416, .nStringId = 1081 },   // [13]
    { .nSoundId = 3417, .nStringId = 1083 },   // [14]
    { .nSoundId = 3418, .nStringId = 1084 },   // [15]
    { .nSoundId = 3425, .nStringId = 1085 },   // [16]
    { .nSoundId = 3426, .nStringId = 1086 },   // [17]
    { .nSoundId = 3404, .nStringId = 1087 },   // [18]
    { .nSoundId = 3402, .nStringId = 1089 },   // [19]
    { .nSoundId = 3403, .nStringId = 1088 },   // [20]
    { .nSoundId = 3427, .nStringId = 1091 },   // [21]
    { .nSoundId = 3428, .nStringId = 1090 },   // [22]
    { .nSoundId = 3423, .nStringId = 1094 },   // [23]
    { .nSoundId = 3424, .nStringId = 1093 },   // [24]
    { .nSoundId = 3443, .nStringId = 21895 },   // [25]
    { .nSoundId = 3437, .nStringId = 21898 },   // [26]
    { .nSoundId = 3434, .nStringId = 21900 },   // [27]
    { .nSoundId = 3439, .nStringId = 21902 },   // [28]
    { .nSoundId = 3440, .nStringId = 21904 },   // [29]
    { .nSoundId = 3438, .nStringId = 21906 },   // [30]
    { .nSoundId = 3436, .nStringId = 21908 },   // [31]
    { .nSoundId = 3431, .nStringId = 21910 },   // [32]
    { .nSoundId = 3430, .nStringId = 21912 },   // [33]
    { .nSoundId = 3441, .nStringId = 21914 },   // [34]
    { .nSoundId = 3435, .nStringId = 21916 },   // [35]
    { .nSoundId = 3442, .nStringId = 21918 },   // [36]
    { .nSoundId = 3444, .nStringId = 21920 },   // [37]
    { .nSoundId = 3432, .nStringId = 21922 },   // [38]
    { .nSoundId = 3433, .nStringId = 21924 },   // [39]
    { .nSoundId = 3429, .nStringId = 21926 },   // [40]
    { .nSoundId = 3453, .nStringId = 21928 },   // [41]
    { .nSoundId = 3446, .nStringId = 21930 },   // [42]
    { .nSoundId = 3445, .nStringId = 21932 },   // [43]
    { .nSoundId = 3452, .nStringId = 21934 },   // [44]
    { .nSoundId = 3448, .nStringId = 21936 },   // [45]
    { .nSoundId = 3451, .nStringId = 21938 },   // [46]
    { .nSoundId = 3454, .nStringId = 21940 },   // [47]
    { .nSoundId = 3447, .nStringId = 21942 },   // [48]
    { .nSoundId = 3455, .nStringId = 21944 },   // [49]
    { .nSoundId = 3456, .nStringId = 21946 },   // [50]
    { .nSoundId = 3450, .nStringId = 21948 },   // [51]
    { .nSoundId = 3469, .nStringId = 21950 },   // [52]
    { .nSoundId = 3463, .nStringId = 21953 },   // [53]
    { .nSoundId = 3464, .nStringId = 21955 },   // [54]
    { .nSoundId = 3466, .nStringId = 21957 },   // [55]
    { .nSoundId = 3459, .nStringId = 21959 },   // [56]
    { .nSoundId = 3461, .nStringId = 21961 },   // [57]
    { .nSoundId = 3468, .nStringId = 21963 },   // [58]
    { .nSoundId = 3465, .nStringId = 21965 },   // [59]
    { .nSoundId = 3460, .nStringId = 21967 },   // [60]
    { .nSoundId = 3467, .nStringId = 21969 },   // [61]
    { .nSoundId = 3462, .nStringId = 21971 },   // [62]
    { .nSoundId = 3470, .nStringId = 21974 },   // [63]
    { .nSoundId = 3419, .nStringId = 10916 },   // [64]
    { .nSoundId = 3420, .nStringId = 21894 },   // [65]
    { .nSoundId = 3472, .nStringId = 21988 },   // [66]
    { .nSoundId = 3473, .nStringId = 22009 },   // [67]
    { .nSoundId = 3474, .nStringId = 22023 },   // [68]
    { .nSoundId = 3475, .nStringId = 22030 },   // [69]
    { .nSoundId = 3476, .nStringId = 21995 },   // [70]
    { .nSoundId = 3477, .nStringId = 22002 },   // [71]
    { .nSoundId = 3478, .nStringId = 22016 },   // [72]
    { .nSoundId = 3479, .nStringId = 22037 },   // [73]
};
static_assert(sizeof(gSpeechSubtitlesDruid) == 296, "gSpeechSubtitlesDruid: the item's 296 bytes in the .i64");

//  D2Client 0x6FB8E238
inline D2ClientSpeechSubtitleStrc gSpeechSubtitlesAssassin[74] =
{
    { .nSoundId = 3343, .nStringId = 1069 },   // [0]
    { .nSoundId = 3344, .nStringId = 1070 },   // [1]
    { .nSoundId = 3327, .nStringId = 1072 },   // [2]
    { .nSoundId = 3328, .nStringId = 1071 },   // [3]
    { .nSoundId = 3329, .nStringId = 1073 },   // [4]
    { .nSoundId = 3330, .nStringId = 1074 },   // [5]
    { .nSoundId = 3331, .nStringId = 1075 },   // [6]
    { .nSoundId = 3332, .nStringId = 1076 },   // [7]
    { .nSoundId = 3333, .nStringId = 1078 },   // [8]
    { .nSoundId = 3334, .nStringId = 1077 },   // [9]
    { .nSoundId = 3335, .nStringId = 1080 },   // [10]
    { .nSoundId = 3336, .nStringId = 1079 },   // [11]
    { .nSoundId = 3337, .nStringId = 1082 },   // [12]
    { .nSoundId = 3338, .nStringId = 1081 },   // [13]
    { .nSoundId = 3339, .nStringId = 1083 },   // [14]
    { .nSoundId = 3340, .nStringId = 1084 },   // [15]
    { .nSoundId = 3347, .nStringId = 1085 },   // [16]
    { .nSoundId = 3348, .nStringId = 1086 },   // [17]
    { .nSoundId = 3326, .nStringId = 1087 },   // [18]
    { .nSoundId = 3324, .nStringId = 1089 },   // [19]
    { .nSoundId = 3325, .nStringId = 1088 },   // [20]
    { .nSoundId = 3349, .nStringId = 1091 },   // [21]
    { .nSoundId = 3350, .nStringId = 1090 },   // [22]
    { .nSoundId = 3345, .nStringId = 1094 },   // [23]
    { .nSoundId = 3346, .nStringId = 1093 },   // [24]
    { .nSoundId = 3365, .nStringId = 21896 },   // [25]
    { .nSoundId = 3359, .nStringId = 21897 },   // [26]
    { .nSoundId = 3356, .nStringId = 21899 },   // [27]
    { .nSoundId = 3361, .nStringId = 21901 },   // [28]
    { .nSoundId = 3362, .nStringId = 21903 },   // [29]
    { .nSoundId = 3360, .nStringId = 21905 },   // [30]
    { .nSoundId = 3358, .nStringId = 21907 },   // [31]
    { .nSoundId = 3353, .nStringId = 21909 },   // [32]
    { .nSoundId = 3352, .nStringId = 21911 },   // [33]
    { .nSoundId = 3363, .nStringId = 21913 },   // [34]
    { .nSoundId = 3357, .nStringId = 21915 },   // [35]
    { .nSoundId = 3364, .nStringId = 21917 },   // [36]
    { .nSoundId = 3366, .nStringId = 21919 },   // [37]
    { .nSoundId = 3354, .nStringId = 21921 },   // [38]
    { .nSoundId = 3355, .nStringId = 21923 },   // [39]
    { .nSoundId = 3351, .nStringId = 21925 },   // [40]
    { .nSoundId = 3375, .nStringId = 21927 },   // [41]
    { .nSoundId = 3368, .nStringId = 21929 },   // [42]
    { .nSoundId = 3367, .nStringId = 21931 },   // [43]
    { .nSoundId = 3374, .nStringId = 21933 },   // [44]
    { .nSoundId = 3370, .nStringId = 21935 },   // [45]
    { .nSoundId = 3373, .nStringId = 21937 },   // [46]
    { .nSoundId = 3376, .nStringId = 21939 },   // [47]
    { .nSoundId = 3369, .nStringId = 21941 },   // [48]
    { .nSoundId = 3377, .nStringId = 21943 },   // [49]
    { .nSoundId = 3378, .nStringId = 21945 },   // [50]
    { .nSoundId = 3372, .nStringId = 21947 },   // [51]
    { .nSoundId = 3391, .nStringId = 21949 },   // [52]
    { .nSoundId = 3385, .nStringId = 21951 },   // [53]
    { .nSoundId = 3386, .nStringId = 21954 },   // [54]
    { .nSoundId = 3388, .nStringId = 21956 },   // [55]
    { .nSoundId = 3381, .nStringId = 21958 },   // [56]
    { .nSoundId = 3383, .nStringId = 21960 },   // [57]
    { .nSoundId = 3390, .nStringId = 21962 },   // [58]
    { .nSoundId = 3387, .nStringId = 21964 },   // [59]
    { .nSoundId = 3382, .nStringId = 21966 },   // [60]
    { .nSoundId = 3389, .nStringId = 21968 },   // [61]
    { .nSoundId = 3384, .nStringId = 21970 },   // [62]
    { .nSoundId = 3392, .nStringId = 21972 },   // [63]
    { .nSoundId = 3341, .nStringId = 10916 },   // [64]
    { .nSoundId = 3342, .nStringId = 21894 },   // [65]
    { .nSoundId = 3394, .nStringId = 21987 },   // [66]
    { .nSoundId = 3395, .nStringId = 22008 },   // [67]
    { .nSoundId = 3396, .nStringId = 22022 },   // [68]
    { .nSoundId = 3397, .nStringId = 22029 },   // [69]
    { .nSoundId = 3398, .nStringId = 21994 },   // [70]
    { .nSoundId = 3399, .nStringId = 22001 },   // [71]
    { .nSoundId = 3400, .nStringId = 22015 },   // [72]
    { .nSoundId = 3401, .nStringId = 22036 },   // [73]
};
static_assert(sizeof(gSpeechSubtitlesAssassin) == 296, "gSpeechSubtitlesAssassin: the item's 296 bytes in the .i64");

//  D2Client 0x6FB8E360
//  Seven pointers, one per character class, indexed by pUnit->dwClassId, to the seven speech subtitle tables 0x6FB8DB48..0x6FB8E238 (stride
//  0x128). The eighth dword (0x58A91207) is not an address, which fixes the count at 7. Each table is up to 0x4A { nSoundId, nStringId }
//  records, which is the lookup loop's bound.
//  TEXT_ChatShowSpeechSubtitle reads it as a two-level lookup (`mov ecx, gpSpeechSubtitleTables[eax*4]`, then `mov dx, [ecx+eax*4]`); the
//  record's first word, the sound id, is bounded to 2934..4656 (`cmp edi, 0B76h` and `cmp edi, 1230h`).
inline D2ClientSpeechSubtitleStrc *gpSpeechSubtitleTables[7] =
{
    gSpeechSubtitlesAmazon,   // [0]
    gSpeechSubtitlesSorceress,   // [1]
    gSpeechSubtitlesNecromancer,   // [2]
    gSpeechSubtitlesPaladin,   // [3]
    gSpeechSubtitlesBarbarian,   // [4]
    gSpeechSubtitlesDruid,   // [5]
    gSpeechSubtitlesAssassin,   // [6]
};
static_assert(sizeof(gpSpeechSubtitleTables) == 28, "gpSpeechSubtitleTables: the item's 28 bytes in the .i64");

//  D2Client 0x6FB8E37C
//  The monster speech-subtitle table: six { nSoundId, nStringId } records, searched linearly by TEXT_ChatShowSpeechSubtitle when the speaking
//  unit is a UNIT_MONSTER. The loop's own bound is 6.
inline D2ClientSpeechSubtitleStrc gMonsterSpeechSubtitles[6] =
{
    { .nSoundId = 4615, .nStringId = 22697 },   // [0]
    { .nSoundId = 4625, .nStringId = 22698 },   // [1]
    { .nSoundId = 4612, .nStringId = 1069 },   // [2]
    { .nSoundId = 4616, .nStringId = 1069 },   // [3]
    { .nSoundId = 4613, .nStringId = 1087 },   // [4]
    { .nSoundId = 4619, .nStringId = 1087 },   // [5]
};
static_assert(sizeof(gMonsterSpeechSubtitles) == 24, "gMonsterSpeechSubtitles: the item's 24 bytes in the .i64");

//  D2Client 0x6FB8E540
//  The assert text `Unicode::strlen( StrGetStringByIndex(strPlayerNotInBattlenet) ) < MAX_GLOBAL_MESSAGE_LEN-MAX_PLR_NAME_LEN-30`, 109 bytes,
//  pushed by TEXT_HandleSystemMessage; read-only. The `-30` is a margin the author left for the rest of the line.
inline char gszAssertGlobalMessageLen[109] = "Unicode::strlen( StrGetStringByIndex(strPlayerNotInBattlenet) ) < MAX_GLOBAL_MESSAGE_LEN-MAX_PLR_NAME_LEN-30";
static_assert(sizeof(gszAssertGlobalMessageLen) == 109, "gszAssertGlobalMessageLen: the item's 109 bytes in the .i64");

//  D2Client 0x6FBB9D90
//  The 16 RECTs TEXT_PlaceHoverMsg fits hover messages into. Written by TEXT_PlaceHoverMsg and TEXT_TickStaticInfoPanel, and its address is taken
//  four times inside the placement routine, so it doubles as the scratch rect the fitting arithmetic works in. It sits before text.cpp's state
//  block and is not part of it.
inline RECT gHoverMsgRect[16];
static_assert(sizeof(gHoverMsgRect) == 256, "gHoverMsgRect: the item's 256 bytes in the .i64");

//  D2Client 0x6FBB9E90
//  The speech popup's record, a parsed-text list holder whose line nodes are { pszText, pNext }, freed by TEXT_FreeParsedLines. Written by
//  TEXT_OnPopupDismissed, TEXT_Shutdown, TEXT_TickStaticInfoPanel, TEXT_ShowSpeechPopup, TEXT_ShowQuestScrollingText, TEXT_ClosePopup and
//  TEXT_ClearMessages; TEXT_IsPopupActive reports whether it is non-NULL, which is how the unit-label and selection code keeps clear of it.
//  It is not sgPopup.pParsedText (0x6FBB9F38), which holds the same kind of object; TEXT_OnPopupDismissed frees the two on alternative
//  branches. It sits 8 bytes before sgTextState's base and is not part of it.
inline D2ClientPopupStrc *gPopupTextList;
static_assert(sizeof(gPopupTextList) == 4, "gPopupTextList: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9E98
//  text.cpp's static text-subsystem state, 159 bytes, packed. The extent is TEXT_Init's inline whole-struct memset.
inline D2TextStateStrc sgTextState;
static_assert(sizeof(sgTextState) == 159, "sgTextState: the item's 159 bytes in the .i64");

//  D2Client 0x6FBB9F38
//  text.cpp's popup record. Only +0x00 is established, pParsedText, named by TEXT_OnPopupDismissed's assert; where the record ends is not known.
inline D2PopupStrc sgPopup;
static_assert(sizeof(sgPopup) == 4, "sgPopup: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9F3C
//  The tick at which the static info panel's message expires. TEXT_TickStaticInfoPanel tests it (`cmp gnStaticInfoDeadline, GetTickCount()` and a
//  `jnb` that skips the whole draw) immediately after sgTextState.bDisplayingStaticInfo, and TEXT_OnMessageListPacket is the only writer, so the
//  deadline is set when the server's message list arrives and the panel stops when the clock passes it.
inline uint32_t gnStaticInfoDeadline;
static_assert(sizeof(gnStaticInfoDeadline) == 4, "gnStaticInfoDeadline: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9F40
//  Head of the parsed message list. TEXT_GetMessageListHead is nothing but `return gpMessageListHead;`; TEXT_OnMessageListPacket both sets and
//  clears it.
inline D2TextHeaderStrc *gpMessageListHead;
static_assert(sizeof(gpMessageListHead) == 4, "gpMessageListHead: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9F44
//  uint16[6]: the order of the five Cairn Stones as the server sent it. Written by TEXT_QuestOnCairnStoneOrderCmd (0x6FB1EDFC, 0x6FB1EE04,
//  0x6FB1EE17), which QUESTS_DispatchQuestPacket reaches by a tail jump (0x6FAB8E43) for server packet 0x50, in the arm for quest 4
//  (QUEST_A1Q4_CAIN). The copy is three dwords, so all twelve bytes are one object. Read by TEXT_DrawQuestScrollFrame, once per stone, each
//  value indexing sgRunePos (bounded by the client's own assert `w < arrsize(sgRunePos)`).
inline uint16_t gwCairnStoneOrder[6];
static_assert(sizeof(gwCairnStoneOrder) == 12, "gwCairnStoneOrder: the item's 12 bytes in the .i64");

//  D2Client 0x6FBB9F50
//  Whether the quest scroll panel is open. TEXT_IniScrollOpen sets it on one branch (0x6FB1EE52, before it sends the item message) and clears it
//  on the other (0x6FB1EE79); TEXT_QuestOnCairnStoneOrderCmd clears it (0x6FB1EE0D). TEXT_ScrollDrawQuestScrollPanel is the only reader.
inline BOOL gbQuestScrollOpen;
static_assert(sizeof(gbQuestScrollOpen) == 4, "gbQuestScrollOpen: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9F54
//  Left edge of the speech popup. TEXT_ShowSpeechPopup sets it to `(gnScreenWidth - 325) / 2`, centred for the placeholder width and not for the
//  measured one. Read by TEXT_DrawPopup and TEXT_TickStaticInfoPanel.
//  The initialiser is not the image's: the image holds 0 here, past .data's raw data. The original's CRT_StaticInit16 (0x6FB1A7A0) fills it at
//  load from gnScreenWidth, with a signed halving (`cdq / sub eax, edx / sar eax, 1`): (gnScreenWidth - 325) / 2. The declaration does what that
//  initialiser did; gnScreenWidth is CoreArchive.h's, included above, constant-initialised.
inline int32_t gnPopupX = (gnScreenWidth - 325) / 2;
static_assert(sizeof(gnPopupX) == 4, "gnPopupX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9F58
//  Optional continuation for the speech popup. TEXT_OnPopupDismissed calls it as `gpfnPopupCallback(gpPopupCallbackArg, 1, bDismissedByMouse)` and
//  treats a non-zero return as 'handled', which suppresses the NPCMENU_EndInteraction that would otherwise close the conversation. That is how a
//  scripted sequence advances to its next line instead of ending.
inline int32_t (__fastcall *gpfnPopupCallback)(uint32_t, uint32_t, uint32_t);
static_assert(sizeof(gpfnPopupCallback) == 4, "gpfnPopupCallback: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9F5C
//  First argument handed to gpfnPopupCallback.
inline int32_t gpPopupCallbackArg;
static_assert(sizeof(gpPopupCallbackArg) == 4, "gpPopupCallbackArg: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9F60
//  The sound currently playing behind a speech popup, or 0 when none. TEXT_StopPopupSpeechSound clears it; TEXT_OnPopupDismissed reads and
//  rewrites it.
inline D2C_Sounds gnPopupSpeechSoundId;
static_assert(sizeof(gnPopupSpeechSoundId) == 4, "gnPopupSpeechSoundId: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9F64
//  A one-shot: when it holds 1, TEXT_DrawPopup clears it and returns (`if (gbPopupDismissPending == 1) { gbPopupDismissPending = 0; return; }`).
//  The test (0x6FB1C188) is reached only after a size test on the popup's scroll position and comes after every draw TEXT_DrawPopup makes; the
//  `ret` it reaches skips nothing.
//  Both references are in that function and nothing in the module sets it to 1; whether something outside the module writes it is
//  NOT ESTABLISHED.
inline BOOL gbPopupDismissPending;
static_assert(sizeof(gbPopupDismissPending) == 4, "gbPopupDismissPending: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9F68
//  Whether the popup's seven message handlers are hooked onto the game window. TEXT_ShowSpeechPopup registers them on the first popup and sets
//  this; TEXT_ClosePopup unregisters and clears it. The handler table is gPopupMessageHandlers.
inline int32_t gbPopupHandlersRegistered;
static_assert(sizeof(gbPopupHandlersRegistered) == 4, "gbPopupHandlersRegistered: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9F70
//  Cel handle of the quest scroll panel image `UI\menu\scroin3`, loaded by TEXT_DrawQuestScrollFrame. All three scroll images are freed together
//  in TEXT_Shutdown and in TEXT_IniScrollOnMouseDown, each guarded by a null test.
inline D2CellFileStrc *gIniScrollImage3;
static_assert(sizeof(gIniScrollImage3) == 4, "gIniScrollImage3: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9F74
//  Cel handle of the quest scroll panel image `UI\menu\scroin2`, loaded by TEXT_DrawQuestScrollFrame. All three scroll images are freed together
//  in TEXT_Shutdown and in TEXT_IniScrollOnMouseDown, each guarded by a null test.
inline D2CellFileStrc *gIniScrollImage2;
static_assert(sizeof(gIniScrollImage2) == 4, "gIniScrollImage2: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9F78
//  Cel handle of the quest scroll panel image `UI\MENU\scroin`, loaded by TEXT_IniScrollDrawPanel. All three scroll images are freed together
//  in TEXT_Shutdown and in TEXT_IniScrollOnMouseDown, each guarded by a null test.
inline D2CellFileStrc *gIniScrollImage;
static_assert(sizeof(gIniScrollImage) == 4, "gIniScrollImage: the item's 4 bytes in the .i64");

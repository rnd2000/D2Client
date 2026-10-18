//  QuestLog.h -- the globals of QuestLog.cpp that are ours: the quest log's close rectangles, icon names, text blocks, button positions, act
//  ranges and the 41 quest definitions, then the panel's state, flags, image handles and per-entry arrays.
//  Each is a C++17 inline variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above a global
//  gives its address in the original. gQuestLogCloseRects is computed from gnScreenWidth and gnScreenHeight, which is why this header
//  includes CoreArchive.h first.
//  Include it after D2Client.h.
#pragma once

#include "CoreArchive.h"

//  D2Client 0x6FB7D0E0
//  The quest-log frame's close box, one rectangle per UI layout: 4 rows of 5 dwords, left/right/top/bottom and then nTitleY, the panel
//  heading's Y (read once, at 0x6FB06725). QUESTLOG_OnMouseDown and QUESTLOG_OnMouseUp pick the row with `5 * nLayout`, nLayout 0..3 across
//  the four arms that set it: it depends on the resolution mode and on whether the new stats panel is open beside the quest log.
//  The original fills the table at load time with a static initialiser (the image holds only 40 and 75 in the first two dwords), so ours
//  computes it in the declaration, from the 640 and 480 the globals hold at start: rows 1 and 3 take nLeft and nRight from gnScreenWidth / 2
//  plus 40 and 75; nTop, nBottom and nTitleY come from gnScreenHeight less 140 / 105 / 143 (rows 0 and 1) and 195 / 160 / 198 (rows 2 and 3);
//  row 2's 40 and 75 and row 0's nLeft and nRight are constants.
inline D2QuestLogCloseRectStrc gQuestLogCloseRects[4] =
{
    { 40, 75, gnScreenHeight - 140, gnScreenHeight - 105, gnScreenHeight - 143 },
    { gnScreenWidth / 2 + 40, gnScreenWidth / 2 + 75, gnScreenHeight - 140, gnScreenHeight - 105, gnScreenHeight - 143 },
    { 40, 75, gnScreenHeight - 195, gnScreenHeight - 160, gnScreenHeight - 198 },
    { gnScreenWidth / 2 + 40, gnScreenWidth / 2 + 75, gnScreenHeight - 195, gnScreenHeight - 160, gnScreenHeight - 198 },
};
static_assert(sizeof(gQuestLogCloseRects) == 80, "gQuestLogCloseRects: the item's 80 bytes in the .i64");

//  D2Client 0x6FB7D130
//  The quest icon .dc6 basenames, `a1q1` through `a5q6`, loaded from DATA\GLOBAL\ui\menu\<name> by QUESTLOG_SelectActPage. 27 entries:
//  gQuestLogActIconRange's five pairs cover 0..26 with no gap.
inline const char *gQuestLogIconNames[27] =
{
    "a1q1",
    "a1q2",
    "a1q3",
    "a1q4",
    "a1q5",
    "a1q6",
    "a2q1",
    "a2q2",
    "a2q3",
    "a2q4",
    "a2q5",
    "a2q6",
    "a3q1",
    "a3q2",
    "a3q3",
    "a3q4",
    "a3q5",
    "a3q6",
    "a4q1",
    "a4q2",
    "a4q3",
    "a5q1",
    "a5q2",
    "a5q3",
    "a5q4",
    "a5q5",
    "a5q6",
};
static_assert(sizeof(gQuestLogIconNames) == 108, "gQuestLogIconNames: the item's 108 bytes in the .i64");

//  D2Client 0x6FB7D19C
//  28 records of 64 bytes: the quest-log text blocks gQuestLogQuestDefs' pTextBlock points at (27 of its 41 records hold one; block [12] is
//  pointed at by none). QUESTLOG_BuildQuestDisplayInfo compares a block pointer with blocks [11] (Seven Tombs), [19] (Fallen Angel) and [22]
//  (Siege of Harrogath), and QuestLog.cpp compares with &gQuestLogTextBlocks[11], [19] and [22].
inline D2QuestLogTextBlockStrc gQuestLogTextBlocks[28] =
{
    {
        .wTitleStringId = 3714,
        .wDescStringId = 76,
        .wStateIndex = 4,
        .tStates = {
            { .wTextStringId = 3735, .wTailStringId = 64 }, { .wTextStringId = 3736, .wTailStringId = 64 },
            { .wTextStringId = 3737, .wTailStringId = 64 }, { .wTextStringId = 3738, .wTailStringId = 64 },
            { .wTextStringId = 3740, .wTailStringId = 64 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3740, .wTailStringId = 64 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 64 }
        },
    },
    {
        .wTitleStringId = 3715,
        .wDescStringId = 92,
        .wStateIndex = 2,
        .tStates = {
            { .wTextStringId = 3741, .wTailStringId = 81 }, { .wTextStringId = 3742, .wTailStringId = 81 },
            { .wTextStringId = 3743, .wTailStringId = 81 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3743, .wTailStringId = 81 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 81 }
        },
    },
    {
        .wTitleStringId = 3716,
        .wDescStringId = 163,
        .wStateIndex = 65535,
        .tStates = {
            { .wTextStringId = 3755, .wTailStringId = 146 }, { .wTextStringId = 3756, .wTailStringId = 146 },
            { .wTextStringId = 3733, .wTailStringId = 146 }, { .wTextStringId = 3732, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3757, .wTailStringId = 163 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 146 }
        },
    },
    {
        .wTitleStringId = 3717,
        .wDescStringId = 123,
        .wStateIndex = 65535,
        .tStates = {
            { .wTextStringId = 3744, .wTailStringId = 97 }, { .wTextStringId = 3745, .wTailStringId = 97 },
            { .wTextStringId = 3746, .wTailStringId = 97 }, { .wTextStringId = 3747, .wTailStringId = 97 },
            { .wTextStringId = 3748, .wTailStringId = 97 }, { .wTextStringId = 3749, .wTailStringId = 97 },
            { .wTextStringId = 3734, .wTailStringId = 97 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3750, .wTailStringId = 97 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 97 }
        },
    },
    {
        .wTitleStringId = 3718,
        .wDescStringId = 127,
        .wStateIndex = 65535,
        .tStates = {
            { .wTextStringId = 3751, .wTailStringId = 127 }, { .wTextStringId = 3754, .wTailStringId = 127 },
            { .wTextStringId = 3752, .wTailStringId = 127 }, { .wTextStringId = 3753, .wTailStringId = 127 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 127 }
        },
    },
    {
        .wTitleStringId = 3719,
        .wDescStringId = 184,
        .wStateIndex = 9,
        .tStates = {
            { .wTextStringId = 3758, .wTailStringId = 166 }, { .wTextStringId = 3759, .wTailStringId = 166 },
            { .wTextStringId = 3761, .wTailStringId = 166 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3760, .wTailStringId = 166 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 184 }
        },
    },
    {
        .wTitleStringId = 923,
        .wDescStringId = 334,
        .wStateIndex = 2,
        .tStates = {
            { .wTextStringId = 941, .wTailStringId = 304 }, { .wTextStringId = 942, .wTailStringId = 304 },
            { .wTextStringId = 943, .wTailStringId = 304 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 943, .wTailStringId = 3725 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 304 }
        },
    },
    {
        .wTitleStringId = 924,
        .wDescStringId = 339,
        .wStateIndex = 65535,
        .tStates = {
            { .wTextStringId = 944, .wTailStringId = 3725 }, { .wTextStringId = 945, .wTailStringId = 335 },
            { .wTextStringId = 946, .wTailStringId = 338 }, { .wTextStringId = 948, .wTailStringId = 3725 },
            { .wTextStringId = 947, .wTailStringId = 339 }, { .wTextStringId = 947, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3730, .wTailStringId = 3725 }, { .wTextStringId = 947, .wTailStringId = 3725 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 339 }
        },
    },
    {
        .wTitleStringId = 925,
        .wDescStringId = 3725,
        .wStateIndex = 65535,
        .tStates = {
            { .wTextStringId = 950, .wTailStringId = 3725 }, { .wTextStringId = 951, .wTailStringId = 3725 },
            { .wTextStringId = 952, .wTailStringId = 348 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 952, .wTailStringId = 3725 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 3725 }
        },
    },
    {
        .wTitleStringId = 926,
        .wDescStringId = 3725,
        .wStateIndex = 65535,
        .tStates = {
            { .wTextStringId = 954, .wTailStringId = 3725 }, { .wTextStringId = 953, .wTailStringId = 373 },
            { .wTextStringId = 953, .wTailStringId = 377 }, { .wTextStringId = 955, .wTailStringId = 377 },
            { .wTextStringId = 956, .wTailStringId = 396 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 956, .wTailStringId = 396 },
            { .wTextStringId = 3728, .wTailStringId = 396 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 396 }
        },
    },
    {
        .wTitleStringId = 927,
        .wDescStringId = 3725,
        .wStateIndex = 65535,
        .tStates = {
            { .wTextStringId = 957, .wTailStringId = 3725 }, { .wTextStringId = 958, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 3725 }, { .wTextStringId = 959, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 959, .wTailStringId = 3725 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 3725 }
        },
    },
    {
        .wTitleStringId = 928,
        .wDescStringId = 442,
        .wStateIndex = 4,
        .tStates = {
            { .wTextStringId = 960, .wTailStringId = 430 }, { .wTextStringId = 962, .wTailStringId = 431 },
            { .wTextStringId = 11030, .wTailStringId = 431 }, { .wTextStringId = 964, .wTailStringId = 431 },
            { .wTextStringId = 965, .wTailStringId = 302 }, { .wTextStringId = 966, .wTailStringId = 442 },
            { .wTextStringId = 961, .wTailStringId = 396 }, { .wTextStringId = 3729, .wTailStringId = 3725 },
            { .wTextStringId = 3730, .wTailStringId = 3725 }, { .wTextStringId = 965, .wTailStringId = 3725 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 442 }
        },
    },
    {
        .wTitleStringId = 929,
        .wDescStringId = 3725,
        .wStateIndex = 65535,
        .tStates = {
            { .wTextStringId = 967, .wTailStringId = 465 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }
        },
    },
    {
        .wTitleStringId = 930,
        .wDescStringId = 564,
        .wStateIndex = 65535,
        .tStates = {
            { .wTextStringId = 968, .wTailStringId = 549 }, { .wTextStringId = 969, .wTailStringId = 549 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3730, .wTailStringId = 3725 },
            { .wTextStringId = 3729, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 3725 }
        },
    },
    {
        .wTitleStringId = 931,
        .wDescStringId = 548,
        .wStateIndex = 65535,
        .tStates = {
            { .wTextStringId = 970, .wTailStringId = 543 }, { .wTextStringId = 971, .wTailStringId = 543 },
            { .wTextStringId = 972, .wTailStringId = 543 }, { .wTextStringId = 973, .wTailStringId = 543 },
            { .wTextStringId = 974, .wTailStringId = 543 }, { .wTextStringId = 975, .wTailStringId = 548 },
            { .wTextStringId = 976, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 3725 }
        },
    },
    {
        .wTitleStringId = 932,
        .wDescStringId = 587,
        .wStateIndex = 65535,
        .tStates = {
            { .wTextStringId = 993, .wTailStringId = 3725 }, { .wTextStringId = 977, .wTailStringId = 571 },
            { .wTextStringId = 978, .wTailStringId = 571 }, { .wTextStringId = 979, .wTailStringId = 571 },
            { .wTextStringId = 980, .wTailStringId = 587 }, { .wTextStringId = 981, .wTailStringId = 587 },
            { .wTextStringId = 3730, .wTailStringId = 3725 }, { .wTextStringId = 3729, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3728, .wTailStringId = 3725 },
            { .wTextStringId = 3727, .wTailStringId = 3725 }, { .wTextStringId = 3726, .wTailStringId = 587 }
        },
    },
    {
        .wTitleStringId = 933,
        .wDescStringId = 3725,
        .wStateIndex = 4,
        .tStates = {
            { .wTextStringId = 982, .wTailStringId = 3725 }, { .wTextStringId = 983, .wTailStringId = 527 },
            { .wTextStringId = 984, .wTailStringId = 529 }, { .wTextStringId = 985, .wTailStringId = 531 },
            { .wTextStringId = 986, .wTailStringId = 534 }, { .wTextStringId = 3729, .wTailStringId = 3725 },
            { .wTextStringId = 3730, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 986, .wTailStringId = 534 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 3725 }
        },
    },
    {
        .wTitleStringId = 934,
        .wDescStringId = 3725,
        .wStateIndex = 3,
        .tStates = {
            { .wTextStringId = 994, .wTailStringId = 3725 }, { .wTextStringId = 987, .wTailStringId = 594 },
            { .wTextStringId = 988, .wTailStringId = 594 }, { .wTextStringId = 989, .wTailStringId = 594 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 989, .wTailStringId = 594 },
            { .wTextStringId = 3728, .wTailStringId = 626 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 626 }
        },
    },
    {
        .wTitleStringId = 935,
        .wDescStringId = 3725,
        .wStateIndex = 4,
        .tStates = {
            { .wTextStringId = 995, .wTailStringId = 3725 }, { .wTextStringId = 990, .wTailStringId = 628 },
            { .wTextStringId = 991, .wTailStringId = 628 }, { .wTextStringId = 992, .wTailStringId = 628 },
            { .wTextStringId = 935, .wTailStringId = 628 }, { .wTextStringId = 3729, .wTailStringId = 3725 },
            { .wTextStringId = 3730, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3728, .wTailStringId = 3725 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 3725 }
        },
    },
    {
        .wTitleStringId = 937,
        .wDescStringId = 675,
        .wStateIndex = 65535,
        .tStates = {
            { .wTextStringId = 996, .wTailStringId = 670 }, { .wTextStringId = 997, .wTailStringId = 670 },
            { .wTextStringId = 998, .wTailStringId = 670 }, { .wTextStringId = 999, .wTailStringId = 675 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 999, .wTailStringId = 3725 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 676 }
        },
    },
    {
        .wTitleStringId = 938,
        .wDescStringId = 3725,
        .wStateIndex = 65535,
        .tStates = {
            { .wTextStringId = 1004, .wTailStringId = 681 }, { .wTextStringId = 1005, .wTailStringId = 681 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 3725 }
        },
    },
    {
        .wTitleStringId = 939,
        .wDescStringId = 3725,
        .wStateIndex = 65535,
        .tStates = {
            { .wTextStringId = 1000, .wTailStringId = 678 }, { .wTextStringId = 1001, .wTailStringId = 678 },
            { .wTextStringId = 1002, .wTailStringId = 678 }, { .wTextStringId = 1000, .wTailStringId = 679 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3729, .wTailStringId = 3725 },
            { .wTextStringId = 3730, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3728, .wTailStringId = 3725 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 3725 }
        },
    },
    {
        .wTitleStringId = 22618,
        .wDescStringId = 3725,
        .wStateIndex = 65535,
        .tStates = {
            { .wTextStringId = 22619, .wTailStringId = 20077 }, { .wTextStringId = 22620, .wTailStringId = 20077 },
            { .wTextStringId = 22621, .wTailStringId = 20077 }, { .wTextStringId = 21786, .wTailStringId = 20090 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3729, .wTailStringId = 3725 },
            { .wTextStringId = 3730, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 21786, .wTailStringId = 20090 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 3725 }
        },
    },
    {
        .wTitleStringId = 22622,
        .wDescStringId = 3725,
        .wStateIndex = 65535,
        .tStates = {
            { .wTextStringId = 22623, .wTailStringId = 20096 }, { .wTextStringId = 22624, .wTailStringId = 20104 },
            { .wTextStringId = 22625, .wTailStringId = 20096 }, { .wTextStringId = 22626, .wTailStringId = 20096 },
            { .wTextStringId = 21789, .wTailStringId = 20096 }, { .wTextStringId = 3729, .wTailStringId = 3725 },
            { .wTextStringId = 3730, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 22625, .wTailStringId = 20096 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 3725 }
        },
    },
    {
        .wTitleStringId = 22627,
        .wDescStringId = 3725,
        .wStateIndex = 65535,
        .tStates = {
            { .wTextStringId = 22628, .wTailStringId = 20116 }, { .wTextStringId = 21788, .wTailStringId = 20116 },
            { .wTextStringId = 22629, .wTailStringId = 20116 }, { .wTextStringId = 22630, .wTailStringId = 20116 },
            { .wTextStringId = 22631, .wTailStringId = 20116 }, { .wTextStringId = 22632, .wTailStringId = 20116 },
            { .wTextStringId = 3730, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 22631, .wTailStringId = 20116 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 3725 }
        },
    },
    {
        .wTitleStringId = 22633,
        .wDescStringId = 3725,
        .wStateIndex = 65535,
        .tStates = {
            { .wTextStringId = 22634, .wTailStringId = 20137 }, { .wTextStringId = 21787, .wTailStringId = 20137 },
            { .wTextStringId = 22635, .wTailStringId = 20137 }, { .wTextStringId = 22636, .wTailStringId = 20137 },
            { .wTextStringId = 21790, .wTailStringId = 20148 }, { .wTextStringId = 3729, .wTailStringId = 3725 },
            { .wTextStringId = 3730, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 21790, .wTailStringId = 3725 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 3725 }
        },
    },
    {
        .wTitleStringId = 22637,
        .wDescStringId = 3725,
        .wStateIndex = 3,
        .tStates = {
            { .wTextStringId = 22638, .wTailStringId = 20153 }, { .wTextStringId = 22639, .wTailStringId = 20153 },
            { .wTextStringId = 22640, .wTailStringId = 20002 }, { .wTextStringId = 22639, .wTailStringId = 20002 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3729, .wTailStringId = 3725 },
            { .wTextStringId = 3730, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 22639, .wTailStringId = 20002 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 3725 }
        },
    },
    {
        .wTitleStringId = 22641,
        .wDescStringId = 3725,
        .wStateIndex = 65535,
        .tStates = {
            { .wTextStringId = 21792, .wTailStringId = 20169 }, { .wTextStringId = 21881, .wTailStringId = 20169 },
            { .wTextStringId = 22643, .wTailStringId = 20169 }, { .wTextStringId = 22644, .wTailStringId = 20169 },
            { .wTextStringId = 22645, .wTailStringId = 20169 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3725, .wTailStringId = 3725 },
            { .wTextStringId = 3725, .wTailStringId = 3725 }, { .wTextStringId = 3728, .wTailStringId = 3725 },
            { .wTextStringId = 3728, .wTailStringId = 3725 }, { .wTextStringId = 3727, .wTailStringId = 3725 },
            { .wTextStringId = 3726, .wTailStringId = 3725 }
        },
    },
};
static_assert(sizeof(gQuestLogTextBlocks) == 1792, "gQuestLogTextBlocks: the item's 1792 bytes in the .i64");

//  D2Client 0x6FB7D8A0
//  Where the six quest icons sit on a page. Both the array name and the element type name are Blizzard's, from the assert at 0x6FB7DDD0:
//  `sgQuestLog.nPageActiveButtonCount <= sizeof(sgQuestLogButtonPositions)/sizeof(QUESTLOGBUTTONPOSITIONT)`.
//  Each record is two point pairs and they are not interchangeable: +0/+4 is the DRAW position, passed to TEXTURE_CelDraw in DC6 bottom-left
//  convention, and +8/+12 the hit-test TOP-LEFT of a 56x52 box. A 3x2 grid: draw X steps 97, hit X steps 96.
inline QUESTLOGBUTTONPOSITIONT sgQuestLogButtonPositions[6] =
{
    { .nDrawX = 26, .nDrawY = 121, .nHitX = 32, .nHitY = 65 },
    { .nDrawX = 123, .nDrawY = 121, .nHitX = 128, .nHitY = 65 },
    { .nDrawX = 220, .nDrawY = 121, .nHitX = 224, .nHitY = 65 },
    { .nDrawX = 26, .nDrawY = 218, .nHitX = 32, .nHitY = 163 },
    { .nDrawX = 123, .nDrawY = 218, .nHitX = 128, .nHitY = 163 },
    { .nDrawX = 220, .nDrawY = 218, .nHitX = 224, .nHitY = 163 },
};
static_assert(sizeof(sgQuestLogButtonPositions) == 96, "sgQuestLogButtonPositions: the item's 96 bytes in the .i64");

//  D2Client 0x6FB7D900
//  The first and last gQuestLogIconNames index belonging to each act: {0,5} {6,11} {12,17} {18,20} {21,26}. Act IV has three quests, the rest
//  six. QUESTLOG_SelectActPage walks the range to load an act's icons lazily. Reading a sixth pair would return the first bytes of
//  gQuestLogQuestDefs (0x6FB7D928), which is why it looks like an empty {0,0} act.
inline int32_t gQuestLogActIconRange[5][2] = { { 0, 5 }, { 6, 11 }, { 12, 17 }, { 18, 20 }, { 21, 26 } };
static_assert(sizeof(gQuestLogActIconRange) == 40, "gQuestLogActIconRange: the item's 40 bytes in the .i64");

//  D2Client 0x6FB7D928
//  The static quest DEFINITIONS, not to be confused with gQuestLogEntries, the six live display blocks. The same 41 the save file's quest bit
//  array and packet 0x52 use; 27 carry a real quest. A record is 16 bytes:
//
//      +0  bHasIcon     +1  nIconIndex     +2  nSlotInPage   +3  nAct
//      +4  pTextBlock   +8  nQuestId       +12 nQuestStateFlag (= the record index)
inline D2QuestLogQuestDefStrc gQuestLogQuestDefs[41] =
{
    { .nQuestId = QUEST_NUMCLASS },
    { .bHasIcon = 1, .pTextBlock = &gQuestLogTextBlocks[0], .nQuestId = QUEST_A1Q1_DENOFEVIL, .nQuestStateFlag = QUESTSTATEFLAG_A1Q1 },
    {
        .bHasIcon = 1,
        .nIconIndex = 1,
        .nSlotInPage = 1,
        .pTextBlock = &gQuestLogTextBlocks[1],
        .nQuestId = QUEST_A1Q2_BLOODRAVEN,
        .nQuestStateFlag = QUESTSTATEFLAG_A1Q2,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 2,
        .nSlotInPage = 4,
        .pTextBlock = &gQuestLogTextBlocks[2],
        .nQuestId = QUEST_A1Q3_MALUS,
        .nQuestStateFlag = QUESTSTATEFLAG_A1Q3,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 3,
        .nSlotInPage = 2,
        .pTextBlock = &gQuestLogTextBlocks[3],
        .nQuestId = QUEST_A1Q4_CAIN,
        .nQuestStateFlag = QUESTSTATEFLAG_A1Q4,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 4,
        .nSlotInPage = 3,
        .pTextBlock = &gQuestLogTextBlocks[4],
        .nQuestId = QUEST_A1Q5_COUNTESS,
        .nQuestStateFlag = QUESTSTATEFLAG_A1Q5,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 5,
        .nSlotInPage = 5,
        .pTextBlock = &gQuestLogTextBlocks[5],
        .nQuestId = QUEST_A1Q6_ANDARIEL,
        .nQuestStateFlag = QUESTSTATEFLAG_A1Q6,
    },
    { .nQuestId = QUEST_NUMCLASS, .nQuestStateFlag = QUESTSTATEFLAG_A1COMPLETED },
    { .nAct = 1, .nQuestId = QUEST_NUMCLASS, .nQuestStateFlag = QUESTSTATEFLAG_A2Q0 },
    {
        .bHasIcon = 1,
        .nIconIndex = 6,
        .nAct = 1,
        .pTextBlock = &gQuestLogTextBlocks[6],
        .nQuestId = QUEST_A2Q1_RADAMENT,
        .nQuestStateFlag = QUESTSTATEFLAG_A2Q1,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 7,
        .nSlotInPage = 1,
        .nAct = 1,
        .pTextBlock = &gQuestLogTextBlocks[7],
        .nQuestId = QUEST_A2Q2_HORADRICSTAFF,
        .nQuestStateFlag = QUESTSTATEFLAG_A2Q2,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 8,
        .nSlotInPage = 2,
        .nAct = 1,
        .pTextBlock = &gQuestLogTextBlocks[8],
        .nQuestId = QUEST_A2Q3_TAINTEDSUN,
        .nQuestStateFlag = QUESTSTATEFLAG_A2Q3,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 9,
        .nSlotInPage = 3,
        .nAct = 1,
        .pTextBlock = &gQuestLogTextBlocks[9],
        .nQuestId = QUEST_A2Q4_HORAZONTOME,
        .nQuestStateFlag = QUESTSTATEFLAG_A2Q4,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 10,
        .nSlotInPage = 4,
        .nAct = 1,
        .pTextBlock = &gQuestLogTextBlocks[10],
        .nQuestId = QUEST_A2Q5_SUMMONER,
        .nQuestStateFlag = QUESTSTATEFLAG_A2Q5,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 11,
        .nSlotInPage = 5,
        .nAct = 1,
        .pTextBlock = &gQuestLogTextBlocks[11],
        .nQuestId = QUEST_A2Q6_DURIEL,
        .nQuestStateFlag = QUESTSTATEFLAG_A2Q6,
    },
    { .nAct = 1, .nQuestId = QUEST_NUMCLASS, .nQuestStateFlag = QUESTSTATEFLAG_A2COMPLETED },
    { .nAct = 2, .nQuestId = QUEST_NUMCLASS, .nQuestStateFlag = QUESTSTATEFLAG_A3Q0 },
    {
        .bHasIcon = 1,
        .nIconIndex = 12,
        .nSlotInPage = 3,
        .nAct = 2,
        .pTextBlock = &gQuestLogTextBlocks[13],
        .nQuestId = QUEST_A3Q1_LAMESENTOME,
        .nQuestStateFlag = QUESTSTATEFLAG_A3Q1,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 13,
        .nSlotInPage = 2,
        .nAct = 2,
        .pTextBlock = &gQuestLogTextBlocks[14],
        .nQuestId = QUEST_A3Q2_KHALIMFLAIL,
        .nQuestStateFlag = QUESTSTATEFLAG_A3Q2,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 14,
        .nSlotInPage = 1,
        .nAct = 2,
        .pTextBlock = &gQuestLogTextBlocks[15],
        .nQuestId = QUEST_A3Q3_GIDBINN,
        .nQuestStateFlag = QUESTSTATEFLAG_A3Q3,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 15,
        .nAct = 2,
        .pTextBlock = &gQuestLogTextBlocks[16],
        .nQuestId = QUEST_A3Q4_GOLDENBIRD,
        .nQuestStateFlag = QUESTSTATEFLAG_A3Q4,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 16,
        .nSlotInPage = 4,
        .nAct = 2,
        .pTextBlock = &gQuestLogTextBlocks[17],
        .nQuestId = QUEST_A3Q5_TRAVINCAL,
        .nQuestStateFlag = QUESTSTATEFLAG_A3Q5,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 17,
        .nSlotInPage = 5,
        .nAct = 2,
        .pTextBlock = &gQuestLogTextBlocks[18],
        .nQuestId = QUEST_A3Q6_MEPHISTO,
        .nQuestStateFlag = QUESTSTATEFLAG_A3Q6,
    },
    { .nAct = 2, .nQuestId = QUEST_NUMCLASS, .nQuestStateFlag = QUESTSTATEFLAG_A3COMPLETED },
    { .nAct = 3, .nQuestId = QUEST_NUMCLASS, .nQuestStateFlag = QUESTSTATEFLAG_A4Q0 },
    {
        .bHasIcon = 1,
        .nIconIndex = 18,
        .nAct = 3,
        .pTextBlock = &gQuestLogTextBlocks[19],
        .nQuestId = QUEST_A4Q1_IZUAL,
        .nQuestStateFlag = QUESTSTATEFLAG_A4Q1,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 20,
        .nSlotInPage = 2,
        .nAct = 3,
        .pTextBlock = &gQuestLogTextBlocks[20],
        .nQuestId = QUEST_A4Q2_DIABLO,
        .nQuestStateFlag = QUESTSTATEFLAG_A4Q2,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 19,
        .nSlotInPage = 1,
        .nAct = 3,
        .pTextBlock = &gQuestLogTextBlocks[21],
        .nQuestId = QUEST_A4Q3_HELLFORGE,
        .nQuestStateFlag = QUESTSTATEFLAG_A4Q3,
    },
    { .nAct = 3, .nQuestId = QUEST_NUMCLASS, .nQuestStateFlag = QUESTSTATEFLAG_A4COMPLETED },
    { .nAct = 9, .nQuestId = QUEST_NUMCLASS, .nQuestStateFlag = QUESTSTATEFLAG_A1Q7 },
    { .nAct = 9, .nQuestId = QUEST_NUMCLASS, .nQuestStateFlag = QUESTSTATEFLAG_A2Q7 },
    { .nAct = 9, .nQuestId = QUEST_NUMCLASS, .nQuestStateFlag = QUESTSTATEFLAG_A2Q8 },
    { .nAct = 9, .nQuestId = QUEST_NUMCLASS, .nQuestStateFlag = QUESTSTATEFLAG_A3Q7 },
    { .nAct = 9, .nQuestId = QUEST_NUMCLASS, .nQuestStateFlag = QUESTSTATEFLAG_A4Q4 },
    { .nAct = 4, .nQuestId = (D2Quests)30, .nQuestStateFlag = (D2QuestStateFlagIds)34 },
    {
        .bHasIcon = 1,
        .nIconIndex = 21,
        .nAct = 4,
        .pTextBlock = &gQuestLogTextBlocks[22],
        .nQuestId = QUEST_A5Q1_SHENK,
        .nQuestStateFlag = QUESTSTATEFLAG_A5Q1,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 22,
        .nSlotInPage = 1,
        .nAct = 4,
        .pTextBlock = &gQuestLogTextBlocks[23],
        .nQuestId = QUEST_A5Q2_RESCUESOLDIERS,
        .nQuestStateFlag = QUESTSTATEFLAG_A5Q2,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 23,
        .nSlotInPage = 2,
        .nAct = 4,
        .pTextBlock = &gQuestLogTextBlocks[24],
        .nQuestId = QUEST_A5Q3_PRISONOFICE,
        .nQuestStateFlag = QUESTSTATEFLAG_A5Q3,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 24,
        .nSlotInPage = 3,
        .nAct = 4,
        .pTextBlock = &gQuestLogTextBlocks[25],
        .nQuestId = QUEST_A5Q4_NIHLATHAK,
        .nQuestStateFlag = QUESTSTATEFLAG_A5Q4,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 25,
        .nSlotInPage = 4,
        .nAct = 4,
        .pTextBlock = &gQuestLogTextBlocks[26],
        .nQuestId = QUEST_A5Q5_ANCIENTS,
        .nQuestStateFlag = QUESTSTATEFLAG_A5Q5,
    },
    {
        .bHasIcon = 1,
        .nIconIndex = 26,
        .nSlotInPage = 5,
        .nAct = 4,
        .pTextBlock = &gQuestLogTextBlocks[27],
        .nQuestId = QUEST_A5Q6_BAAL,
        .nQuestStateFlag = QUESTSTATEFLAG_A5Q6,
    },
};
static_assert(sizeof(gQuestLogQuestDefs) == 656, "gQuestLogQuestDefs: the item's 656 bytes in the .i64");

//  D2Client 0x6FB7DBB8
//  Read once, by QUESTLOG_Draw. It is also the first dword of the record that QUESTLOG_Draw's 42nd pass reads just past gQuestLogQuestDefs
//  (its nAct is this int's top byte, 0xFF). Reading past an object is undefined, so QuestLog.cpp builds that record from this and the next
//  three globals' bytes.
inline int32_t gnQuestLogDoneYOffset = -1;
static_assert(sizeof(gnQuestLogDoneYOffset) == 4, "gnQuestLogDoneYOffset: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7DBBC
//  Read once, by QUESTLOG_Draw.
inline int32_t gnQuestLogSelectedSocketYOffset = -1;
static_assert(sizeof(gnQuestLogSelectedSocketYOffset) == 4, "gnQuestLogSelectedSocketYOffset: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7DBC0
//  The first of three consecutive layout constants -- 16, 248, 270, here and in gnQuestLogTitleY and gnQuestLogTextY -- read together by
//  QUESTLOG_Draw into the quest log's text drawing (loaded at 0x6FB04B74 beside gnBorderXDifference and gnBorderYDifference).
//  Its ADDRESS is also the end bound of the walks over gQuestLogQuestDefs' nQuestId column (`cmp eax, 6FB7DBC0h`, 0x6FB06AE3, 0x6FB06B69,
//  0x6FB06BF0), eight bytes past the table's end. QuestLog.cpp computes those bounds from gQuestLogQuestDefs with an integer cursor and names
//  this only for its value.
inline int32_t gnQuestLogTextX = 16;
static_assert(sizeof(gnQuestLogTextX) == 4, "gnQuestLogTextX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7DBC4
//  The second of the three layout constants, value 248, loaded by QUESTLOG_Draw at 0x6FB04B02 next to gnBorderYDifference.
//  QUESTLOG_BuildQuestDisplayInfo never reads the value: its one use there is the address, `cmp eax, 6FB7DBC4h` (0x6FB04D93), the end of a
//  walk over the +0xC column (nQuestStateFlag, from 0x6FB7D934). QuestLog.cpp computes that bound from gQuestLogQuestDefs.
inline int32_t gnQuestLogTitleY = 248;
static_assert(sizeof(gnQuestLogTitleY) == 4, "gnQuestLogTitleY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7DBC8
//  The third of the three layout constants, value 270, read by QUESTLOG_Draw.
inline int32_t gnQuestLogTextY = 270;
static_assert(sizeof(gnQuestLogTextY) == 4, "gnQuestLogTextY: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5F40
//  The quest each act page has highlighted, or -1 for none. QUESTLOG_SelectActPage reuses an act's entry when it is set and searches for one
//  when it is not; `bClearSelections` resets all five, and the five explicit stores at 0x6FBB5F40/44/48/4C/50 are what fix the count.
inline int32_t gnQuestLogSelectedPerAct[5];
static_assert(sizeof(gnQuestLogSelectedPerAct) == 20, "gnQuestLogSelectedPerAct: the item's 20 bytes in the .i64");

//  D2Client 0x6FBB5F58
//  The quest-log panel's state machine: 0 closed, 1 opening, 2 open. Written by QUESTLOG_TogglePanel and by QUESTLOG_Draw, which resets it to 0
//  on each of its three early exits -- so the panel closing itself and the panel being closed by a keypress converge on the same state.
inline int32_t gnQuestLogPanelState;
static_assert(sizeof(gnQuestLogPanelState) == 4, "gnQuestLogPanelState: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5F60
//  How many quest entries the log is currently showing. Bounds the walk over gQuestLogEntries (618-byte records) in
//  QUESTLOG_AcknowledgeAndClose, QUESTLOG_Draw, QUESTLOG_QuestScreenOnMouseDown and QUESTLOG_QuestScreenOnMouseUp.
inline int32_t gnQuestLogEntryCount;
static_assert(sizeof(gnQuestLogEntryCount) == 4, "gnQuestLogEntryCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5F64
//  The Den of Evil's monsters left to kill, written from server packet 0x50 and from packet 0x5D's field_4 (nQuestFilter == 1).
//  QUESTLOG_BuildQuestDisplayInfo prints it into the Den of Evil row (title tbl 3714) as 'Monsters remaining: ' (3738) plus the number, or
//  'One monster left.' (3739) when it is 0 or 1. Writers: QUESTLOG_QuestResetAllState 0x6FB0418D, QUESTLOG_QuestOnQuestLogStateCmd
//  0x6FB06291, QUESTLOG_QuestOnQuestStatePacket 0x6FB06B3D -- which replaces a count outside 0..5 with 666 at 0x6FB06B48.
inline int32_t gnMonstersRemaining;
static_assert(sizeof(gnMonstersRemaining) == 4, "gnMonstersRemaining: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5F68
//  The cel frame of the quest picture QUESTLOG_Draw draws with gQuestLogInvPsImage, at (0x6E + gnBorderXDifference, gnScreenHeight - 0x6E +
//  gnBorderYDifference). Clamped by its reader, not its writer: it arrives as a word out of packet 0x50 (the +5 word, written by
//  QUESTLOG_QuestOnQuestLogStateCmd at 0x6FB0629C) and QUESTLOG_Draw substitutes 0 when it is outside 0..6.
inline int32_t gnQuestLogPictureFrame;
static_assert(sizeof(gnQuestLogPictureFrame) == 4, "gnQuestLogPictureFrame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5F6C
//  The Act 5 soldiers still to be rescued. QUESTLOG_QuestOnQuestLogStateCmd (0x6FB062A6) stores the +7 word of packet 0x50;
//  QUESTLOG_QuestOnQuestStatePacket (0x6FB06AA2) stores packet 0x5D's field_4 in its quest-32 arm; QUESTLOG_QuestResetAllState (0x6FB04193)
//  clears it. QUESTLOG_BuildQuestDisplayInfo's A5Q2 arm (title tbl 22622) formats it with tbl 22624 'Rescue %d more Soldiers in the Frigid
//  Highlands.' whenever it is non-zero.
inline int32_t gnBarbsToBeRescued;
static_assert(sizeof(gnBarbsToBeRescued) == 4, "gnBarbsToBeRescued: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5F70
//  The quest-log frame's close box, held down. QUESTLOG_OnMouseDown sets it when the click lands inside gQuestLogCloseRects[nLayout] and plays
//  sound 4; QUESTLOG_OnMouseUp requires it still set, and a release inside the same rect does `SetUIToggle(UI_QUESTLOG, UI_TURNOFF, 0)` then
//  QUESTLOG_TogglePanel -- the press-and-release pairing that stops a drag from outside closing the panel. QUESTLOG_DrawFrameAndTitle reads it
//  as a cel frame. This byte belongs to the quest-log FRAME while the four after it belong to the quest SCREEN. Cleared on every path that
//  resets the panel.
inline uint8_t gbQuestLogCloseBoxDown;
static_assert(sizeof(gbQuestLogCloseBoxDown) == 1, "gbQuestLogCloseBoxDown: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB5F71
//  Set to 1 by QUESTLOG_QuestScreenOnMouseDown when the press lands on a quest icon whose entry is clickable, and cleared by
//  QUESTLOG_QuestScreenOnMouseUp, QUESTLOG_QuestScreenOnMouseDown and QUESTLOG_QuestResetAllState. Write-only: nothing in the module reads it.
//  The press it records is tracked by gnQuestScreenPressedQuest, which is set on the same line and IS read.
inline uint8_t gbQuestScreenIconDown;
static_assert(sizeof(gbQuestScreenIconDown) == 1, "gbQuestScreenIconDown: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB5F72
//  The quest screen's own close button, held down. Rect (278..313, 392..425) in panel coordinates. QUESTLOG_QuestScreenOnMouseDown sets it;
//  QUESTLOG_QuestScreenOnMouseUp calls QUESTLOG_TogglePanel(0, gnBorderYDifference) when the release lands in the same rect; QUESTLOG_Draw
//  draws the button as frame `(gbQuestScreenCloseButtonDown != 0) + 10`, so 10 up and 11 down.
//  D2ExpRes patches this pair of sites and calls them `QuestCloseButtonClickdown` and `QuestCloseButtonRelease`.
inline uint8_t gbQuestScreenCloseButtonDown;
static_assert(sizeof(gbQuestScreenCloseButtonDown) == 1, "gbQuestScreenCloseButtonDown: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB5F73
//  Set while the quest description is being spoken, and it blocks the whole screen while set. QUESTLOG_QuestScreenOnMouseUp starts the speech
//  -- `TEXT_ShowQuestScrollingText(nDescStrId, 1, 0)` then `TEXT_SetCallback(QUESTLOG_OnSpeechFinished, 0)` -- and sets this last.
//  QUESTLOG_QuestScreenOnMouseDown swallows the click when it is set (VIDEO_IsCinematicPlaying and two early exits come first);
//  QUESTLOG_QuestScreenOnMouseUp tests it before every branch except the close button, so close still works while a quest is talking and
//  nothing else does. Cleared by QUESTLOG_OnSpeechFinished and by QUESTLOG_Draw on each of its three early exits, so a panel that closes
//  mid-speech does not leave the flag stuck.
inline uint8_t gbQuestScreenSpeechPlaying;
static_assert(sizeof(gbQuestScreenSpeechPlaying) == 1, "gbQuestScreenSpeechPlaying: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB5F74
//  The quest screen's speech button, held down. Rect (229..260, 390..424) in panel coordinates, immediately left of the close button. On
//  release inside the rect, and only with a quest selected, QUESTLOG_QuestScreenOnMouseUp finds that quest's entry and speaks its nDescStrId.
//  Tbl 3724 and 3725 are refused: those two are the placeholder lines, so a quest with nothing to say has a button that does nothing.
//  QUESTLOG_Draw draws it as frame `gbQuestScreenSpeechButtonDown != 0`, so 0 up and 1 down -- a different frame base from the close button.
//  D2ExpRes calls the same two sites `QuestSpeechButtonClickdown` and `QuestSpeechButtonRelease`.
inline uint8_t gbQuestScreenSpeechButtonDown;
static_assert(sizeof(gbQuestScreenSpeechButtonDown) == 1, "gbQuestScreenSpeechButtonDown: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB5F75
//  The quest icon the mouse went DOWN on, or -1. QUESTLOG_QuestScreenOnMouseDown stores the icon index the press hit;
//  QUESTLOG_QuestScreenOnMouseUp recomputes the index under the release and commits the selection only if the two agree, which is what makes
//  dragging off an icon cancel it; QUESTLOG_Draw draws that icon with cel frame 0x19 instead of 0 while it is held. Reset to -1 by
//  QUESTLOG_SetActTab and by every path that clears the screen; 0 would be a valid icon, which is why -1 is the empty value.
inline int32_t gnQuestScreenPressedQuest;
static_assert(sizeof(gnQuestScreenPressedQuest) == 4, "gnQuestScreenPressedQuest: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5F79
//  The quest currently highlighted on the open page, or -1. Set by QUESTLOG_SelectActPage from either the per-act memory or its search, and reset
//  to -1 by QUESTLOG_SetActTab.
inline int32_t gnQuestLogSelectedQuest;
static_assert(sizeof(gnQuestLogSelectedQuest) == 4, "gnQuestLogSelectedQuest: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5F7D
//  The quest last CLICKED on each act's page, or -1. Written by QUESTLOG_QuestScreenOnMouseUp when a click commits, cleared by
//  QUESTLOG_QuestResetAllState. Read once, by QUESTLOG_SelectActPage, and only in the fallback arm: when the walk over all 41 quest definitions
//  found completed quests but nothing in progress, it reopens on this remembered choice.
//  It is NOT gnQuestLogSelectedPerAct, the other int[5]: that one holds the act's CURRENT highlight, is written by
//  QUESTLOG_QuestOnQuestStatePacket when the SERVER changes a quest, and is consulted first -- if it is set, this array is never reached.
//  The act tab that indexes it is clamped by QUESTLOG_SetActTab from above only: `cmp esi, 5 / jl` zeroes 5 and up (0x6FB05D83..0x6FB05D88);
//  a negative value would skip the quest tests (`lea eax, [esi-1] / cmp eax, 3 / ja`) and be stored.
inline int32_t gnQuestLogLastClickedPerAct[5];
static_assert(sizeof(gnQuestLogLastClickedPerAct) == 20, "gnQuestLogLastClickedPerAct: the item's 20 bytes in the .i64");

//  D2Client 0x6FBB5F91
//  A quest-log image handle: `questsockets`, the sockets each quest icon sits in. Loaded by QUESTLOG_LoadPanelImages from
//  DATA\GLOBAL\ui\menu\questsockets, cleared by QUESTLOG_QuestUnloadPanelImages, read by QUESTLOG_Draw. It is one of six handles laid out as
//  unaligned dwords from 0x6FBB5F91.
inline D2CellFileStrc *gQuestLogSocketsImage;
static_assert(sizeof(gQuestLogSocketsImage) == 4, "gQuestLogSocketsImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5F95
//  A quest-log image handle: `questbackground`, the panel's backdrop. Loaded by QUESTLOG_LoadPanelImages, cleared by
//  QUESTLOG_QuestUnloadPanelImages, read by QUESTLOG_Draw.
inline D2CellFileStrc *gQuestLogBackgroundImage;
static_assert(sizeof(gQuestLogBackgroundImage) == 4, "gQuestLogBackgroundImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5F99
//  A quest-log image handle: `questtabs` or `expquesttabs`, the act tabs across the top. Two possible files, chosen by the expansion check above
//  the store, so the same handle serves both. Loaded by QUESTLOG_LoadPanelImages, cleared by QUESTLOG_QuestUnloadPanelImages, read by
//  QUESTLOG_Draw.
inline D2CellFileStrc *gQuestLogTabsImage;
static_assert(sizeof(gQuestLogTabsImage) == 4, "gQuestLogTabsImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5F9D
//  A quest-log image handle: `questdone`, the overlay drawn on a completed quest. Loaded by QUESTLOG_LoadPanelImages, cleared by
//  QUESTLOG_QuestUnloadPanelImages, read by QUESTLOG_Draw.
inline D2CellFileStrc *gQuestLogDoneImage;
static_assert(sizeof(gQuestLogDoneImage) == 4, "gQuestLogDoneImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5FA1
//  27 loaded image handles, parallel to gQuestLogIconNames, filled lazily by QUESTLOG_SelectActPage and freed by
//  QUESTLOG_QuestUnloadPanelImages. An unloaded slot is NULL, which is both the load test and the draw test.
//  The odd base address is not an error: this and the quest-log's other state are fields of the packed `sgQuestLog` struct the asserts name.
inline D2CellFileStrc *gQuestLogIconImages[27];
static_assert(sizeof(gQuestLogIconImages) == 108, "gQuestLogIconImages: the item's 108 bytes in the .i64");

//  D2Client 0x6FBB600D
//  A quest-log image handle: `questlast`, loaded last and the only one whose file name says nothing about what it shows. Loaded by
//  QUESTLOG_LoadPanelImages, cleared by QUESTLOG_QuestUnloadPanelImages, read by QUESTLOG_Draw.
//  Its address is gQuestLogIconImages' end: QUESTLOG_QuestUnloadPanelImages' loop bound (`cmp esi, offset gQuestLogLastImage`, 0x6FB042B7),
//  which QuestLog.cpp writes as gQuestLogIconImages + 27.
inline D2CellFileStrc *gQuestLogLastImage;
static_assert(sizeof(gQuestLogLastImage) == 4, "gQuestLogLastImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB6011
//  A quest-log image handle: `invps`, the shared inventory-panel artwork this panel borrows. Loaded by QUESTLOG_LoadPanelImages, cleared by
//  QUESTLOG_QuestUnloadPanelImages, read by QUESTLOG_Draw.
inline D2CellFileStrc *gQuestLogInvPsImage;
static_assert(sizeof(gQuestLogInvPsImage) == 4, "gQuestLogInvPsImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB6015
//  A verbatim 42-byte copy of server packet 0x52, made by QUESTLOG_ApplyQuestListPacket -- header byte included, so index 0 is 0x52 and the 41
//  quest status bytes start at index 1. The original's copy (`rep movsd` / `movsw`, 0x6FB0623E) writes this byte and the 41 bytes of
//  gQuestLogEntryFlags after it; ours writes them by name, the header byte here and the 41 quest states there.
//  QUESTLOG_QuestOnQuestStatePacket's store at index -1 of the flags (0x6FB06B77, LATENT) writes this byte too, and ours names it.
inline uint8_t gQuestListPacketCopy;
static_assert(sizeof(gQuestListPacketCopy) == 1, "gQuestListPacketCopy: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB6016
//  41 bytes, one per quest status of packet 0x52, read by QUESTLOG_BuildQuestDisplayInfo as the entry's stored state colour before the quest
//  record can override it. The copy's end, 0x6FBB6016 + 41 = 0x6FBB603F, is one byte short of gQuestLogEntryColours (0x6FBB6040).
inline char gQuestLogEntryFlags[41];
static_assert(sizeof(gQuestLogEntryFlags) == 41, "gQuestLogEntryFlags: the item's 41 bytes in the .i64");

//  D2Client 0x6FBB6040
//  41 bytes, the colour QUESTLOG_BuildQuestDisplayInfo last decided for each entry (11 for a previous-difficulty completion, 13 for a completion
//  in this one). Written when the info block is built; its readers are in QUESTLOG_BuildQuestDisplayInfo (QUESTLOG_Draw does not read it).
inline uint8_t gQuestLogEntryColours[41];
static_assert(sizeof(gQuestLogEntryColours) == 41, "gQuestLogEntryColours: the item's 41 bytes in the .i64");

//  D2Client 0x6FBB6069
//  Six quest slots per act page, 618 bytes each (`add esi, 26Ah` states the stride in three functions), filled by
//  QUESTLOG_BuildQuestDisplayInfo and read by QUESTLOG_Draw and the two QUESTSCREEN mouse handlers. 3724 is the placeholder `qstsxxx` and BOTH
//  string fields (+5 title, +607 tail) default to it.
//  The original's CRT_StaticInit14 runs Unicode's default constructor over each record's 300 wszText elements (a zero store over zero-filled
//  bytes); the struct types the field `unsigned __int16[300]`, so ours is zero-initialised and calls nothing: the same state.
inline D2QuestLogEntryStrc gQuestLogEntries[6];
static_assert(sizeof(gQuestLogEntries) == 3708, "gQuestLogEntries: the item's 3708 bytes in the .i64");

//  D2Client 0x6FBB6EE5
//  The quest-log icon's animation FRAME, one per entry, indexed by gnQuestLogEntryCount. Advanced by QUESTLOG_Draw only when the companion
//  timestamp gnQuestLogIconLastTick says 100 ms have passed (increment at 0x6FB047AB..0x6FB047B3). Frame 1 is special and expansion-only: the
//  branch (`call FOG_IsExpansion`, 0x6FB047CD) runs only when the frame has just reached 1 AND FOG_IsExpansion is true.
inline int32_t gnQuestLogIconFrame[6];
static_assert(sizeof(gnQuestLogIconFrame) == 24, "gnQuestLogIconFrame: the item's 24 bytes in the .i64");

//  D2Client 0x6FBB6EFD
//  The tick the quest-log icon last advanced, one per entry, indexed by gnQuestLogEntryCount; the timing half of the pair with
//  gnQuestLogIconFrame. A still-zero entry is seeded with GetTickCount (0x6FB04777..0x6FB04780), and an entry advances when the elapsed time
//  reaches 100 ms (`cmp edx, 64h`), so the icons step at ten frames a second regardless of the game's frame rate.
//  QUESTLOG_SetActTab stores 0 into all six and into gnQuestLogIconFrame's six; the original's walk is bounded by gnQuestLogActTab's address
//  (0x6FB05E1B), the array's end, and QuestLog.cpp walks by index.
inline int32_t gnQuestLogIconLastTick[6];
static_assert(sizeof(gnQuestLogIconLastTick) == 24, "gnQuestLogIconLastTick: the item's 24 bytes in the .i64");

//  D2Client 0x6FBB6F15
//  Which act tab the quest log is showing, 0..4. Written only by QUESTLOG_SetActTab, which clamps it down to the furthest act the character has
//  reached before storing it; QUESTLOG_Draw compares each tab index against it to pick the lit cel.
inline int32_t gnQuestLogActTab;
static_assert(sizeof(gnQuestLogActTab) == 4, "gnQuestLogActTab: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB6F19
//  GetTickCount at the moment the quest log was last closed. QUESTLOG_TogglePanel refuses to reopen within 250 ms of it, which is what stops
//  one keypress from closing and immediately reopening the panel.
inline uint32_t gdwQuestLogCloseTick;
static_assert(sizeof(gdwQuestLogCloseTick) == 4, "gdwQuestLogCloseTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB6F1D
//  Set once by QUEST_PreloadDurielCof so the COF load runs a single time. Cleared by QUESTLOG_QuestResetAllState and by both quest-screen mouse
//  handlers, which clear all four flags in this run together.
inline uint8_t gbDurielCofLoaded;
static_assert(sizeof(gbDurielCofLoaded) == 1, "gbDurielCofLoaded: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB6F1E
//  Set once by QUEST_PreloadAncientStatueCof. Same lifecycle as gbDurielCofLoaded.
inline uint8_t gbAncientStatueCofLoaded;
static_assert(sizeof(gbAncientStatueCofLoaded) == 1, "gbAncientStatueCofLoaded: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB6F1F
//  Arms the Zoo object. Set by QUESTS_DispatchQuestPacket in its 0x24 arm, read by QUEST_IsZooArmed and by QUEST_SpawnZooChickens, cleared by
//  QUESTLOG_QuestResetAllState and the quest-screen mouse handlers.
inline uint8_t gbZooArmed;
static_assert(sizeof(gbZooArmed) == 1, "gbZooArmed: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB6F20
//  Raised by QUEST_SpawnZooChickens on every unit it creates, read by QUEST_ZooChickensPresent. On its own it would latch; it is the unit-id
//  lookup beside it that lets the spawn happen again.
inline uint8_t gbZooChickensSpawned;
static_assert(sizeof(gbZooChickensSpawned) == 1, "gbZooChickensSpawned: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB6F21
//  The client id of the LAST chicken QUEST_SpawnZooChickens created, or -1 when the create failed. QUEST_ZooChickensPresent looks it up to
//  decide whether the spawn may run again.
inline int32_t gnZooChickenUnitId;
static_assert(sizeof(gnZooChickenUnitId) == 4, "gnZooChickenUnitId: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB6F25
//  Write-only: one reference in the module, QUEST_ArmZooEffect's store at 0x6FB06ED7. The value comes from server packet 0x50 case 36
//  (QUEST_A5Q6_BAAL), the word at +3 of the payload, filled on the send side with the quest's own zoo monster id. There is no reader in stock
//  1.10.
inline int32_t gnZooMonsterId;
static_assert(sizeof(gnZooMonsterId) == 4, "gnZooMonsterId: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB6F2C
//  Read once, by QUESTLOG_Draw. Nothing writes it and its value is 0: the loader zero-fills this part of .data.
inline int32_t gnQuestLogDoneXOffset;
static_assert(sizeof(gnQuestLogDoneXOffset) == 4, "gnQuestLogDoneXOffset: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB6F30
//  Read once, by QUESTLOG_Draw (0x6FB049BD). Nothing writes it and its value is 0, as gnQuestLogDoneXOffset.
inline int32_t gnQuestLogSocketYOffset;
static_assert(sizeof(gnQuestLogSocketYOffset) == 4, "gnQuestLogSocketYOffset: the item's 4 bytes in the .i64");

//  skillstree.h -- the globals of skillstree.cpp's data that are ours, read and written by skillstree.cpp alone: the selected page, the seven
//  skill-tree charts, the tab and its offsets, the cell state and palette arrays, and the button and panel flags. Each is a C++17 `inline`
//  variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above a global gives its address in the
//  original. The last eight lie in 0x6FBB9438..0x6FBB9850, past .data's raw data, so they are zero at load.
//  Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB8CF00
//  Which of the three skill tabs the tree panel is showing, 1, 2 or 3. SKILLSTREE_OnLButtonDown writes it; skillstree.cpp filters skills by it.
inline int32_t gnSelectedSkillPage = 1;
static_assert(sizeof(gnSelectedSkillPage) == 4, "gnSelectedSkillPage: the item's 4 bytes in the .i64");

//  D2Client 0x6FB8CF08
//  The skill-tree background per character class (the game's own name, from SKILLSTREE_LoadChartImage's assert): seven records of 34 bytes, a
//  loaded image handle and a 30-byte path "Spells\skltree_<c>_back" for a, s, n, p, b, d, i (D2C_PlayerClasses order).
inline D2ClientSkillTreeChartStrc sgptPlayerSkillTreeChart[7] =
{
    { .szFileName = "Spells\\skltree_a_back" },   // [0]
    { .szFileName = "Spells\\skltree_s_back" },   // [1]
    { .szFileName = "Spells\\skltree_n_back" },   // [2]
    { .szFileName = "Spells\\skltree_p_back" },   // [3]
    { .szFileName = "Spells\\skltree_b_back" },   // [4]
    { .szFileName = "Spells\\skltree_d_back" },   // [5]
    { .szFileName = "Spells\\skltree_i_back" },   // [6]
};
static_assert(sizeof(sgptPlayerSkillTreeChart) == 238, "sgptPlayerSkillTreeChart: the item's 238 bytes in the .i64");

//  D2Client 0x6FB8CFF8
//  Which of the three skill tabs is showing, 1, 2 or 3; it starts at 1. SKILLSTREE_DrawPanel stores it every frame by comparing
//  gnSkillTabXOffsets[page + 3 * class] with -149, -220 and -305; its reads only compare it with 1 to pick the tab's x offset.
inline int32_t gnSkillTreeTab = 1;
static_assert(sizeof(gnSkillTreeTab) == 4, "gnSkillTreeTab: the item's 4 bytes in the .i64");

//  D2Client 0x6FB8CFFC
//  The close button's X relative to the right edge, per class and page: index page + 3 * class, 22 dwords (the seven classes reach index 21).
//  [0] is 0, [1] -149, [2] -220, [3..5] -305, and entries 1..21 are all -149, -220 or -305. SKILLSTREE_DrawPanel derives gnSkillTreeTab from it.
inline int32_t gnSkillTabXOffsets[22] =
{ 0, -149, -220, -305, -305, -305, -149, -305, -149, -305, -305, -220, -305, -149, -305, -149, -149, -149, -149, -220, -149, -305 };
static_assert(sizeof(gnSkillTabXOffsets) == 88, "gnSkillTabXOffsets: the item's 88 bytes in the .i64");

//  D2Client 0x6FBB9438
//  The palette shift used to draw each skill-tree cell: 0 when the skill can be raised, 5 when it cannot (greyed out). Parallel to
//  gnSkillTreeCellState, 0x200 bytes after it. The length is not established: the index is a per-skill counter with no bound of its own, and
//  128 dwords is the gap before gnSkillTreeCellState.
inline int32_t gnSkillTreeCellPalette[128];
static_assert(sizeof(gnSkillTreeCellPalette) == 512, "gnSkillTreeCellPalette: the item's 512 bytes in the .i64");

//  D2Client 0x6FBB9638
//  Per-cell state, parallel to gnSkillTreeCellPalette: 1 (armed by a press), 0, or -1 (cannot be raised). Only the raisable arm of
//  SKILLSTREE_DrawAvailableSkills leaves a cell holding 1 alone; the other arm writes -1 over it. The index is as unbounded as the palette's.
inline int32_t gnSkillTreeCellState[128];
static_assert(sizeof(gnSkillTreeCellState) == 512, "gnSkillTreeCellState: the item's 512 bytes in the .i64");

//  D2Client 0x6FBB9838
//  The close button's pressed state: set by SKILLSTREE_OnLButtonDown, cleared by SKILLSTREE_OnLButtonUp, read by SKILLSTREE_DrawPanel.
inline BOOL gbSkillTreeButtonDown;
static_assert(sizeof(gbSkillTreeButtonDown) == 4, "gbSkillTreeButtonDown: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB983C
//  Meant to stop a second skill-point spend going out while one is in flight, but nothing ever sets it: both writes store zero
//  (SKILLSTREE_OnLButtonUp after the send, and SCMD_HandleSrv21_UpdateSkills), so the guard before client command 59 never fires.
inline BOOL gbSkillPointSendPending;
static_assert(sizeof(gbSkillPointSendPending) == 4, "gbSkillPointSendPending: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9840
//  Write-only: SKILLSTREE_DrawPanel sets it and its twin gbHasUnspentSkillPoints with the same value, and only the twin is read.
inline BOOL gbHasUnspentSkillPointsUnread;
static_assert(sizeof(gbHasUnspentSkillPointsUnread) == 4, "gbHasUnspentSkillPointsUnread: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9844
//  The player has unspent skill points, set by SKILLSTREE_DrawPanel from STAT_SKILLPTS > 0. SKILLSTREE_DrawUnavailableSkills reads it to choose
//  whether to draw a centred "0"; its one caller stores 0 here just before the call, so it is always 0 when read.
inline BOOL gbHasUnspentSkillPoints;
static_assert(sizeof(gbHasUnspentSkillPoints) == 4, "gbHasUnspentSkillPoints: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB984C
//  The skill tree panel drew this frame: set to 1 near the start of SKILLSTREE_DrawPanel and tested by both mouse handlers, so a click before
//  the panel has drawn is swallowed. Nothing clears it.
inline BOOL gbSkillTreeDrawn;
static_assert(sizeof(gbSkillTreeDrawn) == 4, "gbSkillTreeDrawn: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9850
//  The palette index for pure black, D2WIN_10034_MixRGB(0, 0, 0); stored by SKILLSTREE_CacheBlackIndex and again by SKILLSTREE_DrawTabLabel
//  every frame.
inline uint8_t gnBlackPaletteIndex;
static_assert(sizeof(gnBlackPaletteIndex) == 1, "gnBlackPaletteIndex: the item's 1 bytes in the .i64");

//  SkillDesc.h -- the globals of SkillDesc.cpp: the descdam and descatt handler tables of the skill panel, the elemental damage stat and colour
//  tables, the line offsets and text layout of the skill description panel, the literal gszPlus and the latch bytes.
//  Each is a C++17 `inline` variable, so every file that includes this header shares one object. The comment line "D2Client 0x6F..." above a
//  global gives its address in the original. gnElemDamageStats and gnElemDamageColors live in .rdata and are const. The globals at
//  0x6FBB942C..0x6FBB9434 lie past .data's raw data (0x6FB9B000): the loader zero-fills them and ours are zero-initialised.
//  Include it after D2Client.h, which declares every function named here.
#pragma once

//  D2Client 0x6FB8CD00
//  skilldesc.txt descdam handlers: 36 entries (24 distinct), [0] null, indexed by D2SkillDescTxt.wDescDam (+0x12). The table ends exactly where
//  gpSkillDescAttFuncs begins (+144). The code's bound is 0x90, larger than the array; past the end SKILLDESC_GetDamFuncOrNull in SkillDesc.cpp
//  answers NULL (a declared difference). The slots past the last handler are zero.
inline void (__fastcall *gpSkillDescDamFuncs[36])(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight) =
{
    NULL,   // [0]
    &SKILLDESC_DamFunc1,   // [1]
    &SKILLDESC_DamFunc2,   // [2]
    &SKILLDESC_DamFunc3,   // [3]
    &SKILLDESC_DamFunc4,   // [4]
    &SKILLDESC_DamFunc5,   // [5]
    &SKILLDESC_DamFunc6,   // [6]
    &SKILLDESC_DamFunc7,   // [7]
    &SKILLDESC_DamFunc8,   // [8]
    &SKILLDESC_DamFunc9,   // [9]
    &SKILLDESC_DamFunc10,   // [10]
    &SKILLDESC_DamFunc11,   // [11]
    &SKILLDESC_DamFunc12,   // [12]
    &SKILLDESC_DamFunc13,   // [13]
    &SKILLDESC_DamFunc14,   // [14]
    &SKILLDESC_DamFunc15,   // [15]
    &SKILLDESC_DamFunc16,   // [16]
    &SKILLDESC_DamFunc17,   // [17]
    &SKILLDESC_DamFunc18,   // [18]
    &SKILLDESC_DamFunc19,   // [19]
    &SKILLDESC_DamFunc20,   // [20]
    &SKILLDESC_DamFunc21,   // [21]
    &SKILLDESC_DamFunc22,   // [22]
    &SKILLDESC_DamFunc23,   // [23]
    &SKILLDESC_DamFunc24,   // [24]
};
static_assert(sizeof(gpSkillDescDamFuncs) == 144, "gpSkillDescDamFuncs: the item's 144 bytes in the .i64");

//  D2Client 0x6FB8CD90
//  skilldesc.txt descatt handlers: 18 entries, 0x48 bytes, 1-based ([0] null, SKILLDESC_AttFunc1..5 in [1..5], the rest reserved and zero),
//  indexed by D2SkillDescTxt.wDescAtt (+0x14). The table ends at 0x6FB8CDD8 where gSkillDescPanelLayout begins. The code's bound is 0x48
//  (72, four times the array), so an index of 18..71 lies past the table; stock skilldesc.txt never exceeds 5. Past the end
//  SKILLDESC_GetAttFuncOrNull in SkillDesc.cpp answers NULL (a declared difference). SKILLDESC_GetAttackRatingForHand carries the same bound.
inline void (__fastcall *gpSkillDescAttFuncs[18])(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnAttackRatingA, int32_t *pnColorA, int32_t *pnAttackRatingB, int32_t *pnColorB) =
{ NULL, &SKILLDESC_AttFunc1, &SKILLDESC_AttFunc2, &SKILLDESC_AttFunc3, &SKILLDESC_AttFunc4, &SKILLDESC_AttFunc5 };
static_assert(sizeof(gpSkillDescAttFuncs) == 72, "gpSkillDescAttFuncs: the item's 72 bytes in the .i64");

//  D2Client 0x6FB6FB6C
//  Four rows of three stat ids {min, max, mastery}, walked by SKILLDESC_AddItemElemDamage:
//      48 firemindam,  49 firemaxdam,  329 passive_fire_mastery
//      50 lightmindam, 51 lightmaxdam, 330 passive_ltng_mastery
//      54 coldmindam,  55 coldmaxdam,  331 passive_cold_mastery
//      52 magicmindam, 53 magicmaxdam,  -1 (magic has no mastery)
//  Poison is deliberately absent: it needs its duration and is handled separately after the loop.
inline const int32_t gnElemDamageStats[12] = { 48, 49, 329, 50, 51, 330, 54, 55, 331, 52, 53, -1 };
static_assert(sizeof(gnElemDamageStats) == 48, "gnElemDamageStats: the item's 48 bytes in the .i64");

//  D2Client 0x6FB6FB9C
//  The ui_color_t for each row of gnElemDamageStats, in the same order: 1 UI_COLOR_RED (fire), 9 UI_COLOR_YELLOW (lightning), 3 UI_COLOR_BLUE
//  (cold), 3 UI_COLOR_BLUE (magic). It starts immediately after gnElemDamageStats with no padding, and the original's stat walk ends on
//  &gnElemDamageColors[2] by adjacency only. The loop does read entry [2], on cold's row: SKILLDESC_AddItemElemDamage reads a colour per row
//  through a copied pointer (mov ecx, [eax] at 0x6FB0B89D, the pointer stepped 4 at 0x6FB0B8A8) whenever the row's min or max is not 0.
//  Ours bounds the stat walk from gnElemDamageStats itself (gnElemDamageStats + 14, in dwords).
inline const int32_t gnElemDamageColors[4] = { 1, 9, 3, 3 };
static_assert(sizeof(gnElemDamageColors) == 16, "gnElemDamageColors: the item's 16 bytes in the .i64");

//  D2Client 0x6FB8CCF4
//  Y nudge for the first of the two lines SKILLDESC_DrawTwoLines draws. Initial value -6; set to -7 once, on first use, for languages 6..9
//  (Japanese, Korean, ChineseSin, ChineseTwn) to make room for the taller glyphs. Guarded by gbSkillDescLineOffsetsReady.
//  SKILLDESC_DrawSkillInfo resolves it too, under the same latch (0x6FB1600F, 0x6FB1601B; the latch is set at 0x6FB16025), whichever runs first.
inline int32_t gnSkillDescLine1YOffset = -6;
static_assert(sizeof(gnSkillDescLine1YOffset) == 4, "gnSkillDescLine1YOffset: the item's 4 bytes in the .i64");

//  D2Client 0x6FB8CCF8
//  Y nudge for the second line SKILLDESC_DrawTwoLines draws. Constant 2, read but never written.
inline int32_t gnSkillDescLine2YOffset = 2;
static_assert(sizeof(gnSkillDescLine2YOffset) == 4, "gnSkillDescLine2YOffset: the item's 4 bytes in the .i64");

//  D2Client 0x6FB8CDD8
//  The skill description panel's text geometry, int[36] (144 bytes, 0x6FB8CDD8..0x6FB8CE68): 12 rows of { nLeft, nY, nRight }, passed to
//  CHAR_DrawTextCentered as (x1, y, x2), so a row is a centred span and not a rectangle.
//  Six rows per mouse button: SKILLDESC_DrawBothSkillInfos calls SKILLDESC_DrawSkillInfo with 0 for the left skill and 6 for the right, and the
//  drawer reads rows a3, a3+1, a3+2, a3+3 and a3+5 (row a3+4 is never read). Rows 6..11 repeat rows 0..5 with the same nLeft/nRight and nY + 24.
//  Reads are offset at runtime: gnBorderXDifference + nLeft and gnBorderYDifference + nY + gnScreenHeight - 480. Not every read is: two Y sites
//  take -484 and -476 (0x6FB15EA2, 0x6FB15EDE), and the three reads at 0x6FB15D5B..0x6FB15D67 are pushed as they are, arguments of the damage
//  function. The CJK fixup in SKILLDESC_DrawBothSkillInfos decrements the nY of rows 0 and 6.
inline int32_t gSkillDescPanelLayout[36] =
{
    162,   // [0]
    93,   // [1]
    258,   // [2]
    162,   // [3]
    101,   // [4]
    258,   // [5]
    263,   // [6]
    98,   // [7]
    307,   // [8]
    162,   // [9]
    160,   // [10]
    270,   // [11]
    162,   // [12]
    163,   // [13]
    270,   // [14]
    270,   // [15]
    160,   // [16]
    310,   // [17]
    162,   // [18]
    117,   // [19]
    258,   // [20]
    162,   // [21]
    125,   // [22]
    258,   // [23]
    263,   // [24]
    122,   // [25]
    307,   // [26]
    162,   // [27]
    184,   // [28]
    270,   // [29]
    162,   // [30]
    187,   // [31]
    270,   // [32]
    270,   // [33]
    184,   // [34]
    310,   // [35]
};
static_assert(sizeof(gSkillDescPanelLayout) == 144, "gSkillDescPanelLayout: the item's 144 bytes in the .i64");

//  D2Client 0x6FB8CE8C
//  The string "+" (2 bytes), loaded by SKILLDESC_FormatDamageRange and by nothing else: the sign a stat modifier carries when it is positive.
//  The two bytes after its NUL are zero as well and the source path string "C:\p..." begins at 0x6FB8CE90, so the extent of 2 is the string's.
inline char gszPlus[2] = "+";
static_assert(sizeof(gszPlus) == 2, "gszPlus: the item's 2 bytes in the .i64");

//  D2Client 0x6FBB942C
//  Set once, the first time SKILLDESC_DrawTwoLines runs, after it has resolved gnSkillDescLine1YOffset from the language. A lazy init.
inline uint8_t gbSkillDescLineOffsetsReady;
static_assert(sizeof(gbSkillDescLineOffsetsReady) == 1, "gbSkillDescLineOffsetsReady: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB9430
//  A one-way latch. It gates a full walk of sgptDataTables->pSkillsTxt over all nSkillsTxtRecordCount records, in both
//  SKILLDESC_SkillsBuildSkillLevelText and SKILLDESC_SkillsBuildSkillRequirementText. Both read it and both write 1; nothing clears it, so the
//  scan runs until the first time either function reaches its setting line, and never again for the life of the process.
inline BOOL gbSkillsTxtScanDone;
static_assert(sizeof(gbSkillsTxtScanDone) == 4, "gbSkillsTxtScanDone: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9434
//  State of SKILLDESC_DrawBothSkillInfos, its only writer and reader: the one-time latch in front of the CJK fixup of the panel layout.
inline uint8_t gbSkillDescPanelNudged;
static_assert(sizeof(gbSkillDescPanelNudged) == 1, "gbSkillDescPanelNudged: the item's 1 bytes in the .i64");

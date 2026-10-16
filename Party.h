//  Party.h -- the globals of Party.cpp: the four party-panel hover-hint tables (tbl string indices), four literals, the row list and the images.
//  Address ranges 0x6FB7CFE8..0x6FB7D0B0 and 0x6FBB5EF8..0x6FBB5F3C. Each is a C++17 `inline` variable, one object for every file that includes
//  this header. The tables and the literals hold their values in the image and are initialised to the same; the globals from 0x6FBB5EF8 on lie
//  past .data's raw data and are zero at load.
//  Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB7CFE8
//  tbl string indices of the party-panel hover hint about a MALE character's hostility, indexed `2 * ROSTER_PlayerIsHostileToEntry +
//  ROSTER_EntryIsHostileToPlayer`: 10115 'You are friendly to each other.', 10111 '%s is hostile to you, but you are friendly to him.',
//  10113 '%s is friendly to you, but you are hostile to him.', 10110 'You are hostile towards each other.'
//  Slots 0 and 3 go through Unicode::strcpy, slots 1 and 2 carry a %s and go through Unicode::sprintf: that is why PARTY_DrawHoverHint has
//  two arms. The items are WORDs (`mov cx, word ptr [...+eax*2]`, 0x6FB032A4). Female twin at 0x6FB7CFF0.
inline int16_t gaPartyHostileHintMale[4] = { 10115, 10111, 10113, 10110 };
static_assert(sizeof(gaPartyHostileHintMale) == 8, "gaPartyHostileHintMale: the item's 8 bytes in the .i64");

//  D2Client 0x6FB7CFF0
//  Female form of gaPartyHostileHintMale, selected by ROSTER_EntryIsMale: 10115, 10112 '%s is hostile to you, but you are friendly to her.',
//  10114 '%s is friendly to you, but you are hostile to her.', 10110. Slots 0 and 3 are shared with the male table.
inline int16_t gaPartyHostileHintFemale[4] = { 10115, 10112, 10114, 10110 };
static_assert(sizeof(gaPartyHostileHintFemale) == 8, "gaPartyHostileHintFemale: the item's 8 bytes in the .i64");

//  D2Client 0x6FB7CFF8
//  tbl string indices of the party-panel hover hint about corpse looting with a MALE character, indexed `2 * ROSTER_PlayerAllowsEntryLoot +
//  ROSTER_EntryAllowsPlayerLoot`: 10121 "Neither of you may loot the other's corpse.", 10119 '%s may not loot your corpse, but you may loot his.',
//  10117 '%s may loot your corpse, but you may not loot his.', 10116 "You may loot each other's corpse."
//  Read only in PARTY_DrawHoverHint's `WINMAIN_IsHardcoreGame()` arm. WORD items. Female twin at 0x6FB7D000.
inline int16_t gaPartyLootHintMale[4] = { 10121, 10119, 10117, 10116 };
static_assert(sizeof(gaPartyLootHintMale) == 8, "gaPartyLootHintMale: the item's 8 bytes in the .i64");

//  D2Client 0x6FB7D000
//  Female form of gaPartyLootHintMale, selected by ROSTER_EntryIsMale: 10121, 10120 '%s may not loot your corpse, but you may loot hers.',
//  10118 '%s may loot your corpse, but you may not loot hers.', 10116. Slots 0 and 3 are shared with the male table.
inline int16_t gaPartyLootHintFemale[4] = { 10121, 10120, 10118, 10116 };
static_assert(sizeof(gaPartyLootHintFemale) == 8, "gaPartyLootHintFemale: the item's 8 bytes in the .i64");

//  D2Client 0x6FB7D044
//  The literal "..." that the C runtime's CRT_NMSG_WRITE also pushes.
inline char Source[4] = "...";
static_assert(sizeof(Source) == 4, "Source: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7D050
//  The placeholder name PARTY_DrawHoverHint copies when ROSTER_GetDisplayName answers 0. It never does (it answers entry + 0x66 for every
//  entry, 0x6FAAF5EE), so the copy behind the test at 0x6FB03239 is UNREACHABLE.
inline char gszErrorPlayerName[18] = "ERROR PLAYER NAME";
static_assert(sizeof(gszErrorPlayerName) == 18, "gszErrorPlayerName: the item's 18 bytes in the .i64");

//  D2Client 0x6FB7D074
//  The command prefix PARTY_OnLButtonUp builds a line from by appending a player name; the trailing space is part of it.
inline char gszUnsquelchCommand[12] = "/unsquelch ";
static_assert(sizeof(gszUnsquelchCommand) == 12, "gszUnsquelchCommand: the item's 12 bytes in the .i64");

//  D2Client 0x6FB7D0B0
//  The path of the party panel's own image, loaded by PARTY_LoadImages.
inline char gszPartyBoxesPath[16] = "Menu\\partyboxes";
static_assert(sizeof(gszPartyBoxesPath) == 16, "gszPartyBoxesPath: the item's 16 bytes in the .i64");

//  D2Client 0x6FBB5EF8
//  One shared party id per roster row (ROSTER_PartyGetSharedPartyId, 0xFFFF meaning none or a party of one), in party-list order. Rebuilt from
//  scratch at the top of PARTY_DrawPartyGroupDividers, its only user, and read as adjacent pairs to decide where a divider bar goes. Eight is
//  the party-panel row count.
inline int32_t gnPartyRowPartyIds[8];
static_assert(sizeof(gnPartyRowPartyIds) == 32, "gnPartyRowPartyIds: the item's 32 bytes in the .i64");

//  D2Client 0x6FBB5F18
//  Head of the party panel's linked list of row records. PARTY_BuildRowList frees the old chain and rebuilds it, one row per party member;
//  PARTY_FreeRowList frees it and nulls this. Read by PARTY_DrawPanel, PARTY_OnLButtonDown, PARTY_OnLButtonUp, PARTY_DrawHoverHint and
//  PARTY_DrawControlButtonHints.
inline D2ClientPartyRowStrc *gpPartyRowList;
static_assert(sizeof(gpPartyRowList) == 4, "gpPartyRowList: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5F1C
//  `Menu\party` -- the party panel's background. Loaded by PARTY_LoadImages, freed by PARTY_UnloadImages.
inline D2CellFileStrc *gpPartyPanelImage;
static_assert(sizeof(gpPartyPanelImage) == 4, "gpPartyPanelImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5F20
//  `Menu\partybuttons` -- the per-member hostile/loot buttons. Loaded by PARTY_LoadImages.
inline D2CellFileStrc *gpPartyButtonsImage;
static_assert(sizeof(gpPartyButtonsImage) == 4, "gpPartyButtonsImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5F24
//  `Menu\partyboxes` -- the checkbox art for the party panel's toggles. Loaded by PARTY_LoadImages.
inline D2CellFileStrc *gpPartyBoxesImage;
static_assert(sizeof(gpPartyBoxesImage) == 4, "gpPartyBoxesImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5F28
//  `Panel\inv_goldbtn` -- borrowed from the inventory panel's art, not from Menu\party*. Loaded by PARTY_LoadImages with the other four.
inline D2CellFileStrc *gpPartyGoldButtonImage;
static_assert(sizeof(gpPartyGoldButtonImage) == 4, "gpPartyGoldButtonImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5F2C
//  `Menu\partybar` -- the party member's bar. Loaded by PARTY_LoadImages.
inline D2CellFileStrc *gpPartyBarImage;
static_assert(sizeof(gpPartyBarImage) == 4, "gpPartyBarImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5F34
//  The party panel button's pressed state: set by PARTY_OnLButtonDown, cleared by PARTY_OnLButtonUp, read by PARTY_DrawPanel. Not the same as
//  gbPartyRowButtonDown, which belongs to the per-player buttons.
inline BOOL gbPartyButtonDown;
static_assert(sizeof(gbPartyButtonDown) == 4, "gbPartyButtonDown: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5F38
//  Scratch palette index for the next box drawn with GFX_DrawFilledRect, produced by D2Win_10034_MixRGB. Not a stable constant:
//  PARTY_DrawRowButtons re-mixes it to red for the disabled-hostile overlay, PARTY_DrawControlButtonHints and PARTY_DrawHoverHint re-mix it to
//  black for the hint backdrop, and PANEL_SetBoxFillColorRed and PANEL_SetBoxFillColorBlack both write it at load time, so the load-time value
//  is black. Read it as the colour the caller just chose.
inline uint8_t gnUiBoxFillColor;
static_assert(sizeof(gnUiBoxFillColor) == 1, "gnUiBoxFillColor: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB5F3C
//  A press is in flight on one of the party panel's per-player buttons. PARTY_OnLButtonDown sets it to 1 (0x6FB039D5, 0x6FB03A9B) and never
//  clears it; PARTY_OnLButtonUp reads it as the gate of its whole completion block and clears it only when the walk over the rows ends with no
//  button hit (0x6FB03F07). The hostile button below level 9 (0x6FB03E4A) and a button hit (0x6FB03EDC..0x6FB03F02) leave it set.
inline BOOL gbPartyRowButtonDown;
static_assert(sizeof(gbPartyRowButtonDown) == 4, "gbPartyRowButtonDown: the item's 4 bytes in the .i64");

//  panel.h -- the globals of panel.cpp (the control panel, the belt, the quantity-warning icons and the panel images). Each is a C++17 inline
//  variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above a global gives its address in the
//  original. The globals at 0x6FBB5E18..0x6FBB5EF4 lie past the end of .data's raw data (0x6FB9B000): the loader zero-fills them, and ours are
//  zero-initialised.
//  Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB7CD70
//  The belts.txt row the belt panel is drawing: UNITS_GetBeltType(pEquipped) when the item in the belt body slot has item type 19 (Belt), and 2 --
//  belts.txt's default, the four-box beltless layout -- when the slot is empty; an item there of another type leaves it as it was.
inline int32_t gnBeltsTxtIndex = 2;
static_assert(sizeof(gnBeltsTxtIndex) == 4, "gnBeltsTxtIndex: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7CD74
//  Set to 1 by PANEL_GameOnLButtonDown on every press it accepts inside the control bar (both mini-skill buttons, the mini-panel toggle and the
//  run toggle) and cleared by PANEL_GameOnLButtonUp on the belt-release path. Nothing reads it.
inline BOOL gbGameBarBtnDownUnread = 1;
static_assert(sizeof(gbGameBarBtnDownUnread) == 4, "gbGameBarBtnDownUnread: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7CD78
//  The belt box the cursor is over. PANEL_BeltUpdateHoveredItem stores the box's index whether or not an item is there; only
//  PANEL_BeltResetHoverState stores -1, and a walk that finds no box leaves it as it was.
inline int32_t gnBeltHoverBox = -1;
static_assert(sizeof(gnBeltHoverBox) == 4, "gnBeltHoverBox: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7CD7C
//  X of the belt hint anchor: rect.nLeft + 14 of the hovered item's box. PANEL_BeltUpdateHoveredItem stores it only on its found-item path; the
//  only -1 store is PANEL_BeltResetHoverState's.
inline int32_t gnBeltHintX = -1;
static_assert(sizeof(gnBeltHintX) == 4, "gnBeltHintX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7CD80
//  Y of the belt hint anchor: rect.nTop of the hovered item's box. Stored and reset like gnBeltHintX.
inline int32_t gnBeltHintY = -1;
static_assert(sizeof(gnBeltHintY) == 4, "gnBeltHintY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7CD90
//  The four hotkey command ids (HOTKEY_POTION1..HOTKEY_POTION4, 0x17..0x1A) the belt draw looks up for the key hints. Each is passed to
//  CMDTBL_ControlsFindBoundKey twice, once with bPrimary = 1 and again with 0 when the primary slot returns -1, so a slot shows whichever of its
//  two bindings exists.
inline D2HotkeyIndices gnBeltSlotHotkeys[4] = { HOTKEY_POTION1, HOTKEY_POTION2, HOTKEY_POTION3, HOTKEY_POTION4 };
static_assert(sizeof(gnBeltSlotHotkeys) == 16, "gnBeltSlotHotkeys: the item's 16 bytes in the .i64");

//  D2Client 0x6FB7CDA0
//  The equip slots that get a low-quantity warning: BODYLOC_RARM and BODYLOC_LARM, the two weapon hands, and nothing else. Count in
//  gnQtyWarnBodyLocCount.
inline D2C_PlayerBodyLocs gaQtyWarnBodyLocs[2] = { BODYLOC_RARM, BODYLOC_LARM };
static_assert(sizeof(gaQtyWarnBodyLocs) == 8, "gaQtyWarnBodyLocs: the item's 8 bytes in the .i64");

//  D2Client 0x6FB7CDA8
//  Length of gaQtyWarnBodyLocs.
inline uint32_t gnQtyWarnBodyLocCount = 2;
static_assert(sizeof(gnQtyWarnBodyLocCount) == 4, "gnQtyWarnBodyLocCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7CDB0
//  Five rows, one per qntwarning value in weapons.txt / misc.txt: nKind is the item's qntwarning column, nBaseFrame the first of three consecutive
//  cel frames; a quantity at or below nThresholdHigh shows nBaseFrame, at or below nThresholdMid nBaseFrame + 1, at or below nThresholdLow
//  nBaseFrame + 2. The thresholds are compared unsigned, lowest first.
inline D2QtyWarnLevelStrc gaQtyWarnLevels[5] =
{
    { .nKind = 1, .nThresholdHigh = 50, .nThresholdMid = 25, .nThresholdLow = 10 },   // [0]
    { .nKind = 2, .nBaseFrame = 3, .nThresholdHigh = 15, .nThresholdMid = 10, .nThresholdLow = 5 },   // [1]
    { .nKind = 3, .nBaseFrame = 6, .nThresholdHigh = 5, .nThresholdMid = 4, .nThresholdLow = 2 },   // [2]
    { .nKind = 4, .nBaseFrame = 9, .nThresholdHigh = 15, .nThresholdMid = 10, .nThresholdLow = 5 },   // [3]
    { .nKind = 5, .nBaseFrame = 12, .nThresholdHigh = 5, .nThresholdMid = 4, .nThresholdLow = 2 },   // [4]
};
static_assert(sizeof(gaQtyWarnLevels) == 100, "gaQtyWarnLevels: the item's 100 bytes in the .i64");

//  D2Client 0x6FB7CE14
//  Number of gaQtyWarnLevels rows.
inline uint32_t gnQtyWarnLevelCount = 5;
static_assert(sizeof(gnQtyWarnLevelCount) == 4, "gnQtyWarnLevelCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7CE18
//  Read only, by PANEL_DrawStatusWarningIcons.
inline D2C_PlayerBodyLocs gaWarnIconBodyLocs[7] =
{ BODYLOC_HEAD, BODYLOC_TORSO, BODYLOC_BELT, BODYLOC_FEET, BODYLOC_GLOVES, BODYLOC_RARM, BODYLOC_LARM };
static_assert(sizeof(gaWarnIconBodyLocs) == 28, "gaWarnIconBodyLocs: the item's 28 bytes in the .i64");

//  D2Client 0x6FB7CEB0
//  The string Panel\ctrlpnl_popbelt, the name of gpImgPopBelt's image; referenced once, by PANEL_LoadImages.
inline char gszPopBeltPath[22] = "Panel\\ctrlpnl_popbelt";
static_assert(sizeof(gszPopBeltPath) == 22, "gszPopBeltPath: the item's 22 bytes in the .i64");

//  D2Client 0x6FBB5E18
//  The four-frame cel for the control panel's mini-panel toggle button. Loaded by PANEL_LoadImages, released and nulled by PANEL_UnloadImages,
//  drawn by PANEL_DrawMiniPanelToggleButton.
inline D2CellFileStrc *gpImgMiniPanelToggle;
static_assert(sizeof(gpImgMiniPanelToggle) == 4, "gpImgMiniPanelToggle: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E1C
//  The panel image Panel\ctrlpnl_popbelt, the pop-up belt image. Loaded by PANEL_LoadImages, released and nulled by PANEL_UnloadImages; read by
//  PANEL_DrawBelt.
inline D2CellFileStrc *gpImgPopBelt;
static_assert(sizeof(gpImgPopBelt) == 4, "gpImgPopBelt: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E20
//  The panel image Panel\Arrow. Loaded by PANEL_LoadImages, released and nulled by PANEL_UnloadImages.
inline D2CellFileStrc *gpImgPanelArrow;
static_assert(sizeof(gpImgPanelArrow) == 4, "gpImgPanelArrow: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E24
//  The inventory/character panel backdrop. PANEL_LoadImages loads InvChar6 instead of InvChar when FOG_IsExpansion() && GAME_IsExpansionGame(),
//  and for languages 6..9 (Japanese, Korean, ChineseSin, ChineseTwn) loads the file from DATA\LOCAL\UI\... through LoadImage rather than from the
//  UI directory. Freed and nulled by PANEL_UnloadImages; read by INV_UpdateAndDrawPanels.
inline D2CellFileStrc *gpImgInvCharPanel;
static_assert(sizeof(gpImgInvCharPanel) == 4, "gpImgInvCharPanel: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E28
//  The panel image Panel\DuroIcon, the durability warning. Loaded by PANEL_LoadImages, released and nulled by PANEL_UnloadImages; read by
//  PANEL_DrawStatusWarningIcons.
inline D2CellFileStrc *gpImgDuroIcon;
static_assert(sizeof(gpImgDuroIcon) == 4, "gpImgDuroIcon: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E2C
//  The panel image Panel\btn. Loaded by PANEL_LoadImages, released and nulled by PANEL_UnloadImages.
inline D2CellFileStrc *gpImgPanelButton;
static_assert(sizeof(gpImgPanelButton) == 4, "gpImgPanelButton: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E30
//  The liquid cel for both control-panel globes, loaded by PANEL_LoadImages and released by PANEL_UnloadImages. The frame index selects which:
//  0 life, 1 mana, 2 poisoned life. Drawn with TEXTURE_CelDrawEx over a pixel height of 80 * value / max.
inline D2CellFileStrc *gpImgGlobeFill;
static_assert(sizeof(gpImgGlobeFill) == 4, "gpImgGlobeFill: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E34
//  Panel\GemSocket -- the empty socket cel drawn behind each socketed item. Loaded unconditionally by PANEL_LoadImages, freed by
//  PANEL_UnloadImages. Read by INV_DrawSocketedItems, which draws it once per socket at the position gSocketLayouts gives, one pixel left of
//  where the gem itself goes.
inline D2CellFileStrc *gpImgGemSocket;
static_assert(sizeof(gpImgGemSocket) == 4, "gpImgGemSocket: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E38
//  Four palette indices, seeded by PANEL_CacheHighlightColours with half-intensity red, green, blue and olive. The belt box draw, which
//  highlights the hovered box behind gbBeltHoverActive, overwrites slot 0 on six branches with a MixRGB of its own before use, so the seeded
//  value survives only until the first of them runs.
inline uint8_t gnPanelHighlightColours[4];
static_assert(sizeof(gnPanelHighlightColours) == 4, "gnPanelHighlightColours: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E3C
//  The panel image Panel\800BorderFrame, one of three the 800x600 border frame needs. PANEL_Draw800BorderFrame loads it on demand; that function
//  and PANEL_UnloadImages release it and null it.
inline D2CellFileStrc *gPanel800BorderImage;
static_assert(sizeof(gPanel800BorderImage) == 4, "gPanel800BorderImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E40
//  The panel image Panel\hostilepic. Loaded by PANEL_LoadImages, released and nulled by PANEL_UnloadImages.
inline D2CellFileStrc *gpImgHostilePic;
static_assert(sizeof(gpImgHostilePic) == 4, "gpImgHostilePic: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E44
//  The panel image Panel\CtrlPnl7, one of three the 800x600 border frame needs. PANEL_Draw800BorderFrame loads it on demand; that function and
//  PANEL_UnloadImages release it and null it.
inline D2CellFileStrc *gPanelCtrlPnl7Image;
static_assert(sizeof(gPanelCtrlPnl7Image) == 4, "gPanelCtrlPnl7Image: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E48
//  The panel image Panel\runbutton. Loaded by PANEL_LoadImages, released and nulled by PANEL_UnloadImages; read by PANEL_Draw800BorderFrame (the
//  run/walk toggle).
inline D2CellFileStrc *gpImgRunButton;
static_assert(sizeof(gpImgRunButton) == 4, "gpImgRunButton: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E4C
//  The panel image Panel\menupanel, the panel that the button in gpImgMiniPanelToggle (Panel\menubutton) opens. Loaded by PANEL_LoadImages,
//  released and nulled by PANEL_UnloadImages.
inline D2CellFileStrc *gpImgMenuPanel;
static_assert(sizeof(gpImgMenuPanel) == 4, "gpImgMenuPanel: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E50
//  The panel image Panel\skillpoints. Loaded by PANEL_LoadImages, released and nulled by PANEL_UnloadImages; read by CHAR_CharPanelDraw, where
//  unspent skill points are shown.
inline D2CellFileStrc *gpImgSkillPoints;
static_assert(sizeof(gpImgSkillPoints) == 4, "gpImgSkillPoints: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E54
//  The panel image Panel\800CtrlPnl7, one of three the 800x600 border frame needs. PANEL_Draw800BorderFrame loads it on demand; that function and
//  PANEL_UnloadImages release it and null it.
inline D2CellFileStrc *gPanel800CtrlPnl7Image;
static_assert(sizeof(gPanel800CtrlPnl7Image) == 4, "gPanel800CtrlPnl7Image: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E58
//  The cel holding all fifteen quantity-warning frames plus the no-ammo icon. Loaded by PANEL_LoadImages, replaced by PANEL_UnloadImages; read by
//  PANEL_DrawQuantityWarningIcons -- once per icon, into the DC6 draw block's second field -- and by PANEL_UnloadImages, to release it.
inline D2CellFileStrc *ghWarnIconCel;
static_assert(sizeof(ghWarnIconCel) == 4, "ghWarnIconCel: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E5C
//  Three 16-byte records, one per globe (life, mana, stamina in that order), used by PANEL_GetSmoothedStatValue to animate the fill; it reads
//  and writes all four dwords of a record, and seeds the fields at +0x00 and +0x08 with the true stat value while GAME_GetFrameCount() <= 1.
//  PANEL_LoadImages and PANEL_UnloadImages each zero all 48 bytes.
inline int32_t gOrbAnimState[12];
static_assert(sizeof(gOrbAnimState) == 48, "gOrbAnimState: the item's 48 bytes in the .i64");

//  D2Client 0x6FBB5E8C
//  The cover cel drawn over both globes after their liquid, with the same frame convention (0 life, 1 mana). One TEXTURE_CelDraw each, at
//  gnScreenHeight - 5 for the health globe and - 9 for the mana one.
inline D2CellFileStrc *gpImgGlobeOverlay;
static_assert(sizeof(gpImgGlobeOverlay) == 4, "gpImgGlobeOverlay: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E90
//  Set to 1 by PANEL_GameOnLButtonDown when the press lands in the left mini-skill button (x 117..165 on the control bar) and cleared to 0 by
//  PANEL_GameOnLButtonUp before any hit test. Nothing reads it.
inline BOOL gbMiniSkillLeftBtnDownUnread;
static_assert(sizeof(gbMiniSkillLeftBtnDownUnread) == 4, "gbMiniSkillLeftBtnDownUnread: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5E94
//  The mirror of gbMiniSkillLeftBtnDownUnread: set to 1 by PANEL_GameOnLButtonDown when the press lands in the right mini-skill button
//  (x gnScreenWidth-165..gnScreenWidth-117), cleared by PANEL_GameOnLButtonUp. Nothing reads it.
inline BOOL gbMiniSkillRightBtnDownUnread;
static_assert(sizeof(gbMiniSkillRightBtnDownUnread) == 4, "gbMiniSkillRightBtnDownUnread: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5EA0
//  Non-zero while the cursor is over a belt box that can be acted on. PANEL_BeltUpdateHoveredItem sets it for a box holding an item AND for any
//  box while the cursor is carrying one, and clears it for an empty box; PANEL_GameOnMouseMove, PANEL_GameOnRButtonDown and one more function
//  also write it, so it is the belt's interaction flag rather than a pure hit-test result.
inline BOOL gbBeltHoverActive;
static_assert(sizeof(gbBeltHoverActive) == 4, "gbBeltHoverActive: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5EA4
//  The belt panel is being drawn. PANEL_DrawBelt clears it when UI window 0x1F is closed and gbBeltPanelHovered is clear, and when UI window 9
//  is open; when it is zero PANEL_DrawBelt skips the frame block but the draw goes on. PANEL_GameOnMouseMove clears it with gbBeltPanelHovered
//  when the cursor leaves, only while UI window 0x1F is closed, and sets both when it enters the belt strip. PANEL_BeltShowPanel is the
//  one-instruction setter.
inline BOOL gbBeltPanelVisible;
static_assert(sizeof(gbBeltPanelVisible) == 4, "gbBeltPanelVisible: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5EA8
//  The cursor is over the belt strip. Written only by PANEL_GameOnMouseMove -- set with gbBeltPanelVisible when the cursor enters, cleared with it
//  when UI window 0x1F is closed and alone only while that window is open. Read by PANEL_DrawBelt, which clears gbBeltPanelVisible when UI window
//  0x1F is closed and this is clear, so a hover holds the panel open, and by PANEL_DrawBeltItemHint, which tests the pair together.
inline BOOL gbBeltPanelHovered;
static_assert(sizeof(gbBeltPanelHovered) == 4, "gbBeltPanelHovered: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5EAC
inline int32_t gnHintPhaseSuppressed;
static_assert(sizeof(gnHintPhaseSuppressed) == 4, "gnHintPhaseSuppressed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5EB0
//  A left-button press is in flight on the game screen. PANEL_GameOnLButtonDown sets it; PANEL_GameOnLButtonUp uses it as the gate on its whole
//  first half -- the belt, the run/walk toggle, the mini skill selector and the mini panel -- and clears it on each branch that consumes the
//  click. It means "was the press ours to finish": a drag that began inside a panel does not toggle the mini panel when it ends over the game
//  screen.
inline BOOL gbGameLButtonDown;
static_assert(sizeof(gbGameLButtonDown) == 4, "gbGameLButtonDown: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5EB4
//  The item under the cursor in the belt, or null; the belt hint is drawn for it.
inline D2UnitStrc *gpBeltHoverItem;
static_assert(sizeof(gpBeltHoverItem) == 4, "gpBeltHoverItem: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5EB8
//  The item the hint anchor was last computed for. PANEL_BeltUpdateHoveredItem compares against it and recomputes only on a change, so moving the
//  cursor within one box does no work.
inline D2UnitStrc *gpBeltHoverItemLast;
static_assert(sizeof(gpBeltHoverItemLast) == 4, "gpBeltHoverItemLast: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5EBC
//  One byte per belt column, 1 when that column holds an item. Written only through PANEL_BeltSetColumnHasItem, read only by
//  PANEL_BeltUseColumn0..3, each of which checks its own byte and does nothing when it is clear.
inline char gbBeltColumnHasItem[4];
static_assert(sizeof(gbBeltColumnHasItem) == 4, "gbBeltColumnHasItem: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5EC0
//  Six bytes, one per warning-icon position, cleared at the top of PANEL_DrawQuantityWarningIcons. Each icon claims the first zero byte and marks
//  it, and draws at gnScreenWidth - 55 - 40 * slot. When all six are taken the search runs off the end, the mark is skipped, and the icon is drawn
//  at slot 0's x, so a seventh warning overlaps rather than disappearing.
inline uint8_t gaWarnIconSlotUsed[6];
static_assert(sizeof(gaWarnIconSlotUsed) == 6, "gaWarnIconSlotUsed: the item's 6 bytes in the .i64");

//  D2Client 0x6FBB5EDC
//  Set by PANEL_GameOnLButtonDown when the press lands inside the mini-panel toggle button's rect, cleared at the end of PANEL_GameOnLButtonUp.
//  PANEL_DrawMiniPanelToggleButton requires it AND a current hover before it draws a pressed frame, so it is the "the press started here" half
//  of the test rather than the hover itself.
inline BOOL gbMiniPanelTogglePressed;
static_assert(sizeof(gbMiniPanelTogglePressed) == 4, "gbMiniPanelTogglePressed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5EE0
//  Flipped by PANEL_GameOnLButtonUp and by nothing else, under `if (gbMiniPanelTogglePressed)`, on the release of the mini-panel toggle at the
//  centre of the control bar: it counts presses of that button modulo two. Its only reader is its own flip, so no behaviour depends on it.
inline BOOL gbMiniPanelToggleParityUnread;
static_assert(sizeof(gbMiniPanelToggleParityUnread) == 4, "gbMiniPanelToggleParityUnread: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5EE4
//  The run/walk button on the control bar is held down. PANEL_GameOnLButtonDown sets it inside that button's rect and PANEL_GameOnLButtonUp clears
//  it. PANEL_Draw800BorderFrame combines it with the hover test to pick the cel frame: running -> down && over ? 3 : 2; walking ->
//  down && over ? 1 : 0. The frame carries two bits: the toggle's state (GAME_GetRunToggled) and whether the button is being pressed now.
inline BOOL gbRunButtonDown;
static_assert(sizeof(gbRunButtonDown) == 4, "gbRunButtonDown: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5EE8
//  The three palette indices PANEL_DrawStaminaBar picks between: [0] red for an empty bar, [1] amber normally, [2] blue when over-full or under
//  state mask 24; [3] is never written. PANEL_DrawStaminaBar seeds all three lazily on first use, guarded on [0] == 0; UI_CachePaletteIndices
//  refreshes them through the three PANEL_Cache* setters whenever the act palette changes.
inline uint8_t gnStaminaBarColours[4];
static_assert(sizeof(gnStaminaBarColours) == 4, "gnStaminaBarColours: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5EF0
//  The level id PANEL_ArmLevelChangeTimer last saw, so that the timer beside it is armed once per level rather than every frame. Read and
//  written by that function only.
inline int32_t gnPanelLastLevelId;
static_assert(sizeof(gnPanelLastLevelId) == 4, "gnPanelLastLevelId: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5EF4
//  Set to 60 by PANEL_ArmLevelChangeTimer when the player changes level; written once, never read or decremented.
inline int32_t gnPanelLevelChangeTimer;
static_assert(sizeof(gnPanelLevelChangeTimer) == 4, "gnPanelLevelChangeTimer: the item's 4 bytes in the .i64");

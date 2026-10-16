//  spellsel.h -- the globals of spellsel.cpp that are ours: the spinner's two vtables, the class skill icon table, the panel and hotkey-slot state
//  of the skill-select popup, the bound-key label buffer and the shared icon sheet.
//  The vtables are at 0x6FB6FBAC and 0x6FB6FBE0; the rest lie in 0x6FB8D0C8..0x6FBB9D78 (.data; the panel and the hotkey slots are past .data's raw
//  data, so they are zero at load). Each is a C++17 inline variable, one object for every file that includes this header. The comment line
//  "D2Client 0x6F..." above a global gives its address in the original.
//  Include it after D2Client.h, which declares every function named here.
#pragma once

//  D2Client 0x6FB6FBAC
//  The spinner control body's vtable, 13 slots, installed by SPELLSEL_UiSpinnerConstruct and SPELLSEL_UiSpinnerDestruct: 0
//  SPELLSEL_UiSpinnerVecDelDtor, 1 SPELLSEL_UiSpinnerDraw, 2 SPELLSEL_UiSpinnerOnMouseWheel, 3 SPELLSEL_UiSpinnerOnMouseDown, 4
//  SPELLSEL_UiSpinnerOnMouseUp, 5..10 EDITBOX_DialogNullCallback, 11 SPELLSEL_UiSpinnerGetPressedArrow, 12 SPELLSEL_UiSpinnerTickAutoRepeat. Its
//  artwork path `ui\menu\spinner` names the class. The 13 dwords after it are gVtblUiSpinnerProxy, a separate table.
//  The slots are typed by their functions: the original's `__thiscall` slots are spelled `__fastcall` with nEdxUnused.
inline const D2UiSpinnerImplVtbl gVtblUiSpinnerImpl =
{
    .pfnDeletingDtor = &SPELLSEL_UiSpinnerVecDelDtor,
    .pfnDraw = &SPELLSEL_UiSpinnerDraw,
    .pfnOnMouseWheel = &SPELLSEL_UiSpinnerOnMouseWheel,
    .pfnOnMouseDown = &SPELLSEL_UiSpinnerOnMouseDown,
    .pfnOnMouseUp = &SPELLSEL_UiSpinnerOnMouseUp,
    .pfnOnMouseMove = &EDITBOX_DialogNullCallback,
    .pfnOnChar = &EDITBOX_DialogNullCallback,
    .pfnOnArrowUp = &EDITBOX_DialogNullCallback,
    .pfnOnArrowDown = &EDITBOX_DialogNullCallback,
    .pfnOnArrowRight = &EDITBOX_DialogNullCallback,
    .pfnOnArrowLeft = &EDITBOX_DialogNullCallback,
    .pfnGetValue = &SPELLSEL_UiSpinnerGetPressedArrow,
    .pfnTickAutoRepeat = &SPELLSEL_UiSpinnerTickAutoRepeat,
};
static_assert(sizeof(gVtblUiSpinnerImpl) == 52, "gVtblUiSpinnerImpl: the item's 52 bytes in the .i64");

//  D2Client 0x6FB6FBE0
//  The spinner proxy's vtable, 13 slots, installed by SPELLSEL_UiSpinnerProxyConstruct and SPELLSEL_UiSpinnerProxyDestruct. Slot 0 is the proxy's
//  scalar-deleting destructor; the rest forward to the wrapped control's slots. Slot 12 is SPELLSEL_UiCtrlProxyFwdSlot12Arg, the call/retn-4 form:
//  it calls the body's slot 12, SPELLSEL_UiSpinnerTickAutoRepeat, with ecx alone and drops its own argument.
inline const D2UiSpinnerProxyVtbl gVtblUiSpinnerProxy =
{
    .pfnDeletingDtor = &SPELLSEL_UiSpinnerProxyVecDelDtor,
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
    .pfnTickAutoRepeat = &SPELLSEL_UiCtrlProxyFwdSlot12Arg,
};
static_assert(sizeof(gVtblUiSpinnerProxy) == 52, "gVtblUiSpinnerProxy: the item's 52 bytes in the .i64");

//  D2Client 0x6FB8D0C8
//  The per-class skill icon table: 7 records of 34 bytes, one per character class in class-id order (Amazon, Sorceress, Necromancer, Paladin,
//  Barbarian, Druid, Assassin). Each record is { D2CellFileStrc *hImage; char szFile[30]; }: the handle at +0 and the file name at +4. Sheets are
//  loaded on demand by SPELLSEL_GetSkillIconImage and freed by SPELLSEL_Shutdown.
inline D2ClientClassSkillIconStrc gClassSkillIcons[7] =
{
    { .szFile = "Spells\\AmSkillicon" },   // [0]
    { .szFile = "Spells\\SoSkillicon" },   // [1]
    { .szFile = "Spells\\NeSkillicon" },   // [2]
    { .szFile = "Spells\\PaSkillicon" },   // [3]
    { .szFile = "Spells\\BaSkillicon" },   // [4]
    { .szFile = "Spells\\DrSkillicon" },   // [5]
    { .szFile = "Spells\\AsSkillicon" },   // [6]
};
static_assert(sizeof(gClassSkillIcons) == 238, "gClassSkillIcons: the item's 238 bytes in the .i64");

//  D2Client 0x6FBB9858
//  A 0/1 discriminator per skill hotkey slot (16 ints), parallel to gnSpellSelSkillIds and gnSpellSelOwnerGUIDs; very likely which mouse button the
//  hotkey casts on, but which value is which button is not established. SPELLSEL_Init sets it to -1, so no slot matches either mode until something
//  sets it, and SPELLSEL_UnbindEmptyScrollHotkeys stores -1 too. SPELLSEL_BindHoveredSkillToSlot, SPELLSEL_SetHotkeySlot and
//  SPELLSEL_UnbindEmptyScrollHotkeys write it; SPELLSEL_SkillsSelectHotkeySlot, SPELLSEL_CycleSkill and SPELLSEL_BindHoveredSkillToSlot read it.
//  SPELLSEL_DrawHotkeyLabelMode1 and SPELLSEL_DrawHotkeyLabelMode0 draw a slot's key label only when it matches the panel being drawn.
inline int32_t gnSpellSelHotkeyMode[16];
static_assert(sizeof(gnSpellSelHotkeyMode) == 64, "gnSpellSelHotkeyMode: the item's 64 bytes in the .i64");

//  D2Client 0x6FBB9898
//  The skill-select popup's panel state, one packed 965-byte struct that SPELLSEL_Init clears as a block (`rep stosd` of 241 dwords and one
//  `stosb`): four sixty-entry parallel arrays -- nX and nY (the position of each laid-out skill icon), nSkillId and OwnerGUID (the unit that grants
//  it) -- then the byte count nCount at +0x3C0 and nLastTextColor, the DWORD that SPELLSEL_DrawPanelSkills stores one byte past the count (+0x3C1).
//  SPELLSEL_Init also sets OwnerGUID to -1. Sixty is the panel's capacity, not a class's skill count.
//  The original keeps the count in a 32-bit variable whose low byte alone is used (the reads mask with 0xFF, the update is `inc cl`).
inline D2SpellSelPanelStrc gSpellSelPanel;
static_assert(sizeof(gSpellSelPanel) == 965, "gSpellSelPanel: the item's 965 bytes in the .i64");

//  D2Client 0x6FBB9C60
//  int[4], write-only: SPELLSEL_DrawPanelSkills stores the row's Y into one of the four, chosen by a jump table (the compiler unrolled `arr[row -
//  1] = y` into four stores). Nothing reads them. They are not part of the 965-byte block SPELLSEL_Init clears.
inline int32_t gnSpellSelRowY[4];
static_assert(sizeof(gnSpellSelRowY) == 16, "gnSpellSelRowY: the item's 16 bytes in the .i64");

//  D2Client 0x6FBB9C78
//  int[16], a fourth parallel hotkey-slot array that nothing reads: SPELLSEL_Init fills it with -1 beside gnSpellSelHotkeyMode,
//  gnSpellSelOwnerGUIDs and gnSpellSelSkillIds, and that clear is its one reference. What it holds is unknown.
inline int32_t gnSpellSelSlotsUnread[16];
static_assert(sizeof(gnSpellSelSlotsUnread) == 64, "gnSpellSelSlotsUnread: the item's 64 bytes in the .i64");

//  D2Client 0x6FBB9CB8
//  int[4], write-only: its only writer is the last four instructions of SPELLSEL_Init, which zero it (four stores rather than a `rep stosd`).
//  Nothing reads it.
inline int32_t gnSpellSelInitTailUnread[4];
static_assert(sizeof(gnSpellSelInitTailUnread) == 16, "gnSpellSelInitTailUnread: the item's 16 bytes in the .i64");

//  D2Client 0x6FBB9CC8
//  A 16 wide-character scratch buffer that SPELLSEL_ControlsDrawBoundKeyLabel builds the bound-key text in. It is constructed element by element on
//  first use, guarded by bit 0 of gnBoundKeyLabelGuard.
inline Unicode gwszBoundKeyLabel[16];
static_assert(sizeof(gwszBoundKeyLabel) == 32, "gwszBoundKeyLabel: the item's 32 bytes in the .i64");

//  D2Client 0x6FBB9CE8
//  The selected hotkey slot, an index into the parallel 16-slot arrays, or -1 for none (the value SPELLSEL_Init leaves in all four of the slot
//  arrays). SPELLSEL_CycleSkill reads it to start from the current slot and writes the slot it lands on; SPELLSEL_SkillsSelectHotkeySlot writes it.
inline int32_t gnSpellSelHotkeySlot;
static_assert(sizeof(gnSpellSelHotkeySlot) == 4, "gnSpellSelHotkeySlot: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9CF0
//  int[16], the spell selector's ring of skill ids, -1 for an empty slot (SPELLSEL_Init sets it with a `rep stosd` over 0x40 bytes).
//  SPELLSEL_CycleSkill walks it with an explicit wrap at 16 and 0. Parallel to gnSpellSelOwnerGUIDs, which follows it.
inline int32_t gnSpellSelSkillIds[16];
static_assert(sizeof(gnSpellSelSkillIds) == 64, "gnSpellSelSkillIds: the item's 64 bytes in the .i64");

//  D2Client 0x6FBB9D30
//  D2UnitGUID[16], the unit GUID that granted each entry of gnSpellSelSkillIds, -1 when empty. SPELLSEL_CycleSkill reads it alongside the skill id,
//  so a skill granted by an item is tracked with the item that grants it.
inline D2UnitGUID gnSpellSelOwnerGUIDs[16];
static_assert(sizeof(gnSpellSelOwnerGUIDs) == 64, "gnSpellSelOwnerGUIDs: the item's 64 bytes in the .i64");

//  D2Client 0x6FBB9D70
//  Bit 0 is set once gwszBoundKeyLabel is constructed. SPELLSEL_ControlsDrawBoundKeyLabel is its only writer and its only reader.
inline uint8_t gnBoundKeyLabelGuard;
static_assert(sizeof(gnBoundKeyLabelGuard) == 1, "gnBoundKeyLabelGuard: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB9D74
//  The shared (class-independent) skill icon sheet, loaded by SPELLSEL_Init from `Spells\Skillicon` and freed by SPELLSEL_Shutdown.
//  SPELLSEL_GetSkillIconImage falls back to it for a skill that belongs to no character class.
inline D2CellFileStrc *ghSkillIconImage;
static_assert(sizeof(ghSkillIconImage) == 4, "ghSkillIconImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9D78
//  0 or 1, set by SPELLSEL_SetMiniSkillMode when UI_MINISKILL (UI window 3) is toggled on and cleared to 0 by SPELLSEL_MiniSkillOnLButtonDown; very
//  likely which of the two skill buttons the selector was opened for, but that mapping is not established. SPELLSEL_DrawBothPanels,
//  SPELLSEL_MiniSkillOnLButtonDown and SPELLSEL_BindHoveredSkillToSlot read it.
inline int32_t gnMiniSkillMode;
static_assert(sizeof(gnMiniSkillMode) == 4, "gnMiniSkillMode: the item's 4 bytes in the .i64");

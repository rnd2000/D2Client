//  inv.h -- the globals of inv.cpp (the inventory and the panels beside it: NPC shop, trade, stash, Horadric cube, mercenary, level-up buttons).
//  Each is a C++17 inline variable, one object for every file that includes this header, initialised with the value the original's image holds.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. gSocketLayouts and gMercPanelLabels live in .rdata
//  and are const. The globals at 0x6FBB36D8..0x6FBB5BC0 lie past the end of .data's raw data (0x6FB9B000): the loader zero-fills them,
//  they have no value in the file, and ours are zero-initialised.
//  Include it after D2Client.h, which declares every function named here.
#pragma once

//  D2Client 0x6FB792AC
//  Click handlers for the equipment slots, indexed directly by D2C_PlayerBodyLocs; entry 0 (BODYLOC_NONE) is null.
//  [4] BODYLOC_RARM is INV_OnBodyLocClickRightArm, [5] BODYLOC_LARM is INV_OnBodyLocClickLeftArm, every other slot is INV_OnBodyLocClickGeneric.
//  The three handlers are reached only through this table.
inline int32_t (__fastcall *gBodyLocClickHandlers[11])(D2UnitStrc *, D2InventoryStrc *, D2C_PlayerBodyLocs) =
{
    NULL,   //  BODYLOC_NONE
    &INV_OnBodyLocClickGeneric,
    &INV_OnBodyLocClickGeneric,
    &INV_OnBodyLocClickGeneric,
    &INV_OnBodyLocClickRightArm,
    &INV_OnBodyLocClickLeftArm,
    &INV_OnBodyLocClickGeneric,
    &INV_OnBodyLocClickGeneric,
    &INV_OnBodyLocClickGeneric,
    &INV_OnBodyLocClickGeneric,
    &INV_OnBodyLocClickGeneric,
};
static_assert(sizeof(gBodyLocClickHandlers) == 44, "gBodyLocClickHandlers: the item's 44 bytes in the .i64");

//  D2Client 0x6FB6EFD0
//  Where each socket sits inside an item, as (x, y) cell coordinates of 8 bytes per socket. INV_DrawSocketedItems indexes it as
//      &gSocketLayouts + 384*nSockets + 192*nWidth + 48*nHeight - 624
//  with nSockets 1..6, nWidth 1..2 and nHeight 1..4 (the -624 is the 1-based origin), 6 sockets at most per entry: 2304 bytes in all.
//  The coordinates are grid cells, scaled by the panel's cell size and centred before the draw. nWidth is limited to 2, so no three-wide item
//  can carry sockets.
inline const int32_t gSocketLayouts[576] =
{
    1,
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    2,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    3,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    2,
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    2,
    2,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    2,
    3,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    2,
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    1,
    3,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    2,
    1,
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    3,
    1,
    5,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    3,
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    2,
    1,
    2,
    3,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    2,
    2,
    2,
    4,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    2,
    2,
    2,
    6,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    1,
    3,
    1,
    5,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    2,
    1,
    4,
    1,
    6,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    3,
    1,
    2,
    3,
    0,
    0,
    0,
    0,
    0,
    0,
    2,
    1,
    2,
    3,
    2,
    5,
    0,
    0,
    0,
    0,
    0,
    0,
    2,
    2,
    2,
    4,
    2,
    6,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    1,
    3,
    1,
    5,
    1,
    7,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    3,
    3,
    1,
    3,
    3,
    1,
    0,
    0,
    0,
    0,
    1,
    2,
    3,
    4,
    1,
    4,
    3,
    2,
    0,
    0,
    0,
    0,
    2,
    1,
    2,
    3,
    2,
    5,
    2,
    7,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    2,
    3,
    1,
    1,
    3,
    5,
    1,
    5,
    3,
    1,
    0,
    0,
    2,
    4,
    1,
    2,
    3,
    6,
    1,
    6,
    3,
    2,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    1,
    3,
    1,
    5,
    3,
    1,
    3,
    3,
    3,
    5,
    1,
    2,
    1,
    4,
    1,
    6,
    3,
    2,
    3,
    4,
    3,
    6,
};
static_assert(sizeof(gSocketLayouts) == 2304, "gSocketLayouts: the item's 2304 bytes in the .i64");

//  D2Client 0x6FB6F8D0
//  The mercenary panel's 12 captions, 14 bytes each, walked once per frame by INV_MercInvDrawPanel. nLeft is offset by gnBorderXDifference and
//  nY by -gnBorderYDifference; nStringId is a tbl string id (a word). nRight is the right edge a caption is centred against, 0 for none;
//  the draw tests the sum gnBorderXDifference + nRight, so the border offset takes part in the choice of draw path.
//  Only the four resistance captions (tbl 4071..4074) have a non-zero nRight (245): their strings hold a newline, CHAR_TextCutNextLine splits
//  them and the two halves are drawn at nY - 4 and nY + 4.
inline const D2MercPanelLabelStrc gMercPanelLabels[12] =
{
    { .nLeft = 15, .nY = 236, .nStringId = STR_IDX_4058_strchrexp },   // [0]
    { .nLeft = 145, .nY = 236, .nStringId = STR_IDX_4057_strchrlvl },   // [1]
    { .nLeft = 200, .nY = 236, .nStringId = STR_IDX_4059_strchrnxtlvl },   // [2]
    { .nLeft = 15, .nY = 281, .nStringId = STR_IDX_4060_strchrstr },   // [3]
    { .nLeft = 15, .nY = 305, .nStringId = STR_IDX_4062_strchrdex },   // [4]
    { .nLeft = 15, .nY = 329, .nStringId = STR_IDX_4061_strchrskm },   // [5]
    { .nLeft = 15, .nY = 353, .nStringId = STR_IDX_4064_strchrdef },   // [6]
    { .nLeft = 180, .nRight = 245, .nY = 281, .nStringId = STR_IDX_4071_strchrfir },   // [7]
    { .nLeft = 180, .nRight = 245, .nY = 305, .nStringId = STR_IDX_4072_strchrcol },   // [8]
    { .nLeft = 180, .nRight = 245, .nY = 329, .nStringId = STR_IDX_4073_strchrlit },   // [9]
    { .nLeft = 180, .nRight = 245, .nY = 353, .nStringId = STR_IDX_4074_strchrpos },   // [10]
    { .nLeft = 180, .nY = 214, .nStringId = STR_IDX_4068_strchrlif },   // [11]
};
static_assert(sizeof(gMercPanelLabels) == 168, "gMercPanelLabels: the item's 168 bytes in the .i64");

//  D2Client 0x6FB79288
//  Gate on the inventory's hover descriptions: INV_OnMouseMove returns at once when it is 0. INV_SetHoverEnabled is the only writer;
//  DIALOG_Create and DIALOG_CreateFromImage pass 0 and DIALOG_Close passes 1, so a modal dialog suppresses the tooltips while it is up.
inline BOOL gbInvHoverEnabled = 1;
static_assert(sizeof(gbInvHoverEnabled) == 4, "gbInvHoverEnabled: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7928C
//  NPC trade sub-mode; 0 is a plain vendor. INV_OnMouseMove sets it to 1 on five paths and INV_NpcShopApplyTabSelection resets it to 0;
//  INV_DrawItemDescription and INV_AppendPriceFooterText read it to decide whether to show vendor pricing.
inline BOOL gnNpcTradeMode = 1;
static_assert(sizeof(gnNpcTradeMode) == 4, "gnNpcTradeMode: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79290
//  Popup anchor X.
inline int32_t gnDescPopupX = -1;
static_assert(sizeof(gnDescPopupX) == 4, "gnDescPopupX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79294
//  Popup anchor Y; the popup moves when the text is as tall as this or taller.
inline int32_t gnDescPopupY = -1;
static_assert(sizeof(gnDescPopupY) == 4, "gnDescPopupY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79298
//  Fallback anchor X, used when the text does not fit.
inline int32_t gnDescPopupAltX = -1;
static_assert(sizeof(gnDescPopupAltX) == 4, "gnDescPopupAltX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7929C
//  Fallback anchor Y; the text is drawn at this plus its height.
inline int32_t gnDescPopupBottomY = -1;
static_assert(sizeof(gnDescPopupBottomY) == 4, "gnDescPopupBottomY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB792A0
//  The inventory grid cell the mouse is over, X; -1 when nothing is hovered. INV_PixelToGridCell computes it and INV_ClearHoverForItem
//  resets it together with gnDescPopupX/Y, gbShowItemDesc and gpHoverItem.
inline int32_t gnHoverCellX = -1;
static_assert(sizeof(gnHoverCellX) == 4, "gnHoverCellX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB792A4
//  The inventory grid cell the mouse is over, Y; -1 when nothing is hovered. Written and cleared wherever gnHoverCellX is.
inline int32_t gnHoverCellY = -1;
static_assert(sizeof(gnHoverCellY) == 4, "gnHoverCellY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB792A8
//  GUID of the unit whose Horadric cube panel is open. Written once, by pfnUnitFunction; read by the cube's three exit paths
//  (INV_CubeOnLButtonUp, INV_CubeOnDismissKey, INV_CubeOnPanelClose), which pass it to UNITMODE_InteractWithUnit to end the interaction.
inline int32_t gnCubeUnitGUID = -1;
static_assert(sizeof(gnCubeUnitGUID) == 4, "gnCubeUnitGUID: the item's 4 bytes in the .i64");

//  D2Client 0x6FB792D8
//  Item type -> the weapon class line of an item description: 15 rows of {item type, tbl string id}. Staff, Orb and Wand share 4085
//  (Staff Class) and both hand-to-hand types share 21258 (Claw Class).
inline D2WeaponDescStrc gWeaponClassDescs[15] =
{
    { .nKey = 26, .nStringId = 4085 },   // [0]
    { .nKey = 28, .nStringId = 4078 },   // [1]
    { .nKey = 30, .nStringId = 4079 },   // [2]
    { .nKey = 32, .nStringId = 4080 },   // [3]
    { .nKey = 38, .nStringId = 4081 },   // [4]
    { .nKey = 44, .nStringId = 4082 },   // [5]
    { .nKey = 33, .nStringId = 4083 },   // [6]
    { .nKey = 27, .nStringId = 4084 },   // [7]
    { .nKey = 34, .nStringId = 4086 },   // [8]
    { .nKey = 35, .nStringId = 4087 },   // [9]
    { .nKey = 67, .nStringId = 21258 },   // [10]
    { .nKey = 88, .nStringId = 21258 },   // [11]
    { .nKey = 68, .nStringId = 4085 },   // [12]
    { .nKey = 25, .nStringId = 4085 },   // [13]
    { .nKey = 57, .nStringId = 4077 },   // [14]
};
static_assert(sizeof(gWeaponClassDescs) == 90, "gWeaponClassDescs: the item's 90 bytes in the .i64");

//  D2Client 0x6FB79334
//  Speed class 0..6 -> the attack speed line of an item description, tbl 4088 (Fastest Attack Speed) through 4094 (Slowest Attack Speed),
//  with Normal Attack Speed at 3. The row comes from gptAttackSpeedMatrix for weapon speeds 10..27 only: 28 and up is row 5, below 10 row 1.
inline D2WeaponDescStrc gWeaponSpeedDescs[7] =
{
    { .nStringId = 4088 },   // [0]
    { .nKey = 1, .nStringId = 4089 },   // [1]
    { .nKey = 2, .nStringId = 4090 },   // [2]
    { .nKey = 3, .nStringId = 4091 },   // [3]
    { .nKey = 4, .nStringId = 4092 },   // [4]
    { .nKey = 5, .nStringId = 4093 },   // [5]
    { .nKey = 6, .nStringId = 4094 },   // [6]
};
static_assert(sizeof(gWeaponSpeedDescs) == 42, "gWeaponSpeedDescs: the item's 42 bytes in the .i64");

//  D2Client 0x6FB79360
//  Weapon speed (18 rows, speeds 10..27) x 5 columns -> an index into gWeaponSpeedDescs. The columns are not character classes: the read is
//  flat, 5 * (speed - 10) plus a column offset 0..4 from gptAttackSpeedClassOffsets.
inline int32_t gptAttackSpeedMatrix[18][5] =
{
    { 1, 1, 1, 1, 1 },   // [0]
    { 1, 1, 1, 1, 1 },   // [1]
    { 1, 1, 1, 1, 1 },   // [2]
    { 1, 1, 2, 1, 1 },   // [3]
    { 2, 1, 2, 2, 1 },   // [4]
    { 2, 1, 2, 2, 2 },   // [5]
    { 2, 2, 3, 2, 2 },   // [6]
    { 3, 2, 3, 3, 2 },   // [7]
    { 3, 2, 3, 3, 3 },   // [8]
    { 3, 2, 4, 3, 3 },   // [9]
    { 4, 3, 4, 4, 3 },   // [10]
    { 4, 3, 4, 4, 4 },   // [11]
    { 4, 3, 5, 4, 4 },   // [12]
    { 5, 4, 5, 5, 4 },   // [13]
    { 5, 4, 5, 5, 5 },   // [14]
    { 5, 4, 5, 5, 5 },   // [15]
    { 5, 5, 5, 5, 5 },   // [16]
    { 5, 5, 5, 5, 5 },   // [17]
};
static_assert(sizeof(gptAttackSpeedMatrix) == 360, "gptAttackSpeedMatrix: the item's 360 bytes in the .i64");

//  D2Client 0x6FB794C8
//  Per-class column offsets (0..4) added to the flat index into gptAttackSpeedMatrix: two entries per class (one-hand, two-hand), selected by
//  whether the item is type 35 or 27. Seven pairs, fourteen values.
inline int32_t gptAttackSpeedClassOffsets[7][2] = { { 0, 2 }, { 1, 4 }, { 1, 4 }, { 0, 3 }, { 0, 3 }, { 1, 4 }, { 0, 3 } };
static_assert(sizeof(gptAttackSpeedClassOffsets) == 56, "gptAttackSpeedClassOffsets: the item's 56 bytes in the .i64");

//  D2Client 0x6FB79500
//  The 24 globals from here to 0x6FB7955C are ten (X, Y) pixel nudge pairs for the placeholders drawn in empty equipment slots (torso, belt,
//  feet, head, gloves, neck, left ring, right ring, right arm, left arm), then two pairs for the weapon-swap tabs. All are read-only. X is
//  added to the left and Y to the bottom of the slot's rect in gInvBodyLocRects. INV_UpdateAndDrawPanels draws each placeholder only when the
//  slot holds no item; INV_MercInvDrawEmptySlots applies the same values to the mercenary panel's slots.
//  This one: X nudge of the empty armour slot (gpImgInvArmor, cel 0), added to gInvBodyLocRects[BODYLOC_TORSO].rect.nLeft.
inline int32_t gnInvTorsoEmptyOffsetX = 2;
static_assert(sizeof(gnInvTorsoEmptyOffsetX) == 4, "gnInvTorsoEmptyOffsetX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79504
//  Y nudge of the empty armour slot, added to gInvBodyLocRects[BODYLOC_TORSO].rect.nBottom.
inline int32_t gnInvTorsoEmptyOffsetY = -2;
static_assert(sizeof(gnInvTorsoEmptyOffsetY) == 4, "gnInvTorsoEmptyOffsetY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79508
//  X nudge of the empty belt slot (gpImgInvBelt, cel 0), added to gInvBodyLocRects[BODYLOC_BELT].rect.nLeft.
inline int32_t gnInvBeltEmptyOffsetX;
static_assert(sizeof(gnInvBeltEmptyOffsetX) == 4, "gnInvBeltEmptyOffsetX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7950C
//  Y nudge of the empty belt slot, added to gInvBodyLocRects[BODYLOC_BELT].rect.nBottom.
inline int32_t gnInvBeltEmptyOffsetY = -1;
static_assert(sizeof(gnInvBeltEmptyOffsetY) == 4, "gnInvBeltEmptyOffsetY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79510
//  X nudge of the empty boots slot (gpImgInvBoots, cel 0), added to gInvBodyLocRects[BODYLOC_FEET].rect.nLeft.
inline int32_t gnInvFeetEmptyOffsetX = -1;
static_assert(sizeof(gnInvFeetEmptyOffsetX) == 4, "gnInvFeetEmptyOffsetX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79514
//  Y nudge of the empty boots slot, added to gInvBodyLocRects[BODYLOC_FEET].rect.nBottom.
inline int32_t gnInvFeetEmptyOffsetY = -4;
static_assert(sizeof(gnInvFeetEmptyOffsetY) == 4, "gnInvFeetEmptyOffsetY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79518
//  X nudge of the empty helm slot (gpImgInvHelmGlove, cel 1; the gloves slot uses cel 0 of the same file), added to
//  gInvBodyLocRects[BODYLOC_HEAD].rect.nLeft.
inline int32_t gnInvHeadEmptyOffsetX;
static_assert(sizeof(gnInvHeadEmptyOffsetX) == 4, "gnInvHeadEmptyOffsetX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7951C
//  Y nudge of the empty helm slot, added to gInvBodyLocRects[BODYLOC_HEAD].rect.nBottom.
inline int32_t gnInvHeadEmptyOffsetY = -1;
static_assert(sizeof(gnInvHeadEmptyOffsetY) == 4, "gnInvHeadEmptyOffsetY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79520
//  X nudge of the empty gloves slot (gpImgInvHelmGlove, cel 0), added to gInvBodyLocRects[BODYLOC_GLOVES].rect.nLeft.
inline int32_t gnInvGlovesEmptyOffsetX = -1;
static_assert(sizeof(gnInvGlovesEmptyOffsetX) == 4, "gnInvGlovesEmptyOffsetX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79524
//  Y nudge of the empty gloves slot, added to gInvBodyLocRects[BODYLOC_GLOVES].rect.nBottom.
inline int32_t gnInvGlovesEmptyOffsetY = -2;
static_assert(sizeof(gnInvGlovesEmptyOffsetY) == 4, "gnInvGlovesEmptyOffsetY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79528
//  X nudge of the empty amulet slot (gpImgInvRingAmulet, cel 0; the two ring slots use cel 1), added to gInvBodyLocRects[BODYLOC_NECK].rect.nLeft.
inline int32_t gnInvNeckEmptyOffsetX;
static_assert(sizeof(gnInvNeckEmptyOffsetX) == 4, "gnInvNeckEmptyOffsetX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7952C
//  Y nudge of the empty amulet slot, added to gInvBodyLocRects[BODYLOC_NECK].rect.nBottom.
inline int32_t gnInvNeckEmptyOffsetY = -1;
static_assert(sizeof(gnInvNeckEmptyOffsetY) == 4, "gnInvNeckEmptyOffsetY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79530
//  X nudge of the empty left-ring slot (gpImgInvRingAmulet, cel 1), added to gInvBodyLocRects[BODYLOC_LRIN].rect.nLeft. In memory the left ring's
//  pair comes first, but INV_UpdateAndDrawPanels draws BODYLOC_RRIN before BODYLOC_LRIN, so it reads the right ring's pair first. Both pairs hold
//  the same values.
inline int32_t gnInvLRingEmptyOffsetX;
static_assert(sizeof(gnInvLRingEmptyOffsetX) == 4, "gnInvLRingEmptyOffsetX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79534
//  Y nudge of the empty left-ring slot, added to gInvBodyLocRects[BODYLOC_LRIN].rect.nBottom.
inline int32_t gnInvLRingEmptyOffsetY = -1;
static_assert(sizeof(gnInvLRingEmptyOffsetY) == 4, "gnInvLRingEmptyOffsetY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79538
//  X nudge of the empty right-ring slot (gpImgInvRingAmulet, cel 1), added to gInvBodyLocRects[BODYLOC_RRIN].rect.nLeft.
inline int32_t gnInvRRingEmptyOffsetX;
static_assert(sizeof(gnInvRRingEmptyOffsetX) == 4, "gnInvRRingEmptyOffsetX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7953C
//  Y nudge of the empty right-ring slot, added to gInvBodyLocRects[BODYLOC_RRIN].rect.nBottom.
inline int32_t gnInvRRingEmptyOffsetY = -1;
static_assert(sizeof(gnInvRRingEmptyOffsetY) == 4, "gnInvRRingEmptyOffsetY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79540
//  X nudge of the empty right-hand weapon slot (gpImgInvWeapons, cel 0), added to gInvBodyLocRects[BODYLOC_RARM].rect.nLeft; in
//  INV_MercInvDrawEmptySlots it applies to gMercInvSlots[4]. Two of its four uses in INV_UpdateAndDrawPanels also add gnWeaponSwapSet's
//  (+4, -4) shift; INV_MercInvDrawEmptySlots does not.
inline int32_t gnInvRArmEmptyOffsetX;
static_assert(sizeof(gnInvRArmEmptyOffsetX) == 4, "gnInvRArmEmptyOffsetX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79544
//  Y nudge of the empty right-hand weapon slot, added to gInvBodyLocRects[BODYLOC_RARM].rect.nBottom.
inline int32_t gnInvRArmEmptyOffsetY = -2;
static_assert(sizeof(gnInvRArmEmptyOffsetY) == 4, "gnInvRArmEmptyOffsetY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79548
//  X nudge of the empty left-hand weapon slot (gpImgInvWeapons, cel 0), added to gInvBodyLocRects[BODYLOC_LARM].rect.nLeft and, in
//  INV_MercInvDrawEmptySlots, to gMercInvSlots[5].
inline int32_t gnInvLArmEmptyOffsetX;
static_assert(sizeof(gnInvLArmEmptyOffsetX) == 4, "gnInvLArmEmptyOffsetX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7954C
//  Y nudge of the empty left-hand weapon slot, added to gInvBodyLocRects[BODYLOC_LARM].rect.nBottom.
inline int32_t gnInvLArmEmptyOffsetY = -2;
static_assert(sizeof(gnInvLArmEmptyOffsetY) == 4, "gnInvLArmEmptyOffsetY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79550
//  X nudge of the weapon-swap tab over the right-arm slot (gpImgInvChar6TabCube, cel 1), added to gInvBodyLocRects[BODYLOC_RARM].rect.nLeft.
//  Unlike the empty-slot nudges this draw does not depend on the slot being empty: INV_UpdateAndDrawPanels guards it with
//  gnWeaponSwapSet && FOG_IsExpansion() && GAME_IsExpansionGame().
inline int32_t gnInvRArmSwapTabOffsetX = -5;
static_assert(sizeof(gnInvRArmSwapTabOffsetX) == 4, "gnInvRArmSwapTabOffsetX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79554
//  Y nudge of the right-arm weapon-swap tab, added to gInvBodyLocRects[BODYLOC_RARM].rect.nBottom.
inline int32_t gnInvRArmSwapTabOffsetY = 4;
static_assert(sizeof(gnInvRArmSwapTabOffsetY) == 4, "gnInvRArmSwapTabOffsetY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79558
//  X nudge of the weapon-swap tab over the left-arm slot (gpImgInvChar6TabCube, cel 0, drawn first of the two), added to
//  gInvBodyLocRects[BODYLOC_LARM].rect.nLeft.
inline int32_t gnInvLArmSwapTabOffsetX = -5;
static_assert(sizeof(gnInvLArmSwapTabOffsetX) == 4, "gnInvLArmSwapTabOffsetX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7955C
//  Y nudge of the left-arm weapon-swap tab, added to gInvBodyLocRects[BODYLOC_LARM].rect.nBottom.
inline int32_t gnInvLArmSwapTabOffsetY = 4;
static_assert(sizeof(gnInvLArmSwapTabOffsetY) == 4, "gnInvLArmSwapTabOffsetY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB79560
//  The vendor page-tab bar (Armor, Weapons, Weapons, Misc): four 18-byte records, drawn by INV_UpdateAndDrawPanels as 80-pixel cels at x += 0x50.
//      nX            the label's centre
//      nY            the label's offset from the panel's top edge: drawn at nY + gnBorderYDifference + gnScreenHeight - 0x1E0
//      nStringIndex  tbl string id of the label, a word; 4037 Weapons appears twice because weapons occupy two inventory pages
//      bSelected     this tab is the open page; INV_GetVendorPageInventory sets exactly one. It picks the text colour (4 when selected, 0
//                    when not) and the cel in gpImgBuySellTabs (slot when selected, slot + 4 when not)
//      bActive       the page holds at least one item; a tab with 0 is drawn neither as cel nor as label, and INV_NpcShopOnLButtonDown
//                    ignores a click on it
//  INV_NpcShopOnLButtonDown hit-tests the four tabs with four unrolled branches and reads each record's bActive by its own address.
inline D2BuySellTabStrc gVendorTabs[4] =
{
    { .nX = 42, .nY = 20, .nStringIndex = STR_IDX_4036_strBSArmor },   // [0]
    { .nX = 121, .nY = 20, .nStringIndex = STR_IDX_4037_strBSWeapons },   // [1]
    { .nX = 201, .nY = 20, .nStringIndex = STR_IDX_4037_strBSWeapons },   // [2]
    { .nX = 281, .nY = 20, .nStringIndex = STR_IDX_4039_strBSMisc },   // [3]
};
static_assert(sizeof(gVendorTabs) == 72, "gVendorTabs: the item's 72 bytes in the .i64");

//  D2Client 0x6FB795A8
//  Number of vendor page-tab records; never written. Bounds the tab-rebuild loop in INV_GetVendorPageInventory and is read by
//  INV_NpcShopBuildTabs and INV_UpdateAndDrawPanels. The item-bucketing array there holds five pages and a page argument of 5 or more is
//  clamped to 0, so the vendor has five pages but only four tabs.
inline int32_t gnVendorTabCount = 4;
static_assert(sizeof(gnVendorTabCount) == 4, "gnVendorTabCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FB795AC
//  The slot of the Close tab. INV_NpcShopBuildTabs writes the literal 3 on the path that sets gnNpcShopTabCount to 4; INV_UpdateAndDrawPanels
//  and both NPCSHOP button handlers read it. With one bar shape in the game it equals gnNpcShopTabCount - 1.
inline int32_t gnNpcShopCloseTab = 3;
static_assert(sizeof(gnNpcShopCloseTab) == 4, "gnNpcShopCloseTab: the item's 4 bytes in the .i64");

//  D2Client 0x6FB795B0
//  How many tabs the open NPC has: 4, or 0 for a monster that is not a vendor. INV_NpcShopBuildTabs is the only writer;
//  INV_NpcShopApplyTabSelection bounds-checks gnNpcShopTabHit against it before indexing.
inline int32_t gnNpcShopTabCount = 4;
static_assert(sizeof(gnNpcShopTabCount) == 4, "gnNpcShopTabCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FB795B8
//  Inventory button X offsets.
inline int32_t gatButtonsOffsetX[4][4] = { { 273 }, { 169, 273 }, { 169, 221, 273 }, { 116, 169, 221, 273 } };
static_assert(sizeof(gatButtonsOffsetX) == 64, "gatButtonsOffsetX: the item's 64 bytes in the .i64");

//  D2Client 0x6FB795F8
//  The mercenary panel's stat layout: 12 rows of 20 bytes, walked once per frame by INV_MercInvDrawStatRows.
//      nLeft      0 means right-align at nX instead of centring (only the life row is non-zero, 210)
//      nX         offset by gnBorderXDifference
//      nY         offset by -gnBorderYDifference - 2
//      nMaxWidth  a row wider than this is redrawn in font 6, one pixel lower; 48 for the eight column rows, 0 (no shrink) for the other four
//      nStatId    the stat the row displays (D2C_ItemStats), passed to STATLIST_UnitGetStatValue
//  The resistances read down the screen as fire, cold, lightning, poison, not in stat-id order. Seven stat ids have an arm of their own
//  in INV_MercInvDrawStatRows: hitpoints, mindamage, armorclass and the four resistances.
inline D2MercStatRowStrc gMercStatRows[12] =
{
    { .nX = 120, .nY = 254, .nStatId = STAT_EXPERIENCE },   // [0]
    { .nX = 175, .nY = 254, .nStatId = STAT_LEVEL },   // [1]
    { .nX = 305, .nY = 254, .nStatId = STAT_NEXTEXP },   // [2]
    { .nX = 154, .nY = 282, .nMaxWidth = 48 },   // [3]
    { .nX = 154, .nY = 306, .nMaxWidth = 48, .nStatId = STAT_DEXTERITY },   // [4]
    { .nX = 154, .nY = 330, .nMaxWidth = 48, .nStatId = STAT_MINDAMAGE },   // [5]
    { .nX = 154, .nY = 354, .nMaxWidth = 48, .nStatId = STAT_ARMORCLASS },   // [6]
    { .nX = 309, .nY = 282, .nMaxWidth = 48, .nStatId = STAT_FIRERESIST },   // [7]
    { .nX = 309, .nY = 306, .nMaxWidth = 48, .nStatId = STAT_COLDRESIST },   // [8]
    { .nX = 309, .nY = 330, .nMaxWidth = 48, .nStatId = STAT_LIGHTRESIST },   // [9]
    { .nX = 309, .nY = 354, .nMaxWidth = 48, .nStatId = STAT_POISONRESIST },   // [10]
    { .nLeft = 210, .nX = 305, .nY = 215, .nStatId = STAT_HITPOINTS },   // [11]
};
static_assert(sizeof(gMercStatRows) == 240, "gMercStatRows: the item's 240 bytes in the .i64");

//  D2Client 0x6FB796FC
//  The literal ")"; see gszAffixOpenParen.
inline char gszAffixCloseParen[2] = ")";
static_assert(sizeof(gszAffixCloseParen) == 2, "gszAffixCloseParen: the item's 2 bytes in the .i64");

//  D2Client 0x6FB79700
//  The literal "(": wraps an affix that has no tbl string, so the raw txt name shows as (name). Used by the magic and rare naming arms of
//  INV_BuildItemNameText.
inline char gszAffixOpenParen[2] = "(";
static_assert(sizeof(gszAffixOpenParen) == 2, "gszAffixOpenParen: the item's 2 bytes in the .i64");

//  D2Client 0x6FB798A0
inline char gszRuneQuoteClose[2] = "'";
static_assert(sizeof(gszRuneQuoteClose) == 2, "gszRuneQuoteClose: the item's 2 bytes in the .i64");

//  D2Client 0x6FB79918
//  The list separator ", ", used by INV_AppendEtherealAndSocketsText.
inline char gszCommaSpace[3] = ", ";
static_assert(sizeof(gszCommaSpace) == 3, "gszCommaSpace: the item's 3 bytes in the .i64");

//  D2Client 0x6FBB36D8
//  Panel rectangle for inventory.txt record 11, Trophy Case Page 1, filled by INV_EnsurePanelLayout. Loaded and never read.
inline D2InvRectStrc gInvTrophyCaseRect;
static_assert(sizeof(gInvTrophyCaseRect) == 16, "gInvTrophyCaseRect: the item's 16 bytes in the .i64");

//  D2Client 0x6FBB36E8
//  The rectangle of the NPC's own inventory panel, from that monster's inventory.txt record. INV_EnsurePanelLayout fills it in its second pass,
//  which runs only while a shop panel is open (sgInventoryMode) and NPCMENU_GetInteractingNpc finds someone, so it is rebuilt per NPC.
//  Read by INV_UpdateAndDrawPanels and the three NPCSHOP_ mouse handlers.
inline D2InvRectStrc gInvNpcShopRect;
static_assert(sizeof(gInvNpcShopRect) == 16, "gInvNpcShopRect: the item's 16 bytes in the .i64");

//  D2Client 0x6FBB36F8
//  Panel grid (cell size and origin) for inventory.txt record 13, Hireling, filled by INV_EnsurePanelLayout. Read by INV_MercInvDrawPanel; the
//  matching rect, gInvMercRect, has no reader but the loader. The panel's ten component grids go to gMercInvSlots, slots 1..10 only.
//  inventory.txt has an Expansion marker at data line 12, so every record id from 12 up is one data line lower than its row.
inline D2InventoryGridInfoStrc gInvMercGrid;
static_assert(sizeof(gInvMercGrid) == 24, "gInvMercGrid: the item's 24 bytes in the .i64");

//  D2Client 0x6FBB3710
//  The player's inventory grid (cell size and origin) for the same record as gInvPanelRect, filled by INV_EnsurePanelLayout. Read by
//  INV_UpdateAndDrawPanels, INV_OnLButtonDown, INV_OnMouseMove and INV_DrawInventoryGridItems (which converts cell to pixel with it).
inline D2InventoryGridInfoStrc gInvPanelGrid;
static_assert(sizeof(gInvPanelGrid) == 24, "gInvPanelGrid: the item's 24 bytes in the .i64");

//  D2Client 0x6FBB3728
//  Panel grid for inventory.txt record 8, Bank Page 1: the classic stash. INV_StashOnLButtonDown takes it on the not-expansion branch of its
//  GAME_IsExpansionGame test.
inline D2InventoryGridInfoStrc gInvStashGrid;
static_assert(sizeof(gInvStashGrid) == 24, "gInvStashGrid: the item's 24 bytes in the .i64");

//  D2Client 0x6FBB3740
//  Panel rectangle for inventory.txt record 10, Guild Vault Page 1. Loaded and never read: neither the rect nor the grid has a reader beyond
//  INV_EnsurePanelLayout (the panel does not exist in 1.10).
inline D2InvRectStrc gInvGuildVaultRect;
static_assert(sizeof(gInvGuildVaultRect) == 16, "gInvGuildVaultRect: the item's 16 bytes in the .i64");

//  D2Client 0x6FBB3750
//  Panel rectangle for inventory.txt record 12, Big Bank Page 1: the expansion (LoD) stash. INV_StashOnLButtonDown takes it when
//  GAME_IsExpansionGame returns non-zero; the classic game uses gInvStashRect.
inline D2InvRectStrc gInvBigStashRect;
static_assert(sizeof(gInvBigStashRect) == 16, "gInvBigStashRect: the item's 16 bytes in the .i64");

//  D2Client 0x6FBB3760
//  Panel grid for inventory.txt record 12, Big Bank Page 1: the expansion (LoD) stash. INV_StashOnLButtonDown takes it when
//  GAME_IsExpansionGame returns non-zero; the classic game uses gInvStashGrid.
inline D2InventoryGridInfoStrc gInvBigStashGrid;
static_assert(sizeof(gInvBigStashGrid) == 24, "gInvBigStashGrid: the item's 24 bytes in the .i64");

//  D2Client 0x6FBB3778
//  Five palette indices for the inventory's tints, filled by INV_InitHighlightColors and re-filled around every loading screen (an index is
//  valid only for the palette it was mixed against). Each is a half-intensity RGB, so the tint sits under the item art:
//      [0] MixRGB(0x80, 0, 0) dark red, the item does not fit here    [1] MixRGB(0, 0x80, 0) dark green, it does
//      [2] MixRGB(0, 0, 0x80) dark blue    [3] MixRGB(0x80, 0x80, 0) dark yellow    [4] MixRGB(0x80, 0x40, 0x40) dark rose
//  INV_DrawGridHighlight indexes the table; INV_MercInvDrawSlotHighlights and INV_UpdateAndDrawPanels reach [0] and [1] by name,
//  INV_UpdateAndDrawPanels also reads [4].
inline uint8_t gInvHighlightColors[5];
static_assert(sizeof(gInvHighlightColors) == 5, "gInvHighlightColors: the item's 5 bytes in the .i64");

//  D2Client 0x6FBB3780
//  Panel rectangle for inventory.txt record 13, Hireling, filled by INV_EnsurePanelLayout (its only reference). See gInvMercGrid.
inline D2InvRectStrc gInvMercRect;
static_assert(sizeof(gInvMercRect) == 16, "gInvMercRect: the item's 16 bytes in the .i64");

//  D2Client 0x6FBB3790
//  Written (0) by four functions, INV_StashLoadImages, INV_CubeLoadPanelImage, UNRESOLVED_sub_6FAED210 and INV_NpcShopBuildTabs, and read by
//  none, so its meaning cannot be recovered and it stays unnamed. It looks like a button-pressed or panel-state flag whose reader is gone
//  (a hypothesis, not a reading). It is not part of gInvMercRect, which ends at this address.
inline int32_t UNRESOLVED_dword_6FBB3790;
static_assert(sizeof(UNRESOLVED_dword_6FBB3790) == 4, "UNRESOLVED_dword_6FBB3790: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB3794
//  Panel rectangle for inventory.txt record 8, Bank Page 1: the classic stash. INV_StashOnLButtonDown takes it on the not-expansion branch of
//  its second GAME_IsExpansionGame test; the expansion branch falls through to gInvBigStashRect.
inline D2InvRectStrc gInvStashRect;
static_assert(sizeof(gInvStashRect) == 16, "gInvStashRect: the item's 16 bytes in the .i64");

//  D2Client 0x6FBB37A8
//  The NPC shop's tab bar: four 20-byte records, built per NPC by INV_NpcShopBuildTabs and read by INV_NpcShopApplyTabSelection, the two
//  NPCSHOP mouse handlers and INV_UpdateAndDrawPanels.
//      +0   bEnabled        0 blanks the slot
//      +4   bPressed        the selected flag; no NPCSHOP_SetTab* setter writes it, INV_NpcShopApplyTabSelection sets it on the tab whose
//                           nBaseFrame equals the wanted action
//      +8   nHoverTextId    the label's tbl string id, a word: 3335 buy, 3336 sell, 3338 repair, 10095 repair all, 0 blank
//      +10  nInventoryMode  the mode the tab opens, a dword: 2 buy, 3 sell, 4 repair; 1 for repair all and for a blank slot
//      +14  nBaseFrame      the tab's action code, a word: 2 buy, 4 sell, 6 repair, 18 repair all, 0 blank
//      +16  nCursorFrame    the cursor passed to CURSOR_SetCursorBuySell: 3 buy, 4 sell, 1 repair and blank, 8 repair all
//  Selection matches on the action code, not on the slot, so the bar can be built in a different order for each NPC. INV_NpcShopApplyTabSelection
//  tests nHoverTextId against 10095 because the repair-all tab acts instead of opening a panel. Slot 1 is Sell on every path that builds a bar;
//  slot 2 is Repair for the five act smiths and blank for every other vendor; slot 3 is the Close tab (string 4144, mode 9, action 10) except on
//  the branch where it is INV_NpcShopSetTabRepairAll(3). The Close form writes no cursor, so its +16 keeps what the previous NPC left.
//  INV_NpcShopBuildTabs writes the fields of slots 1 to 3 inline by absolute address on its vendor-gamble path.
inline D2InvButtonInfoStrc gNpcShopTabs[4];
static_assert(sizeof(gNpcShopTabs) == 80, "gNpcShopTabs: the item's 80 bytes in the .i64");

//  D2Client 0x6FBB37F8
//  Panel grid for inventory.txt record 6, Trade Page 1, filled by INV_EnsurePanelLayout. Read by INV_DrawInventoryGridItems, INV_OnMouseMove
//  and INV_TradeOnMouseMove.
inline D2InventoryGridInfoStrc gInvTradePage1Grid;
static_assert(sizeof(gInvTradePage1Grid) == 24, "gInvTradePage1Grid: the item's 24 bytes in the .i64");

//  D2Client 0x6FBB3810
//  Panel rectangle for inventory.txt record 6, Trade Page 1, filled by INV_EnsurePanelLayout (its only reference). Loaded and never read.
inline D2InvRectStrc gInvTradePage1Rect;
static_assert(sizeof(gInvTradePage1Rect) == 16, "gInvTradePage1Rect: the item's 16 bytes in the .i64");

//  D2Client 0x6FBB3820
//  Panel grid for inventory.txt record 11, Trophy Case Page 1, filled by INV_EnsurePanelLayout. Loaded and never read.
inline D2InventoryGridInfoStrc gInvTrophyCaseGrid;
static_assert(sizeof(gInvTrophyCaseGrid) == 24, "gInvTrophyCaseGrid: the item's 24 bytes in the .i64");

//  D2Client 0x6FBB3838
//  Panel grid for inventory.txt record 7, Trade Page 2, filled by INV_EnsurePanelLayout. Read by INV_DrawInventoryGridItems,
//  INV_OnLButtonDown and INV_OnMouseMove.
inline D2InventoryGridInfoStrc gInvTradePage2Grid;
static_assert(sizeof(gInvTradePage2Grid) == 24, "gInvTradePage2Grid: the item's 24 bytes in the .i64");

//  D2Client 0x6FBB3850
//  Panel rectangle for inventory.txt record 9, Transmogrify Box Page 1, filled by INV_EnsurePanelLayout. This is the Horadric Cube grid, which
//  the row name does not say; INV_CubeOnLButtonDown is its only reader.
inline D2InvRectStrc gInvCubeRect;
static_assert(sizeof(gInvCubeRect) == 16, "gInvCubeRect: the item's 16 bytes in the .i64");

//  D2Client 0x6FBB3860
//  Panel rectangle for inventory.txt record 7, Trade Page 2, filled by INV_EnsurePanelLayout (its only reference). Loaded and never read.
inline D2InvRectStrc gInvTradePage2Rect;
static_assert(sizeof(gInvTradePage2Rect) == 16, "gInvTradePage2Rect: the item's 16 bytes in the .i64");

//  D2Client 0x6FBB3870
//  GetTickCount of the last item use, against which INV_UseItemAtGridCell enforces its 250 ms (0xFA) cooldown, in multiplayer game types only.
inline int32_t gnLastItemUseTick;
static_assert(sizeof(gnLastItemUseTick) == 4, "gnLastItemUseTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB3874
//  Panel grid for inventory.txt record 10, Guild Vault Page 1, filled by INV_EnsurePanelLayout. Loaded and never read.
inline D2InventoryGridInfoStrc gInvGuildVaultGrid;
static_assert(sizeof(gInvGuildVaultGrid) == 24, "gInvGuildVaultGrid: the item's 24 bytes in the .i64");

//  D2Client 0x6FBB388C
//  Cursor X for the UI, from pMsg->nXpos (+0x0C of SMSGHANDLER_PARAMS). Written by INV_ResetPanelState, INV_TradeResetPanelState and the
//  handlers of the NPC shop, anvil, trade, stash and cube panels; read by their hit tests. INV_OnLButtonDown, INV_OnMouseMove,
//  INV_OnRButtonDown and INV_OnLButtonUp do not use it.
inline int32_t gnUiMouseX;
static_assert(sizeof(gnUiMouseX) == 4, "gnUiMouseX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB3890
//  data\global\ui\panel\trade, loaded on first use by INV_TradeSetPanelMode.
inline D2CellFileStrc *gpImgTradePanel;
static_assert(sizeof(gpImgTradePanel) == 4, "gpImgTradePanel: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB3894
//  Cursor Y for the UI, from pMsg->nYpos (+0x0E of SMSGHANDLER_PARAMS).
inline int32_t gnUiMouseY;
static_assert(sizeof(gnUiMouseY) == 4, "gnUiMouseY: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB3898
//  The grid of the NPC's own inventory panel, the second output of INV_EnsurePanelLayout's NPC pass (see gInvNpcShopRect). Read by
//  INV_DrawInventoryGridItems, INV_NpcShopOnMouseMove, INV_NpcShopOnLButtonDown and INV_NpcShopApplyTabSelection.
inline D2InventoryGridInfoStrc gInvNpcShopGrid;
static_assert(sizeof(gInvNpcShopGrid) == 24, "gInvNpcShopGrid: the item's 24 bytes in the .i64");

//  D2Client 0x6FBB38B0
//  Panel grid for inventory.txt record 9, Transmogrify Box Page 1, filled by INV_EnsurePanelLayout: the Horadric Cube grid.
inline D2InventoryGridInfoStrc gInvCubeGrid;
static_assert(sizeof(gInvCubeGrid) == 24, "gInvCubeGrid: the item's 24 bytes in the .i64");

//  D2Client 0x6FBB38C8
//  The player's inventory panel rectangle, filled by INV_EnsurePanelLayout from
//  DATATBLS_GetInventoryRect(UNITS_GetInventoryRecordId(player, INVPAGE_INVENTORY, bExpansion), gbHighResolution, &this), so the record is
//  the player's class row, and cached until the resolution changes. The mouse handlers of every panel that can overlap the inventory
//  (INV_, ITEMPANEL_, NPCSHOP_, ANVIL_, TRADE_, STASH_, CUBE_) hit-test against it, though not every handler of those panels does.
inline D2InvRectStrc gInvPanelRect;
static_assert(sizeof(gInvPanelRect) == 16, "gInvPanelRect: the item's 16 bytes in the .i64");

//  D2Client 0x6FBB38D8
//  The recipe scroll's text, written by NEWSTATS_RecipeScrollShow and drawn by NEWSTATS_RecipeScrollDraw. 4096 Unicode units (8192 bytes);
//  the original runs Unicode's default constructor over all of it during static initialisation.
//  BUG: NEWSTATS_RecipeScrollShow copies into it with an unbounded Unicode::strcpy.
inline Unicode gwszRecipeScrollText[4096];
static_assert(sizeof(gwszRecipeScrollText) == 8192, "gwszRecipeScrollText: the item's 8192 bytes in the .i64");

//  D2Client 0x6FBB58D8
//  Set once INV_EnsurePanelLayout has filled the player-side rects, grids and component grids. Cleared only by a resolution change, so the
//  player layout is built once per resolution rather than per panel open.
inline BOOL gbInvLayoutBuilt;
static_assert(sizeof(gbInvLayoutBuilt) == 4, "gbInvLayoutBuilt: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB58DC
//  Set once the interacting NPC's exchange rect and grid have been filled. Separate from gbInvLayoutBuilt because it depends on which NPC is
//  open, not on the resolution, though a resolution change clears both.
inline BOOL gbInvNpcLayoutBuilt;
static_assert(sizeof(gbInvNpcLayoutBuilt) == 4, "gbInvNpcLayoutBuilt: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB58E0
//  Master switch for the item description.
inline BOOL gbShowItemDesc;
static_assert(sizeof(gbShowItemDesc) == 4, "gbShowItemDesc: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB58E4
//  Master switch for the set-item description.
inline BOOL gbShowSetItemDesc;
static_assert(sizeof(gbShowSetItemDesc) == 4, "gbShowSetItemDesc: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB58E8
//  A one-shot request: when it holds 1, INV_UpdateAndDrawPanels clears bPressed on every shop tab and sets this back to 0. Set by the
//  inventory click handlers (INV_OnBodyLocClickGeneric, INV_OnBodyLocClickLeftArm, INV_OnBodyLocClickRightArm, INV_OnGridLeftClick), so
//  touching your own items drops the shop's tab highlight. INV_NpcShopBuildTabs clears it: opening a vendor cancels a pending request.
inline BOOL gbDeselectShopTabsPending;
static_assert(sizeof(gbDeselectShopTabsPending) == 4, "gbDeselectShopTabsPending: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB58EC
//  The inventory / buy-sell mode: which panel family the inventory screen is in. 1 is the ordinary inventory; 5 is VENDORMODE_VERIFY and
//  7 is VENDORMODE_DISPLAYING_RESULTS (the original's asserts at inv.cpp lines 7783 and 7833 name these two). INV_TradeSetPanelMode stores
//  11 or 10, INV_StashOpenPanel 12 and INV_CubeOpenPanel 14. The other values are known by number only.
inline int32_t sgInventoryMode;
static_assert(sizeof(sgInventoryMode) == 4, "sgInventoryMode: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB58F0
//  The item the cursor is over; the whole description popup is about this.
inline D2UnitStrc *gpHoverItem;
static_assert(sizeof(gpHoverItem) == 4, "gpHoverItem: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB58F4
//  A second copy of the hovered item pointer, written with the same value as gpHoverItem. INV_OnMouseMove tests the new item against it before
//  the writes, and recomputes the popup geometry only when the hover moves to a different item. INV_ClearHoverForItem and four arms of
//  INV_OnMouseMove clear both; INV_MercInvOnMouseUp clears only gpHoverItem, so the two can diverge there.
inline D2UnitStrc *gpHoverItemLast;
static_assert(sizeof(gpHoverItemLast) == 4, "gpHoverItemLast: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5900
//  Which vendor page the inventory helpers are working on, values 0..4. Accessed a byte at a time (three dword loads are masked with 0xFF,
//  hence uint8_t). Mostly scratch state: INV_UpdateAndDrawPanels sets it to the page it is about to draw and puts the previous value back.
//  Written by INV_OnMouseMove, INV_ResetPanelState and INV_NpcShopEnterPanelMode19; read by the NPC shop tab functions and
//  INV_GetVendorPageInventory.
inline uint8_t gnVendorPage;
static_assert(sizeof(gnVendorPage) == 1, "gnVendorPage: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB5904
//  A GetTickCount deadline. INV_NpcShopBuildTabs arms it at GetTickCount() + 500 when a vendor opens; while it is non-zero
//  INV_GetVendorPageInventory returns at once without rebuilding the page buckets. INV_UpdateAndDrawPanels zeroes it once expired and
//  INV_NpcShopOnLButtonDown clears it so a click rebuilds at once.
inline uint32_t gnVendorPageHoldUntil;
static_assert(sizeof(gnVendorPageHoldUntil) == 4, "gnVendorPageHoldUntil: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5908
//  Highest vendor page this NPC has, set by INV_NpcShopBuildTabs: 3 for the NPCs that get a full bar, 1 on the reduced path taken when
//  NPCMENU_IsVendorGamble() is non-zero. INV_GetVendorPageInventory reads it once after bucketing to wrap the page counter (past it, or at 0xFF,
//  the page goes back to 0); INV_NpcShopOnLButtonUp also reads it.
inline uint8_t gnVendorMaxPage;
static_assert(sizeof(gnVendorMaxPage) == 1, "gnVendorMaxPage: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB590C
//  The vendor's stock changed; re-bucket its pages on the next frame. Set to 1 by INV_NpcShopRequestPageRebuild, which
//  ITEM_ItemMsg0B0C_UpdateNpcStore calls after adding or removing an item in the NPC's inventory. INV_UpdateAndDrawPanels consumes it when it
//  is exactly 1 (a value other than 0 or 1 would be ignored; nothing writes one) and gnVendorPageHoldUntil has expired.
inline BOOL gbVendorPageRebuildPending;
static_assert(sizeof(gbVendorPageRebuildPending) == 4, "gbVendorPageRebuildPending: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5910
//  Non-zero while the Horadric Cube transmute animation plays. Set by INV_CubeStartQuestItemAnim; cleared by INV_CubeResetQuestItemAnim,
//  INV_CubeOnDismissKey, INV_CubeOnPanelClose and by INV_UpdateAndDrawPanels when the frame counter runs out. It also hides the cube's item
//  grid: inventory page 3 is drawn only when this is clear or gnHoradricAnimFrame has reached 14.
inline BOOL gbHoradricAnimActive;
static_assert(sizeof(gbHoradricAnimActive) == 4, "gbHoradricAnimActive: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5914
//  Current cel of the Horadric Cube transmute animation, 0..29. INV_UpdateAndDrawPanels advances it every 70 ms and ends the animation at 30.
inline int32_t gnHoradricAnimFrame;
static_assert(sizeof(gnHoradricAnimFrame) == 4, "gnHoradricAnimFrame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5918
//  GetTickCount() at the last advance of gnHoradricAnimFrame; the animation steps when 0x46 ms (70) have passed.
inline uint32_t gdwHoradricAnimLastTick;
static_assert(sizeof(gdwHoradricAnimLastTick) == 4, "gdwHoradricAnimLastTick: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB591C
//  The data\global\ui\menu\horadric cel file, loaded lazily by INV_CubeStartQuestItemAnim and freed by INV_CubeResetQuestItemAnim.
//  INV_UpdateAndDrawPanels draws it with TEXTURE_CelDraw at the screen centre, one cel per gnHoradricAnimFrame.
inline D2CellFileStrc *gpHoradricAnimImage;
static_assert(sizeof(gpHoradricAnimImage) == 4, "gpHoradricAnimImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5920
//  The equipment body location under the cursor, 0 for none. INV_OnMouseMove zeroes it and sets it from the hovered slot; INV_UpdateAndDrawPanels
//  reads it to draw that slot's highlight.
inline int32_t gnHoverBodyLoc;
static_assert(sizeof(gnHoverBodyLoc) == 4, "gnHoverBodyLoc: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5924
//  data\global\ui\panel\buyselltabs, loaded once by INV_NpcShopLoadTabImages.
inline D2CellFileStrc *gpImgBuySellTabs;
static_assert(sizeof(gpImgBuySellTabs) == 4, "gpImgBuySellTabs: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5928
//  data\global\ui\panel\miniconvert, loaded once by INV_NpcShopLoadTabImages.
inline D2CellFileStrc *gpImgMiniConvert;
static_assert(sizeof(gpImgMiniConvert) == 4, "gpImgMiniConvert: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB592C
//  DATA\GLOBAL\ui\panel\bank, the stash panel background. Loaded by INV_StashLoadImages, freed by INV_StashOnPanelClose and
//  INV_StashTeardownPanel, drawn by INV_UpdateAndDrawPanels.
inline D2CellFileStrc *gpStashPanelImage;
static_assert(sizeof(gpStashPanelImage) == 4, "gpStashPanelImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5930
//  The Horadric cube panel background, DATA\GLOBAL\ui\panel\supertransmogrifier. Loaded lazily by INV_CubeLoadPanelImage (not by
//  PANEL_LoadImages), freed and nulled by INV_CubeOnPanelClose. INV_UpdateAndDrawPanels draws it as four quadrant cels 0..3 at
//  x = gnBorderXDifference and + 256, y = height + gnBorderYDifference - 224 (cels 0 and 1) and - 48 (cels 2 and 3).
inline D2CellFileStrc *gpImgCubePanel;
static_assert(sizeof(gpImgCubePanel) == 4, "gpImgCubePanel: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5934
//  DATA\GLOBAL\UI\Panel\goldcoinbtn, loaded by INV_LoadPanelImages.
inline D2CellFileStrc *gpImgGoldCoinButton;
static_assert(sizeof(gpImgGoldCoinButton) == 4, "gpImgGoldCoinButton: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5938
//  data\global\ui\panel\invchar6tab, loaded once by INV_NpcShopLoadTabImages and only when both FOG_IsExpansion and GAME_IsExpansionGame are
//  true; it stays null for a classic character, so anything drawing it has to cope with that.
inline D2CellFileStrc *gpImgInvChar6Tab;
static_assert(sizeof(gpImgInvChar6Tab) == 4, "gpImgInvChar6Tab: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB593C
//  DATA\GLOBAL\ui\panel\TradeStash, the trade window's stash panel, loaded only when FOG_IsExpansion. Same lifetime and users as
//  gpStashPanelImage.
inline D2CellFileStrc *gpTradeStashImage;
static_assert(sizeof(gpTradeStashImage) == 4, "gpTradeStashImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5940
//  Index of the tab the mouse went down and up on, -1 for none. Set by INV_NpcShopOnLButtonUp; INV_NpcShopBuildTabs and INV_ResetItemGrids
//  reset it to -1 (the latter then stores the index of the first tab whose bPressed is set, if there is one). INV_NpcShopApplyTabSelection
//  reads it to fetch that tab's mode, string and cursor argument; the selection itself goes through gnNpcShopTabAction.
inline int32_t gnNpcShopTabHit;
static_assert(sizeof(gnNpcShopTabHit) == 4, "gnNpcShopTabHit: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5944
//  The action id of the clicked tab, written by INV_NpcShopOnLButtonUp beside gnNpcShopTabHit. INV_NpcShopApplyTabSelection sets every tab's
//  bPressed to (tab.nBaseFrame == this), so the bar is selected by action and not by index: two tabs sharing an action id would light up
//  together.
inline int32_t gnNpcShopTabAction;
static_assert(sizeof(gnNpcShopTabAction) == 4, "gnNpcShopTabAction: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5948
//  Whether the item a pending NPC transaction is about belongs to the player rather than to the vendor. Written by INV_NpcShopOnLButtonUp
//  from the bPlayerOwnsItem it passes to NPCMENU_BeginTransaction, and read back through INV_GetTransactionItemIsPlayers by
//  NPCMENU_BeginTransaction and NPCMENU_SendTransactionAndWait, which runs after a server round trip, when the click's own arguments are gone.
inline BOOL gbTransactionItemIsPlayers;
static_assert(sizeof(gbTransactionItemIsPlayers) == 4, "gbTransactionItemIsPlayers: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB594C
//  Which weapon set is active: 0 primary, 1 secondary. SCMD_HandleSrv97_WeaponSwitch toggles it on server packet 0x97, which carries no
//  payload, so the client keeps the set itself. The toggle is inside FOG_IsExpansion() && GAME_IsExpansionGame(), and INV_LoadPanelImages
//  initialises it to 0 in both arms of the same test. INV_UpdateAndDrawPanels reads it to draw the I/II indicator.
inline int32_t gnWeaponSwapSet;
static_assert(sizeof(gnWeaponSwapSet) == 4, "gnWeaponSwapSet: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5950
//  Set by INV_UseItemAtGridCell when the item used is the Horadric Cube (base code "box ", 0x20786F62) and the stash is open, so the cube
//  panel returns to the stash when it closes. Read and cleared by INV_CubeOnLButtonUp, INV_CubeOnDismissKey and INV_CubeOnPanelClose;
//  INV_StashOpenPanel clears it when the stash opens.
inline BOOL gbCubeOpenedFromStash;
static_assert(sizeof(gbCubeOpenedFromStash) == 4, "gbCubeOpenedFromStash: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5954
//  Send-once latch for the cube's close notification. Each of the three ways of closing the cube (INV_CubeOnLButtonUp, INV_CubeOnDismissKey,
//  INV_CubeOnPanelClose) tests it first and then sends MSG_SendCmd3Words(79, 23, 0, 0) and sets it, so the server is told once however many of
//  the three fire. INV_CubeOpenPanel clears it. Command 79 (0x4F) is "close container" and its first word says which: INV_StashOnLButtonUp sends
//  (79, 18, 0, 0), so 18 is the stash and 23 the cube.
inline BOOL gbCubeCloseCmdSent;
static_assert(sizeof(gbCubeCloseCmdSent) == 4, "gbCubeCloseCmdSent: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5958
//  Eleven records of 20 bytes indexed by D2C_PlayerBodyLocs (SWRARM and SWLARM are not in it; record 0 is never filled): four dwords of
//  rectangle (left, right, top, bottom), then the cell size in two bytes. INV_OnMouseMove's (field0 + field1) / 2 popup anchor X fixes field 0
//  as left and field 1 as right. DATATBLS_GetInventoryComponentGrid maps inventory.txt components 0..9 (rArm, torso, lArm, head, neck, rHand,
//  lHand, belt, feet, gloves) onto records 4, 3, 5, 1, 2, 6, 7, 8, 9, 10. The inventory.txt columns rHand and lHand are the ring slots (records
//  6 and 7, BODYLOC_RRIN and BODYLOC_LRIN); the hands are rArm and lArm, components 0 and 2.
inline D2InvCompGridStrc gInvBodyLocRects[11];
static_assert(sizeof(gInvBodyLocRects) == 220, "gInvBodyLocRects: the item's 220 bytes in the .i64");

//  D2Client 0x6FBB5A38
//  The hireling panel's equip-slot grids: the same layout as gInvBodyLocRects (11 records of 20 bytes, indexed by D2C_PlayerBodyLocs), for
//  inventory.txt record 13, Hireling, filled by INV_EnsurePanelLayout's ten DATATBLS_GetInventoryComponentGrid(13, ...) calls. Slot 0 is never
//  written, so INV_MercInvDrawSlotHighlights skips it with an nLeft <= 0 guard rather than an index test.
inline D2InvCompGridStrc gMercInvSlots[11];
static_assert(sizeof(gMercInvSlots) == 220, "gMercInvSlots: the item's 220 bytes in the .i64");

//  D2Client 0x6FBB5B14
//  The bHigherRes value the inventory layout was last built for. A mismatch clears both gbInvLayoutBuilt and gbInvNpcLayoutBuilt: the single
//  invalidator for every inventory rectangle in the client, since a resolution change moves all of them.
inline int32_t gnInvLayoutRes;
static_assert(sizeof(gnInvLayoutRes) == 4, "gnInvLayoutRes: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B18
//  Whether the local player has accepted the trade, as last reported by the trade state machine (TRADE_HandleStateMessage passes it as the
//  first argument). INV_UpdateAndDrawPanels draws the checked cel (frame 17 instead of 16) at the local accept button (borderX + 223,
//  height - 64) when it or gbTradeAcceptChecked is set, and INV_UpdateToggleSounds compares the incoming pair against it to choose the sound
//  before storing the new value. By draw position it is the bottom half of the trade window, i.e. the local player's.
inline BOOL gbTradeOwnAccepted;
static_assert(sizeof(gbTradeOwnAccepted) == 4, "gbTradeOwnAccepted: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B1C
//  Whether the other player has accepted the trade: the second argument to INV_UpdateToggleSounds and the twin of gbTradeOwnAccepted.
//  INV_UpdateAndDrawPanels draws its checked cel in the other player's half (borderX + 275, height - 278). Cleared with its twin by
//  INV_TradeSetPanelMode on every panel-mode switch.
inline BOOL gbTradeOtherAccepted;
static_assert(sizeof(gbTradeOtherAccepted) == 4, "gbTradeOtherAccepted: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B20
//  Whether the local player's accept-trade box is checked. INV_TradeOnLButtonDown toggles it when the accept button is clicked
//  (TRADE_SendGreenCheck is called on the way back to 0 only); INV_TradeOnLButtonUp sends TRADE_SendAccept only if it is set and
//  gbTradeAcceptBtnArmed is still set. Cleared by INV_TradeSetPanelMode on every panel-mode switch.
//  D2ExpRes ExchangeAcceptTradeButtonClickdown and ExchangeAcceptTradeButtonRelease patch this button in the two handlers.
inline BOOL gbTradeAcceptChecked;
static_assert(sizeof(gbTradeAcceptChecked) == 4, "gbTradeAcceptChecked: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B24
//  Set when the mouse goes down on the trade cancel button; INV_TradeOnLButtonUp turns it into TRADE_SendCancelOnce only when
//  gbTradeCancelBtnArmed is also still set, then clears it.
//  D2ExpRes ExchangeCancelButtonClickdown and ExchangeCancelButtonRelease patch this button.
inline BOOL gbTradeCancelPending;
static_assert(sizeof(gbTradeCancelPending) == 4, "gbTradeCancelPending: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B28
//  Set while the mouse went down on the accept-trade button and has not been released yet: the press half of a press-and-release pair.
//  INV_TradeOnLButtonUp requires it before sending TRADE_SendAccept, and its released-elsewhere path uses it to undo an accept that was armed
//  but not completed (unchecking gbTradeAcceptChecked and sending TRADE_SendGreenCheck to clear it on the server).
inline BOOL gbTradeAcceptBtnArmed;
static_assert(sizeof(gbTradeAcceptBtnArmed) == 4, "gbTradeAcceptBtnArmed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B2C
//  The press half of the cancel button's pair, the counterpart of gbTradeAcceptBtnArmed. It and gbTradeCancelPending must both survive to the
//  release for TRADE_SendCancelOnce to fire there; INV_TradeOnLButtonUp also calls TRADE_SendCancelOnce with neither tested when the release
//  lands on the inventory close button's rect with no belt hit and nothing held.
inline BOOL gbTradeCancelBtnArmed;
static_assert(sizeof(gbTradeCancelBtnArmed) == 4, "gbTradeCancelBtnArmed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B30
//  A third visual state of the trade accept button, taking priority over the other two. INV_UpdateAndDrawPanels picks one of three draws at
//  the accept button's rect (borderX + 223, height - 64):
//      gbTradeAcceptBlocked  -> frame 16 via TEXTURE_CelDrawColor, colour mode 1
//      own/checked accepted  -> frame 17 via TEXTURE_CelDrawColor, colour mode 2
//      otherwise             -> frame 16 via plain TEXTURE_CelDraw
//  Set by TRADE_HandleStateMessage through INV_TradeSetAcceptBlocked and cleared by INV_TradeClearAcceptBlocked from three places including
//  TRADE_CloseWindow. "Blocked" names the visual, not a confirmed gameplay rule.
inline BOOL gbTradeAcceptBlocked;
static_assert(sizeof(gbTradeAcceptBlocked) == 4, "gbTradeAcceptBlocked: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B34
//  Re-entry guard on UI_OpenGoldEntryDialog: while set, a second click on a gold button does nothing. INV_OnLButtonUp, INV_TradeOnLButtonUp,
//  INV_StashOnLButtonUp (twice) and INV_CubeOnLButtonUp test it after the button hit test and before the call. Written 1 by
//  INV_ArmGoldDialogGuard and cleared to 0 by UI_CloseGoldEntryDialog, so its lifetime is the dialog's.
inline BOOL gbGoldDialogOpen;
static_assert(sizeof(gbGoldDialogOpen) == 4, "gbGoldDialogOpen: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B38
//  Pressed state of the drop/deposit gold button that INV_DrawGoldAmount draws in its carried-gold branch (STAT_GOLD): it is the cel frame
//  (1 while held, 0 when not) and adds one to y, so the button appears to sink. Reset by INV_ResetPanelState.
//  D2ExpRes InventoryDepositDropTradeGoldButtonAndGoldCount names this button.
inline BOOL gbDropGoldBtnPressed;
static_assert(sizeof(gbDropGoldBtnPressed) == 4, "gbDropGoldBtnPressed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B3C
//  Pressed state of the stash withdraw button, the counterpart of gbDropGoldBtnPressed, used in INV_DrawGoldAmount's stash branch
//  (STAT_GOLDBANK) with the same one-pixel sink.
//  D2ExpRes StashWithdrawButton1, StashWithdrawButton2 and StashWithdrawButtonMouseHoverOver name this button.
inline BOOL gbWithdrawGoldBtnPressed;
static_assert(sizeof(gbWithdrawGoldBtnPressed) == 4, "gbWithdrawGoldBtnPressed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B40
//  Pressed state of the button at the bottom edge of the stash panel. Written by INV_StashOnLButtonDown, INV_StashOnLButtonUp and
//  INV_StashOpenPanel; read by INV_UpdateAndDrawPanels as the cel frame value + 10 (10 released art, 11 pressed art) and by
//  INV_StashOnLButtonUp.
//  D2ExpRes StashCloseButtonClickdown1..5 (INV_StashOnLButtonDown) and StashCloseButtonRelease1..6 (INV_StashOnLButtonUp) name the button.
inline BOOL gbStashCloseBtnPressed;
static_assert(sizeof(gbStashCloseBtnPressed) == 4, "gbStashCloseBtnPressed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B44
//  A left-button press is armed on the stash's close button; the partner of gbStashCloseBtnPressed, which is the drawn state. Set by
//  INV_StashOnLButtonDown and read once, in INV_StashOnLButtonUp's cleanup:
//      else if ( gbStashCloseBtnArmed && gbStashCloseBtnPressed )
//          gbStashCloseBtnPressed = 0;
//  That is the branch for a release that landed off the button: the sunk art is undone and nothing is sent. The two flags carry the same value
//  almost always; the difference shows only where the press is abandoned.
inline BOOL gbStashCloseBtnArmed;
static_assert(sizeof(gbStashCloseBtnArmed) == 4, "gbStashCloseBtnArmed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B48
//  Pressed state of the Horadric cube's close button, the cube's equivalent of gbStashCloseBtnPressed: INV_UpdateAndDrawPanels reads it as the
//  cel frame value + 10 of a GetButtonImage() draw at (borderX + 275, height - 65), the same bottom-edge slot as the stash's (the two
//  panels are never open together). Written by INV_CubeOpenPanel, INV_CubeOnLButtonDown and INV_CubeOnLButtonUp.
inline BOOL gbCubeCloseBtnPressed;
static_assert(sizeof(gbCubeCloseBtnPressed) == 4, "gbCubeCloseBtnPressed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B4C
//  The cube's twin of gbStashCloseBtnArmed: armed by INV_CubeOnLButtonDown and read once by INV_CubeOnLButtonUp as
//  if (gbCubeCloseBtnArmed && gbCubeCloseBtnPressed) gbCubeCloseBtnPressed = 0;
inline BOOL gbCubeCloseBtnArmed;
static_assert(sizeof(gbCubeCloseBtnArmed) == 4, "gbCubeCloseBtnArmed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B50
//  Pressed state of the cube's Transmute button: the raw cel frame (no +10 base, unlike the two close buttons) for gpImgMiniConvert, drawn at
//  (borderX + 144, height - 188) inside the cube's grid area. Written by INV_CubeOnLButtonDown and INV_CubeOnLButtonUp.
inline BOOL gbCubeTransmuteBtnPressed;
static_assert(sizeof(gbCubeTransmuteBtnPressed) == 4, "gbCubeTransmuteBtnPressed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B54
//  Armed state of the cube's Transmute button, the partner of gbCubeTransmuteBtnPressed; read only where a release that missed the button
//  has to undo the sunk art. Set by INV_CubeOnLButtonDown.
inline BOOL gbCubeTransmuteBtnArmed;
static_assert(sizeof(gbCubeTransmuteBtnArmed) == 4, "gbCubeTransmuteBtnArmed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B70
//  DATA\GLOBAL\UI\Panel\inv_armor, the inventory's armour slot art. Loaded by INV_LoadPanelImages through UI_LoadImageFromUiDir, freed by
//  INV_UnloadPanelImages, drawn by INV_UpdateAndDrawPanels when the body location is empty.
inline D2CellFileStrc *gpImgInvArmor;
static_assert(sizeof(gpImgInvArmor) == 4, "gpImgInvArmor: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B74
//  DATA\GLOBAL\UI\Panel\inv_belt. See gpImgInvArmor.
inline D2CellFileStrc *gpImgInvBelt;
static_assert(sizeof(gpImgInvBelt) == 4, "gpImgInvBelt: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B78
//  DATA\GLOBAL\UI\Panel\inv_boots. See gpImgInvArmor.
inline D2CellFileStrc *gpImgInvBoots;
static_assert(sizeof(gpImgInvBoots) == 4, "gpImgInvBoots: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B7C
//  DATA\GLOBAL\UI\Panel\inv_helm_glove: one cel file for both slots. See gpImgInvArmor.
inline D2CellFileStrc *gpImgInvHelmGlove;
static_assert(sizeof(gpImgInvHelmGlove) == 4, "gpImgInvHelmGlove: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B80
//  DATA\GLOBAL\UI\Panel\inv_ring_amulet: one cel file for all three slots. See gpImgInvArmor.
inline D2CellFileStrc *gpImgInvRingAmulet;
static_assert(sizeof(gpImgInvRingAmulet) == 4, "gpImgInvRingAmulet: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B84
//  DATA\GLOBAL\UI\Panel\inv_weapons. See gpImgInvArmor.
inline D2CellFileStrc *gpImgInvWeapons;
static_assert(sizeof(gpImgInvWeapons) == 4, "gpImgInvWeapons: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B88
//  DATA\GLOBAL\UI\Panel\tradebtn. Loaded and freed with the inventory slot art, so it belongs to the inventory panel rather than to the trade
//  panel.
inline D2CellFileStrc *gpImgTradeButton;
static_assert(sizeof(gpImgTradeButton) == 4, "gpImgTradeButton: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B8C
//  DATA\GLOBAL\UI\Panel\invchar6Tab, loaded only in an expansion game: INV_LoadPanelImages guards it on FOG_IsExpansion and
//  GAME_IsExpansionGame and NULLs it otherwise. A separate pointer from gpImgInvChar6Tab, which INV_UnloadShopPanelImages frees: two
//  panels load the same asset independently.
inline D2CellFileStrc *gpImgInvChar6TabCube;
static_assert(sizeof(gpImgInvChar6TabCube) == 4, "gpImgInvChar6TabCube: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B90
//  DATA\GLOBAL\UI\Panel\NPCInv, the vendor's own inventory grid art. Loaded in the same expansion-only branch as gpImgInvChar6TabCube, but
//  freed unconditionally.
inline D2CellFileStrc *gpImgNpcInv;
static_assert(sizeof(gpImgNpcInv) == 4, "gpImgNpcInv: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B94
//  menu\recipescroll, loaded lazily by NEWSTATS_RecipeScrollDraw on its first frame rather than by a loader function. Freed by
//  INV_UnloadPanelImages.
inline D2CellFileStrc *gpRecipeScrollImage;
static_assert(sizeof(gpRecipeScrollImage) == 4, "gpRecipeScrollImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B98
//  Set to 1 by INV_AnvilOnInvCloseButtonPress when a left press lands on the inventory panel's close button, and cleared by every panel's
//  button-up handler (TRADE, STASH, CUBE and the anvil). INV_AnvilOnInvCloseButtonRelease closes the panel only if it is still set.
inline BOOL gbInvCloseBtnArmed;
static_assert(sizeof(gbInvCloseBtnArmed) == 4, "gbInvCloseBtnArmed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5B9C
//  A twin of gbInvCloseBtnArmed that is written and never read: the five panel button-up handlers (INV_, NPCSHOP_, TRADE_, STASH_ and
//  INV_CubeOnLButtonUp) store the same value to it on the instruction after they clear gbInvCloseBtnArmed.
inline BOOL gbInvCloseBtnArmedUnread;
static_assert(sizeof(gbInvCloseBtnArmedUnread) == 4, "gbInvCloseBtnArmedUnread: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5BA8
//  A left-button press is in flight on this panel: set by INV_TradeOnLButtonDown when it arms the accept or cancel button, and used by the
//  release handlers as the gate for their whole button-completion block, which clears it again. Not trade-specific: INV_NpcShopOnLButtonUp
//  gates its release path on gbPanelBtnPressActive == 1 the same way, and INV_NpcShopOnLButtonDown and INV_NpcShopOnDismissKey clear it. It is
//  the shared press latch of the right-hand panel family.
inline BOOL gbPanelBtnPressActive;
static_assert(sizeof(gbPanelBtnPressActive) == 4, "gbPanelBtnPressActive: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5BAC
//  Pressed state of the mercenary panel's close button. INV_MercInvDrawPanel reads it as the cel frame (value != 0) + 10 of a GetButtonImage()
//  draw at (borderX + 272, height - 63), the same slot and frame base as gbStashCloseBtnPressed and gbCubeCloseBtnPressed. Set to 1 by
//  INV_MercInvOnLButtonDown when the click lands in the button; cleared by INV_MercInvOnMouseMove when the cursor leaves it while held and by
//  INV_MercInvOnMouseUp.
inline BOOL gbMercCloseBtnPressed;
static_assert(sizeof(gbMercCloseBtnPressed) == 4, "gbMercCloseBtnPressed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5BB0
//  The colour NEWSTATS_RecipeScrollDraw passes to D2Win_10117_DrawText for gwszRecipeScrollText, set by NEWSTATS_RecipeScrollShow alongside
//  the text. Those two functions are its only users.
inline int32_t gnRecipeScrollColor;
static_assert(sizeof(gnRecipeScrollColor) == 4, "gnRecipeScrollColor: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5BB4
//  The Panel\Level cel, the red button itself. Loaded by NEWSTATS_LoadImages, released by NEWSTATS_UnloadImages.
inline D2CellFileStrc *gpImgLevelUpButton;
static_assert(sizeof(gpImgLevelUpButton) == 4, "gpImgLevelUpButton: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5BB8
//  The Panel\Levelsocket cel, the recess the level-up button sits in. Loaded and released alongside gpImgLevelUpButton.
inline D2CellFileStrc *gpImgLevelUpSocket;
static_assert(sizeof(gpImgLevelUpSocket) == 4, "gpImgLevelUpSocket: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5BBC
//  The new-stats button's pressed state: armed on the press, checked on the release and cleared there on all three exit paths, and cleared
//  again by the draw, so a press that never reaches its release does not leave the button stuck.
inline BOOL gbNewStatsButtonDown;
static_assert(sizeof(gbNewStatsButtonDown) == 4, "gbNewStatsButtonDown: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB5BC0
//  The new-skills button's pressed state: armed on the press, checked on the release and cleared there on all three exit paths, and cleared
//  again by the draw, so a press that never reaches its release does not leave the button stuck.
inline BOOL gbNewSkillsButtonDown;
static_assert(sizeof(gbNewSkillsButtonDown) == 4, "gbNewSkillsButtonDown: the item's 4 bytes in the .i64");

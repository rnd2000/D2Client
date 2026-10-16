//  help.h -- the globals of help.cpp that are ours: the cached HelpMenu registry value, the path string szName, the help screen's three images, its
//  dim colour and the state of the help buttons. Each is a C++17 `inline` variable, one object for every file that includes this header. The
//  comment line "D2Client 0x6F..." above a global gives its address in the original. The items from 0x6FBB3650 on lie past the end of .data's raw
//  data (0x6FB9B000): the loader zero-fills them, and ours are zero-initialised. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB79088
//  Cached copy of the `Help Menu` value under the `Diablo II` registry key, -1 meaning "not read yet"; HELP_DrawHelpHint loads it once, lazily. When
//  it is non-zero the help button (UI_HELPBUTTON, 0x22) is turned off instead of the hint being drawn.
inline int32_t gnHelpMenuSeen = -1;
static_assert(sizeof(gnHelpMenuSeen) == 4, "gnHelpMenuSeen: the item's 4 bytes in the .i64");

//  D2Client 0x6FB790A4
inline CHAR szName[21] = "Menu\\helpwhitebullet";
static_assert(sizeof(szName) == 21, "szName: the item's 21 bytes in the .i64");

//  D2Client 0x6FBB3650
//  Cel handle for the help overlay's background tiles, used by HELP_DrawBackground; HELP_DrawScreen's 800x600 arm draws seven frames of it itself.
inline D2CellFileStrc *gpHelpBackgroundCel;
static_assert(sizeof(gpHelpBackgroundCel) == 4, "gpHelpBackgroundCel: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB3654
//  Cel handle for the pointer marks at the end of each help leader line (HELP_DrawLeaderLines).
inline D2CellFileStrc *gpHelpPointerCel;
static_assert(sizeof(gpHelpPointerCel) == 4, "gpHelpPointerCel: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB3658
//  Cel handle for the eight bullets down the left of the help overlay, one per F1..F8 line (HELP_DrawKeyBulletCels).
inline D2CellFileStrc *gpHelpBulletCel;
static_assert(sizeof(gpHelpBulletCel) == 4, "gpHelpBulletCel: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB365C
//  The palette index HELP_DimGameScreen fills the screen with (GFX_DrawFilledRect's dwColor, draw mode 1). Nothing writes it: the byte lies past
//  .data's raw data, so it is 0 at load and stays 0, palette index 0, which is black in every act palette of the stock install.
inline uint8_t gnHelpDimColor;
static_assert(sizeof(gnHelpDimColor) == 1, "gnHelpDimColor: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB3660
//  Pressed state of the help overlay's close button. Selects cel frame `STOREBUTTON_CLOSE + 1` (11) instead of 10 (HELP_DrawCloseButton).
inline int32_t gbHelpCloseButtonDown;
static_assert(sizeof(gbHelpCloseButtonDown) == 4, "gbHelpCloseButtonDown: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB3664
//  Set when HELP_DrawScreen had to turn UI_MINIPANEL (0x15) on for itself, so that closing the help overlay can put it back the way it was. Not a
//  general "help is open" flag.
inline int32_t gbHelpOpenedMiniPanel;
static_assert(sizeof(gbHelpOpenedMiniPanel) == 4, "gbHelpOpenedMiniPanel: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB3668
//  The help button's pressed state: set by HELP_HelpButtonOnLButtonDown, cleared by HELP_HelpButtonOnLButtonUp, read by HELP_DrawHelpHint.
inline BOOL gbHelpButtonDown;
static_assert(sizeof(gbHelpButtonDown) == 4, "gbHelpButtonDown: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB366C
//  Set for the frame when one of the six panels that hide the mini panel's Help hint is open (skill tree, skill selector, inventory, vendor, trade,
//  stash). Written by HELP_DrawHelpHint.
inline int32_t gbHelpHintSuppressed;
static_assert(sizeof(gbHelpHintSuppressed) == 4, "gbHelpHintSuppressed: the item's 4 bytes in the .i64");

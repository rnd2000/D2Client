//  Hireables.h -- the globals of Hireables.cpp that are ours: the Druid's icon path, the pet icon cache, the class icons, the party
//  portrait mode, the life bar's three colours and their latch, and the hint and hover state of the hireling icon. Address ranges
//  0x6FB79120 and 0x6FBB3670..0x6FBB36AC; the second lies past .data's raw data, so the loader zero-fills it and ours are
//  zero-initialised.
//  Each is a C++17 inline variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above a
//  global gives its address in the original.
//  Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB79120
//  The Druid's icon path, referenced once, by HIREABLES_PanelLoadHirelingIcons.
inline char gszDruidIconPath[20] = "Hireables\\DruidIcon";
static_assert(sizeof(gszDruidIconPath) == 20, "gszDruidIconPath: the item's 20 bytes in the .i64");

//  D2Client 0x6FBB3670
//  One record per pettype.txt row, five image handles each; allocated and zeroed by HIREABLES_PanelLoadHirelingIcons, filled lazily by
//  HIREABLES_PanelGetHirelingIcon.
inline D2ClientPetIconCacheStrc *gpPetIconCache;
static_assert(sizeof(gpPetIconCache) == 4, "gpPetIconCache: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB3674
//  Seven loaded class icons, NOT indexed by character class (Amazon, Sorceress, Barbarian, Paladin, Necromancer, Assassin, Druid); a
//  classic game loads five.
inline D2CellFileStrc *gpClassIcons[7];
static_assert(sizeof(gpClassIcons) == 28, "gpClassIcons: the item's 28 bytes in the .i64");

//  D2Client 0x6FBB3690
//  How the party portraits and hireling icons are displayed: 0 the party and the pets, 1 the pets only, 2 nothing. Written by
//  HIREABLES_CmdTblActionShowPortraits and HIREABLES_HireIconsResetAndShow.
inline int32_t gnPartyPortraitMode;
static_assert(sizeof(gnPartyPortraitMode) == 4, "gnPartyPortraitMode: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB3694
//  Palette index for a healthy life bar: MixRGB(0, 128, 0), green. Used at 50% and above.
inline uint8_t gnLifeBarColorHigh;
static_assert(sizeof(gnLifeBarColorHigh) == 1, "gnLifeBarColorHigh: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB3695
//  Palette index for a wounded life bar: MixRGB(192, 192, 0), yellow. Used from 25% to 49%.
inline uint8_t gnLifeBarColorMid;
static_assert(sizeof(gnLifeBarColorMid) == 1, "gnLifeBarColorMid: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB3696
//  Palette index for a critical life bar: MixRGB(255, 0, 0), red. Used below 25%.
inline uint8_t gnLifeBarColorLow;
static_assert(sizeof(gnLifeBarColorLow) == 1, "gnLifeBarColorLow: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB3697
//  Set once the three life-bar colours have been mixed for the current palette; cleared by HIREABLES_PanelInvalidateLifeBarColors.
inline uint8_t gbLifeBarColorsInit;
static_assert(sizeof(gbLifeBarColorsInit) == 1, "gbLifeBarColorsInit: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB3698
//  X the hireling-icon hover hint is drawn at; written by HIREABLES_PanelDrawHirelingIconHint, and the three HireIcons input handlers
//  hit-test against it.
inline int32_t gnHireIconHintX;
static_assert(sizeof(gnHireIconHintX) == 4, "gnHireIconHintX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB369C
//  Y the hireling-icon hover hint is drawn at, the companion of gnHireIconHintX; written and read by HIREABLES_PanelDrawHirelingIconHint.
inline int32_t gnHireIconHintY;
static_assert(sizeof(gnHireIconHintY) == 4, "gnHireIconHintY: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB36A0
//  The hireling-icon hover hint is up: raised after 700 ms on the icon, or at once when the cursor item can be given.
inline BOOL gbHireIconHintShown;
static_assert(sizeof(gbHireIconHintShown) == 4, "gbHireIconHintShown: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB36A4
//  The cursor is on a hireling icon; the first of the two gates for drawing the hint.
inline BOOL gbHireIconHovered;
static_assert(sizeof(gbHireIconHovered) == 4, "gbHireIconHovered: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB36A8
//  Raised by HIREABLES_HireIconsResetAndShow when the registry has no "PopupHireling" value or a zero one: the one-time hint about the
//  hireling panel is due.
inline uint8_t gbHirelingPopupPending;
static_assert(sizeof(gbHirelingPopupPending) == 1, "gbHirelingPopupPending: the item's 1 bytes in the .i64");

//  D2Client 0x6FBB36AC
//  The tick the hireling-icon hover began (GetTickCount), 0 when not hovering; the hint appears once 700 ms (0x2BC) have passed.
inline uint32_t gnHireIconHoverTick;
static_assert(sizeof(gnHireIconHoverTick) == 4, "gnHireIconHoverTick: the item's 4 bytes in the .i64");

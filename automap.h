//  automap.h -- the globals of automap.cpp: the cell-group and town tables, the settings, the cell-block pool and layer list, the pan and clip
//  state of the draw pass and the working values of the frame tick. Address range 0x6FB6EE00..0x6FBB1A40.
//  Each is a C++17 inline variable, so every file that includes this header shares one object. The two tables from .rdata are const. The globals at
//  0x6FBAF990..0x6FBB1A40 lie past the end of .data's raw data (0x6FB9B000): the loader zero-fills them, they have no value in the file, and ours
//  are zero-initialised.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after DLLmain.h and D2Client.h.
#pragma once

//  D2Client 0x6FB6EE00
//  The cel numbers AUTOMAP_AddTown does not plot for the act 2 town, -1 terminated. The count starts at 0 or at 20 (the town has more than one
//  preset .ds1), so the entries are two sets of holes, {0, 10, 15, 16, 19} and {20, 25, 30, 35, 36, 39}, that are not the same holes shifted.
//  Levels 103 and 109 pass NULL and plot every cell.
inline const int32_t gaAutomapTownSkipCels[12] = { 0, 10, 15, 16, 19, 20, 25, 30, 35, 36, 39, -1 };
static_assert(sizeof(gaAutomapTownSkipCels) == 48, "gaAutomapTownSkipCels: the item's 48 bytes in the .i64");

//  D2Client 0x6FB6EE30
//  The party-marker shape AUTOMAP_DrawSymbol and AUTOMAP_DrawPartyMember draw: 13 points as x/y pairs, 26 ints. The loop runs i = 0; i < 24;
//  i += 2 and draws D2GFX_DrawLine(a[i], a[i+1], a[i+2], a[i+3]) with every coordinate doubled -- twelve segments of one polyline, each starting
//  where the last ended.
inline const int32_t gaAutomapPartyMarker[26] = { 0, -1, 2, -2, 4, -1, 2, 0, 4, 1, 2, 2, 0, 1, -2, 2, -4, 1, -2, 0, -4, -1, -2, -2, 0, -1 };
static_assert(sizeof(gaAutomapPartyMarker) == 104, "gaAutomapPartyMarker: the item's 104 bytes in the .i64");

//  D2Client 0x6FB77A38
//  49 { cell type, group } pairs in 18 groups, the sparse source for gaAutomapCellGroup. The keys are the automap cell's type word (at +4 of the
//  cell), not Objects.txt ids.
inline int32_t gaAutomapCellGroupPairs[98] =
{
    0,
    0,
    1,
    0,
    2,
    0,
    3,
    0,
    6,
    1,
    7,
    1,
    8,
    1,
    11,
    2,
    12,
    2,
    13,
    3,
    14,
    3,
    20,
    4,
    38,
    4,
    21,
    5,
    39,
    5,
    46,
    6,
    47,
    6,
    48,
    6,
    49,
    6,
    51,
    7,
    52,
    7,
    53,
    7,
    54,
    7,
    60,
    8,
    70,
    8,
    61,
    9,
    71,
    9,
    120,
    10,
    169,
    10,
    171,
    10,
    121,
    11,
    170,
    11,
    172,
    11,
    257,
    12,
    258,
    12,
    259,
    12,
    266,
    13,
    267,
    13,
    337,
    14,
    338,
    14,
    472,
    15,
    473,
    15,
    474,
    15,
    475,
    15,
    520,
    16,
    521,
    16,
    522,
    16,
    533,
    17,
    534,
    17,
};
static_assert(sizeof(gaAutomapCellGroupPairs) == 392, "gaAutomapCellGroupPairs: the item's 392 bytes in the .i64");

//  D2Client 0x6FB77BC0
//  The automap's scale divisor: client coordinates are divided by it to get map pixels, so a larger value means zoomed out. Only 10 and 20 are ever
//  written (by AUTOMAP_Draw and AUTOMAP_SetMode), and the divisions by it are signed.
inline int32_t gnAutomapZoom = 10;
static_assert(sizeof(gnAutomapZoom) == 4, "gnAutomapZoom: the item's 4 bytes in the .i64");

//  D2Client 0x6FB77BC4
//  The "AutoMap Centers" registry setting, default 1: whether the map recentres on the player (read by AUTOMAP_RecomputeCenterOffsets).
inline BOOL gbAutomapCenters = 1;
static_assert(sizeof(gbAutomapCenters) == 4, "gbAutomapCenters: the item's 4 bytes in the .i64");

//  D2Client 0x6FB77BC8
//  The "AutoMap Party" registry setting, default 1: show party members on the automap. The setter writes the registry before the global, so the
//  two cannot drift. Read through AUTOMAP_GetShowParty and directly by AUTOMAP_DrawUnits and AUTOMAP_DrawUnit.
inline BOOL gbAutomapShowParty = 1;
static_assert(sizeof(gbAutomapShowParty) == 4, "gbAutomapShowParty: the item's 4 bytes in the .i64");

//  D2Client 0x6FB77BCC
//  The "AutoMap Party Names" registry setting, default 1: draw party members' names. It gates only the name, not the marker.
//  ESCMENU_AutomapPartyNamesRefresh requires both this and gbAutomapShowParty, so the names row in the options panel is dead while party display
//  is off.
inline BOOL gbAutomapPartyNames = 1;
static_assert(sizeof(gbAutomapPartyNames) == 4, "gbAutomapPartyNames: the item's 4 bytes in the .i64");

//  D2Client 0x6FB77BDC
//  The registry value name "AutoMap Centers", pushed by AUTOMAP_LoadSettings and AUTOMAP_SetCentersAndSave.
inline char szValueName[16] = "AutoMap Centers";
static_assert(sizeof(szValueName) == 16, "szValueName: the item's 16 bytes in the .i64");

//  D2Client 0x6FB77C5C
//  The format "%s%s.ma%d" of the explored-map data file names. AUTOMAP_SaveExploredMap's four sprintf calls use it; the original pushes it three
//  times, because the arm that finds the game's id already in the index jumps into the function's own tail.
inline char Format[10] = "%s%s.ma%d";
static_assert(sizeof(Format) == 10, "Format: the item's 10 bytes in the .i64");

//  D2Client 0x6FBAF990
//  2048 entries, default -1, mapping an automap cell's type (the word at +4 of the cell) to a group id 0..17; AUTOMAP_LoadSettings fills 49 of them
//  from gaAutomapCellGroupPairs. AUTOMAP_RebalanceTree reads it once the x and y comparisons tie: a group of -1, or a group equal to the root's,
//  abandons the insertion (the root is written back and 0 returned, so the cell never enters the tree); only differing groups order by the raw
//  cell type.
inline int32_t gaAutomapCellGroup[2048];
static_assert(sizeof(gaAutomapCellGroup) == 8192, "gaAutomapCellGroup: the item's 8192 bytes in the .i64");

//  D2Client 0x6FBB1990
//  The automap display mode: 1 is the mini-map (zoom 20 instead of 10, its own pan origin and its own icon cels), anything else the full screen.
//  In mode 1 fade setting 1 is not representable (see AUTOMAP_GetFade). Read only by the automap and the ESC menu.
inline int32_t gnAutomapMode;
static_assert(sizeof(gnAutomapMode) == 4, "gnAutomapMode: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1994
//  Write cursor into the draw buffer AUTOMAP_RebuildDrawLists is currently filling: set to each FOG_Alloc result in turn (one buffer per cell tree)
//  and advanced six bytes per cell by AUTOMAP_WriteCellTree. It means nothing between rebuilds.
inline uint8_t *gpAutomapDrawWriteCursor;
static_assert(sizeof(gpAutomapDrawWriteCursor) == 4, "gpAutomapDrawWriteCursor: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1998
//  Head of the cell block pool, a singly linked list of 10244-byte blocks (512 cells of 20 bytes, then a next pointer at +10240) allocated by
//  AUTOMAP_AllocCell. AUTOMAP_Shutdown, AUTOMAP_Tick, AUTOMAP_RebuildAutomapLayer and AUTOMAP_AddTown free the chain and zero this.
inline D2AutomapCellBlockStrc *gpAutomapCellBlocks;
static_assert(sizeof(gpAutomapCellBlocks) == 4, "gpAutomapCellBlocks: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB199C
//  How many cells have been handed out: AUTOMAP_AllocCell increments it on every call and uses it as the flat index into the pool (/ 512 selects the
//  block, % 512 the slot). A high-water mark, not a live count: nothing decrements it, and AUTOMAP_AddTown (twice), AUTOMAP_RebuildAutomapLayer
//  (twice) and AUTOMAP_Tick zero it beside a free of the chain.
inline int32_t gnAutomapCellsUsed;
static_assert(sizeof(gnAutomapCellsUsed) == 4, "gnAutomapCellsUsed: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB19A0
//  Head of the per-layer record list, one record per distinct dwLayer the player has visited. A layer record is 28 bytes, zeroed when allocated:
//      +0   dwLayer, the key      +4   fSaved: 1, 2 or 3 for a town layer, written by AUTOMAP_AddTown; AUTOMAP_Draw's switches read it
//      +8   floor cell tree       +12  wall cell tree
//      +16  third cell tree       +20  fourth cell tree
//      +24  next
//  The four tree fields are roots, not cel bases: AUTOMAP_AddRoom passes the address of the first two into AUTOMAP_AddTile, which inserts there.
//  AUTOMAP_Tick looks the current level's layer up by walking next and allocates a record when it misses, pushing it on the front.
inline D2AutomapLayerStrc *gpAutomapLayers;
static_assert(sizeof(gpAutomapLayers) == 4, "gpAutomapLayers: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB19A4
//  The layer record the player is on (see gpAutomapLayers). Its four cell lists, +0x08..+0x14, are cleared together at six sites: AUTOMAP_Tick,
//  AUTOMAP_Shutdown, and two each in AUTOMAP_AddTown and AUTOMAP_RebuildAutomapLayer.
inline D2AutomapLayerStrc *gpAutomapCurrentLayer;
static_assert(sizeof(gpAutomapCurrentLayer) == 4, "gpAutomapCurrentLayer: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB19A8
//  The four automap icon cel files, loaded by AUTOMAP_LoadIconCels: [0] MaxiMap, [1] Act2Map, [2] Act4Map, [3] ExTnMap. In mode 1 (the mini-map) it
//  loads the ...S variants instead (MaxiMapS, Act2MapS, Act4MapS, ExTnMapS) with no expansion test; otherwise [3] is loaded only when
//  FOG_IsExpansion() and is zeroed, with its count, in a classic game.
inline D2CellFileStrc *gpAutomapIconCels[4];
static_assert(sizeof(gpAutomapIconCels) == 16, "gpAutomapIconCels: the item's 16 bytes in the .i64");

//  D2Client 0x6FBB19B8
//  Cels per direction of each gpAutomapIconCels entry, filled by AUTOMAP_LoadIconCels with D2CMP_CELCMP_CelFileGetCelsPerDirection right
//  after each load and read by AUTOMAP_LoadCelData. Entry [3] is 0 in a classic game outside mode 1, where ExTnMap is never loaded.
inline int32_t gnAutomapIconCelCounts[4];
static_assert(sizeof(gnAutomapIconCelCounts) == 16, "gnAutomapIconCelCounts: the item's 16 bytes in the .i64");

//  D2Client 0x6FBB19C8
//  How far the player has scrolled the automap sideways from centre. AUTOMAP_OnArrowLeft and AUTOMAP_OnArrowRight move it by 16 pixels a press,
//  AUTOMAP_RecomputeCenterOffsets zeroes or recentres it, and AUTOMAP_UpdateClipRect recomputes it for the mini-map as
//  gnScreenWidth / 3 - nLeft - 16. AUTOMAP_Draw folds it into gnAutomapOriginX.
inline int32_t gnAutomapPanX;
static_assert(sizeof(gnAutomapPanX) == 4, "gnAutomapPanX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB19CC
//  The vertical half of the pan, moved 16 pixels a press by AUTOMAP_OnArrowUp and AUTOMAP_OnArrowDown; the same writers and role as gnAutomapPanX.
inline int32_t gnAutomapPanY;
static_assert(sizeof(gnAutomapPanY) == 4, "gnAutomapPanY: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB19D0
//  The player's position in map pixels less half the viewport: the scroll origin before the pan is added. Written once, in AUTOMAP_Draw, and read
//  by nothing -- the value is folded into gnAutomapOriginX from the same register.
inline int32_t gnAutomapScrollX;
static_assert(sizeof(gnAutomapScrollX) == 4, "gnAutomapScrollX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB19D4
//  The vertical counterpart of gnAutomapScrollX and dead in the same way: one write in AUTOMAP_Draw, no readers.
inline int32_t gnAutomapScrollY;
static_assert(sizeof(gnAutomapScrollY) == 4, "gnAutomapScrollY: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB19D8
//  The x origin: gnAutomapPanX + gnAutomapScrollX + 40. AUTOMAP_DrawUnits and AUTOMAP_DrawPartyMember place a point as
//  nX / gnAutomapZoom - gnAutomapOriginX + 8; AUTOMAP_DrawCellList as 10 * xPixel / gnAutomapZoom - gnAutomapOriginX, with no +8 (and y with no -8).
//  The +40 here and the +15 in gnAutomapOriginY are the fixed offset of the map area itself.
inline int32_t gnAutomapOriginX;
static_assert(sizeof(gnAutomapOriginX) == 4, "gnAutomapOriginX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB19DC
//  The y origin: gnAutomapPanY + gnAutomapScrollY + 15. AUTOMAP_DrawUnits and AUTOMAP_DrawPartyMember subtract it and then apply -8 where the x path
//  applies +8 (the asymmetry is the original's); AUTOMAP_DrawCellList applies neither.
inline int32_t gnAutomapOriginY;
static_assert(sizeof(gnAutomapOriginY) == 4, "gnAutomapOriginY: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB19E0
//  AUTOMAP_AddRoom adds a floor or wall tile when its flag 0x20000 is set, or this is non-zero, or the caller passed bRevealAll (a tile with bit 0x8
//  is skipped first). It lies past .data's raw size, so the loader zero-fills it and, with no writer, it stays 0: the per-tile test is live.
//  Three references, all reads, in AUTOMAP_AddRoom.
inline BOOL gbAutomapRevealAll;
static_assert(sizeof(gbAutomapRevealAll) == 4, "gbAutomapRevealAll: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB19E4
//  Countdown of ticks before AUTOMAP_Tick reveals anything: Tick decrements it and does nothing else while it is non-zero. AUTOMAP_DelayReveal,
//  whose only caller is CUNIT_WarpUnit, sets it to 2, so after a warp the tick skips twice and then runs every frame.
inline int32_t gnAutomapRevealDelay;
static_assert(sizeof(gnAutomapRevealDelay) == 4, "gnAutomapRevealDelay: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB19E8
//  The "AutoMapFade" registry setting, default 0 (AUTOMAP_LoadSettings' preset; the other four settings default to 1). Its get/set pair is
//  AUTOMAP_GetFade and AUTOMAP_SetFade, which command 9 (CMDTBL_ActionFadeAutomap) cycles through 0..3 with (AUTOMAP_GetFade() + 1) % 4;
//  AUTOMAP_DrawCellList reads it too.
inline int32_t gnAutomapFade;
static_assert(sizeof(gnAutomapFade) == 4, "gnAutomapFade: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB19EC
//  The fade setting as last requested, kept beside gnAutomapFade so that a fade of 1 can toggle. AUTOMAP_SetFade writes the same value to both;
//  gnAutomapFade is overwritten by whoever resolves it, this is not. Fade 1 is not representable in mini-map mode (gnAutomapMode == 1) and
//  resolves to this != 0 ? 0 : 2; AUTOMAP_SetFade on write, AUTOMAP_GetFade on read and AUTOMAP_DrawCellList carry that same expression.
//  Compare gnAutomapLeftSaved, the same pattern for the AutoMap Left setting.
inline int32_t gnAutomapFadeSaved;
static_assert(sizeof(gnAutomapFadeSaved) == 4, "gnAutomapFadeSaved: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB19F0
//  The automap's eleven symbol colours, resolved once by AUTOMAP_InitSymbolColors through D2WIN_10034_MixRGB into palette indices:
//      [0] 00 00 FF   [1] FF 00 00   [2] FF 00 FF   [3] 00 FF 00
//      [4] 44 70 74   [5] 48 A0 34   [6] F4 F4 F4   [7] 80 80 80
//      [8] F4 F4 00   [9] 00 00 FF   [10] 00 FF 00
//  [9] repeats [0] and [10] repeats [3], so eleven slots carry nine distinct colours. 11 bytes; one padding byte follows before gnAutomapTextY.
//  Read by AUTOMAP_DrawUnit, AUTOMAP_DrawUnits and AUTOMAP_DrawPartyMember.
inline uint8_t gAutomapSymbolColors[11];
static_assert(sizeof(gAutomapSymbolColors) == 11, "gAutomapSymbolColors: the item's 11 bytes in the .i64");

//  D2Client 0x6FBB19FC
//  The running y cursor of the automap's text column. AUTOMAP_Draw seeds it at 24 and each of the six corner-label drawers reads it, draws its
//  line and advances it by 16 (AUTOMAP_DrawGameInfo twice), so the lines stack in call order.
inline int32_t gnAutomapTextY;
static_assert(sizeof(gnAutomapTextY) == 4, "gnAutomapTextY: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1A00
//  A second copy of gnAutomapMiniLeft, written by AUTOMAP_UpdateClipRect on the same line as the real one and never read.
inline int32_t gnAutomapMiniLeftUnused;
static_assert(sizeof(gnAutomapMiniLeftUnused) == 4, "gnAutomapMiniLeftUnused: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1A04
//  A second copy of gnAutomapMiniTop, written beside gnAutomapMiniLeftUnused and never read.
inline int32_t gnAutomapMiniTopUnused;
static_assert(sizeof(gnAutomapMiniTopUnused) == 4, "gnAutomapMiniTopUnused: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1A08
//  Left edge of the automap's clip rectangle: -16 in full-screen mode, so a marker centred just off the left edge still draws the half that
//  overhangs; the mini-map panel's left minus 8 in mini-map mode. Set by AUTOMAP_UpdateClipRect; read by AUTOMAP_DrawUnits and
//  AUTOMAP_DrawPartyMember.
inline int32_t gnAutomapClipMinX;
static_assert(sizeof(gnAutomapClipMinX) == 4, "gnAutomapClipMinX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1A0C
//  Top edge of the clip rectangle: -16 in full-screen mode, the mini-map panel's top otherwise.
inline int32_t gnAutomapClipMinY;
static_assert(sizeof(gnAutomapClipMinY) == 4, "gnAutomapClipMinY: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1A10
//  Right edge of the clip rectangle: gnScreenWidth in full-screen mode, nLeft + gnScreenWidth / 3 in mini-map mode.
inline int32_t gnAutomapClipMaxX;
static_assert(sizeof(gnAutomapClipMaxX) == 4, "gnAutomapClipMaxX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1A14
//  Bottom edge of the clip rectangle: gnScreenHeight in full-screen mode, nTop + gnScreenHeight / 3 in mini-map mode.
inline int32_t gnAutomapClipMaxY;
static_assert(sizeof(gnAutomapClipMaxY) == 4, "gnAutomapClipMaxY: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1A18
//  Top edge of the mini-map, in screen pixels: 78 when it is pinned right, 0x60 or 0 when pinned left. The 0x60 is the party-portrait allowance:
//  HIREABLES_PanelPortraitsReserveTopStrip() is gnPartyPortraitMode != 2, and with the portraits up a left-hand mini-map starts 96 pixels lower so
//  it does not sit on them. Read back by AUTOMAP_RecomputeCenterOffsets as gnAutomapPanY = gnScreenHeight / 3 - this - 16.
inline int32_t gnAutomapMiniTop;
static_assert(sizeof(gnAutomapMiniTop) == 4, "gnAutomapMiniTop: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1A1C
//  Left edge of the mini-map, in screen pixels: 2 * gnScreenWidth / 3 when it is pinned right, 0 when pinned left (see AUTOMAP_UpdateClipRect).
//  AUTOMAP_LoadSettings presets AutoMap Left to 1, so with no registry value the mini-map is pinned left. Read back by
//  AUTOMAP_RecomputeCenterOffsets as gnAutomapPanX = gnScreenWidth / 3 - this - 16, and by AUTOMAP_Draw and AUTOMAP_SetMode.
inline int32_t gnAutomapMiniLeft;
static_assert(sizeof(gnAutomapMiniLeft) == 4, "gnAutomapMiniLeft: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1A20
//  The live "AutoMap Left" value, which side of the screen the mini-map sits on. Read by AUTOMAP_UpdateClipRect and AUTOMAP_Draw, toggled by
//  CMDTBL_ActionToggleAutomapLeft. AUTOMAP_Draw overwrites it temporarily when the UI is shifted, stashing the old value in gnAutomapLeftSaved,
//  which is why three globals hold one setting.
inline BOOL gbAutomapLeft;
static_assert(sizeof(gbAutomapLeft) == 4, "gbAutomapLeft: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1A24
//  Where AUTOMAP_Draw stashes gbAutomapLeft while the UI shift forces the map to one side, and what it restores from afterwards. Loaded with the same
//  registry value at startup so the first restore has something sane to use.
inline int32_t gnAutomapLeftSaved;
static_assert(sizeof(gnAutomapLeftSaved) == 4, "gnAutomapLeftSaved: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1A28
//  The user's "AutoMap Left" preference, the third of the three copies. In AUTOMAP_Draw's third arm (no UI shift) the live value is first put back
//  from gnAutomapLeftSaved and then set to this whenever the two differ, so that arm always leaves gbAutomapLeft equal to this.
inline int32_t gnAutomapLeftSetting;
static_assert(sizeof(gnAutomapLeftSetting) == 4, "gnAutomapLeftSetting: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1A2C
//  The party-portrait state the clip rectangle was last computed for. AUTOMAP_UpdateClipRect compares HIREABLES_PanelPortraitsReserveTopStrip()
//  against it and, in mini-map mode only, latches the new value and re-enters itself so the rectangle is rebuilt.
inline int32_t gnAutomapClipLatchPortraits;
static_assert(sizeof(gnAutomapClipLatchPortraits) == 4, "gnAutomapClipLatchPortraits: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1A30
//  The gbAutomapLeft setting the clip rectangle was last computed for: the same change-detect latch as gnAutomapClipLatchPortraits for the other
//  input, likewise acted on in mini-map mode only. The self-call after either latch is a real call (at 0x6FACB7A2, with code after it), so the
//  recursion is at most two deep, one per latch.
inline int32_t gnAutomapClipLatchLeft;
static_assert(sizeof(gnAutomapClipLatchLeft) == 4, "gnAutomapClipLatchLeft: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1A34
//  The player's client X at the last reveal. AUTOMAP_Tick only walks the room list once the player has moved far enough from here:
//      dx = |this - x|,  dy = |gnAutomapLastRevealY - y|
//      d  = (dx <= dy ? dx + 2 * dy : dy + 2 * dx) / 2   and d >= 80
//  the usual integer approximation to a distance (the larger axis weighted double), not a real radius.
inline int32_t gnAutomapLastRevealX;
static_assert(sizeof(gnAutomapLastRevealX) == 4, "gnAutomapLastRevealX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1A38
//  The player's client Y at the last reveal; see gnAutomapLastRevealX for the distance test that gates the next one. Both are updated only when
//  that test passes.
inline int32_t gnAutomapLastRevealY;
static_assert(sizeof(gnAutomapLastRevealY) == 4, "gnAutomapLastRevealY: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1A3C
//  The player's client X this tick, from UNITS_GetClientCoordX. Written and read once each, inside AUTOMAP_Tick, as the left side of the distance
//  test against gnAutomapLastRevealX.
inline int32_t gnAutomapPlayerX;
static_assert(sizeof(gnAutomapPlayerX) == 4, "gnAutomapPlayerX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB1A40
//  The player's client Y this tick, from UNITS_GetClientCoordY, and never read: its only reference is that one write. AUTOMAP_Tick keeps the Y it
//  needs in a local and uses that for the distance test.
inline int32_t gnAutomapPlayerYUnused;
static_assert(sizeof(gnAutomapPlayerYUnused) == 4, "gnAutomapPlayerYUnused: the item's 4 bytes in the .i64");

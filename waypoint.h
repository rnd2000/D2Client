//  waypoint.h -- the globals of waypoint.cpp: the act table and its count, the coordinate table, the panel state and the nine-row destination
//  list (0x6FB90778..0x6FBBA9FD). Each is a C++17 inline variable, so every file that includes this header shares one object. The thirteen from
//  0x6FBBA9A4 on lie past the end of .data's raw data (0x6FB9B000): the loader zero-fills them, and ours are zero-initialised.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB90778
//  One 11-byte record per act for WAYPOINT_BuildActWaypointList: the act's level id range (the first and last level its counter runs over)
//  and the cached waypoint-number range. Levels 39 and 108 fall in no act's range -- they are the transitions. The five records are 55 bytes
//  and one alignment byte follows before gnWaypointActCount.
inline D2ClientWaypointActStrc gWaypointActTable[5] =
{
    { .nFirstLevelId = 1, .nLastLevelId = 38, .nFirstWaypointNo = 255 },   // [0]
    { .nFirstLevelId = 40, .nLastLevelId = 74, .nFirstWaypointNo = 255 },   // [1]
    { .nFirstLevelId = 75, .nLastLevelId = 102, .nFirstWaypointNo = 255 },   // [2]
    { .nFirstLevelId = 103, .nLastLevelId = 107, .nFirstWaypointNo = 255 },   // [3]
    { .nFirstLevelId = 109, .nLastLevelId = 132, .nFirstWaypointNo = 255 },   // [4]
};
static_assert(sizeof(gWaypointActTable) == 55, "gWaypointActTable: the item's 55 bytes in the .i64");

//  D2Client 0x6FB907B0
//  How many act tabs the waypoint panel may show: the constant 5, read once, in WAYPOINT_ClampActTab (if (nAct >= gnWaypointActCount) nAct = 0).
//  It bounds the tab from above only (5 and up are zeroed); the two callers give 0..4. Nothing writes it.
inline int32_t gnWaypointActCount = 5;
static_assert(sizeof(gnWaypointActCount) == 4, "gnWaypointActCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FB907B8
//  D2WaypointCoordTableStrc { nX, nY, nTextX, nTextY, nClickX, nClickY }: six int32, 24 bytes per entry, the panel-space layout of the nine
//  destination rows. The hit test reads the click pair (nClickX, nClickY); the draw pass reads the first two pairs.
inline D2WaypointCoordTableStrc gWaypointCoords[9] =
{
    { .nX = 17, .nY = 89, .nTextX = 80, .nTextY = 84, .nClickX = 17, .nClickY = 60 },   // [0]
    { .nX = 17, .nY = 125, .nTextX = 80, .nTextY = 119, .nClickX = 17, .nClickY = 96 },   // [1]
    { .nX = 17, .nY = 161, .nTextX = 80, .nTextY = 154, .nClickX = 17, .nClickY = 132 },   // [2]
    { .nX = 17, .nY = 197, .nTextX = 80, .nTextY = 189, .nClickX = 17, .nClickY = 168 },   // [3]
    { .nX = 17, .nY = 234, .nTextX = 80, .nTextY = 224, .nClickX = 17, .nClickY = 205 },   // [4]
    { .nX = 17, .nY = 270, .nTextX = 80, .nTextY = 259, .nClickX = 17, .nClickY = 241 },   // [5]
    { .nX = 17, .nY = 306, .nTextX = 80, .nTextY = 294, .nClickX = 17, .nClickY = 277 },   // [6]
    { .nX = 17, .nY = 342, .nTextX = 80, .nTextY = 329, .nClickX = 17, .nClickY = 313 },   // [7]
    { .nX = 17, .nY = 378, .nTextX = 80, .nTextY = 364, .nClickX = 17, .nClickY = 349 },   // [8]
};
static_assert(sizeof(gWaypointCoords) == 216, "gWaypointCoords: the item's 216 bytes in the .i64");

//  D2Client 0x6FBBA9A4
//  Set while the mouse is held on the waypoint panel's cancel button, so the release handler knows the press started there.
inline uint8_t gbWaypointCancelDown;
static_assert(sizeof(gbWaypointCancelDown) == 1, "gbWaypointCancelDown: the item's 1 bytes in the .i64");

//  D2Client 0x6FBBA9A5
//  Index of the destination row under the cursor, or -1 for none. Indexes gWaypointList / gWaypointCoords.
inline int32_t gnWaypointHoverIndex;
static_assert(sizeof(gnWaypointHoverIndex) == 4, "gnWaypointHoverIndex: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA9A9
//  Handle for %s\ui\menu\waygatebackground. Unloaded and nulled by WAYPOINT_ClosePanel.
inline D2CellFileStrc *gpWaypointBackgroundImage;
static_assert(sizeof(gpWaypointBackgroundImage) == 4, "gpWaypointBackgroundImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA9AD
//  Handle for ui\menu\waygatetabs, or ui\menu\expwaygatetabs in an expansion game -- the act tab strip along the top.
inline D2CellFileStrc *gpWaypointTabsImage;
static_assert(sizeof(gpWaypointTabsImage) == 4, "gpWaypointTabsImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA9B1
//  Handle for ui\menu\waygateicons -- the per-destination icons.
inline D2CellFileStrc *gpWaypointIconsImage;
static_assert(sizeof(gpWaypointIconsImage) == 4, "gpWaypointIconsImage: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA9B5
//  GUID of the waypoint object being used, taken from field +1 of server packet 0x63 and sent back in every client 0x49.
inline int32_t gnWaypointUnitGUID;
static_assert(sizeof(gnWaypointUnitGUID) == 4, "gnWaypointUnitGUID: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA9B9
//  The waypoint panel's allocated data, written by WAYPOINT_AllocPanelData and cleared by WAYPOINT_ClosePanel; read by
//  WAYPOINT_BuildActWaypointList, WAYPOINT_ClosePanel and WAYPOINT_OnOpenPacket. The address is unaligned.
inline void *gpWaypointPanelData;
static_assert(sizeof(gpWaypointPanelData) == 4, "gpWaypointPanelData: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA9BD
//  Guards the close packet so it goes exactly once per panel session. Cleared when the panel opens (WAYPOINT_OnOpenPacket and
//  WAYPOINT_AllocPanelData) and by WAYPOINT_BuildActWaypointList. Set by the six places that send client 0x49 with level 0 -- WAYPOINT_ClosePanel,
//  WAYPOINT_DrawPanel, WAYPOINT_OnChar, WAYPOINT_OnMouseDown, WAYPOINT_OnMouseUp, WAYPOINT_SendCloseOnce -- each behind its own test of this
//  flag, and by WAYPOINT_OnMouseUp's travel send, whose level is the chosen row's.
inline uint8_t gbWaypointCloseSent;
static_assert(sizeof(gbWaypointCloseSent) == 1, "gbWaypointCloseSent: the item's 1 bytes in the .i64");

//  D2Client 0x6FBBA9BE
//  The act tab on show. WAYPOINT_ClampActTab is the only writer (it clamps against gnWaypointActCount and walks the act-unlock cascade), so
//  the stored value is never the raw tab the cursor was over; WAYPOINT_OnMouseDown compares the raw tab with it to decide whether to play
//  sound 6. A raw tab of 0 is stored as it is: the quest tests run for 1..4 only.
inline int32_t gnWaypointActTab;
static_assert(sizeof(gnWaypointActTab) == 4, "gnWaypointActTab: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA9C2
//  How many rows WAYPOINT_BuildActWaypointList has filled into gWaypointList for the current act -- the running write index, and afterwards
//  the row count.
inline int32_t gnWaypointRowCount;
static_assert(sizeof(gnWaypointRowCount) == 4, "gnWaypointRowCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA9C6
//  Counts the activated destinations of this act excluding the level the player is standing in: the builder takes the current level down a
//  branch that stores the row with bActivated = 0 and does not increment this. Hence tbl 3991, "No Other Waypoints Activated", appears for a
//  player alone on their act's first waypoint.
inline int32_t gnWaypointOtherActivatedCount;
static_assert(sizeof(gnWaypointOtherActivatedCount) == 4, "gnWaypointOtherActivatedCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA9CC
//  The nine destinations of the act on screen, rebuilt by WAYPOINT_BuildActWaypointList. D2WaypointTableStrc { int32 nLevelId; bool bActivated },
//  pack(1): 5 bytes, 9 entries, 0x6FBBA9CC..0x6FBBA9F9. The original's walks start at &gWaypointList[0].bActivated, four bytes into record 0;
//  the builder's clear loop (a dword behind the cursor and a byte at it) shows that the record begins four bytes earlier.
inline D2WaypointTableStrc gWaypointList[9];
static_assert(sizeof(gWaypointList) == 45, "gWaypointList: the item's 45 bytes in the .i64");

//  D2Client 0x6FBBA9FD
//  The original's end address for the walks over gWaypointList's bActivated bytes, which start at +4: &gWaypointList[9].bActivated, four bytes
//  past the list's end (0x6FBBA9F9). Not a variable. waypoint.cpp computes the bound from gWaypointList with an integer cursor.
inline char gWaypointListEnd[3];
static_assert(sizeof(gWaypointListEnd) == 3, "gWaypointListEnd: the item's 3 bytes in the .i64");

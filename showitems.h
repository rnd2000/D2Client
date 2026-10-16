//  showitems.h -- the globals of showitems.cpp that are ours: the 32 ground-item label records and their count. Each is a C++17 inline variable, so
//  every file that includes this header shares one object. Both lie past .data's raw data (it ends at 0x6FB9B000): the loader zero-fills them, and
//  ours are zero-initialised. The original's static initialiser (CRT_StaticInit15) runs the Unicode constructor over each record's 128-character
//  text at +0x14, 32 records; ours does the same by its declaration, gShowItemLabels being an array of a struct with that member.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FBB7028
//  The ground-item label records: 32 records of 288 bytes. The first four dwords of a record are its rectangle (left, top, right, bottom at +0,
//  +4, +8, +12). SHOWITEMS_DrawGroundItemLabels fills the array each frame and SHOWITEMS_FitLabelRect walks it to keep new labels from
//  overlapping the ones already placed.
inline D2ClientShowItemLabelStrc gShowItemLabels[32];
static_assert(sizeof(gShowItemLabels) == 9216, "gShowItemLabels: the item's 9216 bytes in the .i64");

//  D2Client 0x6FBB9428
//  How many ground-item label rectangles are currently in gShowItemLabels. Reset to 0 by SHOWITEMS_DrawGroundItemLabels every frame and asserted
//  below 32 by SHOWITEMS_FitLabelRect (sgnNumShowItems < MAX_SHOW_ITEMS, showitems.cpp:71).
inline int32_t gnNumShowItems;
static_assert(sizeof(gnNumShowItems) == 4, "gnNumShowItems: the item's 4 bytes in the .i64");

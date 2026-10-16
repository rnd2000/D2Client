//  PalShift.h -- the globals of PalShift.cpp that are ours: the monster palette-shift tables, the random-transform table and the green-blood remap.
//  Each is a C++17 inline variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above a global gives
//  its address in the original. All five lie past .data's raw data (it ends at 0x6FB9B000), so they are zero at load and ours are zero-initialised.
//  A "PalShift.cpp:<n>" in a comment below is a line of the original's source, the one its FOG_Alloc and FOG_Free calls are handed (99, 138, 132,
//  144), not a line of our PalShift.cpp.
//  Include it after D2Client.h.
#pragma once

//  D2Client 0x6FBA7C10
//  One 8-byte record per monstats.txt record, allocated by PALSHIFT_Init (PalShift.cpp:99) and freed at :138. Field +0 is seeded with the record's
//  own index (an identity mapping); field +4 is left null until PALSHIFT_EnsureMonsterPalette loads the class's palshift.dat. That pointer is what
//  PALSHIFT_GetUnitShiftTable tests: without it every palette shift of the class answers NULL.
inline D2ClientMonPalShiftStrc *gpMonPalShift;
static_assert(sizeof(gpMonPalShift) == 4, "gpMonPalShift: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7C14
//  Head of the on-demand cache of loaded monster palette-shift files, emptied by PALSHIFT_Init, filled by PALSHIFT_EnsureMonsterPalette and freed at
//  PalShift.cpp:132. Each node is a D2ClientPalShiftStrc of 4104 bytes: nShift at +4 (palshift.dat as read, eight 256-colour levels), nGreenShift
//  at +0x804 (its GreenBlood.dat copy) and the next pointer at +4100.
inline D2ClientPalShiftStrc *gpMonPalShiftCache;
static_assert(sizeof(gpMonPalShiftCache) == 4, "gpMonPalShiftCache: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA7C18
//  The 7680 bytes PALSHIFT_Init reads out of Data\Global\Monsters\RandTransforms.dat: 30 tables of 256 colours (PALSHIFT_GetUnitShiftTable asserts
//  eTransform >= 0 && eTransform < NUM_RANDTABLES). PALSHIFT_GetUnitShiftTable's `add eax, offset` addresses it as a base (a row is
//  gRandTransformTables[nTransform] here). Valid only once PALSHIFT_Init has read the file.
inline uint8_t gRandTransformTables[30][256];
static_assert(sizeof(gRandTransformTables) == 7680, "gRandTransformTables: the item's 7680 bytes in the .i64");

//  D2Client 0x6FBA9A18
//  Set to 1 by PALSHIFT_Init only when RandTransforms.dat opened and was read into gRandTransformTables. PALSHIFT_GetUnitShiftTable reads it in the
//  default arm of its jump table, which switches on the unit's palette shift: a shift of 8 and up takes RandTransforms table `shift - 8` while this
//  is set, and falls into the 2..7 arm while it is clear.
inline int32_t gbRandTransformsLoaded;
static_assert(sizeof(gbRandTransformsLoaded) == 4, "gbRandTransformsLoaded: the item's 4 bytes in the .i64");

//  D2Client 0x6FBA9A1C
//  The 256-entry colour remap loaded from Data\Global\Monsters\GreenBlood.dat by PALSHIFT_Init and freed at PalShift.cpp:144.
//  PALSHIFT_ApplyGreenBlood runs a monster's palette through it when monstats2.txt LocalBlood is 2, leaving a byte unchanged where the remap entry
//  is zero.
inline uint8_t *gpGreenBloodRemap;
static_assert(sizeof(gpGreenBloodRemap) == 4, "gpGreenBloodRemap: the item's 4 bytes in the .i64");

//  MemoryPartitions.h -- the global of MemoryPartitions.cpp that is ours: gfBytesToMegabytes, a C++17 `inline` variable, one object for every file
//  that includes this header. The comment line "D2Client 0x6F..." above it gives its address in the original. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB6E0BC
//  2^-20 exactly (9.5367431640625e-07), bytes becoming megabytes: MEMPART_InitMemoryPartitions multiplies by it (`fmul` at 0x6FAA13EB), its only
//  reference. The item is .rdata, so it is const.
inline const float gfBytesToMegabytes = 9.5367431640625e-07f;
static_assert(sizeof(gfBytesToMegabytes) == 4, "gfBytesToMegabytes: the item's 4 bytes in the .i64");

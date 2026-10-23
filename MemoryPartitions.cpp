#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "MemoryPartitions.h"

//  MemoryPartitions.cpp -- the memory partitioning of D2Client.dll 1.10: MEMPART_InitMemoryPartitions divides physical memory between the sound,
//  tile and sprite caches and refuses to start on a machine with too little, MEMPART_DestroyCaches drops the caches, and MEMPART_Nullsub1 is an
//  empty function. The original's source file is D2Client\Core\MemoryPartitions.cpp (the assert at 0x6FAA13B5 in MEMPART_InitMemoryPartitions
//  names it; it is the only function that carries it). Address range 0x6FAA13A0..0x6FAA1500, three functions. The one global, gfBytesToMegabytes,
//  is in MemoryPartitions.h.

//  The source file, as the assert at 0x6FAA13B5 hands it over.
static const char szSourceFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\Core\\MemoryPartitions.cpp";

//1.10f: D2Client.0x6FAA13A0
//  Divides physical memory between the sound, tile and sprite caches and refuses to start on a machine with too little. Its one caller passes
//  ecx = ghClientPoolSystem and edx = 1. The last statement, GFX_InitCelCache, is a call (a tail jump in the original).
//  Both divisions are UNSIGNED (`mul` against a magic reciprocal: 0x51EB851F >> 5 for /100, 0xAAAAAAAB >> 1 for /3, never `idiv`), both clamps are
//  SIGNED (`jle` and `jl`), and both results are shifted right with `shr`, which is unsigned again. So the source computed the quotient into a
//  signed local and clamped it into an unsigned one, which is how it is written below; the clamps read `<=` and `>=` because the bytes say `jle`
//  and `jl`.
//  BUG: `FOG_Trace("Tile Cache Size: %dMB", 6)` pushes a literal 6 outside the `if (bInitTileCache)`, so a call that passed 0 would print six
//  megabytes of tile cache it had not reserved. The only caller passes 1, so it never shows; reproduced.

//  The minimum-memory threshold, 22 MB plus 512000; 512000 is also the third argument this function hands to D2CMP_InitSpriteCache (`push 7D000h`).
static_assert(0x167D000 == 22 * 1024 * 1024 + 512000,
              "the RAM floor is 22 MB plus the sprite cache's own 512000");
static_assert(0x7D000 == 512000,
              "D2CMP_InitSpriteCache's third argument is that same 512000");
//  The five budgets, each an exact number of megabytes.
static_assert(0xB00000 == 11 * 1024 * 1024 && 0x300000 == 3 * 1024 * 1024
              && 0x500000 == 5 * 1024 * 1024 && 0x600000 == 6 * 1024 * 1024
              && 0x200000 == 2 * 1024 * 1024,
              "the reserve and the sound and tile budgets, in megabytes");
static_assert(0x1800000 == 24 * 1024 * 1024 && 0x4000000 == 64 * 1024 * 1024,
              "the sprite cache clamps, in megabytes");

void __fastcall MEMPART_InitMemoryPartitions(int32_t hPool, int32_t bInitTileCache)
{
    D2FogSystemInfoStrc *pSysInfo;
    uint32_t nPhysicalBytes;
    uint32_t nUsableBytes;
    uint32_t nRemainingBytes;
    int32_t nSoundBudget;
    uint32_t nSoundCacheSize;
    int32_t nSpriteThird;
    uint32_t nSpriteCacheSize;

    pSysInfo = FOG_GetSystemInfo();
    if (!pSysInfo)
    {
        FOG_DisplayAssert("pSysInfo != NULL", szSourceFile, 48);
        exit(-1);
    }

    nPhysicalBytes = pSysInfo->dwTotalPhysicalBytes;
    if (nPhysicalBytes < 0x167D000)
    {
        //  The product is taken in double: the original stores the count beside a zero dword and loads it with `fild qword ptr [esp+4]`
        //  (0x6FAA13D8..0x6FAA13E4), an exact unsigned load, then `fmul dword ptr [gfBytesToMegabytes]` (0x6FAA13EB) and
        //  `fstp qword ptr [esp]` (0x6FAA13F1). Converting the DWORD to float first would round it to 24 bits from 16 MB up, inside this very branch,
        //  which runs below 0x167D000. Any DWORD is exact in a double and the factor is 2^-20, so the double product is the x87 one, exactly.
        FOG_Trace("This machine doesn't have enough physical memory (> %.2f MB) for minimal performance!", (double)nPhysicalBytes * gfBytesToMegabytes);
        exit(-1);
    }

    nUsableBytes = nPhysicalBytes - 11 * 1024 * 1024;

    nSoundBudget = nUsableBytes * 10 / 100;
    if (nSoundBudget <= 3 * 1024 * 1024)
        nSoundCacheSize = 3 * 1024 * 1024;
    else if (nSoundBudget >= 5 * 1024 * 1024)
        nSoundCacheSize = 5 * 1024 * 1024;
    else
        nSoundCacheSize = nSoundBudget;

    SOUNDHDR_SetCacheBudget(nSoundCacheSize);

    nRemainingBytes = nUsableBytes - nSoundCacheSize;
    if (bInitTileCache)
    {
        nRemainingBytes -= 6 * 1024 * 1024;
        D2CMP_10089(hPool, 6 * 1024 * 1024);
    }

    nSpriteThird = (nRemainingBytes - 2 * 1024 * 1024) / 3;
    if (nSpriteThird <= 24 * 1024 * 1024)
        nSpriteCacheSize = 24 * 1024 * 1024;
    else if (nSpriteThird >= 64 * 1024 * 1024)
        nSpriteCacheSize = 64 * 1024 * 1024;
    else
        nSpriteCacheSize = nSpriteThird;

    FOG_Trace("Sound Cache Size: %dMB", nSoundCacheSize >> 20);
    FOG_Trace("Tile Cache Size: %dMB", 6);
    FOG_Trace("Sprite Cache Size: %dMB", nSpriteCacheSize >> 20);

    D2CMP_InitSpriteCache(hPool, nSpriteCacheSize, 512000, nUsableBytes);

    //  A tail call (`jmp GFX_InitCelCache` after `mov ecx, 200000h`) in the original, not the start of a second half of this function.
    GFX_InitCelCache(2 * 1024 * 1024);
}


//1.10f: D2Client.0x6FAA14E0
//  Drops the three graphics caches, in order: D2CMP #10090 (paired with the tile cache's #10089 by ordinal and order), the sprite cache and the cel
//  cache. It mirrors, call for call, the tile, sprite and cel caches MEMPART_InitMemoryPartitions builds. The flag goes to both D2CMP calls
//  (`push esi` twice, 0x6FAA14E3 and 0x6FAA14E9); the cel cache's free is a tail jump (0x6FAA14F0). Its caller, GAME_AppModeGame, discards eax.
void __fastcall MEMPART_DestroyCaches(BOOL bFreeLRU)
{
    D2CMP_10090(bFreeLRU);
    D2CMP_FlushSpriteCache(bFreeLRU);
    GFX_FreeCelCache();
}

//1.10f: D2Client.0x6FAA1500
//  A single `retn`. Its callers set up different numbers of arguments for it (two at 0x6FAA97EA and 0x6FABBC58, one at 0x6FAA2448, none at the rest),
//  so the source declared it inconsistently, not once per prototype; `void __cdecl()` is true of every call.
void __cdecl MEMPART_Nullsub1()
{
}

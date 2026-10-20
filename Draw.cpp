#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Draw.h"

//  Draw.cpp -- the display type and the frame-rate overlay of D2Client.dll 1.10: eleven functions, 0x6FAA2860..0x6FAA2E21. No assert in this unit
//  names the original's source file, so the file name is ours.
//  DRAW_SetGamma forwards the gamma to D2Gfx; DRAW_SelectGdiDisplay and the four DRAW_SetDisplayType{Glide,OpenGL,Direct3D,Rave} store the renderer
//  choice (called from the command-line handlers in Game.cpp) and DRAW_GetDisplayType and DRAW_IsDisplayType3D read it;
//  DRAW_GameDrawDebugOverlay draws the /framerate overlay every frame; DRAW_GameToggleDebugOverlay (/framerate) and DRAW_ToggleDebugOverlay
//  (/fps, the overlay's second page) switch it.

//  The renderer ids the setters store and the overlay names.
static_assert(DISPLAYTYPE_GDI == 1 && DISPLAYTYPE_SOFTWARE == 2
              && DISPLAYTYPE_DDRAW == 3 && DISPLAYTYPE_GLIDE == 4
              && DISPLAYTYPE_OPENGL == 5 && DISPLAYTYPE_DIRECT3D == 6
              && DISPLAYTYPE_RAVE == 7,
              "the setters' immediates are D2MOO's DisplayType");

//1.10f: D2Client.0x6FAA2860
//  Hands the gamma to D2GFX_SetGamma. Its answer is left in eax and read by no caller.
void __fastcall DRAW_SetGamma(uint32_t nGamma)
{
    D2GFX_SetGamma(nGamma);
}

//1.10f: D2Client.0x6FAA2870
//  Selects the GDI display and raises gbAskedForGDI: one 1 is stored into both. Its one caller reaches it by a tail jmp from a void function,
//  so the 1 left in eax is nobody's.
//  BUG: gbAskedForGDI is stored here and read by nothing. INERT: this store is its only reference.
void __cdecl DRAW_SelectGdiDisplay()
{
    gDisplayType = DISPLAYTYPE_GDI;
    gbAskedForGDI = 1;
}

//1.10f: D2Client.0x6FAA2880
void __cdecl DRAW_SetDisplayTypeGlide()
{
    gDisplayType = DISPLAYTYPE_GLIDE;
}

//1.10f: D2Client.0x6FAA2890
void __cdecl DRAW_SetDisplayTypeOpenGL()
{
    gDisplayType = DISPLAYTYPE_OPENGL;
}

//1.10f: D2Client.0x6FAA28A0
void __cdecl DRAW_SetDisplayTypeDirect3D()
{
    gDisplayType = DISPLAYTYPE_DIRECT3D;
}

//1.10f: D2Client.0x6FAA28B0
void __cdecl DRAW_SetDisplayTypeRave()
{
    gDisplayType = DISPLAYTYPE_RAVE;
}

//1.10f: D2Client.0x6FAA28C0
//  Returns the renderer in use (gDisplayType).
int32_t __cdecl DRAW_GetDisplayType()
{
    return (int32_t)gDisplayType;
}

//1.10f: D2Client.0x6FAA28D0
//  True when the display type is DISPLAYTYPE_GLIDE (4) or above. The compare is signed (setnl); the cast keeps it so.
BOOL __cdecl DRAW_IsDisplayType3D()
{
    return (int32_t)gDisplayType >= DISPLAYTYPE_GLIDE;
}

//1.10f: D2Client.0x6FAA28E0
//  Draws the /framerate overlay. Called every frame; it counts frames whether or not the overlay is on and draws only when gbDebugOverlay is set.
//  The frame-rate window test is an unsigned compare against 0x3E8 (1000 ms), so a tick wrap behaves as the subtraction does. gbDebugOverlay is
//  read once and tested twice. gnFpsSkipStartTick and gnFpsSkipCounter are armed when the overlay is on and nothing else reads either.
//  The lines: page 0 only, below the header lines, GFX, COF, sprite cache, async files and cel cache, then the tile and D2Gfx cache statistics.
//  Each "used of budget" line takes both values as calls in its argument list, pushed right to left, so the budget is called before the used
//  figure (GFX_GetCofMemoryBudget before GFX_GetCofMemoryUsed, D2CMP #10061 before #10060, #10064 before #10063). #10060 and #10063 take no argument.
//  BUG: the six statistics lines are formatted twice. sprintf fills szLine and szLine is then handed to D2GFX_OutputString as its format, so a %
//  in the text is formatted again: the tile line's "%3.2f%%" leaves "12.34%, Misses" in szLine, and "%," is then formatted with no argument behind
//  it. REACHABLE: by typing /framerate. Reproduced.
//  The arithmetic is the x87's: fild of a qword whose high dword is 0 (the byte counts, unsigned), fmul by gfOneOver1024 and gfPercentDivisor,
//  fild and fidiv of the tile misses and accesses as signed ints (misses times 100, then the division); two decimals
//  are printed.
void __cdecl DRAW_GameDrawDebugOverlay()
{
    uint32_t dwNow;
    uint32_t dwElapsed;
    int32_t nSkipRate;
    int32_t nCofBudget;
    int32_t nSpriteBudget;
    int32_t nCelBudget;
    uint32_t nTileBytes;
    int32_t nTileMisses;
    int32_t nTileAccesses;
    int32_t nTilePercent;
    uint32_t *pCache;
    char szRenderer[32];
    MEMORYSTATUS tMemStatus;
    char szLine[1024];

    dwNow = GetTickCount();
    dwElapsed = dwNow - (uint32_t)gnFpsWindowTick;
    if (gbDebugOverlay && !gnFpsSkipStartTick)
    {
        gnFpsSkipStartTick = (int32_t)dwNow;
        gnFpsSkipCounter = 0;
    }

    if (dwElapsed > 1000)
    {
        gnFpsWindowTick = (int32_t)dwNow;
        gnFramesPerSecond = gnFpsFrameCounter;
        gnFpsFrameCounter = 0;
    }
    else
    {
        ++gnFpsFrameCounter;
        ++gnFpsSkipCounter;
    }

    if (!gbDebugOverlay)
    {
        gnFpsSkipStartTick = 0;
        return;
    }

    //  The skip rate is called first and the frame count read after it.
    nSkipRate = GAME_GetRecentSkipRate();
    D2GFX_OutputString(10, 2, szFormat, gnFramesPerSecond, nSkipRate);
    D2GFX_OutputString(160, 2, "Ping: %d", GAME_GetNetworkLatency());

    //  Megabytes: an unsigned shift right by 20, plus one.
    tMemStatus.dwLength = sizeof(tMemStatus);
    GlobalMemoryStatus(&tMemStatus);
    D2GFX_OutputString(10, 16, "Phys:%d/%dm", ((tMemStatus.dwTotalPhys - tMemStatus.dwAvailPhys) >> 20) + 1, (tMemStatus.dwTotalPhys >> 20) + 1);
    D2GFX_OutputString(160, 16, "Page:%d/%dm", ((tMemStatus.dwTotalPageFile - tMemStatus.dwAvailPageFile) >> 20) + 1, (tMemStatus.dwTotalPageFile >> 20) + 1);

    //  Each arm hands the renderer's name to sprintf as the format, with no argument.
    switch (D2GFX_GetDisplayType())
    {
    case DISPLAYTYPE_GDI:
    case DISPLAYTYPE_SOFTWARE:
        sprintf(szRenderer, "Software");
        break;
    case DISPLAYTYPE_DDRAW:
        sprintf(szRenderer, "DirectDraw");
        break;
    case DISPLAYTYPE_GLIDE:
        sprintf(szRenderer, "Glide");
        break;
    case DISPLAYTYPE_OPENGL:
        sprintf(szRenderer, "OpenGL");
        break;
    case DISPLAYTYPE_DIRECT3D:
        sprintf(szRenderer, "Direct3D");
        break;
    case DISPLAYTYPE_RAVE:
        sprintf(szRenderer, "RAVE");
        break;
    default:
        sprintf(szRenderer, "Unknown");
        break;
    }
    D2GFX_OutputString(500, 2, "Video: %s", szRenderer);

    if (gnDebugOverlayPage)
        return;

    D2GFX_OutputString(10, 120, "GFX Memory->%i", GFX_GetGfxMemoryUsed());

    nCofBudget = GFX_GetCofMemoryBudget();
    D2GFX_OutputString(10, 132, "COF Memory->%i of %i", GFX_GetCofMemoryUsed(), nCofBudget);

    nSpriteBudget = D2CMP_10061();
    D2GFX_OutputString(10, 144, "Sprite Cache Memory->%i of %i", D2CMP_10060(), nSpriteBudget);

    D2GFX_OutputString(10, 156, "Async Files->%i", FOG_10098());

    nCelBudget = D2CMP_10064();
    D2GFX_OutputString(10, 168, "CelData Cache Memory->%i of %i", D2CMP_10063(), nCelBudget);

    MEMPART_Nullsub1();

    D2CMP_10092((uint32_t)(UINT_PTR)&nTileBytes, (uint32_t)(UINT_PTR)&nTileMisses, (uint32_t)(UINT_PTR)&nTileAccesses, (uint32_t)(UINT_PTR)&nTilePercent);
    if (nTileAccesses > 0)
    {
        sprintf(szLine, gszTileCacheDebugFmt, (double)nTileBytes * gfOneOver1024, (double)nTileMisses * gfPercentDivisor / nTileAccesses, nTileMisses, nTilePercent);
        //  szLine is handed on as the format.
        D2GFX_OutputString(10, 48, szLine);
    }

    pCache = D2GFX_GetCacheResults();
    if (!pCache)
        return;

    if (pCache[0])
    {
        sprintf(szLine, "Floor Cache: (%.2fK) Misses: %d\n", (double)pCache[0] * gfOneOver1024, pCache[2]);
        D2GFX_OutputString(10, 60, szLine);
    }
    if (pCache[3])
    {
        sprintf(szLine, "Subtile Cache: (%.2fK) Misses: %d\n", (double)pCache[3] * gfOneOver1024, pCache[5]);
        D2GFX_OutputString(10, 72, szLine);
    }
    if (pCache[6])
    {
        sprintf(szLine, "Sprite Cache 1: (%.2fK) Misses: %d, Accesses Per Frame: %d\n", (double)pCache[6] * gfOneOver1024, pCache[12], pCache[18]);
        D2GFX_OutputString(10, 84, szLine);
    }
    if (pCache[7])
    {
        sprintf(szLine, "Sprite Cache 2: (%.2fK) Misses: %d, Accesses Per Frame: %d\n", (double)pCache[7] * gfOneOver1024, pCache[13], pCache[19]);
        D2GFX_OutputString(10, 96, szLine);
    }
    if (pCache[8])
    {
        sprintf(szLine, "Sprite Cache 3: (%.2fK) Misses: %d, Accesses Per Frame: %d\n", (double)pCache[8] * gfOneOver1024, pCache[14], pCache[20]);
        D2GFX_OutputString(10, 108, szLine);
    }
}


//1.10f: D2Client.0x6FAA2D30
//  /framerate: flips the overlay and goes back to page 0. When the overlay turns on it clears sixteen dwords of D2Gfx's cache-results block
//  (indices 1, 2, 4, 5, 9..14, 18..20 and 24..26: the misses and per-frame counts beside the sizes the overlay prints), in that order, and then
//  ends in a tail jmp to D2CMP #10093 whether or not the block was there. void: its callers do not read eax.
void __cdecl DRAW_GameToggleDebugOverlay()
{
    uint32_t *pCache;

    gnDebugOverlayPage = 0;
    gbDebugOverlay = gbDebugOverlay == 0;
    if (!gbDebugOverlay)
        return;

    pCache = D2GFX_GetCacheResults();
    if (pCache)
    {
        pCache[1] = 0;
        pCache[2] = 0;
        pCache[4] = 0;
        pCache[5] = 0;
        pCache[9] = 0;
        pCache[10] = 0;
        pCache[11] = 0;
        pCache[12] = 0;
        pCache[13] = 0;
        pCache[14] = 0;
        pCache[18] = 0;
        pCache[19] = 0;
        pCache[20] = 0;
        pCache[24] = 0;
        pCache[25] = 0;
        pCache[26] = 0;
    }
    D2CMP_10093();
}

//1.10f: D2Client.0x6FAA2DA0
//  /fps. At page 0 with the overlay on it only moves to page 1; otherwise it toggles as DRAW_GameToggleDebugOverlay does (page 0 first, then the
//  flag, including the cache-results clear and D2CMP #10093 when turning on) and still ends at page 1 on every path. So from off it turns the
//  overlay on at page 1; at page 0, where /framerate leaves it, it moves to page 1; at page 1 it turns the overlay off. void: its callers do not
//  read eax.
void __cdecl DRAW_ToggleDebugOverlay()
{
    uint32_t *pCache;

    if (gnDebugOverlayPage || !gbDebugOverlay)
    {
        gnDebugOverlayPage = 0;
        gbDebugOverlay = gbDebugOverlay == 0;
        if (gbDebugOverlay)
        {
            pCache = D2GFX_GetCacheResults();
            if (pCache)
            {
                pCache[1] = 0;
                pCache[2] = 0;
                pCache[4] = 0;
                pCache[5] = 0;
                pCache[9] = 0;
                pCache[10] = 0;
                pCache[11] = 0;
                pCache[12] = 0;
                pCache[13] = 0;
                pCache[14] = 0;
                pCache[18] = 0;
                pCache[19] = 0;
                pCache[20] = 0;
                pCache[24] = 0;
                pCache[25] = 0;
                pCache[26] = 0;
            }
            D2CMP_10093();
        }
    }
    gnDebugOverlayPage = 1;
}

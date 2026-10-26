#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "WINMAIN.h"
#include "CoreArchive.h"
#include "automap.h"

//  automap.cpp -- the automap of D2Client.dll 1.10: the mini-map and full-screen map, its settings (kept in the registry under "Diablo II"), the
//  per-frame reveal around the player, the layer list and the AVL tree of map cells, the explored-map files on disk, the level generator's two
//  callbacks that put preset levels on the map, and the draw pass with its corner labels and unit markers.
//  The original source file is UI\automap.cpp (the asserts in this unit name it). Address range 0x6FACB6A0..0x6FACF8C5.

//1.10f: D2Client.0x6FACB6A0
//  VK_DOWN, record 1 of gAutomapMessageHandlers (registered under UI_AUTOMAP; the fifth record, VK_END, is HELP_Nullsub5): gnAutomapPanY += 16.
//  The four arrow handlers claim nothing (they neither set bReturn nor call STORM_SMsgBreakHandlerChain), so an arrow key pans the map and then
//  reaches every other handler of that key. The pan is never clamped; AUTOMAP_RecomputeCenterOffsets puts it back. pMsg is unread.
void __stdcall AUTOMAP_OnArrowDown(SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    gnAutomapPanY += 16;
}


//1.10f: D2Client.0x6FACB6B0
//  VK_UP, record 0: gnAutomapPanY -= 16, so the map is drawn higher up the screen (AUTOMAP_Draw adds the pan into the draw origin).
void __stdcall AUTOMAP_OnArrowUp(SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    gnAutomapPanY -= 16;
}


//1.10f: D2Client.0x6FACB6C0
//  VK_LEFT, record 3: gnAutomapPanX -= 16.
void __stdcall AUTOMAP_OnArrowLeft(SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    gnAutomapPanX -= 16;
}


//1.10f: D2Client.0x6FACB6D0
//  VK_RIGHT, record 2: gnAutomapPanX += 16.
void __stdcall AUTOMAP_OnArrowRight(SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    gnAutomapPanX += 16;
}


//  Registry names of the automap settings: the key "Diablo II" and the values "AutoMapFade", "AutoMap Centers", "AutoMap Left", "AutoMap Party"
//  and "AutoMap Party Names". Centers, Party and PartyNames each have a getter and a setter that saves; Fade and Mode have pairs of their own.
static const char szDiabloIIKey[] = "Diablo II";
static const char szAutoMapFade[] = "AutoMapFade";
static const char szAutoMapParty[] = "AutoMap Party";
static const char szAutoMapPartyNames[] = "AutoMap Party Names";
//  The data root every automap cel path is built on.
static const char aDataGlobal[] = "DATA\\GLOBAL";
static const char szAutoMapLeft[] = "AutoMap Left";
//  __FILE__ of the original unit, as every FOG_Alloc, FOG_Free and FOG_DisplayAssert call here passes it.
static const char szAutomapCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UI\\automap.cpp";

//  The assert texts this unit pushes: the frame tick's, then the cell tree's, which name its fields ptLess, ptMore and sBalence (the original's
//  names for the cell's pPrev, pNext and wWeight).
static const char szAssertHUnit[] = "hUnit";
static const char szAssertPtMoreBalence[] = "ptMore->sBalence == BALANCE_LHIGH || ptMore->sBalence == BALANCE_MHIGH";
static const char szAssertPtMoreNull[] = "ptRoot->ptMore != NULL";
static const char szAssertPtLessNull[] = "ptRoot->ptLess != NULL";
static const char szAssertPtRootNull[] = "ptRoot != NULL";
static const char szAssertPtLessBalence[] = "ptLess->sBalence == BALANCE_LHIGH || ptLess->sBalence == BALANCE_MHIGH";
//  The three range checks every cell store passes, and the AUTOMAP_AddTown assert.
static const char szAssertPixelX[] = "nPixelX >= SHRT_MIN && nPixelX <= SHRT_MAX";
static const char szAssertPixelY[] = "nPixelY >= SHRT_MIN && nPixelY <= SHRT_MAX";
static const char szAssertCelNum[] = "nCelNum >= SHRT_MIN && nCelNum <= SHRT_MAX";
static const char szAssertAddTown[] = "AutoMapAddTown() - Why are you calling this function here???";
//  The four length checks of AUTOMAP_LoadCelData, one per block; tAutoData is the original's name for the descriptor.
static const char szAssertExtraSize[] = "dwBytes == tAutoData.dwExtraSize";
static const char szAssertUnitsSize[] = "dwBytes == tAutoData.dwUnitsSize";
static const char szAssertWallsSize[] = "dwBytes == tAutoData.dwWallsSize";
static const char szAssertFloorSize[] = "dwBytes == tAutoData.dwFloorSize";


//1.10f: D2Client.0x6FACB6E0
//  Puts the pan back to the centre: a no-op unless the centre-on-player setting is on or bForce is set. void: no caller reads an answer (two of
//  them are tail jumps from key commands). The four lines under the mode test are the block AUTOMAP_SetMode and AUTOMAP_UpdateClipRect also end
//  with; it is written out in each.
void __fastcall AUTOMAP_RecomputeCenterOffsets(int32_t bForce)
{
    if (gbAutomapCenters || bForce)
    {
        AUTOMAP_UpdateClipRect();

        if (gnAutomapMode == 1)
        {
            gnAutomapPanX = gnScreenWidth / 3 - gnAutomapMiniLeft - 16;
            gnAutomapPanY = gnScreenHeight / 3 - gnAutomapMiniTop - 16;
        }
        else
        {
            gnAutomapPanX = 0;
            gnAutomapPanY = 0;
        }
    }
}


//1.10f: D2Client.0x6FACB8F0
//  The fade setting. Not a plain accessor: in mini-map mode (mode 1) a stored fade of 1 is not legal, so the getter coerces it to whichever of
//  0 and 2 the saved value is not (gnAutomapFadeSaved != 0 ? 0 : 2) and stores that back into gnAutomapFade on the way out.
int32_t __cdecl AUTOMAP_GetFade()
{
    int32_t nFade;

    nFade = gnAutomapFade;

    if (gnAutomapMode == 1 && gnAutomapFade == 1)
    {
        nFade = gnAutomapFadeSaved != 0 ? 0 : 2;
        gnAutomapFade = nFade;
    }

    return nFade;
}


//1.10f: D2Client.0x6FACB920
//  Sets the fade: the registry, then the saved copy, then the live one. In mini-map mode a requested 1 is coerced as in AUTOMAP_GetFade, so the
//  setting alternates between 0 and 2 each time 1 is requested. The store of 1 into gnAutomapFade inside the coercion branch is dead (the
//  resolved value overwrites it four lines later); it is kept.
int32_t __fastcall AUTOMAP_SetFade(int32_t nFade)
{
    int32_t nResolvedFade;

    nResolvedFade = nFade;

    if (gnAutomapMode == 1 && nFade == 1)
    {
        gnAutomapFade = 1;
        nResolvedFade = gnAutomapFadeSaved != 0 ? 0 : 2;
    }

    STORM_SRegSaveValue(szDiabloIIKey, szAutoMapFade, 0, nResolvedFade);
    gnAutomapFadeSaved = nResolvedFade;
    gnAutomapFade = nResolvedFade;

    return nResolvedFade;
}


//1.10f: D2Client.0x6FACB970
//  Centre the map on the player?
int32_t __cdecl AUTOMAP_GetCenters()
{
    return gbAutomapCenters;
}


//1.10f: D2Client.0x6FACB980
//  Saves the setting to the registry and sets it; the first of three identical setters that differ only in the value name. Returns its argument
//  (mov eax, esi), which no caller reads.
int32_t __fastcall AUTOMAP_SetCentersAndSave(int32_t bCenters)
{
    STORM_SRegSaveValue(szDiabloIIKey, szValueName, 0, bCenters);
    gbAutomapCenters = bCenters;

    return bCenters;
}


//1.10f: D2Client.0x6FACB9A0
//  Is the mini-map pinned to the left edge? Returns the live gbAutomapLeft; AUTOMAP_UpdateClipRect reads the flag directly rather than through
//  this. The ESC menu does not offer the choice; the next function is its setter, driven by a bindable key.
int32_t __cdecl AUTOMAP_GetLeft()
{
    return gbAutomapLeft;
}


//1.10f: D2Client.0x6FACB9B0
//  Saves the left-edge setting as the registry value "AutoMap Left", then sets the setting's three globals in this order: gnAutomapLeftSaved,
//  gnAutomapLeftSetting and gbAutomapLeft. Returns its argument (mov eax, esi at 0x6FACB9D7), which its one caller, a tail jump from
//  CMDTBL_ActionToggleAutomapLeft, does not read.
int32_t __fastcall AUTOMAP_ApplyAutomapLeftSetting(int32_t bAutomapLeft)
{
    STORM_SRegSaveValue(szDiabloIIKey, szAutoMapLeft, 0, bAutomapLeft);
    gnAutomapLeftSaved = bAutomapLeft;
    gnAutomapLeftSetting = bAutomapLeft;
    gbAutomapLeft = bAutomapLeft;

    return bAutomapLeft;
}


//1.10f: D2Client.0x6FACB9E0
//  Draw the other players' markers?
int32_t __cdecl AUTOMAP_GetShowParty()
{
    return gbAutomapShowParty;
}


//1.10f: D2Client.0x6FACB9F0
//  The second of the three identical setters.
int32_t __fastcall AUTOMAP_SetShowPartyAndSave(int32_t bShow)
{
    STORM_SRegSaveValue(szDiabloIIKey, szAutoMapParty, 0, bShow);
    gbAutomapShowParty = bShow;

    return bShow;
}


//1.10f: D2Client.0x6FACBA10
//  Draw their names beside the markers? The ESC menu's row reads it as GetShowParty() && GetPartyNames(), so this flag is only half the answer.
int32_t __cdecl AUTOMAP_GetPartyNames()
{
    return gbAutomapPartyNames;
}


//1.10f: D2Client.0x6FACBA20
//  The third of the three identical setters.
int32_t __fastcall AUTOMAP_SetPartyNamesAndSave(int32_t bShow)
{
    STORM_SRegSaveValue(szDiabloIIKey, szAutoMapPartyNames, 0, bShow);
    gbAutomapPartyNames = bShow;

    return bShow;
}


//1.10f: D2Client.0x6FACF820
//  Mini-map (1) or full screen.
int32_t __cdecl AUTOMAP_GetMode()
{
    return gnAutomapMode;
}


//1.10f: D2Client.0x6FACF830
//  Switches between the mini-map (mode 1) and the full screen. The whole body runs only if nMode differs from gnAutomapMode. On a change: the zoom
//  becomes 20 for mode 1 and 10 otherwise, the icon cels are reloaded, the clip rectangle is recomputed and the pan is re-centred (the same
//  four-line block as in AUTOMAP_RecomputeCenterOffsets). void: both callers discard the answer.
void __fastcall AUTOMAP_SetMode(int32_t nMode)
{
    if (nMode != gnAutomapMode)
    {
        gnAutomapMode = nMode;

        gnAutomapZoom = 10;
        if (nMode == 1)
            gnAutomapZoom = 20;

        AUTOMAP_LoadIconCels();
        AUTOMAP_UpdateClipRect();

        if (gnAutomapMode == 1)
        {
            gnAutomapPanX = gnScreenWidth / 3 - gnAutomapMiniLeft - 16;
            gnAutomapPanY = gnScreenHeight / 3 - gnAutomapMiniTop - 16;
        }
        else
        {
            gnAutomapPanX = 0;
            gnAutomapPanY = 0;
        }
    }
}


//1.10f: D2Client.0x6FACB760
//  Recomputes the rectangle the automap may draw inside. Full screen: min x and y are -16 and the maxima are the screen size; the -16 margins let a
//  marker whose centre is just off-screen draw its overhanging half. Mini-map: a third of the screen each way, min x = nLeft - 8, max x = nLeft +
//  gnScreenWidth / 3, min y = nTop, max y = nTop + gnScreenHeight / 3; pinned left when gbAutomapLeft is set (nLeft = 0, nTop = 0x60 with the
//  party-portrait strip and 0 without) and right otherwise (nLeft = 2 * gnScreenWidth / 3, nTop = 78).
//  gnAutomapMiniLeftUnused and gnAutomapMiniTopUnused are written here and read nowhere.
//  The function calls itself (a real call at 0x6FACB7A2; control comes back). Two inputs are latched, the party-portrait strip and gbAutomapLeft;
//  in mini-map mode, when either has changed, it writes the new latch and re-enters, and on the way in the latch matches, so the second pass falls
//  through to the geometry. The depth is bounded at two by the two latches: with both stale the outer frame latches the portraits and calls,
//  the middle frame latches Left and calls, and the innermost computes the geometry. Each returning frame then runs the pan re-centre, so it
//  runs twice; it is idempotent (it reads gnAutomapMiniLeft and gnAutomapMiniTop, which the innermost pass has already set).
//  The re-latch calls HIREABLES_PanelPortraitsReserveTopStrip a second time rather than storing the value it already has in a register.
//  The gotos are the original's graph: two separate tests reach one re-latch block.
void __cdecl AUTOMAP_UpdateClipRect()
{
    int32_t nPortraitStrip;
    int32_t nAutomapMode;
    BOOL bPortraitStrip;
    int32_t nLeft;
    int32_t nScreenWidth;
    int32_t nTop;

    nPortraitStrip = HIREABLES_PanelPortraitsReserveTopStrip();
    nAutomapMode = gnAutomapMode;

    if (gnAutomapClipLatchPortraits != nPortraitStrip && gnAutomapMode == 1)
    {
        //  asked a second time rather than reusing nPortraitStrip
        gnAutomapClipLatchPortraits = HIREABLES_PanelPortraitsReserveTopStrip();
        goto relatched;
    }

    if (gnAutomapClipLatchLeft != gbAutomapLeft && gnAutomapMode == 1)
    {
        gnAutomapClipLatchLeft = gbAutomapLeft;

    relatched:
        AUTOMAP_UpdateClipRect();

        if (gnAutomapMode == 1)
        {
            gnAutomapPanX = gnScreenWidth / 3 - gnAutomapMiniLeft - 16;
            gnAutomapPanY = gnScreenHeight / 3 - gnAutomapMiniTop - 16;
        }
        else
        {
            gnAutomapPanX = 0;
            gnAutomapPanY = 0;
        }
        return;
    }

    if (gbAutomapLeft)
    {
        gnAutomapMiniLeft = 0;
        bPortraitStrip = HIREABLES_PanelPortraitsReserveTopStrip();
        nAutomapMode = gnAutomapMode;
        nLeft = gnAutomapMiniLeft;
        nScreenWidth = gnScreenWidth;
        nTop = bPortraitStrip ? 0x60 : 0;
    }
    else
    {
        nScreenWidth = gnScreenWidth;
        nTop = 78;
        nLeft = 2 * gnScreenWidth / 3;
        gnAutomapMiniLeft = nLeft;
    }

    gnAutomapMiniTop = nTop;
    gnAutomapMiniLeftUnused = nLeft;
    gnAutomapMiniTopUnused = nTop;

    if (nAutomapMode == 1)
    {
        gnAutomapClipMinX = nLeft - 8;
        gnAutomapClipMaxX = nLeft + nScreenWidth / 3;
        gnAutomapClipMinY = nTop;
        gnAutomapClipMaxY = nTop + gnScreenHeight / 3;
    }
    else
    {
        gnAutomapClipMaxX = nScreenWidth;
        gnAutomapClipMinX = -16;
        gnAutomapClipMinY = -16;
        gnAutomapClipMaxY = gnScreenHeight;
    }
}


//1.10f: D2Client.0x6FACF490
//  Loads the four icon cel files: MaxiMapS, Act2MapS, Act4MapS and ExTnMapS in mini-map mode, MaxiMap, Act2Map, Act4Map and ExTnMap otherwise.
//  Called from AUTOMAP_LoadSettings at startup and from AUTOMAP_SetMode on every mode change, so it unloads first.
//  BUG: in mini-map mode ExTnMapS is loaded with no FOG_IsExpansion test, while the other arm loads ExTnMap only on an expansion install and
//  otherwise sets slot 3 to 0. ARCHIVE_CellFileLoad returns 0 for a file it cannot find, and that null reaches D2CMP_CelFileGetCelsPerDirection.
//  NOT ESTABLISHED: it turns on whether ExTnMapS.dc6 ships in the classic archive.
//  The path buffer is one MAX_PATH array, cleared in the three steps the bytes have.
void __cdecl AUTOMAP_LoadIconCels()
{
    D2CellFileStrc **pAutomapIconCels;
    char szPath[260];

    szPath[0] = gszEmptyString;
    pAutomapIconCels = gpAutomapIconCels;
    memset(&szPath[1], 0, 0x100);
    *(int16_t *)&szPath[257] = 0;
    szPath[259] = 0;

    do
    {
        if (*pAutomapIconCels)
            ARCHIVE_CellFileFree(*pAutomapIconCels);
        *pAutomapIconCels++ = 0;
    }
    //  end of gpAutomapIconCels: the original compares with the next global's address (cmp esi, 6FBB19B8h); here it is the array plus its 4 cels
    while ((int32_t)pAutomapIconCels < (int32_t)(gpAutomapIconCels + 4));

    if (gnAutomapMode == 1)
    {
        sprintf(szPath, "%s\\UI\\AutoMap\\MaxiMapS", aDataGlobal);
        gpAutomapIconCels[0] = ARCHIVE_CellFileLoad(szPath, 0);
        gnAutomapIconCelCounts[0] = D2CMP_CelFileGetCelsPerDirection(gpAutomapIconCels[0]);

        sprintf(szPath, "%s\\UI\\AutoMap\\Act2MapS", aDataGlobal);
        gpAutomapIconCels[1] = ARCHIVE_CellFileLoad(szPath, 0);
        gnAutomapIconCelCounts[1] = D2CMP_CelFileGetCelsPerDirection(gpAutomapIconCels[1]);

        sprintf(szPath, "%s\\UI\\AutoMap\\Act4MapS", aDataGlobal);
        gpAutomapIconCels[2] = ARCHIVE_CellFileLoad(szPath, 0);
        gnAutomapIconCelCounts[2] = D2CMP_CelFileGetCelsPerDirection(gpAutomapIconCels[2]);

        //  no FOG_IsExpansion() test here, unlike the arm below
        sprintf(szPath, "%s\\UI\\AutoMap\\ExTnMapS", aDataGlobal);
    }
    else
    {
        sprintf(szPath, "%s\\UI\\AutoMap\\MaxiMap", aDataGlobal);
        gpAutomapIconCels[0] = ARCHIVE_CellFileLoad(szPath, 0);
        gnAutomapIconCelCounts[0] = D2CMP_CelFileGetCelsPerDirection(gpAutomapIconCels[0]);

        sprintf(szPath, "%s\\UI\\AutoMap\\Act2Map", aDataGlobal);
        gpAutomapIconCels[1] = ARCHIVE_CellFileLoad(szPath, 0);
        gnAutomapIconCelCounts[1] = D2CMP_CelFileGetCelsPerDirection(gpAutomapIconCels[1]);

        sprintf(szPath, "%s\\UI\\AutoMap\\Act4Map", aDataGlobal);
        gpAutomapIconCels[2] = ARCHIVE_CellFileLoad(szPath, 0);
        gnAutomapIconCelCounts[2] = D2CMP_CelFileGetCelsPerDirection(gpAutomapIconCels[2]);

        if (!FOG_IsExpansion())
        {
            gpAutomapIconCels[3] = 0;
            gnAutomapIconCelCounts[3] = 0;
            return;
        }

        sprintf(szPath, "%s\\UI\\AutoMap\\ExTnMap", aDataGlobal);
    }

    gpAutomapIconCels[3] = ARCHIVE_CellFileLoad(szPath, 0);
    gnAutomapIconCelCounts[3] = D2CMP_CelFileGetCelsPerDirection(gpAutomapIconCels[3]);
}


//1.10f: D2Client.0x6FACF380
//  Loads the five settings from the registry and builds the cell-group table. The local is preset to the setting's default before its address is
//  passed to STORM_SRegLoadValue, so a value the registry lacks keeps the default: AutoMapFade 0, AutoMap Centers 1, AutoMap Party 1,
//  AutoMap Party Names 1, AutoMap Left 1. AutoMap Left is stored three times (gbAutomapLeft, gnAutomapLeftSaved and gnAutomapLeftSetting)
//  because AUTOMAP_Draw shuffles between them when the UI shifts, so the player's preference survives a temporary override.
//  The cell-group table: gaAutomapCellGroup (8192 bytes) is cleared to 0xFF, then the 49 (key, group) pairs of gaAutomapCellGroupPairs are
//  written as table[key] = group -- a sparse table written as a dense one, 0xFF meaning not present.
//  `nValue = 0` is done twice at the top and the first is dead. The leading `push ecx` only reserves a stack dword; it is not an argument.
void __cdecl AUTOMAP_LoadSettings()
{
    int32_t *pPair;
    int32_t nCellKey;
    int32_t nCellGroup;
    uint32_t nValue;

    nValue = 0;

    AUTOMAP_LoadIconCels();

    //  the whole array (0x800 dwords) and nothing past it
    memset(gaAutomapCellGroup, 0xFF, 0x2000);
    pPair = gaAutomapCellGroupPairs;
    do
    {
        nCellKey = pPair[0];
        nCellGroup = pPair[1];
        pPair += 2;
        gaAutomapCellGroup[nCellKey] = nCellGroup;
    }
    //  end of gaAutomapCellGroupPairs: the original compares with gnAutomapZoom's address (cmp eax, 6FB77BC0h); here it is the array plus its 98 ints
    while ((int32_t)pPair < (int32_t)(gaAutomapCellGroupPairs + 98));

    nValue = 0;
    STORM_SRegLoadValue(szDiabloIIKey, szAutoMapFade, 0, &nValue);
    gnAutomapFade = nValue;

    nValue = 1;
    STORM_SRegLoadValue(szDiabloIIKey, szValueName, 0, &nValue);
    gbAutomapCenters = nValue;

    nValue = 1;
    STORM_SRegLoadValue(szDiabloIIKey, szAutoMapParty, 0, &nValue);
    gbAutomapShowParty = nValue;

    nValue = 1;
    STORM_SRegLoadValue(szDiabloIIKey, szAutoMapPartyNames, 0, &nValue);
    gbAutomapPartyNames = nValue;

    nValue = 1;
    STORM_SRegLoadValue(szDiabloIIKey, szAutoMapLeft, 0, &nValue);
    gbAutomapLeft = nValue;
    gnAutomapLeftSaved = nValue;
    gnAutomapLeftSetting = nValue;

}


static_assert(sizeof(D2AutomapLayerStrc) == 28,
              "the tick allocates 28 bytes for a layer and memsets exactly "
              "that much -- `FOG_Alloc(28, automap.cpp, 1517, 0)`");


//1.10f: D2Client.0x6FACBAF0
//  The frame tick: reveals the rooms around the player on distance moved rather than on elapsed time, and switches layers when the player has
//  changed one. gnAutomapRevealDelay is a countdown and the whole body is in its else, so a non-zero delay costs one decrement. Past that, the
//  reveal happens only when the player has moved far enough since the last one, by an octagonal approximation of the distance:
//  dx = |lastX - x|, dy = |lastY - y|, approx = (dx <= dy) ? dx + 2 * dy : dy + 2 * dx, and the reveal is made when approx / 2 >= 80.
//  With no player the tick halts the client: FOG_DisplayAssert(szAssertHUnit, automap.cpp, 1686), then exit(-1).
//  The layer list has one 28-byte node per layer, not per level: DATATBLS_GetLevelDefRecord turns the room's level id into dwLayer, rooms that
//  share a layer share a node, the list is searched linearly, and a miss allocates and pushes the node at the head, so the most recently visited
//  layer is found first. The search is written with a goto into the else, as the original's graph is: the allocation is reached both by an empty
//  list and by the walk running off the end.
//  Switching layers calls AUTOMAP_RebuildDrawLists, frees the block chain, nulls the current layer's four lists and calls AUTOMAP_LoadCelData for
//  the new one. The block free is the same code as in AUTOMAP_Shutdown, and the whole layer switch is written out again in
//  AUTOMAP_RebuildAutomapLayer (twice) and AUTOMAP_AddTown (twice): the original has no out-of-line copy of it. The link of a cell block is the
//  dword at +10240 of a 10 KB block; the block is untyped and the link is read by pointer arithmetic.
void __cdecl AUTOMAP_Tick()
{
    D2UnitStrc *pPlayer;
    int32_t nPlayerY;
    int32_t nDeltaX;
    int32_t nDeltaY;
    int32_t nApproxDistance;
    D2ActiveRoomStrc *pRoom;
    D2LevelDefBin *pLevelDef;
    D2AutomapLayerStrc *pLayer;
    D2AutomapLayerStrc *pNewLayer;
    uint32_t dwLayer;
    int32_t nBlock;
    int32_t nBlockToFree;
    uint32_t i;
    D2ActiveRoomStrc **ppRoomList;
    int32_t nNumRooms;

    if (gnAutomapRevealDelay)
    {
        --gnAutomapRevealDelay;
        return;
    }

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        FOG_DisplayAssert(szAssertHUnit, szAutomapCpp, 1686);
        exit(-1);
    }

    gnAutomapPlayerX = D2COMMON_UNITS_GetClientCoordX(pPlayer);
    nPlayerY = D2COMMON_UNITS_GetClientCoordY(pPlayer);
    gnAutomapPlayerYUnused = nPlayerY;

    nDeltaX = gnAutomapLastRevealX - gnAutomapPlayerX;
    if ((int32_t)(gnAutomapLastRevealX - gnAutomapPlayerX) < 0)
        nDeltaX = gnAutomapPlayerX - gnAutomapLastRevealX;

    nDeltaY = gnAutomapLastRevealY - nPlayerY;
    if ((int32_t)(gnAutomapLastRevealY - nPlayerY) < 0)
        nDeltaY = nPlayerY - gnAutomapLastRevealY;

    if (nDeltaX <= nDeltaY)
        nApproxDistance = nDeltaX + 2 * nDeltaY;
    else
        nApproxDistance = nDeltaY + 2 * nDeltaX;

    if (nApproxDistance / 2 < 80)
        return;

    gnAutomapLastRevealX = gnAutomapPlayerX;
    gnAutomapLastRevealY = nPlayerY;

    pRoom = CUNIT_GetPlayerRoom();
    if (!pRoom)
        return;

    ppRoomList = NULL;
    nNumRooms = 0;
    D2COMMON_DUNGEON_GetAdjacentRoomsListFromRoom(pRoom, &ppRoomList, &nNumRooms);

    pLevelDef = D2COMMON_DATATBLS_GetLevelDefRecord(D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom));
    dwLayer = pLevelDef->dwLayer;

    pLayer = gpAutomapLayers;
    if (gpAutomapLayers)
    {
        while (pLayer->nLayerNo != dwLayer)
        {
            pLayer = pLayer->pNext;
            if (!pLayer)
                goto make_layer;
        }
    }
    else
    {
    make_layer:
        pNewLayer = (D2AutomapLayerStrc *)FOG_Alloc(28, szAutomapCpp, 1517, 0);
        memset(pNewLayer, 0, sizeof(D2AutomapLayerStrc));
        pNewLayer->nLayerNo = dwLayer;
        pNewLayer->pNext = gpAutomapLayers;
        gpAutomapLayers = pNewLayer;
        pLayer = pNewLayer;
    }

    if (pLayer != gpAutomapCurrentLayer)
    {
        AUTOMAP_RebuildDrawLists();

        if (gpAutomapCurrentLayer)
        {
            nBlock = (int32_t)(UINT_PTR)gpAutomapCellBlocks;
            while (nBlock)
            {
                nBlockToFree = nBlock;
                nBlock = *(uint32_t *)(nBlock + 10240);
                FOG_Free((void *)(UINT_PTR)nBlockToFree, szAutomapCpp, 1310, 0);
            }
            gpAutomapCellBlocks = 0;
            gnAutomapCellsUsed = 0;
            gpAutomapCurrentLayer->pFloors = NULL;
            gpAutomapCurrentLayer->pWalls = NULL;
            gpAutomapCurrentLayer->pObjects = NULL;
            gpAutomapCurrentLayer->pExtras = NULL;
        }

        gpAutomapCurrentLayer = pLayer;
        AUTOMAP_LoadCelData();
    }

    for (i = 0; i < (uint32_t)nNumRooms; ++i)
    {
        if (dwLayer == D2COMMON_DATATBLS_GetLevelDefRecord(D2COMMON_DUNGEON_GetLevelIdFromRoom(ppRoomList[i]))->dwLayer)
        {
            AUTOMAP_AddRoom(ppRoomList[i], 0, pLayer);
        }
    }
}


//1.10f: D2Client.0x6FACF670
//  Tears the automap down; called from UI_ShutdownUiSubsystems. Three walks: the cell blocks (the same free as in AUTOMAP_Tick), the icon cels and
//  the layer list. The layer walk takes the successor before freeing the node, in the for's increment clause.
//  void: eax at its exit is whichever walk ran last, and the one caller discards it.
void __cdecl AUTOMAP_Shutdown()
{
    int32_t nBlock;
    int32_t nBlockToFree;
    D2CellFileStrc **pIconCels;
    D2AutomapLayerStrc *pNext;
    D2AutomapLayerStrc *pLayer;

    AUTOMAP_RebuildDrawLists();

    if (gpAutomapCurrentLayer)
    {
        nBlock = (int32_t)(UINT_PTR)gpAutomapCellBlocks;
        while (nBlock)
        {
            nBlockToFree = nBlock;
            nBlock = *(uint32_t *)(nBlock + 10240);
            FOG_Free((void *)(UINT_PTR)nBlockToFree, szAutomapCpp, 1310, 0);
        }
        gpAutomapCellBlocks = 0;
        gnAutomapCellsUsed = 0;
        gpAutomapCurrentLayer->pFloors = NULL;
        gpAutomapCurrentLayer->pWalls = NULL;
        gpAutomapCurrentLayer->pObjects = NULL;
        gpAutomapCurrentLayer->pExtras = NULL;
    }

    pIconCels = gpAutomapIconCels;
    do
    {
        if (*pIconCels)
            ARCHIVE_CellFileFree(*pIconCels);
        *pIconCels++ = 0;
    }
    //  end of gpAutomapIconCels, as in AUTOMAP_LoadIconCels: the array plus its 4 cels
    while ((int32_t)pIconCels < (int32_t)(gpAutomapIconCels + 4));

    //  the successor is taken before the node is freed, in the increment
    for (pNext = gpAutomapLayers; pNext; FOG_Free(pLayer, szAutomapCpp, 3098, 0))
    {
        pLayer = pNext;
        pNext = pNext->pNext;
    }

    gpAutomapLayers = NULL;
    gpAutomapCurrentLayer = NULL;
}


//1.10f: D2Client.0x6FACF730
//  Resolves the eleven symbol colours once. Three callers, all of which discard the answer and all of which call the next initialiser in a run of
//  them. Every entry is a literal RGB triple; entries 0 and 9 are both pure blue and 3 and 10 both pure green, so the eleven slots carry nine
//  colours.
void __cdecl AUTOMAP_InitSymbolColors()
{
    gAutomapSymbolColors[0] = D2WIN_10034_MixRGB(0, 0, 0xFF);
    gAutomapSymbolColors[1] = D2WIN_10034_MixRGB(0xFF, 0, 0);
    gAutomapSymbolColors[2] = D2WIN_10034_MixRGB(0xFF, 0, 0xFF);
    gAutomapSymbolColors[3] = D2WIN_10034_MixRGB(0, 0xFF, 0);
    gAutomapSymbolColors[4] = D2WIN_10034_MixRGB(0x44, 0x70, 0x74);
    gAutomapSymbolColors[5] = D2WIN_10034_MixRGB(0x48, 0xA0, 0x34);
    gAutomapSymbolColors[6] = D2WIN_10034_MixRGB(0xF4, 0xF4, 0xF4);
    gAutomapSymbolColors[7] = D2WIN_10034_MixRGB(0x80, 0x80, 0x80);
    gAutomapSymbolColors[8] = D2WIN_10034_MixRGB(0xF4, 0xF4, 0);
    gAutomapSymbolColors[9] = D2WIN_10034_MixRGB(0, 0, 0xFF);
    gAutomapSymbolColors[10] = D2WIN_10034_MixRGB(0, 0xFF, 0);
}


//  The cell tree: the automap's store, an AVL tree of 20-byte cells. The assert texts name its fields: ptLess, ptMore and sBalence are the
//  original's names for the cell's pPrev, pNext and wWeight. BALANCE_LHIGH is -1 and BALANCE_MHIGH is +1 (the asserts fire unless the value is one
//  of those two); the balanced state has no known name, so 0 is written as a literal.
//  The ordering key is three-level: AUTOMAP_RebalanceTree compares yPixel, then xPixel, then, only when both are equal, nCellNo, and that third
//  level is guarded: if gaAutomapCellGroup[nCellNo] is -1 or equals the group of the root, the insertion is dropped (the subtree is written back
//  unchanged and 0 is returned). A dropped cell never enters the tree and is never freed; it stays in the block pool until the whole pool is
//  released, which is why gnAutomapCellsUsed climbs faster than the tree grows. All three comparisons sign-extend (movsx on +4, +6 and +8); the
//  struct declares those fields uint16_t, so the casts in the code are what the instructions do.
//  The block pool is a singly linked list of 10244-byte blocks: 512 cells of 20 bytes and a four-byte link at +10240. There is no struct for a
//  block; the three places that walk the list (here, the tick's layer switch and AUTOMAP_Shutdown) all do the same raw arithmetic.

static_assert(sizeof(D2AutomapCellStrc) == 20,
              "AUTOMAP_AllocCell hands out &pBlock[5 * i] out of a 10244-byte "
              "block whose link sits at +10240: 512 * 20 + 4 = 10244");


//1.10f: D2Client.0x6FACBA40
//  Hands out the next cell from the block pool; every path that puts anything on the automap comes through here. The counter gnAutomapCellsUsed
//  divides by 512 to pick the block and its remainder picks the slot; the walk stops early if the list is shorter than the quotient, and the
//  allocation below covers both that case and the empty list. The division is signed (gnAutomapCellsUsed is a signed int). No caller tests the
//  return.
D2AutomapCellStrc *__cdecl AUTOMAP_AllocCell()
{
    uint32_t *pBlock;
    uint32_t *pPrevBlock;
    int32_t iBlock;
    int32_t iCellInBlock;
    int32_t iScan;
    D2AutomapCellStrc *pCell;

    iBlock = gnAutomapCellsUsed / 512;
    iCellInBlock = gnAutomapCellsUsed % 512;
    pBlock = (uint32_t *)gpAutomapCellBlocks;
    pPrevBlock = NULL;
    ++gnAutomapCellsUsed;

    for (iScan = 0; pBlock && iScan < iBlock; ++iScan)
    {
        pPrevBlock = pBlock;
        pBlock = (uint32_t *)pBlock[2560];
    }

    if (!pBlock)
    {
        pBlock = (uint32_t *)FOG_Alloc(10244, szAutomapCpp, 861, 0);
        memset(pBlock, 0, 10244);

        if (pPrevBlock)
            pPrevBlock[2560] = (uint32_t)pBlock;
        else
            gpAutomapCellBlocks = pBlock;
    }

    //  clears the 20-byte cell as five dword stores, the original's expansion of a constant-size memset
    pCell = (D2AutomapCellStrc *)&pBlock[5 * iCellInBlock];
    memset(pCell, 0, sizeof(D2AutomapCellStrc));
    return pCell;
}


//1.10f: D2Client.0x6FACD0C0
//  AVL left rotation: promotes pNext, moves its pPrev onto the root's pNext and hangs the old root under it. The two asserts are at lines 563 and
//  564 of the original file. The new subtree root is also written back through the parameter.
void __fastcall AUTOMAP_RotateLeft(D2AutomapCellStrc **pptRoot)
{
    D2AutomapCellStrc *pRoot;
    D2AutomapCellStrc *pNewRoot;

    pRoot = *pptRoot;
    if (!pRoot)
    {
        FOG_DisplayAssert(szAssertPtRootNull, szAutomapCpp, 563);
        exit(-1);
    }

    pNewRoot = pRoot->pNext;
    if (!pNewRoot)
    {
        FOG_DisplayAssert(szAssertPtMoreNull, szAutomapCpp, 564);
        exit(-1);
    }

    pRoot->pNext = pNewRoot->pPrev;
    pNewRoot->pPrev = pRoot;
    *pptRoot = pNewRoot;
}


//1.10f: D2Client.0x6FACD120
//  AVL right rotation, the mirror of AUTOMAP_RotateLeft with pNext and pPrev swapped; its asserts are at lines 580 and 581.
void __fastcall AUTOMAP_RotateRight(D2AutomapCellStrc **pptRoot)
{
    D2AutomapCellStrc *pRoot;
    D2AutomapCellStrc *pNewRoot;

    pRoot = *pptRoot;
    if (!pRoot)
    {
        FOG_DisplayAssert(szAssertPtRootNull, szAutomapCpp, 580);
        exit(-1);
    }

    pNewRoot = pRoot->pPrev;
    if (!pNewRoot)
    {
        FOG_DisplayAssert(szAssertPtLessNull, szAutomapCpp, 581);
        exit(-1);
    }

    pRoot->pPrev = pNewRoot->pNext;
    pNewRoot->pNext = pRoot;
    *pptRoot = pNewRoot;
}


//1.10f: D2Client.0x6FACCD50
//  The AVL insert: inserts ptNew under *ppRoot, rebalances on the way back up and returns whether the subtree got taller (1 while the height
//  change still has to propagate, 0 once a rotation absorbs it; only the function's own two recursive calls read it). ptNew is the cell being
//  inserted: AUTOMAP_AddTile fills +4, +6 and +8 of a fresh cell and passes it here, and the empty-subtree arm makes it the root.
//  The original inlines AUTOMAP_RotateRight once and AUTOMAP_RotateLeft twice here; ours calls them at all six rotation sites.
//  The two asserts that are this function's own are at lines 601 and 660: a child that is neither BALANCE_LHIGH nor BALANCE_MHIGH when the parent
//  is already leaning that way is an impossible state, so the code takes the two-case shape and asserts the third away.
//  The three-way weight fix-ups below have no else. When a grandchild's balance matches none of the three arms, both parents keep their weights
//  and only the grandchild is cleared; that case cannot arise (a child of a rotation site is always -1, 0 or 1) and the original has no default.
int32_t __fastcall AUTOMAP_RebalanceTree(D2AutomapCellStrc *ptNew, D2AutomapCellStrc **ppRoot)
{
    D2AutomapCellStrc *ptRoot;
    D2AutomapCellStrc *ptLess;
    D2AutomapCellStrc *ptMore;
    D2AutomapCellStrc *ptGrand;
    int32_t nCompare;
    int32_t bTaller;

    ptRoot = *ppRoot;
    if (!ptRoot)
    {
        ptNew->pPrev = NULL;
        ptNew->pNext = NULL;
        ptNew->wWeight = 0;
        *ppRoot = ptNew;
        return 1;
    }

    nCompare = (int16_t)ptNew->yPixel - (int16_t)ptRoot->yPixel;
    if (!nCompare)
    {
        nCompare = (int16_t)ptNew->xPixel - (int16_t)ptRoot->xPixel;
        if (!nCompare)
        {
            if (gaAutomapCellGroup[(int16_t)ptNew->nCellNo] == -1 || gaAutomapCellGroup[(int16_t)ptNew->nCellNo] == gaAutomapCellGroup[(int16_t)ptRoot->nCellNo])
            {
                *ppRoot = ptRoot;
                return 0;
            }

            nCompare = (int16_t)ptNew->nCellNo - (int16_t)ptRoot->nCellNo;
        }
    }

    if (nCompare < 0)
    {
        bTaller = AUTOMAP_RebalanceTree(ptNew, &ptRoot->pPrev);
        if (!bTaller)
        {
            *ppRoot = ptRoot;
            return bTaller;
        }

        if ((int16_t)ptRoot->wWeight != -1)
        {
            if ((int16_t)ptRoot->wWeight == 0)
            {
                ptRoot->wWeight = -1;
                *ppRoot = ptRoot;
                return bTaller;
            }

            if ((int16_t)ptRoot->wWeight == 1)
            {
                ptRoot->wWeight = 0;
                *ppRoot = ptRoot;
                return 0;
            }

            *ppRoot = ptRoot;
            return bTaller;
        }

        ptLess = ptRoot->pPrev;
        if ((int16_t)ptLess->wWeight == -1)
        {
            ptRoot->wWeight = 0;
            ptLess->wWeight = 0;
            AUTOMAP_RotateRight(&ptRoot);
            *ppRoot = ptRoot;
            return 0;
        }

        if ((int16_t)ptLess->wWeight != 1)
        {
            FOG_DisplayAssert(szAssertPtLessBalence, szAutomapCpp, 601);
            exit(-1);
        }

        ptGrand = ptLess->pNext;
        if ((int16_t)ptGrand->wWeight == -1)
        {
            ptRoot->wWeight = 1;
            ptLess->wWeight = 0;
        }
        else if ((int16_t)ptGrand->wWeight == 0)
        {
            ptRoot->wWeight = 0;
            ptLess->wWeight = 0;
        }
        else if ((int16_t)ptGrand->wWeight == 1)
        {
            ptRoot->wWeight = 0;
            ptLess->wWeight = -1;
        }

        ptGrand->wWeight = 0;
        AUTOMAP_RotateLeft(&ptRoot->pPrev);
        AUTOMAP_RotateRight(&ptRoot);
        *ppRoot = ptRoot;
        return 0;
    }

    if (nCompare > 0)
    {
        bTaller = AUTOMAP_RebalanceTree(ptNew, &ptRoot->pNext);
        if (!bTaller)
        {
            *ppRoot = ptRoot;
            return bTaller;
        }

        if ((int16_t)ptRoot->wWeight == -1)
        {
            ptRoot->wWeight = 0;
            *ppRoot = ptRoot;
            return 0;
        }

        if ((int16_t)ptRoot->wWeight == 0)
        {
            ptRoot->wWeight = 1;
            *ppRoot = ptRoot;
            return bTaller;
        }

        if ((int16_t)ptRoot->wWeight != 1)
        {
            *ppRoot = ptRoot;
            return bTaller;
        }

        ptMore = ptRoot->pNext;
        if ((int16_t)ptMore->wWeight == -1)
        {
            ptGrand = ptMore->pPrev;
            if ((int16_t)ptGrand->wWeight == -1)
            {
                ptRoot->wWeight = 0;
                ptMore->wWeight = 1;
            }
            else if ((int16_t)ptGrand->wWeight == 0)
            {
                ptRoot->wWeight = 0;
                ptMore->wWeight = 0;
            }
            else if ((int16_t)ptGrand->wWeight == 1)
            {
                ptRoot->wWeight = -1;
                ptMore->wWeight = 0;
            }

            ptGrand->wWeight = 0;
            AUTOMAP_RotateRight(&ptRoot->pNext);
            AUTOMAP_RotateLeft(&ptRoot);
            *ppRoot = ptRoot;
            return 0;
        }

        if ((int16_t)ptMore->wWeight != 1)
        {
            FOG_DisplayAssert(szAssertPtMoreBalence, szAutomapCpp, 660);
            exit(-1);
        }

        ptRoot->wWeight = 0;
        ptMore->wWeight = 0;
        AUTOMAP_RotateLeft(&ptRoot);
        *ppRoot = ptRoot;
        return 0;
    }

    *ppRoot = ptRoot;
    return 0;
}


//1.10f: D2Client.0x6FACC580
//  Measures one cell tree: adds six bytes per live cell to *pnSize; a cell whose fSaved is set is already in the saved map and is not counted.
//  A plain in-order walk with two recursive calls (the original's second, a tail call, became a loop). AUTOMAP_WriteCellTree has the same shape
//  and the two must agree or the buffer is the wrong size. The original inlines four copies of this walk in AUTOMAP_RebuildDrawLists.
void __fastcall AUTOMAP_MeasureCellTree(D2AutomapCellStrc *pNode, int32_t *pnSize)
{
    if (!pNode)
        return;

    AUTOMAP_MeasureCellTree(pNode->pPrev, pnSize);

    if (!pNode->fSaved) *pnSize += 6;

    AUTOMAP_MeasureCellTree(pNode->pNext, pnSize);
}


//1.10f: D2Client.0x6FACC5B0
//  Serialises one cell tree: the same walk as the measure above, writing three words per live cell (nCellNo, xPixel, yPixel; six bytes, as the
//  measure counted) at gpAutomapDrawWriteCursor and advancing it. A draw buffer is therefore a flat run of {cel, x, y} triples in tree order,
//  that is sorted by y, then x, then cel. It takes the node in ecx and nothing else (a one-argument __thiscall is the same code as __fastcall).
void __fastcall AUTOMAP_WriteCellTree(D2AutomapCellStrc *pNode)
{
    if (!pNode)
        return;

    AUTOMAP_WriteCellTree(pNode->pPrev);

    if (!pNode->fSaved)
    {
        *(uint16_t *)gpAutomapDrawWriteCursor = pNode->nCellNo;
        gpAutomapDrawWriteCursor += 2;
        *(uint16_t *)gpAutomapDrawWriteCursor = pNode->xPixel;
        gpAutomapDrawWriteCursor += 2;
        *(uint16_t *)gpAutomapDrawWriteCursor = pNode->yPixel;
        gpAutomapDrawWriteCursor += 2;
    }

    AUTOMAP_WriteCellTree(pNode->pNext);
}


//  The functions that put things on the map. AUTOMAP_RebuildAutomapLayer and AUTOMAP_AddTown are the level generator's two automap callbacks;
//  neither has a code caller in this unit. GAME_PreloadActData passes both to DUNGEON_AllocAct, which stores them on the generator, and D2Common
//  picks between them by level id: LEVEL_LUTGHOLEIN, LEVEL_THEPANDEMONIUMFORTRESS and LEVEL_HARROGATH get the town callback, every other level
//  the per-room one.
//  nPixelX, nPixelY and nCelNum are checked against SHRT_MIN and SHRT_MAX before every store into a cell (limits.h macros): the cell's three fields
//  are 16 bits wide and the caller must narrow safely.

static_assert(sizeof(D2DrlgTileDataStrc) == 48,
              "AUTOMAP_AddRoom strides the tile array with `add esi, 30h` "
              "and reaches the record base with `lea ecx, [esi-14h]`");


//1.10f: D2Client.0x6FACD3C0
//  Adds one tile to a layer's tree; the third argument is the address of the tree root, so an empty tree can be replaced in place. It returns at
//  once if the already-on-the-automap bit (0x40000 of the tile flags) is set, and sets it otherwise, so a tile is added once.
//  The cel is looked up from the level's automap type, the tile's type, its style and its sequence; -1 means the tile has no automap drawing,
//  which is the common case. The tile's own position is added to the room's origin, converted to client coordinates and divided by ten; the
//  division is signed (imul 66666667h, sar, at 0x6FACD457), so a tile west or north of the origin keeps its sign.
//  BUG: a tile whose nTileType is 16 or more is moved 24 pixels lower after the range assert on the un-offset value, so a y close to SHRT_MAX would
//  pass the check and then overflow the short. LATENT: the automap's pixel coordinates are a tenth of client coordinates and never come near 32767.
void __fastcall AUTOMAP_AddTile(D2DrlgTileDataStrc *pTile, D2DrlgRoomStrc *pRoom, D2AutomapCellStrc **ppTreeRoot)
{
    uint32_t dwAutomapType;
    uint32_t dwTileType;
    int32_t nStyle;
    int32_t nSequence;
    int32_t nCelNum;
    int32_t nPixelX;
    int32_t nPixelY;
    int32_t pX;
    int32_t pY;
    D2AutomapCellStrc *pCell;

    if ((pTile->dwFlags & 0x40000) != 0)
        return;

    pTile->dwFlags |= 0x40000;

    dwAutomapType = D2COMMON_DRLG_GetLevelTypeFromLevelId((D2C_Levels)pRoom->pLevel->nLevelId);
    dwTileType = D2CMP_10077_GetTileType((uint32_t)(UINT_PTR)pTile->pTile);
    nStyle = D2CMP_10078_GetTileStyle((uint32_t)(UINT_PTR)pTile->pTile);
    nSequence = D2CMP_10082_GetTileSequence((uint32_t)(UINT_PTR)pTile->pTile);

    nCelNum = D2COMMON_DATATBLS_GetAutomapCellId(dwAutomapType, dwTileType, nStyle, nSequence);
    if (nCelNum == -1)
        return;

    pCell = AUTOMAP_AllocCell();

    //  the room's tile origin sits two anonymous unions down in the room struct
    pX = pRoom->nTileXPos + pTile->nPosX;
    pY = pRoom->nTileYPos + pTile->nPosY;
    D2COMMON_DUNGEON_GameTileToClientCoords(&pX, &pY);

    nPixelX = pX / 10;
    nPixelY = pY / 10;
    pX = nPixelX;
    pY = nPixelY;

    if (nPixelX < -32768 || nPixelX > 0x7FFF)
    {
        FOG_DisplayAssert(szAssertPixelX, szAutomapCpp, 913);
        exit(-1);
    }

    if (nPixelY < -32768 || nPixelY > 0x7FFF)
    {
        FOG_DisplayAssert(szAssertPixelY, szAutomapCpp, 914);
        exit(-1);
    }

    if (nCelNum < -32768 || nCelNum > 0x7FFF)
    {
        FOG_DisplayAssert(szAssertCelNum, szAutomapCpp, 915);
        exit(-1);
    }

    //  the upper storey drops 24 pixels, applied after the assert
    if (pTile->nTileType >= 16)
        pY = nPixelY + 24;

    pCell->nCellNo = (uint16_t)nCelNum;
    pCell->xPixel = (uint16_t)pX;
    pCell->yPixel = (uint16_t)pY;
    AUTOMAP_RebalanceTree(pCell, ppTreeRoot);
}


//1.10f: D2Client.0x6FACD560
//  Adds one unit's marker to a layer's tree: the same shape as AUTOMAP_AddTile with the cel handed in rather than looked up. The marker sits one
//  pixel right and three up from the unit's position, which centres a small icon on a foot position.
void __fastcall AUTOMAP_AddUnit(D2UnitStrc *pUnit, int32_t nCelNum, D2AutomapCellStrc **ppTreeRoot)
{
    D2AutomapCellStrc *pCell;
    int32_t nPixelX;
    int32_t nPixelY;

    pCell = AUTOMAP_AllocCell();
    nPixelX = D2COMMON_UNITS_GetClientCoordX(pUnit) / 10 + 1;
    nPixelY = D2COMMON_UNITS_GetClientCoordY(pUnit) / 10 - 3;

    if (nPixelX < -32768 || nPixelX > 0x7FFF)
    {
        FOG_DisplayAssert(szAssertPixelX, szAutomapCpp, 965);
        exit(-1);
    }

    if (nPixelY < -32768 || nPixelY > 0x7FFF)
    {
        FOG_DisplayAssert(szAssertPixelY, szAutomapCpp, 966);
        exit(-1);
    }

    if (nCelNum < -32768 || nCelNum > 0x7FFF)
    {
        FOG_DisplayAssert(szAssertCelNum, szAutomapCpp, 967);
        exit(-1);
    }

    pCell->nCellNo = (uint16_t)nCelNum;
    pCell->xPixel = (uint16_t)nPixelX;
    pCell->yPixel = (uint16_t)nPixelY;
    AUTOMAP_RebalanceTree(pCell, ppTreeRoot);
}


//1.10f: D2Client.0x6FACD650
//  Starts a delay of two ticks after a warp. Its one caller is CUNIT_WarpUnit; the delay lets the room list be rebuilt at the destination before
//  AUTOMAP_Tick walks it.
void __cdecl AUTOMAP_DelayReveal()
{
    gnAutomapRevealDelay = 2;
}


//1.10f: D2Client.0x6FACD180
//  Adds one room's tiles and units to a layer. Floors go into pFloors and walls into pWalls, both by the address of the tree root; units go into
//  pObjects. A tile goes on the map when bit 3 of its flags is clear and any of three things is true: its own bit 17 is set, the global reveal is
//  on, or bRevealAll was passed. A unit goes on when bit 28 is set and bit 29 is not, and bit 29 is then set so it is added once -- unless the
//  global reveal is on, which skips the test and re-adds the unit every time the room is walked. This is the only reader of the tile flags.
//  The cel of a unit: a monster contributes MonStats2's dwAutomapCel, reached through MonStats's wMonStatsEx, with both indices bounds-checked
//  against the table's own record counts; an object contributes Objects.txt's dwAutomap, and three objects are conditional (267 only in acts 2
//  and 3, 366 only while its animation mode is 2, 402 only on level 74); every other object with a non-zero dwAutomap is added unconditionally.
//  The two tile loops compare unsigned (jbe, jc), so a negative count from DUNGEON_GetFloorTilesFromRoom would run about four billion times
//  instead of being skipped; it cannot return one.
void __fastcall AUTOMAP_AddRoom(D2ActiveRoomStrc *pRoom, int32_t bRevealAll, D2AutomapLayerStrc *pLayer)
{
    D2DrlgRoomStrc *pRoomEx;
    D2DrlgTileDataStrc *pTiles;
    uint32_t i;
    int32_t nTileCount;
    D2UnitStrc *pUnit;
    int32_t nClassId;
    int32_t nCel;
    D2ObjectsTxt *pObjectsTxt;
    int32_t nMonStatsEx;
    D2MonStats2Txt *pMonStats2Txt;
    uint8_t nActNo;

    nTileCount = 0;
    pRoomEx = D2COMMON_DUNGEON_GetRoomExFromRoom(pRoom);

    pTiles = D2COMMON_DUNGEON_GetFloorTilesFromRoom(pRoom, &nTileCount);
    for (i = 0; i < (uint32_t)nTileCount; ++i)
    {
        if ((pTiles[i].dwFlags & 8) == 0 && ((pTiles[i].dwFlags & 0x20000) != 0 || gbAutomapRevealAll || bRevealAll))
        {
            AUTOMAP_AddTile(&pTiles[i], pRoomEx, &pLayer->pFloors);
        }
    }

    pTiles = D2COMMON_DUNGEON_GetWallTilesFromRoom(pRoom, &nTileCount);
    for (i = 0; i < (uint32_t)nTileCount; ++i)
    {
        if ((pTiles[i].dwFlags & 8) == 0 && ((pTiles[i].dwFlags & 0x20000) != 0 || gbAutomapRevealAll || bRevealAll))
        {
            AUTOMAP_AddTile(&pTiles[i], pRoomEx, &pLayer->pWalls);
        }
    }

    for (pUnit = pRoom->pUnitFirst; pUnit; pUnit = pUnit->pRoomNext)
    {
        if (!gbAutomapRevealAll && (((pUnit->dwFlags >> 28) & 1) == 0 || ((pUnit->dwFlags >> 29) & 1) != 0))
        {
            continue;
        }

        pUnit->dwFlags |= 0x20000000;

        if (pUnit->dwUnitType == UNIT_MONSTER)
        {
            nClassId = pUnit->dwClassId;
            if (nClassId < 0 || nClassId >= sgptDataTables->nMonStatsTxtRecordCount)
            {
                continue;
            }

            nMonStatsEx = (int16_t)sgptDataTables->pMonStatsTxt[nClassId].wMonStatsEx;
            if (nMonStatsEx < 0 || nMonStatsEx >= sgptDataTables->nMonStats2TxtRecordCount)
            {
                continue;
            }

            pMonStats2Txt = &sgptDataTables->pMonStats2Txt[nMonStatsEx];
            if (!pMonStats2Txt)
                continue;

            nCel = pMonStats2Txt->dwAutomapCel;
            if (!nCel)
                continue;

            AUTOMAP_AddUnit(pUnit, nCel, &pLayer->pObjects);
        }
        else if (pUnit->dwUnitType == UNIT_OBJECT)
        {
            nClassId = pUnit->dwClassId;
            pObjectsTxt = D2COMMON_DATATBLS_GetObjectsTxtRecord(nClassId);

            nCel = pObjectsTxt->dwAutomap;
            if (!nCel)
                continue;

            switch (nClassId)
            {
            case 267:
                nActNo = D2COMMON_DRLG_GetActNoFromLevelId((D2C_Levels)D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom));
                if (nActNo == 2 || nActNo == 3)
                    AUTOMAP_AddUnit(pUnit, pObjectsTxt->dwAutomap, &pLayer->pObjects);
                break;

            case 366:
                if (pUnit->dwAnimMode == 2)
                    AUTOMAP_AddUnit(pUnit, nCel, &pLayer->pObjects);
                break;

            case 402:
                if (D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom) == 74)
                    AUTOMAP_AddUnit(pUnit, pObjectsTxt->dwAutomap, &pLayer->pObjects);
                break;

            default:
                AUTOMAP_AddUnit(pUnit, nCel, &pLayer->pObjects);
                break;
            }
        }
    }
}


//1.10f: D2Client.0x6FACD660
//  The level generator's per-room automap callback, for a preset level that is not one of the three towns with a drawn map; D2Common calls it once
//  per room. It remembers which layer was current, switches to the room's own layer, adds the room with bRevealAll set and switches back. The
//  layer switch (find the layer for this level; if it is not the current one, free the cell pool and load the new one) is written out in
//  place twice here. Each switch that actually changes layers throws the whole cell pool away and reloads the cel data, so a preset level
//  generated while the player stands somewhere else pays that twice.
void __stdcall AUTOMAP_RebuildAutomapLayer(D2ActiveRoomStrc *pRoom)
{
    uint32_t nPreviousLayerNo;
    uint32_t dwLayer;
    D2AutomapLayerStrc *pLayer;
    D2AutomapLayerStrc *pNewLayer;
    int32_t nBlock;
    int32_t nBlockToFree;

    if (gpAutomapCurrentLayer)
        nPreviousLayerNo = gpAutomapCurrentLayer->nLayerNo;
    else
        nPreviousLayerNo = -1;

    dwLayer = D2COMMON_DATATBLS_GetLevelDefRecord(D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom))->dwLayer;

    for (pLayer = gpAutomapLayers; pLayer; pLayer = pLayer->pNext)
    {
        if (pLayer->nLayerNo == dwLayer)
            break;
    }

    if (!pLayer)
    {
        pNewLayer = (D2AutomapLayerStrc *)FOG_Alloc(28, szAutomapCpp, 1517, 0);
        memset(pNewLayer, 0, sizeof(D2AutomapLayerStrc));
        pNewLayer->nLayerNo = dwLayer;
        pNewLayer->pNext = gpAutomapLayers;
        gpAutomapLayers = pNewLayer;
        pLayer = pNewLayer;
    }

    if (pLayer != gpAutomapCurrentLayer)
    {
        AUTOMAP_RebuildDrawLists();

        if (gpAutomapCurrentLayer)
        {
            nBlock = (int32_t)(UINT_PTR)gpAutomapCellBlocks;
            while (nBlock)
            {
                nBlockToFree = nBlock;
                nBlock = *(uint32_t *)(nBlock + 10240);
                FOG_Free((void *)(UINT_PTR)nBlockToFree, szAutomapCpp, 1310, 0);
            }
            gpAutomapCellBlocks = 0;
            gnAutomapCellsUsed = 0;
            gpAutomapCurrentLayer->pFloors = NULL;
            gpAutomapCurrentLayer->pWalls = NULL;
            gpAutomapCurrentLayer->pObjects = NULL;
            gpAutomapCurrentLayer->pExtras = NULL;
        }

        gpAutomapCurrentLayer = pLayer;
        AUTOMAP_LoadCelData();
    }

    AUTOMAP_AddRoom(pRoom, 1, pLayer);

    if (nPreviousLayerNo == -1)
        return;

    //  switch back to the layer that was current
    for (pLayer = gpAutomapLayers; pLayer; pLayer = pLayer->pNext)
    {
        if (pLayer->nLayerNo == nPreviousLayerNo)
            break;
    }

    if (!pLayer)
    {
        pNewLayer = (D2AutomapLayerStrc *)FOG_Alloc(28, szAutomapCpp, 1517, 0);
        memset(pNewLayer, 0, sizeof(D2AutomapLayerStrc));
        pNewLayer->nLayerNo = nPreviousLayerNo;
        pNewLayer->pNext = gpAutomapLayers;
        gpAutomapLayers = pNewLayer;
        pLayer = pNewLayer;
    }

    if (pLayer != gpAutomapCurrentLayer)
    {
        AUTOMAP_RebuildDrawLists();

        if (gpAutomapCurrentLayer)
        {
            nBlock = (int32_t)(UINT_PTR)gpAutomapCellBlocks;
            while (nBlock)
            {
                nBlockToFree = nBlock;
                nBlock = *(uint32_t *)(nBlock + 10240);
                FOG_Free((void *)(UINT_PTR)nBlockToFree, szAutomapCpp, 1310, 0);
            }
            gpAutomapCellBlocks = 0;
            gnAutomapCellsUsed = 0;
            gpAutomapCurrentLayer->pFloors = NULL;
            gpAutomapCurrentLayer->pWalls = NULL;
            gpAutomapCurrentLayer->pObjects = NULL;
            gpAutomapCurrentLayer->pExtras = NULL;
        }

        gpAutomapCurrentLayer = pLayer;
        AUTOMAP_LoadCelData();
    }
}


//1.10f: D2Client.0x6FACD840
//  The level generator's town callback, for the three towns with a drawn map, and the only function in this unit that plots cels from a table
//  rather than from the dungeon. A town's map is a fixed grid of cels laid around the level's centre; the cel number counts up across the grid,
//  left to right and top to bottom, and everything goes into pExtras, the layer's fourth tree:
//
//      level  grid   pitch X  pitch Y  offset X  offset Y  fSaved  skips
//         40  5 x 4      160      100        53       -81       1  yes
//        103  2 x 2      136       90        -3       -50       2  none
//        109  3 x 2      180      170       -20      -155       3  none
//
//  fSaved is not a flag here: the layer's +0x04 gets 1, 2 or 3 to record which drawn town map the layer carries. Level 40 is the only one with
//  holes: gaAutomapTownSkipCels lists the cels it does not plot, and it covers two twenty-cel drawings because the act 2 town has more than one
//  preset .ds1 and the cel base is 0 or 20 accordingly.
//  The default arm of the level switch asserts and halts the client, and it cannot fire through the shipped call site: D2Common calls this only for
//  levels 40, 103 and 109. Five preset levels carry AutoMap in LvlPrest.txt (1, 40, 75, 103 and 109), so levels 1 and 75 would land on the default
//  arm if that guard were not there.
//  pCell->fSaved = 0 is written again although AUTOMAP_AllocCell has just zeroed the whole record; the original does it. In every arm of the
//  switch the original also loads gnAutomapMode and computes gnAutomapMode - 1, which nothing reads; nothing is written for it.
void __stdcall AUTOMAP_AddTown(int32_t nLevelId, int32_t nPickedFile, int32_t nCenterX, int32_t nCenterY)
{
    uint32_t nPreviousLayerNo;
    uint32_t dwLayer;
    D2AutomapLayerStrc *pLayer;
    D2AutomapLayerStrc *pNewLayer;
    int32_t nBlock;
    int32_t nBlockToFree;
    const int32_t *pSkipCels;
    const int32_t *pSkipCel;
    int32_t nCelNum;
    int32_t nCellsX;
    int32_t nCellsY;
    int32_t nCellPitchX;
    int32_t nCellPitchY;
    int32_t nOriginOffsetX;
    int32_t nOriginOffsetY;
    int32_t nHalfWidth;
    int32_t nPixelX;
    int32_t nPixelY;
    int32_t iRow;
    int32_t iCol;
    D2AutomapCellStrc *pCell;

    nCelNum = 0;

    if (gpAutomapCurrentLayer)
        nPreviousLayerNo = gpAutomapCurrentLayer->nLayerNo;
    else
        nPreviousLayerNo = -1;

    dwLayer = D2COMMON_DATATBLS_GetLevelDefRecord(nLevelId)->dwLayer;

    for (pLayer = gpAutomapLayers; pLayer; pLayer = pLayer->pNext)
    {
        if (pLayer->nLayerNo == dwLayer)
            break;
    }

    if (!pLayer)
    {
        pNewLayer = (D2AutomapLayerStrc *)FOG_Alloc(28, szAutomapCpp, 1517, 0);
        memset(pNewLayer, 0, sizeof(D2AutomapLayerStrc));
        pNewLayer->nLayerNo = dwLayer;
        pNewLayer->pNext = gpAutomapLayers;
        gpAutomapLayers = pNewLayer;
        pLayer = pNewLayer;
    }

    if (pLayer != gpAutomapCurrentLayer)
    {
        AUTOMAP_RebuildDrawLists();

        if (gpAutomapCurrentLayer)
        {
            nBlock = (int32_t)(UINT_PTR)gpAutomapCellBlocks;
            while (nBlock)
            {
                nBlockToFree = nBlock;
                nBlock = *(uint32_t *)(nBlock + 10240);
                FOG_Free((void *)(UINT_PTR)nBlockToFree, szAutomapCpp, 1310, 0);
            }
            gpAutomapCellBlocks = 0;
            gnAutomapCellsUsed = 0;
            gpAutomapCurrentLayer->pFloors = NULL;
            gpAutomapCurrentLayer->pWalls = NULL;
            gpAutomapCurrentLayer->pObjects = NULL;
            gpAutomapCurrentLayer->pExtras = NULL;
        }

        gpAutomapCurrentLayer = pLayer;
        AUTOMAP_LoadCelData();
    }

    switch (nLevelId)
    {
    case 40:                                    //  Lut Gholein, act 2
        pLayer->fSaved = 1;
        pSkipCels = gaAutomapTownSkipCels;
        nCellPitchY = 100;
        nCellPitchX = 160;
        nOriginOffsetY = -81;
        nOriginOffsetX = 53;
        nCellsY = 4;
        nCellsX = 5;
        nCelNum = (nPickedFile != 1) ? 0x14 : 0;
        break;

    case 103:                                   //  the Pandemonium Fortress
        nCellsX = 2;
        pSkipCels = NULL;
        pLayer->fSaved = 2;
        nCellPitchY = 90;
        nCellPitchX = 136;
        nOriginOffsetY = -50;
        nOriginOffsetX = -3;
        nCellsY = 2;
        break;

    case 109:                                   //  Harrogath, act 5
        nCellsX = 3;
        pSkipCels = NULL;
        pLayer->fSaved = 3;
        nCellPitchY = 170;
        nCellPitchX = 180;
        nOriginOffsetY = -155;
        nOriginOffsetX = -20;
        nCellsY = 2;
        break;

    default:
        FOG_DisplayAssert(szAssertAddTown, szAutomapCpp, 1873);
        exit(-1);
    }

    D2COMMON_DUNGEON_GameTileToClientCoords(&nCenterX, &nCenterY);
    nCenterX /= 10;
    nCenterY /= 10;

    nHalfWidth = nOriginOffsetX + nCellsX * nCellPitchX / 2;
    nPixelY = nCenterY - nCellsY * nCellPitchY / 2 - nOriginOffsetY;

    for (iRow = 0; iRow < nCellsY; ++iRow)
    {
        nPixelX = nCenterX - nHalfWidth;

        for (iCol = 0; iCol < nCellsX; ++iCol)
        {
            if (pSkipCels)
            {
                for (pSkipCel = pSkipCels; *pSkipCel != -1; ++pSkipCel)
                {
                    if (*pSkipCel == nCelNum)
                        goto next_cell;
                }
            }

            if (nPixelX < -32768 || nPixelX > 0x7FFF)
            {
                FOG_DisplayAssert(szAssertPixelX, szAutomapCpp, 1911);
                exit(-1);
            }

            if (nPixelY < -32768 || nPixelY > 0x7FFF)
            {
                FOG_DisplayAssert(szAssertPixelY, szAutomapCpp, 1912);
                exit(-1);
            }

            if (nCelNum < -32768 || nCelNum > 0x7FFF)
            {
                FOG_DisplayAssert(szAssertCelNum, szAutomapCpp, 1913);
                exit(-1);
            }

            pCell = AUTOMAP_AllocCell();
            pCell->fSaved = 0;
            pCell->nCellNo = (uint16_t)nCelNum;
            pCell->xPixel = (uint16_t)nPixelX;
            pCell->yPixel = (uint16_t)nPixelY;
            AUTOMAP_RebalanceTree(pCell, &pLayer->pExtras);

        next_cell:
            ++nCelNum;
            nPixelX += nCellPitchX;
        }

        nPixelY += nCellPitchY;
    }

    if (nPreviousLayerNo == -1)
        return;

    //  switch back to the layer that was current
    for (pLayer = gpAutomapLayers; pLayer; pLayer = pLayer->pNext)
    {
        if (pLayer->nLayerNo == nPreviousLayerNo)
            break;
    }

    if (!pLayer)
    {
        pNewLayer = (D2AutomapLayerStrc *)FOG_Alloc(28, szAutomapCpp, 1517, 0);
        memset(pNewLayer, 0, sizeof(D2AutomapLayerStrc));
        pNewLayer->nLayerNo = nPreviousLayerNo;
        pNewLayer->pNext = gpAutomapLayers;
        gpAutomapLayers = pNewLayer;
        pLayer = pNewLayer;
    }

    if (pLayer != gpAutomapCurrentLayer)
    {
        AUTOMAP_RebuildDrawLists();

        if (gpAutomapCurrentLayer)
        {
            nBlock = (int32_t)(UINT_PTR)gpAutomapCellBlocks;
            while (nBlock)
            {
                nBlockToFree = nBlock;
                nBlock = *(uint32_t *)(nBlock + 10240);
                FOG_Free((void *)(UINT_PTR)nBlockToFree, szAutomapCpp, 1310, 0);
            }
            gpAutomapCellBlocks = 0;
            gnAutomapCellsUsed = 0;
            gpAutomapCurrentLayer->pFloors = NULL;
            gpAutomapCurrentLayer->pWalls = NULL;
            gpAutomapCurrentLayer->pObjects = NULL;
            gpAutomapCurrentLayer->pExtras = NULL;
        }

        gpAutomapCurrentLayer = pLayer;
        AUTOMAP_LoadCelData();
    }
}


//  The explored map on disk: two kinds of file, one format. <player>.map is an index of 24 bytes:
//
//      int nVersion;        must be 12, or the whole index is rebuilt
//      int nNextSlot;       round robin, 0..3
//      int dwGameIds[4];    which game each slot holds
//
//  and <player>.ma0 .. <player>.ma3 are the data files. AUTOMAP_SaveExploredMap decides which slot the current game belongs to and returns a
//  handle to its data file; both the writer and the reader go through it. Four games are remembered and the fifth evicts the oldest: a game id
//  already in the table reuses its slot and the index is not written back; an absent id takes nNextSlot, advances it and deletes that slot's data
//  file first. A header that is short or of the wrong version deletes all four.
//  A data file is
//
//      int aLayerChainHead[100];    file offset of each layer's first record
//      ... records ...
//
//  each record being
//
//      int nNextRecord;             offset of the next record for this layer
//      int nLayerNo;
//      int nExtraCelSet;            indexes gnAutomapIconCelCounts
//      int dwGameField;             checked against gpClientGameBlock[1]
//      int dwFloorSize, dwWallsSize, dwUnitsSize, dwExtraSize;
//      ... the four blocks, in that order ...
//
//  and a block being a flat run of six-byte {cel, x, y} triples: the 20-byte in-memory cell persists as nCellNo, xPixel and yPixel, and its tree
//  links and balance are rebuilt by AUTOMAP_RebalanceTree on the way back in. A cell read back from the file gets fSaved = 1 and a cell put on the
//  map by exploration has fSaved = 0; the measure and the write both skip fSaved != 0, so a record holds only what was explored since the last
//  write and the per-layer chain reassembles the whole map. Nothing is ever rewritten; the file only grows.
//  BUG: the reader and the writer both index aLayerChainHead[pLayer->nLayerNo] with no bound check, so a layer number of 100 or more reads and
//  writes past a 400-byte stack array. Levels.txt's Layer column feeds it; it is not reachable with the shipped data.
//  The struct below is the 24-byte index that AUTOMAP_SaveExploredMap reads and writes in one call.
typedef struct
{
    int32_t nVersion;
    int32_t nNextSlot;
    int32_t dwGameIds[4];
} D2AutomapIndexStrc;

static_assert(sizeof(D2AutomapIndexStrc) == 24,
              "the index header is read and written as one 24-byte block");
static_assert(offsetof(D2AutomapIndexStrc, nNextSlot) == 4,
              "nNextSlot is [esp+0Ch] where nVersion is [esp+8h]");
static_assert(offsetof(D2AutomapIndexStrc, dwGameIds) == 8,
              "the four game ids are [esp+10h], two dwords in");


//1.10f: D2Client.0x6FACC2B0
//  Opens the data file for this game; the one place the file layout is decided. Two callers, AUTOMAP_WriteExploredLayer and AUTOMAP_LoadCelData,
//  and both test the answer against -1.
//  A bad save-name parameter does not fail, it writes somewhere else: when gszRealmName is set the index goes in a subdirectory of the save path,
//  and if that file cannot be opened the code falls through to the flat save directory with the same name and carries on, so a mod whose save
//  name is wrong silently shares the base game's map files.
//  The three MAX_PATH buffers are zeroed a byte at a time and then in bulk. FOG_CreateFileA(szFile, 0xC0000000, 0, 0, 4, 128, 0) is read/write,
//  OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL.
HANDLE __stdcall AUTOMAP_SaveExploredMap()
{
    void *hIndexFile;
    int32_t nSlot;
    int32_t nSlotUsed;
    HANDLE hMapFile;
    DWORD nBytesDone;
    D2AutomapIndexStrc tIndex;
    CHAR szFile[260];
    char szSaveDir[260];
    char szPath[260];

    szSaveDir[0] = gszEmptyString;
    memset(&szSaveDir[1], 0, 259);
    szFile[0] = gszEmptyString;
    memset(&szFile[1], 0, 259);
    szPath[0] = gszEmptyString;
    memset(&szPath[1], 0, 259);

    FOG_GetSavePath(szSaveDir, 260);

    hIndexFile = (void *)-1;
    if (*(char *)gszRealmName)
    {
        STORM_SStrPrintf(szPath, 0x104u, "%s%s\\", szSaveDir, (const char *)gszRealmName);
        FOG_CreatePathHierarchy(szPath);
        STORM_SStrPrintf(szFile, 0x104u, "%s%s.map", szPath, gLocalPlayerName);
        hIndexFile = FOG_CreateFileA(szFile, 0xC0000000, 0, 0, 4, 128, 0);
        if (hIndexFile != (void *)-1)
            STORM_SStrCopy(szSaveDir, szPath, 0x104u);
    }

    if (hIndexFile == (void *)-1)
    {
        //  the flat fallback, reached both when there is no save name and when the sub-directory's index could not be opened
        sprintf(szFile, "%s%s.map", szSaveDir, gLocalPlayerName);
        hMapFile = FOG_CreateFileA(szFile, 0xC0000000, 0, 0, 4, 128, 0);
        hIndexFile = hMapFile;
        if (hMapFile == (void *)-1)
            return hMapFile;
    }

    tIndex.nNextSlot = 0;
    memset(tIndex.dwGameIds, 0, sizeof(tIndex.dwGameIds));
    tIndex.nVersion = 0;
    nBytesDone = 0;
    FOG_10110(hIndexFile, &tIndex, 24, &nBytesDone, 0);

    if (nBytesDone == 24 && tIndex.nVersion == 12)
    {
        for (nSlotUsed = 0; nSlotUsed < 4; ++nSlotUsed)
        {
            if (tIndex.dwGameIds[nSlotUsed] == *(uint32_t *)gpClientGameBlock)
                break;
        }

        if (nSlotUsed != 4)
        {
            //  the id is already in the table: reuse the slot and leave the index alone
            FOG_CloseFile(hIndexFile);
            sprintf(szFile, Format, szSaveDir, gLocalPlayerName, nSlotUsed);
            return FOG_CreateFileA(szFile, 0xC0000000, 0, 0, 4, 128, 0);
        }

        nSlotUsed = tIndex.nNextSlot;
        tIndex.nNextSlot = (tIndex.nNextSlot + 1) % 4;
        tIndex.dwGameIds[nSlotUsed] = *(uint32_t *)gpClientGameBlock;
        sprintf(szFile, Format, szSaveDir, gLocalPlayerName, nSlotUsed);
        FOG_DeleteFileA(szFile);
    }
    else
    {
        //  no header, a short one, or the wrong version: rebuild it and delete all four data files
        tIndex.nVersion = 12;
        tIndex.nNextSlot = 1;
        tIndex.dwGameIds[0] = *(uint32_t *)gpClientGameBlock;

        for (nSlot = 0; nSlot < 4; ++nSlot)
        {
            sprintf(szFile, Format, szSaveDir, gLocalPlayerName, nSlot);
            FOG_DeleteFileA(szFile);
        }

        nSlotUsed = 0;
    }

    FOG_10112(hIndexFile, 0, 0, 0);
    FOG_10111(hIndexFile, &tIndex, 24, &nBytesDone, 0);

    FOG_CloseFile(hIndexFile);
    sprintf(szFile, Format, szSaveDir, gLocalPlayerName, nSlotUsed);
    return FOG_CreateFileA(szFile, 0xC0000000, 0, 0, 4, 128, 0);
}


//1.10f: D2Client.0x6FACC080
//  Appends one layer record; its sole caller is AUTOMAP_RebuildDrawLists, which has just built the descriptor. The append walks the layer's chain
//  to its last record and patches that record's next-pointer to the new record's offset, which is the file size at that moment; an empty chain
//  takes the head-table entry instead, and the table is written back either way. The seven header dwords are written one call at a time (seven
//  four-byte Fog_10111 calls) although the descriptor is contiguous, as the original does. void: the value that reached the caller was
//  AUTOMAP_SaveExploredMap's handle, which this function has already closed, and the caller discards it.
void __fastcall AUTOMAP_WriteExploredLayer(D2AutomapDataStrc *pData)
{
    void *hFile;
    uint32_t dwAppendAt;
    int32_t nSeekTo;
    int32_t nLastRecord;
    DWORD nBytesDone;
    int32_t nChainOffset;
    uint32_t dwRecordOffset;
    int32_t aLayerChainHead[100];

    hFile = AUTOMAP_SaveExploredMap();
    if (hFile == (void *)-1)
        return;

    memset(aLayerChainHead, 0, sizeof(aLayerChainHead));
    nBytesDone = 0;
    FOG_10110(hFile, aLayerChainHead, 400, &nBytesDone, 0);

    if (nBytesDone != 400)
    {
        //  a fresh file: zero the table and put it there
        memset(aLayerChainHead, 0, sizeof(aLayerChainHead));
        FOG_10112(hFile, 0, 0, 0);
        FOG_10111(hFile, aLayerChainHead, 400, &nBytesDone, 0);
    }

    nChainOffset = aLayerChainHead[pData->nLayerNo];
    dwAppendAt = GetFileSize(hFile, NULL);
    nSeekTo = nChainOffset;
    dwRecordOffset = dwAppendAt;

    if (nChainOffset)
    {
        nLastRecord = 0;
        do
        {
            FOG_10112(hFile, nSeekTo, 0, 0);
            nLastRecord = nChainOffset;
            FOG_10110(hFile, &nChainOffset, 4, &nBytesDone, 0);
            nSeekTo = nChainOffset;
        }
        while (nChainOffset);

        FOG_10112(hFile, nLastRecord, 0, 0);
        FOG_10111(hFile, &dwRecordOffset, 4, &nBytesDone, 0);
    }
    else
    {
        aLayerChainHead[pData->nLayerNo] = dwAppendAt;
    }

    FOG_10112(hFile, 0, 0, 0);
    FOG_10111(hFile, aLayerChainHead, 400, &nBytesDone, 0);

    FOG_10112(hFile, dwRecordOffset, 0, 0);
    dwRecordOffset = 0;
    FOG_10111(hFile, &dwRecordOffset, 4, &nBytesDone, 0);

    FOG_10111(hFile, pData, 4, &nBytesDone, 0);
    FOG_10111(hFile, &pData->fSaved, 4, &nBytesDone, 0);
    FOG_10111(hFile, &pData->dwGameField, 4, &nBytesDone, 0);
    FOG_10111(hFile, &pData->dwFloorSize, 4, &nBytesDone, 0);
    FOG_10111(hFile, &pData->dwWallsSize, 4, &nBytesDone, 0);
    FOG_10111(hFile, &pData->dwUnitsSize, 4, &nBytesDone, 0);
    FOG_10111(hFile, &pData->dwExtraSize, 4, &nBytesDone, 0);

    if (pData->dwFloorSize)
        FOG_10111(hFile, pData->pFloors, pData->dwFloorSize, &nBytesDone, 0);
    if (pData->dwWallsSize)
        FOG_10111(hFile, pData->pWalls, pData->dwWallsSize, &nBytesDone, 0);
    if (pData->dwUnitsSize)
        FOG_10111(hFile, pData->pUnits, pData->dwUnitsSize, &nBytesDone, 0);
    if (pData->dwExtraSize)
        FOG_10111(hFile, pData->pExtras, pData->dwExtraSize, &nBytesDone, 0);

    FOG_CloseFile(hFile);
}


//1.10f: D2Client.0x6FACBCD0
//  Builds the four draw buffers and writes them out; always paired with AUTOMAP_LoadCelData ("work out what to draw" and "load what it is drawn
//  with"). Six callers. It measures each of the current layer's four trees, allocates a buffer per non-empty one, serialises the tree into it,
//  hands the whole descriptor to AUTOMAP_WriteExploredLayer and frees all four; the descriptor never outlives the call. The buffers are freed
//  before it returns, so what survives the call is the file.
//  gpAutomapDrawWriteCursor is the walk's only channel: each block sets it to the freshly allocated buffer and AUTOMAP_WriteCellTree advances it
//  as it writes; there is no cursor parameter.
//  The original inlines AUTOMAP_MeasureCellTree and AUTOMAP_WriteCellTree here, each one level deep, four measures and four writes; ours makes the
//  eight calls.
void __cdecl AUTOMAP_RebuildDrawLists()
{
    D2AutomapDataStrc tAutoData;

    if (!gpAutomapCurrentLayer)
        return;

    memset(&tAutoData, 0, sizeof(tAutoData));
    tAutoData.nLayerNo = gpAutomapCurrentLayer->nLayerNo;
    tAutoData.fSaved = gpAutomapCurrentLayer->fSaved;
    tAutoData.dwGameField = *((uint32_t *)gpClientGameBlock + 1);

    AUTOMAP_MeasureCellTree(gpAutomapCurrentLayer->pFloors, (int32_t *)&tAutoData.dwFloorSize);
    AUTOMAP_MeasureCellTree(gpAutomapCurrentLayer->pWalls, (int32_t *)&tAutoData.dwWallsSize);
    AUTOMAP_MeasureCellTree(gpAutomapCurrentLayer->pObjects, (int32_t *)&tAutoData.dwUnitsSize);
    AUTOMAP_MeasureCellTree(gpAutomapCurrentLayer->pExtras, (int32_t *)&tAutoData.dwExtraSize);

    if (tAutoData.dwFloorSize)
    {
        gpAutomapDrawWriteCursor = (uint8_t *)FOG_Alloc(tAutoData.dwFloorSize, szAutomapCpp, 1258, 0);
        tAutoData.pFloors = (void *)gpAutomapDrawWriteCursor;
        AUTOMAP_WriteCellTree(gpAutomapCurrentLayer->pFloors);
    }

    if (tAutoData.dwWallsSize)
    {
        gpAutomapDrawWriteCursor = (uint8_t *)FOG_Alloc(tAutoData.dwWallsSize, szAutomapCpp, 1265, 0);
        tAutoData.pWalls = (void *)gpAutomapDrawWriteCursor;
        AUTOMAP_WriteCellTree(gpAutomapCurrentLayer->pWalls);
    }

    if (tAutoData.dwUnitsSize)
    {
        gpAutomapDrawWriteCursor = (uint8_t *)FOG_Alloc(tAutoData.dwUnitsSize, szAutomapCpp, 1272, 0);
        tAutoData.pUnits = (void *)gpAutomapDrawWriteCursor;
        AUTOMAP_WriteCellTree(gpAutomapCurrentLayer->pObjects);
    }

    if (tAutoData.dwExtraSize)
    {
        gpAutomapDrawWriteCursor = (uint8_t *)FOG_Alloc(tAutoData.dwExtraSize, szAutomapCpp, 1279, 0);
        tAutoData.pExtras = (void *)gpAutomapDrawWriteCursor;
        AUTOMAP_WriteCellTree(gpAutomapCurrentLayer->pExtras);
    }

    AUTOMAP_WriteExploredLayer(&tAutoData);

    if (tAutoData.pFloors)
        FOG_Free(tAutoData.pFloors, szAutomapCpp, 1289, 0);
    if (tAutoData.pWalls)
        FOG_Free(tAutoData.pWalls, szAutomapCpp, 1290, 0);
    if (tAutoData.pUnits)
        FOG_Free(tAutoData.pUnits, szAutomapCpp, 1291, 0);
    if (tAutoData.pExtras)
        FOG_Free(tAutoData.pExtras, szAutomapCpp, 1292, 0);
}


//1.10f: D2Client.0x6FACC610
//  Reads the explored map back; the largest function of the unit. Five call sites, in AUTOMAP_Tick, AUTOMAP_RebuildAutomapLayer and
//  AUTOMAP_AddTown -- the same three that call AUTOMAP_RebuildDrawLists. It walks the current layer's chain of records and inserts every cell it
//  finds into the layer's four trees with fSaved = 1, so the tree holds the saved map and the exploration since. Each block is allocated, read
//  whole, walked and freed before the next.
//  Four blocks, and the four asserts say which is which: dwFloorSize, dwWallsSize, dwUnitsSize and dwExtraSize against the insert targets pFloors,
//  pWalls, pObjects and pExtras. Two things the four do not share:
//      block 2 (units) is skipped whole when the record's dwGameField does not match gpClientGameBlock[1] -- seeked past, not read. Units are
//        game-specific and floors are not
//      block 3 (extras) bounds its cel number with gnAutomapIconCelCounts[nExtraCelSet], the record's own third dword, where the other three
//        use [0]. The extras are the town drawings AUTOMAP_AddTown plots, and they come from a different cel file
//  A cel number at or above its bound stops the block's loop and truncates the chain, and so does a record whose nLayerNo is not this layer's.
//  Both do the same two-armed thing: with nPrevOffset == 0 they zero this layer's chain-head entry and write the 400-byte table back; otherwise
//  they seek to nPrevOffset and write a zero over the record's next-pointer.
//  BUG: from the second record onward nPrevOffset is the current record's offset, not the previous one's: the loop tail does nPrevOffset =
//  nNextOffset and the next iteration seeks to that same value (0x6FACCC4A, 0x6FACC692). So the outcome depends on where in the chain the bad
//  cell is: in the first record the head is zeroed and the whole chain is dropped, including the record just read; in any other record that
//  record's own next-pointer is zeroed, so the offending record stays and its successors are lost. NOT ESTABLISHED: it needs a saved map holding a
//  cel number at or above the current gnAutomapIconCelCounts, which changes when the icon cel files do (see AUTOMAP_LoadIconCels).
//  The block is walked three words at a time over dwSize >> 1 words and the loop bound is on the word count, so a block whose byte size is not a
//  multiple of six stops one triple early rather than reading past; the index advances by 3 and the test is >=.
void __stdcall AUTOMAP_LoadCelData()
{
    void *hFile;
    DWORD nBytesRead;
    int32_t nRecordOffset;
    int32_t nPrevOffset;
    int32_t nLastRecord;
    int32_t nNextOffset;
    int32_t nRecordLayerNo;
    int32_t nRecordHeader[10];
    int32_t aLayerChainHead[100];
    uint16_t *pBlock;
    uint32_t nWordCount;
    uint32_t iWord;
    int32_t nCellNo;
    D2AutomapCellStrc *pCell;
    DWORD nUnlinkDone;
    int32_t nZero;

    if (!gpAutomapCurrentLayer)
        return;

    hFile = AUTOMAP_SaveExploredMap();
    if (hFile == (void *)-1)
        return;

    memset(aLayerChainHead, 0, sizeof(aLayerChainHead));
    nBytesRead = 0;
    FOG_10110(hFile, aLayerChainHead, 400, &nBytesRead, 0);

    if (nBytesRead != 400)
    {
        FOG_CloseFile(hFile);
        return;
    }

    nLastRecord = 0;
    nPrevOffset = 0;
    nNextOffset = aLayerChainHead[gpAutomapCurrentLayer->nLayerNo];
    nRecordOffset = nNextOffset;

    if (!nNextOffset)
    {
        FOG_CloseFile(hFile);
        return;
    }

    while (1)
    {
        nRecordLayerNo = 0;
        memset(nRecordHeader, 0, sizeof(nRecordHeader));

        FOG_10112(hFile, nRecordOffset, 0, 0);
        FOG_10110(hFile, &nNextOffset, 4, &nBytesRead, 0);
        FOG_10110(hFile, &nRecordLayerNo, 4, &nBytesRead, 0);
        FOG_10110(hFile, &nRecordHeader[0], 4, &nBytesRead, 0);
        FOG_10110(hFile, &nRecordHeader[1], 4, &nBytesRead, 0);
        FOG_10110(hFile, &nRecordHeader[2], 4, &nBytesRead, 0);
        FOG_10110(hFile, &nRecordHeader[3], 4, &nBytesRead, 0);
        FOG_10110(hFile, &nRecordHeader[4], 4, &nBytesRead, 0);
        FOG_10110(hFile, &nRecordHeader[5], 4, &nBytesRead, 0);

        if (nRecordLayerNo != (int32_t)gpAutomapCurrentLayer->nLayerNo)
            break;

        //  block 0: floors
        if (nRecordHeader[2])
        {
            pBlock = (uint16_t *)FOG_Alloc(nRecordHeader[2], szAutomapCpp, 1397, 0);
            nWordCount = (uint32_t)nRecordHeader[2] >> 1;
            FOG_10110(hFile, pBlock, nRecordHeader[2], &nBytesRead, 0);

            if (nBytesRead != nRecordHeader[2])
            {
                FOG_DisplayAssert(szAssertFloorSize, szAutomapCpp, 1402);
                exit(-1);
            }

            for (iWord = 0; iWord < nWordCount; iWord += 3)
            {
                nCellNo = pBlock[iWord];
                if ((int16_t)nCellNo >= gnAutomapIconCelCounts[0])
                {
                    //  cut the chain; nPrevOffset is this record's own offset from the second record onward
                    if (nPrevOffset)
                    {
                        FOG_10112(hFile, nPrevOffset, 0, 0);
                        nZero = 0;
                        FOG_10111(hFile, &nZero, 4, &nUnlinkDone, 0);
                    }
                    else
                    {
                        aLayerChainHead[gpAutomapCurrentLayer->nLayerNo] = 0;
                        FOG_10112(hFile, 0, 0, 0);
                        FOG_10111(hFile, aLayerChainHead, 400, &nUnlinkDone, 0);
                    }
                    break;
                }

                pCell = AUTOMAP_AllocCell();
                pCell->nCellNo = (uint16_t)nCellNo;
                pCell->xPixel = pBlock[iWord + 1];
                pCell->yPixel = pBlock[iWord + 2];
                pCell->fSaved = 1;
                AUTOMAP_RebalanceTree(pCell, &gpAutomapCurrentLayer->pFloors);
            }

            FOG_Free(pBlock, szAutomapCpp,1414, 0);
        }

        //  block 1: walls
        if (nRecordHeader[3])
        {
            pBlock = (uint16_t *)FOG_Alloc(nRecordHeader[3], szAutomapCpp, 1418, 0);
            nWordCount = (uint32_t)nRecordHeader[3] >> 1;
            FOG_10110(hFile, pBlock, nRecordHeader[3], &nBytesRead, 0);

            if (nBytesRead != nRecordHeader[3])
            {
                FOG_DisplayAssert(szAssertWallsSize, szAutomapCpp, 1423);
                exit(-1);
            }

            for (iWord = 0; iWord < nWordCount; iWord += 3)
            {
                nCellNo = pBlock[iWord];
                if ((int16_t)nCellNo >= gnAutomapIconCelCounts[0])
                {
                    //  cut the chain; nPrevOffset is this record's own offset from the second record onward
                    if (nPrevOffset)
                    {
                        FOG_10112(hFile, nPrevOffset, 0, 0);
                        nZero = 0;
                        FOG_10111(hFile, &nZero, 4, &nUnlinkDone, 0);
                    }
                    else
                    {
                        aLayerChainHead[gpAutomapCurrentLayer->nLayerNo] = 0;
                        FOG_10112(hFile, 0, 0, 0);
                        FOG_10111(hFile, aLayerChainHead, 400, &nUnlinkDone, 0);
                    }
                    break;
                }

                pCell = AUTOMAP_AllocCell();
                pCell->nCellNo = (uint16_t)nCellNo;
                pCell->xPixel = pBlock[iWord + 1];
                pCell->yPixel = pBlock[iWord + 2];
                pCell->fSaved = 1;
                AUTOMAP_RebalanceTree(pCell, &gpAutomapCurrentLayer->pWalls);
            }

            FOG_Free(pBlock, szAutomapCpp,1435, 0);
        }

        //  block 2: units, the only block gated on the game id
        if (nRecordHeader[4])
        {
            if (nRecordHeader[1] == *((uint32_t *)gpClientGameBlock + 1))
            {
                pBlock = (uint16_t *)FOG_Alloc(nRecordHeader[4], szAutomapCpp, 1442, 0);
                nWordCount = (uint32_t)nRecordHeader[4] >> 1;
                FOG_10110(hFile, pBlock, nRecordHeader[4], &nBytesRead, 0);

                if (nBytesRead != nRecordHeader[4])
                {
                    FOG_DisplayAssert(szAssertUnitsSize, szAutomapCpp, 1447);
                    exit(-1);
                }

                for (iWord = 0; iWord < nWordCount; iWord += 3)
                {
                    nCellNo = pBlock[iWord];
                    if ((int16_t)nCellNo >= gnAutomapIconCelCounts[0])
                    {
                        //  cut the chain; nPrevOffset is this record's own offset from the second record onward
                        if (nPrevOffset)
                        {
                            FOG_10112(hFile, nPrevOffset, 0, 0);
                            nZero = 0;
                            FOG_10111(hFile, &nZero, 4, &nUnlinkDone, 0);
                        }
                        else
                        {
                            aLayerChainHead[gpAutomapCurrentLayer->nLayerNo] = 0;
                            FOG_10112(hFile, 0, 0, 0);
                            FOG_10111(hFile, aLayerChainHead, 400, &nUnlinkDone, 0);
                        }
                        break;
                    }

                    pCell = AUTOMAP_AllocCell();
                    pCell->nCellNo = (uint16_t)nCellNo;
                    pCell->xPixel = pBlock[iWord + 1];
                    pCell->yPixel = pBlock[iWord + 2];
                    pCell->fSaved = 1;
                    AUTOMAP_RebalanceTree(pCell, &gpAutomapCurrentLayer->pObjects);
                }

                FOG_Free(pBlock, szAutomapCpp,1459, 0);
            }
            else
            {
                //  a different game's units: seek past the block, do not read it
                FOG_10112(hFile, nRecordHeader[4], 0, 1);
            }
        }

        //  block 3: extras, the only block with its own cel-set index
        if (nRecordHeader[5])
        {
            pBlock = (uint16_t *)FOG_Alloc(nRecordHeader[5], szAutomapCpp, 1469, 0);
            nWordCount = (uint32_t)nRecordHeader[5] >> 1;
            FOG_10110(hFile, pBlock, nRecordHeader[5], &nBytesRead, 0);

            if (nBytesRead != nRecordHeader[5])
            {
                FOG_DisplayAssert(szAssertExtraSize, szAutomapCpp, 1474);
                exit(-1);
            }

            for (iWord = 0; iWord < nWordCount; iWord += 3)
            {
                nCellNo = pBlock[iWord];
                if ((int16_t)nCellNo >= gnAutomapIconCelCounts[nRecordHeader[0]])
                {
                    //  cut the chain; nPrevOffset is this record's own offset from the second record onward
                    if (nPrevOffset)
                    {
                        FOG_10112(hFile, nPrevOffset, 0, 0);
                        nZero = 0;
                        FOG_10111(hFile, &nZero, 4, &nUnlinkDone, 0);
                    }
                    else
                    {
                        aLayerChainHead[gpAutomapCurrentLayer->nLayerNo] = 0;
                        FOG_10112(hFile, 0, 0, 0);
                        FOG_10111(hFile, aLayerChainHead, 400, &nUnlinkDone, 0);
                    }
                    break;
                }

                pCell = AUTOMAP_AllocCell();
                pCell->nCellNo = (uint16_t)nCellNo;
                pCell->xPixel = pBlock[iWord + 1];
                pCell->yPixel = pBlock[iWord + 2];
                pCell->fSaved = 1;
                AUTOMAP_RebalanceTree(pCell, &gpAutomapCurrentLayer->pExtras);
            }

            FOG_Free(pBlock, szAutomapCpp,1486, 0);
        }

        nRecordOffset = nNextOffset;
        nPrevOffset = nNextOffset;

        if (!nNextOffset)
        {
            FOG_CloseFile(hFile);
            return;
        }

        nLastRecord = nPrevOffset;
    }

    //  the layer number did not match: cut the chain here, as in the blocks above
    if (nLastRecord)
    {
        FOG_10112(hFile, nLastRecord, 0, 0);
        nZero = 0;
        FOG_10111(hFile, &nZero, 4, &nUnlinkDone, 0);
    }
    else
    {
        aLayerChainHead[gpAutomapCurrentLayer->nLayerNo] = 0;
        FOG_10112(hFile, 0, 0, 0);
        FOG_10111(hFile, aLayerChainHead, 400, &nUnlinkDone, 0);
    }
    FOG_CloseFile(hFile);
}


//  The corner labels and the party marker: seven functions, six of them one shape. Each label right-aligns at gnViewportRight - nTextWidth - 16,
//  draws on the current gnAutomapTextY in colour 4 and advances gnAutomapTextY by 16, the automap's text line height. The stack is built by the
//  drawing rather than laid out in advance, so a label that draws nothing and does not advance moves everything below it up. Some do not advance
//  when they draw nothing (the level name with no player room, the game line with no game name and no server address, the difficulty on Normal);
//  the version label advances even when it is skipped.
//  AUTOMAP_Draw sets gnAutomapTextY = 0x18 and calls GameInfo, LevelName, VersionLabel and DifficultyLabel in that order, then HostIP for a
//  hosted game and ExpansionLabel for an expansion game.
//  Every one of them opens with the Unicode array constructor loop; struct Unicode carries the constructor D2Lang exports, so `Unicode wszText[100];`
//  emits that loop itself, and the declaration sits where the original's loop sat, because three of these construct only after an early return
//  the original never pays for. The construction count fixes each buffer's size: a loop that starts at element 0 runs once per element, so the
//  array is the count; one that starts at &buf[1] leaves element 0 to a store beside it, so the array is the count plus one. Where the count and a
//  later length argument disagree (96 declared against 100 converted in AUTOMAP_DrawVersionLabel) the frame says the buffer is the larger.
//  The one assert text these seven push names the buffer bound AUTOMAP_LOCAL_BUFFER_MAX, which is 300 (cmp eax, 12Ch at 0x6FACF2F6).
static const char szAssertAutomapDifLen[] = "Unicode::strlen(StrGetStringByIndex(strautomapdif)) + Unicode::strlen(StrGetStringByIndex(wStringId)) + 3 < AUTOMAP_LOCAL_BUFFER_MAX";


//1.10f: D2Client.0x6FACF0D0
//  The level name. With no player room it draws nothing and does not advance the row: no line and no gap.
void __cdecl AUTOMAP_DrawLevelName()
{
    D2ActiveRoomStrc *pRoom;
    const Unicode *pwszLevelName;
    int32_t nY;
    int32_t nTextWidth;

    pRoom = CUNIT_GetPlayerRoom();
    if (!pRoom)
        return;

    pwszLevelName = (const Unicode *)UI_LevelsGetLevelName(D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom));

    nY = gnAutomapTextY;
    nTextWidth = D2WIN_10121_GetTextWidth(pwszLevelName);
    D2WIN_10117_DrawText(pwszLevelName, gnViewportRight - nTextWidth - 16, nY, 4, 0);
    gnAutomapTextY += 16;
}


//1.10f: D2Client.0x6FACF120
//  The version: "v %d.%02d" of n / 100 + 1 and n % 100, so the internal 10 prints as v 1.10 (the major part is stored one less than it reads).
//  Both numbers come from calls through the Battle.net interface GAME_GetComInterface returns, +92 (0x5C) for the version and +176 (0xB0) for the
//  build, and the build is appended only when gpLaunchConfig->bBuild is set. The whole label is skipped when there is no interface, and the row
//  still advances, which is the opposite of the level name.
void __cdecl AUTOMAP_DrawVersionLabel()
{
    struct BnClientInterface *pComInterface;
    int32_t nVersion;
    int32_t nBuild;
    int32_t nTextWidth;
    char szBuffer[100];

    Unicode wszText[100];

    if (GAME_GetComInterface())
    {
        pComInterface = GAME_GetComInterface();
        nVersion = (*(int32_t(__cdecl **)(void))((char *)pComInterface + 92))();

        if (gpLaunchConfig->bBuild)
        {
            pComInterface = GAME_GetComInterface();
            nBuild = (*(char(__cdecl **)(void))((char *)pComInterface + 176))();
            sprintf(szBuffer, "v %d.%02d %d", nVersion / 100 + 1, nVersion % 100, nBuild);
        }
        else
        {
            sprintf(szBuffer, "v %d.%02d", nVersion / 100 + 1, nVersion % 100);
        }

        Unicode::toUnicode(wszText, szBuffer, 100);
        nTextWidth = D2WIN_10121_GetTextWidth(wszText);
        D2WIN_10117_DrawText(wszText, gnViewportRight - nTextWidth - 16, gnAutomapTextY, 4, 0);
    }

    gnAutomapTextY += 16;
}


//1.10f: D2Client.0x6FACF210
//  The host IP: a 300-element Unicode buffer, constructed element by element, holds a string that FOG_GetHostIPAddress caps at 300.
void __cdecl AUTOMAP_DrawHostIP()
{
    const char *pszHostIP;
    int32_t nY;
    int32_t nTextWidth;

    Unicode wszText[300];

    pszHostIP = FOG_GetHostIPAddress();
    Unicode::toUnicode(wszText, pszHostIP, 300);

    nY = gnAutomapTextY;
    nTextWidth = D2WIN_10121_GetTextWidth(wszText);
    D2WIN_10117_DrawText(wszText, gnViewportRight - nTextWidth - 16, nY, 4, 0);
    gnAutomapTextY += 16;
}


//1.10f: D2Client.0x6FACF060
//  The expansion label, tbl string 22730; AUTOMAP_Draw calls it last, and only for an expansion game (GAME_IsExpansionGame).
//  What 22730 says is install-dependent and is not established here.
void __cdecl AUTOMAP_DrawExpansionLabel()
{
    const Unicode *pwszSrc;
    int32_t nY;
    int32_t nTextWidth;

    Unicode wszText[50];

    pwszSrc = D2LANG_GetStringFromTblIndex(STR_IDX_22730_x_EXPANSION);
    Unicode::strcpy(wszText, pwszSrc);

    nY = gnAutomapTextY;
    nTextWidth = D2WIN_10121_GetTextWidth(wszText);
    D2WIN_10117_DrawText(wszText, gnViewportRight - nTextWidth - 16, nY, 4, 0);
    gnAutomapTextY += 16;
}


//1.10f: D2Client.0x6FACEF10
//  Game and password: two lines from tbl 4181 (automapgame) and 4182 (automappw), each the tbl text with the value concatenated. The two lines are
//  independent: with no game name the function returns early only when WINMAIN_GetConfigServerIP is also null, so a password line can be drawn
//  without a game line above it.
//  BUG: the value is copied through a 28-byte char buffer with sprintf("%s") and only then converted with a length of 25, so a game name longer
//  than 27 characters overruns the char buffer before the 25 ever applies. The names come from WINMAIN_GetConfigGameName, which the launcher
//  fills; the game's own UI, whose game-name field is shorter, cannot produce one that long.
void __cdecl AUTOMAP_DrawGameInfo()
{
    char *pszGameName;
    char *pszServerIP;
    char *pszPassword;
    const Unicode *pwszTbl;
    int32_t nY;
    int32_t nTextWidth;
    char szBuffer[28];

    Unicode wszValue[25];

    Unicode wszText[50];

    pszGameName = WINMAIN_GetConfigGameName();
    pszServerIP = WINMAIN_GetConfigServerIP();

    if (pszGameName)
    {
        sprintf(szBuffer, "%s", pszGameName);
        Unicode::toUnicode(wszValue, szBuffer, 25);
        pwszTbl = D2LANG_GetStringFromTblIndex(STR_IDX_4181_automapgame);
        Unicode::strcpy(wszText, pwszTbl);
        Unicode::strcat(wszText, wszValue);

        nY = gnAutomapTextY;
        nTextWidth = D2WIN_10121_GetTextWidth(wszText);
        D2WIN_10117_DrawText(wszText, gnViewportRight - nTextWidth - 16, nY, 4, 0);
        gnAutomapTextY += 16;
    }
    else if (!pszServerIP)
    {
        return;
    }

    pszPassword = WINMAIN_GetConfigGamePassword();
    if (!pszPassword)
        return;

    sprintf(szBuffer, "%s", pszPassword);
    Unicode::toUnicode(wszValue, szBuffer, 25);
    pwszTbl = D2LANG_GetStringFromTblIndex(STR_IDX_4182_automappw);
    Unicode::strcpy(wszText, pwszTbl);
    Unicode::strcat(wszText, wszValue);

    nY = gnAutomapTextY;
    nTextWidth = D2WIN_10121_GetTextWidth(wszText);
    D2WIN_10117_DrawText(wszText, gnViewportRight - nTextWidth - 16, nY, 4, 0);
    gnAutomapTextY += 16;
}


//1.10f: D2Client.0x6FACF290
//  The difficulty: tbl 4183 (automapdif) formatted with the difficulty's own name, 5155 Nightmare or 5154 Hell. Normal draws nothing, because the
//  switch has only two arms: GAME_GetDifficulty() - 1 is 0 for Nightmare and 1 for Hell, and anything else returns before the row advance.
//  Both strings are translated, so the formatted length is not known at compile time and the assert runs before the sprintf:
//  strlen(strautomapdif) + strlen(wStringId) + 3 < 300, where 300 is AUTOMAP_LOCAL_BUFFER_MAX and the +3 is the separator and the terminator.
//  Both tbl rows are fetched twice, once to measure and once to format, as the original does; D2LANG_GetStringFromTblIndex has no side effect.
void __cdecl AUTOMAP_DrawDifficultyLabel()
{
    int32_t nDifficulty;
    D2C_StringIndices nDifficultyStrId;
    const Unicode *pwszDifficulty;
    const Unicode *pwszFormat;
    int32_t nDifficultyLen;
    int32_t nY;
    int32_t nTextWidth;

    nDifficulty = (uint8_t)GAME_GetDifficulty() - 1;
    if (nDifficulty == 0)
    {
        nDifficultyStrId = STR_IDX_5155_Nightmare;
    }
    else if (nDifficulty == 1)
    {
        nDifficultyStrId = STR_IDX_5154_Hell;
    }
    else
    {
        return;
    }

    Unicode wszText[300];

    pwszDifficulty = D2LANG_GetStringFromTblIndex(nDifficultyStrId);
    nDifficultyLen = Unicode::strlen(pwszDifficulty);
    pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_4183_automapdif);

    if (nDifficultyLen + Unicode::strlen(pwszFormat) + 3 >= 300)
    {
        FOG_DisplayAssert(szAssertAutomapDifLen, szAutomapCpp, 2878);
        exit(-1);
    }

    pwszDifficulty = D2LANG_GetStringFromTblIndex(nDifficultyStrId);
    pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_4183_automapdif);
    Unicode::sprintf(300, wszText, pwszFormat, pwszDifficulty);

    nY = gnAutomapTextY;
    nTextWidth = D2WIN_10121_GetTextWidth(wszText);
    D2WIN_10117_DrawText(wszText, gnViewportRight - nTextWidth - 16, nY, 4, 0);
    gnAutomapTextY += 16;
}


//1.10f: D2Client.0x6FACED50
//  One party member: a twelve-segment green polyline and, when the setting is on, their name above it. Sole caller AUTOMAP_DrawUnits.
//  It refuses across acts before anything else: the roster entry's level and the player's room both go through DRLG_GetActNoFromLevelId and the
//  whole body is skipped unless they match, so a party member in another act is absent rather than drawn at a stale position.
//  The marker walks gaAutomapPartyMarker four ints at a time and steps by two, so each segment starts where the last ended: a closed polyline over
//  thirteen (x, y) pairs, twelve segments, stopping at index 24 with the last read at 25; the array is int[26]. Every coordinate is doubled and
//  offset by the marker's screen position. The colour is gAutomapSymbolColors[3], which AUTOMAP_InitSymbolColors sets from MixRGB(0, 255, 0).
//  The name is drawn in font 6, colour 2, centred on the marker's x and ten pixels above its y, and the previous font is restored afterwards; it is
//  skipped when ROSTER_GetDisplayName gives an empty string.
void __fastcall AUTOMAP_DrawPartyMember(D2RosterUnitStrc *pRosterEntry)
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    D2C_Levels nPlayerLevelId;
    D2C_Levels nEntryLevelId;
    uint8_t nEntryAct;
    int32_t nScreenX;
    int32_t nScreenY;
    uint32_t iMarkerPoint;
    char *pszName;
    enum Font nPrevFont;
    int32_t nY;
    int32_t nTextWidth;
    int32_t pX;
    int32_t pY;

    pX = 0;
    pY = 0;

    pPlayer = CUNIT_GetPlayer();
    pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
    nPlayerLevelId = (D2C_Levels)D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);
    nEntryLevelId = (D2C_Levels)ROSTER_GetEntryLevelId(pRosterEntry);

    nEntryAct = D2COMMON_DRLG_GetActNoFromLevelId(nEntryLevelId);
    if (D2COMMON_DRLG_GetActNoFromLevelId(nPlayerLevelId) != nEntryAct)
        return;

    ROSTER_GetEntryPosition(pRosterEntry, &pX, &pY);
    D2COMMON_DUNGEON_GameSubtileToClientCoords(&pX, &pY);

    nScreenX = pX / gnAutomapZoom - gnAutomapOriginX + 8;
    nScreenY = pY / gnAutomapZoom - gnAutomapOriginY - 8;
    pX = nScreenX;
    pY = nScreenY;

    if (nScreenX < gnAutomapClipMinX || nScreenX > gnAutomapClipMaxX || nScreenY < gnAutomapClipMinY || nScreenY > gnAutomapClipMaxY)
    {
        return;
    }

    for (iMarkerPoint = 0; iMarkerPoint < 24; iMarkerPoint += 2)
    {
        D2GFX_DrawLine(nScreenX + 2 * gaAutomapPartyMarker[iMarkerPoint], nScreenY + 2 * gaAutomapPartyMarker[iMarkerPoint + 1], nScreenX + 2 * gaAutomapPartyMarker[iMarkerPoint + 2], nScreenY + 2 * gaAutomapPartyMarker[iMarkerPoint + 3],
            gAutomapSymbolColors[3], 0xFF);
    }

    if (!gbAutomapPartyNames)
        return;

    pszName = ROSTER_GetDisplayName(pRosterEntry);
    if (!strlen(pszName))
        return;

    nPrevFont = D2WIN_10127_SetFont((enum Font)6);

    Unicode wszName[23];

    //  the array constructor and then a memset over the same 46 bytes; both are in the original
    memset(wszName, 0, 46);
    Unicode::toUnicode(wszName, pszName, 23);

    nY = pY - 10;
    nTextWidth = D2WIN_10121_GetTextWidth(wszName);
    D2WIN_10117_DrawText(wszName, pX - nTextWidth / 2, nY, 2, 0);

    D2WIN_10127_SetFont(nPrevFont);
}


//1.10f: D2Client.0x6FACECF0
//  Draws the twelve-segment marker. It walks the same table as AUTOMAP_DrawPartyMember, gaAutomapPartyMarker: four ints read per step and the
//  index advanced by two, so each segment starts where the last ended; every coordinate is doubled. In mini-map mode the origin is nudged by
//  (-1, +5) before drawing, an alignment tweak that is not part of the shape.
void __fastcall AUTOMAP_DrawSymbol(int32_t nX, int32_t nY, uint8_t nColor)
{
    int32_t nOriginX;
    int32_t nOriginY;
    uint32_t i;

    nOriginX = nX;
    nOriginY = nY;

    if (gnAutomapMode == 1)
    {
        nOriginX = nX - 1;
        nOriginY = nY + 5;
    }

    for (i = 0; i < 24; i += 2)
    {
        D2GFX_DrawLine(nOriginX + 2 * gaAutomapPartyMarker[i], nOriginY + 2 * gaAutomapPartyMarker[i + 1], nOriginX + 2 * gaAutomapPartyMarker[i + 2], nOriginY + 2 * gaAutomapPartyMarker[i + 3], nColor, 0xFF);
    }
}


//1.10f: D2Client.0x6FACEC40
//  Draws the label under a marker: font 6, centred on nX and ten pixels above nY, the same geometry AUTOMAP_DrawPartyMember uses for its own label.
//  The colour is clamped: if (nTextColor >= 13) nTextColor = 0, so thirteen is the count of text colours and anything past it falls back to zero.
//  void: the empty-name path leaves eax at 0 (xor eax, eax set AL up for strlen's repne scasb) and the drawn path leaves whatever
//  D2WIN_10127_SetFont returned.
void __fastcall AUTOMAP_DrawUnitName(const char *pszName, int32_t nX, int32_t nY, uint8_t nTextColor)
{
    enum Font nPrevFont;
    int32_t nTextWidth;

    if (!strlen(pszName))
        return;

    if (nTextColor >= 0xD)
        nTextColor = 0;

    nPrevFont = D2WIN_10127_SetFont((enum Font)6);

    //  23 elements: the construction loop (edi = 17h at 0x6FACEC7B) runs once per element from &wszText[0], and the rep stosd + stosw beside it
    //  clears eleven dwords and a word, 46 bytes
    Unicode wszText[23];

    memset(wszText, 0, 46);
    Unicode::toUnicode(wszText, pszName, 23);

    nTextWidth = D2WIN_10121_GetTextWidth(wszText);
    D2WIN_10117_DrawText(wszText, nX - nTextWidth / 2, nY - 10, nTextColor, 0);

    D2WIN_10127_SetFont(nPrevFont);
}


//1.10f: D2Client.0x6FACE8F0
//  Decides whether a unit belongs on the map; it draws nothing. The product is the byte written through pColor, and the BOOL says whether to draw
//  at all; AUTOMAP_DrawUnits does the drawing. The symbol table (indices into gAutomapSymbolColors):
//
//      0  you
//      1  a player not in your party
//      2  a corpse that owns an inventory -- a dead player
//      3  a party member
//      4  a minion, yours or a party member's
//      5  a party member, if gbAutomapShowParty
//      6  a monster with bit 1 of monstats flag word 1
//      8  the two special object classes below
//
//  Three object classes and six level ids, tested when the unit is an object:
//
//      class 59   symbol 8, always
//      class 60   symbol 8, except on levels 111, 112, 117, 125, 126 and 127, where it is not drawn at all
//      class 267  colour 0, written as a literal and not through gAutomapSymbolColors
//
//  In the monster branch, monstats classes 537, 538 and 539 (ancientstatue1..3) carry the flag and are excluded.
//
//  A player is only considered when dwAnimMode != 17; 17 is the dead body, and that path instead asks whether the corpse owns an inventory before
//  giving it symbol 2.
//  In the ROSTERPETS_GetAutomapClass switch, case 1 reads bit 9 of pUnit->dwFlags, stores it and tests it, and both arms of that test write the
//  same colour (gAutomapSymbolColors[4]) and return the same value. The test is in the image, so it is kept; every minion goes through it with no
//  observable effect.
BOOL __fastcall AUTOMAP_DrawUnit(D2UnitStrc *pUnit, uint8_t *pColor)
{
    D2C_UnitTypes dwUnitType;
    int32_t nClassId;
    int32_t nLevelId;
    D2MonStatsTxt *pMonStatsTxt;
    D2UnitStrc *pPlayer;
    int16_t nPartyId;
    int32_t pType;
    int32_t pMode;
    int32_t pClass;
    D2UnitGUID dwUnitId;
    BOOL bShow;

    //  the unit's type, or UNIT_TYPES_COUNT when there is none (the same three lines open the loop body of AUTOMAP_DrawUnits)
    dwUnitType = UNIT_TYPES_COUNT;
    if (pUnit)
        dwUnitType = pUnit->dwUnitType;

    if (dwUnitType == UNIT_PLAYER)
    {
        if (!pUnit || pUnit->dwAnimMode != 17)
        {
            if (pUnit == CUNIT_GetPlayer())
            {
                *pColor = gAutomapSymbolColors[0];
                return 1;
            }

            pPlayer = CUNIT_GetPlayer();
            nPartyId = ROSTER_GetPartyIdForUnit(pPlayer);
            if (nPartyId == -1 || nPartyId != ROSTER_GetPartyIdForUnit(pUnit))
                *pColor = gAutomapSymbolColors[1];
            else
                *pColor = gAutomapSymbolColors[3];
            return 1;
        }

        //  a dead body counts only when it still owns its inventory
        if (pUnit->pInventory && D2COMMON_INVENTORY_GetOwnerId(pUnit->pInventory) == (int32_t)pUnit->dwUnitId)
        {
            *pColor = gAutomapSymbolColors[2];
            return 1;
        }
        return 0;
    }

    if (dwUnitType != UNIT_MONSTER)
    {
        if (dwUnitType == UNIT_OBJECT)
        {
            nClassId = pUnit ? pUnit->dwClassId : -1;

            if (nClassId == 59)
            {
                *pColor = gAutomapSymbolColors[8];
                return 1;
            }

            if (nClassId == 60)
            {
                nLevelId = D2COMMON_DUNGEON_GetLevelIdFromRoom(D2COMMON_UNITS_GetRoom(pUnit));
                if (nLevelId != 125 && nLevelId != 126 && nLevelId != 127 && nLevelId != 111 && nLevelId != 112 && nLevelId != 117)
                {
                    *pColor = gAutomapSymbolColors[8];
                    return 1;
                }
            }
            else if (nClassId == 267)
            {
                *pColor = 0;
                return 1;
            }
        }
        return 0;
    }

    //  a monster
    if (!pUnit)
    {
        dwUnitType = UNIT_TYPES_COUNT;
    }
    else
    {
        if (pUnit->dwAnimMode == 12)
            return 0;

        //  bit 21 of dwFlags hides a monster from the map outright
        pClass = (pUnit->dwFlags >> 21) & 1;
        if (pClass)
            return 0;

        nClassId = pUnit->dwClassId;
        if (nClassId >= 0 && nClassId < sgptDataTables->nMonStatsTxtRecordCount)
        {
            pMonStatsTxt = &sgptDataTables->pMonStatsTxt[nClassId];
            if (pMonStatsTxt)
            {
                pClass = gdwBitMasks[1] & pMonStatsTxt->nMonStatsFlags[1];
                if (pClass)
                {
                    if (nClassId != 537 && nClassId != 538 && nClassId != 539)
                    {
                        *pColor = gAutomapSymbolColors[6];
                        return 1;
                    }
                    return 0;
                }
            }
        }

        dwUnitType = pUnit->dwUnitType;
    }

    pType = dwUnitType;
    pClass = pUnit ? pUnit->dwClassId : -1;
    pMode = pUnit ? (int32_t)pUnit->dwAnimMode : 0;
    D2COMMON_11013_ConvertMode(pUnit, &pType, &pClass, &pMode, szAutomapCpp, 2349);

    if (!pType && D2COMMON_STATES_CheckStateMaskOnUnit(pUnit, 37) && CUNIT_GetOwnerUnit(pUnit))
    {
        pPlayer = CUNIT_GetPlayer();
        nPartyId = ROSTER_GetPartyIdForUnit(pPlayer);
        if (nPartyId == -1 || nPartyId != ROSTER_GetPartyIdForUnit(pUnit))
            *pColor = gAutomapSymbolColors[1];
        else
            *pColor = gAutomapSymbolColors[4];
        return 1;
    }

    dwUnitId = pUnit ? pUnit->dwUnitId : (D2UnitGUID)-1;
    switch (ROSTERPETS_GetAutomapClass(dwUnitId))
    {
    case 0:
    case 3:
        return 0;

    case 1:
        //  both arms are the same: the test on bit 9 is in the image and reaches two identical destinations
        if (pUnit && (pClass = (pUnit->dwFlags >> 9) & 1) != 0)
        {
            *pColor = gAutomapSymbolColors[4];
            bShow = 1;
        }
        else
        {
            *pColor = gAutomapSymbolColors[4];
            bShow = 1;
        }
        break;

    case 2:
        if (!gbAutomapShowParty)
            return 0;
        *pColor = gAutomapSymbolColors[5];
        bShow = 1;
        break;

    default:
        return 1;
    }

    return bShow;
}


//1.10f: D2Client.0x6FACE510
//  The live overlay: everything that moves -- the units in the player's room and the rooms next to it, then the party members who are not in any
//  of those rooms. The second pass walks the roster and draws only entries CUNIT_FindUnitByGuidAndType cannot find as a live UNIT_PLAYER, so a
//  party member you can see is drawn by the first pass from their unit, and one you cannot is drawn by the second from their roster entry; they
//  never double up.
//  Three arms by unit type:
//      a player   a symbol unless they are a party member and the party display is off, then the name in colour 2 for a party member and 1 for
//                 anyone else
//      a monster  a symbol, then the monstats name in colour 4 for the monsters that carry the flag, then the minion check
//      an object  class 267 with names on draws the word "stash" from tbl 3315 instead of a symbol; everything else gets a symbol
//  Two tests in the player and monster arms repeat a flag that already holds (marked below). void: eax at its exit holds the last
//  ROSTER_PartyGetSharedPartyId result, which nobody reads.
void __cdecl AUTOMAP_DrawUnits()
{
    D2RosterUnitStrc *pFirstEntry;
    D2RosterUnitStrc *pEntry;
    D2ActiveRoomStrc **ppRoomList;
    int32_t nNumRooms;
    int32_t iRoom;
    D2UnitStrc *pUnit;
    D2UnitStrc *pOwner;
    D2C_UnitTypes dwUnitType;
    int32_t nX;
    int32_t nY;
    int32_t nTextWidth;
    int32_t nPrevFont;
    D2MonStatsTxt *pMonStatsTxt;
    const Unicode *pwszText;
    Unicode *pwszUnitName;
    int16_t nSharedPartyId;
    int16_t nPartyId;
    uint8_t nTextColor;
    uint8_t nColor[4];
    int32_t pType;
    int32_t pMode;
    int32_t pClass;

    pFirstEntry = ROSTER_PartyGetNextEntry(NULL);

    ppRoomList = NULL;
    nNumRooms = 0;
    D2COMMON_DUNGEON_GetAdjacentRoomsListFromRoom(D2COMMON_UNITS_GetRoom(CUNIT_GetPlayer()), &ppRoomList, &nNumRooms);

    for (iRoom = 0; iRoom < nNumRooms; ++iRoom)
    {
        for (pUnit = ppRoomList[iRoom]->pUnitFirst; pUnit; pUnit = pUnit->pRoomNext)
        {
            dwUnitType = pUnit->dwUnitType;
            nColor[0] = 0;

            //  a dead player is skipped unless the body still carries the PLAYERBODY state
            if (dwUnitType == UNIT_PLAYER && CUNIT_IsUnitDead(pUnit) && !D2COMMON_STATES_CheckState(pUnit, STATE_PLAYERBODY))
            {
                continue;
            }

            if (!AUTOMAP_DrawUnit(pUnit, nColor))
                continue;

            nX = D2COMMON_UNITS_GetClientCoordX(pUnit) / gnAutomapZoom - gnAutomapOriginX + 8;
            nY = D2COMMON_UNITS_GetClientCoordY(pUnit) / gnAutomapZoom - gnAutomapOriginY - 8;

            if (nX < gnAutomapClipMinX || nX > gnAutomapClipMaxX || nY < gnAutomapClipMinY || nY > gnAutomapClipMaxY)
            {
                continue;
            }

            if (dwUnitType == UNIT_PLAYER)
            {
                if (gbAutomapShowParty || nColor[0] != gAutomapSymbolColors[3])
                {
                    AUTOMAP_DrawSymbol(nX, nY, nColor[0]);
                }

                if (gbAutomapPartyNames && gbAutomapShowParty && pUnit != CUNIT_GetPlayer())
                {
                    if (nColor[0] == gAutomapSymbolColors[3])
                    {
                        //  gbAutomapShowParty is already known true here
                        if (!gbAutomapShowParty)
                            continue;
                        nTextColor = 2;
                    }
                    else
                    {
                        nTextColor = 1;
                    }

                    AUTOMAP_DrawUnitName(D2COMMON_UNITS_GetPlayerData(pUnit)->szName, nX, nY, nTextColor);
                }
            }
            else if (dwUnitType == UNIT_MONSTER)
            {
                AUTOMAP_DrawSymbol(nX, nY, nColor[0]);

                if (!gbAutomapPartyNames)
                    continue;

                pMonStatsTxt = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);
                if (pMonStatsTxt && (gdwBitMasks[1] & pMonStatsTxt->nMonStatsFlags[1]))
                {
                    nPrevFont = D2WIN_10127_SetFont((enum Font)6);
                    pwszUnitName = CUNIT_GetUnitName(pUnit);
                    nTextWidth = D2WIN_10121_GetTextWidth(pwszUnitName);
                    D2WIN_10117_DrawText(pwszUnitName, nX - nTextWidth / 2, nY - 10, 4, 0);
                    D2WIN_10127_SetFont((enum Font)nPrevFont);
                }

                //  and the same flag again, one level in
                if (!gbAutomapPartyNames || !gbAutomapShowParty)
                    continue;

                pType = pUnit->dwUnitType;
                pClass = pUnit->dwClassId;
                pMode = pUnit->dwAnimMode;
                D2COMMON_11013_ConvertMode(pUnit, &pType, &pClass, &pMode, szAutomapCpp, 2581);
                if (pType)
                    continue;

                if (!D2COMMON_STATES_CheckStateMaskOnUnit(pUnit, 37))
                    continue;

                pOwner = CUNIT_GetOwnerUnit(pUnit);
                if (!pOwner || pOwner->dwUnitType != UNIT_PLAYER)
                    continue;

                nPartyId = ROSTER_GetPartyIdForUnit(CUNIT_GetPlayer());
                if ((uint16_t)nPartyId == 0xFFFF || nPartyId != ROSTER_GetPartyIdForUnit(pUnit))
                {
                    AUTOMAP_DrawUnitName(D2COMMON_UNITS_GetPlayerData(pOwner)->szName, nX, nY, 1);
                }
            }
            else if (dwUnitType == UNIT_OBJECT)
            {
                if (gbAutomapPartyNames && pUnit->dwClassId == 267)
                {
                    nPrevFont = D2WIN_10127_SetFont((enum Font)6);
                    pwszText = D2LANG_GetStringFromTblIndex(STR_IDX_3315_stash);
                    nTextWidth = D2WIN_10121_GetTextWidth(pwszText);
                    D2WIN_10117_DrawText(pwszText, nX - nTextWidth / 2, nY - 10, nColor[0], 0);
                    D2WIN_10127_SetFont((enum Font)nPrevFont);
                }
                else
                {
                    AUTOMAP_DrawSymbol(nX, nY, nColor[0]);
                }
            }
        }
    }

    nSharedPartyId = ROSTER_PartyGetSharedPartyId(pFirstEntry);
    if ((uint16_t)nSharedPartyId == 0xFFFF)
        return;
    if (!gbAutomapShowParty)
        return;

    for (pEntry = ROSTER_PartyGetNextEntry(pFirstEntry); pEntry; pEntry = ROSTER_PartyGetNextEntry(pEntry))
    {
        if (!CUNIT_FindUnitByGuidAndType(ROSTER_GetEntryUnitId(pEntry), UNIT_PLAYER) && nSharedPartyId == (int16_t)(uint16_t)ROSTER_PartyGetSharedPartyId(pEntry))
        {
            AUTOMAP_DrawPartyMember(pEntry);
        }
    }
}


//1.10f: D2Client.0x6FACE230
//  Draws one cell tree, clipped. It is a tree walk: the recursion takes pPrev and the loop takes pNext, like AUTOMAP_MeasureCellTree and
//  AUTOMAP_WriteCellTree; the argument is called pCellList. The clip is what prunes it: yPixel is the tree's first
//  ordering key, so a cell above the clip box means its whole left subtree is above it too and the recursion is skipped, and a cell below means
//  everything after it in order is below and the walk returns. Only the x test filters individual cells.
//  gnAutomapFade picks the draw mode:
//      1  full-screen only: the draw mode is chosen per cell by distance from the middle of the viewport -- 0 under 50, 1 under 100, 2 under 150,
//         5 beyond. The distance is the octagonal approximation min + 2 * max, halved, the same shape AUTOMAP_Tick uses for the reveal radius
//      2  draw mode 1 everywhere
//      3  draw mode 1 in mini-map mode and 2 full-screen, but only in acts 1 and 3 (nAct == 0 || nAct == 2); any other act leaves it at 5
//      otherwise  5
//  In mini-map mode with fade 1 the fade is rewritten on the way past -- gnAutomapFade = gnAutomapFadeSaved ? 0 : 2 -- the same correction
//  AUTOMAP_GetFade makes, in the same branchless shape, in a function whose name says it only reads.
//  The crop rectangle is the mini-map's own 280 x 225 box when the mode is 1, placed left or right by gbAutomapLeft (on the left its top is 75
//  with the portrait strip up and -21 without; on the right it is 57), and the whole screen otherwise.
//  void: the null-list path never assigns eax at all (test esi, esi / jz goes straight to the epilogue).
void __fastcall AUTOMAP_DrawCellList(D2AutomapCellStrc *pCellList, D2ClientAutomapDrawCtxStrc *pDrawCtx)
{
    D2AutomapCellStrc *pCell;
    int32_t nUiShiftX;
    int32_t nUiShiftMinus1;
    int32_t nScreenX;
    int32_t nYPos;
    int32_t nAutomapFade;
    int32_t eDrawMode;
    int32_t nClipX;
    int32_t nClipY;
    int32_t nHalfRight;
    int32_t nHalfBottom;
    int32_t nDx;
    int32_t nDy;
    int32_t nDistance;
    int32_t nTopStrip;
    int32_t aCropRect[4];
    D2GfxDataStrc tData;

    for (pCell = pCellList; pCell; pCell = pCell->pNext)
    {
        nUiShiftX = 0;
        nUiShiftMinus1 = SCROLL_GetUiShiftScreen() - 1;
        if (nUiShiftMinus1 == 0)
            nUiShiftX = gnScreenWidth / -4;
        else if (nUiShiftMinus1 == 1)
            nUiShiftX = gnScreenWidth / 4;

        nScreenX = 10 * (int16_t)pCell->xPixel / gnAutomapZoom - gnAutomapOriginX;
        nYPos = 10 * (int16_t)pCell->yPixel / gnAutomapZoom - gnAutomapOriginY;

        if (nYPos >= pDrawCtx->nClipTop)
        {
            AUTOMAP_DrawCellList(pCell->pPrev, pDrawCtx);

            if (nYPos > pDrawCtx->nClipBottom)
                return;

            if (nScreenX >= pDrawCtx->nClipLeft && nScreenX <= pDrawCtx->nClipRight)
            {
                eDrawMode = 5;
                memset(&tData, 0, sizeof(tData));
                tData.pCellFile = pDrawCtx->hCel;
                tData.nFrame = (int16_t)pCell->nCellNo;
                tData.nDirection = 0;

                nAutomapFade = gnAutomapFade;
                if (gnAutomapMode == 1 && gnAutomapFade == 1)
                {
                    //  the same correction as in AUTOMAP_GetFade, made again here
                    nAutomapFade = gnAutomapFadeSaved != 0 ? 0 : 2;
                    gnAutomapFade = nAutomapFade;
                }

                if (nAutomapFade == 1)
                {
                    if (pDrawCtx->hCel == gpAutomapIconCels[0] && !gnAutomapMode)
                    {
                        nClipX = nScreenX + pDrawCtx->nMarginX;
                        nClipY = nYPos + pDrawCtx->nMarginY;
                        nHalfRight = gnViewportRight / 2;
                        nHalfBottom = gnViewportBottom / 2;

                        if (nClipX >= nHalfRight + nUiShiftX - 140 && nClipX <= nHalfRight + nUiShiftX + 140 && nClipY >= nHalfBottom - 150 && nClipY <= nHalfBottom + 130)
                        {
                            nDx = nUiShiftX + nHalfRight - nClipX;
                            if (nDx < 0)
                                nDx = -nDx;

                            nDy = gnViewportBottom / 2 - 10 - nClipY;
                            if (nDy < 0)
                                nDy = nClipY - (gnViewportBottom / 2 - 10);

                            if (nDx <= nDy)
                                nDistance = (nDx + 2 * nDy) / 2;
                            else
                                nDistance = (nDy + 2 * nDx) / 2;

                            if (nDistance < 150)
                            {
                                eDrawMode = 2;
                                if (nDistance < 100)
                                    eDrawMode = (nDistance >= 50);
                            }
                        }
                    }
                }
                else if (nAutomapFade == 2)
                {
                    eDrawMode = 1;
                }
                else if (nAutomapFade == 3)
                {
                    if (CUNIT_GetPlayer()->nAct == 0 || CUNIT_GetPlayer()->nAct == 2)
                    {
                        eDrawMode = (gnAutomapMode == 1) ? 1 : 2;
                    }
                }

                if (gnAutomapMode == 1)
                {
                    if (gbAutomapLeft)
                    {
                        aCropRect[0] = 0;
                        nTopStrip = HIREABLES_PanelPortraitsReserveTopStrip() ? 75 : -21;
                    }
                    else
                    {
                        aCropRect[0] = gnScreenWidth - 281;
                        nTopStrip = 57;
                    }
                    aCropRect[1] = aCropRect[0] + 279;
                    aCropRect[2] = nTopStrip;
                    aCropRect[3] = nTopStrip + 225;
                }
                else
                {
                    aCropRect[0] = 0;
                    aCropRect[1] = gnScreenWidth - 1;
                    aCropRect[2] = 0;
                    aCropRect[3] = gnScreenHeight - 1;
                }

                D2GFX_TEXTURE_CelDrawClipped(&tData, nScreenX, nYPos, aCropRect, eDrawMode);
            }
        }

        //  reached from the pruned arm too, where it cannot be true
        if (nYPos > pDrawCtx->nClipBottom)
            return;
    }
}


//1.10f: D2Client.0x6FACDCB0
//  One frame of the automap; sole caller UIPANELS_DrawInGamePanels. It gives up unless gpAct, the local player and the player's room all exist,
//  which is why the automap is simply absent during a level change rather than stale.
//  The UI shift moves the map and the side it sits on. SCROLL_GetUiShiftScreen is 1, 2 or something else, and the three arms do two things at
//  once, a scroll offset and a side:
//      1      scroll a quarter-screen left (except in mini-map mode), and if the mini-map is on the right, save that and force it left
//      2      scroll a quarter-screen right (except in mini-map mode), and if the mini-map is on the left, save that and force it right
//      other  no scroll, and put the side back: first to what was saved, then to the setting
//  So gbAutomapLeft is a live value the panel layout overrides, and gnAutomapLeftSaved and gnAutomapLeftSetting are what restore it.
//  The origin:
//      gnAutomapScrollX = ViewportOriginX / zoom - gnViewportRight / 2 - uiShift
//      gnAutomapScrollY = ViewportOriginY / zoom - gnViewportBottom / 2
//      gnAutomapOriginX = gnAutomapPanX + gnAutomapScrollX + 40
//      gnAutomapOriginY = gnAutomapPanY + gnAutomapScrollY + 15
//  -- the pan the four arrow keys move, plus a fixed 40 x 15 nudge that centres the map on the player rather than on the viewport.
//  gnAutomapZoom is assigned every frame, 20 in mini-map mode and 10 full-screen, and twice on the way through (before the first three cell lists
//  and again after the switch below) to the same value; there is no zoom setting.
//  The switch on gpAutomapCurrentLayer->fSaved picks which icon cel file the extras are drawn from and how far outside the clip box a partially
//  visible cell of that layer may start (the margins grow with the artwork):
//      fSaved  cel  margin
//           0    0  8 x 16 (4 x 8 in mini-map mode)
//           1    1  80 x 50
//           2    2  68 x 45
//           3    3  90 x 85
//  fSaved is what AUTOMAP_AddTown wrote (1 for Lut Gholein, 2 for the Pandemonium Fortress, 3 for Harrogath), so it names which drawn town map the
//  layer carries.
void __cdecl AUTOMAP_Draw()
{
    D2UnitStrc *pPlayer;
    int32_t nUiShiftX;
    int32_t nUiShiftMinus1;
    int32_t nLeftSaved;
    int32_t nScrollX;
    int32_t nScrollY;
    eD2GameTypes eGameType;
    enum Font nSavedFont;
    D2CellFileStrc *pIconCel;
    int32_t nIconRight;
    int32_t nIconBottom;
    D2ClientAutomapDrawCtxStrc tDrawCtx;

    if (!gpAct)
        return;
    if (!CUNIT_GetPlayer())
        return;

    pPlayer = CUNIT_GetPlayer();
    if (!D2COMMON_UNITS_GetRoom(pPlayer))
        return;

    AUTOMAP_UpdateClipRect();

    if (gpAutomapCurrentLayer)
    {
        nUiShiftX = 0;
        nUiShiftMinus1 = SCROLL_GetUiShiftScreen() - 1;

        if (nUiShiftMinus1 == 0)
        {
            if (gnAutomapMode != 1)
                nUiShiftX = gnScreenWidth / -4;
            if (!gbAutomapLeft)
            {
                gnAutomapLeftSaved = 0;
                gbAutomapLeft = 1;
            }
        }
        else if (nUiShiftMinus1 == 1)
        {
            if (gnAutomapMode != 1)
                nUiShiftX = gnScreenWidth / 4;
            if (gbAutomapLeft)
            {
                gnAutomapLeftSaved = gbAutomapLeft;
                gbAutomapLeft = 0;
            }
        }
        else
        {
            nLeftSaved = gbAutomapLeft;
            if (gbAutomapLeft != gnAutomapLeftSaved)
            {
                nLeftSaved = gnAutomapLeftSaved;
                gbAutomapLeft = gnAutomapLeftSaved;
            }
            if (nLeftSaved != gnAutomapLeftSetting)
                gbAutomapLeft = gnAutomapLeftSetting;
        }

        nScrollX = SCROLL_GetViewportOriginXRaw() / gnAutomapZoom - gnViewportRight / 2 - nUiShiftX;
        gnAutomapScrollX = nScrollX;
        nScrollY = SCROLL_GetViewportOriginYRaw() / gnAutomapZoom - gnViewportBottom / 2;
        gnAutomapScrollY = nScrollY;
        gnAutomapOriginX = gnAutomapPanX + nScrollX + 40;
        //  Y's value comes from its register, as X's does: the original writes the global at 0x6FACDDD0 and computes with the same ecx, never reading
        //  the global back
        gnAutomapOriginY = gnAutomapPanY + nScrollY + 15;

        if (gnAutomapMode == 1)
        {
            tDrawCtx.hCel = gpAutomapIconCels[0];
            tDrawCtx.nClipLeft = gnAutomapMiniLeft - 8;
            tDrawCtx.nClipRight = gnViewportRight / 3 + gnAutomapMiniLeft + 8;
            tDrawCtx.nClipTop = gnAutomapMiniTop - 16;
            tDrawCtx.nClipBottom = gnViewportBottom / 3 + gnAutomapMiniTop + 16;
            tDrawCtx.nMarginX = 4;
            tDrawCtx.nMarginY = 8;
            gnAutomapZoom = 20;
        }
        else
        {
            tDrawCtx.hCel = gpAutomapIconCels[0];
            tDrawCtx.nClipLeft = -16;
            tDrawCtx.nClipRight = gnViewportRight + 16;
            tDrawCtx.nClipTop = -32;
            tDrawCtx.nClipBottom = gnViewportBottom + 32;
            tDrawCtx.nMarginX = 8;
            tDrawCtx.nMarginY = 16;
            gnAutomapZoom = 10;
        }

        AUTOMAP_DrawCellList(gpAutomapCurrentLayer->pFloors, &tDrawCtx);
        AUTOMAP_DrawCellList(gpAutomapCurrentLayer->pWalls, &tDrawCtx);
        AUTOMAP_DrawCellList(gpAutomapCurrentLayer->pObjects, &tDrawCtx);

        if (gnAutomapMode == 1)
        {
            switch (gpAutomapCurrentLayer->fSaved)
            {
            case 1:
                tDrawCtx.hCel = gpAutomapIconCels[1];
                tDrawCtx.nClipLeft = gnAutomapMiniLeft - 160;
                tDrawCtx.nClipTop = gnAutomapMiniTop - 100;
                tDrawCtx.nMarginX = 80;
                tDrawCtx.nMarginY = 50;
                tDrawCtx.nClipRight = gnViewportRight / 3 + gnAutomapMiniLeft + 160;
                nIconBottom = gnViewportBottom / 3 + gnAutomapMiniTop + 100;
                break;

            case 2:
                tDrawCtx.hCel = gpAutomapIconCels[2];
                tDrawCtx.nClipLeft = gnAutomapMiniLeft - 136;
                tDrawCtx.nClipTop = gnAutomapMiniTop - 90;
                tDrawCtx.nMarginX = 68;
                tDrawCtx.nMarginY = 45;
                tDrawCtx.nClipRight = gnViewportRight / 3 + gnAutomapMiniLeft + 136;
                nIconBottom = gnViewportBottom / 3 + gnAutomapMiniTop + 90;
                break;

            case 3:
                tDrawCtx.hCel = gpAutomapIconCels[3];
                tDrawCtx.nClipLeft = gnAutomapMiniLeft - 180;
                tDrawCtx.nClipTop = gnAutomapMiniTop - 170;
                tDrawCtx.nMarginX = 90;
                tDrawCtx.nMarginY = 85;
                tDrawCtx.nClipRight = gnViewportRight / 3 + gnAutomapMiniLeft + 180;
                nIconBottom = gnViewportBottom / 3 + gnAutomapMiniTop + 170;
                break;

            default:
                tDrawCtx.hCel = gpAutomapIconCels[0];
                tDrawCtx.nClipLeft = gnAutomapMiniLeft - 8;
                tDrawCtx.nClipTop = gnAutomapMiniTop - 16;
                tDrawCtx.nMarginX = 4;
                tDrawCtx.nMarginY = 8;
                tDrawCtx.nClipRight = gnViewportRight / 3 + gnAutomapMiniLeft + 8;
                nIconBottom = gnViewportBottom / 3 + gnAutomapMiniTop + 16;
                break;
            }

            gnAutomapZoom = 20;
        }
        else
        {
            switch (gpAutomapCurrentLayer->fSaved)
            {
            case 1:
                pIconCel = gpAutomapIconCels[1];
                nIconRight = gnViewportRight + 160;
                tDrawCtx.nClipLeft = -160;
                tDrawCtx.nClipTop = -100;
                nIconBottom = gnViewportBottom + 100;
                tDrawCtx.nMarginX = 80;
                tDrawCtx.nMarginY = 50;
                break;

            case 2:
                pIconCel = gpAutomapIconCels[2];
                nIconRight = gnViewportRight + 136;
                tDrawCtx.nClipLeft = -136;
                tDrawCtx.nClipTop = -90;
                nIconBottom = gnViewportBottom + 90;
                tDrawCtx.nMarginX = 68;
                tDrawCtx.nMarginY = 45;
                break;

            case 3:
                pIconCel = gpAutomapIconCels[3];
                nIconRight = gnViewportRight + 180;
                tDrawCtx.nClipLeft = -180;
                tDrawCtx.nClipTop = -170;
                nIconBottom = gnViewportBottom + 170;
                tDrawCtx.nMarginX = 90;
                tDrawCtx.nMarginY = 85;
                break;

            default:
                pIconCel = gpAutomapIconCels[0];
                nIconRight = gnViewportRight + 16;
                tDrawCtx.nClipLeft = -16;
                tDrawCtx.nClipTop = -32;
                nIconBottom = gnViewportBottom + 32;
                tDrawCtx.nMarginX = 8;
                tDrawCtx.nMarginY = 16;
                break;
            }

            tDrawCtx.hCel = pIconCel;
            tDrawCtx.nClipRight = nIconRight;
            gnAutomapZoom = 10;
        }

        tDrawCtx.nClipBottom = nIconBottom;
        AUTOMAP_DrawCellList(gpAutomapCurrentLayer->pExtras, &tDrawCtx);
    }

    AUTOMAP_DrawUnits();

    gnAutomapTextY = 24;
    eGameType = GAME_GetGameType();
    nSavedFont = D2WIN_10127_SetFont((enum Font)1);

    AUTOMAP_DrawGameInfo();
    AUTOMAP_DrawLevelName();
    AUTOMAP_DrawVersionLabel();
    AUTOMAP_DrawDifficultyLabel();

    if (eGameType == GAMETYPE_OBNET_HOST || eGameType == GAMETYPE_LAN_HOST)
        AUTOMAP_DrawHostIP();

    if (GAME_IsExpansionGame())
        AUTOMAP_DrawExpansionLabel();

    D2WIN_10127_SetFont(nSavedFont);
}

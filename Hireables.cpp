#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Hireables.h"

//  Hireables.cpp -- the party and pet portrait strip of D2Client.dll 1.10: the class-icon and pet-icon loading, the strip drawn across
//  the top of the screen, and the mouse and key handlers of the two panels UI_HIRICONS (19) and UI_HIREICONS (35).
//  The original source file is Ui\Hireables.cpp (three functions pass it as __FILE__). Address range 0x6FADA710..0x6FADBF48.
//  UI_HIREICONS and UI_HIRICONS are two different panels, one letter apart: HIREABLES_HirIconsOnF11 belongs to panel 19 and the
//  HIREABLES_HireIcons* handlers to panel 35.

//  This file uses UI_HIREICONS (35).
static_assert(UI_HIREICONS == 35 && UI_HIRICONS == 19,
              "35 and 19 -- two panels, two message tables, two arms of "
              "UI_RegisterPanelHandlers");
static_assert(UI_MERCINV == 36, "the mercenary inventory this unit draws into");
static_assert(offsetof(D2PetTypeTxt, dwPetFlags) == 0x04
              && offsetof(D2PetTypeTxt, szBaseicon) == 0x0F
              && offsetof(D2PetTypeTxt, szIcon) == 0x2F
              && offsetof(D2PetTypeTxt, wClass) == 0xB2,
              "GetHirelingIcon: `mov dl, [esi+4]`; the five 32-byte names from "
              "`lea edx, [eax+esi+0Fh]`, szIcon at `lea ebx, [esi+2Fh]`; "
              "`lea edx, [esi+0B2h]`");
static_assert(PETTYPEFLAGINDEX_DRAWHP == 5,
              "GetHirelingIcon: `and edx, [ebx+14h]` -- gdwBitMasks[5], 0x6FADAA3C");

static_assert(offsetof(D2PetTypeTxt, nIconType) == 0x0E,
              "DrawHirelingIconHint: `mov cl, [eax+0Eh]` at 0x6FADAE89, the switch over "
              "icontype");
static_assert(STR_IDX_22690_hireiconinfo1 == 22690 && STR_IDX_22691_hireiconinfo2 == 22691,
              "DrawHirelingIconHint: `mov ecx, 58A2h` and `mov ecx, 58A3h`, 0x6FADB0B3 "
              "and 0x6FADB0C7");
static_assert(HOTKEY_MERCINV == 54,
              "DrawHirelingIconHint: `mov ecx, 36h` into CMDTBL_ControlsGetBoundKeyName, "
              "0x6FADB0E4");
static_assert(offsetof(D2PetTypeTxt, wStringId) == 0x0C && STR_IDX_5382_dummy == 5382,
              "DrawIconPetTypeClipped: `mov ax, [esi+0Ch]` and `cmp ax, 1506h`, "
              "0x6FADB40E and 0x6FADB41B");
static_assert(ITEMTYPE_HEALING_POTION == 76 && ITEMTYPE_ANTIDOTE_POTION == 80
                  && ITEMTYPE_THAWING_POTION == 81 && ITEMTYPE_ARMOR == 3
                  && ITEMTYPE_HELM == 37 && ITEMTYPE_SHIELD == 2 && ITEMTYPE_SWORD == 30
                  && ITEMTYPE_SPEAR == 33 && ITEMTYPE_POLEARM == 34 && ITEMTYPE_BOW == 27
                  && ITEMTYPE_AXE == 28,
              "CanGiveCursorItem's `push 4Ch`, 50h, 51h, 3, 25h, 2, 1Eh, 21h, 22h, 1Bh "
              "and 1Ch into ITEMS_CheckItemTypeId");
static_assert(MONSTER_ROGUEHIRE == 271 && MONSTER_ACT2HIRE == 338 && MONSTER_ACT3HIRE == 359
                  && MONSTER_ACT5HIRE1 == 560 && MONSTER_ACT5HIRE2 == 561,
              "CanGiveCursorItem's switch: `cmp eax, 167h`, `sub eax, 10Fh`, `sub eax, "
              "43h`, `sub eax, 230h`, `dec eax` -- monstats hcIdx, not the row");
//  The source path that three functions pass to their callees as __FILE__.
static const char szHireablesCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\Ui\\Hireables.cpp";

//  Registry strings: the key "Diablo II" and the value "PopupHireling".
static const char szDiabloIIKey[] = "Diablo II";
static const char szPopupHireling[] = "PopupHireling";


//1.10f: D2Client.0x6FADA710
//  Returns 1 when the party portrait strip takes the top of the screen, that is for every gnPartyPortraitMode except 2; the automap
//  layout moves down when it answers 1. Mode 0 draws the party and the pets, 1 the pets only, 2 nothing.
BOOL __cdecl HIREABLES_PanelPortraitsReserveTopStrip()
{
    return gnPartyPortraitMode != 2;
}


//1.10f: D2Client.0x6FADA720
//  Loads the class icons and opens the pet-icon cache. The icon names are one char [12][32] local initialised with seven strings.
//  The rows are in the strings' order, which is NOT D2C_CharClass's: row 2 is the Barbarian (class 4), row 4 the Necromancer (class 2),
//  row 5 the Assassin (class 6) and row 6 the Druid (class 5). gpClassIcons is filled in that order, so it is not indexed by a class id;
//  HIREABLES_PanelDrawHirelingIconHint maps a class to its icon by the inverse permutation.
//  The first loop loads rows 0..4 into gpClassIcons[0..4]. The second runs in the expansion only, starts at row 5 and loads up to seven
//  more, leaving at the first empty name.
//  The pet-icon cache is 20 * nPetTypeTxtRecordCount bytes, one D2ClientPetIconCacheStrc (five int slots) per pettype.txt row, allocated
//  with FOG_AllocPool (__FILE__, line 150) and cleared.
void __cdecl HIREABLES_PanelLoadHirelingIcons()
{
    char szIconNames[12][32] = {
        "Hireables\\AmazonIcon",
        "Hireables\\SorceressIcon",
        "Hireables\\BarbarianIcon",
        "Hireables\\PaladinIcon",
        "Hireables\\NecromancerIcon",
        "Hireables\\AssassinIcon",
        "Hireables\\DruidIcon",
    };
    int32_t nIcon;
    int32_t nExpIcon;
    uint32_t nPetIconBytes;

    memset(gpClassIcons, 0, 7 * sizeof(D2CellFileStrc *));

    for (nIcon = 0; nIcon < 5; ++nIcon)
    {
        gpClassIcons[nIcon] = UI_LoadImageFromUiDir(szIconNames[nIcon]);
    }

    if (FOG_IsExpansion())
    {
        //  BUG: the loop is bounded by the name table (5 + 7 rows), not by gpClassIcons' seven slots, so indices 7..11 would be written past it,
        //  and index 7 is gnPartyPortraitMode. UNREACHABLE: row 7 is empty in a table built from literals, so the loop leaves after two loads
        //  every time.
        for (nExpIcon = 0; nExpIcon < 7; ++nExpIcon)
        {
            if (!strlen(szIconNames[nIcon + nExpIcon]))
            {
                break;
            }
            gpClassIcons[nIcon + nExpIcon] = UI_LoadImageFromUiDir(szIconNames[nIcon + nExpIcon]);
        }
    }

    nPetIconBytes = sgptDataTables->nPetTypeTxtRecordCount * sizeof(D2ClientPetIconCacheStrc);
    gpPetIconCache = (D2ClientPetIconCacheStrc *)FOG_AllocPool(0, nPetIconBytes, szHireablesCpp, 150, 0);
    //  BUG: the allocation is cleared untested; FOG_AllocPool reports a failure and returns NULL, so a failed allocation faults here. LATENT:
    //  only a failed 400-byte allocation (the stock table's 20 rows) reaches it.
    memset(gpPetIconCache, 0, nPetIconBytes);
}


//1.10f: D2Client.0x6FADA8F0
//  Unloads both icon sets and frees the cache. The class icons are walked by a pointer from gpClassIcons up to the address of
//  gnPartyPortraitMode, which is also &gpClassIcons[7]; the compare is signed, on two addresses, and the casts keep it so. Each non-NULL
//  handle is unloaded and its slot cleared.
//  The pet cache, when there is one, is walked flat (5 * nPetTypeTxtRecordCount int slots, the count taken from the table again, not
//  remembered from the allocation), the slots are unloaded but not cleared, then the cache goes back with FOG_FreePool (__FILE__, line
//  183) and the pointer is cleared.
void __cdecl HIREABLES_PanelFreeHirelingIcons()
{
    D2CellFileStrc **pIcon;
    int32_t nIcon;
    int32_t nIconCount;
    D2CellFileStrc *pImage;

    pIcon = gpClassIcons;
    do
    {
        if (*pIcon)
        {
            ARCHIVE_CellFileFree(*pIcon);
            *pIcon = 0;
        }
        ++pIcon;
    } while ((int32_t)pIcon < (int32_t)(gpClassIcons + 7));

    if (gpPetIconCache)
    {
        nIconCount = 5 * sgptDataTables->nPetTypeTxtRecordCount;
        for (nIcon = 0; nIcon < nIconCount; ++nIcon)
        {
            pImage = ((D2CellFileStrc **)gpPetIconCache)[nIcon];
            if (pImage)
            {
                ARCHIVE_CellFileFree(pImage);
            }
        }
        FOG_FreePool(NULL, gpPetIconCache, szHireablesCpp, 183, 0);
        gpPetIconCache = NULL;
    }
}


//1.10f: D2Client.0x6FADA970
//  Computes the three life-bar colours as bytes with D2Win #10034, MixRGB, the index of the palette entry nearest an RGB triple (so an
//  index is good only until the palette changes): gnLifeBarColorHigh = MixRGB(0, 128, 0), gnLifeBarColorMid = MixRGB(192, 192, 0),
//  gnLifeBarColorLow = MixRGB(255, 0, 0).
//  With bForce zero it computes once, behind the byte latch gbLifeBarColorsInit; with bForce non-zero it computes again and leaves the
//  latch as it is.
void __fastcall HIREABLES_PanelInitLifeBarColors(int32_t bForce)
{
    if (!bForce)
    {
        if (gbLifeBarColorsInit)
        {
            return;
        }
        gbLifeBarColorsInit = 1;
    }
    gnLifeBarColorHigh = D2WIN_10034_MixRGB(0, 128, 0);
    gnLifeBarColorMid = D2WIN_10034_MixRGB(192, 192, 0);
    gnLifeBarColorLow = D2WIN_10034_MixRGB(255, 0, 0);
}


//1.10f: D2Client.0x6FADA9D0
//  Clears the life-bar latch, so the next bForce-zero computation runs again.
void __cdecl HIREABLES_PanelInvalidateLifeBarColors()
{
    gbLifeBarColorsInit = 0;
}


//1.10f: D2Client.0x6FADA9E0
//  Returns the icon of one pet type of a unit, loaded on first use. The original inlines DATATBLS_GetPetTypeTxtRecord here; a type
//  outside [0, nPetTypeTxtRecordCount) answers 0 with neither out-parameter written.
//  *pbFlag, when asked for, is the record's low flag byte AND gdwBitMasks[5]: 0x20 or 0 (pettype.txt's drawhp column). *pdwGUID is
//  ROSTERPETS_GetAssociatedUnitGUID(pUnit, nPetType, 0), stored before it is tested, so a caller gets the -1 on the no-pet path too, and
//  the function answers 0.
//  A record's five icon names are one run of 32-byte fields from +0x0F (szBaseicon, then szIcon[0..3]), and wClass[0..3] at +0xB2 pairs
//  with szIcon. Slots 1..4 are tried in order; the first whose class equals the pet's monstats id (ROSTERPETS_GetMonStatsId) and whose
//  name is not empty is taken, and if none is, slot 0, the base name. A slot whose class matches but whose name is empty is passed over.
//  The handle is cached at gpPetIconCache[nPetType].pIcon[nIconSlot] and on a miss loaded as "hireables\<name>".
D2CellFileStrc *__fastcall HIREABLES_PanelGetHirelingIcon(D2UnitStrc *pUnit, int32_t nPetType, uint32_t *pdwGUID, int32_t *pbFlag)
{
    D2PetTypeTxt *pPetType;
    uint32_t dwGUID;
    int32_t nMonStatsId;
    int32_t nIconSlot;
    int32_t nSlot;
    char szPath[256];

    if (nPetType >= 0 && nPetType < sgptDataTables->nPetTypeTxtRecordCount)
    {
        pPetType = &sgptDataTables->pPetTypeTxt[nPetType];
    }
    else
    {
        pPetType = NULL;
    }
    if (!pPetType)
    {
        return 0;
    }

    if (pbFlag)
    {
        *pbFlag = (uint8_t)pPetType->dwPetFlags & gdwBitMasks[PETTYPEFLAGINDEX_DRAWHP];
    }

    dwGUID = ROSTERPETS_GetAssociatedUnitGUID(pUnit, nPetType, 0);
    *pdwGUID = dwGUID;
    if (dwGUID == 0xFFFFFFFF)
    {
        return 0;
    }

    nMonStatsId = ROSTERPETS_GetMonStatsId(dwGUID);
    nIconSlot = 0;
    for (nSlot = 1; nSlot < 5; ++nSlot)
    {
        if (nMonStatsId == pPetType->wClass[nSlot - 1] && pPetType->szIcon[nSlot - 1][0])
        {
            nIconSlot = nSlot;
            break;
        }
    }

    //  BUG: the cache pointer is not tested; before HIREABLES_PanelLoadHirelingIcons has run, after HIREABLES_PanelFreeHirelingIcons, or
    //  after a failed allocation, the lookup reads through NULL. LATENT: the cache is read only once the unit has a pet of the type, and the
    //  pet list and the cache are filled and emptied around the same game; that no frame draws between them is not established.
    if (!gpPetIconCache[nPetType].pIcon[nIconSlot])
    {
        sprintf(szPath, "hireables\\%s", (const char *)pPetType + offsetof(D2PetTypeTxt, szBaseicon) + 32 * nIconSlot);
        gpPetIconCache[nPetType].pIcon[nIconSlot] = UI_LoadImageFromUiDir(szPath);
    }
    return gpPetIconCache[nPetType].pIcon[nIconSlot];
}


//1.10f: D2Client.0x6FADAB00
//  Draws the portrait strip across the top of the screen, left to right from x = 15, one icon every 56 pixels, each at y = 60; the labels
//  under them step between y = 72 and 82 (HIREABLES_PanelToggleHintIndent adds 10 to nY and takes it off again on alternate calls, so
//  neighbouring names do not overlap).
//      the party     every roster entry in the player's party but the player: class icon, life bar, name
//      the hireling  pet type 7: icon, life bar, the monster's name
//      the pets      every pettype.txt row, by its icontype byte (+0x0E):
//                    1  one pet -- icon, life bar, the type's label
//                    2  a counted kind -- icon, the count, the type's label, and a life bar only when there is exactly one and
//                       the row's drawhp bit is set
//                    3  HIREABLES_PanelDrawHirelingHintRow draws the row
//  The original inlines DATATBLS_GetPetTypeTxtRecord in the pet loop. The original also keeps several values in one stack slot; ours
//  gives each value a local.
//  Nothing is drawn, and gbHireIconHintShown and gbHireIconHovered are cleared, when the screen is shifted to both sides (3) or to the
//  left (2), the ESC menu (9) or the config panel (11) is open, or gnPartyPortraitMode is 2.
//  Then, in the expansion, the hireling-icons panel (UI_HIREICONS, 35) is turned on when it is off, the mode is not 2 and the player has
//  a hireling. The party is skipped when the player's party id has the low word 0xFFFF or gnPartyPortraitMode is not 0.
//  The class switch over ROSTER_EntryGetClassId is unsigned (a negative class takes the default too) and inverts the order
//  HIREABLES_PanelLoadHirelingIcons loads the icons in: the Necromancer (2) is icon 4, the Barbarian (4) icon 2, the Druid (5) icon 6 and
//  the Assassin (6) icon 5. An entry whose icon is not loaded (the expansion's two in a classic game) is passed over whole: no life bar,
//  no name, no step.
//  One flag gathers whether a party member, the hireling or any pet was drawn; when none was, UI_HIRICONS (19) is turned off.
//  gbHirelingPopupPending opens the mercenary inventory once: when UI_MERCINV is not open and UI_SetUIToggle answers non-zero, the
//  registry gets "PopupHireling" = 1 under "Diablo II" and the flag is cleared.
//  The hireling's icon is drawn in mode 7 (DRAWMODE_HIGHLIGHT) while the hint is up and in mode 1 (DRAWMODE_TRANS50) otherwise, and this
//  function also raises the hint once the cursor has rested 700 ms (an unsigned compare), the dwell HIREABLES_HireIconsOnMouseMove
//  applies.
//  The hint needs both gbHireIconHovered and gbHireIconHintShown: tbl 22691 ("Right-click to Open Inventory (%s)") formatted with the key
//  bound to the mercenary inventory, tbl 3998 (the newline) and tbl 22690 ("Drop Potion on Portrait to Heal"), in 256 characters, framed
//  at (gnHireIconHintX, gnHireIconHintY + 36).
void __cdecl HIREABLES_PanelDrawHirelingIconHint()
{
    D2UnitStrc *pPlayer;
    D2C_UIpanels eShift;
    int32_t nX;
    int32_t nY;
    int32_t nLabelStagger;
    int16_t nPartyId;
    uint32_t dwPlayerId;
    D2RosterUnitStrc *pEntry;
    uint32_t dwEntryId;
    D2CellFileStrc *pClassIcon;
    int32_t bPartyDrawn;
    int32_t bAnyDrawn;
    //  pIcon and nIcon are one register in the original: the icon, then 1 once it is drawn (0 when there is none), OR-ed into bAnyDrawn.
    D2CellFileStrc *pIcon;
    int32_t nIcon;
    int32_t nDrawMode;
    uint32_t dwHirelingGUID;
    int32_t nPetType;
    D2PetTypeTxt *pPetType;

    pPlayer = CUNIT_GetPlayer();
    nY = 72;
    nLabelStagger = 0;
    nX = 15;

    eShift = SCROLL_GetUiShiftScreen();
    if (eShift == PANEL_BOTH || eShift == PANEL_LEFT || UI_GetUiWindowState(UI_ESCMENU) || UI_GetUiWindowState(UI_CONFIG) || gnPartyPortraitMode == 2)
    {
        gbHireIconHintShown = 0;
        gbHireIconHovered = 0;
        return;
    }

    if (GAME_IsExpansionGame() && !UI_GetUiWindowState(UI_HIREICONS) && gnPartyPortraitMode != 2 && ROSTERPETS_GetAssociatedUnitGUID(pPlayer, 7, 0) != -1)
    {
        UI_SetUIToggle(UI_HIREICONS, UI_TURNON, 0);
    }

    //  ---- the party
    bPartyDrawn = 0;
    nPartyId = ROSTER_GetPartyIdForUnit(pPlayer);
    dwPlayerId = pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF;
    if (nPartyId != -1 && !gnPartyPortraitMode)
    {
        for (pEntry = ROSTER_PartyGetNextEntry(NULL); pEntry; pEntry = ROSTER_PartyGetNextEntry(pEntry))
        {
            if (nPartyId != ROSTER_PartyGetSharedPartyId(pEntry))
            {
                continue;
            }
            dwEntryId = ROSTER_GetEntryUnitId(pEntry);
            if (dwEntryId == dwPlayerId)
            {
                continue;
            }

            switch (ROSTER_EntryGetClassId(pEntry))
            {
            case PCLASS_AMAZON:
                pClassIcon = gpClassIcons[0];
                break;
            case PCLASS_SORCERESS:
                pClassIcon = gpClassIcons[1];
                break;
            case PCLASS_NECROMANCER:
                pClassIcon = gpClassIcons[4];
                break;
            case PCLASS_PALADIN:
                pClassIcon = gpClassIcons[3];
                break;
            case PCLASS_BARBARIAN:
                pClassIcon = gpClassIcons[2];
                break;
            case PCLASS_DRUID:
                pClassIcon = gpClassIcons[6];
                break;
            case PCLASS_ASSASSIN:
                pClassIcon = gpClassIcons[5];
                break;
            default:
                pClassIcon = NULL;
                break;
            }
            if (!pClassIcon)
            {
                continue;
            }

            HIREABLES_PanelDrawPartyLifeBar(dwEntryId, nX, 0);
            {
                const char *pszName = ROSTER_GetDisplayName(pEntry);
                Unicode wszName[32];
                D2GfxDataStrc tClassCel;

                Unicode::toUnicode(wszName, pszName, 32);
                HIREABLES_PanelDrawTruncatedHintLabel(wszName, nX, nY, 0);
                HIREABLES_PanelToggleHintIndent(&nY, &nLabelStagger);
                memset(&tClassCel, 0, sizeof(tClassCel));
                tClassCel.pCellFile = pClassIcon;
                D2GFX_TEXTURE_CelDraw(&tClassCel, nX, 60, 0xFFFFFFFF, 1, NULL);
            }
            bPartyDrawn = 1;
            nX += 56;
        }
    }

    //  ---- the hireling
    {
        D2GfxDataStrc tHirelingCel;

        memset(&tHirelingCel, 0, sizeof(tHirelingCel));
        pIcon = HIREABLES_PanelGetHirelingIcon(pPlayer, 7, &dwHirelingGUID, NULL);
        tHirelingCel.pCellFile = pIcon;
        nIcon = 0;
        if (pIcon)
        {
            HIREABLES_PanelDrawPartyLifeBar(dwHirelingGUID, nX, 7);
            HIREABLES_PanelDrawIconNameClipped(CUNIT_FindUnitByGuidAndType(dwHirelingGUID, UNIT_MONSTER), nX, nY);
            HIREABLES_PanelToggleHintIndent(&nY, &nLabelStagger);
            gnHireIconHintX = nX;
            gnHireIconHintY = nY;
            if (gbHireIconHovered && gnHireIconHoverTick && GetTickCount() - gnHireIconHoverTick >= 0x2BC)
            {
                gbHireIconHintShown = 1;
                nDrawMode = 7;
            }
            else if (gbHireIconHintShown)
            {
                nDrawMode = 7;
            }
            else
            {
                nDrawMode = 1;
            }
            D2GFX_TEXTURE_CelDraw(&tHirelingCel, nX, 60, 0xFFFFFFFF, nDrawMode, NULL);
            nX += 56;
            nIcon = 1;
        }
    }
    bAnyDrawn = bPartyDrawn | nIcon;

    //  ---- the pets
    for (nPetType = 0; nPetType < sgptDataTables->nPetTypeTxtRecordCount; ++nPetType)
    {
        if (nPetType >= 0 && nPetType < sgptDataTables->nPetTypeTxtRecordCount)
        {
            pPetType = &sgptDataTables->pPetTypeTxt[nPetType];
        }
        else
        {
            pPetType = NULL;
        }
        if (!pPetType)
        {
            continue;
        }

        switch (pPetType->nIconType)
        {
        case 1:
        {
            //  one pet
            D2GfxDataStrc tPetCel;
            uint32_t dwSinglePetGUID;

            memset(&tPetCel, 0, sizeof(tPetCel));
            pIcon = HIREABLES_PanelGetHirelingIcon(pPlayer, nPetType, &dwSinglePetGUID, NULL);
            tPetCel.pCellFile = pIcon;
            nIcon = 0;
            if (pIcon)
            {
                HIREABLES_PanelDrawPartyLifeBar(dwSinglePetGUID, nX, nPetType);
                HIREABLES_PanelDrawIconPetTypeClipped(nPetType, nX, nY);
                HIREABLES_PanelToggleHintIndent(&nY, &nLabelStagger);
                D2GFX_TEXTURE_CelDraw(&tPetCel, nX, 60, 0xFFFFFFFF, 1, NULL);
                nX += 56;
                nIcon = 1;
            }
            bAnyDrawn |= nIcon;
            break;
        }
        case 2:
        {
            //  a counted kind; dwCountedPetGUID is written by the icon lookup and read by nothing -- the life bar asks for the GUID again
            D2GfxDataStrc tCountedCel;
            uint32_t dwCountedPetGUID;
            int32_t bDrawHp;
            int32_t nPetCount;

            nPetCount = ROSTERPETS_CountByType(pPlayer, nPetType);
            nIcon = 0;
            if (nPetCount)
            {
                memset(&tCountedCel, 0, sizeof(tCountedCel));
                bDrawHp = 0;
                pIcon = HIREABLES_PanelGetHirelingIcon(pPlayer, nPetType, &dwCountedPetGUID, &bDrawHp);
                tCountedCel.pCellFile = pIcon;
                if (pIcon)
                {
                    if (nPetCount == 1 && bDrawHp)
                    {
                        HIREABLES_PanelDrawPartyLifeBar(ROSTERPETS_GetAssociatedUnitGUID(pPlayer, nPetType, 0), nX, nPetType);
                    }
                    HIREABLES_PanelDrawIconNumberClipped(nPetCount, nX);
                    HIREABLES_PanelDrawIconPetTypeClipped(nPetType, nX, nY);
                    HIREABLES_PanelToggleHintIndent(&nY, &nLabelStagger);
                    D2GFX_TEXTURE_CelDraw(&tCountedCel, nX, 60, 0xFFFFFFFF, 1, NULL);
                    nX += 56;
                    nIcon = 1;
                }
            }
            bAnyDrawn |= nIcon;
            break;
        }
        case 3:
            bAnyDrawn |= HIREABLES_PanelDrawHirelingHintRow(pPlayer, nPetType, &nX, &nY, &nLabelStagger);
            break;
        default:
            break;
        }
    }

    if (!bAnyDrawn)
    {
        UI_SetUIToggle(UI_HIRICONS, UI_TURNOFF, 0);
    }

    if (gbHirelingPopupPending && !UI_GetUiWindowState(UI_MERCINV) && UI_SetUIToggle(UI_MERCINV, UI_TURNON, 0))
    {
        STORM_SRegSaveValue(szDiabloIIKey, szPopupHireling, 0, 1);
        gbHirelingPopupPending = 0;
    }

    if (gbHireIconHovered && gbHireIconHintShown)
    {
        Unicode wszHint[256];
        const Unicode *pwszHealLine;
        const Unicode *pwszInvLine;
        const Unicode *pwszNewline;

        wszHint[0].ch = 0;
        pwszHealLine = D2LANG_GetStringFromTblIndex(STR_IDX_22690_hireiconinfo1);
        pwszInvLine = D2LANG_GetStringFromTblIndex(STR_IDX_22691_hireiconinfo2);
        pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
        Unicode::sprintf(256, wszHint, pwszInvLine, CMDTBL_ControlsGetBoundKeyName(HOTKEY_MERCINV, TRUE));
        Unicode::strcat(wszHint, pwszNewline);
        Unicode::strcat(wszHint, pwszHealLine);
        D2WIN_10129_DrawFramedText(wszHint, gnHireIconHintX, gnHireIconHintY + 36, 0, 0);
    }
}


//1.10f: D2Client.0x6FADB180
//  Draws one life bar above a portrait. bIsPet picks the lookup: ROSTERPETS_GetLifePercent when it is not zero, ROSTER_GetPartyLife when
//  it is zero. The callers pass 0 for a party member, 7 for the hireling and the pet type for a pet.
//  A life of exactly 0 draws nothing (the test is on the raw answer, before the clamp); anything else is clamped to 0..100 (signed) and
//  drawn 46 pixels wide from nX, rows 14..19: the filled part to nX + 46 * life / 100 (a signed divide) in the high colour, the mid one
//  below 50 and the low one below 25, then the rest in colour 0, both through D2GFX_DrawBox in mode 5 (DRAWMODE_NORMAL).
//  The original inlines HIREABLES_PanelInitLifeBarColors(0) here.
//  The original writes the colour as a byte into the stack slot of bIsPet and pushes the slot whole as D2GFX_DrawBox's dwColor, so the
//  dword D2Gfx receives is bIsPet's upper three bytes over the colour: zeros at all four call sites. Ours passes the byte; the two dwords
//  are equal.
void __fastcall HIREABLES_PanelDrawPartyLifeBar(uint32_t dwGUID, int32_t nX, int32_t bIsPet)
{
    int32_t nLife;
    int32_t nLifeClamped;
    int32_t nFillEnd;
    uint8_t nColor;

    if (bIsPet)
    {
        nLife = ROSTERPETS_GetLifePercent(dwGUID);
    }
    else
    {
        nLife = ROSTER_GetPartyLife(dwGUID);
    }
    if (!nLife)
    {
        return;
    }

    if (nLife <= 0)
    {
        nLifeClamped = 0;
    }
    else if (nLife >= 100)
    {
        nLifeClamped = 100;
    }
    else
    {
        nLifeClamped = nLife;
    }
    nFillEnd = nX + 46 * nLifeClamped / 100;

    HIREABLES_PanelInitLifeBarColors(0);

    nColor = gnLifeBarColorHigh;
    if (nLifeClamped < 50)
    {
        nColor = gnLifeBarColorMid;
    }
    if (nLifeClamped < 25)
    {
        nColor = gnLifeBarColorLow;
    }

    D2GFX_DrawBox(nX, 14, nFillEnd, 19, nColor, 5);
    D2GFX_DrawBox(nFillEnd, 14, nX + 46, 19, 0, 5);
}


//1.10f: D2Client.0x6FADB280
//  Draws a party member's name under its class icon; the caller hands it the name converted into 32 Unicode.
//  The label shape the four label drawers share: font 6, the text cut from the END OF THE BUFFER until it is 66 pixels wide or less,
//  centred on nX + 22 and kept off the left edge. The cut writes into the caller's buffer: the cursor starts at element 31 and steps back
//  before each write, so the first NUL lands on element 30 and element 31 stays as the caller left it. A step behind the text's own
//  terminator changes nothing; the text shortens once the cursor reaches it. The width is halved by a signed division by 2, and the clamp
//  tests nX + (22 - width / 2) for non-negative.
void __fastcall HIREABLES_PanelDrawTruncatedHintLabel(Unicode *wszText, int32_t nX, int32_t nY, int32_t nColor)
{
    Font nPrevFont;
    Unicode *pCut;
    int32_t nLabelX;

    nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT6);
    pCut = &wszText[31];
    while (D2WIN_10121_GetTextWidth(wszText) > 66)
    {
        --pCut;
        pCut->ch = 0;
    }
    nLabelX = nX + (22 - D2WIN_10121_GetTextWidth(wszText) / 2);
    if (nLabelX < 0)
    {
        nLabelX = 0;
    }
    D2WIN_10117_DrawText(wszText, nLabelX, nY, nColor, FALSE);
    D2WIN_10127_SetFont(nPrevFont);
}


//1.10f: D2Client.0x6FADB300
//  Draws the hireling's name under its icon. The caller passes the monster found for the hireling's GUID; a NULL unit draws nothing, and
//  is the only thing tested. The name, CUNIT_GetUnitName's, goes into 32 Unicode through Unicode::strncpy, element 31 is zeroed, and the
//  label shape above draws it in colour 0, the centre computed as nX - width / 2 + 22.
void __fastcall HIREABLES_PanelDrawIconNameClipped(D2UnitStrc *pUnit, int32_t nX, int32_t nY)
{
    Font nPrevFont;
    Unicode *pCut;
    int32_t nLabelX;

    if (!pUnit)
    {
        return;
    }

    Unicode wszName[32];
    memset(wszName, 0, sizeof(wszName));
    Unicode::strncpy(wszName, CUNIT_GetUnitName(pUnit), 32);
    wszName[31].ch = 0;
    nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT6);
    pCut = &wszName[31];
    while (D2WIN_10121_GetTextWidth(wszName) > 66)
    {
        --pCut;
        pCut->ch = 0;
    }
    nLabelX = nX - D2WIN_10121_GetTextWidth(wszName) / 2 + 22;
    if (nLabelX < 0)
    {
        nLabelX = 0;
    }
    D2WIN_10117_DrawText(wszName, nLabelX, nY, 0, FALSE);
    D2WIN_10127_SetFont(nPrevFont);
}


//1.10f: D2Client.0x6FADB3D0
//  Draws a pet type's name under its icon. The original inlines DATATBLS_GetPetTypeTxtRecord here. The row's wStringId (+0x0C) names the
//  string; 0 draws nothing, and so does 5382 (`dummy`, "an evil force" in the stock tables), so a mod that gives that index a real text
//  still never sees it here.
//  The string index goes to D2Lang #10004 as a 16-bit argument (only cx is written; ecx's upper half is not read). Then the label shape
//  above, in colour 0, the centre as nX - width / 2 + 22.
void __fastcall HIREABLES_PanelDrawIconPetTypeClipped(int32_t nPetType, int32_t nX, int32_t nY)
{
    D2PetTypeTxt *pPetType;
    Font nPrevFont;
    Unicode *pCut;
    int32_t nLabelX;

    if (nPetType >= 0 && nPetType < sgptDataTables->nPetTypeTxtRecordCount)
    {
        pPetType = &sgptDataTables->pPetTypeTxt[nPetType];
    }
    else
    {
        pPetType = NULL;
    }
    if (!pPetType)
    {
        return;
    }
    if (!pPetType->wStringId || pPetType->wStringId == STR_IDX_5382_dummy)
    {
        return;
    }

    Unicode wszText[32];
    Unicode::strcpy(wszText, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pPetType->wStringId));
    nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT6);
    pCut = &wszText[31];
    while (D2WIN_10121_GetTextWidth(wszText) > 66)
    {
        --pCut;
        pCut->ch = 0;
    }
    nLabelX = nX - D2WIN_10121_GetTextWidth(wszText) / 2 + 22;
    if (nLabelX < 0)
    {
        nLabelX = 0;
    }
    D2WIN_10117_DrawText(wszText, nLabelX, nY, 0, FALSE);
    D2WIN_10127_SetFont(nPrevFont);
}


//1.10f: D2Client.0x6FADB4D0
//  Draws a count under a counted pet's icon: sprintf "%d" into 32 chars, Unicode::toUnicode into 32 Unicode, Unicode::strncpy into a
//  second, zeroed one, element 31 zeroed, then cut to 46 pixels (the life bar's width) from the TEXT's end and drawn at (nX + 8, 55) in
//  colour 0. The conversion's buffer is constructed first and wszText second, and the C declares them in that order.
void __fastcall HIREABLES_PanelDrawIconNumberClipped(int32_t nValue, int32_t nX)
{
    Unicode wszConverted[32];
    Unicode wszText[32];
    char szNumber[32];
    Unicode *pCut;
    Font nPrevFont;

    sprintf(szNumber, "%d", nValue);
    memset(wszText, 0, sizeof(wszText));
    Unicode::toUnicode(wszConverted, szNumber, 32);
    Unicode::strncpy(wszText, wszConverted, 32);
    wszText[31].ch = 0;
    pCut = &wszText[Unicode::strlen(wszText)];
        //  BUG: the width is measured before the font is set; the cut runs in whatever font is current and D2WIN_10127_SetFont(6) comes
        //  after it, where the other three label drawers set font 6 first. LATENT: UIPANELS_DrawInGamePanels sets no font before the
        //  strip, and in the stock font tables a two-digit count passes 46 pixels only in font 42 ("20" is 50), which is set only on
        //  screens drawn in place of the game.
    while (D2WIN_10121_GetTextWidth(wszText) > 46)
    {
        --pCut;
        pCut->ch = 0;
    }
    nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT6);
    D2WIN_10117_DrawText(wszText, nX + 8, 55, 0, FALSE);
    D2WIN_10127_SetFont(nPrevFont);
}


//1.10f: D2Client.0x6FADB5C0
//  Steps a label row down or back: a zero *pnStagger becomes 10 and *pnPos grows by 10; otherwise *pnPos loses the stagger and it becomes
//  0, so the labels under neighbouring icons alternate between two rows. void: eax ends as the old stagger on one arm and the new
//  position on the other, and the callers discard it.
void __fastcall HIREABLES_PanelToggleHintIndent(int32_t *pnPos, int32_t *pnStagger)
{
    int32_t nStagger;

    nStagger = *pnStagger;
    if (nStagger)
    {
        *pnPos -= nStagger;
        *pnStagger = 0;
    }
    else
    {
        *pnStagger = 10;
        *pnPos += 10;
    }
}


//1.10f: D2Client.0x6FADB5F0
//  Draws one pet icon for a pettype.txt row of icontype 3 (the golem, totem and vine rows of the stock table): the one-pet arm of
//  HIREABLES_PanelDrawHirelingIconHint as a function of its own. The icon comes from HIREABLES_PanelGetHirelingIcon, then a life bar, the
//  type's label, the label row stepped, the icon at (*pnX, 60) in mode 1 and *pnX advanced 56. It answers 0 when there is no icon and 1
//  when it drew; the caller ORs the answer into its "anything drawn" flag. The parameter pnStagger is the caller's stagger counter.
//  The original inlines HIREABLES_PanelDrawPartyLifeBar, HIREABLES_PanelDrawIconPetTypeClipped and HIREABLES_PanelToggleHintIndent here,
//  and HIREABLES_PanelInitLifeBarColors(0) inside the life bar.
//  DIFFERENCE: the life bar's copy writes the colour byte into the stack slot that holds the pet's GUID and loads the slot whole for
//  D2GFX_DrawBox, so the dword D2Gfx gets carries the GUID's upper three bytes, which a monster's GUID can have; ours passes the byte.
//  Every render driver masks the colour to its byte, so nothing reads the difference.
BOOL __fastcall HIREABLES_PanelDrawHirelingHintRow(D2UnitStrc *pOwner, int32_t nPetType, int32_t *pnX, int32_t *pnY, int32_t *pnStagger)
{
    D2GfxDataStrc tCel;
    uint32_t dwGUID;
    D2CellFileStrc *pIcon;
    int32_t nXStart;
    int32_t nLife;
    int32_t nLifeClamped;
    int32_t nFillEnd;
    uint8_t nColor;

    memset(&tCel, 0, sizeof(tCel));
    pIcon = HIREABLES_PanelGetHirelingIcon(pOwner, nPetType, &dwGUID, NULL);
    tCel.pCellFile = pIcon;
    if (!pIcon)
    {
        return FALSE;
    }

    nXStart = *pnX;
    if (nPetType)
    {
        nLife = ROSTERPETS_GetLifePercent(dwGUID);
    }
    else
    {
        nLife = ROSTER_GetPartyLife(dwGUID);
    }
    if (nLife)
    {
        if (nLife <= 0)
        {
            nLifeClamped = 0;
        }
        else if (nLife >= 100)
        {
            nLifeClamped = 100;
        }
        else
        {
            nLifeClamped = nLife;
        }
        nFillEnd = nXStart + 46 * nLifeClamped / 100;

        HIREABLES_PanelInitLifeBarColors(0);

        nColor = gnLifeBarColorHigh;
        if (nLifeClamped < 50)
        {
            nColor = gnLifeBarColorMid;
        }
        if (nLifeClamped < 25)
        {
            nColor = gnLifeBarColorLow;
        }
        D2GFX_DrawBox(nXStart, 14, nFillEnd, 19, nColor, 5);
        D2GFX_DrawBox(nFillEnd, 14, nXStart + 46, 19, 0, 5);
    }

    HIREABLES_PanelDrawIconPetTypeClipped(nPetType, *pnX, *pnY);

    HIREABLES_PanelToggleHintIndent(pnY, pnStagger);

    D2GFX_TEXTURE_CelDraw(&tCel, *pnX, 60, 0xFFFFFFFF, 1, NULL);
    *pnX += 56;
    return TRUE;
}


//1.10f: D2Client.0x6FADB890
//  Shows or hides UI_HIRICONS. void: eax ends as the mode, a window state or UI_SetUIToggle's answer, and the one caller does not read
//  it.
//      gnPartyPortraitMode 2   the three hover globals cleared, and nothing else
//      a party member (unless the mode is 1), or any pet
//                              the hover globals cleared when there is no hireling or its monster is dead -- not when the monster is not
//                              found; UI_HIRICONS turned on when it is off
//      neither                 the hover globals cleared; UI_HIRICONS turned off when it is on
//  The pet test runs before the party test, whatever it answers.
void __cdecl HIREABLES_PanelUpdateHirelingIconsVisible()
{
    D2UnitStrc *pPlayer;
    BOOL bHasPets;
    uint32_t dwHirelingGUID;
    D2UnitStrc *pHireling;

    if (gnPartyPortraitMode == 2)
    {
        gnHireIconHoverTick = 0;
        gbHireIconHovered = 0;
        gbHireIconHintShown = 0;
        return;
    }

    pPlayer = CUNIT_GetPlayer();
    bHasPets = FALSE;
    if (ROSTERPETS_UnitHasPets(pPlayer))
    {
        bHasPets = TRUE;
    }

    if ((ROSTER_HasOtherPartyMember(pPlayer) && gnPartyPortraitMode != 1) || bHasPets)
    {
        dwHirelingGUID = ROSTERPETS_GetAssociatedUnitGUID(pPlayer, 7, 0);
        if (dwHirelingGUID == 0xFFFFFFFF || ((pHireling = CUNIT_FindUnitByGuidAndType(dwHirelingGUID, UNIT_MONSTER)) != NULL && CUNIT_IsUnitDead(pHireling)))
        {
            gnHireIconHoverTick = 0;
            gbHireIconHovered = 0;
            gbHireIconHintShown = 0;
        }
        if (!UI_GetUiWindowState(UI_HIRICONS))
        {
            UI_SetUIToggle(UI_HIRICONS, UI_TURNON, 0);
        }
    }
    else
    {
        gnHireIconHoverTick = 0;
        gbHireIconHovered = 0;
        gbHireIconHintShown = 0;
        if (UI_GetUiWindowState(UI_HIRICONS))
        {
            UI_SetUIToggle(UI_HIRICONS, UI_TURNOFF, 0);
        }
    }
}


//1.10f: D2Client.0x6FADB980
//  F11 on the hireling-icons panel: the handler table entry is class 3 (a virtual key) with key 0x7A, VK_F11; no code calls it.
//  BUG: the first arm has no return. When gnPartyPortraitMode == 2 the function reports the message handled and breaks the handler chain
//  (0x6FADB98D..0x6FADB99E) and then falls through into the rest of the body, which can set bReturn back to 0 on the very next branch, so
//  the message is reported unhandled after its chain was already broken; if the player has no such unit and the all-ones lookup succeeds,
//  the chain is broken a second time. REACHABLE: HIREABLES_CmdTblActionShowPortraits, the portraits toggle key, sets the mode to 2;
//  toggle to 2 and press F11.
//  Reproduced as written: the if below has no return either.
//  ROSTERPETS_GetAssociatedUnitGUID(pPlayer, 7, 0) returning -1 means the player has no associated unit of kind 7, and the response is to
//  ask CUNIT_FindUnitByGuidAndType(0xFFFFFFFF, UNIT_MONSTER), the unit whose GUID is all ones; only when that succeeds does the hireling
//  display state get set. Written as the original has it; whether the sentinel GUID is ever a live unit is not established.
void __stdcall HIREABLES_HirIconsOnF11(SMSGHANDLER_PARAMS *pMsg)
{
    D2UnitStrc *pPlayer;
    D2UnitStrc *pPet;

    if (gnPartyPortraitMode == 2)
    {
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        //  no return here, as in the original
    }

    pPlayer = CUNIT_GetPlayer();
    if (ROSTERPETS_GetAssociatedUnitGUID(pPlayer, 7, 0) == -1)
    {
        pPet = CUNIT_FindUnitByGuidAndType(0xFFFFFFFF, UNIT_MONSTER);
        if (pPet)
        {
            SELECT_RosterPetsSetHirelingDisplayState(pPlayer, pPet, 1);
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
            STORM_SMsgBreakHandlerChain(pMsg);
        }
        else
        {
            pMsg->bReturn = 0;
        }
    }
    else
    {
        pMsg->bReturn = 0;
    }
}


//1.10f: D2Client.0x6FADBA00
//  Left-clicking the hireling icon gives it the cursor item. Panel 35 (UI_HIREICONS).
//  The three panel-35 handlers open with the same guard and hit box: nothing happens outside the expansion (GAME_IsExpansionGame), and
//  the box is x gnHireIconHintX .. gnHireIconHintX + 46, y 24 .. 60, inclusive on all four edges, with gnPartyPortraitMode == 2 disabling
//  it outright. bReturn is set to 0 before the hit test, so every path that misses the box leaves the message unhandled without saying so
//  again.
//  MSG_SendCmdWord is called with command 0x61 (97): the original sets only the low byte of ecx and the callee reads a byte, so the upper
//  bits do not matter.
void __stdcall HIREABLES_HireIconsOnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;

    if (!GAME_IsExpansionGame())
        return;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    pMsg->bReturn = 0;

    if (gnPartyPortraitMode == 2 || nXpos < gnHireIconHintX || nXpos > gnHireIconHintX + 46 || nYpos < 24 || nYpos > 60)
    {
        gnHireIconHoverTick = 0;
    }
    else if (HIREABLES_CanGiveCursorItem(TRUE))
    {
        MSG_SendCmdWord(97, 0);
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
    }
}


//1.10f: D2Client.0x6FADBA90
//  Whether the item on the cursor can go to the hireling; answers 0 or 1. Callers: HIREABLES_HireIconsOnLButtonDown (TRUE) and
//  HIREABLES_HireIconsOnMouseMove (FALSE).
//  bPlaySound only decides whether the hireling speaks; it is tested for zero three times and never used otherwise (the hireling's unit
//  is looked up separately). The sounds, through UNITSND_PlayUnitSoundEvent on the hireling's GUID with unit type 1:
//      86   the item is not identified (IFLAG_IDENTIFIED clear) or is broken (IFLAG_BROKEN set)
//      84   a healing, antidote or thawing potion -- answered 1 at once
//      85   nothing below accepted it
//  The two flag tests pass __FILE__ and __LINE__ 955 to ITEMS_CheckItemFlag. Before them: not the expansion, no player, no cursor item or
//  no hireling answer 0 silently.
//  Armour and a helm are taken by every hireling; then the hireling's monster has to be found (CUNIT_FindUnitByGuidAndType, UNIT_MONSTER)
//  or the answer is 0 with no sound. The switch on its monstats id adds:
//      271 roguehire    a bow
//      338 act2hire     a spear or a polearm
//      359 act3hire     a shield; a sword that is not two-handed
//      560 act5hire1    an axe that is not two-handed
//      561 act5hire2    a sword, two-handed or not
//  and an accepted item must then pass ITEMS_CheckRequirements against the hireling's unit, or the answer is 0 with no sound.
BOOL __fastcall HIREABLES_CanGiveCursorItem(BOOL bPlaySound)
{
    D2UnitStrc *pPlayer;
    D2UnitStrc *pPlayerForInventory;
    D2UnitStrc *pItem;
    uint32_t dwMercGUID;
    D2UnitStrc *pMerc;
    BOOL bCanEquip;

    if (!GAME_IsExpansionGame())
    {
        return FALSE;
    }
    bCanEquip = FALSE;
    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        return FALSE;
    }
    pPlayerForInventory = CUNIT_GetPlayer();
    if (!pPlayerForInventory)
    {
        return FALSE;
    }
    pItem = D2COMMON_INVENTORY_GetCursorItem(pPlayerForInventory->pInventory);
    if (!pItem)
    {
        return FALSE;
    }
    dwMercGUID = ROSTERPETS_GetAssociatedUnitGUID(pPlayer, 7, 0);
    if (dwMercGUID == 0xFFFFFFFF)
    {
        return FALSE;
    }

    if (!D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_IDENTIFIED, 955, szHireablesCpp) || D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_BROKEN, 955, szHireablesCpp))
    {
        if (bPlaySound)
        {
            UNITSND_PlayUnitSoundEvent(dwMercGUID, 1, 86);
        }
        return FALSE;
    }

    if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_HEALING_POTION) || D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_ANTIDOTE_POTION) || D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_THAWING_POTION))
    {
        if (bPlaySound)
        {
            UNITSND_PlayUnitSoundEvent(dwMercGUID, 1, 84);
        }
        return TRUE;
    }

    if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_ARMOR) || D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_HELM))
    {
        bCanEquip = TRUE;
    }

    pMerc = CUNIT_FindUnitByGuidAndType(dwMercGUID, UNIT_MONSTER);
    if (!pMerc)
    {
        return FALSE;
    }

    switch (ROSTERPETS_GetMonStatsId(dwMercGUID))
    {
    case MONSTER_ROGUEHIRE:
        if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_BOW))
        {
            bCanEquip = TRUE;
        }
        break;
    case MONSTER_ACT2HIRE:
        if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_SPEAR) || D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_POLEARM))
        {
            bCanEquip = TRUE;
        }
        break;
    case MONSTER_ACT3HIRE:
        if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_SHIELD))
        {
            bCanEquip = TRUE;
        }
        if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_SWORD) && !D2COMMON_ITEMS_CheckWeaponIfTwoHanded(pItem))
        {
            bCanEquip = TRUE;
        }
        break;
    case MONSTER_ACT5HIRE1:
        if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_AXE) && !D2COMMON_ITEMS_CheckWeaponIfTwoHanded(pItem))
        {
            bCanEquip = TRUE;
        }
        break;
    case MONSTER_ACT5HIRE2:
        if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_SWORD))
        {
            bCanEquip = TRUE;
        }
        break;
    default:
        break;
    }

    if (!bCanEquip)
    {
        if (bPlaySound)
        {
            UNITSND_PlayUnitSoundEvent(dwMercGUID, 1, 85);
        }
        return FALSE;
    }
    if (!D2COMMON_ITEMS_CheckRequirements(pItem, pMerc, FALSE, NULL, NULL, NULL))
    {
        return FALSE;
    }
    return bCanEquip;
}

//1.10f: D2Client.0x6FADBC90
//  Hovering the hireling icon: arms the timer, then shows the hint. gnHireIconHoverTick is stamped with GetTickCount() the first time the
//  cursor is found inside the box, and the hint appears only once 700 ms (0x2BC) have passed since; leaving the box clears both the hint
//  and the timer, so the dwell restarts. Two conditions raise the hint, and they are not the same:
//      gbHireIconHovered   the player has a hireling (an associated unit of kind 7) and is not holding an item on the cursor; gates the 700 ms hint
//      the second test     HIREABLES_CanGiveCursorItem(FALSE) sets the hint immediately, with no dwell; it runs after the block above and outside it
//  So hovering an empty cursor over the mercenary waits 700 ms for the hint, and hovering an item it could take shows it at once.
void __stdcall HIREABLES_HireIconsOnMouseMove(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    D2UnitStrc *pPlayer;

    if (!GAME_IsExpansionGame())
        return;

    pMsg->bReturn = 0;
    gbHireIconHovered = 0;
    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;

    if (gnPartyPortraitMode == 2 || nXpos < gnHireIconHintX || nXpos > gnHireIconHintX + 46 || nYpos < 24 || nYpos > 60)
    {
        gbHireIconHintShown = 0;
        gnHireIconHoverTick = 0;
        return;
    }

    if (!gnHireIconHoverTick)
        gnHireIconHoverTick = GetTickCount();

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer || !D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory))
    {
        pPlayer = CUNIT_GetPlayer();
        if (ROSTERPETS_GetAssociatedUnitGUID(pPlayer, 7, 0) != -1)
        {
            gbHireIconHovered = 1;
            if (GetTickCount() - gnHireIconHoverTick >= 0x2BC)
                gbHireIconHintShown = 1;
        }
    }

    //  no dwell required
    if (HIREABLES_CanGiveCursorItem(FALSE))
        gbHireIconHintShown = 1;
}


//1.10f: D2Client.0x6FADBD70
//  The "show portraits" key (sgtKeyFunctions[43].FKeyDown): steps gnPartyPortraitMode and switches the hireling-icons panel
//  (UI_HIREICONS, 35) with it. It returns nothing; no caller reads a key function's answer.
//      from 0   multiplayer: to 1, the panel on
//               single player: to 2, the hover timer cleared, the panel off
//      from 1   multiplayer: to 2, the timer cleared, the panel off
//               single player: to 0, the panel on
//      from any other value     to 0, the panel on
//  So a multiplayer game cycles 0 -> 1 -> 2 -> 0 and a single-player one 0 -> 2 -> 0. Mode 0 draws the party and the pets, 1 the pets
//  only, 2 nothing. "Single player" is GAME_GetGameType() == 0.
void __cdecl HIREABLES_CmdTblActionShowPortraits()
{
    if (gnPartyPortraitMode == 0)
    {
        if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER)
        {
            gnPartyPortraitMode = 1;
            UI_SetUIToggle(UI_HIREICONS, UI_TURNON, 0);
        }
        else
        {
            gnPartyPortraitMode = 2;
            gnHireIconHoverTick = 0;
            UI_SetUIToggle(UI_HIREICONS, UI_TURNOFF, 0);
        }
    }
    else if (gnPartyPortraitMode == 1)
    {
        if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER)
        {
            gnPartyPortraitMode = 2;
            gnHireIconHoverTick = 0;
            UI_SetUIToggle(UI_HIREICONS, UI_TURNOFF, 0);
        }
        else
        {
            gnPartyPortraitMode = 0;
            UI_SetUIToggle(UI_HIREICONS, UI_TURNON, 0);
        }
    }
    else
    {
        gnPartyPortraitMode = 0;
        UI_SetUIToggle(UI_HIREICONS, UI_TURNON, 0);
    }
}

//1.10f: D2Client.0x6FADBE20
//  Right-clicking the hireling icon opens the mercenary inventory, but only after the same 700 ms dwell the hint uses: a right-click that
//  arrives too soon after the cursor enters the box does nothing at all, not even break the chain. The timer is shared with
//  HIREABLES_HireIconsOnMouseMove.
//  Three gates follow, in order, and each abandons the function: UI_HIREICONS (35) must be open, UI_MERCINV (36) must be closed, and the
//  player must have an associated unit of kind 7.
//  gnHireIconHoverTick = 0 is reached from the two early-outs and by falling out of the successful UI_SetUIToggle, but not from any of
//  the three gates, each of which jumps past it to the epilogue: a right-click that opens the mercenary inventory clears the dwell, and
//  one rejected by a gate leaves it running. Reproduced with a goto.
void __stdcall HIREABLES_HireIconsOnRButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    D2UnitStrc *pPlayer;

    if (!GAME_IsExpansionGame())
        return;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    pMsg->bReturn = 0;

    if (gnPartyPortraitMode == 2 || nXpos < gnHireIconHintX || nXpos > gnHireIconHintX + 46 || nYpos < 24 || nYpos > 60)
    {
        goto CLEAR_TICK;
    }

    if (!gnHireIconHoverTick)
        gnHireIconHoverTick = GetTickCount();
    if (GetTickCount() - gnHireIconHoverTick < 0x2BC)
        goto CLEAR_TICK;

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    if (!UI_GetUiWindowState(UI_HIREICONS))
        return;                              //  the tick is not cleared
    if (UI_GetUiWindowState(UI_MERCINV))
        return;                              //  nor here
    pPlayer = CUNIT_GetPlayer();
    if (ROSTERPETS_GetAssociatedUnitGUID(pPlayer, 7, 0) == -1)
        return;                              //  nor here

    UI_SetUIToggle(UI_MERCINV, UI_TURNON, 0);

CLEAR_TICK:
    gnHireIconHoverTick = 0;
}


//1.10f: D2Client.0x6FADBF00
//  Puts the portraits back and arms the hireling popup: gnPartyPortraitMode to 0 and UI_HIREICONS on; then the registry value
//  "PopupHireling" under "Diablo II" is read into a local set to 0 first, so a missing value reads as 0, and a 0 raises
//  gbHirelingPopupPending, the byte HIREABLES_PanelDrawHirelingIconHint turns into opening the mercenary inventory once and writing the
//  value 1.
void __cdecl HIREABLES_HireIconsResetAndShow()
{
    uint32_t dwPopupShown;

    gnPartyPortraitMode = 0;
    UI_SetUIToggle(UI_HIREICONS, UI_TURNON, 0);
    dwPopupShown = 0;
    STORM_SRegLoadValue(szDiabloIIKey, szPopupHireling, 0, &dwPopupShown);
    if (!dwPopupShown)
    {
        gbHirelingPopupPending = 1;
    }
}

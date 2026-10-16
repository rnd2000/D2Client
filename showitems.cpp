#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "CoreArchive.h"
#include "showitems.h"

//  showitems.cpp -- the ground-item name pass of D2Client.dll 1.10, drawn while ALT is held: the whole of the original's UI\showitems.cpp (named by
//  its own assert strings), two functions over 0x6FB09D80..0x6FB0A422: SHOWITEMS_FitLabelRect (0x6FB09D80) and SHOWITEMS_DrawGroundItemLabels
//  (0x6FB09F60). FitLabelRect's only caller is DrawGroundItemLabels; DrawGroundItemLabels' only caller is UIPANELS_DrawInGamePanels.

//  The bound the assert string names and the loop tests (cmp esi, 20h): the record count of gShowItemLabels.
#define MAX_SHOW_ITEMS 32

//  The original's strings, with the address of each in the DLL.
static const char szShowItemsCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UI\\showitems.cpp";  // 0x6FB8CCB0
static const char szAssertNumShowItems[] = "sgnNumShowItems < MAX_SHOW_ITEMS";  // 0x6FB8CC8C
static_assert(sizeof(szShowItemsCpp) == 60 + 1
                  && sizeof(szAssertNumShowItems) == 32 + 1,
              "the two strings at 0x6FB8CCB0 and 0x6FB8CC8C, 60 and 32 characters");

//  The offsets this file leans on: the room's pUnitFirst, the unit's dwFlagEx and pRoomNext, the label record's fields, and the items record's
//  nQuest and dwCode (the quest override).
static_assert(offsetof(D2ActiveRoomStrc, pUnitFirst) == 0x2C,
              "pUnitFirst is read at +0x2C by 0x6FB09FEF");
static_assert(offsetof(D2UnitStrc, dwFlagEx) == 0xC8
                  && offsetof(D2UnitStrc, pRoomNext) == 0xE8,
              "dwFlagEx at +0xC8 and pRoomNext at +0xE8, 0x6FB0A00C/0x6FB0A399");
static_assert(offsetof(D2ClientShowItemLabelStrc, nTop) == 0x4
                  && offsetof(D2ClientShowItemLabelStrc, nRight) == 0x8
                  && offsetof(D2ClientShowItemLabelStrc, nBottom) == 0xC
                  && offsetof(D2ClientShowItemLabelStrc, pItem) == 0x10
                  && offsetof(D2ClientShowItemLabelStrc, wszText) == 0x14
                  && offsetof(D2ClientShowItemLabelStrc, dwBgColor) == 0x114
                  && offsetof(D2ClientShowItemLabelStrc, eDrawMode) == 0x118
                  && offsetof(D2ClientShowItemLabelStrc, nTextColor) == 0x11C,
              "the label record's fields, as 0x6FB0A039..0x6FB0A20D index them");
static_assert(offsetof(D2ItemsTxt, dwCode) == 0x80
                  && offsetof(D2ItemsTxt, nQuest) == 0x12A,
              "the items record's dwCode and nQuest, 0x6FB0A35B/0x6FB0A365");

//  The enums whose values the original carries as immediates.
static_assert(UNIT_ITEM == 4, "dwUnitType == 4 at 0x6FB0A022");
static_assert(PANEL_RIGHT == 1 && PANEL_LEFT == 2,
              "the two UiShift arms at 0x6FB0A0F8/0x6FB0A10A and 0x6FB0A08C/0x6FB0A09C");
static_assert(DRAWMODE_TRANS50 == 1 && DRAWMODE_NORMAL == 5,
              "the record's eDrawMode, stored 1 and 5 at 0x6FB0A2BF and 0x6FB0A238");
static_assert(ITEMQUAL_MAGIC == 4 && ITEMQUAL_SET == 5 && ITEMQUAL_RARE == 6
                  && ITEMQUAL_UNIQUE == 7 && ITEMQUAL_CRAFT == 8
                  && ITEMQUAL_TEMPERED == 9,
              "the six cases of the quality switch, biased by -4 at 0x6FB0A2CE");
static_assert(IFLAG_SOCKETED == 0x800 && IFLAG_ETHEREAL == 0x400000,
              "the two item flags tested at 0x6FB0A311 and 0x6FB0A33E");

//1.10f: D2Client.0x6FB09D80
//  Places one ground-item label clear of the ones already placed. Seven arguments, the first two in ecx and edx. Returns 1 when the rectangle is
//  free and 0 after nudging it, so the caller loops (at most twelve times). First the rectangle is clamped into the play area: top to 6, right to
//  nMaxRight - 1, bottom to nMaxBottom exactly (the axes do not agree on the off-by-one) and left as the BUG note below says. Then it walks the
//  placed records and stops at the first overlap; the nudge is vertical when nAnchor <= 6 and horizontal otherwise, and each axis then chooses a
//  direction from a two-part guard:
//      vertical:   up    (*pnBottom - nTop) / 2 != 0 && nTop <= nMaxBottom
//                  down  otherwise
//      horizontal: left  (*pnRight - nLeft) / 2 != 0 && nLeft >= nMaxRight
//                  right otherwise
//  Neither guard is a midpoint test: (*pnBottom - nTop) is the label's own height and (*pnRight - nLeft) its own width, each halved (signed, round
//  toward zero) and tested for zero, and the second half compares the label's own edge with the bound the clamps just enforced.
//  In normal use the down and left arms are never taken: the bottom clamp has just forced *pnBottom <= nMaxBottom and the caller's box is at least
//  two high for any shipped font, and the left arm needs an inverted box (nLeft > *pnRight), which only the left clamp's defect can produce. So a
//  colliding label is pushed up and right. Each shift moves both edges by the same amount, so the box slides.
//  The assert is the original's, sgnNumShowItems < MAX_SHOW_ITEMS (showitems.cpp:71), and does not return (FOG_DisplayAssert, then exit(-1)); it
//  cannot fire from the one caller, which bounds the count with the same 32.
int32_t __fastcall SHOWITEMS_FitLabelRect(int32_t *pnLeft, int32_t *pnTop, int32_t *pnRight, int32_t *pnBottom, int32_t nAnchor, int32_t nMaxRight, int32_t nMinLeft)
{
    int32_t nMaxBottom;
    int32_t nRight;
    int32_t nBottom;
    int32_t nLabel;
    int32_t nLeft;
    int32_t nTop;
    int32_t nShift;

    nMaxBottom = gnScreenHeight - 48;

    if (*pnLeft < nMinLeft)
    {
        //  BUG: the shift is by nMinLeft but the left edge is then set to a literal 6, not to nMinLeft (0x6FB09DAF). The caller accepts a label
        //  only if pnLeft >= nMinLeft, so a label that needed the clamp under a non-zero nMinLeft (the PANEL_LEFT half screen,
        //  nMinLeft == gnScreenWidth / 2) is moved to x = 6 and then rejected instead of shown clamped; with nMinLeft == 0 it is accepted with
        //  its left edge six pixels in from where the shift assumed it. REACHABLE: visible in a stock game whenever the left panel is open.
        //  Reproduced as the bytes have it.
        *pnRight += nMinLeft - *pnLeft;
        *pnLeft = 6;
    }
    if (*pnTop < 6)
    {
        *pnBottom += 6 - *pnTop;
        *pnTop = 6;
    }

    nRight = *pnRight;
    if (*pnRight >= nMaxRight)
    {
        *pnRight = nMaxRight - 1;
        *pnLeft -= nRight - nMaxRight + 1;
    }

    nBottom = *pnBottom;
    if (*pnBottom >= nMaxBottom)
    {
        *pnBottom = nMaxBottom;
        *pnTop -= nBottom - nMaxBottom;
    }

    if (gnNumShowItems >= MAX_SHOW_ITEMS)
    {
        FOG_DisplayAssert((uint32_t)szAssertNumShowItems, (uint32_t)szShowItemsCpp, 71);
        exit(-1);
    }

    nLabel = 0;
    if (gnNumShowItems <= 0)
    {
        return 1;
    }

    nLeft = *pnLeft;
    nTop = *pnTop;
    for (;;)
    {
        BOOL bOverlapX = nLeft <= gShowItemLabels[nLabel].nRight && *pnRight >= gShowItemLabels[nLabel].nLeft;
        BOOL bOverlapY = nTop <= gShowItemLabels[nLabel].nBottom && *pnBottom >= gShowItemLabels[nLabel].nTop;
        if (bOverlapX && bOverlapY)
        {
            break;
        }
        if (++nLabel >= gnNumShowItems)
        {
            return 1;
        }
    }

    if (nAnchor <= 6)
    {
        if ((*pnBottom - nTop) / 2 != 0 && nTop <= nMaxBottom)
        {
            nShift = gShowItemLabels[nLabel].nTop - *pnBottom - 3;
        }
        else
        {
            nShift = gShowItemLabels[nLabel].nBottom - nTop + 5;
        }
        *pnTop = nTop + nShift;
        *pnBottom += nShift;
        return 0;
    }
    else if ((*pnRight - nLeft) / 2 != 0 && nLeft >= nMaxRight)
    {
        nShift = gShowItemLabels[nLabel].nLeft - *pnRight - 1;
        *pnLeft = nLeft + nShift;
        *pnRight += nShift;
        return 0;
    }
    else
    {
        nShift = gShowItemLabels[nLabel].nRight - nLeft + 1;
        *pnLeft = gShowItemLabels[nLabel].nRight + 1;
        *pnRight += nShift;
        return 0;
    }
}

//1.10f: D2Client.0x6FB09F60
//  The ground-item name pass, drawn while ALT is held. Walks the player's room and every room DUNGEON_GetAdjacentRoomsListFromRoom returns, and
//  builds one label per item unit on the floor. The gate is dwFlagEx bit 7 (the bit the ground-label code tests to decide whether an item is
//  already known) plus UNIT_ITEM and the 32-record bound. gnNumShowItems is reset to 0 at the top, so the list is rebuilt every frame.
//  Two placement modes, chosen once by D2GFX_CheckPerspective at the top:
//      perspective off  client coords minus the viewport origin, and the label sixteen pixels above: -16 - originY
//      perspective on   precision coords through SCALE_AdjustPerspectivePosition with nBais 0, then a quarter-screen shift by the UI side:
//                       w / -4 on PANEL_RIGHT, w / 4 on PANEL_LEFT
//  The UI side then bounds the label's X: PANEL_RIGHT caps the right edge at w/2 - 32, PANEL_LEFT floors the left edge at w/2 (32 pixels, not
//  MAX_SHOW_ITEMS). A label outside [-8, gnScreenHeight - 16] on Y is dropped, and so is one whose rectangle ends up outside the X bounds.
//  The label text is built by UIPANELS_BuildItemLabelText and measured by D2Win_10131, the rectangle is placed through SHOWITEMS_FitLabelRect up
//  to twelve times, and the last attempt's result decides whether the record is used at all. The highlight is two arms: the mouse over the
//  rectangle, or the unit already the selected one; both paint the cached MixRGB(0, 0x40, 0x80) background with DrawMode 5. Everything else is
//  the quality colour, with the socketable/ethereal 5 and the quest override of 4 tested last; 'leg ' (Wirt's Leg) is the one quest item that
//  keeps its quality colour.
//  The draw pass runs from the left and bottom edges of each record through D2Win_10132, which takes six arguments (wszText, nLeft, nBottom,
//  dwBgColor, eDrawMode, nTextColor), the first two in ecx and edx; the frame ends by clearing the selection it may have set, unless one of the
//  two highlight arms ran.
//  BUG: pLabel->pItem = pUnit is written before the perspective-position test can reject the unit. HARMLESS: it lands beyond gnNumShowItems.
void __cdecl SHOWITEMS_DrawGroundItemLabels(void)
{
    int32_t bPerspective;
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    D2UnitStrc *pSelected;
    int32_t nMouseX;
    int32_t nMouseY;
    D2ActiveRoomStrc **ppRoomList;
    int32_t nNumRooms;
    uint8_t nBgColor;
    uint32_t iRoom;
    D2UnitStrc *pUnit;
    D2ClientShowItemLabelStrc *pLabel;
    int32_t bSelectionClaimed;
    int32_t nScreenWidth;
    int32_t nLabelX;
    int32_t nLabelY;
    int32_t nMinLeft;
    int32_t nMaxRight;
    int32_t nUiShift;
    int32_t nPrecisionX;
    int32_t nPrecisionY;
    int32_t nXAdjust;
    int32_t nYAdjust;
    int32_t nWidth;
    int32_t nHeight;
    int32_t nLeft;
    int32_t nRight;
    int32_t nBottom;
    int32_t nAnchor;
    int32_t bPlaced;
    int32_t bMayHighlight;
    int32_t nLabelColor;
    const D2ItemsTxt *pItemsTxt;
    int32_t iShowItem;

    bPerspective = D2GFX_CheckPerspective();
    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        return;
    }
    pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
    if (!pRoom)
    {
        return;
    }

    pSelected = SELECT_GetSelectedUnit();
    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    D2COMMON_DUNGEON_GetAdjacentRoomsListFromRoom(pRoom, &ppRoomList, &nNumRooms);

    //  One palette index for every highlight this frame; the original stores the byte into the low end of a dword and reads it back masked.
    nBgColor = D2WIN_10034_MixRGB(0, 0x40, 0x80);

    gnNumShowItems = 0;
    bSelectionClaimed = 0;
    if (nNumRooms == 0)
    {
        goto DRAW_LABELS;
    }

    //  The back edge is jb (unsigned), so the counter is unsigned and the bound cast so the compare matches.
    for (iRoom = 0; iRoom < (uint32_t)nNumRooms; ++iRoom)
    {
        for (pUnit = ppRoomList[iRoom]->pUnitFirst; pUnit; pUnit = pUnit->pRoomNext)
        {
            //  shr edx, 7: logical, and dwFlagEx is an unsigned member, so `>>` is the same shift.
            //  DIFFERENCE: the original tests and indexes with a local mirror of the count held in esi and re-syncs it as nNumShowItems =
            //  ++gnNumShowItems (0x6FB0A37C); ours reads the global each time. Nothing between the two writes it, so the values cannot part.
            if (((pUnit->dwFlagEx >> 7) & 1) == 0 || pUnit->dwUnitType != UNIT_ITEM || gnNumShowItems >= MAX_SHOW_ITEMS)
            {
                continue;
            }

            pLabel = &gShowItemLabels[gnNumShowItems];
            pLabel->pItem = pUnit;

            if (bPerspective == 0)
            {
                nLabelX = D2COMMON_UNITS_GetClientCoordX(pUnit)
                          - SCROLL_GetViewportOriginX();
                nLabelY = D2COMMON_UNITS_GetClientCoordY(pUnit) + (-16 - SCROLL_GetViewportOriginY());
                nScreenWidth = gnScreenWidth;
            }
            else
            {
                nPrecisionX = D2COMMON_UNITS_GetPrecisionX(pUnit);
                nPrecisionY = D2COMMON_UNITS_GetPrecisionY(pUnit);
                if (!D2GFX_SCALE_CheckPerspectivePosition(nPrecisionX, nPrecisionY))
                {
                    continue;
                }
                D2GFX_SCALE_AdjustPerspectivePosition(nPrecisionX, nPrecisionY, 0, &nXAdjust, &nYAdjust);
                nUiShift = SCROLL_GetUiShiftScreen();
                nScreenWidth = gnScreenWidth;
                if (nUiShift == PANEL_RIGHT)
                {
                    nXAdjust += gnScreenWidth / -4;
                }
                else if (nUiShift == PANEL_LEFT)
                {
                    nXAdjust += gnScreenWidth / 4;
                }
                nLabelX = nXAdjust;
                nLabelY = nYAdjust;
            }

            nBottom = nLabelY;
            nMinLeft = 0;
            nMaxRight = nScreenWidth;
            nUiShift = SCROLL_GetUiShiftScreen();
            if (nUiShift == PANEL_RIGHT)
            {
                nMaxRight = gnScreenWidth / 2 - 32;
            }
            else if (nUiShift == PANEL_LEFT)
            {
                nMinLeft = gnScreenWidth / 2;
            }

            if (nLabelX < nMinLeft || nLabelX > nMaxRight || nLabelY < -8 || nLabelY > gnScreenHeight - 16)
            {
                continue;
            }

            pLabel->wszText[0].ch = 0;
            UIPANELS_BuildItemLabelText(pUnit, pLabel->wszText, 256);
            D2WIN_10131_GetTextDimensions(pLabel->wszText, &nWidth, &nHeight);
            bPlaced = 1;
            nWidth += 8;
            nLeft = nLabelX - (nWidth >> 1);
            //  From here nHeight is the rectangle's top, not the text height.
            nHeight = nLabelY - nHeight + 4;
            nRight = nLeft + nWidth;
            nAnchor = 0;
            for (;;)
            {
                if (SHOWITEMS_FitLabelRect(&nLeft, &nHeight, &nRight, &nBottom, nAnchor, nMaxRight, nMinLeft))
                {
                    break;
                }
                //  BUG: after twelve attempts the label is dropped, not placed where it last landed (0x6FB0A1CB).
                if (++nAnchor >= 12)
                {
                    bPlaced = 0;
                    break;
                }
            }

            if (nRight <= nMaxRight && nLeft >= nMinLeft && bPlaced)
            {
                nLabelColor = 0;
                bMayHighlight = (bSelectionClaimed == 0);
                pLabel->nLeft = nLeft;
                pLabel->nTop = nHeight;
                pLabel->nRight = nRight;
                pLabel->nBottom = nBottom;

                if (bMayHighlight && nMouseX >= nLeft && nMouseX <= nRight && nMouseY >= nHeight && nMouseY <= nBottom + 4)
                {
                    pLabel->dwBgColor = nBgColor;
                    pLabel->eDrawMode = DRAWMODE_NORMAL;
                    pLabel->nTextColor = 0;
                    SELECT_SetSelectedUnit(pUnit);
                    bSelectionClaimed = 1;
                }
                else if (bMayHighlight && pUnit == pSelected)
                {
                    pLabel->dwBgColor = nBgColor;
                    pLabel->eDrawMode = DRAWMODE_NORMAL;
                    pLabel->nTextColor = 0;
                    D2WIN_10129_DrawFramedText(NULL, 0, 0, 0, 0);
                    SELECT_ClearSelectedUnit();
                    bSelectionClaimed = 1;
                }
                else
                {
                    pLabel->dwBgColor = 0;
                    pLabel->eDrawMode = DRAWMODE_TRANS50;
                    switch (D2COMMON_ITEMS_GetItemQuality(pUnit))
                    {
                    case ITEMQUAL_MAGIC:    nLabelColor = 3;  break;
                    case ITEMQUAL_SET:      nLabelColor = 2;  break;
                    case ITEMQUAL_RARE:     nLabelColor = 9;  break;
                    case ITEMQUAL_UNIQUE:   nLabelColor = 4;  break;
                    case ITEMQUAL_CRAFT:    nLabelColor = 8;  break;
                    case ITEMQUAL_TEMPERED: nLabelColor = 10; break;
                    default:
                        //  The line numbers are the original's own, pushed as the assert's third argument; not this file's.
                        if ((D2COMMON_ITEMS_CheckItemFlag(pUnit, IFLAG_SOCKETED, 262, szShowItemsCpp) && D2COMMON_ITEMS_CheckIfSocketable(pUnit) != 0 && D2COMMON_ITEMS_GetMaxSockets(pUnit) != 0)
                            || D2COMMON_ITEMS_CheckItemFlag(pUnit, IFLAG_ETHEREAL, 268, szShowItemsCpp))
                        {
                            nLabelColor = 5;
                        }
                        break;
                    }

                    //  'leg ' -- 0x2067656C, little-endian, Wirt's Leg.
                    //  BUG: DATATBLS_GetItemsTxtRecord returns NULL for a dwClassId at or above the record count, and 0x6FB0A35B loads nQuest from
                    //  it regardless. LATENT. Reproduced.
                    pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(pUnit->dwClassId);
                    if (pItemsTxt->nQuest && pItemsTxt->dwCode != 0x2067656C)
                    {
                        nLabelColor = 4;
                    }
                    pLabel->nTextColor = nLabelColor;
                }
                ++gnNumShowItems;
            }
        }
    }

DRAW_LABELS:
    for (iShowItem = 0; iShowItem < gnNumShowItems; ++iShowItem)
    {
        pLabel = &gShowItemLabels[iShowItem];
        D2WIN_10132(pLabel->wszText, pLabel->nLeft, pLabel->nBottom, pLabel->dwBgColor, (DrawMode)pLabel->eDrawMode, pLabel->nTextColor);
    }

    if (!bSelectionClaimed)
    {
        SELECT_ClearSelectedUnit();
        D2WIN_10129_DrawFramedText(NULL, 0, 0, 0, 0);
    }
}

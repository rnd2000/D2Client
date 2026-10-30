#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "WINMAIN.h"
#include "CoreArchive.h"
#include "ui.h"
#include "npcmenu.h"
#include "panel.h"
#include "inv.h"

//  inv.cpp -- the inventory family of D2Client.dll 1.10: the inventory grid, the item description panel, the item name and price text
//  builders, the shop panel's open, close and release paths, and the message handlers of the mercenary inventory, trade, cube, stash and
//  shop panels. The original source file is UI\inv.cpp (the asserts in this unit name it). Function starts span 0x6FADD2F0..0x6FAEE730.
//
//  Panel to screen space. UIPANELS_DrawInGamePanels is the only writer of gnBorderXDifference (+80) and gnBorderYDifference (-60). The panels
//  convert with one of four forms:
//      left-anchored X    gnBorderXDifference + k
//      right-anchored X   gnScreenWidth - gnBorderXDifference - k
//      bottom-anchored Y  gnBorderYDifference + gnScreenHeight - k
//      top-anchored Y     k - gnBorderYDifference
//  The mercenary close button's hit box and INV_MercInvDrawPanel's drawing use the same bottom-anchored expression (0x6FAED5D0).
//
//  gMercInvSlots is eleven D2InvCompGridStrc of 20 bytes; the array index is the D2C_PlayerBodyLocs value (both loops step a body location
//  alongside the slot pointer). Neither loop knows the count: each stops on an end address, and the two addresses are different symbols of
//  the image. INV_MercInvOnMouseUp's cursor starts at &slots[0].rect.nRight (+4) and stops at >= 0x6FBB5B18, INV_MercInvOnMouseMove's
//  starts at &slots[0] and stops at < 0x6FBB5B14. Both mean eleven (0x6FBB5A38 + 11 * 20 is 0x6FBB5B14); an end-address sentinel is not a variable.

//  Layout checks. The rectangles of this unit are in the order Left, Right, Top, Bottom, not Windows' RECT order.
static_assert(offsetof(D2InvRectStrc, nLeft) == 0
                  && offsetof(D2InvRectStrc, nRight) == 4
                  && offsetof(D2InvRectStrc, nTop) == 8
                  && offsetof(D2InvRectStrc, nBottom) == 12,
              "Left, RIGHT, Top, Bottom -- NOT the Windows RECT order");
static_assert(offsetof(D2InventoryGridInfoStrc, nGridX) == 0
                  && offsetof(D2InventoryGridInfoStrc, nGridY) == 1
                  && offsetof(D2InventoryGridInfoStrc, nGridLeft) == 4
                  && offsetof(D2InventoryGridInfoStrc, nGridRight) == 8
                  && offsetof(D2InventoryGridInfoStrc, nGridTop) == 0x0C
                  && offsetof(D2InventoryGridInfoStrc, nGridBottom) == 0x10,
              "the grid's bounds, in the same Left/RIGHT/Top/Bottom order");
static_assert(offsetof(D2InventoryGridInfoStrc, nGridBoxWidth) == 0x14
                  && offsetof(D2InventoryGridInfoStrc, nGridBoxHeight) == 0x15,
              "the cell size, two ADJACENT BYTES -- a swap is invisible");
static_assert(offsetof(D2InvCompGridStrc, rect) == 0
                  && offsetof(D2InvCompGridStrc, nWidth) == 0x10
                  && offsetof(D2InvCompGridStrc, nHeight) == 0x11,
              "the body-location grid: a rect and then two adjacent bytes");
static_assert(offsetof(SMSGHANDLER_PARAMS, nXpos) == 0x0C
                  && offsetof(SMSGHANDLER_PARAMS, nYpos) == 0x0E,
              "the mouse position; nYpos is the HIGH half of nXpos' dword");
static_assert(offsetof(SMSGHANDLER_PARAMS, wParam) == 8
                  && offsetof(SMSGHANDLER_PARAMS, bReturn) == 0x18
                  && offsetof(SMSGHANDLER_PARAMS, lResult) == 0x1C,
              "what a handler is given and what it answers with");
//  D2ItemsTxt: nMinDam and nMaxDam are adjacent bytes.
static_assert(offsetof(D2ItemsTxt, nMinDam) == 0xFE
                  && offsetof(D2ItemsTxt, nMaxDam) == 0xFF,
              "min then max, one byte apart");
static_assert(offsetof(D2ItemsTxt, wReqStr) == 0x10A
                  && offsetof(D2ItemsTxt, wReqDex) == 0x10C,
              "strength then dexterity, and both are WORDS");
static_assert(offsetof(D2ItemsTxt, nSpellDesc) == 0xB4
                  && offsetof(D2ItemsTxt, wSpellDescStr) == 0xB6,
              "the spell-description selector and its tbl index");
static_assert(offsetof(D2ItemsTxt, wNameStr) == 0xF4
                  && offsetof(D2ItemsTxt, nSkipName) == 0x144
                  && offsetof(D2ItemsTxt, nBlock) == 0x111,
              "the name index, the skip-name flag and the block chance");
static_assert(offsetof(D2ItemsTxt, nQuest) == 0x12A
                  && offsetof(D2ItemsTxt, nQuestDiffCheck) == 0x12B,
              "the quest pair, also two adjacent bytes");
static_assert(offsetof(D2ItemsTxt, wStat) == 0x9E
                  && offsetof(D2ItemsTxt, dwCalc) == 0xA4,
              "the three stat ids and the three calc values beside them");
static_assert(UI_INVENTORY == 1 && UI_NPCSHOP == 12 && UI_STASH == 25
                  && UI_CUBE == 26,
              "the four panel ids this unit toggles");
static_assert(UI_TURNON == 0 && UI_TURNOFF == 1 && UI_TOGGLE == 2,
              "UI_SetUIToggle's nUIState, and TURNON being 0 reads backwards");
static_assert(UNIT_PLAYER == 0 && UNIT_MONSTER == 1 && UNIT_ITEM == 4,
              "the unit types this unit switches on");

//1.10f: D2Client.0x6FAEE340
//  Left button pressed on the mercenary inventory: arms the close button. Like the other two handlers of this table it first fetches the
//  player's associated unit of kind 7; if that guid is -1 or resolves to no monster, the panel toggles itself closed (UI_TOGGLE, not
//  UI_TURNOFF) and the handler returns without writing bReturn, so the click is not consumed.
//  The cursor is bounds-checked before the button (0 <= x <= gnScreenWidth / 2 and 0 <= y <= gnScreenHeight - 49, all inclusive); failing
//  it leaves bReturn at 0, so the click falls through to the panel behind. The mouse is read again inside for the close button itself.
void __stdcall INV_MercInvOnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    uint32_t dwMercGUID;
    int32_t nPanelBottom;
    int32_t nHalfScreenWidth;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nCloseLeft;
    int32_t nCloseRight;
    int32_t nCloseTop;
    int32_t nCloseBottom;

    dwMercGUID = ROSTERPETS_GetAssociatedUnitGUID(CUNIT_GetPlayer(), 7, 0);
    if (dwMercGUID == -1 || !CUNIT_FindUnitByGuidAndType(dwMercGUID, UNIT_MONSTER))
    {
        //  no bReturn written on this path
        UI_SetUIToggle(UI_MERCINV, UI_TOGGLE, 0);
        return;
    }

    pMsg->bReturn = 0;
    nPanelBottom = gnScreenHeight - 49;
    nHalfScreenWidth = gnScreenWidth / 2;
    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();

    if (nMouseX < 0 || nMouseX > nHalfScreenWidth || nMouseY < 0 || nMouseY > nPanelBottom)
    {
        return;
    }

    //  D2ExpRes: MercenaryCloseButtonClickdown [Mercenary] detours at 0x4E3FB,
    //  the `lea edx, [eax+ecx-3Fh]` this -63 compiles to.
    nCloseBottom = gnBorderYDifference + gnScreenHeight - 63;
    nCloseTop = gnBorderYDifference + gnScreenHeight - 95;
    nCloseRight = gnBorderXDifference + 304;
    nCloseLeft = gnBorderXDifference + 272;
    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();

    if (nMouseX < nCloseLeft || nMouseX > nCloseRight || nMouseY < nCloseTop || nMouseY > nCloseBottom)
    {
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        return;
    }

    gbMercCloseBtnPressed = 1;
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAEE490
//  Left button released on the mercenary inventory: the equip and un-equip path. The close button first, then a walk of the eleven
//  body-location slots; with an item on the cursor it tries to put it on the mercenary, with an empty cursor it takes the slot's item off.
//  The equip test runs twice on the way to failing: arm 1 is CheckBodyLocation && CheckRequirements && INV_MercInvCanMercEquipItem (equip);
//  arm 2 calls CheckBodyLocation and INV_MercInvCanMercEquipItem again and tests *pbFits (the "wrong item" voice, 86); otherwise the "no"
//  voice, 85. The second calls are kept because INV_MercInvCanMercEquipItem writes pbFits through its out-parameter each time.
//  Equipping sends MSG_SendCmdWord(0x61, 0); unequipping sends the slot's body location. The loop skips slots whose rect.nLeft is not
//  positive but still advances the body-location counter, which keeps the slot index and D2C_PlayerBodyLocs in step.
void __stdcall INV_MercInvOnMouseUp(SMSGHANDLER_PARAMS *pMsg)
{
    uint32_t dwMercGUID;
    D2UnitStrc *pMerc;
    D2UnitStrc *pCursorItem;
    D2UnitStrc *pSlotItem;
    D2InventoryStrc *pMercInventory;
    D2InvCompGridStrc *pSlot;
    //  an int rather than the enum: it is incremented, which C++ does not allow on an enumeration
    int32_t nBodyLoc;
    int32_t nPanelBottom;
    int32_t nHalfScreenWidth;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nCloseLeft;
    int32_t nCloseRight;
    int32_t nCloseTop;
    int32_t nCloseBottom;
    int32_t nSlotLeft;
    int32_t nSlotRight;
    int32_t nSlotTop;
    int32_t nSlotBottom;
    int32_t bFits;
    D2C_Sounds nDropSound;

    dwMercGUID = ROSTERPETS_GetAssociatedUnitGUID(CUNIT_GetPlayer(), 7, 0);
    if (dwMercGUID == -1)
    {
        UI_SetUIToggle(UI_MERCINV, UI_TOGGLE, 0);
        return;
    }
    pMerc = CUNIT_FindUnitByGuidAndType(dwMercGUID, UNIT_MONSTER);
    if (!pMerc)
    {
        UI_SetUIToggle(UI_MERCINV, UI_TOGGLE, 0);
        return;
    }

    pMsg->bReturn = 0;
    CURSOR_OnPanelMouseUp(pMsg);
    gbMercCloseBtnPressed = 0;

    nPanelBottom = gnScreenHeight - 49;
    nHalfScreenWidth = gnScreenWidth / 2;
    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    if (nMouseX < 0 || nMouseX > nHalfScreenWidth || nMouseY < 0 || nMouseY > nPanelBottom)
    {
        return;
    }

    nCloseTop = gnBorderYDifference + gnScreenHeight - 95;
    nCloseBottom = gnBorderYDifference + gnScreenHeight - 63;
    nCloseRight = gnBorderXDifference + 304;
    nCloseLeft = gnBorderXDifference + 272;
    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();

    if (nMouseX >= nCloseLeft && nMouseX <= nCloseRight && nMouseY >= nCloseTop && nMouseY <= nCloseBottom)
    {
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        UI_SetUIToggle(UI_MERCINV, UI_TOGGLE, 0);
        return;
    }

    pCursorItem = D2COMMON_INVENTORY_GetCursorItem(CUNIT_GetPlayer()->pInventory);

    for (nBodyLoc = BODYLOC_NONE, pSlot = gMercInvSlots;; ++nBodyLoc, ++pSlot)
    {
        //  End-address sentinel for eleven slots: the original's cursor is &slots[i].rect.nRight and stops at 0x6FBB5B18 (cmp esi, 6FBB5B18h / jl at
        //  0x6FAEE636); ours compares the record pointer with one past the array, the same compare less four on both sides, signed as jl.
        if ((int32_t)pSlot >= (int32_t)(gMercInvSlots + 11))
        {
            GAME_ResetMouseButtonState();
            pMsg->lResult = 0;
            pMsg->bReturn = 1;
            STORM_SMsgBreakHandlerChain(pMsg);
            return;
        }

        nSlotLeft = pSlot->rect.nLeft;
        if (nSlotLeft <= 0)
        {
            continue;
        }

        nSlotRight = pSlot->rect.nRight;
        nSlotTop = pSlot->rect.nTop;
        nSlotBottom = pSlot->rect.nBottom;
        nMouseX = CURSOR_GetMouseX();
        nMouseY = CURSOR_GetMouseY();

        if (nMouseX < nSlotLeft || nMouseX > nSlotRight || nMouseY < nSlotTop || nMouseY > nSlotBottom)
        {
            continue;
        }

        if (pCursorItem)
        {
            bFits = 0;
            if (D2COMMON_ITEMS_CheckBodyLocation(pCursorItem, nBodyLoc) && D2COMMON_ITEMS_CheckRequirements(pCursorItem, pMerc, 0, NULL, NULL, NULL) && INV_MercInvCanMercEquipItem(pMerc, pCursorItem, &bFits))
            {
                ITEM_SetUninterruptableState(pCursorItem);
                nDropSound = ITEM_GetDropSoundNoFrame(pCursorItem);
                SOUNDINST_PlaySound(nDropSound, NULL, 0, 0, 0);
                MSG_SendCmdWord(97, 0);
                pMsg->lResult = 0;
            }
            //  both predicates run a second time here
            else if (D2COMMON_ITEMS_CheckBodyLocation(pCursorItem, nBodyLoc) && INV_MercInvCanMercEquipItem(pMerc, pCursorItem, &bFits) && bFits)
            {
                UNITSND_PlayUnitSoundEvent(pMerc->dwUnitId, 1u, 86);
                pMsg->lResult = 0;
            }
            else
            {
                UNITSND_PlayUnitSoundEvent(pMerc->dwUnitId, 1u, 85);
                pMsg->lResult = 0;
            }

            pMsg->bReturn = 1;
            STORM_SMsgBreakHandlerChain(pMsg);
            return;
        }

        pMercInventory = pMerc->pInventory;
        if (pMercInventory)
        {
            pSlotItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pMercInventory, (D2C_PlayerBodyLocs)nBodyLoc);
            if (pSlotItem)
            {
                ITEM_SetUninterruptableState(pSlotItem);
                MSG_SendCmdWord(97, (int16_t)nBodyLoc);
                gbShowItemDesc = 0;
                gbShowSetItemDesc = 0;
                gpHoverItem = NULL;
            }
        }
    }
}

//1.10f: D2Client.0x6FAEE730
//  Cursor moved over the mercenary inventory: the hover description. Same opening and outer bounds test as the other two handlers.
//  If gbMercCloseBtnPressed is set and the cursor is now outside the close box the flag is cleared, so dragging off the button cancels it.
//  The hover state (ten globals) is cleared to -1 or 0 on every move inside the panel, before any slot is tested, so moving across dead
//  space between two slots blanks the item description for that frame. The slot walk runs only while the cursor holds no item.
//  A slot with an item sets gbShowSetItemDesc = 1 on every move and writes the five popup coordinates only when the item differs from
//  gpHoverItemLast. The popup X, (rect.nLeft + rect.nRight) / 2, is written into both gnDescPopupX and gnDescPopupAltX.
//  This loop's end sentinel is a different symbol from INV_MercInvOnMouseUp's (see the file head).
void __stdcall INV_MercInvOnMouseMove(SMSGHANDLER_PARAMS *pMsg)
{
    uint32_t dwMercGUID;
    D2UnitStrc *pMerc;
    D2UnitStrc *pHoverItem;
    D2InventoryStrc *pMercInventory;
    D2InvCompGridStrc *pSlot;
    //  an int rather than the enum: it is incremented, which C++ does not allow on an enumeration
    int32_t nBodyLoc;
    int32_t nPanelBottom;
    int32_t nHalfScreenWidth;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nCloseLeft;
    int32_t nCloseRight;
    int32_t nCloseTop;
    int32_t nCloseBottom;
    int32_t nSlotLeft;
    int32_t nSlotRight;
    int32_t nSlotTop;
    int32_t nSlotBottom;
    int32_t nPopupX;

    dwMercGUID = ROSTERPETS_GetAssociatedUnitGUID(CUNIT_GetPlayer(), 7, 0);
    if (dwMercGUID == -1)
    {
        UI_SetUIToggle(UI_MERCINV, UI_TOGGLE, 0);
        return;
    }
    pMerc = CUNIT_FindUnitByGuidAndType(dwMercGUID, UNIT_MONSTER);
    if (!pMerc)
    {
        UI_SetUIToggle(UI_MERCINV, UI_TOGGLE, 0);
        return;
    }

    pMsg->bReturn = 0;
    nPanelBottom = gnScreenHeight - 49;
    nHalfScreenWidth = gnScreenWidth / 2;
    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    if (nMouseX < 0 || nMouseX > nHalfScreenWidth || nMouseY < 0 || nMouseY > nPanelBottom)
    {
        return;
    }

    if (gbMercCloseBtnPressed)
    {
        nCloseTop = gnBorderYDifference + gnScreenHeight - 95;
        nCloseBottom = gnBorderYDifference + gnScreenHeight - 63;
        nCloseRight = gnBorderXDifference + 304;
        nCloseLeft = gnBorderXDifference + 272;
        nMouseX = CURSOR_GetMouseX();
        nMouseY = CURSOR_GetMouseY();
        if (nMouseX < nCloseLeft || nMouseX > nCloseRight || nMouseY < nCloseTop || nMouseY > nCloseBottom)
        {
            gbMercCloseBtnPressed = 0;
        }
    }

    pMercInventory = pMerc->pInventory;
    if (!pMercInventory)
    {
        return;
    }

    //  cleared on every move inside the panel, before anything is tested
    gnDescPopupX = -1;
    gnDescPopupY = -1;
    gnDescPopupAltX = -1;
    gnDescPopupBottomY = -1;
    gnHoverCellX = -1;
    gnHoverCellY = -1;
    gbShowItemDesc = 0;
    gbShowSetItemDesc = 0;
    gpHoverItem = NULL;
    gpHoverItemLast = 0;

    if (D2COMMON_INVENTORY_GetCursorItem(CUNIT_GetPlayer()->pInventory))
    {
        return;
    }

    nBodyLoc = BODYLOC_NONE;
    pSlot = gMercInvSlots;
    do
    {
        nSlotLeft = pSlot->rect.nLeft;
        if (nSlotLeft > 0)
        {
            nSlotRight = pSlot->rect.nRight;
            nSlotBottom = pSlot->rect.nBottom;
            nSlotTop = pSlot->rect.nTop;
            nMouseX = CURSOR_GetMouseX();
            nMouseY = CURSOR_GetMouseY();

            if (nMouseX >= nSlotLeft && nMouseX <= nSlotRight && nMouseY >= nSlotTop && nMouseY <= nSlotBottom)
            {
                pHoverItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pMercInventory, (D2C_PlayerBodyLocs)nBodyLoc);
                if (pHoverItem)
                {
                    //  set before the "is it new" test: refreshed on every move while the cursor stays on the slot
                    gbShowSetItemDesc = 1;
                    if (pHoverItem != gpHoverItemLast)
                    {
                        nPopupX = pSlot->rect.nLeft + pSlot->rect.nRight;
                        gpHoverItem = pHoverItem;
                        gnDescPopupX = nPopupX / 2;
                        gnDescPopupAltX = nPopupX / 2;
                        gnDescPopupY = pSlot->rect.nTop;
                        gnDescPopupBottomY = pSlot->rect.nBottom;
                        gpHoverItemLast = pHoverItem;
                    }
                }
            }
        }
        ++pSlot;
        ++nBodyLoc;
        //  the other end-address sentinel: 0x6FBB5B14 (cmp esi, 6FBB5B14h / jl at 0x6FAEE955), one past gMercInvSlots
    }
    while ((int32_t)pSlot < (int32_t)(gMercInvSlots + 11));
}

//  Message handlers of the trade window. sgInventoryMode == 11 gates three of its handlers: they are registered whenever the table is,
//  so the mode decides whether a trade is in progress, and a message arriving in any other mode falls through unconsumed.

//1.10f: D2Client.0x6FAEABD0
//  Swallows the message. The trade window binds it to both right-button messages, so a right-click anywhere in a trade window does nothing
//  and is not passed on (right-click moves items in every other item panel). Shared by several tables.
void __stdcall INV_ConsumeMessage(SMSGHANDLER_PARAMS *pMsg)
{
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAEABF0
//  Enter opens the chat box: UI_SetUIToggle(UI_CHATBOX, UI_TURNON, 0). Four panels bind it to VK_RETURN and share this handler; it has no
//  code caller. The two guards at the top (chat box already open, an NPC dialog up) return without consuming the message. On a key repeat
//  (lParam bit 30) the UI_SetUIToggle is skipped but the message is still consumed, so holding Enter opens the chat box once.
void __stdcall INV_OnChatKey(SMSGHANDLER_PARAMS *pMsg)
{
    if (UI_GetUiWindowStateRaw(UI_CHATBOX))
    {
        return;
    }
    if (NPCMENU_IsAnyDialogOpen())
    {
        return;
    }

    if ((pMsg->lParam & 0x40000000) == 0)
    {
        UI_SetUIToggle(UI_CHATBOX, UI_TURNON, 0);
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAEB870
//  Escape or Space cancels the trade (VK_ESCAPE once and VK_SPACE twice in the table; the second VK_SPACE is a separate rebindable binding).
//  The message is consumed before the chat-box guard is tested: with the chat box open Escape is swallowed here and the trade is not
//  cancelled. Whether an earlier handler in the chain takes the key first is not established.
void __stdcall INV_TradeOnDismissKey(SMSGHANDLER_PARAMS *pMsg)
{
    if (sgInventoryMode != 11)
    {
        return;
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    if (!UI_GetUiWindowStateRaw(UI_CHATBOX))
    {
        TRADE_SendCancelOnce();
    }
}

//1.10f: D2Client.0x6FAEC440
//  The belt key, 0xC0 (the backtick), two table slots. Unlike the other trade handlers it has no sgInventoryMode gate, so it acts whenever
//  the trade table is registered, and it consumes unconditionally, repeat or not.
void __stdcall INV_TradeOnShowBeltKey(SMSGHANDLER_PARAMS *pMsg)
{
    if ((pMsg->lParam & 0x40000000) == 0)
    {
        CMDTBL_ActionShowBelt();
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAEC470
//  'W' swaps weapons: expansion only, shared by three panels. In classic it does nothing at all, not even consume: the outer
//  GAME_IsExpansionGame() test wraps the bReturn store as well as the work, so 'W' falls through to the next handler.
//  The command goes out first (MSG_SendCmd1(0x60)); then BODYLOC_RARM and BODYLOC_LARM are looked up and each hand that holds an item is
//  marked uninterruptable, with the inventory pointer fetched once for both. The original inlines INV_SwapWeaponsAndLockItems here.
void __stdcall INV_OnKeySwapWeapons(SMSGHANDLER_PARAMS *pMsg)
{
    D2InventoryStrc *pInventory;
    D2UnitStrc *pItem;

    if (!GAME_IsExpansionGame())
    {
        return;
    }

    if ((pMsg->lParam & 0x40000000) == 0
        && !UI_GetUiWindowStateRaw(UI_CHATBOX)
        && FOG_IsExpansion()
        //  the expansion test of the inlined INV_SwapWeaponsAndLockItems, never false here
        && GAME_IsExpansionGame())
    {
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
        MSG_SendCmd1(96);

        pInventory = CUNIT_GetPlayer()->pInventory;
        pItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RARM);
        if (pItem)
        {
            ITEM_SetUninterruptableState(pItem);
        }
        pItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_LARM);
        if (pItem)
        {
            ITEM_SetUninterruptableState(pItem);
        }
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAEB1D0
//  Cursor moved over the trade window. Three rectangles are tested and the cursor must miss all three for the hover state to be blanked;
//  inside any of them the work goes to INV_OnMouseMove: gInvPanelRect (the player's own grid), gInvTradePage2Grid (the other side of the
//  trade) and gInvTradePage1Grid (this side). The left edges disagree: gInvPanelRect rejects x < nLeft (inclusive), both trade grids reject
//  x <= nGridLeft (exclusive); top and bottom agree across all three.
//  The blank is the ten hover globals that INV_MercInvOnMouseMove clears plus D2WIN_10129_DrawFramedText(NULL, -1, -1, 0, 0), a draw
//  call made to clear.
void __stdcall INV_TradeOnMouseMove(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;

    if (sgInventoryMode != 11)
    {
        return;
    }

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    gnUiMouseX = nXpos;
    gnUiMouseY = nYpos;

    if ((nXpos >= gInvPanelRect.nLeft
         && nXpos < gInvPanelRect.nRight
         && nYpos >= gInvPanelRect.nTop
         && nYpos <= gInvPanelRect.nBottom)
        || (nXpos > gInvTradePage2Grid.nGridLeft
            && nXpos < gInvTradePage2Grid.nGridRight
            && nYpos >= gInvTradePage2Grid.nGridTop
            && nYpos <= gInvTradePage2Grid.nGridBottom)
        || (nXpos > gInvTradePage1Grid.nGridLeft
            && nXpos < gInvTradePage1Grid.nGridRight
            && nYpos >= gInvTradePage1Grid.nGridTop
            && nYpos <= gInvTradePage1Grid.nGridBottom))
    {
        INV_OnMouseMove(pMsg);
        return;
    }

    gnDescPopupX = -1;
    gnDescPopupY = -1;
    gnDescPopupAltX = -1;
    gnDescPopupBottomY = -1;
    gnHoverCellX = -1;
    gnHoverCellY = -1;
    gbShowItemDesc = 0;
    gbShowSetItemDesc = 0;
    gpHoverItem = NULL;
    gpHoverItemLast = 0;
    D2WIN_10129_DrawFramedText(NULL, -1, -1, 0, 0);
}

//1.10f: D2Client.0x6FAEB2D0
//  Left button pressed in the trade window. Outcomes in the order the original tests them: the close button (right-anchored, arms
//  gbInvCloseBtnArmed); a grid cell (handed to INV_OnLButtonDown); an item on the cursor anywhere else (un-accepts the trade); the accept
//  button (toggles the green check); the cancel button (arms the cancel); the gold button (arms the gold drop).
//  PANEL_BeltHitTestPanel is called up to three times for one click: !x guards the close button, != 1 guards everything after it and == 1
//  guards the final reset. The first and second tests are the same test only if it returns nothing but 0 and 1; three calls are written.
//  An item on the cursor on a click outside the grids and the close button clears all four trade flags and sends the green check, which
//  is the un-accept.
//  The accept button: checked -> unchecked sends TRADE_SendGreenCheck at once; unchecked -> checked sets the flag, arms the press and plays
//  the sound but sends nothing (the accept goes out from INV_TradeOnLButtonUp).
//  gbTradeCancelBtnArmed and gbTradeAcceptBtnArmed are stored after SMsgBreakHandlerChain returns, on the fall-through path only.
void __stdcall INV_TradeOnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    D2UnitStrc *pPlayer;

    if (sgInventoryMode != 11)
    {
        return;
    }

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    gnUiMouseX = nXpos;
    gnUiMouseY = nYpos;

    //  the close button, right-anchored
    if (!PANEL_BeltHitTestPanel(pMsg) && nXpos >= gnScreenWidth - gnBorderXDifference - 302 && nXpos <= gnScreenWidth - gnBorderXDifference - 270 && nYpos >= gnBorderYDifference + gnScreenHeight - 96
        && nYpos <= gnBorderYDifference + gnScreenHeight - 64)
    {
        gbInvCloseBtnArmed = 1;
        goto CONSUME_WITH_SOUND;
    }

    //  a second call, and a different test from the first
    if (PANEL_BeltHitTestPanel(pMsg) == 1)
    {
        return;
    }

    if ((nXpos >= gInvPanelRect.nLeft && nXpos < gInvPanelRect.nRight && nYpos >= gInvPanelRect.nTop && nYpos <= gInvPanelRect.nBottom) || (nXpos > gInvTradePage2Grid.nGridLeft && nXpos < gInvTradePage2Grid.nGridRight
            && nYpos >= gInvTradePage2Grid.nGridTop && nYpos <= gInvTradePage2Grid.nGridBottom))
    {
        INV_OnLButtonDown(pMsg);
        return;
    }

    pPlayer = CUNIT_GetPlayer();
    if (D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory))
    {
        //  holding an item retracts your acceptance
        gbTradeAcceptBtnArmed = 0;
        gbTradeAcceptChecked = 0;
        gbPanelBtnPressActive = 0;
        gbTradeCancelPending = 0;
        TRADE_SendGreenCheck();
        goto CONSUME;
    }

    //  the accept button, left-anchored
    if (nXpos > gnBorderXDifference + 223 && nXpos < gnBorderXDifference + 255 && nYpos > gnBorderYDifference + gnScreenHeight - 96 && nYpos < gnBorderYDifference + gnScreenHeight - 64)
    {
        gbTradeAcceptBtnArmed = 1;
        if (gbTradeAcceptChecked == 1)
        {
            gbTradeAcceptChecked = 0;
            TRADE_SendGreenCheck();
        }
        else
        {
            //  the accept itself goes out from the release handler
            gbTradeAcceptChecked = 1;
            gbPanelBtnPressActive = 1;
            SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
        }
        goto CONSUME;
    }

    //  the cancel button, left-anchored
    if (nXpos > gnBorderXDifference + 275 && nXpos < gnBorderXDifference + 307 && nYpos > gnBorderYDifference + gnScreenHeight - 96 && nYpos < gnBorderYDifference + gnScreenHeight - 64)
    {
        gbTradeCancelBtnArmed = 1;
        gbTradeCancelPending = 1;
        gbPanelBtnPressActive = 1;
        goto CONSUME_WITH_SOUND;
    }

    //  the gold button, right-anchored and only 20 by 18
    pPlayer = CUNIT_GetPlayer();
    if (!D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory) && nYpos >= gnBorderYDifference + gnScreenHeight - 87 && nYpos <= gnBorderYDifference + gnScreenHeight - 69 && nXpos >= gnScreenWidth - gnBorderXDifference - 237
        && nXpos <= gnScreenWidth - gnBorderXDifference - 217)
    {
        gbDropGoldBtnPressed = 1;
        gbTradeAcceptBtnArmed = 0;
        gbTradeAcceptChecked = 0;
        gbPanelBtnPressActive = 0;
        gbTradeCancelPending = 0;
        SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
    }
    //  a third call to the belt hit test
    else if (PANEL_BeltHitTestPanel(pMsg) == 1)
    {
        gbTradeAcceptChecked = 0;
        gbPanelBtnPressActive = 0;
        gbTradeCancelPending = 0;
        gbTradeAcceptBtnArmed = 0;
        return;
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
    //  stored after the chain is broken, on this path only
    gbTradeCancelBtnArmed = 0;
    gbTradeAcceptBtnArmed = 0;
    return;

CONSUME_WITH_SOUND:
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
    STORM_SMsgBreakHandlerChain(pMsg);
    return;

CONSUME:
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAEB5B0
//  Left button released in the trade window: where the accept and the cancel actually go out.
//  Out of trade mode (sgInventoryMode != 11) it returns at once, but not before writing gbPanelBtnPressActive = 0.
//  Releasing on the close button calls TRADE_SendCancelOnce() and returns without writing bReturn, so the release falls through to whatever
//  is behind the trade window; every other arm consumes.
//  Each send needs two flags set by different events: accept = gbTradeAcceptChecked && gbTradeAcceptBtnArmed, cancel =
//  gbTradeCancelPending && gbTradeCancelBtnArmed. The Checked and Pending halves survive between clicks and the Armed halves are set by
//  the press and cleared here, so a release only commits if the press that armed it landed on the same button.
//  A release inside gInvTradePage2Grid jumps to the tail, which consumes, sends the green check if the accept was checked, then clears all
//  three flags; the item move itself was handed to INV_OnLButtonDown by the press.
//  UI_OpenGoldEntryDialog(2) is not followed by a return, so the gbPanelBtnPressActive block below runs in the same pass.
void __stdcall INV_TradeOnLButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    D2InventoryStrc *pInventory;
    D2UnitStrc *pPlayer;

    if (sgInventoryMode != 11)
    {
        gbPanelBtnPressActive = 0;
        return;
    }

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    gnUiMouseX = nXpos;
    gnUiMouseY = nYpos;
    gbInvCloseBtnArmed = 0;
    gbInvCloseBtnArmedUnread = 0;

    pInventory = CUNIT_GetPlayer()->pInventory;
    if (!PANEL_BeltHitTestPanel(pMsg) && !D2COMMON_INVENTORY_GetCursorItem(pInventory))
    {
        if (nXpos >= gnScreenWidth - gnBorderXDifference - 302 && nXpos <= gnScreenWidth - gnBorderXDifference - 270 && nYpos >= gnBorderYDifference + gnScreenHeight - 96 && nYpos <= gnBorderYDifference + gnScreenHeight - 64)
        {
            //  no bReturn written on this path
            TRADE_SendCancelOnce();
            return;
        }
    }

    if (nXpos > gInvTradePage2Grid.nGridLeft && nXpos < gInvTradePage2Grid.nGridRight && nYpos >= gInvTradePage2Grid.nGridTop && nYpos <= gInvTradePage2Grid.nGridBottom)
    {
        goto RETRACT_TAIL;
    }

    //  the accept button
    if (nXpos > gnBorderXDifference + 223 && nXpos < gnBorderXDifference + 255 && nYpos > gnBorderYDifference + gnScreenHeight - 96 && nYpos < gnBorderYDifference + gnScreenHeight - 64)
    {
        if (gbTradeAcceptChecked && gbTradeAcceptBtnArmed)
        {
            TRADE_SendAccept();
        }
        gbTradeAcceptBtnArmed = 0;
        goto CONSUME_CLEAR_PRESS;
    }

    //  the cancel button
    if (nXpos > gnBorderXDifference + 275 && nXpos < gnBorderXDifference + 307 && nYpos > gnBorderYDifference + gnScreenHeight - 96 && nYpos < gnBorderYDifference + gnScreenHeight - 64)
    {
        if (gbTradeCancelPending && gbTradeCancelBtnArmed)
        {
            TRADE_SendCancelOnce();
            gbTradeCancelPending = 0;
        }
        gbTradeCancelBtnArmed = 0;
        goto CONSUME_CLEAR_PRESS;
    }

    if (PANEL_BeltHitTestPanel(pMsg) == 1)
    {
        gbTradeAcceptBtnArmed = 0;
        gbTradeAcceptChecked = 0;
        gbPanelBtnPressActive = 0;
        gbTradeCancelPending = 0;
        return;
    }

    pPlayer = CUNIT_GetPlayer();
    if (!D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory) && nYpos >= gnBorderYDifference + gnScreenHeight - 87 && nYpos <= gnBorderYDifference + gnScreenHeight - 69 && nXpos >= gnScreenWidth - gnBorderXDifference - 237
        && nXpos <= gnScreenWidth - gnBorderXDifference - 217 && !gbGoldDialogOpen)
    {
        //  and no return -- the block below runs too
        UI_OpenGoldEntryDialog(2);
    }

    if (!gbPanelBtnPressActive)
    {
        goto RETRACT_TAIL;
    }

    gbPanelBtnPressActive = 0;
    if (gbTradeAcceptBtnArmed)
    {
        if (gbTradeAcceptChecked == 1)
        {
            gbTradeAcceptChecked = 0;
            TRADE_SendGreenCheck();
        }
    }
    else if (gbTradeCancelBtnArmed && gbTradeCancelPending)
    {
        gbTradeCancelPending = 0;
    }
    gbTradeAcceptBtnArmed = 0;
    gbTradeCancelBtnArmed = 0;
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
    return;

CONSUME_CLEAR_PRESS:
    gbPanelBtnPressActive = 0;
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
    return;

RETRACT_TAIL:
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
    if (gbTradeAcceptChecked)
    {
        TRADE_SendGreenCheck();
    }
    gbPanelBtnPressActive = 0;
    gbTradeCancelPending = 0;
    gbTradeAcceptChecked = 0;
}

//  The Horadric Cube panel's handlers, and the right-button handler that three item panels share (INV_ItemPanelOnRButtonDown).
//  sgInventoryMode is the panel selector tested by every handler of this family: 1 the plain inventory, 2 to 4 the shop's modes, 11 a
//  trade, 12 the stash, 14 the cube. INV_ItemPanelOnRButtonDown does not gate on it but switches on it.

//1.10f: D2Client.0x6FAE9B60
//  Right button pressed inside an item panel: shared by the cube, the stash and the shop tables. It runs in every mode and picks its grid
//  from sgInventoryMode instead of being gated by it, which is why three tables can point at it.
//  The mode-12 (stash) path calls GAME_IsExpansionGame() twice: once to choose gInvBigStashGrid over gInvStashGrid for the "is the cursor in
//  the stash" test and once more, after the inventory-rect test, to decide between using the item and playing the refusal sound.
//  The refusal is UNITSND_PlayPlayerSoundEvent(pPlayer, 0x13), the "cannot do that" grunt, reached from the cube grid's miss and the stash's.
//  The original passes its incoming parameter slot to NPCMENU_GetInteractingNpc as the out-buffer for a guid and reads it back; ours uses
//  a separate local, since the C cannot alias a parameter that way.

//  The clearing loop that the original writes out six times in this function: it zeroes the first int of four 20-byte records of
//  gNpcShopTabs (the bPressed column). The six copies differ in nothing, so this helper stands for all of them.
//  DIFFERENCE: the original's end-address sentinel names a field of the next struct (0x6FBB37FC, four bytes into gInvTradePage1Grid:
//  cmp eax, 6FBB37FCh / jl at 0x6FAE9CC9), i.e. &gNpcShopTabs[4].bPressed, four bytes past the array. Ours carries the record in an integer
//  cursor, compares less four on both sides (signed, as the bytes compare it) and reads the record only while it is inside the table.
static void ITEMPANEL_ClearShopTabs(void)
{
    UINT_PTR nTab;

    nTab = (UINT_PTR)&gNpcShopTabs[0];
    do
    {
        ((D2InvButtonInfoStrc *)nTab)->bPressed = 0;
        nTab += sizeof(D2InvButtonInfoStrc);
    }
    while ((int32_t)nTab < (int32_t)((UINT_PTR)&gNpcShopTabs[0] + 4 * sizeof(D2InvButtonInfoStrc)));
}

void __stdcall INV_ItemPanelOnRButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    D2UnitStrc *pPlayer;
    D2UnitStrc *pNpc;
    D2UnitStrc *pCursorItem;
    D2UnitGUID nNpcGUID;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nCursorState;

    pPlayer = CUNIT_GetPlayer();
    if (GAME_IsDisconnecting() || !pPlayer || CUNIT_IsPlayerDeadOrMissing())
    {
        return;
    }

    nMouseX = pMsg->nXpos;
    nMouseY = pMsg->nYpos;

    if (nMouseX >= gInvPanelGrid.nGridLeft && nMouseX <= gInvPanelGrid.nGridRight && nMouseY >= gInvPanelGrid.nGridTop && nMouseY <= gInvPanelGrid.nGridBottom)
    {
        INV_OnRButtonDown(pMsg);
        return;
    }

    if (PANEL_BeltHitTestPanel(pMsg) == 1)
    {
        return;
    }

    if (CURSOR_GetCursorState() == 6)
    {
        UI_CursorSendUseHeldItem();
        goto CONSUME;
    }
    if (CURSOR_GetCursorState() == 8)
    {
        MSG_SendServerItemMessage(76, -1);
        goto CONSUME;
    }

    if (D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory))
    {
        //  holding an item: only the shop modes react at all
        if (sgInventoryMode < 2 || sgInventoryMode > 4)
        {
            goto CONSUME;
        }
        ITEMPANEL_ClearShopTabs();
        goto RESET_MODE;
    }

    if (sgInventoryMode == 1 || sgInventoryMode == 2)
    {
        //  the shop panel; note the inventory rect's nLeft is used as a right-hand limit
        if (nMouseX < 0 || nMouseX > gInvPanelRect.nLeft || nMouseY < 0 || nMouseY > gInvPanelRect.nBottom)
        {
            goto CONSUME;
        }

        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);

        if (nMouseX >= gInvNpcShopGrid.nGridLeft && nMouseX <= gInvNpcShopGrid.nGridRight && nMouseY >= gInvNpcShopGrid.nGridTop && nMouseY <= gInvNpcShopGrid.nGridBottom)
        {
            if (gpHoverItem)
            {
                //  the original passes the parameter slot itself as the out-buffer
                if (NPCMENU_GetInteractingNpc(&nNpcGUID))
                {
                    pNpc = CUNIT_FindUnitByGuidAndType(nNpcGUID, UNIT_MONSTER);
                    if (pNpc)
                    {
                        if (!gpHoverItem || !NPCMENU_BeginTransaction(pNpc, gpHoverItem, 0, nMouseX, nMouseY, pMsg->nKey, 1, 0))
                        {
                            goto CONSUME;
                        }
                        pCursorItem = NULL;
                        goto SET_HELD;
                    }
                }
                ITEMPANEL_ClearShopTabs();
                sgInventoryMode = 1;
                goto TAKE_CURSOR_ITEM;
            }

            if (sgInventoryMode != 1 && sgInventoryMode != 2 && sgInventoryMode != 4 && sgInventoryMode != 3)
            {
                goto CONSUME;
            }
            pPlayer = CUNIT_GetPlayer();
            if (D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory))
            {
                goto CONSUME;
            }
            nCursorState = CURSOR_GetCursorState();
            if (nCursorState != 7 && nCursorState != 1 && nCursorState != 4)
            {
                goto CONSUME;
            }
            ITEMPANEL_ClearShopTabs();
            goto RESET_MODE;
        }

        //  outside the shop grid, still inside the outer box
        if (sgInventoryMode != 1 && sgInventoryMode != 2 && sgInventoryMode != 4 && sgInventoryMode != 3)
        {
            goto CONSUME;
        }
        pPlayer = CUNIT_GetPlayer();
        if (D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory))
        {
            goto CONSUME;
        }
        nCursorState = CURSOR_GetCursorState();
        if (nCursorState != 7 && nCursorState != 1 && nCursorState != 4)
        {
            goto CONSUME;
        }
        ITEMPANEL_ClearShopTabs();
        goto RESET_MODE;
    }

    if (sgInventoryMode == 14)
    {
        //  the cube
        if (nMouseX < gInvCubeGrid.nGridLeft || nMouseX >= gInvCubeGrid.nGridRight || nMouseY < gInvCubeGrid.nGridTop || nMouseY > gInvCubeGrid.nGridBottom)
        {
            goto CONSUME;
        }
        goto REFUSE;
    }

    if (sgInventoryMode == 12)
    {
        //  the stash, and the expansion test is the first of two
        if (GAME_IsExpansionGame())
        {
            if (nMouseX <= gInvBigStashGrid.nGridLeft || nMouseX > gInvBigStashGrid.nGridRight || nMouseY < gInvBigStashGrid.nGridTop || nMouseY > gInvBigStashGrid.nGridBottom)
            {
                goto SHOP_TABS_BY_MODE;
            }
        }
        else if (nMouseX <= gInvStashGrid.nGridLeft || nMouseX > gInvStashGrid.nGridRight || nMouseY < gInvStashGrid.nGridTop || nMouseY > gInvStashGrid.nGridBottom)
        {
            goto SHOP_TABS_BY_MODE;
        }

        if (nMouseX >= gInvPanelRect.nLeft && nMouseX < gInvPanelRect.nRight && nMouseY >= gInvPanelRect.nTop && nMouseY <= gInvPanelRect.nBottom)
        {
            INV_OnRButtonDown(pMsg);
            goto CONSUME;
        }

        //  and the second expansion test, on the same two grids
        if (GAME_IsExpansionGame())
        {
            if (nMouseX <= gInvBigStashGrid.nGridLeft || nMouseX > gInvBigStashGrid.nGridRight || nMouseY < gInvBigStashGrid.nGridTop || nMouseY > gInvBigStashGrid.nGridBottom)
            {
                goto REFUSE;
            }
        }
        else if (nMouseX <= gInvStashGrid.nGridLeft || nMouseX > gInvStashGrid.nGridRight || nMouseY < gInvStashGrid.nGridTop || nMouseY > gInvStashGrid.nGridBottom)
        {
            goto REFUSE;
        }

        INV_UseItemAtGridCell(pPlayer, pPlayer->pInventory, nMouseX, nMouseY, INVPAGE_STASH);
        goto CONSUME;
    }

SHOP_TABS_BY_MODE:
    if (sgInventoryMode < 2 || sgInventoryMode > 4)
    {
        goto CONSUME;
    }
    ITEMPANEL_ClearShopTabs();

RESET_MODE:
    sgInventoryMode = 1;

TAKE_CURSOR_ITEM:
    pPlayer = CUNIT_GetPlayer();
    pCursorItem = D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory);

SET_HELD:
    CURSOR_SetHeldUnit(pCursorItem);
    goto CONSUME;

REFUSE:
    UNITSND_PlayPlayerSoundEvent(pPlayer, 0x13u);

CONSUME:
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAEC720
//  Cursor moved over the cube: the same shape as INV_TradeOnMouseMove with the same two rectangles (the player's inventory and the cube's
//  own grid), but it clears only four hover globals where the trade and mercenary panels clear ten; the six popup coordinates stay.
void __stdcall INV_CubeOnMouseMove(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;

    if (sgInventoryMode != 14)
    {
        return;
    }

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    gnUiMouseX = nXpos;
    gnUiMouseY = nYpos;

    if ((nXpos >= gInvPanelRect.nLeft && nXpos < gInvPanelRect.nRight && nYpos >= gInvPanelRect.nTop && nYpos <= gInvPanelRect.nBottom) || (nXpos >= gInvCubeGrid.nGridLeft && nXpos < gInvCubeGrid.nGridRight && nYpos >= gInvCubeGrid.nGridTop
            && nYpos <= gInvCubeGrid.nGridBottom))
    {
        INV_OnMouseMove(pMsg);
        return;
    }

    //  four, where the other two panels clear ten
    gbShowItemDesc = 0;
    gbShowSetItemDesc = 0;
    gpHoverItem = NULL;
    gpHoverItemLast = 0;
    D2WIN_10129_DrawFramedText(NULL, -1, -1, 0, 0);
}

//1.10f: D2Client.0x6FAECEA0
//  Escape closes the cube; the close is the same six lines the release handler uses.
//  The ten-global hover blank runs only if !UI_GetUiWindowState(UI_INVENTORY): closing the cube while the inventory is up leaves the item
//  description alone, since the inventory owns it.
//  The close is guarded by the chat box: with the chat box open the message is consumed and the cube stays open. The two guards are
//  independent, and neither happens if the mode is not 14.
//  If gbCubeOpenedFromStash is set, closing re-interacts with the cube's object unit, UNITMODE_InteractWithUnit(2, gnCubeUnitGUID), but only
//  if that unit is still findable; the stash comes back, and if the cube object has gone the flag is cleared anyway.
void __stdcall INV_CubeOnDismissKey(SMSGHANDLER_PARAMS *pMsg)
{
    if (sgInventoryMode != 14)
    {
        return;
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    //  the inventory still owns the hover if it is open
    if (!UI_GetUiWindowState(UI_INVENTORY))
    {
        gbShowItemDesc = 0;
        gnDescPopupX = -1;
        gnDescPopupY = -1;
        gnDescPopupAltX = -1;
        gnDescPopupBottomY = -1;
        gnHoverCellX = -1;
        gnHoverCellY = -1;
        gbShowSetItemDesc = 0;
        gpHoverItem = NULL;
        gpHoverItemLast = 0;
    }

    if (UI_GetUiWindowStateRaw(UI_CHATBOX))
    {
        return;
    }

    gbHoradricAnimActive = 0;
    UI_SetUIToggle(UI_CUBE, UI_TURNOFF, 0);
    if (!gbCubeCloseCmdSent)
    {
        MSG_SendCmd3Words(79, 23, 0, 0);
        gbCubeCloseCmdSent = 1;
    }
    if (gbCubeOpenedFromStash)
    {
        if (CUNIT_FindUnitByGuidAndType(gnCubeUnitGUID, UNIT_OBJECT))
        {
            UNITMODE_InteractWithUnit(2u, gnCubeUnitGUID);
        }
        gbCubeOpenedFromStash = 0;
    }
}

//1.10f: D2Client.0x6FAEC7C0
//  Left button pressed on the cube panel. Five boxes: the panel's own close button (right-anchored, with the trade panel's coordinates),
//  the cube's close button (left-anchored, 275..315 by h-100..h-60), the transmute button (144..184 by h-223..h-183), the gold button and
//  the two item grids.
//  The transmute button is refused with an item on the cursor (CURSOR_GetHeldUnit() non-null): nothing is armed and no sound plays, but
//  lResult = 0 and SMsgBreakHandlerChain still run and bReturn is left as the caller had it, so this is the one path of the function that
//  consumes the message without claiming it.
//  The 800x600 fall-through after the grids and the gold button (bReturn still 0, resolution 800x600, not on the belt, y <= gnScreenHeight - 48
//  and UiShift != PANEL_RIGHT or x >= w/2) has a further inner condition: SCROLL_GetUiShiftScreen() != PANEL_BOTH || !sgInventoryMode ||
//  x >= w/2. sgInventoryMode is 14 here, so the whole condition always passes; it is written out. The block also calls
//  UI_CursorSendUseHeldItem() on cursor state 6.
void __stdcall INV_CubeOnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    D2UnitStrc *pPlayer;

    if (sgInventoryMode != 14)
    {
        return;
    }

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    gnUiMouseX = nXpos;
    gnUiMouseY = nYpos;

    if (!PANEL_BeltHitTestPanel(pMsg) && nXpos >= gnScreenWidth - gnBorderXDifference - 302 && nXpos <= gnScreenWidth - gnBorderXDifference - 270 && nYpos >= gnBorderYDifference + gnScreenHeight - 96
        && nYpos <= gnBorderYDifference + gnScreenHeight - 64)
    {
        gbInvCloseBtnArmed = 1;
        goto CONSUME_WITH_SOUND;
    }

    //  the cube's own close button
    if (nXpos > gnBorderXDifference + 275 && nXpos < gnBorderXDifference + 315 && nYpos > gnBorderYDifference + gnScreenHeight - 100 && nYpos < gnBorderYDifference + gnScreenHeight - 60)
    {
        gbCubeCloseBtnArmed = 1;
        gbCubeCloseBtnPressed = 1;
        gbCubeTransmuteBtnArmed = 0;
        gbCubeTransmuteBtnPressed = 0;
        gbPanelBtnPressActive = 1;
        goto CONSUME_WITH_SOUND;
    }

    //  the TRANSMUTE button
    if (nXpos > gnBorderXDifference + 144 && nXpos < gnBorderXDifference + 184 && nYpos > gnBorderYDifference + gnScreenHeight - 223 && nYpos < gnBorderYDifference + gnScreenHeight - 183)
    {
        gbCubeCloseBtnArmed = 0;
        gbCubeCloseBtnPressed = 0;
        if (!CURSOR_GetHeldUnit())
        {
            gbCubeTransmuteBtnArmed = 1;
            gbCubeTransmuteBtnPressed = 1;
            gbPanelBtnPressActive = 1;
            pMsg->bReturn = 1;
            SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
        }
        //  bReturn is not written when the cursor is full
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        return;
    }

    if (PANEL_BeltHitTestPanel(pMsg) == 1)
    {
        gbTradeAcceptChecked = 0;
        gbPanelBtnPressActive = 0;
        gbCubeCloseBtnPressed = 0;
        gbCubeTransmuteBtnPressed = 0;
        goto DISARM;
    }

    if ((nXpos >= gInvPanelRect.nLeft && nXpos < gInvPanelRect.nRight && nYpos >= gInvPanelRect.nTop && nYpos <= gInvPanelRect.nBottom) || (nXpos >= gInvCubeGrid.nGridLeft && nXpos < gInvCubeGrid.nGridRight && nYpos >= gInvCubeGrid.nGridTop
            && nYpos <= gInvCubeGrid.nGridBottom))
    {
        INV_OnLButtonDown(pMsg);
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        goto LATE_800x600;
    }

    pPlayer = CUNIT_GetPlayer();
    if (!D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory) && nYpos >= gnBorderYDifference + gnScreenHeight - 87 && nYpos <= gnBorderYDifference + gnScreenHeight - 69 && nXpos >= gnScreenWidth - gnBorderXDifference - 237
        && nXpos <= gnScreenWidth - gnBorderXDifference - 217)
    {
        gbDropGoldBtnPressed = 1;
        gbTradeAcceptBtnArmed = 0;
        gbTradeAcceptChecked = 0;
        gbPanelBtnPressActive = 0;
        gbCubeCloseBtnPressed = 0;
        gbCubeTransmuteBtnPressed = 0;
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
        STORM_SMsgBreakHandlerChain(pMsg);
        goto LATE_800x600;
    }

    //  the cube panel's own rect, with NO top bound
    if (nXpos >= gInvCubeRect.nLeft && nXpos < gInvCubeRect.nRight && nYpos <= gInvCubeRect.nBottom)
    {
        gbTradeAcceptBtnArmed = 0;
        gbTradeAcceptChecked = 0;
        gbPanelBtnPressActive = 0;
        gbCubeCloseBtnPressed = 0;
        gbCubeCloseBtnArmed = 0;
        gbCubeTransmuteBtnPressed = 0;
        gbCubeTransmuteBtnArmed = 0;
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        return;
    }

LATE_800x600:
    if (!pMsg->bReturn && D2GFX_GetResolutionMode() == D2GAMERES_800x600 && !PANEL_BeltHitTestPanel(pMsg) && nYpos <= gnScreenHeight - 48 && (SCROLL_GetUiShiftScreen() != PANEL_RIGHT || nXpos >= gnScreenWidth / 2))
    {
        if (CURSOR_GetCursorState() == 6)
        {
            UI_CursorSendUseHeldItem();
        }
        //  sgInventoryMode is 14 here, so this cannot fail
        if (SCROLL_GetUiShiftScreen() != PANEL_BOTH || !sgInventoryMode || nXpos >= gnScreenWidth / 2)
        {
            pMsg->bReturn = 1;
            gbShowItemDesc = 0;
            gbShowSetItemDesc = 0;
            STORM_SMsgBreakHandlerChain(pMsg);
        }
    }

DISARM:
    gbTradeAcceptBtnArmed = 0;
    gbCubeCloseBtnArmed = 0;
    gbCubeTransmuteBtnArmed = 0;
    return;

CONSUME_WITH_SOUND:
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAECB60
//  Left button released on the cube panel, where the transmute is sent: command 0x4F with word 23 closes the cube, word 24 transmutes.
//  BUG: before the gbPanelBtnPressActive block the function stores 0 into gbCubeCloseBtnPressed, gbCubeCloseBtnArmed, gbCubeTransmuteBtnPressed
//  and gbCubeTransmuteBtnArmed, and the chain at the tail then tests exactly those flags, so three of the four bodies in that chain can
//  never run (only the gbTradeAcceptBtnArmed arm survives, since that flag is not among the four). UNREACHABLE: all of them are written out.
//  The cube panel shares gbTradeAcceptBtnArmed, gbTradeAcceptChecked, gbPanelBtnPressActive and gbDropGoldBtnPressed with INV_TradeOnLButtonUp;
//  only sgInventoryMode keeps the panels apart.
//  The panel close button calls UI_SetUIToggle(UI_CUBE, UI_TURNOFF, 0) and returns without writing bReturn, as INV_TradeOnLButtonUp does;
//  it is guarded by PANEL_InvHintPhaseSuppressed() and by there being no item on the cursor. The gold dialog opens with
//  UI_OpenGoldEntryDialog(1) here and (2) in the trade window.
void __stdcall INV_CubeOnLButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    D2InventoryStrc *pInventory;
    D2UnitStrc *pPlayer;

    if (sgInventoryMode != 14)
    {
        gbPanelBtnPressActive = 0;
        return;
    }

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    gbInvCloseBtnArmed = 0;
    gbInvCloseBtnArmedUnread = 0;
    gnUiMouseX = nXpos;
    gnUiMouseY = nYpos;

    pInventory = CUNIT_GetPlayer()->pInventory;
    if (!PANEL_InvHintPhaseSuppressed() && !D2COMMON_INVENTORY_GetCursorItem(pInventory) && nXpos >= gnScreenWidth - gnBorderXDifference - 302 && nXpos <= gnScreenWidth - gnBorderXDifference - 270 && nYpos >= gnBorderYDifference + gnScreenHeight - 96
        && nYpos <= gnBorderYDifference + gnScreenHeight - 64)
    {
        //  no bReturn written on this path
        UI_SetUIToggle(UI_CUBE, UI_TURNOFF, 0);
        return;
    }

    //  the cube's own close button
    if (nXpos > gnBorderXDifference + 275 && nXpos < gnBorderXDifference + 315 && nYpos > gnBorderYDifference + gnScreenHeight - 100 && nYpos < gnBorderYDifference + gnScreenHeight - 60)
    {
        if (gbCubeCloseBtnPressed)
        {
            gbCubeCloseBtnPressed = 0;
            gbHoradricAnimActive = 0;
            UI_SetUIToggle(UI_CUBE, UI_TURNOFF, 0);
            if (!gbCubeCloseCmdSent)
            {
                MSG_SendCmd3Words(79, 23, 0, 0);
                gbCubeCloseCmdSent = 1;
            }
            if (gbCubeOpenedFromStash)
            {
                if (CUNIT_FindUnitByGuidAndType(gnCubeUnitGUID, UNIT_OBJECT))
                {
                    UNITMODE_InteractWithUnit(2u, gnCubeUnitGUID);
                }
                gbCubeOpenedFromStash = 0;
            }
        }
        gbPanelBtnPressActive = 0;
        gbCubeCloseBtnArmed = 0;
        goto CONSUME;
    }

    //  the TRANSMUTE button
    if (nXpos > gnBorderXDifference + 144 && nXpos < gnBorderXDifference + 184 && nYpos > gnBorderYDifference + gnScreenHeight - 223 && nYpos < gnBorderYDifference + gnScreenHeight - 183)
    {
        if (gbCubeTransmuteBtnPressed)
        {
            gbCubeTransmuteBtnPressed = 0;
            MSG_SendCmd3Words(79, 24, 0, 0);
        }
        gbCubeTransmuteBtnArmed = 0;
        gbPanelBtnPressActive = 0;
        goto CONSUME;
    }

    //  the four stores that make the arms at the tail unreachable
    gbCubeCloseBtnPressed = 0;
    gbCubeCloseBtnArmed = 0;
    gbCubeTransmuteBtnPressed = 0;
    gbCubeTransmuteBtnArmed = 0;

    if (PANEL_BeltHitTestPanel(pMsg) == 1)
    {
        gbTradeAcceptBtnArmed = 0;
        gbCubeCloseBtnArmed = 0;
        gbCubeTransmuteBtnArmed = 0;
        return;
    }

    pPlayer = CUNIT_GetPlayer();
    if (!D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory) && nYpos >= gnBorderYDifference + gnScreenHeight - 87 && nYpos <= gnBorderYDifference + gnScreenHeight - 69 && nXpos >= gnScreenWidth - gnBorderXDifference - 237
        && nXpos <= gnScreenWidth - gnBorderXDifference - 217 && !gbGoldDialogOpen)
    {
        //  mode 1 here, mode 2 in the trade panel
        UI_OpenGoldEntryDialog(1);
    }

    gbDropGoldBtnPressed = 0;

    if (!gbPanelBtnPressActive)
    {
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        gbPanelBtnPressActive = 0;
        gbCubeCloseBtnPressed = 0;
        gbCubeTransmuteBtnPressed = 0;
        gbTradeAcceptChecked = 0;
        return;
    }

    gbPanelBtnPressActive = 0;
    if (gbTradeAcceptBtnArmed)
    {
        if (gbTradeAcceptChecked)
        {
            gbTradeAcceptChecked = 0;
        }
    }
    //  both arms below are unreachable: the four stores above cleared every flag they test
    else if (gbCubeCloseBtnArmed)
    {
        if (gbCubeCloseBtnPressed)
        {
            gbCubeCloseBtnPressed = 0;
        }
    }
    else if (gbCubeTransmuteBtnArmed && gbCubeTransmuteBtnPressed)
    {
        gbCubeTransmuteBtnPressed = 0;
    }
    gbTradeAcceptBtnArmed = 0;
    gbCubeTransmuteBtnArmed = 0;
    gbCubeCloseBtnArmed = 0;

CONSUME:
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//  The stash panel's handlers. GAME_IsExpansionGame() picks between two grids and two rects at every hit test in this family:
//  gInvBigStashGrid / gInvBigStashRect (the expansion's larger stash) and gInvStashGrid / gInvStashRect (classic). The close button moves by
//  one pixel in each axis (left dx + 272 against dx + 275, bottom h - 64 against h - 65) and the withdraw-gold button by nearly two hundred
//  (y h-455..h-438 against h-259..h-242, at the same x dx+73..dx+150).
//  Three of the four handlers accept sgInventoryMode == 12 || == 13; INV_StashOnDismissKey accepts only 12, so Escape does not close mode 13.
//  What mode 13 is has not been established.

//1.10f: D2Client.0x6FAEB9F0
//  Cursor moved over the stash. The cursor must miss both the player's inventory rect and the stash grid for the hover to be blanked;
//  inside either, the work goes to INV_OnMouseMove. The stash test is split across the expansion branch: three of the four bounds are tested
//  inside each arm and the bottom is tested once, outside them, against a value the arms picked.
//  Like the cube's, this handler clears only four hover globals, not the ten the trade and mercenary panels clear.
void __stdcall INV_StashOnMouseMove(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    int32_t nGridBottom;

    if (sgInventoryMode != 12 && sgInventoryMode != 13)
    {
        return;
    }

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    gnUiMouseX = nXpos;
    gnUiMouseY = nYpos;

    if (nXpos >= gInvPanelRect.nLeft && nXpos < gInvPanelRect.nRight && nYpos >= gInvPanelRect.nTop && nYpos <= gInvPanelRect.nBottom)
    {
        INV_OnMouseMove(pMsg);
        return;
    }

    if (GAME_IsExpansionGame())
    {
        if (nXpos <= gInvBigStashGrid.nGridLeft || nXpos > gInvBigStashGrid.nGridRight || nYpos < gInvBigStashGrid.nGridTop)
        {
            goto BLANK;
        }
        nGridBottom = gInvBigStashGrid.nGridBottom;
    }
    else
    {
        if (nXpos <= gInvStashGrid.nGridLeft || nXpos > gInvStashGrid.nGridRight || nYpos < gInvStashGrid.nGridTop)
        {
            goto BLANK;
        }
        nGridBottom = gInvStashGrid.nGridBottom;
    }

    //  the fourth bound, hoisted out of both arms
    if (nYpos > nGridBottom)
    {
        goto BLANK;
    }

    INV_OnMouseMove(pMsg);
    return;

BLANK:
    gbShowItemDesc = 0;
    gbShowSetItemDesc = 0;
    gpHoverItem = NULL;
    gpHoverItemLast = 0;
    D2WIN_10129_DrawFramedText(NULL, -1, -1, 0, 0);
}

//1.10f: D2Client.0x6FAEC3B0
//  Escape closes the stash, and only from mode 12. The message is consumed first and the chat-box guard tested after, as in
//  INV_CubeOnDismissKey and INV_TradeOnDismissKey: with the chat box open Escape is swallowed here and the stash stays open.
//  Command 0x4F word 18 is the close (the cube's close is word 23 and its transmute 24). Unlike the cube's dismiss handler, the ten-global
//  hover blank here is not guarded by the inventory being shut.
void __stdcall INV_StashOnDismissKey(SMSGHANDLER_PARAMS *pMsg)
{
    if (sgInventoryMode != 12)
    {
        return;
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    if (UI_GetUiWindowStateRaw(UI_CHATBOX))
    {
        return;
    }

    UI_SetUIToggle(UI_STASH, UI_TURNOFF, 0);
    MSG_SendCmd3Words(79, 18, 0, 0);
    gbShowItemDesc = 0;
    gnDescPopupX = -1;
    gnDescPopupY = -1;
    gnDescPopupAltX = -1;
    gnDescPopupBottomY = -1;
    gnHoverCellX = -1;
    gnHoverCellY = -1;
    gbShowSetItemDesc = 0;
    gpHoverItem = NULL;
    gpHoverItemLast = 0;
}

//1.10f: D2Client.0x6FAEBAD0
//  Left button pressed on the stash. GAME_IsExpansionGame() is called seven times: once for the withdraw-gold box at the top, four times for
//  the close button (twice for its left edge and twice for its bottom, recomputing the same expression each time), once for the stash grid
//  and once for the stash rect. They are written as seven calls.
//  BUG: the second withdraw-gold test can never pass. The withdraw box is tested at the very top and a miss is what jumps into the body; deep
//  inside, after the stash grid has also been missed, the same box is tested again with the cursor already known to be outside it, so the
//  gbWithdrawGoldBtnPressed = 1 after it never runs. UNREACHABLE: both tests are written out.
//  The 800x600 fall-through has the same dead condition as INV_CubeOnLButtonDown's: UiShift == PANEL_BOTH && !sgInventoryMode && x < w/2
//  never holds because sgInventoryMode is 12 or 13 here (the polarity is inverted because this one jumps past the consume where the
//  cube's falls through to it).
void __stdcall INV_StashOnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    int32_t nBtnLeft;
    int32_t nBtnLeft2;
    int32_t nBtnBottom;
    int32_t nBtnBottom2;
    D2UnitStrc *pPlayer;

    if (sgInventoryMode != 12 && sgInventoryMode != 13)
    {
        return;
    }

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    gnUiMouseX = nXpos;
    gnUiMouseY = nYpos;

    //  the panel's own close button, shared coordinates with trade and cube
    if ((!PANEL_InvHintPhaseSuppressed() || !PANEL_BeltHitTestPanel(pMsg)) && nXpos >= gnScreenWidth - gnBorderXDifference - 302 && nXpos <= gnScreenWidth - gnBorderXDifference - 270 && nYpos >= gnBorderYDifference + gnScreenHeight - 96
        && nYpos <= gnBorderYDifference + gnScreenHeight - 64)
    {
        gbInvCloseBtnArmed = 1;
        goto CONSUME_WITH_SOUND;
    }

    //  the withdraw-gold box: the first of its two tests, and the only live one
    if (GAME_IsExpansionGame())
    {
        if (nYpos >= gnBorderYDifference + gnScreenHeight - 455 && nYpos <= gnBorderYDifference + gnScreenHeight - 438 && nXpos >= gnBorderXDifference + 73 && nXpos <= gnBorderXDifference + 150)
        {
            gbWithdrawGoldBtnPressed = 1;
            goto CONSUME_WITH_SOUND;
        }
    }
    else if (nYpos >= gnBorderYDifference + gnScreenHeight - 259 && nYpos <= gnBorderYDifference + gnScreenHeight - 242 && nXpos >= gnBorderXDifference + 73 && nXpos <= gnBorderXDifference + 150)
    {
        gbWithdrawGoldBtnPressed = 1;
        goto CONSUME_WITH_SOUND;
    }

    //  the stash's own close button: four calls for one box
    nBtnLeft = GAME_IsExpansionGame() ? gnBorderXDifference + 272 : gnBorderXDifference + 275;
    if (nXpos > nBtnLeft)
    {
        nBtnLeft2 = GAME_IsExpansionGame() ? gnBorderXDifference + 272 : gnBorderXDifference + 275;
        if (nXpos < nBtnLeft2 + 40)
        {
            nBtnBottom = GAME_IsExpansionGame() ? gnBorderYDifference + gnScreenHeight - 64 : gnBorderYDifference + gnScreenHeight - 65;
            if (nYpos > nBtnBottom - 35)
            {
                nBtnBottom2 = GAME_IsExpansionGame() ? gnBorderYDifference + gnScreenHeight - 64 : gnBorderYDifference + gnScreenHeight - 65;
                if (nYpos < nBtnBottom2 + 5)
                {
                    gbStashCloseBtnArmed = 1;
                    gbStashCloseBtnPressed = 1;
                    gbPanelBtnPressActive = 1;
                    goto CONSUME_WITH_SOUND;
                }
            }
        }
    }

    if (PANEL_BeltHitTestPanel(pMsg) == 1)
    {
        gbWithdrawGoldBtnPressed = 0;
        gbTradeAcceptChecked = 0;
        gbPanelBtnPressActive = 0;
        gbStashCloseBtnPressed = 0;
        gbTradeAcceptBtnArmed = 0;
        gbStashCloseBtnArmed = 0;
        return;
    }

    if (nXpos >= gInvPanelRect.nLeft && nXpos < gInvPanelRect.nRight && nYpos >= gInvPanelRect.nTop && nYpos <= gInvPanelRect.nBottom)
    {
        goto TO_INVENTORY;
    }

    if (GAME_IsExpansionGame())
    {
        if (nXpos > gInvBigStashGrid.nGridLeft && nXpos <= gInvBigStashGrid.nGridRight && nYpos >= gInvBigStashGrid.nGridTop && nYpos <= gInvBigStashGrid.nGridBottom)
        {
            goto TO_INVENTORY;
        }
    }
    else if (nXpos > gInvStashGrid.nGridLeft && nXpos <= gInvStashGrid.nGridRight && nYpos >= gInvStashGrid.nGridTop && nYpos <= gInvStashGrid.nGridBottom)
    {
        goto TO_INVENTORY;
    }

    //  the gold drop button, which the trade and cube panels also have
    pPlayer = CUNIT_GetPlayer();
    if (!D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory) && nYpos >= gnBorderYDifference + gnScreenHeight - 87 && nYpos <= gnBorderYDifference + gnScreenHeight - 69 && nXpos >= gnScreenWidth - gnBorderXDifference - 237
        && nXpos <= gnScreenWidth - gnBorderXDifference - 217)
    {
        gbDropGoldBtnPressed = 1;
        gbTradeAcceptBtnArmed = 0;
        gbTradeAcceptChecked = 0;
        gbPanelBtnPressActive = 0;
        gbStashCloseBtnPressed = 0;
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
        STORM_SMsgBreakHandlerChain(pMsg);
        goto DISARM;
    }

    //  the second withdraw-gold test: the cursor is already known to be outside this box, so the arming below is unreachable
    if (GAME_IsExpansionGame())
    {
        if (nYpos >= gnBorderYDifference + gnScreenHeight - 455 && nYpos <= gnBorderYDifference + gnScreenHeight - 438 && nXpos >= gnBorderXDifference + 73 && nXpos <= gnBorderXDifference + 150)
        {
            goto ARM_WITHDRAW_UNREACHABLE;
        }
    }
    else if (nYpos >= gnBorderYDifference + gnScreenHeight - 259 && nYpos <= gnBorderYDifference + gnScreenHeight - 242 && nXpos >= gnBorderXDifference + 73 && nXpos <= gnBorderXDifference + 150)
    {
        goto ARM_WITHDRAW_UNREACHABLE;
    }

    //  the whole stash rect; the top bound is 0, not the rect's top
    if (GAME_IsExpansionGame())
    {
        if (nXpos >= gInvBigStashRect.nLeft && nXpos < gInvBigStashRect.nRight && nYpos >= 0 && nYpos <= gInvBigStashRect.nBottom)
        {
            goto SWALLOW_IN_PANEL;
        }
    }
    else if (nXpos >= gInvStashRect.nLeft && nXpos < gInvStashRect.nRight && nYpos >= 0 && nYpos <= gInvStashRect.nBottom)
    {
        goto SWALLOW_IN_PANEL;
    }

    //  the 800x600 last chance
    if (D2GFX_GetResolutionMode() != D2GAMERES_800x600 || PANEL_BeltHitTestPanel(pMsg) || nYpos > gnScreenHeight - 48 || (SCROLL_GetUiShiftScreen() == PANEL_RIGHT && nXpos < gnScreenWidth / 2))
    {
        goto DISARM;
    }
    if (CURSOR_GetCursorState() == 6)
    {
        UI_CursorSendUseHeldItem();
    }
    //  sgInventoryMode is 12 or 13 here, so this cannot hold
    if (SCROLL_GetUiShiftScreen() == PANEL_BOTH && !sgInventoryMode && nXpos < gnScreenWidth / 2)
    {
        goto DISARM;
    }
    pMsg->bReturn = 1;
    gbShowItemDesc = 0;
    gbShowSetItemDesc = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
    goto DISARM;

TO_INVENTORY:
    INV_OnLButtonDown(pMsg);
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
    goto DISARM;

SWALLOW_IN_PANEL:
    gbWithdrawGoldBtnPressed = 0;
    gbTradeAcceptBtnArmed = 0;
    gbTradeAcceptChecked = 0;
    gbPanelBtnPressActive = 0;
    gbStashCloseBtnPressed = 0;
    gbStashCloseBtnArmed = 0;
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
    return;

ARM_WITHDRAW_UNREACHABLE:
    gbWithdrawGoldBtnPressed = 1;
    gbTradeAcceptBtnArmed = 0;
    gbTradeAcceptChecked = 0;
    gbPanelBtnPressActive = 0;
    gbStashCloseBtnPressed = 0;
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
    STORM_SMsgBreakHandlerChain(pMsg);
    gbStashCloseBtnArmed = 0;
    return;

DISARM:
    gbTradeAcceptBtnArmed = 0;
    gbStashCloseBtnArmed = 0;
    return;

CONSUME_WITH_SOUND:
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAEC030
//  Left button released on the stash. The gold dialog opens in two modes from here: UI_OpenGoldEntryDialog(3) for the drop box
//  (right-anchored, and only when PANEL_InvHintPhaseSuppressed() is false) and (4) for the withdraw box (left-anchored); the cube uses (1) and
//  the trade window (2). The withdraw arm tests !gbGoldDialogOpen once around the call, where the drop arm carries the same test as one
//  more && in a chain of six.
//  The close button's four GAME_IsExpansionGame() calls are repeated as in the press handler, including recomputing both edges twice.
//  The panel close button returns without consuming, as the trade and cube releases do; it is guarded by an empty cursor and by
//  PANEL_InvHintPhaseSuppressed().
void __stdcall INV_StashOnLButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    int32_t nBtnLeft;
    int32_t nBtnLeft2;
    int32_t nBtnBottom;
    int32_t nBtnBottom2;
    D2InventoryStrc *pInventory;
    D2UnitStrc *pPlayer;

    if (sgInventoryMode != 12 && sgInventoryMode != 13)
    {
        gbPanelBtnPressActive = 0;
        return;
    }

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    gbInvCloseBtnArmed = 0;
    gbInvCloseBtnArmedUnread = 0;
    gnUiMouseX = nXpos;
    gnUiMouseY = nYpos;

    pInventory = CUNIT_GetPlayer()->pInventory;
    if ((!PANEL_InvHintPhaseSuppressed() || !PANEL_BeltHitTestPanel(pMsg)) && !D2COMMON_INVENTORY_GetCursorItem(pInventory) && nXpos >= gnScreenWidth - gnBorderXDifference - 302 && nXpos <= gnScreenWidth - gnBorderXDifference - 270
        && nYpos >= gnBorderYDifference + gnScreenHeight - 96 && nYpos <= gnBorderYDifference + gnScreenHeight - 64)
    {
        //  no bReturn written on this path
        UI_SetUIToggle(UI_STASH, UI_TURNOFF, 0);
        return;
    }

    nBtnLeft = GAME_IsExpansionGame() ? gnBorderXDifference + 272 : gnBorderXDifference + 275;
    if (nXpos > nBtnLeft)
    {
        nBtnLeft2 = GAME_IsExpansionGame() ? gnBorderXDifference + 272 : gnBorderXDifference + 275;
        if (nXpos < nBtnLeft2 + 40)
        {
            nBtnBottom = GAME_IsExpansionGame() ? gnBorderYDifference + gnScreenHeight - 64 : gnBorderYDifference + gnScreenHeight - 65;
            if (nYpos > nBtnBottom - 35)
            {
                nBtnBottom2 = GAME_IsExpansionGame() ? gnBorderYDifference + gnScreenHeight - 64 : gnBorderYDifference + gnScreenHeight - 65;
                if (nYpos < nBtnBottom2 + 5)
                {
                    if (gbStashCloseBtnPressed)
                    {
                        gbStashCloseBtnPressed = 0;
                        UI_SetUIToggle(UI_STASH, UI_TURNOFF, 0);
                        MSG_SendCmd3Words(79, 18, 0, 0);
                    }
                    gbStashCloseBtnArmed = 0;
                    gbPanelBtnPressActive = 0;
                    pMsg->bReturn = 1;
                    pMsg->lResult = 0;
                    STORM_SMsgBreakHandlerChain(pMsg);
                    return;
                }
            }
        }
    }

    gbStashCloseBtnPressed = 0;
    gbStashCloseBtnArmed = 0;

    if (!PANEL_InvHintPhaseSuppressed())
    {
        pPlayer = CUNIT_GetPlayer();
        if (!D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory) && nYpos >= gnBorderYDifference + gnScreenHeight - 87 && nYpos <= gnBorderYDifference + gnScreenHeight - 69 && nXpos >= gnScreenWidth - gnBorderXDifference - 237
            && nXpos <= gnScreenWidth - gnBorderXDifference - 217 && !gbGoldDialogOpen)
        {
            UI_OpenGoldEntryDialog(3);
        }
    }

    //  the withdraw box: mode 4 of the same dialog
    if (GAME_IsExpansionGame())
    {
        if (nYpos >= gnBorderYDifference + gnScreenHeight - 455 && nYpos <= gnBorderYDifference + gnScreenHeight - 438 && nXpos >= gnBorderXDifference + 73 && nXpos <= gnBorderXDifference + 150)
        {
            if (!gbGoldDialogOpen)
            {
                UI_OpenGoldEntryDialog(4);
            }
        }
    }
    else if (nYpos >= gnBorderYDifference + gnScreenHeight - 259 && nYpos <= gnBorderYDifference + gnScreenHeight - 242 && nXpos >= gnBorderXDifference + 73 && nXpos <= gnBorderXDifference + 150)
    {
        if (!gbGoldDialogOpen)
        {
            UI_OpenGoldEntryDialog(4);
        }
    }

    if (PANEL_BeltHitTestPanel(pMsg) == 1)
    {
        gbTradeAcceptBtnArmed = 0;
        gbStashCloseBtnArmed = 0;
        return;
    }

    gbWithdrawGoldBtnPressed = 0;
    gbDropGoldBtnPressed = 0;

    if (!gbPanelBtnPressActive)
    {
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        gbPanelBtnPressActive = 0;
        gbStashCloseBtnPressed = 0;
        gbTradeAcceptChecked = 0;
        return;
    }

    gbPanelBtnPressActive = 0;
    if (gbTradeAcceptBtnArmed)
    {
        if (gbTradeAcceptChecked == 1)
        {
            gbTradeAcceptChecked = 0;
        }
    }
    //  both flags were cleared above, so this arm cannot run either
    else if (gbStashCloseBtnArmed && gbStashCloseBtnPressed)
    {
        gbStashCloseBtnPressed = 0;
    }
    gbTradeAcceptBtnArmed = 0;
    gbStashCloseBtnArmed = 0;
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//  The NPC shop panel's handlers.
//  gNpcShopTabs is an array of four 20-byte records (bEnabled, bPressed, nHoverTextId, nInventoryMode, nBaseFrame, nCursorFrame), which is
//  why every loop over it steps five ints; each of the five tab setters at the end of this file writes all six fields of one record.
//  A second inv.cpp assert, FOG_DisplayAssert("sgbNPCMaxPages < MAX_INV_PAGES", <inv.cpp>, 8870), is pushed at 0x6FAEAB18 in
//  INV_NpcShopOnLButtonUp, so the original file is at least 8870 lines long.

//  The second path literal this unit pushes (0x6FB7974C) and the assert's own text (0x6FB7999C).
static const char szInvCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UI\\inv.cpp";
static const char szNpcMaxPagesAssert[] = "sgbNPCMaxPages < MAX_INV_PAGES";

//1.10f: D2Client.0x6FAE9FC0
//  Escape leaves the shop. Five modes accept it: 1, 2, 3, 4 and 9 (no other handler of this family mentions 9; what it is is not established).
//  The chat-box guard is folded into the same condition as the mode test, so unlike the cube's and the stash's dismiss handlers this one does
//  not consume the message when the chat box is open: it does nothing and the key goes on down the chain.
//  The held item is re-read from the inventory and put back on the cursor before anything else, which drops a half-finished transaction.
void __stdcall INV_NpcShopOnDismissKey(SMSGHANDLER_PARAMS *pMsg)
{
    D2UnitStrc *pPlayer;

    if (sgInventoryMode != 1 && sgInventoryMode != 2 && sgInventoryMode != 3 && sgInventoryMode != 4 && sgInventoryMode != 9)
    {
        return;
    }
    //  and with the chat box open this handler does nothing at all
    if (UI_GetUiWindowStateRaw(UI_CHATBOX))
    {
        return;
    }

    gbInvNpcLayoutBuilt = 0;
    pPlayer = CUNIT_GetPlayer();
    CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory));

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    gbPanelBtnPressActive = 0;
    NPCMENU_NpcShopCloseAndEndInteraction();
    gnUiMouseX = 0;
    gnUiMouseY = 0;
    gbShowItemDesc = 0;
    gbShowSetItemDesc = 0;
    gpHoverItem = NULL;
    gpHoverItemLast = 0;
    gnDescPopupX = -1;
    gnDescPopupY = -1;
    gnDescPopupAltX = -1;
    gnDescPopupBottomY = -1;
    gnHoverCellX = -1;
    gnHoverCellY = -1;
}

//1.10f: D2Client.0x6FAEA080
//  Cursor moved over the shop. The mode test is five equalities, modes 0, 1, 3, 2 and 4 in that order.
//  The hover is suppressed while an item is on the cursor: CURSOR_GetHeldUnit() non-null with the cursor inside the shop rect skips the whole
//  body and writes nothing, so dragging an item across the vendor's stock leaves the description alone.
void __stdcall INV_NpcShopOnMouseMove(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nX;
    int32_t nY;
    D2UnitGUID nNpcGUID;
    D2UnitStrc *pNpc;
    D2InventoryStrc *pInventory;

    if (sgInventoryMode != 0 && sgInventoryMode != 1 && sgInventoryMode != 3 && sgInventoryMode != 2 && sgInventoryMode != 4)
    {
        return;
    }

    nX = pMsg->nXpos;
    nY = pMsg->nYpos;
    gnUiMouseX = nX;
    gnUiMouseY = nY;

    if (CURSOR_GetHeldUnit() && nX >= gInvNpcShopRect.nLeft && nX <= gInvNpcShopRect.nRight && nY >= gInvNpcShopRect.nTop && nY <= gInvNpcShopRect.nBottom)
    {
        //  nothing at all, not even a bReturn
        return;
    }

    if (!sgInventoryMode || !NPCMENU_GetInteractingNpc(&nNpcGUID))
    {
        INV_OnMouseMove(pMsg);
        return;
    }
    pNpc = CUNIT_FindUnitByGuidAndType(nNpcGUID, UNIT_MONSTER);
    if (!pNpc)
    {
        INV_OnMouseMove(pMsg);
        return;
    }

    pInventory = pNpc->pInventory;

    if (nX < gInvNpcShopRect.nLeft || nX > gInvNpcShopRect.nRight || nY < gInvNpcShopRect.nTop || nY > gInvNpcShopRect.nBottom)
    {
        INV_OnMouseMove(pMsg);
        return;
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;

    if (nX < gInvNpcShopGrid.nGridLeft || nX > gInvNpcShopGrid.nGridRight || nY < gInvNpcShopGrid.nGridTop || nY > gInvNpcShopGrid.nGridBottom)
    {
        INV_OnMouseMove(pMsg);
        return;
    }

    INV_PixelToGridCell(pNpc, pInventory, NULL, nX, nY, &gInvNpcShopGrid, (D2C_ItemInvPage)gnVendorPage);
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    gnNpcTradeMode = 0;
}

//1.10f: D2Client.0x6FAEA1C0
//  Left button pressed on the shop panel: four jobs. The panel close button (right-anchored, shared with three panels); the four vendor tabs
//  (a strip 320 wide at the top of the panel); the shop's own tab buttons (from gatButtonsOffsetX, count in gnNpcShopTabCount); and a click on
//  the stock grid, handed to INV_OnGridLeftClick.
//  The vendor tabs are four 80-pixel boxes, x from gnBorderXDifference to gnScreenWidth / 2 - 1, y h-479..h-449, split at +80, +160, +240 and
//  bounded at +320. Each tab is refused unless its gVendorTab<N>HasItems is set and it is not already the current page, (BYTE)gnVendorPage,
//  compared as a byte where the global is an int. D2ExpRes BuySellTabsClick3 and BuySellTabsClick4 detour the split points.
//  A tab-loop variable is read after the loop and assigned only inside it; gnNpcShopTabCount > 0 guards the whole block so it is always
//  assigned, and ours carries an explicit initialiser.
//  Clicking the selected tab again closes it: it is deselected, sgInventoryMode goes back to 1 and the cursor is emptied. If a different tab
//  was selected the new one is set and the old one cleared; if the tab's own record is not 1, everything is cleared and the mode resets.
void __stdcall INV_NpcShopOnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    uint8_t nInvPage;
    int32_t iTab;
    int32_t nTabHit;
    int32_t nTabPrevSelected;
    int32_t bHadSelection;
    int32_t nCloseX;
    int32_t nTabStripTop;
    D2UnitGUID nNpcGUID;
    int32_t nCost;
    D2UnitStrc *pNpc;
    D2UnitStrc *pPlayer;
    D2UnitStrc *pHeldItem;
    D2InventoryStrc *pInventory;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;

    if (sgInventoryMode != 0 && sgInventoryMode != 1 && sgInventoryMode != 3 && sgInventoryMode != 2 && sgInventoryMode != 4)
    {
        goto CONSUME;
    }

    //  the panel close button, with the mode test again as the last conjunct
    if (!PANEL_BeltHitTestPanel(pMsg) && nXpos >= gnScreenWidth - gnBorderXDifference - 302 && nXpos <= gnScreenWidth - gnBorderXDifference - 270 && nYpos >= gnBorderYDifference + gnScreenHeight - 96
        && nYpos <= gnBorderYDifference + gnScreenHeight - 64 && (sgInventoryMode == 0 || sgInventoryMode == 1 || sgInventoryMode == 3 || sgInventoryMode == 2 || sgInventoryMode == 4))
    {
        gbInvCloseBtnArmed = 1;
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
        STORM_SMsgBreakHandlerChain(pMsg);
        return;
    }

    if (CURSOR_GetHeldUnit())
    {
        if (PANEL_BeltHitTestPanel(pMsg) == 1)
        {
            ITEMPANEL_ClearShopTabs();
            goto RESET_AND_TAKE;
        }

        if (nXpos < 0 || nXpos > gInvPanelRect.nLeft || nYpos < 0 || nYpos > gInvPanelRect.nBottom)
        {
            INV_OnLButtonDown(pMsg);
            return;
        }

        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);

        if (nXpos >= gInvNpcShopGrid.nGridLeft && nXpos <= gInvNpcShopGrid.nGridRight && nYpos >= gInvNpcShopGrid.nGridTop && nYpos <= gInvNpcShopGrid.nGridBottom && CURSOR_GetCursorState() != 6 && NPCMENU_GetInteractingNpc(&nNpcGUID))
        {
            pNpc = CUNIT_FindUnitByGuidAndType(nNpcGUID, UNIT_MONSTER);
            if (pNpc)
            {
                //  the original calls Unicode's default constructor sixty-four times, once per element (an inlined `Unicode wszOut[64];`)
                Unicode wszOut[64];
                if (NPCMENU_TransactionCost(CURSOR_GetHeldUnit(), 1, &nCost, wszOut, 64))
                {
                    pHeldItem = CURSOR_GetHeldUnit();
                    if (pHeldItem && NPCMENU_BeginTransaction(pNpc, pHeldItem, 1, nXpos, nYpos, pMsg->wParam, 1, 0))
                    {
                        CURSOR_SetHeldUnit(NULL);
                    }
                }
            }
        }
        return;
    }

    if (PANEL_BeltHitTestPanel(pMsg) == 1)
    {
        ITEMPANEL_ClearShopTabs();
        goto RESET_AND_TAKE;
    }

    pMsg->bReturn = 0;

    //  the four vendor tabs
    if (nXpos >= gnBorderXDifference && nXpos <= gnScreenWidth / 2 - 1 && nYpos >= gnScreenHeight + gnBorderYDifference - 479 && nYpos <= gnScreenHeight + gnBorderYDifference - 449)
    {
        if (nXpos <= gnBorderXDifference + 80)
        {
            if (!gVendorTabs[0].bActive || !gnVendorPage)
            {
                goto CONSUME;
            }
            SOUNDINST_PlaySound(SOUND_CURSOR_SWITCH, NULL, 0, 0, 0);
            nInvPage = 0;
        }
        //  D2ExpRes: BuySellTabsClick3 [BuySell] detours at the `lea eax, [ecx+0F0h]` this +240 compiles to.
        else if (nXpos <= gnBorderXDifference + 160)
        {
            if (!gVendorTabs[1].bActive || gnVendorPage == 1)
            {
                goto CONSUME;
            }
            SOUNDINST_PlaySound(SOUND_CURSOR_SWITCH, NULL, 0, 0, 0);
            nInvPage = 1;
        }
        else if (nXpos <= gnBorderXDifference + 240)
        {
            if (!gVendorTabs[2].bActive || gnVendorPage == 2)
            {
                goto CONSUME;
            }
            SOUNDINST_PlaySound(SOUND_CURSOR_SWITCH, NULL, 0, 0, 0);
            nInvPage = 2;
        }
        else
        {
            if (nXpos >= gnBorderXDifference + 320 || !gVendorTabs[3].bActive || gnVendorPage == 3)
            {
                goto CONSUME;
            }
            SOUNDINST_PlaySound(SOUND_CURSOR_SWITCH, NULL, 0, 0, 0);
            nInvPage = 3;
        }
        gnVendorPageHoldUntil = 0;
        INV_GetVendorPageInventory(nInvPage);
        goto CONSUME;
    }

    if (nXpos < gInvNpcShopRect.nLeft || nXpos > gInvNpcShopRect.nRight)
    {
        INV_OnLButtonDown(pMsg);
        return;
    }

    nTabStripTop = gnScreenHeight + gnBorderYDifference - 109;

    //  the shop's own tab buttons
    if (nYpos >= nTabStripTop && nYpos <= gInvNpcShopRect.nBottom)
    {
        nTabPrevSelected = 0;
        bHadSelection = 0;
        nTabHit = -1;
        if (gnNpcShopTabCount > 0)
        {
            for (iTab = 0; iTab < gnNpcShopTabCount; ++iTab)
            {
                if (gNpcShopTabs[iTab].bPressed == 1)
                {
                    nTabPrevSelected = iTab;
                    bHadSelection = 1;
                }
                nCloseX = gnBorderXDifference + gatButtonsOffsetX[gnNpcShopCloseTab][iTab];
                if (gnUiMouseX > nCloseX && gnUiMouseX < nCloseX + 45 && gnUiMouseY > nTabStripTop && gnUiMouseY < gnBorderYDifference + gnScreenHeight - 65)
                {
                    nTabHit = iTab;
                }
            }

            if (nTabHit != -1)
            {
                if (gNpcShopTabs[nTabHit].bEnabled == 1)
                {
                    SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
                    if (!bHadSelection)
                    {
                        gNpcShopTabs[nTabHit].bPressed = 1;
                        gbPanelBtnPressActive = 1;
                    }
                    else if (nTabHit == nTabPrevSelected)
                    {
                        //  clicking the open tab again closes it
                        gNpcShopTabs[nTabHit].bPressed = 0;
                        sgInventoryMode = 1;
                        if (CUNIT_GetPlayer())
                        {
                            pInventory = CUNIT_GetPlayer()->pInventory;
                            if (pInventory && !D2COMMON_INVENTORY_GetCursorItem(pInventory))
                            {
                                CURSOR_SetHeldUnit(NULL);
                            }
                        }
                        gbPanelBtnPressActive = 1;
                    }
                    else
                    {
                        gNpcShopTabs[nTabHit].bPressed = 1;
                        gbPanelBtnPressActive = 1;
                        gNpcShopTabs[nTabPrevSelected].bPressed = 0;
                    }
                }
                else
                {
                    ITEMPANEL_ClearShopTabs();
                    sgInventoryMode = 1;
                    pPlayer = CUNIT_GetPlayer();
                    CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory));
                    gbPanelBtnPressActive = 0;
                }
            }
        }
        goto CONSUME;
    }

    if (nYpos < gInvNpcShopRect.nTop || nYpos > gInvNpcShopRect.nBottom)
    {
        INV_OnLButtonDown(pMsg);
        return;
    }

    //  a click on the vendor's stock
    if ((sgInventoryMode == 1 || sgInventoryMode == 2) && nXpos >= gInvNpcShopGrid.nGridLeft && nXpos <= gInvNpcShopGrid.nGridRight && nYpos >= gInvNpcShopGrid.nGridTop && nYpos <= gInvNpcShopGrid.nGridBottom && NPCMENU_GetInteractingNpc(&nNpcGUID))
    {
        pNpc = CUNIT_FindUnitByGuidAndType(nNpcGUID, UNIT_MONSTER);
        if (pNpc)
        {
            INV_OnGridLeftClick(pNpc, pNpc->pInventory, nXpos, nYpos, (uint8_t)pMsg->wParam, &gInvNpcShopGrid, (D2C_ItemInvPage)gnVendorPage);
        }
    }

CONSUME:
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
    return;

RESET_AND_TAKE:
    sgInventoryMode = 1;
    pPlayer = CUNIT_GetPlayer();
    CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory));
}

//1.10f: D2Client.0x6FAEA850
//  Left button released on the shop panel, where a tab's action runs.
//  An item on the cursor (CURSOR_GetHeldUnit() non-null) writes bReturn = 0 and returns, so no tab and no close button reacts while
//  something is held.
//  The tab record is 20 bytes. The original walks it with a cursor that starts ten bytes into the record (the mode) and reads -10 (enabled),
//  -6 (selected), +4 (the word at record +14) and +6 (the cursor frame), stride five ints; ours indexes gNpcShopTabs[iTab] and names each field.
//  A tab whose mode (record +10) is 9 closes the shop and is the only one that returns immediately. The others set sgInventoryMode from the
//  tab and then split on the word at record +14: 14 or 12 blank the ten hover globals and do not set the hit flag, so the tail below still
//  runs; 18 calls NPCMENU_BeginTransaction with a NULL item and gbTransactionItemIsPlayers raised around the call; anything else calls
//  CURSOR_SetCursorBuySell and records which tab was hit.
//  The close button at the end tests nXpos, which the tab block may have reassigned from the cached copy, against the cached nYpos; on every
//  path that reaches it the two are equal because the reassignment writes the same value back, and ours uses one pair.
void __stdcall INV_NpcShopOnLButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    int32_t iTab;
    int32_t nButtonX;
    int32_t nTabStripTop;
    int32_t bTabActionTaken;
    int32_t nAction;
    int32_t nCursorState;
    int16_t wSubAction;
    D2UnitStrc *pPlayer;
    D2InventoryStrc *pInventory;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    gbInvCloseBtnArmed = 0;
    gbInvCloseBtnArmedUnread = 0;

    if (CURSOR_GetHeldUnit())
    {
        pMsg->bReturn = 0;
        return;
    }

    if (gbPanelBtnPressActive == 1)
    {
        bTabActionTaken = 0;
        nTabStripTop = gnBorderYDifference + gnScreenHeight - 109;

        if (nXpos >= gInvNpcShopRect.nLeft && nXpos <= gInvNpcShopRect.nRight && nYpos >= nTabStripTop && nYpos <= gInvNpcShopRect.nBottom && gnNpcShopTabCount > 0)
        {
            for (iTab = 0; iTab < gnNpcShopTabCount; ++iTab)
            {
                //  the original's cursor starts ten bytes into the record (+0Ah, the mode) and reads the rest at -10, -6, +4 and +6;
                //  ours reads the record by its fields
                if (gNpcShopTabs[iTab].bEnabled != 1)
                {
                    continue;
                }
                nButtonX = gnBorderXDifference + gatButtonsOffsetX[gnNpcShopCloseTab][iTab];
                if (gnUiMouseX <= nButtonX || gnUiMouseX >= nButtonX + 45 || gnUiMouseY <= nTabStripTop || gnUiMouseY >= gnBorderYDifference + gnScreenHeight - 65 || gNpcShopTabs[iTab].bPressed != 1)
                {
                    continue;
                }

                nAction = gNpcShopTabs[iTab].nInventoryMode;
                if (nAction == 9)
                {
                    pMsg->bReturn = 1;
                    pMsg->lResult = 0;
                    STORM_SMsgBreakHandlerChain(pMsg);
                    gbPanelBtnPressActive = 0;
                    pPlayer = CUNIT_GetPlayer();
                    if (!D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory))
                    {
                        CURSOR_SetHeldUnit(NULL);
                    }
                    NPCMENU_NpcShopCloseAndEndInteraction();
                    gnUiMouseX = 0;
                    gnUiMouseY = 0;
                    return;
                }

                sgInventoryMode = nAction;
                if (gnVendorMaxPage >= 5u)
                {
                    FOG_DisplayAssert(szNpcMaxPagesAssert, szInvCpp, 8870);
                    exit(-1);
                }

                wSubAction = (int16_t)gNpcShopTabs[iTab].nBaseFrame;
                if (wSubAction == 14 || wSubAction == 12)
                {
                    gnDescPopupX = -1;
                    gnDescPopupY = -1;
                    gnDescPopupAltX = -1;
                    gnDescPopupBottomY = -1;
                    gnHoverCellX = -1;
                    gnHoverCellY = -1;
                    gbShowItemDesc = 0;
                    gbShowSetItemDesc = 0;
                    gpHoverItem = NULL;
                    gpHoverItemLast = 0;
                    //  and the hit flag stays clear, so the tail below runs
                    bTabActionTaken = 0;
                }
                else
                {
                    bTabActionTaken = 1;
                    if (wSubAction == 18)
                    {
                        gbTransactionItemIsPlayers = 1;
                        pPlayer = CUNIT_GetPlayer();
                        NPCMENU_BeginTransaction(pPlayer, NULL, 1, nXpos, nYpos, 0, 1, 1);
                        gbTransactionItemIsPlayers = 0;
                    }
                    else
                    {
                        //  record +16, the cursor frame
                        CURSOR_SetCursorBuySell(gNpcShopTabs[iTab].nCursorFrame, 0);
                        gnNpcShopTabHit = iTab;
                        gnNpcShopTabAction = (uint16_t)wSubAction;
                    }
                }
            }

            if (bTabActionTaken)
            {
                goto CONSUME_AND_CLEAR;
            }
        }

        if (sgInventoryMode == 1 || sgInventoryMode == 2 || sgInventoryMode == 4 || sgInventoryMode == 3)
        {
            pPlayer = CUNIT_GetPlayer();
            if (!D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory))
            {
                nCursorState = CURSOR_GetCursorState();
                if (nCursorState == 7 || nCursorState == 1 || nCursorState == 4)
                {
                    pPlayer = CUNIT_GetPlayer();
                    CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory));
                }
            }
        }

        //  the original counts down over the tab count here instead of using the end-address sentinel of the other copies
        for (iTab = 0; iTab < gnNpcShopTabCount; ++iTab)
        {
            if (gNpcShopTabs[iTab].bPressed == 1)
            {
                gNpcShopTabs[iTab].bPressed = 0;
            }
        }

    CONSUME_AND_CLEAR:
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        gbPanelBtnPressActive = 0;
    }

    pInventory = CUNIT_GetPlayer()->pInventory;
    if (!PANEL_BeltHitTestPanel(pMsg) && !D2COMMON_INVENTORY_GetCursorItem(pInventory) && nXpos >= gnScreenWidth - gnBorderXDifference - 302 && nXpos <= gnScreenWidth - gnBorderXDifference - 270 && nYpos >= gnBorderYDifference + gnScreenHeight - 96
        && nYpos <= gnBorderYDifference + gnScreenHeight - 64)
    {
        pPlayer = CUNIT_GetPlayer();
        CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory));
        NPCMENU_NpcShopCloseAndEndInteraction();
    }
}

//  The inventory panel proper (gInv): four handlers for the grid, the eleven equipment slots and the weapon-swap tab; every other item panel
//  in this file delegates to them.
//  gInvBodyLocRects is D2InvCompGridStrc[11]; the array index is the D2C_PlayerBodyLocs value (BODYLOC_NONE..BODYLOC_GLOVES), so gInvRArmRect
//  and gInvLArmRect are elements [4] and [5] (BODYLOC_RARM and BODYLOC_LARM). The walk steps 20 bytes from &gInvBodyLocRects[0].rect.nRight,
//  four bytes in, and stops at >= gMercInvSlots, so it runs eleven times; the array ends at 0x6FBB5A33 and four unreferenced bytes
//  (0x6FBB5A34..0x6FBB5A37) lie between it and gMercInvSlots. The bound is the array that comes next: one more end-address sentinel that
//  names another symbol.

//1.10f: D2Client.0x6FAE8140
//  Left button released on the inventory. On the path where the close button was missed, INVENTORY_GetCursorItem(pInventory) is called and
//  its answer thrown away (the call is in the bytes and is reproduced). gbDropGoldBtnPressed is tested, and cleared, only inside
//  if (!sgInventoryMode); in any other mode the flag survives the release. The close button returns without consuming, as the trade, cube
//  and stash releases do.
void __stdcall INV_OnLButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    D2InventoryStrc *pInventory;
    D2UnitStrc *pPlayer;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    gbInvCloseBtnArmed = 0;
    gbInvCloseBtnArmedUnread = 0;
    pInventory = CUNIT_GetPlayer()->pInventory;

    if (!PANEL_BeltHitTestPanel(pMsg))
    {
        if (nXpos >= gnScreenWidth - gnBorderXDifference - 302 && nXpos <= gnScreenWidth - gnBorderXDifference - 270 && nYpos >= gnBorderYDifference + gnScreenHeight - 96 && nYpos <= gnBorderYDifference + gnScreenHeight - 64)
        {
            //  no bReturn written on this path
            UI_SetUIToggle(UI_INVENTORY, UI_TURNOFF, 0);
            return;
        }
        //  the answer is thrown away
        D2COMMON_INVENTORY_GetCursorItem(pInventory);
    }

    if (gbDropGoldBtnPressed && !sgInventoryMode)
    {
        gbDropGoldBtnPressed = 0;
        pPlayer = CUNIT_GetPlayer();
        if (!D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory) && nYpos >= gnBorderYDifference + gnScreenHeight - 87 && nYpos <= gnBorderYDifference + gnScreenHeight - 69 && nXpos >= gnScreenWidth - gnBorderXDifference - 237
            && nXpos <= gnScreenWidth - gnBorderXDifference - 217 && !gbGoldDialogOpen)
        {
            UI_OpenGoldEntryDialog(1);
        }
    }

    GAME_ResetMouseButtonState();

    if (nXpos >= gInvPanelRect.nLeft && nXpos < gInvPanelRect.nRight && nYpos >= gInvPanelRect.nTop && nYpos <= gInvPanelRect.nBottom && !PANEL_BeltHitTestPanel(pMsg))
    {
        pMsg->lResult = 0;
        pMsg->bReturn = 1;
        STORM_SMsgBreakHandlerChain(pMsg);
    }
}

//1.10f: D2Client.0x6FAE8AA0
//  Right button pressed on the inventory: uses an item where it lies. SMsgBreakHandlerChain sits inside if (sgInventoryMode), so in mode 0
//  (the plain inventory) the cursor-state work runs and the chain is not broken, even though bReturn was set to 1 above; the two arms are
//  otherwise the same three lines. A successful INV_UseItemAtGridCell in a shop mode clears every shop tab, resets sgInventoryMode to 1 and
//  puts the cursor item back, so using a potion while a vendor is open closes the vendor's tab.
void __stdcall INV_OnRButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nMouseX;
    int32_t nMouseY;
    D2UnitStrc *pPlayer;
    D2InventoryStrc *pInventory;

    if (GAME_IsDisconnecting() || CUNIT_IsPlayerDeadOrMissing())
    {
        pMsg->bReturn = 0;
        return;
    }

    nMouseX = pMsg->nXpos;
    nMouseY = pMsg->nYpos;
    pPlayer = CUNIT_GetPlayer();
    INV_EnsurePanelLayout();
    pInventory = pPlayer->pInventory;

    if (PANEL_BeltHitTestPanel(pMsg) || nMouseX < gInvPanelRect.nLeft || nMouseX >= gInvPanelRect.nRight || nMouseY < gInvPanelRect.nTop || nMouseY > gInvPanelRect.nBottom)
    {
        return;
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;

    if (nMouseX < gInvPanelGrid.nGridLeft || nMouseX > gInvPanelGrid.nGridRight || nMouseY < gInvPanelGrid.nGridTop || nMouseY > gInvPanelGrid.nGridBottom)
    {
        pMsg->bReturn = 0;
    }
    else if (INV_UseItemAtGridCell(pPlayer, pInventory, nMouseX, nMouseY, INVPAGE_INVENTORY))
    {
        if (sgInventoryMode >= 2 && sgInventoryMode <= 4)
        {
            ITEMPANEL_ClearShopTabs();
            goto RESET_MODE;
        }
        goto BREAK_CHAIN;
    }

    if (!sgInventoryMode)
    {
        //  the chain is not broken on this arm
        if (CURSOR_GetCursorState() == 6)
        {
            UI_CursorSendUseHeldItem();
        }
        else if (CURSOR_GetCursorState() == 8)
        {
            MSG_SendServerItemMessage(76, -1);
        }
        return;
    }

    if (CURSOR_GetCursorState() == 6)
    {
        UI_CursorSendUseHeldItem();
        goto BREAK_CHAIN;
    }
    if (CURSOR_GetCursorState() == 8)
    {
        MSG_SendServerItemMessage(76, -1);
        goto BREAK_CHAIN;
    }
    if (sgInventoryMode < 2 || sgInventoryMode > 4)
    {
        goto BREAK_CHAIN;
    }
    ITEMPANEL_ClearShopTabs();

RESET_MODE:
    sgInventoryMode = 1;
    pPlayer = CUNIT_GetPlayer();
    CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory));

BREAK_CHAIN:
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAE8290
//  Cursor moved over the inventory: the hover, for every item panel.
//  Each of the four out-of-panel arms saves the low byte of gnVendorPage, overwrites it with a page number (1 to 4), calls INV_PixelToGridCell
//  and restores the saved byte. Only the low byte is ever touched (the upper three bytes of the int global are neither saved nor changed),
//  so the global is a different value for the duration of one call; this is reproduced byte for byte.
//  Outside every grid the hover state is blanked, unless UI_GetUiWindowState(UI_MERCINV) is set, in which case only bReturn is cleared and the
//  ten globals stay: the mercenary panel owns the description while it is open.
//  Two-handed rule: hovering either arm slot with INVENTORY_GetWieldType(...) == 2 shows the item in the other hand as well, unless either
//  item is a bow or a crossbow (four ITEMS_CheckItemTypeId calls decide it); when both hands are empty, sgInventoryMode == 4 alone keeps
//  the substitute. gnHoverBodyLoc is a dword.
void __stdcall INV_OnMouseMove(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nX;
    int32_t nY;
    int32_t iBodyLoc;
    int32_t nBodyLocHit;
    int32_t bHoverFound;
    int32_t bTwoHanded;
    uint8_t nVendorPageSaved;
    D2UnitStrc *pPlayer;
    D2UnitStrc *pCursorItem;
    D2UnitStrc *pItem;
    D2UnitStrc *pOtherHand;
    D2InventoryStrc *pInventory;

    if (GAME_IsDisconnecting() || CUNIT_IsPlayerDeadOrMissing() || !gbInvHoverEnabled)
    {
        pMsg->bReturn = 0;
        return;
    }

    nX = pMsg->nXpos;
    nY = pMsg->nYpos;
    pPlayer = CUNIT_GetPlayer();
    INV_EnsurePanelLayout();
    pInventory = pPlayer->pInventory;
    bHoverFound = 0;

    if (!PANEL_BeltHitTestPanel(pMsg) && nX >= gInvPanelRect.nLeft && nX < gInvPanelRect.nRight && nY >= gInvPanelRect.nTop && nY <= gInvPanelRect.nBottom)
    {
        goto IN_PANEL;
    }

    if (sgInventoryMode == 11)
    {
        //  the low byte only
        nVendorPageSaved = gnVendorPage;
        if (nX > gInvTradePage1Grid.nGridLeft && nX < gInvTradePage1Grid.nGridRight && nY >= gInvTradePage1Grid.nGridTop && nY <= gInvTradePage1Grid.nGridBottom)
        {
            gnVendorPage = 1;
            INV_PixelToGridCell(pPlayer, pInventory, D2COMMON_INVENTORY_GetCursorItem(pInventory), nX, nY, &gInvTradePage1Grid, INVPAGE_EQUIP);
            bHoverFound = 1;
        }
        else if (nX > gInvTradePage2Grid.nGridLeft && nX < gInvTradePage2Grid.nGridRight && nY >= gInvTradePage2Grid.nGridTop && nY <= gInvTradePage2Grid.nGridBottom)
        {
            gnVendorPage = 2;
            INV_PixelToGridCell(pPlayer, pInventory, D2COMMON_INVENTORY_GetCursorItem(pInventory), nX, nY, &gInvTradePage2Grid, INVPAGE_TRADE);
            bHoverFound = 1;
        }
        gnVendorPage = nVendorPageSaved;
        if (!bHoverFound)
        {
            pMsg->bReturn = 0;
            gbShowItemDesc = 0;
            gbShowSetItemDesc = 0;
            gpHoverItem = NULL;
            gpHoverItemLast = 0;
        }
        gnNpcTradeMode = 1;
        return;
    }

    if (sgInventoryMode == 12)
    {
        nVendorPageSaved = gnVendorPage;
        if (GAME_IsExpansionGame())
        {
            if (nX <= gInvBigStashGrid.nGridLeft || nX > gInvBigStashGrid.nGridRight || nY < gInvBigStashGrid.nGridTop || nY > gInvBigStashGrid.nGridBottom)
            {
                goto BLANK_AND_RESTORE;
            }
        }
        else if (nX <= gInvStashGrid.nGridLeft || nX > gInvStashGrid.nGridRight || nY < gInvStashGrid.nGridTop || nY > gInvStashGrid.nGridBottom)
        {
            goto BLANK_AND_RESTORE;
        }
        gnVendorPage = 4;
        pCursorItem = D2COMMON_INVENTORY_GetCursorItem(pInventory);
        if (GAME_IsExpansionGame())
        {
            INV_PixelToGridCell(pPlayer, pInventory, pCursorItem, nX, nY, &gInvBigStashGrid, INVPAGE_STASH);
        }
        else
        {
            INV_PixelToGridCell(pPlayer, pInventory, pCursorItem, nX, nY, &gInvStashGrid, INVPAGE_STASH);
        }
        goto RESTORE_AND_RETURN;
    }

    if (sgInventoryMode != 14)
    {
        //  the mercenary panel keeps the description
        if (UI_GetUiWindowState(UI_MERCINV))
        {
            pMsg->bReturn = 0;
            return;
        }
        pMsg->bReturn = 0;
        gbShowItemDesc = 0;
        gbShowSetItemDesc = 0;
        gpHoverItem = NULL;
        gpHoverItemLast = 0;
        gnNpcTradeMode = 0;
        return;
    }

    nVendorPageSaved = gnVendorPage;
    if (nX >= gInvCubeGrid.nGridLeft && nX < gInvCubeGrid.nGridRight && nY >= gInvCubeGrid.nGridTop && nY <= gInvCubeGrid.nGridBottom)
    {
        gnVendorPage = 3;
        INV_PixelToGridCell(pPlayer, pInventory, D2COMMON_INVENTORY_GetCursorItem(pInventory), nX, nY, &gInvCubeGrid, INVPAGE_CUBE);
        goto RESTORE_AND_RETURN;
    }

BLANK_AND_RESTORE:
    pMsg->bReturn = 0;
    gbShowItemDesc = 0;
    gbShowSetItemDesc = 0;
    gpHoverItem = NULL;
    gpHoverItemLast = 0;

RESTORE_AND_RETURN:
    gnNpcTradeMode = 1;
    gnVendorPage = nVendorPageSaved;
    return;

IN_PANEL:
    pMsg->bReturn = 1;
    pMsg->lResult = 0;

    if (nX >= gInvPanelGrid.nGridLeft && nX <= gInvPanelGrid.nGridRight && nY >= gInvPanelGrid.nGridTop && nY <= gInvPanelGrid.nGridBottom)
    {
        INV_PixelToGridCell(pPlayer, pInventory, D2COMMON_INVENTORY_GetCursorItem(pInventory), nX, nY, &gInvPanelGrid, INVPAGE_INVENTORY);
        gnHoverBodyLoc = 0;
        gnNpcTradeMode = 1;
        return;
    }

    //  the eleven equipment rects
    nBodyLocHit = -1;
    for (iBodyLoc = 0; iBodyLoc < 11; ++iBodyLoc)
    {
        if (nX >= gInvBodyLocRects[iBodyLoc].rect.nLeft && nX <= gInvBodyLocRects[iBodyLoc].rect.nRight && nY >= gInvBodyLocRects[iBodyLoc].rect.nTop && nY <= gInvBodyLocRects[iBodyLoc].rect.nBottom)
        {
            nBodyLocHit = iBodyLoc;
            break;
        }
    }
    if (nBodyLocHit == -1)
    {
        gnHoverBodyLoc = 0;
        pMsg->bReturn = 0;
        gbShowItemDesc = 0;
        goto CLEAR_HOVER;
    }

    if (D2COMMON_INVENTORY_GetCursorItem(pInventory))
    {
        gnHoverCellX = -1;
        gnHoverCellY = -1;
        gbShowSetItemDesc = 1;
        gnHoverBodyLoc = nBodyLocHit;
        goto CLEAR_ITEM;
    }

    bTwoHanded = (D2COMMON_INVENTORY_GetWieldType(pPlayer, pInventory) == 2);
    pItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pPlayer->pInventory, (D2C_PlayerBodyLocs)nBodyLocHit);

    if (nBodyLocHit == BODYLOC_RARM || nBodyLocHit == BODYLOC_LARM)
    {
        if (bTwoHanded)
        {
            pOtherHand = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RARM);
            if (!pOtherHand)
            {
                pOtherHand = D2COMMON_INVENTORY_GetItemFromBodyLoc(pPlayer->pInventory, BODYLOC_LARM);
            }
            if (pOtherHand)
            {
                if (pItem && !D2COMMON_ITEMS_CheckItemTypeId(pOtherHand, ITEMTYPE_CROSSBOW) && !D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_CROSSBOW) && !D2COMMON_ITEMS_CheckItemTypeId(pOtherHand, ITEMTYPE_BOW)
                    && !D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_BOW))
                {
                    pItem = pOtherHand;
                }
                if (!pItem)
                {
                    if (sgInventoryMode != 4)
                    {
                        goto CLEAR_HOVER;
                    }
                    pItem = pOtherHand;
                }
            }
        }
    }

    if (!pItem)
    {
        goto CLEAR_HOVER;
    }

    gbShowSetItemDesc = 1;
    if (pItem == gpHoverItemLast)
    {
        gnNpcTradeMode = 1;
        return;
    }
    gpHoverItem = pItem;
    gpHoverItemLast = pItem;
    gnDescPopupX = (gInvBodyLocRects[nBodyLocHit].rect.nLeft + gInvBodyLocRects[nBodyLocHit].rect.nRight) / 2;
    gnDescPopupAltX = gnDescPopupX;
    gnDescPopupY = gInvBodyLocRects[nBodyLocHit].rect.nTop;
    gnDescPopupBottomY = gInvBodyLocRects[nBodyLocHit].rect.nBottom;
    gnNpcTradeMode = 1;
    return;

CLEAR_HOVER:
    gbShowSetItemDesc = 0;

CLEAR_ITEM:
    gpHoverItem = NULL;
    gpHoverItemLast = 0;
    gnNpcTradeMode = 1;
}

//1.10f: D2Client.0x6FAE6E60
//  Left button pressed on the inventory: the busiest handler of the panel tables. The original inlines INV_SwapWeaponsAndLockItems in the
//  weapon-swap strip.
//  The first test, (uint16)nXpos > gnScreenWidth, sets bReturn, breaks the chain and then falls through into the whole body with no return,
//  so a click right of the screen edge is claimed twice over.
//  The gold button works in modes 0, 11, 12 and 15 and the close button in modes 0 and 19; both are inside
//  if (!PANEL_InvHintPhaseSuppressed()), so the hint phase disables both.
//  Four panels delegate their grid click here: trade page 2, the stash (either size), the cube and the inventory grid all reach
//  INV_OnGridLeftClick, each with its own D2C_ItemInvPage. Only the trade arm uses the result: non-zero sends the green check, which is how
//  putting an item into the trade square retracts your acceptance.
//  Dropping a gem onto an equipped socketed item sends command 0x28. In a single-player game it goes out immediately; otherwise it needs
//  either TRADE_TickTimeout() (which sends the green check instead) or 250 ms (0xFA) since gnLastItemUseTick. GetTickCount() is called twice,
//  once to compare and once to store.
//  A 24-pixel strip directly above either arm slot (nTop - 24 .. nTop, exclusive at the bottom) is the weapon-swap tab; it does what
//  INV_OnKeySwapWeapons does for 'W'. In classic it consumes the click and does nothing, because bReturn = 1 is written before the expansion test.
//  The 800x600 fall-through is reached from the very end: if (!bInGrid) the code jumps back to the block above it. It is the block the cube
//  and the stash have, with the same sgInventoryMode conjunct, but here the mode can be 0, so this is the one copy where that conjunct is live.
void __stdcall INV_OnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    int32_t iBodyLoc;
    int32_t nBodyLocHit;
    int32_t bInGrid;
    int32_t nHandled;
    int32_t nGridLeft;
    int32_t nGridRight;
    int32_t nGridTop;
    int32_t nGridBottom;
    int32_t nCursorItemId;
    int32_t nSlotItemId;
    D2C_Sounds nDropSound;
    D2UnitStrc *pPlayer;
    D2UnitStrc *pCursorItem;
    D2UnitStrc *pSlotItem;
    D2UnitStrc *pItem;
    D2InventoryStrc *pInventory;

    if (GAME_IsDisconnecting() || CUNIT_IsPlayerDeadOrMissing())
    {
        pMsg->bReturn = 0;
        return;
    }

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;

    //  claimed here, and the function keeps going
    if (nXpos > gnScreenWidth)
    {
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
    }

    pPlayer = CUNIT_GetPlayer();
    INV_EnsurePanelLayout();
    bInGrid = 0;
    pInventory = pPlayer->pInventory;

    if (!PANEL_InvHintPhaseSuppressed())
    {
        if (!D2COMMON_INVENTORY_GetCursorItem(CUNIT_GetPlayer()->pInventory) && nYpos >= gnBorderYDifference + gnScreenHeight - 87 && nYpos <= gnBorderYDifference + gnScreenHeight - 69 && nXpos >= gnScreenWidth - gnBorderXDifference - 237
            && nXpos <= gnScreenWidth - gnBorderXDifference - 217 && (!sgInventoryMode || sgInventoryMode == 11 || sgInventoryMode == 12 || sgInventoryMode == 15))
        {
            gbDropGoldBtnPressed = 1;
            SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
        }

        if (nXpos >= gnScreenWidth - gnBorderXDifference - 302 && nXpos <= gnScreenWidth - gnBorderXDifference - 270 && nYpos >= gnBorderYDifference + gnScreenHeight - 96 && nYpos <= gnBorderYDifference + gnScreenHeight - 64
            && (!sgInventoryMode || sgInventoryMode == 19))
        {
            gbInvCloseBtnArmed = 1;
            pMsg->bReturn = 1;
            goto CONSUME_WITH_SOUND;
        }
    }

    if (!PANEL_BeltHitTestPanel(pMsg) && nXpos >= gInvPanelRect.nLeft && nXpos < gInvPanelRect.nRight && nYpos >= gInvPanelRect.nTop && nYpos <= gInvPanelRect.nBottom)
    {
        goto IN_PANEL;
    }

    //  the panels that delegate their grid to this function
    if (sgInventoryMode == 11)
    {
        if (nXpos >= gInvTradePage2Grid.nGridLeft && nXpos <= gInvTradePage2Grid.nGridRight && nYpos >= gInvTradePage2Grid.nGridTop && nYpos <= gInvTradePage2Grid.nGridBottom)
        {
            nHandled = INV_OnGridLeftClick(pPlayer, pInventory, nXpos, nYpos, (uint8_t)pMsg->wParam, &gInvTradePage2Grid, INVPAGE_TRADE);
            pMsg->lResult = 0;
            pMsg->bReturn = 1;
            gbShowItemDesc = 0;
            gbShowSetItemDesc = 0;
            gpHoverItem = NULL;
            gpHoverItemLast = 0;
            STORM_SMsgBreakHandlerChain(pMsg);
            //  the only arm that reads the result
            if (nHandled)
            {
                TRADE_SendGreenCheck();
            }
            return;
        }
    }
    else if (sgInventoryMode == 12)
    {
        if (GAME_IsExpansionGame())
        {
            nGridLeft = gInvBigStashGrid.nGridLeft;
            nGridRight = gInvBigStashGrid.nGridRight;
            nGridTop = gInvBigStashGrid.nGridTop;
            nGridBottom = gInvBigStashGrid.nGridBottom;
        }
        else
        {
            nGridLeft = gInvStashGrid.nGridLeft;
            nGridRight = gInvStashGrid.nGridRight;
            nGridTop = gInvStashGrid.nGridTop;
            nGridBottom = gInvStashGrid.nGridBottom;
        }
        if (nXpos >= nGridLeft && nXpos <= nGridRight && nYpos >= nGridTop && nYpos <= nGridBottom)
        {
            //  and the expansion is asked again to pick the grid pointer
            if (GAME_IsExpansionGame())
            {
                INV_OnGridLeftClick(pPlayer, pInventory, nXpos, nYpos, (uint8_t)pMsg->wParam, &gInvBigStashGrid, INVPAGE_STASH);
            }
            else
            {
                INV_OnGridLeftClick(pPlayer, pInventory, nXpos, nYpos, (uint8_t)pMsg->wParam, &gInvStashGrid, INVPAGE_STASH);
            }
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
            gbShowItemDesc = 0;
            gbShowSetItemDesc = 0;
            gpHoverItem = NULL;
            gpHoverItemLast = 0;
            STORM_SMsgBreakHandlerChain(pMsg);
            return;
        }
    }
    else if (sgInventoryMode == 14 && nXpos >= gInvCubeGrid.nGridLeft && nXpos <= gInvCubeGrid.nGridRight && nYpos >= gInvCubeGrid.nGridTop && nYpos <= gInvCubeGrid.nGridBottom)
    {
        INV_OnGridLeftClick(pPlayer, pInventory, nXpos, nYpos, (uint8_t)pMsg->wParam, &gInvCubeGrid, INVPAGE_CUBE);
        pMsg->lResult = 0;
        pMsg->bReturn = 1;
        gbShowItemDesc = 0;
        gbShowSetItemDesc = 0;
        gpHoverItem = NULL;
        gpHoverItemLast = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        return;
    }

    pMsg->bReturn = 0;

LATE_800x600:
    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600 && !PANEL_BeltHitTestPanel(pMsg) && nYpos <= gnScreenHeight - 48 && (SCROLL_GetUiShiftScreen() != PANEL_RIGHT || nXpos >= gnScreenWidth / 2))
    {
        if (CURSOR_GetCursorState() == 6)
        {
            UI_CursorSendUseHeldItem();
        }
        //  here the mode can really be 0, unlike the cube's and the stash's copies of this block
        if (SCROLL_GetUiShiftScreen() != PANEL_BOTH || sgInventoryMode || nXpos >= gnScreenWidth / 2)
        {
            pMsg->bReturn = 1;
            gbShowItemDesc = 0;
            gbShowSetItemDesc = 0;
            STORM_SMsgBreakHandlerChain(pMsg);
        }
    }
    return;

IN_PANEL:
    if (nXpos >= gnScreenWidth - gnBorderXDifference - 302 && nXpos <= gnScreenWidth - gnBorderXDifference - 270 && nYpos >= gnBorderYDifference + gnScreenHeight - 96 && nYpos <= gnBorderYDifference + gnScreenHeight - 64)
    {
        gbInvCloseBtnArmed = 1;
        UI_SetUIToggle(UI_INVENTORY, UI_TURNOFF, 0);
        pMsg->bReturn = 1;
        goto CONSUME_WITH_SOUND;
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    nHandled = 0;

    if (nXpos >= gInvPanelGrid.nGridLeft && nXpos <= gInvPanelGrid.nGridRight && nYpos >= gInvPanelGrid.nGridTop && nYpos <= gInvPanelGrid.nGridBottom)
    {
        nHandled = INV_OnGridLeftClick(pPlayer, pInventory, nXpos, nYpos, (uint8_t)pMsg->wParam, &gInvPanelGrid, INVPAGE_INVENTORY);
        goto GRID_HANDLED;
    }

    nBodyLocHit = -1;
    for (iBodyLoc = 0; iBodyLoc < 11; ++iBodyLoc)
    {
        if (nXpos >= gInvBodyLocRects[iBodyLoc].rect.nLeft && nXpos <= gInvBodyLocRects[iBodyLoc].rect.nRight && nYpos >= gInvBodyLocRects[iBodyLoc].rect.nTop && nYpos <= gInvBodyLocRects[iBodyLoc].rect.nBottom)
        {
            nBodyLocHit = iBodyLoc;
            break;
        }
    }
    if (nBodyLocHit == -1)
    {
        goto WEAPON_SWAP_TAB;
    }

    pCursorItem = D2COMMON_INVENTORY_GetCursorItem(pInventory);
    pSlotItem = NULL;
    if (CURSOR_GetCursorState() == 6 || CURSOR_GetCursorState() == 8)
    {
        nHandled = gBodyLocClickHandlers[nBodyLocHit](pPlayer, pInventory, (D2C_PlayerBodyLocs)nBodyLocHit);
        goto GRID_HANDLED;
    }
    pSlotItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, (D2C_PlayerBodyLocs)nBodyLocHit);
    if (!INV_CanInsertSocketFiller(pCursorItem, pSlotItem))
    {
        nHandled = gBodyLocClickHandlers[nBodyLocHit](pPlayer, pInventory, (D2C_PlayerBodyLocs)nBodyLocHit);
        goto GRID_HANDLED;
    }

    if (!ITEM_IsItemLocked(pCursorItem))
    {
        if (GAME_GetGameType() == GAMETYPE_SINGLEPLAYER || GAME_GetGameType() == GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            goto SEND_SOCKET_INSERT;
        }
        if (TRADE_TickTimeout())
        {
            TRADE_SendGreenCheck();
        }
        else if (GetTickCount() - gnLastItemUseTick > 0xFA)
        {
            //  GetTickCount twice, once to compare and once to store
            gnLastItemUseTick = GetTickCount();
            goto SEND_SOCKET_INSERT;
        }
    }
    goto AFTER_SOCKET;

SEND_SOCKET_INSERT:
    nSlotItemId = pSlotItem ? pSlotItem->dwUnitId : -1;
    nCursorItemId = pCursorItem ? pCursorItem->dwUnitId : -1;
    MSG_SendCmdUnit(40, nCursorItemId, nSlotItemId);
    ITEM_SetUninterruptableState(pCursorItem);
    nDropSound = ITEM_GetDropSoundNoFrame(pSlotItem);
    SOUNDINST_PlaySound(nDropSound, NULL, 0, 0, 0);
    bInGrid = 1;

AFTER_SOCKET:
    nHandled = 0;
    if (bInGrid)
    {
        goto CONSUME_TAIL;
    }

WEAPON_SWAP_TAB:
    //  a 24-pixel strip above either arm rect
    if (GAME_IsExpansionGame()
        && ((nXpos >= gInvBodyLocRects[BODYLOC_LARM].rect.nLeft
             && nXpos <= gInvBodyLocRects[BODYLOC_LARM].rect.nRight
             && nYpos >= gInvBodyLocRects[BODYLOC_LARM].rect.nTop - 24
             && nYpos < gInvBodyLocRects[BODYLOC_LARM].rect.nTop)
            || (nXpos >= gInvBodyLocRects[BODYLOC_RARM].rect.nLeft
                && nXpos <= gInvBodyLocRects[BODYLOC_RARM].rect.nRight
                && nYpos >= gInvBodyLocRects[BODYLOC_RARM].rect.nTop - 24
                && nYpos < gInvBodyLocRects[BODYLOC_RARM].rect.nTop)))
    {
        //  claimed before the expansion test below, so classic consumes the click and does nothing
        pMsg->bReturn = 1;
        if (FOG_IsExpansion() && GAME_IsExpansionGame())
        {
            SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
            MSG_SendCmd1(96);
            pInventory = CUNIT_GetPlayer()->pInventory;
            pItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RARM);
            if (pItem)
            {
                ITEM_SetUninterruptableState(pItem);
            }
            pItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_LARM);
            if (pItem)
            {
                ITEM_SetUninterruptableState(pItem);
            }
        }
        STORM_SMsgBreakHandlerChain(pMsg);
        return;
    }
    nHandled = 0;
    goto CONSUME_TAIL;

GRID_HANDLED:
    bInGrid = 1;

CONSUME_TAIL:
    pMsg->lResult = 0;
    pMsg->bReturn = 1;
    if (!nHandled)
    {
        if (CURSOR_GetCursorState() == 6)
        {
            UI_CursorSendUseHeldItem();
        }
        else if (CURSOR_GetCursorState() == 8)
        {
            MSG_SendServerItemMessage(76, -1);
        }
    }
    STORM_SMsgBreakHandlerChain(pMsg);
    gbShowItemDesc = 0;
    gbShowSetItemDesc = 0;
    gpHoverItem = NULL;
    gpHoverItemLast = 0;
    if (!bInGrid)
    {
        goto LATE_800x600;
    }
    return;

CONSUME_WITH_SOUND:
    pMsg->lResult = 0;
    SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
    STORM_SMsgBreakHandlerChain(pMsg);
}

//  The shop's state functions: thirteen functions, none a message handler, that the handlers above and the NPCMENU_ transaction path call to
//  move the shop between its modes and to build its tab bar.
//  gNpcShopTabs is an array of 20-byte records with no padding (D2InvButtonInfoStrc in D2Structs.h declares the members bEnabled, bPressed,
//  nHoverTextId, nInventoryMode, nBaseFrame and nCursorFrame, in this order):
//      +0   int     the tab is enabled
//      +4   int     the tab is selected
//      +8   uint16  the label's string id
//      +10  int     the panel it opens (a four-byte store to an address that is 2 mod 4, so it straddles its own words)
//      +14  int16   the action id
//      +16  int     the cursor to show
//  The five tab setters are one function with five constant rows:
//      setter           string            panel  action  cursor  enabled
//      SetTabBuy        3335 Buy             2      2       3       1
//      SetTabSell       3336 Sell            3      4       4       1
//      SetTabRepair     3338 Repair          4      6       1       1
//      SetTabRepairAll  10095 Repair all     1     18       8       1
//      SetTabEmpty      0                    1      0       1       0
//  SetTabEmpty is what makes a slot dead, through the enabled flag and not the string: it is the only one that writes zero there. Every
//  NPC that cannot repair gets it at slot 2, so the merchants' bar reads Buy, Sell, nothing, Close. Each setter leaves 20 * nSlot in eax at
//  its return; no caller reads it, so they are void.
//  The clearing loop of ITEMPANEL_ClearShopTabs (end-address sentinel 0x6FBB37FC) is written out in several functions of this unit.
//  FOG_DisplayAssert("sgInventoryMode == BUYSELL_VERIFY", <inv.cpp>, 7783) and FOG_DisplayAssert("sgInventoryMode == BUYSELL_DISPLAYING_RESULTS",
//  <inv.cpp>, 7833) make 5 BUYSELL_VERIFY and 7 BUYSELL_DISPLAYING_RESULTS, the game's own names for two values of sgInventoryMode.

//  The two assert texts this block pushes (0x6FB79948 and 0x6FB7996C).
static const char szVerifyModeAssert[] = "sgInventoryMode == BUYSELL_VERIFY";
static const char szResultsModeAssert[] = "sgInventoryMode == BUYSELL_DISPLAYING_RESULTS";

//1.10f: D2Client.0x6FAE94D0
//  Requests a page rebuild: the vendor's stock changed, so the pages are re-bucketed next frame. The dirty flag lies between the item-message
//  handler ITEM_ItemMsg0B0C_UpdateNpcStore (item messages 0x0B and 0x0C of packet 0x9C) and the draw loop, INV_UpdateAndDrawPanels, which clears it
//  after it calls INV_GetVendorPageInventory.
void __cdecl INV_NpcShopRequestPageRebuild(void)
{
    gbVendorPageRebuildPending = 1;
}

//1.10f: D2Client.0x6FAE94E0
//  Enters the quest-item panel (imbue, socket, personalise); sole caller USEITEM_AnvilOpenPanel. The vendor page is zeroed through its low
//  byte only (mov byte ptr gnVendorPage, 0 at 0x6FAE94E0), so the upper three bytes keep whatever the last shop left there.
void __cdecl INV_NpcShopEnterPanelMode19(void)
{
    gnVendorPage = 0;
    sgInventoryMode = 19;
}

//1.10f: D2Client.0x6FAE9500
//  Leaves panel mode 5, and traces instead of asserting if the mode is not 5: it calls FOG_Trace("Inventory mode is %d\n", sgInventoryMode)
//  and returns, so a wrong mode is a line in a log rather than a process exit. Its siblings assert (INV_NpcShopLeaveVerifyMode on 5,
//  INV_NpcShopCloseAndRestoreCursor on 7); whether the difference is deliberate is not established.
void __cdecl INV_LeavePanelMode5(void)
{
    if (sgInventoryMode != 5)
    {
        FOG_Trace("Inventory mode is %d\n", sgInventoryMode);
        return;
    }

    ITEMPANEL_ClearShopTabs();
    sgInventoryMode = 1;
    CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(CUNIT_GetPlayer()->pInventory));
}

//1.10f: D2Client.0x6FAE9550
//  Leaves the buy/sell verify screen: asserts the mode is 5 (BUYSELL_VERIFY, inv.cpp:7783), then sets it to 6.
//  Sole caller NPCMENU_SendTransactionAndWait.
void __cdecl INV_NpcShopLeaveVerifyMode(void)
{
    if (sgInventoryMode != 5)
    {
        FOG_DisplayAssert(szVerifyModeAssert, szInvCpp, 7783);
        exit(-1);
    }

    sgInventoryMode = 6;
}

//1.10f: D2Client.0x6FAE9590
//  Enters mode 6 and says whether it changed anything: 0 when the mode was already 6 (xor eax, eax at 0x6FAE959F), 1 when it was not
//  (mov eax, 1 at 0x6FAE95A7). The only one of the three mode setters that reports. The sole caller, NPCMENU_SendTransactionAndWait (call
//  at 0x6FAFA7DE), does not read the answer; the BOOL stays because the two exits are the source's.
BOOL __cdecl INV_NpcShopEnterPanelMode6(void)
{
    if (sgInventoryMode == 6)
    {
        return 0;
    }

    sgInventoryMode = 6;
    return 1;
}

//1.10f: D2Client.0x6FAE95B0
//  Resets the shop panel after a transaction. In order: remember which tab was selected, clear the whole selection column, put the mode back
//  to 1, re-attach whatever the cursor held. The search advances while the selected flag is zero and stores the index where it is not, so it
//  finds the first selected tab and leaves -1 when there is none; the loop bound and the guard on the result are signed comparisons.
//  The hit index (gnNpcShopTabHit) is written twice and both writes are in the bytes: 0x6FAE95BA stores -1 unconditionally before the
//  search and 0x6FAE95DA stores the index only if a selected tab was found; a reader running between them sees -1.
void __cdecl INV_ResetItemGrids(void)
{
    int32_t iSelectedTab;

    iSelectedTab = 0;
    gnNpcShopTabHit = -1;
    if (gnNpcShopTabCount > 0)
    {
        //  the original steps a pointer and the count together (0x6FAE95C6); the count stops it, and gnNpcShopTabCount is at most 4,
        //  so the record is read by the index
        while (!gNpcShopTabs[iSelectedTab].bPressed)
        {
            ++iSelectedTab;
            if (iSelectedTab >= gnNpcShopTabCount)
            {
                goto CLEAR;
            }
        }
        gnNpcShopTabHit = iSelectedTab;
    }

CLEAR:
    ITEMPANEL_ClearShopTabs();
    sgInventoryMode = 1;
    CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(CUNIT_GetPlayer()->pInventory));
}

//1.10f: D2Client.0x6FAE9620
//  Enters mode 7, the results screen, once the server has answered; the counterpart of the 6 that INV_NpcShopEnterPanelMode6 sets while the
//  transaction is in flight. Sole caller NPCMENU_OnTransactionResult.
void __cdecl INV_NpcShopEnterPanelMode7(void)
{
    sgInventoryMode = 7;
}

//1.10f: D2Client.0x6FAE9630
//  Leaves the results screen: asserts the mode is 7 (BUYSELL_DISPLAYING_RESULTS, inv.cpp:7833), clears the selection column, drops to mode 1
//  and puts the cursor item back. Sole caller NPCMENU_OnResultContinueClicked, which overwrites eax with 1 on the next instruction.
void __cdecl INV_NpcShopCloseAndRestoreCursor(void)
{
    if (sgInventoryMode != 7)
    {
        FOG_DisplayAssert(szResultsModeAssert, szInvCpp, 7833);
        exit(-1);
    }

    ITEMPANEL_ClearShopTabs();
    sgInventoryMode = 1;
    CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(CUNIT_GetPlayer()->pInventory));
}

//1.10f: D2Client.0x6FAE9690
//  The repair-all tab: string 10095, panel 1, action 18, cursor 8. Used only by the five act smiths, always at slot 3, where
//  INV_NpcShopBuildTabs then overwrites it with the Close tab. Its string id is the one INV_NpcShopApplyTabSelection tests for, because this
//  tab performs an action instead of opening a page.
void __fastcall INV_NpcShopSetTabRepairAll(int32_t nSlot)
{
    gNpcShopTabs[nSlot].nHoverTextId = STR_IDX_10095_x_REPAIR_ALL_EQUIPMENT_FMT;
    gNpcShopTabs[nSlot].nInventoryMode = VENDORMODE_TRADE;
    gNpcShopTabs[nSlot].nCursorFrame = CURSORBUYSELLFRAME_TELEKINESIS;
    gNpcShopTabs[nSlot].nBaseFrame = 18;
    gNpcShopTabs[nSlot].bEnabled = 1;
}

//1.10f: D2Client.0x6FAE96D0
//  The repair tab: string 3338, panel 4, action 6, cursor 1. The five act smiths only.
void __fastcall INV_NpcShopSetTabRepair(int32_t nSlot)
{
    gNpcShopTabs[nSlot].nHoverTextId = STR_IDX_3338_NPCRepairItems;
    gNpcShopTabs[nSlot].nInventoryMode = VENDORMODE_REPAIR;
    gNpcShopTabs[nSlot].nCursorFrame = CURSORBUYSELLFRAME_REPAIR;
    gNpcShopTabs[nSlot].nBaseFrame = 6;
    gNpcShopTabs[nSlot].bEnabled = 1;
}

//1.10f: D2Client.0x6FAE9710
//  The buy tab: string 3335, panel 2, action 2, cursor 3.
void __fastcall INV_NpcShopSetTabBuy(int32_t nSlot)
{
    gNpcShopTabs[nSlot].nHoverTextId = STR_IDX_3335_NPCPurchaseItems;
    gNpcShopTabs[nSlot].nInventoryMode = VENDORMODE_BUY;
    gNpcShopTabs[nSlot].nCursorFrame = CURSORBUYSELLFRAME_BUY;
    gNpcShopTabs[nSlot].nBaseFrame = 2;
    gNpcShopTabs[nSlot].bEnabled = 1;
}

//1.10f: D2Client.0x6FAE9750
//  The sell tab: string 3336, panel 3, action 4, cursor 4. The only setter both NPC groups use.
void __fastcall INV_NpcShopSetTabSell(int32_t nSlot)
{
    gNpcShopTabs[nSlot].nHoverTextId = STR_IDX_3336_NPCSellItems;
    gNpcShopTabs[nSlot].nInventoryMode = VENDORMODE_SELL;
    gNpcShopTabs[nSlot].nCursorFrame = CURSORBUYSELLFRAME_SELL;
    gNpcShopTabs[nSlot].nBaseFrame = 4;
    gNpcShopTabs[nSlot].bEnabled = 1;
}

//1.10f: D2Client.0x6FAE9790
//  Blanks a tab: string 0, panel 1, action 0, cursor 1, and enabled = 0. The only setter that clears the enabled flag instead of setting
//  it, which is what makes the slot dead (not the zero string). Slot 2 of every NPC that cannot repair.
void __fastcall INV_NpcShopSetTabEmpty(int32_t nSlot)
{
    gNpcShopTabs[nSlot].nHoverTextId = STR_IDX_0_WarrivAct1IntroGossip1;
    gNpcShopTabs[nSlot].nInventoryMode = VENDORMODE_TRADE;
    gNpcShopTabs[nSlot].nCursorFrame = CURSORBUYSELLFRAME_REPAIR;
    gNpcShopTabs[nSlot].nBaseFrame = 0;
    gNpcShopTabs[nSlot].bEnabled = 0;
}

//  The panel lifecycle functions: INV_CubeFindStashUnit, INV_StashLoadImages and INV_StashOnPanelClose (0x6FAEC560..0x6FAEC6BC), and the cube's
//  load/close/anim pair with the trade panel's three resets and one refusal (0x6FAECFE0..0x6FAED27B). UI_RegisterPanelHandlers calls the
//  loaders, UI_UnregisterPanelHandlers the closers and the NPCMENU state machine the trade trio. The first half of the cube's and the stash's
//  close functions is the same code as their dismiss-key handlers (escape and panel teardown converge).
//  Both close functions end with MSG_SendCmd3Words(79, ...): mov cl, 4Fh writes only the low byte of ecx, the callee's parameter being a char.

//  The module's single copy of the string DATA\GLOBAL (0x6FB743EC); other source files of ours declare their own.
static const char aDataGlobalInv[] = "DATA\\GLOBAL";

//1.10f: D2Client.0x6FAEC560
//  A unit visitor that finds the stash: it has no ordinary caller, its address is handed to a unit-walking function. The unit it caches is the
//  stash chest (objects.txt row 267, bank), not the cube; the GUID is read by the three cube-panel exit paths and passed to
//  UNITMODE_InteractWithUnit, and the global keeps the name gnCubeUnitGUID because it is the cube's half of that handshake.
//  The act test is done the long way round (room to level id to act number, on both units) rather than by comparing rooms.
void __fastcall INV_CubeFindStashUnit(D2UnitStrc *pUnit)
{
    D2ActiveRoomStrc *pUnitRoom;
    D2ActiveRoomStrc *pPlayerRoom;
    uint8_t nPlayerAct;

    if (!pUnit || pUnit->dwClassId != 267)
    {
        return;
    }

    pUnitRoom = D2COMMON_UNITS_GetRoom(pUnit);
    if (!pUnitRoom)
    {
        return;
    }

    pPlayerRoom = D2COMMON_UNITS_GetRoom(CUNIT_GetPlayer());
    if (!pPlayerRoom)
    {
        return;
    }

    nPlayerAct = D2COMMON_DRLG_GetActNoFromLevelId((D2C_Levels)D2COMMON_DUNGEON_GetLevelIdFromRoom(pPlayerRoom));
    if (nPlayerAct == D2COMMON_DRLG_GetActNoFromLevelId((D2C_Levels)D2COMMON_DUNGEON_GetLevelIdFromRoom(pUnitRoom)))
    {
        gnCubeUnitGUID = pUnit->dwUnitId;
    }
}

//1.10f: D2Client.0x6FAEC5C0
//  Loads the stash panel art (called from UI_RegisterPanelHandlers): gpStashPanelImage = DATA\GLOBAL\ui\panel\bank and, in the expansion
//  only, gpTradeStashImage = DATA\GLOBAL\ui\panel\TradeStash. The zero stored into the unnamed global 0x6FBB3790 is on the path taken on
//  every call, before the guard.
//  BUG: the guard tests gpStashPanelImage alone but also covers a second image loaded behind FOG_IsExpansion(), so an installation with bank
//  and without TradeStash would never retry the second. LATENT: the expansion MPQ carries both; the test is about the installation
//  (FOG_IsExpansion), not the game being joined (GAME_IsExpansionGame).
void __cdecl INV_StashLoadImages(void)
{
    char szPath[260];

    UNRESOLVED_dword_6FBB3790 = 0;
    if (!gpStashPanelImage)
    {
        STORM_SStrPrintf(szPath, 0x104u, "%s\\ui\\panel\\bank", aDataGlobalInv);
        gpStashPanelImage = ARCHIVE_CellFileLoad(szPath, 0);
        if (FOG_IsExpansion())
        {
            STORM_SStrPrintf(szPath, 0x104u, "%s\\ui\\panel\\TradeStash", aDataGlobalInv);
            gpTradeStashImage = ARCHIVE_CellFileLoad(szPath, 0);
        }
    }
}

//1.10f: D2Client.0x6FAEC650
//  The stash arm of UI_UnregisterPanelHandlers. Modes 12 and 13 both belong here because the stash and the trade stash share one panel.
//  Command 79 with subcommand 18 is "I closed the stash" (INV_CubeOnPanelClose sends 79 with 23 for the cube). Unlike INV_StashLoadImages
//  there is no expansion test on the unload: a null handle is simply skipped.
void __cdecl INV_StashOnPanelClose(void)
{
    if (sgInventoryMode == 12 || sgInventoryMode == 13)
    {
        sgInventoryMode = 0;
        UI_SetUIToggle(UI_STASH, UI_TURNOFF, 0);
        MSG_SendCmd3Words(79, 18, 0, 0);
    }

    if (gpStashPanelImage)
    {
        ARCHIVE_CellFileFree(gpStashPanelImage);
        gpStashPanelImage = NULL;
    }
    if (gpTradeStashImage)
    {
        ARCHIVE_CellFileFree(gpTradeStashImage);
        gpTradeStashImage = NULL;
    }
}

//1.10f: D2Client.0x6FAECFE0
//  Loads the Horadric Cube panel background (called from UI_RegisterPanelHandlers): the file is supertransmogrifier, built with SStrPrintf and
//  loaded with ARCHIVE_CellFileLoad as INV_StashLoadImages does. Loads once; the free is in INV_CubeOnPanelClose. The zero stored into the
//  unnamed global happens on every call, including the early-out, because it is before the guard.
void __cdecl INV_CubeLoadPanelImage(void)
{
    char szPath[260];

    UNRESOLVED_dword_6FBB3790 = 0;
    if (!gpImgCubePanel)
    {
        STORM_SStrPrintf(szPath, 0x104u, "%s\\ui\\panel\\supertransmogrifier", aDataGlobalInv);
        gpImgCubePanel = ARCHIVE_CellFileLoad(szPath, 0);
    }
}

//1.10f: D2Client.0x6FAED030
//  The cube arm of UI_UnregisterPanelHandlers (case 26). Its first half is INV_CubeOnDismissKey's tail from gbHoradricAnimActive = 0 onward;
//  the difference is that escape tests the chat box first and this does not, because a panel teardown is not a key press. Then the panel
//  image is freed unconditionally.
void __cdecl INV_CubeOnPanelClose(void)
{
    if (sgInventoryMode == 14)
    {
        sgInventoryMode = 0;
        gbHoradricAnimActive = 0;
        UI_SetUIToggle(UI_CUBE, UI_TURNOFF, 0);
        if (!gbCubeCloseCmdSent)
        {
            MSG_SendCmd3Words(79, 23, 0, 0);
            gbCubeCloseCmdSent = 1;
        }
        if (gbCubeOpenedFromStash)
        {
            if (CUNIT_FindUnitByGuidAndType(gnCubeUnitGUID, UNIT_OBJECT))
            {
                UNITMODE_InteractWithUnit(2u, gnCubeUnitGUID);
            }
            gbCubeOpenedFromStash = 0;
        }
    }

    if (gpImgCubePanel)
    {
        ARCHIVE_CellFileFree(gpImgCubePanel);
        gpImgCubePanel = NULL;
    }
}

//1.10f: D2Client.0x6FAED0D0
//  Starts the transmute animation when a quest item lands in the cube. The one caller, ITEM_PlaceAtStorePage (owner is the local player,
//  page 3), passes only two item codes:
//      0x20747368  'hst '  Horadric Staff  -> quest 10, state 11
//      0x20326671  'qf2 '  Khalim's Will   -> quest 18, state 11
//  The quest numbers are D2QuestStateFlagIds, not D2Quests. State 11 is QFLAG_CUSTOM7, a client-side "already shown" bit, checked first and
//  set second. The two arms converge on one block, which is kept as a goto.
//  BUG: any other item code falls through to starting the animation with no quest bookkeeping (the shape of a switch that lost its other
//  arms). UNREACHABLE: the one caller passes only the two codes above.
void __fastcall INV_CubeStartQuestItemAnim(uint32_t dwItemCode)
{
    D2BitBufferStrc *pQuestRecord;
    char szPath[260];

    pQuestRecord = NPCMENU_QuestGetQuestRecord();
    if (dwItemCode != 0x20747368)
    {
        if (dwItemCode == 0x20326671)
        {
            if (!pQuestRecord || D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, 18, 11))
            {
                return;
            }
            D2COMMON_QUESTRECORD_SetQuestState(pQuestRecord, 18, 11);
        }
        goto START;
    }

    if (!pQuestRecord || D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, 10, 11))
    {
        return;
    }
    D2COMMON_QUESTRECORD_SetQuestState(pQuestRecord, 10, 11);

START:
    gbHoradricAnimActive = 1;
    gnHoradricAnimFrame = 0;
    gdwHoradricAnimLastTick = GetTickCount();
    if (!gpHoradricAnimImage)
    {
        STORM_SStrPrintf(szPath, 0x104u, "%s\\ui\\menu\\horadric", aDataGlobalInv);
        gpHoradricAnimImage = ARCHIVE_CellFileLoad(szPath, 0);
    }
}

//1.10f: D2Client.0x6FAED190
//  Clears the transmute animation. Its one caller, UI_InitUiForPlayerUnit, makes it the start-of-life reset; nothing calls it to end an
//  animation (INV_UpdateAndDrawPanels does that inline when the frame counter reaches 30).
void __cdecl INV_CubeResetQuestItemAnim(void)
{
    if (gpHoradricAnimImage)
    {
        ARCHIVE_CellFileFree(gpHoradricAnimImage);
        gpHoradricAnimImage = NULL;
    }
    gbHoradricAnimActive = 0;
    gnHoradricAnimFrame = 0;
    gdwHoradricAnimLastTick = 0;
}

//1.10f: D2Client.0x6FAED1C0
//  Puts the held item back after a transaction: the mode goes to 1, the inventory's cursor item goes back on the cursor, and a system
//  message { 0x5A, 14, 1, 0 } is synthesised and handed to TEXT_HandleSystemMessage, the handler that receives them from the server. The
//  original's eight-byte stack buffer is one object: nHeader, nType, nColor and dwParam of D2GSPacketSrv5A.
//  BUG: message type 14 has no arm in TEXT_HandleSystemMessage (its jump table has 19 entries and cases 12 and 14 both point at the
//  epilogue, 0x6FB1DD80), so the call prints nothing and changes nothing. INERT: the packet is built and the call happens, with no effect.
//  Whether 14 was cut or never existed is not established.
//  DIFFERENCE: the original's frame for the packet is eight bytes; D2GSPacketSrv5A is 40 because the struct carries the union of every 0x5A
//  form, including the 32-character text. Nothing reads past offset 7 on this path, so only the stack local is larger.
void __cdecl INV_TradeRestoreHeldItem(void)
{
    D2GSPacketSrv5A tPacket;

    sgInventoryMode = 1;
    CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(CUNIT_GetPlayer()->pInventory));

    tPacket.nHeader = 0x5A;
    tPacket.nColor = 1;
    tPacket.dwParam = 0;
    tPacket.nType = 14;
    TEXT_HandleSystemMessage(&tPacket);
}

//1.10f: D2Client.0x6FAED210
//  Eleven bytes, one store: zeroes the unnamed global 0x6FBB3790, which has four writers (INV_CubeLoadPanelImage, INV_NpcShopBuildTabs,
//  INV_StashLoadImages and this function), all writing zero, and no readers. It has no name for that reason. With no arguments __cdecl and
//  __stdcall emit the same bytes, so the convention written here is not a finding.
void __cdecl UNRESOLVED_sub_6FAED210(void)
{
    UNRESOLVED_dword_6FBB3790 = 0;
}

//1.10f: D2Client.0x6FAED220
//  Ends a transaction, and an item in hand survives it: no player -> CURSOR_SetHeldUnit(NULL); a player without an item ->
//  CURSOR_SetHeldUnit(NULL); a player with an item -> nothing, the item stays on the cursor. The callers are the three ways a transaction
//  ends: NPCMENU_EndInteractionByUiState, NPCMENU_OnInteractionStop and TRADE_TickTimeout. It returns void; the eax it leaves differs by arm
//  and every caller discards it.
void __cdecl INV_TradeResetPanelState(void)
{
    D2UnitStrc *pPlayer;

    gbPanelBtnPressActive = 0;
    sgInventoryMode = 0;
    gnUiMouseX = 0;
    gnUiMouseY = 0;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        CURSOR_SetHeldUnit(NULL);
        return;
    }
    if (!D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory))
    {
        CURSOR_SetHeldUnit(NULL);
    }
}

//1.10f: D2Client.0x6FAED260
//  Blocks the trade accept button (one store). Sole caller TRADE_HandleStateMessage, which tail-jumps here.
void __cdecl INV_TradeSetAcceptBlocked(void)
{
    gbTradeAcceptBlocked = 1;
}

//1.10f: D2Client.0x6FAED270
//  Unblocks it (the twin of INV_TradeSetAcceptBlocked). The blocked look is dropped when the window closes (TRADE_CloseWindow), when the hint
//  text is redrawn (TRADE_DrawButtonHints) and twice by the state machine (TRADE_HandleStateMessage, one of them a tail jump). The reader of
//  the flag is INV_UpdateAndDrawPanels.
void __cdecl INV_TradeClearAcceptBlocked(void)
{
    gbTradeAcceptBlocked = 0;
}

//1.10f: D2Client.0x6FAED280
//  The server says the weapon set changed. It is the target of the tail jump of SCMD_HandleSrv97_WeaponSwitch (0x6FAB43E0), which asserts that
//  the packet's first byte is 0x97.
//  The flip is a comparison, not an xor (mov ecx, gnWeaponSwapSet / xor eax, eax / test ecx, ecx / setz al / mov gnWeaponSwapSet, eax), so any
//  non-zero value collapses to 0 and only 0 becomes 1; the variable is a boolean in practice.
//  In classic the packet is ignored: both expansion tests are inside this function rather than in the handler, so a classic client consumes
//  the message and does nothing with it.
void __cdecl INV_ToggleWeaponSwapSet(void)
{
    if (FOG_IsExpansion() && GAME_IsExpansionGame())
    {
        gnWeaponSwapSet = (gnWeaponSwapSet == 0);
    }
}

//1.10f: D2Client.0x6FAED2B0
//  The player asks to swap weapons. This is the owner-less tail of CMDTBL_ActionSwapWeapons (0x6FAD3EF0), which refuses while UI window 0x0C,
//  0x17 or 0x19 is open and otherwise jumps here. The original also inlines this body into INV_OnKeySwapWeapons (0x6FAEC499..0x6FAEC4F3) and
//  keeps this out-of-line copy for the command table, so the guard that looks redundant at that call site is the callee's own; the one
//  difference is that the handler reaches ITEM_SetUninterruptableState with a call (it has message bookkeeping still to do), this copy with
//  a tail jmp. The order is fixed: sound, command, then the hands; the inventory is fetched once for both, and either hand may be empty.
void __cdecl INV_SwapWeaponsAndLockItems(void)
{
    D2InventoryStrc *pInventory;
    D2UnitStrc *pItem;

    if (FOG_IsExpansion() && GAME_IsExpansionGame())
    {
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
        MSG_SendCmd1(96);

        pInventory = CUNIT_GetPlayer()->pInventory;
        pItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RARM);
        if (pItem)
        {
            ITEM_SetUninterruptableState(pItem);
        }
        pItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_LARM);
        if (pItem)
        {
            ITEM_SetUninterruptableState(pItem);
        }
    }
}

//  The vendor page functions (0x6FAE8F10..0x6FAE93F9). Two tab arrays with different strides are touched here: gNpcShopTabs (20-byte
//  records, four of them, the bar across the top: Buy, Sell, Repair, Close) and gVendorTabs (18-byte records, gnVendorTabCount of them, the
//  page tabs down the side); gnNpcShopTabCount and gnVendorTabCount are different counters.
//  INV_GetVendorPageInventory counts the NPC's items into five buckets and, if the requested page is empty, cycles forward up to four times
//  (wrapping at gnVendorMaxPage and again at 0xFF) until it finds one that is not, so asking for an empty tab lands on the next occupied one
//  and an NPC with nothing at all leaves you where you started.

//1.10f: D2Client.0x6FAE8F10
//  Drops the item-description hover unconditionally: ten globals (four popup corners and two grid cells to -1, two show flags, and the two
//  hover-item pointers to NULL). Every path that stops hovering an item comes through here. It returns void. It takes no arguments, so
//  __cdecl and __stdcall emit the same bytes.
void __cdecl INV_ResetItemDescState(void)
{
    gnDescPopupX = -1;
    gnDescPopupY = -1;
    gnDescPopupAltX = -1;
    gnDescPopupBottomY = -1;
    gnHoverCellX = -1;
    gnHoverCellY = -1;
    gbShowItemDesc = 0;
    gbShowSetItemDesc = 0;
    gpHoverItem = NULL;
    gpHoverItemLast = 0;
}

//1.10f: D2Client.0x6FAE8F50
//  Drops the item-description hover only if pItem is the item being hovered: the same ten stores as INV_ResetItemDescState behind one test, written
//  out inline because the bytes are ten stores and not a call. Sole caller CUNIT_UnlinkAndFreeUnit: the unit is going away, and the popup must not
//  outlive it.
void __fastcall INV_ClearHoverForItem(D2UnitStrc *pItem)
{
    if (gpHoverItem == pItem)
    {
        gnDescPopupX = -1;
        gnDescPopupY = -1;
        gnDescPopupAltX = -1;
        gnDescPopupBottomY = -1;
        gnHoverCellX = -1;
        gnHoverCellY = -1;
        gbShowItemDesc = 0;
        gbShowSetItemDesc = 0;
        gpHoverItem = NULL;
        gpHoverItemLast = 0;
    }
}

//1.10f: D2Client.0x6FAE8F90
//  Rebuilds the vendor's page tabs and picks a page that has something on it. The argument, a byte, is clamped to 0 when it is 5 or more
//  (five inventory pages), so an out-of-range request silently becomes the first page.
//  gnVendorPageHoldUntil is a deadline, not a cache: while it is non-zero this function does nothing. INV_NpcShopBuildTabs arms it at now + 500,
//  INV_UpdateAndDrawPanels expires it once the clock passes, and INV_NpcShopOnLButtonDown clears it to force an immediate rebuild.
//  Then it walks the NPC's inventory, buckets every item by ITEMS_GetInvPage into five counters, marks each side tab as having items or not, and
//  selects a page: the one asked for if it has items, otherwise the next occupied one within four tries (the limit stops an empty vendor spinning).
void __fastcall INV_GetVendorPageInventory(uint8_t nInvPage)
{
    uint8_t nStartPage;
    uint8_t nRequestedPage;
    D2UnitStrc *pNpc;
    D2InventoryStrc *pInventory;
    D2UnitStrc *pItem;
    int32_t nVendorTabCount;
    int32_t nTries;
    uint8_t nPage;
    int32_t iTab;
    uint8_t nVendorPage;
    D2UnitGUID dwNpcGUID;
    uint8_t nItemPage;
    int32_t aItemsPerPage[5];

    nStartPage = nInvPage;
    nRequestedPage = nInvPage;
    dwNpcGUID = nInvPage;
    if (nInvPage >= 5u)
    {
        //  only the low byte of the slot is cleared, as in the original; the dword is overwritten before anything reads it
        nStartPage = 0;
        *(uint8_t *)&dwNpcGUID = 0;
        nRequestedPage = 0;
    }

    if (gnVendorPageHoldUntil)
    {
        return;
    }
    if (!NPCMENU_GetInteractingNpc(&dwNpcGUID))
    {
        return;
    }
    pNpc = CUNIT_FindUnitByGuidAndType(dwNpcGUID, UNIT_MONSTER);
    if (!pNpc)
    {
        return;
    }
    pInventory = pNpc->pInventory;
    if (!pInventory)
    {
        return;
    }

    memset(aItemsPerPage, 0, sizeof(aItemsPerPage));
    for (pItem = D2COMMON_INVENTORY_GetFirstItem(pInventory); pItem; pItem = D2COMMON_INVENTORY_GetNextItem(pItem))
    {
        nItemPage = D2COMMON_ITEMS_GetInvPage(D2COMMON_INVENTORY_UnitIsItem(pItem));
        if (nItemPage < 5u)
        {
            ++aItemsPerPage[nItemPage];
        }
    }

    nVendorTabCount = gnVendorTabCount;
    nTries = 0;
    nPage = nStartPage;
    iTab = 0;
    gnVendorPage = nStartPage;

    //  the side tabs: 18-byte records, not the 20-byte bar
    if (nVendorTabCount > 0)
    {
        //  the original steps a pointer 18 bytes a record (0x6FAE905A) beside the count; gnVendorTabCount is 4 (nothing writes it),
        //  so the record is read by the index the count keeps
        do
        {
            gVendorTabs[iTab].bSelected = 0;
            gVendorTabs[iTab].bActive = aItemsPerPage[iTab] != 0;
            ++iTab;
        }
        while (iTab < nVendorTabCount);
    }

    if (aItemsPerPage[nRequestedPage])
    {
        nVendorPage = gnVendorPage;
    }
    else
    {
        do
        {
            ++nPage;
            ++nTries;
            gnVendorPage = nPage;
            if (nPage > gnVendorMaxPage || nPage == 0xFF)
            {
                nPage = 0;
                gnVendorPage = 0;
            }
            nVendorPage = gnVendorPage;
        }
        while (!aItemsPerPage[gnVendorPage] && nTries < 4);
    }

    //  BUG: nothing bounds the page index by the four records of gVendorTabs. UNREACHABLE: nVendorPage is 3 at most here (the page comes in as
    //  gnVendorPage, which holds 4 only inside two save-and-restore windows that no path to here crosses, or as the tab clicked, 0..3; the wrap
    //  above resets above gnVendorMaxPage, which is 0 at load, 1 or 3).
    gVendorTabs[nVendorPage].bSelected = 1;
}

//1.10f: D2Client.0x6FAE90F0
//  Resets the inventory panel between sessions: the mode, the vendor page's low byte, the gold button, the UI mouse pair and the item-use tick,
//  plus the trade panel image if one is loaded. gnVendorPage is cleared through its low byte only (mov byte ptr gnVendorPage, bl at 0x6FAE9101),
//  so the upper three bytes survive a reset.
void __cdecl INV_ResetPanelState(void)
{
    sgInventoryMode = 0;
    gnVendorPage = 0;
    gbDropGoldBtnPressed = 0;
    gnUiMouseX = 0;
    gnUiMouseY = 0;
    if (gpImgTradePanel)
    {
        ARCHIVE_CellFileFree(gpImgTradePanel);
        gpImgTradePanel = NULL;
    }
    gnLastItemUseTick = 0;
}

//1.10f: D2Client.0x6FAE9130
//  Builds the shop's tab bar for this NPC and chooses the opening page; sole caller UI_RegisterPanelHandlers, once when the panel opens. Both jobs
//  switch on the NPC's monster id.
//  Tabs: Buy / Sell / Repair / Repair-all for ids 154, 178, 253, 257 and 511 (exactly the five act smiths); Buy / Sell / (empty) for 147, 148, 177,
//  199, 202, 252, 254, 255, 405, 512 and 513; no bar (count = 0) for anything else. When NPCMENU_IsVendorGamble() is true the id checks are skipped
//  and the bar is hard-coded to Buy / Sell / empty, written out inline rather than through the three setters.
//  Slot 0 is written three ways: INV_NpcShopSetTabBuy(0) is called on the merchant path, and the identical five stores are inline on the smith
//  path and on the NPCMENU_IsVendorGamble path; it is written the way the bytes are.
//  The smith path jumps past the Close-tab block: 6FAE938C call INV_NpcShopSetTabRepairAll is followed by 6FAE9391 jmp loc_6FAE924D, which is
//  mov gnNpcShopTabCount, 4, and the Close stores at 0x6FAE922B..0x6FAE9244 are skipped (the merchant path reaches them by 6FAE9337 jmp
//  loc_6FAE922B). So a smith's slot 3 is Repair-all, with no Close tab, while a merchant's bar reads Buy / Sell / (blank) / Close.
//  BUG: the merchant path stores the Close tab's four fields (enabled, string, panel, action) and never writes slot 3's cursor at record +16,
//  which INV_NpcShopApplyTabSelection reads at 0x6FAEAD0F; after any smith it still holds that smith's 8. REACHABLE: the stale cursor frame is used.
//  gnNpcShopCloseTab = 3 is set on both paths and is not a claim that slot 3 is the Close tab: its readers use it as a row index,
//  gatButtonsOffsetX[gnNpcShopCloseTab][iTab] (the bytes' flat dword index is 4 * row + tab), and both bars are four long, so both want row 3.
//  The opening page is a second switch on the same id: page 0 for gheed and drehya, 1 for charsi, larzuk, halbu, jamella and malah, 3 for akara,
//  drognan, fara, lysander, asheara, hratli and ormus. Only NPCs that matched get the 500 ms hold stamp. dwClassId > 0xFF and > 0x1FF bracket the
//  switch as the compiler's range narrowing, not validation; dwClassId - 512 tested against 0 and 1 is how 512 and 513 are reached.
//  The two id switches share their tails, so the labels and gotos are kept.
void __cdecl INV_NpcShopBuildTabs(void)
{
    int32_t nVendorTabCount;
    int32_t iVendorTab;
    int32_t bVendorGamble;
    D2UnitStrc *pNpc;
    UINT_PTR nTab;
    int32_t bHasTabs;
    uint32_t dwClassId;
    uint32_t nClassIdFrom512;
    D2UnitGUID dwNpcGUID;
    D2UnitGUID dwPageNpcGUID;

    nVendorTabCount = gnVendorTabCount;
    gnNpcShopTabHit = -1;
    gnVendorPage = 0;
    gbDropGoldBtnPressed = 0;
    UNRESOLVED_dword_6FBB3790 = 0;
    if (nVendorTabCount > 0)
    {
        //  the original steps a pointer 18 bytes a record with the count down (0x6FAE915F..0x6FAE916D); the record is read by the index
        //  and the count stops it inside the table
        iVendorTab = 0;
        do
        {
            gVendorTabs[iVendorTab].bSelected = 0;
            gVendorTabs[iVendorTab].bActive = 0;
            ++iVendorTab;
            --nVendorTabCount;
        }
        while (nVendorTabCount);
    }
    gbDeselectShopTabsPending = 0;
    sgInventoryMode = 1;

    bVendorGamble = NPCMENU_IsVendorGamble();
    if (!NPCMENU_GetInteractingNpc(&dwNpcGUID))
    {
        goto PICK_PAGE;
    }
    pNpc = CUNIT_FindUnitByGuidAndType(dwNpcGUID, UNIT_MONSTER);
    if (!pNpc)
    {
        goto PICK_PAGE;
    }

    if (bVendorGamble)
    {
        gnVendorMaxPage = 1;
        gNpcShopTabs[0].nHoverTextId = STR_IDX_3335_NPCPurchaseItems;
        gNpcShopTabs[0].nInventoryMode = VENDORMODE_BUY;
        gNpcShopTabs[0].nCursorFrame = CURSORBUYSELLFRAME_BUY;
        gNpcShopTabs[0].nBaseFrame = 2;
        gNpcShopTabs[0].bEnabled = 1;
        gNpcShopTabs[1].nHoverTextId = STR_IDX_3336_NPCSellItems;
        gNpcShopTabs[1].nInventoryMode = VENDORMODE_SELL;
        gNpcShopTabs[1].nCursorFrame = CURSORBUYSELLFRAME_SELL;
        gNpcShopTabs[1].nBaseFrame = 4;
        gNpcShopTabs[1].bEnabled = 1;
        gNpcShopTabs[2].nHoverTextId = STR_IDX_0_WarrivAct1IntroGossip1;
        gNpcShopTabs[2].nInventoryMode = VENDORMODE_TRADE;
        gNpcShopTabs[2].nCursorFrame = CURSORBUYSELLFRAME_REPAIR;
        gNpcShopTabs[2].nBaseFrame = 0;
        gNpcShopTabs[2].bEnabled = 0;
        goto CLOSE_TAB;
    }

    dwClassId = pNpc->dwClassId;
    if (dwClassId > 252)
    {
        if (dwClassId > 405)
        {
            if (dwClassId == 511)
            {
                goto SMITH;
            }
            if (dwClassId <= 511 || dwClassId > 513)
            {
                goto NO_BAR;
            }
        }
        else if (dwClassId != 405)
        {
            switch (dwClassId)
            {
            case 253:
            case 257:
                goto SMITH;
            case 254:
            case 255:
                break;
            default:
                goto NO_BAR;
            }
        }
    }
    else if (dwClassId != 252)
    {
        switch (dwClassId)
        {
        case 147:
        case 148:
        case 177:
        case 199:
        case 202:
            break;
        case 154:
        case 178:
            goto SMITH;
        default:
            goto NO_BAR;
        }
    }

    //  the merchants
    gnVendorMaxPage = 3;
    INV_NpcShopSetTabBuy(0);
    INV_NpcShopSetTabSell(1);
    INV_NpcShopSetTabEmpty(2);
    goto CLOSE_TAB;

SMITH:
    gnVendorMaxPage = 3;
    gNpcShopTabs[0].nHoverTextId = STR_IDX_3335_NPCPurchaseItems;
    gNpcShopTabs[0].nInventoryMode = VENDORMODE_BUY;
    gNpcShopTabs[0].nCursorFrame = CURSORBUYSELLFRAME_BUY;
    gNpcShopTabs[0].nBaseFrame = 2;
    gNpcShopTabs[0].bEnabled = 1;
    INV_NpcShopSetTabSell(1);
    INV_NpcShopSetTabRepair(2);
    INV_NpcShopSetTabRepairAll(3);
    goto COUNT_AND_DESELECT;

CLOSE_TAB:
    //  slot 3; the cursor at +16 is not among these four stores
    gNpcShopTabs[3].bEnabled = 1;
    gNpcShopTabs[3].nHoverTextId = STR_IDX_4144_strClose;
    gNpcShopTabs[3].nInventoryMode = VENDORMODE_UNKNOWN;
    gNpcShopTabs[3].nBaseFrame = 10;

COUNT_AND_DESELECT:
    gnNpcShopTabCount = 4;
    gnNpcShopCloseTab = 3;
    //  the clear of ITEMPANEL_ClearShopTabs, in line here (0x6FAE9261..0x6FAE926B): an integer cursor over our records, signed as the bytes' jl
    nTab = (UINT_PTR)&gNpcShopTabs[0];
    do
    {
        ((D2InvButtonInfoStrc *)nTab)->bPressed = 0;
        nTab += sizeof(D2InvButtonInfoStrc);
    }
    while ((int32_t)nTab < (int32_t)((UINT_PTR)&gNpcShopTabs[0] + 4 * sizeof(D2InvButtonInfoStrc)));
    goto PICK_PAGE;

NO_BAR:
    gnNpcShopTabCount = 0;

PICK_PAGE:
    if (!NPCMENU_GetInteractingNpc(&dwPageNpcGUID))
    {
        return;
    }
    pNpc = CUNIT_FindUnitByGuidAndType(dwPageNpcGUID, UNIT_MONSTER);
    bHasTabs = 0;
    if (!pNpc)
    {
        goto GET_PAGE;
    }

    dwClassId = pNpc->dwClassId;
    if (dwClassId > 0xFF)
    {
        if (dwClassId > 0x1FF)
        {
            nClassIdFrom512 = dwClassId - 512;
            if (!nClassIdFrom512)
            {
                goto PAGE_0;
            }
            if (nClassIdFrom512 != 1)
            {
                goto GET_PAGE;
            }
        }
        else
        {
            if (dwClassId == 511)
            {
                goto PAGE_1;
            }
            if (dwClassId != 257 && dwClassId != 405)
            {
                goto GET_PAGE;
            }
        }
    }
    else if (dwClassId != 255)
    {
        switch (dwClassId)
        {
        case 0x93u:
            goto PAGE_0;
        case 0x94u:
        case 0xB1u:
        case 0xB2u:
        case 0xCAu:
        case 0xFCu:
        case 0xFDu:
            goto PAGE_3;
        case 0x9Au:
            goto PAGE_1;
        default:
            goto GET_PAGE;
        }
    }

PAGE_3:
    gnVendorPage = 3;
    goto HAS_TABS;

PAGE_1:
    gnVendorPage = 1;
    goto HAS_TABS;

PAGE_0:
    gnVendorPage = 0;

HAS_TABS:
    bHasTabs = 1;

GET_PAGE:
    INV_GetVendorPageInventory(gnVendorPage);
    if (bHasTabs)
    {
        gnVendorPageHoldUntil = GetTickCount() + 500;
    }
}

//  The shop-panel and anvil functions (0x6FAEAC40..0x6FAEB1CA). The four anvil functions are not message handlers and are in no table:
//  they are called by USEITEM_AnvilOnMouseMove, USEITEM_AnvilOnButtonDown and USEITEM_AnvilOnButtonUp.
//  The inventory-rect tests of the anvil functions are not symmetric: x < nLeft || x >= nRight || y < nTop || y > nBottom, so the right edge is
//  exclusive and the bottom edge inclusive. Every anvil function that tests the rect carries the same four tests.

//1.10f: D2Client.0x6FAEAC40
//  Applies the tab the mouse went down and up on. Four guards before anything happens: the mode must be 1, 2, 3 or 4; the cursor must be holding
//  nothing; the cursor state must be 7, 1 or 4. Then the whole selection column is cleared, the mode drops to 1 and the player's cursor item is
//  re-attached. The Repair-all tab is the exception, by string id: a hit on string 10095 skips the re-selection and the panel switch, because that
//  tab performs an action instead of opening a page. Everything else selects every tab whose action matches gnNpcShopTabAction, so two tabs
//  sharing an action light up together. Then, if the mouse is over the shop grid, the hovered cell is recomputed and gnNpcTradeMode is cleared.
void __cdecl INV_NpcShopApplyTabSelection(void)
{
    D2UnitStrc *pPlayer;
    int32_t nCursorState;
    UINT_PTR nTab;
    int32_t nWantedAction;
    int32_t nCursorMode;
    D2UnitStrc *pNpc;
    int32_t nMouseX;
    int32_t nMouseY;
    D2UnitGUID dwNpcGUID;

    if (sgInventoryMode != 1 && sgInventoryMode != 2 && sgInventoryMode != 4 && sgInventoryMode != 3)
    {
        return;
    }

    pPlayer = CUNIT_GetPlayer();
    if (D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory))
    {
        return;
    }

    nCursorState = CURSOR_GetCursorState();
    if (nCursorState != 7 && nCursorState != 1 && nCursorState != 4)
    {
        return;
    }

    ITEMPANEL_ClearShopTabs();
    sgInventoryMode = 1;
    pPlayer = CUNIT_GetPlayer();
    CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory));

    if (gnNpcShopTabHit >= 0 && gnNpcShopTabHit < gnNpcShopTabCount)
    {
        if (gNpcShopTabs[gnNpcShopTabHit].nHoverTextId != STR_IDX_10095_x_REPAIR_ALL_EQUIPMENT_FMT)
        {
            //  light every tab whose action matches, not just the one hit
            nWantedAction = gnNpcShopTabAction;
            //  the original's cursor is &[i].bPressed, with the action word read at +0Ah from it (0x6FAEACEA..0x6FAEAD02, cmp eax, 6FBB37FCh / jl):
            //  an integer cursor over our records, as in ITEMPANEL_ClearShopTabs
            nTab = (UINT_PTR)&gNpcShopTabs[0];
            do
            {
                ((D2InvButtonInfoStrc *)nTab)->bPressed = ((D2InvButtonInfoStrc *)nTab)->nBaseFrame == nWantedAction;
                nTab += sizeof(D2InvButtonInfoStrc);
            }
            while ((int32_t)nTab < (int32_t)((UINT_PTR)&gNpcShopTabs[0] + 4 * sizeof(D2InvButtonInfoStrc)));

            //  the stale-cursor read (0x6FAEAD0F): INV_NpcShopBuildTabs never writes slot 3's cursor on the merchant path
            nCursorMode = gNpcShopTabs[gnNpcShopTabHit].nCursorFrame;
            sgInventoryMode = gNpcShopTabs[gnNpcShopTabHit].nInventoryMode;
            CURSOR_SetCursorBuySell(nCursorMode, 0);
        }
    }

    if (!NPCMENU_GetInteractingNpc(&dwNpcGUID))
    {
        return;
    }
    pNpc = CUNIT_FindUnitByGuidAndType(dwNpcGUID, UNIT_MONSTER);
    if (!pNpc)
    {
        return;
    }

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    if (nMouseX >= gInvNpcShopGrid.nGridLeft && nMouseX <= gInvNpcShopGrid.nGridRight && nMouseY >= gInvNpcShopGrid.nGridTop && nMouseY <= gInvNpcShopGrid.nGridBottom)
    {
        INV_PixelToGridCell(pNpc, pNpc->pInventory, NULL, nMouseX, nMouseY, &gInvNpcShopGrid, (D2C_ItemInvPage)gnVendorPage);
        gnNpcTradeMode = 0;
    }
}

//1.10f: D2Client.0x6FAEADA0
//  Anvil panel, WM_MOUSEMOVE: hands the move to the inventory if it lands there. It records the cursor position either way and breaks nothing: a
//  move that misses the inventory rect returns 0 and travels on (its button-down sibling does the opposite). Called only from
//  USEITEM_AnvilOnMouseMove, and only while gnAnvilState == 1.
int32_t __stdcall INV_AnvilForwardMouseMoveToInventory(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    gnUiMouseX = (uint16_t)nXpos;
    gnUiMouseY = (uint16_t)nYpos;
    if ((uint16_t)nXpos < gInvPanelRect.nLeft || nXpos >= gInvPanelRect.nRight || nYpos < gInvPanelRect.nTop || nYpos > gInvPanelRect.nBottom)
    {
        return 0;
    }

    INV_OnMouseMove(pMsg);
    return 1;
}

//1.10f: D2Client.0x6FAEAE00
//  Anvil panel, WM_LBUTTONDOWN: swallows the click, then forwards it if it landed on the inventory. The chain is broken before the rect test, so
//  the anvil panel eats every left click while it is up and only clicks inside the inventory reach INV_OnLButtonDown. Called only from
//  USEITEM_AnvilOnButtonDown.
int32_t __stdcall INV_AnvilForwardLButtonDownToInventory(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    gnUiMouseX = (uint16_t)nXpos;
    gnUiMouseY = (uint16_t)nYpos;
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
    if ((uint16_t)nXpos < gInvPanelRect.nLeft || nXpos >= gInvPanelRect.nRight || nYpos < gInvPanelRect.nTop || nYpos > gInvPanelRect.nBottom)
    {
        return 0;
    }

    INV_OnLButtonDown(pMsg);
    return 1;
}

//1.10f: D2Client.0x6FAEAE70
//  Anvil panel, WM_LBUTTONUP over the inventory: closes the panel if the press and the release both landed on the inventory's close button. The
//  ordinary press-and-release contract is latched through gbInvCloseBtnArmed, and the latch is disarmed on both exits, so a release anywhere
//  cancels a press.
//  The close box is 32x32, measured from the bottom-right of the screen: x in [gnScreenWidth - gnBorderXDifference - 302, ... - 270], y in
//  [gnScreenHeight + gnBorderYDifference - 96, ... - 64]. D2ExpRes patches this site for 1024x768 and 1280x768 as InventoryCloseButtonRelease6;
//  replacing the function wholesale disables that patch.
//  The return is real (used in USEITEM_AnvilOnButtonUp), and the chain is broken twice on the hit path.
int32_t __stdcall INV_AnvilOnInvCloseButtonRelease(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    gnUiMouseX = (uint16_t)nXpos;
    gnUiMouseY = (uint16_t)nYpos;
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    if ((uint16_t)nXpos < gInvPanelRect.nLeft || nXpos >= gInvPanelRect.nRight || nYpos < gInvPanelRect.nTop || nYpos > gInvPanelRect.nBottom)
    {
        gbInvCloseBtnArmed = 0;
        return 0;
    }

    if (nXpos >= (gnScreenWidth - gnBorderXDifference - 302) && nXpos <= (gnScreenWidth - gnBorderXDifference - 270) && nYpos >= (gnBorderYDifference + gnScreenHeight - 96)
        && nYpos <= (gnBorderYDifference + gnScreenHeight - 64))
    {
        if (gbInvCloseBtnArmed)
        {
            //  BUG: the chain is broken a second time on this path, with bReturn and lResult set again to the values they already hold.
            //  SMsgBreakHandlerChain links a new node into a global list on every call, so two calls allocate two nodes. NOT ESTABLISHED: whether
            //  a duplicate node changes anything downstream. Reproduced.
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
            STORM_SMsgBreakHandlerChain(pMsg);
            USEITEM_AnvilCloseOnInteractionStop();
        }
    }

    gbInvCloseBtnArmed = 0;
    return 1;
}

//1.10f: D2Client.0x6FAEAF40
//  Anvil panel, WM_LBUTTONDOWN on the inventory's close button: arms the latch. The same 32x32 box as the release handler; D2ExpRes patches this
//  site too, as InventoryCloseButtonClickdownRelease.
//  __fastcall, not __stdcall like the other three anvil functions: it ends in retn 0 (argument size 0) and both call sites, in
//  USEITEM_AnvilOnButtonDown and USEITEM_AnvilOnButtonUp, do mov ecx, esi rather than pushing. Its return is used at both call sites, and the chain
//  is broken twice on the hit path.
int32_t __fastcall INV_AnvilOnInvCloseButtonPress(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    //  all four bounds are signed (cmp edi, ecx / jl at 0x6FAEAF78..0x6FAEAF7A on the zero-extended word, then jg, jl, jg); an unsigned first
    //  compare would give the same answer while the bound is positive, which it is for both resolutions (258..498)
    if (nXpos < (gnScreenWidth - gnBorderXDifference - 302) || nXpos > (gnScreenWidth - gnBorderXDifference - 270) || nYpos < (gnBorderYDifference + gnScreenHeight - 96)
        || nYpos > (gnBorderYDifference + gnScreenHeight - 64))
    {
        return 0;
    }

    gbInvCloseBtnArmed = 1;
    //  the second break on this path, as in INV_AnvilOnInvCloseButtonRelease
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
    return 1;
}

//1.10f: D2Client.0x6FAEAFD0
//  Loads the vendor bar's three images (from UI_InitUiSubsystems): data\global\ui\panel\buyselltabs -> gpImgBuySellTabs, miniconvert ->
//  gpImgMiniConvert, invchar6tab -> gpImgInvChar6Tab. The third is gated twice, FOG_IsExpansion() for the installation and
//  GAME_IsExpansionGame() for the game being joined, so a classic character in an expansion install does not get the six-tab bar; the six-tab
//  variant is the expansion's. Each image has its own if (!handle) guard.
//  gpImgInvChar6Tab is not gpImgInvChar6TabCube, which INV_UnloadPanelImages frees: two globals hold the same Panel\invchar6Tab file, loaded by
//  two panels.
void __cdecl INV_NpcShopLoadTabImages(void)
{
    char szPath[260];

    if (!gpImgBuySellTabs)
    {
        STORM_SStrPrintf(szPath, 0x104u, "%s\\ui\\panel\\buyselltabs", aDataGlobalInv);
        gpImgBuySellTabs = ARCHIVE_CellFileLoad(szPath, 0);
    }
    if (!gpImgMiniConvert)
    {
        STORM_SStrPrintf(szPath, 0x104u, "%s\\ui\\panel\\miniconvert", aDataGlobalInv);
        gpImgMiniConvert = ARCHIVE_CellFileLoad(szPath, 0);
    }
    if (!gpImgInvChar6Tab && FOG_IsExpansion())
    {
        if (GAME_IsExpansionGame())
        {
            STORM_SStrPrintf(szPath, 0x104u, "%s\\ui\\panel\\invchar6tab", aDataGlobalInv);
            gpImgInvChar6Tab = ARCHIVE_CellFileLoad(szPath, 0);
        }
    }
}

//1.10f: D2Client.0x6FAEB090
//  Frees the four images the vendor and trade panels use (from UI_ShutdownUiSubsystems, alongside INV_UnloadPanelImages). The expansion check
//  guards gpImgInvChar6Tab only, since the sixth character tab exists only in Lord of Destruction. gpImgInvChar6Tab is not
//  gpImgInvChar6TabCube, which INV_UnloadPanelImages frees: two globals hold the same Panel\invchar6Tab file.
void __cdecl INV_UnloadShopPanelImages(void)
{
    if (gpImgTradePanel)
    {
        ARCHIVE_CellFileFree(gpImgTradePanel);
        gpImgTradePanel = NULL;
    }
    if (gpImgBuySellTabs)
    {
        ARCHIVE_CellFileFree(gpImgBuySellTabs);
        gpImgBuySellTabs = NULL;
    }
    if (gpImgMiniConvert)
    {
        ARCHIVE_CellFileFree(gpImgMiniConvert);
        gpImgMiniConvert = NULL;
    }
    if (GAME_IsExpansionGame())
    {
        if (gpImgInvChar6Tab)
        {
            ARCHIVE_CellFileFree(gpImgInvChar6Tab);
            gpImgInvChar6Tab = NULL;
        }
    }
}

//1.10f: D2Client.0x6FAEB100
//  Switches the right-hand panel between its trade layout and its normal one. bTradeMode: sgInventoryMode = 11, SCROLL_SetWindowMode(PANEL_BOTH)
//  and panel\trade loaded into gpImgTradePanel on first use; otherwise sgInventoryMode = 10, SCROLL_SetWindowMode(PANEL_NONE). The argument of
//  SCROLL_SetWindowMode is a D2C_UIpanels value (how far the viewport slides to make room), not a mode number.
//  It opens by playing sound 3 if either accept flag is set, and clears seven globals in all, so a mode switch always cancels a pending trade.
void __fastcall INV_TradeSetPanelMode(int32_t bTradeMode)
{
    char szPath[260];

    if (gbTradeOwnAccepted || gbTradeOtherAccepted)
    {
        SOUNDINST_PlaySound(SOUND_CURSOR_ERROR, NULL, 0, 0, 0);
    }
    gbTradeOwnAccepted = 0;
    gbTradeOtherAccepted = 0;
    gbDropGoldBtnPressed = 0;

    if (bTradeMode)
    {
        gnLastItemUseTick = 0;
        sgInventoryMode = 11;
        SCROLL_SetWindowMode(PANEL_BOTH);
        if (!gpImgTradePanel)
        {
            STORM_SStrPrintf(szPath, 0x104u, "%s\\ui\\panel\\trade", aDataGlobalInv);
            gpImgTradePanel = ARCHIVE_CellFileLoad(szPath, 0);
        }
    }
    else
    {
        sgInventoryMode = 10;
        SCROLL_SetWindowMode(PANEL_NONE);
    }

    gbPanelBtnPressActive = 0;
    gbTradeAcceptChecked = 0;
    gbTradeCancelPending = 0;
    gbTradeAcceptBtnArmed = 0;
    gbTradeCancelBtnArmed = 0;
}

//  The panel-state accessors (0x6FAEB8B0..0x6FAEB9E6): seven small functions. INV_GetTransactionItemIsPlayers, INV_IsPanelModeNormal and
//  INV_IsInventoryMode4 return answers that callers read; the two gold-dialog guards, INV_ClearGoldDialogGuard and INV_ArmGoldDialogGuard, are void.
//  The gold-dialog guards are a pair: INV_ClearGoldDialogGuard sets the flag to 0 and maps mode 13 to 12; INV_ArmGoldDialogGuard sets it to 1, maps
//  13 to 12 and closes the chat box on the way in. The guard is what the gold-dialog openers test before opening a dialog (INV_OnLButtonUp,
//  INV_TradeOnLButtonUp, INV_StashOnLButtonUp twice and INV_CubeOnLButtonUp).
//  INV_SetHoverEnabled's polarity is the opposite of the obvious one: DIALOG_Create and DIALOG_CreateFromImage call it with 0 (disable) and
//  DIALOG_Close with 1 (enable), so a modal dialog going up suppresses inventory tooltips and wipes the one on screen, and closing lets them back.
//  gbInvHoverEnabled's initialiser is 1. The argument is a flag in one register (argument size 0, a bare retn).

//1.10f: D2Client.0x6FAEB8B0
//  Plays a sound when either trade accept flag changes, then stores both: an edge detector. It compares its two arguments with the two flags they
//  are about to replace: sound 3 when a side that had accepted no longer does, sound 1 when a side that had not accepted now does, silence when
//  nothing changed. All call sites are in TRADE_HandleStateMessage, which re-derives both flags after every state change. Written with the
//  original's branch structure: the two gotos are the compiler's shared arms, and flattening them into two independent tests changes which
//  comparisons run.
void __fastcall INV_UpdateToggleSounds(int32_t bFirst, int32_t bSecond)
{
    D2C_Sounds nSoundId;

    if (!gbTradeOwnAccepted && !gbTradeOtherAccepted)
    {
        goto CHECK_FIRST;
    }
    if (!bFirst || !bSecond)
    {
        nSoundId = SOUND_CURSOR_ERROR;
        goto PLAY;
    }
    if (!gbTradeOwnAccepted)
    {
CHECK_FIRST:
        if (bFirst)
        {
            goto SOUND_ONE;
        }
    }
    if (bSecond && !gbTradeOtherAccepted)
    {
SOUND_ONE:
        nSoundId = SOUND_CURSOR_PASS;
PLAY:
        SOUNDINST_PlaySound(nSoundId, NULL, 0, 0, 0);
    }

    gbTradeOwnAccepted = bFirst;
    gbTradeOtherAccepted = bSecond;
}

//1.10f: D2Client.0x6FAEB910
//  True when sgInventoryMode is 1, 2 or 3. Mode 1 is the idle value (INV_ResetItemGrids, INV_ResetPanelState, INV_LeavePanelMode5, INV_OnRButtonDown
//  and INV_NpcShopApplyTabSelection all write it), against 5 for an item on the cursor, 6 and 7 for a pending transaction, 12 for the gold dialog's
//  and 19 for the anvil; so the group is "no transaction in progress". What distinguishes 2 and 3 from 1 is not established.
//  One caller, PANEL_DrawBeltItemHint, which uses it to decide whether the belt hint gets its extra line.
BOOL __cdecl INV_IsPanelModeNormal(void)
{
    return sgInventoryMode == 1 || sgInventoryMode == 2 || sgInventoryMode == 3;
}

//1.10f: D2Client.0x6FAEB930
//  return sgInventoryMode == 4. The 4 stays in the name because no assert names value 4, unlike 5 (BUYSELL_VERIFY) and 7
//  (BUYSELL_DISPLAYING_RESULTS). Both callers are in the NPCMENU transaction path.
BOOL __cdecl INV_IsInventoryMode4(void)
{
    return sgInventoryMode == 4;
}

//1.10f: D2Client.0x6FAEB940
//  Returns gbTransactionItemIsPlayers. INV_NpcShopOnLButtonUp records the bPlayerOwnsItem argument it is about to hand to NPCMENU_BeginTransaction,
//  and the readers are on the far side of a server round trip, where the click's own argument is gone.
int32_t __cdecl INV_GetTransactionItemIsPlayers(void)
{
    return gbTransactionItemIsPlayers;
}

//1.10f: D2Client.0x6FAEB950
//  Drops the gold dialog's re-entry guard and maps mode 13 back to 12, the same mapping INV_ArmGoldDialogGuard applies on the way in. Its one
//  site is a tail jump in UI_CloseGoldEntryDialog (0x6FB23B1C).
void __cdecl INV_ClearGoldDialogGuard(void)
{
    gbGoldDialogOpen = 0;
    if (sgInventoryMode == 13)
    {
        sgInventoryMode = 12;
    }
}

//1.10f: D2Client.0x6FAEB970
//  Raises the guard, maps mode 13 to 12 and closes the chat box: UI_SetUIToggle(5, 1, 0) is UI_SetUIToggle(UI_CHATBOX, UI_TURNOFF, 0). All four
//  callers are switch arms of UI_OpenGoldEntryDialog (cases 1 to 4) and every one discards the result. It takes no arguments, so the __cdecl
//  written here is C++'s default made explicit.
void __cdecl INV_ArmGoldDialogGuard(void)
{
    gbGoldDialogOpen = 1;
    if (sgInventoryMode == 13)
    {
        sgInventoryMode = 12;
    }
    UI_SetUIToggle(UI_CHATBOX, UI_TURNOFF, 0);
}

//1.10f: D2Client.0x6FAEB9A0
//  Enables or disables the inventory's hover description. On disable it clears the same ten globals as INV_ResetItemDescState in a slightly
//  different order (gbShowItemDesc first, gbShowSetItemDesc after the two grid cells), which is the original's order. The polarity is the
//  opposite of what the call sites suggest at a glance: DIALOG_Create and DIALOG_CreateFromImage pass 0 and DIALOG_Close passes 1.
void __fastcall INV_SetHoverEnabled(int32_t bEnabled)
{
    gbInvHoverEnabled = bEnabled;
    if (!bEnabled)
    {
        gbShowItemDesc = 0;
        gnDescPopupX = -1;
        gnDescPopupY = -1;
        gnDescPopupAltX = -1;
        gnDescPopupBottomY = -1;
        gnHoverCellX = -1;
        gnHoverCellY = -1;
        gbShowSetItemDesc = 0;
        gpHoverItem = NULL;
        gpHoverItemLast = 0;
    }
}

//  The ten UI-directory path strings this unit loads (0x6FADE8A0..0x6FADE934).
static const char aPanelInvArmor[]     = "Panel\\inv_armor";
static const char aPanelInvBelt[]      = "Panel\\inv_belt";
static const char aPanelInvBoots[]     = "Panel\\inv_boots";
static const char aPanelInvHelmGl[]    = "Panel\\inv_helm_glove";
static const char aPanelInvRingAm[]    = "Panel\\inv_ring_amulet";
static const char aPanelInvWeapon[]    = "Panel\\inv_weapons";
static const char aPanelTradebtn[]     = "Panel\\tradebtn";
static const char aPanelGoldcoinb[]    = "Panel\\goldcoinbtn";
static const char aPanelInvchar6t[]    = "Panel\\invchar6Tab";
static const char aPanelNpcinv[]       = "Panel\\NPCInv";

//1.10f: D2Client.0x6FADD2F0
//  Mixes the five inventory highlight colours into palette indices: red, green, blue, yellow and a dull red, each at half intensity except the
//  last, which is (0x80, 0x40, 0x40). They land in five consecutive bytes at 0x6FBB3778 (gInvHighlightColors). It runs three times
//  (UI_LoadScreenBegin, UI_UnloadLoadingScreen and UI_CachePaletteIndices) because a palette index is only valid for the palette it was mixed
//  against; all three callers follow it with PANEL_SetBoxFillColorRed, so the box-fill colours are re-cached together with these.
void __cdecl INV_InitHighlightColors(void)
{
    gInvHighlightColors[0] = D2WIN_10034_MixRGB(0x80u, 0, 0);
    gInvHighlightColors[1] = D2WIN_10034_MixRGB(0, 0x80u, 0);
    gInvHighlightColors[2] = D2WIN_10034_MixRGB(0, 0, 0x80u);
    gInvHighlightColors[3] = D2WIN_10034_MixRGB(0x80u, 0x80u, 0);
    gInvHighlightColors[4] = D2WIN_10034_MixRGB(0x80u, 0x40u, 0x40u);
}

//1.10f: D2Client.0x6FADE8A0
//  Loads the inventory panel's ten cels: eight unconditionally and two behind both expansion tests, FOG_IsExpansion() for the installation and
//  GAME_IsExpansionGame() for the game being joined. gnWeaponSwapSet = 0 is written on both arms of that test, which is why it is not hoisted out
//  here: the bytes have it twice.
void __cdecl INV_LoadPanelImages(void)
{
    gpImgInvArmor = UI_LoadImageFromUiDir(aPanelInvArmor);
    gpImgInvBelt = UI_LoadImageFromUiDir(aPanelInvBelt);
    gpImgInvBoots = UI_LoadImageFromUiDir(aPanelInvBoots);
    gpImgInvHelmGlove = UI_LoadImageFromUiDir(aPanelInvHelmGl);
    gpImgInvRingAmulet = UI_LoadImageFromUiDir(aPanelInvRingAm);
    gpImgInvWeapons = UI_LoadImageFromUiDir(aPanelInvWeapon);
    gpImgTradeButton = UI_LoadImageFromUiDir(aPanelTradebtn);
    gpImgGoldCoinButton = UI_LoadImageFromUiDir(aPanelGoldcoinb);

    if (FOG_IsExpansion() && GAME_IsExpansionGame())
    {
        gpImgInvChar6TabCube = UI_LoadImageFromUiDir(aPanelInvchar6t);
        gnWeaponSwapSet = 0;
        gpImgNpcInv = UI_LoadImageFromUiDir(aPanelNpcinv);
    }
    else
    {
        gpImgInvChar6TabCube = NULL;
        gnWeaponSwapSet = 0;
        gpImgNpcInv = NULL;
    }
}

//1.10f: D2Client.0x6FADE970
//  Frees them again (from UI_ShutdownUiSubsystems). Twelve handles, each behind its own null check, and eight of them are NULLed a second time in
//  the run at the end (harmless). Two of the twelve are not this loader's: gpHoradricAnimImage, which INV_CubeStartQuestItemAnim loads lazily,
//  and gpRecipeScrollImage, which belongs to the recipe scroll.
//  The expansion pair is asymmetric between loader and unloader: the loader writes gnWeaponSwapSet = 0 on both arms of its expansion test and
//  NULLs the two expansion handles on the classic arm; the unloader tests the same pair again before freeing gpImgInvChar6TabCube but frees
//  gpImgNpcInv unconditionally. Reproduced.
void __cdecl INV_UnloadPanelImages(void)
{
    if (gpHoradricAnimImage)
    {
        ARCHIVE_CellFileFree(gpHoradricAnimImage);
        gpHoradricAnimImage = NULL;
    }
    if (gpImgInvArmor)
    {
        ARCHIVE_CellFileFree(gpImgInvArmor);
        gpImgInvArmor = NULL;
    }
    if (gpImgInvBelt)
    {
        ARCHIVE_CellFileFree(gpImgInvBelt);
        gpImgInvBelt = NULL;
    }
    if (gpImgInvBoots)
    {
        ARCHIVE_CellFileFree(gpImgInvBoots);
        gpImgInvBoots = NULL;
    }
    if (gpImgInvHelmGlove)
    {
        ARCHIVE_CellFileFree(gpImgInvHelmGlove);
        gpImgInvHelmGlove = NULL;
    }
    if (gpImgInvRingAmulet)
    {
        ARCHIVE_CellFileFree(gpImgInvRingAmulet);
        gpImgInvRingAmulet = NULL;
    }
    if (gpImgInvWeapons)
    {
        ARCHIVE_CellFileFree(gpImgInvWeapons);
        gpImgInvWeapons = NULL;
    }
    if (gpImgTradeButton)
    {
        ARCHIVE_CellFileFree(gpImgTradeButton);
        gpImgTradeButton = NULL;
    }
    if (gpImgGoldCoinButton)
    {
        ARCHIVE_CellFileFree(gpImgGoldCoinButton);
        gpImgGoldCoinButton = NULL;
    }
    if (FOG_IsExpansion() && GAME_IsExpansionGame() && gpImgInvChar6TabCube)
    {
        ARCHIVE_CellFileFree(gpImgInvChar6TabCube);
        gpImgInvChar6TabCube = NULL;
    }
    if (gpImgNpcInv)
    {
        ARCHIVE_CellFileFree(gpImgNpcInv);
        gpImgNpcInv = NULL;
    }
    if (gpRecipeScrollImage)
    {
        ARCHIVE_CellFileFree(gpRecipeScrollImage);
        gpRecipeScrollImage = NULL;
    }

    //  the second run: eight of the twelve, already NULL by here
    gpImgInvArmor = NULL;
    gpImgInvBelt = NULL;
    gpImgInvBoots = NULL;
    gpImgInvHelmGlove = NULL;
    gpImgInvRingAmulet = NULL;
    gpImgInvWeapons = NULL;
    gpImgTradeButton = NULL;
    gpImgGoldCoinButton = NULL;
}

//1.10f: D2Client.0x6FAE1710
//  Fills a rectangle of grid cells with a highlight colour: the green or red overlay under an item being placed. Cell to pixel is the panel's
//  own geometry, nGridLeft + cellX * nGridBoxWidth and nGridTop + cellY * nGridBoxHeight, and each box is clipped against gnViewportRight /
//  gnViewportBottom before it is drawn. The clip is per box, not per rectangle, so a partly off-screen highlight loses whole cells rather than
//  being trimmed. The outer test is the signed comparison the flags spell out (a plain <), so it is written as the comparison and not as
//  if (nHeight).
void __fastcall INV_DrawGridHighlight(D2InventoryGridInfoStrc *pGridInfo, int32_t nCellX, int32_t nCellY, uint8_t nWidth, uint8_t nHeight, uint32_t nColorIdx)
{
    int32_t nRowY;
    int32_t nRowEnd;
    int32_t nColEnd;
    int32_t iCol;
    int32_t nX;
    int32_t nY;
    int32_t nBoxHeight;

    nRowY = nCellY;
    nRowEnd = nCellY + nHeight;
    if (nCellY >= nRowEnd)
    {
        return;
    }

    nColEnd = nCellX + nWidth;
    do
    {
        for (iCol = nCellX; iCol < nColEnd; ++iCol)
        {
            nX = pGridInfo->nGridLeft + iCol * pGridInfo->nGridBoxWidth;
            nBoxHeight = pGridInfo->nGridBoxHeight;
            nY = pGridInfo->nGridTop + nRowY * nBoxHeight;
            if (nX >= 0 && nX < gnViewportRight && nY >= 0 && nY < gnViewportBottom)
            {
                GFX_DrawFilledRect(nX, nY, pGridInfo->nGridBoxWidth, nBoxHeight, gInvHighlightColors[nColorIdx], 0);
            }
        }
        ++nRowY;
    }
    while (nRowY < nRowEnd);
}

//1.10f: D2Client.0x6FAE17D0
//  May this gem go into that item? The client-side test, in the order the original checks it (the order matters because the last test is the
//  one a full item fails):
//      both pointers non-NULL
//      the panel mode is neither 10 nor 11
//      ITEMS_IsSocketFiller(pFiller)
//      pTarget is IFLAG_SOCKETED and ITEMS_CheckIfSocketable
//      ITEMS_GetMaxSockets(pTarget) is non-zero
//      pTarget is IFLAG_IDENTIFIED and not IFLAG_BROKEN
//      and finally the target's inventory holds fewer items than its socket count
//  The socket count is taken as a byte (ITEMS_GetSockets returns uint8_t) and compared with an int item count, so the comparison is between
//  0..255 and a signed count.
BOOL __fastcall INV_CanInsertSocketFiller(D2UnitStrc *pFiller, D2UnitStrc *pTarget)
{
    D2InventoryStrc *pInventory;
    uint32_t nSockets;

    if (!pFiller)
    {
        return 0;
    }
    if (!pTarget)
    {
        return 0;
    }
    if (sgInventoryMode == 10)
    {
        return 0;
    }
    if (sgInventoryMode == 11)
    {
        return 0;
    }
    if (!D2COMMON_ITEMS_IsSocketFiller(pFiller))
    {
        return 0;
    }
    if (!D2COMMON_ITEMS_CheckItemFlag(pTarget, IFLAG_SOCKETED, 1482, szInvCpp))
    {
        return 0;
    }
    if (!D2COMMON_ITEMS_CheckIfSocketable(pTarget))
    {
        return 0;
    }
    if (!D2COMMON_ITEMS_GetMaxSockets(pTarget))
    {
        return 0;
    }
    if (!D2COMMON_ITEMS_CheckItemFlag(pTarget, IFLAG_IDENTIFIED, 1488, szInvCpp))
    {
        return 0;
    }
    if (D2COMMON_ITEMS_CheckItemFlag(pTarget, IFLAG_BROKEN, 1490, szInvCpp))
    {
        return 0;
    }

    pInventory = pTarget->pInventory;
    if (!pInventory)
    {
        return 0;
    }
    nSockets = (uint8_t)D2COMMON_ITEMS_GetSockets(pTarget);
    return (uint32_t)D2COMMON_INVENTORY_GetItemCount(pInventory) < nSockets;
}

//1.10f: D2Client.0x6FAE5C50
//  sets.txt by row, bounds-checked against sgptDataTables (D2Common's exported table pointer).
D2SetsTxt *__fastcall INV_DataTblsGetSetsTxtRecord(int32_t nSetId)
{
    if (nSetId < 0 || nSetId >= sgptDataTables->nSetsTxtRecordCount)
    {
        return NULL;
    }
    return &sgptDataTables->pSetsTxt[nSetId];
}

//  DATATBLS_GetSetItemsTxtRecord (0x6FAE5C80) is written in Shared.cpp.

//  D2Common's own header string, pushed by every ITEMS_GetDimensions call site in this unit (0x6FB79840).
static const char szD2CommonCmnItemsH[] = "..\\D2Common/CmnItems.h";

//1.10f: D2Client.0x6FAE5990
//  Finds the equipped piece of the same set, for the description's set bonus. Walks the inventory for a set-quality item that is on the body
//  (page 0, 3, 4 or 0xFF and node page 1, 3 or 4) whose setitems.txt row's own wSetItemId equals the id asked for; the match is on the record's
//  field, not on the index it was fetched with. The if (pSetItemsTxt) after the bounds check is the compiler's null test on a pointer computed
//  from a base that can be null, not a real possibility of a missing row.
D2UnitStrc *__fastcall INV_FindComparableEquippedItem(D2InventoryStrc *pInventory, int32_t nFileIndex)
{
    D2UnitStrc *pNode;
    D2UnitStrc *pItem;
    uint8_t nInvPage;
    int32_t nNodePage;
    int32_t nFileIdx;
    D2SetItemsTxt *pSetItemsTxt;

    pNode = D2COMMON_INVENTORY_GetFirstItem(pInventory);
    if (!pNode)
    {
        return NULL;
    }

    for (;;)
    {
        pItem = D2COMMON_INVENTORY_UnitIsItem(pNode);
        if (D2COMMON_ITEMS_GetItemQuality(pItem) == ITEMQUAL_SET)
        {
            nInvPage = D2COMMON_ITEMS_GetInvPage(pItem);
            if (!nInvPage || nInvPage == 3 || nInvPage == 4 || nInvPage == 0xFF)
            {
                nNodePage = D2COMMON_INVENTORY_GetItemNodePage(pNode);
                if (nNodePage == 1 || nNodePage == 3 || nNodePage == 4)
                {
                    nFileIdx = D2COMMON_ITEMS_GetFileIndex(pItem);
                    if (nFileIdx >= 0 && nFileIdx < sgptDataTables->nSetItemsTxtRecordCount)
                    {
                        pSetItemsTxt = &sgptDataTables->pSetItemsTxt[nFileIdx];
                        if (pSetItemsTxt)
                        {
                            if ((int16_t)pSetItemsTxt->wSetItemId == nFileIndex)
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }

        pNode = D2COMMON_INVENTORY_GetNextItem(pNode);
        if (!pNode)
        {
            return NULL;
        }
    }

    return pItem;
}

//  DATATBLS_ApplyRatio (0x6FAE5BD0) is written in Shared.cpp.

//1.10f: D2Client.0x6FAE0DC0
//  Draws the socket holes and whatever is in them. Only for an identified item (inv.cpp:1118). The socket positions come out of gSocketLayouts,
//  indexed [96 * nSockets - 156 + 48 * nWidth + 12 * nHeight]: the stride is 96 ints per socket count, 48 per item width and 12 per item height,
//  and the -156 is the bias for the lowest socket count the table starts at (576 ints = 6 * 96, six socket counts). Each entry is a pair,
//  pnSocketLayout[2 * i] and [2 * i + 1], the socket's cell X and Y, and the pixel position is nX + (nGridBoxWidth * (cellX - 1)) / 2 and
//  nY + (nGridBoxHeight * (cellY + 1)) / 2 with a signed halve; the asymmetric -1 and +1 are the original's.
//  The gem art is drawn one grid row above the hole (nYPos - nGridBoxHeight) and the hole itself one pixel to the left (nX2 - 1); both offsets
//  are the original's. The items in the sockets are the item's own inventory, walked in order, so socket N gets the Nth child regardless of
//  which hole it is. ITEMS_GetSockets is called on every iteration, not hoisted, and on a second copy of the item pointer in the loop test.
void __fastcall INV_DrawSocketedItems(D2UnitStrc *pItem, D2InventoryGridInfoStrc *pGridInfo, int32_t nX, int32_t nY)
{
    D2UnitStrc *pSocketNode;
    uint8_t nSockets;
    int32_t iSocket;
    const int32_t *pnSocketLayout;   // into gSocketLayouts (.rdata)
    int32_t nSocketX;
    int32_t nSocketY;
    uint8_t nWidth;
    uint8_t nHeight;
    D2GfxDataStrc tData;

    if (!D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_IDENTIFIED, 1118, szInvCpp))
    {
        return;
    }

    pSocketNode = D2COMMON_INVENTORY_GetFirstItem(pItem->pInventory);
    D2COMMON_ITEMS_GetDimensions(pItem, &nWidth, &nHeight, szD2CommonCmnItemsH, 1754);
    nSockets = D2COMMON_ITEMS_GetSockets(pItem);
    pnSocketLayout = &gSocketLayouts[96 * nSockets - 156 + 48 * nWidth + 12 * nHeight];

    iSocket = 0;
    if (!(uint8_t)D2COMMON_ITEMS_GetSockets(pItem))
    {
        return;
    }

    do
    {
        //  `(x - (x >> 31)) >> 1` in the bytes: the signed halve, which rounds towards zero where `>> 1` alone rounds down
        nSocketX = pGridInfo->nGridBoxWidth * (pnSocketLayout[2 * iSocket] - 1);
        nSocketX = nX + ((nSocketX - (nSocketX >> 31)) >> 1);
        nSocketY = pGridInfo->nGridBoxHeight * (pnSocketLayout[2 * iSocket + 1] + 1);
        nSocketY = nY + ((nSocketY - (nSocketY >> 31)) >> 1);

        memset(&tData, 0, sizeof(tData));
        tData.nDirection = 0;
        tData.nFrame = 0;
        tData.pCellFile = gpImgGemSocket;
        D2GFX_TEXTURE_CelDraw(&tData, nSocketX - 1, nSocketY, 0xFFFFFFFF, 0, NULL);

        if (pSocketNode)
        {
            GFX_DrawInventoryItem(D2COMMON_INVENTORY_UnitIsItem(pSocketNode), nSocketX, nSocketY - pGridInfo->nGridBoxHeight);
            pSocketNode = D2COMMON_INVENTORY_GetNextItem(pSocketNode);
        }

        ++iSocket;
    }
    while ((uint32_t)iSocket < (uint32_t)(uint8_t)D2COMMON_ITEMS_GetSockets(pItem));
}

//1.10f: D2Client.0x6FAED310
//  Draws green or red under every mercenary slot the held item could go in. Does nothing unless the cursor is holding something. For each of
//  the eleven slots whose rect has a positive left edge, if the mouse is inside it the box is filled green when the item may be equipped there
//  and red otherwise, so the panel answers before the click.
//  Three tests have to pass for green: ITEMS_CheckBodyLocation for the slot, INV_MercInvCanMercEquipItem for the mercenary and
//  ITEMS_CheckRequirements for the stats. INV_MercInvCanMercEquipItem already calls CheckRequirements itself, so it is run twice on the green path.
//  ITEMS_GetDimensions is called and its two outputs are never read (the width and height go into a scratch). Reproduced.
void __fastcall INV_MercInvDrawSlotHighlights(D2UnitStrc *pMerc)
{
    D2UnitStrc *pCursorItem;
    D2InvCompGridStrc *pSlot;
    int32_t nBodyLoc;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t pbFits;
    uint8_t nWidth;
    uint8_t nHeight;
    uint32_t dwColor;

    pCursorItem = D2COMMON_INVENTORY_GetCursorItem(CUNIT_GetPlayer()->pInventory);
    if (!pCursorItem)
    {
        return;
    }

    nBodyLoc = 0;
    //  end-address sentinel: &gbTradeCancelPending + 1 against a cursor that starts at &gMercInvSlots[0].nHeight, offset 17 into the record
    //  (cmp esi, 6FBB5B25h / jl at 0x6FAED44F); the original reads the record's four rect fields at -17, -13, -9 and -5 from that cursor.
    //  Ours compares the record pointer with one past the array, the same compare less seventeen on both sides, signed as jl.
    for (pSlot = gMercInvSlots; (int32_t)pSlot < (int32_t)(gMercInvSlots + 11); ++pSlot, ++nBodyLoc)
    {
        if (pSlot->rect.nLeft <= 0)
        {
            continue;
        }

        nMouseX = CURSOR_GetMouseX();
        nMouseY = CURSOR_GetMouseY();
        if (nMouseX < pSlot->rect.nLeft || nMouseX > pSlot->rect.nRight || nMouseY < pSlot->rect.nTop || nMouseY > pSlot->rect.nBottom)
        {
            continue;
        }

        //  the two outputs are never read
        D2COMMON_ITEMS_GetDimensions(pCursorItem, &nWidth, &nHeight, szD2CommonCmnItemsH, 1754);
        pbFits = 0;
        if (D2COMMON_ITEMS_CheckBodyLocation(pCursorItem, (uint8_t)nBodyLoc) && INV_MercInvCanMercEquipItem(pMerc, pCursorItem, &pbFits) && D2COMMON_ITEMS_CheckRequirements(pCursorItem, pMerc, 0, NULL, NULL, NULL))
        {
            if (pSlot->rect.nLeft < 0 || pSlot->rect.nLeft >= gnViewportRight || pSlot->rect.nTop < 0 || pSlot->rect.nTop >= gnViewportBottom)
            {
                continue;
            }
            dwColor = gInvHighlightColors[1];
        }
        else
        {
            if (pSlot->rect.nLeft < 0 || pSlot->rect.nLeft >= gnViewportRight || pSlot->rect.nTop < 0 || pSlot->rect.nTop >= gnViewportBottom)
            {
                continue;
            }
            dwColor = gInvHighlightColors[0];
        }

        GFX_DrawFilledRect(pSlot->rect.nLeft, pSlot->rect.nTop, pSlot->nWidth, pSlot->nHeight, dwColor, 0);
    }
}

//1.10f: D2Client.0x6FAED470
//  May this mercenary wear that? pbFits is an out-parameter set alongside the answer, on exactly the paths that set the local, so it is a second
//  copy of the same bit. It switches on ROSTERPETS_GetMonStatsId: 271 BOW (the rogue); 338 SPEAR or POLEARM (the desert mercenary); 359 SHIELD or
//  a one-handed SWORD (the iron wolf); 560 one-handed AXE or PRIMAL_HELM (the barbarian); 561 SWORD or PRIMAL_HELM (the barbarian). ARMOR or HELM is
//  accepted for every one of them before the switch is reached. The whole function is gated on GAME_IsExpansionGame() (mercenaries carry no
//  equipment in classic) and on the item being identified and not broken (ITEMS_CheckItemFlag with inv.cpp lines 12274 and 12277).
//  338 is reached as MonStatsId - 271 == 67 and 561 as MonStatsId - 560 == 1: the compiler's range narrowing, not two different checks.
int32_t __fastcall INV_MercInvCanMercEquipItem(D2UnitStrc *pMerc, D2UnitStrc *pItem, int32_t *pbFits)
{
    int32_t bExpansion;
    int32_t bCanEquip;
    uint32_t dwUnitId;
    int32_t nMonStatsId;
    BOOL bTypeMatches;

    *pbFits = 0;
    bExpansion = GAME_IsExpansionGame();
    if (!bExpansion)
    {
        return bExpansion;
    }

    bCanEquip = 0;
    bExpansion = D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_IDENTIFIED, 12274, szInvCpp);
    if (!bExpansion)
    {
        return bExpansion;
    }
    if (D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_BROKEN, 12277, szInvCpp))
    {
        return 0;
    }

    if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_ARMOR) || D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_HELM))
    {
        bCanEquip = 1;
        *pbFits = 1;
    }

    dwUnitId = pMerc ? pMerc->dwUnitId : (uint32_t)-1;
    nMonStatsId = ROSTERPETS_GetMonStatsId(dwUnitId);

    if (nMonStatsId <= 359)
    {
        if (nMonStatsId == 359)
        {
            //  the iron wolf: a shield, or a one-handed sword
            if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_SHIELD))
            {
                bCanEquip = 1;
                *pbFits = 1;
            }
            if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_SWORD) && !D2COMMON_ITEMS_CheckWeaponIfTwoHanded(pItem))
            {
                goto ACCEPT;
            }
            goto TEST_CAN_EQUIP;
        }

        if (nMonStatsId - 271)
        {
            if (nMonStatsId - 271 != 67)
            {
                goto TEST_CAN_EQUIP;
            }
            //  the desert mercenary: a spear, or a polearm
            if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_SPEAR))
            {
                goto ACCEPT;
            }
            bTypeMatches = D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_POLEARM);
        }
        else
        {
            //  the rogue: a bow
            bTypeMatches = D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_BOW);
        }
        goto TEST_TYPE;
    }

    if (nMonStatsId - 560)
    {
        if (nMonStatsId - 560 != 1)
        {
            goto TEST_CAN_EQUIP;
        }
        if (!D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_SWORD))
        {
            goto TEST_PRIMAL;
        }
    }
    else if (!D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_AXE) || D2COMMON_ITEMS_CheckWeaponIfTwoHanded(pItem))
    {
        goto TEST_PRIMAL;
    }
    bCanEquip = 1;
    *pbFits = 1;

TEST_PRIMAL:
    //  at this site the variable holds a primal-helm test
    bTypeMatches = D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_PRIMAL_HELM);

TEST_TYPE:
    if (bTypeMatches)
    {
ACCEPT:
        bCanEquip = 1;
        *pbFits = 1;
        goto CHECK_REQUIREMENTS;
    }

TEST_CAN_EQUIP:
    if (!bCanEquip)
    {
        return bCanEquip;
    }

CHECK_REQUIREMENTS:
    if (!D2COMMON_ITEMS_CheckRequirements(pItem, pMerc, 0, NULL, NULL, NULL))
    {
        return 0;
    }
    return bCanEquip;
}

//1.10f: D2Client.0x6FAEE150
//  Draws the placeholder art in the mercenary's empty slots: torso, head and the two weapon arms. The arms interlock: with the left arm empty the
//  weapon placeholder is drawn there; with the right arm empty it is drawn there too, unless a two-handed weapon is in the other hand.
//  INVENTORY_GetWieldType(...) == 2 is the two-handed test, read once and used by both arms. A null inventory takes the same path as an empty one
//  for the torso and head (!pInventory || !GetItemFromBodyLoc(...)), but for the arms it is a separate branch that zeroes all three locals.
//  The two weapon arms share a tail, kept as a goto into the middle of the weapon arm. The D2GfxDataStrc is memset once at the top and reused
//  for all four draws, with only two fields rewritten each time.
void __fastcall INV_MercInvDrawEmptySlots(D2UnitStrc *pMerc)
{
    D2InventoryStrc *pInventory;
    D2UnitStrc *pRightArm;
    D2UnitStrc *pLeftArm;
    BOOL bTwoHanded;
    D2GfxDataStrc tData;

    pInventory = pMerc->pInventory;
    memset(&tData, 0, sizeof(tData));

    if (!pInventory || !D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_TORSO))
    {
        tData.pCellFile = gpImgInvArmor;
        tData.nFrame = 0;
        D2GFX_TEXTURE_CelDraw(&tData, gMercInvSlots[3].rect.nLeft + gnInvTorsoEmptyOffsetX, gMercInvSlots[3].rect.nBottom + gnInvTorsoEmptyOffsetY, 0xFFFFFFFF, 5, NULL);
    }

    if (!pInventory || !D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_HEAD))
    {
        tData.pCellFile = gpImgInvHelmGlove;
        tData.nFrame = 1;
        D2GFX_TEXTURE_CelDraw(&tData, gMercInvSlots[1].rect.nLeft + gnInvHeadEmptyOffsetX, gMercInvSlots[1].rect.nBottom + gnInvHeadEmptyOffsetY, 0xFFFFFFFF, 5, NULL);
    }

    if (pInventory)
    {
        pRightArm = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RARM);
        pLeftArm = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_LARM);
        bTwoHanded = D2COMMON_INVENTORY_GetWieldType(pMerc, pInventory) == 2;
        if (pLeftArm)
        {
            goto LEFT_DONE;
        }
    }
    else
    {
        pRightArm = NULL;
        pLeftArm = NULL;
        bTwoHanded = 0;
    }

    tData.pCellFile = gpImgInvWeapons;
    tData.nFrame = 0;
    if (pRightArm)
    {
        if (!bTwoHanded)
        {
            D2GFX_TEXTURE_CelDraw(&tData, gMercInvSlots[5].rect.nLeft + gnInvLArmEmptyOffsetX, gMercInvSlots[5].rect.nBottom + gnInvLArmEmptyOffsetY, 0xFFFFFFFF, 5, NULL);
        }
LEFT_DONE:
        if (pRightArm)
        {
            return;
        }
        goto RIGHT_ARM;
    }

    D2GFX_TEXTURE_CelDraw(&tData, gMercInvSlots[5].rect.nLeft + gnInvLArmEmptyOffsetX, gMercInvSlots[5].rect.nBottom + gnInvLArmEmptyOffsetY, 0xFFFFFFFF, 5, NULL);

RIGHT_ARM:
    tData.pCellFile = gpImgInvWeapons;
    tData.nFrame = 0;
    if (!pLeftArm || !bTwoHanded)
    {
        D2GFX_TEXTURE_CelDraw(&tData, gMercInvSlots[4].rect.nLeft + gnInvRArmEmptyOffsetX, gMercInvSlots[4].rect.nBottom + gnInvRArmEmptyOffsetY, 0xFFFFFFFF, 5, NULL);
    }
}


//1.10f: D2Client.0x6FAE0A10
//  Rebuilds the inventory rectangles and grids from inventory.txt, but only when they are stale; seven callers, all on the way into something
//  that needs a panel rectangle. Two caches with a shared invalidator: a resolution change clears both gbInvLayoutBuilt and gbInvNpcLayoutBuilt,
//  because it invalidates every rectangle at once; otherwise each is checked independently.
//  The player's pass reads his own inventory record, then record 13 for the mercenary, then 6, 7, 8, 12, 9, 10 and 11 for a rect and a grid each:
//  Trade Page 1 and 2, the classic stash, the expansion stash, the cube, and the Guild Vault and Trophy Case that nothing reads back. Record 13 is
//  Hireling, not the row of that number: inventory.txt has an Expansion marker at data line 12, so every id from 12 up is one data line lower than
//  its row (id 12 is Big Bank Page 1, the expansion stash).
//  The mercenary's ten component grids land in slots 1..10 and nothing writes slot 0: the BODYLOC_NONE record is never touched by the loader, which
//  is why INV_MercInvDrawSlotHighlights skips it with an nLeft <= 0 guard rather than an index test. The component-to-slot order is permuted for
//  the first five (slot 4 <- component 0, slot 3 <- 1, slot 5 <- 2, slot 1 <- 3, slot 2 <- 4); slots 6..10 take components 5..9. The player's
//  own pass is permuted the same way into its five separately named rects.
//  The NPC pass is separate: it runs only while a shop panel is open and an NPC is being interacted with, and it fills the exchange rect and grid
//  from that NPC's inventory record, so the NPC layout is rebuilt per NPC and the player's per resolution.
//  It takes no arguments: the push ecx at the start only reserves a dword for dwNpcGUID, a local filled further down by NPCMENU_GetInteractingNpc.
void __cdecl INV_EnsurePanelLayout(void)
{
    D2UnitStrc *pNpc;
    int32_t nInvRecordId;
    int32_t bExpansion;
    D2UnitGUID dwNpcGUID;

    //  the original's shape: the if body falls into the rebuild and the second test jumps into it; kept, because flattening it into one
    //  condition reorders when gnInvLayoutRes is written
    if (gbHighResolution != gnInvLayoutRes)
    {
        gnInvLayoutRes = gbHighResolution;
        gbInvLayoutBuilt = 0;
        gbInvNpcLayoutBuilt = 0;
        goto REBUILD;
    }
    if (gbInvLayoutBuilt)
    {
        goto NPC_PASS;
    }

REBUILD:
    bExpansion = GAME_IsExpansionGame();
    nInvRecordId = D2COMMON_UNITS_GetInventoryRecordId(CUNIT_GetPlayer(), INVPAGE_INVENTORY, bExpansion);

    D2COMMON_DATATBLS_GetInventoryRect(nInvRecordId, gbHighResolution, &gInvPanelRect);
    D2COMMON_DATATBLS_GetInventoryGridInfo(nInvRecordId, gbHighResolution, &gInvPanelGrid);
    //  the permutation: component 0 goes to the right arm, 3 to the head
    D2COMMON_DATATBLS_GetInventoryComponentGrid(nInvRecordId, gbHighResolution, &gInvBodyLocRects[BODYLOC_RARM], 0);
    D2COMMON_DATATBLS_GetInventoryComponentGrid(nInvRecordId, gbHighResolution, &gInvBodyLocRects[3], 1);
    D2COMMON_DATATBLS_GetInventoryComponentGrid(nInvRecordId, gbHighResolution, &gInvBodyLocRects[BODYLOC_LARM], 2);
    D2COMMON_DATATBLS_GetInventoryComponentGrid(nInvRecordId, gbHighResolution, &gInvBodyLocRects[1], 3);
    D2COMMON_DATATBLS_GetInventoryComponentGrid(nInvRecordId, gbHighResolution, &gInvBodyLocRects[2], 4);
    D2COMMON_DATATBLS_GetInventoryComponentGrid(nInvRecordId, gbHighResolution, &gInvBodyLocRects[BODYLOC_RRIN], 5);
    D2COMMON_DATATBLS_GetInventoryComponentGrid(nInvRecordId, gbHighResolution, &gInvBodyLocRects[BODYLOC_LRIN], 6);
    D2COMMON_DATATBLS_GetInventoryComponentGrid(nInvRecordId, gbHighResolution, &gInvBodyLocRects[BODYLOC_BELT], 7);
    D2COMMON_DATATBLS_GetInventoryComponentGrid(nInvRecordId, gbHighResolution, &gInvBodyLocRects[BODYLOC_FEET], 8);
    D2COMMON_DATATBLS_GetInventoryComponentGrid(nInvRecordId, gbHighResolution, &gInvBodyLocRects[BODYLOC_GLOVES], 9);

    //  record 13 is Hireling, and its ten components land in slots 1..10
    D2COMMON_DATATBLS_GetInventoryRect(13, gbHighResolution, &gInvMercRect);
    D2COMMON_DATATBLS_GetInventoryGridInfo(13, gbHighResolution, &gInvMercGrid);
    D2COMMON_DATATBLS_GetInventoryComponentGrid(13, gbHighResolution, &gMercInvSlots[4], 0);
    D2COMMON_DATATBLS_GetInventoryComponentGrid(13, gbHighResolution, &gMercInvSlots[3], 1);
    D2COMMON_DATATBLS_GetInventoryComponentGrid(13, gbHighResolution, &gMercInvSlots[5], 2);
    D2COMMON_DATATBLS_GetInventoryComponentGrid(13, gbHighResolution, &gMercInvSlots[1], 3);
    D2COMMON_DATATBLS_GetInventoryComponentGrid(13, gbHighResolution, &gMercInvSlots[2], 4);
    D2COMMON_DATATBLS_GetInventoryComponentGrid(13, gbHighResolution, &gMercInvSlots[6], 5);
    D2COMMON_DATATBLS_GetInventoryComponentGrid(13, gbHighResolution, &gMercInvSlots[7], 6);
    D2COMMON_DATATBLS_GetInventoryComponentGrid(13, gbHighResolution, &gMercInvSlots[8], 7);
    D2COMMON_DATATBLS_GetInventoryComponentGrid(13, gbHighResolution, &gMercInvSlots[9], 8);
    D2COMMON_DATATBLS_GetInventoryComponentGrid(13, gbHighResolution, &gMercInvSlots[10], 9);

    D2COMMON_DATATBLS_GetInventoryRect(6, gbHighResolution, &gInvTradePage1Rect);
    D2COMMON_DATATBLS_GetInventoryGridInfo(6, gbHighResolution, &gInvTradePage1Grid);
    D2COMMON_DATATBLS_GetInventoryRect(7, gbHighResolution, &gInvTradePage2Rect);
    D2COMMON_DATATBLS_GetInventoryGridInfo(7, gbHighResolution, &gInvTradePage2Grid);
    D2COMMON_DATATBLS_GetInventoryRect(8, gbHighResolution, &gInvStashRect);
    D2COMMON_DATATBLS_GetInventoryGridInfo(8, gbHighResolution, &gInvStashGrid);
    D2COMMON_DATATBLS_GetInventoryRect(12, gbHighResolution, &gInvBigStashRect);
    D2COMMON_DATATBLS_GetInventoryGridInfo(12, gbHighResolution, &gInvBigStashGrid);
    D2COMMON_DATATBLS_GetInventoryRect(9, gbHighResolution, &gInvCubeRect);
    D2COMMON_DATATBLS_GetInventoryGridInfo(9, gbHighResolution, &gInvCubeGrid);
    D2COMMON_DATATBLS_GetInventoryRect(10, gbHighResolution, &gInvGuildVaultRect);
    D2COMMON_DATATBLS_GetInventoryGridInfo(10, gbHighResolution, &gInvGuildVaultGrid);
    D2COMMON_DATATBLS_GetInventoryRect(11, gbHighResolution, &gInvTrophyCaseRect);
    D2COMMON_DATATBLS_GetInventoryGridInfo(11, gbHighResolution, &gInvTrophyCaseGrid);

    gbInvLayoutBuilt = 1;

NPC_PASS:
    if (!gbInvNpcLayoutBuilt && sgInventoryMode && NPCMENU_GetInteractingNpc(&dwNpcGUID))
    {
        pNpc = CUNIT_FindUnitByGuidAndType(dwNpcGUID, UNIT_MONSTER);
        if (pNpc)
        {
            bExpansion = GAME_IsExpansionGame();
            nInvRecordId = D2COMMON_UNITS_GetInventoryRecordId(pNpc, INVPAGE_INVENTORY, bExpansion);
            D2COMMON_DATATBLS_GetInventoryRect(nInvRecordId, gbHighResolution, &gInvNpcShopRect);
            D2COMMON_DATATBLS_GetInventoryGridInfo(nInvRecordId, gbHighResolution, &gInvNpcShopGrid);
            gbInvNpcLayoutBuilt = 1;
        }
    }
}

//1.10f: D2Client.0x6FAE8C60
//  Right-click use of the item in a grid cell. The order of decisions:
//      1  resolve the item; nothing there, nothing to do
//      2  an empty cursor plus a transmogrifiable item returns 1 without sending anything, unless the item is locked (the "this item turns into
//         something" affordance, handled before ordinary use)
//      3  otherwise, if the item is useable, send the use command, refusing a locked item outright
//      4  if it is not useable but has a unique column, the three scroll codes open the Inifuss reader
//  The parameters after pInventory are nX, nY, nInvPage: nX is divided by a grid's box width, nY by its box height, and the fifth is copied
//  into a byte and used as the inventory page.
//  The scroll branch tests three packed codes, 'trs ' (544436852), 'bks ' (544435042, Bark Scroll) and 'bkd ' (543452002, deciphered Bark
//  Scroll), and all three open TEXT_IniScrollOpen, the Scroll of Inifuss reader.
//  BUG: 'trs ' is defined by no code column in weapons.txt, armor.txt or misc.txt. UNREACHABLE: that arm cannot run in an unmodified game (a mod that
//  gives an item the code trs gets the readable-scroll panel with no patching).
//  Using the cube ('box ', 0x20786F62, ITEMS_GetBaseCode == 544763746) while the stash is open sets gbCubeOpenedFromStash, which
//  INV_CubeOnPanelClose and INV_CubeOnDismissKey read to decide whether to re-issue the interaction with the stash on the way out.
//  Item use is rate-limited only in multiplayer: GAME_GetGameType() other than 0 or 1 (the two offline types) adds TRADE_TickTimeout() and a
//  250 ms cooldown on gnLastItemUseTick. The timeout arm sends the green check and returns 0, so a refused use is not silent.
//  The original inlines two coordinate accessors for the player's X and Y: each tests dwUnitType for UNIT_OBJECT or the MISSILE..TILE range before
//  choosing between the path's cached client coordinate and PATH_GetXPosition / PATH_GetYPosition.
int32_t __fastcall INV_UseItemAtGridCell(D2UnitStrc *pUnit, D2InventoryStrc *pInventory, int32_t nX, int32_t nY, D2C_ItemInvPage nInvPage)
{
    int32_t bExpansion;
    uint8_t nPage;
    uint32_t nGridX;
    uint32_t nRowOffset;
    uint32_t nGridBoxHeight;
    D2UnitStrc *pItem;
    D2C_UnitTypes nUnitType;
    int32_t nUnitX;
    int32_t nUnitY;
    D2DynamicPathStrc *pDynamicPath;
    D2C_Sounds nUseSound;
    D2ItemsTxt *pItemsTxt;
    uint32_t dwCode;
    int32_t nInventoryRecordId;
    int32_t pX;
    int32_t pY;

    bExpansion = GAME_IsExpansionGame();
    nPage = (uint8_t)nInvPage;
    nInventoryRecordId = D2COMMON_UNITS_GetInventoryRecordId(pUnit, nInvPage, bExpansion);

    if (nPage)
    {
        if (GAME_IsExpansionGame())
        {
            nGridX = (nX - gInvBigStashGrid.nGridLeft) / (uint32_t)gInvBigStashGrid.nGridBoxWidth;
            nRowOffset = nY - gInvBigStashGrid.nGridTop;
            nGridBoxHeight = gInvBigStashGrid.nGridBoxHeight;
        }
        else
        {
            nGridX = (nX - gInvStashGrid.nGridLeft) / (uint32_t)gInvStashGrid.nGridBoxWidth;
            nRowOffset = nY - gInvStashGrid.nGridTop;
            nGridBoxHeight = gInvStashGrid.nGridBoxHeight;
        }
    }
    else
    {
        nGridX = (nX - gInvPanelGrid.nGridLeft) / (uint32_t)gInvPanelGrid.nGridBoxWidth;
        nRowOffset = nY - gInvPanelGrid.nGridTop;
        nGridBoxHeight = gInvPanelGrid.nGridBoxHeight;
    }

    pItem = D2COMMON_INVENTORY_GetItemFromInventoryPage(pInventory, nGridX, nRowOffset / nGridBoxHeight, &pX, &pY, (D2C_InventoryRecords)nInventoryRecordId, nPage);
    if (!pItem)
    {
        return 0;
    }

    if (!D2COMMON_INVENTORY_GetCursorItem(pInventory) && D2COMMON_ITEMS_GetTransmogrify(pItem))
    {
        if (ITEM_IsItemLocked(pItem))
        {
            return 0;
        }
        MSG_SendServerItemMessage(76, pItem->dwUnitId);
        ITEM_SetUninterruptableState(pItem);
        return 1;
    }

    if (!D2COMMON_ITEMS_CheckIfUseable(pItem))
    {
        if (!D2COMMON_ITEMS_GetUniqueColumnFromItemsTxt(pItem))
        {
            return 0;
        }
        pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
        dwCode = pItemsTxt->dwCode;
        if (dwCode == 544436852 || dwCode == 544435042 || dwCode == 543452002)
        {
            TEXT_IniScrollOpen(pItem, pItemsTxt);
        }
        return 1;
    }

    //  the two inlined coordinate accessors
    nUnitType = pUnit->dwUnitType;
    //  signed, as the image compares it (jle, jle); no unit type 0..5 tells it from unsigned
    if (pUnit->dwUnitType == UNIT_OBJECT || (int32_t)nUnitType > (int32_t)UNIT_MISSILE && (int32_t)nUnitType <= (int32_t)UNIT_TILE)
    {
        nUnitX = pUnit->pDynamicPath->dwClientCoordY;
    }
    else if (pUnit->pDynamicPath)
    {
        nUnitX = D2COMMON_PATH_GetXPosition(pUnit->pDynamicPath);
    }
    else
    {
        nUnitX = 0;
    }

    nUnitType = pUnit->dwUnitType;
    //  signed, as the image compares it (jle, jle); no unit type 0..5 tells it from unsigned
    if (pUnit->dwUnitType == UNIT_OBJECT || (int32_t)nUnitType > (int32_t)UNIT_MISSILE && (int32_t)nUnitType <= (int32_t)UNIT_TILE)
    {
        //  a four-byte read at +0x10, which our struct names a D2PathPointStrc pair while D2Client uses it as the client Y
        //  (the same mismatch as UNITS_GetYPosition, where the X sits at +0x0C under a Y's name)
        nUnitY = *(int32_t *)&pUnit->pDynamicPath->tTargetCoord;
    }
    else
    {
        pDynamicPath = pUnit->pDynamicPath;
        nUnitY = pDynamicPath ? D2COMMON_PATH_GetYPosition(pDynamicPath) : 0;
    }

    if (ITEM_IsItemLocked(pItem))
    {
        return 0;
    }

    //  the rate limit, multiplayer only
    if (GAME_GetGameType() && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
    {
        if (TRADE_TickTimeout())
        {
            TRADE_SendGreenCheck();
            return 0;
        }
        if (GetTickCount() - gnLastItemUseTick <= 0xFA)
        {
            return 0;
        }
        gnLastItemUseTick = GetTickCount();
    }

    //  'box ': using the cube out of the stash arms the handshake INV_CubeOnPanelClose reads on the way out
    if (UI_GetUiWindowState(UI_STASH))
    {
        if (D2COMMON_ITEMS_GetBaseCode(pItem) == 544763746)
        {
            gbCubeOpenedFromStash = 1;
        }
    }

    MSG_SendCmdUnitParam(32, pItem->dwUnitId, nUnitX, nUnitY);
    ITEM_SetUninterruptableState(pItem);
    nUseSound = ITEM_GetUseSound(pItem);
    SOUNDINST_PlaySound(nUseSound, NULL, 0, 0, 0);
    gbShowItemDesc = 0;
    return 1;
}


//1.10f: D2Client.0x6FAE87C0
//  Pixel to grid cell, both directions. With pItem != NULL (the cursor is holding something) it works out which cell the item would land in,
//  centres it on the held item's size, clamps it inside the grid and records the cell for the placement highlight; with pItem == NULL it finds
//  the item under the cursor and records its description popup rectangle. Seven parameters: five stack arguments plus ecx and edx (retn 14h).
//  An item's grid cell lives in its path's client coordinates: the popup rectangle is built from
//  gnDescPopupX = nGridLeft + itemX * nGridBoxWidth + nGridBoxWidth * itemCellWidth / 2, where itemX comes from the same inlined coordinate
//  accessor pair the rest of this unit uses (dwClientCoordY at +0x0C for the X, the four bytes at +0x10 for the Y). So an item sitting in an
//  inventory grid keeps its cell in the path structure a walking unit uses for its map position.
//  The popup is rebuilt only when the hovered item differs from gpHoverItemLast; gbShowItemDesc = 1 outside that test keeps the popup on screen.
//  The centring is asymmetric: an even dimension shifts the pick point by a quarter of the cel size before dividing,
//  (nX + celW / 4 - nGridLeft) / nGridBoxWidth, and an odd one does not. A dimension equal to the whole grid pins the cell to the middle, and a
//  dimension above 1 subtracts half the item and clamps at 0 and at the far edge. The quarter is >> 2 and the halves are >> 1 in the bytes.
void __fastcall INV_PixelToGridCell(D2UnitStrc *pUnit, D2InventoryStrc *pInventory, D2UnitStrc *pItem, uint32_t nX, uint32_t nY, D2InventoryGridInfoStrc *pGridInfo, D2C_ItemInvPage nInvPage)
{
    int32_t bExpansion;
    D2C_InventoryRecords nInventoryRecordId;
    int32_t nGridX;
    int32_t nGridY;
    uint8_t nItemW;
    uint8_t nItemH;
    int32_t nCelW;
    int32_t nCelH;
    uint32_t nWidthForClamp;
    uint32_t nHeightForClamp;
    uint8_t nGridCellsX;
    uint8_t nGridCellsY;
    D2UnitStrc *pHovered;
    int32_t nItemX;
    int32_t nItemY;
    int32_t nUnitType;
    uint8_t nHoverW;
    uint8_t nHoverH;
    int32_t pX;
    int32_t pY;

    bExpansion = GAME_IsExpansionGame();
    nInventoryRecordId = (D2C_InventoryRecords)D2COMMON_UNITS_GetInventoryRecordId(pUnit, nInvPage, bExpansion);

    nGridX = (nX - pGridInfo->nGridLeft) / pGridInfo->nGridBoxWidth;
    nGridY = (nY - pGridInfo->nGridTop) / pGridInfo->nGridBoxHeight;

    if (pItem)
    {
        //  the four out-parameters the compiler put in argument slots
        D2COMMON_ITEMS_GetDimensions(pItem, &nItemW, &nItemH, szD2CommonCmnItemsH, 1754);
        GFXUTIL_GetUnitCelSize(pItem, 0, 0, &nCelW, &nCelH);

        nWidthForClamp = nItemW;
        if ((nItemW & 1) == 0)
        {
            nGridX = (nX + ((uint32_t)nCelW >> 2) - pGridInfo->nGridLeft) / pGridInfo->nGridBoxWidth;
        }
        nGridCellsX = pGridInfo->nGridX;
        if (nItemW == pGridInfo->nGridX)
        {
            nGridX = nGridCellsX >> 1;
        }

        nHeightForClamp = nItemH;
        if ((nItemH & 1) == 0)
        {
            nWidthForClamp = nItemW;
            nGridY = (nY + ((uint32_t)nCelH >> 2) - pGridInfo->nGridTop) / pGridInfo->nGridBoxHeight;
        }
        nGridCellsY = pGridInfo->nGridY;
        if (nItemH == nGridCellsY)
        {
            nGridY = nGridCellsY >> 1;
        }

        //  a width of 0 or 1 needs no centring; anything wider is pulled back by half itself and clamped at both edges
        if (nItemW > 1u)
        {
            nGridX -= nWidthForClamp >> 1;
            if (nGridX < 0)
            {
                nGridX = 0;
            }
            if ((int32_t)(nGridX + nWidthForClamp) > nGridCellsX)
            {
                return;
            }
        }

        if (nItemH > 1u)
        {
            nGridY -= nHeightForClamp >> 1;
            if (nGridY < 0)
            {
                nGridY = 0;
            }
            if ((int32_t)(nGridY + nHeightForClamp) > pGridInfo->nGridY)
            {
                return;
            }
        }

        gnHoverCellY = nGridY;
        gnHoverCellX = nGridX;
        gbShowItemDesc = 1;
        gpHoverItem = NULL;
        gpHoverItemLast = 0;
        return;
    }

    pHovered = D2COMMON_INVENTORY_GetItemFromInventoryPage(pInventory, nGridX, nGridY, &pX, &pY, nInventoryRecordId, (uint8_t)nInvPage);
    if (!pHovered)
    {
        gbShowItemDesc = 0;
        gpHoverItem = NULL;
        gpHoverItemLast = 0;
        return;
    }

    gbShowItemDesc = 1;
    if (pHovered == gpHoverItemLast)
    {
        return;
    }

    D2COMMON_ITEMS_GetDimensions(pHovered, &nHoverW, &nHoverH, szD2CommonCmnItemsH, 1754);

    //  the item's own grid cell, out of its path's client coordinates
    nUnitType = pHovered->dwUnitType;
    if (pHovered->dwUnitType == UNIT_OBJECT || nUnitType > 3 && nUnitType <= 5)
    {
        nItemX = pHovered->pDynamicPath->dwClientCoordY;
    }
    else if (pHovered->pDynamicPath)
    {
        nItemX = D2COMMON_PATH_GetXPosition(pHovered->pDynamicPath);
    }
    else
    {
        nItemX = 0;
    }

    nItemY = 0;
    nUnitType = pHovered->dwUnitType;
    if (pHovered->dwUnitType == UNIT_OBJECT || nUnitType > 3 && nUnitType <= 5)
    {
        nItemY = *(int32_t *)&pHovered->pDynamicPath->tTargetCoord;
    }
    else if (pHovered->pDynamicPath)
    {
        nItemY = D2COMMON_PATH_GetYPosition(pHovered->pDynamicPath);
    }

    gnDescPopupX = pGridInfo->nGridLeft + nItemX * pGridInfo->nGridBoxWidth + pGridInfo->nGridBoxWidth * nHoverW / 2;
    gnDescPopupAltX = gnDescPopupX;
    gnDescPopupY = pGridInfo->nGridTop + nItemY * pGridInfo->nGridBoxHeight;
    gpHoverItem = pHovered;
    gpHoverItemLast = pHovered;
    gnDescPopupBottomY = pGridInfo->nGridTop + pGridInfo->nGridBoxHeight * (nItemY + nHoverH);
}


//1.10f: D2Client.0x6FAE0F30
//  Draws the items in the grid and the placement preview; sole caller INV_UpdateAndDrawPanels. It is the whole of what the inventory looks like
//  while an item is in hand. void: the eax the original leaves differs by path and all six call sites discard it.
//  The panel is chosen twice over, by unit type then by page: unit type 1 (monster) is the NPC shop grid and the cursor preview at the end is
//  skipped entirely; unit type 0 switches on gnVendorPage (1 trade page 1, 2 trade page 2, 3 the cube, 4 the stash, big or small by expansion,
//  else the inventory grid); anything else returns without drawing.
//  One pass over the whole list, filtered twice: the walk is INVENTORY_GetFirstItem / GetNextItem over every item the unit owns, and each is
//  dropped unless INVENTORY_GetItemNodePage is 1 and ITEMS_GetInvPage equals gnVendorPage.
//  The five highlight colours (indexes into gInvHighlightColors): 0 red, cannot be used (requirements unmet, targeting, locked, not equippable,
//  or a quest item whose quest is over; also the hover colour when the cursor state is 8 and the item is not transmogrifiable); 1 green, hovered
//  or a legal drop for the carried item; 2 blue, an identified item that is still quest-relevant; 3 yellow, the item that would be swapped out
//  by the drop; 4 dull red, unidentified.
//  Three quest items are special-cased by code: 'ass ' (544437089) Book of Skill, quest 9, state 5; 'xyz ' (544897400) scroll of self resurrect,
//  quest 20, state 5; 'tr2 ' (540177012) Scroll of Malah, quest 37, state 8 set and state 7 clear. Any other item whose nQuest column is non-zero
//  keeps colour 2 without a state check. State 5 is QFLAG_CUSTOM1; Malah's pair is QFLAG_CUSTOM4 set with QFLAG_CUSTOM3 clear.
//  An item's cell is in its path again: the inlined coordinate pair (dwClientCoordY at +0x0C for the X, the four bytes at +0x10 for the Y)
//  appears three times, for each drawn item and twice more for the item the swap would displace.
//  The preview is gated by the mouse position: the cursor must be inside the grid that sgInventoryMode says is live (trade page 2 in mode 11,
//  the stash in 12 and 13, the cube in 14, the inventory rect otherwise), and then two further gates apply: with sgInventoryMode 1 or 19 the mouse
//  X must be at or past gnScreenWidth / 2, and always the mouse Y must be under gnScreenHeight - 39.
//  The yellow highlight is drawn when INVENTORY_CanItemsBeExchanged says so or when the item under the cursor is the Horadric Cube
//  (dwCode == 544763746, 'box '), so dropping something onto the cube always previews as a swap.
void __fastcall INV_DrawInventoryGridItems(D2UnitStrc *pUnit)
{
    D2UnitStrc *pPlayer;
    D2InventoryGridInfoStrc *pPanel;
    D2InventoryStrc *pInventory;
    D2UnitStrc *pNode;
    D2UnitStrc *pItem;
    D2UnitStrc *pCursorItem;
    D2UnitStrc *pExchangeTarget;
    D2ItemsTxt *pItemsTxt;
    int32_t nUnitType;
    int32_t nInventoryRecordId;
    int32_t nX;
    int32_t nY;
    int32_t nColorIdx;
    int32_t nQuest;
    int32_t nMouseX;
    int32_t nMouseY;
    uint8_t nWidth;
    uint8_t nHeight;
    uint8_t nCursorW;
    uint8_t nCursorH;
    uint8_t nTargetW;
    uint8_t nTargetH;
    uint8_t nPage;
    uint32_t dwCode;
    uint32_t dwHoveredItems;
    D2UnitStrc *pPlacedTarget;

    INV_EnsurePanelLayout();
    pPlayer = CUNIT_GetPlayer();

    nUnitType = pUnit ? pUnit->dwUnitType : 6;
    if (nUnitType)
    {
        if (nUnitType != 1)
        {
            return;
        }
        pPanel = &gInvNpcShopGrid;
    }
    else
    {
        //  the page zero-extended: the bytes load the dword gnVendorPage and mask it with 0FFh (0x6FAE0F69); ours is a byte load (movzx)
        switch (gnVendorPage)
        {
        case 1:
            pPanel = &gInvTradePage1Grid;
            break;
        case 2:
            pPanel = &gInvTradePage2Grid;
            break;
        case 3:
            pPanel = &gInvCubeGrid;
            break;
        case 4:
            pPanel = GAME_IsExpansionGame() ? &gInvBigStashGrid : &gInvStashGrid;
            break;
        default:
            pPanel = &gInvPanelGrid;
            break;
        }
    }

    pInventory = pUnit->pInventory;
    if (!pInventory)
    {
        return;
    }

    nInventoryRecordId = D2COMMON_UNITS_GetInventoryRecordId(pUnit, (D2C_ItemInvPage)gnVendorPage, GAME_IsExpansionGame());

    for (pNode = D2COMMON_INVENTORY_GetFirstItem(pInventory); pNode; pNode = D2COMMON_INVENTORY_GetNextItem(pNode))
    {
        pItem = D2COMMON_INVENTORY_UnitIsItem(pNode);
        if (!pItem || D2COMMON_INVENTORY_GetItemNodePage(pNode) != 1)
        {
            continue;
        }

        //  the item's grid cell, out of its own path
        nUnitType = pItem->dwUnitType;
        if (pItem->dwUnitType == UNIT_OBJECT || nUnitType > 3 && nUnitType <= 5)
        {
            nX = pItem->pDynamicPath->dwClientCoordY;
        }
        else if (pItem->pDynamicPath)
        {
            nX = D2COMMON_PATH_GetXPosition(pItem->pDynamicPath);
        }
        else
        {
            nX = 0;
        }

        nUnitType = pItem->dwUnitType;
        if (pItem->dwUnitType == UNIT_OBJECT || nUnitType > 3 && nUnitType <= 5)
        {
            nY = *(int32_t *)&pItem->pDynamicPath->tTargetCoord;
        }
        else if (pItem->pDynamicPath)
        {
            nY = D2COMMON_PATH_GetYPosition(pItem->pDynamicPath);
        }
        else
        {
            nY = 0;
        }

        D2COMMON_ITEMS_GetDimensions(pItem, &nWidth, &nHeight, szD2CommonCmnItemsH, 1754);
        if (D2COMMON_ITEMS_GetInvPage(pItem) != gnVendorPage)
        {
            continue;
        }

        if (pItem == gpHoverItem && gbShowItemDesc)
        {
            nColorIdx = (CURSOR_GetCursorState() != 8 || D2COMMON_ITEMS_GetTransmogrify(gpHoverItem)) ? 1 : 0;
            INV_DrawGridHighlight(pPanel, nX, nY, nWidth, nHeight, (uint32_t)nColorIdx);
            GFX_DrawInventoryItem(pItem, pPanel->nGridLeft + nX * pPanel->nGridBoxWidth, pPanel->nGridTop + nY * pPanel->nGridBoxHeight);

            if (D2COMMON_ITEMS_CheckIfSocketable(pItem) && D2COMMON_ITEMS_GetMaxSockets(pItem) && D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_SOCKETED, 1306, szInvCpp))
            {
                INV_DrawSocketedItems(pItem, pPanel, pPanel->nGridLeft + nX * pPanel->nGridBoxWidth, pPanel->nGridTop + nY * pPanel->nGridBoxHeight);
            }
            continue;
        }

        if (!D2COMMON_ITEMS_CheckRequirements(pItem, pPlayer, 0, NULL, NULL, NULL) || D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_TARGETING, 1339, szInvCpp) || ITEM_IsItemLocked(pItem) || !D2COMMON_ITEMS_CanBeEquipped(pItem))
        {
            nColorIdx = 0;
        }
        else if (!D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_IDENTIFIED, 1343, szInvCpp))
        {
            nColorIdx = 4;
        }
        else
        {
            pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
            nColorIdx = 2;
            if (pItemsTxt->nQuest)
            {
                dwCode = pItemsTxt->dwCode;
                if (dwCode == 544437089)
                {
                    nQuest = 9;
                    goto CHECK_STATE_5;
                }
                if (dwCode == 544897400)
                {
                    nQuest = 20;
CHECK_STATE_5:
                    if (!D2COMMON_QUESTRECORD_GetQuestState(NPCMENU_QuestGetQuestRecord(), nQuest, 5))
                    {
                        nColorIdx = 0;
                    }
                }
                else if (dwCode == 540177012)
                {
                    //  Malah's scroll: relevant only while CUSTOM4 is set and CUSTOM3 is not
                    if (!D2COMMON_QUESTRECORD_GetQuestState(NPCMENU_QuestGetQuestRecord(), 37, 8) || D2COMMON_QUESTRECORD_GetQuestState(NPCMENU_QuestGetQuestRecord(), 37, 7))
                    {
                        nColorIdx = 0;
                    }
                }
            }
        }

        INV_DrawGridHighlight(pPanel, nX, nY, nWidth, nHeight, (uint32_t)nColorIdx);
        GFX_DrawInventoryItem(pItem, pPanel->nGridLeft + nX * pPanel->nGridBoxWidth, pPanel->nGridTop + nY * pPanel->nGridBoxHeight);
    }

    //  the placement preview -- never for the NPC shop panel
    if (pUnit->dwUnitType == UNIT_MONSTER)
    {
        return;
    }

    pCursorItem = D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory);
    if (!pCursorItem)
    {
        return;
    }

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    nPage = gnVendorPage;

    switch (sgInventoryMode)
    {
    case 11:
        if (nPage != 1)
        {
            if (nPage != 2)
            {
                goto TEST_INV_RECT;
            }
            if (!(nMouseX > gInvTradePage2Grid.nGridLeft && nMouseX < gInvTradePage2Grid.nGridRight && nMouseY >= gInvTradePage2Grid.nGridTop && nMouseY <= gInvTradePage2Grid.nGridBottom))
            {
                return;
            }
        }
        else
        {
            return;
        }
        break;

    case 12:
    case 13:
        if (nPage == 4)
        {
            if (GAME_IsExpansionGame())
            {
                if (!(nMouseX > gInvBigStashGrid.nGridLeft && nMouseX <= gInvBigStashGrid.nGridRight && nMouseY >= gInvBigStashGrid.nGridTop && nMouseY <= gInvBigStashGrid.nGridBottom))
                {
                    return;
                }
            }
            else if (!(nMouseX > gInvStashGrid.nGridLeft && nMouseX <= gInvStashGrid.nGridRight && nMouseY >= gInvStashGrid.nGridTop && nMouseY <= gInvStashGrid.nGridBottom))
            {
                return;
            }
        }
        else
        {
            goto TEST_INV_RECT;
        }
        break;

    case 14:
        if (nPage == 3)
        {
            if (!(nMouseX >= gInvCubeGrid.nGridLeft && nMouseX < gInvCubeGrid.nGridRight && nMouseY >= gInvCubeGrid.nGridTop && nMouseY <= gInvCubeGrid.nGridBottom))
            {
                return;
            }
        }
        else if (!(nMouseX >= gInvPanelRect.nLeft && nMouseX < gInvPanelRect.nRight && nMouseY >= gInvPanelRect.nTop && nMouseY <= gInvPanelRect.nBottom))
        {
            return;
        }
        break;

    default:
        break;

TEST_INV_RECT:
        if (!(nMouseX >= gInvPanelRect.nLeft && nMouseX < gInvPanelRect.nRight && nMouseY >= gInvPanelRect.nTop && nMouseY <= gInvPanelRect.nBottom))
        {
            return;
        }
        break;
    }

    if (nPage == 0xFF)
    {
        //  a page of 0xFF traces instead of drawing; the trace prints the literal 255 rather than the variable, as the one in
        //  INV_OnGridLeftClick does
        FOG_Trace(" Invalid page %d at line %d of file %s\n", 255, 1050, szInvCpp);
        return;
    }

    if (!gbShowItemDesc)
    {
        return;
    }

    pExchangeTarget = NULL;
    D2COMMON_ITEMS_GetDimensions(pCursorItem, &nCursorW, &nCursorH, szD2CommonCmnItemsH, 1754);
    if (gnHoverCellX < 0 || gnHoverCellY < 0)
    {
        return;
    }

    //  the Y gate is tested twice, once inside each mode gate (the compiler's doing, not two different tests)
    if (sgInventoryMode == 1 && CURSOR_GetMouseX() < (gnScreenWidth / 2))
    {
        return;
    }
    if (CURSOR_GetMouseY() >= (gnScreenHeight - 39))
    {
        return;
    }
    if (sgInventoryMode == 19 && CURSOR_GetMouseX() < (gnScreenWidth / 2))
    {
        return;
    }
    if (CURSOR_GetMouseY() >= (gnScreenHeight - 39))
    {
        return;
    }

    if (D2COMMON_INVENTORY_CanItemBePlaced(pInventory, pCursorItem, gnHoverCellX, gnHoverCellY, (D2C_InventoryRecords)nInventoryRecordId, &pPlacedTarget, &dwHoveredItems, nPage))
    {
        INV_DrawGridHighlight(pPanel, gnHoverCellX, gnHoverCellY, nCursorW, nCursorH, 1u);
        return;
    }

    if (D2COMMON_INVENTORY_CanItemsBeExchanged(pInventory, pCursorItem, gnHoverCellX, gnHoverCellY, (D2C_InventoryRecords)nInventoryRecordId, &pExchangeTarget, nPage, 1) || pExchangeTarget
               && D2COMMON_DATATBLS_GetItemsTxtRecord(pExchangeTarget->dwClassId)->dwCode == 544763746)
    {
        //  BUG: pExchangeTarget is not null-checked on the first arm of the ||. The second arm requires it non-NULL, but the first
        //  (CanItemsBeExchanged returning true) is trusted to have filled it. NOT ESTABLISHED: whether that function (D2Common's) can return true
        //  and leave the pointer alone, in which case the arm would fault. Reproduced without a guard.
        D2COMMON_ITEMS_GetDimensions(pExchangeTarget, &nTargetW, &nTargetH, szD2CommonCmnItemsH, 1754);

        nUnitType = pExchangeTarget->dwUnitType;
        //  signed, as the image compares it (jle, jle); no unit type 0..5 tells it from unsigned
        if (pExchangeTarget->dwUnitType == UNIT_OBJECT || nUnitType > (int32_t)UNIT_MISSILE && nUnitType <= (int32_t)UNIT_TILE)
        {
            nY = *(int32_t *)&pExchangeTarget->pDynamicPath->tTargetCoord;
        }
        else if (pExchangeTarget->pDynamicPath)
        {
            nY = D2COMMON_PATH_GetYPosition(pExchangeTarget->pDynamicPath);
        }
        else
        {
            nY = 0;
        }

        nUnitType = pExchangeTarget->dwUnitType;
        if (pExchangeTarget->dwUnitType == UNIT_OBJECT || nUnitType > 3 && nUnitType <= 5)
        {
            nX = pExchangeTarget->pDynamicPath->dwClientCoordY;
        }
        else if (pExchangeTarget->pDynamicPath)
        {
            nX = D2COMMON_PATH_GetXPosition(pExchangeTarget->pDynamicPath);
        }
        else
        {
            nX = 0;
        }

        INV_DrawGridHighlight(pPanel, nX, nY, nTargetW, nTargetH, 3u);
        return;
    }

    INV_DrawGridHighlight(pPanel, gnHoverCellX, gnHoverCellY, nCursorW, nCursorH, 0);
}

//  The three body-slot click handlers are the whole of gBodyLocClickHandlers. None has a code caller: they are reached only as data, indexed by
//  the body location in INV_OnLButtonDown (call gBodyLocClickHandlers[ebx*4] at 0x6FAE72E6).
//      6FAE5CB0  INV_OnBodyLocClickRightArm   entry 3
//      6FAE6360  INV_OnBodyLocClickLeftArm    entry 4
//      6FAE6A00  INV_OnBodyLocClickGeneric    the other eight
//  The answer is read by the caller: 1 on every path that sends a command, 0 on the path that opens an NPC transaction instead
//  ("the click was consumed").
//  D2Common #10299 (pUnit, nBodyLoc, pCursorItem, bDontCheckReqs) returns a code and each handler is a switch on it. The generic slot uses four
//  of the codes and the two arms use eight:
//      0  nothing may go here      play sound 0x14 or 0x13 and consume
//      1  place it                 command 26
//      2  swap with the other arm  command 27 (arms only)
//      3  the slot is occupied     command 28 to unequip, or an NPC transaction when a shop mode is up
//      4  the other arm's turn     command 28 with the other arm's number, or an NPC transaction in mode 4 (arms only)
//      5  place over what is here  command 29
//      6  swap with the cursor     command 33 (arms only)
//      7  a three-way swap         command 30 (arms only)
//  The two sound events on code 0: 0x14 when the item is class-specific and the class matches (or is 7), 0x13 otherwise.
//  The two arms are one function with four constants swapped: the body location fetched into each local, and 4 against 5 in the four
//  MSG_SendCmdUnit / MSG_SendCmdWord calls (BODYLOC_RARM and BODYLOC_LARM); each handler uses the other hand's number exactly once, in case 4,
//  the reach-across. They are written out twice because they are two functions in the image.
//  The 250 ms cooldown is on every arm and only in multiplayer: every path that sends a command first checks GAME_GetGameType() against the two
//  offline types and, if online, that TRADE_TickTimeout() is clear and 250 ms have passed since gnLastItemUseTick. The timeout arm sends the green
//  check and returns, so a refused click is not silent.
//  In both arms NPCMENU_BeginTransaction(pUnit, <item>, 1, gnUiMouseX, gnUiMouseY, 0, 0, 0) appears twice, once with the item already in the slot
//  and once with the other arm's, and the two arms of the switch share the single test that follows; only one of the two runs per call (the
//  compiler's tail merge), which is why case 3's default falls into case 4's body.

//1.10f: D2Client.0x6FAE5CB0
//  BODYLOC_RARM (4), entry 3 of gBodyLocClickHandlers.
int32_t __fastcall INV_OnBodyLocClickRightArm(D2UnitStrc *pUnit, D2InventoryStrc *pInventory, D2C_PlayerBodyLocs nBodyLoc)
{
    int32_t nResult;
    D2UnitStrc *pCursorItem;
    D2UnitStrc *pThisArm;
    D2UnitStrc *pOtherArm;
    D2UnitStrc *pHeld;
    D2UnitStrc *pUnequip;
    BOOL bTransactionBegun;
    int32_t nClassOfItem;
    int32_t dwClassId;
    int32_t dwUnitId;
    D2C_Sounds nSoundId;

    nResult = 0;
    pCursorItem = D2COMMON_INVENTORY_GetCursorItem(pInventory);
    pOtherArm = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_LARM);
    pThisArm = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RARM);

    if (CURSOR_GetCursorState() == 6)
    {
        pHeld = CURSOR_GetHeldUnit();
        if (pHeld && pThisArm && !ITEM_IsItemLocked(pThisArm))
        {
            if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
            {
                if (TRADE_TickTimeout())
                {
                    goto GREEN_CHECK;
                }
                if (GetTickCount() - gnLastItemUseTick <= 0xFA)
                {
                    goto DISPATCH;
                }
                gnLastItemUseTick = GetTickCount();
            }
            MSG_SendCmdUnit(39, pThisArm->dwUnitId, pHeld->dwUnitId);
            ITEM_SetUninterruptableState(pThisArm);
            nResult = 1;
        }
        goto DISPATCH;
    }

    if (CURSOR_GetCursorState() == 8 && pThisArm && !ITEM_IsItemLocked(pThisArm))
    {
        if (GAME_GetGameType() && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                goto GREEN_CHECK;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                goto DISPATCH;
            }
            gnLastItemUseTick = GetTickCount();
        }
        MSG_SendServerItemMessage(76, pThisArm->dwUnitId);
        ITEM_SetUninterruptableState(pThisArm);
        nResult = 1;
        goto DISPATCH;
    }
    goto DISPATCH;

GREEN_CHECK:
    TRADE_SendGreenCheck();

DISPATCH:
    switch (D2COMMON_10299(pUnit, nBodyLoc, pCursorItem, 0))
    {
    case 0:
        if (!pCursorItem)
        {
            return 1;
        }
        nClassOfItem = 0;
        if (D2COMMON_10299(pUnit, nBodyLoc, pCursorItem, 1))
        {
            nClassOfItem = D2COMMON_ITEMS_GetClassOfClassSpecificItem(pCursorItem);
            dwClassId = pUnit ? pUnit->dwClassId : -1;
            if (nClassOfItem == 7 || nClassOfItem == dwClassId)
            {
                UNITSND_PlayPlayerSoundEvent(pUnit, 0x14u);
                return 1;
            }
        }
        UNITSND_PlayPlayerSoundEvent(pUnit, 0x13u);
        return 1;

    case 1:
        if (ITEM_IsItemLocked(pCursorItem))
        {
            return nResult;
        }
        if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                goto GREEN_CHECK_RETURN;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                return nResult;
            }
            gnLastItemUseTick = GetTickCount();
        }
        dwUnitId = pCursorItem ? pCursorItem->dwUnitId : -1;
        MSG_SendCmdUnit(26, dwUnitId, 4);
        goto FINISH_DROP;

    case 2:
        if (ITEM_IsItemLocked(pCursorItem) || ITEM_IsItemLocked(pOtherArm))
        {
            return nResult;
        }
        if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                goto GREEN_CHECK_RETURN;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                return nResult;
            }
            gnLastItemUseTick = GetTickCount();
        }
        dwUnitId = pCursorItem ? pCursorItem->dwUnitId : -1;
        MSG_SendCmdUnit(27, dwUnitId, 4);
        ITEM_SetUninterruptableState(pOtherArm);
        ITEM_SetUninterruptableState(pCursorItem);
        nSoundId = ITEM_GetDropSoundNoFrame(pCursorItem);
        SOUNDINST_PlaySound(nSoundId, NULL, 0, 0, 0);
        gbDeselectShopTabsPending = 1;
        return 1;

    case 3:
        switch (sgInventoryMode)
        {
        case 0:
        case 1:
        case 2:
        case 11:
        case 12:
        case 14:
        case 15:
        case 19:
            if (ITEM_IsItemLocked(pThisArm))
            {
                return nResult;
            }
            if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
            {
                if (TRADE_TickTimeout())
                {
                    goto GREEN_CHECK_RETURN;
                }
                if (GetTickCount() - gnLastItemUseTick <= 0xFA)
                {
                    return nResult;
                }
                gnLastItemUseTick = GetTickCount();
            }
            MSG_SendCmdWord(28, 4);
            pUnequip = pThisArm;
            goto FINISH_UNEQUIP;

        default:
            //  one cmp eax, 13h emitted before the jump table: sgInventoryMode >= 19
            if (sgInventoryMode >= 19 || !pThisArm)
            {
                return nResult;
            }
            bTransactionBegun = NPCMENU_BeginTransaction(pUnit, pThisArm, 1, gnUiMouseX, gnUiMouseY, 0, 0, 0);
            goto TEST_TRANSACTION;
        }

    case 4:
        if (sgInventoryMode == 4)
        {
            if (!pOtherArm)
            {
                return nResult;
            }
            bTransactionBegun = NPCMENU_BeginTransaction(pUnit, pOtherArm, 1, gnUiMouseX, gnUiMouseY, 0, 0, 0);
TEST_TRANSACTION:
            if (bTransactionBegun)
            {
                sgInventoryMode = 5;
                CURSOR_SetHeldUnit(NULL);
            }
            return nResult;
        }
        if (ITEM_IsItemLocked(pOtherArm))
        {
            return nResult;
        }
        if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                goto GREEN_CHECK_RETURN;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                return nResult;
            }
            gnLastItemUseTick = GetTickCount();
        }
        //  the reach-across: the OTHER hand's number
        MSG_SendCmdWord(28, 5);
        pUnequip = pOtherArm;
FINISH_UNEQUIP:
        ITEM_SetUninterruptableState(pUnequip);
        gbDeselectShopTabsPending = 1;
        return 1;

    case 5:
        if (ITEM_IsItemLocked(pThisArm))
        {
            return nResult;
        }
        if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                goto GREEN_CHECK_RETURN;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                return nResult;
            }
            gnLastItemUseTick = GetTickCount();
        }
        dwUnitId = pCursorItem ? pCursorItem->dwUnitId : -1;
        MSG_SendCmdUnit(29, dwUnitId, 4);
        goto FINISH_DROP;

    case 6:
        if (ITEM_IsItemLocked(pCursorItem))
        {
            return nResult;
        }
        if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                goto GREEN_CHECK_RETURN;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                return nResult;
            }
            gnLastItemUseTick = GetTickCount();
        }
        if (sgInventoryMode == 11)
        {
            return nResult;
        }
        MSG_SendCmdUnit(33, pCursorItem ? pCursorItem->dwUnitId : -1, pThisArm ? pThisArm->dwUnitId : -1);
FINISH_DROP:
        ITEM_SetUninterruptableState(pCursorItem);
        nSoundId = ITEM_GetDropSoundNoFrame(pCursorItem);
        SOUNDINST_PlaySound(nSoundId, NULL, 0, 0, 0);
        return 1;

    case 7:
        if (ITEM_IsItemLocked(pCursorItem) || ITEM_IsItemLocked(pThisArm) || ITEM_IsItemLocked(pOtherArm))
        {
            return nResult;
        }
        if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                goto GREEN_CHECK_RETURN;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                return nResult;
            }
            gnLastItemUseTick = GetTickCount();
        }
        dwUnitId = pCursorItem ? pCursorItem->dwUnitId : -1;
        MSG_SendCmdUnit(30, dwUnitId, 4);
        ITEM_SetUninterruptableState(pCursorItem);
        ITEM_SetUninterruptableState(pThisArm);
        ITEM_SetUninterruptableState(pOtherArm);
        return 1;

    default:
        return nResult;
    }

GREEN_CHECK_RETURN:
    TRADE_SendGreenCheck();
    return nResult;
}
//1.10f: D2Client.0x6FAE6360
//  BODYLOC_LARM (5), entry 4 of gBodyLocClickHandlers: the same code as INV_OnBodyLocClickRightArm with the body locations and the four command
//  constants swapped. Written out rather than shared, because they are two functions in the image.
int32_t __fastcall INV_OnBodyLocClickLeftArm(D2UnitStrc *pUnit, D2InventoryStrc *pInventory, D2C_PlayerBodyLocs nBodyLoc)
{
    int32_t nResult;
    D2UnitStrc *pCursorItem;
    D2UnitStrc *pThisArm;
    D2UnitStrc *pOtherArm;
    D2UnitStrc *pHeld;
    D2UnitStrc *pUnequip;
    BOOL bTransactionBegun;
    int32_t nClassOfItem;
    int32_t dwClassId;
    int32_t dwUnitId;
    D2C_Sounds nSoundId;

    nResult = 0;
    pCursorItem = D2COMMON_INVENTORY_GetCursorItem(pInventory);
    pThisArm = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_LARM);
    pOtherArm = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RARM);

    if (CURSOR_GetCursorState() == 6)
    {
        pHeld = CURSOR_GetHeldUnit();
        if (pHeld && pThisArm && !ITEM_IsItemLocked(pThisArm))
        {
            if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
            {
                if (TRADE_TickTimeout())
                {
                    goto GREEN_CHECK;
                }
                if (GetTickCount() - gnLastItemUseTick <= 0xFA)
                {
                    goto DISPATCH;
                }
                gnLastItemUseTick = GetTickCount();
            }
            MSG_SendCmdUnit(39, pThisArm->dwUnitId, pHeld->dwUnitId);
            ITEM_SetUninterruptableState(pThisArm);
            nResult = 1;
        }
        goto DISPATCH;
    }

    if (CURSOR_GetCursorState() == 8 && pThisArm && !ITEM_IsItemLocked(pThisArm))
    {
        if (GAME_GetGameType() && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                goto GREEN_CHECK;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                goto DISPATCH;
            }
            gnLastItemUseTick = GetTickCount();
        }
        MSG_SendServerItemMessage(76, pThisArm->dwUnitId);
        ITEM_SetUninterruptableState(pThisArm);
        nResult = 1;
        goto DISPATCH;
    }
    goto DISPATCH;

GREEN_CHECK:
    TRADE_SendGreenCheck();

DISPATCH:
    switch (D2COMMON_10299(pUnit, nBodyLoc, pCursorItem, 0))
    {
    case 0:
        if (!pCursorItem)
        {
            return 1;
        }
        nClassOfItem = 0;
        if (D2COMMON_10299(pUnit, nBodyLoc, pCursorItem, 1))
        {
            nClassOfItem = D2COMMON_ITEMS_GetClassOfClassSpecificItem(pCursorItem);
            dwClassId = pUnit ? pUnit->dwClassId : -1;
            if (nClassOfItem == 7 || nClassOfItem == dwClassId)
            {
                UNITSND_PlayPlayerSoundEvent(pUnit, 0x14u);
                return 1;
            }
        }
        UNITSND_PlayPlayerSoundEvent(pUnit, 0x13u);
        return 1;

    case 1:
        if (ITEM_IsItemLocked(pCursorItem))
        {
            return nResult;
        }
        if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                goto GREEN_CHECK_RETURN;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                return nResult;
            }
            gnLastItemUseTick = GetTickCount();
        }
        dwUnitId = pCursorItem ? pCursorItem->dwUnitId : -1;
        MSG_SendCmdUnit(26, dwUnitId, 5);
        goto FINISH_DROP;

    case 2:
        if (ITEM_IsItemLocked(pCursorItem) || ITEM_IsItemLocked(pOtherArm))
        {
            return nResult;
        }
        if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                goto GREEN_CHECK_RETURN;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                return nResult;
            }
            gnLastItemUseTick = GetTickCount();
        }
        dwUnitId = pCursorItem ? pCursorItem->dwUnitId : -1;
        MSG_SendCmdUnit(27, dwUnitId, 5);
        ITEM_SetUninterruptableState(pOtherArm);
        ITEM_SetUninterruptableState(pCursorItem);
        nSoundId = ITEM_GetDropSoundNoFrame(pCursorItem);
        SOUNDINST_PlaySound(nSoundId, NULL, 0, 0, 0);
        gbDeselectShopTabsPending = 1;
        return 1;

    case 3:
        switch (sgInventoryMode)
        {
        case 0:
        case 1:
        case 2:
        case 11:
        case 12:
        case 14:
        case 15:
        case 19:
            if (ITEM_IsItemLocked(pThisArm))
            {
                return nResult;
            }
            if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
            {
                if (TRADE_TickTimeout())
                {
                    goto GREEN_CHECK_RETURN;
                }
                if (GetTickCount() - gnLastItemUseTick <= 0xFA)
                {
                    return nResult;
                }
                gnLastItemUseTick = GetTickCount();
            }
            MSG_SendCmdWord(28, 5);
            pUnequip = pThisArm;
            goto FINISH_UNEQUIP;

        default:
            //  one cmp eax, 13h emitted before the jump table: sgInventoryMode >= 19
            if (sgInventoryMode >= 19 || !pThisArm)
            {
                return nResult;
            }
            bTransactionBegun = NPCMENU_BeginTransaction(pUnit, pThisArm, 1, gnUiMouseX, gnUiMouseY, 0, 0, 0);
            goto TEST_TRANSACTION;
        }

    case 4:
        if (sgInventoryMode == 4)
        {
            if (!pOtherArm)
            {
                return nResult;
            }
            bTransactionBegun = NPCMENU_BeginTransaction(pUnit, pOtherArm, 1, gnUiMouseX, gnUiMouseY, 0, 0, 0);
TEST_TRANSACTION:
            if (bTransactionBegun)
            {
                sgInventoryMode = 5;
                CURSOR_SetHeldUnit(NULL);
            }
            return nResult;
        }
        if (ITEM_IsItemLocked(pOtherArm))
        {
            return nResult;
        }
        if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                goto GREEN_CHECK_RETURN;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                return nResult;
            }
            gnLastItemUseTick = GetTickCount();
        }
        //  the reach-across: the OTHER hand's number
        MSG_SendCmdWord(28, 4);
        pUnequip = pOtherArm;
FINISH_UNEQUIP:
        ITEM_SetUninterruptableState(pUnequip);
        gbDeselectShopTabsPending = 1;
        return 1;

    case 5:
        if (ITEM_IsItemLocked(pThisArm))
        {
            return nResult;
        }
        if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                goto GREEN_CHECK_RETURN;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                return nResult;
            }
            gnLastItemUseTick = GetTickCount();
        }
        dwUnitId = pCursorItem ? pCursorItem->dwUnitId : -1;
        MSG_SendCmdUnit(29, dwUnitId, 5);
        goto FINISH_DROP;

    case 6:
        if (ITEM_IsItemLocked(pCursorItem))
        {
            return nResult;
        }
        if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                goto GREEN_CHECK_RETURN;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                return nResult;
            }
            gnLastItemUseTick = GetTickCount();
        }
        if (sgInventoryMode == 11)
        {
            return nResult;
        }
        MSG_SendCmdUnit(33, pCursorItem ? pCursorItem->dwUnitId : -1, pThisArm ? pThisArm->dwUnitId : -1);
FINISH_DROP:
        ITEM_SetUninterruptableState(pCursorItem);
        nSoundId = ITEM_GetDropSoundNoFrame(pCursorItem);
        SOUNDINST_PlaySound(nSoundId, NULL, 0, 0, 0);
        return 1;

    case 7:
        if (ITEM_IsItemLocked(pCursorItem) || ITEM_IsItemLocked(pThisArm) || ITEM_IsItemLocked(pOtherArm))
        {
            return nResult;
        }
        if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                goto GREEN_CHECK_RETURN;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                return nResult;
            }
            gnLastItemUseTick = GetTickCount();
        }
        dwUnitId = pCursorItem ? pCursorItem->dwUnitId : -1;
        MSG_SendCmdUnit(30, dwUnitId, 5);
        //  the order is the right arm's, not this one's: cursor, then the right arm, then the left; both handlers mark them in that order,
        //  so here pOtherArm comes first
        ITEM_SetUninterruptableState(pCursorItem);
        ITEM_SetUninterruptableState(pOtherArm);
        ITEM_SetUninterruptableState(pThisArm);
        return 1;

    default:
        return nResult;
    }

GREEN_CHECK_RETURN:
    TRADE_SendGreenCheck();
    return nResult;
}

//1.10f: D2Client.0x6FAE6A00
//  The ordinary equipment slot: eight of the ten entries of gBodyLocClickHandlers point here. It uses four of D2Common #10299's codes where the two
//  arms use eight, and passes the slot's own body location to every command rather than a constant, because it serves eight different slots.
//  Its case 3 has no ITEM_IsItemLocked guard where both arms do, and it sends command 28 with nBodyLoc straight from the parameter.
int32_t __fastcall INV_OnBodyLocClickGeneric(D2UnitStrc *pUnit, D2InventoryStrc *pInventory, D2C_PlayerBodyLocs nBodyLoc)
{
    int32_t nResult;
    D2UnitStrc *pCursorItem;
    D2UnitStrc *pHeld;
    D2UnitStrc *pSlotItem;
    BOOL bTransactionBegun;
    int32_t nClassOfItem;
    int32_t dwClassId;
    int32_t dwUnitId;
    D2C_Sounds nSoundId;

    nResult = 0;
    pCursorItem = D2COMMON_INVENTORY_GetCursorItem(pInventory);

    if (CURSOR_GetCursorState() == 6)
    {
        pHeld = CURSOR_GetHeldUnit();
        pSlotItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, nBodyLoc);
        if (pHeld && pSlotItem && !ITEM_IsItemLocked(pSlotItem))
        {
            if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
            {
                if (TRADE_TickTimeout())
                {
                    goto GREEN_CHECK_ZERO;
                }
                if (GetTickCount() - gnLastItemUseTick <= 0xFA)
                {
                    return nResult;
                }
                gnLastItemUseTick = GetTickCount();
            }
            MSG_SendCmdUnit(39, pSlotItem->dwUnitId, pHeld->dwUnitId);
            ITEM_SetUninterruptableState(pSlotItem);
            return 1;
        }
        return nResult;
    }

    if (CURSOR_GetCursorState() == 8)
    {
        pSlotItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, nBodyLoc);
        if (pSlotItem && !ITEM_IsItemLocked(pSlotItem))
        {
            if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
            {
                if (TRADE_TickTimeout())
                {
                    goto GREEN_CHECK_ZERO;
                }
                if (GetTickCount() - gnLastItemUseTick <= 0xFA)
                {
                    return nResult;
                }
                gnLastItemUseTick = GetTickCount();
            }
            MSG_SendServerItemMessage(76, pSlotItem->dwUnitId);
            ITEM_SetUninterruptableState(pSlotItem);
            return 1;
        }
        return nResult;
    }

    switch (D2COMMON_10299(pUnit, nBodyLoc, pCursorItem, 0))
    {
    case 0:
        if (!pCursorItem)
        {
            return 1;
        }
        if (D2COMMON_10299(pUnit, nBodyLoc, pCursorItem, 1))
        {
            nClassOfItem = D2COMMON_ITEMS_GetClassOfClassSpecificItem(pCursorItem);
            dwClassId = pUnit ? pUnit->dwClassId : -1;
            if (nClassOfItem == 7 || nClassOfItem == dwClassId)
            {
                UNITSND_PlayPlayerSoundEvent(pUnit, 0x14u);
                return 1;
            }
        }
        UNITSND_PlayPlayerSoundEvent(pUnit, 0x13u);
        return 1;

    case 1:
        if (ITEM_IsItemLocked(pCursorItem))
        {
            return nResult;
        }
        if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                goto GREEN_CHECK_ZERO;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                return nResult;
            }
            gnLastItemUseTick = GetTickCount();
        }
        dwUnitId = pCursorItem ? pCursorItem->dwUnitId : -1;
        MSG_SendCmdUnit(26, dwUnitId, (uint16_t)nBodyLoc);
        ITEM_SetUninterruptableState(pCursorItem);
        nSoundId = ITEM_GetDropSoundNoFrame(pCursorItem);
        SOUNDINST_PlaySound(nSoundId, NULL, 0, 0, 0);
        gbDeselectShopTabsPending = 1;
        return 1;

    case 3:
        switch (sgInventoryMode)
        {
        case 0:
        case 1:
        case 2:
        case 11:
        case 12:
        case 14:
        case 15:
        case 19:
            //  no IsItemLocked guard here, unlike both arms
            if (GAME_GetGameType() && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
            {
                if (TRADE_TickTimeout())
                {
                    goto GREEN_CHECK_ZERO;
                }
                if (GetTickCount() - gnLastItemUseTick <= 0xFA)
                {
                    return nResult;
                }
                gnLastItemUseTick = GetTickCount();
            }
            MSG_SendCmdWord(28, (int16_t)nBodyLoc);
            gbDeselectShopTabsPending = 1;
            return 1;

        default:
            //  one cmp eax, 13h before the jump table (as in the arms)
            if (sgInventoryMode >= 19)
            {
                return nResult;
            }
            pSlotItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, nBodyLoc);
            if (!pSlotItem)
            {
                return nResult;
            }
            bTransactionBegun = NPCMENU_BeginTransaction(pUnit, pSlotItem, 1, gnUiMouseX, gnUiMouseY, 0, 0, 0);
            if (!bTransactionBegun)
            {
                return nResult;
            }
            sgInventoryMode = 5;
            CURSOR_SetHeldUnit(NULL);
            return 0;
        }

    case 5:
        if (ITEM_IsItemLocked(pCursorItem))
        {
            return nResult;
        }
        if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                goto GREEN_CHECK_ZERO;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                return nResult;
            }
            gnLastItemUseTick = GetTickCount();
        }
        dwUnitId = pCursorItem ? pCursorItem->dwUnitId : -1;
        MSG_SendCmdUnit(29, dwUnitId, (uint16_t)nBodyLoc);
        ITEM_SetUninterruptableState(pCursorItem);
        nSoundId = ITEM_GetDropSoundNoFrame(pCursorItem);
        SOUNDINST_PlaySound(nSoundId, NULL, 0, 0, 0);
        return 1;

    default:
        return nResult;
    }

GREEN_CHECK_ZERO:
    TRADE_SendGreenCheck();
    return 0;
}

//  The third assert text this unit pushes (0x6FAE7EEB).
static const char szPtItemStatsAssert[] = "ptItemStats";

//1.10f: D2Client.0x6FAE75C0
//  The left click on an inventory grid; called from INV_NpcShopOnLButtonDown once and from INV_OnLButtonDown three times (trade page, stash,
//  cube). The fifth parameter is the mouse message's wParam: the three call sites pass (uint8_t)pMsg->wParam, and its only read is
//  if ((nMouseFlags & 4) && ITEMS_CheckIfBeltable(pItem) && !ITEMS_GetInvPage(pItem)) MSG_SendServerItemMessage(99, ...);
//  SOUNDINST_PlaySound(235, ...); where bit 2 of wParam is MK_SHIFT, so that arm is shift-click-to-belt. The arm beside it reads
//  GetAsyncKeyState(17), VK_CONTROL: one modifier comes out of the message and the other out of the keyboard.
//  What a click can do, in the order it is tried. With something on the cursor: stack it (ITEMS_AreStackablesEqual, command 33); socket it
//  (INV_CanInsertSocketFiller, command 40); a scroll into a tome (matching ITEMS_GetSuffixId on a SCROLL and a BOOK, command 41, with
//  SPELLSEL_UnbindEmptyScrollHotkeys first); into the cube (the target's code is 'box ', command 42, placed at a free cell of inventory record 3);
//  swap (INVENTORY_CanItemsBeExchanged, command 31); place (INVENTORY_CanItemBePlaced, command 24). With an empty cursor, on an item: shift-click,
//  beltable and on page 0, item message 99; ctrl-click with an NPC interacting, command 51 with ITEMS_GetTransactionCost(..., TRANSACTIONTYPE_SELL);
//  plain, item message 25; and when the mode is 3..18 or 20+, an NPC transaction instead.
//  The cube refuses to go inside itself: if (nInvPage == INVPAGE_CUBE && DATATBLS_GetItemsTxtRecord(pCursorItem->dwClassId)->dwCode == 'box ')
//  return 1; the lookup asserts first, ptItemStats at inv.cpp:5655.
//  The original's flag register also carries a pointer comparison, (int)pUnit < (int)pPlayer, that nothing reads; the pUnit == pPlayer beside it
//  is bPlayerOwnsItem. They are written as two separate things.
//  The 250 ms cooldown appears at nine sites, in the same shape as in the body-slot handlers: offline games skip it and the TRADE_TickTimeout arm
//  sends the green check. Two of the nine send it and continue rather than returning: the socket-filler and scroll-into-tome arms fall through
//  to the cube test afterwards.
int32_t __fastcall INV_OnGridLeftClick(D2UnitStrc *pUnit, D2InventoryStrc *pInventory, int32_t nXPos, int32_t nYPos, uint8_t nMouseFlags, D2InventoryGridInfoStrc *pGridInfo, D2C_ItemInvPage nInvPage)
{
    int32_t bHandled;
    D2UnitStrc *pCursorItem;
    D2UnitStrc *pHeld;
    D2UnitStrc *pSlotItem;
    D2UnitStrc *pHoveredItem;
    D2UnitStrc *pExchangeTarget;
    D2UnitStrc *pPlayer;
    D2UnitStrc *pNpc;
    D2ItemsTxt *pItemsTxt;
    D2BitBufferStrc *pQuestRecord;
    D2C_InventoryRecords nInventoryRecordId;
    int32_t bPlayerOwnsItem;
    int32_t bStillPlaceable;
    int32_t nGridX;
    int32_t nGridY;
    int32_t nGridCol;
    int32_t nGridRow;
    int32_t nUnitGUID;
    D2C_Sounds nSoundId;
    int32_t nTransactionCost;
    int32_t dwClassId;
    D2C_Difficulties nDifficulty;
    uint16_t nSuffixId;
    uint8_t nItemW;
    uint8_t nItemH;
    int32_t nCelW;
    int32_t nCelH;
    int32_t pX;
    int32_t pY;
    uint32_t dwHoveredItems;
    D2UnitStrc *pPlacedItem;
    uint32_t nCanPlaceCount;
    D2UnitStrc *pCanPlaceItem;
    D2UnitGUID dwNpcGUID;
    int32_t nFreeX;
    int32_t nFreeY;

    bHandled = 0;
    if ((uint8_t)nInvPage == INVPAGE_NULL)
    {
        //  the literal 255 again, not the variable
        FOG_Trace(" Invalid page %d at line %d of file %s\n", 255, 5430, szInvCpp);
        return 0;
    }

    pCursorItem = D2COMMON_INVENTORY_GetCursorItem(pInventory);
    nInventoryRecordId = (D2C_InventoryRecords)D2COMMON_UNITS_GetInventoryRecordId(pUnit, nInvPage, GAME_IsExpansionGame());
    nGridX = (nXPos - pGridInfo->nGridLeft) / (uint32_t)pGridInfo->nGridBoxWidth;
    nGridY = (nYPos - pGridInfo->nGridTop) / (uint32_t)pGridInfo->nGridBoxHeight;

    //  the cursor is holding a socket filler or a scroll
    if (CURSOR_GetCursorState() == 6)
    {
        pHeld = CURSOR_GetHeldUnit();
        pSlotItem = D2COMMON_INVENTORY_GetItemFromInventoryPage(pInventory, nGridX, nGridY, &pX, &pY, nInventoryRecordId, (uint8_t)nInvPage);
        if (!pHeld || !pSlotItem)
        {
            return bHandled;
        }
        bHandled = 1;
        if (ITEM_IsItemLocked(pSlotItem))
        {
            return bHandled;
        }
        if (pUnit != CUNIT_GetPlayer() || (uint8_t)nInvPage == INVPAGE_EQUIP || (uint8_t)nInvPage == INVPAGE_TRADE)
        {
            return 0;
        }
        if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                TRADE_SendGreenCheck();
                return 0;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                return 0;
            }
            gnLastItemUseTick = GetTickCount();
        }
        MSG_SendCmdUnit(39, pSlotItem->dwUnitId, pHeld->dwUnitId);
        ITEM_SetUninterruptableState(pSlotItem);
        return 1;
    }

    //  the cursor is in the "identify / repair" state
    if (CURSOR_GetCursorState() == 8)
    {
        pSlotItem = D2COMMON_INVENTORY_GetItemFromInventoryPage(pInventory, nGridX, nGridY, &pX, &pY, nInventoryRecordId, (uint8_t)nInvPage);
        if (!pSlotItem)
        {
            return bHandled;
        }
        bHandled = 1;
        if (ITEM_IsItemLocked(pSlotItem))
        {
            return bHandled;
        }
        if (pUnit != CUNIT_GetPlayer() || (uint8_t)nInvPage == INVPAGE_EQUIP || (uint8_t)nInvPage == INVPAGE_TRADE)
        {
            return 0;
        }
        if (GAME_GetGameType() && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                TRADE_SendGreenCheck();
                return 0;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                return 0;
            }
            gnLastItemUseTick = GetTickCount();
        }
        MSG_SendServerItemMessage(76, pSlotItem->dwUnitId);
        ITEM_SetUninterruptableState(pSlotItem);
        return 1;
    }

    //  an item is on the cursor
    if (pCursorItem)
    {
        pHoveredItem = NULL;
        D2COMMON_INVENTORY_CanItemBePlaced(pInventory, pCursorItem, gnHoverCellX, gnHoverCellY, nInventoryRecordId, &pHoveredItem, &dwHoveredItems, (uint8_t)nInvPage);
        bStillPlaceable = 1;
        pPlacedItem = NULL;

        //  more than one item under the drop: if one of them is the cube, the cube arm wins outright
        if (dwHoveredItems > 1)
        {
            pExchangeTarget = NULL;
            D2COMMON_INVENTORY_CanItemsBeExchanged(pInventory, pCursorItem, gnHoverCellX, gnHoverCellY, nInventoryRecordId, &pExchangeTarget, (uint8_t)nInvPage, 1);
            if (pExchangeTarget && D2COMMON_DATATBLS_GetItemsTxtRecord(pExchangeTarget->dwClassId)->dwCode == 544763746)
            {
                pHoveredItem = pExchangeTarget;
                goto INTO_THE_CUBE;
            }
        }

        if (!pHoveredItem || dwHoveredItems != 1)
        {
            //  the cube refuses to go inside itself
            if ((uint8_t)nInvPage == INVPAGE_CUBE)
            {
                pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(pCursorItem->dwClassId);
                if (!pItemsTxt)
                {
                    FOG_DisplayAssert(szPtItemStatsAssert, szInvCpp, 5655);
                    exit(-1);
                }
                if (pItemsTxt->dwCode == 544763746)
                {
                    return 1;
                }
            }

            //  the same centring INV_PixelToGridCell does, written out again
            nGridCol = (nXPos - pGridInfo->nGridLeft) / (uint32_t)pGridInfo->nGridBoxWidth;
            nGridRow = (nYPos - pGridInfo->nGridTop) / (uint32_t)pGridInfo->nGridBoxHeight;
            D2COMMON_ITEMS_GetDimensions(pCursorItem, &nItemW, &nItemH, szD2CommonCmnItemsH, 1754);
            GFXUTIL_GetUnitCelSize(pCursorItem, 0, 0, &nCelW, &nCelH);

            if ((nItemW & 1) == 0)
            {
                nGridCol = (nXPos + ((uint32_t)nCelW >> 2) - pGridInfo->nGridLeft) / pGridInfo->nGridBoxWidth;
            }
            if (nItemW == pGridInfo->nGridX)
            {
                nGridCol = pGridInfo->nGridX >> 1;
            }
            if ((nItemH & 1) == 0)
            {
                nGridRow = (nYPos + ((uint32_t)nCelH >> 2) - pGridInfo->nGridTop) / pGridInfo->nGridBoxHeight;
            }
            if (nItemH == pGridInfo->nGridY)
            {
                nGridRow = pGridInfo->nGridY >> 1;
            }
            if (nItemW > 1u)
            {
                nGridCol = nGridCol - (nItemW >> 1) < 0 ? 0 : nGridCol - (nItemW >> 1);
            }
            if (nItemH > 1u)
            {
                nGridRow = nGridRow - (nItemH >> 1) < 0 ? 0 : nGridRow - (nItemH >> 1);
            }

            if (!D2COMMON_INVENTORY_CanItemBePlaced(pInventory, pCursorItem, nGridCol, nGridRow, nInventoryRecordId, &pCanPlaceItem, &nCanPlaceCount, (uint8_t)nInvPage) || ITEM_IsItemLocked(pCursorItem))
            {
                return bHandled;
            }
            if (GAME_GetGameType() && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
            {
                if (TRADE_TickTimeout())
                {
                    goto GREEN_CHECK_HANDLED;
                }
                if (GetTickCount() - gnLastItemUseTick <= 0xFA)
                {
                    return bHandled;
                }
                gnLastItemUseTick = GetTickCount();
            }
            MSG_SendCmd4Dwords(24, pCursorItem->dwUnitId, nGridCol, nGridRow, (uint8_t)nInvPage);
            ITEM_SetUninterruptableState(pCursorItem);
            nSoundId = ITEM_GetDropSoundNoFrame(pCursorItem);
            SOUNDINST_PlaySound(nSoundId, NULL, 0, 0, 0);
            return 1;
        }

        //  exactly one item under the drop: stack it if the two stack
        if (D2COMMON_ITEMS_AreStackablesEqual(pCursorItem, pHoveredItem))
        {
            if (ITEM_IsItemLocked(pCursorItem))
            {
                return bHandled;
            }
            if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
            {
                if (TRADE_TickTimeout())
                {
                    goto GREEN_CHECK_HANDLED;
                }
                if (GetTickCount() - gnLastItemUseTick <= 0xFA)
                {
                    return bHandled;
                }
                gnLastItemUseTick = GetTickCount();
            }
            if (sgInventoryMode == 11 || (uint8_t)nInvPage == INVPAGE_TRADE)
            {
                return bHandled;
            }
            MSG_SendCmdUnit(33, pCursorItem->dwUnitId, pHoveredItem ? (int32_t)pHoveredItem->dwUnitId : -1);
            ITEM_SetUninterruptableState(pCursorItem);
            nSoundId = ITEM_GetDropSoundNoFrame(pCursorItem);
            SOUNDINST_PlaySound(nSoundId, NULL, 0, 0, 0);
            return 1;
        }

        if (gnHoverCellX < 0 || gnHoverCellY < 0)
        {
            return bHandled;
        }

        //  a gem into a socketed item
        if (INV_CanInsertSocketFiller(pCursorItem, pHoveredItem) && !ITEM_IsItemLocked(pCursorItem))
        {
            if (GAME_GetGameType() && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
            {
                if (TRADE_TickTimeout())
                {
                    //  sends and CONTINUES -- one of the two arms that do
                    TRADE_SendGreenCheck();
                    goto AFTER_SOCKET;
                }
                if (GetTickCount() - gnLastItemUseTick <= 0xFA)
                {
                    goto AFTER_SOCKET;
                }
                gnLastItemUseTick = GetTickCount();
            }
            nUnitGUID = pHoveredItem ? (int32_t)pHoveredItem->dwUnitId : -1;
            MSG_SendCmdUnit(40, pCursorItem->dwUnitId, nUnitGUID);
            ITEM_SetUninterruptableState(pCursorItem);
            nSoundId = ITEM_GetDropSoundNoFrame(pHoveredItem);
            SOUNDINST_PlaySound(nSoundId, NULL, 0, 0, 0);
            bStillPlaceable = 0;
            bHandled = 1;
        }

AFTER_SOCKET:
        //  a scroll into a tome of the same kind
        if (D2COMMON_ITEMS_CheckItemTypeId(pCursorItem, ITEMTYPE_SCROLL) && D2COMMON_ITEMS_CheckItemTypeId(pHoveredItem, ITEMTYPE_BOOK))
        {
            nSuffixId = D2COMMON_ITEMS_GetSuffixId(pHoveredItem, 0);
            if (D2COMMON_ITEMS_GetSuffixId(pCursorItem, 0) == nSuffixId && !ITEM_IsItemLocked(pCursorItem))
            {
                if (GAME_GetGameType() && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
                {
                    if (TRADE_TickTimeout())
                    {
                        //  the second arm that sends and continues
                        TRADE_SendGreenCheck();
                        goto AFTER_TOME;
                    }
                    if (GetTickCount() - gnLastItemUseTick <= 0xFA)
                    {
                        goto AFTER_TOME;
                    }
                    gnLastItemUseTick = GetTickCount();
                }
                if (sgInventoryMode != 11)
                {
                    SPELLSEL_UnbindEmptyScrollHotkeys();
                    nUnitGUID = pHoveredItem ? (int32_t)pHoveredItem->dwUnitId : -1;
                    MSG_SendCmdUnit(41, pCursorItem->dwUnitId, nUnitGUID);
                    ITEM_SetUninterruptableState(pCursorItem);
                    nSoundId = ITEM_GetDropSoundNoFrame(pHoveredItem);
                    SOUNDINST_PlaySound(nSoundId, NULL, 0, 0, 0);
                    bStillPlaceable = 0;
                    bHandled = 1;
                }
            }
        }

AFTER_TOME:
        //  into the cube
        if (D2COMMON_DATATBLS_GetItemsTxtRecord(pHoveredItem ? pHoveredItem->dwClassId : -1)->dwCode != 544763746)
        {
            goto TRY_EXCHANGE;
        }

INTO_THE_CUBE:
        if (ITEM_IsItemLocked(pCursorItem))
        {
            goto TRY_EXCHANGE;
        }
        if (GAME_GetGameType() && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                TRADE_SendGreenCheck();
                goto TRY_EXCHANGE;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                goto TRY_EXCHANGE;
            }
            gnLastItemUseTick = GetTickCount();
        }
        if (!D2COMMON_INVENTORY_GetFreePosition(pInventory, pCursorItem, (D2C_InventoryRecords)D2COMMON_UNITS_GetInventoryRecordId(pUnit, INVPAGE_CUBE, GAME_IsExpansionGame()), &nFreeY, &nFreeX, 3u))
        {
            UNITSND_PlayPlayerSoundEvent(pUnit, 0x13u);
            return bHandled;
        }
        nUnitGUID = pHoveredItem ? (int32_t)pHoveredItem->dwUnitId : -1;
        MSG_SendCmdUnit(42, pCursorItem->dwUnitId, nUnitGUID);
        ITEM_SetUninterruptableState(pCursorItem);
        nSoundId = ITEM_GetDropSoundNoFrame(pHoveredItem);
        SOUNDINST_PlaySound(nSoundId, NULL, 0, 0, 0);
        return 1;

TRY_EXCHANGE:
        if (!bStillPlaceable || !D2COMMON_INVENTORY_CanItemsBeExchanged(pInventory, pCursorItem, gnHoverCellX, gnHoverCellY, nInventoryRecordId, &pPlacedItem, (uint8_t)nInvPage, 0) || ITEM_IsItemLocked(pCursorItem))
        {
            return bHandled;
        }
        if (GAME_GetGameType() && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                goto GREEN_CHECK_HANDLED;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                return bHandled;
            }
            gnLastItemUseTick = GetTickCount();
        }
        if ((uint8_t)nInvPage == INVPAGE_TRADE)
        {
            return bHandled;
        }
        MSG_SendCmd4Dwords(31, pCursorItem->dwUnitId, pPlacedItem ? (int32_t)pPlacedItem->dwUnitId : -1, gnHoverCellX, gnHoverCellY);
        ITEM_SetUninterruptableState(pCursorItem);
        nSoundId = ITEM_GetDropSoundNoFrame(pCursorItem);
        SOUNDINST_PlaySound(nSoundId, NULL, 0, 0, 0);
        return 1;

GREEN_CHECK_HANDLED:
        TRADE_SendGreenCheck();
        return bHandled;
    }

    //  nothing on the cursor: pick up, belt, sell or trade
    pSlotItem = D2COMMON_INVENTORY_GetItemFromInventoryPage(pInventory, nGridX, nGridY, &pX, &pY, nInventoryRecordId, (uint8_t)nInvPage);
    if (!pSlotItem)
    {
        return bHandled;
    }

    pPlayer = CUNIT_GetPlayer();
    bPlayerOwnsItem = pUnit == pPlayer;

    switch (sgInventoryMode)
    {
    case 0:
    case 1:
    case 2:
    case 11:
    case 12:
    case 14:
    case 19:
        if (!bPlayerOwnsItem)
        {
            goto TRY_TRANSACTION;
        }
        if (ITEM_IsItemLocked(pSlotItem))
        {
            return bHandled;
        }
        if (GAME_GetGameType() != GAMETYPE_SINGLEPLAYER && GAME_GetGameType() != GAMETYPE_SINGLEPLAYER_UNCAPPED)
        {
            if (TRADE_TickTimeout())
            {
                TRADE_SendGreenCheck();
                return 0;
            }
            if (GetTickCount() - gnLastItemUseTick <= 0xFA)
            {
                return bHandled;
            }
            gnLastItemUseTick = GetTickCount();
        }

        //  VK_CONTROL from the KEYBOARD, MK_SHIFT from the MESSAGE
        if ((((uint16_t)GetAsyncKeyState(17) >> 8) & 0x80u) == 0)
        {
            if ((nMouseFlags & 4) != 0 && D2COMMON_ITEMS_CheckIfBeltable(pSlotItem) && !D2COMMON_ITEMS_GetInvPage(pSlotItem))
            {
                MSG_SendServerItemMessage(99, pSlotItem->dwUnitId);
                SOUNDINST_PlaySound(SOUND_ITEM_PICKUP, NULL, 0, 0, 0);
                ITEM_SetUninterruptableState(pSlotItem);
                return 1;
            }
            MSG_SendServerItemMessage(25, pSlotItem->dwUnitId);
            gbDeselectShopTabsPending = 1;
        }
        else if (NPCMENU_GetInteractingNpc(&dwNpcGUID))
        {
            pNpc = CUNIT_FindUnitByGuidAndType(dwNpcGUID, UNIT_MONSTER);
            if (pNpc)
            {
                dwClassId = pNpc->dwClassId;
                pQuestRecord = NPCMENU_QuestGetQuestRecord();
                nDifficulty = (D2C_Difficulties)GAME_GetDifficulty();
                nTransactionCost = D2COMMON_ITEMS_GetTransactionCost(CUNIT_GetPlayer(), pSlotItem, nDifficulty, pQuestRecord, dwClassId, TRANSACTIONTYPE_SELL);
                MSG_SendCmd4Dwords(51, pNpc->dwUnitId, pSlotItem->dwUnitId, pSlotItem->dwAnimMode, nTransactionCost);
                ITEM_SetUninterruptableState(pSlotItem);
                return 1;
            }
        }
        ITEM_SetUninterruptableState(pSlotItem);
        return 1;

    default:
TRY_TRANSACTION:
        //  the pointer comparison that shares this flag in the original is a leftover nothing reads
        if (sgInventoryMode >= 19 || !NPCMENU_BeginTransaction(pUnit, pSlotItem, bPlayerOwnsItem, nXPos, nYPos, 0, 0, 0))
        {
            return bHandled;
        }
        sgInventoryMode = 5;
        CURSOR_SetHeldUnit(NULL);
        return 0;
    }
}


//  The item description's lines (0x6FAE3EE0..0x6FAE43D0): five functions in one contiguous run. INV_DrawItemDescription builds the hover box a
//  line at a time and these are five of the lines. Every one has the same shape:
//      Unicode wszNumber[10];            the scratch, constructed
//      space   = tbl 3995                fetched before anything is decided
//      newline = tbl 3998
//      strcpy(wszOut, <the label>)       the line replaces the buffer
//      SStrPrintf(szNumber, 10, "%i", n) the number, in ASCII
//      win2Unicode(wszNumber, szNumber, 9)
//      strcat(wszOut, space)
//      strcat(wszOut, wszNumber)
//      strcat(wszOut, newline)
//  strcpy, not strcat, on the first line: each of these owns the buffer it is handed and the caller concatenates the results itself. The two
//  requirement lines that can be empty write wszOut->ch = 0 instead, which is the same contract stated the other way.
//  The scratch is ten and the conversion is nine: Unicode wszNumber[10] is constructed element by element and
//  Unicode::win2Unicode(wszNumber, szNumber, 9) is handed nine, the array minus its terminator (the pair occurs four times in this run with the
//  same numbers, and once with 16 and 15). The ASCII buffer is written [10]; SStrPrintf(szNumber, 10, ...) takes its capacity as 10, and
//  nothing reads the buffer's size.
//  The tbl indices: 3457 ItemStats1d "Durability:", 3463 ItemStats1j "of", 3469 ItemStats1p "Required Level:", 3458 ItemStats1e "Required
//  Strength:", 3459 ItemStats1f "Required Dexterity:", 3453 Socketable, 22745 X_Ethereal_CanNotBeRepaired, 3995 space, 3998 newline.

//  "Ethereal": written into a buffer nothing reads (see INV_AppendEtherealAndSocketsText).
static const char szEthereal[] = "Ethereal";


//1.10f: D2Client.0x6FAE3EE0
//  Ethereal and sockets on one line: the only one of the five that can put two facts on a line. The bAppended flag picks between four outcomes:
//      neither          the buffer is emptied and nothing else happens
//      ethereal only    tbl 22745, then a newline
//      sockets only     tbl 3453, a space, "(n)", a newline
//      both             tbl 22745, ", ", tbl 3453, a space, "(n)", a newline
//  The buffer is emptied through D2Lang, not by a store (6FAE3F1E..6FAE3F35): a sixteen-element scratch is constructed, an empty ANSI string is
//  widened into it (win2Unicode, count 15) and the result is copied over the output with Unicode::strcpy. wszOut->ch = 0 would do the same in one
//  store; it is reproduced as written.
//  BUG: the widened "Ethereal" (win2Unicode of the literal at 6FAE3F5A..6FAE3F65) is overwritten by the next win2Unicode and never appended; the
//  text that reaches the player is tbl 22745 in every case. LATENT: the call cannot fail, the buffer it writes is large enough and no path reads
//  the result.
//  The two asserts push inv.cpp's own __FILE__: 6FAE3F40 push 6BFh (line 1727, the ethereal check) and 6FAE3F84 push 6C7h (line 1735, the
//  socketed check).
//  The socket count is masked to a byte (6FAE3FFE call ITEMS_GetSockets, 6FAE4003 and eax, 0FFh): the import is declared int and the call site
//  takes eight bits of it, so the cast is the original's.
void __fastcall INV_AppendEtherealAndSocketsText(D2UnitStrc *pItem, Unicode *wszOut)
{
    const Unicode *pwszNewline;
    const Unicode *pwszSpace;
    int32_t bAppended;
    char szSockets[10];

    bAppended = 0;

    Unicode wszScratch[16];

    pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);

    //  the buffer is emptied through D2Lang
    Unicode::win2Unicode(wszScratch, &gszEmptyString, 15);
    Unicode::strcpy(wszOut, wszScratch);

    if (D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_ETHEREAL, 1727, szInvCpp))
    {
        //  the dead conversion: its output is overwritten before any read
        Unicode::win2Unicode(wszScratch, szEthereal, 15);
        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_22745_X_Ethereal_CanNotBeRepaired));
        bAppended = 1;
    }

    if (D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_SOCKETED, 1735, szInvCpp))
    {
        if (bAppended)
        {
            //  ", " -- a global, not a literal: gszCommaSpace at 0xD9918
            Unicode::win2Unicode(wszScratch, gszCommaSpace, 15);
            Unicode::strcat(wszOut, wszScratch);
        }
        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3453_Socketable));

        //  constructed here, inside the branch, which is where the second loop is: an item with no sockets never pays for it
        Unicode wszSockets[10];

        pwszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
        STORM_SStrPrintf(szSockets, 10, "(%i)", (uint8_t)D2COMMON_ITEMS_GetSockets(pItem));
        Unicode::win2Unicode(wszSockets, szSockets, 9);
        Unicode::strcat(wszOut, pwszSpace);
        Unicode::strcat(wszOut, wszSockets);
        Unicode::strcat(wszOut, pwszNewline);
    }
    else if (bAppended)
    {
        Unicode::strcat(wszOut, pwszNewline);
    }
}


//1.10f: D2Client.0x6FAE4060
//  Durability: the longest line in the run: label, current, "of", maximum, newline, with a space between every pair.
//  Three guards, and the second is called twice: ITEMS_HasDurability(pItem); STATLIST_GetMaxDurabilityFromUnit(pItem) > 0;
//  !ITEMS_CheckIfThrowable(pItem). STATLIST_GetMaxDurabilityFromUnit is called again further down for the number itself, so an item that
//  shows a durability line pays for that lookup twice (the original has no cached value). A throwable weapon has durability and does not
//  show it; its quantity line stands in for it.
//  The colour lands on the maximum and only on the maximum: bMaxDurabilityBonus = STATLIST_GetUnitStatBonus(pItem, 0x4B, 0) != 0, and
//  if (bMaxDurabilityBonus) UI_AddColorCodeToWstring(wszNumber, 3). Stat 0x4B is item_maxdurability_percent and the code is applied to the second
//  win2Unicode, the maximum, so the highlight marks the number the bonus moved. The flag is computed at the top of the branch, before either
//  number exists.
//  The third parameter, pItemsTxt, is passed and never read; it stays in the signature because the caller pushes it.
void __fastcall INV_AppendDurabilityText(D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt)
{
    const Unicode *pwszSpace;
    const Unicode *pwszNewline;
    int32_t bMaxDurabilityBonus;
    char szNumber[10];

    Unicode wszNumber[10];

    pwszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
    pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);

    if (D2COMMON_ITEMS_HasDurability(pItem) && D2COMMON_STATLIST_GetMaxDurabilityFromUnit(pItem) > 0 && !D2COMMON_ITEMS_CheckIfThrowable(pItem))
    {
        bMaxDurabilityBonus = D2COMMON_STATLIST_GetUnitStatBonus(pItem, STAT_ITEM_MAXDURABILITY_PERCENT, 0) != 0;

        Unicode::strcpy(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3457_ItemStats1d));
        STORM_SStrPrintf(szNumber, 10, "%i", D2COMMON_STATLIST_UnitGetStatValue(pItem, STAT_DURABILITY, 0));
        //  the space is appended before the number is widened; the order is the original's
        Unicode::strcat(wszOut, pwszSpace);
        Unicode::win2Unicode(wszNumber, szNumber, 9);
        Unicode::strcat(wszOut, wszNumber);

        Unicode::strcat(wszOut, pwszSpace);
        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3463_ItemStats1j));
        Unicode::strcat(wszOut, pwszSpace);

        STORM_SStrPrintf(szNumber, 10, "%i", D2COMMON_STATLIST_GetMaxDurabilityFromUnit(pItem));
        Unicode::win2Unicode(wszNumber, szNumber, 9);
        if (bMaxDurabilityBonus)
            UI_AddColorCodeToWstring((wchar_t *)wszNumber, 3);
        Unicode::strcat(wszOut, wszNumber);
        Unicode::strcat(wszOut, pwszNewline);
    }
}


//1.10f: D2Client.0x6FAE41B0
//  The required level, 149 bytes: the shortest of the three requirement lines because it has no test; a level requirement is printed whatever it
//  is, including zero, and the caller decides whether to call at all. The scratch is constructed before the two tbl lookups, so the ten calls into
//  D2Lang happen whether or not the line is wanted, but there is no path here that does not want it.
void __fastcall INV_FormatReqLevelLine(Unicode *wszOut, int32_t nLevel)
{
    const Unicode *pwszSpace;
    const Unicode *pwszNewline;
    char szLevel[10];

    Unicode wszLevel[10];

    pwszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
    pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);

    Unicode::strcpy(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3469_ItemStats1p));
    STORM_SStrPrintf(szLevel, 10, "%i", nLevel);
    Unicode::win2Unicode(wszLevel, szLevel, 9);
    Unicode::strcat(wszOut, pwszSpace);
    Unicode::strcat(wszOut, wszLevel);
    Unicode::strcat(wszOut, pwszNewline);
}


//1.10f: D2Client.0x6FAE4250
//  The required strength, 191 bytes. The requirement is the items.txt base plus a delta the caller computed, and the sum is what is tested:
//  nRequired = nDelta + pItemsTxt->wReqStr; if (nRequired > 0) print it, else wszOut->ch = 0. wReqStr is an unsigned 16-bit field, so the base
//  cannot be negative and the delta is what can take the sum to zero or below (a set or runeword bonus that reduces requirements); an item whose
//  requirement is cancelled shows no line rather than "Required Strength: 0". The scratch is constructed before the test, so an empty line still
//  pays for ten calls into D2Lang.
void __fastcall INV_FormatReqStrengthLine(Unicode *wszOut, D2ItemsTxt *pItemsTxt, int32_t nDelta)
{
    const Unicode *pwszSpace;
    const Unicode *pwszNewline;
    int32_t nReqStrength;
    char szNumber[10];

    Unicode wszNumber[10];

    pwszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
    pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);

    nReqStrength = nDelta + pItemsTxt->wReqStr;
    if (nReqStrength > 0)
    {
        Unicode::strcpy(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3458_ItemStats1e));
        STORM_SStrPrintf(szNumber, 10, "%i", nReqStrength);
        Unicode::win2Unicode(wszNumber, szNumber, 9);
        Unicode::strcat(wszOut, pwszSpace);
        Unicode::strcat(wszOut, wszNumber);
        Unicode::strcat(wszOut, pwszNewline);
    }
    else
    {
        wszOut->ch = 0;
    }
}


//1.10f: D2Client.0x6FAE4310
//  The required dexterity, 191 bytes: the same body as the strength line with wReqDex for wReqStr and tbl 3459 for 3458. Two functions, not one
//  with a parameter; the duplication is the original's.
void __fastcall INV_FormatReqDexterityLine(Unicode *wszOut, D2ItemsTxt *pItemsTxt, int32_t nDelta)
{
    const Unicode *pwszSpace;
    const Unicode *pwszNewline;
    int32_t nReqDexterity;
    char szNumber[10];

    Unicode wszNumber[10];

    pwszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
    pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);

    nReqDexterity = nDelta + pItemsTxt->wReqDex;
    if (nReqDexterity > 0)
    {
        Unicode::strcpy(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3459_ItemStats1f));
        STORM_SStrPrintf(szNumber, 10, "%i", nReqDexterity);
        Unicode::win2Unicode(wszNumber, szNumber, 9);
        Unicode::strcat(wszOut, pwszSpace);
        Unicode::strcat(wszOut, wszNumber);
        Unicode::strcat(wszOut, pwszNewline);
    }
    else
    {
        wszOut->ch = 0;
    }
}


//  More of the description's lines (0x6FAE4C60..0x6FAE5925): seven functions in one contiguous run: the elemental damage range, chance to block,
//  smite and kick damage, defense, stack quantity, attack speed and the item's spell description. They build their lines like the group above
//  (strcpy label / SStrPrintf / win2Unicode(.., 9) / strcat). Where each scratch buffer is declared is where the original's construction loop
//  sits, and that differs: three construct first thing, chance-to-block constructs only after its early return, and the spell description
//  constructs inside the one case that uses it.


//1.10f: D2Client.0x6FAE4C60
//  One elemental damage range, as the tooltip shows it, 634 bytes: the only one of the seven that prints nothing. It computes a (min, max) pair
//  and a flag for INV_AppendElementalDamageText, its only caller, which calls it five times (fire, lightning, and the pair at 159/160). The
//  original inlines DATATBLS_ApplyRatio(v, p, 100) here for the percentage.
//  The defaults come first, before anything is tested: *pnMinDamage = 1; *pnMaxDamage = 2, so a missing unit or item still leaves the caller a
//  well-formed "1-2". *pbAdjusted is never cleared here, only raised at three sites, so the caller's own initial value is what a range with no
//  adjustment keeps.
//  The stats are merged on, read, and taken off: the item's stat list is merged onto the unit so the reading includes what the character
//  contributes, then unmerged: back through STATLIST_MergeStatLists with the saved dynamic flag when the item has an owner, through
//  STATLIST_ExpireUnitStatlist when it does not. The owner is looked up on gpHoverItem and the merge is done on the parameter; every call is from
//  INV_AppendElementalDamageText with the hover item as that parameter, so the two are the same unit on every path there is.
//  Both base stats are read before either is compared: STATLIST_GetUnitBaseStat is called for the min stat and the max stat before the ||, so a
//  short-circuiting a < b || c < d would skip a call the original makes; the two are computed into locals first.
//  The base time reuses the dead min-stat slot for the period-of-day call's out-parameter and zeroes it first (STAT_STRENGTH is only the enum's
//  name for zero); ours is a local, nBaseTime, with the same zero.
void __fastcall INV_CalcElementalDamageRange(D2UnitStrc *pTarget, D2UnitStrc *pItem, D2C_ItemStats nMinStatId, D2C_ItemStats nMaxStatId, int32_t *pnMinDamage, int32_t *pnMaxDamage, BOOL *pbAdjusted)
{
    D2UnitStrc *pOwner;
    D2UnitStrc *pMergeTarget;
    BOOL bStatNotDynamic;
    BOOL bHasOwner;
    int32_t nMinDamage;
    int32_t nMaxDamage;
    int32_t nBaseMin;
    int32_t nBaseMax;
    int32_t nAmount;
    int32_t nBaseTime;
    int32_t nPeriodOfDay;
    int32_t nAdjust;

    *pnMinDamage = 1;
    pMergeTarget = pTarget;
    *pnMaxDamage = 2;

    if (!pTarget || !pItem)
        return;

    bStatNotDynamic = 0;
    pOwner = D2COMMON_STATLIST_GetOwner(gpHoverItem, &bStatNotDynamic);
    bHasOwner = pOwner != NULL;
    if (pOwner && pOwner != pTarget)
    {
        pMergeTarget = pOwner;
        pTarget = pOwner;
    }

    D2COMMON_STATLIST_MergeStatLists(pTarget, pItem, 1);
    nMinDamage = D2COMMON_STATLIST_UnitGetStatValue(pItem, nMinStatId, 0);
    nMaxDamage = D2COMMON_STATLIST_UnitGetStatValue(pItem, nMaxStatId, 0);
    if (bHasOwner)
        D2COMMON_STATLIST_MergeStatLists(pMergeTarget, pItem, bStatNotDynamic);
    else
        D2COMMON_STATLIST_ExpireUnitStatlist(pMergeTarget, pItem);

    //  both, before the test
    nBaseMin = D2COMMON_STATLIST_GetUnitBaseStat(pItem, nMinStatId, 0);
    nBaseMax = D2COMMON_STATLIST_GetUnitBaseStat(pItem, nMaxStatId, 0);
    if (nBaseMin < nMinDamage || nBaseMax < nMaxDamage)
        *pbAdjusted = 1;

    if (nMaxDamage <= nMinDamage)
        nMaxDamage = nMinDamage;

    //  the flat by-time-of-day bonus, stat 272
    nAmount = D2COMMON_STATLIST_UnitGetStatValue(pItem, STAT_ITEM_MAXDAMAGE_BYTIME, 0);
    if (nAmount && STAT_ITEM_MAXDAMAGE_BYTIME < sgptDataTables->nItemStatCostTxtRecordCount && &sgptDataTables->pItemStatCostTxt[STAT_ITEM_MAXDAMAGE_BYTIME] && pMergeTarget->pDrlgAct)
    {
        nBaseTime = 0;
        nPeriodOfDay = D2COMMON_ENVIRONMENT_GetPeriodOfDayFromAct(pMergeTarget->pDrlgAct, &nBaseTime);
        nAdjust = D2COMMON_ITEMMODS_GetByTimeAdjustment(nAmount, nPeriodOfDay, nBaseTime, NULL, NULL, NULL);
        nMaxDamage += nAdjust;
        if (nAdjust)
            *pbAdjusted = 1;
    }

    //  the percentage one, stat 273, through the inlined DATATBLS_ApplyRatio
    nAmount = D2COMMON_STATLIST_UnitGetStatValue(pItem, STAT_ITEM_MAXDAMAGE_PERCENT_BYTIME, 0);
    if (nAmount && STAT_ITEM_MAXDAMAGE_PERCENT_BYTIME < sgptDataTables->nItemStatCostTxtRecordCount && &sgptDataTables->pItemStatCostTxt[STAT_ITEM_MAXDAMAGE_PERCENT_BYTIME] && pMergeTarget->pDrlgAct)
    {
        nBaseTime = 0;
        nPeriodOfDay = D2COMMON_ENVIRONMENT_GetPeriodOfDayFromAct(pMergeTarget->pDrlgAct, &nBaseTime);
        nAdjust = D2COMMON_ITEMMODS_GetByTimeAdjustment(nAmount, nPeriodOfDay, nBaseTime, NULL, NULL, NULL);
        nMaxDamage += DATATBLS_ApplyRatio(nMaxDamage, nAdjust, 100);
        if (nAdjust)
            *pbAdjusted = 1;
    }

    *pnMinDamage = nMinDamage;
    *pnMaxDamage = nMaxDamage;
}


//1.10f: D2Client.0x6FAE4EE0
//  Chance to block, 339 bytes: the item's STAT_TOBLOCK, plus the class's BlockFactor, plus Holy Shield while its state is up, capped at 75 and
//  coloured when it beats the shield's own ItemsTxt nBlock.
//  BUG: the class lookup faults on its own out-of-range arm. An inlined DATATBLS_GetCharStatsTxtRecord honours the bounds test (6FAE4F08..18,
//  against nCharStatsTxtRecordCount) by producing NULL (6FAE4F2D xor eax, eax) and nothing tests the NULL, so an out-of-range class id reads the
//  BlockFactor at address 0x49 (6FAE4F36 mov cl, [eax+49h]). Reproduced. UNREACHABLE: the unit is the player, whose class is one of the seven
//  playable ones, and CharStats.txt has a row for each.
//  The cap is inside the player test: with no player the block is printed uncapped (if (!Player) goto lands past cmp ebx, 4Bh); only the zero
//  test is shared.
//  BUG: the conversion is given the whole buffer, win2Unicode(wszPercent, .., 32) with a 32-element array (6FAE4FC6 push 20h), where everywhere
//  else in this unit the count is the array minus one. D2Lang's Unicode::win2Unicode writes at most count characters, but a source exactly count
//  long gets its terminator at dest[count], one past the array. INERT: the source is "%d%%\n", at most thirteen characters for any int, so it
//  can never be 32 long.
void __fastcall INV_AppendChanceToBlockText(D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt)
{
    D2UnitStrc *pPlayer;
    const D2CharStatsTxt *pCharStatsTxt;
    D2SkillStrc *pSkill;
    int32_t nBlock;
    int32_t nClassId;
    int32_t nSkillLevel;
    char szPercent[32];

    nBlock = D2COMMON_STATLIST_UnitGetStatValue(pItem, STAT_TOBLOCK, 0);
    pPlayer = CUNIT_GetPlayer();
    if (pPlayer)
    {
        nClassId = pPlayer->dwClassId;
        if (nClassId < 0 || nClassId >= sgptDataTables->nCharStatsTxtRecordCount)
        {
            pCharStatsTxt = NULL;
        }
        else
        {
            pCharStatsTxt = &sgptDataTables->pCharStatsTxt[nClassId];
        }

        //  the fault, when the arm above produced the NULL; written as the original wrote it, deliberately
        nBlock += pCharStatsTxt->nBlockFactor;

        pSkill = D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pPlayer, SKILL_HOLYSHIELD);
        if (pSkill && D2COMMON_STATES_CheckState(pPlayer, STATE_HOLYSHIELD))
        {
            nSkillLevel = D2COMMON_SKILLS_GetSkillLevel(pPlayer, pSkill, 1);
            nBlock += D2COMMON_11036_GetMonCurseResistanceSubtraction(nSkillLevel, SKILL_HOLYSHIELD);
        }

        if (nBlock > 75)
            nBlock = 75;
        else if (!nBlock)
            return;
    }
    else if (!nBlock)
    {
        return;
    }

    //  constructed only now, past the early returns: the original's two loops are here, and an item with no block never pays for them
    Unicode wszPercent[32];
    Unicode wszLabel[256];

    STORM_SStrPrintf(szPercent, 32, "%d%%\n", nBlock);
    Unicode::win2Unicode(wszPercent, szPercent, 32);   // the whole buffer
    if (nBlock > pItemsTxt->nBlock)
        UI_AddColorCodeToWstring((wchar_t *)wszPercent, 3);

    Unicode::strcpy(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_11018_ItemStats1r));
    UI_AddColorCodeToWstring((wchar_t *)wszLabel, 0);
    Unicode::strcpy(wszOut, wszLabel);
    Unicode::strcat(wszOut, wszPercent);
}


//1.10f: D2Client.0x6FAE5040
//  Smite and kick damage, 397 bytes: the two lines where the body is the weapon. A shield gets tbl 3468 (ItemStats1o) and boots tbl 21782
//  (ModStre10k), joined by 3464 (ItemStast1k, the tbl key's own spelling); anything else draws nothing.
//  The range is the item's own nMinDam / nMaxDam. A shield adds Holy Shield's physical damage on top while its state is up, and the skill answers
//  in 8.8 fixed point: sar eax, 8 at 0x6FAE50E7 and 0x6FAE50FF, an arithmetic shift, so a negative bonus stays negative.
//  The player is not null-checked before SKILLS_GetHighestLevelSkill...; that is D2Common's to decide, and its answer for a NULL unit is NULL.
//  nMinDam lives in a positive frame slot, [ebp+4h], the stack argument pItemsTxt arrived in, reused once the two damage bytes are read out of
//  it; it is a local here.
void __fastcall INV_AppendSkillDamageText(D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt)
{
    const Unicode *pwszSpace;
    const Unicode *pwszNewline;
    D2UnitStrc *pPlayer;
    D2SkillStrc *pSkill;
    D2C_StringIndices nLabelStrId;
    int32_t nMinDam;
    int32_t nMaxDam;
    int32_t nSkillLevel;
    char szNumber[10];

    Unicode wszNumber[10];

    pwszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
    pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);

    nMaxDam = pItemsTxt->nMaxDam;
    nMinDam = pItemsTxt->nMinDam;

    if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_ANY_SHIELD))
    {
        nLabelStrId = STR_IDX_3468_ItemStats1o;
        pPlayer = CUNIT_GetPlayer();
        pSkill = D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId(pPlayer, SKILL_HOLYSHIELD);
        if (pSkill && D2COMMON_STATES_CheckState(pPlayer, STATE_HOLYSHIELD))
        {
            nSkillLevel = D2COMMON_SKILLS_GetSkillLevel(pPlayer, pSkill, 1);
            nMinDam += D2COMMON_SKILLS_GetMinPhysDamage(pPlayer, SKILL_HOLYSHIELD, nSkillLevel, 1) >> 8;
            nMaxDam += D2COMMON_SKILLS_GetMaxPhysDamage(pPlayer, SKILL_HOLYSHIELD, nSkillLevel, 1) >> 8;
        }
    }
    else
    {
        if (!D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_BOOTS))
            return;
        nLabelStrId = STR_IDX_21782_ModStre10k;
    }

    Unicode::strcpy(wszOut, D2LANG_GetStringFromTblIndex(nLabelStrId));
    STORM_SStrPrintf(szNumber, 10, "%i", nMinDam);
    Unicode::win2Unicode(wszNumber, szNumber, 9);
    Unicode::strcat(wszOut, pwszSpace);
    Unicode::strcat(wszOut, wszNumber);
    Unicode::strcat(wszOut, pwszSpace);
    Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3464_ItemStast1k));
    STORM_SStrPrintf(szNumber, 10, "%i", nMaxDam);
    Unicode::win2Unicode(wszNumber, szNumber, 9);
    Unicode::strcat(wszOut, pwszSpace);
    Unicode::strcat(wszOut, wszNumber);
    Unicode::strcat(wszOut, pwszNewline);
}


//1.10f: D2Client.0x6FAE51D0
//  Defense, 725 bytes: the same merge-read-unmerge as the damage range, on STATLIST_GetDefenseFromUnit, with stats 268 (flat) and 269 (percent,
//  through the inlined DATATBLS_ApplyRatio) added for the time of day. The number is coloured when it differs from the item's base
//  STAT_ARMORCLASS or when either time adjustment moved it.
//  gpHoverItem is read afresh at each of its four uses (the owner lookup, both merges and the defense read), where pItem is used only for the
//  base stat and the two time stats. The loads are the original's.
//  The inlined DATATBLS_GetItemStatCostTxtRecord(268) test is &pItemStatCostTxt[268] != NULL with the stride folded in (268 * 324 = 86832, and
//  324 is sizeof(D2ItemStatCostTxt)).
//  The fourth parameter, pItemsTxt, is never read; the siblings take the same record and both call sites push it.
void __fastcall INV_AppendDefenseText(D2UnitStrc *pUnit, D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt)
{
    const Unicode *pwszSpace;
    const Unicode *pwszNewline;
    D2UnitStrc *pOwner;
    BOOL bStatNotDynamic;
    BOOL bHasOwner;
    BOOL bDefenseModified;
    int32_t nDefense;
    int32_t nAmount;
    int32_t nBaseTime;
    int32_t nPeriodOfDay;
    int32_t nAdjust;
    char szDefense[10];

    if (!pUnit)
        return;

    Unicode wszDefense[10];

    pwszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
    pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);

    bStatNotDynamic = 0;
    pOwner = D2COMMON_STATLIST_GetOwner(gpHoverItem, &bStatNotDynamic);
    bHasOwner = pOwner != NULL;
    if (pOwner && pUnit != pOwner)
        pUnit = pOwner;

    D2COMMON_STATLIST_MergeStatLists(pUnit, gpHoverItem, 1);
    nDefense = D2COMMON_STATLIST_GetDefenseFromUnit(gpHoverItem);
    if (bHasOwner)
        D2COMMON_STATLIST_MergeStatLists(pOwner, gpHoverItem, bStatNotDynamic);
    else
        D2COMMON_STATLIST_ExpireUnitStatlist(pUnit, gpHoverItem);

    bDefenseModified = D2COMMON_STATLIST_GetUnitBaseStat(pItem, STAT_ARMORCLASS, 0) != nDefense;

    nAmount = D2COMMON_STATLIST_UnitGetStatValue(pItem, STAT_ITEM_ARMOR_BYTIME, 0);
    if (nAmount && STAT_ITEM_ARMOR_BYTIME < sgptDataTables->nItemStatCostTxtRecordCount && &sgptDataTables->pItemStatCostTxt[STAT_ITEM_ARMOR_BYTIME] && pUnit->pDrlgAct)
    {
        nBaseTime = 0;
        nPeriodOfDay = D2COMMON_ENVIRONMENT_GetPeriodOfDayFromAct(pUnit->pDrlgAct, &nBaseTime);
        nAdjust = D2COMMON_ITEMMODS_GetByTimeAdjustment(nAmount, nPeriodOfDay, nBaseTime, NULL, NULL, NULL);
        nDefense += nAdjust;
        if (nAdjust)
            bDefenseModified = 1;
    }

    nAmount = D2COMMON_STATLIST_UnitGetStatValue(pItem, STAT_ITEM_ARMORPERCENT_BYTIME, 0);
    if (nAmount && STAT_ITEM_ARMORPERCENT_BYTIME < sgptDataTables->nItemStatCostTxtRecordCount && &sgptDataTables->pItemStatCostTxt[STAT_ITEM_ARMORPERCENT_BYTIME] && pUnit->pDrlgAct)
    {
        nBaseTime = 0;
        nPeriodOfDay = D2COMMON_ENVIRONMENT_GetPeriodOfDayFromAct(pUnit->pDrlgAct, &nBaseTime);
        nAdjust = D2COMMON_ITEMMODS_GetByTimeAdjustment(nAmount, nPeriodOfDay, nBaseTime, NULL, NULL, NULL);
        nDefense += DATATBLS_ApplyRatio(nDefense, nAdjust, 100);
        if (nAdjust)
            bDefenseModified = 1;
    }

    //  the space goes in before the number is printed, unlike the lines above; the call order is the original's
    Unicode::strcpy(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3461_ItemStats1h));
    Unicode::strcat(wszOut, pwszSpace);
    STORM_SStrPrintf(szDefense, 10, "%i", nDefense);
    Unicode::win2Unicode(wszDefense, szDefense, 9);
    if (bDefenseModified)
        UI_AddColorCodeToWstring((wchar_t *)wszDefense, 3);
    Unicode::strcat(wszOut, wszDefense);
    Unicode::strcat(wszOut, pwszNewline);
}


//1.10f: D2Client.0x6FAE54B0
//  "Quantity: N", 185 bytes. The test is not the value it prints: if (STAT_QUANTITY > 0 || ITEMS_GetTotalMaxStack(pItem) > 0) it prints
//  STAT_QUANTITY, read a second time, so an item that can stack but holds none prints "Quantity: 0" rather than nothing. The || short-circuits as
//  the original's does: with a positive quantity the max stack is never asked.
//  %ld, not %i (the same number either way on this target). The third parameter is not read. It returns void: the one call site discards the
//  value. The caller has a second, inline copy of this line on its other path (see INV_AppendSpellDescText).
void __fastcall INV_AppendStackQuantityText(D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt)
{
    const Unicode *pwszSpace;
    const Unicode *pwszNewline;
    char szNumber[10];

    Unicode wszNumber[10];

    pwszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
    pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);

    if (D2COMMON_STATLIST_UnitGetStatValue(pItem, STAT_QUANTITY, 0) > 0 || D2COMMON_ITEMS_GetTotalMaxStack(pItem) > 0)
    {
        Unicode::strcpy(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3462_ItemStats1i));
        STORM_SStrPrintf(szNumber, 10, "%ld", D2COMMON_STATLIST_UnitGetStatValue(pItem, STAT_QUANTITY, 0));
        Unicode::win2Unicode(wszNumber, szNumber, 9);
        Unicode::strcat(wszOut, pwszSpace);
        Unicode::strcat(wszOut, wszNumber);
        Unicode::strcat(wszOut, pwszNewline);
    }
}


//1.10f: D2Client.0x6FAE5570
//  "Axe class - fast attack speed", 414 bytes: a weapon-class half and a speed half joined by " - " (tbl 3996, dash).
//  The speed class: speed >= 28 is 5 (Very Slow); 10 <= speed <= 27 is matrix[speed - 10][offsets[2 * class + bDistance]]; speed < 10 is 1 (Very
//  Fast). Both tables are int (add ecx, gptAttackSpeedClassOffsets[eax*4], then mov edx, gptAttackSpeedMatrix[ecx*4]) and the row stride is five
//  (lea ecx, [esi+esi*4-32h] is (speed - 10) * 5). bDistance is item types CROSSBOW and BOW, asked in that order.
//  BUG: with no player the class is -1 (6FAE561A or eax, 0FFFFFFFFh) and the offsets read goes backwards (6FAE5622 lea eax, [ecx+eax*2] is
//  2 * -1 + bDistance, -2 or -1). The matrix is 18 x 5 ints at 0x6FB79360, 360 bytes, and ends exactly at 0x6FB794C8 where the offsets table
//  begins, so offsets[-1] is matrix[17][4] and offsets[-2] is matrix[17][3] (both 5): the column read comes out of the matrix's own last row,
//  deterministic in the original's own data and not a fault. UNREACHABLE: a tooltip is only drawn for a player. Reproduced as written.
//  The class scan is bounded by another global's address (6FAE5655 add esi, 6 / 6FAE5659 cmp esi, offset gWeaponClassDescsEnd, 0x6FB79332):
//  gWeaponClassDescs is 15 six-byte rows from 0x6FB792D8, so the bound is the address one past its end, written + 15 here. The found-index test
//  after the loop (test ebx, ebx / jl) can never fire, since the index only counts up from zero; it is the shape a -1-means-none search compiles to.
//  BUG: a weapon whose type is in none of the fifteen rows appends the speed to the caller's buffer as it stands (only the class half strcpys;
//  the speed half strcats). INERT: both call sites hand it a buffer the caller constructed and has not written, so it appends to an empty string.
void __fastcall INV_AppendAttackSpeedText(D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt)
{
    const Unicode *pwszSpace;
    const Unicode *pwszNewline;
    const Unicode *pwszDash;
    D2UnitStrc *pPlayer;
    D2WeaponDescStrc *pClassDesc;
    int32_t nAttackSpeed;
    int32_t bSpeedBonus;
    int32_t nSpeedIndex;
    int32_t bIsDistanceWeapon;
    int32_t nClassId;
    int32_t nOffsetIndex;
    int32_t nColumn;
    int32_t nFlat;
    int32_t iWeaponClass;
    int32_t nWeaponClass;

    pwszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
    pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
    pwszDash = D2LANG_GetStringFromTblIndex(STR_IDX_3996_dash);

    nAttackSpeed = D2COMMON_ITEMS_GetWeaponAttackSpeed(CUNIT_GetPlayer(), pItem);
    bSpeedBonus = D2COMMON_STATLIST_GetUnitStatBonus(pItem, STAT_ITEM_FASTERATTACKRATE, 0) != 0;

    if (nAttackSpeed >= 28)
    {
        nSpeedIndex = 5;
    }
    else if (nAttackSpeed >= 10)
    {
        pPlayer = CUNIT_GetPlayer();
        bIsDistanceWeapon = D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_CROSSBOW) || D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_BOW);
        nClassId = pPlayer ? pPlayer->dwClassId : -1;

        //  class -1 reads offsets[-2 + bDistance], i.e. matrix[17][3..4]. The bytes index both tables flat (add ecx,
        //  gptAttackSpeedClassOffsets[eax*4], mov edx, gptAttackSpeedMatrix[ecx*4], 0x6FAE5629) and the offsets table begins where the matrix ends;
        //  ours are two objects, so the read the class -1 makes is written by name: offsets[-2] and [-1] are matrix[17][3] and [17][4] (both 5),
        //  and that column 5 reads the next row's first entry, past row 17, offsets[0][0]. The same values as the original's.
        nOffsetIndex = 2 * nClassId + bIsDistanceWeapon;
        nColumn = nOffsetIndex >= 0 ? gptAttackSpeedClassOffsets[nOffsetIndex / 2][nOffsetIndex % 2] : gptAttackSpeedMatrix[17][5 + nOffsetIndex];
        nFlat = 5 * (nAttackSpeed - 10) + nColumn;
        nSpeedIndex = nFlat < 90 ? gptAttackSpeedMatrix[nFlat / 5][nFlat % 5] : gptAttackSpeedClassOffsets[(nFlat - 90) / 2][(nFlat - 90) % 2];
    }
    else
    {
        nSpeedIndex = 1;
    }

    nWeaponClass = -1;
    iWeaponClass = 0;
    //  one past the array, compared signed as the bytes' jl (0x6FAE565F)
    for (pClassDesc = gWeaponClassDescs; (int32_t)pClassDesc < (int32_t)(gWeaponClassDescs + 15); ++pClassDesc, ++iWeaponClass)
    {
        if (D2COMMON_ITEMS_CheckItemTypeId(pItem, (D2C_ItemTypes)pClassDesc->nKey))
        {
            nWeaponClass = iWeaponClass;
            break;
        }
    }

    if (nWeaponClass >= 0)
    {
        Unicode::strcpy(wszOut, D2LANG_GetStringFromTblIndex((D2C_StringIndices) gWeaponClassDescs[nWeaponClass].nStringId));
        Unicode::strcat(wszOut, pwszSpace);
        Unicode::strcat(wszOut, pwszDash);
        Unicode::strcat(wszOut, pwszSpace);
    }

    Unicode wszSpeed[256];

    Unicode::strcpy(wszSpeed, D2LANG_GetStringFromTblIndex((D2C_StringIndices) gWeaponSpeedDescs[nSpeedIndex].nStringId));
    if (bSpeedBonus)
        UI_AddColorCodeToWstring((wchar_t *)wszSpeed, 3);
    Unicode::strcat(wszOut, wszSpeed);
    Unicode::strcat(wszOut, pwszNewline);
}


//1.10f: D2Client.0x6FAE5710
//  The item's spell description, 533 bytes: the line ItemsTxt writes for a misc item rather than a stat. nSpellDesc picks one of four shapes,
//  wSpellDescStr is the tbl string, dwCalc[0] the formula and wStat[0] which stat the value is:
//      1  the string, a newline
//      2  string, space, value; the value through ITEMS_GetBonusLife/ManaBasedOnClass when wStat[0] is HITPOINTS/HPREGEN or MANA/MANARECOVERY
//      3  string, space, value; the plain formula result
//      4  the string as a format, the value substituted, a newline
//  Nothing is drawn without an item, a record, a spelldesc, a player, and a string that is not tbl 5382 (dummy).
//  Cases 2 and 3 share a tail in the bytes and not here: each case's scratch is constructed inside its own case (two loops, at 0x6FAE57D7 and
//  0x6FAE583D, ten elements each into the same slot, and case 4's 512 at 0x6FAE58D1), which a single declaration above the switch could not
//  reproduce. Each case is written whole.
//  BUG: case 4 appends (strcat) where cases 1-3 replace (strcpy), and the caller, on its second path, hands this function the same buffer it has
//  just written the stack-quantity line into (6FAE37B8 call ITEMS_GetTotalMaxStack, 6FAE37D4 strcpy tbl 3462 "Quantity:", the number, a newline,
//  then 6FAE388F call INV_AppendSpellDescText), so a stackable item with a spell description would lose its quantity line to cases 1-3, or have the
//  description run on after it in case 4. LATENT: stock data has no such item (no row of Misc.txt has both a maxstack and a spelldesc, and
//  Weapons.txt and Armor.txt have no spelldesc column); a mod that gives a stackable item a spell description gets the collision.
void __fastcall INV_AppendSpellDescText(D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt)
{
    D2UnitStrc *pPlayer;
    const Unicode *pwszSpace;
    const Unicode *pwszNewline;
    int32_t nValue;
    char szValue[10];

    if (!pItem || !pItemsTxt || !pItemsTxt->nSpellDesc)
        return;
    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer || pItemsTxt->wSpellDescStr == STR_IDX_5382_dummy)
        return;

    pwszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
    pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);

    switch (pItemsTxt->nSpellDesc)
    {
    case 1:
        Unicode::strcpy(wszOut, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pItemsTxt->wSpellDescStr));
        Unicode::strcat(wszOut, pwszNewline);
        return;

    case 2:
    {
        Unicode wszValue[10];

        nValue = D2COMMON_ITEMMODS_EvaluateItemFormula(pPlayer, pItem, pItemsTxt->dwCalc[0]);
        //  the original reads the field sign-extended
        switch ((int16_t)pItemsTxt->wStat[0])
        {
        case STAT_HITPOINTS:
        case STAT_HPREGEN:
            nValue = D2COMMON_ITEMS_GetBonusLifeBasedOnClass(pPlayer, nValue);
            break;
        case STAT_MANA:
        case STAT_MANARECOVERY:
            nValue = D2COMMON_ITEMS_GetBonusManaBasedOnClass(pPlayer, nValue);
            break;
        default:
            break;
        }

        Unicode::strcpy(wszOut, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pItemsTxt->wSpellDescStr));
        STORM_SStrPrintf(szValue, 10, "%ld", nValue);
        Unicode::win2Unicode(wszValue, szValue, 9);
        Unicode::strcat(wszOut, pwszSpace);
        Unicode::strcat(wszOut, wszValue);
        Unicode::strcat(wszOut, pwszNewline);
        return;
    }

    case 3:
    {
        Unicode wszValue[10];

        nValue = D2COMMON_ITEMMODS_EvaluateItemFormula(pPlayer, pItem, pItemsTxt->dwCalc[0]);
        Unicode::strcpy(wszOut, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pItemsTxt->wSpellDescStr));
        STORM_SStrPrintf(szValue, 10, "%ld", nValue);
        Unicode::win2Unicode(wszValue, szValue, 9);
        Unicode::strcat(wszOut, pwszSpace);
        Unicode::strcat(wszOut, wszValue);
        Unicode::strcat(wszOut, pwszNewline);
        return;
    }

    case 4:
    {
        //  the formula first, then the 512-element buffer's construction: the other order from cases 2 and 3, and the bytes' order
        nValue = D2COMMON_ITEMMODS_EvaluateItemFormula(pPlayer, pItem, pItemsTxt->dwCalc[0]);

        Unicode wszText[512];

        Unicode::sprintf(512, wszText, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pItemsTxt->wSpellDescStr), nValue);
        Unicode::strcat(wszOut, wszText);   // appends (strcat), unlike cases 1-3
        Unicode::strcat(wszOut, pwszNewline);
        return;
    }

    default:
        return;
    }
}


//  The item's name: INV_BuildItemNameText (0x6FADD360, 5364 bytes) and the four helpers the original inlined into it.
//  The function's own asserts run 1840..2252; three calls inside it push earlier lines (1690, 1768 and 1801), the fingerprint of a static function
//  defined above it in inv.cpp and inlined here. The 1801 one is inlined at three sites (set, rare, unique), each copy with the same three buffers
//  in different stack slots. The bracket trim has no assert and so no line, but it is the same eleven instructions at seven sites. They are static
//  helpers here; the compiler may inline them or not.
//  The quest colour: if (nQuestDiffCheck && stat356 < difficulty) red; else if (dwCode != 'leg ') gold. jge loc_6FADE82F sends "not below the
//  difficulty" straight to the Wirt's-Leg test and the gold.
//  At the tail the original also calls an identity function, mov ecx, edi / call 0x6FB5EF30 at 0x6FADE7DC with edi holding wszOut (the callee is
//  mov eax, ecx / retn and does nothing). The unique name is appended, not copied: strcat(wszOut, ...) after the base name and newline.
//  The construction points are the original's: thirty-four construction loops in the bytes. Three run at entry, before the space and newline are
//  fetched; every other one runs inside the arm or helper that uses it, and several construct buffers nothing reads. Each declaration below sits
//  where its loop does. Two shapes are not a single loop and are written as the store and the clear the bytes show: the personalised name's
//  scratch (element 0 stored, 1..99 cleared, 1..99 constructed, so [100]) and the rare name's scratch (element 0 stored, 1..199 cleared, 1..199
//  constructed, so [200]).
//  DIFFERENCE: for those two scratch buffers (the &buf[1] shape) the declaration constructs one element more than the original does.
//  The runeword name's tbl index goes over as mov ecx, eax after ITEMS_GetPrefixId, which writes only ax; the callee's parameter is sixteen bits
//  (GetStringFromTblIndex reads si and masks it, and esi, 0FFFFh), so this is the ABI and not a partial store.

static const char szLenMaxLenAssert[] = "Len < MaxLen";
static const char szPlayerClassAssert[] = "nClass >= PLAYERCLASS_AMAZON && nClass < NUM_PLAYERCLASS";
static const char szAssertZero[] = "0";


//  The bracket tag; the original inlines it at seven sites, always the same eleven instructions. A name like "[xx] Axe" drops everything up to and
//  including the ']', but only when the '[' sits exactly three characters before it, a two-letter tag (cmp [.. eax*2 ..], '[' six bytes back at
//  0x6FADDD07 fixes the three). Any other shape and the name is used whole. The scan starts at the terminator and walks back while it is above
//  zero, so index 0 is looked at but can never start a tag.
static const Unicode *__fastcall INV_SkipBracketTag(const Unicode *wszName)
{
    int32_t nIndex;

    nIndex = Unicode::strlen(wszName);
    while (nIndex > 0 && wszName[nIndex].ch != ']')
        --nIndex;

    if (wszName[nIndex].ch == ']' && nIndex >= 3 && wszName[nIndex - 3].ch == '[')
    {
        return &wszName[nIndex + 1];
    }
    return wszName;
}


//  The class name for an ear; inv.cpp line 1690, inlined once in the original. The assert is the whole guard: cmp eax, 7 / jnb covers both halves of
//  its text as one unsigned compare. The switch keeps its own range test and its own default (xor eax, eax at 0x6FADD7AD, a NULL), which the
//  assert has already made unreachable.
static const Unicode *__fastcall INV_GetPlayerClassName(int32_t nClass)
{
    if (!(nClass >= PCLASS_AMAZON && nClass < NUMBER_OF_PLAYERCLASSES))
    {
        FOG_DisplayAssert(szPlayerClassAssert, szInvCpp, 1690);
        exit(-1);
    }

    switch (nClass)
    {
    case PCLASS_AMAZON:
        return D2LANG_GetStringFromTblIndex(STR_IDX_4011_Amazon);
    case PCLASS_SORCERESS:
        return D2LANG_GetStringFromTblIndex(STR_IDX_4010_Sorceress);
    case PCLASS_NECROMANCER:
        return D2LANG_GetStringFromTblIndex(STR_IDX_4009_Necromancer);
    case PCLASS_PALADIN:
        return D2LANG_GetStringFromTblIndex(STR_IDX_4008_Paladin);
    case PCLASS_BARBARIAN:
        return D2LANG_GetStringFromTblIndex(STR_IDX_4007_Barbarian);
    case PCLASS_DRUID:
        return D2LANG_GetStringFromTblIndex(STR_IDX_10097_Druid);
    case PCLASS_ASSASSIN:
        return D2LANG_GetStringFromTblIndex(STR_IDX_10098_Assassin);
    default:
        return NULL;
    }
}


//  The owner's name placed into a name: inv.cpp line 1801, inlined three times in the original (the set name, the rare name and the unique name).
//  Three buffers are constructed after the flag test and only one of them is used: the owner's name first, then a 512 and a 32 that nothing
//  reads; each inlining put them in different stack slots.
//  Unicode::Personalize places the name the way the language wants it (STRTABLE_GetLanguage decides), which is why it is not a concatenation.
//  The capacity is 512 here and 100 in the ear's own call.
static BOOL __fastcall INV_PersonalizeName(D2UnitStrc *pItem, Unicode *wszOut, const Unicode *wszName)
{
    if (!D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_PERSONALIZED, 1801, szInvCpp))
        return FALSE;

    Unicode wszOwner[512];
    Unicode wszUnused512[512];      // constructed, never read
    Unicode wszUnused32[32];        // constructed, never read

    Unicode::win2Unicode(wszOwner, D2COMMON_ITEMS_GetEarName(pItem), 32);
    Unicode::Personalize(wszOut, wszOwner, wszName, 512, (ELANGUAGE)D2LANG_STRTABLE_GetLanguage());
    return TRUE;
}


//  The finished name, personalised: inv.cpp line 1768, inlined once in the original, at the head of the shared tail. Only for qualities outside
//  5..9: set, unique and the rare family place the owner's name into their own name inside their arm, and everything else (low, normal, superior,
//  magic, and the two flag paths) gets it here, on the whole assembled line. Three buffers again, two of them used.
static void __fastcall INV_PersonalizeFinishedName(D2UnitStrc *pItem, Unicode *wszOut)
{
    D2C_ItemQualities nQuality;

    if (!D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_PERSONALIZED, 1768, szInvCpp))
        return;

    nQuality = D2COMMON_ITEMS_GetItemQuality(pItem);
    if (nQuality >= ITEMQUAL_SET && nQuality <= ITEMQUAL_TEMPERED)
        return;

    Unicode wszOwner[512];
    Unicode wszPersonal[512];
    Unicode wszUnused32[32];        // constructed, never read

    Unicode::win2Unicode(wszOwner, D2COMMON_ITEMS_GetEarName(pItem), 32);
    Unicode::Personalize(wszPersonal, wszOwner, wszOut, 512, (ELANGUAGE)D2LANG_STRTABLE_GetLanguage());
    Unicode::strcpy(wszOut, wszPersonal);
}


//1.10f: D2Client.0x6FADD360
//  The item's name, 5364 bytes, called from eight sites: five in INV_DrawItemDescription (capacities 128 and 256), PANEL_DrawBeltItemHint (128),
//  NPCMENU_BuildConfirmDialog (191) and UIPANELS_BuildItemLabelText (256, for a buffer of 128).
//  Two flag paths first, then a switch on the quality, then a shared tail:
//      runeword (IFLAG_RUNEWORD)   trimmed base name + newline, then the runeword name coloured 4, appended
//      unidentified                the trimmed base name, copied
//      1 low      tbl 1712 "%0 %1": LowQualityItemsTxt.wTblId, base name
//      2 normal   books/scrolls, ears, body parts, gemmed, plain
//      3 superior tbl 1711 "%0 %1": tbl 1727, base name
//      4 magic    tbl 1714 "%0 %1 %2": prefix, base name, suffix
//      5 set      base name + newline + the set name, personalised or 10089
//      6, 8, 9    base name + newline + tbl 1718 "%0 %1" of the rare prefix and suffix, personalised or not, appended
//      7 unique   base name + newline (unless nSkipName) + the unique name
//      default    assert "0" at 2252, and exit
//  Then the tail, for every path but the default: the 1768 personalisation, the identity call, and the quest colour.
//  The tooltip draws bottom to top, so every "base name, newline, X" puts X on the line above the base name, which is why the rare, set, unique
//  and runeword names are always built second.
//  Three paths append and the rest copy: the runeword and rare paths strcat into wszOut; everything else strcpys its first piece. The caller
//  constructs each of its five buffers before the call, so the append lands on an empty string, but it is the caller that makes that true.
//  BUG: the length check kills the process: if ((int)(nTotalLen + Unicode::strlen(base) + 3) >= nMaxLen) FOG_DisplayAssert("Len < MaxLen", inv.cpp,
//  2243 or 2080); exit(-1); in the magic and rare arms. The original ends the process on a name too long for the caller's buffer. nTotalLen is
//  unsigned and the sum is cast to int for the compare; the prefix is measured in bytes before it is widened while the rare suffix's fallback is
//  measured in characters after it is. UNREACHABLE: over the stock tables it never fires (95 at most for magic and 77 for rare, against the
//  smallest capacity passed, 128).
//  BUG: in the body-part arm, after the base name has been fetched, the file index is out of range (6FADD8BB xor esi, esi) and the name is read
//  anyway (6FADD8CB mov cx, [esi+6]). Reproduced. UNREACHABLE: all twelve body-part items in Misc.txt are "Not used" with spawnable 0, so the arm
//  itself never runs in stock play.
void __fastcall INV_BuildItemNameText(D2UnitStrc *pItem, Unicode *wszOut, int32_t nMaxLen)
{
    const Unicode *pwszSpace;
    const Unicode *pwszNewline;
    const Unicode *pwszBase;
    const Unicode *pwszQualifier;
    const Unicode *pwszFormat;
    const Unicode *pwszPiece;
    D2ItemsTxt *pItemsTxt;
    D2C_ItemTypes nItemType;
    uint32_t nTotalLen;
    int32_t nSuffixId;
    int32_t nFileIndex;
    D2C_StringIndices nBookStrId;
    D2LowQualityItemsTxt *pLowQuality;
    D2MonStatsTxt *pMonStats;
    D2InventoryStrc *pInventory;
    D2MagicAffixTxt *pMagicPrefix;
    D2MagicAffixTxt *pMagicSuffix;
    D2RareAffixTxt *pRarePrefix;
    D2RareAffixTxt *pRareSuffix;
    D2SetItemsTxt *pSetItems;
    D2UniqueItemsTxt *pUnique;

    //  The affix fallback builds "(" + szName + ")" here a byte at a time, in the stack slot at [ebp-1C00h], which the ear arm uses for a Unicode
    //  buffer of its own. The slot measures 1024 bytes; the longest string it ever holds as bytes is the two parentheses around a 32-byte name.
    char szAffixName[1024];

    if (!pItem)
        return;

    //  The three buffers the original constructs at entry, before the space and the newline are fetched. Each carries more than one role: the
    //  runeword's base line or an affix prefix; the runeword's name or an affix suffix; the coloured runeword name or the rare line.
    Unicode wszPrefixPart[512];
    Unicode wszSuffixPart[512];
    Unicode wszLine[512];

    pwszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
    pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
    nTotalLen = 0;

    pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(pItem->dwClassId);
    if (!pItemsTxt)
    {
        FOG_DisplayAssert(szPtItemStatsAssert, szInvCpp, 1840);
        exit(-1);
    }

    //  read here, before the flag tests, and used only by the books arm
    nItemType = D2COMMON_ITEMS_GetItemType(pItem);

    if (D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_RUNEWORD, 1843, szInvCpp))
    {
        Unicode wszBase[512];

        Unicode::strcpy(wszBase, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pItemsTxt->wNameStr));
        Unicode::strcpy(wszPrefixPart, INV_SkipBracketTag(wszBase));
        Unicode::strcat(wszPrefixPart, pwszNewline);

        //  the runeword's own name is its prefix id's tbl string, and a NULL falls back to the space string rather than an empty one
        pwszPiece = D2LANG_GetStringFromTblIndex((D2C_StringIndices)D2COMMON_ITEMS_GetPrefixId(pItem, 0));
        if (!pwszPiece)
            pwszPiece = pwszSpace;
        Unicode::strcpy(wszSuffixPart, pwszPiece);
        Unicode::strcat(wszLine, wszSuffixPart);
        UI_AddColorCodeToWstring((wchar_t *)wszLine, 4);

        Unicode::strcat(wszOut, wszPrefixPart);
        Unicode::strcat(wszOut, wszLine);
    }
    else if (!D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_IDENTIFIED, 1862, szInvCpp))
    {
        Unicode wszBase[512];

        Unicode::strcpy(wszBase, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pItemsTxt->wNameStr));
        Unicode::strcpy(wszOut, INV_SkipBracketTag(wszBase));
    }
    else
    {
        switch (D2COMMON_ITEMS_GetItemQuality(pItem))
        {
        case ITEMQUAL_INFERIOR:
            pLowQuality = D2COMMON_DATATBLS_GetLowQualityItemsTxtRecord(D2COMMON_ITEMS_GetFileIndex(pItem));
            if (pLowQuality)
            {
                pwszBase = D2LANG_GetStringFromTblIndex((D2C_StringIndices)pItemsTxt->wNameStr);
                pwszQualifier = D2LANG_GetStringFromTblIndex((D2C_StringIndices)pLowQuality->wTblId);
                pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_1712_LowqualityFormat);
                LANG_FormatString(wszOut, pwszFormat, pwszQualifier, pwszBase, (const Unicode *)NULL);
            }
            break;

        case ITEMQUAL_NORMAL:
        {
            //  books and scrolls: the suffix id picks the pair (0 the t- pair, 1 the i- pair) and the item's own type picks within it;
            //  any other suffix id leaves wszOut as the caller had it
            if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_SCROLL) || D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_BOOK))
            {
                nSuffixId = D2COMMON_ITEMS_GetSuffixId(pItem, 0);
                if (nSuffixId)
                {
                    if (nSuffixId != 1)
                        break;
                    nBookStrId = STR_IDX_2201_ibk;
                    if (nItemType != ITEMTYPE_BOOK)
                        nBookStrId = STR_IDX_2202_isc;
                }
                else
                {
                    nBookStrId = STR_IDX_2199_tbk;
                    if (nItemType != ITEMTYPE_BOOK)
                        nBookStrId = STR_IDX_2200_tsc;
                }
                Unicode::strcpy(wszOut, D2LANG_GetStringFromTblIndex(nBookStrId));
                break;
            }

            //  ears: "Hardcore" when flagged, "Level N", the class, then the owner placed into the item's own name, read bottom up
            if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_PLAYER_BODY_PART))
            {
                //  eighteen bytes cleared by six stores at 0x6FADD5FB, three of them ever used (push 3 at 0x6FADD65A), so a three-digit level
                //  would print truncated
                char szEarLevel[18] = { 0 };
                Unicode wszEarName[256];
                Unicode wszEarLevel[3];

                STORM_SStrPrintf(szEarLevel, 3, "%d", D2COMMON_ITEMS_GetEarLevel(pItem));
                Unicode::win2Unicode(wszEarLevel, szEarLevel, 3);

                //  constructed, and nothing in this arm reads it
                Unicode wszUnused256[256];

                //  an inlined, unbounded strcpy in the original too; the slot is 20 bytes and the field ITEMS_GetEarName points into is 16
                char szEarName[20];

                strcpy(szEarName, D2COMMON_ITEMS_GetEarName(pItem));
                Unicode::win2Unicode(wszEarName, szEarName, 256);

                if (D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_NAMED, 1907, szInvCpp))
                {
                    Unicode::strcpy(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_5126_Hardcore));
                    Unicode::strcat(wszOut, pwszNewline);
                    Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_4141_level));
                }
                else
                {
                    Unicode::strcpy(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_4141_level));
                }
                Unicode::strcat(wszOut, pwszSpace);
                Unicode::strcat(wszOut, wszEarLevel);
                Unicode::strcat(wszOut, pwszNewline);
                Unicode::strcat(wszOut, INV_GetPlayerClassName(D2COMMON_ITEMS_GetFileIndex(pItem)));
                Unicode::strcat(wszOut, pwszNewline);

                //  a hundred: element 0 stored, 1..99 cleared and constructed
                Unicode wszPersonal[100];
                wszPersonal[0].ch = 0;
                memset(&wszPersonal[1], 0, 99 * sizeof(Unicode));
                Unicode wszNameSrc[512];

                Unicode::strcpy(wszNameSrc, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pItemsTxt->wNameStr));
                Unicode::Personalize(wszPersonal, wszEarName, wszNameSrc, 100, (ELANGUAGE)D2LANG_STRTABLE_GetLanguage());
                Unicode::strcat(wszOut, wszPersonal);
                break;
            }

            //  body parts: "%0 %1" with the monster's own name
            if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_BODY_PART))
            {
                nFileIndex = D2COMMON_ITEMS_GetFileIndex(pItem);
                if (nFileIndex < 0 || nFileIndex >= sgptDataTables->nMonStatsTxtRecordCount)
                {
                    pMonStats = NULL;
                }
                else
                {
                    pMonStats = &sgptDataTables->pMonStatsTxt[nFileIndex];
                }

                pwszBase = D2LANG_GetStringFromTblIndex((D2C_StringIndices)pItemsTxt->wNameStr);
                //  the fault, when the arm above produced the NULL; written as the original wrote it
                pwszQualifier = D2LANG_GetStringFromTblIndex((D2C_StringIndices)pMonStats->wNameStr);
                pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_1716_BodyPartsFormat);
                LANG_FormatString(wszOut, pwszFormat, pwszQualifier, pwszBase, (const Unicode *)NULL);
                break;
            }

            //  socketed, with something in the sockets: "Gemmed <name>"; the inventory pointer is read before the flag is asked
            pInventory = pItem->pInventory;
            if (D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_SOCKETED, 1943, szInvCpp) && pInventory && D2COMMON_INVENTORY_GetItemCount(pInventory))
            {
                pwszBase = D2LANG_GetStringFromTblIndex((D2C_StringIndices)pItemsTxt->wNameStr);
                pwszQualifier = D2LANG_GetStringFromTblIndex(STR_IDX_1728_Gemmed);
                pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_1715_GemmedNormalName);
                LANG_FormatString(wszOut, pwszFormat, pwszQualifier, pwszBase, (const Unicode *)NULL);
                break;
            }

            //  everything else: the plain name, trimmed
            Unicode wszBase[512];

            Unicode::strcpy(wszBase, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pItemsTxt->wNameStr));
            Unicode::strcpy(wszOut, INV_SkipBracketTag(wszBase));
            break;
        }

        case ITEMQUAL_SUPERIOR:
            pwszBase = D2LANG_GetStringFromTblIndex((D2C_StringIndices)pItemsTxt->wNameStr);
            pwszQualifier = D2LANG_GetStringFromTblIndex(STR_IDX_1727_Hiquality);
            pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_1711_HiqualityFormat);
            LANG_FormatString(wszOut, pwszFormat, pwszQualifier, pwszBase, (const Unicode *)NULL);
            break;

        case ITEMQUAL_MAGIC:
        {
            //  both records are fetched before either is looked at
            pMagicPrefix = D2COMMON_DATATBLS_GetMagicAffixTxtRecord(D2COMMON_ITEMS_GetPrefixId(pItem, 0));
            pMagicSuffix = D2COMMON_DATATBLS_GetMagicAffixTxtRecord(D2COMMON_ITEMS_GetSuffixId(pItem, 0));

            if (pMagicPrefix)
            {
                if (pMagicPrefix->wTblIndex)
                {
                    //  the tbl string fetched twice, once to measure and once to copy
                    nTotalLen = Unicode::strlen(D2LANG_GetStringFromTblIndex((D2C_StringIndices)pMagicPrefix->wTblIndex));
                    Unicode::strcpy(wszPrefixPart, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pMagicPrefix->wTblIndex));
                }
                else
                {
                    //  "(name)", a modding aid: a stock affix always has a string
                    strcpy(szAffixName, gszAffixOpenParen);
                    strcat(szAffixName, pMagicPrefix->szName);
                    strcat(szAffixName, gszAffixCloseParen);
                    nTotalLen = strlen(szAffixName);
                    Unicode::toUnicode(wszPrefixPart, szAffixName, 512);
                }
            }
            else
            {
                wszPrefixPart[0].ch = 0;
            }

            if (pMagicSuffix)
            {
                if (pMagicSuffix->wTblIndex)
                {
                    nTotalLen += Unicode::strlen(D2LANG_GetStringFromTblIndex((D2C_StringIndices)pMagicSuffix->wTblIndex));
                    //  strcat, onto the empty buffer constructed at entry
                    Unicode::strcat(wszSuffixPart, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pMagicSuffix->wTblIndex));
                }
                else
                {
                    Unicode wszConverted[512];

                    strcpy(szAffixName, gszAffixOpenParen);
                    strcat(szAffixName, pMagicSuffix->szName);
                    strcat(szAffixName, gszAffixCloseParen);
                    nTotalLen += strlen(szAffixName);
                    Unicode::toUnicode(wszConverted, szAffixName, 512);
                    Unicode::strcat(wszSuffixPart, wszConverted);
                }
            }
            else
            {
                wszSuffixPart[0].ch = 0;
            }

            if ((int32_t)(nTotalLen + Unicode::strlen(D2LANG_GetStringFromTblIndex((D2C_StringIndices)pItemsTxt->wNameStr)) + 3) >= nMaxLen)
            {
                FOG_DisplayAssert(szLenMaxLenAssert, szInvCpp, 2243);
                exit(-1);
            }

            pwszBase = D2LANG_GetStringFromTblIndex((D2C_StringIndices)pItemsTxt->wNameStr);
            pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_1714_MagicFormat);
            LANG_FormatString(wszOut, pwszFormat, wszPrefixPart, pwszBase, wszSuffixPart, (const Unicode *)NULL);
            break;
        }

        case ITEMQUAL_SET:
        {
            //  the inlined lookup in the bytes is DATATBLS_GetSetItemsTxtRecord, bounds test and all; a record that is not there sends the name,
            //  as it stands, to the tail
            pSetItems = DATATBLS_GetSetItemsTxtRecord(D2COMMON_ITEMS_GetFileIndex(pItem));
            if (!pSetItems)
                break;

            Unicode wszBase[512];

            Unicode::strcpy(wszBase, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pItemsTxt->wNameStr));
            Unicode::strcpy(wszOut, INV_SkipBracketTag(wszBase));
            Unicode::strcat(wszOut, pwszNewline);

            if (pSetItems->wStringId)
            {
                Unicode wszPersonal[512];
                Unicode wszSetName[512];

                Unicode::strcpy(wszSetName, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pSetItems->wStringId));
                if (INV_PersonalizeName(pItem, wszPersonal, wszSetName))
                {
                    Unicode::strcat(wszOut, wszPersonal);
                }
                else
                {
                    //  tbl 10089 SetItemFormatX is "%0", a pass-through a locale can wrap; the scratch is the &buf[1] hundred
                    Unicode wszFormatted[100];
                    wszFormatted[0].ch = 0;
                    memset(&wszFormatted[1], 0, 99 * sizeof(Unicode));

                    pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_10089_SetItemFormatX);
                    LANG_FormatString(wszFormatted, pwszFormat, wszSetName, (const Unicode *)NULL);
                    Unicode::strcat(wszOut, wszFormatted);
                }
            }
            break;
        }

        case ITEMQUAL_RARE:
        case ITEMQUAL_CRAFT:
        case ITEMQUAL_TEMPERED:
        {
            pRarePrefix = D2COMMON_DATATBLS_GetRareAffixTxtRecord(D2COMMON_ITEMS_GetRarePrefixId(pItem));
            pRareSuffix = D2COMMON_DATATBLS_GetRareAffixTxtRecord(D2COMMON_ITEMS_GetRareSuffixId(pItem));

            if (pRarePrefix)
            {
                if (pRarePrefix->wStringId)
                {
                    nTotalLen = Unicode::strlen(D2LANG_GetStringFromTblIndex((D2C_StringIndices)pRarePrefix->wStringId));
                    Unicode::strcpy(wszPrefixPart, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pRarePrefix->wStringId));
                }
                else
                {
                    strcpy(szAffixName, gszAffixOpenParen);
                    strcat(szAffixName, pRarePrefix->szName);
                    strcat(szAffixName, gszAffixCloseParen);
                    nTotalLen = strlen(szAffixName);
                    Unicode::toUnicode(wszPrefixPart, szAffixName, 512);
                }
            }
            else
            {
                wszPrefixPart[0].ch = 0;
            }

            //  the suffix goes in by strcat when there is a prefix record and by strcpy when there is not. The bytes share one tail between the
            //  tbl path and the fallback; the fallback's buffer is constructed inside its branch, so the tail is written into both branches here
            //  rather than reached with a pointer to a buffer that has gone out of scope
            if (pRareSuffix)
            {
                if (pRareSuffix->wStringId)
                {
                    nTotalLen += Unicode::strlen(D2LANG_GetStringFromTblIndex((D2C_StringIndices)pRareSuffix->wStringId));
                    pwszPiece = D2LANG_GetStringFromTblIndex((D2C_StringIndices)pRareSuffix->wStringId);
                    if (pRarePrefix)
                        Unicode::strcat(wszSuffixPart, pwszPiece);
                    else
                        Unicode::strcpy(wszSuffixPart, pwszPiece);
                }
                else
                {
                    Unicode wszConverted[512];

                    strcpy(szAffixName, gszAffixOpenParen);
                    strcat(szAffixName, pRareSuffix->szName);
                    strcat(szAffixName, gszAffixCloseParen);
                    Unicode::toUnicode(wszConverted, szAffixName, 512);
                    //  measured after widening here, before it for the prefix
                    nTotalLen += Unicode::strlen(wszConverted);
                    if (pRarePrefix)
                        Unicode::strcat(wszSuffixPart, wszConverted);
                    else
                        Unicode::strcpy(wszSuffixPart, wszConverted);
                }
            }
            else
            {
                wszSuffixPart[0].ch = 0;
            }

            if ((int32_t)(nTotalLen + Unicode::strlen(D2LANG_GetStringFromTblIndex((D2C_StringIndices)pItemsTxt->wNameStr)) + 3) >= nMaxLen)
            {
                FOG_DisplayAssert(szLenMaxLenAssert, szInvCpp, 2080);
                exit(-1);
            }

            Unicode wszBase[512];

            Unicode::strcpy(wszBase, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pItemsTxt->wNameStr));
            Unicode::strcpy(wszLine, INV_SkipBracketTag(wszBase));
            Unicode::strcat(wszLine, pwszNewline);

            //  two hundred: element 0 stored, 1..199 cleared and constructed (mov word ptr [..], 0 then rep stosd x 99 + stosw)
            Unicode wszRareName[200];
            wszRareName[0].ch = 0;
            memset(&wszRareName[1], 0, 199 * sizeof(Unicode));

            pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_1718_RareFormat);
            LANG_FormatString(wszRareName, pwszFormat, wszPrefixPart, wszSuffixPart, (const Unicode *)NULL);

            //  constructed before the flag test: it is the helper's output
            Unicode wszPersonal[512];

            if (INV_PersonalizeName(pItem, wszPersonal, wszRareName))
                pwszPiece = wszPersonal;
            else
                pwszPiece = wszRareName;
            Unicode::strcat(wszLine, pwszPiece);
            Unicode::strcat(wszOut, wszLine);
            break;
        }

        case ITEMQUAL_UNIQUE:
        {
            Unicode wszBase[512];

            Unicode::strcpy(wszBase, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pItemsTxt->wNameStr));

            if (D2COMMON_ITEMS_GetFileIndex(pItem) < 0)
            {
                Unicode::strcpy(wszOut, INV_SkipBracketTag(wszBase));
                break;
            }

            //  asked a second time, and the record looked up inline
            nFileIndex = D2COMMON_ITEMS_GetFileIndex(pItem);
            pUnique = NULL;
            if (nFileIndex >= 0 && nFileIndex < sgptDataTables->nUniqueItemsTxtRecordCount)
            {
                pUnique = &sgptDataTables->pUniqueItemsTxt[nFileIndex];
            }
            if (!pUnique)
            {
                Unicode::strcpy(wszOut, INV_SkipBracketTag(wszBase));
                break;
            }

            //  nSkipName: the unique name alone, no base name under it
            if (!pItemsTxt->nSkipName)
            {
                Unicode::strcpy(wszOut, INV_SkipBracketTag(wszBase));
                Unicode::strcat(wszOut, pwszNewline);
            }

            Unicode wszPersonal[512];

            pwszPiece = INV_SkipBracketTag(D2LANG_GetStringFromTblIndex((D2C_StringIndices)pUnique->wTblIndex));
            if (INV_PersonalizeName(pItem, wszPersonal, pwszPiece))
            {
                Unicode::strcat(wszOut, wszPersonal);
                break;
            }

            //  the same string fetched and trimmed a second time rather than the one just computed (the original's), and appended, not copied
            Unicode::strcat(wszOut, INV_SkipBracketTag(D2LANG_GetStringFromTblIndex((D2C_StringIndices)pUnique->wTblIndex)));
            break;
        }

        default:
            FOG_DisplayAssert(szAssertZero, szInvCpp, 2252);
            exit(-1);
        }
    }

    //  the tail
    INV_PersonalizeFinishedName(pItem, wszOut);

    //  mov eax, ecx / retn: the identity call at 0x6FADE7DC; its answer is discarded
    GFXUTIL_ReturnArg(wszOut);

    if (pItemsTxt->nQuest)
    {
        //  red for a quest item carried down from a harder difficulty; gold for every other quest item, including a difficulty-checked one
        //  that is not below, except Wirt's Leg
        if (pItemsTxt->nQuestDiffCheck && D2COMMON_STATLIST_UnitGetStatValue(pItem, STAT_QUESTITEMDIFFICULTY, 0) < GAME_GetDifficulty())
        {
            UI_AddColorCodeToWstring((wchar_t *)wszOut, 1);
        }
        else if (pItemsTxt->dwCode != 0x2067656C)       // 'leg '
        {
            UI_AddColorCodeToWstring((wchar_t *)wszOut, 4);
        }
    }
}

//  The item description: INV_DrawItemDescription (0x6FAE1890, 9674 bytes) and the four helpers the original inlined into it.
//  The function's own asserts run 3994..4239. Four groups push earlier lines, each with its own asserts and buffers, and are static helpers here:
//  3137 the socket filler's lines (INV_AppendSocketFillerText); 3308 and 3312 a tome's description (INV_DrawBookDescription); 3409..3551 a set
//  item's description (INV_DrawSetItemDescription); 3676 and 3679 the transmogrify preview (INV_DrawTransmogrifyDescription). The set arm even
//  re-tests the caller's own entry conditions (show-desc flags, a hover item, nothing on the cursor), which is what a function does and a
//  block does not.
//  Four more lines are inlined copies of functions this unit already has, body for body: the required level, strength and dexterity lines
//  (INV_FormatReq*Line, whose standalone copies the set arm calls) and the quantity line (INV_AppendStackQuantityText, which the tome arm calls).
//  The ordinary path calls them.
//  The second parameter, bVendorUnit, is 1 exactly when pUnit is the vendor NPC (0x6FAE09D3: a vendor mode, the mouse on the vendor's half, the NPC
//  found by GUID); INV_MercInvDrawPanel passes 0 with the mercenary. With 1, the requirement colours are decided by the player's stats.
//  Fifty-eight constructions, each where the bytes put it: 33 construction loops and 25 calls to the vector constructor iterator. Twenty-two run
//  at entry (twenty-one buffers, the newline fetch, then one more) and the rest inside the arm or helper that uses them. Three are constructed
//  and never read: 256 and 10 elements at entry and 512 in the tome arm. The transmogrify preview's output is the &buf[1] shape (element 0
//  stored, 1..1023 cleared and constructed).
//  DIFFERENCE: for that buffer the declaration constructs one element more than the original does.
//  Two dead blocks are reproduced: 0x6FAE2E41..0x6FAE2F0E, a quiver's "Quantity: N" plus its base name written into wszCombined, which the
//  combine's strcpy at 0x6FAE38C5 overwrites before anything reads it; and 0x6FAE3CE4, strcpy(tbl 5387) into the transmogrify text, overwritten
//  by the next strcpy. Both run and cost calls into D2Lang and D2Common; neither can be seen.
//  The identity call (0x6FB5EF30, mov eax, ecx / retn) is made six times in this function. The inlined DATATBLS_ApplyRatio's nValue <= 0x100000
//  test is kept in the bytes (a 16-bit wReq* is always under it), so the C calls the function.
//  The vendor-mode test in the ordinary and set paths is mode == 0 || mode < 1 || mode > 9, with the redundant half, as found. The finished text
//  is clamped after it is built: a length of 1023 or more gets element 1023 zeroed (jb at 0x6FAE3BB9, so unsigned).
//  D2ExpRes detours eight instructions inside this function to reposition the hints for its resolutions; a rewrite of the function does not
//  carry them.

static const char szSocketFillerAssert[] = "ITEMSIsA(sghSelItem, ITEMTYPE_SOCKETFILLER_TYPE)";
static const char szSelItemBookAssert[] = "sghSelItem && ITEMSGetType(sghSelItem) == ITEMTYPE_BOOK";
static const char szPtOldItemStatsAssert[] = "ptOldItemStats";
static const char szPtNewItemStatsAssert[] = "ptNewItemStats";


//  What a gem or rune grants, and where it goes: inv.cpp line 3137, inlined once in the original, under the caller's own type test.
//  BUG: the capacity passed is 512 and the caller's buffer is 256 (push 200h at 0x6FAE3070). The builder's callee ignores its capacity anyway
//  (MODS_AppendMagicModsText renders into a fixed 1024), so only the data bounds the text: the longest stock one is the Ist rune's, 241
//  characters with the colour code the caller adds. LATENT: in the original the 256 elements above this buffer are set-arm slots dead on this path,
//  so an overrun lands nowhere live; in this rewrite the neighbours are the compiler's choice.
static void __fastcall INV_AppendSocketFillerText(Unicode *wszOut)
{
    const Unicode *pwszNewline;

    pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
    if (!D2COMMON_ITEMS_CheckItemTypeId(gpHoverItem, ITEMTYPE_SOCKET_FILLER))
    {
        FOG_DisplayAssert(szSocketFillerAssert, szInvCpp, 3137);
        exit(-1);
    }

    MODS_BuildSocketFillerText(gpHoverItem, wszOut, 512);
    Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_11080_ExInsertSocketsX));
    Unicode::strcat(wszOut, pwszNewline);
}


//  A tome's description: inv.cpp lines 3308 and 3312, inlined once in the original. The quantity, two usage hints when no panel mode is set, and
//  the name, drawn left-aligned, which no other arm does.
static void __fastcall INV_DrawBookDescription(void)
{
    const Unicode *pwszNewline;
    D2ItemsTxt *pItemsTxt;
    int32_t nWidth;
    int32_t nHeight;

    Unicode wszText[1024];
    Unicode wszUnused512[512];      // constructed, never read

    pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
    if (!gpHoverItem || D2COMMON_ITEMS_GetItemType(gpHoverItem) != ITEMTYPE_BOOK)
    {
        FOG_DisplayAssert(szSelItemBookAssert, szInvCpp, 3308);
        exit(-1);
    }

    pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(gpHoverItem ? gpHoverItem->dwClassId : -1);
    if (!pItemsTxt)
    {
        FOG_DisplayAssert(szPtItemStatsAssert, szInvCpp, 3312);
        exit(-1);
    }

    INV_AppendStackQuantityText(gpHoverItem, wszText, pItemsTxt);
    if (!sgInventoryMode)
    {
        Unicode::strcat(wszText, D2LANG_GetStringFromTblIndex(STR_IDX_2203_RightClicktoUse));
        Unicode::strcat(wszText, pwszNewline);
        Unicode::strcat(wszText, D2LANG_GetStringFromTblIndex(STR_IDX_2206_InsertScrolls));
        Unicode::strcat(wszText, pwszNewline);
    }

    Unicode wszName[128];

    INV_BuildItemNameText(gpHoverItem, wszName, 128);
    GFXUTIL_ReturnArg(wszName);
    Unicode::strcat(wszText, wszName);

    Unicode wszOut[1024];

    if (sgInventoryMode < 1 || sgInventoryMode > 9)
        Unicode::strcpy(wszOut, wszText);
    else
        INV_AppendPriceFooterText(wszText, wszOut);
    GFXUTIL_ReturnArg(wszOut);

    D2WIN_10131_GetTextDimensions(wszOut, &nWidth, &nHeight);
    if (gnDescPopupY - nHeight > 0)
    {
        D2WIN_10129_DrawFramedText(wszOut, gnDescPopupX, gnDescPopupY, 0, 0);
        return;
    }
    D2WIN_10129_DrawFramedText(wszOut, gnDescPopupAltX, (gnDescPopupBottomY + nHeight), 0, 0);
}


//  A set item's description: inv.cpp lines 3409..3551, inlined once in the original. Its own sections in its own buffers: the set's pieces green
//  when worn and red when not, the set name, the set bonuses, and the item's own lines under them.
//  Its own rule for whose stats count: the player's when pUnit is a monster that is not a hireling (a vendor), where the caller uses its
//  bVendorUnit; the two agree for both callers.
//  Nineteen buffers at entry, in the bytes' order; the sockets' 300, the vendor note's 64 and "priceless"'s 64 where they are used.
static void __fastcall INV_DrawSetItemDescription(D2UnitStrc *pUnit)
{
    const Unicode *pwszNewline;
    const Unicode *pwszPieceName;
    const Unicode *pwszFormat;
    D2UnitStrc *pPlayer;
    D2UnitStrc *pEquipped;
    D2InventoryStrc *pInventory;
    D2ItemsTxt *pItemsTxt;
    D2SetItemsTxt *pSetItems;
    D2SetItemsTxt *pPiece;
    D2SetsTxt *pSetsTxt;
    uint32_t nColorLevel;
    uint32_t nColorStrength;
    uint32_t nColorDexterity;
    uint32_t nColorClass;
    BOOL bStrengthOk;
    BOOL bDexterityOk;
    BOOL bLevelOk;
    int32_t nLevel;
    int32_t nReqPercent;
    int32_t nStrDelta;
    int32_t nDexDelta;
    int32_t nItemClass;
    int32_t nPiece;
    int32_t nCost;
    int32_t nWidth;
    int32_t nHeight;

    nColorLevel = 0;
    nColorStrength = 0;
    nColorDexterity = 0;

    Unicode wszSetText[1024];
    Unicode wszPopup[2048];
    Unicode wszItemName[256];
    Unicode wszSetName[256];
    Unicode wszDurability[256];
    Unicode wszReqLevel[256];
    Unicode wszReqStrength[256];
    Unicode wszReqDexterity[256];
    Unicode wszElemDamage[256];
    Unicode wszChanceToBlock[256];
    Unicode wszSkillDamage[256];
    Unicode wszDefense[256];
    Unicode wszStats[1024];
    Unicode wszAttackSpeed[256];
    Unicode wszPieceList[512];
    Unicode wszMagicMods[1024];
    Unicode wszSetBonus[512];
    Unicode wszStateMods[512];
    Unicode wszClassOnly[256];

    nColorClass = 0;
    pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);

    pPlayer = pUnit;
    if (pUnit->dwUnitType == UNIT_MONSTER && !D2COMMON_MONSTERS_GetHirelingTypeId(pUnit))
    {
        pPlayer = CUNIT_GetPlayer();
    }
    pInventory = pUnit->pInventory;

    //  the caller's own entry conditions, asked again
    if (!gbShowSetItemDesc && !gbShowItemDesc)
        return;
    if (!gpHoverItem)
        return;
    if (D2COMMON_INVENTORY_GetCursorItem(pInventory))
        return;

    pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(gpHoverItem ? gpHoverItem->dwClassId : -1);
    if (!pItemsTxt)
    {
        FOG_DisplayAssert(szPtItemStatsAssert, szInvCpp, 3409);
        exit(-1);
    }

    pSetItems = DATATBLS_GetSetItemsTxtRecord(D2COMMON_ITEMS_GetFileIndex(gpHoverItem));
    if (!pSetItems)
        return;
    pSetsTxt = INV_DataTblsGetSetsTxtRecord(pSetItems->nSetId);
    if (!pSetsTxt)
        return;

    Unicode::strcpy(wszSetName, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pSetsTxt->wStringId));
    Unicode::strcat(wszSetName, pwszNewline);

    //  out-parameters in the order strength, dexterity, level
    D2COMMON_ITEMS_CheckRequirements(gpHoverItem, pPlayer, FALSE, &bStrengthOk, &bDexterityOk, &bLevelOk);
    INV_AppendDurabilityText(gpHoverItem, wszDurability, pItemsTxt);

    nLevel = D2COMMON_ITEMS_GetLevelRequirement(gpHoverItem, pPlayer);
    if (nLevel > 1)
    {
        if (!bLevelOk)
            nColorLevel = 1;
        INV_FormatReqLevelLine(wszReqLevel, nLevel);
    }

    if (D2COMMON_ITEMS_CheckItemTypeId(gpHoverItem, ITEMTYPE_WEAPON) || D2COMMON_ITEMS_CheckItemTypeId(gpHoverItem, ITEMTYPE_ANY_ARMOR))
    {
        nStrDelta = 0;
        nDexDelta = 0;
        nReqPercent = D2COMMON_STATLIST_UnitGetItemStatOrSkillStatValue(gpHoverItem, STAT_ITEM_REQ_PERCENT, 0);
        if (nReqPercent)
        {
            //  ecx the base, edx the percentage: the same roles as the ordinary path's inlined copy
            nStrDelta = DATATBLS_ApplyRatio(pItemsTxt->wReqStr, nReqPercent, 100);
            nDexDelta = DATATBLS_ApplyRatio(pItemsTxt->wReqDex, nReqPercent, 100);
        }
        if (D2COMMON_ITEMS_CheckItemFlag(gpHoverItem, IFLAG_ETHEREAL, 3459, szInvCpp))
        {
            nStrDelta -= 10;
            nDexDelta -= 10;
        }
        if (pItemsTxt->wReqStr)
        {
            if (!bStrengthOk)
                nColorStrength = 1;
            INV_FormatReqStrengthLine(wszReqStrength, pItemsTxt, nStrDelta);
        }
        if (pItemsTxt->wReqDex)
        {
            if (!bDexterityOk)
                nColorDexterity = 1;
            INV_FormatReqDexterityLine(wszReqDexterity, pItemsTxt, nDexDelta);
        }
    }

    Unicode::strcpy(wszStats, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszReqLevel, nColorLevel));
    Unicode::strcat(wszStats, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszReqStrength, nColorStrength));
    Unicode::strcat(wszStats, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszReqDexterity, nColorDexterity));

    nItemClass = D2COMMON_ITEMS_GetClassOfClassSpecificItem(gpHoverItem);
    if (nItemClass != PCLASS_INVALID)
    {
        Unicode::strcat(wszClassOnly, D2LANG_GetStringFromTblIndex((D2C_StringIndices)(nItemClass + STR_IDX_10917_AmaOnly)));
        Unicode::strcat(wszClassOnly, pwszNewline);
        //  red only for a player of another class; the ordinary path does not ask the unit type
        if (pPlayer && pPlayer->dwUnitType == UNIT_PLAYER && pPlayer->dwClassId != nItemClass)
        {
            nColorClass = 1;
        }
        Unicode::strcat(wszStats, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszClassOnly, nColorClass));
    }

    Unicode::strcat(wszStats, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszDurability, 0));

    //  attack speed before the damage range here; the ordinary path builds them the other way round
    if (D2COMMON_ITEMS_CheckItemTypeId(gpHoverItem, ITEMTYPE_WEAPON))
    {
        INV_AppendAttackSpeedText(gpHoverItem, wszAttackSpeed, pItemsTxt);
        UI_AddColorCodeToWstring((wchar_t *)wszAttackSpeed, 0);
        Unicode::strcat(wszStats, wszAttackSpeed);
        if (D2COMMON_STATLIST_GetMinDamageFromUnit(gpHoverItem, FALSE) >= 0 && D2COMMON_STATLIST_GetMaxDamageFromUnit(gpHoverItem, FALSE) >= 0)
        {
            INV_AppendElementalDamageText(gpHoverItem, wszElemDamage, pItemsTxt);
            UI_AddColorCodeToWstring((wchar_t *)wszElemDamage, 0);
            Unicode::strcat(wszStats, wszElemDamage);
        }
    }

    if (D2COMMON_ITEMS_CheckItemTypeId(gpHoverItem, ITEMTYPE_ANY_SHIELD))
    {
        if (pPlayer && pPlayer->dwUnitType == UNIT_PLAYER && pPlayer->dwClassId == PCLASS_PALADIN && (!D2COMMON_ITEMS_IsClassValid(gpHoverItem) || D2COMMON_ITEMS_GetClassOfClassSpecificItem(gpHoverItem) == PCLASS_PALADIN))
        {
            INV_AppendSkillDamageText(gpHoverItem, wszSkillDamage, pItemsTxt);
            UI_AddColorCodeToWstring((wchar_t *)wszSkillDamage, 0);
            Unicode::strcat(wszStats, wszSkillDamage);
        }
        INV_AppendChanceToBlockText(gpHoverItem, wszChanceToBlock, pItemsTxt);
        UI_AddColorCodeToWstring((wchar_t *)wszChanceToBlock, 0);
        Unicode::strcat(wszStats, wszChanceToBlock);
    }

    if (D2COMMON_ITEMS_CheckItemTypeId(gpHoverItem, ITEMTYPE_ANY_ARMOR) && D2COMMON_STATLIST_GetDefenseFromUnit(gpHoverItem) > 0)
    {
        INV_AppendDefenseText(pPlayer, gpHoverItem, wszDefense, pItemsTxt);
        UI_AddColorCodeToWstring((wchar_t *)wszDefense, 0);
        Unicode::strcat(wszStats, wszDefense);
    }

    INV_BuildItemNameText(gpHoverItem, wszItemName, 256);
    UI_AddColorCodeToWstring((wchar_t *)wszItemName, D2COMMON_ITEMS_CheckItemFlag(gpHoverItem, IFLAG_BROKEN, 3542, szInvCpp) ? 1 : 2);
    Unicode::strcat(wszStats, wszItemName);

    wszMagicMods[0].ch = 0;
    if (D2COMMON_ITEMS_CheckItemFlag(gpHoverItem, IFLAG_SOCKETED, 3551, szInvCpp))
    {
        Unicode wszSockets[300];

        INV_AppendEtherealAndSocketsText(gpHoverItem, wszSockets);
        Unicode::strcpy(wszMagicMods, wszSockets);
    }
    MODS_AppendMagicModsText(gpHoverItem, wszMagicMods, 1024, 1, (const Unicode *)NULL);

    wszSetBonus[0].ch = 0;
    MODS_AppendSetBonusText(pPlayer, gpHoverItem, wszSetBonus, 512);
    wszStateMods[0].ch = 0;
    if (gpHoverItem && gpHoverItem->dwItemMode == IMODE_EQUIP)
    {
        MODS_AppendStateModsText(pPlayer, gpHoverItem, wszStateMods, 512);
    }

    //  the piece list. wszSetText carries over between pieces: a piece with no string skips the format and appends the previous piece's line
    //  again, recoloured (no stock row lacks one). The count is capped at six by D2Common's loader, so pSetItem[] is never overrun.
    for (nPiece = 0; nPiece < pSetsTxt->nSetItems; ++nPiece)
    {
        pPiece = pSetsTxt->pSetItem[nPiece];
        if (!pPiece)
            break;

        pEquipped = INV_FindComparableEquippedItem(pInventory, (int16_t)pPiece->wSetItemId);
        if (pPiece->wStringId)
        {
            //  the piece's name fetched before the format, as pushed
            pwszPieceName = D2LANG_GetStringFromTblIndex((D2C_StringIndices)pPiece->wStringId);
            pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_10089_SetItemFormatX);
            LANG_FormatString(wszSetText, pwszFormat, pwszPieceName, (const Unicode *)NULL);
            Unicode::strcat(wszSetText, pwszNewline);
        }
        UI_AddColorCodeToWstring((wchar_t *)wszSetText, pEquipped ? 2 : 1);
        Unicode::strcat(wszPieceList, wszSetText);
    }

    Unicode::strcpy(wszSetText, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszPieceList, 2));
    Unicode::strcat(wszSetText, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszSetName, 4));
    if (wszStateMods[0].ch)
    {
        Unicode::strcat(wszSetText, pwszNewline);
        Unicode::strcat(wszSetText, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszStateMods, 4));
    }
    Unicode::strcat(wszSetText, pwszNewline);
    if (wszSetBonus[0].ch)
    {
        Unicode::strcat(wszSetText, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszSetBonus, 2));
    }
    Unicode::strcat(wszSetText, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszMagicMods, 3));
    Unicode::strcat(wszSetText, wszStats);

    //  mode == 0 || is redundant beside mode < 1 and it is in the bytes
    if (sgInventoryMode == 0 || sgInventoryMode < 1 || sgInventoryMode > 9)
    {
        Unicode::strcpy(wszPopup, wszSetText);
    }
    else
    {
        Unicode wszActionNote[64];

        if (NPCMENU_TransactionCost(gpHoverItem, gnNpcTradeMode, &nCost, wszActionNote, 64))
        {
            Unicode::strcpy(wszPopup, wszSetText);
            if (wszActionNote[0].ch)
                Unicode::strcat(wszPopup, pwszNewline);
            Unicode::strcat(wszPopup, wszActionNote);
        }
        else
        {
            Unicode::strcpy(wszPopup, wszSetText);
            if (sgInventoryMode != 4)
            {
                Unicode wszPriceless[64];

                Unicode::strcpy(wszPriceless, D2LANG_GetStringFromTblIndex(STR_IDX_3333_priceless));
                Unicode::strcat(wszPopup, pwszNewline);
                Unicode::strcat(wszPopup, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszPriceless, 1));
            }
        }
    }

    GFXUTIL_ReturnArg(wszPopup);
    D2WIN_10131_GetTextDimensions(wszPopup, &nWidth, &nHeight);
    if (gnDescPopupY - nHeight > 0)
    {
        D2WIN_10129_DrawFramedText(wszPopup, gnDescPopupX, gnDescPopupY, 0, 1);
        return;
    }
    D2WIN_10129_DrawFramedText(wszPopup, gnDescPopupAltX, (gnDescPopupBottomY + nHeight), 0, 1);
}


//  What the item will become: inv.cpp lines 3676 and 3679, inlined once in the original. The caller enters on cursor state 8 or a
//  transmogrifiable item, and this returns with nothing drawn when the item is not one: on cursor state 8 an ordinary item gets no description at
//  all. The new item's name, a newline, tbl 5387, a newline, then the current name, drawn left-aligned. The first strcpy is dead: the second
//  overwrites it.
static void __fastcall INV_DrawTransmogrifyDescription(void)
{
    const Unicode *pwszNewline;
    D2ItemsTxt *pOldItemsTxt;
    D2ItemsTxt *pNewItemsTxt;
    int32_t nNewItemId;
    int32_t nWidth;
    int32_t nHeight;

    Unicode wszText[1024];

    pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
    if (!D2COMMON_ITEMS_GetTransmogrify(gpHoverItem))
        return;

    pOldItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(gpHoverItem ? gpHoverItem->dwClassId : -1);
    if (!pOldItemsTxt)
    {
        FOG_DisplayAssert(szPtOldItemStatsAssert, szInvCpp, 3676);
        exit(-1);
    }

    nNewItemId = 0;
    pNewItemsTxt = D2COMMON_DATATBLS_GetItemRecordFromItemCode(pOldItemsTxt->dwTransmogrifyType, &nNewItemId);
    if (!pNewItemsTxt)
    {
        FOG_DisplayAssert(szPtNewItemStatsAssert, szInvCpp, 3679);
        exit(-1);
    }

    //  dead: overwritten on the next line (0x6FAE3CE4)
    Unicode::strcpy(wszText, D2LANG_GetStringFromTblIndex(STR_IDX_5387_convertsto));
    Unicode::strcpy(wszText, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pNewItemsTxt->wNameStr));
    Unicode::strcat(wszText, pwszNewline);
    Unicode::strcat(wszText, D2LANG_GetStringFromTblIndex(STR_IDX_5387_convertsto));
    Unicode::strcat(wszText, pwszNewline);

    Unicode wszName[128];

    INV_BuildItemNameText(gpHoverItem, wszName, 128);
    GFXUTIL_ReturnArg(wszName);
    Unicode::strcat(wszText, wszName);

    //  element 0 stored, 1..1023 cleared and constructed (mov word ptr [..], 0, rep stosd x 1FFh + stosw, a 3FFh-count loop)
    Unicode wszOut[1024];
    wszOut[0].ch = 0;
    memset(&wszOut[1], 0, 1023 * sizeof(Unicode));

    if (sgInventoryMode < 1 || sgInventoryMode > 9)
        Unicode::strcpy(wszOut, wszText);
    else
        INV_AppendPriceFooterText(wszText, wszOut);
    GFXUTIL_ReturnArg(wszOut);

    D2WIN_10131_GetTextDimensions(wszOut, &nWidth, &nHeight);
    if (gnDescPopupY - nHeight > 0)
    {
        D2WIN_10129_DrawFramedText(wszOut, gnDescPopupX, gnDescPopupY, 0, 0);
        return;
    }
    D2WIN_10129_DrawFramedText(wszOut, gnDescPopupAltX, (gnDescPopupBottomY + nHeight), 0, 0);
}


//1.10f: D2Client.0x6FAE1890
//  The item's description: the hover box, and the hints of the panel buttons under it. In order:
//      the hint phase     the mercenary panel's close button, the inventory family's close and gold buttons, the weapon-swap hint in the
//                         expansion; it does not replace the item's description: every exit goes on
//      the entry tests    show-desc flags, a hover item, the unit's own inventory, nothing on the cursor, the item in this inventory unless trading
//      the vendor stall   "Unidentified", the class, the name: three lines while an NPC menu is up in a vendor mode
//      transmogrify, a set item, a tome: the three helpers above
//      the ordinary path  seventeen sections and the name, each coloured, concatenated bottom to top: the first section is drawn lowest and
//                         the name, last, on top
//  The sections are built into the buffers constructed at entry, whether or not this path uses them.
void __fastcall INV_DrawItemDescription(D2UnitStrc *pUnit, BOOL bVendorUnit)
{
    const Unicode *pwszNewline;
    const Unicode *pwszSpace;
    const Unicode *pwszLineEnd;
    D2UnitStrc *pOwner;
    D2UnitStrc *pQuiver;
    D2UnitStrc *pNode;
    D2UnitStrc *pSocketed;
    D2InventoryStrc *pInventory;
    D2InventoryStrc *pSocketInventory;
    D2ItemsTxt *pItemsTxt;
    D2GemsTxt *pGems;
    D2C_ItemQualities nItemQuality;
    D2C_StringIndices nStrId;
    uint32_t nColorLevel;
    uint32_t nColorStrength;
    uint32_t nColorDexterity;
    uint32_t nColorClass;
    uint32_t nNameColor;
    uint32_t nDrawColor;
    uint32_t dwCode;
    BOOL bStrengthOk;
    BOOL bDexterityOk;
    BOOL bLevelOk;
    BOOL bRuneQuoteOpen;
    int32_t nItemClass;
    int32_t nLevel;
    int32_t nReqPercent;
    int32_t nReqStrBonus;
    int32_t nReqDexBonus;
    int32_t nGemId;
    int32_t nX;
    int32_t nY;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nWidth;
    int32_t nHeight;
    char szNumber[10];

    nNameColor = 0;
    nColorLevel = 0;
    nColorStrength = 0;
    nColorDexterity = 0;

    Unicode wszCombined[1024];
    Unicode wszPopupText[1024];
    Unicode wszUnused256[256];      // constructed, never read
    Unicode wszSocketFillers[256];
    Unicode wszCharmNote[256];
    Unicode wszRuneLetters[256];
    Unicode wszDurability[256];
    Unicode wszReqLevel[256];
    Unicode wszReqStrength[256];
    Unicode wszReqDexterity[256];
    Unicode wszElemDamage[256];
    Unicode wszChanceToBlock[256];
    Unicode wszSkillDamage[256];
    Unicode wszDefense[256];
    Unicode wszUnidentified[256];
    Unicode wszClassOnly[256];
    nColorClass = 0;
    Unicode wszEtherealSockets[256];
    Unicode wszQuantity[256];
    Unicode wszAttackSpeed[1024];
    Unicode wszItemName[1024];
    Unicode wszMagicMods[4096];

    pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);

    Unicode wszUnused10[10];        // constructed, never read

    pOwner = pUnit;
    if (bVendorUnit && CUNIT_GetPlayer())
        pOwner = CUNIT_GetPlayer();

    //  the hint phase. Each hotspot is a box anchored at its bottom-left, x..x+w by y-h..y, through the letterbox offsets. The numbers are the
    //  original's; D2ExpRes rewrites five of them in place.
    if (!PANEL_InvHintPhaseSuppressed())
    {
        if (UI_GetUiWindowState(UI_MERCINV))
        {
            nY = gnBorderYDifference + gnScreenHeight - 63;
            nX = gnBorderXDifference + 272;
            nMouseX = CURSOR_GetMouseX();
            nMouseY = CURSOR_GetMouseY();
            if (nMouseX >= nX && nMouseX <= nX + 32 && nMouseY >= nY - 32 && nMouseY <= nY)
            {
                D2WIN_10129_DrawFramedText(D2LANG_GetStringFromTblIndex(STR_IDX_4144_strClose), gnBorderXDifference + 287, gnBorderYDifference + gnScreenHeight - 98, 0, 1);
            }
        }

        if (UI_GetUiWindowState(UI_INVENTORY) || UI_GetUiWindowState(UI_MPTRADE) || UI_GetUiWindowState(UI_STASH) || UI_GetUiWindowState(UI_NPCSHOP) || UI_GetUiWindowState(UI_ANVIL) || UI_GetUiWindowState(UI_CUBE))
        {
            nY = gnBorderYDifference + gnScreenHeight - 64;
            nX = gnScreenWidth - gnBorderXDifference - 302;
            nMouseX = CURSOR_GetMouseX();
            nMouseY = CURSOR_GetMouseY();
            if (nMouseX >= nX && nMouseX <= nX + 32 && nMouseY >= nY - 32 && nMouseY <= nY)
            {
                D2WIN_10129_DrawFramedText(D2LANG_GetStringFromTblIndex(STR_IDX_4144_strClose), gnScreenWidth - gnBorderXDifference - 287, gnBorderYDifference + gnScreenHeight - 99, 0, 1);
            }

            //  the gold button, 20 by 18, and not while a dialog is open
            nY = gnBorderYDifference + gnScreenHeight - 69;
            nX = gnScreenWidth - gnBorderXDifference - 237;
            nMouseX = CURSOR_GetMouseX();
            nMouseY = CURSOR_GetMouseY();
            if (nMouseX >= nX && nMouseX <= nX + 20 && nMouseY >= nY - 18 && nMouseY <= nY && !NPCMENU_IsAnyDialogOpen())
            {
                nStrId = STR_IDX_4125_strGoldDrop;
                switch (sgInventoryMode)
                {
                case 1:
                    nStrId = STR_IDX_4099_strGoldLabel;
                    break;
                case 11:
                    nStrId = STR_IDX_4129_strGoldTradepup;
                    break;
                case 12:
                case 13:
                case 15:
                    nStrId = STR_IDX_4126_strGoldDeposit;
                    break;
                default:
                    break;
                }
                D2WIN_10129_DrawFramedText(D2LANG_GetStringFromTblIndex(nStrId), gnScreenWidth - gnBorderXDifference - 222, gnBorderYDifference + gnScreenHeight - 89, 0, 1);
            }

            //  the weapon-swap hint: a 24-pixel band above either arm's slot, and the mouse's Y is asked before its X
            if (FOG_IsExpansion() && GAME_IsExpansionGame())
            {
                nMouseY = CURSOR_GetMouseY();
                nMouseX = CURSOR_GetMouseX();
                if ((nMouseX >= gInvBodyLocRects[BODYLOC_LARM].rect.nLeft && nMouseX <= gInvBodyLocRects[BODYLOC_LARM].rect.nRight && nMouseY >= gInvBodyLocRects[BODYLOC_LARM].rect.nTop - 24 && nMouseY < gInvBodyLocRects[BODYLOC_LARM].rect.nTop)
                    || (nMouseX >= gInvBodyLocRects[BODYLOC_RARM].rect.nLeft && nMouseX <= gInvBodyLocRects[BODYLOC_RARM].rect.nRight && nMouseY >= gInvBodyLocRects[BODYLOC_RARM].rect.nTop - 24 && nMouseY < gInvBodyLocRects[BODYLOC_RARM].rect.nTop))
                {
                    Unicode wszSwapHint[512];

                    Unicode::strcpy(wszSwapHint, D2LANG_GetStringFromTblIndex(STR_IDX_22726_Cfgswapweapons));
                    //  cmp ax, 0FFFFh: the binding is a short
                    if (CMDTBL_ControlsFindBoundKey(HOTKEY_SWITCHWEAP, TRUE) != D2VK_INVALID)
                    {
                        Unicode::strcat(wszSwapHint, D2LANG_GetStringFromTblIndex(STR_IDX_3997_colon));
                        Unicode::strcat(wszSwapHint, CMDTBL_ControlsGetBoundKeyName(HOTKEY_SWITCHWEAP, TRUE));
                    }
                    else if (CMDTBL_ControlsFindBoundKey(HOTKEY_SWITCHWEAP, FALSE) != D2VK_INVALID)
                    {
                        Unicode::strcat(wszSwapHint, D2LANG_GetStringFromTblIndex(STR_IDX_3997_colon));
                        Unicode::strcat(wszSwapHint, CMDTBL_ControlsGetBoundKeyName(HOTKEY_SWITCHWEAP, FALSE));
                    }

                    nX = 368;
                    if (CURSOR_GetMouseX() > gnBorderXDifference + 500)
                        nX = 600;
                    D2WIN_10129_DrawFramedText(wszSwapHint, nX + gnBorderXDifference, 21 - gnBorderYDifference, 0, 1);
                }
            }
        }
    }

    //  the entry tests
    if (!gbShowSetItemDesc && !gbShowItemDesc)
        return;
    if (!gpHoverItem)
        return;
    if (!pUnit->pInventory)
        return;
    if (D2COMMON_INVENTORY_GetCursorItem(pUnit->pInventory))
        return;
    pInventory = pUnit->pInventory;
    if (!UI_GetUiWindowState(UI_MPTRADE) && pInventory && gpHoverItem && !D2COMMON_INVENTORY_CompareWithItemsParentInventory(pInventory, gpHoverItem))
    {
        return;
    }

    //  the vendor stall: three lines, coloured once as a whole. The class comes from the item type through a switch of its own, not
    //  ITEMS_GetClassOfClassSpecificItem as the ordinary path asks; the two agree for every stock type. The name is built to 128 into the 1024
    //  constructed at entry.
    if (gbShowItemDesc && !gnNpcTradeMode && NPCMENU_IsVendorGamble() && sgInventoryMode >= 1 && sgInventoryMode <= 9)
    {
        INV_BuildItemNameText(gpHoverItem, wszItemName, 128);
        Unicode::strcpy(wszUnidentified, D2LANG_GetStringFromTblIndex(STR_IDX_3455_ItemStats1b));
        Unicode::strcat(wszUnidentified, pwszNewline);
        Unicode::strcat(wszCombined, wszUnidentified);

        switch (D2COMMON_ITEMS_GetItemType(gpHoverItem))
        {
        case ITEMTYPE_AMAZON_ITEM:
        case ITEMTYPE_AMAZON_BOW:
        case ITEMTYPE_AMAZON_SPEAR:
        case ITEMTYPE_AMAZON_JAVELIN:
            nStrId = STR_IDX_10917_AmaOnly;
            break;
        case ITEMTYPE_BARBARIAN_ITEM:
        case ITEMTYPE_PRIMAL_HELM:
            nStrId = STR_IDX_10921_BarOnly;
            break;
        case ITEMTYPE_NECROMANCER_ITEM:
        case ITEMTYPE_VOODOO_HEADS:
            nStrId = STR_IDX_10919_NecOnly;
            break;
        case ITEMTYPE_PALADIN_ITEM:
        case ITEMTYPE_AURIC_SHIELDS:
            nStrId = STR_IDX_10920_PalOnly;
            break;
        case ITEMTYPE_SORCERESS_ITEM:
        case ITEMTYPE_ORB:
            nStrId = STR_IDX_10918_SorOnly;
            break;
        case ITEMTYPE_ASSASSIN_ITEM:
        case ITEMTYPE_HAND_TO_HAND:
        case ITEMTYPE_HAND_TO_HAND_2:
            nStrId = STR_IDX_10923_AssOnly;
            break;
        case ITEMTYPE_DRUID_ITEM:
        case ITEMTYPE_PELT:
        case ITEMTYPE_CLOAK:
            nStrId = STR_IDX_10922_DruOnly;
            break;
        default:
            //  the bytes jump past the two appends; no tbl index is 0 here
            nStrId = (D2C_StringIndices)0;
            break;
        }
        if (nStrId)
        {
            Unicode::strcat(wszCombined, D2LANG_GetStringFromTblIndex(nStrId));
            Unicode::strcat(wszCombined, pwszNewline);
        }

        Unicode::strcat(wszCombined, wszItemName);
        UI_AddColorCodeToWstring((wchar_t *)wszCombined, 0);
        INV_AppendPriceFooterText(wszCombined, wszPopupText);

        D2WIN_10131_GetTextDimensions(wszPopupText, &nWidth, &nHeight);
        if (gnDescPopupY - nHeight > 0)
        {
            D2WIN_10129_DrawFramedText(wszPopupText, gnDescPopupX, gnDescPopupY, 0, 1);
            return;
        }
        D2WIN_10129_DrawFramedText(wszPopupText, gnDescPopupAltX, (gnDescPopupBottomY + nHeight), 0, 1);
        return;
    }

    //  the three helpers. The quality is asked first, before the cursor or the item; the set arm's flag test only when it is a set item.
    nItemQuality = D2COMMON_ITEMS_GetItemQuality(gpHoverItem);
    if (CURSOR_GetCursorState() == 8 || D2COMMON_ITEMS_GetTransmogrify(gpHoverItem))
    {
        INV_DrawTransmogrifyDescription();
        return;
    }
    if (nItemQuality == ITEMQUAL_SET && D2COMMON_ITEMS_CheckItemFlag(gpHoverItem, IFLAG_IDENTIFIED, 3994, szInvCpp))
    {
        INV_DrawSetItemDescription(pUnit);
        return;
    }
    if (D2COMMON_ITEMS_GetItemType(gpHoverItem) == ITEMTYPE_BOOK)
    {
        INV_DrawBookDescription();
        return;
    }

    //  the ordinary path. The class line: red when the owner's class differs (a missing owner counts as class -1), with no test of the unit
    //  type, unlike the set arm.
    nItemClass = D2COMMON_ITEMS_GetClassOfClassSpecificItem(gpHoverItem);
    if (nItemClass != PCLASS_INVALID)
    {
        Unicode::strcat(wszClassOnly, D2LANG_GetStringFromTblIndex((D2C_StringIndices)(nItemClass + STR_IDX_10917_AmaOnly)));
        Unicode::strcat(wszClassOnly, pwszNewline);
        if ((pOwner ? pOwner->dwClassId : -1) != nItemClass)
            nColorClass = 1;
    }

    pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(gpHoverItem ? gpHoverItem->dwClassId : -1);
    if (!pItemsTxt)
    {
        FOG_DisplayAssert(szPtItemStatsAssert, szInvCpp, 4029);
        exit(-1);
    }

    if (D2COMMON_ITEMS_GetQuiverType(gpHoverItem))
    {
        //  dead: into wszCombined, which the combine below strcpys over before anything reads it. The quantity a player sees on arrows comes
        //  from the identified block further down. Reproduced.
        pQuiver = gpHoverItem;

        Unicode wszQuiverNumber[10];

        pwszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
        pwszLineEnd = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
        if (D2COMMON_STATLIST_UnitGetStatValue(pQuiver, STAT_QUANTITY, 0) > 0)
        {
            Unicode::strcpy(wszCombined, D2LANG_GetStringFromTblIndex(STR_IDX_3462_ItemStats1i));
            STORM_SStrPrintf(szNumber, 10, "%ld", D2COMMON_STATLIST_UnitGetStatValue(pQuiver, STAT_QUANTITY, 0));
            Unicode::win2Unicode(wszQuiverNumber, szNumber, 9);
            Unicode::strcat(wszCombined, pwszSpace);
            Unicode::strcat(wszCombined, wszQuiverNumber);
            Unicode::strcat(wszCombined, pwszLineEnd);
            Unicode::strcat(wszCombined, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pItemsTxt->wNameStr));
        }
    }
    else
    {
        INV_AppendDurabilityText(gpHoverItem, wszDurability, pItemsTxt);
    }

    //  out-parameters in the order strength, dexterity, level
    D2COMMON_ITEMS_CheckRequirements(gpHoverItem, pOwner, FALSE, &bStrengthOk, &bDexterityOk, &bLevelOk);

    if (D2COMMON_ITEMS_CheckItemFlag(gpHoverItem, IFLAG_IDENTIFIED, 4049, szInvCpp))
    {
        nLevel = D2COMMON_ITEMS_GetLevelRequirement(gpHoverItem, pOwner);
        if (nLevel > 1)
        {
            if (!bLevelOk)
                nColorLevel = 1;
            INV_FormatReqLevelLine(wszReqLevel, nLevel);   // inlined in the original
        }
    }

    if (D2COMMON_ITEMS_CheckItemTypeId(gpHoverItem, ITEMTYPE_SOCKET_FILLER))
    {
        INV_AppendSocketFillerText(wszSocketFillers);
    }

    //  the runes in the sockets, between quotes
    if (D2COMMON_ITEMS_CheckIfSocketable(gpHoverItem))
    {
        pSocketInventory = gpHoverItem->pInventory;
        if (pSocketInventory)
        {
            //  six: toUnicode's count below; the longest stock letter is five, so its terminator fits
            Unicode wszRuneLetter[6];

            bRuneQuoteOpen = FALSE;
            pwszLineEnd = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
            pNode = D2COMMON_INVENTORY_GetFirstItem(pSocketInventory);
            if (pNode)
            {
                do
                {
                    pSocketed = D2COMMON_INVENTORY_UnitIsItem(pNode);
                    if (!pSocketed)
                        break;
                    if (D2COMMON_ITEMS_CheckItemTypeId(pSocketed, ITEMTYPE_RUNE))
                    {
                        //  BUG: the socketed rune's ItemsTxt record is not tested for NULL (0x6FAE3127). UNREACHABLE: a socketed item's class
                        //  indexes ItemsTxt.
                        nGemId = (int32_t)D2COMMON_DATATBLS_GetItemsTxtRecord(pSocketed->dwClassId)->dwGemOffset;
                        if (nGemId > 0)
                        {
                            pGems = D2COMMON_DATATBLS_GetGemsTxtRecord(nGemId);
                            if (pGems)
                            {
                                if (!bRuneQuoteOpen)
                                {
                                    bRuneQuoteOpen = TRUE;
                                    Unicode::strcat(wszRuneLetters, D2LANG_GetStringFromTblIndex(STR_IDX_20506_RuneQuote));
                                }
                                Unicode::toUnicode(wszRuneLetter, pGems->szLetter, 6);
                                Unicode::strcat(wszRuneLetters, wszRuneLetter);
                            }
                        }
                    }
                    pNode = D2COMMON_INVENTORY_GetNextItem(pNode);
                }
                while (pNode);

                if (bRuneQuoteOpen)
                {
                    Unicode::toUnicode(wszRuneLetter, gszRuneQuoteClose, 5);
                    Unicode::strcat(wszRuneLetters, wszRuneLetter);
                    Unicode::strcat(wszRuneLetters, pwszLineEnd);
                }
            }
        }
    }

    if (D2COMMON_ITEMS_CheckItemTypeId(gpHoverItem, ITEMTYPE_CHARM))
    {
        //  the newline is fetched first, as the bytes do
        pwszLineEnd = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
        Unicode::strcat(wszCharmNote, D2LANG_GetStringFromTblIndex(STR_IDX_20438_Charmdes));
        Unicode::strcat(wszCharmNote, pwszLineEnd);
    }

    if (D2COMMON_ITEMS_CheckItemTypeId(gpHoverItem, ITEMTYPE_WEAPON) || D2COMMON_ITEMS_CheckItemTypeId(gpHoverItem, ITEMTYPE_ANY_ARMOR))
    {
        INV_AppendEtherealAndSocketsText(gpHoverItem, wszEtherealSockets);

        //  DATATBLS_ApplyRatio(base, percentage, 100), inlined in the bytes with only the !nDivisor test folded away
        nReqDexBonus = 0;
        nReqStrBonus = 0;
        nReqPercent = D2COMMON_STATLIST_UnitGetItemStatOrSkillStatValue(gpHoverItem, STAT_ITEM_REQ_PERCENT, 0);
        if (nReqPercent)
        {
            nReqStrBonus = DATATBLS_ApplyRatio(pItemsTxt->wReqStr, nReqPercent, 100);
            nReqDexBonus = DATATBLS_ApplyRatio(pItemsTxt->wReqDex, nReqPercent, 100);
        }
        if (D2COMMON_ITEMS_CheckItemFlag(gpHoverItem, IFLAG_ETHEREAL, 4085, szInvCpp))
        {
            nReqStrBonus -= 10;
            nReqDexBonus -= 10;
        }

        //  dexterity before strength here, and each is an inlined copy of the line function
        if (pItemsTxt->wReqDex)
        {
            if (!bDexterityOk)
                nColorDexterity = 1;
            INV_FormatReqDexterityLine(wszReqDexterity, pItemsTxt, nReqDexBonus);
        }
        if (pItemsTxt->wReqStr)
        {
            if (!bStrengthOk)
                nColorStrength = 1;
            INV_FormatReqStrengthLine(wszReqStrength, pItemsTxt, nReqStrBonus);
        }

        if (D2COMMON_ITEMS_CheckItemTypeId(gpHoverItem, ITEMTYPE_WEAPON))
        {
            if (D2COMMON_STATLIST_GetMinDamageFromUnit(gpHoverItem, FALSE) >= 0 && D2COMMON_STATLIST_GetMaxDamageFromUnit(gpHoverItem, FALSE) >= 0)
            {
                INV_AppendElementalDamageText(gpHoverItem, wszElemDamage, pItemsTxt);
            }
            INV_AppendAttackSpeedText(gpHoverItem, wszAttackSpeed, pItemsTxt);
        }

        //  smite for a paladin's shield, kick for an assassin's boots; the boots test only in the shield test's else
        if (D2COMMON_ITEMS_CheckItemTypeId(gpHoverItem, ITEMTYPE_ANY_SHIELD))
        {
            if (pOwner && pOwner->dwClassId == PCLASS_PALADIN && (!D2COMMON_ITEMS_IsClassValid(gpHoverItem) || D2COMMON_ITEMS_GetClassOfClassSpecificItem(gpHoverItem) == PCLASS_PALADIN))
            {
                INV_AppendSkillDamageText(gpHoverItem, wszSkillDamage, pItemsTxt);
            }
            INV_AppendChanceToBlockText(gpHoverItem, wszChanceToBlock, pItemsTxt);
        }
        else if (D2COMMON_ITEMS_CheckItemTypeId(gpHoverItem, ITEMTYPE_BOOTS) && pOwner && pOwner->dwClassId == PCLASS_ASSASSIN)
        {
            INV_AppendSkillDamageText(gpHoverItem, wszSkillDamage, pItemsTxt);
        }

        if (D2COMMON_ITEMS_CheckItemTypeId(gpHoverItem, ITEMTYPE_ANY_ARMOR) && D2COMMON_STATLIST_GetDefenseFromUnit(gpHoverItem) > 0)
        {
            INV_AppendDefenseText(pOwner, gpHoverItem, wszDefense, pItemsTxt);
        }
    }

    //  the name's colour, by quality; low, normal and superior grey when socketed or ethereal
    switch (nItemQuality)
    {
    case ITEMQUAL_MAGIC:
        nNameColor = 3;
        break;
    case ITEMQUAL_SET:
        nNameColor = 2;
        break;
    case ITEMQUAL_RARE:
        nNameColor = 9;
        break;
    case ITEMQUAL_UNIQUE:
        nNameColor = 4;
        break;
    case ITEMQUAL_CRAFT:
        nNameColor = 8;
        break;
    case ITEMQUAL_TEMPERED:
        nNameColor = 10;
        break;
    default:
        if (D2COMMON_ITEMS_CheckItemFlag(gpHoverItem, IFLAG_SOCKETED, 4171, szInvCpp) || D2COMMON_ITEMS_CheckItemFlag(gpHoverItem, IFLAG_ETHEREAL, 4175, szInvCpp))
        {
            nNameColor = 5;
        }
        break;
    }
    if (!D2COMMON_ITEMS_CheckItemFlag(gpHoverItem, IFLAG_IDENTIFIED, 4184, szInvCpp) && sgInventoryMode >= 1 && sgInventoryMode <= 9 && !gnNpcTradeMode)
    {
        nNameColor = 0;
    }

    if (D2COMMON_ITEMS_CheckItemFlag(gpHoverItem, IFLAG_IDENTIFIED, 4196, szInvCpp))
    {
        if (!D2COMMON_ITEMS_CheckItemFlag(gpHoverItem, IFLAG_SOCKETED, 4199, szInvCpp))
        {
            //  inlined in the original, with no ItemsTxt argument to ignore
            INV_AppendStackQuantityText(gpHoverItem, wszQuantity, pItemsTxt);
        }
        MODS_AppendMagicModsText(gpHoverItem, wszMagicMods, 4096, 1, (const Unicode *)NULL);
    }
    else
    {
        Unicode::strcpy(wszUnidentified, D2LANG_GetStringFromTblIndex(STR_IDX_3455_ItemStats1b));
        Unicode::strcat(wszUnidentified, pwszNewline);
    }

    //  BUG: the spell description is appended into the quantity line's buffer (see INV_AppendSpellDescText). LATENT: stock data has no such item.
    INV_AppendSpellDescText(gpHoverItem, wszQuantity, pItemsTxt);
    INV_BuildItemNameText(gpHoverItem, wszItemName, 256);

    //  the combine, bottom to top
    Unicode::strcpy(wszCombined, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszEtherealSockets, 3));
    Unicode::strcat(wszCombined, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszMagicMods, 3));
    Unicode::strcat(wszCombined, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszUnidentified, 1));
    Unicode::strcat(wszCombined, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszAttackSpeed, 0));
    Unicode::strcat(wszCombined, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszReqLevel, nColorLevel));
    Unicode::strcat(wszCombined, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszReqStrength, nColorStrength));
    Unicode::strcat(wszCombined, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszReqDexterity, nColorDexterity));
    Unicode::strcat(wszCombined, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszClassOnly, nColorClass));
    Unicode::strcat(wszCombined, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszDurability, 0));
    Unicode::strcat(wszCombined, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszSocketFillers, 0));
    Unicode::strcat(wszCombined, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszCharmNote, 0));
    Unicode::strcat(wszCombined, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszQuantity, 0));
    Unicode::strcat(wszCombined, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszElemDamage, 0));
    Unicode::strcat(wszCombined, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszSkillDamage, 0));
    Unicode::strcat(wszCombined, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszChanceToBlock, 0));
    Unicode::strcat(wszCombined, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszDefense, 0));
    Unicode::strcat(wszCombined, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszRuneLetters, 4));
    if (D2COMMON_ITEMS_CheckItemFlag(gpHoverItem, IFLAG_BROKEN, 4239, szInvCpp))
    {
        nNameColor = 1;
    }
    Unicode::strcat(wszCombined, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszItemName, nNameColor));

    //  mode == 0 || is redundant beside mode < 1 and it is in the bytes
    if (sgInventoryMode == 0 || sgInventoryMode < 1 || sgInventoryMode > 9)
    {
        Unicode::strcpy(wszPopupText, wszCombined);
    }
    else
    {
        INV_AppendPriceFooterText(wszCombined, wszPopupText);
    }

    //  a quest item, Wirt's Leg excepted, is drawn in gold, and in the inventory a stored Bark Scroll or Horadric Cube gets its right-click hint
    //  as the bottom line; anything else draws with colour 0 (the original loads a slot zeroed at entry that no path reaching here writes)
    nDrawColor = 0;
    if (pItemsTxt->nQuest)
    {
        dwCode = pItemsTxt->dwCode;
        if (dwCode != 0x2067656C)                                        // 'leg '
        {
            if (!sgInventoryMode && (!gpHoverItem || !gpHoverItem->dwItemMode))
            {
                if (dwCode == 0x20646B62)                                // 'bkd '
                {
                    Unicode::strcpy(wszCombined, wszPopupText);
                    Unicode::strcpy(wszPopupText, D2LANG_GetStringFromTblIndex(STR_IDX_2205_RightClicktoRead));
                    Unicode::strcat(wszPopupText, pwszNewline);
                    Unicode::strcat(wszPopupText, wszCombined);
                }
                else if (dwCode == 0x20786F62)                           // 'box '
                {
                    Unicode::strcpy(wszCombined, wszPopupText);
                    Unicode::strcpy(wszPopupText, D2LANG_GetStringFromTblIndex(STR_IDX_2204_RightClicktoOpen));
                    Unicode::strcat(wszPopupText, pwszNewline);
                    Unicode::strcat(wszPopupText, wszCombined);
                }
            }
            nDrawColor = 4;
            UI_AddColorCodeToWstring((wchar_t *)wszPopupText, 4);
        }
    }

    GFXUTIL_ReturnArg(wszPopupText);

    //  the clamp comes after the text is built; jb, so unsigned
    if ((uint32_t)Unicode::strlen(wszPopupText) >= 1023)
        wszPopupText[1023].ch = 0;

    D2WIN_10131_GetTextDimensions(wszPopupText, &nWidth, &nHeight);
    if (gnDescPopupY - nHeight > 0)
    {
        D2WIN_10129_DrawFramedText(wszPopupText, gnDescPopupX, gnDescPopupY, nDrawColor, 1);
        return;
    }
    D2WIN_10129_DrawFramedText(wszPopupText, gnDescPopupAltX, (gnDescPopupBottomY + nHeight), nDrawColor, 1);
}

//  The description's last two callees in this unit: the damage lines (0x6FAE43D0, 2159 bytes) and the price footer (0x6FAE5A40, 399 bytes).
//  After them INV_DrawItemDescription leaves inv.cpp only for the property builders of another translation unit.
//  The throwing potion's range half: the "to <max>" half is skipped when the shifted minimum equals the maximum (cmp ecx, eax at 0x6FAE45DE
//  compares the two values stored after sar 8). The throwing block's buffer is 256 (mov [..], 100h at 0x6FAE4AC7). Type 38 is not ammo: it is
//  ITEMTYPE_MISSILE_POTION ('tpot', the throwing potions) and its label is tbl 3467 "Throw Damage:". The two-line shape is the barbarian's alone:
//  ITEMS_Is1Or2HandedForBarbarian is FALSE unless the player is a barbarian, so every other class sees one line.
//  Only the minimum is coloured in the weapon lines, but a colour code runs to the next one, so " to <max>" inherits it. The two-line shape does
//  not reset its adjusted flag between its two ranges: a bonus on the two-handed range colours the one-handed line too. Digit counts differ by
//  shape (9 for the potion, the single line and the throw, 5 for the two-line shape), so a two-line range above 99999 would print truncated;
//  none does.

//1.10f: D2Client.0x6FAE43D0
//  The damage lines, three shapes by what the item is:
//      a throwing potion   "Throw Damage: a to b", from the missile it throws; the element picks the numbers' colour (1 red, 2 gold, 4 blue,
//                          5 green) and poison is spread over its duration in 25-frame steps; 8.8 fixed point, shifted down after the physical
//                          damage is added
//      a barbarian's 1-or-2-handed sword   both lines, two-handed first
//      any other weapon    one line, by ITEMS_CheckWeaponIfTwoHanded, the maximum forced above the minimum
//  and, after the weapon shapes, a throwing weapon's "Throw Damage" line. pItemsTxt is not read; retn 4 pops it.
void __fastcall INV_AppendElementalDamageText(D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt)
{
    const Unicode *pwszSpace;
    const Unicode *pwszNewline;
    D2UnitStrc *pPlayer;
    D2C_StringIndices nLabel;
    int32_t nMissileId;
    int32_t nPoisonSteps;
    int32_t nMaxRaw;
    int32_t nMinDamage;
    int32_t nMaxDamage;
    uint32_t nColor;
    BOOL bAdjusted;
    char szNumber[256];

    Unicode wszNumber[256];

    pwszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
    pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
    pPlayer = CUNIT_GetPlayer();

    //  a throwing potion
    if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_MISSILE_POTION))
    {
        nMissileId = D2COMMON_ITEMS_GetMissileType(pItem);

        Unicode wszLine[256];

        nColor = 0;
        nMinDamage = D2COMMON_MISSILE_GetMinElemDamage(NULL, pPlayer, nMissileId, 1);
        nMaxDamage = D2COMMON_MISSILE_GetMaxElemDamage(NULL, pPlayer, nMissileId, 1);
        Unicode::strcpy(wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_3467_ItemStats1n));
        UI_AddColorCodeToWstring((wchar_t *)wszLine, 0);

        switch (D2COMMON_MISSILE_GetElemTypeFromMissileId(nMissileId))
        {
        case 1:
            nColor = 1;
            break;
        case 2:
            nColor = 4;
            break;
        case 4:
            nColor = 3;
            break;
        case 5:
            nColor = 2;
            nPoisonSteps = D2COMMON_MISSILE_GetElementalLength(0, pPlayer, nMissileId, 1) / 25;
            if (nPoisonSteps <= 0)
                nPoisonSteps = 1;
            nMinDamage /= nPoisonSteps;
            nMaxDamage /= nPoisonSteps;
            break;
        default:
            break;
        }

        nMinDamage += D2COMMON_MISSILE_GetMinDamage(NULL, pPlayer, nMissileId, 1);
        nMaxRaw = D2COMMON_MISSILE_GetMaxDamage(NULL, pPlayer, nMissileId, 1) + nMaxDamage;
        nMinDamage >>= 8;
        nMaxDamage = nMaxRaw >> 8;
        if (nMaxDamage <= nMinDamage)
            nMaxDamage = nMinDamage;

        STORM_SStrPrintf(szNumber, 256, "%i", nMinDamage);
        Unicode::win2Unicode(wszNumber, szNumber, 9);
        UI_AddColorCodeToWstring((wchar_t *)wszNumber, nColor);
        Unicode::strcat(wszLine, pwszSpace);
        Unicode::strcat(wszLine, wszNumber);

        //  the shifted minimum against the maximum
        if (nMinDamage != nMaxDamage)
        {
            Unicode::strcat(wszLine, pwszSpace);
            Unicode::strcat(wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_3464_ItemStast1k));
            STORM_SStrPrintf(szNumber, 256, "%i", nMaxDamage);
            Unicode::win2Unicode(wszNumber, szNumber, 9);
            UI_AddColorCodeToWstring((wchar_t *)wszNumber, nColor);
            Unicode::strcat(wszLine, pwszSpace);
            Unicode::strcat(wszLine, wszNumber);
        }
        Unicode::strcat(wszLine, pwszNewline);
        Unicode::strcpy(wszOut, wszLine);
        return;
    }

    //  a weapon
    bAdjusted = FALSE;
    if (D2COMMON_ITEMS_Is1Or2HandedForBarbarian(pPlayer, pItem))
    {
        INV_CalcElementalDamageRange(pPlayer, pItem, STAT_SECONDARY_MINDAMAGE, STAT_SECONDARY_MAXDAMAGE, &nMinDamage, &nMaxDamage, &bAdjusted);

        Unicode wszTwoHand[128];

        Unicode::strcpy(wszTwoHand, D2LANG_GetStringFromTblIndex(STR_IDX_3466_ItemStats1m));
        Unicode::strcat(wszTwoHand, pwszSpace);
        STORM_SStrPrintf(szNumber, 256, "%i", nMinDamage);
        Unicode::win2Unicode(wszNumber, szNumber, 5);
        if (bAdjusted)
            UI_AddColorCodeToWstring((wchar_t *)wszNumber, 3);
        Unicode::strcat(wszTwoHand, wszNumber);
        Unicode::strcat(wszTwoHand, pwszSpace);
        Unicode::strcat(wszTwoHand, D2LANG_GetStringFromTblIndex(STR_IDX_3464_ItemStast1k));
        STORM_SStrPrintf(szNumber, 256, "%i", nMaxDamage);
        Unicode::win2Unicode(wszNumber, szNumber, 5);
        Unicode::strcat(wszTwoHand, pwszSpace);
        Unicode::strcat(wszTwoHand, wszNumber);
        Unicode::strcat(wszTwoHand, pwszNewline);

        //  bAdjusted is carried over, not reset
        INV_CalcElementalDamageRange(pPlayer, pItem, STAT_MINDAMAGE, STAT_MAXDAMAGE, &nMinDamage, &nMaxDamage, &bAdjusted);

        Unicode wszOneHand[128];

        Unicode::strcpy(wszOneHand, D2LANG_GetStringFromTblIndex(STR_IDX_3465_ItemStats1l));
        Unicode::strcat(wszOneHand, pwszSpace);
        STORM_SStrPrintf(szNumber, 256, "%i", nMinDamage);
        Unicode::win2Unicode(wszNumber, szNumber, 5);
        if (bAdjusted)
            UI_AddColorCodeToWstring((wchar_t *)wszNumber, 3);
        Unicode::strcat(wszOneHand, wszNumber);
        Unicode::strcat(wszOneHand, pwszSpace);
        Unicode::strcat(wszOneHand, D2LANG_GetStringFromTblIndex(STR_IDX_3464_ItemStast1k));
        STORM_SStrPrintf(szNumber, 256, "%i", nMaxDamage);
        Unicode::win2Unicode(wszNumber, szNumber, 5);
        Unicode::strcat(wszOneHand, pwszSpace);
        Unicode::strcat(wszOneHand, wszNumber);
        Unicode::strcat(wszOneHand, pwszNewline);

        UI_AddColorCodeToWstring((wchar_t *)wszTwoHand, 0);
        UI_AddColorCodeToWstring((wchar_t *)wszOneHand, 0);
        Unicode::strcpy(wszOut, wszTwoHand);
        //  the bytes share this strcat with the other branch, whose operand is the newline (0x6FAE492F)
        Unicode::strcat(wszOut, wszOneHand);
    }
    else
    {
        if (D2COMMON_ITEMS_CheckWeaponIfTwoHanded(pItem))
        {
            INV_CalcElementalDamageRange(pPlayer, pItem, STAT_SECONDARY_MINDAMAGE, STAT_SECONDARY_MAXDAMAGE, &nMinDamage, &nMaxDamage, &bAdjusted);
            nLabel = STR_IDX_3466_ItemStats1m;
        }
        else
        {
            INV_CalcElementalDamageRange(pPlayer, pItem, STAT_MINDAMAGE, STAT_MAXDAMAGE, &nMinDamage, &nMaxDamage, &bAdjusted);
            nLabel = STR_IDX_3465_ItemStats1l;
        }
        if (nMaxDamage <= nMinDamage + 1)
            nMaxDamage = nMinDamage + 1;

        Unicode::strcpy(wszOut, D2LANG_GetStringFromTblIndex(nLabel));
        STORM_SStrPrintf(szNumber, 256, "%i", nMinDamage);
        Unicode::win2Unicode(wszNumber, szNumber, 9);
        //  the space before the colour here, after it in the two-line shape
        Unicode::strcat(wszOut, pwszSpace);
        if (bAdjusted)
            UI_AddColorCodeToWstring((wchar_t *)wszNumber, 3);
        Unicode::strcat(wszOut, wszNumber);
        Unicode::strcat(wszOut, pwszSpace);
        Unicode::strcat(wszOut, D2LANG_GetStringFromTblIndex(STR_IDX_3464_ItemStast1k));
        STORM_SStrPrintf(szNumber, 256, "%i", nMaxDamage);
        Unicode::win2Unicode(wszNumber, szNumber, 9);
        Unicode::strcat(wszOut, pwszSpace);
        Unicode::strcat(wszOut, wszNumber);
        Unicode::strcat(wszOut, pwszNewline);
    }

    //  a throwing weapon, after the weapon lines
    bAdjusted = FALSE;
    if (D2COMMON_STATLIST_GetUnitStatBonus(pItem, STAT_ITEM_MINDAMAGE_PERCENT, 0) || D2COMMON_STATLIST_GetUnitStatBonus(pItem, STAT_ITEM_MAXDAMAGE_PERCENT, 0) || D2COMMON_STATLIST_GetUnitStatBonus(pItem, STAT_ITEM_THROW_MINDAMAGE, 0)
        || D2COMMON_STATLIST_GetUnitStatBonus(pItem, STAT_ITEM_THROW_MAXDAMAGE, 0))
    {
        bAdjusted = TRUE;
    }

    if (D2COMMON_ITEMS_CheckIfThrowable(pItem))
    {
        Unicode wszThrow[256];

        Unicode::strcpy(wszThrow, D2LANG_GetStringFromTblIndex(STR_IDX_3467_ItemStats1n));
        UI_AddColorCodeToWstring((wchar_t *)wszThrow, 0);
        INV_CalcElementalDamageRange(pPlayer, pItem, STAT_ITEM_THROW_MINDAMAGE, STAT_ITEM_THROW_MAXDAMAGE, &nMinDamage, &nMaxDamage, &bAdjusted);

        STORM_SStrPrintf(szNumber, 256, "%i", nMinDamage);
        Unicode::win2Unicode(wszNumber, szNumber, 9);
        UI_AddColorCodeToWstring((wchar_t *)wszNumber, bAdjusted ? 3 : 0);
        Unicode::strcat(wszThrow, pwszSpace);
        Unicode::strcat(wszThrow, wszNumber);
        Unicode::strcat(wszThrow, pwszSpace);
        Unicode::strcat(wszThrow, D2LANG_GetStringFromTblIndex(STR_IDX_3464_ItemStast1k));
        STORM_SStrPrintf(szNumber, 256, "%i", nMaxDamage);
        Unicode::win2Unicode(wszNumber, szNumber, 9);
        UI_AddColorCodeToWstring((wchar_t *)wszNumber, bAdjusted ? 3 : 0);
        Unicode::strcat(wszThrow, pwszSpace);
        Unicode::strcat(wszThrow, wszNumber);
        Unicode::strcat(wszThrow, pwszNewline);
        Unicode::strcat(wszOut, wszThrow);
    }
}


//1.10f: D2Client.0x6FAE5A40
//  The footer under a vendor's description: the description copied, then whichever applies, each as newline-then-text:
//      mode 4 (VENDORMODE_REPAIR) and ethereal   tbl 22746 "This item cannot be repaired.", red
//      the NPC's line      NPCMENU_TransactionCost's note, when it answers (the newline only when the note is not empty), and nothing after it
//      no answer, not in mode 4   tbl 3333 "Item cannot be traded here.", red
//  It returns void: the four call sites, all in INV_DrawItemDescription, never read eax.
void __fastcall INV_AppendPriceFooterText(const Unicode *pwszIn, Unicode *pwszOut)
{
    const Unicode *pwszNewline;
    int32_t nCost;

    //  sixty-four: element 0 stored, 1..63 cleared and constructed (rep stosd x 1Fh + stosw, a 3Fh-count loop)
    Unicode wszActionNote[64];
    wszActionNote[0].ch = 0;
    memset(&wszActionNote[1], 0, 63 * sizeof(Unicode));

    pwszNewline = D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline);
    Unicode::strcpy(pwszOut, pwszIn);

    if (sgInventoryMode == 4 && D2COMMON_ITEMS_CheckItemFlag(gpHoverItem, IFLAG_ETHEREAL, 3271, szInvCpp))
    {
        Unicode wszCannotRepair[128];

        Unicode::strcpy(wszCannotRepair, D2LANG_GetStringFromTblIndex(STR_IDX_22746_X_ItemCanNotBeRepaired));
        Unicode::strcat(pwszOut, pwszNewline);
        Unicode::strcat(pwszOut, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszCannotRepair, 1));
    }

    if (NPCMENU_TransactionCost(gpHoverItem, gnNpcTradeMode, &nCost, wszActionNote, 64))
    {
        if (wszActionNote[0].ch)
            Unicode::strcat(pwszOut, pwszNewline);
        Unicode::strcat(pwszOut, wszActionNote);
        return;
    }

    if (sgInventoryMode != 4)
    {
        Unicode wszPriceless[64];

        Unicode::strcpy(wszPriceless, D2LANG_GetStringFromTblIndex(STR_IDX_3333_priceless));
        Unicode::strcat(pwszOut, pwszNewline);
        Unicode::strcat(pwszOut, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszPriceless, 1));
    }
}

//  The mercenary panel and the gold: INV_MercInvDrawStatRows (6FAEDA20, 1749 bytes), INV_MercInvDrawPanel (6FAED5D0, 1104 bytes; the stat rows'
//  only caller) and INV_DrawGoldAmount (6FAE97C0, 920 bytes; three calls, all from INV_UpdateAndDrawPanels).

//1.10f: D2Client.0x6FAEDA20
//  The mercenary's twelve stat rows. One pass over gMercStatRows, twelve records of 20 bytes indexed the way the bytes index them, [5 * i]
//  (lea ebx, [ebx+ebx*4] / shl ebx, 2 at 0x6FAEDA6C); the members are nLeft, nX, nY, nMaxWidth and nStatId (inv.h):
//      +0   nLeft       0 = right-align at the right edge
//      +4   nX          the right edge; 0 = left-align at nLeft; both set = centred between them
//      +8   nY          drawn at nY - gnBorderYDifference - 2
//      +12  nMaxWidth   0 disables the shrink-to-fit
//      +16  nStatId     the stat
//  Each row starts in colour 0 and is coloured by its stat against the base stat (3 above, 1 below) before its arm, if it has one, colours it
//  again. Five stats have an arm; the rest print the value through STR_GroupIntDigits:
//      6   hitpoints   "cur / max", cur from the roster's life percent
//      21  mindamage   the whole damage range, "min-max"
//      31  armorclass  UNITS_GetDefense rather than the stat, colour 3 under the ArmBlue state mask
//      39 41 43 45     the resistances, with the difficulty's penalty
//  BUG: the life floor cannot fire: if (!dead && nLife < 0) nLife = 1 (test ebp, ebp / jge at 0x6FAEDB3A) tests < 0, but storing 1 makes sense only
//  as a floor for a living hireling and nothing the test sees is negative (the roster percent is a zero-extended word,
//  SCMD_HandleSrv7F_PartyLifeUpdate at 0x6FAB42B2, and the max life is positive). REACHABLE: the server's percent is 100 * hp / maxhp in integer
//  division, so a hireling under 1% of its life reads "0 / <max>" while it is alive. Reproduced. The damage row: the row's own value is the
//  hireling's MINDAMAGE, with MAXDAMAGE read beside it. A two-handed weapon in the right hand has its own pair taken off both and the hireling's
//  SECONDARY pair added. The percentage is DAMAGEPERCENT plus the weapon's strength and dexterity bonuses as a percentage of the hireling's own
//  stats, plus the weapon mastery or, with no weapon, plus the whole strength (the dexterity for monster 271, the Act 1 rogue). Clamped at -90, then
//      min = normaldamage + min * (minpct + pct + 100) / 100
//      max = normaldamage + max * (maxpct + pct + 100) / 100
//  and four elemental pairs from a table the function builds on its stack, twelve dwords of {min stat, max stat, colour}: fire 1, lightning 9,
//  magic 3, cold 3. Each pair adds to both ends, and a pair that is not zero gives the row its colour, so the last such pair wins. Poison comes
//  after them, adds (dam * length) >> 8 to each end and colours 2.
//  The resistances: colour 3 under the element's blue state mask, 1 under its red, tested in that order. The MAX*RESIST stat raises the cap from
//  75, and the cap stops at 95. The expansion adds DifficultyLevels.txt's ResistPenalty (0, -40, -100; a member typed unsigned, hence the cast)
//  and classic hard-codes -20 and -50. Floored at -100; at or above the cap the cap is printed, in colour 4. The difficulty's record is fetched
//  in classic too, and not read there.
//  The draw: measured in font 0; wider than a non-zero nMaxWidth and the one draw happens in font 6, a pixel lower. The text height is written by
//  D2Win_10131_GetTextDimensions and never read.
void __fastcall INV_MercInvDrawStatRows(D2UnitStrc *pMerc)
{
    Font nPrevFont;
    uint32_t i;
    int32_t nColor;
    int32_t nValue;
    int32_t nBaseValue;
    int32_t nLeftX;
    int32_t nRightX;
    int32_t nRowY;
    int32_t nDrawY;
    int32_t nMaxWidth;
    int32_t nTextWidth;
    int32_t nTextHeight;
    int32_t nMaxLife;
    int32_t nLife;
    int32_t nDefense;
    int32_t nDamagePercent;
    int32_t nMinPercent;
    int32_t nMaxPercent;
    int32_t nMaxDamage;
    int32_t nMinDelta;
    int32_t nMaxDelta;
    int32_t nNormalDamage;
    int16_t nStatBonus;
    D2UnitStrc *pRightArmItem;
    int32_t nMinTotal;
    int32_t nMaxTotal;
    int32_t iElem;
    int32_t nElemMin;
    int32_t nElemMax;
    int32_t nPoisonMin;
    int32_t nPoisonMax;
    int32_t nPoisonLength;
    int32_t nPoisonCount;
    D2DifficultyLevelsTxt *pDifficultyTxt;
    int32_t nMaxResist;
    int32_t nResistCap;

    if (!pMerc)
    {
        return;
    }

    nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT8);
    for (i = 0; i < 12; ++i)
    {
        Unicode wszText[128];
        char szText[128];

        nColor = 0;
        nValue = D2COMMON_STATLIST_UnitGetStatValue(pMerc, gMercStatRows[i].nStatId, 0);
        nBaseValue = D2COMMON_STATLIST_GetUnitBaseStat(pMerc, gMercStatRows[i].nStatId, 0);
        if (nValue > nBaseValue)
        {
            nColor = 3;
        }
        else if (nValue < nBaseValue)
        {
            nColor = 1;
        }

        nLeftX = gnBorderXDifference + gMercStatRows[i].nLeft;
        nRightX = gnBorderXDifference + gMercStatRows[i].nX;
        nRowY = gMercStatRows[i].nY - gnBorderYDifference - 2;

        switch (gMercStatRows[i].nStatId)
        {
        case STAT_HITPOINTS:
            nMaxLife = D2COMMON_STATLIST_GetMaxLifeFromUnit(pMerc) >> 8;
            nLife = nMaxLife * ROSTERPETS_GetLifePercent(pMerc->dwUnitId) / 100;
            //  the floor that cannot fire
            if (!CUNIT_IsUnitDead(pMerc) && nLife < 0)
            {
                nLife = 1;
            }
            STORM_SStrPrintf(szText, 0x80u, "%d / %d", nLife, nMaxLife);
            Unicode::toUnicode(wszText, szText, 128);
            if ((D2COMMON_STATLIST_GetUnitBaseStat(pMerc, STAT_MAXHP, 0) >> 8) < nMaxLife)
            {
                nColor = 3;
            }
            break;

        case STAT_MINDAMAGE:
        {
            nNormalDamage = 0;
            pRightArmItem = NULL;
            nDamagePercent = D2COMMON_STATLIST_UnitGetStatValue(pMerc, STAT_DAMAGEPERCENT, 0);
            nMinPercent = D2COMMON_STATLIST_UnitGetStatValue(pMerc, STAT_ITEM_MINDAMAGE_PERCENT, 0);
            nMaxPercent = D2COMMON_STATLIST_UnitGetStatValue(pMerc, STAT_ITEM_MAXDAMAGE_PERCENT, 0);
            if (nMinPercent + nDamagePercent > 0)
            {
                nColor = 3;
            }
            else if (nMinPercent + nDamagePercent < 0)
            {
                nColor = 1;
            }

            if (pMerc->pInventory)
            {
                pRightArmItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pMerc->pInventory, BODYLOC_RARM);
            }
            //  nValue is the row's own stat, MINDAMAGE
            nMaxDamage = D2COMMON_STATLIST_UnitGetStatValue(pMerc, STAT_MAXDAMAGE, 0);

            if (pRightArmItem)
            {
                if (D2COMMON_ITEMS_CheckWeaponIfTwoHanded(pRightArmItem))
                {
                    nMinDelta = nValue - D2COMMON_STATLIST_UnitGetStatValue(pRightArmItem, STAT_MINDAMAGE, 0);
                    nMaxDelta = nMaxDamage - D2COMMON_STATLIST_UnitGetStatValue(pRightArmItem, STAT_MAXDAMAGE, 0);
                    nValue = D2COMMON_STATLIST_UnitGetStatValue(pMerc, STAT_SECONDARY_MINDAMAGE, 0) + nMinDelta;
                    nMaxDamage = D2COMMON_STATLIST_UnitGetStatValue(pMerc, STAT_SECONDARY_MAXDAMAGE, 0) + nMaxDelta;
                }

                nStatBonus = D2COMMON_ITEMS_GetStrengthBonus(pRightArmItem);
                if (nStatBonus)
                {
                    nDamagePercent += nStatBonus * D2COMMON_STATLIST_UnitGetStatValue(pMerc, STAT_STRENGTH, 0) / 100;
                }
                nStatBonus = D2COMMON_ITEMS_GetDexBonus(pRightArmItem);
                if (nStatBonus)
                {
                    nDamagePercent += nStatBonus * D2COMMON_STATLIST_UnitGetStatValue(pMerc, STAT_DEXTERITY, 0) / 100;
                }
                nDamagePercent += D2COMMON_SKILLS_GetWeaponMasteryBonus(pMerc, pRightArmItem, NULL, 1);
                nNormalDamage = D2COMMON_STATLIST_UnitGetItemStatOrSkillStatValue(pRightArmItem, STAT_ITEM_NORMALDAMAGE, 0);
            }
            else
            {
                //  one call, the stat chosen before it
                nDamagePercent += D2COMMON_STATLIST_UnitGetStatValue(pMerc, pMerc->dwClassId == 271 ? STAT_DEXTERITY : STAT_STRENGTH, 0);
            }

            if (nDamagePercent <= -90)
            {
                nDamagePercent = -90;
            }

            //  {min stat, max stat, colour}, stored at 0x6FAEDCF8..0x6FAEDD8C
            int32_t anElementalStats[12] = {
                STAT_FIREMINDAM,  STAT_FIREMAXDAM,  1,
                STAT_LIGHTMINDAM, STAT_LIGHTMAXDAM, 9,
                STAT_MAGICMINDAM, STAT_MAGICMAXDAM, 3,
                STAT_COLDMINDAM,  STAT_COLDMAXDAM,  3,
            };

            nMinTotal = nNormalDamage + nValue * (nMinPercent + nDamagePercent + 100) / 100;
            nMaxTotal = nNormalDamage + nMaxDamage * (nMaxPercent + nDamagePercent + 100) / 100;
            for (iElem = 0; iElem < 12; iElem += 3)
            {
                nElemMin = D2COMMON_STATLIST_UnitGetStatValue(pMerc, (D2C_ItemStats)anElementalStats[iElem], 0);
                nElemMax = D2COMMON_STATLIST_UnitGetStatValue(pMerc, (D2C_ItemStats)anElementalStats[iElem + 1], 0);
                if (nElemMin || nElemMax)
                {
                    nColor = anElementalStats[iElem + 2];
                }
                nMaxTotal += nElemMax;
                nMinTotal += nElemMin;
            }

            nPoisonMin = D2COMMON_STATLIST_UnitGetStatValue(pMerc, STAT_POISONMINDAM, 0);
            nPoisonMax = D2COMMON_STATLIST_UnitGetStatValue(pMerc, STAT_POISONMAXDAM, 0);
            if (nPoisonMin || nPoisonMax)
            {
                nPoisonLength = D2COMMON_STATLIST_UnitGetStatValue(pMerc, STAT_SKILL_POISON_OVERRIDE_LENGTH, 0);
                if (nPoisonLength <= 0)
                {
                    nPoisonCount = D2COMMON_STATLIST_UnitGetStatValue(pMerc, STAT_POISON_COUNT, 0);
                    if (nPoisonCount < 1)
                    {
                        nPoisonCount = 1;
                    }
                    nPoisonLength = D2COMMON_STATLIST_UnitGetStatValue(pMerc, STAT_POISONLENGTH, 0) / nPoisonCount;
                }
                nMinTotal += (nPoisonMin * nPoisonLength) >> 8;
                nColor = 2;
                nMaxTotal += (nPoisonMax * nPoisonLength) >> 8;
            }

            STORM_SStrPrintf(szText, 0x80u, "%d-%d", nMinTotal, nMaxTotal);
            Unicode::toUnicode(wszText, szText, 128);
            break;
        }

        case STAT_ARMORCLASS:
            nDefense = D2COMMON_UNITS_GetDefense(pMerc);
            if (D2COMMON_STATES_CheckStateMaskArmBlueOnUnit(pMerc))
            {
                nColor = 3;
            }
            D2LANG_STR_GroupIntDigits(wszText, nDefense, 128);
            break;

        case STAT_FIRERESIST:
        case STAT_LIGHTRESIST:
        case STAT_COLDRESIST:
        case STAT_POISONRESIST:
            pDifficultyTxt = D2COMMON_DATATBLS_GetDifficultyLevelsTxtRecord(GAME_GetDifficulty());
            nMaxResist = 0;
            switch (gMercStatRows[i].nStatId)
            {
            case STAT_FIRERESIST:
                if (D2COMMON_STATES_CheckStateMaskRFBlueOnUnit(pMerc))
                {
                    nColor = 3;
                }
                if (D2COMMON_STATES_CheckStateMaskRFRedOnUnit(pMerc))
                {
                    nColor = 1;
                }
                nMaxResist = D2COMMON_STATLIST_UnitGetStatValue(pMerc, STAT_MAXFIRERESIST, 0);
                break;
            case STAT_LIGHTRESIST:
                if (D2COMMON_STATES_CheckStateMaskRLBlueOnUnit(pMerc))
                {
                    nColor = 3;
                }
                if (D2COMMON_STATES_CheckStateMaskRLRedOnUnit(pMerc))
                {
                    nColor = 1;
                }
                nMaxResist = D2COMMON_STATLIST_UnitGetStatValue(pMerc, STAT_MAXLIGHTRESIST, 0);
                break;
            case STAT_COLDRESIST:
                if (D2COMMON_STATES_CheckStateMaskRCBlueOnUnit(pMerc))
                {
                    nColor = 3;
                }
                if (D2COMMON_STATES_CheckStateMaskRCRedOnUnit(pMerc))
                {
                    nColor = 1;
                }
                nMaxResist = D2COMMON_STATLIST_UnitGetStatValue(pMerc, STAT_MAXCOLDRESIST, 0);
                break;
            case STAT_POISONRESIST:
                if (D2COMMON_STATES_CheckStateMaskRPBlueOnUnit(pMerc))
                {
                    nColor = 3;
                }
                if (D2COMMON_STATES_CheckStateMaskRPRedOnUnit(pMerc))
                {
                    nColor = 1;
                }
                nMaxResist = D2COMMON_STATLIST_UnitGetStatValue(pMerc, STAT_MAXPOISONRESIST, 0);
                break;
            }

            if (GAME_IsExpansionGame())
            {
                nValue += (int32_t)pDifficultyTxt->dwResistPenalty;
            }
            else
            {
                switch (GAME_GetDifficulty())
                {
                case 1:
                    nValue -= 20;
                    break;
                case 2:
                    nValue -= 50;
                    break;
                }
            }

            nResistCap = nMaxResist + 75;
            if (nResistCap >= 95)
            {
                nResistCap = 95;
            }
            if (nValue <= -100)
            {
                nValue = -100;
            }
            if (nValue >= nResistCap)
            {
                nColor = 4;
                D2LANG_STR_GroupIntDigits(wszText, nResistCap, 128);
            }
            else
            {
                if (nValue < 0)
                {
                    nColor = 1;
                }
                D2LANG_STR_GroupIntDigits(wszText, nValue, 128);
            }
            break;

        default:
            D2LANG_STR_GroupIntDigits(wszText, nValue, 128);
            break;
        }

        D2WIN_10127_SetFont(D2FONT_FONT8);
        nTextWidth = 0;
        nTextHeight = 0;
        D2WIN_10131_GetTextDimensions(wszText, &nTextWidth, &nTextHeight);
        nMaxWidth = gMercStatRows[i].nMaxWidth;
        if (nMaxWidth && nTextWidth > nMaxWidth)
        {
            D2WIN_10127_SetFont(D2FONT_FONT6);
            nDrawY = nRowY + 1;
        }
        else
        {
            nDrawY = nRowY;
        }

        if (!gMercStatRows[i].nLeft)
        {
            D2WIN_10117_DrawText(wszText, nRightX - D2WIN_10121_GetTextWidth(wszText), nDrawY, nColor, 0);
        }
        else if (!gMercStatRows[i].nX)
        {
            D2WIN_10117_DrawText(wszText, nLeftX, nDrawY, nColor, 0);
        }
        else
        {
            CHAR_DrawTextCentered(nLeftX, nDrawY, nRightX, wszText, nColor);
        }
    }
    D2WIN_10127_SetFont(nPrevFont);
}

//1.10f: D2Client.0x6FAED5D0
//  The mercenary panel. Expansion only; without a live hireling it closes itself with the same UI_SetUIToggle(UI_MERCINV, UI_TOGGLE, 0) as
//  the three handlers at the top of this file, and returns.
//  In order: the layout cache; the panel art, gpImgNpcInv in four cels, a 2x2 at x +0/+256 and y -224/-48; the close button; the hireling's
//  name in font 6, cut a character at a time from the end until it fits 160 pixels; the twelve captions; the stat rows; the empty-slot art; the
//  equipped items; the slot highlights; the item description.
//  The captions: gMercPanelLabels, twelve records of 14 bytes (int left, right and Y, then a word string id), walked by byte offset, add ebp, 0Eh
//  up to cmp ebp, 0A8h (0x6FAED8C9). Four captions hold a newline, tbl 4071 to 4074, Fire\nResistance and its siblings; CHAR_TextCutNextLine
//  cuts them in place and the halves are centred at Y - 4 and Y + 4.
//  BUG: the alignment test is on the sum: test edi, edi at 0x6FAED8A7 tests gnBorderXDifference + nRight, so at 800x600, where the border is 80,
//  the eight one-line captions whose nRight is 0 take the centred path with a right edge left of their left edge. INERT: CHAR_DrawTextCentered
//  shifts only text narrower than nRight - nLeft + 1, which a negative box never is, and draws at nLeft in colour 0, exactly the call the
//  left-aligned path makes.
//  Each equipped item is centred in its slot of gMercInvSlots.
//  BUG: the vertical term multiplies the item's height by the grid's box width (0x6FAED948 loads nGridBoxWidth once and 0x6FAED983 uses it
//  again); the player's panel uses nGridBoxHeight there (0x6FAE03DB). INERT: both Hireling records in inventory.txt are 29 by 29. The item walk
//  stops on this file's < &gnInvLayoutRes sentinel, tested at the bottom only.
//  D2ExpRes detours twice: MercenaryCloseButton at 0x6FAED73E, the lea eax, [edx+ecx-3Fh] of the button's -63, and MercenaryName at 0x6FAED7D7,
//  the mov eax, 0D6h of the name's 214.
void __cdecl INV_MercInvDrawPanel(void)
{
    uint32_t dwMercGUID;
    D2UnitStrc *pMerc;
    D2InventoryStrc *pInventory;
    D2UnitStrc *pItem;
    D2InvCompGridStrc *pSlot;
    int32_t nBodyLoc;
    Font nPrevFont;
    uint32_t nLabelOffset;
    const D2MercPanelLabelStrc *pLabel;
    int32_t nLeft;
    int32_t nRight;
    int32_t nX;
    int32_t nY;
    uint8_t nItemWidth;
    uint8_t nItemHeight;
    Unicode *pwszCut;
    Unicode *pwszSecondLine;
    D2GfxDataStrc tData;

    if (!GAME_IsExpansionGame())
    {
        return;
    }

    dwMercGUID = ROSTERPETS_GetAssociatedUnitGUID(CUNIT_GetPlayer(), 7, 0);
    if (dwMercGUID == -1 || (pMerc = CUNIT_FindUnitByGuidAndType(dwMercGUID, UNIT_MONSTER)) == NULL)
    {
        UI_SetUIToggle(UI_MERCINV, UI_TOGGLE, 0);
        return;
    }

    INV_EnsurePanelLayout();

    memset(&tData, 0, sizeof(tData));
    tData.pCellFile = gpImgNpcInv;
    tData.nDirection = 0;
    tData.nFrame = 0;
    D2GFX_TEXTURE_CelDraw(&tData, gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
    tData.nFrame = 1;
    D2GFX_TEXTURE_CelDraw(&tData, gnBorderXDifference + 256, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
    tData.nFrame = 2;
    D2GFX_TEXTURE_CelDraw(&tData, gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
    tData.nFrame = 3;
    D2GFX_TEXTURE_CelDraw(&tData, gnBorderXDifference + 256, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);

    tData.pCellFile = UI_GetBuySellButtonImage();
    tData.nFrame = (gbMercCloseBtnPressed != 0) + 10;
    //  D2ExpRes: MercenaryCloseButton [Mercenary] detours at 0x6FAED73E
    D2GFX_TEXTURE_CelDraw(&tData, gnBorderXDifference + 272, gnBorderYDifference + gnScreenHeight - 63, 0xFFFFFFFF, 5, NULL);

    Unicode wszName[32];

    nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT6);
    Unicode::strncpy(wszName, CUNIT_GetUnitName(pMerc), 32);
    wszName[31].ch = 0;
    pwszCut = &wszName[31];
    while (D2WIN_10121_GetTextWidth(wszName) > 160)
    {
        --pwszCut;
        pwszCut->ch = 0;
    }
    //  D2ExpRes: MercenaryName [Mercenary] detours at 0x6FAED7D7
    D2WIN_10117_DrawText(wszName, gnBorderXDifference + 15, 214 - gnBorderYDifference, 0, 0);
    D2WIN_10127_SetFont(nPrevFont);

    nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT6);
    for (nLabelOffset = 0; nLabelOffset < 168; nLabelOffset += 14)
    {
        Unicode wszText[100];

        //  the bytes walk the byte offset (add ebp, 0Eh / cmp ebp, 0A8h) and read each field at it; ours indexes a record of gMercPanelLabels
        pLabel = (const D2MercPanelLabelStrc *)((const char *)gMercPanelLabels + nLabelOffset);
        Unicode::strncpy(wszText, D2LANG_GetStringFromTblIndex(pLabel->nStringId), 99);
        nLeft = pLabel->nLeft;
        nRight = pLabel->nRight;
        wszText[99].ch = 0;
        nY = pLabel->nY - gnBorderYDifference;
        nLeft += gnBorderXDifference;
        nRight += gnBorderXDifference;

        if (CHAR_TextCutNextLine(wszText, &pwszSecondLine))
        {
            CHAR_DrawTextCentered(nLeft, nY - 4, nRight, wszText, 0);
            CHAR_DrawTextCentered(nLeft, nY + 4, nRight, pwszSecondLine, 0);
        }
        else if (nRight)
        {
            //  the sum, border included
            CHAR_DrawTextCentered(nLeft, nY, nRight, wszText, 0);
        }
        else
        {
            D2WIN_10117_DrawText(wszText, nLeft, nY, 0, 0);
        }
    }
    D2WIN_10127_SetFont(nPrevFont);

    INV_MercInvDrawStatRows(pMerc);
    INV_MercInvDrawEmptySlots(pMerc);

    pInventory = pMerc->pInventory;
    if (pInventory)
    {
        pSlot = gMercInvSlots;
        nBodyLoc = BODYLOC_NONE;
        do
        {
            pItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, (D2C_PlayerBodyLocs)nBodyLoc);
            if (pItem)
            {
                D2COMMON_ITEMS_GetDimensions(pItem, &nItemWidth, &nItemHeight, szD2CommonCmnItemsH, 1754);
                nX = pSlot->rect.nLeft + (pSlot->nWidth - nItemWidth * gInvMercGrid.nGridBoxWidth) / 2;
                //  the box width again
                nY = pSlot->rect.nTop + (pSlot->nHeight - nItemHeight * gInvMercGrid.nGridBoxWidth) / 2;
                GFX_DrawInventoryItem(pItem, nX, nY);

                if (gpHoverItem == pItem && D2COMMON_ITEMS_CheckIfSocketable(pItem))
                {
                    if (D2COMMON_ITEMS_GetMaxSockets(pItem) > 0 && D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_SOCKETED, 12430, szInvCpp))
                    {
                        INV_DrawSocketedItems(pItem, &gInvMercGrid, nX, nY);
                    }
                }
            }
            ++pSlot;
            ++nBodyLoc;
        }
        while ((int32_t)pSlot < (int32_t)(gMercInvSlots + 11));   // 0x6FBB5B14, one past the array (`cmp` 0x6FAED9ED, `jl` 0x6FAED9FB)
    }

    INV_MercInvDrawSlotHighlights(pMerc);
    INV_DrawItemDescription(pMerc, FALSE);
}

//1.10f: D2Client.0x6FAE97C0
//  The gold, for three panels. The selector nPanelKind, from the three call sites in INV_UpdateAndDrawPanels, and the stat each arm reads:
//      0  the stash      STAT_GOLDBANK at gnBorderXDifference + 165, the withdraw coin and its hover text         (0x6FADF40E)
//      1  the inventory  STAT_GOLD drawn from gnScreenWidth - gnBorderXDifference - 212, the drop coin             (0x6FAE092E)
//      2  the shop       STAT_GOLDBANK right-aligned at gnBorderXDifference + 198, tbl 3315 "Stash" at + 21        (0x6FADEE52)
//      3  --             the caption alone; no caller passes it
//  The selector does not follow D2C_ItemInvPage: INVPAGE_INVENTORY would be the stash's arm.
//  The stash's withdraw hover is a 20 by 18 box above the coin's anchor, in the expansion at gnScreenHeight - 438 and in classic at - 242; both
//  editions have it, and the coin. The amount is plain "%d" into 20 bytes, widened with win2Unicode's 19.
//  A sixty-element Unicode buffer is constructed at the top and never read. The count is the loop's (mov edi, 3Ch, 0x6FAE97E5) and its slot is
//  the frame's highest local, 0x78 bytes below the top, so sixty is the whole of it; declared, for the sixty constructor calls.
void __fastcall INV_DrawGoldAmount(int32_t nPanelKind)
{
    Font nPrevFont;
    int32_t nGold;
    int32_t nX;
    int32_t nY;
    int32_t nCoinX;
    int32_t nCoinY;
    int32_t nCoinFrame;
    int32_t nHotX;
    int32_t nHotY;
    int32_t nMouseX;
    int32_t nMouseY;
    D2GfxDataStrc tData;

    nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT16);

    Unicode wszUnused[60];

    memset(&tData, 0, sizeof(tData));
    tData.nDirection = 0;

    switch (nPanelKind)
    {
    case 0:
        nGold = D2COMMON_STATLIST_UnitGetStatValue(CUNIT_GetPlayer(), STAT_GOLDBANK, 0);
        //  D2ExpRes: StashGoldCount2 [Stash] detours at 0x6FAE98E9
        nX = gnBorderXDifference + 165;
        if (GAME_IsExpansionGame())
        {
            //  D2ExpRes: StashGoldCount1 and StashWithdrawButton1,
            //  0x6FAE990B and 0x6FAE9912
            nY = gnBorderYDifference + gnScreenHeight - 440;
            nCoinY = gnBorderYDifference + gnScreenHeight - 438;
        }
        else
        {
            nY = gnBorderYDifference + gnScreenHeight - 244;
            nCoinY = gnBorderYDifference + gnScreenHeight - 242;
        }
        //  D2ExpRes: StashWithdrawButton2 [Stash] detours at 0x6FAE9939
        nCoinX = gnBorderXDifference + 73;
        nCoinFrame = gbWithdrawGoldBtnPressed != 0;

        if (GAME_IsExpansionGame())
        {
            //  D2ExpRes: StashWithdrawButtonMouseHoverOver [Stash], 0x6FAE996D
            nHotY = gnBorderYDifference + gnScreenHeight - 438;
            nHotX = gnBorderXDifference + 73;
            nMouseX = CURSOR_GetMouseX();
            nMouseY = CURSOR_GetMouseY();
            if (nMouseX >= nHotX && nMouseX <= nHotX + 20 && nMouseY >= nHotY - 18 && nMouseY <= nHotY)
            {
                //  D2ExpRes: StashWithdrawString [Stash], 0x6FAE99A8
                D2WIN_10129_DrawFramedText(D2LANG_GetStringFromTblIndex(STR_IDX_4124_strGoldWithdraw), gnBorderXDifference + 53, gnBorderYDifference + gnScreenHeight - 458, 0, 0);
            }
        }
        else
        {
            //  the classic rectangle, 0x6FAE9A9D
            nHotY = gnBorderYDifference + gnScreenHeight - 242;
            nHotX = gnBorderXDifference + 73;
            nMouseX = CURSOR_GetMouseX();
            nMouseY = CURSOR_GetMouseY();
            if (nMouseX >= nHotX && nMouseX <= nHotX + 20 && nMouseY >= nHotY - 18 && nMouseY <= nHotY)
            {
                D2WIN_10129_DrawFramedText(D2LANG_GetStringFromTblIndex(STR_IDX_4124_strGoldWithdraw), gnBorderXDifference + 53, gnBorderYDifference + gnScreenHeight - 262, 0, 0);
            }
        }
        break;

    case 1:
        nGold = D2COMMON_STATLIST_UnitGetStatValue(CUNIT_GetPlayer(), STAT_GOLD, 0);
        //  D2ExpRes: InventoryDepositDropTradeGoldButtonAndGoldCount
        //  [Inventory] detours at 0x6FAE989C
        nX = gnScreenWidth - gnBorderXDifference - 212;
        nCoinX = gnScreenWidth - gnBorderXDifference - 237;
        nY = gnBorderYDifference + gnScreenHeight - 72;
        nCoinY = gnBorderYDifference + gnScreenHeight - 69;
        nCoinFrame = gbDropGoldBtnPressed != 0;
        break;

    default:
        nGold = D2COMMON_STATLIST_UnitGetStatValue(CUNIT_GetPlayer(), STAT_GOLDBANK, 0);
        nX = D2WIN_10121_GetTextWidth(D2LANG_GetStringFromTblIndex(STR_IDX_3315_stash)) + gnBorderXDifference + 21;
        nCoinY = 0;
        //  D2ExpRes: BuySellStashStringGoldCountY [BuySell] detours at 0x6FAE9856
        nY = gnBorderYDifference + gnScreenHeight - 106;
        nCoinX = 0;
        nCoinFrame = 0;
        break;
    }

    if (nPanelKind != 3)
    {
        Unicode wszGold[20];
        char szGold[20];

        STORM_SStrPrintf(szGold, 0x14u, "%d", nGold);
        Unicode::win2Unicode(wszGold, szGold, 19);
        if (nPanelKind == 2)
        {
            //  D2ExpRes: BuySellGoldCountX [BuySell] detours at 0x6FAE9A2D
            nX = gnBorderXDifference - D2WIN_10121_GetTextWidth(wszGold) + 198;
        }
        D2WIN_10117_DrawText(wszGold, nX, nY, 0, 0);
    }

    switch (nPanelKind)
    {
    case 0:
        if (gbWithdrawGoldBtnPressed)
        {
            ++nCoinY;
        }
        nCoinY -= 2;
        nCoinX += 2;
        break;

    case 1:
        if (gbDropGoldBtnPressed)
        {
            ++nCoinY;
        }
        nCoinY -= 2;
        nCoinX += 1;
        break;

    default:
        //  D2ExpRes: BuySellStashStringX [BuySell] detours at 0x6FAE9A73
        D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_3315_stash), gnBorderXDifference + 21, nY, 0, 0);
        D2WIN_10127_SetFont(nPrevFont);
        return;
    }

    tData.nFrame = nCoinFrame;
    tData.pCellFile = gpImgGoldCoinButton;
    D2GFX_TEXTURE_CelDraw(&tData, nCoinX, nCoinY, 0xFFFFFFFF, 5, NULL);
    D2WIN_10127_SetFont(nPrevFont);
}


//1.10f: D2Client.0x6FADEAC0
//  The panels' draw pass, the last function of the unit. Called twice from UIPANELS_DrawInGamePanels, once for the seven window ids that share
//  the inventory draw and once more after the trade window's button hints, so with trade open everything below runs twice in a frame.
//  It draws every inventory-side panel (the vendor's goods, the stash, the cube, the trade window and the character's own equipment) and reads
//  the mouse for the four hover hints that go with them. D2ExpRes detours thirty-five sites in here; each one is noted beside the instruction it
//  lands on. The idioms that later versions have as helper functions are written out where the bytes have them.
//  sgInventoryMode is read at the top and again after the vendor block, because that block can set it to 1: 10 returns at once, drawing nothing;
//  1..9 the vendor's left panel, its tabs and the shop's gold; 12 and 13 the stash; 14 the cube (this arm can return, closing the panel); 11 the
//  trade window, after the character panel's art. Everything below the mode blocks (the character panel, the empty-slot art, the equipped items,
//  the inventory grid, the close button and the item description) runs in every mode but 10.
//  BUG: the second pass over the weapon slots cannot draw. The empty weapon placeholders are drawn twice over, and the second pass
//  (0x6FADFA89..0x6FADFB98, the shape INV_MercInvDrawEmptySlots has) guards every one of its draws with a fresh INVENTORY_GetItemFromBodyLoc whose
//  answer the pass has already read into a local: pLArm && !Get(LARM) and its three siblings are contradictions, and nothing between the two
//  reads touches the inventory. UNREACHABLE: no draw of the second pass can happen; it is reproduced whole.
//  DIFFERENCE: four GFX_DrawFilledRect calls take their colour from seven stores of the shape mov dl, <byte> / mov al, <byte> into a register
//  the caller has not set, so the dword argument carries 24 bits of whatever was in it. Three of the seven sit over a register holding a small
//  number (the cursor state at 0x6FAE047A and 0x6FAE0499, a zero at 0x6FAE0590) and their top bits are 0; the rest (0x6FAE0380, 0x6FAE05BB,
//  0x6FAE070F, 0x6FAE0747) sit over what a call left behind. Ours writes the byte and nothing else, because reading an uninitialised value is
//  undefined in C. NOT ESTABLISHED: what the renderer does with the upper bits (the value goes out through D2Gfx into the renderer's vtable).
//  gnHoverBodyLoc is a dword: all eight accesses (four here, four in INV_OnMouseMove) are mov reg, [...] / mov [...], reg.
void __cdecl INV_UpdateAndDrawPanels(void)
{
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nInventoryMode;
    D2UnitGUID nNpcGUID;
    D2UnitStrc *pNpc;
    D2UnitStrc *pPlayer;
    D2UnitStrc *pItem;
    D2UnitStrc *pOtherArmItem;
    D2UnitStrc *pRightArm;
    D2UnitStrc *pLeftArm;
    D2UnitStrc *pCursorItem;
    D2UnitStrc *pDescUnit;
    D2UnitStrc *pActiveNpc;
    D2InventoryStrc *pInventory;
    D2InvCompGridStrc *pHoverRect;
    Font nPrevFont;
    int32_t iShopTab;
    int32_t iVendorTab;
    int32_t nTabX;
    int32_t nColor;
    uint32_t nStashGoldLimit;
    const Unicode *pwszText;
    int32_t nTextWidth;
    int32_t nNameLen;
    Unicode *pwszCut;
    int32_t nX;
    int32_t nY;
    int32_t nHotX;
    int32_t nHotY;
    int32_t nPanelX;
    int32_t nPanelY;
    int32_t nSwapOffsetX;
    int32_t nSwapOffsetY;
    int32_t nBodyLoc;
    BOOL bTwoHanded;
    BOOL bUseHireling;
    BOOL bDrawFrame;
    BOOL bHoverGreen;
    uint32_t dwTick;
    uint32_t dwColor;
    uint8_t nItemWidth;
    uint8_t nItemHeight;
    uint8_t nCursorWidth;
    uint8_t nCursorHeight;
    uint8_t nVendorPageSaved;
    int32_t nRepairCost;
    D2BitBufferStrc *pQuestFlags;
    D2GfxDataStrc tData;
    D2GfxDataStrc tPanel;

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    nInventoryMode = sgInventoryMode;
    if (nInventoryMode == 10)
    {
        return;
    }

    if (nInventoryMode >= 1 && nInventoryMode <= 9)
    {
        //  the vendor's left panel
        if (GetTickCount() > gnVendorPageHoldUntil)
        {
            gnVendorPageHoldUntil = 0;
        }
        if (gbVendorPageRebuildPending == 1 && !gnVendorPageHoldUntil && NPCMENU_GetInteractingNpc(&nNpcGUID))
        {
            INV_GetVendorPageInventory(gnVendorPage);
            gbVendorPageRebuildPending = 0;
        }

        if (gbDeselectShopTabsPending == 1)
        {
            //  the clearing loop of ITEMPANEL_ClearShopTabs
            ITEMPANEL_ClearShopTabs();
            sgInventoryMode = 1;
            CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(CUNIT_GetPlayer()->pInventory));
            gbDeselectShopTabsPending = 0;
        }

        memset(&tData, 0, sizeof(tData));
        tData.pCellFile = gNpcBuySellImage;
        tData.nDirection = 0;
        tData.nFrame = 0;
        D2GFX_TEXTURE_CelDraw(&tData, gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
        tData.nFrame = 1;
        D2GFX_TEXTURE_CelDraw(&tData, gnBorderXDifference + 256, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
        tData.nFrame = 2;
        D2GFX_TEXTURE_CelDraw(&tData, gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
        tData.nFrame = 3;
        D2GFX_TEXTURE_CelDraw(&tData, gnBorderXDifference + 256, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);

        if (NPCMENU_GetInteractingNpc(&nNpcGUID))
        {
            pNpc = CUNIT_FindUnitByGuidAndType(nNpcGUID, UNIT_MONSTER);
            if (pNpc)
            {
                INV_DrawInventoryGridItems(pNpc);
            }
        }

        //  the shop's tab buttons
        tData.pCellFile = UI_GetBuySellButtonImage();
        iShopTab = 0;
        if (gnNpcShopTabCount > 0)
        {
            do
            {
                //  pressed + the record's base frame (the bytes' cursor is &[i].bPressed and reads the word ten bytes in, 0x6FADECC4);
                //  the count stops it, 4 or less, so the record is read by index
                tData.nFrame = gNpcShopTabs[iShopTab].bPressed + gNpcShopTabs[iShopTab].nBaseFrame;
                //  D2ExpRes: BuySellButtonsY at 0x6FADECE2, BuySellButtonsDX
                //  at 0x6FADED06
                D2GFX_TEXTURE_CelDraw(&tData, gatButtonsOffsetX[gnNpcShopCloseTab][iShopTab] + gnBorderXDifference - 1, gnBorderYDifference + gnScreenHeight - 63, 0xFFFFFFFF, 5, NULL);
                ++iShopTab;
            }
            while (iShopTab < gnNpcShopTabCount);
        }

        //  the vendor's side tabs, 18-byte records
        nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT16);
        iVendorTab = 0;
        if (gnVendorTabCount > 0)
        {
            nTabX = 0;
            //  the bytes' cursor is &[i].bActive (set at 0x6FADED4C, stepped by `add esi, 12h` at 0x6FADEE31) and
            //  reads the record at -14, -10, -6 and -4 from it; the count stops it,
            //  so the record is read by the index
            do
            {
                nColor = 0;
                if (gVendorTabs[iVendorTab].bSelected)
                {
                    nColor = 4;
                }
                if (gVendorTabs[iVendorTab].bActive)
                {
                    pwszText = D2LANG_GetStringFromTblIndex(gVendorTabs[iVendorTab].nStringIndex);
                    nTextWidth = D2WIN_10121_GetTextWidth(pwszText);
                    tData.pCellFile = gpImgBuySellTabs;
                    //  two tests, not a ternary: the colour is 0 or 4
                    if (nColor == 4)
                    {
                        tData.nFrame = iVendorTab;
                    }
                    else if (!nColor)
                    {
                        tData.nFrame = iVendorTab + 4;
                    }
                    //  D2ExpRes: BuySellTabs1 at 0x6FADEDBD
                    D2GFX_TEXTURE_CelDraw(&tData, gnBorderXDifference + nTabX, gnBorderYDifference + gnScreenHeight - 449, 0xFFFFFFFF, 5, NULL);
                    if (!NPCMENU_IsVendorGamble())
                    {
                        D2WIN_10117_DrawText(pwszText, gVendorTabs[iVendorTab].nX - nTextWidth / 2 + gnBorderXDifference, gVendorTabs[iVendorTab].nY + gnBorderYDifference + gnScreenHeight - 480, nColor, 0);
                    }
                }
                ++iVendorTab;
                //  D2ExpRes: BuySellTabs2 at 0x6FADEE2E
                nTabX += 80;
            }
            while (iVendorTab < gnVendorTabCount);
        }
        D2WIN_10127_SetFont(nPrevFont);
        INV_DrawGoldAmount(2);

        //  the shop-tab block above can have set it to 1
        nInventoryMode = sgInventoryMode;
    }

    memset(&tPanel, 0, sizeof(tPanel));
    tPanel.nDirection = 0;

    if (nInventoryMode >= 12)
    {
        if (nInventoryMode <= 13)
        {
            //  the stash
            if (gpStashPanelImage)
            {
                if (GAME_IsExpansionGame())
                {
                    tPanel.pCellFile = gpTradeStashImage;
                }
                else
                {
                    tPanel.pCellFile = gpStashPanelImage;
                }
                tPanel.nFrame = 0;
                D2GFX_TEXTURE_CelDraw(&tPanel, gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
                tPanel.nFrame = 1;
                D2GFX_TEXTURE_CelDraw(&tPanel, gnBorderXDifference + 256, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
                tPanel.nFrame = 2;
                D2GFX_TEXTURE_CelDraw(&tPanel, gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
                tPanel.nFrame = 3;
                D2GFX_TEXTURE_CelDraw(&tPanel, gnBorderXDifference + 256, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
            }

            nStashGoldLimit = D2COMMON_UNITS_GetStashGoldLimit(CUNIT_GetPlayer());

            Unicode wszText[100];

            pwszText = D2LANG_GetStringFromTblIndex(STR_IDX_4051_GoldMax);
            Unicode::sprintf(100, wszText, pwszText, nStashGoldLimit);
            //  D2ExpRes: StashMaxGoldString at 0x6FADF333
            if (GAME_IsExpansionGame())
            {
                D2WIN_10117_DrawText(wszText, gnBorderXDifference + 78, gnBorderYDifference + gnScreenHeight - 415, 0, 0);
            }
            else
            {
                D2WIN_10117_DrawText(wszText, gnBorderXDifference + 78, gnBorderYDifference + gnScreenHeight - 219, 0, 0);
            }

            nVendorPageSaved = gnVendorPage;
            gnVendorPage = 4;
            INV_DrawInventoryGridItems(CUNIT_GetPlayer());
            gnVendorPage = nVendorPageSaved;

            tPanel.pCellFile = UI_GetBuySellButtonImage();
            tPanel.nFrame = gbStashCloseBtnPressed + 10;
            //  D2ExpRes: StashCloseButton1 at 0x6FADF3CE, StashCloseButton2
            //  at 0x6FADF3F4
            if (GAME_IsExpansionGame())
            {
                nPanelY = gnBorderYDifference + gnScreenHeight - 64;
            }
            else
            {
                nPanelY = gnBorderYDifference + gnScreenHeight - 65;
            }
            if (GAME_IsExpansionGame())
            {
                nPanelX = gnBorderXDifference + 272;
            }
            else
            {
                nPanelX = gnBorderXDifference + 275;
            }
            D2GFX_TEXTURE_CelDraw(&tPanel, nPanelX, nPanelY, 0xFFFFFFFF, 5, NULL);

            INV_DrawGoldAmount(0);

            //  the close button's hint -- the mouse is read again here
            nHotX = CURSOR_GetMouseX();
            nHotY = CURSOR_GetMouseY();
            //  D2ExpRes: StashCloseString1..6, 0x6FADF43E to 0x6FADF524
            if (GAME_IsExpansionGame())
            {
                nX = gnBorderXDifference + 272;
            }
            else
            {
                nX = gnBorderXDifference + 275;
            }
            if (nHotX >= nX)
            {
                nX = GAME_IsExpansionGame() ? gnBorderXDifference + 272 : gnBorderXDifference + 275;
                if (nHotX <= nX + 40)
                {
                    nY = GAME_IsExpansionGame() ? gnBorderYDifference + gnScreenHeight - 64 : gnBorderYDifference + gnScreenHeight - 65;
                    if (nHotY >= nY - 35)
                    {
                        nY = GAME_IsExpansionGame() ? gnBorderYDifference + gnScreenHeight - 64 : gnBorderYDifference + gnScreenHeight - 65;
                        if (nHotY <= nY + 5)
                        {
                            pwszText = D2LANG_GetStringFromTblIndex(STR_IDX_4144_strClose);
                            if (GAME_IsExpansionGame())
                            {
                                nPanelY = gnBorderYDifference + gnScreenHeight - 64;
                            }
                            else
                            {
                                nPanelY = gnBorderYDifference + gnScreenHeight - 65;
                            }
                            if (GAME_IsExpansionGame())
                            {
                                nPanelX = gnBorderXDifference + 272;
                            }
                            else
                            {
                                nPanelX = gnBorderXDifference + 275;
                            }
                            D2WIN_10129_DrawFramedText(pwszText, nPanelX - D2WIN_10121_GetTextWidth(pwszText) / 2 + 12, nPanelY - 35, 0, 0);
                        }
                    }
                }
            }
        }
        else if (nInventoryMode == 14)
        {
            //  the cube
            if (GAME_IsDisconnecting() || CUNIT_IsPlayerDeadOrMissing())
            {
                UI_SetUIToggle(UI_CUBE, UI_TURNOFF, 0);
                MSG_SendCmd3Words(79, 23, 0, 0);
                return;
            }

            if (gpImgCubePanel)
            {
                tPanel.pCellFile = gpImgCubePanel;
                tPanel.nFrame = 0;
                D2GFX_TEXTURE_CelDraw(&tPanel, gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
                tPanel.nFrame = 1;
                D2GFX_TEXTURE_CelDraw(&tPanel, gnBorderXDifference + 256, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
                tPanel.nFrame = 2;
                D2GFX_TEXTURE_CelDraw(&tPanel, gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
                tPanel.nFrame = 3;
                D2GFX_TEXTURE_CelDraw(&tPanel, gnBorderXDifference + 256, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
            }

            nVendorPageSaved = gnVendorPage;
            if (!gbHoradricAnimActive || (uint32_t)gnHoradricAnimFrame >= 14)
            {
                gnVendorPage = 3;
                INV_DrawInventoryGridItems(CUNIT_GetPlayer());
                gnVendorPage = nVendorPageSaved;
            }

            tPanel.pCellFile = UI_GetBuySellButtonImage();
            tPanel.nFrame = gbCubeCloseBtnPressed + 10;
            //  D2ExpRes: CubeCloseButton at 0x6FADEFDC
            D2GFX_TEXTURE_CelDraw(&tPanel, gnBorderXDifference + 275, gnBorderYDifference + gnScreenHeight - 65, 0xFFFFFFFF, 5, NULL);

            tPanel.pCellFile = gpImgMiniConvert;
            tPanel.nFrame = gbCubeTransmuteBtnPressed;
            //  D2ExpRes: CubeTransmuteButton at 0x6FADF01D
            D2GFX_TEXTURE_CelDraw(&tPanel, gnBorderXDifference + 144, gnBorderYDifference + gnScreenHeight - 188, 0xFFFFFFFF, 5, NULL);

            if (gbHoradricAnimActive)
            {
                //  one GetTickCount, as the bytes have it
                dwTick = GetTickCount();
                bDrawFrame = TRUE;
                if (dwTick - gdwHoradricAnimLastTick > 70)
                {
                    gdwHoradricAnimLastTick = dwTick;
                    ++gnHoradricAnimFrame;
                    bDrawFrame = (uint32_t)gnHoradricAnimFrame < 30;
                }
                if (bDrawFrame)
                {
                    tPanel.nFrame = gnHoradricAnimFrame;
                    tPanel.pCellFile = gpHoradricAnimImage;
                    D2GFX_TEXTURE_CelDraw(&tPanel, gnScreenWidth / 2, gnScreenHeight / 2 - 1, 0xFFFFFFFF, 3, NULL);
                }
                else
                {
                    gbHoradricAnimActive = 0;
                    if (gpHoradricAnimImage)
                    {
                        ARCHIVE_CellFileFree(gpHoradricAnimImage);
                        gpHoradricAnimImage = NULL;
                    }
                }
            }

            D2WIN_10127_SetFont(D2FONT_FONT16);
            //  D2ExpRes: CubeCloseButtonMouseHoverOver at 0x6FADF0E0,
            //  CubeCloseStringY at 0x6FADF122, CubeCloseStringX at 0x6FADF13E
            if (nMouseX > gnBorderXDifference + 275 && nMouseX < gnBorderXDifference + 315 && nMouseY > gnBorderYDifference + gnScreenHeight - 100 && nMouseY < gnBorderYDifference + gnScreenHeight - 60)
            {
                pwszText = D2LANG_GetStringFromTblIndex(STR_IDX_4144_strClose);
                nY = gnBorderYDifference + gnScreenHeight - 100;
                nTextWidth = D2WIN_10121_GetTextWidth(pwszText);
                D2WIN_10129_DrawFramedText(pwszText, gnBorderXDifference - nTextWidth / 2 + 289, nY, 0, 0);
            }
            //  D2ExpRes: CubeTransmuteButtonMouseHoverOver at 0x6FADF149,
            //  CubeTransmuteStringY at 0x6FADF19D, CubeTransmuteStringX at
            //  0x6FADF1BD
            else if (nMouseX > gnBorderXDifference + 144 && nMouseX < gnBorderXDifference + 184 && nMouseY > gnBorderYDifference + gnScreenHeight - 223 && nMouseY < gnBorderYDifference + gnScreenHeight - 183)
            {
                pwszText = D2LANG_GetStringFromTblIndex(STR_IDX_3341_strUiMenu2);
                nY = gnBorderYDifference + gnScreenHeight - 223;
                nTextWidth = D2WIN_10121_GetTextWidth(pwszText);
                D2WIN_10129_DrawFramedText(pwszText, gnBorderXDifference - nTextWidth / 2 + 158, nY, 0, 0);
            }
        }
    }

    //  the character panel's own art, right-anchored
    tPanel.pCellFile = gpImgInvCharPanel;
    tPanel.nFrame = 4;
    D2GFX_TEXTURE_CelDraw(&tPanel, gnScreenWidth - gnBorderXDifference - 320, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
    tPanel.nFrame = 5;
    D2GFX_TEXTURE_CelDraw(&tPanel, gnScreenWidth - gnBorderXDifference - 64, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
    tPanel.nFrame = 6;
    D2GFX_TEXTURE_CelDraw(&tPanel, gnScreenWidth - gnBorderXDifference - 320, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
    tPanel.nFrame = 7;
    D2GFX_TEXTURE_CelDraw(&tPanel, gnScreenWidth - gnBorderXDifference - 64, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);

    INV_EnsurePanelLayout();

    //  the swap-set tabs, expansion only and only with the second set up
    if (gnWeaponSwapSet && FOG_IsExpansion() && GAME_IsExpansionGame())
    {
        tPanel.pCellFile = gpImgInvChar6TabCube;
        tPanel.nFrame = 0;
        D2GFX_TEXTURE_CelDraw(&tPanel, gInvBodyLocRects[BODYLOC_LARM].rect.nLeft + gnInvLArmSwapTabOffsetX, gInvBodyLocRects[BODYLOC_LARM].rect.nBottom + gnInvLArmSwapTabOffsetY, 0xFFFFFFFF, 5, NULL);
        tPanel.pCellFile = gpImgInvChar6TabCube;
        tPanel.nFrame = 1;
        D2GFX_TEXTURE_CelDraw(&tPanel, gInvBodyLocRects[BODYLOC_RARM].rect.nLeft + gnInvRArmSwapTabOffsetX, gInvBodyLocRects[BODYLOC_RARM].rect.nBottom + gnInvRArmSwapTabOffsetY, 0xFFFFFFFF, 5, NULL);
    }

    //  the empty-slot art
    pPlayer = CUNIT_GetPlayer();
    pInventory = pPlayer->pInventory;
    nSwapOffsetX = 0;

    if (!D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_TORSO))
    {
        tPanel.pCellFile = gpImgInvArmor;
        tPanel.nFrame = 0;
        D2GFX_TEXTURE_CelDraw(&tPanel, gInvBodyLocRects[3].rect.nLeft + gnInvTorsoEmptyOffsetX, gInvBodyLocRects[3].rect.nBottom + gnInvTorsoEmptyOffsetY, 0xFFFFFFFF, 5, NULL);
    }
    if (!D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_BELT))
    {
        tPanel.pCellFile = gpImgInvBelt;
        tPanel.nFrame = 0;
        D2GFX_TEXTURE_CelDraw(&tPanel, gInvBodyLocRects[BODYLOC_BELT].rect.nLeft + gnInvBeltEmptyOffsetX, gInvBodyLocRects[BODYLOC_BELT].rect.nBottom + gnInvBeltEmptyOffsetY, 0xFFFFFFFF, 5, NULL);
    }
    if (!D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_FEET))
    {
        tPanel.pCellFile = gpImgInvBoots;
        tPanel.nFrame = 0;
        D2GFX_TEXTURE_CelDraw(&tPanel, gInvBodyLocRects[BODYLOC_FEET].rect.nLeft + gnInvFeetEmptyOffsetX, gInvBodyLocRects[BODYLOC_FEET].rect.nBottom + gnInvFeetEmptyOffsetY, 0xFFFFFFFF, 5, NULL);
    }
    if (!D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_HEAD))
    {
        tPanel.pCellFile = gpImgInvHelmGlove;
        tPanel.nFrame = 1;
        D2GFX_TEXTURE_CelDraw(&tPanel, gInvBodyLocRects[1].rect.nLeft + gnInvHeadEmptyOffsetX, gInvBodyLocRects[1].rect.nBottom + gnInvHeadEmptyOffsetY, 0xFFFFFFFF, 5, NULL);
    }
    if (!D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_GLOVES))
    {
        tPanel.pCellFile = gpImgInvHelmGlove;
        tPanel.nFrame = 0;
        D2GFX_TEXTURE_CelDraw(&tPanel, gInvBodyLocRects[BODYLOC_GLOVES].rect.nLeft + gnInvGlovesEmptyOffsetX, gInvBodyLocRects[BODYLOC_GLOVES].rect.nBottom + gnInvGlovesEmptyOffsetY, 0xFFFFFFFF, 5, NULL);
    }
    if (!D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_NECK))
    {
        tPanel.pCellFile = gpImgInvRingAmulet;
        tPanel.nFrame = 0;
        D2GFX_TEXTURE_CelDraw(&tPanel, gInvBodyLocRects[2].rect.nLeft + gnInvNeckEmptyOffsetX, gInvBodyLocRects[2].rect.nBottom + gnInvNeckEmptyOffsetY, 0xFFFFFFFF, 5, NULL);
    }
    if (!D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RRIN))
    {
        tPanel.pCellFile = gpImgInvRingAmulet;
        tPanel.nFrame = 1;
        D2GFX_TEXTURE_CelDraw(&tPanel, gInvBodyLocRects[BODYLOC_RRIN].rect.nLeft + gnInvRRingEmptyOffsetX, gInvBodyLocRects[BODYLOC_RRIN].rect.nBottom + gnInvRRingEmptyOffsetY, 0xFFFFFFFF, 5, NULL);
    }
    if (!D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_LRIN))
    {
        tPanel.pCellFile = gpImgInvRingAmulet;
        tPanel.nFrame = 1;
        D2GFX_TEXTURE_CelDraw(&tPanel, gInvBodyLocRects[BODYLOC_LRIN].rect.nLeft + gnInvLRingEmptyOffsetX, gInvBodyLocRects[BODYLOC_LRIN].rect.nBottom + gnInvLRingEmptyOffsetY, 0xFFFFFFFF, 5, NULL);
    }

    //  the weapon slots, nudged when the swap set is the one in hand
    nSwapOffsetY = 0;
    if (GAME_IsExpansionGame() && gnWeaponSwapSet)
    {
        nSwapOffsetX = 4;
        nSwapOffsetY = -4;
    }
    pRightArm = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RARM);
    pLeftArm = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_LARM);
    bTwoHanded = D2COMMON_INVENTORY_GetWieldType(pPlayer, pInventory) == 2;

    if (!D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_LARM))
    {
        tPanel.nFrame = 0;
        tPanel.pCellFile = gpImgInvWeapons;
        if (!pRightArm || !bTwoHanded)
        {
            D2GFX_TEXTURE_CelDraw(&tPanel, nSwapOffsetX + gInvBodyLocRects[BODYLOC_LARM].rect.nLeft + gnInvLArmEmptyOffsetX, nSwapOffsetY + gInvBodyLocRects[BODYLOC_LARM].rect.nBottom + gnInvLArmEmptyOffsetY, 0xFFFFFFFF, 5, NULL);
        }
    }
    if (!D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RARM))
    {
        tPanel.nFrame = 0;
        tPanel.pCellFile = gpImgInvWeapons;
        if (!pLeftArm || !bTwoHanded)
        {
            D2GFX_TEXTURE_CelDraw(&tPanel, nSwapOffsetX + gInvBodyLocRects[BODYLOC_RARM].rect.nLeft + gnInvRArmEmptyOffsetX, nSwapOffsetY + gInvBodyLocRects[BODYLOC_RARM].rect.nBottom + gnInvRArmEmptyOffsetY, 0xFFFFFFFF, 5, NULL);
        }
    }

    //  and the same placeholders again, without the nudge. Every draw below is behind a re-read of a slot this block has already read, so
    //  none of them can happen (see the note at the head). Written as the bytes have it, gotos included.
    tPanel.pCellFile = gpImgInvWeapons;
    tPanel.nFrame = 0;
    if (!pRightArm)
    {
        if (pLeftArm && !D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_LARM))
        {
            D2GFX_TEXTURE_CelDraw(&tPanel, gInvBodyLocRects[BODYLOC_LARM].rect.nLeft + gnInvLArmEmptyOffsetX, gInvBodyLocRects[BODYLOC_LARM].rect.nBottom + gnInvLArmEmptyOffsetY, 0xFFFFFFFF, 5, NULL);
        }
        goto TRADE_PANEL;
    }
    if (pLeftArm)
    {
        if (bTwoHanded)
        {
            goto TRADE_PANEL;
        }
        if (!D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_LARM))
        {
            D2GFX_TEXTURE_CelDraw(&tPanel, gInvBodyLocRects[BODYLOC_LARM].rect.nLeft + gnInvLArmEmptyOffsetX, gInvBodyLocRects[BODYLOC_LARM].rect.nBottom + gnInvLArmEmptyOffsetY, 0xFFFFFFFF, 5, NULL);
        }
        if (D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RARM))
        {
            goto TRADE_PANEL;
        }
    }
    else if (D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RARM))
    {
        goto TRADE_PANEL;
    }
    D2GFX_TEXTURE_CelDraw(&tPanel, gInvBodyLocRects[BODYLOC_RARM].rect.nLeft + gnInvRArmEmptyOffsetX, gInvBodyLocRects[BODYLOC_RARM].rect.nBottom + gnInvRArmEmptyOffsetY, 0xFFFFFFFF, 5, NULL);

TRADE_PANEL:
    if (sgInventoryMode == 11 && gpImgTradePanel)
    {
        tPanel.pCellFile = gpImgTradePanel;
        tPanel.nFrame = 0;
        D2GFX_TEXTURE_CelDraw(&tPanel, gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
        tPanel.nFrame = 1;
        D2GFX_TEXTURE_CelDraw(&tPanel, gnBorderXDifference + 256, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
        tPanel.nFrame = 2;
        D2GFX_TEXTURE_CelDraw(&tPanel, gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
        tPanel.nFrame = 3;
        D2GFX_TEXTURE_CelDraw(&tPanel, gnBorderXDifference + 256, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);

        tPanel.pCellFile = UI_GetBuySellButtonImage();
        //  the partner's accept lamp -- green when they have accepted
        //  D2ExpRes: ExchangeAcceptTradeButtonGreenOpponent at 0x6FADFCB1,
        //  ExchangeAcceptTradeButtonOpponent at 0x6FADFCE5
        if (gbTradeOtherAccepted)
        {
            tPanel.nFrame = 17;
            D2GFX_TEXTURE_CelDrawColor(&tPanel, gnBorderXDifference + 275, gnBorderYDifference + gnScreenHeight - 278, 0xFFFFFFFF, 5, 2);
        }
        else
        {
            tPanel.nFrame = 16;
            D2GFX_TEXTURE_CelDraw(&tPanel, gnBorderXDifference + 275, gnBorderYDifference + gnScreenHeight - 278, 0xFFFFFFFF, 5, NULL);
        }

        //  ours: blocked wins over accepted, and both over the plain one
        //  D2ExpRes: ExchangeAcceptTradeButtonRed at 0x6FADFD22,
        //  ExchangeAcceptTradeButton at 0x6FADFD62,
        //  ExchangeAcceptTradeButtonGreen at 0x6FADFD93
        if (gbTradeAcceptBlocked)
        {
            tPanel.nFrame = 16;
            D2GFX_TEXTURE_CelDrawColor(&tPanel, gnBorderXDifference + 223, gnBorderYDifference + gnScreenHeight - 64, 0xFFFFFFFF, 5, 1);
        }
        else if (gbTradeOwnAccepted || gbTradeAcceptChecked)
        {
            tPanel.nFrame = 17;
            D2GFX_TEXTURE_CelDrawColor(&tPanel, gnBorderXDifference + 223, gnBorderYDifference + gnScreenHeight - 64, 0xFFFFFFFF, 5, 2);
        }
        else
        {
            tPanel.nFrame = 16;
            D2GFX_TEXTURE_CelDraw(&tPanel, gnBorderXDifference + 223, gnBorderYDifference + gnScreenHeight - 64, 0xFFFFFFFF, 5, NULL);
        }

        //  D2ExpRes: ExchangeCancelButton at 0x6FADFDDB
        tPanel.nFrame = gbTradeCancelPending == 1 ? 11 : 10;
        D2GFX_TEXTURE_CelDraw(&tPanel, gnBorderXDifference + 275, gnBorderYDifference + gnScreenHeight - 64, 0xFFFFFFFF, 5, NULL);

        nVendorPageSaved = gnVendorPage;
        gnVendorPage = 1;
        INV_DrawInventoryGridItems(CUNIT_GetPlayer());
        gnVendorPage = 2;
        INV_DrawInventoryGridItems(CUNIT_GetPlayer());
        gnVendorPage = nVendorPageSaved;

        D2WIN_10127_SetFont(D2FONT_FONT16);

        //  the two names. The own name's buffer is the panel's GfxData slot
        //  again -- one stack address, two lives; here they are two locals.
        Unicode wszOwnName[23];

        pPlayer = CUNIT_GetPlayer();
        if (pPlayer)
        {
            char *szRosterName = ROSTER_GetPlayerDisplayName(pPlayer);
            if (szRosterName)
            {
                Unicode::win2Unicode(wszOwnName, szRosterName, 23);
            }
        }

        Unicode wszName[23];

        Unicode::strcpy(wszName, TRADE_GetPartnerName());
        nNameLen = Unicode::strlen(wszName);
        if (D2WIN_10121_GetTextWidth(wszName) > 112)
        {
            //  the first store lands on the terminator, so the first pass
            //  shortens nothing and measures again
            pwszCut = &wszName[nNameLen];
            do
            {
                pwszCut->ch = 0;
                --pwszCut;
            }
            while (D2WIN_10121_GetTextWidth(wszName) > 112);
        }
        //  D2ExpRes: ExchangeNameOpponent at 0x6FADFEF3
        D2WIN_10117_DrawText(wszName, gnBorderXDifference + 25, 27 - gnBorderYDifference, 0, 0);

        Unicode::strcpy(wszName, wszOwnName);
        nNameLen = Unicode::strlen(wszName);
        if (D2WIN_10121_GetTextWidth(wszName) > 112)
        {
            pwszCut = &wszName[nNameLen];
            do
            {
                pwszCut->ch = 0;
                --pwszCut;
            }
            while (D2WIN_10121_GetTextWidth(wszName) > 112);
        }
        //  D2ExpRes: ExchangeName at 0x6FADFF65
        D2WIN_10117_DrawText(wszName, gnBorderXDifference + 25, 242 - gnBorderYDifference, 0, 0);

        //  the two gold lines, theirs at the top and ours below
        nY = 27 - gnBorderYDifference;
        pwszText = D2LANG_GetStringFromTblIndex(STR_IDX_2215_gld);
        nX = gnBorderXDifference - D2WIN_10121_GetTextWidth(pwszText) / 2 + 187;
        D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_2215_gld), nX, nY, 0, 0);

        Unicode wszGold[15];
        char szGold[16];

        STORM_SStrPrintf(szGold, 0xFu, "%d", TRADE_GetGoldOther());
        Unicode::win2Unicode(wszGold, szGold, 15);
        nY = 27 - gnBorderYDifference;
        nTextWidth = D2WIN_10121_GetTextWidth(wszGold);
        D2WIN_10117_DrawText(wszGold, gnBorderXDifference - nTextWidth + 292, nY, 0, 0);

        nY = 242 - gnBorderYDifference;
        pwszText = D2LANG_GetStringFromTblIndex(STR_IDX_2215_gld);
        nX = gnBorderXDifference - D2WIN_10121_GetTextWidth(pwszText) / 2 + 187;
        D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_2215_gld), nX, nY, 0, 0);

        STORM_SStrPrintf(szGold, 0xFu, "%d", TRADE_GetGoldSelf());
        Unicode::win2Unicode(wszGold, szGold, 15);
        nY = 242 - gnBorderYDifference;
        nTextWidth = D2WIN_10121_GetTextWidth(wszGold);
        D2WIN_10117_DrawText(wszGold, gnBorderXDifference - nTextWidth + 292, nY, 0, 0);
    }

    //  the inventory grid itself
    nVendorPageSaved = gnVendorPage;
    gnVendorPage = 0;
    INV_DrawInventoryGridItems(CUNIT_GetPlayer());
    gnVendorPage = nVendorPageSaved;

    nSwapOffsetX = 0;
    nSwapOffsetY = 0;
    if (GAME_IsExpansionGame() && gnWeaponSwapSet)
    {
        nSwapOffsetX = 4;
        nSwapOffsetY = -4;
    }

    pPlayer = CUNIT_GetPlayer();
    pInventory = pPlayer->pInventory;
    D2COMMON_UNITS_GetInventoryRecordId(pPlayer, INVPAGE_INVENTORY, GAME_IsExpansionGame());
    INV_EnsurePanelLayout();

    //  the equipped items, eleven body locations
    nBodyLoc = 0;
    do
    {
        pOtherArmItem = NULL;
        pItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, (D2C_PlayerBodyLocs)nBodyLoc);
        if (!pItem)
        {
            goto NEXT_BODYLOC;
        }

        D2COMMON_ITEMS_GetDimensions(pItem, &nItemWidth, &nItemHeight, szD2CommonCmnItemsH, 1754);
        bTwoHanded = D2COMMON_INVENTORY_GetWieldType(pPlayer, pInventory) == 2;

        //  a two-handed weapon is drawn in the OTHER hand's slot as well,
        //  centred by ITS rect, and that slot gets the plain highlight
        if (nBodyLoc == BODYLOC_RARM)
        {
            pOtherArmItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_LARM);
            if (bTwoHanded && !D2COMMON_ITEMS_GetQuiverType(pOtherArmItem) && !D2COMMON_ITEMS_GetQuiverType(pItem))
            {
                GFX_DrawInventoryItem(pItem, gInvBodyLocRects[BODYLOC_LARM].rect.nLeft + nSwapOffsetX + ((gInvBodyLocRects[BODYLOC_RARM].nWidth - nItemWidth * gInvPanelGrid.nGridBoxWidth) >> 1), nSwapOffsetY + gInvBodyLocRects[BODYLOC_LARM].rect.nTop
                        + ((gInvBodyLocRects[BODYLOC_RARM].nHeight - nItemHeight * gInvPanelGrid.nGridBoxHeight) >> 1));
                if (gInvBodyLocRects[BODYLOC_LARM].rect.nLeft >= 0 && gInvBodyLocRects[BODYLOC_LARM].rect.nLeft < gnViewportRight && gInvBodyLocRects[BODYLOC_LARM].rect.nTop >= 0 && gInvBodyLocRects[BODYLOC_LARM].rect.nTop < gnViewportBottom)
                {
                    //  the colour byte only (see the DIFFERENCE note at the head)
                    dwColor = gInvHighlightColors[0];
                    GFX_DrawFilledRect(gInvBodyLocRects[BODYLOC_LARM].rect.nLeft, gInvBodyLocRects[BODYLOC_LARM].rect.nTop, gInvBodyLocRects[BODYLOC_LARM].nWidth, gInvBodyLocRects[BODYLOC_LARM].nHeight, dwColor, 0);
                }
            }
        }
        else if (nBodyLoc == BODYLOC_LARM)
        {
            pOtherArmItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RARM);
            if (bTwoHanded && !D2COMMON_ITEMS_GetQuiverType(pOtherArmItem) && !D2COMMON_ITEMS_GetQuiverType(pItem))
            {
                GFX_DrawInventoryItem(pItem, gInvBodyLocRects[BODYLOC_RARM].rect.nLeft + nSwapOffsetX + ((gInvBodyLocRects[BODYLOC_LARM].nWidth - nItemWidth * gInvPanelGrid.nGridBoxWidth) >> 1), nSwapOffsetY + gInvBodyLocRects[BODYLOC_RARM].rect.nTop
                        + ((gInvBodyLocRects[BODYLOC_LARM].nHeight - nItemHeight * gInvPanelGrid.nGridBoxHeight) >> 1));
                if (gInvBodyLocRects[BODYLOC_RARM].rect.nLeft >= 0 && gInvBodyLocRects[BODYLOC_RARM].rect.nLeft < gnViewportRight && gInvBodyLocRects[BODYLOC_RARM].rect.nTop >= 0 && gInvBodyLocRects[BODYLOC_RARM].rect.nTop < gnViewportBottom)
                {
                    dwColor = gInvHighlightColors[0];
                    GFX_DrawFilledRect(gInvBodyLocRects[BODYLOC_RARM].rect.nLeft, gInvBodyLocRects[BODYLOC_RARM].rect.nTop, gInvBodyLocRects[BODYLOC_RARM].nWidth, gInvBodyLocRects[BODYLOC_RARM].nHeight, dwColor, 0);
                }
            }
        }

        nX = gInvBodyLocRects[nBodyLoc].rect.nLeft + ((gInvBodyLocRects[nBodyLoc].nWidth - nItemWidth * gInvPanelGrid.nGridBoxWidth) >> 1);
        nY = gInvBodyLocRects[nBodyLoc].rect.nTop + ((gInvBodyLocRects[nBodyLoc].nHeight - nItemHeight * gInvPanelGrid.nGridBoxHeight) >> 1);

        //  the per-slot nudges, in the original's switch
        switch (nBodyLoc)
        {
        case 2:
        case 6:
        case 7:
        case 10:
            nY -= 2;
            break;
        case 3:
            nX += 3;
            break;
        case 4:
        case 5:
            nX += nSwapOffsetX;
            nY += nSwapOffsetY;
            break;
        case 8:
            nX += 2;
            nY -= 2;
            break;
        case 9:
            nX += 2;
            nY -= 4;
            break;
        default:
            break;
        }

        if (pItem == gpHoverItem && gbShowSetItemDesc)
        {
            //  the hovered item: green unless the cursor is in state 8
            if (CURSOR_GetCursorState() == 8)
            {
                if (gInvBodyLocRects[nBodyLoc].rect.nLeft >= 0 && gInvBodyLocRects[nBodyLoc].rect.nLeft < gnViewportRight && gInvBodyLocRects[nBodyLoc].rect.nTop >= 0 && gInvBodyLocRects[nBodyLoc].rect.nTop < gnViewportBottom)
                {
                    dwColor = gInvHighlightColors[0];
                    GFX_DrawFilledRect(gInvBodyLocRects[nBodyLoc].rect.nLeft, gInvBodyLocRects[nBodyLoc].rect.nTop, gInvBodyLocRects[nBodyLoc].nWidth, gInvBodyLocRects[nBodyLoc].nHeight, dwColor, 0);
                }
            }
            else if (gInvBodyLocRects[nBodyLoc].rect.nLeft >= 0 && gInvBodyLocRects[nBodyLoc].rect.nLeft < gnViewportRight && gInvBodyLocRects[nBodyLoc].rect.nTop >= 0 && gInvBodyLocRects[nBodyLoc].rect.nTop < gnViewportBottom)
            {
                dwColor = gInvHighlightColors[1];
                GFX_DrawFilledRect(gInvBodyLocRects[nBodyLoc].rect.nLeft, gInvBodyLocRects[nBodyLoc].rect.nTop, gInvBodyLocRects[nBodyLoc].nWidth, gInvBodyLocRects[nBodyLoc].nHeight, dwColor, 0);
            }

            GFX_DrawInventoryItem(pItem, nX, nY);

            if (D2COMMON_ITEMS_CheckIfSocketable(pItem) && D2COMMON_ITEMS_GetMaxSockets(pItem) > 0 && D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_SOCKETED, 1649, szInvCpp))
            {
                INV_DrawSocketedItems(pItem, &gInvPanelGrid, nX, nY);
            }
            goto NEXT_BODYLOC;
        }

        //  not hovered: the box says why the item is there or not usable
        if (!pOtherArmItem && (D2COMMON_ITEMS_GetAmmoType(pItem) || D2COMMON_ITEMS_GetQuiverType(pItem)))
        {
            goto PLAIN_BOX;
        }
        if (!D2COMMON_ITEMS_CheckRequirements(pItem, pPlayer, 0, NULL, NULL, NULL) || ITEM_IsItemLocked(pItem) || !D2COMMON_ITEMS_CanBeEquipped(pItem))
        {
            goto PLAIN_BOX;
        }
        if (D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_IDENTIFIED, 1664, szInvCpp))
        {
            goto DRAW_ITEM;
        }
        if (gInvBodyLocRects[nBodyLoc].rect.nLeft < 0 || gInvBodyLocRects[nBodyLoc].rect.nLeft >= gnViewportRight || gInvBodyLocRects[nBodyLoc].rect.nTop < 0 || gInvBodyLocRects[nBodyLoc].rect.nTop >= gnViewportBottom)
        {
            goto DRAW_ITEM;
        }
        dwColor = gInvHighlightColors[4];
        goto DRAW_BOX;

PLAIN_BOX:
        if (gInvBodyLocRects[nBodyLoc].rect.nLeft < 0 || gInvBodyLocRects[nBodyLoc].rect.nLeft >= gnViewportRight || gInvBodyLocRects[nBodyLoc].rect.nTop < 0 || gInvBodyLocRects[nBodyLoc].rect.nTop >= gnViewportBottom)
        {
            goto DRAW_ITEM;
        }
        dwColor = gInvHighlightColors[0];

DRAW_BOX:
        GFX_DrawFilledRect(gInvBodyLocRects[nBodyLoc].rect.nLeft, gInvBodyLocRects[nBodyLoc].rect.nTop, gInvBodyLocRects[nBodyLoc].nWidth, gInvBodyLocRects[nBodyLoc].nHeight, dwColor, 0);

DRAW_ITEM:
        GFX_DrawInventoryItem(pItem, nX, nY);

NEXT_BODYLOC:
        ++nBodyLoc;
    }
    while (nBodyLoc < 11);

    //  the slot under a held item
    if ((sgInventoryMode != 1 || CURSOR_GetMouseX() >= gnScreenWidth / 2) && CURSOR_GetMouseY() < gnScreenHeight - 39)
    {
        if (CURSOR_GetMouseX() < gnScreenWidth / 2)
        {
            gnHoverBodyLoc = 0;
        }
        else
        {
            pCursorItem = D2COMMON_INVENTORY_GetCursorItem(pInventory);
            if (pCursorItem && gbShowSetItemDesc)
            {
                //  measured and thrown away -- neither output is read
                D2COMMON_ITEMS_GetDimensions(pCursorItem, &nCursorWidth, &nCursorHeight, szD2CommonCmnItemsH, 1754);
                //  green when the slot would take it, or when it is a filler
                //  the socketed item there can hold; the plain colour
                //  otherwise. The original shares one box call between the
                //  three arms.
                if (D2COMMON_ITEMS_CheckBodyLocation(pCursorItem, (uint8_t)gnHoverBodyLoc) && D2COMMON_ITEMS_CheckRequirements(pCursorItem, CUNIT_GetPlayer(), 0, NULL, NULL, NULL))
                {
                    bHoverGreen = TRUE;
                }
                else if (INV_CanInsertSocketFiller(pCursorItem, D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, (D2C_PlayerBodyLocs)gnHoverBodyLoc)))
                {
                    bHoverGreen = TRUE;
                }
                else
                {
                    bHoverGreen = FALSE;
                }

                pHoverRect = &gInvBodyLocRects[gnHoverBodyLoc];
                if (pHoverRect->rect.nLeft >= 0 && pHoverRect->rect.nLeft < gnViewportRight && pHoverRect->rect.nTop >= 0 && pHoverRect->rect.nTop < gnViewportBottom)
                {
                    dwColor = bHoverGreen ? gInvHighlightColors[1] : gInvHighlightColors[0];
                    GFX_DrawFilledRect(pHoverRect->rect.nLeft, pHoverRect->rect.nTop, pHoverRect->nWidth, pHoverRect->nHeight, dwColor, 0);
                }
            }
        }
    }

    //  the shop buttons' hover hint
    if (sgInventoryMode >= 1 && sgInventoryMode <= 9 && gnUiMouseX >= gInvNpcShopRect.nLeft && gnUiMouseX <= gInvNpcShopRect.nRight)
    {
        //  D2ExpRes: BuySellStringsMouseHoverOver1 at 0x6FAE079E
        nY = gnBorderYDifference + gnScreenHeight - 109;
        if (gnUiMouseY >= nY && gnUiMouseY <= gInvNpcShopRect.nBottom && gnNpcShopTabCount > 0)
        {
            iShopTab = 0;
            do
            {
                if (gNpcShopTabs[iShopTab].bEnabled == 1)
                {
                    nX = gnBorderXDifference + gatButtonsOffsetX[gnNpcShopCloseTab][iShopTab];
                    //  D2ExpRes: BuySellStringsMouseHoverOver3 at 0x6FAE07FD
                    if (gnUiMouseX > nX
                        && gnUiMouseX < nX + 45
                        && gnUiMouseY > nY
                        //  D2ExpRes: BuySellStringsMouseHoverOver2, 0x6FAE0811
                        && gnUiMouseY < gnBorderYDifference
                                                      + gnScreenHeight - 65)
                    {
                        break;
                    }
                }
                ++iShopTab;
            }
            while (iShopTab < gnNpcShopTabCount);

            if (iShopTab < gnNpcShopTabCount)
            {
                if (gNpcShopTabs[iShopTab].nHoverTextId == STR_IDX_10095_x_REPAIR_ALL_EQUIPMENT_FMT)
                {
                    Unicode wszRepairAll[256];

                    pPlayer = CUNIT_GetPlayer();
                    pActiveNpc = NPCMENU_NPC_GetActiveNpcUnit();
                    if (pPlayer && pActiveNpc)
                    {
                        pQuestFlags = NPCMENU_QuestGetQuestRecord();
                        nRepairCost = D2COMMON_ITEMS_GetAllRepairCosts(NULL, pPlayer, pActiveNpc->dwClassId, (D2C_Difficulties)GAME_GetDifficulty(), pQuestFlags, NULL);
                        Unicode::sprintf(256, wszRepairAll, D2LANG_GetStringFromTblIndex(STR_IDX_10095_x_REPAIR_ALL_EQUIPMENT_FMT), nRepairCost);
                        //  D2ExpRes: BuySellRepairAllString at 0x6FAE08D0
                        D2WIN_10129_DrawFramedText(wszRepairAll, gnBorderXDifference + gatButtonsOffsetX[gnNpcShopCloseTab][iShopTab], gnBorderYDifference + gnScreenHeight - 100, 0, 0);
                    }
                }
                else
                {
                    //  D2ExpRes: BuySellRepairSellBuyStrings at 0x6FAE0908
                    nY = gnBorderYDifference + gnScreenHeight - 100;
                    nX = gnBorderXDifference + gatButtonsOffsetX[gnNpcShopCloseTab][iShopTab];
                    D2WIN_10129_DrawFramedText(D2LANG_GetStringFromTblIndex(gNpcShopTabs[iShopTab].nHoverTextId), nX, nY, 0, 0);
                }
            }
        }
    }

    //  the carried gold, the close button and the description
    INV_DrawGoldAmount(1);

    tPanel.pCellFile = UI_GetBuySellButtonImage();
    tPanel.nFrame = (gbInvCloseBtnArmed != 0) + 10;
    //  D2ExpRes: InventoryCloseButton at 0x6FAE0963
    D2GFX_TEXTURE_CelDraw(&tPanel, gnScreenWidth - gnBorderXDifference - 302, gnBorderYDifference + gnScreenHeight - 64, 0xFFFFFFFF, 5, NULL);

    pDescUnit = CUNIT_GetPlayer();
    bUseHireling = 0;
    if (sgInventoryMode >= 1 && sgInventoryMode <= 9 && nMouseX < gnScreenWidth / 2 && NPCMENU_GetInteractingNpc(&nNpcGUID))
    {
        pNpc = CUNIT_FindUnitByGuidAndType(nNpcGUID, UNIT_MONSTER);
        if (pNpc)
        {
            pDescUnit = pNpc;
            bUseHireling = 1;
        }
    }
    INV_DrawItemDescription(pDescUnit, bUseHireling);
}

//  Three functions of this unit (0x6FAEC510, 0x6FAEC6C0 and 0x6FAECF90) whose only caller is in another unit: TRADE_HandleStateMessage ends three of
//  its switch arms with a tail jmp into them (case 0x10 the stash opens, 0x11 the stash goes away, 0x15 the cube opens).

//1.10f: D2Client.0x6FAEC510
//  Opens the stash. The toggle decides: if UI_STASH was already on, UI_SetUIToggle returns 0 and nothing else happens. The four flags cleared
//  after it are the two gold buttons' (drop and withdraw) and the close button's pressed and armed pair. The walk over the object units is what
//  finds the stash chest for the cube-opened-from-the-stash path.
void __cdecl INV_StashOpenPanel()
{
    if (UI_SetUIToggle(UI_STASH, UI_TURNON, 0))
    {
        gbCubeOpenedFromStash = 0;
        CUNIT_ForEachObjectUnit(INV_CubeFindStashUnit);
        sgInventoryMode = 12;
        GAME_ResetClickState();
        gbDropGoldBtnPressed = 0;
        gbWithdrawGoldBtnPressed = 0;
        gbStashCloseBtnPressed = 0;
        gbStashCloseBtnArmed = 0;
    }
}

//1.10f: D2Client.0x6FAEC6C0
//  The stash goes away, and its two images with it. The mode is only cleared when it is the stash's own 12 or 13; the images are freed whatever
//  the mode was.
void __cdecl INV_StashTeardownPanel()
{
    if (sgInventoryMode == 12 || sgInventoryMode == 13)
    {
        sgInventoryMode = 0;
        UI_SetUIToggle(UI_STASH, UI_TURNOFF, 0);
    }
    if (gpStashPanelImage)
    {
        ARCHIVE_CellFileFree(gpStashPanelImage);
        gpStashPanelImage = NULL;
    }
    if (gpTradeStashImage)
    {
        ARCHIVE_CellFileFree(gpTradeStashImage);
        gpTradeStashImage = NULL;
    }
}

//1.10f: D2Client.0x6FAECF90
//  Opens the cube, the same shape as the stash's: the toggle decides, and five flags are cleared after it: the drop-gold button it shares with the
//  inventory, and the cube's own close pressed, close armed, transmute pressed and transmute armed.
void __cdecl INV_CubeOpenPanel()
{
    if (UI_SetUIToggle(UI_CUBE, UI_TURNON, 0))
    {
        gbCubeCloseCmdSent = 0;
        sgInventoryMode = 14;
        GAME_ResetClickState();
        gbDropGoldBtnPressed = 0;
        gbCubeCloseBtnPressed = 0;
        gbCubeCloseBtnArmed = 0;
        gbCubeTransmuteBtnPressed = 0;
        gbCubeTransmuteBtnArmed = 0;
    }
}

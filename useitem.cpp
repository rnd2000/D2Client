#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "CoreArchive.h"
#include "useitem.h"

//  useitem.cpp -- the Hellforge anvil panel of D2Client.dll 1.10: put an item on the anvil, confirm, and the server answers. Opening and closing
//  the panel, its two buttons and the item slot, the frame tick, the eight-entry message table and the server packet that drives it.
//  The original's source file is D2Client\UI\useitem.cpp (the path four of its asserts push). The unit starts at USEITEM_AnvilInit
//  (0x6FB24840) and ends with USEITEM_AnvilOpenPanelLocal (0x6FB25BA0).

//  The two panel images, loaded once by USEITEM_AnvilInit and released by USEITEM_AnvilShutdown. Lower case in the original.
static const char szMenuUpgrade[] = "menu\\upgrade";
static const char szMenuHoradricBack[] = "menu\\horadricback";


//  gnAnvilState: 0 closed, 1 the item is on the cursor and the slot is empty, 2 the item is in the slot, 3 finished (the result line is on
//  the dialog and nothing more happens until the panel closes).
//  The button table is two 32 x 32 buttons on one row, 55 pixels apart (rectangles L 122 R 154 T 224 B 256 and L 177 R 209 T 224 B 256),
//  compiled in; nothing in the module writes those fields. Button 0 is the confirm, and `if (nButton)` takes the cancel path.
//  bPressed is not an enabled flag: it is set on the button under a mouse-down after every button was cleared, and read on the mouse-up in
//  USEITEM_AnvilOnPanelClick, so it pairs a release with its own press.

static_assert(UI_ANVIL == 14 && UI_TURNOFF == 1,
              "every close path here ends in `UI_SetUIToggle(14, 1, 0)`, which is "
              "UI_ANVIL turned OFF -- and 68 = 0x44 is D2GSPacketClt44, whose "
              "second field D2MOO names nAnvilInteractGUID");
static_assert(sizeof(D2WinCommandRegisterStrc) == 12,
              "gAnvilMsgHandlers is unregistered eight records at a time, and "
              "the table shares its record with every other panel's");


//1.10f: Inlined in D2Client.0x6FB248A0 USEITEM_AnvilShutdown, 0x6FB249D0
//       USEITEM_AnvilCloseOnInteractionStop, 0x6FB24B20 USEITEM_AnvilOnButtonUp,
//       0x6FB24CD0 USEITEM_AnvilOnPanelClick -- and four more outside this file.
//       EIGHT copies, no out-of-line one.
//  Moves the item between the cursor and the anvil slot, whichever way round it currently is. State 1 (on the cursor): it goes into the
//  slot and the item's own drop sound plays. State 2 (in the slot): it comes back to the cursor with sound 1. Any other state does nothing.
//  BUG: at four of its eight call sites the caller writes 0 to gnAnvilState just before (USEITEM_AnvilShutdown,
//  USEITEM_AnvilCloseOnInteractionStop, USEITEM_AnvilOnButtonUp, USEITEM_AnvilOnPanelClick), and both arms of the helper test for 1 and 2,
//  so the body does nothing there. LATENT: the caller has already put the item back on the cursor at the top of its own close sequence.
//  Reproduced.
static void ANVIL_SwapSlotItem()
{
    D2UnitStrc *pPlayer;
    D2UnitStrc *pCursorItem;
    D2C_Sounds nDropSound;

    pPlayer = CUNIT_GetPlayer();
    if (pPlayer)
        pCursorItem = D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory);
    else
        pCursorItem = NULL;

    if (gnAnvilState == 1)
    {
        gnAnvilState = 2;
        CURSOR_SetHeldUnit(NULL);
        if (pCursorItem)
        {
            gnAnvilSlotUnitId = pCursorItem->dwUnitId;
            nDropSound = ITEM_GetDropSoundNoFrame(pCursorItem);
            SOUNDINST_PlaySound(nDropSound, NULL, 0, 0, 0);
        }
    }
    else if (gnAnvilState == 2)
    {
        gnAnvilState = 1;
        gnAnvilSlotUnitId = 0;
        if (pCursorItem)
            CURSOR_SetHeldUnit(pCursorItem);
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
    }
}


//1.10f: Inlined in D2Client.0x6FB248A0 USEITEM_AnvilShutdown, 0x6FB249D0
//       USEITEM_AnvilCloseOnInteractionStop, 0x6FB24B20 USEITEM_AnvilOnButtonUp,
//       0x6FB24CD0 USEITEM_AnvilOnPanelClick -- and USEITEM_AnvilOnCloseKey.
//  Takes the panel down: unhooks its eight message handlers, closes the dialog if one is up, turns UI_ANVIL off and drops the click region.
//  Every close path ends in exactly these four steps, in this order.
static void ANVIL_TeardownPanel()
{
    WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gAnvilMsgHandlers, 8);
    if (gpAnvilDialog)
    {
        DIALOG_Close(gpAnvilDialog);
        gpAnvilDialog = NULL;
    }
    UI_SetUIToggle(UI_ANVIL, UI_TURNOFF, 0);
    SELECT_SetClickRegion(NULL);
}


//1.10f: Inlined in D2Client.0x6FB249D0 USEITEM_AnvilCloseOnInteractionStop,
//       0x6FB24B20 USEITEM_AnvilOnButtonUp, 0x6FB24CD0 USEITEM_AnvilOnPanelClick,
//       0x6FB25A10 USEITEM_AnvilOpenPanel -- and USEITEM_AnvilOnCloseKey.
//  Tells the other side the interaction is over; which side depends on how the panel was opened: locally through
//  USEITEM_AnvilOpenPanelLocal, or by the server's packet.
static void ANVIL_NotifyInteractionEnd()
{
    D2UnitStrc *pPlayer;
    int32_t dwUnitId;

    if (gnAnvilPanelMode)
    {
        NPCMENU_NpcCloseInteraction();
    }
    else
    {
        pPlayer = CUNIT_GetPlayer();
        dwUnitId = pPlayer ? (int32_t)pPlayer->dwUnitId : -1;
        MSG_SendCmd4Dwords(68, dwUnitId, gnAnvilInteractGUID, 0, 2);
    }
}


//1.10f: Inlined in every close path of the family -- EIGHT sites.
//  Zeroes the state and takes the panel down. The swap in the middle does nothing, and cannot: the line above it puts the state outside both
//  of that helper's arms. Reproduced because the original runs it.
static void ANVIL_ResetAndTeardown()
{
    gnAnvilState = 0;
    ANVIL_SwapSlotItem();
    ANVIL_TeardownPanel();
}


//1.10f: Inlined in D2Client.0x6FB249D0 USEITEM_AnvilCloseOnInteractionStop,
//       0x6FB24B20 USEITEM_AnvilOnButtonUp, 0x6FB24CD0 USEITEM_AnvilOnPanelClick -- and
//       USEITEM_AnvilOnCloseKey.
//  Closes the panel and tells the other side. The item comes back to the cursor first if it was in the slot, then sound 1, then the
//  notification: NPCMENU_NpcCloseInteraction() when gnAnvilPanelMode is set, otherwise MSG_SendCmd4Dwords(68, myUnitId, gnAnvilInteractGUID,
//  0, 2). Then the state is zeroed, the swap helper is called (and does nothing because of it), and the panel comes down.
static void ANVIL_CloseAndNotify()
{
    D2UnitStrc *pPlayer;
    D2UnitStrc *pCursorItem;

    if (gnAnvilState == 2)
    {
        pPlayer = CUNIT_GetPlayer();
        if (pPlayer)
            pCursorItem = D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory);
        else
            pCursorItem = NULL;
        CURSOR_SetHeldUnit(pCursorItem);
    }
    SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
    ANVIL_NotifyInteractionEnd();
    ANVIL_ResetAndTeardown();
}


//1.10f: Inlined in D2Client.0x6FB24840 USEITEM_AnvilInit, 0x6FB24CD0
//       USEITEM_AnvilOnPanelClick, 0x6FB24F80 USEITEM_AnvilOnPanelButtonDown.
//  Clears the press marker on every button: both bytes bState and bPressed, at +0x26 and +0x27 of each 40-byte record, gnPanelButtonCount of
//  them. This makes the press radio-style: a click that misses every rectangle leaves them all clear, and the release handler finds nothing
//  to pair with.
static void ANVIL_ClearButtonPresses()
{
    D2AnvilPanelButtonStrc *pButton;
    int32_t nCount;

    nCount = gnPanelButtonCount;
    if (nCount > 0)
    {
        pButton = gAnvilPanelButtons;
        do
        {
            pButton->bPressed = 0;
            pButton->bState = 0;
            ++pButton;
            --nCount;
        }
        while (nCount);
    }
}


//1.10f: D2Client.0x6FB24840
//  Brings the anvil UI up: four state words cleared, the two panel images loaded, and every button's press marker cleared.
//  gnAnvilNpcGUID is the odd one out: write-only (two writers, no reader anywhere), and it lies past .data's raw size so it starts at 0
//  anyway. Reproduced; LATENT.
void __cdecl USEITEM_AnvilInit()
{
    gnAnvilState = 0;
    gnAnvilInteractGUID = 0;
    gnAnvilNpcGUID = 0;
    gdwAnvilLastClickTick = 0;

    gpImgAnvilUpgrade = UI_LoadImageFromUiDir(szMenuUpgrade);
    gpImgAnvilBackground = UI_LoadImageFromUiDir(szMenuHoradricBack);

    ANVIL_ClearButtonPresses();
}


//1.10f: D2Client.0x6FB248A0
//  Tears the panel down for good; the counterpart of USEITEM_AnvilInit. Two halves, and only the first is conditional: with the panel open it
//  zeroes the state, runs the swap helper (which does nothing, because of that) and takes the panel down. Then, unconditionally, it releases
//  the two images, so a shutdown with the panel already closed still frees the art.
void __cdecl USEITEM_AnvilShutdown()
{
    if (gnAnvilState)
    {
        gnAnvilState = 0;
        //  Dead, as in ANVIL_CloseAndNotify: the state was zeroed just above.
        ANVIL_SwapSlotItem();
        ANVIL_TeardownPanel();
    }

    if (gpImgAnvilUpgrade)
    {
        ARCHIVE_CellFileFree(gpImgAnvilUpgrade);
        gpImgAnvilUpgrade = NULL;
    }
    if (gpImgAnvilBackground)
    {
        ARCHIVE_CellFileFree(gpImgAnvilBackground);
        gpImgAnvilBackground = NULL;
    }
}


//1.10f: D2Client.0x6FB249A0
//  Slot 4 of gAnvilMsgHandlers, WM_MOUSEMOVE (0x200). Clears the framed-text tooltip, hit-tests the belt, and only while the item is still
//  on the cursor hands the message on for the hover feedback. Once the item is in the slot the handler does nothing but clear the tooltip.
void __stdcall USEITEM_AnvilOnMouseMove(SMSGHANDLER_PARAMS *pMsg)
{
    D2WIN_10129_DrawFramedText(NULL, -1, -1, 0, 0);
    PANEL_BeltHitTestPanel(pMsg);
    if (gnAnvilState == 1)
        INV_AnvilForwardMouseMoveToInventory(pMsg);
}


//1.10f: D2Client.0x6FB249D0
//  The panel's third exit, from NPCMENU_OnInteractionStop: the NPC ended the conversation rather than the player pressing anything. Does
//  nothing at all in state 3, the finished state; otherwise it is ANVIL_CloseAndNotify whole. void: its callers keep no answer.
void __cdecl USEITEM_AnvilCloseOnInteractionStop()
{
    if (gnAnvilState != 3)
        ANVIL_CloseAndNotify();
}


//1.10f: D2Client.0x6FB24B20
//  A mouse button was released over the panel. Slots 0 and 3 of gAnvilMsgHandlers, WM_LBUTTONUP (0x202) and WM_RBUTTONUP (0x205), both bound
//  to this one function, as USEITEM_AnvilOnButtonDown is bound to the two downs.
//  It claims the message first and decides afterwards, so a release over the panel never reaches the game underneath. With the item in the
//  slot it offers the release to the panel and then to the inventory close button; only if both decline, and the state has not become 3 in
//  the meantime, does the panel close. Otherwise the inventory close button gets first refusal and the panel second.
void __stdcall USEITEM_AnvilOnButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nMouseX;
    int32_t nMouseY;

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    nMouseX = pMsg->nXpos;
    nMouseY = pMsg->nYpos;

    if (gnAnvilState == 2)
    {
        if (!USEITEM_AnvilOnPanelClick(nMouseX, nMouseY) && INV_AnvilOnInvCloseButtonPress(pMsg) && gnAnvilState != 3)
        {
            ANVIL_CloseAndNotify();
        }
    }
    else if (!INV_AnvilOnInvCloseButtonRelease(pMsg))
    {
        USEITEM_AnvilOnPanelClick(nMouseX, nMouseY);
    }
}


//1.10f: D2Client.0x6FB24CD0
//  Did the click land on a panel button? The answer is read: USEITEM_AnvilOnButtonUp falls through to its own handling only when it is 0.
//  Walks the two-button table and hit-tests the cursor against each rectangle, requiring the button's press marker to be set, which pairs the
//  release with a press on the same button.
//      button 0, state 2   the confirm: sound 2, a dialog with tbl 3353 (TransactionResults1) on it, state 3, and the transaction sent:
//                          NPCMENU_SendQuestItemAction in NPC mode, otherwise SendCmd4Dwords(68, ..., 3)
//      any other button    the cancel: close and notify, unless the state is already 3
//  Whichever it was, every button's press marker is cleared on the way out; the sweep re-reads gnPanelButtonCount inside the loop, so it is
//  read once per button plus once at the end. Reproduced.
int32_t __fastcall USEITEM_AnvilOnPanelClick(int32_t nMouseX, int32_t nMouseY)
{
    int32_t bHandled;
    int32_t nButton;
    const Unicode *pwszResult;
    D2UnitStrc *pPlayer;
    int32_t dwPlayerGuid;

    bHandled = 0;
    nButton = 0;
    if (gnPanelButtonCount <= 0)
        return bHandled;

    do
    {
        if (gAnvilPanelButtons[nButton].bPressed && nMouseX >= gAnvilPanelButtons[nButton].nLeft && nMouseX <= gAnvilPanelButtons[nButton].nRight && nMouseY >= gAnvilPanelButtons[nButton].nTop && nMouseY <= gAnvilPanelButtons[nButton].nBottom)
        {
            SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
            if (nButton)
            {
                if (gnAnvilState != 3)
                    ANVIL_CloseAndNotify();
            }
            else if (gnAnvilState == 2)
            {
                SOUNDINST_PlaySound(SOUND_CURSOR_SELECT, NULL, 0, 0, 0);
                gpAnvilDialog = DIALOG_Create(150, 130, (DIALOGCALLBACK)USEITEM_NullCallback, NULL, 1, 0, 1, 0, 0, 0, 1);
                DIALOG_SetHighlight(gpAnvilDialog, 0);
                pwszResult = D2LANG_GetStringFromTblIndex(STR_IDX_3353_TransactionResults1);
                DIALOG_AddLine(gpAnvilDialog, pwszResult, 15, 0, 0, 1, NULL, 0);
                gnAnvilState = 3;

                if (gnAnvilPanelMode)
                {
                    NPCMENU_SendQuestItemAction(gnAnvilSlotUnitId);
                }
                else
                {
                    pPlayer = CUNIT_GetPlayer();
                    dwPlayerGuid = pPlayer ? (int32_t)pPlayer->dwUnitId : -1;
                    MSG_SendCmd4Dwords(68, dwPlayerGuid, gnAnvilInteractGUID, gnAnvilSlotUnitId, 3);
                }
            }
            bHandled = 1;
        }
        ++nButton;
    }
    while (nButton < gnPanelButtonCount);

    ANVIL_ClearButtonPresses();
    return bHandled;
}

//1.10f: D2Client.0x6FB24F70
//  Does nothing: `xor eax, eax; retn`, three bytes, a bare retn that purges nothing. No code calls it: the NPC menu and the anvil panel
//  register it as DIALOG_Create's dismiss callback, the gold entry dialog as its edit box's Enter/Escape callback, and none of them pushes an
//  argument or reads the 0. It is typed int __cdecl() as the original is, although the DIALOGCALLBACK typedef is __stdcall.
int32_t __cdecl USEITEM_NullCallback(void)
{
    return 0;
}

//1.10f: D2Client.0x6FB24F80
//  A mouse button went down over the panel; called by USEITEM_AnvilOnButtonDown. Two passes over the two-button table: clear every button's
//  press marker, then set it on the one the cursor is inside.
//  The original walks the second pass through gPanelButtonRight and reads the rectangle around it ([-1] left, [0] right, [1] top, [2] bottom),
//  the same record the first pass walks 40 bytes at a time. Written here through the four named fields (gAnvilPanelButtons[i].nLeft .. .nBottom).
void __fastcall USEITEM_AnvilOnPanelButtonDown(int32_t nX, int32_t nY)
{
    int32_t nCount;
    int32_t nButtonIndex;

    nCount = gnPanelButtonCount;
    ANVIL_ClearButtonPresses();

    nButtonIndex = 0;
    if (nCount > 0)
    {
        do
        {
            if (nX >= gAnvilPanelButtons[nButtonIndex].nLeft && nX <= gAnvilPanelButtons[nButtonIndex].nRight && nY >= gAnvilPanelButtons[nButtonIndex].nTop && nY <= gAnvilPanelButtons[nButtonIndex].nBottom)
            {
                SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
                //  The count is re-read here in the original, inside the hit arm and nowhere else, so a table that grew mid-loop would be walked
                //  further. Nothing writes it; reproduced anyway.
                nCount = gnPanelButtonCount;
                gAnvilPanelButtons[nButtonIndex].bPressed = 1;
                gAnvilPanelButtons[nButtonIndex].bState = 1;
            }
            ++nButtonIndex;
        }
        while (nButtonIndex < nCount);
    }
}


//  __FILE__ for this unit, the string both asserts push (0x6FB9073C).
static const char szUseitemCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UI\\useitem.cpp";

static_assert(UNIT_ITEM == 4,
              "USEITEM_AnvilTickPanel finds the slotted item with "
              "CUNIT_FindUnitByGuidAndType(gnAnvilSlotUnitId, 4)");


//1.10f: D2Client.0x6FB251E0
//  Esc or Space closes the panel: entries 5 and 6 of gAnvilMsgHandlers with flag 3, so it is bound to two key messages rather than mouse ones.
//  It acts only in states 1 and 2: with the panel closed or the transaction finished it claims the key and does nothing else. That is a
//  different guard from USEITEM_AnvilCloseOnInteractionStop's, which refuses state 3 alone and so still closes from state 0.
//  The rest is ANVIL_CloseAndNotify whole; its leading `if (state == 2)` is where the item comes back to the cursor, which is why the guard
//  admits both states rather than just 2.
void __stdcall USEITEM_AnvilOnCloseKey(SMSGHANDLER_PARAMS *pMsg)
{
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    if (gnAnvilState != 1 && gnAnvilState != 2)
        return;

    ANVIL_CloseAndNotify();
}


//1.10f: D2Client.0x6FB25A10
//  Opens the panel. Everything else closes first, the state goes to 1 (the item is on the cursor), the click throttle is stamped, the eight
//  message handlers go on, and UI_ANVIL is asked for.
//  On success the click region is fixed to the panel's own rectangle, (107, 91) - (222, 278), so clicks outside it do not reach the world.
//  On refusal it backs out by the short path: notify, reset, tear down, with no returning of the item to the cursor and no sound 1 (the
//  original uses only the two lower helpers here), so a panel that cannot open does not leave the client thinking it did.
void __cdecl USEITEM_AnvilOpenPanel()
{
    int32_t pRect[4];

    UI_CloseAllPanels(0, 0);
    gnAnvilState = 1;
    gdwAnvilLastClickTick = GetTickCount();
    WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gAnvilMsgHandlers, 8);
    INV_NpcShopEnterPanelMode19();

    if (UI_SetUIToggle(UI_ANVIL, UI_TURNON, 0))
    {
        GAME_ResetClickState();
        pRect[1] = 91;
        pRect[0] = 107;
        pRect[3] = 278;
        pRect[2] = 222;
        SELECT_SetClickRegion(pRect);
        return;
    }

    ANVIL_NotifyInteractionEnd();
    ANVIL_ResetAndTeardown();
    //  The last call of ANVIL_ResetAndTeardown is this one; the original leaves its answer in eax, which no caller reads.
    SELECT_SetClickRegion(NULL);
}


//1.10f: D2Client.0x6FB25BA0
//  Opens the panel in mode 1. Mode 0 arrives on the server's packet and carries gnAnvilInteractGUID; mode 1 is entered locally and cleans up
//  through NPCMENU_NpcCloseInteraction rather than by telling the server.
//  The argument is the NPC's GUID (the caller, NPCMENU_OnItemServiceClicked, loads it from sgUIState.NpcUnitGUID); it goes into
//  gnAnvilNpcGUID, which nothing reads.
void __fastcall USEITEM_AnvilOpenPanelLocal(D2UnitGUID nNpcGUID)
{
    gnAnvilPanelMode = 1;
    gnAnvilNpcGUID = nNpcGUID;
    USEITEM_AnvilOpenPanel();
}


//1.10f: D2Client.0x6FB25320
//  The panel's frame tick. It asserts that the panel is open (useitem.cpp:797) rather than returning, so calling it closed halts the client.
//  If the player has died or gone away the transaction is abandoned through the ordinary reset-and-teardown. Otherwise the panel draws and,
//  when the item is in the slot, the item is drawn on top of it.
//  The slot's box is 114 pixels tall with its top at 106, and the item is centred on x = 167 within it: (114 - height) >> 1 vertically, and
//  only when the item is shorter than 0x70. The cel size comes from the item's own graphic, and the draw happens only after both
//  GFXUTIL_FillGfxData and D2CMP_SpriteValidate have agreed.
//  The anim mode is changed to 0 before the draw and back to 4 after, which is the original's way of forcing the item's inventory frame.
void __cdecl USEITEM_AnvilTickPanel()
{
    D2UnitStrc *pItem;
    D2GfxDataStrc pParams;
    int32_t pClass;
    int32_t pnWidth;
    int32_t pnHeight;
    uint32_t nCenterOffsetY;

    if (!gnAnvilState)
    {
        FOG_DisplayAssert("0", szUseitemCpp, 797);
        exit(-1);
    }

    if (CUNIT_IsPlayerDeadOrMissing())
    {
        ANVIL_ResetAndTeardown();
        return;
    }

    USEITEM_AnvilDrawPanel();

    if (gnAnvilState != 2)
        return;

    pItem = CUNIT_FindUnitByGuidAndType(gnAnvilSlotUnitId, UNIT_ITEM);
    if (!pItem)
        return;

    memset(&pParams, 0, sizeof(pParams));
    if (!GFXUTIL_FillGfxData(&pParams, pItem, 0, 0, 0, &pClass, 1, -1))
        return;
    if (!D2CMP_SpriteValidate(&pParams, 0, 1) || !pParams.pCurrentCell)
        return;

    GFXUTIL_GetUnitCelSize(pItem, 0, 0, &pnWidth, &pnHeight);
    D2COMMON_UNITS_ChangeAnimMode(pItem, 0);
    nCenterOffsetY = 0;
    if ((uint32_t)pnHeight < 0x70)
        nCenterOffsetY = (uint32_t)(114 - pnHeight) >> 1;
    GFX_DrawInventoryItem(pItem, 167 - ((uint32_t)pnWidth >> 1), nCenterOffsetY + 106);
    D2COMMON_UNITS_ChangeAnimMode(pItem, 4);
}


//1.10f: D2Client.0x6FB257B0
//  The server packet that drives the panel. It switches on the byte at +5 and takes the guid from the dword at +1, so the packet is packed
//  and not aligned.
//      0     open in mode 0, with the guid from +1
//      1     close
//      4     the item is accepted: state 2, and the dialog goes if one is up
//      5     the server refuses what was held: clear the cursor item, sound 5 when the flag at +6 is set and 3 when it is not, then FALL
//            THROUGH into the close
//      6, 7  the same close plus one trailing NPCMENU_NpcCloseInteraction_Dup1
//      else  assert at useitem.cpp:852 and exit
//  Sub-commands 2 and 3 therefore halt the client outright.
//  BUG: case 5 does not null-check the player before reaching into its inventory, where every other arm of this family does. REACHABLE: only
//  if the player unit goes away between the request and the answer (USEITEM_AnvilTickPanel's CUNIT_IsPlayerDeadOrMissing arm handles that one
//  frame later). Reproduced.
void __fastcall USEITEM_AnvilOnPanelPacket(D2GSPacketSrv58 *pPacket)
{
    D2UnitStrc *pPlayer;
    D2C_Sounds nSoundId;

    switch (pPacket->nAction)
    {
    case 0u:
        gnAnvilPanelMode = 0;
        gnAnvilInteractGUID = pPacket->nUnitId;
        USEITEM_AnvilOpenPanel();
        return;

    case 5u:
        //  No null check on the player -- the original's own shape.
        pPlayer = CUNIT_GetPlayer();
        D2COMMON_INVENTORY_SetCursorItem(pPlayer->pInventory, NULL);
        nSoundId = pPacket->bSuccess ? SOUND_CURSOR_POINT_DROP : SOUND_CURSOR_ERROR;
        SOUNDINST_PlaySound(nSoundId, NULL, 0, 0, 0);
        //  Falls through into the close, exactly as the original does.
    case 1u:
        ANVIL_ResetAndTeardown();
        break;

    case 4u:
        gnAnvilState = 2;
        if (gpAnvilDialog)
        {
            DIALOG_Close(gpAnvilDialog);
            gpAnvilDialog = NULL;
        }
        return;

    case 6u:
    case 7u:
        ANVIL_ResetAndTeardown();
        NPCMENU_NpcCloseInteraction_Dup1();
        break;

    default:
        FOG_DisplayAssert("0", szUseitemCpp, 852);
        exit(-1);
    }
}


//  gAnvilPanelButtons (useitem.h) is the panel's two 40-byte button records, starting at 0x6FB90660 and followed at 0x6FB906B0 by
//  gnPanelButtonCount. The cel index is at +0x22, the state byte at +0x26 and the pressed byte at +0x27.
//  D2ExpRes patch sites in this unit: each is a jmp into the plugin, which recomputes the position from D2ExpRes.ini for 1024x768 and
//  1280x768 and jumps back; each is marked at the line it lands on below.
//      0x85097  QuestImbueInscribeSocketItemslotborders  USEITEM_AnvilOnButtonDown, at the item slot's left edge test
//      0x85558  QuestImbueInscribeSocketBackground       USEITEM_AnvilDrawPanel, at the background cel's X
//      0x85737  QuestImbueInscribeSocketStrings1         USEITEM_AnvilDrawPanel, at the jz after the text is split
//      0x8574C  QuestImbueInscribeSocketStrings2         USEITEM_AnvilDrawPanel, at the instruction lines' centring

static_assert(STR_IDX_4017_Upgrade == 4017,
              "button record 0's label field holds 4017, read out of "
              "0x6FB90660 -- and USEITEM_AnvilDrawPanel tests for it by name");


//1.10f: D2Client.0x6FB25000
//  A mouse button went down over the panel. Entries 1 and 2 of gAnvilMsgHandlers, so it takes both WM_LBUTTONDOWN and WM_RBUTTONDOWN and does
//  not distinguish them. It rate-limits itself to one action per 400 ms. void: the nine exits leave nine different things in eax (the elapsed
//  milliseconds when it refuses, a sound handle, a state, a pointer) and nothing reads any of them.
//  Inside the slot rectangle, an item on the cursor moves between the cursor and the anvil. What may go in depends on how the panel was opened:
//      mode 1   the vendor decides, by monstats id: 154 charsi with an imbueable item, 511 larzuk with a socketable one, 512 drehya with a
//               personalisable one. Anything else plays sound 3
//      mode 0   the item's items.txt dwCode must be `hst ` (the Horadric Staff; 544502632 is those four characters little-endian)
//  and either way the item must pass ITEMS_CheckItemFlag (useitem.cpp:528).
//  The swap here is not the shared helper: the cursor item is already known non-null, so there is no guard, and each arm leaves its sound
//  handle in eax on the way out, which nothing reads.
void __stdcall USEITEM_AnvilOnButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nTicksSinceClick;
    D2UnitStrc *pPlayer;
    D2UnitStrc *pCursorItem;
    D2ItemsTxt *pItemsTxt;
    D2C_Sounds nDropSound;
    int32_t nResult;

    nMouseX = pMsg->nXpos;
    nMouseY = pMsg->nYpos;
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    nTicksSinceClick = (int32_t)(GetTickCount() - gdwAnvilLastClickTick);
    if ((uint32_t)nTicksSinceClick < 0x190)
        return;
    gdwAnvilLastClickTick = GetTickCount();

    if (gnAnvilState == 1 || gnAnvilState == 2)
    {
        if (gnAnvilState == 1)
            nResult = INV_AnvilForwardLButtonDownToInventory(pMsg);
        else
            nResult = INV_AnvilOnInvCloseButtonPress(pMsg);
        if (nResult)
            return;
        USEITEM_AnvilOnPanelButtonDown(nMouseX, nMouseY);
    }

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;
    pCursorItem = D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory);
    if (!pCursorItem)
        return;

    //  D2ExpRes QuestImbueInscribeSocketItemslotborders (0x85097) lands on the `cmp edi, 7Bh` this line compiles to: the slot rect.
    if (nMouseX < 123 || nMouseX > 211 || nMouseY < 106 || nMouseY > 220)
        return;

    pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(pCursorItem->dwClassId);
    if (!pItemsTxt)
        return;

    if (!D2COMMON_ITEMS_CheckItemFlag(pCursorItem, IFLAG_IDENTIFIED, 528, szUseitemCpp))
    {
        return;
    }

    if (gnAnvilPanelMode == 1)
    {
        if ((NPCMENU_NpcGetVendorClassId() != 154 || !D2COMMON_ITEMS_IsImbueable(pCursorItem)) && (NPCMENU_NpcGetVendorClassId() != 511 || !D2COMMON_ITEMS_IsSocketable(pCursorItem))
            && (NPCMENU_NpcGetVendorClassId() != 512 || !D2COMMON_ITEMS_IsPersonalizable(pCursorItem)))
        {
            SOUNDINST_PlaySound(SOUND_CURSOR_ERROR, NULL, 0, 0, 0);
            return;
        }
    }
    //  `hst ` little-endian: the Horadric Staff, the only thing mode 0 accepts. Reached by raw offset in the original, so the +128 is a number
    //  and not a field.
    else if (gnAnvilPanelMode || *(uint32_t *)((uint8_t *)pItemsTxt + 128) != 544502632)
    {
        return;
    }

    nResult = gnAnvilState;
    if (gnAnvilState == 1)
    {
        gnAnvilState = 2;
        CURSOR_SetHeldUnit(NULL);
        gnAnvilSlotUnitId = pCursorItem->dwUnitId;
        nDropSound = ITEM_GetDropSoundNoFrame(pCursorItem);
        SOUNDINST_PlaySound(nDropSound, NULL, 0, 0, 0);
        return;
    }
    if (gnAnvilState == 2)
    {
        gnAnvilState = 1;
        gnAnvilSlotUnitId = 0;
        CURSOR_SetHeldUnit(pCursorItem);
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
        return;
    }
}


//1.10f: D2Client.0x6FB25500
//  Draws the panel. It draws nothing at all unless gnAnvilPanelMode is 0 or 1, which an unsigned compare against 1 decides.
//  A background cel chosen by the mode, then every button, then the hovered button's label in a filled box above it, then, in mode 1 only, the
//  instruction text split at 300 pixels and centred on x = 160.
//  Each button's pressed frame is the next cel after its normal one: `cel + (state != 0)` rather than a second table, from bState (+0x26) and
//  not bPressed.
//  The label the hover box shows is the record's own, except that Upgrade is rewritten three ways: to `ok` in mode 0, to Addsockets for larzuk
//  and to Personalize for drehya. So one record serves four transactions.
//  The hover test is skipped entirely when the cursor is past x 320 or in the bottom 48 pixels, which is the control panel's band.
void __cdecl USEITEM_AnvilDrawPanel()
{
    D2GfxDataStrc pData;
    int32_t nPrevFont;
    int32_t i;
    int32_t nMouseX;
    int32_t nMouseY;
    D2C_StringIndices nLabelStrId;
    D2C_StringIndices nInstructStrId;
    const Unicode *pwszText;
    int32_t nTextWidth;
    int32_t nLineHeight;
    int32_t nBoxHeight;
    D2SplittedTextStrc *pSplitText;
    D2SplittedTextStrc *pSplitNext;
    int32_t pLines;
    int32_t nY;

    nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT16);

    if ((uint32_t)gnAnvilPanelMode <= 1)
    {
        memset(&pData, 0, sizeof(pData));
        pData.nDirection = 0;
        if (gnAnvilPanelMode)
            pData.pCellFile = gpImgAnvilUpgrade;
        else
            pData.pCellFile = gpImgAnvilBackground;
        pData.nFrame = 0;
        //  D2ExpRes QuestImbueInscribeSocketBackground (0x85558) lands on the `push 10Eh` this 270 compiles to.
        D2GFX_TEXTURE_CelDraw(&pData, 105, 270, 0xFFFFFFFF, 5, NULL);

        pData.pCellFile = UI_GetBuySellButtonImage();
        for (i = 0; i < gnPanelButtonCount; ++i)
        {
            //  The cel index is +0x22 of the record and the state byte +0x26 (bState, not bPressed): mov cl, [esi+4] at 0x6FB25586.
            pData.nFrame = gAnvilPanelButtons[i].nCelIndex + (gAnvilPanelButtons[i].bState != 0);
            D2GFX_TEXTURE_CelDraw(&pData, gAnvilPanelButtons[i].nCelX, gAnvilPanelButtons[i].nCelY, 0xFFFFFFFF, 5, NULL);
        }

        nMouseX = CURSOR_GetMouseX();
        nMouseY = CURSOR_GetMouseY();
        if (nMouseX < 320 && nMouseY < gnScreenHeight - 48)
        {
            for (i = 0; i < gnPanelButtonCount; ++i)
            {
                if (nMouseX < gAnvilPanelButtons[i].nLeft || nMouseX > gAnvilPanelButtons[i].nRight || nMouseY < gAnvilPanelButtons[i].nTop || nMouseY > gAnvilPanelButtons[i].nBottom)
                {
                    continue;
                }

                nLabelStrId = gAnvilPanelButtons[i].wLabelStrId;
                if (nLabelStrId == STR_IDX_4017_Upgrade)
                {
                    if (!gnAnvilPanelMode)
                        nLabelStrId = STR_IDX_3401_ok;
                    if (NPCMENU_NpcGetVendorClassId() == 511)
                        nLabelStrId = STR_IDX_22748_Addsocketsui;
                    else if (NPCMENU_NpcGetVendorClassId() == 512)
                        nLabelStrId = STR_IDX_22749_Personalizeui;
                }

                pwszText = D2LANG_GetStringFromTblIndex(nLabelStrId);
                nTextWidth = D2WIN_10121_GetTextWidth(pwszText);
                nBoxHeight = D2WIN_10125() + 6;
                nLineHeight = D2WIN_10125();
                //  The box and the text are placed from the record's nX (+0x02) and nY (+0x06), not from its cel position.
                GFX_DrawFilledRect(gAnvilPanelButtons[i].nX - nTextWidth / 2 + 10, gAnvilPanelButtons[i].nY - nLineHeight - 42, nTextWidth + 12, nBoxHeight, 0, 2);
                D2WIN_10117_DrawText(pwszText, gAnvilPanelButtons[i].nX - nTextWidth / 2 + 16, gAnvilPanelButtons[i].nY - 38, 0, 0);
            }
        }

        if (gnAnvilPanelMode == 1)
        {
            nInstructStrId = STR_IDX_10076_upgraderestrictEx;
            if (NPCMENU_NpcGetVendorClassId() == 511)
                nInstructStrId = STR_IDX_22750_Addsocketsui2;
            else if (NPCMENU_NpcGetVendorClassId() == 512)
                nInstructStrId = STR_IDX_22747_Rename_Instruct;

            pwszText = D2LANG_GetStringFromTblIndex(nInstructStrId);
            pSplitText = D2WIN_10199(pwszText, &pLines, 300);
            //  D2ExpRes QuestImbueInscribeSocketStrings1 (0x85737) lands on the `jz` this test compiles to.
            if (pSplitText)
            {
                nY = 30;
                do
                {
                    nTextWidth = D2WIN_10121_GetTextWidth(pSplitText->wszLine);
                    //  D2ExpRes QuestImbueInscribeSocketStrings2 (0x8574C) lands on the `sar eax, 1` of this centring, with the 160 in the
                    //  register beside it.
                    D2WIN_10117_DrawText(pSplitText->wszLine, 160 - nTextWidth / 2, nY, 4, 0);
                    pSplitNext = pSplitText->pNextLine;
                    FOG_Free(pSplitText->wszLine, szUseitemCpp, 707, 0);
                    FOG_Free(pSplitText, szUseitemCpp, 708, 0);
                    nY += 20;
                    pSplitText = pSplitNext;
                }
                while (pSplitNext);
            }
        }
    }

    if (gpAnvilDialog)
        DIALOG_Draw(gpAnvilDialog);

    D2WIN_10127_SetFont((Font)nPrevFont);
}

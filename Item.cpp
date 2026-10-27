#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "Item.h"

//  Item.cpp -- the item unit of D2Client.dll 1.10: unhooking client item units and setting their item mode, their drop and use sounds, the
//  refresh of a unit's active items, the socket modifiers, the item lock (state 54), and the handlers of the item server packets (0x3F, 0x40,
//  0x42, 0x48, 0x7C, 0x7D, 0x92 and the item messages of packets 0x9C and 0x9D).
//  The original source file is UNIT\Item.cpp (the asserts in this unit name it). Address range 0x6FB2A880..0x6FB2FF18.

//  Assert strings of the original.
static const char szItemCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UNIT\\Item.cpp";
static const char szAssertPtUnit[] = "ptUnit";
static const char szAssertZero[] = "0";
static_assert(sizeof(szItemCpp) == 57 + 1 && sizeof(szAssertPtUnit) == 6 + 1
                  && sizeof(szAssertZero) == 1 + 1,
              "the three strings at 0x6FB90C4C, 0x6FB909A4 and 0x6FB74C04, "
              "measured in the DLL with probe_named_strings.py");

//  Layout checks. wDropSound and wUseSound of the items.txt, UniqueItems.txt and SetItems.txt records are adjacent words;
//  ITEM_GetUseSound tests the wrong one of each pair, as the original does.
static_assert(offsetof(D2ItemsTxt, wDropSound) == 0x124
                  && offsetof(D2ItemsTxt, wUseSound) == 0x126
                  && offsetof(D2ItemsTxt, nDropSfxFrame) == 0x128,
              "items.txt: the drop sound, the use sound and the frame, read at "
              "[eax+124h], [eax+126h] and [eax+128h] in ITEM_GetDropSound and "
              "ITEM_GetUseSound -- an adjacent pair a transcription could swap");
static_assert(offsetof(D2UniqueItemsTxt, wDropSound) == 0x84
                  && offsetof(D2UniqueItemsTxt, wUseSound) == 0x86
                  && offsetof(D2UniqueItemsTxt, nDropSfxFrame) == 0x88,
              "UniqueItems.txt: the same three, at [ecx+84h], [eax+86h] and "
              "[ecx+88h]");
static_assert(offsetof(D2SetItemsTxt, wDropSound) == 0x82
                  && offsetof(D2SetItemsTxt, wUseSound) == 0x84
                  && offsetof(D2SetItemsTxt, nDropSfxFrame) == 0x86,
              "SetItems.txt: the same three, at [ecx+82h], [eax+84h] and "
              "[ecx+86h] -- two bytes below the unique record's, which is why "
              "the two arms cannot share a constant");

//  Record strides of UniqueItems.txt (332 bytes) and SetItems.txt (440 bytes).
static_assert(sizeof(D2UniqueItemsTxt) == 0x14C && sizeof(D2SetItemsTxt) == 0x1B8,
              "the record strides the index chains at 0x6FB2AB21 and 0x6FB2AB8F "
              "compute; Binary Ninja folds them to 0x14c and 0x1b8");

//  Literals the original carries as bare numbers: ITEM_GetDropSound and ITEM_GetUseSound compare the quality with 7 and 5,
//  ITEM_DetachItemFromUnit pushes 0x100 and ITEM_SetClientItemMode pushes 0x10.
static_assert(ITEMQUAL_UNIQUE == 7 && ITEMQUAL_SET == 5,
              "the two quality arms, as the `cmp eax, 7` / `cmp eax, 5` pair "
              "has them");
static_assert(IFLAG_BROKEN == 0x100 && IFLAG_IDENTIFIED == 0x10,
              "the two item flags this batch passes, as the `push 100h` at "
              "0x6FB2A8A2 and the `push 10h` at 0x6FB2AA26 have them");
static_assert(IMODE_STORED == 0 && IMODE_EQUIP == 1 && IMODE_INBELT == 2
                  && IMODE_ONGROUND == 3 && IMODE_ONCURSOR == 4
                  && IMODE_DROPPING == 5 && IMODE_SOCKETED == 6,
              "the seven item modes, which are the seven arms of the jump "
              "table at 0x6FB2A927 and its bound `cmp eax, 6`");


//1.10f: D2Client.0x6FB2A880
//  Unhooks an item from the unit that holds it. The statlist is expired when the item is not broken or when bForceExpire is set (two jumps to
//  different targets, an `||`). The left-hand weapon is fetched and the result discarded, because the original makes the call.
//  ITEMS_CheckItemFlag takes the original's source line (150) and file name, which D2Common logs when the flag test is made.
void __fastcall ITEM_DetachItemFromUnit(D2UnitStrc *pItem, D2UnitStrc *pOwner, int32_t bForceExpire, int32_t bClampVitals)
{
    if (pOwner)
    {
        if (pOwner->dwUnitType == UNIT_PLAYER)
            D2COMMON_INVENTORY_GetLeftHandWeapon(pOwner->pInventory);

        if (!D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_BROKEN, 150, szItemCpp) || bForceExpire)
        {
            D2COMMON_STATLIST_ExpireUnitStatlist(pOwner, pItem);
        }

        if (bClampVitals)
            D2COMMON_STATLIST_ClampStaminaManaHP(pOwner);
    }
}


//1.10f: D2Client.0x6FB2A8E0
//  Puts a client item unit into an item mode and does the per-mode work (nItemMode < 7; the jump table has all seven arms).
//  The field at +0x10 is read as dwItemMode, since every unit here is an item. In the DROPPING arm nY is reused as the out-parameter of
//  ITEM_GetDropSound (preset to 12, then the frame the drop sound plays on), so after the call it is no longer a coordinate.
//  The flags are written as the bytes have them: and 0xFFFFFFF3 (clears bits 2 and 3), then or 0x20 (sets bit 5).
//      3 ONGROUND   static path, block collision, join the room, flag bit 1 set
//      4 ONCURSOR   SOUNDINST_PlaySound(235) (item_pickup), then flag bit 1 cleared
//      5 DROPPING   the placement of 3, flag bit 1 cleared, SOUNDINST_PlaySound(216) (item_flippy), then the item's own drop sound at volume 180
//      0/1/2/6      no placement; flag bit 1 cleared and nothing else
//  Leaving mode 3 or 5 for anything else frees the collision path first, and that is the only cleanup.
void __fastcall ITEM_SetClientItemMode(D2UnitStrc *pItem, D2ActiveRoomStrc *pRoom, int32_t nX, int32_t nY, int32_t nItemMode, int32_t bInitStats)
{
    if (nItemMode >= 7)
        return;

    if ((pItem->dwItemMode == IMODE_DROPPING || pItem->dwItemMode == IMODE_ONGROUND) && nItemMode != IMODE_DROPPING && nItemMode != IMODE_ONGROUND)
    {
        D2COMMON_UNITS_FreeCollisionPath(pItem);
    }

    pItem->dwItemMode = nItemMode;
    D2COMMON_UNITROOM_UpdatePath(pItem);

    //  The switch re-reads the field it has just written (0x6FB2A91B) rather than reusing the register holding nItemMode.
    switch (pItem->dwItemMode)
    {
    case IMODE_STORED:
    case IMODE_EQUIP:
    case IMODE_INBELT:
    case IMODE_SOCKETED:
        goto CLEAR_GROUND_FLAG;

    case IMODE_ONGROUND:
        D2COMMON_UNITS_InitializeStaticPath(pItem, pRoom, nX, nY);
        D2COMMON_UNITS_BlockCollisionPath(pItem, pRoom, nX, nY);
        D2COMMON_UNITROOM_AddUnitToRoom(pItem, pRoom);
        pItem->dwFlags |= 2u;
        D2COMMON_UNITS_SetAnimStartFrame(pItem);
        goto INIT_STATS;

    case IMODE_ONCURSOR:
        SOUNDINST_PlaySound(SOUND_ITEM_PICKUP, NULL, 0, 0, 0);
        //  Shared tail, also entered by the four quiet modes (0x6FB2A9FC).
    CLEAR_GROUND_FLAG:
        pItem->dwFlags &= ~2u;
        goto INIT_STATS;

    case IMODE_DROPPING:
    {
        D2COMMON_UNITS_InitializeStaticPath(pItem, pRoom, nX, nY);
        D2COMMON_UNITS_BlockCollisionPath(pItem, pRoom, nX, nY);
        D2COMMON_UNITROOM_AddUnitToRoom(pItem, pRoom);
        pItem->dwFlags &= ~2u;
        D2COMMON_UNITS_SetAnimStartFrame(pItem);

        //  nY stops being a coordinate here: it is the out-parameter of ITEM_GetDropSound.
        nY = 12;
        D2C_Sounds nDropSound = ITEM_GetDropSound(pItem, &nY);

        SOUNDINST_PlaySound(SOUND_ITEM_FLIPPY, pItem, 0, 0, 0);
        if (nDropSound > 0)
        {
            int32_t nHandle = SOUNDINST_PlaySound(nDropSound, pItem, nY, 0, 0);
            if (nHandle)
                SOUNDINST_SetVolumeByHandle(nHandle, 180);
        }
        goto INIT_STATS;
    }

    default:
        return;
    }

INIT_STATS:
    pItem->dwFlags = (pItem->dwFlags & 0xFFFFFFF3) | 0x20;

    if (bInitStats)
    {
        D2COMMON_ITEMS_SetItemFlag(pItem, IFLAG_IDENTIFIED, 1);
        D2COMMON_STATLIST_AllocStatListEx(pItem, 0, NULL, NULL);

        D2StatListStrc *pStatList = D2COMMON_STATLIST_AllocStatList(NULL, 0x40u, 0, UNIT_ITEM, pItem->dwUnitId);
        D2COMMON_10475_PostStatToStatList(pItem, pStatList, 1);

        //  An inventory only for a socketable item: a plain potion has no pInventory, a socketed sword does.
        if (D2COMMON_ITEMS_CheckIfSocketable(pItem) && !pItem->pInventory)
            D2COMMON_INVENTORY_AllocInventory(NULL, pItem);
    }
}


//1.10f: D2Client.0x6FB2AA90
//  The sound an item makes when it lands, and the frame to play it on. items.txt first, then the quality overrides it: ITEMQUAL_UNIQUE takes
//  UniqueItems.txt and ITEMQUAL_SET takes SetItems.txt, each only when that record's own wDropSound is positive. pnDropSfxFrame may be NULL;
//  when it is not, it is preset to 12 and overwritten only by a non-zero table value.
//  wDropSound is unsigned __int16 but every read of it is movsx and a signed compare, so the (int16_t) casts are load-bearing: without them
//  a row carrying 0xFFFF would pass the test. The unique arm calls ITEMS_GetFileIndex twice (two identical tests, the second answer is used);
//  the set arm does not, and neither does either arm of ITEM_GetUseSound.
D2C_Sounds __fastcall ITEM_GetDropSound(D2UnitStrc *pItem, int32_t *pnDropSfxFrame)
{
    D2C_Sounds nDropSound = SOUND_NONE;

    if (pnDropSfxFrame)
        *pnDropSfxFrame = 12;

    D2ItemsTxt *pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(pItem ? pItem->dwClassId : -1);

    if (pItemsTxt && (int16_t)pItemsTxt->wDropSound > 0)
    {
        if (pnDropSfxFrame && pItemsTxt->nDropSfxFrame)
            *pnDropSfxFrame = pItemsTxt->nDropSfxFrame;
        nDropSound = (D2C_Sounds)(int16_t)pItemsTxt->wDropSound;
    }

    if (D2COMMON_ITEMS_GetItemQuality(pItem) != ITEMQUAL_UNIQUE)
    {
        if (D2COMMON_ITEMS_GetItemQuality(pItem) == ITEMQUAL_SET)
        {
            int32_t nFileIndex = D2COMMON_ITEMS_GetFileIndex(pItem);
            if (nFileIndex >= 0 && nFileIndex < sgptDataTables->nSetItemsTxtRecordCount)
            {
                D2SetItemsTxt *pSetTxt = &sgptDataTables->pSetItemsTxt[nFileIndex];
                if (pSetTxt && (int16_t)pSetTxt->wDropSound > 0)
                {
                    if (pnDropSfxFrame && pSetTxt->nDropSfxFrame)
                        *pnDropSfxFrame = pSetTxt->nDropSfxFrame;
                    return (D2C_Sounds)(int16_t)pSetTxt->wDropSound;
                }
            }
        }
        return nDropSound;
    }

    //  The doubled call, as the original has it.
    if (D2COMMON_ITEMS_GetFileIndex(pItem) < 0)
        return nDropSound;

    int32_t nFileIndex = D2COMMON_ITEMS_GetFileIndex(pItem);
    if (nFileIndex < 0)
        return nDropSound;
    if (nFileIndex >= sgptDataTables->nUniqueItemsTxtRecordCount)
        return nDropSound;

    D2UniqueItemsTxt *pUniqueTxt = &sgptDataTables->pUniqueItemsTxt[nFileIndex];
    if (!pUniqueTxt || (int16_t)pUniqueTxt->wDropSound <= 0)
        return nDropSound;

    if (pnDropSfxFrame && pUniqueTxt->nDropSfxFrame)
        *pnDropSfxFrame = pUniqueTxt->nDropSfxFrame;

    return (D2C_Sounds)(int16_t)pUniqueTxt->wDropSound;
}


//1.10f: D2Client.0x6FB2ABD0
//  The item arm of CUNIT_DestroyUnit's unit-type switch. Frees the gfx info; then, only for a socketable item, the socket contents and the
//  inventory struct that held them; then, while gpAct is still up, the collision footprint and the room membership (guarded by
//  UNITROOM_IsUnitInRoom, since an item on the cursor or inside a container is in no room).
void __fastcall ITEM_FreeUnitData(D2UnitStrc *pItem)
{
    GFX_FreeUnitGfxInfo(pItem);

    if (D2COMMON_ITEMS_CheckIfSocketable(pItem))
    {
        CUNIT_FreeUnitInventory(pItem, 0);
        D2COMMON_INVENTORY_FreeInventory(pItem->pInventory);
        pItem->pInventory = NULL;
    }

    if (gpAct)
    {
        D2COMMON_UNITS_FreeCollisionPath(pItem);

        D2ActiveRoomStrc *pRoom = D2COMMON_UNITS_GetRoom(pItem);
        if (D2COMMON_UNITROOM_IsUnitInRoom(pRoom, pItem))
            D2COMMON_UNITROOM_RemoveUnitFromRoom(pItem);
    }
}

//  The overlay kind GFX_AddOverlay takes.
static_assert(OVERLAYTYPE_DELAYED == 6, "ITEM_OnUnitModeUpdate: `push 6` (0x6FB2AC7F)");
static_assert(OVERLAY_ITEMGLEAM == 70,
              "ITEM_OnUnitModeUpdate and ITEM_ItemMsg15_UpdateStats: `mov edx, 46h` "
              "(0x6FB2AC81, 0x6FB2F98C)");


//1.10f: D2Client.0x6FB2AC30
//  The per-frame hook for an item unit, called from UNITMODE_UpdateUnit: lets an item on the ground finish its flippy animation and settle.
//  Overlay 70 is added from two places that share one block: the DROPPING arm when the animation reaches the end of its cycle (the mode becomes
//  ONGROUND), and the ONGROUND arm, which sets flag bit 1 and jumps into the middle of the DROPPING arm (0x6FB2AC75) when the item does not
//  already carry the overlay. Hence the label. The overlay call passes nRandomStartFrames = 8: the gleam starts within its first 8 frames.
void __fastcall ITEM_OnUnitModeUpdate(D2UnitStrc *pUnit)
{
    if (!pUnit)
    {
        FOG_DisplayAssert(szAssertPtUnit, szItemCpp, 840);
        exit(-1);
    }

    switch (pUnit->dwItemMode)
    {
    case IMODE_STORED:
    case IMODE_EQUIP:
    case IMODE_INBELT:
    case IMODE_ONCURSOR:
    case IMODE_SOCKETED:
        pUnit->dwFlags &= ~2u;
        return;

    case IMODE_ONGROUND:
        pUnit->dwFlags |= 2u;
        if (GFX_HasOverlay(pUnit, 70))
            return;
        goto ADD_SETTLE_OVERLAY;

    case IMODE_DROPPING:
        if (!D2COMMON_UNITS_IsAtEndOfFrameCycle(pUnit))
        {
            D2COMMON_UNITS_UpdateFrame(pUnit);
            return;
        }
        pUnit->dwItemMode = IMODE_ONGROUND;
    ADD_SETTLE_OVERLAY:
        GFX_AddOverlay(pUnit, OVERLAY_ITEMGLEAM, OVERLAYTYPE_DELAYED, 8, 0, 0, 8, 0);
        return;

    default:
        //  The mode is bounded by `cmp eax, 6` above, so this arm needs a value outside 0..6 in a field this unit's own writer fills.
        //  The assert message is the single character "0".
        FOG_DisplayAssert(szAssertZero, szItemCpp, 866);
        exit(-1);
    }
}


//1.10f: D2Client.0x6FB2AD10
//  The item arm of the client action dispatch (unit type 4). Only action 2 (CLTACTION_INTERACT) does anything: it changes the item's anim mode
//  and sets or clears dwFlags bit 1 from bSet. Every other action on an item is ignored. The action test is `sub ecx, 2`, so nothing reads
//  nAction later.
void __fastcall ITEM_DoClientAction(D2C_ClientActions nAction, D2UnitStrc *pItem, int32_t bSet, int32_t nMode)
{
    if (nAction == CLTACTION_INTERACT)
    {
        D2COMMON_UNITS_ChangeAnimMode(pItem, nMode);

        if (pItem)
        {
            if (bSet)
                pItem->dwFlags |= 2u;
            else
                pItem->dwFlags &= ~2u;
        }
    }
}


//1.10f: D2Client.0x6FB2D040
//  Stashes ITEMS_GetWeaponClassId(pItem) into one of the two hand slots: BODYLOC_RARM (4) into gnWeaponClassRArm, BODYLOC_LARM (5) into
//  gnWeaponClassLArm, and nowhere for any other body location. ITEM_ItemMsg09_EquipOnPlayer passes the
//  equip message's own nBodyLoc. Nothing in D2Client reads either slot.
//  void: the weapon class is left in eax either way, and no caller reads it.
void __fastcall ITEM_CacheWeaponClassForBodyLoc(D2UnitStrc *pItem, uint8_t nBodyLoc)
{
    uint16_t nWeaponClassId = D2COMMON_ITEMS_GetWeaponClassId(pItem);

    if (nBodyLoc == 4)
        gnWeaponClassRArm = nWeaponClassId;
    else if (nBodyLoc == 5)
        gnWeaponClassLArm = nWeaponClassId;

}


//1.10f: D2Client.0x6FB2E590
//  ITEM_GetDropSound(pItem, NULL): the sound id without the frame.
D2C_Sounds __fastcall ITEM_GetDropSoundNoFrame(D2UnitStrc *pItem)
{
    return ITEM_GetDropSound(pItem, NULL);
}


//1.10f: D2Client.0x6FB2E5A0
//  The sound an item makes when it is used. A copy of ITEM_GetDropSound without the frame out-parameter: items.txt first, then UniqueItems.txt for
//  ITEMQUAL_UNIQUE and SetItems.txt for ITEMQUAL_SET, each record overriding the one before it.
D2C_Sounds __fastcall ITEM_GetUseSound(D2UnitStrc *pItem)
{
    D2C_Sounds nUseSound = SOUND_NONE;

    D2ItemsTxt *pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(pItem ? pItem->dwClassId : -1);

    //  BUG: every one of the three arms tests wDropSound and then returns wUseSound (arm 1 of 3: items.txt, 0x6FB2E5BC / 0x6FB2E5C6). The guard
    //  was not changed when the function was copied from ITEM_GetDropSound, where guard and value are the same field. An item with a usesound and
    //  no dropsound is silent when used; one with a dropsound and no usesound answers sound id 0. LATENT: no stock row has usesound set and
    //  dropsound blank, so 1.10's data never trips it; a mod that fills usesound alone will.
    if (pItemsTxt && (int16_t)pItemsTxt->wDropSound > 0)
        nUseSound = (D2C_Sounds)(int16_t)pItemsTxt->wUseSound;

    if (D2COMMON_ITEMS_GetItemQuality(pItem) == ITEMQUAL_UNIQUE)
    {
        int32_t nFileIndex = D2COMMON_ITEMS_GetFileIndex(pItem);
        if (nFileIndex >= 0 && nFileIndex < sgptDataTables->nUniqueItemsTxtRecordCount)
        {
            D2UniqueItemsTxt *pUniqueTxt = &sgptDataTables->pUniqueItemsTxt[nFileIndex];
            //  BUG: the same guard on wDropSound, arm 2 of 3 (UniqueItems.txt).
            if (pUniqueTxt && (int16_t)pUniqueTxt->wDropSound > 0)
                return (D2C_Sounds)(int16_t)pUniqueTxt->wUseSound;
        }
    }
    else if (D2COMMON_ITEMS_GetItemQuality(pItem) == ITEMQUAL_SET)
    {
        int32_t nFileIndex = D2COMMON_ITEMS_GetFileIndex(pItem);
        if (nFileIndex >= 0 && nFileIndex < sgptDataTables->nSetItemsTxtRecordCount)
        {
            D2SetItemsTxt *pSetTxt = &sgptDataTables->pSetItemsTxt[nFileIndex];
            //  BUG: the same guard on wDropSound, arm 3 of 3 (SetItems.txt).
            if (pSetTxt && (int16_t)pSetTxt->wDropSound > 0)
                return (D2C_Sounds)(int16_t)pSetTxt->wUseSound;
        }
    }

    return nUseSound;
}


//1.10f: D2Client.0x6FB2E680
//  Zeroes both hand slots. Its caller PLAYER_DoClientAction calls it when the player goes back to a bare-handed state. __cdecl with no
//  arguments; the `xor eax, eax` at 0x6FB2E680 is the zero being stored, not a return value.
void __cdecl ITEM_ClearWeaponClassCache()
{
    gnWeaponClassRArm = 0;
    gnWeaponClassLArm = 0;
}


//  The item cursor and display-state strings.
static const char szAssertPtItem[] = "ptItem";
static const char szAssertHItem[] = "hItem";
static_assert(sizeof(szAssertPtItem) == 6 + 1 && sizeof(szAssertHItem) == 5 + 1,
              "the two strings at 0x6FB90F3C and 0x6FB90BC0, measured in the DLL");

//  Constants used by the item lock and the charged-skill code below.
static_assert(STATE_UNINTERRUPTABLE == 54,
              "states.txt `uninterruptable`, as the `push 36h` at 0x6FB2FAF6, "
              "0x6FB2FB33 and 0x6FB2E915 has it");
static_assert(STAT_ITEM_CHARGED_SKILL == 0xCC,
              "ItemStatCost.txt 204 `item_charged_skill`, as the `push 0CCh` "
              "at 0x6FB2E6C6 has it");
static_assert(IFLAG_TARGETING == 4 && IFLAG_NOEQUIP == 0x4000,
              "the two flags stat 70 clears, as the `push 4` at 0x6FB2E830 and "
              "the `push 4000h` at 0x6FB2E850 have them");
static_assert(UI_INVENTORY == 1 && UI_TURNON == 0,
              "the panel the cursor scan opens, as the `mov ecx, 1` and the "
              "`xor edx, edx` at 0x6FB2E8C1 have it");
static_assert(UNIT_TYPES_COUNT == 6,
              "the out-of-range unit type ITEM_SetUninterruptableState uses as "
              "its no-unit sentinel -- `mov esi, 6` at 0x6FB2FB46");


//1.10f: D2Client.0x6FB2C2B0
//  Reads a unit out of a small message (the type is one byte at +1 and the GUID a dword at +3, an unaligned pair: the signature of a packet body)
//  and refreshes its items. ITEM_RefreshUnitFromPacket below is byte-identical; the image holds two definitions, so both are written out.
void __fastcall ITEM_RefreshUnitFromMsg(const void *pMsg)
{
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(*(uint32_t *)((const char *)pMsg + 3), (D2C_UnitTypes)*((const uint8_t *)pMsg + 1));

    if (pUnit)
        ITEM_RefreshActiveItems(pUnit);
}


//1.10f: D2Client.0x6FB2C2D0
//  The whole body of server packet 0x48, byte for byte the function above. A unit the client has not been told about is not in the table and
//  is silently ignored.
void __fastcall ITEM_RefreshUnitFromPacket(const void *pPacket)
{
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(*(uint32_t *)((const char *)pPacket + 3), (D2C_UnitTypes)*((const uint8_t *)pPacket + 1));

    if (pUnit)
        ITEM_RefreshActiveItems(pUnit);
}


//1.10f: D2Client.0x6FB2C2F0
//  A five-byte jump thunk to ITEM_RefreshActiveItems. It has its own address, so it is its own function.
void __fastcall j_ITEM_RefreshActiveItems(D2UnitStrc *pUnit)
{
    ITEM_RefreshActiveItems(pUnit);
}


//1.10f: D2Client.0x6FB2DE80
//  Sets an item's client-side display state -- its room, its position and its animation mode -- and then its inventory page.
//  The room lookup is guarded twice: if (bResolveRoom && (nX || nY)) pRoom = CUNIT_FindRoomForCoords(nX, nY). None of the
//  out-of-line calls sets bResolveRoom (they pass 0 for the flag, for nY and for nX); the copy inlined in ITEM_ItemMsg02_DropToGround passes 1.
//  Both coordinates are tested as words and masked to sixteen bits at every use, so both are uint16_t.
void __fastcall ITEM_SetItemDisplayState(D2UnitStrc *pItem, uint16_t nX, uint16_t nY, int32_t nAnimMode, int32_t bResolveRoom, int32_t nStorePage)
{
    if (!pItem)
    {
        FOG_DisplayAssert(szAssertPtItem, szItemCpp, 172);
        exit(-1);
    }

    D2ActiveRoomStrc *pRoom = NULL;
    if (bResolveRoom && (nX || nY))
        pRoom = CUNIT_FindRoomForCoords(nX, nY);

    ITEM_SetClientItemMode(pItem, pRoom, nX, nY, nAnimMode, 0);
    D2COMMON_ITEMS_SetInvPage(pItem, (uint8_t)nStorePage);
}


//1.10f: D2Client.0x6FB2E690
//  Sets the remaining charges on a charged-skill stat, searching the unit first and then, recursively, every item in its inventory.
//  Returns 1 as soon as one is found. The layer is the packed pair (nSkillLevel & sgptDataTables->nShiftedStuff) + (nSkillId << nStuff).
//  The value is packed too: high byte maximum, low byte current. A high byte of zero means "not a charged skill" (the lower bound is > 0). The
//  upper bound (<= 255) cannot fail while the stat is 16 bits wide (item_charged_skill has Save Bits = 16 in stock ItemStatCost.txt).
//  The clamp is branchless: nCharges <= 0 ? 0 : nCharges. It returns zero by returning the exhausted cursor: the inventory walk ends
//  `if (!pNextItem) return pNextItem;`, and the int return and the walk's pointer share one register.
int32_t __fastcall ITEM_SetChargedSkillCharges(D2UnitStrc *pUnit, int32_t nSkillId, int32_t nSkillLevel, int32_t nCharges)
{
    const int32_t nLayer = (nSkillLevel & sgptDataTables->nShiftedStuff) + (nSkillId << sgptDataTables->nStuff);

    D2StatListStrc *pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndFlag(pUnit, 64);
    if (pStatList)
    {
        int32_t nMaxCharges;
        while (1)
        {
            nMaxCharges = D2COMMON_STATLIST_GetStatValue(pStatList, STAT_ITEM_CHARGED_SKILL, (uint16_t)nLayer);
            if (nMaxCharges)
                break;

            pStatList = D2COMMON_STATLIST_GetStatListFromFlag(pStatList, 64);
            if (!pStatList)
                goto WALK_THE_INVENTORY;
        }

        const int32_t nMaxChargesHigh = nMaxCharges >> 8;
        if (nMaxChargesHigh > 0 && nMaxChargesHigh <= 255)
        {
            int32_t nNewCharges = nCharges <= 0 ? 0 : nCharges;
            if (nNewCharges >= nMaxChargesHigh)
                nNewCharges = (nNewCharges & ~0xFF) | (nMaxChargesHigh & 0xFF);

            D2COMMON_STATLIST_SetBaseStat2(pStatList, STAT_ITEM_CHARGED_SKILL, (uint8_t)nNewCharges + (nMaxChargesHigh << 8), (uint16_t)nLayer, pUnit);
            return 1;
        }
    }
    else
    {
    WALK_THE_INVENTORY:
        D2InventoryStrc *pInventory = pUnit->pInventory;
        if (pInventory)
        {
            D2UnitStrc *pItem = D2COMMON_INVENTORY_GetFirstItem(pInventory);
            if (pItem)
            {
                while (1)
                {
                    D2UnitStrc *pItemUnit = D2COMMON_INVENTORY_UnitIsItem(pItem);
                    if (ITEM_SetChargedSkillCharges(pItemUnit, nSkillId, nSkillLevel, nCharges))
                    {
                        break;
                    }

                    pItem = D2COMMON_INVENTORY_GetNextItem(pItem);
                    if (!pItem)
                        return (int32_t)(UINT_PTR)pItem;
                }
                return 1;
            }
        }
    }

    return 0;
}


//1.10f: D2Client.0x6FB2E780
//  Applies a single stat change to an item. Its caller SCMD_HandleSrv3E_ItemStatUpdate unpacks a buffer into (nStatId, nValue) pairs and calls this
//  once per pair.
//      204  item_charged_skill: unpack the layer into a skill id and a level and hand it to ITEM_SetChargedSkillCharges; the stat is not set
//           here, the charge search writes it
//      70   clears IFLAG_TARGETING and IFLAG_NOEQUIP when the new value is positive, each behind its own ITEMS_CheckItemFlag
//      any  STATLIST_SetUnitStat when bSetStat == 1, then a full ITEM_RefreshActiveItems of the player, not of the item
//  The unpacking inverts the packing of ITEM_SetChargedSkillCharges: nSkillId = (uint16_t)nStatParam >> (uint8_t)nStuff,
//  nSkillLevel = (uint16_t)nStatParam & nShiftedStuff. CUNIT_GetPlayer() is called up to three times on one path (0x6FB2E7A6, 0x6FB2E801,
//  0x6FB2E80F); nothing caches it. The two ITEMS_CheckItemFlag calls pass the original's source lines 2572 and 2576.
void __fastcall ITEM_OnItemStatUpdate(uint32_t dwItemGUID, uint8_t bSetStat, int32_t nStatId, int32_t nValue, int32_t nStatParam)
{
    D2UnitStrc *pItem = CUNIT_FindUnitByGuidAndType(dwItemGUID, UNIT_ITEM);
    if (!pItem)
        return;

    if (nStatId == STAT_ITEM_CHARGED_SKILL)
    {
        if (CUNIT_GetPlayer())
        {
            ITEM_SetChargedSkillCharges(pItem, (uint16_t)nStatParam >> (uint8_t)sgptDataTables->nStuff, (uint16_t)nStatParam & sgptDataTables->nShiftedStuff, (uint8_t)nValue);
        }
        return;
    }

    if (bSetStat == 1)
        D2COMMON_STATLIST_SetUnitStat(pItem, (D2C_ItemStats)nStatId, nValue, 0);

    if (!CUNIT_GetPlayer())
        return;

    if (nStatId == 70)
    {
        if (nValue > 0)
        {
            if (D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_TARGETING, 2572, szItemCpp))
                D2COMMON_ITEMS_SetItemFlag(pItem, IFLAG_TARGETING, 0);

            if (D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_NOEQUIP, 2576, szItemCpp))
                D2COMMON_ITEMS_SetItemFlag(pItem, IFLAG_NOEQUIP, 0);
        }
    }
    else
    {
        ITEM_RefreshActiveItems(CUNIT_GetPlayer());
    }
}


//1.10f: D2Client.0x6FB2E880
//  Server packet 0x3F: the server tells the client which item the cursor is carrying, and with which skill.
//  The two cursor scans over gaItemCursors are not the same loop. With nSkill != 0xFFFF the scan breaks out once UI_SetUIToggle has opened the
//  panel (0x6FB2E8CA); with nSkill == 0xFFFF it falls on to the next iteration (0x6FB2E984). The difference is INERT on this data: the toggle
//  turns UI_INVENTORY on, so later iterations find UI_GetUiWindowStateRaw true and toggle nothing; it would show only if the same cursor value
//  appeared twice in the table or the toggle could fail. The cursor setter takes the unit first and the frame second:
//  CURSOR_SetBuysellCursor(pUnit, nFrame).
//  The nCursor == 0xFF arm carries ITEM_ClearUninterruptableState's body with its assert dropped (the pointer is known non-null there);
//  the original inlines it here.
void __fastcall ITEM_OnCursorItemSkill(D2GSPacketSrv3F *pPacket)
{
    if (pPacket->nSkill != 0xFFFF)
    {
        //  The scan that breaks once it has opened the panel.
        for (uint32_t i = 0; i < (uint32_t)gnItemCursorUIStates; ++i)
        {
            if (pPacket->nCursor == gaItemCursors[i] && !UI_GetUiWindowStateRaw(UI_INVENTORY))
            {
                UI_SetUIToggle(UI_INVENTORY, UI_TURNON, 0);
                break;
            }
        }

        D2UnitStrc *pItem = CUNIT_FindUnitByGuidAndType(pPacket->dwItemId, UNIT_ITEM);
        if (pItem)
            CURSOR_SetBuysellCursor(pItem, pPacket->nCursor);

        return;
    }

    if (pPacket->nCursor == 0xFF)
    {
        D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(pPacket->dwItemId, UNIT_ITEM);
        if (pUnit)
        {
            D2COMMON_ITEMS_SetItemFlag(pUnit, IFLAG_TARGETING, 0);

            ITEM_ClearUninterruptableState(pUnit);
        }

        CURSOR_SetHeldUnit(NULL);
        return;
    }

    //  The scan that does not break: the one difference from the arm above.
    for (uint32_t i = 0; i < (uint32_t)gnItemCursorUIStates; ++i)
    {
        if (pPacket->nCursor == gaItemCursors[i] && !UI_GetUiWindowStateRaw(UI_INVENTORY))
        {
            UI_SetUIToggle(UI_INVENTORY, UI_TURNON, 0);
        }
    }

    D2UnitStrc *pItem = CUNIT_FindUnitByGuidAndType(pPacket->dwItemId, UNIT_ITEM);
    if (pItem)
    {
        D2COMMON_ITEMS_SetItemFlag(pItem, IFLAG_TARGETING, 1);
        CURSOR_SetBuysellCursor(pItem, pPacket->nCursor);
    }
}


//1.10f: D2Client.0x6FB2E9D0
//  The whole body of server packet 0x40: look the item up by the GUID at +1 and hand the two dwords at +5 and +9 to ITEMS_SetItemFlag.
//  Unlike packet 0x48, the unit type is not in the packet: the lookup is hard-coded to UNIT_ITEM, so a GUID that belongs to something else misses.
void __fastcall ITEM_SetItemFlagFromPacket(const void *pPacket)
{
    D2UnitStrc *pItem = CUNIT_FindUnitByGuidAndType(*(uint32_t *)((const char *)pPacket + 1), UNIT_ITEM);

    if (pItem)
    {
        D2COMMON_ITEMS_SetItemFlag(pItem, *(D2C_ItemFlags *)((const char *)pPacket + 5), *(int32_t *)((const char *)pPacket + 9));
    }
}


//1.10f: D2Client.0x6FB2FAD0
//  The release half of the item lock: find the state-54 statlist, toggle the state off, detach the list, free it, and refresh the unit's
//  animation rate. A unit that did not carry the state is left alone. The original inlines this body in many of the item handlers.
void __fastcall ITEM_ClearUninterruptableState(D2UnitStrc *pUnit)
{
    if (!pUnit)
    {
        FOG_DisplayAssert(szAssertHItem, szItemCpp, 3212);
        exit(-1);
    }

    D2StatListStrc *pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, STATE_UNINTERRUPTABLE);
    if (pStatList)
    {
        D2COMMON_STATES_ToggleState(pUnit, STATE_UNINTERRUPTABLE, 0);
        D2COMMON_STATLIST_ExpireStatList(pUnit, pStatList);
        D2COMMON_STATLIST_FreeStatList(pStatList);
        D2COMMON_UNITS_UpdateAnimRateAndVelocity(pUnit, szItemCpp, 3220);
    }
}


//1.10f: D2Client.0x6FB2FB30
//  The acquire half: put state 54 on the unit for the duration of a client-side item move. If a state-54 statlist already exists it re-sets the
//  state on it; otherwise it allocates one and registers the remove callback below. The allocation flag 2 is what ITEM_IsItemLocked tests for.
//  With no unit the allocation still happens but belongs to nothing: the GUID is -1 and the type 6, one past the last unit type.
void __fastcall ITEM_SetUninterruptableState(D2UnitStrc *pUnit)
{
    D2StatListStrc *pStatList = D2COMMON_STATLIST_GetStatListFromUnitAndState(pUnit, STATE_UNINTERRUPTABLE);

    if (pStatList)
    {
        D2COMMON_STATLIST_SetState(pStatList, STATE_UNINTERRUPTABLE);
        return;
    }

    D2UnitGUID dwUnitId;
    D2C_UnitTypes dwUnitType;
    if (pUnit)
    {
        dwUnitId = pUnit->dwUnitId;
        dwUnitType = pUnit->dwUnitType;
    }
    else
    {
        dwUnitId = (D2UnitGUID)-1;
        dwUnitType = UNIT_TYPES_COUNT;
    }

    pStatList = D2COMMON_STATLIST_AllocStatList(NULL, 2u, 0xCu, dwUnitType, dwUnitId);
    D2COMMON_STATLIST_SetState(pStatList, STATE_UNINTERRUPTABLE);
    D2COMMON_STATLIST_SetStatRemoveCallback(pStatList, ITEM_OnUninterruptableStateRemoved);
}


//1.10f: D2Client.0x6FB2FB80
//  The remove callback of the statlist above: turns the state back off. The third argument is never read but is real: the epilogue is `retn 4`,
//  which purges one stack argument.
void __fastcall ITEM_OnUninterruptableStateRemoved(D2UnitStrc *pUnit, int32_t nState, D2StatListStrc *pUnusedStatList)
{
    D2COMMON_STATES_ToggleState(pUnit, (D2C_States)nState, 0);
}


//1.10f: D2Client.0x6FB2FB90
//  True when the unit carries a statlist with flag 2 (the one ITEM_SetUninterruptableState allocates) or when the local player has state 54
//  regardless of the item, so the inventory refuses every click while any move is in flight.
BOOL __fastcall ITEM_IsItemLocked(D2UnitStrc *pUnit)
{
    if (D2COMMON_STATLIST_GetStatListFromUnitAndFlag(pUnit, 2))
        return 1;

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    return pPlayer && D2COMMON_STATES_CheckState(pPlayer, STATE_UNINTERRUPTABLE);
}


//  Assert strings of the original.
static const char szAssertUnitIsItem[] = "UnitGetType(hUnit) == UNIT_ITEM";
static const char szAssertPtItemStats[] = "ptItemStats";
static const char szAssertPtGemMods[] = "ptGemMods";
static const char szAssertHInvItem[] = "hInvItem";
static const char szAssertHRemove[] = "hRemove";
static const char szAssertHRemoveIsInvItem[] = "hRemove == hInvItem";
static_assert(sizeof(szAssertUnitIsItem) == 31 + 1
                  && sizeof(szAssertPtItemStats) == 11 + 1
                  && sizeof(szAssertPtGemMods) == 9 + 1
                  && sizeof(szAssertHInvItem) == 8 + 1
                  && sizeof(szAssertHRemove) == 7 + 1
                  && sizeof(szAssertHRemoveIsInvItem) == 19 + 1,
              "the six strings at 0x6FB90CE8, 0x6FB79740, 0x6FB7A4A4, 0x6FB911F8, "
              "0x6FB90F50 and 0x6FB911E4, measured in the DLL");

//  Item type numbers used below.
static_assert(ITEMTYPE_GEM == 20 && ITEMTYPE_RUNE == 74 && ITEMTYPE_WEAPON == 45,
              "the three item types ITEM_ApplyGemMods branches on -- `push 14h` "
              "at 0x6FB2B8F0, `push 4Ah` at 0x6FB2B997, `push 2Dh` at 0x6FB2BA86");
static_assert(STAT_HITPOINTS == 6 && STAT_MANA == 8 && STAT_STAMINA == 0xA,
              "the three vitals ITEM_RefreshActiveItems reads and throws away, "
              "as the `push 6` / `push 8` / `push 0Ah` at 0x6FB2B39F have them");
static_assert(BODYLOC_RARM == 4 && BODYLOC_LARM == 5
                  && BODYLOC_SWRARM == 11 && BODYLOC_SWLARM == 12,
              "the two hands the quiver test asks about, and the two switch "
              "slots the component update skips -- `cmp al, 0Bh` / `cmp al, 0Ch`");
static_assert(SKILLUSTATE_NOQUANTITY == 2 && SKILLUSTATE_NOLEVEL == 7,
              "the two states that stop a remembered skill being restored: the "
              "`sub eax, 2` / `jz` then `sub eax, 5` / `jz` chain at 0x6FB2B854 "
              "is two compares the compiler folded, not one against 5");
static_assert(offsetof(D2ItemsTxt, dwGemOffset) == 0xF0,
              "items.txt's gem-record index, read at [eax+0F0h] in both arms of "
              "ITEM_ApplyGemMods");

//  The eleven body locations both walks cover. The loop is `inc / cmp 0Bh / jl`, so it runs 0..10: BODYLOC_SWRARM is the bound and is not visited.
#define ITEM_BODYLOC_WALK_COUNT 11
static_assert(ITEM_BODYLOC_WALK_COUNT == BODYLOC_SWRARM,
              "the walks stop where the weapon-switch slots begin");


//1.10f: D2Client.0x6FB2B360
//  Re-evaluates which of a unit's items are active and re-applies their bonuses. It runs only for a player or for a unit with dwFlags bit 9, and
//  only when it has an inventory. In order: remember the two active skills; activate charms; deactivate anything on the body that no longer
//  qualifies; re-activate in a fixpoint loop; re-apply set items; restore the two skills; refresh the graphics if anything moved.
//  The original inlines ITEM_DetachItemFromUnit twice here.
//  Hitpoints, mana and stamina (stats 6, 8, 10) are read and the results thrown away, as three discarded calls, because a D2Common accessor is
//  not provably pure. The fixpoint counter is a post-decrement that tests the old value, so the body can run 101 times in the worst case; the
//  ordinary exit is the break when a pass changes nothing. Two pairs of variables share one stack slot each in the original (bAnyChanged and
//  the item's anim mode; bChangedThisPass and a component id); they are declared separately. The body-location counters are plain ints.
void __fastcall ITEM_RefreshActiveItems(D2UnitStrc *pUnit)
{
    if (!pUnit || (pUnit->dwUnitType != UNIT_PLAYER && ((pUnit->dwFlags >> 9) & 1) == 0))
        return;

    D2InventoryStrc *pInventory = pUnit->pInventory;
    if (!pInventory)
        return;

    int32_t bAnyChanged = 0;

    //  Read and discarded: hitpoints, mana, stamina.
    D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_HITPOINTS, 0);
    D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_MANA, 0);
    D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_STAMINA, 0);

    D2C_Skills nLeftSkillId;
    D2UnitGUID nLeftOwnerGUID;
    D2SkillStrc *pLeftSkill = D2COMMON_UNITS_GetLeftSkill(pUnit);
    if (pLeftSkill)
    {
        nLeftOwnerGUID = D2COMMON_SKILLS_GetOwnerGUIDFromSkill(pLeftSkill);
        nLeftSkillId = (D2C_Skills)D2COMMON_SKILLS_GetSkillIdFromSkill(pLeftSkill, szItemCpp, 341);
    }
    else
    {
        nLeftOwnerGUID = (D2UnitGUID)-1;
        nLeftSkillId = SKILL_ATTACK;
    }

    D2C_Skills nRightSkillId;
    D2UnitGUID nRightOwnerGUID;
    D2SkillStrc *pRightSkill = D2COMMON_UNITS_GetRightSkill(pUnit);
    if (pRightSkill)
    {
        nRightOwnerGUID = D2COMMON_SKILLS_GetOwnerGUIDFromSkill(pRightSkill);
        nRightSkillId = (D2C_Skills)D2COMMON_SKILLS_GetSkillIdFromSkill(pRightSkill, szItemCpp, 353);
    }
    else
    {
        nRightOwnerGUID = (D2UnitGUID)-1;
        nRightSkillId = SKILL_ATTACK;
    }

    //  1. the inventory walk: charms on page 1
    for (D2UnitStrc *pNode = D2COMMON_INVENTORY_GetFirstItem(pInventory); pNode; pNode = D2COMMON_INVENTORY_GetNextItem(pNode))
    {
        D2UnitStrc *pCharm = D2COMMON_INVENTORY_UnitIsItem(pNode);
        if (!pCharm || D2COMMON_INVENTORY_GetItemNodePage(pNode) != 1 || !D2COMMON_ITEMS_IsCharmUsable(pCharm, pUnit) || D2COMMON_STATLIST_GetOwner(pCharm, NULL) || !D2COMMON_ITEMS_CheckRequirements(pCharm, pUnit, 0, NULL, NULL, NULL))
        {
            continue;
        }

        //  A quiver only counts when the weapon in the other hand matches it.
        if (D2COMMON_ITEMS_GetQuiverType(pCharm))
        {
            D2InventoryStrc *pInv = pUnit->pInventory;
            D2UnitStrc *pWeapon = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInv, BODYLOC_RARM);
            if (pWeapon == pCharm)
                pWeapon = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInv, BODYLOC_LARM);
            if (!pWeapon)
                continue;
            if (!D2COMMON_ITEMS_CheckItemTypeId(pWeapon, (D2C_ItemTypes)D2COMMON_ITEMS_GetQuiverType(pCharm)))
            {
                continue;
            }
        }

        D2COMMON_ITEMS_SetItemFlag(pCharm, IFLAG_NOEQUIP, 0);
        ITEM_ApplyGemMods(pCharm, pUnit, 0, 0);
    }

    //  2. the body walk: deactivate what no longer qualifies
    for (int32_t nBodyLoc = BODYLOC_NONE; nBodyLoc < ITEM_BODYLOC_WALK_COUNT; ++nBodyLoc)
    {
        D2UnitStrc *pItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, (D2C_PlayerBodyLocs)nBodyLoc);
        if (!pItem)
            continue;

        //  Each flag is only disqualifying when the item has no statlist owner: a flagged item that is already granting its stats stays.
        if (D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_BROKEN, 515, szItemCpp) && !D2COMMON_STATLIST_GetOwner(pItem, NULL))
        {
            continue;
        }
        if (D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_NOEQUIP, 520, szItemCpp) && !D2COMMON_STATLIST_GetOwner(pItem, NULL))
        {
            continue;
        }

        if (D2COMMON_ITEMS_CheckRequirements(pItem, pUnit, 0, NULL, NULL, NULL))
        {
            //  Requirements pass. Only a quiver whose weapon has gone can still be deactivated here; anything else is left alone.
            if (!D2COMMON_ITEMS_GetQuiverType(pItem))
                continue;

            D2InventoryStrc *pInv = pUnit->pInventory;
            D2UnitStrc *pWeapon = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInv, BODYLOC_RARM);
            if (pWeapon == pItem)
                pWeapon = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInv, BODYLOC_LARM);
            if (pWeapon && D2COMMON_ITEMS_CheckItemTypeId(pWeapon, (D2C_ItemTypes)D2COMMON_ITEMS_GetQuiverType(pItem)))
            {
                continue;
            }
        }

        D2COMMON_ITEMS_SetItemFlag(pItem, IFLAG_NOEQUIP, 1);
        D2Composits eComponent = (D2Composits)D2COMMON_UNITS_GetItemComponentId(pUnit, pItem);

        D2InventoryStrc *pInv = pUnit->pInventory;
        if (pInv)
        {
            D2COMMON_INVENTORY_UpdateWeaponGUIDOnRemoval(pInv, pItem);

            ITEM_DetachItemFromUnit(pItem, pUnit, 1, 0);

            GFX_ClearUnitModesLoaded(pUnit, eComponent);
        }
        bAnyChanged = 1;
    }

    //  3. the fixpoint: re-activate until a pass changes nothing
    int32_t nPassesLeft = 100;
    do
    {
        int32_t bChangedThisPass = 0;

        for (int32_t nBodyLoc = BODYLOC_NONE; nBodyLoc < ITEM_BODYLOC_WALK_COUNT; ++nBodyLoc)
        {
            D2UnitStrc *pItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, (D2C_PlayerBodyLocs)nBodyLoc);

            if (!pItem || D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_BROKEN, 550, szItemCpp) || (!D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_NOEQUIP, 554, szItemCpp) && D2COMMON_STATLIST_GetOwner(pItem, NULL))
                || !D2COMMON_ITEMS_CheckRequirements(pItem, pUnit, 0, NULL, NULL, NULL))
            {
                continue;
            }

            if (D2COMMON_ITEMS_GetQuiverType(pItem))
            {
                D2InventoryStrc *pInv = pUnit->pInventory;
                D2UnitStrc *pWeapon = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInv, BODYLOC_RARM);
                if (pWeapon == pItem)
                    pWeapon = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInv, BODYLOC_LARM);
                if (!pWeapon)
                    continue;
                if (!D2COMMON_ITEMS_CheckItemTypeId(pWeapon, (D2C_ItemTypes)D2COMMON_ITEMS_GetQuiverType(pItem)))
                {
                    continue;
                }
            }

            D2COMMON_ITEMS_SetItemFlag(pItem, IFLAG_NOEQUIP, 0);

            D2Composits eComponent = (D2Composits)D2COMMON_UNITS_GetItemComponentId(pUnit, pItem);
            D2InventoryStrc *pInv = pUnit->pInventory;
            const uint32_t dwItemMode = pItem->dwItemMode;

            if (pInv)
            {
                //  The two weapon-switch slots are skipped: an item parked on the inactive weapon set contributes no component and no stats.
                const uint8_t nBodyLocation = D2COMMON_ITEMS_GetBodyLocation(pItem);
                if (nBodyLocation != BODYLOC_SWRARM && nBodyLocation != BODYLOC_SWLARM)
                {
                    D2COMMON_INVENTORY_UpdateWeaponGUIDOnRemoval(pInv, pItem);
                    GFX_ClearUnitModesLoaded(pUnit, eComponent);

                    if (dwItemMode == IMODE_EQUIP && !D2COMMON_STATLIST_GetOwner(pItem, NULL))
                    {
                        ITEM_ApplyGemMods(pItem, pUnit, 0, 1);
                    }

                    D2COMMON_INVENTORY_UpdateWeaponGUIDOnInsert(pInv, pItem);
                }
            }

            bChangedThisPass = 1;
            bAnyChanged = 1;
        }

        if (!bChangedThisPass)
            break;
    }
    while (nPassesLeft-- != 0);

    //  4. set items on the body, re-applied
    for (int32_t nBodyLoc = BODYLOC_NONE; nBodyLoc < ITEM_BODYLOC_WALK_COUNT; ++nBodyLoc)
    {
        D2UnitStrc *pItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, (D2C_PlayerBodyLocs)nBodyLoc);
        if (!pItem || D2COMMON_ITEMS_GetItemQuality(pItem) != ITEMQUAL_SET || D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_BROKEN, 583, szItemCpp) || D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_NOEQUIP, 584, szItemCpp))
        {
            continue;
        }

        ITEM_DetachItemFromUnit(pItem, pUnit, 1, 0);

        ITEM_ApplyGemMods(pItem, pUnit, 1, 0);
    }

    //  5. put the two active skills back
    //  Nothing is restored while the skill is out of quantity or below its level requirement: the `sub 2` / `sub 5` chain names states 2 and 7.
    if (nLeftSkillId)
    {
        D2SkillStrc *pSkill = D2COMMON_SKILLS_GetSkillById(pUnit, nLeftSkillId, nLeftOwnerGUID);
        if (pSkill && pSkill != D2COMMON_UNITS_GetLeftSkill(pUnit))
        {
            const D2C_SkillUseStates eUseState = PLRSKILLS_SkillsGetUseStateForClient(pUnit, pSkill);
            if (eUseState != SKILLUSTATE_NOQUANTITY && eUseState != SKILLUSTATE_NOLEVEL)
                D2COMMON_SKILLS_SetLeftActiveSkill(pUnit, nLeftSkillId, nLeftOwnerGUID);
        }
    }

    if (nRightSkillId)
    {
        D2SkillStrc *pSkill = D2COMMON_SKILLS_GetSkillById(pUnit, nRightSkillId, nRightOwnerGUID);
        if (pSkill && pSkill != D2COMMON_UNITS_GetRightSkill(pUnit))
        {
            const D2C_SkillUseStates eUseState = PLRSKILLS_SkillsGetUseStateForClient(pUnit, pSkill);
            if (eUseState != SKILLUSTATE_NOQUANTITY && eUseState != SKILLUSTATE_NOLEVEL)
                D2COMMON_SKILLS_SetRightActiveSkill(pUnit, nRightSkillId, nRightOwnerGUID);
        }
    }

    //  6. the graphics, if anything moved
    if (bAnyChanged)
    {
        if (pUnit->dwUnitType)
        {
            MONSTER_DoClientAction(CLTACTION_NEUTRAL, pUnit, NULL);
        }
        else
        {
            GFX_StopIfAlive(pUnit);
            GFX_RebuildUnitAnimForMode(pUnit, 0);
        }
    }
}


//1.10f: D2Client.0x6FB2B8E0
//  Applies an item's socket-filler modifiers to whatever it is socketed into, then merges its stat list into the target. Three arms, by the
//  item's type:
//      GEM   (20)  ITEMMODS_AssignProperty(2, NULL,    pItem, mods, set, 0)
//      RUNE  (74)  ITEMMODS_AssignProperty(5, pTarget, pItem, mods, set, 0)
//      else        ITEMS_UpdateSets(pTarget, pItem, 0, 0)
//  The gem and rune arms are the same six steps (assert the target is an item, look up items.txt, look up gems.txt through dwGemOffset, assert
//  each, read the apply type, assign); they differ in the property set (2 against 5) and in whether the target is handed to the assignment.
//  The dual-wield arm does not run for a monster: INVENTORY_GetLeftHandWeapon is not called unless the target is a player. When the target is
//  a player who can dual wield, the merge flag depends on the hand: a weapon that is not the left-hand weapon merges with 0, everything else with 1.
void __fastcall ITEM_ApplyGemMods(D2UnitStrc *pItem, D2UnitStrc *pTarget, int32_t nUnused, int32_t bIgnoreBroken)
{
    if (!pTarget)
        return;

    if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_GEM))
    {
        if (pTarget->dwUnitType != UNIT_ITEM)
        {
            FOG_DisplayAssert(szAssertUnitIsItem, szItemCpp, 48);
            exit(-1);
        }

        D2ItemsTxt *pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(pItem ? pItem->dwClassId : -1);
        if (!pItemsTxt)
        {
            FOG_DisplayAssert(szAssertPtItemStats, szItemCpp, 50);
            exit(-1);
        }

        D2GemsTxt *pMods = D2COMMON_DATATBLS_GetGemsTxtRecord(pItemsTxt->dwGemOffset);
        if (!pMods)
        {
            FOG_DisplayAssert(szAssertPtGemMods, szItemCpp, 52);
            exit(-1);
        }

        D2COMMON_ITEMMODS_AssignProperty(2, NULL, pItem, pMods, D2COMMON_ITEMS_GetGemApplyType(pTarget), 0);
    }
    else if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_RUNE))
    {
        if (pTarget->dwUnitType != UNIT_ITEM)
        {
            FOG_DisplayAssert(szAssertUnitIsItem, szItemCpp, 58);
            exit(-1);
        }

        D2ItemsTxt *pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(pItem ? pItem->dwClassId : -1);
        if (!pItemsTxt)
        {
            FOG_DisplayAssert(szAssertPtItemStats, szItemCpp, 60);
            exit(-1);
        }

        D2GemsTxt *pMods = D2COMMON_DATATBLS_GetGemsTxtRecord(pItemsTxt->dwGemOffset);
        if (!pMods)
        {
            FOG_DisplayAssert(szAssertPtGemMods, szItemCpp, 62);
            exit(-1);
        }

        D2COMMON_ITEMMODS_AssignProperty(5, pTarget, pItem, pMods, D2COMMON_ITEMS_GetGemApplyType(pTarget), 0);
    }
    else
    {
        D2COMMON_ITEMS_UpdateSets(pTarget, pItem, 0, 0);
    }

    if (D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_NOEQUIP, 72, szItemCpp))
        return;

    if (pTarget->dwUnitType == UNIT_PLAYER)
    {
        D2UnitStrc *pLeftHandWeapon = D2COMMON_INVENTORY_GetLeftHandWeapon(pTarget->pInventory);

        if (D2COMMON_UNITS_CanDualWield(pTarget))
        {
            BOOL bType = 1;
            if (pLeftHandWeapon && pItem != pLeftHandWeapon)
                bType = !D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_WEAPON);

            if (!D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_BROKEN, 100, szItemCpp) || bIgnoreBroken)
            {
                D2COMMON_STATLIST_MergeStatLists(pTarget, pItem, bType);
            }
            return;
        }
    }

    if (!D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_BROKEN, 112, szItemCpp) || bIgnoreBroken)
    {
        D2COMMON_STATLIST_MergeStatLists(pTarget, pItem, 1);
    }
}


//1.10f: D2Client.0x6FB2FA80
//  Server packet 0x7C: a bare reference to one unit; all it does is release that unit's item lock (ITEM_ClearUninterruptableState, which the
//  original inlines here). The unit type comes out of the packet, so the reference is not necessarily to an item. What the server means by
//  the packet is not established.
void __fastcall ITEM_OnItemRefPacket7C(D2GSPacketSrv7C *pPacket)
{
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(pPacket->nItemGUID, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pUnit)
        return;

    ITEM_ClearUninterruptableState(pUnit);
}


//1.10f: D2Client.0x6FB2FBC0
//  Server packet 0x7D: two unit GUIDs; GUID2 is the item and GUID1 whoever holds it. Two commands, chosen by unk0x0A:
//      256 (0x100)  the item breaks: set the flag from unk0x0E, stop the holder, detach the item, rebuild the animation, tell the set logic
//                   it went away (the original inlines ITEM_DetachItemFromUnit here)
//      512 (0x200)  the item is repaired: clear the flag and put its component and stats back
//  Anything else returns without touching either unit. The break arm passes the command word it has just compared against 0x100 (which is
//  IFLAG_BROKEN) as dwFlag; the repair arm pushes the literal 100h (0x6FB2FC85) instead. Written as the original has it.
void __fastcall ITEM_OnItemPairPacket7D(D2GSPacketSrv7D *pPacket)
{
    D2UnitStrc *pItem = CUNIT_FindUnitByGuidAndType(pPacket->nUnitGUID2, UNIT_ITEM);
    if (!pItem)
        return;

    D2UnitStrc *pOwner = CUNIT_FindUnitByGuidAndType(pPacket->nUnitGUID1, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pOwner)
        return;

    const D2Composits eComponent = (D2Composits)D2COMMON_UNITS_GetItemComponentId(pOwner, pItem);
    const uint32_t dwItemMode = pItem->dwItemMode;

    if (pPacket->nItemFlag == 256)
    {
        D2COMMON_ITEMS_SetItemFlag(pItem, (D2C_ItemFlags)pPacket->nItemFlag, pPacket->bSet);
        GFX_StopIfAlive(pOwner);

        D2InventoryStrc *pInventory = pOwner->pInventory;
        if (pInventory)
        {
            D2COMMON_INVENTORY_UpdateWeaponGUIDOnRemoval(pInventory, pItem);

            ITEM_DetachItemFromUnit(pItem, pOwner, 1, 0);

            GFX_ClearUnitModesLoaded(pOwner, eComponent);
        }

        GFX_RebuildUnitAnimForMode(pOwner, 0);
        D2COMMON_ITEMS_UpdateSets(pOwner, pItem, 1, 1);
    }
    else
    {
        if (pPacket->nItemFlag != 512)
            return;

        D2COMMON_ITEMS_SetItemFlag(pItem, IFLAG_BROKEN, 0);

        D2InventoryStrc *pInventory = pOwner->pInventory;
        if (pInventory)
        {
            const uint8_t nBodyLocation = D2COMMON_ITEMS_GetBodyLocation(pItem);
            if (nBodyLocation != BODYLOC_SWRARM && nBodyLocation != BODYLOC_SWLARM)
            {
                D2COMMON_INVENTORY_UpdateWeaponGUIDOnRemoval(pInventory, pItem);
                GFX_ClearUnitModesLoaded(pOwner, eComponent);

                if (dwItemMode == IMODE_EQUIP && !D2COMMON_STATLIST_GetOwner(pItem, NULL))
                {
                    ITEM_ApplyGemMods(pItem, pOwner, 0, 1);
                }

                D2COMMON_INVENTORY_UpdateWeaponGUIDOnInsert(pInventory, pItem);
            }
        }

        D2COMMON_ITEMS_UpdateSets(pOwner, pItem, 0, 0);
    }

    ITEM_RefreshActiveItems(pOwner);
}


//1.10f: D2Client.0x6FB2FDA0
//  Server packet 0x92: empty the player's inventory of everything equipped and every usable charm, then free what is left. The argument is
//  used throughout. The walk takes the next node before doing anything, because removing the current one unlinks it. Page 3 is the body, page 1
//  the inventory grid; a grid item is only taken when it is a usable charm. The third assert checks that what the removal returned is what was
//  asked for. The original inlines ITEM_DetachItemFromUnit here.
void __fastcall ITEM_OnPacket92(D2UnitStrc *pPlayer)
{
    if (!pPlayer)
        return;

    D2InventoryStrc *pInventory = pPlayer->pInventory;
    if (!pInventory)
        return;

    D2UnitStrc *pNode = D2COMMON_INVENTORY_GetFirstItem(pInventory);
    while (pNode)
    {
        D2UnitStrc *pNextNode = D2COMMON_INVENTORY_GetNextItem(pNode);

        D2UnitStrc *pItem = D2COMMON_INVENTORY_UnitIsItem(pNode);
        if (!pItem)
        {
            FOG_DisplayAssert(szAssertHInvItem, szItemCpp, 3353);
            exit(-1);
        }

        if (D2COMMON_INVENTORY_GetItemNodePage(pNode) == 3 || (D2COMMON_INVENTORY_GetItemNodePage(pNode) == 1 && D2COMMON_ITEMS_IsCharmUsable(pItem, pPlayer)))
        {
            GFX_StopIfAlive(pPlayer);
            D2COMMON_INVENTORY_UpdateWeaponGUIDOnRemoval(pInventory, pItem);

            D2UnitStrc *pRemoved = D2COMMON_INVENTORY_RemoveItemFromInventory(pInventory, pItem);
            if (!pRemoved)
            {
                FOG_DisplayAssert(szAssertHRemove, szItemCpp, 3364);
                exit(-1);
            }
            if (pRemoved != pItem)
            {
                FOG_DisplayAssert(szAssertHRemoveIsInvItem, szItemCpp, 3365);
                exit(-1);
            }

            if (D2COMMON_INVENTORY_GetItemNodePage(pNode) == 3)
            {
                D2COMMON_10254_Return0(pInventory, D2COMMON_INVENTORY_GetBodyLocFromEquippedItem(pInventory, pItem));
            }

            D2COMMON_ITEMS_UpdateSets(pPlayer, pRemoved, 1, 1);

            ITEM_DetachItemFromUnit(pRemoved, pPlayer, 0, 0);

            ITEM_RefreshActiveItems(pPlayer);
        }

        pNode = pNextNode;
    }

    D2COMMON_INVENTORY_RemoveAllItems(pInventory);
}


//1.10f: D2Client.0x6FB2EDA0
//  Server packet 0x42: "the item you were holding is gone". The packet is read directly: a unit type byte at +1, a unit GUID dword at +2.
//  If that unit is the local player, its cursor item is taken off the cursor, the item unit is destroyed by id, and the acknowledgement runs
//  against the destroyed item's GUID. The GUID is read before the cursor is cleared, because the clear is what loses it.
//  The body ends in a tail jump to NPCMENU_AckItemMessage.
void __fastcall ITEM_NpcMenuAckItemActionAndDropCursorItem(void *pPacket)
{
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(*(uint32_t *)((const char *)pPacket + 2), (D2C_UnitTypes)*((const uint8_t *)pPacket + 1));

    if (!pUnit || pUnit != CUNIT_GetPlayer())
        return;

    D2InventoryStrc *pInventory = pUnit->pInventory;
    D2UnitStrc *pCursorItem = D2COMMON_INVENTORY_GetCursorItem(pInventory);
    if (!pCursorItem)
        return;

    const uint32_t dwUnitId = pCursorItem->dwUnitId;

    D2COMMON_INVENTORY_SetCursorItem(pInventory, NULL);
    CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pInventory));

    CUNIT_RemoveUnitById(dwUnitId, pCursorItem->dwUnitType);
    NPCMENU_AckItemMessage(dwUnitId);
}


//  Assert and halt strings of the original.
static const char szAssertNotInUnitTable[] = "!UnitGetFromId(ptCmd->dwId, UNIT_ITEM)";
static const char szAssertHPlayer[] = "hPlayer";
static const char szHaltPlaceItemGround[] = "[UnitItemPlaceItemGroundEx] This should never happen!\n";
static_assert(sizeof(szAssertNotInUnitTable) == 38 + 1
                  && sizeof(szAssertHPlayer) == 7 + 1
                  && sizeof(szHaltPlaceItemGround) == 54 + 1,
              "the three strings at 0x6FB90C88, 0x6FB761EC and 0x6FB90CB0 -- the "
              "last one ends in a newline, which is why it is 54 and not 53");

//  The header lengths are the only real difference between the two item creators: nPacketSize - 8 against nPacketSize - 13 are where each
//  packet's bitstream begins.
static_assert(offsetof(D2GSPacketSrv9C, pBitstream) == 8
                  && offsetof(D2GSPacketSrv9D, pBitstream) == 13,
              "packet 0x9D carries nUnitType at +0x08 and nUnitId at +0x09 where "
              "0x9C's bitstream has already started -- the five-byte difference "
              "the two `sub` immediates encode");
static_assert(offsetof(D2ItemSaveStrc, nClassId) == 0x00
                  && offsetof(D2ItemSaveStrc, nX) == 0x04
                  && offsetof(D2ItemSaveStrc, nY) == 0x06
                  && offsetof(D2ItemSaveStrc, nAnimMode) == 0x08
                  && offsetof(D2ItemSaveStrc, dwFlags) == 0x0C
                  && offsetof(D2ItemSaveStrc, nStorePage) == 0x10,
              "the decoded header this batch reads field by field, as the "
              "[esi+0], [esi+8] and [esi+0Ch] loads at 0x6FB2AEA4 have it");
static_assert(IFLAG_NEWITEM == 1 && IFLAG_TARGET == 2 && IFLAG_INSTORE == 0x2000,
              "the three flags batch 4 passes -- `push 1` at 0x6FB2B095, `push 2` "
              "at 0x6FB2AEEE and `push 2000h` at 0x6FB2ADF0");


//1.10f: D2Client.0x6FB2AD50
//  Item message 0x00 of packet 0x9C: the item appears on the ground. Decodes the bitstream, builds the unit at the saved position, clears the
//  uninterruptable lock if it arrived with one and, only for an item already in ONGROUND mode that carries IFLAG_INSTORE, puts it into DROPPING
//  and plays the flippy: sound 216, then the item's own drop sound at the frame ITEM_GetDropSound writes back (preset to 12), at volume 180
//  through its instance handle. The original inlines ITEM_ClearUninterruptableState here.
void __fastcall ITEM_ItemMsg00_PutOnGround(D2GSPacketSrv9C *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 8, 0, &tItemSave);

    D2UnitStrc *pItem = ITEM_CreateItemFromItemExMsg(pPacket, &tItemSave, tItemSave.nX, tItemSave.nY, 0);
    if (!pItem)
    {
        FOG_DisplayAssert(szAssertHItem, szItemCpp, 900);
        exit(-1);
    }

    ITEM_ClearUninterruptableState(pItem);

    if (pItem->dwItemMode != IMODE_ONGROUND)
        return;
    if (!D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_INSTORE, 904, szItemCpp))
        return;

    D2COMMON_UNITS_ChangeAnimMode(pItem, IMODE_DROPPING);

    int32_t nDropSfxFrame = 12;
    const D2C_Sounds nDropSound = ITEM_GetDropSound(pItem, &nDropSfxFrame);

    SOUNDINST_PlaySound(SOUND_ITEM_FLIPPY, pItem, 0, 0, 0);
    if (nDropSound > 0)
    {
        const int32_t nHandle = SOUNDINST_PlaySound(nDropSound, pItem, nDropSfxFrame, 0, 0);
        if (nHandle)
            SOUNDINST_SetVolumeByHandle(nHandle, 180);
    }
}


//1.10f: D2Client.0x6FB2AE60
//  Builds the item unit for a packet 0x9C message; every subtype that can create an item comes through here. When pItemSave is NULL it decodes
//  the compact header itself into a local, otherwise it trusts the caller's. Then it creates the unit and hands the same bitstream to the full
//  decoder. On failure it removes the half-built unit and returns NULL, which is why callers check and most of them assert hItem.
//  IFLAG_TARGET in the decoded flags (not on the unit) means the item is newly targeted: the flag is cleared on the new unit and, if the caller
//  asked to announce, client message 55 goes out unless the item is already identified.
//  ITEM_CreateItemFromItemUnitExMsg below is this function for packet 0x9D.
D2UnitStrc *__fastcall ITEM_CreateItemFromItemExMsg(D2GSPacketSrv9C *pPacket, D2ItemSaveStrc *pItemSave, uint16_t nX, uint16_t nY, int32_t bAnnounce)
{
    const size_t nSize = pPacket->nPacketSize - 8;

    D2ItemSaveStrc tLocalSave;
    if (!pItemSave)
    {
        D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, nSize, 0, &tLocalSave);
        pItemSave = &tLocalSave;
    }

    D2UnitStrc *pItem = CUNIT_CreateUnit(pPacket->nItemId, pItemSave->nClassId, nX, nY, UNIT_ITEM, pItemSave->nAnimMode);

    BOOL bFail;
    D2COMMON_ITEMS_DecodeItemFromBitstream(pItem, pPacket->pBitstream, nSize, 0, NULL, 0x60u, &bFail);
    if (bFail)
    {
        CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);
        return NULL;
    }

    if (D2COMMON_ITEMS_CheckIfFlagIsSet(pItemSave->dwFlags, IFLAG_TARGET))
    {
        D2COMMON_ITEMS_SetItemFlag(pItem, IFLAG_TARGET, 0);
        if (bAnnounce && !D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_IDENTIFIED, 216, szItemCpp))
        {
            MSG_SendServerItemMessage(55, pPacket->nItemId);
        }
    }

    return pItem;
}


//1.10f: D2Client.0x6FB2AF40
//  Item message 0x01 of packet 0x9C: the item goes onto the cursor. It acts only when the decoded anim mode is ONCURSOR, and it asserts that
//  the item is not already in the client's unit table before creating it. The cursor is set in two steps: the inventory owns the cursor item,
//  and CURSOR_SetHeldUnit is then pointed at whatever the inventory now holds (read back, not passed through).
//  The original inlines ITEM_ClearUninterruptableState here.
//  BUG: the player's inventory is fetched before the mode test, so a message with any other mode still dereferences the player; nothing
//  guards it. LATENT: the server sends action 1 from one place and only when the item's owner is the receiving client's own player.
void __fastcall ITEM_ItemMsg01_ToCursor(D2GSPacketSrv9C *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 8, 0, &tItemSave);

    D2InventoryStrc *pInventory = CUNIT_GetPlayer()->pInventory;

    if (tItemSave.nAnimMode != IMODE_ONCURSOR)
        return;

    if (CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM))
    {
        FOG_DisplayAssert(szAssertNotInUnitTable, szItemCpp, 928);
        exit(-1);
    }

    D2UnitStrc *pItem = ITEM_CreateItemFromItemExMsg(pPacket, &tItemSave, 0, 0, 0);
    if (!pItem)
    {
        FOG_DisplayAssert(szAssertHItem, szItemCpp, 930);
        exit(-1);
    }

    D2COMMON_INVENTORY_SetCursorItem(pInventory, pItem);
    CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pInventory));

    ITEM_ClearUninterruptableState(pItem);
}


//1.10f: D2Client.0x6FB2B030
//  Item message 0x02 of packet 0x9C: an item that may already exist is dropped to the ground. Three ways in, which converge:
//      the item is unknown                          build it from the packet
//      the item exists, IFLAG_NEWITEM decoded set   destroy it and build it again
//      the item exists, the flag is clear           keep it
//  If what it finds is the item on our cursor, the cursor is cleared first (the same read-back pair message 0x01 uses).
//  The original inlines ITEM_ClearUninterruptableState and, as its last four statements, ITEM_SetItemDisplayState with bResolveRoom = 1.
void __fastcall ITEM_ItemMsg02_DropToGround(D2GSPacketSrv9C *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 8, 0, &tItemSave);

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    D2UnitStrc *pItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);

    if (!pItem)
    {
        pItem = ITEM_CreateItemFromItemExMsg(pPacket, &tItemSave, tItemSave.nX, tItemSave.nY, 0);
    }
    else
    {
        D2InventoryStrc *pInventory = pPlayer->pInventory;
        D2UnitStrc *pCursorItem = D2COMMON_INVENTORY_GetCursorItem(pInventory);
        if (pCursorItem && pCursorItem == pItem)
        {
            D2COMMON_INVENTORY_SetCursorItem(pInventory, NULL);
            CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pInventory));
        }

        if (D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_NEWITEM))
        {
            CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);
            pItem = ITEM_CreateItemFromItemExMsg(pPacket, &tItemSave, tItemSave.nX, tItemSave.nY, 0);
        }
    }

    if (!pItem)
    {
        FOG_DisplayAssert(szAssertHItem, szItemCpp, 975);
        exit(-1);
    }

    ITEM_ClearUninterruptableState(pItem);

    ITEM_SetItemDisplayState(pItem, tItemSave.nX, tItemSave.nY, IMODE_DROPPING, 1, tItemSave.nStorePage);
}


//1.10f: D2Client.0x6FB2B190
//  Item message 0x03 of packet 0x9C: an item the player was carrying is now on the ground. The existing unit is taken out of the inventory,
//  the weapon bookkeeping is repaired, the unit is destroyed, and a fresh one is built from the packet.
//  It halts rather than asserts on mode 0: mode STORED reaches FOG_DisplayHalt with "[UnitItemPlaceItemGroundEx] This should never happen!".
//  Only mode EQUIP does the removal; every other mode falls straight through to the destroy. The `sub eax, 0` / `dec eax` pair is the compiler's
//  chain for two compares against 0 and 1, not arithmetic on the mode.
//  The original inlines ITEM_DetachItemFromUnit and ITEM_ClearUninterruptableState here.
void __fastcall ITEM_ItemMsg03_OnGround(D2GSPacketSrv9C *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 8, 0, &tItemSave);

    D2UnitStrc *pItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);
    if (pItem)
    {
        if (pItem->dwItemMode == IMODE_STORED)
        {
            FOG_DisplayHalt(szHaltPlaceItemGround, szItemCpp, 1021);
            exit(-1);
        }

        if (pItem->dwItemMode == IMODE_EQUIP)
        {
            D2UnitStrc *pPlayer = CUNIT_GetPlayer();
            if (!pPlayer)
            {
                FOG_DisplayAssert(szAssertHPlayer, szItemCpp, 1001);
                exit(-1);
            }

            D2InventoryStrc *pInventory = pPlayer->pInventory;
            GFX_StopIfAlive(pPlayer);
            D2COMMON_INVENTORY_UpdateWeaponGUIDOnRemoval(pInventory, pItem);

            const D2Composits eComponent = (D2Composits)D2COMMON_UNITS_GetItemComponentId(pPlayer, pItem);
            GFX_ClearUnitModesLoaded(pPlayer, eComponent);

            const D2C_PlayerBodyLocs eBodyLoc = (D2C_PlayerBodyLocs)D2COMMON_ITEMS_GetBodyLocation(pItem);

            //  This can replace pItem: the out-parameter is the same slot.
            D2COMMON_INVENTORY_GetSecondWieldingWeapon(pPlayer, pInventory, &pItem, eBodyLoc);

            D2UnitStrc *pRemoved = D2COMMON_INVENTORY_RemoveItemFromInventory(pInventory, pItem);
            if (pRemoved)
            {
                D2COMMON_10254_Return0(pInventory, eBodyLoc);

                ITEM_DetachItemFromUnit(pRemoved, pPlayer, 0, 0);
            }

            ITEM_RefreshActiveItems(pPlayer);
            GFX_RebuildUnitAnimForMode(pPlayer, 0);
        }

        CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);
    }

    D2UnitStrc *pNewItem = ITEM_CreateItemFromItemExMsg(pPacket, &tItemSave, tItemSave.nX, tItemSave.nY, 0);
    if (!pNewItem)
    {
        FOG_DisplayAssert(szAssertHItem, szItemCpp, 1029);
        exit(-1);
    }

    ITEM_ClearUninterruptableState(pNewItem);
}


//1.10f: D2Client.0x6FB2C1D0
//  The packet 0x9D form of ITEM_CreateItemFromItemExMsg: the same routine written twice, for a packet whose bitstream starts five bytes later.
//  The two differ in exactly two places, both the header length: the `- 13` here against `- 8`, and pBitstream at +0x0D against +0x08.
D2UnitStrc *__fastcall ITEM_CreateItemFromItemUnitExMsg(D2GSPacketSrv9D *pPacket, D2ItemSaveStrc *pItemSave, uint16_t nX, uint16_t nY, int32_t bAnnounce)
{
    const size_t nSize = pPacket->nPacketSize - 13;

    D2ItemSaveStrc tLocalSave;
    if (!pItemSave)
    {
        D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, nSize, 0, &tLocalSave);
        pItemSave = &tLocalSave;
    }

    D2UnitStrc *pItem = CUNIT_CreateUnit(pPacket->nItemId, pItemSave->nClassId, nX, nY, UNIT_ITEM, pItemSave->nAnimMode);

    BOOL bFail;
    D2COMMON_ITEMS_DecodeItemFromBitstream(pItem, pPacket->pBitstream, nSize, 0, NULL, 0x60u, &bFail);
    if (bFail)
    {
        CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);
        return NULL;
    }

    if (D2COMMON_ITEMS_CheckIfFlagIsSet(pItemSave->dwFlags, IFLAG_TARGET))
    {
        D2COMMON_ITEMS_SetItemFlag(pItem, IFLAG_TARGET, 0);
        if (bAnnounce && !D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_IDENTIFIED, 254, szItemCpp))
        {
            MSG_SendServerItemMessage(55, pPacket->nItemId);
        }
    }

    return pItem;
}


//  Trace, assert and warning strings of the original.
static const char szTraceInvalidPage[] = " Invalid page %d at line %d of file %s\n";
static const char szAssertHInventory[] = "hInventory";
static const char szTraceInsertFailed[] = "sUnitItemInsertItemInvGridBase failed at %d in %s\n";
static const char szTraceRemoveFailed[] = "UnitItemRemoveItemInvGrid failed at line %d in file %s\n";
static const char szTraceRemoveFailedItem[] = "UnitItemRemoveItemInvGrid failed at line %d in file %s for item %s\n";
static const char szAssertHRemoveIsItem[] = "hRemove == hItem";
static const char szAssertHUnit[] = "hUnit";
static_assert(sizeof(szTraceInvalidPage) == 39 + 1
                  && sizeof(szAssertHInventory) == 10 + 1
                  && sizeof(szTraceRemoveFailed) == 55 + 1
                  && sizeof(szTraceRemoveFailedItem) == 67 + 1
                  && sizeof(szAssertHRemoveIsItem) == 16 + 1
                  && sizeof(szAssertHUnit) == 5 + 1,
              "six of batch 5's seven strings, each measured in the DLL; the "
              "seventh, szTraceInsertFailed, is asserted below");

//  szTraceInsertFailed is 50 characters and a NUL.
static_assert(sizeof(szTraceInsertFailed) == 50 + 1,
              "0x6FB90D08: 50 characters to the NUL, walked byte by byte at the "
              "unit's close");

static_assert(IFLAG_DELETED == 8 && IFLAG_QUANTITY == 0x20,
              "the two decoded flags this batch branches on -- `push 8` at "
              "0x6FB2D6EB and `push 20h` at 0x6FB2C081");
static_assert(UNIT_OBJECT == 2 && UNIT_MISSILE == 3 && UNIT_TILE == 5,
              "the three unit types the coordinate accessor's arm test names: "
              "`cmp eax, 2` / `cmp eax, 3` / `cmp eax, 5` at 0x6FB2BB77");
static_assert(offsetof(D2StaticPathStrc, tGameCoords) == 0x0C,
              "the static path's game coordinates, which is what [eax+0Ch] and "
              "[eax+10h] reach -- NOT the dynamic arm Hex-Rays prints");

//  The two quest items the cube animates, as four-character item codes.
#define ITEM_CODE_HORADRIC_STAFF 0x20747368u  /* 'hst ' */
#define ITEM_CODE_KHALIMS_FLAIL  0x20326671u  /* 'qf2 ' */
static_assert(ITEM_CODE_HORADRIC_STAFF == 544502632u
                  && ITEM_CODE_KHALIMS_FLAIL == 540173937u,
              "the two `cmp ecx, ...` immediates at 0x6FB2BED0 and 0x6FB2BED8, "
              "which Hex-Rays prints as decimals");


//1.10f: D2Client.0x6FB2BAF0
//  Item message 0x04 of packet 0x9C: the item lands on a store or stash page. Three ways to reach the placement, which converge on one tail:
//      the item is unknown            build it, announcing (bAnnounce 1)
//      the item exists, not new       re-place it, then clear every flag and set the decoded ones
//      the item exists and is new     take it out of the inventory, destroy it, build it again
//  A store page of 0xFF is not fatal: it goes to FOG_Trace and the handler returns. The trace's first argument is the literal 255 rather than
//  the page variable, which is correct because the branch has just proved the page is 255.
//  The tail is ITEM_ClearUninterruptableState with its assert kept. The original inlines ITEM_SetItemDisplayState, UNITS_GetYPosition,
//  UNITS_GetXPosition, ITEM_DetachItemFromUnit and ITEM_ClearUninterruptableState here.
void __fastcall ITEM_ItemMsg04_ToStorePage(D2GSPacketSrv9C *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 8, 0, &tItemSave);

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    D2UnitStrc *pItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);

    if (!pItem)
    {
        pItem = ITEM_CreateItemFromItemExMsg(pPacket, &tItemSave, 0, 0, 1);
    }
    else if (!D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_NEWITEM))
    {
        ITEM_SetItemDisplayState(pItem, tItemSave.nX, tItemSave.nY, tItemSave.nAnimMode, 0, tItemSave.nStorePage);

        //  `push 0FFFFFFFFh`: every flag cleared, then the decoded set put back.
        D2COMMON_ITEMS_SetItemFlag(pItem, (D2C_ItemFlags)0xFFFFFFFF, 0);
        D2COMMON_ITEMS_SetItemFlag(pItem, (D2C_ItemFlags)tItemSave.dwFlags, 1);
    }
    else
    {
        if (D2COMMON_INVENTORY_RemoveItemFromInventory(pPlayer->pInventory, pItem))
        {
            const D2C_ItemInvPage eInvPage = (D2C_ItemInvPage)D2COMMON_ITEMS_GetInvPage(pItem);
            const D2C_InventoryRecords nRecordId = (D2C_InventoryRecords)D2COMMON_UNITS_GetInventoryRecordId(pPlayer, eInvPage, GAME_IsExpansionGame());

            int32_t nY = UNITS_GetYPosition(pItem);

            int32_t nX = UNITS_GetXPosition(pItem);

            D2COMMON_INVENTORY_Return(szItemCpp, 1108, pPlayer->pInventory, nX, nY, nRecordId, 1, (uint8_t)eInvPage);
            INV_ResetItemDescState();

            ITEM_DetachItemFromUnit(pItem, pPlayer, 0, 0);
        }

        CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);
        pItem = ITEM_CreateItemFromItemExMsg(pPacket, &tItemSave, 0, 0, 0);
        if (!pItem)
            return;
    }

    if (tItemSave.nStorePage == 0xFF)
    {
        FOG_Trace(szTraceInvalidPage, 255, 1137, szItemCpp);
        return;
    }

    //  DIFFERENCE: the original pushes the page as a whole dword, whose upper three bytes are nBodyLoc and two bytes the decoder never writes; ours
    //  passes the byte. Every reader narrows the argument to a byte, so nothing reads the difference.
    ITEM_PlaceAtStorePage(tItemSave.nX, tItemSave.nY, tItemSave.nStorePage, pPlayer, pItem);

    if (D2COMMON_ITEMS_IsCharmUsable(pItem, pPlayer))
        ITEM_ApplyGemMods(pItem, pPlayer, 0, 0);

    UI_PostSyntheticMouseMove();

    ITEM_ClearUninterruptableState(pItem);

    NPCMENU_AckItemAction_ToStorePage(pItem->dwUnitId);
}


//1.10f: D2Client.0x6FB2BD70
//  Puts an item at (nX, nY) on a store or stash page. The two placement calls are gated in sequence: first the socket
//  (INVENTORY_PlaceItemInSocket), and only inside it the grid. A grid failure traces and returns; nothing is rolled back.
//  The save record's page byte is an inventory page when the owner is a player and a store page when it is an NPC.
//      page 1  play the item's own drop sound, with no unit and no frame
//      else    clear the cursor (the read-back pair again)
//      page 3  and the item is the Horadric Staff or Khalim's Flail: start the cube's quest animation
//  All three are guarded by pPlayer == CUNIT_GetPlayer(), asked twice. The original inlines UNITS_GetYPosition and UNITS_GetXPosition here.
void __fastcall ITEM_PlaceAtStorePage(uint16_t nX, uint16_t nY, int32_t nStorePage, D2UnitStrc *pPlayer, D2UnitStrc *pItem)
{
    D2COMMON_UNITS_SetXForStaticUnit(pItem, nX);
    D2COMMON_UNITS_SetYForStaticUnit(pItem, nY);

    D2InventoryStrc *pInventory = pPlayer->pInventory;
    if (!pInventory)
    {
        FOG_DisplayAssert(szAssertHInventory, szItemCpp, 1043);
        exit(-1);
    }

    if (!D2COMMON_INVENTORY_PlaceItemInSocket(pInventory, pItem, 1))
        return;

    int32_t nItemY = UNITS_GetYPosition(pItem);

    int32_t nItemX = UNITS_GetXPosition(pItem);

    const D2C_InventoryRecords nRecordId = (D2C_InventoryRecords)D2COMMON_UNITS_GetInventoryRecordId(pPlayer, (D2C_ItemInvPage)nStorePage, GAME_IsExpansionGame());

    if (!D2COMMON_INVENTORY_PlaceItemAtInventoryPage(pInventory, pItem, nItemX, nItemY, nRecordId, 1, nStorePage))
    {
        FOG_Trace(szTraceInsertFailed, 1046, szItemCpp);
        return;
    }

    if (pPlayer == CUNIT_GetPlayer())
    {
        if ((uint8_t)nStorePage == 1)
        {
            SOUNDINST_PlaySound(ITEM_GetDropSound(pItem, NULL), NULL, 0, 0, 0);
        }
        else
        {
            D2COMMON_INVENTORY_SetCursorItem(pInventory, NULL);
            CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pInventory));
        }
    }

    //  The player is asked for a second time here rather than reused.
    if (pPlayer == CUNIT_GetPlayer() && (uint8_t)nStorePage == 3)
    {
        const uint32_t dwCode = D2COMMON_DATATBLS_GetItemsTxtRecord(pItem->dwClassId)->dwCode;
        if (dwCode == ITEM_CODE_HORADRIC_STAFF || dwCode == ITEM_CODE_KHALIMS_FLAIL)
            INV_CubeStartQuestItemAnim(dwCode);
    }
}


//1.10f: D2Client.0x6FB2BEF0
//  Item message 0x05 of packet 0x9D: the item goes into a unit's inventory grid. The owner comes from the packet's own (+8 type, +9 GUID) pair,
//  so this is the one message of the family that can act on a unit other than the local player. It is also the only one that checks the removal
//  against the item it was told about (hRemove == hItem) instead of trusting the GUID.
//  The original inlines ITEM_DetachItemFromUnit, ITEM_SetItemDisplayState and ITEM_ClearUninterruptableState here.
void __fastcall ITEM_ItemMsg05_ToInventoryGrid(D2GSPacketSrv9D *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 13, 0, &tItemSave);

    D2UnitStrc *pOwner = CUNIT_FindUnitByGuidAndType(pPacket->nUnitId, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pOwner)
        return;

    D2UnitStrc *pItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);
    if (!pItem)
    {
        //  The player is fetched and thrown away; nothing reads it.
        if (tItemSave.nStorePage == 1)
            CUNIT_GetPlayer();
        FOG_Trace(szTraceRemoveFailed, 1196, szItemCpp);
        return;
    }

    D2InventoryStrc *pInventory = pOwner->pInventory;
    if (!pInventory)
    {
        FOG_DisplayAssert(szAssertHInventory, szItemCpp, 1201);
        exit(-1);
    }

    D2UnitStrc *pRemoved = D2COMMON_INVENTORY_RemoveItemFromInventory(pInventory, pItem);
    if (!pRemoved)
    {
        if (tItemSave.nStorePage == 2 && pOwner == CUNIT_GetPlayer())
            return;

        char szName[512];
        FOG_Trace(szTraceRemoveFailedItem, 1215, szItemCpp, D2COMMON_DATATBLS_GetUnitNameFromUnit(pItem, szName));
        return;
    }

    if (pRemoved != pItem)
    {
        FOG_DisplayAssert(szAssertHRemoveIsItem, szItemCpp, 1218);
        exit(-1);
    }

    const uint8_t nInvPage = D2COMMON_ITEMS_GetInvPage(pItem);
    const D2C_InventoryRecords nRecordId = (D2C_InventoryRecords)D2COMMON_UNITS_GetInventoryRecordId(pOwner, (D2C_ItemInvPage)nInvPage, GAME_IsExpansionGame());

    D2COMMON_INVENTORY_Return(szItemCpp, 1222, pInventory, tItemSave.nX, tItemSave.nY, nRecordId, 1, nInvPage);
    INV_ResetItemDescState();

    ITEM_DetachItemFromUnit(pItem, pOwner, 0, 0);

    if (D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_QUANTITY))
    {
        CUNIT_RemoveUnitById(pItem->dwUnitId, UNIT_ITEM);
        if (pOwner == CUNIT_GetPlayer())
        {
            if (tItemSave.nStorePage == 1)
                SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
            else
                CURSOR_SetHeldUnit(NULL);
        }
    }
    else
    {
        if (D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_NEWITEM))
        {
            CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);
            pItem = ITEM_CreateItemFromItemUnitExMsg(pPacket, &tItemSave, 0, 0, 0);
            if (!pItem)
            {
                FOG_DisplayAssert(szAssertHItem, szItemCpp, 1236);
                exit(-1);
            }
        }
        else
        {
            //  BUG: in the arm that re-places an existing item (neither IFLAG_QUANTITY nor IFLAG_NEWITEM set) the original pushes the same register
            //  twice (0x6FB2C105, 0x6FB2C106), so ITEM_SetClientItemMode receives the item's X as its Y. INERT: it reads nX and nY only in its
            //  IMODE_ONGROUND and IMODE_DROPPING arms, and a stock server never reaches this arm with either.
            ITEM_SetItemDisplayState(pItem, tItemSave.nX, tItemSave.nX, tItemSave.nAnimMode, 0, tItemSave.nStorePage);
        }

        D2COMMON_ITEMS_SetInvPage(pItem, 0xFFu);
        D2COMMON_INVENTORY_SetCursorItem(pInventory, pItem);
        CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pInventory));

        ITEM_ClearUninterruptableState(pItem);
    }

    if (pOwner == CUNIT_GetPlayer())
        NPCMENU_AckItemAction_ToInventoryGrid(pItem->dwUnitId);
}


//1.10f: D2Client.0x6FB2D650
//  Item message 0x0A of packet 0x9C: re-decode the bitstream onto the existing unit; there is no creation step. This is how the server rewrites
//  an item in place when it is identified, socketed or personalised.
//  The unit is not null-checked before the decode: CUNIT_FindUnitByGuidAndType can answer NULL and its answer goes straight into
//  ITEMS_DecodeItemFromBitstream; only the player and its inventory are asserted, and only afterwards.
//  On a decode failure the unit is dropped. Otherwise the cursor is cleared (unless the item carries IFLAG_DELETED) and the uninterruptable lock
//  is taken off whatever is on the cursor afterwards, which need not be the item this message was about.
//  The original inlines ITEM_ClearUninterruptableState here.
void __fastcall ITEM_ItemMsg0A_UpdateItem(D2GSPacketSrv9C *pPacket)
{
    const size_t nSize = pPacket->nPacketSize - 8;

    D2UnitStrc *pItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);

    BOOL bFail;
    D2COMMON_ITEMS_DecodeItemFromBitstream(pItem, pPacket->pBitstream, nSize, 0, NULL, 0x60u, &bFail);
    if (bFail)
    {
        CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);
        return;
    }

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        FOG_DisplayAssert(szAssertHUnit, szItemCpp, 1946);
        exit(-1);
    }

    D2InventoryStrc *pInventory = pPlayer->pInventory;
    if (!pInventory)
    {
        FOG_DisplayAssert(szAssertHInventory, szItemCpp, 1949);
        exit(-1);
    }

    if (!D2COMMON_ITEMS_CheckIfFlagIsSet(D2COMMON_ITEMS_GetItemFlags(pItem), IFLAG_DELETED))
    {
        D2COMMON_INVENTORY_SetCursorItem(pInventory, NULL);
        CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pInventory));
    }

    D2UnitStrc *pCursorItem = D2COMMON_INVENTORY_GetCursorItem(pInventory);
    if (!pCursorItem)
        return;

    ITEM_ClearUninterruptableState(pCursorItem);
}


//1.10f: D2Client.0x6FB2EE00
//  Item message 0x12 of packet 0x9C: the forced-recreate counterpart of message 0x01. Where 0x01 asserts that the client does not already know
//  the item, this one destroys whatever it finds and rebuilds. The creation's result is not checked, unlike every other message of the
//  family: it goes straight into INVENTORY_SetCursorItem.
//  void: eax is left holding CURSOR_SetHeldUnit's result (a tail return), and no caller reads it.
void __fastcall ITEM_ItemMsg12_RecreateToCursor(D2GSPacketSrv9C *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 8, 0, &tItemSave);

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        FOG_DisplayAssert(szAssertHUnit, szItemCpp, 2782);
        exit(-1);
    }

    if (CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM))
        CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);

    D2UnitStrc *pItem = ITEM_CreateItemFromItemExMsg(pPacket, &tItemSave, 0, 0, 0);

    D2InventoryStrc *pInventory = pPlayer->pInventory;
    if (!pInventory)
    {
        FOG_DisplayAssert(szAssertHInventory, szItemCpp, 2789);
        exit(-1);
    }

    D2COMMON_INVENTORY_SetCursorItem(pInventory, pItem);
    CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pInventory));
}


//  Item messages 0x06, 0x07, 0x08 and 0x09 of packet 0x9D (the body locations), and the helper 0x09 hands its switch-out half to.
//  Trace and assert strings of the original.
static const char szTraceInsertBodyInvalidUnit[] = "UnitItemInsertItemBodyEx() invalid unit type:%d  id:%d";
static const char szTraceAddNullItem[] = "[INVENTORY ERROR] Trying to add a NULL item!\n";
static const char szTraceInsertNullIntoBody[] = "[INVENTORY ERROR] Trying to insert null item into unit's body location!";
static const char szTraceForcedIntoBody[] = "[INVENTORY ERROR] Forced an item into unit's body location!";
static const char szTraceUnableToInsertIntoBody[] = "[INVENTORY ERROR] Unable to insert item into unit's body location!";
static const char szAssertHOpposite[] = "hOpposite";
static const char szAssertHRemoveIsOpposite[] = "hRemove && hRemove == hOpposite";
static const char szAssertHHandItem[] = "hHandItem";
static const char szAssertUnitIsPlayer[] = "UnitGetType(hUnit) == UNIT_PLAYER";
static_assert(sizeof(szTraceInsertBodyInvalidUnit) == 54 + 1
                  && sizeof(szTraceAddNullItem) == 45 + 1
                  && sizeof(szTraceInsertNullIntoBody) == 71 + 1
                  && sizeof(szTraceForcedIntoBody) == 59 + 1
                  && sizeof(szTraceUnableToInsertIntoBody) == 66 + 1,
              "the five traces at 0x6FB90ED8, 0x6FB90EA8, 0x6FB90E60, 0x6FB90E24 and "
              "0x6FB90DE0, whose IDA sizes 55, 46, 72, 60 and 67 count the NUL");
static_assert(sizeof(szAssertHOpposite) == 9 + 1
                  && sizeof(szAssertHRemoveIsOpposite) == 31 + 1
                  && sizeof(szAssertHHandItem) == 9 + 1
                  && sizeof(szAssertUnitIsPlayer) == 33 + 1,
              "the four asserts at 0x6FB90F44, 0x6FB90F10, 0x6FB90F30 and 0x6FB90A1C");

//  Item flags used by the body-location messages.
static_assert(IFLAG_SWITCHIN == 0x40 && IFLAG_SWITCHOUT == 0x80 && IFLAG_NOEQUIP == 0x4000,
              "`push 40h` at 0x6FB2CE15, `push 80h` at 0x6FB2D01E and `push 4000h` at "
              "0x6FB2CDA0 -- the three flags batch 6 spells as names");
static_assert(NODEPAGE_EQUIP == 3,
              "`mov ebp, 3` at 0x6FB2C49D: the node page an ordinary body location gets");
static_assert(offsetof(D2GSPacketSrv9D, nComponent) == 3
                  && offsetof(D2GSPacketSrv9D, nItemId) == 4
                  && offsetof(D2GSPacketSrv9D, nUnitType) == 8
                  && offsetof(D2GSPacketSrv9D, nUnitId) == 9,
              "packet 0x9D: [ecx+3] at 0x6FB2C574, [ebp+4] at 0x6FB2C36B, [ebp+8] at "
              "0x6FB2C33E and [ebp+9] at 0x6FB2C339 -- the owner pair sits where packet "
              "0x9C's bitstream has already begun");
static_assert(offsetof(D2ItemSaveStrc, nBodyLoc) == 0x11,
              "`mov al, [edi+11h]` at 0x6FB2D0E0: the body location, one byte past the "
              "store page -- an adjacent pair of bytes a transcription could swap");


//1.10f: D2Client.0x6FB2C300
//  Item message 0x06 of packet 0x9D: the item goes into a body location of the unit the packet names (the original's own name is
//  "UnitItemInsertItemBodyEx", from the trace it emits for an unknown owner). The three ways in are those of message 0x04: an unknown item is
//  built (announced only when the owner is the local player), a known item that is not new is re-moded where it stands, and a known item that
//  is new has its old copy taken off the owner, dropped from the unit table and built again.
//  The node page of 0x06, 0x07 and 0x09 is 3, or 4 when the body location is BODYLOC_SWRARM or BODYLOC_SWLARM; 4 has no name and is written as
//  the literal. Only 0x06 reads the value back: `cmp ebp, 3` at 0x6FB2C548 gates INVENTORY_UpdateWeaponGUIDOnInsert, which 0x07 and 0x09 call
//  regardless. INVENTORY_GetSecondWieldingWeapon acts only on an empty slot, so at 0x06, 0x07 and 0x08, where the slot holds an item just tested
//  non-null, the call does nothing; it is written because the original makes it.
//  The owner type is a two-case switch (UNIT_PLAYER and UNIT_MONSTER, a default that returns). This is the one of the five that returns quietly:
//  it returns when there is no local player and on an owner with no inventory; it applies the socket modifiers only below BODYLOC_SWRARM
//  (`cmp byte ptr [..nBodyloc], 0Bh / jnb` at 0x6FB2C5E6); and it keeps the arriving item's weapon class only for the local player.
//  ITEMS_SetBodyLocation is pushed the body location four bytes wide (0x6FB2C47F); its parameter is a byte, so passing the field is the same call.
//  The original inlines ITEM_SetItemDisplayState, ITEM_CacheWeaponClassForBodyLoc and ITEM_ClearUninterruptableState here.
void __fastcall ITEM_ItemMsg06_InsertIntoBodyLoc(D2GSPacketSrv9D *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 13, 0, &tItemSave);

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;

    D2UnitStrc *pOwner = CUNIT_FindUnitByGuidAndType(pPacket->nUnitId, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pOwner)
    {
        FOG_Trace(szTraceInsertBodyInvalidUnit, pPacket->nUnitType, pPacket->nUnitId);
        return;
    }

    D2UnitStrc *pItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);
    if (!pItem)
    {
        pItem = ITEM_CreateItemFromItemUnitExMsg(pPacket, &tItemSave, 0, 0, pOwner == pPlayer);
    }
    else if (!D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_NEWITEM))
    {
        ITEM_SetItemDisplayState(pItem, 0, 0, tItemSave.nAnimMode, 0, tItemSave.nStorePage);
    }
    else
    {
        D2InventoryStrc *pOwnerInventory = pOwner->pInventory;
        D2UnitStrc *pOldItem = pItem;
        const int32_t nOldBodyLoc = D2COMMON_ITEMS_GetBodyLocation(pOldItem);
        if (nOldBodyLoc == BODYLOC_RARM || nOldBodyLoc == BODYLOC_LARM)
        {
            D2COMMON_INVENTORY_UpdateWeaponGUIDOnRemoval(pOwnerInventory, pOldItem);
            GFX_ClearUnitModesLoaded(pOwner, (D2Composits)D2COMMON_UNITS_GetItemComponentId(pOwner, pOldItem));
            D2COMMON_INVENTORY_GetSecondWieldingWeapon(pOwner, pOwnerInventory, &pOldItem, (D2C_PlayerBodyLocs)nOldBodyLoc);
        }

        D2UnitStrc *pRemoved = D2COMMON_INVENTORY_RemoveItemFromInventory(pOwnerInventory, pOldItem);
        if (pRemoved)
        {
            ITEM_DetachItemFromUnit(pRemoved, pOwner, 0, 0);
            D2COMMON_10254_Return0(pOwnerInventory, nOldBodyLoc);
        }

        //  The old copy's NULL guard cannot fire: the slot holds the item this arm found, and the one call in between that could change it
        //  (INVENTORY_GetSecondWieldingWeapon) leaves a full slot alone. The -1 arm is dead and is written because it is in the bytes (0x6FB2C411).
        if (pOldItem)
            CUNIT_RemoveUnitById(pOldItem->dwUnitId, pOldItem->dwUnitType);
        else
            CUNIT_RemoveUnitById(-1, UNIT_TYPES_COUNT);

        pItem = ITEM_CreateItemFromItemUnitExMsg(pPacket, &tItemSave, 0, 0, 0);
    }

    if (pItem)
        D2COMMON_ITEMS_SetBodyLocation(pItem, tItemSave.nBodyLoc);

    D2InventoryStrc *pInventory = pOwner->pInventory;
    if (!pInventory)
        return;

    D2C_NodePages nNodePage = NODEPAGE_EQUIP;
    if (tItemSave.nBodyLoc == BODYLOC_SWRARM || tItemSave.nBodyLoc == BODYLOC_SWLARM)
        nNodePage = (D2C_NodePages)4;

    if (!D2COMMON_INVENTORY_PlaceItemInSocket(pInventory, pItem, nNodePage))
    {
        FOG_Trace(szTraceAddNullItem);
        return;
    }

    const int32_t nBodyLoc = tItemSave.nBodyLoc;
    if (!pItem)
    {
        FOG_Trace(szTraceInsertNullIntoBody);
        return;
    }

    if (D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, (D2C_PlayerBodyLocs)nBodyLoc))
    {
        D2COMMON_10254_Return0(pInventory, nBodyLoc);
        FOG_Trace(szTraceForcedIntoBody);
    }

    if (!D2COMMON_INVENTORY_PlaceItemInBodyLoc(pInventory, pItem, (D2C_PlayerBodyLocs)nBodyLoc))
    {
        FOG_Trace(szTraceUnableToInsertIntoBody);
        return;
    }

    GFX_StopIfAlive(pOwner);
    if (nNodePage == NODEPAGE_EQUIP)
        D2COMMON_INVENTORY_UpdateWeaponGUIDOnInsert(pInventory, pItem);
    GFX_SetAllUnitModesLoaded(pOwner, 0);

    switch (pOwner->dwUnitType)
    {
    case UNIT_PLAYER:
    case UNIT_MONSTER:
        break;

    default:
        return;
    }

    if (pPacket->nComponent == 0 || pPacket->nComponent == 1)
        GFX_ClearUnitModesLoaded(pOwner, (D2Composits)pPacket->nComponent);
    GFX_RebuildUnitAnimForMode(pOwner, 0);

    if (pOwner == pPlayer)
    {
        ITEM_CacheWeaponClassForBodyLoc(pItem, tItemSave.nBodyLoc);

        if (!D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_DELETED))
        {
            D2COMMON_INVENTORY_SetCursorItem(pInventory, NULL);
            CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pInventory));
        }
    }

    if (tItemSave.nBodyLoc < (int32_t)BODYLOC_SWRARM)
        ITEM_ApplyGemMods(pItem, pOwner, 0, 0);

    ITEM_RefreshActiveItems(pOwner);
    ITEM_RefreshActiveItems(pOwner);
    UI_PostSyntheticMouseMove();

    ITEM_ClearUninterruptableState(pItem);

    NPCMENU_AckItemAction_InsertIntoBodyLoc(pItem->dwUnitId);
}


//1.10f: D2Client.0x6FB2C660
//  Item message 0x07 of packet 0x9D: an item goes into one hand and pushes the other hand's item off. The packet names only the arriving item;
//  the leaving one is whatever the opposite hand holds (the asserts "hOpposite" and "hRemove && hRemove == hOpposite"). The leaving item is taken
//  off first -- for the local player all the way onto the cursor -- and the arriving one placed after, so when one of the three [INVENTORY ERROR]
//  traces returns, the removal has already happened. An owner that is neither a player nor a monster asserts "0".
//  The jump table at 0x6FB2C6C2 covers body locations 4..12 and has two live arms: 4 BODYLOC_RARM and 11 BODYLOC_SWRARM look in 5 BODYLOC_LARM;
//  5 BODYLOC_LARM and 12 BODYLOC_SWLARM look in 4 BODYLOC_RARM.
//  BUG: an item arriving in a swap-set hand (11 or 12) is paired with the active set's other hand, not with 12 or 11. Transcribed as the
//  table has it.
//  UNREACHABLE: the server sends this action for an item carrying the command flag 0x10000, whose one setter (0x6FC45550) takes BODYLOC_RARM
//  and BODYLOC_LARM only.
//  The original inlines ITEM_DetachItemFromUnit, ITEM_SetItemDisplayState, ITEM_CacheWeaponClassForBodyLoc and ITEM_ClearUninterruptableState here.
void __fastcall ITEM_ItemMsg07_SwapBodyLocItems(D2GSPacketSrv9D *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 13, 0, &tItemSave);

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    D2UnitStrc *pOwner = CUNIT_FindUnitByGuidAndType(pPacket->nUnitId, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pOwner)
        return;

    GFX_StopIfAlive(pOwner);

    D2C_PlayerBodyLocs nOppositeBodyLoc;
    switch (tItemSave.nBodyLoc)
    {
    case BODYLOC_RARM:
    case BODYLOC_SWRARM:
        nOppositeBodyLoc = BODYLOC_LARM;
        break;

    case BODYLOC_LARM:
    case BODYLOC_SWLARM:
        nOppositeBodyLoc = BODYLOC_RARM;
        break;

    default:
        return;
    }

    D2InventoryStrc *pInventory = pOwner->pInventory;
    D2UnitStrc *pOpposite = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, nOppositeBodyLoc);
    if (!pOpposite)
    {
        FOG_DisplayAssert(szAssertHOpposite, szItemCpp, 1500);
        exit(-1);
    }

    D2COMMON_INVENTORY_UpdateWeaponGUIDOnRemoval(pInventory, pOpposite);
    GFX_ClearUnitModesLoaded(pOwner, (D2Composits)D2COMMON_UNITS_GetItemComponentId(pOwner, pOpposite));
    D2COMMON_INVENTORY_GetSecondWieldingWeapon(pOwner, pInventory, &pOpposite, nOppositeBodyLoc);

    D2UnitStrc *pRemoved = D2COMMON_INVENTORY_RemoveItemFromInventory(pInventory, pOpposite);
    if (!pRemoved || pRemoved != pOpposite)
    {
        FOG_DisplayAssert(szAssertHRemoveIsOpposite, szItemCpp, 1508);
        exit(-1);
    }

    ITEM_DetachItemFromUnit(pRemoved, pOwner, 0, 0);

    D2COMMON_10254_Return0(pInventory, nOppositeBodyLoc);
    GFX_RebuildUnitAnimForMode(pOwner, 0);

    if (pOwner == pPlayer)
    {
        INV_ResetItemDescState();

        //  BUG: the weapon-class clear on the cursor path is keyed on the packet's body location, the hand the new item is going into, and the
        //  inlined ITEM_CacheWeaponClassForBodyLoc near the end writes that same slot with the new item's class. So the clear is always overwritten
        //  and the hand the old item left keeps that item's class. INERT: nothing in D2Client reads either slot.
        if (tItemSave.nBodyLoc == BODYLOC_RARM)
            gnWeaponClassRArm = 0;
        else if (tItemSave.nBodyLoc == BODYLOC_LARM)
            gnWeaponClassLArm = 0;

        ITEM_SetItemDisplayState(pOpposite, 0, 0, IMODE_ONCURSOR, 0, tItemSave.nStorePage);

        D2COMMON_ITEMS_SetBodyLocation(pOpposite, 0);
        D2COMMON_INVENTORY_SetCursorItem(pInventory, pOpposite);
        CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pInventory));
    }

    ITEM_ClearUninterruptableState(pOpposite);

    D2UnitStrc *pHandItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);
    if (!pHandItem)
    {
        pHandItem = ITEM_CreateItemFromItemUnitExMsg(pPacket, &tItemSave, 0, 0, 0);
    }
    else if (D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_NEWITEM))
    {
        CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);
        pHandItem = ITEM_CreateItemFromItemUnitExMsg(pPacket, &tItemSave, 0, 0, 0);
        if (!pHandItem)
        {
            FOG_DisplayAssert(szAssertHHandItem, szItemCpp, 1541);
            exit(-1);
        }
    }
    else
    {
        ITEM_SetItemDisplayState(pHandItem, 0, 0, tItemSave.nAnimMode, 0, tItemSave.nStorePage);
    }

    //  Not null-checked: the first creation above is not asserted.
    D2COMMON_ITEMS_SetBodyLocation(pHandItem, tItemSave.nBodyLoc);

    D2C_NodePages nNodePage = NODEPAGE_EQUIP;
    if (tItemSave.nBodyLoc == BODYLOC_SWRARM || tItemSave.nBodyLoc == BODYLOC_SWLARM)
        nNodePage = (D2C_NodePages)4;

    if (!D2COMMON_INVENTORY_PlaceItemInSocket(pInventory, pHandItem, nNodePage))
    {
        FOG_Trace(szTraceAddNullItem);
        return;
    }

    const int32_t nBodyLoc = tItemSave.nBodyLoc;
    if (!pHandItem)
    {
        FOG_Trace(szTraceInsertNullIntoBody);
        return;
    }

    if (D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, (D2C_PlayerBodyLocs)nBodyLoc))
    {
        D2COMMON_10254_Return0(pInventory, nBodyLoc);
        FOG_Trace(szTraceForcedIntoBody);
    }

    if (!D2COMMON_INVENTORY_PlaceItemInBodyLoc(pInventory, pHandItem, (D2C_PlayerBodyLocs)nBodyLoc))
    {
        FOG_Trace(szTraceUnableToInsertIntoBody);
        return;
    }

    D2COMMON_INVENTORY_UpdateWeaponGUIDOnInsert(pInventory, pHandItem);
    GFX_SetAllUnitModesLoaded(pOwner, 0);

    switch (pOwner->dwUnitType)
    {
    case UNIT_PLAYER:
    case UNIT_MONSTER:
        break;

    default:
        FOG_DisplayAssert(szAssertZero, szItemCpp, 1580);
        exit(-1);
    }

    if (pPacket->nComponent == 0 || pPacket->nComponent == 1)
        GFX_ClearUnitModesLoaded(pOwner, (D2Composits)pPacket->nComponent);
    GFX_RebuildUnitAnimForMode(pOwner, 0);

    ITEM_CacheWeaponClassForBodyLoc(pHandItem, tItemSave.nBodyLoc);

    ITEM_ApplyGemMods(pHandItem, pOwner, 0, 0);
    ITEM_RefreshActiveItems(pOwner);
    ITEM_RefreshActiveItems(pOwner);
    UI_PostSyntheticMouseMove();

    ITEM_ClearUninterruptableState(pHandItem);
}


//1.10f: D2Client.0x6FB2CB10
//  Item message 0x08 of packet 0x9D: the item comes out of the body location the packet names. It is found in the slot, taken out, its set bonuses
//  and stat list unwound, and then the owner decides what becomes of the unit:
//      the local player   acknowledge; clear that hand's weapon class; then, unless IFLAG_QUANTITY, the item -- or its rebuilt copy, for
//                         IFLAG_NEWITEM -- goes onto the cursor
//      anyone else        the item leaves the unit table, if it is in it
//  An IFLAG_QUANTITY item of the local player leaves the unit table without that lookup. The unwinding is D2COMMON_10254_Return0,
//  ITEMS_UpdateSets(pOwner, pRemoved, 1, 1), then the inlined ITEM_DetachItemFromUnit; ITEM_UnequipSwitchedOutItem runs the same three in the
//  opposite order. The removed item is not null-checked: INVENTORY_RemoveItemFromInventory's answer goes straight into both calls that take it.
//  The cursor path ends by asking ITEMS_CheckItemFlag for IFLAG_NOEQUIP (0x4000) and clearing it when set.
//  The original inlines ITEM_DetachItemFromUnit, ITEM_SetItemDisplayState and ITEM_ClearUninterruptableState here (the last under `if (pItem)`,
//  so without its assert).
void __fastcall ITEM_ItemMsg08_RemoveFromBodyLoc(D2GSPacketSrv9D *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 13, 0, &tItemSave);

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    D2UnitStrc *pOwner = CUNIT_FindUnitByGuidAndType(pPacket->nUnitId, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pOwner)
        return;

    D2InventoryStrc *pInventory = pOwner->pInventory;
    if (!pInventory)
    {
        FOG_DisplayAssert(szAssertHInventory, szItemCpp, 1607);
        exit(-1);
    }

    D2UnitStrc *pItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, (D2C_PlayerBodyLocs)tItemSave.nBodyLoc);
    if (!pItem)
        return;

    GFX_StopIfAlive(pOwner);
    D2COMMON_INVENTORY_UpdateWeaponGUIDOnRemoval(pInventory, pItem);
    GFX_ClearUnitModesLoaded(pOwner, (D2Composits)pPacket->nComponent);
    D2COMMON_INVENTORY_GetSecondWieldingWeapon(pOwner, pInventory, &pItem, (D2C_PlayerBodyLocs)tItemSave.nBodyLoc);

    D2UnitStrc *pRemoved = D2COMMON_INVENTORY_RemoveItemFromInventory(pInventory, pItem);
    D2COMMON_10254_Return0(pInventory, tItemSave.nBodyLoc);
    D2COMMON_ITEMS_UpdateSets(pOwner, pRemoved, 1, 1);

    ITEM_DetachItemFromUnit(pRemoved, pOwner, 0, 0);

    ITEM_RefreshActiveItems(pOwner);
    GFX_RebuildUnitAnimForMode(pOwner, 0);

    if (pOwner == pPlayer)
    {
        //  The guard that cannot fire: the item was tested non-null on entry and the slot-filling call cannot empty the slot, so the -1 is dead
        //  (0x6FB2CC39).
        NPCMENU_AckItemAction_RemoveFromBodyLoc(pItem ? pItem->dwUnitId : (uint32_t)-1);
        INV_ResetItemDescState();

        if (tItemSave.nBodyLoc == BODYLOC_RARM)
            gnWeaponClassRArm = 0;
        else if (tItemSave.nBodyLoc == BODYLOC_LARM)
            gnWeaponClassLArm = 0;

        if (!D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_QUANTITY))
        {
            if (D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_NEWITEM))
            {
                CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);
                pItem = ITEM_CreateItemFromItemUnitExMsg(pPacket, &tItemSave, 0, 0, 0);
                if (!pItem)
                {
                    FOG_DisplayAssert(szAssertHItem, szItemCpp, 1641);
                    exit(-1);
                }
            }
            else
            {
                ITEM_SetItemDisplayState(pItem, 0, 0, tItemSave.nAnimMode, 0, tItemSave.nStorePage);
            }

            D2COMMON_ITEMS_SetBodyLocation(pItem, 0);
            D2COMMON_INVENTORY_SetCursorItem(pInventory, pItem);
            CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pInventory));
            ITEM_RefreshActiveItems(pOwner);
            ITEM_RefreshActiveItems(pOwner);

            if (pItem)
            {
                ITEM_ClearUninterruptableState(pItem);

                if (D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_NOEQUIP, 1671, szItemCpp))
                    D2COMMON_ITEMS_SetItemFlag(pItem, IFLAG_NOEQUIP, 0);
            }
            return;
        }
    }
    else if (!CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM))
    {
        return;
    }

    CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);
}


//1.10f: D2Client.0x6FB2CDF0
//  Item message 0x09 of packet 0x9D: one half of a body-item swap. The decoded flags say which half the packet is:
//      IFLAG_SWITCHIN  (0x40)   the item goes onto the body -- here
//      IFLAG_SWITCHOUT (0x80)   it comes off -- ITEM_UnequipSwitchedOutItem
//      neither                  nothing
//  The switch-in half asserts where 0x06 returns: the owner must be a player and must have an inventory. It calls
//  ITEM_CacheWeaponClassForBodyLoc and ITEM_ClearUninterruptableState (0x6FB2CFF9, 0x6FB2D00E) out of line. The socket modifiers are applied
//  for every body location. The original inlines ITEM_SetItemDisplayState here.
void __fastcall ITEM_ItemMsg09_EquipOnPlayer(D2GSPacketSrv9D *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 13, 0, &tItemSave);

    if (D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_SWITCHIN))
    {
        D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(pPacket->nUnitId, (D2C_UnitTypes)pPacket->nUnitType);
        if (!pUnit)
            return;

        if (pUnit->dwUnitType != UNIT_PLAYER)
        {
            FOG_DisplayAssert(szAssertUnitIsPlayer, szItemCpp, 1691);
            exit(-1);
        }

        D2UnitStrc *pItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);
        if (D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_NEWITEM))
        {
            CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);
            pItem = ITEM_CreateItemFromItemUnitExMsg(pPacket, &tItemSave, 0, 0, 0);
            if (!pItem)
            {
                FOG_DisplayAssert(szAssertHHandItem, szItemCpp, 1698);
                exit(-1);
            }
        }
        else
        {
            ITEM_SetItemDisplayState(pItem, 0, 0, tItemSave.nAnimMode, 0, tItemSave.nStorePage);
        }

        D2COMMON_ITEMS_SetBodyLocation(pItem, tItemSave.nBodyLoc);

        D2InventoryStrc *pInventory = pUnit->pInventory;
        if (!pInventory)
        {
            FOG_DisplayAssert(szAssertHInventory, szItemCpp, 1705);
            exit(-1);
        }

        D2C_NodePages nNodePage = NODEPAGE_EQUIP;
        if (tItemSave.nBodyLoc == BODYLOC_SWRARM || tItemSave.nBodyLoc == BODYLOC_SWLARM)
            nNodePage = (D2C_NodePages)4;

        if (!D2COMMON_INVENTORY_PlaceItemInSocket(pInventory, pItem, nNodePage))
        {
            FOG_Trace(szTraceAddNullItem);
            return;
        }

        const int32_t nBodyLoc = tItemSave.nBodyLoc;
        if (D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, (D2C_PlayerBodyLocs)nBodyLoc))
        {
            D2COMMON_10254_Return0(pInventory, nBodyLoc);
            FOG_Trace(szTraceForcedIntoBody);
        }

        if (!D2COMMON_INVENTORY_PlaceItemInBodyLoc(pInventory, pItem, (D2C_PlayerBodyLocs)nBodyLoc))
        {
            FOG_Trace(szTraceUnableToInsertIntoBody);
            return;
        }

        GFX_StopIfAlive(pUnit);
        D2COMMON_INVENTORY_UpdateWeaponGUIDOnInsert(pInventory, pItem);
        GFX_SetAllUnitModesLoaded(pUnit, 0);
        ITEM_ApplyGemMods(pItem, pUnit, 0, 0);
        ITEM_RefreshActiveItems(pUnit);
        ITEM_RefreshActiveItems(pUnit);
        ITEM_CacheWeaponClassForBodyLoc(pItem, tItemSave.nBodyLoc);
        GFX_RebuildUnitAnimForMode(pUnit, 0);
        UI_PostSyntheticMouseMove();
        ITEM_ClearUninterruptableState(pItem);
    }
    else if (D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_SWITCHOUT))
    {
        ITEM_UnequipSwitchedOutItem(pPacket, &tItemSave);
    }
}


//1.10f: D2Client.0x6FB2D070
//  The switch-out half of message 0x09: the item in the packet's body location comes off a player and, when the player is the local one, onto
//  the cursor. It takes the item out of pItemSave->nBodyLoc, asserting "hRemove" and "hRemove == hItem", and unwinds its stat list and its set
//  bonuses. Then for IFLAG_NEWITEM the unit leaves the unit table and, for the local player only, is built again; otherwise it is re-moded where
//  it is. Only the local player's item then goes onto the cursor.
//  INVENTORY_GetItemFromBodyLoc's answer is not tested before it is passed on, so this is the one site where the slot can reach
//  INVENTORY_GetSecondWieldingWeapon empty. The unwinding runs backwards from message 0x08 (the inlined ITEM_DetachItemFromUnit, then
//  ITEMS_UpdateSets, then D2COMMON_10254_Return0), and CUNIT_GetPlayer() is called inside the rebuild arm (0x6FB2D1D6) and again before the
//  cursor steps (0x6FB2D265) rather than kept.
//  The original inlines ITEM_DetachItemFromUnit, ITEM_SetItemDisplayState and ITEM_ClearUninterruptableState here.
void __fastcall ITEM_UnequipSwitchedOutItem(D2GSPacketSrv9D *pPacket, D2ItemSaveStrc *pItemSave)
{
    D2UnitStrc *pUnit = CUNIT_FindUnitByGuidAndType(pPacket->nUnitId, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pUnit)
        return;

    if (pUnit->dwUnitType != UNIT_PLAYER)
    {
        FOG_DisplayAssert(szAssertUnitIsPlayer, szItemCpp, 1750);
        exit(-1);
    }

    D2InventoryStrc *pInventory = pUnit->pInventory;
    if (!pInventory)
    {
        FOG_DisplayAssert(szAssertHInventory, szItemCpp, 1752);
        exit(-1);
    }

    D2UnitStrc *pItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, (D2C_PlayerBodyLocs)pItemSave->nBodyLoc);
    GFX_StopIfAlive(pUnit);
    D2COMMON_INVENTORY_UpdateWeaponGUIDOnRemoval(pInventory, pItem);
    GFX_ClearUnitModesLoaded(pUnit, (D2Composits)pPacket->nComponent);
    D2COMMON_INVENTORY_GetSecondWieldingWeapon(pUnit, pInventory, &pItem, (D2C_PlayerBodyLocs)pItemSave->nBodyLoc);

    D2UnitStrc *pRemoved = D2COMMON_INVENTORY_RemoveItemFromInventory(pInventory, pItem);
    if (!pRemoved)
    {
        FOG_DisplayAssert(szAssertHRemove, szItemCpp, 1760);
        exit(-1);
    }
    if (pRemoved != pItem)
    {
        FOG_DisplayAssert(szAssertHRemoveIsItem, szItemCpp, 1761);
        exit(-1);
    }

    ITEM_DetachItemFromUnit(pRemoved, pUnit, 0, 0);

    D2COMMON_ITEMS_UpdateSets(pUnit, pRemoved, 1, 1);
    D2COMMON_10254_Return0(pInventory, pItemSave->nBodyLoc);

    if (D2COMMON_ITEMS_CheckIfFlagIsSet(pItemSave->dwFlags, IFLAG_NEWITEM))
    {
        CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);
        if (pUnit == CUNIT_GetPlayer())
        {
            pItem = ITEM_CreateItemFromItemUnitExMsg(pPacket, pItemSave, 0, 0, 0);
            if (!pItem)
            {
                FOG_DisplayAssert(szAssertHItem, szItemCpp, 1772);
                exit(-1);
            }
        }
    }
    else
    {
        ITEM_SetItemDisplayState(pItem, 0, 0, pItemSave->nAnimMode, 0, pItemSave->nStorePage);
    }

    if (pUnit != CUNIT_GetPlayer())
        return;

    INV_ResetItemDescState();
    D2COMMON_ITEMS_SetBodyLocation(pItem, 0);
    D2COMMON_INVENTORY_SetCursorItem(pInventory, pItem);
    CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pInventory));

    ITEM_ClearUninterruptableState(pItem);
}


//  Item message 0x17 of packet 0x9D and messages 0x0B/0x0C, 0x0D and 0x0E of packet 0x9C: the weapon switch, the store, the container and the belt.
//  The original pushes one-byte fields (such as nStorePage) with a dword load. Message 0x0B/0x0C's delete arm stores the page with
//  `mov byte ptr [esp+38h+nInvPage], al` (0x6FB2D86E) into a local nothing else writes, so the upper bytes are never written; the D2Common callees
//  read the parameter a byte wide, so passing the byte is the same call. ITEM_SetItemDisplayState narrows the page to a byte at its only use.
//  Message 0x0B/0x0C calls j_INV_NpcShopRequestPageRebuild with the store's NPC in ecx (`mov ecx, ebx` at 0x6FB2DA58): the argument is real.

//  Assert and trace strings of the original.
static const char szAssertRecreateFlag[] = "ITEMSTestFlags(ptInfo->dwFlags, ITEM_FLAG_RECREATE)";
static const char szTraceInvalidBPage[] = "Invalid bpage in file %s\n";
static const char szTraceNullInventory[] = "NULL inventory Error at line %d of file %s\n";
static const char szTraceDeleteWithoutMenu[] = "got delete while no menu active at line %d of file %s\n";
static const char szTraceNullRemove[] = "NULL hRemove Error of item %x at line %d of file %s\n";
static const char szTraceNullError[] = "NULL Error %d at line %d of file %s\n";
static const char szTraceAddPageConflict[] = " Add Error page conflict %d:%d  %dat line %d of file %s\n";
static_assert(sizeof(szAssertRecreateFlag) == 51 + 1
                  && sizeof(szTraceInvalidBPage) == 25 + 1
                  && sizeof(szTraceNullInventory) == 43 + 1
                  && sizeof(szTraceDeleteWithoutMenu) == 54 + 1,
              "the strings at 0x6FB90F58, 0x6FB9108C, 0x6FB91060 and 0x6FB91028, whose "
              "IDA sizes 52, 26, 44 and 55 count the NUL");
static_assert(sizeof(szTraceNullRemove) == 52 + 1 && sizeof(szTraceNullError) == 36 + 1
                  && sizeof(szTraceAddPageConflict) == 56 + 1,
              "the traces at 0x6FB90FF0, 0x6FB90FC8 and 0x6FB90F8C, IDA sizes 53, 37 "
              "and 57");

//  Node page numbers used below.
static_assert(NODEPAGE_STORAGE == 1 && NODEPAGE_BELTSLOTS == 2,
              "`push 1` at 0x6FB2DDAA and `push 2` at 0x6FB2DFF4: the container and the "
              "belt node pages, beside batch 6's NODEPAGE_EQUIP");
static_assert(IFLAG_UNK1 == 0x400,
              "`test ah, 4` at 0x6FB2E04F and `push 400h` at 0x6FB2E056");
static_assert(ITEMACTION_REMOVEFROMSHOP == 12,
              "`cmp al, 0Ch` at 0x6FB2D804 on the packet's action byte");
static_assert(offsetof(D2GSPacketSrv9C, nItemId) == 4,
              "`mov ecx, [edi+4]` at 0x6FB2D7F2 -- the item GUID, where packet 0x9D "
              "keeps it too");


//1.10f: D2Client.0x6FB2D300
//  Item message 0x17 of packet 0x9D: the weapon switch, the client half of CMDTBL_ActionSwapWeapons. Each message rebuilds one item into one
//  body location, and it is always a rebuild: the assert is ITEMSTestFlags(ptInfo->dwFlags, ITEM_FLAG_RECREATE), on IFLAG_NEWITEM.
//  Before the rebuild the old copy goes, by GUID, and so does whatever holds the destination body location (the two repeated blocks). Then only the
//  two active hands, RARM and LARM, get the weapon GUID, the components, the socket modifiers, the class cache and the animation rebuild; an
//  item switched into BODYLOC_SWRARM or BODYLOC_SWLARM gets the two refreshes and nothing else.
//  A failed INVENTORY_PlaceItemInBodyLoc returns silently here, where 0x06, 0x07 and 0x09 trace
//  "[INVENTORY ERROR] Unable to insert item into unit's body location!". The original inlines ITEM_CacheWeaponClassForBodyLoc here.
void __fastcall ITEM_ItemMsg17_WeaponSwitch(D2GSPacketSrv9D *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 13, 0, &tItemSave);

    D2UnitStrc *pPlayer = CUNIT_FindUnitByGuidAndType(pPacket->nUnitId, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pPlayer)
        return;

    if (pPlayer->dwUnitType != UNIT_PLAYER)
    {
        FOG_DisplayAssert(szAssertUnitIsPlayer, szItemCpp, 1849);
        exit(-1);
    }

    D2InventoryStrc *pInventory = pPlayer->pInventory;
    if (!pInventory)
    {
        FOG_DisplayAssert(szAssertHInventory, szItemCpp, 1852);
        exit(-1);
    }

    if (!D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_NEWITEM))
    {
        FOG_DisplayAssert(szAssertRecreateFlag, szItemCpp, 1854);
        exit(-1);
    }

    GFX_StopIfAlive(pPlayer);

    D2UnitStrc *pOldItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);
    if (pOldItem)
    {
        //  Repeated block (also the one below and message 0x06's new-item arm; no function has it out of line): takes the item off its owner and out
        //  of the unit table.
        D2InventoryStrc *pOldInventory = pPlayer->pInventory;
        const int32_t nOldBodyLoc = D2COMMON_ITEMS_GetBodyLocation(pOldItem);
        if (nOldBodyLoc == BODYLOC_RARM || nOldBodyLoc == BODYLOC_LARM)
        {
            D2COMMON_INVENTORY_UpdateWeaponGUIDOnRemoval(pOldInventory, pOldItem);
            GFX_ClearUnitModesLoaded(pPlayer, (D2Composits)D2COMMON_UNITS_GetItemComponentId(pPlayer, pOldItem));
            D2COMMON_INVENTORY_GetSecondWieldingWeapon(pPlayer, pOldInventory, &pOldItem, (D2C_PlayerBodyLocs)nOldBodyLoc);
        }

        D2UnitStrc *pOldRemoved = D2COMMON_INVENTORY_RemoveItemFromInventory(pOldInventory, pOldItem);
        if (pOldRemoved)
        {
            ITEM_DetachItemFromUnit(pOldRemoved, pPlayer, 0, 0);
            D2COMMON_10254_Return0(pOldInventory, nOldBodyLoc);
        }

        if (pOldItem)
            CUNIT_RemoveUnitById(pOldItem->dwUnitId, pOldItem->dwUnitType);
        else
            CUNIT_RemoveUnitById(-1, UNIT_TYPES_COUNT);
    }

    D2UnitStrc *pBodyItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, (D2C_PlayerBodyLocs)tItemSave.nBodyLoc);
    if (pBodyItem)
    {
        //  Repeated block: the same, for whatever holds the destination body location.
        D2InventoryStrc *pBodyInventory = pPlayer->pInventory;
        const int32_t nBodyItemLoc = D2COMMON_ITEMS_GetBodyLocation(pBodyItem);
        if (nBodyItemLoc == BODYLOC_RARM || nBodyItemLoc == BODYLOC_LARM)
        {
            D2COMMON_INVENTORY_UpdateWeaponGUIDOnRemoval(pBodyInventory, pBodyItem);
            GFX_ClearUnitModesLoaded(pPlayer, (D2Composits)D2COMMON_UNITS_GetItemComponentId(pPlayer, pBodyItem));
            D2COMMON_INVENTORY_GetSecondWieldingWeapon(pPlayer, pBodyInventory, &pBodyItem, (D2C_PlayerBodyLocs)nBodyItemLoc);
        }

        D2UnitStrc *pBodyRemoved = D2COMMON_INVENTORY_RemoveItemFromInventory(pBodyInventory, pBodyItem);
        if (pBodyRemoved)
        {
            ITEM_DetachItemFromUnit(pBodyRemoved, pPlayer, 0, 0);
            D2COMMON_10254_Return0(pBodyInventory, nBodyItemLoc);
        }

        if (pBodyItem)
            CUNIT_RemoveUnitById(pBodyItem->dwUnitId, pBodyItem->dwUnitType);
        else
            CUNIT_RemoveUnitById(-1, UNIT_TYPES_COUNT);
    }

    D2UnitStrc *pNewItem = ITEM_CreateItemFromItemUnitExMsg(pPacket, &tItemSave, 0, 0, 0);
    if (!pNewItem)
    {
        FOG_DisplayAssert(szAssertHItem, szItemCpp, 1870);
        exit(-1);
    }

    D2COMMON_ITEMS_SetBodyLocation(pNewItem, tItemSave.nBodyLoc);

    D2C_NodePages nNodePage = NODEPAGE_EQUIP;
    if (tItemSave.nBodyLoc == BODYLOC_SWRARM || tItemSave.nBodyLoc == BODYLOC_SWLARM)
        nNodePage = (D2C_NodePages)4;

    if (!D2COMMON_INVENTORY_PlaceItemInSocket(pInventory, pNewItem, nNodePage))
    {
        FOG_Trace(szTraceAddNullItem);
        return;
    }

    if (!D2COMMON_INVENTORY_PlaceItemInBodyLoc(pInventory, pNewItem, (D2C_PlayerBodyLocs)tItemSave.nBodyLoc))
        return;

    if (tItemSave.nBodyLoc == BODYLOC_RARM || tItemSave.nBodyLoc == BODYLOC_LARM)
    {
        D2COMMON_INVENTORY_UpdateWeaponGUIDOnInsert(pInventory, pNewItem);
        GFX_SetAllUnitModesLoaded(pPlayer, 0);
        ITEM_ApplyGemMods(pNewItem, pPlayer, 0, 0);
        ITEM_RefreshActiveItems(pPlayer);
        ITEM_RefreshActiveItems(pPlayer);

        ITEM_CacheWeaponClassForBodyLoc(pNewItem, tItemSave.nBodyLoc);

        GFX_RebuildUnitAnimForMode(pPlayer, 0);
        UI_PostSyntheticMouseMove();
        ITEM_ClearUninterruptableState(pNewItem);
    }
    else
    {
        ITEM_RefreshActiveItems(pPlayer);
        ITEM_RefreshActiveItems(pPlayer);
        UI_PostSyntheticMouseMove();
        ITEM_ClearUninterruptableState(pNewItem);
    }
}


//1.10f: D2Client.0x6FB2D760
//  Item messages 0x0B and 0x0C of packet 0x9C: an item added to, or taken out of, the store of the NPC the local player is talking to. The NPC
//  does not come from the packet: NPCMENU_GetInteractingNpc supplies its GUID, and the store is that monster's inventory.
//  Every failure traces and returns (a bad page, no inventory, a failed removal, a failed rebuild, a page conflict); the one assert is the
//  inlined uninterruptable clear at the very end. The add path tests nStorePage == 0xFF again after the entry test has already returned on it,
//  with nothing in between that writes the record: dead, and written because it is in the bytes.
//  An item that is created, or that exists and is not new, gets ITEMS_SetInvPage with the same page twice in a row (the first calls of the two
//  arms merge at 0x6FB2D9F1, the second is at 0x6FB2D9FE); a rebuilt IFLAG_NEWITEM item gets it once. The created item is not null-checked
//  before either call, nor before INVENTORY_CanItemBePlaced and ITEM_PlaceAtStorePage; the inlined "hItem" assert comes last.
//  The removal trace prints the item pointer with `%x`. The original inlines ITEM_SetItemDisplayState and ITEM_ClearUninterruptableState here.
void __fastcall ITEM_ItemMsg0B0C_UpdateNpcStore(D2GSPacketSrv9C *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 8, 0, &tItemSave);

    if (tItemSave.nStorePage == 0xFF)
    {
        FOG_Trace(szTraceInvalidBPage, szItemCpp);
        return;
    }

    D2UnitGUID nNpcGUID;
    if (!NPCMENU_GetInteractingNpc(&nNpcGUID))
        return;

    D2UnitStrc *pNpc = CUNIT_FindUnitByGuidAndType(nNpcGUID, UNIT_MONSTER);
    if (!pNpc)
        return;

    D2InventoryStrc *pInventory = pNpc->pInventory;
    if (!pInventory)
    {
        FOG_Trace(szTraceNullInventory, 1998, szItemCpp);
        return;
    }

    D2UnitStrc *pItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);

    if (pPacket->nAction == ITEMACTION_REMOVEFROMSHOP)
    {
        if (!pItem)
            return;

        if (NPCMENU_IsUiStateActive())
            INV_ResetItemDescState();
        else
            FOG_Trace(szTraceDeleteWithoutMenu, 2014, szItemCpp);

        if (!D2COMMON_INVENTORY_RemoveItemFromInventory(pInventory, pItem))
        {
            FOG_Trace(szTraceNullRemove, pItem, 2031, szItemCpp);
            return;
        }

        //  The byte the delete arm pushes four bytes wide.
        const uint8_t nInvPage = D2COMMON_ITEMS_GetInvPage(pItem);
        if (nInvPage == 0xFF)
        {
            FOG_Trace(szTraceInvalidPage, 255, 2038, szItemCpp);
            return;
        }

        const D2C_InventoryRecords nRecordId = (D2C_InventoryRecords)D2COMMON_UNITS_GetInventoryRecordId(pNpc, (D2C_ItemInvPage)nInvPage, GAME_IsExpansionGame());
        D2COMMON_INVENTORY_Return(szItemCpp, 2042, pInventory, tItemSave.nX, tItemSave.nY, nRecordId, 1, nInvPage);
        UI_PostSyntheticMouseMove();
        CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);
        NPCMENU_ClearSelectionIfStoreItem(pPacket->nItemId);
        return;
    }

    //  The test that cannot fire: the entry test has already returned on 0xFF.
    if (tItemSave.nStorePage == 0xFF)
    {
        FOG_Trace(szTraceInvalidPage, 255, 2051, szItemCpp);
        return;
    }

    if (!pItem)
    {
        pItem = ITEM_CreateItemFromItemExMsg(pPacket, &tItemSave, 0, 0, 0);
        D2COMMON_ITEMS_SetInvPage(pItem, tItemSave.nStorePage);
    }
    else
    {
        const BOOL bInParentInventory = D2COMMON_INVENTORY_CompareWithItemsParentInventory(pInventory, pItem);
        if (D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_NEWITEM))
        {
            CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);
            pItem = ITEM_CreateItemFromItemExMsg(pPacket, &tItemSave, 0, 0, 0);
            if (!pItem)
            {
                FOG_Trace(szTraceNullError, bInParentInventory, 2073, szItemCpp);
                return;
            }

            if (tItemSave.nStorePage != D2COMMON_ITEMS_GetInvPage(pItem))
            {
                FOG_Trace(szTraceAddPageConflict, tItemSave.nStorePage, D2COMMON_ITEMS_GetInvPage(pItem), bInParentInventory, 2078, szItemCpp);
                return;
            }
        }
        else
        {
            ITEM_SetItemDisplayState(pItem, 0, 0, IMODE_STORED, 0, tItemSave.nStorePage);
        }
    }

    D2COMMON_ITEMS_SetInvPage(pItem, tItemSave.nStorePage);

    D2UnitStrc *pExchangeItem;
    uint32_t nHoveredItems;
    const D2C_InventoryRecords nRecordId = (D2C_InventoryRecords)D2COMMON_UNITS_GetInventoryRecordId(pNpc, (D2C_ItemInvPage)tItemSave.nStorePage, GAME_IsExpansionGame());
    if (!D2COMMON_INVENTORY_CanItemBePlaced(pInventory, pItem, tItemSave.nX, tItemSave.nY, nRecordId, &pExchangeItem, &nHoveredItems, tItemSave.nStorePage))
        return;

    //  DIFFERENCE: the original pushes the page as a whole dword, two bytes of which are never written; ours passes the byte, which every
    //  reader narrows to anyway.
    ITEM_PlaceAtStorePage(tItemSave.nX, tItemSave.nY, tItemSave.nStorePage, pNpc, pItem);

    j_INV_NpcShopRequestPageRebuild(pNpc);

    ITEM_ClearUninterruptableState(pItem);
}


//1.10f: D2Client.0x6FB2DAC0
//  Item message 0x0D of packet 0x9C: two operations chosen by the item's current mode: IMODE_STORED takes it out of the player's grid and onto the
//  cursor, IMODE_ONCURSOR puts it from the cursor into the player's grid. Any other mode does nothing. Both arms look the item up a second time
//  (CUNIT_FindUnitByGuidAndType at 0x6FB2DB33 and 0x6FB2DCEE) after the entry lookup that chose the arm.
//  The stored arm runs its inlined ITEM_DetachItemFromUnit before INVENTORY_RemoveItemFromInventory, and hands INVENTORY_GetLeftHandWeapon the
//  inventory already in ebx rather than reading it again. The cursor arm tests the packet's page against 0xFF (`mov al, [..nStorePage]` at
//  0x6FB2DDD6) while the page the item goes onto is the item's own, read a moment earlier with ITEMS_GetInvPage. The coordinates come from
//  UNITS_GetXPosition and UNITS_GetYPosition, called here. ITEM_SetItemDisplayState is called in both arms and narrows the page to a byte.
//  The original inlines ITEM_DetachItemFromUnit here.
void __fastcall ITEM_ItemMsg0D_SwapInContainer(D2GSPacketSrv9C *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 8, 0, &tItemSave);

    D2UnitStrc *pEntryItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);
    if (!pEntryItem)
        return;

    if (pEntryItem->dwItemMode == IMODE_STORED)
    {
        D2UnitStrc *pPlayer = CUNIT_GetPlayer();
        if (!pPlayer)
        {
            FOG_DisplayAssert(szAssertHUnit, szItemCpp, 2170);
            exit(-1);
        }

        D2UnitStrc *pItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);
        const uint8_t nInvPage = D2COMMON_ITEMS_GetInvPage(pItem);
        D2InventoryStrc *pInventory = pPlayer->pInventory;
        if (!pInventory)
        {
            FOG_DisplayAssert(szAssertHInventory, szItemCpp, 2175);
            exit(-1);
        }

        if (pPlayer->dwUnitType == UNIT_PLAYER)
            D2COMMON_INVENTORY_GetLeftHandWeapon(pPlayer->pInventory);
        if (!D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_BROKEN, 150, szItemCpp))
            D2COMMON_STATLIST_ExpireUnitStatlist(pPlayer, pItem);

        D2UnitStrc *pRemoved = D2COMMON_INVENTORY_RemoveItemFromInventory(pInventory, pItem);
        if (!pRemoved)
        {
            FOG_DisplayAssert(szAssertHRemove, szItemCpp, 2180);
            exit(-1);
        }
        if (pRemoved != pItem)
        {
            FOG_DisplayAssert(szAssertHRemoveIsItem, szItemCpp, 2181);
            exit(-1);
        }

        const D2C_InventoryRecords nRecordId = (D2C_InventoryRecords)D2COMMON_UNITS_GetInventoryRecordId(pPlayer, (D2C_ItemInvPage)nInvPage, GAME_IsExpansionGame());
        D2COMMON_INVENTORY_Return(szItemCpp, 2184, pInventory, tItemSave.nX, tItemSave.nY, nRecordId, 1, nInvPage);
        INV_ResetItemDescState();

        if (D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_NEWITEM))
        {
            CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);
            pItem = ITEM_CreateItemFromItemExMsg(pPacket, &tItemSave, 0, 0, 0);
            if (!pItem)
            {
                FOG_DisplayAssert(szAssertHItem, szItemCpp, 2191);
                exit(-1);
            }
        }
        else
        {
            ITEM_SetItemDisplayState(pItem, 0, 0, tItemSave.nAnimMode, 0, tItemSave.nStorePage);
        }

        D2COMMON_INVENTORY_SetCursorItem(pInventory, pItem);
        CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pInventory));
        ITEM_ClearUninterruptableState(pItem);
    }
    else if (pEntryItem->dwItemMode == IMODE_ONCURSOR)
    {
        D2UnitStrc *pPlayer = CUNIT_GetPlayer();
        if (!pPlayer)
        {
            FOG_DisplayAssert(szAssertHUnit, szItemCpp, 2116);
            exit(-1);
        }

        D2UnitStrc *pItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);
        if (D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_NEWITEM))
        {
            CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);
            pItem = ITEM_CreateItemFromItemExMsg(pPacket, &tItemSave, 0, 0, 0);
            if (!pItem)
            {
                FOG_DisplayAssert(szAssertHHandItem, szItemCpp, 2124);
                exit(-1);
            }
        }
        else
        {
            ITEM_SetItemDisplayState(pItem, 0, 0, tItemSave.nAnimMode, 0, tItemSave.nStorePage);
        }

        D2InventoryStrc *pInventory = pPlayer->pInventory;
        if (!pInventory)
        {
            FOG_DisplayAssert(szAssertHInventory, szItemCpp, 2132);
            exit(-1);
        }

        D2COMMON_UNITS_SetXForStaticUnit(pItem, tItemSave.nX);
        D2COMMON_UNITS_SetYForStaticUnit(pItem, tItemSave.nY);

        if (!D2COMMON_INVENTORY_PlaceItemInSocket(pInventory, pItem, NODEPAGE_STORAGE))
        {
            FOG_Trace(szTraceAddNullItem);
            return;
        }

        const uint8_t nInvPage = D2COMMON_ITEMS_GetInvPage(pItem);
        if (tItemSave.nStorePage == 0xFF)
        {
            FOG_Trace(szTraceInvalidPage, 255, 2145, szItemCpp);
            return;
        }

        const D2C_InventoryRecords nRecordId = (D2C_InventoryRecords)D2COMMON_UNITS_GetInventoryRecordId(pPlayer, (D2C_ItemInvPage)nInvPage, GAME_IsExpansionGame());
        if (!D2COMMON_INVENTORY_PlaceItemAtInventoryPage(pInventory, pItem, UNITS_GetXPosition(pItem), UNITS_GetYPosition(pItem), nRecordId, 1, nInvPage))
        {
            FOG_DisplayAssert(szAssertZero, szItemCpp, 2150);
            exit(-1);
        }

        if (D2COMMON_ITEMS_IsCharmUsable(pItem, pPlayer))
            ITEM_ApplyGemMods(pItem, pPlayer, 0, 0);

        UI_PostSyntheticMouseMove();
        ITEM_ClearUninterruptableState(pItem);
    }
}


//1.10f: D2Client.0x6FB2DF10
//  Item message 0x0E of packet 0x9C: an item into a belt slot. It places the item with INVENTORY_PlaceItemInSocket(..., NODEPAGE_BELTSLOTS) and
//  INVENTORY_PlaceItemInBeltSlot and marks the column with PANEL_BeltSetColumnHasItem; the slot and the column are both the item's own X.
//  The unknown item is built announcing it (bAnnounce 1). Neither the lookup nor the creation is null-checked before the item is used.
//  `test ah, 4` at 0x6FB2E04F reads bit 10 of the decoded flags directly, not through ITEMS_CheckIfFlagIsSet as the unit's other flag tests go;
//  when it is set, ITEMS_SetItemFlag(pItem, IFLAG_UNK1, 1) copies it onto the item. The inlined ITEM_ClearUninterruptableState has no assert,
//  although the created item was never tested.
//  The original inlines ITEM_SetItemDisplayState, UNITS_GetXPosition and ITEM_ClearUninterruptableState here.
void __fastcall ITEM_ItemMsg0E_SetInvPage(D2GSPacketSrv9C *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 8, 0, &tItemSave);

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        FOG_DisplayAssert(szAssertHUnit, szItemCpp, 2243);
        exit(-1);
    }

    D2UnitStrc *pItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);
    if (pItem)
    {
        ITEM_SetItemDisplayState(pItem, 0, 0, tItemSave.nAnimMode, 0, tItemSave.nStorePage);
    }
    else
    {
        pItem = ITEM_CreateItemFromItemExMsg(pPacket, &tItemSave, 0, 0, 1);
    }

    D2COMMON_UNITS_SetXForStaticUnit(pItem, tItemSave.nX);
    D2COMMON_UNITS_SetYForStaticUnit(pItem, tItemSave.nY);

    D2InventoryStrc *pInventory = pPlayer->pInventory;
    if (!pInventory)
    {
        FOG_DisplayAssert(szAssertHInventory, szItemCpp, 2259);
        exit(-1);
    }

    if (D2COMMON_INVENTORY_PlaceItemInSocket(pInventory, pItem, NODEPAGE_BELTSLOTS))
    {
        int32_t nSlot = UNITS_GetXPosition(pItem);
        D2COMMON_INVENTORY_PlaceItemInBeltSlot(pInventory, pItem, nSlot);
    }

    if (D2COMMON_ITEMS_IsCharmUsable(pItem, pPlayer))
        ITEM_ApplyGemMods(pItem, pPlayer, 0, 0);

    if (tItemSave.dwFlags & IFLAG_UNK1)
        D2COMMON_ITEMS_SetItemFlag(pItem, IFLAG_UNK1, 1);

    D2COMMON_INVENTORY_SetCursorItem(pInventory, NULL);
    CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pInventory));

    uint32_t nColumn = UNITS_GetXPosition(pItem);
    PANEL_BeltSetColumnHasItem(nColumn, 1);

    UI_PostSyntheticMouseMove();

    ITEM_ClearUninterruptableState(pItem);

    NPCMENU_AckItemAction_SetInvPage(pItem->dwUnitId);
}


//  Item messages 0x0F and 0x10 of packet 0x9C and 0x11 and 0x13 of packet 0x9D: out of the belt, within the belt, off the body, into a socket.
//  D2COMMON_10267_Return joins 10254 in this unit: both belt messages call it with the slot the item came from, (pInventory, nX), where the
//  body-location messages call 10254 with the body location.

//  Assert and warning strings of the original.
static const char szAssertHRemoveIsBeltItem[] = "hRemove == hBeltItem";
static const char szWarnUnableToInsertInGrid[] = "Unable to insert item in grid";
static const char szAssertHCtrlInventory[] = "hCtrlInventory";
static_assert(sizeof(szAssertHRemoveIsBeltItem) == 20 + 1
                  && sizeof(szWarnUnableToInsertInGrid) == 29 + 1
                  && sizeof(szAssertHCtrlInventory) == 14 + 1,
              "the strings at 0x6FB910A8, 0x6FB910C0 and 0x6FB910E0, IDA sizes 21, 30 "
              "and 15");

//  Item type number used below.
static_assert(ITEMTYPE_CHARM == 13,
              "`push 0Dh` at 0x6FB2E522, the item type 0x10's cursor arm tests");
static_assert(INVPAGE_INVENTORY == 0,
              "`push 0` at 0x6FB2EA42, the page 0x11 asks the record id for");


//1.10f: D2Client.0x6FB2E100
//  Item message 0x0F of packet 0x9C: the item comes out of the local player's belt (the assert "hRemove == hBeltItem" names the belt). The column,
//  taken from the item's X in the packet, is marked empty; then the item either leaves the unit table (IFLAG_QUANTITY) or goes onto the cursor.
//  A failed removal returns silently, with no assert and no trace. The lookup's answer is not tested before INVENTORY_RemoveItemFromInventory and
//  the inlined ITEM_DetachItemFromUnit; the "ptItem" assert of the inlined ITEM_SetItemDisplayState is the first test of it, and only on the
//  cursor path. The belt bookkeeping (10267, PANEL_BeltResetHoverState, PANEL_BeltSetColumnHasItem) runs before the inlined detach here.
//  The original inlines ITEM_DetachItemFromUnit, ITEM_SetItemDisplayState and ITEM_ClearUninterruptableState here.
void __fastcall ITEM_ItemMsg0F_RemoveFromBelt(D2GSPacketSrv9C *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 8, 0, &tItemSave);

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        FOG_DisplayAssert(szAssertHUnit, szItemCpp, 2304);
        exit(-1);
    }

    D2UnitStrc *pItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);
    D2InventoryStrc *pInventory = pPlayer->pInventory;
    if (!pInventory)
    {
        FOG_DisplayAssert(szAssertHInventory, szItemCpp, 2309);
        exit(-1);
    }

    D2UnitStrc *pRemoved = D2COMMON_INVENTORY_RemoveItemFromInventory(pInventory, pItem);
    if (!pRemoved)
        return;

    if (pRemoved != pItem)
    {
        FOG_DisplayAssert(szAssertHRemoveIsBeltItem, szItemCpp, 2318);
        exit(-1);
    }

    D2COMMON_10267_Return(pInventory, tItemSave.nX);
    PANEL_BeltResetHoverState();
    PANEL_BeltSetColumnHasItem(tItemSave.nX, 0);

    ITEM_DetachItemFromUnit(pItem, pPlayer, 0, 0);

    if (D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_QUANTITY))
    {
        CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);
        CURSOR_SetHeldUnit(NULL);
        return;
    }

    ITEM_SetItemDisplayState(pItem, 0, 0, tItemSave.nAnimMode, 0, tItemSave.nStorePage);

    D2COMMON_INVENTORY_SetCursorItem(pInventory, pItem);
    CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pInventory));

    ITEM_ClearUninterruptableState(pItem);
}


//1.10f: D2Client.0x6FB2E2E0
//  Item message 0x10 of packet 0x9C: like 0x0D, two operations chosen by the item's current mode: IMODE_INBELT takes it out of the belt and onto
//  the cursor, IMODE_ONCURSOR puts it from the cursor into a belt slot. Anything else does nothing, and both arms look the item up a second time.
//  The belt arm returns quietly on no local player and on a failed second lookup, then asserts "hRemove" and "hRemove == hItem"; the cursor arm
//  asserts "hUnit" but returns quietly on a failed lookup. The belt arm calls ITEM_SetItemDisplayState (the page passed as a byte); the cursor arm
//  carries its body inlined. The cursor arm applies the socket modifiers when ITEMS_CheckItemTypeId(pItem, ITEMTYPE_CHARM), before placing the
//  item. The belt slot is the item's X through a called UNITS_GetXPosition, and a failed INVENTORY_PlaceItemInBeltSlot asserts "0".
//  The original inlines ITEM_DetachItemFromUnit, ITEM_ClearUninterruptableState and ITEM_SetItemDisplayState here.
void __fastcall ITEM_ItemMsg10_SwapInBelt(D2GSPacketSrv9C *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 8, 0, &tItemSave);

    D2UnitStrc *pEntryItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);
    if (!pEntryItem)
        return;

    if (pEntryItem->dwItemMode == IMODE_INBELT)
    {
        D2UnitStrc *pPlayer = CUNIT_GetPlayer();
        if (!pPlayer)
            return;

        D2UnitStrc *pItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);
        if (!pItem)
            return;

        D2InventoryStrc *pInventory = pPlayer->pInventory;
        if (!pInventory)
        {
            FOG_DisplayAssert(szAssertHInventory, szItemCpp, 2401);
            exit(-1);
        }

        D2UnitStrc *pRemoved = D2COMMON_INVENTORY_RemoveItemFromInventory(pInventory, pItem);
        if (!pRemoved)
        {
            FOG_DisplayAssert(szAssertHRemove, szItemCpp, 2403);
            exit(-1);
        }
        if (pRemoved != pItem)
        {
            FOG_DisplayAssert(szAssertHRemoveIsItem, szItemCpp, 2404);
            exit(-1);
        }

        ITEM_DetachItemFromUnit(pItem, pPlayer, 0, 0);

        D2COMMON_10267_Return(pInventory, tItemSave.nX);
        ITEM_SetItemDisplayState(pItem, 0, 0, tItemSave.nAnimMode, 0, tItemSave.nStorePage);
        D2COMMON_INVENTORY_SetCursorItem(pInventory, pItem);
        CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pInventory));

        ITEM_ClearUninterruptableState(pItem);
    }
    else if (pEntryItem->dwItemMode == IMODE_ONCURSOR)
    {
        D2UnitStrc *pPlayer = CUNIT_GetPlayer();
        if (!pPlayer)
        {
            FOG_DisplayAssert(szAssertHUnit, szItemCpp, 2347);
            exit(-1);
        }

        D2UnitStrc *pItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);
        if (!pItem)
            return;

        ITEM_SetItemDisplayState(pItem, 0, 0, tItemSave.nAnimMode, 0, tItemSave.nStorePage);

        D2InventoryStrc *pInventory = pPlayer->pInventory;
        if (!pInventory)
        {
            FOG_DisplayAssert(szAssertHInventory, szItemCpp, 2359);
            exit(-1);
        }

        D2COMMON_UNITS_SetXForStaticUnit(pItem, tItemSave.nX);
        D2COMMON_UNITS_SetYForStaticUnit(pItem, tItemSave.nY);

        if (D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_CHARM))
            ITEM_ApplyGemMods(pItem, pPlayer, 0, 0);

        if (D2COMMON_INVENTORY_PlaceItemInSocket(pInventory, pItem, NODEPAGE_BELTSLOTS))
        {
            if (!D2COMMON_INVENTORY_PlaceItemInBeltSlot(pInventory, pItem, UNITS_GetXPosition(pItem)))
            {
                FOG_DisplayAssert(szAssertZero, szItemCpp, 2372);
                exit(-1);
            }
        }

        UI_PostSyntheticMouseMove();
        ITEM_ClearUninterruptableState(pItem);
    }
}


//1.10f: D2Client.0x6FB2EA00
//  Item message 0x11 of packet 0x9D: the server takes an item off a unit. The item in the packet's body location is removed and unwound, and
//  then, for the local player only, put back into the inventory grid at the packet's (nX, nY), with the record id worked out for
//  INVPAGE_INVENTORY before anything else happened.
//  The uninterruptable state is cleared twice: between ITEMS_UpdateSets and D2COMMON_10254_Return0 on the removed item (its assert folded under
//  the "hRemove" test), and in the local player's tail on the item it placed (assert kept). They are the same unit unless IFLAG_NEWITEM rebuilt
//  it in between. INVENTORY_PlaceItemAtInventoryPage failing is a FOG_DisplayWarning ("Unable to insert item in grid"), after which the
//  function goes on to the refreshes; INVENTORY_PlaceItemInSocket failing after a successful grid placement traces
//  "[INVENTORY ERROR] Trying to add a NULL item!" and returns without them. The grid is tried first here.
//  The original inlines ITEM_DetachItemFromUnit, ITEM_ClearUninterruptableState and ITEM_SetItemDisplayState here.
void __fastcall ITEM_ItemMsg11_AutoUnequip(D2GSPacketSrv9D *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 13, 0, &tItemSave);

    D2UnitStrc *pOwner = CUNIT_FindUnitByGuidAndType(pPacket->nUnitId, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pOwner)
        return;

    const D2C_InventoryRecords nRecordId = (D2C_InventoryRecords)D2COMMON_UNITS_GetInventoryRecordId(pOwner, INVPAGE_INVENTORY, GAME_IsExpansionGame());

    D2InventoryStrc *pInventory = pOwner->pInventory;
    if (!pInventory)
    {
        FOG_DisplayAssert(szAssertHInventory, szItemCpp, 2683);
        exit(-1);
    }

    D2UnitStrc *pItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, (D2C_PlayerBodyLocs)tItemSave.nBodyLoc);
    if (!pItem)
    {
        FOG_DisplayAssert(szAssertHItem, szItemCpp, 2685);
        exit(-1);
    }

    GFX_StopIfAlive(pOwner);
    D2COMMON_INVENTORY_UpdateWeaponGUIDOnRemoval(pInventory, pItem);
    GFX_ClearUnitModesLoaded(pOwner, (D2Composits)pPacket->nComponent);
    D2COMMON_INVENTORY_GetSecondWieldingWeapon(pOwner, pInventory, &pItem, (D2C_PlayerBodyLocs)tItemSave.nBodyLoc);

    D2UnitStrc *pRemoved = D2COMMON_INVENTORY_RemoveItemFromInventory(pInventory, pItem);
    if (!pRemoved)
    {
        FOG_DisplayAssert(szAssertHRemove, szItemCpp, 2691);
        exit(-1);
    }
    if (pRemoved != pItem)
    {
        FOG_DisplayAssert(szAssertHRemoveIsItem, szItemCpp, 2692);
        exit(-1);
    }

    ITEM_DetachItemFromUnit(pRemoved, pOwner, 0, 0);

    D2COMMON_ITEMS_UpdateSets(pOwner, pRemoved, 1, 1);

    ITEM_ClearUninterruptableState(pRemoved);

    D2COMMON_10254_Return0(pInventory, tItemSave.nBodyLoc);
    GFX_RebuildUnitAnimForMode(pOwner, 0);

    if (D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_NEWITEM))
    {
        CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);
        if (pOwner == CUNIT_GetPlayer())
        {
            pItem = ITEM_CreateItemFromItemUnitExMsg(pPacket, &tItemSave, 0, 0, 0);
            if (!pItem)
            {
                FOG_DisplayAssert(szAssertHItem, szItemCpp, 2705);
                exit(-1);
            }
        }
    }
    else
    {
        ITEM_SetItemDisplayState(pItem, 0, 0, tItemSave.nAnimMode, 0, tItemSave.nStorePage);
    }

    if (pOwner != CUNIT_GetPlayer())
        return;

    INV_ResetItemDescState();

    if (tItemSave.nBodyLoc == BODYLOC_RARM)
        gnWeaponClassRArm = 0;
    else if (tItemSave.nBodyLoc == BODYLOC_LARM)
        gnWeaponClassLArm = 0;

    if (D2COMMON_INVENTORY_PlaceItemAtInventoryPage(pInventory, pItem, tItemSave.nX, tItemSave.nY, nRecordId, 1, 0))
    {
        if (!D2COMMON_INVENTORY_PlaceItemInSocket(pInventory, pItem, NODEPAGE_STORAGE))
        {
            FOG_Trace(szTraceAddNullItem);
            return;
        }

        D2COMMON_ITEMS_SetBodyLocation(pItem, 0);
        D2COMMON_INVENTORY_SetCursorItem(pInventory, NULL);
        CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pInventory));
    }
    else
    {
        FOG_DisplayWarning(szWarnUnableToInsertInGrid, szItemCpp, 2737);
    }

    ITEM_RefreshActiveItems(pOwner);
    ITEM_RefreshActiveItems(pOwner);

    ITEM_ClearUninterruptableState(pItem);
}


//1.10f: D2Client.0x6FB2EEC0
//  Item message 0x13 of packet 0x9D: a filler into a socket. The packet's (nUnitType, nUnitId) names the item taking the filler; the filler goes
//  into that unit's inventory with INVENTORY_PlaceItemInSocket(..., NODEPAGE_STORAGE), and its modifiers are applied with
//  ITEM_ApplyGemMods(pItem, pTarget, 1, 0). A known filler is re-moded at the packet's (nX, nY); an unknown one is built, announcing nothing.
//  The two share one `push 0` ahead of the branch (0x6FB2EF09): bInitStats for the first call, bAnnounce for the second.
//  The original inlines ITEM_SetItemDisplayState and ITEM_ClearUninterruptableState here.
void __fastcall ITEM_ItemMsg13_ItemInSocket(D2GSPacketSrv9D *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 13, 0, &tItemSave);

    D2UnitStrc *pTarget = CUNIT_FindUnitByGuidAndType(pPacket->nUnitId, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pTarget)
        return;

    D2UnitStrc *pItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);
    if (pItem)
    {
        ITEM_SetItemDisplayState(pItem, tItemSave.nX, tItemSave.nY, tItemSave.nAnimMode, 0, tItemSave.nStorePage);
    }
    else
    {
        pItem = ITEM_CreateItemFromItemUnitExMsg(pPacket, &tItemSave, 0, 0, 0);
    }

    D2InventoryStrc *pTargetInventory = pTarget->pInventory;
    if (!pTargetInventory)
    {
        FOG_DisplayAssert(szAssertHInventory, szItemCpp, 2822);
        exit(-1);
    }

    if (D2COMMON_INVENTORY_PlaceItemInSocket(pTargetInventory, pItem, NODEPAGE_STORAGE))
    {
        if (!D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_DELETED))
        {
            //  BUG: unless the item carries IFLAG_DELETED the cursor is cleared through CUNIT_GetPlayer (0x6FB2EF9E) whose answer is dereferenced
            //  (`mov edi, [eax+60h]`) without a test, and only then asserted as "hCtrlInventory". UNREACHABLE: a stock server sends message 0x13
            //  only with IFLAG_DELETED raised.
            D2InventoryStrc *pCtrlInventory = CUNIT_GetPlayer()->pInventory;
            if (!pCtrlInventory)
            {
                FOG_DisplayAssert(szAssertHCtrlInventory, szItemCpp, 2828);
                exit(-1);
            }
            D2COMMON_INVENTORY_SetCursorItem(pCtrlInventory, NULL);
            CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pCtrlInventory));
        }

        ITEM_ApplyGemMods(pItem, pTarget, 1, 0);
    }

    ITEM_ClearUninterruptableState(pItem);
}


//  Item messages 0x14, 0x15 and 0x16 of packet 0x9D. The bodies say what the messages do; the original's names for the actions are not known,
//  so they keep their numbers.

//  Assert, halt and trace strings of the original.
static const char szAssertHGridItemIsRemoved[] = "hGridItem == hRemove";
static const char szAssertHGridItem[] = "hGridItem";
static const char szAssertHRemoveIsGridItem[] = "hRemove == hGridItem";
static const char szAssertHRemoveIsBodyItem[] = "hRemove == hBodyItem";
static const char szAssertHNewItem[] = "hNewItem";
static const char szHaltRecreateItemBelt[] = "[RECREATE ITEM BELT] Could not add to inventory!";
static const char szTraceBeltQuit[] = "Unable to insert item in belt at %d in %s.  Quitting game";
static_assert(sizeof(szAssertHGridItemIsRemoved) == 20 + 1
                  && sizeof(szAssertHGridItem) == 9 + 1
                  && sizeof(szAssertHRemoveIsGridItem) == 20 + 1
                  && sizeof(szAssertHRemoveIsBodyItem) == 20 + 1
                  && sizeof(szAssertHNewItem) == 8 + 1,
              "the asserts at 0x6FB910FC, 0x6FB910F0, 0x6FB911C0, 0x6FB911A8 and "
              "0x6FB911D8, IDA sizes 21, 10, 21, 21 and 9");
static_assert(sizeof(szHaltRecreateItemBelt) == 48 + 1 && sizeof(szTraceBeltQuit) == 57 + 1,
              "the halt at 0x6FB91114 and the trace at 0x6FB91148, IDA sizes 49 and 58");

static const char szAssertBModeIsInvBelt[] = "tInfo.bMode == ITEM_MODE_INVBELT";
static_assert(sizeof(szAssertBModeIsInvBelt) == 32 + 1,
              "0x6FB91184: 32 characters to the NUL, walked byte by byte at the "
              "unit's close");


//1.10f: D2Client.0x6FB2F050
//  Item message 0x14 of packet 0x9D: an item goes onto a body location of the unit the packet names, and when that unit is the local player it is
//  first taken out of the inventory grid ("hGridItem == hRemove") and its cells returned with INVENTORY_Return on INVPAGE_INVENTORY. Only that
//  branch looks the item up. Then the item itself: a new one is dropped from the unit table and built again ("hGridItem"); one the lookup found
//  is re-moded where it is; any other is built ("hGridItem"), which for an owner that is not the local player is every item. One `push 0` ahead
//  of the last two (0x6FB2F19B) is bInitStats on one side and bAnnounce on the other, as in 0x13.
//  The placement follows message 0x06's order (ITEMS_SetBodyLocation, the node page, INVENTORY_PlaceItemInSocket, the forced-slot 10254,
//  INVENTORY_PlaceItemInBodyLoc, three of 0x06's four [INVENTORY ERROR] traces, GFX_StopIfAlive, the animation rebuild, the two refreshes and the
//  uninterruptable clear). Unlike 0x06 it does not test for the local player, returns without a trace for an unknown owner, asserts the
//  inventory early ("hInventory"), applies the socket modifiers for every body location and sends no acknowledgement.
//  The original inlines ITEM_SetItemDisplayState and ITEM_ClearUninterruptableState here.
void __fastcall ITEM_ItemMsg14_Unknown(D2GSPacketSrv9D *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 13, 0, &tItemSave);

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    D2UnitStrc *pOwner = CUNIT_FindUnitByGuidAndType(pPacket->nUnitId, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pOwner)
        return;

    D2InventoryStrc *pInventory = pOwner->pInventory;
    if (!pInventory)
    {
        FOG_DisplayAssert(szAssertHInventory, szItemCpp, 2889);
        exit(-1);
    }

    D2UnitStrc *pItem = NULL;
    if (pOwner == pPlayer)
    {
        pItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);
        if (!pItem)
            return;

        if (pItem != D2COMMON_INVENTORY_RemoveItemFromInventory(pInventory, pItem))
        {
            FOG_DisplayAssert(szAssertHGridItemIsRemoved, szItemCpp, 2901);
            exit(-1);
        }

        const D2C_InventoryRecords nRecordId = (D2C_InventoryRecords)D2COMMON_UNITS_GetInventoryRecordId(pOwner, INVPAGE_INVENTORY, GAME_IsExpansionGame());
        D2COMMON_INVENTORY_Return(szItemCpp, 2903, pInventory, tItemSave.nX, tItemSave.nY, nRecordId, 1, 0);
        INV_ResetItemDescState();
        UI_PostSyntheticMouseMove();
    }

    if (D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_NEWITEM))
    {
        CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);
        pItem = ITEM_CreateItemFromItemUnitExMsg(pPacket, &tItemSave, 0, 0, 0);
        if (!pItem)
        {
            FOG_DisplayAssert(szAssertHGridItem, szItemCpp, 2911);
            exit(-1);
        }
    }
    else if (pItem)
    {
        ITEM_SetItemDisplayState(pItem, 0, 0, tItemSave.nAnimMode, 0, tItemSave.nStorePage);
    }
    else
    {
        pItem = ITEM_CreateItemFromItemUnitExMsg(pPacket, &tItemSave, 0, 0, 0);
        if (!pItem)
        {
            FOG_DisplayAssert(szAssertHGridItem, szItemCpp, 2917);
            exit(-1);
        }
    }

    D2COMMON_ITEMS_SetBodyLocation(pItem, tItemSave.nBodyLoc);

    D2C_NodePages nNodePage = NODEPAGE_EQUIP;
    if (tItemSave.nBodyLoc == BODYLOC_SWRARM || tItemSave.nBodyLoc == BODYLOC_SWLARM)
        nNodePage = (D2C_NodePages)4;

    if (!D2COMMON_INVENTORY_PlaceItemInSocket(pInventory, pItem, nNodePage))
    {
        FOG_Trace(szTraceAddNullItem);
        return;
    }

    const int32_t nBodyLoc = tItemSave.nBodyLoc;
    if (D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, (D2C_PlayerBodyLocs)nBodyLoc))
    {
        D2COMMON_10254_Return0(pInventory, nBodyLoc);
        FOG_Trace(szTraceForcedIntoBody);
    }

    if (!D2COMMON_INVENTORY_PlaceItemInBodyLoc(pInventory, pItem, (D2C_PlayerBodyLocs)nBodyLoc))
    {
        FOG_Trace(szTraceUnableToInsertIntoBody);
        return;
    }

    GFX_StopIfAlive(pOwner);
    D2COMMON_INVENTORY_UpdateWeaponGUIDOnInsert(pInventory, pItem);
    GFX_RebuildUnitAnimForMode(pOwner, 0);
    ITEM_ApplyGemMods(pItem, pOwner, 0, 0);
    ITEM_RefreshActiveItems(pOwner);
    ITEM_RefreshActiveItems(pOwner);

    ITEM_ClearUninterruptableState(pItem);
}


//1.10f: D2Client.0x6FB2F300
//  Item message 0x15 of packet 0x9D, the largest function of the unit. An item whose stats changed is not patched: its unit is thrown away and
//  rebuilt from the packet, in whichever of four homes the decoded mode names (the original's own word is in its halt text,
//  "[RECREATE ITEM BELT] Could not add to inventory!"):
//      IMODE_STORED    the local player's grid: out, cells returned, rebuilt and placed through ITEM_PlaceAtStorePage
//      IMODE_EQUIP     any player's body: unequipped with sets and stat list unwound, rebuilt and equipped again; for the local player the
//                      class cache and the cursor too
//      IMODE_INBELT    the local player's belt, through the item's X
//      IMODE_ONCURSOR  the local player's cursor
//  An unknown owner, one that is not a player or has no inventory, a mode with no arm, a local-player mode for anyone else and an empty body
//  location all return NULL; so does every traced failure on the way. The return is real: six of the nine exits zero eax before `retn`, two leave
//  the NULL a test found in it, and the common tail returns the rebuilt item (0x6FB2FA6B) or the zero put in esi at 0x6FB2F361 on the two jumps
//  that skip every arm.
//  When INVENTORY_PlaceItemInBeltSlot refuses the rebuilt item, the arm calls MSG_Send_D2CLTSYS_ENDGAME, WINMAIN_SetAppModeContinue(0),
//  GAME_SetLeaveGameSent(1) and GAME_SetDisconnecting (the trace after them is "Unable to insert item in belt at %d in %s.  Quitting game")
//  and then carries on with the rest of the arm, because nothing there returns. A refused INVENTORY_PlaceItemInSocket one step earlier is a
//  FOG_DisplayHalt.
//  Inside the IMODE_INBELT arm, after the rebuild, the assert "tInfo.bMode == ITEM_MODE_INVBELT" tests tItemSave.nAnimMode, the byte the arm was
//  chosen by, so it cannot fire. All three removals assert with a single comparison and no separate null test. The grid arm calls
//  INVENTORY_Return before it tests the page for 0xFF. The body arm writes NULL into its item slot after destroying the unit (0x6FB2F5C4) and
//  never reads it again, and destroys that unit through the same dead `pItem ? id : -1` guard as 0x06. The belt arm's rebuilt item gets overlay
//  70 when the decoded flags carry IFLAG_UNK1, and the arm skips the common UI_PostSyntheticMouseMove, having made its own.
//  The original inlines ITEM_DetachItemFromUnit, UNITS_GetXPosition, ITEM_CacheWeaponClassForBodyLoc and ITEM_ClearUninterruptableState here.
D2UnitStrc *__fastcall ITEM_ItemMsg15_UpdateStats(D2GSPacketSrv9D *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 13, 0, &tItemSave);

    D2UnitStrc *pPlayer = CUNIT_GetPlayer();
    D2UnitStrc *pOwner = CUNIT_FindUnitByGuidAndType(pPacket->nUnitId, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pOwner)
        return NULL;

    D2InventoryStrc *pInventory = pOwner->pInventory;
    if (!pInventory)
        return NULL;

    D2UnitStrc *pNewItem = NULL;
    if (pOwner->dwUnitType != UNIT_PLAYER)
        return NULL;

    if (pOwner == pPlayer && tItemSave.nAnimMode == IMODE_STORED)
    {
        D2UnitStrc *pGridItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);
        if (pGridItem)
        {
            D2UnitStrc *pRemoved = D2COMMON_INVENTORY_RemoveItemFromInventory(pInventory, pGridItem);
            if (pRemoved != pGridItem)
            {
                FOG_DisplayAssert(szAssertHRemoveIsGridItem, szItemCpp, 3013);
                exit(-1);
            }

            ITEM_DetachItemFromUnit(pRemoved, pOwner, 0, 0);

            CUNIT_RemoveUnitById(pGridItem->dwUnitId, pGridItem->dwUnitType);
        }

        INV_ResetItemDescState();

        const D2C_InventoryRecords nRecordId = (D2C_InventoryRecords)D2COMMON_UNITS_GetInventoryRecordId(pOwner, (D2C_ItemInvPage)tItemSave.nStorePage, GAME_IsExpansionGame());
        D2COMMON_INVENTORY_Return(szItemCpp, 3020, pInventory, tItemSave.nX, tItemSave.nY, nRecordId, 1, tItemSave.nStorePage);

        if (tItemSave.nStorePage == 0xFF)
        {
            FOG_Trace(szTraceInvalidPage, 255, 3024, szItemCpp);
            return NULL;
        }

        pNewItem = ITEM_CreateItemFromItemUnitExMsg(pPacket, &tItemSave, 0, 0, 0);

        //  DIFFERENCE: the original pushes the page as a whole dword, two bytes of which are never written; ours passes the byte, which every
        //  reader narrows to anyway.
        ITEM_PlaceAtStorePage(tItemSave.nX, tItemSave.nY, tItemSave.nStorePage, pOwner, pNewItem);

        if (D2COMMON_ITEMS_IsCharmUsable(pNewItem, pOwner))
            ITEM_ApplyGemMods(pNewItem, pOwner, 0, 0);

        UI_PostSyntheticMouseMove();
    }
    else if (tItemSave.nAnimMode == IMODE_EQUIP)
    {
        D2UnitStrc *pItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, (D2C_PlayerBodyLocs)tItemSave.nBodyLoc);
        GFX_StopIfAlive(pOwner);
        if (!pItem)
            return NULL;

        D2COMMON_INVENTORY_UpdateWeaponGUIDOnRemoval(pInventory, pItem);
        GFX_ClearUnitModesLoaded(pOwner, (D2Composits)pPacket->nComponent);
        D2COMMON_INVENTORY_GetSecondWieldingWeapon(pOwner, pInventory, &pItem, (D2C_PlayerBodyLocs)tItemSave.nBodyLoc);

        D2UnitStrc *pRemoved = D2COMMON_INVENTORY_RemoveItemFromInventory(pInventory, pItem);
        if (pRemoved != pItem)
        {
            FOG_DisplayAssert(szAssertHRemoveIsBodyItem, szItemCpp, 3052);
            exit(-1);
        }

        ITEM_DetachItemFromUnit(pRemoved, pOwner, 0, 0);

        D2COMMON_ITEMS_UpdateSets(pOwner, pRemoved, 1, 1);
        D2COMMON_10254_Return0(pInventory, tItemSave.nBodyLoc);
        GFX_RebuildUnitAnimForMode(pOwner, 0);

        //  The guard that cannot fire, as in 0x06.
        if (pItem)
            CUNIT_RemoveUnitById(pItem->dwUnitId, pItem->dwUnitType);
        else
            CUNIT_RemoveUnitById(-1, UNIT_TYPES_COUNT);
        pItem = NULL;

        if (pOwner == pPlayer)
        {
            INV_ResetItemDescState();

            if (tItemSave.nBodyLoc == BODYLOC_RARM)
                gnWeaponClassRArm = 0;
            else if (tItemSave.nBodyLoc == BODYLOC_LARM)
                gnWeaponClassLArm = 0;
        }

        pNewItem = ITEM_CreateItemFromItemUnitExMsg(pPacket, &tItemSave, 0, 0, 0);
        D2COMMON_ITEMS_SetBodyLocation(pNewItem, tItemSave.nBodyLoc);

        D2C_NodePages nNodePage = NODEPAGE_EQUIP;
        if (tItemSave.nBodyLoc == BODYLOC_SWRARM || tItemSave.nBodyLoc == BODYLOC_SWLARM)
            nNodePage = (D2C_NodePages)4;

        if (!D2COMMON_INVENTORY_PlaceItemInSocket(pInventory, pNewItem, nNodePage))
        {
            FOG_Trace(szTraceAddNullItem);
            return NULL;
        }

        const int32_t nBodyLoc = tItemSave.nBodyLoc;
        if (!pNewItem)
        {
            FOG_Trace(szTraceInsertNullIntoBody);
            return NULL;
        }

        if (D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, (D2C_PlayerBodyLocs)nBodyLoc))
        {
            D2COMMON_10254_Return0(pInventory, nBodyLoc);
            FOG_Trace(szTraceForcedIntoBody);
        }

        if (!D2COMMON_INVENTORY_PlaceItemInBodyLoc(pInventory, pNewItem, (D2C_PlayerBodyLocs)nBodyLoc))
        {
            FOG_Trace(szTraceUnableToInsertIntoBody);
            return NULL;
        }

        D2COMMON_INVENTORY_UpdateWeaponGUIDOnInsert(pInventory, pNewItem);
        GFX_SetAllUnitModesLoaded(pOwner, 0);
        if (pPacket->nComponent == 0 || pPacket->nComponent == 1)
            GFX_ClearUnitModesLoaded(pOwner, (D2Composits)pPacket->nComponent);
        GFX_RebuildUnitAnimForMode(pOwner, 0);

        if (pOwner == pPlayer)
        {
            ITEM_CacheWeaponClassForBodyLoc(pNewItem, tItemSave.nBodyLoc);

            if (!D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_DELETED))
            {
                D2COMMON_INVENTORY_SetCursorItem(pInventory, NULL);
                CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pInventory));
            }
        }

        ITEM_ApplyGemMods(pNewItem, pOwner, 0, 0);
        ITEM_RefreshActiveItems(pOwner);
        ITEM_RefreshActiveItems(pOwner);
        UI_PostSyntheticMouseMove();
    }
    else
    {
        if (pOwner != pPlayer)
            return NULL;

        if (tItemSave.nAnimMode == IMODE_INBELT)
        {
            D2UnitStrc *pBeltItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);
            if (pBeltItem)
            {
                if (D2COMMON_INVENTORY_RemoveItemFromInventory(pInventory, pBeltItem) != pBeltItem)
                {
                    FOG_DisplayAssert(szAssertHRemoveIsBeltItem, szItemCpp, 3114);
                    exit(-1);
                }

                int32_t nOldSlot = UNITS_GetXPosition(pBeltItem);
                D2COMMON_10267_Return(pInventory, nOldSlot);

                uint32_t nOldColumn = UNITS_GetXPosition(pBeltItem);
                PANEL_BeltSetColumnHasItem(nOldColumn, 0);

                ITEM_DetachItemFromUnit(pBeltItem, pOwner, 0, 0);

                CUNIT_RemoveUnitById(pBeltItem->dwUnitId, pBeltItem->dwUnitType);
            }

            PANEL_BeltResetHoverState();
            pNewItem = ITEM_CreateItemFromItemUnitExMsg(pPacket, &tItemSave, 0, 0, 0);
            if (tItemSave.nAnimMode != IMODE_INBELT)
            {
                FOG_DisplayAssert(szAssertBModeIsInvBelt, szItemCpp, 3125);
                exit(-1);
            }

            D2COMMON_UNITS_SetXForStaticUnit(pNewItem, tItemSave.nX);
            D2COMMON_UNITS_SetYForStaticUnit(pNewItem, tItemSave.nY);

            if (!D2COMMON_INVENTORY_PlaceItemInSocket(pInventory, pNewItem, NODEPAGE_BELTSLOTS))
            {
                FOG_DisplayHalt(szHaltRecreateItemBelt, szItemCpp, 3155);
                exit(-1);
            }

            int32_t nSlot = UNITS_GetXPosition(pNewItem);

            if (!D2COMMON_INVENTORY_PlaceItemInBeltSlot(pInventory, pNewItem, nSlot))
            {
                MSG_Send_D2CLTSYS_ENDGAME();
                WINMAIN_SetAppModeContinue(0);
                GAME_SetLeaveGameSent(1);
                GAME_SetDisconnecting();
                FOG_Trace(szTraceBeltQuit, 3135, szItemCpp);
            }

            if (D2COMMON_ITEMS_IsCharmUsable(pNewItem, pOwner))
                ITEM_ApplyGemMods(pNewItem, pOwner, 0, 0);

            uint32_t nColumn = UNITS_GetXPosition(pNewItem);
            PANEL_BeltSetColumnHasItem(nColumn, 1);

            UI_PostSyntheticMouseMove();

            if (D2COMMON_ITEMS_CheckIfFlagIsSet(tItemSave.dwFlags, IFLAG_UNK1))
                GFX_AddOverlay(pNewItem, OVERLAY_ITEMGLEAM, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);
        }
        else if (tItemSave.nAnimMode == IMODE_ONCURSOR)
        {
            PANEL_BeltResetHoverState();

            D2UnitStrc *pOldItem = CUNIT_FindUnitByGuidAndType(pPacket->nItemId, UNIT_ITEM);
            if (pOldItem)
                CUNIT_RemoveUnitById(pOldItem->dwUnitId, pOldItem->dwUnitType);

            pNewItem = ITEM_CreateItemFromItemUnitExMsg(pPacket, &tItemSave, 0, 0, 0);
            D2COMMON_UNITS_SetXForStaticUnit(pNewItem, tItemSave.nX);
            D2COMMON_UNITS_SetYForStaticUnit(pNewItem, tItemSave.nY);
            D2COMMON_INVENTORY_SetCursorItem(pInventory, pNewItem);
            CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pInventory));
            UI_PostSyntheticMouseMove();
        }
        else
        {
            return NULL;
        }
    }

    if (pNewItem)
    {
        ITEM_ClearUninterruptableState(pNewItem);
    }

    return pNewItem;
}


//1.10f: D2Client.0x6FB2FD00
//  Item message 0x16 of packet 0x9D: the item is destroyed and rebuilt from the packet ("hNewItem"), its STAT_QUANTITY set to 1 on layer 0, and
//  put on the cursor of the unit the packet names -- whatever that unit is: neither its type nor its inventory is tested, where 0x15 requires a
//  player with one.
//  void: eax is left holding CURSOR_SetHeldUnit's answer, and no caller reads it.
void __fastcall ITEM_ItemMsg16_Unknown(D2GSPacketSrv9D *pPacket)
{
    D2ItemSaveStrc tItemSave;
    D2COMMON_ITEMS_GetCompactItemDataFromBitstream(pPacket->pBitstream, pPacket->nPacketSize - 13, 0, &tItemSave);

    D2UnitStrc *pOwner = CUNIT_FindUnitByGuidAndType(pPacket->nUnitId, (D2C_UnitTypes)pPacket->nUnitType);
    if (!pOwner)
        return;

    CUNIT_RemoveUnitById(pPacket->nItemId, UNIT_ITEM);
    D2UnitStrc *pNewItem = ITEM_CreateItemFromItemUnitExMsg(pPacket, &tItemSave, 0, 0, 0);
    if (!pNewItem)
    {
        FOG_DisplayAssert(szAssertHNewItem, szItemCpp, 3327);
        exit(-1);
    }

    D2COMMON_STATLIST_SetUnitStat(pNewItem, STAT_QUANTITY, 1, 0);

    D2InventoryStrc *pInventory = pOwner->pInventory;
    D2COMMON_INVENTORY_SetCursorItem(pInventory, pNewItem);
    CURSOR_SetHeldUnit(D2COMMON_INVENTORY_GetCursorItem(pInventory));
}

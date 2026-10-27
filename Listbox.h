//  Listbox.h -- the globals of Listbox.cpp that are ours: the list box's two vtables, the controls' abstract base vtable and the scroll bar's image.
//  Each is a C++17 inline variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above a global gives
//  its address in the original.
//  Include it after D2Client.h, which declares every function named here.
#pragma once

//  vcruntime's pure-virtual handler; no public header declares it. The twelve slots of gVtblUiControlBase hold it.
extern "C" int32_t __cdecl _purecall(void);

//  D2Client 0x6FB6F97C
//  The abstract base vtable of the UI controls, installed by LISTBOX_ImplConstruct, LISTBOX_UiCtrlBaseDestruct and LISTBOX_ProxyDestruct. All its
//  twelve slots are the pure-virtual handler.
inline const D2UiControlBaseVtbl gVtblUiControlBase =
{
    .pfnDeletingDtor = &_purecall,
    .pfnDraw = &_purecall,
    .pfnOnMouseWheel = &_purecall,
    .pfnOnMouseDown = &_purecall,
    .pfnOnMouseUp = &_purecall,
    .pfnOnMouseMove = &_purecall,
    .pfnOnChar = &_purecall,
    .pfnOnArrowUp = &_purecall,
    .pfnOnArrowDown = &_purecall,
    .pfnOnArrowRight = &_purecall,
    .pfnOnArrowLeft = &_purecall,
    .pfnGetValue = &_purecall,
};
static_assert(sizeof(gVtblUiControlBase) == 48, "gVtblUiControlBase: the item's 48 bytes in the .i64");

//  D2Client 0x6FB6F9B0
//  The list box body's vtable, 16 slots, installed by LISTBOX_ImplConstruct. Slots: 0 vecdeldtor, 1 Draw, 2 wheel, 3 mouse down, 4 mouse up, 5 mouse
//  move, 6 char, 7 arrow up, 8 arrow down, 9/10 EDITBOX_DialogNullCallback (a vertical list ignores left/right), 11 GetSelection, 12
//  UpdateScrollThumb, 13 GetNumEntries, 14 RemoveAll, 15 AddEntry. The next 16 dwords are gVtblListBox, a separate vtable, not slots 16-31.
inline const D2ListBoxImplVtbl gVtblListBoxImpl =
{
    .pfnDeletingDtor = &LISTBOX_ImplVecDelDtor,
    .pfnDraw = &LISTBOX_ImplDraw,
    .pfnOnMouseWheel = &LISTBOX_ImplOnMouseWheel,
    .pfnOnMouseDown = &LISTBOX_ImplOnMouseDown,
    .pfnOnMouseUp = &LISTBOX_ImplOnMouseUp,
    .pfnOnMouseMove = &LISTBOX_ImplOnMouseMove,
    .pfnOnChar = &LISTBOX_ImplOnChar,
    .pfnOnArrowUp = &LISTBOX_ImplOnArrowUp,
    .pfnOnArrowDown = &LISTBOX_ImplOnArrowDown,
    .pfnOnArrowRight = &EDITBOX_DialogNullCallback,
    .pfnOnArrowLeft = &EDITBOX_DialogNullCallback,
    .pfnGetValue = &LISTBOX_ImplGetSelection,
    .pfnUpdateScrollThumb = &LISTBOX_ImplUpdateScrollThumb,
    .pfnGetNumEntries = &LISTBOX_ImplGetNumEntries,
    .pfnRemoveAll = &LISTBOX_ImplRemoveAll,
    .pfnAddEntry = &LISTBOX_ImplAddEntry,
};
static_assert(sizeof(gVtblListBoxImpl) == 64, "gVtblListBoxImpl: the item's 64 bytes in the .i64");

//  D2Client 0x6FB6F9F0
//  The list box proxy's vtable, 16 slots, installed by LISTBOX_Construct and LISTBOX_ProxyDestruct. Slot 0 is its own vector deleting destructor
//  and slot 13 is LISTBOX_GetEntryUserData; every other slot is a forwarder to the body. The proxy's slot order is not the body's: its slots
//  12/14/15 forward to body slots 15/13/14.
inline const D2ListBoxProxyVtbl gVtblListBox =
{
    .pfnDeletingDtor = &LISTBOX_ProxyVecDelDtor,
    .pfnDraw = &LISTBOX_UiCtrlProxyFwdSlot1,
    .pfnOnMouseWheel = &LISTBOX_UiCtrlProxyFwdSlot2,
    .pfnOnMouseDown = &SPELLSEL_UiCtrlProxyFwdSlot3,
    .pfnOnMouseUp = &EDITBOX_UiCtrlProxyFwdSlot4,
    .pfnOnMouseMove = &EDITBOX_UiCtrlProxyFwdSlot5,
    .pfnOnChar = &EDITBOX_UiCtrlProxyFwdSlot6,
    .pfnOnArrowUp = &EDITBOX_UiCtrlProxyFwdSlot7,
    .pfnOnArrowDown = &EDITBOX_UiCtrlProxyFwdSlot8,
    .pfnOnArrowRight = &EDITBOX_UiCtrlProxyFwdSlot9,
    .pfnOnArrowLeft = &EDITBOX_UiCtrlProxyFwdSlot10,
    .pfnGetValue = &EDITBOX_UiCtrlProxyFwdSlot11,
    .pfnAddEntry = (decltype(D2ListBoxProxyVtbl::pfnAddEntry))&EDITBOX_UiCtrlProxyFwdSlot15,
    .pfnGetEntryUserData = &LISTBOX_GetEntryUserData,
    .pfnGetNumEntries = &LISTBOX_UiCtrlProxyFwdSlot13,
    .pfnRemoveAll = (decltype(D2ListBoxProxyVtbl::pfnRemoveAll))&EDITBOX_UiCtrlProxyFwdSlot14,
};
static_assert(sizeof(gVtblListBox) == 64, "gVtblListBox: the item's 64 bytes in the .i64");

//  D2Client 0x6FBB5BC4
//  DATA\GLOBAL\ui\menu\textslid, loaded by LISTBOX_LoadScrollBarImage. Cels used: 8/10 down arrow (normal/held), 9/11 up arrow, 13 track, 14 thumb
//  shaft, 15 thumb tail, 16 thumb cap.
inline D2CellFileStrc *gpImgListBoxScrollBar;
static_assert(sizeof(gpImgListBoxScrollBar) == 4, "gpImgListBoxScrollBar: the item's 4 bytes in the .i64");

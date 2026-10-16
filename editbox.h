//  editbox.h -- the globals of editbox.cpp that are ours: the edit box's two vtables.
//  Each is a C++17 inline variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above a global gives
//  its address in the original.
//  Include it after D2Client.h, which declares every function named here.
#pragma once

//  D2Client 0x6FB6EF30
//  The edit box control's vtable, 19 slots, installed by EDITBOX_UiEditBoxConstruct and EDITBOX_UiEditBoxDestruct. It is the body's vtable of a
//  body/proxy pair; the proxy's is gVtblUiControlProxy, which follows it. Slots 9 and 10 are the arrow handlers.
//  The slots are typed by their functions: the original's `__thiscall` slots are spelled `__fastcall` with nEdxUnused.
inline const D2UiEditBoxImplVtbl gVtblUiEditBoxImpl =
{
    .pfnDeletingDtor = &EDITBOX_UiEditBoxVecDelDtor,
    .pfnDraw = &EDITBOX_UiEditBoxDraw,
    .pfnOnMouseWheel = &EDITBOX_DialogNullCallback,
    .pfnOnMouseDown = &EDITBOX_UiEditBoxOnMouseDown,
    .pfnOnMouseUp = &EDITBOX_UiEditBoxOnMouseUp,
    .pfnOnMouseMove = &EDITBOX_DialogNullCallback,
    .pfnOnChar = &EDITBOX_UiEditBoxOnChar,
    .pfnOnArrowUp = &EDITBOX_DialogNullCallback,
    .pfnOnArrowDown = &EDITBOX_DialogNullCallback,
    .pfnOnArrowRight = &EDITBOX_UiEditBoxOnArrowRight,
    .pfnOnArrowLeft = &EDITBOX_UiEditBoxOnArrowLeft,
    .pfnGetValue = &EDITBOX_UiEditBoxGetIntValue,
    .pfnGetTextUtf8 = &EDITBOX_UiEditBoxGetTextUtf8,
    .pfnSlot13 = &EDITBOX_ReturnZeroCallback,
    .pfnSetTextFromAscii = &EDITBOX_UiEditBoxSetTextFromAscii,
    .pfnSetTextFromInt = &EDITBOX_UiEditBoxSetTextFromInt,
    .pfnSetWidth = &EDITBOX_UiEditBoxSetWidth,
    .pfnSetFlag24 = &EDITBOX_UiCtrlSetFlag24,
    .pfnSetNumericOnly = &EDITBOX_UiEditBoxSetNumericOnly,
};
static_assert(sizeof(gVtblUiEditBoxImpl) == 76, "gVtblUiEditBoxImpl: the item's 76 bytes in the .i64");

//  D2Client 0x6FB6EF80
//  The edit box's proxy vtable, 19 slots, installed by EDITBOX_UiCtrlProxyConstructEditbox and EDITBOX_UiCtrlProxyDestruct. Slot 0 is the vector
//  deleting destructor; slots 1-18 are the forwarders to the wrapped control's slots.
inline const D2UiControlProxyVtbl gVtblUiControlProxy =
{
    .pfnDeletingDtor = &EDITBOX_UiCtrlProxyVecDelDtor,
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
    .pfnSlot13 = &EDITBOX_UiCtrlProxyFwdSlot13Arg,
    .pfnSetTextFromAscii = &EDITBOX_UiCtrlProxyFwdSlot14,
    .pfnSetTextFromInt = &EDITBOX_UiCtrlProxyFwdSlot15,
    .pfnGetTextUtf8 = &EDITBOX_UiCtrlProxyFwdSlot12,
    .pfnSetWidth = &EDITBOX_UiCtrlProxyFwdSlot16,
    .pfnSetFlag24 = &EDITBOX_UiCtrlProxyFwdSlot17,
    .pfnSetNumericOnly = &EDITBOX_UiCtrlProxyFwdSlot18,
};
static_assert(sizeof(gVtblUiControlProxy) == 76, "gVtblUiControlProxy: the item's 76 bytes in the .i64");

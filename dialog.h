//  dialog.h -- the globals of dialog.cpp: the dialog's two message tables, the image-name table, the wheel-handler count, the cel frame counter,
//  the global dialog list, the handlers-registered flag and the nesting depth (0x6FB78DF8..0x6FBB364C).
//  Each is a C++17 inline variable, so every file that includes this header shares one object. The items from 0x6FBB363C on lie past the end
//  of .data's raw data (0x6FB9B000): the loader zero-fills them, and ours are zero-initialised.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB78DF8
//  The dialog's 14 message records, registered by DIALOG_Create and DIALOG_CreateFromImage and dropped by DIALOG_Close and DIALOG_Shutdown
//  (14 x 12 bytes ends at 0x6FB78EA0, where gDialogWheelHandlers begins). Records 0 and 1 are the dialog's cancel key, bound twice (a primary
//  and a secondary binding of the same action); DIALOG_Create and DIALOG_CreateFromImage write the bound cancel keys into them before they
//  register the table, so the VK_ESCAPE compiled in is only the default.
//  Records 2..13, handlers all DIALOG_On*: 2,4 WM_L/RBUTTONUP MouseUp; 3,5 WM_L/RBUTTONDOWN MouseDown; 6-9 VK_UP/DOWN/RIGHT/LEFT Arrow*;
//  10 VK_RETURN Enter; 11 VK_ESCAPE Escape; 12 WM_CHAR Char; 13 WM_MOUSEMOVE MouseMove.
inline D2WinCommandRegisterStrc gDialogMessageHandlers[14] =
{
    { D2_WINPROC_KEYDOWN, VK_ESCAPE, &DIALOG_OnCancelKey },   // [0]
    { D2_WINPROC_KEYDOWN, VK_ESCAPE, &DIALOG_OnCancelKey },   // [1]
    { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &DIALOG_OnMouseUp },   // [2]
    { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &DIALOG_OnMouseDown },   // [3]
    { D2_WINPROC_MESSAGE, WM_RBUTTONUP, &DIALOG_OnMouseUp },   // [4]
    { D2_WINPROC_MESSAGE, WM_RBUTTONDOWN, &DIALOG_OnMouseDown },   // [5]
    { D2_WINPROC_KEYDOWN, VK_UP, &DIALOG_OnArrowUp },   // [6]
    { D2_WINPROC_KEYDOWN, VK_DOWN, &DIALOG_OnArrowDown },   // [7]
    { D2_WINPROC_KEYDOWN, VK_RIGHT, &DIALOG_OnArrowRight },   // [8]
    { D2_WINPROC_KEYDOWN, VK_LEFT, &DIALOG_OnArrowLeft },   // [9]
    { D2_WINPROC_KEYDOWN, VK_RETURN, &DIALOG_OnEnter },   // [10]
    { D2_WINPROC_KEYDOWN, VK_ESCAPE, &DIALOG_OnEscape },   // [11]
    { D2_WINPROC_MESSAGE, WM_CHAR, &DIALOG_OnChar },   // [12]
    { D2_WINPROC_MESSAGE, WM_MOUSEMOVE, &DIALOG_OnMouseMove },   // [13]
};
static_assert(sizeof(gDialogMessageHandlers) == 168, "gDialogMessageHandlers: the item's 168 bytes in the .i64");

//  D2Client 0x6FB78EA0
//  A second, one-record table: WM_MOUSEWHEEL, handler DIALOG_OnMouseWheel. DIALOG_Create registers it only when GAME_HasNativeMouseWheel()
//  holds, and DIALOG_Shutdown drops it under the same condition, so a dialog gets wheel scrolling only on a machine whose OS delivers
//  WM_MOUSEWHEEL.
inline D2WinCommandRegisterStrc gDialogWheelHandlers = { D2_WINPROC_MESSAGE, WM_MOUSEWHEEL, &DIALOG_OnMouseWheel };
static_assert(sizeof(gDialogWheelHandlers) == 12, "gDialogWheelHandlers: the item's 12 bytes in the .i64");

//  D2Client 0x6FB78EC0
//  Image names for DIALOG_CreateFromImage, formatted into DATA\GLOBAL\ui\menu\%s. One entry, dialogbackground.
inline const char *gDialogImageNames[1] = { "dialogbackground" };
static_assert(sizeof(gDialogImageNames) == 4, "gDialogImageNames: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB363C
//  How many dialogs have gDialogWheelHandlers registered on the window: incremented by DIALOG_Create and DIALOG_CreateFromImage right after the
//  registration, decremented by DIALOG_Shutdown right after the matching unregister.
//  The count does not follow the registration: DIALOG_Create and DIALOG_CreateFromImage skip the registration when there is no native wheel
//  (je 0x6FAD6D68, 0x6FAD6B7D) and increment all the same (0x6FAD6D8A, 0x6FAD6B9F); DIALOG_Shutdown skips the unregister the same way
//  (je 0x6FAD6893) and decrements (0x6FAD68A8). Neither increment is guarded: the gnDialogNestDepth compare comes after the store (je 0x6FAD6D90,
//  0x6FAD6BA5) and only chooses whether an assert runs.
inline int32_t gnDialogWheelHandlersRefCount;
static_assert(sizeof(gnDialogWheelHandlersRefCount) == 4, "gnDialogWheelHandlersRefCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB3640
//  A free-running counter whose remainder modulo 7 is the cel frame passed to TEXTURE_CelDraw: mov eax, counter; cdq; mov ecx, 7; idiv ecx at
//  0x6FAD6FF0, then edx goes into the draw call's frame field, with the inc at 0x6FAD7035. Both references are in DIALOG_Draw. The draw sits
//  behind [esi+5Ch] == 2, so only one kind of dialog reaches it.
inline int32_t gnDialogCelFrameCounter;
static_assert(sizeof(gnDialogCelFrameCounter) == 4, "gnDialogCelFrameCounter: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB3644
//  Head of the global dialog list. DIALOG_Create pushes each new dialog on the front through the pointer at +0xB0C; DIALOG_Shutdown walks it
//  freeing everything.
inline D2ClientDialogStrc *gpDialogList;
static_assert(sizeof(gpDialogList) == 4, "gpDialogList: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB3648
//  Set by DIALOG_Create (at 0x6FAD6CEB, before the two registration calls at 0x6FAD6D5C and 0x6FAD6D77); DIALOG_Shutdown checks it before
//  unregistering, so a shutdown with no dialog ever created is a no-op.
inline BOOL gbDialogHandlersRegistered;
static_assert(sizeof(gbDialogHandlersRegistered) == 4, "gbDialogHandlersRegistered: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB364C
//  Incremented on entry to DIALOG_Create and asserted to be exactly 1 before it returns (dialog.cpp:1040), so two dialogs cannot be under
//  construction at the same time.
inline int32_t gnDialogNestDepth;
static_assert(sizeof(gnDialogNestDepth) == 4, "gnDialogNestDepth: the item's 4 bytes in the .i64");

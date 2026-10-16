//  Cursor.h -- the globals of Cursor.cpp: the cursor's message table, gCursorDescs and gnCursors (0x6FB96B30..0x6FB96C1F), the cursor state and
//  the mouse position (0x6FBC1AA4..0x6FBC1B07), and the packet counters kept by SCMD_RouteServerPackets and the MSG_Send* functions.
//  Each is a C++17 inline variable, so every file that includes this header shares one object. The items from 0x6FBC1AE4 on lie past the end
//  of .data's raw data (0x6FB9B000): the loader zero-fills them, and ours are zero-initialised.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB96B30
//  The cursor's 3 message records, registered by CURSOR_Init and dropped by CURSOR_UnloadCursors: WM_MOUSEMOVE CURSOR_OnMouseMove,
//  WM_NCMOUSEMOVE CURSOR_OnNonClientMouseMove, WM_LBUTTONUP CURSOR_OnPanelMouseUp.
inline D2WinCommandRegisterStrc gCursorMessageHandlers[3] =
{
    { D2_WINPROC_MESSAGE, WM_MOUSEMOVE,   &CURSOR_OnMouseMove },
    { D2_WINPROC_MESSAGE, WM_NCMOUSEMOVE, &CURSOR_OnNonClientMouseMove },
    { D2_WINPROC_MESSAGE, WM_LBUTTONUP,   &CURSOR_OnPanelMouseUp },
};
static_assert(sizeof(gCursorMessageHandlers) == 36,
              "gCursorMessageHandlers: three 12-byte records, CURSOR_Init and CURSOR_UnloadCursors pass 3 (`push 3`, 0x6FB575FC, 0x6FB576B1)");
static_assert(WM_MOUSEMOVE == 0x200 && WM_NCMOUSEMOVE == 0xA0 && WM_LBUTTONUP == 0x202,
              "the image's message ids at 0x6FB96B34, 0x6FB96B40, 0x6FB96B4C");

//  D2Client 0x6FB96B58
//  CursorDescStrc[7], the seven cursor definitions, 196 bytes to 0x6FB96C1C. CURSOR_DrawCursorDefault jumps through pDrawFunction (offset 0x14)
//  and CURSOR_Init walks szName (offset 0x18). Rows 0..5 draw with CURSOR_DrawCurrent, row 6 with CURSOR_DrawCursorBuySell; both are
//  __fastcall, as the struct's pDrawFunction is. In .data's raw bytes, not .rdata, so not const.
inline CursorDescStrc gCursorDescs[7] =
{
    //  bAnimated  bLoops  nFrameCount  nFrameAdvance  bCanPress  pDrawFunction  szName
    { 0, 0, 1, 0x00, 1, &CURSOR_DrawCurrent, "Gaunt" },               //  [0]
    { 1, 0, 8, 0x00, 1, &CURSOR_DrawCurrent, "grasp" },               //  [1]
    { 1, 0, 8, 0x40, 1, &CURSOR_DrawCurrent, "ohand" },               //  [2]
    { 1, 1, 8, 0x20, 1, &CURSOR_DrawCurrent, "orotate" },             //  [3]
    { 1, 1, 8, 0x40, 1, &CURSOR_DrawCurrent, "ppress" },              //  [4]
    { 1, 1, 8, 0x40, 1, &CURSOR_DrawCurrent, "protate" },             //  [5]
    { 0, 0, 1, 0x00, 0, &CURSOR_DrawCursorBuySell, "buysell" },       //  [6]
};
static_assert(sizeof(gCursorDescs) == 196 && sizeof(gCursorDescs) / sizeof(gCursorDescs[0]) == NUM_CURSORS,
              "gCursorDescs: seven 28-byte records, 0x6FB96B58..0x6FB96C1B, gnCursors at +196");

//  D2Client 0x6FB96C1C
//  The cursor count, NUM_CURSORS (7): the asserts "NUM_CURSORS == nCursorTableSize" in CURSOR_GetCursorLoops and CURSOR_GetCursorFrameDuration
//  compare it. In .data's raw bytes and read, never written, but not const: const would let the compiler fold the asserts the original makes
//  from memory.
inline uint32_t gnCursors = NUM_CURSORS;
static_assert(sizeof(gnCursors) == 4, "gnCursors: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1AA4
//  The unit currently on the cursor; CURSOR_DrawCursorDefault draws it when its dwUnitType is 4 (UNIT_ITEM). Set by CURSOR_SetHeldUnit and
//  CURSOR_SetBuysellCursor, and with NULL by CURSOR_SetCursorBuySell; read by CURSOR_GetHeldUnit.
inline D2UnitStrc * gpCursorHeldUnit = NULL;
static_assert(sizeof(gpCursorHeldUnit) == 4, "gpCursorHeldUnit: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1AA8
//  The seven loaded cursor images, parallel to gCursorDescs' szName column and filled by CURSOR_Init in the same loop. Read by
//  CURSOR_UnloadCursors, CURSOR_DrawCurrent and CURSOR_DrawCursorBuySell. It runs to 0x6FBC1AC4, where gnCursorType follows.
inline D2CellFileStrc * gpCursorImages[7] = {};
static_assert(sizeof(gpCursorImages) == 28 && sizeof(gpCursorImages) / sizeof(gpCursorImages[0]) == NUM_CURSORS,
              "gpCursorImages: seven handles, 0x6FBC1AA8..0x6FBC1AC3, gnCursorType at +28");

//  D2Client 0x6FBC1AC4
//  Which cursor is showing: the index into gCursorDescs. Zero at rest (CURSOR_Gauntlet). CURSOR_UpdateCursorPosInGame forces it to
//  CURSOR_PPress (4) when it moves the cursor into CURSORSTATE_ButtonPressed.
inline D2C_CursorTypes gnCursorType = CURSOR_Gauntlet;
static_assert(sizeof(gnCursorType) == 4, "gnCursorType: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1AC8
//  Current animation frame of the cursor: 8.8 fixed point for every type but CURSOR_Buysell, where it is a raw frame index.
//  CURSOR_SetBuysellCursor and CURSOR_SetCursorBuySell store a raw frame beside CURSOR_Buysell, and CURSOR_OnMouseMove stores
//  CURSOR_GetCursorFrameDuration(CURSOR_OHand) - 256 beside CURSOR_OHand.
inline int32_t gnCursorFrame = 0;
static_assert(sizeof(gnCursorFrame) == 4, "gnCursorFrame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1ACC
//  Write-only: the one store is in CURSOR_Init (mov gwCursorWriteOnly, ax at 0x6FB57673) and nothing reads it.
inline uint16_t gwCursorWriteOnly = 0;
static_assert(sizeof(gwCursorWriteOnly) == 2, "gwCursorWriteOnly: the item's 2 bytes in the .i64");

//  D2Client 0x6FBC1AD0
//  GetTickCount() at the last cursor move. Stamped by CURSOR_OnMouseMove, CURSOR_UpdateCursorPosInGame, CURSOR_Init and CURSOR_OnPanelMouseUp;
//  the one read compares it unsigned (jnb at 0x6FB578FA).
inline uint32_t gnCursorLastPositionUpdateTickCount = 0;
static_assert(sizeof(gnCursorLastPositionUpdateTickCount) == 4, "gnCursorLastPositionUpdateTickCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1AD4
//  The cursor's animation state, a CURSORSTATE_ value written by the CURSOR_ functions. At rest it is 0, no member of the enum: the state
//  before CURSOR_Init.
inline D2C_CursorState gnCursorState = (D2C_CursorState)0;
static_assert(sizeof(gnCursorState) == 4, "gnCursorState: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1AE0
//  CURSOR_Init's one argument, stored at 0x6FB575F3; read by the three draws -- CURSOR_DrawCurrent, CURSOR_DrawCursorBuySell and
//  CURSOR_DrawCursorDefault.
inline int32_t gnCursorClickOffsetX = 0;
static_assert(sizeof(gnCursorClickOffsetX) == 4, "gnCursorClickOffsetX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1B04
//  The cursors-loaded flag: CURSOR_Init sets it to 1 after the seven loads, CURSOR_UnloadCursors clears it before freeing anything, and
//  CURSOR_DrawCursorDefault draws nothing while it is 0. It is state, not a cursor image handle.
inline BOOL gbCursorsLoaded = FALSE;
static_assert(sizeof(gbCursorsLoaded) == 4, "gbCursorsLoaded: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1AE4
//  Cursor X in screen pixels. CURSOR_Init writes gnMouseX and gnMouseY together as gnScreenWidth / 2 and gnScreenHeight / 2.
inline int32_t gnMouseX;
static_assert(sizeof(gnMouseX) == 4, "gnMouseX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1AE8
inline int32_t gnMouseY;
static_assert(sizeof(gnMouseY) == 4, "gnMouseY: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1AF0
//  Running total of the packet bytes SCMD_RouteServerPackets has consumed: each packet's 16-bit length is added to it. Written and never read.
inline int32_t gnRoutedPacketBytes;
static_assert(sizeof(gnRoutedPacketBytes) == 4, "gnRoutedPacketBytes: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1AF4
//  The bytes of the packets MSG_SendPacketToServer and the six MSG_Send* functions add up.
inline uint32_t gPacketAllSize;
static_assert(sizeof(gPacketAllSize) == 4, "gPacketAllSize: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1AF8
//  How many times SCMD_RouteServerPackets has run to its end: the increment is the last instruction before the epilogue, so it counts calls,
//  not packets, and the two early exits (je 6FAB53B7h at 0x6FAB50C0, and the one at 0x6FAB50FF) jump past it. Written and never read.
inline int32_t gnRoutePacketCalls;
static_assert(sizeof(gnRoutePacketCalls) == 4, "gnRoutePacketCalls: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1AFC
//  The packets MSG_SendPacketToServer and the six MSG_Send* functions add up.
inline uint32_t gPacketCounter;
static_assert(sizeof(gPacketCounter) == 4, "gPacketCounter: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC1B00
//  Incremented once per call to SCMD_RouteServerPackets whose incoming data is larger than 0x1F4 (500) bytes (cmp edi, 1F4h / jle): it
//  counts unusually large arrivals, not packets. Written and never read.
inline int32_t gnOversizePacketBatches;
static_assert(sizeof(gnOversizePacketBatches) == 4, "gnOversizePacketBatches: the item's 4 bytes in the .i64");

//  Fault.h -- the globals of Fault.cpp: the message-source list, its lock and the lock's flag, the assert handler slot and the language
//  init's debug-break flag (0x6FBC3430..0x6FBC345F). Each is a C++17 inline variable, so every file that includes this header shares one
//  object. They lie past the end of .data's raw data (0x6FB9B000): the loader zero-fills them, and ours are zero-initialised.
//  gMessageSourceList is built and torn down by Fault.cpp's sgMessageSourceListLifetime; gMsgSrcCriticalSection is initialised by FAULT_Init.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FBC3430
//  The critical section guarding the message-source registry: FAULT_Init initialises it once, FAULT_RegisterMessageSource takes it, and
//  FAULT_AtExit enters, leaves and deletes it.
inline struct _RTL_CRITICAL_SECTION gMsgSrcCriticalSection;
static_assert(sizeof(gMsgSrcCriticalSection) == 24, "gMsgSrcCriticalSection: the item's 24 bytes in the .i64");

//  D2Client 0x6FBC3448
//  The head of Storm's TMessageSource list; FAULT_RegisterMessageSource adds to it and FAULT_AtExit frees it.
inline TSExplicitList gMessageSourceList;
static_assert(sizeof(gMessageSourceList) == 12, "gMessageSourceList: the item's 12 bytes in the .i64");

//  D2Client 0x6FBC3454
//  Set once FAULT_Init has initialised the critical section and registered its atexit; read by FAULT_Init and by
//  FAULT_RegisterMessageSource, which calls it when the flag is clear. Cleared by FAULT_AtExit.
inline int32_t gbMsgSourceLockReady;
static_assert(sizeof(gbMsgSourceLockReady) == 4, "gbMsgSourceLockReady: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC3458
//  The assert handler slot, installed or chained by FAULT_SetAssertHandler (GAME_InitGame installs GAME_TeardownGraphics; RECORD_InstallSaveOnCleanup
//  installs RECORD_CleanupHandler in front of what was there). Its only two references are that function's read and write: written, never called.
inline void *gpfnFaultAssertHandler;
static_assert(sizeof(gpfnFaultAssertHandler) == 4, "gpfnFaultAssertHandler: the item's 4 bytes in the .i64");

//  D2Client 0x6FBC345C
//  Written by LNG_Initialize around its debug break; nothing reads it.
inline int32_t gbLngInitInDebugBreak;
static_assert(sizeof(gbLngInitInDebugBreak) == 4, "gbLngInitInDebugBreak: the item's 4 bytes in the .i64");

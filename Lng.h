//  Lng.h -- the globals of Lng.cpp that are ours: the three string pointers (library name, init-failure text, error text), the registry key text
//  szKeyName, the accelerator table, the language DLL path buffer and the module handle. Each is a C++17 `inline` variable, one object for every
//  file that includes this header. The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB6E0B0
//  Points at "D2.LNG", the library LNG_Initialize loads when neither the command line nor the registry names one. One of four LPCSTR slots packed
//  together at 0x6FB6E0AC..0x6FB6E0B8; gpszD2ClientCaption heads the group.
inline const LPCSTR gpszLngLibName = "D2.LNG";
static_assert(sizeof(gpszLngLibName) == 4, "gpszLngLibName: the item's 4 bytes in the .i64");

//  D2Client 0x6FB6E0B4
//  Points at "Unable to start LNG manager.", the message LNG_Initialize shows when the load fails.
inline const LPCSTR gpszLngInitFailedText = "Unable to start LNG manager.";
static_assert(sizeof(gpszLngInitFailedText) == 4, "gpszLngInitFailedText: the item's 4 bytes in the .i64");

//  D2Client 0x6FB6E0B8
//  Points at "Stupid Lng.cpp error.", the text LNG_MessageSource copies in place of Storm's string.
inline const LPCSTR gpszLngErrorText = "Stupid Lng.cpp error.";
static_assert(sizeof(gpszLngErrorText) == 4, "gpszLngErrorText: the item's 4 bytes in the .i64");

//  D2Client 0x6FB74150
//  The registry key every Storm SReg* call of the module names: "Diablo II", nine characters and the NUL. A pooled literal rather than a declared
//  variable: one copy of the text serves every file that pushes it.
inline char szKeyName[10] = "Diablo II";
static_assert(sizeof(szKeyName) == 10, "szKeyName: the item's 10 bytes in the .i64");

//  D2Client 0x6FB9A710
//  The window's accelerator table, loaded from the language DLL by LNG_Initialize, cleared by LNG_Shutdown and read by WNDPROC_WindowRunMessagePump
//  (`mov eax, ghAccelTable`, 0x6FAA2614) for the TranslateAccelerator call in the message loop, so the keyboard accelerators are localised
//  with the strings.
inline HACCEL ghAccelTable;
static_assert(sizeof(ghAccelTable) == 4, "ghAccelTable: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9A718
//  The path buffer of the language DLL, MAX_PATH bytes: written by LNG_RegisterCmdLineArguments (the `-lng` switch) and by STORM_SRegLoadString
//  (from the registry), and passed to LoadLibraryExA by LNG_Initialize, which also tests its first byte for zero to see whether the name is
//  set.
inline char szLangDllPath[260];
static_assert(sizeof(szLangDllPath) == 260, "szLangDllPath: the item's 260 bytes in the .i64");

//  D2Client 0x6FB9A81C
//  The loaded language DLL. LNG_Initialize tries three sources in order: the path in szLangDllPath, the registry value "LNG File" under the key
//  "Diablo II" (szKeyName), and *gpszLngLibName, "D2.LNG"; it then hands the handle to LoadAcceleratorsA and SErrRegisterMessageSource(4095, ...).
inline HMODULE ghLangModule;
static_assert(sizeof(ghLangModule) == 4, "ghLangModule: the item's 4 bytes in the .i64");

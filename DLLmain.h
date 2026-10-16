#pragma once

/****************************************************************************
*                                                                           *
*   DLLmain.h                                                               *
*                                                                           *
*   Licensed under the Apache License, Version 2.0 (the "License");         *
*   you may not use this file except in compliance with the License.        *
*   You may obtain a copy of the License at                                 *
*                                                                           *
*   http://www.apache.org/licenses/LICENSE-2.0                              *
*                                                                           *
*   Unless required by applicable law or agreed to in writing, software     *
*   distributed under the License is distributed on an "AS IS" BASIS,       *
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
*   See the License for the specific language governing permissions and     *
*   limitations under the License.                                          *
*                                                                           *
*---------------------------------------------------------------------------*
*                                                                           *
*   https://github.com/olivier-verville/D2Template                          *
*                                                                           *
*   D2Template core file, do not modify unless you know what you're doing   *
*                                                                           *
*****************************************************************************/

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _CRT_SECURE_NO_DEPRECATE
#define _WIN32_WINNT 0x600

#pragma warning(disable : 4200)

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <shellapi.h>
#include <algorithm>
#include <utility>
#include <vector>
#include <functional>
#include <memory>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <format>

HMODULE GetOrLoadModule(const char* module_path);

//static const DWORD DLLBASE_BNCLIENT = (DWORD)GetOrLoadModule("Bnclient.dll");
//static const DWORD DLLBASE_D2CLIENT = (DWORD)GetOrLoadModule("_Original_D2Client.dll");
//static const DWORD DLLBASE_D2CMP = (DWORD)GetOrLoadModule("D2CMP.dll");
//static const DWORD DLLBASE_D2COMMON = (DWORD)GetOrLoadModule("D2Common.dll");
//static const DWORD DLLBASE_D2DDRAW = (DWORD)GetOrLoadModule("D2DDraw.dll");
//static const DWORD DLLBASE_D2DIRECT3D = (DWORD)GetOrLoadModule("D2Direct3D.dll");
//static const DWORD DLLBASE_D2GAME = (DWORD)GetOrLoadModule("D2Game.dll");
//static const DWORD DLLBASE_D2GDI = (DWORD)GetOrLoadModule("D2Gdi.dll");
//static const DWORD DLLBASE_D2GFX = (DWORD)GetOrLoadModule("D2Gfx.dll");
//static const DWORD DLLBASE_D2GLIDE = (DWORD)GetOrLoadModule("D2Glide.dll");
//static const DWORD DLLBASE_D2LANG = (DWORD)GetOrLoadModule("D2Lang.dll");
//static const DWORD DLLBASE_D2LAUNCH = (DWORD)GetOrLoadModule("D2Launch.dll");
//static const DWORD DLLBASE_D2MCPCLIENT = (DWORD)GetOrLoadModule("D2MCPClient.dll");
//static const DWORD DLLBASE_D2MULTI = (DWORD)GetOrLoadModule("D2Multi.dll");
//static const DWORD DLLBASE_D2NET = (DWORD)GetOrLoadModule("D2Net.dll");
//static const DWORD DLLBASE_D2SOUND = (DWORD)GetOrLoadModule("D2Sound.dll");
//static const DWORD DLLBASE_D2WIN = (DWORD)GetOrLoadModule("D2Win.dll");
//static const DWORD DLLBASE_FOG = (DWORD)GetOrLoadModule("Fog.dll");
//static const DWORD DLLBASE_STORM = (DWORD)GetOrLoadModule("Storm.dll");
//static const DWORD DLLBASE_IJL11 = (DWORD)GetOrLoadModule("ijl11.dll");
//static const DWORD DLLBASE_BINKW32 = (DWORD)GetOrLoadModule("binkw32.dll");
//static const DWORD DLLBASE_SMACKW32 = (DWORD)GetOrLoadModule("SmackW32.dll");

#include "D2Constants.h"
#include "D2Structs.h"
#include "D2Ptrs.h"
#include "D2Vars.h"

struct DLLBaseStrc
{
    const char* szName;
    uint32_t dwAddress;
};

struct DLLPatchStrc
{
    int32_t nDLL;
    uint32_t dwAddress;
    uint32_t dwData;
    BOOL bRelative;
    size_t nPatchSize;
};

enum D2TEMPLATE_DLL_FILES
{
    D2DLL_BINKW32,
    D2DLL_BNCLIENT,
    D2DLL_D2CLIENT,
    D2DLL_D2CMP,
    D2DLL_D2COMMON,
    D2DLL_D2DDRAW,
    D2DLL_D2DIRECT3D,
    D2DLL_D2GAME,
    D2DLL_D2GDI,
    D2DLL_D2GFX,
    D2DLL_D2GLIDE,
    D2DLL_D2LANG,
    D2DLL_D2LAUNCH,
    D2DLL_D2MCPCLIENT,
    D2DLL_D2MULTI,
    D2DLL_D2NET,
    D2DLL_D2SOUND,
    D2DLL_D2WIN,
    D2DLL_FOG,
    D2DLL_IJL11,
    D2DLL_SMACKW32,
    D2DLL_STORM,
    D2DLL_INVALID
};

static DLLBaseStrc gptDllFiles[] =
{
	{"Binkw32.dll",         NULL},
	{"BnClient.dll",        NULL},
	{"D2Client.dll",        NULL},
	{"D2CMP.dll",           NULL},
	{"D2Common.dll",        NULL},
	{"D2DDraw.dll",         NULL},
	{"D2Direct3D.dll",      NULL},
	{"D2Game.dll",          NULL},
	{"D2Gdi.dll",           NULL},
	{"D2Gfx.dll",           NULL},
	{"D2Glide.dll",         NULL},
	{"D2Lang.dll",          NULL},
	{"D2Launch.dll",        NULL},
	{"D2MCPClient.dll",     NULL},
	{"D2Multi.dll",         NULL},
	{"D2Net.dll",           NULL},
	{"D2Sound.dll",         NULL},
	{"D2Win.dll",           NULL},
	{"Fog.dll",             NULL},
	{"Ijl11.dll",           NULL},
	{"SmackW32.dll",        NULL},
	{"Storm.dll",           NULL},
};

#define FCT_ASM(N) __declspec(naked) void N() {__asm{
#define ARRAY_SIZE(Array) (sizeof((Array)) / sizeof((Array)[0]))


HMODULE GetOrLoadModule(const char* module_path);
uint32_t GetDllOffset(const char* ModuleName, uint32_t BaseAddress, int32_t Offset);
// FIX (bug #6): GetModuleExt returned a `const char*` to a local stack buffer (a dangling pointer). It returns std::string by value now, and its
// one call site, GetDllOffset, calls .c_str() on it.
std::string GetModuleExt(const char* ModuleName);

//  DLLMAIN_Entrypoint is declared here for the interface table below. The globals of DLLmain.cpp that are ours are gpQueryInterfaceVtbl (the
//  one-slot table QueryInterface hands out) and ghD2ClientModule (the module handle). Each is a C++17 `inline` variable, one object for every file
//  that includes this header; this header is also the D2Template core that every file includes first. The comment line "D2Client 0x6F..." above
//  a global gives its address in the original.
int32_t __fastcall DLLMAIN_Entrypoint(D2ConfigStrc* pConfig);

//  D2Client 0x6FB74108
//  The interface D2Client gives Game.exe: one __fastcall slot, DLLMAIN_Entrypoint (0x6FAA11D0), which takes the launch configuration.
//  QueryInterface (export ordinal 10004) returns the address of this table. It is not a COM vtable, despite the name.
inline D2ClientInterface gpQueryInterfaceVtbl = { .pEntrypoint = &DLLMAIN_Entrypoint };
static_assert(sizeof(gpQueryInterfaceVtbl) == 4, "gpQueryInterfaceVtbl: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9A70C
//  D2Client.dll's own instance handle: written by DllMain (0x6FAA11A4) and read by DLLMAIN_Entrypoint, which hands it to WINMAIN_D2ClientEntrypoint.
inline HMODULE ghD2ClientModule;
static_assert(sizeof(ghD2ClientModule) == 4, "ghD2ClientModule: the item's 4 bytes in the .i64");

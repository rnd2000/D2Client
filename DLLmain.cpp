#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"

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

HMODULE GetOrLoadModule(const char* module_path) {
    HMODULE hModule = GetModuleHandle(module_path);
    if (!hModule)
    {
        hModule = LoadLibrary(module_path);
    }

    return hModule;
}

//  DLLmain.cpp -- the module entry surface of D2Client.dll 1.10: everything the rest of the game can reach before WINMAIN_D2ClientEntrypoint.
//  Four functions: DllEntryPoint (0x6FB61C1D, the PE entry: CRT init, then DllMain), DllMain (0x6FAA11A0, 17 bytes: remembers hinstDLL and returns
//  TRUE), QueryInterface (0x6FAA11C0, 6 bytes: hands back a one-slot table; export ordinal 10004, the only export this module names) and
//  DLLMAIN_Entrypoint (0x6FAA11D0, 13 bytes: the one slot in that table).
//  What this file leans on is the config record the launcher hands in; its layout is asserted below.
static_assert(offsetof(D2ConfigStrc, szGameName) == 0x1A
                  && offsetof(D2ConfigStrc, szServerIP) == 0x32,
              "the two 24-byte strings DLLMAIN_Entrypoint forwards");
static_assert(GAMETYPE_OBNET_HOST == 6,
              "`pConfig->dwGameType == 6` -- the constant that falsified a "
              "name here: the test had been read as an expansion check and "
              "is the open-Battle.net host arm");


//1.10f: D2Client.0x6FAA11C0 (export #10004)
//  Not a COM QueryInterface despite the name: no arguments, no interface matching, six bytes of `mov eax, offset gpQueryInterfaceVtbl; retn`.
//  The table is ours, in DLLmain.h.
extern "C" D2ClientInterface* __cdecl QueryInterface()
{
    return &gpQueryInterfaceVtbl;
}

//1.10f: D2Client.0x6FAA11D0
//  __fastcall, and the parameter is the point: pConfig arrives in ecx, which is read and never written, and is forwarded with ghD2ClientModule to
//  WINMAIN_D2ClientEntrypoint (`mov eax, ghD2ClientModule / push ecx / push eax / call WINMAIN_D2ClientEntrypoint / retn`). The return is that call's
//  eax, handed back untouched (0x6FAA11D7..0x6FAA11DC).
int32_t __fastcall DLLMAIN_Entrypoint(D2ConfigStrc* pConfig)
{
    return WINMAIN_D2ClientEntrypoint(ghD2ClientModule, pConfig);
}

//1.10f: D2Client.0x6FAA11A0
//  Seventeen bytes and no reason switch: `mov eax, [esp+hinstDLL] / mov ghD2ClientModule, eax / mov eax, 1 / retn 0Ch`. No thread-attach handling
//  and no teardown; everything this module needs at load time happens later, through the interface above.
BOOL __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    ghD2ClientModule = hinstDLL;
    return TRUE;
}

//1.10f: D2Client.0x6FB61C1D -- the PE entry point, RVA 0xC1C1D, confirmed
//  In the binary this is the VC6 CRT's own DLL startup: it calls a hook through a pointer at 0x6FBC47F0, then _CRT_INIT, then DllMain, then
//  _CRT_INIT again on detach. That is what today's `_DllMainCRTStartup` does, and `_CRT_INIT` is a CRT-internal symbol no modern UCRT exports, so
//  the original's NAME is kept as the entry point and the CRT's own startup does the work. /ENTRY:DllEntryPoint in the project file is the
//  other half.
extern "C" BOOL WINAPI _DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);

extern "C" BOOL __stdcall DllEntryPoint(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    return _DllMainCRTStartup(hinstDLL, fdwReason, lpReserved);
}

uint32_t GetDllOffset(const char* ModuleName, uint32_t BaseAddress, int32_t Offset)
{
	if(!BaseAddress)
		BaseAddress = (uint32_t)LoadLibraryA(GetModuleExt(ModuleName).c_str());

	if(Offset < 0)
		return (uint32_t)GetProcAddress((HMODULE)BaseAddress,(LPCSTR)(-Offset));

	return BaseAddress + Offset;
}

// FIX (bug #6): the template's GetModuleExt returned the address of a local stack array (a dangling pointer). It returns std::string by value now,
// and the length of ModuleName is checked before it is copied into the fixed 32-byte buffer.
std::string GetModuleExt(const char* ModuleName)
{
	const char DLLExt[] = ".dll";
	char DLLName[32] = { 0 };

	strncpy_s(DLLName, sizeof(DLLName), ModuleName, _TRUNCATE);
	size_t nUsed = strnlen(DLLName, sizeof(DLLName));
	size_t nExtLen = sizeof(DLLExt) - 1;
	if (nUsed + nExtLen < sizeof(DLLName)) {
		strcat_s(DLLName, sizeof(DLLName), DLLExt);
	}

	return std::string(DLLName);
}
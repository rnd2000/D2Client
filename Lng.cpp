#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Fault.h"
#include "Lng.h"

//  Lng.cpp -- the language module of D2Client.dll 1.10: a resource DLL (D2.LNG unless the command line or the registry names another), loaded as a
//  data file, its accelerator table taken, and the module registered as a source of error strings with Storm and with Fault.cpp.
//  The original's source file is D2Client\CORE\Lng.cpp (the assert of LNG_MessageSource, line 129, names it). Address range 0x6FAA11E0..0x6FAA1391,
//  four functions. The globals these functions touch are in Lng.h, except gbLngInitInDebugBreak (0x6FBC345C), which is in Fault.h. The registry key
//  and value names and the command-line switch are literals.

//  The path literal LNG_MessageSource's assert passes.
static const char szLngCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\CORE\\Lng.cpp";

//  The literal flag of all three LoadLibraryExA calls.
static_assert(LOAD_LIBRARY_AS_DATAFILE == 2, "LNG_Initialize: `push 2` before each LoadLibraryExA");


//1.10f: D2Client.0x6FAA11E0
//  Registers the `-lng <file>` switch with Storm's command line: a string argument (flag 0x20000) written straight into szLangDllPath, MAX_PATH
//  bytes, with no callback. Always returns 1.
int32_t __cdecl LNG_RegisterCmdLineArguments(void)
{
    STORM_SCmdRegisterArgument(0x20000, -1, "lng", szLangDllPath, MAX_PATH, 0, 0, 0);
    return 1;
}

//1.10f: D2Client.0x6FAA1210
//  Loads the language module, three tries in order: the path the `-lng` switch wrote (which is then saved to the registry, "Diablo II" / "LNG
//  File"); else the path the registry holds; else *gpszLngLibName, "D2.LNG". Each is LoadLibraryExA as a data file. With a module: its accelerator
//  table (resource 100) into ghAccelTable, the module registered with Storm as the source of message ids from 0xFFF up, LNG_MessageSource
//  registered with Fault.cpp, and 1. Without one: FAULT_PromptErrorOrCancel with *gpszLngInitFailedText ("Unable to start LNG manager.") and 0 --
//  and if Cancel was pressed, `int 3` between two stores to gbLngInitInDebugBreak, 1 before (0x6FAA12AB) and 0 after (0x6FAA12B6), so the
//  flag is 1 only while a debugger sits on the break. Nothing in the module reads it.
//  SRegLoadString's flags are `push eax` (0x6FAA1259), eax being the module handle the `jnz` just found NULL: 0.
int32_t __cdecl LNG_Initialize(void)
{
    if (szLangDllPath[0])
    {
        ghLangModule = LoadLibraryExA(szLangDllPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
        STORM_SRegSaveString(szKeyName, "LNG File", 0, szLangDllPath);
    }

    if (!ghLangModule && STORM_SRegLoadString(szKeyName, "LNG File", 0, szLangDllPath, MAX_PATH))
    {
        ghLangModule = LoadLibraryExA(szLangDllPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
    }

    if (!ghLangModule)
        ghLangModule = LoadLibraryExA(gpszLngLibName, NULL, LOAD_LIBRARY_AS_DATAFILE);

    if (!ghLangModule)
    {
        if (FAULT_PromptErrorOrCancel(gpszLngInitFailedText))
        {
            gbLngInitInDebugBreak = 1;
            __debugbreak();
            gbLngInitInDebugBreak = 0;
        }
        return 0;
    }

    ghAccelTable = LoadAcceleratorsA(ghLangModule, MAKEINTRESOURCEA(100));
    STORM_SErrRegisterMessageSource(0xFFF, ghLangModule, 0);
    FAULT_RegisterMessageSource(LNG_MessageSource);
    return 1;
}

//1.10f: D2Client.0x6FAA1300
//  Frees the language module when there is one, and clears it and the accelerator table (0x6FAA1313, 0x6FAA1319). Returns void: nothing reads eax
//  after any of its calls.
void __cdecl LNG_Shutdown(void)
{
    if (ghLangModule)
        FreeLibrary(ghLangModule);
    ghLangModule = NULL;
    ghAccelTable = NULL;
}

//1.10f: D2Client.0x6FAA1330
//  The message source LNG_Initialize registers with Fault.cpp: for an error id, Storm's string (SErrGetErrorStr, which reads the module
//  LNG_Initialize registered) and 1, or *gpszLngErrorText ("Stupid Lng.cpp error.") copied in its place and 0. It asserts the module first
//  (`sghLNGInstance`, line 129) and exits. `retn 0Ch`, three stack arguments -- Fault.cpp's MessageSource contract; the length is `int` as
//  lstrcpynA takes it.
//  Nothing in D2Client calls it: its one reference is LNG_Initialize's `mov ecx, offset LNG_MessageSource` (0x6FAA12E5), and nothing reads the
//  pfnMessageSource of the list node it is stored in.
int32_t __stdcall LNG_MessageSource(DWORD dwMessageId, LPSTR pszBuffer, int32_t nMaxLength)
{
    if (!ghLangModule)
    {
        FOG_DisplayAssert("sghLNGInstance", szLngCpp, 129);
        exit(-1);
    }

    if (STORM_SErrGetErrorStr(dwMessageId, pszBuffer, nMaxLength))
        return 1;

    lstrcpynA(pszBuffer, gpszLngErrorText, nMaxLength);
    return 0;
}

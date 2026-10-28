#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "CoreArchive.h"

//  CoreArchive.cpp -- the cel file loader of D2Client.dll 1.10: a cel file loaded from the game's archive by name, and freed. The original source
//  file is D2Client\CORE\ARCHIVE.CPP (the three functions pass its path, with lines 83/86, 104/107 and 122); it is named CoreArchive.cpp here.
//  Address range 0x6FAA1000..0x6FAA115F, the first code of the image.

//  The path literal the three functions pass as __FILE__ is szSrcFile, declared in CoreArchive.h.

//  The name buffer is a MAX_PATH array: the original's `sub esp, 108h` is it plus the four-byte cel slot the normaliser fills.
static_assert(MAX_PATH == 260, "ARCHIVE_CellFileLoad: `sub esp, 108h` at 0x6FAA1000 is 260 + 4");


//1.10f: D2Client.0x6FAA1000
//  Loads a cel file by name: the name is copied into a MAX_PATH buffer, the .dcc or .dc6 extension bAllowCompressed picks is appended
//  (D2CMP_SPRITECACHE_GetGfxFileExtension), the file is read whole from D2Win's archive (ARCHIVE_LoadFile, with no size out-parameter: the 0
//  left in eax by the inlined strlen is pushed in its place), and D2CMP_CELCMP_CelFileNormalize makes the cel file of the bytes, writing it
//  into a slot cleared first. The slot is the answer: 0 when the normaliser wrote nothing.
//  The copy and the append are MSVC's inlined strcpy and strcat: a one-register byte loop that walks the source and stores at a fixed distance
//  from it, then two `repne scasb` and a `rep movsd` / `movsb` pair. Written as the calls they are.
//  The normaliser's sixth argument is the name buffer (nUnused); D2CMP never reads it, so it goes in as an int.
D2CellFileStrc *__fastcall ARCHIVE_CellFileLoad(char *pszName, int32_t bAllowCompressed)
{
    char szFilename[MAX_PATH];
    void *pFileData;
    D2CellFileStrc *pCellFile;

    strcpy(szFilename, pszName);
    strcat(szFilename, D2CMP_SPRITECACHE_GetGfxFileExtension(bAllowCompressed));
    pFileData = ARCHIVE_LoadFile(D2WIN_GetArchive(), szFilename, NULL, szSrcFile, 83);
    pCellFile = NULL;
    D2CMP_CELCMP_CelFileNormalize((D2CellFileStrc *)pFileData, &pCellFile, szSrcFile, 86, -1, (int32_t)(UINT_PTR)szFilename);
    return pCellFile;
}

//1.10f: D2Client.0x6FAA10A0
//  ARCHIVE_CellFileLoad with the file's size handed back through pdwFileSize (ARCHIVE_LoadFile's size pointer). Its callers are in
//  ENV_LoadImages, each with bAllowCompressed 0.
//  The body differs from ARCHIVE_CellFileLoad's in four things only: pdwFileSize parked in ebx and bAllowCompressed read from the stack
//  (`retn 4`), every frame displacement four more for the extra saved register, the two source lines (104 and 107 against 83 and 86), and
//  ARCHIVE_LoadFile's size pointer (`push ebx`, where the other pushes the 0 in eax).
D2CellFileStrc *__fastcall ARCHIVE_CellFileLoadWithSize(char *pszName, uint32_t *pdwFileSize, int32_t bAllowCompressed)
{
    char szFilename[MAX_PATH];
    void *pFileData;
    D2CellFileStrc *pCellFile;

    strcpy(szFilename, pszName);
    strcat(szFilename, D2CMP_SPRITECACHE_GetGfxFileExtension(bAllowCompressed));
    pFileData = ARCHIVE_LoadFile(D2WIN_GetArchive(), szFilename, pdwFileSize, szSrcFile, 104);
    pCellFile = NULL;
    D2CMP_CELCMP_CelFileNormalize((D2CellFileStrc *)pFileData, &pCellFile, szSrcFile, 107, -1, (int32_t)(UINT_PTR)szFilename);
    return pCellFile;
}

//1.10f: D2Client.0x6FAA1140
//  Frees a cel file: D2CMP_CELCMP_CelFileFreeHardware on it whatever it is, then FOG_Free at line 122 when it is not NULL. void: eax comes back
//  as the last call left it (FOG_Free's answer, or the hardware free's 0 for NULL) and no caller reads it. It is a __fastcall of one argument:
//  the handle in ecx, nothing in edx, a bare `retn`.
void __fastcall ARCHIVE_CellFileFree(D2CellFileStrc *pCellFile)
{
    D2CMP_CELCMP_CelFileFreeHardware(pCellFile);
    if (pCellFile)
        FOG_Free(pCellFile, szSrcFile, 122, 0);
}

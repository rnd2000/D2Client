#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "PalShift.h"

//  PalShift.cpp -- the colour remaps a monster is drawn through, in D2Client.dll 1.10: PALSHIFT_Init and PALSHIFT_Shutdown, the on-demand load of a
//  monster class's palshift.dat, PALSHIFT_GetUnitShiftTable and PALSHIFT_GetGreenBloodRemap, and the roll of a unique monster's shift.
//  The original source file is GAME\PalShift.cpp (PALSHIFT_Init's FOG_Alloc calls name it). Address range 0x6FAADEA0..0x6FAAE5AD.
//
//  PALSHIFT_GetUnitShiftTable picks among three kinds of remap from the unit's palette shift 2:
//    * the monster's own palshift.dat: eight 256-byte tables read from Data\Global\Monsters\<code>\Cof\palshift.dat on first sight of the class
//      into a cache record, D2ClientPalShiftStrc;
//    * that record's green copy: the same eight tables run through GreenBlood.dat, built only for a monstats2.txt LocalBlood 2 monster and chosen
//      only while Color.txt has switched green blood on;
//    * the thirty tables of RandTransforms.dat, for a shift of 8 and up, which is what PALSHIFT_RollUniqueShift rolls for a unique monster.
//  GFX_DrawUnitCels also draws some components through GreenBlood.dat's own table, which PALSHIFT_GetGreenBloodRemap hands out.
//
//  The source file name that every FOG_Alloc, FOG_Free and FOG_DisplayAssert call of the unit passes (0x6FB75FAC).
static const char szSourceFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\GAME\\PalShift.cpp";

//  The record's layout as the unit's instructions address it (D2Structs.h asserts the two sizes, 4104 and 8).
static_assert(offsetof(D2ClientPalShiftStrc, nShift) == 4,
              "the 0x800 read lands at +4 (add eax, 4 at 0x6FAAE23B)");
static_assert(offsetof(D2ClientPalShiftStrc, nGreenShift) == 0x804,
              "lea eax, [edx+804h] at 0x6FAAE35F; [esi+ebx+804h] at 0x6FAAE4E1");
static_assert(offsetof(D2ClientPalShiftStrc, pNext) == 0x1004,
              "[ecx+1004h] at 0x6FAADFBB, [eax+1004h] at 0x6FAAE2ED");
static_assert(offsetof(D2ClientMonPalShiftStrc, pPalShift) == 4,
              "[ecx+eax*8+4] at 0x6FAAE2CE and 0x6FAAE3DF");
static_assert(offsetof(D2MonStatsTxt, dwCode) == 0x10,
              "movsx ecx, byte ptr [eax+10h] at 0x6FAAE072");
static_assert(offsetof(D2MonStats2Txt, nLocalBlood) == 0x11C,
              "cmp byte ptr [eax+11Ch], 2 at 0x6FAAE354 and 0x6FAAE4D0");

//1.10f: D2Client.0x6FAADEA0
//  Builds the palette-shift tables at the start of a game. Three things happen and only the first is unconditional:
//    * gpMonPalShift gets eight bytes per monstats.txt row, zeroed, and each record's first dword is set to its own index (an identity mapping,
//      so a monster with no transform maps to itself); the second dword stays null and is what PALSHIFT_GetUnitShiftTable tests;
//    * RandTransforms.dat is read whole into gRandTransformTables, and gbRandTransformsLoaded is raised only if that file was there;
//    * GreenBlood.dat is read into a fresh 256-byte buffer; if it is missing, gpGreenBloodRemap stays whatever it was.
//  The first member of D2ClientMonPalShiftStrc is written here and read nowhere in the module; the second is written by
//  PALSHIFT_EnsureMonsterPalette and read by PALSHIFT_GetUnitShiftTable.
//  The two FOG_Alloc line numbers run backwards (the monster table is PalShift.cpp:99, the green-blood buffer :82): the green-blood block is a
//  helper of the source that the original compiled into this function, so it stays where the bytes put it.

//  0x1E00 twice over: as thirty 256-byte tables (the bound PALSHIFT_GetUnitShiftTable asserts), and as the distance from the array to the next
//  named global.
static_assert(30 * 256 == 0x1E00,
              "RandTransforms.dat is read as 30 tables of 256 bytes");
static_assert(0x107C18 + 0x1E00 == 0x109A18,
              "gRandTransformTables ends exactly where gbRandTransformsLoaded begins");

void __cdecl PALSHIFT_Init()
{
    uint32_t nPalShiftSize;
    int32_t iMonster;
    int32_t nMonStatsTxtRecordCount;
    void *hFile;
    void *hFile2;

    nPalShiftSize = 8 * sgptDataTables->nMonStatsTxtRecordCount;
    gpMonPalShift = (D2ClientMonPalShiftStrc *)(UINT_PTR)FOG_Alloc(nPalShiftSize, szSourceFile, 99, 0);
    memset(gpMonPalShift, 0, nPalShiftSize);

    nMonStatsTxtRecordCount = sgptDataTables->nMonStatsTxtRecordCount;
    for (iMonster = 0; iMonster < nMonStatsTxtRecordCount; ++iMonster)
    {
        gpMonPalShift[iMonster].nMonsterClassId = iMonster;
    }

    gpMonPalShiftCache = NULL;

    if (ARCHIVE_OpenFileOrTrace(0, "Data\\Global\\Monsters\\RandTransforms.dat", &hFile, 1))
    {
        ARCHIVE_ReadFile(NULL, hFile, gRandTransformTables, 0x1E00);
        ARCHIVE_CloseFile(NULL, hFile);
        gbRandTransformsLoaded = 1;
    }

    if (ARCHIVE_OpenFileOrTrace(0, "Data\\Global\\Monsters\\GreenBlood.dat", &hFile2, 1))
    {
        gpGreenBloodRemap = (uint8_t *)(UINT_PTR)FOG_Alloc(256, szSourceFile, 82, 0);
        memset(gpGreenBloodRemap, 0, 0x100);
        ARCHIVE_ReadFile(NULL, hFile2, gpGreenBloodRemap, 0x100);
        ARCHIVE_CloseFile(NULL, hFile2);
    }
}


//1.10f: D2Client.0x6FAADFB0
//  Frees what PALSHIFT_Init and the loader built, at the end of every game: the cache chain (FOG_Free line 132), the per-monster table (138) and
//  the green-blood remap (144). In the cache loop the head is advanced inside the loop: pNext is read before the free and gpMonPalShiftCache is
//  stored on every pass, so the global always names the first node not yet freed and is NULL when the loop ends. void: nothing reads eax after
//  the one call.
void __cdecl PALSHIFT_Shutdown()
{
    D2ClientPalShiftStrc *pNode;
    D2ClientPalShiftStrc *pNext;

    pNode = gpMonPalShiftCache;
    if (pNode)
    {
        do
        {
            pNext = pNode->pNext;
            FOG_Free(pNode, szSourceFile, 132, 0);
            pNode = pNext;
            gpMonPalShiftCache = pNode;
        }
        while (pNext);
    }

    if (gpMonPalShift)
    {
        FOG_Free(gpMonPalShift, szSourceFile, 138, 0);
        gpMonPalShift = NULL;
    }

    if (gpGreenBloodRemap)
    {
        FOG_Free(gpGreenBloodRemap, szSourceFile, 144, 0);
        gpGreenBloodRemap = NULL;
    }
}


//1.10f: D2Client.0x6FAAE030
//  Loads a monster class's palshift.dat once and hangs it on the class. Called when a monster is assigned (SCMD_HandleSrvAC_SpawnMonster). void: the
//  product is the cache record and the pointer in gpMonPalShift[class]. The original compiles this function without optimisation (an ebp frame, every
//  local in memory, each test a jnz over a jmp to the epilogue). The shape is close to the source's, so the C returns early three times, as the bytes
//  do. It calls DATATBLS_GetMonStatsTxtRecord and PALSHIFT_GetMonStatsRecordCount out of line, where the unit's optimised functions write them in
//  place. The directory is monstats.txt's Code column, a space turned into a NUL. The bytes build three characters (+0x10 into szCode[0], +0x11 into
//  szCode[1], +0x12 into szCode[2]) and then store 0 into szCode[3] and 0 into szCode[2] again (0x6FAAE102, 0x6FAAE109), so the path carries two
//  characters and the third is read and thrown away. Written as the bytes have it. The path "Data\Global\Monsters\" is copied byte by byte (an
//  unoptimised string copy), szCode is appended and the tail "\Cof\palshift.dat" added; the library calls are written. The record: FOG_Alloc(0x1008,
//  PalShift.cpp:244), zeroed by rep stosd of 402h dwords; the file's 0x800 bytes go into nShift at +4; PALSHIFT_ApplyGreenBlood builds nGreenShift;
//  then the class id at +0, and the record is pushed onto the front of gpMonPalShiftCache. BUG: FOG_Alloc's answer is untested. LATENT. The insert is
//  a scan for an index it already has: it walks every monstats.txt row comparing the row number with the record's class id to write one slot, calling
//  PALSHIFT_GetMonStatsRecordCount on every pass (0x6FAAE2A8) and comparing signed (jge). Correct and wasteful; reproduced.
void __fastcall PALSHIFT_EnsureMonsterPalette(D2C_MonsterIds nMonsterClassId)
{
    D2MonStatsTxt *pMonStats;
    const char *pszMonCode;
    D2ClientPalShiftStrc *pPalShift;
    void *hFile;
    char szCode[4];
    char szFileName[256];
    int32_t i;

    pMonStats = DATATBLS_GetMonStatsTxtRecord(nMonsterClassId);
    if (!pMonStats)
        return;

    if (PALSHIFT_FindCacheEntry(nMonsterClassId))
        return;

    pszMonCode = (const char *)&pMonStats->dwCode;
    szCode[0] = (pszMonCode[0] == ' ') ? 0 : pszMonCode[0];
    szCode[1] = (pszMonCode[1] == ' ') ? 0 : pszMonCode[1];
    szCode[2] = (pszMonCode[2] == ' ') ? 0 : pszMonCode[2];
    szCode[3] = 0;
    szCode[2] = 0;      //  the third character, overwritten (0x6FAAE109)

    strcpy(szFileName, "Data\\Global\\Monsters\\");
    strcat(szFileName, szCode);
    strcat(szFileName, "\\Cof\\palshift.dat");

    if (!ARCHIVE_OpenFileOrTrace(0, szFileName, &hFile, 1))
        return;

    pPalShift = (D2ClientPalShiftStrc *)(UINT_PTR)FOG_Alloc(sizeof(D2ClientPalShiftStrc), szSourceFile, 244, 0);
    memset(pPalShift, 0, sizeof(D2ClientPalShiftStrc));
    ARCHIVE_ReadFile(NULL, hFile, pPalShift->nShift, 0x800);
    ARCHIVE_CloseFile(NULL, hFile);
    PALSHIFT_ApplyGreenBlood(nMonsterClassId, pPalShift);

    pPalShift->nMonsterClassId = nMonsterClassId;
    pPalShift->pNext = gpMonPalShiftCache;
    gpMonPalShiftCache = pPalShift;

    for (i = 0; i < PALSHIFT_GetMonStatsRecordCount(); ++i)
    {
        if (pPalShift->nMonsterClassId == i)
            gpMonPalShift[i].pPalShift = pPalShift;
    }
}


//1.10f: D2Client.0x6FAAE2E0
//  The cache record for a class, or NULL. A singly linked list, newest first, compared on the class id at +0 and stepped through pNext at
//  +0x1004. PALSHIFT_EnsureMonsterPalette loads only when this says NULL.
D2ClientPalShiftStrc *__fastcall PALSHIFT_FindCacheEntry(D2C_MonsterIds nMonsterClassId)
{
    D2ClientPalShiftStrc *pEntry;

    for (pEntry = gpMonPalShiftCache; pEntry; pEntry = pEntry->pNext)
    {
        if (pEntry->nMonsterClassId == (int32_t)nMonsterClassId)
            return pEntry;
    }

    return NULL;
}


//1.10f: D2Client.0x6FAAE300
//  Builds the green-blood copy of a fresh record, right after the file is read, only for a class whose monstats2.txt row has LocalBlood 2; for
//  every other class nGreenShift stays as the loader zeroed it. The original inlines DATATBLS_GetMonStats2TxtRecord here.
//  The eight tables are copied whole (rep movsd, 200h dwords), then each entry of the copy is replaced by GreenBlood.dat's entry for the original
//  table's value, unless that entry is 0: a zero in GreenBlood.dat means "leave this colour", not "make it black". The index goes through a
//  one-byte local (an unsigned byte, written as one). The loops count down in the bytes; the order of the writes does not matter.
//  Color.txt is not consulted here: the green copy is built for every LocalBlood-2 class whether green blood is on or not;
//  PALSHIFT_GetUnitShiftTable decides at draw time.
//  BUG: the remap is not tested for NULL (mov esi, gpGreenBloodRemap at 0x6FAAE374, mov al, [eax+esi] at 0x6FAAE39B), and PALSHIFT_Init allocates
//  it only when GreenBlood.dat opens. LATENT: a LocalBlood-2 monster in an install without the file faults here; the stock install has it.
void __fastcall PALSHIFT_ApplyGreenBlood(D2C_MonsterIds nMonsterClassId, D2ClientPalShiftStrc *pPalShift)
{
    D2MonStats2Txt *pMonStats2Txt;
    uint8_t *pRemap;
    int32_t nTable;
    int32_t nEntry;
    uint8_t nIndex;
    uint8_t nRemapped;

    pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord((int32_t)nMonsterClassId);

    if (!pMonStats2Txt)
        return;
    if (pMonStats2Txt->nLocalBlood != 2)
        return;

    memcpy(pPalShift->nGreenShift, pPalShift->nShift, 0x800);

    pRemap = gpGreenBloodRemap;
    for (nTable = 0; nTable < 8; ++nTable)
    {
        for (nEntry = 0; nEntry < 256; ++nEntry)
        {
            nIndex = pPalShift->nShift[nTable][nEntry];
            nRemapped = pRemap[nIndex];
            if (nRemapped)
                pPalShift->nGreenShift[nTable][nEntry] = nRemapped;
        }
    }
}


//  0x16B and 0x16C in the bytes: the two classes that shifts 0 and 1 are kept for.
static_assert(MONSTER_NECROSKELETON == 0x16B && MONSTER_NECROMAGE == 0x16C,
              "cmp edi, 16Bh / cmp edi, 16Ch at 0x6FAAE40B, 0x6FAAE417");

//1.10f: D2Client.0x6FAAE3C0
//  Which 256-byte colour table a monster is drawn through, or NULL for the default palette. GFX_DrawUnitCels starts every COF layer from the
//  answer, lets GFXUTIL_GetComponentColor replace it, and passes it to D2GFX_TEXTURE_CelDraw as its palette. The original inlines
//  DATATBLS_GetMonStats2TxtRecord here.
//  NULL unless the unit is a monster and its class has a loaded record (gpMonPalShift[class].pPalShift), which only a class with a palshift.dat
//  can have. Then the unit's palette shift 2 decides, through an eight-case jump table (cmp esi, 7 / ja, unsigned, so a negative shift is the
//  default):
//    0, 1     the record's own table, for MONSTER_NECROSKELETON and MONSTER_NECROMAGE only, NULL for any other class (compared unsigned: jb, ja)
//    2..7     the record's green table when green blood is on and the class is LocalBlood 2, else its own table
//    8 and up the RandTransforms.dat table `shift - 8`, asserted to be 0..29, when that file was read; when it was not, the 2..7 arm
//             (default falls into case 2)
//  "Green blood is on" is GAME_IsGreenBloodEnabled, whose global is set only when Data\Local\Color.txt's first byte is '1'.
//  The assert "eTransform >= 0 && eTransform < NUM_RANDTABLES" (PalShift.cpp:352, push 160h) is signed, on the biased index: sub esi, 8 / js and
//  cmp esi, 1Eh / jge. CUNIT_CreateMonsterUnit sends any shift of 30 and more back to 2, so its rolls reach at most 29 here, table 21.
//  BUG: the two record arms address the table as lea eax, [esi+ebx+4] and [esi+ebx+804h] with esi = shift << 8, for any shift that reaches them,
//  and a shift outside 0..7 reaches them when RandTransforms.dat was not read: 8..15 address nGreenShift through the ordinary arm, from 16 up the
//  256 bytes run past the end of the 4104-byte record, and a negative shift lands before it. LATENT: the stock data has RandTransforms.dat, so
//  the flag is set and the default arm takes the RandTransforms branch.
//  The C computes the address as the bytes do, in unsigned integer arithmetic, rather than as an index that would be undefined out of range.
uint8_t *__fastcall PALSHIFT_GetUnitShiftTable(D2UnitStrc *pUnit)
{
    D2ClientPalShiftStrc *pPalShift;
    D2MonStats2Txt *pMonStats2Txt;
    int32_t nShift;
    int32_t nTransform;

    if (!pUnit)
        return NULL;
    if (pUnit->dwUnitType != UNIT_MONSTER)
        return NULL;

    //  BUG: no bound on the class id before this index (0x6FAAE3D6..0x6FAAE3DF). LATENT: a monster's class is bounded where it is written.
    pPalShift = gpMonPalShift[pUnit->dwClassId].pPalShift;
    if (!pPalShift)
        return NULL;

    nShift = CUNIT_GetUnitPaletteShift2(pUnit);
    switch (nShift)
    {
    case 0:
    case 1:
        if ((uint32_t)pUnit->dwClassId < MONSTER_NECROSKELETON)
            return NULL;
        if ((uint32_t)pUnit->dwClassId > MONSTER_NECROMAGE)
            return NULL;
        break;

    default:
        if (gbRandTransformsLoaded)
        {
            nTransform = nShift - 8;
            if (!(nTransform >= 0 && nTransform < 30))
            {
                FOG_DisplayAssert("eTransform >= 0 && eTransform < NUM_RANDTABLES", szSourceFile, 352);
                exit(-1);
            }
            //  the row: shl eax, 8 / add eax, offset gRandTransformTables (0x6FAAE444, 0x6FAAE448)
            return gRandTransformTables[nTransform];
        }
        //  gbRandTransformsLoaded clear: into the 2..7 arm (0x6FAAE435)

    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        if (GAME_IsGreenBloodEnabled())
        {
            pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pUnit->dwClassId);

            if (pMonStats2Txt && pMonStats2Txt->nLocalBlood == 2)
            {
                return (uint8_t *)((UINT_PTR)pPalShift + offsetof(D2ClientPalShiftStrc, nGreenShift) + ((uint32_t)nShift << 8));
            }
        }
        break;
    }

    return (uint8_t *)((UINT_PTR)pPalShift + offsetof(D2ClientPalShiftStrc, nShift) + ((uint32_t)nShift << 8));
}


//1.10f: D2Client.0x6FAAE510
//  Rolls a unique monster's palette shift. Called by CUNIT_CreateMonsterUnit, which rolls it for a unique monster unless monstats2.txt's
//  NoUniqueShift is set, lets a non-zero nUtrans from monstats2.txt or superuniques.txt replace it, and sends 30 and up back to 2. A roll that
//  stands therefore reaches PALSHIFT_GetUnitShiftTable as 9..29, RandTransforms.dat tables 1..21: the random colours of unique monsters.
//  A NULL unit answers 9. Otherwise the unit's own seed (at +0x20) is re-seeded from dwClassId + nOffset (the offset zero-extended), stepped once,
//  and its new low dword taken % 30 (unsigned, div) plus 9. The original inlines SEED_RollRandomNumber here.
uint32_t __fastcall PALSHIFT_RollUniqueShift(D2UnitStrc *pUnit, uint16_t nOffset)
{
    D2SeedStrc *pSeed;

    if (!pUnit)
        return 9;

    pSeed = &pUnit->pSeed;
    D2COMMON_SEED_InitLowSeed(pSeed, pUnit->dwClassId + nOffset);

    return (uint32_t)SEED_RollRandomNumber(pSeed) % 30 + 9;
}


//1.10f: D2Client.0x6FAAE560
//  Returns gpGreenBloodRemap. GFX_DrawUnitCels passes the answer on as a palette; GFX_DrawUnitOverlays throws it away and pushes a literal 0.
uint8_t *__cdecl PALSHIFT_GetGreenBloodRemap()
{
    return gpGreenBloodRemap;
}


//1.10f: D2Client.0x6FAAE5A0
//  Returns sgptDataTables->nMonStatsTxtRecordCount; the loop condition of PALSHIFT_EnsureMonsterPalette. It and DATATBLS_GetMonStatsTxtRecord
//  (0x6FAAE570, written in Shared.cpp) are out-of-line copies of small inline helpers: PALSHIFT_EnsureMonsterPalette calls both, where the
//  unit's optimised functions write them in place.
int32_t __cdecl PALSHIFT_GetMonStatsRecordCount()
{
    return sgptDataTables->nMonStatsTxtRecordCount;
}

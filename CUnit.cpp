#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "CoreArchive.h"
#include "CUnit.h"

//  CUnit.cpp -- the client's units in D2Client.dll 1.10: the two unit hash tables and their lookups, creating, initialising, registering and
//  destroying units of every type, warping, the palette shifts, the skill select procs and the hit test, the whole-table passes, the facing
//  helpers, overlays, the inventory teardown and the packet queue. Seventy-two functions, address range 0x6FB269F0..0x6FB2A6AB.
//  The original source file is UNIT\CUnit.cpp (36 of its functions carry the path in their asserts).

//  The path every assert in this file pushes: __FILE__ in the original, so a single object.
static const char szCUnitCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UNIT\\CUnit.cpp";


//  The client keeps every unit in two contiguous hash tables of identical shape, [6][128] of D2UnitStrc *: gpUnitTableById (0x6FBBAA00) and
//  gpUnitTableByClientId (0x6FBBB600, 0xC00 bytes later). Row is a D2C_UnitTypes value, bucket is `dwUnitId & 0x7F`, the chain runs through
//  pListNext (+0xE4) and the key is dwUnitId (+0x0C) checked against dwUnitType (+0x00). CUNIT_ClearUnitTables drops both.
//  A unit's table is chosen by dwFlags bit 21 (UNITFLAG_ISASYNC): clear, the by-id table; set, the client-id table. The table names are the
//  original's, from the asserts in CUNIT_AddUnitToTable (`!UnitGetFromId(...)` and `!UnitGetFromClientId(...)`).
//  The lookup walk has one shape: id and type match, return the unit; id matches and type does not, FOG_Trace and an assert that kills the
//  process; id does not match, follow pListNext. The second type test is not a second check but the assert's own condition (the D2_ASSERT
//  macro): the bytes re-read `[esi]` after FOG_Trace returns (0x6FB26A25, 0x6FB26B46, 0x6FB26B95). FOG_DisplayAssert is followed by an
//  unconditional `exit(-1)`, so the `continue` after it is unreachable; the trace is not, it is the last thing the process prints.
//  Written out as the original has it.
//
//  The original carries these three as literals (`cmp eax, 3` at 0x6FB26B30, `cmp dword ptr [esi], 4` at 0x6FB26C4A, and
//  `shr eax, 0Ah; and eax, 1` at 0x6FB26AFC); this file spells each as a name, so a renumbering of the enum fails here.
static_assert(UNIT_MISSILE == 3,
              "CUNIT_GetOwnerUnit's missile arm is `cmp eax, 3` (0x6FB26B30)");
static_assert(UNIT_ITEM == 4,
              "CUNIT_UnlinkAndFreeUnit's item arm is `cmp dword ptr [esi], 4` "
              "(0x6FB26C4A)");
static_assert(UNITFLAGEX_STOREOWNERINFO == 0x400,
              "CUNIT_GetOwnerUnit tests bit 10 of dwFlagEx: `shr eax, 0Ah; "
              "and eax, 1` (0x6FB26AFC)");


//1.10f: D2Client.0x6FB269F0
//  Looks a unit up by the server's id: one bucket, one chain, one comparison. The row stride is a shift by 7 and the bucket a mask of 0x7F.
//  Returns the unit or NULL (the original's return type was `_DWORD *`, `&pUnit->dwUnitType`, which is the unit pointer).
D2UnitStrc *__fastcall CUNIT_FindUnitByGuidAndType(uint32_t dwUnitGUID, D2C_UnitTypes nUnitType)
{
    D2UnitStrc *pUnit;

    //  DIFFERENCE: a type of 6 or more finds nothing. The original indexes its table with no bound (`shl ecx, 7`, 0x6FB269FE) and walks
    //  whatever the .data past its six rows holds; ours would read past CUnit.h's [6][128], which C++ leaves undefined. Such a type comes only
    //  from the server -- a packet field no handler bounds, or the byte past a one-byte packet 0x6E..0x72 the router reads, which D2Net's
    //  server sends as 0x6F when it drops a client. NOT ESTABLISHED for the stock server.
    if ((uint32_t)nUnitType >= (uint32_t)UNIT_TYPES_COUNT)
        return NULL;

    pUnit = gpUnitTableById[nUnitType][dwUnitGUID & 0x7F];

    while (pUnit)
    {
        if (pUnit->dwUnitId == dwUnitGUID)
        {
            if (pUnit->dwUnitType == nUnitType)
                return pUnit;

            FOG_Trace("current unit is %d, table is supposed to be %d\n", pUnit->dwUnitType, nUnitType);

            //  the assert's own condition, re-read at 0x6FB26A25
            if (pUnit->dwUnitType != nUnitType)
            {
                FOG_DisplayAssert((uint32_t)"ptUnit->eType == eTOU", (uint32_t)szCUnitCpp, 88);
                exit(-1);
            }
            //  unreachable: FOG_DisplayAssert never returns here
        }

        pUnit = pUnit->pListNext;
    }

    return NULL;
}


//1.10f: D2Client.0x6FB26A60
//  The same lookup over the other table, byte for byte the same shape: `gpUnitTableByClientId` instead of `gpUnitTableById`, and it reaches the
//  same assert (line 88). A unit is in this table when dwFlags bit 21 (UNITFLAG_ISASYNC) is set: the units the client invented rather than
//  was told about, which it has to find by the id it handed out.
D2UnitStrc *__fastcall CUNIT_FindUnitByClientIdAndType(int32_t nUnitId, D2C_UnitTypes nUnitType)
{
    D2UnitStrc *ptUnit;

    //  DIFFERENCE: a type of 6 or more finds nothing (see CUNIT_FindUnitByGuidAndType).
    if ((uint32_t)nUnitType >= (uint32_t)UNIT_TYPES_COUNT)
        return NULL;

    ptUnit = gpUnitTableByClientId[nUnitType][nUnitId & 0x7F];

    while (ptUnit)
    {
        if (ptUnit->dwUnitId == (uint32_t)nUnitId)
        {
            if (ptUnit->dwUnitType == nUnitType)
                return ptUnit;

            FOG_Trace("current unit is %d, table is supposed to be %d\n", ptUnit->dwUnitType, nUnitType);

            if (ptUnit->dwUnitType != nUnitType)
            {
                FOG_DisplayAssert((uint32_t)"ptUnit->eType == eTOU", (uint32_t)szCUnitCpp, 88);
                exit(-1);
            }
        }

        ptUnit = ptUnit->pListNext;
    }

    return NULL;
}


//1.10f: D2Client.0x6FB26AD0
//  Who owns this unit: a missile's caster, a minion's master. The argument is asserted (`test ecx, ecx`, then FOG_DisplayAssert("ptUnit", ...,
//  129) and `exit(-1)`), so NULL never returns. dwFlagEx bit 10 (UNITFLAGEX_STOREOWNERINFO) clear: no owner, NULL, with neither table touched.
//  The table is chosen by the owner's type, not by the owner's flags: `cmp edi, 3` (dwOwnerType) and `(gpUnitTableByClientId+600h)[ecx*4]`
//  (0x600 is 3 * 128 * 4, the missile row folded into the address), so a missile owner is looked up in the client-id table and everything
//  else in the by-id table. The original inlines CUNIT_FindUnitByClientIdAndType and CUNIT_FindUnitByGuidAndType in the two arms.
D2UnitStrc *__fastcall CUNIT_GetOwnerUnit(D2UnitStrc *pUnit)
{
    D2C_UnitTypes dwOwnerType;

    if (!pUnit)
    {
        FOG_DisplayAssert((uint32_t)"ptUnit", (uint32_t)szCUnitCpp, 129);
        exit(-1);
    }

    if ((pUnit->dwFlagEx & UNITFLAGEX_STOREOWNERINFO) == 0)
        return NULL;

    dwOwnerType = pUnit->dwOwnerType;

    if (dwOwnerType == UNIT_MISSILE)
    {
        return CUNIT_FindUnitByClientIdAndType(pUnit->dwOwnerGUID, UNIT_MISSILE);
    }

    return CUNIT_FindUnitByGuidAndType(pUnit->dwOwnerGUID, dwOwnerType);
}


//1.10f: D2Client.0x6FB26BE0
//  Removes a unit from the by-id table: computes the row's address (`shl ecx, 9`: a byte address, 128 pointers of 4 bytes) and the bucket,
//  and the work is CUNIT_UnlinkAndFreeUnit's.
void __fastcall CUNIT_RemoveUnitById(int32_t nUnitId, D2C_UnitTypes nUnitType)
{
    //  DIFFERENCE: a type of 6 or more removes nothing (see CUNIT_FindUnitByGuidAndType).
    if ((uint32_t)nUnitType >= (uint32_t)UNIT_TYPES_COUNT)
        return;

    CUNIT_UnlinkAndFreeUnit(gpUnitTableById[nUnitType], nUnitId & 0x7F, nUnitId, nUnitType);
}


//1.10f: D2Client.0x6FB26C00
//  Unlinks a unit from its bucket and destroys it; the only function here that writes to a table. The match takes both keys, dwUnitId
//  (+0x0C) first and then dwUnitType (+0), so a bucket collision between two units of different types is walked past, with no assert on a
//  mismatch, unlike the lookups. The unlink is the ordinary singly-linked one: no previous node means the bucket head is rewritten
//  (0x6FB26C39), otherwise the predecessor's pListNext (0x6FB26C44). An item (`cmp dword ptr [esi], 4`, UNIT_ITEM) gets one extra call,
//  INV_ClearHoverForItem, so a destroyed item cannot be left as the cursor's hover target.
//  `retn 8` confirms the two stack arguments; ecx and edx carry the row and the bucket, which is why this is __fastcall with four parameters.
void __fastcall CUNIT_UnlinkAndFreeUnit(D2UnitStrc **pTable, int32_t nBucket, int32_t nUnitId, D2C_UnitTypes nUnitType)
{
    D2UnitStrc *pUnit;
    D2UnitStrc *pPrev;

    pUnit = pTable[nBucket];
    pPrev = NULL;

    if (!pUnit)
        return;

    while (pUnit->dwUnitId != (uint32_t)nUnitId || pUnit->dwUnitType != (uint32_t)nUnitType)
    {
        pPrev = pUnit;
        pUnit = pUnit->pListNext;
        if (!pUnit)
            return;
    }

    if (pPrev)
        pPrev->pListNext = pUnit->pListNext;
    else
        pTable[nBucket] = pUnit->pListNext;

    if (pUnit->dwUnitType == UNIT_ITEM)
        INV_ClearHoverForItem(pUnit);

    CUNIT_DestroyUnit(pUnit);
}


//1.10f: D2Client.0x6FB26C70
//  The client-id twin of CUNIT_RemoveUnitById: the same except for the table it names (0x6FB26C7E); both tail into CUNIT_UnlinkAndFreeUnit.
void __fastcall CUNIT_RemoveUnitByClientId(int32_t nUnitId, D2C_UnitTypes nUnitType)
{
    //  DIFFERENCE: a type of 6 or more removes nothing. The original indexes past its six rows, into gpPlayerUnit and the globals after it; ours
    //  would be indexed past its end, which C++ leaves undefined.
    if ((uint32_t)nUnitType >= (uint32_t)UNIT_TYPES_COUNT)
        return;

    CUNIT_UnlinkAndFreeUnit(gpUnitTableByClientId[nUnitType], nUnitId & 0x7F, nUnitId, nUnitType);
}


//1.10f: D2Client.0x6FB26C90
//  Finds the room that holds the subtile coordinates, in three steps of growing cost: the player's own room, if the point is inside it; the
//  rooms adjacent to it, in order; DUNGEON_FindRoomBySubtileCoordinates over the whole act. A failure at step 3 kills the process:
//  FOG_DisplayWarning with a formatted line (CUnit.cpp:314), then an assert on `hFindRoom` (CUnit.cpp:315) and `exit(-1)`.
//  (0,0) is the "nowhere" coordinate and returns NULL before anything is searched (`test esi, esi` / `test edi, edi`, 0x6FB26CA6);
//  CUNIT_InitUnit relies on that for a unit created with no position.
//  The player is read from the global gpPlayerUnit (`mov ebp, gpPlayerUnit`, 0x6FB26C98), not through a call; the value is taken once and
//  reused, which matters because the printf at the end reports it.
//  The adjacent walk is unsigned (`jbe`, `jb`: the loop counter is `unsigned int`), the bounds tests are signed (`jl`, `jge`: the coordinates
//  are `int`). The adjacent room is NULL-tested (`test eax, eax`, 0x6FB26D25) before any field of it is read: a hole in the adjacency list is
//  skipped, not dereferenced.
D2ActiveRoomStrc *__fastcall CUNIT_FindRoomForCoords(int32_t nX, int32_t nY)
{
    D2UnitStrc *pPlayerUnit;
    D2ActiveRoomStrc *pRoom;
    D2ActiveRoomStrc *pFound;
    D2ActiveRoomStrc **ppRoomList;
    int32_t nNumRooms;
    uint32_t iRoom;
    char szScratch[512];

    pPlayerUnit = gpPlayerUnit;
    pRoom = NULL;

    if (!nX && !nY)
        return NULL;

    if (pPlayerUnit)
    {
        pRoom = D2COMMON_UNITS_GetRoom(pPlayerUnit);
        if (pRoom)
        {
            if (nX >= pRoom->tCoords.nSubtileX && nX < pRoom->tCoords.nSubtileX + pRoom->tCoords.nSubtileWidth && nY >= pRoom->tCoords.nSubtileY && nY < pRoom->tCoords.nSubtileY + pRoom->tCoords.nSubtileHeight)
            {
                return pRoom;
            }

            ppRoomList = NULL;
            nNumRooms = 0;
            D2COMMON_DUNGEON_GetAdjacentRoomsListFromRoom(pRoom, &ppRoomList, &nNumRooms);

            for (iRoom = 0; iRoom < (uint32_t)nNumRooms; ++iRoom)
            {
                D2ActiveRoomStrc *pAdj = ppRoomList[iRoom];

                if (!pAdj)
                    continue;

                if (nX >= pAdj->tCoords.nSubtileX && nX < pAdj->tCoords.nSubtileX + pAdj->tCoords.nSubtileWidth && nY >= pAdj->tCoords.nSubtileY && nY < pAdj->tCoords.nSubtileY + pAdj->tCoords.nSubtileHeight)
                {
                    //  the original tests the entry again before returning it (0x6FB26D5A), although the loop already skipped a NULL one
                    if (pAdj)
                        return pAdj;
                    break;
                }
            }
        }
    }

    pFound = D2COMMON_DUNGEON_FindRoomBySubtileCoordinates(gpAct, nX, nY);
    if (!pFound)
    {
        STORM_SStrPrintf(szScratch, 0x200, "Unable to find room for unit...  hControlUnit:%8x  hUnitRoom:%8x  ghAct:%8x  nX:%d  nY:%d", pPlayerUnit, pRoom, gpAct, nX, nY);
        FOG_DisplayWarning((uint32_t)szScratch, (uint32_t)szCUnitCpp, 314);
        FOG_DisplayAssert((uint32_t)"hFindRoom", (uint32_t)szCUnitCpp, 315);
        exit(-1);
    }

    return pFound;
}


static_assert(SKILLSFLAGINDEX2_WARP == 6,
              "skills.txt `warp`, bit 38: byte +8 against gdwBitMasks[6] -- "
              "`mov al, [eax+8]` / `test [ecx+18h], al` at 0x6FB26EC9..0x6FB26ECC");

//1.10f: D2Client.0x6FB26DD0
//  Moves a unit to a new place and does everything a teleport has to do around it: drop the selection, re-point the path, pull the palette
//  across, arm the automap reveal and, for the player, the warp animation.
//  The fourth parameter is not a room id but a flag: non-zero reaches DLIGHTMAP_StartWarpFlash, and it is handed on to ENV_OnLevelChange ->
//  ENV_ResetWeather(int bForce). The one written caller passes a byte out of packet 0x15.
//  The skill can cancel the flash. If the player's used skill carries skills.txt's `warp` column, the flag is zeroed before it is tested, so the
//  warp flash is skipped and ENV_OnLevelChange gets 0:
//
//      (uint8_t)SkillsTxt->dwFlags[1] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX2_WARP]
//
//  The bytes test the record's byte +8 against gdwBitMasks[6] (`mov al, [eax+8]` / `test [ecx+18h], al`, 0x6FB26EC9..0x6FB26ECC); that byte
//  and bit are the `warp` column, bit 38 of the flag run.
//  Four asserts, all fatal: no dynamic path (CUnit.cpp:356), a non-zero destination with no room found (358), a non-player unit asking for
//  the warp flash (379), and the second path placement failing (423).
//  dwFlags bit 16 gates the whole body -- `(dwFlags & 0x10000) == 0` -- and a unit with it set is warped by nothing at all, not even the path
//  update. The anim-mode early-outs are per type: a player in mode 0 or 17 and a monster in mode 0 or 12 are left alone; those four numbers
//  are the original's literals, written as such because this file has no enum that names them.
void __fastcall CUNIT_WarpUnit(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t bIsLevelChange)
{
    D2UnitStrc *pPlayerUnit;
    D2ActiveRoomStrc *pDestRoom;
    D2ActiveRoomStrc *pOldRoom;
    D2DynamicPathStrc *pDynamicPath;
    D2SkillStrc *pUsedSkill;
    D2SkillsTxt *pSkillsTxt;
    D2LevelsTxt *pOldLevel;
    D2LevelsTxt *pNewLevel;
    D2CoordStrc tSpawnPoint;
    uint32_t dwAnimMode;

    tSpawnPoint.nX = nX;
    pPlayerUnit = gpPlayerUnit;

    if (pUnit == pPlayerUnit)
        SELECT_ClearSelectedUnit();

    pDestRoom = CUNIT_FindRoomForCoords(nX, nY);

    pDynamicPath = pUnit->pDynamicPath;
    if (!pDynamicPath)
    {
        FOG_DisplayAssert((uint32_t)"hPath", (uint32_t)szCUnitCpp, 356);
        exit(-1);
    }

    if (nX && nY && !pDestRoom)
    {
        FOG_DisplayAssert((uint32_t)"Tried to warp to a room we didn't know about", (uint32_t)szCUnitCpp, 358);
        exit(-1);
    }

    if ((pUnit->dwFlags & 0x10000) != 0)
        return;

    dwAnimMode = pUnit->dwAnimMode;
    if (pUnit->dwUnitType == UNIT_PLAYER)
    {
        if (!dwAnimMode || dwAnimMode == 17)
            return;
    }
    else if (pUnit->dwUnitType == UNIT_MONSTER && (!dwAnimMode || dwAnimMode == 12))
    {
        return;
    }

    pOldRoom = NULL;

    if (pUnit == pPlayerUnit)
    {
        pOldRoom = D2COMMON_UNITS_GetRoom(pUnit);
        if (pOldRoom)
        {
            pUsedSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
            pSkillsTxt = pUsedSkill ? D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill(pUsedSkill)
                       : NULL;

            if (pSkillsTxt && ((uint8_t)pSkillsTxt->dwFlags[1] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX2_WARP]) != 0)
            {
                bIsLevelChange = 0;
            }
            else if (bIsLevelChange)
            {
                if (pUnit->dwUnitType != UNIT_PLAYER)
                {
                    FOG_DisplayAssert((uint32_t)"UnitGetType(hUnit) == UNIT_PLAYER", (uint32_t)szCUnitCpp, 379);
                    exit(-1);
                }
                DLIGHTMAP_StartWarpFlash();
            }
        }

        if (pDestRoom)
        {
            D2COMMON_DUNGEON_UpdatePops(pDestRoom, nX, nY, pOldRoom != pDestRoom);
            D2GFX_SetUnitPerspective(D2COMMON_DATATBLS_GetNoPerFromLevelsTxt(D2COMMON_DUNGEON_GetLevelIdFromRoom(pDestRoom)) == 0);

            if (pOldRoom)
            {
                pOldLevel = D2COMMON_DATATBLS_GetLevelsTxtRecord(D2COMMON_DUNGEON_GetLevelIdFromRoom(pOldRoom));
                pNewLevel = D2COMMON_DATATBLS_GetLevelsTxtRecord(D2COMMON_DUNGEON_GetLevelIdFromRoom(pDestRoom));

                if (pOldLevel->nPal != pNewLevel->nPal)
                {
                    D2WIN_10177(pNewLevel->nPal);
                    D2CMP_10097(pNewLevel->nPal);
                }
            }

            nX = tSpawnPoint.nX;
        }
    }

    if (!D2COMMON_10229(pDynamicPath, pUnit, pDestRoom, nX, nY))
    {
        tSpawnPoint.nX = nX;
        tSpawnPoint.nY = nY;
        D2COMMON_COLLISION_GetFreeCoordinates(pDestRoom, &tSpawnPoint, D2COMMON_UNITS_GetUnitSizeX(pUnit), 0x1C09u, 1);

        if (!D2COMMON_10231(pDynamicPath, pUnit, pDestRoom, (uint16_t)tSpawnPoint.nX, (uint16_t)tSpawnPoint.nY))
        {
            FOG_DisplayAssert((uint32_t)"fRet", (uint32_t)szCUnitCpp, 423);
            exit(-1);
        }
    }

    AUTOMAP_DelayReveal();

    if (pUnit == pPlayerUnit)
        ENV_OnLevelChange((void *)(UINT_PTR)bIsLevelChange);

    if (pUnit->dwUnitType == UNIT_PLAYER)
    {
        dwAnimMode = pUnit->dwAnimMode;
        if (D2COMMON_UNITS_GetRoom(pUnit))
        {
            if (dwAnimMode == 1 || dwAnimMode == 5)
                PLAYER_DoClientAction(CLTACTION_NEUTRAL, pUnit, NULL, TRUE);
        }
    }
}


//1.10f: D2Client.0x6FB27040
//  Allocates and initialises, the plain form: UNITS_AllocUnit(NULL, dwUnitType) -- the first argument is the memory pool, always NULL on the
//  client -- and on success CUNIT_InitUnit with nObjectMode 0. Of the three Create wrappers this one does nothing afterwards.
//  The NULL test here is what keeps CUNIT_InitUnit's defect latent: InitUnit tests its own pUnit and then writes through it regardless, and
//  all four of its callers test the allocation first.
D2UnitStrc *__fastcall CUNIT_CreateUnit(uint32_t dwUnitId, int32_t nClassId, int32_t nPosX, int32_t nPosY, D2C_UnitTypes dwUnitType, int32_t nItemMode)
{
    D2UnitStrc *pUnit;

    pUnit = D2COMMON_UNITS_AllocUnit(NULL, dwUnitType);
    if (!pUnit)
        return pUnit;

    return CUNIT_InitUnit(pUnit, dwUnitId, nClassId, nPosX, nPosY, dwUnitType, nItemMode, 0);
}


//1.10f: D2Client.0x6FB27080
//  Fills a fresh unit in and dispatches on its type to the subsystem that owns that kind.
//  BUG: the NULL test of pUnit (`cmp esi, edi / jz`, 0x6FB27092) guards only the expansion-flag read/modify/write; the store of
//  pUnit->dwUnitId (`mov [esi+0Ch], ebx`, 0x6FB270BE) is unconditional, so a NULL argument skips the flag update and then writes to address
//  0x0C. Reproduced as the bytes have it. UNREACHABLE: all four callers (CUNIT_CreateUnit, CUNIT_CreatePlayerUnit, CUNIT_CreateObjectUnit and
//  CUNIT_CreateClientOnlyUnit) test UNITS_AllocUnit's result before calling, and AllocUnit cannot return NULL at all.
//  dwFlagEx bit 25 (0x2000000) is set in an expansion game and cleared in a classic one.
//  The seed step is 64-bit and unsigned: when a room was found, the room's seed advances and the unit takes the new low word,
//  `(uint64)nLowSeed * 1791398085 + nHighSeed` split back into the two halves (`mul`, not `imul`, makes the multiply unsigned).
//  The tail chunk (`jmp 0x6FB2723D` at 0x6FB27212) tests dwUnitType with two equality tests (0 -> call, 1 -> call), not an unsigned compare.
//  Between the function's reported range and the chunk sit five bytes, `cmp eax, 5` / `jz 6FB27224` at 0x6FB27214, that nothing reaches:
//  the remains of `if (dwUnitType != UNIT_TILE) UNITGFX_UnitInitGfx(...)`, whose test the compiler made dead by sending the UNIT_TILE arm
//  straight to the registration and every other arm straight to the InitGfx call. The `goto register_it` below computes exactly that.
D2UnitStrc *__fastcall CUNIT_InitUnit(D2UnitStrc *pUnit, uint32_t dwUnitId, int32_t nClassId, int32_t nPosX, int32_t nPosY, D2C_UnitTypes dwUnitType, int32_t nItemMode, uint8_t nObjectMode)
{
    D2ActiveRoomStrc *pRoom;
    uint64_t nSeed;
    D2UnitStrc *pRegistered;

    if (pUnit)
    {
        if (GAME_IsExpansionGame())
            pUnit->dwFlagEx |= 0x2000000;
        else
            pUnit->dwFlagEx &= 0xFDFFFFFF;
    }

    //  the guard above ends here; everything below runs on a NULL pUnit too
    pRoom = NULL;
    pUnit->dwUnitId = dwUnitId;
    pUnit->dwUnitType = dwUnitType;
    pUnit->dwClassId = nClassId;
    pUnit->dwTickCount = 0;
    pUnit->pMemoryPool = NULL;

    if (nPosX || nPosY)
    {
        pRoom = CUNIT_FindRoomForCoords(nPosX, nPosY);
        pUnit->nAct = (uint8_t)D2COMMON_DRLG_GetActNoFromLevelId((D2C_Levels)D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom));
        pUnit->pDrlgAct = gpAct;
    }

    D2COMMON_SEED_InitSeed(&pUnit->pSeed);

    if (dwUnitType == UNIT_ITEM)
    {
        D2COMMON_ITEMS_AllocItemData(NULL, pUnit);
        D2COMMON_ITEMS_InitItemSeed(pUnit);
    }

    if (pRoom)
    {
        nSeed = (uint64_t)1791398085 * pRoom->pSeed.nLowSeed + pRoom->pSeed.nHighSeed;
        pRoom->pSeed.nLowSeed = (uint32_t)nSeed;
        pRoom->pSeed.nHighSeed = (uint32_t)(nSeed >> 32);
        pUnit->dwInitSeed = (uint32_t)nSeed;
        D2COMMON_SEED_InitLowSeed(&pUnit->pSeed, (int32_t)(uint32_t)nSeed);
    }

    switch (dwUnitType)
    {
    case UNIT_PLAYER:
        PLAYER_InitClientUnit(pUnit, pRoom, nPosX, nPosY);
        break;

    case UNIT_MONSTER:
        FOG_DisplayAssert((uint32_t)"eTOU != UNIT_MONSTER", (uint32_t)szCUnitCpp, 512);
        exit(-1);

    case UNIT_OBJECT:
        D2COMMON_UNITS_AllocStaticPath(pUnit);
        OBJECT_InitClientUnit(pUnit, pRoom, nPosX, nPosY, (D2Objects)nClassId, nObjectMode);
        break;

    case UNIT_MISSILE:
        MISSILE_InitClientUnit(pUnit, pRoom, nPosX, nPosY, nClassId);
        break;

    case UNIT_ITEM:
        D2COMMON_UNITS_AllocStaticPath(pUnit);
        ITEM_SetClientItemMode(pUnit, pRoom, nPosX, nPosY, nItemMode, 1);
        break;

    case UNIT_TILE:
        D2COMMON_UNITS_AllocStaticPath(pUnit);
        pUnit->dwFlags |= 0x22u;
        D2COMMON_UNITS_InitializeStaticPath(pUnit, pRoom, nPosX, nPosY);
        D2COMMON_UNITS_BlockCollisionPath(pUnit, pRoom, nPosX, nPosY);
        D2COMMON_UNITROOM_AddUnitToRoom(pUnit, pRoom);
        goto register_it;

    default:
        break;
    }

    UNITGFX_UnitInitGfx(pUnit);

    //  the tail chunk's pair of equality tests, not an unsigned <=
    if (dwUnitType == UNIT_PLAYER || dwUnitType == UNIT_MONSTER)
        D2COMMON_UNITS_UpdateAnimRateAndVelocity(pUnit, szCUnitCpp, 539);

register_it:
    pRegistered = CUNIT_RegisterUnit(pUnit);
    if (pRegistered && dwUnitType == UNIT_OBJECT)
        OBJECT_AddIdleOverlay(pRegistered);

    return pRegistered;
}


//1.10f: D2Client.0x6FB27280
//  Puts a unit in its table and evicts whatever is already there under its id. The retry is the whole function:
//
//      add -> success                         -> done
//      add -> refused (the id is taken)       -> find the squatter, unlink it, add again
//      still refused                          -> destroy the new unit, NULL
//
//  If the unit already in the table is this unit, the assert at CUnit.cpp:452 fires and the process exits.
//  The squatter's own flag decides which table it is unlinked from, not the new unit's: `shr ecx, 15h` on the found unit's dwFlags at
//  0x6FB272D5. The lookup for the squatter uses the by-id table unconditionally, even though the unit being added may belong in the
//  client-id one; that is the original's, and it is why the second add can still fail.
D2UnitStrc *__fastcall CUNIT_RegisterUnit(D2UnitStrc *pUnit)
{
    D2UnitStrc *pExisting;
    D2UnitStrc **ppTable;
    uint32_t dwUnitId;
    D2C_UnitTypes nUnitType;

    //  DIFFERENCE: a unit of type 6 or more -- made from packet 0x09 or 0x51 with a type no handler bounds, CUnit.cpp's creators passing it to
    //  D2Common's UNITS_AllocUnit, which takes any -- is refused as a second failed add is: destroyed, NULL. The original files it in a row
    //  past its six; ours has no such row. This guard covers CUNIT_AddUnitToTable and CUNIT_FindUnitInTable, whose only callers are here and there.
    if (pUnit && (uint32_t)pUnit->dwUnitType >= (uint32_t)UNIT_TYPES_COUNT)
    {
        CUNIT_DestroyUnit(pUnit);
        return NULL;
    }

    if (CUNIT_AddUnitToTable(pUnit))
        return pUnit;

    pExisting = CUNIT_FindUnitInTable(gpUnitTableById[pUnit->dwUnitType], pUnit->dwUnitId & 0x7F, pUnit->dwUnitId, (D2C_UnitTypes)pUnit->dwUnitType);

    if (pExisting == pUnit)
    {
        FOG_DisplayAssert((uint32_t)"hToRemove != (HUNIT) ptUnit", (uint32_t)szCUnitCpp, 452);
        exit(-1);
    }

    if (pExisting)
    {
        dwUnitId = pExisting->dwUnitId;
        nUnitType = (D2C_UnitTypes)pExisting->dwUnitType;

        if (((pExisting->dwFlags >> 21) & 1) == 0)
            ppTable = gpUnitTableById[nUnitType];
        else
            ppTable = gpUnitTableByClientId[nUnitType];

        CUNIT_UnlinkAndFreeUnit(ppTable, dwUnitId & 0x7F, dwUnitId, nUnitType);

        if (CUNIT_AddUnitToTable(pUnit))
            return pUnit;
    }

    CUNIT_DestroyUnit(pUnit);
    return NULL;
}


//1.10f: D2Client.0x6FB27320
//  The walk, parameterised: the same body as CUNIT_FindUnitByGuidAndType and its twin (the same FOG_Trace, the same assert at CUnit.cpp:88),
//  but taking the table row and the bucket rather than computing them.
D2UnitStrc *__fastcall CUNIT_FindUnitInTable(D2UnitStrc **pTable, int32_t nBucket, int32_t nUnitId, D2C_UnitTypes nUnitType)
{
    D2UnitStrc *ptUnit;

    ptUnit = pTable[nBucket];

    while (ptUnit)
    {
        if (ptUnit->dwUnitId == (uint32_t)nUnitId)
        {
            if (ptUnit->dwUnitType == (uint32_t)nUnitType)
                return ptUnit;

            FOG_Trace("current unit is %d, table is supposed to be %d\n", ptUnit->dwUnitType, nUnitType);

            if (ptUnit->dwUnitType != (uint32_t)nUnitType)
            {
                FOG_DisplayAssert((uint32_t)"ptUnit->eType == eTOU", (uint32_t)szCUnitCpp, 88);
                exit(-1);
            }
        }

        ptUnit = ptUnit->pListNext;
    }

    return NULL;
}


//1.10f: D2Client.0x6FB27390
//  Inserts a unit if its id is free. Returns 1 on success and 0 when the bucket already holds that id, which is the signal CUNIT_RegisterUnit
//  acts on. The two tables are named here, by the asserts:
//
//      6FB27392 path   assert "!UnitGetFromClientId(ptUnit->dwId, ptUnit->eType)"  :154
//      6FB273A0 path   assert "!UnitGetFromId(ptUnit->dwId, ptUnit->eType)" :163
//
//  Each path looks the unit up twice, and the second lookup is the assert's own condition, the same D2_ASSERT shape as the type test inside
//  the walk: the first answers "is the id taken" and returns 0, the second can only fire if the table changed between them, which nothing
//  here does. Both are written out. On the bit-21 path the lookups are calls to CUNIT_FindUnitInTable; on the by-id path the original
//  inlines CUNIT_FindUnitByGuidAndType twice, each loading the type and the id from the unit itself (0x6FB2740C, 0x6FB27452).
//  The bucket is kept sorted by descending dwUnitId, and the comparison is unsigned (`jbe`, 0x6FB27537): the new unit goes in front of the
//  first entry whose id is not greater than its own.
int32_t __fastcall CUNIT_AddUnitToTable(D2UnitStrc *pUnit)
{
    D2UnitStrc **ppBuckets;
    D2UnitStrc *pAfter;
    D2UnitStrc *pBefore;
    uint32_t nBucket;
    D2C_UnitTypes nUnitType;

    if (pUnit && ((pUnit->dwFlags >> 21) & 1) == 1)
    {
        if (CUNIT_FindUnitInTable(gpUnitTableByClientId[pUnit->dwUnitType], pUnit->dwUnitId & 0x7F, pUnit->dwUnitId, (D2C_UnitTypes)pUnit->dwUnitType))
        {
            return 0;
        }

        if (CUNIT_FindUnitInTable(gpUnitTableByClientId[pUnit->dwUnitType], pUnit->dwUnitId & 0x7F, pUnit->dwUnitId, (D2C_UnitTypes)pUnit->dwUnitType))
        {
            FOG_DisplayAssert((uint32_t)"!UnitGetFromClientId(ptUnit->dwId, ptUnit->eType)", (uint32_t)szCUnitCpp, 154);
            exit(-1);
        }
    }
    else
    {
        if (CUNIT_FindUnitByGuidAndType(pUnit->dwUnitId, pUnit->dwUnitType))
        {
            return 0;
        }

        if (CUNIT_FindUnitByGuidAndType(pUnit->dwUnitId, pUnit->dwUnitType))
        {
            FOG_DisplayAssert((uint32_t)"!UnitGetFromId(ptUnit->dwId, ptUnit->eType)", (uint32_t)szCUnitCpp, 163);
            exit(-1);
        }
    }

    UNITSND_AdjustUnitSoundRefCounts(pUnit, 1);

    nBucket = pUnit->dwUnitId & 0x7F;
    nUnitType = (D2C_UnitTypes)pUnit->dwUnitType;

    if (((pUnit->dwFlags >> 21) & 1) == 1)
        ppBuckets = gpUnitTableByClientId[nUnitType];
    else
        ppBuckets = gpUnitTableById[nUnitType];

    pBefore = NULL;
    for (pAfter = ppBuckets[nBucket]; pAfter; pAfter = pAfter->pListNext)
    {
        if (pAfter->dwUnitId <= pUnit->dwUnitId)
            break;
        pBefore = pAfter;
    }

    pUnit->pListNext = pAfter;
    if (pBefore)
        pBefore->pListNext = pUnit;
    else
        ppBuckets[nBucket] = pUnit;

    return 1;
}


//1.10f: D2Client.0x6FB27570
//  The player form of CUNIT_CreateUnit: allocate, init as UNIT_PLAYER with nItemMode and nObjectMode both 0, then hang the name on the
//  player data. The two NULL tests are separate: a failed allocation returns NULL without calling anything, and a failed init returns NULL
//  without setting a name.
D2UnitStrc *__fastcall CUNIT_CreatePlayerUnit(uint32_t dwPlayerId, D2C_PlayerClasses nCharClass, int32_t nX, int32_t nY, char *szName)
{
    D2UnitStrc *pAllocated;
    D2UnitStrc *pUnit;

    pAllocated = D2COMMON_UNITS_AllocUnit(NULL, UNIT_PLAYER);
    if (!pAllocated)
        return pAllocated;

    pUnit = CUNIT_InitUnit(pAllocated, dwPlayerId, nCharClass, nX, nY, UNIT_PLAYER, 0, 0);
    if (pUnit)
        D2COMMON_UNITS_SetNameInPlayerData(pUnit, szName);

    return pUnit;
}


//1.10f: D2Client.0x6FB275C0
//  The object form: allocate, init, then store the interact type and, for a shrine, run OBJECT_InitShrine.
//  The store is into pObjectData: +0x14 is the per-type data pointer, and +4 of D2ObjectDataStrc is InteractType, a uint8 -- the field the
//  parameter is named after (`mov eax, [esi+14h]` / `mov [eax+4], cl`, 0x6FB27602..0x6FB2760A).
D2UnitStrc *__fastcall CUNIT_CreateObjectUnit(uint32_t dwUnitId, int32_t nObjectId, int32_t nPosX, int32_t nPosY, D2C_UnitTypes dwUnitType, int32_t nItemMode, uint8_t nObjectMode, uint8_t nInteractType)
{
    D2UnitStrc *pAllocated;
    D2UnitStrc *pUnit;

    pAllocated = D2COMMON_UNITS_AllocUnit(NULL, dwUnitType);
    if (!pAllocated)
        return pAllocated;

    pUnit = CUNIT_InitUnit(pAllocated, dwUnitId, nObjectId, nPosX, nPosY, dwUnitType, nItemMode, nObjectMode);
    if (pUnit)
    {
        pUnit->pObjectData->InteractType = nInteractType;
        if (D2COMMON_UNITS_IsShrine(pUnit))
            OBJECT_InitShrine(pUnit);
    }

    return pUnit;
}


//  UNITS_AllocUnit never returns NULL, and several notes in this file rest on that. D2Common 0x6FDBD6B0 puts FOG_AllocPool's answer
//  straight into `rep stosd` (0x6FDBD6DA) and then writes `[esi+8]` and `[esi]` with no test between, so a NULL from the allocator faults
//  inside D2Common before AllocUnit could return it. For a UNIT_MONSTER it allocates the 0x60-byte monster data in the same breath
//  (0x6FDBD6FB) and stores it at +0x14. So every NULL test on AllocUnit's answer in this file is dead, and so is every NULL test on a
//  monster's pMonsterData made straight after the allocation.
//
//  The literals the C spells as names; each is the original's immediate.
static_assert((UNITFLAG_ISASYNC | UNITFLAG_ISCLIENTUNIT) == 0x600000,
              "`or dword ptr [esi+0C4h], 600000h` at 0x6FB27773 and "
              "`or edx, 600000h` at 0x6FB27DE1");
static_assert(MONSTATS2FLAGINDEX_NOUNIQUESHIFT == 15,
              "the unique palette-shift test reads nFlags[15 >> 3] "
              "(`mov dl, [eax+5]`, 0x6FB27A86) and gdwBitMasks[15 & 7] "
              "(`and edx, [eax+1Ch]`, 0x6FB27A8E)");
static_assert(MONMODE_NEUTRAL == 1,
              "a client-only monster's assign block gets anim mode 1: "
              "`mov [esp+38h+var_4], 1` at 0x6FB27DAC");
static_assert(UNIT_ALIGNMENT_GOOD == 2,
              "CUNIT_OnUnitAlignmentChanged: `cmp esi, 2` at 0x6FB27E3E and "
              "`cmp ebx, 2` at 0x6FB27E55");

//  The offsets CUNIT_CreateMonsterUnit reads, at the instruction named.
static_assert(offsetof(D2MonStatsTxt, wNameStr) == 0x06,
              "`mov cx, [eax+6]` at 0x6FB2782A");
static_assert(offsetof(D2MonStatsTxt, wMonStatsEx) == 0x18,
              "`movsx eax, word ptr [ecx+18h]` at 0x6FB276CF");
static_assert(offsetof(D2MonStatsTxt, nTransLvl) == 0x4D,
              "`mov al, [ecx+4Dh]` at 0x6FB27A28, ecx the MONSTATS record");
static_assert(offsetof(D2MonStats2Txt, nFlags) == 0x04,
              "the NoUniqueShift byte is +5, nFlags[1]: 0x6FB27A86");
static_assert(offsetof(D2MonStats2Txt, nMeleeRng) == 0x0E,
              "`mov al, [edx+0Eh]` at 0x6FB278B8");
static_assert(offsetof(D2MonStats2Txt, nUtrans) == 0x122,
              "`mov cl, [eax+ebx+122h]` at 0x6FB27AEF");
static_assert(offsetof(D2SuperUniquesTxt, nUtrans) == 0x28,
              "`mov cl, [eax+ebx+28h]` at 0x6FB27B51");
static_assert(offsetof(D2MonsterDataStrc, nComponent) == 0x04
              && sizeof(((D2MonsterDataStrc *)0)->nComponent) == 16,
              "`add eax, 4` at 0x6FB27878, then four dword moves "
              "0x6FB2787F..0x6FB27892");
//  The two halves of the hireling hand-over: what the roster pet holds, and what MONSTER_InitHireling reads -- the same two values in
//  opposite order.
static_assert(offsetof(D2ClientPetInfoStrc, nSeed) == 0
              && offsetof(D2ClientPetInfoStrc, wName) == 4,
              "`mov edx, [eax]` at 0x6FB27BAB, `mov ax, [eax+4]` at 0x6FB27BB1");
static_assert(offsetof(D2ClientHirelingInitStrc, wNameId) == 0
              && offsetof(D2ClientHirelingInitStrc, nLowSeed) == 4,
              "MONSTER_InitHireling: `mov ax, [edi]` at 0x6FB446E5, "
              "`mov edx, [edi+4]` at 0x6FB446FE");
//  The preset walk: the DS1 spawn-flag word (nSpawnFlags) and the chain it steps.
static_assert(offsetof(D2PresetUnitStrc, nSpawnFlags) == 0x14
              && offsetof(D2PresetUnitStrc, pNext) == 0x1C,
              "`test byte ptr [esi+14h], 1` at 0x6FB27E83, "
              "`mov esi, [esi+1Ch]` at 0x6FB27EB4");

//  GFX_AddOverlay's kind (D2C_OverlayTypes): the original pushes each as a literal.
static_assert(OVERLAYTYPE_COUNTER == 1 && OVERLAYTYPE_ONCE == 2
              && OVERLAYTYPE_LOOP == 3 && OVERLAYTYPE_LOOPEX == 4,
              "CUNIT_AddOverlay: `push 1` (0x6FB29D2B); CUNIT_CreateMonsterUnit: "
              "`push 2` (0x6FB2790E), `push 3` (0x6FB27A16), `push 4` (0x6FB279AB)");
static_assert(OVERLAY_MEPHISTO == 122 && OVERLAY_LIGHT_CAST_2 == 186
                  && OVERLAY_SHADOWWARRIORAPPEAR == 270 && OVERLAY_BAAL_ON_THRONE == 275,
              "CUNIT_CreateMonsterUnit: `mov edx, 7Ah / 0BAh / 10Eh / 113h` "
              "(0x6FB278FA, 0x6FB27910, 0x6FB279F7, 0x6FB27A07), one call at 0x6FB27A1A");


//1.10f: D2Client.0x6FB27630
//  A monster, built from an assign block. Callers: SCMD_HandleSrvAC_SpawnMonster, which parses the server's 0xAC packet into the block, and
//  CUNIT_CreateClientOnlyUnit, which builds one for a monster the server never sees.
//
//  The assign block is seven dwords:
//
//      [0]  the unit id
//      [1]  the monstats class id
//      [2]  x
//      [3]  y
//      [4]  the type flags, low word only (`mov dx, ...`, 0x6FB277F4)
//      [5]  the dword the monster data keeps at +0x3C
//      [6]  the anim mode
//
//  The id and the class are read twice from the block, once for the tests at the head and again for the stores into the unit (0x6FB2778C,
//  0x6FB2779A); the stores are written from the block, as the bytes have them.
//
//  An existing hireling is refreshed, not recreated: when the id is the player's associated unit of kind 7 (pettype.txt row 7, `hireable`) and
//  a monster with that id is already in the table, its graphics are rebuilt, its mode set from [6], and it is returned. The original inlines
//  CUNIT_FindUnitByGuidAndType for that lookup (row 1 folded into the address, `gpUnitTableById+200h`, 0x6FB27659) and tests its answer for NULL
//  once more (`test esi, esi`, 0x6FB2771F), which a found unit cannot fail; written here as that `if` on the call's answer.
//
//  The head is DATATBLS_GetMonStats2TxtRecord and DATATBLS_GetMonStatsTxtRecord, which the original inlines together with the MONSTATS half
//  computed once and shared: the sign test, the count at +0xA80, the 424-byte stride, the signed wMonStatsEx, the count at +0xA98, the 308-byte
//  stride, followed by the caller's two NULL tests, the MonStats2 record's first (0x6FB276F1) and the MonStats record's second (0x6FB2774A).
//  The palette code later looks the same MonStats2 record up twice more (0x6FB27A43, 0x6FB27AA4), from the table base again and with the
//  424-byte product the head saved; those two copies have no sign test, because the head already returned on a negative class id.
//
//  BUG: the NULL test of the allocation (`cmp esi, ebx / jz`, 0x6FB2776F) guards only the async-flag write; `mov dword ptr [esi], 1`
//  (0x6FB27783) is unconditional. Reproduced. UNREACHABLE: UNITS_AllocUnit cannot return NULL (CUNIT_InitUnit has the same shape).
//
//  The room's seed advances exactly as in CUNIT_InitUnit (`mul` at 0x6FB277C3, `adc` at 0x6FB277D9), but here SEED_InitLowSeed runs
//  unconditionally, from the unit's dwInitSeed (`mov edx, [esi+28h]`, 0x6FB277E1, is the jump target of the no-room path) -- 0 when no room was
//  found, because AllocUnit's `rep stosd` cleared it. In CUNIT_InitUnit the same call is inside the room arm.
//
//  The component copy: sixteen composite choices, copied as four dword moves after an accessor that has no out-of-line copy in this image: type
//  is monster, monster data present, then the address of the array tested for zero (`add eax, 4` / `test eax, eax`, 0x6FB27878). The umod copy
//  beside it takes MONSTER_GetMonUmodArray's answer with no test at all (`mov [eax], dl`, 0x6FB2785D); for a monster that array is always there,
//  so this is a missing test and not a reachable fault.
//
//  The class switch, by monstats.txt hcIdx (row 410 is the Expansion marker, so a row number would be one off from 411 on) and overlay.txt:
//
//      242  mephisto          overlay 122 mephisto, kind 3
//      333  diabloclone       overlay 186 light_cast_2, kind 2
//      340..343  boneprison1..4    particles at one corner each
//      351..353  hydra1..3    overlay hydra_start1..3 (172..174), then hydra_loop1..3 (175..177), kind 4
//      406  izualghost        particles, raised 55 and drifting up
//      417, 418  shadowwarrior, shadowmaster   overlay 270 shadowwarriorappear
//      521  tyrael3           QUESTSFX_OnTyrael3Created
//      543  baalthrone        overlay 275 baal_on_throne, kind 3
//
//  MSVC split the sparse switch in two tables around 343 (`cmp ebx, 157h`, 0x6FB278C9); one switch is written. The hydra arm reads the class
//  from the unit (`mov edx, [esi+4]`, 0x6FB27996), not from the block's copy the switch tests; the C does the same.
//
//  The palette shift, in five steps:
//
//      2, or nTransLvl + 2 when monstats.txt's nTransLvl is under 8 -- an unsigned byte compare, `jnb` at 0x6FB27A2D
//      for a unique: a roll, unless monstats2.txt NoUniqueShift is set
//      monstats2.txt nUtrans for the difficulty, when non-zero; 255 means "1 if the pet's owner is not hostile, else 0" (`neg`/`sbb`/`inc`,
//        0x6FB27B20)
//      superuniques.txt nUtrans for the difficulty, when non-zero
//      and anything of 30 or more becomes 2 -- signed, `jl` at 0x6FB27B6E
//
//  The roll is 9 + r % 30, so 9..38, and the last step sends 30..38 back to 2: PALSHIFT_RollUniqueShift re-seeds the unit's own seed from
//  dwClassId + the name seed, steps it once and returns `edx % 30 + 9` (`div ecx` with ecx = 1Eh, then `lea eax, [edx+9]`); it takes the unit
//  (it reads [ecx+20h] and [ecx+4], pSeed and dwClassId). GAME_GetDifficulty is called twice per table (0x6FB27AE5 and 0x6FB27AFA, 0x6FB27B47
//  and 0x6FB27B59), and twice is what the C does.
//
//  The hireling's two values change places: the roster pet holds `{nSeed at +0, wName at +4}` (D2ClientPetInfoStrc, reached through
//  ROSTERPETS_GetPetInfo, which hands out the address of the pet's +0x24) and MONSTER_InitHireling reads `{name at +0, seed at +4}`, handing
//  them to D2Common's MONSTERS_GetActFromHirelingTxt as nNameId and MONSTERS_HirelingInit as nLowSeed. ROSTERPETS_OnPetPacket81 builds the
//  same block the same way. The block is a D2ClientHirelingInitStrc.
D2UnitStrc *__fastcall CUNIT_CreateMonsterUnit(int32_t *pAssign, uint8_t *pComponents, uint8_t *pMonUmod, uint16_t wNameSeed, int32_t nBossHcIdx, int32_t bAsyncClientUnit)
{
    uint32_t dwUnitId;
    D2UnitStrc *pExisting;
    int32_t nMonClassId;
    int32_t nMonStatsEx;
    D2MonStatsTxt *pMonStatsTxt;
    D2MonStats2Txt *pMonStats2Txt;
    D2MonStats2Txt *pMonStats2Again;
    D2UnitStrc *pMonster;
    D2UnitStrc *pRegistered;
    D2ActiveRoomStrc *pRoom;
    uint64_t nSeed;
    uint8_t *pMonUmodDst;
    uint8_t *pComponentDst;
    D2SuperUniquesTxt *pSuperUniquesTxt;
    D2ClientPetInfoStrc *pPetInfo;
    D2ClientHirelingInitStrc tHirelingInfo;
    int32_t nX;
    int32_t nY;
    int32_t nShift;
    int32_t i;

    dwUnitId = (uint32_t)pAssign[0];

    if (dwUnitId == (uint32_t)ROSTERPETS_GetAssociatedUnitGUID(gpPlayerUnit, 7, 1))
    {
        pExisting = CUNIT_FindUnitByGuidAndType(dwUnitId, UNIT_MONSTER);

        //  the caller's test of the inlined lookup's answer, 0x6FB2771F
        if (pExisting)
        {
            GFX_FreeUnitGfxInfo(pExisting);
            UNITGFX_UnitInitGfx(pExisting);
            D2COMMON_UNITS_ChangeAnimMode(pExisting, pAssign[6]);
            return pExisting;
        }
    }

    nMonClassId = pAssign[1];

    if (nMonClassId < 0)
        return NULL;
    if (nMonClassId >= sgptDataTables->nMonStatsTxtRecordCount)
        return NULL;

    pMonStatsTxt = &sgptDataTables->pMonStatsTxt[nMonClassId];
    nMonStatsEx = (int16_t)pMonStatsTxt->wMonStatsEx;
    if (nMonStatsEx < 0)
        return NULL;
    if (nMonStatsEx >= sgptDataTables->nMonStats2TxtRecordCount)
        return NULL;

    pMonStats2Txt = &sgptDataTables->pMonStats2Txt[nMonStatsEx];
    if (!pMonStats2Txt)
        return NULL;
    if (!pMonStatsTxt)
        return NULL;

    pMonster = D2COMMON_UNITS_AllocUnit(NULL, UNIT_MONSTER);
    if (bAsyncClientUnit && pMonster)
        pMonster->dwFlags |= UNITFLAG_ISASYNC | UNITFLAG_ISCLIENTUNIT;

    //  the test above guards the flag write and nothing after it
    nX = pAssign[2];
    nY = pAssign[3];
    pMonster->dwUnitType = UNIT_MONSTER;
    pMonster->dwClassId = pAssign[1];
    pMonster->dwUnitId = pAssign[0];
    pMonster->pMemoryPool = NULL;
    pMonster->dwTickCount = 0;

    pRoom = CUNIT_FindRoomForCoords(nX, nY);
    D2COMMON_SEED_InitSeed(&pMonster->pSeed);

    if (pRoom)
    {
        nSeed = (uint64_t)1791398085 * pRoom->pSeed.nLowSeed + pRoom->pSeed.nHighSeed;
        pRoom->pSeed.nLowSeed = (uint32_t)nSeed;
        pRoom->pSeed.nHighSeed = (uint32_t)(nSeed >> 32);
        pMonster->dwInitSeed = (uint32_t)nSeed;
    }

    //  unconditional here; CUNIT_InitUnit makes it part of the room arm
    D2COMMON_SEED_InitLowSeed(&pMonster->pSeed, (int32_t)pMonster->dwInitSeed);

    MONSTER_BindMonStatsRecord(pMonster, 0);
    //  the low word of the flags, `mov dx, word ptr [edi+10h]` (0x6FB277F4)
    MONSTER_SetTypeFlag(pMonster, (D2C_MonTypeFlags)pAssign[4], 1);

    if (MONSTER_TestTypeFlag(pMonster, MONTYPEFLAG_SUPERUNIQUE))
        MONSTER_SetBossHcIdx(pMonster, (uint16_t)nBossHcIdx);

    MONSTER_SetHirelingOwnerGUID(pMonster, pAssign[5]);
    D2COMMON_MONSTERS_SetMonsterNameInMonsterData(pMonster, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pMonStatsTxt->wNameStr));

    if (pMonUmod)
    {
        MONSTER_SetNameSeed(pMonster, wNameSeed);

        //  the array is used untested, 0x6FB2785D
        pMonUmodDst = MONSTER_GetMonUmodArray(pMonster);
        for (i = 0; i < 9; ++i)
            pMonUmodDst[i] = pMonUmod[i];
    }

    if (pComponents)
    {
        //  an inlined accessor with no out-of-line copy
        pComponentDst = NULL;
        if (pMonster->dwUnitType == UNIT_MONSTER && pMonster->pMonsterData)
            pComponentDst = pMonster->pMonsterData->nComponent;

        if (pComponentDst)
            memcpy(pComponentDst, pComponents, 16);
    }

    MONSTER_InitClientUnit(pMonster, pRoom, nX, nY, (D2C_MonsterIds)nMonClassId, pAssign[6]);
    D2COMMON_PATH_SetStepNum(pMonster->pDynamicPath, pMonStats2Txt->nMeleeRng);
    UNITGFX_UnitInitGfx(pMonster);

    switch (nMonClassId)
    {
    case 242:   //  mephisto
        GFX_AddOverlay(pMonster, OVERLAY_MEPHISTO, OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
        break;

    case 333:   //  diabloclone
        GFX_AddOverlay(pMonster, OVERLAY_LIGHT_CAST_2, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);
        break;

    case 340:   //  boneprison1
        PARTICLE_AllocUnitParticles(pMonster);
        PARTICLE_SetUnitGfxCoords(pMonster, -32, -32, 0);
        break;

    case 341:   //  boneprison2
        PARTICLE_AllocUnitParticles(pMonster);
        PARTICLE_SetUnitGfxCoords(pMonster, -32, 32, 0);
        break;

    case 342:   //  boneprison3
        PARTICLE_AllocUnitParticles(pMonster);
        PARTICLE_SetUnitGfxCoords(pMonster, 32, 32, 0);
        break;

    case 343:   //  boneprison4
        PARTICLE_AllocUnitParticles(pMonster);
        PARTICLE_SetUnitGfxCoords(pMonster, 32, -32, 0);
        break;

    case 351:   //  hydra1..3: hydra_start1..3, then hydra_loop1..3
    case 352:
    case 353:
        GFX_AddOverlay(pMonster, (D2_OverlayIds)(pMonster->dwClassId - 179), OVERLAYTYPE_LOOPEX, 0, pMonster->dwClassId - 179 + 3, 0, 0, 0);
        break;

    case 406:   //  izualghost
        PARTICLE_AllocUnitParticles(pMonster);
        PARTICLE_SetUnitGfxCoords(pMonster, 0, 0, -55);
        PARTICLE_SetUnitGfxVector(pMonster, 0, 0, 1, 1);
        PARTICLE_SetUnitParticleVector(pMonster, 0, 0, 50, 0);
        PARTICLE_SetUnitRisesToTarget(pMonster, 1);
        break;

    case 417:   //  shadowwarrior, shadowmaster
    case 418:
        GFX_AddOverlay(pMonster, OVERLAY_SHADOWWARRIORAPPEAR, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);
        break;

    case 521:   //  tyrael3
        QUESTSFX_OnTyrael3Created(pMonster);
        break;

    case 543:   //  baalthrone
        GFX_AddOverlay(pMonster, OVERLAY_BAAL_ON_THRONE, OVERLAYTYPE_LOOP, 0, 0, 0, 0, 0);
        break;

    default:
        break;
    }

    nShift = 2;
    if (pMonStatsTxt->nTransLvl < 8)
        nShift = pMonStatsTxt->nTransLvl + 2;

    if (MONSTER_IsUnique(pMonster))
    {
        pMonStats2Again = NULL;
        if (nMonClassId < sgptDataTables->nMonStatsTxtRecordCount)
        {
            nMonStatsEx = (int16_t)sgptDataTables->pMonStatsTxt[nMonClassId].wMonStatsEx;
            if (nMonStatsEx >= 0 && nMonStatsEx < sgptDataTables->nMonStats2TxtRecordCount)
            {
                pMonStats2Again = &sgptDataTables->pMonStats2Txt[nMonStatsEx];
            }
        }

        if (!pMonStats2Again || (pMonStats2Again->nFlags[MONSTATS2FLAGINDEX_NOUNIQUESHIFT >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_NOUNIQUESHIFT & 7]) == 0)
        {
            nShift = (int32_t)PALSHIFT_RollUniqueShift(pMonster, wNameSeed);
        }
    }

    if (nMonClassId < sgptDataTables->nMonStatsTxtRecordCount)
    {
        nMonStatsEx = (int16_t)sgptDataTables->pMonStatsTxt[nMonClassId].wMonStatsEx;
        if (nMonStatsEx >= 0 && nMonStatsEx < sgptDataTables->nMonStats2TxtRecordCount)
        {
            pMonStats2Again = &sgptDataTables->pMonStats2Txt[nMonStatsEx];
            if (pMonStats2Again && pMonStats2Again->nUtrans[(uint8_t) GAME_GetDifficulty()])
            {
                nShift = pMonStats2Again->nUtrans[(uint8_t) GAME_GetDifficulty()];
                if (nShift == 255)
                    nShift = ROSTERPETS_IsPetOwnerHostile(pMonster) == 0;
            }
        }
    }

    if (MONSTER_TestTypeFlag(pMonster, MONTYPEFLAG_SUPERUNIQUE))
    {
        pSuperUniquesTxt = D2COMMON_DATATBLS_GetSuperUniquesTxtRecord(MONSTER_GetBossHcIdx(pMonster));

        //  BUG: DATATBLS_GetSuperUniquesTxtRecord answers NULL for an id out of range, and the record is indexed with no NULL test (0x6FB27B51).
        //  The id is the one the server sent (nBossHcIdx, stored only when type flag 2 says superunique). LATENT: packet 0xAC reads the id with
        //  no bound and CUNIT_CreateClientOnlyUnit passes 0 with no flags, so only a server whose superuniques.txt differs from the client's, or a
        //  malformed packet, reaches the NULL.
        if (pSuperUniquesTxt->nUtrans[(uint8_t) GAME_GetDifficulty()])
        {
            nShift = pSuperUniquesTxt->nUtrans[(uint8_t) GAME_GetDifficulty()];
        }
    }

    if (nShift >= 30)
        nShift = 2;

    GFX_SetPaletteShift1(pMonster->pGfxData, nShift);
    GFX_SetPaletteShift2(pMonster->pGfxData, nShift);

    pRegistered = CUNIT_RegisterUnit(pMonster);

    //  BUG: pRegistered is used untested (`mov ecx, [esi+0Ch]`, 0x6FB27B92), and from here on the allocated unit is used and returned (`mov esi,
    //  [esp+3Ch+pMonster]`, 0x6FB27BAD and 0x6FB27BCB). CUNIT_RegisterUnit answers NULL when it gives up, and destroys the new unit before it
    //  does, so a failed registration faults reading address 0x0C, and were it not to, every call after it would be made on freed memory.
    //  LATENT: the 0xAC path files by id, where a squatter is always evicted, and a client-id collision needs the pre-incremented
    //  gdwNextClientUnitId to wrap past some 4.29 billion ids while the first holder of the id still lives.
    if (ROSTERPETS_GetPetType(pRegistered->dwUnitId) == 7)
    {
        pPetInfo = ROSTERPETS_GetPetInfo(pRegistered->dwUnitId);
        if (pPetInfo)
        {
            tHirelingInfo.nLowSeed = pPetInfo->nSeed;
            tHirelingInfo.wNameId = pPetInfo->wName;
            MONSTER_InitHireling(pMonster, &tHirelingInfo);
        }
    }

    if (D2COMMON_MONSTERS_GetHirelingTypeId(pMonster))
        D2COMMON_INVENTORY_AllocInventory(NULL, pMonster);

    UNITSND_PlayMonsterInitSound(pMonster);
    return pMonster;
}


//1.10f: D2Client.0x6FB27D50
//  A unit the server never hears about, with an id the client hands out itself.
//  The id is pre-incremented and wraps before 0xFFFFFFFF: the ids run 1, 2, ... 0xFFFFFFFE, 0, 1, so -1 is never handed out and 0 is, once per
//  wrap. The bytes compare before they store and branch after it (0x6FB27D5F, 0x6FB27D64, 0x6FB27D6A), so the incremented value reaches the
//  global on both paths before the wrap zeroes it; the C stores and then tests, which computes the same.
//  A monster goes through CUNIT_CreateMonsterUnit, with an assign block cleared by `rep stosd` of 7 dwords (0x6FB27D8A) and filled with the id,
//  the class, x, y and anim mode MONMODE_NEUTRAL; no components, no umods, name seed 0, boss 0, and bAsyncClientUnit 1. Everything else is
//  allocated here, flagged UNITFLAG_ISASYNC | UNITFLAG_ISCLIENTUNIT -- the flag that files it in gpUnitTableByClientId -- and initialised.
//  The init re-reads the global for the id (`mov edx, gdwNextClientUnitId`, 0x6FB27DF6) where the monster path used the register; the two are
//  the same value. Written as the bytes have it. `retn 0Ch` is three stack arguments beside ecx and edx.
D2UnitStrc *__fastcall CUNIT_CreateClientOnlyUnit(int32_t nClassId, int32_t nPosX, int32_t nPosY, D2C_UnitTypes dwUnitType, int32_t nItemMode)
{
    uint32_t dwNewUnitId;
    int32_t tAssign[7];
    D2UnitStrc *pUnit;

    dwNewUnitId = gdwNextClientUnitId + 1;
    gdwNextClientUnitId = dwNewUnitId;
    if (dwNewUnitId == 0xFFFFFFFF)
    {
        dwNewUnitId = 0;
        gdwNextClientUnitId = 0;
    }

    if (dwUnitType == UNIT_MONSTER)
    {
        memset(tAssign, 0, sizeof(tAssign));
        tAssign[0] = (int32_t)dwNewUnitId;
        tAssign[1] = nClassId;
        tAssign[2] = nPosX;
        tAssign[3] = nPosY;
        tAssign[6] = MONMODE_NEUTRAL;
        return CUNIT_CreateMonsterUnit(tAssign, NULL, NULL, 0, 0, 1);
    }

    pUnit = D2COMMON_UNITS_AllocUnit(NULL, dwUnitType);
    if (!pUnit)
        return pUnit;

    pUnit->dwFlags |= UNITFLAG_ISASYNC | UNITFLAG_ISCLIENTUNIT;
    return CUNIT_InitUnit(pUnit, gdwNextClientUnitId, nClassId, nPosX, nPosY, dwUnitType, nItemMode, 0);
}


//1.10f: D2Client.0x6FB27E10
//  Keeps the room's ally count in step with a monster's alignment. Called from PLRSKILLS_SetStateStat when the stat it writes is STAT_ALIGNMENT;
//  the old value comes in edx and the new on the stack (`retn 4`).
//  Nothing happens unless the unit is a monster, the value changed and the unit is in a room. Then UNIT_ALIGNMENT_GOOD in raises the room's
//  count and GOOD out lowers it, and in every case that got this far D2Common #10214 runs on the unit (it recomputes the dynamic path's
//  collision pattern from the unit size, re-stamping the footprint when the unit is in a room -- an ally and an enemy do not collide alike).
//  Both byte arguments are widened with `and 0FFh` (0x6FB27E19, 0x6FB27E1F) before anything compares them. A unit is counted in the room it
//  stands in at the moment of the change, and nothing here moves the count with it.
void __fastcall CUNIT_OnUnitAlignmentChanged(D2UnitStrc *pUnit, uint8_t nOldAlign, uint8_t nNewAlign)
{
    D2ActiveRoomStrc *pRoom;

    if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER || nOldAlign == nNewAlign)
        return;

    pRoom = D2COMMON_UNITS_GetRoom(pUnit);
    if (!pRoom)
        return;

    if (nNewAlign == UNIT_ALIGNMENT_GOOD)
    {
        D2COMMON_DUNGEON_IncreaseAlliedCountOfRoom(pRoom);
        D2COMMON_10214(pUnit);
        return;
    }

    if (nOldAlign == UNIT_ALIGNMENT_GOOD)
        D2COMMON_DUNGEON_DecreaseAlliedCountOfRoom(pRoom);

    D2COMMON_10214(pUnit);
}


//1.10f: D2Client.0x6FB27E70
//  Spawns a room's client-side presets. void: both call sites overwrite eax with their next call without reading it.
//  Bit 0 of the preset's flag word (nSpawnFlags, ORed in from the DS1 file's per-unit flag word) means "the client spawns this one": the
//  server spawns exactly the presets without it, and the client takes the complement, here, as units the server will never mention. The test
//  reads one byte, `test byte ptr [esi+14h], 1`.
//  The room's coordinates are fetched again for every preset that passes, as the bytes do (0x6FB27E8F inside the loop).
void __fastcall CUNIT_CreatePresetUnitsInRoom(D2ActiveRoomStrc *pRoom)
{
    D2PresetUnitStrc *pPreset;
    D2DrlgCoordsStrc tCoords;

    for (pPreset = D2COMMON_DUNGEON_GetPresetUnitsFromRoom(pRoom); pPreset; pPreset = pPreset->pNext)
    {
        if (pPreset->nSpawnFlags & 1)
        {
            D2COMMON_DUNGEON_GetRoomCoordinates(pRoom, &tCoords);
            CUNIT_CreateClientOnlyUnit(pPreset->nIndex, tCoords.nSubtileX + pPreset->nXpos, pPreset->nYpos + tCoords.nSubtileY, (D2C_UnitTypes)pPreset->nUnitType, pPreset->nMode);
        }
    }
}


//1.10f: D2Client.0x6FB27ED0
//  Tears a client unit down. It never touches the hash tables -- the unlink happens before it.
//  The order: the unit's light, a second light handed back by D2Common, the particles, the sounds (stopped, then the reference counts
//  dropped), the per-type teardown, the timed events, the packet queue, the player pointer if this was the player, and the unit itself.
//  +0x64 is the client arm of the union, pLight: the D2ClientLightStrc that DLIGHTMAP_AddLight allocates and DLIGHTMAP_RemoveLight frees.
//  The second light is D2Common #10958(pUnit, NULL), SKILLS_SwapCastLight: it swaps the cast light parked in pUnit's skills with its second
//  argument and returns the old value, so this takes the light a skill parked on the unit and leaves NULL behind.
//  The switch has one merged tail: the item arm jumps into the tile arm's UNITS_FreeStaticPath (0x6FB27F87 -> 0x6FB27FD0); the compiler shared
//  the two identical statements, and each arm is written with its own. A tile leaves its room only while there is an act (`cmp gpAct, edi`,
//  0x6FB27FC2).
//  The end is the original's inline of CUNIT_SetPlayerUnit(NULL) (0x6FB27FED..0x6FB27FF6): `cmp eax, edi` / `jz` / `call UI_ResetUiSubsystems` /
//  `mov gpPlayerUnit, edi`. The load of gpPlayerUnit is shared with this function's own comparison (0x6FB27FE4), and the redundant test it
//  leaves -- a unit that equals the player is not NULL -- is the inlined function's.
void __fastcall CUNIT_DestroyUnit(D2UnitStrc *pUnit)
{
    D2ClientLightStrc *pSkillLight;

    if (pUnit->pLight)
    {
        DLIGHTMAP_RemoveLight(pUnit->pLight);
        pUnit->pLight = NULL;
    }

    pSkillLight = (D2ClientLightStrc *)D2COMMON_SKILLS_SwapCastLight(pUnit, NULL);
    if (pSkillLight)
        DLIGHTMAP_RemoveLight(pSkillLight);

    PARTICLE_FreeUnitParticles(pUnit);
    UNITSND_StopAllSoundsOnUnit(pUnit, 0);
    UNITSND_AdjustUnitSoundRefCounts(pUnit, -1);

    switch (pUnit->dwUnitType)
    {
    case UNIT_PLAYER:
        if (pUnit->pHoverText)
        {
            TEXT_FreeUnitHoverMsg(pUnit->pHoverText);
            pUnit->pHoverText = NULL;
        }
        PLAYER_FreeClientUnit(pUnit);
        UNITSND_StopAllSoundsOnUnit(pUnit, 1);
        break;

    case UNIT_MONSTER:
        if (pUnit->pHoverText)
        {
            TEXT_FreeUnitHoverMsg(pUnit->pHoverText);
            pUnit->pHoverText = NULL;
        }
        MONSTER_FreeUnitData(pUnit);
        UNITSND_StopAllSoundsOnUnit(pUnit, 1);
        break;

    case UNIT_OBJECT:
        if (pUnit->pHoverText)
        {
            TEXT_FreeUnitHoverMsg(pUnit->pHoverText);
            pUnit->pHoverText = NULL;
        }
        OBJECT_FreeClientUnit(pUnit);
        D2COMMON_UNITS_FreeStaticPath(pUnit);
        UNITSND_StopAllSoundsOnUnit(pUnit, 1);
        break;

    case UNIT_MISSILE:
        MISSILE_FreeUnitData(pUnit);
        break;

    case UNIT_ITEM:
        ITEM_FreeUnitData(pUnit);
        D2COMMON_ITEMS_FreeItemData(NULL, pUnit);
        D2COMMON_UNITS_FreeStaticPath(pUnit);
        break;

    case UNIT_TILE:
        if (gpAct)
            D2COMMON_UNITROOM_RemoveUnitFromRoom(pUnit);
        D2COMMON_UNITS_FreeStaticPath(pUnit);
        break;

    default:
        break;
    }

    CUNITEVENT_FreeAllForUnit(pUnit);
    SCMD_FreeUnitPacketQueue(pUnit);

    if (pUnit == gpPlayerUnit)
    {
        if (gpPlayerUnit)
            UI_ResetUiSubsystems();
        gpPlayerUnit = NULL;
    }

    D2COMMON_UNITS_FreeUnit(pUnit);
}


//1.10f: D2Client.0x6FB28020
//  Drops both unit hash tables: two `rep stosd` of 0x300 dwords each, 0xC00 bytes, which is 6 rows of 128 pointers. The tables are contiguous
//  (0x6FBBAA00 + 0xC00 = 0x6FBBB600). The row is a D2C_UnitTypes value (0 player, 1 monster, 2 object, 3 missile, 4 item, 5 roomtile) and the
//  bucket is `dwUnitId & 0x7F`.
//  Not a destructor: it drops the lists without freeing anything, so it is only safe on a table already empty or whose units were freed some
//  other way. Called at game start, by GAME_InitGame; the teardown counterpart is CUNIT_DestroyAllUnits.
static_assert(6 * 128 * sizeof(D2UnitStrc *) == 0xC00,
              "the unit tables: each rep stosd at 0x6FB28021 fills 0xC00 bytes");
static_assert(0x11B600 - 0x11AA00 == 0xC00,
              "gpUnitTableById and gpUnitTableByClientId are contiguous");

void __cdecl CUNIT_ClearUnitTables()
{
    memset(gpUnitTableById, 0, sizeof(gpUnitTableById));
    memset(gpUnitTableByClientId, 0, sizeof(gpUnitTableByClientId));
}


//  Every walk below reads the successor before it calls out, so whatever is called may free the unit it was handed: `mov esi, [ecx+0E4h]`
//  precedes every call in all eleven walks. None reads it after.
//  Their loop bounds are signed compares of addresses (`cmp edi, offset gpUnitTableById+800h` / `jl`, 0x6FB28113; `cmp ebx, offset
//  gCircle30OffsetX` / `jl`, 0x6FB28089): the trace of an `int` index the compiler strength-reduced into a pointer and kept the signed compare
//  of. Where the index survives, the test is the same `jl` (`inc edi` / `cmp edi, 80h` in DestroyAllUnits, 0x6FB2807D; `inc ebx` / `cmp ebx,
//  80h` in ForEachUnitInTable, 0x6FB28370). Every loop below is written with that `int` index, which computes the same signed test.
//
//  The row the order table visits first is the monster row, and the table's six entries end where the next .rdata object begins.
static_assert(0xD0160 - 0xD0148 == 6 * sizeof(int32_t),
              "gUnitTypeDestroyOrder is six dwords: 0x6FB70148 up to the end "
              "marker `cmp ebx, offset gCircle30OffsetX` at 0x6FB28089");
static_assert(PLRMODE_DEAD == 17,
              "CUNIT_IsPlayerDeadOrMissing: `cmp edx, 11h` at 0x6FB283F4");
static_assert(GAMETYPE_SINGLEPLAYER == 0,
              "CUNIT_UpdateAllUnits resyncs only when `test eax, eax` on "
              "gnGameType is zero (0x6FB28325)");
static_assert(offsetof(D2UnitStrc, pGfxData) == 0x54,
              "CUNIT_GfxGetUnitGfxInfo: `mov eax, [ecx+54h]` at 0x6FB28407");


//1.10f: D2Client.0x6FB28040
//  Destroys every unit the client holds -- both tables, every bucket, every chain -- in the order gUnitTypeDestroyOrder gives: {1, 0, 2, 3, 4, 5},
//  monsters first, then players, objects, missiles, items, room tiles. The end of the order table is the next object's address,
//  gCircle30OffsetX, compared signed.
//  The bucket is rewritten after every destroy: the successor is saved, CUNIT_DestroyUnit runs, and the successor is stored into the bucket
//  (`mov gpUnitTableById[edx*4], esi`, 0x6FB28074) before the next pass, so the table never points at a freed unit, even between two destroys.
//  The row is recomputed from the order table for that store (`mov eax, [ebx]` / `shl eax, 7`, 0x6FB28068), not kept in a register across the
//  call; reading it again is what the C does too. The two tables are walked by the same code twice, the by-id one first.
void __cdecl CUNIT_DestroyAllUnits()
{
    D2UnitStrc *pUnit;
    D2UnitStrc *pNext;
    int32_t nOrder;
    int32_t iBucket;

    for (nOrder = 0; nOrder < 6; ++nOrder)
    {
        for (iBucket = 0; iBucket < 128; ++iBucket)
        {
            pUnit = gpUnitTableById[gUnitTypeDestroyOrder[nOrder]][iBucket];

            while (pUnit)
            {
                pNext = pUnit->pListNext;
                CUNIT_DestroyUnit(pUnit);
                pUnit = pNext;
                gpUnitTableById[gUnitTypeDestroyOrder[nOrder]][iBucket] = pNext;
            }
        }
    }

    for (nOrder = 0; nOrder < 6; ++nOrder)
    {
        for (iBucket = 0; iBucket < 128; ++iBucket)
        {
            pUnit = gpUnitTableByClientId[gUnitTypeDestroyOrder[nOrder]][iBucket];

            while (pUnit)
            {
                pNext = pUnit->pListNext;
                CUNIT_DestroyUnit(pUnit);
                pUnit = pNext;
                gpUnitTableByClientId[gUnitTypeDestroyOrder[nOrder]][iBucket] = pNext;
            }
        }
    }
}


//1.10f: D2Client.0x6FB280F0
//  The per-frame pass over every unit. Eight walks, in an order that is written out, not looped:
//
//      by-id      row 3  missiles
//      client-id  row 3  missiles
//      client-id  row 2  objects
//      by-id      row 0  players
//      by-id      row 1  monsters
//      by-id      row 2  objects
//      by-id      row 4  items
//      client-id  row 1  monsters
//
//  Row 5, the room tiles, is walked in neither table. Every unit visited gets UNITMODE_UpdateUnit.
//  A client-id unit is looked up again after its update: its id and type are read before the call (0x6FB2812D, 0x6FB28130), and after it
//  CUNIT_FindUnitByClientIdAndType is asked whether the unit still exists -- the update may have freed it. Only then does a surviving monster
//  get MONSTER_CritterTickAmbient and a surviving object OBJECT_ClientProcess; the type is read once from the found unit (`mov eax, [ecx]`,
//  0x6FB28146) for both tests.
//  Nine table-wide callback passes follow, through CUNIT_ForEachUnitInTable: GAME_NullFunc over four rows, then CUNIT_FreeUnitModStats over
//  three, then -- only in a single-player game, `test eax, eax` on gnGameType (0x6FB28325) -- CUNIT_ResyncUnitFromServer over the players and
//  the monsters, the last of them a tail `jmp` (0x6FB28342). GAME_NullFunc is one `retn` (1 byte) and so harmless under the fastcall the walker
//  uses; the cast below is only to the walker's callback type.
//  The five walks that only update have CUNIT_ForEachUnitInTable's exact shape (head NULL test, successor before the call, 128 buckets) with the
//  callback called directly and the index strength-reduced into a pointer; the walks are written out as the bytes have them.
void __cdecl CUNIT_UpdateAllUnits()
{
    D2UnitStrc *pUnit;
    D2UnitStrc *pNext;
    D2UnitStrc *pFound;
    uint32_t dwUnitId;
    D2C_UnitTypes nUnitType;
    int32_t iBucket;

    for (iBucket = 0; iBucket < 128; ++iBucket)
    {
        pUnit = gpUnitTableById[UNIT_MISSILE][iBucket];
        while (pUnit)
        {
            pNext = pUnit->pListNext;
            UNITMODE_UpdateUnit(pUnit);
            pUnit = pNext;
        }
    }

    for (iBucket = 0; iBucket < 128; ++iBucket)
    {
        pUnit = gpUnitTableByClientId[UNIT_MISSILE][iBucket];
        while (pUnit)
        {
            pNext = pUnit->pListNext;
            dwUnitId = pUnit->dwUnitId;
            nUnitType = pUnit->dwUnitType;
            UNITMODE_UpdateUnit(pUnit);

            pFound = CUNIT_FindUnitByClientIdAndType((int32_t)dwUnitId, nUnitType);
            if (pFound)
            {
                if (pFound->dwUnitType == UNIT_MONSTER)
                    MONSTER_CritterTickAmbient(pFound);
                else if (pFound->dwUnitType == UNIT_OBJECT)
                    OBJECT_ClientProcess(pFound);
            }

            pUnit = pNext;
        }
    }

    for (iBucket = 0; iBucket < 128; ++iBucket)
    {
        pUnit = gpUnitTableByClientId[UNIT_OBJECT][iBucket];
        while (pUnit)
        {
            pNext = pUnit->pListNext;
            dwUnitId = pUnit->dwUnitId;
            nUnitType = pUnit->dwUnitType;
            UNITMODE_UpdateUnit(pUnit);

            pFound = CUNIT_FindUnitByClientIdAndType((int32_t)dwUnitId, nUnitType);
            if (pFound)
            {
                if (pFound->dwUnitType == UNIT_MONSTER)
                    MONSTER_CritterTickAmbient(pFound);
                else if (pFound->dwUnitType == UNIT_OBJECT)
                    OBJECT_ClientProcess(pFound);
            }

            pUnit = pNext;
        }
    }

    for (iBucket = 0; iBucket < 128; ++iBucket)
    {
        pUnit = gpUnitTableById[UNIT_PLAYER][iBucket];
        while (pUnit)
        {
            pNext = pUnit->pListNext;
            UNITMODE_UpdateUnit(pUnit);
            pUnit = pNext;
        }
    }

    for (iBucket = 0; iBucket < 128; ++iBucket)
    {
        pUnit = gpUnitTableById[UNIT_MONSTER][iBucket];
        while (pUnit)
        {
            pNext = pUnit->pListNext;
            UNITMODE_UpdateUnit(pUnit);
            pUnit = pNext;
        }
    }

    for (iBucket = 0; iBucket < 128; ++iBucket)
    {
        pUnit = gpUnitTableById[UNIT_OBJECT][iBucket];
        while (pUnit)
        {
            pNext = pUnit->pListNext;
            UNITMODE_UpdateUnit(pUnit);
            pUnit = pNext;
        }
    }

    for (iBucket = 0; iBucket < 128; ++iBucket)
    {
        pUnit = gpUnitTableById[UNIT_ITEM][iBucket];
        while (pUnit)
        {
            pNext = pUnit->pListNext;
            UNITMODE_UpdateUnit(pUnit);
            pUnit = pNext;
        }
    }

    for (iBucket = 0; iBucket < 128; ++iBucket)
    {
        pUnit = gpUnitTableByClientId[UNIT_MONSTER][iBucket];
        while (pUnit)
        {
            pNext = pUnit->pListNext;
            dwUnitId = pUnit->dwUnitId;
            nUnitType = pUnit->dwUnitType;
            UNITMODE_UpdateUnit(pUnit);

            pFound = CUNIT_FindUnitByClientIdAndType((int32_t)dwUnitId, nUnitType);
            if (pFound)
            {
                if (pFound->dwUnitType == UNIT_MONSTER)
                    MONSTER_CritterTickAmbient(pFound);
                else if (pFound->dwUnitType == UNIT_OBJECT)
                    OBJECT_ClientProcess(pFound);
            }

            pUnit = pNext;
        }
    }

    CUNIT_ForEachUnitInTable(gpUnitTableByClientId[UNIT_MONSTER], (void (__fastcall *)(D2UnitStrc *))GAME_NullFunc);
    CUNIT_ForEachUnitInTable(gpUnitTableById[UNIT_ITEM], (void (__fastcall *)(D2UnitStrc *))GAME_NullFunc);
    CUNIT_ForEachUnitInTable(gpUnitTableById[UNIT_PLAYER], (void (__fastcall *)(D2UnitStrc *))GAME_NullFunc);
    CUNIT_ForEachUnitInTable(gpUnitTableById[UNIT_MONSTER], (void (__fastcall *)(D2UnitStrc *))GAME_NullFunc);

    CUNIT_ForEachUnitInTable(gpUnitTableByClientId[UNIT_MONSTER], CUNIT_FreeUnitModStats);
    CUNIT_ForEachUnitInTable(gpUnitTableById[UNIT_ITEM], CUNIT_FreeUnitModStats);
    CUNIT_ForEachUnitInTable(gpUnitTableById[UNIT_PLAYER], CUNIT_FreeUnitModStats);

    if (gnGameType == GAMETYPE_SINGLEPLAYER)
    {
        CUNIT_ForEachUnitInTable(gpUnitTableById[UNIT_PLAYER], CUNIT_ResyncUnitFromServer);
        CUNIT_ForEachUnitInTable(gpUnitTableById[UNIT_MONSTER], CUNIT_ResyncUnitFromServer);
    }
}


//1.10f: D2Client.0x6FB28350
//  Calls a callback over one row of a unit table: 128 buckets, each chain walked with the successor read before the call (`mov esi, [ecx+0E4h]`,
//  0x6FB28362), so the callback may free the unit it is handed. The unit goes in ecx (`call edi`), the callbacks being `__fastcall(D2UnitStrc *)`.
//  The bucket index is `int`, compared signed (`jl`, 0x6FB28377).
void __fastcall CUNIT_ForEachUnitInTable(D2UnitStrc **pTable, void (__fastcall *pfnCallback)(D2UnitStrc *pUnit))
{
    D2UnitStrc *pUnit;
    D2UnitStrc *pNext;
    int32_t nBucket;

    for (nBucket = 0; nBucket < 128; ++nBucket)
    {
        pUnit = pTable[nBucket];
        while (pUnit)
        {
            pNext = pUnit->pListNext;
            pfnCallback(pUnit);
            pUnit = pNext;
        }
    }
}


//1.10f: D2Client.0x6FB28380
//  The mod-stat free, wrapped so it can be a callback: `push ecx` / `call STATLIST_FreeModStats` / `retn`. It is a call and a return, not a tail
//  jump (`call` at 0x6FB28381). Its address is only taken, in CUNIT_UpdateAllUnits.
void __fastcall CUNIT_FreeUnitModStats(D2UnitStrc *pUnit)
{
    D2COMMON_STATLIST_FreeModStats(pUnit);
}


//1.10f: D2Client.0x6FB28390
//  The one writer of gpPlayerUnit, and the only place the client changes which unit it is playing. Passing NULL is the teardown case, reached on
//  leaving a game.
//  Order matters, and the assignment is last. The UI is torn down first -- UI_ResetUiSubsystems, and only if a player was already set -- then,
//  for a non-NULL new player, the path direction is cleared, the class is cached, and the two panel/skill rebuilds run. gpPlayerUnit itself is
//  written at 0x6FB283C4, after all of it, so every call above still sees the old player; UI_InitUiForPlayerUnit reads gpPlayerUnit, so on a
//  change of player it initialises the new player's UI from the previous unit.
//  Both branches are independent tests, not an if/else: a NULL argument with a live gpPlayerUnit tears down and assigns NULL; a non-NULL
//  argument with a NULL gpPlayerUnit skips the teardown and builds.
//  BUG: the class is truncated to a byte: `mov eax, [esi+4]` then `mov gnPlayerClass, al` -- dwClassId is 32-bit and the global is one byte, so
//  anything above 0xFF is lost. Reproduced with the cast written out. LATENT: the class ids are 0..6, so nothing can reach the truncation.
//  void: there is no `xor eax, eax` before the `retn`, so eax carries out whatever was last put in it (UI_InitUiForPlayerUnit's answer on the
//  building path, UI_ResetUiSubsystems' on the teardown path, the old gpPlayerUnit when neither is called); no caller reads it.
static_assert(offsetof(D2UnitStrc, dwClassId) == 0x04,
              "CUNIT_SetPlayerUnit reads the class at [esi+4] (0x6FB283B0)");
//  The path pointer is inside an anonymous union, shared with pStaticPath.
static_assert(offsetof(D2UnitStrc, pDynamicPath) == 0x2C,
              "CUNIT_SetPlayerUnit reads the dynamic path at [esi+2Ch] "
              "(0x6FB283A5); pStaticPath shares the offset");


//  The offset of every D2UnitStrc member, the union arms included: +0x10 has three arms, +0x14 has five, and 0x64..0x93 is the server/client
//  split.

static_assert(sizeof(D2UnitStrc) == 0xF4, "D2MOO: Size: 0xF4");

static_assert(offsetof(D2UnitStrc, dwUnitType) == 0x00, "D2MOO 0x00");
static_assert(offsetof(D2UnitStrc, dwClassId) == 0x04, "D2MOO 0x04");
static_assert(offsetof(D2UnitStrc, pMemoryPool) == 0x08, "D2MOO 0x08");
static_assert(offsetof(D2UnitStrc, dwUnitId) == 0x0C, "D2MOO 0x0C");

//  the +0x10 union: three arms, chosen by dwUnitType
static_assert(offsetof(D2UnitStrc, dwAnimMode) == 0x10, "D2MOO 0x10");
static_assert(offsetof(D2UnitStrc, dwItemMode) == 0x10, "D2MOO 0x10");
static_assert(offsetof(D2UnitStrc, dwMissileMode) == 0x10, "D2MOO 0x10");

//  the +0x14 union: five arms, the same choice
static_assert(offsetof(D2UnitStrc, pPlayerData) == 0x14, "D2MOO 0x14");
static_assert(offsetof(D2UnitStrc, pItemData) == 0x14, "D2MOO 0x14");
static_assert(offsetof(D2UnitStrc, pMonsterData) == 0x14, "D2MOO 0x14");
static_assert(offsetof(D2UnitStrc, pObjectData) == 0x14, "D2MOO 0x14");
static_assert(offsetof(D2UnitStrc, pMissileData) == 0x14, "D2MOO 0x14");
static_assert(offsetof(D2UnitStrc, nAct) == 0x18, "D2MOO 0x18");
static_assert(offsetof(D2UnitStrc, unk0x19) == 0x19, "D2MOO 0x19");
static_assert(offsetof(D2UnitStrc, pDrlgAct) == 0x1C, "D2MOO 0x1C");
static_assert(offsetof(D2UnitStrc, pSeed) == 0x20, "D2MOO 0x20");
static_assert(offsetof(D2UnitStrc, dwInitSeed) == 0x28, "D2MOO 0x28");

//  the +0x2C union: the path, dynamic or static
static_assert(offsetof(D2UnitStrc, pStaticPath) == 0x2C, "D2MOO 0x2C");
static_assert(offsetof(D2UnitStrc, pAnimSeq) == 0x30, "D2MOO 0x30");
static_assert(offsetof(D2UnitStrc, dwSeqFrameCount) == 0x34, "D2MOO 0x34");
static_assert(offsetof(D2UnitStrc, dwSeqFrame) == 0x38, "D2MOO 0x38");
static_assert(offsetof(D2UnitStrc, dwSeqSpeed) == 0x3C, "D2MOO 0x3C");
static_assert(offsetof(D2UnitStrc, dwSeqMode) == 0x40, "D2MOO 0x40");
static_assert(offsetof(D2UnitStrc, nSeqCurrentFramePrecise) == 0x44, "D2MOO 0x44");
static_assert(offsetof(D2UnitStrc, dwFrameCountPrecise) == 0x48, "D2MOO 0x48");
static_assert(offsetof(D2UnitStrc, wAnimSpeed) == 0x4C, "D2MOO 0x4C");
static_assert(offsetof(D2UnitStrc, nActionFrame) == 0x4E, "D2MOO 0x4E");
static_assert(offsetof(D2UnitStrc, unk0x4F) == 0x4F, "D2MOO 0x4F");
static_assert(offsetof(D2UnitStrc, pAnimData) == 0x50, "D2MOO 0x50");
static_assert(offsetof(D2UnitStrc, pGfxData) == 0x54, "D2MOO 0x54");
static_assert(offsetof(D2UnitStrc, pGfxDataCopy) == 0x58, "D2MOO 0x58");
static_assert(offsetof(D2UnitStrc, nComponentDrawCount) == 0x58, "the client's arm (TODO.md T57): `mov dword ptr [ecx+58h], 3` at 0x6FB29AA2");
static_assert(offsetof(D2UnitStrc, pStatListEx) == 0x5C, "D2MOO 0x5C");
static_assert(offsetof(D2UnitStrc, pInventory) == 0x60, "D2MOO 0x60");

//  0x64..0x93 -- the server view
static_assert(offsetof(D2UnitStrc, dwInteractGUID) == 0x64, "D2MOO 0x64");
static_assert(offsetof(D2UnitStrc, dwInteractType) == 0x68, "D2MOO 0x68");
static_assert(offsetof(D2UnitStrc, nInteract) == 0x6C, "D2MOO 0x6C");
static_assert(offsetof(D2UnitStrc, nUpdateType) == 0x6E, "D2MOO 0x6E");
static_assert(offsetof(D2UnitStrc, pUpdateUnit) == 0x70, "D2MOO 0x70");
static_assert(offsetof(D2UnitStrc, pQuestEventList) == 0x74, "D2MOO 0x74");
static_assert(offsetof(D2UnitStrc, bSparkChest) == 0x78, "D2MOO 0x78");
static_assert(offsetof(D2UnitStrc, pTimerParams) == 0x7C, "D2MOO 0x7C");
static_assert(offsetof(D2UnitStrc, pGame) == 0x80, "D2MOO 0x80");
static_assert(offsetof(D2UnitStrc, unk0x084) == 0x84, "D2MOO 0x84");
static_assert(offsetof(D2UnitStrc, pSrvUnitEventsList) == 0x90, "D2MOO 0x90");

//  0x64..0x93 -- the client view
static_assert(offsetof(D2UnitStrc, pLight) == 0x64, "D2MOO 0x64");
static_assert(offsetof(D2UnitStrc, dwStartLight) == 0x68, "D2MOO 0x68");
static_assert(offsetof(D2UnitStrc, nPaletteIndex) == 0x6C, "D2MOO 0x6C");
static_assert(offsetof(D2UnitStrc, bUnitSfx) == 0x70, "D2MOO 0x70");
static_assert(offsetof(D2UnitStrc, dwSfxMode) == 0x74, "D2MOO 0x74");
static_assert(offsetof(D2UnitStrc, pUnitSfxData) == 0x78, "D2MOO 0x78");
static_assert(offsetof(D2UnitStrc, dwSfxTicks) == 0x7C, "D2MOO 0x7C");
static_assert(offsetof(D2UnitStrc, dwSfxAsyncTicks) == 0x80, "D2MOO 0x80");
static_assert(offsetof(D2UnitStrc, dwSfxStepTicks) == 0x84, "D2MOO 0x84");
static_assert(offsetof(D2UnitStrc, bHasActiveSound) == 0x88, "D2MOO 0x88");
static_assert(offsetof(D2UnitStrc, nLastClickX) == 0x8C, "D2MOO 0x8C");
static_assert(offsetof(D2UnitStrc, nLastClickY) == 0x8E, "D2MOO 0x8E");
static_assert(offsetof(D2UnitStrc, pCltTimerList) == 0x90, "D2MOO 0x90");
static_assert(offsetof(D2UnitStrc, dwOwnerType) == 0x94, "D2MOO 0x94");
static_assert(offsetof(D2UnitStrc, dwOwnerGUID) == 0x98, "D2MOO 0x98");
static_assert(offsetof(D2UnitStrc, dwKillerType) == 0x9C, "D2MOO 0x9C");
static_assert(offsetof(D2UnitStrc, dwKillerGUID) == 0xA0, "D2MOO 0xA0");
static_assert(offsetof(D2UnitStrc, pHoverText) == 0xA4, "D2MOO 0xA4");
static_assert(offsetof(D2UnitStrc, pSkills) == 0xA8, "D2MOO 0xA8");
static_assert(offsetof(D2UnitStrc, pCombat) == 0xAC, "D2MOO 0xAC");
static_assert(offsetof(D2UnitStrc, dwLastHitClass) == 0xB0, "D2MOO 0xB0");
static_assert(offsetof(D2UnitStrc, unk0xB4) == 0xB4, "D2MOO 0xB4");
static_assert(offsetof(D2UnitStrc, dwDropItemCode) == 0xB8, "D2MOO 0xB8");
static_assert(offsetof(D2UnitStrc, unk0xBC) == 0xBC, "D2MOO 0xBC");
static_assert(offsetof(D2UnitStrc, dwFlags) == 0xC4, "D2MOO 0xC4");
static_assert(offsetof(D2UnitStrc, dwFlagEx) == 0xC8, "D2MOO 0xC8");
static_assert(offsetof(D2UnitStrc, pUnused0xCC) == 0xCC, "D2MOO 0xCC");
static_assert(offsetof(D2UnitStrc, dwNodeIndex) == 0xD0, "D2MOO 0xD0");
static_assert(offsetof(D2UnitStrc, dwTickCount) == 0xD4, "D2MOO 0xD4");

//  the +0xD8 union: a tick count on the server, a packet list on the client
static_assert(offsetof(D2UnitStrc, dwSrvTickCount) == 0xD8, "D2MOO 0xD8");
static_assert(offsetof(D2UnitStrc, pPacketList) == 0xD8, "D2MOO 0xD8");
static_assert(offsetof(D2UnitStrc, pTimer) == 0xDC, "D2MOO 0xDC");
static_assert(offsetof(D2UnitStrc, pChangeNextUnit) == 0xE0, "D2MOO 0xE0");
static_assert(offsetof(D2UnitStrc, pListNext) == 0xE4, "D2MOO 0xE4");
static_assert(offsetof(D2UnitStrc, pRoomNext) == 0xE8, "D2MOO 0xE8");
static_assert(offsetof(D2UnitStrc, pMsgFirst) == 0xEC, "D2MOO 0xEC");
static_assert(offsetof(D2UnitStrc, pMsgLast) == 0xF0, "D2MOO 0xF0");

void __fastcall CUNIT_SetPlayerUnit(D2UnitStrc *pPlayer)
{
    if (gpPlayerUnit)
        UI_ResetUiSubsystems();

    if (pPlayer)
    {
        D2COMMON_PATH_SetDirection(pPlayer->pDynamicPath, 0);
        gnPlayerClass = (uint8_t)pPlayer->dwClassId;
        GAME_SetStandStillKeyHeld(0);
        UI_InitUiForPlayerUnit();
    }

    gpPlayerUnit = pPlayer;
}


//1.10f: D2Client.0x6FB283D0
//  The local player: one load and a return (`mov eax, gpPlayerUnit`). No arguments, so the calling convention cannot be seen in the bytes;
//  __fastcall is followed.
D2UnitStrc *__fastcall CUNIT_GetPlayer()
{
    return gpPlayerUnit;
}


//1.10f: D2Client.0x6FB283E0
//  No player, or a dead one -- the UI's "can the player act" test: TRUE with no player, else whether dwAnimMode (+0x10) is PLRMODE_DEAD (11h).
//  The mode is read without asking what type the unit is: the player unit is a player. No arguments, so the calling convention cannot be seen
//  in the bytes; __cdecl is followed.
BOOL __cdecl CUNIT_IsPlayerDeadOrMissing()
{
    if (!gpPlayerUnit)
        return TRUE;

    return gpPlayerUnit->dwAnimMode == PLRMODE_DEAD;
}


//1.10f: D2Client.0x6FB28400
//  The unit's client graphics block (`mov eax, [ecx+54h]` after a NULL test), a D2ClientUnitGfxInfoStrc, or NULL for no unit.
D2ClientUnitGfxInfoStrc *__fastcall CUNIT_GfxGetUnitGfxInfo(D2UnitStrc *pUnit)
{
    if (!pUnit)
        return NULL;

    return pUnit->pGfxData;
}


//  The six select procs are reached only through gpSkillSelectProcs (slot 0 NULL, slots 1 to 6 these six, indexed by skills.txt's SelectProc
//  column). Their image order is 4, 6, 5 -- GrimWard sits before FindItem -- while their names follow the slot.
//
//  The graphics block's palette-shift members (+0x34 and +0x38), written by the setters and read by the getters.
static_assert(offsetof(D2ClientUnitGfxInfoStrc, nPaletteShift1) == 0x34
              && offsetof(D2ClientUnitGfxInfoStrc, nPaletteShift2) == 0x38,
              "GFX_SetPaletteShift1/2 write [ecx+34h] / [ecx+38h]");
static_assert(offsetof(D2LvlWarpTxt, dwSelectX) == 0x04
              && offsetof(D2LvlWarpTxt, dwSelectY) == 0x08
              && offsetof(D2LvlWarpTxt, dwSelectDX) == 0x0C
              && offsetof(D2LvlWarpTxt, dwSelectDY) == 0x10,
              "the warp box: [edi+4] 0x6FB2857F, [edi+8] 0x6FB2858C, "
              "[edi+0Ch] 0x6FB2859D, [edi+10h] 0x6FB285A6");
static_assert(offsetof(D2MonStatsTxt, nBaseId) == 0x02,
              "Revive's family test: `movsx eax, word ptr [ecx+2]` at 0x6FB2864F");
static_assert(offsetof(D2ObjectsTxt, nSubClass) == 0x167 && OBJSUBCLASS_DOOR == 0x80,
              "the door carve-out: `mov cl, [eax+167h]` / `test cl, cl` / "
              "`jns` at 0x6FB286CA..0x6FB286D2 tests bit 7");


//1.10f: D2Client.0x6FB28410
//  The first palette shift of a unit's graphics block. A NULL unit is an assert that exits ("hUnit", CUnit.cpp:1377); otherwise the block at +0x54
//  goes to GFX_SetPaletteShift1 in ecx with nShift still in edx, as a tail `jmp` (0x6FB28435). The setter asserts its own `ptInfo` and stores
//  +0x34 (nPaletteShift1).
void __fastcall CUNIT_SetUnitPaletteShift1(D2UnitStrc *pUnit, int32_t nShift)
{
    if (!pUnit)
    {
        FOG_DisplayAssert((uint32_t)"hUnit", (uint32_t)szCUnitCpp, 1377);
        exit(-1);
    }

    GFX_SetPaletteShift1(pUnit->pGfxData, nShift);
}


//1.10f: D2Client.0x6FB28440
//  Its getter: "hUnit" at CUnit.cpp:1386, then a tail `jmp` to GFX_GetPaletteShift1 (0x6FB28465), which reads +0x34.
int32_t __fastcall CUNIT_GetUnitPaletteShift1(D2UnitStrc *pUnit)
{
    if (!pUnit)
    {
        FOG_DisplayAssert((uint32_t)"hUnit", (uint32_t)szCUnitCpp, 1386);
        exit(-1);
    }

    return GFX_GetPaletteShift1(pUnit->pGfxData);
}


//1.10f: D2Client.0x6FB28470
//  The second shift, the same shape: "hUnit" at CUnit.cpp:1395, then GFX_SetPaletteShift2 (+0x38).
void __fastcall CUNIT_SetUnitPaletteShift2(D2UnitStrc *pUnit, int32_t nShift)
{
    if (!pUnit)
    {
        FOG_DisplayAssert((uint32_t)"hUnit", (uint32_t)szCUnitCpp, 1395);
        exit(-1);
    }

    GFX_SetPaletteShift2(pUnit->pGfxData, nShift);
}


//1.10f: D2Client.0x6FB284A0
//  Its getter: "hUnit" at CUnit.cpp:1404, then GFX_GetPaletteShift2 (+0x38).
int32_t __fastcall CUNIT_GetUnitPaletteShift2(D2UnitStrc *pUnit)
{
    if (!pUnit)
    {
        FOG_DisplayAssert((uint32_t)"hUnit", (uint32_t)szCUnitCpp, 1404);
        exit(-1);
    }

    return GFX_GetPaletteShift2(pUnit->pGfxData);
}


//1.10f: D2Client.0x6FB284D0
//  Is this screen point inside the warp's select box -- what makes a stairway or a portal highlight under the cursor. The unit must be a room
//  tile: the game's own assert spells the type UNIT_ROOMTILE ("ptUnit->eType == UNIT_ROOMTILE", CUnit.cpp:1417), value 5 (UNIT_TILE here).
//  The box is a corner plus a size: lvlwarp.txt's SelectX/SelectY are added to the warp's own position (converted from subtiles to client
//  coordinates by DUNGEON_GameSubtileToClientCoords) to give the left and top edges; SelectDX/SelectDY are the width and height past them. All
//  four tests are signed and inclusive (`jl`, `jl`, `jg`, `jg` at 0x6FB28593..0x6FB285AD), so the box is one pixel wider and taller than DX/DY.
//  The original inlines UNITS_GetXPosition and UNITS_GetYPosition for the position: `cmp eax, 2` / `jz` static, `cmp eax, 3` / `jle` dynamic,
//  `cmp eax, 5` / `jle` static, then the dynamic path's PATH_GetX/YPosition or 0 for no path. The static arm reads the static path's tGameCoords
//  (+0xC, +0x10). The type compares are signed (`jle`) and D2C_UnitTypes is `: uint32_t`, so the compare casts both sides to int; the values are
//  0..5 and cannot tell the difference, but the bytes can.
//  The sums are taken in unsigned arithmetic, as the `unsigned int` fields make them, and converted to int for the signed compares -- which is
//  what `add` then `cmp`/`jl` computes, without a signed overflow in the C.
BOOL __fastcall CUNIT_IsPointInWarpSelectBox(D2UnitStrc *pWarp, int32_t nScreenX, int32_t nScreenY)
{
    D2ActiveRoomStrc *pRoom;
    D2LvlWarpTxt *pLvlWarp;
    int32_t nX;
    int32_t nY;
    int32_t nBoxLeft;
    int32_t nBoxTop;

    if (pWarp->dwUnitType != UNIT_TILE)
    {
        FOG_DisplayAssert((uint32_t)"ptUnit->eType == UNIT_ROOMTILE", (uint32_t)szCUnitCpp, 1417);
        exit(-1);
    }

    pRoom = D2COMMON_UNITS_GetRoom(pWarp);
    pLvlWarp = D2COMMON_DUNGEON_GetLvlWarpTxtRecordFromRoomAndUnit(pRoom, pWarp);
    if (!pLvlWarp)
        return FALSE;

    nX = UNITS_GetXPosition(pWarp);

    nY = UNITS_GetYPosition(pWarp);

    D2COMMON_DUNGEON_GameSubtileToClientCoords(&nX, &nY);

    nBoxLeft = (int32_t)((uint32_t)nX + pLvlWarp->dwSelectX);
    nBoxTop = (int32_t)((uint32_t)nY + pLvlWarp->dwSelectY);

    return nScreenX >= nBoxLeft && nScreenY >= nBoxTop && nScreenX <= (int32_t)((uint32_t)nBoxLeft + pLvlWarp->dwSelectDX) && nScreenY <= (int32_t)((uint32_t)nBoxTop + pLvlWarp->dwSelectDY);
}


//1.10f: D2Client.0x6FB285D0
//  gpSkillSelectProcs[1]: the target test of every skill whose skills.txt SelectProc is 1 -- Corpse Explosion (74), Poison Explosion (83) and
//  NihlathakCorpseExplosion (351) in the stock table. `push edx` / `call D2COMMON_11021` / `retn`. The first argument is the table's and is not
//  read; the unit comes in edx.
BOOL __fastcall CUNIT_SkillsSelectProc1_CorpseExplosion(int32_t nUnused, D2UnitStrc *pUnit)
{
    return D2COMMON_11021(pUnit);
}


//1.10f: D2Client.0x6FB285E0
//  gpSkillSelectProcs[2], Raise Skeleton's and Raise Skeletal Mage's: the corpse must still be consumeable, D2Common #11017 with 0 as its
//  second argument.
BOOL __fastcall CUNIT_SkillsSelectProc2_RaiseSkeleton(int32_t nUnused, D2UnitStrc *pUnit)
{
    return D2COMMON_11017_CheckUnitIfConsumeable(pUnit, 0);
}


//1.10f: D2Client.0x6FB285F0
//  gpSkillSelectProcs[3], Revive's, and the only one of the six with logic of its own: a consumeable corpse (#11017), one MONSTER_CanBeRevived
//  accepts, a monster, and not of the sand leaper family -- base id 78, `sandleaper1` in stock monstats.txt.
//  The family test has three ways to say yes and one to say no. The class is walked to its base through DATATBLS_GetMonStatsTxtRecord, which the
//  original inlines (sign test, the count at +0xA80, the 424-byte stride, the NULL test), falling back to the id it was given. Then the base id is
//  range-tested again before it is compared: negative, or at or past the count, and Revive is allowed (0x6FB28655, 0x6FB2865F). Only an
//  in-range base of exactly 78 refuses. A negative class id jumps straight to "allowed" (0x6FB2862C): the compiler threaded the inlined sign
//  test into the second one, and the C writes that jump as a return.
//  The two early answers return what the callee returned, zero, as the bytes do (`test eax, eax` / `jnz` / `retn`, 0x6FB28603 and 0x6FB28610).
BOOL __fastcall CUNIT_SkillsSelectProc3_Revive(int32_t nUnused, D2UnitStrc *pUnit)
{
    D2MonStatsTxt *pMonStatsTxt;
    BOOL bResult;
    int32_t nMonsterId;

    if (!pUnit)
        return FALSE;

    bResult = D2COMMON_11017_CheckUnitIfConsumeable(pUnit, 0);
    if (!bResult)
        return bResult;

    bResult = MONSTER_CanBeRevived(pUnit);
    if (!bResult)
        return bResult;

    if (pUnit->dwUnitType != UNIT_MONSTER)
        return FALSE;

    nMonsterId = pUnit->dwClassId;
    if (nMonsterId < 0)
        return TRUE;

    if (nMonsterId < sgptDataTables->nMonStatsTxtRecordCount)
    {
        pMonStatsTxt = &sgptDataTables->pMonStatsTxt[nMonsterId];
        if (pMonStatsTxt)
            nMonsterId = pMonStatsTxt->nBaseId;
    }

    if (nMonsterId < 0 || nMonsterId >= sgptDataTables->nMonStatsTxtRecordCount)
        return TRUE;

    return nMonsterId != 78;
}


//1.10f: D2Client.0x6FB28680
//  gpSkillSelectProcs[4], Find Potion's: `return D2COMMON_11018(pUnit)`.
BOOL __fastcall CUNIT_SkillsSelectProc4_FindPotion(int32_t nUnused, D2UnitStrc *pUnit)
{
    return D2COMMON_11018(pUnit);
}


//1.10f: D2Client.0x6FB28690
//  gpSkillSelectProcs[6], Grim Ward's: `return D2COMMON_11020(pUnit)`. It sits before slot 5's function in the image.
BOOL __fastcall CUNIT_SkillsSelectProc6_GrimWard(int32_t nUnused, D2UnitStrc *pUnit)
{
    return D2COMMON_11020(pUnit);
}


//1.10f: D2Client.0x6FB286A0
//  gpSkillSelectProcs[5], Find Item's: `return D2COMMON_11019(pUnit)` (`push edx` / `call` / `retn`).
BOOL __fastcall CUNIT_SkillsSelectProc5_FindItem(int32_t nUnused, D2UnitStrc *pUnit)
{
    return D2COMMON_11019(pUnit);
}


//1.10f: D2Client.0x6FB286B0
//  Draw this unit highlighted? Only if it is the selected unit -- and never when the selected unit is a door: an object whose objects.txt
//  SubClass has OBJSUBCLASS_DOOR.
//  The door bit is tested as the sign of the byte -- `test cl, cl` / `jns` (0x6FB286D0) -- which is bit 7, OBJSUBCLASS_DOOR; the C tests the named
//  bit, which computes the same. SELECT_GetSelectedUnit is called a second time for the final comparison (0x6FB286D8) rather than reusing the
//  first answer, and the C calls it twice.
//  The objects.txt record is used untested, which is safe: D2Common's DATATBLS_GetObjectsTxtRecord (#10626) asserts both ends of the range and
//  exits, so it never returns NULL.
BOOL __fastcall CUNIT_ShouldHighlightUnit(D2UnitStrc *pUnit)
{
    D2UnitStrc *pSelected;

    pSelected = SELECT_GetSelectedUnit();
    if (pSelected && pSelected->dwUnitType == UNIT_OBJECT && (D2COMMON_DATATBLS_GetObjectsTxtRecord(pSelected->dwClassId)->nSubClass & OBJSUBCLASS_DOOR))
    {
        return FALSE;
    }

    return SELECT_GetSelectedUnit() == pUnit;
}


//  HitTestUnit's stack frame holds several meanings in the same slots (the screen X for the whole function; the unit's class id, in and out of
//  D2Common #11013; four flag tests and then a Y coordinate; the fourth argument's slot, reused after it has been tested as the "this unit has
//  several pieces" flag, cleared at 0x6FB28A38 and set at 0x6FB28C63, and in between as an object's X, 0x6FB28ADA). The C gives each meaning
//  its own variable.
//
//  The flags and modes HitTestUnit and its helpers test, as the immediates the bytes carry.
static_assert(UNITFLAG_TARGETABLE == 0x02 && UNITFLAG_CANBEATTACKED == 0x04
              && UNITFLAG_ISDEAD == 0x10000 && UNITFLAGEX_ISINLOS == 0x80,
              "`shr ecx, 1` 0x6FB2881C, `shr edx, 2` 0x6FB28853, "
              "`test edx, 10000h` 0x6FB2882E, `shr eax, 7` on dwFlagEx 0x6FB28874");
static_assert(MONSTATS2FLAGINDEX_NOGFXHITTEST == 0,
              "`mov dl, [eax+4]` / `test [gdwBitMasks], dl` at 0x6FB28954");
static_assert(PCLASS_BARBARIAN == 4 && PLRMODE_SEQUENCE == 18,
              "`cmp dword ptr [esi+4], 4` 0x6FB28C91, `cmp [esi+10h], 12h` 0x6FB28C97");
static_assert(OBJMODE_OPENED == 2 && OBJMODE_SPECIAL2 == 4 && OBJMODE_SPECIAL3 == 5,
              "the door modes: `cmp eax, 2/4/5` at 0x6FB28A9D..0x6FB28AA7");
static_assert(IMODE_ONGROUND == 3 && UI_HOLDALT == 13 && NUM_COMPONENTS == 16,
              "`cmp dword ptr [esi+10h], 3` 0x6FB28A1C, `mov ecx, 0Dh` "
              "0x6FB28A22, `cmp edi, 10h` 0x6FB28CFF");
//  The offsets read, at the instruction named -- the monstats2.txt hit box is read with `movsx` from fields the structure declares unsigned,
//  which is why the C casts them.
static_assert(offsetof(D2MonStats2Txt, whtTop) == 0x10E
              && offsetof(D2MonStats2Txt, whtLeft) == 0x110
              && offsetof(D2MonStats2Txt, whtWidth) == 0x112
              && offsetof(D2MonStats2Txt, whtHeight) == 0x114
              && offsetof(D2MonStats2Txt, nTotalPieces) == 0xEC,
              "`movsx` from +10Eh..+114h at 0x6FB2895B..0x6FB2897A; "
              "`cmp byte ptr [eax+0ECh], 1` at 0x6FB28C5A");
static_assert(offsetof(D2ObjectsTxt, nIsDoor) == 0x13A
              && offsetof(D2ObjectsTxt, nTotalPieces) == 0x161
              && offsetof(D2ObjectsTxt, nCollisionSubst) == 0x19D
              && offsetof(D2ObjectsTxt, dwLeft) == 0x1A0
              && offsetof(D2ObjectsTxt, dwTop) == 0x1A4
              && offsetof(D2ObjectsTxt, dwWidth) == 0x1A8
              && offsetof(D2ObjectsTxt, dwHeight) == 0x1AC,
              "[ebx+13Ah] 0x6FB28A08, [ebx+161h] 0x6FB28C02, [ebx+19Dh] 0x6FB28A12, "
              "[ebx+1A0h..1ACh] 0x6FB28BB3..0x6FB28BF3");
static_assert(offsetof(D2SkillsTxt, nSelectProc) == 0x15,
              "`mov al, [eax+15h]` at 0x6FB287CB and 0x6FB28F4F");
static_assert(offsetof(D2GfxDataStrc, nMaxFrames) == 0x14,
              "the frame bound, `cmp esi, [esp+58h+var_34]` at 0x6FB28E08");
static_assert(offsetof(D2BoundingBoxStrc, nLeft) == 0 && offsetof(D2BoundingBoxStrc, nBottom) == 4
              && offsetof(D2BoundingBoxStrc, nRight) == 8 && offsetof(D2BoundingBoxStrc, nTop) == 12,
              "the box: +0 and +8 are X, +4 and +12 are Y -- `lea edx, [eax+ecx+18h]` "
              "into [esi+8] from [esi] at 0x6FB28E4E");


//1.10f: D2Client.0x6FB286F0
//  Is this unit under the screen point, and may the player select it? The original inlines CUNIT_IsUnitDead, DATATBLS_GetMonStats2TxtRecord,
//  UNITS_GetXPosition and UNITS_GetYPosition in this function. The gates, in the order the bytes take them:
//
//  1.  The UI shift: SCROLL_GetUiShiftScreen answering 1 refuses the right half of the screen, 2 the left half, 3 everything -- tested against
//      the mouse's X (CURSOR_GetMouseX, read first), not the point asked about. `gnScreenWidth / 2` is a signed divide (`cdq` / `sub` / `sar`).
//  2.  A unit in the client-id table (dwFlags bit 21) is never selectable.
//  3.  One of the three target tests must accept it.
//  4.  The select procs: for a monster, the left skill's skills.txt SelectProc, called out of gpSkillSelectProcs, or the right skill's through
//      CUNIT_SkillsRunSelectProcById. If either accepts, UNITFLAG_TARGETABLE makes it selectable, and so does UNITFLAG_CANBEATTACKED on a dead
//      monster -- that is how a corpse becomes clickable for Corpse Explosion or Revive. If neither accepts, or it is not a monster, only
//      UNITFLAG_TARGETABLE counts. The dead test is CUNIT_IsUnitDead inlined (0x6FB29550), player arm and all, although the unit is already
//      known to be a monster.
//  5.  UNITFLAGEX_ISINLOS: a unit out of sight is not selectable.
//
//  With bTestGeometry 0 the answer is TRUE here. Otherwise the geometry. The unit's type, class and mode go through D2Common #11013, which may
//  change all three, and the rest works on what it returns:
//
//      PLAYER   the COF box test, GFX_HitTestUnitCof
//      MONSTER  monstats2.txt NoGfxHitTest set: the fixed box whtLeft/whtTop/whtWidth/whtHeight through PtInRect, and that is the answer;
//               otherwise the cel test
//      OBJECT   the cel test -- a miss is forgiven for a door or an object with CollisionSubst
//      ITEM     only lying on the ground (IMODE_ONGROUND), and only while the UI_HOLDALT window is closed
//      TILE     CUNIT_IsPointInWarpSelectBox, and that is the answer
//
//  and then an object may still answer from its own objects.txt box -- a door in mode 2, 4 or 5, or an object with CollisionSubst -- and
//  otherwise the component boxes decide: every one of the 16 for a player and for a monster or object with more than one piece, or component 1
//  alone.
//  A barbarian in PLRMODE_SEQUENCE is accepted outright just before the component pass (0x6FB28C91..0x6FB28C9B); the bytes say nothing of why.
//  The object record is asserted twice, once by D2Common's getter (which never returns NULL) and once here -- "ptObjectData", CUnit.cpp:1855 --
//  and the second cannot fire.
//  Signs: the select proc index is compared signed with gnSkillSelectProcCount (`jge`, 0x6FB287DF). The unit-type tests of the four
//  UNITS_GetX/YPosition copies are signed `jle`. The objects.txt box is formed from `unsigned int` fields and compared signed (`jl`/`jg`), as
//  the warp box is. The component loop is unsigned -- `cmp edi, 10h` / `jb` at 0x6FB28D02. The monstats2.txt box is sign-extended (`movsx`).
BOOL __fastcall CUNIT_HitTestUnit(D2UnitStrc *pUnit, int32_t nScreenX, int32_t nScreenY, int32_t bTestGeometry)
{
    D2UnitStrc *pPlayer;
    D2SkillsTxt *pSkillsTxt;
    D2MonStats2Txt *pMonStats2Txt;
    D2ObjectsTxt *pObjectsTxt;
    D2BoundingBoxStrc tBox;
    RECT tRect;
    POINT tPoint;
    uint32_t dwFlags;
    uint32_t iComponent;
    int32_t nMouseX;
    int32_t bSelectable;
    int32_t bProcAccepts;
    int32_t bDead;
    int32_t nSelectProc;
    int32_t nType;
    int32_t nClass;
    int32_t nMode;
    int32_t nClientX;
    int32_t nClientY;
    int32_t nX;
    int32_t nY;
    int32_t nLeft;
    int32_t nTop;
    int32_t nFrame;
    int32_t nFrameCount;
    int32_t bMultiPiece;

    nMouseX = CURSOR_GetMouseX();

    switch (SCROLL_GetUiShiftScreen())
    {
    case 1:
        if (nMouseX >= gnScreenWidth / 2)
            return FALSE;
        break;

    case 2:
        if (nMouseX < gnScreenWidth / 2)
            return FALSE;
        break;

    case 3:
        return FALSE;
    }

    pPlayer = gpPlayerUnit;

    //  UNITFLAG_ISASYNC: the client-id table
    if (pUnit && ((pUnit->dwFlags >> 21) & 1))
        return FALSE;

    if (!CUNIT_IsUnitSelectableTarget(pPlayer, pUnit) && !CUNIT_IsSelectableAsPetTarget(pPlayer, pUnit) && !CUNIT_IsSelectableAsAllyTarget(pPlayer, pUnit))
    {
        return FALSE;
    }

    bSelectable = FALSE;
    if (!pUnit)
        return FALSE;

    bProcAccepts = FALSE;
    if (pUnit->dwUnitType == UNIT_MONSTER)
    {
        pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(D2COMMON_SKILLS_GetSkillIdFromSkill(D2COMMON_UNITS_GetLeftSkill(pPlayer), szCUnitCpp, 1672));
        if (pSkillsTxt)
        {
            nSelectProc = pSkillsTxt->nSelectProc;
            if (nSelectProc != 0 && nSelectProc < gnSkillSelectProcCount && gpSkillSelectProcs[nSelectProc]((int32_t)pPlayer, pUnit))
            {
                bProcAccepts = TRUE;
            }
        }

        if (!bProcAccepts && CUNIT_SkillsRunSelectProcById(pPlayer, pUnit, D2COMMON_SKILLS_GetSkillIdFromSkill(D2COMMON_UNITS_GetRightSkill(pPlayer), szCUnitCpp, 1676)))
        {
            bProcAccepts = TRUE;
        }
    }

    if (bProcAccepts)
    {
        dwFlags = pUnit->dwFlags;
        if ((dwFlags >> 1) & 1)                         //  UNITFLAG_TARGETABLE
        {
            bSelectable = TRUE;
        }
        else if (pUnit->dwUnitType == UNIT_MONSTER)
        {
            if (dwFlags & UNITFLAG_ISDEAD)
                bDead = TRUE;
            else if (pUnit->dwUnitType == UNIT_PLAYER)
                bDead = pUnit->dwAnimMode == PLRMODE_DEATH || pUnit->dwAnimMode == PLRMODE_DEAD;
            else if (pUnit->dwUnitType == UNIT_MONSTER)
                bDead = pUnit->dwAnimMode == MONMODE_DEATH || pUnit->dwAnimMode == MONMODE_DEAD;
            else
                bDead = FALSE;

            if (bDead)
                bSelectable = (dwFlags >> 2) & 1;       //  UNITFLAG_CANBEATTACKED
        }
    }
    else
    {
        bSelectable = (pUnit->dwFlags >> 1) & 1;        //  UNITFLAG_TARGETABLE
    }

    if (!((pUnit->dwFlagEx >> 7) & 1))                  //  UNITFLAGEX_ISINLOS
        return FALSE;
    if (!bSelectable)
        return FALSE;
    if (!bTestGeometry)
        return TRUE;

    nMode = D2COMMON_UNITS_GetAnimOrSeqMode(pUnit);
    nClass = pUnit->dwClassId;
    nType = pUnit->dwUnitType;
    D2COMMON_11013_ConvertMode(pUnit, &nType, &nClass, &nMode, szCUnitCpp, 1771);

    pObjectsTxt = NULL;
    nClientX = D2COMMON_UNITS_GetClientCoordX(pUnit);
    nClientY = D2COMMON_UNITS_GetClientCoordY(pUnit);

    switch (nType)
    {
    case UNIT_PLAYER:
        if (!GFX_HitTestUnitCof(pUnit, nClientX, nClientY, nScreenX, nScreenY))
            return FALSE;
        break;

    case UNIT_MONSTER:
        pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(nClass);

        //  BUG: the monster's MonStats2 record is read with no NULL test. The lookup answers NULL for a class out of range (`xor eax, eax` at
        //  0x6FB2894C) and the next instruction reads `[eax+4]` (0x6FB28954): a fault at address 4. The class is what #11013 returned. LATENT: a
        //  monster's own class is bounded where it is written (CreateMonsterUnit's head, MONSTER_ChangeMonsterClass's entry), and #11013's
        //  substitute for a shapeshifted unit, states.txt gfxclass, names a valid row in all four stock transform states.
        if (pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_NOGFXHITTEST >> 3] & (uint8_t)gdwBitMasks[MONSTATS2FLAGINDEX_NOGFXHITTEST & 7])
        {
            tRect.left = nClientX + (int16_t)pMonStats2Txt->whtLeft;
            tRect.top = nClientY + (int16_t)pMonStats2Txt->whtTop;
            tRect.right = tRect.left + (int16_t)pMonStats2Txt->whtWidth;
            tRect.bottom = tRect.top + (int16_t)pMonStats2Txt->whtHeight;
            tPoint.x = nScreenX;
            tPoint.y = nScreenY;
            return PtInRect(&tRect, tPoint) ? TRUE : FALSE;
        }

        if (!GFX_HitTestUnitCof(pUnit, nClientX, nClientY, nScreenX, nScreenY))
            return FALSE;
        break;

    case UNIT_OBJECT:
        pObjectsTxt = D2COMMON_DATATBLS_GetObjectsTxtRecord(nClass);
        if (!GFX_HitTestUnitCof(pUnit, nClientX, nClientY, nScreenX, nScreenY) && !pObjectsTxt->nIsDoor && !pObjectsTxt->nCollisionSubst)
        {
            return FALSE;
        }
        break;

    case UNIT_ITEM:
        if (pUnit->dwItemMode != IMODE_ONGROUND)
            return FALSE;
        if (UI_GetUiWindowState(UI_HOLDALT))
            return FALSE;
        break;

    case UNIT_TILE:
        return CUNIT_IsPointInWarpSelectBox(pUnit, nScreenX, nScreenY);

    default:
        break;
    }

    bMultiPiece = FALSE;

    if (nType == UNIT_PLAYER)
    {
        bMultiPiece = TRUE;
    }
    else if (nType == UNIT_MONSTER)
    {
        pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(nClass);
        if (pMonStats2Txt && pMonStats2Txt->nTotalPieces > 1)
            bMultiPiece = TRUE;
    }
    else if (nType == UNIT_OBJECT)
    {
        if (!pObjectsTxt)
        {
            FOG_DisplayAssert((uint32_t)"ptObjectData", (uint32_t)szCUnitCpp, 1855);
            exit(-1);
        }

        if (pObjectsTxt->nIsDoor)
        {
            if (pUnit->dwAnimMode == OBJMODE_OPENED || pUnit->dwAnimMode == OBJMODE_SPECIAL2 || pUnit->dwAnimMode == OBJMODE_SPECIAL3)
            {
                nX = UNITS_GetXPosition(pUnit);

                nY = UNITS_GetYPosition(pUnit);

                D2COMMON_DUNGEON_GameSubtileToClientCoords(&nX, &nY);

                nLeft = (int32_t)((uint32_t)nX + pObjectsTxt->dwLeft);
                nTop = (int32_t)((uint32_t)nY + pObjectsTxt->dwTop);
                return nScreenX >= nLeft && nScreenY >= nTop && nScreenX <= (int32_t)((uint32_t)nLeft + pObjectsTxt->dwWidth) && nScreenY <= (int32_t)((uint32_t)nTop + pObjectsTxt->dwHeight);
            }
        }
        else if (pObjectsTxt->nCollisionSubst)
        {
            nX = UNITS_GetXPosition(pUnit);

            nY = UNITS_GetYPosition(pUnit);

            //  the two object boxes share their tail from this call on
            //  (0x6FB28BAE); the compiler merged it
            D2COMMON_DUNGEON_GameSubtileToClientCoords(&nX, &nY);

            nLeft = (int32_t)((uint32_t)nX + pObjectsTxt->dwLeft);
            nTop = (int32_t)((uint32_t)nY + pObjectsTxt->dwTop);
            return nScreenX >= nLeft && nScreenY >= nTop && nScreenX <= (int32_t)((uint32_t)nLeft + pObjectsTxt->dwWidth) && nScreenY <= (int32_t)((uint32_t)nTop + pObjectsTxt->dwHeight);
        }

        if (pObjectsTxt->nTotalPieces > 1)
            bMultiPiece = TRUE;
    }

    D2COMMON_UNITS_GetShiftedFrameMetrics(pUnit, &nFrame, &nFrameCount);
    memset(&tBox, 0, sizeof(tBox));

    if (pUnit->dwUnitType == UNIT_PLAYER && pUnit->dwClassId == PCLASS_BARBARIAN && pUnit->dwAnimMode == PLRMODE_SEQUENCE)
    {
        return TRUE;
    }

    if (bMultiPiece)
    {
        for (iComponent = 0; iComponent < NUM_COMPONENTS; ++iComponent)
        {
            if (CUNIT_GetComponentScreenBox(pUnit, nMode, (int32_t)iComponent, nFrame, &tBox, bMultiPiece) && GFXUTIL_IsUnitComponentVisible(pUnit, (D2Composits)iComponent) && nScreenX >= tBox.nLeft && nScreenX <= tBox.nRight
                && nScreenY >= tBox.nTop && nScreenY <= tBox.nBottom)
            {
                return TRUE;
            }
        }

        return FALSE;
    }

    if (!CUNIT_GetComponentScreenBox(pUnit, nMode, COMPOSIT_TORSO, nFrame, &tBox, 0))
        return FALSE;

    return nScreenX >= tBox.nLeft && nScreenX <= tBox.nRight && nScreenY >= tBox.nTop && nScreenY <= tBox.nBottom;
}


//1.10f: D2Client.0x6FB28D90
//  The screen box of one component: the cel's offset and size around the unit's client position, padded, stretched to at least 32x32 and
//  shifted by the particle offsets.
//  Two arguments are not read: edx (nUnused1, which the caller loads from its mode) is overwritten at 0x6FB28D9A before anything uses it, and
//  the last stack slot (nUnused2) is never addressed; `retn 10h` counts both, so both stay.
//  The frame is bounded unsigned: "(UINT)nAnimFrame <= tCelContext.dwCelsPerDirection", CUnit.cpp:1501, is `cmp esi, [...]` / `jbe` at
//  0x6FB28E08 against the draw record's nMaxFrames.
//  The padding: 12 pixels on each side (-12/+24 across, +12/-24 down; screen Y grows downward, so the client Y is the box's bottom), then a box
//  narrower or shorter than 32 grows by half the shortfall each way -- `sar` on a difference that is positive there. The height is taken from
//  the bottom and top as computed, not reread from the box (0x6FB28EA3).
//  The order of the calls is kept with locals: the cel offset before the client coordinate on each axis (0x6FB28E2E before 0x6FB28E36,
//  0x6FB28E55 before 0x6FB28E5D), and the three particle offsets X, Y, Z last.
BOOL __fastcall CUNIT_GetComponentScreenBox(D2UnitStrc *pUnit, int32_t nUnused1, int32_t nComponent, int32_t nAnimFrame, D2BoundingBoxStrc *pBox, int32_t nUnused2)
{
    D2GfxDataStrc tParams;
    D2GfxCellStrc *pCel;
    int32_t nClass;
    int32_t nOffset;
    int32_t nBottom;
    int32_t nTop;
    int32_t nWidth;
    int32_t nHeight;
    int32_t nPad;
    int32_t nShiftX;
    int32_t nShiftY;

    if (!GFXUTIL_UnitHasComponent(pUnit, nComponent))
        return FALSE;

    memset(&tParams, 0, sizeof(tParams));
    if (!GFXUTIL_FillGfxData(&tParams, pUnit, 0, D2COMMON_UNITS_GetDirection(pUnit), nAnimFrame, &nClass, nComponent, -1))
    {
        return FALSE;
    }

    if (!D2CMP_SpriteValidate(&tParams, FALSE, TRUE))
        return FALSE;

    pCel = tParams.pCurrentCell;
    if (!pCel)
        return FALSE;

    if ((uint32_t)nAnimFrame > (uint32_t)tParams.nMaxFrames)
    {
        FOG_DisplayAssert((uint32_t)"(UINT)nAnimFrame <= tCelContext.dwCelsPerDirection", (uint32_t)szCUnitCpp, 1501);
        exit(-1);
    }

    nOffset = D2CMP_CelGetOffsetX(pCel);
    pBox->nLeft = nOffset + D2COMMON_UNITS_GetClientCoordX(pUnit) - 12;
    pBox->nRight = D2CMP_CelGetWidth(pCel) + pBox->nLeft + 24;

    nOffset = D2CMP_CelGetOffsetY(pCel);
    nBottom = D2COMMON_UNITS_GetClientCoordY(pUnit) + nOffset + 12;
    pBox->nBottom = nBottom;
    nTop = nBottom - D2CMP_CelGetHeight(pCel) - 24;
    pBox->nTop = nTop;

    nWidth = pBox->nRight - pBox->nLeft;
    if (nWidth < 32)
    {
        nPad = (32 - nWidth) >> 1;
        pBox->nRight = pBox->nRight + nPad;
        pBox->nLeft = pBox->nLeft - nPad;
    }

    nHeight = nBottom - nTop;
    if (nHeight < 32)
    {
        nPad = (32 - nHeight) >> 1;
        pBox->nTop = pBox->nTop - nPad;
        pBox->nBottom = nBottom + nPad;
    }

    nShiftX = PARTICLE_GetUnitParticleOffsetX(pUnit);
    nShiftY = PARTICLE_GetUnitParticleOffsetY(pUnit);
    nShiftY += PARTICLE_GetUnitParticleOffsetZ(pUnit);

    pBox->nLeft += nShiftX;
    pBox->nRight += nShiftX;
    pBox->nBottom += nShiftY;
    pBox->nTop += nShiftY;
    return TRUE;
}


//1.10f: D2Client.0x6FB28F20
//  Runs the select proc of one skill: skills.txt SelectProc for nSkillId, called out of gpSkillSelectProcs. The original inlines
//  DATATBLS_GetSkillsTxtRecordById for the record: sign test, count at +0xBA0, table at +0xB98, a 572-byte stride as `lea`/`shl 4`/`sub`, then
//  the caller's NULL test.
//  ecx and edx pass straight through: the body never writes either, and the proc it calls receives the caller's pPlayer and pUnit in them
//  (`call gpSkillSelectProcs[eax*4]`, 0x6FB28F65). The C passes the two on, which is the same.
//  The index is compared signed with gnSkillSelectProcCount (`jge`, 0x6FB28F63), after a zero index is refused (`test al, al` / `jbe`).
BOOL __fastcall CUNIT_SkillsRunSelectProcById(D2UnitStrc *pPlayer, D2UnitStrc *pUnit, int32_t nSkillId)
{
    D2SkillsTxt *pSkillsTxt;
    int32_t nSelectProc;

    pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);
    if (!pSkillsTxt)
        return FALSE;

    nSelectProc = pSkillsTxt->nSelectProc;
    if (nSelectProc == 0)
        return FALSE;

    if (nSelectProc >= gnSkillSelectProcCount)
        return FALSE;

    if (gpSkillSelectProcs[nSelectProc]((int32_t)pPlayer, pUnit))
        return TRUE;

    return FALSE;
}


//1.10f: D2Client.0x6FB28F90
//  Aims a unit's path at another unit, named by the server's id. The target is looked up in gpUnitTableById and whatever comes back -- the
//  unit, or NULL when the id is not there -- goes to D2Common's UNITS_SetTargetUnitForDynamicUnit, so a stale id clears the target.
//  The lookup is CUNIT_FindUnitByGuidAndType inlined in the original: the row `shl 7`, the bucket `and 7Fh`, the id at +0x0C, the type,
//  FOG_Trace, the type re-read after it (0x6FB28FCA) and the line-88 assert, the chain on pListNext. A found unit jumps straight to the D2Common
//  call (0x6FB28FBC -> 0x6FB28FDF); an exhausted chain clears esi first (0x6FB28FDD), which is the inlined function's `return NULL`. The
//  FOG_Trace is live: it runs when the id matches and the type does not, and it is the last thing the process prints before the assert ends it.
//  `retn 4`: the id is the one stack argument, the type rides in edx. The id is compared for equality only, so the `int` and the walk's
//  `unsigned int` cannot disagree.
void __fastcall CUNIT_SetUnitPathTargetById(D2UnitStrc *pUnit, D2C_UnitTypes nTargetType, int32_t nTargetId)
{
    D2UnitStrc *pTarget;

    pTarget = CUNIT_FindUnitByGuidAndType((uint32_t)nTargetId, nTargetType);

    D2COMMON_UNITS_SetTargetUnitForDynamicUnit(pUnit, pTarget);
}


//1.10f: D2Client.0x6FB29010
//  Turns a unit towards whatever its path targets. bSnap 1 sets the facing at once and 0 turns towards it over time (D2Common #10216's own
//  `if (a4)`). Nothing happens without a target, or when the target is the unit itself.
//  The head is CUNIT_GetPathTargetUnit inlined in the original: the first assert is "hPath" at CUnit.cpp:2372 (that function's own line) and the
//  second "hPath != NULL" at :2016 (this body's). Matched 0x6FB29017..0x6FB29042 against 0x6FB296A3..0x6FB296CE: UNITMODE_ClearStaleTargetUnit,
//  the path at +0x2C, the assert, PATH_GetTargetUnit.
//  The path is read again after it (0x6FB29059) and asserted again: the call went out of the module, and the source asked twice.
//  The target's position is UNITS_GetYPosition and UNITS_GetXPosition inlined in the original, in that order (Y at 0x6FB2907E, X at 0x6FB290AB,
//  the order MSVC gives the arguments of one call, right to left), and the locals keep PATH_GetYPosition ahead of PATH_GetXPosition. The type
//  tests are signed (`jle`); the static arm reads tGameCoords (+0xC, +0x10). The compiler copied the final call into each of the three X arms
//  (0x6FB290CD, 0x6FB290DD, 0x6FB290F1); the source has one.
void __fastcall CUNIT_FaceUnitAtPathTarget(D2UnitStrc *pUnit, int32_t bSnap)
{
    D2DynamicPathStrc *hPath;
    D2UnitStrc *pTarget;
    int32_t nX;
    int32_t nY;

    pTarget = CUNIT_GetPathTargetUnit(pUnit);

    if (!pTarget || pTarget == pUnit)
        return;

    hPath = pUnit->pDynamicPath;
    if (!hPath)
    {
        FOG_DisplayAssert((uint32_t)"hPath != NULL", (uint32_t)szCUnitCpp, 2016);
        exit(-1);
    }

    nY = UNITS_GetYPosition(pTarget);

    nX = UNITS_GetXPosition(pTarget);

    D2COMMON_PATH_FaceCoords(hPath, nX, nY, bSnap);
}


//1.10f: D2Client.0x6FB29100
//  Puts a unit in mode 1 and turns it to face the local player, over time (snap 0). Its caller, NPCMENU_BeginInteraction, passes the NPC, so
//  the mode is MONMODE_NEUTRAL.
//  Both loads come before the mode change: the unit's path at 0x6FB29101 and gpPlayerUnit at 0x6FB29105, then the assert (:2033), then
//  UNITS_ChangeAnimMode (0x6FB29131). A call out of the module could have changed either, so the source read both first, and the C does too.
//  The position is UNITS_GetYPosition and UNITS_GetXPosition inlined in the original, Y before X, as in CUNIT_FaceUnitAtPathTarget.
void __fastcall CUNIT_TurnUnitToFacePlayer(D2UnitStrc *pUnit)
{
    D2DynamicPathStrc *hPath;
    D2UnitStrc *pPlayer;
    int32_t nX;
    int32_t nY;

    hPath = pUnit->pDynamicPath;
    pPlayer = gpPlayerUnit;
    if (!hPath)
    {
        FOG_DisplayAssert((uint32_t)"hPath != NULL", (uint32_t)szCUnitCpp, 2033);
        exit(-1);
    }

    D2COMMON_UNITS_ChangeAnimMode(pUnit, MONMODE_NEUTRAL);

    //  BUG: pPlayer is used untested from here: the inlined UNITS_GetYPosition reads its type at 0x6FB29136 with nothing in between, where the
    //  twin CUNIT_TurnPlayerToFaceUnit tests gpPlayerUnit before anything else. LATENT: the one path in is packet 0x28 (SCMD_HandleSrv28_NpcMenu),
    //  during a game; the one caller dereferences the player itself right after this call, untested there as well.
    nY = UNITS_GetYPosition(pPlayer);

    nX = UNITS_GetXPosition(pPlayer);

    D2COMMON_PATH_FaceCoords(hPath, nX, nY, 0);
}


static_assert(PLRMODE_NEUTRAL == 1 && PLRMODE_TOWNNEUTRAL == 5,
              "the stand-still test: `cmp ecx, 1` 0x6FB291D4, `cmp ecx, 5` "
              "0x6FB291D9");

//1.10f: D2Client.0x6FB291C0
//  Turns the local player to face a unit, over time (snap 0) -- only while the player stands still: PLRMODE_NEUTRAL or PLRMODE_TOWNNEUTRAL,
//  `cmp ecx, 1` and `cmp ecx, 5` (0x6FB291D4, 0x6FB291D9).
//  gpPlayerUnit is loaded once (0x6FB291C0) and tested; the mode and the path are both read through that load. No mode change here, unlike the
//  twin above, and the assert is :2048. The target's position is UNITS_GetYPosition and UNITS_GetXPosition inlined in the original, Y before X.
void __fastcall CUNIT_TurnPlayerToFaceUnit(D2UnitStrc *pUnit)
{
    D2UnitStrc *pPlayer;
    D2DynamicPathStrc *hPath;
    int32_t nX;
    int32_t nY;

    pPlayer = gpPlayerUnit;
    if (!pPlayer)
        return;

    if (pPlayer->dwAnimMode != PLRMODE_NEUTRAL && pPlayer->dwAnimMode != PLRMODE_TOWNNEUTRAL)
    {
        return;
    }

    hPath = pPlayer->pDynamicPath;
    if (!hPath)
    {
        FOG_DisplayAssert((uint32_t)"hPath != NULL", (uint32_t)szCUnitCpp, 2048);
        exit(-1);
    }

    nY = UNITS_GetYPosition(pUnit);

    nX = UNITS_GetXPosition(pUnit);

    D2COMMON_PATH_FaceCoords(hPath, nX, nY, 0);
}


static_assert(PLRMODE_DEATH == 0 && PLRMODE_GETHIT == 4 && PLRMODE_ATTACK1 == 7
                  && PLRMODE_ATTACK2 == 8 && PLRMODE_BLOCK == 9
                  && PLRMODE_CAST == 10 && PLRMODE_THROW == 11
                  && PLRMODE_KICK == 12 && PLRMODE_KNOCKBACK == 19,
              "the modes the index table at 0x6FB29354 sends to FALSE, "
              "0x6FB292F0; DEAD's 17 and SEQUENCE's 18 are pinned above");
static_assert(PLRMODE_SPECIAL1 == 13 && PLRMODE_SPECIAL2 == 14
                  && PLRMODE_SPECIAL3 == 15 && PLRMODE_SPECIAL4 == 16,
              "the same table's per-class arms: 13 0x6FB2930A, 14 0x6FB29314, "
              "15 and 16 0x6FB2931E");
static_assert(offsetof(D2SkillsTxt, nAnim) == 0x10
                  && offsetof(D2SkillsTxt, nSeqInput) == 0x16,
              "`cmp byte ptr [eax+10h], 12h` 0x6FB292DB, `mov al, [eax+16h]` "
              "0x6FB292E1");

//1.10f: D2Client.0x6FB29290
//  May the player start a new mouse command now. FALSE while an item is on the cursor (INVENTORY_GetCursorItem answers NULL for a NULL
//  inventory) and for a unit that is not a player; then a switch on the mode.
//  The second argument is there and is not read. Every call site loads edx with the skill it has just asked D2Common for (`mov edx, eax` after
//  UNITS_GetLeftSkill or GetRightSkill) and stores the same value to memory separately, so the register load serves only the call. The body
//  never reads edx: its one touch is a write (0x6FB292F4), on a path that has made three calls by then.
//  The switch: a 20-byte index table at 0x6FB29354 over six targets at 0x6FB2933C, bounded unsigned (`cmp eax, 13h` / `ja`):
//
//      0 4 7 8 9 10 11 12 17 19   FALSE, 0x6FB292F0
//      1 2 3 5 6, and above 19    TRUE, the default's own target 0x6FB29334
//      13 SPECIAL1                FALSE for the paladin only
//      14 SPECIAL2                FALSE for the assassin only
//      15 16 SPECIAL3, SPECIAL4   FALSE for barbarian, assassin and druid, tested in that order: 4, 6, 5
//      18 SEQUENCE                the used skill's skills.txt row, below
//
//  Modes 1, 2, 3, 5 and 6 share the default's target, so whether the source named them cannot be told; the C lets them fall through to TRUE.
//  Sequence, in full. No used skill: TRUE. A used skill without a skills.txt record: FALSE -- the `retn` at 0x6FB292DA returns the NULL itself,
//  eax already 0. A record whose Anim is not SQ (18): TRUE. SeqInput 0 or 0xFF: FALSE. Otherwise `dwSeqFrame >> 8 >= SeqInput`, signed on both
//  counts -- `sar edx, 8` on the int frame and `setnl` -- against the byte zero-extended (`and eax, 0FFh`).
BOOL __fastcall CUNIT_CanPlayerIssueCommand(D2UnitStrc *pPlayer, D2SkillStrc *pUnusedSkill)
{
    D2SkillStrc *pSkill;
    D2SkillsTxt *pSkillsTxt;
    uint8_t nSeqInput;

    if (D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory))
        return FALSE;

    if (pPlayer->dwUnitType != UNIT_PLAYER)
        return FALSE;

    switch (pPlayer->dwAnimMode)
    {
    case PLRMODE_DEATH:
    case PLRMODE_GETHIT:
    case PLRMODE_ATTACK1:
    case PLRMODE_ATTACK2:
    case PLRMODE_BLOCK:
    case PLRMODE_CAST:
    case PLRMODE_THROW:
    case PLRMODE_KICK:
    case PLRMODE_DEAD:
    case PLRMODE_KNOCKBACK:
        return FALSE;

    case PLRMODE_SPECIAL1:
        if (pPlayer->dwClassId == PCLASS_PALADIN)
            return FALSE;
        break;

    case PLRMODE_SPECIAL2:
        if (pPlayer->dwClassId == PCLASS_ASSASSIN)
            return FALSE;
        break;

    case PLRMODE_SPECIAL3:
    case PLRMODE_SPECIAL4:
        if (pPlayer->dwClassId == PCLASS_BARBARIAN || pPlayer->dwClassId == PCLASS_ASSASSIN || pPlayer->dwClassId == PCLASS_DRUID)
        {
            return FALSE;
        }
        break;

    case PLRMODE_SEQUENCE:
        pSkill = D2COMMON_UNITS_GetUsedSkill(pPlayer);
        if (!pSkill)
            break;

        pSkillsTxt = D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill(pSkill);
        if (!pSkillsTxt)
            return FALSE;

        if (pSkillsTxt->nAnim != PLRMODE_SEQUENCE)
            break;

        nSeqInput = pSkillsTxt->nSeqInput;
        if (nSeqInput == 0 || nSeqInput == 0xFF)
            return FALSE;

        return (pPlayer->dwSeqFrame >> 8) >= (int32_t)nSeqInput;
    }

    return TRUE;
}


//1.10f: D2Client.0x6FB29370
//  The room the player is in, or NULL before there is a player. The null test is on the unit and not on the room, so a live player in no room
//  still returns whatever UNITS_GetRoom answers -- which is NULL too, but by a different route. Reproduced as written rather than folded,
//  because the two paths are distinguishable in the original and only one of them calls out of the module.
//  No arguments, so the calling convention cannot be seen in the bytes; __cdecl is followed.
D2ActiveRoomStrc *__cdecl CUNIT_GetPlayerRoom()
{
    if (!gpPlayerUnit)
        return NULL;

    return D2COMMON_UNITS_GetRoom(gpPlayerUnit);
}


//1.10f: D2Client.0x6FB29390
//  When the period of day changes, re-runs every object's day/night mode. ENVIRONMENT_GetPeriodOfDayFromAct(gpAct, NULL) is compared with
//  gnLastPeriodOfDay: equal, nothing; different, store it and walk the UNIT_OBJECT row (row 2 of 128 dwords) of both unit tables, calling
//  OBJECT_ApplyDayNightMode(pObject, 0) on each unit (its `int` argument is not read).
//  Each walk reads the successor before the call, so the callee may unlink the object, and asserts the type of every unit -- :2186 by id, :2196
//  by client id. The type is loaded before the successor (0x6FB293B7, 0x6FB293B9): two loads with no store between them, so the order is the
//  compiler's, and the C reads the successor first as the walks above do.
//  The bucket loops end `cmp edi, offset ...+600h` / `jl` (0x6FB293D4, 0x6FB29404): a signed compare of two addresses, the trace of an `int`
//  index strength-reduced into a pointer, as in CUNIT_UpdateAllUnits, and written with the index.
void __cdecl CUNIT_RefreshObjectsOnDayNightChange()
{
    D2UnitStrc *pObject;
    D2UnitStrc *pNext;
    int32_t nPeriodOfDay;
    int32_t iBucket;

    nPeriodOfDay = D2COMMON_ENVIRONMENT_GetPeriodOfDayFromAct(gpAct, NULL);
    if (nPeriodOfDay == gnLastPeriodOfDay)
        return;

    gnLastPeriodOfDay = nPeriodOfDay;

    for (iBucket = 0; iBucket < 128; ++iBucket)
    {
        pObject = gpUnitTableById[UNIT_OBJECT][iBucket];
        while (pObject)
        {
            pNext = pObject->pListNext;
            if (pObject->dwUnitType != UNIT_OBJECT)
            {
                FOG_DisplayAssert((uint32_t)"ptUnit->eType == UNIT_OBJECT", (uint32_t)szCUnitCpp, 2186);
                exit(-1);
            }

            OBJECT_ApplyDayNightMode(pObject, FALSE);
            pObject = pNext;
        }
    }

    for (iBucket = 0; iBucket < 128; ++iBucket)
    {
        pObject = gpUnitTableByClientId[UNIT_OBJECT][iBucket];
        while (pObject)
        {
            pNext = pObject->pListNext;
            if (pObject->dwUnitType != UNIT_OBJECT)
            {
                FOG_DisplayAssert((uint32_t)"ptUnit->eType == UNIT_OBJECT", (uint32_t)szCUnitCpp, 2196);
                exit(-1);
            }

            OBJECT_ApplyDayNightMode(pObject, FALSE);
            pObject = pNext;
        }
    }
}


//1.10f: D2Client.0x6FB29450
//  The unit's server-packet queue, D2UnitStrc +0xD8: the client arm of the union whose server arm is dwSrvTickCount. Asserts "ptUnit" (:2211).
D2PacketListStrc *__fastcall CUNIT_GetUnitPacketQueue(D2UnitStrc *pUnit)
{
    if (!pUnit)
    {
        FOG_DisplayAssert((uint32_t)"ptUnit", (uint32_t)szCUnitCpp, 2211);
        exit(-1);
    }

    return pUnit->pPacketList;
}


//1.10f: D2Client.0x6FB29480
//  Its setter, asserting "ptUnit" at :2221: one `mov [ecx+0D8h], edx`.
void __fastcall CUNIT_SetUnitPacketQueue(D2UnitStrc *pUnit, D2PacketListStrc *pQueue)
{
    if (!pUnit)
    {
        FOG_DisplayAssert((uint32_t)"ptUnit", (uint32_t)szCUnitCpp, 2221);
        exit(-1);
    }

    pUnit->pPacketList = pQueue;
}


static_assert(MONSTATS2FLAGINDEX_ISATT == 9 && MONSTATS2FLAGINDEX_UNFLATDEAD == 20,
              "monstats2 byte 1 and gdwBitMasks[1] at 0x6FB2951E/0x6FB29526, "
              "byte 2 and gdwBitMasks[4] at 0x6FB2963E/0x6FB29646");
static_assert(MONMODE_DEATH == 0 && MONMODE_DEAD == 12,
              "`test edx, edx` / `cmp edx, 0Ch` at 0x6FB29569..0x6FB2956D, "
              "`cmp esi, 0Ch` at 0x6FB2964F");

//1.10f: D2Client.0x6FB294B0
//  Clears the dead flag, and sets the attackable one where it applies.
//  UNITFLAG_CANBEATTACKED is only ever set here, never cleared. The mask at 0x6FB294BA clears UNITFLAG_ISDEAD alone; the cleared word is stored
//  at once (0x6FB294C4), and `or edi, 4` (0x6FB2952F) adds the attackable bit to that same word for any unit that is not a monster, and for a
//  monster whose monstats2.txt row carries ISATT. A monster without it keeps whatever bit 2 it had.
//  The row comes from DATATBLS_GetMonStats2TxtRecord, which the original inlines: the class's sign and bound tests, wMonStatsEx sign-extended
//  (`movsx`) and tested the same way, the 308-byte stride, then the caller's NULL test (0x6FB29518). The masked flag byte is also stored to a
//  frame slot nothing reads (`mov [esp+0Ch+var_4], edx`, 0x6FB29529), a local the compiler kept in memory; the C has none.
//  pUnit is not tested, where CUNIT_SetUnitDead below tests it.
void __fastcall CUNIT_SetUnitAlive(D2UnitStrc *pUnit)
{
    D2MonStats2Txt *pMonStats2Txt;
    uint32_t dwFlags;

    dwFlags = pUnit->dwFlags & ~UNITFLAG_ISDEAD;
    pUnit->dwFlags = dwFlags;

    if (pUnit->dwUnitType == UNIT_MONSTER)
    {
        pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pUnit->dwClassId);

        if (!pMonStats2Txt)
            return;

        if (!(pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_ISATT >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_ISATT & 7]))
        {
            return;
        }
    }

    pUnit->dwFlags = dwFlags | UNITFLAG_CANBEATTACKED;
}


//1.10f: D2Client.0x6FB29540
//  Sets the dead flag: `or dword ptr [ecx+0C4h], 10000h` behind a NULL test, fifteen bytes.
void __fastcall CUNIT_SetUnitDead(D2UnitStrc *pUnit)
{
    if (pUnit)
        pUnit->dwFlags |= UNITFLAG_ISDEAD;
}


//1.10f: D2Client.0x6FB29550
//  Is the unit dead or dying: UNITFLAG_ISDEAD, or a player in PLRMODE_DEATH or PLRMODE_DEAD, or a monster in MONMODE_DEATH or MONMODE_DEAD;
//  every other type is alive.
//  The type is dispatched `sub eax, 0` / `jz`, then `dec eax` / `jnz` (0x6FB29561, 0x6FB29566) -- a two-case switch, written as one. The mode
//  is loaded before it (0x6FB2955E) and compared for equality only, 0 first.
BOOL __fastcall CUNIT_IsUnitDead(D2UnitStrc *pUnit)
{
    uint32_t dwAnimMode;

    if (pUnit->dwFlags & UNITFLAG_ISDEAD)
        return TRUE;

    dwAnimMode = pUnit->dwAnimMode;
    switch (pUnit->dwUnitType)
    {
    case UNIT_PLAYER:
        if (dwAnimMode == PLRMODE_DEATH || dwAnimMode == PLRMODE_DEAD)
            return TRUE;
        break;

    case UNIT_MONSTER:
        if (dwAnimMode == MONMODE_DEATH || dwAnimMode == MONMODE_DEAD)
            return TRUE;
        break;
    }

    return FALSE;
}


//1.10f: D2Client.0x6FB29590
//  Is the unit drawn under the others, flat on the floor. By type, through a table (0x6FB29684, five cases, bounded unsigned -- `cmp eax, 4` /
//  `ja`):
//
//      0 player    in PLRMODE_DEAD
//      1 monster   in MONMODE_DEAD, unless its monstats2.txt row carries UNFLATDEAD
//      2 object    objects.txt DrawUnder bit 1, or bit 0 while OBJMODE_OPENED
//      3 missile   UNITFLAG_ISDEAD
//      4 item      IMODE_ONGROUND
//
//  and FALSE for a NULL unit or any other type. The monster's row is DATATBLS_GetMonStats2TxtRecord inlined in the original as in
//  CUNIT_SetUnitAlive, with the same flag byte kept in a frame slot (0x6FB29649); a lookup that fails reads as "no UNFLATDEAD". The objects.txt
//  record is D2Common's, which asserts its range and never returns NULL.
BOOL __fastcall CUNIT_ShouldDrawUnitUnder(D2UnitStrc *pUnit)
{
    D2MonStats2Txt *pMonStats2Txt;
    D2ObjectsTxt *pObjectsTxt;
    uint32_t dwAnimMode;

    if (!pUnit)
        return FALSE;

    dwAnimMode = pUnit->dwAnimMode;
    switch (pUnit->dwUnitType)
    {
    case UNIT_PLAYER:
        return dwAnimMode == PLRMODE_DEAD;

    case UNIT_MONSTER:
        pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(pUnit->dwClassId);

        if (pMonStats2Txt && (pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_UNFLATDEAD >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_UNFLATDEAD & 7]))
        {
            return FALSE;
        }

        return dwAnimMode == MONMODE_DEAD;

    case UNIT_OBJECT:
        pObjectsTxt = D2COMMON_DATATBLS_GetObjectsTxtRecord(pUnit->dwClassId);
        if (pObjectsTxt->nDrawUnder & 2)
            return TRUE;

        return dwAnimMode == OBJMODE_OPENED && (pObjectsTxt->nDrawUnder & 1);

    case UNIT_MISSILE:
        return (pUnit->dwFlags & UNITFLAG_ISDEAD) != 0;

    case UNIT_ITEM:
        return dwAnimMode == IMODE_ONGROUND;
    }

    return FALSE;
}


//1.10f: D2Client.0x6FB296A0
//  The unit this one's path is aimed at, a stale target dropped first (UNITMODE_ClearStaleTargetUnit). Asserts "hPath" (:2372).
D2UnitStrc *__fastcall CUNIT_GetPathTargetUnit(D2UnitStrc *pUnit)
{
    D2DynamicPathStrc *hPath;

    UNITMODE_ClearStaleTargetUnit(pUnit);
    hPath = pUnit->pDynamicPath;
    if (!hPath)
    {
        FOG_DisplayAssert((uint32_t)"hPath", (uint32_t)szCUnitCpp, 2372);
        exit(-1);
    }

    return D2COMMON_PATH_GetTargetUnit(hPath);
}


//1.10f: D2Client.0x6FB296E0
//  Drops the unit's light: D2UnitStrc +0x64, the client arm pLight, handed to DLIGHTMAP_RemoveLight and cleared; tested for NULL first, so a
//  second call does nothing. The body reads the unit's +0x64 (0x6FB296E3) and clears it (0x6FB296EF), as CUNIT_DestroyUnit does to the same member.
void __fastcall CUNIT_GfxFreeUnitLightMap(D2UnitStrc *pUnit)
{
    if (pUnit->pLight)
    {
        DLIGHTMAP_RemoveLight(pUnit->pLight);
        pUnit->pLight = NULL;
    }
}


//1.10f: D2Client.0x6FB29700
//  Calls pfnUnitFunction on every player unit in gpUnitTableById, the unit in ecx. The callback is asserted (:2399), each successor read before
//  the call -- the callback may free the unit -- and the 128 buckets stepped by an `int` index strength-reduced into a pointer: `cmp ebx, offset
//  gpUnitTableById+200h` / `jl` (0x6FB29743), signed, as in CUNIT_UpdateAllUnits.
//  The monster and object forms below are the same body over rows 1 and 2, asserting at :2410 and :2421. CUNIT_ForEachUnitInTable is the
//  general walk, and these are not copies of it: it asserts nothing.
void __fastcall CUNIT_ForEachPlayerUnit(void (__fastcall *pfnUnitFunction)(D2UnitStrc *pUnit))
{
    D2UnitStrc *pUnit;
    D2UnitStrc *pNext;
    int32_t iBucket;

    if (!pfnUnitFunction)
    {
        FOG_DisplayAssert((uint32_t)"pfnUnitFunction", (uint32_t)szCUnitCpp, 2399);
        exit(-1);
    }

    for (iBucket = 0; iBucket < 128; ++iBucket)
    {
        pUnit = gpUnitTableById[UNIT_PLAYER][iBucket];
        while (pUnit)
        {
            pNext = pUnit->pListNext;
            pfnUnitFunction(pUnit);
            pUnit = pNext;
        }
    }
}


//1.10f: D2Client.0x6FB29750
//  The same over the monster row, asserting at :2410 (`jl` 0x6FB29799).
void __fastcall CUNIT_ForEachMonsterUnit(void (__fastcall *pfnUnitFunction)(D2UnitStrc *pUnit))
{
    D2UnitStrc *pUnit;
    D2UnitStrc *pNext;
    int32_t iBucket;

    if (!pfnUnitFunction)
    {
        FOG_DisplayAssert((uint32_t)"pfnUnitFunction", (uint32_t)szCUnitCpp, 2410);
        exit(-1);
    }

    for (iBucket = 0; iBucket < 128; ++iBucket)
    {
        pUnit = gpUnitTableById[UNIT_MONSTER][iBucket];
        while (pUnit)
        {
            pNext = pUnit->pListNext;
            pfnUnitFunction(pUnit);
            pUnit = pNext;
        }
    }
}


//1.10f: D2Client.0x6FB297A0
//  The same over the object row, asserting at :2421 (`jl` 0x6FB297E9). The by-id table only -- where CUNIT_RefreshObjectsOnDayNightChange walks both.
void __fastcall CUNIT_ForEachObjectUnit(void (__fastcall *pfnUnitFunction)(D2UnitStrc *pUnit))
{
    D2UnitStrc *pUnit;
    D2UnitStrc *pNext;
    int32_t iBucket;

    if (!pfnUnitFunction)
    {
        FOG_DisplayAssert((uint32_t)"pfnUnitFunction", (uint32_t)szCUnitCpp, 2421);
        exit(-1);
    }

    for (iBucket = 0; iBucket < 128; ++iBucket)
    {
        pUnit = gpUnitTableById[UNIT_OBJECT][iBucket];
        while (pUnit)
        {
            pNext = pUnit->pListNext;
            pfnUnitFunction(pUnit);
            pUnit = pNext;
        }
    }
}


static_assert(OBJMODE_NEUTRAL == 0 && OBJMODE_SPECIAL4 == 6,
              "the door's `test eax, eax` 0x6FB298F8 and `cmp eax, 6` 0x6FB2990D");
static_assert(STR_IDX_3225_Door == 3225 && STR_IDX_3227_ODoor == 3227
                  && STR_IDX_3228_BlockedDoor == 3228
                  && STR_IDX_3229_LockedDoor == 3229
                  && STR_IDX_3262_lockedchest == 3262
                  && STR_IDX_10809_ShrId0 == 10809,
              "`mov ecx, 0C99h/0C9Bh/0C9Ch/0C9Dh/0CBEh`, `add ecx, 2A39h` "
              "(0x6FB298E9..0x6FB2996B)");
static_assert(offsetof(D2ObjectsTxt, wszName) == 0x40
                  && offsetof(D2ObjectsTxt, nLockable) == 0x171
                  && offsetof(D2ObjectsTxt, nDrawUnder) == 0x1B7
                  && offsetof(D2ObjectsTxt, nOpenWarp) == 0x1B8
                  && offsetof(D2MonsterDataStrc, wszMonName) == 0x2C,
              "`lea eax, [ecx+40h]` 0x6FB2997E, `[ecx+171h]` 0x6FB2995A, "
              "`[eax+1B7h]` 0x6FB295B8, `[ecx+1B8h]` 0x6FB29831, `[eax+2Ch]` "
              "0x6FB29991");

//1.10f: D2Client.0x6FB297F0
//  The name shown for a unit. Asserts "ptUnit" (:2459), then a switch whose table is at 0x6FB29A1C (six cases, `cmp eax, 5` / `ja`): monsters,
//  objects and items are named; players, missiles and room tiles get NULL, as does any type above 5.
//
//  Monster: the monster data's wszMonName (+0x2C, the client arm of its union) when Unicode::strlen says it is not empty -- a NULL monster data
//  hands strlen a NULL, and D2Lang answers 0 for it -- else monstats.txt NameStr through D2Lang. The record comes from
//  DATATBLS_GetMonStatsTxtRecord, which the original inlines with its NULL not tested.
//
//  Object, in the order the bytes take it:
//
//    * OpenWarp in objects.txt and the object OBJMODE_OPENED: the first UNIT_TILE in its room's unit list, else in each adjacent room but its
//      own -- the index compared unsigned (`jb` 0x6FB298A5) -- goes to DUNGEON_GetWarpDestinationLevel with the room it was found in. A level
//      gives its levels.txt LevelWarp text, 0 gives NULL, and no tile at all falls through to the door test.
//    * IsDoor: string.tbl 3225 'Closed Door' in OBJMODE_NEUTRAL, 3228 'Blocked Door' in SPECIAL2 and SPECIAL3, 3229 'Locked Door' in SPECIAL4,
//      else 3227 'Open Door'. The bytes compare 5, 4, 0, 6.
//    * a shrine (D2Common's UNITS_IsShrine): 10809 ShrId0 plus the object data's InteractType, zero-extended into cx (0x6FB29941).
//    * Lockable in objects.txt and InteractType's top bit set (`jns`, 0x6FB29969): 3262 'Locked Chest'; otherwise the objects.txt Name itself,
//      +0x40 of the record.
//
//  Item: items.txt NameStr through D2Lang, from D2Common's DATATBLS_GetItemsTxtRecord.
//
//  Also untested: the room D2Common's UNITS_GetRoom gives an object (its static path's), and the levels.txt record -- the level id comes from
//  DUNGEON_GetWarpDestinationLevel and 0 is refused first.
Unicode *__fastcall CUNIT_GetUnitName(D2UnitStrc *pUnit)
{
    D2ActiveRoomStrc *pRoom;
    D2ActiveRoomStrc *pWarpRoom;
    D2ActiveRoomStrc **ppRoomList;
    D2UnitStrc *pTile;
    D2ObjectDataStrc *pObjectData;
    D2MonStatsTxt *pMonStatsTxt;
    D2ItemsTxt *pItemsTxt;
    Unicode *pwszName;
    D2C_StringIndices nDoorStr;
    uint32_t iRoom;
    int32_t nNumRooms;
    int32_t nClassId;
    int32_t nDestLevel;

    if (!pUnit)
    {
        FOG_DisplayAssert((uint32_t)"ptUnit", (uint32_t)szCUnitCpp, 2459);
        exit(-1);
    }

    switch (pUnit->dwUnitType)
    {
    case UNIT_MONSTER:
        pwszName = pUnit->pMonsterData ? pUnit->pMonsterData->wszMonName : NULL;
        if (Unicode::strlen(pwszName))
            return pwszName;

        pMonStatsTxt = NULL;
        nClassId = pUnit->dwClassId;
        if (nClassId >= 0 && nClassId < sgptDataTables->nMonStatsTxtRecordCount)
            pMonStatsTxt = &sgptDataTables->pMonStatsTxt[nClassId];

        //  BUG: the monstats.txt record is read with no NULL test: out of range, `xor eax, eax` / `mov cx, [eax+6]` (0x6FB299DB) reads address 6.
        //  LATENT: every class a client monster can hold is bounded where it is written.
        return (Unicode *)D2LANG_GetStringFromTblIndex((D2C_StringIndices)pMonStatsTxt->wNameStr);

    case UNIT_OBJECT:
        if (pUnit->pObjectData->pObjectTxt->nOpenWarp && pUnit->dwAnimMode == OBJMODE_OPENED)
        {
            pRoom = D2COMMON_UNITS_GetRoom(pUnit);
            pWarpRoom = NULL;
            for (pTile = pRoom->pUnitFirst; pTile; pTile = pTile->pRoomNext)
            {
                if (pTile->dwUnitType == UNIT_TILE)
                {
                    pWarpRoom = pRoom;
                    break;
                }
            }

            if (!pWarpRoom)
            {
                D2COMMON_DUNGEON_GetAdjacentRoomsListFromRoom(pRoom, &ppRoomList, &nNumRooms);
                for (iRoom = 0; !pWarpRoom && iRoom < (uint32_t)nNumRooms; ++iRoom)
                {
                    if (ppRoomList[iRoom] == pRoom)
                        continue;

                    for (pTile = ppRoomList[iRoom]->pUnitFirst; pTile; pTile = pTile->pRoomNext)
                    {
                        if (pTile->dwUnitType == UNIT_TILE)
                        {
                            pWarpRoom = ppRoomList[iRoom];
                            break;
                        }
                    }
                }
            }

            if (pWarpRoom)
            {
                nDestLevel = D2COMMON_DUNGEON_GetWarpDestinationLevel(pWarpRoom, pTile->dwClassId);
                if (!nDestLevel)
                    return NULL;

                return D2COMMON_DATATBLS_GetLevelsTxtRecord(nDestLevel)->wszLevelWarp;
            }
        }

        if (pUnit->pObjectData->pObjectTxt->nIsDoor)
        {
            nDoorStr = STR_IDX_3227_ODoor;
            switch (pUnit->dwAnimMode)
            {
            case OBJMODE_SPECIAL3:
            case OBJMODE_SPECIAL2:
                nDoorStr = STR_IDX_3228_BlockedDoor;
                break;

            case OBJMODE_NEUTRAL:
                return (Unicode *)D2LANG_GetStringFromTblIndex(STR_IDX_3225_Door);

            case OBJMODE_SPECIAL4:
                return (Unicode *)D2LANG_GetStringFromTblIndex(STR_IDX_3229_LockedDoor);
            }

            return (Unicode *)D2LANG_GetStringFromTblIndex(nDoorStr);
        }

        if (D2COMMON_UNITS_IsShrine(pUnit))
        {
            return (Unicode *)D2LANG_GetStringFromTblIndex((D2C_StringIndices)(pUnit->pObjectData->InteractType + STR_IDX_10809_ShrId0));
        }

        pObjectData = pUnit->pObjectData;
        if (pObjectData->pObjectTxt->nLockable && (pObjectData->InteractType & 0x80))
            return (Unicode *)D2LANG_GetStringFromTblIndex(STR_IDX_3262_lockedchest);

        return pObjectData->pObjectTxt->wszName;

    case UNIT_ITEM:
        pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(pUnit->dwClassId);

        //  BUG: the items.txt record is used with no NULL test (0x6FB299FF). The getter returns NULL at or above the record count, so an
        //  out-of-range item class faults at 0xF4 (and a negative one reads before the table: the getter bounds only from above). LATENT: item
        //  classes come from the server's own items.txt, through the code link table (0x9C, 0x9D) or raw (0x09).
        return (Unicode *)D2LANG_GetStringFromTblIndex((D2C_StringIndices)pItemsTxt->wNameStr);
    }

    return NULL;
}


static_assert(STATE_ATTACHED == 143, "`push 8Fh` at 0x6FB29A6B");

//1.10f: D2Client.0x6FB29A40
//  Spends one tick of the component-draw counter, and says whether to draw the components: not while STATE_ATTACHED. Asserts "ptUnit" (:2583).
//  A zero counter returns FALSE untouched -- the `retn` at 0x6FB29A69 hands back the zero itself.
//  +0x58 is a counter in this module: it is loaded, tested, decremented and stored as a dword (0x6FB29A62..0x6FB29A71), and
//  CUNIT_SetUnitComponentDrawCount stores the literal 3. The member is a union, and the C reads the int arm (nComponentDrawCount). The bytes do
//  not decide a sign -- `test` / `jnz` and `dec` (0x6FB29A65, 0x6FB29A6A) read the same either way. The decrement is stored before the D2Common
//  call (0x6FB29A71).
//  `neg` / `sbb` / `inc` (0x6FB29A79) is `!STATES_CheckState(...)`: 1 for 0, 0 for anything else.
BOOL __fastcall CUNIT_ShouldDrawUnitComponents(D2UnitStrc *pUnit)
{
    int32_t nCount;

    if (!pUnit)
    {
        FOG_DisplayAssert((uint32_t)"ptUnit", (uint32_t)szCUnitCpp, 2583);
        exit(-1);
    }

    nCount = pUnit->nComponentDrawCount;
    if (!nCount)
        return FALSE;

    pUnit->nComponentDrawCount = nCount - 1;
    return !D2COMMON_STATES_CheckState(pUnit, STATE_ATTACHED);
}


//1.10f: D2Client.0x6FB29A80
//  Arms the counter: +0x58 = 3 behind the "ptUnit" assert (:2610). The member is the client's arm of the +0x58 union (nComponentDrawCount).
void __fastcall CUNIT_SetUnitComponentDrawCount(D2UnitStrc *pUnit)
{
    if (!pUnit)
    {
        FOG_DisplayAssert((uint32_t)"ptUnit", (uint32_t)szCUnitCpp, 2610);
        exit(-1);
    }

    pUnit->nComponentDrawCount = 3;
}


//1.10f: D2Client.0x6FB29AB0
//  Stores the unit's start light: +0x68, the client arm dwStartLight, behind the "ptUnit" assert (:2632). The callers pass the light radius
//  DLIGHTMAP_GetRadius gives, which is in subtiles.
void __fastcall CUNIT_SetUnitStartLight(D2UnitStrc *pUnit, int32_t nStartLight)
{
    if (!pUnit)
    {
        FOG_DisplayAssert((uint32_t)"ptUnit", (uint32_t)szCUnitCpp, 2632);
        exit(-1);
    }

    pUnit->dwStartLight = (uint32_t)nStartLight;
}


//1.10f: D2Client.0x6FB29AE0
//  Empties a unit's inventory, destroying every item unit -- or, with bKeepItemsHeldElsewhere, only those no other player or monster also holds.
//  Asserts "hUnit" (:2686).
//  Each inventory entry becomes its item unit by the GUID D2Common gives for it; the original inlines CUNIT_FindUnitByGuidAndType over the item
//  row and then tests its answer for NULL (0x6FB29B8A). An entry whose unit is not in the table is skipped. The successor is read before
//  anything is freed (0x6FB29B31).
//  The search for another holder is an inline function in the original, with no out-of-line copy. Its assert, "hItem", carries line :2649
//  inside a body that begins at :2686 -- a line from above this function. It walks rows 0 and 1 of gpUnitTableById with `int` indexes -- `cmp
//  ebp, 80h` and `cmp eax, 2`, both `jl` (0x6FB29BFD, 0x6FB29C12) -- and the inventory of every unit but pUnit, comparing each entry's item
//  unit (INVENTORY_UnitIsItem, asserted) with the one being freed. Its answer is negated by a `setz` and tested again (0x6FB29C1D..0x6FB29C28);
//  the C tests it once.
//  Held elsewhere, the item is only removed from this inventory ("hRemovedItem", :2708). Otherwise it is detached from the unit
//  (ITEM_DetachItemFromUnit, both flags 0), removed (:2704), and unlinked from its table and freed (CUNIT_UnlinkAndFreeUnit).
void __fastcall CUNIT_FreeUnitInventory(D2UnitStrc *pUnit, int32_t bKeepItemsHeldElsewhere)
{
    D2InventoryStrc *pInventory;
    D2UnitStrc *pInvItem;
    D2UnitStrc *pNextItem;
    D2UnitStrc *pItemUnit;
    D2UnitStrc *pOther;
    D2UnitStrc *pOtherInvItem;
    D2UnitStrc *hItem;
    D2UnitGUID dwItemGUID;
    BOOL bHeldElsewhere;
    int32_t nRow;
    int32_t nBucket;

    if (!pUnit)
    {
        FOG_DisplayAssert((uint32_t)"hUnit", (uint32_t)szCUnitCpp, 2686);
        exit(-1);
    }

    pInventory = pUnit->pInventory;
    if (!pInventory)
        return;

    pInvItem = D2COMMON_INVENTORY_GetFirstItem(pInventory);
    while (pInvItem)
    {
        pNextItem = D2COMMON_INVENTORY_GetNextItem(pInvItem);
        dwItemGUID = D2COMMON_INVENTORY_GetItemGUID(pInvItem);

        pItemUnit = CUNIT_FindUnitByGuidAndType(dwItemGUID, UNIT_ITEM);

        //  the caller's test of the inlined lookup's answer, 0x6FB29B8A
        if (pItemUnit)
        {
            bHeldElsewhere = FALSE;
            if (bKeepItemsHeldElsewhere)
            {
                for (nRow = 0; nRow < 2; ++nRow)
                {
                    for (nBucket = 0; nBucket < 128; ++nBucket)
                    {
                        for (pOther = gpUnitTableById[nRow][nBucket]; pOther; pOther = pOther->pListNext)
                        {
                            if (pOther == pUnit || !pOther->pInventory)
                                continue;

                            for (pOtherInvItem = D2COMMON_INVENTORY_GetFirstItem(pOther->pInventory); pOtherInvItem; pOtherInvItem = D2COMMON_INVENTORY_GetNextItem(pOtherInvItem))
                            {
                                hItem = D2COMMON_INVENTORY_UnitIsItem(pOtherInvItem);
                                if (!hItem)
                                {
                                    FOG_DisplayAssert((uint32_t)"hItem", (uint32_t)szCUnitCpp, 2649);
                                    exit(-1);
                                }

                                if (hItem == pItemUnit)
                                {
                                    bHeldElsewhere = TRUE;
                                    goto search_done;
                                }
                            }
                        }
                    }
                }
            search_done:;
            }

            if (!bHeldElsewhere)
            {
                ITEM_DetachItemFromUnit(pItemUnit, pUnit, FALSE, FALSE);
                if (!D2COMMON_INVENTORY_RemoveItemFromInventory(pInventory, pItemUnit))
                {
                    FOG_DisplayAssert((uint32_t)"hRemovedItem", (uint32_t)szCUnitCpp, 2704);
                    exit(-1);
                }

                CUNIT_UnlinkAndFreeUnit(gpUnitTableById[pItemUnit->dwUnitType], pItemUnit->dwUnitId & 0x7F, pItemUnit->dwUnitId, pItemUnit->dwUnitType);
            }
            else if (!D2COMMON_INVENTORY_RemoveItemFromInventory(pInventory, pItemUnit))
            {
                FOG_DisplayAssert((uint32_t)"hRemovedItem", (uint32_t)szCUnitCpp, 2708);
                exit(-1);
            }
        }

        pInvItem = pNextItem;
    }
}


//1.10f: D2Client.0x6FB29D10
//  Adds an overlay to a unit, and plays the two steal sounds. Every caller passes 0 as the third argument.
//  The third argument is an integer, not a pointer: it is tested signed (`test eax, eax` / `jle`, 0x6FB29D26), which a pointer's NULL test is
//  not, and handed on as GFX_AddOverlay's `int nTargetOrStateId`. Positive: kind 1 with it attached; otherwise kind 2 with 0.
//  Then a switch on the overlay id -- `sub edi, 97h` / `jz`, `dec edi` / `jnz` (0x6FB29D3A) -- plays Sounds.txt 396 impact_steal_life for
//  Overlay.txt 151 steallife and 397 impact_steal_mana for 152 stealmana, with zeros for the rest.
void __fastcall CUNIT_AddOverlay(D2UnitStrc *pUnit, int32_t nOverlayId, int32_t nTargetOrStateId)
{
    if (!pUnit)
        return;

    if (nTargetOrStateId > 0)
        GFX_AddOverlay(pUnit, (D2_OverlayIds)nOverlayId, OVERLAYTYPE_COUNTER, 0, nTargetOrStateId, 0, 0, 0);
    else
        GFX_AddOverlay(pUnit, (D2_OverlayIds)nOverlayId, OVERLAYTYPE_ONCE, 0, 0, 0, 0, 0);

    switch (nOverlayId)
    {
    case 151:
        SOUNDINST_PlaySound(SOUND_IMPACT_STEAL_LIFE, pUnit, 0, 0, 0);
        break;

    case 152:
        SOUNDINST_PlaySound(SOUND_IMPACT_STEAL_MANA, pUnit, 0, 0, 0);
        break;
    }
}


static_assert(MONSTATS2FLAGINDEX_ALSEL == 4 && MONSTATS2FLAGINDEX_NOSEL == 5,
              "monstats2 byte 0 against gdwBitMasks[4] and [5]: `[ebp+10h]` "
              "0x6FB29FFF, `[ebp+14h]` 0x6FB2A065");

//1.10f: D2Client.0x6FB29D80
//  May pSource target pTarget at all -- the alignment half of the selection, before any geometry. pSource is the selecting unit and need not be a
//  player: the second arm below treats a monster source, and the missile target tests pass a missile's owner.
//  The two units' types and GUIDs are taken first, a missing unit reading as UNIT_TYPES_COUNT and -1. pSource then goes to UNITS_GetRoom
//  untested, and D2Common asserts its unit there.
//  In town a dead player target (CUNIT_IsUnitDead, which the original inlines, monster arm and all) needs ROSTER_CanUnitLootFrom, and a player
//  picking a monster that has a roster owner needs CUNIT_IsSelectableAsPetTarget; anything else is selectable.
//  Outside town each side is first replaced by its owner when it is a monster: the roster's owner (ROSTERPETS_GetOwnerId), then a unit carrying
//  UNITFLAGEX_STOREOWNERINFO whose owner type is UNIT_PLAYER. A unit that ends up as itself is refused. Two players: a dead target the source may
//  loot from is selectable (CUNIT_IsUnitDead inlined a second time), and otherwise the roster's info flag 8, ROSTERINFOFLAG_HOSTILE, decides
//  (PLAYERLIST_RosterTestInfoFlag). Anything else: a monster target whose monstats2.txt row carries ALSEL is always selectable and NOSEL never
//  (DATATBLS_GetMonStats2TxtRecord inlined twice, the first's failures jumping into the second's tests) -- a player's pick of an item or an
//  object is selectable, and the rest is `!STATLIST_AreUnitsAligned`. ALSEL returns TRUE (0x6FB2A00D).
BOOL __fastcall CUNIT_IsUnitSelectableTarget(D2UnitStrc *pSource, D2UnitStrc *pTarget)
{
    D2MonStats2Txt *pMonStats2Txt;
    D2C_UnitTypes nSourceType;
    D2C_UnitTypes nTargetType;
    uint32_t dwSourceGUID;
    uint32_t dwTargetGUID;
    BOOL bDead;
    int32_t nOwnerId;
    int32_t nClassId;
    int32_t nMonStatsEx;

    if (pSource)
    {
        nSourceType = pSource->dwUnitType;
        dwSourceGUID = pSource->dwUnitId;
    }
    else
    {
        nSourceType = UNIT_TYPES_COUNT;
        dwSourceGUID = (uint32_t)-1;
    }

    if (pTarget)
    {
        nTargetType = pTarget->dwUnitType;
        dwTargetGUID = pTarget->dwUnitId;
    }
    else
    {
        nTargetType = UNIT_TYPES_COUNT;
        dwTargetGUID = (uint32_t)-1;
    }

    if (D2COMMON_DUNGEON_IsRoomInTown(D2COMMON_UNITS_GetRoom(pSource)))
    {
        if (pTarget && pTarget->dwUnitType == UNIT_PLAYER)
        {
            if (pTarget->dwFlags & UNITFLAG_ISDEAD)
                bDead = TRUE;
            else if (pTarget->dwUnitType == UNIT_PLAYER)
                bDead = pTarget->dwAnimMode == PLRMODE_DEATH || pTarget->dwAnimMode == PLRMODE_DEAD;
            else if (pTarget->dwUnitType == UNIT_MONSTER)
                bDead = pTarget->dwAnimMode == MONMODE_DEATH || pTarget->dwAnimMode == MONMODE_DEAD;
            else
                bDead = FALSE;

            if (bDead && !ROSTER_CanUnitLootFrom(dwSourceGUID, pTarget->dwUnitId))
            {
                return FALSE;
            }
        }

        if (pSource && pSource->dwUnitType == UNIT_PLAYER && pTarget && pTarget->dwUnitType == UNIT_MONSTER && ROSTERPETS_GetOwnerId(pTarget->dwUnitId) != -1 && !CUNIT_IsSelectableAsPetTarget(pSource, pTarget))
        {
            return FALSE;
        }

        return TRUE;
    }

    if (nSourceType == UNIT_MONSTER)
    {
        nOwnerId = ROSTERPETS_GetOwnerId(dwSourceGUID);
        if (nOwnerId != -1)
        {
            nSourceType = UNIT_PLAYER;
            dwSourceGUID = (uint32_t)nOwnerId;
        }

        //  BUG: the source arm's owner test reads the target. The flag and the owner GUID are pSource's (`[ebp+0C8h]` 0x6FB29E9A, `[ebp+98h]`
        //  0x6FB29EB6), but the owner type is pTarget's (`mov eax, [esi+94h]`, 0x6FB29EAC, esi being pTarget), where the target arm reads all three
        //  from pTarget (0x6FB29EE5..0x6FB29F01). Reproduced; a NULL pTarget would fault there as well. NOT ESTABLISHED: the arm runs (the missile
        //  target tests pass a missile's owner), but a monster's owner info comes with type UNIT_PLAYER from packet 0xAC and with the state's type
        //  from STATES_SetFunc4_SourceUnit, and which stock states store what is the open question.
        if (pSource && ((pSource->dwFlagEx >> 10) & 1) && pTarget->dwOwnerType == UNIT_PLAYER)
        {
            nSourceType = UNIT_PLAYER;
            dwSourceGUID = pSource->dwOwnerGUID;
        }
    }

    if (nTargetType == UNIT_MONSTER)
    {
        nOwnerId = ROSTERPETS_GetOwnerId(dwTargetGUID);
        if (nOwnerId != -1)
        {
            nTargetType = UNIT_PLAYER;
            dwTargetGUID = (uint32_t)nOwnerId;
        }

        if (pTarget && ((pTarget->dwFlagEx >> 10) & 1) && pTarget->dwOwnerType == UNIT_PLAYER)
        {
            dwTargetGUID = pTarget->dwOwnerGUID;
            nTargetType = UNIT_PLAYER;
        }
    }

    if (nSourceType == nTargetType && dwSourceGUID == dwTargetGUID)
        return FALSE;

    if (nSourceType == UNIT_PLAYER && nTargetType == UNIT_PLAYER)
    {
        if (pTarget && pTarget->dwUnitType == UNIT_PLAYER)
        {
            if (pTarget->dwFlags & UNITFLAG_ISDEAD)
                bDead = TRUE;
            else if (pTarget->dwUnitType == UNIT_PLAYER)
                bDead = pTarget->dwAnimMode == PLRMODE_DEATH || pTarget->dwAnimMode == PLRMODE_DEAD;
            else if (pTarget->dwUnitType == UNIT_MONSTER)
                bDead = pTarget->dwAnimMode == MONMODE_DEATH || pTarget->dwAnimMode == MONMODE_DEAD;
            else
                bDead = FALSE;

            if (bDead && ROSTER_CanUnitLootFrom(dwSourceGUID, pTarget->dwUnitId))
            {
                return TRUE;
            }
        }

        return PLAYERLIST_RosterTestInfoFlag(dwSourceGUID, dwTargetGUID, ROSTERINFOFLAG_HOSTILE);
    }

    if (nTargetType == UNIT_MONSTER && pTarget)
    {
        nClassId = pTarget->dwClassId;
        pMonStats2Txt = DATATBLS_GetMonStats2TxtRecord(nClassId);

        if (pMonStats2Txt && (pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_ALSEL >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_ALSEL & 7]))
        {
            return TRUE;
        }

        pMonStats2Txt = NULL;
        if (nClassId >= 0 && nClassId < sgptDataTables->nMonStatsTxtRecordCount)
        {
            nMonStatsEx = (int16_t)sgptDataTables->pMonStatsTxt[nClassId].wMonStatsEx;
            if (nMonStatsEx >= 0 && nMonStatsEx < sgptDataTables->nMonStats2TxtRecordCount)
            {
                pMonStats2Txt = &sgptDataTables->pMonStats2Txt[nMonStatsEx];
            }
        }

        if (pMonStats2Txt && (pMonStats2Txt->nFlags[MONSTATS2FLAGINDEX_NOSEL >> 3] & gdwBitMasks[MONSTATS2FLAGINDEX_NOSEL & 7]))
        {
            return FALSE;
        }
    }

    if (nSourceType == UNIT_PLAYER && (nTargetType == UNIT_ITEM || nTargetType == UNIT_OBJECT))
    {
        return TRUE;
    }

    return !D2COMMON_STATLIST_AreUnitsAligned(pSource, pTarget);
}


static_assert(SKILLSFLAGINDEX_TARGETPET == 25 && SKILLSFLAGINDEX_TARGETALLY == 26,
              "skills.txt byte 7 against gdwBitMasks[1] and [2]: `test [edx+4], "
              "cl` 0x6FB2A134, `test [edx+8], cl` 0x6FB2A23A");

//1.10f: D2Client.0x6FB2A0C0
//  A TARGETPET skill on either button, and the target is the player's own pet.
//  The early-out refuses a missing unit, and a non-player source only when the target is not a monster either (`test eax, eax` / `jz` past the
//  target test, 0x6FB2A0DD) -- a player source goes on whatever the target is.
//  Each button's skill is looked up through DATATBLS_GetSkillsTxtRecordById, which the original inlines (sign, count at +0xBA0, table at +0xB98,
//  the 572-byte stride), and tested as a byte: `mov cl, [eax+7]` / `test [edx+4], cl`, skills.txt TARGETPET (index 25). D2Common's
//  SKILLS_GetSkillIdFromSkill is handed this file and a line, :2888 and :2899. The right button's flag goes straight to the owner test; the left
//  one's only counts when the right one's does not.
BOOL __fastcall CUNIT_IsSelectableAsPetTarget(D2UnitStrc *pSource, D2UnitStrc *pTarget)
{
    D2SkillStrc *pSkill;
    D2SkillsTxt *pSkillsTxt;
    BOOL bLeftPet;
    BOOL bRightPet;
    int32_t nSkillId;

    bLeftPet = FALSE;
    if (!pSource || !pTarget)
        return FALSE;

    if (pSource->dwUnitType != UNIT_PLAYER && pTarget->dwUnitType != UNIT_MONSTER)
        return FALSE;

    pSkill = D2COMMON_UNITS_GetLeftSkill(pSource);
    if (pSkill)
    {
        nSkillId = D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szCUnitCpp, 2888);

        pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);

        if (pSkillsTxt && (((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_TARGETPET >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_TARGETPET & 7]))
        {
            bLeftPet = TRUE;
        }
    }

    bRightPet = FALSE;
    pSkill = D2COMMON_UNITS_GetRightSkill(pSource);
    if (pSkill)
    {
        nSkillId = D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szCUnitCpp, 2899);

        pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);

        if (pSkillsTxt && (((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_TARGETPET >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_TARGETPET & 7]))
        {
            bRightPet = TRUE;
        }
    }

    if (!bRightPet && !bLeftPet)
        return FALSE;

    return (uint32_t)ROSTERPETS_GetOwnerId(pTarget->dwUnitId) == pSource->dwUnitId;
}


//1.10f: D2Client.0x6FB2A1C0
//  A TARGETALLY skill on either button, and the target -- a monster by its roster owner -- in the player's party (ROSTER_UnitsShareParty).
//  The same body as the pet test above with TARGETALLY (index 26, `test [edx+8], cl`), the lines :2941 and :2950, and a wider early-out: a
//  non-player source is refused only for a target that is neither a player nor a monster (0x6FB2A1DF..0x6FB2A1E8). A monster target with no
//  roster owner is refused; the party test's answer is normalised to 0 or 1 by `neg` / `sbb` / `neg` (0x6FB2A2B9).
BOOL __fastcall CUNIT_IsSelectableAsAllyTarget(D2UnitStrc *pSource, D2UnitStrc *pTarget)
{
    D2SkillStrc *pSkill;
    D2SkillsTxt *pSkillsTxt;
    uint32_t dwTargetId;
    BOOL bLeftAlly;
    BOOL bRightAlly;
    int32_t nSkillId;

    bLeftAlly = FALSE;
    if (!pSource || !pTarget)
        return FALSE;

    if (pSource->dwUnitType != UNIT_PLAYER && pTarget->dwUnitType != UNIT_PLAYER && pTarget->dwUnitType != UNIT_MONSTER)
    {
        return FALSE;
    }

    pSkill = D2COMMON_UNITS_GetLeftSkill(pSource);
    if (pSkill)
    {
        nSkillId = D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szCUnitCpp, 2941);

        pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);

        if (pSkillsTxt && (((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_TARGETALLY >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_TARGETALLY & 7]))
        {
            bLeftAlly = TRUE;
        }
    }

    bRightAlly = FALSE;
    pSkill = D2COMMON_UNITS_GetRightSkill(pSource);
    if (pSkill)
    {
        nSkillId = D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szCUnitCpp, 2950);

        pSkillsTxt = DATATBLS_GetSkillsTxtRecordById(nSkillId);

        if (pSkillsTxt && (((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_TARGETALLY >> 3] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_TARGETALLY & 7]))
        {
            bRightAlly = TRUE;
        }
    }

    if (!bRightAlly && !bLeftAlly)
        return FALSE;

    dwTargetId = pTarget->dwUnitId;
    if (pTarget->dwUnitType == UNIT_MONSTER)
    {
        dwTargetId = (uint32_t)ROSTERPETS_GetOwnerId(dwTargetId);
        if (dwTargetId == (uint32_t)-1)
            return FALSE;
    }

    return ROSTER_UnitsShareParty(pSource->dwUnitId, dwTargetId) != 0;
}


static_assert(offsetof(D2DynamicPathStrc, dwClientCoordX) == 0x08
                  && offsetof(D2DynamicPathStrc, tTargetCoord) == 0x10
                  && offsetof(D2DynamicPathStrc, tFinalTargetCoord) == 0x18
                  && offsetof(D2DynamicPathStrc, dwCurrentPointIdx) == 0x24
                  && offsetof(D2DynamicPathStrc, dwPathPoints) == 0x28
                  && offsetof(D2DynamicPathStrc, unk0x38) == 0x38
                  && offsetof(D2DynamicPathStrc, dwPathType) == 0x3C
                  && offsetof(D2DynamicPathStrc, dwPrevPathType) == 0x40
                  && offsetof(D2DynamicPathStrc, dwUnitSize) == 0x44,
              "the path copy's first half: `[ebx+8h]`..`[ebx+44h]`, "
              "0x6FB2A42E..0x6FB2A55F");
static_assert(offsetof(D2DynamicPathStrc, dwTargetType) == 0x5C
                  && offsetof(D2DynamicPathStrc, dwTargetId) == 0x60
                  && offsetof(D2DynamicPathStrc, unk0x67) == 0x67
                  && offsetof(D2DynamicPathStrc, dwSpeed) == 0x68
                  && offsetof(D2DynamicPathStrc, tDirectionVector) == 0x6A
                  && offsetof(D2DynamicPathStrc, dwVelocity) == 0x7C
                  && offsetof(D2DynamicPathStrc, dwAccelerationsCounter) == 0x8C
                  && offsetof(D2DynamicPathStrc, nDistMax) == 0x91
                  && offsetof(D2DynamicPathStrc, nDistance) == 0x94
                  && offsetof(D2DynamicPathStrc, nDirOffset) == 0x98
                  && offsetof(D2DynamicPathStrc, PathPoints) == 0x9C
                  && offsetof(D2DynamicPathStrc, nSavedStepsCount) == 0x1D4
                  && offsetof(D2DynamicPathStrc, SavedSteps) == 0x1D8,
              "the rest of the copy: `[ebx+5Ch]`..`[ebx+1D8h]`, "
              "0x6FB2A45E..0x6FB2A52F");
static_assert(PLRMODE_WALK == 2 && PLRMODE_RUN == 3 && PLRMODE_TOWNWALK == 6
                  && MONMODE_WALK == 2 && MONMODE_RUN == 15,
              "the mode tests and the `push 2/3/6/0Fh` of 0x6FB2A58E..0x6FB2A63C");
static_assert(PATHTYPE_KNOCKBACK_SERVER == 8 && PATHTYPE_KNOCKBACK_CLIENT == 11,
              "`cmp eax, 8` / `cmp eax, 0Bh` at 0x6FB2A538..0x6FB2A54D");

//1.10f: D2Client.0x6FB2A2D0
//  Pulls a unit's movement from the server's copy of it -- single player only, "gnGameType == GT_SINGLE_PLAYER" (:2984), where D2Game's
//  DEBUG_GetUnitFromTypeAndGUID hands back the server's unit for the same type and GUID. It has no code caller: CUNIT_UpdateAllUnits passes it
//  to CUNIT_ForEachUnitInTable for the player and monster rows.
//  A move first, when the positions differ. The server's subtile position (tGameCoords.wPosX/wPosY, zero-extended words) must lie in the unit's
//  room or in one of its neighbours; the original inlines CLIENT_FindRoomContainingSubtile for that, with its own NULL test on the room folded
//  away because the caller has just made it; bounds signed (`jl`/`jge`), the neighbour index unsigned (`jb`, 0x6FB2A3EE). Not found: return,
//  before any copy. Found: D2Common #10231 is called with the unit's current room (`push eax`, eax reloaded from the frame at 0x6FB2A400), never
//  with the room that was found, which only decided whether to call (#10231 re-places the path at its own current position with that room as
//  a hint, its nX/nY unused).
//  Then the path is copied field by field, in this order, and the order is the source's: the two paths are two pointers the compiler cannot
//  prove apart, so no store may pass a load. PathPoints and SavedSteps go by `rep movsd` for as many entries as the server counts; a zero count
//  skips it. dwPathType and dwPrevPathType are kept when the server's is PATHTYPE_KNOCKBACK_SERVER or _CLIENT (8, 11).
//  Then the mode is reconciled, not copied. A monster: server NEUTRAL pulls a client in WALK or RUN to NEUTRAL, server WALK or RUN pulls a
//  NEUTRAL client to it. A player: a switch whose index table is gnAnimModeSyncClass (18 bytes for modes 1..18, bounded `cmp ebp, 11h` / `ja`
//  after `dec`) -- NEUTRAL and TOWNNEUTRAL bring a walking or running client to a stand, WALK and TOWNWALK a standing client to a walk, RUN a
//  standing one to a run (each town-aware where the bytes ask the room), SEQUENCE does nothing, and every other mode, 0 and anything past 18
//  included, takes the action path: a client in KNOCKBACK takes the server's mode outright; otherwise a client skill with flag 1 gets flag 2 --
//  unless the server's skill has flag 1 without flag 2 -- and the function ends there. The RUN arm also accepts a client in RUN, which the
//  entry test (the modes differ) rules out.
//  Every path that reaches the copy ends with #10212 PATH_SetMoveFlags(pUnit, FALSE) -- the skill arm calls it itself and returns; the three
//  early returns (no server unit, no room, no room holding the new position) do not.
void __fastcall CUNIT_ResyncUnitFromServer(D2UnitStrc *pUnit)
{
    D2UnitStrc *pSrvUnit;
    D2DynamicPathStrc *pSrvPath;
    D2DynamicPathStrc *pCltPath;
    D2ActiveRoomStrc *pRoom;
    D2ActiveRoomStrc *pFoundRoom;
    D2SkillStrc *pCltSkill;
    D2SkillStrc *pSrvSkill;
    D2C_UnitTypes nUnitType;
    uint32_t dwCltMode;
    uint32_t dwSrvMode;
    int32_t nUnitGUID;
    int32_t nX;
    int32_t nY;
    int32_t nFlags;

    if (gnGameType != GAMETYPE_SINGLEPLAYER)
    {
        FOG_DisplayAssert((uint32_t)"gnGameType == GT_SINGLE_PLAYER", (uint32_t)szCUnitCpp, 2984);
        exit(-1);
    }

    if (pUnit)
    {
        nUnitType = pUnit->dwUnitType;
        nUnitGUID = (int32_t)pUnit->dwUnitId;
    }
    else
    {
        nUnitType = UNIT_TYPES_COUNT;
        nUnitGUID = -1;
    }

    pSrvUnit = D2GAME_DEBUG_GetUnitFromTypeAndGUID(nUnitType, nUnitGUID);
    if (!pSrvUnit)
        return;

    pSrvPath = pSrvUnit->pDynamicPath;
    pCltPath = pUnit->pDynamicPath;
    pRoom = D2COMMON_UNITS_GetRoom(pUnit);
    if (!pRoom)
        return;

    nX = pSrvPath->tGameCoords.wPosX;
    nY = pSrvPath->tGameCoords.wPosY;
    if (nX != pCltPath->tGameCoords.wPosX || nY != pCltPath->tGameCoords.wPosY)
    {
        pFoundRoom = CLIENT_FindRoomContainingSubtile(pRoom, nX, nY);

        //  the caller's test of the inlined lookup's answer, 0x6FB2A407
        if (!pFoundRoom)
            return;

        D2COMMON_10231(pCltPath, pUnit, pRoom, (uint16_t)nX, (uint16_t)nY);
    }

    pCltPath->tGameCoords.dwPrecisionX = pSrvPath->tGameCoords.dwPrecisionX;
    pCltPath->tGameCoords.dwPrecisionY = pSrvPath->tGameCoords.dwPrecisionY;
    pCltPath->dwClientCoordX = pSrvPath->dwClientCoordX;
    pCltPath->dwClientCoordY = pSrvPath->dwClientCoordY;
    pCltPath->dwCurrentPointIdx = pSrvPath->dwCurrentPointIdx;
    pCltPath->dwPathPoints = pSrvPath->dwPathPoints;
    pCltPath->unk0x38 = pSrvPath->unk0x38;
    pCltPath->tTargetCoord = pSrvPath->tTargetCoord;
    pCltPath->tPrevTargetCoord = pSrvPath->tPrevTargetCoord;
    pCltPath->tFinalTargetCoord = pSrvPath->tFinalTargetCoord;
    pCltPath->dwTargetType = pSrvPath->dwTargetType;
    pCltPath->dwTargetId = pSrvPath->dwTargetId;
    pCltPath->unk0x67 = pSrvPath->unk0x67;
    pCltPath->dwSpeed = pSrvPath->dwSpeed;
    pCltPath->dwVelocity = pSrvPath->dwVelocity;
    pCltPath->nPreviousVelocity = pSrvPath->nPreviousVelocity;
    pCltPath->dwMaxVelocity = pSrvPath->dwMaxVelocity;
    pCltPath->dwAcceleration = pSrvPath->dwAcceleration;
    pCltPath->dwAccelerationsCounter = pSrvPath->dwAccelerationsCounter;
    pCltPath->nDistMax = pSrvPath->nDistMax;
    pCltPath->nIDAStarInitFScore = pSrvPath->nIDAStarInitFScore;
    pCltPath->nStepNum = pSrvPath->nStepNum;
    pCltPath->nDistance = pSrvPath->nDistance;
    pCltPath->nDirOffset = pSrvPath->nDirOffset;
    pCltPath->tDirectionVector.nX = pSrvPath->tDirectionVector.nX;
    pCltPath->tDirectionVector.nY = pSrvPath->tDirectionVector.nY;

    if (pSrvPath->dwPathPoints)
    {
        memcpy(pCltPath->PathPoints, pSrvPath->PathPoints, pSrvPath->dwPathPoints * sizeof(D2PathPointStrc));
    }

    if (pSrvPath->nSavedStepsCount)
    {
        memcpy(pCltPath->SavedSteps, pSrvPath->SavedSteps, pSrvPath->nSavedStepsCount * sizeof(D2PathPointStrc));
    }

    pCltPath->nSavedStepsCount = pSrvPath->nSavedStepsCount;

    if (pSrvPath->dwPathType != PATHTYPE_KNOCKBACK_SERVER && pSrvPath->dwPathType != PATHTYPE_KNOCKBACK_CLIENT)
    {
        pCltPath->dwPathType = pSrvPath->dwPathType;
    }

    if (pSrvPath->dwPrevPathType != PATHTYPE_KNOCKBACK_SERVER && pSrvPath->dwPrevPathType != PATHTYPE_KNOCKBACK_CLIENT)
    {
        pCltPath->dwPrevPathType = pSrvPath->dwPrevPathType;
    }

    //  BUG: dwPrevPathType is stored unconditionally (0x6FB2A555), so the guard just above does nothing. Reproduced as written. REACHABLE: the
    //  function runs every single-player frame for every player and monster; what a knockback path type in the client's dwPrevPathType changes
    //  is not known.
    pCltPath->dwPrevPathType = pSrvPath->dwPrevPathType;
    pCltPath->dwUnitSize = pSrvPath->dwUnitSize;
    D2COMMON_10208_PathSetPathingFlag(pCltPath, D2COMMON_10209_PathCheckPathingFlag(pSrvPath));

    dwCltMode = pUnit->dwAnimMode;
    dwSrvMode = pSrvUnit->dwAnimMode;
    if (dwCltMode != dwSrvMode)
    {
        switch (pUnit->dwUnitType)
        {
        case UNIT_PLAYER:
            switch (dwSrvMode)
            {
            case PLRMODE_NEUTRAL:
            case PLRMODE_TOWNNEUTRAL:
                if (dwCltMode == PLRMODE_WALK || dwCltMode == PLRMODE_RUN || dwCltMode == PLRMODE_TOWNWALK)
                {
                    if (D2COMMON_DUNGEON_IsRoomInTown(D2COMMON_UNITS_GetRoom(pUnit)))
                        D2COMMON_UNITS_ChangeAnimMode(pUnit, PLRMODE_TOWNNEUTRAL);
                    else
                        D2COMMON_UNITS_ChangeAnimMode(pUnit, PLRMODE_NEUTRAL);
                }
                break;

            case PLRMODE_WALK:
            case PLRMODE_TOWNWALK:
                if (dwCltMode == PLRMODE_NEUTRAL || dwCltMode == PLRMODE_TOWNNEUTRAL)
                {
                    if (D2COMMON_DUNGEON_IsRoomInTown(D2COMMON_UNITS_GetRoom(pUnit)))
                        D2COMMON_UNITS_ChangeAnimMode(pUnit, PLRMODE_TOWNWALK);
                    else
                        D2COMMON_UNITS_ChangeAnimMode(pUnit, PLRMODE_WALK);
                }
                break;

            case PLRMODE_RUN:
                if (dwCltMode == PLRMODE_NEUTRAL || dwCltMode == PLRMODE_RUN || dwCltMode == PLRMODE_TOWNNEUTRAL)
                {
                    D2COMMON_UNITS_ChangeAnimMode(pUnit, PLRMODE_RUN);
                }
                break;

            case PLRMODE_SEQUENCE:
                break;

            default:
                if (dwCltMode == PLRMODE_KNOCKBACK)
                {
                    D2COMMON_UNITS_ChangeAnimMode(pUnit, (int32_t)dwSrvMode);
                    break;
                }

                pCltSkill = D2COMMON_UNITS_GetUsedSkill(pUnit);
                pSrvSkill = D2COMMON_UNITS_GetUsedSkill(pSrvUnit);
                if (pCltSkill && (!pSrvSkill || !(D2COMMON_SKILLS_GetFlags(pSrvSkill) & 1) || (D2COMMON_SKILLS_GetFlags(pSrvSkill) & 2)))
                {
                    nFlags = D2COMMON_SKILLS_GetFlags(pCltSkill);
                    if (nFlags & 1)
                    {
                        D2COMMON_SKILLS_SetFlags(pCltSkill, nFlags | 2);
                        D2COMMON_10212_PATH_SetMoveFlags(pUnit, FALSE);
                        return;
                    }
                }
                break;
            }
            break;

        case UNIT_MONSTER:
            switch (dwSrvMode)
            {
            case MONMODE_NEUTRAL:
                if (dwCltMode == MONMODE_WALK || dwCltMode == MONMODE_RUN)
                    D2COMMON_UNITS_ChangeAnimMode(pUnit, MONMODE_NEUTRAL);
                break;

            case MONMODE_WALK:
                if (dwCltMode == MONMODE_NEUTRAL)
                    D2COMMON_UNITS_ChangeAnimMode(pUnit, MONMODE_WALK);
                break;

            case MONMODE_RUN:
                if (dwCltMode == MONMODE_NEUTRAL)
                    D2COMMON_UNITS_ChangeAnimMode(pUnit, MONMODE_RUN);
                break;
            }
            break;
        }
    }

    D2COMMON_10212_PATH_SetMoveFlags(pUnit, FALSE);
}

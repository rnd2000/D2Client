//  SCmd.h -- the globals of SCmd.cpp: the server-command table gSCmdTable (and SCmdFn / SCmdUnitFn, the two slot types its rows are cast to),
//  nine string literals, and four scalars of the character-file buffer and the packet routing.
//  Address ranges 0x6FB76270..0x6FB76B70 and 0x6FBAB9A4..0x6FBAB9B4. Each is a C++17 `inline` variable, one object for every file that
//  includes this header. The table and the literals hold their values in the image and are initialised to the same; the four scalars lie
//  past .data's raw data (0x6FB9B000) and are zero at load.
//  Include it after D2Client.h.
#pragma once

using SCmdFn = decltype(D2SCmdStrc::pfProcessCmd);
using SCmdUnitFn = decltype(D2SCmdStrc::pfProcessUnit);

//  D2Client 0x6FB76270
//  The server-command table: NUM_SCMDS = 174 entries of 12 bytes (2088 bytes), index = the packet id.
//      +0  pfProcessCmd    called on receipt by SCMD_RouteServerPackets, packet in ecx
//      +4  nCmdSize        declared length, -1 for variable
//      +8  pfProcessUnit   called later by SCMD_ProcessUnitPacketQueue as (pUnit, pPacket)
//  The names are the original's own, from SCmd.cpp's asserts: `bCmd < NUM_SCMDS`, `(ptMsgStruct->nCmdSize == -1) || (packetSize ==
//  ptMsgStruct->nCmdSize)` and `ptMsgStruct->pfProcessUnit`. +8 is a function pointer; the router only tests it for non-NULL. Twenty entries
//  have one: the twelve unit handlers (0x0C..0x10, 0x67..0x6D), 0x17, 0x4C, 0x4D, and 0x6E..0x72, which point it at GAME_NullFunc just to get
//  the deferral.
inline D2SCmdStrc gSCmdTable[174] =
{
    /* 0x00 */ { (SCmdFn)GAME_NullFunc, 1, NULL },
    /* 0x01 */ { (SCmdFn)SCMD_HandleSrv01_GameFlags, 8, NULL },
    /* 0x02 */ { (SCmdFn)SCMD_HandleSrv02_ActLoaded, 1, NULL },
    /* 0x03 */ { (SCmdFn)SCMD_HandleSrv03_GameSeed, 12, NULL },
    /* 0x04 */ { (SCmdFn)SCMD_HandleSrv04_LoadComplete, 1, NULL },
    /* 0x05 */ { (SCmdFn)SCMD_HandleSrv05_UnloadComplete, 1, NULL },
    /* 0x06 */ { (SCmdFn)SCMD_HandleSrv06_GameExit, 1, NULL },
    /* 0x07 */ { (SCmdFn)SCMD_HandleSrv07_MapReveal, 6, NULL },
    /* 0x08 */ { (SCmdFn)SCMD_HandleSrv08_MapHide, 6, NULL },
    /* 0x09 */ { (SCmdFn)SCMD_HandleSrv09_SpawnUnit, 11, NULL },
    /* 0x0A */ { (SCmdFn)SCMD_HandleSrv0A_RemoveUnit, 6, NULL },
    /* 0x0B */ { (SCmdFn)SCMD_HandleSrv0B_SetPlayerUnit, 6, NULL },
    /* 0x0C */ { (SCmdFn)GAME_NullFunc, 9,
                   (SCmdUnitFn)SCMD_HandleSrv0C_Action },
    /* 0x0D */ { (SCmdFn)GAME_NullFunc, 13,
                   (SCmdUnitFn)SCMD_HandleSrv0D_ActionAtXY },
    /* 0x0E */ { (SCmdFn)GAME_NullFunc, 12,
                   (SCmdUnitFn)SCMD_HandleSrv0E_ActionOnTarget },
    /* 0x0F */ { (SCmdFn)GAME_NullFunc, 16,
                   (SCmdUnitFn)SCMD_HandleSrv0F_ActionAtXYFromPos },
    /* 0x10 */ { (SCmdFn)GAME_NullFunc, 16,
                   (SCmdUnitFn)SCMD_HandleSrv10_ActionOnTargetFromPos },
    /* 0x11 */ { (SCmdFn)SCMD_HandleSrv11_AddOverlay, 8, NULL },
    /* 0x12 */ { (SCmdFn)GAME_NullFunc, 26, NULL },
    /* 0x13 */ { (SCmdFn)GAME_NullFunc, 14, NULL },
    /* 0x14 */ { (SCmdFn)GAME_NullFunc, 18, NULL },
    /* 0x15 */ { (SCmdFn)SCMD_HandleSrv15_ReassignPlayer, 11, NULL },
    /* 0x16 */ { (SCmdFn)SCMD_HandleSrv16_UnitsUpdate, -1, NULL },
    /* 0x17 */ { (SCmdFn)GAME_NullFunc, -1,
                   (SCmdUnitFn)SCMD_HandleSrv17_ApplyRemoteUnitAction },
    /* 0x18 */ { (SCmdFn)SCMD_HandleSrv18_PlayerAttributes, 15, NULL },
    /* 0x19 */ { (SCmdFn)SCMD_HandleSrv19_1F_AttributeUpdate, 2, NULL },
    /* 0x1A */ { (SCmdFn)SCMD_HandleSrv19_1F_AttributeUpdate, 2, NULL },
    /* 0x1B */ { (SCmdFn)SCMD_HandleSrv19_1F_AttributeUpdate, 3, NULL },
    /* 0x1C */ { (SCmdFn)SCMD_HandleSrv19_1F_AttributeUpdate, 5, NULL },
    /* 0x1D */ { (SCmdFn)SCMD_HandleSrv19_1F_AttributeUpdate, 3, NULL },
    /* 0x1E */ { (SCmdFn)SCMD_HandleSrv19_1F_AttributeUpdate, 4, NULL },
    /* 0x1F */ { (SCmdFn)SCMD_HandleSrv19_1F_AttributeUpdate, 6, NULL },
    /* 0x20 */ { (SCmdFn)SCMD_HandleSrv20_AttributeUpdate, 10, NULL },
    /* 0x21 */ { (SCmdFn)SCMD_HandleSrv21_UpdateSkills, 12, NULL },
    /* 0x22 */ { (SCmdFn)SCMD_HandleSrv22_SetSkill, 12, NULL },
    /* 0x23 */ { (SCmdFn)SCMD_HandleSrv23_SetActiveSkill, 13, NULL },
    /* 0x24 */ { NULL, 90, NULL },
    /* 0x25 */ { NULL, 90, NULL },
    /* 0x26 */ { (SCmdFn)SCMD_HandleSrv26_ChatMessage, -1, NULL },
    /* 0x27 */ { (SCmdFn)SCMD_HandleSrv27_OverheadText, 40, NULL },
    /* 0x28 */ { (SCmdFn)SCMD_HandleSrv28_NpcMenu, 103, NULL },
    /* 0x29 */ { (SCmdFn)SCMD_HandleSrv29_QuestRecord, 97, NULL },
    /* 0x2A */ { (SCmdFn)SCMD_HandleSrv2A_TransactionResult, 15, NULL },
    /* 0x2B */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x2C */ { (SCmdFn)SCMD_HandleSrv2C_PlayUnitSound, 8, NULL },
    /* 0x2D */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x2E */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x2F */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x30 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x31 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x32 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x33 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x34 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x35 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x36 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x37 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x38 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x39 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x3A */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x3B */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x3C */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x3D */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x3E */ { (SCmdFn)SCMD_HandleSrv3E_ItemStatUpdate, -1, NULL },
    /* 0x3F */ { (SCmdFn)SCMD_HandleSrv3F_CursorItemSkill, 8, NULL },
    /* 0x40 */ { (SCmdFn)SCMD_HandleSrv40_SetItemFlag, 13, NULL },
    /* 0x41 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x42 */ { (SCmdFn)SCMD_HandleSrv42_ClearCursor, 6, NULL },
    /* 0x43 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x44 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x45 */ { (SCmdFn)GAME_NullFunc, 13, NULL },
    /* 0x46 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x47 */ { (SCmdFn)SCMD_HandleSrv47_RefreshItem, 11, NULL },
    /* 0x48 */ { (SCmdFn)SCMD_HandleSrv48_RefreshUnit, 11, NULL },
    /* 0x49 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x4A */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x4B */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x4C */ { (SCmdFn)GAME_NullFunc, 16,
                   (SCmdUnitFn)SCMD_HandleSrv4C_UnitAction },
    /* 0x4D */ { (SCmdFn)GAME_NullFunc, 17,
                   (SCmdUnitFn)SCMD_HandleSrv4D_UnitAction },
    /* 0x4E */ { (SCmdFn)SCMD_HandleSrv4E_AddHirelingOffer, 7, NULL },
    /* 0x4F */ { (SCmdFn)SCMD_HandleSrv4F_ClearHirelingOffers, 1, NULL },
    /* 0x50 */ { (SCmdFn)SCMD_HandleSrv50_QuestSpecial, 15, NULL },
    /* 0x51 */ { (SCmdFn)SCMD_HandleSrv51_SpawnObject, 14, NULL },
    /* 0x52 */ { (SCmdFn)SCMD_HandleSrv52_QuestList, 42, NULL },
    /* 0x53 */ { (SCmdFn)SCMD_HandleSrv53_Environment, 10, NULL },
    /* 0x54 */ { (SCmdFn)j_MEMPART_Nullsub1, 3, NULL },
    /* 0x55 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x56 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x57 */ { (SCmdFn)SCMD_HandleSrv57_MonsterEnchants, 14, NULL },
    /* 0x58 */ { (SCmdFn)SCMD_HandleSrv58_AnvilPanel, 7, NULL },
    /* 0x59 */ { (SCmdFn)SCMD_HandleSrv59_AssignPlayer, 26, NULL },
    /* 0x5A */ { (SCmdFn)SCMD_HandleSrv5A_SystemMessage, 40, NULL },
    /* 0x5B */ { (SCmdFn)SCMD_HandleSrv5B_RosterAddPlayer, -1, NULL },
    /* 0x5C */ { (SCmdFn)SCMD_HandleSrv5C_RosterRemovePlayer, 5, NULL },
    /* 0x5D */ { (SCmdFn)SCMD_HandleSrv5D_QuestState, 6, NULL },
    /* 0x5E */ { (SCmdFn)SCMD_HandleSrv5E_QuestAvailability, 38, NULL },
    /* 0x5F */ { (SCmdFn)SCMD_HandleSrv5F_PortalFlags, 5, NULL },
    /* 0x60 */ { (SCmdFn)SCMD_HandleSrv60_TownPortalState, 7, NULL },
    /* 0x61 */ { (SCmdFn)SCMD_HandleSrv61_PlayCinematic, 2, NULL },
    /* 0x62 */ { (SCmdFn)SCMD_HandleSrv62_NpcInteractionStop, 7, NULL },
    /* 0x63 */ { (SCmdFn)SCMD_HandleSrv63_WaypointMenu, 21, NULL },
    /* 0x64 */ { NULL, 0, NULL },
    /* 0x65 */ { (SCmdFn)SCMD_HandleSrv65_RosterUpdate, 7, NULL },
    /* 0x66 */ { (SCmdFn)GAME_NullFunc, 7, NULL },
    /* 0x67 */ { (SCmdFn)GAME_NullFunc, 16,
                   (SCmdUnitFn)SCMD_HandleSrv67_ActionAtXYPathed },
    /* 0x68 */ { (SCmdFn)GAME_NullFunc, 21,
                   (SCmdUnitFn)SCMD_HandleSrv68_ActionOnTargetPathedFromPos },
    /* 0x69 */ { (SCmdFn)GAME_NullFunc, 12,
                   (SCmdUnitFn)SCMD_HandleSrv69_ActionAtXY },
    /* 0x6A */ { (SCmdFn)GAME_NullFunc, 12,
                   (SCmdUnitFn)SCMD_HandleSrv6A_ActionOnTarget },
    /* 0x6B */ { (SCmdFn)GAME_NullFunc, 16,
                   (SCmdUnitFn)SCMD_HandleSrv6B_ActionAtXYFromPos },
    /* 0x6C */ { (SCmdFn)GAME_NullFunc, 16,
                   (SCmdUnitFn)SCMD_HandleSrv6C_ActionOnTargetFromPos },
    /* 0x6D */ { (SCmdFn)GAME_NullFunc, 10,
                   (SCmdUnitFn)SCMD_HandleSrv6D_PierceAtXY },
    /* 0x6E */ { (SCmdFn)GAME_NullFunc, 1,
                   (SCmdUnitFn)GAME_NullFunc },
    /* 0x6F */ { (SCmdFn)GAME_NullFunc, 1,
                   (SCmdUnitFn)GAME_NullFunc },
    /* 0x70 */ { (SCmdFn)GAME_NullFunc, 1,
                   (SCmdUnitFn)GAME_NullFunc },
    /* 0x71 */ { (SCmdFn)GAME_NullFunc, 1,
                   (SCmdUnitFn)GAME_NullFunc },
    /* 0x72 */ { (SCmdFn)GAME_NullFunc, 1,
                   (SCmdUnitFn)GAME_NullFunc },
    /* 0x73 */ { (SCmdFn)SCMD_HandleSrv73_SpawnMissile, 32, NULL },
    /* 0x74 */ { (SCmdFn)SCMD_HandleSrv74_AssignCorpse, 10, NULL },
    /* 0x75 */ { (SCmdFn)SCMD_HandleSrv75_PartyRosterUpdate, 13, NULL },
    /* 0x76 */ { (SCmdFn)SCMD_HandleSrv76_ClearUnitHoverText, 6, NULL },
    /* 0x77 */ { (SCmdFn)SCMD_HandleSrv77_TradeState, 2, NULL },
    /* 0x78 */ { (SCmdFn)SCMD_HandleSrv78_TradePartner, 21, NULL },
    /* 0x79 */ { (SCmdFn)SCMD_HandleSrv79_TradeGold, 6, NULL },
    /* 0x7A */ { (SCmdFn)SCMD_HandleSrv7A_RosterPetsOnPetPacket, 13, NULL },
    /* 0x7B */ { (SCmdFn)SCMD_HandleSrv7B_AssignSkillHotkey, 8, NULL },
    /* 0x7C */ { (SCmdFn)SCMD_HandleSrv7C_ReleaseItemLock, 6, NULL },
    /* 0x7D */ { (SCmdFn)SCMD_HandleSrv7D_ItemBreakRepair, 18, NULL },
    /* 0x7E */ { (SCmdFn)SCMD_HandleSrv7E_LoadActCofData, 5, NULL },
    /* 0x7F */ { (SCmdFn)SCMD_HandleSrv7F_PartyLifeUpdate, 10, NULL },
    /* 0x80 */ { NULL, 4, NULL },
    /* 0x81 */ { (SCmdFn)SCMD_HandleSrv81_AddPet, 20, NULL },
    /* 0x82 */ { (SCmdFn)SCMD_HandleSrv82_PortalOwner, 29, NULL },
    /* 0x83 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x84 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x85 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x86 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x87 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x88 */ { (SCmdFn)GAME_NullFunc, 0, NULL },
    /* 0x89 */ { (SCmdFn)SCMD_HandleSrv89_QuestTrigger, 2, NULL },
    /* 0x8A */ { (SCmdFn)SCMD_HandleSrv8A_NpcAlert, 6, NULL },
    /* 0x8B */ { (SCmdFn)SCMD_HandleSrv8B_SetPartyButton, 6, NULL },
    /* 0x8C */ { (SCmdFn)SCMD_HandleSrv8C_PlayerRelation, 11, NULL },
    /* 0x8D */ { (SCmdFn)SCMD_HandleSrv8D_SetPartyId, 7, NULL },
    /* 0x8E */ { (SCmdFn)SCMD_HandleSrv8E_RosterOnCorpseAssignment, 10, NULL },
    /* 0x8F */ { (SCmdFn)SCMD_HandleSrv8F_Pong, 33, NULL },
    /* 0x90 */ { (SCmdFn)SCMD_HandleSrv90_PartyPosition, 13, NULL },
    /* 0x91 */ { (SCmdFn)SCMD_HandleSrv91_NpcIntros, 26, NULL },
    /* 0x92 */ { (SCmdFn)SCMD_HandleSrv92_RefreshInventory, 6, NULL },
    /* 0x93 */ { (SCmdFn)SCMD_HandleSrv93_SkillPageLevels, 8, NULL },
    /* 0x94 */ { (SCmdFn)SCMD_HandleSrv94_BaseSkillLevels, -1, NULL },
    /* 0x95 */ { (SCmdFn)SCMD_HandleSrv95_LifeManaUpdate, 13, NULL },
    /* 0x96 */ { (SCmdFn)SCMD_HandleSrv96_WalkVerify, 9, NULL },
    /* 0x97 */ { (SCmdFn)SCMD_HandleSrv97_WeaponSwitch, 1, NULL },
    /* 0x98 */ { (SCmdFn)SCMD_HandleSrv98_MinionSpawnClass, 7, NULL },
    /* 0x99 */ { (SCmdFn)SCMD_HandleSrv99_CastAtUnit, 16, NULL },
    /* 0x9A */ { (SCmdFn)SCMD_HandleSrv9A_CastAtXY, 17, NULL },
    /* 0x9B */ { (SCmdFn)SCMD_HandleSrv9B_ResurrectMercOffer, 7, NULL },
    /* 0x9C */ { (SCmdFn)SCMD_HandleSrv9C_ItemDropPickup, -1, NULL },
    /* 0x9D */ { (SCmdFn)SCMD_HandleSrv9D_ItemEquip, -1, NULL },
    /* 0x9E */ { (SCmdFn)SCMD_HandleSrv9E_A2, 7, NULL },
    /* 0x9F */ { (SCmdFn)SCMD_HandleSrv9E_A2, 8, NULL },
    /* 0xA0 */ { (SCmdFn)SCMD_HandleSrv9E_A2, 10, NULL },
    /* 0xA1 */ { (SCmdFn)SCMD_HandleSrv9E_A2, 7, NULL },
    /* 0xA2 */ { (SCmdFn)SCMD_HandleSrv9E_A2, 8, NULL },
    /* 0xA3 */ { (SCmdFn)SCMD_HandleSrvA3_SkillEffect, 24, NULL },
    /* 0xA4 */ { (SCmdFn)SCMD_HandleSrvA4_PreloadMonsterCofs, 3, NULL },
    /* 0xA5 */ { (SCmdFn)SCMD_HandleSrvA5_EndSkillMove, 8, NULL },
    /* 0xA6 */ { (SCmdFn)SCMD_HandleSrvA6_RunesRecord, -1, NULL },
    /* 0xA7 */ { (SCmdFn)SCMD_HandleSrvA7_StateOn, 7, NULL },
    /* 0xA8 */ { (SCmdFn)SCMD_HandleSrvA8_StateOnWithStats, -1, NULL },
    /* 0xA9 */ { (SCmdFn)SCMD_HandleSrvA9_StateOff, 7, NULL },
    /* 0xAA */ { (SCmdFn)SCMD_HandleSrvAA_AllStates, -1, NULL },
    /* 0xAB */ { (SCmdFn)SCMD_HandleSrvAB_UnitHealth, 7, NULL },
    /* 0xAC */ { (SCmdFn)SCMD_HandleSrvAC_SpawnMonster, -1, NULL },
    /* 0xAD */ { (SCmdFn)SCMD_HandleSrvAD_ServerMemoryProbe, 9, NULL },
};

//  D2Client 0x6FB76A98
//  `"wb"` -- the fopen mode in SCMD_SaveCharacterFile, which builds `"%s%s.d2s"` from gLocalPlayerName into a stack buffer and opens it for
//  writing.
inline char szFileModeWriteBinary[3] = "wb";
static_assert(sizeof(szFileModeWriteBinary) == 3, "szFileModeWriteBinary: the item's 3 bytes in the .i64");

//  D2Client 0x6FB76A9C
//  The source path SCmd.cpp hands to FOG_DisplayAssert, FOG_DisplayHalt, FOG_DisplayWarning, FOG_Alloc and FOG_Free. SCMD_AllocUnitPacketQueue's
//  `cmp eax, offset szFile` (0x6FAB54E8) is not a use of the string: it is the bound of its walk over the table, &gSCmdTable[174].nCmdSize.
inline char szFile[58] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\GAME\\SCmd.cpp";
static_assert(sizeof(szFile) == 58, "szFile: the item's 58 bytes in the .i64");

//  D2Client 0x6FB76AD8
//  The assert string "Could not get act!" -- 18 characters and a NUL. Passed to `FOG_DisplayHalt(msg, szFile, line)` by
//  SCMD_HandleSrv04_LoadComplete and SCMD_HandleSrv15_ReassignPlayer, both of which then call exit(-1).
inline char szCouldNotGetActMsg[19] = "Could not get act!";
static_assert(sizeof(szCouldNotGetActMsg) == 19, "szCouldNotGetActMsg: the item's 19 bytes in the .i64");

//  D2Client 0x6FB76AEC
//  The halt message SCMD_HandleSrv15_ReassignPlayer prints at 0x6FAB2A3A before FOG_DisplayHalt and exit(-1). It names `sSCmd_UnitWarpXY`, the
//  packet handler's own name in the original source.
inline char szIllegalPacketWarpXYMsg[35] = "Illegal packet in sSCmd_UnitWarpXY";
static_assert(sizeof(szIllegalPacketWarpXYMsg) == 35, "szIllegalPacketWarpXYMsg: the item's 35 bytes in the .i64");

//  D2Client 0x6FB76B10
//  `"hUnit"` -- the `assert(hUnit)` message pushed to FOG_DisplayAssert by many call sites across nine source files.
inline char szAssertHUnit[6] = "hUnit";
static_assert(sizeof(szAssertHUnit) == 6, "szAssertHUnit: the item's 6 bytes in the .i64");

//  D2Client 0x6FB76B18
//  `"ptCmd->bNumUpdates != 0"`, the assert message of SCMD_HandleSrv16_UnitsUpdate.
inline char szAssertNumUpdates[24] = "ptCmd->bNumUpdates != 0";
static_assert(sizeof(szAssertNumUpdates) == 24, "szAssertNumUpdates: the item's 24 bytes in the .i64");

//  D2Client 0x6FB76B30
//  `"hSkill"` -- the assert message of SCMD_HandleSrv22_SetSkill and SPELLSEL_UnbindEmptyScrollHotkeys.
inline char szAssertHSkill[7] = "hSkill";
static_assert(sizeof(szAssertHSkill) == 7, "szAssertHSkill: the item's 7 bytes in the .i64");

//  D2Client 0x6FB76B38
//  `"UnitGetControlUnit()"` -- the assert message of SCMD_HandleSrv60_TownPortalState.
inline char szAssertUnitGetControlUnit[21] = "UnitGetControlUnit()";
static_assert(sizeof(szAssertUnitGetControlUnit) == 21, "szAssertUnitGetControlUnit: the item's 21 bytes in the .i64");

//  D2Client 0x6FB76B70
//  `"These should be scmd_itemunitex messages!"`, pushed by SCMD_HandleSrv9C_ItemDropPickup (0x6FAB4462) to FOG_DisplayWarning, which logs and
//  returns -- a warning written as prose, not an assert. Nothing writes it.
inline char gszAssertItemUnitExMsg[42] = "These should be scmd_itemunitex messages!";
static_assert(sizeof(gszAssertItemUnitExMsg) == 42, "gszAssertItemUnitExMsg: the item's 42 bytes in the .i64");

//  D2Client 0x6FBAB9A4
//  The buffer SCMD_SaveCharacterFile assembles the character file in. It is allocated with FOG_Alloc, sized by the dword at +3 of the first
//  chunk (`mov ecx, [esi+3]`, 0x6FAB1D2F). It is freed and zeroed where a chunk with byte +2 set arrives while a buffer is held, before the new
//  allocation (0x6FAB1D11..0x6FAB1D29), and after the finished file is written (`fwrite` 0x6FAB1E5E, FOG_Free 0x6FAB1E7A, the stores from
//  0x6FAB1E7F). A chunk that leaves the total short of +3 returns with the buffer held (`jne` 0x6FAB1D8A).
inline void *gpScmdBuffer;
static_assert(sizeof(gpScmdBuffer) == 4, "gpScmdBuffer: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB9A8
//  The running total of the chunk lengths received so far. It is not the size handed to FOG_Alloc (that is the dword at +3 of the chunk): it
//  is what the total is compared against. The buffer is allocated first (0x6FAB1D2F..0x6FAB1D41) and the chunk copied; only then is the total
//  grown (`mov dl, [esi+1]` / `add ebp, edx`, 0x6FAB1D7A..0x6FAB1D7F) and compared with the dword at +3 (0x6FAB1D85..0x6FAB1D8A). Cleared to
//  0 alongside the buffer pointer on both free paths.
inline int32_t gnScmdBufferSize;
static_assert(sizeof(gnScmdBufferSize) == 4, "gnScmdBufferSize: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB9B0
//  Width of one slot in a unit's deferred-packet queue, named by the assert `nSize <= sgnMaxUnitPacketDataSize`. SCMD_AllocUnitPacketQueue
//  computes it once: it scans gSCmdTable from &gSCmdTable[0].nCmdSize (`mov eax, 6FB76274h`, 0x6FAB54D1), takes a row's nCmdSize only where its
//  pfProcessUnit is set (0x6FAB54D6..0x6FAB54DB), and stores the largest plus 5.
inline int32_t sgnMaxUnitPacketDataSize;
static_assert(sizeof(sgnMaxUnitPacketDataSize) == 4, "sgnMaxUnitPacketDataSize: the item's 4 bytes in the .i64");

//  D2Client 0x6FBAB9B4
//  The packet id SCMD_RouteServerPackets last dispatched: a byte stored from a stack slot the loop fills with `mov bl, [ebp+0]`, the first byte
//  of the packet. Written and never read.
inline uint8_t gnLastRoutedPacketId;
static_assert(sizeof(gnLastRoutedPacketId) == 1, "gnLastRoutedPacketId: the item's 1 bytes in the .i64");

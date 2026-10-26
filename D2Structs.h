#pragma once

/****************************************************************************
*                                                                           *
*   D2Structs.h                                                             *
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
*   This file is used to declare data structures used by the game to        *
*   represent various entities, such as the data structure representing     *
*   a unit entity, or a game entity                                         *
*                                                                           *
*****************************************************************************/

/*---------------------------------------------------------------------------
 *
 *  GENERATED -- do not edit by hand; regenerate instead.
 *
 *    python <Tools>/sweep/bridge.py --run D2Client \
 *           <Tools>/sweep/ida_dump_structs.py DS_OUT=<dump>.json
 *    python <Tools>/sweep/gen_structs_header.py \
 *           GS_DUMP=<dump>.json GS_APPLY=1
 *
 *  Source: the D2Client IDA database of the d2client-sweep repository, which
 *  is a Diablo II 1.10 D2Client.dll at image base 0x6FAA0000. The layouts are
 *  what that database holds after the function-by-function sweep, NOT a
 *  reconstruction from D2MOO -- where the two disagree, this file follows the
 *  binary as read.
 *
 *  Included by DLLmain.h AFTER <windows.h>, <cstdint> and D2Constants.h, and
 *  it depends on all three: the SDK and CRT types are not redeclared here,
 *  and neither are the enums D2Constants.h already carries.
 *
 *  The definitions are wrapped in #pragma pack(1) and carry an explicit
 *  _gapN member for every hole IDA left implicit. Both are required to
 *  reproduce the binary: without pack(1) MSVC re-pads IDA's layouts (
 *  D2ConfigStrc: 967 bytes measured, 996 compiled, dwGameType +0x14 -> +0x18),
 *  and without the gap members six structs come out short. The static_asserts
 *  at the end of the file hold the size IDA measured in the binary for every
 *  type that has one; if one fires, this header and the database have diverged
 *  and no offset from it should be trusted until it is known which moved.
 *
 *  Those assertions are compiled under _MSVC_LANG as well as __cplusplus --
 *  MSVC reports __cplusplus as 199711L without /Zc:__cplusplus, which left
 *  the entire block dead in the consuming project until 2026-09-06.
 *
 *  Nested types are hoisted and renamed -- D2ClientStrc::PacketList becomes
 *  D2ClientStrc__PacketList, TSLink<D2GameStrc> becomes TSLink_D2GameStrc.
 *  The layout is unaffected; the assertions cover it.
 *
 *-------------------------------------------------------------------------*/

/* ---- IDA base types ---- */
/* IDA prints padding as "_BYTE gap0[8];" and exports no definition for it. */
#ifndef _IDA_BASE_TYPES
#define _IDA_BASE_TYPES
typedef unsigned char       _BYTE;
typedef unsigned short      _WORD;
typedef unsigned int        _DWORD;
typedef unsigned __int64    _QWORD;
typedef unsigned char       _UNKNOWN;
typedef unsigned char       _BOOL1;
typedef unsigned short      _BOOL2;
typedef unsigned int        _BOOL4;
#endif

/* ---- forward declarations ---- */
struct HD2ARCHIVE__;
struct AsyncData;
struct AsyncDataEventSlot;
struct AsyncDataLoadFileCallback;
struct BnClientInterface;
// struct ClientPacketSaveHeaderPart;
struct CursorDescStrc;
// struct D2Act1Quest1Strc;
// struct D2Act1Quest2Strc;
// struct D2Act1Quest3Strc;
// struct D2Act1Quest4Strc;
// struct D2Act1Quest5Strc;
// struct D2Act1Quest6Strc;
// struct D2Act2Quest0Strc;
// struct D2Act2Quest1Strc;
// struct D2Act2Quest2Strc;
// struct D2Act2Quest3Strc;
// struct D2Act2Quest4Strc;
// struct D2Act2Quest5Strc;
// struct D2Act2Quest6Strc;
// struct D2Act2Quest7Strc;
// struct D2Act2Quest8Strc;
// struct D2Act3Quest0Strc;
// struct D2Act3Quest1Strc;
// struct D2Act3Quest2Strc;
// struct D2Act3Quest3Strc;
// struct D2Act3Quest4Strc;
// struct D2Act3Quest5Strc;
// struct D2Act3Quest6Strc;
// struct D2Act3Quest7Strc;
// struct D2Act4Quest1Strc;
// struct D2Act4Quest2Strc;
// struct D2Act4Quest3Strc;
// struct D2Act5Quest1Strc;
// struct D2Act5Quest2Strc;
// struct D2Act5Quest3Strc;
// struct D2Act5Quest4Strc;
// struct D2Act5Quest5Strc;
// struct D2Act5Quest6Strc;
struct D2ActiveRoomStrc;
// struct D2AiCallback11ArgStrc;
// struct D2AiCallback7ArgStrc;
// struct D2AiCmdStrc;
struct D2AiControlStrc;
struct D2AiParamStrc;
// struct D2AiTableStrc;
// struct D2AiTickParamStrc;
// struct D2AltDrawStrc;
struct D2AnimDataBucketStrc;
struct D2AnimDataRecordStrc;
struct D2AnimDataTableStrc;
struct D2AnimSeqRecordStrc;
struct D2AnimSeqTxt;
// struct D2AnimUnkStrc;
// struct D2AnimatedImageDescriptor;
struct D2AnvilPanelButtonStrc;
// struct D2AnvilUIButtonStrc;
// struct D2ArchiveHandleStrc;
struct D2AreaEntryQuestLineStrc;
// struct D2ArenaStrc;
// struct D2ArenaTxt;
struct D2ArenaUnitStrc;
struct D2ArmTypeTxt;
// struct D2AuraCallbackStrc;
struct D2AutomapCellStrc;
struct D2AutomapDataStrc;
struct D2AutomapLayerStrc;
struct D2AutomapRandStrc;
struct D2AutomapShortStrc;
// struct D2AutomapTxt;
// struct D2BaalThroneAiCallbackArgStrc;
struct D2BeltsTxt;
struct D2BetaScreenStrc;
struct D2BinFieldStrc;
struct D2BinFileStrc;
struct D2BitBufferStrc;
struct D2BodyLocsTxt;
struct D2BookDataTbl;
struct D2BooksTxt;
struct D2BoundingBoxStrc;
struct D2BuySellTabStrc;
// union D2C_PackedTileInformation;
// struct D2CalcCallbackInfoStrc;
struct D2CellFileStrc;
struct D2CfgControlsRowStrc;
struct D2ChampionUModNameStrc;
struct D2CharItemStrc;
struct D2CharPanelLabelStrc;
struct D2CharStatsTxt;
// struct D2CharStrc;
// struct D2CharTemplateTxt;
struct D2CharacterPreviewInfoStrc;
struct D2ChatPrefixStrc;
struct D2CinematicEntry;
// struct D2ClassicStatAdjustmentStrc;
struct D2ClientAutomapDrawCtxStrc;
struct D2ClientButtonStrc;
struct D2ClientCfgControlsButtonStrc;
struct D2ClientClassSkillIconStrc;
struct D2ClientClickStrc;
struct D2ClientCofCacheNodeStrc;
struct D2ClientCofCacheStrc;
struct D2ClientCofNodeStrc;
struct D2ClientDamageGroupStrc;
struct D2ClientDialogLineStrc;
struct D2ClientDialogStrc;
struct D2ClientDrawNodeStrc;
struct D2ClientEditBoxStrc;
struct D2ClientEnvParticleStrc;
struct D2ClientEnvSplashStrc;
struct D2ClientEscMenuInfoStrc;
struct D2ClientEscMenuItemStrc;
struct D2ClientEscMenuUiWindowStrc;
struct D2ClientFloorVertexStrc;
struct D2ClientGfxEntryStrc;
struct D2ClientHirelingInitStrc;
// struct D2ClientInfoStrc;
struct D2ClientInterface;
// struct D2ClientKeySkillStrc;
struct D2ClientLevelEntryTitleStrc;
struct D2ClientLightStrc;
struct D2ClientListBoxEntryStrc;
struct D2ClientListBoxStrc;
struct D2ClientMissileTargetCtxStrc;
struct D2ClientModePreloadStrc;
struct D2ClientMonCofTokensStrc;
struct D2ClientMonModeDescStrc;
struct D2ClientMonModeTokenOverrideStrc;
struct D2ClientMonPalShiftStrc;
struct D2ClientNearbyTargetCtxStrc;
struct D2ClientNpcSpeechStrc;
struct D2ClientObjectModeSoundStrc;
struct D2ClientPalShiftStrc;
struct D2ClientParticleStrc;
struct D2ClientPartyButtonStrc;
struct D2ClientPartyRowStrc;
struct D2ClientPetIconCacheStrc;
struct D2ClientPetInfoStrc;
// struct D2ClientPlayerDataStrc;
struct D2ClientPopupStrc;
struct D2ClientRecordFileHeaderStrc;
struct D2ClientRecordHolderStrc;
struct D2ClientRecordStrc;
struct D2ClientShowItemLabelStrc;
struct D2ClientShrineChartStrc;
struct D2ClientSkillTreeChartStrc;
struct D2ClientSpeechSoundStrc;
struct D2ClientSpeechSubtitleStrc;
struct D2ClientSpinnerStrc;
struct D2ClientStatDescStrc;
struct D2ClientStrc;
struct D2ClientSubMissileSpawnStrc;
struct D2ClientTargetCycleStrc;
struct D2ClientTextLineStrc;
struct D2ClientTextNodeStrc;
struct D2ClientUnitGfxInfoStrc;
// struct D2ClientUnitUpdateSortStrc;
struct D2ClientUnitUpdateStrc;
struct D2ClientViewStrc;
struct D2ClientWallRoomArgsStrc;
struct D2ClientWallSquareStrc;
struct D2ClientWaypointActStrc;
// struct D2CofDataStrc;
struct D2CofHeaderStrc;
// struct D2CofInfoStrc;
// struct D2CofNodeStrc;
struct D2CollisionFieldStrc;
struct D2ColorsTxt;
struct D2CombatStrc;
struct D2CompCodeTxt;
// struct D2CompositStrc;
struct D2CompositTxt;
// struct D2CompositeUnitStrc;
struct D2ConfigCTempStrc;
struct D2ConfigStrc;
struct D2CoordStrc;
struct D2CorpseStrc;
// struct D2CrtSbhNewEntryStrc;
// struct D2CrtSbhNewGroupStrc;
// struct D2CrtSbhNewHeaderStrc;
// struct D2CrtSbhNewListHeadStrc;
// struct D2CrtSbhNewRegionStrc;
// struct D2CrtSbhPageEntryStrc;
// struct D2CrtSbhPageStrc;
// struct D2CrtSbhRegionStrc;
struct D2CubeInputItem;
// struct D2CubeItemStrc;
struct D2CubeMainTxt;
struct D2CubeOutputItem;
struct D2CubeOutputParamStrc;
// struct D2CubeTableStrc;
// struct D2CurseStrc;
// struct D2DS1FileStrc;
// struct D2DamageInfoStrc;
// struct D2DamageStatTableStrc;
struct D2DamageStrc;
struct D2DataArrayStrc;
struct D2DataTablesStrc;
struct D2DifficultyLevelsTxt;
// struct D2DoorObjectAiCallbackArgStrc;
struct D2DrlgActStrc;
struct D2DrlgAnimTileGridStrc;
struct D2DrlgBuildStrc;
struct D2DrlgCoordStrc;
struct D2DrlgCoordsStrc;
struct D2DrlgDeleteStrc;
struct D2DrlgEnvironmentStrc;
struct D2DrlgFileStrc;
struct D2DrlgGridStrc;
// struct D2DrlgLevelLinkDataStrc;
struct D2DrlgLevelRectStrc;
union D2DrlgLevelRectUnion;
struct D2DrlgLevelStrc;
union D2DrlgLevelTypeDataUnion;
// struct D2DrlgLinkStrc;
// struct D2DrlgLinkerParamsStrc;
struct D2DrlgLogicalRoomInfoStrc;
struct D2DrlgMapStrc;
struct D2DrlgOrthStrc;
union D2DrlgOrthTargetUnion;
// struct D2DrlgOutDesertInitStrc;
// struct D2DrlgOutSiegeInitStrc;
// struct D2DrlgOutSiegeInitStrc2;
// struct D2DrlgOutSiegeInitStrc3;
// struct D2DrlgOutdoorGridStrc;
struct D2DrlgOutdoorInfoStrc;
// union D2DrlgOutdoorPackedGrid2InfoStrc;
struct D2DrlgOutdoorRoomStrc;
struct D2DrlgOutdoorSizeStrc;
union D2DrlgOutdoorSizeUnion;
struct D2DrlgPresetInfoStrc;
struct D2DrlgPresetRoomFlagsStrc;
union D2DrlgPresetRoomFlagsUnion;
struct D2DrlgPresetRoomStrc;
// struct D2DrlgRGBStrc;
union D2DrlgRoomCoordUnion;
struct D2DrlgRoomStrc;
struct D2DrlgRoomTileRectStrc;
struct D2DrlgRoomTilesStrc;
union D2DrlgRoomTypeDataUnion;
struct D2DrlgStrc;
struct D2DrlgSubstGroupStrc;
struct D2DrlgTileDataStrc;
struct D2DrlgTileGridStrc;
struct D2DrlgTileInfoStrc;
struct D2DrlgTileLinkStrc;
// struct D2DrlgUnitsStrc;
struct D2DrlgVertexStrc;
struct D2DrlgWarpStrc;
struct D2DynamicPathStrc;
// struct D2EffectStrc;
struct D2ElemTypesTxt;
struct D2ElixirStatLabelStrc;
// struct D2EnvironmentCycleStrc;
// struct D2EventTimerQueueStrc;
// struct D2EventTimerSlabListStrc;
struct D2EventTimerStrc;
struct D2EventsTxt;
struct D2ExperienceDataTbl;
struct D2ExperienceTxt;
union D2FP32_16;
struct D2FP32_16PrecisionStrc;
struct D2FP32_16WordsStrc;
// struct D2FallenShamanAiCallbackArgStrc;
struct D2FieldStrc;
struct D2FloorDrawCtxStrc;
struct D2FogSystemInfoStrc;
// struct D2FontCacheStrc;
// struct D2FontStrc;
// struct D2GSPacketClt01;
// struct D2GSPacketClt02;
// struct D2GSPacketClt03;
// struct D2GSPacketClt04;
// struct D2GSPacketClt05;
// struct D2GSPacketClt06;
// struct D2GSPacketClt07;
// struct D2GSPacketClt08;
// struct D2GSPacketClt09;
// struct D2GSPacketClt0A;
// struct D2GSPacketClt0B;
// struct D2GSPacketClt0C;
// struct D2GSPacketClt0D;
// struct D2GSPacketClt0E;
// struct D2GSPacketClt0F;
// struct D2GSPacketClt10;
// struct D2GSPacketClt11;
// struct D2GSPacketClt12;
// struct D2GSPacketClt13;
// struct D2GSPacketClt14;
struct D2GSPacketClt15;
// struct D2GSPacketClt16;
// struct D2GSPacketClt17;
// struct D2GSPacketClt18;
// struct D2GSPacketClt19;
// struct D2GSPacketClt20;
// struct D2GSPacketClt23;
// struct D2GSPacketClt26;
// struct D2GSPacketClt27;
// struct D2GSPacketClt29;
// struct D2GSPacketClt2A;
// struct D2GSPacketClt2B;
// struct D2GSPacketClt31;
// struct D2GSPacketClt32;
// struct D2GSPacketClt33;
// struct D2GSPacketClt38;
// struct D2GSPacketClt3A;
// struct D2GSPacketClt3B;
// struct D2GSPacketClt3C;
// struct D2GSPacketClt3D;
// struct D2GSPacketClt41;
// struct D2GSPacketClt42;
// struct D2GSPacketClt44;
// struct D2GSPacketClt45;
// struct D2GSPacketClt49;
// struct D2GSPacketClt4F;
// struct D2GSPacketClt51;
// struct D2GSPacketClt58;
// struct D2GSPacketClt59;
struct D2GSPacketClt5D;
struct D2GSPacketClt5E;
// struct D2GSPacketClt5F;
// struct D2GSPacketClt63;
// struct D2GSPacketClt64;
// struct D2GSPacketClt65;
struct D2GSPacketClt66;
struct D2GSPacketClt67;
struct D2GSPacketClt68;
// struct D2GSPacketClt69;
struct D2GSPacketClt6A;
// struct D2GSPacketClt6B;
struct D2GSPacketClt6C;
// struct D2GSPacketClt6D;
// struct D2GSPacketClt6F;
// struct D2GSPacketSrv00;
struct D2GSPacketSrv01;
struct D2GSPacketSrv02;
struct D2GSPacketSrv03;
struct D2GSPacketSrv04;
struct D2GSPacketSrv05;
struct D2GSPacketSrv06;
struct D2GSPacketSrv07;
struct D2GSPacketSrv08;
struct D2GSPacketSrv09;
struct D2GSPacketSrv0A;
struct D2GSPacketSrv0B;
struct D2GSPacketSrv0C;
struct D2GSPacketSrv0D;
struct D2GSPacketSrv0E;
struct D2GSPacketSrv0F;
struct D2GSPacketSrv10;
struct D2GSPacketSrv11;
// struct D2GSPacketSrv12;
// struct D2GSPacketSrv13;
// struct D2GSPacketSrv14;
struct D2GSPacketSrv15;
struct D2GSPacketSrv16;
// struct D2GSPacketSrv17;
struct D2GSPacketSrv18;
struct D2GSPacketSrv19;
struct D2GSPacketSrv1A;
struct D2GSPacketSrv1B;
struct D2GSPacketSrv1C;
struct D2GSPacketSrv1D;
struct D2GSPacketSrv1E;
struct D2GSPacketSrv1F;
struct D2GSPacketSrv20;
struct D2GSPacketSrv21;
struct D2GSPacketSrv22;
struct D2GSPacketSrv23;
// struct D2GSPacketSrv24;
// struct D2GSPacketSrv25;
struct D2GSPacketSrv26;
// struct D2GSPacketSrv26Args;
struct D2GSPacketSrv27;
struct D2GSPacketSrv28;
struct D2GSPacketSrv29;
struct D2GSPacketSrv2A;
// struct D2GSPacketSrv2B;
struct D2GSPacketSrv2C;
// struct D2GSPacketSrv3A;
// struct D2GSPacketSrv3B;
struct D2GSPacketSrv3E;
struct D2GSPacketSrv3F;
struct D2GSPacketSrv40;
struct D2GSPacketSrv42;
// struct D2GSPacketSrv45;
struct D2GSPacketSrv47;
struct D2GSPacketSrv48;
struct D2GSPacketSrv4C;
struct D2GSPacketSrv4D;
struct D2GSPacketSrv4E;
struct D2GSPacketSrv4F;
struct D2GSPacketSrv50;
struct D2GSPacketSrv51;
struct D2GSPacketSrv52;
struct D2GSPacketSrv53;
struct D2GSPacketSrv54;
// struct D2GSPacketSrv55;
struct D2GSPacketSrv57;
struct D2GSPacketSrv58;
struct D2GSPacketSrv59;
struct D2GSPacketSrv5A;
struct D2GSPacketSrv5B;
struct D2GSPacketSrv5C;
struct D2GSPacketSrv5D;
struct D2GSPacketSrv5E;
struct D2GSPacketSrv5F;
struct D2GSPacketSrv60;
struct D2GSPacketSrv61;
struct D2GSPacketSrv62;
struct D2GSPacketSrv63;
struct D2GSPacketSrv65;
// struct D2GSPacketSrv66;
struct D2GSPacketSrv67;
struct D2GSPacketSrv68;
struct D2GSPacketSrv69;
struct D2GSPacketSrv6A;
struct D2GSPacketSrv6B;
struct D2GSPacketSrv6C;
struct D2GSPacketSrv6D;
// struct D2GSPacketSrv6E;
// struct D2GSPacketSrv6F;
// struct D2GSPacketSrv70;
// struct D2GSPacketSrv71;
// struct D2GSPacketSrv72;
struct D2GSPacketSrv73;
struct D2GSPacketSrv74;
struct D2GSPacketSrv75;
struct D2GSPacketSrv76;
struct D2GSPacketSrv77;
struct D2GSPacketSrv78;
struct D2GSPacketSrv79;
struct D2GSPacketSrv7A;
struct D2GSPacketSrv7B;
struct D2GSPacketSrv7C;
struct D2GSPacketSrv7D;
struct D2GSPacketSrv7E;
struct D2GSPacketSrv7F;
struct D2GSPacketSrv81;
struct D2GSPacketSrv82;
struct D2GSPacketSrv89;
struct D2GSPacketSrv8A;
struct D2GSPacketSrv8B;
struct D2GSPacketSrv8C;
struct D2GSPacketSrv8D;
struct D2GSPacketSrv8E;
struct D2GSPacketSrv8F;
struct D2GSPacketSrv90;
struct D2GSPacketSrv91;
struct D2GSPacketSrv92;
struct D2GSPacketSrv93;
struct D2GSPacketSrv94;
struct D2GSPacketSrv94__SkillInfo;
struct D2GSPacketSrv95;
struct D2GSPacketSrv96;
struct D2GSPacketSrv97;
struct D2GSPacketSrv98;
struct D2GSPacketSrv99;
struct D2GSPacketSrv9A;
struct D2GSPacketSrv9B;
struct D2GSPacketSrv9C;
struct D2GSPacketSrv9D;
struct D2GSPacketSrv9E;
struct D2GSPacketSrv9F;
struct D2GSPacketSrvA0;
struct D2GSPacketSrvA1;
struct D2GSPacketSrvA2;
struct D2GSPacketSrvA3;
struct D2GSPacketSrvA4;
struct D2GSPacketSrvA5;
struct D2GSPacketSrvA6;
struct D2GSPacketSrvA7;
struct D2GSPacketSrvA8;
struct D2GSPacketSrvA9;
struct D2GSPacketSrvAA;
struct D2GSPacketSrvAB;
struct D2GSPacketSrvAC;
struct D2GSPacketSrvAD;
// struct D2GSPacketSrvAE;
// struct D2GSPacketSrvAF;
// struct D2GSPacketSrvB0;
// struct D2GSPacketSrvB1;
// struct D2GSPacketSrvB2;
// struct D2GSPacketSrvB3;
// struct D2GSPacketSrvB4;
// struct D2GSPacketSrvFF01;
// struct D2GSPacketSrvFFFD;
// struct D2GSPacketSrvStat;
// struct D2GSPacketSrvStatAndGuid;
struct D2GambleDataTbl;
// struct D2GambleTxt;
// struct D2GameDataTableStrc;
// struct D2GameInfoStrc;
// struct D2GameServerInfoExStrc;
// struct D2GameServerInfoStrc;
// struct D2GameStatisticsStrc;
struct D2GameStrc;
struct D2GemDataTbl;
struct D2GemsTxt;
union D2GemsTxtCodeUnion;
// struct D2GfxCellNodeStrc;
struct D2GfxCellStrc;
struct D2GfxDataStrc;
// struct D2GfxHelperStrc;
// struct D2GfxInfoStrc;
struct D2GfxLightExStrc;
struct D2GfxLightStrc;
// struct D2GfxSettingsStrc;
// struct D2GraphicsInterfaceStrc;
// struct D2GuildInformationStrc;
struct D2HireDescTxt;
struct D2HirelingInitStrc;
struct D2HirelingTxt;
struct D2HitClassSoundStrc;
struct D2HitClassTxt;
struct D2HoverTextStrc;
// struct D2InactiveItemNodeStrc;
// struct D2InactiveMonsterNodeStrc;
// struct D2InactiveUnitListStrc;
// struct D2InactiveUnitNodeStrc;
struct D2InteractInfoStrc;
struct D2InvButtonInfoStrc;
struct D2InvCompGridStrc;
struct D2InvRectStrc;
// struct D2InventoryComponentItemTypeStrc;
struct D2InventoryGridInfoStrc;
struct D2InventoryGridStrc;
struct D2InventoryNodeStrc;
struct D2InventoryStrc;
struct D2InventoryTxt;
// struct D2ItemCacheStrc;
// struct D2ItemCalcStrc;
union D2ItemDataPageUnion;
struct D2ItemDataStrc;
struct D2ItemDataTbl;
// struct D2ItemDropStrc;
struct D2ItemExtraDataStrc;
// struct D2ItemModeArgStrc;
struct D2ItemRatioDataTbl;
struct D2ItemRatioTxt;
struct D2ItemSaveStrc;
// struct D2ItemStatCostDescStrc;
struct D2ItemStatCostTxt;
struct D2ItemTradeStrc;
// struct D2ItemTypeCheckStrc;
struct D2ItemTypesTxt;
struct D2ItemsTxt;
union D2ItemsTxtCodeUnion;
// struct D2JungleStrc;
// struct D2KeyConfigOptionStrc;
// struct D2KeyConfigStrc;
struct D2LeapArcBandStrc;
struct D2LevelDefBin;
// struct D2LevelFileListStrc;
struct D2LevelsTxt;
struct D2LightGridCellStrc;
// struct D2LinkStrc;
struct D2ListBoxImplVtbl;
struct D2ListBoxProxyVtbl;
struct D2LowQualityItemDataTbl;
struct D2LowQualityItemsTxt;
struct D2LvlMazeTxt;
struct D2LvlPrestTxt;
struct D2LvlSubTxt;
struct D2LvlTypesTxt;
struct D2LvlWarpTxt;
struct D2MagicAffixDataTbl;
struct D2MagicAffixTxt;
struct D2MapAIPathPositionStrc;
struct D2MapAIStrc;
// struct D2MazeLevelIdStrc;
// struct D2MenuEntryStrc;
// struct D2MenuInfoStrc;
// struct D2MenuItemStrc;
// struct D2MenuUIStateSaveStrc;
// struct D2MercDataStrc;
struct D2MercPanelLabelStrc;
// struct D2MercSaveDataStrc;
struct D2MercStatRowStrc;
struct D2MessageListStrc;
struct D2MessageSourceStrc;
struct D2MessageStrc;
// struct D2MinionListStrc;
struct D2MissCalcTxt;
// struct D2MissileCalcStrc;
// struct D2MissileDamageDataStrc;
struct D2MissileDataStrc;
struct D2MissileFxStageStrc;
struct D2MissileStrc;
struct D2MissileStreamStrc;
struct D2MissileTargetPosStrc;
union D2MissileTargetUnion;
// struct D2MissileUnitFindArgStrc;
struct D2MissileUnitFindTableStrc;
struct D2MissilesTxt;
struct D2ModStatsArrayStrc;
// struct D2ModeChangeStrc;
struct D2MonAiTxt;
struct D2MonEquipTxt;
struct D2MonItemPercentDataTbl;
struct D2MonItemPercentTxt;
struct D2MonLvlTxt;
// struct D2MonModeCallbackTableStrc;
struct D2MonModeDataTbl;
struct D2MonModeTxt;
struct D2MonModeTxtStub;
struct D2MonPlaceTxt;
struct D2MonPresetTxt;
struct D2MonPropTxt;
// struct D2MonRegDataStrc;
// struct D2MonSkillInfoStrc;
struct D2MonSoundsTxt;
struct D2MonStats2ChoiceCountsStrc;
union D2MonStats2ChoiceCountsUnion;
union D2MonStats2FlagsUnion;
union D2MonStats2ModeFlagsUnion;
struct D2MonStats2Txt;
union D2MonStatsFlagsUnion;
struct D2MonStatsInitStrc;
struct D2MonStatsTxt;
struct D2MonTypeTxt;
struct D2MonUModTxt;
union D2MonsterDataAiParamUnion;
union D2MonsterDataInteractUnion;
struct D2MonsterDataStrc;
struct D2MonsterInteractStrc;
// struct D2MonsterRegionStrc;
// struct D2NPCMessageListStrc;
// struct D2NPCMessageStrc;
// struct D2NPCMessageTableStrc;
// struct D2NpcControlStrc;
// struct D2NpcEventStrc;
// struct D2NpcGambleStrc;
struct D2NpcHirelingOfferStrc;
struct D2NpcMenuDescStrc;
struct D2NpcMenuEntryStrc;
struct D2NpcMenuQuestUnlockStrc;
struct D2NpcMenuUIStateStrc;
// struct D2NpcRecordStrc;
struct D2NpcSpeechLineStrc;
struct D2NpcSpeechPairStrc;
// struct D2NpcTradeStrc;
struct D2NpcTxt;
// struct D2NpcVendorChainStrc;
// struct D2ObeliskPowerUpStrc;
struct D2ObjGroupTxt;
// struct D2ObjInitFnStrc;
struct D2ObjModeDataTbl;
struct D2ObjModeTypeTxt;
// struct D2ObjOperateFnStrc;
// struct D2ObjectControlStrc;
struct D2ObjectDataStrc;
// struct D2ObjectRegionStrc;
// struct D2ObjectRoomCoordStrc;
struct D2ObjectsTxt;
struct D2OpStatDataStrc;
struct D2OverlayTxt;
struct D2PacketBufferStrc;
// struct D2PacketDataStrc;
struct D2PacketListStrc;
// struct D2PacketStrc;
// struct D2PacketTableStrc;
// struct D2PaletteStrc;
// struct D2PaletteTableStrc;
struct D2PanelKeyBindStrc;
// struct D2PartyControlStrc;
// struct D2PartyNodeStrc;
// struct D2PartyStrc;
// struct D2PathFoWallContextStrc;
// struct D2PathFoWallNodeStrc;
// struct D2PathIDAStarContextStrc;
// struct D2PathIDAStarNodeStrc;
// struct D2PathInfoStrc;
// struct D2PathMovStrc;
struct D2PathPointStrc;
// struct D2PetArgStrc;
struct D2PetDataStrc;
struct D2PetInfoStrc;
struct D2PetListStrc;
struct D2PetTypeTxt;
struct D2PlayerClassTxt;
// struct D2PlayerCountBonusStrc;
struct D2PlayerDataStrc;
struct D2PlayerListStrc;
struct D2PlayerPetStrc;
struct D2PlayerTradeStrc;
// struct D2PlayerWeaponSequencesStrc;
struct D2PlrIntroStrc;
struct D2PlrModeDataTbl;
struct D2PlrModeTxtStub;
struct D2PlrModeTypeTxt;
struct D2PopupStrc;
struct D2PresetUnitStrc;
// struct D2PrisonOfIceRewardStrc;
struct D2PropertiesTxt;
// struct D2PropertyAssignStrc;
struct D2PropertyStrc;
struct D2QtyWarnLevelStrc;
struct D2QualityItemDataTbl;
struct D2QualityItemsTxt;
// struct D2QuestArgStrc;
struct D2QuestChainStrc;
// struct D2QuestDataStrc;
// struct D2QuestDescStrc;
// struct D2QuestDescriptorStrc;
// struct D2QuestGUIDStrc;
// struct D2QuestInfoStrc;
// struct D2QuestInitTableStrc;
// struct D2QuestIntroTableStrc;
struct D2QuestLogCloseRectStrc;
struct D2QuestLogEntryStrc;
struct D2QuestLogQuestDefStrc;
struct D2QuestLogTextBlockStrc;
struct D2QuestLogTextPairStrc;
struct D2QuestMenuStringStrc;
// struct D2QuestTimerStrc;
// struct D2QuestUiButtonCoordStrc;
// struct D2QuestUiStrc;
// struct D2QuestUiTabStrc;
struct D2RareAffixDataTbl;
struct D2RareAffixTxt;
struct D2RoomCollisionGridStrc;
struct D2RoomCoordListStrc;
struct D2RoomTileStrc;
// struct D2RosterButtonStrc;
// struct D2RosterControlStrc;
struct D2RosterCorpseStrc;
struct D2RosterGroupStrc;
struct D2RosterInfoStrc;
struct D2RosterPetStrc;
struct D2RosterUnitStrc;
struct D2RuneDataTbl;
struct D2RunesTxt;
struct D2SCmdStrc;
struct D2SLayerStatIdPartsStrc;
struct D2SLayerStatIdStrc;
union D2SLayerStatIdUnion;
// struct D2SaveHeaderStrc;
// struct D2SaveSkillKeyStrc;
// struct D2SavedItemStrc;
struct D2SeedPartsStrc;
struct D2SeedStrc;
union D2SeedUnion;
// struct D2ServerCallbackFunctions;
struct D2SetItemsTxt;
struct D2SetsTxt;
struct D2ShadowLightGammaPaletteStrc;
// struct D2ShrineDataStrc;
// struct D2ShrineTableStrc;
struct D2ShrinesTxt;
// struct D2SkillCalcStrc;
struct D2SkillCalcTxt;
struct D2SkillDescTxt;
struct D2SkillListStrc;
struct D2SkillStrc;
union D2SkillTargetGUIDUnion;
union D2SkillTargetXUnion;
union D2SkillTargetYUnion;
// struct D2SkillTreeChartStrc;
struct D2SkillsTxt;
struct D2SoundChannelCacheStrc;
struct D2SoundChannelStrc;
struct D2SoundEnvironTxt;
struct D2SoundInstanceStrc;
struct D2SoundListNodeStrc;
struct D2SoundsTxt;
struct D2SoundsTxtStub;
struct D2SpellSelPanelStrc;
struct D2SplittedTextStrc;
struct D2Srv50A1Q4CainPayloadStrc;
struct D2Srv50A5Q6BaalPayloadStrc;
struct D2Srv50HirelingPayloadStrc;
struct D2Srv50QuestsStatusPayloadStrc;
struct D2Srv50TombOffsetPayloadStrc;
struct D2StatListExStrc;
struct D2StatListStrc;
struct D2StatStrc;
struct D2StatesTxt;
union D2StatesTxtFlagsUnion;
// struct D2StaticFieldCallbackArgStrc;
struct D2StaticPathStrc;
struct D2StatsArrayStrc;
struct D2StorePageTxt;
// struct D2SummonArgStrc;
struct D2SuperUniquesTxt;
struct D2TCExInfoStrc;
struct D2TCExShortStrc;
// struct D2TargetNodeStrc;
// struct D2TaskStrc;
// struct D2TblHeaderStrc;
// struct D2TblNodeStrc;
struct D2TextDisplayListStrc;
struct D2TextHeaderStrc;
// struct D2TextMessageStrc;
struct D2TextNodeStrc;
struct D2TextStateStrc;
// struct D2TileLibraryBlockStrc;
struct D2TileLibraryEntryStrc;
struct D2TileLibraryHashNodeStrc;
struct D2TileLibraryHashRefStrc;
struct D2TileLibraryHashStrc;
// struct D2TileLibraryHeaderStrc;
struct D2TileRecordStrc;
// struct D2TileStrc;
// struct D2TimerArg2Strc;
// struct D2TimerArgStrc;
// struct D2TreasureClassExTxt;
union D2TxtLinkCodeUnion;
struct D2TxtLinkNodeStrc;
struct D2TxtLinkStrc;
struct D2TxtLinkTblStrc;
struct D2UiButtonImageStrc;
struct D2UiButtonImplVtbl;
struct D2UiButtonProxyVtbl;
struct D2UiControlBaseVtbl;
struct D2UiControlProxyVtbl;
struct D2UiEditBoxImplVtbl;
struct D2UiSpinnerImplVtbl;
struct D2UiSpinnerProxyVtbl;
struct D2UniqueAppellationTxt;
struct D2UniqueItemsTxt;
struct D2UniquePrefixTxt;
struct D2UniqueSuffixTxt;
struct D2UniqueTitleTxt;
// struct D2UnitAnimModeStatBasePercentageStrc;
struct D2UnitClientStrc;
union D2UnitDataUnion;
// struct D2UnitDescriptionListStrc;
struct D2UnitEventStrc;
struct D2UnitFindArgStrc;
struct D2UnitFindDataStrc;
union D2UnitGfxDataCopyUnion;
// struct D2UnitInfoStrc;
union D2UnitModeUnion;
struct D2UnitPacketListStrc;
union D2UnitPathUnion;
// struct D2UnitProxyStrc;
union D2UnitServerClientUnion;
struct D2UnitServerStrc;
struct D2UnitStrc;
union D2UnitTickOrPacketsUnion;
// struct D2UnkAiCallbackArgStrc;
// struct D2UnkAiCallbackArgStrc2;
// struct D2UnkDrlgLogicStrc;
// struct D2UnkDrlgRoomTileStrc;
// struct D2UnkDrlgRoomTileStrc2;
// struct D2UnkExcelStrc;
// struct D2UnkHireStrc;
// struct D2UnkHireStrc2;
// struct D2UnkHireStrc3;
// struct D2UnkHireStrc4;
// struct D2UnkHireStrc5;
// struct D2UnkItemModeStrc;
// struct D2UnkItemModeStrc2;
// struct D2UnkMissileCalcStrc;
struct D2UnkMonCompStrc;
// struct D2UnkMonCreateStrc;
// struct D2UnkMonCreateStrc2;
// struct D2UnkMonCreateStrc3;
union D2UnkMonsterDataLifeUnion;
struct D2UnkMonsterDataStrc;
// struct D2UnkNecSkillStrc;
// struct D2UnkOutPlaceStrc12;
// struct D2UnkOutdoorStrc;
struct D2UnkOutdoorStrc2;
// struct D2UnkOutdoorStrc3;
// struct D2UnkPathStrc;
// struct D2UnkPathStrc2;
struct D2UnkPlayerDataStrc;
// struct D2UnkPlrModeStrc2;
// struct D2VileMotherAiCallbackArgStrc;
// struct D2WaypointActTableStrc;
struct D2WaypointCoordTableStrc;
struct D2WaypointDataStrc;
// struct D2WaypointFlagTableStrc;
struct D2WaypointTableStrc;
struct D2WeaponDescStrc;
// struct D2WinAccountListDataStrc;
// struct D2WinAccountListStrc;
// struct D2WinAnimImageStrc;
// struct D2WinButtonStrc;
struct D2WinCommandRegisterStrc;
// struct D2WinControlStrc;
// struct D2WinEditBoxStrc;
// struct D2WinImage2Strc;
// struct D2WinImageClickRectStrc;
// struct D2WinImageStrc;
// struct D2WinListDataStrc;
// struct D2WinListStrc;
// struct D2WinPopUpLineStrc;
// struct D2WinPopUpStrc;
// struct D2WinProgressBarStrc;
// struct D2WinScrollBarStrc;
// struct D2WinSmackStrc;
// struct D2WinTextBoxLineStrc;
// struct D2WinTextBoxStrc;
// struct D2WinTimerStrc;
// struct D2WindowPlacementStrc;
struct D2WindowProcCallbackStrc;
// struct D2_6FD106F0_Strc;
// struct D2_6FD10880_Strc;
// struct D2pSpellTblStrc;
union FOGASTNodeStrc;
// struct HSARCHIVE__;
// struct HSFILE__;
struct HotkeyMappingEntry;
struct KeyFunctions;
struct QUESTLOGBUTTONPOSITIONT;
struct SCmdArg;
struct SGameDataChunkStrc;
struct SGameDataNodeStrc;
struct SGameDataTableStrc;
struct SGameDataTableVtbl;
struct SMSGHANDLER_PARAMS;
struct TSExplicitList;
struct TSExplicitListArray;
struct TSLink;
// struct TanToDirectionLutEntryStrc;
struct Unicode;
// struct _OVERLAPPED;
// union anon_127762106345282DF2C438C51804D07A;
// struct anon_14587F979C641006F380FCBF23CEC161;
// struct anon_17EA52B533BA0577390A0A91F2834258;
// struct anon_34227162FB87FBED6A1B09BFAFEE6B72;
// union anon_380F2128073F347D8978CA0677085670;
// struct anon_3F3466B13695857A15A82D9650D51B49;
// struct anon_41F79909BF2D6EA47E380FF32B7BDFF7;
// struct anon_427EBAF5AD08AB583E1D4DF460AB8325;
// struct anon_4CADA87776A5C43294374C4CD737B374;
// struct anon_50DA7804EBF03AEE90BE8F81913C2806;
// struct anon_527403D9EC2CF491362D3AE59B5ADD67;
// union anon_5A9DD72A24B1493213128C856E309FBA;
// union anon_83B6E30208D726A49ED3B6646DA7606D;
// union anon_9061D5506B188DF6886B65C2F499A751;
// union anon_9C8EE1B46AB7CB663EA62D576175A87F;
// struct anon_9E01DBFBEC86134033FD3477EE0CD60D;
// union anon_9E656BAF27D4BA1E170AEB1A92E0D77D;
// struct anon_A0B592BF32EB0681EBBB5D252D2EFAF3;
// struct anon_A3719FF1A699486E8629E16291555876;
// struct anon_A85CD68F813312C5E6BB1494B5EEB9CC;
// struct anon_ABAA6F78D0909A5EC38B8DAC19886F2D;
// struct anon_B40DC28E98FE5F06F50437E23D68FF31;
// union anon_BBC6B91D16152AC7247EC2523B3ED144;
// struct anon_CAAEA91160A63143F2AE058B4EF58ACE;
// union anon_FB734C8D8ED8BB4F2FAE5CDADDE28D6C;
struct ptMsgStruct;

/* ---- D2Lang's spelling of D2C_Language ----------------------------------
 * Unicode::Personalize's exported name carries W4ELANGUAGE@@, so this is the
 * type its fifth parameter has to be declared with -- a typedef of
 * D2C_Language mangles to W4D2C_Language@@ and does not link. Opaque on
 * purpose: C++11 makes an enum with a fixed underlying type complete, so it
 * can be passed and cast, and the enumerators are D2Constants.h's.
 *------------------------------------------------------------------------*/
enum ELANGUAGE : int;

/* ---- definitions ---------------------------------------------------------
 * pack(1) is REQUIRED, not stylistic: these are IDA's layouts, where a member
 * sits at the offset the binary puts it and nowhere else. Under the
 * compiler's own alignment MSVC re-pads them -- D2ConfigStrc went from the
 * 967 bytes IDA measured to 996 and dwGameType moved from +0x14 to +0x18.
 * The `_gapN` members below carry the holes IDA left implicit; together with
 * pack(1) they reproduce the binary byte for byte, and the assertions at the
 * end of the file are what makes that checkable.
 *------------------------------------------------------------------------*/
#pragma pack(push, 1)
typedef void (__fastcall *ACTCALLBACKFN)(D2ActiveRoomStrc *);

typedef int32_t (__stdcall *ARCHIVE_ShowMessageFunctionPtr)();

typedef void (__stdcall *AUTOMAPFN)(D2ActiveRoomStrc *);

struct AsyncDataLoadFileCallback
{
uint32_t (__stdcall *callback)(void *pFileData, uint32_t nDataLength, struct AsyncDataLoadFileCallback *pCallbackData);
uint32_t userData[4];
void *pData;
uint32_t nFileSize;
};

struct AsyncData
{
void *hFile;
void *pMemPool;
OVERLAPPED tOverlapped;
struct AsyncDataLoadFileCallback pAsyncOpDesc;
struct AsyncDataEventSlot *pEventSlot;
void *pBuffer;
uint32_t nFileSize;
uint32_t nPriority;
uint32_t nFlags;
uint32_t nPreviousPriority;
struct AsyncData *pNextPostponedPriorityChange;
};

struct AsyncDataEventSlot
{
void *hEvent;
uint32_t bSlotAvailable;
struct AsyncData *pAsyncOp;
};

struct CursorDescStrc
{
uint32_t bAnimated;
uint32_t bLoops;
uint32_t nFrameCount;
uint32_t nFrameAdvance;
uint32_t bCanPress;
void (__fastcall *pDrawFunction)();
const char *szName;
};

struct D2DrlgCoordsStrc
{
int32_t nSubtileX;
int32_t nSubtileY;
int32_t nSubtileWidth;
int32_t nSubtileHeight;
int32_t nTileXPos;
int32_t nTileYPos;
int32_t nTileWidth;
int32_t nTileHeight;
};

struct D2SeedStrc
{
union
{
struct
{
uint32_t nLowSeed;
uint32_t nHighSeed;
};
uint64_t lSeed;
};
};

struct D2ActiveRoomStrc
{
D2DrlgCoordsStrc tCoords;
D2DrlgRoomTilesStrc *pRoomTiles;
D2ActiveRoomStrc **ppRoomList;
int32_t nNumRooms;
D2UnitStrc *pUnitFirst;
D2UnitStrc *pUnitUpdate;
D2RoomCollisionGridStrc *pCollisionGrid;
D2DrlgRoomStrc *pDrlgRoom;
__declspec(align(1)) D2SeedStrc pSeed;
D2DrlgDeleteStrc *pDrlgDelete;
uint32_t dwFlags;
struct D2ClientStrc **ppClients;
int32_t nNumClients;
int32_t nMaxClients;
int32_t nTileCount;
int32_t nAllies;
int32_t unk0x60;
uint8_t nCurrentDeathIndex;
uint8_t pad0x65[3];
uint32_t nLastDeadGUIDs[4];
D2DrlgActStrc *pAct;
D2ActiveRoomStrc *pRoomNext;
};

struct D2AnimDataRecordStrc
{
char szAnimDataName[8];
uint32_t dwFrames;
int32_t dwAnimSpeed;
uint8_t pFrameFlags[144];
};

struct D2AnimDataBucketStrc
{
int32_t nbEntries;
D2AnimDataRecordStrc aEntries[];
};

struct D2AnimDataTableStrc
{
void *pBinaryData;
D2AnimDataBucketStrc *pHashTableBucket[256];
D2AnimDataRecordStrc tDefaultRecord;
};

struct D2AnimSeqRecordStrc
{
D2AnimSeqTxt *pAnimSeqTxtRecord;
int32_t nSeqFramesCount;
int32_t nFramesCount;
};

struct D2AnimSeqTxt
{
uint16_t wSequence;
uint8_t nMode;
uint8_t nFrame;
uint8_t nDir;
D2AnimSeqEvent nEvent;
};

struct D2AnvilPanelButtonStrc
{
D2C_StringIndices wLabelStrId;
int32_t nX;
int32_t nY;
int32_t nCelX;
int32_t nCelY;
int32_t nLeft;
int32_t nRight;
int32_t nTop;
int32_t nBottom;
int32_t nCelIndex;
uint8_t bState;
uint8_t bPressed;
};

struct D2AreaEntryQuestLineStrc
{
int32_t nLevelIds[10];
D2Quests nQuestId;
uint16_t nSoundEvent;
uint16_t unk0x2E;
};

struct D2ArmTypeTxt
{
char szName[32];
char szToken[20];
};

struct D2AutomapCellStrc
{
uint32_t fSaved;
uint16_t nCellNo;
uint16_t xPixel;
uint16_t yPixel;
uint16_t wWeight;
struct D2AutomapCellStrc *pPrev;
struct D2AutomapCellStrc *pNext;
};

struct D2AutomapDataStrc
{
uint32_t nLayerNo;
uint32_t fSaved;
uint32_t dwGameField;
uint32_t dwFloorSize;
uint32_t dwWallsSize;
uint32_t dwUnitsSize;
uint32_t dwExtraSize;
void *pFloors;
void *pWalls;
void *pUnits;
void *pExtras;
};

struct D2AutomapLayerStrc
{
uint32_t nLayerNo;
uint32_t fSaved;
struct D2AutomapCellStrc *pFloors;
struct D2AutomapCellStrc *pWalls;
struct D2AutomapCellStrc *pObjects;
struct D2AutomapCellStrc *pExtras;
struct D2AutomapLayerStrc *pNext;
};

struct D2AutomapRandStrc
{
int32_t nStart;
int32_t nEnd;
};

struct D2AutomapShortStrc
{
uint32_t dwAutomapLevelType;
uint32_t dwAutomapTileType;
uint8_t nStyle;
uint8_t nStartSequence;
uint8_t nEndSequence;
uint8_t pad0x0B;
int32_t nCel[4];
int32_t nCels;
};

struct D2InvRectStrc
{
int32_t nLeft;
int32_t nRight;
int32_t nTop;
int32_t nBottom;
};

struct D2BeltsTxt
{
uint8_t nIndex;
uint8_t pad0x01[3];
uint8_t nBoxes;
uint8_t pad0x05[3];
D2InvRectStrc beltBoxes[16];
};

struct D2BetaScreenStrc
{
int32_t dwStartTick;
D2CellFileStrc *hImage;
uint8_t nPaletteAct;
const char *szName;
};

struct D2BinFieldStrc
{
const char *szFieldName;
D2C_TxtFieldTypes nFieldType;
int32_t nFieldLength;
int32_t nFieldOffset;
void *pLinkField;
};

struct D2BinFileStrc
{
uint8_t *pDataBuffer;
uint8_t *pData;
int32_t nRecordCount;
int32_t nCellCount;
};

struct D2BitBufferStrc
{
uint8_t *pBuffer;
int32_t nBits;
int32_t nPos;
int32_t nPosBits;
int32_t bFull;
};

struct D2BodyLocsTxt
{
uint32_t dwCode;
};

struct D2BookDataTbl
{
int32_t nBooksTxtRecordCount;
D2BooksTxt *pBooksTxt;
};

struct D2BooksTxt
{
uint16_t wStr;
uint8_t nSpellIcon;
uint8_t pad0x03;
uint32_t dwPSpell;
int32_t dwScrollSkillId;
int32_t dwBookSkillId;
uint32_t dwBaseCost;
uint32_t dwCostPerCharge;
uint32_t dwScrollSpellCode;
uint32_t dwBookSpellCode;
};

struct D2BoundingBoxStrc
{
int32_t nLeft;
int32_t nBottom;
int32_t nRight;
int32_t nTop;
};

struct D2BuySellTabStrc
{
int32_t nX;
int32_t nY;
D2C_StringIndices nStringIndex;
BOOL bSelected;
BOOL bActive;
};

struct D2CfgControlsRowStrc
{
enum D2HotkeyIndices eCmd;
uint16_t nTblIndex;
uint32_t unk0x06;
};

struct D2ChampionUModNameStrc
{
D2C_MonUMods nUMod;
D2C_StringIndices nNameStr;
uint16_t pad0x06;
};

struct D2CharItemStrc
{
uint32_t dwItemCode;
uint8_t nBodyLoc;
uint8_t nItemCount;
uint16_t pad0x06;
};

struct D2CharPanelLabelStrc
{
int32_t nLeft;
int32_t nY;
int32_t nRight;
int16_t wStringId;
int32_t nTextWidth;
};

struct __declspec(dllimport) Unicode
{
uint16_t ch;

Unicode(uint16_t ch = 0);

static int32_t __fastcall isLineBreak(const Unicode *pwsz, uint32_t nIndex);
static void __fastcall Personalize(Unicode *pwszOut, const Unicode *pwszName,
                                   const Unicode *pwszPattern, int32_t nMaxChars,
                                   ELANGUAGE eLanguage);
static void __cdecl sprintf(int32_t nMaxChars, Unicode *pwszDst,
                            const Unicode *pwszFmt, ...);
static Unicode *__fastcall strcat(Unicode *pwszDst, const Unicode *pwszSrc);
static Unicode *__fastcall strchr(const Unicode *pwsz, Unicode ch);
static Unicode *__fastcall strcpy(Unicode *pwszDst, const Unicode *pwszSrc);
static int32_t __fastcall strlen(const Unicode *pwsz);
static Unicode *__fastcall strncpy(Unicode *pwszDst, const Unicode *pwszSrc,
                                   int32_t nMaxChars);
static Unicode *__fastcall strstr(const Unicode *pwszHaystack,
                                  const Unicode *pwszNeedle);
static Unicode *__fastcall toUnicode(Unicode *pwszOut, const char *szIn,
                                     int32_t nMaxChars);
static char *__fastcall toUtf(char *szOut, const Unicode *pwszIn,
                              int32_t nMaxChars);
static uint32_t __fastcall unicodenwidth(const char *szIn, int32_t nMaxChars);
static Unicode *__fastcall win2Unicode(Unicode *pwszOut, const char *szIn,
                                       int32_t nMaxChars);

private:
static uint16_t _toLowerTable[256];
static uint16_t _toUpperTable[256];

friend struct UnicodeTables;
};

struct D2CharStatsTxt
{
Unicode wszClassName[16];
char szClassName[16];
uint8_t nStr;
uint8_t nDex;
uint8_t nInt;
uint8_t nVit;
uint8_t nStamina;
uint8_t nLifeAdd;
uint8_t nPercentStr;
uint8_t nPercentInt;
uint8_t nPercentDex;
uint8_t nPercentVit;
uint8_t nManaRegen;
uint8_t pad0x3B;
uint32_t dwToHitFactor;
uint8_t nWalkSpeed;
uint8_t nRunSpeed;
uint8_t nRunDrain;
uint8_t nLifePerLevel;
uint8_t nStaminaPerLevel;
uint8_t nManaPerLevel;
uint8_t nLifePerVitality;
uint8_t nStaminaPerVitality;
uint8_t nManaPerMagic;
uint8_t nBlockFactor;
uint16_t pad0x4A;
uint32_t dwBaseWClass;
uint8_t nStatsPerLevel;
uint8_t pad0x51;
uint16_t wStrAllSkills;
uint16_t wStrSkillTab[3];
uint16_t wStrClassOnly;
D2CharItemStrc pItems[10];
uint16_t wStartSkill;
int16_t nBaseSkill[10];
uint16_t pad0xC2;
};

struct D2CharacterPreviewInfoStrc
{
uint16_t nVersion;
uint8_t pComponents[11];
uint8_t nClass;
uint8_t pComponentColors[11];
uint8_t nLevel;
uint16_t nClientFlags;
uint16_t nGuildFlags;
uint8_t nGuildEmblemBgColor;
uint8_t nGuildEmblemFgColor;
uint8_t nGuildEmblemType;
uint32_t szGuildTag;
uint8_t pad0x25;
};

struct D2ChatPrefixStrc
{
const char *pszPrefix;
int16_t nKind;
int32_t bTakesTargetName;
int32_t bAllowedInSinglePlayer;
};

struct D2CinematicEntry
{
int32_t nVideoId;
const char *pszFileName;
uint8_t bLocalized;
uint8_t bHasResolutionSuffix;
uint8_t nUnk0A;
uint8_t nUnk0B;
};

struct D2ClientAutomapDrawCtxStrc
{
D2CellFileStrc *hCel;
int32_t nClipLeft;
int32_t nClipRight;
int32_t nClipTop;
int32_t nClipBottom;
int32_t nMarginX;
int32_t nMarginY;
};

struct D2ClientButtonStrc
{
const D2UiButtonImplVtbl *pVtbl;
int32_t field_04;
int32_t nX;
int32_t nY;
int32_t field_10;
int32_t field_14;
int32_t nHeight;
int32_t nWidth;
int32_t bPressed;
int32_t bFireOnEnter;
int32_t (__cdecl *pfnOnClick)();
int32_t field_2C;
uint16_t wFrame;
int32_t nImageIndex;
};

struct D2ClientCfgControlsButtonStrc
{
D2C_StringIndices nLabelStrId;
void (__cdecl *pfnOnClick)();
int32_t nHoverColor;
int32_t nLeft;
int32_t nRight;
int32_t nTop;
int32_t nBottom;
};

struct D2ClientClassSkillIconStrc
{
D2CellFileStrc *hImage;
char szFile[30];
};

struct D2ClientClickStrc
{
int32_t dwClickFlags;
D2UnitStrc *pUnit;
D2UnitStrc *pTarget;
int32_t nX;
int32_t nY;
int32_t nMoveToPointCmd;
int32_t nMoveToUnitCmd;
D2SkillStrc *pSkill;
};

struct D2ClientCofCacheNodeStrc
{
struct D2ClientCofCacheNodeStrc *pNext;
uint32_t dwFlags;
int32_t nSize;
char szName[8];
void *pData;
int32_t unk0x18;
};

struct D2ClientCofCacheStrc
{
int32_t bLoaded;
void *pBlock;
int32_t nBlockSize;
D2ClientCofCacheNodeStrc *pBuckets[512];
};

struct D2ClientCofNodeStrc
{
int32_t nAnimMode;
int32_t nWeaponClassCode;
D2ClientCofCacheNodeStrc *pCofFile;
int32_t field_0C;
struct D2ClientCofNodeStrc *pNext;
};

struct D2ClientDamageGroupStrc
{
int32_t nMin;
int32_t nMax;
int32_t nLengthFrames;
int32_t nPoisonCount;
int32_t bPresent;
int32_t bIsRange;
};

struct D2ClientDialogLineStrc
{
Unicode wszText[120];
int32_t nLineHeight;
int32_t nCenterOffset;
int32_t nTextWidth;
int32_t nUnusedParam;
int32_t nFont;
int32_t nBaseColor;
int32_t (__cdecl *pfnOnSelect)();
int32_t bSelectable;
};

typedef int32_t (__stdcall *DIALOGCALLBACK)(int32_t);

struct D2ClientDialogStrc
{
uint32_t dwCreateTick;
int32_t field_04;
D2CellFileStrc *hImage;
int32_t bOwnsImage;
int32_t bLinePressActive;
int32_t bVAlignByStaticLines;
int32_t nInputMode;
int32_t bIgnoreCancelKey;
int32_t bGrabsKeyboard;
int32_t nCenterX;
int32_t nCenterY;
int32_t nLeft;
int32_t nTop;
int32_t nWidth;
int32_t nHeight;
int32_t field_3C;
int32_t field_40;
int32_t nSelectedLine;
int32_t nPressedLine;
int32_t nSelectableCount;
int32_t NumLines;
uint32_t nFramesDrawn;
int32_t bAutoSize;
int32_t eHighlight;
DIALOGCALLBACK pfnCallback;
DIALOGCALLBACK pfnCallback2;
struct D2ClientDialogLineStrc Lines[10];
void *pControlList;
struct D2ClientDialogStrc *pNext;
};

struct D2ClientDrawNodeStrc
{
int32_t dwFlags;
int32_t nX;
int32_t nY;
void *pObject;
struct D2ClientDrawNodeStrc *pNext;
};

struct D2ClientEditBoxStrc
{
const D2UiEditBoxImplVtbl *pVtbl;
int32_t (__fastcall *pfnCharFilter)(D2ClientEditBoxStrc *, SMSGHANDLER_PARAMS *);
int32_t nBoxX;
int32_t nBoxY;
int32_t nTextX;
int32_t nTextY;
int32_t nWidth;
int32_t nLineCount;
int32_t bNumericOnly;
int32_t bAllowKeyRepeat;
Unicode *pwszText;
Unicode *pwszTextAlt;
int32_t nMaxValue;
uint16_t nMaxLen;
int32_t nCaretPos;
int32_t nTextLength;
int32_t bMouseDown;
int32_t (__cdecl *pfnCallback)();
int32_t nBlinkTick;
int32_t bCaretVisible;
int32_t nFont;
D2CellFileStrc *hCelFile;
int32_t bDrawBackground;
};

struct D2ClientEnvParticleStrc
{
uint16_t wInUse;
uint16_t wPad02;
int32_t nX;
int32_t nY;
int32_t nGroundY;
int32_t nSize;
int32_t nSpeed;
int32_t nSwayPhase;
int32_t bLanded;
int32_t nLingerFrames;
uint8_t nColour;
uint8_t nTransIndex;
uint8_t pad26[2];
};

struct D2ClientEnvSplashStrc
{
uint16_t wInUse;
uint16_t wPad02;
int32_t nX;
int32_t nY;
int32_t nWhichRain;
int32_t nFrame;
int32_t nFrameDelay;
};

struct D2ClientEscMenuInfoStrc
{
int32_t nItemCount;
int32_t nRowHeight;
int32_t nLabelYOffset;
int32_t nPentYOffset;
int32_t nSliderYOffset;
int32_t field_14;
};

typedef int32_t (__cdecl *ESCMENUENABLEDFN)();

typedef void (__fastcall *ESCMENUITEMFN)(struct D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg);

struct D2ClientEscMenuItemStrc
{
int32_t dwType;
int32_t bExpansionOnly;
int32_t nScreenY;
char szImageFile[260];
ESCMENUENABLEDFN pfnIsEnabled;
ESCMENUITEMFN pfnOnChange;
ESCMENUITEMFN pfnRefresh;
ESCMENUITEMFN pfnOnDraw;
uint32_t dwCount;
int32_t nValue;
int32_t bUniformTrack;
char pcChoiceFile[4][260];
D2CellFileStrc *hImage;
D2CellFileStrc *hChoiceImage[4];
};

struct D2ClientEscMenuUiWindowStrc
{
int32_t bCloseOnEscMenu;
int32_t bRestoreFlag;
int32_t nSavedState;
};

struct D2ClientFloorVertexStrc
{
uint8_t nIntensity;
uint8_t nRed;
uint8_t nGreen;
uint8_t nBlue;
int32_t nScreenX;
int32_t nScreenY;
};

struct D2ClientGfxEntryStrc
{
D2C_OverlayTypes nKind;
int32_t nOverlayId;
int32_t field_08;
int32_t nX;
int32_t nY;
int32_t nFrameRate;
int32_t nFrame;
int32_t nFrameCount;
int32_t nOffsetX;
int32_t nOffsetY;
int32_t nLoopWaitTime;
uint32_t dwLastTick;
int32_t field_30;
int32_t bPreDraw;
int32_t nStateId;
int32_t bAuraActive;
D2ClientLightStrc *pLight;
char field_44[96];
struct D2ClientGfxEntryStrc *pNext;
};

struct D2ClientHirelingInitStrc
{
uint16_t wNameId;
uint8_t pad0x02[2];
uint32_t nLowSeed;
};

struct D2ClientInterface
{
int32_t (__fastcall *pEntrypoint)(D2ConfigStrc *pConfig);
};

struct D2ClientLevelEntryTitleStrc
{
D2C_Levels nLevelId;
D2Quests nQuestId;
char szTitle[5];
};

struct D2ClientLightStrc
{
D2C_UnitTypes dwUnitType;
uint32_t dwUnitId;
uint8_t bUnitIsAsync;
uint8_t _gap0[3];
int32_t nType;
int32_t nSubtileX;
int32_t nSubtileY;
int32_t nRadius;
int32_t nTargetRadius;
int32_t bOrphaned;
uint8_t nIntensity;
uint8_t nRed;
uint8_t nGreen;
uint8_t nBlue;
struct D2ClientLightStrc *pNext;
int32_t fStaticValid;
int32_t *pnStaticMap;
};

struct D2ClientListBoxEntryStrc
{
Unicode wszCol[6][1024];
uint16_t field_3000;
int32_t nUserData;
int32_t nColor;
int32_t nFont;
};

struct D2ClientListBoxStrc
{
const D2ListBoxImplVtbl *pVtbl;
int32_t field_04;
struct D2ClientListBoxEntryStrc entries[128];
int32_t nX;
int32_t nY;
int32_t nWidth;
int32_t nHeight;
int32_t nScrollBarRows;
int32_t field_18071C[5];
int32_t nTopEntry;
int32_t nHoveredEntry;
int32_t nSelection;
int32_t nStringsStored;
int32_t nDefaultColor;
int32_t nFont;
uint16_t wNumEntries;
int32_t (__cdecl *pfnOnSelect)();
int32_t nVisibleRows;
int32_t nThumbTop;
int32_t nThumbBottom;
int32_t nScrollBarX;
int32_t nScrollBarY;
int32_t nTrackRows;
int32_t nLineHeight;
int32_t nEntryGap;
uint32_t nHoldTicks;
uint32_t nDrawCount;
};

typedef void (__fastcall *D2ClientMessageCallbackFn)(int32_t bNoDefaultCursor);

struct D2ClientMissileTargetCtxStrc
{
D2UnitStrc *pMissile;
D2UnitStrc *pOwner;
D2MissilesTxt *pMissilesTxt;
};

struct D2ClientModePreloadStrc
{
int32_t bLoaded[20];
int32_t bQueued[20];
};

struct D2ClientMonCofTokensStrc
{
int16_t nMonsterId;
uint8_t nUnused02;
uint8_t nCofDirections;
uint8_t nVariants[3][16];
};

struct D2ClientMonModeDescStrc
{
int32_t nPathStep;
int32_t nAnimEnd;
int32_t nSplEndTrigger;
};

struct D2ClientMonModeTokenOverrideStrc
{
uint32_t dwToken;
int32_t nMonMode;
};

struct D2ClientMonPalShiftStrc
{
int32_t nMonsterClassId;
struct D2ClientPalShiftStrc *pPalShift;
};

struct D2ClientNearbyTargetCtxStrc
{
D2UnitStrc *pUnit;
int32_t nAccepted;
void *pUserData;
};

struct D2ClientNpcSpeechStrc
{
D2C_Sounds nSoundId;
D2C_Sounds nAltSoundId;
D2C_Sounds nDayCycleSoundBase;
D2C_Sounds nSoundIdKind2;
D2C_Sounds nLastPicked;
int32_t nLastFrame;
};

struct D2ClientObjectModeSoundStrc
{
D2C_Sounds nModeSounds[8];
D2C_Sounds nAltSound1;
int32_t nAltMode1;
D2C_Sounds nAltSound2;
int32_t nAltMode2;
};

struct D2ClientPalShiftStrc
{
int32_t nMonsterClassId;
uint8_t nShift[8][256];
uint8_t nGreenShift[8][256];
struct D2ClientPalShiftStrc *pNext;
};

struct D2ClientParticleStrc
{
uint32_t dwFlags;
int32_t nX;
int32_t nY;
int32_t nZ;
int32_t nVelX;
int32_t nVelY;
int32_t nVelZ;
int32_t nAccelX;
int32_t nAccelY;
int32_t nAccelZ;
int32_t nTargetX;
int32_t nTargetY;
int32_t nTargetZ;
int32_t nOffsetX;
int32_t nOffsetY;
int32_t nOffsetZ;
int32_t nBouncesLeft;
int32_t nRestitution;
int32_t nDurationFrames;
};

struct D2ClientPartyButtonStrc
{
int32_t field_00;
int32_t nX;
int32_t nY;
D2CellFileStrc *hImage;
int32_t nFrame;
int32_t nButtonType;
D2C_PartyButtons eButton;
int32_t field_1C;
};

struct D2ClientPartyRowStrc
{
struct D2ClientPartyButtonStrc Buttons[5];
char szName[25];
int32_t nClassId;
int32_t nLevel;
int32_t nLevelId;
uint32_t dwUnitId;
int32_t nNameBandBottom;
struct D2ClientPartyRowStrc *pNext;
};

struct D2ClientPetIconCacheStrc
{
D2CellFileStrc *pIcon[5];
};

struct D2ClientPetInfoStrc
{
uint32_t nSeed;
uint16_t wName;
int16_t unk0x06;
int32_t nDisplayState;
};

struct D2ClientPopupStrc
{
struct D2ClientTextLineStrc *pLines;
int32_t nLineCount;
int32_t nScrollPixels;
int32_t nScrollStepPixels;
uint16_t wWidth;
uint16_t wHeight;
uint16_t wScrollRate;
uint16_t field_16;
uint16_t field_18;
uint32_t dwLastTick;
uint32_t dwStartTick;
int32_t nStepTicks;
int32_t nAccumTicks;
uint16_t wSameTickDraws;
};

struct D2ClientRecordFileHeaderStrc
{
int32_t nVersion;
int32_t nHolders;
int32_t nRecords;
int32_t nEndFrame;
int32_t nInitSeed;
};

struct D2ClientRecordStrc
{
int32_t nFrame;
uint32_t nMsg;
uint32_t nWParam;
int32_t nLParam;
};

struct D2ClientRecordHolderStrc
{
struct D2ClientRecordStrc tRecords[500];
int32_t nRecordWriteCurr;
int32_t nRecordReadCurr;
struct D2ClientRecordHolderStrc *ptNext;
};

struct D2ClientShowItemLabelStrc
{
int32_t nLeft;
int32_t nTop;
int32_t nRight;
int32_t nBottom;
struct D2UnitStrc *pItem;
struct Unicode wszText[128];
uint32_t dwBgColor;
int32_t eDrawMode;
int32_t nTextColor;
};

struct D2ClientShrineChartStrc
{
void (__fastcall *pfnApplyOverlays)(D2UnitStrc *pObject, D2ShrinesTxt *ptShrineData);
void (__fastcall *pfnClientEffect)(D2UnitStrc *pShrine, D2UnitStrc *pPlayer, D2ShrinesTxt *ptShrineData);
D2_OverlayIds nOverlayId;
D2_OverlayIds nShimmerOverlay;
D2C_Sounds nSoundId;
};

struct D2ClientSkillTreeChartStrc
{
D2CellFileStrc *pImage;
char szFileName[30];
};

struct D2ClientSpeechSoundStrc
{
D2C_Sounds nSoundId;
D2C_StringIndices nTblStringId;
uint16_t pad0x06;
};

struct D2ClientSpeechSubtitleStrc
{
uint16_t nSoundId;
uint16_t nStringId;
};

struct D2ClientSpinnerStrc
{
const D2UiSpinnerImplVtbl *pVtbl;
int32_t field_04;
int32_t nX;
int32_t nY;
int32_t nUpX;
int32_t nUpY;
int32_t nDownX;
int32_t nDownY;
int32_t bWheelStep;
int32_t nCelHeight;
int32_t nCelWidth;
int32_t bHorizontal;
int32_t bMouseDown;
int32_t bUpPressed;
int32_t bDownPressed;
void (__cdecl *pfnOnChange)();
int32_t nRepeatTicks;
int32_t dwPressTick;
int32_t dwLastStepTick;
int32_t bFirstStep;
D2CellFileStrc *hCelFile;
};

struct D2ClientStatDescStrc
{
D2C_ItemStats nStatId;
int32_t nDescFunc;
D2C_StringIndices wDescStrPos;
D2C_StringIndices wDescStrNeg;
int32_t nDescVal;
};

struct D2ClientSubMissileSpawnStrc
{
struct D2UnitStrc *pOwner;
struct D2UnitStrc *pMissile;
int32_t nSkillId;
int32_t nSkillLevel;
int32_t nMaxTargets;
int32_t nSubMissileId;
};

struct D2ClientTargetCycleStrc
{
struct D2UnitStrc *pBest;
uint32_t dwBestId;
struct D2UnitStrc *pWrapped;
uint32_t dwWrappedId;
uint32_t dwAfterId;
int32_t nSeen;
};

struct D2ClientTextLineStrc
{
struct Unicode *pszText;
struct D2ClientTextLineStrc *pNext;
};

struct D2ClientTextNodeStrc
{
struct Unicode *pLines[6];
uint16_t wLineCount;
int32_t nColor;
uint32_t dwExpireTick;
struct D2ClientTextNodeStrc *pNext;
};

struct D2ClientUnitGfxInfoStrc
{
uint16_t wModeLoaded[20];
struct D2ClientCofNodeStrc *pAnimList;
struct D2ClientGfxEntryStrc *pOverlayList;
void *pParticles;
int32_t nPaletteShift1;
int32_t nPaletteShift2;
};

struct D2ClientUnitUpdateStrc
{
uint8_t nUnitType;
int32_t nUnitGUID;
uint16_t nX;
uint16_t nY;
};

struct D2ClientViewStrc
{
int32_t dwFlags;
struct tagRECT rcView;
struct tagRECT rcClip;
int32_t nOriginX;
int32_t nOriginY;
void *pGouraudTblX;
void *pGouraudTblY;
void *pGouraudTblXVal;
int32_t dwLayerMask;
struct D2ClientDrawNodeStrc pDrawNodes[3000];
int32_t nDrawNodesUsed;
int32_t nGridOriginX;
int32_t nGridOriginY;
struct D2ClientWallSquareStrc *pSquares;
int32_t nSquareCount;
int32_t nGridSide;
int32_t nTilesAcross;
int32_t nTilesDown;
};

struct D2ClientWallRoomArgsStrc
{
D2ClientViewStrc *pView;
struct tagRECT rcClip;
D2ActiveRoomStrc *pPlayerRoom;
int32_t bSkipUnits;
};

struct D2ClientWallSquareStrc
{
int32_t field_00;
int32_t field_04;
int32_t nGameX;
int32_t nGameY;
struct D2ClientDrawNodeStrc *pShadowList;
struct D2ClientDrawNodeStrc *pWallList;
struct D2ClientDrawNodeStrc *pUnitList;
struct D2ClientDrawNodeStrc *pFloorList;
struct D2ClientDrawNodeStrc *pTileList;
};

struct D2ClientWaypointActStrc
{
uint8_t bHasWaypoint;
int32_t nFirstLevelId;
int32_t nLastLevelId;
uint8_t nFirstWaypointNo;
uint8_t nLastWaypointNo;
};

struct D2CofHeaderStrc
{
uint8_t unk0x00;
uint8_t unk0x01;
uint8_t nDirections;
uint8_t unk0x03;
int32_t unk0x04;
int32_t nXMin;
int32_t nXMax;
int32_t nYMin;
int32_t nYMax;
};

struct D2CollisionFieldStrc
{
int32_t nWidth;
int32_t nHeight;
int32_t nArea;
int32_t nCenterX;
int32_t nCenterY;
};

struct D2ColorsTxt
{
uint32_t dwCode;
};

struct D2CompCodeTxt
{
uint32_t dwCode;
};

struct D2CompositTxt
{
char szName[32];
char szToken[20];
};

struct D2ConfigCTempStrc
{
uint8_t nUnk;
uint8_t nCharacterClassId;
uint16_t nCharacterSaveFlags;
};

struct D2ConfigStrc
{
BOOL bIsExpansion;
uint8_t bWindow;
uint8_t b3DFX;
uint8_t bOpenGL;
uint8_t bRave;
uint8_t bD3D;
uint8_t bPerspective;
uint8_t bQuality;
uint32_t dwGamma;
uint8_t bVSync;
uint32_t dwFramerate;
eD2GameTypes dwGameType;
uint16_t wJoinID;
char szGameName[24];
char szServerIP[24];
char szBattleNetIP[24];
char szMCPIP[24];
uint8_t _007A[4];
uint8_t bNoPK;
uint8_t bOpenC;
uint8_t bAmazon;
uint8_t bPaladin;
uint8_t bSorceress;
uint8_t bNecromancer;
uint8_t bBarbarian;
uint8_t bDruid;
uint8_t bAssassin;
uint8_t bInvincible;
uint8_t _0088[48];
char szName[24];
char szRealm[24];
D2CharacterPreviewInfoStrc tCharPreviewInfo;
char szUnk[194];
uint8_t _01D0[24];
union
{
uint32_t dwCTemp;
struct D2ConfigCTempStrc unpackedCTemp;
};
uint8_t bNoMonsters;
uint32_t dwMonsterClass;
uint8_t bMonsterInfo;
uint32_t dwMonsterDebug;
uint8_t bRare;
uint8_t bUnique;
uint8_t _01F8[2];
int32_t dwAct;
uint8_t bNoPreload;
uint8_t bDirect;
uint8_t bLowEnd;
uint8_t bNoCompress;
uint16_t wArena;
uint32_t nArenaFlags;
uint8_t nArenaTemplate;
uint8_t _0209[2];
uint8_t nArenaDifficulty;
BOOL (__stdcall *pAllowExpansionCallback)();
uint8_t bTxt;
uint8_t bLog;
uint8_t bMsgLog;
uint8_t bSafeMode;
uint8_t bNoSave;
uint32_t dwSeed;
uint8_t bCheats;
uint8_t bTeen;
uint8_t bNoSound;
uint8_t bQuests;
uint8_t _021D;
uint8_t bBuild;
struct BnClientInterface *pComInterface;
uint32_t nTokenId;
char szCharacterRealm[24];
char szGamePassword[24];
char szGameStatstring[256];
uint8_t bSkipToBNet;
D2C_LaunchType nLaunchType;
uint8_t bShownLogo;
uint8_t bUnk035A;
char szCurrentChannelName[32];
char szDefaultChannelName[32];
uint8_t nComponents[16];
uint8_t nComponentsColors[16];
uint8_t nCharacterLevel;
uint8_t bLadder;
uint32_t nAccountPasswordHash;
uint32_t nAccountPasswordLength;
uint16_t nSaveFlags;
};

struct D2CoordStrc
{
int32_t nX;
int32_t nY;
};

struct D2CorpseStrc
{
uint32_t unk0x00;
uint32_t dwUnitId;
uint32_t unk0x08;
D2CorpseStrc *pNextCorpse;
};

struct D2CubeInputItem
{
uint16_t wInputFlags;
uint16_t wItem;
uint16_t wItemID;
uint8_t nQuality;
uint8_t nQuantity;
};

struct D2CubeOutputParamStrc
{
int32_t nMod;
uint16_t nModParam;
int16_t nModMin;
int16_t nModMax;
uint16_t nModChance;
};

struct D2CubeOutputItem
{
uint16_t wItemFlags;
uint16_t wBaseItemId;
uint16_t wItemID;
uint8_t nQuality;
uint8_t nQuantity;
uint8_t nType;
uint8_t nLvl;
uint8_t nPLvl;
uint8_t nILvl;
uint16_t wPrefixId[3];
uint16_t wSuffixId[3];
D2CubeOutputParamStrc pParam[5];
};

struct D2CubeMainTxt
{
uint8_t nEnabled;
uint8_t nLadder;
uint8_t nMinDiff;
uint8_t nClass;
uint8_t nOp;
uint8_t pad0x05[3];
int32_t nParam;
int32_t nValue;
int8_t nNumInputs;
uint8_t pad0x11;
uint16_t wVersion;
D2CubeInputItem pInputItem[7];
D2CubeOutputItem pOutputItem[3];
};

struct D2DataArrayStrc
{
char szName[256];
int32_t nSlotCount;
int32_t nSlotSize;
int32_t fValid;
int32_t nSearchCursor;
int32_t nHighWater;
int32_t nUsedCount;
uint16_t wSerial;
uint16_t unk0x11A;
void *pSlots;
};

struct D2GambleDataTbl
{
int32_t nGambleTxtRecordCount;
uint32_t *pGambleSelection;
int32_t pGambleChooseLimit[100];
};

struct D2GemDataTbl
{
int32_t nGemsTxtRecordCount;
D2GemsTxt *pGemsTxt;
};

struct D2ItemDataTbl
{
int32_t nItemsTxtRecordCount;
D2ItemsTxt *pItemsTxt;
D2ItemsTxt *pWeapons;
int32_t nWeaponsTxtRecordCount;
D2ItemsTxt *pArmor;
int32_t nArmorTxtRecordCount;
D2ItemsTxt *pMisc;
int32_t nMiscTxtRecordCount;
};

struct D2ItemRatioDataTbl
{
int32_t nItemRatioTxtRecordCount;
D2ItemRatioTxt *pItemRatioTxt;
};

struct D2LowQualityItemDataTbl
{
int32_t nLowQualityItemsTxtRecordCount;
D2LowQualityItemsTxt *pLowQualityItemsTxt;
};

struct D2MagicAffixDataTbl
{
int32_t nMagicAffixTxtRecordCount;
D2MagicAffixTxt *pMagicAffixTxt;
D2MagicAffixTxt *pMagicSuffix;
D2MagicAffixTxt *pMagicPrefix;
D2MagicAffixTxt *pAutoMagic;
};

struct D2MonItemPercentDataTbl
{
int32_t nMonItemPercentTxtRecordCount;
D2MonItemPercentTxt *pMonItemPercentTxt;
};

struct D2MonModeDataTbl
{
int32_t nMonModeTxtRecordCount;
D2MonModeTxt *pMonModeTxt;
D2MonModeTxt *pMonMode[2];
};

struct D2ObjModeDataTbl
{
int32_t nObjModeTypeTxtRecordCount;
D2ObjModeTypeTxt *pObjModeTypeTxt;
D2ObjModeTypeTxt *pObjType;
D2ObjModeTypeTxt *pObjMode;
};

struct D2PlrModeDataTbl
{
int32_t nPlrModeTypeTxtRecordCount;
D2PlrModeTypeTxt *pPlrModeTypeTxt;
D2PlrModeTypeTxt *pPlayerType;
D2PlrModeTypeTxt *pPlayerMode;
};

struct D2QualityItemDataTbl
{
int32_t nQualityItemsTxtRecordCount;
D2QualityItemsTxt *pQualityItemsTxt;
};

struct D2RareAffixDataTbl
{
int32_t nRareAffixTxtRecordCount;
D2RareAffixTxt *pRareAffixTxt;
D2RareAffixTxt *pRareSuffix;
D2RareAffixTxt *pRarePrefix;
};

struct D2RuneDataTbl
{
int32_t nRunesTxtRecordCount;
D2RunesTxt *pRunesTxt;
};

struct __declspec(align(4)) D2DataTablesStrc
{
D2PlayerClassTxt *pPlayerClassTxt;
D2TxtLinkStrc *pPlayerClassLinker;
D2BodyLocsTxt *pBodyLocsTxt;
D2TxtLinkStrc *pBodyLocsLinker;
D2StorePageTxt *pStorePageTxt;
D2TxtLinkStrc *pStorePageLinker;
D2ElemTypesTxt *pElemTypesTxt;
D2TxtLinkStrc *pElemTypesLinker;
D2HitClassTxt *pHitClassTxt;
D2TxtLinkStrc *pHitClassLinker;
D2MonModeTxtStub *pMonModeTxtStub;
D2TxtLinkStrc *pMonModeLinker;
D2PlrModeTxtStub *pPlrModeTxtStub;
D2TxtLinkStrc *pPlrModeLinker;
D2SkillCalcTxt *pSkillCalcTxt;
D2TxtLinkStrc *pSkillCalcLinker;
FOGASTNodeStrc *pSkillsCode;
uint32_t nSkillsCodeSize;
uint32_t nSkillsCodeSizeEx;
FOGASTNodeStrc *pSkillDescCode;
uint32_t nSkillDescCodeSize;
uint32_t nSkillDescCodeSizeEx;
D2MissCalcTxt *pMissileCalcTxt;
D2TxtLinkStrc *pMissileCalcLinker;
FOGASTNodeStrc *pMissCode;
uint32_t nMissCodeSize;
int32_t nMissCodeSizeEx;
const char *pSkillCode;
D2TxtLinkStrc *iSkillCode;
D2EventsTxt *pEventsTxt;
D2TxtLinkStrc *pEventsLinker;
D2CompCodeTxt *pCompCodeTxt;
D2TxtLinkStrc *pCompCodeLinker;
int32_t nCompCodeTxtRecordCount;
D2MonAiTxt *pMonAiTxt;
D2TxtLinkStrc *pMonAiLinker;
int32_t nMonAiTxtRecordCount;
D2TxtLinkStrc *pItemsLinker;
FOGASTNodeStrc *pItemsCode;
uint32_t nItemsCodeSize;
int32_t nItemsCodeSizeEx;
D2PropertiesTxt *pPropertiesTxt;
D2TxtLinkStrc *pPropertiesLinker;
int32_t nPropertiesTxtRecordCount;
D2TxtLinkStrc *pRunesLinker;
D2HireDescTxt *pHireDescTxt;
D2TxtLinkStrc *pHireDescLinker;
D2StatesTxt *pStatesTxt;
D2TxtLinkStrc *pStatesLinker;
int32_t nStatesTxtRecordCount;
uint32_t *pStateMasks;
uint32_t *fStateMasks[40];
int16_t *pProgressiveStates;
int16_t nProgressiveStates;
uint8_t pad0x172[2];
int16_t *pCurseStates;
int16_t nCurseStates;
uint8_t pad0x17A[2];
int16_t *pTransformStates;
int16_t nTransformStates;
uint8_t pad0x182[2];
int16_t *pActionStates;
int16_t nActionStates;
uint8_t pad0x18A[2];
int16_t *pColourStates;
int16_t nColourStates;
uint8_t pad0x192[2];
D2SoundsTxtStub *pSoundsTxtCodes;
D2TxtLinkStrc *pSoundsLinker;
int32_t nSoundsTxtCodes;
D2HirelingTxt *pHirelingTxt;
int32_t nHirelingTxtRecordCount;
int32_t nClassicHirelingStartRecordIds[256];
int32_t nExpansionHirelingStartRecordIds[256];
D2NpcTxt *pNpcTxt;
int32_t nNpcTxtRecordCount;
D2ColorsTxt *pColorsTxt;
D2TxtLinkStrc *pColorsLinker;
D2TxtLinkStrc *pTreasureClassExLinker;
D2TCExShortStrc *pTreasureClassEx;
int32_t nTreasureClassEx;
D2TCExShortStrc *pChestTreasureClasses[45];
D2MonStatsTxt *pMonStatsTxt;
D2TxtLinkStrc *pMonStatsLinker;
int32_t nMonStatsTxtRecordCount;
D2MonSoundsTxt *pMonSoundsTxt;
D2TxtLinkStrc *pMonSoundsLinker;
int32_t nMonSoundsTxtRecordCount;
D2MonStats2Txt *pMonStats2Txt;
D2TxtLinkStrc *pMonStats2Linker;
int32_t nMonStats2TxtRecordCount;
D2MonPlaceTxt *pMonPlaceTxt;
D2TxtLinkStrc *pMonPlaceLinker;
int32_t nMonPlaceTxtRecordCount;
D2MonPresetTxt *pMonPresetTxt;
D2MonPresetTxt *pMonPresetTxtActSections[5];
int32_t nMonPresetTxtActRecordCounts[5];
D2SuperUniquesTxt *pSuperUniquesTxt;
D2TxtLinkStrc *pSuperUniquesLinker;
int32_t nSuperUniquesTxtRecordCount;
int16_t nSuperUniqueIds[66];
D2MissilesTxt *pMissilesTxt;
D2TxtLinkStrc *pMissilesLinker;
int32_t nMissilesTxtRecordCount;
D2MonLvlTxt *pMonLvlTxt;
int32_t nMonLvlTxtRecordCount;
D2AnimSeqTxt *pMonSeqTxt;
D2TxtLinkStrc *pMonSeqLinker;
int32_t nMonSeqTxtRecordCount;
D2AnimSeqRecordStrc *pMonSeqTable;
int32_t nMonSeqTableRecordCount;
D2SkillDescTxt *pSkillDescTxt;
D2TxtLinkStrc *pSkillDescLinker;
int32_t nSkillDescTxtRecordCount;
D2SkillsTxt *pSkillsTxt;
D2TxtLinkStrc *pSkillsLinker;
int32_t nSkillsTxtRecordCount;
int32_t *nClassSkillCount;
int32_t nHighestClassSkillCount;
int16_t *nClassSkillList;
int32_t nPassiveSkills;
uint16_t *pPassiveSkills;
D2TxtLinkStrc *pOverlayLinker;
D2OverlayTxt *pOverlayTxt;
int32_t nOverlayTxtRecordCount;
D2CharStatsTxt *pCharStatsTxt;
int32_t nCharStatsTxtRecordCount;
D2ItemStatCostTxt *pItemStatCostTxt;
D2TxtLinkStrc *pItemStatCostLinker;
int32_t nItemStatCostTxtRecordCount;
uint16_t *pStatsWithDescFunc;
int32_t nStatsWithDescFunc;
D2MonEquipTxt *pMonEquipTxt;
int32_t nMonEquipTxtRecordCount;
D2PetTypeTxt *pPetTypeTxt;
D2TxtLinkStrc *pPetTypeLinker;
int32_t nPetTypeTxtRecordCount;
D2TxtLinkStrc *pItemTypesLinker;
D2ItemTypesTxt *pItemTypesTxt;
int32_t nItemTypesTxtRecordCount;
int32_t nItemTypesIndex;
uint32_t *pItemTypesEquivalenceLUTs;
D2TxtLinkStrc *pSetsLinker;
D2SetsTxt *pSetsTxt;
int32_t nSetsTxtRecordCount;
D2TxtLinkStrc *pSetItemsLinker;
D2SetItemsTxt *pSetItemsTxt;
int32_t nSetItemsTxtRecordCount;
D2TxtLinkStrc *pUniqueItemsLinker;
D2UniqueItemsTxt *pUniqueItemsTxt;
int32_t nUniqueItemsTxtRecordCount;
D2TxtLinkStrc *pMonPropLinker;
D2MonPropTxt *pMonPropTxt;
int32_t nMonPropTxtRecordCount;
D2TxtLinkStrc *pMonTypeLinker;
D2MonTypeTxt *pMonTypeTxt;
int32_t nMonTypeTxtRecordCount;
uint32_t *pMonTypeNest;
int32_t nMonTypeIndex;
D2TxtLinkStrc *pMonUModLinker;
D2MonUModTxt *pMonUModTxt;
int32_t nMonUModTxtRecordCount;
D2LevelsTxt *pLevelsTxt;
int32_t nLevelsTxtRecordCount;
D2LevelDefBin *pLevelDefBin;
D2LvlPrestTxt *pLvlPrestTxt;
int32_t nLvlPrestTxtRecordCount;
int32_t nStuff;
int32_t nShiftedStuff;
D2AnimDataTableStrc *pAnimData;
D2ExperienceDataTbl *pExperienceTxt;
D2DifficultyLevelsTxt *pDifficultyLevelsTxt;
int32_t nDifficultyLevelsTxtRecordCount;
int32_t bCompileTxt;
int32_t ExpFieldI[9];
uint8_t *pFieldData;
D2CollisionFieldStrc pCollisionField;
D2CubeMainTxt *pCubeMainTxt;
int32_t nCubeMainTxtRecordCount;
int32_t nInventoryTxtRecordCount;
D2InventoryTxt *pInventoryTxt;
uint32_t unk0xCD4;
D2ItemDataTbl pItemDataTables;
uint16_t *pIndexOldToCurrent;
D2GemDataTbl pGemDataTables;
D2LowQualityItemDataTbl pLowQualityItemDataTables;
D2BookDataTbl pBookDataTables;
D2RareAffixDataTbl pRareAffixDataTables;
D2ItemRatioDataTbl pItemRatioDataTables;
uint32_t unk0xD2C;
D2GambleDataTbl pGambleDataTables;
D2MagicAffixDataTbl pMagicAffixDataTables;
D2RuneDataTbl pRuneDataTables;
D2QualityItemDataTbl pQualityItemDataTables;
uint32_t unk0xEEC;
D2SeedStrc pAutomapSeed;
D2AutomapRandStrc pAutoMapRand[36];
D2LvlTypesTxt *pLvlTypesTxt;
int32_t *pPortalLevels;
int32_t nPortalLevels;
int32_t nLvlTypesTxtRecordCount;
D2LvlWarpTxt *pLvlWarpTxt;
int32_t nLvlWarpTxtRecordCount;
D2LvlMazeTxt *pLvlMazeTxt;
int32_t nLvlMazeTxtRecordCount;
D2LvlSubTxt *pLvlSubTxt;
int32_t nLvlSubTxtRecordCount;
int32_t *pLvlSubTypeStartIds;
D2TileLibraryHashStrc **ppTileLibraryHash;
D2DrlgFileStrc **ppLvlPrestFiles;
D2DrlgFileStrc **ppLvlSubTypeFiles;
D2AutomapShortStrc *pAutoMap;
int32_t nAutoMap;
char szDefaultString;
char pad0x1059[3];
D2MonItemPercentDataTbl pMonItemPercentDataTables;
Unicode wszDefault;
uint16_t unk0x1066;
D2UniqueTitleTxt *pUniqueTitleTxt;
D2UniquePrefixTxt *pUniquePrefixTxt;
D2UniqueSuffixTxt *pUniqueSuffixTxt;
D2UniqueAppellationTxt *pUniqueAppellationTxt;
int32_t nUniqueTitleTxtRecordCount;
int32_t nUniquePrefixTxtRecordCount;
int32_t nUniqueSuffixTxtRecordCount;
int32_t nUniqueAppellationTxtRecordCount;
uint32_t unk7[2];
int16_t nTreasureClassItemTypes;
int16_t unk0x1092;
D2ShrinesTxt *pShrinesTxt;
int32_t nShrinesTxtRecordCount;
D2ObjectsTxt *pObjectsTxt;
int32_t nObjectsTxtRecordCount;
D2ObjGroupTxt *pObjGroupTxt;
int32_t nObjGroupTxtRecordCount;
D2ArmTypeTxt *pArmTypeTxt;
D2MonModeDataTbl pMonModeDataTables;
D2CompositTxt *pCompositTxt;
D2ObjModeDataTbl pObjModeDataTables;
D2PlrModeDataTbl pPlrModeDataTables;
};

struct D2DifficultyLevelsTxt
{
uint32_t dwResistPenalty;
uint32_t dwDeathExpPenalty;
uint32_t dwUberCodeOddsNorm;
uint32_t dwUberCodeOddsGood;
uint32_t dwMonsterSkillBonus;
uint32_t dwMonsterFreezeDiv;
uint32_t dwMonsterColdDiv;
uint32_t dwAiCurseDiv;
uint32_t dwUltraCodeOddsNorm;
uint32_t dwUltraCodeOddsGood;
uint32_t dwLifeStealDiv;
uint32_t dwManaStealDiv;
uint32_t dwUniqueDmgBonus;
uint32_t dwChampionDmgBonus;
uint32_t dwHireableBossDmgPercent;
uint32_t dwMonsterCEDmgPercent;
uint32_t dwStaticFieldMin;
uint32_t dwGambleRare;
uint32_t dwGambleSet;
uint32_t dwGambleUniq;
uint32_t dwGambleUber;
uint32_t dwGambleUltra;
};

struct D2DrlgTileDataStrc
{
int32_t nDrawX;
int32_t nDrawY;
int32_t nPosX;
int32_t nPosY;
D2RoomCoordListStrc *pCoordList;
uint32_t dwFlags;
D2TileLibraryEntryStrc *pTile;
int32_t nTileType;
D2DrlgTileDataStrc *unk0x20;
uint32_t dwFadeState;
uint8_t nAlpha;
uint8_t nAlphaFrom;
uint8_t nAlphaTo;
uint8_t nIntensity;
uint32_t dwFadeEndTick;
};

struct D2DrlgActStrc
{
uint8_t nAct;
uint8_t pad0x01[3];
D2ActiveRoomStrc *pRoom;
D2DrlgStrc *pDrlg;
uint32_t dwInitSeed;
int32_t nTownId;
D2DrlgEnvironmentStrc *pEnvironment;
void (__fastcall *pfnActCallBack)(D2ActiveRoomStrc *);
int32_t bClient;
int32_t bHasPendingRoomsUpdates;
int32_t bHasPendingRoomDeletions;
int32_t bHasPendingUnitListUpdates;
D2DrlgTileDataStrc pTileData;
void *pMemPool;
};

struct D2DrlgAnimTileGridStrc
{
D2DrlgTileDataStrc **ppMapTileData;
int32_t nFrames;
int32_t nCurrentFrame;
int32_t nAnimationSpeed;
D2DrlgAnimTileGridStrc *pNext;
};

struct D2DrlgBuildStrc
{
int32_t nPreset;
int32_t nDivisor;
int32_t nRand;
D2DrlgBuildStrc *pNext;
};

struct D2DrlgCoordStrc
{
int32_t nPosX;
int32_t nPosY;
int32_t nWidth;
int32_t nHeight;
};

struct D2DrlgDeleteStrc
{
int32_t nUnitType;
uint32_t nUnitGUID;
D2DrlgDeleteStrc *pNext;
};

struct D2DrlgEnvironmentStrc
{
int32_t nCycleIndex;
int32_t nPeriodOfDay;
int32_t nTicks;
int32_t nIntensity;
uint32_t dwInitTick;
int32_t unk0x14;
uint8_t nRed;
uint8_t nGreen;
uint8_t nBlue;
uint8_t pad0x1B;
float fCos;
float fLast;
float fSin;
int32_t nTimeRate;
int32_t nTimeRateIndex;
int32_t bEclipse;
int32_t nPrev;
};

struct D2DrlgFileStrc
{
int32_t nSubstMethod;
void *pDS1File;
int32_t unk0x08;
int32_t nWidth;
int32_t nHeight;
int32_t nWallLayers;
int32_t nFloorLayers;
void *pTileTypeLayer[4];
void *pWallLayer[4];
void *pFloorLayer[2];
void *pShadowLayer;
void *pSubstGroupTags;
int32_t nSubstGroups;
D2DrlgSubstGroupStrc *pSubstGroups;
D2PresetUnitStrc *pPresetUnit;
D2DrlgFileStrc *pNext;
};

struct D2DrlgGridStrc
{
int32_t *pCellsFlags;
int32_t *pCellsRowOffsets;
int32_t nWidth;
int32_t nHeight;
int32_t unk0x10;
};

struct D2DrlgLevelRectStrc
{
int32_t nPosX;
int32_t nPosY;
int32_t nWidth;
int32_t nHeight;
};

union D2DrlgLevelRectUnion
{
struct
{
int32_t nPosX;
int32_t nPosY;
int32_t nWidth;
int32_t nHeight;
};
D2DrlgCoordStrc pLevelCoords;
};

struct D2DrlgTileInfoStrc
{
int32_t nPosX;
int32_t nPosY;
int32_t nTileIndex;
};

struct D2DrlgLevelStrc
{
D2DrlgStrc *pDrlg;
int32_t nLevelId;
int32_t nLevelType;
int32_t nDrlgType;
uint32_t dwFlags;
__declspec(align(1)) D2SeedStrc pSeed;
uint32_t dwInitSeed;
union
{
struct
{
int32_t nPosX;
int32_t nPosY;
int32_t nWidth;
int32_t nHeight;
};
D2DrlgCoordStrc pLevelCoords;
};
D2DrlgRoomStrc *pFirstRoomEx;
int32_t nRooms;
union
{
D2DrlgPresetInfoStrc *pPreset;
D2DrlgOutdoorInfoStrc *pOutdoors;
D2LvlMazeTxt *pMaze;
};
D2DrlgMapStrc *pCurrentMap;
int32_t nCoordLists;
D2DrlgTileInfoStrc pTileInfo[32];
int32_t nTileInfo;
int32_t nRoom_Center_Warp_X[9];
int32_t nRoom_Center_Warp_Y[9];
int32_t nRoomCoords;
int32_t *pJungleDefs;
int32_t nJungleDefs;
D2DrlgBuildStrc *pBuild;
int32_t bActive;
uint32_t dwInactiveFrames;
int32_t *pPresetMaps;
D2DrlgLevelStrc *pNextLevel;
};

union D2DrlgLevelTypeDataUnion
{
D2DrlgPresetInfoStrc *pPreset;
D2DrlgOutdoorInfoStrc *pOutdoors;
D2LvlMazeTxt *pMaze;
};

struct D2DrlgLogicalRoomInfoStrc
{
int32_t dwFlags;
int32_t nLists;
D2DrlgGridStrc pIndexX;
D2DrlgGridStrc pIndexY;
D2RoomCoordListStrc *pCoordList;
};

struct D2DrlgMapStrc
{
int32_t nLevelPrest;
int32_t nPickedFile;
D2LvlPrestTxt *pLvlPrestTxtRecord;
D2DrlgFileStrc *pFile;
D2DrlgCoordStrc pDrlgCoord;
int32_t bHasInfo;
D2DrlgGridStrc pMapGrid;
D2PresetUnitStrc *pPresetUnit;
int32_t bInited;
int32_t nPops;
int32_t *pPopsIndex;
int32_t *pPopsSubIndex;
int32_t *pPopsOrientation;
D2DrlgCoordStrc *pPopsLocation;
D2DrlgMapStrc *pNext;
};

struct D2DrlgOrthStrc
{
union
{
D2DrlgRoomStrc *pDrlgRoom;
D2DrlgLevelStrc *pLevel;
};
uint8_t nDirection;
uint8_t unk0x05[3];
int32_t bPreset;
int32_t bInit;
D2DrlgCoordStrc *pBox;
D2DrlgOrthStrc *pNext;
};

union D2DrlgOrthTargetUnion
{
D2DrlgRoomStrc *pDrlgRoom;
D2DrlgLevelStrc *pLevel;
};

struct D2DrlgVertexStrc
{
int32_t nPosX;
int32_t nPosY;
uint8_t nDirection;
uint8_t pad0x09[3];
int32_t dwFlags;
D2DrlgVertexStrc *pNext;
};

struct D2DrlgOutdoorInfoStrc
{
uint32_t dwFlags;
D2DrlgGridStrc pGrid[4];
union
{
struct
{
int32_t nWidth;
int32_t nHeight;
int32_t nGridWidth;
int32_t nGridHeight;
};
D2DrlgCoordStrc pCoord;
};
D2DrlgVertexStrc *pVertex;
D2DrlgVertexStrc *pPathStarts[6];
D2DrlgVertexStrc pVertices[24];
int32_t nVertices;
D2DrlgOrthStrc *pRoomData;
};

struct D2DrlgOutdoorRoomStrc
{
D2DrlgGridStrc pTileTypeGrid;
D2DrlgGridStrc pWallGrid;
D2DrlgGridStrc pFloorGrid;
D2DrlgGridStrc pDirtPathGrid;
D2DrlgVertexStrc *pVertex;
int32_t dwFlags;
int32_t dwFlagsEx;
int32_t unk0x5C;
int32_t unk0x60;
int32_t nSubType;
int32_t nSubTheme;
int32_t nSubThemePicked;
};

struct D2DrlgOutdoorSizeStrc
{
int32_t nWidth;
int32_t nHeight;
int32_t nGridWidth;
int32_t nGridHeight;
};

union D2DrlgOutdoorSizeUnion
{
struct
{
int32_t nWidth;
int32_t nHeight;
int32_t nGridWidth;
int32_t nGridHeight;
};
D2DrlgCoordStrc pCoord;
};

struct D2DrlgPresetInfoStrc
{
D2DrlgMapStrc *pDrlgMap;
int32_t nDirection;
};

struct D2DrlgPresetRoomFlagsStrc
{
uint8_t nFlags;
uint8_t unk0x0D[3];
};

union D2DrlgPresetRoomFlagsUnion
{
struct
{
uint8_t nFlags;
uint8_t unk0x0D[3];
};
uint32_t dwFlags;
};

struct D2DrlgPresetRoomStrc
{
int32_t nLevelPrest;
int32_t nPickedFile;
D2DrlgMapStrc *pMap;
union
{
struct
{
uint8_t nFlags;
uint8_t unk0x0D[3];
};
uint32_t dwFlags;
};
D2DrlgGridStrc pWallGrid[4];
D2DrlgGridStrc pTileTypeGrid[4];
D2DrlgGridStrc pFloorGrid[2];
D2DrlgGridStrc pCellGrid;
D2DrlgGridStrc *pMazeGrid;
D2CoordStrc *pTombStoneTiles;
int32_t nTombStoneTiles;
};

union D2DrlgRoomCoordUnion
{
struct
{
int32_t nTileXPos;
int32_t nTileYPos;
int32_t nTileWidth;
int32_t nTileHeight;
};
D2DrlgCoordStrc pDrlgCoord;
};

struct __declspec(align(4)) D2DrlgRoomStrc
{
D2DrlgLevelStrc *pLevel;
union
{
struct
{
int32_t nTileXPos;
int32_t nTileYPos;
int32_t nTileWidth;
int32_t nTileHeight;
};
D2DrlgCoordStrc pDrlgCoord;
};
uint32_t dwFlags;
uint32_t dwOtherFlags;
int32_t nType;
union
{
D2DrlgPresetRoomStrc *pMaze;
D2DrlgOutdoorRoomStrc *pOutdoor;
};
uint32_t dwDT1Mask;
D2TileLibraryHashStrc *pTiles[32];
D2DrlgTileGridStrc *pTileGrid;
uint8_t fRoomStatus;
uint8_t unk0xAD;
uint16_t wRoomsInList[5];
D2DrlgRoomStrc *pStatusNext;
D2DrlgRoomStrc *pStatusPrev;
D2DrlgRoomStrc **ppRoomsNear;
int32_t nRoomsNear;
D2RoomTileStrc *pRoomTiles;
D2PresetUnitStrc *pPresetUnits;
D2DrlgOrthStrc *pDrlgOrth;
D2SeedStrc pSeed;
uint32_t dwInitSeed;
D2DrlgLogicalRoomInfoStrc *pLogicalRoomInfo;
D2ActiveRoomStrc *pRoom;
D2DrlgRoomStrc *pDrlgRoomNext;
};

struct D2DrlgRoomTileRectStrc
{
int32_t nTileXPos;
int32_t nTileYPos;
int32_t nTileWidth;
int32_t nTileHeight;
};

struct D2DrlgRoomTilesStrc
{
D2DrlgTileDataStrc *pWallTiles;
int32_t nWalls;
D2DrlgTileDataStrc *pFloorTiles;
int32_t nFloors;
D2DrlgTileDataStrc *pRoofTiles;
int32_t nRoofs;
};

union D2DrlgRoomTypeDataUnion
{
D2DrlgPresetRoomStrc *pMaze;
D2DrlgOutdoorRoomStrc *pOutdoor;
};

struct D2DrlgStrc
{
D2DrlgLevelStrc *pLevel;
void *pMempool;
struct HD2ARCHIVE__ *hArchive;
D2DrlgActStrc *pAct;
uint8_t nAct;
uint8_t padding0x11[3];
__declspec(align(1)) D2SeedStrc pSeed;
uint32_t dwStartSeed;
uint32_t dwGameLowSeed;
uint32_t dwFlags;
D2DrlgRoomStrc tStatusRoomsLists[4];
D2DrlgRoomStrc *pDrlgRoom;
uint8_t nRoomsInitSinceLastUpdate;
uint8_t nRoomsInitTimeout;
uint8_t padding0x3DE[2];
int32_t nAllocatedRooms;
int32_t nFreedRooms;
struct D2GameStrc *pGame;
uint8_t nDifficulty;
uint8_t pad0x3ED[3];
void (__stdcall *pfAutomap)(D2ActiveRoomStrc *);
void (__stdcall *pfTownAutomap)(int32_t, int32_t, int32_t, int32_t);
int32_t nStaffTombLevel;
int32_t nBossTombLevel;
D2TileLibraryHashStrc *pTiles[32];
int32_t bJungleInterlink;
D2DrlgWarpStrc *pWarp;
};

struct D2DrlgSubstGroupStrc
{
D2DrlgCoordStrc tBox;
int32_t field_10;
int32_t field_14;
};

struct D2DrlgTileGridStrc
{
D2DrlgTileLinkStrc *pMapLinks;
D2DrlgAnimTileGridStrc *pAnimTiles;
int32_t nWalls;
int32_t nFloors;
int32_t nShadows;
D2DrlgRoomTilesStrc pTiles;
};

struct D2DrlgTileLinkStrc
{
int32_t bFloor;
D2DrlgTileDataStrc *pMapTile;
D2DrlgTileLinkStrc *pNext;
};

struct D2DrlgWarpStrc
{
int32_t nLevel;
int32_t nVis[8];
int32_t nWarp[8];
D2DrlgWarpStrc *pNext;
};

union D2FP32_16
{
struct
{
uint16_t wOffsetX;
uint16_t wPosX;
uint16_t wOffsetY;
uint16_t wPosY;
};
struct
{
uint32_t dwPrecisionX;
uint32_t dwPrecisionY;
};
};

struct D2PathPointStrc
{
uint16_t X;
uint16_t Y;
};

struct D2DynamicPathStrc
{
D2FP32_16 tGameCoords;
int32_t dwClientCoordX;
int32_t dwClientCoordY;
D2PathPointStrc tTargetCoord;
D2PathPointStrc tPrevTargetCoord;
D2PathPointStrc tFinalTargetCoord;
D2ActiveRoomStrc *pRoom;
D2ActiveRoomStrc *pPreviousRoom;
int32_t dwCurrentPointIdx;
int32_t dwPathPoints;
void *unk0x2C;
D2UnitStrc *pUnit;
uint32_t dwFlags;
uint32_t unk0x38;
uint32_t dwPathType;
uint32_t dwPrevPathType;
uint32_t dwUnitSize;
uint32_t dwCollisionPattern;
uint32_t nFootprintCollisionMask;
uint32_t nMoveTestCollisionMask;
uint16_t nCollidedWithMask;
uint16_t padding0x56;
D2UnitStrc *pTargetUnit;
uint32_t dwTargetType;
uint32_t dwTargetId;
uint8_t nDirection;
uint8_t nNewDirection;
uint8_t nDiffDirection;
uint8_t unk0x67;
uint8_t dwSpeed;
uint8_t unk0x69;
__declspec(align(1)) D2CoordStrc tDirectionVector;
__declspec(align(1)) D2CoordStrc tVelocityVector;
char padding0x7A[2];
int32_t dwVelocity;
uint32_t nPreviousVelocity;
int32_t dwMaxVelocity;
int32_t dwAcceleration;
int32_t dwAccelerationsCounter;
uint8_t nDist;
uint8_t nDistMax;
uint8_t nIDAStarInitFScore;
uint8_t nStepNum;
uint8_t nDistance;
char padding0x95[3];
int32_t nDirOffset;
D2PathPointStrc PathPoints[78];
int32_t nSavedStepsCount;
D2PathPointStrc SavedSteps[10];
};

struct D2ElemTypesTxt
{
uint32_t dwCode;
};

struct D2ElixirStatLabelStrc
{
int32_t nFileIndex;
int32_t nAlwaysOne;
uint16_t wStrPositive;
uint16_t wStrNegative;
int32_t nAlwaysTwo;
};

struct D2EventsTxt
{
uint16_t wEvent;
};

struct D2ExperienceTxt
{
uint32_t dwClass[7];
uint32_t dwExpRatio;
};

struct D2ExperienceDataTbl
{
D2ExperienceTxt tMax;
D2ExperienceTxt aLevels[];
};

struct D2FP32_16PrecisionStrc
{
uint32_t dwPrecisionX;
uint32_t dwPrecisionY;
};

struct D2FP32_16WordsStrc
{
uint16_t wOffsetX;
uint16_t wPosX;
uint16_t wOffsetY;
uint16_t wPosY;
};

struct D2FieldStrc
{
int32_t nX;
int32_t nY;
};

struct D2FloorDrawCtxStrc
{
int32_t bLowQuality;
int32_t bPerspective;
int32_t bSplashWindow;
int32_t bBubbleWindow;
int32_t nWeatherIntensity;
int32_t nLastSplashFrame;
int32_t nLastBubbleFrame;
D2SeedStrc *pSplashSeed;
int32_t nUiShiftScreen;
struct D2ClientViewStrc *pView;
int32_t bDrawEdges;
int32_t nMinX;
int32_t nMaxX;
int32_t nMinY;
int32_t nMaxY;
int32_t nScreenOffsetX;
int32_t nScreenOffsetY;
int32_t field_44;
};

struct D2FogSystemInfoStrc
{
uint32_t bInitialized;
uint8_t unknown_04[8];
uint32_t dwTotalPhysicalBytes;
};

struct D2GSPacketClt15
{
uint8_t nHeader;
uint8_t nMessageType;
uint8_t nLang;
char szMessage[256];
char szName[16];
};

struct D2GSPacketClt5D
{
uint8_t nHeader;
uint8_t nType;
uint8_t nToggle;
uint32_t dwPlayerGUID;
};

struct D2GSPacketClt5E
{
uint8_t nHeader;
uint8_t nType;
int32_t dwPlayerGUID;
};

struct D2GSPacketClt66
{
uint8_t nHeader;
char szGameName[16];
uint8_t nGameType;
uint8_t nPlayerClass;
int8_t nTemplate;
int8_t nDifficulty;
char szClientName[16];
int16_t wArena;
uint32_t nGameFlags;
int8_t unk0x2B;
int8_t unk0x2C;
int8_t nLocale;
};

struct D2GSPacketClt67
{
uint8_t nHeader;
int32_t nTokenId;
uint16_t nGameId;
uint8_t nPlayerClass;
int32_t dwVersion;
uint8_t nLocale;
char szClientName[16];
};

struct D2GSPacketClt68
{
uint8_t nHeader;
};

struct D2GSPacketClt6A
{
uint8_t nHeader;
};

struct D2GSPacketClt6C
{
uint8_t nHeader;
int32_t nSendTick;
int32_t nHalfAveragePing;
};

struct D2GSPacketSrv01
{
uint8_t nHeader;
uint8_t nDifficulty;
uint32_t dwArenaFlags;
uint8_t nExpansion;
uint8_t nLadder;
};

struct D2GSPacketSrv02
{
uint8_t nHeader;
};

struct D2GSPacketSrv03
{
uint8_t nHeader;
uint8_t nAct;
uint32_t dwGameSeed;
uint16_t nTownLevelId;
uint32_t nObjectSeed;
};

struct D2GSPacketSrv04
{
uint8_t nHeader;
};

struct D2GSPacketSrv05
{
uint8_t nHeader;
};

struct D2GSPacketSrv06
{
uint8_t nHeader;
};

struct D2GSPacketSrv07
{
uint8_t nHeader;
uint16_t nX;
uint16_t nY;
uint8_t nLevelId;
};

struct D2GSPacketSrv08
{
uint8_t nHeader;
uint16_t nX;
uint16_t nY;
uint8_t nLevelId;
};

struct D2GSPacketSrv09
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t dwUnitGUID;
uint8_t nClassId;
uint16_t nPosX;
uint16_t nPosY;
};

struct D2GSPacketSrv0A
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t dwUnitGUID;
};

struct D2GSPacketSrv0B
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t dwUnitGUID;
};

struct D2GSPacketSrv0C
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t dwUnitGUID;
uint8_t nAction;
uint8_t nHitClass;
uint8_t nLife;
};

struct D2GSPacketSrv0D
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t dwUnitGUID;
uint8_t nAction;
uint16_t nX;
uint16_t nY;
uint8_t nHitType;
uint8_t nUnitLifePct;
};

struct D2GSPacketSrv0E
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t dwUnitGUID;
uint8_t nAction;
uint8_t bTargetable;
uint32_t nAnimMode;
};

struct D2GSPacketSrv0F
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t dwUnitGUID;
uint8_t nAction;
uint16_t nDestPosX;
uint16_t nDestPosY;
uint8_t nHitClass;
uint16_t nCurrentPosX;
uint16_t nCurrentPosY;
};

struct D2GSPacketSrv10
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t dwUnitGUID;
uint8_t nAction;
uint8_t nTargetType;
uint32_t dwTargetGUID;
uint16_t nCurrentPosX;
uint16_t nCurrentPosY;
};

struct D2GSPacketSrv11
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t dwUnitGUID;
uint16_t nOverlayId;
};

struct D2GSPacketSrv15
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t dwUnitGUID;
uint16_t nPosX;
uint16_t nPosY;
uint8_t bReassign;
};

struct D2GSPacketSrv16
{
uint8_t nHeader;
uint16_t nSize;
uint8_t nNumUpdates;
D2ClientUnitUpdateStrc unitUpdate[55];
};

struct D2GSPacketSrv18
{
uint8_t nHeader;
uint8_t unk0x001[14];
};

struct D2GSPacketSrv19
{
uint8_t nHeader;
uint8_t nGold;
};

struct D2GSPacketSrv1A
{
uint8_t nHeader;
uint8_t nExperience;
};

struct D2GSPacketSrv1B
{
uint8_t nHeader;
uint16_t nExperience;
};

struct D2GSPacketSrv1C
{
uint8_t nHeader;
uint32_t nExperience;
};

struct D2GSPacketSrv1D
{
uint8_t nHeader;
uint8_t nStat;
uint8_t nValue;
};

struct D2GSPacketSrv1E
{
uint8_t nHeader;
uint8_t nStat;
uint16_t nValue;
};

struct D2GSPacketSrv1F
{
uint8_t nHeader;
uint8_t nStat;
int32_t nValue;
};

struct D2GSPacketSrv20
{
uint8_t nHeader;
uint32_t dwUnitGUID;
uint8_t nStat;
int32_t nValue;
};

struct D2GSPacketSrv21
{
uint8_t nHeader;
uint8_t nUnitType;
uint8_t bRemove;
uint32_t nUnitGUID;
uint16_t nSkillId;
uint8_t nSkillLevel;
uint8_t nBonusSkillLevel;
uint8_t unk0x0B;
};

struct D2GSPacketSrv22
{
uint8_t nHeader;
uint8_t nUnitType;
uint8_t unk0x02;
int32_t nUnitGUID;
uint16_t nSkillId;
uint8_t nQuantity;
uint8_t unk0x0A;
uint8_t bIsCorpse;
};

struct D2GSPacketSrv23
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t dwUnitGUID;
uint8_t bLeftSkill;
uint16_t nSkill;
uint32_t nOwnerGUID;
};

struct D2GSPacketSrv26
{
uint8_t nHeader;
uint8_t nMessageType;
uint8_t nLang;
uint8_t nUnitType;
uint32_t dwUnitGUID;
uint8_t nMessageColor;
uint8_t nNameColor;
char szName[16];
char szMessage[256];
};

struct D2MessageStrc
{
uint8_t nMenu;
uint8_t pad0x01;
uint16_t nStringId;
};

struct D2MessageListStrc
{
uint8_t nCount;
uint8_t pad0x01;
D2MessageStrc pMessages[8];
};

struct D2GSPacketSrv27
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t dwUnitGUID;
D2MessageListStrc pMessageList;
};

struct D2GSPacketSrv28
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t nUnitGUID;
uint8_t nAbortUnitType;
uint8_t pData[96];
};

struct D2GSPacketSrv29
{
uint8_t nHeader;
uint8_t pData[96];
};

struct D2GSPacketSrv2A
{
uint8_t nHeader;
uint8_t unk0x01;
uint8_t nResult;
uint32_t unk0x03;
uint32_t dwItemGUID;
uint32_t nGold;
};

struct D2GSPacketSrv2C
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t dwUnitGUID;
uint16_t nSoundEvent;
};

struct D2GSPacketSrv3E
{
uint8_t nHeader;
uint8_t nSize;
uint8_t pBuffer[32];
};

struct D2GSPacketSrv3F
{
uint8_t nHeader;
uint8_t nCursor;
uint32_t dwItemId;
uint16_t nSkill;
};

struct D2GSPacketSrv40
{
uint8_t nHeader;
uint32_t nItemGUID;
D2C_ItemFlags dwFlag;
int32_t bSet;
};

struct D2GSPacketSrv42
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t dwUnitGUID;
};

struct D2GSPacketSrv47
{
uint8_t nHeader;
uint8_t nUnitType;
uint8_t unk0x02;
uint32_t nUnitGUID;
uint32_t unk0x07;
};

struct D2GSPacketSrv48
{
uint8_t nHeader;
uint8_t nUnitType;
uint8_t unk0x02;
uint32_t nUnitGUID;
uint32_t unk0x07;
};

struct D2GSPacketSrv4C
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t dwUnitGUID;
uint16_t nSkillId;
uint8_t nSkillLevel;
uint8_t nTargetType;
uint32_t dwTargetGUID;
uint16_t nItemEffect;
};

struct D2GSPacketSrv4D
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t nUnitGUID;
uint32_t nSkillId;
uint8_t nSkillLevel;
uint16_t nX;
uint16_t nY;
uint16_t nItemEffect;
};

struct D2GSPacketSrv4E
{
uint8_t nHeader;
uint16_t nMercName;
uint32_t nSeed;
};

struct D2GSPacketSrv4F
{
uint8_t nHeader;
};

struct D2Srv50A1Q4CainPayloadStrc
{
uint16_t nStoneOrder[5];
uint16_t padding0x0D;
};

struct D2Srv50A5Q6BaalPayloadStrc
{
uint16_t nZooMonsterId;
uint16_t padding0x05[5];
};

struct D2Srv50HirelingPayloadStrc
{
uint16_t nHireling;
uint16_t padding0x05[5];
};

struct D2Srv50QuestsStatusPayloadStrc
{
uint16_t nMonstersToBeKilled;
uint16_t nStaffTombLevelOffset;
uint16_t nBarbsToBeRescued;
uint8_t field_9;
uint8_t field_A;
uint8_t field_B;
uint16_t field_C;
uint8_t field_E;
};

struct D2Srv50TombOffsetPayloadStrc
{
uint16_t nStaffTombLevelOffset;
uint16_t padding0x05[5];
};

struct D2GSPacketSrv50
{
uint8_t nHeader;
uint16_t nQuestId;
union
{
struct D2Srv50QuestsStatusPayloadStrc QuestsStatusPayload;
struct D2Srv50HirelingPayloadStrc HirelingPayload;
struct D2Srv50TombOffsetPayloadStrc TombOffsetPayload;
struct D2Srv50A1Q4CainPayloadStrc A1Q4_CainPayload;
struct D2Srv50A5Q6BaalPayloadStrc A5Q6_BaalPayload;
};
};

struct D2GSPacketSrv51
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t dwUnitGUID;
uint16_t nObject;
uint16_t nPosX;
uint16_t nPosY;
uint8_t nObjectMode;
uint8_t nInteractType;
};

struct D2GSPacketSrv52
{
uint8_t nHeader;
uint8_t pQuestList[41];
};

struct D2GSPacketSrv53
{
uint8_t nHeader;
int32_t nIndex;
int32_t nTicks;
uint8_t bEclipse;
};

struct D2GSPacketSrv54
{
uint8_t nHeader;
uint8_t unk0x001[2];
};

struct D2GSPacketSrv57
{
uint8_t nHeader;
uint32_t dwUnitGUID;
uint8_t nUnitType;
uint16_t nNameSeed;
uint16_t wUmods;
uint16_t wUmod2;
uint16_t bChampion;
};

struct D2GSPacketSrv58
{
uint8_t nHeader;
int32_t nUnitId;
uint8_t nAction;
uint8_t bSuccess;
};

struct D2GSPacketSrv59
{
uint8_t nHeader;
uint32_t dwGUID;
uint8_t nClass;
char szName[16];
uint16_t nPosX;
uint16_t nPosY;
};

struct D2GSPacketSrv5A
{
uint8_t nHeader;
uint8_t nType;
union
{
struct
{
uint8_t nColor;
uint32_t dwParam;
uint8_t nParam;
union
{
char szText[32];
char szTextData[2][16];
};
};
struct
{
uint32_t dwSkill;
uint32_t dwEndFrame;
uint8_t nExtraData2[30];
};
struct
{
uint8_t nGroup;
uint32_t dwGroupEndFrame;
uint8_t nExtraData3[33];
};
struct
{
int32_t nItemId;
int32_t nPlayerId;
uint8_t nExtraData4[30];
};
struct
{
uint8_t nDamageColor;
uint32_t dwDamage;
int32_t nDefenderGUID;
int32_t nDefenderType;
uint8_t nExtraData5[25];
};
struct
{
uint8_t nUnitType;
int32_t nItemGUID;
uint8_t nAction;
int32_t nPosX;
int32_t nPosY;
uint8_t nExtraData6[24];
};
struct
{
uint8_t nDefenderUnitType;
int32_t nDefenderUnitGUID;
uint8_t nAttackerUnitType;
int32_t nAttackerUnitGUID;
uint8_t nAttackMessageColor;
uint8_t nExtraData7[27];
};
struct
{
int32_t nFrames;
uint8_t nExtraData8[34];
};
struct
{
int32_t nCount;
uint8_t nExtraData9[34];
};
};
};

struct D2GSPacketSrv5B
{
uint8_t nHeader;
uint16_t nPacketLen;
uint32_t nUnitGUID;
uint8_t nClass;
char szName[16];
uint16_t nLevel;
uint16_t nPartyId;
uint16_t unk0x1C;
uint16_t nPartyFlags;
uint16_t nGuildFlags;
char szGuildTag[256];
};

struct D2GSPacketSrv5C
{
uint8_t nHeader;
uint32_t dwPlayerGUID;
};

struct D2GSPacketSrv5D
{
uint8_t nHeader;
uint8_t nQuestId;
uint8_t nFlags;
uint8_t nState;
uint16_t field_4;
};

struct D2GSPacketSrv5E
{
uint8_t nHeader;
uint8_t nQuestIntroFlags[37];
};

struct D2GSPacketSrv5F
{
uint8_t nHeader;
uint32_t dwPortalFlags;
};

struct D2GSPacketSrv60
{
uint8_t nHeader;
uint8_t nFlags;
uint8_t nLevel;
uint32_t dwUnitGUID;
};

struct D2GSPacketSrv61
{
uint8_t nHeader;
uint8_t nCinematic;
};

struct D2GSPacketSrv62
{
uint8_t nHeader;
uint8_t nUnitType;
int32_t nUnitId;
uint8_t unk0x06;
};

struct D2GSPacketSrv63
{
uint8_t nHeader;
uint32_t dwUnitId;
uint16_t pData[8];
};

struct D2GSPacketSrv65
{
uint8_t nHeader;
uint32_t dwUnitId;
uint16_t nKillCount;
};

struct D2GSPacketSrv67
{
uint8_t nHeader;
int32_t nUnitGUID;
uint8_t nAction;
int16_t nX;
int16_t nY;
int8_t nUnitLife;
int8_t nHitClass;
int8_t nPathType;
int16_t nVelocity;
int8_t nMaxDistance;
};

struct D2GSPacketSrv68
{
uint8_t nHeader;
int32_t nUnitGUID;
uint8_t nAction;
int16_t nX;
int16_t nY;
int8_t nTargetUnitType;
int32_t nTargetUnitGUID;
int8_t nUnitLife;
int8_t nHitClass;
int8_t nPathType;
int16_t nVelocity;
int8_t nMaxDistance;
};

struct D2GSPacketSrv69
{
uint8_t nHeader;
uint32_t nUnitGUID;
uint8_t nAction;
uint16_t nX;
uint16_t nY;
uint8_t nUnitLife;
uint8_t nHitClass;
};

struct D2GSPacketSrv6A
{
uint8_t nHeader;
uint32_t nUnitGUID;
uint8_t nAction;
uint8_t nTargetUnitType;
uint32_t nTargetUnitGuid;
uint8_t nDirection;
};

struct D2GSPacketSrv6B
{
uint8_t nHeader;
uint32_t nUnitGUID;
uint8_t nAction;
uint16_t nTargetX;
uint16_t nTargetY;
uint8_t nUnitLife;
uint8_t nHitClass;
uint16_t nCurrentPosX;
uint16_t nCurrentPosY;
};

struct D2GSPacketSrv6C
{
uint8_t nHeader;
uint32_t nUnitGUID;
uint8_t nAction;
uint8_t nTargetUnitType;
uint32_t nTargetUnitGUID;
uint8_t nUnitLife;
uint16_t nCurrentPosX;
uint16_t nCurrentPosY;
};

struct D2GSPacketSrv6D
{
uint8_t nHeader;
uint32_t nUnitGUID;
uint16_t nX;
uint16_t nY;
uint8_t nUnitLife;
};

struct D2GSPacketSrv73
{
uint8_t nHeader;
int32_t unk0x01;
int16_t nMissileId;
int32_t nX;
int32_t nY;
int32_t nFirstX;
int32_t nFirstY;
int16_t nCurrentFrame;
uint8_t nUnitType;
int32_t nUnitGUID;
uint8_t nLevel;
uint8_t nPierce;
};

struct D2GSPacketSrv74
{
uint8_t nHeader;
uint8_t bIsCorpse;
int32_t nOwnerGUID;
int32_t nCorpseGUID;
};

struct D2GSPacketSrv75
{
uint8_t nHeader;
uint32_t nUnitGUID;
uint16_t nPartyId;
uint16_t nLevel;
uint16_t nFlags;
uint16_t nPartyFlags;
};

struct D2GSPacketSrv76
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t dwUnitGUID;
};

struct D2GSPacketSrv77
{
uint8_t nHeader;
uint8_t nAction;
};

struct D2GSPacketSrv78
{
uint8_t nHeader;
char szName[16];
uint32_t dwPlayerGUID;
};

struct D2GSPacketSrv79
{
uint8_t nHeader;
uint8_t bOwnGold;
uint32_t dwGold;
};

struct D2GSPacketSrv7A
{
uint8_t nHeader;
uint8_t bAdd;
uint8_t nPetType;
uint16_t nMonStatsId;
uint32_t dwOwnerGUID;
uint32_t dwPetGUID;
};

struct D2GSPacketSrv7B
{
uint8_t nHeader;
uint8_t nSlot;
struct
{
unsigned __int16 nSkill : 12;
unsigned __int16 nHand : 4;
};
uint32_t nItemGUID;
};

struct D2GSPacketSrv7C
{
uint8_t nHeader;
uint8_t nUnitType;
int32_t nItemGUID;
};

struct D2GSPacketSrv7D
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t nUnitGUID1;
uint32_t nUnitGUID2;
uint32_t nItemFlag;
uint32_t bSet;
};

struct D2GSPacketSrv7E
{
uint8_t nHeader;
uint8_t unk0x001[4];
};

struct D2GSPacketSrv7F
{
uint8_t nHeader;
uint8_t bIsPlayer;
uint16_t nLifePercentage;
int32_t nUnitGUID;
uint16_t nLevelId;
};

struct D2GSPacketSrv81
{
uint8_t nHeader;
uint8_t nPetType;
uint16_t nMonStatsId;
uint32_t nOwnerGUID;
uint32_t nPetGUID;
uint32_t dwNameSeed;
uint32_t dwNameStringId;
};

struct D2GSPacketSrv82
{
uint8_t nHeader;
uint32_t dwOwnerGUID;
char szOwnerName[16];
uint32_t dwPortalGUID[2];
};

struct D2GSPacketSrv89
{
uint8_t nHeader;
uint8_t nEvent;
};

struct D2GSPacketSrv8A
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t dwUnitGUID;
};

struct D2GSPacketSrv8B
{
uint8_t nHeader;
int32_t nUnitGUID;
uint8_t bParty;
};

struct D2GSPacketSrv8C
{
uint8_t nHeader;
uint32_t dwPlayerGUID[2];
uint16_t nFlags;
};

struct D2GSPacketSrv8D
{
uint8_t nHeader;
uint32_t dwPlayerGUID;
uint16_t nPartyGUID;
};

struct D2GSPacketSrv8E
{
uint8_t nHeader;
uint8_t nType;
uint32_t dwPlayerGUID;
uint32_t dwCorpseGUID;
};

struct D2GSPacketSrv8F
{
uint8_t nHeader;
uint8_t pPongData[32];
};

struct D2GSPacketSrv90
{
uint8_t nHeader;
uint32_t dwGUID;
uint32_t dwPosX;
uint32_t dwPosY;
};

struct D2GSPacketSrv91
{
uint8_t nHeader;
uint8_t nAct;
uint16_t pNPCFlags[12];
};

struct D2GSPacketSrv92
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t dwUnitGUID;
};

struct D2GSPacketSrv93
{
uint8_t nHeader;
uint32_t dwUnitGUID;
uint8_t nLevelDelta;
uint8_t nElement;
uint8_t nSkillTab;
};

struct D2GSPacketSrv94__SkillInfo
{
uint16_t nSkill;
uint8_t nSkillLevel;
};

struct D2GSPacketSrv94
{
uint8_t nHeader;
uint8_t nSkills;
uint32_t dwUnitGUID;
struct D2GSPacketSrv94__SkillInfo Skills[255];
};

struct D2GSPacketSrv95
{
uint8_t nHeader;
uint8_t unk0x01[12];
};

struct D2GSPacketSrv96
{
uint8_t nHeader;
uint8_t unk0x01[8];
};

struct D2GSPacketSrv97
{
uint8_t nHeader;
};

struct D2GSPacketSrv98
{
uint8_t nHeader;
uint32_t nUnitGUID;
uint16_t nMinionSpawnClassId;
};

struct D2GSPacketSrv99
{
uint8_t nHeader;
uint8_t nSourceUnitType;
uint32_t nSourceUnitGUID;
uint16_t nSkillId;
uint8_t nSkillLevel;
uint8_t nTargetType;
int32_t dwTargetGUID;
uint16_t nItemEffect;
};

struct D2GSPacketSrv9A
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t nUnitGUID;
uint32_t nSkillId;
uint8_t nSkillLevel;
uint16_t nX;
uint16_t nY;
uint16_t nItemEffect;
};

struct D2GSPacketSrv9B
{
uint8_t nHeader;
uint16_t nMercNameStringId;
uint32_t nReviveCost;
};

struct D2GSPacketSrv9C
{
uint8_t nHeader;
uint8_t nAction;
uint8_t nPacketSize;
uint8_t nComponent;
int32_t nItemId;
uint8_t pBitstream[244];
};

struct D2GSPacketSrv9D
{
uint8_t nHeader;
uint8_t nAction;
uint8_t nPacketSize;
uint8_t nComponent;
int32_t nItemId;
uint8_t nUnitType;
int32_t nUnitId;
uint8_t pBitstream[244];
};

struct D2GSPacketSrv9E
{
uint8_t nHeader;
uint8_t nStat;
uint32_t dwGUID;
uint8_t nValue;
};

struct D2GSPacketSrv9F
{
uint8_t nHeader;
uint8_t nStat;
uint32_t dwGUID;
uint16_t nValue;
};

struct D2GSPacketSrvA0
{
uint8_t nHeader;
uint8_t nStat;
uint32_t dwGUID;
uint32_t nValue;
};

struct D2GSPacketSrvA1
{
uint8_t nHeader;
uint8_t nStat;
uint32_t dwGUID;
uint8_t nValue;
};

struct D2GSPacketSrvA2
{
uint8_t nHeader;
uint8_t nStat;
uint32_t dwGUID;
uint16_t nValue;
};

struct D2GSPacketSrvA3
{
uint8_t nHeader;
uint8_t nSeed;
uint16_t nSkillId;
uint16_t nSkillLevel;
uint8_t nUnitType;
uint32_t nUnitGUID;
uint8_t nTargetType;
uint32_t nTargetGUID;
uint32_t nX;
uint32_t nY;
};

struct D2GSPacketSrvA4
{
uint8_t nHeader;
uint16_t nMonStatsId;
};

struct D2GSPacketSrvA5
{
uint8_t nHeader;
uint8_t nUnitType;
int32_t nUnitGUID;
uint16_t nSkillId;
};

struct D2PropertyStrc
{
int32_t nProperty;
int32_t nLayer;
int32_t nMin;
int32_t nMax;
};

struct D2RunesTxt
{
char szName[64];
char szRuneName[64];
uint8_t nComplete;
uint8_t nServer;
uint16_t wStringId;
uint16_t pad0x84;
uint16_t wIType[6];
uint16_t wEType[3];
int32_t nRune[6];
D2PropertyStrc pProperties[7];
};

struct D2GSPacketSrvA6
{
uint8_t nHeader;
uint8_t unk0x01;
uint16_t nSize;
uint16_t nRecordId;
D2RunesTxt tRecord;
};

struct D2GSPacketSrvA7
{
uint8_t nHeader;
uint8_t nUnitType;
int32_t nUnitId;
uint8_t nState;
};

struct D2GSPacketSrvA8
{
uint8_t nHeader;
uint8_t nUnitType;
int32_t nUnitId;
uint8_t nSize;
uint8_t nState;
uint8_t pStream[512];
};

struct D2GSPacketSrvA9
{
uint8_t nHeader;
uint8_t nUnitType;
int32_t nUnitId;
uint8_t nState;
};

struct D2GSPacketSrvAA
{
uint8_t nHeader;
uint8_t nUnitType;
int32_t nUnitId;
uint8_t nSize;
uint8_t pStream[512];
};

struct D2GSPacketSrvAB
{
uint8_t nHeader;
uint8_t nUnitType;
uint32_t dwUnitGUID;
uint8_t nLife;
};

struct D2GSPacketSrvAC
{
uint8_t nHeader;
uint32_t dwUnitGUID;
uint16_t nUnitIndex;
uint16_t nPosX;
uint16_t nPosY;
uint8_t nLifePct;
uint8_t nPacketLength;
uint8_t bitstream[244];
};

struct D2GSPacketSrvAD
{
uint8_t nHeader;
int32_t dwMemCheckAddress1;
int32_t dwMemCheckAddress2;
};

struct D2GemsTxt
{
char szName[32];
char szLetter[8];
union
{
uint32_t dwItemCode;
char szItemCode[4];
};
uint16_t wStringId;
uint8_t nNumMods;
uint8_t nTransForm;
D2PropertyStrc pProperties[3][3];
};

union D2GemsTxtCodeUnion
{
uint32_t dwItemCode;
char szItemCode[4];
};

struct D2GfxDataStrc
{
D2GfxCellStrc *pCurrentCell;
D2CellFileStrc *pCellFile;
uint32_t nFrame;
uint32_t nDirection;
int32_t nMaxDirections;
int32_t nMaxFrames;
uint32_t fFlags;
uint8_t fState;
union
{
uint8_t nComponent;
uint8_t fItemFlags;
};
uint8_t unk0x1E;
uint8_t unk0x1F;
int32_t nUnitType;
int32_t nUnitIndex;
int32_t nMode;
int32_t nOverlay;
union
{
struct
{
char szToken[4];
char szComponent[4];
char szArmorType[4];
char szMode[4];
char szHitClass[4];
};
uint32_t dwName[5];
};
char *pName;
};

struct D2HireDescTxt
{
uint32_t dwCode;
};

struct __declspec(align(2)) D2HirelingInitStrc
{
int32_t nId;
int32_t nLevel;
int32_t nHitpoints;
int32_t nStrength;
int32_t nDexterity;
int32_t nGold;
int32_t nExperience;
int32_t nDefense;
int32_t nMinDamage;
int32_t nMaxDamage;
int32_t nShare;
int32_t nResist;
int32_t nHireDesc;
int32_t field_34;
int16_t nFlags;
};

struct D2HirelingTxt
{
uint16_t wVersion;
uint16_t pad0x02;
int32_t nId;
uint32_t dwClass;
uint32_t dwAct;
uint32_t dwDifficulty;
uint32_t dwSeller;
int32_t nGold;
int32_t nHirelingLevel;
uint32_t dwExpPerLvl;
uint32_t dwHitpoints;
uint32_t dwHitpointsPerLvl;
int32_t dwDefense;
int32_t dwDefensePerLvl;
int32_t nStr;
int32_t nStrPerLvl;
int32_t nDex;
int32_t nDexPerLvl;
int32_t dwAttackRate;
int32_t dwAttackRatePerLvl;
uint32_t dwShare;
int32_t nDmgMin;
int32_t nDmgMax;
int32_t nDmgPerLvl;
int32_t dwResist;
int32_t dwResistPerLvl;
uint32_t dwDefaultChance;
uint32_t dwHead;
uint32_t dwTorso;
uint32_t dwWeapon;
uint32_t dwShield;
uint32_t dwSkill[6];
uint32_t dwChance[6];
uint32_t dwChancePerLvl[6];
int8_t nMode[6];
int8_t nLevel[6];
int8_t nSLvlPerLvl[6];
uint8_t nHireDesc;
char szNameFirst[32];
char szNameLast[32];
uint8_t pad0x113;
uint16_t wNameFirst;
uint16_t wNameLast;
};

struct D2HitClassSoundStrc
{
D2C_Sounds nSoundId;
int32_t nHitFrame;
};

struct D2HitClassTxt
{
uint32_t dwCode;
};

struct D2HoverTextStrc
{
uint32_t dwDisplayTime;
uint32_t dwExpireTime;
uint8_t nLangId;
uint8_t pad0x09[3];
int32_t bUsed;
char szMsg[256];
};

struct D2InteractInfoStrc
{
D2UnitStrc *pUnit;
int32_t nInteract;
D2InteractInfoStrc *pNext;
};

struct D2InvButtonInfoStrc
{
BOOL bEnabled;
BOOL bPressed;
D2C_StringIndices nHoverTextId;
D2C_VendorInventoryModes nInventoryMode;
uint16_t nBaseFrame;
D2C_CursorBuySellFrame nCursorFrame;
};

struct D2InvCompGridStrc
{
D2InvRectStrc rect;
uint8_t nWidth;
uint8_t nHeight;
uint16_t pad0x12;
};

struct D2InventoryGridInfoStrc
{
uint8_t nGridX;
uint8_t nGridY;
uint16_t pad0x02;
int32_t nGridLeft;
int32_t nGridRight;
int32_t nGridTop;
int32_t nGridBottom;
uint8_t nGridBoxWidth;
uint8_t nGridBoxHeight;
uint16_t pad0x16;
};

struct D2InventoryGridStrc
{
D2UnitStrc *pItem;
D2UnitStrc *pLastItem;
uint8_t nGridWidth;
uint8_t nGridHeight;
uint16_t pad0x0A;
D2UnitStrc **ppItems;
};

struct D2InventoryNodeStrc
{
int32_t nItemId;
D2InventoryNodeStrc *pNext;
};

struct D2InventoryStrc
{
uint32_t dwSignature;
void *pMemPool;
D2UnitStrc *pOwner;
D2UnitStrc *pFirstItem;
D2UnitStrc *pLastItem;
D2InventoryGridStrc *pGrids;
int32_t nGridCount;
uint32_t dwLeftItemGUID;
D2UnitStrc *pCursorItem;
uint32_t dwOwnerGuid;
uint32_t dwItemCount;
D2InventoryNodeStrc *pFirstNode;
D2InventoryNodeStrc *pLastNode;
D2CorpseStrc *pFirstCorpse;
D2CorpseStrc *pLastCorpse;
int32_t nCorpseCount;
};

struct D2InventoryTxt
{
D2InvRectStrc pRect;
D2InventoryGridInfoStrc pGridInfo;
D2InvCompGridStrc pComponents[10];
};

union D2ItemDataPageUnion
{
uint8_t nPage;
uint8_t nInvPage;
D2C_ItemStorePage nStorePage;
};

struct D2ItemExtraDataStrc
{
D2InventoryStrc *pParentInv;
D2UnitStrc *pPreviousItem;
D2UnitStrc *pNextItem;
int8_t nNodePos;
int8_t nNodePosOther;
uint16_t unk0x0E;
D2UnitStrc *pPreviousGridItem;
D2UnitStrc *pNextGridItem;
};

struct __declspec(align(4)) D2ItemDataStrc
{
uint32_t dwQualityNo;
D2SeedStrc pSeed;
uint32_t dwOwnerGUID;
uint32_t dwInitSeed;
uint32_t dwCommandFlags;
uint32_t dwItemFlags;
uint32_t dwRealmData[2];
uint32_t dwActionStamp;
int32_t dwFileIndex;
uint32_t dwItemLevel;
uint16_t wItemFormat;
uint16_t wRarePrefix;
uint16_t wRareSuffix;
uint16_t wAutoAffix;
uint16_t wMagicPrefix[3];
uint16_t wMagicSuffix[3];
uint8_t nBodyLoc;
union
{
uint8_t nPage;
uint8_t nInvPage;
D2C_ItemStorePage nStorePage;
};
uint8_t nCellOverlap;
uint8_t nItemCell;
uint8_t nEarLvl;
uint8_t nInvGfxIdx;
char szPlayerName[16];
uint8_t unk0x5A[2];
D2ItemExtraDataStrc pExtraData;
};

struct D2ItemRatioTxt
{
int32_t dwUnique;
int32_t dwUniqueDivisor;
int32_t dwUniqueMin;
int32_t dwRare;
int32_t dwRareDivisor;
int32_t dwRareMin;
int32_t dwSet;
int32_t dwSetDivisor;
int32_t dwSetMin;
int32_t dwMagic;
int32_t dwMagicDivisor;
int32_t dwMagicMin;
int32_t dwHiQuality;
int32_t dwHiQualityDivisor;
int32_t dwNormal;
int32_t dwNormalDivisor;
uint16_t wVersion;
uint8_t nUber;
uint8_t nClassSpecific;
};

struct D2ItemSaveStrc
{
int32_t nClassId;
uint16_t nX;
uint16_t nY;
uint8_t nAnimMode;
uint8_t pad0x09[3];
uint32_t dwFlags;
D2C_ItemStorePage nStorePage;
uint8_t nBodyLoc;
uint8_t pad0x12[2];
int32_t nItemFileIndex;
};

struct D2OpStatDataStrc
{
uint16_t nOpBase;
uint16_t nStat;
uint8_t nOp;
uint8_t nOpParam;
};

struct D2ItemStatCostTxt
{
uint16_t wStatId;
uint16_t pad0x02;
uint32_t dwItemStatFlags;
uint8_t nSendBits;
uint8_t nSendParamBits;
uint8_t nCsvBits;
uint8_t nCsvParam;
uint32_t dwDivide;
uint32_t dwMultiply;
uint32_t dwAdd;
uint8_t nValShift;
uint8_t nSaveBits;
uint8_t n09SaveBits;
uint8_t pad0x1B;
uint32_t dwSaveAdd;
uint32_t dw09SaveAdd;
uint32_t dwSaveParamBits;
uint32_t dw09SaveParamBits;
int32_t dwMinAccr;
uint8_t nEncode;
uint8_t pad0x31;
uint16_t wMaxStat;
int16_t nDescPriority;
uint8_t nDescFunc;
uint8_t nDescVal;
uint16_t wDescStrPos;
uint16_t wDescStrNeg;
uint16_t wDescStr2;
uint16_t wDescGrp;
uint8_t nDescGrpFunc;
uint8_t nDescGrpVal;
uint16_t wDescGrpStrPos;
uint16_t wDescGrpStrNeg;
uint16_t wDescGrpStr2;
int16_t wItemEvent[2];
uint16_t wItemEventFunc[2];
uint8_t nKeepZero;
uint8_t bIsBaseOfOtherStatOp;
uint8_t bHasOpStatData;
uint8_t bHasOpApplyingToItem;
uint8_t nOp;
uint8_t nOpParam;
uint16_t wOpBase;
uint16_t wOpStat[3];
uint16_t unk0x5E[64];
D2OpStatDataStrc pOpStatData[16];
uint16_t pad0x13E;
uint32_t dwStuff;
};

struct D2ItemTradeStrc
{
int32_t nItemGUID1;
int32_t nItemGUID2;
D2ItemTradeStrc *pNext;
};

struct D2ItemTypesTxt
{
char szCode[4];
int16_t nEquiv1;
int16_t nEquiv2;
uint8_t nRepair;
uint8_t nBody;
uint8_t nBodyLoc1;
uint8_t nBodyLoc2;
uint16_t wShoots;
uint16_t wQuiver;
uint8_t nThrowable;
uint8_t nReload;
uint8_t nReEquip;
uint8_t nAutoStack;
uint8_t nMagic;
uint8_t nRare;
uint8_t nNormal;
uint8_t nCharm;
uint8_t nGem;
uint8_t nBeltable;
uint8_t nMaxSock1;
uint8_t nMaxSock25;
uint8_t nMaxSock40;
uint8_t nTreasureClass;
uint8_t nRarity;
uint8_t nStaffMods;
uint8_t nCostFormula;
uint8_t nClass;
uint8_t nStorePage;
uint8_t nVarInvGfx;
char szInvGfx[6][32];
};

struct D2ItemsTxt
{
char szFlippyFile[32];
char szInvFile[32];
char szUniqueInvFile[32];
char szSetInvFile[32];
union
{
uint32_t dwCode;
char szCode[4];
};
uint32_t dwNormCode;
uint32_t dwUberCode;
uint32_t dwUltraCode;
uint32_t dwAlternateGfx;
uint32_t dwPspell;
uint16_t wState;
uint16_t wCurseState[2];
uint16_t wStat[3];
uint32_t dwCalc[3];
uint32_t dwLen;
uint8_t nSpellDesc;
uint8_t pad0xB5;
uint16_t wSpellDescStr;
uint32_t dwSpellDescCalc;
uint32_t dwBetterGem;
uint32_t dwWeapClass;
uint32_t dwWeapClass2Hand;
uint32_t dwTransmogrifyType;
int32_t dwMinAc;
int32_t dwMaxAc;
uint32_t dwGambleCost;
int32_t dwSpeed;
uint32_t dwBitField1;
uint32_t dwCost;
uint32_t dwMinStack;
uint32_t dwMaxStack;
uint32_t dwSpawnStack;
uint32_t dwGemOffset;
uint16_t wNameStr;
uint16_t wVersion;
uint16_t wAutoPrefix;
uint16_t wMissileType;
uint8_t nRarity;
uint8_t nLevel;
uint8_t nMinDam;
uint8_t nMaxDam;
uint8_t nMinMisDam;
uint8_t nMaxMisDam;
uint8_t n2HandMinDam;
uint8_t n2HandMaxDam;
uint8_t nRangeAdder;
uint8_t unk0x105;
int16_t nStrBonus;
int16_t nDexBonus;
uint16_t wReqStr;
uint16_t wReqDex;
uint8_t nAbsorb;
uint8_t nInvWidth;
uint8_t nInvHeight;
uint8_t nBlock;
uint8_t nDurability;
uint8_t nNoDurability;
uint8_t nMissile;
uint8_t nComponent;
uint8_t nArmorComp[6];
uint8_t n2Handed;
uint8_t nUseable;
int16_t wType[2];
uint8_t nSubType;
uint8_t unk0x123;
uint16_t wDropSound;
uint16_t wUseSound;
uint8_t nDropSfxFrame;
uint8_t nUnique;
uint8_t nQuest;
uint8_t nQuestDiffCheck;
uint8_t nTransparent;
uint8_t nTransTbl;
uint8_t pad0x12E;
uint8_t nLightRadius;
uint8_t nBelt;
uint8_t nAutoBelt;
uint8_t nStackable;
uint8_t nSpawnable;
uint8_t nSpellIcon;
uint8_t nDurWarning;
uint8_t nQuantityWarning;
uint8_t nHasInv;
uint8_t nGemSockets;
uint8_t nTransmogrify;
uint8_t nTmogMin;
uint8_t nTmogMax;
uint8_t nHitClass;
uint8_t n1or2Handed;
uint8_t nGemApplyType;
uint8_t nLevelReq;
uint8_t nMagicLevel;
int8_t nTransform;
int8_t nInvTrans;
uint8_t nCompactSave;
uint8_t nSkipName;
uint8_t nNameable;
uint8_t nVendorMin[17];
uint8_t nVendorMax[17];
uint8_t nVendorMagicMin[17];
uint8_t nVendorMagicMax[17];
uint8_t nVendorMagicLvl[17];
uint8_t pad0x19B;
uint32_t dwNightmareUpgrade;
uint32_t dwHellUpgrade;
uint8_t nPermStoreItem;
uint8_t nMultibuy;
uint16_t pad0x1A6;
};

union D2ItemsTxtCodeUnion
{
uint32_t dwCode;
char szCode[4];
};

struct D2LeapArcBandStrc
{
int32_t nMaxDistance;
int32_t nSlope;
int32_t nIntercept;
};

struct D2LevelDefBin
{
uint32_t dwQuestFlag;
uint32_t dwQuestFlagEx;
uint32_t dwLayer;
uint32_t dwSizeX[3];
uint32_t dwSizeY[3];
uint32_t dwOffsetX;
uint32_t dwOffsetY;
uint32_t dwDepend;
uint32_t dwDrlgType;
uint32_t dwLevelType;
uint32_t dwSubType;
uint32_t dwSubTheme;
uint32_t dwSubWaypoint;
uint32_t dwSubShrine;
int32_t dwVis[8];
uint32_t dwWarp[8];
uint8_t nIntensity;
uint8_t nRed;
uint8_t nGreen;
uint8_t nBlue;
uint32_t dwPortal;
uint32_t dwPosition;
uint32_t dwSaveMonsters;
uint32_t dwLOSDraw;
};

struct D2LevelsTxt
{
uint16_t wLevelNo;
uint8_t nPal;
uint8_t nAct;
uint8_t nTeleport;
uint8_t nRain;
uint8_t nMud;
uint8_t nNoPer;
uint8_t nIsInside;
uint8_t nDrawEdges;
uint16_t unk0x0A;
uint32_t dwWarpDist;
uint16_t wMonLvl[3];
uint16_t wMonLvlEx[3];
uint32_t dwMonDen[3];
uint8_t nMonUMin[3];
uint8_t nMonUMax[3];
uint8_t nMonWndr;
uint8_t nMonSpcWalk;
uint8_t nQuest;
uint8_t nRangedSpawn;
uint8_t nNumMon;
uint8_t nNumNormalMonsters;
uint8_t nNumNightmareHellMonsters;
uint8_t nNumUniqueMonsters;
int16_t wNormalMonsters[25];
int16_t wNightmareHellMonsters[25];
int16_t wUniqueMonsters[25];
uint16_t wCMon[4];
uint16_t wCPct[4];
uint16_t wCAmt[4];
uint8_t nWaypoint;
uint8_t nObjGroup[8];
uint8_t nObjPrb[8];
char szLevelName[40];
char szLevelWarp[40];
char szEntryFile[40];
uint8_t pad0x16D;
Unicode wszLevelName[40];
Unicode wszLevelWarp[40];
uint16_t pad0x20E;
uint32_t dwThemes;
uint32_t dwFloorFilter;
uint32_t dwBlankScreen;
uint32_t dwSoundEnv;
};

struct D2LightGridCellStrc
{
int32_t nLit;
uint8_t nIntensity;
uint8_t nRed;
uint8_t nGreen;
uint8_t nBlue;
};

struct D2ListBoxImplVtbl
{
D2ClientListBoxStrc *(__fastcall *pfnDeletingDtor)(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, uint8_t bDelete);
int32_t (__fastcall *pfnDraw)(D2ClientListBoxStrc *pListBox);
int32_t (__fastcall *pfnOnMouseWheel)(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnMouseDown)(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnMouseUp)(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnMouseMove)(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnChar)(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnArrowUp)(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnArrowDown)(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__stdcall *pfnOnArrowRight)(int32_t a1);
int32_t (__stdcall *pfnOnArrowLeft)(int32_t a1);
int32_t (__fastcall *pfnGetValue)(D2ClientListBoxStrc *pListBox);
void (__fastcall *pfnUpdateScrollThumb)(D2ClientListBoxStrc *pListBox);
int32_t (__fastcall *pfnGetNumEntries)(D2ClientListBoxStrc *pListBox);
void (__fastcall *pfnRemoveAll)(D2ClientListBoxStrc *pListBox);
void (__fastcall *pfnAddEntry)(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, const struct Unicode *pwsCol0, const struct Unicode *pwsCol1, const struct Unicode *pwsCol2, const struct Unicode *pwsCol3, const struct Unicode *pwsCol4, const struct Unicode *pwsCol5, int32_t nUserData, int32_t nColor, int32_t bUseDefaultColor, int32_t nFont);
};

struct D2ListBoxProxyVtbl
{
uint32_t **(__fastcall *pfnDeletingDtor)(uint32_t **pProxy, int32_t nEdxUnused, uint8_t bFreeObject);
int32_t (__fastcall *pfnDraw)(uint32_t **pProxy);
int32_t (__fastcall *pfnOnMouseWheel)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnMouseDown)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnMouseUp)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnMouseMove)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnChar)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnArrowUp)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnArrowDown)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnArrowRight)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnArrowLeft)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnGetValue)(uint32_t **pProxy);
int32_t (__fastcall *pfnAddEntry)(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg);
int32_t (__fastcall *pfnGetEntryUserData)(uint32_t **pProxy, int32_t nEdxUnused, int32_t nEntryIndex);
int32_t (__fastcall *pfnGetNumEntries)(uint32_t **pProxy);
int32_t (__fastcall *pfnRemoveAll)(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg);
};

struct D2LowQualityItemsTxt
{
char szName[32];
uint16_t wTblId;
};

struct D2LvlMazeTxt
{
uint32_t dwLevelId;
uint32_t dwRooms[3];
uint32_t dwSizeX;
uint32_t dwSizeY;
uint32_t dwMerge;
};

struct D2LvlPrestTxt
{
uint32_t dwDef;
uint32_t dwLevelId;
uint32_t dwPopulate;
uint32_t dwLogicals;
uint32_t dwOutdoors;
uint32_t dwAnimate;
uint32_t dwKillEdge;
uint32_t dwFillBlanks;
uint32_t dwExpansion;
uint32_t nAnimSpeed;
int32_t nSizeX;
int32_t nSizeY;
uint32_t dwAutoMap;
uint32_t dwScan;
uint32_t dwPops;
uint32_t dwPopPad;
uint32_t dwFiles;
char szFile[6][60];
uint32_t dwDt1Mask;
};

struct D2LvlSubTxt
{
uint32_t dwType;
char szFile[60];
uint32_t dwCheckAll;
uint32_t dwBordType;
uint32_t dwDt1Mask;
uint32_t dwGridSize;
D2DrlgFileStrc *pDrlgFile;
D2DrlgGridStrc pTileTypeGrid[4];
D2DrlgGridStrc pWallGrid[4];
D2DrlgGridStrc pFloorGrid;
D2DrlgGridStrc pShadowGrid;
uint32_t nProb[5];
int32_t nTrials[5];
int32_t nMax[5];
uint32_t dwExpansion;
};

struct D2LvlTypesTxt
{
char szFile[32][60];
uint8_t nAct;
uint8_t pad0x781[3];
uint32_t dwExpansion;
};

struct D2LvlWarpTxt
{
uint32_t dwLevelId;
uint32_t dwSelectX;
uint32_t dwSelectY;
uint32_t dwSelectDX;
uint32_t dwSelectDY;
uint32_t dwExitWalkX;
uint32_t dwExitWalkY;
uint32_t dwOffsetX;
uint32_t dwOffsetY;
uint32_t dwLitVersion;
uint32_t dwTiles;
char szDirection[4];
};

struct D2MagicAffixTxt
{
char szName[32];
uint16_t wTblIndex;
uint16_t wVersion;
D2PropertyStrc pProperties[3];
uint8_t wSpawnable;
uint8_t padding0x54;
int8_t nTransformColor;
uint8_t unk0x57;
uint32_t dwLevel;
uint32_t dwGroup;
uint32_t dwMaxLevel;
uint8_t nRare;
uint8_t nLevelReq;
uint8_t nClassSpecific;
uint8_t nClass;
uint8_t nClassLevelReq;
uint8_t pad0x69;
uint16_t wIType[7];
uint16_t wEType[5];
uint8_t wFrequency;
uint8_t padding0x83;
uint32_t dwDivide;
uint32_t dwMultiply;
uint32_t dwAdd;
};

struct D2MapAIPathPositionStrc
{
int32_t nMapAIAction;
int32_t nX;
int32_t nY;
};

struct D2MapAIStrc
{
int32_t nPathNodes;
D2MapAIPathPositionStrc *pPosition;
};

struct D2MercPanelLabelStrc
{
int32_t nLeft;
int32_t nRight;
int32_t nY;
D2C_StringIndices nStringId;
};

struct D2MercStatRowStrc
{
int32_t nLeft;
int32_t nX;
int32_t nY;
int32_t nMaxWidth;
D2C_ItemStats nStatId;
};

struct TSLink
{
struct TSLink *pPrevLink;
int32_t nNext;
};

struct D2MessageSourceStrc
{
struct TSLink tLink;
int32_t (__stdcall *pfnMessageSource)(DWORD dwMessageId, LPSTR pszBuffer, int32_t nMaxLength);
};

struct D2MissCalcTxt
{
uint32_t dwCode;
};

struct D2MissileDataStrc
{
uint32_t unk0x00;
int16_t nStreamMissile;
int16_t nStreamRange;
int16_t nActivateFrame;
int16_t nSkill;
int16_t nLevel;
int16_t nTotalFrames;
int16_t nCurrentFrame;
uint16_t unk0x12;
uint32_t fFlags;
int32_t nOwnerType;
uint32_t dwOwnerGUID;
int32_t nHomeType;
uint32_t dwHomeGUID;
union
{
struct
{
int32_t nStatus;
D2PathPointStrc pCoords;
};
D2CoordStrc pTargetCoords;
};
D2MissileStreamStrc *pStream;
};

struct D2MissileFxStageStrc
{
int32_t nFrameBound;
int32_t nPeriod;
int32_t nParam;
};

struct D2MissileStrc
{
D2C_MissileSpawnFlags dwFlags;
struct D2UnitStrc *pOwner;
struct D2UnitStrc *pOrigin;
struct D2UnitStrc *pTarget;
int32_t nMissile;
int32_t nX;
int32_t nY;
int32_t nTargetX;
int32_t nTargetY;
int32_t nGfxArg;
int32_t nVelocity;
int32_t nSkill;
int32_t nSkillLevel;
int32_t nLoops;
uint32_t unk0x38;
uint32_t unk0x3C;
int32_t nFrame;
int32_t nActivateFrame;
int32_t nAttBonus;
int32_t nRange;
int32_t nLightRadius;
void (__fastcall *pInitFunc)(struct D2UnitStrc *, int32_t);
int32_t pInitArgs;
};

struct D2MissileStreamStrc
{
int32_t *unk0x00;
int32_t unk0x04;
};

struct D2MissileTargetPosStrc
{
int32_t nStatus;
D2PathPointStrc pCoords;
};

union D2MissileTargetUnion
{
struct
{
int32_t nStatus;
D2PathPointStrc pCoords;
};
D2CoordStrc pTargetCoords;
};

struct D2MissileUnitFindTableStrc
{
int32_t (__fastcall *pfUnitFindCallback)(D2UnitStrc *, void *);
int32_t nCollisionMask;
};

struct D2MissilesTxt
{
uint16_t wId;
uint16_t pad0x02;
uint32_t dwMissileFlags;
uint16_t wCltDoFunc;
uint16_t wCltHitFunc;
uint16_t wSrvDoFunc;
uint16_t wSrvHitFunc;
uint16_t wSrvDmgFunc;
uint16_t wTravelSound;
uint16_t wHitSound;
uint16_t wExplosionMissile;
uint16_t wSubMissile[3];
uint16_t wCltSubMissile[3];
uint16_t wHitSubMissile[4];
uint16_t wCltHitSubMissile[4];
uint16_t wProgSound;
uint16_t wProgOverlay;
int32_t dwParam[5];
uint32_t dwHitPar[3];
uint32_t dwCltParam[5];
uint32_t dwCltHitPar[3];
int32_t dwDmgParam[2];
uint32_t dwSrvCalc;
uint32_t dwCltCalc;
uint32_t dwHitCalc;
uint32_t dwCltHitCalc;
uint32_t dwDmgCalc;
uint8_t nHitClass;
uint8_t pad0x95;
uint16_t wRange;
uint16_t wLevRange;
uint8_t nVel;
uint8_t nVelLev;
uint8_t nMaxVel;
uint8_t pad0x9D;
uint16_t wAccel;
uint16_t wAnimRate;
uint16_t wXoffset;
uint16_t wYoffset;
uint16_t wZoffset;
uint32_t dwHitFlags;
uint16_t wResultFlags;
uint8_t nKnockBack;
uint8_t pad0xAF;
uint32_t dwMinDamage;
uint32_t dwMaxDamage;
uint32_t dwMinLevDam[5];
uint32_t dwMaxLevDam[5];
uint32_t dwDmgSymPerCalc;
uint8_t nElemType;
uint8_t pad0xE5[3];
uint32_t dwElemMin;
uint32_t dwElemMax;
uint32_t dwMinElemLev[5];
uint32_t dwMaxElemLev[5];
uint32_t dwElemDmgSymPerCalc;
uint32_t dwElemLen;
uint32_t dwElemLevLen[3];
uint8_t nCltSrcTown;
uint8_t nSrcDamage;
uint8_t nSrcMissDmg;
uint8_t nHoly;
uint8_t nLight;
uint8_t nFlicker;
uint8_t nRGB[3];
uint8_t nInitSteps;
uint8_t nActivate;
uint8_t nLoopAnim;
char szCelFile[64];
uint8_t nAnimLen;
uint8_t pad0x179[3];
uint32_t dwRandStart;
uint8_t nSubLoop;
uint8_t nSubStart;
uint8_t nSubStop;
D2C_MissilesTxtCollideTypes nCollideType;
uint8_t nCollision;
uint8_t nClientCol;
uint8_t nCollideKill;
uint8_t nCollideFriend;
uint8_t nNextHit;
uint8_t nNextDelay;
uint8_t nSize;
uint8_t nToHit;
uint8_t nAlwaysExplode;
uint8_t nTrans;
uint8_t nQty;
uint8_t pad0x18F;
uint32_t dwSpecialSetup;
int16_t nSkill;
uint8_t nHitShift;
uint8_t unk0x197[5];
uint32_t dwDamageRate;
uint8_t nNumDirections;
uint8_t nAnimSpeed;
uint8_t nLocalBlood;
uint8_t pad0x1A3;
};

struct D2ModStatsArrayStrc
{
D2SLayerStatIdStrc *pStat;
uint16_t nStatCount;
uint16_t nCapacity;
};

struct D2MonAiTxt
{
uint16_t wAi;
};

struct D2MonEquipTxt
{
int16_t nMonster;
uint16_t wLevel;
uint8_t nOnInit;
uint8_t pad0x05[3];
uint32_t dwItem[3];
uint8_t nLoc[3];
uint8_t nMod[3];
uint16_t pad0x1A;
};

struct D2MonItemPercentTxt
{
uint8_t nHeartPercent;
uint8_t nBodyPartPercent;
uint8_t nTreasureClassPercent;
uint8_t nComponentPercent;
};

struct D2MonLvlTxt
{
uint32_t dwAC[3];
uint32_t dwLAC[3];
uint32_t dwTH[3];
uint32_t dwLTH[3];
uint32_t dwHP[3];
uint32_t dwLHP[3];
uint32_t dwDM[3];
uint32_t dwLDM[3];
uint32_t dwXP[3];
uint32_t dwLXP[3];
};

struct D2MonModeTxt
{
char szName[32];
uint32_t dwToken;
uint8_t nDTDir;
uint8_t nNUDir;
uint8_t nWLDir;
uint8_t nGHDir;
uint8_t nA1Dir;
uint8_t nA2Dir;
uint8_t nBLDir;
uint8_t nSCDir;
uint8_t nS1Dir;
uint8_t nS2Dir;
uint8_t nS3Dir;
uint8_t nS4Dir;
uint8_t nDDDir;
uint8_t nKBDir;
uint8_t nSQDir;
uint8_t nRNDir;
};

struct D2MonModeTxtStub
{
uint32_t dwCode;
};

struct D2MonPlaceTxt
{
uint16_t nCode;
};

struct D2MonPresetTxt
{
uint8_t nAct;
uint8_t nType;
uint16_t wPlace;
};

struct D2MonPropTxt
{
uint32_t dwId;
D2PropertyStrc props[3][6];
uint8_t nChance[3][6];
uint16_t pad0x136;
};

struct D2MonSoundsTxt
{
uint32_t dwId;
D2C_Sounds dwAttack1;
uint32_t dwAtt1Del;
uint32_t dwAtt1Prb;
D2C_Sounds dwWeapon1;
uint32_t dwWea1Del;
uint32_t dwWea1Vol;
D2C_Sounds dwAttack2;
uint32_t dwAtt2Del;
uint32_t dwAtt2Prb;
D2C_Sounds dwWeapon2;
uint32_t dwWea2Del;
uint32_t dwWea2Vol;
D2C_Sounds dwHitSound;
uint32_t dwHitDelay;
D2C_Sounds dwDeathSound;
uint32_t dwDeaDelay;
D2C_Sounds dwSkill[4];
D2C_Sounds dwFootstep;
D2C_Sounds dwFootstepLayer;
uint32_t dwFsCnt;
uint32_t dwFsOff;
uint32_t dwFsPrb;
D2C_Sounds dwNeutral;
uint32_t dwNeuTime;
D2C_Sounds dwInit;
D2C_Sounds dwTaunt;
D2C_Sounds dwFlee;
uint8_t nCvtMo1;
uint8_t nCvtTgt1;
uint16_t pad0x7E;
int32_t dwCvtSk1;
uint8_t nCvtMo2;
uint8_t nCvtTgt2;
uint16_t pad0x86;
int32_t dwCvtSk2;
uint8_t nCvtMo3;
uint8_t nCvtTgt3;
uint16_t pad0x8E;
int32_t dwCvtSk3;
};

struct D2MonStats2ChoiceCountsStrc
{
uint8_t nHD_ChoiceCount;
uint8_t nTR_ChoiceCount;
uint8_t nLG_ChoiceCount;
uint8_t nRa_ChoiceCount;
uint8_t nLa_ChoiceCount;
uint8_t nRH_ChoiceCount;
uint8_t nLH_ChoiceCount;
uint8_t nSH_ChoiceCount;
uint8_t nS1_ChoiceCount;
uint8_t nS2_ChoiceCount;
uint8_t nS3_ChoiceCount;
uint8_t nS4_ChoiceCount;
uint8_t nS5_ChoiceCount;
uint8_t nS6_ChoiceCount;
uint8_t nS7_ChoiceCount;
uint8_t nS8_ChoiceCount;
};

union D2MonStats2ChoiceCountsUnion
{
struct
{
uint8_t nHD_ChoiceCount;
uint8_t nTR_ChoiceCount;
uint8_t nLG_ChoiceCount;
uint8_t nRa_ChoiceCount;
uint8_t nLa_ChoiceCount;
uint8_t nRH_ChoiceCount;
uint8_t nLH_ChoiceCount;
uint8_t nSH_ChoiceCount;
uint8_t nS1_ChoiceCount;
uint8_t nS2_ChoiceCount;
uint8_t nS3_ChoiceCount;
uint8_t nS4_ChoiceCount;
uint8_t nS5_ChoiceCount;
uint8_t nS6_ChoiceCount;
uint8_t nS7_ChoiceCount;
uint8_t nS8_ChoiceCount;
};
uint8_t nComponentChoiceCounts[16];
};

union D2MonStats2FlagsUnion
{
uint32_t dwFlags;
uint8_t nFlags[4];
};

union D2MonStats2ModeFlagsUnion
{
uint32_t dwModeFlags;
uint8_t nModeFlags[4];
};

struct D2UnkMonCompStrc
{
char nComposit[12];
};

struct D2MonStats2Txt
{
uint32_t dwId;
union
{
uint32_t dwFlags;
uint8_t nFlags[4];
};
uint8_t nSizeX;
uint8_t nSizeY;
uint8_t nSpawnCol;
uint8_t nHeight;
uint8_t nOverlayHeight;
uint8_t nPixHeight;
uint8_t nMeleeRng;
uint8_t pad0x0F;
int32_t nBaseW;
uint8_t nHitClass;
union
{
struct
{
uint8_t nHD_ChoiceCount;
uint8_t nTR_ChoiceCount;
uint8_t nLG_ChoiceCount;
uint8_t nRa_ChoiceCount;
uint8_t nLa_ChoiceCount;
uint8_t nRH_ChoiceCount;
uint8_t nLH_ChoiceCount;
uint8_t nSH_ChoiceCount;
uint8_t nS1_ChoiceCount;
uint8_t nS2_ChoiceCount;
uint8_t nS3_ChoiceCount;
uint8_t nS4_ChoiceCount;
uint8_t nS5_ChoiceCount;
uint8_t nS6_ChoiceCount;
uint8_t nS7_ChoiceCount;
uint8_t nS8_ChoiceCount;
};
uint8_t nComponentChoiceCounts[16];
};
uint8_t unk0x25;
D2UnkMonCompStrc unk0x26[16];
uint8_t pad0xE6[2];
uint32_t dwComponentFlags;
uint8_t nTotalPieces;
uint8_t pad0xED[3];
union
{
uint32_t dwModeFlags;
uint8_t nModeFlags[4];
};
uint8_t nModeDirections[16];
uint8_t nModeEnabledWhenMovingFlags[4];
uint8_t nInfernoLen;
uint8_t nInfernoAnim;
uint8_t nInfernoRollback;
uint8_t nResurrectMode;
uint16_t nResurrectSkill;
uint16_t whtTop;
uint16_t whtLeft;
uint16_t whtWidth;
uint16_t whtHeight;
uint8_t pad0x116[2];
uint32_t dwAutomapCel;
uint8_t nLocalBlood;
uint8_t nBleed;
uint8_t nLight;
uint8_t nLight_R;
uint8_t nLight_G;
uint8_t nLight_B;
uint8_t nUtrans[3];
uint8_t pad0x125[3];
char szHeart[4];
char szBodyPart[4];
uint8_t nRestore;
uint8_t pad0x131[3];
};

union D2MonStatsFlagsUnion
{
uint32_t dwMonStatsFlags;
uint8_t nMonStatsFlags[4];
};

struct D2MonStatsInitStrc
{
int32_t nMinHP;
int32_t nMaxHP;
int32_t nAC;
int32_t nTH;
int32_t nExp;
int32_t nA1MinD;
int32_t nA1MaxD;
int32_t nA2MinD;
int32_t nA2MaxD;
int32_t nS1MinD;
int32_t nS1MaxD;
int32_t nElMinD;
int32_t nElMaxD;
int32_t nElDur;
};

struct D2MonStatsTxt
{
int16_t nId;
int16_t nBaseId;
int16_t nNextInClass;
uint16_t wNameStr;
uint16_t wDescStr;
uint16_t unk0x0A;
union
{
uint32_t dwMonStatsFlags;
uint8_t nMonStatsFlags[4];
};
uint32_t dwCode;
int16_t wMonSound;
int16_t wUMonSound;
uint16_t wMonStatsEx;
uint16_t wMonProp;
int16_t wMonType;
uint16_t wAI;
uint16_t wSpawn;
uint8_t nSpawnX;
uint8_t nSpawnY;
uint8_t nSpawnMode;
uint8_t unk0x25;
int16_t wMinion[2];
int16_t nMonEquipTxtRecordId;
uint8_t nPartyMin;
uint8_t nPartyMax;
uint8_t nRarity;
uint8_t nMinGrp;
uint8_t nMaxGrp;
uint8_t nSparsePopulate;
int16_t nVelocity;
int16_t nRun;
int16_t nWalkAnimSpeed;
int16_t nRunAnimSpeed;
uint16_t wMissA1;
uint16_t wMissA2;
uint16_t wMissS1;
uint16_t wMissS2;
uint16_t wMissS3;
uint16_t wMissS4;
uint16_t wMissC;
uint16_t wMissSQ;
uint8_t nMaxChainId;
uint8_t nChainId;
uint8_t nAlign;
uint8_t nTransLvl;
uint8_t nThreat;
uint8_t nAIdel[3];
uint8_t nAiDist[3];
uint8_t unk0x55;
int16_t wAiParam[8][3];
uint16_t wTreasureClass[3][4];
uint8_t nTCQuestId;
uint8_t nTCQuestCP;
uint8_t nDrain[3];
uint8_t nToBlock[3];
uint8_t nCrit;
uint8_t unk0xA7;
int16_t wSkillDamage;
uint16_t nLevel[3];
uint16_t nMinHP[3];
uint16_t nMaxHP[3];
uint16_t nAC[3];
uint16_t nA1TH[3];
uint16_t nA2TH[3];
uint16_t nS1TH[3];
uint16_t nExp[3];
uint16_t nA1MinD[3];
uint16_t nA1MaxD[3];
uint16_t nA2MinD[3];
uint16_t nA2MaxD[3];
uint16_t nS1MinD[3];
uint16_t nS1MaxD[3];
uint8_t nElMode[3];
uint8_t nElType[3];
uint8_t nElPct[3][3];
uint8_t unk0x10D;
uint16_t nElMinD[3][3];
uint16_t nElMaxD[3][3];
uint16_t nElDur[3][3];
uint16_t nResistances[6][3];
char nColdEffect[3];
uint8_t unk0x16B;
uint8_t nSendSkills[4];
int16_t nSkill[8];
uint8_t nSkillMode[8];
uint16_t nSequence[8];
uint8_t nSkLvl[8];
uint32_t dwDamageRegen;
uint8_t nSplEndDeath;
uint8_t nSplGetModeChart;
uint8_t nSplEndGeneric;
uint8_t nSplClientEnd;
};

struct D2MonTypeTxt
{
uint16_t wType;
int16_t nEquiv[3];
uint16_t wStrSing;
uint16_t wStrPlur;
};

struct D2MonUModTxt
{
uint32_t dwUniqueMod;
uint16_t wVersion;
uint8_t nEnabled;
uint8_t nXfer;
uint8_t nChampion;
uint8_t nFPick;
int16_t wExclude[2];
uint16_t wCPick[3];
uint16_t wUPick[3];
uint16_t pad0x1A;
uint32_t dwConstants;
};

union D2MonsterDataAiParamUnion
{
struct D2AiParamStrc *pAiParam;
Unicode *wszMonName;
};

union D2MonsterDataInteractUnion
{
D2MonsterInteractStrc *pMonInteract;
int32_t nCritterIdleTicks;
};

struct D2UnkMonsterDataStrc
{
int32_t unk0x00;
union
{
int32_t nLifePercentage;
int32_t nMinionClassId;
};
int32_t unk0x08;
uint8_t nCount;
uint8_t pad0x0D[3];
};

struct D2MonsterDataStrc
{
D2MonStatsTxt *pMonstatsTxt;
uint8_t nComponent[16];
uint16_t wNameSeed;
D2C_MonTypeFlags nTypeFlag;
uint32_t dwDurielFlag;
uint8_t nMonUmod[10];
uint16_t wBossHcIdx;
struct D2AiControlStrc *pAiControl;
union
{
struct D2AiParamStrc *pAiParam;
Unicode *wszMonName;
};
union
{
D2MonsterInteractStrc *pMonInteract;
int32_t nCritterIdleTicks;
};
int32_t bCritterLatch;
uint32_t unk0x38;
D2UnkMonsterDataStrc unk0x3C;
uint32_t unk0x4C;
uint32_t pVision;
uint32_t dwAiState;
uint32_t dwTxtLevelNo;
uint16_t wSummonerFlag;
uint8_t pad0x5E[2];
};

struct D2MonsterInteractStrc
{
D2InteractInfoStrc *pInteractInfo;
};

struct D2NpcHirelingOfferStrc
{
uint16_t wMercName;
int16_t wUnk02;
int32_t nSeed;
int32_t bUsed;
int32_t nUnk0C;
};

struct D2NpcMenuDescStrc
{
int32_t nMonsterId;
int32_t nRows;
uint16_t wTblIdx[5];
void *pfnHandler[5];
uint8_t bEnabled;
};

struct D2NpcMenuEntryStrc
{
uint32_t nMonsterClassId;
uint8_t unk0x04;
struct D2NpcSpeechLineStrc *pSpeechLineTable;
uint32_t nSpeechLineCount;
uint32_t nSelectedLine;
uint8_t bRerollLine;
uint8_t unk0x12;
uint8_t unk0x13;
uint8_t bIntroHeard;
uint8_t bAborted;
};

struct D2NpcMenuQuestUnlockStrc
{
int32_t nMonsterClassId;
D2QuestStateFlagIds nQuestStateFlag;
int32_t nQuestState;
int32_t bWantState;
int32_t (__cdecl *pfnAddRow)();
};

typedef uint32_t D2UnitGUID;

struct D2NpcMenuUIStateStrc
{
uint8_t bNpcSkipMenuOnInteract;
uint8_t bNpcSpeechActive;
uint8_t bNpcSpeechLinesRolled;
int32_t CurrentState;
uint8_t unk0x07[4];
int32_t nCachedPlayerGold;
uint16_t wNpcSpeechTblId;
uint16_t wNpcNextSpeechTblId;
uint16_t wNpcShownSpeechTblId;
D2NpcMenuEntryStrc *pNpcSpeechDesc;
int32_t nTalkMenuBaseEntryIdx;
D2NpcHirelingOfferStrc NpcHirelingOffers[10];
D2UnitGUID NpcUnitGUID;
BOOL bNpcInteractionActive;
int32_t NpcClassId;
int32_t Transaction;
uint8_t unk0xCD[4];
BOOL bNpcQuestMsgPending;
uint16_t wNpcQuestMsgId;
BOOL bTransactionItemWasOnCursor;
D2BitBufferStrc *pNpcQuestRecord;
D2BitBufferStrc *pQuestRecordSecondary;
int32_t unk0xE3;
int32_t unk0xE7;
uint32_t nTransactionTimeoutTick;
uint32_t nNextItemActionTick;
D2C_Sounds nTransactionResultSoundId;
D2ClientDialogStrc *hDialogConfirm;
D2ClientDialogStrc *hDialogResults;
D2ClientDialogStrc *hDialogWaiting;
D2ClientDialogStrc *hDialogMainMenu;
D2ClientDialogStrc *hDialogTalkMenu;
D2ClientDialogStrc *hDialogHireList;
D2ClientDialogStrc *hDialogInterrupt;
int32_t TransactionCost;
void *pHireListControl;
D2GSPacketSrv2A LastTransactionResult;
BOOL bWaitingForItemMsg;
int16_t wNpcSpeechesPlayed[10];
uint16_t nNpcSpeechesPlayed;
BOOL bNpcVendorGamble;
BOOL bNpcGreetingIsOneShot;
int32_t pNpcGreetingSound;
BOOL bNihlathakHurrySoundPlayed;
BOOL bNihlathakHurryTimerArmed;
uint32_t nNihlathakHurryTick;
};

struct D2NpcSpeechLineStrc
{
uint16_t nTblStringId;
uint8_t bHasQuestGate;
int32_t nRequiredState;
D2QuestStateFlagIds nQuestStateFlag;
int32_t nClassFilter;
};

struct D2NpcSpeechPairStrc
{
int32_t nMonsterClassId;
uint16_t wShownSpeechId;
uint16_t wNextSpeechId;
};

struct D2NpcTxt
{
uint32_t dwNpc;
uint32_t dwSellMult;
uint32_t dwBuyMult;
uint32_t dwRepMult;
uint32_t dwQuestFlag[3];
uint32_t dwQuestSellMult[3];
uint32_t dwQuestBuyMult[3];
uint32_t dwQuestRepMult[3];
int32_t nMaxBuy[3];
};

struct D2ObjGroupTxt
{
uint32_t dwId[8];
uint8_t nDensity[8];
uint8_t nProbability[8];
uint8_t nShrines;
uint8_t nWells;
uint16_t pad0x32;
};

struct D2ObjModeTypeTxt
{
char szName[32];
char szToken[20];
};

struct D2ObjectDataStrc
{
D2ObjectsTxt *pObjectTxt;
uint8_t InteractType;
uint8_t nPortalFlags;
uint16_t unk0x06;
D2ShrinesTxt *pShrineTxt;
uint32_t dwOperateGUID;
int32_t bPermanent;
uint32_t unk0x014;
D2CoordStrc DestRoomCooords;
D2CoordStrc DestPortalCoords;
char szOwner[16];
};

struct D2ObjectsTxt
{
char szName[64];
Unicode wszName[64];
char szToken[3];
uint8_t nSpawnMax;
uint8_t nSelectable[8];
uint8_t nTrapProb;
uint8_t pad0xCD[3];
int32_t dwSizeX;
int32_t dwSizeY;
uint32_t dwFrameCnt[8];
uint16_t wFrameDelta[8];
uint8_t nCycleAnim[8];
uint8_t nLit[8];
uint8_t nBlocksLight[8];
uint8_t nHasCollision[8];
uint8_t nIsAttackable0;
uint8_t nStart[8];
uint8_t nOrderFlag[8];
uint8_t nEnvEffect;
uint8_t nIsDoor;
uint8_t nBlocksVis;
uint8_t nOrientation;
uint8_t nPreOperate;
uint8_t nTrans;
uint8_t nMode[8];
uint8_t pad0x147;
int32_t dwXOffset;
int32_t dwYOffset;
uint8_t nDraw;
uint8_t nHD;
uint8_t nTR;
uint8_t nLG;
uint8_t nRA;
uint8_t nLA;
uint8_t nRH;
uint8_t nLH;
uint8_t nSH;
uint8_t nS1;
uint8_t nS2;
uint8_t nS3;
uint8_t nS4;
uint8_t nS5;
uint8_t nS6;
uint8_t nS7;
uint8_t nS8;
uint8_t nTotalPieces;
uint8_t nXSpace;
uint8_t nYSpace;
uint8_t nRed;
uint8_t nGreen;
uint8_t nBlue;
uint8_t nSubClass;
uint32_t dwNameOffset;
uint8_t pad0x16C;
uint8_t nMonsterOK;
uint8_t nOperateRange;
uint8_t nShrineFunction;
uint8_t nAct;
uint8_t nLockable;
uint8_t nGore;
uint8_t nRestore;
uint8_t nRestoreVirgins;
uint8_t nSync;
uint16_t pad0x176;
uint32_t dwParm[8];
uint8_t nTgtFX;
uint8_t nTgtFY;
uint8_t nTgtBX;
uint8_t nTgtBY;
uint8_t nDamage;
uint8_t nCollisionSubst;
uint16_t pad0x19E;
uint32_t dwLeft;
uint32_t dwTop;
uint32_t dwWidth;
uint32_t dwHeight;
uint8_t nBeta;
uint8_t nInitFn;
uint8_t nPopulateFn;
uint8_t nOperateFn;
uint8_t nClientFn;
uint8_t nOverlay;
uint8_t nBlockMissile;
uint8_t nDrawUnder;
uint8_t nOpenWarp;
uint8_t pad0x1B9[3];
uint32_t dwAutomap;
};

struct D2OverlayTxt
{
uint16_t wOverlay;
char szFilename[64];
uint16_t wVersion;
uint32_t dwFrames;
uint8_t nPreDraw;
uint8_t pad0x49[3];
uint32_t dw1ofN;
uint8_t nDir;
uint8_t nOpen;
uint8_t nBeta;
uint8_t pad0x53;
uint32_t dwOffsetX;
uint32_t dwOffsetY;
uint32_t dwHeight[4];
uint32_t dwAnimRate;
uint32_t dwInitRadius;
uint32_t dwRadius;
uint32_t dwLoopWaitTime;
uint8_t nTrans;
uint8_t nRed;
uint8_t nGreen;
uint8_t nBlue;
uint8_t nNumDirections;
uint8_t nLocalBlood;
uint16_t pad0x82;
};

struct D2PacketListStrc
{
int32_t nTotal;
int32_t nUsed;
int32_t unk0x08;
uint8_t *pPacketData;
};

struct D2PanelKeyBindStrc
{
int32_t nClearScreenPrimary;
int32_t nShowBeltPrimary;
int32_t nClearScreenSecondary;
int32_t nShowBeltSecondary;
int32_t nClearMessagesPrimary;
int32_t nClearMessagesSecondary;
int32_t nSwapWeaponsPrimary;
int32_t nSwapWeaponsSecondary;
struct D2WinCommandRegisterStrc *pHandlers;
};

struct D2PetInfoStrc
{
uint32_t nSeed;
uint16_t wName;
int16_t unk0x06;
int32_t nHirelingId;
};

struct D2PetDataStrc
{
uint32_t nFlags;
int32_t nUnitGUID;
D2PetInfoStrc petInfo;
D2PetDataStrc *pNext;
};

struct D2PetListStrc
{
D2PetDataStrc *pPetData;
int32_t nCurrent;
int32_t nMax;
};

struct D2PetTypeTxt
{
uint32_t dwHcIdx;
uint32_t dwPetFlags;
uint16_t wGroup;
uint16_t wBaseMax;
uint16_t wStringId;
uint8_t nIconType;
char szBaseicon[32];
char szIcon[4][32];
uint8_t pad0xAF[3];
uint16_t wClass[4];
uint8_t unk0xBA[2];
int32_t nSkillCount;
uint16_t wSkillIds[15];
uint16_t unk0xDE;
};

struct D2PlayerClassTxt
{
uint32_t dwCode;
};

struct D2PlayerDataStrc
{
char szName[16];
D2BitBufferStrc *pQuestData[3];
D2WaypointDataStrc *pWaypointData[3];
uint32_t unk0x28;
int32_t nPortalFlags;
int32_t unk0x30;
struct D2ArenaUnitStrc *pArenaUnit;
D2PlayerListStrc *pPlayerList;
uint32_t unk0x3C[2];
D2PlayerPetStrc *pPlayerPets;
uint32_t dwUniqueId;
uint32_t bBusy;
uint32_t dwTradeState;
uint32_t unk0x54;
uint32_t dwAcceptTradeTick;
D2PlayerTradeStrc *pTrade;
D2PlrIntroStrc *pPlayerIntro[3];
uint32_t dwBoughtItemId;
int32_t nRightSkillId;
int32_t nLeftSkillId;
int32_t nRightSkillFlags;
int32_t nLeftSkillFlags;
int32_t nSwitchRightSkillId;
int32_t nSwitchLeftSkillId;
int32_t nSwitchRightSkillFlags;
int32_t nSwitchLeftSkillFlags;
uint32_t nWeaponGUID;
uint32_t unk0x94;
D2UnkPlayerDataStrc *unk0x98;
struct D2ClientStrc *pClient;
uint8_t unk0xA0;
uint8_t unk0xA1[3];
uint32_t unk0xA4;
uint32_t unk0xA8[46];
uint32_t dwHostileDelay;
uint32_t unk0x164;
uint32_t dwGameFrame;
};

struct D2PlayerListStrc
{
int32_t nUnitGUID;
uint32_t dwFlags;
int32_t field_8;
int32_t field_C;
D2PlayerListStrc *pNext;
};

struct D2PlayerPetStrc
{
D2PetListStrc *pPetList;
};

struct D2PlayerTradeStrc
{
int32_t nSaveLength;
uint8_t *pSaveData;
D2ItemTradeStrc *pItemTrade;
int32_t nGoldInTrade;
int32_t nGoldInTradeOtherPlayer;
int32_t nNextUpdateTick;
};

struct D2PlrIntroStrc
{
D2BitBufferStrc *pQuestIntroFlags;
D2BitBufferStrc *pNpcIntroFlags;
};

struct D2PlrModeTxtStub
{
uint32_t dwCode;
};

struct D2PlrModeTypeTxt
{
char szName[32];
char szToken[20];
};

struct D2PopupStrc
{
struct D2ClientPopupStrc *pParsedText;
};

struct D2PresetUnitStrc
{
int32_t nUnitType;
int32_t nIndex;
int32_t nMode;
int32_t nXpos;
int32_t nYpos;
int32_t nSpawnFlags;
D2MapAIStrc *pMapAI;
D2PresetUnitStrc *pNext;
};

struct D2PropertiesTxt
{
uint16_t wProp;
uint8_t nSet[7];
uint8_t pad0x09;
uint16_t wVal[7];
uint8_t nFunc[7];
uint8_t pad0x1F;
uint16_t wStat[7];
};

struct D2QtyWarnLevelStrc
{
uint32_t nKind;
uint32_t nBaseFrame;
uint32_t nThresholdHigh;
uint32_t nThresholdMid;
uint32_t nThresholdLow;
};

struct D2QualityItemsTxt
{
uint8_t nArmor;
uint8_t nWeapon;
uint8_t nShield;
uint8_t nScepter;
uint8_t nWand;
uint8_t nStaff;
uint8_t nBow;
uint8_t nBoots;
uint8_t nGloves;
uint8_t nBelt;
uint8_t nNumMods;
uint8_t pad0x0B;
D2PropertyStrc pProperties[2];
char szEffect1[32];
char szEffect2[32];
uint16_t wEffect1TblId;
uint16_t wEffect2TblId;
};

struct D2QuestLogCloseRectStrc
{
int32_t nLeft;
int32_t nRight;
int32_t nTop;
int32_t nBottom;
int32_t nTitleY;
};

struct D2QuestLogEntryStrc
{
uint8_t bShown;
D2QuestStateFlagIds nQuestStateFlag;
uint16_t wTitleStringId;
uint16_t wszText[300];
uint16_t wTailStringId;
uint8_t nSlotInPage;
uint8_t nIconIndex;
uint8_t nFlag0x263;
uint8_t bHasText;
uint8_t nStateColour;
int32_t nState;
};

struct D2QuestLogQuestDefStrc
{
uint8_t bHasIcon;
uint8_t nIconIndex;
uint8_t nSlotInPage;
uint8_t nAct;
struct D2QuestLogTextBlockStrc *pTextBlock;
D2Quests nQuestId;
D2QuestStateFlagIds nQuestStateFlag;
};

struct D2QuestLogTextPairStrc
{
uint16_t wTextStringId;
uint16_t wTailStringId;
};

struct D2QuestLogTextBlockStrc
{
uint16_t wTitleStringId;
uint16_t wDescStringId;
uint16_t wStateIndex;
struct D2QuestLogTextPairStrc tStates[14];
uint16_t wPad3E;
};

struct D2QuestMenuStringStrc
{
uint16_t nQuestStringId;
uint16_t nTblIdx;
};

struct D2RareAffixTxt
{
uint32_t unk0x00[3];
uint16_t wStringId;
uint16_t wVersion;
uint16_t wIType[7];
uint16_t wEType[4];
char szName[32];
uint16_t pad0x46;
};

struct D2RoomCollisionGridStrc
{
D2DrlgCoordsStrc pRoomCoords;
uint16_t *pCollisionMask;
};

struct D2RoomCoordListStrc
{
D2DrlgCoordStrc pBox[2];
int32_t bNode;
int32_t bRoomActive;
int32_t nIndex;
D2RoomCoordListStrc *pNext;
};

struct D2RoomTileStrc
{
D2DrlgRoomStrc *pDrlgRoom;
D2LvlWarpTxt *pLvlWarpTxtRecord;
int32_t bEnabled;
D2DrlgTileDataStrc *unk0x0C;
D2DrlgTileDataStrc *unk0x10;
D2RoomTileStrc *pNext;
};

struct D2RosterCorpseStrc
{
uint32_t dwCorpseId;
struct D2RosterCorpseStrc *pPrev;
};

struct D2RosterGroupStrc
{
uint16_t wPartyId;
uint16_t unk0x02;
struct D2RosterUnitStrc *pMembers;
struct D2RosterGroupStrc *pNext;
};

struct D2RosterInfoStrc
{
uint32_t dwUnitId;
uint32_t dwPartyFlags;
struct D2RosterInfoStrc *pNext;
};

struct D2RosterPetStrc
{
int32_t nMonStatsId;
int32_t nPetTypeId;
uint32_t dwUnitId;
uint32_t dwOwnerId;
uint32_t unk0x10;
uint32_t unk0x14;
uint32_t unk0x18;
int32_t nLifePercent;
int32_t bInactive;
D2ClientPetInfoStrc tPetInfo;
struct D2RosterPetStrc *pNext;
};

struct D2RosterUnitStrc
{
char szName[16];
uint32_t dwUnitId;
uint32_t dwPartyLife;
uint32_t dwKillCount;
uint32_t dwClassId;
uint16_t wLevel;
uint16_t wPartyId;
uint32_t dwLevelId;
uint32_t dwPosX;
uint32_t dwPosY;
D2C_PartyButtons eButton;
struct D2RosterInfoStrc **pRosterInfo;
struct D2RosterCorpseStrc *pCorpse;
uint32_t dwSrcPortalGUID;
uint32_t dwDstPortalGUID;
uint16_t wGuildFlags;
char szGuildTag[4];
char szGuildName[28];
char szNameEx[26];
struct D2RosterUnitStrc *pNext;
};

struct D2SCmdStrc
{
void (__fastcall *pfProcessCmd)(uint8_t *pPacket);
int32_t nCmdSize;
void (__fastcall *pfProcessUnit)(D2UnitStrc *pUnit, uint8_t *pPacket);
};

struct D2SLayerStatIdPartsStrc
{
uint16_t nLayer;
uint16_t nStat;
};

struct D2SLayerStatIdStrc
{
union
{
struct
{
uint16_t nLayer;
uint16_t nStat;
};
int32_t nPackedValue;
};
};

union D2SLayerStatIdUnion
{
struct
{
uint16_t nLayer;
uint16_t nStat;
};
int32_t nPackedValue;
};

struct D2SeedPartsStrc
{
uint32_t nLowSeed;
uint32_t nHighSeed;
};

union D2SeedUnion
{
struct
{
uint32_t nLowSeed;
uint32_t nHighSeed;
};
uint64_t lSeed;
};

struct D2SetItemsTxt
{
uint16_t wSetItemId;
char szName[32];
uint16_t wVersion;
uint16_t wStringId;
uint16_t pad0x26;
uint32_t szItemCode;
int16_t nSetId;
int16_t nSetItems;
uint16_t wLvl;
uint16_t wLvlReq;
uint32_t dwRarity;
uint32_t dwCostMult;
uint32_t dwCostAdd;
int8_t nChrTransform;
int8_t nInvTransform;
char szFlippyFile[32];
char szInvFile[32];
uint16_t wDropSound;
uint16_t wUseSound;
uint8_t nDropSfxFrame;
uint8_t nAddFunc;
D2PropertyStrc pProperties[9];
D2PropertyStrc pPartialBoni[10];
};

struct D2SetsTxt
{
uint16_t wSetId;
uint16_t wStringId;
uint16_t wVersion;
uint16_t pad0x06;
uint32_t unk0x08;
int32_t nSetItems;
D2PropertyStrc pBoni2[2];
D2PropertyStrc pBoni3[2];
D2PropertyStrc pBoni4[2];
D2PropertyStrc pBoni5[2];
D2PropertyStrc pFBoni[8];
D2SetItemsTxt *pSetItem[6];
};

struct D2ShadowLightGammaPaletteStrc
{
PALETTEENTRY shadowsPalette[32][64];
PALETTEENTRY lightPalette[16][64];
PALETTEENTRY gammaPalette[64];
};

struct D2ShrinesTxt
{
uint8_t nCode;
uint8_t pad0x01[3];
uint32_t dwArg0;
uint32_t dwArg1;
uint32_t dwDurationInFrames;
uint8_t nResetTimeInMins;
uint8_t nRarity;
char szViewname[32];
char szNiftyphrase[128];
uint8_t nEffectClass;
uint8_t pad0xB3;
uint32_t dwLevelMin;
};

struct D2SkillCalcTxt
{
uint32_t dwCode;
};

struct D2SkillDescTxt
{
uint16_t wSkillDesc;
uint8_t nSkillPage;
uint8_t nSkillRow;
uint8_t nSkillColumn;
uint8_t nListRow;
uint8_t nListPool;
uint8_t nIconCel;
uint16_t wStrName;
uint16_t wStrShort;
uint16_t wStrLong;
uint16_t wStrAlt;
uint16_t wStrMana;
uint16_t wDescDam;
uint16_t wDescAtt;
uint16_t pad0x16;
uint32_t dwDamCalc[2];
uint8_t nPrgDamElem[3];
uint8_t pad0x23;
uint32_t dwProgDmgMin[3];
uint32_t dwProgDmgMax[3];
uint16_t wDescMissile[3];
uint8_t nDescLine[17];
uint8_t pad0x53;
uint16_t wDescTextA[17];
uint16_t wDescTextB[17];
uint32_t dwDescCalcA[17];
uint32_t dwDescCalcB[17];
};

struct D2SkillListStrc
{
void *pMemPool;
D2SkillStrc *pFirstSkill;
D2SkillStrc *pLeftSkill;
D2SkillStrc *pRightSkill;
D2SkillStrc *pUsedSkill;
void *pCastLight;
};

struct D2SkillStrc
{
D2SkillsTxt *pSkillsTxt;
D2SkillStrc *pNextSkill;
uint32_t dwSkillMode;
uint32_t dwFlags;
uint32_t unk0x10[2];
union
{
uint32_t dwTargetInfo;
uint32_t nXpos;
uint32_t nPar1;
};
union
{
uint32_t dwTargetType;
uint32_t nYpos;
uint32_t nPar2;
};
union
{
uint32_t dwTargetGUID;
uint32_t nPar3;
};
uint32_t nPar4;
int32_t nSkillLevel;
uint32_t nLevelBonus;
int32_t nQuantity;
uint32_t nOwnerGUID;
int32_t nCharges;
};

union D2SkillTargetGUIDUnion
{
uint32_t dwTargetGUID;
uint32_t nPar3;
};

union D2SkillTargetXUnion
{
uint32_t dwTargetInfo;
uint32_t nXpos;
uint32_t nPar1;
};

union D2SkillTargetYUnion
{
uint32_t dwTargetType;
uint32_t nYpos;
uint32_t nPar2;
};

struct D2SkillsTxt
{
int16_t nSkillId;
uint16_t unk0x02;
uint32_t dwFlags[2];
int8_t nCharClass;
uint8_t unk0x0D[3];
uint8_t nAnim;
uint8_t nMonAnim;
uint8_t nSeqTrans;
uint8_t nSeqNum;
uint8_t nRange;
uint8_t nSelectProc;
uint8_t nSeqInput;
uint8_t pad0x17;
int16_t nITypeA[3];
int16_t nITypeB[3];
int16_t nETypeA[2];
int16_t nETypeB[2];
int16_t wSrvStartFunc;
uint16_t wSrvDoFunc;
uint16_t wSrvPrgFunc[3];
uint16_t pad0x36;
uint32_t dwPrgCalc[3];
uint8_t nPrgDamage;
uint8_t pad0x45;
int16_t wSrvMissile;
int16_t wSrvMissileA;
int16_t wSrvMissileB;
int16_t wSrvMissileC;
int16_t wSrvOverlay;
uint32_t dwAuraFilter;
int16_t wAuraStat[6];
uint32_t dwAuraLenCalc;
uint32_t dwAuraRangeCalc;
int32_t dwAuraStatCalc[6];
int16_t nAuraState;
int16_t wAuraTargetState;
int16_t wAuraEvent[3];
int16_t wAuraEventFunc[3];
uint16_t wAuraTgtEvent;
uint16_t wAuraTgtEventFunc;
int16_t nPassiveState;
int16_t nPassiveIType;
int16_t nPassiveStat[5];
uint16_t pad0xA2;
uint32_t dwPassiveCalc[5];
uint16_t wPassiveEvent;
uint16_t wPassiveEventFunc;
uint16_t wSummon;
int8_t nPetType;
int8_t nSumMode;
uint32_t dwPetMax;
uint16_t wSumSkill[5];
uint16_t pad0xCE;
uint32_t dwSumSkCalc[5];
int16_t wSumUMod;
int16_t wSumOverlay;
uint16_t wCltMissile;
uint16_t wCltMissileA;
uint16_t wCltMissileB;
uint16_t wCltMissileC;
uint16_t wCltMissileD;
uint16_t wCltStFunc;
uint16_t wCltDoFunc;
uint16_t wCltPrgFunc[3];
int16_t wStSound;
uint16_t nStSoundClass;
uint16_t wDoSound;
uint16_t wDoSoundA;
uint16_t wDoSoundB;
uint16_t wCastOverlay;
uint16_t wTgtOverlay;
uint16_t wTgtSound;
uint16_t wPrgOverlay;
uint16_t wPrgSound;
uint16_t wCltOverlayA;
uint16_t wCltOverlayB;
int32_t dwCltCalc[3];
D2C_SkillsTxtItemTarget nItemTarget;
uint8_t pad0x121;
uint16_t wItemCastSound;
uint16_t wItemCastOverlay;
uint16_t pad0x126;
uint32_t dwPerDelay;
uint16_t wMaxLvl;
uint16_t wResultFlags;
uint32_t dwHitFlags;
uint32_t dwHitClass;
uint32_t dwCalc[4];
int32_t dwParam[8];
uint8_t nWeapSel;
uint8_t pad0x169;
uint16_t wItemEffect;
uint16_t wItemCltEffect;
uint16_t pad0x16E;
uint32_t dwSkPoints;
uint16_t wReqLevel;
uint16_t wReqStr;
uint16_t wReqDex;
uint16_t wReqInt;
uint16_t wReqVit;
int16_t nReqSkill[3];
int16_t wStartMana;
uint16_t wMinMana;
uint8_t nManaShift;
uint8_t pad0x189;
int16_t wMana;
int16_t wLevelMana;
uint8_t nAttackRank;
D2C_SkillsTxtLineOfSight nLineOfSight;
uint32_t dwDelay;
uint16_t wSkillDesc;
uint16_t pad0x196;
uint32_t dwToHit;
uint32_t dwLevToHit;
uint32_t dwToHitCalc;
uint8_t nToHitShift;
uint8_t nSrcDam;
uint16_t pad0x1A6;
uint32_t dwMinDam;
uint32_t dwMaxDam;
uint32_t dwMinLvlDam[5];
uint32_t dwMaxLvlDam[5];
uint32_t dwDmgSymPerCalc;
uint8_t nEType;
uint8_t pad0x1DD[3];
uint32_t dwEMin;
uint32_t dwEMax;
uint32_t dwEMinLev[5];
uint32_t dwEMaxLev[5];
uint32_t dwEDmgSymPerCalc;
uint32_t dwELen;
uint32_t dwELevLen[3];
uint32_t dwELenSymPerCalc;
uint8_t nRestrict;
uint8_t pad0x229;
int16_t nState[3];
uint8_t nAiType;
uint8_t pad0x231;
int16_t wAiBonus;
int32_t nCostMult;
int32_t nCostAdd;
};

struct D2SoundChannelCacheStrc
{
float fPosX;
float fPosY;
float fPosZ;
int32_t nVolume;
float fReverbMix;
};

struct D2SoundChannelStrc
{
void *hSound;
int32_t nKind;
D2SoundInstanceStrc *pInstance;
D2SoundChannelCacheStrc tCache;
};

struct D2SoundEnvironTxt
{
D2C_Sounds nSong;
D2C_Sounds nDayAmbience;
D2C_Sounds nNightAmbience;
D2C_Sounds nDayEvent;
D2C_Sounds nNightEvent;
uint32_t nEventDelay;
uint8_t bIndoors;
uint8_t pad0x19[3];
int32_t nMaterial1;
int32_t nMaterial2;
int32_t nEaxEnviron;
int32_t nEaxEnvSize;
int32_t nEaxEnvDiff;
int32_t nEaxRoomVol;
int32_t nEaxRoomHF;
int32_t nEaxDecayTime;
int32_t nEaxDecayHF;
int32_t nEaxReflect;
int32_t nEaxReflectDelay;
int32_t nEaxReverb;
int32_t nEaxRevDelay;
int32_t nEaxRoomRoll;
int32_t nEaxAirAbsorb;
};

struct D2SoundInstanceStrc
{
int32_t nInUse;
D2C_Sounds nSoundId;
int32_t hInstance;
float fPosX;
float fPosY;
float fPosZ;
float fDistanceSquared;
int32_t nBaseVolume;
float fReverbMix;
int32_t nStartBlock;
D2SoundListNodeStrc *pUnitList;
struct D2SoundChannelStrc *ptChannel;
int32_t eState;
uint32_t dwStartTick;
int32_t nPlayFlags;
uint8_t nPriority;
int32_t bStarted;
int32_t nSavedStreamPos;
int32_t bFading;
int32_t nFadeFromVolume;
int32_t nFadeToVolume;
uint32_t dwFadeStartTick;
uint32_t dwFadeEndTick;
};

struct D2SoundListNodeStrc
{
int32_t nValue;
struct D2SoundListNodeStrc *pNext;
};

struct D2SoundsTxt
{
char szFileName[59];
uint8_t unk0x3B;
uint8_t nVolume;
uint8_t nGroupSize;
uint8_t bLoop;
uint8_t nFadeIn;
uint8_t nFadeOut;
uint8_t nDeferInst;
uint8_t nStopInst;
uint16_t nDuration;
uint16_t nCompound;
uint32_t dwFalloff;
uint8_t nReverb;
uint8_t bCache;
uint8_t bAsyncOnly;
uint8_t nPriority;
uint8_t bStream;
uint8_t bStereo;
uint8_t bTracking;
uint8_t bSolo;
uint8_t nMusicVol;
int32_t nBlock1;
int32_t nBlock2;
int32_t nBlock3;
int32_t nGroupBaseIndex;
D2C_Sounds nRecentPicks[2];
void *pWaveData;
uint32_t nWaveSize;
void *pPcmData;
int32_t nPcmSize;
int32_t nCacheStamp;
uint8_t nBlockCount;
uint8_t bLoadFailed;
AsyncData *hAsyncData;
int32_t eDataState;
uint32_t dwRefCount;
};

struct D2SoundsTxtStub
{
uint16_t wSound;
};

struct D2SpellSelPanelStrc
{
int32_t nX[60];
int32_t nY[60];
int32_t nSkillId[60];
D2UnitGUID OwnerGUID[60];
uint8_t nCount;
uint32_t nLastTextColor;
};

struct D2SplittedTextStrc
{
struct Unicode *wszLine;
struct D2SplittedTextStrc *pNextLine;
};

struct D2StatsArrayStrc
{
D2StatStrc *pStat;
uint16_t nStatCount;
uint16_t nCapacity;
};

struct D2StatListStrc
{
void *pMemPool;
D2UnitStrc *pUnit;
uint32_t dwOwnerType;
uint32_t dwOwnerId;
uint32_t dwFlags;
uint32_t dwStateNo;
int32_t dwExpireFrame;
uint32_t dwSkillNo;
uint32_t dwSLvl;
D2StatsArrayStrc Stats;
D2StatListStrc *pPrevLink;
D2StatListStrc *pNextLink;
D2StatListStrc *pParent;
void (__fastcall *fpStatRemove)(D2UnitStrc *, int32_t, D2StatListStrc *);
};

struct D2StatListExStrc : D2StatListStrc
{
D2StatListStrc *pMyLastList;
D2StatListStrc *pMyStats;
D2UnitStrc *pOwner;
D2StatsArrayStrc FullStats;
D2ModStatsArrayStrc ModStats;
uint32_t *StatFlags;
void (__fastcall *pfOnValueChanged)(struct D2GameStrc *, D2UnitStrc *, D2UnitStrc *, int32_t, int32_t, int32_t);
struct D2GameStrc *pGame;
};

struct D2StatStrc : D2SLayerStatIdStrc
{
int32_t nValue;
};

struct D2StatesTxt
{
uint16_t wState;
uint16_t wOverlay[4];
uint16_t wCastOverlay;
uint16_t wRemoveOverlay;
uint16_t wPrgOverlay;
union
{
uint8_t nStateFlags[4];
uint32_t dwStateFlags;
};
uint32_t dwStateFlagsEx;
uint16_t wStat;
uint16_t wSetFunc;
uint16_t wRemFunc;
uint16_t wGroup;
uint8_t nColorPri;
uint8_t nColorShift;
uint8_t nLightRGB[3];
uint8_t pad0x25;
uint16_t wOnSound;
uint16_t wOffSound;
uint16_t wItemType;
int8_t nItemTrans;
uint8_t nGfxType;
uint16_t wGfxClass;
uint16_t wCltEvent;
uint16_t wCltEventFunc;
uint16_t wCltActiveFunc;
uint16_t wSrvActiveFunc;
uint16_t wSkill;
uint16_t wMissile;
};

union D2StatesTxtFlagsUnion
{
uint8_t nStateFlags[4];
uint32_t dwStateFlags;
};

struct D2StaticPathStrc
{
D2ActiveRoomStrc *pRoom;
int32_t dwClientCoordX;
int32_t dwClientCoordY;
D2CoordStrc tGameCoords;
uint32_t unk0x14[2];
uint8_t nDirection;
uint8_t bRoomNeedsUpdate;
uint8_t unk0x1E[2];
};

struct D2StorePageTxt
{
uint32_t dwCode;
};

struct D2SuperUniquesTxt
{
uint16_t wSuperUnique;
uint16_t wNameStr;
uint32_t dwClass;
uint32_t dwHcIdx;
uint32_t dwMod[3];
int32_t dwMonSound;
uint32_t dwMinGrp;
uint32_t dwMaxGrp;
uint8_t nAutopos;
uint8_t nEclass;
uint8_t nStacks;
uint8_t nReplaceable;
uint8_t nUtrans[3];
uint8_t pad0x2B;
uint16_t dwTC[3];
uint16_t pad0x32;
};

struct D2TCExInfoStrc
{
int32_t nClassic;
int32_t nProb;
int16_t nItemId;
int16_t nTxtRow;
uint8_t nFlags;
int8_t unk0x0D;
int16_t nMagic;
int16_t nRare;
int16_t nSet;
int16_t nUnique;
int16_t nSuperior;
int16_t nNormal;
int16_t unk0x1A;
};

struct D2TCExShortStrc
{
int16_t nGroup;
int16_t nLevel;
int32_t nTypes;
int32_t nClassic;
int32_t nProb;
int32_t nPicks;
int32_t nNoDrop;
int16_t unk0x18;
int16_t nMagic;
int16_t nRare;
int16_t nSet;
int16_t nUnique;
int16_t nSuperior;
int16_t nNormal;
int16_t unk0x26;
D2TCExInfoStrc *pInfo;
};

struct D2TextDisplayListStrc
{
struct D2ClientTextNodeStrc *pHead;
uint16_t wCount;
};

struct D2TextHeaderStrc
{
void *pMemPool;
uint16_t nCount;
uint16_t pad0x06;
D2TextNodeStrc *pNode;
};

struct D2TextNodeStrc
{
int16_t nStringId;
uint16_t pad0x02;
int32_t nMenu;
D2TextNodeStrc *pNext;
};

struct D2TextStateStrc
{
void *pMessageLogDialog;
void *pMessageLogListBox;
int32_t field_08;
int32_t bPopupDismissed;
int32_t bPopupActive;
int32_t bKeepNpcInteraction;
int32_t bDisplayingStaticInfo;
int32_t field_1C;
int32_t nStaticInfoPanelX;
int32_t nStaticInfoPanelY;
int32_t field_28;
int32_t field_2C;
int32_t field_30;
struct D2TextDisplayListStrc GlobalMessageDisplayList;
struct D2TextDisplayListStrc QuickMessageDisplayList;
struct D2TextDisplayListStrc MessageLogDisplayList;
int32_t nTextDisplayDelay;
int32_t bForceDrawPopupText;
int32_t field_4E;
int32_t dwPopupNpcGUID;
uint32_t dwPopupObjectGUID;
int32_t bPopupIsNpcSpeech;
int32_t nStaticInfoTimeout;
uint16_t nPopupStringId;
RECT rcTextInfoPanel;
int32_t nHoverMsgCount;
D2ItemsTxt *pOpenScrollItemsTxt;
int32_t dwPopupStartTick;
int32_t bPopupNeedsAck;
uint16_t field_84;
int32_t bPopupIsQuestText;
uint8_t bPopupHasOwnerUnit;
uint32_t dwPopupOwnerUnitGUID;
int32_t bDismissedByMouse;
uint32_t dwQuestScrollLastTick;
int32_t nQuestScrollFrame;
int32_t hPopupSpeechSound;
};

struct D2TileLibraryEntryStrc
{
int32_t nLightDirection;
uint16_t nRoofHeight;
uint16_t nFlags;
int32_t nTotalHeight;
int32_t nWidth;
int32_t nHeightToBottom;
int32_t nType;
int32_t nStyle;
int32_t nSequence;
int32_t nRarity_Frame;
int32_t transparentColorRGB24;
uint8_t dwTileFlags[4];
int32_t dwBlockOffset_pBlock;
int32_t nBlockSize;
int32_t nBlocks;
D2TileRecordStrc *pParent;
uint16_t unk0x3C;
uint16_t nCacheIndex;
uint32_t unk0x40[4];
};

struct D2TileLibraryHashNodeStrc
{
int32_t nStyle;
int32_t nSequence;
int32_t nType;
D2TileLibraryHashRefStrc *pRef;
D2TileLibraryHashNodeStrc *pPrev;
};

struct D2TileLibraryHashRefStrc
{
D2TileLibraryEntryStrc *pTile;
D2TileLibraryHashRefStrc *pPrev;
};

struct D2TileLibraryHashStrc
{
D2TileLibraryHashNodeStrc *pNodes[128];
};

struct D2TileRecordStrc
{
char szLibraryName[260];
void *pLibrary;
D2TileLibraryHashStrc *pHashBlock;
D2TileRecordStrc *pPrev;
};

union D2TxtLinkCodeUnion
{
char szCode[4];
uint32_t dwCode;
};

struct D2TxtLinkNodeStrc
{
char szText[32];
int32_t nLinkIndex;
D2TxtLinkNodeStrc *pPrevious;
D2TxtLinkNodeStrc *pNext;
};

struct D2TxtLinkStrc
{
int32_t nRecords;
int32_t nAllocatedCells;
D2TxtLinkTblStrc *pTbl;
D2TxtLinkNodeStrc *pFirstNode;
};

struct D2TxtLinkTblStrc
{
union
{
char szCode[4];
uint32_t dwCode;
};
int32_t nLinkIndex;
};

struct D2UiButtonImageStrc
{
uint16_t wFrame;
uint16_t wCaptionStrId;
};

struct D2UiButtonImplVtbl
{
D2ClientButtonStrc *(__fastcall *pfnDeletingDtor)(D2ClientButtonStrc *pButton, int32_t nEdxUnused, uint8_t bFreeObject);
void (__fastcall *pfnDraw)(D2ClientButtonStrc *pButton);
int32_t (__stdcall *pfnOnMouseWheel)(int32_t a1);
int32_t (__fastcall *pfnOnMouseDown)(D2ClientButtonStrc *pButton, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnMouseUp)(D2ClientButtonStrc *pButton, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__stdcall *pfnOnMouseMove)(int32_t a1);
int32_t (__fastcall *pfnOnChar)(D2ClientButtonStrc *pButton, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__stdcall *pfnOnArrowUp)(int32_t a1);
int32_t (__stdcall *pfnOnArrowDown)(int32_t a1);
int32_t (__stdcall *pfnOnArrowRight)(int32_t a1);
int32_t (__stdcall *pfnOnArrowLeft)(int32_t a1);
int32_t (__cdecl *pfnGetValue)();
int32_t (__fastcall *pfnSetFlag24)(void *pControl, int32_t nEdxUnused, int32_t bFlag);
};

struct D2UiButtonProxyVtbl
{
uint32_t **(__fastcall *pfnDeletingDtor)(uint32_t **pProxy, int32_t nEdxUnused, uint8_t bFreeObject);
int32_t (__fastcall *pfnDraw)(uint32_t **pProxy);
int32_t (__fastcall *pfnOnMouseWheel)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnMouseDown)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnMouseUp)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnMouseMove)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnChar)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnArrowUp)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnArrowDown)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnArrowRight)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnArrowLeft)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnGetValue)(uint32_t **pProxy);
int32_t (__fastcall *pfnSetFlag24)(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg);
};

struct D2UiControlBaseVtbl
{
int32_t (__cdecl *pfnDeletingDtor)();
int32_t (__cdecl *pfnDraw)();
int32_t (__cdecl *pfnOnMouseWheel)();
int32_t (__cdecl *pfnOnMouseDown)();
int32_t (__cdecl *pfnOnMouseUp)();
int32_t (__cdecl *pfnOnMouseMove)();
int32_t (__cdecl *pfnOnChar)();
int32_t (__cdecl *pfnOnArrowUp)();
int32_t (__cdecl *pfnOnArrowDown)();
int32_t (__cdecl *pfnOnArrowRight)();
int32_t (__cdecl *pfnOnArrowLeft)();
int32_t (__cdecl *pfnGetValue)();
};

struct D2UiControlProxyVtbl
{
uint32_t **(__fastcall *pfnDeletingDtor)(uint32_t **pProxy, int32_t nEdxUnused, uint8_t bFreeObject);
int32_t (__fastcall *pfnDraw)(uint32_t **pProxy);
int32_t (__fastcall *pfnOnMouseWheel)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnMouseDown)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnMouseUp)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnMouseMove)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnChar)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnArrowUp)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnArrowDown)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnArrowRight)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnArrowLeft)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnGetValue)(uint32_t **pProxy);
int32_t (__fastcall *pfnSlot13)(uint32_t **pProxy, int32_t nEdxUnused, int32_t);
int32_t (__fastcall *pfnSetTextFromAscii)(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg);
int32_t (__fastcall *pfnSetTextFromInt)(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg);
int32_t (__fastcall *pfnGetTextUtf8)(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg);
int32_t (__fastcall *pfnSetWidth)(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg);
int32_t (__fastcall *pfnSetFlag24)(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg);
int32_t (__fastcall *pfnSetNumericOnly)(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg);
};

struct D2UiEditBoxImplVtbl
{
D2ClientEditBoxStrc *(__fastcall *pfnDeletingDtor)(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, uint8_t bFreeObject);
void (__fastcall *pfnDraw)(D2ClientEditBoxStrc *pEditBox);
int32_t (__stdcall *pfnOnMouseWheel)(int32_t a1);
int32_t (__fastcall *pfnOnMouseDown)(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnMouseUp)(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__stdcall *pfnOnMouseMove)(int32_t a1);
int32_t (__fastcall *pfnOnChar)(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__stdcall *pfnOnArrowUp)(int32_t a1);
int32_t (__stdcall *pfnOnArrowDown)(int32_t a1);
int32_t (__fastcall *pfnOnArrowRight)(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnArrowLeft)(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnGetValue)(D2ClientEditBoxStrc *pEditBox);
char *(__fastcall *pfnGetTextUtf8)(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, char *);
int32_t (__cdecl *pfnSlot13)();
int32_t (__fastcall *pfnSetTextFromAscii)(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, const char *szText);
int32_t (__fastcall *pfnSetTextFromInt)(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, int32_t nValue);
int32_t (__fastcall *pfnSetWidth)(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, int32_t nWidth);
int32_t (__fastcall *pfnSetFlag24)(void *pControl, int32_t nEdxUnused, int32_t bFlag);
int32_t (__fastcall *pfnSetNumericOnly)(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, int32_t bNumericOnly);
};

struct D2UiSpinnerImplVtbl
{
D2ClientSpinnerStrc *(__fastcall *pfnDeletingDtor)(D2ClientSpinnerStrc *pSpinner, int32_t nEdxUnused, uint8_t bFreeObject);
void (__fastcall *pfnDraw)(D2ClientSpinnerStrc *pSpinner);
int32_t (__fastcall *pfnOnMouseWheel)(D2ClientSpinnerStrc *pSpinner, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnMouseDown)(D2ClientSpinnerStrc *pSpinner, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnMouseUp)(D2ClientSpinnerStrc *pSpinner, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__stdcall *pfnOnMouseMove)(int32_t a1);
int32_t (__stdcall *pfnOnChar)(int32_t a1);
int32_t (__stdcall *pfnOnArrowUp)(int32_t a1);
int32_t (__stdcall *pfnOnArrowDown)(int32_t a1);
int32_t (__stdcall *pfnOnArrowRight)(int32_t a1);
int32_t (__stdcall *pfnOnArrowLeft)(int32_t a1);
int32_t (__fastcall *pfnGetValue)(D2ClientSpinnerStrc *pSpinner);
int32_t (__fastcall *pfnTickAutoRepeat)(D2ClientSpinnerStrc *pSpinner);
};

struct D2UiSpinnerProxyVtbl
{
uint32_t **(__fastcall *pfnDeletingDtor)(uint32_t **pProxy, int32_t nEdxUnused, uint8_t bFreeObject);
int32_t (__fastcall *pfnDraw)(uint32_t **pProxy);
int32_t (__fastcall *pfnOnMouseWheel)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnMouseDown)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnMouseUp)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnMouseMove)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnChar)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnArrowUp)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnArrowDown)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnArrowRight)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnOnArrowLeft)(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t (__fastcall *pfnGetValue)(uint32_t **pProxy);
int32_t (__fastcall *pfnTickAutoRepeat)(uint32_t **pProxy, int32_t nEdxUnused, int32_t);
};

struct D2UniqueAppellationTxt
{
uint16_t wStringId;
};

struct D2UniqueItemsTxt
{
uint16_t wId;
char szName[32];
uint16_t wTblIndex;
uint16_t wVersion;
uint16_t pad0x26;
uint32_t dwBaseItemCode;
uint32_t dwUniqueItemFlags;
uint16_t wRarity;
uint16_t pad0x32;
uint16_t wLvl;
uint16_t wLvlReq;
int8_t nChrTransform;
int8_t nInvTransform;
char szFlippyFile[32];
char szInvFile[32];
uint16_t pad0x7A;
uint32_t dwCostMult;
uint32_t dwCostAdd;
uint16_t wDropSound;
uint16_t wUseSound;
uint8_t nDropSfxFrame;
uint8_t pad0x89[3];
D2PropertyStrc pProperties[12];
};

struct D2UniquePrefixTxt
{
uint16_t wStringId;
};

struct D2UniqueSuffixTxt
{
uint16_t wStringId;
};

struct D2UniqueTitleTxt
{
uint16_t wStringId;
};

struct D2UnitClientStrc
{
D2ClientLightStrc *pLight;
uint32_t dwStartLight;
int32_t nPaletteIndex;
uint8_t bUnitSfx;
uint8_t _gap0[3];
int32_t dwSfxMode;
D2SoundListNodeStrc *pUnitSfxData;
uint32_t dwSfxTicks;
uint32_t dwSfxAsyncTicks;
uint32_t dwSfxStepTicks;
int32_t bHasActiveSound;
uint16_t nLastClickX;
uint16_t nLastClickY;
struct D2UnitEventStrc *pCltTimerList;
};

union D2UnitDataUnion
{
D2PlayerDataStrc *pPlayerData;
D2ItemDataStrc *pItemData;
D2MonsterDataStrc *pMonsterData;
D2ObjectDataStrc *pObjectData;
D2MissileDataStrc *pMissileData;
};

typedef int32_t (__fastcall *D2UnitEventCallbackFunction)(struct D2GameStrc *, int32_t, D2UnitStrc *, D2UnitStrc *, struct D2DamageStrc *, int32_t, int32_t);

struct D2UnitEventStrc
{
D2C_UnitEventTypes nUnitEvent;
uint8_t unk0x01;
uint16_t nFlags;
uint32_t nQueueNo;
int32_t unk0x08;
int32_t nGUID1;
int32_t nGUID2;
D2UnitEventCallbackFunction pCallback;
D2UnitEventStrc *pPrevious;
D2UnitEventStrc *pNext;
};

struct D2UnitFindArgStrc
{
int32_t nFlags;
int32_t unk0x04;
D2UnitStrc *pUnit;
int32_t nX;
int32_t nY;
int32_t nSize;
int32_t nMaxArrayEntries;
int32_t nIndex;
D2FieldStrc *pField;
int32_t (__fastcall *pfnUnitTest)(D2UnitStrc *, D2UnitFindArgStrc *);
int32_t unk0x28;
int32_t unk0x2C;
int32_t unk0x30;
int32_t unk0x34;
};

struct D2UnitFindDataStrc
{
void *pMemPool;
D2UnitStrc **pUnitsArray;
D2ActiveRoomStrc *pRoom;
int32_t (__fastcall *pfnUnitTest)(D2UnitStrc *, D2UnitFindArgStrc *);
D2UnitFindArgStrc *pUnitFindArg;
int32_t nFlags;
int32_t nIndex;
int32_t nMaxArrayEntries;
int32_t nX;
int32_t nY;
int32_t nSize;
};

union D2UnitGfxDataCopyUnion
{
D2GfxDataStrc *pGfxDataCopy;
int32_t nComponentDrawCount;
};

union D2UnitModeUnion
{
uint32_t dwAnimMode;
uint32_t dwItemMode;
uint32_t dwMissileMode;
};

struct __declspec(align(1)) D2UnitPacketListStrc
{
D2UnitPacketListStrc *pNext;
uint8_t nHeader;
};

union D2UnitPathUnion
{
D2DynamicPathStrc *pDynamicPath;
D2StaticPathStrc *pStaticPath;
};

union D2UnitServerClientUnion
{
struct
{
uint32_t dwInteractGUID;
uint32_t dwInteractType;
uint16_t nInteract;
uint16_t nUpdateType;
D2UnitStrc *pUpdateUnit;
struct D2QuestChainStrc *pQuestEventList;
int32_t bSparkChest;
void *pTimerParams;
struct D2GameStrc *pGame;
uint32_t unk0x084[3];
struct D2UnitEventStrc *pSrvUnitEventsList;
};
struct
{
D2ClientLightStrc *pLight;
uint32_t dwStartLight;
int32_t nPaletteIndex;
uint8_t bUnitSfx;
uint8_t _gap0[3];
int32_t dwSfxMode;
D2SoundListNodeStrc *pUnitSfxData;
uint32_t dwSfxTicks;
uint32_t dwSfxAsyncTicks;
uint32_t dwSfxStepTicks;
int32_t bHasActiveSound;
uint16_t nLastClickX;
uint16_t nLastClickY;
struct D2UnitEventStrc *pCltTimerList;
};
};

struct D2UnitServerStrc
{
uint32_t dwInteractGUID;
uint32_t dwInteractType;
uint16_t nInteract;
uint16_t nUpdateType;
D2UnitStrc *pUpdateUnit;
struct D2QuestChainStrc *pQuestEventList;
int32_t bSparkChest;
void *pTimerParams;
struct D2GameStrc *pGame;
uint32_t unk0x084[3];
struct D2UnitEventStrc *pSrvUnitEventsList;
};

struct __declspec(align(4)) D2UnitStrc
{
D2C_UnitTypes dwUnitType;
int32_t dwClassId;
void *pMemoryPool;
uint32_t dwUnitId;
union
{
uint32_t dwAnimMode;
uint32_t dwItemMode;
uint32_t dwMissileMode;
};
union
{
D2PlayerDataStrc *pPlayerData;
D2ItemDataStrc *pItemData;
D2MonsterDataStrc *pMonsterData;
D2ObjectDataStrc *pObjectData;
D2MissileDataStrc *pMissileData;
};
uint8_t nAct;
uint8_t unk0x19[3];
D2DrlgActStrc *pDrlgAct;
D2SeedStrc pSeed;
uint32_t dwInitSeed;
union
{
D2DynamicPathStrc *pDynamicPath;
D2StaticPathStrc *pStaticPath;
};
D2AnimSeqTxt *pAnimSeq;
uint32_t dwSeqFrameCount;
int32_t dwSeqFrame;
uint32_t dwSeqSpeed;
uint32_t dwSeqMode;
int32_t nSeqCurrentFramePrecise;
int32_t dwFrameCountPrecise;
int16_t wAnimSpeed;
uint8_t nActionFrame;
uint8_t unk0x4F;
const D2AnimDataRecordStrc *pAnimData;
D2ClientUnitGfxInfoStrc *pGfxData;
union
{
D2GfxDataStrc *pGfxDataCopy;
int32_t nComponentDrawCount;
};
D2StatListExStrc *pStatListEx;
D2InventoryStrc *pInventory;
union
{
struct
{
uint32_t dwInteractGUID;
uint32_t dwInteractType;
uint16_t nInteract;
uint16_t nUpdateType;
D2UnitStrc *pUpdateUnit;
struct D2QuestChainStrc *pQuestEventList;
int32_t bSparkChest;
void *pTimerParams;
struct D2GameStrc *pGame;
uint32_t unk0x084[3];
struct D2UnitEventStrc *pSrvUnitEventsList;
};
struct
{
D2ClientLightStrc *pLight;
uint32_t dwStartLight;
int32_t nPaletteIndex;
uint8_t bUnitSfx;
uint8_t _gap0[3];
int32_t dwSfxMode;
D2SoundListNodeStrc *pUnitSfxData;
uint32_t dwSfxTicks;
uint32_t dwSfxAsyncTicks;
uint32_t dwSfxStepTicks;
int32_t bHasActiveSound;
uint16_t nLastClickX;
uint16_t nLastClickY;
struct D2UnitEventStrc *pCltTimerList;
};
};
D2C_UnitTypes dwOwnerType;
uint32_t dwOwnerGUID;
uint32_t dwKillerType;
uint32_t dwKillerGUID;
D2HoverTextStrc *pHoverText;
D2SkillListStrc *pSkills;
struct D2CombatStrc *pCombat;
uint32_t dwLastHitClass;
uint32_t unk0xB4;
uint32_t dwDropItemCode;
uint32_t unk0xBC[2];
uint32_t dwFlags;
uint32_t dwFlagEx;
void *pUnused0xCC;
uint32_t dwNodeIndex;
uint32_t dwTickCount;
union
{
uint32_t dwSrvTickCount;
struct D2PacketListStrc *pPacketList;
};
struct D2EventTimerStrc *pTimer;
D2UnitStrc *pChangeNextUnit;
D2UnitStrc *pListNext;
D2UnitStrc *pRoomNext;
D2UnitPacketListStrc *pMsgFirst;
D2UnitPacketListStrc *pMsgLast;
};

union D2UnitTickOrPacketsUnion
{
uint32_t dwSrvTickCount;
struct D2PacketListStrc *pPacketList;
};

union D2UnkMonsterDataLifeUnion
{
int32_t nLifePercentage;
int32_t nMinionClassId;
};

struct D2UnkOutdoorStrc2
{
D2DrlgRoomStrc *pDrlgRoom;
D2DrlgOutdoorRoomStrc *pOutdoorRooms[4];
D2DrlgGridStrc *pWallsGrids[4];
D2DrlgGridStrc *pFloorGrid;
int32_t field_28;
int32_t field_2C;
int32_t nSubWaypoint_Shrine;
int32_t nSubTheme;
int32_t nSubThemePicked;
};

struct D2UnkPlayerDataStrc
{
int32_t nUnitGUID;
int32_t unk0x04;
int32_t unk0x08;
uint8_t unk0x0C;
uint8_t unk0x0D[3];
D2UnkPlayerDataStrc *pNext;
};

struct D2WaypointCoordTableStrc
{
int32_t nX;
int32_t nY;
int32_t nTextX;
int32_t nTextY;
int32_t nClickX;
int32_t nClickY;
};

struct D2WaypointDataStrc
{
uint16_t nFlags[8];
};

struct D2WaypointTableStrc
{
int32_t nLevelId;
uint8_t bActivated;
};

struct D2WeaponDescStrc
{
int32_t nKey;
uint16_t nStringId;
};

struct D2WinCommandRegisterStrc
{
D2WindowProcCallbackType nKind;
uint32_t nMessageOrKey;
void (__stdcall *pfnHandler)(struct SMSGHANDLER_PARAMS *);
};

struct D2WindowProcCallbackStrc
{
enum D2WindowProcCallbackType nType;
uint32_t uMessage;
int32_t (__stdcall *pfCallback)(struct SMSGHANDLER_PARAMS *);
};

union FOGASTNodeStrc
{
FOGASTType nType;
uint8_t nRawValue;
};

typedef struct HD2ARCHIVE__ *HD2ARCHIVE;

struct HotkeyMappingEntry
{
D2HotkeyIndices eCmd;
D2VirtualKeys VKCode;
BOOL bPrimaryHotkey;
};

struct KeyFunctions
{
void (__fastcall *FKeyDown)();
void (__fastcall *FKeyLift)();
int32_t bUnk0x8;
};

struct QUESTLOGBUTTONPOSITIONT
{
int32_t nDrawX;
int32_t nDrawY;
int32_t nHitX;
int32_t nHitY;
};

typedef int32_t (__stdcall *ROOMCALLBACKFN)(D2ActiveRoomStrc *, void *);

struct SCmdArg
{
uint32_t dword0;
uint32_t dword4;
char *argString;
int32_t (__stdcall *callback)(void *pParam, int32_t nUnused);
};

struct SGameDataChunkStrc
{
uint32_t nCapacity;
uint32_t nCount;
char *pRecords;
uint32_t nGrowQuantum;
struct TSLink tLink;
};

struct SGameDataNodeStrc
{
int32_t nUnk00;
struct TSLink tBucketLink;
struct TSLink tNodeLink;
};

struct TSExplicitList
{
int32_t nLinkOffset;
struct TSLink tSentinel;
};

struct TSExplicitListArray
{
uint32_t nCapacity;
uint32_t nCount;
struct TSExplicitList *pLists;
uint32_t nGrowQuantum;
};

struct SGameDataTableStrc
{
const SGameDataTableVtbl *pVtbl;
struct TSExplicitList tNodeList;
int32_t nFullnessIndicator;
struct TSExplicitListArray tBuckets;
uint32_t nBucketMask;
struct TSExplicitList tReuseList;
int32_t nChunkSize;
struct TSExplicitList tChunkList;
int32_t unk0x44;
uint32_t dwCookie;
int32_t unk0x4C;
CRITICAL_SECTION tLock;
};

struct SGameDataTableVtbl
{
void (__fastcall *pfnInternalDelete)(struct SGameDataTableStrc *pTable, int32_t nEdxUnused, SGameDataNodeStrc *pNode);
SGameDataNodeStrc *(__fastcall *pfnInternalNew)(struct SGameDataTableStrc *pTable, int32_t nEdxUnused, struct TSExplicitList *pList, int32_t nExtraBytes, int32_t nAllocFlags);
struct SGameDataTableStrc *(__fastcall *pfnDeletingDtor)(struct SGameDataTableStrc *pTable, int32_t nEdxUnused, uint8_t bDelete);
void (__fastcall *pfnDestroy)(struct SGameDataTableStrc *pTable);
};

typedef void (__stdcall *SMSGHANDLER)(struct SMSGHANDLER_PARAMS *);

struct SMSGHANDLER_PARAMS
{
HWND hWnd;
uint32_t uMessage;
union
{
WPARAM wParam;
int32_t nKey;
};
union
{
struct
{
uint16_t nXpos;
uint16_t nYpos;
};
LPARAM lParam;
};
uint32_t dwCommandSource;
uint32_t dwArg;
BOOL bReturn;
LRESULT lResult;
};

typedef void (__fastcall *StatListRemoveCallback)(D2UnitStrc *, int32_t, D2StatListStrc *);

typedef void (__fastcall *StatListValueChangeFunc)(struct D2GameStrc *, D2UnitStrc *, D2UnitStrc *, int32_t, int32_t, int32_t);

typedef void (__stdcall *TOWNAUTOMAPFN)(int32_t, int32_t, int32_t, int32_t);

typedef int32_t (__fastcall *UNITFINDTEST)(D2UnitStrc *, D2UnitFindArgStrc *);

struct ptMsgStruct
{
void (__fastcall *pfnHandler)(uint8_t *pPacket);
int32_t nCmdSize;
int32_t bCopyToUnitBuffer;
};

#pragma pack(pop)

/* ---- NOT USED BY OUR CODE -- commented out ------------------------------
 * The user, 2026-10-14: every type no code of ours reaches is written here
 * commented out, by name. The census is gen_structs_header.py's (GS_SRC):
 * the code tokens of D2Client's .cpp and .h files -- not this file, not
 * D2Constants.h -- and the closure through each reached definition's body.
 * A type our code starts to use comes back above at the next regeneration;
 * nothing here is edited by hand. Each keeps its size assertion, commented.
 *------------------------------------------------------------------------*/
// struct ClientPacketSaveHeaderPart
// {
// uint8_t nHeaderId;
// uint8_t nPartDataSize;
// uint8_t bFirstPart;
// uint32_t nSaveHeaderSize;
// uint8_t aData[256];
// };
// static_assert(sizeof(ClientPacketSaveHeaderPart) == 263, "ClientPacketSaveHeaderPart: layout differs from the database");

// struct D2AltDrawStrc
// {
// int32_t nLeft;
// int32_t nTop;
// int32_t nRight;
// int32_t nBottom;
// D2UnitStrc *pItem;
// wchar_t wszName[128];
// int32_t nRectColor;
// int32_t nRectDrawMode;
// int32_t nColor;
// };
// static_assert(sizeof(D2AltDrawStrc) == 288, "D2AltDrawStrc: layout differs from the database");

// struct D2AnimUnkStrc
// {
// int32_t bCanUseVelocityModifierByPassiveSkill;
// int32_t bCanUseVelocityModifier;
// int32_t bCanUseSkillAttackRate;
// int32_t bCanUseAttackRate;
// int32_t bAllowAnimSpeed;
// };
// static_assert(sizeof(D2AnimUnkStrc) == 20, "D2AnimUnkStrc: layout differs from the database");

// struct D2AnvilUIButtonStrc
// {
// uint16_t nStringId;
// uint8_t _gap0[2];
// int32_t nTextX;
// int32_t nTextY;
// int32_t nCellfileX;
// int32_t nCellfileY;
// int32_t nClickAreaXStart;
// int32_t nClickAreaXEnd;
// int32_t nClickAreaYStart;
// int32_t nClickAreaYEnd;
// int32_t nFrame;
// uint8_t bButtonClicked;
// uint8_t unk0x27;
// uint8_t _gap1[2];
// };
// static_assert(sizeof(D2AnvilUIButtonStrc) == 44, "D2AnvilUIButtonStrc: layout differs from the database");

// struct D2ArchiveHandleStrc
// {
// HSARCHIVE__ *hArchive;
// char szPath[260];
// };
// static_assert(sizeof(D2ArchiveHandleStrc) == 264, "D2ArchiveHandleStrc: layout differs from the database");

// struct D2ArenaTxt
// {
// union
// {
// struct
// {
// int32_t dwSuicide;
// int32_t dwPlayerKill;
// int32_t dwPlayerKillPercent;
// int32_t dwMonsterKill;
// int32_t dwPlayerDeath;
// int32_t dwPlayerDeathPercent;
// int32_t dwMonsterDeath;
// };
// int32_t dwScores[7];
// };
// };
// static_assert(sizeof(D2ArenaTxt) == 28, "D2ArenaTxt: layout differs from the database");

// struct D2AutomapTxt
// {
// char szLevelName[16];
// char szTileName[8];
// uint8_t nStyle;
// uint8_t nStartSequence;
// uint8_t nEndSequence;
// uint8_t pad0x1B;
// int32_t nCel[4];
// };
// static_assert(sizeof(D2AutomapTxt) == 44, "D2AutomapTxt: layout differs from the database");

// union D2C_PackedTileInformation
// {
// uint32_t nPackedValue;
// struct
// {
// unsigned __int32 bIsWall : 1;
// unsigned __int32 bIsFloor : 1;
// unsigned __int32 bLOS : 1;
// unsigned __int32 bEnclosed : 1;
// unsigned __int32 bExit : 1;
// unsigned __int32 bUnk0x20 : 1;
// unsigned __int32 bLayerBelow : 1;
// unsigned __int32 bLayerAbove : 1;
// unsigned __int32 nTileSequence : 8;
// unsigned __int32 bFillLOS : 1;
// unsigned __int32 bUnwalkable : 1;
// unsigned __int32 nWallLayer : 2;
// unsigned __int32 nTileStyle : 6;
// unsigned __int32 bRevealHidden : 1;
// unsigned __int32 bShadow : 1;
// unsigned __int32 bLinkage : 1;
// unsigned __int32 bObjectWall : 1;
// unsigned __int32 bUnk0x40000000 : 1;
// unsigned __int32 bHidden : 1;
// };
// };
// static_assert(sizeof(D2C_PackedTileInformation) == 4, "D2C_PackedTileInformation: layout differs from the database");

// struct D2CalcCallbackInfoStrc
// {
// int32_t (__fastcall *fpCallBack)(int32_t, int32_t, int32_t, void *);
// int32_t nParameters;
// };
// static_assert(sizeof(D2CalcCallbackInfoStrc) == 8, "D2CalcCallbackInfoStrc: layout differs from the database");

// struct D2CharTemplateTxt
// {
// char szName[30];
// uint8_t nClass;
// uint8_t nLevel;
// uint8_t nAct;
// uint8_t nStr;
// uint8_t nDex;
// uint8_t nInt;
// uint8_t nVit;
// uint8_t nMana;
// uint8_t nHitpoints;
// uint8_t nManaRegenBonus;
// uint8_t nVelocity;
// uint8_t nAttackRate;
// uint8_t nOtherRate;
// uint8_t pad0x2B;
// uint32_t dwRightSkill;
// uint32_t dwSkill[9];
// uint32_t dwSkillLevel[9];
// D2CharItemStrc pItems[15];
// };
// static_assert(sizeof(D2CharTemplateTxt) == 240, "D2CharTemplateTxt: layout differs from the database");

// struct D2ClassicStatAdjustmentStrc
// {
// D2C_ItemStats nStatID;
// int32_t nNightmareMultiplier;
// int32_t nNightmareDivisor;
// int32_t nHellMultiplier;
// int32_t nHellDivisor;
// };
// static_assert(sizeof(D2ClassicStatAdjustmentStrc) == 20, "D2ClassicStatAdjustmentStrc: layout differs from the database");

// struct D2CompositStrc
// {
// int32_t nWeaponClassCode;
// int32_t nWeaponClassId;
// };
// static_assert(sizeof(D2CompositStrc) == 8, "D2CompositStrc: layout differs from the database");

// struct D2CrtSbhNewEntryStrc
// {
// int32_t nSizeFront;
// struct D2CrtSbhNewEntryStrc *pEntryNext;
// struct D2CrtSbhNewEntryStrc *pEntryPrev;
// };
// static_assert(sizeof(D2CrtSbhNewEntryStrc) == 12, "D2CrtSbhNewEntryStrc: layout differs from the database");

// struct D2CrtSbhNewGroupStrc
// {
// int32_t nEntriesInUse;
// struct D2CrtSbhNewListHeadStrc listHead[64];
// };
// static_assert(sizeof(D2CrtSbhNewGroupStrc) == 516, "D2CrtSbhNewGroupStrc: layout differs from the database");

// struct D2CrtSbhNewHeaderStrc
// {
// uint32_t bitvEntryHi;
// uint32_t bitvEntryLo;
// uint32_t bitvCommit;
// uint8_t *pHeapData;
// struct D2CrtSbhNewRegionStrc *pRegion;
// };
// static_assert(sizeof(D2CrtSbhNewHeaderStrc) == 20, "D2CrtSbhNewHeaderStrc: layout differs from the database");

// struct D2CrtSbhNewListHeadStrc
// {
// struct D2CrtSbhNewEntryStrc *pEntryNext;
// struct D2CrtSbhNewEntryStrc *pEntryPrev;
// };
// static_assert(sizeof(D2CrtSbhNewListHeadStrc) == 8, "D2CrtSbhNewListHeadStrc: layout differs from the database");

// struct D2CrtSbhNewRegionStrc
// {
// int32_t nGroupInUse;
// uint8_t aClassGroupCount[64];
// uint32_t aGroupBitvHi[32];
// uint32_t aGroupBitvLo[32];
// struct D2CrtSbhNewGroupStrc aGroups[32];
// };
// static_assert(sizeof(D2CrtSbhNewRegionStrc) == 16836, "D2CrtSbhNewRegionStrc: layout differs from the database");

// struct D2CrtSbhPageEntryStrc
// {
// int32_t nFreeUnits;
// int32_t nLongestFreeRun;
// };
// static_assert(sizeof(D2CrtSbhPageEntryStrc) == 8, "D2CrtSbhPageEntryStrc: layout differs from the database");

// struct D2CrtSbhPageStrc
// {
// uint8_t *pCursor;
// uint32_t nRunAtCursor;
// uint8_t aUnits[240];
// uint8_t nEndSentinel;
// uint8_t aPadding[7];
// uint8_t aBlockData[3840];
// };
// static_assert(sizeof(D2CrtSbhPageStrc) == 4096, "D2CrtSbhPageStrc: layout differs from the database");

// struct D2CrtSbhRegionStrc
// {
// struct D2CrtSbhRegionStrc *pNext;
// struct D2CrtSbhRegionStrc *pPrev;
// struct D2CrtSbhPageEntryStrc *pEntries;
// struct D2CrtSbhPageEntryStrc *pFirstDecommitted;
// uint8_t *pReserveBase;
// uint8_t *pReserveEnd;
// struct D2CrtSbhPageEntryStrc aPageEntries[1024];
// int32_t dwUnreadTail[2];
// };
// static_assert(sizeof(D2CrtSbhRegionStrc) == 8224, "D2CrtSbhRegionStrc: layout differs from the database");

// struct D2DS1FileStrc
// {
// int32_t nVersion;
// int32_t nWidth;
// int32_t nHeight;
// int32_t pData[1];
// };
// static_assert(sizeof(D2DS1FileStrc) == 16, "D2DS1FileStrc: layout differs from the database");

// struct __declspec(align(4)) D2DrlgLevelLinkDataStrc
// {
// D2SeedStrc pSeed;
// D2DrlgCoordStrc pLevelCoord[15];
// D2DrlgLinkStrc *pLink;
// union
// {
// int32_t nRand[4][15];
// int32_t nRand2[60];
// };
// int32_t nIteration;
// int32_t nCurrentLevel;
// };
// static_assert(sizeof(D2DrlgLevelLinkDataStrc) == 500, "D2DrlgLevelLinkDataStrc: layout differs from the database");

// struct D2DrlgLinkStrc
// {
// void *pfLinker;
// int32_t nLevel;
// int32_t nLevelLink;
// int32_t nLevelLinkEx;
// };
// static_assert(sizeof(D2DrlgLinkStrc) == 16, "D2DrlgLinkStrc: layout differs from the database");

// struct D2DrlgOutDesertInitStrc
// {
// int32_t nLvlPrestId;
// int32_t nRand;
// int32_t nX;
// int32_t nY;
// };
// static_assert(sizeof(D2DrlgOutDesertInitStrc) == 16, "D2DrlgOutDesertInitStrc: layout differs from the database");

// struct D2DrlgOutSiegeInitStrc
// {
// int32_t nLevelId;
// int32_t field_4;
// int32_t field_8;
// int32_t nLevelPrestId1;
// int32_t nLevelPrestId2;
// };
// static_assert(sizeof(D2DrlgOutSiegeInitStrc) == 20, "D2DrlgOutSiegeInitStrc: layout differs from the database");

// struct D2DrlgOutSiegeInitStrc2
// {
// int32_t nStyle;
// int32_t field_4;
// int32_t field_8;
// int32_t nLevelPrestId1;
// int32_t nLevelPrestId2;
// };
// static_assert(sizeof(D2DrlgOutSiegeInitStrc2) == 20, "D2DrlgOutSiegeInitStrc2: layout differs from the database");

// struct D2DrlgOutSiegeInitStrc3
// {
// int32_t nLevelId;
// int32_t nLevelPrestId1;
// int32_t nLevelPrestId2;
// int32_t field_C;
// int32_t field_10;
// int32_t field_14;
// int32_t bMustHave;
// };
// static_assert(sizeof(D2DrlgOutSiegeInitStrc3) == 28, "D2DrlgOutSiegeInitStrc3: layout differs from the database");

// union D2DrlgOutdoorPackedGrid2InfoStrc
// {
// uint32_t nPackedValue;
// struct
// {
// unsigned __int32 nUnkb00 : 1;
// unsigned __int32 bHasDirection : 1;
// unsigned __int32 nUnkb02 : 5;
// unsigned __int32 nUnkb07 : 1;
// unsigned __int32 nUnkb08 : 1;
// unsigned __int32 bHasPickedFile : 1;
// unsigned __int32 bLvlLink : 1;
// unsigned __int32 nUnkb11 : 1;
// unsigned __int32 nUnkb12 : 1;
// unsigned __int32 nUnkb13 : 3;
// unsigned __int32 nPickedFile : 4;
// unsigned __int32 nUnkb20 : 12;
// };
// };
// static_assert(sizeof(D2DrlgOutdoorPackedGrid2InfoStrc) == 4, "D2DrlgOutdoorPackedGrid2InfoStrc: layout differs from the database");

// struct D2EnvironmentCycleStrc
// {
// int32_t nTicksBegin;
// int32_t nPeriodOfDay;
// uint8_t nRed;
// uint8_t nGreen;
// uint8_t nBlue;
// uint8_t nIntensity;
// };
// static_assert(sizeof(D2EnvironmentCycleStrc) == 12, "D2EnvironmentCycleStrc: layout differs from the database");

// struct D2GSPacketClt01
// {
// uint8_t nHeader;
// uint16_t nPosX;
// uint16_t nPosY;
// };
// static_assert(sizeof(D2GSPacketClt01) == 5, "D2GSPacketClt01: layout differs from the database");

// struct D2GSPacketClt02
// {
// uint8_t nHeader;
// uint32_t dwUnitType;
// uint32_t dwUnitGUID;
// };
// static_assert(sizeof(D2GSPacketClt02) == 9, "D2GSPacketClt02: layout differs from the database");

// struct D2GSPacketClt03
// {
// uint8_t nHeader;
// uint16_t nPosX;
// uint16_t nPosY;
// };
// static_assert(sizeof(D2GSPacketClt03) == 5, "D2GSPacketClt03: layout differs from the database");

// struct D2GSPacketClt04
// {
// uint8_t nHeader;
// uint32_t dwUnitType;
// uint32_t dwUnitGUID;
// };
// static_assert(sizeof(D2GSPacketClt04) == 9, "D2GSPacketClt04: layout differs from the database");

// struct D2GSPacketClt05
// {
// uint8_t nHeader;
// uint16_t nPosX;
// uint16_t nPosY;
// };
// static_assert(sizeof(D2GSPacketClt05) == 5, "D2GSPacketClt05: layout differs from the database");

// struct D2GSPacketClt06
// {
// uint8_t nHeader;
// uint32_t dwUnitType;
// uint32_t dwUnitGUID;
// };
// static_assert(sizeof(D2GSPacketClt06) == 9, "D2GSPacketClt06: layout differs from the database");

// struct D2GSPacketClt07
// {
// uint8_t nHeader;
// uint32_t dwUnitType;
// uint32_t dwUnitGUID;
// };
// static_assert(sizeof(D2GSPacketClt07) == 9, "D2GSPacketClt07: layout differs from the database");

// struct D2GSPacketClt08
// {
// uint8_t nHeader;
// uint16_t nPosX;
// uint16_t nPosY;
// };
// static_assert(sizeof(D2GSPacketClt08) == 5, "D2GSPacketClt08: layout differs from the database");

// struct D2GSPacketClt09
// {
// uint8_t nHeader;
// uint32_t dwUnitType;
// uint32_t dwUnitGUID;
// };
// static_assert(sizeof(D2GSPacketClt09) == 9, "D2GSPacketClt09: layout differs from the database");

// struct D2GSPacketClt0A
// {
// uint8_t nHeader;
// uint32_t dwUnitType;
// uint32_t dwUnitGUID;
// };
// static_assert(sizeof(D2GSPacketClt0A) == 9, "D2GSPacketClt0A: layout differs from the database");

// struct D2GSPacketClt0B
// {
// uint8_t nHeader;
// };
// static_assert(sizeof(D2GSPacketClt0B) == 1, "D2GSPacketClt0B: layout differs from the database");

// struct D2GSPacketClt0C
// {
// uint8_t nHeader;
// uint16_t nPosX;
// uint16_t nPosY;
// };
// static_assert(sizeof(D2GSPacketClt0C) == 5, "D2GSPacketClt0C: layout differs from the database");

// struct D2GSPacketClt0D
// {
// uint8_t nHeader;
// uint32_t dwUnitType;
// uint32_t dwUnitGUID;
// };
// static_assert(sizeof(D2GSPacketClt0D) == 9, "D2GSPacketClt0D: layout differs from the database");

// struct D2GSPacketClt0E
// {
// uint8_t nHeader;
// uint32_t dwUnitType;
// uint32_t dwUnitGUID;
// };
// static_assert(sizeof(D2GSPacketClt0E) == 9, "D2GSPacketClt0E: layout differs from the database");

// struct D2GSPacketClt0F
// {
// uint8_t nHeader;
// uint16_t nPosX;
// uint16_t nPosY;
// };
// static_assert(sizeof(D2GSPacketClt0F) == 5, "D2GSPacketClt0F: layout differs from the database");

// struct D2GSPacketClt10
// {
// uint8_t nHeader;
// uint32_t dwUnitType;
// uint32_t dwUnitGUID;
// };
// static_assert(sizeof(D2GSPacketClt10) == 9, "D2GSPacketClt10: layout differs from the database");

// struct D2GSPacketClt11
// {
// uint8_t nHeader;
// uint32_t dwUnitType;
// uint32_t dwUnitGUID;
// };
// static_assert(sizeof(D2GSPacketClt11) == 9, "D2GSPacketClt11: layout differs from the database");

// struct D2GSPacketClt12
// {
// uint8_t nHeader;
// };
// static_assert(sizeof(D2GSPacketClt12) == 1, "D2GSPacketClt12: layout differs from the database");

// struct D2GSPacketClt13
// {
// uint8_t nHeader;
// uint32_t dwUnitType;
// uint32_t dwUnitGUID;
// };
// static_assert(sizeof(D2GSPacketClt13) == 9, "D2GSPacketClt13: layout differs from the database");

// struct D2GSPacketClt14
// {
// uint8_t nHeader;
// uint8_t nMessageType;
// uint8_t nLang;
// char szMessage[256];
// };
// static_assert(sizeof(D2GSPacketClt14) == 259, "D2GSPacketClt14: layout differs from the database");

// struct D2GSPacketClt16
// {
// uint8_t nHeader;
// uint32_t dwUnitType;
// uint32_t dwUnitGUID;
// uint32_t bCursor;
// };
// static_assert(sizeof(D2GSPacketClt16) == 13, "D2GSPacketClt16: layout differs from the database");

// struct D2GSPacketClt17
// {
// uint8_t nHeader;
// uint32_t dwItemGUID;
// };
// static_assert(sizeof(D2GSPacketClt17) == 5, "D2GSPacketClt17: layout differs from the database");

// struct D2GSPacketClt18
// {
// uint8_t nHeader;
// uint32_t dwItemGUID;
// uint32_t dwPosX;
// uint32_t dwPosY;
// uint32_t dwInvPage;
// };
// static_assert(sizeof(D2GSPacketClt18) == 17, "D2GSPacketClt18: layout differs from the database");

// struct D2GSPacketClt19
// {
// uint8_t nHeader;
// uint32_t dwItemGUID;
// };
// static_assert(sizeof(D2GSPacketClt19) == 5, "D2GSPacketClt19: layout differs from the database");

// struct D2GSPacketClt20
// {
// uint8_t nHeader;
// int32_t nItemGUID;
// int32_t nPosX;
// int32_t nPosY;
// };
// static_assert(sizeof(D2GSPacketClt20) == 13, "D2GSPacketClt20: layout differs from the database");

// struct D2GSPacketClt23
// {
// uint8_t nHeader;
// int32_t nCursorItemGUID;
// int32_t nBeltSlotId;
// };
// static_assert(sizeof(D2GSPacketClt23) == 9, "D2GSPacketClt23: layout differs from the database");

// struct D2GSPacketClt26
// {
// uint8_t nHeader;
// int32_t nItemGUID;
// uint32_t bUseOnMerc;
// int32_t unk0x09;
// };
// static_assert(sizeof(D2GSPacketClt26) == 13, "D2GSPacketClt26: layout differs from the database");

// struct D2GSPacketClt27
// {
// uint8_t nHeader;
// int32_t nTargetItemGUID;
// int32_t nUseItemGUID;
// };
// static_assert(sizeof(D2GSPacketClt27) == 9, "D2GSPacketClt27: layout differs from the database");

// struct D2GSPacketClt29
// {
// uint8_t nHeader;
// int32_t nScrollGUID;
// int32_t nBookGUID;
// };
// static_assert(sizeof(D2GSPacketClt29) == 9, "D2GSPacketClt29: layout differs from the database");

// struct D2GSPacketClt2A
// {
// uint8_t nHeader;
// uint32_t dwItemGUID;
// uint32_t dwCubeGUID;
// };
// static_assert(sizeof(D2GSPacketClt2A) == 9, "D2GSPacketClt2A: layout differs from the database");

// struct D2GSPacketClt2B
// {
// uint8_t nHeader;
// int32_t nUnitGUID;
// uint8_t nOptionValues[39];
// };
// static_assert(sizeof(D2GSPacketClt2B) == 44, "D2GSPacketClt2B: layout differs from the database");

// struct D2GSPacketClt31
// {
// uint8_t nHeader;
// int32_t nNpcGUID;
// int32_t nMessageId;
// };
// static_assert(sizeof(D2GSPacketClt31) == 9, "D2GSPacketClt31: layout differs from the database");

// struct D2GSPacketClt32
// {
// uint8_t nHeader;
// int32_t dwNpcGUID;
// int32_t dwItemGUID;
// uint16_t nTransactionType;
// struct
// {
// unsigned __int16 nItemMode : 15;
// unsigned __int16 bMultibuy : 1;
// };
// int32_t dwCost;
// };
// static_assert(sizeof(D2GSPacketClt32) == 17, "D2GSPacketClt32: layout differs from the database");

// struct D2GSPacketClt33
// {
// uint8_t nHeader;
// uint32_t dwMonGUID;
// uint32_t dwItemGUID;
// uint16_t nTab;
// uint16_t unk0x0B;
// uint32_t dwGold;
// };
// static_assert(sizeof(D2GSPacketClt33) == 17, "D2GSPacketClt33: layout differs from the database");

// struct D2GSPacketClt38
// {
// uint8_t nHeader;
// uint32_t nAction;
// int32_t nNpcGUID;
// int32_t nItemGUID;
// };
// static_assert(sizeof(D2GSPacketClt38) == 13, "D2GSPacketClt38: layout differs from the database");

// struct D2GSPacketClt3A
// {
// uint8_t nHeader;
// uint8_t nStat;
// uint8_t nPointsLessOne;
// };
// static_assert(sizeof(D2GSPacketClt3A) == 3, "D2GSPacketClt3A: layout differs from the database");

// struct D2GSPacketClt3B
// {
// uint8_t nHeader;
// uint16_t nSkill;
// };
// static_assert(sizeof(D2GSPacketClt3B) == 3, "D2GSPacketClt3B: layout differs from the database");

// struct D2GSPacketClt3C
// {
// uint8_t nHeader;
// uint16_t nSkill;
// uint16_t nMode;
// uint32_t dwFlags;
// };
// static_assert(sizeof(D2GSPacketClt3C) == 9, "D2GSPacketClt3C: layout differs from the database");

// struct D2GSPacketClt3D
// {
// uint8_t nHeader;
// int32_t nDoorGUID;
// };
// static_assert(sizeof(D2GSPacketClt3D) == 5, "D2GSPacketClt3D: layout differs from the database");

// struct D2GSPacketClt41
// {
// uint8_t nHeader;
// };
// static_assert(sizeof(D2GSPacketClt41) == 1, "D2GSPacketClt41: layout differs from the database");

// struct D2GSPacketClt42
// {
// uint8_t nHeader;
// char szVersion[10];
// };
// static_assert(sizeof(D2GSPacketClt42) == 11, "D2GSPacketClt42: layout differs from the database");

// struct D2GSPacketClt44
// {
// uint8_t nHeader;
// int32_t nPlayerGUID;
// int32_t nAnvilInteractGUID;
// int32_t nItemGUID;
// int16_t nState;
// int16_t unk0x0F;
// };
// static_assert(sizeof(D2GSPacketClt44) == 17, "D2GSPacketClt44: layout differs from the database");

// struct D2GSPacketClt45
// {
// uint8_t nHeader;
// int32_t nPortalGUID;
// int32_t nLevelId;
// };
// static_assert(sizeof(D2GSPacketClt45) == 9, "D2GSPacketClt45: layout differs from the database");

// struct D2GSPacketClt49
// {
// uint8_t nHeader;
// int32_t nWaypointGUID;
// int32_t nLevelId;
// };
// static_assert(sizeof(D2GSPacketClt49) == 9, "D2GSPacketClt49: layout differs from the database");

// struct D2GSPacketClt4F
// {
// uint8_t nHeader;
// uint16_t nButtonId;
// union
// {
// struct
// {
// uint16_t nParams[2];
// };
// int32_t nItemGUID;
// };
// };
// static_assert(sizeof(D2GSPacketClt4F) == 7, "D2GSPacketClt4F: layout differs from the database");

// struct D2GSPacketClt51
// {
// uint8_t nHeader;
// int32_t nSkillId;
// int32_t nOwnerGUID;
// };
// static_assert(sizeof(D2GSPacketClt51) == 9, "D2GSPacketClt51: layout differs from the database");

// struct D2GSPacketClt58
// {
// uint8_t nHeader;
// uint16_t nQuest;
// };
// static_assert(sizeof(D2GSPacketClt58) == 3, "D2GSPacketClt58: layout differs from the database");

// struct D2GSPacketClt59
// {
// uint8_t nHeader;
// uint32_t nUnitType;
// uint32_t dwUnitGUID;
// uint32_t dwTargetX;
// uint32_t dwTargetY;
// };
// static_assert(sizeof(D2GSPacketClt59) == 17, "D2GSPacketClt59: layout differs from the database");

// struct D2GSPacketClt5F
// {
// uint8_t nHeader;
// uint16_t nX;
// uint16_t nY;
// };
// static_assert(sizeof(D2GSPacketClt5F) == 5, "D2GSPacketClt5F: layout differs from the database");

// struct D2GSPacketClt63
// {
// uint8_t nHeader;
// uint32_t dwItemGUID;
// };
// static_assert(sizeof(D2GSPacketClt63) == 5, "D2GSPacketClt63: layout differs from the database");

// struct D2GSPacketClt64
// {
// uint8_t nHeader;
// int32_t unk0x01[2];
// };
// static_assert(sizeof(D2GSPacketClt64) == 9, "D2GSPacketClt64: layout differs from the database");

// struct D2GSPacketClt65
// {
// uint8_t nHeader;
// int32_t unk0x01[4];
// };
// static_assert(sizeof(D2GSPacketClt65) == 17, "D2GSPacketClt65: layout differs from the database");

// struct D2GSPacketClt69
// {
// uint8_t nHeader;
// };
// static_assert(sizeof(D2GSPacketClt69) == 1, "D2GSPacketClt69: layout differs from the database");

// struct D2GSPacketClt6B
// {
// uint8_t nHeader;
// uint8_t nSize;
// uint32_t nTotalSize;
// uint8_t saveData[8192];
// };
// static_assert(sizeof(D2GSPacketClt6B) == 8198, "D2GSPacketClt6B: layout differs from the database");

// struct D2GSPacketClt6D
// {
// uint8_t nHeader;
// };
// static_assert(sizeof(D2GSPacketClt6D) == 1, "D2GSPacketClt6D: layout differs from the database");

// struct D2GSPacketClt6F
// {
// uint8_t nHeader;
// };
// static_assert(sizeof(D2GSPacketClt6F) == 1, "D2GSPacketClt6F: layout differs from the database");

// struct D2GSPacketSrv00
// {
// uint8_t nHeader;
// };
// static_assert(sizeof(D2GSPacketSrv00) == 1, "D2GSPacketSrv00: layout differs from the database");

// struct D2GSPacketSrv12
// {
// uint8_t nHeader;
// uint8_t unk0x001[25];
// };
// static_assert(sizeof(D2GSPacketSrv12) == 26, "D2GSPacketSrv12: layout differs from the database");

// struct D2GSPacketSrv13
// {
// uint8_t nHeader;
// uint8_t unk0x001[13];
// };
// static_assert(sizeof(D2GSPacketSrv13) == 14, "D2GSPacketSrv13: layout differs from the database");

// struct D2GSPacketSrv14
// {
// uint8_t nHeader;
// uint8_t unk0x001[17];
// };
// static_assert(sizeof(D2GSPacketSrv14) == 18, "D2GSPacketSrv14: layout differs from the database");

// struct D2GSPacketSrv17
// {
// uint8_t nHeader;
// };
// static_assert(sizeof(D2GSPacketSrv17) == 1, "D2GSPacketSrv17: layout differs from the database");

// struct D2GSPacketSrv24
// {
// uint8_t nHeader;
// uint8_t unk0x001[89];
// };
// static_assert(sizeof(D2GSPacketSrv24) == 90, "D2GSPacketSrv24: layout differs from the database");

// struct D2GSPacketSrv25
// {
// uint8_t nHeader;
// uint8_t unk0x001[89];
// };
// static_assert(sizeof(D2GSPacketSrv25) == 90, "D2GSPacketSrv25: layout differs from the database");

// struct D2GSPacketSrv26Args
// {
// D2GSPacketSrv26 *pPacket26;
// D2GSPacketClt15 *pPacket15;
// int32_t nUnitId;
// };
// static_assert(sizeof(D2GSPacketSrv26Args) == 12, "D2GSPacketSrv26Args: layout differs from the database");

// struct D2GSPacketSrv2B
// {
// uint8_t nHeader;
// uint32_t dwSkill;
// uint32_t dwFrame;
// };
// static_assert(sizeof(D2GSPacketSrv2B) == 9, "D2GSPacketSrv2B: layout differs from the database");

// struct D2GSPacketSrv3A
// {
// uint8_t nHeader;
// };
// static_assert(sizeof(D2GSPacketSrv3A) == 1, "D2GSPacketSrv3A: layout differs from the database");

// struct D2GSPacketSrv3B
// {
// uint8_t nHeader;
// };
// static_assert(sizeof(D2GSPacketSrv3B) == 1, "D2GSPacketSrv3B: layout differs from the database");

// struct D2GSPacketSrv45
// {
// uint8_t nHeader;
// uint8_t unk0x001[12];
// };
// static_assert(sizeof(D2GSPacketSrv45) == 13, "D2GSPacketSrv45: layout differs from the database");

// struct D2GSPacketSrv55
// {
// uint8_t nHeader;
// uint8_t unk0x001[2];
// };
// static_assert(sizeof(D2GSPacketSrv55) == 3, "D2GSPacketSrv55: layout differs from the database");

// struct D2GSPacketSrv66
// {
// uint8_t nHeader;
// uint8_t unk0x001[6];
// };
// static_assert(sizeof(D2GSPacketSrv66) == 7, "D2GSPacketSrv66: layout differs from the database");

// struct D2GSPacketSrv6E
// {
// uint8_t nHeader;
// };
// static_assert(sizeof(D2GSPacketSrv6E) == 1, "D2GSPacketSrv6E: layout differs from the database");

// struct D2GSPacketSrv6F
// {
// uint8_t nHeader;
// };
// static_assert(sizeof(D2GSPacketSrv6F) == 1, "D2GSPacketSrv6F: layout differs from the database");

// struct D2GSPacketSrv70
// {
// uint8_t nHeader;
// };
// static_assert(sizeof(D2GSPacketSrv70) == 1, "D2GSPacketSrv70: layout differs from the database");

// struct D2GSPacketSrv71
// {
// uint8_t nHeader;
// };
// static_assert(sizeof(D2GSPacketSrv71) == 1, "D2GSPacketSrv71: layout differs from the database");

// struct D2GSPacketSrv72
// {
// uint8_t nHeader;
// };
// static_assert(sizeof(D2GSPacketSrv72) == 1, "D2GSPacketSrv72: layout differs from the database");

// struct D2GSPacketSrvAE
// {
// uint8_t nHeader;
// };
// static_assert(sizeof(D2GSPacketSrvAE) == 1, "D2GSPacketSrvAE: layout differs from the database");

// struct D2GSPacketSrvAF
// {
// uint8_t nHeader;
// };
// static_assert(sizeof(D2GSPacketSrvAF) == 1, "D2GSPacketSrvAF: layout differs from the database");

// struct D2GSPacketSrvB0
// {
// uint8_t nHeader;
// };
// static_assert(sizeof(D2GSPacketSrvB0) == 1, "D2GSPacketSrvB0: layout differs from the database");

// struct D2GSPacketSrvB1
// {
// uint8_t nHeader;
// char szGameName[48];
// uint16_t nClientCount;
// uint16_t nGameId;
// };
// static_assert(sizeof(D2GSPacketSrvB1) == 53, "D2GSPacketSrvB1: layout differs from the database");

// struct D2GSPacketSrvB2
// {
// uint8_t nHeader;
// };
// static_assert(sizeof(D2GSPacketSrvB2) == 1, "D2GSPacketSrvB2: layout differs from the database");

// struct D2GSPacketSrvB3
// {
// uint8_t nHeader;
// uint32_t nErrorCode;
// };
// static_assert(sizeof(D2GSPacketSrvB3) == 5, "D2GSPacketSrvB3: layout differs from the database");

// struct D2GSPacketSrvB4
// {
// uint8_t nHeader;
// uint32_t dwType;
// };
// static_assert(sizeof(D2GSPacketSrvB4) == 5, "D2GSPacketSrvB4: layout differs from the database");

// struct D2GSPacketSrvFF01
// {
// uint8_t nPacketSubType;
// D2GameServerInfoStrc tServerInfo;
// int32_t field_0xbd;
// int32_t aSomeInts[8];
// int32_t field_0xe1;
// int32_t field_0xe5;
// int32_t field_0xe9;
// int32_t field_0xed;
// int32_t field_0xf1;
// int16_t field_0xf5;
// char field_0xf6[20];
// int32_t field_0x10b;
// int32_t field_0x10f;
// int32_t field_0x113;
// int32_t field_0x117;
// char nZeroed[74];
// };
// static_assert(sizeof(D2GSPacketSrvFF01) == 357, "D2GSPacketSrvFF01: layout differs from the database");

// struct D2GSPacketSrvFFFD
// {
// uint8_t nPacketSubType;
// D2GameServerInfoStrc tServerInfo;
// };
// static_assert(sizeof(D2GSPacketSrvFFFD) == 189, "D2GSPacketSrvFFFD: layout differs from the database");

// struct D2GSPacketSrvStat
// {
// uint8_t nHeader;
// PacketStatId nStat;
// uint8_t nValue;
// };
// static_assert(sizeof(D2GSPacketSrvStat) == 3, "D2GSPacketSrvStat: layout differs from the database");

// struct D2GSPacketSrvStatAndGuid
// {
// uint8_t nHeader;
// PacketStatId nStat;
// uint32_t dwGUID;
// uint8_t nValue;
// };
// static_assert(sizeof(D2GSPacketSrvStatAndGuid) == 7, "D2GSPacketSrvStatAndGuid: layout differs from the database");

// struct D2GambleTxt
// {
// uint32_t dwItemCode;
// int32_t nLevel;
// int32_t nItemId;
// };
// static_assert(sizeof(D2GambleTxt) == 12, "D2GambleTxt: layout differs from the database");

// struct D2GameServerInfoExStrc : D2GameServerInfoStrc
// {
// uint32_t dwords0xBC[6];
// uint16_t word0xD4;
// char szUnk0xD6[20];
// uint16_t word0xEA;
// uint32_t dwords0xD8;
// uint32_t dwords0xF0[8];
// uint32_t dwords0x110;
// };
// static_assert(sizeof(D2GameServerInfoExStrc) == 276, "D2GameServerInfoExStrc: layout differs from the database");

// struct D2GameServerInfoStrc
// {
// int32_t field_0x0;
// int32_t field_0x4;
// int32_t field_0x8;
// int32_t field_0xc;
// int32_t field_0x10;
// int32_t field_0x14;
// int32_t field_0x18;
// int32_t field_0x1c;
// int32_t field_0x20;
// int32_t field_0x24;
// char szCompilationData[128];
// int32_t field_0xa8;
// int32_t field_0xac;
// int32_t field_0xb0;
// int32_t field_0xb4;
// int32_t field_0xb8;
// };
// static_assert(sizeof(D2GameServerInfoStrc) == 188, "D2GameServerInfoStrc: layout differs from the database");

// struct D2InventoryComponentItemTypeStrc
// {
// int32_t dwCode;
// int32_t nItemType;
// };
// static_assert(sizeof(D2InventoryComponentItemTypeStrc) == 8, "D2InventoryComponentItemTypeStrc: layout differs from the database");

// struct D2ItemCalcStrc
// {
// D2UnitStrc *pUnit;
// D2UnitStrc *pItem;
// };
// static_assert(sizeof(D2ItemCalcStrc) == 8, "D2ItemCalcStrc: layout differs from the database");

// struct D2ItemStatCostDescStrc
// {
// uint16_t nRecordId;
// int16_t nDescPriority;
// };
// static_assert(sizeof(D2ItemStatCostDescStrc) == 4, "D2ItemStatCostDescStrc: layout differs from the database");

// struct D2ItemTypeCheckStrc
// {
// uint8_t bItemType;
// uint8_t pad0x01[3];
// int32_t nItemType;
// };
// static_assert(sizeof(D2ItemTypeCheckStrc) == 8, "D2ItemTypeCheckStrc: layout differs from the database");

// struct D2JungleStrc
// {
// D2DrlgCoordStrc pDrlgCoord;
// int32_t field_10;
// int32_t nBranch;
// D2JungleStrc *pBasedOnJungle;
// D2JungleStrc *pJungleBranches[3];
// int32_t nPresetsBlocksX;
// int32_t nPresetsBlocksY;
// int32_t *pJungleDefs;
// int32_t nJungleDefs;
// };
// static_assert(sizeof(D2JungleStrc) == 56, "D2JungleStrc: layout differs from the database");

// struct D2KeyConfigOptionStrc
// {
// uint16_t nStringIndex;
// uint8_t _gap0[2];
// void *pCallback;
// uint16_t word0x06;
// uint8_t _gap1[2];
// int32_t nTextBeginX;
// int32_t nTextEndX;
// int32_t nTextBeginY;
// int32_t nTextEndY;
// uint16_t word0x18;
// uint8_t _gap2[2];
// };
// static_assert(sizeof(D2KeyConfigOptionStrc) == 32, "D2KeyConfigOptionStrc: layout differs from the database");

// struct D2KeyConfigStrc
// {
// int32_t nConfig;
// uint16_t nStringIndex;
// uint8_t _gap0[2];
// int32_t unk0x06;
// };
// static_assert(sizeof(D2KeyConfigStrc) == 12, "D2KeyConfigStrc: layout differs from the database");

// struct D2LevelFileListStrc
// {
// char szPath[260];
// int32_t nRefCount;
// D2DrlgFileStrc *pFile;
// D2LevelFileListStrc *pNext;
// };
// static_assert(sizeof(D2LevelFileListStrc) == 272, "D2LevelFileListStrc: layout differs from the database");

// struct D2MazeLevelIdStrc
// {
// int32_t nLevelPrestId1;
// int32_t nLevelPrestId2;
// int32_t nPickedFile;
// int32_t nDirection;
// };
// static_assert(sizeof(D2MazeLevelIdStrc) == 16, "D2MazeLevelIdStrc: layout differs from the database");

// struct D2MenuEntryStrc
// {
// int32_t nType;
// int32_t nLeft;
// int32_t nTop;
// int32_t nWidth;
// int32_t nHeight;
// int32_t unk0x014;
// int32_t nStrIndex;
// D2GfxDataStrc *pGfxData;
// int32_t (__stdcall *pBtnFunction)(void *);
// uint32_t unk0x024[2];
// int32_t nFont;
// };
// static_assert(sizeof(D2MenuEntryStrc) == 48, "D2MenuEntryStrc: layout differs from the database");

// struct D2MenuInfoStrc
// {
// int32_t nItemCount;
// int32_t unk0x04;
// int32_t unk0x08;
// int32_t unk0x0C;
// int32_t unk0x10;
// int32_t unk0x14;
// };
// static_assert(sizeof(D2MenuInfoStrc) == 24, "D2MenuInfoStrc: layout differs from the database");

// struct D2MenuUIStateSaveStrc
// {
// BOOL bCloseWhenOpen;
// BOOL bSaveUIState;
// int32_t nUIState;
// };
// static_assert(sizeof(D2MenuUIStateSaveStrc) == 12, "D2MenuUIStateSaveStrc: layout differs from the database");

// struct D2MissileCalcStrc
// {
// D2UnitStrc *pMissile;
// D2UnitStrc *pOwner;
// int32_t nMissileId;
// int32_t nMissileLevel;
// };
// static_assert(sizeof(D2MissileCalcStrc) == 16, "D2MissileCalcStrc: layout differs from the database");

// struct D2MissileDamageDataStrc
// {
// int32_t nFlags;
// int32_t nMinDamage;
// int32_t nMaxDamage;
// int32_t nFireMinDamage;
// int32_t nFireMaxDamage;
// int32_t nFireLength;
// int32_t nLightMinDamage;
// int32_t nLightMaxDamage;
// int32_t nMagicMinDamage;
// int32_t nMagicMaxDamage;
// int32_t nColdMinDamage;
// int32_t nColdMaxDamage;
// int32_t nColdLength;
// int32_t nPoisonMinDamage;
// int32_t nPoisonMaxDamage;
// int32_t nPoisonLength;
// int32_t nPoisonCount;
// int32_t nLifeDrainMinDamage;
// int32_t nLifeDrainMaxDamage;
// int32_t nManaDrainMinDamage;
// int32_t nManaDrainMaxDamage;
// int32_t nStaminaDrainMinDamage;
// int32_t nStaminaDrainMaxDamage;
// int32_t nStunLength;
// int32_t nBurningMin;
// int32_t nBurningMax;
// int32_t nBurnLength;
// int32_t nDemonDamagePercent;
// int32_t nUndeadDamagePercent;
// int32_t nDamageTargetAC;
// int32_t nDamagePercent;
// };
// static_assert(sizeof(D2MissileDamageDataStrc) == 124, "D2MissileDamageDataStrc: layout differs from the database");

// struct D2PathFoWallContextStrc
// {
// D2PathFoWallNodeStrc *aPendingCache[128];
// D2PathFoWallNodeStrc *aVisitedCache[128];
// D2PathFoWallNodeStrc *pSortedListByFScore;
// D2PathFoWallNodeStrc aNodesStorage[200];
// int32_t nNodesCount;
// D2PathFoWallNodeStrc *aPointsStack[200];
// int32_t nStackCount;
// };
// static_assert(sizeof(D2PathFoWallContextStrc) == 13036, "D2PathFoWallContextStrc: layout differs from the database");

// struct D2PathFoWallNodeStrc
// {
// D2PathPointStrc tPoint;
// int16_t nFScore;
// int16_t nHeuristicDistanceToTarget;
// int16_t nBestDistanceFromStart;
// uint16_t wPad;
// D2PathFoWallNodeStrc *pBestParent;
// D2PathFoWallNodeStrc *pChildren[8];
// D2PathFoWallNodeStrc *pNextCachePoint;
// D2PathFoWallNodeStrc *pNextSortedByFScore;
// };
// static_assert(sizeof(D2PathFoWallNodeStrc) == 56, "D2PathFoWallNodeStrc: layout differs from the database");

// struct D2PathIDAStarContextStrc
// {
// D2PathIDAStarNodeStrc *pCurrentNode;
// D2PathIDAStarNodeStrc aNodesStorage[900];
// int32_t nNodesCount;
// D2PathPointStrc nCoord[3];
// int32_t nStride;
// int32_t nXOffset;
// int32_t nYOffset;
// int32_t aCoordData[50000];
// int32_t bRandomDirection;
// D2SeedStrc *pSeed;
// };
// static_assert(sizeof(D2PathIDAStarContextStrc) == 225240, "D2PathIDAStarContextStrc: layout differs from the database");

// struct D2PathIDAStarNodeStrc
// {
// uint16_t nFScore;
// uint16_t nHeuristicDistanceToTarget;
// uint16_t nBestDistanceFromStart;
// int16_t nEvaluationsCount;
// D2PathPointStrc tCoord;
// int32_t *pNeighborsSequence;
// int32_t nNextNeighborIndex;
// D2PathIDAStarNodeStrc *pParent;
// D2PathIDAStarNodeStrc *pBestChild;
// };
// static_assert(sizeof(D2PathIDAStarNodeStrc) == 28, "D2PathIDAStarNodeStrc: layout differs from the database");

// struct D2PathMovStrc
// {
// int32_t nDirectionIndex;
// int32_t nPoints;
// D2PathPointStrc tCurrentCoord;
// D2PathPointStrc tTargetCoord;
// int32_t bReachedDeadEnd;
// int32_t *pPrevDirection;
// int32_t *pNextDirection;
// int32_t nLastSyncPointWithOtherPathIndex;
// int32_t bPathFinished;
// D2PathPointStrc pPoints[200];
// };
// static_assert(sizeof(D2PathMovStrc) == 836, "D2PathMovStrc: layout differs from the database");

// struct D2PlayerWeaponSequencesStrc
// {
// D2AnimSeqRecordStrc weaponRecords[14];
// };
// static_assert(sizeof(D2PlayerWeaponSequencesStrc) == 168, "D2PlayerWeaponSequencesStrc: layout differs from the database");

// struct D2PropertyAssignStrc
// {
// int32_t (__fastcall *pfAssign)(int32_t, D2UnitStrc *, D2UnitStrc *, const D2PropertyStrc *, int32_t, int32_t, int32_t, int32_t, D2UnitStrc *);
// int32_t nStatId;
// };
// static_assert(sizeof(D2PropertyAssignStrc) == 8, "D2PropertyAssignStrc: layout differs from the database");

// struct D2QuestDescStrc
// {
// uint16_t wTblTitle;
// uint16_t wTblInitDesc;
// };
// static_assert(sizeof(D2QuestDescStrc) == 4, "D2QuestDescStrc: layout differs from the database");

// struct D2QuestDescriptorStrc
// {
// uint8_t bActive;
// uint8_t nQuestNo;
// uint8_t nPosition;
// uint8_t nTab;
// uint16_t *pStringIds;
// int32_t nQuestFlag;
// int32_t nArrayId;
// };
// static_assert(sizeof(D2QuestDescriptorStrc) == 16, "D2QuestDescriptorStrc: layout differs from the database");

// struct D2QuestUiButtonCoordStrc
// {
// int32_t nCellfileX;
// int32_t nCellfileY;
// int32_t nClickX;
// int32_t nClickY;
// };
// static_assert(sizeof(D2QuestUiButtonCoordStrc) == 16, "D2QuestUiButtonCoordStrc: layout differs from the database");

// struct D2QuestUiStrc
// {
// uint8_t field_0;
// uint8_t _gap0[3];
// int32_t nQuest;
// int16_t nQuestTitleStringId;
// wchar_t wszQuestDesc[300];
// int16_t field_25F;
// uint8_t nPosition;
// uint8_t nQuestNo;
// uint8_t field_263;
// uint8_t field_264;
// uint8_t field_265;
// uint8_t _gap1[3];
// int32_t nQuestState;
// };
// static_assert(sizeof(D2QuestUiStrc) == 624, "D2QuestUiStrc: layout differs from the database");

// struct D2QuestUiTabStrc
// {
// int32_t nStartQuest;
// int32_t nEndQuest;
// };
// static_assert(sizeof(D2QuestUiTabStrc) == 8, "D2QuestUiTabStrc: layout differs from the database");

// struct D2SkillCalcStrc
// {
// D2UnitStrc *pUnit;
// int32_t nSkillId;
// int32_t nSkillLevel;
// };
// static_assert(sizeof(D2SkillCalcStrc) == 12, "D2SkillCalcStrc: layout differs from the database");

// struct D2TreasureClassExTxt
// {
// char szTreasureClass[32];
// int32_t nPicks;
// int16_t nGroup;
// int16_t nLevel;
// int16_t nMagic;
// int16_t nRare;
// int16_t nSet;
// int16_t nUnique;
// int16_t nSuperior;
// int16_t nNormal;
// int32_t nNoDrop;
// char szItem[10][64];
// int32_t nProb[10];
// };
// static_assert(sizeof(D2TreasureClassExTxt) == 736, "D2TreasureClassExTxt: layout differs from the database");

// struct D2UnitAnimModeStatBasePercentageStrc
// {
// int32_t bHasBasePercentage;
// int32_t nBasePercentage;
// int32_t nStatId;
// };
// static_assert(sizeof(D2UnitAnimModeStatBasePercentageStrc) == 12, "D2UnitAnimModeStatBasePercentageStrc: layout differs from the database");

// struct D2UnkDrlgLogicStrc
// {
// D2DrlgRoomStrc *pDrlgRoom;
// D2DrlgGridStrc *field_4;
// D2DrlgGridStrc *pTileTypeGrid;
// D2DrlgGridStrc *pWallGrid;
// D2DrlgGridStrc *pFloorGrid;
// D2DrlgGridStrc *field_14;
// int32_t field_18;
// int32_t nFlags;
// };
// static_assert(sizeof(D2UnkDrlgLogicStrc) == 32, "D2UnkDrlgLogicStrc: layout differs from the database");

// struct D2UnkDrlgRoomTileStrc
// {
// int32_t nLevelId;
// int32_t nArrayStartId;
// int32_t nArrayEndId;
// };
// static_assert(sizeof(D2UnkDrlgRoomTileStrc) == 12, "D2UnkDrlgRoomTileStrc: layout differs from the database");

// struct D2UnkDrlgRoomTileStrc2
// {
// int32_t nTileStyle;
// int32_t nTileSequence;
// int32_t bIsRightWallWithDoor;
// int32_t nUnitId;
// int32_t nUnitType;
// int32_t nX;
// int32_t nY;
// };
// static_assert(sizeof(D2UnkDrlgRoomTileStrc2) == 28, "D2UnkDrlgRoomTileStrc2: layout differs from the database");

// struct D2UnkHireStrc
// {
// int32_t field_0;
// int32_t field_4;
// int32_t field_8;
// int32_t field_C;
// int32_t field_10;
// int32_t field_14;
// int32_t field_18;
// };
// static_assert(sizeof(D2UnkHireStrc) == 28, "D2UnkHireStrc: layout differs from the database");

// struct D2UnkHireStrc2
// {
// int32_t field_0;
// int32_t field_4;
// };
// static_assert(sizeof(D2UnkHireStrc2) == 8, "D2UnkHireStrc2: layout differs from the database");

// struct D2UnkHireStrc3
// {
// int32_t field_0;
// int32_t field_4;
// int32_t field_8;
// int32_t field_C;
// };
// static_assert(sizeof(D2UnkHireStrc3) == 16, "D2UnkHireStrc3: layout differs from the database");

// struct D2UnkHireStrc4
// {
// int32_t field_0;
// int32_t field_4;
// int32_t field_8;
// int32_t field_C;
// int32_t field_10;
// };
// static_assert(sizeof(D2UnkHireStrc4) == 20, "D2UnkHireStrc4: layout differs from the database");

// struct D2UnkHireStrc5
// {
// int32_t field_0;
// int32_t field_4;
// int32_t field_8;
// int32_t field_C;
// int32_t field_10;
// int32_t field_14;
// };
// static_assert(sizeof(D2UnkHireStrc5) == 24, "D2UnkHireStrc5: layout differs from the database");

// struct D2UnkMissileCalcStrc
// {
// int32_t field_0;
// int32_t field_4;
// int32_t field_8;
// int32_t field_C;
// int32_t field_10;
// int32_t field_14;
// int32_t field_18;
// int32_t field_1C;
// D2SeedStrc pSeed;
// };
// static_assert(sizeof(D2UnkMissileCalcStrc) == 40, "D2UnkMissileCalcStrc: layout differs from the database");

// struct D2UnkOutPlaceStrc12
// {
// int32_t field_0;
// int32_t field_4;
// int32_t field_8;
// int32_t nX;
// int32_t nY;
// int32_t field_14;
// uint8_t *field_18;
// uint8_t nIndex;
// uint8_t field_1D;
// uint8_t field_1E;
// uint8_t field_1F;
// D2UnkOutPlaceStrc12 *pNext;
// D2UnkOutPlaceStrc12 *field_24;
// };
// static_assert(sizeof(D2UnkOutPlaceStrc12) == 40, "D2UnkOutPlaceStrc12: layout differs from the database");

// struct D2UnkOutdoorStrc
// {
// D2DrlgLevelStrc *pLevel;
// int32_t *field_4;
// D2DrlgGridStrc *pGrid1;
// D2DrlgGridStrc *pGrid2;
// int32_t nLevelPrestId;
// int32_t field_14;
// int32_t nLvlSubId;
// uint32_t (__fastcall *field_1C)(D2DrlgLevelStrc *, int32_t, int32_t);
// int32_t (__fastcall *field_20)(D2DrlgLevelStrc *, int32_t, int32_t, int32_t, int32_t, uint8_t);
// int32_t (__fastcall *field_24)(D2DrlgLevelStrc *, int32_t, int32_t, int32_t, int32_t, uint32_t);
// int32_t (__fastcall *field_28)(D2DrlgLevelStrc *, int32_t, int32_t);
// void (__fastcall *field_2C)(D2DrlgLevelStrc *, int32_t, int32_t);
// void (__fastcall *field_30)(D2DrlgLevelStrc *, int32_t, int32_t);
// void (__fastcall *field_34)(D2DrlgLevelStrc *, int32_t, int32_t, int32_t, int32_t, int32_t);
// };
// static_assert(sizeof(D2UnkOutdoorStrc) == 56, "D2UnkOutdoorStrc: layout differs from the database");

// struct D2UnkOutdoorStrc3
// {
// int32_t nLevelId;
// int32_t nExcludedLevel1;
// int32_t nExcludedLevel2;
// int32_t nRand;
// int32_t nNextRand;
// int32_t nFlags;
// };
// static_assert(sizeof(D2UnkOutdoorStrc3) == 24, "D2UnkOutdoorStrc3: layout differs from the database");

// struct D2UnkPathStrc
// {
// int32_t unk0x00;
// int32_t unk0x04;
// int32_t unk0x08;
// };
// static_assert(sizeof(D2UnkPathStrc) == 12, "D2UnkPathStrc: layout differs from the database");

// struct D2UnkPathStrc2
// {
// uint8_t unk0x00;
// uint8_t unk0x01;
// uint8_t unk0x02;
// };
// static_assert(sizeof(D2UnkPathStrc2) == 3, "D2UnkPathStrc2: layout differs from the database");

// struct D2WindowPlacementStrc
// {
// HWND hWnd;
// WINDOWPLACEMENT windowPlacement;
// };
// static_assert(sizeof(D2WindowPlacementStrc) == 48, "D2WindowPlacementStrc: layout differs from the database");

// struct HSARCHIVE__
// {
// int32_t unused;
// };
// static_assert(sizeof(HSARCHIVE__) == 4, "HSARCHIVE__: layout differs from the database");

// struct HSFILE__
// {
// int32_t unused;
// };
// static_assert(sizeof(HSFILE__) == 4, "HSFILE__: layout differs from the database");

// typedef uint8_t PacketStatId;

// struct TanToDirectionLutEntryStrc
// {
// D2CoordStrc dirVector;
// uint32_t nAngle;
// };
// static_assert(sizeof(TanToDirectionLutEntryStrc) == 12, "TanToDirectionLutEntryStrc: layout differs from the database");

// struct _OVERLAPPED
// {
// ULONG_PTR Internal;
// ULONG_PTR InternalHigh;
// union
// {
// struct
// {
// DWORD Offset;
// DWORD OffsetHigh;
// };
// PVOID Pointer;
// };
// HANDLE hEvent;
// };
// static_assert(sizeof(_OVERLAPPED) == 20, "_OVERLAPPED: layout differs from the database");

// union anon_127762106345282DF2C438C51804D07A
// {
// struct
// {
// char szToken[4];
// char szComponent[4];
// char szArmorType[4];
// char szMode[4];
// char szHitClass[4];
// };
// uint32_t dwName[5];
// };
// static_assert(sizeof(anon_127762106345282DF2C438C51804D07A) == 20, "anon_127762106345282DF2C438C51804D07A: layout differs from the database");

// struct anon_14587F979C641006F380FCBF23CEC161
// {
// int32_t nCount;
// uint8_t nExtraData9[34];
// };
// static_assert(sizeof(anon_14587F979C641006F380FCBF23CEC161) == 38, "anon_14587F979C641006F380FCBF23CEC161: layout differs from the database");

// struct anon_17EA52B533BA0577390A0A91F2834258
// {
// uint16_t nParams[2];
// };
// static_assert(sizeof(anon_17EA52B533BA0577390A0A91F2834258) == 4, "anon_17EA52B533BA0577390A0A91F2834258: layout differs from the database");

// struct anon_34227162FB87FBED6A1B09BFAFEE6B72
// {
// unsigned __int16 nSkill : 12;
// unsigned __int16 nHand : 4;
// };
// static_assert(sizeof(anon_34227162FB87FBED6A1B09BFAFEE6B72) == 2, "anon_34227162FB87FBED6A1B09BFAFEE6B72: layout differs from the database");

// union anon_380F2128073F347D8978CA0677085670
// {
// struct D2Srv50QuestsStatusPayloadStrc QuestsStatusPayload;
// struct D2Srv50HirelingPayloadStrc HirelingPayload;
// struct D2Srv50TombOffsetPayloadStrc TombOffsetPayload;
// struct D2Srv50A1Q4CainPayloadStrc A1Q4_CainPayload;
// struct D2Srv50A5Q6BaalPayloadStrc A5Q6_BaalPayload;
// };
// static_assert(sizeof(anon_380F2128073F347D8978CA0677085670) == 12, "anon_380F2128073F347D8978CA0677085670: layout differs from the database");

// struct anon_3F3466B13695857A15A82D9650D51B49
// {
// unsigned __int32 nUnkb00 : 1;
// unsigned __int32 bHasDirection : 1;
// unsigned __int32 nUnkb02 : 5;
// unsigned __int32 nUnkb07 : 1;
// unsigned __int32 nUnkb08 : 1;
// unsigned __int32 bHasPickedFile : 1;
// unsigned __int32 bLvlLink : 1;
// unsigned __int32 nUnkb11 : 1;
// unsigned __int32 nUnkb12 : 1;
// unsigned __int32 nUnkb13 : 3;
// unsigned __int32 nPickedFile : 4;
// unsigned __int32 nUnkb20 : 12;
// };
// static_assert(sizeof(anon_3F3466B13695857A15A82D9650D51B49) == 4, "anon_3F3466B13695857A15A82D9650D51B49: layout differs from the database");

// struct anon_41F79909BF2D6EA47E380FF32B7BDFF7
// {
// unsigned __int32 bIsWall : 1;
// unsigned __int32 bIsFloor : 1;
// unsigned __int32 bLOS : 1;
// unsigned __int32 bEnclosed : 1;
// unsigned __int32 bExit : 1;
// unsigned __int32 bUnk0x20 : 1;
// unsigned __int32 bLayerBelow : 1;
// unsigned __int32 bLayerAbove : 1;
// unsigned __int32 nTileSequence : 8;
// unsigned __int32 bFillLOS : 1;
// unsigned __int32 bUnwalkable : 1;
// unsigned __int32 nWallLayer : 2;
// unsigned __int32 nTileStyle : 6;
// unsigned __int32 bRevealHidden : 1;
// unsigned __int32 bShadow : 1;
// unsigned __int32 bLinkage : 1;
// unsigned __int32 bObjectWall : 1;
// unsigned __int32 bUnk0x40000000 : 1;
// unsigned __int32 bHidden : 1;
// };
// static_assert(sizeof(anon_41F79909BF2D6EA47E380FF32B7BDFF7) == 4, "anon_41F79909BF2D6EA47E380FF32B7BDFF7: layout differs from the database");

// struct anon_427EBAF5AD08AB583E1D4DF460AB8325
// {
// uint8_t nDefenderUnitType;
// int32_t nDefenderUnitGUID;
// uint8_t nAttackerUnitType;
// int32_t nAttackerUnitGUID;
// uint8_t nAttackMessageColor;
// uint8_t nExtraData7[27];
// };
// static_assert(sizeof(anon_427EBAF5AD08AB583E1D4DF460AB8325) == 38, "anon_427EBAF5AD08AB583E1D4DF460AB8325: layout differs from the database");

// struct anon_4CADA87776A5C43294374C4CD737B374
// {
// int32_t dwSuicide;
// int32_t dwPlayerKill;
// int32_t dwPlayerKillPercent;
// int32_t dwMonsterKill;
// int32_t dwPlayerDeath;
// int32_t dwPlayerDeathPercent;
// int32_t dwMonsterDeath;
// };
// static_assert(sizeof(anon_4CADA87776A5C43294374C4CD737B374) == 28, "anon_4CADA87776A5C43294374C4CD737B374: layout differs from the database");

// struct anon_50DA7804EBF03AEE90BE8F81913C2806
// {
// uint8_t nColor;
// uint32_t dwParam;
// uint8_t nParam;
// union
// {
// char szText[32];
// char szTextData[2][16];
// };
// };
// static_assert(sizeof(anon_50DA7804EBF03AEE90BE8F81913C2806) == 38, "anon_50DA7804EBF03AEE90BE8F81913C2806: layout differs from the database");

// struct anon_527403D9EC2CF491362D3AE59B5ADD67
// {
// uint8_t nDamageColor;
// uint32_t dwDamage;
// int32_t nDefenderGUID;
// int32_t nDefenderType;
// uint8_t nExtraData5[25];
// };
// static_assert(sizeof(anon_527403D9EC2CF491362D3AE59B5ADD67) == 38, "anon_527403D9EC2CF491362D3AE59B5ADD67: layout differs from the database");

// union anon_5A9DD72A24B1493213128C856E309FBA
// {
// int32_t nRand[4][15];
// int32_t nRand2[60];
// };
// static_assert(sizeof(anon_5A9DD72A24B1493213128C856E309FBA) == 240, "anon_5A9DD72A24B1493213128C856E309FBA: layout differs from the database");

// union anon_83B6E30208D726A49ED3B6646DA7606D
// {
// uint32_t dwCTemp;
// struct D2ConfigCTempStrc unpackedCTemp;
// };
// static_assert(sizeof(anon_83B6E30208D726A49ED3B6646DA7606D) == 4, "anon_83B6E30208D726A49ED3B6646DA7606D: layout differs from the database");

// union anon_9061D5506B188DF6886B65C2F499A751
// {
// struct
// {
// uint8_t nColor;
// uint32_t dwParam;
// uint8_t nParam;
// union
// {
// char szText[32];
// char szTextData[2][16];
// };
// };
// struct
// {
// uint32_t dwSkill;
// uint32_t dwEndFrame;
// uint8_t nExtraData2[30];
// };
// struct
// {
// uint8_t nGroup;
// uint32_t dwGroupEndFrame;
// uint8_t nExtraData3[33];
// };
// struct
// {
// int32_t nItemId;
// int32_t nPlayerId;
// uint8_t nExtraData4[30];
// };
// struct
// {
// uint8_t nDamageColor;
// uint32_t dwDamage;
// int32_t nDefenderGUID;
// int32_t nDefenderType;
// uint8_t nExtraData5[25];
// };
// struct
// {
// uint8_t nUnitType;
// int32_t nItemGUID;
// uint8_t nAction;
// int32_t nPosX;
// int32_t nPosY;
// uint8_t nExtraData6[24];
// };
// struct
// {
// uint8_t nDefenderUnitType;
// int32_t nDefenderUnitGUID;
// uint8_t nAttackerUnitType;
// int32_t nAttackerUnitGUID;
// uint8_t nAttackMessageColor;
// uint8_t nExtraData7[27];
// };
// struct
// {
// int32_t nFrames;
// uint8_t nExtraData8[34];
// };
// struct
// {
// int32_t nCount;
// uint8_t nExtraData9[34];
// };
// };
// static_assert(sizeof(anon_9061D5506B188DF6886B65C2F499A751) == 38, "anon_9061D5506B188DF6886B65C2F499A751: layout differs from the database");

// union anon_9C8EE1B46AB7CB663EA62D576175A87F
// {
// struct
// {
// int32_t dwSuicide;
// int32_t dwPlayerKill;
// int32_t dwPlayerKillPercent;
// int32_t dwMonsterKill;
// int32_t dwPlayerDeath;
// int32_t dwPlayerDeathPercent;
// int32_t dwMonsterDeath;
// };
// int32_t dwScores[7];
// };
// static_assert(sizeof(anon_9C8EE1B46AB7CB663EA62D576175A87F) == 28, "anon_9C8EE1B46AB7CB663EA62D576175A87F: layout differs from the database");

// struct anon_9E01DBFBEC86134033FD3477EE0CD60D
// {
// unsigned __int16 nItemMode : 15;
// unsigned __int16 bMultibuy : 1;
// };
// static_assert(sizeof(anon_9E01DBFBEC86134033FD3477EE0CD60D) == 2, "anon_9E01DBFBEC86134033FD3477EE0CD60D: layout differs from the database");

// union anon_9E656BAF27D4BA1E170AEB1A92E0D77D
// {
// struct
// {
// uint16_t nParams[2];
// };
// int32_t nItemGUID;
// };
// static_assert(sizeof(anon_9E656BAF27D4BA1E170AEB1A92E0D77D) == 4, "anon_9E656BAF27D4BA1E170AEB1A92E0D77D: layout differs from the database");

// struct anon_A0B592BF32EB0681EBBB5D252D2EFAF3
// {
// uint8_t nUnitType;
// int32_t nItemGUID;
// uint8_t nAction;
// int32_t nPosX;
// int32_t nPosY;
// uint8_t nExtraData6[24];
// };
// static_assert(sizeof(anon_A0B592BF32EB0681EBBB5D252D2EFAF3) == 38, "anon_A0B592BF32EB0681EBBB5D252D2EFAF3: layout differs from the database");

// struct anon_A3719FF1A699486E8629E16291555876
// {
// int32_t nFrames;
// uint8_t nExtraData8[34];
// };
// static_assert(sizeof(anon_A3719FF1A699486E8629E16291555876) == 38, "anon_A3719FF1A699486E8629E16291555876: layout differs from the database");

// struct anon_A85CD68F813312C5E6BB1494B5EEB9CC
// {
// char szToken[4];
// char szComponent[4];
// char szArmorType[4];
// char szMode[4];
// char szHitClass[4];
// };
// static_assert(sizeof(anon_A85CD68F813312C5E6BB1494B5EEB9CC) == 20, "anon_A85CD68F813312C5E6BB1494B5EEB9CC: layout differs from the database");

// struct anon_ABAA6F78D0909A5EC38B8DAC19886F2D
// {
// int32_t nItemId;
// int32_t nPlayerId;
// uint8_t nExtraData4[30];
// };
// static_assert(sizeof(anon_ABAA6F78D0909A5EC38B8DAC19886F2D) == 38, "anon_ABAA6F78D0909A5EC38B8DAC19886F2D: layout differs from the database");

// struct anon_B40DC28E98FE5F06F50437E23D68FF31
// {
// uint32_t dwSkill;
// uint32_t dwEndFrame;
// uint8_t nExtraData2[30];
// };
// static_assert(sizeof(anon_B40DC28E98FE5F06F50437E23D68FF31) == 38, "anon_B40DC28E98FE5F06F50437E23D68FF31: layout differs from the database");

// union anon_BBC6B91D16152AC7247EC2523B3ED144
// {
// char szText[32];
// char szTextData[2][16];
// };
// static_assert(sizeof(anon_BBC6B91D16152AC7247EC2523B3ED144) == 32, "anon_BBC6B91D16152AC7247EC2523B3ED144: layout differs from the database");

// struct anon_CAAEA91160A63143F2AE058B4EF58ACE
// {
// uint8_t nGroup;
// uint32_t dwGroupEndFrame;
// uint8_t nExtraData3[33];
// };
// static_assert(sizeof(anon_CAAEA91160A63143F2AE058B4EF58ACE) == 38, "anon_CAAEA91160A63143F2AE058B4EF58ACE: layout differs from the database");

// union anon_FB734C8D8ED8BB4F2FAE5CDADDE28D6C
// {
// uint8_t nComponent;
// uint8_t fItemFlags;
// };
// static_assert(sizeof(anon_FB734C8D8ED8BB4F2FAE5CDADDE28D6C) == 1, "anon_FB734C8D8ED8BB4F2FAE5CDADDE28D6C: layout differs from the database");


/* ---- layout self-check --------------------------------------------------
 * sizeof() as IDA measured it in D2Client.dll. This file is generated text:
 * one bad rewrite could produce something that compiles and describes the
 * wrong structure, and only an assertion catches that. If one fires, the
 * header and the database have diverged -- find out which moved before
 * trusting any offset from either.
 *------------------------------------------------------------------------*/
#ifndef D2STRUCTS_NO_ASSERTS
/* MSVC reports __cplusplus as 199711L unless /Zc:__cplusplus is passed -- even
 * under /std:c++20, where the real value is in _MSVC_LANG. Testing __cplusplus
 * alone left this whole block UNCOMPILED in the project that consumes it, and
 * 101 of the assertions below were failing unnoticed. Measured 2026-09-06. */
#if defined(_MSVC_LANG)
#  define D2STRUCTS_LANG _MSVC_LANG
#elif defined(__cplusplus)
#  define D2STRUCTS_LANG __cplusplus
#else
#  define D2STRUCTS_LANG 0L
#endif
#if D2STRUCTS_LANG >= 201103L
static_assert(sizeof(AsyncDataLoadFileCallback) == 28, "AsyncDataLoadFileCallback: layout differs from the database");
static_assert(sizeof(AsyncData) == 84, "AsyncData: layout differs from the database");
static_assert(sizeof(AsyncDataEventSlot) == 12, "AsyncDataEventSlot: layout differs from the database");
static_assert(sizeof(CursorDescStrc) == 28, "CursorDescStrc: layout differs from the database");
static_assert(sizeof(D2DrlgCoordsStrc) == 32, "D2DrlgCoordsStrc: layout differs from the database");
static_assert(sizeof(D2SeedStrc) == 8, "D2SeedStrc: layout differs from the database");
static_assert(sizeof(D2ActiveRoomStrc) == 128, "D2ActiveRoomStrc: layout differs from the database");
static_assert(sizeof(D2AnimDataRecordStrc) == 160, "D2AnimDataRecordStrc: layout differs from the database");
static_assert(sizeof(D2AnimDataBucketStrc) == 4, "D2AnimDataBucketStrc: layout differs from the database");
static_assert(sizeof(D2AnimDataTableStrc) == 1188, "D2AnimDataTableStrc: layout differs from the database");
static_assert(sizeof(D2AnimSeqRecordStrc) == 12, "D2AnimSeqRecordStrc: layout differs from the database");
static_assert(sizeof(D2AnimSeqTxt) == 6, "D2AnimSeqTxt: layout differs from the database");
static_assert(sizeof(D2AnvilPanelButtonStrc) == 40, "D2AnvilPanelButtonStrc: layout differs from the database");
static_assert(sizeof(D2AreaEntryQuestLineStrc) == 48, "D2AreaEntryQuestLineStrc: layout differs from the database");
static_assert(sizeof(D2ArmTypeTxt) == 52, "D2ArmTypeTxt: layout differs from the database");
static_assert(sizeof(D2AutomapCellStrc) == 20, "D2AutomapCellStrc: layout differs from the database");
static_assert(sizeof(D2AutomapDataStrc) == 44, "D2AutomapDataStrc: layout differs from the database");
static_assert(sizeof(D2AutomapLayerStrc) == 28, "D2AutomapLayerStrc: layout differs from the database");
static_assert(sizeof(D2AutomapRandStrc) == 8, "D2AutomapRandStrc: layout differs from the database");
static_assert(sizeof(D2AutomapShortStrc) == 32, "D2AutomapShortStrc: layout differs from the database");
static_assert(sizeof(D2InvRectStrc) == 16, "D2InvRectStrc: layout differs from the database");
static_assert(sizeof(D2BeltsTxt) == 264, "D2BeltsTxt: layout differs from the database");
static_assert(sizeof(D2BetaScreenStrc) == 13, "D2BetaScreenStrc: layout differs from the database");
static_assert(sizeof(D2BinFieldStrc) == 20, "D2BinFieldStrc: layout differs from the database");
static_assert(sizeof(D2BinFileStrc) == 16, "D2BinFileStrc: layout differs from the database");
static_assert(sizeof(D2BitBufferStrc) == 20, "D2BitBufferStrc: layout differs from the database");
static_assert(sizeof(D2BodyLocsTxt) == 4, "D2BodyLocsTxt: layout differs from the database");
static_assert(sizeof(D2BookDataTbl) == 8, "D2BookDataTbl: layout differs from the database");
static_assert(sizeof(D2BooksTxt) == 32, "D2BooksTxt: layout differs from the database");
static_assert(sizeof(D2BoundingBoxStrc) == 16, "D2BoundingBoxStrc: layout differs from the database");
static_assert(sizeof(D2BuySellTabStrc) == 18, "D2BuySellTabStrc: layout differs from the database");
static_assert(sizeof(D2CfgControlsRowStrc) == 10, "D2CfgControlsRowStrc: layout differs from the database");
static_assert(sizeof(D2ChampionUModNameStrc) == 8, "D2ChampionUModNameStrc: layout differs from the database");
static_assert(sizeof(D2CharItemStrc) == 8, "D2CharItemStrc: layout differs from the database");
static_assert(sizeof(D2CharPanelLabelStrc) == 18, "D2CharPanelLabelStrc: layout differs from the database");
static_assert(sizeof(Unicode) == 2, "Unicode: layout differs from the database");
static_assert(sizeof(D2CharStatsTxt) == 196, "D2CharStatsTxt: layout differs from the database");
static_assert(sizeof(D2CharacterPreviewInfoStrc) == 38, "D2CharacterPreviewInfoStrc: layout differs from the database");
static_assert(sizeof(D2ChatPrefixStrc) == 14, "D2ChatPrefixStrc: layout differs from the database");
static_assert(sizeof(D2CinematicEntry) == 12, "D2CinematicEntry: layout differs from the database");
static_assert(sizeof(D2ClientAutomapDrawCtxStrc) == 28, "D2ClientAutomapDrawCtxStrc: layout differs from the database");
static_assert(sizeof(D2ClientButtonStrc) == 54, "D2ClientButtonStrc: layout differs from the database");
static_assert(sizeof(D2ClientCfgControlsButtonStrc) == 26, "D2ClientCfgControlsButtonStrc: layout differs from the database");
static_assert(sizeof(D2ClientClassSkillIconStrc) == 34, "D2ClientClassSkillIconStrc: layout differs from the database");
static_assert(sizeof(D2ClientClickStrc) == 32, "D2ClientClickStrc: layout differs from the database");
static_assert(sizeof(D2ClientCofCacheNodeStrc) == 28, "D2ClientCofCacheNodeStrc: layout differs from the database");
static_assert(sizeof(D2ClientCofCacheStrc) == 2060, "D2ClientCofCacheStrc: layout differs from the database");
static_assert(sizeof(D2ClientCofNodeStrc) == 20, "D2ClientCofNodeStrc: layout differs from the database");
static_assert(sizeof(D2ClientDamageGroupStrc) == 24, "D2ClientDamageGroupStrc: layout differs from the database");
static_assert(sizeof(D2ClientDialogLineStrc) == 272, "D2ClientDialogLineStrc: layout differs from the database");
static_assert(sizeof(D2ClientDialogStrc) == 2832, "D2ClientDialogStrc: layout differs from the database");
static_assert(sizeof(D2ClientDrawNodeStrc) == 20, "D2ClientDrawNodeStrc: layout differs from the database");
static_assert(sizeof(D2ClientEditBoxStrc) == 90, "D2ClientEditBoxStrc: layout differs from the database");
static_assert(sizeof(D2ClientEnvParticleStrc) == 40, "D2ClientEnvParticleStrc: layout differs from the database");
static_assert(sizeof(D2ClientEnvSplashStrc) == 24, "D2ClientEnvSplashStrc: layout differs from the database");
static_assert(sizeof(D2ClientEscMenuInfoStrc) == 24, "D2ClientEscMenuInfoStrc: layout differs from the database");
static_assert(sizeof(D2ClientEscMenuItemStrc) == 1360, "D2ClientEscMenuItemStrc: layout differs from the database");
static_assert(sizeof(D2ClientEscMenuUiWindowStrc) == 12, "D2ClientEscMenuUiWindowStrc: layout differs from the database");
static_assert(sizeof(D2ClientFloorVertexStrc) == 12, "D2ClientFloorVertexStrc: layout differs from the database");
static_assert(sizeof(D2ClientGfxEntryStrc) == 168, "D2ClientGfxEntryStrc: layout differs from the database");
static_assert(sizeof(D2ClientHirelingInitStrc) == 8, "D2ClientHirelingInitStrc: layout differs from the database");
static_assert(sizeof(D2ClientInterface) == 4, "D2ClientInterface: layout differs from the database");
static_assert(sizeof(D2ClientLevelEntryTitleStrc) == 13, "D2ClientLevelEntryTitleStrc: layout differs from the database");
static_assert(sizeof(D2ClientLightStrc) == 52, "D2ClientLightStrc: layout differs from the database");
static_assert(sizeof(D2ClientListBoxEntryStrc) == 12302, "D2ClientListBoxEntryStrc: layout differs from the database");
static_assert(sizeof(D2ClientListBoxStrc) == 1574774, "D2ClientListBoxStrc: layout differs from the database");
static_assert(sizeof(D2ClientMissileTargetCtxStrc) == 12, "D2ClientMissileTargetCtxStrc: layout differs from the database");
static_assert(sizeof(D2ClientModePreloadStrc) == 160, "D2ClientModePreloadStrc: layout differs from the database");
static_assert(sizeof(D2ClientMonCofTokensStrc) == 52, "D2ClientMonCofTokensStrc: layout differs from the database");
static_assert(sizeof(D2ClientMonModeDescStrc) == 12, "D2ClientMonModeDescStrc: layout differs from the database");
static_assert(sizeof(D2ClientMonModeTokenOverrideStrc) == 8, "D2ClientMonModeTokenOverrideStrc: layout differs from the database");
static_assert(sizeof(D2ClientMonPalShiftStrc) == 8, "D2ClientMonPalShiftStrc: layout differs from the database");
static_assert(sizeof(D2ClientNearbyTargetCtxStrc) == 12, "D2ClientNearbyTargetCtxStrc: layout differs from the database");
static_assert(sizeof(D2ClientNpcSpeechStrc) == 24, "D2ClientNpcSpeechStrc: layout differs from the database");
static_assert(sizeof(D2ClientObjectModeSoundStrc) == 48, "D2ClientObjectModeSoundStrc: layout differs from the database");
static_assert(sizeof(D2ClientPalShiftStrc) == 4104, "D2ClientPalShiftStrc: layout differs from the database");
static_assert(sizeof(D2ClientParticleStrc) == 76, "D2ClientParticleStrc: layout differs from the database");
static_assert(sizeof(D2ClientPartyButtonStrc) == 32, "D2ClientPartyButtonStrc: layout differs from the database");
static_assert(sizeof(D2ClientPartyRowStrc) == 209, "D2ClientPartyRowStrc: layout differs from the database");
static_assert(sizeof(D2ClientPetIconCacheStrc) == 20, "D2ClientPetIconCacheStrc: layout differs from the database");
static_assert(sizeof(D2ClientPetInfoStrc) == 12, "D2ClientPetInfoStrc: layout differs from the database");
static_assert(sizeof(D2ClientPopupStrc) == 44, "D2ClientPopupStrc: layout differs from the database");
static_assert(sizeof(D2ClientRecordFileHeaderStrc) == 20, "D2ClientRecordFileHeaderStrc: layout differs from the database");
static_assert(sizeof(D2ClientRecordStrc) == 16, "D2ClientRecordStrc: layout differs from the database");
static_assert(sizeof(D2ClientRecordHolderStrc) == 8012, "D2ClientRecordHolderStrc: layout differs from the database");
static_assert(sizeof(D2ClientShowItemLabelStrc) == 288, "D2ClientShowItemLabelStrc: layout differs from the database");
static_assert(sizeof(D2ClientShrineChartStrc) == 20, "D2ClientShrineChartStrc: layout differs from the database");
static_assert(sizeof(D2ClientSkillTreeChartStrc) == 34, "D2ClientSkillTreeChartStrc: layout differs from the database");
static_assert(sizeof(D2ClientSpeechSoundStrc) == 8, "D2ClientSpeechSoundStrc: layout differs from the database");
static_assert(sizeof(D2ClientSpeechSubtitleStrc) == 4, "D2ClientSpeechSubtitleStrc: layout differs from the database");
static_assert(sizeof(D2ClientSpinnerStrc) == 84, "D2ClientSpinnerStrc: layout differs from the database");
static_assert(sizeof(D2ClientStatDescStrc) == 16, "D2ClientStatDescStrc: layout differs from the database");
static_assert(sizeof(D2ClientSubMissileSpawnStrc) == 24, "D2ClientSubMissileSpawnStrc: layout differs from the database");
static_assert(sizeof(D2ClientTargetCycleStrc) == 24, "D2ClientTargetCycleStrc: layout differs from the database");
static_assert(sizeof(D2ClientTextLineStrc) == 8, "D2ClientTextLineStrc: layout differs from the database");
static_assert(sizeof(D2ClientTextNodeStrc) == 38, "D2ClientTextNodeStrc: layout differs from the database");
static_assert(sizeof(D2ClientUnitGfxInfoStrc) == 60, "D2ClientUnitGfxInfoStrc: layout differs from the database");
static_assert(sizeof(D2ClientUnitUpdateStrc) == 9, "D2ClientUnitUpdateStrc: layout differs from the database");
static_assert(sizeof(D2ClientViewStrc) == 60092, "D2ClientViewStrc: layout differs from the database");
static_assert(sizeof(D2ClientWallRoomArgsStrc) == 28, "D2ClientWallRoomArgsStrc: layout differs from the database");
static_assert(sizeof(D2ClientWallSquareStrc) == 36, "D2ClientWallSquareStrc: layout differs from the database");
static_assert(sizeof(D2ClientWaypointActStrc) == 11, "D2ClientWaypointActStrc: layout differs from the database");
static_assert(sizeof(D2CofHeaderStrc) == 24, "D2CofHeaderStrc: layout differs from the database");
static_assert(sizeof(D2CollisionFieldStrc) == 20, "D2CollisionFieldStrc: layout differs from the database");
static_assert(sizeof(D2ColorsTxt) == 4, "D2ColorsTxt: layout differs from the database");
static_assert(sizeof(D2CompCodeTxt) == 4, "D2CompCodeTxt: layout differs from the database");
static_assert(sizeof(D2CompositTxt) == 52, "D2CompositTxt: layout differs from the database");
static_assert(sizeof(D2ConfigCTempStrc) == 4, "D2ConfigCTempStrc: layout differs from the database");
static_assert(sizeof(D2ConfigStrc) == 967, "D2ConfigStrc: layout differs from the database");
static_assert(sizeof(D2CoordStrc) == 8, "D2CoordStrc: layout differs from the database");
static_assert(sizeof(D2CorpseStrc) == 16, "D2CorpseStrc: layout differs from the database");
static_assert(sizeof(D2CubeInputItem) == 8, "D2CubeInputItem: layout differs from the database");
static_assert(sizeof(D2CubeOutputParamStrc) == 12, "D2CubeOutputParamStrc: layout differs from the database");
static_assert(sizeof(D2CubeOutputItem) == 84, "D2CubeOutputItem: layout differs from the database");
static_assert(sizeof(D2CubeMainTxt) == 328, "D2CubeMainTxt: layout differs from the database");
static_assert(sizeof(D2DataArrayStrc) == 288, "D2DataArrayStrc: layout differs from the database");
static_assert(sizeof(D2GambleDataTbl) == 408, "D2GambleDataTbl: layout differs from the database");
static_assert(sizeof(D2GemDataTbl) == 8, "D2GemDataTbl: layout differs from the database");
static_assert(sizeof(D2ItemDataTbl) == 32, "D2ItemDataTbl: layout differs from the database");
static_assert(sizeof(D2ItemRatioDataTbl) == 8, "D2ItemRatioDataTbl: layout differs from the database");
static_assert(sizeof(D2LowQualityItemDataTbl) == 8, "D2LowQualityItemDataTbl: layout differs from the database");
static_assert(sizeof(D2MagicAffixDataTbl) == 20, "D2MagicAffixDataTbl: layout differs from the database");
static_assert(sizeof(D2MonItemPercentDataTbl) == 8, "D2MonItemPercentDataTbl: layout differs from the database");
static_assert(sizeof(D2MonModeDataTbl) == 16, "D2MonModeDataTbl: layout differs from the database");
static_assert(sizeof(D2ObjModeDataTbl) == 16, "D2ObjModeDataTbl: layout differs from the database");
static_assert(sizeof(D2PlrModeDataTbl) == 16, "D2PlrModeDataTbl: layout differs from the database");
static_assert(sizeof(D2QualityItemDataTbl) == 8, "D2QualityItemDataTbl: layout differs from the database");
static_assert(sizeof(D2RareAffixDataTbl) == 16, "D2RareAffixDataTbl: layout differs from the database");
static_assert(sizeof(D2RuneDataTbl) == 8, "D2RuneDataTbl: layout differs from the database");
static_assert(sizeof(D2DataTablesStrc) == 4324, "D2DataTablesStrc: layout differs from the database");
static_assert(sizeof(D2DifficultyLevelsTxt) == 88, "D2DifficultyLevelsTxt: layout differs from the database");
static_assert(sizeof(D2DrlgTileDataStrc) == 48, "D2DrlgTileDataStrc: layout differs from the database");
static_assert(sizeof(D2DrlgActStrc) == 96, "D2DrlgActStrc: layout differs from the database");
static_assert(sizeof(D2DrlgAnimTileGridStrc) == 20, "D2DrlgAnimTileGridStrc: layout differs from the database");
static_assert(sizeof(D2DrlgBuildStrc) == 16, "D2DrlgBuildStrc: layout differs from the database");
static_assert(sizeof(D2DrlgCoordStrc) == 16, "D2DrlgCoordStrc: layout differs from the database");
static_assert(sizeof(D2DrlgDeleteStrc) == 12, "D2DrlgDeleteStrc: layout differs from the database");
static_assert(sizeof(D2DrlgEnvironmentStrc) == 56, "D2DrlgEnvironmentStrc: layout differs from the database");
static_assert(sizeof(D2DrlgFileStrc) == 92, "D2DrlgFileStrc: layout differs from the database");
static_assert(sizeof(D2DrlgGridStrc) == 20, "D2DrlgGridStrc: layout differs from the database");
static_assert(sizeof(D2DrlgLevelRectStrc) == 16, "D2DrlgLevelRectStrc: layout differs from the database");
static_assert(sizeof(D2DrlgLevelRectUnion) == 16, "D2DrlgLevelRectUnion: layout differs from the database");
static_assert(sizeof(D2DrlgTileInfoStrc) == 12, "D2DrlgTileInfoStrc: layout differs from the database");
static_assert(sizeof(D2DrlgLevelStrc) == 560, "D2DrlgLevelStrc: layout differs from the database");
static_assert(sizeof(D2DrlgLevelTypeDataUnion) == 4, "D2DrlgLevelTypeDataUnion: layout differs from the database");
static_assert(sizeof(D2DrlgLogicalRoomInfoStrc) == 52, "D2DrlgLogicalRoomInfoStrc: layout differs from the database");
static_assert(sizeof(D2DrlgMapStrc) == 88, "D2DrlgMapStrc: layout differs from the database");
static_assert(sizeof(D2DrlgOrthStrc) == 24, "D2DrlgOrthStrc: layout differs from the database");
static_assert(sizeof(D2DrlgOrthTargetUnion) == 4, "D2DrlgOrthTargetUnion: layout differs from the database");
static_assert(sizeof(D2DrlgVertexStrc) == 20, "D2DrlgVertexStrc: layout differs from the database");
static_assert(sizeof(D2DrlgOutdoorInfoStrc) == 616, "D2DrlgOutdoorInfoStrc: layout differs from the database");
static_assert(sizeof(D2DrlgOutdoorRoomStrc) == 112, "D2DrlgOutdoorRoomStrc: layout differs from the database");
static_assert(sizeof(D2DrlgOutdoorSizeStrc) == 16, "D2DrlgOutdoorSizeStrc: layout differs from the database");
static_assert(sizeof(D2DrlgOutdoorSizeUnion) == 16, "D2DrlgOutdoorSizeUnion: layout differs from the database");
static_assert(sizeof(D2DrlgPresetInfoStrc) == 8, "D2DrlgPresetInfoStrc: layout differs from the database");
static_assert(sizeof(D2DrlgPresetRoomFlagsStrc) == 4, "D2DrlgPresetRoomFlagsStrc: layout differs from the database");
static_assert(sizeof(D2DrlgPresetRoomFlagsUnion) == 4, "D2DrlgPresetRoomFlagsUnion: layout differs from the database");
static_assert(sizeof(D2DrlgPresetRoomStrc) == 248, "D2DrlgPresetRoomStrc: layout differs from the database");
static_assert(sizeof(D2DrlgRoomCoordUnion) == 16, "D2DrlgRoomCoordUnion: layout differs from the database");
static_assert(sizeof(D2DrlgRoomStrc) == 236, "D2DrlgRoomStrc: layout differs from the database");
static_assert(sizeof(D2DrlgRoomTileRectStrc) == 16, "D2DrlgRoomTileRectStrc: layout differs from the database");
static_assert(sizeof(D2DrlgRoomTilesStrc) == 24, "D2DrlgRoomTilesStrc: layout differs from the database");
static_assert(sizeof(D2DrlgRoomTypeDataUnion) == 4, "D2DrlgRoomTypeDataUnion: layout differs from the database");
static_assert(sizeof(D2DrlgStrc) == 1160, "D2DrlgStrc: layout differs from the database");
static_assert(sizeof(D2DrlgSubstGroupStrc) == 24, "D2DrlgSubstGroupStrc: layout differs from the database");
static_assert(sizeof(D2DrlgTileGridStrc) == 44, "D2DrlgTileGridStrc: layout differs from the database");
static_assert(sizeof(D2DrlgTileLinkStrc) == 12, "D2DrlgTileLinkStrc: layout differs from the database");
static_assert(sizeof(D2DrlgWarpStrc) == 72, "D2DrlgWarpStrc: layout differs from the database");
static_assert(sizeof(D2FP32_16) == 8, "D2FP32_16: layout differs from the database");
static_assert(sizeof(D2PathPointStrc) == 4, "D2PathPointStrc: layout differs from the database");
static_assert(sizeof(D2DynamicPathStrc) == 512, "D2DynamicPathStrc: layout differs from the database");
static_assert(sizeof(D2ElemTypesTxt) == 4, "D2ElemTypesTxt: layout differs from the database");
static_assert(sizeof(D2ElixirStatLabelStrc) == 16, "D2ElixirStatLabelStrc: layout differs from the database");
static_assert(sizeof(D2EventsTxt) == 2, "D2EventsTxt: layout differs from the database");
static_assert(sizeof(D2ExperienceTxt) == 32, "D2ExperienceTxt: layout differs from the database");
static_assert(sizeof(D2ExperienceDataTbl) == 32, "D2ExperienceDataTbl: layout differs from the database");
static_assert(sizeof(D2FP32_16PrecisionStrc) == 8, "D2FP32_16PrecisionStrc: layout differs from the database");
static_assert(sizeof(D2FP32_16WordsStrc) == 8, "D2FP32_16WordsStrc: layout differs from the database");
static_assert(sizeof(D2FieldStrc) == 8, "D2FieldStrc: layout differs from the database");
static_assert(sizeof(D2FloorDrawCtxStrc) == 72, "D2FloorDrawCtxStrc: layout differs from the database");
static_assert(sizeof(D2FogSystemInfoStrc) == 16, "D2FogSystemInfoStrc: layout differs from the database");
static_assert(sizeof(D2GSPacketClt15) == 275, "D2GSPacketClt15: layout differs from the database");
static_assert(sizeof(D2GSPacketClt5D) == 7, "D2GSPacketClt5D: layout differs from the database");
static_assert(sizeof(D2GSPacketClt5E) == 6, "D2GSPacketClt5E: layout differs from the database");
static_assert(sizeof(D2GSPacketClt66) == 46, "D2GSPacketClt66: layout differs from the database");
static_assert(sizeof(D2GSPacketClt67) == 29, "D2GSPacketClt67: layout differs from the database");
static_assert(sizeof(D2GSPacketClt68) == 1, "D2GSPacketClt68: layout differs from the database");
static_assert(sizeof(D2GSPacketClt6A) == 1, "D2GSPacketClt6A: layout differs from the database");
static_assert(sizeof(D2GSPacketClt6C) == 9, "D2GSPacketClt6C: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv01) == 8, "D2GSPacketSrv01: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv02) == 1, "D2GSPacketSrv02: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv03) == 12, "D2GSPacketSrv03: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv04) == 1, "D2GSPacketSrv04: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv05) == 1, "D2GSPacketSrv05: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv06) == 1, "D2GSPacketSrv06: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv07) == 6, "D2GSPacketSrv07: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv08) == 6, "D2GSPacketSrv08: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv09) == 11, "D2GSPacketSrv09: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv0A) == 6, "D2GSPacketSrv0A: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv0B) == 6, "D2GSPacketSrv0B: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv0C) == 9, "D2GSPacketSrv0C: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv0D) == 13, "D2GSPacketSrv0D: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv0E) == 12, "D2GSPacketSrv0E: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv0F) == 16, "D2GSPacketSrv0F: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv10) == 16, "D2GSPacketSrv10: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv11) == 8, "D2GSPacketSrv11: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv15) == 11, "D2GSPacketSrv15: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv16) == 499, "D2GSPacketSrv16: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv18) == 15, "D2GSPacketSrv18: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv19) == 2, "D2GSPacketSrv19: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv1A) == 2, "D2GSPacketSrv1A: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv1B) == 3, "D2GSPacketSrv1B: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv1C) == 5, "D2GSPacketSrv1C: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv1D) == 3, "D2GSPacketSrv1D: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv1E) == 4, "D2GSPacketSrv1E: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv1F) == 6, "D2GSPacketSrv1F: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv20) == 10, "D2GSPacketSrv20: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv21) == 12, "D2GSPacketSrv21: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv22) == 12, "D2GSPacketSrv22: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv23) == 13, "D2GSPacketSrv23: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv26) == 282, "D2GSPacketSrv26: layout differs from the database");
static_assert(sizeof(D2MessageStrc) == 4, "D2MessageStrc: layout differs from the database");
static_assert(sizeof(D2MessageListStrc) == 34, "D2MessageListStrc: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv27) == 40, "D2GSPacketSrv27: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv28) == 103, "D2GSPacketSrv28: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv29) == 97, "D2GSPacketSrv29: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv2A) == 15, "D2GSPacketSrv2A: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv2C) == 8, "D2GSPacketSrv2C: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv3E) == 34, "D2GSPacketSrv3E: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv3F) == 8, "D2GSPacketSrv3F: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv40) == 13, "D2GSPacketSrv40: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv42) == 6, "D2GSPacketSrv42: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv47) == 11, "D2GSPacketSrv47: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv48) == 11, "D2GSPacketSrv48: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv4C) == 16, "D2GSPacketSrv4C: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv4D) == 17, "D2GSPacketSrv4D: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv4E) == 7, "D2GSPacketSrv4E: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv4F) == 1, "D2GSPacketSrv4F: layout differs from the database");
static_assert(sizeof(D2Srv50A1Q4CainPayloadStrc) == 12, "D2Srv50A1Q4CainPayloadStrc: layout differs from the database");
static_assert(sizeof(D2Srv50A5Q6BaalPayloadStrc) == 12, "D2Srv50A5Q6BaalPayloadStrc: layout differs from the database");
static_assert(sizeof(D2Srv50HirelingPayloadStrc) == 12, "D2Srv50HirelingPayloadStrc: layout differs from the database");
static_assert(sizeof(D2Srv50QuestsStatusPayloadStrc) == 12, "D2Srv50QuestsStatusPayloadStrc: layout differs from the database");
static_assert(sizeof(D2Srv50TombOffsetPayloadStrc) == 12, "D2Srv50TombOffsetPayloadStrc: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv50) == 15, "D2GSPacketSrv50: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv51) == 14, "D2GSPacketSrv51: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv52) == 42, "D2GSPacketSrv52: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv53) == 10, "D2GSPacketSrv53: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv54) == 3, "D2GSPacketSrv54: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv57) == 14, "D2GSPacketSrv57: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv58) == 7, "D2GSPacketSrv58: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv59) == 26, "D2GSPacketSrv59: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv5A) == 40, "D2GSPacketSrv5A: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv5B) == 290, "D2GSPacketSrv5B: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv5C) == 5, "D2GSPacketSrv5C: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv5D) == 6, "D2GSPacketSrv5D: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv5E) == 38, "D2GSPacketSrv5E: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv5F) == 5, "D2GSPacketSrv5F: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv60) == 7, "D2GSPacketSrv60: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv61) == 2, "D2GSPacketSrv61: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv62) == 7, "D2GSPacketSrv62: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv63) == 21, "D2GSPacketSrv63: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv65) == 7, "D2GSPacketSrv65: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv67) == 16, "D2GSPacketSrv67: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv68) == 21, "D2GSPacketSrv68: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv69) == 12, "D2GSPacketSrv69: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv6A) == 12, "D2GSPacketSrv6A: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv6B) == 16, "D2GSPacketSrv6B: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv6C) == 16, "D2GSPacketSrv6C: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv6D) == 10, "D2GSPacketSrv6D: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv73) == 32, "D2GSPacketSrv73: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv74) == 10, "D2GSPacketSrv74: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv75) == 13, "D2GSPacketSrv75: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv76) == 6, "D2GSPacketSrv76: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv77) == 2, "D2GSPacketSrv77: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv78) == 21, "D2GSPacketSrv78: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv79) == 6, "D2GSPacketSrv79: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv7A) == 13, "D2GSPacketSrv7A: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv7B) == 8, "D2GSPacketSrv7B: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv7C) == 6, "D2GSPacketSrv7C: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv7D) == 18, "D2GSPacketSrv7D: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv7E) == 5, "D2GSPacketSrv7E: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv7F) == 10, "D2GSPacketSrv7F: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv81) == 20, "D2GSPacketSrv81: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv82) == 29, "D2GSPacketSrv82: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv89) == 2, "D2GSPacketSrv89: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv8A) == 6, "D2GSPacketSrv8A: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv8B) == 6, "D2GSPacketSrv8B: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv8C) == 11, "D2GSPacketSrv8C: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv8D) == 7, "D2GSPacketSrv8D: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv8E) == 10, "D2GSPacketSrv8E: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv8F) == 33, "D2GSPacketSrv8F: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv90) == 13, "D2GSPacketSrv90: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv91) == 26, "D2GSPacketSrv91: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv92) == 6, "D2GSPacketSrv92: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv93) == 8, "D2GSPacketSrv93: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv94__SkillInfo) == 3, "D2GSPacketSrv94__SkillInfo: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv94) == 771, "D2GSPacketSrv94: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv95) == 13, "D2GSPacketSrv95: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv96) == 9, "D2GSPacketSrv96: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv97) == 1, "D2GSPacketSrv97: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv98) == 7, "D2GSPacketSrv98: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv99) == 16, "D2GSPacketSrv99: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv9A) == 17, "D2GSPacketSrv9A: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv9B) == 7, "D2GSPacketSrv9B: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv9C) == 252, "D2GSPacketSrv9C: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv9D) == 257, "D2GSPacketSrv9D: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv9E) == 7, "D2GSPacketSrv9E: layout differs from the database");
static_assert(sizeof(D2GSPacketSrv9F) == 8, "D2GSPacketSrv9F: layout differs from the database");
static_assert(sizeof(D2GSPacketSrvA0) == 10, "D2GSPacketSrvA0: layout differs from the database");
static_assert(sizeof(D2GSPacketSrvA1) == 7, "D2GSPacketSrvA1: layout differs from the database");
static_assert(sizeof(D2GSPacketSrvA2) == 8, "D2GSPacketSrvA2: layout differs from the database");
static_assert(sizeof(D2GSPacketSrvA3) == 24, "D2GSPacketSrvA3: layout differs from the database");
static_assert(sizeof(D2GSPacketSrvA4) == 3, "D2GSPacketSrvA4: layout differs from the database");
static_assert(sizeof(D2GSPacketSrvA5) == 8, "D2GSPacketSrvA5: layout differs from the database");
static_assert(sizeof(D2PropertyStrc) == 16, "D2PropertyStrc: layout differs from the database");
static_assert(sizeof(D2RunesTxt) == 288, "D2RunesTxt: layout differs from the database");
static_assert(sizeof(D2GSPacketSrvA6) == 294, "D2GSPacketSrvA6: layout differs from the database");
static_assert(sizeof(D2GSPacketSrvA7) == 7, "D2GSPacketSrvA7: layout differs from the database");
static_assert(sizeof(D2GSPacketSrvA8) == 520, "D2GSPacketSrvA8: layout differs from the database");
static_assert(sizeof(D2GSPacketSrvA9) == 7, "D2GSPacketSrvA9: layout differs from the database");
static_assert(sizeof(D2GSPacketSrvAA) == 519, "D2GSPacketSrvAA: layout differs from the database");
static_assert(sizeof(D2GSPacketSrvAB) == 7, "D2GSPacketSrvAB: layout differs from the database");
static_assert(sizeof(D2GSPacketSrvAC) == 257, "D2GSPacketSrvAC: layout differs from the database");
static_assert(sizeof(D2GSPacketSrvAD) == 9, "D2GSPacketSrvAD: layout differs from the database");
static_assert(sizeof(D2GemsTxt) == 192, "D2GemsTxt: layout differs from the database");
static_assert(sizeof(D2GemsTxtCodeUnion) == 4, "D2GemsTxtCodeUnion: layout differs from the database");
static_assert(sizeof(D2GfxDataStrc) == 72, "D2GfxDataStrc: layout differs from the database");
static_assert(sizeof(D2HireDescTxt) == 4, "D2HireDescTxt: layout differs from the database");
static_assert(sizeof(D2HirelingInitStrc) == 58, "D2HirelingInitStrc: layout differs from the database");
static_assert(sizeof(D2HirelingTxt) == 280, "D2HirelingTxt: layout differs from the database");
static_assert(sizeof(D2HitClassSoundStrc) == 8, "D2HitClassSoundStrc: layout differs from the database");
static_assert(sizeof(D2HitClassTxt) == 4, "D2HitClassTxt: layout differs from the database");
static_assert(sizeof(D2HoverTextStrc) == 272, "D2HoverTextStrc: layout differs from the database");
static_assert(sizeof(D2InteractInfoStrc) == 12, "D2InteractInfoStrc: layout differs from the database");
static_assert(sizeof(D2InvButtonInfoStrc) == 20, "D2InvButtonInfoStrc: layout differs from the database");
static_assert(sizeof(D2InvCompGridStrc) == 20, "D2InvCompGridStrc: layout differs from the database");
static_assert(sizeof(D2InventoryGridInfoStrc) == 24, "D2InventoryGridInfoStrc: layout differs from the database");
static_assert(sizeof(D2InventoryGridStrc) == 16, "D2InventoryGridStrc: layout differs from the database");
static_assert(sizeof(D2InventoryNodeStrc) == 8, "D2InventoryNodeStrc: layout differs from the database");
static_assert(sizeof(D2InventoryStrc) == 64, "D2InventoryStrc: layout differs from the database");
static_assert(sizeof(D2InventoryTxt) == 240, "D2InventoryTxt: layout differs from the database");
static_assert(sizeof(D2ItemDataPageUnion) == 1, "D2ItemDataPageUnion: layout differs from the database");
static_assert(sizeof(D2ItemExtraDataStrc) == 24, "D2ItemExtraDataStrc: layout differs from the database");
static_assert(sizeof(D2ItemDataStrc) == 116, "D2ItemDataStrc: layout differs from the database");
static_assert(sizeof(D2ItemRatioTxt) == 68, "D2ItemRatioTxt: layout differs from the database");
static_assert(sizeof(D2ItemSaveStrc) == 24, "D2ItemSaveStrc: layout differs from the database");
static_assert(sizeof(D2OpStatDataStrc) == 6, "D2OpStatDataStrc: layout differs from the database");
static_assert(sizeof(D2ItemStatCostTxt) == 324, "D2ItemStatCostTxt: layout differs from the database");
static_assert(sizeof(D2ItemTradeStrc) == 12, "D2ItemTradeStrc: layout differs from the database");
static_assert(sizeof(D2ItemTypesTxt) == 228, "D2ItemTypesTxt: layout differs from the database");
static_assert(sizeof(D2ItemsTxt) == 424, "D2ItemsTxt: layout differs from the database");
static_assert(sizeof(D2ItemsTxtCodeUnion) == 4, "D2ItemsTxtCodeUnion: layout differs from the database");
static_assert(sizeof(D2LeapArcBandStrc) == 12, "D2LeapArcBandStrc: layout differs from the database");
static_assert(sizeof(D2LevelDefBin) == 156, "D2LevelDefBin: layout differs from the database");
static_assert(sizeof(D2LevelsTxt) == 544, "D2LevelsTxt: layout differs from the database");
static_assert(sizeof(D2LightGridCellStrc) == 8, "D2LightGridCellStrc: layout differs from the database");
static_assert(sizeof(D2ListBoxImplVtbl) == 64, "D2ListBoxImplVtbl: layout differs from the database");
static_assert(sizeof(D2ListBoxProxyVtbl) == 64, "D2ListBoxProxyVtbl: layout differs from the database");
static_assert(sizeof(D2LowQualityItemsTxt) == 34, "D2LowQualityItemsTxt: layout differs from the database");
static_assert(sizeof(D2LvlMazeTxt) == 28, "D2LvlMazeTxt: layout differs from the database");
static_assert(sizeof(D2LvlPrestTxt) == 432, "D2LvlPrestTxt: layout differs from the database");
static_assert(sizeof(D2LvlSubTxt) == 348, "D2LvlSubTxt: layout differs from the database");
static_assert(sizeof(D2LvlTypesTxt) == 1928, "D2LvlTypesTxt: layout differs from the database");
static_assert(sizeof(D2LvlWarpTxt) == 48, "D2LvlWarpTxt: layout differs from the database");
static_assert(sizeof(D2MagicAffixTxt) == 144, "D2MagicAffixTxt: layout differs from the database");
static_assert(sizeof(D2MapAIPathPositionStrc) == 12, "D2MapAIPathPositionStrc: layout differs from the database");
static_assert(sizeof(D2MapAIStrc) == 8, "D2MapAIStrc: layout differs from the database");
static_assert(sizeof(D2MercPanelLabelStrc) == 14, "D2MercPanelLabelStrc: layout differs from the database");
static_assert(sizeof(D2MercStatRowStrc) == 20, "D2MercStatRowStrc: layout differs from the database");
static_assert(sizeof(TSLink) == 8, "TSLink: layout differs from the database");
static_assert(sizeof(D2MessageSourceStrc) == 12, "D2MessageSourceStrc: layout differs from the database");
static_assert(sizeof(D2MissCalcTxt) == 4, "D2MissCalcTxt: layout differs from the database");
static_assert(sizeof(D2MissileDataStrc) == 52, "D2MissileDataStrc: layout differs from the database");
static_assert(sizeof(D2MissileFxStageStrc) == 12, "D2MissileFxStageStrc: layout differs from the database");
static_assert(sizeof(D2MissileStrc) == 92, "D2MissileStrc: layout differs from the database");
static_assert(sizeof(D2MissileStreamStrc) == 8, "D2MissileStreamStrc: layout differs from the database");
static_assert(sizeof(D2MissileTargetPosStrc) == 8, "D2MissileTargetPosStrc: layout differs from the database");
static_assert(sizeof(D2MissileTargetUnion) == 8, "D2MissileTargetUnion: layout differs from the database");
static_assert(sizeof(D2MissileUnitFindTableStrc) == 8, "D2MissileUnitFindTableStrc: layout differs from the database");
static_assert(sizeof(D2MissilesTxt) == 420, "D2MissilesTxt: layout differs from the database");
static_assert(sizeof(D2ModStatsArrayStrc) == 8, "D2ModStatsArrayStrc: layout differs from the database");
static_assert(sizeof(D2MonAiTxt) == 2, "D2MonAiTxt: layout differs from the database");
static_assert(sizeof(D2MonEquipTxt) == 28, "D2MonEquipTxt: layout differs from the database");
static_assert(sizeof(D2MonItemPercentTxt) == 4, "D2MonItemPercentTxt: layout differs from the database");
static_assert(sizeof(D2MonLvlTxt) == 120, "D2MonLvlTxt: layout differs from the database");
static_assert(sizeof(D2MonModeTxt) == 52, "D2MonModeTxt: layout differs from the database");
static_assert(sizeof(D2MonModeTxtStub) == 4, "D2MonModeTxtStub: layout differs from the database");
static_assert(sizeof(D2MonPlaceTxt) == 2, "D2MonPlaceTxt: layout differs from the database");
static_assert(sizeof(D2MonPresetTxt) == 4, "D2MonPresetTxt: layout differs from the database");
static_assert(sizeof(D2MonPropTxt) == 312, "D2MonPropTxt: layout differs from the database");
static_assert(sizeof(D2MonSoundsTxt) == 148, "D2MonSoundsTxt: layout differs from the database");
static_assert(sizeof(D2MonStats2ChoiceCountsStrc) == 16, "D2MonStats2ChoiceCountsStrc: layout differs from the database");
static_assert(sizeof(D2MonStats2ChoiceCountsUnion) == 16, "D2MonStats2ChoiceCountsUnion: layout differs from the database");
static_assert(sizeof(D2MonStats2FlagsUnion) == 4, "D2MonStats2FlagsUnion: layout differs from the database");
static_assert(sizeof(D2MonStats2ModeFlagsUnion) == 4, "D2MonStats2ModeFlagsUnion: layout differs from the database");
static_assert(sizeof(D2UnkMonCompStrc) == 12, "D2UnkMonCompStrc: layout differs from the database");
static_assert(sizeof(D2MonStats2Txt) == 308, "D2MonStats2Txt: layout differs from the database");
static_assert(sizeof(D2MonStatsFlagsUnion) == 4, "D2MonStatsFlagsUnion: layout differs from the database");
static_assert(sizeof(D2MonStatsInitStrc) == 56, "D2MonStatsInitStrc: layout differs from the database");
static_assert(sizeof(D2MonStatsTxt) == 424, "D2MonStatsTxt: layout differs from the database");
static_assert(sizeof(D2MonTypeTxt) == 12, "D2MonTypeTxt: layout differs from the database");
static_assert(sizeof(D2MonUModTxt) == 32, "D2MonUModTxt: layout differs from the database");
static_assert(sizeof(D2MonsterDataAiParamUnion) == 4, "D2MonsterDataAiParamUnion: layout differs from the database");
static_assert(sizeof(D2MonsterDataInteractUnion) == 4, "D2MonsterDataInteractUnion: layout differs from the database");
static_assert(sizeof(D2UnkMonsterDataStrc) == 16, "D2UnkMonsterDataStrc: layout differs from the database");
static_assert(sizeof(D2MonsterDataStrc) == 96, "D2MonsterDataStrc: layout differs from the database");
static_assert(sizeof(D2MonsterInteractStrc) == 4, "D2MonsterInteractStrc: layout differs from the database");
static_assert(sizeof(D2NpcHirelingOfferStrc) == 16, "D2NpcHirelingOfferStrc: layout differs from the database");
static_assert(sizeof(D2NpcMenuDescStrc) == 39, "D2NpcMenuDescStrc: layout differs from the database");
static_assert(sizeof(D2NpcMenuEntryStrc) == 22, "D2NpcMenuEntryStrc: layout differs from the database");
static_assert(sizeof(D2NpcMenuQuestUnlockStrc) == 20, "D2NpcMenuQuestUnlockStrc: layout differs from the database");
static_assert(sizeof(D2NpcMenuUIStateStrc) == 348, "D2NpcMenuUIStateStrc: layout differs from the database");
static_assert(sizeof(D2NpcSpeechLineStrc) == 15, "D2NpcSpeechLineStrc: layout differs from the database");
static_assert(sizeof(D2NpcSpeechPairStrc) == 8, "D2NpcSpeechPairStrc: layout differs from the database");
static_assert(sizeof(D2NpcTxt) == 76, "D2NpcTxt: layout differs from the database");
static_assert(sizeof(D2ObjGroupTxt) == 52, "D2ObjGroupTxt: layout differs from the database");
static_assert(sizeof(D2ObjModeTypeTxt) == 52, "D2ObjModeTypeTxt: layout differs from the database");
static_assert(sizeof(D2ObjectDataStrc) == 56, "D2ObjectDataStrc: layout differs from the database");
static_assert(sizeof(D2ObjectsTxt) == 448, "D2ObjectsTxt: layout differs from the database");
static_assert(sizeof(D2OverlayTxt) == 132, "D2OverlayTxt: layout differs from the database");
static_assert(sizeof(D2PacketListStrc) == 16, "D2PacketListStrc: layout differs from the database");
static_assert(sizeof(D2PanelKeyBindStrc) == 36, "D2PanelKeyBindStrc: layout differs from the database");
static_assert(sizeof(D2PetInfoStrc) == 12, "D2PetInfoStrc: layout differs from the database");
static_assert(sizeof(D2PetDataStrc) == 24, "D2PetDataStrc: layout differs from the database");
static_assert(sizeof(D2PetListStrc) == 12, "D2PetListStrc: layout differs from the database");
static_assert(sizeof(D2PetTypeTxt) == 224, "D2PetTypeTxt: layout differs from the database");
static_assert(sizeof(D2PlayerClassTxt) == 4, "D2PlayerClassTxt: layout differs from the database");
static_assert(sizeof(D2PlayerDataStrc) == 364, "D2PlayerDataStrc: layout differs from the database");
static_assert(sizeof(D2PlayerListStrc) == 20, "D2PlayerListStrc: layout differs from the database");
static_assert(sizeof(D2PlayerPetStrc) == 4, "D2PlayerPetStrc: layout differs from the database");
static_assert(sizeof(D2PlayerTradeStrc) == 24, "D2PlayerTradeStrc: layout differs from the database");
static_assert(sizeof(D2PlrIntroStrc) == 8, "D2PlrIntroStrc: layout differs from the database");
static_assert(sizeof(D2PlrModeTxtStub) == 4, "D2PlrModeTxtStub: layout differs from the database");
static_assert(sizeof(D2PlrModeTypeTxt) == 52, "D2PlrModeTypeTxt: layout differs from the database");
static_assert(sizeof(D2PopupStrc) == 4, "D2PopupStrc: layout differs from the database");
static_assert(sizeof(D2PresetUnitStrc) == 32, "D2PresetUnitStrc: layout differs from the database");
static_assert(sizeof(D2PropertiesTxt) == 46, "D2PropertiesTxt: layout differs from the database");
static_assert(sizeof(D2QtyWarnLevelStrc) == 20, "D2QtyWarnLevelStrc: layout differs from the database");
static_assert(sizeof(D2QualityItemsTxt) == 112, "D2QualityItemsTxt: layout differs from the database");
static_assert(sizeof(D2QuestLogCloseRectStrc) == 20, "D2QuestLogCloseRectStrc: layout differs from the database");
static_assert(sizeof(D2QuestLogEntryStrc) == 618, "D2QuestLogEntryStrc: layout differs from the database");
static_assert(sizeof(D2QuestLogQuestDefStrc) == 16, "D2QuestLogQuestDefStrc: layout differs from the database");
static_assert(sizeof(D2QuestLogTextPairStrc) == 4, "D2QuestLogTextPairStrc: layout differs from the database");
static_assert(sizeof(D2QuestLogTextBlockStrc) == 64, "D2QuestLogTextBlockStrc: layout differs from the database");
static_assert(sizeof(D2QuestMenuStringStrc) == 4, "D2QuestMenuStringStrc: layout differs from the database");
static_assert(sizeof(D2RareAffixTxt) == 72, "D2RareAffixTxt: layout differs from the database");
static_assert(sizeof(D2RoomCollisionGridStrc) == 36, "D2RoomCollisionGridStrc: layout differs from the database");
static_assert(sizeof(D2RoomCoordListStrc) == 48, "D2RoomCoordListStrc: layout differs from the database");
static_assert(sizeof(D2RoomTileStrc) == 24, "D2RoomTileStrc: layout differs from the database");
static_assert(sizeof(D2RosterCorpseStrc) == 8, "D2RosterCorpseStrc: layout differs from the database");
static_assert(sizeof(D2RosterGroupStrc) == 12, "D2RosterGroupStrc: layout differs from the database");
static_assert(sizeof(D2RosterInfoStrc) == 12, "D2RosterInfoStrc: layout differs from the database");
static_assert(sizeof(D2RosterPetStrc) == 52, "D2RosterPetStrc: layout differs from the database");
static_assert(sizeof(D2RosterUnitStrc) == 132, "D2RosterUnitStrc: layout differs from the database");
static_assert(sizeof(D2SCmdStrc) == 12, "D2SCmdStrc: layout differs from the database");
static_assert(sizeof(D2SLayerStatIdPartsStrc) == 4, "D2SLayerStatIdPartsStrc: layout differs from the database");
static_assert(sizeof(D2SLayerStatIdStrc) == 4, "D2SLayerStatIdStrc: layout differs from the database");
static_assert(sizeof(D2SLayerStatIdUnion) == 4, "D2SLayerStatIdUnion: layout differs from the database");
static_assert(sizeof(D2SeedPartsStrc) == 8, "D2SeedPartsStrc: layout differs from the database");
static_assert(sizeof(D2SeedUnion) == 8, "D2SeedUnion: layout differs from the database");
static_assert(sizeof(D2SetItemsTxt) == 440, "D2SetItemsTxt: layout differs from the database");
static_assert(sizeof(D2SetsTxt) == 296, "D2SetsTxt: layout differs from the database");
static_assert(sizeof(D2ShadowLightGammaPaletteStrc) == 12544, "D2ShadowLightGammaPaletteStrc: layout differs from the database");
static_assert(sizeof(D2ShrinesTxt) == 184, "D2ShrinesTxt: layout differs from the database");
static_assert(sizeof(D2SkillCalcTxt) == 4, "D2SkillCalcTxt: layout differs from the database");
static_assert(sizeof(D2SkillDescTxt) == 288, "D2SkillDescTxt: layout differs from the database");
static_assert(sizeof(D2SkillListStrc) == 24, "D2SkillListStrc: layout differs from the database");
static_assert(sizeof(D2SkillStrc) == 60, "D2SkillStrc: layout differs from the database");
static_assert(sizeof(D2SkillTargetGUIDUnion) == 4, "D2SkillTargetGUIDUnion: layout differs from the database");
static_assert(sizeof(D2SkillTargetXUnion) == 4, "D2SkillTargetXUnion: layout differs from the database");
static_assert(sizeof(D2SkillTargetYUnion) == 4, "D2SkillTargetYUnion: layout differs from the database");
static_assert(sizeof(D2SkillsTxt) == 572, "D2SkillsTxt: layout differs from the database");
static_assert(sizeof(D2SoundChannelCacheStrc) == 20, "D2SoundChannelCacheStrc: layout differs from the database");
static_assert(sizeof(D2SoundChannelStrc) == 32, "D2SoundChannelStrc: layout differs from the database");
static_assert(sizeof(D2SoundEnvironTxt) == 88, "D2SoundEnvironTxt: layout differs from the database");
static_assert(sizeof(D2SoundInstanceStrc) == 89, "D2SoundInstanceStrc: layout differs from the database");
static_assert(sizeof(D2SoundListNodeStrc) == 8, "D2SoundListNodeStrc: layout differs from the database");
static_assert(sizeof(D2SoundsTxt) == 142, "D2SoundsTxt: layout differs from the database");
static_assert(sizeof(D2SoundsTxtStub) == 2, "D2SoundsTxtStub: layout differs from the database");
static_assert(sizeof(D2SpellSelPanelStrc) == 965, "D2SpellSelPanelStrc: layout differs from the database");
static_assert(sizeof(D2SplittedTextStrc) == 8, "D2SplittedTextStrc: layout differs from the database");
static_assert(sizeof(D2StatsArrayStrc) == 8, "D2StatsArrayStrc: layout differs from the database");
static_assert(sizeof(D2StatListStrc) == 60, "D2StatListStrc: layout differs from the database");
static_assert(sizeof(D2StatListExStrc) == 100, "D2StatListExStrc: layout differs from the database");
static_assert(sizeof(D2StatStrc) == 8, "D2StatStrc: layout differs from the database");
static_assert(sizeof(D2StatesTxt) == 60, "D2StatesTxt: layout differs from the database");
static_assert(sizeof(D2StatesTxtFlagsUnion) == 4, "D2StatesTxtFlagsUnion: layout differs from the database");
static_assert(sizeof(D2StaticPathStrc) == 32, "D2StaticPathStrc: layout differs from the database");
static_assert(sizeof(D2StorePageTxt) == 4, "D2StorePageTxt: layout differs from the database");
static_assert(sizeof(D2SuperUniquesTxt) == 52, "D2SuperUniquesTxt: layout differs from the database");
static_assert(sizeof(D2TCExInfoStrc) == 28, "D2TCExInfoStrc: layout differs from the database");
static_assert(sizeof(D2TCExShortStrc) == 44, "D2TCExShortStrc: layout differs from the database");
static_assert(sizeof(D2TextDisplayListStrc) == 6, "D2TextDisplayListStrc: layout differs from the database");
static_assert(sizeof(D2TextHeaderStrc) == 12, "D2TextHeaderStrc: layout differs from the database");
static_assert(sizeof(D2TextNodeStrc) == 12, "D2TextNodeStrc: layout differs from the database");
static_assert(sizeof(D2TextStateStrc) == 159, "D2TextStateStrc: layout differs from the database");
static_assert(sizeof(D2TileLibraryEntryStrc) == 80, "D2TileLibraryEntryStrc: layout differs from the database");
static_assert(sizeof(D2TileLibraryHashNodeStrc) == 20, "D2TileLibraryHashNodeStrc: layout differs from the database");
static_assert(sizeof(D2TileLibraryHashRefStrc) == 8, "D2TileLibraryHashRefStrc: layout differs from the database");
static_assert(sizeof(D2TileLibraryHashStrc) == 512, "D2TileLibraryHashStrc: layout differs from the database");
static_assert(sizeof(D2TileRecordStrc) == 272, "D2TileRecordStrc: layout differs from the database");
static_assert(sizeof(D2TxtLinkCodeUnion) == 4, "D2TxtLinkCodeUnion: layout differs from the database");
static_assert(sizeof(D2TxtLinkNodeStrc) == 44, "D2TxtLinkNodeStrc: layout differs from the database");
static_assert(sizeof(D2TxtLinkStrc) == 16, "D2TxtLinkStrc: layout differs from the database");
static_assert(sizeof(D2TxtLinkTblStrc) == 8, "D2TxtLinkTblStrc: layout differs from the database");
static_assert(sizeof(D2UiButtonImageStrc) == 4, "D2UiButtonImageStrc: layout differs from the database");
static_assert(sizeof(D2UiButtonImplVtbl) == 52, "D2UiButtonImplVtbl: layout differs from the database");
static_assert(sizeof(D2UiButtonProxyVtbl) == 52, "D2UiButtonProxyVtbl: layout differs from the database");
static_assert(sizeof(D2UiControlBaseVtbl) == 48, "D2UiControlBaseVtbl: layout differs from the database");
static_assert(sizeof(D2UiControlProxyVtbl) == 76, "D2UiControlProxyVtbl: layout differs from the database");
static_assert(sizeof(D2UiEditBoxImplVtbl) == 76, "D2UiEditBoxImplVtbl: layout differs from the database");
static_assert(sizeof(D2UiSpinnerImplVtbl) == 52, "D2UiSpinnerImplVtbl: layout differs from the database");
static_assert(sizeof(D2UiSpinnerProxyVtbl) == 52, "D2UiSpinnerProxyVtbl: layout differs from the database");
static_assert(sizeof(D2UniqueAppellationTxt) == 2, "D2UniqueAppellationTxt: layout differs from the database");
static_assert(sizeof(D2UniqueItemsTxt) == 332, "D2UniqueItemsTxt: layout differs from the database");
static_assert(sizeof(D2UniquePrefixTxt) == 2, "D2UniquePrefixTxt: layout differs from the database");
static_assert(sizeof(D2UniqueSuffixTxt) == 2, "D2UniqueSuffixTxt: layout differs from the database");
static_assert(sizeof(D2UniqueTitleTxt) == 2, "D2UniqueTitleTxt: layout differs from the database");
static_assert(sizeof(D2UnitClientStrc) == 48, "D2UnitClientStrc: layout differs from the database");
static_assert(sizeof(D2UnitDataUnion) == 4, "D2UnitDataUnion: layout differs from the database");
static_assert(sizeof(D2UnitEventStrc) == 32, "D2UnitEventStrc: layout differs from the database");
static_assert(sizeof(D2UnitFindArgStrc) == 56, "D2UnitFindArgStrc: layout differs from the database");
static_assert(sizeof(D2UnitFindDataStrc) == 44, "D2UnitFindDataStrc: layout differs from the database");
static_assert(sizeof(D2UnitGfxDataCopyUnion) == 4, "D2UnitGfxDataCopyUnion: layout differs from the database");
static_assert(sizeof(D2UnitModeUnion) == 4, "D2UnitModeUnion: layout differs from the database");
static_assert(sizeof(D2UnitPacketListStrc) == 5, "D2UnitPacketListStrc: layout differs from the database");
static_assert(sizeof(D2UnitPathUnion) == 4, "D2UnitPathUnion: layout differs from the database");
static_assert(sizeof(D2UnitServerClientUnion) == 48, "D2UnitServerClientUnion: layout differs from the database");
static_assert(sizeof(D2UnitServerStrc) == 48, "D2UnitServerStrc: layout differs from the database");
static_assert(sizeof(D2UnitStrc) == 244, "D2UnitStrc: layout differs from the database");
static_assert(sizeof(D2UnitTickOrPacketsUnion) == 4, "D2UnitTickOrPacketsUnion: layout differs from the database");
static_assert(sizeof(D2UnkMonsterDataLifeUnion) == 4, "D2UnkMonsterDataLifeUnion: layout differs from the database");
static_assert(sizeof(D2UnkOutdoorStrc2) == 60, "D2UnkOutdoorStrc2: layout differs from the database");
static_assert(sizeof(D2UnkPlayerDataStrc) == 20, "D2UnkPlayerDataStrc: layout differs from the database");
static_assert(sizeof(D2WaypointCoordTableStrc) == 24, "D2WaypointCoordTableStrc: layout differs from the database");
static_assert(sizeof(D2WaypointDataStrc) == 16, "D2WaypointDataStrc: layout differs from the database");
static_assert(sizeof(D2WaypointTableStrc) == 5, "D2WaypointTableStrc: layout differs from the database");
static_assert(sizeof(D2WeaponDescStrc) == 6, "D2WeaponDescStrc: layout differs from the database");
static_assert(sizeof(D2WinCommandRegisterStrc) == 12, "D2WinCommandRegisterStrc: layout differs from the database");
static_assert(sizeof(D2WindowProcCallbackStrc) == 12, "D2WindowProcCallbackStrc: layout differs from the database");
static_assert(sizeof(FOGASTNodeStrc) == 1, "FOGASTNodeStrc: layout differs from the database");
static_assert(sizeof(HotkeyMappingEntry) == 10, "HotkeyMappingEntry: layout differs from the database");
static_assert(sizeof(KeyFunctions) == 12, "KeyFunctions: layout differs from the database");
static_assert(sizeof(QUESTLOGBUTTONPOSITIONT) == 16, "QUESTLOGBUTTONPOSITIONT: layout differs from the database");
static_assert(sizeof(SCmdArg) == 16, "SCmdArg: layout differs from the database");
static_assert(sizeof(SGameDataChunkStrc) == 24, "SGameDataChunkStrc: layout differs from the database");
static_assert(sizeof(SGameDataNodeStrc) == 20, "SGameDataNodeStrc: layout differs from the database");
static_assert(sizeof(TSExplicitList) == 12, "TSExplicitList: layout differs from the database");
static_assert(sizeof(TSExplicitListArray) == 16, "TSExplicitListArray: layout differs from the database");
static_assert(sizeof(SGameDataTableStrc) == 104, "SGameDataTableStrc: layout differs from the database");
static_assert(sizeof(SGameDataTableVtbl) == 16, "SGameDataTableVtbl: layout differs from the database");
static_assert(sizeof(SMSGHANDLER_PARAMS) == 32, "SMSGHANDLER_PARAMS: layout differs from the database");
static_assert(sizeof(ptMsgStruct) == 12, "ptMsgStruct: layout differs from the database");
#endif
#endif

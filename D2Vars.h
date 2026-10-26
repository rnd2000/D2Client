#ifdef _D2VARS_H
#define VAR(Type, Name)         Type Name;
#else
#define VAR(Type, Name)         extern Type Name;
#endif

/****************************************************************************
*                                                                           *
*   D2Vars.h                                                                *
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
*   This file is used to declare your own global variables to use           *
*   within your code. These variables can be used anywhere in your code     *
*                                                                           *
*****************************************************************************/


///////////////////////////////////////////////////////////////////////////////
//
//  GENERATED BELOW THIS LINE -- do not edit by hand; regenerate instead.
//
//    python <Tools>/sweep/bridge.py --run D2Client \
//           <Tools>/sweep/ida_dump_globals.py DG_OUT=<dump>.json
//    python <Tools>/sweep/gen_vars_header.py \
//           GV_DUMP=<dump>.json GV_APPLY=1
//
//  Source: the D2Client IDA database of the d2client-sweep repository --
//  Diablo II 1.10 D2Client.dll, image base 0x6FAA0000.
//
//  These are D2VAR, not VAR. The VAR macro above declares one of YOUR OWN
//  variables; D2VAR points at one of the GAME's, computed from the loaded
//  module base. D2Ptrs.h defines it and DLLmain.h includes that first, so the
//  macro is in scope here.
//
//  D2VAR expands TYPE into `typedef TYPE NAME_vt;`, which an array spelling
//  cannot survive -- `typedef unsigned __int8[18] X;` is not C++. So an array
//  global carries its ELEMENT type here and the pointer addresses the first
//  element; the extent the database measured is on the line. 0 of the 0
//  emitted are arrays.
//
//  A FUNCTION POINTER cannot survive it either -- the name belongs inside the
//  parentheses -- so 0 of them are spelled as a `using D2CLIENT_<name>_ft =`
//  alias on the line before, and the D2VAR names the alias. `_ft` is free:
//  D2FUNC makes `_t` and D2VAR makes `_vt`. An ARRAY of function pointers
//  takes both routes at once, the alias for the element and the comment for
//  the extent.
//
//  47 global(s) could not go through the macro and are listed at the end as
//  comments carrying the reason, so nothing is lost from the record.
//
//  1402 global(s) are OURS: the rewrite holds its own object in the module
//  header the line names (RULES.md, "A GLOBAL OF OURS IS WRITTEN IN ITS
//  MODULE'S HEADER"), so the line is COMMENTED IN PLACE -- never deleted --
//  and D2CLIENT_<name> is not declared. The type on it is still the
//  database's, and the generator refuses to write this file while the
//  header's type disagrees with it.
//
//  172 item(s) of the image are not the game's and are not in this file at
//  all -- the user, 2026-10-12: "ссылки на таблицу импорта, CRT и Windows
//  функции сразу убери". 16 of the PE's import directory, 99 import address
//  table slots (the Windows functions, which our code calls directly, and
//  other game DLLs' imports), 57 of the VC6 runtime's data (every instruction
//  that references one lies in a CRT_ function). gen_vars_header.py measures
//  them on the stock DLL and its report names each.
//
//  Ordered by address.
//
///////////////////////////////////////////////////////////////////////////////


// ---- .text ----
// OURS, CUnit.h: D2VAR(D2CLIENT, gnAnimModeSyncClass, const uint8_t, 0x8A6C0)   // [18]
// OURS, UnitMode.h: D2VAR(D2CLIENT, gnClientCmdClass, const uint8_t, 0xAED9C)   // [19]

// ---- .rdata ----
// OURS, CoreArchive.h: D2VAR(D2CLIENT, gpszD2ClientCaption, const LPCSTR, 0xCE0AC)
// OURS, Lng.h: D2VAR(D2CLIENT, gpszLngLibName, const LPCSTR, 0xCE0B0)
// OURS, Lng.h: D2VAR(D2CLIENT, gpszLngInitFailedText, const LPCSTR, 0xCE0B4)
// OURS, Lng.h: D2VAR(D2CLIENT, gpszLngErrorText, const LPCSTR, 0xCE0B8)
// OURS, MemoryPartitions.h: D2VAR(D2CLIENT, gfBytesToMegabytes, const float, 0xCE0BC)
// OURS, WINMAIN.h: D2VAR(D2CLIENT, gpszMsgBoxCaption, const LPCSTR, 0xCE0C0)
// OURS, WINMAIN.h: using D2CLIENT_gafnCommandsToRegister_ft = int32_t (__cdecl *const)();
// OURS, WINMAIN.h: D2VAR(D2CLIENT, gafnCommandsToRegister, D2CLIENT_gafnCommandsToRegister_ft, 0xCE0C8)   // [1]
// OURS, WINMAIN.h: D2VAR(D2CLIENT, gaCommandLineOptions, const SCmdArg, 0xCE0D0)   // [8]
// OURS, WINMAIN.h: D2VAR(D2CLIENT, gpszCmdLineHelpText, const LPCSTR, 0xCE150)
// OURS, Draw.h: D2VAR(D2CLIENT, gfOneOver1024, const float, 0xCE154)
// OURS, Draw.h: D2VAR(D2CLIENT, gfPercentDivisor, const float, 0xCE158)
// OURS, dFloor.h: D2VAR(D2CLIENT, gfConst1000, const float, 0xCE15C)
// OURS, dWall.h: D2VAR(D2CLIENT, gaTileLightCornerCount, const int32_t, 0xCE160)   // [10]
// OURS, Env.h: D2VAR(D2CLIENT, gaEnvSnowTransByPeriod, const uint8_t, 0xCE189)   // [7]
// OURS, Env.h: D2VAR(D2CLIENT, gnSnowShapeSegmentOffsets, const int32_t, 0xCE190)   // [36]
// OURS, Env.h: D2VAR(D2CLIENT, gfOneOver256, const float, 0xCE220)
// OURS, Env.h: D2VAR(D2CLIENT, gfWeatherSmoothOld, const float, 0xCE224)
// OURS, Env.h: D2VAR(D2CLIENT, gfWeatherSmoothNew, const float, 0xCE228)
// OURS, Env.h: D2VAR(D2CLIENT, gfQuarter, const float, 0xCE22C)
// OURS, Env.h: D2VAR(D2CLIENT, gfConstNeg15, const float, 0xCE230)
// OURS, Env.h: D2VAR(D2CLIENT, gfConstNeg8, const float, 0xCE234)
// OURS, Env.h: D2VAR(D2CLIENT, gfConstNeg28, const float, 0xCE238)
// OURS, Game.h: D2VAR(D2CLIENT, gVtblSGameData0, const SGameDataTableVtbl, 0xCE23C)
// OURS, Game.h: D2VAR(D2CLIENT, gVtblSGameData1, const SGameDataTableVtbl, 0xCE24C)
// OURS, Game.h: D2VAR(D2CLIENT, gVtblSGameData2, const SGameDataTableVtbl, 0xCE25C)
// OURS, Select.h: D2VAR(D2CLIENT, gnDir8ProbeX, const int32_t, 0xCE27C)   // [8]
// OURS, Select.h: D2VAR(D2CLIENT, gnDir8ProbeY, const int32_t, 0xCE29C)   // [8]
// OURS, Select.h: D2VAR(D2CLIENT, gnTargetProbeOrder, const int32_t, 0xCE2BC)   // [5]
// OURS, Select.h: D2VAR(D2CLIENT, gnTargetProbeOrderEnd, const int32_t, 0xCE2D0)   // [2]
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gfConst80, const float, 0xCE2D8)
// OURS, Skills.h: D2VAR(D2CLIENT, gnRingOffsetX, const int32_t, 0xCE2E0)   // [64]
// OURS, Skills.h: D2VAR(D2CLIENT, gnRingOffsetY, const int32_t, 0xCE3E0)   // [64]
// OURS, Skills.h: D2VAR(D2CLIENT, gnMissileArrayQuadrantX, const int32_t, 0xCE4E0)   // [4]
// OURS, Skills.h: D2VAR(D2CLIENT, gnMissileArrayQuadrantY, const int32_t, 0xCE4F0)   // [4]
// OURS, Skills.h: D2VAR(D2CLIENT, gnMissileArrayRingA, const int32_t, 0xCE500)   // [7]
// OURS, Skills.h: D2VAR(D2CLIENT, gnMissileArrayRingB, const int32_t, 0xCE51C)   // [7]
// OURS, Skills.h: D2VAR(D2CLIENT, gnMissileArrayAxisX, const int32_t, 0xCE538)   // [8]
// OURS, Skills.h: D2VAR(D2CLIENT, gnMissileArrayAxisY, const int32_t, 0xCE558)   // [8]
// OURS, SkillsAss.h: D2VAR(D2CLIENT, gnDir64CircleX, const int32_t, 0xCE578)   // [64]
// OURS, SkillsAss.h: D2VAR(D2CLIENT, gnDir64CircleY, const int32_t, 0xCE678)   // [64]
// OURS, SkillsAss.h: D2VAR(D2CLIENT, gnDir8StepX, const int32_t, 0xCE77C)   // [8]
// OURS, SkillsAss.h: D2VAR(D2CLIENT, gnDir8StepY, const int32_t, 0xCE79C)   // [8]
// OURS, SkillsAss.h: D2VAR(D2CLIENT, gnRoyalStrikeCircleX, const int32_t, 0xCE7C0)   // [64]
// OURS, SkillsAss.h: D2VAR(D2CLIENT, gnRoyalStrikeCircleY, const int32_t, 0xCE8C0)   // [64]
// OURS, SkillsBar.h: D2VAR(D2CLIENT, gLeapArcTable, const D2LeapArcBandStrc, 0xCE9C4)   // [2]
// OURS, SkillsDru.h: D2VAR(D2CLIENT, gnDir8CircleX, const int32_t, 0xCE9E0)   // [8]
// OURS, SkillsDru.h: D2VAR(D2CLIENT, gnDir8CircleY, const int32_t, 0xCEA00)   // [8]
// OURS, SkillsEMon.h: D2VAR(D2CLIENT, gnImpInfernoSpread, const int32_t, 0xCEA28)   // [6]
// OURS, SkillsEMon.h: D2VAR(D2CLIENT, gnDir8SignX, const int32_t, 0xCEA40)   // [8]
// OURS, SkillsEMon.h: D2VAR(D2CLIENT, gnDir8SignY, const int32_t, 0xCEA60)   // [8]
// OURS, SkillsMon.h: D2VAR(D2CLIENT, gnCltEffect48DirMatrix, const uint8_t, 0xCEA80)   // [72]
// OURS, SkillsMon.h: D2VAR(D2CLIENT, gnCltEffect48Dirs, const uint8_t, 0xCEAC8)   // [8]
// OURS, SkillsMon.h: D2VAR(D2CLIENT, gnDir64ToSector4, const uint8_t, 0xCEAD0)   // [64]
// OURS, SkillsMon.h: D2VAR(D2CLIENT, gnSector4Direction, const uint8_t, 0xCEB10)   // [4]
// OURS, SkillsMon.h: D2VAR(D2CLIENT, gMissileVelJitter, const int32_t, 0xCEB14)   // [5]
// OURS, SkillsMon.h: D2VAR(D2CLIENT, gnDir16SquareX, const int32_t, 0xCEB28)   // [16]
// OURS, SkillsMon.h: D2VAR(D2CLIENT, gnDir16SquareY, const int32_t, 0xCEB68)   // [16]
// OURS, SkillsMon.h: D2VAR(D2CLIENT, gnDiabLightVelJitter, const int32_t, 0xCEBA8)   // [5]
// OURS, SkillsMon.h: D2VAR(D2CLIENT, gnArcaneTowerCircleX, const int32_t, 0xCEBC0)   // [64]
// OURS, SkillsMon.h: D2VAR(D2CLIENT, gnArcaneTowerCircleY, const int32_t, 0xCECC0)   // [64]
// OURS, SkillsPal.h: D2VAR(D2CLIENT, gnCltEffect34OffsetX, const int32_t, 0xCEDC0)   // [8]
// OURS, SkillsPal.h: D2VAR(D2CLIENT, gnCltEffect34OffsetY, const int32_t, 0xCEDE0)   // [8]
// OURS, automap.h: D2VAR(D2CLIENT, gaAutomapTownSkipCels, const int32_t, 0xCEE00)   // [12]
// OURS, automap.h: D2VAR(D2CLIENT, gaAutomapPartyMarker, const int32_t, 0xCEE30)   // [26]
// OURS, char.h: D2VAR(D2CLIENT, gVtblUiButtonImpl, const D2UiButtonImplVtbl, 0xCEE98)
// OURS, char.h: D2VAR(D2CLIENT, gVtblUiButtonProxy, const D2UiButtonProxyVtbl, 0xCEECC)
// OURS, char.h: D2VAR(D2CLIENT, gCharPanelHiliteBoxes, const POINT, 0xCEF00)   // [2][3]
// OURS, editbox.h: D2VAR(D2CLIENT, gVtblUiEditBoxImpl, const D2UiEditBoxImplVtbl, 0xCEF30)
// OURS, editbox.h: D2VAR(D2CLIENT, gVtblUiControlProxy, const D2UiControlProxyVtbl, 0xCEF80)
// OURS, inv.h: D2VAR(D2CLIENT, gSocketLayouts, const int32_t, 0xCEFD0)   // [576]
// OURS, inv.h: D2VAR(D2CLIENT, gMercPanelLabels, const D2MercPanelLabelStrc, 0xCF8D0)   // [12]
// OURS, Listbox.h: D2VAR(D2CLIENT, gVtblUiControlBase, const D2UiControlBaseVtbl, 0xCF97C)
// OURS, Listbox.h: D2VAR(D2CLIENT, gVtblListBoxImpl, const D2ListBoxImplVtbl, 0xCF9B0)
// OURS, Listbox.h: D2VAR(D2CLIENT, gVtblListBox, const D2ListBoxProxyVtbl, 0xCF9F0)
// OURS, mods.h: D2VAR(D2CLIENT, gaPeriodOfDayStrIndices, const D2C_StringIndices, 0xCFA30)   // [4]
// OURS, mods.h: D2VAR(D2CLIENT, gnSetBonusStates, const D2C_States, 0xCFA38)   // [6]
// OURS, mods.h: D2VAR(D2CLIENT, gnSixBitPopCount, const int32_t, 0xCFA50)   // [64]
// OURS, Restart.h: D2VAR(D2CLIENT, gfPercentScale, const float, 0xCFB50)
// OURS, Restart.h: D2VAR(D2CLIENT, gfGammaMin, const float, 0xCFB54)
// OURS, Restart.h: D2VAR(D2CLIENT, gfConst200, const float, 0xCFB58)
// OURS, Restart.h: D2VAR(D2CLIENT, gfGammaScale, const float, 0xCFB5C)
// OURS, Restart.h: D2VAR(D2CLIENT, gfOne, const float, 0xCFB60)
// OURS, Restart.h: D2VAR(D2CLIENT, gfSliderHalfWidth, const float, 0xCFB64)
// OURS, Restart.h: D2VAR(D2CLIENT, gfSliderWidthNeg, const float, 0xCFB68)
// OURS, SkillDesc.h: D2VAR(D2CLIENT, gnElemDamageStats, const int32_t, 0xCFB6C)   // [12]
// OURS, SkillDesc.h: D2VAR(D2CLIENT, gnElemDamageColors, const int32_t, 0xCFB9C)   // [4]
// OURS, spellsel.h: D2VAR(D2CLIENT, gVtblUiSpinnerImpl, const D2UiSpinnerImplVtbl, 0xCFBAC)
// OURS, spellsel.h: D2VAR(D2CLIENT, gVtblUiSpinnerProxy, const D2UiSpinnerProxyVtbl, 0xCFBE0)
// OURS, ui.h: D2VAR(D2CLIENT, gGameMessageHandlers, const D2WinCommandRegisterStrc, 0xCFC18)   // [4]
// OURS, ui.h: D2VAR(D2CLIENT, gInvMessageHandlers, const D2WinCommandRegisterStrc, 0xCFC48)   // [4]
// OURS, ui.h: D2VAR(D2CLIENT, gNpcshopPanelKeys, const D2PanelKeyBindStrc, 0xCFC78)
// OURS, ui.h: D2VAR(D2CLIENT, gSkilltreeMessageHandlers, const D2WinCommandRegisterStrc, 0xCFC9C)   // [2]
// OURS, ui.h: D2VAR(D2CLIENT, gMiniskillMessageHandlers, const D2WinCommandRegisterStrc, 0xCFCB4)
// OURS, ui.h: D2VAR(D2CLIENT, gStatsMessageHandlers, const D2WinCommandRegisterStrc, 0xCFCC0)   // [2]
// OURS, ui.h: D2VAR(D2CLIENT, gNewstatsMessageHandlers, const D2WinCommandRegisterStrc, 0xCFCD8)   // [2]
// OURS, ui.h: D2VAR(D2CLIENT, gHiriconsMessageHandlers, const D2WinCommandRegisterStrc, 0xCFCF0)
// OURS, ui.h: D2VAR(D2CLIENT, gNewskillsMessageHandlers, const D2WinCommandRegisterStrc, 0xCFCFC)   // [2]
// OURS, ui.h: D2VAR(D2CLIENT, gEscmenuMessageHandlers, const D2WinCommandRegisterStrc, 0xCFD18)   // [7]
// OURS, ui.h: D2VAR(D2CLIENT, gConfigMessageHandlers, const D2WinCommandRegisterStrc, 0xCFD70)   // [13]
// OURS, ui.h: D2VAR(D2CLIENT, gConfigWheelHandler, const D2WinCommandRegisterStrc, 0xCFE0C)
// OURS, ui.h: D2VAR(D2CLIENT, gAutomapMessageHandlers, const D2WinCommandRegisterStrc, 0xCFE18)   // [5]
// OURS, ui.h: D2VAR(D2CLIENT, gQuestlogMessageHandlers, const D2WinCommandRegisterStrc, 0xCFE54)   // [4]
// OURS, ui.h: D2VAR(D2CLIENT, gQuestscreenPanelKeys, const D2PanelKeyBindStrc, 0xCFE84)
// OURS, ui.h: D2VAR(D2CLIENT, gIniscrollMessageHandlers, const D2WinCommandRegisterStrc, 0xCFEA8)   // [2]
// OURS, ui.h: D2VAR(D2CLIENT, gWaypointMessageHandlers, const D2WinCommandRegisterStrc, 0xCFEC0)   // [5]
// OURS, ui.h: D2VAR(D2CLIENT, gMinipanelMessageHandlers, const D2WinCommandRegisterStrc, 0xCFEFC)   // [2]
// OURS, ui.h: D2VAR(D2CLIENT, gPartyMessageHandlers, const D2WinCommandRegisterStrc, 0xCFF14)   // [2]
// OURS, ui.h: D2VAR(D2CLIENT, gTradePanelKeys, const D2PanelKeyBindStrc, 0xCFF2C)
// OURS, ui.h: D2VAR(D2CLIENT, gStashPanelKeys, const D2PanelKeyBindStrc, 0xCFF50)
// OURS, ui.h: D2VAR(D2CLIENT, gCubePanelKeys, const D2PanelKeyBindStrc, 0xCFF74)
// OURS, ui.h: D2VAR(D2CLIENT, gBeltPanelKeys, const D2PanelKeyBindStrc, 0xCFF98)
// OURS, ui.h: D2VAR(D2CLIENT, gHelpscreenMessageHandlers, const D2WinCommandRegisterStrc, 0xCFFBC)   // [5]
// OURS, ui.h: D2VAR(D2CLIENT, gHelpbuttonMessageHandlers, const D2WinCommandRegisterStrc, 0xCFFF8)   // [2]
// OURS, ui.h: D2VAR(D2CLIENT, gHireiconsMessageHandlers, const D2WinCommandRegisterStrc, 0xD0010)   // [3]
// OURS, ui.h: D2VAR(D2CLIENT, gMercinvMessageHandlers, const D2WinCommandRegisterStrc, 0xD0034)   // [3]
// OURS, ui.h: D2VAR(D2CLIENT, gRecipescrollMessageHandlers, const D2WinCommandRegisterStrc, 0xD0058)   // [2]
// OURS, ui.h: D2VAR(D2CLIENT, gMonImmunityLabels, const uint8_t, 0xD0070)   // [60]
// OURS, ui.h: D2VAR(D2CLIENT, gbUiWindowCloseable, const int32_t, 0xD00B0)   // [38]
// OURS, CUnit.h: D2VAR(D2CLIENT, gUnitTypeDestroyOrder, const int32_t, 0xD0148)   // [6]
// OURS, Missile.h: D2VAR(D2CLIENT, gCircle30OffsetX, const int32_t, 0xD0160)   // [64]
// OURS, Missile.h: D2VAR(D2CLIENT, gCircle30OffsetY, const int32_t, 0xD0260)   // [64]
// OURS, Missile.h: D2VAR(D2CLIENT, gnMephistoDir4OffsetX, const int32_t, 0xD0364)   // [4]
// OURS, Missile.h: D2VAR(D2CLIENT, gnMephistoDir4OffsetY, const int32_t, 0xD0374)   // [4]
// OURS, Missile.h: D2VAR(D2CLIENT, gnSpawnRing16X, const int32_t, 0xD0388)   // [16]
// OURS, Missile.h: D2VAR(D2CLIENT, gnSpawnRing16Y, const int32_t, 0xD03C8)   // [16]
// OURS, Missile.h: D2VAR(D2CLIENT, gnMissileDir8OffsetX, const int32_t, 0xD0408)   // [8]
// OURS, Missile.h: D2VAR(D2CLIENT, gnMissileDir8OffsetY, const int32_t, 0xD0428)   // [8]
// OURS, Missile.h: D2VAR(D2CLIENT, gnDirection8OffsetX, const int32_t, 0xD044C)   // [8]
// OURS, Missile.h: D2VAR(D2CLIENT, gnDirection8OffsetY, const int32_t, 0xD046C)   // [8]
// OURS, Missile.h: D2VAR(D2CLIENT, gnGlacialSpikeBurstParams, const int32_t, 0xD048C)   // [15]
// OURS, Missile.h: D2VAR(D2CLIENT, gnMeteorSplashOffsetX, const int32_t, 0xD04C8)   // [18]
// OURS, Missile.h: D2VAR(D2CLIENT, gnMeteorSplashOffsetY, const int32_t, 0xD0510)   // [18]
// OURS, Missile.h: D2VAR(D2CLIENT, gnMeteorCrossOffsetX, const int32_t, 0xD0558)   // [5]
// OURS, Missile.h: D2VAR(D2CLIENT, gnMeteorCrossOffsetY, const int32_t, 0xD056C)   // [5]
// OURS, Missile.h: D2VAR(D2CLIENT, gaMissileRingOffsetX, const int32_t, 0xD0580)   // [8]
// OURS, Missile.h: D2VAR(D2CLIENT, gaMissileRingOffsetY, const int32_t, 0xD05A0)   // [8]
// OURS, Missile.h: D2VAR(D2CLIENT, gnFrozenOrbRingX, const int32_t, 0xD05C0)   // [64]
// OURS, Missile.h: D2VAR(D2CLIENT, gnFrozenOrbRingY, const int32_t, 0xD06C0)   // [64]
// OURS, Missile.h: D2VAR(D2CLIENT, gnHitFunc4RingX, const int32_t, 0xD07C0)   // [64]
// OURS, Missile.h: D2VAR(D2CLIENT, gnHitFunc4RingY, const int32_t, 0xD08C0)   // [64]
// OURS, Missile.h: D2VAR(D2CLIENT, gnBoulderFireOffsetX, const int32_t, 0xD09C0)   // [18]
// OURS, Missile.h: D2VAR(D2CLIENT, gnBoulderFireOffsetY, const int32_t, 0xD0A08)   // [18]
// OURS, Missile.h: D2VAR(D2CLIENT, gnHoradricLightSteps, const int32_t, 0xD0A50)   // [21]
// OURS, Missile.h: D2VAR(D2CLIENT, gRadamentDeathFx, const D2MissileFxStageStrc, 0xD0AA8)   // [6]
// OURS, Missile.h: D2VAR(D2CLIENT, gTaintedSunFx, const D2MissileFxStageStrc, 0xD0AF0)   // [5]
// OURS, Missile.h: D2VAR(D2CLIENT, gDurielDeathFx, const D2MissileFxStageStrc, 0xD0B2C)   // [5]
// OURS, Missile.h: D2VAR(D2CLIENT, gBaalFx, const D2MissileFxStageStrc, 0xD0B68)   // [5]
// OURS, Monster.h: D2VAR(D2CLIENT, gnCatapultMissiles, const int32_t, 0xD0BA8)   // [5]
// OURS, Monster.h: D2VAR(D2CLIENT, scnCatapultMissiles, const int32_t, 0xD0BBC)
// OURS, Monster.h: D2VAR(D2CLIENT, gnBloodSpawnOffsetX, const int32_t, 0xD0BC0)   // [8]
// OURS, Monster.h: D2VAR(D2CLIENT, gnBloodSpawnOffsetY, const int32_t, 0xD0BE0)   // [8]
// OURS, Monster.h: D2VAR(D2CLIENT, gMonActionModeTable, const int32_t, 0xD0C00)   // [60]
// OURS, MonUnique.h: D2VAR(D2CLIENT, gaChampionUModNames, const D2ChampionUModNameStrc, 0xD0CF0)   // [5]
// OURS, MonUnique.h: D2VAR(D2CLIENT, gnMonUModOrder, const D2C_MonUMods, 0xD0D1C)   // [4]
// OURS, Object.h: D2VAR(D2CLIENT, gShrineChart, D2ClientShrineChartStrc, 0xD0D30)   // [23]
// OURS, UnitMode.h: D2VAR(D2CLIENT, gnHitClassOverlay, const int32_t, 0xD0EFC)   // [7][2]
// OURS, SoundChan.h: D2VAR(D2CLIENT, gfEaxMilliScale, const float, 0xD0F50)
// OURS, SoundChan.h: D2VAR(D2CLIENT, gfSoundPositionScale, const float, 0xD0F70)
// OURS, SoundEnv.h: D2VAR(D2CLIENT, gfConst255, const float, 0xD0F74)
// OURS, SoundEnv.h: D2VAR(D2CLIENT, gfSoundZero, const float, 0xD0F78)
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gfConst60, const float, 0xD0F7C)
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gfSoundMaxDistance, const float, 0xD0F80)
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gfConst400, const float, 0xD0F84)
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gfConst700, const float, 0xD0F88)
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gfConst1500, const float, 0xD0F8C)
// OURS, SoundInst.h: D2VAR(D2CLIENT, gfSoundMaxDistanceNeg, const float, 0xD0F90)
// OURS, SoundInst.h: D2VAR(D2CLIENT, gfConst640, const float, 0xD0F94)
// OURS, SoundInst.h: D2VAR(D2CLIENT, gfHalf, const float, 0xD0F98)
// OURS, SoundInst.h: D2VAR(D2CLIENT, gfConstNegPoint4, const float, 0xD0F9C)
// OURS, SoundInst.h: D2VAR(D2CLIENT, gfNegTwo, const float, 0xD0FA0)
// OURS, SoundInst.h: D2VAR(D2CLIENT, gfSoundFadeStep, const float, 0xD0FA4)
// OURS, SoundInst.h: D2VAR(D2CLIENT, gfEarshotDistanceSq, const float, 0xD0FA8)

// ---- .data ----
// OURS, CoreArchive.h: D2VAR(D2CLIENT, szSrcFile, char, 0xD40A0)   // [61]
// OURS, CoreArchive.h: D2VAR(D2CLIENT, gnScreenWidth, int32_t, 0xD40EC)
// OURS, CoreArchive.h: D2VAR(D2CLIENT, gnScreenHeight, int32_t, 0xD40F0)
// OURS, CoreArchive.h: D2VAR(D2CLIENT, gnDrawMaxWidth, int32_t, 0xD40F4)
// OURS, DLLmain.h: D2VAR(D2CLIENT, gpQueryInterfaceVtbl, D2ClientInterface, 0xD4108)
// OURS, Lng.h: D2VAR(D2CLIENT, szKeyName, char, 0xD4150)   // [10]
// OURS, Video.h: D2VAR(D2CLIENT, gCinematics, D2CinematicEntry, 0xD42B8)   // [8]
// OURS, Video.h: D2VAR(D2CLIENT, gnCinematicCount, int32_t, 0xD4318)
// OURS, WINMAIN.h: D2VAR(D2CLIENT, gnAppMode, D2C_AppMode, 0xD44D8)
// OURS, WINMAIN.h: using D2CLIENT_gafnAppModeEntryPoints_ft = D2C_AppMode (__fastcall *)();
// OURS, WINMAIN.h: D2VAR(D2CLIENT, gafnAppModeEntryPoints, D2CLIENT_gafnAppModeEntryPoints_ft, 0xD44EC)   // [7]
// OURS, WINMAIN.h: D2VAR(D2CLIENT, gpLaunchParamCategories, LPCSTR, 0xD4508)   // [11]
// OURS, WINMAIN.h: D2VAR(D2CLIENT, gbAppModeContinue, BOOL, 0xD4538)
// OURS, WNDPROC.h: D2VAR(D2CLIENT, gbAppActive, int32_t, 0xD47C0)
// OURS, WNDPROC.h: D2VAR(D2CLIENT, sgWndProcHandlers, D2WinCommandRegisterStrc, 0xD47C8)   // [6]
// OURS, Draw.h: D2VAR(D2CLIENT, gDisplayType, DisplayType, 0xD4970)
// OURS, Draw.h: D2VAR(D2CLIENT, gszTileCacheDebugFmt, char, 0xD4A70)   // [62]
// OURS, Draw.h: D2VAR(D2CLIENT, szFormat, char, 0xD4B9C)   // [16]
// OURS, dLightMap.h: D2VAR(D2CLIENT, gbColoredLights, int32_t, 0xD4C88)
// OURS, dLightMap.h: D2VAR(D2CLIENT, gszAssertHSrcUnit, char, 0xD4D50)   // [9]
// OURS, dWall.h: D2VAR(D2CLIENT, gaTileLightOffsets, int32_t, 0xD4D60)   // [10][6][2]
// OURS, dWall.h: D2VAR(D2CLIENT, gaTileLightOffsetsAlt, int32_t, 0xD4F40)   // [10][6][2]
// OURS, dWall.h: D2VAR(D2CLIENT, gszAssertOnlyDrawShadowLayer, char, 0xD5140)   // [55]
// OURS, arena.h: D2VAR(D2CLIENT, gdwArenaFlags, uint32_t, 0xD51B4)
// OURS, arena.h: D2VAR(D2CLIENT, gnArenaPlayerClass, int32_t, 0xD51B8)
// OURS, Env.h: D2VAR(D2CLIENT, gbEnvPlayThunder, int32_t, 0xD51C0)
// OURS, Game.h: D2VAR(D2CLIENT, gafnMessageCallbacks, D2ClientMessageCallbackFn, 0xD5450)   // [3]
// OURS, Game.h: D2VAR(D2CLIENT, gnAppModeAfterStartup, D2C_AppMode, 0xD545C)
// OURS, Game.h: D2VAR(D2CLIENT, gnGameTickIntervalMs, int32_t, 0xD5460)
// OURS, Game.h: D2VAR(D2CLIENT, gszServerIP, char, 0xD5468)   // [264]
// OURS, Game.h: D2VAR(D2CLIENT, gszBattleNetIP, char, 0xD5570)   // [264]
// OURS, Game.h: D2VAR(D2CLIENT, gszMCPIP, char, 0xD5678)   // [264]
// OURS, Game.h: D2VAR(D2CLIENT, gBetaScreens, D2BetaScreenStrc, 0xD5780)   // [10]
// OURS, Game.h: D2VAR(D2CLIENT, gnBetaScreenCount, int32_t, 0xD5804)
// OURS, Game.h: D2VAR(D2CLIENT, gnLeftClickModifierFlags, int32_t, 0xD580C)
// OURS, Game.h: D2VAR(D2CLIENT, gnRightClickModifierFlags, int32_t, 0xD5810)
// OURS, Game.h: D2VAR(D2CLIENT, gnForceClearFrames, int32_t, 0xD5814)
// OURS, Game.h: D2VAR(D2CLIENT, gGameWindowMessageHandlers, D2WinCommandRegisterStrc, 0xD5818)   // [10]
// OURS, Game.h: D2VAR(D2CLIENT, gWheelNativeHandler, D2WinCommandRegisterStrc, 0xD5890)
// OURS, Game.h: D2VAR(D2CLIENT, gMButtonHandlers, D2WinCommandRegisterStrc, 0xD589C)   // [2]
// OURS, Game.h: D2VAR(D2CLIENT, gXButtonHandlers, D2WinCommandRegisterStrc, 0xD58B4)   // [2]
// OURS, Game.h: D2VAR(D2CLIENT, gWheelWin95Handler, D2WinCommandRegisterStrc, 0xD58CC)   // [1]
// OURS, Game.h: D2VAR(D2CLIENT, gnPrevViewOffsetX, int32_t, 0xD58D8)
// OURS, Game.h: D2VAR(D2CLIENT, gnPrevViewOffsetY, int32_t, 0xD58DC)
// OURS, Game.h: D2VAR(D2CLIENT, gnDisconnectMessageStrings, uint16_t, 0xD58E0)   // [29]
// OURS, Game.h: D2VAR(D2CLIENT, gnMaxPlayerCountForGameRefresh, int32_t, 0xD591C)
// OURS, Game.h: D2VAR(D2CLIENT, gBetaSlidesMessageHandlers, D2WinCommandRegisterStrc, 0xD5920)   // [7]
// OURS, Game.h: D2VAR(D2CLIENT, gVanquishedMessageHandlers, D2WinCommandRegisterStrc, 0xD5978)   // [9]
// OURS, Game.h: D2VAR(D2CLIENT, gbLaunchParamsOk, BOOL, 0xD59E4)
// OURS, Game.h: using D2CLIENT_gLaunchParamHandlers_ft = void (__fastcall *)(D2ConfigStrc *pConfig);
// OURS, Game.h: D2VAR(D2CLIENT, gLaunchParamHandlers, D2CLIENT_gLaunchParamHandlers_ft, 0xD59E8)   // [48]
// OURS, Game.h: D2VAR(D2CLIENT, pszFileName, char, 0xD5C18)   // [21]
// OURS, Game.h: D2VAR(D2CLIENT, OutputString, CHAR, 0xD5C88)   // [30]
// OURS, Game.h: D2VAR(D2CLIENT, ClassName, CHAR, 0xD5E64)   // [7]
// OURS, Game.h: D2VAR(D2CLIENT, WindowName, CHAR, 0xD5E6C)   // [17]
// OURS, Game.h: D2VAR(D2CLIENT, String, CHAR, 0xD5E80)   // [21]
// OURS, Game.h: D2VAR(D2CLIENT, szTSExplicitListSGameDataRawName, char, 0xD5EA0)   // [48]
// OURS, Record.h: D2VAR(D2CLIENT, gpRecordBlock, D2ClientRecordHolderStrc *, 0xD6048)
// OURS, Record.h: D2VAR(D2CLIENT, FileName, CHAR, 0xD6088)   // [11]
// OURS, SCmd.h: D2VAR(D2CLIENT, gSCmdTable, D2SCmdStrc, 0xD6270)   // [174]
// OURS, SCmd.h: D2VAR(D2CLIENT, szFileModeWriteBinary, char, 0xD6A98)   // [3]
// OURS, SCmd.h: D2VAR(D2CLIENT, szFile, char, 0xD6A9C)   // [58]
// OURS, SCmd.h: D2VAR(D2CLIENT, szCouldNotGetActMsg, char, 0xD6AD8)   // [19]
// OURS, SCmd.h: D2VAR(D2CLIENT, szIllegalPacketWarpXYMsg, char, 0xD6AEC)   // [35]
// OURS, SCmd.h: D2VAR(D2CLIENT, szAssertHUnit, char, 0xD6B10)   // [6]
// OURS, SCmd.h: D2VAR(D2CLIENT, szAssertNumUpdates, char, 0xD6B18)   // [24]
// OURS, SCmd.h: D2VAR(D2CLIENT, szAssertHSkill, char, 0xD6B30)   // [7]
// OURS, SCmd.h: D2VAR(D2CLIENT, szAssertUnitGetControlUnit, char, 0xD6B38)   // [21]
// OURS, SCmd.h: D2VAR(D2CLIENT, gszAssertItemUnitExMsg, char, 0xD6B70)   // [42]
// OURS, Select.h: D2VAR(D2CLIENT, gnToHitRefMonster, int32_t, 0xD7080)
// OURS, Select.h: D2VAR(D2CLIENT, gnToBeHitRefMonster, int32_t, 0xD7084)
// OURS, Select.h: D2VAR(D2CLIENT, gnUnitTypeSelectPriority, int32_t, 0xD7088)   // [6]
// OURS, View.h: D2VAR(D2CLIENT, gpEnvSeed, D2SeedStrc *, 0xD712C)
// OURS, View.h: D2VAR(D2CLIENT, gpParallaxCloudSeed, D2SeedStrc *, 0xD7130)
// OURS, Wall2.h: D2VAR(D2CLIENT, gnWallEdgeTestMode, int32_t, 0xD7264)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gnDenOfEvilPulseFrame, int32_t, 0xD73A8)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gnDiabloAppearsTintFrame, int32_t, 0xD73AC)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gdwQuestFxWatchedUnitId, uint32_t, 0xD73B0)
// OURS, QuestsFX.h: using D2CLIENT_gQuestTriggerFuncs_ft = void (__cdecl *)();
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gQuestTriggerFuncs, D2CLIENT_gQuestTriggerFuncs_ft, 0xD73B8)   // [20]
// OURS, Skills.h: using D2CLIENT_gpSkillCltStFuncs_ft = int32_t (__fastcall *)(D2UnitStrc *, D2C_Skills, int32_t);
// OURS, Skills.h: D2VAR(D2CLIENT, gpSkillCltStFuncs, D2CLIENT_gpSkillCltStFuncs_ft, 0xD7488)   // [70]
// OURS, Skills.h: D2VAR(D2CLIENT, gnSkillCltStFuncCount, int32_t, 0xD75A0)
// OURS, Skills.h: using D2CLIENT_gpSkillCltDoFuncs_ft = int32_t (__fastcall *)(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
// OURS, Skills.h: D2VAR(D2CLIENT, gpSkillCltDoFuncs, D2CLIENT_gpSkillCltDoFuncs_ft, 0xD75A8)   // [130]
// OURS, Skills.h: D2VAR(D2CLIENT, gnSkillCltDoFuncCount, int32_t, 0xD77B0)
// OURS, Skills.h: using D2CLIENT_gpStateCltEventFuncs_ft = int32_t (__fastcall *)(int32_t nEvent, D2UnitStrc *pUnit, D2UnitStrc *pMissile, int32_t nUnused, D2C_Skills nSkillId, int32_t nSkillLevel);
// OURS, Skills.h: D2VAR(D2CLIENT, gpStateCltEventFuncs, D2CLIENT_gpStateCltEventFuncs_ft, 0xD77B4)   // [2]
// OURS, SkillsAss.h: D2VAR(D2CLIENT, gnWaverFlipFrames, int32_t, 0xD78BC)
// OURS, SkillsAss.h: D2VAR(D2CLIENT, gnWaverBaseTurn, int32_t, 0xD78C0)
// OURS, automap.h: D2VAR(D2CLIENT, gaAutomapCellGroupPairs, int32_t, 0xD7A38)   // [98]
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapZoom, int32_t, 0xD7BC0)
// OURS, automap.h: D2VAR(D2CLIENT, gbAutomapCenters, BOOL, 0xD7BC4)
// OURS, automap.h: D2VAR(D2CLIENT, gbAutomapShowParty, BOOL, 0xD7BC8)
// OURS, automap.h: D2VAR(D2CLIENT, gbAutomapPartyNames, BOOL, 0xD7BCC)
// OURS, automap.h: D2VAR(D2CLIENT, szValueName, char, 0xD7BDC)   // [16]
// OURS, automap.h: D2VAR(D2CLIENT, Format, char, 0xD7C5C)   // [10]
// OURS, char.h: D2VAR(D2CLIENT, gUiButtonImages, D2UiButtonImageStrc, 0xD8008)   // [2]
// OURS, char.h: D2VAR(D2CLIENT, gCharPanelLabels, D2CharPanelLabelStrc, 0xD8010)   // [15]
// OURS, char.h: D2VAR(D2CLIENT, gCharPanelStatRows, int32_t, 0xD8120)   // [72]
// OURS, char.h: D2VAR(D2CLIENT, gCharPanelStatButtons, uint8_t, 0xD8240)   // [56]
// OURS, chat.h: D2VAR(D2CLIENT, gnChatHistoryIndex, int32_t, 0xD82E8)
// OURS, chat.h: D2VAR(D2CLIENT, gnChatHistoryHead, int32_t, 0xD82EC)
// OURS, chat.h: D2VAR(D2CLIENT, gChatMessageHandlers, D2WinCommandRegisterStrc, 0xD82F0)   // [4]
// OURS, chat.h: D2VAR(D2CLIENT, gChatImeHandlers, D2WinCommandRegisterStrc, 0xD8320)   // [7]
// OURS, chat.h: D2VAR(D2CLIENT, gbChatInputBlinkOn, BOOL, 0xD8374)
// OURS, chat.h: D2VAR(D2CLIENT, gnChatInputBlinkMs, uint16_t, 0xD8378)
// OURS, chat.h: D2VAR(D2CLIENT, gChatCommandPrefixes, D2ChatPrefixStrc, 0xD837C)   // [3]
// OURS, chat.h: D2VAR(D2CLIENT, Delimiter, char, 0xD83A6)   // [2]
// OURS, chat.h: D2VAR(D2CLIENT, gszChatHelpCommand, char, 0xD8404)   // [3]
// OURS, chat.h: D2VAR(D2CLIENT, pszCommand, char, 0xD8408)   // [6]
// OURS, chat.h: D2VAR(D2CLIENT, szCommand, char, 0xD8424)   // [5]
// OURS, chat.h: D2VAR(D2CLIENT, pStr2, char, 0xD845C)   // [9]
// OURS, chat.h: D2VAR(D2CLIENT, String2, char, 0xD8474)   // [5]
// OURS, chat.h: D2VAR(D2CLIENT, gszSpace, char, 0xD847C)   // [2]
// OURS, chat.h: D2VAR(D2CLIENT, gszBattleNetPrefix, char, 0xD8480)   // [14]
// OURS, CmdTbl.h: D2VAR(D2CLIENT, gDefaultCommandTable, HotkeyMappingEntry, 0xD8500)   // [114]
// OURS, CmdTbl.h: D2VAR(D2CLIENT, sgtKeyFunctions, KeyFunctions, 0xD8978)   // [57]
// OURS, CmdTbl.h: D2VAR(D2CLIENT, gKeyCommandHandlers, D2WinCommandRegisterStrc, 0xD8C24)   // [4]
// OURS, CmdTbl.h: D2VAR(D2CLIENT, gKeyCommandReleaseHandlers, D2WinCommandRegisterStrc, 0xD8C54)   // [2]
// OURS, dialog.h: D2VAR(D2CLIENT, gDialogMessageHandlers, D2WinCommandRegisterStrc, 0xD8DF8)   // [14]
// OURS, dialog.h: D2VAR(D2CLIENT, gDialogWheelHandlers, D2WinCommandRegisterStrc, 0xD8EA0)
// OURS, dialog.h: D2VAR(D2CLIENT, gDialogImageNames, const char *, 0xD8EC0)   // [1]
// OURS, help.h: D2VAR(D2CLIENT, gnHelpMenuSeen, int32_t, 0xD9088)
// OURS, help.h: D2VAR(D2CLIENT, szName, CHAR, 0xD90A4)   // [21]
// OURS, Hireables.h: D2VAR(D2CLIENT, gszDruidIconPath, char, 0xD9120)   // [20]
// OURS, InsertDisc.h: D2VAR(D2CLIENT, gInsertDiscMessageHandlers, D2WinCommandRegisterStrc, 0xD91E8)   // [4]
// OURS, InsertDisc.h: D2VAR(D2CLIENT, gCineDiscMessageHandlers, D2WinCommandRegisterStrc, 0xD9218)   // [4]
// OURS, InsertDisc.h: D2VAR(D2CLIENT, pszName, char, 0xD9268)   // [31]
// OURS, inv.h: D2VAR(D2CLIENT, gbInvHoverEnabled, BOOL, 0xD9288)
// OURS, inv.h: D2VAR(D2CLIENT, gnNpcTradeMode, BOOL, 0xD928C)
// OURS, inv.h: D2VAR(D2CLIENT, gnDescPopupX, int32_t, 0xD9290)
// OURS, inv.h: D2VAR(D2CLIENT, gnDescPopupY, int32_t, 0xD9294)
// OURS, inv.h: D2VAR(D2CLIENT, gnDescPopupAltX, int32_t, 0xD9298)
// OURS, inv.h: D2VAR(D2CLIENT, gnDescPopupBottomY, int32_t, 0xD929C)
// OURS, inv.h: D2VAR(D2CLIENT, gnHoverCellX, int32_t, 0xD92A0)
// OURS, inv.h: D2VAR(D2CLIENT, gnHoverCellY, int32_t, 0xD92A4)
// OURS, inv.h: D2VAR(D2CLIENT, gnCubeUnitGUID, int32_t, 0xD92A8)
// OURS, inv.h: using D2CLIENT_gBodyLocClickHandlers_ft = int32_t (__fastcall *)(D2UnitStrc *, D2InventoryStrc *, D2C_PlayerBodyLocs);
// OURS, inv.h: D2VAR(D2CLIENT, gBodyLocClickHandlers, D2CLIENT_gBodyLocClickHandlers_ft, 0xD92AC)   // [11]
// OURS, inv.h: D2VAR(D2CLIENT, gWeaponClassDescs, D2WeaponDescStrc, 0xD92D8)   // [15]
// OURS, inv.h: D2VAR(D2CLIENT, gWeaponSpeedDescs, D2WeaponDescStrc, 0xD9334)   // [7]
// OURS, inv.h: D2VAR(D2CLIENT, gptAttackSpeedMatrix, int32_t, 0xD9360)   // [18][5]
// OURS, inv.h: D2VAR(D2CLIENT, gptAttackSpeedClassOffsets, int32_t, 0xD94C8)   // [7][2]
// OURS, inv.h: D2VAR(D2CLIENT, gnInvTorsoEmptyOffsetX, int32_t, 0xD9500)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvTorsoEmptyOffsetY, int32_t, 0xD9504)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvBeltEmptyOffsetX, int32_t, 0xD9508)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvBeltEmptyOffsetY, int32_t, 0xD950C)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvFeetEmptyOffsetX, int32_t, 0xD9510)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvFeetEmptyOffsetY, int32_t, 0xD9514)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvHeadEmptyOffsetX, int32_t, 0xD9518)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvHeadEmptyOffsetY, int32_t, 0xD951C)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvGlovesEmptyOffsetX, int32_t, 0xD9520)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvGlovesEmptyOffsetY, int32_t, 0xD9524)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvNeckEmptyOffsetX, int32_t, 0xD9528)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvNeckEmptyOffsetY, int32_t, 0xD952C)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvLRingEmptyOffsetX, int32_t, 0xD9530)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvLRingEmptyOffsetY, int32_t, 0xD9534)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvRRingEmptyOffsetX, int32_t, 0xD9538)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvRRingEmptyOffsetY, int32_t, 0xD953C)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvRArmEmptyOffsetX, int32_t, 0xD9540)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvRArmEmptyOffsetY, int32_t, 0xD9544)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvLArmEmptyOffsetX, int32_t, 0xD9548)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvLArmEmptyOffsetY, int32_t, 0xD954C)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvRArmSwapTabOffsetX, int32_t, 0xD9550)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvRArmSwapTabOffsetY, int32_t, 0xD9554)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvLArmSwapTabOffsetX, int32_t, 0xD9558)
// OURS, inv.h: D2VAR(D2CLIENT, gnInvLArmSwapTabOffsetY, int32_t, 0xD955C)
// OURS, inv.h: D2VAR(D2CLIENT, gVendorTabs, D2BuySellTabStrc, 0xD9560)   // [4]
// OURS, inv.h: D2VAR(D2CLIENT, gnVendorTabCount, int32_t, 0xD95A8)
// OURS, inv.h: D2VAR(D2CLIENT, gnNpcShopCloseTab, int32_t, 0xD95AC)
// OURS, inv.h: D2VAR(D2CLIENT, gnNpcShopTabCount, int32_t, 0xD95B0)
// OURS, inv.h: D2VAR(D2CLIENT, gatButtonsOffsetX, int32_t, 0xD95B8)   // [4][4]
// OURS, inv.h: D2VAR(D2CLIENT, gMercStatRows, D2MercStatRowStrc, 0xD95F8)   // [12]
// OURS, inv.h: D2VAR(D2CLIENT, gszAffixCloseParen, char, 0xD96FC)   // [2]
// OURS, inv.h: D2VAR(D2CLIENT, gszAffixOpenParen, char, 0xD9700)   // [2]
// OURS, inv.h: D2VAR(D2CLIENT, gszRuneQuoteClose, char, 0xD98A0)   // [2]
// OURS, inv.h: D2VAR(D2CLIENT, gszCommaSpace, char, 0xD9918)   // [3]
// OURS, mods.h: D2VAR(D2CLIENT, gptClientStatDescs, D2ClientStatDescStrc, 0xD9B48)   // [143]
// OURS, mods.h: D2VAR(D2CLIENT, gnClientStatDescCount, int32_t, 0xDA438)
// OURS, mods.h: D2VAR(D2CLIENT, gptElixirStatLabels, D2ElixirStatLabelStrc, 0xDA440)   // [6]
// OURS, mods.h: D2VAR(D2CLIENT, gnElixirStatLabelCount, uint32_t, 0xDA4A0)
// OURS, npcmenu.h: D2VAR(D2CLIENT, gNpcMenuQuestUnlocks, D2NpcMenuQuestUnlockStrc, 0xDA518)   // [8]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gnNpcMenuQuestUnlockCount, int32_t, 0xDA5B8)
// OURS, npcmenu.h: D2VAR(D2CLIENT, gwMercResurrectNameTblId, D2C_StringIndices, 0xDA5BC)
// OURS, npcmenu.h: D2VAR(D2CLIENT, gKashyaSpeechLines, D2NpcSpeechLineStrc, 0xDA5C0)   // [12]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gWarriv1SpeechLines, D2NpcSpeechLineStrc, 0xDA678)   // [11]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gGheedSpeechLines, D2NpcSpeechLineStrc, 0xDA720)   // [9]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gCain1SpeechLines, D2NpcSpeechLineStrc, 0xDA7A8)   // [7]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gCain2SpeechLines, D2NpcSpeechLineStrc, 0xDA818)   // [7]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gCain3SpeechLines, D2NpcSpeechLineStrc, 0xDA888)   // [8]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gCharsiSpeechLines, D2NpcSpeechLineStrc, 0xDA900)   // [9]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gAkaraSpeechLines, D2NpcSpeechLineStrc, 0xDA988)   // [13]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gAtmaSpeechLines, D2NpcSpeechLineStrc, 0xDAA50)   // [10]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gDrognanSpeechLines, D2NpcSpeechLineStrc, 0xDAAE8)   // [12]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gGreizSpeechLines, D2NpcSpeechLineStrc, 0xDABA0)   // [14]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gElzixSpeechLines, D2NpcSpeechLineStrc, 0xDAC78)   // [12]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gGeglashSpeechLines, D2NpcSpeechLineStrc, 0xDAD30)   // [11]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gJerhynSpeechLines, D2NpcSpeechLineStrc, 0xDADD8)   // [10]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gLysanderSpeechLines, D2NpcSpeechLineStrc, 0xDAE70)   // [12]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gFaraSpeechLines, D2NpcSpeechLineStrc, 0xDAF28)   // [11]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gMeshif1SpeechLines, D2NpcSpeechLineStrc, 0xDAFD0)   // [12]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gWarriv2SpeechLines, D2NpcSpeechLineStrc, 0xDB088)   // [7]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gMeshif2SpeechLines, D2NpcSpeechLineStrc, 0xDB0F8)   // [11]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gHratliSpeechLines, D2NpcSpeechLineStrc, 0xDB1A0)   // [12]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gAshearaSpeechLines, D2NpcSpeechLineStrc, 0xDB258)   // [9]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gAlkorSpeechLines, D2NpcSpeechLineStrc, 0xDB2E0)   // [11]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gOrmusSpeechLines, D2NpcSpeechLineStrc, 0xDB388)   // [12]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gTyrael1SpeechLines, D2NpcSpeechLineStrc, 0xDB43C)   // [3]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gCain4SpeechLines, D2NpcSpeechLineStrc, 0xDB46C)   // [3]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gNatalyaSpeechLines, D2NpcSpeechLineStrc, 0xDB4A0)   // [6]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gCain6SpeechLines, D2NpcSpeechLineStrc, 0xDB500)   // [12]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gDrehyaSpeechLines, D2NpcSpeechLineStrc, 0xDB5B8)   // [12]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gLarzukSpeechLines, D2NpcSpeechLineStrc, 0xDB670)   // [12]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gMalahSpeechLines, D2NpcSpeechLineStrc, 0xDB728)   // [16]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gNihlathakSpeechLines, D2NpcSpeechLineStrc, 0xDB818)   // [12]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gQualKehkSpeechLines, D2NpcSpeechLineStrc, 0xDB8D0)   // [11]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gNpcMenuEntries, D2NpcMenuEntryStrc, 0xDB978)   // [46]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gnNpcMenuEntryCount, int32_t, 0xDBD6C)
// OURS, npcmenu.h: D2VAR(D2CLIENT, gNpcMenuDescriptors, D2NpcMenuDescStrc, 0xDBD70)   // [48]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gnNpcMenuDescriptorCount, int32_t, 0xDC4C0)
// OURS, npcmenu.h: D2VAR(D2CLIENT, gNpcSpeechPairs, D2NpcSpeechPairStrc, 0xDC4C4)   // [2]
// OURS, npcmenu.h: D2VAR(D2CLIENT, gnNpcSpeechPairCount, int32_t, 0xDC4D4)
// OURS, config.h: D2VAR(D2CLIENT, gCfgControlsList, D2CfgControlsRowStrc, 0xDC828)   // [51]
// OURS, config.h: D2VAR(D2CLIENT, gCfgControlsListExpansion, D2CfgControlsRowStrc, 0xDCA28)   // [62]
// OURS, config.h: D2VAR(D2CLIENT, gpCfgControlsList, D2CfgControlsRowStrc *, 0xDCC94)
// OURS, config.h: D2VAR(D2CLIENT, gCfgControlsButtons, D2ClientCfgControlsButtonStrc, 0xDCC98)   // [3]
// OURS, config.h: D2VAR(D2CLIENT, gnCfgBindColumn, int32_t, 0xDCCE8)
// OURS, config.h: D2VAR(D2CLIENT, gbCfgBindArmed, BOOL, 0xDCCEC)
// OURS, config.h: D2VAR(D2CLIENT, gCfgCaptureKeyHandlers, D2WinCommandRegisterStrc, 0xDCCF0)   // [6]
// OURS, config.h: D2VAR(D2CLIENT, gCfgCaptureMButtonHandler, D2WinCommandRegisterStrc, 0xDCD38)
// OURS, config.h: D2VAR(D2CLIENT, gCfgCaptureXButtonHandler, D2WinCommandRegisterStrc, 0xDCD44)
// OURS, config.h: D2VAR(D2CLIENT, gCfgCaptureWheelHandler, D2WinCommandRegisterStrc, 0xDCD50)
// OURS, panel.h: D2VAR(D2CLIENT, gnBeltsTxtIndex, int32_t, 0xDCD70)
// OURS, panel.h: D2VAR(D2CLIENT, gbGameBarBtnDownUnread, BOOL, 0xDCD74)
// OURS, panel.h: D2VAR(D2CLIENT, gnBeltHoverBox, int32_t, 0xDCD78)
// OURS, panel.h: D2VAR(D2CLIENT, gnBeltHintX, int32_t, 0xDCD7C)
// OURS, panel.h: D2VAR(D2CLIENT, gnBeltHintY, int32_t, 0xDCD80)
// OURS, panel.h: D2VAR(D2CLIENT, gnBeltSlotHotkeys, D2HotkeyIndices, 0xDCD90)   // [4]
// OURS, panel.h: D2VAR(D2CLIENT, gaQtyWarnBodyLocs, D2C_PlayerBodyLocs, 0xDCDA0)   // [2]
// OURS, panel.h: D2VAR(D2CLIENT, gnQtyWarnBodyLocCount, uint32_t, 0xDCDA8)
// OURS, panel.h: D2VAR(D2CLIENT, gaQtyWarnLevels, D2QtyWarnLevelStrc, 0xDCDB0)   // [5]
// OURS, panel.h: D2VAR(D2CLIENT, gnQtyWarnLevelCount, uint32_t, 0xDCE14)
// OURS, panel.h: D2VAR(D2CLIENT, gaWarnIconBodyLocs, D2C_PlayerBodyLocs, 0xDCE18)   // [7]
// OURS, panel.h: D2VAR(D2CLIENT, gszPopBeltPath, char, 0xDCEB0)   // [22]
// OURS, Party.h: D2VAR(D2CLIENT, gaPartyHostileHintMale, int16_t, 0xDCFE8)   // [4]
// OURS, Party.h: D2VAR(D2CLIENT, gaPartyHostileHintFemale, int16_t, 0xDCFF0)   // [4]
// OURS, Party.h: D2VAR(D2CLIENT, gaPartyLootHintMale, int16_t, 0xDCFF8)   // [4]
// OURS, Party.h: D2VAR(D2CLIENT, gaPartyLootHintFemale, int16_t, 0xDD000)   // [4]
// OURS, Party.h: D2VAR(D2CLIENT, Source, char, 0xDD044)   // [4]
// OURS, Party.h: D2VAR(D2CLIENT, gszErrorPlayerName, char, 0xDD050)   // [18]
// OURS, Party.h: D2VAR(D2CLIENT, gszUnsquelchCommand, char, 0xDD074)   // [12]
// OURS, Party.h: D2VAR(D2CLIENT, gszPartyBoxesPath, char, 0xDD0B0)   // [16]
// OURS, QuestLog.h: D2VAR(D2CLIENT, gQuestLogCloseRects, D2QuestLogCloseRectStrc, 0xDD0E0)   // [4]
// OURS, QuestLog.h: D2VAR(D2CLIENT, gQuestLogIconNames, const char *, 0xDD130)   // [27]
// OURS, QuestLog.h: D2VAR(D2CLIENT, gQuestLogTextBlocks, D2QuestLogTextBlockStrc, 0xDD19C)   // [28]
// OURS, QuestLog.h: D2VAR(D2CLIENT, sgQuestLogButtonPositions, QUESTLOGBUTTONPOSITIONT, 0xDD8A0)   // [6]
// OURS, QuestLog.h: D2VAR(D2CLIENT, gQuestLogActIconRange, int32_t, 0xDD900)   // [5][2]
// OURS, QuestLog.h: D2VAR(D2CLIENT, gQuestLogQuestDefs, D2QuestLogQuestDefStrc, 0xDD928)   // [41]
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnQuestLogDoneYOffset, int32_t, 0xDDBB8)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnQuestLogSelectedSocketYOffset, int32_t, 0xDDBBC)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnQuestLogTextX, int32_t, 0xDDBC0)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnQuestLogTitleY, int32_t, 0xDDBC4)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnQuestLogTextY, int32_t, 0xDDBC8)
// OURS, Restart.h: D2VAR(D2CLIENT, gEscMenuUiWindows, D2ClientEscMenuUiWindowStrc, 0xDDE68)   // [38]
// OURS, Restart.h: D2VAR(D2CLIENT, gEscMenuMain, D2ClientEscMenuInfoStrc, 0xDE030)
// OURS, Restart.h: D2VAR(D2CLIENT, gEscMenuMainItems, D2ClientEscMenuItemStrc, 0xDE048)   // [3]
// OURS, Restart.h: D2VAR(D2CLIENT, gEscMenuOptions, D2ClientEscMenuInfoStrc, 0xDF038)
// OURS, Restart.h: D2VAR(D2CLIENT, gEscMenuOptionsItems, D2ClientEscMenuItemStrc, 0xDF050)   // [5]
// OURS, Restart.h: D2VAR(D2CLIENT, gEscMenuSound, D2ClientEscMenuInfoStrc, 0xE0AE0)
// OURS, Restart.h: D2VAR(D2CLIENT, gEscMenuSoundItems, D2ClientEscMenuItemStrc, 0xE0AF8)   // [8]
// OURS, Restart.h: D2VAR(D2CLIENT, gEscMenuVideoClassic, D2ClientEscMenuInfoStrc, 0xE3578)
// OURS, Restart.h: D2VAR(D2CLIENT, gEscMenuVideoClassicItems, D2ClientEscMenuItemStrc, 0xE3590)   // [7]
// OURS, Restart.h: D2VAR(D2CLIENT, gEscMenuVideoExpansion, D2ClientEscMenuInfoStrc, 0xE5AC0)
// OURS, Restart.h: D2VAR(D2CLIENT, gEscMenuVideoExpansionItems, D2ClientEscMenuItemStrc, 0xE5AD8)   // [8]
// OURS, Restart.h: D2VAR(D2CLIENT, gEscMenuAutomapClassic, D2ClientEscMenuInfoStrc, 0xE8558)
// OURS, Restart.h: D2VAR(D2CLIENT, gEscMenuAutomapClassicItems, D2ClientEscMenuItemStrc, 0xE8570)   // [6]
// OURS, Restart.h: D2VAR(D2CLIENT, gEscMenuAutomapExpansion, D2ClientEscMenuInfoStrc, 0xEA550)
// OURS, Restart.h: D2VAR(D2CLIENT, gEscMenuAutomapExpansionItems, D2ClientEscMenuItemStrc, 0xEA568)   // [7]
// OURS, SkillDesc.h: D2VAR(D2CLIENT, gnSkillDescLine1YOffset, int32_t, 0xECCF4)
// OURS, SkillDesc.h: D2VAR(D2CLIENT, gnSkillDescLine2YOffset, int32_t, 0xECCF8)
// OURS, SkillDesc.h: using D2CLIENT_gpSkillDescDamFuncs_ft = void (__fastcall *)(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
// OURS, SkillDesc.h: D2VAR(D2CLIENT, gpSkillDescDamFuncs, D2CLIENT_gpSkillDescDamFuncs_ft, 0xECD00)   // [36]
// OURS, SkillDesc.h: using D2CLIENT_gpSkillDescAttFuncs_ft = void (__fastcall *)(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnAttackRatingA, int32_t *pnColorA, int32_t *pnAttackRatingB, int32_t *pnColorB);
// OURS, SkillDesc.h: D2VAR(D2CLIENT, gpSkillDescAttFuncs, D2CLIENT_gpSkillDescAttFuncs_ft, 0xECD90)   // [18]
// OURS, SkillDesc.h: D2VAR(D2CLIENT, gSkillDescPanelLayout, int32_t, 0xECDD8)   // [36]
// OURS, SkillDesc.h: D2VAR(D2CLIENT, gszPlus, char, 0xECE8C)   // [2]
// OURS, skillstree.h: D2VAR(D2CLIENT, gnSelectedSkillPage, int32_t, 0xECF00)
// OURS, skillstree.h: D2VAR(D2CLIENT, sgptPlayerSkillTreeChart, D2ClientSkillTreeChartStrc, 0xECF08)   // [7]
// OURS, skillstree.h: D2VAR(D2CLIENT, gnSkillTreeTab, int32_t, 0xECFF8)
// OURS, skillstree.h: D2VAR(D2CLIENT, gnSkillTabXOffsets, int32_t, 0xECFFC)   // [22]
// OURS, spellsel.h: D2VAR(D2CLIENT, gClassSkillIcons, D2ClientClassSkillIconStrc, 0xED0C8)   // [7]
// OURS, text.h: D2VAR(D2CLIENT, gnPopupY, int32_t, 0xED220)
// OURS, text.h: D2VAR(D2CLIENT, gPopupMessageHandlers, D2WinCommandRegisterStrc, 0xED228)   // [7]
// OURS, text.h: D2VAR(D2CLIENT, gnHoverMsgOffsetSigns, int32_t, 0xED27C)   // [8]
// OURS, text.h: D2VAR(D2CLIENT, gnHoverMsgOffsetSignCount, int32_t, 0xED29C)
// OURS, text.h: D2VAR(D2CLIENT, gQuestMenuStrings, D2QuestMenuStringStrc, 0xED2A0)   // [527]
// OURS, text.h: D2VAR(D2CLIENT, gnQuestMenuStringCount, int32_t, 0xEDADC)
// OURS, text.h: D2VAR(D2CLIENT, sgRunePos, D2CoordStrc, 0xEDAE0)   // [5]
// OURS, text.h: D2VAR(D2CLIENT, sgRuneOverlayPos, D2CoordStrc, 0xEDB08)   // [5]
// OURS, text.h: D2VAR(D2CLIENT, gnQuestScrollFrameThresholds, int32_t, 0xEDB30)   // [5]
// OURS, text.h: D2VAR(D2CLIENT, gSpeechSubtitlesAmazon, D2ClientSpeechSubtitleStrc, 0xEDB48)   // [74]
// OURS, text.h: D2VAR(D2CLIENT, gSpeechSubtitlesSorceress, D2ClientSpeechSubtitleStrc, 0xEDC70)   // [74]
// OURS, text.h: D2VAR(D2CLIENT, gSpeechSubtitlesNecromancer, D2ClientSpeechSubtitleStrc, 0xEDD98)   // [74]
// OURS, text.h: D2VAR(D2CLIENT, gSpeechSubtitlesPaladin, D2ClientSpeechSubtitleStrc, 0xEDEC0)   // [74]
// OURS, text.h: D2VAR(D2CLIENT, gSpeechSubtitlesBarbarian, D2ClientSpeechSubtitleStrc, 0xEDFE8)   // [74]
// OURS, text.h: D2VAR(D2CLIENT, gSpeechSubtitlesDruid, D2ClientSpeechSubtitleStrc, 0xEE110)   // [74]
// OURS, text.h: D2VAR(D2CLIENT, gSpeechSubtitlesAssassin, D2ClientSpeechSubtitleStrc, 0xEE238)   // [74]
// OURS, text.h: D2VAR(D2CLIENT, gpSpeechSubtitleTables, D2ClientSpeechSubtitleStrc *, 0xEE360)   // [7]
// OURS, text.h: D2VAR(D2CLIENT, gMonsterSpeechSubtitles, D2ClientSpeechSubtitleStrc, 0xEE37C)   // [6]
// OURS, text.h: D2VAR(D2CLIENT, gszAssertGlobalMessageLen, char, 0xEE540)   // [109]
// OURS, ui.h: D2VAR(D2CLIENT, gnLabelScreenX, int32_t, 0xEEA28)
// OURS, ui.h: D2VAR(D2CLIENT, gnLabelScreenY, int32_t, 0xEEA2C)
// OURS, ui.h: D2VAR(D2CLIENT, gNpcshopMessageHandlers, D2WinCommandRegisterStrc, 0xEEA30)   // [8]
// OURS, ui.h: D2VAR(D2CLIENT, gQuestscreenMessageHandlers, D2WinCommandRegisterStrc, 0xEEA90)   // [7]
// OURS, ui.h: D2VAR(D2CLIENT, gTradeMessageHandlers, D2WinCommandRegisterStrc, 0xEEAE8)   // [13]
// OURS, ui.h: D2VAR(D2CLIENT, gStashMessageHandlers, D2WinCommandRegisterStrc, 0xEEB88)   // [8]
// OURS, ui.h: D2VAR(D2CLIENT, gCubeMessageHandlers, D2WinCommandRegisterStrc, 0xEEBE8)   // [7]
// OURS, ui.h: D2VAR(D2CLIENT, gBeltMessageHandlers, D2WinCommandRegisterStrc, 0xEEC3C)   // [2]
// OURS, ui.h: D2VAR(D2CLIENT, gLevelEntryQuestTitles, D2ClientLevelEntryTitleStrc, 0xEEC54)   // [2]
// OURS, ui.h: D2VAR(D2CLIENT, gnLevelEntryQuestTitleCount, int32_t, 0xEEC70)
// OURS, ui.h: D2VAR(D2CLIENT, gnChargeUpIconRightInset, int32_t, 0xEEC74)
// OURS, ui.h: D2VAR(D2CLIENT, gnChargeUpIconBottomInset, int32_t, 0xEEC78)
// OURS, ui.h: D2VAR(D2CLIENT, gnChargeUpIconBottomInset800, int32_t, 0xEEC7C)
// OURS, ui.h: D2VAR(D2CLIENT, gnUiToggleRuleRows, int32_t, 0xEEC80)   // [38][38]
// OURS, ui.h: D2VAR(D2CLIENT, gaUiToggleRules, int32_t *, 0xF0310)   // [38]
// OURS, ui.h: D2VAR(D2CLIENT, sgNumStateTbl, int32_t, 0xF03A8)
// OURS, ui.h: D2VAR(D2CLIENT, gbPanelToggleDelayEnabled, BOOL, 0xF03AC)
// OURS, ui.h: D2VAR(D2CLIENT, gszExpansion, char, 0xF04DC)   // [10]
// OURS, useitem.h: D2VAR(D2CLIENT, gAnvilPanelButtons, D2AnvilPanelButtonStrc, 0xF0660)   // [2]
// OURS, useitem.h: D2VAR(D2CLIENT, gnPanelButtonCount, int32_t, 0xF06B0)
// OURS, useitem.h: D2VAR(D2CLIENT, gAnvilMsgHandlers, D2WinCommandRegisterStrc, 0xF06B8)   // [8]
// OURS, waypoint.h: D2VAR(D2CLIENT, gWaypointActTable, D2ClientWaypointActStrc, 0xF0778)   // [5]
// OURS, waypoint.h: D2VAR(D2CLIENT, gnWaypointActCount, int32_t, 0xF07B0)
// OURS, waypoint.h: D2VAR(D2CLIENT, gWaypointCoords, D2WaypointCoordTableStrc, 0xF07B8)   // [9]
// OURS, CUnit.h: D2VAR(D2CLIENT, gdwNextClientUnitId, uint32_t, 0xF08FC)
// OURS, CUnit.h: using D2CLIENT_gpSkillSelectProcs_ft = int32_t (__fastcall *)(int32_t, D2UnitStrc *);
// OURS, CUnit.h: D2VAR(D2CLIENT, gpSkillSelectProcs, D2CLIENT_gpSkillSelectProcs_ft, 0xF0900)   // [7]
// OURS, CUnit.h: D2VAR(D2CLIENT, gnSkillSelectProcCount, int32_t, 0xF091C)
// OURS, Item.h: D2VAR(D2CLIENT, gaItemCursors, int32_t, 0xF0C28)   // [2]
// OURS, Item.h: D2VAR(D2CLIENT, gnItemCursorUIStates, int32_t, 0xF0C30)
// OURS, Missile.h: D2VAR(D2CLIENT, gaFireSprayMissiles, int32_t, 0xF1208)   // [3]
// OURS, Missile.h: D2VAR(D2CLIENT, gnLightQuality, int32_t, 0xF1214)
// OURS, Missile.h: D2VAR(D2CLIENT, gaMissileTargetTests, D2MissileUnitFindTableStrc, 0xF1218)   // [9]
// OURS, Missile.h: using D2CLIENT_gpMissileCltDoFuncs_ft = void (__fastcall *)(D2UnitStrc *pMissile);
// OURS, Missile.h: D2VAR(D2CLIENT, gpMissileCltDoFuncs, D2CLIENT_gpMissileCltDoFuncs_ft, 0xF1260)   // [91]
// OURS, Missile.h: D2VAR(D2CLIENT, gnMissileCltDoFuncCount, int32_t, 0xF13CC)
// OURS, Missile.h: using D2CLIENT_gpMissileCltHitFuncs_ft = int32_t (__fastcall *)(D2UnitStrc *, D2UnitStrc *);
// OURS, Missile.h: D2VAR(D2CLIENT, gpMissileCltHitFuncs, D2CLIENT_gpMissileCltHitFuncs_ft, 0xF13D0)   // [81]
// OURS, Missile.h: D2VAR(D2CLIENT, gnMissileCltHitFuncCount, int32_t, 0xF1514)
// OURS, Monster.h: D2VAR(D2CLIENT, gMonModeDescs, D2ClientMonModeDescStrc, 0xF1590)   // [16]
// OURS, Monster.h: D2VAR(D2CLIENT, gMonModeDescsMoving, D2ClientMonModeDescStrc, 0xF1650)   // [16]
// OURS, MonUnique.h: using D2CLIENT_gpMonUModHandlers_ft = void (__fastcall *)(D2UnitStrc *pMonster, D2C_MonUMods nModId, int32_t bUnique);
// OURS, MonUnique.h: D2VAR(D2CLIENT, gpMonUModHandlers, D2CLIENT_gpMonUModHandlers_ft, 0xF17A8)   // [43]
// OURS, MonUnique.h: using D2CLIENT_gpMonUModCltFuncs_ft = void (__fastcall *)(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique);
// OURS, MonUnique.h: D2VAR(D2CLIENT, gpMonUModCltFuncs, D2CLIENT_gpMonUModCltFuncs_ft, 0xF1858)   // [43][5]
// OURS, MonUnique.h: D2VAR(D2CLIENT, gMonUModLabelStrings, D2C_StringIndices, 0xF1BB8)   // [44]
// OURS, Object.h: D2VAR(D2CLIENT, gbNpcSpeechAudio, int32_t, 0xF1C50)
// OURS, Object.h: D2VAR(D2CLIENT, gaNpcGreetingSounds, D2ClientNpcSpeechStrc, 0xF1C54)   // [29]
// OURS, Object.h: D2VAR(D2CLIENT, gSpeechSoundTable, D2ClientSpeechSoundStrc, 0xF1F10)   // [865]
// OURS, Object.h: D2VAR(D2CLIENT, gnShrineChartSize, int32_t, 0xF3AC0)
// OURS, Object.h: using D2CLIENT_sgpfnProcessFunctions_ft = uint8_t (__fastcall *)(D2UnitStrc *pUnit);
// OURS, Object.h: D2VAR(D2CLIENT, sgpfnProcessFunctions, D2CLIENT_sgpfnProcessFunctions_ft, 0xF3AC8)   // [19]
// OURS, Player.h: D2VAR(D2CLIENT, gnAnimModeControl, int32_t, 0xF3CF8)   // [20][3]
// OURS, Player.h: D2VAR(D2CLIENT, sgSnds, int32_t, 0xF3DE8)   // [9]
// OURS, Player.h: D2VAR(D2CLIENT, sgNumSnds, int32_t, 0xF3E0C)
// OURS, PlrSkills.h: using D2CLIENT_gpStateSetFuncs_ft = void (__fastcall *)(D2UnitStrc *, D2C_States);
// OURS, PlrSkills.h: D2VAR(D2CLIENT, gpStateSetFuncs, D2CLIENT_gpStateSetFuncs_ft, 0xF3EE0)   // [32]
// OURS, PlrSkills.h: using D2CLIENT_gpStateRemFuncs_ft = void (__fastcall *)(D2UnitStrc *, D2C_States);
// OURS, PlrSkills.h: D2VAR(D2CLIENT, gpStateRemFuncs, D2CLIENT_gpStateRemFuncs_ft, 0xF3F60)   // [30]
// OURS, UnitSnd.h: D2VAR(D2CLIENT, gHitClassSounds, D2HitClassSoundStrc, 0xF40D0)   // [14]
// OURS, UnitSnd.h: D2VAR(D2CLIENT, gObjectModeSoundTable, D2ClientObjectModeSoundStrc, 0xF4140)   // [121]
// OURS, UnitSnd.h: D2VAR(D2CLIENT, gpObjectModeSounds, D2ClientObjectModeSoundStrc *, 0xF57F0)   // [573]
// OURS, UnitSnd.h: D2VAR(D2CLIENT, gPlayerClassSoundTable, D2C_Sounds, 0xF60F8)   // [7][11]
// OURS, UnitSnd.h: D2VAR(D2CLIENT, gpPlayerClassSounds, D2C_Sounds *, 0xF622C)   // [7]
// OURS, UnitSnd.h: D2VAR(D2CLIENT, gAreaEntryQuestLineTable, D2AreaEntryQuestLineStrc, 0xF6248)   // [14]
// OURS, UnitSnd.h: D2VAR(D2CLIENT, gpAreaEntryQuestLines, D2AreaEntryQuestLineStrc *, 0xF64E8)   // [16]
// OURS, SoundChan.h: D2VAR(D2CLIENT, gnCurrentEaxEnvironment, int32_t, 0xF661C)
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gszSfxPathPrefix, char, 0xF6A84)   // [6]
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gszMusicPathPrefix, char, 0xF6A8C)   // [8]
// OURS, SoundInst.h: D2VAR(D2CLIENT, gnSoundPauseFadePercent, int32_t, 0xF6A94)
// OURS, SoundInst.h: D2VAR(D2CLIENT, gnSoundDuckPercent, int32_t, 0xF6A98)
// OURS, Cursor.h: D2VAR(D2CLIENT, gCursorMessageHandlers, D2WinCommandRegisterStrc, 0xF6B30)   // [3]
// OURS, Cursor.h: D2VAR(D2CLIENT, gCursorDescs, CursorDescStrc, 0xF6B58)   // [7]
// OURS, Cursor.h: D2VAR(D2CLIENT, gnCursors, uint32_t, 0xF6C1C)
// OURS, Gfx.h: D2VAR(D2CLIENT, gPreloadComponents, D2Composits, 0xF6E08)   // [8]
// OURS, Gfx.h: D2VAR(D2CLIENT, gnPreloadComponentCount, int32_t, 0xF6E28)
// OURS, Gfx.h: D2VAR(D2CLIENT, gPreloadAnimModes, int32_t, 0xF6E2C)   // [15]
// OURS, Gfx.h: D2VAR(D2CLIENT, gnPreloadAnimModeCount, int32_t, 0xF6E68)
// OURS, Gfx.h: D2VAR(D2CLIENT, gActCofDataFiles, const char *, 0xF6E6C)   // [5]
// OURS, Gfx.h: D2VAR(D2CLIENT, gMonModeTokenOverrides, D2ClientMonModeTokenOverrideStrc, 0xF6E80)   // [1]
// OURS, Gfx.h: D2VAR(D2CLIENT, gnMonModeOverrideCount, uint32_t, 0xF6E88)
// OURS, CoreArchive.h: D2VAR(D2CLIENT, gnViewportBottom, int32_t, 0xFA700)
// OURS, CoreArchive.h: D2VAR(D2CLIENT, gnViewportRight, int32_t, 0xFA704)
// OURS, CoreArchive.h: D2VAR(D2CLIENT, gbHighResolution, int32_t, 0xFA708)
// OURS, DLLmain.h: D2VAR(D2CLIENT, ghD2ClientModule, HMODULE, 0xFA70C)
// OURS, Lng.h: D2VAR(D2CLIENT, ghAccelTable, HACCEL, 0xFA710)
// OURS, Lng.h: D2VAR(D2CLIENT, szLangDllPath, char, 0xFA718)   // [260]
// OURS, Lng.h: D2VAR(D2CLIENT, ghLangModule, HMODULE, 0xFA81C)
// OURS, Video.h: D2VAR(D2CLIENT, gbGameCompletedClassic, uint8_t, 0xFA820)
// OURS, Video.h: D2VAR(D2CLIENT, gbCinematicOnScreen, uint8_t, 0xFA821)
// OURS, Video.h: D2VAR(D2CLIENT, gbUseFullSizeCinematics, uint8_t, 0xFA822)
// OURS, Video.h: D2VAR(D2CLIENT, gbCinematicPlaying, uint8_t, 0xFA823)
// OURS, Video.h: D2VAR(D2CLIENT, gbActVIntroPending, uint8_t, 0xFA824)
// OURS, WINMAIN.h: D2VAR(D2CLIENT, gszEmptyString, char, 0xFA828)
// OURS, WINMAIN.h: D2VAR(D2CLIENT, Filename, char, 0xFA830)   // [260]
// OURS, WINMAIN.h: D2VAR(D2CLIENT, sgaAppModeStack, D2C_AppMode, 0xFA934)   // [5]
// OURS, WINMAIN.h: D2VAR(D2CLIENT, gpLaunchConfig, D2ConfigStrc *, 0xFA948)
// OURS, WINMAIN.h: D2VAR(D2CLIENT, ghClientInstance, HINSTANCE, 0xFA94C)
// OURS, WINMAIN.h: D2VAR(D2CLIENT, gnEntrypointReturnCode, int32_t, 0xFA950)
// OURS, WINMAIN.h: D2VAR(D2CLIENT, hAccTableAlt, HACCEL, 0xFA954)
// OURS, WINMAIN.h: D2VAR(D2CLIENT, gbCommandLineHelpShown, BOOL, 0xFA958)
// OURS, WINMAIN.h: D2VAR(D2CLIENT, sgnLastAppmode, int32_t, 0xFA95C)
// OURS, Draw.h: D2VAR(D2CLIENT, gbAskedForGDI, BOOL, 0xFA968)
// OURS, Draw.h: D2VAR(D2CLIENT, gnFramesPerSecond, int32_t, 0xFA96C)
// OURS, Draw.h: D2VAR(D2CLIENT, gnFpsWindowTick, int32_t, 0xFA970)
// OURS, Draw.h: D2VAR(D2CLIENT, gnFpsFrameCounter, int32_t, 0xFA974)
// OURS, Draw.h: D2VAR(D2CLIENT, gnFpsSkipCounter, int32_t, 0xFA978)
// OURS, Draw.h: D2VAR(D2CLIENT, gnFpsSkipStartTick, int32_t, 0xFA97C)
// OURS, Draw.h: D2VAR(D2CLIENT, gbDebugOverlay, int32_t, 0xFA980)
// OURS, Draw.h: D2VAR(D2CLIENT, gnDebugOverlayPage, int32_t, 0xFA988)
// OURS, dFloor.h: D2VAR(D2CLIENT, gFloorDrawCtx, D2FloorDrawCtxStrc, 0xFA990)
// OURS, dLightMap.h: D2VAR(D2CLIENT, gnPropagateStepY, int32_t, 0xFA9D8)
// OURS, dLightMap.h: D2VAR(D2CLIENT, gStaticLightGrid, int32_t, 0xFA9E0)   // [4096]
// OURS, dLightMap.h: D2VAR(D2CLIENT, gnLightGridOriginX, int32_t, 0xFE9E0)
// OURS, dLightMap.h: D2VAR(D2CLIENT, gnLightGridOriginY, int32_t, 0xFE9E4)
// OURS, dLightMap.h: D2VAR(D2CLIENT, gnLightGridEndX, int32_t, 0xFE9E8)
// OURS, dLightMap.h: D2VAR(D2CLIENT, gnLightGridEndY, int32_t, 0xFE9EC)
// OURS, dLightMap.h: D2VAR(D2CLIENT, gnPropagateStepX, int32_t, 0xFE9F0)
// OURS, dLightMap.h: D2VAR(D2CLIENT, gLightGrid, uint32_t, 0xFE9F8)   // [4096]
// OURS, dLightMap.h: D2VAR(D2CLIENT, gnReciprocal16, int32_t, 0x1029F8)   // [256]
// OURS, dLightMap.h: D2VAR(D2CLIENT, gnPropagateFrac, int32_t, 0x102DF8)
// OURS, dLightMap.h: D2VAR(D2CLIENT, gPlayerLightGrid, D2LightGridCellStrc, 0x102E00)   // [2304]
// OURS, dLightMap.h: D2VAR(D2CLIENT, gnPropagateMode, int32_t, 0x107600)
// OURS, dLightMap.h: D2VAR(D2CLIENT, gpDynamicLights, D2ClientLightStrc *, 0x107604)
// OURS, dLightMap.h: D2VAR(D2CLIENT, gnDLightAllocCount, int32_t, 0x107608)
// OURS, dLightMap.h: D2VAR(D2CLIENT, gnDLightFreeCount, int32_t, 0x10760C)
// OURS, dLightMap.h: D2VAR(D2CLIENT, gnActiveDynamicLights, int32_t, 0x107610)
// OURS, dLightMap.h: D2VAR(D2CLIENT, gnDLightQualityLevel, int32_t, 0x107614)
// OURS, dLightMap.h: D2VAR(D2CLIENT, gnDLightQualityLastTick, int32_t, 0x107618)
// OURS, dLightMap.h: D2VAR(D2CLIENT, gdwWarpFlashEndTime, uint32_t, 0x10761C)
// OURS, dLightMap.h: D2VAR(D2CLIENT, gnAmbientLightGreen, uint8_t, 0x107620)
// OURS, dLightMap.h: D2VAR(D2CLIENT, gnAmbientLightBlue, uint8_t, 0x107621)
// OURS, dLightMap.h: D2VAR(D2CLIENT, gnAmbientLightRed, uint8_t, 0x107622)
// OURS, dLightMap.h: D2VAR(D2CLIENT, gnAmbientLightIntensity, uint8_t, 0x107623)
// OURS, Env.h: D2VAR(D2CLIENT, gEnvRainColoursTwilight, char, 0x10762C)   // [12]
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvWindTarget, int32_t, 0x107638)
// OURS, Env.h: D2VAR(D2CLIENT, gflWeatherParticleIntensity, float, 0x10763C)
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvThunderTimer, int32_t, 0x107640)
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvWind, int32_t, 0x107644)
// OURS, Env.h: D2VAR(D2CLIENT, gpEnvBubbleImages, D2CellFileStrc *, 0x107648)   // [4]
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvTargetParticleCount, int32_t, 0x107658)
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvWindChangeTimer, int32_t, 0x10765C)
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvPhaseDurationRange, int32_t, 0x107660)   // [4]
// OURS, Env.h: D2VAR(D2CLIENT, gEnvRainColoursDay, char, 0x107670)   // [12]
// OURS, Env.h: D2VAR(D2CLIENT, gEnvSnowColours, char, 0x10767C)   // [12]
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvLightningDelay, int32_t, 0x107688)
// OURS, Env.h: D2VAR(D2CLIENT, gEnvRainColoursNight, char, 0x10768C)   // [12]
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvRainFrameCounts, int32_t, 0x107698)   // [4]
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvPeakParticleCount, uint32_t, 0x1076A8)
// OURS, Env.h: D2VAR(D2CLIENT, gbEnvThunderScheduled, int32_t, 0x1076AC)
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvWindTargetBase, int32_t, 0x1076B0)
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvWindTargetBase2, int32_t, 0x1076B4)
// OURS, Env.h: D2VAR(D2CLIENT, gpEnvRainImages, D2CellFileStrc *, 0x1076B8)   // [4]
// OURS, Env.h: D2VAR(D2CLIENT, gEnvSnowColoursFallback, char, 0x1076C8)   // [12]
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvBubbleFrameCounts, int32_t, 0x1076D4)   // [4]
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvPhaseDurationMin, int32_t, 0x1076E4)   // [4]
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvWindTargetRange, int32_t, 0x1076F4)
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvWindTargetRange2, int32_t, 0x1076F8)
// OURS, Env.h: D2VAR(D2CLIENT, ghEnvSplashes, D2DataArrayStrc *, 0x1076FC)
// OURS, Env.h: D2VAR(D2CLIENT, ghEnvBubbles, D2DataArrayStrc *, 0x107700)
// OURS, Env.h: D2VAR(D2CLIENT, ghEnvParticles, D2DataArrayStrc *, 0x107704)
// OURS, Env.h: D2VAR(D2CLIENT, gbEnvLightningActive, BOOL, 0x107708)
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvLastWeatherFrame, uint32_t, 0x10770C)
// OURS, Env.h: D2VAR(D2CLIENT, gbPlayerRoomLosDraw, int32_t, 0x107710)
// OURS, Env.h: D2VAR(D2CLIENT, gbEnvWeatherIsSnow, BOOL, 0x107714)
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvParticleFrameBias, int32_t, 0x107718)
// OURS, Env.h: D2VAR(D2CLIENT, gbEnvWeatherForced, BOOL, 0x10771C)
// OURS, Env.h: D2VAR(D2CLIENT, gbEnvRainStateLocked, BOOL, 0x107720)
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvRainState, uint32_t, 0x107724)
// OURS, Env.h: D2VAR(D2CLIENT, gbEnvWindInitDone, BOOL, 0x107728)
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvParticleSwayPhase, uint32_t, 0x10772C)
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvPhaseFramesLeft, uint32_t, 0x107730)
// OURS, Env.h: D2VAR(D2CLIENT, gnEnvPhaseFramesTotal, uint32_t, 0x107734)
// OURS, Env.h: D2VAR(D2CLIENT, gbEnvWasRaining, BOOL, 0x107738)
// OURS, Env.h: D2VAR(D2CLIENT, gbEnvWasMuddy, BOOL, 0x10773C)
// OURS, Game.h: D2VAR(D2CLIENT, gnJoinGameId, int16_t, 0x107748)
// OURS, Game.h: D2VAR(D2CLIENT, gnGameThreadId, DWORD, 0x10774C)
// OURS, Game.h: D2VAR(D2CLIENT, gnActiveMessageCallbackIndex, int32_t, 0x107750)
// OURS, Game.h: D2VAR(D2CLIENT, gpfnActiveMessageCallback, D2ClientMessageCallbackFn, 0x107754)
// OURS, Game.h: D2VAR(D2CLIENT, ghClientPoolSystem, void *, 0x107758)
// OURS, Game.h: D2VAR(D2CLIENT, gdwFrameTick, uint32_t, 0x10775C)
// OURS, Game.h: D2VAR(D2CLIENT, gdwLastGameTickTime, uint32_t, 0x107760)
// OURS, Game.h: D2VAR(D2CLIENT, gnTotalGameTicks, int32_t, 0x107764)
// OURS, Game.h: D2VAR(D2CLIENT, gnClientFrame, uint32_t, 0x107768)
// OURS, Game.h: D2VAR(D2CLIENT, gnNextPingTick, int32_t, 0x10776C)
// OURS, Game.h: D2VAR(D2CLIENT, gnPingSentTick, int32_t, 0x107770)
// OURS, Game.h: D2VAR(D2CLIENT, gnNetworkLatency, int32_t, 0x107774)
// OURS, Game.h: D2VAR(D2CLIENT, gnRecentTickRate, int32_t, 0x107778)
// OURS, Game.h: D2VAR(D2CLIENT, gnFrameCountInStatsWindow, int32_t, 0x10777C)
// OURS, Game.h: D2VAR(D2CLIENT, gnRecentSkipRate, int32_t, 0x107780)
// OURS, Game.h: D2VAR(D2CLIENT, gnSkippedTickCountInStatsWindow, int32_t, 0x107784)
// OURS, Game.h: D2VAR(D2CLIENT, gdwFrameStatsWindowStart, uint32_t, 0x107788)
// OURS, Game.h: D2VAR(D2CLIENT, gdwCatchUpResetDeadline, uint32_t, 0x10778C)
// OURS, Game.h: D2VAR(D2CLIENT, UNRESOLVED_dword_6FBA7794, int32_t, 0x107794)
// OURS, Game.h: D2VAR(D2CLIENT, gnInteractThrottleTick, uint32_t, 0x107798)
// OURS, Game.h: D2VAR(D2CLIENT, gnLatencySampleCount, uint32_t, 0x10779C)
// OURS, Game.h: D2VAR(D2CLIENT, gnNetworkLatencyRunningMean, uint32_t, 0x1077A0)
// OURS, Game.h: D2VAR(D2CLIENT, gPongPacketBody, uint32_t, 0x1077A4)   // [8]
// OURS, Game.h: D2VAR(D2CLIENT, gbExpansionGame, BOOL, 0x1077C4)
// OURS, Game.h: D2VAR(D2CLIENT, gbLadderGame, BOOL, 0x1077C8)
// OURS, Game.h: D2VAR(D2CLIENT, gnSkillBlockUntilFrame, int32_t, 0x1077CC)
// OURS, Game.h: D2VAR(D2CLIENT, gnConnectionTimestamp, uint32_t, 0x1077D0)
// OURS, Game.h: D2VAR(D2CLIENT, gGameLoopCritSect, struct _RTL_CRITICAL_SECTION, 0x1077D4)
// OURS, Game.h: D2VAR(D2CLIENT, gnDisconnectMessage, D2C_JoinErrors, 0x1077EC)
// OURS, Game.h: D2VAR(D2CLIENT, gszRealmName, char, 0x1077F0)   // [32]
// OURS, Game.h: D2VAR(D2CLIENT, gLocalPlayerName, char, 0x107810)   // [16]
// OURS, Game.h: D2VAR(D2CLIENT, ghGameThread, HANDLE, 0x107824)
// OURS, Game.h: D2VAR(D2CLIENT, gGameData, SGameDataTableStrc, 0x107828)
// OURS, Game.h: D2VAR(D2CLIENT, gnJoinTokenId, int32_t, 0x107890)
// OURS, Game.h: D2VAR(D2CLIENT, gnPlayerClass, uint8_t, 0x107894)
// OURS, Game.h: D2VAR(D2CLIENT, szJoinGameName, char, 0x107898)   // [24]
// OURS, Game.h: D2VAR(D2CLIENT, gOsVersionInfo, struct _OSVERSIONINFOA, 0x1078B0)
// OURS, Game.h: D2VAR(D2CLIENT, gpComInterface, struct BnClientInterface *, 0x107944)
// OURS, Game.h: D2VAR(D2CLIENT, gnGameStartTick, int32_t, 0x107948)
// OURS, Game.h: D2VAR(D2CLIENT, gbUseGreenBlood, int32_t, 0x10794C)
// OURS, Game.h: D2VAR(D2CLIENT, gbBetaSlidesRegistered, int32_t, 0x107950)
// OURS, Game.h: D2VAR(D2CLIENT, gbEndOfGamePending, BOOL, 0x107954)
// OURS, Game.h: D2VAR(D2CLIENT, gbVanquishedDismissed, BOOL, 0x107958)
// OURS, Game.h: D2VAR(D2CLIENT, gnDifficulty, uint8_t, 0x10795C)
// OURS, Game.h: D2VAR(D2CLIENT, gnGameType, eD2GameTypes, 0x107960)
// OURS, Game.h: D2VAR(D2CLIENT, gbConnectedToGame, int32_t, 0x107968)
// OURS, Game.h: D2VAR(D2CLIENT, gbInGame, BOOL, 0x10796C)
// OURS, Game.h: D2VAR(D2CLIENT, gbDisconnecting, BOOL, 0x107970)
// OURS, Game.h: D2VAR(D2CLIENT, gbGameExitPending, int32_t, 0x107974)
// OURS, Game.h: D2VAR(D2CLIENT, gbD2xOutCinematicPending, BOOL, 0x107978)
// OURS, Game.h: D2VAR(D2CLIENT, gbGameTeardownActive, BOOL, 0x10797C)
// OURS, Game.h: D2VAR(D2CLIENT, gGameList, uint8_t, 0x107980)
// OURS, Game.h: D2VAR(D2CLIENT, gpAct, D2DrlgActStrc *, 0x107984)
// OURS, Game.h: D2VAR(D2CLIENT, gpClientGameBlock, void *, 0x107988)
// OURS, Game.h: D2VAR(D2CLIENT, gnPreloadedLevelId, int32_t, 0x10798C)
// OURS, Game.h: D2VAR(D2CLIENT, gpView, D2ClientViewStrc *, 0x107990)
// OURS, Game.h: D2VAR(D2CLIENT, gnBetaScreenIndex, uint8_t, 0x107994)
// OURS, Game.h: D2VAR(D2CLIENT, gbOsVersionKnown, BOOL, 0x107998)
// OURS, Game.h: D2VAR(D2CLIENT, gnDisconnectCountdown, int32_t, 0x10799C)
// OURS, Game.h: D2VAR(D2CLIENT, gbGameThreadStop, int32_t, 0x1079A0)
// OURS, Game.h: D2VAR(D2CLIENT, gbGameLoopEnabled, int32_t, 0x1079A4)
// OURS, Game.h: D2VAR(D2CLIENT, gbLeftButtonDown, BOOL, 0x1079A8)
// OURS, Game.h: D2VAR(D2CLIENT, gbRightButtonDown, BOOL, 0x1079AC)
// OURS, Game.h: D2VAR(D2CLIENT, gbSuppressHeldClick, BOOL, 0x1079B0)
// OURS, Game.h: D2VAR(D2CLIENT, gbRunKeyHeld, BOOL, 0x1079B4)
// OURS, Game.h: D2VAR(D2CLIENT, gbRunToggled, BOOL, 0x1079B8)
// OURS, Game.h: D2VAR(D2CLIENT, gbStandStillKeyHeld, BOOL, 0x1079BC)
// OURS, Game.h: D2VAR(D2CLIENT, gbLeftClickIssuedThisFrame, BOOL, 0x1079C4)
// OURS, Game.h: D2VAR(D2CLIENT, gbRightClickIssuedThisFrame, BOOL, 0x1079C8)
// OURS, Game.h: D2VAR(D2CLIENT, gnMouseWheelAccum, int32_t, 0x1079CC)
// OURS, Game.h: D2VAR(D2CLIENT, gbActEnvironmentDirty, uint8_t, 0x1079D0)
// OURS, Game.h: D2VAR(D2CLIENT, gbLeaveGameSent, BOOL, 0x1079D4)
// OURS, Game.h: D2VAR(D2CLIENT, gnViewScrollDeltaX, int32_t, 0x1079D8)
// OURS, Game.h: D2VAR(D2CLIENT, gnViewScrollDeltaY, int32_t, 0x1079DC)
// OURS, Game.h: D2VAR(D2CLIENT, gnFramesSinceLastTick, int32_t, 0x1079E0)
// OURS, Game.h: D2VAR(D2CLIENT, gbVanquishedButtonHeld, BOOL, 0x1079E4)
// OURS, Game.h: D2VAR(D2CLIENT, gpImgVanquishedScreen, D2CellFileStrc *, 0x1079E8)
// OURS, Game.h: D2VAR(D2CLIENT, gpImgVanquishedOkButton, D2CellFileStrc *, 0x1079EC)
// OURS, Msg.h: D2VAR(D2CLIENT, gnLastPing6CTick, uint32_t, 0x1079F0)
// OURS, Msg.h: D2VAR(D2CLIENT, gnLoadProgressRatio, int32_t, 0x1079F4)
// OURS, Msg.h: D2VAR(D2CLIENT, gLastSentPacket, uint8_t, 0x1079F8)   // [512]
// OURS, Msg.h: D2VAR(D2CLIENT, gnLastPacketSentTick, uint32_t, 0x107BF8)
// OURS, Msg.h: D2VAR(D2CLIENT, gbJoinGameLoopDone, int32_t, 0x107BFC)
// OURS, Msg.h: D2VAR(D2CLIENT, gnGameJoinStartTickCount, uint32_t, 0x107C00)
// OURS, Msg.h: D2VAR(D2CLIENT, gnGameLeaveStartTickCount, uint32_t, 0x107C04)
// OURS, Msg.h: D2VAR(D2CLIENT, gbGameAllocated, int32_t, 0x107C08)
// OURS, PalShift.h: D2VAR(D2CLIENT, gpMonPalShift, D2ClientMonPalShiftStrc *, 0x107C10)
// OURS, PalShift.h: D2VAR(D2CLIENT, gpMonPalShiftCache, D2ClientPalShiftStrc *, 0x107C14)
// OURS, PalShift.h: D2VAR(D2CLIENT, gRandTransformTables, uint8_t, 0x107C18)   // [30][256]
// OURS, PalShift.h: D2VAR(D2CLIENT, gbRandTransformsLoaded, int32_t, 0x109A18)
// OURS, PalShift.h: D2VAR(D2CLIENT, gpGreenBloodRemap, uint8_t *, 0x109A1C)
// OURS, Record.h: D2VAR(D2CLIENT, gRecordFileHeader, D2ClientRecordFileHeaderStrc, 0x109A20)
// OURS, Record.h: using D2CLIENT_gpfnPreviousCleanupHandler_ft = int32_t (__stdcall *)(int32_t, int32_t, int32_t);
// OURS, Record.h: D2VAR(D2CLIENT, gpfnPreviousCleanupHandler, D2CLIENT_gpfnPreviousCleanupHandler_ft, 0x109A34)
// OURS, Record.h: D2VAR(D2CLIENT, gRecordHolderFirst, D2ClientRecordHolderStrc, 0x109A38)
// OURS, Record.h: D2VAR(D2CLIENT, gbRecordingRequested, int32_t, 0x10B984)
// OURS, Record.h: D2VAR(D2CLIENT, gbRecordSaveOnCleanupInstalled, int32_t, 0x10B988)
// OURS, Record.h: D2VAR(D2CLIENT, gbPlaybackRequested, int32_t, 0x10B98C)
// OURS, Record.h: D2VAR(D2CLIENT, gbRecordPlaybackLoaded, int32_t, 0x10B990)
// OURS, Roster.h: D2VAR(D2CLIENT, gpPartyEntries, D2RosterUnitStrc *, 0x10B994)
// OURS, Roster.h: D2VAR(D2CLIENT, gpPendingRosterEntries, D2RosterUnitStrc *, 0x10B998)
// OURS, Roster.h: D2VAR(D2CLIENT, gpRosterList, D2RosterGroupStrc *, 0x10B99C)
// OURS, RosterPets.h: D2VAR(D2CLIENT, gpRosterPetList, D2RosterPetStrc *, 0x10B9A0)
// OURS, SCmd.h: D2VAR(D2CLIENT, gpScmdBuffer, void *, 0x10B9A4)
// OURS, SCmd.h: D2VAR(D2CLIENT, gnScmdBufferSize, int32_t, 0x10B9A8)
// OURS, SCmd.h: D2VAR(D2CLIENT, sgnMaxUnitPacketDataSize, int32_t, 0x10B9B0)
// OURS, SCmd.h: D2VAR(D2CLIENT, gnLastRoutedPacketId, uint8_t, 0x10B9B4)
// OURS, scroll.h: D2VAR(D2CLIENT, gnViewportOriginY, int32_t, 0x10B9B8)
// OURS, scroll.h: D2VAR(D2CLIENT, gnViewportOriginX, int32_t, 0x10B9BC)
// OURS, scroll.h: D2VAR(D2CLIENT, gnUiShiftOffsetX, int32_t, 0x10B9C0)
// OURS, scroll.h: D2VAR(D2CLIENT, gUiShiftScreen, D2C_UIpanels, 0x10B9C4)
// OURS, scroll.h: D2VAR(D2CLIENT, gUiPlayerPosAdjust, int32_t, 0x10B9C8)
// OURS, Select.h: D2VAR(D2CLIENT, gnSelectedUnitType, int32_t, 0x10B9CC)
// OURS, Select.h: D2VAR(D2CLIENT, gnSelectedUnitGUID, int32_t, 0x10B9D0)
// OURS, Select.h: D2VAR(D2CLIENT, gnClickRegionLeft, int32_t, 0x10B9D4)
// OURS, Select.h: D2VAR(D2CLIENT, gnClickRegionTop, int32_t, 0x10B9D8)
// OURS, Select.h: D2VAR(D2CLIENT, gnClickRegionRight, int32_t, 0x10B9DC)
// OURS, Select.h: D2VAR(D2CLIENT, gnClickRegionBottom, int32_t, 0x10B9E0)
// OURS, Select.h: D2VAR(D2CLIENT, gbNoPickup, int32_t, 0x10B9E4)
// OURS, Select.h: D2VAR(D2CLIENT, gbHasSelectedUnit, int32_t, 0x10B9E8)
// OURS, Select.h: D2VAR(D2CLIENT, gbSelectionLocked, int32_t, 0x10B9EC)
// OURS, Select.h: D2VAR(D2CLIENT, gbClickRegionValid, int32_t, 0x10B9F0)
// OURS, Select.h: D2VAR(D2CLIENT, gbSelectionRoomSeen, int32_t, 0x10B9F4)
// OURS, Select.h: D2VAR(D2CLIENT, gpLastSelectionRoom, D2ActiveRoomStrc *, 0x10B9F8)
// OURS, View.h: D2VAR(D2CLIENT, gnArcaneStarY, int32_t, 0x10BA00)   // [256]
// OURS, View.h: D2VAR(D2CLIENT, gnArcaneStarX, int32_t, 0x10BE00)   // [256]
// OURS, View.h: D2VAR(D2CLIENT, gnParallaxOriginX, int32_t, 0x10C200)
// OURS, View.h: D2VAR(D2CLIENT, gnScreenShakeRampDownMs, uint32_t, 0x10C204)
// OURS, View.h: D2VAR(D2CLIENT, gnScreenShakeRampUpMs, uint32_t, 0x10C208)
// OURS, View.h: D2VAR(D2CLIENT, gnScreenShakeOffsetY, uint32_t, 0x10C20C)
// OURS, View.h: D2VAR(D2CLIENT, ghScreenShakeSound, int32_t, 0x10C210)
// OURS, View.h: D2VAR(D2CLIENT, gnViewScaleFactor, int32_t, 0x10C214)
// OURS, View.h: D2VAR(D2CLIENT, gnParallaxCloudX, int32_t, 0x10C218)   // [10]
// OURS, View.h: D2VAR(D2CLIENT, gShadowLightGammaPalette, D2ShadowLightGammaPaletteStrc, 0x10C240)
// OURS, View.h: D2VAR(D2CLIENT, gbParallaxOriginLatched, uint8_t, 0x10F340)
// OURS, View.h: D2VAR(D2CLIENT, gdwScreenShakeStartTime, int32_t, 0x10F344)
// OURS, View.h: D2VAR(D2CLIENT, gParallaxCloudSeedStorage, D2SeedStrc, 0x10F348)
// OURS, View.h: D2VAR(D2CLIENT, gnParallaxCloudY, int32_t, 0x10F350)   // [10]
// OURS, View.h: D2VAR(D2CLIENT, gnArcaneStarTick, int32_t, 0x10F378)
// OURS, View.h: D2VAR(D2CLIENT, gEnvSeedStorage, D2SeedStrc, 0x10F37C)
// OURS, View.h: D2VAR(D2CLIENT, gnArcaneStarColors, uint8_t, 0x10F388)   // [256]
// OURS, View.h: D2VAR(D2CLIENT, gnScreenShakeHoldMs, uint32_t, 0x10F488)
// OURS, View.h: D2VAR(D2CLIENT, gnArcaneStarVelX, int32_t, 0x10F490)   // [256]
// OURS, View.h: D2VAR(D2CLIENT, gnParallaxCloudSpeedX, int32_t, 0x10F890)   // [10]
// OURS, View.h: D2VAR(D2CLIENT, gArcaneStarPalette, char, 0x10F8B8)   // [8]
// OURS, View.h: D2VAR(D2CLIENT, gnViewEffectsLastFrame, int32_t, 0x10F8C0)
// OURS, View.h: D2VAR(D2CLIENT, gnScreenShakeMagnitude, uint32_t, 0x10F8C4)
// OURS, View.h: D2VAR(D2CLIENT, gnScreenShakeOffsetX, uint32_t, 0x10F8C8)
// OURS, View.h: D2VAR(D2CLIENT, sgpGouraudTblX, uint8_t **, 0x10F8CC)
// OURS, View.h: D2VAR(D2CLIENT, sgpGouraudTblY, uint8_t *, 0x10F8D0)
// OURS, View.h: D2VAR(D2CLIENT, sgpGouraudTblXVal, uint8_t *, 0x10F8D4)
// OURS, View.h: D2VAR(D2CLIENT, gpSubtileGouraudTbl, uint8_t **, 0x10F8DC)
// OURS, View.h: D2VAR(D2CLIENT, gpTileGouraudTbl, uint8_t *, 0x10F8E0)
// OURS, View.h: D2VAR(D2CLIENT, gpImgParallaxSummit, D2CellFileStrc *, 0x10F8E4)
// OURS, View.h: D2VAR(D2CLIENT, gpImgParallaxCloud, D2CellFileStrc *, 0x10F8E8)
// OURS, View.h: D2VAR(D2CLIENT, gbArcaneStarsSeeded, int32_t, 0x10F8EC)
// OURS, View.h: D2VAR(D2CLIENT, gbParallaxCloudsSeeded, uint8_t, 0x10F8F0)
// OURS, View.h: D2VAR(D2CLIENT, gnParallaxOffsetX, int32_t, 0x10F8F4)
// OURS, View.h: D2VAR(D2CLIENT, gbParallaxOriginForced, uint8_t, 0x10F8F8)
// OURS, View.h: D2VAR(D2CLIENT, gbDebugDrawLightGrid, int32_t, 0x10F8FC)
// OURS, Wall2.h: D2VAR(D2CLIENT, gnPlayerCoordListIndex, int32_t, 0x10F900)
// OURS, Wall2.h: D2VAR(D2CLIENT, gnPlayerTileY, uint32_t, 0x10F904)
// OURS, Wall2.h: D2VAR(D2CLIENT, gnPlayerTileX, uint32_t, 0x10F908)
// OURS, Wall2.h: D2VAR(D2CLIENT, gWallClipRect, RECT, 0x10F90C)
// OURS, Quests.h: D2VAR(D2CLIENT, gQuestStates, char, 0x10F91C)   // [37]
// OURS, Quests.h: D2VAR(D2CLIENT, gbQuestStatesReceived, int32_t, 0x10F944)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gbQuestFxFadeActive, int32_t, 0x10F948)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gnQuestFxIntensity, int32_t, 0x10F94C)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gnQuestFxLightRed, uint8_t, 0x10F950)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gnQuestFxLightGreen, uint8_t, 0x10F951)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gnQuestFxLightBlue, uint8_t, 0x10F952)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gnQuestFxFadeOutFrames, int32_t, 0x10F954)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gnQuestFxFadeInFrames, int32_t, 0x10F958)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gnQuestFxDarkHoldFrames, int32_t, 0x10F95C)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gnQuestFxFadeTotalFrames, int32_t, 0x10F960)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gnQuestFxFadeFrame, int32_t, 0x10F964)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gnQuestFxFadeCosPhase, int32_t, 0x10F968)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gnQuestFxFadeLevelId, int32_t, 0x10F96C)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gbQuestFxFadeLocked, int32_t, 0x10F970)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gdwQuestTriggersFired, uint32_t, 0x10F974)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gbDenOfEvilLightDone, int32_t, 0x10F978)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gbDiabloAppearsTintActive, int32_t, 0x10F97C)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gnGameEndDeadline, int32_t, 0x10F980)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gbWorldstoneShakeArmed, int32_t, 0x10F984)
// OURS, QuestsFX.h: D2VAR(D2CLIENT, gbWorldstoneShakeSpawned, int32_t, 0x10F988)
// OURS, Skills.h: D2VAR(D2CLIENT, gnOpenWoundsNextFrame, uint32_t, 0x10F98C)
// OURS, automap.h: D2VAR(D2CLIENT, gaAutomapCellGroup, int32_t, 0x10F990)   // [2048]
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapMode, int32_t, 0x111990)
// OURS, automap.h: D2VAR(D2CLIENT, gpAutomapDrawWriteCursor, uint8_t *, 0x111994)
// OURS, automap.h: D2VAR(D2CLIENT, gpAutomapCellBlocks, void *, 0x111998)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapCellsUsed, int32_t, 0x11199C)
// OURS, automap.h: D2VAR(D2CLIENT, gpAutomapLayers, D2AutomapLayerStrc *, 0x1119A0)
// OURS, automap.h: D2VAR(D2CLIENT, gpAutomapCurrentLayer, D2AutomapLayerStrc *, 0x1119A4)
// OURS, automap.h: D2VAR(D2CLIENT, gpAutomapIconCels, D2CellFileStrc *, 0x1119A8)   // [4]
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapIconCelCounts, int32_t, 0x1119B8)   // [4]
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapPanX, int32_t, 0x1119C8)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapPanY, int32_t, 0x1119CC)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapScrollX, int32_t, 0x1119D0)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapScrollY, int32_t, 0x1119D4)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapOriginX, int32_t, 0x1119D8)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapOriginY, int32_t, 0x1119DC)
// OURS, automap.h: D2VAR(D2CLIENT, gbAutomapRevealAll, BOOL, 0x1119E0)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapRevealDelay, int32_t, 0x1119E4)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapFade, int32_t, 0x1119E8)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapFadeSaved, int32_t, 0x1119EC)
// OURS, automap.h: D2VAR(D2CLIENT, gAutomapSymbolColors, uint8_t, 0x1119F0)   // [11]
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapTextY, int32_t, 0x1119FC)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapMiniLeftUnused, int32_t, 0x111A00)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapMiniTopUnused, int32_t, 0x111A04)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapClipMinX, int32_t, 0x111A08)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapClipMinY, int32_t, 0x111A0C)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapClipMaxX, int32_t, 0x111A10)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapClipMaxY, int32_t, 0x111A14)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapMiniTop, int32_t, 0x111A18)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapMiniLeft, int32_t, 0x111A1C)
// OURS, automap.h: D2VAR(D2CLIENT, gbAutomapLeft, BOOL, 0x111A20)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapLeftSaved, int32_t, 0x111A24)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapLeftSetting, int32_t, 0x111A28)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapClipLatchPortraits, int32_t, 0x111A2C)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapClipLatchLeft, int32_t, 0x111A30)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapLastRevealX, int32_t, 0x111A34)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapLastRevealY, int32_t, 0x111A38)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapPlayerX, int32_t, 0x111A3C)
// OURS, automap.h: D2VAR(D2CLIENT, gnAutomapPlayerYUnused, int32_t, 0x111A40)
// OURS, char.h: D2VAR(D2CLIENT, gbGoldDialogOpenUnread, BOOL, 0x111A44)
// OURS, char.h: D2VAR(D2CLIENT, gbStatsButtonDown, BOOL, 0x111A4C)
// OURS, char.h: D2VAR(D2CLIENT, gnCharPanelHiliteColor, uint32_t, 0x111A50)
// OURS, chat.h: D2VAR(D2CLIENT, gChatbuff, Unicode, 0x111A58)   // [257]
// OURS, chat.h: D2VAR(D2CLIENT, gwszImePrefix, Unicode, 0x111C60)   // [32]
// OURS, chat.h: D2VAR(D2CLIENT, gnChatHistoryCursor, int32_t, 0x111CA0)
// OURS, chat.h: D2VAR(D2CLIENT, gChatHistory, Unicode, 0x111CA8)   // [8][257]
// OURS, chat.h: D2VAR(D2CLIENT, gwszImeComposition, Unicode, 0x112CB8)   // [32]
// OURS, chat.h: D2VAR(D2CLIENT, gbChatCaretVisible, uint8_t, 0x112CF8)
// OURS, chat.h: D2VAR(D2CLIENT, gnChatLanguage, D2C_Language, 0x112CFC)
// OURS, chat.h: D2VAR(D2CLIENT, gnChatHistoryCount, uint16_t, 0x112D00)
// OURS, chat.h: D2VAR(D2CLIENT, gnChatInputLength, uint32_t, 0x112D04)
// OURS, chat.h: D2VAR(D2CLIENT, gnChatInputDrawTick, int32_t, 0x112D08)
// OURS, chat.h: D2VAR(D2CLIENT, gnChatImeMode, int32_t, 0x112D0C)
// OURS, chat.h: D2VAR(D2CLIENT, gpChatCandidateList, CANDIDATELIST *, 0x112D10)
// OURS, chat.h: D2VAR(D2CLIENT, gbChatInputActive, uint8_t, 0x112D14)
// OURS, chat.h: D2VAR(D2CLIENT, gnChatBlinkLastTick, uint32_t, 0x112D18)
// OURS, CmdTbl.h: D2VAR(D2CLIENT, sgpfnMouse5LiftFn, FARPROC, 0x112D20)
// OURS, CmdTbl.h: D2VAR(D2CLIENT, gwszUnknownKeyNameFallback, Unicode, 0x112D24)   // [4]
// OURS, CmdTbl.h: D2VAR(D2CLIENT, sgpfnMouse5DownFn, FARPROC, 0x112D2C)
// OURS, CmdTbl.h: D2VAR(D2CLIENT, sgpfnMouseWheelDownFn, FARPROC, 0x112D30)
// OURS, CmdTbl.h: D2VAR(D2CLIENT, sgpfnMouse4LiftFn, FARPROC, 0x112D34)
// OURS, CmdTbl.h: D2VAR(D2CLIENT, gKeyBindings, HotkeyMappingEntry, 0x112D38)   // [114]
// OURS, CmdTbl.h: D2VAR(D2CLIENT, sgpfnMouse4DownFn, FARPROC, 0x1131AC)
// OURS, CmdTbl.h: D2VAR(D2CLIENT, sgSavedCmdMapping, HotkeyMappingEntry, 0x1131B0)   // [114]
// OURS, CmdTbl.h: D2VAR(D2CLIENT, sgpfnMouse3DownFn, FARPROC, 0x113624)
// OURS, CmdTbl.h: D2VAR(D2CLIENT, sgpfnMouse3LiftFn, FARPROC, 0x113628)
// OURS, CmdTbl.h: D2VAR(D2CLIENT, sgpfnMouseWheelUpFn, FARPROC, 0x11362C)
// OURS, CmdTbl.h: D2VAR(D2CLIENT, gnKeyHookMode, int32_t, 0x113630)
// OURS, CmdTbl.h: D2VAR(D2CLIENT, gbKeyCommandHandlersRegistered, BOOL, 0x113634)
// OURS, CmdTbl.h: D2VAR(D2CLIENT, gbKeyCommandReleaseHandlersRegistered, BOOL, 0x113638)
// OURS, dialog.h: D2VAR(D2CLIENT, gnDialogWheelHandlersRefCount, int32_t, 0x11363C)
// OURS, dialog.h: D2VAR(D2CLIENT, gnDialogCelFrameCounter, int32_t, 0x113640)
// OURS, dialog.h: D2VAR(D2CLIENT, gpDialogList, D2ClientDialogStrc *, 0x113644)
// OURS, dialog.h: D2VAR(D2CLIENT, gbDialogHandlersRegistered, BOOL, 0x113648)
// OURS, dialog.h: D2VAR(D2CLIENT, gnDialogNestDepth, int32_t, 0x11364C)
// OURS, help.h: D2VAR(D2CLIENT, gpHelpBackgroundCel, D2CellFileStrc *, 0x113650)
// OURS, help.h: D2VAR(D2CLIENT, gpHelpPointerCel, D2CellFileStrc *, 0x113654)
// OURS, help.h: D2VAR(D2CLIENT, gpHelpBulletCel, D2CellFileStrc *, 0x113658)
// OURS, help.h: D2VAR(D2CLIENT, gnHelpDimColor, uint8_t, 0x11365C)
// OURS, help.h: D2VAR(D2CLIENT, gbHelpCloseButtonDown, int32_t, 0x113660)
// OURS, help.h: D2VAR(D2CLIENT, gbHelpOpenedMiniPanel, int32_t, 0x113664)
// OURS, help.h: D2VAR(D2CLIENT, gbHelpButtonDown, BOOL, 0x113668)
// OURS, help.h: D2VAR(D2CLIENT, gbHelpHintSuppressed, int32_t, 0x11366C)
// OURS, Hireables.h: D2VAR(D2CLIENT, gpPetIconCache, D2ClientPetIconCacheStrc *, 0x113670)
// OURS, Hireables.h: D2VAR(D2CLIENT, gpClassIcons, D2CellFileStrc *, 0x113674)   // [7]
// OURS, Hireables.h: D2VAR(D2CLIENT, gnPartyPortraitMode, int32_t, 0x113690)
// OURS, Hireables.h: D2VAR(D2CLIENT, gnLifeBarColorHigh, uint8_t, 0x113694)
// OURS, Hireables.h: D2VAR(D2CLIENT, gnLifeBarColorMid, uint8_t, 0x113695)
// OURS, Hireables.h: D2VAR(D2CLIENT, gnLifeBarColorLow, uint8_t, 0x113696)
// OURS, Hireables.h: D2VAR(D2CLIENT, gbLifeBarColorsInit, uint8_t, 0x113697)
// OURS, Hireables.h: D2VAR(D2CLIENT, gnHireIconHintX, int32_t, 0x113698)
// OURS, Hireables.h: D2VAR(D2CLIENT, gnHireIconHintY, int32_t, 0x11369C)
// OURS, Hireables.h: D2VAR(D2CLIENT, gbHireIconHintShown, BOOL, 0x1136A0)
// OURS, Hireables.h: D2VAR(D2CLIENT, gbHireIconHovered, BOOL, 0x1136A4)
// OURS, Hireables.h: D2VAR(D2CLIENT, gbHirelingPopupPending, uint8_t, 0x1136A8)
// OURS, Hireables.h: D2VAR(D2CLIENT, gnHireIconHoverTick, uint32_t, 0x1136AC)
// OURS, InsertDisc.h: D2VAR(D2CLIENT, gnDiscPromptKind, int32_t, 0x1136B0)
// OURS, InsertDisc.h: D2VAR(D2CLIENT, gpDiscPromptFrameCel, D2CellFileStrc *, 0x1136B4)
// OURS, InsertDisc.h: D2VAR(D2CLIENT, gpDiscPromptButtonCel, D2CellFileStrc *, 0x1136B8)
// OURS, InsertDisc.h: D2VAR(D2CLIENT, gpCinematicsPromptFrameCel, D2CellFileStrc *, 0x1136BC)
// OURS, InsertDisc.h: D2VAR(D2CLIENT, gbDiscPromptButtonDown, BOOL, 0x1136C0)
// OURS, InsertDisc.h: D2VAR(D2CLIENT, gnDiscPromptButtonFrame, int32_t, 0x1136C4)
// OURS, InsertDisc.h: D2VAR(D2CLIENT, gnDiscPromptButton2Frame, int32_t, 0x1136C8)
// OURS, InsertDisc.h: D2VAR(D2CLIENT, gbDiscPromptDone, int32_t, 0x1136CC)
// OURS, InsertDisc.h: D2VAR(D2CLIENT, gbDiscPromptAccepted, BOOL, 0x1136D0)
// OURS, InsertDisc.h: D2VAR(D2CLIENT, gbDiscPromptCancelled, BOOL, 0x1136D4)
// OURS, inv.h: D2VAR(D2CLIENT, gInvTrophyCaseRect, D2InvRectStrc, 0x1136D8)
// OURS, inv.h: D2VAR(D2CLIENT, gInvNpcShopRect, D2InvRectStrc, 0x1136E8)
// OURS, inv.h: D2VAR(D2CLIENT, gInvMercGrid, D2InventoryGridInfoStrc, 0x1136F8)
// OURS, inv.h: D2VAR(D2CLIENT, gInvPanelGrid, D2InventoryGridInfoStrc, 0x113710)
// OURS, inv.h: D2VAR(D2CLIENT, gInvStashGrid, D2InventoryGridInfoStrc, 0x113728)
// OURS, inv.h: D2VAR(D2CLIENT, gInvGuildVaultRect, D2InvRectStrc, 0x113740)
// OURS, inv.h: D2VAR(D2CLIENT, gInvBigStashRect, D2InvRectStrc, 0x113750)
// OURS, inv.h: D2VAR(D2CLIENT, gInvBigStashGrid, D2InventoryGridInfoStrc, 0x113760)
// OURS, inv.h: D2VAR(D2CLIENT, gInvHighlightColors, uint8_t, 0x113778)   // [5]
// OURS, inv.h: D2VAR(D2CLIENT, gInvMercRect, D2InvRectStrc, 0x113780)
// OURS, inv.h: D2VAR(D2CLIENT, UNRESOLVED_dword_6FBB3790, int32_t, 0x113790)
// OURS, inv.h: D2VAR(D2CLIENT, gInvStashRect, D2InvRectStrc, 0x113794)
// OURS, inv.h: D2VAR(D2CLIENT, gNpcShopTabs, D2InvButtonInfoStrc, 0x1137A8)   // [4]
// OURS, inv.h: D2VAR(D2CLIENT, gInvTradePage1Grid, D2InventoryGridInfoStrc, 0x1137F8)
// OURS, inv.h: D2VAR(D2CLIENT, gInvTradePage1Rect, D2InvRectStrc, 0x113810)
// OURS, inv.h: D2VAR(D2CLIENT, gInvTrophyCaseGrid, D2InventoryGridInfoStrc, 0x113820)
// OURS, inv.h: D2VAR(D2CLIENT, gInvTradePage2Grid, D2InventoryGridInfoStrc, 0x113838)
// OURS, inv.h: D2VAR(D2CLIENT, gInvCubeRect, D2InvRectStrc, 0x113850)
// OURS, inv.h: D2VAR(D2CLIENT, gInvTradePage2Rect, D2InvRectStrc, 0x113860)
// OURS, inv.h: D2VAR(D2CLIENT, gnLastItemUseTick, int32_t, 0x113870)
// OURS, inv.h: D2VAR(D2CLIENT, gInvGuildVaultGrid, D2InventoryGridInfoStrc, 0x113874)
// OURS, inv.h: D2VAR(D2CLIENT, gnUiMouseX, int32_t, 0x11388C)
// OURS, inv.h: D2VAR(D2CLIENT, gpImgTradePanel, D2CellFileStrc *, 0x113890)
// OURS, inv.h: D2VAR(D2CLIENT, gnUiMouseY, int32_t, 0x113894)
// OURS, inv.h: D2VAR(D2CLIENT, gInvNpcShopGrid, D2InventoryGridInfoStrc, 0x113898)
// OURS, inv.h: D2VAR(D2CLIENT, gInvCubeGrid, D2InventoryGridInfoStrc, 0x1138B0)
// OURS, inv.h: D2VAR(D2CLIENT, gInvPanelRect, D2InvRectStrc, 0x1138C8)
// OURS, inv.h: D2VAR(D2CLIENT, gwszRecipeScrollText, Unicode, 0x1138D8)   // [4096]
// OURS, inv.h: D2VAR(D2CLIENT, gbInvLayoutBuilt, BOOL, 0x1158D8)
// OURS, inv.h: D2VAR(D2CLIENT, gbInvNpcLayoutBuilt, BOOL, 0x1158DC)
// OURS, inv.h: D2VAR(D2CLIENT, gbShowItemDesc, BOOL, 0x1158E0)
// OURS, inv.h: D2VAR(D2CLIENT, gbShowSetItemDesc, BOOL, 0x1158E4)
// OURS, inv.h: D2VAR(D2CLIENT, gbDeselectShopTabsPending, BOOL, 0x1158E8)
// OURS, inv.h: D2VAR(D2CLIENT, sgInventoryMode, int32_t, 0x1158EC)
// OURS, inv.h: D2VAR(D2CLIENT, gpHoverItem, D2UnitStrc *, 0x1158F0)
// OURS, inv.h: D2VAR(D2CLIENT, gpHoverItemLast, D2UnitStrc *, 0x1158F4)
// OURS, inv.h: D2VAR(D2CLIENT, gnVendorPage, uint8_t, 0x115900)
// OURS, inv.h: D2VAR(D2CLIENT, gnVendorPageHoldUntil, uint32_t, 0x115904)
// OURS, inv.h: D2VAR(D2CLIENT, gnVendorMaxPage, uint8_t, 0x115908)
// OURS, inv.h: D2VAR(D2CLIENT, gbVendorPageRebuildPending, BOOL, 0x11590C)
// OURS, inv.h: D2VAR(D2CLIENT, gbHoradricAnimActive, BOOL, 0x115910)
// OURS, inv.h: D2VAR(D2CLIENT, gnHoradricAnimFrame, int32_t, 0x115914)
// OURS, inv.h: D2VAR(D2CLIENT, gdwHoradricAnimLastTick, uint32_t, 0x115918)
// OURS, inv.h: D2VAR(D2CLIENT, gpHoradricAnimImage, D2CellFileStrc *, 0x11591C)
// OURS, inv.h: D2VAR(D2CLIENT, gnHoverBodyLoc, int32_t, 0x115920)
// OURS, inv.h: D2VAR(D2CLIENT, gpImgBuySellTabs, D2CellFileStrc *, 0x115924)
// OURS, inv.h: D2VAR(D2CLIENT, gpImgMiniConvert, D2CellFileStrc *, 0x115928)
// OURS, inv.h: D2VAR(D2CLIENT, gpStashPanelImage, D2CellFileStrc *, 0x11592C)
// OURS, inv.h: D2VAR(D2CLIENT, gpImgCubePanel, D2CellFileStrc *, 0x115930)
// OURS, inv.h: D2VAR(D2CLIENT, gpImgGoldCoinButton, D2CellFileStrc *, 0x115934)
// OURS, inv.h: D2VAR(D2CLIENT, gpImgInvChar6Tab, D2CellFileStrc *, 0x115938)
// OURS, inv.h: D2VAR(D2CLIENT, gpTradeStashImage, D2CellFileStrc *, 0x11593C)
// OURS, inv.h: D2VAR(D2CLIENT, gnNpcShopTabHit, int32_t, 0x115940)
// OURS, inv.h: D2VAR(D2CLIENT, gnNpcShopTabAction, int32_t, 0x115944)
// OURS, inv.h: D2VAR(D2CLIENT, gbTransactionItemIsPlayers, BOOL, 0x115948)
// OURS, inv.h: D2VAR(D2CLIENT, gnWeaponSwapSet, int32_t, 0x11594C)
// OURS, inv.h: D2VAR(D2CLIENT, gbCubeOpenedFromStash, BOOL, 0x115950)
// OURS, inv.h: D2VAR(D2CLIENT, gbCubeCloseCmdSent, BOOL, 0x115954)
// OURS, inv.h: D2VAR(D2CLIENT, gInvBodyLocRects, D2InvCompGridStrc, 0x115958)   // [11]
// OURS, inv.h: D2VAR(D2CLIENT, gMercInvSlots, D2InvCompGridStrc, 0x115A38)   // [11]
// OURS, inv.h: D2VAR(D2CLIENT, gnInvLayoutRes, int32_t, 0x115B14)
// OURS, inv.h: D2VAR(D2CLIENT, gbTradeOwnAccepted, BOOL, 0x115B18)
// OURS, inv.h: D2VAR(D2CLIENT, gbTradeOtherAccepted, BOOL, 0x115B1C)
// OURS, inv.h: D2VAR(D2CLIENT, gbTradeAcceptChecked, BOOL, 0x115B20)
// OURS, inv.h: D2VAR(D2CLIENT, gbTradeCancelPending, BOOL, 0x115B24)
// OURS, inv.h: D2VAR(D2CLIENT, gbTradeAcceptBtnArmed, BOOL, 0x115B28)
// OURS, inv.h: D2VAR(D2CLIENT, gbTradeCancelBtnArmed, BOOL, 0x115B2C)
// OURS, inv.h: D2VAR(D2CLIENT, gbTradeAcceptBlocked, BOOL, 0x115B30)
// OURS, inv.h: D2VAR(D2CLIENT, gbGoldDialogOpen, BOOL, 0x115B34)
// OURS, inv.h: D2VAR(D2CLIENT, gbDropGoldBtnPressed, BOOL, 0x115B38)
// OURS, inv.h: D2VAR(D2CLIENT, gbWithdrawGoldBtnPressed, BOOL, 0x115B3C)
// OURS, inv.h: D2VAR(D2CLIENT, gbStashCloseBtnPressed, BOOL, 0x115B40)
// OURS, inv.h: D2VAR(D2CLIENT, gbStashCloseBtnArmed, BOOL, 0x115B44)
// OURS, inv.h: D2VAR(D2CLIENT, gbCubeCloseBtnPressed, BOOL, 0x115B48)
// OURS, inv.h: D2VAR(D2CLIENT, gbCubeCloseBtnArmed, BOOL, 0x115B4C)
// OURS, inv.h: D2VAR(D2CLIENT, gbCubeTransmuteBtnPressed, BOOL, 0x115B50)
// OURS, inv.h: D2VAR(D2CLIENT, gbCubeTransmuteBtnArmed, BOOL, 0x115B54)
// OURS, inv.h: D2VAR(D2CLIENT, gpImgInvArmor, D2CellFileStrc *, 0x115B70)
// OURS, inv.h: D2VAR(D2CLIENT, gpImgInvBelt, D2CellFileStrc *, 0x115B74)
// OURS, inv.h: D2VAR(D2CLIENT, gpImgInvBoots, D2CellFileStrc *, 0x115B78)
// OURS, inv.h: D2VAR(D2CLIENT, gpImgInvHelmGlove, D2CellFileStrc *, 0x115B7C)
// OURS, inv.h: D2VAR(D2CLIENT, gpImgInvRingAmulet, D2CellFileStrc *, 0x115B80)
// OURS, inv.h: D2VAR(D2CLIENT, gpImgInvWeapons, D2CellFileStrc *, 0x115B84)
// OURS, inv.h: D2VAR(D2CLIENT, gpImgTradeButton, D2CellFileStrc *, 0x115B88)
// OURS, inv.h: D2VAR(D2CLIENT, gpImgInvChar6TabCube, D2CellFileStrc *, 0x115B8C)
// OURS, inv.h: D2VAR(D2CLIENT, gpImgNpcInv, D2CellFileStrc *, 0x115B90)
// OURS, inv.h: D2VAR(D2CLIENT, gpRecipeScrollImage, D2CellFileStrc *, 0x115B94)
// OURS, inv.h: D2VAR(D2CLIENT, gbInvCloseBtnArmed, BOOL, 0x115B98)
// OURS, inv.h: D2VAR(D2CLIENT, gbInvCloseBtnArmedUnread, BOOL, 0x115B9C)
// OURS, inv.h: D2VAR(D2CLIENT, gbPanelBtnPressActive, BOOL, 0x115BA8)
// OURS, inv.h: D2VAR(D2CLIENT, gbMercCloseBtnPressed, BOOL, 0x115BAC)
// OURS, inv.h: D2VAR(D2CLIENT, gnRecipeScrollColor, int32_t, 0x115BB0)
// OURS, inv.h: D2VAR(D2CLIENT, gpImgLevelUpButton, D2CellFileStrc *, 0x115BB4)
// OURS, inv.h: D2VAR(D2CLIENT, gpImgLevelUpSocket, D2CellFileStrc *, 0x115BB8)
// OURS, inv.h: D2VAR(D2CLIENT, gbNewStatsButtonDown, BOOL, 0x115BBC)
// OURS, inv.h: D2VAR(D2CLIENT, gbNewSkillsButtonDown, BOOL, 0x115BC0)
// OURS, Listbox.h: D2VAR(D2CLIENT, gpImgListBoxScrollBar, D2CellFileStrc *, 0x115BC4)
// OURS, npcmenu.h: D2VAR(D2CLIENT, gnSelectedHirelingRow, int32_t, 0x115BC8)
// OURS, npcmenu.h: D2VAR(D2CLIENT, gNpcGoldButtonImage, D2CellFileStrc *, 0x115BCC)
// OURS, npcmenu.h: D2VAR(D2CLIENT, gbTransactionPending, uint8_t, 0x115BD0)
// OURS, npcmenu.h: D2VAR(D2CLIENT, gTransactionItemClassId, int32_t, 0x115BD1)
// OURS, npcmenu.h: D2VAR(D2CLIENT, gTransactionItemGUID, uint32_t, 0x115BD5)
// OURS, npcmenu.h: D2VAR(D2CLIENT, gnLastItemActionTick, uint32_t, 0x115BD9)
// OURS, npcmenu.h: D2VAR(D2CLIENT, gnItemActionBurstCount, uint32_t, 0x115BDD)
// OURS, npcmenu.h: D2VAR(D2CLIENT, gLastTransactionTick, uint32_t, 0x115BE1)
// OURS, npcmenu.h: D2VAR(D2CLIENT, gwszNpcConfirmText, Unicode, 0x115BE5)   // [40]
// OURS, npcmenu.h: D2VAR(D2CLIENT, sgUIState, D2NpcMenuUIStateStrc, 0x115C38)
// OURS, npcmenu.h: D2VAR(D2CLIENT, gnHireNpcUnitGUID, int32_t, 0x115D94)
// OURS, npcmenu.h: D2VAR(D2CLIENT, gNpcBuySellImage, D2CellFileStrc *, 0x115D98)
// OURS, npcmenu.h: D2VAR(D2CLIENT, gnMercResurrectCost, int32_t, 0x115D9C)
// OURS, npcmenu.h: D2VAR(D2CLIENT, gpNpcTransactionPlayer, D2UnitStrc *, 0x115DA0)
// OURS, npcmenu.h: D2VAR(D2CLIENT, UNRESOLVED_word_6FBB5DA4, uint16_t, 0x115DA4)
// OURS, npcmenu.h: D2VAR(D2CLIENT, gnLastTransactionSendTick, uint32_t, 0x115DA8)
// OURS, npcmenu.h: D2VAR(D2CLIENT, gbFoundOpenedEunuch, uint8_t, 0x115DAC)
// OURS, config.h: D2VAR(D2CLIENT, gnCfgScrollThumbTop, int32_t, 0x115DB0)
// OURS, config.h: D2VAR(D2CLIENT, gnCfgControlsRowCount, int32_t, 0x115DB4)
// OURS, config.h: D2VAR(D2CLIENT, gnCfgScrollThumbRows, int32_t, 0x115DB8)
// OURS, config.h: D2VAR(D2CLIENT, gnCfgMaxFirstVisibleRow, int32_t, 0x115DBC)
// OURS, config.h: D2VAR(D2CLIENT, gbCfgMouseButtonDown, BOOL, 0x115DC0)
// OURS, config.h: D2VAR(D2CLIENT, gbCfgMouseWasDown, BOOL, 0x115DC4)
// OURS, config.h: D2VAR(D2CLIENT, gaCfgRegionPressed, int32_t, 0x115DC8)   // [7]
// OURS, config.h: D2VAR(D2CLIENT, gnCfgThumbDragStartTop, int32_t, 0x115DE4)
// OURS, config.h: D2VAR(D2CLIENT, gnCfgThumbDragStartY, int32_t, 0x115DE8)
// OURS, config.h: D2VAR(D2CLIENT, gnCfgFirstVisibleRow, int32_t, 0x115DEC)
// OURS, config.h: D2VAR(D2CLIENT, gnCfgSelectedRow, int32_t, 0x115DF0)
// OURS, config.h: D2VAR(D2CLIENT, gnCfgHoveredButton, int32_t, 0x115DF4)
// OURS, config.h: D2VAR(D2CLIENT, gbCfgCapturingBind, BOOL, 0x115DF8)
// OURS, config.h: D2VAR(D2CLIENT, gpImgCfgTextSlider, D2CellFileStrc *, 0x115DFC)
// OURS, config.h: D2VAR(D2CLIENT, gnCfgScrollRepeatTick, uint32_t, 0x115E00)
// OURS, config.h: D2VAR(D2CLIENT, gnCfgBindErrorStrId, D2C_StringIndices, 0x115E04)
// OURS, config.h: D2VAR(D2CLIENT, gnCfgBindErrorTick, uint32_t, 0x115E08)
// OURS, config.h: D2VAR(D2CLIENT, gnCfgFrameCounter, uint32_t, 0x115E0C)
// OURS, config.h: D2VAR(D2CLIENT, gnCfgLastMouseX, int32_t, 0x115E10)
// OURS, config.h: D2VAR(D2CLIENT, gnCfgLastMouseY, int32_t, 0x115E14)
// OURS, panel.h: D2VAR(D2CLIENT, gpImgMiniPanelToggle, D2CellFileStrc *, 0x115E18)
// OURS, panel.h: D2VAR(D2CLIENT, gpImgPopBelt, D2CellFileStrc *, 0x115E1C)
// OURS, panel.h: D2VAR(D2CLIENT, gpImgPanelArrow, D2CellFileStrc *, 0x115E20)
// OURS, panel.h: D2VAR(D2CLIENT, gpImgInvCharPanel, D2CellFileStrc *, 0x115E24)
// OURS, panel.h: D2VAR(D2CLIENT, gpImgDuroIcon, D2CellFileStrc *, 0x115E28)
// OURS, panel.h: D2VAR(D2CLIENT, gpImgPanelButton, D2CellFileStrc *, 0x115E2C)
// OURS, panel.h: D2VAR(D2CLIENT, gpImgGlobeFill, D2CellFileStrc *, 0x115E30)
// OURS, panel.h: D2VAR(D2CLIENT, gpImgGemSocket, D2CellFileStrc *, 0x115E34)
// OURS, panel.h: D2VAR(D2CLIENT, gnPanelHighlightColours, uint8_t, 0x115E38)   // [4]
// OURS, panel.h: D2VAR(D2CLIENT, gPanel800BorderImage, D2CellFileStrc *, 0x115E3C)
// OURS, panel.h: D2VAR(D2CLIENT, gpImgHostilePic, D2CellFileStrc *, 0x115E40)
// OURS, panel.h: D2VAR(D2CLIENT, gPanelCtrlPnl7Image, D2CellFileStrc *, 0x115E44)
// OURS, panel.h: D2VAR(D2CLIENT, gpImgRunButton, D2CellFileStrc *, 0x115E48)
// OURS, panel.h: D2VAR(D2CLIENT, gpImgMenuPanel, D2CellFileStrc *, 0x115E4C)
// OURS, panel.h: D2VAR(D2CLIENT, gpImgSkillPoints, D2CellFileStrc *, 0x115E50)
// OURS, panel.h: D2VAR(D2CLIENT, gPanel800CtrlPnl7Image, D2CellFileStrc *, 0x115E54)
// OURS, panel.h: D2VAR(D2CLIENT, ghWarnIconCel, D2CellFileStrc *, 0x115E58)
// OURS, panel.h: D2VAR(D2CLIENT, gOrbAnimState, int32_t, 0x115E5C)   // [12]
// OURS, panel.h: D2VAR(D2CLIENT, gpImgGlobeOverlay, D2CellFileStrc *, 0x115E8C)
// OURS, panel.h: D2VAR(D2CLIENT, gbMiniSkillLeftBtnDownUnread, BOOL, 0x115E90)
// OURS, panel.h: D2VAR(D2CLIENT, gbMiniSkillRightBtnDownUnread, BOOL, 0x115E94)
// OURS, panel.h: D2VAR(D2CLIENT, gbBeltHoverActive, BOOL, 0x115EA0)
// OURS, panel.h: D2VAR(D2CLIENT, gbBeltPanelVisible, BOOL, 0x115EA4)
// OURS, panel.h: D2VAR(D2CLIENT, gbBeltPanelHovered, BOOL, 0x115EA8)
// OURS, panel.h: D2VAR(D2CLIENT, gnHintPhaseSuppressed, int32_t, 0x115EAC)
// OURS, panel.h: D2VAR(D2CLIENT, gbGameLButtonDown, BOOL, 0x115EB0)
// OURS, panel.h: D2VAR(D2CLIENT, gpBeltHoverItem, D2UnitStrc *, 0x115EB4)
// OURS, panel.h: D2VAR(D2CLIENT, gpBeltHoverItemLast, D2UnitStrc *, 0x115EB8)
// OURS, panel.h: D2VAR(D2CLIENT, gbBeltColumnHasItem, char, 0x115EBC)   // [4]
// OURS, panel.h: D2VAR(D2CLIENT, gaWarnIconSlotUsed, uint8_t, 0x115EC0)   // [6]
// OURS, panel.h: D2VAR(D2CLIENT, gbMiniPanelTogglePressed, BOOL, 0x115EDC)
// OURS, panel.h: D2VAR(D2CLIENT, gbMiniPanelToggleParityUnread, BOOL, 0x115EE0)
// OURS, panel.h: D2VAR(D2CLIENT, gbRunButtonDown, BOOL, 0x115EE4)
// OURS, panel.h: D2VAR(D2CLIENT, gnStaminaBarColours, uint8_t, 0x115EE8)   // [4]
// OURS, panel.h: D2VAR(D2CLIENT, gnPanelLastLevelId, int32_t, 0x115EF0)
// OURS, panel.h: D2VAR(D2CLIENT, gnPanelLevelChangeTimer, int32_t, 0x115EF4)
// OURS, Party.h: D2VAR(D2CLIENT, gnPartyRowPartyIds, int32_t, 0x115EF8)   // [8]
// OURS, Party.h: D2VAR(D2CLIENT, gpPartyRowList, D2ClientPartyRowStrc *, 0x115F18)
// OURS, Party.h: D2VAR(D2CLIENT, gpPartyPanelImage, D2CellFileStrc *, 0x115F1C)
// OURS, Party.h: D2VAR(D2CLIENT, gpPartyButtonsImage, D2CellFileStrc *, 0x115F20)
// OURS, Party.h: D2VAR(D2CLIENT, gpPartyBoxesImage, D2CellFileStrc *, 0x115F24)
// OURS, Party.h: D2VAR(D2CLIENT, gpPartyGoldButtonImage, D2CellFileStrc *, 0x115F28)
// OURS, Party.h: D2VAR(D2CLIENT, gpPartyBarImage, D2CellFileStrc *, 0x115F2C)
// OURS, Party.h: D2VAR(D2CLIENT, gbPartyButtonDown, BOOL, 0x115F34)
// OURS, Party.h: D2VAR(D2CLIENT, gnUiBoxFillColor, uint8_t, 0x115F38)
// OURS, Party.h: D2VAR(D2CLIENT, gbPartyRowButtonDown, BOOL, 0x115F3C)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnQuestLogSelectedPerAct, int32_t, 0x115F40)   // [5]
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnQuestLogPanelState, int32_t, 0x115F58)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnQuestLogEntryCount, int32_t, 0x115F60)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnMonstersRemaining, int32_t, 0x115F64)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnQuestLogPictureFrame, int32_t, 0x115F68)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnBarbsToBeRescued, int32_t, 0x115F6C)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gbQuestLogCloseBoxDown, uint8_t, 0x115F70)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gbQuestScreenIconDown, uint8_t, 0x115F71)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gbQuestScreenCloseButtonDown, uint8_t, 0x115F72)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gbQuestScreenSpeechPlaying, uint8_t, 0x115F73)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gbQuestScreenSpeechButtonDown, uint8_t, 0x115F74)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnQuestScreenPressedQuest, int32_t, 0x115F75)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnQuestLogSelectedQuest, int32_t, 0x115F79)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnQuestLogLastClickedPerAct, int32_t, 0x115F7D)   // [5]
// OURS, QuestLog.h: D2VAR(D2CLIENT, gQuestLogSocketsImage, D2CellFileStrc *, 0x115F91)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gQuestLogBackgroundImage, D2CellFileStrc *, 0x115F95)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gQuestLogTabsImage, D2CellFileStrc *, 0x115F99)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gQuestLogDoneImage, D2CellFileStrc *, 0x115F9D)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gQuestLogIconImages, D2CellFileStrc *, 0x115FA1)   // [27]
// OURS, QuestLog.h: D2VAR(D2CLIENT, gQuestLogLastImage, D2CellFileStrc *, 0x11600D)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gQuestLogInvPsImage, D2CellFileStrc *, 0x116011)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gQuestListPacketCopy, uint8_t, 0x116015)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gQuestLogEntryFlags, char, 0x116016)   // [41]
// OURS, QuestLog.h: D2VAR(D2CLIENT, gQuestLogEntryColours, uint8_t, 0x116040)   // [41]
// OURS, QuestLog.h: D2VAR(D2CLIENT, gQuestLogEntries, D2QuestLogEntryStrc, 0x116069)   // [6]
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnQuestLogIconFrame, int32_t, 0x116EE5)   // [6]
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnQuestLogIconLastTick, int32_t, 0x116EFD)   // [6]
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnQuestLogActTab, int32_t, 0x116F15)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gdwQuestLogCloseTick, uint32_t, 0x116F19)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gbDurielCofLoaded, uint8_t, 0x116F1D)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gbAncientStatueCofLoaded, uint8_t, 0x116F1E)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gbZooArmed, uint8_t, 0x116F1F)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gbZooChickensSpawned, uint8_t, 0x116F20)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnZooChickenUnitId, int32_t, 0x116F21)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnZooMonsterId, int32_t, 0x116F25)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnQuestLogDoneXOffset, int32_t, 0x116F2C)
// OURS, QuestLog.h: D2VAR(D2CLIENT, gnQuestLogSocketYOffset, int32_t, 0x116F30)
// OURS, Restart.h: D2VAR(D2CLIENT, gaMiniPanelButtonX, int32_t, 0x116F38)   // [8]
// OURS, Restart.h: D2VAR(D2CLIENT, gaMiniPanelButtonY, int32_t, 0x116F58)   // [8]
// OURS, Restart.h: D2VAR(D2CLIENT, gaMiniPanelButtonId, int32_t, 0x116F78)   // [8]
// OURS, Restart.h: D2VAR(D2CLIENT, gbMiniPanelButtonPressed, int32_t, 0x116F98)   // [8]
// OURS, Restart.h: D2VAR(D2CLIENT, gaMiniPanelButtonFrame, int32_t, 0x116FB8)   // [8]
// OURS, Restart.h: D2VAR(D2CLIENT, gnEscMenuSelectedItem, int32_t, 0x116FD8)
// OURS, Restart.h: D2VAR(D2CLIENT, sgptMenuInfo, D2ClientEscMenuInfoStrc *, 0x116FDC)
// OURS, Restart.h: D2VAR(D2CLIENT, sgptMenuItem, D2ClientEscMenuItemStrc *, 0x116FE0)
// OURS, Restart.h: D2VAR(D2CLIENT, gnEscMenuPentagramFrame, int32_t, 0x116FE4)
// OURS, Restart.h: D2VAR(D2CLIENT, gnEscMenuDragging, BOOL, 0x116FE8)
// OURS, Restart.h: D2VAR(D2CLIENT, gbEscMenuSliderDragging, BOOL, 0x116FEC)
// OURS, Restart.h: D2VAR(D2CLIENT, ghEscMenuOptBar, D2CellFileStrc *, 0x116FF0)
// OURS, Restart.h: D2VAR(D2CLIENT, ghEscMenuOptBarC, D2CellFileStrc *, 0x116FF4)
// OURS, Restart.h: D2VAR(D2CLIENT, ghEscMenuOptSkull, D2CellFileStrc *, 0x116FF8)
// OURS, Restart.h: D2VAR(D2CLIENT, gnEscMenuPentagramWidth, int32_t, 0x116FFC)
// OURS, Restart.h: D2VAR(D2CLIENT, gnEscMenuLastMouseX, int32_t, 0x117000)
// OURS, Restart.h: D2VAR(D2CLIENT, gnEscMenuLastMouseY, int32_t, 0x117004)
// OURS, Restart.h: D2VAR(D2CLIENT, gpImgMiniPanel, D2CellFileStrc *, 0x117008)
// OURS, Restart.h: D2VAR(D2CLIENT, gpImgMiniPanelButtons, D2CellFileStrc *, 0x11700C)
// OURS, Restart.h: D2VAR(D2CLIENT, gbMiniPanelInvSideOpen, int32_t, 0x117010)
// OURS, Restart.h: D2VAR(D2CLIENT, gbMiniPanelWindowOpen, int32_t, 0x117014)
// OURS, Restart.h: D2VAR(D2CLIENT, gbMiniPanelHidden, int32_t, 0x117018)
// OURS, Restart.h: D2VAR(D2CLIENT, gbMiniPanelFlagUnread, int32_t, 0x11701C)
// OURS, Restart.h: D2VAR(D2CLIENT, gbMiniPanelSinglePlayer, BOOL, 0x117020)
// OURS, Restart.h: D2VAR(D2CLIENT, gbMiniPanelButtonDown, BOOL, 0x117024)
// OURS, showitems.h: D2VAR(D2CLIENT, gShowItemLabels, D2ClientShowItemLabelStrc, 0x117028)   // [32]
// OURS, showitems.h: D2VAR(D2CLIENT, gnNumShowItems, int32_t, 0x119428)
// OURS, SkillDesc.h: D2VAR(D2CLIENT, gbSkillDescLineOffsetsReady, uint8_t, 0x11942C)
// OURS, SkillDesc.h: D2VAR(D2CLIENT, gbSkillsTxtScanDone, BOOL, 0x119430)
// OURS, SkillDesc.h: D2VAR(D2CLIENT, gbSkillDescPanelNudged, uint8_t, 0x119434)
// OURS, skillstree.h: D2VAR(D2CLIENT, gnSkillTreeCellPalette, int32_t, 0x119438)   // [128]
// OURS, skillstree.h: D2VAR(D2CLIENT, gnSkillTreeCellState, int32_t, 0x119638)   // [128]
// OURS, skillstree.h: D2VAR(D2CLIENT, gbSkillTreeButtonDown, BOOL, 0x119838)
// OURS, skillstree.h: D2VAR(D2CLIENT, gbSkillPointSendPending, BOOL, 0x11983C)
// OURS, skillstree.h: D2VAR(D2CLIENT, gbHasUnspentSkillPointsUnread, BOOL, 0x119840)
// OURS, skillstree.h: D2VAR(D2CLIENT, gbHasUnspentSkillPoints, BOOL, 0x119844)
// OURS, skillstree.h: D2VAR(D2CLIENT, gbSkillTreeDrawn, BOOL, 0x11984C)
// OURS, skillstree.h: D2VAR(D2CLIENT, gnBlackPaletteIndex, uint8_t, 0x119850)
// OURS, spellsel.h: D2VAR(D2CLIENT, gnSpellSelHotkeyMode, int32_t, 0x119858)   // [16]
// OURS, spellsel.h: D2VAR(D2CLIENT, gSpellSelPanel, D2SpellSelPanelStrc, 0x119898)
// OURS, spellsel.h: D2VAR(D2CLIENT, gnSpellSelRowY, int32_t, 0x119C60)   // [4]
// OURS, spellsel.h: D2VAR(D2CLIENT, gnSpellSelSlotsUnread, int32_t, 0x119C78)   // [16]
// OURS, spellsel.h: D2VAR(D2CLIENT, gnSpellSelInitTailUnread, int32_t, 0x119CB8)   // [4]
// OURS, spellsel.h: D2VAR(D2CLIENT, gwszBoundKeyLabel, Unicode, 0x119CC8)   // [16]
// OURS, spellsel.h: D2VAR(D2CLIENT, gnSpellSelHotkeySlot, int32_t, 0x119CE8)
// OURS, spellsel.h: D2VAR(D2CLIENT, gnSpellSelSkillIds, int32_t, 0x119CF0)   // [16]
// OURS, spellsel.h: D2VAR(D2CLIENT, gnSpellSelOwnerGUIDs, D2UnitGUID, 0x119D30)   // [16]
// OURS, spellsel.h: D2VAR(D2CLIENT, gnBoundKeyLabelGuard, uint8_t, 0x119D70)
// OURS, spellsel.h: D2VAR(D2CLIENT, ghSkillIconImage, D2CellFileStrc *, 0x119D74)
// OURS, spellsel.h: D2VAR(D2CLIENT, gnMiniSkillMode, int32_t, 0x119D78)
// OURS, text.h: D2VAR(D2CLIENT, gHoverMsgRect, RECT, 0x119D90)   // [16]
// OURS, text.h: D2VAR(D2CLIENT, gPopupTextList, D2ClientPopupStrc *, 0x119E90)
// OURS, text.h: D2VAR(D2CLIENT, sgTextState, D2TextStateStrc, 0x119E98)
// OURS, text.h: D2VAR(D2CLIENT, sgPopup, D2PopupStrc, 0x119F38)
// OURS, text.h: D2VAR(D2CLIENT, gnStaticInfoDeadline, uint32_t, 0x119F3C)
// OURS, text.h: D2VAR(D2CLIENT, gpMessageListHead, D2TextHeaderStrc *, 0x119F40)
// OURS, text.h: D2VAR(D2CLIENT, gwCairnStoneOrder, uint16_t, 0x119F44)   // [6]
// OURS, text.h: D2VAR(D2CLIENT, gbQuestScrollOpen, BOOL, 0x119F50)
// OURS, text.h: D2VAR(D2CLIENT, gnPopupX, int32_t, 0x119F54)
// OURS, text.h: using D2CLIENT_gpfnPopupCallback_ft = int32_t (__fastcall *)(uint32_t, uint32_t, uint32_t);
// OURS, text.h: D2VAR(D2CLIENT, gpfnPopupCallback, D2CLIENT_gpfnPopupCallback_ft, 0x119F58)
// OURS, text.h: D2VAR(D2CLIENT, gpPopupCallbackArg, int32_t, 0x119F5C)
// OURS, text.h: D2VAR(D2CLIENT, gnPopupSpeechSoundId, D2C_Sounds, 0x119F60)
// OURS, text.h: D2VAR(D2CLIENT, gbPopupDismissPending, BOOL, 0x119F64)
// OURS, text.h: D2VAR(D2CLIENT, gbPopupHandlersRegistered, int32_t, 0x119F68)
// OURS, text.h: D2VAR(D2CLIENT, gIniScrollImage3, D2CellFileStrc *, 0x119F70)
// OURS, text.h: D2VAR(D2CLIENT, gIniScrollImage2, D2CellFileStrc *, 0x119F74)
// OURS, text.h: D2VAR(D2CLIENT, gIniScrollImage, D2CellFileStrc *, 0x119F78)
// OURS, trade.h: D2VAR(D2CLIENT, gpTradeWaitDialog, D2ClientDialogStrc *, 0x119F7C)
// OURS, trade.h: D2VAR(D2CLIENT, gwszTradePartnerName, Unicode, 0x119F80)   // [16]
// OURS, trade.h: D2VAR(D2CLIENT, gnTradeDeadline, uint32_t, 0x119FA0)
// OURS, trade.h: D2VAR(D2CLIENT, gnTradeGoldOther, int32_t, 0x119FA4)
// OURS, trade.h: D2VAR(D2CLIENT, gbTradeCancelSent, uint8_t, 0x119FA8)
// OURS, trade.h: D2VAR(D2CLIENT, gnTradeGoldSelf, int32_t, 0x119FAC)
// OURS, trade.h: D2VAR(D2CLIENT, gTradePartnerGUID, uint32_t, 0x119FB0)
// OURS, trade.h: D2VAR(D2CLIENT, gpTradeConfirmDialog, D2ClientDialogStrc *, 0x119FB4)
// OURS, trade.h: D2VAR(D2CLIENT, gnTradeState, int32_t, 0x119FB8)
// OURS, trade.h: D2VAR(D2CLIENT, gbTradeWindowOpen, uint8_t, 0x119FBC)
// OURS, ui.h: D2VAR(D2CLIENT, gbLoadScreenExitPending, int32_t, 0x119FC0)
// OURS, ui.h: D2VAR(D2CLIENT, gpGoldDialog, D2ClientDialogStrc *, 0x119FC4)
// OURS, ui.h: D2VAR(D2CLIENT, gpGoldDialogStepButton, int32_t, 0x119FC8)
// OURS, ui.h: D2VAR(D2CLIENT, gpGoldDialogSpinner, int32_t, 0x119FCC)
// OURS, ui.h: D2VAR(D2CLIENT, gpGoldDialogOkButton, int32_t, 0x119FD0)
// OURS, ui.h: D2VAR(D2CLIENT, gpGoldDialogCancelButton, int32_t, 0x119FD4)
// OURS, ui.h: D2VAR(D2CLIENT, gbGoldDialogKeyHookSuspended, BOOL, 0x119FD8)
// OURS, ui.h: D2VAR(D2CLIENT, gwszDrawTextScratch, Unicode, 0x119FE0)   // [256]
// OURS, ui.h: D2VAR(D2CLIENT, gnUnitLabelColor, int32_t, 0x11A1E0)
// OURS, ui.h: D2VAR(D2CLIENT, gpImgUiBoxPieces, D2CellFileStrc *, 0x11A1E4)
// OURS, ui.h: D2VAR(D2CLIENT, gpImgLevelEntryTitle, D2CellFileStrc *, 0x11A1E8)
// OURS, ui.h: D2VAR(D2CLIENT, gwszTransientText, Unicode, 0x11A1F0)   // [80]
// OURS, ui.h: D2VAR(D2CLIENT, gnTransientTextX, int32_t, 0x11A290)
// OURS, ui.h: D2VAR(D2CLIENT, gnTransientTextY, int32_t, 0x11A294)
// OURS, ui.h: D2VAR(D2CLIENT, gnTransientTextColor, int32_t, 0x11A298)
// OURS, ui.h: D2VAR(D2CLIENT, gwszUnitLabelLine1, Unicode, 0x11A2A0)   // [256]
// OURS, ui.h: D2VAR(D2CLIENT, gwszUnitLabelLine2, Unicode, 0x11A4A0)   // [256]
// OURS, ui.h: D2VAR(D2CLIENT, gnUnitLabelLine2Color, int32_t, 0x11A6A0)
// OURS, ui.h: D2VAR(D2CLIENT, gUiWindowStates, int32_t, 0x11A6A8)   // [38]
// OURS, ui.h: D2VAR(D2CLIENT, gnPanelToggleDelayUntil, uint32_t, 0x11A740)
// OURS, ui.h: D2VAR(D2CLIENT, gnGoldDialogMode, int32_t, 0x11A744)
// OURS, ui.h: D2VAR(D2CLIENT, gnBorderXDifference, int32_t, 0x11A748)
// OURS, ui.h: D2VAR(D2CLIENT, gnBorderYDifference, int32_t, 0x11A74C)
// OURS, ui.h: D2VAR(D2CLIENT, gbAutomapHandlersRegistered, BOOL, 0x11A754)
// OURS, ui.h: D2VAR(D2CLIENT, gbLoadingScreenActive, int32_t, 0x11A758)
// OURS, ui.h: D2VAR(D2CLIENT, gpImgPentSpin, D2CellFileStrc *, 0x11A768)
// OURS, ui.h: D2VAR(D2CLIENT, gpImgBuySellButton, D2CellFileStrc *, 0x11A76C)
// OURS, ui.h: D2VAR(D2CLIENT, gpImgFocus16, D2CellFileStrc *, 0x11A770)
// OURS, ui.h: D2VAR(D2CLIENT, UNRESOLVED_dword_6FBBA774, int32_t, 0x11A774)
// OURS, ui.h: D2VAR(D2CLIENT, sghLoadScreenProgress, D2CellFileStrc *, 0x11A778)
// OURS, ui.h: D2VAR(D2CLIENT, gnLoadScreenAct, int32_t, 0x11A77C)
// OURS, ui.h: D2VAR(D2CLIENT, gbDeathMessageShown, BOOL, 0x11A780)
// OURS, ui.h: D2VAR(D2CLIENT, gLevelId, D2C_Levels, 0x11A784)
// OURS, ui.h: D2VAR(D2CLIENT, gLevelEntryFrameCount, int32_t, 0x11A788)
// OURS, ui.h: D2VAR(D2CLIENT, gpImgYouDiedHardcore, D2CellFileStrc *, 0x11A78C)
// OURS, ui.h: D2VAR(D2CLIENT, gpImgYouDiedInst, D2CellFileStrc *, 0x11A790)
// OURS, ui.h: D2VAR(D2CLIENT, gbChargeUpIconsEnabled, int32_t, 0x11A794)
// OURS, ui.h: D2VAR(D2CLIENT, gnChargeUpIconStepX, int32_t, 0x11A798)
// OURS, ui.h: D2VAR(D2CLIENT, gnUiToggleRuleRowsZero, int32_t, 0x11A7A0)   // [3][38]
// OURS, ui.h: D2VAR(D2CLIENT, gnGoldDialogAmount, int32_t, 0x11A968)
// OURS, ui.h: D2VAR(D2CLIENT, gnLoadScreenFrame, int32_t, 0x11A96C)
// OURS, ui.h: D2VAR(D2CLIENT, gnLoadBarFillColour, uint8_t, 0x11A970)
// OURS, ui.h: D2VAR(D2CLIENT, gnLoadBarFrameColour, uint8_t, 0x11A971)
// OURS, ui.h: D2VAR(D2CLIENT, gdwUiThrottleTick, uint32_t, 0x11A974)
// OURS, useitem.h: D2VAR(D2CLIENT, gnAnvilPanelMode, int32_t, 0x11A978)
// OURS, useitem.h: D2VAR(D2CLIENT, gnAnvilState, int32_t, 0x11A97C)
// OURS, useitem.h: D2VAR(D2CLIENT, gnAnvilNpcGUID, D2UnitGUID, 0x11A980)
// OURS, useitem.h: D2VAR(D2CLIENT, gnAnvilInteractGUID, int32_t, 0x11A984)
// OURS, useitem.h: D2VAR(D2CLIENT, gnAnvilSlotUnitId, D2UnitGUID, 0x11A988)
// OURS, useitem.h: D2VAR(D2CLIENT, gpAnvilDialog, D2ClientDialogStrc *, 0x11A98C)
// OURS, useitem.h: D2VAR(D2CLIENT, gdwAnvilLastClickTick, uint32_t, 0x11A990)
// OURS, useitem.h: D2VAR(D2CLIENT, gpImgAnvilUpgrade, D2CellFileStrc *, 0x11A99C)
// OURS, useitem.h: D2VAR(D2CLIENT, gpImgAnvilBackground, D2CellFileStrc *, 0x11A9A0)
// OURS, waypoint.h: D2VAR(D2CLIENT, gbWaypointCancelDown, uint8_t, 0x11A9A4)
// OURS, waypoint.h: D2VAR(D2CLIENT, gnWaypointHoverIndex, int32_t, 0x11A9A5)
// OURS, waypoint.h: D2VAR(D2CLIENT, gpWaypointBackgroundImage, D2CellFileStrc *, 0x11A9A9)
// OURS, waypoint.h: D2VAR(D2CLIENT, gpWaypointTabsImage, D2CellFileStrc *, 0x11A9AD)
// OURS, waypoint.h: D2VAR(D2CLIENT, gpWaypointIconsImage, D2CellFileStrc *, 0x11A9B1)
// OURS, waypoint.h: D2VAR(D2CLIENT, gnWaypointUnitGUID, int32_t, 0x11A9B5)
// OURS, waypoint.h: D2VAR(D2CLIENT, gpWaypointPanelData, void *, 0x11A9B9)
// OURS, waypoint.h: D2VAR(D2CLIENT, gbWaypointCloseSent, uint8_t, 0x11A9BD)
// OURS, waypoint.h: D2VAR(D2CLIENT, gnWaypointActTab, int32_t, 0x11A9BE)
// OURS, waypoint.h: D2VAR(D2CLIENT, gnWaypointRowCount, int32_t, 0x11A9C2)
// OURS, waypoint.h: D2VAR(D2CLIENT, gnWaypointOtherActivatedCount, int32_t, 0x11A9C6)
// OURS, waypoint.h: D2VAR(D2CLIENT, gWaypointList, D2WaypointTableStrc, 0x11A9CC)   // [9]
// OURS, waypoint.h: D2VAR(D2CLIENT, gWaypointListEnd, char, 0x11A9FD)   // [3]
// OURS, CUnit.h: D2VAR(D2CLIENT, gpUnitTableById, D2UnitStrc *, 0x11AA00)   // [6][128]
// OURS, CUnit.h: D2VAR(D2CLIENT, gpUnitTableByClientId, D2UnitStrc *, 0x11B600)   // [6][128]
// OURS, CUnit.h: D2VAR(D2CLIENT, gpPlayerUnit, D2UnitStrc *, 0x11C200)
// OURS, CUnit.h: D2VAR(D2CLIENT, gnLastPeriodOfDay, int32_t, 0x11C204)
// OURS, Item.h: D2VAR(D2CLIENT, gnWeaponClassRArm, uint16_t, 0x11C208)
// OURS, Item.h: D2VAR(D2CLIENT, gnWeaponClassLArm, uint16_t, 0x11C20A)
// OURS, Monster.h: D2VAR(D2CLIENT, gapClientPlayers, D2UnitStrc *, 0x11C20C)   // [8]
// OURS, Monster.h: D2VAR(D2CLIENT, gpKashyaUnit, D2UnitStrc *, 0x11C22C)
// OURS, MonUnique.h: D2VAR(D2CLIENT, gwszNameSeparator, Unicode, 0x11C230)   // [2]
// OURS, Player.h: D2VAR(D2CLIENT, gbLastRoomInTown, int32_t, 0x11C234)
// OURS, Player.h: D2VAR(D2CLIENT, gnLastClickRepeatFrame, uint32_t, 0x11C238)
// OURS, Player.h: D2VAR(D2CLIENT, gbClickConsumed, BOOL, 0x11C23C)
// OURS, Player.h: D2VAR(D2CLIENT, gnClickPressPhase, int32_t, 0x11C240)
// OURS, UnitMode.h: D2VAR(D2CLIENT, gnServerProbeAddr1, uint32_t, 0x11C244)
// OURS, UnitMode.h: D2VAR(D2CLIENT, gnServerProbeAddr2, uint32_t, 0x11C248)
// OURS, UnitSnd.h: D2VAR(D2CLIENT, gbLevelSoundPlayed, int32_t, 0x11C250)   // [1024]
// OURS, UnitSnd.h: D2VAR(D2CLIENT, gnLastNeutralSoundFrame, int32_t, 0x11D250)
// OURS, UnitSnd.h: D2VAR(D2CLIENT, gnLastMonsterVoiceFrame, int32_t, 0x11D254)
// OURS, UnitSnd.h: D2VAR(D2CLIENT, gnNeutralSoundReArmDelay, int32_t, 0x11D258)
// OURS, UnitSnd.h: D2VAR(D2CLIENT, gnLastNagSoundFrame, int32_t, 0x11D25C)
// OURS, UnitSnd.h: D2VAR(D2CLIENT, gnLastNagSoundId, int32_t, 0x11D260)
// OURS, UnitSnd.h: D2VAR(D2CLIENT, gnLastLevelIdForSound, int32_t, 0x11D264)
// OURS, SoundChan.h: D2VAR(D2CLIENT, gaSoundChannels, D2SoundChannelStrc, 0x11D268)   // [16]
// OURS, SoundChan.h: D2VAR(D2CLIENT, gnSoundChanReentryGuard, LONG, 0x11D468)
// OURS, SoundChan.h: D2VAR(D2CLIENT, sgfSoundChannelsActive, int32_t, 0x11D46C)
// OURS, SoundChan.h: D2VAR(D2CLIENT, gnMoveToUnitGuidOrMask, uint32_t, 0x11D470)
// OURS, SoundEnv.h: D2VAR(D2CLIENT, gnLastSoundLevelId, int32_t, 0x11D474)
// OURS, SoundEnv.h: D2VAR(D2CLIENT, ghCurrentAmbienceInstance, int32_t, 0x11D478)
// OURS, SoundEnv.h: D2VAR(D2CLIENT, ghWeatherSoundInstance, int32_t, 0x11D47C)
// OURS, SoundEnv.h: D2VAR(D2CLIENT, gnCurrentAmbienceSound, D2C_Sounds, 0x11D480)
// OURS, SoundEnv.h: D2VAR(D2CLIENT, gnWeatherSoundId, D2C_Sounds, 0x11D484)
// OURS, SoundEnv.h: D2VAR(D2CLIENT, gnCurrentEventSound, D2C_Sounds, 0x11D488)
// OURS, SoundEnv.h: D2VAR(D2CLIENT, gnEventSoundDelay, uint32_t, 0x11D48C)
// OURS, SoundEnv.h: D2VAR(D2CLIENT, gdwLastEventSoundTick, uint32_t, 0x11D490)
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gpSoundEnvironLookup, int32_t *, 0x11D494)
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gpSoundHeaders, D2SoundsTxt *, 0x11D498)
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gnSoundHeaderCount, int32_t, 0x11D49C)
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gpSoundEnvironTxt, D2SoundEnvironTxt *, 0x11D4A0)
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gnSoundEnvironTxtRecordCount, int32_t, 0x11D4A4)
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gnSoundEnvironLookupMin, int32_t, 0x11D4A8)
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gnSoundEnvironLookupMax, int32_t, 0x11D4AC)
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gSoundDataList, D2SoundListNodeStrc *, 0x11D4B0)
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gnSoundPreloadCursor, uint32_t, 0x11D4B4)
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gdwSoundLastServiceTick, uint32_t, 0x11D4B8)
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gdwSoundLastLoadTick, uint32_t, 0x11D4BC)
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gnSoundAsyncLoadsInFlight, uint32_t, 0x11D4C0)
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gnSoundCacheBudget, uint32_t, 0x11D4C4)
// OURS, SoundHdr.h: D2VAR(D2CLIENT, gnSoundCacheUsed, uint32_t, 0x11D4C8)
// OURS, SoundInst.h: D2VAR(D2CLIENT, gaSoundInstances, D2SoundInstanceStrc, 0x11D4D0)   // [200]
// OURS, SoundInst.h: D2VAR(D2CLIENT, gpSoundInstances, D2SoundListNodeStrc *, 0x121A58)
// OURS, SoundInst.h: D2VAR(D2CLIENT, gbSoundPlaybackEnabled, int32_t, 0x121A5C)
// OURS, SoundInst.h: D2VAR(D2CLIENT, gnNextSoundInstanceHandle, int32_t, 0x121A60)
// OURS, SoundInst.h: D2VAR(D2CLIENT, gnSoundVolumeRampTick, uint32_t, 0x121A64)
// OURS, SoundInst.h: D2VAR(D2CLIENT, gbSoundChaosDebug, int32_t, 0x121A68)
// OURS, SoundSong.h: D2VAR(D2CLIENT, gnLevelChangeTick, uint32_t, 0x121A6C)
// OURS, SoundSong.h: D2VAR(D2CLIENT, gdwSoundMusicTickUnused, uint32_t, 0x121A70)
// OURS, SoundSong.h: D2VAR(D2CLIENT, gnCurrentSongId, D2C_Sounds, 0x121A74)
// OURS, SoundSong.h: D2VAR(D2CLIENT, gnSongLevelId, int32_t, 0x121A78)
// OURS, SoundSong.h: D2VAR(D2CLIENT, gnSongChangeTick, uint32_t, 0x121A7C)
// OURS, SoundSong.h: D2VAR(D2CLIENT, gbStingActive, BOOL, 0x121A80)
// OURS, SoundSong.h: D2VAR(D2CLIENT, gnSting1SoundId, D2C_Sounds, 0x121A84)
// OURS, SoundSong.h: D2VAR(D2CLIENT, gnSting1Frame, uint32_t, 0x121A88)
// OURS, SoundSong.h: D2VAR(D2CLIENT, gbSting1Pending, BOOL, 0x121A8C)
// OURS, SoundSong.h: D2VAR(D2CLIENT, gnSting2SoundId, D2C_Sounds, 0x121A90)
// OURS, SoundSong.h: D2VAR(D2CLIENT, gnSting2Frame, uint32_t, 0x121A94)
// OURS, SoundSong.h: D2VAR(D2CLIENT, gbSting2Pending, BOOL, 0x121A98)
// OURS, SoundSong.h: D2VAR(D2CLIENT, gnStingEndFrame, uint32_t, 0x121A9C)
// OURS, SoundSong.h: D2VAR(D2CLIENT, gnAreaEntryQuestLineLevelId, int32_t, 0x121AA0)
// OURS, Cursor.h: D2VAR(D2CLIENT, gpCursorHeldUnit, D2UnitStrc *, 0x121AA4)
// OURS, Cursor.h: D2VAR(D2CLIENT, gpCursorImages, D2CellFileStrc *, 0x121AA8)   // [7]
// OURS, Cursor.h: D2VAR(D2CLIENT, gnCursorType, D2C_CursorTypes, 0x121AC4)
// OURS, Cursor.h: D2VAR(D2CLIENT, gnCursorFrame, int32_t, 0x121AC8)
// OURS, Cursor.h: D2VAR(D2CLIENT, gwCursorWriteOnly, uint16_t, 0x121ACC)
// OURS, Cursor.h: D2VAR(D2CLIENT, gnCursorLastPositionUpdateTickCount, uint32_t, 0x121AD0)
// OURS, Cursor.h: D2VAR(D2CLIENT, gnCursorState, D2C_CursorState, 0x121AD4)
// OURS, Cursor.h: D2VAR(D2CLIENT, gnCursorClickOffsetX, int32_t, 0x121AE0)
// OURS, Cursor.h: D2VAR(D2CLIENT, gnMouseX, int32_t, 0x121AE4)
// OURS, Cursor.h: D2VAR(D2CLIENT, gnMouseY, int32_t, 0x121AE8)
// OURS, Cursor.h: D2VAR(D2CLIENT, gnRoutedPacketBytes, int32_t, 0x121AF0)
// OURS, Cursor.h: D2VAR(D2CLIENT, gPacketAllSize, uint32_t, 0x121AF4)
// OURS, Cursor.h: D2VAR(D2CLIENT, gnRoutePacketCalls, int32_t, 0x121AF8)
// OURS, Cursor.h: D2VAR(D2CLIENT, gPacketCounter, uint32_t, 0x121AFC)
// OURS, Cursor.h: D2VAR(D2CLIENT, gnOversizePacketBatches, int32_t, 0x121B00)
// OURS, Cursor.h: D2VAR(D2CLIENT, gbCursorsLoaded, BOOL, 0x121B04)
// OURS, Gfx.h: D2VAR(D2CLIENT, gCofCachePlayer, D2ClientCofCacheStrc, 0x121B08)
// OURS, Gfx.h: D2VAR(D2CLIENT, gaPlayerPreloadTable, D2ClientModePreloadStrc, 0x122318)   // [7]
// OURS, Gfx.h: D2VAR(D2CLIENT, gCofCacheOther, D2ClientCofCacheStrc, 0x122778)
// OURS, Gfx.h: D2VAR(D2CLIENT, gpMonsterPreloadTable, D2ClientModePreloadStrc *, 0x122F84)
// OURS, Gfx.h: D2VAR(D2CLIENT, gnPaletteIndexGreen, uint8_t, 0x122F88)
// OURS, Gfx.h: D2VAR(D2CLIENT, gnPaletteIndexYellow, uint8_t, 0x122F89)
// OURS, Gfx.h: D2VAR(D2CLIENT, gnPaletteIndexRed, uint8_t, 0x122F8A)
// OURS, Gfx.h: D2VAR(D2CLIENT, gnPaletteIndexBlue, uint8_t, 0x122F8B)
// OURS, Gfx.h: D2VAR(D2CLIENT, gdwPreloadThrottleTick, uint32_t, 0x122F8C)
// OURS, Gfx.h: D2VAR(D2CLIENT, gnGfxMemoryUsed, int32_t, 0x122F90)
// OURS, Gfx.h: D2VAR(D2CLIENT, gnCofMemoryUsed, int32_t, 0x122F94)
// OURS, Gfx.h: D2VAR(D2CLIENT, gnCofMemoryBudget, int32_t, 0x122F98)
// OURS, Gfx.h: D2VAR(D2CLIENT, gbPreloadPassActive, int32_t, 0x122F9C)
// OURS, Gfx.h: D2VAR(D2CLIENT, gbPaletteIndicesInit, int32_t, 0x122FA0)
// OURS, Gfx.h: D2VAR(D2CLIENT, gbPreloadRequired, BOOL, 0x122FA4)
// OURS, GfxUtil.h: D2VAR(D2CLIENT, gaMonModeTokens, int32_t, 0x122FB0)   // [16]
// OURS, GfxUtil.h: D2VAR(D2CLIENT, gaCompositTokens, int32_t, 0x122FF0)   // [16]
// OURS, GfxUtil.h: D2VAR(D2CLIENT, gaObjModeTokens, int32_t, 0x123030)   // [8]
// OURS, GfxUtil.h: D2VAR(D2CLIENT, gaArmTypeTokens, int32_t, 0x123050)   // [3]
// OURS, GfxUtil.h: D2VAR(D2CLIENT, gaPlrModeTokens, int32_t, 0x123060)   // [20]
// OURS, Fault.h: D2VAR(D2CLIENT, gMsgSrcCriticalSection, struct _RTL_CRITICAL_SECTION, 0x123430)
// OURS, Fault.h: D2VAR(D2CLIENT, gMessageSourceList, TSExplicitList, 0x123448)
// OURS, Fault.h: D2VAR(D2CLIENT, gbMsgSourceLockReady, int32_t, 0x123454)
// OURS, Fault.h: D2VAR(D2CLIENT, gpfnFaultAssertHandler, void *, 0x123458)
// OURS, Fault.h: D2VAR(D2CLIENT, gbLngInitInDebugBreak, int32_t, 0x12345C)

///////////////////////////////////////////////////////////////////////////////
//
//  NOT EXPRESSIBLE THROUGH D2VAR -- listed, not dropped.
//
//  Each line is a real named global in the database. To reach one of these,
//  call GetDllOffset() directly and cast the result yourself.
//
///////////////////////////////////////////////////////////////////////////////

// 0xAC557    $LN9_2                                   the name is not a C identifier
// 0xD0F38    gSehScopeTable_CLIENT_FlushServerMemoryP undeclared type(s): _SCOPETABLE_ENTRY
// 0xD0F58    gSehScopeTable_UNRESOLVED_sub_6FB527A0   undefined bytes: the database gives it no type at all
// 0xD16CC    gEhUnwindMap_GAMEDATA_DestructTable      undefined bytes: the database gives it no type at all
// 0xD170C    gEhFuncInfo_GAMEDATA_DestructTable       undefined bytes: the database gives it no type at all
// 0xD177C    gEhUnwindMap_SGAMEDATA_ListDestruct      undefined bytes: the database gives it no type at all
// 0xD1794    gEhFuncInfo_SGAMEDATA_ListDestruct       undefined bytes: the database gives it no type at all
// 0xD17B0    gEhUnwindMap_SGAMEDATA_DestructHashTable undefined bytes: the database gives it no type at all
// 0xD17F0    gEhFuncInfo_SGAMEDATA_DestructHashTable  undefined bytes: the database gives it no type at all
// 0xD180C    gEhUnwindMap_SGAMEDATA_HashUnlinkNodesAn undefined bytes: the database gives it no type at all
// 0xD1814    gEhFuncInfo_SGAMEDATA_HashUnlinkNodesAnd undefined bytes: the database gives it no type at all
// 0xD1830    gEhUnwindMap_SGAMEDATA_Destructor        undefined bytes: the database gives it no type at all
// 0xD1858    gEhFuncInfo_SGAMEDATA_Destructor         undefined bytes: the database gives it no type at all
// 0xD1874    gEhUnwindMap_UIBUTTON_Construct          undefined bytes: the database gives it no type at all
// 0xD187C    gEhFuncInfo_UIBUTTON_Construct           undefined bytes: the database gives it no type at all
// 0xD1898    gEhUnwindMap_UIBUTTON_ProxyDestruct      undefined bytes: the database gives it no type at all
// 0xD18A0    gEhFuncInfo_UIBUTTON_ProxyDestruct       undefined bytes: the database gives it no type at all
// 0xD18BC    gEhUnwindMap_UIBUTTON_ProxyConstruct     undefined bytes: the database gives it no type at all
// 0xD18CC    gEhFuncInfo_UIBUTTON_ProxyConstruct      undefined bytes: the database gives it no type at all
// 0xD18E8    gEhUnwindMap_UIEDITBOX_Construct         undefined bytes: the database gives it no type at all
// 0xD18F0    gEhFuncInfo_UIEDITBOX_Construct          undefined bytes: the database gives it no type at all
// 0xD190C    gEhUnwindMap_UIEDITBOX_Destruct          undefined bytes: the database gives it no type at all
// 0xD1914    gEhFuncInfo_UIEDITBOX_Destruct           undefined bytes: the database gives it no type at all
// 0xD1930    gEhUnwindMap_UICTRLPROXY_Destruct        undefined bytes: the database gives it no type at all
// 0xD1938    gEhFuncInfo_UICTRLPROXY_Destruct         undefined bytes: the database gives it no type at all
// 0xD1954    gEhUnwindMap_UICTRLPROXY_ConstructEditbo undefined bytes: the database gives it no type at all
// 0xD1964    gEhFuncInfo_UICTRLPROXY_ConstructEditbox undefined bytes: the database gives it no type at all
// 0xD1980    gEhUnwindMap_LISTBOX_ImplConstruct       undefined bytes: the database gives it no type at all
// 0xD1988    gEhFuncInfo_LISTBOX_ImplConstruct        undefined bytes: the database gives it no type at all
// 0xD19A4    gEhUnwindMap_LISTBOX_ProxyDestruct       undefined bytes: the database gives it no type at all
// 0xD19AC    gEhFuncInfo_LISTBOX_ProxyDestruct        undefined bytes: the database gives it no type at all
// 0xD19C8    gEhUnwindMap_LISTBOX_Construct           undefined bytes: the database gives it no type at all
// 0xD19D8    gEhFuncInfo_LISTBOX_Construct            undefined bytes: the database gives it no type at all
// 0xD19F4    gEhUnwindMap_NPCMENU_DrawHireMercenaryLi undefined bytes: the database gives it no type at all
// 0xD19FC    gEhFuncInfo_NPCMENU_DrawHireMercenaryLis undefined bytes: the database gives it no type at all
// 0xD1A18    gEhUnwindMap_UISPINNER_Construct         undefined bytes: the database gives it no type at all
// 0xD1A20    gEhFuncInfo_UISPINNER_Construct          undefined bytes: the database gives it no type at all
// 0xD1A3C    gEhUnwindMap_UISPINNER_Destruct          undefined bytes: the database gives it no type at all
// 0xD1A44    gEhFuncInfo_UISPINNER_Destruct           undefined bytes: the database gives it no type at all
// 0xD1A60    gEhUnwindMap_UISPINNER_ProxyDestruct     undefined bytes: the database gives it no type at all
// 0xD1A68    gEhFuncInfo_UISPINNER_ProxyDestruct      undefined bytes: the database gives it no type at all
// 0xD1A84    gEhUnwindMap_UISPINNER_ProxyConstruct    undefined bytes: the database gives it no type at all
// 0xD1A94    gEhFuncInfo_UISPINNER_ProxyConstruct     undefined bytes: the database gives it no type at all
// 0xD1AB0    gEhUnwindMap_TEXT_OpenMessageLog         undefined bytes: the database gives it no type at all
// 0xD1AB8    gEhFuncInfo_TEXT_OpenMessageLog          undefined bytes: the database gives it no type at all
// 0xD1AD4    gEhUnwindMap_UI_OpenGoldEntryDialog      undefined bytes: the database gives it no type at all
// 0xD1AF4    gEhFuncInfo_UI_OpenGoldEntryDialog       undefined bytes: the database gives it no type at all

// end of file ---------------------------------------------------------------
#undef _D2VARS_H

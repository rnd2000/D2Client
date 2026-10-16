#pragma once

/****************************************************************************
*                                                                           *
*   D2Ptrs.h                                                                *
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
*   This file is used to declare pointers, be it function or variable       *
*   pointers, from the game's libraries.                                    *
*                                                                           *
*   It is recommended that you keep this file organized by which dll the    *
*   pointer is imported from, what type of pointer it is, etc. Ordering     *
*   them by address can also end up being very useful                       *
*                                                                           *
*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  These are the macros used by the template core to declare                                                                                                                                                                   ///
//  pointers. Do not touch unless you know what you're doing                                                                                                                                                                    ///
//                                                                                                                                                                                                                              ///
#ifdef _MSC_VER // MS Compiler                                                                                                                                                                                                  ///
#define D2FUNC(DLL, NAME, RETURN, CONV, ARGS, OFFSET) typedef RETURN (CONV* DLL##_##NAME##_t) ARGS; inline DLL##_##NAME##_t DLL##_##NAME = (DLL##_##NAME##_t)GetDllOffset(#DLL, DLLBASE_##DLL, OFFSET);   ///
#define D2VAR(DLL, NAME, TYPE, OFFSET) typedef TYPE DLL##_##NAME##_vt; inline DLL##_##NAME##_vt * DLL##_##NAME = (DLL##_##NAME##_vt *)GetDllOffset(#DLL, DLLBASE_##DLL, OFFSET);                          ///
#define D2PTR(DLL, NAME, OFFSET) inline DWORD NAME = GetDllOffset(#DLL, DLLBASE_##DLL, OFFSET);                                                                                                           ///
#else // GCC Compiler                                                                                                                                                                                                           ///
#define D2FUNC(DLL, NAME, RETURN, CONV, ARGS, OFFSET) typedef RETURN (CONV* DLL##_##NAME##_t) ARGS; DLL##_##NAME##_t DLL##_##NAME __attribute__((weak)) = (DLL##_##NAME##_t)GetDllOffset(#DLL, DLLBASE_##DLL, OFFSET);          ///
#define D2VAR(DLL, NAME, TYPE, OFFSET) typedef TYPE DLL##_##NAME##_vt; DLL##_##NAME##_vt * DLL##_##NAME __attribute__((weak)) = (DLL##_##NAME##_vt *)GetDllOffset(#DLL, DLLBASE_##DLL, OFFSET);                                 ///
#define D2PTR(DLL, NAME, OFFSET) DWORD NAME __attribute__((weak)) = GetDllOffset(#DLL, DLLBASE_##DLL, OFFSET);                                                                                                              ///
#endif 

extern DWORD GetDllOffset(const char* ModuleName, DWORD BaseAddress, int Offset); 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
//  GENERATED -- do not edit by hand; regenerate instead.
//
//    python tools/d2client/sweep/gen_ptrs_header.py \
//           GP_DB=<repo>/database/D2Client.json GP_APPLY=1
//
//  Source: database/D2Client.json, the export of the D2Client IDA database of
//  the d2client-sweep repository -- Diablo II 1.10 D2Client.dll, image base
//  0x6FAA0000. Names, prototypes and calling conventions are that database's,
//  after the function-by-function sweep.
//
//  THE SIGN OF THE LAST ARGUMENT IS NOT COSMETIC. GetDllOffset() in
//  DLLmain.cpp reads a NEGATIVE value as an export ORDINAL and resolves it
//  through GetProcAddress; a positive one is an RVA added to the module base.
//  This module exports 4 functions by ordinal and they are emitted that way,
//  because an ordinal survives a rebuild that moves the code and an RVA does
//  not. The other 3625 have no ordinal and carry their RVA.
//
//  Ordered by address, as the file's own header recommends.
//
//  3 function(s) could not go through the macro and are written below as
//  comments carrying the reason, so nothing is lost from the record.
//
//  AND 916 OF THE D2FUNC LINES ARE THEMSELVES COMMENTED OUT. Those names are
//  not D2Client functions: they are IMPORT THUNKS, and D2Imports.h declares
//  what each one jumps to, so the call now leaves for the module that owns
//  it instead of going through a pointer to a jump to an IAT slot. The line
//  is kept, commented, with the direct spelling on the end -- the offset it
//  carries is the measurement that identified the thunk in the first place.
//  The remaining live thunk rows are the ones D2Imports.h still cannot
//  declare: bound by name, or a prototype that will not parse.
//
//  12 more are WINDOWS imports and are commented for a different reason:
//  windows.h declares them and this rewrite calls Windows directly. Only
//  twelve of the 109 Windows imports ever had a thunk to point at -- the
//  other 97 go straight through the IAT and never had a row here.
//
//  309 functions have NO line here at all: the VC6 runtime the original was
//  linked with, named CRT_ in the database. This rewrite does not write it and
//  does not call it -- memory, files and formatting are our compiler's own
//  runtime (RULES.md, "THE VC6 RUNTIME IS NOT REWRITTEN"; the user,
//  2026-10-07).
//
//  AND 2701 ARE FUNCTIONS OF OURS, written `//D2FUNC(` -- the user's form of
//  2026-10-12. D2Client/*.cpp defines each at the row's address, under its
//  `//1.10f:` marker, and our code calls it by its bare name, so the slot to
//  the original's copy is not declared. The line stays, commented in place,
//  as D2Vars.h keeps a global of ours: the file still shows which are ours.
//  A blank line separates each run of them from each run of the thunks.
//
///////////////////////////////////////////////////////////////////////////////

//D2FUNC(D2CLIENT, ARCHIVE_CellFileLoad, D2CellFileStrc *, __fastcall, (char *pszName, int32_t nGfxType), 0x1000)
//D2FUNC(D2CLIENT, ARCHIVE_CellFileLoadWithSize, D2CellFileStrc *, __fastcall, (char *pszName, uint32_t *pdwFileSize, int32_t nGfxType), 0x10A0)
//D2FUNC(D2CLIENT, ARCHIVE_CellFileFree, void, __fastcall, (D2CellFileStrc *pCellFile), 0x1140)
//D2FUNC(D2CLIENT, QueryInterface, D2ClientInterface *, __cdecl, (), -10004)
//D2FUNC(D2CLIENT, DLLMAIN_Entrypoint, int32_t, __fastcall, (D2ConfigStrc *pConfig), 0x11D0)
//D2FUNC(D2CLIENT, LNG_RegisterCmdLineArguments, int32_t, __cdecl, (), 0x11E0)
//D2FUNC(D2CLIENT, LNG_Initialize, int32_t, __cdecl, (), 0x1210)
//D2FUNC(D2CLIENT, LNG_Shutdown, void, __cdecl, (), 0x1300)
//D2FUNC(D2CLIENT, LNG_MessageSource, int32_t, __stdcall, (DWORD dwMessageId, LPSTR pszBuffer, int32_t nMaxLength), 0x1330)
//D2FUNC(D2CLIENT, MEMPART_InitMemoryPartitions, void, __fastcall, (int32_t hPool, int32_t bInitTileCache), 0x13A0)
//D2FUNC(D2CLIENT, MEMPART_DestroyCaches, void, __fastcall, (BOOL bFreeLRU), 0x14E0)
//D2FUNC(D2CLIENT, MEMPART_Nullsub1, void, __cdecl, (), 0x1500)
//D2FUNC(D2CLIENT, VIDEO_RecordCinematicProgress, void, __fastcall, (int32_t nCinematicIndex), 0x1510)
//D2FUNC(D2CLIENT, VIDEO_PlayCinematic, void, __fastcall, (int32_t nCinematicId), 0x1680)
//D2FUNC(D2CLIENT, VIDEO_SetGameCompletedClassic, void, __cdecl, (), 0x1CA0)
//D2FUNC(D2CLIENT, VIDEO_ResetCinematicState, void, __cdecl, (), 0x1CB0)
//D2FUNC(D2CLIENT, VIDEO_SetActVIntroPending, void, __cdecl, (), 0x1CE0)
//D2FUNC(D2CLIENT, VIDEO_IsCinematicPlaying, uint8_t, __cdecl, (), 0x1CF0)
//D2FUNC(D2CLIENT, WINMAIN_AppModeEnqueue, void, __fastcall, (D2C_AppMode nAppMode), 0x1D00)
//D2FUNC(D2CLIENT, WINMAIN_AppModeDequeue, D2C_AppMode, __cdecl, (), 0x1D40)
//D2FUNC(D2CLIENT, WINMAIN_CmdArgDisplayHelp, int32_t, __stdcall, (void *pParam, int32_t nUnused), 0x1DA0)
//D2FUNC(D2CLIENT, WINMAIN_CmdArgPreCache, int32_t, __stdcall, (void *pParam, int32_t nUnused), 0x1E10)
//D2FUNC(D2CLIENT, WINMAIN_CmdArgRecord, int32_t, __stdcall, (void *pParam, int32_t nUnused), 0x1E60)
//D2FUNC(D2CLIENT, WINMAIN_CmdArgPlay, int32_t, __stdcall, (void *pParam, int32_t nUnused), 0x1E70)
//D2FUNC(D2CLIENT, WINMAIN_CmdArgSet, int32_t, __stdcall, (void *pParam, int32_t nUnused), 0x1E80)
//D2FUNC(D2CLIENT, WINMAIN_CmdArgMode, int32_t, __stdcall, (void *pParam, int32_t nUnused), 0x1EA0)
//D2FUNC(D2CLIENT, WINMAIN_DeactivateApp, void, __cdecl, (), -10003)
//D2FUNC(D2CLIENT, WINMAIN_GetConfig, D2ConfigStrc *, __cdecl, (), 0x1F70)
//D2FUNC(D2CLIENT, WINMAIN_GetBnClientInterface, struct BnClientInterface *, __cdecl, (), 0x1F80)
//D2FUNC(D2CLIENT, WINMAIN_GetConfigGameName, char *, __cdecl, (), 0x1F90)
//D2FUNC(D2CLIENT, WINMAIN_GetConfigGamePassword, char *, __cdecl, (), 0x1FC0)
//D2FUNC(D2CLIENT, WINMAIN_GetConfigServerIP, char *, __cdecl, (), 0x1FF0)
//D2FUNC(D2CLIENT, WINMAIN_IsHardcoreGame, int32_t, __cdecl, (), 0x2020)
//D2FUNC(D2CLIENT, WINMAIN_MarkCharacterDead, void, __cdecl, (), 0x2030)
//D2FUNC(D2CLIENT, WINMAIN_SetAppModeContinue, void, __fastcall, (BOOL bContinue), 0x2040)
//D2FUNC(D2CLIENT, WINMAIN_D2ClientEntrypoint, int32_t, __stdcall, (HMODULE hModule, D2ConfigStrc *pConfig), 0x2050)
//D2FUNC(D2CLIENT, WINMAIN_SCmdDisplayHelpOnExtraArg, int32_t, __stdcall, (int32_t nCmdArg), 0x22D0)
//D2FUNC(D2CLIENT, WINMAIN_SCmdDisplayHelp, int32_t, __stdcall, (int32_t nCmdArg), 0x2340)
//D2FUNC(D2CLIENT, WNDPROC_SetResolutionMode, void, __fastcall, (D2GameResolutionMode nMode), 0x23B0)
//D2FUNC(D2CLIENT, WNDPROC_OnDestroy, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x2460)
//D2FUNC(D2CLIENT, WNDPROC_OnActivateApp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x2480)
//D2FUNC(D2CLIENT, WNDPROC_OnDefault, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x24A0)
//D2FUNC(D2CLIENT, WNDPROC_RedirectToClose, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x24D0)
//D2FUNC(D2CLIENT, WNDPROC_OnIgnoredMessage, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x2500)
//D2FUNC(D2CLIENT, WNDPROC_RedirectToUser, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x2520)
//D2FUNC(D2CLIENT, WNDPROC_WndProc, LRESULT, __stdcall, (HWND, UINT, WPARAM, LPARAM), 0x2550)
//D2FUNC(D2CLIENT, WNDPROC_WindowRunMessagePump, int32_t, __fastcall, (int32_t (__stdcall *pfnIdle)(int32_t)), 0x25D0)
//D2FUNC(D2CLIENT, WNDPROC_RegisterMessageHandlers, void, __fastcall, (HWND hWnd, const D2WinCommandRegisterStrc *pCallbacks, size_t nCallbacks), 0x26C0)
//D2FUNC(D2CLIENT, WNDPROC_UnregisterMessageHandlers, void, __fastcall, (HWND hWnd, const D2WinCommandRegisterStrc *pCallbacks, size_t nCallbacks), 0x2780)
//D2FUNC(D2CLIENT, WNDPROC_PushStormMessageState, void, __cdecl, (), 0x2840)
//D2FUNC(D2CLIENT, WNDPROC_PopStormMessageState, void, __cdecl, (), 0x2850)
//D2FUNC(D2CLIENT, DRAW_SetGamma, void, __fastcall, (uint32_t nGamma), 0x2860)
//D2FUNC(D2CLIENT, DRAW_SelectGdiDisplay, void, __cdecl, (), 0x2870)
//D2FUNC(D2CLIENT, DRAW_SetDisplayTypeGlide, void, __cdecl, (), 0x2880)
//D2FUNC(D2CLIENT, DRAW_SetDisplayTypeOpenGL, void, __cdecl, (), 0x2890)
//D2FUNC(D2CLIENT, DRAW_SetDisplayTypeDirect3D, void, __cdecl, (), 0x28A0)
//D2FUNC(D2CLIENT, DRAW_SetDisplayTypeRave, void, __cdecl, (), 0x28B0)
//D2FUNC(D2CLIENT, DRAW_GetDisplayType, int32_t, __cdecl, (), 0x28C0)
//D2FUNC(D2CLIENT, DRAW_IsDisplayType3D, BOOL, __cdecl, (), 0x28D0)
//D2FUNC(D2CLIENT, DRAW_GameDrawDebugOverlay, void, __cdecl, (), 0x28E0)
//D2FUNC(D2CLIENT, DRAW_GameToggleDebugOverlay, void, __cdecl, (), 0x2D30)
//D2FUNC(D2CLIENT, DRAW_ToggleDebugOverlay, void, __cdecl, (), 0x2DA0)
//D2FUNC(D2CLIENT, DFLOOR_DrawFloors, void, __fastcall, (D2ClientViewStrc *pView), 0x2E60)
//D2FUNC(D2CLIENT, DFLOOR_DrawFloorTile, void, __thiscall, (D2FloorDrawCtxStrc *pCtx, D2DrlgTileDataStrc *pTile, BOOL bFloorFilter, int32_t nXPos, int32_t nYPos, int32_t nWorldX, int32_t nWorldY), 0x2E70)
//D2FUNC(D2CLIENT, DFLOOR_DrawEdgeTile, void, __thiscall, (D2FloorDrawCtxStrc *pCtx, int32_t nWorldX, int32_t nWorldY), 0x3290)
//D2FUNC(D2CLIENT, DFLOOR_DrawRoomFloors, void, __thiscall, (D2FloorDrawCtxStrc *pCtx, D2ClientViewStrc *pView), 0x3330)
//D2FUNC(D2CLIENT, DFLOOR_DrawViewGridFloors, void, __fastcall, (D2ClientViewStrc *pView), 0x3B90)
//D2FUNC(D2CLIENT, DFLOOR_FillLightGrid8x8, void, __fastcall, (D2ClientFloorVertexStrc *pVertices, int32_t nSubtileX, int32_t nSubtileY, int32_t bFlatAmbient), 0x3E70)
//D2FUNC(D2CLIENT, DFLOOR_FillEdgeLightGrid6x6, void, __fastcall, (D2ClientFloorVertexStrc *pVertices, int32_t nScreenX, int32_t nScreenY, int32_t nSubtileX, int32_t nSubtileY, int32_t bFlatAmbient, int32_t nSideShift, int32_t bCornersOnly), 0x3F40)
//D2FUNC(D2CLIENT, DFLOOR_BuildTileLightGrid, void, __fastcall, (D2ClientFloorVertexStrc *pVertices, int32_t nTileX, int32_t nTileY, int32_t nBais, int32_t nHalfShift, int32_t bCornersOnly), 0x4060)
//D2FUNC(D2CLIENT, DFLOOR_APPROX_Distance, uint32_t, __fastcall, (int32_t nDX, int32_t nDY), 0x4210)
//D2FUNC(D2CLIENT, DLIGHTMAP_AddLight, D2ClientLightStrc *, __fastcall, (D2UnitStrc *pUnit, int32_t nType, int32_t nRadiusSubtiles, int32_t nIntensity, int32_t nRed, int32_t nGreen, int32_t nBlue), 0x4280)
//D2FUNC(D2CLIENT, DLIGHTMAP_SetTargetRadius, void, __fastcall, (D2ClientLightStrc *ptLight, int32_t nRadiusSubtiles), 0x43A0)
//D2FUNC(D2CLIENT, DLIGHTMAP_SetRadius, void, __fastcall, (D2ClientLightStrc *ptLight, int32_t nRadiusSubtiles), 0x43E0)
//D2FUNC(D2CLIENT, DLIGHTMAP_GetRadius, int32_t, __fastcall, (D2ClientLightStrc *ptLight), 0x4460)
//D2FUNC(D2CLIENT, DLIGHTMAP_SetColor, void, __fastcall, (D2ClientLightStrc *ptLight, uint8_t nRed, uint8_t nGreen, uint8_t nBlue), 0x4490)
//D2FUNC(D2CLIENT, DLIGHTMAP_RemoveLight, void, __fastcall, (D2ClientLightStrc *pLight), 0x44D0)
//D2FUNC(D2CLIENT, DLIGHTMAP_ReleaseLight, void, __fastcall, (D2ClientLightStrc *ptLight), 0x4560)
//D2FUNC(D2CLIENT, DLIGHTMAP_GetRoomLightColor, uint8_t *, __fastcall, (D2ActiveRoomStrc *pRoom, int32_t *pnIntensity, uint8_t *pnRed, uint8_t *pnGreen, uint8_t *pnBlue, int32_t bUseRoomOverride), 0x45C0)
//D2FUNC(D2CLIENT, DLIGHTMAP_ClearAmbientLight, void, __cdecl, (), 0x46A0)
//D2FUNC(D2CLIENT, DLIGHTMAP_RebuildAmbientLight, void, __fastcall, (int32_t bClear), 0x46B0)
//D2FUNC(D2CLIENT, DLIGHTMAP_Init, int32_t, __cdecl, (), 0x4A30)
//D2FUNC(D2CLIENT, DLIGHTMAP_UpdateQualityLevel, int32_t, __cdecl, (), 0x4A70)
//D2FUNC(D2CLIENT, DLIGHTMAP_UpdateForPlayer, void, __cdecl, (), 0x4AF0)
//D2FUNC(D2CLIENT, DLIGHTMAP_Update, void, __fastcall, (D2ClientLightStrc *pLight, int32_t nQualityLevel), 0x4D00)
//D2FUNC(D2CLIENT, DLIGHTMAP_AddLightToGrid, void, __fastcall, (int32_t nX, int32_t nY, int32_t nIntensity, uint8_t nRed, uint8_t nGreen, uint8_t nBlue), 0x51D0)
//D2FUNC(D2CLIENT, DLIGHTMAP_ApplyLightSource, void, __fastcall, (D2ClientLightStrc *pLight, int32_t nQualityLevel), 0x5300)
//D2FUNC(D2CLIENT, DLIGHTMAP_SeedStaticGridFromAmbient, void, __fastcall, (D2ClientLightStrc *pLight), 0x54D0)
//D2FUNC(D2CLIENT, DLIGHTMAP_SetupPropagation, int32_t, __fastcall, (int32_t nX, int32_t nY, D2ClientLightStrc *pLight), 0x55D0)
//D2FUNC(D2CLIENT, DLIGHTMAP_PropagateCell, void, __fastcall, (int32_t nX, int32_t nY), 0x5680)
//D2FUNC(D2CLIENT, DLIGHTMAP_BuildStaticMap, void, __fastcall, (D2ClientLightStrc *pLight), 0x57E0)
//D2FUNC(D2CLIENT, DLIGHTMAP_ApplyLight, void, __fastcall, (D2ClientLightStrc *pLight), 0x5BC0)
//D2FUNC(D2CLIENT, DLIGHTMAP_FreeAll, void, __cdecl, (), 0x5DC0)
//D2FUNC(D2CLIENT, DLIGHTMAP_ActCallback, void, __fastcall, (D2ActiveRoomStrc *pRoom), 0x5E10)
//D2FUNC(D2CLIENT, DLIGHTMAP_GetLightAtPoint, void, __fastcall, (int32_t nX, int32_t nY, uint8_t *pnIntensity, uint8_t *pnRed, uint8_t *pnGreen, uint8_t *pnBlue), 0x60B0)
//D2FUNC(D2CLIENT, DLIGHTMAP_DebugDrawLightGrid, void, __cdecl, (), 0x6130)
//D2FUNC(D2CLIENT, CLIENT_FindRoomContainingSubtile, D2ActiveRoomStrc *, __fastcall, (D2ActiveRoomStrc *pRoom, int32_t nSubtileX, int32_t nSubtileY), 0x6150)
//D2FUNC(D2CLIENT, DLIGHTMAP_StartWarpFlash, void, __cdecl, (), 0x61F0)
//D2FUNC(D2CLIENT, DLIGHTMAP_DrawWarpFlash, void, __cdecl, (), 0x6210)
//D2FUNC(D2CLIENT, DLIGHTMAP_CancelWarpFlash, void, __cdecl, (), 0x62A0)
//D2FUNC(D2CLIENT, DLIGHTMAP_EnvUpdateAmbientLightCache, void, __cdecl, (), 0x62B0)
//D2FUNC(D2CLIENT, DWALL_EnvDrawCellUnits, void, __fastcall, (D2ClientViewStrc *pView), 0x6340)
//D2FUNC(D2CLIENT, DWALL_DrawRoomWalls, void, __fastcall, (D2ClientWallSquareStrc *pRoom, uint32_t fOnlyDrawShadowLayer, D2ClientViewStrc *pView), 0x63E0)
//D2FUNC(D2CLIENT, DWALL_EnvDrawRoomTilesLit, void, __fastcall, (D2ClientViewStrc *pView), 0x6670)
//D2FUNC(D2CLIENT, DWALL_EnvDrawCellShadowsAndUnits, void, __fastcall, (D2ClientViewStrc *pView), 0x6860)
//D2FUNC(D2CLIENT, ARENA_IsArenaMode, BOOL, __cdecl, (), 0x6990)
//D2FUNC(D2CLIENT, ARENA_GetArenaFlags, int32_t, __cdecl, (), 0x69A0)
//D2FUNC(D2CLIENT, ARENA_SetArenaFlags, void, __fastcall, (uint32_t dwArenaFlags), 0x69B0)
//D2FUNC(D2CLIENT, ARENA_SetArenaFlag0, void, __fastcall, (int32_t bClear), 0x69C0)
//D2FUNC(D2CLIENT, ARENA_SetArenaPlayerClass, void, __fastcall, (int32_t nPlayerClass), 0x69E0)
//D2FUNC(D2CLIENT, ARENA_GetArenaPlayerClass, int32_t, __cdecl, (), 0x69F0)
//D2FUNC(D2CLIENT, ARENA_IsArenaPlayerClassForced, BOOL, __cdecl, (), 0x6A00)
//D2FUNC(D2CLIENT, ENV_Init, void, __cdecl, (), 0x6A10)
//D2FUNC(D2CLIENT, ENV_Shutdown, void, __cdecl, (), 0x6AB0)
//D2FUNC(D2CLIENT, ENV_SetRainState, void, __fastcall, (uint32_t eState), 0x6AE0)
//D2FUNC(D2CLIENT, ENV_ResetWeather, int32_t, __fastcall, (int32_t bForce), 0x6D40)
//D2FUNC(D2CLIENT, ENV_LoadImages, void, __fastcall, (D2C_Levels nLevelId), 0x7050)
//D2FUNC(D2CLIENT, ENV_UnloadImages, void, __cdecl, (), 0x7440)
//D2FUNC(D2CLIENT, ENV_OnLevelChange, int32_t, __fastcall, (void *pParam), 0x7490)
//D2FUNC(D2CLIENT, ENV_TriggerLightningFlash, int32_t, __fastcall, (int32_t bPlayThunder), 0x74C0)
//D2FUNC(D2CLIENT, ENV_UpdateBubbles, void, __cdecl, (), 0x74F0)
//D2FUNC(D2CLIENT, ENV_SpawnSplash, void, __fastcall, (int32_t nX, int32_t nY, int32_t nPerspX, int32_t nPerspY), 0x7590)
//D2FUNC(D2CLIENT, ENV_SpawnBubble, void, __fastcall, (int32_t nX, int32_t nY, int32_t nPerspX, int32_t nPerspY), 0x7760)
//D2FUNC(D2CLIENT, ENV_IsPlayerLevelMuddy, int32_t, __cdecl, (), 0x7900)
//D2FUNC(D2CLIENT, ENV_UpdateWeather, void, __cdecl, (), 0x7940)
//D2FUNC(D2CLIENT, ENV_SpawnParticle, void, __fastcall, (D2SeedStrc *pSeed), 0x7D70)
//D2FUNC(D2CLIENT, ENV_UpdateWeatherIntensity, void, __fastcall, (D2SeedStrc *pSeed), 0x80C0)
//D2FUNC(D2CLIENT, ENV_UpdateLightningAndThunder, void, __cdecl, (), 0x8240)
//D2FUNC(D2CLIENT, ENV_DrawParticleArray, void, __fastcall, (D2DataArrayStrc *pArray, D2CellFileStrc **pImageArray), 0x8690)
//D2FUNC(D2CLIENT, ENV_DrawSplashesAndBubbles, void, __cdecl, (), 0x8830)
//D2FUNC(D2CLIENT, ENV_ShouldDrawRain, BOOL, __cdecl, (), 0x8870)
//D2FUNC(D2CLIENT, ENV_GetWeatherParticleIntensity, double, __cdecl, (), 0x88B0)
//D2FUNC(D2CLIENT, ENV_SetPlayerRoomLosDraw, void, __fastcall, (int32_t bLosDraw), 0x88C0)
//D2FUNC(D2CLIENT, SEED_RollRandomNumber, uint64_t, __fastcall, (D2SeedStrc *pSeed), 0x88D0)
//D2FUNC(D2CLIENT, SEED_RollLimitedRandomNumber, int32_t, __fastcall, (D2SeedStrc *pSeed, int32_t nMax), 0x88F0)
//D2FUNC(D2CLIENT, GAME_GameDataDestructTable, void, __thiscall, (struct SGameDataTableStrc *pTable), 0x8970)
//D2FUNC(D2CLIENT, GAME_SetRunKeyHeld, void, __fastcall, (int32_t bHeld), 0x8B10)
//D2FUNC(D2CLIENT, GAME_SetRunToggled, void, __fastcall, (int32_t bRun), 0x8B20)
//D2FUNC(D2CLIENT, GAME_GetRunToggled, int32_t, __cdecl, (), 0x8B30)
//D2FUNC(D2CLIENT, GAME_SetStandStillKeyHeld, void, __fastcall, (int32_t bHeld), 0x8B40)
//D2FUNC(D2CLIENT, GAME_GetMovementFlags, uint8_t, __cdecl, (), 0x8B50)
//D2FUNC(D2CLIENT, GAME_SuppressHeldClick, void, __cdecl, (), 0x8B80)
//D2FUNC(D2CLIENT, GAME_GetLeftClickModifierFlags, int32_t, __cdecl, (), 0x8B90)
//D2FUNC(D2CLIENT, GAME_GetRightClickModifierFlags, int32_t, __cdecl, (), 0x8BA0)
//D2FUNC(D2CLIENT, GAME_GetLeftButtonDown, uint32_t, __cdecl, (), 0x8BB0)
//D2FUNC(D2CLIENT, GAME_WindowOnClose, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x8BC0)
//D2FUNC(D2CLIENT, GAME_WindowOnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x8C10)
//D2FUNC(D2CLIENT, GAME_GetLeftClickIssuedThisFrame, BOOL, __cdecl, (), 0x8CB0)
//D2FUNC(D2CLIENT, GAME_WindowOnLButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x8CC0)
//D2FUNC(D2CLIENT, GAME_WindowOnRButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x8D50)
//D2FUNC(D2CLIENT, GAME_WindowOnRButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x8E90)
//D2FUNC(D2CLIENT, GAME_WindowOnMouseWheel, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x8F20)
//D2FUNC(D2CLIENT, GAME_WindowOnMButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x8F90)
//D2FUNC(D2CLIENT, GAME_WindowOnMButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x8FC0)
//D2FUNC(D2CLIENT, GAME_WindowOnXButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x8FF0)
//D2FUNC(D2CLIENT, GAME_WindowOnXButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x9040)
//D2FUNC(D2CLIENT, GAME_WindowOnSysCommand, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x9090)
//D2FUNC(D2CLIENT, GAME_WindowConsumeMessage, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x90E0)
//D2FUNC(D2CLIENT, GAME_WindowOnActivateApp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x9100)
//D2FUNC(D2CLIENT, GAME_CmdTblActionClearScreen, int32_t, __stdcall, (), 0x9230)
//D2FUNC(D2CLIENT, GAME_EnvReapplyActEnvironment, void, __cdecl, (), 0x9260)
//D2FUNC(D2CLIENT, GAME_PreloadActData, void, __fastcall, (uint8_t nAct, uint32_t dwGameSeed, int32_t nObjectSeed, int32_t nTownLevelId), 0x9290)
//D2FUNC(D2CLIENT, GAME_TeardownGraphics, int32_t, __stdcall, (int32_t a1, int32_t a2, int32_t a3), 0x9420)
//D2FUNC(D2CLIENT, GAME_SetLeaveGameSent, void, __fastcall, (int32_t bSent), 0x9430)
//D2FUNC(D2CLIENT, GAME_GetLeaveGameSent, int32_t, __cdecl, (), 0x9440)
//D2FUNC(D2CLIENT, GAME_InitGame, void, __cdecl, (), 0x9450)
//D2FUNC(D2CLIENT, j_ENV_UnloadImages, void, __cdecl, (), 0x9610)
//D2FUNC(D2CLIENT, GAME_SetActiveMessageCallback, void, __fastcall, (int32_t nIndex), 0x9620)
//D2FUNC(D2CLIENT, GAME_DrawGameScene, void, __fastcall, (int32_t bNoDefaultCursor), 0x9640)
//D2FUNC(D2CLIENT, GAME_BeginDisconnect, void, __fastcall, (D2C_JoinErrors nMessageId), 0x9850)
//D2FUNC(D2CLIENT, GAME_DrawDisconnectMessage, int32_t, __stdcall, (int32_t nUnused), 0x98C0)
//D2FUNC(D2CLIENT, GAME_SendPingIfDue, void, __cdecl, (), 0x9A10)
//D2FUNC(D2CLIENT, GAME_OnPongUpdateLatency, void, __fastcall, (const uint8_t *pPacket), 0x9A50)
//D2FUNC(D2CLIENT, GAME_SetSkillBlockUntilFrame, void, __fastcall, (int32_t nFrame), 0x9AB0)
//D2FUNC(D2CLIENT, GAME_GetSkillBlockUntilFrame, int32_t, __cdecl, (), 0x9AC0)
//D2FUNC(D2CLIENT, GAME_GetNetworkLatency, int32_t, __cdecl, (), 0x9AD0)
//D2FUNC(D2CLIENT, GAME_GetNetworkLatencyRunningMean, int32_t, __cdecl, (), 0x9AE0)
//D2FUNC(D2CLIENT, GAME_UpdateFrame, int32_t, __stdcall, (int32_t nIdleIteration), 0x9AF0)
//D2FUNC(D2CLIENT, GAME_ServerThreadProc, DWORD, __stdcall, (LPVOID lpThreadParameter), 0xA3B0)
//D2FUNC(D2CLIENT, GAME_AppModeStartup, D2C_AppMode, __fastcall, (), 0xA4E0)
//D2FUNC(D2CLIENT, GAME_BetaSlidesOnButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0xA500)
//D2FUNC(D2CLIENT, GAME_BetaSlidesOnKeyDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0xA530)
//D2FUNC(D2CLIENT, GAME_BetaSlidesTick, int32_t, __stdcall, (int32_t nUnused), 0xA560)
//D2FUNC(D2CLIENT, GAME_VanquishedOnButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0xA7D0)
//D2FUNC(D2CLIENT, GAME_VanquishedOnButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0xA880)
//D2FUNC(D2CLIENT, GAME_VanquishedOnKeyDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0xA920)
//D2FUNC(D2CLIENT, GAME_ShowDiabloVanquishedMessage, int32_t, __stdcall, (int32_t nIdlePass), 0xA980)
//D2FUNC(D2CLIENT, GAME_MarkClassicCompletion, void, __cdecl, (), 0xB1F0)
//D2FUNC(D2CLIENT, GAME_SetGameTeardownActive, void, __cdecl, (), 0xB230)
//D2FUNC(D2CLIENT, GAME_SetDisconnecting, void, __cdecl, (), 0xB240)
//D2FUNC(D2CLIENT, GAME_NeedsWin95WheelWorkaround, BOOL, __cdecl, (), 0xB250)
//D2FUNC(D2CLIENT, GAME_HasNativeMouseWheel, int32_t, __cdecl, (), 0xB290)
//D2FUNC(D2CLIENT, GAME_IsSupportedWindows, BOOL, __cdecl, (), 0xB2E0)
//D2FUNC(D2CLIENT, GAME_IsWin2000OrXp, BOOL, __cdecl, (), 0xB320)
//D2FUNC(D2CLIENT, GAME_SetD2xOutCinematicPending, void, __fastcall, (BOOL bPending), 0xB350)
//D2FUNC(D2CLIENT, GAME_GetComInterface, struct BnClientInterface *, __cdecl, (), 0xB360)
//D2FUNC(D2CLIENT, GAME_AppModeGame, D2C_AppMode, __fastcall, (), 0xB370)
//D2FUNC(D2CLIENT, GAME_AppModeCloseWindow, D2C_AppMode, __fastcall, (), 0xBB40)
//D2FUNC(D2CLIENT, GAME_ApplyParamDisableSaving, void, __fastcall, (D2ConfigStrc *pConfig), 0xBB70)
//D2FUNC(D2CLIENT, GAME_ApplyParamStartAct, void, __fastcall, (D2ConfigStrc *pConfig), 0xBB90)
//D2FUNC(D2CLIENT, GAME_ApplyParamJoinInfo, void, __fastcall, (D2ConfigStrc *pConfig), 0xBBF0)
//D2FUNC(D2CLIENT, GAME_ApplyParamBattleNetIp, void, __fastcall, (D2ConfigStrc *pConfig), 0xBC50)
//D2FUNC(D2CLIENT, GAME_ApplyParamRealmIp, void, __fastcall, (D2ConfigStrc *pConfig), 0xBC80)
//D2FUNC(D2CLIENT, GAME_ApplyParamPlayerClass, void, __fastcall, (D2ConfigStrc *pConfig), 0xBCB0)
//D2FUNC(D2CLIENT, GAME_ApplyParamGDI, void, __fastcall, (D2ConfigStrc *pConfig), 0xBD30)
//D2FUNC(D2CLIENT, GAME_ApplyParamGlide, void, __fastcall, (D2ConfigStrc *pConfig), 0xBD40)
//D2FUNC(D2CLIENT, GAME_ApplyParamOpenGL, void, __fastcall, (D2ConfigStrc *pConfig), 0xBD50)
//D2FUNC(D2CLIENT, GAME_ApplyParamDirect3D, void, __fastcall, (D2ConfigStrc *pConfig), 0xBD60)
//D2FUNC(D2CLIENT, GAME_ApplyParamRave, void, __fastcall, (D2ConfigStrc *pConfig), 0xBD70)
//D2FUNC(D2CLIENT, GAME_ApplyParamGamma, void, __fastcall, (D2ConfigStrc *pConfig), 0xBD80)
//D2FUNC(D2CLIENT, GAME_ApplyParamVSync, void, __fastcall, (D2ConfigStrc *pConfig), 0xBD90)
//D2FUNC(D2CLIENT, GAME_ApplyParamEnableLog, void, __fastcall, (D2ConfigStrc *pConfig), 0xBDA0)
//D2FUNC(D2CLIENT, GAME_ApplyParamInitSeed, void, __fastcall, (D2ConfigStrc *pConfig), 0xBDC0)
//D2FUNC(D2CLIENT, GAME_ApplyParamCharacterName, void, __fastcall, (D2ConfigStrc *pConfig), 0xBDE0)
//D2FUNC(D2CLIENT, GAME_ApplyParamRealm, void, __fastcall, (D2ConfigStrc *pConfig), 0xBE40)
//D2FUNC(D2CLIENT, GAME_ApplyParamSetArenaFlag0, void, __fastcall, (D2ConfigStrc *pConfig), 0xBE60)
//D2FUNC(D2CLIENT, GAME_ApplyParamArenaPlayerClass, void, __fastcall, (D2ConfigStrc *pConfig), 0xBE70)
//D2FUNC(D2CLIENT, GAME_ApplyParamComInterface, void, __fastcall, (D2ConfigStrc *pConfig), 0xBE80)
//D2FUNC(D2CLIENT, GAME_NullFunc, void, __cdecl, (), 0xBE90)
//D2FUNC(D2CLIENT, GAME_ApplyParamCompressedData, void, __fastcall, (D2ConfigStrc *pConfig), 0xBEA0)
//D2FUNC(D2CLIENT, GAME_ApplyParamCompileTxt, void, __fastcall, (D2ConfigStrc *pConfig), 0xBEC0)
//D2FUNC(D2CLIENT, GAME_ApplyLaunchParams, BOOL, __fastcall, (D2ConfigStrc *pConfig), 0xBEE0)
//D2FUNC(D2CLIENT, GAME_IsDisconnecting, int32_t, __cdecl, (), 0xBF20)
//D2FUNC(D2CLIENT, GAME_ResetClickState, void, __cdecl, (), 0xBF30)
//D2FUNC(D2CLIENT, GAME_ResetMouseButtonState, void, __cdecl, (), 0xBF60)
//D2FUNC(D2CLIENT, GAME_GetFrameCount, uint32_t, __cdecl, (), 0xBF80)
//D2FUNC(D2CLIENT, GAME_GetFrameTick, uint32_t, __cdecl, (), 0xBF90)
//D2FUNC(D2CLIENT, GAME_GetInteractThrottle, uint32_t, __cdecl, (), 0xBFA0)
//D2FUNC(D2CLIENT, GAME_SetInteractThrottle, void, __fastcall, (uint32_t dwTick), 0xBFB0)
//D2FUNC(D2CLIENT, GAME_GetRecentTickRate, int32_t, __cdecl, (), 0xBFC0)
//D2FUNC(D2CLIENT, GAME_GetRecentSkipRate, int32_t, __cdecl, (), 0xBFD0)
//D2FUNC(D2CLIENT, UNRESOLVED_sub_6FAABFE0, int32_t, __cdecl, (), 0xBFE0)
//D2FUNC(D2CLIENT, GAME_GetGameType, eD2GameTypes, __cdecl, (), 0xBFF0)
//D2FUNC(D2CLIENT, GAME_ExtendCatchUpDeadline, void, __fastcall, (int32_t nExtraMs), 0xC000)
//D2FUNC(D2CLIENT, GAME_IsGreenBloodEnabled, int32_t, __cdecl, (), 0xC020)
//D2FUNC(D2CLIENT, GAME_SetDifficulty, void, __fastcall, (D2C_Difficulties nDifficulty), 0xC030)
//D2FUNC(D2CLIENT, GAME_SetExpansionGame, void, __fastcall, (uint8_t bExpansion), 0xC040)
//D2FUNC(D2CLIENT, GAME_SetLadderGame, void, __fastcall, (int32_t bLadder), 0xC050)
//D2FUNC(D2CLIENT, GAME_IsLadderGame, int32_t, __cdecl, (), 0xC060)
//D2FUNC(D2CLIENT, GAME_IsExpansionGame, int32_t, __cdecl, (), 0xC080)
//D2FUNC(D2CLIENT, GAME_GetDifficulty, uint8_t, __cdecl, (), 0xC090)
//D2FUNC(D2CLIENT, GAME_UnregisterMouseInputHandlers, void, __cdecl, (), 0xC0A0)
//D2FUNC(D2CLIENT, GAME_RegisterMouseInputHandlers, void, __cdecl, (), 0xC1B0)
//D2FUNC(D2CLIENT, GAME_LeaveGameAfterCinematic, void, __cdecl, (), 0xC2F0)
//D2FUNC(D2CLIENT, GAME_SGameDataFreeNode, void, __thiscall, (struct SGameDataTableStrc *pTable, SGameDataNodeStrc *pNode), 0xC450)
//D2FUNC(D2CLIENT, GAME_SGameDataHashDestroyAll, void, __thiscall, (struct SGameDataTableStrc *pTable), 0xC470)
//D2FUNC(D2CLIENT, GAME_SGameDataAllocNode, SGameDataNodeStrc *, __thiscall, (struct SGameDataTableStrc *pTable, struct TSExplicitList *pList, int32_t nExtraBytes, int32_t nAllocFlags), 0xC500)
//D2FUNC(D2CLIENT, GAME_SGameDataListDestruct, void, __fastcall, (SGameDataTableStrc *pTable), 0xC520)
//D2FUNC(D2CLIENT, GAME_SGameDataListDestroyAll, void, __thiscall, (struct SGameDataTableStrc *pTable), 0xC6A0)
//D2FUNC(D2CLIENT, GAME_SGameDataVecDelDtor2, struct SGameDataTableStrc *, __thiscall, (struct SGameDataTableStrc *pTable, uint8_t bDelete), 0xC780)
//D2FUNC(D2CLIENT, GAME_SGameDataNodeVecDelDtor, SGameDataNodeStrc *, __thiscall, (SGameDataNodeStrc *pNode, uint8_t bDelete), 0xC7B0)
//D2FUNC(D2CLIENT, GAME_SGameDataVecDelDtor1, struct SGameDataTableStrc *, __thiscall, (struct SGameDataTableStrc *pTable, uint8_t bDelete), 0xC7E0)
//D2FUNC(D2CLIENT, GAME_SGameDataDestructHashTable, void, __thiscall, (struct SGameDataTableStrc *pTable), 0xC810)
//D2FUNC(D2CLIENT, GAME_SGameDataVecDelDtor0, struct SGameDataTableStrc *, __thiscall, (struct SGameDataTableStrc *pTable, uint8_t bDelete), 0xC9A0)
//D2FUNC(D2CLIENT, GAME_SGameDataHashUnlinkNodesAndFree, void, __thiscall, (struct TSExplicitListArray *pArray), 0xC9D0)
//D2FUNC(D2CLIENT, GAME_SGameDataUnlinkNode, void, __thiscall, (SGameDataNodeStrc *pNode), 0xCA80)
//D2FUNC(D2CLIENT, GAME_SGameDataAllocAndLinkNode, SGameDataNodeStrc *, __thiscall, (TSExplicitList *pList, int32_t eLinkMode, int32_t nExtraBytes, int32_t nAllocFlags), 0xCAF0)
//D2FUNC(D2CLIENT, GAME_SGameDataHashUnlinkListsAndFree, void, __thiscall, (struct TSExplicitListArray *pArray), 0xCBC0)
//D2FUNC(D2CLIENT, GAME_ListDestruct, void, __fastcall, (struct TSExplicitList *pList), 0xCC00)
//D2FUNC(D2CLIENT, GAME_SGameDataVecDelDtor3, struct SGameDataTableStrc *, __thiscall, (struct SGameDataTableStrc *pTable, uint8_t bDelete), 0xCC80)
//D2FUNC(D2CLIENT, GAME_SGameDataDestructor, void, __thiscall, (struct SGameDataTableStrc *pTable), 0xCCB0)
//D2FUNC(D2CLIENT, GAME_ListNextLink, struct TSLink *, __thiscall, (struct TSLink *pLink, int32_t nLinkOffset), 0xCE10)
//D2FUNC(D2CLIENT, GAME_ListGetLinkSlot, struct TSLink *, __thiscall, (struct TSExplicitList *pList, char *pNode), 0xCE40)
//D2FUNC(D2CLIENT, GAME_ListUnlinkAllNodes, void, __fastcall, (struct TSExplicitList *pList), 0xCE60)
//D2FUNC(D2CLIENT, GAME_ListVecDelDtor, void *, __thiscall, (struct TSExplicitList *pList, uint8_t bDelete), 0xCEB0)
//D2FUNC(D2CLIENT, GAME_SGameDataFreeRecordArray, void, __thiscall, (struct SGameDataChunkStrc *pChunk), 0xCEE0)
//D2FUNC(D2CLIENT, GAME_ListUnlinkNode, void, __fastcall, (struct TSLink *pLink), 0xD010)
//D2FUNC(D2CLIENT, GAME_ListNodeToLink, struct TSLink *, __fastcall, (char *pNode, int32_t nLinkOffset), 0xD050)
//D2FUNC(D2CLIENT, MSG_Send_D2CLTSYS_NEWGAME, void, __fastcall, (const char *szName), 0xD260)
//D2FUNC(D2CLIENT, MSG_Send_D2CLTSYS_JOINACT, void, __cdecl, (), 0xD380)
//D2FUNC(D2CLIENT, MSG_SendPing6C, void, __cdecl, (), 0xD3B0)
//D2FUNC(D2CLIENT, MSG_Send_D2CLTSYS_ENDGAME, void, __cdecl, (), 0xD420)
//D2FUNC(D2CLIENT, MSG_Send_D2CLTSYS_JOINGAME, void, __fastcall, (int16_t nGameId, int32_t nTokenId), 0xD4B0)
//D2FUNC(D2CLIENT, MSG_SetLoadProgressRatio, void, __fastcall, (int32_t nDone, int32_t nTotal), 0xD530)
//D2FUNC(D2CLIENT, MSG_GetLoadProgressRatio, int32_t, __cdecl, (), 0xD550)
//D2FUNC(D2CLIENT, MSG_Send_D2CLTSYS_OPENCHAR, void, __fastcall, (int32_t bShowLoadingScreen), 0xD560)
//D2FUNC(D2CLIENT, MSG_SendCmd1, void, __fastcall, (uint8_t nCmd), 0xD830)
//D2FUNC(D2CLIENT, MSG_SendPacketToServer, void, __fastcall, (uint8_t *pPacket, int32_t nLen), 0xD850)
//D2FUNC(D2CLIENT, MSG_SendCmdWord, void, __fastcall, (uint8_t nCmd, int16_t nValue), 0xD990)
//D2FUNC(D2CLIENT, MSG_SendCmdXY, void, __fastcall, (uint8_t nCmd, int16_t nX, int16_t nY), 0xD9B0)
//D2FUNC(D2CLIENT, MSG_SendCmd3Words, void, __fastcall, (uint8_t nCmd, int16_t nWord1, int16_t nWord2, int16_t nWord3), 0xD9E0)
//D2FUNC(D2CLIENT, MSG_SendServerItemMessage, void, __fastcall, (uint8_t nCmd, int32_t nUnitGUID), 0xDA20)
//D2FUNC(D2CLIENT, MSG_SendCmdUnit, void, __fastcall, (uint8_t nCmd, int32_t nUnitType, int32_t nUnitGUID), 0xDA40)
//D2FUNC(D2CLIENT, MSG_SendCmdUnitParam, void, __fastcall, (uint8_t nCmd, int32_t nUnitType, int32_t nUnitGUID, int32_t nParam), 0xDA70)
//D2FUNC(D2CLIENT, MSG_SendCmd4Dwords, void, __fastcall, (uint8_t nCmd, int32_t nDword1, int32_t nDword2, int32_t nDword3, int32_t nDword4), 0xDAA0)
//D2FUNC(D2CLIENT, MSG_SendClt5D, void, __fastcall, (D2C_PartyCommands nType, int32_t dwPlayerGUID, int32_t bToggle), 0xDAE0)
//D2FUNC(D2CLIENT, MSG_SendClt5E, void, __fastcall, (D2C_PartyCommands nType, int32_t dwPlayerGUID), 0xDB20)
//D2FUNC(D2CLIENT, MSG_SendChatMessage, void, __fastcall, (const void *pMsg), 0xDB50)
//D2FUNC(D2CLIENT, MSG_JoinGameMessageLoop, BOOL, __stdcall, (int32_t nUnused), 0xDBC0)
//D2FUNC(D2CLIENT, MSG_FreeGame, int32_t, __cdecl, (), 0xDC70)
//D2FUNC(D2CLIENT, MSG_IsJoinGameLoopDone, int32_t, __cdecl, (), 0xDC90)
//D2FUNC(D2CLIENT, MSG_JoinGameIdlePoll, BOOL, __stdcall, (int32_t nUnused), 0xDCA0)
//D2FUNC(D2CLIENT, MSG_AppModeJoinGame, D2C_AppMode, __fastcall, (), 0xDD00)
//D2FUNC(D2CLIENT, MSG_LeaveGameMessageLoop, BOOL, __stdcall, (int32_t nUnused), 0xDDF0)
//D2FUNC(D2CLIENT, MSG_AppModeLeaveGame, D2C_AppMode, __fastcall, (), 0xDE60)
//D2FUNC(D2CLIENT, PALSHIFT_Init, void, __cdecl, (), 0xDEA0)
//D2FUNC(D2CLIENT, PALSHIFT_Shutdown, void, __cdecl, (), 0xDFB0)
//D2FUNC(D2CLIENT, PALSHIFT_EnsureMonsterPalette, void, __fastcall, (D2C_MonsterIds nMonsterClassId), 0xE030)
//D2FUNC(D2CLIENT, PALSHIFT_FindCacheEntry, D2ClientPalShiftStrc *, __fastcall, (D2C_MonsterIds nMonsterClassId), 0xE2E0)
//D2FUNC(D2CLIENT, PALSHIFT_ApplyGreenBlood, void, __fastcall, (D2C_MonsterIds nMonsterClassId, D2ClientPalShiftStrc *pPalShift), 0xE300)
//D2FUNC(D2CLIENT, PALSHIFT_GetUnitShiftTable, uint8_t *, __fastcall, (D2UnitStrc *pUnit), 0xE3C0)
//D2FUNC(D2CLIENT, PALSHIFT_RollUniqueShift, uint32_t, __fastcall, (D2UnitStrc *pUnit, uint16_t nOffset), 0xE510)
//D2FUNC(D2CLIENT, PALSHIFT_GetGreenBloodRemap, uint8_t *, __cdecl, (), 0xE560)
//D2FUNC(D2CLIENT, DATATBLS_GetMonStatsTxtRecord, D2MonStatsTxt *, __fastcall, (int32_t nMonStatsId), 0xE570)
//D2FUNC(D2CLIENT, PALSHIFT_GetMonStatsRecordCount, int32_t, __cdecl, (), 0xE5A0)
//D2FUNC(D2CLIENT, RECORD_LogEvent, void, __fastcall, (uint32_t nEventId, WPARAM wParam, LPARAM lParam), 0xE5B0)
//D2FUNC(D2CLIENT, RECORD_SaveToFile, void, __cdecl, (), 0xE810)
//D2FUNC(D2CLIENT, RECORD_FreeBufferList, void, __cdecl, (), 0xE910)
//D2FUNC(D2CLIENT, RECORD_Play, void, __cdecl, (), 0xE940)
//D2FUNC(D2CLIENT, RECORD_GetRecordingRequested, int32_t, __cdecl, (), 0xEAA0)
//D2FUNC(D2CLIENT, RECORD_CleanupHandler, int32_t, __stdcall, (int32_t a1, int32_t a2, int32_t a3), 0xEAB0)
//D2FUNC(D2CLIENT, RECORD_InstallSaveOnCleanup, void, __cdecl, (), 0xEAD0)
//D2FUNC(D2CLIENT, RECORD_SetRecordPlaybackLoaded, void, __cdecl, (), 0xEAF0)
//D2FUNC(D2CLIENT, RECORD_GetPlaybackRequested, int32_t, __cdecl, (), 0xEB00)
//D2FUNC(D2CLIENT, RECORD_GetRecordPlaybackLoaded, int32_t, __cdecl, (), 0xEB10)
//D2FUNC(D2CLIENT, RECORD_RequestRecording, void, __cdecl, (), 0xEB20)
//D2FUNC(D2CLIENT, RECORD_ReadNextForFrame, void, __cdecl, (), 0xEB40)
//D2FUNC(D2CLIENT, ROSTER_AddPlayer, void, __fastcall, (uint32_t dwUnitGUID, int32_t nClass, const char *szName, int16_t nLevel, int16_t nPartyId, int16_t nUnused, D2C_PartyButtons eButton, int16_t nGuildFlags, const char *szGuildTag, const char *szGuildName), 0xEC80)
//D2FUNC(D2CLIENT, ROSTER_EntryHasCorpse, int32_t, __fastcall, (D2RosterUnitStrc *pRosterEntry, uint32_t dwCorpseId), 0xF050)
//D2FUNC(D2CLIENT, ROSTER_UpdatePartyState, void, __fastcall, (D2UnitGUID dwUnitId, int16_t nPartyId, int16_t nLevel, int16_t nUnusedFlags, D2C_PartyButtons eButton), 0xF070)
//D2FUNC(D2CLIENT, ROSTER_RemovePlayer, void, __fastcall, (uint32_t dwPlayerGUID), 0xF260)
//D2FUNC(D2CLIENT, ROSTER_FreeAll, void, __cdecl, (), 0xF4E0)
//D2FUNC(D2CLIENT, ROSTER_PartyGetNextEntry, D2RosterUnitStrc *, __fastcall, (D2RosterUnitStrc *pEntry), 0xF5B0)
//D2FUNC(D2CLIENT, ROSTER_GetDisplayName, char *, __fastcall, (D2RosterUnitStrc *pEntry), 0xF5D0)
//D2FUNC(D2CLIENT, ROSTER_SortEntriesByName, D2RosterUnitStrc *, __fastcall, (D2RosterUnitStrc *pList), 0xF600)
//D2FUNC(D2CLIENT, ROSTER_SortEntriesByKillCount, D2RosterUnitStrc *, __fastcall, (D2RosterUnitStrc *pList), 0xF6C0)
//D2FUNC(D2CLIENT, ROSTER_FindListNode, D2RosterGroupStrc *, __fastcall, (int16_t nPartyId), 0xF720)
//D2FUNC(D2CLIENT, ROSTER_FindOrAddListNode, D2RosterGroupStrc *, __fastcall, (int16_t nPartyId), 0xF740)
//D2FUNC(D2CLIENT, ROSTER_FreeList, void, __cdecl, (), 0xF790)
//D2FUNC(D2CLIENT, ROSTER_PartyExtractPartyMembers, void, __fastcall, (D2RosterGroupStrc *pGroup), 0xF7D0)
//D2FUNC(D2CLIENT, ROSTER_SortList, void, __stdcall, (), 0xF830)
//D2FUNC(D2CLIENT, ROSTER_MoveListNodeAndEntryToFront, void, __fastcall, (D2UnitGUID dwUnitId, int16_t nPartyId), 0xF890)
//D2FUNC(D2CLIENT, ROSTER_ResortAllGroups, void, __fastcall, (), 0xF900)
//D2FUNC(D2CLIENT, ROSTER_FlattenGroupsIntoEntryList, void, __cdecl, (), 0xFA90)
//D2FUNC(D2CLIENT, ROSTER_MoveEntryToFront, void, __fastcall, (D2UnitGUID dwUnitId), 0xFAE0)
//D2FUNC(D2CLIENT, ROSTER_OnPartyUpdate, void, __fastcall, (uint32_t dwUnitId, int32_t nKillCount), 0xFB20)
//D2FUNC(D2CLIENT, ROSTER_EntryGetKillCount, int32_t, __fastcall, (D2RosterUnitStrc *pEntry), 0xFD30)
//D2FUNC(D2CLIENT, ROSTER_EntryGetLevel, int16_t, __fastcall, (D2RosterUnitStrc *pEntry), 0xFD40)
//D2FUNC(D2CLIENT, ROSTER_EntryGetClassId, int32_t, __fastcall, (D2RosterUnitStrc *pEntry), 0xFD50)
//D2FUNC(D2CLIENT, ROSTER_FindEntryByUnitOrCorpse, D2RosterUnitStrc *, __fastcall, (D2UnitGUID dwUnitId), 0xFD60)
//D2FUNC(D2CLIENT, ROSTER_PartyGetSharedPartyId, int16_t, __fastcall, (D2RosterUnitStrc *pRosterEntry), 0xFDA0)
//D2FUNC(D2CLIENT, ROSTER_FindPartyIdByUnit, int16_t, __fastcall, (D2UnitStrc *pUnit), 0xFDE0)
//D2FUNC(D2CLIENT, ROSTER_EntryAllowsLoot, BOOL, __fastcall, (D2RosterUnitStrc *pEntry), 0xFE30)
//D2FUNC(D2CLIENT, ROSTER_EntryIsIgnored, BOOL, __fastcall, (D2RosterUnitStrc *pEntry), 0xFE70)
//D2FUNC(D2CLIENT, ROSTER_EntryIsSquelched, BOOL, __fastcall, (D2RosterUnitStrc *pEntry), 0xFEB0)
//D2FUNC(D2CLIENT, ROSTER_EntryIsHostile, BOOL, __fastcall, (D2RosterUnitStrc *pEntry), 0xFEF0)
//D2FUNC(D2CLIENT, ROSTER_EntryGetPartyButton, D2C_PartyButtons, __fastcall, (D2RosterUnitStrc *pEntry), 0xFF30)
//D2FUNC(D2CLIENT, ROSTER_ToggleLootAndNotify, void, __fastcall, (D2RosterUnitStrc *pEntry), 0xFF40)
//D2FUNC(D2CLIENT, ROSTER_ToggleIgnoreAndNotify, void, __fastcall, (D2RosterUnitStrc *pEntry), 0xFFB0)
//D2FUNC(D2CLIENT, ROSTER_ToggleSquelchAndNotify, void, __fastcall, (D2RosterUnitStrc *pEntry), 0x10020)
//D2FUNC(D2CLIENT, ROSTER_PartySendButtonCommand, void, __fastcall, (D2RosterUnitStrc *pEntry), 0x10090)
//D2FUNC(D2CLIENT, ROSTER_PartySendLeaveParty, void, __cdecl, (), 0x10330)
//D2FUNC(D2CLIENT, ROSTER_PartySendHostileToggle, void, __fastcall, (D2RosterUnitStrc *pEntry), 0x10540)
//D2FUNC(D2CLIENT, ROSTER_IsHostileTo, BOOL, __fastcall, (D2UnitGUID dwPlayerGUID, D2UnitGUID dwTargetGUID), 0x107D0)
//D2FUNC(D2CLIENT, ROSTER_GetEntryUnitId, int32_t, __fastcall, (D2RosterUnitStrc *pRosterEntry), 0x10870)
//D2FUNC(D2CLIENT, ROSTER_UpdateLifeAndLevel, void, __fastcall, (int32_t dwUnitGUID, int32_t nLifePercentage, uint16_t nLevelId), 0x10880)
//D2FUNC(D2CLIENT, ROSTER_SetEntryPosition, void, __fastcall, (uint32_t dwUnitId, int32_t nX, int32_t nY), 0x10A90)
//D2FUNC(D2CLIENT, ROSTER_GetPartyLife, int32_t, __fastcall, (uint32_t dwUnitId), 0x10AE0)
//D2FUNC(D2CLIENT, ROSTER_IsUnitNotSquelched, BOOL, __fastcall, (D2UnitGUID dwUnitId), 0x10B20)
//D2FUNC(D2CLIENT, ROSTER_GetEntryPosition, void, __fastcall, (D2RosterUnitStrc *pRosterEntry, int32_t *pnX, int32_t *pnY), 0x10B80)
//D2FUNC(D2CLIENT, ROSTER_GetEntryLevelId, int32_t, __fastcall, (D2RosterUnitStrc *pRosterEntry), 0x10BB0)
//D2FUNC(D2CLIENT, ROSTER_GetNameEx, char *, __fastcall, (uint32_t dwUnitId), 0x10BC0)
//D2FUNC(D2CLIENT, ROSTER_GetPlayerDisplayName, char *, __fastcall, (D2UnitStrc *pUnit), 0x10C00)
//D2FUNC(D2CLIENT, ROSTER_SetEntryPortalGuids, void, __fastcall, (uint32_t dwUnitId, uint32_t dwSrcPortalGUID, uint32_t dwDstPortalGUID), 0x10CB0)
//D2FUNC(D2CLIENT, ROSTER_GetNameExByPortalGuid, char *, __fastcall, (uint32_t dwPortalGUID), 0x10D00)
//D2FUNC(D2CLIENT, ROSTER_HasOtherPartyMember, BOOL, __fastcall, (D2UnitStrc *pUnit), 0x10D30)
//D2FUNC(D2CLIENT, ROSTER_GetInfoListByUnitOrCorpse, D2RosterInfoStrc **, __fastcall, (uint32_t dwUnitId), 0x10D80)
//D2FUNC(D2CLIENT, ROSTER_SetPartyId, void, __fastcall, (int32_t dwPlayerGUID, int16_t nPartyGUID), 0x10DC0)
//D2FUNC(D2CLIENT, ROSTER_SetPartyButton, void, __fastcall, (int32_t nUnitId, uint8_t eButton), 0x10FB0)
//D2FUNC(D2CLIENT, ROSTER_ApplyInfoFlags, void, __fastcall, (D2UnitGUID dwPlayerGUID, D2UnitGUID dwTargetGUID, uint16_t nFlags), 0x11000)
//D2FUNC(D2CLIENT, ROSTER_FindCorpseEntry, void, __fastcall, (uint32_t dwUnitId, uint32_t dwCorpseId), 0x11080)
//D2FUNC(D2CLIENT, ROSTER_RemoveCorpseFromEntry, void, __fastcall, (int32_t nUnitId, int32_t nCorpseId), 0x11180)
//D2FUNC(D2CLIENT, ROSTER_CanUnitLootFrom, BOOL, __fastcall, (D2UnitGUID dwLooterGUID, D2UnitGUID dwOwnerGUID), 0x11200)
//D2FUNC(D2CLIENT, ROSTER_UnitsShareParty, BOOL, __fastcall, (D2UnitGUID dwUnitIdA, D2UnitGUID dwUnitIdB), 0x11260)
//D2FUNC(D2CLIENT, ROSTER_IsAnyoneHostileToPlayer, BOOL, __cdecl, (), 0x112F0)
//D2FUNC(D2CLIENT, ROSTER_EntryIsHostileToPlayer, BOOL, __fastcall, (D2RosterUnitStrc *pEntry), 0x11340)
//D2FUNC(D2CLIENT, ROSTER_PlayerIsHostileToEntry, BOOL, __fastcall, (D2RosterUnitStrc *pEntry), 0x11370)
//D2FUNC(D2CLIENT, ROSTER_EntryAllowsPlayerLoot, BOOL, __fastcall, (D2RosterUnitStrc *pEntry), 0x113A0)
//D2FUNC(D2CLIENT, ROSTER_PlayerAllowsEntryLoot, BOOL, __fastcall, (D2RosterUnitStrc *pEntry), 0x113D0)
//D2FUNC(D2CLIENT, ROSTER_EntryIsMale, BOOL, __fastcall, (D2RosterUnitStrc *pRosterEntry), 0x11400)
//D2FUNC(D2CLIENT, ROSTER_SetPartyLife, void, __fastcall, (uint32_t dwUnitId, uint8_t nLife), 0x11420)
//D2FUNC(D2CLIENT, ROSTER_GetOtherPlayerPartyLife, int32_t, __fastcall, (uint32_t dwUnitId), 0x11460)
//D2FUNC(D2CLIENT, ROSTERPETS_FreeAll, void, __cdecl, (), 0x114C0)
//D2FUNC(D2CLIENT, ROSTERPETS_FindOrAddPet, void, __fastcall, (uint32_t dwUnitId, uint32_t dwOwnerId, int32_t nPetTypeId, int32_t nMonStatsId), 0x114F0)
//D2FUNC(D2CLIENT, ROSTERPETS_OnPetPacket81, void, __fastcall, (uint32_t dwUnitId, uint32_t dwOwnerId, int32_t nPetTypeId, uint32_t nMonStatsId, uint32_t *pPetInfo), 0x115D0)
//D2FUNC(D2CLIENT, ROSTERPETS_RemovePetOrMarkHireling, void, __fastcall, (uint32_t dwUnitId), 0x117A0)
//D2FUNC(D2CLIENT, ROSTERPETS_GetAutomapClass, int32_t, __fastcall, (D2UnitGUID dwPetUnitId), 0x11810)
//D2FUNC(D2CLIENT, ROSTERPETS_GetPetInfo, D2ClientPetInfoStrc *, __fastcall, (uint32_t dwPetUnitId), 0x118D0)
//D2FUNC(D2CLIENT, ROSTERPETS_CountByType, int32_t, __fastcall, (D2UnitStrc *pOwner, int32_t nPetTypeId), 0x118F0)
//D2FUNC(D2CLIENT, ROSTERPETS_GetAssociatedUnitGUID, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t nKind, int32_t bIncludeInactive), 0x11920)
//D2FUNC(D2CLIENT, ROSTERPETS_UnitHasPets, BOOL, __fastcall, (D2UnitStrc *pUnit), 0x11970)
//D2FUNC(D2CLIENT, ROSTERPETS_UpdateSkeletonPalettes, void, __cdecl, (), 0x119A0)
//D2FUNC(D2CLIENT, ROSTERPETS_UpdateLifeAndArea, void, __fastcall, (uint32_t dwPetUnitId, int32_t nLifePercent, uint16_t wArea), 0x11A40)
//D2FUNC(D2CLIENT, ROSTERPETS_GetLifePercent, int32_t, __fastcall, (uint32_t dwPetUnitId), 0x11AC0)
//D2FUNC(D2CLIENT, ROSTERPETS_GetMonStatsId, int32_t, __fastcall, (uint32_t dwPetUnitId), 0x11AE0)
//D2FUNC(D2CLIENT, ROSTERPETS_GetOwnerId, int32_t, __fastcall, (uint32_t dwPetUnitId), 0x11B00)
//D2FUNC(D2CLIENT, ROSTERPETS_GetPetType, int32_t, __fastcall, (uint32_t dwPetUnitId), 0x11B20)
//D2FUNC(D2CLIENT, ROSTERPETS_IsPetOwnerHostile, int32_t, __fastcall, (D2UnitStrc *pPet), 0x11B40)
//D2FUNC(D2CLIENT, ROSTERPETS_UpdateRaisedUndead, void, __fastcall, (uint32_t dwOwnerGuid), 0x11BB0)
//D2FUNC(D2CLIENT, SCMD_SaveCharacterFile, void, __fastcall, (uint8_t *pBuffer, int32_t nLen), 0x11CB0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv01_GameFlags, void, __fastcall, (D2GSPacketSrv01 *pPacket), 0x12130)
//D2FUNC(D2CLIENT, SCMD_HandleSrv03_GameSeed, void, __fastcall, (D2GSPacketSrv03 *pPacket), 0x12160)
//D2FUNC(D2CLIENT, SCMD_HandleSrv02_ActLoaded, void, __fastcall, (D2GSPacketSrv02 *pPacket), 0x12180)
//D2FUNC(D2CLIENT, SCMD_HandleSrv04_LoadComplete, void, __fastcall, (D2GSPacketSrv04 *pPacket), 0x12190)
//D2FUNC(D2CLIENT, SCMD_HandleSrv05, void, __fastcall, (D2GSPacketSrv05 *pPacket), 0x12220)
//D2FUNC(D2CLIENT, SCMD_HandleSrv06, void, __fastcall, (D2GSPacketSrv06 *pPacket), 0x12250)
//D2FUNC(D2CLIENT, SCMD_HandleSrv07_MapReveal, void, __fastcall, (D2GSPacketSrv07 *pPacket), 0x122A0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv08_MapHide, void, __fastcall, (D2GSPacketSrv08 *pPacket), 0x12310)
//D2FUNC(D2CLIENT, SCMD_HandleSrv09_SpawnUnit, void, __fastcall, (D2GSPacketSrv09 *pPacket), 0x12380)
//D2FUNC(D2CLIENT, SCMD_HandleSrv51_SpawnObject, void, __fastcall, (D2GSPacketSrv51 *pPacket), 0x123D0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv52_QuestList, void, __fastcall, (D2GSPacketSrv52 *pPacket), 0x12400)
//D2FUNC(D2CLIENT, SCMD_HandleSrv0A_RemoveUnit, void, __fastcall, (D2GSPacketSrv0A *pPacket), 0x12410)
//D2FUNC(D2CLIENT, SCMD_HandleSrv0B, void, __fastcall, (D2GSPacketSrv0B *pPacket), 0x12450)
//D2FUNC(D2CLIENT, SCMD_HandleSrv0C_Action, void, __fastcall, (D2UnitStrc *pUnit, D2GSPacketSrv0C *pPacket), 0x12470)
//D2FUNC(D2CLIENT, SCMD_HandleSrv0D_ActionAtXY, void, __fastcall, (D2UnitStrc *pUnit, D2GSPacketSrv0D *pPacket), 0x124B0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv0E_ActionOnTarget, void, __fastcall, (D2UnitStrc *pUnit, D2GSPacketSrv0E *pPacket), 0x12510)
//D2FUNC(D2CLIENT, SCMD_HandleSrv0F_ActionAtXYFromPos, void, __fastcall, (D2UnitStrc *pUnit, D2GSPacketSrv0F *pPacket), 0x12540)
//D2FUNC(D2CLIENT, SCMD_HandleSrv10_ActionOnTargetFromPos, void, __fastcall, (D2UnitStrc *pUnit, D2GSPacketSrv10 *pPacket), 0x125A0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv67_ActionAtXYPathed, void, __fastcall, (D2UnitStrc *pUnit, D2GSPacketSrv67 *pPacket), 0x125F0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv68_ActionOnTargetPathedFromPos, void, __fastcall, (D2UnitStrc *pUnit, D2GSPacketSrv68 *pPacket), 0x12650)
//D2FUNC(D2CLIENT, SCMD_HandleSrv69_ActionAtXY, void, __fastcall, (D2UnitStrc *pUnit, D2GSPacketSrv69 *pPacket), 0x126D0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv6A_ActionOnTarget, void, __fastcall, (D2UnitStrc *pUnit, D2GSPacketSrv6A *pPacket), 0x12730)
//D2FUNC(D2CLIENT, SCMD_HandleSrv6B_ActionAtXYFromPos, void, __fastcall, (D2UnitStrc *pUnit, D2GSPacketSrv6B *pPacket), 0x12790)
//D2FUNC(D2CLIENT, SCMD_HandleSrv6C_ActionOnTargetFromPos, void, __fastcall, (D2UnitStrc *pUnit, D2GSPacketSrv6C *pPacket), 0x12810)
//D2FUNC(D2CLIENT, SCMD_HandleSrv6D_PierceAtXY, void, __fastcall, (D2UnitStrc *pUnit, D2GSPacketSrv6D *pPacket), 0x12880)
//D2FUNC(D2CLIENT, SCMD_HandleSrv11, void, __fastcall, (D2GSPacketSrv11 *pPacket), 0x12930)
//D2FUNC(D2CLIENT, SCMD_HandleSrv15_ReassignPlayer, void, __fastcall, (D2GSPacketSrv15 *pPacket), 0x12970)
//D2FUNC(D2CLIENT, SCMD_HandleSrv17_ApplyRemoteUnitAction, void, __fastcall, (D2UnitStrc *pUnit, uint8_t *pPacket), 0x12A80)
//D2FUNC(D2CLIENT, SCMD_HandleSrv16_UnitsUpdate, void, __fastcall, (D2GSPacketSrv16 *pPacket), 0x12B00)
//D2FUNC(D2CLIENT, SCMD_HandleSrv28_NpcMenu, void, __fastcall, (D2GSPacketSrv28 *pPacket), 0x12B80)
//D2FUNC(D2CLIENT, SCMD_HandleSrv62, void, __fastcall, (D2GSPacketSrv62 *pPacket), 0x12BA0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv29_QuestRecord, void, __fastcall, (D2GSPacketSrv29 *pPacket), 0x12BB0)
//D2FUNC(D2CLIENT, SCMD_RemergeItemStatLists, void, __fastcall, (D2UnitStrc *pUnit), 0x12BC0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv9E_A2, void, __fastcall, (uint8_t *pPacket), 0x12C90)
//D2FUNC(D2CLIENT, SCMD_HandleSrvA3, void, __fastcall, (D2GSPacketSrvA3 *pPacket), 0x12D60)
//D2FUNC(D2CLIENT, SCMD_HandleSrvA5, void, __fastcall, (D2GSPacketSrvA5 *pPacket), 0x12E20)
//D2FUNC(D2CLIENT, SCMD_HandleSrvA4, void, __fastcall, (D2GSPacketSrvA4 *pPacket), 0x12EE0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv19_1F_AttributeUpdate, void, __fastcall, (uint8_t *pPacket), 0x12F10)
//D2FUNC(D2CLIENT, SCMD_HandleSrv20, void, __fastcall, (D2GSPacketSrv20 *pPacket), 0x130A0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv18_PlayerAttributes, void, __fastcall, (D2GSPacketSrv18 *pPacket), 0x13170)
//D2FUNC(D2CLIENT, SCMD_HandleSrv95, void, __fastcall, (D2GSPacketSrv95 *pPacket), 0x13320)
//D2FUNC(D2CLIENT, SCMD_HandleSrv96, void, __fastcall, (D2GSPacketSrv96 *pPacket), 0x13480)
//D2FUNC(D2CLIENT, SCMD_HandleSrv21_UpdateSkills, void, __fastcall, (D2GSPacketSrv21 *pPacket), 0x13570)
//D2FUNC(D2CLIENT, SCMD_HandleSrv93, void, __fastcall, (D2GSPacketSrv93 *pPacket), 0x135C0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv94_BaseSkillLevels, void, __fastcall, (D2GSPacketSrv94 *pPacket), 0x13610)
//D2FUNC(D2CLIENT, SCMD_HandleSrv22_SetSkill, void, __fastcall, (D2GSPacketSrv22 *pPacket), 0x13670)
//D2FUNC(D2CLIENT, SCMD_HandleSrv23, void, __fastcall, (D2GSPacketSrv23 *pPacket), 0x136D0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv98, void, __fastcall, (D2GSPacketSrv98 *pPacket), 0x13710)
//D2FUNC(D2CLIENT, SCMD_HandleSrv99, void, __fastcall, (D2GSPacketSrv99 *pPacket), 0x13740)
//D2FUNC(D2CLIENT, SCMD_HandleSrv9A, void, __fastcall, (D2GSPacketSrv9A *pPacket), 0x13790)
//D2FUNC(D2CLIENT, SCMD_HandleSrv4C_UnitAction, void, __fastcall, (D2UnitStrc *pUnit, D2GSPacketSrv4C *pPacket), 0x137D0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv4D_UnitAction, void, __fastcall, (D2UnitStrc *pUnit, D2GSPacketSrv4D *pPacket), 0x13830)
//D2FUNC(D2CLIENT, SCMD_HandleSrv26, void, __fastcall, (D2GSPacketSrv26 *pPacket), 0x13890)
//D2FUNC(D2CLIENT, SCMD_HandleSrv76_ClearUnitHoverText, void, __fastcall, (D2GSPacketSrv76 *pPacket), 0x13910)
//D2FUNC(D2CLIENT, SCMD_HandleSrv5A, void, __fastcall, (D2GSPacketSrv5A *pPacket), 0x13930)
//D2FUNC(D2CLIENT, SCMD_HandleSrv27_OverheadText, void, __fastcall, (D2GSPacketSrv27 *pPacket), 0x13960)
//D2FUNC(D2CLIENT, SCMD_HandleSrv2A_TransactionResult, void, __fastcall, (D2GSPacketSrv2A *pPacket), 0x13990)
//D2FUNC(D2CLIENT, SCMD_HandleSrv2C_PlayUnitSound, void, __fastcall, (D2GSPacketSrv2C *pPacket), 0x139D0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv3E, void, __fastcall, (D2GSPacketSrv3E *pPacket), 0x139F0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv3F, void, __fastcall, (D2GSPacketSrv3F *pPacket), 0x13AE0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv40, void, __fastcall, (D2GSPacketSrv40 *pPacket), 0x13B00)
//D2FUNC(D2CLIENT, SCMD_HandleSrv42, void, __fastcall, (D2GSPacketSrv42 *pPacket), 0x13B30)
//D2FUNC(D2CLIENT, SCMD_HandleSrv47_RefreshItem, void, __fastcall, (D2GSPacketSrv47 *pPacket), 0x13B50)
//D2FUNC(D2CLIENT, SCMD_HandleSrv48, void, __fastcall, (D2GSPacketSrv48 *pPacket), 0x13B90)
//D2FUNC(D2CLIENT, SCMD_HandleSrv53, void, __fastcall, (D2GSPacketSrv53 *pPacket), 0x13BC0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv50, void, __fastcall, (D2GSPacketSrv50 *pPacket), 0x13C30)
//D2FUNC(D2CLIENT, j_MEMPART_Nullsub1, void, __thiscall, (void *thisptr), 0x13C70)
//D2FUNC(D2CLIENT, SCMD_HandleSrv4F, void, __fastcall, (D2GSPacketSrv4F *pPacket), 0x13C80)
//D2FUNC(D2CLIENT, SCMD_HandleSrv4E, void, __fastcall, (D2GSPacketSrv4E *pPacket), 0x13C90)
//D2FUNC(D2CLIENT, SCMD_HandleSrv57, void, __fastcall, (D2GSPacketSrv57 *pPacket), 0x13CC0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv58, void, __fastcall, (D2GSPacketSrv58 *pPacket), 0x13D50)
//D2FUNC(D2CLIENT, SCMD_HandleSrv59_AssignPlayer, void, __fastcall, (D2GSPacketSrv59 *pPacket), 0x13D80)
//D2FUNC(D2CLIENT, SCMD_HandleSrv5B, void, __fastcall, (D2GSPacketSrv5B *pPacket), 0x13DA0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv5C, void, __fastcall, (D2GSPacketSrv5C *pPacket), 0x13DF0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv5D, void, __fastcall, (D2GSPacketSrv5D *pPacket), 0x13E00)
//D2FUNC(D2CLIENT, SCMD_HandleSrv5E, void, __fastcall, (D2GSPacketSrv5E *pPacket), 0x13E30)
//D2FUNC(D2CLIENT, SCMD_HandleSrv91, void, __fastcall, (D2GSPacketSrv91 *pPacket), 0x13E40)
//D2FUNC(D2CLIENT, SCMD_HandleSrv92, void, __fastcall, (D2GSPacketSrv92 *pPacket), 0x13E70)
//D2FUNC(D2CLIENT, SCMD_HandleSrv5F, void, __fastcall, (D2GSPacketSrv5F *pPacket), 0x13E90)
//D2FUNC(D2CLIENT, SCMD_HandleSrv60_TownPortalState, void, __fastcall, (D2GSPacketSrv60 *pPacket), 0x13ED0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv61, void, __fastcall, (D2GSPacketSrv61 *pPacket), 0x13F20)
//D2FUNC(D2CLIENT, SCMD_HandleSrv63, void, __fastcall, (D2GSPacketSrv63 *pPacket), 0x13F30)
//D2FUNC(D2CLIENT, SCMD_HandleSrv65_RosterUpdate, void, __fastcall, (D2GSPacketSrv65 *pPacket), 0x13F60)
//D2FUNC(D2CLIENT, SCMD_HandleSrv73, void, __fastcall, (D2GSPacketSrv73 *pPacket), 0x13F70)
//D2FUNC(D2CLIENT, SCMD_HandleSrv74, void, __fastcall, (D2GSPacketSrv74 *pPacket), 0x140C0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv75, void, __fastcall, (D2GSPacketSrv75 *pPacket), 0x140F0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv77, void, __fastcall, (D2GSPacketSrv77 *pPacket), 0x14110)
//D2FUNC(D2CLIENT, SCMD_HandleSrv78, void, __fastcall, (D2GSPacketSrv78 *pPacket), 0x14120)
//D2FUNC(D2CLIENT, SCMD_HandleSrv79, void, __fastcall, (D2GSPacketSrv79 *pPacket), 0x14150)
//D2FUNC(D2CLIENT, SCMD_HandleSrv7A_RosterPetsOnPetPacket, void, __fastcall, (D2GSPacketSrv7A *pPacket), 0x14160)
//D2FUNC(D2CLIENT, SCMD_HandleSrv81, void, __fastcall, (D2GSPacketSrv81 *pPacket), 0x14190)
//D2FUNC(D2CLIENT, SCMD_HandleSrv7B, void, __fastcall, (D2GSPacketSrv7B *pPacket), 0x141D0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv7C, void, __fastcall, (D2GSPacketSrv7C *pPacket), 0x14220)
//D2FUNC(D2CLIENT, SCMD_HandleSrv7D, void, __fastcall, (D2GSPacketSrv7D *pPacket), 0x14240)
//D2FUNC(D2CLIENT, SCMD_HandleSrv7E, void, __fastcall, (D2GSPacketSrv7E *pPacket), 0x14280)
//D2FUNC(D2CLIENT, SCMD_HandleSrv7F, void, __fastcall, (D2GSPacketSrv7F *pPacket), 0x142A0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv90, void, __fastcall, (D2GSPacketSrv90 *pPacket), 0x142D0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv82, void, __fastcall, (D2GSPacketSrv82 *pPacket), 0x142E0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv89, void, __fastcall, (D2GSPacketSrv89 *pPacket), 0x14340)
//D2FUNC(D2CLIENT, SCMD_HandleSrv8A, void, __fastcall, (D2GSPacketSrv8A *pPacket), 0x14350)
//D2FUNC(D2CLIENT, SCMD_HandleSrv8B, void, __fastcall, (D2GSPacketSrv8B *pPacket), 0x14370)
//D2FUNC(D2CLIENT, SCMD_HandleSrv8C, void, __fastcall, (D2GSPacketSrv8C *pPacket), 0x14380)
//D2FUNC(D2CLIENT, SCMD_HandleSrv8D, void, __fastcall, (D2GSPacketSrv8D *pPacket), 0x143A0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv8E_RosterOnCorpseAssignment, void, __fastcall, (D2GSPacketSrv8E *pPacket), 0x143B0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv9B, void, __fastcall, (D2GSPacketSrv9B *pPacket), 0x143D0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv97_WeaponSwitch, void, __fastcall, (D2GSPacketSrv97 *pPacket), 0x143E0)
//D2FUNC(D2CLIENT, SCMD_HandleSrv8F, void, __fastcall, (D2GSPacketSrv8F *pPacket), 0x14410)
//D2FUNC(D2CLIENT, SCMD_HandleSrv9C_ItemDropPickup, void, __fastcall, (D2GSPacketSrv9C *pPacket), 0x14420)
//D2FUNC(D2CLIENT, SCMD_HandleSrv9D_ItemEquip, void, __fastcall, (D2GSPacketSrv9D *pPacket), 0x14590)
//D2FUNC(D2CLIENT, SCMD_HandleSrvA6, void, __fastcall, (D2GSPacketSrvA6 *pPacket), 0x146E0)
//D2FUNC(D2CLIENT, SCMD_HandleSrvA7, void, __fastcall, (D2GSPacketSrvA7 *pPacket), 0x14700)
//D2FUNC(D2CLIENT, SCMD_HandleSrvA8, void, __fastcall, (D2GSPacketSrvA8 *pPacket), 0x14750)
//D2FUNC(D2CLIENT, SCMD_HandleSrvA9, void, __fastcall, (D2GSPacketSrvA9 *pPacket), 0x14900)
//D2FUNC(D2CLIENT, SCMD_HandleSrvAA, void, __fastcall, (D2GSPacketSrvAA *pPacket), 0x14950)
//D2FUNC(D2CLIENT, SCMD_HandleSrvAB, void, __fastcall, (D2GSPacketSrvAB *pPacket), 0x14B40)
//D2FUNC(D2CLIENT, SCMD_HandleSrvAC, void, __fastcall, (D2GSPacketSrvAC *pPacket), 0x14BD0)
//D2FUNC(D2CLIENT, SCMD_HandleSrvAD_ServerMemoryProbe, void, __fastcall, (D2GSPacketSrvAD *pPacket), 0x15090)
//D2FUNC(D2CLIENT, SCMD_RouteServerPackets, void, __fastcall, (uint8_t *pBuffer, int32_t nSize), 0x150B0)
//D2FUNC(D2CLIENT, SCMD_AllocUnitPacketQueue, D2PacketListStrc *, __fastcall, (D2UnitStrc *pUnit), 0x154C0)
//D2FUNC(D2CLIENT, SCMD_FreeUnitPacketQueue, void, __fastcall, (D2UnitStrc *pUnit), 0x155C0)
//D2FUNC(D2CLIENT, SCMD_ProcessUnitPacketQueue, void, __fastcall, (D2UnitStrc *pUnit), 0x15610)
//D2FUNC(D2CLIENT, SCROLL_GetUiShiftScreen, D2C_UIpanels, __cdecl, (), 0x15750)
//D2FUNC(D2CLIENT, SCROLL_SetWindowMode, void, __fastcall, (D2C_UIpanels nPanel), 0x15760)
//D2FUNC(D2CLIENT, SCROLL_GetViewportOriginXRaw, int32_t, __cdecl, (), 0x15870)
//D2FUNC(D2CLIENT, SCROLL_GetViewportOriginYRaw, int32_t, __cdecl, (), 0x15880)
//D2FUNC(D2CLIENT, SCROLL_GetViewportOriginX, int32_t, __cdecl, (), 0x15890)
//D2FUNC(D2CLIENT, SCROLL_GetViewportOriginY, int32_t, __cdecl, (), 0x158A0)
//D2FUNC(D2CLIENT, SCROLL_ScreenToGameCoords, void, __fastcall, (int32_t *pnX, int32_t *pnY), 0x158B0)
//D2FUNC(D2CLIENT, SCROLL_UpdateViewportOrigin, void, __cdecl, (), 0x15960)
//D2FUNC(D2CLIENT, SELECT_GetSelectedUnit, D2UnitStrc *, __cdecl, (), 0x15A20)
//D2FUNC(D2CLIENT, SELECT_GetToHitRefMonster, int32_t, __cdecl, (), 0x15A70)
//D2FUNC(D2CLIENT, SELECT_GetToBeHitRefMonster, int32_t, __cdecl, (), 0x15A80)
//D2FUNC(D2CLIENT, SELECT_SetSelectedUnit, void, __fastcall, (D2UnitStrc *pUnit), 0x15A90)
//D2FUNC(D2CLIENT, SELECT_ApplySelectedUnit, void, __fastcall, (D2UnitStrc *pUnit), 0x15AE0)
//D2FUNC(D2CLIENT, SELECT_ClearSelectedUnit, int32_t, __cdecl, (), 0x15D40)
//D2FUNC(D2CLIENT, SELECT_UnlockSelection, void, __cdecl, (), 0x15D50)
//D2FUNC(D2CLIENT, SELECT_UpdateSelectedUnit, void, __cdecl, (), 0x15D60)
//D2FUNC(D2CLIENT, SELECT_RosterPetsSetHirelingDisplayState, void, __fastcall, (D2UnitStrc *pOwner, D2UnitStrc *pPet, int32_t nState), 0x16430)
//D2FUNC(D2CLIENT, SELECT_SetClickRegion, int32_t, __fastcall, (const int32_t *pRect), 0x16450)
//D2FUNC(D2CLIENT, SELECT_SkillsResolveClientTarget, D2UnitStrc *, __fastcall, (D2SkillStrc *pSkill, D2UnitStrc *pTargetUnit, int32_t *pnX, int32_t *pnY, uint8_t nCastFlags, int32_t bAllowSearch), 0x16490)
//D2FUNC(D2CLIENT, SELECT_SkillsIsIronGolemTarget, BOOL, __fastcall, (D2UnitStrc *pUnit, D2UnitFindArgStrc *pUnitFindArg), 0x16860)
//D2FUNC(D2CLIENT, SELECT_SkillsFindEnemyInFrontOfUnit, D2UnitStrc *, __fastcall, (D2UnitStrc *pUnit, D2SkillsTxt *pSkillsTxt), 0x16880)
//D2FUNC(D2CLIENT, SELECT_UnitFindIsMonster, int32_t, __fastcall, (D2UnitStrc *pUnit, void *pContext), 0x16A30)
//D2FUNC(D2CLIENT, SELECT_ResetRefMonsters, void, __cdecl, (), 0x16A50)
//D2FUNC(D2CLIENT, UNITS_GetXPosition, int32_t, __fastcall, (D2UnitStrc *pUnit), 0x16A60)
//D2FUNC(D2CLIENT, UNITS_GetYPosition, int32_t, __fastcall, (D2UnitStrc *pUnit), 0x16A90)
//D2FUNC(D2CLIENT, VIEW_AllocGouraudTables, void, __cdecl, (), 0x16AC0)
//D2FUNC(D2CLIENT, VIEW_FreeGouraudTables, void, __cdecl, (), 0x16EB0)
//D2FUNC(D2CLIENT, VIEW_Alloc, D2ClientViewStrc *, __cdecl, (), 0x16FF0)
//D2FUNC(D2CLIENT, VIEW_Free, void, __fastcall, (D2ClientViewStrc *pView), 0x170D0)
//D2FUNC(D2CLIENT, VIEW_MarkDirty, void, __fastcall, (D2ClientViewStrc *pView), 0x170F0)
//D2FUNC(D2CLIENT, VIEW_SetRect, void, __fastcall, (D2ClientViewStrc *pView, int32_t xLeft, int32_t yTop, int32_t xRight, int32_t yBottom), 0x17100)
//D2FUNC(D2CLIENT, VIEW_GetRect, void, __fastcall, (D2ClientViewStrc *pView, int32_t *pnLeft, int32_t *pnTop, int32_t *pnRight, int32_t *pnBottom), 0x17180)
//D2FUNC(D2CLIENT, VIEW_SetOrigin, void, __fastcall, (D2ClientViewStrc *pView, int32_t nX, int32_t nY), 0x171B0)
//D2FUNC(D2CLIENT, VIEW_GetOrigin, void, __fastcall, (D2ClientViewStrc *pView, int32_t *pnX, int32_t *pnY), 0x171E0)
//D2FUNC(D2CLIENT, VIEW_Draw, void, __fastcall, (D2ClientViewStrc *pView, int32_t bSkipUnits), 0x17200)
//D2FUNC(D2CLIENT, VIEWEFFECTS_EnvUpdateViewAndLevelEffects, void, __fastcall, (D2ClientViewStrc *pView), 0x17260)
//D2FUNC(D2CLIENT, VIEWEFFECTS_EnvRespawnArcaneStar, void, __fastcall, (int32_t nIndex, int32_t bAtRightEdge), 0x17590)
//D2FUNC(D2CLIENT, VIEWEFFECTS_EnvDrawParallax, void, __fastcall, (D2UnitStrc *pUnit), 0x176F0)
//D2FUNC(D2CLIENT, VIEWEFFECTS_UpdateScreenShake, void, __fastcall, (int32_t *pViewOffsetX, int32_t *pViewOffsetY), 0x17C50)
//D2FUNC(D2CLIENT, VIEWEFFECTS_UpdateScreenShakeSound, void, __fastcall, (int32_t nMagnitude), 0x17DC0)
//D2FUNC(D2CLIENT, VIEWEFFECTS_StartScreenShake, void, __fastcall, (int32_t nMagnitude, int32_t nRampUpMs, int32_t nHoldMs, int32_t nRampDownMs), 0x17E70)
//D2FUNC(D2CLIENT, VIEWEFFECTS_GetScreenShakeOffset, int32_t, __fastcall, (int32_t *pOffsetX, int32_t *pOffsetY), 0x17EB0)
//D2FUNC(D2CLIENT, VIEWEFFECTS_ResetScreenShake, void, __cdecl, (), 0x17EE0)
//D2FUNC(D2CLIENT, WALL2_WallInitSquares, void, __fastcall, (D2ClientViewStrc *pView), 0x17F00)
//D2FUNC(D2CLIENT, WALL2_WallFreeSquares, void, __fastcall, (D2ClientViewStrc *pView), 0x17F20)
//D2FUNC(D2CLIENT, WALL2_WallBuildSquareGrid, void, __fastcall, (D2ClientViewStrc *pView, int32_t bSkipUnits), 0x17F60)
//D2FUNC(D2CLIENT, WALL2_WallAddRoomToSquareGrid, int32_t, __stdcall, (D2ActiveRoomStrc *pRoom, D2ClientWallRoomArgsStrc *pArgs), 0x18290)
//D2FUNC(D2CLIENT, WALL2_WallIsPlayerBehindTile, BOOL, __fastcall, (D2ActiveRoomStrc *pRoom, D2DrlgTileDataStrc *pTile), 0x18B50)
//D2FUNC(D2CLIENT, WALL2_WallAppendTileToShadowList, void, __fastcall, (uint32_t *pContext, D2ClientWallSquareStrc *pSquare, D2DrlgTileDataStrc *pTile, D2ActiveRoomStrc *pRoom), 0x18C60)
//D2FUNC(D2CLIENT, WALL2_WallAppendUnitToShadowList, void, __fastcall, (uint32_t *pContext, D2ClientWallSquareStrc *pSquare, D2UnitStrc *pUnit), 0x18D20)
//D2FUNC(D2CLIENT, QUESTS_DispatchQuestPacket, void, __fastcall, (D2GSPacketSrv50 *pCmd), 0x18E00)
//D2FUNC(D2CLIENT, QUESTS_StoreSrv5EPayload, void, __fastcall, (D2GSPacketSrv5E *pPacket), 0x18EC0)
//D2FUNC(D2CLIENT, QUESTS_AreStatesReceived, int32_t, __stdcall, (int32_t nUnused), -10001)
//D2FUNC(D2CLIENT, QUESTS_GetQuestState, int32_t, __stdcall, (int32_t nUnused, D2Quests nQuest), -10002)
//D2FUNC(D2CLIENT, QUESTS_PlayCinematicFromPacket, void, __fastcall, (D2GSPacketSrv61 *pPacket), 0x18F50)
//D2FUNC(D2CLIENT, QUESTSFX_FireTrigger, void, __fastcall, (D2GSPacketSrv89 *pPacket), 0x18F60)
//D2FUNC(D2CLIENT, QUESTSFX_StartScreenFade, void, __fastcall, (int32_t nFadeOut, int32_t nDarkHold, int32_t nFadeIn, int32_t nLevelId, int32_t bForce), 0x18F70)
//D2FUNC(D2CLIENT, QUESTSFX_TriggerCairnStones, void, __cdecl, (), 0x19000)
//D2FUNC(D2CLIENT, QUESTSFX_TriggerDenOfEvil, void, __cdecl, (), 0x19140)
//D2FUNC(D2CLIENT, QUESTSFX_TriggerHoradricStaff, void, __cdecl, (), 0x19150)
//D2FUNC(D2CLIENT, QUESTSFX_TriggerTaintedSun, void, __cdecl, (), 0x19280)
//D2FUNC(D2CLIENT, QUESTSFX_TriggerDiabloAppears, void, __cdecl, (), 0x19330)
//D2FUNC(D2CLIENT, QUESTSFX_TriggerHellforge, void, __cdecl, (), 0x193E0)
//D2FUNC(D2CLIENT, QUESTSFX_ArmTriggerTimer, void, __cdecl, (), 0x194D0)
//D2FUNC(D2CLIENT, QUESTSFX_OnOverseerDeath, void, __fastcall, (D2UnitStrc *pUnit), 0x194F0)
//D2FUNC(D2CLIENT, QUESTSFX_TriggerAnya, void, __cdecl, (), 0x19570)
//D2FUNC(D2CLIENT, QUESTSFX_TriggerNihlathak, void, __cdecl, (), 0x19620)
//D2FUNC(D2CLIENT, QUESTSFX_TriggerWorldstoneShake, void, __cdecl, (), 0x196F0)
//D2FUNC(D2CLIENT, QUESTSFX_OnAncientDeath, void, __fastcall, (D2UnitStrc *pUnit), 0x19700)
//D2FUNC(D2CLIENT, QUESTSFX_FireTriggerById, void, __fastcall, (int32_t eTrigger), 0x19800)
//D2FUNC(D2CLIENT, QUESTSFX_OnBloodRavenDeath, void, __fastcall, (D2UnitStrc *pUnit), 0x19870)
//D2FUNC(D2CLIENT, QUESTSFX_SpawnTowerChestEffect, void, __fastcall, (D2UnitStrc *pUnit), 0x19900)
//D2FUNC(D2CLIENT, QUESTSFX_OnCompellingOrbDeath, void, __fastcall, (D2UnitStrc *pUnit), 0x19A40)
//D2FUNC(D2CLIENT, QUESTSFX_OnMephistoDeath, void, __fastcall, (D2UnitStrc *pUnit), 0x19B90)
//D2FUNC(D2CLIENT, QUESTSFX_OnAndarielDeath, void, __fastcall, (D2UnitStrc *pUnit), 0x19C40)
//D2FUNC(D2CLIENT, QUESTSFX_OnRadamentDeath, void, __fastcall, (D2UnitStrc *pUnit), 0x19C80)
//D2FUNC(D2CLIENT, QUESTSFX_OnMaggotQueenDeath, void, __fastcall, (D2UnitStrc *pUnit), 0x19D00)
//D2FUNC(D2CLIENT, QUESTSFX_OnDurielDeath, void, __fastcall, (D2UnitStrc *pUnit), 0x19D80)
//D2FUNC(D2CLIENT, QUESTSFX_OnIzualDeath, void, __fastcall, (D2UnitStrc *pUnit), 0x19E00)
//D2FUNC(D2CLIENT, QUESTSFX_SetWatchedUnit, void, __fastcall, (D2UnitStrc *pUnit), 0x19E60)
//D2FUNC(D2CLIENT, QUESTSFX_OnBaalDeath, void, __cdecl, (), 0x19E80)
//D2FUNC(D2CLIENT, QUESTSFX_OnTyrael3Created, void, __fastcall, (D2UnitStrc *pMonster), 0x19F10)
//D2FUNC(D2CLIENT, QUESTSFX_GetQuestLightOverride, int32_t, __fastcall, (D2ActiveRoomStrc *pRoom, uint8_t *pnRed, uint8_t *pnGreen, uint8_t *pnBlue), 0x19FE0)
//D2FUNC(D2CLIENT, QUESTSFX_OnRoomActivated, void, __thiscall, (D2ActiveRoomStrc *pRoom), 0x1A0D0)
//D2FUNC(D2CLIENT, QUESTSFX_SpawnAmbientMissiles, void, __thiscall, (D2ActiveRoomStrc *pRoom), 0x1A120)
//D2FUNC(D2CLIENT, QUESTSFX_TickScreenFade, void, __cdecl, (), 0x1A270)
//D2FUNC(D2CLIENT, QUESTSFX_SpawnWorldstoneShake, void, __fastcall, (D2ActiveRoomStrc *pRoom), 0x1A440)
//D2FUNC(D2CLIENT, QUESTSFX_DrawGameEndCountdown, void, __cdecl, (), 0x1A4A0)
//D2FUNC(D2CLIENT, QUESTSFX_Reset, void, __cdecl, (), 0x1A580)
//D2FUNC(D2CLIENT, QUESTSFX_ArmGameEndDeadline, void, __fastcall, (int32_t nDelaySeconds), 0x1A5C0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc85, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1A5E0)
//D2FUNC(D2CLIENT, SKILLS_ClientCastAtTarget, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, D2UnitStrc *pTarget, int32_t nX, int32_t nY, uint16_t nParam), 0x1A610)
//D2FUNC(D2CLIENT, SKILLS_ClientCastAtUnit, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, D2UnitStrc *pTarget, uint16_t nParam), 0x1A7F0)
//D2FUNC(D2CLIENT, SKILLS_ClientCastAtPosition, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nX, int32_t nY, uint16_t nParam), 0x1A820)
//D2FUNC(D2CLIENT, SKILLS_GetPosInFrontOfUnit, void, __fastcall, (D2UnitStrc *pUnit, int32_t *pnX, int32_t *pnY), 0x1A860)
//D2FUNC(D2CLIENT, SKILLS_GetPathTargetPos, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t *pnX, int32_t *pnY), 0x1A950)
//D2FUNC(D2CLIENT, SKILLS_GrowVectorToMinLength, void, __fastcall, (int32_t *pX, int32_t *pY), 0x1AB20)
//D2FUNC(D2CLIENT, SKILLS_ShrinkPerpendicularVector, void, __fastcall, (int32_t *pX, int32_t *pY), 0x1AB70)
//D2FUNC(D2CLIENT, SKILLS_MissileClientSpawnAimedAtTargetUnit, D2UnitStrc *, __fastcall, (D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1ABC0)
//D2FUNC(D2CLIENT, SKILLS_MissileClientSpawnOnTargetUnit, D2UnitStrc *, __fastcall, (D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1AD50)
//D2FUNC(D2CLIENT, SKILLS_HasAmmoForShot, BOOL, __fastcall, (D2UnitStrc *pUnit), 0x1AEB0)
//D2FUNC(D2CLIENT, SKILLS_SpawnCltMissileStraight, D2UnitStrc *, __fastcall, (D2C_MissilesHcIdx nMissileId, D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nSpawnOffsetX, int32_t nSpawnOffsetY, int32_t nTargetX, int32_t nTargetY), 0x1AFC0)
//D2FUNC(D2CLIENT, SKILLS_UnitMatchesTargetMask, BOOL, __fastcall, (D2UnitStrc *pUnit, D2UnitStrc *pTarget, D2C_AuraFilters nMask), 0x1B220)
//D2FUNC(D2CLIENT, SKILLS_ForEachNearbyTarget, void, __fastcall, (D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nRadius, D2C_AuraFilters nMask, int32_t (__fastcall *pfnVisit)(D2ClientNearbyTargetCtxStrc *pCtx, D2UnitStrc *pTarget), void *pUserData, BOOL bSkipNoAuraMonsters), 0x1B590)
//D2FUNC(D2CLIENT, SKILLS_CountNearbyTargets, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nRadius, D2C_AuraFilters nMask), 0x1B840)
//D2FUNC(D2CLIENT, SKILLS_VisitCountUnits, int32_t, __fastcall, (D2ClientNearbyTargetCtxStrc *pCtx, D2UnitStrc *pTarget), 0x1B880)
//D2FUNC(D2CLIENT, SKILLS_FindNextNearbyTargetById, D2UnitStrc *, __fastcall, (D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nRadius, D2C_AuraFilters nMask, uint32_t dwAfterUnitId, int32_t *pnCount), 0x1B890)
//D2FUNC(D2CLIENT, SKILLS_NextTargetByIdVisitor, int32_t, __fastcall, (D2ClientNearbyTargetCtxStrc *pCtx, D2UnitStrc *pTarget), 0x1B910)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc1, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1B960)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc2, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1B9B0)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc3, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1B9E0)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc4, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1BA80)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc1, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1BA90)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc2, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1BC70)
//D2FUNC(D2CLIENT, SKILLS_RunCltStFuncSwapped, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, D2UnitGUID nOwnerGUID), 0x1BEB0)
//D2FUNC(D2CLIENT, SKILLS_RunCltStFunc, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, D2UnitGUID nOwnerGUID, int32_t nSkillLevel), 0x1BED0)
//D2FUNC(D2CLIENT, SKILLS_RunClientDoFunc, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t bUseTarget), 0x1C6B0)
//D2FUNC(D2CLIENT, SKILLS_RunUsedSkillClientDoFunc, void, __fastcall, (D2UnitStrc *pUnit), 0x1CA30)
//D2FUNC(D2CLIENT, SKILLS_ApplyClientSkillEffectNonProgressive, void, __fastcall, (D2UnitStrc *pUnit, D2UnitStrc *pTarget, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nX, int32_t nY, uint8_t nSeed), 0x1CA70)
//D2FUNC(D2CLIENT, SKILLS_ApplyClientSkillEffect, void, __fastcall, (D2UnitStrc *pUnit, D2UnitStrc *pTarget, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nX, int32_t nY, int32_t nCharges), 0x1CD70)
//D2FUNC(D2CLIENT, SKILLS_CltStartSkillOnUnit, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t *pParams), 0x1D2A0)
//D2FUNC(D2CLIENT, SKILLS_CltAlternateDualWieldHand, void, __fastcall, (D2UnitStrc *pUnit, int32_t *pParams), 0x1D330)
//D2FUNC(D2CLIENT, SKILLS_CltStartSkillAtCoords, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t *pParams), 0x1D400)
//D2FUNC(D2CLIENT, SKILLS_SpawnMissileRingFromUnit, D2UnitStrc *, __fastcall, (D2UnitStrc *pOwner, D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId, D2C_MissilesHcIdx nAltMissileId, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nVelocity), 0x1D4D0)
//D2FUNC(D2CLIENT, SKILLS_SpawnMissileRing, D2UnitStrc *, __fastcall, (D2MissileStrc *pParams, D2MissileStrc *pAltParams), 0x1D5D0)
//D2FUNC(D2CLIENT, SKILLS_SpawnMissileArray, D2UnitStrc *, __fastcall, (int32_t nMissiles, D2MissileStrc *pParams), 0x1D690)
//D2FUNC(D2CLIENT, SKILLS_StatesRunActiveStateFuncs, void, __fastcall, (D2UnitStrc *pUnit), 0x1D780)
//D2FUNC(D2CLIENT, SKILLS_SpawnScatteredMissiles, void, __fastcall, (D2UnitStrc *pUnit, D2MissileStrc *pParams, int32_t nJitter, int32_t nCount), 0x1D8A0)
//D2FUNC(D2CLIENT, SKILLS_SetUnitDirection, void, __fastcall, (D2UnitStrc *pUnit, int32_t nDirection), 0x1DA50)
//D2FUNC(D2CLIENT, SKILLS_DispatchUnitAction22, int32_t, __fastcall, (D2UnitStrc *pUnit, D2UnitStrc *pTarget), 0x1DA60)
//D2FUNC(D2CLIENT, SKILLS_ReduceFixedFieldByPercent, void, __fastcall, (D2UnitStrc *pUnit, int32_t nPercent), 0x1DAD0)
//D2FUNC(D2CLIENT, SKILLS_SpawnClientEffectScatter, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId, int32_t nCount, int32_t nRadius, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1DB00)
//D2FUNC(D2CLIENT, SKILLS_PostSkillStatWithRemoveCallback, void, __thiscall, (D2UnitStrc *pUnit), 0x1DD20)
//D2FUNC(D2CLIENT, SKILLS_StatRemoveCallback_EndMoveSkill, void, __fastcall, (D2UnitStrc *pUnit, int32_t nState, D2StatListStrc *pStatList), 0x1DD70)
//D2FUNC(D2CLIENT, SKILLS_AddLevelsToSkillPage, void, __fastcall, (D2UnitStrc *pUnit, int32_t nDelta, int32_t nElement, uint8_t nSkillPage), 0x1DE30)
//D2FUNC(D2CLIENT, SKILLS_CanMissileLandAtAimPoint, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId), 0x1DFF0)
//D2FUNC(D2CLIENT, SKILLS_GetExtendedMeleeRange, int32_t, __thiscall, (D2UnitStrc *pUnit), 0x1E1F0)
//D2FUNC(D2CLIENT, SKILLS_ApplyAttackRateStat, void, __fastcall, (D2UnitStrc *pUnit, int32_t nValue), 0x1E200)
//D2FUNC(D2CLIENT, SKILLS_StatesAddCltEventFunc, D2UnitEventStrc *, __fastcall, (D2UnitStrc *pUnit, int32_t nCltEvent, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nCltEventFunc, int32_t nKind, int32_t nParam), 0x1E260)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc11, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1E2A0)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc12, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1E2B0)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc13, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1E2D0)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc14, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1E490)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc53, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1E610)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc16, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1E760)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc17, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1E7F0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc18, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1EA20)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc19, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1EBC0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc20, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1EDC0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc21, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1F0C0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc22, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1F2A0)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc5, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1F4B0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc3, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1F510)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc6, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1F580)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc4, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1F620)
//D2FUNC(D2CLIENT, SKILLS_GetClientMissileForSkill, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId), 0x1F670)
//D2FUNC(D2CLIENT, SKILLS_EvaluateProgressiveSkillCalc, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1F730)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc5, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1F850)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc7, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1F910)
//D2FUNC(D2CLIENT, SKILLS_StartChannelState, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1F970)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc8, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1FAC0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc6, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1FAD0)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc9, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1FC50)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc7, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1FCB0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc8, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1FD40)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc10, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1FE60)
//D2FUNC(D2CLIENT, SKILLS_ScatterClientMissilesInDisc, void, __fastcall, (D2UnitStrc *pUnit, int32_t nSeed, D2C_MissilesHcIdx nMissileId, int32_t nCenterX, int32_t nCenterY, int32_t nRadius, D2C_Skills nSkillId, int32_t nSkillLevel), 0x1FE90)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc9, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x200B0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc10, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x201F0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc11, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x20280)
//D2FUNC(D2CLIENT, SKILLS_MissileInitClientNovaBolt, void, __fastcall, (D2UnitStrc *pMissile, int32_t nLowSeed), 0x20480)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc83, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x204E0)
//D2FUNC(D2CLIENT, SKILLS_MissileBuildWaveringPath, void, __fastcall, (D2UnitStrc *pMissile, int32_t nSeed), 0x206E0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc12, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x208E0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc13, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x209F0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc14, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x20AE0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc15, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x20B80)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc26, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x20D30)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc38, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x20D50)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc27, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x20D90)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc39, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x20DF0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc42, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x20F40)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc28, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x21000)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc40, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x21020)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc41, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x21070)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc29, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x21310)
//D2FUNC(D2CLIENT, SKILLS_ResolveClientTargetPoint, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t *pnX, int32_t *pnY), 0x216F0)
//D2FUNC(D2CLIENT, SKILLS_StartChargeMove, BOOL, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill), 0x21A20)
//D2FUNC(D2CLIENT, SKILLS_StartLeapArc, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill), 0x21B30)
//D2FUNC(D2CLIENT, SKILLS_RestoreCollisionFootprint, void, __fastcall, (D2UnitStrc *pUnit), 0x21C00)
//D2FUNC(D2CLIENT, SKILLS_ApplyDefaultCollision, void, __fastcall, (D2UnitStrc *pUnit, int32_t nX, int32_t nY), 0x21CE0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc43, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x21DC0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFuncLeap, BOOL, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, BOOL bShowDustOverlay), 0x21E40)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc30, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x22180)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc44, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x222F0)
//D2FUNC(D2CLIENT, SKILLS_DrawSpinCopy, void, __fastcall, (D2UnitStrc *pUnit, int32_t nDrawParam, BOOL bFirstThird), 0x223F0)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc31, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x224C0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc45, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x22700)
//D2FUNC(D2CLIENT, SKILLS_StopWhirlwindSound, void, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId), 0x22800)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc45, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x22840)
//D2FUNC(D2CLIENT, SKILLS_SpawnSeededMissileVolley, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t nCount, D2C_MissilesHcIdx nMissileId, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nAnimSpeed, int32_t bFirstAimedAtTargetUnit, int32_t bRandomFacing), 0x22900)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc67, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x22A80)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc68, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x22B30)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc69, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x22BE0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc90, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x22C90)
//D2FUNC(D2CLIENT, SKILLS_StatesSpawnHurricaneCltMissile, void, __fastcall, (D2UnitStrc *pUnit, D2SkillsTxt *pSkillsTxt, D2C_Skills nSkillId, int32_t nSkillLevel), 0x22D90)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc92, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x23010)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc91, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x23200)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc70, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x23310)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc71, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x23480)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc72, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x237C0)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc46, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x238E0)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc47, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x23970)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc48, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x23B00)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc73, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x23BC0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc74, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x23C90)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc49, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x23D50)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc75, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x23F30)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc76, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x24070)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc52, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x243B0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc77, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x24450)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc50, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x24540)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc78, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x24600)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc51, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x24710)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc79, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x247C0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc88, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x24870)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc89, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x24890)
//D2FUNC(D2CLIENT, SKILLS_StatesMatchVineBeastNeutralMissile, int32_t, __fastcall, (D2UnitStrc *pUnit, void *pUnused), 0x24BC0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc93, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x24BE0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc80, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x24CA0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc81, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x24EC0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc82, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x24FE0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc46, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x251E0)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc32, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x251F0)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc33, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x25440)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc47, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x254D0)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc34, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x25510)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc48, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x255C0)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc35, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x257D0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc49, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x25860)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc36, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x25900)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc50, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x25A50)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc51, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x25B50)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc37, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x25DC0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc53, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x25DE0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc54, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x25ED0)
//D2FUNC(D2CLIENT, SKILLS_MissileCltSpawnWithSkillRange, D2UnitStrc *, __fastcall, (D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId, const int32_t *pVelJitter, int32_t nTargetX, int32_t nTargetY, D2C_Skills nSkillId, int32_t nSkillLevel), 0x25FF0)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc38, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x26240)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc39, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x26260)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc55, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x26290)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc40, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x26460)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc56, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x264E0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc57, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x266A0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc58, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x26960)
//D2FUNC(D2CLIENT, SKILLS_MissileStartSeededPathOrStall, void, __fastcall, (D2UnitStrc *pMissile, int32_t nSeed), 0x26AB0)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc41, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x26B50)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc59, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x26B70)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc60, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x26DB0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc61, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x27030)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc42, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x271E0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc62, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x27330)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc43, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x27400)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc63, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x27460)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc64, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x275C0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc65, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x27650)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc44, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x27770)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc94, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x279F0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc95, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x27B10)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc66, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x27C30)
//D2FUNC(D2CLIENT, SKILLS_LaySpiderGooTrail, D2UnitStrc *, __fastcall, (D2UnitStrc *pUnit), 0x27D60)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc18, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x27F90)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc30, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x27FF0)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc19, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x28150)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc20, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x28370)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc31, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x28440)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc21, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x28530)
//D2FUNC(D2CLIENT, SKILLS_SpawnCorpseExplosionEffect, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nX, int32_t nY, D2C_MonsterIds nCorpseClassId), 0x285F0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc32, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x288F0)
//D2FUNC(D2CLIENT, SKILLS_SpawnPoisonExplosionEffect, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nX, int32_t nY, D2C_MonsterIds nCorpseClassId), 0x28940)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc33, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x28AE0)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc22, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x28B30)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc23, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x28C80)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc24, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x28D00)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc34, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x28EE0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc35, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x290E0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc36, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x291F0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc86, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x29350)
//D2FUNC(D2CLIENT, SKILLS_DrawMotionTrail, void, __fastcall, (D2UnitStrc *pUnit, int32_t nDrawParam, int32_t bBehindUnit), 0x29620)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc25, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x29820)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc37, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x29B50)
//D2FUNC(D2CLIENT, SKILLS_EndUninterruptableSkill, void, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId), 0x29E50)
//D2FUNC(D2CLIENT, SKILLS_MissileStartSeededPath, void, __fastcall, (D2UnitStrc *pMissile, int32_t nSeed), 0x29E80)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc23, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x29EE0)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc15, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x2A050)
//D2FUNC(D2CLIENT, SKILLS_SetMissileAnimSpeedDivisor, void, __fastcall, (D2UnitStrc *pUnit, int32_t nDivisor), 0x2A170)
//D2FUNC(D2CLIENT, SKILLS_SpawnInfernoStream, D2UnitStrc *, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x2A1D0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc24, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x2A3B0)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc16, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x2A3D0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc25, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x2A570)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc26, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x2A630)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc27, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x2A900)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc96, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x2A9C0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc28, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x2AC10)
//D2FUNC(D2CLIENT, SKILLS_CltStFunc17, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x2ACA0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc29, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x2ADF0)
//D2FUNC(D2CLIENT, SKILLS_LayBlazeTrail, void, __fastcall, (D2UnitStrc *pUnit), 0x2AE70)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc84, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x2B070)
//D2FUNC(D2CLIENT, SKILLS_StatesCltEventFunc1, int32_t, __fastcall, (int32_t nEvent, D2UnitStrc *pUnit, D2UnitStrc *pMissile, int32_t nUnused, D2C_Skills nSkillId, int32_t nSkillLevel), 0x2B1F0)
//D2FUNC(D2CLIENT, SKILLS_CltDoFunc87, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel), 0x2B3C0)
//D2FUNC(D2CLIENT, UI_SendWmCloseAndReset, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x2B670)
//D2FUNC(D2CLIENT, AUTOMAP_OnArrowDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x2B6A0)
//D2FUNC(D2CLIENT, AUTOMAP_OnArrowUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x2B6B0)
//D2FUNC(D2CLIENT, AUTOMAP_OnArrowLeft, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x2B6C0)
//D2FUNC(D2CLIENT, AUTOMAP_OnArrowRight, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x2B6D0)
//D2FUNC(D2CLIENT, AUTOMAP_RecomputeCenterOffsets, void, __fastcall, (int32_t bForce), 0x2B6E0)
//D2FUNC(D2CLIENT, AUTOMAP_UpdateClipRect, void, __cdecl, (), 0x2B760)
//D2FUNC(D2CLIENT, AUTOMAP_GetFade, int32_t, __cdecl, (), 0x2B8F0)
//D2FUNC(D2CLIENT, AUTOMAP_SetFade, int32_t, __fastcall, (int32_t nFade), 0x2B920)
//D2FUNC(D2CLIENT, AUTOMAP_GetCenters, int32_t, __cdecl, (), 0x2B970)
//D2FUNC(D2CLIENT, AUTOMAP_SetCentersAndSave, int32_t, __fastcall, (int32_t bCenters), 0x2B980)
//D2FUNC(D2CLIENT, AUTOMAP_GetLeft, int32_t, __cdecl, (), 0x2B9A0)
//D2FUNC(D2CLIENT, AUTOMAP_ApplyAutomapLeftSetting, int32_t, __fastcall, (int32_t bAutomapLeft), 0x2B9B0)
//D2FUNC(D2CLIENT, AUTOMAP_GetShowParty, int32_t, __cdecl, (), 0x2B9E0)
//D2FUNC(D2CLIENT, AUTOMAP_SetShowPartyAndSave, int32_t, __fastcall, (int32_t bShow), 0x2B9F0)
//D2FUNC(D2CLIENT, AUTOMAP_GetPartyNames, int32_t, __cdecl, (), 0x2BA10)
//D2FUNC(D2CLIENT, AUTOMAP_SetPartyNamesAndSave, int32_t, __fastcall, (int32_t bShow), 0x2BA20)
//D2FUNC(D2CLIENT, AUTOMAP_AllocCell, D2AutomapCellStrc *, __cdecl, (), 0x2BA40)
//D2FUNC(D2CLIENT, AUTOMAP_Tick, void, __cdecl, (), 0x2BAF0)
//D2FUNC(D2CLIENT, AUTOMAP_RebuildDrawLists, void, __cdecl, (), 0x2BCD0)
//D2FUNC(D2CLIENT, AUTOMAP_WriteExploredLayer, void, __fastcall, (D2AutomapDataStrc *pData), 0x2C080)
//D2FUNC(D2CLIENT, AUTOMAP_SaveExploredMap, int32_t, __stdcall, (), 0x2C2B0)
//D2FUNC(D2CLIENT, AUTOMAP_MeasureCellTree, void, __fastcall, (D2AutomapCellStrc *pNode, int32_t *pnSize), 0x2C580)
//D2FUNC(D2CLIENT, AUTOMAP_WriteCellTree, void, __fastcall, (D2AutomapCellStrc *pNode), 0x2C5B0)
//D2FUNC(D2CLIENT, AUTOMAP_LoadCelData, void, __stdcall, (), 0x2C610)
//D2FUNC(D2CLIENT, AUTOMAP_RebalanceTree, int32_t, __fastcall, (D2AutomapCellStrc *ptNew, D2AutomapCellStrc **ppRoot), 0x2CD50)
//D2FUNC(D2CLIENT, AUTOMAP_RotateLeft, D2AutomapCellStrc *, __fastcall, (D2AutomapCellStrc **pptRoot), 0x2D0C0)
//D2FUNC(D2CLIENT, AUTOMAP_RotateRight, D2AutomapCellStrc *, __fastcall, (D2AutomapCellStrc **pptRoot), 0x2D120)
//D2FUNC(D2CLIENT, AUTOMAP_AddRoom, void, __fastcall, (D2ActiveRoomStrc *pRoom, int32_t bRevealAll, D2AutomapLayerStrc *pLayer), 0x2D180)
//D2FUNC(D2CLIENT, AUTOMAP_AddTile, void, __fastcall, (D2DrlgTileDataStrc *pTile, D2DrlgRoomStrc *pRoom, D2AutomapCellStrc **ppTreeRoot), 0x2D3C0)
//D2FUNC(D2CLIENT, AUTOMAP_AddUnit, void, __fastcall, (D2UnitStrc *pUnit, int32_t nCelNum, D2AutomapCellStrc **ppTreeRoot), 0x2D560)
//D2FUNC(D2CLIENT, AUTOMAP_DelayReveal, void, __cdecl, (), 0x2D650)
//D2FUNC(D2CLIENT, AUTOMAP_RebuildAutomapLayer, void, __stdcall, (D2ActiveRoomStrc *), 0x2D660)
//D2FUNC(D2CLIENT, AUTOMAP_AddTown, void, __stdcall, (int32_t nLevelId, int32_t nPickedFile, int32_t nCenterX, int32_t nCenterY), 0x2D840)
//D2FUNC(D2CLIENT, AUTOMAP_Draw, void, __cdecl, (), 0x2DCB0)
//D2FUNC(D2CLIENT, AUTOMAP_DrawCellList, void, __fastcall, (D2AutomapCellStrc *pCellList, D2ClientAutomapDrawCtxStrc *pDrawCtx), 0x2E230)
//D2FUNC(D2CLIENT, AUTOMAP_DrawUnits, void, __cdecl, (), 0x2E510)
//D2FUNC(D2CLIENT, AUTOMAP_DrawUnit, BOOL, __fastcall, (D2UnitStrc *pUnit, uint8_t *pColor), 0x2E8F0)
//D2FUNC(D2CLIENT, AUTOMAP_DrawUnitName, void, __fastcall, (const char *pszName, int32_t nX, int32_t nY, uint8_t nTextColor), 0x2EC40)
//D2FUNC(D2CLIENT, AUTOMAP_DrawSymbol, void, __fastcall, (int32_t nX, int32_t nY, uint8_t nColor), 0x2ECF0)
//D2FUNC(D2CLIENT, AUTOMAP_DrawPartyMember, void, __fastcall, (D2RosterUnitStrc *pRosterEntry), 0x2ED50)
//D2FUNC(D2CLIENT, AUTOMAP_DrawGameInfo, void, __cdecl, (), 0x2EF10)
//D2FUNC(D2CLIENT, AUTOMAP_DrawExpansionLabel, void, __cdecl, (), 0x2F060)
//D2FUNC(D2CLIENT, AUTOMAP_DrawLevelName, void, __cdecl, (), 0x2F0D0)
//D2FUNC(D2CLIENT, AUTOMAP_DrawVersionLabel, void, __cdecl, (), 0x2F120)
//D2FUNC(D2CLIENT, AUTOMAP_DrawHostIP, void, __cdecl, (), 0x2F210)
//D2FUNC(D2CLIENT, AUTOMAP_DrawDifficultyLabel, void, __cdecl, (), 0x2F290)
//D2FUNC(D2CLIENT, AUTOMAP_LoadSettings, int32_t, __cdecl, (), 0x2F380)
//D2FUNC(D2CLIENT, AUTOMAP_LoadIconCels, void, __cdecl, (), 0x2F490)
//D2FUNC(D2CLIENT, AUTOMAP_Shutdown, void, __cdecl, (), 0x2F670)
//D2FUNC(D2CLIENT, AUTOMAP_InitSymbolColors, void, __cdecl, (), 0x2F730)
//D2FUNC(D2CLIENT, AUTOMAP_GetMode, int32_t, __cdecl, (), 0x2F820)
//D2FUNC(D2CLIENT, AUTOMAP_SetMode, void, __fastcall, (int32_t nMode), 0x2F830)
//D2FUNC(D2CLIENT, CHAR_UiButtonConstruct, int32_t *, __thiscall, (D2ClientButtonStrc *pButton, int32_t nX, int32_t nY, int32_t nImageIndex, void *pfnOnClick), 0x2F8D0)
//D2FUNC(D2CLIENT, CHAR_UiButtonVecDelDtor, D2ClientButtonStrc *, __thiscall, (D2ClientButtonStrc *pButton, uint8_t bFreeObject), 0x2F990)
//D2FUNC(D2CLIENT, CHAR_UiButtonDestruct, void, __thiscall, (D2ClientButtonStrc *pButton), 0x2F9C0)
//D2FUNC(D2CLIENT, CHAR_UiButtonDraw, void, __thiscall, (D2ClientButtonStrc *pButton), 0x2F9D0)
//D2FUNC(D2CLIENT, CHAR_UiButtonOnMouseDown, int32_t, __thiscall, (D2ClientButtonStrc *pButton, SMSGHANDLER_PARAMS *pMsg), 0x2FAD0)
//D2FUNC(D2CLIENT, CHAR_UiButtonOnMouseUp, int32_t, __thiscall, (D2ClientButtonStrc *pButton, SMSGHANDLER_PARAMS *pMsg), 0x2FB30)
//D2FUNC(D2CLIENT, CHAR_UiButtonOnChar, int32_t, __thiscall, (D2ClientButtonStrc *pButton, SMSGHANDLER_PARAMS *pMsg), 0x2FB90)
//D2FUNC(D2CLIENT, CHAR_UiButtonProxyDestruct, void, __thiscall, (int32_t *thisptr), 0x2FBC0)
//D2FUNC(D2CLIENT, CHAR_UiButtonProxyVecDelDtor, uint32_t **, __thiscall, (uint32_t **pProxy, uint8_t bFreeObject), 0x2FC20)
//D2FUNC(D2CLIENT, CHAR_UiButtonProxyConstruct, int32_t *, __thiscall, (int32_t *thisptr, int32_t nX, int32_t nY, int32_t nImageIndex, void *pfnOnClick), 0x2FC50)
//D2FUNC(D2CLIENT, CHAR_DrawTextCentered, void, __fastcall, (int32_t nLeft, int32_t nY, int32_t nRight, const Unicode *wszText, int32_t nColor), 0x2FCD0)
//D2FUNC(D2CLIENT, CHAR_TextCutNextLine, int32_t, __fastcall, (struct Unicode *pText, struct Unicode **ppRest), 0x2FD10)
//D2FUNC(D2CLIENT, CHAR_CharPanelDraw, void, __cdecl, (), 0x2FD60)
//D2FUNC(D2CLIENT, CHAR_CharPanelGetWeaponBlockPercent, int32_t, __fastcall, (D2UnitStrc *pUnit), 0x31010)
//D2FUNC(D2CLIENT, CHAR_CharPanelChanceToHitPercent, int32_t, __fastcall, (int32_t nAttackRating), 0x310D0)
//D2FUNC(D2CLIENT, CHAR_StatsOnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x31290)
//D2FUNC(D2CLIENT, CHAR_StatsOnLButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x31470)
//D2FUNC(D2CLIENT, CHAR_MarkGoldDialogClosed, void, __cdecl, (), 0x316C0)
//D2FUNC(D2CLIENT, CHAR_MarkGoldDialogOpen, void, __cdecl, (), 0x316D0)
//D2FUNC(D2CLIENT, CHAR_CharPanelMeasureLabelWidths, void, __cdecl, (), 0x316E0)
//D2FUNC(D2CLIENT, CHAT_OnImeComposition, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x31790)
//D2FUNC(D2CLIENT, CHAT_OnImeStartComposition, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x319B0)
//D2FUNC(D2CLIENT, CHAT_OnImeEndComposition, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x31A20)
//D2FUNC(D2CLIENT, CHAT_OnImeNotify, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x31A70)
//D2FUNC(D2CLIENT, CHAT_ImeIgnoreMessage, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x31D80)
//D2FUNC(D2CLIENT, CHAT_RegisterInputHandlers, void, __cdecl, (), 0x31DA0)
//D2FUNC(D2CLIENT, CHAT_UnregisterHandlers, void, __cdecl, (), 0x32030)
//D2FUNC(D2CLIENT, CHAT_DrawInputLine, void, __cdecl, (), 0x32100)
//D2FUNC(D2CLIENT, CHAT_DrawImeComposition, void, __fastcall, (int32_t nX, int32_t nY), 0x325F0)
//D2FUNC(D2CLIENT, CHAT_OnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x32720)
//D2FUNC(D2CLIENT, CHAT_OnHistoryKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x32760)
//D2FUNC(D2CLIENT, CHAT_HandleInputKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x32890)
//D2FUNC(D2CLIENT, CHAT_ParseCommandPrefix, BOOL, __fastcall, (const char **ppszInput, uint8_t *pnKind, char *pszTargetName), 0x32DB0)
//D2FUNC(D2CLIENT, CHAT_ResetHistory, void, __cdecl, (), 0x32F60)
//D2FUNC(D2CLIENT, CHAT_ClearInputLine, void, __cdecl, (), 0x32F90)
//D2FUNC(D2CLIENT, CHAT_Update, void, __cdecl, (), 0x32FB0)
//D2FUNC(D2CLIENT, CHAT_RunBattleNetCommand, BOOL, __fastcall, (char *pszInput, const char *pszCommand), 0x33350)
//D2FUNC(D2CLIENT, CHAT_HandleWhisperCommand, BOOL, __fastcall, (char *szInput, const char *szCommand, int32_t bWhisperInGame), 0x334A0)
//D2FUNC(D2CLIENT, CMDTBL_ActionCharacterScreen, void, __cdecl, (), 0x33720)
//D2FUNC(D2CLIENT, CMDTBL_ActionInventoryScreen, void, __cdecl, (), 0x33730)
//D2FUNC(D2CLIENT, CMDTBL_ActionPartyScreen, void, __cdecl, (), 0x33750)
//D2FUNC(D2CLIENT, CMDTBL_ActionMessageLog, void, __cdecl, (), 0x33770)
//D2FUNC(D2CLIENT, CMDTBL_ActionQuestLog, void, __cdecl, (), 0x33790)
//D2FUNC(D2CLIENT, CMDTBL_ActionChat, void, __cdecl, (), 0x337B0)
//D2FUNC(D2CLIENT, CMDTBL_ActionHelpScreen, void, __cdecl, (), 0x337D0)
//D2FUNC(D2CLIENT, CMDTBL_ActionAutomap, void, __cdecl, (), 0x33840)
//D2FUNC(D2CLIENT, CMDTBL_ActionCenterAutomap, void, __cdecl, (), 0x33870)
//D2FUNC(D2CLIENT, CMDTBL_ActionFadeAutomap, void, __cdecl, (), 0x33880)
//D2FUNC(D2CLIENT, CMDTBL_ActionPartyOnAutomap, void, __cdecl, (), 0x338A0)
//D2FUNC(D2CLIENT, CMDTBL_ActionNamesOnAutomap, void, __cdecl, (), 0x338C0)
//D2FUNC(D2CLIENT, CMDTBL_ActionToggleAutomapLeft, void, __cdecl, (), 0x338E0)
//D2FUNC(D2CLIENT, CMDTBL_ActionSkillTree, void, __cdecl, (), 0x33900)
//D2FUNC(D2CLIENT, CMDTBL_ActionSkillSelect, void, __cdecl, (), 0x33920)
//D2FUNC(D2CLIENT, CMDTBL_ActionSelectSkillSlot0, void, __cdecl, (), 0x33940)
//D2FUNC(D2CLIENT, CMDTBL_ActionSelectSkillSlot1, void, __cdecl, (), 0x33970)
//D2FUNC(D2CLIENT, CMDTBL_ActionSelectSkillSlot2, void, __cdecl, (), 0x339A0)
//D2FUNC(D2CLIENT, CMDTBL_ActionSelectSkillSlot3, void, __cdecl, (), 0x339D0)
//D2FUNC(D2CLIENT, CMDTBL_ActionSelectSkillSlot4, void, __cdecl, (), 0x33A00)
//D2FUNC(D2CLIENT, CMDTBL_ActionSelectSkillSlot5, void, __cdecl, (), 0x33A30)
//D2FUNC(D2CLIENT, CMDTBL_ActionSelectSkillSlot6, void, __cdecl, (), 0x33A60)
//D2FUNC(D2CLIENT, CMDTBL_ActionSelectSkillSlot7, void, __cdecl, (), 0x33A90)
//D2FUNC(D2CLIENT, CMDTBL_ActionSelectSkillSlot8, void, __cdecl, (), 0x33AC0)
//D2FUNC(D2CLIENT, CMDTBL_ActionSelectSkillSlot9, void, __cdecl, (), 0x33AF0)
//D2FUNC(D2CLIENT, CMDTBL_ActionSelectSkillSlot10, void, __cdecl, (), 0x33B30)
//D2FUNC(D2CLIENT, CMDTBL_ActionSelectSkillSlot11, void, __cdecl, (), 0x33B70)
//D2FUNC(D2CLIENT, CMDTBL_ActionSelectSkillSlot12, void, __cdecl, (), 0x33BB0)
//D2FUNC(D2CLIENT, CMDTBL_ActionSelectSkillSlot13, void, __cdecl, (), 0x33BF0)
//D2FUNC(D2CLIENT, CMDTBL_ActionSelectSkillSlot14, void, __cdecl, (), 0x33C30)
//D2FUNC(D2CLIENT, CMDTBL_ActionSelectSkillSlot15, void, __cdecl, (), 0x33C70)
//D2FUNC(D2CLIENT, CMDTBL_ActionShowBelt, void, __cdecl, (), 0x33CB0)
//D2FUNC(D2CLIENT, CMDTBL_ActionUseBelt1, void, __cdecl, (), 0x33CF0)
//D2FUNC(D2CLIENT, CMDTBL_ActionUseBelt2, void, __cdecl, (), 0x33D00)
//D2FUNC(D2CLIENT, CMDTBL_ActionUseBelt3, void, __cdecl, (), 0x33D10)
//D2FUNC(D2CLIENT, CMDTBL_ActionUseBelt4, void, __cdecl, (), 0x33D20)
//D2FUNC(D2CLIENT, CMDTBL_ActionSayHelp, void, __cdecl, (), 0x33D30)
//D2FUNC(D2CLIENT, CMDTBL_ActionSayFollowMe, void, __cdecl, (), 0x33D40)
//D2FUNC(D2CLIENT, CMDTBL_ActionSayThisIsForYou, void, __cdecl, (), 0x33D50)
//D2FUNC(D2CLIENT, CMDTBL_ActionSayThanks, void, __cdecl, (), 0x33D60)
//D2FUNC(D2CLIENT, CMDTBL_ActionSaySorry, void, __cdecl, (), 0x33D70)
//D2FUNC(D2CLIENT, CMDTBL_ActionSayBye, void, __cdecl, (), 0x33D80)
//D2FUNC(D2CLIENT, CMDTBL_ActionSayNowYouDie, void, __cdecl, (), 0x33D90)
//D2FUNC(D2CLIENT, CMDTBL_ActionSayRetreat, void, __cdecl, (), 0x33DA0)
//D2FUNC(D2CLIENT, CMDTBL_ActionRun, void, __cdecl, (), 0x33DB0)
//D2FUNC(D2CLIENT, CMDTBL_ActionToggleRunWalk, void, __cdecl, (), 0x33E10)
//D2FUNC(D2CLIENT, CMDTBL_ActionStandStill, void, __cdecl, (), 0x33E30)
//D2FUNC(D2CLIENT, CMDTBL_ActionShowItems, void, __cdecl, (), 0x33E40)
//D2FUNC(D2CLIENT, j_GAME_CmdTblActionClearScreen, int32_t, __cdecl, (), 0x33E50)
//D2FUNC(D2CLIENT, CMDTBL_ActionEscMenu, void, __cdecl, (), 0x33E60)
//D2FUNC(D2CLIENT, CMDTBL_ActionSelectPreviousSkill, void, __cdecl, (), 0x33EB0)
//D2FUNC(D2CLIENT, CMDTBL_ActionSelectNextSkill, void, __cdecl, (), 0x33ED0)
//D2FUNC(D2CLIENT, CMDTBL_ActionSwapWeapons, void, __cdecl, (), 0x33EF0)
//D2FUNC(D2CLIENT, CMDTBL_ActionMercenaryInventory, void, __cdecl, (), 0x33F20)
//D2FUNC(D2CLIENT, j_TEXT_ExpireAllChatMessages, void, __cdecl, (), 0x33F60)
//D2FUNC(D2CLIENT, CMDTBL_ActionRunReleased, void, __cdecl, (), 0x33F70)
//D2FUNC(D2CLIENT, CMDTBL_ActionStandStillReleased, void, __cdecl, (), 0x33FB0)
//D2FUNC(D2CLIENT, CMDTBL_ActionShowItemsReleased, void, __cdecl, (), 0x33FC0)
//D2FUNC(D2CLIENT, CMDTBL_CallMouse3Down, void, __cdecl, (), 0x33FE0)
//D2FUNC(D2CLIENT, CMDTBL_CallMouse3Lift, void, __cdecl, (), 0x34020)
//D2FUNC(D2CLIENT, CMDTBL_CommandHandleMouse4Down, void, __cdecl, (), 0x34060)
//D2FUNC(D2CLIENT, CMDTBL_CommandHandleMouse4Lift, void, __cdecl, (), 0x340A0)
//D2FUNC(D2CLIENT, CMDTBL_CommandHandleMouse5Down, void, __cdecl, (), 0x340E0)
//D2FUNC(D2CLIENT, CMDTBL_CommandHandleMouse5Lift, void, __cdecl, (), 0x34120)
//D2FUNC(D2CLIENT, CMDTBL_CommandHandleMouseWheel, void, __fastcall, (int32_t nDelta), 0x34160)
//D2FUNC(D2CLIENT, CMDTBL_LoadDefaultBindings, void, __cdecl, (), 0x341E0)
//D2FUNC(D2CLIENT, CMDTBL_COMMAND_PrepareMouseMappings, int16_t, __cdecl, (), 0x34470)
//D2FUNC(D2CLIENT, CMDTBL_OpenPlayerKeyFile, int32_t, __fastcall, (int32_t dwDesiredAccess, int32_t dwCreationDisposition), 0x34760)
//D2FUNC(D2CLIENT, CMDTBL_SaveBindingsToFiles, void, __cdecl, (), 0x34890)
//D2FUNC(D2CLIENT, CMDTBL_Shutdown, void, __cdecl, (), 0x349D0)
//D2FUNC(D2CLIENT, CMDTBL_CommandLoadDefaultKeyBindings, int16_t, __fastcall, (void *thisptr), 0x349E0)
//D2FUNC(D2CLIENT, CMDTBL_ControlsFindBoundKey, int16_t, __fastcall, (D2HotkeyIndices eCmd, BOOL bPrimary), 0x34B60)
//D2FUNC(D2CLIENT, CMDTBL_IsKeyBindable, BOOL, __fastcall, (uint16_t nVKCode), 0x34B90)
//D2FUNC(D2CLIENT, CMDTBL_BindKey, BOOL, __fastcall, (int32_t eCmd, int32_t bPrimary, uint16_t nVKCode, int16_t *pnErrorTblIdx), 0x34D20)
//D2FUNC(D2CLIENT, CMDTBL_SaveBindingsSnapshot, int16_t, __cdecl, (), 0x34E90)
//D2FUNC(D2CLIENT, CMDTBL_CommitBindings, void, __cdecl, (), 0x34EB0)
//D2FUNC(D2CLIENT, CMDTBL_RestoreBindingsSnapshot, int16_t, __cdecl, (), 0x34EC0)
//D2FUNC(D2CLIENT, CMDTBL_UnbindCommand, void, __fastcall, (int32_t eCmd, int32_t bPrimary), 0x34EE0)
//D2FUNC(D2CLIENT, CMDTBL_ControlsIsActionBound, BOOL, __fastcall, (D2HotkeyIndices eCmd, BOOL bPrimary), 0x34F00)
//D2FUNC(D2CLIENT, CMDTBL_ControlsGetBoundKeyNameFallback, const Unicode *, __fastcall, (D2HotkeyIndices eCmd, BOOL bPrimary), 0x34F50)
//D2FUNC(D2CLIENT, CMDTBL_ControlsGetBoundKeyName, const Unicode *, __fastcall, (D2HotkeyIndices eCmd, BOOL bPrimary), 0x356B0)
//D2FUNC(D2CLIENT, CMDTBL_OnKeyDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x35A00)
//D2FUNC(D2CLIENT, CMDTBL_OnKeyUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x35B00)
//D2FUNC(D2CLIENT, CMDTBL_SetKeyHookMode, void, __fastcall, (int32_t nMode, int32_t bSwapReleaseHandlers), 0x35BE0)
//D2FUNC(D2CLIENT, DIALOG_OnMouseMove, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x35CA0)
//D2FUNC(D2CLIENT, DIALOG_HitTestLine, int32_t, __fastcall, (D2ClientDialogStrc *pDialog, SMSGHANDLER_PARAMS *pMsg), 0x35D30)
//D2FUNC(D2CLIENT, DIALOG_OnMouseUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x35DC0)
//D2FUNC(D2CLIENT, DIALOG_OnMouseDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x35EC0)
//D2FUNC(D2CLIENT, DIALOG_OnCancelKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x360C0)
//D2FUNC(D2CLIENT, DIALOG_OnMouseWheel, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x36150)
//D2FUNC(D2CLIENT, DIALOG_OnEscape, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x36270)
//D2FUNC(D2CLIENT, DIALOG_OnArrowRight, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x36300)
//D2FUNC(D2CLIENT, DIALOG_OnArrowLeft, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x363C0)
//D2FUNC(D2CLIENT, DIALOG_OnArrowUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x36480)
//D2FUNC(D2CLIENT, DIALOG_OnArrowDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x36540)
//D2FUNC(D2CLIENT, DIALOG_OnEnter, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x36600)
//D2FUNC(D2CLIENT, DIALOG_OnChar, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x36700)
//D2FUNC(D2CLIENT, DIALOG_Init, void, __cdecl, (), 0x36860)
//D2FUNC(D2CLIENT, DIALOG_Shutdown, void, __cdecl, (), 0x36870)
//D2FUNC(D2CLIENT, DIALOG_AddControl, uint32_t *, __fastcall, (D2ClientDialogStrc *pDialog, void *pControl, int32_t nUnused1, int32_t nUnused2), 0x368F0)
//D2FUNC(D2CLIENT, DIALOG_CreateFromImage, D2ClientDialogStrc *, __fastcall, (int32_t nCenterX, int32_t nCenterY, DIALOGCALLBACK pfnCallback, DIALOGCALLBACK pfnCallback2, int32_t nImageIndex, int32_t nInputMode), 0x36950)
//D2FUNC(D2CLIENT, DIALOG_Create, D2ClientDialogStrc *, __fastcall, (int32_t nCenterX, int32_t nCenterY, DIALOGCALLBACK pfnCallback, DIALOGCALLBACK pfnCallback2, int32_t nInputMode, int32_t bVAlignByStaticLines, int32_t bAutoSize, int32_t nWidth, int32_t nHeight, int32_t bIgnoreCancelKey, int32_t bGrabsKeyboard), 0x36BE0)
//D2FUNC(D2CLIENT, DIALOG_Close, int32_t, __fastcall, (D2ClientDialogStrc *hDialog), 0x36DC0)
//D2FUNC(D2CLIENT, DIALOG_Draw, int32_t, __fastcall, (D2ClientDialogStrc *pDialog), 0x36EB0)
//D2FUNC(D2CLIENT, DIALOG_SetSelectedLine, void, __fastcall, (D2ClientDialogStrc *pDialog, int32_t nIndex), 0x37090)
//D2FUNC(D2CLIENT, DIALOG_SetHighlight, void, __fastcall, (D2ClientDialogStrc *pDialog, int32_t eHighlight), 0x37150)
//D2FUNC(D2CLIENT, DIALOG_AddLine, void, __fastcall, (D2ClientDialogStrc *pDialog, const struct Unicode *wszText, int32_t nLineHeight, int32_t nUnused1, int32_t nBaseColor, int32_t nFont, DIALOGCALLBACK pfnOnClick, int32_t bSelectable), 0x37180)
//D2FUNC(D2CLIENT, DIALOG_AutoSize, void, __fastcall, (D2ClientDialogStrc *pDialog), 0x37320)
//D2FUNC(D2CLIENT, DIALOG_GetSelectedLine, int32_t, __fastcall, (D2ClientDialogStrc *pDialog), 0x37520)
//D2FUNC(D2CLIENT, DIALOG_GetNestDepth, int32_t, __cdecl, (), 0x37530)
//D2FUNC(D2CLIENT, EDITBOX_UiEditBoxConstruct, char *, __thiscall, (D2ClientEditBoxStrc *pEditBox, int32_t nX, int32_t nY, int32_t nWidth, int32_t nLineCount, int32_t nMaxValue, uint16_t nMaxChars, void *pfnCallback, int32_t nFont), 0x37540)
//D2FUNC(D2CLIENT, EDITBOX_UiEditBoxVecDelDtor, D2ClientEditBoxStrc *, __thiscall, (D2ClientEditBoxStrc *pEditBox, uint8_t bFreeObject), 0x37730)
//D2FUNC(D2CLIENT, EDITBOX_UiEditBoxDestruct, void, __thiscall, (D2ClientEditBoxStrc *pEditBox), 0x37760)
//D2FUNC(D2CLIENT, EDITBOX_UiEditBoxSetNumericOnly, int32_t, __thiscall, (D2ClientEditBoxStrc *pEditBox, int32_t bNumericOnly), 0x377F0)
//D2FUNC(D2CLIENT, EDITBOX_UiEditBoxDraw, void, __thiscall, (D2ClientEditBoxStrc *pEditBox), 0x37800)
//D2FUNC(D2CLIENT, EDITBOX_UiEditBoxSetTextFromInt, int32_t, __thiscall, (D2ClientEditBoxStrc *pEditBox, int32_t nValue), 0x37B00)
//D2FUNC(D2CLIENT, EDITBOX_UiEditBoxSetTextFromAscii, int32_t, __thiscall, (D2ClientEditBoxStrc *pEditBox, const char *szText), 0x37B60)
//D2FUNC(D2CLIENT, EDITBOX_UiEditBoxGetIntValue, int32_t, __fastcall, (D2ClientEditBoxStrc *pEditBox), 0x37B90)
//D2FUNC(D2CLIENT, EDITBOX_UiEditBoxGetTextUtf8, char *, __thiscall, (D2ClientEditBoxStrc *pEditBox, char *), 0x37BE0)
//D2FUNC(D2CLIENT, EDITBOX_ReturnZeroCallback, int32_t, __cdecl, (), 0x37C00)
//D2FUNC(D2CLIENT, EDITBOX_UiEditBoxOnMouseDown, int32_t, __thiscall, (D2ClientEditBoxStrc *pEditBox, SMSGHANDLER_PARAMS *pMsg), 0x37C10)
//D2FUNC(D2CLIENT, EDITBOX_UiEditBoxOnMouseUp, int32_t, __thiscall, (D2ClientEditBoxStrc *pEditBox, SMSGHANDLER_PARAMS *pMsg), 0x37C20)
//D2FUNC(D2CLIENT, EDITBOX_UiEditBoxOnChar, int32_t, __thiscall, (D2ClientEditBoxStrc *pEditBox, SMSGHANDLER_PARAMS *pMsg), 0x37C40)
//D2FUNC(D2CLIENT, EDITBOX_DialogNullCallback, int32_t, __stdcall, (int32_t a1), 0x37F60)
//D2FUNC(D2CLIENT, EDITBOX_UiEditBoxSetWidth, int32_t, __thiscall, (D2ClientEditBoxStrc *pEditBox, int32_t nWidth), 0x37F70)
//D2FUNC(D2CLIENT, EDITBOX_UiCtrlSetFlag24, int32_t, __thiscall, (void *pControl, int32_t bFlag), 0x37F80)
//D2FUNC(D2CLIENT, EDITBOX_UiEditBoxOnArrowRight, int32_t, __thiscall, (D2ClientEditBoxStrc *pEditBox, SMSGHANDLER_PARAMS *pMsg), 0x37F90)
//D2FUNC(D2CLIENT, EDITBOX_UiEditBoxOnArrowLeft, int32_t, __thiscall, (D2ClientEditBoxStrc *pEditBox, SMSGHANDLER_PARAMS *pMsg), 0x37FB0)
//D2FUNC(D2CLIENT, EDITBOX_UiCtrlProxyDestruct, void, __thiscall, (int32_t *thisptr), 0x37FD0)
//D2FUNC(D2CLIENT, EDITBOX_UiCtrlProxyVecDelDtor, uint32_t **, __thiscall, (uint32_t **pProxy, uint8_t bFreeObject), 0x38030)
//D2FUNC(D2CLIENT, EDITBOX_UiCtrlProxyConstructEditbox, uint32_t *, __thiscall, (uint32_t *thisptr, int32_t nX, int32_t nY, int32_t nWidth, int32_t nLineCount, int32_t nMaxValue, uint16_t nMaxChars, void *pfnCallback, int32_t nFont), 0x38060)
//D2FUNC(D2CLIENT, EDITBOX_UiCtrlProxyFwdSlot4, int32_t, __thiscall, (uint32_t **thisptr, SMSGHANDLER_PARAMS *pMsg), 0x38100)
//D2FUNC(D2CLIENT, EDITBOX_UiCtrlProxyFwdSlot5, int32_t, __thiscall, (uint32_t **thisptr, SMSGHANDLER_PARAMS *pMsg), 0x38110)
//D2FUNC(D2CLIENT, EDITBOX_UiCtrlProxyFwdSlot6, int32_t, __thiscall, (uint32_t **thisptr, SMSGHANDLER_PARAMS *pMsg), 0x38120)
//D2FUNC(D2CLIENT, EDITBOX_UiCtrlProxyFwdSlot8, int32_t, __thiscall, (uint32_t **thisptr, SMSGHANDLER_PARAMS *pMsg), 0x38130)
//D2FUNC(D2CLIENT, EDITBOX_UiCtrlProxyFwdSlot7, int32_t, __thiscall, (uint32_t **thisptr, SMSGHANDLER_PARAMS *pMsg), 0x38140)
//D2FUNC(D2CLIENT, EDITBOX_UiCtrlProxyFwdSlot9, int32_t, __thiscall, (uint32_t **thisptr, SMSGHANDLER_PARAMS *pMsg), 0x38150)
//D2FUNC(D2CLIENT, EDITBOX_UiCtrlProxyFwdSlot10, int32_t, __thiscall, (uint32_t **thisptr, SMSGHANDLER_PARAMS *pMsg), 0x38160)
//D2FUNC(D2CLIENT, EDITBOX_UiCtrlProxyFwdSlot11, int32_t, __thiscall, (uint32_t **thisptr), 0x38170)
//D2FUNC(D2CLIENT, EDITBOX_UiCtrlProxyFwdSlot13Arg, int32_t, __thiscall, (uint32_t **thisptr, int32_t), 0x38180)
//D2FUNC(D2CLIENT, EDITBOX_UiCtrlProxyFwdSlot15, int32_t, __thiscall, (uint32_t **thisptr, int32_t nArg), 0x38190)
//D2FUNC(D2CLIENT, EDITBOX_UiCtrlProxyFwdSlot14, int32_t, __thiscall, (uint32_t **thisptr, int32_t nArg), 0x381A0)
//D2FUNC(D2CLIENT, EDITBOX_UiCtrlProxyFwdSlot12, int32_t, __thiscall, (uint32_t **thisptr, int32_t nArg), 0x381B0)
//D2FUNC(D2CLIENT, EDITBOX_UiCtrlProxyFwdSlot16, int32_t, __thiscall, (uint32_t **thisptr, int32_t nArg), 0x381C0)
//D2FUNC(D2CLIENT, EDITBOX_UiCtrlProxyFwdSlot17, int32_t, __thiscall, (uint32_t **thisptr, int32_t nArg), 0x381D0)
//D2FUNC(D2CLIENT, EDITBOX_UiCtrlProxyFwdSlot18, int32_t, __thiscall, (uint32_t **thisptr, int32_t nArg), 0x381E0)
//D2FUNC(D2CLIENT, HELP_LoadImages, void, __cdecl, (), 0x381F0)
//D2FUNC(D2CLIENT, HELP_CenterTextX, int32_t, __fastcall, (D2C_StringIndices nTblIdx, int32_t nCenterX), 0x38280)
//D2FUNC(D2CLIENT, HELP_DimGameScreen, void, __cdecl, (), 0x382E0)
//D2FUNC(D2CLIENT, HELP_DrawScreen, void, __cdecl, (), 0x38310)
//D2FUNC(D2CLIENT, HELP_DrawBackground, void, __cdecl, (), 0x38920)
//D2FUNC(D2CLIENT, HELP_DrawKeyBulletCels, void, __cdecl, (), 0x38A70)
//D2FUNC(D2CLIENT, HELP_DrawLeaderLines, void, __cdecl, (), 0x38AD0)
//D2FUNC(D2CLIENT, HELP_DrawLabelsWide, void, __cdecl, (), 0x38E50)
//D2FUNC(D2CLIENT, HELP_DrawLabels, void, __cdecl, (), 0x397D0)
//D2FUNC(D2CLIENT, HELP_DrawCloseButton, void, __cdecl, (), 0x3A0D0)
//D2FUNC(D2CLIENT, HELP_UnloadImages, void, __cdecl, (), 0x3A140)
//D2FUNC(D2CLIENT, HELP_HelpScreenOnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x3A190)
//D2FUNC(D2CLIENT, HELP_HelpScreenOnLButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x3A220)
//D2FUNC(D2CLIENT, HELP_HelpScreenOnDismissKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x3A2D0)
//D2FUNC(D2CLIENT, HELP_Nullsub5, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x3A310)
//D2FUNC(D2CLIENT, HELP_CloseMiniPanelIfOpened, int32_t, __cdecl, (), 0x3A320)
//D2FUNC(D2CLIENT, HELP_DrawHelpHint, void, __cdecl, (), 0x3A350)
//D2FUNC(D2CLIENT, HELP_HelpButtonOnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x3A5F0)
//D2FUNC(D2CLIENT, HELP_HelpButtonOnLButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x3A660)
//D2FUNC(D2CLIENT, HIREABLES_PanelPortraitsReserveTopStrip, BOOL, __cdecl, (), 0x3A710)
//D2FUNC(D2CLIENT, HIREABLES_PanelLoadHirelingIcons, void, __cdecl, (), 0x3A720)
//D2FUNC(D2CLIENT, HIREABLES_PanelFreeHirelingIcons, void, __cdecl, (), 0x3A8F0)
//D2FUNC(D2CLIENT, HIREABLES_PanelInitLifeBarColors, void, __fastcall, (int32_t bForce), 0x3A970)
//D2FUNC(D2CLIENT, HIREABLES_PanelInvalidateLifeBarColors, void, __cdecl, (), 0x3A9D0)
//D2FUNC(D2CLIENT, HIREABLES_PanelGetHirelingIcon, D2CellFileStrc *, __fastcall, (D2UnitStrc *pUnit, int32_t nPetType, uint32_t *pdwGUID, int32_t *pbFlag), 0x3A9E0)
//D2FUNC(D2CLIENT, HIREABLES_PanelDrawHirelingIconHint, void, __cdecl, (), 0x3AB00)
//D2FUNC(D2CLIENT, HIREABLES_PanelDrawPartyLifeBar, void, __fastcall, (uint32_t dwGUID, int32_t nX, int32_t bIsPet), 0x3B180)
//D2FUNC(D2CLIENT, HIREABLES_PanelDrawTruncatedHintLabel, void, __fastcall, (Unicode *wszText, int32_t nX, int32_t nY, int32_t nColor), 0x3B280)
//D2FUNC(D2CLIENT, HIREABLES_PanelDrawIconNameClipped, void, __fastcall, (D2UnitStrc *pUnit, int32_t nX, int32_t nY), 0x3B300)
//D2FUNC(D2CLIENT, HIREABLES_PanelDrawIconPetTypeClipped, void, __fastcall, (int32_t nPetType, int32_t nX, int32_t nY), 0x3B3D0)
//D2FUNC(D2CLIENT, HIREABLES_PanelDrawIconNumberClipped, void, __fastcall, (int32_t nValue, int32_t nX), 0x3B4D0)
//D2FUNC(D2CLIENT, HIREABLES_PanelToggleHintIndent, void, __fastcall, (int32_t *pnPos, int32_t *pnStagger), 0x3B5C0)
//D2FUNC(D2CLIENT, HIREABLES_PanelDrawHirelingHintRow, BOOL, __fastcall, (D2UnitStrc *pOwner, int32_t nPetType, int32_t *pnX, int32_t *pnY, int32_t *pnStagger), 0x3B5F0)
//D2FUNC(D2CLIENT, HIREABLES_PanelUpdateHirelingIconsVisible, void, __cdecl, (), 0x3B890)
//D2FUNC(D2CLIENT, HIREABLES_HirIconsOnF11, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x3B980)
//D2FUNC(D2CLIENT, HIREABLES_HireIconsOnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x3BA00)
//D2FUNC(D2CLIENT, HIREABLES_CanGiveCursorItem, BOOL, __fastcall, (BOOL bPlaySound), 0x3BA90)
//D2FUNC(D2CLIENT, HIREABLES_HireIconsOnMouseMove, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x3BC90)
//D2FUNC(D2CLIENT, HIREABLES_CmdTblActionShowPortraits, void, __cdecl, (), 0x3BD70)
//D2FUNC(D2CLIENT, HIREABLES_HireIconsOnRButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x3BE20)
//D2FUNC(D2CLIENT, HIREABLES_HireIconsResetAndShow, void, __cdecl, (), 0x3BF00)
//D2FUNC(D2CLIENT, INSERTDISC_DrawInsertDiscPrompt, BOOL, __stdcall, (int32_t nUnused), 0x3BF50)
//D2FUNC(D2CLIENT, INSERTDISC_DrawInsertDiscPromptScene, int32_t, __cdecl, (), 0x3C180)
//D2FUNC(D2CLIENT, INSERTDISC_ShowInsertPlayDiscPrompt, int32_t, __stdcall, (), 0x3C390)
//D2FUNC(D2CLIENT, INSERTDISC_ShowInsertExpansionDiscPrompt, int32_t, __stdcall, (), 0x3C470)
//D2FUNC(D2CLIENT, INSERTDISC_OnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x3C550)
//D2FUNC(D2CLIENT, INSERTDISC_OnLButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x3C5F0)
//D2FUNC(D2CLIENT, INSERTDISC_OnEnter, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x3C690)
//D2FUNC(D2CLIENT, INSERTDISC_OnEscape, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x3C6D0)
//D2FUNC(D2CLIENT, INSERTDISC_DrawInsertCinematicsDiscPrompt, BOOL, __stdcall, (int32_t nUnused), 0x3C6E0)
//D2FUNC(D2CLIENT, INSERTDISC_DrawInsertCinematicsDiscPromptScene, int32_t, __cdecl, (), 0x3C970)
//D2FUNC(D2CLIENT, INSERTDISC_ShowInsertCinematicsDiscPrompt, int32_t, __stdcall, (), 0x3CBD0)
//D2FUNC(D2CLIENT, INSERTDISC_CineDiscOnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x3CCC0)
//D2FUNC(D2CLIENT, INSERTDISC_CineDiscOnLButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x3CDB0)
//D2FUNC(D2CLIENT, INSERTDISC_CineDiscOnEnter, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x3CEB0)
//D2FUNC(D2CLIENT, INSERTDISC_CineDiscOnEscape, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x3CEF0)
//D2FUNC(D2CLIENT, INSERTDISC_SetDiscPromptKind, void, __fastcall, (int32_t nKind), 0x3CF00)
//D2FUNC(D2CLIENT, LANG_SubstituteArg, void, __fastcall, (struct Unicode *wszText, int32_t nIndex, const struct Unicode *wszValue), 0x3CF10)
//D2FUNC(D2CLIENT, LANG_FormatString, void, __cdecl, (struct Unicode *wszOut, const struct Unicode *wszFormat, ...), 0x3CFE0)
//D2FUNC(D2CLIENT, INV_InitHighlightColors, void, __cdecl, (), 0x3D2F0)
//D2FUNC(D2CLIENT, INV_BuildItemNameText, void, __fastcall, (D2UnitStrc *pItem, Unicode *wszOut, int32_t nMaxLen), 0x3D360)
//D2FUNC(D2CLIENT, INV_LoadPanelImages, void, __cdecl, (), 0x3E8A0)
//D2FUNC(D2CLIENT, INV_UnloadPanelImages, void, __cdecl, (), 0x3E970)
//D2FUNC(D2CLIENT, INV_UpdateAndDrawPanels, void, __cdecl, (), 0x3EAC0)
//D2FUNC(D2CLIENT, INV_EnsurePanelLayout, void, __cdecl, (), 0x40A10)
//D2FUNC(D2CLIENT, INV_DrawSocketedItems, void, __fastcall, (D2UnitStrc *pItem, D2InventoryGridInfoStrc *pGridInfo, int32_t nX, int32_t nY), 0x40DC0)
//D2FUNC(D2CLIENT, INV_DrawInventoryGridItems, void, __fastcall, (D2UnitStrc *pUnit), 0x40F30)
//D2FUNC(D2CLIENT, INV_DrawGridHighlight, void, __fastcall, (D2InventoryGridInfoStrc *pGridInfo, int32_t nCellX, int32_t nCellY, uint8_t nWidth, uint8_t nHeight, uint32_t nColorIdx), 0x41710)
//D2FUNC(D2CLIENT, INV_CanInsertSocketFiller, BOOL, __fastcall, (D2UnitStrc *pFiller, D2UnitStrc *pTarget), 0x417D0)
//D2FUNC(D2CLIENT, INV_DrawItemDescription, void, __fastcall, (D2UnitStrc *pUnit, BOOL bVendorUnit), 0x41890)
//D2FUNC(D2CLIENT, INV_AppendEtherealAndSocketsText, void, __fastcall, (D2UnitStrc *pItem, Unicode *wszOut), 0x43EE0)
//D2FUNC(D2CLIENT, INV_AppendDurabilityText, void, __fastcall, (D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt), 0x44060)
//D2FUNC(D2CLIENT, INV_FormatReqLevelLine, void, __fastcall, (struct Unicode *wszOut, int32_t nLevel), 0x441B0)
//D2FUNC(D2CLIENT, INV_FormatReqStrengthLine, void, __fastcall, (struct Unicode *wszOut, D2ItemsTxt *pItemsTxt, int32_t nDelta), 0x44250)
//D2FUNC(D2CLIENT, INV_FormatReqDexterityLine, void, __fastcall, (struct Unicode *wszOut, D2ItemsTxt *pItemsTxt, int32_t nDelta), 0x44310)
//D2FUNC(D2CLIENT, INV_AppendElementalDamageText, void, __fastcall, (D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt), 0x443D0)
//D2FUNC(D2CLIENT, INV_CalcElementalDamageRange, void, __fastcall, (D2UnitStrc *pTarget, D2UnitStrc *pItem, D2C_ItemStats nMinStatId, D2C_ItemStats nMaxStatId, int32_t *pnMinDamage, int32_t *pnMaxDamage, BOOL *pbAdjusted), 0x44C60)
//D2FUNC(D2CLIENT, INV_AppendChanceToBlockText, void, __fastcall, (D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt), 0x44EE0)
//D2FUNC(D2CLIENT, INV_AppendSkillDamageText, void, __fastcall, (D2UnitStrc *pItem, struct Unicode *pwszOut, D2ItemsTxt *pItemsTxt), 0x45040)
//D2FUNC(D2CLIENT, INV_AppendDefenseText, void, __fastcall, (D2UnitStrc *pUnit, D2UnitStrc *pItem, struct Unicode *pwszOut, D2ItemsTxt *pItemsTxt), 0x451D0)
//D2FUNC(D2CLIENT, INV_AppendStackQuantityText, void, __fastcall, (D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt), 0x454B0)
//D2FUNC(D2CLIENT, INV_AppendAttackSpeedText, void, __fastcall, (D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt), 0x45570)
//D2FUNC(D2CLIENT, INV_AppendSpellDescText, void, __fastcall, (D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt), 0x45710)
//D2FUNC(D2CLIENT, INV_FindComparableEquippedItem, D2UnitStrc *, __fastcall, (D2InventoryStrc *pInventory, int32_t nFileIndex), 0x45990)
//D2FUNC(D2CLIENT, INV_AppendPriceFooterText, void, __fastcall, (const Unicode *pwszIn, Unicode *pwszOut), 0x45A40)
//D2FUNC(D2CLIENT, INV_DataTblsGetSetsTxtRecord, D2SetsTxt *, __fastcall, (int32_t nSetId), 0x45C50)
//D2FUNC(D2CLIENT, DATATBLS_GetSetItemsTxtRecord, D2SetItemsTxt *, __fastcall, (int32_t nFileIndex), 0x45C80)
//D2FUNC(D2CLIENT, INV_OnBodyLocClickRightArm, int32_t, __fastcall, (D2UnitStrc *pUnit, D2InventoryStrc *pInventory, D2C_PlayerBodyLocs nBodyLoc), 0x45CB0)
//D2FUNC(D2CLIENT, INV_OnBodyLocClickLeftArm, int32_t, __fastcall, (D2UnitStrc *pUnit, D2InventoryStrc *pInventory, D2C_PlayerBodyLocs nBodyLoc), 0x46360)
//D2FUNC(D2CLIENT, INV_OnBodyLocClickGeneric, int32_t, __fastcall, (D2UnitStrc *pUnit, D2InventoryStrc *pInventory, D2C_PlayerBodyLocs nBodyLoc), 0x46A00)
//D2FUNC(D2CLIENT, INV_OnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x46E60)
//D2FUNC(D2CLIENT, INV_OnGridLeftClick, int32_t, __fastcall, (D2UnitStrc *pUnit, D2InventoryStrc *pInventory, int32_t nXPos, int32_t nYPos, uint8_t nMouseFlags, D2InventoryGridInfoStrc *pGridInfo, D2C_ItemInvPage nInvPage), 0x475C0)
//D2FUNC(D2CLIENT, INV_OnLButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x48140)
//D2FUNC(D2CLIENT, INV_OnMouseMove, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x48290)
//D2FUNC(D2CLIENT, INV_PixelToGridCell, void, __fastcall, (D2UnitStrc *pUnit, D2InventoryStrc *pInventory, D2UnitStrc *pItem, uint32_t nX, uint32_t nY, D2InventoryGridInfoStrc *pGridInfo, D2C_ItemInvPage nInvPage), 0x487C0)
//D2FUNC(D2CLIENT, INV_OnRButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x48AA0)
//D2FUNC(D2CLIENT, INV_UseItemAtGridCell, int32_t, __fastcall, (D2UnitStrc *pUnit, D2InventoryStrc *pInventory, int32_t nX, int32_t nY, D2C_ItemInvPage nInvPage), 0x48C60)
//D2FUNC(D2CLIENT, INV_ResetItemDescState, void, __cdecl, (), 0x48F10)
//D2FUNC(D2CLIENT, INV_ClearHoverForItem, void, __fastcall, (D2UnitStrc *pItem), 0x48F50)
//D2FUNC(D2CLIENT, INV_GetVendorPageInventory, void, __fastcall, (int32_t nInvPage), 0x48F90)
//D2FUNC(D2CLIENT, INV_ResetPanelState, void, __cdecl, (), 0x490F0)
//D2FUNC(D2CLIENT, INV_NpcShopBuildTabs, void, __cdecl, (), 0x49130)
//D2FUNC(D2CLIENT, INV_NpcShopRequestPageRebuild, void, __cdecl, (), 0x494D0)
//D2FUNC(D2CLIENT, INV_NpcShopEnterPanelMode19, void, __cdecl, (), 0x494E0)
//D2FUNC(D2CLIENT, INV_LeavePanelMode5, void, __cdecl, (), 0x49500)
//D2FUNC(D2CLIENT, INV_NpcShopLeaveVerifyMode, void, __cdecl, (), 0x49550)
//D2FUNC(D2CLIENT, INV_NpcShopEnterPanelMode6, BOOL, __cdecl, (), 0x49590)
//D2FUNC(D2CLIENT, INV_ResetItemGrids, void, __cdecl, (), 0x495B0)
//D2FUNC(D2CLIENT, INV_NpcShopEnterPanelMode7, void, __cdecl, (), 0x49620)
//D2FUNC(D2CLIENT, INV_NpcShopCloseAndRestoreCursor, void, __cdecl, (), 0x49630)
//D2FUNC(D2CLIENT, INV_NpcShopSetTabRepairAll, void, __fastcall, (int32_t nSlot), 0x49690)
//D2FUNC(D2CLIENT, INV_NpcShopSetTabRepair, void, __fastcall, (int32_t nSlot), 0x496D0)
//D2FUNC(D2CLIENT, INV_NpcShopSetTabBuy, void, __fastcall, (int32_t nSlot), 0x49710)
//D2FUNC(D2CLIENT, INV_NpcShopSetTabSell, void, __fastcall, (int32_t nSlot), 0x49750)
//D2FUNC(D2CLIENT, INV_NpcShopSetTabEmpty, void, __fastcall, (int32_t nSlot), 0x49790)
//D2FUNC(D2CLIENT, INV_DrawGoldAmount, void, __fastcall, (int32_t nPanelKind), 0x497C0)
//D2FUNC(D2CLIENT, INV_ItemPanelOnRButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x49B60)
//D2FUNC(D2CLIENT, INV_NpcShopOnDismissKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x49FC0)
//D2FUNC(D2CLIENT, INV_NpcShopOnMouseMove, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4A080)
//D2FUNC(D2CLIENT, INV_NpcShopOnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4A1C0)
//D2FUNC(D2CLIENT, INV_NpcShopOnLButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4A850)
//D2FUNC(D2CLIENT, INV_ConsumeMessage, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4ABD0)
//D2FUNC(D2CLIENT, INV_OnChatKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4ABF0)
//D2FUNC(D2CLIENT, INV_NpcShopApplyTabSelection, void, __cdecl, (), 0x4AC40)
//D2FUNC(D2CLIENT, INV_AnvilForwardMouseMoveToInventory, int32_t, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4ADA0)
//D2FUNC(D2CLIENT, INV_AnvilForwardLButtonDownToInventory, int32_t, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4AE00)
//D2FUNC(D2CLIENT, INV_AnvilOnInvCloseButtonRelease, int32_t, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4AE70)
//D2FUNC(D2CLIENT, INV_AnvilOnInvCloseButtonPress, int32_t, __fastcall, (SMSGHANDLER_PARAMS *pMsg), 0x4AF40)
//D2FUNC(D2CLIENT, INV_NpcShopLoadTabImages, void, __cdecl, (), 0x4AFD0)
//D2FUNC(D2CLIENT, INV_UnloadShopPanelImages, void, __cdecl, (), 0x4B090)
//D2FUNC(D2CLIENT, INV_TradeSetPanelMode, void, __fastcall, (int32_t bTradeMode), 0x4B100)
//D2FUNC(D2CLIENT, INV_TradeOnMouseMove, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4B1D0)
//D2FUNC(D2CLIENT, INV_TradeOnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4B2D0)
//D2FUNC(D2CLIENT, INV_TradeOnLButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4B5B0)
//D2FUNC(D2CLIENT, INV_TradeOnDismissKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4B870)
//D2FUNC(D2CLIENT, INV_UpdateToggleSounds, void, __fastcall, (int32_t bFirst, int32_t bSecond), 0x4B8B0)
//D2FUNC(D2CLIENT, INV_IsPanelModeNormal, BOOL, __cdecl, (), 0x4B910)
//D2FUNC(D2CLIENT, INV_IsInventoryMode4, BOOL, __cdecl, (), 0x4B930)
//D2FUNC(D2CLIENT, INV_GetTransactionItemIsPlayers, int32_t, __cdecl, (), 0x4B940)
//D2FUNC(D2CLIENT, INV_ClearGoldDialogGuard, void, __cdecl, (), 0x4B950)
//D2FUNC(D2CLIENT, INV_ArmGoldDialogGuard, void, __cdecl, (), 0x4B970)
//D2FUNC(D2CLIENT, INV_SetHoverEnabled, void, __fastcall, (int32_t bEnabled), 0x4B9A0)
//D2FUNC(D2CLIENT, INV_StashOnMouseMove, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4B9F0)
//D2FUNC(D2CLIENT, INV_StashOnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4BAD0)
//D2FUNC(D2CLIENT, INV_StashOnLButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4C030)
//D2FUNC(D2CLIENT, INV_StashOnDismissKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4C3B0)
//D2FUNC(D2CLIENT, INV_TradeOnShowBeltKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4C440)
//D2FUNC(D2CLIENT, INV_OnKeySwapWeapons, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4C470)
//D2FUNC(D2CLIENT, INV_StashOpenPanel, void, __cdecl, (), 0x4C510)
//D2FUNC(D2CLIENT, INV_CubeFindStashUnit, void, __fastcall, (D2UnitStrc *pUnit), 0x4C560)
//D2FUNC(D2CLIENT, INV_StashLoadImages, void, __cdecl, (), 0x4C5C0)
//D2FUNC(D2CLIENT, INV_StashOnPanelClose, void, __cdecl, (), 0x4C650)
//D2FUNC(D2CLIENT, INV_StashTeardownPanel, void, __cdecl, (), 0x4C6C0)
//D2FUNC(D2CLIENT, INV_CubeOnMouseMove, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4C720)
//D2FUNC(D2CLIENT, INV_CubeOnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4C7C0)
//D2FUNC(D2CLIENT, INV_CubeOnLButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4CB60)
//D2FUNC(D2CLIENT, INV_CubeOnDismissKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4CEA0)
//D2FUNC(D2CLIENT, INV_CubeOpenPanel, void, __cdecl, (), 0x4CF90)
//D2FUNC(D2CLIENT, INV_CubeLoadPanelImage, void, __cdecl, (), 0x4CFE0)
//D2FUNC(D2CLIENT, INV_CubeOnPanelClose, void, __cdecl, (), 0x4D030)
//D2FUNC(D2CLIENT, INV_CubeStartQuestItemAnim, void, __fastcall, (uint32_t dwItemCode), 0x4D0D0)
//D2FUNC(D2CLIENT, INV_CubeResetQuestItemAnim, void, __cdecl, (), 0x4D190)
//D2FUNC(D2CLIENT, INV_TradeRestoreHeldItem, void, __cdecl, (), 0x4D1C0)
//D2FUNC(D2CLIENT, UNRESOLVED_sub_6FAED210, void, __cdecl, (), 0x4D210)
//D2FUNC(D2CLIENT, INV_TradeResetPanelState, void, __cdecl, (), 0x4D220)
//D2FUNC(D2CLIENT, INV_TradeSetAcceptBlocked, void, __cdecl, (), 0x4D260)
//D2FUNC(D2CLIENT, INV_TradeClearAcceptBlocked, void, __cdecl, (), 0x4D270)
//D2FUNC(D2CLIENT, INV_ToggleWeaponSwapSet, void, __cdecl, (), 0x4D280)
//D2FUNC(D2CLIENT, INV_SwapWeaponsAndLockItems, void, __cdecl, (), 0x4D2B0)
//D2FUNC(D2CLIENT, INV_MercInvDrawSlotHighlights, void, __fastcall, (D2UnitStrc *pMerc), 0x4D310)
//D2FUNC(D2CLIENT, INV_MercInvCanMercEquipItem, int32_t, __fastcall, (D2UnitStrc *pMerc, D2UnitStrc *pItem, int32_t *pbFits), 0x4D470)
//D2FUNC(D2CLIENT, INV_MercInvDrawPanel, void, __cdecl, (), 0x4D5D0)
//D2FUNC(D2CLIENT, INV_MercInvDrawStatRows, void, __fastcall, (D2UnitStrc *pMerc), 0x4DA20)
//D2FUNC(D2CLIENT, INV_MercInvDrawEmptySlots, void, __fastcall, (D2UnitStrc *pMerc), 0x4E150)
//D2FUNC(D2CLIENT, INV_MercInvOnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4E340)
//D2FUNC(D2CLIENT, INV_MercInvOnMouseUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4E490)
//D2FUNC(D2CLIENT, INV_MercInvOnMouseMove, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4E730)
//D2FUNC(D2CLIENT, NEWSTATS_RecipeScrollShow, void, __thiscall, (const Unicode *pwszText, int32_t nColor), 0x4E990)
//D2FUNC(D2CLIENT, NEWSTATS_RecipeScrollDraw, void, __cdecl, (), 0x4E9C0)
//D2FUNC(D2CLIENT, NEWSTATS_RecipeScrollOnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4EB30)
//D2FUNC(D2CLIENT, NEWSTATS_LoadImages, void, __cdecl, (), 0x4EBA0)
//D2FUNC(D2CLIENT, NEWSTATS_UnloadImages, void, __cdecl, (), 0x4EBC0)
//D2FUNC(D2CLIENT, NEWSTATS_UpdateButtonVisibility, void, __cdecl, (), 0x4EC00)
//D2FUNC(D2CLIENT, NEWSTATS_IsPointOverButton, BOOL, __fastcall, (D2C_UIpanels nUiShift, int32_t nXPos, int32_t nYPos), 0x4ECC0)
//D2FUNC(D2CLIENT, NEWSTATS_NewSkillsIsPointOverButton, BOOL, __fastcall, (D2C_UIpanels nUiShift, int32_t nXPos, int32_t nYPos), 0x4ED20)
//D2FUNC(D2CLIENT, NEWSTATS_OnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4ED80)
//D2FUNC(D2CLIENT, NEWSTATS_NewSkillsOnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4EEA0)
//D2FUNC(D2CLIENT, NEWSTATS_OnLButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4EFD0)
//D2FUNC(D2CLIENT, NEWSTATS_NewSkillsOnLButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x4F130)
//D2FUNC(D2CLIENT, NEWSTATS_PanelDrawNewStatsButtonGrey, void, __cdecl, (), 0x4F2A0)
//D2FUNC(D2CLIENT, NEWSTATS_PanelDrawNewStatsButton, void, __cdecl, (), 0x4F360)
//D2FUNC(D2CLIENT, NEWSTATS_PanelDrawNewSkillButtonGrey, void, __cdecl, (), 0x4F670)
//D2FUNC(D2CLIENT, NEWSTATS_PanelDrawNewSkillButton, void, __cdecl, (), 0x4F730)
//D2FUNC(D2CLIENT, LISTBOX_LoadScrollBarImage, D2CellFileStrc *, __cdecl, (), 0x4FA40)
//D2FUNC(D2CLIENT, LISTBOX_UnloadScrollBarImage, void, __cdecl, (), 0x4FA80)
//D2FUNC(D2CLIENT, LISTBOX_ImplConstruct, D2ClientListBoxStrc *, __thiscall, (D2ClientListBoxStrc *pListBox, int32_t nX, int32_t nY, int32_t nWidth, int32_t nHeight, int32_t nEntryGap, int32_t nDefaultColor, int32_t nFont, void *pfnOnSelect, uint8_t bWideLayout), 0x4FAA0)
//D2FUNC(D2CLIENT, LISTBOX_ImplVecDelDtor, D2ClientListBoxStrc *, __thiscall, (D2ClientListBoxStrc *pListBox, uint8_t bDelete), 0x4FD00)
//D2FUNC(D2CLIENT, LISTBOX_UiCtrlBaseDestruct, void, __fastcall, (int32_t *thisptr), 0x4FD30)
//D2FUNC(D2CLIENT, LISTBOX_ImplGetNumEntries, int32_t, __thiscall, (D2ClientListBoxStrc *pListBox), 0x4FD40)
//D2FUNC(D2CLIENT, LISTBOX_ImplRemoveAll, void, __thiscall, (D2ClientListBoxStrc *pListBox), 0x4FD50)
//D2FUNC(D2CLIENT, LISTBOX_ImplUpdateScrollThumb, void, __thiscall, (D2ClientListBoxStrc *pListBox), 0x4FDB0)
//D2FUNC(D2CLIENT, LISTBOX_CountFittingEntries, int32_t, __thiscall, (D2ClientListBoxStrc *pListBox), 0x4FFE0)
//D2FUNC(D2CLIENT, LISTBOX_DrawColumnText, void, __thiscall, (D2ClientListBoxStrc *pListBox, struct Unicode *pwszText, int32_t nX, int32_t nY, int32_t nColor), 0x500A0)
//D2FUNC(D2CLIENT, LISTBOX_ImplDraw, int32_t, __thiscall, (D2ClientListBoxStrc *pListBox), 0x50190)
//D2FUNC(D2CLIENT, LISTBOX_ImplAddEntry, void, __thiscall, (D2ClientListBoxStrc *pListBox, const struct Unicode *pwsCol0, const struct Unicode *pwsCol1, const struct Unicode *pwsCol2, const struct Unicode *pwsCol3, const struct Unicode *pwsCol4, const struct Unicode *pwsCol5, int32_t nUserData, int32_t nColor, int32_t bUseDefaultColor, int32_t nFont), 0x50610)
//D2FUNC(D2CLIENT, LISTBOX_ScrollByTrackClick, int32_t, __thiscall, (D2ClientListBoxStrc *pListBox, SMSGHANDLER_PARAMS *pMsg), 0x508E0)
//D2FUNC(D2CLIENT, LISTBOX_ImplOnMouseWheel, int32_t, __thiscall, (D2ClientListBoxStrc *pListBox, SMSGHANDLER_PARAMS *pMsg), 0x50A30)
//D2FUNC(D2CLIENT, LISTBOX_ImplOnMouseDown, int32_t, __thiscall, (D2ClientListBoxStrc *pListBox, SMSGHANDLER_PARAMS *pMsg), 0x50A90)
//D2FUNC(D2CLIENT, LISTBOX_ImplOnMouseUp, int32_t, __thiscall, (D2ClientListBoxStrc *pListBox, SMSGHANDLER_PARAMS *pMsg), 0x50BF0)
//D2FUNC(D2CLIENT, LISTBOX_ImplOnArrowUp, int32_t, __thiscall, (D2ClientListBoxStrc *pListBox, SMSGHANDLER_PARAMS *pMsg), 0x50DA0)
//D2FUNC(D2CLIENT, LISTBOX_ImplOnArrowDown, int32_t, __thiscall, (D2ClientListBoxStrc *pListBox, SMSGHANDLER_PARAMS *pMsg), 0x50E20)
//D2FUNC(D2CLIENT, LISTBOX_HitTestEntry, int32_t, __thiscall, (D2ClientListBoxStrc *pListBox, SMSGHANDLER_PARAMS *pMsg), 0x50ED0)
//D2FUNC(D2CLIENT, LISTBOX_ImplOnMouseMove, int32_t, __thiscall, (D2ClientListBoxStrc *pListBox, SMSGHANDLER_PARAMS *pMsg), 0x50FF0)
//D2FUNC(D2CLIENT, LISTBOX_ImplOnChar, int32_t, __thiscall, (D2ClientListBoxStrc *pListBox, SMSGHANDLER_PARAMS *pMsg), 0x51040)
//D2FUNC(D2CLIENT, LISTBOX_ImplGetSelection, int32_t, __thiscall, (D2ClientListBoxStrc *pListBox), 0x510A0)
//D2FUNC(D2CLIENT, LISTBOX_ProxyDestruct, int32_t, __thiscall, (uint32_t *thisptr), 0x510B0)
//D2FUNC(D2CLIENT, LISTBOX_ProxyVecDelDtor, uint32_t **, __thiscall, (uint32_t **pProxy, uint8_t bFreeObject), 0x51100)
//D2FUNC(D2CLIENT, LISTBOX_Construct, void *, __thiscall, (void *thisptr, int32_t nX, int32_t nY, int32_t nWidth, int32_t nHeight, int32_t nEntryGap, int32_t nDefaultColor, int32_t nFont, void *pfnOnSelect, uint8_t bWideLayout), 0x51130)
//D2FUNC(D2CLIENT, LISTBOX_UiCtrlProxyFwdSlot1, int32_t, __thiscall, (uint32_t **thisptr), 0x511D0)
//D2FUNC(D2CLIENT, LISTBOX_GetEntryUserData, int32_t, __thiscall, (uint32_t **pProxy, int32_t nEntryIndex), 0x511E0)
//D2FUNC(D2CLIENT, LISTBOX_UiCtrlProxyFwdSlot13, int32_t, __thiscall, (uint32_t **thisptr), 0x512A0)
//D2FUNC(D2CLIENT, LISTBOX_UiCtrlProxyFwdSlot2, int32_t, __thiscall, (uint32_t **thisptr, SMSGHANDLER_PARAMS *pMsg), 0x512B0)
//D2FUNC(D2CLIENT, MODS_AppendBluntUndeadBonusText, void, __fastcall, (D2UnitStrc *pUnit, struct Unicode *wszOut, int32_t nMaxLen), 0x512C0)
//D2FUNC(D2CLIENT, MODS_AppendPropertySetText, void, __fastcall, (D2UnitStrc *pUnit, D2GemsTxt *pGemsTxt, int32_t nPropSet, struct Unicode *wszOut, int32_t nMaxLen, const struct Unicode *wszSep), 0x513C0)
//D2FUNC(D2CLIENT, MODS_BuildSocketFillerText, void, __fastcall, (D2UnitStrc *pItem, struct Unicode *wszOut, int32_t nMaxLen), 0x51480)
//D2FUNC(D2CLIENT, MODS_BuildStatsText, void, __fastcall, (D2UnitStrc *pUnit, struct Unicode *wszOut, int32_t nMaxLen, int32_t bAppendUndeadBonus, D2C_States nState, D2C_States nSourceState, int32_t nFlag, int32_t bAppendNewline), 0x519C0)
//D2FUNC(D2CLIENT, MODS_FormatPropertyLine, BOOL, __fastcall, (D2UnitStrc *pUnit, D2StatListStrc *pStatList, D2C_ItemStats nStatId, int32_t nLayer, int32_t nValue, struct Unicode *wszOut), 0x521C0)
//D2FUNC(D2CLIENT, MODS_AppendMagicModsText, void, __fastcall, (D2UnitStrc *pUnit, struct Unicode *wszOut, int32_t nMaxLen, int32_t bAppendNewline, const struct Unicode *wszSep), 0x53160)
//D2FUNC(D2CLIENT, MODS_AppendSetBonusText, void, __fastcall, (D2UnitStrc *pPlayer, D2UnitStrc *pItem, struct Unicode *wszOut, int32_t nMaxLen), 0x532B0)
//D2FUNC(D2CLIENT, MODS_AppendStateModsText, void, __fastcall, (D2UnitStrc *pUnit, D2UnitStrc *pItem, struct Unicode *wszOut, int32_t nMaxLen), 0x533C0)
//D2FUNC(D2CLIENT, MODS_FormatElementalDamageText, int32_t, __fastcall, (D2ClientDamageGroupStrc *pDamageGroups, D2C_ItemStats nStatId, struct Unicode *pwszDest), 0x53460)
//D2FUNC(D2CLIENT, NPCMENU_NPC_GetActiveNpcUnit, D2UnitStrc *, __cdecl, (), 0x53930)
//D2FUNC(D2CLIENT, NPCMENU_NpcGetVendorClassId, int32_t, __cdecl, (), 0x53950)
//D2FUNC(D2CLIENT, NPCMENU_NpcCloseInteraction_Dup1, void, __cdecl, (), 0x53960)
//D2FUNC(D2CLIENT, NPCMENU_NpcCloseInteraction, void, __cdecl, (), 0x53AD0)
//D2FUNC(D2CLIENT, NPCMENU_OnTalkClicked, int32_t, __cdecl, (), 0x53C40)
//D2FUNC(D2CLIENT, NPCMENU_SetInteractingNpc, void, __fastcall, (int32_t *pNpc), 0x53DC0)
//D2FUNC(D2CLIENT, NPCMENU_BuildTalkMenu, void, __fastcall, (int32_t nUnused, D2C_StringIndices nTitleTblIdx, int32_t nMenuEntryIdx), 0x53DF0)
//D2FUNC(D2CLIENT, NPCMENU_OnTalkMenuCancel, int32_t, __cdecl, (), 0x54110)
//D2FUNC(D2CLIENT, NPCMENU_OnTalkMenuClickAway, int32_t, __cdecl, (), 0x54160)
//D2FUNC(D2CLIENT, NPCMENU_ShowDescriptorSpeech, int32_t, __cdecl, (), 0x54300)
//D2FUNC(D2CLIENT, NPCMENU_PickRandomSpeechLine, void, __fastcall, (D2NpcMenuEntryStrc *pSpeechState), 0x544C0)
//D2FUNC(D2CLIENT, NPCMENU_OnSpeechDismissed, int32_t, __fastcall, (void *pArg, int32_t bDismissed, int32_t bByMouse), 0x545E0)
//D2FUNC(D2CLIENT, NPCMENU_PlayClassGreeting, int32_t, __cdecl, (), 0x54760)
//D2FUNC(D2CLIENT, NPCMENU_RerollAllSpeechLines, void, __cdecl, (), 0x54A40)
//D2FUNC(D2CLIENT, NPCMENU_ShowSpeechLine, void, __fastcall, (int32_t nSpeechLine, uint8_t bTakeLineFromDescriptor), 0x54A70)
//D2FUNC(D2CLIENT, NPCMENU_OnTalkLineClicked, int32_t, __cdecl, (), 0x54C30)
//D2FUNC(D2CLIENT, NPCMENU_OnJerhynMoreGossipClicked, int32_t, __cdecl, (), 0x54D30)
//D2FUNC(D2CLIENT, NPCMENU_OnCainCubeGossipClicked, int32_t, __cdecl, (), 0x54DB0)
//D2FUNC(D2CLIENT, NPCMENU_DrawHireMercenaryList, int32_t, __cdecl, (), 0x54E30)
//D2FUNC(D2CLIENT, NPCMENU_OnHireListBack, int32_t, __stdcall, (), 0x555B0)
//D2FUNC(D2CLIENT, NPCMENU_OnHireListConfirmClicked, int32_t, __cdecl, (), 0x555F0)
//D2FUNC(D2CLIENT, NPCMENU_ConfirmHireMercenary, void, __cdecl, (), 0x556D0)
//D2FUNC(D2CLIENT, NPCMENU_OnTransactionResult, void, __fastcall, (D2GSPacketSrv2A *pPacket), 0x55880)
//D2FUNC(D2CLIENT, NPCMENU_OnTradeClicked, int32_t, __cdecl, (), 0x55B80)
//D2FUNC(D2CLIENT, NPCMENU_OnGambleClicked, int32_t, __cdecl, (), 0x55D20)
//D2FUNC(D2CLIENT, NPCMENU_OnIdentifyClicked, int32_t, __cdecl, (), 0x55EB0)
//D2FUNC(D2CLIENT, NPCMENU_TickAndValidate, void, __cdecl, (), 0x56070)
//D2FUNC(D2CLIENT, NPCMENU_CloseAllDialogs, void, __cdecl, (), 0x56340)
//D2FUNC(D2CLIENT, NPCMENU_QuestUnlock_KashyaHire, int32_t, __cdecl, (), 0x563E0)
//D2FUNC(D2CLIENT, NPCMENU_QuestUnlock_WarrivGoEast, int32_t, __cdecl, (), 0x56410)
//D2FUNC(D2CLIENT, NPCMENU_QuestUnlock_MeshifSailEast, int32_t, __cdecl, (), 0x56440)
//D2FUNC(D2CLIENT, NPCMENU_QuestUnlock_CharsiImbue, int32_t, __cdecl, (), 0x56470)
//D2FUNC(D2CLIENT, NPCMENU_OnItemServiceClicked, int32_t, __cdecl, (), 0x564A0)
//D2FUNC(D2CLIENT, NPCMENU_QuestUnlock_LarzukSockets, int32_t, __cdecl, (), 0x56500)
//D2FUNC(D2CLIENT, NPCMENU_QuestUnlock_AnyaPersonalize, int32_t, __cdecl, (), 0x56530)
//D2FUNC(D2CLIENT, NPCMENU_QuestUnlock_QualKehkHire, int32_t, __cdecl, (), 0x56560)
//D2FUNC(D2CLIENT, NPCMENU_QuestUnlock_TyraelTravel, int32_t, __cdecl, (), 0x56590)
//D2FUNC(D2CLIENT, NPCMENU_BuildMainMenu, void, __cdecl, (), 0x565C0)
//D2FUNC(D2CLIENT, NPCMENU_OnMainMenuCancel, int32_t, __cdecl, (), 0x56BC0)
//D2FUNC(D2CLIENT, NPCMENU_OnMainMenuClickAway, int32_t, __cdecl, (), 0x56D50)
//D2FUNC(D2CLIENT, NPCMENU_OnResultContinueClicked, int32_t, __cdecl, (), 0x56EE0)
//D2FUNC(D2CLIENT, NPCMENU_SendQuestItemAction, void, __fastcall, (D2UnitGUID dwItemGUID), 0x570A0)
//D2FUNC(D2CLIENT, NPCMENU_OnGoEastClicked, int32_t, __cdecl, (), 0x570C0)
//D2FUNC(D2CLIENT, NPCMENU_OnSailEastClicked, int32_t, __cdecl, (), 0x57260)
//D2FUNC(D2CLIENT, NPCMENU_OnTravelToHarrogathClicked, int32_t, __cdecl, (), 0x57400)
//D2FUNC(D2CLIENT, NPCMENU_OnSailWestClicked, int32_t, __cdecl, (), 0x575A0)
//D2FUNC(D2CLIENT, NPCMENU_OnTravelWestClicked, int32_t, __cdecl, (), 0x57740)
//D2FUNC(D2CLIENT, NPCMENU_NpcFreeVendorInventory, void, __fastcall, (D2UnitStrc *pNpc), 0x578E0)
//D2FUNC(D2CLIENT, NPCMENU_NpcResyncVendorInventory, void, __fastcall, (uint32_t dwNpcGUID), 0x57950)
//D2FUNC(D2CLIENT, NPCMENU_ClearNpcAlertOverlay, void, __fastcall, (D2UnitStrc *pUnit), 0x57A00)
//D2FUNC(D2CLIENT, NPCMENU_NpcCloseInteraction_Dup2, void, __cdecl, (), 0x57A60)
//D2FUNC(D2CLIENT, NPCMENU_Init, void, __cdecl, (), 0x57BD0)
//D2FUNC(D2CLIENT, NPCMENU_Shutdown, void, __cdecl, (), 0x57C90)
//D2FUNC(D2CLIENT, NPCMENU_AbortOnPlayerAction, void, __fastcall, (D2UnitStrc *pUnit), 0x57D90)
//D2FUNC(D2CLIENT, NPCMENU_OnNpcCmdPacket, void, __fastcall, (D2GSPacketSrv50 *ptCmd), 0x57F40)
//D2FUNC(D2CLIENT, NPCMENU_QuestCopySecondaryQuestRecord, void, __fastcall, (uint8_t *pBuffer), 0x57FE0)
//D2FUNC(D2CLIENT, NPCMENU_OnInteractionStop, void, __fastcall, (uint8_t nUnitType, int32_t dwGuid), 0x58000)
//D2FUNC(D2CLIENT, NPCMENU_BeginInteraction, void, __fastcall, (uint8_t nUnitType, int32_t dwNpcGUID, uint8_t *pQuestData, uint8_t nAbortUnitType), 0x58250)
//D2FUNC(D2CLIENT, NPCMENU_SpeechNotYetPlayed, bool, __fastcall, (int16_t nTblStringId), 0x58940)
//D2FUNC(D2CLIENT, NPCMENU_IsSpeechOnlyNpc, bool, __fastcall, (D2C_MonsterIds nMonsterClassId), 0x58970)
//D2FUNC(D2CLIENT, NPCMENU_ChooseNextSpeech, int32_t, __fastcall, (int32_t nContext, int32_t bDismissed, int32_t bDismissedByMouse), 0x589E0)
//D2FUNC(D2CLIENT, NPCMENU_FindEntryIndexForNpc, int32_t, __cdecl, (), 0x58E40)
//D2FUNC(D2CLIENT, NPCMENU_OnResurrectMercClicked, int32_t, __cdecl, (), 0x58E80)
//D2FUNC(D2CLIENT, NPCMENU_SetRowCount2, void, __fastcall, (int32_t nRecord), 0x59010)
//D2FUNC(D2CLIENT, NPCMENU_SetRowCount3, void, __fastcall, (int32_t nRecord), 0x59040)
//D2FUNC(D2CLIENT, NPCMENU_SetRowCount4, void, __fastcall, (int32_t nRecord), 0x59070)
//D2FUNC(D2CLIENT, NPCMENU_EnableIdentifyRow, void, __fastcall, (int32_t nRecord), 0x590A0)
//D2FUNC(D2CLIENT, NPCMENU_BuildInterruptDialog, void, __cdecl, (), 0x590D0)
//D2FUNC(D2CLIENT, NPCMENU_OnInterruptCancel, int32_t, __cdecl, (), 0x593D0)
//D2FUNC(D2CLIENT, NPCMENU_OnInterruptClickAway, int32_t, __cdecl, (), 0x59570)
//D2FUNC(D2CLIENT, NPCMENU_OnInterruptNewsClicked, int32_t, __cdecl, (), 0x59710)
//D2FUNC(D2CLIENT, NPCMENU_OpenMainMenu, void, __fastcall, (D2UnitStrc *pNpc, int32_t bAlreadyInteracting), 0x598D0)
//D2FUNC(D2CLIENT, NPCMENU_ToggleResurrectMercRow, int32_t, __fastcall, (int32_t nDescriptorIndex, int32_t bAdd), 0x59CB0)
//D2FUNC(D2CLIENT, j_INV_NpcShopRequestPageRebuild, void, __fastcall, (D2UnitStrc *pNpc), 0x59E50)
//D2FUNC(D2CLIENT, NPCMENU_NpcShopCloseAndEndInteraction, void, __cdecl, (), 0x59E60)
//D2FUNC(D2CLIENT, NPCMENU_EndInteractionByUiState, void, __cdecl, (), 0x59FE0)
//D2FUNC(D2CLIENT, NPCMENU_SetUnitFlag, void, __fastcall, (D2UnitStrc *pUnit, uint32_t nMask, int32_t bSet), 0x5A230)
//D2FUNC(D2CLIENT, NPCMENU_BeginTransaction, BOOL, __fastcall, (D2UnitStrc *pPlayer, D2UnitStrc *pItem, int32_t bPlayerOwnsItem, int32_t nMouseX, int32_t nMouseY, int32_t nMouseKeyFlags, int32_t bQuickTransaction, int32_t bRepairAll), 0x5A260)
//D2FUNC(D2CLIENT, NPCMENU_SendTransactionAndWait, void, __fastcall, (int32_t bSkippedVerify, int32_t nMouseKeyFlags), 0x5A6E0)
//D2FUNC(D2CLIENT, NPCMENU_AckItemMessage, void, __fastcall, (uint32_t dwItemGUID), 0x5AAA0)
//D2FUNC(D2CLIENT, NPCMENU_AckItemAction_ToInventoryGrid, void, __fastcall, (uint32_t dwItemGUID), 0x5AB80)
//D2FUNC(D2CLIENT, NPCMENU_AckItemAction_RemoveFromBodyLoc, void, __fastcall, (uint32_t dwItemGUID), 0x5AC60)
//D2FUNC(D2CLIENT, NPCMENU_AckItemAction_InsertIntoBodyLoc, void, __fastcall, (uint32_t dwItemGUID), 0x5AD40)
//D2FUNC(D2CLIENT, NPCMENU_AckItemAction_ToStorePage, void, __fastcall, (uint32_t dwItemGUID), 0x5AE20)
//D2FUNC(D2CLIENT, NPCMENU_AckItemAction_SetInvPage, void, __fastcall, (uint32_t dwItemGUID), 0x5AF00)
//D2FUNC(D2CLIENT, NPCMENU_ClearSelectionIfStoreItem, void, __fastcall, (D2UnitGUID dwItemGUID), 0x5AFE0)
//D2FUNC(D2CLIENT, NPCMENU_ShowTransactionResult, void, __cdecl, (), 0x5B030)
//D2FUNC(D2CLIENT, NPCMENU_TransactionCost, BOOL, __fastcall, (D2UnitStrc *pItem, BOOL bIsOwned, int32_t *pnCost, struct Unicode *wszOut, int32_t nSize), 0x5B200)
//D2FUNC(D2CLIENT, NPCMENU_IsUiStateActive, BOOL, __cdecl, (), 0x5B6D0)
//D2FUNC(D2CLIENT, NPCMENU_MonsterSetDataFlag, void, __fastcall, (D2UnitStrc *pUnit, int16_t wFlagMask, int32_t bSet), 0x5B6E0)
//D2FUNC(D2CLIENT, NPCMENU_BuildConfirmDialog, void, __cdecl, (), 0x5B710)
//D2FUNC(D2CLIENT, NPCMENU_OnGraphicActionVerified, int32_t, __cdecl, (), 0x5BA30)
//D2FUNC(D2CLIENT, NPCMENU_OnConfirmDialogDismissed, int32_t, __cdecl, (), 0x5BAC0)
//D2FUNC(D2CLIENT, PLAYER_ClearQueuedCommand, void, __fastcall, (D2UnitStrc *pPlayer), 0x5BB10)
//D2FUNC(D2CLIENT, NPCMENU_GetInteractingNpc, int32_t, __fastcall, (D2UnitGUID *pNpcGUID), 0x5BB50)
//D2FUNC(D2CLIENT, NPCMENU_NpcEndInteraction, void, __fastcall, (uint32_t dwNpcGUID), 0x5BB60)
//D2FUNC(D2CLIENT, NPCMENU_AddHirelingOffer, void, __fastcall, (D2GSPacketSrv4E *pPacket), 0x5BCC0)
//D2FUNC(D2CLIENT, NPCMENU_ClearHirelingOffers, void, __cdecl, (), 0x5BD00)
//D2FUNC(D2CLIENT, NPCMENU_QuestGetQuestRecord, D2BitBufferStrc *, __cdecl, (), 0x5BD20)
//D2FUNC(D2CLIENT, NPCMENU_QuestGetQuestRecordSecondary, D2BitBufferStrc *, __cdecl, (), 0x5BD30)
//D2FUNC(D2CLIENT, NPCMENU_Reset, void, __cdecl, (), 0x5BD40)
//D2FUNC(D2CLIENT, NPCMENU_RefreshHireIconsForOffer, void, __fastcall, (int16_t wMercName), 0x5BE30)
//D2FUNC(D2CLIENT, NPCMENU_NoteOpenedEunuchObject, void, __fastcall, (D2UnitStrc *pObject), 0x5BE50)
//D2FUNC(D2CLIENT, NPCMENU_ShowNpcAlert, void, __fastcall, (D2GSPacketSrv8A *pPacket), 0x5BE70)
//D2FUNC(D2CLIENT, NPCMENU_EndInteraction, void, __fastcall, (uint32_t dwNpcGUID), 0x5BF80)
//D2FUNC(D2CLIENT, NPCMENU_GetInteractingNpcUnit, BOOL, __cdecl, (), 0x5C0B0)
//D2FUNC(D2CLIENT, NPCMENU_IsAnyDialogOpen, BOOL, __cdecl, (), 0x5C0E0)
//D2FUNC(D2CLIENT, NPCMENU_IsVendorGamble, BOOL, __fastcall, (), 0x5C130)
//D2FUNC(D2CLIENT, NPCMENU_MarkEntriesFromPacket91, void, __fastcall, (uint16_t *pPacket), 0x5C140)
//D2FUNC(D2CLIENT, NPCMENU_SetResurrectMercOffer, void, __fastcall, (D2C_StringIndices wNameTblId, int32_t nCost), 0x5C190)
//D2FUNC(D2CLIENT, NPCMENU_TeardownHireList, void, __cdecl, (), 0x5C1F0)
//D2FUNC(D2CLIENT, CONFIG_OnDefaultButton, void, __cdecl, (), 0x5C260)
//D2FUNC(D2CLIENT, CONFIG_OnAcceptButton, void, __cdecl, (), 0x5C270)
//D2FUNC(D2CLIENT, CONFIG_OnCancelButton, void, __cdecl, (), 0x5C290)
//D2FUNC(D2CLIENT, CONFIG_InitControlsScreen, void, __cdecl, (), 0x5C2C0)
//D2FUNC(D2CLIENT, CONFIG_SelectControlsList, void, __cdecl, (), 0x5C310)
//D2FUNC(D2CLIENT, CONFIG_LayoutControlsButtons, void, __cdecl, (), 0x5C3F0)
//D2FUNC(D2CLIENT, CONFIG_ScrollList, void, __fastcall, (int32_t nDelta), 0x5C4C0)
//D2FUNC(D2CLIENT, CONFIG_UnloadTextSliderImage, void, __cdecl, (), 0x5C590)
//D2FUNC(D2CLIENT, CONFIG_SuspendKeyCommands, void, __fastcall, (int32_t bSuspend), 0x5C5B0)
//D2FUNC(D2CLIENT, CONFIG_DrawKeyBindingsPanel, void, __cdecl, (), 0x5C620)
//D2FUNC(D2CLIENT, CONFIG_UpdateControlsMouseState, void, __fastcall, (int32_t nMouseX, int32_t nMouseY), 0x5CE10)
//D2FUNC(D2CLIENT, CONFIG_MoveSelection, void, __fastcall, (int32_t nDelta), 0x5D470)
//D2FUNC(D2CLIENT, CONFIG_HitTestButton, int32_t, __fastcall, (int32_t nMouseX, int32_t nMouseY), 0x5D520)
//D2FUNC(D2CLIENT, CONFIG_OnMouseDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x5D550)
//D2FUNC(D2CLIENT, CONFIG_OnMouseUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x5D5A0)
//D2FUNC(D2CLIENT, CONFIG_HitTestRow, int32_t, __fastcall, (int32_t nMouseY), 0x5D7E0)
//D2FUNC(D2CLIENT, CONFIG_OnMouseWheel, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x5D830)
//D2FUNC(D2CLIENT, CONFIG_OnArrowUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x5D8A0)
//D2FUNC(D2CLIENT, CONFIG_OnArrowDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x5D910)
//D2FUNC(D2CLIENT, CONFIG_OnArrowLeft, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x5D980)
//D2FUNC(D2CLIENT, CONFIG_OnArrowRight, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x5D9E0)
//D2FUNC(D2CLIENT, CONFIG_OnEnterBeginCapture, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x5DA40)
//D2FUNC(D2CLIENT, CONFIG_OnEraseKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x5DB50)
//D2FUNC(D2CLIENT, CONFIG_OnDismissKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x5DBD0)
//D2FUNC(D2CLIENT, CONFIG_CaptureMouseButton, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x5DC60)
//D2FUNC(D2CLIENT, CONFIG_CaptureKeyDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x5DD30)
//D2FUNC(D2CLIENT, CONFIG_CaptureKeyUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x5DF10)
//D2FUNC(D2CLIENT, CONFIG_CaptureMiddleButton, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x5E040)
//D2FUNC(D2CLIENT, CONFIG_CaptureXButton, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x5E140)
//D2FUNC(D2CLIENT, CONFIG_CaptureMouseWheel, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x5E250)
//D2FUNC(D2CLIENT, PANEL_LoadImages, void, __cdecl, (), 0x5E370)
//D2FUNC(D2CLIENT, PANEL_UnloadImages, void, __cdecl, (), 0x5E4D0)
//D2FUNC(D2CLIENT, PANEL_DrawHostileIndicator, void, __cdecl, (), 0x5E650)
//D2FUNC(D2CLIENT, PANEL_SpellSelDrawSkillButton, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, int32_t nXPos, int32_t nYPos, int32_t bLeftHand), 0x5E730)
//D2FUNC(D2CLIENT, PANEL_DrawSkillButtons, void, __cdecl, (), 0x5E850)
//D2FUNC(D2CLIENT, PANEL_CacheStaminaColourRed, void, __cdecl, (), 0x5E930)
//D2FUNC(D2CLIENT, PANEL_CacheStaminaColourAmber, void, __cdecl, (), 0x5E950)
//D2FUNC(D2CLIENT, PANEL_CacheStaminaColourBlue, uint8_t, __cdecl, (), 0x5E970)
//D2FUNC(D2CLIENT, PANEL_CacheHighlightColours, void, __cdecl, (), 0x5E990)
//D2FUNC(D2CLIENT, PANEL_InvHintPhaseSuppressed, int32_t, __fastcall, (), 0x5E9E0)
//D2FUNC(D2CLIENT, PANEL_BeltGetRowCount, int32_t, __cdecl, (), 0x5E9F0)
//D2FUNC(D2CLIENT, PANEL_DrawQuantityWarningIcons, void, __cdecl, (), 0x5EA40)
//D2FUNC(D2CLIENT, PANEL_DrawStatusWarningIcons, void, __cdecl, (), 0x5EE70)
//D2FUNC(D2CLIENT, PANEL_DrawOrbHoverText, void, __cdecl, (), 0x5F0B0)
//D2FUNC(D2CLIENT, PANEL_GetSmoothedStatValue, int32_t, __fastcall, (int32_t nStatId), 0x5F2D0)
//D2FUNC(D2CLIENT, PANEL_Draw800BorderFrame, void, __cdecl, (), 0x5F480)
//D2FUNC(D2CLIENT, PANEL_DrawHealthGlobe, void, __cdecl, (), 0x5F9D0)
//D2FUNC(D2CLIENT, PANEL_DrawManaGlobe, void, __cdecl, (), 0x5FB60)
//D2FUNC(D2CLIENT, PANEL_DrawExperienceBar, void, __cdecl, (), 0x5FCA0)
//D2FUNC(D2CLIENT, PANEL_DrawRunHint, void, __cdecl, (), 0x5FE60)
//D2FUNC(D2CLIENT, PANEL_IsPointOverRunButton, BOOL, __fastcall, (int32_t nXPos, int32_t nYPos), 0x5FF90)
//D2FUNC(D2CLIENT, PANEL_DrawStaminaBar, void, __cdecl, (), 0x5FFD0)
//D2FUNC(D2CLIENT, PANEL_DrawMiniPanelToggleButton, void, __cdecl, (), 0x601B0)
//D2FUNC(D2CLIENT, PANEL_DrawBelt, void, __cdecl, (), 0x602B0)
//D2FUNC(D2CLIENT, PANEL_DrawBeltItemHint, void, __cdecl, (), 0x60990)
//D2FUNC(D2CLIENT, PANEL_ArmLevelChangeTimer, int32_t, __cdecl, (), 0x60C10)
//D2FUNC(D2CLIENT, PANEL_DrawMinipanelToggleHint, void, __cdecl, (), 0x60C40)
//D2FUNC(D2CLIENT, PANEL_GameOnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x60CD0)
//D2FUNC(D2CLIENT, PANEL_GameOnLButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x60E20)
//D2FUNC(D2CLIENT, PANEL_BeltOnClickAtPixel, void, __fastcall, (D2UnitStrc *pPlayer, D2InventoryStrc *pInventory, int32_t nX, int32_t nY), 0x612E0)
//D2FUNC(D2CLIENT, PANEL_GameOnMouseMove, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x61430)
//D2FUNC(D2CLIENT, PANEL_BeltUpdateHoveredItem, void, __fastcall, (D2UnitStrc *pPlayer, D2InventoryStrc *pInventory, int32_t nX, int32_t nY), 0x61590)
//D2FUNC(D2CLIENT, PANEL_GameOnRButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x61730)
//D2FUNC(D2CLIENT, PANEL_BeltUseItemInSlot, void, __fastcall, (D2UnitStrc *pUnit, D2InventoryStrc *pInventory, int32_t nSlotId, int32_t bShiftHeld), 0x618A0)
//D2FUNC(D2CLIENT, PANEL_BeltOnDismissKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x61A00)
//D2FUNC(D2CLIENT, PANEL_BeltUseColumn0, void, __cdecl, (), 0x61A10)
//D2FUNC(D2CLIENT, PANEL_BeltUseColumn1, void, __cdecl, (), 0x61A50)
//D2FUNC(D2CLIENT, PANEL_BeltUseColumn2, void, __cdecl, (), 0x61A90)
//D2FUNC(D2CLIENT, PANEL_BeltUseColumn3, void, __cdecl, (), 0x61AD0)
//D2FUNC(D2CLIENT, PANEL_BeltSetColumnHasItem, void, __fastcall, (uint32_t nColumn, uint8_t bHasItem), 0x61B10)
//D2FUNC(D2CLIENT, PANEL_BeltResetHoverState, void, __cdecl, (), 0x61B20)
//D2FUNC(D2CLIENT, PANEL_BeltHitTestPanel, int32_t, __fastcall, (SMSGHANDLER_PARAMS *pMsg), 0x61B50)
//D2FUNC(D2CLIENT, PANEL_BeltClearHoverItem, void, __cdecl, (), 0x61C10)
//D2FUNC(D2CLIENT, PANEL_BeltShowPanel, void, __cdecl, (), 0x61C20)
//D2FUNC(D2CLIENT, PANEL_SetBoxFillColorRed, void, __cdecl, (), 0x61C30)
//D2FUNC(D2CLIENT, PANEL_SetBoxFillColorBlack, void, __cdecl, (), 0x61C50)
//D2FUNC(D2CLIENT, PARTY_FreeRowList, void, __cdecl, (), 0x61C70)
//D2FUNC(D2CLIENT, PARTY_BuildRowList, void, __cdecl, (), 0x61CB0)
//D2FUNC(D2CLIENT, PARTY_DrawPanel, void, __cdecl, (), 0x61F10)
//D2FUNC(D2CLIENT, PARTY_DrawCel, void, __fastcall, (D2CellFileStrc *hImage, int32_t nX, int32_t nY, int32_t nFrame), 0x62330)
//D2FUNC(D2CLIENT, PARTY_DrawLevelLabel, void, __fastcall, (int32_t nLevel, int32_t nY, int32_t nColor), 0x62390)
//D2FUNC(D2CLIENT, PARTY_DrawNameLabel, void, __fastcall, (const char *pszName, int32_t nY, int32_t nColor, int32_t bInParty), 0x62480)
//D2FUNC(D2CLIENT, PARTY_DrawClassLabel, void, __fastcall, (int32_t nCharClass, int32_t nY, int32_t nColor), 0x625B0)
//D2FUNC(D2CLIENT, PARTY_DrawButtonLabel, void, __fastcall, (D2RosterUnitStrc *pUnusedEntry, D2C_PartyButtons nButtonKind, int32_t nY, int32_t bSameParty, int16_t nPartyId), 0x62640)
//D2FUNC(D2CLIENT, PARTY_DrawRowButtons, void, __fastcall, (D2ClientPartyRowStrc *pRow, int32_t nUnusedY), 0x62700)
//D2FUNC(D2CLIENT, PARTY_DrawAreaLabel, void, __fastcall, (D2ClientPartyRowStrc *pRow, int32_t nY, int32_t nColor), 0x628A0)
//D2FUNC(D2CLIENT, PARTY_DrawPartyGroupDividers, void, __cdecl, (), 0x629D0)
//D2FUNC(D2CLIENT, PARTY_IsMouseOverRect, BOOL, __fastcall, (int32_t nX, int32_t nY, int32_t nWidth, int32_t nHeight), 0x62AC0)
//D2FUNC(D2CLIENT, PARTY_DrawControlButtonHints, void, __cdecl, (), 0x62B20)
//D2FUNC(D2CLIENT, PARTY_DrawHintBackdrop, void, __fastcall, (D2C_StringIndices nTblLine1, D2C_StringIndices nTblLine2, int32_t nCenterX, int32_t bTwoLines), 0x63010)
//D2FUNC(D2CLIENT, PARTY_DrawHoverHint, void, __cdecl, (), 0x63110)
//D2FUNC(D2CLIENT, PARTY_DrawHintLines, void, __fastcall, (D2C_StringIndices nTblLine1, D2C_StringIndices nTblLine2, int32_t nCenterX, int32_t nUnused, int32_t nY1, int32_t nY2, int32_t nColor), 0x636C0)
//D2FUNC(D2CLIENT, PARTY_OnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x63870)
//D2FUNC(D2CLIENT, PARTY_OnLButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x63AC0)
//D2FUNC(D2CLIENT, PARTY_LoadImages, void, __cdecl, (), 0x63FA0)
//D2FUNC(D2CLIENT, PARTY_UnloadImages, void, __cdecl, (), 0x63FF0)
//D2FUNC(D2CLIENT, QUESTLOG_QuestResetAllState, void, __cdecl, (), 0x64140)
//D2FUNC(D2CLIENT, j_QUESTLOG_QuestUnloadPanelImages, void, __cdecl, (), 0x64210)
//D2FUNC(D2CLIENT, QUESTLOG_QuestUnloadPanelImages, void, __cdecl, (), 0x64220)
//D2FUNC(D2CLIENT, QUESTLOG_Draw, void, __cdecl, (), 0x642D0)
//D2FUNC(D2CLIENT, QUESTLOG_BuildQuestDisplayInfo, void, __fastcall, (D2QuestLogEntryStrc *pEntry), 0x64D50)
//D2FUNC(D2CLIENT, QUESTLOG_SelectActPage, void, __fastcall, (int32_t nAct, int32_t bClearSelections), 0x656D0)
//D2FUNC(D2CLIENT, QUESTLOG_LoadPanelImages, void, __cdecl, (), 0x658D0)
//D2FUNC(D2CLIENT, QUESTLOG_QuestScreenOnMouseDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x65A30)
//D2FUNC(D2CLIENT, QUESTLOG_SetActTab, void, __fastcall, (int32_t nAct), 0x65D80)
//D2FUNC(D2CLIENT, QUESTLOG_QuestScreenOnMouseUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x65E40)
//D2FUNC(D2CLIENT, QUESTLOG_OnSpeechFinished, int32_t, __fastcall, (uint32_t nContext, uint32_t nDismissed, uint32_t bDismissedByMouse), 0x660A0)
//D2FUNC(D2CLIENT, QUESTLOG_QuestScreenOnDismissKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x660C0)
//D2FUNC(D2CLIENT, QUESTLOG_TogglePanel, void, __fastcall, (int32_t bCheckMousePos), 0x66120)
//D2FUNC(D2CLIENT, QUESTLOG_ApplyQuestListPacket, void, __fastcall, (D2GSPacketSrv52 *pPacket), 0x66230)
//D2FUNC(D2CLIENT, QUESTLOG_QuestOnQuestLogStateCmd, void, __fastcall, (uint8_t *ptCmd), 0x66280)
//D2FUNC(D2CLIENT, QUESTLOG_AcknowledgeAndClose, void, __cdecl, (), 0x662B0)
//D2FUNC(D2CLIENT, QUESTLOG_OnMouseDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x66320)
//D2FUNC(D2CLIENT, QUESTLOG_OnMouseUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x66480)
//D2FUNC(D2CLIENT, QUESTLOG_DrawFrameAndTitle, void, __cdecl, (), 0x665F0)
//D2FUNC(D2CLIENT, QUESTLOG_QuestOnQuestStatePacket, void, __fastcall, (int32_t nUnused, D2GSPacketSrv5D *pPacket), 0x66900)
//D2FUNC(D2CLIENT, QUESTLOG_QuestMakeCainUntargetable, void, __fastcall, (D2UnitStrc *pUnit), 0x66CE0)
//D2FUNC(D2CLIENT, QUESTLOG_QuestMakeFrozenAnyaUntargetable, void, __fastcall, (D2UnitStrc *pUnit), 0x66D00)
//D2FUNC(D2CLIENT, QUESTLOG_ClearPanelState, void, __cdecl, (), 0x66D20)
//D2FUNC(D2CLIENT, QUESTLOG_ClearEntryState, void, __cdecl, (), 0x66D40)
//D2FUNC(D2CLIENT, QUESTLOG_ClearActSelection, void, __cdecl, (), 0x66D60)
//D2FUNC(D2CLIENT, QUEST_ShouldPlayAreaEntrySpiel, BOOL, __fastcall, (D2Quests nQuestKey), 0x66D80)
//D2FUNC(D2CLIENT, QUEST_PreloadDurielCof, void, __cdecl, (), 0x66E70)
//D2FUNC(D2CLIENT, QUEST_PreloadAncientStatueCof, void, __cdecl, (), 0x66E90)
//D2FUNC(D2CLIENT, QUEST_ArmZooEffect, void, __fastcall, (int32_t nValue), 0x66ED0)
//D2FUNC(D2CLIENT, QUEST_IsZooArmed, uint8_t, __cdecl, (), 0x66EE0)
//D2FUNC(D2CLIENT, QUEST_ZooChickensPresent, bool, __cdecl, (), 0x66EF0)
//D2FUNC(D2CLIENT, QUEST_SpawnZooChickens, void, __fastcall, (int32_t nXPos, int32_t nYPos), 0x66F20)
//D2FUNC(D2CLIENT, ESCMENU_GotoOptionsPage, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg), 0x66FB0)
//D2FUNC(D2CLIENT, ESCMENU_OnExitGame, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg), 0x66FC0)
//D2FUNC(D2CLIENT, ESCMENU_OnReturnToGame, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg), 0x67030)
//D2FUNC(D2CLIENT, j_SOUNDCHAN_IsSystemReady_2, int32_t, __cdecl, (), 0x67080)
//D2FUNC(D2CLIENT, ESCMENU_GotoSoundPage, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg), 0x67090)
//D2FUNC(D2CLIENT, ESCMENU_GotoVideoPage, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg), 0x670A0)
//D2FUNC(D2CLIENT, ESCMENU_GotoAutomapPage, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg), 0x670B0)
//D2FUNC(D2CLIENT, ESCMENU_OpenConfigPanel, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg), 0x670C0)
//D2FUNC(D2CLIENT, ESCMENU_SoundVolumeOnChange, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x670F0)
//D2FUNC(D2CLIENT, ESCMENU_SoundVolumeRefresh, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x67130)
//D2FUNC(D2CLIENT, ESCMENU_MusicVolumeOnChange, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x67180)
//D2FUNC(D2CLIENT, ESCMENU_MusicVolumeRefresh, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x671C0)
//D2FUNC(D2CLIENT, j_SOUNDCHAN_Get3DEnabled, int32_t, __cdecl, (), 0x67210)
//D2FUNC(D2CLIENT, j_SOUNDCHAN_Toggle3dSound, int32_t, __cdecl, (), 0x67220)
//D2FUNC(D2CLIENT, ESCMENU_Sound3DRefresh, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x67230)
//D2FUNC(D2CLIENT, j_SOUNDCHAN_Is3DEnabled, BOOL, __cdecl, (), 0x67240)
//D2FUNC(D2CLIENT, ESCMENU_Sound3DBiasOnChange, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x67250)
//D2FUNC(D2CLIENT, ESCMENU_Sound3DBiasRefresh, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x67290)
//D2FUNC(D2CLIENT, j_SOUNDCHAN_GetEaxEnabled, int32_t, __cdecl, (), 0x672E0)
//D2FUNC(D2CLIENT, j_SOUNDCHAN_ToggleEaxEffects, int32_t, __cdecl, (), 0x672F0)
//D2FUNC(D2CLIENT, ESCMENU_SoundEaxRefresh, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x67300)
//D2FUNC(D2CLIENT, ESCMENU_NpcSpeechOnChange, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x67310)
//D2FUNC(D2CLIENT, ESCMENU_NpcSpeechRefresh, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x67370)
//D2FUNC(D2CLIENT, ESCMENU_LightQualityOnChange, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x673E0)
//D2FUNC(D2CLIENT, ESCMENU_LightQualityRefresh, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x67440)
//D2FUNC(D2CLIENT, ESCMENU_BlendShadowOnChange, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x674A0)
//D2FUNC(D2CLIENT, ESCMENU_BlendShadowRefresh, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x674D0)
//D2FUNC(D2CLIENT, ESCMENU_ResolutionOnChange, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x67500)
//D2FUNC(D2CLIENT, ESCMENU_ResolutionRefresh, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x67540)
//D2FUNC(D2CLIENT, j_D2gfx_10007, int32_t, __stdcall, (), 0x67570)
//D2FUNC(D2CLIENT, ESCMENU_PerspectiveOnChange, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x67580)
//D2FUNC(D2CLIENT, ESCMENU_PerspectiveRefresh, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x675D0)
//D2FUNC(D2CLIENT, j_D2gfx_10019, int32_t, __stdcall, (), 0x67640)
//D2FUNC(D2CLIENT, ESCMENU_GammaOnChange, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x67650)
//D2FUNC(D2CLIENT, ESCMENU_GammaRefresh, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x676C0)
//D2FUNC(D2CLIENT, ESCMENU_ContrastOnChange, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x67780)
//D2FUNC(D2CLIENT, ESCMENU_ContrastRefresh, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x677F0)
//D2FUNC(D2CLIENT, ESCMENU_AutomapModeOnChange, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x678B0)
//D2FUNC(D2CLIENT, ESCMENU_AutomapModeRefresh, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x678F0)
//D2FUNC(D2CLIENT, ESCMENU_AutomapFadeOnChange, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x67930)
//D2FUNC(D2CLIENT, ESCMENU_AutomapFadeRefresh, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x67970)
//D2FUNC(D2CLIENT, ESCMENU_AutomapCenterOnChange, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x679B0)
//D2FUNC(D2CLIENT, ESCMENU_AutomapCenterRefresh, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x679C0)
//D2FUNC(D2CLIENT, ESCMENU_AutomapPartyOnChange, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x679D0)
//D2FUNC(D2CLIENT, ESCMENU_AutomapPartyRefresh, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x679E0)
//D2FUNC(D2CLIENT, j_AUTOMAP_GetShowParty, int32_t, __cdecl, (), 0x679F0)
//D2FUNC(D2CLIENT, ESCMENU_AutomapPartyNamesOnChange, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x67A00)
//D2FUNC(D2CLIENT, ESCMENU_AutomapPartyNamesRefresh, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, int32_t nArg), 0x67A10)
//D2FUNC(D2CLIENT, ESCMENU_GotoMainPage, void, __fastcall, (D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg), 0x67A40)
//D2FUNC(D2CLIENT, ESCMENU_OnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x67A50)
//D2FUNC(D2CLIENT, ESCMENU_HitTestRow, int32_t, __fastcall, (int32_t nMouseY), 0x67AA0)
//D2FUNC(D2CLIENT, ESCMENU_DragSlider, void, __fastcall, (int32_t nMouseX, int32_t nMouseY), 0x67B30)
//D2FUNC(D2CLIENT, ESCMENU_OnLButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x67CC0)
//D2FUNC(D2CLIENT, ESCMENU_ActivateSelectedRow, void, __fastcall, (SMSGHANDLER_PARAMS *pMsg), 0x67D20)
//D2FUNC(D2CLIENT, ESCMENU_OnDownKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x67E20)
//D2FUNC(D2CLIENT, ESCMENU_OnUpKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x67EA0)
//D2FUNC(D2CLIENT, ESCMENU_OnLeftKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x67F20)
//D2FUNC(D2CLIENT, ESCMENU_OnRightKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x68060)
//D2FUNC(D2CLIENT, ESCMENU_OnEnter, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x681A0)
//D2FUNC(D2CLIENT, ESCMENU_LoadPageImages, void, __fastcall, (D2ClientEscMenuInfoStrc *ptMenuInfo, D2ClientEscMenuItemStrc *ptMenuItem), 0x681D0)
//D2FUNC(D2CLIENT, ESCMENU_LoadImages, void, __cdecl, (), 0x683C0)
//D2FUNC(D2CLIENT, ESCMENU_UnloadPageImages, void, __fastcall, (D2ClientEscMenuInfoStrc *ptMenuInfo, D2ClientEscMenuItemStrc *ptMenuItem), 0x68540)
//D2FUNC(D2CLIENT, ESCMENU_UnloadImages, void, __cdecl, (), 0x68640)
//D2FUNC(D2CLIENT, ESCMENU_SetPage, void, __fastcall, (int32_t bCloseUiWindows, int32_t nPage), 0x68700)
//D2FUNC(D2CLIENT, ESCMENU_CloseMenu, void, __fastcall, (int32_t bRestorePanels), 0x68860)
//D2FUNC(D2CLIENT, ESCMENU_Draw, void, __cdecl, (), 0x688B0)
//D2FUNC(D2CLIENT, MINIPANEL_LoadAlwaysRunFromRegistry, void, __cdecl, (), 0x68D90)
//D2FUNC(D2CLIENT, MINIPANEL_SaveAlwaysRunToRegistry, int32_t, __cdecl, (), 0x68DC0)
//D2FUNC(D2CLIENT, MINIPANEL_Draw, void, __cdecl, (), 0x68DE0)
//D2FUNC(D2CLIENT, MINIPANEL_DrawBackgroundSegment, void, __fastcall, (int32_t nSegment), 0x69180)
//D2FUNC(D2CLIENT, MINIPANEL_DrawButtonCel, void, __fastcall, (int32_t nFrame, int32_t nXPos, int32_t nYPos, int32_t nUnused), 0x69270)
//D2FUNC(D2CLIENT, MINIPANEL_LayOutButtons, void, __fastcall, (int32_t nSegment), 0x692B0)
//D2FUNC(D2CLIENT, MINIPANEL_DrawButtons, void, __fastcall, (int32_t nSegment), 0x69390)
//D2FUNC(D2CLIENT, MINIPANEL_DrawButtonHint, void, __fastcall, (int32_t nLayout), 0x69460)
//D2FUNC(D2CLIENT, MINIPANEL_OnLButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x69730)
//D2FUNC(D2CLIENT, MINIPANEL_RunButtonAction, void, __fastcall, (int32_t nButton), 0x698D0)
//D2FUNC(D2CLIENT, MINIPANEL_OnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x699F0)
//D2FUNC(D2CLIENT, MINIPANEL_LoadImages, void, __cdecl, (), 0x69B80)
//D2FUNC(D2CLIENT, MINIPANEL_SaveAndUnload, void, __cdecl, (), 0x69BF0)
//D2FUNC(D2CLIENT, MINIPANEL_IsPointOverMiniPanelRow, BOOL, __fastcall, (int32_t nXPos, int32_t nYPos), 0x69C50)
//D2FUNC(D2CLIENT, MINIPANEL_LoadResolutionFromRegistry, void, __cdecl, (), 0x69C90)
//D2FUNC(D2CLIENT, MINIPANEL_WaypointIsMouseOverMiniPanel, BOOL, __cdecl, (), 0x69CC0)
//D2FUNC(D2CLIENT, SHOWITEMS_FitLabelRect, int32_t, __fastcall, (int32_t *pnLeft, int32_t *pnTop, int32_t *pnRight, int32_t *pnBottom, int32_t nAnchor, int32_t nMaxRight, int32_t nMinLeft), 0x69D80)
//D2FUNC(D2CLIENT, SHOWITEMS_DrawGroundItemLabels, void, __cdecl, (), 0x69F60)
//D2FUNC(D2CLIENT, SKILLS_GetSkillDescStringIndex, int16_t, __fastcall, (D2C_Skills nSkillId, int32_t nStringField), 0x6A440)
//D2FUNC(D2CLIENT, SKILLDESC_AttFunc1, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnAttackRatingA, int32_t *pnColorA, int32_t *pnAttackRatingB, int32_t *pnColorB), 0x6A4A0)
//D2FUNC(D2CLIENT, SKILLDESC_ComputeAttackRating, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t *pnAttackRating, int32_t *pnColor, int32_t nSkillLevel, D2SkillStrc *pSkill, D2UnitStrc *pWeapon), 0x6A4E0)
//D2FUNC(D2CLIENT, SKILLDESC_AttFunc5, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnAttackRatingA, int32_t *pnColorA, int32_t *pnAttackRatingB, int32_t *pnColorB), 0x6A610)
//D2FUNC(D2CLIENT, SKILLDESC_AttFunc2, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnAttackRatingA, int32_t *pnColorA, int32_t *pnAttackRatingB, int32_t *pnColorB), 0x6A730)
//D2FUNC(D2CLIENT, SKILLDESC_AttackRatingCallback, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t *pnValue, int32_t *pnColor, int32_t nUnusedA, int32_t nUnusedB, int32_t nSkillLevel, D2SkillStrc *pSkill, int32_t nUnusedC, D2UnitStrc *pWeapon), 0x6A7B0)
//D2FUNC(D2CLIENT, SKILLDESC_ForEachWieldedWeapon, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t *pnValueA, int32_t *pnColorA, int32_t nArgA, int32_t *pnValueB, int32_t *pnColorB, int32_t nArgB, int32_t nShared, void *pfnCompute, int32_t nExtraA, int32_t nExtraB), 0x6A7D0)
//D2FUNC(D2CLIENT, SKILLDESC_PrepareOffHandWeapon, D2UnitStrc *, __fastcall, (D2UnitStrc *pUnit), 0x6A950)
//D2FUNC(D2CLIENT, SKILLDESC_PrepareMainHandWeapon, D2UnitStrc *, __fastcall, (D2UnitStrc *pUnit), 0x6AA00)
//D2FUNC(D2CLIENT, SKILLDESC_AttFunc3, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnAttackRatingA, int32_t *pnColorA, int32_t *pnAttackRatingB, int32_t *pnColorB), 0x6AAB0)
//D2FUNC(D2CLIENT, SKILLDESC_AttFunc4, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnAttackRatingA, int32_t *pnColorA, int32_t *pnAttackRatingB, int32_t *pnColorB), 0x6ABC0)
//D2FUNC(D2CLIENT, SKILLDESC_DrawTwoLines, void, __fastcall, (Unicode *pwszLine1, Unicode *pwszLine2, int32_t nX, int32_t nY, int32_t nRight, int32_t nColor1, int32_t nColor2), 0x6AC10)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc6, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6ACE0)
//D2FUNC(D2CLIENT, SKILLDESC_FormatInt, int32_t, __fastcall, (struct Unicode *pwszOut, int32_t nValue, int32_t bSigned), 0x6B0F0)
//D2FUNC(D2CLIENT, SKILLDESC_FormatDamageRange, struct Unicode *, __fastcall, (struct Unicode *wszOut, int32_t nMin, int32_t nMax, int32_t bSigned, int32_t bAllowK), 0x6B140)
//D2FUNC(D2CLIENT, SKILLDESC_FormatRangeOrValue, int32_t, __fastcall, (struct Unicode *pwszOut, int32_t nMin, int32_t nMax, int32_t bSigned), 0x6B250)
//D2FUNC(D2CLIENT, SKILLDESC_AddWeaponPhysDamage, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t *pnMin, int32_t *pnMax, int32_t *pnColor, int32_t nExtraDamagePercent, int32_t nFlatBonus, D2SkillStrc *pSkill, int32_t nSrcDamOverride, D2UnitStrc *pWeapon, int32_t bUseItemOwnStats), 0x6B2C0)
//D2FUNC(D2CLIENT, SKILLDESC_GetChargeUpDamagePercent, int32_t, __cdecl, (), 0x6B580)
//D2FUNC(D2CLIENT, SKILLDESC_AddItemElemDamage, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t *pnMin, int32_t *pnMax, int32_t *pnColor, int32_t nUnused, D2SkillStrc *pSkill), 0x6B6F0)
//D2FUNC(D2CLIENT, SKILLDESC_AddChargeUpElemDamage, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t *pnMin, int32_t *pnMax, int32_t *pnColor), 0x6BB10)
//D2FUNC(D2CLIENT, SKILLDESC_AddElemDamageByType, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t *pnMin, int32_t *pnMax, uint8_t nEType), 0x6BD60)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc7, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6BEC0)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc1, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6C270)
//D2FUNC(D2CLIENT, SKILLDESC_AddTotalDamageCallback, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t *pnMin, int32_t *pnMax, int32_t *pnColor, int32_t nExtraDamagePercent, int32_t nFlatBonus, D2SkillStrc *pSkill, int32_t nSrcDamOverride, D2UnitStrc *pWeapon), 0x6C3A0)
//D2FUNC(D2CLIENT, SKILLDESC_DrawDualWieldDamage, void, __fastcall, (D2UnitStrc *pUnit, int32_t nShared, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, void *pfnCompute, int32_t nExtraA, int32_t nExtraB, int32_t nX, int32_t nY, int32_t nRight, int32_t bAddElemDamage), 0x6C400)
//D2FUNC(D2CLIENT, SKILLDESC_AddElemDamageAndColor, int32_t, __fastcall, (D2UnitStrc *pUnit, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnMinTotal, int32_t *pnMaxTotal, int32_t *pnColor), 0x6C840)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc20, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6C930)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc19, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6CA60)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc2, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6CD40)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc21, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6CE90)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc3, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6D330)
//D2FUNC(D2CLIENT, SKILLDESC_ComputeThrowDamage, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t *pnMin, int32_t *pnMax, int32_t *pnColor, int32_t nExtraDamagePercent, int32_t nUnusedA, D2SkillStrc *pSkill, int32_t nUnusedB, D2UnitStrc *pWeapon), 0x6D4F0)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc4, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6D920)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc22, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6DAB0)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc5, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6DEC0)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc24, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6E180)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc23, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6E440)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc13, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6E7B0)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc8, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6E800)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc9, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6EAB0)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc10, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6ED40)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc11, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6F080)
//D2FUNC(D2CLIENT, SKILLDESC_DrawDamageValue, void, __fastcall, (int32_t nUnused, int32_t nMin, int32_t nMax, int32_t nColor, int32_t nLeft, int32_t nY, int32_t nRight), 0x6F5E0)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc12, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6F700)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc14, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6FA50)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc15, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x6FC60)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc16, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x70010)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc17, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x70540)
//D2FUNC(D2CLIENT, SKILLDESC_DamFunc18, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight), 0x708F0)
//D2FUNC(D2CLIENT, SKILLDESC_BuildDescLine, int32_t, __fastcall, (D2UnitStrc *pPlayer, struct Unicode *pwszText, D2SkillsTxt *pSkillsTxt, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nCharges, int32_t nLine, int32_t nColor), 0x70970)
//D2FUNC(D2CLIENT, SKILLDESC_ApplyFireMasteryToRange, int32_t, __fastcall, (D2SkillsTxt *pSkillsTxt, int32_t *pnMin, int32_t *pnMax), 0x72F80)
//D2FUNC(D2CLIENT, SKILLDESC_ApplyLightningMasteryToRange, int32_t, __fastcall, (D2SkillsTxt *pSkillsTxt, int32_t *pnMin, int32_t *pnMax), 0x73120)
//D2FUNC(D2CLIENT, SKILLDESC_FormatFixedPoint, int32_t, __fastcall, (struct Unicode *pwszOut, int32_t nWhole, int32_t nFraction, int32_t bSigned), 0x732C0)
//D2FUNC(D2CLIENT, SKILLDESC_FormatFixedPointOrInt, int32_t, __fastcall, (struct Unicode *pwszOut, int32_t nWhole, int32_t nFraction, int32_t bSigned), 0x73320)
//D2FUNC(D2CLIENT, SKILLDESC_FormatFracRange, struct Unicode *, __fastcall, (struct Unicode *pwszOut, int32_t nMinInt, int32_t nMinFrac, int32_t nMaxInt, int32_t nMaxFrac, int32_t bSigned), 0x733C0)
//D2FUNC(D2CLIENT, SKILLDESC_AppendFramesAsSeconds, int32_t, __fastcall, (struct Unicode *pwszText, int32_t nStrIdx, int32_t nFrames, int32_t bSigned), 0x73410)
//D2FUNC(D2CLIENT, SKILLDESC_AppendScaledDamageRange, int32_t, __fastcall, (struct Unicode *pwszText, int32_t nStrIdxA, int32_t nStrIdxB, int32_t nMin, int32_t nMax, int32_t nFracScale), 0x73550)
//D2FUNC(D2CLIENT, SKILLDESC_AppendTextLine, int32_t, __fastcall, (struct Unicode *pwszText, int32_t nStrIdx), 0x736B0)
//D2FUNC(D2CLIENT, SKILLDESC_AppendTextThenValue, int32_t, __fastcall, (struct Unicode *pwszText, int32_t nStrIdx, int32_t nValue, int32_t bSigned), 0x736E0)
//D2FUNC(D2CLIENT, SKILLDESC_AppendFixedPointValue, int32_t, __fastcall, (struct Unicode *pwszText, int32_t nStrIdxA, int32_t nStrIdxB, int32_t nValue, int32_t nFracScale, int32_t bSigned), 0x73790)
//D2FUNC(D2CLIENT, SKILLDESC_AppendIntAndFraction, int32_t, __fastcall, (struct Unicode *pwszText, int32_t nStrIdx, int32_t nInt, int32_t nFrac, int32_t bSigned), 0x738E0)
//D2FUNC(D2CLIENT, SKILLDESC_AppendValueThenText, int32_t, __fastcall, (struct Unicode *pwszText, int32_t nStrIdx, int32_t nValue, int32_t bSigned), 0x739E0)
//D2FUNC(D2CLIENT, SKILLDESC_AppendValueThenTextPair, int32_t, __fastcall, (struct Unicode *pwszText, int32_t nStrIdxA, int32_t nStrIdxB, int32_t nValue, int32_t bSigned), 0x73A90)
//D2FUNC(D2CLIENT, SKILLDESC_AppendValueBetweenTexts, int32_t, __fastcall, (struct Unicode *pwszText, int32_t nStrIdxA, int32_t nStrIdxB, int32_t nValue, int32_t bSigned), 0x73B50)
//D2FUNC(D2CLIENT, SKILLDESC_AppendRangeBetweenTexts, int32_t, __fastcall, (struct Unicode *pwszText, int32_t nStrIdxA, int32_t nStrIdxB, int32_t nMin, int32_t nMax, int32_t bSigned), 0x73C10)
//D2FUNC(D2CLIENT, SKILLDESC_AppendDamageRangeLine, int32_t, __fastcall, (struct Unicode *pwszText, int32_t nStrIdx, int32_t nMin, int32_t nMax, int32_t bSigned, int32_t bTrailingSpace), 0x73D30)
//D2FUNC(D2CLIENT, SKILLDESC_FormatChargesText, int32_t, __fastcall, (struct Unicode *wszOut, int32_t nCharges), 0x73E70)
//D2FUNC(D2CLIENT, SKILLDESC_AppendElemLengthLine, int32_t, __fastcall, (D2UnitStrc *pUnit, struct Unicode *pwszText, D2C_Skills nSkillId, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel), 0x73F40)
//D2FUNC(D2CLIENT, SKILLDESC_AppendElemDamageLine, int32_t, __fastcall, (struct Unicode *pwszText, int32_t nMin, int32_t nMax, int32_t nEType, int32_t nStrIdxOverride), 0x73FA0)
//D2FUNC(D2CLIENT, SKILLDESC_AppendSkillElemDamageLine, int32_t, __fastcall, (D2UnitStrc *pUnit, struct Unicode *pwszText, D2C_Skills nSkillId, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nStrIdxOverride), 0x740B0)
//D2FUNC(D2CLIENT, SKILLDESC_AppendPhysDamageLine, int32_t, __fastcall, (D2UnitStrc *pUnit, struct Unicode *pwszText, D2C_Skills nSkillId, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nPercentBonus, int32_t nFlatBonus), 0x74240)
//D2FUNC(D2CLIENT, SKILLDESC_AppendToHitLine, int32_t, __fastcall, (D2UnitStrc *pUnit, struct Unicode *pwszText, D2C_Skills nSkillId, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel), 0x74470)
//D2FUNC(D2CLIENT, SKILLDESC_AppendAveragePerSecondLine, int32_t, __fastcall, (struct Unicode *pwszText, int32_t nMin, int32_t nMax, int32_t nStrIdx), 0x74530)
//D2FUNC(D2CLIENT, SKILLDESC_AppendValueThenTextLine, int32_t, __fastcall, (struct Unicode *pwszText, int32_t nStrIdxLabel, int32_t nStrIdxSuffix, int32_t nValue, int32_t bSigned, int32_t bPercent), 0x74600)
//D2FUNC(D2CLIENT, SKILLDESC_AppendValueOverValueLine, int32_t, __fastcall, (struct Unicode *pwszText, int16_t nStrIdxLabel, int32_t nStrIdxSuffix, int32_t nValue, int32_t nMax, int32_t bSigned), 0x74710)
//D2FUNC(D2CLIENT, SKILLDESC_AppendTextPairLine, int32_t, __fastcall, (struct Unicode *pwszText, int32_t nStrIdxLabel, int32_t nStrIdxText), 0x74860)
//D2FUNC(D2CLIENT, SKILLDESC_AppendTextThenValueLine, int32_t, __fastcall, (struct Unicode *pwszText, int32_t nStrIdxLabel, int32_t nStrIdxText, int32_t nValue, int32_t bSigned, int32_t bPercent), 0x74910)
//D2FUNC(D2CLIENT, SKILLDESC_AppendSprintfLine, int32_t, __fastcall, (struct Unicode *pwszText, int32_t nStrIdxFormat, int32_t nValue), 0x74A30)
//D2FUNC(D2CLIENT, SKILLDESC_ValidateMonsterId, int32_t, __fastcall, (D2C_MonsterIds nMonsterId), 0x74AC0)
//D2FUNC(D2CLIENT, DATATBLS_GetSkillsTxtRecordById, D2SkillsTxt *, __fastcall, (int32_t nSkillId), 0x74AE0)
//D2FUNC(D2CLIENT, SKILLDESC_DrawQuantityHint, void, __fastcall, (int32_t nQuantity, int32_t nStringId), 0x74B10)
//D2FUNC(D2CLIENT, SKILLDESC_GetLinearParamByLevel, int32_t, __fastcall, (int32_t nSkillId, int32_t nSkillLevel), 0x74BB0)
//D2FUNC(D2CLIENT, SKILLDESC_SkillsBuildSkillLevelText, void, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nCharges, int32_t nIconLeft, int32_t nIconBottom), 0x74C00)
//D2FUNC(D2CLIENT, SKILLDESC_SkillsBuildSkillRequirementText, void, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nCharges, int32_t nIconLeft, int32_t nIconBottom), 0x75220)
//D2FUNC(D2CLIENT, SKILLDESC_SkillsBuildFinishingMoveText, void, __fastcall, (D2UnitStrc *pUnit, int32_t nSkillId, int32_t nSkillLevel, int32_t nCharges, int32_t nTextColor, int32_t nX, int32_t nY), 0x75850)
//D2FUNC(D2CLIENT, SKILLDESC_DrawSkillInfo, void, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, int32_t nLayoutRow), 0x75B00)
//D2FUNC(D2CLIENT, SKILLDESC_DrawBothSkillInfos, void, __fastcall, (D2UnitStrc *pUnit), 0x76190)
//D2FUNC(D2CLIENT, SKILLDESC_GetAttackRatingForHand, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t bLeftHand), 0x76200)
//D2FUNC(D2CLIENT, SKILLSTREE_ClearSkillPointPending, void, __cdecl, (), 0x76300)
//D2FUNC(D2CLIENT, SKILLSTREE_LoadChartImage, D2CellFileStrc *, __cdecl, (), 0x76310)
//D2FUNC(D2CLIENT, SKILLSTREE_UnloadChartImage, void, __cdecl, (), 0x76370)
//D2FUNC(D2CLIENT, SKILLSTREE_DrawAvailableSkills, void, __cdecl, (), 0x763B0)
//D2FUNC(D2CLIENT, SKILLSTREE_DrawSkillIcon, void, __fastcall, (D2C_Skills nSkillId, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nCellState, int32_t nGlobalPaletteShift), 0x766C0)
//D2FUNC(D2CLIENT, SKILLSTREE_DrawUnavailableSkills, void, __cdecl, (), 0x76A50)
//D2FUNC(D2CLIENT, SKILLSTREE_CacheBlackIndex, uint8_t, __cdecl, (), 0x76BE0)
//D2FUNC(D2CLIENT, SKILLSTREE_DrawPanel, void, __cdecl, (), 0x76C00)
//D2FUNC(D2CLIENT, SKILLSTREE_DrawLabels, void, __cdecl, (), 0x77080)
//D2FUNC(D2CLIENT, SKILLSTREE_DrawLabelFromTop480, void, __fastcall, (const Unicode *pwszText, int32_t nY), 0x777C0)
//D2FUNC(D2CLIENT, SKILLSTREE_DrawTabLabel, void, __fastcall, (int32_t nMode), 0x77800)
//D2FUNC(D2CLIENT, SKILLSTREE_IsMouseInRect, BOOL, __fastcall, (int32_t nLeft, int32_t nBottom, int32_t nWidth, int32_t nHeight), 0x77A30)
//D2FUNC(D2CLIENT, SKILLSTREE_OnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x77A80)
//D2FUNC(D2CLIENT, SKILLSTREE_OnLButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x78100)
//D2FUNC(D2CLIENT, SKILLSTREE_ClearCellState, void, __cdecl, (), 0x78580)
//D2FUNC(D2CLIENT, SPELLSEL_Init, void, __cdecl, (), 0x785C0)
//D2FUNC(D2CLIENT, SPELLSEL_Shutdown, void, __cdecl, (), 0x78680)
//D2FUNC(D2CLIENT, SPELLSEL_GetSkillIconImage, D2CellFileStrc *, __fastcall, (int16_t *pnSkillId), 0x78700)
//D2FUNC(D2CLIENT, SPELLSEL_SetMiniSkillMode, void, __fastcall, (int32_t nMode), 0x78760)
//D2FUNC(D2CLIENT, SPELLSEL_GetSkillIconState, int32_t, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill), 0x78770)
//D2FUNC(D2CLIENT, SPELLSEL_DrawSkillChargesTooltip, void, __fastcall, (int16_t *pnSkillId, int32_t nTextColor, int32_t nCharges, int32_t nX, int32_t nY), 0x78790)
//D2FUNC(D2CLIENT, SPELLSEL_DrawFinishingMoveTooltip, void, __fastcall, (int16_t *pnSkillId, int32_t nTextColor, int32_t nSkillLevel, int32_t nCharges, int32_t nX, int32_t nY), 0x78940)
//D2FUNC(D2CLIENT, SPELLSEL_ControlsDrawBoundKeyLabel, void, __fastcall, (D2HotkeyIndices eCmd, int32_t nX, int32_t nY, int32_t nColor), 0x789E0)
//D2FUNC(D2CLIENT, SPELLSEL_Nullsub2, void, __cdecl, (), 0x78AB0)
//D2FUNC(D2CLIENT, SPELLSEL_DrawSkillButtonIcon, void, __fastcall, (D2CellFileStrc *hSkillIconImage, int16_t *pnSkillId, D2UnitGUID nOwnerGUID, int32_t nCharges, int32_t nPaletteShift, int32_t nXPos, int32_t nYPos, int32_t bLeftHand), 0x78AC0)
//D2FUNC(D2CLIENT, SPELLSEL_DrawHotkeyLabelMode1, void, __fastcall, (int16_t *pnSkillId, D2UnitGUID nOwnerGUID, int32_t nX, int32_t nY), 0x78D50)
//D2FUNC(D2CLIENT, SPELLSEL_DrawHotkeyLabelMode0, void, __fastcall, (int16_t *pnSkillId, D2UnitGUID nOwnerGUID, int32_t nX, int32_t nY), 0x790C0)
//D2FUNC(D2CLIENT, SPELLSEL_DrawSelectorSkillIcon, void, __fastcall, (int16_t *pnSkillId, D2UnitGUID nOwnerGUID, int32_t nValue, int32_t nPaletteShift, int32_t nXPos, int32_t nYPos, int32_t bGrowRight), 0x79430)
//D2FUNC(D2CLIENT, SPELLSEL_DrawBothPanels, void, __cdecl, (), 0x79660)
//D2FUNC(D2CLIENT, SPELLSEL_DrawPanelSkills, void, __fastcall, (D2UnitStrc *pPlayer, int32_t nStartX, int32_t bGrowRight, int32_t bHoverTextPass, int32_t bSkipIconDraw), 0x79720)
//D2FUNC(D2CLIENT, SPELLSEL_SkillDescGetSkillQuantity_Always0, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t nSkillId), 0x79B60)
//D2FUNC(D2CLIENT, SPELLSEL_MiniSkillOnLButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x79BB0)
//D2FUNC(D2CLIENT, SPELLSEL_CycleSkill, void, __fastcall, (int32_t nDirection), 0x79D30)
//D2FUNC(D2CLIENT, SPELLSEL_UnbindEmptyScrollHotkeys, void, __cdecl, (), 0x79EF0)
//D2FUNC(D2CLIENT, SPELLSEL_BindHoveredSkillToSlot, void, __fastcall, (int32_t nSlot), 0x79FE0)
//D2FUNC(D2CLIENT, SPELLSEL_SkillsSelectHotkeySlot, void, __fastcall, (int32_t nSlot), 0x7A130)
//D2FUNC(D2CLIENT, SPELLSEL_SetHotkeySlot, void, __fastcall, (int32_t nSlot, int32_t nSkillId, int32_t nMode, D2UnitGUID nOwnerGUID), 0x7A200)
//D2FUNC(D2CLIENT, SPELLSEL_UiSpinnerConstruct, int32_t *, __thiscall, (D2ClientSpinnerStrc *pSpinner, int32_t nX, int32_t nY, int32_t bHorizontal, void *pfnOnChange), 0x7A220)
//D2FUNC(D2CLIENT, SPELLSEL_UiSpinnerVecDelDtor, D2ClientSpinnerStrc *, __thiscall, (D2ClientSpinnerStrc *pSpinner, uint8_t bFreeObject), 0x7A340)
//D2FUNC(D2CLIENT, SPELLSEL_UiSpinnerDestruct, void, __thiscall, (D2ClientSpinnerStrc *pSpinner), 0x7A370)
//D2FUNC(D2CLIENT, SPELLSEL_UiSpinnerDraw, void, __thiscall, (D2ClientSpinnerStrc *pSpinner), 0x7A3D0)
//D2FUNC(D2CLIENT, SPELLSEL_UiSpinnerGetPressedArrow, int32_t, __thiscall, (D2ClientSpinnerStrc *pSpinner), 0x7A460)
//D2FUNC(D2CLIENT, SPELLSEL_UiSpinnerTickAutoRepeat, int32_t, __thiscall, (D2ClientSpinnerStrc *pSpinner), 0x7A480)
//D2FUNC(D2CLIENT, SPELLSEL_UiSpinnerOnMouseWheel, int32_t, __thiscall, (D2ClientSpinnerStrc *pSpinner, SMSGHANDLER_PARAMS *pMsg), 0x7A530)
//D2FUNC(D2CLIENT, SPELLSEL_UiSpinnerOnMouseDown, int32_t, __thiscall, (D2ClientSpinnerStrc *pSpinner, SMSGHANDLER_PARAMS *pMsg), 0x7A580)
//D2FUNC(D2CLIENT, SPELLSEL_UiSpinnerOnMouseUp, int32_t, __thiscall, (D2ClientSpinnerStrc *pSpinner, SMSGHANDLER_PARAMS *pMsg), 0x7A630)
//D2FUNC(D2CLIENT, SPELLSEL_UiSpinnerProxyDestruct, void, __thiscall, (int32_t *thisptr), 0x7A670)
//D2FUNC(D2CLIENT, SPELLSEL_UiSpinnerProxyVecDelDtor, uint32_t **, __thiscall, (uint32_t **pProxy, uint8_t bFreeObject), 0x7A6D0)
//D2FUNC(D2CLIENT, SPELLSEL_UiSpinnerProxyConstruct, int32_t *, __thiscall, (int32_t *thisptr, int32_t nX, int32_t nY, int32_t bHorizontal, void *pfnOnChange), 0x7A700)
//D2FUNC(D2CLIENT, SPELLSEL_UiCtrlProxyFwdSlot3, int32_t, __thiscall, (uint32_t **thisptr, SMSGHANDLER_PARAMS *pMsg), 0x7A780)
//D2FUNC(D2CLIENT, SPELLSEL_UiCtrlProxyFwdSlot12Arg, int32_t, __thiscall, (uint32_t **thisptr, int32_t), 0x7A790)
//D2FUNC(D2CLIENT, TEXT_FreeParsedText, void, __stdcall, (void **ppParsedText), 0x7A7C0)
//D2FUNC(D2CLIENT, TEXT_Init, void, __cdecl, (), 0x7A820)
//D2FUNC(D2CLIENT, TEXT_Shutdown, void, __cdecl, (), 0x7A870)
//D2FUNC(D2CLIENT, TEXT_FreeMessageList, void, __fastcall, (D2TextDisplayListStrc *pList), 0x7AAF0)
//D2FUNC(D2CLIENT, TEXT_SetDisplayDelayAndSave, void, __fastcall, (int32_t nDelay), 0x7AB80)
//D2FUNC(D2CLIENT, TEXT_DrawHoverBox, void, __fastcall, (D2ClientPopupStrc *pPopup, int32_t nX, int32_t nY), 0x7ABA0)
//D2FUNC(D2CLIENT, TEXT_PopupOnSysKeyDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x7AC60)
//D2FUNC(D2CLIENT, TEXT_PopupOnChar, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x7AD00)
//D2FUNC(D2CLIENT, TEXT_PopupOnDismissKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x7ADA0)
//D2FUNC(D2CLIENT, TEXT_OnPopupDismissed, int32_t, __cdecl, (), 0x7AE00)
//D2FUNC(D2CLIENT, TEXT_FreeParsedLines, void, __fastcall, (D2ClientTextLineStrc *pFirstLine), 0x7B0B0)
//D2FUNC(D2CLIENT, TEXT_PopupOnMouseDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x7B0F0)
//D2FUNC(D2CLIENT, TEXT_PopupSetOwnerUnit, void, __fastcall, (D2UnitStrc *pUnit), 0x7B1D0)
//D2FUNC(D2CLIENT, TEXT_PopupClearOwnerUnit, void, __cdecl, (), 0x7B1F0)
//D2FUNC(D2CLIENT, TEXT_PopupSetMessageHandlersActive, void, __fastcall, (int32_t bActive), 0x7B200)
//D2FUNC(D2CLIENT, TEXT_DrawUnitHoverTextHigh, void, __fastcall, (D2UnitStrc *pUnit), 0x7B260)
//D2FUNC(D2CLIENT, TEXT_PlaceHoverMsg, int32_t, __fastcall, (D2UnitStrc *pUnit, D2HoverTextStrc *pHoverMsg, Unicode *wszText, int32_t *pnPos), 0x7B4F0)
//D2FUNC(D2CLIENT, TEXT_DrawHoverMsgIfNotCovered, int32_t, __fastcall, (D2ClientPopupStrc *pPopup, LONG *pnCoords), 0x7B820)
//D2FUNC(D2CLIENT, TEXT_DrawUnitHoverTextLow, void, __fastcall, (D2UnitStrc *pUnit), 0x7B8A0)
//D2FUNC(D2CLIENT, TEXT_TickStaticInfoPanel, void, __fastcall, (D2ClientViewStrc *pView), 0x7BA00)
//D2FUNC(D2CLIENT, TEXT_DrawPopup, BOOL, __fastcall, (D2ClientPopupStrc *pPopup), 0x7BE70)
//D2FUNC(D2CLIENT, TEXT_DrawChatMessages, void, __cdecl, (), 0x7C1C0)
//D2FUNC(D2CLIENT, TEXT_DrawQuickMessages, void, __cdecl, (), 0x7C450)
//D2FUNC(D2CLIENT, TEXT_PrintChatMessage, int32_t, __fastcall, (Unicode *wszText, int32_t nColor), 0x7C600)
//D2FUNC(D2CLIENT, TEXT_PushMessageLogEntry, void, __fastcall, (D2ClientTextNodeStrc *pSrcMessage), 0x7C830)
//D2FUNC(D2CLIENT, TEXT_AddQuickMessage, int32_t, __fastcall, (const struct Unicode *pwszText, int32_t nColor), 0x7C950)
//D2FUNC(D2CLIENT, TEXT_SetCallback, void, __fastcall, (void *pfnCallback, int32_t nContext), 0x7CBD0)
//D2FUNC(D2CLIENT, TEXT_StopPopupSpeechSound, void, __cdecl, (), 0x7CBE0)
//D2FUNC(D2CLIENT, TEXT_ShowSpeechPopup, void, __fastcall, (D2UnitStrc *pNpc, int16_t nTblStringId, int32_t nUnused), 0x7CC20)
//D2FUNC(D2CLIENT, TEXT_ParseLinesAndScrollRate, struct Unicode **, __fastcall, (Unicode *pwszText, int32_t *pnLineCount, int16_t *pnScrollRate), 0x7CF10)
//D2FUNC(D2CLIENT, TEXT_ShowQuestScrollingText, void, __fastcall, (int32_t nTblStringId, int16_t bIsQuestText, int32_t bNeedsAck), 0x7D110)
//D2FUNC(D2CLIENT, TEXT_GetPopupSourceUnit, D2UnitStrc *, __cdecl, (), 0x7D380)
//D2FUNC(D2CLIENT, TEXT_HandleSystemMessage, void, __fastcall, (D2GSPacketSrv5A *pPacket), 0x7D3C0)
//D2FUNC(D2CLIENT, TEXT_OnMessageListPacket, void, __fastcall, (D2GSPacketSrv27 *pPacket), 0x7DE10)
//D2FUNC(D2CLIENT, TEXT_MeasureMessageBox, void, __fastcall, (D2ClientPopupStrc *pPopup), 0x7E060)
//D2FUNC(D2CLIENT, TEXT_FreeUnitHoverMsg, void, __fastcall, (D2HoverTextStrc *pHoverMsg), 0x7E0B0)
//D2FUNC(D2CLIENT, TEXT_HandleChatMessage, void, __fastcall, (D2GSPacketSrv26 *pPacket), 0x7E120)
//D2FUNC(D2CLIENT, TEXT_ChatClearUnitHoverText, void, __fastcall, (D2GSPacketSrv76 *pPacket), 0x7E670)
//D2FUNC(D2CLIENT, TEXT_GetMessageListHead, D2TextHeaderStrc *, __cdecl, (), 0x7E6B0)
//D2FUNC(D2CLIENT, TEXT_QuestGetQuestMenuString, const struct Unicode *, __fastcall, (uint16_t nQuestStringId), 0x7E6C0)
//D2FUNC(D2CLIENT, TEXT_ClearMessages, void, __cdecl, (), 0x7E710)
//D2FUNC(D2CLIENT, TEXT_ClosePopup, void, __cdecl, (), 0x7E810)
//D2FUNC(D2CLIENT, TEXT_IniScrollOnMouseDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x7E8F0)
//D2FUNC(D2CLIENT, TEXT_ScrollDrawQuestScrollPanel, void, __cdecl, (), 0x7E990)
//D2FUNC(D2CLIENT, TEXT_IniScrollDrawPanel, void, __cdecl, (), 0x7E9E0)
//D2FUNC(D2CLIENT, TEXT_DrawQuestScrollFrame, void, __fastcall, (int32_t bAnimate), 0x7EB70)
//D2FUNC(D2CLIENT, TEXT_QuestOnCairnStoneOrderCmd, void, __fastcall, (uint8_t *ptCmd), 0x7EDF0)
//D2FUNC(D2CLIENT, TEXT_IniScrollOpen, void, __fastcall, (D2UnitStrc *pTargetItem, D2ItemsTxt *pItemsTxt), 0x7EE20)
//D2FUNC(D2CLIENT, TEXT_IsPopupActive, BOOL, __cdecl, (), 0x7EE90)
//D2FUNC(D2CLIENT, TEXT_FreeChatAndMessageLogLists, void, __cdecl, (), 0x7EEA0)
//D2FUNC(D2CLIENT, TEXT_OpenMessageLog, void, __cdecl, (), 0x7EFB0)
//D2FUNC(D2CLIENT, TEXT_OnMessageLogClose, int32_t, __cdecl, (), 0x7F140)
//D2FUNC(D2CLIENT, TEXT_CloseMessageLog, void, __cdecl, (), 0x7F160)
//D2FUNC(D2CLIENT, TEXT_ExpireAllChatMessages, void, __cdecl, (), 0x7F1A0)
//D2FUNC(D2CLIENT, TEXT_ChatShowSpeechSubtitle, void, __fastcall, (D2UnitStrc *pUnit, D2C_Sounds nSoundId), 0x7F1C0)
//D2FUNC(D2CLIENT, j_text_1AE00, int32_t, __cdecl, (), 0x7F310)
//D2FUNC(D2CLIENT, TEXT_MsgLogReopen, int32_t, __cdecl, (), 0x7F320)
//D2FUNC(D2CLIENT, TRADE_ResetState, void, __cdecl, (), 0x7F370)
//D2FUNC(D2CLIENT, TRADE_CloseOnShutdown, void, __cdecl, (), 0x7F380)
//D2FUNC(D2CLIENT, TRADE_DrawButtonHints, void, __cdecl, (), 0x7F390)
//D2FUNC(D2CLIENT, TRADE_OpenWindow, void, __cdecl, (), 0x7F590)
//D2FUNC(D2CLIENT, TRADE_CloseWindow, void, __fastcall, (int32_t bReopenInventory), 0x7F5B0)
//D2FUNC(D2CLIENT, TRADE_IsTradeInProgress, BOOL, __cdecl, (), 0x7F680)
//D2FUNC(D2CLIENT, TRADE_SendCancelOnce, void, __cdecl, (), 0x7F6A0)
//D2FUNC(D2CLIENT, TRADE_HandleStateMessage, void, __fastcall, (uint8_t *pMessage), 0x7F6E0)
//D2FUNC(D2CLIENT, TRADE_SendAgreeToTrade, int32_t, __cdecl, (), 0x7FD60)
//D2FUNC(D2CLIENT, TRADE_SendCancel, int32_t, __cdecl, (), 0x7FDB0)
//D2FUNC(D2CLIENT, TRADE_SendCancelEarlyStamp, int32_t, __cdecl, (), 0x7FE10)
//D2FUNC(D2CLIENT, TRADE_SetPartnerFromPacket78, void, __fastcall, (D2GSPacketSrv78 *pPacket), 0x7FE70)
//D2FUNC(D2CLIENT, TRADE_GetPartnerName, struct Unicode *, __cdecl, (), 0x7FEC0)
//D2FUNC(D2CLIENT, TRADE_SendAccept, void, __cdecl, (), 0x7FED0)
//D2FUNC(D2CLIENT, TRADE_SendGreenCheck, void, __cdecl, (), 0x7FF00)
//D2FUNC(D2CLIENT, TRADE_SetGold, void, __fastcall, (int32_t nGold, uint8_t bSelf), 0x7FF20)
//D2FUNC(D2CLIENT, TRADE_GetGoldOther, int32_t, __cdecl, (), 0x7FF40)
//D2FUNC(D2CLIENT, TRADE_GetGoldSelf, int32_t, __cdecl, (), 0x7FF50)
//D2FUNC(D2CLIENT, TRADE_SendGoldAmount, void, __fastcall, (uint32_t nGold), 0x7FF60)
//D2FUNC(D2CLIENT, TRADE_TickTimeout, int32_t, __cdecl, (), 0x7FFF0)
//D2FUNC(D2CLIENT, UI_TradeOnClearMessagesKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x800E0)
//D2FUNC(D2CLIENT, UI_LoadImageFromUiDir, D2CellFileStrc *, __fastcall, (const char *szName), 0x80100)
//D2FUNC(D2CLIENT, UI_InitUiSubsystems, void, __cdecl, (), 0x80160)
//D2FUNC(D2CLIENT, UI_ShutdownUiSubsystems, void, __cdecl, (), 0x802C0)
//D2FUNC(D2CLIENT, UI_CachePaletteIndices, uint8_t, __cdecl, (), 0x803C0)
//D2FUNC(D2CLIENT, UI_TextToUpperInPlace, void, __fastcall, (struct Unicode *pText), 0x803F0)
//D2FUNC(D2CLIENT, UI_AddColorCodeToWstring, wchar_t *, __fastcall, (wchar_t *wszText, uint32_t nColor), 0x80430)
//D2FUNC(D2CLIENT, UIPANELS_TextStripColorCodes, const struct Unicode *, __fastcall, (struct Unicode *pszText), 0x80500)
//D2FUNC(D2CLIENT, UIPANELS_SetTransientText, void, __fastcall, (const struct Unicode *pwszText, int32_t nX, int32_t nY, int32_t nColor), 0x80590)
//D2FUNC(D2CLIENT, UIPANELS_SetUnitLabelText, void, __fastcall, (const struct Unicode *pText, int32_t nColour), 0x805D0)
//D2FUNC(D2CLIENT, UIPANELS_ShouldDrawNameLabel, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_StringIndices nPrefixStr), 0x80600)
//D2FUNC(D2CLIENT, UIPANELS_GetUnitLifeFill128, int32_t, __fastcall, (D2UnitStrc *pUnit), 0x80670)
//D2FUNC(D2CLIENT, UIPANELS_BuildItemLabelText, void, __fastcall, (D2UnitStrc *pUnit, struct Unicode *pszOut, int32_t nSize), 0x80740)
//D2FUNC(D2CLIENT, UIPANELS_DrawUnitNameLabel, void, __fastcall, (D2UnitStrc *pUnit), 0x80A30)
//D2FUNC(D2CLIENT, UIPANELS_BuildMonsterImmunityLabel, void, __fastcall, (const D2UnitStrc *pUnit), 0x81570)
//D2FUNC(D2CLIENT, UIPANELS_BuildUnitLabel, void, __fastcall, (D2UnitStrc *pUnit, int32_t *pnLifeBarFill, int32_t *pbHostileOrCorpse, struct Unicode *pOutBuffer, int32_t nMaxChars), 0x81680)
//D2FUNC(D2CLIENT, UIPANELS_DrawTiledFrame, void, __fastcall, (RECT *pRect), 0x81790)
//D2FUNC(D2CLIENT, UIPANELS_DrawTransientText, void, __cdecl, (), 0x81970)
//D2FUNC(D2CLIENT, UIPANELS_DrawChargeUpIcons, void, __cdecl, (), 0x81A00)
//D2FUNC(D2CLIENT, UIPANELS_DrawInGamePanels, void, __fastcall, (D2ClientViewStrc *pView), 0x81B70)
//D2FUNC(D2CLIENT, UIPANELS_DrawTextCenteredInPanelRect, void, __fastcall, (const struct Unicode *pwszText, int32_t nFont, int32_t nY, int32_t nColor), 0x82280)
//D2FUNC(D2CLIENT, UIPANELS_GameShowDeathMessage, void, __cdecl, (), 0x82320)
//D2FUNC(D2CLIENT, UIPANELS_BuildOtherPlayerLabel, void, __fastcall, (D2UnitStrc *pUnit), 0x82560)
//D2FUNC(D2CLIENT, UI_RegisterPanelHandlers, void, __fastcall, (D2C_UIvars nUiId), 0x826C0)
//D2FUNC(D2CLIENT, UI_ApplyPanelKeyBindings, void, __fastcall, (const D2PanelKeyBindStrc *pBind), 0x82AA0)
//D2FUNC(D2CLIENT, UI_SetPanelHandlerKey, void, __fastcall, (int32_t nEntry, D2HotkeyIndices eCmd, BOOL bPrimary, const D2PanelKeyBindStrc *pBind, uint16_t nDefaultKey), 0x82D10)
//D2FUNC(D2CLIENT, UI_UnregisterPanelHandlers, void, __fastcall, (D2C_UIvars nUiId), 0x82D60)
//D2FUNC(D2CLIENT, UI_GetUiWindowState, int32_t, __fastcall, (D2C_UIvars nUIId), 0x83230)
//D2FUNC(D2CLIENT, UI_SetUIToggle, int32_t, __fastcall, (D2C_UIvars nUIId, D2C_UiStates nUIState, int32_t bCheckMousePos), 0x83260)
//D2FUNC(D2CLIENT, UI_CloseAllPanels, int32_t, __fastcall, (int32_t bIncludeAutomap, int32_t nToggleArg), 0x83800)
//D2FUNC(D2CLIENT, UI_GetUiWindowStateRaw, int32_t, __fastcall, (D2C_UIvars nUIId), 0x83860)
//D2FUNC(D2CLIENT, UI_GetPentSpinImage, D2CellFileStrc *, __cdecl, (), 0x83870)
//D2FUNC(D2CLIENT, UI_GetFocusImage, D2CellFileStrc *, __cdecl, (), 0x83880)
//D2FUNC(D2CLIENT, UI_ClearPanelToggleDelay, void, __cdecl, (), 0x83890)
//D2FUNC(D2CLIENT, UI_EnablePanelToggleDelay, void, __cdecl, (), 0x838A0)
//D2FUNC(D2CLIENT, UI_ArmPanelToggleDelay, uint32_t, __cdecl, (), 0x838B0)
//D2FUNC(D2CLIENT, UI_PostSyntheticMouseMove, LRESULT, __cdecl, (), 0x838D0)
//D2FUNC(D2CLIENT, UI_InitUiForPlayerUnit, void, __cdecl, (), 0x83910)
//D2FUNC(D2CLIENT, UI_ResetUiSubsystems, int32_t, __cdecl, (), 0x83960)
//D2FUNC(D2CLIENT, UI_LevelsGetLevelName, int32_t, __fastcall, (int32_t nLevelId), 0x839F0)
//D2FUNC(D2CLIENT, UI_CursorSendUseHeldItem, void, __cdecl, (), 0x83A40)
//D2FUNC(D2CLIENT, UI_CloseGoldEntryDialog, void, __cdecl, (), 0x83A60)
//D2FUNC(D2CLIENT, UI_OpenGoldEntryDialog, void, __fastcall, (int32_t nGoldDialogMode), 0x83B40)
//D2FUNC(D2CLIENT, UI_GoldDialogOnCancel, int32_t, __cdecl, (), 0x83F60)
//D2FUNC(D2CLIENT, UI_GoldDialogStepAmount, int32_t, __cdecl, (), 0x83F70)
//D2FUNC(D2CLIENT, UI_GoldDialogOnOk, int32_t, __cdecl, (), 0x84010)
//D2FUNC(D2CLIENT, UI_GoldDialogOnCancelButton, int32_t, __cdecl, (), 0x840C0)
//D2FUNC(D2CLIENT, UI_AutomapSetInputEnabled, void, __fastcall, (D2UnkOutdoorStrc2 *a1), 0x840D0)
//D2FUNC(D2CLIENT, UI_GetBuySellButtonImage, D2CellFileStrc *, __cdecl, (), 0x84110)
//D2FUNC(D2CLIENT, UI_LoadScreenLoadPalette, int32_t, __cdecl, (), 0x84150)
//D2FUNC(D2CLIENT, UI_LoadScreenBegin, void, __cdecl, (), 0x841A0)
//D2FUNC(D2CLIENT, UI_DrawLoadingScreen, void, __fastcall, (int32_t nProgress), 0x842E0)
//D2FUNC(D2CLIENT, UI_SetLoadScreenAct, void, __fastcall, (int32_t nAct), 0x845B0)
//D2FUNC(D2CLIENT, UI_UnloadLoadingScreen, void, __cdecl, (), 0x845C0)
//D2FUNC(D2CLIENT, UI_IsLoadingScreenActive, int32_t, __cdecl, (), 0x84650)
//D2FUNC(D2CLIENT, UI_ThrottleTick50ms, int32_t, __cdecl, (), 0x84660)
//D2FUNC(D2CLIENT, UI_GetPlayAreaBand, BOOL, __fastcall, (RECT *pBand), 0x84690)
//D2FUNC(D2CLIENT, UI_GetPlayAreaBottom, int32_t, __cdecl, (), 0x84780)
//D2FUNC(D2CLIENT, UI_ConfigRegisterPanelHandlers, void, __cdecl, (), 0x84790)
//D2FUNC(D2CLIENT, UI_NpcMenuCloseHireListDialog, void, __fastcall, (int32_t a1), 0x847D0)
//D2FUNC(D2CLIENT, UI_IsGoldDialogOpen, BOOL, __cdecl, (), 0x847E0)
//D2FUNC(D2CLIENT, DATATBLS_GetMonStats2TxtRecord, D2MonStats2Txt *, __fastcall, (int32_t nMonStatsId), 0x847F0)
//D2FUNC(D2CLIENT, USEITEM_AnvilInit, void, __cdecl, (), 0x84840)
//D2FUNC(D2CLIENT, USEITEM_AnvilShutdown, void, __cdecl, (), 0x848A0)
//D2FUNC(D2CLIENT, USEITEM_AnvilOnMouseMove, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x849A0)
//D2FUNC(D2CLIENT, USEITEM_AnvilCloseOnInteractionStop, void, __cdecl, (), 0x849D0)
//D2FUNC(D2CLIENT, USEITEM_AnvilOnButtonUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x84B20)
//D2FUNC(D2CLIENT, USEITEM_AnvilOnPanelClick, int32_t, __fastcall, (int32_t nMouseX, int32_t nMouseY), 0x84CD0)
//D2FUNC(D2CLIENT, USEITEM_NpcMenuWaitDialogRefuseClose, int32_t, __cdecl, (), 0x84F70)
//D2FUNC(D2CLIENT, USEITEM_AnvilOnPanelButtonDown, void, __fastcall, (int32_t nX, int32_t nY), 0x84F80)
//D2FUNC(D2CLIENT, USEITEM_AnvilOnButtonDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x85000)
//D2FUNC(D2CLIENT, USEITEM_AnvilOnCloseKey, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x851E0)
//D2FUNC(D2CLIENT, USEITEM_AnvilTickPanel, void, __cdecl, (), 0x85320)
//D2FUNC(D2CLIENT, USEITEM_AnvilDrawPanel, int32_t, __cdecl, (), 0x85500)
//D2FUNC(D2CLIENT, USEITEM_AnvilOnPanelPacket, void, __fastcall, (D2GSPacketSrv58 *pPacket), 0x857B0)
//D2FUNC(D2CLIENT, USEITEM_AnvilOpenPanel, int32_t, __cdecl, (), 0x85A10)
//D2FUNC(D2CLIENT, USEITEM_AnvilOpenPanelLocal, int32_t, __fastcall, (D2UnitGUID nNpcGUID), 0x85BA0)
//D2FUNC(D2CLIENT, WAYPOINT_AllocPanelData, D2WaypointDataStrc *, __cdecl, (), 0x85BC0)
//D2FUNC(D2CLIENT, WAYPOINT_ClosePanel, void, __cdecl, (), 0x85BE0)
//D2FUNC(D2CLIENT, WAYPOINT_DrawPanel, void, __cdecl, (), 0x85C70)
//D2FUNC(D2CLIENT, WAYPOINT_OnChar, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x86250)
//D2FUNC(D2CLIENT, WAYPOINT_SendCloseOnce, void, __cdecl, (), 0x862E0)
//D2FUNC(D2CLIENT, WAYPOINT_OnOpenPacket, void, __fastcall, (D2GSPacketSrv63 *pPacket), 0x86320)
//D2FUNC(D2CLIENT, WAYPOINT_ClampActTab, void, __fastcall, (int32_t nAct), 0x863A0)
//D2FUNC(D2CLIENT, WAYPOINT_BuildActWaypointList, void, __stdcall, (), 0x86440)
//D2FUNC(D2CLIENT, WAYPOINT_OnMouseUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x865C0)
//D2FUNC(D2CLIENT, WAYPOINT_OnMouseDown, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0x867A0)
//D2FUNC(D2CLIENT, CUNIT_FindUnitByGuidAndType, D2UnitStrc *, __fastcall, (uint32_t dwUnitGUID, D2C_UnitTypes nUnitType), 0x869F0)
//D2FUNC(D2CLIENT, CUNIT_FindUnitByClientIdAndType, D2UnitStrc *, __fastcall, (int32_t nUnitId, D2C_UnitTypes nUnitType), 0x86A60)
//D2FUNC(D2CLIENT, CUNIT_GetOwnerUnit, D2UnitStrc *, __fastcall, (D2UnitStrc *pUnit), 0x86AD0)
//D2FUNC(D2CLIENT, CUNIT_RemoveUnitById, void, __fastcall, (int32_t nUnitId, D2C_UnitTypes nUnitType), 0x86BE0)
//D2FUNC(D2CLIENT, CUNIT_UnlinkAndFreeUnit, void, __fastcall, (D2UnitStrc **pTable, int32_t nBucket, int32_t nUnitId, D2C_UnitTypes nUnitType), 0x86C00)
//D2FUNC(D2CLIENT, CUNIT_RemoveUnitByClientId, void, __fastcall, (int32_t nUnitId, D2C_UnitTypes nUnitType), 0x86C70)
//D2FUNC(D2CLIENT, CUNIT_FindRoomForCoords, D2ActiveRoomStrc *, __fastcall, (int32_t nX, int32_t nY), 0x86C90)
//D2FUNC(D2CLIENT, CUNIT_WarpUnit, void, __fastcall, (D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t bIsLevelChange), 0x86DD0)
//D2FUNC(D2CLIENT, CUNIT_CreateUnit, D2UnitStrc *, __fastcall, (uint32_t dwUnitId, int32_t nClassId, int32_t nPosX, int32_t nPosY, D2C_UnitTypes dwUnitType, int32_t nItemMode), 0x87040)
//D2FUNC(D2CLIENT, CUNIT_InitUnit, D2UnitStrc *, __fastcall, (D2UnitStrc *pUnit, uint32_t dwUnitId, int32_t nClassId, int32_t nPosX, int32_t nPosY, D2C_UnitTypes dwUnitType, int32_t nItemMode, uint8_t nObjectMode), 0x87080)
//D2FUNC(D2CLIENT, CUNIT_RegisterUnit, D2UnitStrc *, __fastcall, (D2UnitStrc *pUnit), 0x87280)
//D2FUNC(D2CLIENT, CUNIT_FindUnitInTable, D2UnitStrc *, __fastcall, (D2UnitStrc **pTable, int32_t nBucket, int32_t nUnitId, D2C_UnitTypes nUnitType), 0x87320)
//D2FUNC(D2CLIENT, CUNIT_AddUnitToTable, int32_t, __fastcall, (D2UnitStrc *pUnit), 0x87390)
//D2FUNC(D2CLIENT, CUNIT_CreatePlayerUnit, D2UnitStrc *, __fastcall, (uint32_t dwPlayerId, D2C_PlayerClasses nCharClass, int32_t nX, int32_t nY, char *szName), 0x87570)
//D2FUNC(D2CLIENT, CUNIT_CreateObjectUnit, D2UnitStrc *, __fastcall, (uint32_t dwUnitId, int32_t nObjectId, int32_t nPosX, int32_t nPosY, D2C_UnitTypes dwUnitType, int32_t nItemMode, uint8_t nObjectMode, uint8_t nInteractType), 0x875C0)
//D2FUNC(D2CLIENT, CUNIT_CreateMonsterUnit, D2UnitStrc *, __fastcall, (int32_t *pAssign, uint8_t *pComponents, uint8_t *pMonUmod, uint16_t wNameSeed, int32_t nBossHcIdx, int32_t bAsyncClientUnit), 0x87630)
//D2FUNC(D2CLIENT, CUNIT_CreateClientOnlyUnit, D2UnitStrc *, __fastcall, (int32_t nClassId, int32_t nPosX, int32_t nPosY, D2C_UnitTypes dwUnitType, int32_t nItemMode), 0x87D50)
//D2FUNC(D2CLIENT, CUNIT_OnUnitAlignmentChanged, void, __fastcall, (D2UnitStrc *pUnit, uint8_t nOldAlign, uint8_t nNewAlign), 0x87E10)
//D2FUNC(D2CLIENT, CUNIT_CreatePresetUnitsInRoom, void, __fastcall, (D2ActiveRoomStrc *pRoom), 0x87E70)
//D2FUNC(D2CLIENT, CUNIT_DestroyUnit, void, __fastcall, (D2UnitStrc *pUnit), 0x87ED0)
//D2FUNC(D2CLIENT, CUNIT_ClearUnitTables, void, __cdecl, (), 0x88020)
//D2FUNC(D2CLIENT, CUNIT_DestroyAllUnits, void, __cdecl, (), 0x88040)
//D2FUNC(D2CLIENT, CUNIT_UpdateAllUnits, void, __cdecl, (), 0x880F0)
//D2FUNC(D2CLIENT, CUNIT_ForEachUnitInTable, void, __fastcall, (D2UnitStrc **pTable, void (__fastcall *pfnCallback)(D2UnitStrc *pUnit)), 0x88350)
//D2FUNC(D2CLIENT, CUNIT_FreeUnitModStats, void, __fastcall, (D2UnitStrc *pUnit), 0x88380)
//D2FUNC(D2CLIENT, CUNIT_SetPlayerUnit, void, __fastcall, (D2UnitStrc *pPlayer), 0x88390)
//D2FUNC(D2CLIENT, CUNIT_GetPlayer, D2UnitStrc *, __fastcall, (), 0x883D0)
//D2FUNC(D2CLIENT, CUNIT_IsPlayerDeadOrMissing, BOOL, __cdecl, (), 0x883E0)
//D2FUNC(D2CLIENT, CUNIT_GfxGetUnitGfxInfo, D2ClientUnitGfxInfoStrc *, __fastcall, (D2UnitStrc *pUnit), 0x88400)
//D2FUNC(D2CLIENT, CUNIT_SetUnitPaletteShift1, void, __fastcall, (D2UnitStrc *pUnit, int32_t nShift), 0x88410)
//D2FUNC(D2CLIENT, CUNIT_GetUnitPaletteShift1, int32_t, __fastcall, (D2UnitStrc *pUnit), 0x88440)
//D2FUNC(D2CLIENT, CUNIT_SetUnitPaletteShift2, void, __fastcall, (D2UnitStrc *pUnit, int32_t nShift), 0x88470)
//D2FUNC(D2CLIENT, CUNIT_GetUnitPaletteShift2, int32_t, __fastcall, (D2UnitStrc *pUnit), 0x884A0)
//D2FUNC(D2CLIENT, CUNIT_IsPointInWarpSelectBox, BOOL, __fastcall, (D2UnitStrc *pWarp, int32_t nScreenX, int32_t nScreenY), 0x884D0)
//D2FUNC(D2CLIENT, CUNIT_SkillsSelectProc1_CorpseExplosion, BOOL, __fastcall, (int32_t nUnused, D2UnitStrc *pUnit), 0x885D0)
//D2FUNC(D2CLIENT, CUNIT_SkillsSelectProc2_RaiseSkeleton, BOOL, __fastcall, (int32_t nUnused, D2UnitStrc *pUnit), 0x885E0)
//D2FUNC(D2CLIENT, CUNIT_SkillsSelectProc3_Revive, BOOL, __fastcall, (int32_t nUnused, D2UnitStrc *pUnit), 0x885F0)
//D2FUNC(D2CLIENT, CUNIT_SkillsSelectProc4_FindPotion, BOOL, __fastcall, (int32_t nUnused, D2UnitStrc *pUnit), 0x88680)
//D2FUNC(D2CLIENT, CUNIT_SkillsSelectProc6_GrimWard, BOOL, __fastcall, (int32_t nUnused, D2UnitStrc *pUnit), 0x88690)
//D2FUNC(D2CLIENT, CUNIT_SkillsSelectProc5_FindItem, BOOL, __fastcall, (int32_t nUnused, D2UnitStrc *pUnit), 0x886A0)
//D2FUNC(D2CLIENT, CUNIT_ShouldHighlightUnit, BOOL, __fastcall, (D2UnitStrc *pUnit), 0x886B0)
//D2FUNC(D2CLIENT, CUNIT_HitTestUnit, BOOL, __fastcall, (D2UnitStrc *pUnit, int32_t nScreenX, int32_t nScreenY, int32_t bTestGeometry), 0x886F0)
//D2FUNC(D2CLIENT, CUNIT_GetComponentScreenBox, BOOL, __fastcall, (D2UnitStrc *pUnit, int32_t nUnused1, int32_t nComponent, int32_t nAnimFrame, D2BoundingBoxStrc *pBox, int32_t nUnused2), 0x88D90)
//D2FUNC(D2CLIENT, CUNIT_SkillsRunSelectProcById, BOOL, __fastcall, (D2UnitStrc *pPlayer, D2UnitStrc *pUnit, int32_t nSkillId), 0x88F20)
//D2FUNC(D2CLIENT, CUNIT_SetUnitPathTargetById, void, __fastcall, (D2UnitStrc *pUnit, D2C_UnitTypes nTargetType, int32_t nTargetId), 0x88F90)
//D2FUNC(D2CLIENT, CUNIT_FaceUnitAtPathTarget, void, __fastcall, (D2UnitStrc *pUnit, int32_t bSnap), 0x89010)
//D2FUNC(D2CLIENT, CUNIT_TurnUnitToFacePlayer, void, __fastcall, (D2UnitStrc *pUnit), 0x89100)
//D2FUNC(D2CLIENT, CUNIT_TurnPlayerToFaceUnit, void, __fastcall, (D2UnitStrc *pUnit), 0x891C0)
//D2FUNC(D2CLIENT, CUNIT_CanPlayerIssueCommand, BOOL, __fastcall, (D2UnitStrc *pPlayer, D2SkillStrc *pUnusedSkill), 0x89290)
//D2FUNC(D2CLIENT, CUNIT_GetPlayerRoom, D2ActiveRoomStrc *, __cdecl, (), 0x89370)
//D2FUNC(D2CLIENT, CUNIT_RefreshObjectsOnDayNightChange, void, __cdecl, (), 0x89390)
//D2FUNC(D2CLIENT, CUNIT_GetUnitPacketQueue, D2PacketListStrc *, __fastcall, (D2UnitStrc *pUnit), 0x89450)
//D2FUNC(D2CLIENT, CUNIT_SetUnitPacketQueue, void, __fastcall, (D2UnitStrc *pUnit, D2PacketListStrc *pQueue), 0x89480)
//D2FUNC(D2CLIENT, CUNIT_SetUnitAlive, void, __fastcall, (D2UnitStrc *pUnit), 0x894B0)
//D2FUNC(D2CLIENT, CUNIT_SetUnitDead, void, __fastcall, (D2UnitStrc *pUnit), 0x89540)
//D2FUNC(D2CLIENT, CUNIT_IsUnitDead, BOOL, __fastcall, (D2UnitStrc *pUnit), 0x89550)
//D2FUNC(D2CLIENT, CUNIT_ShouldDrawUnitUnder, BOOL, __fastcall, (D2UnitStrc *pUnit), 0x89590)
//D2FUNC(D2CLIENT, CUNIT_GetPathTargetUnit, D2UnitStrc *, __fastcall, (D2UnitStrc *pUnit), 0x896A0)
//D2FUNC(D2CLIENT, CUNIT_GfxFreeUnitLightMap, void, __fastcall, (D2UnitStrc *pUnit), 0x896E0)
//D2FUNC(D2CLIENT, CUNIT_ForEachPlayerUnit, void, __fastcall, (void (__fastcall *pfnUnitFunction)(D2UnitStrc *pUnit)), 0x89700)
//D2FUNC(D2CLIENT, CUNIT_ForEachMonsterUnit, void, __fastcall, (void (__fastcall *pfnUnitFunction)(D2UnitStrc *pUnit)), 0x89750)
//D2FUNC(D2CLIENT, CUNIT_ForEachObjectUnit, void, __fastcall, (void (__fastcall *pfnUnitFunction)(D2UnitStrc *pUnit)), 0x897A0)
//D2FUNC(D2CLIENT, CUNIT_GetUnitName, Unicode *, __fastcall, (D2UnitStrc *pUnit), 0x897F0)
//D2FUNC(D2CLIENT, CUNIT_ShouldDrawUnitComponents, BOOL, __fastcall, (D2UnitStrc *pUnit), 0x89A40)
//D2FUNC(D2CLIENT, CUNIT_SetUnitComponentDrawCount, void, __fastcall, (D2UnitStrc *pUnit), 0x89A80)
//D2FUNC(D2CLIENT, CUNIT_SetUnitStartLight, void, __fastcall, (D2UnitStrc *pUnit, int32_t nStartLight), 0x89AB0)
//D2FUNC(D2CLIENT, CUNIT_FreeUnitInventory, void, __fastcall, (D2UnitStrc *pUnit, int32_t bKeepItemsHeldElsewhere), 0x89AE0)
//D2FUNC(D2CLIENT, CUNIT_AddOverlay, void, __fastcall, (D2UnitStrc *pUnit, int32_t nOverlayId, int32_t nTargetOrStateId), 0x89D10)
//D2FUNC(D2CLIENT, CUNIT_IsUnitSelectableTarget, BOOL, __fastcall, (D2UnitStrc *pSource, D2UnitStrc *pTarget), 0x89D80)
//D2FUNC(D2CLIENT, CUNIT_IsSelectableAsPetTarget, BOOL, __fastcall, (D2UnitStrc *pSource, D2UnitStrc *pTarget), 0x8A0C0)
//D2FUNC(D2CLIENT, CUNIT_IsSelectableAsAllyTarget, BOOL, __fastcall, (D2UnitStrc *pSource, D2UnitStrc *pTarget), 0x8A1C0)
//D2FUNC(D2CLIENT, CUNIT_ResyncUnitFromServer, void, __fastcall, (D2UnitStrc *pUnit), 0x8A2D0)
//D2FUNC(D2CLIENT, ROSTER_GetPartyIdForUnit, int16_t, __fastcall, (D2UnitStrc *pUnit), 0x8A6E0)
//D2FUNC(D2CLIENT, CUNITEVENT_FreeAllForUnit, void, __fastcall, (D2UnitStrc *pUnit), 0x8A700)
//D2FUNC(D2CLIENT, CUNITEVENT_AddEvent, D2UnitEventStrc *, __fastcall, (D2UnitStrc *pUnit, int32_t nCltEvent, int32_t nSkillId, int32_t nSkillLevel, void *pfnHandler, int32_t nKind, int32_t nParam), 0x8A740)
//D2FUNC(D2CLIENT, CUNITEVENT_RemoveEventsByKey, void, __fastcall, (D2UnitStrc *pUnit, int32_t nKind, int32_t nParam), 0x8A7C0)
//D2FUNC(D2CLIENT, CUNITEVENT_FireEvent, int32_t, __fastcall, (int32_t nEvent, D2UnitStrc *pUnit, D2UnitStrc *pSourceUnit, int32_t nUnused), 0x8A830)
//D2FUNC(D2CLIENT, ITEM_DetachItemFromUnit, void, __fastcall, (D2UnitStrc *pItem, D2UnitStrc *pOwner, int32_t bForceExpire, int32_t bClampVitals), 0x8A880)
//D2FUNC(D2CLIENT, ITEM_SetClientItemMode, void, __fastcall, (D2UnitStrc *pItem, D2ActiveRoomStrc *pRoom, int32_t nX, int32_t nY, int32_t nItemMode, int32_t bInitStats), 0x8A8E0)
//D2FUNC(D2CLIENT, ITEM_GetDropSound, D2C_Sounds, __fastcall, (D2UnitStrc *pItem, int32_t *pnDropSfxFrame), 0x8AA90)
//D2FUNC(D2CLIENT, ITEM_FreeUnitData, void, __fastcall, (D2UnitStrc *pItem), 0x8ABD0)
//D2FUNC(D2CLIENT, ITEM_OnUnitModeUpdate, void, __fastcall, (D2UnitStrc *pUnit), 0x8AC30)
//D2FUNC(D2CLIENT, ITEM_DoClientAction, void, __fastcall, (D2C_ClientActions nAction, D2UnitStrc *pItem, int32_t bSet, int32_t nMode), 0x8AD10)
//D2FUNC(D2CLIENT, ITEM_ItemMsg00_PutOnGround, void, __fastcall, (D2GSPacketSrv9C *pPacket), 0x8AD50)
//D2FUNC(D2CLIENT, ITEM_CreateItemFromItemExMsg, D2UnitStrc *, __fastcall, (D2GSPacketSrv9C *pPacket, D2ItemSaveStrc *pItemSave, uint16_t nX, uint16_t nY, int32_t bAnnounce), 0x8AE60)
//D2FUNC(D2CLIENT, ITEM_ItemMsg01_ToCursor, void, __fastcall, (D2GSPacketSrv9C *pPacket), 0x8AF40)
//D2FUNC(D2CLIENT, ITEM_ItemMsg02_DropToGround, void, __fastcall, (D2GSPacketSrv9C *pPacket), 0x8B030)
//D2FUNC(D2CLIENT, ITEM_ItemMsg03_OnGround, void, __fastcall, (D2GSPacketSrv9C *pPacket), 0x8B190)
//D2FUNC(D2CLIENT, ITEM_RefreshActiveItems, void, __fastcall, (D2UnitStrc *pUnit), 0x8B360)
//D2FUNC(D2CLIENT, ITEM_ApplyGemMods, void, __fastcall, (D2UnitStrc *pItem, D2UnitStrc *pTarget, int32_t nUnused, int32_t bIgnoreBroken), 0x8B8E0)
//D2FUNC(D2CLIENT, ITEM_ItemMsg04_ToStorePage, void, __fastcall, (D2GSPacketSrv9C *pPacket), 0x8BAF0)
//D2FUNC(D2CLIENT, ITEM_PlaceAtStorePage, void, __fastcall, (uint16_t nX, uint16_t nY, int32_t nStorePage, D2UnitStrc *pPlayer, D2UnitStrc *pItem), 0x8BD70)
//D2FUNC(D2CLIENT, ITEM_ItemMsg05_ToInventoryGrid, void, __fastcall, (D2GSPacketSrv9D *pPacket), 0x8BEF0)
//D2FUNC(D2CLIENT, ITEM_CreateItemFromItemUnitExMsg, D2UnitStrc *, __fastcall, (D2GSPacketSrv9D *pPacket, D2ItemSaveStrc *pItemSave, uint16_t nX, uint16_t nY, int32_t bAnnounce), 0x8C1D0)
//D2FUNC(D2CLIENT, ITEM_RefreshUnitFromMsg, void, __fastcall, (const void *pMsg), 0x8C2B0)
//D2FUNC(D2CLIENT, ITEM_RefreshUnitFromPacket, void, __fastcall, (const void *pPacket), 0x8C2D0)
//D2FUNC(D2CLIENT, j_ITEM_RefreshActiveItems, void, __fastcall, (D2UnitStrc *pUnit), 0x8C2F0)
//D2FUNC(D2CLIENT, ITEM_ItemMsg06_InsertIntoBodyLoc, void, __fastcall, (D2GSPacketSrv9D *pPacket), 0x8C300)
//D2FUNC(D2CLIENT, ITEM_ItemMsg07_SwapBodyLocItems, void, __fastcall, (D2GSPacketSrv9D *pPacket), 0x8C660)
//D2FUNC(D2CLIENT, ITEM_ItemMsg08_RemoveFromBodyLoc, void, __fastcall, (D2GSPacketSrv9D *pPacket), 0x8CB10)
//D2FUNC(D2CLIENT, ITEM_ItemMsg09_EquipOnPlayer, void, __fastcall, (D2GSPacketSrv9D *pPacket), 0x8CDF0)
//D2FUNC(D2CLIENT, ITEM_CacheWeaponClassForBodyLoc, uint16_t, __fastcall, (D2UnitStrc *pItem, uint8_t nBodyLoc), 0x8D040)
//D2FUNC(D2CLIENT, ITEM_UnequipSwitchedOutItem, void, __fastcall, (D2GSPacketSrv9D *pPacket, D2ItemSaveStrc *pItemSave), 0x8D070)
//D2FUNC(D2CLIENT, ITEM_ItemMsg17_WeaponSwitch, void, __fastcall, (D2GSPacketSrv9D *pPacket), 0x8D300)
//D2FUNC(D2CLIENT, ITEM_ItemMsg0A_UpdateItem, void, __fastcall, (D2GSPacketSrv9C *pPacket), 0x8D650)
//D2FUNC(D2CLIENT, ITEM_ItemMsg0B0C_UpdateNpcStore, void, __fastcall, (D2GSPacketSrv9C *pPacket), 0x8D760)
//D2FUNC(D2CLIENT, ITEM_ItemMsg0D_SwapInContainer, void, __fastcall, (D2GSPacketSrv9C *pPacket), 0x8DAC0)
//D2FUNC(D2CLIENT, ITEM_SetItemDisplayState, void, __fastcall, (D2UnitStrc *pItem, uint16_t nX, int32_t nY, int32_t nAnimMode, int32_t bResolveRoom, int32_t nStorePage), 0x8DE80)
//D2FUNC(D2CLIENT, ITEM_ItemMsg0E_SetInvPage, void, __fastcall, (D2GSPacketSrv9C *pPacket), 0x8DF10)
//D2FUNC(D2CLIENT, ITEM_ItemMsg0F_RemoveFromBelt, void, __fastcall, (D2GSPacketSrv9C *pPacket), 0x8E100)
//D2FUNC(D2CLIENT, ITEM_ItemMsg10_SwapInBelt, void, __fastcall, (D2GSPacketSrv9C *pPacket), 0x8E2E0)
//D2FUNC(D2CLIENT, ITEM_GetDropSoundNoFrame, D2C_Sounds, __fastcall, (D2UnitStrc *pItem), 0x8E590)
//D2FUNC(D2CLIENT, ITEM_GetUseSound, D2C_Sounds, __fastcall, (D2UnitStrc *pItem), 0x8E5A0)
//D2FUNC(D2CLIENT, ITEM_ClearWeaponClassCache, void, __cdecl, (), 0x8E680)
//D2FUNC(D2CLIENT, ITEM_SetChargedSkillCharges, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t nSkillId, int32_t nSkillLevel, int32_t nCharges), 0x8E690)
//D2FUNC(D2CLIENT, ITEM_OnItemStatUpdate, void, __fastcall, (uint32_t dwItemGUID, uint8_t bSetStat, int32_t nStatId, int32_t nValue, int32_t nStatParam), 0x8E780)
//D2FUNC(D2CLIENT, ITEM_OnCursorItemSkill, void, __fastcall, (D2GSPacketSrv3F *pPacket), 0x8E880)
//D2FUNC(D2CLIENT, ITEM_SetItemFlagFromPacket, void, __fastcall, (const void *pPacket), 0x8E9D0)
//D2FUNC(D2CLIENT, ITEM_ItemMsg11_AutoUnequip, void, __fastcall, (D2GSPacketSrv9D *pPacket), 0x8EA00)
//D2FUNC(D2CLIENT, ITEM_NpcMenuAckItemActionAndDropCursorItem, void, __fastcall, (void *pPacket), 0x8EDA0)
//D2FUNC(D2CLIENT, ITEM_ItemMsg12_RecreateToCursor, int32_t, __fastcall, (D2GSPacketSrv9C *pPacket), 0x8EE00)
//D2FUNC(D2CLIENT, ITEM_ItemMsg13_ItemInSocket, void, __fastcall, (D2GSPacketSrv9D *pPacket), 0x8EEC0)
//D2FUNC(D2CLIENT, ITEM_ItemMsg14_Unknown, void, __fastcall, (D2GSPacketSrv9D *pPacket), 0x8F050)
//D2FUNC(D2CLIENT, ITEM_ItemMsg15_UpdateStats, D2UnitStrc *, __fastcall, (D2GSPacketSrv9D *pPacket), 0x8F300)
//D2FUNC(D2CLIENT, ITEM_OnItemRefPacket7C, void, __fastcall, (D2GSPacketSrv7C *pPacket), 0x8FA80)
//D2FUNC(D2CLIENT, ITEM_ClearUninterruptableState, void, __fastcall, (D2UnitStrc *pUnit), 0x8FAD0)
//D2FUNC(D2CLIENT, ITEM_SetUninterruptableState, void, __fastcall, (D2UnitStrc *pUnit), 0x8FB30)
//D2FUNC(D2CLIENT, ITEM_OnUninterruptableStateRemoved, void, __fastcall, (D2UnitStrc *pUnit, int32_t nState, D2StatListStrc *pStatList), 0x8FB80)
//D2FUNC(D2CLIENT, ITEM_IsItemLocked, BOOL, __fastcall, (D2UnitStrc *pUnit), 0x8FB90)
//D2FUNC(D2CLIENT, ITEM_OnItemPairPacket7D, void, __fastcall, (D2GSPacketSrv7D *pPacket), 0x8FBC0)
//D2FUNC(D2CLIENT, ITEM_ItemMsg16_Unknown, int32_t, __fastcall, (D2GSPacketSrv9D *pPacket), 0x8FD00)
//D2FUNC(D2CLIENT, ITEM_OnPacket92, void, __fastcall, (D2UnitStrc *pPlayer), 0x8FDA0)
//D2FUNC(D2CLIENT, MISSILE_SprayRandomOverArea, void, __fastcall, (D2UnitStrc *pMissile, int32_t *paMissileIds, int32_t nMissileCount, int32_t nRadius, int32_t nRange, int32_t nChancePercent), 0x8FF20)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc2, void, __thiscall, (D2UnitStrc *pMissile), 0x90260)
//D2FUNC(D2CLIENT, MISSILE_CltSpawnTrailSubMissile, D2UnitStrc *, __fastcall, (D2UnitStrc *pMissile, int32_t nSubMissileId, int32_t nRange, int32_t nLoops), 0x902E0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc3, void, __thiscall, (D2UnitStrc *pMissile), 0x903E0)
//D2FUNC(D2CLIENT, MISSILE_EmitCloudPuffs, void, __fastcall, (D2UnitStrc *pMissile, int32_t nEveryNFrames, int32_t nCount, int32_t nSpread, int32_t nPuffMissileId), 0x90470)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc4, void, __fastcall, (D2UnitStrc *pMissile), 0x90690)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc5, void, __thiscall, (D2UnitStrc *pMissile), 0x906F0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc6, void, __thiscall, (D2UnitStrc *pMissile), 0x90820)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc7, void, __thiscall, (D2UnitStrc *pMissile), 0x90AD0)
//D2FUNC(D2CLIENT, MISSILE_RandomiseSpawnAppearance, void, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pParent), 0x90BD0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc8, void, __thiscall, (D2UnitStrc *pMissile), 0x90D20)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc9, void, __thiscall, (D2UnitStrc *pMissile), 0x90E80)
//D2FUNC(D2CLIENT, MISSILE_DropPeriodicSubMissile, D2UnitStrc *, __fastcall, (D2UnitStrc *pMissile, int32_t nRadius, int32_t nPeriod, int32_t nSubMissileId), 0x91180)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc10, void, __thiscall, (D2UnitStrc *pMissile), 0x91390)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc11, void, __thiscall, (D2UnitStrc *pMissile), 0x91540)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc12, void, __thiscall, (D2UnitStrc *pMissile), 0x91570)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc67, void, __thiscall, (D2UnitStrc *pMissile), 0x91660)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc13, void, __thiscall, (D2UnitStrc *pMissile), 0x91990)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc14, void, __thiscall, (D2UnitStrc *pMissile), 0x91BA0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc15, void, __thiscall, (D2UnitStrc *pMissile), 0x91DD0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc16, void, __thiscall, (D2UnitStrc *pMissile), 0x92150)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc17, void, __thiscall, (D2UnitStrc *pMissile), 0x92310)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc18, void, __thiscall, (D2UnitStrc *pMissile), 0x92470)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc19, void, __thiscall, (D2UnitStrc *pMissile), 0x92610)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc20, void, __thiscall, (D2UnitStrc *pMissile), 0x92750)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc21, void, __thiscall, (D2UnitStrc *pMissile), 0x92870)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc22, void, __thiscall, (D2UnitStrc *pMissile), 0x92AC0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc23, void, __thiscall, (D2UnitStrc *pMissile), 0x92CB0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc24, void, __thiscall, (D2UnitStrc *pMissile), 0x92CE0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc25, void, __thiscall, (D2UnitStrc *pMissile), 0x93180)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc26, void, __thiscall, (D2UnitStrc *pMissile), 0x93440)
//D2FUNC(D2CLIENT, MISSILE_CltSpawnScatteredSubMissile, D2UnitStrc *, __fastcall, (D2UnitStrc *pOwner, D2UnitStrc *pMissile, int32_t nSubMissileId, int32_t nSpread), 0x93680)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc27, void, __thiscall, (D2UnitStrc *pMissile), 0x937E0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc28, void, __thiscall, (D2UnitStrc *pMissile), 0x939B0)
//D2FUNC(D2CLIENT, MISSILE_SpawnScatteredSubMissiles, void, __fastcall, (D2UnitStrc *pOwner, D2UnitStrc *pMissile, int32_t nMissileId, int32_t nDensity), 0x93B30)
//D2FUNC(D2CLIENT, MISSILE_StepAndarielCloud, void, __fastcall, (D2UnitStrc *pOwner, D2UnitStrc *pMissile, int32_t nMissileId, int32_t nFrame, int32_t nStage), 0x93D50)
//D2FUNC(D2CLIENT, MISSILE_SpawnRandomSubMissile, void, __fastcall, (D2UnitStrc *pOwner, D2UnitStrc *pMissile, int32_t nMissileId, int32_t nRadius), 0x93F90)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc29, void, __thiscall, (D2UnitStrc *pMissile), 0x94300)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc30, void, __thiscall, (D2UnitStrc *pMissile), 0x94500)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc43, void, __thiscall, (D2UnitStrc *pMissile), 0x94760)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc44, void, __thiscall, (D2UnitStrc *pMissile), 0x947B0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc45, void, __thiscall, (D2UnitStrc *pMissile), 0x948F0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc46, void, __thiscall, (D2UnitStrc *pMissile), 0x94960)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc68, void, __thiscall, (D2UnitStrc *pMissile), 0x94B00)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc47, void, __thiscall, (D2UnitStrc *pMissile), 0x94C30)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc48, void, __thiscall, (D2UnitStrc *pMissile), 0x94CC0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc49, void, __thiscall, (D2UnitStrc *pMissile), 0x94FF0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc50, void, __thiscall, (D2UnitStrc *pMissile), 0x95110)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc51, void, __thiscall, (D2UnitStrc *pMissile), 0x953C0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc52, void, __thiscall, (D2UnitStrc *pMissile), 0x95670)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc53, void, __thiscall, (D2UnitStrc *pMissile), 0x95780)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc56, void, __thiscall, (D2UnitStrc *pMissile), 0x95840)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc57, void, __thiscall, (D2UnitStrc *pMissile), 0x959C0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc58, void, __thiscall, (D2UnitStrc *pMissile), 0x95B40)
//D2FUNC(D2CLIENT, MISSILE_SpawnOverArea, void, __fastcall, (D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nRadius, int32_t nMissileId, int32_t nSkillId, int32_t nLevel, int32_t nOneInChance, int32_t nStep), 0x95D10)
//D2FUNC(D2CLIENT, MISSILE_SpawnFromMissile, void, __fastcall, (D2UnitStrc *pMissile, int32_t nMissileId, int32_t nRingDirectionStep, int32_t nFullRingStep, int32_t nCltParam), 0x95FD0)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc64, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x96150)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc1, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x96250)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc2, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x96350)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc3, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x963B0)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc9, BOOL, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x96570)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc10, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x966D0)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc12, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x96740)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc13, BOOL, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x96890)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc14, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x96B50)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc16, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x96DF0)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc18, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x97060)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc19, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x97330)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc24, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x97470)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc25, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x97560)
//D2FUNC(D2CLIENT, MISSILE_SpawnOnUnitCallback, int32_t, __fastcall, (D2ClientNearbyTargetCtxStrc *pCtx, D2UnitStrc *pUnit), 0x97740)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc26, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x97800)
//D2FUNC(D2CLIENT, MISSILE_SpawnRingOfMissiles, int32_t, __fastcall, (D2UnitStrc *pOwner, D2UnitStrc *pOrigin, int32_t nMissileId, int32_t nSkillId, int32_t nLevel, int32_t nStep), 0x979F0)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc28, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x97B20)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc29, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x97BB0)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc30, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x97D70)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc4, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x97E80)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc32, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x97F90)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc33, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x98160)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc34, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x98360)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc36, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x98380)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc44, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x98450)
//D2FUNC(D2CLIENT, MISSILE_StartSeededPathFromOrigin, void, __fastcall, (D2UnitStrc *pMissile), 0x98570)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc46, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x985C0)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc47, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x98740)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc50, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x98940)
//D2FUNC(D2CLIENT, MISSILE_SpawnBoulderImpactDebris, void, __fastcall, (D2UnitStrc *pMissile, int32_t nRockCount, int32_t nRockVelocity, int32_t nFlyingRockMissile, int32_t nGroundFireMissile), 0x989A0)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc51, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x98BF0)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc52, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x98D20)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc53, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x98F00)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc54, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x99050)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc55, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x99150)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc56, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x99230)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc57, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x992F0)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc60, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x99400)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc61, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x994D0)
//D2FUNC(D2CLIENT, MISSILE_EnvSetLightQuality, void, __fastcall, (int32_t nQuality), 0x994F0)
//D2FUNC(D2CLIENT, MISSILE_GetLightQuality, int32_t, __cdecl, (), 0x99500)
//D2FUNC(D2CLIENT, MISSILE_IsMonsterTarget, BOOL, __fastcall, (D2UnitStrc *pUnit, D2ClientMissileTargetCtxStrc *pCtx), 0x99510)
//D2FUNC(D2CLIENT, MISSILE_IsEnemyTarget, BOOL, __fastcall, (D2UnitStrc *pUnit, D2ClientMissileTargetCtxStrc *pCtx), 0x995B0)
//D2FUNC(D2CLIENT, MISSILE_IsPlayerOrMonsterTarget, BOOL, __fastcall, (D2UnitStrc *pUnit, D2ClientMissileTargetCtxStrc *pCtx), 0x99650)
//D2FUNC(D2CLIENT, MISSILE_IsTargetableMissile, BOOL, __fastcall, (D2UnitStrc *pUnit), 0x996F0)
//D2FUNC(D2CLIENT, MISSILE_InitClientUnit, void, __fastcall, (D2UnitStrc *pUnit, D2ActiveRoomStrc *pRoom, int32_t nX, int32_t nY, int32_t nUnused), 0x99750)
//D2FUNC(D2CLIENT, MISSILE_FreeUnitData, void, __fastcall, (D2UnitStrc *pUnit), 0x99830)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc31, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x99880)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc40, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x99A70)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc37, void, __thiscall, (D2UnitStrc *pMissile), 0x99AC0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc41, void, __thiscall, (D2UnitStrc *pUnit), 0x99B10)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc40, void, __thiscall, (D2UnitStrc *pMissile), 0x99B60)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc39, void, __thiscall, (D2UnitStrc *pMissile), 0x99D00)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc42, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x99D20)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc43, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x99D90)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc38, void, __thiscall, (D2UnitStrc *pMissile), 0x99ED0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc42, void, __thiscall, (D2UnitStrc *pMissile), 0x9A130)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc31, void, __thiscall, (D2UnitStrc *pMissile), 0x9A3E0)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc37, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x9A7D0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc32, void, __thiscall, (D2UnitStrc *pMissile), 0x9A960)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc33, void, __thiscall, (D2UnitStrc *pMissile), 0x9AAA0)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc38, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x9ACE0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc34, void, __thiscall, (D2UnitStrc *pUnit), 0x9AE40)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc35, void, __thiscall, (D2UnitStrc *pMissile), 0x9B280)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc39, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x9B3B0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc36, void, __thiscall, (D2UnitStrc *pMissile), 0x9B4B0)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc41, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x9B720)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc61, void, __thiscall, (D2UnitStrc *pMissile), 0x9B7D0)
//D2FUNC(D2CLIENT, MISSILE_LaunchCatapultShot, void, __fastcall, (D2UnitStrc *pMonster, D2UnitStrc *pMissile, int32_t nMissileId), 0x9B8C0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc62, void, __thiscall, (D2UnitStrc *pMissile), 0x9BAF0)
//D2FUNC(D2CLIENT, MISSILE_SpawnNihlathakDebris, void, __fastcall, (D2UnitStrc *pOwner, D2UnitStrc *pMissile), 0x9BDA0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc63, void, __thiscall, (D2UnitStrc *pUnit), 0x9BFE0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc64, void, __thiscall, (D2UnitStrc *pUnit), 0x9C080)
//D2FUNC(D2CLIENT, MISSILE_SpawnFallingSubMissile, void, __fastcall, (D2UnitStrc *pMissile, int32_t nX, int32_t nY), 0x9C540)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc65, void, __thiscall, (D2UnitStrc *pUnit), 0x9C630)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc66, void, __thiscall, (D2UnitStrc *pMissile), 0x9C8A0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc59, void, __thiscall, (D2UnitStrc *pMissile), 0x9CA50)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc48, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x9CA90)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc54, void, __thiscall, (D2UnitStrc *pMissile), 0x9CE10)
//D2FUNC(D2CLIENT, MISSILE_SpawnSteamVentNearby, void, __fastcall, (D2UnitStrc *pOwner, D2UnitStrc *pMissile, int32_t nX, int32_t nY), 0x9D090)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc55, void, __thiscall, (D2UnitStrc *pMissile), 0x9D200)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc60, void, __thiscall, (D2UnitStrc *pMissile), 0x9D380)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc62, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x9D440)
//D2FUNC(D2CLIENT, MISSILE_CltHitFunc63, int32_t, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget), 0x9D660)
//D2FUNC(D2CLIENT, MISSILE_ClientUpdate, void, __fastcall, (D2UnitStrc *pMissile), 0x9D6D0)
//D2FUNC(D2CLIENT, MISSILE_CltDoFunc1, void, __fastcall, (D2UnitStrc *pMissile), 0x9D880)
//D2FUNC(D2CLIENT, MISSILE_DestroyClientMissile, void, __fastcall, (D2UnitStrc *pMissile), 0x9DBB0)
//D2FUNC(D2CLIENT, MISSILE_ClientOnHit, D2UnitStrc *, __fastcall, (D2UnitStrc *pMissile, D2UnitStrc *pTarget, int32_t bForceExplode), 0x9DC50)
//D2FUNC(D2CLIENT, MISSILE_ClientSpawn, D2UnitStrc *, __fastcall, (D2MissileStrc *pParams), 0x9E070)
//D2FUNC(D2CLIENT, MISSILE_ClientSpawnSimple, D2UnitStrc *, __fastcall, (D2UnitStrc *pOwner, int32_t nMissileId, int32_t nSpawnX, int32_t nSpawnY, int32_t nSkillId, int32_t nSkillLevel), 0x9EAF0)
//D2FUNC(D2CLIENT, MISSILE_ClientSpawnForOwner, D2UnitStrc *, __fastcall, (D2UnitStrc *pUnit, int32_t nMissileId, int32_t nTargetX, int32_t nTargetY, int32_t nSkillId, int32_t nSkillLevel), 0x9EB50)
//D2FUNC(D2CLIENT, MISSILE_ClientSpawnSkillEffect, D2UnitStrc *, __fastcall, (D2UnitStrc *pUnit, int32_t nMissileId, int32_t nSpawnOffsetX, int32_t nSpawnOffsetY, int32_t nTargetX, int32_t nTargetY, int32_t nSkillId, int32_t nSkillLevel, int32_t nGfxArg), 0x9EBE0)
//D2FUNC(D2CLIENT, MISSILE_ClientSpawnAtPathTarget, D2UnitStrc *, __fastcall, (D2UnitStrc *pUnit, int32_t nMissileId, int32_t nSkillId, int32_t nSkillLevel), 0x9ED40)
//D2FUNC(D2CLIENT, UNITS_FindFreeSpotForMonster, void, __fastcall, (D2UnitStrc *pUnit, int32_t nMonStatsId, int32_t nCount), 0x9EDF0)
//D2FUNC(D2CLIENT, ENV_SpawnRoomCritters, void, __fastcall, (D2ActiveRoomStrc *pRoom), 0x9EFB0)
//D2FUNC(D2CLIENT, MONSTER_ForEachUnitNearby, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t, int32_t, int32_t (__fastcall *)(D2UnitStrc *, D2UnitStrc *, int32_t, int32_t)), 0x9F270)
//D2FUNC(D2CLIENT, MONSTER_RegisterPlayerSlot, void, __fastcall, (D2UnitStrc *pPlayer), 0x9F2F0)
//D2FUNC(D2CLIENT, MONSTER_UnregisterPlayerSlot, void, __fastcall, (D2UnitStrc *pPlayer), 0x9F310)
//D2FUNC(D2CLIENT, MONSTER_ClearPlayerSlots, void, __cdecl, (), 0x9F330)
//D2FUNC(D2CLIENT, MONSTER_CritterTickAmbient, void, __fastcall, (D2UnitStrc *pMonster), 0x9F350)
//D2FUNC(D2CLIENT, MONSTER_CritterFindNearestThreatCallback, int32_t, __fastcall, (D2UnitStrc *pCritter, D2UnitStrc *pCandidate, D2UnitStrc **ppNearest, int32_t *pnBestDistance), 0x9F7D0)
//D2FUNC(D2CLIENT, MONSTER_CritterMoveToUnit, void, __fastcall, (D2UnitStrc *pUnit, D2UnitStrc *pTarget, D2C_ClientActions nAction, int32_t nVelocityPercent), 0x9F830)
//D2FUNC(D2CLIENT, MONSTER_CritterFleeFromTarget, int32_t, __fastcall, (D2UnitStrc *pUnit, D2UnitStrc *pTarget, uint8_t nDistance, D2C_ClientActions nAction, int32_t nVelocityPercent), 0x9F8A0)
//D2FUNC(D2CLIENT, MONSTER_CritterWanderRandom, void, __fastcall, (D2UnitStrc *pUnit, uint8_t nDistance, D2C_ClientActions nAction, int32_t nVelocityPercent), 0x9FA50)
//D2FUNC(D2CLIENT, MONSTER_CritterStandStill, void, __fastcall, (D2UnitStrc *pUnit), 0x9FB80)
//D2FUNC(D2CLIENT, MONSTER_CritterSetCooldown, int32_t *, __fastcall, (D2UnitStrc *pUnit, int32_t nTicks), 0x9FC10)
//D2FUNC(D2CLIENT, MONSTER_CritterTickBird, void, __fastcall, (D2UnitStrc *pBird, D2UnitStrc *pTarget, int32_t nNearest), 0x9FC30)
//D2FUNC(D2CLIENT, MONSTER_CritterTickMaggot, void, __fastcall, (D2UnitStrc *pMaggot, D2UnitStrc *pTarget, int32_t nNearest), 0x9FE20)
//D2FUNC(D2CLIENT, MONSTER_CritterTickCrawler, void, __fastcall, (D2UnitStrc *pUnit, D2UnitStrc *pTarget, uint32_t nNearest), 0x9FFE0)
//D2FUNC(D2CLIENT, MONSTER_CritterPickRandomCorpseCallback, int32_t, __fastcall, (D2UnitStrc *pCritter, D2UnitStrc *pCandidate, D2UnitStrc **ppChosen, int32_t *pnDistance), 0xA02F0)
//D2FUNC(D2CLIENT, MONSTER_CritterTickSnake, void, __fastcall, (D2UnitStrc *pSnake, D2UnitStrc *pThreat, uint32_t nNearest), 0xA0390)
//D2FUNC(D2CLIENT, MONSTER_CritterTickBunny, void, __fastcall, (D2UnitStrc *pBunny, D2UnitStrc *pTarget, int32_t nNearest), 0xA0560)
//D2FUNC(D2CLIENT, MONSTER_EnvTickWorldstoneChips, void, __fastcall, (D2UnitStrc *pUnit, D2UnitStrc *pTarget, int32_t nNearest), 0xA0790)
//D2FUNC(D2CLIENT, MONSTER_IsMonModeEnabled, int32_t, __fastcall, (D2C_MonsterIds nMonsterClassId, D2C_MonModes nMode), 0xA0970)
//D2FUNC(D2CLIENT, MONSTER_GetSkillSound, D2C_Sounds, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId), 0xA09E0)
//D2FUNC(D2CLIENT, MONSTER_BindMonStatsRecord, void, __fastcall, (D2UnitStrc *pUnit, int16_t nAiControl), 0xA0A80)
//D2FUNC(D2CLIENT, MONSTER_GetClientTimerPtr, int32_t *, __fastcall, (D2UnitStrc *pUnit), 0xA0AD0)
//D2FUNC(D2CLIENT, MONSTER_GetHirelingOwnerGUID, int32_t, __fastcall, (D2UnitStrc *pUnit), 0xA0AF0)
//D2FUNC(D2CLIENT, MONSTER_SetHirelingOwnerGUID, void, __fastcall, (D2UnitStrc *pUnit, int32_t nOwnerGUID), 0xA0B10)
//D2FUNC(D2CLIENT, MONSTER_GetClientFlagPtr, int32_t *, __fastcall, (D2UnitStrc *pUnit), 0xA0B30)
//D2FUNC(D2CLIENT, MONSTER_UpdateClientLight, void, __fastcall, (D2UnitStrc *pUnit), 0xA0B50)
//D2FUNC(D2CLIENT, MONSTER_IsChampion, BOOL, __fastcall, (D2UnitStrc *pUnit), 0xA0CC0)
//D2FUNC(D2CLIENT, MONSTER_IsGhostly, BOOL, __fastcall, (D2UnitStrc *pUnit), 0xA0CE0)
//D2FUNC(D2CLIENT, MONSTER_IsUnique, BOOL, __fastcall, (D2UnitStrc *pUnit), 0xA0D00)
//D2FUNC(D2CLIENT, MONSTER_IsMinion, BOOL, __fastcall, (D2UnitStrc *pUnit), 0xA0D20)
//D2FUNC(D2CLIENT, MONSTER_IsSuperUnique, BOOL, __fastcall, (D2UnitStrc *pUnit), 0xA0D40)
//D2FUNC(D2CLIENT, MONSTER_InitClientUnit, void, __fastcall, (D2UnitStrc *pUnit, D2ActiveRoomStrc *pRoom, int32_t nX, int32_t nY, D2C_MonsterIds nMonClassId, int32_t nAnimMode), 0xA0D60)
//D2FUNC(D2CLIENT, MONSTER_LinkOwnedUnits, void, __fastcall, (D2UnitStrc *pUnit, D2C_MonsterIds nClassId), 0xA12D0)
//D2FUNC(D2CLIENT, MONSTER_MatchVileMotherDirection, int32_t, __fastcall, (D2UnitStrc *pChild, D2UnitStrc *pOther, int32_t nUnused1, int32_t nUnused2), 0xA1690)
//D2FUNC(D2CLIENT, MONSTER_FreeUnitData, void, __fastcall, (D2UnitStrc *pUnit), 0xA1860)
//D2FUNC(D2CLIENT, MONSTER_ChangeMonsterClass, void, __fastcall, (D2UnitStrc *pUnit, D2C_MonsterIds nNewMonClassId), 0xA1900)
//D2FUNC(D2CLIENT, MONSTER_AdvanceAnimForMode, int32_t, __fastcall, (D2UnitStrc *pUnit, const D2ClientMonModeDescStrc *pModeDesc), 0xA1D70)
//D2FUNC(D2CLIENT, MONSTER_UpdateAnimMode, void, __fastcall, (D2UnitStrc *pUnit), 0xA1E90)
//D2FUNC(D2CLIENT, MONSTER_SpawnAttackMissile, void, __fastcall, (D2UnitStrc *pUnit), 0xA2420)
//D2FUNC(D2CLIENT, MONSTER_DoClientSplEndEffect, void, __fastcall, (D2UnitStrc *pMonster), 0xA2750)
//D2FUNC(D2CLIENT, MONSTER_SpawnBlood, void, __fastcall, (D2UnitStrc *pUnit, int32_t nCount, int32_t bByChance), 0xA2A70)
//D2FUNC(D2CLIENT, MONSTER_DoClientAction, void, __fastcall, (D2C_ClientActions nAction, D2UnitStrc *pUnit, int32_t *pParams), 0xA2D40)
//D2FUNC(D2CLIENT, MONSTER_SetVultureFlightState, void, __fastcall, (D2C_ClientActions nAction, D2UnitStrc *pUnit, int32_t *pUnusedParams), 0xA4460)
//D2FUNC(D2CLIENT, MONSTER_InitHireling, void, __fastcall, (D2UnitStrc *pMonster, const D2ClientHirelingInitStrc *pHirelingInfo), 0xA46B0)
//D2FUNC(D2CLIENT, MONSTER_CanBeRevived, BOOL, __fastcall, (D2UnitStrc *pUnit), 0xA4820)
//D2FUNC(D2CLIENT, MONSTER_SetMinionSpawnClassId, void, __fastcall, (const D2GSPacketSrv98 *pPacket), 0xA4880)
//D2FUNC(D2CLIENT, MONSTER_TestFlag, BOOL, __fastcall, (D2UnitStrc *pUnit, uint32_t dwMask), 0xA48C0)
//D2FUNC(D2CLIENT, MONSTER_SetFlagEx, void, __fastcall, (D2UnitStrc *pUnit, int32_t nFlagEx, int32_t bSet), 0xA48E0)
//D2FUNC(D2CLIENT, MONSTER_GetBaseId, int32_t, __fastcall, (int32_t nMonClassId), 0xA4910)
//D2FUNC(D2CLIENT, MONSTER_GetMonUmodArray, uint8_t *, __fastcall, (D2UnitStrc *pUnit), 0xA4950)
//D2FUNC(D2CLIENT, MONSTER_SetNameSeed, void, __fastcall, (D2UnitStrc *pUnit, uint16_t wNameSeed), 0xA4970)
//D2FUNC(D2CLIENT, MONSTER_SetBossHcIdx, void, __fastcall, (D2UnitStrc *pUnit, uint16_t wBossHcIdx), 0xA4990)
//D2FUNC(D2CLIENT, MONSTER_GetBossHcIdx, int16_t, __fastcall, (D2UnitStrc *pUnit), 0xA49B0)
//D2FUNC(D2CLIENT, MONSTER_TestTypeFlag, BOOL, __fastcall, (D2UnitStrc *pUnit, D2C_MonTypeFlags wMask), 0xA49D0)
//D2FUNC(D2CLIENT, MONSTER_SetTypeFlag, void, __fastcall, (D2UnitStrc *pUnit, D2C_MonTypeFlags wMask, int32_t bSet), 0xA4A00)
//D2FUNC(D2CLIENT, MONSTER_SetAuraRingEnabled, void, __fastcall, (D2UnitStrc *pUnit, int32_t bEnabled), 0xA4A30)
//D2FUNC(D2CLIENT, MONUNIQUE_BuildMonsterNameText, void, __fastcall, (D2UnitStrc *pMonster, D2C_MonUMods nModId, int32_t bUnique), 0xA4A70)
//D2FUNC(D2CLIENT, MONUNIQUE_ModLight, void, __fastcall, (D2UnitStrc *pMonster, D2C_MonUMods nModId, int32_t bUnique), 0xA4D20)
//D2FUNC(D2CLIENT, MONUNIQUE_ModDurielDead, void, __fastcall, (D2UnitStrc *pMonster, D2C_MonUMods nModId, int32_t bUnique), 0xA4E50)
//D2FUNC(D2CLIENT, MONUNIQUE_ModTeleport, void, __fastcall, (D2UnitStrc *pMonster, D2C_MonUMods nModId, int32_t bUnique), 0xA4E70)
//D2FUNC(D2CLIENT, MONUNIQUE_ModResists, void, __fastcall, (D2UnitStrc *pMonster, D2C_MonUMods nModId, int32_t bUnique), 0xA4EB0)
//D2FUNC(D2CLIENT, MONUNIQUE_ApplyModifiers, void, __fastcall, (D2UnitStrc *pMonster), 0xA5140)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModCltGoBoom, void, __fastcall, (D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique), 0xA51D0)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModCltFire, void, __fastcall, (D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique), 0xA5300)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModCltQuestComplete, void, __fastcall, (D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique), 0xA54A0)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModCltElementalDeath, void, __fastcall, (D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique), 0xA56D0)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModCltDurielDead, void, __fastcall, (D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique), 0xA5800)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModCltGetHitOverlay, void, __fastcall, (D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique), 0xA5910)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModSpawnAuraMissileRing, void, __fastcall, (D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId, int32_t nSkillLevel), 0xA5960)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModCltLightning, void, __fastcall, (D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique), 0xA5B20)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModCltScarab, void, __fastcall, (D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique), 0xA5B60)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModCltMultishotSpawn, void, __fastcall, (D2UnitStrc *pMissile, D2C_MonUMods nModId, int32_t bUnique), 0xA5BB0)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModCltFireSpikeExplode, void, __fastcall, (D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique), 0xA5E10)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModCltSuicideMinionExplode, void, __fastcall, (D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique), 0xA5EB0)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModPlayPregnantBurst, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xA6070)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModCltWormsOnDeath, void, __fastcall, (D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique), 0xA6110)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModRunCltFuncsCol0, void, __fastcall, (D2UnitStrc *pUnit), 0xA61B0)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModRunCltFuncsForUnit, void, __fastcall, (D2UnitStrc *pUnit, D2UnitStrc *pMissile, int32_t nColumn), 0xA61C0)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModRunCltFuncsCol1, void, __fastcall, (D2UnitStrc *pUnit), 0xA6270)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModRunCltFuncsCol2, void, __fastcall, (D2UnitStrc *pUnit), 0xA6280)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModRunCltFuncsCol3, void, __fastcall, (D2UnitStrc *pUnit), 0xA6290)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModRunCltFuncsOnMissileSpawn, void, __fastcall, (D2UnitStrc *pUnit, D2UnitStrc *pMissile), 0xA62A0)
//D2FUNC(D2CLIENT, MONUNIQUE_MonUModBuildMonsterModifierLabel, void, __fastcall, (D2UnitStrc *pUnit, struct Unicode *pwszOut, int32_t nCapacity, D2C_StringIndices nPrefixStr), 0xA62C0)
//D2FUNC(D2CLIENT, OBJECT_PickNpcSpeechSound, D2C_Sounds, __fastcall, (D2UnitStrc *pUnit, int32_t nSpeechKind), 0xA64A0)
//D2FUNC(D2CLIENT, OBJECT_FindSpeechSoundByTblIndex, D2C_Sounds, __fastcall, (D2C_StringIndices nTblStringId), 0xA67A0)
//D2FUNC(D2CLIENT, OBJECT_SetNpcSpeechAudio, void, __fastcall, (int32_t bAudio), 0xA67D0)
//D2FUNC(D2CLIENT, OBJECT_ApplyDayNightMode, int32_t, __fastcall, (D2UnitStrc *pObject, int32_t bSkipAnimRebuild), 0xA67E0)
//D2FUNC(D2CLIENT, OBJECT_SetLight, void, __fastcall, (D2UnitStrc *pObject, int32_t nRadius, int32_t nRed, int32_t nGreen, int32_t nBlue), 0xA6930)
//D2FUNC(D2CLIENT, OBJECT_InitClientUnit, void, __fastcall, (D2UnitStrc *pObject, D2ActiveRoomStrc *pRoom, int32_t nX, int32_t nY, D2Objects nObjectId, uint8_t nMode), 0xA69D0)
//D2FUNC(D2CLIENT, OBJECT_AddIdleOverlay, void, __fastcall, (D2UnitStrc *pObject), 0xA6BA0)
//D2FUNC(D2CLIENT, OBJECT_FreeClientUnit, int32_t, __fastcall, (D2UnitStrc *pObject), 0xA6D10)
//D2FUNC(D2CLIENT, OBJECT_AdvanceAnim, void, __fastcall, (D2UnitStrc *pUnit), 0xA6D50)
//D2FUNC(D2CLIENT, OBJECT_AdvanceDoorAnim, void, __fastcall, (D2UnitStrc *pUnit), 0xA6FB0)
//D2FUNC(D2CLIENT, OBJECT_FinishDoorAnim, void, __fastcall, (D2UnitStrc *pUnit), 0xA7030)
//D2FUNC(D2CLIENT, OBJECT_ShrineOfEnirhsEffect, void, __fastcall, (D2UnitStrc *pShrine, D2UnitStrc *pPlayer, D2ShrinesTxt *ptShrineData), 0xA70D0)
//D2FUNC(D2CLIENT, OBJECT_StormShrineEffect, void, __fastcall, (D2UnitStrc *pShrine, D2UnitStrc *pPlayer, D2ShrinesTxt *ptShrineData), 0xA7100)
//D2FUNC(D2CLIENT, OBJECT_ExplodingShrineEffect, void, __fastcall, (D2UnitStrc *pShrine, D2UnitStrc *pPlayer, D2ShrinesTxt *ptShrineData), 0xA7280)
//D2FUNC(D2CLIENT, OBJECT_PoisonShrineEffect, void, __fastcall, (D2UnitStrc *pShrine, D2UnitStrc *pPlayer, D2ShrinesTxt *ptShrineData), 0xA73D0)
//D2FUNC(D2CLIENT, OBJECT_ApplyShrineOverlays, void, __fastcall, (D2UnitStrc *pObject, D2ShrinesTxt *ptShrineData), 0xA7520)
//D2FUNC(D2CLIENT, OBJECT_InitShrine, void, __fastcall, (D2UnitStrc *pUnit), 0xA75D0)
//D2FUNC(D2CLIENT, OBJECT_DoClientAction, void, __fastcall, (D2C_ClientActions nAction, D2UnitStrc *pUnit, int32_t *pParams), 0xA7660)
//D2FUNC(D2CLIENT, OBJECT_SetClientMode, void, __fastcall, (D2UnitStrc *pObject, int32_t bSelectable, int32_t nMode), 0xA78D0)
//D2FUNC(D2CLIENT, OBJECT_PlayShrineSound, void, __fastcall, (D2UnitStrc *pObject, D2UnitStrc *pSourceUnit), 0xA7B70)
//D2FUNC(D2CLIENT, OBJECT_ProcessRipple, uint8_t, __thiscall, (D2UnitStrc *pUnit), 0xA7BF0)
//D2FUNC(D2CLIENT, OBJECT_ProcessRippleWithSound, uint8_t, __thiscall, (D2UnitStrc *pUnit), 0xA7CA0)
//D2FUNC(D2CLIENT, OBJECT_ProcessDrinker, uint8_t, __thiscall, (D2UnitStrc *pUnit), 0xA7D50)
//D2FUNC(D2CLIENT, OBJECT_ProcessGesturer, uint8_t, __thiscall, (D2UnitStrc *pUnit), 0xA7E00)
//D2FUNC(D2CLIENT, OBJECT_ProcessTurner, uint8_t, __thiscall, (D2UnitStrc *pUnit), 0xA7EE0)
//D2FUNC(D2CLIENT, OBJECT_ProcessSkeletonSpawn, uint8_t, __thiscall, (D2UnitStrc *pUnit), 0xA7FA0)
//D2FUNC(D2CLIENT, OBJECT_ProcessIceBubbles, uint8_t, __thiscall, (D2UnitStrc *pUnit), 0xA8050)
//D2FUNC(D2CLIENT, OBJECT_ProcessNothing, uint8_t, __thiscall, (D2UnitStrc *pUnit), 0xA8100)
//D2FUNC(D2CLIENT, OBJECT_ProcessClientSmoke, uint8_t, __thiscall, (D2UnitStrc *pUnit), 0xA8110)
//D2FUNC(D2CLIENT, OBJECT_ProcessAncientsAltar, uint8_t, __thiscall, (D2UnitStrc *pUnit), 0xA8160)
//D2FUNC(D2CLIENT, OBJECT_ProcessInvisibleAncient, uint8_t, __thiscall, (D2UnitStrc *pUnit), 0xA8180)
//D2FUNC(D2CLIENT, OBJECT_ProcessHoradricOrifice, uint8_t, __thiscall, (D2UnitStrc *pUnit), 0xA8210)
//D2FUNC(D2CLIENT, OBJECT_ProcessRogueBonfire, uint8_t, __thiscall, (D2UnitStrc *pUnit), 0xA8230)
//D2FUNC(D2CLIENT, OBJECT_ProcessFrozenAnya, uint8_t, __thiscall, (D2UnitStrc *pUnit), 0xA8260)
//D2FUNC(D2CLIENT, OBJECT_ProcessKeeper, uint8_t, __thiscall, (D2UnitStrc *pUnit), 0xA82C0)
//D2FUNC(D2CLIENT, OBJECT_ProcessBaalPortal, uint8_t, __thiscall, (D2UnitStrc *pUnit), 0xA8350)
//D2FUNC(D2CLIENT, OBJECT_ProcessZoo, uint8_t, __thiscall, (D2UnitStrc *pUnit), 0xA8390)
//D2FUNC(D2CLIENT, OBJECT_ClientProcess, uint8_t, __thiscall, (D2UnitStrc *pUnit), 0xA8430)
//D2FUNC(D2CLIENT, OBJECT_ApplyPortalState, void, __fastcall, (D2GSPacketSrv60 *ptCmd), 0xA8480)
//D2FUNC(D2CLIENT, OBJECT_GetPortalOwnerName, char *, __fastcall, (D2UnitStrc *pObject), 0xA8550)
//D2FUNC(D2CLIENT, OBJECT_SetPortalOwnerName, int32_t, __fastcall, (D2UnitStrc *pObject, const char *pszName), 0xA8560)
//D2FUNC(D2CLIENT, PLAYER_UpdateLightRadius, void, __fastcall, (D2UnitStrc *pUnit, int32_t nRadiusDelta), 0xA8570)
//D2FUNC(D2CLIENT, PLAYER_OnStatValueChanged, void, __fastcall, (struct D2GameStrc *pGame, D2UnitStrc *pOwner, D2UnitStrc *pOther, int32_t nLayer_StatId, int32_t nPreviousValue, int32_t nNewValue), 0xA8630)
//D2FUNC(D2CLIENT, PLAYER_InitClientUnit, void, __fastcall, (D2UnitStrc *pPlayer, D2ActiveRoomStrc *pRoom, int32_t nX, int32_t nY), 0xA88A0)
//D2FUNC(D2CLIENT, PLAYER_FreeClientUnit, void, __fastcall, (D2UnitStrc *pPlayer), 0xA8A60)
//D2FUNC(D2CLIENT, PLAYER_AdvanceAnimMode, void, __fastcall, (D2UnitStrc *pUnit), 0xA8B60)
//D2FUNC(D2CLIENT, PLAYER_DoClientAction, BOOL, __fastcall, (D2C_ClientActions nAction, D2UnitStrc *pUnit, int32_t *pParams, BOOL bInterruptKnockback), 0xA9090)
//D2FUNC(D2CLIENT, PLAYER_CanUseSkill, BOOL, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc **ppSkill, int32_t *pnUseState), 0xA97C0)
//D2FUNC(D2CLIENT, PLAYER_IsUnitInAuraRange, BOOL, __fastcall, (D2SkillStrc *pSkill, D2UnitStrc *pTarget, int32_t *pnRange, int32_t *pnDistance), 0xA98A0)
//D2FUNC(D2CLIENT, PLAYER_ResetClickGuard, void, __cdecl, (), 0xA9910)
//D2FUNC(D2CLIENT, PLAYER_OnMouseClick, int32_t, __fastcall, (int32_t nClickEvent, int32_t nX, int32_t nY, int32_t nMoveFlags), 0xA9920)
//D2FUNC(D2CLIENT, PLAYER_ResumeQueuedMove, void, __fastcall, (D2ClientClickStrc *pClick), 0xA9D70)
//D2FUNC(D2CLIENT, PLAYER_DispatchClickAction, void, __fastcall, (D2ClientClickStrc *pClick), 0xA9DF0)
//D2FUNC(D2CLIENT, PLAYER_SendClickCommand, void, __fastcall, (D2ClientClickStrc *pClick, int32_t bOnUnit), 0xAA570)
//D2FUNC(D2CLIENT, PLAYER_SendMoveToReachablePoint, void, __fastcall, (int32_t nClientCmd, D2UnitStrc *pUnit, int32_t nX, int32_t nY), 0xAA6B0)
//D2FUNC(D2CLIENT, PLAYER_DispatchSkillClick, void, __fastcall, (D2ClientClickStrc *pClick, D2C_UnitTypes nTargetType, uint32_t dwTargetGuid), 0xAA710)
//D2FUNC(D2CLIENT, PLAYER_InteractOrApproachTarget, void, __fastcall, (D2ClientClickStrc *pClick, int32_t nTargetType, int32_t nTargetGuid), 0xAAA50)
//D2FUNC(D2CLIENT, PLAYER_ClampClickTargetAndThrottle, int32_t, __fastcall, (D2UnitStrc *pUnit, D2ClientClickStrc *pClick), 0xAB0A0)
//D2FUNC(D2CLIENT, PLAYER_ShouldHandleClick, BOOL, __fastcall, (D2ClientClickStrc *pClick), 0xAB260)
//D2FUNC(D2CLIENT, PLAYER_IssueRightSkillCommand, int32_t, __fastcall, (D2ClientClickStrc *pClick), 0xAB2D0)
//D2FUNC(D2CLIENT, PLAYER_ReleaseClickAndResetHireling, int32_t, __fastcall, (D2ClientClickStrc *pClick), 0xAB360)
//D2FUNC(D2CLIENT, PLAYER_AssignCorpse, void, __fastcall, (D2GSPacketSrv74 *ptCmd), 0xAB3C0)
//D2FUNC(D2CLIENT, PLAYER_SetUnitRoomAndPos, void, __fastcall, (D2UnitStrc *pUnit, D2ActiveRoomStrc *pRoom, int32_t nX, int16_t nY), 0xAB640)
//D2FUNC(D2CLIENT, PLAYER_SetQueuedCommand, void, __fastcall, (int32_t nCmd, D2UnitStrc *pPlayer, int32_t nTargetType, D2UnitGUID nTargetGUID), 0xAB720)
//D2FUNC(D2CLIENT, PLAYER_GetQueuedCommand, int32_t, __fastcall, (D2UnitStrc *pPlayer), 0xAB760)
//D2FUNC(D2CLIENT, PLAYER_GetQueuedTargetType, int32_t, __fastcall, (D2UnitStrc *pUnit), 0xAB780)
//D2FUNC(D2CLIENT, PLAYER_GetQueuedTargetGuid, int32_t, __fastcall, (D2UnitStrc *pUnit), 0xAB7A0)
//D2FUNC(D2CLIENT, PLAYERLIST_RosterAllocPlayerInfoList, D2RosterInfoStrc **, __cdecl, (), 0xAB7C0)
//D2FUNC(D2CLIENT, PLAYERLIST_RosterFreePlayerInfoList, void, __fastcall, (D2RosterInfoStrc **ppInfoHead), 0xAB800)
//D2FUNC(D2CLIENT, PLAYERLIST_RosterGetOrAddInfoEntry, void, __fastcall, (uint32_t dwPlayerGUID, uint32_t dwTargetGUID), 0xAB840)
//D2FUNC(D2CLIENT, PLAYERLIST_RosterSetInfoFlag, void, __fastcall, (uint32_t dwPlayerGUID, uint32_t dwTargetGUID, int32_t nFlag, int32_t bSet), 0xAB8A0)
//D2FUNC(D2CLIENT, PLAYERLIST_RosterTestInfoFlag, int32_t, __fastcall, (uint32_t dwPlayerGUID, uint32_t dwTargetGUID, D2C_RosterInfoFlags nFlag), 0xAB900)
//D2FUNC(D2CLIENT, PLAYERLIST_RosterReplaceInfoFlags, void, __fastcall, (uint32_t dwPlayerGUID, uint32_t dwTargetGUID, int32_t nFlags), 0xAB940)
//D2FUNC(D2CLIENT, PLRSKILLS_SkillsDrawUnitEffectBehind, void, __fastcall, (D2UnitStrc *pUnit, int32_t nDrawParam), 0xAB980)
//D2FUNC(D2CLIENT, PLRSKILLS_SkillsDrawUnitEffectInFront, void, __fastcall, (D2UnitStrc *pUnit, int32_t nDrawParam), 0xABA60)
//D2FUNC(D2CLIENT, STATES_SetFunc1_Progressive, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xABB40)
//D2FUNC(D2CLIENT, STATES_RemFunc1_Progressive, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xABC00)
//D2FUNC(D2CLIENT, STATES_SetFunc2_CloakOfShadows, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xABC80)
//D2FUNC(D2CLIENT, STATES_SetFunc3_Blaze, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xABD40)
//D2FUNC(D2CLIENT, STATES_SetFunc4_SourceUnit, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xABE20)
//D2FUNC(D2CLIENT, STATES_RemFunc2_SourceUnit, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xABE60)
//D2FUNC(D2CLIENT, STATES_SetFunc5_ChangeClass, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xABE70)
//D2FUNC(D2CLIENT, STATES_RemFunc3_BoneArmor, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xABF60)
//D2FUNC(D2CLIENT, STATES_SetFunc6_BoneArmor, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xAC1B0)
//D2FUNC(D2CLIENT, STATES_RemFunc5_CycloneArmor, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xAC3D0)
//D2FUNC(D2CLIENT, STATES_SetFunc8_CycloneArmor, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xAC560)
//D2FUNC(D2CLIENT, STATES_SetFunc7_Hurricane, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xAC860)
//D2FUNC(D2CLIENT, STATES_RemFunc4_Hurricane, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xAC8D0)
//D2FUNC(D2CLIENT, STATES_SetFunc9_RemFunc6_ShrineSkill, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xAC940)
//D2FUNC(D2CLIENT, STATES_SetFunc10_Redeemed, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xAC950)
//D2FUNC(D2CLIENT, STATES_RemFunc7_Uninterruptable, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xACA50)
//D2FUNC(D2CLIENT, STATES_SetFunc11_VineBeast, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xACC10)
//D2FUNC(D2CLIENT, STATES_SetFunc12_Invis, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xACC20)
//D2FUNC(D2CLIENT, STATES_RemFunc8_Invis, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xACC50)
//D2FUNC(D2CLIENT, STATES_SetFunc13_Attached, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xACC80)
//D2FUNC(D2CLIENT, STATES_RemFunc9_Attached, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xACCA0)
//D2FUNC(D2CLIENT, STATES_RemFunc10_Pregnant, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xACCC0)
//D2FUNC(D2CLIENT, STATES_SetFunc14_Shapeshift, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xACCD0)
//D2FUNC(D2CLIENT, STATES_SetFunc15_Revive, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xACD70)
//D2FUNC(D2CLIENT, STATES_SetFunc16_SkillCharges, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xACDA0)
//D2FUNC(D2CLIENT, STATES_RemFunc11_SkillCharges, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xACF50)
//D2FUNC(D2CLIENT, STATES_SetFunc17_RestInPeace, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xACFC0)
//D2FUNC(D2CLIENT, STATES_SetFunc18_CorpseExp, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xAD060)
//D2FUNC(D2CLIENT, STATES_SetFunc19_RemFunc12_SkillDelay, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xAD110)
//D2FUNC(D2CLIENT, STATES_ApplyStateOverlays, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xAD120)
//D2FUNC(D2CLIENT, STATES_ToggleStateWithSound, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nState), 0xAD3A0)
//D2FUNC(D2CLIENT, STATES_RefreshUnitColorShift, void, __thiscall, (D2UnitStrc *pUnit), 0xAD510)
//D2FUNC(D2CLIENT, PLRSKILLS_EndStateAndSkill, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nState), 0xAD640)
//D2FUNC(D2CLIENT, PLRSKILLS_SetStateStat, D2StatListStrc *, __fastcall, (D2UnitStrc *pUnit, D2StatListStrc *pStatList, D2C_States nState, D2C_ItemStats nStatId, int32_t nValue, uint16_t nLayer), 0xAD830)
//D2FUNC(D2CLIENT, PLRSKILLS_StatesRunStateSetFunc, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xAD920)
//D2FUNC(D2CLIENT, PLRSKILLS_StatesRunStateRemFunc, void, __fastcall, (D2UnitStrc *pUnit, D2C_States nStateId), 0xADA00)
//D2FUNC(D2CLIENT, PLRSKILLS_SkillsGetUseStateForClient, D2C_SkillUseStates, __fastcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill), 0xADA80)
//D2FUNC(D2CLIENT, UNITGFX_UnitInitGfx, void, __fastcall, (D2UnitStrc *pUnit), 0xADAC0)
//D2FUNC(D2CLIENT, UNITGFX_GfxDrawUnit, void, __fastcall, (D2UnitStrc *pUnit, uint32_t dwLight, int32_t bForce), 0xADB40)
//D2FUNC(D2CLIENT, UNITMODE_ClearStaleTargetUnit, void, __fastcall, (D2UnitStrc *pUnit), 0xADEC0)
//D2FUNC(D2CLIENT, UNITMODE_RecacheUnitRoom, void, __fastcall, (D2UnitStrc *pUnit), 0xADF50)
//D2FUNC(D2CLIENT, UNITMODE_StartMoveToLocation, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t nX, int32_t nY), 0xADF60)
//D2FUNC(D2CLIENT, UNITMODE_SyncUnitPosition, void, __fastcall, (D2UnitStrc *pUnit, uint16_t nX, uint16_t nY, uint32_t nToleranceMode, int32_t nNextX, int32_t nNextY), 0xADF90)
//D2FUNC(D2CLIENT, UNITMODE_MoveUnitToCoords, void, __fastcall, (D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t bSnap), 0xAE360)
//D2FUNC(D2CLIENT, UNITMODE_StartMoveToUnit, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t nTargetType, int32_t nTargetGUID), 0xAE580)
//D2FUNC(D2CLIENT, UNITMODE_StepUnitAlongPath, BOOL, __fastcall, (D2UnitStrc *pUnit), 0xAE5D0)
//D2FUNC(D2CLIENT, UNITMODE_UpdateUnit, void, __fastcall, (D2UnitStrc *pUnit), 0xAE6A0)
//D2FUNC(D2CLIENT, UNITMODE_InteractWithUnit, void, __fastcall, (uint16_t nUnitType, int32_t nUnitGUID), 0xAE7E0)
//D2FUNC(D2CLIENT, UNITMODE_PerformClientCommand, void, __fastcall, (int32_t nClientCmd, D2UnitStrc *pUnit, int32_t nTargetTypeOrX, int32_t nTargetGuidOrY, int32_t nUnused), 0xAEA90)
//D2FUNC(D2CLIENT, UNITMODE_DispatchActionByUnitType, void, __fastcall, (D2C_ClientActions nAction, D2UnitStrc *pUnit, void *pActionParams, BOOL bInterruptKnockback), 0xAEE00)
//D2FUNC(D2CLIENT, UNITMODE_RunQueuedCommand, void, __fastcall, (D2UnitStrc *pUnit), 0xAEE70)
//D2FUNC(D2CLIENT, UNITMODE_MustMoveToTarget, BOOL, __fastcall, (D2UnitStrc *pUnit, D2UnitStrc *pTarget, int32_t nClientCmd), 0xAF120)
//D2FUNC(D2CLIENT, UNITMODE_MoveToTargetAndQueueCommand, void, __fastcall, (D2UnitStrc *pUnit, int32_t nQueuedCmd, int32_t nTargetType, int32_t nTargetGUID), 0xAF1D0)
//D2FUNC(D2CLIENT, UNITMODE_PlayHitOverlayForMode, void, __fastcall, (D2UnitStrc *pUnit, int32_t nMode), 0xAF240)
//D2FUNC(D2CLIENT, UNITMODE_FlushServerMemoryProbe, void, __cdecl, (), 0xAF2D0)
//D2FUNC(D2CLIENT, UNITMODE_SetAnimMode, void, __fastcall, (D2UnitStrc *pUnit, int32_t nMode), 0xAF410)
//D2FUNC(D2CLIENT, UNITMODE_SetAnimModeSilent, void, __fastcall, (D2UnitStrc *pUnit, int32_t nMode), 0xAF460)
//D2FUNC(D2CLIENT, UNITSND_ResetLevelSoundState, void, __cdecl, (), 0xAF4A0)
//D2FUNC(D2CLIENT, UNITSND_GetHitClassSound, D2C_Sounds, __fastcall, (D2UnitStrc *pUnit, int32_t nHitClass), 0xAF4D0)
//D2FUNC(D2CLIENT, UNITSND_GetHitClassSoundDelay, uint32_t, __fastcall, (D2UnitStrc *pUnit, int32_t nHitClass), 0xAF530)
//D2FUNC(D2CLIENT, UNITSND_GetImpactSoundsForHitClass, void, __fastcall, (D2UnitStrc *pUnit, int32_t nHitClass, int32_t nMode, D2C_Sounds *pnWeaponSound, D2C_Sounds *pnLayerSound), 0xAF5A0)
//D2FUNC(D2CLIENT, UNITSND_LinkSoundToUnit, void, __fastcall, (D2UnitStrc *pUnit, int32_t hInstance), 0xAF930)
//D2FUNC(D2CLIENT, UNITSND_UnlinkSoundFromUnit, void, __fastcall, (D2UnitStrc *pUnit, int32_t hInstance), 0xAF960)
//D2FUNC(D2CLIENT, UNITSND_FindPlayingSoundInGroup, int32_t, __fastcall, (D2UnitStrc *pUnit, D2C_Sounds nSoundId), 0xAF990)
//D2FUNC(D2CLIENT, UNITSND_GetFirstSoundOnUnit, int32_t, __fastcall, (D2UnitStrc *pUnit), 0xAFA10)
//D2FUNC(D2CLIENT, UNITSND_StopAllSoundsOnUnit, void, __fastcall, (D2UnitStrc *pUnit, int32_t bForce), 0xAFA40)
//D2FUNC(D2CLIENT, UNITSND_GetSurfaceMaterialUnderUnit, int32_t, __fastcall, (D2UnitStrc *pUnit), 0xAFA90)
//D2FUNC(D2CLIENT, UNITSND_GetFootstepSoundId, D2C_Sounds, __fastcall, (D2UnitStrc *pUnit, int32_t bRunning), 0xAFCE0)
//D2FUNC(D2CLIENT, UNITSND_GetMonSoundsRecordForUnit, D2MonSoundsTxt *, __fastcall, (D2UnitStrc *pUnit), 0xAFE50)
//D2FUNC(D2CLIENT, UNITSND_PlayFootstep, void, __fastcall, (D2UnitStrc *pUnit), 0xAFF80)
//D2FUNC(D2CLIENT, UNITSND_StopMonsterSkillSounds, void, __fastcall, (D2UnitStrc *pUnit), 0xB0290)
//D2FUNC(D2CLIENT, UNITSND_UpdateUnitModeSound, void, __fastcall, (D2UnitStrc *pUnit), 0xB0370)
//D2FUNC(D2CLIENT, UNITSND_GetSoundClassId, int32_t, __fastcall, (D2UnitStrc *pUnit), 0xB0850)
//D2FUNC(D2CLIENT, UNITSND_GetSoundUnitType, int32_t, __fastcall, (D2UnitStrc *pUnit), 0xB08B0)
//D2FUNC(D2CLIENT, UNITSND_GetSoundListHead, D2SoundListNodeStrc **, __fastcall, (D2UnitStrc *pUnit), 0xB0910)
//D2FUNC(D2CLIENT, UNITSND_SetSfxStarted, void, __fastcall, (D2UnitStrc *pUnit, uint8_t bStarted), 0xB0940)
//D2FUNC(D2CLIENT, UNITSND_SetSfxMode, void, __fastcall, (D2UnitStrc *pUnit, int32_t nMode), 0xB0970)
//D2FUNC(D2CLIENT, UNITSND_GetSfxTicks, uint32_t, __fastcall, (D2UnitStrc *pUnit), 0xB09A0)
//D2FUNC(D2CLIENT, UNITSND_GetSfxAsyncTicks, uint32_t, __fastcall, (D2UnitStrc *pUnit), 0xB09D0)
//D2FUNC(D2CLIENT, UNITSND_SetSfxAsyncTicks, void, __fastcall, (D2UnitStrc *pUnit, uint32_t nFrame), 0xB0A00)
//D2FUNC(D2CLIENT, UNITSND_PlayAttackSound, void, __fastcall, (D2UnitStrc *pUnit, int32_t nMode, int32_t bDelayed), 0xB0A30)
//D2FUNC(D2CLIENT, UNITSND_PlayModeSound, void, __fastcall, (D2UnitStrc *pUnit, int32_t nMode, int32_t bModeGiven), 0xB0D20)
//D2FUNC(D2CLIENT, UNITSND_StopMonsterVoices, void, __fastcall, (D2UnitStrc *pUnit), 0xB1030)
//D2FUNC(D2CLIENT, UNITSND_PlayHitOrDeathSound, void, __fastcall, (D2UnitStrc *pUnit, int32_t nMode), 0xB1140)
//D2FUNC(D2CLIENT, UNITSND_PlayMonsterSkillSound, void, __fastcall, (D2UnitStrc *pUnit, int32_t nMode), 0xB13B0)
//D2FUNC(D2CLIENT, UNITSND_PlayPlayerSoundEvent, void, __fastcall, (D2UnitStrc *pPlayer, uint16_t nSoundEvent), 0xB14E0)
//D2FUNC(D2CLIENT, UNITSND_PlayUnitSoundEvent, void, __fastcall, (uint32_t dwUnitGUID, uint8_t nUnitType, uint16_t nSoundEvent), 0xB1920)
//D2FUNC(D2CLIENT, UNITSND_AdjustUnitSoundRefCounts, void, __fastcall, (D2UnitStrc *pUnit, int32_t nDelta), 0xB1F00)
//D2FUNC(D2CLIENT, UNITSND_PlayAreaEntryQuestLine, void, __cdecl, (), 0xB20A0)
//D2FUNC(D2CLIENT, UNITSND_PlayMonsterInitSound, void, __fastcall, (D2UnitStrc *pUnit), 0xB2190)
//D2FUNC(D2CLIENT, MONSTER_GetBaseMonsterId, int32_t, __fastcall, (D2UnitStrc *pUnit), 0xB22F0)
//D2FUNC(D2CLIENT, SOUNDCHAN_PrepareWaveBuffer, int32_t, __fastcall, (D2SoundsTxt *ptHeader), 0xB2330)
//D2FUNC(D2CLIENT, SOUNDCHAN_ApplyEaxEnvironment, void, __fastcall, (int32_t nEnvironmentId), 0xB23C0)
//D2FUNC(D2CLIENT, SOUNDCHAN_StartInstance, D2SoundChannelStrc *, __fastcall, (D2SoundInstanceStrc *pSoundInstance), 0xB24A0)
//D2FUNC(D2CLIENT, SOUNDCHAN_SendCodeIntegrityProbe, void, __cdecl, (), 0xB27A0)
//D2FUNC(D2CLIENT, SOUNDCHAN_DetachInstance, void, __fastcall, (D2SoundChannelStrc *pChannel), 0xB28D0)
//D2FUNC(D2CLIENT, SOUNDCHAN_StopAndDetach, void, __fastcall, (D2SoundChannelStrc *pChannel), 0xB28F0)
//D2FUNC(D2CLIENT, SOUNDCHAN_StopAllAndDetach, void, __cdecl, (), 0xB2940)
//D2FUNC(D2CLIENT, SOUNDCHAN_ApplyVolumeAndPosition, void, __fastcall, (D2SoundChannelStrc *ptChannel), 0xB29B0)
//D2FUNC(D2CLIENT, SOUNDCHAN_IsSystemReady, int32_t, __cdecl, (), 0xB2CF0)
//D2FUNC(D2CLIENT, SOUNDCHAN_IsSystemReady_2, int32_t, __cdecl, (), 0xB2D00)
//D2FUNC(D2CLIENT, SOUNDCHAN_ReapFinished, void, __cdecl, (), 0xB2D10)
//D2FUNC(D2CLIENT, SOUNDCHAN_GetStreamPosition, uint32_t, __fastcall, (D2SoundChannelStrc *pChannel), 0xB2DA0)
//D2FUNC(D2CLIENT, SOUNDCHAN_CreateSystem, void, __cdecl, (), 0xB2DB0)
//D2FUNC(D2CLIENT, SOUNDCHAN_ShutdownSystem_2, void, __cdecl, (), 0xB2F90)
//D2FUNC(D2CLIENT, SOUNDCHAN_CreateChannels, BOOL, __fastcall, (int32_t nKindMode), 0xB3030)
//D2FUNC(D2CLIENT, SOUNDCHAN_ShutdownSystem, void, __cdecl, (), 0xB3170)
//D2FUNC(D2CLIENT, SOUNDCHAN_Get3DEnabled, int32_t, __cdecl, (), 0xB3210)
//D2FUNC(D2CLIENT, SOUNDCHAN_GetEaxEnabled, int32_t, __cdecl, (), 0xB3230)
//D2FUNC(D2CLIENT, SOUNDCHAN_Is3DEnabled, BOOL, __cdecl, (), 0xB3250)
//D2FUNC(D2CLIENT, SOUNDCHAN_IsEaxEnabled, BOOL, __cdecl, (), 0xB3280)
//D2FUNC(D2CLIENT, SOUNDCHAN_Toggle3dSound, int32_t, __cdecl, (), 0xB32A0)
//D2FUNC(D2CLIENT, SOUNDCHAN_ToggleEaxEffects, int32_t, __cdecl, (), 0xB3510)
//D2FUNC(D2CLIENT, SOUNDCHAN_FindOldestPlaying, D2SoundChannelStrc *, __fastcall, (D2C_Sounds nSoundId), 0xB3780)
//D2FUNC(D2CLIENT, SOUNDCHAN_IsPlaybackObjectActive, int32_t, __fastcall, (D2SoundChannelStrc *pChannel), 0xB37E0)
//D2FUNC(D2CLIENT, SOUNDENV_ResetEnvironmentState, void, __cdecl, (), 0xB3800)
//D2FUNC(D2CLIENT, SOUNDENV_UpdateEnvironment, void, __fastcall, (int32_t nTick), 0xB3860)
//D2FUNC(D2CLIENT, SOUNDHDR_GetRecord, D2SoundsTxt *, __fastcall, (D2C_Sounds nSoundId), 0xB3C00)
//D2FUNC(D2CLIENT, SOUNDHDR_GetGroupBaseIndex, D2C_Sounds, __fastcall, (D2C_Sounds nSoundId), 0xB3C30)
//D2FUNC(D2CLIENT, SOUNDHDR_GetGroupSize, uint8_t, __fastcall, (D2C_Sounds nSoundId), 0xB3C60)
//D2FUNC(D2CLIENT, SOUNDHDR_IsLooping, int32_t, __fastcall, (D2C_Sounds nSoundId), 0xB3C90)
//D2FUNC(D2CLIENT, SOUNDHDR_GetPlayerLevelId, int32_t, __cdecl, (), 0xB3CD0)
//D2FUNC(D2CLIENT, SOUNDHDR_GetCurrentSoundEnvironId, int32_t, __cdecl, (), 0xB3CF0)
//D2FUNC(D2CLIENT, SOUNDHDR_GetEnvRecord, D2SoundEnvironTxt *, __fastcall, (int32_t nSoundEnvId), 0xB3D30)
//D2FUNC(D2CLIENT, SOUNDHDR_GetEnvironLookupMin, int32_t, __cdecl, (), 0xB3D60)
//D2FUNC(D2CLIENT, SOUNDHDR_GetEnvironLookupMax, int32_t, __cdecl, (), 0xB3D70)
//D2FUNC(D2CLIENT, SOUNDHDR_GetEnvironLookup, int32_t *, __cdecl, (), 0xB3D80)
//D2FUNC(D2CLIENT, SOUNDHDR_Init, void, __cdecl, (), 0xB3D90)
//D2FUNC(D2CLIENT, SOUNDHDR_LoadSoundsTxt, void, __cdecl, (), 0xB3DE0)
//D2FUNC(D2CLIENT, SOUNDHDR_Shutdown, void, __cdecl, (), 0xB4860)
//D2FUNC(D2CLIENT, SOUNDHDR_AdjustRefCount, void, __fastcall, (D2C_Sounds nSoundId, int32_t nDelta), 0xB49C0)
//D2FUNC(D2CLIENT, SOUNDHDR_ClearRefCount, void, __fastcall, (D2C_Sounds nSoundId), 0xB4A90)
//D2FUNC(D2CLIENT, SOUNDHDR_ServiceAsyncLoads, void, __fastcall, (uint32_t dwTickCount), 0xB4B20)
//D2FUNC(D2CLIENT, SOUNDHDR_GetPreloadCursor, uint32_t, __cdecl, (), 0xB4C50)
//D2FUNC(D2CLIENT, SOUNDHDR_PumpPreload, void, __cdecl, (), 0xB4C60)
//D2FUNC(D2CLIENT, SOUNDHDR_StopAllPending, void, __cdecl, (), 0xB4CB0)
//D2FUNC(D2CLIENT, SOUNDHDR_FreeSoundData, void, __fastcall, (D2C_Sounds nSoundId), 0xB4CC0)
//D2FUNC(D2CLIENT, SOUNDHDR_LoadWaveData, void, __fastcall, (D2C_Sounds nSoundId, int32_t bForceLoad, int32_t bServicePass), 0xB4D90)
//D2FUNC(D2CLIENT, SOUNDHDR_GetRangeParam, double, __fastcall, (D2C_Sounds nSoundId), 0xB5230)
//D2FUNC(D2CLIENT, SOUNDHDR_GetFalloffDistance, double, __fastcall, (D2C_Sounds nSoundId), 0xB52A0)
//D2FUNC(D2CLIENT, SOUNDHDR_PickRandomInGroup, D2C_Sounds, __fastcall, (D2C_Sounds nSoundId), 0xB5310)
//D2FUNC(D2CLIENT, SOUNDHDR_SetCacheBudget, void, __fastcall, (uint32_t nBytes), 0xB53A0)
//D2FUNC(D2CLIENT, SOUNDHDR_BuildSoundPath, void, __fastcall, (D2C_Sounds nSoundId, char *pszPath), 0xB53B0)
//D2FUNC(D2CLIENT, SOUNDHDR_TouchCacheEntry, void, __fastcall, (D2C_Sounds nSoundId), 0xB54D0)
//D2FUNC(D2CLIENT, SOUNDINST_IsInstanceActive, int32_t, __fastcall, (int32_t hInstance), 0xB5520)
//D2FUNC(D2CLIENT, SOUNDINST_IsInstanceStillPlaying, int32_t, __fastcall, (int32_t hInstance), 0xB5550)
//D2FUNC(D2CLIENT, SOUNDINST_DetachFromUnit, void, __fastcall, (int32_t hInstance, D2UnitStrc *pUnit, int32_t bForce), 0xB55C0)
//D2FUNC(D2CLIENT, SOUNDINST_MarkStarted, void, __fastcall, (int32_t hInstance), 0xB56A0)
//D2FUNC(D2CLIENT, SOUNDINST_GetHandleForSoundId, int32_t, __fastcall, (D2C_Sounds nSoundId), 0xB56F0)
//D2FUNC(D2CLIENT, SOUNDINST_SetPosition, void, __fastcall, (int32_t hInstance, int32_t x, int32_t y, int32_t z), 0xB5710)
//D2FUNC(D2CLIENT, SOUNDINST_PlaySound, int32_t, __fastcall, (D2C_Sounds nSoundId, D2UnitStrc *pUnit, int32_t nDelayFrames, int32_t nPlayFlags, int32_t nStartBlock), 0xB5820)
//D2FUNC(D2CLIENT, SOUNDINST_GetUnitSoundPosition, void, __fastcall, (D2UnitStrc *pUnit, D2C_Sounds nSoundId, float *pOut), 0xB5AE0)
//D2FUNC(D2CLIENT, SOUNDINST_GetInstanceVolume, int32_t, __fastcall, (int32_t hInstance), 0xB5B90)
//D2FUNC(D2CLIENT, SOUNDINST_SetVolumeByHandle, void, __fastcall, (int32_t hInstance, int32_t nVolume), 0xB5BC0)
//D2FUNC(D2CLIENT, SOUNDINST_UpdateForUnit, void, __fastcall, (int32_t hInstance, int32_t nVolume, uint32_t dwFadeBeginDelay, uint32_t nFadeTicks), 0xB5BF0)
//D2FUNC(D2CLIENT, SOUNDINST_HasActiveChannel, BOOL, __fastcall, (int32_t hInstance), 0xB5D60)
//D2FUNC(D2CLIENT, SOUNDINST_StopSoundsById, void, __fastcall, (D2C_Sounds nSoundId), 0xB5DA0)
//D2FUNC(D2CLIENT, SOUNDINST_MarkAllEnvironStarted, void, __cdecl, (), 0xB5DF0)
//D2FUNC(D2CLIENT, SOUNDINST_StopSceneSounds, void, __fastcall, (int32_t nKeepGroupA, int32_t nKeepGroupB), 0xB5E60)
//D2FUNC(D2CLIENT, SOUNDINST_StopEventSounds, void, __fastcall, (int32_t nKeepGroupA, int32_t nKeepGroupB), 0xB5EE0)
//D2FUNC(D2CLIENT, SOUNDINST_StopSpeechSounds, void, __cdecl, (), 0xB5F60)
//D2FUNC(D2CLIENT, SOUNDINST_StopAllInstances, void, __cdecl, (), 0xB5FC0)
//D2FUNC(D2CLIENT, SOUNDINST_IsAnySpeechActive, int32_t, __cdecl, (), 0xB6010)
//D2FUNC(D2CLIENT, SOUNDINST_IsMusicPlaying, int32_t, __cdecl, (), 0xB6050)
//D2FUNC(D2CLIENT, SOUNDINST_GetSoundIdFromHandle, D2C_Sounds, __fastcall, (int32_t hInstance), 0xB6090)
//D2FUNC(D2CLIENT, SOUNDINST_ServiceInstances, void, __fastcall, (uint32_t nTick), 0xB60C0)
//D2FUNC(D2CLIENT, SOUNDINST_ResetPlayback, void, __cdecl, (), 0xB6AA0)
//D2FUNC(D2CLIENT, SOUNDINST_FreeAllInstances, void, __stdcall, (), 0xB6AC0)
//D2FUNC(D2CLIENT, SOUNDINST_GetPlayPosition, int32_t, __fastcall, (int32_t hInstance), 0xB6B60)
//D2FUNC(D2CLIENT, SOUNDINST_ComparePriority, int32_t, __fastcall, (D2SoundInstanceStrc *pA, D2SoundInstanceStrc *pB, int32_t bUseDistance, int32_t bUseAge), 0xB6BB0)
//D2FUNC(D2CLIENT, SOUNDINST_IsUnitInEarshot, int32_t, __fastcall, (D2UnitStrc *pUnit), 0xB6C30)
//D2FUNC(D2CLIENT, SOUNDINST_IsSpeechPlayingForUnit, BOOL, __fastcall, (D2UnitStrc *pUnit), 0xB6C70)
//D2FUNC(D2CLIENT, SOUNDINST_ToggleChaosDebug, void, __cdecl, (), 0xB6CD0)
//D2FUNC(D2CLIENT, SOUNDSONG_Init, void, __stdcall, (), 0xB6D40)
//D2FUNC(D2CLIENT, SOUNDSONG_UpdateMusic, void, __fastcall, (uint32_t nTick), 0xB6DA0)
//D2FUNC(D2CLIENT, SOUNDSONG_ScheduleSting, int32_t, __fastcall, (D2C_Sounds nSound1, int32_t nDelay1, int32_t nDuration, int32_t nSongStateIndex, D2C_Sounds nSound2, int32_t nDelay2, int32_t bPlaySound2), 0xB7030)
//D2FUNC(D2CLIENT, SOUNDSONG_ScheduleSting2, void, __fastcall, (int32_t nDelayFrames, int32_t nPending), 0xB7100)
//D2FUNC(D2CLIENT, SOUNDSONG_RollRandom, uint32_t, __fastcall, (int32_t nRange), 0xB7150)
//D2FUNC(D2CLIENT, SOUNDSONG_RollRandomInRange, int32_t, __fastcall, (int32_t nMin, int32_t nMax), 0xB71B0)
//D2FUNC(D2CLIENT, SOUNDSONG_RandomOffset, int32_t, __fastcall, (int32_t nRange), 0xB7220)
//D2FUNC(D2CLIENT, SOUNDSONG_Distance, double, __stdcall, (float x, float y), 0xB7290)
//D2FUNC(D2CLIENT, SOUNDSONG_DistanceSquared, double, __stdcall, (float x, float y), 0xB72B0)
//D2FUNC(D2CLIENT, SOUNDSONG_GetCircularDistance, int32_t, __fastcall, (int32_t nA, int32_t nB, int32_t nModulus), 0xB72D0)
//D2FUNC(D2CLIENT, CURSOR_OnMouseMove, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0xB7330)
//D2FUNC(D2CLIENT, CURSOR_OnNonClientMouseMove, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0xB7450)
//D2FUNC(D2CLIENT, CURSOR_UpdateCursorPosInGame, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0xB7480)
//D2FUNC(D2CLIENT, CURSOR_OnPanelMouseUp, void, __stdcall, (SMSGHANDLER_PARAMS *pMsg), 0xB7500)
//D2FUNC(D2CLIENT, CURSOR_SetBuysellCursor, void, __fastcall, (D2UnitStrc *pUnit, int32_t nFrame), 0xB7560)
//D2FUNC(D2CLIENT, CURSOR_SetCursorBuySell, void, __fastcall, (int32_t nParam, int32_t bAltFrame), 0xB7580)
//D2FUNC(D2CLIENT, CURSOR_SetHeldUnit, int32_t, __fastcall, (D2UnitStrc *pUnit), 0xB75B0)
//D2FUNC(D2CLIENT, CURSOR_GetHeldUnit, D2UnitStrc *, __cdecl, (), 0xB75E0)
//D2FUNC(D2CLIENT, CURSOR_Init, void, __fastcall, (int32_t nClickOffsetX), 0xB75F0)
//D2FUNC(D2CLIENT, CURSOR_UnloadCursors, void, __cdecl, (), 0xB76B0)
//D2FUNC(D2CLIENT, CURSOR_DrawCurrent, void, __fastcall, (), 0xB7700)
//D2FUNC(D2CLIENT, CURSOR_GetCursorLoops, int32_t, __fastcall, (int32_t nCursorType), 0xB79A0)
//D2FUNC(D2CLIENT, CURSOR_GetCursorFrameDuration, int32_t, __fastcall, (int32_t nCursorType), 0xB7A00)
//D2FUNC(D2CLIENT, CURSOR_DrawCursorBuySell, void, __fastcall, (), 0xB7A60)
//D2FUNC(D2CLIENT, CURSOR_DrawCursorDefault, void, __cdecl, (), 0xB7AC0)
//D2FUNC(D2CLIENT, CURSOR_GetMouseX, int32_t, __fastcall, (), 0xB7BC0)
//D2FUNC(D2CLIENT, CURSOR_GetMouseY, int32_t, __fastcall, (), 0xB7BD0)
//D2FUNC(D2CLIENT, CURSOR_SetCursorPos, void, __fastcall, (int32_t X, int32_t Y), 0xB7BE0)
//D2FUNC(D2CLIENT, CURSOR_GetCursorState, int32_t, __fastcall, (), 0xB7CC0)
//D2FUNC(D2CLIENT, GFX_HasOverlay, BOOL, __fastcall, (D2UnitStrc *pUnit, int32_t nOverlayId), 0xB7CD0)
//D2FUNC(D2CLIENT, GFX_DrawOverlayAt, void, __fastcall, (D2UnitStrc *pUnit, int32_t nOverlayId, int32_t nX, int32_t nY), 0xB7D20)
//D2FUNC(D2CLIENT, GFX_TickUnitOverlays, void, __fastcall, (D2UnitStrc *pUnit), 0xB7E80)
//D2FUNC(D2CLIENT, GFX_CycleAuraOverlay, void, __fastcall, (D2UnitStrc *pUnit, D2ClientUnitGfxInfoStrc *pGfxInfo, D2ClientGfxEntryStrc *pEntry), 0xB8170)
//D2FUNC(D2CLIENT, GFX_RemoveGfxEntry, void, __fastcall, (D2UnitStrc *pUnit, D2ClientGfxEntryStrc *pEntry), 0xB8270)
//D2FUNC(D2CLIENT, GFX_GetUnitCofData, void *, __fastcall, (D2UnitStrc *pUnit, int32_t nAnimMode), 0xB8320)
//D2FUNC(D2CLIENT, GFX_GetUnitAnimNode, D2ClientCofNodeStrc *, __fastcall, (D2UnitStrc *pUnit, int32_t nWeaponClassCode, int32_t nAnimMode), 0xB8350)
//D2FUNC(D2CLIENT, GFX_RebuildUnitGfx, D2ClientCofNodeStrc *, __fastcall, (D2UnitStrc *pUnit, int32_t nWeaponClassCode, int32_t nAnimMode), 0xB8410)
//D2FUNC(D2CLIENT, GFX_GetUnitCofNode, void *, __fastcall, (D2UnitStrc *pUnit, void **ppReplacedCofData, int32_t nAnimMode), 0xB8720)
//D2FUNC(D2CLIENT, GFX_ExtractFileTitle, void, __fastcall, (const char *szPath, char *szTitle), 0xB8AC0)
//D2FUNC(D2CLIENT, GFX_LoadCofFile, D2ClientCofCacheNodeStrc *, __fastcall, (D2ClientCofCacheStrc *pCache, const char *pszFileName), 0xB8B10)
//D2FUNC(D2CLIENT, GFX_FreeUnitAnimNodeByCof, BOOL, __fastcall, (D2UnitStrc *pUnit, D2ClientCofCacheNodeStrc *pCofFile), 0xB8DC0)
//D2FUNC(D2CLIENT, GFX_LoadMonsterModeGfx, void, __fastcall, (D2UnitStrc *pUnit, int32_t nMode), 0xB8E40)
//D2FUNC(D2CLIENT, GFX_LoadObjectModeGfx, void, __fastcall, (D2UnitStrc *pUnit, int32_t nMode), 0xB8F90)
//D2FUNC(D2CLIENT, GFX_InitCelCache, void, __fastcall, (int32_t nCofMemoryBudget), 0xB90B0)
//D2FUNC(D2CLIENT, GFX_FreeCelCache, void, __cdecl, (), 0xB9140)
//D2FUNC(D2CLIENT, GFX_AllocUnitGfxInfo, D2ClientUnitGfxInfoStrc *, __cdecl, (), 0xB92C0)
//D2FUNC(D2CLIENT, GFX_FreeUnitGfxInfo, void, __fastcall, (D2UnitStrc *pUnit), 0xB9320)
//D2FUNC(D2CLIENT, GFX_FreeUnitAnimList, void, __fastcall, (D2UnitStrc *pUnit), 0xB93C0)
//D2FUNC(D2CLIENT, GFX_FreeUnitAnimListButDeath, void, __fastcall, (D2UnitStrc *pUnit), 0xB9420)
//D2FUNC(D2CLIENT, GFX_DrawUnitCelShadow, void, __fastcall, (D2UnitStrc *pUnit), 0xB9490)
//D2FUNC(D2CLIENT, GFX_DrawUnitShadow, void, __fastcall, (D2UnitStrc *pUnit), 0xB9670)
//D2FUNC(D2CLIENT, GFX_DrawUnitCelHilight, void, __fastcall, (D2UnitStrc *pUnit), 0xB9C10)
//D2FUNC(D2CLIENT, GFX_GetUnitHilightPaletteIndex, uint8_t, __fastcall, (D2UnitStrc *pUnit), 0xB9DD0)
//D2FUNC(D2CLIENT, GFX_DrawUnitHilight, void, __fastcall, (D2UnitStrc *pUnit), 0xB9F20)
//D2FUNC(D2CLIENT, GFX_DrawInventoryItem, void, __fastcall, (D2UnitStrc *pItem, int32_t nX, int32_t nY), 0xBA320)
//D2FUNC(D2CLIENT, GFX_DrawUnitOverlays, void, __fastcall, (D2UnitStrc *pUnit, uint32_t nColor, int32_t bPreDrawLayer, int32_t nX, int32_t nY, int32_t bShiftUp8), 0xBA490)
//D2FUNC(D2CLIENT, GFX_DrawUnitCels, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t nColor, int32_t nX, int32_t nY, int32_t bGhostly, int32_t bDrawCel), 0xBA720)
//D2FUNC(D2CLIENT, GFX_DrawFilledRect, void, __fastcall, (int32_t nX, int32_t nY, int32_t nWidth, int32_t nHeight, uint8_t nColor, int32_t eDrawMode), 0xBB0F0)
//D2FUNC(D2CLIENT, GFX_ResetUnitGfx, void, __fastcall, (D2UnitStrc *pUnit), 0xBB120)
//D2FUNC(D2CLIENT, GFX_SetAllUnitModesLoaded, void, __fastcall, (D2UnitStrc *pUnit, uint8_t bLoaded), 0xBB1D0)
//D2FUNC(D2CLIENT, GFX_PreloadVisibleUnits, void, __fastcall, (int32_t bAsync), 0xBB210)
//D2FUNC(D2CLIENT, GFX_AsyncLoadBlock, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t nAnimMode, int32_t bAsync), 0xBB5A0)
//D2FUNC(D2CLIENT, GFX_PreloadAnimMode, void, __fastcall, (D2UnitStrc *pUnit, int32_t nAnimMode, int32_t bAsync), 0xBB920)
//D2FUNC(D2CLIENT, GFX_GetUnitParticles, D2ClientParticleStrc *, __fastcall, (D2UnitStrc *pUnit), 0xBBA40)
//D2FUNC(D2CLIENT, GFX_SetUnitParticles, void, __fastcall, (D2UnitStrc *pUnit, D2ClientParticleStrc *pParticle), 0xBBA50)
//D2FUNC(D2CLIENT, GFX_AddOverlay, void, __fastcall, (D2UnitStrc *pUnit, D2_OverlayIds nOverlayId, D2C_OverlayTypes nKind, int32_t nRandomStartFrames, int32_t nTargetOrStateId, int32_t nX, int32_t nY, int32_t nRateJitter), 0xBBA70)
//D2FUNC(D2CLIENT, GFX_TestFlagEx, int32_t, __fastcall, (D2UnitStrc *pUnit, int32_t nFlagEx), 0xBBEA0)
//D2FUNC(D2CLIENT, GFX_RemoveOverlay, void, __fastcall, (D2UnitStrc *pUnit, int32_t nUnused, D2_OverlayIds nOverlayId), 0xBBEC0)
//D2FUNC(D2CLIENT, GFX_RemoveAllOverlays, void, __fastcall, (D2UnitStrc *pUnit), 0xBBF70)
//D2FUNC(D2CLIENT, GFX_SetPaletteShift1, void, __fastcall, (D2ClientUnitGfxInfoStrc *pGfxData, int32_t nShift), 0xBBFC0)
//D2FUNC(D2CLIENT, GFX_GetPaletteShift1, int32_t, __fastcall, (D2ClientUnitGfxInfoStrc *ptInfo), 0xBBFF0)
//D2FUNC(D2CLIENT, GFX_SetPaletteShift2, void, __fastcall, (D2ClientUnitGfxInfoStrc *pGfxData, int32_t nShift), 0xBC020)
//D2FUNC(D2CLIENT, GFX_GetPaletteShift2, int32_t, __fastcall, (D2ClientUnitGfxInfoStrc *ptInfo), 0xBC050)
//D2FUNC(D2CLIENT, GFX_RebuildUnitAnimForMode, void, __fastcall, (D2UnitStrc *pUnit, int32_t nUnused), 0xBC080)
//D2FUNC(D2CLIENT, GFX_ClearUnitModesLoaded, void, __fastcall, (D2UnitStrc *pUnit, D2Composits nComponent), 0xBC2E0)
//D2FUNC(D2CLIENT, GFX_GetUnitCel, void, __fastcall, (D2UnitStrc *pUnit, D2Composits nComponent, int32_t *pnOffsetX, int32_t *pnOffsetY), 0xBC3C0)
//D2FUNC(D2CLIENT, GFX_HitTestUnitCof, BOOL, __fastcall, (D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nHitX, int32_t nHitY), 0xBC600)
//D2FUNC(D2CLIENT, GFX_IsUnitCofOnScreen, BOOL, __fastcall, (D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nUnused, int32_t bShadow), 0xBC7D0)
//D2FUNC(D2CLIENT, GFX_LoadActCofData, void, __fastcall, (uint8_t nActIndex), 0xBC9E0)
//D2FUNC(D2CLIENT, GFX_FreeCofCache, void, __cdecl, (), 0xBCB60)
//D2FUNC(D2CLIENT, GFX_GetMonStats2Record, D2MonStats2Txt *, __fastcall, (int32_t nMonStats2Id), 0xBCC10)
//D2FUNC(D2CLIENT, GFX_AddMonsterCofFiles, void, __fastcall, (int32_t nMonsterId, BOOL bValidateSprites), 0xBCC40)
//D2FUNC(D2CLIENT, GFX_HashCofName, uint32_t, __fastcall, (const char *pszFileName), 0xBD040)
//D2FUNC(D2CLIENT, GFX_BuildMonsterCofName, void, __fastcall, (D2MonStatsTxt *pMonStatsTxt, int32_t nMonsterId, int32_t nMonModeIndex, int32_t *pnWeaponClassCode, LPSTR pszOut), 0xBD0B0)
//D2FUNC(D2CLIENT, GFX_PreloadOnLoadComplete, void, __fastcall, (uint8_t nAct), 0xBD290)
//D2FUNC(D2CLIENT, GFX_PreloadAllAnimModes, void, __fastcall, (D2UnitStrc *pUnit, int32_t bAsync), 0xBD630)
//D2FUNC(D2CLIENT, GFX_PreloadPlayerMode, void, __fastcall, (D2UnitStrc *pUnit, int32_t nAnimMode, int32_t bAsync), 0xBD830)
//D2FUNC(D2CLIENT, GFX_PreloadAllPlayerModes, void, __fastcall, (D2UnitStrc *pUnit, int32_t bAsync), 0xBD930)
//D2FUNC(D2CLIENT, GFX_StopIfAlive, void, __fastcall, (D2UnitStrc *pUnit), 0xBDA40)
//D2FUNC(D2CLIENT, GFX_GetGfxMemoryUsed, int32_t, __cdecl, (), 0xBDA70)
//D2FUNC(D2CLIENT, GFX_GetCofMemoryUsed, int32_t, __cdecl, (), 0xBDA80)
//D2FUNC(D2CLIENT, GFX_GetCofMemoryBudget, int32_t, __cdecl, (), 0xBDA90)
//D2FUNC(D2CLIENT, GFX_GetMonsterPreloadEntry, D2ClientModePreloadStrc *, __fastcall, (int32_t nMonsterId), 0xBDAA0)
//D2FUNC(D2CLIENT, GFX_PreloadUnit, void, __fastcall, (D2UnitStrc *pUnit), 0xBDAB0)
//D2FUNC(D2CLIENT, GFXUTIL_GetEffectiveClassId, int32_t, __fastcall, (D2UnitStrc *pUnit), 0xBDD70)
//D2FUNC(D2CLIENT, GFXUTIL_GetUnitDisplayInventory, D2InventoryStrc *, __fastcall, (D2UnitStrc *pUnit, BOOL *pbFromOwner), 0xBDDE0)
//D2FUNC(D2CLIENT, GFXUTIL_CacheTokens, void, __cdecl, (), 0xBDE40)
//D2FUNC(D2CLIENT, GFXUTIL_GetUnitCelSize, void, __fastcall, (D2UnitStrc *pUnit, int32_t nDirection, int32_t nFrame, int32_t *pnWidth, int32_t *pnHeight), 0xBDFB0)
//D2FUNC(D2CLIENT, GFXUTIL_IsUnitCelOnScreen, BOOL, __fastcall, (D2UnitStrc *pUnit, int32_t nX, int32_t nY), 0xBE030)
//D2FUNC(D2CLIENT, GFXUTIL_IsCelVisible, int32_t, __fastcall, (D2GfxCellStrc *pCel, int32_t nX, int32_t nY, int32_t bShear), 0xBE170)
//D2FUNC(D2CLIENT, GFXUTIL_UnitHasComponent, BOOL, __fastcall, (D2UnitStrc *pUnit, int32_t nComponent), 0xBE200)
//D2FUNC(D2CLIENT, GFXUTIL_ResolveUnitAnimIdentity, int32_t, __fastcall, (D2UnitStrc *pUnit, D2GfxDataStrc *pGfxData, int32_t *pnClass, int32_t nModeOverride), 0xBE370)
//D2FUNC(D2CLIENT, GFXUTIL_FillGfxData, int32_t, __fastcall, (D2GfxDataStrc *pGfxData, D2UnitStrc *pUnit, D2CellFileStrc *pCellFile, int32_t nDirection, int32_t nFrame, int32_t *pnClass, int32_t nComponent, int32_t nModeOverride), 0xBEC80)
//D2FUNC(D2CLIENT, GFXUTIL_FillCelContextTokens, int32_t, __fastcall, (D2GfxDataStrc *pGfxData, int32_t nDirection, int32_t nFrame, int32_t nComponent, int32_t nAnimMode, int32_t dwWClass, D2ClientCofCacheNodeStrc *pCofNode, D2ClientMonCofTokensStrc *pMonTokens, D2MonStatsTxt *pMonStatsTxt, int32_t nCofDirection), 0xBECC0)
//D2FUNC(D2CLIENT, GFXUTIL_ReturnArg, void *, __fastcall, (void *p), 0xBEF30)
//D2FUNC(D2CLIENT, GFXUTIL_GetCofLayerOverrideTransLvl, int32_t, __fastcall, (uint8_t *pCofData, int32_t nComponent), 0xBEF40)
//D2FUNC(D2CLIENT, GFXUTIL_GetCofLayerCastsShadow, int32_t, __fastcall, (uint8_t *pCofData, int32_t nComponent), 0xBEF80)
//D2FUNC(D2CLIENT, GFXUTIL_GetCofLayerOrder, uint8_t *, __fastcall, (uint8_t *pCofData, int32_t nDirection, int32_t nFrame), 0xBEFC0)
//D2FUNC(D2CLIENT, GFXUTIL_GetCofLayerTransLvl, int32_t, __fastcall, (uint8_t *pCofData, int32_t nComponent), 0xBEFF0)
//D2FUNC(D2CLIENT, GFXUTIL_IsDrawableUnitType, BOOL, __fastcall, (D2UnitStrc *pUnit), 0xBF030)
//D2FUNC(D2CLIENT, GFXUTIL_IsUnitModeLoaded, uint8_t, __fastcall, (D2UnitStrc *pUnit, int32_t nMode), 0xBF050)
//D2FUNC(D2CLIENT, GFXUTIL_SetUnitModeLoaded, void, __fastcall, (D2UnitStrc *pUnit, int32_t nMode, uint8_t bLoaded), 0xBF070)
//D2FUNC(D2CLIENT, GFXUTIL_ResolveAnimDirection, void, __fastcall, (D2UnitStrc *pUnit, D2C_UnitTypes nUnitType, int32_t nClassId, int32_t nAnimMode, int32_t *pnUnitDirection, int32_t *pnCelDirection, int32_t *pnNumDirections, int32_t nCofDirections, int32_t bMayRemap), 0xBF090)
//D2FUNC(D2CLIENT, GFXUTIL_MeasureLayersBottomY, int32_t, __fastcall, (D2UnitStrc *pUnit, uint8_t *pCofData, uint8_t *pLayerOrder, int32_t *pnBottomY, int32_t nDirection, int32_t nFrame), 0xBF330)
//D2FUNC(D2CLIENT, GFXUTIL_ResolveUnitTransLevel, BOOL, __fastcall, (D2UnitStrc *pUnit, D2Composits nComponent, int32_t *pnTransLevel, int32_t nItemClassId), 0xBF420)
//D2FUNC(D2CLIENT, GFXUTIL_GetComponentColor, uint8_t *, __fastcall, (D2UnitStrc *pUnit, int32_t nComponent, uint8_t *pDefaultPalette), 0xBF6B0)
//D2FUNC(D2CLIENT, GFXUTIL_IsUnitComponentVisible, BOOL, __fastcall, (D2UnitStrc *pUnit, D2Composits nComponent), 0xBF7F0)
//D2FUNC(D2CLIENT, GFXUTIL_GetOverlayFrameCount, int32_t, __fastcall, (D2UnitStrc *pUnit, D2ClientGfxEntryStrc *pEntry), 0xBF9D0)
//D2FUNC(D2CLIENT, GFXUTIL_CopyTokenPadded, void, __fastcall, (const char *pszToken, char *pszOut), 0xBFA60)
//D2FUNC(D2CLIENT, GFXUTIL_GetOverlayTxtRecord, D2OverlayTxt *, __fastcall, (int32_t nOverlayId), 0xBFAB0)
//D2FUNC(D2CLIENT, GFXUTIL_GetUniqueItemsTxtRecord, D2UniqueItemsTxt *, __fastcall, (int32_t nUniqueItemId), 0xBFAE0)
//D2FUNC(D2CLIENT, PARTICLE_AllocUnitParticles, void, __stdcall, (D2UnitStrc *pUnit), 0xBFB10)
//D2FUNC(D2CLIENT, PARTICLE_FreeUnitParticles, void, __stdcall, (D2UnitStrc *pUnit), 0xBFB70)
//D2FUNC(D2CLIENT, PARTICLE_GetUnitParticleOffsetX, int32_t, __stdcall, (D2UnitStrc *pUnit), 0xBFBA0)
//D2FUNC(D2CLIENT, PARTICLE_GetUnitParticleOffsetY, int32_t, __stdcall, (D2UnitStrc *pUnit), 0xBFBC0)
//D2FUNC(D2CLIENT, PARTICLE_GetUnitParticleOffsetZ, int32_t, __stdcall, (D2UnitStrc *pUnit), 0xBFBE0)
//D2FUNC(D2CLIENT, PARTICLE_GetUnitGfxCoordX, int32_t, __stdcall, (D2UnitStrc *pUnit), 0xBFC00)
//D2FUNC(D2CLIENT, PARTICLE_GetUnitGfxCoordY, int32_t, __stdcall, (D2UnitStrc *pUnit), 0xBFC20)
//D2FUNC(D2CLIENT, PARTICLE_GetUnitGfxCoordZ, int32_t, __stdcall, (D2UnitStrc *pUnit), 0xBFC40)
//D2FUNC(D2CLIENT, PARTICLE_GetUnitGfxCoordZRaw, int32_t, __stdcall, (D2UnitStrc *pUnit), 0xBFC60)
//D2FUNC(D2CLIENT, PARTICLE_SetUnitGfxCoords, D2ClientParticleStrc *, __stdcall, (D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nZ), 0xBFC80)
//D2FUNC(D2CLIENT, PARTICLE_SetUnitGfxVector, D2ClientParticleStrc *, __stdcall, (D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nZ, int32_t bScale), 0xBFCB0)
//D2FUNC(D2CLIENT, PARTICLE_SetUnitGfxVector2, D2ClientParticleStrc *, __stdcall, (D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nZ, int32_t bScale), 0xBFD00)
//D2FUNC(D2CLIENT, PARTICLE_SetUnitParticleVector, D2ClientParticleStrc *, __stdcall, (D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nZ, int32_t bFixedPoint), 0xBFD50)
//D2FUNC(D2CLIENT, PARTICLE_StartUnitBounce, D2ClientParticleStrc *, __stdcall, (D2UnitStrc *pUnit, int32_t nBouncesLeft, int32_t nRestitution), 0xBFDA0)
//D2FUNC(D2CLIENT, PARTICLE_GetUnitBouncesLeft, int32_t, __stdcall, (D2UnitStrc *pUnit), 0xBFDD0)
//D2FUNC(D2CLIENT, PARTICLE_ClearUnitTimedMotion, D2ClientParticleStrc *, __fastcall, (D2UnitStrc *pUnit), 0xBFDF0)
//D2FUNC(D2CLIENT, PARTICLE_UpdateUnitParticle, void, __stdcall, (D2UnitStrc *pUnit), 0xBFE00)
//D2FUNC(D2CLIENT, PARTICLE_StartUnitArc, void, __fastcall, (D2UnitStrc *pUnit, int32_t nStartHeight, int32_t nDurationFrames), 0xC0080)
//D2FUNC(D2CLIENT, PARTICLE_SetUnitAttached, D2ClientParticleStrc *, __fastcall, (D2UnitStrc *pUnit, int32_t bAttached), 0xC00F0)
//D2FUNC(D2CLIENT, PARTICLE_StopUnitMotion, D2ClientParticleStrc *, __stdcall, (D2UnitStrc *pUnit), 0xC0110)
//D2FUNC(D2CLIENT, PARTICLE_SetUnitParticleFlag, D2ClientParticleStrc *, __stdcall, (D2UnitStrc *pUnit), 0xC0130)
//D2FUNC(D2CLIENT, PARTICLE_IsUnitMotionActive, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xC0150)
//D2FUNC(D2CLIENT, PARTICLE_SetUnitRisesToTarget, D2ClientParticleStrc *, __fastcall, (D2UnitStrc *pUnit, int32_t bRising), 0xC0180)

//  D2FUNC(D2CLIENT, STORM_SMemFree, BOOL, __stdcall, (void *pMemory, const char *szLogFile, int nLogLine, unsigned int dwFlags), 0xC01A0)   -- now STORM_SMemFree
//  D2FUNC(D2CLIENT, STORM_SMemAlloc, void *, __stdcall, (size_t nSize, const char *szFile, int nLine, unsigned int dwFlags), 0xC01A6)   -- now STORM_SMemAlloc
//  D2FUNC(D2CLIENT, STORM_SCmdRegisterArgument, int, __stdcall, (unsigned int dwFlags, int nId, const char *szName, void *pBuffer, size_t nBufferSize, int a6, int a7, int a8), 0xC01AC)   -- now STORM_SCmdRegisterArgument
//  D2FUNC(D2CLIENT, STORM_SErrRegisterMessageSource, int, __stdcall, (unsigned int dwBaseMessageId, HMODULE hModule, unsigned int dwFlags), 0xC01B2)   -- now STORM_SErrRegisterMessageSource
//  D2FUNC(D2CLIENT, STORM_SRegLoadString, int, __stdcall, (const char *szKeyName, const char *szValueName, unsigned int dwFlags, char *pBuffer, unsigned int nSize), 0xC01B8)   -- now STORM_SRegLoadString
//  D2FUNC(D2CLIENT, STORM_SRegSaveString, int, __stdcall, (const char *szKeyName, const char *szValueName, unsigned int dwFlags, const char *szValue), 0xC01BE)   -- now STORM_SRegSaveString
//  D2FUNC(D2CLIENT, STORM_SErrGetErrorStr, int, __stdcall, (unsigned int dwMessageId, char *pBuffer, unsigned int nSize), 0xC01C4)   -- now STORM_SErrGetErrorStr
//  D2FUNC(D2CLIENT, STORM_SFileCloseFile, int, __stdcall, (void *hFile), 0xC01CA)   -- now STORM_SFileCloseFile
//  D2FUNC(D2CLIENT, STORM_SFileOpenFileEx, int, __stdcall, (int hMpq, char *szFileName, char nSearchScope, int *phFile), 0xC01D0)   -- now STORM_SFileOpenFileEx
//  D2FUNC(D2CLIENT, STORM_SStrCmp, int, __stdcall, (const char *pStr1, const char *pStr2, size_t nMaxCount), 0xC01D6)   -- now STORM_SStrCmp
//  D2FUNC(D2CLIENT, STORM_SCmdProcess, int, __stdcall, (const char *szCommandLine, int a2, BOOL (__stdcall *a3)(int), BOOL (__stdcall *a4)(int)), 0xC01DC)   -- now STORM_SCmdProcess
//  D2FUNC(D2CLIENT, STORM_SCmdRegisterArgList, BOOL, __stdcall, (const SCmdArg *pArgs, int nArgsCount), 0xC01E2)   -- now STORM_SCmdRegisterArgList
//  D2FUNC(D2CLIENT, STORM_SMsgDispatchMessage, int, __stdcall, (HWND hWnd, unsigned int uMessage, unsigned int wParam, int lParam, BOOL *pbReturn, LRESULT *plResult), 0xC01E8)   -- now STORM_SMsgDispatchMessage
//  D2FUNC(D2CLIENT, STORM_SMsgRegisterKeyDown, int, __stdcall, (HWND hWnd, unsigned int nVirtKey, void (__stdcall *pfnHandler)(struct SMSGHANDLER_PARAMS *)), 0xC01EE)   -- now STORM_SMsgRegisterKeyDown
//  D2FUNC(D2CLIENT, STORM_SMsgRegisterKeyUp, int, __stdcall, (HWND hWnd, unsigned int nVirtKey, void (__stdcall *pfnHandler)(struct SMSGHANDLER_PARAMS *)), 0xC01F4)   -- now STORM_SMsgRegisterKeyUp
//  D2FUNC(D2CLIENT, STORM_SMsgRegisterCommand, int, __stdcall, (HWND hWnd, unsigned int nCommandId, void (__stdcall *pfnHandler)(struct SMSGHANDLER_PARAMS *)), 0xC01FA)   -- now STORM_SMsgRegisterCommand
//  D2FUNC(D2CLIENT, STORM_SMsgRegisterMessage, int, __stdcall, (HWND hWnd, unsigned int uMessage, void (__stdcall *pfnHandler)(struct SMSGHANDLER_PARAMS *)), 0xC0200)   -- now STORM_SMsgRegisterMessage
//  D2FUNC(D2CLIENT, STORM_SMsgUnregisterKeyDown, int, __stdcall, (HWND hWnd, unsigned int nVirtKey, void (__stdcall *pfnHandler)(struct SMSGHANDLER_PARAMS *)), 0xC0206)   -- now STORM_SMsgUnregisterKeyDown
//  D2FUNC(D2CLIENT, STORM_SMsgUnregisterKeyUp, int, __stdcall, (HWND hWnd, unsigned int nVirtKey, void (__stdcall *pfnHandler)(struct SMSGHANDLER_PARAMS *)), 0xC020C)   -- now STORM_SMsgUnregisterKeyUp
//  D2FUNC(D2CLIENT, STORM_SMsgUnregisterCommand, int, __stdcall, (HWND hWnd, unsigned int nCommandId, void (__stdcall *pfnHandler)(struct SMSGHANDLER_PARAMS *)), 0xC0212)   -- now STORM_SMsgUnregisterCommand
//  D2FUNC(D2CLIENT, STORM_SMsgUnregisterMessage, int, __stdcall, (HWND hWnd, unsigned int uMessage, void (__stdcall *pfnHandler)(struct SMSGHANDLER_PARAMS *)), 0xC0218)   -- now STORM_SMsgUnregisterMessage
//  D2FUNC(D2CLIENT, STORM_SMsgPushRegisterState, int, __stdcall, (HWND hWnd), 0xC021E)   -- now STORM_SMsgPushRegisterState
//  D2FUNC(D2CLIENT, STORM_SMsgPopRegisterState, int, __stdcall, (HWND hWnd), 0xC0224)   -- now STORM_SMsgPopRegisterState
//  D2FUNC(D2CLIENT, STORM_SMsgBreakHandlerChain, int, __stdcall, (SMSGHANDLER_PARAMS *pParams), 0xC022A)   -- now STORM_SMsgBreakHandlerChain
//  D2FUNC(D2CLIENT, STORM_SStrCopy, size_t, __stdcall, (char *pDest, const char *pSrc, size_t nMaxLength), 0xC0230)   -- now STORM_SStrCopy
//  D2FUNC(D2CLIENT, STORM_SMemReAlloc, void *, __stdcall, (void *pMemory, size_t nSize, const char *szFile, int nLine, unsigned int dwFlags), 0xC0236)   -- now STORM_SMemReAlloc
//  D2FUNC(D2CLIENT, STORM_SStrLen, size_t, __stdcall, (const char *pStr), 0xC023C)   -- now STORM_SStrLen
//  D2FUNC(D2CLIENT, STORM_SRegSaveValue, int, __stdcall, (const char *szKeyName, const char *szValueName, unsigned int dwFlags, unsigned int dwValue), 0xC0242)   -- now STORM_SRegSaveValue
//  D2FUNC(D2CLIENT, STORM_SStrPrintf, int, __cdecl, (char *pBuffer, size_t nBufferCount, const char *szFormat, ...), 0xC0248)   -- now STORM_SStrPrintf
//  D2FUNC(D2CLIENT, STORM_SRegLoadValue, int, __stdcall, (const char *szKeyName, const char *szValueName, unsigned int dwFlags, unsigned int *pValue), 0xC024E)   -- now STORM_SRegLoadValue
//  D2FUNC(D2CLIENT, STORM_SStrTokenize, void, __stdcall, (const char **ppszString, char *pBuffer, size_t nBufferChars, const char *szDelimiters, int *pnQuoted), 0xC0254)   -- now STORM_SStrTokenize
//  D2FUNC(D2CLIENT, STORM_SStrCmpI, int, __stdcall, (const char *pStr1, const char *pStr2, size_t nMaxCount), 0xC025A)   -- now STORM_SStrCmpI
//  D2FUNC(D2CLIENT, STORM_SStrChr, char *, __stdcall, (const char *str, char c), 0xC0260)   -- now STORM_SStrChr
//  D2FUNC(D2CLIENT, STORM_SStrHash, unsigned int, __stdcall, (const char *pStr, unsigned int nHashType, unsigned int dwSeed), 0xC0266)   -- now STORM_SStrHash

//D2FUNC(D2CLIENT, DllEntryPoint, BOOL, __stdcall, (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved), 0xC1C1D)

//  D2FUNC(D2CLIENT, FOG_Free, int, __fastcall, (_DWORD, _DWORD, _DWORD, _DWORD), 0xCA6B0)   -- now FOG_Free
//  D2FUNC(D2CLIENT, FOG_DisplayAssert, int, __cdecl, (_DWORD, _DWORD, _DWORD), 0xCA6B6)   -- now FOG_DisplayAssert
//  D2FUNC(D2CLIENT, FOG_Trace, int, __cdecl, (const char *, ...), 0xCA6BC)   -- now FOG_Trace
//  D2FUNC(D2CLIENT, FOG_GetSystemInfo, int, __cdecl, (), 0xCA6C2)   -- now FOG_GetSystemInfo
//  D2FUNC(D2CLIENT, FOG_GetInstallPath, int, __fastcall, (_DWORD, _DWORD), 0xCA6C8)   -- now FOG_GetInstallPath
//  D2FUNC(D2CLIENT, FOG_10191, int, __cdecl, (), 0xCA6CE)   -- now FOG_10191
//  D2FUNC(D2CLIENT, FOG_Alloc, int, __fastcall, (_DWORD, _DWORD, _DWORD, _DWORD), 0xCA6D4)   -- now FOG_Alloc
//  D2FUNC(D2CLIENT, FOG_IsExpansion, int, __cdecl, (), 0xCA6DA)   -- now FOG_IsExpansion
//  D2FUNC(D2CLIENT, FOG_SetMemoryPoolSystemMode, int, __cdecl, (_DWORD, _DWORD), 0xCA6E0)   -- now FOG_SetMemoryPoolSystemMode
//  D2FUNC(D2CLIENT, FOG_10144, int, __cdecl, (_DWORD), 0xCA6E6)   -- now FOG_10144
//  D2FUNC(D2CLIENT, FOG_10098, int, __cdecl, (), 0xCA6EC)   -- now FOG_10098
//  D2FUNC(D2CLIENT, FOG_DATAARRAYS_Init, int, __fastcall, (D2DataArrayStrc *pArray), 0xCA6F2)   -- now FOG_DATAARRAYS_Init
//  D2FUNC(D2CLIENT, FOG_DATAARRAYS_Alloc, int, __fastcall, (_DWORD, _DWORD, _DWORD), 0xCA6F8)   -- now FOG_DATAARRAYS_Alloc
//  D2FUNC(D2CLIENT, FOG_DATAARRAYS_Free, void, __fastcall, (D2DataArrayStrc *pArray), 0xCA6FE)   -- now FOG_DATAARRAYS_Free
//  D2FUNC(D2CLIENT, FOG_DATAARRAYS_Reset, int, __fastcall, (D2DataArrayStrc *pArray), 0xCA704)   -- now FOG_DATAARRAYS_Reset
//  D2FUNC(D2CLIENT, FOG_DATAARRAYS_FreeSlot, void, __fastcall, (D2DataArrayStrc *pArray, int hSlot), 0xCA70A)   -- now FOG_DATAARRAYS_FreeSlot
//  D2FUNC(D2CLIENT, FOG_DATAARRAYS_AllocSlot, int, __fastcall, (D2DataArrayStrc *pArray), 0xCA710)   -- now FOG_DATAARRAYS_AllocSlot
//  D2FUNC(D2CLIENT, FOG_10084_Sin_LUT, double, __stdcall, (_DWORD), 0xCA716)   -- now FOG_10084_Sin_LUT
//  D2FUNC(D2CLIENT, FOG_10083_Cos_LUT, double, __stdcall, (_DWORD), 0xCA71C)   -- now FOG_10083_Cos_LUT
//  D2FUNC(D2CLIENT, FOG_CreateNewPoolSystem, int, __cdecl, (_DWORD, _DWORD, _DWORD, _DWORD), 0xCA722)   -- now FOG_CreateNewPoolSystem
//  D2FUNC(D2CLIENT, FOG_DestroyMemoryPoolSystem, int, __cdecl, (_DWORD), 0xCA728)   -- now FOG_DestroyMemoryPoolSystem
//  D2FUNC(D2CLIENT, FOG_GetSavePath, int, __fastcall, (_DWORD, _DWORD), 0xCA72E)   -- now FOG_GetSavePath
//  D2FUNC(D2CLIENT, FOG_DisplayWarning, int, __cdecl, (_DWORD, _DWORD, _DWORD), 0xCA734)   -- now FOG_DisplayWarning
//  D2FUNC(D2CLIENT, FOG_DisplayHalt, int, __cdecl, (const char *pszMessage, const char *pszFile, int nLine), 0xCA73A)   -- now FOG_DisplayHalt
//  D2FUNC(D2CLIENT, FOG_BITMANIP_Read, uint32_t, __stdcall, (D2BitBufferStrc *pBuffer, int nBits), 0xCA740)   -- now FOG_BITMANIP_Read
//  D2FUNC(D2CLIENT, FOG_BITMANIP_Initialize, void, __stdcall, (D2BitBufferStrc *pBuffer, uint8_t *pBitStream, size_t nSize), 0xCA746)   -- now FOG_BITMANIP_Initialize
//  D2FUNC(D2CLIENT, FOG_BITMANIP_ReadSigned, int, __stdcall, (D2BitBufferStrc *pBuffer, int nBits), 0xCA74C)   -- now FOG_BITMANIP_ReadSigned
//  D2FUNC(D2CLIENT, FOG_10055_GetSyncTime, int, __cdecl, (), 0xCA752)   -- now FOG_10055_GetSyncTime
//  D2FUNC(D2CLIENT, FOG_CloseFile, BOOL, __fastcall, (HANDLE hObject), 0xCA758)   -- now FOG_CloseFile
//  D2FUNC(D2CLIENT, FOG_10111, int, __fastcall, (_DWORD, _DWORD, _DWORD, _DWORD, _DWORD), 0xCA75E)   -- now FOG_10111
//  D2FUNC(D2CLIENT, FOG_10112, int, __fastcall, (_DWORD, _DWORD, _DWORD, _DWORD), 0xCA764)   -- now FOG_10112
//  D2FUNC(D2CLIENT, FOG_10110, int, __fastcall, (_DWORD, _DWORD, _DWORD, _DWORD, _DWORD), 0xCA76A)   -- now FOG_10110
//  D2FUNC(D2CLIENT, FOG_DeleteFileA, BOOL, __fastcall, (LPCSTR lpFileName), 0xCA770)   -- now FOG_DeleteFileA
//  D2FUNC(D2CLIENT, FOG_CreateFileA, int, __fastcall, (_DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD), 0xCA776)   -- now FOG_CreateFileA
//  D2FUNC(D2CLIENT, FOG_CreatePathHierarchy, BOOL, __fastcall, (char *szPath), 0xCA77C)   -- now FOG_CreatePathHierarchy
//  D2FUNC(D2CLIENT, FOG_GetHostIPAddress, const char *, __fastcall, (), 0xCA782)   -- now FOG_GetHostIPAddress
//  D2FUNC(D2CLIENT, FOG_AllocPool, int, __fastcall, (_DWORD, _DWORD, _DWORD, _DWORD, _DWORD), 0xCA788)   -- now FOG_AllocPool
//  D2FUNC(D2CLIENT, FOG_FreePool, int, __fastcall, (void *pMemPool, void *pMemory, const char *szFile, int nLine, int a5), 0xCA78E)   -- now FOG_FreePool
//  D2FUNC(D2CLIENT, FOG_FreeBinFile, int, __stdcall, (_DWORD), 0xCA794)   -- now FOG_FreeBinFile
//  D2FUNC(D2CLIENT, FOG_10207, int, __stdcall, (_DWORD, _DWORD, _DWORD, _DWORD, _DWORD), 0xCA79A)   -- now FOG_10207
//  D2FUNC(D2CLIENT, FOG_GetRecordCountFromBinFile, int, __stdcall, (_DWORD), 0xCA7A0)   -- now FOG_GetRecordCountFromBinFile
//  D2FUNC(D2CLIENT, FOG_CreateBinFile, int, __stdcall, (_DWORD, _DWORD), 0xCA7A6)   -- now FOG_CreateBinFile
//  D2FUNC(D2CLIENT, FOG_AsyncDataFree, void, __fastcall, (void *a1), 0xCA7AC)   -- now FOG_AsyncDataFree
//  D2FUNC(D2CLIENT, FOG_AsyncDataGetBufferSize, int, __fastcall, (void *pAsyncData), 0xCA7B2)   -- now FOG_AsyncDataGetBufferSize
//  D2FUNC(D2CLIENT, FOG_AsyncDataGetBuffer, int, __fastcall, (void *pAsyncData), 0xCA7B8)   -- now FOG_AsyncDataGetBuffer
//  D2FUNC(D2CLIENT, FOG_AsyncDataTestLoaded, int, __fastcall, (int pAsyncData), 0xCA7BE)   -- now FOG_AsyncDataTestLoaded
//  D2FUNC(D2CLIENT, FOG_AsyncDataLoadFileEx, int, __fastcall, (_DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD), 0xCA7C4)   -- now FOG_AsyncDataLoadFileEx
//  D2FUNC(D2CLIENT, FOG_FCloseFile, int, __fastcall, (_DWORD), 0xCA7CA)   -- now FOG_FCloseFile
//  D2FUNC(D2CLIENT, FOG_FGetFileSize, int, __fastcall, (_DWORD, _DWORD), 0xCA7D0)   -- now FOG_FGetFileSize
//  D2FUNC(D2CLIENT, FOG_FOpenFile, int, __fastcall, (_DWORD, _DWORD), 0xCA7D6)   -- now FOG_FOpenFile
//  D2FUNC(D2CLIENT, FOG_IsHandlingError, int, __cdecl, (), 0xCA7DC)   -- now FOG_IsHandlingError
//  D2FUNC(D2CLIENT, FOG_csprintf, int, __cdecl, (_DWORD, const char *, ...), 0xCA7E2)   -- now FOG_csprintf
//  D2FUNC(D2CLIENT, RtlUnwind, void, __stdcall, (PVOID TargetFrame, PVOID TargetIp, PEXCEPTION_RECORD ExceptionRecord, PVOID ReturnValue), 0xCA7E8)   -- KERNEL32, called directly
//  D2FUNC(D2CLIENT, ImmReleaseContext, BOOL, __stdcall, (HWND, HIMC), 0xCA7EE)   -- IMM32, called directly
//  D2FUNC(D2CLIENT, ImmGetOpenStatus, BOOL, __stdcall, (HIMC), 0xCA7F4)   -- IMM32, called directly
//  D2FUNC(D2CLIENT, ImmGetCompositionStringA, LONG, __stdcall, (HIMC, DWORD, LPVOID lpBuf, DWORD dwBufLen), 0xCA7FA)   -- IMM32, called directly
//  D2FUNC(D2CLIENT, ImmGetContext, HIMC, __stdcall, (HWND), 0xCA800)   -- IMM32, called directly
//  D2FUNC(D2CLIENT, ImmIsIME, BOOL, __stdcall, (HKL), 0xCA806)   -- IMM32, called directly
//  D2FUNC(D2CLIENT, ImmGetCandidateListA, DWORD, __stdcall, (HIMC, DWORD deIndex, LPCANDIDATELIST lpCandList, DWORD dwBufLen), 0xCA80C)   -- IMM32, called directly
//  D2FUNC(D2CLIENT, ImmGetCandidateListCountA, DWORD, __stdcall, (HIMC, LPDWORD lpdwListCount), 0xCA812)   -- IMM32, called directly
//  D2FUNC(D2CLIENT, ImmGetConversionStatus, BOOL, __stdcall, (HIMC, LPDWORD lpfdwConversion, LPDWORD lpfdwSentence), 0xCA818)   -- IMM32, called directly
//  D2FUNC(D2CLIENT, ImmSimulateHotKey, BOOL, __stdcall, (HWND, DWORD), 0xCA81E)   -- IMM32, called directly
//  D2FUNC(D2CLIENT, ImmSetConversionStatus, BOOL, __stdcall, (HIMC, DWORD, DWORD), 0xCA824)   -- IMM32, called directly
//  D2FUNC(D2CLIENT, ImmSetOpenStatus, BOOL, __stdcall, (HIMC, BOOL), 0xCA82A)   -- IMM32, called directly
//  D2FUNC(D2CLIENT, D2CMP_CelFileNormalize, void, __stdcall, (D2CellFileStrc *pFile, D2CellFileStrc **ppOutFile, const char *szFile, int nLine, int nSpecVersion, int nUnused), 0xCA830)   -- now D2CMP_CelFileNormalize
//  D2FUNC(D2CLIENT, D2CMP_GetGfxFileExtension, char *, __stdcall, (int nGfxType), 0xCA836)   -- now D2CMP_GetGfxFileExtension
//  D2FUNC(D2CLIENT, D2CMP_CelFileFreeHardware, BOOL, __stdcall, (D2CellFileStrc *pFile), 0xCA83C)   -- now D2CMP_CelFileFreeHardware
//  D2FUNC(D2CLIENT, D2CMP_InitSpriteCache, int, __stdcall, (int a1, int a2, int a3, unsigned int a4), 0xCA842)   -- now D2CMP_InitSpriteCache
//  D2FUNC(D2CLIENT, D2CMP_10089, int, __stdcall, (int a1, int a2), 0xCA848)   -- now D2CMP_10089
//  D2FUNC(D2CLIENT, D2CMP_FlushSpriteCache, int, __stdcall, (int a1), 0xCA84E)   -- now D2CMP_FlushSpriteCache
//  D2FUNC(D2CLIENT, D2CMP_10090, int, __stdcall, (_DWORD), 0xCA854)   -- now D2CMP_10090
//  D2FUNC(D2CLIENT, D2CMP_10092, int, __stdcall, (_DWORD, _DWORD, _DWORD, _DWORD), 0xCA85A)   -- now D2CMP_10092
//  D2FUNC(D2CLIENT, D2CMP_10063, int, __cdecl, (), 0xCA860)   -- now D2CMP_10063
//  D2FUNC(D2CLIENT, D2CMP_10064, int, __cdecl, (), 0xCA866)   -- now D2CMP_10064
//  D2FUNC(D2CLIENT, D2CMP_10060, int, __cdecl, (), 0xCA86C)   -- now D2CMP_10060
//  D2FUNC(D2CLIENT, D2CMP_10061, int, __cdecl, (), 0xCA872)   -- now D2CMP_10061
//  D2FUNC(D2CLIENT, D2CMP_10093, int, __cdecl, (), 0xCA878)   -- now D2CMP_10093
//  D2FUNC(D2CLIENT, D2CMP_CelFileGetCelsPerDirection, int, __stdcall, (D2CellFileStrc *pCelFile), 0xCA87E)   -- now D2CMP_CelFileGetCelsPerDirection
//  D2FUNC(D2CLIENT, D2CMP_10095, int, __stdcall, (_DWORD, _DWORD), 0xCA884)   -- now D2CMP_10095
//  D2FUNC(D2CLIENT, D2CMP_10096, int, __stdcall, (), 0xCA88A)   -- now D2CMP_10096
//  D2FUNC(D2CLIENT, D2CMP_TileFreeAsyncLoads, int, __cdecl, (), 0xCA890)   -- now D2CMP_TileFreeAsyncLoads
//  D2FUNC(D2CLIENT, D2CMP_SpriteFreeAsyncLoads, int, __cdecl, (), 0xCA896)   -- now D2CMP_SpriteFreeAsyncLoads
//  D2FUNC(D2CLIENT, D2CMP_SetCompressedDataMode, int, __stdcall, (_DWORD), 0xCA89C)   -- now D2CMP_SetCompressedDataMode
//  D2FUNC(D2CLIENT, D2CMP_10084, int, __stdcall, (_DWORD), 0xCA8A2)   -- now D2CMP_10084
//  D2FUNC(D2CLIENT, D2CMP_10082_GetTileSequence, int, __stdcall, (_DWORD), 0xCA8A8)   -- now D2CMP_10082_GetTileSequence
//  D2FUNC(D2CLIENT, D2CMP_10078_GetTileStyle, int, __stdcall, (_DWORD), 0xCA8AE)   -- now D2CMP_10078_GetTileStyle
//  D2FUNC(D2CLIENT, D2CMP_10077_GetTileType, int, __stdcall, (_DWORD), 0xCA8B4)   -- now D2CMP_10077_GetTileType
//  D2FUNC(D2CLIENT, D2CMP_CelGetWidth, int, __stdcall, (D2GfxCellStrc *pCel), 0xCA8BA)   -- now D2CMP_CelGetWidth
//  D2FUNC(D2CLIENT, D2CMP_CelGetHeight, int, __stdcall, (D2GfxCellStrc *pCel), 0xCA8C0)   -- now D2CMP_CelGetHeight
//  D2FUNC(D2CLIENT, D2CMP_CelGetHandle, D2GfxCellStrc *, __stdcall, (D2GfxDataStrc *pGfxData), 0xCA8C6)   -- now D2CMP_CelGetHandle
//  D2FUNC(D2CLIENT, D2CMP_10097, int, __stdcall, (_DWORD), 0xCA8CC)   -- now D2CMP_10097
//  D2FUNC(D2CLIENT, D2CMP_SpriteValidate, BOOL, __stdcall, (D2GfxDataStrc *pGfxData, BOOL bAsyncWait, BOOL bAsyncLoad), 0xCA8D2)   -- now D2CMP_SpriteValidate
//  D2FUNC(D2CLIENT, D2CMP_CelGetOffsetY, int, __stdcall, (D2GfxCellStrc *pCel), 0xCA8D8)   -- now D2CMP_CelGetOffsetY
//  D2FUNC(D2CLIENT, D2CMP_CelGetOffsetX, int, __stdcall, (D2GfxCellStrc *pCel), 0xCA8DE)   -- now D2CMP_CelGetOffsetX
//  D2FUNC(D2CLIENT, D2CMP_10058, int, __stdcall, (_DWORD, _DWORD, _DWORD), 0xCA8E4)   -- now D2CMP_10058
//  D2FUNC(D2CLIENT, D2CMP_10057, int, __cdecl, (), 0xCA8EA)   -- now D2CMP_10057
//  D2FUNC(D2CLIENT, D2CMP_10056, int, __stdcall, (_DWORD), 0xCA8F0)   -- now D2CMP_10056
//  D2FUNC(D2CLIENT, D2CMP_10022, int, __stdcall, (_DWORD, _DWORD), 0xCA8F6)   -- now D2CMP_10022
//  D2FUNC(D2CLIENT, D2CMP_10019, int, __stdcall, (_DWORD, _DWORD, _DWORD, _DWORD), 0xCA8FC)   -- now D2CMP_10019
//  D2FUNC(D2CLIENT, D2COMMON_LOG_11105, void, __cdecl, (), 0xCA902)   -- now D2COMMON_LOG_11105
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_UnloadAllBins, void, __stdcall, (), 0xCA908)   -- now D2COMMON_DATATBLS_UnloadAllBins
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_LoadAllTxts, void, __stdcall, (HD2ARCHIVE hArchive, int a2, int a3), 0xCA90E)   -- now D2COMMON_DATATBLS_LoadAllTxts
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GetTileDataFromAct, D2DrlgTileDataStrc *, __stdcall, (D2DrlgActStrc *pAct), 0xCA914)   -- now D2COMMON_DUNGEON_GetTileDataFromAct
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GameSubtileToClientCoords, void, __stdcall, (int *pX, int *pY), 0xCA91A)   -- now D2COMMON_DUNGEON_GameSubtileToClientCoords
//  D2FUNC(D2CLIENT, D2COMMON_PATH_GetYPosition, int, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCA920)   -- now D2COMMON_PATH_GetYPosition
//  D2FUNC(D2CLIENT, D2COMMON_PATH_GetXPosition, int, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCA926)   -- now D2COMMON_PATH_GetXPosition
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GetFloorTilesFromRoom, D2DrlgTileDataStrc *, __stdcall, (D2ActiveRoomStrc *pRoom, int *pFloorCount), 0xCA92C)   -- now D2COMMON_DUNGEON_GetFloorTilesFromRoom
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GetRoomCoordinates, void, __stdcall, (D2ActiveRoomStrc *pRoom, D2DrlgCoordsStrc *pCoords), 0xCA932)   -- now D2COMMON_DUNGEON_GetRoomCoordinates
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GetAdjacentRoomsListFromRoom, void, __stdcall, (D2ActiveRoomStrc *pRoom, D2ActiveRoomStrc ***pppRoomList, int *pNumRooms), 0xCA938)   -- now D2COMMON_DUNGEON_GetAdjacentRoomsListFromRoom
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetLevelsTxtRecord, D2LevelsTxt *, __stdcall, (int nLevelId), 0xCA93E)   -- now D2COMMON_DATATBLS_GetLevelsTxtRecord
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GetLevelIdFromRoom, int, __stdcall, (D2ActiveRoomStrc *pRoom), 0xCA944)   -- now D2COMMON_DUNGEON_GetLevelIdFromRoom
//  D2FUNC(D2CLIENT, D2COMMON_ENVIRONMENT_GetLightColorFromAct, void, __stdcall, (D2DrlgActStrc *pAct, uint8_t *pRed, uint8_t *pGreen, uint8_t *pBlue), 0xCA94A)   -- now D2COMMON_ENVIRONMENT_GetLightColorFromAct
//  D2FUNC(D2CLIENT, D2COMMON_ENVIRONMENT_GetIntensityFromAct, int, __stdcall, (D2DrlgActStrc *pAct), 0xCA950)   -- now D2COMMON_ENVIRONMENT_GetIntensityFromAct
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetPrecisionY, int, __stdcall, (D2UnitStrc *pUnit), 0xCA956)   -- now D2COMMON_UNITS_GetPrecisionY
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetPrecisionX, int, __stdcall, (D2UnitStrc *pUnit), 0xCA95C)   -- now D2COMMON_UNITS_GetPrecisionX
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GetRGB_IntensityFromRoom, void, __stdcall, (D2ActiveRoomStrc *pRoom, uint8_t *pIntensity, uint8_t *pRed, uint8_t *pGreen, uint8_t *pBlue), 0xCA962)   -- now D2COMMON_DUNGEON_GetRGB_IntensityFromRoom
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetRoom, D2ActiveRoomStrc *, __stdcall, (D2UnitStrc *pUnit), 0xCA968)   -- now D2COMMON_UNITS_GetRoom
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_SetActCallbackFunc, void, __stdcall, (D2DrlgActStrc *pAct, ACTCALLBACKFN pActCallbackFunction), 0xCA96E)   -- now D2COMMON_DUNGEON_SetActCallbackFunc
//  D2FUNC(D2CLIENT, D2COMMON_COLLISION_CheckMask, uint16_t, __stdcall, (D2ActiveRoomStrc *pRoom, int nX, int nY, uint16_t nMask), 0xCA974)   -- now D2COMMON_COLLISION_CheckMask
//  D2FUNC(D2CLIENT, D2COMMON_DRLG_GetActNoFromLevelId, uint8_t, __stdcall, (D2C_Levels nLevelId), 0xCA97A)   -- now D2COMMON_DRLG_GetActNoFromLevelId
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetMudFromLevelsTxt, uint8_t, __stdcall, (int nLevelId), 0xCA980)   -- now D2COMMON_DATATBLS_GetMudFromLevelsTxt
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetRainFromLevelsTxt, uint8_t, __stdcall, (int nLevelId), 0xCA986)   -- now D2COMMON_DATATBLS_GetRainFromLevelsTxt
//  D2FUNC(D2CLIENT, D2COMMON_ENVIRONMENT_GetPeriodOfDayFromAct, int, __stdcall, (D2DrlgActStrc *pAct, int *pBaseTime), 0xCA98C)   -- now D2COMMON_ENVIRONMENT_GetPeriodOfDayFromAct
//  D2FUNC(D2CLIENT, D2COMMON_ENVIRONMENT_InitializeEnvironment, void, __stdcall, (D2DrlgActStrc *pAct, D2ActiveRoomStrc *pRoom, int nIndex, int nTicks, BOOL bEclipse), 0xCA992)   -- now D2COMMON_ENVIRONMENT_InitializeEnvironment
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_AllocAct, D2DrlgActStrc *, __stdcall, (uint8_t nActNo, uint32_t nInitSeed, BOOL bClient, D2GameStrc *pGame, uint8_t nDifficulty, void *pMemPool, int nTownLevelId, AUTOMAPFN pfAutoMap, TOWNAUTOMAPFN pfTownAutoMap), 0xCA998)   -- now D2COMMON_DUNGEON_AllocAct
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_FreeAct, void, __stdcall, (D2DrlgActStrc *pAct), 0xCA99E)   -- now D2COMMON_DUNGEON_FreeAct
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetFileNameFromLevelTypeAndFileId, void, __stdcall, (int nLevelType, int nFile, char *szFile), 0xCA9A4)   -- now D2COMMON_DATATBLS_GetFileNameFromLevelTypeAndFileId
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_CheckActInLevelTypesTxt, bool, __stdcall, (int nLevelType, uint8_t nAct), 0xCA9AA)   -- now D2COMMON_DATATBLS_CheckActInLevelTypesTxt
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetClientCoordY, int, __stdcall, (D2UnitStrc *pUnit), 0xCA9B0)   -- now D2COMMON_UNITS_GetClientCoordY
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetClientCoordX, int, __stdcall, (D2UnitStrc *pUnit), 0xCA9B6)   -- now D2COMMON_UNITS_GetClientCoordX
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_UpdateAndFreeInactiveRooms, void, __stdcall, (D2DrlgActStrc *pAct), 0xCA9BC)   -- now D2COMMON_DUNGEON_UpdateAndFreeInactiveRooms
//  D2FUNC(D2CLIENT, D2COMMON_DRLGACTIVATE_Update, void, __stdcall, (D2DrlgStrc *pDrlg), 0xCA9C2)   -- now D2COMMON_DRLGACTIVATE_Update
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GetDrlgFromAct, D2DrlgStrc *, __stdcall, (D2DrlgActStrc *pAct), 0xCA9C8)   -- now D2COMMON_DUNGEON_GetDrlgFromAct
//  D2FUNC(D2CLIENT, D2COMMON_ENVIRONMENT_UpdatePeriodOfDay, BOOL, __stdcall, (D2DrlgActStrc *pAct, D2ActiveRoomStrc *pRoom), 0xCA9CE)   -- now D2COMMON_ENVIRONMENT_UpdatePeriodOfDay
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_AnimateTiles, void, __stdcall, (D2ActiveRoomStrc *pRoom), 0xCA9D4)   -- now D2COMMON_DUNGEON_AnimateTiles
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GetRoomFromAct, D2ActiveRoomStrc *, __stdcall, (D2DrlgActStrc *pAct), 0xCA9DA)   -- now D2COMMON_DUNGEON_GetRoomFromAct
//  D2FUNC(D2CLIENT, D2COMMON_LOG_11102, void, __fastcall, (void *a1), 0xCA9E0)   -- now D2COMMON_LOG_11102
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_ToggleCompileTxtFlag, void, __stdcall, (BOOL bSilent), 0xCA9E6)   -- now D2COMMON_DATATBLS_ToggleCompileTxtFlag
//  D2FUNC(D2CLIENT, D2COMMON_SEED_InitLowSeed, void, __fastcall, (D2SeedStrc *pSeed, int nLowSeed), 0xCA9EC)   -- now D2COMMON_SEED_InitLowSeed
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GetInitSeedFromAct, int, __stdcall, (D2DrlgActStrc *pAct), 0xCA9F2)   -- now D2COMMON_DUNGEON_GetInitSeedFromAct
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetPlayerData, D2PlayerDataStrc *, __stdcall, (D2UnitStrc *pUnit), 0xCA9F8)   -- now D2COMMON_UNITS_GetPlayerData
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_ChangeAnimMode, BOOL, __stdcall, (D2UnitStrc *pUnit, int nMode), 0xCA9FE)   -- now D2COMMON_UNITS_ChangeAnimMode
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_AreUnitsAligned, BOOL, __stdcall, (D2UnitStrc *pUnit1, D2UnitStrc *pUnit2), 0xCAA04)   -- now D2COMMON_STATLIST_AreUnitsAligned
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_SetUnitStat, void, __stdcall, (D2UnitStrc *pUnit, D2C_ItemStats nStatId, int nValue, uint16_t nLayer), 0xCAA0A)   -- now D2COMMON_STATLIST_SetUnitStat
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_GetMaxLifeFromUnit, int, __stdcall, (D2UnitStrc *pUnit), 0xCAA10)   -- now D2COMMON_STATLIST_GetMaxLifeFromUnit
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_SetClientIsInSight, void, __stdcall, (D2DrlgActStrc *pAct, D2C_Levels nLevelId, int nX, int nY, D2ActiveRoomStrc *pRoom), 0xCAA16)   -- now D2COMMON_DUNGEON_SetClientIsInSight
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_UnsetClientIsInSight, void, __stdcall, (D2DrlgActStrc *pAct, D2C_Levels nLevelId, int nX, int nY, D2ActiveRoomStrc *pRoom), 0xCAA1C)   -- now D2COMMON_DUNGEON_UnsetClientIsInSight
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_GetUnitBaseStat, int, __stdcall, (D2UnitStrc *pUnit, D2C_ItemStats nStatId, uint16_t nLayer), 0xCAA22)   -- now D2COMMON_STATLIST_GetUnitBaseStat
//  D2FUNC(D2CLIENT, D2COMMON_10083_Return0, int, __stdcall, (D2ActiveRoomStrc *pRoom), 0xCAA28)   -- now D2COMMON_10083_Return0
//  D2FUNC(D2CLIENT, D2COMMON_PATH_SetFootprintCollisionMask, void, __stdcall, (D2DynamicPathStrc *pDynamicPath, int nCollisionMask), 0xCAA2E)   -- now D2COMMON_PATH_SetFootprintCollisionMask
//  D2FUNC(D2CLIENT, D2COMMON_MONSTERS_IsDead, BOOL, __stdcall, (D2UnitStrc *pMonster), 0xCAA34)   -- now D2COMMON_MONSTERS_IsDead
//  D2FUNC(D2CLIENT, D2COMMON_PATH_SetMoveTestCollisionMask, void, __stdcall, (D2DynamicPathStrc *pDynamicPath, int nCollisionMask), 0xCAA3A)   -- now D2COMMON_PATH_SetMoveTestCollisionMask
//  D2FUNC(D2CLIENT, D2COMMON_10214, void, __stdcall, (D2UnitStrc *pUnit), 0xCAA40)   -- now D2COMMON_10214
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_SetRightActiveSkill, void, __stdcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, D2UnitGUID nOwnerGUID), 0xCAA46)   -- now D2COMMON_SKILLS_SetRightActiveSkill
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_SetLeftActiveSkill, void, __stdcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, D2UnitGUID nOwnerGUID), 0xCAA4C)   -- now D2COMMON_SKILLS_SetLeftActiveSkill
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_MergeStatLists, void, __stdcall, (D2UnitStrc *pTarget, D2UnitStrc *pUnit, BOOL bType), 0xCAA52)   -- now D2COMMON_STATLIST_MergeStatLists
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_ExpireUnitStatlist, void, __stdcall, (D2UnitStrc *pUnused, D2UnitStrc *pUnit), 0xCAA58)   -- now D2COMMON_STATLIST_ExpireUnitStatlist
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_GetOwner, D2UnitStrc *, __stdcall, (D2UnitStrc *pUnit, BOOL *pStatNotDynamic), 0xCAA5E)   -- now D2COMMON_STATLIST_GetOwner
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_UnitIsItem, D2UnitStrc *, __stdcall, (D2UnitStrc *pItem), 0xCAA64)   -- now D2COMMON_INVENTORY_UnitIsItem
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_GetNextItem, D2UnitStrc *, __stdcall, (D2UnitStrc *pItem), 0xCAA6A)   -- now D2COMMON_INVENTORY_GetNextItem
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_GetFirstItem, D2UnitStrc *, __stdcall, (D2InventoryStrc *pInventory), 0xCAA70)   -- now D2COMMON_INVENTORY_GetFirstItem
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_AddUnitStat, void, __stdcall, (D2UnitStrc *pUnit, D2C_ItemStats nStatId, int nValue, uint16_t nLayer), 0xCAA76)   -- now D2COMMON_STATLIST_AddUnitStat
//  D2FUNC(D2CLIENT, D2COMMON_STATES_ToggleState, void, __stdcall, (D2UnitStrc *pUnit, D2C_States nState, BOOL bSet), 0xCAA7C)   -- now D2COMMON_STATES_ToggleState
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_UnitGetStatValue, int32_t, __stdcall, (const D2UnitStrc *pUnit, D2C_ItemStats nStatId, uint16_t nLayer), 0xCAA82)   -- now D2COMMON_STATLIST_UnitGetStatValue
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_AssignSkill, void, __stdcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int nSkillLevel, BOOL bRemove, const char *szFile, int nLine), 0xCAA88)   -- now D2COMMON_SKILLS_AssignSkill
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_SetQuantity, void, __stdcall, (D2SkillStrc *pSkill, int nQuantity), 0xCAA8E)   -- now D2COMMON_SKILLS_SetQuantity
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetSkillById, D2SkillStrc *, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, D2UnitGUID nOwnerGUID), 0xCAA94)   -- now D2COMMON_SKILLS_GetSkillById
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_SetPlayerPortalFlags, void, __stdcall, (D2UnitStrc *pUnit, int nPortalFlags), 0xCAA9A)   -- now D2COMMON_UNITS_SetPlayerPortalFlags
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_SetCurrentFrame, void, __stdcall, (D2UnitStrc *pMissile, int nCurrentFrame), 0xCAAA0)   -- now D2COMMON_MISSILE_SetCurrentFrame
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_StoreOwnerTypeAndGUID, void, __stdcall, (D2UnitStrc *pUnit, int nOwnerType, D2UnitGUID nOwnerId), 0xCAAA6)   -- now D2COMMON_UNITS_StoreOwnerTypeAndGUID
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_AddOrChangeRunesTxtRecord, void, __stdcall, (int nRecordId, D2RunesTxt *pRecord), 0xCAAAC)   -- now D2COMMON_DATATBLS_AddOrChangeRunesTxtRecord
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_SetStatIfListIsValid, void, __stdcall, (D2StatListStrc *pStatList, D2C_ItemStats nStatId, int nValue, uint16_t nLayer), 0xCAAB2)   -- now D2COMMON_STATLIST_SetStatIfListIsValid
//  D2FUNC(D2CLIENT, D2COMMON_10475_PostStatToStatList, void, __stdcall, (D2UnitStrc *pUnit, D2StatListStrc *pStatList, BOOL bResetFlag), 0xCAAB8)   -- now D2COMMON_10475_PostStatToStatList
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_SetState, void, __stdcall, (D2StatListStrc *pStatList, D2C_States nState), 0xCAABE)   -- now D2COMMON_STATLIST_SetState
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_AllocStatList, D2StatListStrc *, __stdcall, (void *pMemPool, uint32_t fFilter, uint32_t dwTimeout, D2C_UnitTypes nUnitType, D2UnitGUID nUnitGUID), 0xCAAC4)   -- now D2COMMON_STATLIST_AllocStatList
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_GetStatListFromUnitStateAndFlag, D2StatListStrc *, __stdcall, (D2UnitStrc *pUnit, D2C_States nState, int nFlag), 0xCAACA)   -- now D2COMMON_STATLIST_GetStatListFromUnitStateAndFlag
//  D2FUNC(D2CLIENT, D2COMMON_MONSTERS_GetHirelingTypeId, int, __stdcall, (D2UnitStrc *pHireling), 0xCAAD0)   -- now D2COMMON_MONSTERS_GetHirelingTypeId
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_ClientToGameSubtileCoords, void, __stdcall, (int *pX, int *pY), 0xCAAD6)   -- now D2COMMON_DUNGEON_ClientToGameSubtileCoords
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GetLvlWarpTxtRecordFromRoomAndUnit, D2LvlWarpTxt *, __stdcall, (D2ActiveRoomStrc *pRoom, D2UnitStrc *pUnit), 0xCAADC)   -- now D2COMMON_DUNGEON_GetLvlWarpTxtRecordFromRoomAndUnit
//  D2FUNC(D2CLIENT, D2COMMON_10095, int, __stdcall, (D2ActiveRoomStrc *pRoom, int nX, int nY), 0xCAAE2)   -- now D2COMMON_10095
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GameTileToClientCoords, void, __stdcall, (int *pX, int *pY), 0xCAAE8)   -- now D2COMMON_DUNGEON_GameTileToClientCoords
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GetWallTilesFromRoom, D2DrlgTileDataStrc *, __stdcall, (D2ActiveRoomStrc *pRoom, int *pWallCount), 0xCAAEE)   -- now D2COMMON_DUNGEON_GetWallTilesFromRoom
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_IsDoor, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCAAF4)   -- now D2COMMON_UNITS_IsDoor
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_UpdateWarpRoomSelect, void, __stdcall, (D2ActiveRoomStrc *pRoom, D2C_Levels nLevelId), 0xCAAFA)   -- now D2COMMON_DUNGEON_UpdateWarpRoomSelect
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_UpdateWarpRoomDeselect, void, __stdcall, (D2ActiveRoomStrc *pRoom, D2C_Levels nLevelId), 0xCAB00)   -- now D2COMMON_DUNGEON_UpdateWarpRoomDeselect
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetObjectTxtRecordFromObject, D2ObjectsTxt *, __stdcall, (D2UnitStrc *pUnit), 0xCAB06)   -- now D2COMMON_UNITS_GetObjectTxtRecordFromObject
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetCoords, void, __stdcall, (D2UnitStrc *pUnit, D2CoordStrc *pCoord), 0xCAB0C)   -- now D2COMMON_UNITS_GetCoords
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckState, BOOL, __stdcall, (D2UnitStrc *pUnit, D2C_States nState), 0xCAB12)   -- now D2COMMON_STATES_CheckState
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_IsInMovingMode, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCAB18)   -- now D2COMMON_UNITS_IsInMovingMode
//  D2FUNC(D2CLIENT, D2COMMON_COLLISION_GetFreeCoordinatesWithMaxDistance, D2ActiveRoomStrc *, __stdcall, (D2ActiveRoomStrc *pRoom, D2CoordStrc *pSpawnPoint, int nUnitSize, unsigned int nMask, BOOL bAllowNeighborRooms, int nMaxDistance), 0xCAB1E)   -- now D2COMMON_COLLISION_GetFreeCoordinatesWithMaxDistance
//  D2FUNC(D2CLIENT, D2COMMON_UNITFINDS_FreeUnitFindData, void, __stdcall, (D2UnitFindDataStrc *pUnitFindData), 0xCAB24)   -- now D2COMMON_UNITFINDS_FreeUnitFindData
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetDistanceToCoordinates, unsigned int, __stdcall, (D2UnitStrc *pUnit, int nX, int nY), 0xCAB2A)   -- now D2COMMON_UNITS_GetDistanceToCoordinates
//  D2FUNC(D2CLIENT, D2COMMON_UNITFINDS_FindAllMatchingUnitsInNeighboredRooms, void, __stdcall, (D2UnitFindDataStrc *pUnitFindData), 0xCAB30)   -- now D2COMMON_UNITFINDS_FindAllMatchingUnitsInNeighboredRooms
//  D2FUNC(D2CLIENT, D2COMMON_UNITFINDS_InitializeUnitFindData, void, __stdcall, (void *pMemPool, D2UnitFindDataStrc *pUnitFindData, D2ActiveRoomStrc *pRoom, int nX, int nY, int nSize, UNITFINDTEST pfnUnitTest, D2UnitFindArgStrc *pUnitFindArg), 0xCAB36)   -- now D2COMMON_UNITFINDS_InitializeUnitFindData
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_IsMetalItem, int, __stdcall, (int nItemId), 0xCAB3C)   -- now D2COMMON_ITEMS_IsMetalItem
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetSkillIdFromSkill, int, __stdcall, (D2SkillStrc *pSkill, const char *szFile, int nLine), 0xCAB42)   -- now D2COMMON_SKILLS_GetSkillIdFromSkill
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill, D2SkillsTxt *, __fastcall, (D2SkillStrc *pSkill), 0xCAB48)   -- now D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_SetTargetUnitForDynamicUnit, void, __stdcall, (D2UnitStrc *pUnit, D2UnitStrc *pTargetUnit), 0xCAB4E)   -- now D2COMMON_UNITS_SetTargetUnitForDynamicUnit
//  D2FUNC(D2CLIENT, D2COMMON_10407, D2UnitStrc *, __stdcall, (D2ActiveRoomStrc *pRoom, int nX, int nY, int (__fastcall *pCallback)(D2UnitStrc *, void *), void *a5, int a6), 0xCAB54)   -- now D2COMMON_10407
//  D2FUNC(D2CLIENT, D2COMMON_COLLISION_CheckMaskWithSize, uint16_t, __stdcall, (D2ActiveRoomStrc *pRoom, int nX, int nY, int nUnitSize, uint16_t nMask), 0xCAB5A)   -- now D2COMMON_COLLISION_CheckMaskWithSize
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetNewDirection, int, __stdcall, (D2UnitStrc *pUnit), 0xCAB60)   -- now D2COMMON_UNITS_GetNewDirection
//  D2FUNC(D2CLIENT, D2COMMON_SEED_GetRandomValue, int, __fastcall, (int nValue), 0xCAB66)   -- now D2COMMON_SEED_GetRandomValue
//  D2FUNC(D2CLIENT, D2COMMON_SEED_InitSeed, void, __fastcall, (D2SeedStrc *pSeed), 0xCAB6C)   -- now D2COMMON_SEED_InitSeed
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_CallRoomCallback, void, __stdcall, (D2ActiveRoomStrc *pRoom, ROOMCALLBACKFN pfnRoomCallback, void *pArgs), 0xCAB72)   -- now D2COMMON_DUNGEON_CallRoomCallback
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_ClientTileDrawPositionToGameCoords, void, __stdcall, (int nX, int nY, int *pX, int *pY), 0xCAB78)   -- now D2COMMON_DUNGEON_ClientTileDrawPositionToGameCoords
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GetFirstUnitInRoom, D2UnitStrc *, __stdcall, (D2ActiveRoomStrc *pRoom), 0xCAB7E)   -- now D2COMMON_DUNGEON_GetFirstUnitInRoom
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GetRoofTilesFromRoom, D2DrlgTileDataStrc *, __stdcall, (D2ActiveRoomStrc *pRoom, int *pRoofCount), 0xCAB84)   -- now D2COMMON_DUNGEON_GetRoofTilesFromRoom
//  D2FUNC(D2CLIENT, D2COMMON_10052, void, __stdcall, (D2ActiveRoomStrc *pRoom, RECT *pRect), 0xCAB8A)   -- now D2COMMON_10052
//  D2FUNC(D2CLIENT, D2COMMON_UNITFINDS_FindUnitInNeighboredRooms, D2UnitStrc *, __stdcall, (D2ActiveRoomStrc *pRoom, D2C_UnitTypes nUnitType, int nClassId), 0xCAB90)   -- now D2COMMON_UNITFINDS_FindUnitInNeighboredRooms
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_SetTargetY, void, __stdcall, (D2UnitStrc *pMissile, int nTargetY), 0xCAB96)   -- now D2COMMON_MISSILE_SetTargetY
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_SetTargetX, void, __stdcall, (D2UnitStrc *pMissile, int nTargetX), 0xCAB9C)   -- now D2COMMON_MISSILE_SetTargetX
//  D2FUNC(D2CLIENT, D2COMMON_10170_PathSetTargetPos, void, __stdcall, (D2DynamicPathStrc *pDynamicPath, int nX, int nY), 0xCABA2)   -- now D2COMMON_10170_PathSetTargetPos
//  D2FUNC(D2CLIENT, D2COMMON_10176_PathGetFirstPointY, int, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCABA8)   -- now D2COMMON_10176_PathGetFirstPointY
//  D2FUNC(D2CLIENT, D2COMMON_10175_PathGetFirstPointX, int, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCABAE)   -- now D2COMMON_10175_PathGetFirstPointX
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetDirection, uint8_t, __stdcall, (D2UnitStrc *pUnit), 0xCABB4)   -- now D2COMMON_UNITS_GetDirection
//  D2FUNC(D2CLIENT, D2COMMON_PATH_ComputeSquaredDistance, int, __stdcall, (int nX1, int nY1, int nX2, int nY2), 0xCABBA)   -- now D2COMMON_PATH_ComputeSquaredDistance
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_CanItemBeUsedForThrowSkill, BOOL, __stdcall, (D2UnitStrc *pItem), 0xCABC0)   -- now D2COMMON_ITEMS_CanItemBeUsedForThrowSkill
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_CheckIfThrowable, int, __stdcall, (D2UnitStrc *pItem), 0xCABC6)   -- now D2COMMON_ITEMS_CheckIfThrowable
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_CheckIfStackable, uint32_t, __stdcall, (D2UnitStrc *pItem), 0xCABCC)   -- now D2COMMON_ITEMS_CheckIfStackable
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_GetItemFromBodyLoc, D2UnitStrc *, __stdcall, (D2InventoryStrc *pInventory, D2C_PlayerBodyLocs nBodyLoc), 0xCABD2)   -- now D2COMMON_INVENTORY_GetItemFromBodyLoc
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_UnitGetItemStatOrSkillStatValue, int32_t, __stdcall, (D2UnitStrc *pUnit, D2C_ItemStats nStatId, uint16_t nLayer), 0xCABD8)   -- now D2COMMON_STATLIST_UnitGetItemStatOrSkillStatValue
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetUsedSkill, D2SkillStrc *, __stdcall, (D2UnitStrc *pUnit), 0xCABDE)   -- now D2COMMON_UNITS_GetUsedSkill
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetAmmoType, int32_t, __stdcall, (D2UnitStrc *pItem), 0xCABE4)   -- now D2COMMON_ITEMS_GetAmmoType
//  D2FUNC(D2CLIENT, D2COMMON_10434, D2UnitStrc *, __stdcall, (D2UnitStrc *pUnit, BOOL a2), 0xCABEA)   -- now D2COMMON_10434
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_SetAnimationSpeed, void, __stdcall, (D2UnitStrc *pUnit, int nSpeed), 0xCABF0)   -- now D2COMMON_UNITS_SetAnimationSpeed
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_SwapCastLight, void *, __stdcall, (D2UnitStrc *pUnit, void *a2), 0xCABF6)   -- now D2COMMON_SKILLS_SwapCastLight
//  D2FUNC(D2CLIENT, D2COMMON_COLLISION_RayTrace, BOOL, __stdcall, (D2ActiveRoomStrc *pRoom, D2CoordStrc *pBeginCoord, D2CoordStrc *pEndCoord, uint16_t nCollisionMask), 0xCABFC)   -- now D2COMMON_COLLISION_RayTrace
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_IsInMeleeRange, BOOL, __stdcall, (D2UnitStrc *pUnit1, D2UnitStrc *pUnit2, int nRangeBonus), 0xCAC02)   -- now D2COMMON_UNITS_IsInMeleeRange
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_IsRoomInTown, BOOL, __stdcall, (D2ActiveRoomStrc *pRoom), 0xCAC08)   -- now D2COMMON_DUNGEON_IsRoomInTown
//  D2FUNC(D2CLIENT, D2COMMON_MONSTERS_IsPrimeEvil, BOOL, __stdcall, (D2UnitStrc *pMonster), 0xCAC0E)   -- now D2COMMON_MONSTERS_IsPrimeEvil
//  D2FUNC(D2CLIENT, D2COMMON_MONSTERS_IsBoss, BOOL, __stdcall, (D2MonStatsTxt *pMonStatsTxtRecord, D2UnitStrc *pMonster), 0xCAC14)   -- now D2COMMON_MONSTERS_IsBoss
//  D2FUNC(D2CLIENT, D2COMMON_MONSTERS_IsUndead, BOOL, __stdcall, (D2UnitStrc *pMonster), 0xCAC1A)   -- now D2COMMON_MONSTERS_IsUndead
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_CheckItemTypeId, BOOL, __stdcall, (const D2UnitStrc *pItem, D2C_ItemTypes nItemType), 0xCAC20)   -- now D2COMMON_ITEMS_CheckItemTypeId
//  D2FUNC(D2CLIENT, D2COMMON_COMPOSIT_IsWeaponBowOrXBow, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCAC26)   -- now D2COMMON_COMPOSIT_IsWeaponBowOrXBow
//  D2FUNC(D2CLIENT, D2COMMON_11039_CheckWeaponIsMissileBased, int, __stdcall, (D2UnitStrc *pUnit, int *pValue), 0xCAC2C)   -- now D2COMMON_11039_CheckWeaponIsMissileBased
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetItemType, D2C_ItemTypes, __stdcall, (D2UnitStrc *pItem), 0xCAC32)   -- now D2COMMON_ITEMS_GetItemType
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetMissileType, int, __stdcall, (D2UnitStrc *pItem), 0xCAC38)   -- now D2COMMON_ITEMS_GetMissileType
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetHitClass, int, __fastcall, (D2UnitStrc *pUnit), 0xCAC3E)   -- now D2COMMON_UNITS_GetHitClass
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetSkillModeFromUnit, int, __stdcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill), 0xCAC44)   -- now D2COMMON_SKILLS_GetSkillModeFromUnit
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_GetEquippedWeapon, BOOL, __stdcall, (D2InventoryStrc *pInventory, D2UnitStrc **ppItem, int *pBodyLoc, BOOL *pIsLeftHandItem), 0xCAC4A)   -- now D2COMMON_INVENTORY_GetEquippedWeapon
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetSkillLevel, int, __stdcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill, BOOL bBonus), 0xCAC50)   -- now D2COMMON_SKILLS_GetSkillLevel
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetSkillInfo, BOOL, __stdcall, (D2SkillStrc *pSkill, D2UnitGUID *pOwnerGUID, int *pSkillId, int *pSkillLevel, int *pCharges), 0xCAC56)   -- now D2COMMON_SKILLS_GetSkillInfo
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_SetUsedSkill, void, __stdcall, (D2UnitStrc *pUnit, D2SkillStrc *pUsedSkill), 0xCAC5C)   -- now D2COMMON_UNITS_SetUsedSkill
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_SetSkillLevelBonus, void, __stdcall, (D2UnitStrc *pUnit, int nSkillId, int a3), 0xCAC62)   -- now D2COMMON_SKILLS_SetSkillLevelBonus
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_SetFlags, void, __stdcall, (D2SkillStrc *pSkill, int nFlags), 0xCAC68)   -- now D2COMMON_SKILLS_SetFlags
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_AddSkill, D2SkillStrc *, __stdcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId), 0xCAC6E)   -- now D2COMMON_SKILLS_AddSkill
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_HasLineOfSightToUnit, BOOL, __stdcall, (int nX, int nY, D2UnitStrc *pUnit, uint16_t nColMask), 0xCAC74)   -- now D2COMMON_SKILLS_HasLineOfSightToUnit
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_EvaluateSkillFormula, int, __stdcall, (D2UnitStrc *pUnit, unsigned int nCalc, D2C_Skills nSkillId, int nSkillLevel), 0xCAC7A)   -- now D2COMMON_SKILLS_EvaluateSkillFormula
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_GetStatListFromUnitAndState, D2StatListStrc *, __stdcall, (D2UnitStrc *pUnit, D2C_States nState), 0xCAC80)   -- now D2COMMON_STATLIST_GetStatListFromUnitAndState
//  D2FUNC(D2CLIENT, D2COMMON_LOG_11100, void, __cdecl, (int a1, int a2, int a3, int a4, const char *szFile, int nLine, const char *szFormat, ...), 0xCAC86)   -- now D2COMMON_LOG_11100
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_CanDualWield, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCAC8C)   -- now D2COMMON_UNITS_CanDualWield
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_SetParam4, void, __stdcall, (D2SkillStrc *pSkill, int nPar4), 0xCAC92)   -- now D2COMMON_SKILLS_SetParam4
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetParam4, int, __stdcall, (D2SkillStrc *pSkill), 0xCAC98)   -- now D2COMMON_SKILLS_GetParam4
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_CanBeEquipped, BOOL, __stdcall, (D2UnitStrc *pItem), 0xCAC9E)   -- now D2COMMON_ITEMS_CanBeEquipped
//  D2FUNC(D2CLIENT, D2COMMON_10160_PathUpdateDirection, void, __stdcall, (D2DynamicPathStrc *pDynamicPath, uint8_t nDirection), 0xCACA4)   -- now D2COMMON_10160_PathUpdateDirection
//  D2FUNC(D2CLIENT, D2COMMON_PATH_GetNewDirection, uint8_t, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCACAA)   -- now D2COMMON_PATH_GetNewDirection
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_UpdateDirectionAndSpeed, void, __stdcall, (D2UnitStrc *pUnit, int nX, int nY), 0xCACB0)   -- now D2COMMON_UNITS_UpdateDirectionAndSpeed
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_GetStatValue, int, __stdcall, (D2StatListStrc *pStatList, D2C_ItemStats nStatId, uint16_t nLayer), 0xCACB6)   -- now D2COMMON_STATLIST_GetStatValue
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskActiveOnUnit, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCACBC)   -- now D2COMMON_STATES_CheckStateMaskActiveOnUnit
//  D2FUNC(D2CLIENT, D2COMMON_PATH_SetDirection, void, __stdcall, (D2DynamicPathStrc *pDynamicPath, uint8_t nDirection), 0xCACC2)   -- now D2COMMON_PATH_SetDirection
//  D2FUNC(D2CLIENT, D2COMMON_COMPOSIT_GetWeaponClassId, int, __stdcall, (D2UnitStrc *pUnit, D2InventoryStrc *pInventory, int *pWeaponClassId, int nAnimMode, BOOL a5), 0xCACC8)   -- now D2COMMON_COMPOSIT_GetWeaponClassId
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_SetStatRemoveCallback, void, __stdcall, (D2StatListStrc *pStatList, StatListRemoveCallback pfStatRemove), 0xCACCE)   -- now D2COMMON_STATLIST_SetStatRemoveCallback
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetFlags, int, __stdcall, (D2SkillStrc *pSkill), 0xCACD4)   -- now D2COMMON_SKILLS_GetFlags
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_RefreshPassiveSkills, void, __stdcall, (D2UnitStrc *pUnit), 0xCACDA)   -- now D2COMMON_SKILLS_RefreshPassiveSkills
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_RemoveSkill, void, __stdcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, const char *szFile, int nLine), 0xCACE0)   -- now D2COMMON_SKILLS_RemoveSkill
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_AddSkillLevelBonus, void, __stdcall, (D2UnitStrc *pUnit, int nSkillId, int a3), 0xCACE6)   -- now D2COMMON_SKILLS_AddSkillLevelBonus
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetOwnerGUIDFromSkill, D2UnitGUID, __stdcall, (D2SkillStrc *pSkill), 0xCACEC)   -- now D2COMMON_SKILLS_GetOwnerGUIDFromSkill
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_IsEnhanceable, BOOL, __stdcall, (D2C_Skills nSkillId), 0xCACF2)   -- now D2COMMON_SKILLS_IsEnhanceable
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetNextSkill, D2SkillStrc *, __stdcall, (D2SkillStrc *pSkill), 0xCACF8)   -- now D2COMMON_SKILLS_GetNextSkill
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetFirstSkillFromSkillList, D2SkillStrc *, __stdcall, (D2SkillListStrc *pSkillList), 0xCACFE)   -- now D2COMMON_SKILLS_GetFirstSkillFromSkillList
//  D2FUNC(D2CLIENT, D2COMMON_COLLISION_CheckMaskWithSizeXY, uint16_t, __stdcall, (D2ActiveRoomStrc *pRoom, int nX, int nY, unsigned int nSizeX, unsigned int nSizeY, uint16_t nMask), 0xCAD04)   -- now D2COMMON_COLLISION_CheckMaskWithSizeXY
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetMeleeRange, int, __stdcall, (D2UnitStrc *pUnit), 0xCAD0A)   -- now D2COMMON_UNITS_GetMeleeRange
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_UpdateAnimRateAndVelocity, void, __stdcall, (D2UnitStrc *pUnit, const char *szFile, int nLine), 0xCAD10)   -- now D2COMMON_UNITS_UpdateAnimRateAndVelocity
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_SetParam1, void, __stdcall, (D2SkillStrc *pSkill, int nPar1), 0xCAD16)   -- now D2COMMON_SKILLS_SetParam1
//  D2FUNC(D2CLIENT, D2COMMON_PATH_FaceCoords, void, __stdcall, (D2DynamicPathStrc *pDynamicPath, int nX, int nY, int a4), 0xCAD1C)   -- now D2COMMON_PATH_FaceCoords
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_SetParam3, void, __stdcall, (D2SkillStrc *pSkill, int nPar3), 0xCAD22)   -- now D2COMMON_SKILLS_SetParam3
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_SetParam2, void, __stdcall, (D2SkillStrc *pSkill, int nPar2), 0xCAD28)   -- now D2COMMON_SKILLS_SetParam2
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetWeaponClass, int, __fastcall, (D2UnitStrc *pUnit), 0xCAD2E)   -- now D2COMMON_UNITS_GetWeaponClass
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetParam2, int, __stdcall, (D2SkillStrc *pSkill), 0xCAD34)   -- now D2COMMON_SKILLS_GetParam2
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetParam3, int, __stdcall, (D2SkillStrc *pSkill), 0xCAD3A)   -- now D2COMMON_SKILLS_GetParam3
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetParam1, int, __stdcall, (D2SkillStrc *pSkill), 0xCAD40)   -- now D2COMMON_SKILLS_GetParam1
//  D2FUNC(D2CLIENT, D2COMMON_10476, void, __stdcall, (D2StatListStrc *pStatList, int nExpireFrame), 0xCAD46)   -- now D2COMMON_10476
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_SetAnimationFrame, void, __stdcall, (D2UnitStrc *pUnit, int nFrame), 0xCAD4C)   -- now D2COMMON_UNITS_SetAnimationFrame
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId, D2SkillStrc *, __fastcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId), 0xCAD52)   -- now D2COMMON_SKILLS_GetHighestLevelSkillFromUnitAndId
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_SetStreamRange, void, __stdcall, (D2UnitStrc *pMissile, __int16 nStreamRange), 0xCAD58)   -- now D2COMMON_MISSILE_SetStreamRange
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_SetStreamMissile, void, __stdcall, (D2UnitStrc *pMissile, uint16_t nStreamMissile), 0xCAD5E)   -- now D2COMMON_MISSILE_SetStreamMissile
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetMissileVelocityFromMissilesTxt, int, __stdcall, (int nMissileId, int nLevel), 0xCAD64)   -- now D2COMMON_DATATBLS_GetMissileVelocityFromMissilesTxt
//  D2FUNC(D2CLIENT, D2COMMON_PATH_StartUnitMove, int, __stdcall, (D2DynamicPathStrc *pPath, D2UnitStrc *pUnit, int bAllowInTown), 0xCAD6A)   -- now D2COMMON_PATH_StartUnitMove
//  D2FUNC(D2CLIENT, D2COMMON_PATH_SetNewDistance, void, __stdcall, (D2DynamicPathStrc *pDynamicPath, uint8_t nNewDistance), 0xCAD70)   -- now D2COMMON_PATH_SetNewDistance
//  D2FUNC(D2CLIENT, D2COMMON_PATH_SetType, void, __stdcall, (D2DynamicPathStrc *pDynamicPath, int nPathType), 0xCAD76)   -- now D2COMMON_PATH_SetType
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_SetTotalFrames, void, __stdcall, (D2UnitStrc *pMissile, int nTotalFrames), 0xCAD7C)   -- now D2COMMON_MISSILE_SetTotalFrames
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_GetTotalFrames, int, __stdcall, (D2UnitStrc *pMissile), 0xCAD82)   -- now D2COMMON_MISSILE_GetTotalFrames
//  D2FUNC(D2CLIENT, D2COMMON_PATH_SetNumberOfPathPoints, void, __stdcall, (D2DynamicPathStrc *pDynamicPath, int a2), 0xCAD88)   -- now D2COMMON_PATH_SetNumberOfPathPoints
//  D2FUNC(D2CLIENT, D2COMMON_11053, uint8_t, __stdcall, (uint8_t a1), 0xCAD8E)   -- now D2COMMON_11053
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetDirectionToCoords, int, __stdcall, (D2UnitStrc *pUnit, int nNewX, int nNewY), 0xCAD94)   -- now D2COMMON_UNITS_GetDirectionToCoords
//  D2FUNC(D2CLIENT, D2COMMON_PATH_GetPathPoints, int, __stdcall, (D2DynamicPathStrc *pDynamicPath, D2PathPointStrc **ppPathPoints), 0xCAD9A)   -- now D2COMMON_PATH_GetPathPoints
//  D2FUNC(D2CLIENT, D2COMMON_SEED_GetLowSeed, uint32_t, __fastcall, (D2SeedStrc *pSeed), 0xCADA0)   -- now D2COMMON_SEED_GetLowSeed
//  D2FUNC(D2CLIENT, D2COMMON_11018, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCADA6)   -- now D2COMMON_11018
//  D2FUNC(D2CLIENT, D2COMMON_11019, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCADAC)   -- now D2COMMON_11019
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_IsSoftMonster, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCADB2)   -- now D2COMMON_UNITS_IsSoftMonster
//  D2FUNC(D2CLIENT, D2COMMON_COLLISION_GetFreeCoordinates, D2ActiveRoomStrc *, __stdcall, (D2ActiveRoomStrc *pRoom, D2CoordStrc *pSpawnPoint, int nUnitSize, unsigned int nMask, BOOL bAllowNeighborRooms), 0xCADB8)   -- now D2COMMON_COLLISION_GetFreeCoordinates
//  D2FUNC(D2CLIENT, D2COMMON_COLLISION_SetMaskWithPattern, void, __stdcall, (D2ActiveRoomStrc *pRoom, int nX, int nY, D2C_CollisionPattern nCollisionPattern, uint16_t nMask), 0xCADBE)   -- now D2COMMON_COLLISION_SetMaskWithPattern
//  D2FUNC(D2CLIENT, D2COMMON_PATH_GetUnitCollisionPattern, int, __stdcall, (D2UnitStrc *pUnit), 0xCADC4)   -- now D2COMMON_PATH_GetUnitCollisionPattern
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetUnitSizeX, int, __stdcall, (D2UnitStrc *pUnit), 0xCADCA)   -- now D2COMMON_UNITS_GetUnitSizeX
//  D2FUNC(D2CLIENT, D2COMMON_11025, BOOL, __stdcall, (int nX1, int nY1, int nX2, int nY2, D2ActiveRoomStrc *pRoom, int a6), 0xCADD0)   -- now D2COMMON_11025
//  D2FUNC(D2CLIENT, D2COMMON_COLLISION_CheckAnyCollisionWithPattern, int, __stdcall, (D2ActiveRoomStrc *pRoom, int nX, int nY, int nCollisionPattern, uint16_t nMask), 0xCADD6)   -- now D2COMMON_COLLISION_CheckAnyCollisionWithPattern
//  D2FUNC(D2CLIENT, D2COMMON_PATH_SetVelocity, void, __stdcall, (D2DynamicPathStrc *pDynamicPath, int nVelocity, const char *szFile, int nLine), 0xCADDC)   -- now D2COMMON_PATH_SetVelocity
//  D2FUNC(D2CLIENT, D2COMMON_PATH_GetVelocity, int, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCADE2)   -- now D2COMMON_PATH_GetVelocity
//  D2FUNC(D2CLIENT, D2COMMON_COLLISION_ResetMaskWithPattern, void, __stdcall, (D2ActiveRoomStrc *pRoom, int nX, int nY, D2C_CollisionPattern nCollisionPattern, uint16_t nMask), 0xCADE8)   -- now D2COMMON_COLLISION_ResetMaskWithPattern
//  D2FUNC(D2CLIENT, D2COMMON_PATH_GetFootprintCollisionMask, int, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCADEE)   -- now D2COMMON_PATH_GetFootprintCollisionMask
//  D2FUNC(D2CLIENT, D2COMMON_PATH_SetUnitDeadCollision, void, __stdcall, (D2UnitStrc *pUnit, BOOL bForGameLogic), 0xCADF4)   -- now D2COMMON_PATH_SetUnitDeadCollision
//  D2FUNC(D2CLIENT, D2COMMON_10190_PATH_SetDistance, void, __stdcall, (D2DynamicPathStrc *pDynamicPath, uint8_t nDistance), 0xCADFA)   -- now D2COMMON_10190_PATH_SetDistance
//  D2FUNC(D2CLIENT, D2COMMON_PATH_SetTargetUnit, void, __stdcall, (D2DynamicPathStrc *pDynamicPath, D2UnitStrc *pUnit), 0xCAE00)   -- now D2COMMON_PATH_SetTargetUnit
//  D2FUNC(D2CLIENT, D2COMMON_PATH_SetStepNum, void, __stdcall, (D2DynamicPathStrc *pDynamicPath, uint8_t nSteps), 0xCAE06)   -- now D2COMMON_PATH_SetStepNum
//  D2FUNC(D2CLIENT, D2COMMON_PATH_RecacheRoomIfNeeded, void, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCAE0C)   -- now D2COMMON_PATH_RecacheRoomIfNeeded
//  D2FUNC(D2CLIENT, D2COMMON_10212_PATH_SetMoveFlags, void, __stdcall, (D2UnitStrc *pUnit, BOOL bSet), 0xCAE12)   -- now D2COMMON_10212_PATH_SetMoveFlags
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_CheckIfCanLeapTo, BOOL, __stdcall, (D2UnitStrc *pUnit1, D2UnitStrc *pUnit2, int *pX, int *pY), 0xCAE18)   -- now D2COMMON_SKILLS_CheckIfCanLeapTo
//  D2FUNC(D2CLIENT, D2COMMON_PATH_GetDirection, uint8_t, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCAE1E)   -- now D2COMMON_PATH_GetDirection
//  D2FUNC(D2CLIENT, D2COMMON_10203_PATH_SetRotateFlag, void, __stdcall, (D2DynamicPathStrc *pDynamicPath, BOOL bReset), 0xCAE24)   -- now D2COMMON_10203_PATH_SetRotateFlag
//  D2FUNC(D2CLIENT, D2COMMON_11066, void, __stdcall, (D2UnitStrc *pMonster, int *pDirectionX, int *pDirectionY), 0xCAE2A)   -- now D2COMMON_11066
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_SetActivateFrame, void, __stdcall, (D2UnitStrc *pMissile, int nActivateFrame), 0xCAE30)   -- now D2COMMON_MISSILE_SetActivateFrame
//  D2FUNC(D2CLIENT, D2COMMON_10235_PATH_UpdateRiderPath, void, __stdcall, (D2UnitStrc *pRiderUnit, D2UnitStrc *pMountUnit), 0xCAE36)   -- now D2COMMON_10235_PATH_UpdateRiderPath
//  D2FUNC(D2CLIENT, D2COMMON_10231, int, __stdcall, (D2DynamicPathStrc *pDynamicPath, D2UnitStrc *pUnit_unused, D2ActiveRoomStrc *pRooms, uint16_t nX, uint16_t nY), 0xCAE3C)   -- now D2COMMON_10231
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_SetAnimStartFrame, void, __stdcall, (D2UnitStrc *pUnit), 0xCAE42)   -- now D2COMMON_UNITS_SetAnimStartFrame
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetShiftedFrameMetrics, void, __stdcall, (D2UnitStrc *pUnit, int *pFrameNo, int *pFrameCount), 0xCAE48)   -- now D2COMMON_UNITS_GetShiftedFrameMetrics
//  D2FUNC(D2CLIENT, D2COMMON_11055, void, __stdcall, (uint8_t a1, int *a2, int *a3), 0xCAE4E)   -- now D2COMMON_11055
//  D2FUNC(D2CLIENT, D2COMMON_PATH_GetNumberOfPathPoints, int, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCAE54)   -- now D2COMMON_PATH_GetNumberOfPathPoints
//  D2FUNC(D2CLIENT, D2COMMON_MONSTERS_GetSpawnMode_XY, int, __stdcall, (D2UnitStrc *pMonster, BOOL bFromMonster, int nSkillId, int nSkillLevel, int *pSpawnMode, int *pX, int *pY), 0xCAE5A)   -- now D2COMMON_MONSTERS_GetSpawnMode_XY
//  D2FUNC(D2CLIENT, D2COMMON_10399, int, __stdcall, (D2UnitStrc *pUnit1, D2UnitStrc *pUnit2), 0xCAE60)   -- now D2COMMON_10399
//  D2FUNC(D2CLIENT, D2COMMON_PATH_GetTargetUnit, D2UnitStrc *, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCAE66)   -- now D2COMMON_PATH_GetTargetUnit
//  D2FUNC(D2CLIENT, D2COMMON_PATH_HasMovedThisFrame, BOOL, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCAE6C)   -- now D2COMMON_PATH_HasMovedThisFrame
//  D2FUNC(D2CLIENT, D2COMMON_11017_CheckUnitIfConsumeable, BOOL, __stdcall, (D2UnitStrc *pUnit, int a2), 0xCAE72)   -- now D2COMMON_11017_CheckUnitIfConsumeable
//  D2FUNC(D2CLIENT, D2COMMON_11021, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCAE78)   -- now D2COMMON_11021
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_CheckItemFlag, BOOL, __stdcall, (D2UnitStrc *pItem, D2C_ItemFlags dwFlag, int nLine, const char *szFile), 0xCAE7E)   -- now D2COMMON_ITEMS_CheckItemFlag
//  D2FUNC(D2CLIENT, D2COMMON_PATH_GetClientCoordsVelocity, void, __stdcall, (D2UnitStrc *pUnit, int *pX, int *pY), 0xCAE84)   -- now D2COMMON_PATH_GetClientCoordsVelocity
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetSeqFrameCount, int, __stdcall, (D2UnitStrc *pUnit), 0xCAE8A)   -- now D2COMMON_DATATBLS_GetSeqFrameCount
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetMonSeqTxtRecordFromUnit, D2AnimSeqTxt *, __stdcall, (D2UnitStrc *pUnit), 0xCAE90)   -- now D2COMMON_DATATBLS_GetMonSeqTxtRecordFromUnit
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetLevelDefRecord, D2LevelDefBin *, __fastcall, (int nLevelId), 0xCAE96)   -- now D2COMMON_DATATBLS_GetLevelDefRecord
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetObjectsTxtRecord, D2ObjectsTxt *, __stdcall, (int nObjectId), 0xCAE9C)   -- now D2COMMON_DATATBLS_GetObjectsTxtRecord
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GetRoomExFromRoom, D2DrlgRoomStrc *, __fastcall, (D2ActiveRoomStrc *pRoom), 0xCAEA2)   -- now D2COMMON_DUNGEON_GetRoomExFromRoom
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetAutomapCellId, int, __fastcall, (uint32_t dwAutomapLevelType, uint32_t dwAutomapTileType, int nStyle, int nSequence), 0xCAEA8)   -- now D2COMMON_DATATBLS_GetAutomapCellId
//  D2FUNC(D2CLIENT, D2COMMON_DRLG_GetLevelTypeFromLevelId, int, __stdcall, (D2C_Levels nLevelId), 0xCAEAE)   -- now D2COMMON_DRLG_GetLevelTypeFromLevelId
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskOnUnit, BOOL, __stdcall, (D2UnitStrc *pUnit, int nStateMask), 0xCAEB4)   -- now D2COMMON_STATES_CheckStateMaskOnUnit
//  D2FUNC(D2CLIENT, D2COMMON_11013_ConvertMode, void, __stdcall, (D2UnitStrc *pUnit, int *pType, int *pClass, int *pMode, const char *szFile, int nLine), 0xCAEBA)   -- now D2COMMON_11013_ConvertMode
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_GetOwnerId, int, __stdcall, (D2InventoryStrc *pInventory), 0xCAEC0)   -- now D2COMMON_INVENTORY_GetOwnerId
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetLevelThreshold, uint32_t, __stdcall, (int nClass, uint32_t dwLevel), 0xCAEC6)   -- now D2COMMON_DATATBLS_GetLevelThreshold
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetMaxLevel, int, __stdcall, (int nClass), 0xCAECC)   -- now D2COMMON_DATATBLS_GetMaxLevel
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskArmRedOnUnit, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCAED2)   -- now D2COMMON_STATES_CheckStateMaskArmRedOnUnit
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_GetEquippedShield, BOOL, __stdcall, (D2InventoryStrc *pInventory, D2UnitStrc **ppItem), 0xCAED8)   -- now D2COMMON_INVENTORY_GetEquippedShield
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskArmBlueOnUnit, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCAEDE)   -- now D2COMMON_STATES_CheckStateMaskArmBlueOnUnit
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskRPRedOnUnit, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCAEE4)   -- now D2COMMON_STATES_CheckStateMaskRPRedOnUnit
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskRPBlueOnUnit, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCAEEA)   -- now D2COMMON_STATES_CheckStateMaskRPBlueOnUnit
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskRCRedOnUnit, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCAEF0)   -- now D2COMMON_STATES_CheckStateMaskRCRedOnUnit
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskRCBlueOnUnit, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCAEF6)   -- now D2COMMON_STATES_CheckStateMaskRCBlueOnUnit
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskRLRedOnUnit, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCAEFC)   -- now D2COMMON_STATES_CheckStateMaskRLRedOnUnit
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskRLBlueOnUnit, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCAF02)   -- now D2COMMON_STATES_CheckStateMaskRLBlueOnUnit
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskRFRedOnUnit, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCAF08)   -- now D2COMMON_STATES_CheckStateMaskRFRedOnUnit
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskRFBlueOnUnit, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCAF0E)   -- now D2COMMON_STATES_CheckStateMaskRFBlueOnUnit
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetDifficultyLevelsTxtRecord, D2DifficultyLevelsTxt *, __stdcall, (int nDifficulty), 0xCAF14)   -- now D2COMMON_DATATBLS_GetDifficultyLevelsTxtRecord
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetBlockRate, int, __stdcall, (D2UnitStrc *pUnit, BOOL bExpansion), 0xCAF1A)   -- now D2COMMON_UNITS_GetBlockRate
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_CalculateMonsterStatsByLevel, BOOL, __stdcall, (int nMonsterId, int nGameType, int nDifficulty, int nLevel, __int16 nFlags, D2MonStatsInitStrc *pMonStatsInit), 0xCAF20)   -- now D2COMMON_DATATBLS_CalculateMonsterStatsByLevel
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetDefense, int, __stdcall, (D2UnitStrc *pUnit), 0xCAF26)   -- now D2COMMON_UNITS_GetDefense
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_CopyStats, int, __stdcall, (D2UnitStrc *pUnit, D2C_ItemStats nStatId, D2StatStrc *pBuffer, int nBufferSize), 0xCAF2C)   -- now D2COMMON_STATLIST_CopyStats
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_CheckRequirements, BOOL, __stdcall, (D2UnitStrc *pItem, D2UnitStrc *pUnit, BOOL bEquipping, BOOL *bStrength, BOOL *bDexterity, BOOL *bLevel), 0xCAF32)   -- now D2COMMON_ITEMS_CheckRequirements
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_CheckWeaponIfTwoHanded, uint32_t, __stdcall, (D2UnitStrc *pItem), 0xCAF38)   -- now D2COMMON_ITEMS_CheckWeaponIfTwoHanded
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_GetCursorItem, D2UnitStrc *, __stdcall, (D2InventoryStrc *pInventory), 0xCAF3E)   -- now D2COMMON_INVENTORY_GetCursorItem
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetMagicAffixTxtRecord, D2MagicAffixTxt *, __stdcall, (int nIndex), 0xCAF44)   -- now D2COMMON_DATATBLS_GetMagicAffixTxtRecord
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetRareSuffixId, uint16_t, __stdcall, (D2UnitStrc *pItem), 0xCAF4A)   -- now D2COMMON_ITEMS_GetRareSuffixId
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetRareAffixTxtRecord, D2RareAffixTxt *, __stdcall, (int nId), 0xCAF50)   -- now D2COMMON_DATATBLS_GetRareAffixTxtRecord
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetRarePrefixId, uint16_t, __stdcall, (D2UnitStrc *pItem), 0xCAF56)   -- now D2COMMON_ITEMS_GetRarePrefixId
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetSuffixId, uint16_t, __stdcall, (D2UnitStrc *pItem, int nSuffixNo), 0xCAF5C)   -- now D2COMMON_ITEMS_GetSuffixId
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_GetItemCount, int, __stdcall, (D2InventoryStrc *pInventory), 0xCAF62)   -- now D2COMMON_INVENTORY_GetItemCount
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetEarName, char *, __stdcall, (D2UnitStrc *pItem), 0xCAF68)   -- now D2COMMON_ITEMS_GetEarName
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetEarLevel, uint8_t, __stdcall, (D2UnitStrc *pItem), 0xCAF6E)   -- now D2COMMON_ITEMS_GetEarLevel
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetLowQualityItemsTxtRecord, D2LowQualityItemsTxt *, __stdcall, (int nId), 0xCAF74)   -- now D2COMMON_DATATBLS_GetLowQualityItemsTxtRecord
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetFileIndex, int, __stdcall, (D2UnitStrc *pItem), 0xCAF7A)   -- now D2COMMON_ITEMS_GetFileIndex
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetItemQuality, D2C_ItemQualities, __stdcall, (D2UnitStrc *pItem), 0xCAF80)   -- now D2COMMON_ITEMS_GetItemQuality
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetPrefixId, uint16_t, __stdcall, (D2UnitStrc *pItem, int nPrefixNo), 0xCAF86)   -- now D2COMMON_ITEMS_GetPrefixId
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetItemsTxtRecord, D2ItemsTxt *, __stdcall, (int nItemId), 0xCAF8C)   -- now D2COMMON_DATATBLS_GetItemsTxtRecord
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetAllRepairCosts, int, __stdcall, (D2GameStrc *pGame, D2UnitStrc *pUnit, int nNpcId, D2C_Difficulties nDifficulty, D2BitBufferStrc *pQuestFlags, void (__fastcall *pfCallback)(void *, D2UnitStrc *, D2UnitStrc *)), 0xCAF92)   -- now D2COMMON_ITEMS_GetAllRepairCosts
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_CheckBodyLocation, BOOL, __stdcall, (D2UnitStrc *pItem, uint8_t nBodyLoc), 0xCAF98)   -- now D2COMMON_ITEMS_CheckBodyLocation
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetMaxSockets, uint8_t, __stdcall, (D2UnitStrc *pItem), 0xCAF9E)   -- now D2COMMON_ITEMS_GetMaxSockets
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_CheckIfSocketable, int, __stdcall, (D2UnitStrc *pItem), 0xCAFA4)   -- now D2COMMON_ITEMS_CheckIfSocketable
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetQuiverType, int32_t, __stdcall, (D2UnitStrc *pItem), 0xCAFAA)   -- now D2COMMON_ITEMS_GetQuiverType
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetDimensions, void, __stdcall, (D2UnitStrc *pItem, uint8_t *pWidth, uint8_t *pHeight, const char *szFile, int nLine), 0xCAFB0)   -- now D2COMMON_ITEMS_GetDimensions
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetInventoryRecordId, int, __stdcall, (D2UnitStrc *pUnit, D2C_ItemInvPage nInvPage, BOOL bLoD), 0xCAFB6)   -- now D2COMMON_UNITS_GetInventoryRecordId
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_GetWieldType, int, __stdcall, (D2UnitStrc *pPlayer, D2InventoryStrc *pInventory), 0xCAFBC)   -- now D2COMMON_INVENTORY_GetWieldType
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetStashGoldLimit, unsigned int, __stdcall, (D2UnitStrc *pUnit), 0xCAFC2)   -- now D2COMMON_UNITS_GetStashGoldLimit
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetInventoryComponentGrid, void, __stdcall, (int nInventoryTxtId, int bHigherRes, D2InvCompGridStrc *pInvCompGrid, int nComponent), 0xCAFC8)   -- now D2COMMON_DATATBLS_GetInventoryComponentGrid
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetInventoryGridInfo, void, __stdcall, (int nInventoryTxtId, int bHigherRes, D2InventoryGridInfoStrc *pInventoryGridInfo), 0xCAFCE)   -- now D2COMMON_DATATBLS_GetInventoryGridInfo
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetInventoryRect, void, __stdcall, (int nInventoryTxtId, int bHigherRes, D2InvRectStrc *pInvRect), 0xCAFD4)   -- now D2COMMON_DATATBLS_GetInventoryRect
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetSockets, int, __stdcall, (D2UnitStrc *pItem), 0xCAFDA)   -- now D2COMMON_ITEMS_GetSockets
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_CanItemsBeExchanged, BOOL, __stdcall, (D2InventoryStrc *pInventory, D2UnitStrc *pItem, int nXPos, int nYPos, D2C_InventoryRecords nInventoryRecordId, D2UnitStrc **ppExchangeItem, uint8_t nPage, BOOL bCheckIfCube), 0xCAFE0)   -- now D2COMMON_INVENTORY_CanItemsBeExchanged
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_CanItemBePlaced, BOOL, __stdcall, (D2InventoryStrc *pInventory, D2UnitStrc *pItem, int nXPos, int nYPos, D2C_InventoryRecords nInventoryRecordId, D2UnitStrc **ppExchangeItem, unsigned int *pHoveredItems, uint8_t nPage), 0xCAFE6)   -- now D2COMMON_INVENTORY_CanItemBePlaced
//  D2FUNC(D2CLIENT, D2COMMON_QUESTRECORD_GetQuestState, BOOL, __stdcall, (D2BitBufferStrc *pQuestRecord, int nQuest, int nState), 0xCAFEC)   -- now D2COMMON_QUESTRECORD_GetQuestState
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetTransmogrify, uint32_t, __stdcall, (D2UnitStrc *pItem), 0xCAFF2)   -- now D2COMMON_ITEMS_GetTransmogrify
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetInvPage, uint8_t, __stdcall, (D2UnitStrc *pItem), 0xCAFF8)   -- now D2COMMON_ITEMS_GetInvPage
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_GetItemNodePage, int, __stdcall, (D2UnitStrc *pItem), 0xCAFFE)   -- now D2COMMON_INVENTORY_GetItemNodePage
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_IsSocketFiller, BOOL, __stdcall, (D2UnitStrc *pItem), 0xCB004)   -- now D2COMMON_ITEMS_IsSocketFiller
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetItemRecordFromItemCode, D2ItemsTxt *, __stdcall, (uint32_t dwCode, int *pItemId), 0xCB00A)   -- now D2COMMON_DATATBLS_GetItemRecordFromItemCode
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetTotalMaxStack, int, __stdcall, (D2UnitStrc *pItem), 0xCB010)   -- now D2COMMON_ITEMS_GetTotalMaxStack
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetGemsTxtRecord, D2GemsTxt *, __stdcall, (int nGemId), 0xCB016)   -- now D2COMMON_DATATBLS_GetGemsTxtRecord
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_GetDefenseFromUnit, int, __stdcall, (D2UnitStrc *pUnit), 0xCB01C)   -- now D2COMMON_STATLIST_GetDefenseFromUnit
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_IsClassValid, BOOL, __stdcall, (D2UnitStrc *pItem), 0xCB022)   -- now D2COMMON_ITEMS_IsClassValid
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_GetMaxDamageFromUnit, int, __stdcall, (D2UnitStrc *pUnit, BOOL b2Handed), 0xCB028)   -- now D2COMMON_STATLIST_GetMaxDamageFromUnit
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_GetMinDamageFromUnit, int, __stdcall, (D2UnitStrc *pUnit, BOOL b2Handed), 0xCB02E)   -- now D2COMMON_STATLIST_GetMinDamageFromUnit
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetClassOfClassSpecificItem, int, __stdcall, (D2UnitStrc *pItem), 0xCB034)   -- now D2COMMON_ITEMS_GetClassOfClassSpecificItem
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetLevelRequirement, int, __stdcall, (D2UnitStrc *pItem, D2UnitStrc *pUnit), 0xCB03A)   -- now D2COMMON_ITEMS_GetLevelRequirement
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_CompareWithItemsParentInventory, BOOL, __stdcall, (D2InventoryStrc *pInventory, D2UnitStrc *pItem), 0xCB040)   -- now D2COMMON_INVENTORY_CompareWithItemsParentInventory
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_GetUnitStatBonus, int, __stdcall, (D2UnitStrc *pUnit, D2C_ItemStats nStatId, uint16_t nLayer), 0xCB046)   -- now D2COMMON_STATLIST_GetUnitStatBonus
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_GetMaxDurabilityFromUnit, int, __stdcall, (D2UnitStrc *pUnit), 0xCB04C)   -- now D2COMMON_STATLIST_GetMaxDurabilityFromUnit
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_HasDurability, BOOL, __stdcall, (D2UnitStrc *pItem), 0xCB052)   -- now D2COMMON_ITEMS_HasDurability
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_Is1Or2HandedForBarbarian, int, __stdcall, (D2UnitStrc *pPlayer, D2UnitStrc *pItem), 0xCB058)   -- now D2COMMON_ITEMS_Is1Or2HandedForBarbarian
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_GetMaxDamage, int, __stdcall, (D2UnitStrc *pMissile, D2UnitStrc *pOwner, int nMissileId, int nLevel), 0xCB05E)   -- now D2COMMON_MISSILE_GetMaxDamage
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_GetMinDamage, int, __stdcall, (D2UnitStrc *pMissile, D2UnitStrc *pOwner, int nMissileId, int nLevel), 0xCB064)   -- now D2COMMON_MISSILE_GetMinDamage
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_GetElementalLength, int, __stdcall, (int nUnused, D2UnitStrc *pMissile, int nMissileId, int nLevel), 0xCB06A)   -- now D2COMMON_MISSILE_GetElementalLength
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_GetElemTypeFromMissileId, uint8_t, __stdcall, (int nMissileId), 0xCB070)   -- now D2COMMON_MISSILE_GetElemTypeFromMissileId
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_GetMaxElemDamage, int, __stdcall, (D2UnitStrc *pMissile, D2UnitStrc *pOwner, int nMissileId, int nLevel), 0xCB076)   -- now D2COMMON_MISSILE_GetMaxElemDamage
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_GetMinElemDamage, int, __stdcall, (D2UnitStrc *pMissile, D2UnitStrc *pOwner, int nMissileId, int nLevel), 0xCB07C)   -- now D2COMMON_MISSILE_GetMinElemDamage
//  D2FUNC(D2CLIENT, D2COMMON_ITEMMODS_GetByTimeAdjustment, int, __stdcall, (int nAmount, int nPeriodOfDay, int nBaseTime, int *pItemModPeriodOfDay, int *pItemModMin, int *pItemModMax), 0xCB082)   -- now D2COMMON_ITEMMODS_GetByTimeAdjustment
//  D2FUNC(D2CLIENT, D2COMMON_11036_GetMonCurseResistanceSubtraction, int, __stdcall, (int nLevel, int nSkillId), 0xCB088)   -- now D2COMMON_11036_GetMonCurseResistanceSubtraction
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetMaxPhysDamage, int, __stdcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int nSkillLevel, BOOL a4), 0xCB08E)   -- now D2COMMON_SKILLS_GetMaxPhysDamage
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetMinPhysDamage, int, __stdcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int nSkillLevel, BOOL a4), 0xCB094)   -- now D2COMMON_SKILLS_GetMinPhysDamage
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetWeaponAttackSpeed, int, __stdcall, (D2UnitStrc *pUnit, D2UnitStrc *pWeapon), 0xCB09A)   -- now D2COMMON_ITEMS_GetWeaponAttackSpeed
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetBonusManaBasedOnClass, int, __stdcall, (D2UnitStrc *pPlayer, int nValue), 0xCB0A0)   -- now D2COMMON_ITEMS_GetBonusManaBasedOnClass
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetBonusLifeBasedOnClass, int, __stdcall, (D2UnitStrc *pPlayer, int nValue), 0xCB0A6)   -- now D2COMMON_ITEMS_GetBonusLifeBasedOnClass
//  D2FUNC(D2CLIENT, D2COMMON_ITEMMODS_EvaluateItemFormula, int, __stdcall, (D2UnitStrc *pUnit, D2UnitStrc *pItem, unsigned int nCalc), 0xCB0AC)   -- now D2COMMON_ITEMMODS_EvaluateItemFormula
//  D2FUNC(D2CLIENT, D2COMMON_10299, int, __stdcall, (D2UnitStrc *pUnit, D2C_PlayerBodyLocs nBodyLoc, D2UnitStrc *pItem, BOOL bDontCheckReqs), 0xCB0B2)   -- now D2COMMON_10299
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_GetFreePosition, BOOL, __stdcall, (D2InventoryStrc *pInventory, D2UnitStrc *pItem, D2C_InventoryRecords nInventoryRecordId, int *pFreeX, int *pFreeY, uint8_t nPage), 0xCB0B8)   -- now D2COMMON_INVENTORY_GetFreePosition
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_AreStackablesEqual, BOOL, __stdcall, (D2UnitStrc *pItem1, D2UnitStrc *pItem2), 0xCB0BE)   -- now D2COMMON_ITEMS_AreStackablesEqual
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_CheckIfBeltable, uint32_t, __stdcall, (D2UnitStrc *pItem), 0xCB0C4)   -- now D2COMMON_ITEMS_CheckIfBeltable
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetTransactionCost, int, __stdcall, (D2UnitStrc *pPlayer, D2UnitStrc *pItem, D2C_Difficulties nDifficulty, D2BitBufferStrc *pQuestFlags, int nVendorId, D2C_TransactionTypes nTransactionType), 0xCB0CA)   -- now D2COMMON_ITEMS_GetTransactionCost
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_GetItemFromInventoryPage, D2UnitStrc *, __stdcall, (D2InventoryStrc *pInventory, int nGridX, int nGridY, int *pX, int *pY, D2C_InventoryRecords nInventoryRecordId, uint8_t nPage), 0xCB0D0)   -- now D2COMMON_INVENTORY_GetItemFromInventoryPage
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetUniqueColumnFromItemsTxt, int, __stdcall, (D2UnitStrc *pItem), 0xCB0D6)   -- now D2COMMON_ITEMS_GetUniqueColumnFromItemsTxt
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetBaseCode, uint32_t, __stdcall, (D2UnitStrc *pItem), 0xCB0DC)   -- now D2COMMON_ITEMS_GetBaseCode
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_CheckIfUseable, BOOL, __stdcall, (D2UnitStrc *pItem), 0xCB0E2)   -- now D2COMMON_ITEMS_CheckIfUseable
//  D2FUNC(D2CLIENT, D2COMMON_QUESTRECORD_SetQuestState, void, __stdcall, (D2BitBufferStrc *pQuestRecord, int nQuest, int nState), 0xCB0E8)   -- now D2COMMON_QUESTRECORD_SetQuestState
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetWeaponMasteryBonus, int, __stdcall, (D2UnitStrc *pUnit, D2UnitStrc *pItem, D2SkillStrc *pSkill, int nType), 0xCB0EE)   -- now D2COMMON_SKILLS_GetWeaponMasteryBonus
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetDexBonus, __int16, __stdcall, (D2UnitStrc *pItem), 0xCB0F4)   -- now D2COMMON_ITEMS_GetDexBonus
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetStrengthBonus, __int16, __stdcall, (D2UnitStrc *pItem), 0xCB0FA)   -- now D2COMMON_ITEMS_GetStrengthBonus
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_IsPlayerClassSkill, BOOL, __stdcall, (D2C_Skills nSkillId, int *pPlayerClass), 0xCB100)   -- now D2COMMON_SKILLS_IsPlayerClassSkill
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_FreeStatList, void, __stdcall, (D2StatListStrc *pStatList), 0xCB106)   -- now D2COMMON_STATLIST_FreeStatList
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_ExpireStatList, void, __stdcall, (D2UnitStrc *pUnused, D2StatListStrc *pStatList), 0xCB10C)   -- now D2COMMON_STATLIST_ExpireStatList
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_GetStatListFromUnitStateOrFlag, D2StatListStrc *, __stdcall, (D2UnitStrc *pUnit, D2C_States nState, int nFlag), 0xCB112)   -- now D2COMMON_STATLIST_GetStatListFromUnitStateOrFlag
//  D2FUNC(D2CLIENT, D2COMMON_ITEMMODS_AssignProperty, void, __stdcall, (int nType, D2UnitStrc *a2, D2UnitStrc *pItem, const void *pMods, int nPropSet, int nApplyType), 0xCB118)   -- now D2COMMON_ITEMMODS_AssignProperty
//  D2FUNC(D2CLIENT, D2COMMON_11269_CopyStats, int, __stdcall, (D2StatListExStrc *pStatListEx, int nStatId, D2StatStrc *pBuffer, int nBufferSize), 0xCB11E)   -- now D2COMMON_11269_CopyStats
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_MergeBaseStats, void, __stdcall, (D2StatListStrc *pTargetStatList, D2StatListStrc *pSourceStatlist), 0xCB124)   -- now D2COMMON_STATLIST_MergeBaseStats
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetNoOfSetItemsFromItem, int, __stdcall, (D2UnitStrc *pItem), 0xCB12A)   -- now D2COMMON_ITEMS_GetNoOfSetItemsFromItem
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetSetItemsMask, uint32_t, __stdcall, (D2UnitStrc *pPlayer, D2UnitStrc *pSetItem, BOOL bDontIgnoreInputItem), 0xCB130)   -- now D2COMMON_ITEMS_GetSetItemsMask
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetSetItemsTxtRecordFromItem, D2SetItemsTxt *, __stdcall, (D2UnitStrc *pItem), 0xCB136)   -- now D2COMMON_ITEMS_GetSetItemsTxtRecordFromItem
//  D2FUNC(D2CLIENT, D2COMMON_10251_Return, void, __stdcall, (int a1), 0xCB13C)   -- now D2COMMON_10251_Return
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_RemoveInventoryItems, void, __stdcall, (D2InventoryStrc *pInventory), 0xCB142)   -- now D2COMMON_INVENTORY_RemoveInventoryItems
//  D2FUNC(D2CLIENT, D2COMMON_TEXT_GetStringIdOfMenuType1Or2Node, __int16, __stdcall, (D2TextHeaderStrc *pTextHeader, int nNodeId), 0xCB148)   -- now D2COMMON_TEXT_GetStringIdOfMenuType1Or2Node
//  D2FUNC(D2CLIENT, D2COMMON_TEXT_GetStringIdOfMenuType2Node, __int16, __stdcall, (D2TextHeaderStrc *pTextHeader, int nNodeId), 0xCB14E)   -- now D2COMMON_TEXT_GetStringIdOfMenuType2Node
//  D2FUNC(D2CLIENT, D2COMMON_TEXT_GetMenuType2NodeCount, int, __stdcall, (D2TextHeaderStrc *pTextHeader), 0xCB154)   -- now D2COMMON_TEXT_GetMenuType2NodeCount
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetClientCoords, void, __stdcall, (D2UnitStrc *pUnit, D2CoordStrc *pClientCoords), 0xCB15A)   -- now D2COMMON_UNITS_GetClientCoords
//  D2FUNC(D2CLIENT, D2COMMON_MONSTERS_GetHirelingDescString, const Unicode *, __fastcall, (int nId), 0xCB160)   -- now D2COMMON_MONSTERS_GetHirelingDescString
//  D2FUNC(D2CLIENT, D2COMMON_MONSTERS_HirelingInit, int, __fastcall, (BOOL bExpansion, D2UnitStrc *pMonster, int nLowSeed, D2C_Acts nAct, D2C_Difficulties nDifficulty, D2HirelingInitStrc *pHirelingInit), 0xCB166)   -- now D2COMMON_MONSTERS_HirelingInit
//  D2FUNC(D2CLIENT, D2COMMON_MONSTERS_GetActFromHirelingTxt, int, __fastcall, (BOOL bExpansion, int nClassId, uint16_t nNameId), 0xCB16C)   -- now D2COMMON_MONSTERS_GetActFromHirelingTxt
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetNoOfUnidItems, int, __stdcall, (D2UnitStrc *pUnit), 0xCB172)   -- now D2COMMON_ITEMS_GetNoOfUnidItems
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetHealingCost, unsigned int, __stdcall, (D2UnitStrc *pUnit), 0xCB178)   -- now D2COMMON_UNITS_GetHealingCost
//  D2FUNC(D2CLIENT, D2COMMON_QUESTRECORD_AllocRecord, D2BitBufferStrc *, __stdcall, (void *pMemPool), 0xCB17E)   -- now D2COMMON_QUESTRECORD_AllocRecord
//  D2FUNC(D2CLIENT, D2COMMON_QUESTRECORD_FreeRecord, void, __stdcall, (void *pMemPool, D2BitBufferStrc *pQuestRecord), 0xCB184)   -- now D2COMMON_QUESTRECORD_FreeRecord
//  D2FUNC(D2CLIENT, D2COMMON_QUESTRECORD_CopyBufferToRecord, void, __stdcall, (D2BitBufferStrc *pQuestRecord, uint8_t *pBuffer, uint16_t nBufferSize, BOOL bResetStates), 0xCB18A)   -- now D2COMMON_QUESTRECORD_CopyBufferToRecord
//  D2FUNC(D2CLIENT, D2COMMON_10153, void, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCB190)   -- now D2COMMON_10153
//  D2FUNC(D2CLIENT, D2COMMON_TEXT_GetStringIdOfMenuType0Or2Node, __int16, __stdcall, (D2TextHeaderStrc *pTextHeader, int nNodeId), 0xCB196)   -- now D2COMMON_TEXT_GetStringIdOfMenuType0Or2Node
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_IsNotQuestItem, BOOL, __stdcall, (D2UnitStrc *pItem), 0xCB19C)   -- now D2COMMON_ITEMS_IsNotQuestItem
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_HasUsedCharges, int, __stdcall, (D2UnitStrc *pItem, BOOL *pHasChargedSkills), 0xCB1A2)   -- now D2COMMON_ITEMS_HasUsedCharges
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_IsRepairable, BOOL, __stdcall, (D2UnitStrc *pItem), 0xCB1A8)   -- now D2COMMON_ITEMS_IsRepairable
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetQuantity, int, __stdcall, (D2SkillStrc *pSkill), 0xCB1AE)   -- now D2COMMON_SKILLS_GetQuantity
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetRightSkill, D2SkillStrc *, __stdcall, (D2UnitStrc *pUnit), 0xCB1B4)   -- now D2COMMON_UNITS_GetRightSkill
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetLeftSkill, D2SkillStrc *, __stdcall, (D2UnitStrc *pUnit), 0xCB1BA)   -- now D2COMMON_UNITS_GetLeftSkill
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_IsMagSetRarUniCrfOrTmp, int, __stdcall, (D2UnitStrc *pItem), 0xCB1C0)   -- now D2COMMON_ITEMS_IsMagSetRarUniCrfOrTmp
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetQtyWarnCount, uint8_t, __stdcall, (D2UnitStrc *pItem), 0xCB1C6)   -- now D2COMMON_ITEMS_GetQtyWarnCount
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_GetMaxManaFromUnit, int, __stdcall, (D2UnitStrc *pUnit), 0xCB1CC)   -- now D2COMMON_STATLIST_GetMaxManaFromUnit
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_GetMaxStaminaFromUnit, int, __stdcall, (D2UnitStrc *pUnit), 0xCB1D2)   -- now D2COMMON_STATLIST_GetMaxStaminaFromUnit
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_GetUseableItemFromBeltSlot, BOOL, __stdcall, (D2InventoryStrc *pInventory, D2UnitStrc *pItem, int nSlotId, D2UnitStrc **ppItem), 0xCB1D8)   -- now D2COMMON_INVENTORY_GetUseableItemFromBeltSlot
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetInvRectFromBeltsTxt, void, __stdcall, (int nIndex, int bHigherRes, D2InvRectStrc *pInvRect, int nBoxId), 0xCB1DE)   -- now D2COMMON_DATATBLS_GetInvRectFromBeltsTxt
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_GetItemFromBeltSlot, D2UnitStrc *, __stdcall, (D2InventoryStrc *pInventory, int nSlotId), 0xCB1E4)   -- now D2COMMON_INVENTORY_GetItemFromBeltSlot
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetBeltsTxtRecord, void, __stdcall, (int nIndex, int bHigherRes, D2BeltsTxt *pRecord), 0xCB1EA)   -- now D2COMMON_DATATBLS_GetBeltsTxtRecord
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetBeltType, int, __stdcall, (D2UnitStrc *pUnit), 0xCB1F0)   -- now D2COMMON_UNITS_GetBeltType
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_GetLeftHandWeapon, D2UnitStrc *, __stdcall, (D2InventoryStrc *pInventory), 0xCB1F6)   -- now D2COMMON_INVENTORY_GetLeftHandWeapon
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskAttRedOnUnit, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCB1FC)   -- now D2COMMON_STATES_CheckStateMaskAttRedOnUnit
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskAttBlueOnUnit, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCB202)   -- now D2COMMON_STATES_CheckStateMaskAttBlueOnUnit
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetToHitFactor, int, __stdcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int nSkillLevel), 0xCB208)   -- now D2COMMON_SKILLS_GetToHitFactor
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetAttackRate, int, __stdcall, (D2UnitStrc *pAttacker), 0xCB20E)   -- now D2COMMON_UNITS_GetAttackRate
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetBodyLocation, uint8_t, __stdcall, (D2UnitStrc *pItem), 0xCB214)   -- now D2COMMON_ITEMS_GetBodyLocation
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetMaxElemDamage, int, __stdcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int nSkillLevel, BOOL a4), 0xCB21A)   -- now D2COMMON_SKILLS_GetMaxElemDamage
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetMinElemDamage, int, __stdcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int nSkillLevel, BOOL a4), 0xCB220)   -- now D2COMMON_SKILLS_GetMinElemDamage
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskDamRedOnUnit, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCB226)   -- now D2COMMON_STATES_CheckStateMaskDamRedOnUnit
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskDamBlueOnUnit, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCB22C)   -- now D2COMMON_STATES_CheckStateMaskDamBlueOnUnit
//  D2FUNC(D2CLIENT, D2COMMON_11248, int, __stdcall, (D2UnitStrc *pUnused, D2UnitStrc *pUnit, int nStatId), 0xCB232)   -- now D2COMMON_11248
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_EvaluateSkillDescFormula, int, __stdcall, (D2UnitStrc *pUnit, unsigned int nCalc, D2C_Skills nSkillId, int nSkillLevel), 0xCB238)   -- now D2COMMON_SKILLS_EvaluateSkillDescFormula
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetElementalLength, int, __stdcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId, int nSkillLevel, BOOL bUnused), 0xCB23E)   -- now D2COMMON_SKILLS_GetElementalLength
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetConcentrationDamageBonus, int, __stdcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId), 0xCB244)   -- now D2COMMON_SKILLS_GetConcentrationDamageBonus
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_CalculateKickDamage, void, __stdcall, (D2UnitStrc *pUnit, int *pMinDamage, int *pMaxDamage, int *pDamagePercent), 0xCB24A)   -- now D2COMMON_SKILLS_CalculateKickDamage
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_CheckRequiredAttributes, BOOL, __stdcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId), 0xCB250)   -- now D2COMMON_SKILLS_CheckRequiredAttributes
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_CheckRequiredSkills, BOOL, __stdcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId), 0xCB256)   -- now D2COMMON_SKILLS_CheckRequiredSkills
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetClassSkillId, int, __stdcall, (int nClassId, int nPosition), 0xCB25C)   -- now D2COMMON_SKILLS_GetClassSkillId
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetPlayerSkillCount, int, __stdcall, (int nClassId), 0xCB262)   -- now D2COMMON_SKILLS_GetPlayerSkillCount
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetBonusSkillLevelFromSkillId, int, __stdcall, (D2UnitStrc *pUnit, D2C_Skills nSkillId), 0xCB268)   -- now D2COMMON_SKILLS_GetBonusSkillLevelFromSkillId
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetClassIdFromSkillId, int, __stdcall, (D2C_Skills nSkillId), 0xCB26E)   -- now D2COMMON_SKILLS_GetClassIdFromSkillId
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetStartSkill, D2SkillStrc *, __stdcall, (D2UnitStrc *pUnit), 0xCB274)   -- now D2COMMON_UNITS_GetStartSkill
//  D2FUNC(D2CLIENT, D2COMMON_TEXT_FreeTextHeader, void, __stdcall, (D2TextHeaderStrc *pTextHeader), 0xCB27A)   -- now D2COMMON_TEXT_FreeTextHeader
//  D2FUNC(D2CLIENT, D2COMMON_CHAT_GetLangIdFromHoverMsg, uint8_t, __stdcall, (D2HoverTextStrc *pHoverMsg), 0xCB280)   -- now D2COMMON_CHAT_GetLangIdFromHoverMsg
//  D2FUNC(D2CLIENT, D2COMMON_CHAT_CopyHoverMsgToBuffer, void, __stdcall, (D2HoverTextStrc *pHoverMsg, char *szMessage), 0xCB286)   -- now D2COMMON_CHAT_CopyHoverMsgToBuffer
//  D2FUNC(D2CLIENT, D2COMMON_CHAT_FreeHoverMsg, void, __stdcall, (void *pMemPool, D2HoverTextStrc *pHoverMsg), 0xCB28C)   -- now D2COMMON_CHAT_FreeHoverMsg
//  D2FUNC(D2CLIENT, D2COMMON_CHAT_GetTimeoutFromHoverMsg, uint32_t, __stdcall, (D2HoverTextStrc *pHoverMsg), 0xCB292)   -- now D2COMMON_CHAT_GetTimeoutFromHoverMsg
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetSuperUniquesTxtRecord, D2SuperUniquesTxt *, __stdcall, (int nSuperUniqueId), 0xCB298)   -- now D2COMMON_DATATBLS_GetSuperUniquesTxtRecord
//  D2FUNC(D2CLIENT, D2COMMON_TEXT_SortTextNodeListByStringId, void, __stdcall, (D2TextHeaderStrc *pTextHeader), 0xCB29E)   -- now D2COMMON_TEXT_SortTextNodeListByStringId
//  D2FUNC(D2CLIENT, D2COMMON_TEXT_CreateTextHeaderFromMessageList, void, __stdcall, (D2TextHeaderStrc *pTextHeader, D2MessageListStrc *pMsgList), 0xCB2A4)   -- now D2COMMON_TEXT_CreateTextHeaderFromMessageList
//  D2FUNC(D2CLIENT, D2COMMON_TEXT_AllocTextHeader, D2TextHeaderStrc *, __stdcall, (void *pMemPool), 0xCB2AA)   -- now D2COMMON_TEXT_AllocTextHeader
//  D2FUNC(D2CLIENT, D2COMMON_CHAT_AllocHoverMsg, D2HoverTextStrc *, __stdcall, (void *pMemPool, const char *szText, int nTimeout), 0xCB2B0)   -- now D2COMMON_CHAT_AllocHoverMsg
//  D2FUNC(D2CLIENT, D2COMMON_CHAT_SetUsedInHoverMsg, void, __stdcall, (D2HoverTextStrc *pHoverMsg, BOOL bUsed), 0xCB2B6)   -- now D2COMMON_CHAT_SetUsedInHoverMsg
//  D2FUNC(D2CLIENT, D2COMMON_CHAT_GetUsedFromHoverMsg, BOOL, __stdcall, (D2HoverTextStrc *pHoverMsg), 0xCB2BC)   -- now D2COMMON_CHAT_GetUsedFromHoverMsg
//  D2FUNC(D2CLIENT, D2COMMON_CHAT_SetLangIdInHoverMsg, void, __stdcall, (D2HoverTextStrc *pHoverMsg, uint8_t nLangId), 0xCB2C2)   -- now D2COMMON_CHAT_SetLangIdInHoverMsg
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_IsTownLevelId, BOOL, __stdcall, (D2C_Levels nLevelId), 0xCB2C8)   -- now D2COMMON_DUNGEON_IsTownLevelId
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GetWarpDestinationLevel, int, __stdcall, (D2ActiveRoomStrc *pRoom, int nSourceLevel), 0xCB2CE)   -- now D2COMMON_DUNGEON_GetWarpDestinationLevel
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetNameOffsetFromObject, uint32_t, __stdcall, (D2UnitStrc *pUnit), 0xCB2D4)   -- now D2COMMON_UNITS_GetNameOffsetFromObject
//  D2FUNC(D2CLIENT, D2COMMON_MONSTERS_SetMonsterNameInMonsterData, void, __fastcall, (D2UnitStrc *pMonster, const Unicode *wszName), 0xCB2DA)   -- now D2COMMON_MONSTERS_SetMonsterNameInMonsterData
//  D2FUNC(D2CLIENT, D2COMMON_MONSTERS_IsDemon, BOOL, __stdcall, (D2UnitStrc *pMonster), 0xCB2E0)   -- now D2COMMON_MONSTERS_IsDemon
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetDistanceToOtherUnit, int, __stdcall, (D2UnitStrc *pUnit1, D2UnitStrc *pUnit2), 0xCB2E6)   -- now D2COMMON_UNITS_GetDistanceToOtherUnit
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_IsPersonalizable, BOOL, __stdcall, (D2UnitStrc *pItem), 0xCB2EC)   -- now D2COMMON_ITEMS_IsPersonalizable
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_IsSocketable, BOOL, __stdcall, (D2UnitStrc *pItem), 0xCB2F2)   -- now D2COMMON_ITEMS_IsSocketable
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_IsImbueable, BOOL, __stdcall, (D2UnitStrc *pItem), 0xCB2F8)   -- now D2COMMON_ITEMS_IsImbueable
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_SetCursorItem, void, __stdcall, (D2InventoryStrc *pInventory, D2UnitStrc *pItem), 0xCB2FE)   -- now D2COMMON_INVENTORY_SetCursorItem
//  D2FUNC(D2CLIENT, D2COMMON_WAYPOINTS_AllocWaypointData, D2WaypointDataStrc *, __stdcall, (void *pMemPool), 0xCB304)   -- now D2COMMON_WAYPOINTS_AllocWaypointData
//  D2FUNC(D2CLIENT, D2COMMON_WAYPOINTS_FreeWaypointData, void, __stdcall, (void *pMemPool, D2WaypointDataStrc *pData), 0xCB30A)   -- now D2COMMON_WAYPOINTS_FreeWaypointData
//  D2FUNC(D2CLIENT, D2COMMON_WAYPOINTS_CopyAndValidateWaypointData, void, __stdcall, (D2WaypointDataStrc *pDestination, D2WaypointDataStrc *pSource), 0xCB310)   -- now D2COMMON_WAYPOINTS_CopyAndValidateWaypointData
//  D2FUNC(D2CLIENT, D2COMMON_WAYPOINTS_GetLevelIdFromWaypointNo, BOOL, __stdcall, (__int16 nWaypointNo, int *pLevelId), 0xCB316)   -- now D2COMMON_WAYPOINTS_GetLevelIdFromWaypointNo
//  D2FUNC(D2CLIENT, D2COMMON_WAYPOINTS_IsActivated, int, __stdcall, (D2WaypointDataStrc *pData, uint16_t wField), 0xCB31C)   -- now D2COMMON_WAYPOINTS_IsActivated
//  D2FUNC(D2CLIENT, D2COMMON_WAYPOINTS_GetWaypointNoFromLevelId, BOOL, __stdcall, (D2C_Levels nLevelId, __int16 *pWaypointNo), 0xCB322)   -- now D2COMMON_WAYPOINTS_GetWaypointNoFromLevelId
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_FindRoomBySubtileCoordinates, D2ActiveRoomStrc *, __stdcall, (D2DrlgActStrc *pAct, int nX, int nY), 0xCB328)   -- now D2COMMON_DUNGEON_FindRoomBySubtileCoordinates
//  D2FUNC(D2CLIENT, D2COMMON_10229, BOOL, __stdcall, (D2DynamicPathStrc *pDynamicPath, D2UnitStrc *pUnit, D2ActiveRoomStrc *pDestRoom, uint32_t nDestX, uint32_t nDestY), 0xCB32E)   -- now D2COMMON_10229
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetNoPerFromLevelsTxt, uint8_t, __stdcall, (int nLevelId), 0xCB334)   -- now D2COMMON_DATATBLS_GetNoPerFromLevelsTxt
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_UpdatePops, void, __stdcall, (D2ActiveRoomStrc *pRoom, int nX, int nY, BOOL bOtherRoom), 0xCB33A)   -- now D2COMMON_DUNGEON_UpdatePops
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_AllocUnit, D2UnitStrc *, __stdcall, (void *pMemPool, D2C_UnitTypes nUnitType), 0xCB340)   -- now D2COMMON_UNITS_AllocUnit
//  D2FUNC(D2CLIENT, D2COMMON_UNITROOM_AddUnitToRoom, int, __stdcall, (D2UnitStrc *pUnit, D2ActiveRoomStrc *pRoom), 0xCB346)   -- now D2COMMON_UNITROOM_AddUnitToRoom
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_BlockCollisionPath, void, __stdcall, (D2UnitStrc *pUnit, D2ActiveRoomStrc *pRoom, int nX, int nY), 0xCB34C)   -- now D2COMMON_UNITS_BlockCollisionPath
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_InitializeStaticPath, void, __stdcall, (D2UnitStrc *pUnit, D2ActiveRoomStrc *pRoom, int nX, int nY), 0xCB352)   -- now D2COMMON_UNITS_InitializeStaticPath
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_AllocStaticPath, void, __stdcall, (D2UnitStrc *pUnit), 0xCB358)   -- now D2COMMON_UNITS_AllocStaticPath
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_InitItemSeed, void, __stdcall, (D2UnitStrc *pItem), 0xCB35E)   -- now D2COMMON_ITEMS_InitItemSeed
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_AllocItemData, void, __stdcall, (void *pMemPool, D2UnitStrc *pItem), 0xCB364)   -- now D2COMMON_ITEMS_AllocItemData
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_SetNameInPlayerData, void, __stdcall, (D2UnitStrc *pUnit, char *szName), 0xCB36A)   -- now D2COMMON_UNITS_SetNameInPlayerData
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_IsShrine, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCB370)   -- now D2COMMON_UNITS_IsShrine
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_AllocInventory, D2InventoryStrc *, __stdcall, (void *pMemPool, D2UnitStrc *pOwner), 0xCB376)   -- now D2COMMON_INVENTORY_AllocInventory
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_DecreaseAlliedCountOfRoom, void, __stdcall, (D2ActiveRoomStrc *pRoom), 0xCB37C)   -- now D2COMMON_DUNGEON_DecreaseAlliedCountOfRoom
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_IncreaseAlliedCountOfRoom, void, __stdcall, (D2ActiveRoomStrc *pRoom), 0xCB382)   -- now D2COMMON_DUNGEON_IncreaseAlliedCountOfRoom
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GetPresetUnitsFromRoom, D2PresetUnitStrc *, __stdcall, (D2ActiveRoomStrc *pRoom), 0xCB388)   -- now D2COMMON_DUNGEON_GetPresetUnitsFromRoom
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_FreeUnit, void, __stdcall, (D2UnitStrc *pUnit), 0xCB38E)   -- now D2COMMON_UNITS_FreeUnit
//  D2FUNC(D2CLIENT, D2COMMON_UNITROOM_RemoveUnitFromRoom, void, __stdcall, (D2UnitStrc *pUnit), 0xCB394)   -- now D2COMMON_UNITROOM_RemoveUnitFromRoom
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_FreeStaticPath, void, __stdcall, (D2UnitStrc *pUnit), 0xCB39A)   -- now D2COMMON_UNITS_FreeStaticPath
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_FreeItemData, void, __stdcall, (void *pMemPool, D2UnitStrc *pItem), 0xCB3A0)   -- now D2COMMON_ITEMS_FreeItemData
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_FreeModStats, void, __stdcall, (D2UnitStrc *pUnit), 0xCB3A6)   -- now D2COMMON_STATLIST_FreeModStats
//  D2FUNC(D2CLIENT, D2COMMON_11020, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCB3AC)   -- now D2COMMON_11020
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetAnimOrSeqMode, int, __stdcall, (D2UnitStrc *pUnit), 0xCB3B2)   -- now D2COMMON_UNITS_GetAnimOrSeqMode
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_RemoveItemFromInventory, D2UnitStrc *, __stdcall, (D2InventoryStrc *pInventory, D2UnitStrc *pItem), 0xCB3B8)   -- now D2COMMON_INVENTORY_RemoveItemFromInventory
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_GetItemGUID, D2UnitGUID, __stdcall, (D2UnitStrc *pItem), 0xCB3BE)   -- now D2COMMON_INVENTORY_GetItemGUID
//  D2FUNC(D2CLIENT, D2COMMON_10208_PathSetPathingFlag, void, __stdcall, (D2DynamicPathStrc *pDynamicPath, BOOL bSet), 0xCB3C4)   -- now D2COMMON_10208_PathSetPathingFlag
//  D2FUNC(D2CLIENT, D2COMMON_10209_PathCheckPathingFlag, BOOL, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCB3CA)   -- now D2COMMON_10209_PathCheckPathingFlag
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_ClampStaminaManaHP, void, __stdcall, (D2UnitStrc *pUnit), 0xCB3D0)   -- now D2COMMON_STATLIST_ClampStaminaManaHP
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_AllocStatListEx, void, __stdcall, (D2UnitStrc *pUnit, char nFlags, StatListValueChangeFunc pfOnValueChanged, D2GameStrc *pGame), 0xCB3D6)   -- now D2COMMON_STATLIST_AllocStatListEx
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_SetItemFlag, void, __stdcall, (D2UnitStrc *pItem, D2C_ItemFlags dwFlag, BOOL bSet), 0xCB3DC)   -- now D2COMMON_ITEMS_SetItemFlag
//  D2FUNC(D2CLIENT, D2COMMON_UNITROOM_UpdatePath, void, __stdcall, (D2UnitStrc *pUnit), 0xCB3E2)   -- now D2COMMON_UNITROOM_UpdatePath
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_FreeCollisionPath, void, __stdcall, (D2UnitStrc *pUnit), 0xCB3E8)   -- now D2COMMON_UNITS_FreeCollisionPath
//  D2FUNC(D2CLIENT, D2COMMON_UNITROOM_IsUnitInRoom, BOOL, __stdcall, (D2ActiveRoomStrc *pRoom, D2UnitStrc *pUnit), 0xCB3EE)   -- now D2COMMON_UNITROOM_IsUnitInRoom
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_FreeInventory, void, __stdcall, (D2InventoryStrc *pInventory), 0xCB3F4)   -- now D2COMMON_INVENTORY_FreeInventory
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_UpdateFrame, void, __stdcall, (D2UnitStrc *pUnit), 0xCB3FA)   -- now D2COMMON_UNITS_UpdateFrame
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_IsAtEndOfFrameCycle, int, __stdcall, (D2UnitStrc *pUnit), 0xCB400)   -- now D2COMMON_UNITS_IsAtEndOfFrameCycle
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetCompactItemDataFromBitstream, BOOL, __stdcall, (uint8_t *pBitstream, size_t nSize, BOOL bCheckForHeader, D2ItemSaveStrc *pItemSave), 0xCB406)   -- now D2COMMON_ITEMS_GetCompactItemDataFromBitstream
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_CheckIfFlagIsSet, int, __stdcall, (int nFlags, D2C_ItemFlags nFlag), 0xCB40C)   -- now D2COMMON_ITEMS_CheckIfFlagIsSet
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_DecodeItemFromBitstream, size_t, __stdcall, (D2UnitStrc *pItem, uint8_t *pBitstream, size_t nSize, BOOL bCheckForHeader, int *pSocketedItemCount, uint32_t dwVersion, BOOL *pFail), 0xCB412)   -- now D2COMMON_ITEMS_DecodeItemFromBitstream
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_SetInvPage, void, __stdcall, (D2UnitStrc *pItem, uint8_t nPage), 0xCB418)   -- now D2COMMON_ITEMS_SetInvPage
//  D2FUNC(D2CLIENT, D2COMMON_10254_Return0, void, __stdcall, (int a1, int a2), 0xCB41E)   -- now D2COMMON_10254_Return0
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_GetSecondWieldingWeapon, void, __stdcall, (D2UnitStrc *pPlayer, D2InventoryStrc *pInventory, D2UnitStrc **ppItem, D2C_PlayerBodyLocs nBodyLoc), 0xCB424)   -- now D2COMMON_INVENTORY_GetSecondWieldingWeapon
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetItemComponentId, uint8_t, __stdcall, (D2UnitStrc *pUnit, D2UnitStrc *pItem), 0xCB42A)   -- now D2COMMON_UNITS_GetItemComponentId
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_UpdateWeaponGUIDOnRemoval, void, __stdcall, (D2InventoryStrc *pInventory, D2UnitStrc *pItem), 0xCB430)   -- now D2COMMON_INVENTORY_UpdateWeaponGUIDOnRemoval
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_UpdateWeaponGUIDOnInsert, void, __stdcall, (D2InventoryStrc *pInventory, D2UnitStrc *pItem), 0xCB436)   -- now D2COMMON_INVENTORY_UpdateWeaponGUIDOnInsert
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_IsCharmUsable, BOOL, __stdcall, (D2UnitStrc *pItem, D2UnitStrc *pPlayer), 0xCB43C)   -- now D2COMMON_ITEMS_IsCharmUsable
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_UpdateSets, BOOL, __stdcall, (D2UnitStrc *pUnit, D2UnitStrc *pItem, int a3, int a4), 0xCB442)   -- now D2COMMON_ITEMS_UpdateSets
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetGemApplyType, int, __stdcall, (D2UnitStrc *pItem), 0xCB448)   -- now D2COMMON_ITEMS_GetGemApplyType
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_Return, void, __stdcall, (const char *szFile, int nLine, D2InventoryStrc *pInventory, int nX, int nY, D2C_InventoryRecords nInventoryRecordId, BOOL bClient, uint8_t nPage), 0xCB44E)   -- now D2COMMON_INVENTORY_Return
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_PlaceItemAtInventoryPage, BOOL, __stdcall, (D2InventoryStrc *pInventory, D2UnitStrc *pItem, int nXPos, int nYPos, D2C_InventoryRecords nInventoryRecordId, BOOL bUnused, uint8_t nPage), 0xCB454)   -- now D2COMMON_INVENTORY_PlaceItemAtInventoryPage
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_PlaceItemInSocket, BOOL, __stdcall, (D2InventoryStrc *pInventory, D2UnitStrc *pItem, int nUnused), 0xCB45A)   -- now D2COMMON_INVENTORY_PlaceItemInSocket
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_SetYForStaticUnit, void, __stdcall, (D2UnitStrc *pUnit, int nY), 0xCB460)   -- now D2COMMON_UNITS_SetYForStaticUnit
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_SetXForStaticUnit, void, __stdcall, (D2UnitStrc *pUnit, int nX), 0xCB466)   -- now D2COMMON_UNITS_SetXForStaticUnit
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetUnitNameFromUnit, char *, __stdcall, (D2UnitStrc *pUnit, char *szName), 0xCB46C)   -- now D2COMMON_DATATBLS_GetUnitNameFromUnit
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_PlaceItemInBodyLoc, BOOL, __stdcall, (D2InventoryStrc *pInventory, D2UnitStrc *pItem, D2C_PlayerBodyLocs nBodyLoc), 0xCB472)   -- now D2COMMON_INVENTORY_PlaceItemInBodyLoc
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetWeaponClassId, uint16_t, __stdcall, (D2UnitStrc *pItem), 0xCB478)   -- now D2COMMON_ITEMS_GetWeaponClassId
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_SetBodyLocation, void, __stdcall, (D2UnitStrc *pItem, uint8_t nBodyLoc), 0xCB47E)   -- now D2COMMON_ITEMS_SetBodyLocation
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetItemFlags, D2C_ItemFlags, __stdcall, (D2UnitStrc *pItem), 0xCB484)   -- now D2COMMON_ITEMS_GetItemFlags
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_PlaceItemInBeltSlot, BOOL, __stdcall, (D2InventoryStrc *pInventory, D2UnitStrc *pItem, int nSlot), 0xCB48A)   -- now D2COMMON_INVENTORY_PlaceItemInBeltSlot
//  D2FUNC(D2CLIENT, D2COMMON_10267_Return, void, __stdcall, (int a1, int a2), 0xCB490)   -- now D2COMMON_10267_Return
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_SetBaseStat2, void, __stdcall, (D2StatListStrc *pStatList, D2C_ItemStats nStatId, int nValue, uint16_t nLayer, D2UnitStrc *pUnit), 0xCB496)   -- now D2COMMON_STATLIST_SetBaseStat2
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_GetStatListFromFlag, D2StatListStrc *, __stdcall, (D2StatListStrc *pStatList, int nFlag), 0xCB49C)   -- now D2COMMON_STATLIST_GetStatListFromFlag
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_GetStatListFromUnitAndFlag, D2StatListStrc *, __stdcall, (D2UnitStrc *pUnit, int nFlag), 0xCB4A2)   -- now D2COMMON_STATLIST_GetStatListFromUnitAndFlag
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_RemoveAllItems, BOOL, __stdcall, (D2InventoryStrc *pInventory), 0xCB4A8)   -- now D2COMMON_INVENTORY_RemoveAllItems
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_GetBodyLocFromEquippedItem, int, __stdcall, (D2InventoryStrc *pInventory, D2UnitStrc *pItem), 0xCB4AE)   -- now D2COMMON_INVENTORY_GetBodyLocFromEquippedItem
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_GetLevel, uint32_t, __stdcall, (D2UnitStrc *pMissile), 0xCB4B4)   -- now D2COMMON_MISSILE_GetLevel
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_GetSkill, int, __stdcall, (D2UnitStrc *pMissile), 0xCB4BA)   -- now D2COMMON_MISSILE_GetSkill
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_EvaluateMissileFormula, int, __stdcall, (D2UnitStrc *pMissile, D2UnitStrc *pOwner, unsigned int nCalc, int nMissileId, int nLevel), 0xCB4C0)   -- now D2COMMON_MISSILE_EvaluateMissileFormula
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_GetCurrentFrame, int, __stdcall, (D2UnitStrc *pMissile), 0xCB4C6)   -- now D2COMMON_MISSILE_GetCurrentFrame
//  D2FUNC(D2CLIENT, D2COMMON_PATH_SetPrecisionY, void, __stdcall, (D2DynamicPathStrc *pDynamicPath, int nPrecisionY), 0xCB4CC)   -- now D2COMMON_PATH_SetPrecisionY
//  D2FUNC(D2CLIENT, D2COMMON_PATH_SetPrecisionX, void, __stdcall, (D2DynamicPathStrc *pDynamicPath, int nPrecisionX), 0xCB4D2)   -- now D2COMMON_PATH_SetPrecisionX
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_GetRemainingFrames, int, __stdcall, (D2UnitStrc *pMissile), 0xCB4D8)   -- now D2COMMON_MISSILE_GetRemainingFrames
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_GetTargetX, int, __stdcall, (D2UnitStrc *pMissile), 0xCB4DE)   -- now D2COMMON_MISSILE_GetTargetX
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_GetStreamRange, int, __stdcall, (D2UnitStrc *pMissile), 0xCB4E4)   -- now D2COMMON_MISSILE_GetStreamRange
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_GetStreamMissile, int, __stdcall, (D2UnitStrc *pMissile), 0xCB4EA)   -- now D2COMMON_MISSILE_GetStreamMissile
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_GetTargetY, int, __stdcall, (D2UnitStrc *pMissile), 0xCB4F0)   -- now D2COMMON_MISSILE_GetTargetY
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_SetOwner, void, __stdcall, (D2UnitStrc *pMissile, D2UnitStrc *pOwner), 0xCB4F6)   -- now D2COMMON_MISSILE_SetOwner
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_CheckUnitIfOwner, BOOL, __stdcall, (D2UnitStrc *pMissile, D2UnitStrc *pUnit), 0xCB4FC)   -- now D2COMMON_MISSILE_CheckUnitIfOwner
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_GetUnitAlignment, int, __stdcall, (D2UnitStrc *pUnit), 0xCB502)   -- now D2COMMON_STATLIST_GetUnitAlignment
//  D2FUNC(D2CLIENT, D2COMMON_PATH_AllocDynamicPath, void, __stdcall, (void *pMemPool, D2ActiveRoomStrc *pRoom, int nX, int nY, D2UnitStrc *pUnit, BOOL bSetFlag), 0xCB508)   -- now D2COMMON_PATH_AllocDynamicPath
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_AllocMissileData, void, __stdcall, (D2UnitStrc *pMissile), 0xCB50E)   -- now D2COMMON_MISSILE_AllocMissileData
//  D2FUNC(D2CLIENT, D2COMMON_PATH_FreeDynamicPath, void, __stdcall, (void *pMemPool, D2DynamicPathStrc *pDynamicPath), 0xCB514)   -- now D2COMMON_PATH_FreeDynamicPath
//  D2FUNC(D2CLIENT, D2COMMON_10228, void, __stdcall, (D2UnitStrc *pUnit), 0xCB51A)   -- now D2COMMON_10228
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_FreeMissileData, void, __stdcall, (D2UnitStrc *pMissile), 0xCB520)   -- now D2COMMON_MISSILE_FreeMissileData
//  D2FUNC(D2CLIENT, D2COMMON_MISSTREAM_FreeMissileStream, void, __stdcall, (D2UnitStrc *pMissile), 0xCB526)   -- now D2COMMON_MISSTREAM_FreeMissileStream
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_GetClassId, int, __stdcall, (D2UnitStrc *pMissile), 0xCB52C)   -- now D2COMMON_MISSILE_GetClassId
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_SetAnimSpeed, void, __stdcall, (D2UnitStrc *pMissile, int nAnimSpeed), 0xCB532)   -- now D2COMMON_MISSILE_SetAnimSpeed
//  D2FUNC(D2CLIENT, D2COMMON_10375_UNITS_SetFrameNonRate, void, __stdcall, (D2UnitStrc *pUnit, int nRate, int nFailRate), 0xCB538)   -- now D2COMMON_10375_UNITS_SetFrameNonRate
//  D2FUNC(D2CLIENT, D2COMMON_10198_PathGetSaveStep, int, __stdcall, (D2DynamicPathStrc *pDynamicPath, D2PathPointStrc **ppPathPoints), 0xCB53E)   -- now D2COMMON_10198_PathGetSaveStep
//  D2FUNC(D2CLIENT, D2COMMON_10201, uint16_t, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCB544)   -- now D2COMMON_10201
//  D2FUNC(D2CLIENT, D2COMMON_10226, BOOL, __stdcall, (D2UnitStrc *pUnit, signed int a2), 0xCB54A)   -- now D2COMMON_10226
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_GetActivateFrame, int, __stdcall, (D2UnitStrc *pMissile), 0xCB550)   -- now D2COMMON_MISSILE_GetActivateFrame
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_ResetLightMap, void *, __stdcall, (D2UnitStrc *pUnit), 0xCB556)   -- now D2COMMON_UNITS_ResetLightMap
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_StoreOwner, void, __stdcall, (D2UnitStrc *pUnit, D2UnitStrc *pOwner), 0xCB55C)   -- now D2COMMON_UNITS_StoreOwner
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_SetLevel, void, __stdcall, (D2UnitStrc *pMissile, uint16_t nLevel), 0xCB562)   -- now D2COMMON_MISSILE_SetLevel
//  D2FUNC(D2CLIENT, D2COMMON_MISSILE_SetSkill, void, __stdcall, (D2UnitStrc *pMissile, D2C_Skills nSkill), 0xCB568)   -- now D2COMMON_MISSILE_SetSkill
//  D2FUNC(D2CLIENT, D2COMMON_PATH_SetMaxVelocity, void, __stdcall, (D2DynamicPathStrc *pDynamicPath, int nMaxVelocity), 0xCB56E)   -- now D2COMMON_PATH_SetMaxVelocity
//  D2FUNC(D2CLIENT, D2COMMON_PATH_SetAcceleration, void, __stdcall, (D2DynamicPathStrc *pDynamicPath, int nAcceleration), 0xCB574)   -- now D2COMMON_PATH_SetAcceleration
//  D2FUNC(D2CLIENT, D2COMMON_PATH_GetMaxDistance, uint8_t, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCB57A)   -- now D2COMMON_PATH_GetMaxDistance
//  D2FUNC(D2CLIENT, D2COMMON_PATH_GetType, int, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCB580)   -- now D2COMMON_PATH_GetType
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetMonSoundsTxtRecordFromSoundId, D2MonSoundsTxt *, __stdcall, (int nSoundId), 0xCB586)   -- now D2COMMON_DATATBLS_GetMonSoundsTxtRecordFromSoundId
//  D2FUNC(D2CLIENT, D2COMMON_MONSTERS_GetMaximalLightRadius, uint8_t, __stdcall, (D2UnitStrc *pMonster), 0xCB58C)   -- now D2COMMON_MONSTERS_GetMaximalLightRadius
//  D2FUNC(D2CLIENT, D2COMMON_11062_Return, void, __stdcall, (int a1, int a2), 0xCB592)   -- now D2COMMON_11062_Return
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_SetSkillMode, void, __stdcall, (D2SkillStrc *pSkill, int nSkillMode), 0xCB598)   -- now D2COMMON_SKILLS_SetSkillMode
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_AllocSkillList, D2SkillListStrc *, __stdcall, (void *pMemPool), 0xCB59E)   -- now D2COMMON_SKILLS_AllocSkillList
//  D2FUNC(D2CLIENT, D2COMMON_PATH_SetUnitCollisionPattern, void, __stdcall, (D2UnitStrc *pUnit, int nCollisionPattern), 0xCB5A4)   -- now D2COMMON_PATH_SetUnitCollisionPattern
//  D2FUNC(D2CLIENT, D2COMMON_PATH_RemoveCollisionFootprintForUnit, BOOL, __stdcall, (D2UnitStrc *pUnit, BOOL bForce), 0xCB5AA)   -- now D2COMMON_PATH_RemoveCollisionFootprintForUnit
//  D2FUNC(D2CLIENT, D2COMMON_MONSTERS_ApplyClassicScaling, void, __stdcall, (D2UnitStrc *pMonster, BOOL bExpansion, uint8_t nDifficulty), 0xCB5B0)   -- now D2COMMON_MONSTERS_ApplyClassicScaling
//  D2FUNC(D2CLIENT, D2COMMON_PATH_SetUnitAliveCollision, void, __stdcall, (D2UnitStrc *pUnit, BOOL bForGameLogic), 0xCB5B6)   -- now D2COMMON_PATH_SetUnitAliveCollision
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_StopSequence, void, __stdcall, (D2UnitStrc *pUnit), 0xCB5BC)   -- now D2COMMON_UNITS_StopSequence
//  D2FUNC(D2CLIENT, D2COMMON_10193_PATH_AdjustDirection, void, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCB5C2)   -- now D2COMMON_10193_PATH_AdjustDirection
//  D2FUNC(D2CLIENT, D2COMMON_10213, void, __stdcall, (D2UnitStrc *pUnit), 0xCB5C8)   -- now D2COMMON_10213
//  D2FUNC(D2CLIENT, D2COMMON_PATH_GetTargetTypeAndGUID, void, __stdcall, (D2DynamicPathStrc *pDynamicPath, int *pTargetType, D2UnitGUID *pTargetGUID), 0xCB5CE)   -- now D2COMMON_PATH_GetTargetTypeAndGUID
//  D2FUNC(D2CLIENT, D2COMMON_MONSTERS_IsSandLeaper, BOOL, __stdcall, (D2UnitStrc *pMonster, BOOL bAlwaysReturnFalse), 0xCB5D4)   -- now D2COMMON_MONSTERS_IsSandLeaper
//  D2FUNC(D2CLIENT, D2COMMON_11050, int, __stdcall, (D2UnitStrc *pUnit, int a2), 0xCB5DA)   -- now D2COMMON_11050
//  D2FUNC(D2CLIENT, D2COMMON_PATH_ResetToPreviousType, void, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCB5E0)   -- now D2COMMON_PATH_ResetToPreviousType
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_DeactivateTemporaryStates, void, __stdcall, (D2UnitStrc *pUnit), 0xCB5E6)   -- now D2COMMON_STATLIST_DeactivateTemporaryStates
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_ComputeSequenceAnimation, void, __stdcall, (D2AnimSeqTxt *pMonSeqTxt, int nTargetFramePoint, int nCurrentFramePoint, unsigned int *pMode, unsigned int *pFrame, int *pDirection, int *pEvent), 0xCB5EC)   -- now D2COMMON_DATATBLS_ComputeSequenceAnimation
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetSeqFramePointsCount, int, __stdcall, (D2UnitStrc *pUnit), 0xCB5F2)   -- now D2COMMON_DATATBLS_GetSeqFramePointsCount
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskShatterOnUnit, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCB5F8)   -- now D2COMMON_STATES_CheckStateMaskShatterOnUnit
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskHideOnUnit, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCB5FE)   -- now D2COMMON_STATES_CheckStateMaskHideOnUnit
//  D2FUNC(D2CLIENT, D2COMMON_PATH_AddCollisionFootprintForOptionalUnit, void, __stdcall, (D2UnitStrc *pUnit), 0xCB604)   -- now D2COMMON_PATH_AddCollisionFootprintForOptionalUnit
//  D2FUNC(D2CLIENT, D2COMMON_COLLISION_SetMask, void, __stdcall, (D2ActiveRoomStrc *pRoom, int nX, int nY, uint16_t nMask), 0xCB60A)   -- now D2COMMON_COLLISION_SetMask
//  D2FUNC(D2CLIENT, D2COMMON_COLLISION_ResetMask, void, __stdcall, (D2ActiveRoomStrc *pRoom, int nX, int nY, uint16_t nMask), 0xCB610)   -- now D2COMMON_COLLISION_ResetMask
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetHirelingTxtRecordFromIdAndLevel, D2HirelingTxt *, __stdcall, (BOOL bExpansion, int nId, int nLevel), 0xCB616)   -- now D2COMMON_DATATBLS_GetHirelingTxtRecordFromIdAndLevel
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_CanSwitchAI, BOOL, __fastcall, (int nMonsterId), 0xCB61C)   -- now D2COMMON_UNITS_CanSwitchAI
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_RollRandomUniqueAppellationString, const Unicode *, __fastcall, (D2UnitStrc *pUnit), 0xCB622)   -- now D2COMMON_DATATBLS_RollRandomUniqueAppellationString
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_RollRandomUniquePrefixString, const Unicode *, __fastcall, (D2UnitStrc *pUnit), 0xCB628)   -- now D2COMMON_DATATBLS_RollRandomUniquePrefixString
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_RollRandomUniqueSuffixString, const Unicode *, __fastcall, (D2UnitStrc *pUnit), 0xCB62E)   -- now D2COMMON_DATATBLS_RollRandomUniqueSuffixString
//  D2FUNC(D2CLIENT, D2COMMON_ENVIRONMENT_GetCycleIndexFromAct, int, __stdcall, (D2DrlgActStrc *pAct), 0xCB634)   -- now D2COMMON_ENVIRONMENT_GetCycleIndexFromAct
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GetTownLevelIdFromActNo, int, __stdcall, (uint8_t nAct), 0xCB63A)   -- now D2COMMON_DUNGEON_GetTownLevelIdFromActNo
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetShrineTxtRecordFromObject, D2ShrinesTxt *, __stdcall, (D2UnitStrc *pUnit), 0xCB640)   -- now D2COMMON_UNITS_GetShrineTxtRecordFromObject
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_SetShrineTxtRecordInObjectData, void, __stdcall, (D2UnitStrc *pUnit, D2ShrinesTxt *pShrinesTxtRecord), 0xCB646)   -- now D2COMMON_UNITS_SetShrineTxtRecordInObjectData
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetShrinesTxtRecord, D2ShrinesTxt *, __stdcall, (int nShrineId), 0xCB64C)   -- now D2COMMON_DATATBLS_GetShrinesTxtRecord
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_SetObjectPortalFlags, void, __stdcall, (D2UnitStrc *pUnit, uint8_t nPortalFlag), 0xCB652)   -- now D2COMMON_UNITS_SetObjectPortalFlags
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetObjectPortalFlags, uint8_t, __stdcall, (D2UnitStrc *pUnit), 0xCB658)   -- now D2COMMON_UNITS_GetObjectPortalFlags
//  D2FUNC(D2CLIENT, D2COMMON_10954, void, __stdcall, (D2UnitStrc *pUnit, D2UnitGUID nOwnerGUID, int nSkillId, int nSkillLevel, int nCharges, BOOL bFreeMemory), 0xCB65E)   -- now D2COMMON_10954
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_AllocPlayerData, void, __stdcall, (D2UnitStrc *pUnit), 0xCB664)   -- now D2COMMON_UNITS_AllocPlayerData
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_InitSkillList, void, __stdcall, (D2UnitStrc *pUnit), 0xCB66A)   -- now D2COMMON_SKILLS_InitSkillList
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_FreePlayerData, void, __stdcall, (void *pMemPool, D2UnitStrc *pPlayer), 0xCB670)   -- now D2COMMON_UNITS_FreePlayerData
//  D2FUNC(D2CLIENT, D2COMMON_COLLISION_CheckMaskWithPattern, uint16_t, __stdcall, (D2ActiveRoomStrc *pRoom, int nX, int nY, D2C_CollisionPattern nCollisionPattern, uint16_t nMask), 0xCB676)   -- now D2COMMON_COLLISION_CheckMaskWithPattern
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_CheckLOSDraw, int, __stdcall, (D2ActiveRoomStrc *pRoom), 0xCB67C)   -- now D2COMMON_DUNGEON_CheckLOSDraw
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_InitRoomTileAnimation, void, __stdcall, (D2DrlgActStrc *pAct, D2ActiveRoomStrc *pRoom1, D2ActiveRoomStrc *pRoom2), 0xCB682)   -- now D2COMMON_DUNGEON_InitRoomTileAnimation
//  D2FUNC(D2CLIENT, D2COMMON_PATH_GetNextRoom, D2ActiveRoomStrc *, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCB688)   -- now D2COMMON_PATH_GetNextRoom
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GetPortalFlagFromLevelId, int, __stdcall, (int nPortalLevelId), 0xCB68E)   -- now D2COMMON_DUNGEON_GetPortalFlagFromLevelId
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetPlayerPortalFlags, int, __stdcall, (D2UnitStrc *pUnit), 0xCB694)   -- now D2COMMON_UNITS_GetPlayerPortalFlags
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_SetCurrentRoomInvalid, void, __stdcall, (D2UnitStrc *pUnit, int a2), 0xCB69A)   -- now D2COMMON_UNITS_SetCurrentRoomInvalid
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_IsCurrentRoomInvalid, int, __stdcall, (D2UnitStrc *pUnit), 0xCB6A0)   -- now D2COMMON_UNITS_IsCurrentRoomInvalid
//  D2FUNC(D2CLIENT, D2COMMON_STATES_IsUnitShapeShifted, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCB6A6)   -- now D2COMMON_STATES_IsUnitShapeShifted
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_TestCollisionBetweenInteractingUnits, BOOL, __stdcall, (D2UnitStrc *pUnit1, D2UnitStrc *pUnit2, int nCollisionMask), 0xCB6AC)   -- now D2COMMON_UNITS_TestCollisionBetweenInteractingUnits
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetRunAndWalkSpeedForPlayer, void, __stdcall, (int nUnused, int nCharId, int *pWalkSpeed, int *pRunSpeed), 0xCB6B2)   -- now D2COMMON_UNITS_GetRunAndWalkSpeedForPlayer
//  D2FUNC(D2CLIENT, D2COMMON_10178_PATH_GetLastPointY, int, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCB6B8)   -- now D2COMMON_10178_PATH_GetLastPointY
//  D2FUNC(D2CLIENT, D2COMMON_10177_PATH_GetLastPointX, int, __stdcall, (D2DynamicPathStrc *pDynamicPath), 0xCB6BE)   -- now D2COMMON_10177_PATH_GetLastPointX
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_IsObjectInInteractRange, BOOL, __stdcall, (D2UnitStrc *pUnit, D2UnitStrc *pObject), 0xCB6C4)   -- now D2COMMON_UNITS_IsObjectInInteractRange
//  D2FUNC(D2CLIENT, D2COMMON_11016, int, __stdcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill), 0xCB6CA)   -- now D2COMMON_11016
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_IsInMovingModeEx, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCB6D0)   -- now D2COMMON_UNITS_IsInMovingModeEx
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetComponent, uint8_t, __stdcall, (D2UnitStrc *pItem), 0xCB6D6)   -- now D2COMMON_ITEMS_GetComponent
//  D2FUNC(D2CLIENT, D2COMMON_10230, BOOL, __stdcall, (D2DynamicPathStrc *pDynamicPath, int a2, D2ActiveRoomStrc *pRoom, unsigned int a4, __int16 a5), 0xCB6DC)   -- now D2COMMON_10230
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_Get1OfNFromOverlayTxt, uint32_t, __stdcall, (int nOverlayId), 0xCB6E2)   -- now D2COMMON_DATATBLS_Get1OfNFromOverlayTxt
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskAuraOnUnit, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCB6E8)   -- now D2COMMON_STATES_CheckStateMaskAuraOnUnit
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskAuraByStateId, BOOL, __stdcall, (D2C_States nState), 0xCB6EE)   -- now D2COMMON_STATES_CheckStateMaskAuraByStateId
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_RemoveAllStats, void, __stdcall, (D2StatListStrc *pStatList), 0xCB6F4)   -- now D2COMMON_STATLIST_RemoveAllStats
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_GetState, int, __stdcall, (D2StatListStrc *pStatList), 0xCB6FA)   -- now D2COMMON_STATLIST_GetState
//  D2FUNC(D2CLIENT, D2COMMON_SKILLS_GetUseState, int, __stdcall, (D2UnitStrc *pUnit, D2SkillStrc *pSkill), 0xCB700)   -- now D2COMMON_SKILLS_GetUseState
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_TestCollisionWithUnit, BOOL, __stdcall, (D2UnitStrc *pUnit1, D2UnitStrc *pUnit2, int nCollisionMask), 0xCB706)   -- now D2COMMON_UNITS_TestCollisionWithUnit
//  D2FUNC(D2CLIENT, D2COMMON_DRLG_GetLOSDrawFromLevelId, int, __stdcall, (D2C_Levels nLevelId), 0xCB70C)   -- now D2COMMON_DRLG_GetLOSDrawFromLevelId
//  D2FUNC(D2CLIENT, D2COMMON_STATLIST_UpdateStatListsExpiration, void, __stdcall, (D2UnitStrc *pUnit, int nFrame), 0xCB712)   -- now D2COMMON_STATLIST_UpdateStatListsExpiration
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetMonSoundsTxtRecordFromMonsterId, D2MonSoundsTxt *, __stdcall, (int nMonsterId), 0xCB718)   -- now D2COMMON_DATATBLS_GetMonSoundsTxtRecordFromMonsterId
//  D2FUNC(D2CLIENT, D2COMMON_10885_COMPOSIT_unk, void, __stdcall, (D2UnitStrc *pUnit, char *szPath, int *pWeaponClassCode, BOOL bAddPathPrefix, int a5, D2InventoryStrc *pInventory, int nAnimMode), 0xCB71E)   -- now D2COMMON_10885_COMPOSIT_unk
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetColor, uint8_t *, __stdcall, (D2UnitStrc *pPlayer, D2UnitStrc *pItem, uint8_t *pColor, int nTransType), 0xCB724)   -- now D2COMMON_ITEMS_GetColor
//  D2FUNC(D2CLIENT, D2COMMON_STATES_CheckStateMaskGreenOnUnit, BOOL, __stdcall, (D2UnitStrc *pUnit), 0xCB72A)   -- now D2COMMON_STATES_CheckStateMaskGreenOnUnit
//  D2FUNC(D2CLIENT, D2COMMON_UNITS_GetOverlayHeight, int, __stdcall, (D2UnitStrc *pUnit), 0xCB730)   -- now D2COMMON_UNITS_GetOverlayHeight
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetMonModeTxtRecord, D2MonModeTxt *, __stdcall, (int nIndex, int bGetMode), 0xCB736)   -- now D2COMMON_DATATBLS_GetMonModeTxtRecord
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetArmTypeTxtRecord, D2ArmTypeTxt *, __stdcall, (int nId), 0xCB73C)   -- now D2COMMON_DATATBLS_GetArmTypeTxtRecord
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetObjModeTypeTxtRecord, D2ObjModeTypeTxt *, __stdcall, (int nIndex, int bGetMode), 0xCB742)   -- now D2COMMON_DATATBLS_GetObjModeTypeTxtRecord
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetPlrModeTypeTxtRecord, D2PlrModeTypeTxt *, __stdcall, (int nIndex, int bGetMode), 0xCB748)   -- now D2COMMON_DATATBLS_GetPlrModeTypeTxtRecord
//  D2FUNC(D2CLIENT, D2COMMON_DATATBLS_GetCompositTxtRecord, D2CompositTxt *, __stdcall, (int nComposit), 0xCB74E)   -- now D2COMMON_DATATBLS_GetCompositTxtRecord
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetVarInvGfxString, char *, __stdcall, (D2UnitStrc *pItem, int nId), 0xCB754)   -- now D2COMMON_ITEMS_GetVarInvGfxString
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetVarInvGfxCount, uint8_t, __stdcall, (D2UnitStrc *pItem), 0xCB75A)   -- now D2COMMON_ITEMS_GetVarInvGfxCount
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetVarGfxIndex, uint8_t, __stdcall, (D2UnitStrc *pItem), 0xCB760)   -- now D2COMMON_ITEMS_GetVarGfxIndex
//  D2FUNC(D2CLIENT, D2COMMON_ITEMS_GetAltGfx, uint32_t, __stdcall, (D2UnitStrc *pItem), 0xCB766)   -- now D2COMMON_ITEMS_GetAltGfx
//  D2FUNC(D2CLIENT, D2COMMON_COMPOSIT_GetArmorTypeFromComponent, unsigned int, __stdcall, (D2Composits nComponent, uint8_t *pArmorComponents), 0xCB76C)   -- now D2COMMON_COMPOSIT_GetArmorTypeFromComponent
//  D2FUNC(D2CLIENT, D2COMMON_COMPOSIT_IsArmorComponent, BOOL, __stdcall, (D2Composits nComponent), 0xCB772)   -- now D2COMMON_COMPOSIT_IsArmorComponent
//  D2FUNC(D2CLIENT, D2COMMON_INVENTORY_GetCompositItem, D2UnitStrc *, __stdcall, (D2InventoryStrc *pInventory, int nComponent), 0xCB778)   -- now D2COMMON_INVENTORY_GetCompositItem
//  D2FUNC(D2CLIENT, D2COMMON_11069, uint32_t, __stdcall, (D2UnitStrc *pMonster, unsigned int nIndex, unsigned int nComponent), 0xCB77E)   -- now D2COMMON_11069
//  D2FUNC(D2CLIENT, D2COMMON_11070, int, __stdcall, (int nMonsterId, unsigned int nComponent, unsigned int a3), 0xCB784)   -- now D2COMMON_11070
//  D2FUNC(D2CLIENT, D2COMMON_DUNGEON_GameToClientCoords, void, __stdcall, (int *pX, int *pY), 0xCB78A)   -- now D2COMMON_DUNGEON_GameToClientCoords
//  D2FUNC(D2CLIENT, D2GAME_GAME_CloseAllGames, void, __stdcall, (), 0xCB790)   -- now D2GAME_GAME_CloseAllGames
//  D2FUNC(D2CLIENT, D2GAME_GAME_UpdateClients, void, __stdcall, (int32_t a1, int32_t a2), 0xCB796)   -- now D2GAME_GAME_UpdateClients
//  D2FUNC(D2CLIENT, D2GAME_GAME_UpdateGamesProgress, int32_t, __stdcall, (int32_t a1), 0xCB79C)   -- now D2GAME_GAME_UpdateGamesProgress
//  D2FUNC(D2CLIENT, D2GAME_GAME_ProcessNetworkMessages, void, __fastcall, (), 0xCB7A2)   -- now D2GAME_GAME_ProcessNetworkMessages
//  D2FUNC(D2CLIENT, D2GAME_GAME_GetPlayerUnitsCount, int32_t, __stdcall, (uint16_t nGameId), 0xCB7A8)   -- now D2GAME_GAME_GetPlayerUnitsCount
//  D2FUNC(D2CLIENT, D2GAME_10036_PLRSAVE_EnableSaveFileWriting, void, __fastcall, (int32_t bWriteSaveFile), 0xCB7AE)   -- now D2GAME_10036_PLRSAVE_EnableSaveFileWriting
//  D2FUNC(D2CLIENT, D2GAME_GAME_SetGlobalAct, void, __stdcall, (D2C_Acts nAct), 0xCB7B4)   -- now D2GAME_GAME_SetGlobalAct
//  D2FUNC(D2CLIENT, D2GAME_GAME_SetInitSeed, void, __stdcall, (int32_t nInitSeed), 0xCB7BA)   -- now D2GAME_GAME_SetInitSeed
//  D2FUNC(D2CLIENT, D2GAME_10046, int, __stdcall, (), 0xCB7C0)   -- now D2GAME_10046
//  D2FUNC(D2CLIENT, D2GAME_10050, int, __stdcall, (), 0xCB7C6)   -- now D2GAME_10050
//  D2FUNC(D2CLIENT, D2GAME_GAME_InitGameDataTable, void, __stdcall, (SGameDataTableStrc *pGameDataTbl, void *pGameList), 0xCB7CC)   -- now D2GAME_GAME_InitGameDataTable
//  D2FUNC(D2CLIENT, D2GAME_PLAYER_SetVirtualPlayerCount, void, __fastcall, (int32_t nPlayers), 0xCB7D2)   -- now D2GAME_PLAYER_SetVirtualPlayerCount
//  D2FUNC(D2CLIENT, D2GAME_DEBUG_GetUnitFromTypeAndGUID, D2UnitStrc *, __stdcall, (D2C_UnitTypes nUnitType, int32_t nUnitGUID), 0xCB7D8)   -- now D2GAME_DEBUG_GetUnitFromTypeAndGUID
//  D2FUNC(D2CLIENT, D2GFX_GetResolutionMode, D2GameResolutionMode, __stdcall, (), 0xCB7DE)   -- now D2GFX_GetResolutionMode
//  D2FUNC(D2CLIENT, D2GFX_WINDOW_OpenSmackCutscene, void, __stdcall, (const char *szFile, D2GameResolutionMode nResolution, void (__cdecl *pfFrame)()), 0xCB7E4)   -- now D2GFX_WINDOW_OpenSmackCutscene
//  D2FUNC(D2CLIENT, D2GFX_WINDOW_ShowAll, void, __stdcall, (), 0xCB7EA)   -- now D2GFX_WINDOW_ShowAll
//  D2FUNC(D2CLIENT, D2GFX_WINDOW_GetWindow, HWND, __stdcall, (), 0xCB7F0)   -- now D2GFX_WINDOW_GetWindow
//  D2FUNC(D2CLIENT, D2GFX_SetGamma, int32_t, __stdcall, (uint32_t dwGamma), 0xCB7F6)   -- now D2GFX_SetGamma
//  D2FUNC(D2CLIENT, D2GFX_GetCacheResults, uint32_t *, __stdcall, (), 0xCB7FC)   -- now D2GFX_GetCacheResults
//  D2FUNC(D2CLIENT, D2GFX_GetDisplayType, int, __stdcall, (), 0xCB802)   -- now D2GFX_GetDisplayType
//  D2FUNC(D2CLIENT, D2GFX_OutputString, void, __cdecl, (int32_t nXPos, int32_t nYPos, const char *szFormat, ...), 0xCB808)   -- now D2GFX_OutputString
//  D2FUNC(D2CLIENT, D2GFX_SCALE_AdjustPerspectivePosition, void, __stdcall, (int32_t nXPos, int32_t nYPos, int32_t nBais, int32_t *pXAdjust, int32_t *pYAdjust), 0xCB80E)   -- now D2GFX_SCALE_AdjustPerspectivePosition
//  D2FUNC(D2CLIENT, D2GFX_FloorTileDraw, BOOL, __stdcall, (D2TileLibraryEntryStrc *pTile, D2GfxLightExStrc *pLight, int32_t nXPos, int32_t nYPos, int32_t nWorldXpos, int32_t nWorldYpos, uint8_t nAlpha, int32_t nScreenPanels, BOOL bFloorFilter), 0xCB814)   -- now D2GFX_FloorTileDraw
//  D2FUNC(D2CLIENT, D2GFX_CheckPerspective, int32_t, __stdcall, (), 0xCB81A)   -- now D2GFX_CheckPerspective
//  D2FUNC(D2CLIENT, D2GFX_CheckLowQuality, int32_t, __stdcall, (), 0xCB820)   -- now D2GFX_CheckLowQuality
//  D2FUNC(D2CLIENT, D2GFX_DebugDraw, void, __stdcall, (uint32_t *pLight, uint32_t *pPlayerLight, int32_t nXPos, int32_t nYPos), 0xCB826)   -- now D2GFX_DebugDraw
//  D2FUNC(D2CLIENT, D2GFX_DrawBoxAlpha, void, __stdcall, (int32_t nXStart, int32_t nYStart, int32_t nXEnd, int32_t nYEnd, uint32_t dwColor, uint8_t nAlpha), 0xCB82C)   -- now D2GFX_DrawBoxAlpha
//  D2FUNC(D2CLIENT, D2GFX_TILE_TileDrawLit, int32_t, __stdcall, (D2TileLibraryEntryStrc *pTile, int32_t nXPos, int32_t nYPos, D2GfxLightStrc *pLight, int32_t nScreenPanels), 0xCB832)   -- now D2GFX_TILE_TileDrawLit
//  D2FUNC(D2CLIENT, D2GFX_TILE_TileDrawTrans, int32_t, __stdcall, (D2TileLibraryEntryStrc *pTile, int32_t nXPos, int32_t nYPos, D2GfxLightStrc *pLight, int32_t nScreenPanels, uint8_t nAlpha), 0xCB838)   -- now D2GFX_TILE_TileDrawTrans
//  D2FUNC(D2CLIENT, D2GFX_TILE_ShadowTileDraw, int32_t, __stdcall, (D2TileLibraryEntryStrc *pTile, int32_t nXPos, int32_t nYPos, int32_t nDrawMode, int32_t nScreenPanels), 0xCB83E)   -- now D2GFX_TILE_ShadowTileDraw
//  D2FUNC(D2CLIENT, D2GFX_DrawBox, void, __stdcall, (int32_t nXStart, int32_t nYStart, int32_t nXEnd, int32_t nYEnd, uint8_t nColor, int eDrawMode), 0xCB844)   -- now D2GFX_DrawBox
//  D2FUNC(D2CLIENT, D2GFX_DrawLine, void, __stdcall, (int32_t nXStart, int32_t nYStart, int32_t nXEnd, int32_t nYEnd, uint8_t nColor, uint8_t nAlpha), 0xCB84A)   -- now D2GFX_DrawLine
//  D2FUNC(D2CLIENT, D2GFX_TEXTURE_CelDraw, int32_t, __stdcall, (D2GfxDataStrc *pData, int32_t nXPos, int32_t nYPos, uint32_t dwGamma, int eDrawMode, uint8_t *pPalette), 0xCB850)   -- now D2GFX_TEXTURE_CelDraw
//  D2FUNC(D2CLIENT, D2GFX_SCALE_CheckPerspectivePosition, int32_t, __stdcall, (int32_t nXPos, int32_t nYPos), 0xCB856)   -- now D2GFX_SCALE_CheckPerspectivePosition
//  D2FUNC(D2CLIENT, D2GFX_WINDOW_IsFullScreen, int32_t, __stdcall, (), 0xCB85C)   -- now D2GFX_WINDOW_IsFullScreen
//  D2FUNC(D2CLIENT, D2GFX_Release, int32_t, __stdcall, (), 0xCB862)   -- now D2GFX_Release
//  D2FUNC(D2CLIENT, D2GFX_WINDOW_Destroy, int32_t, __stdcall, (), 0xCB868)   -- now D2GFX_WINDOW_Destroy
//  D2FUNC(D2CLIENT, D2GFX_EndScene, int32_t, __stdcall, (), 0xCB86E)   -- now D2GFX_EndScene
//  D2FUNC(D2CLIENT, D2GFX_ClearScreen, void, __stdcall, (int32_t bPartial), 0xCB874)   -- now D2GFX_ClearScreen
//  D2FUNC(D2CLIENT, D2GFX_StartDraw, int32_t, __stdcall, (int32_t bClear, uint8_t nRed, uint8_t nGreen, uint8_t nBlue), 0xCB87A)   -- now D2GFX_StartDraw
//  D2FUNC(D2CLIENT, D2GFX_SetAmbientColor, void, __stdcall, (uint8_t nRed, uint8_t nGreen, uint8_t nBlue), 0xCB880)   -- now D2GFX_SetAmbientColor
//  D2FUNC(D2CLIENT, D2GFX_WINDOW_GetState, int32_t, __stdcall, (), 0xCB886)   -- now D2GFX_WINDOW_GetState
//  D2FUNC(D2CLIENT, D2GFX_EnableVSync, void, __stdcall, (), 0xCB88C)   -- now D2GFX_EnableVSync
//  D2FUNC(D2CLIENT, D2GFX_SetViewMatrixAdjust, void, __fastcall, (int32_t nAdjust), 0xCB892)   -- now D2GFX_SetViewMatrixAdjust
//  D2FUNC(D2CLIENT, D2GFX_SCALE_SetDefaultFactor, void, __stdcall, (), 0xCB898)   -- now D2GFX_SCALE_SetDefaultFactor
//  D2FUNC(D2CLIENT, D2GFX_SCALE_SetPerspectiveScale, void, __stdcall, (int32_t nXScale, int32_t nYScale), 0xCB89E)   -- now D2GFX_SCALE_SetPerspectiveScale
//  D2FUNC(D2CLIENT, D2GFX_SCALE_GetFactor, int32_t, __stdcall, (), 0xCB8A4)   -- now D2GFX_SCALE_GetFactor
//  D2FUNC(D2CLIENT, D2GFX_SCALE_UpdateFactor, void, __stdcall, (), 0xCB8AA)   -- now D2GFX_SCALE_UpdateFactor
//  D2FUNC(D2CLIENT, D2GFX_SCALE_ScalePerspectivePosition, void, __stdcall, (int32_t nXPos, int32_t nYPos, int32_t nAngle, int32_t *pXAdjust, int32_t *pYAdjust, int32_t bOrder), 0xCB8B0)   -- now D2GFX_SCALE_ScalePerspectivePosition
//  D2FUNC(D2CLIENT, D2GFX_TEXTURE_CelDrawClipped, void, __stdcall, (D2GfxDataStrc *pData, int32_t nXPos, int32_t nYPos, void *pCropRect, int eDrawMode), 0xCB8B6)   -- now D2GFX_TEXTURE_CelDrawClipped
//  D2FUNC(D2CLIENT, D2GFX_TEXTURE_CelDrawColor, void, __stdcall, (D2GfxDataStrc *pData, int32_t nXPos, int32_t nYPos, uint32_t dwGamma, int eDrawMode, int32_t nGlobalPaletteShift), 0xCB8BC)   -- now D2GFX_TEXTURE_CelDrawColor
//  D2FUNC(D2CLIENT, D2GFX_TEXTURE_CelDrawEx, void, __stdcall, (D2GfxDataStrc *pData, int32_t nXPos, int32_t nYPos, int32_t nSkipLines, int32_t nDrawLines, int eDrawMode), 0xCB8C2)   -- now D2GFX_TEXTURE_CelDrawEx
//  D2FUNC(D2CLIENT, D2GFX_SetLowQuality, void, __stdcall, (int32_t bLowQuality), 0xCB8C8)   -- now D2GFX_SetLowQuality
//  D2FUNC(D2CLIENT, D2GFX_SetBlendShadows, void, __stdcall, (int32_t bBlendedShadows), 0xCB8CE)   -- now D2GFX_SetBlendShadows
//  D2FUNC(D2CLIENT, D2GFX_CheckUnitPerspective, int32_t, __stdcall, (), 0xCB8D4)   -- now D2GFX_CheckUnitPerspective
//  D2FUNC(D2CLIENT, D2GFX_SetPerspective, void, __stdcall, (int32_t bPerspective), 0xCB8DA)   -- now D2GFX_SetPerspective
//  D2FUNC(D2CLIENT, D2GFX_GammaCanBeControlled, int, __stdcall, (), 0xCB8E0)   -- now D2GFX_GammaCanBeControlled
//  D2FUNC(D2CLIENT, D2GFX_SetOption, int32_t, __fastcall, (int32_t nOption, int32_t nValue), 0xCB8E6)   -- now D2GFX_SetOption
//  D2FUNC(D2CLIENT, D2GFX_SetUnitPerspective, void, __stdcall, (int32_t bPerspective), 0xCB8EC)   -- now D2GFX_SetUnitPerspective
//  D2FUNC(D2CLIENT, D2GFX_HardwareAcceleratedRenderMode_10006, int, __stdcall, (), 0xCB8F2)   -- now D2GFX_HardwareAcceleratedRenderMode_10006
//  D2FUNC(D2CLIENT, D2GFX_SCALE_SetDefaultFactorPerspectiveFactor, void, __stdcall, (), 0xCB8F8)   -- now D2GFX_SCALE_SetDefaultFactorPerspectiveFactor
//  D2FUNC(D2CLIENT, D2GFX_WINDOW_ShowCursor, void, __stdcall, (int32_t bShow), 0xCB8FE)   -- now D2GFX_WINDOW_ShowCursor
//  D2FUNC(D2CLIENT, D2GFX_HardwareAcceleratedRenderMode, int32_t, __stdcall, (), 0xCB904)   -- now D2GFX_HardwareAcceleratedRenderMode
//  D2FUNC(D2CLIENT, D2GFX_TEXTURE_CelDrawShadow, void, __stdcall, (D2GfxDataStrc *pData, int32_t nXPos, int32_t nYPos), 0xCB90A)   -- now D2GFX_TEXTURE_CelDrawShadow
//  D2FUNC(D2CLIENT, D2GFX_TEXTURE_CelDrawHilight, void, __stdcall, (D2GfxDataStrc *pData, int32_t nXPos, int32_t nYPos, uint8_t nPaletteIndex), 0xCB910)   -- now D2GFX_TEXTURE_CelDrawHilight
//  D2FUNC(D2CLIENT, D2GFX_TEXTURE_CelFlatSpriteDraw, void, __stdcall, (D2GfxDataStrc *pData, int32_t nXPos, int32_t nYPos, uint32_t dwGamma, int eDrawMode, int32_t nScreenMode, uint8_t *pPalette), 0xCB916)   -- now D2GFX_TEXTURE_CelFlatSpriteDraw

//D2FUNC(D2CLIENT, ARCHIVE_OpenFileOrTrace, int32_t, __fastcall, (int32_t nUnused, const char *pszFileName, void **phFile, int32_t bQuietIfMissing), 0xCB91C)
//D2FUNC(D2CLIENT, ARCHIVE_CloseFile, int32_t, __fastcall, (void *hArchive, void *hFile), 0xCB958)
//D2FUNC(D2CLIENT, ARCHIVE_GetFileSize, uint32_t, __fastcall, (void *hArchive, void *hFile, uint32_t *pdwSizeHigh), 0xCB97E)
//D2FUNC(D2CLIENT, ARCHIVE_ReadFile, int32_t, __fastcall, (void *hArchive, void *hFile, void *pBuffer, uint32_t dwBytesToRead), 0xCB9ED)
//D2FUNC(D2CLIENT, ARCHIVE_LoadFile, void *, __fastcall, (void *hArchive, const char *szFilename, uint32_t *pdwFileSize, const char *szSrcFile, int32_t nSrcLine), 0xCBA94)
//D2FUNC(D2CLIENT, FAULT_SetAssertHandler, void *, __fastcall, (void *pfnNewHandler), 0xCBB88)
//D2FUNC(D2CLIENT, FAULT_RegisterMessageSource, void, __fastcall, (int32_t (__stdcall *pfnMessageSource)(DWORD dwMessageId, LPSTR pszBuffer, int32_t nMaxLength)), 0xCBB94)
//D2FUNC(D2CLIENT, FAULT_Init, void, __cdecl, (), 0xCBBCF)
//D2FUNC(D2CLIENT, FAULT_AtExit, void, __cdecl, (), 0xCBBF9)
//D2FUNC(D2CLIENT, FAULT_PromptErrorOrCancel, BOOL, __fastcall, (LPCSTR lpText), 0xCBC27)
//D2FUNC(D2CLIENT, FAULT_TLinkedListFreeAll, void, __fastcall, (TSExplicitList *pList), 0xCBC59)
//D2FUNC(D2CLIENT, FAULT_TLinkedListAllocNode, D2MessageSourceStrc *, __thiscall, (TSExplicitList *pList, uint32_t nLinkType, uint32_t nExtraBytes, uint32_t dwAllocFlags), 0xCBC89)
//D2FUNC(D2CLIENT, FAULT_TLinkUnlinkThunk, void, __fastcall, (TSLink *pLink), 0xCBCD3)
//D2FUNC(D2CLIENT, FAULT_TLinkedListLink, void, __thiscall, (TSExplicitList *pList, D2MessageSourceStrc *pNode, uint32_t nLinkType, D2MessageSourceStrc *pExisting), 0xCBCD8)
//D2FUNC(D2CLIENT, FAULT_TLinkUnlink, void, __fastcall, (TSLink *pLink), 0xCBD6E)
//D2FUNC(D2CLIENT, CLIENT_VirtualAlloc, LPVOID, __fastcall, (SIZE_T dwSize), 0xCBD9A)
//D2FUNC(D2CLIENT, CLIENT_VirtualFree, BOOL, __fastcall, (LPVOID lpAddress), 0xCBDAB)

//  D2FUNC(D2CLIENT, STORM_SFileGetFileName, int, __stdcall, (void *hFile, char *szFileName, int nMaxLen), 0xCBDBA)   -- now STORM_SFileGetFileName
//  D2FUNC(D2CLIENT, D2LANG_STRTABLE_GetLanguage, D2C_Language, __cdecl, (), 0xCBDC0)   -- now D2LANG_STRTABLE_GetLanguage
//  D2FUNC(D2CLIENT, D2LANG_10006, int, __stdcall, (_DWORD, _DWORD), 0xCBDC6)   -- now D2LANG_10006
//  D2FUNC(D2CLIENT, D2LANG_10001, int, __cdecl, (), 0xCBDCC)   -- now D2LANG_10001
//  D2FUNC(D2CLIENT, D2LANG_10000, int, __fastcall, (_DWORD, _DWORD, _DWORD), 0xCBDD2)   -- now D2LANG_10000
//  D2FUNC(D2CLIENT, D2LANG_GetStringFromTblIndex, const Unicode *, __fastcall, (D2C_StringIndices nStringIndex), 0xCBDD8)   -- now D2LANG_GetStringFromTblIndex
//  D2FUNC(D2CLIENT, D2LANG_STR_GroupUintDigits, void, __stdcall, (Unicode *pUnicode, unsigned int dwValue, int nMaxLength), 0xCBDDE)   -- now D2LANG_STR_GroupUintDigits
//  D2FUNC(D2CLIENT, D2LANG_STR_GroupIntDigits, void, __stdcall, (Unicode *pUnicode, int nValue, int nMaxLength), 0xCBDE4)   -- now D2LANG_STR_GroupIntDigits
//  D2FUNC(D2CLIENT, D2LANG_GetStringByReferenceString, int, __fastcall, (_DWORD), 0xCBDEA)   -- now D2LANG_GetStringByReferenceString
//  D2FUNC(D2CLIENT, D2MCPCLIENT_10003, int, __cdecl, (), 0xCBDF0)   -- now D2MCPCLIENT_10003
//  D2FUNC(D2CLIENT, D2NET_10017, int32_t, __stdcall, (), 0xCBDF6)   -- now D2NET_10017
//  D2FUNC(D2CLIENT, D2NET_CLIENT_DequeueGamePacket, int32_t, __stdcall, (uint8_t *pBuffer, uint32_t nBufferSize), 0xCBDFC)   -- now D2NET_CLIENT_DequeueGamePacket
//  D2FUNC(D2CLIENT, D2NET_CLIENT_DequeueSystemPacket, int32_t, __stdcall, (uint8_t *pBuffer, uint32_t nBufferSize), 0xCBE02)   -- now D2NET_CLIENT_DequeueSystemPacket
//  D2FUNC(D2CLIENT, D2NET_CLIENT_Send, int32_t, __stdcall, (int32_t nUnused, const uint8_t *pBuffer, int32_t nBufferSize), 0xCBE08)   -- now D2NET_CLIENT_Send
//  D2FUNC(D2CLIENT, D2NET_10025, int32_t, __stdcall, (), 0xCBE0E)   -- now D2NET_10025
//  D2FUNC(D2CLIENT, D2NET_CLIENT_Initialize, void, __stdcall, (int32_t a1, const char *szIpAddress), 0xCBE14)   -- now D2NET_CLIENT_Initialize
//  D2FUNC(D2CLIENT, D2NET_SERVER_SetMaxClientsPerGame, void, __stdcall, (int32_t nMaxClients), 0xCBE1A)   -- now D2NET_SERVER_SetMaxClientsPerGame
//  D2FUNC(D2CLIENT, D2NET_SERVER_SetHackListEnabled, void, __stdcall, (BOOL bEnabled), 0xCBE20)   -- now D2NET_SERVER_SetHackListEnabled
//  D2FUNC(D2CLIENT, D2NET_SERVER_Initialize, void, __stdcall, (int32_t a1, int32_t a2), 0xCBE26)   -- now D2NET_SERVER_Initialize
//  D2FUNC(D2CLIENT, D2NET_SERVER_Release, void, __stdcall, (), 0xCBE2C)   -- now D2NET_SERVER_Release
//  D2FUNC(D2CLIENT, D2NET_CLIENT_Release, void, __stdcall, (), 0xCBE32)   -- now D2NET_CLIENT_Release
//  D2FUNC(D2CLIENT, D2NET_SERVER_GetServerPacketSize, int32_t, __fastcall, (D2PacketBufferStrc *pBuffer, uint32_t nBufferSize, int32_t *pSize), 0xCBE38)   -- now D2NET_SERVER_GetServerPacketSize
//  D2FUNC(D2CLIENT, D2NET_10040, int, __stdcall, (), 0xCBE3E)   -- now D2NET_10040
//  D2FUNC(D2CLIENT, D2NET_10039, int, __stdcall, (), 0xCBE44)   -- now D2NET_10039
//  D2FUNC(D2CLIENT, D2SOUND_10027_ApplyMasterVolume, void, __fastcall, (unsigned int nVolume), 0xCBE4A)   -- now D2SOUND_10027_ApplyMasterVolume
//  D2FUNC(D2CLIENT, D2SOUND_10031_StopStreamSample, void, __cdecl, (), 0xCBE50)   -- now D2SOUND_10031_StopStreamSample
//  D2FUNC(D2CLIENT, D2SOUND_10029_FadeOutMasterVolume, void, __fastcall, (int nDurationMs), 0xCBE56)   -- now D2SOUND_10029_FadeOutMasterVolume
//  D2FUNC(D2CLIENT, D2SOUND_10032_IsStreamSamplePlaying, int, __cdecl, (), 0xCBE5C)   -- now D2SOUND_10032_IsStreamSamplePlaying
//  D2FUNC(D2CLIENT, D2SOUND_10047_SetMasterVolumeSetting, int, __thiscall, (int nValue), 0xCBE62)   -- now D2SOUND_10047_SetMasterVolumeSetting
//  D2FUNC(D2CLIENT, D2SOUND_10046_GetMasterVolumeSetting, int, __cdecl, (), 0xCBE68)   -- now D2SOUND_10046_GetMasterVolumeSetting
//  D2FUNC(D2CLIENT, D2SOUND_10049_SetMusicVolumeSetting, int, __thiscall, (int nValue), 0xCBE6E)   -- now D2SOUND_10049_SetMusicVolumeSetting
//  D2FUNC(D2CLIENT, D2SOUND_10048_GetMusicVolumeSetting, int, __cdecl, (), 0xCBE74)   -- now D2SOUND_10048_GetMusicVolumeSetting
//  D2FUNC(D2CLIENT, D2SOUND_10051_SetPositionalBiasSetting, int, __thiscall, (int nValue), 0xCBE7A)   -- now D2SOUND_10051_SetPositionalBiasSetting
//  D2FUNC(D2CLIENT, D2SOUND_10050_GetPositionalBiasSetting, int, __cdecl, (), 0xCBE80)   -- now D2SOUND_10050_GetPositionalBiasSetting
//  D2FUNC(D2CLIENT, D2SOUND_10053_SetNpcSpeechSetting, int, __thiscall, (int nValue), 0xCBE86)   -- now D2SOUND_10053_SetNpcSpeechSetting
//  D2FUNC(D2CLIENT, D2SOUND_10052_GetNpcSpeechSetting, int, __cdecl, (), 0xCBE8C)   -- now D2SOUND_10052_GetNpcSpeechSetting
//  D2FUNC(D2CLIENT, D2SOUND_10058_ListPushFront, D2SoundListNodeStrc *, __fastcall, (D2SoundListNodeStrc **ppHead, int nValue), 0xCBE92)   -- now D2SOUND_10058_ListPushFront
//  D2FUNC(D2CLIENT, D2SOUND_10059_ListRemove, void, __fastcall, (D2SoundListNodeStrc **ppHead, int nValue), 0xCBE98)   -- now D2SOUND_10059_ListRemove
//  D2FUNC(D2CLIENT, D2SOUND_10056_ParseWaveData, void *, __fastcall, (void *pRiff, unsigned int *pdwSize, void *pWaveFormatEx), 0xCBE9E)   -- now D2SOUND_10056_ParseWaveData
//  D2FUNC(D2CLIENT, D2SOUND_10006_SetListenerPosition, int, __thiscall, (float *pfCoords), 0xCBEA4)   -- now D2SOUND_10006_SetListenerPosition
//  D2FUNC(D2CLIENT, D2SOUND_10004_Is3dListenerAllowed, int, __cdecl, (), 0xCBEAA)   -- now D2SOUND_10004_Is3dListenerAllowed
//  D2FUNC(D2CLIENT, D2SOUND_10044_GetSoundMixerSetting, int, __cdecl, (), 0xCBEB0)   -- now D2SOUND_10044_GetSoundMixerSetting
//  D2FUNC(D2CLIENT, D2SOUND_10025_PlaySampleStream, int, __fastcall, (void *ptSample, int nFileId, int nStartBlock, int bLoop), 0xCBEB6)   -- now D2SOUND_10025_PlaySampleStream
//  D2FUNC(D2CLIENT, D2SOUND_10011_PlaySampleLooping, int, __thiscall, (void *ptSample), 0xCBEBC)   -- now D2SOUND_10011_PlaySampleLooping
//  D2FUNC(D2CLIENT, D2SOUND_10022_SetSampleEaxLevel, int, __thiscall, (void *ptSample, float fLevel), 0xCBEC2)   -- now D2SOUND_10022_SetSampleEaxLevel
//  D2FUNC(D2CLIENT, D2SOUND_10010_SetSampleWaveData, int, __fastcall, (void *ptSample, void *pbWaveData, unsigned int dwWaveSize), 0xCBEC8)   -- now D2SOUND_10010_SetSampleWaveData
//  D2FUNC(D2CLIENT, D2SOUND_10020_SetSampleLoopBlockStart, void, __fastcall, (void *ptSample, int dwLoopBlockStart), 0xCBECE)   -- now D2SOUND_10020_SetSampleLoopBlockStart
//  D2FUNC(D2CLIENT, D2SOUND_10019, void, __fastcall, (void *ptSample, int nValue), 0xCBED4)   -- now D2SOUND_10019
//  D2FUNC(D2CLIENT, D2SOUND_10009_ResetSample, void, __thiscall, (void *ptSample), 0xCBEDA)   -- now D2SOUND_10009_ResetSample
//  D2FUNC(D2CLIENT, D2SOUND_10012_StopSample, void, __thiscall, (void *ptSample), 0xCBEE0)   -- now D2SOUND_10012_StopSample
//  D2FUNC(D2CLIENT, D2SOUND_10018_GetSampleStreamPosition, unsigned int, __thiscall, (void *ptSample), 0xCBEE6)   -- now D2SOUND_10018_GetSampleStreamPosition
//  D2FUNC(D2CLIENT, D2SOUND_10013_IsSampleStarted, int, __thiscall, (void *ptSample), 0xCBEEC)   -- now D2SOUND_10013_IsSampleStarted
//  D2FUNC(D2CLIENT, D2SOUND_10014_SetSampleVolume, int, __fastcall, (void *ptSample, int nVolume), 0xCBEF8)   -- now D2SOUND_10014_SetSampleVolume
//  D2FUNC(D2CLIENT, D2SOUND_10021_SetSampleReverbMix, void, __thiscall, (void *ptSample, float fReverbMix), 0xCBEFE)   -- now D2SOUND_10021_SetSampleReverbMix
//  D2FUNC(D2CLIENT, D2SOUND_10017_SetSample3dPosition, int, __thiscall, (void *ptSample, int nX, int nY, int nZ), 0xCBF04)   -- now D2SOUND_10017_SetSample3dPosition
//  D2FUNC(D2CLIENT, D2SOUND_10016_SetSamplePan, int, __fastcall, (void *ptSample, int nPan), 0xCBF0A)   -- now D2SOUND_10016_SetSamplePan
//  D2FUNC(D2CLIENT, D2SOUND_10057_ComputeVolumeAndPan, int, __fastcall, (int *pnVolume, int *pnPan, float fX, float fY, float fZ), 0xCBF10)   -- now D2SOUND_10057_ComputeVolumeAndPan
//  D2FUNC(D2CLIENT, D2SOUND_10023_IsSampleStreaming, int, __thiscall, (void *ptSample), 0xCBF16)   -- now D2SOUND_10023_IsSampleStreaming
//  D2FUNC(D2CLIENT, D2SOUND_10045_SetSoundMixerSetting, int, __thiscall, (int nValue), 0xCBF1C)   -- now D2SOUND_10045_SetSoundMixerSetting
//  D2FUNC(D2CLIENT, D2SOUND_10007_AllocSample, int, __fastcall, (char nFlags), 0xCBF22)   -- now D2SOUND_10007_AllocSample
//  D2FUNC(D2CLIENT, D2SOUND_10008_FreeSample, int, __thiscall, (void *ptSample), 0xCBF28)   -- now D2SOUND_10008_FreeSample
//  D2FUNC(D2CLIENT, D2SOUND_10003_Is3dSampleAllowed, int, __cdecl, (), 0xCBF2E)   -- now D2SOUND_10003_Is3dSampleAllowed
//  D2FUNC(D2CLIENT, D2SOUND_10064_IsSampleBufferPlaying, int, __thiscall, (void *ptSample), 0xCBF34)   -- now D2SOUND_10064_IsSampleBufferPlaying
//  D2FUNC(D2CLIENT, D2SOUND_10063_ListPushBack, D2SoundListNodeStrc *, __fastcall, (D2SoundListNodeStrc **ppHead, int nValue), 0xCBF3A)   -- now D2SOUND_10063_ListPushBack
//  D2FUNC(D2CLIENT, D2SOUND_10062_ListFree, void, __thiscall, (D2SoundListNodeStrc **ppHead), 0xCBF40)   -- now D2SOUND_10062_ListFree
//  D2FUNC(D2CLIENT, D2SOUND_10060_ListContains, int, __fastcall, (D2SoundListNodeStrc *pHead, int nValue), 0xCBF46)   -- now D2SOUND_10060_ListContains
//  D2FUNC(D2CLIENT, D2WIN_GetArchive, HD2ARCHIVE, __stdcall, (), 0xCBF4C)   -- now D2WIN_GetArchive
//  D2FUNC(D2CLIENT, D2WIN_ARCHIVE_LoadExpansionArchives, BOOL, __fastcall, (ARCHIVE_ShowMessageFunctionPtr pfShowInsertPlayDisc, ARCHIVE_ShowMessageFunctionPtr pfShowInsertExpansionDisc, HANDLE hFile, D2ConfigStrc *pConfig), 0xCBF52)   -- now D2WIN_ARCHIVE_LoadExpansionArchives
//  D2FUNC(D2CLIENT, D2WIN_10177, int32_t, __fastcall, (uint8_t nAct), 0xCBF58)   -- now D2WIN_10177
//  D2FUNC(D2CLIENT, D2WIN_10004, void, __stdcall, (D2GameResolutionMode nResolution), 0xCBF5E)   -- now D2WIN_10004
//  D2FUNC(D2CLIENT, D2WIN_10003, int, __stdcall, (), 0xCBF64)   -- now D2WIN_10003
//  D2FUNC(D2CLIENT, D2WIN_ARCHIVE_LoadVideoArchives, BOOL, __fastcall, (ARCHIVE_ShowMessageFunctionPtr pfShowMessage, HANDLE hFile, int32_t bExpansion), 0xCBF6A)   -- now D2WIN_ARCHIVE_LoadVideoArchives
//  D2FUNC(D2CLIENT, D2WIN_ARCHIVE_UnloadExpansionArchives, void, __fastcall, (), 0xCBF70)   -- now D2WIN_ARCHIVE_UnloadExpansionArchives
//  D2FUNC(D2CLIENT, D2WIN_10016, int, __stdcall, (), 0xCBF76)   -- now D2WIN_10016
//  D2FUNC(D2CLIENT, D2WIN_10014_SetWndProc, int, __stdcall, (WNDPROC wndProc), 0xCBF7C)   -- now D2WIN_10014_SetWndProc
//  D2FUNC(D2CLIENT, D2WIN_10015, int, __fastcall, (), 0xCBF82)   -- now D2WIN_10015
//  D2FUNC(D2CLIENT, D2WIN_10005, int, __stdcall, (D2GameResolutionMode nResolution), 0xCBF88)   -- now D2WIN_10005
//  D2FUNC(D2CLIENT, D2WIN_10034_MixRGB, uint8_t, __stdcall, (uint8_t nRed, uint8_t nGreen, uint8_t nBlue), 0xCBF8E)   -- now D2WIN_10034_MixRGB
//  D2FUNC(D2CLIENT, D2WIN_10176, void, __fastcall, (int32_t a1), 0xCBF94)   -- now D2WIN_10176
//  D2FUNC(D2CLIENT, D2WIN_10019, void, __stdcall, (), 0xCBF9A)   -- now D2WIN_10019
//  D2FUNC(D2CLIENT, D2WIN_10179, void, __stdcall, (), 0xCBFA0)   -- now D2WIN_10179
//  D2FUNC(D2CLIENT, D2WIN_10200, void, __fastcall, (D2SplittedTextStrc *pSplitText), 0xCBFA6)   -- now D2WIN_10200
//  D2FUNC(D2CLIENT, D2WIN_10125, int, __stdcall, (), 0xCBFAC)   -- now D2WIN_10125
//  D2FUNC(D2CLIENT, D2WIN_10199, D2SplittedTextStrc *, __fastcall, (const Unicode *wszText, int *pLines, int nMaxLength), 0xCBFB2)   -- now D2WIN_10199
//  D2FUNC(D2CLIENT, D2WIN_10117_DrawText, void, __fastcall, (const Unicode *wszText, int nX, int nY, int nColor, BOOL bCentered), 0xCBFB8)   -- now D2WIN_10117_DrawText
//  D2FUNC(D2CLIENT, D2WIN_10121_GetTextWidth, int, __fastcall, (const Unicode *wszText), 0xCBFBE)   -- now D2WIN_10121_GetTextWidth
//  D2FUNC(D2CLIENT, D2WIN_10127_SetFont, enum Font, __fastcall, (enum Font nFontSize), 0xCBFC4)   -- now D2WIN_10127_SetFont
//  D2FUNC(D2CLIENT, D2WIN_10161, const char *, __stdcall, (int nPlayerClass, int nProgression, int nHardcoreFlag, int nExpansionFlag), 0xCBFCA)   -- now D2WIN_10161
//  D2FUNC(D2CLIENT, D2WIN_10028, int32_t, __stdcall, (const char *pszPalDatFilename, const char *pszPalPL2Filename), 0xCBFD0)   -- now D2WIN_10028
//  D2FUNC(D2CLIENT, D2WIN_10006_ClearDrawCaches, void, __stdcall, (), 0xCBFD6)   -- now D2WIN_10006_ClearDrawCaches
//  D2FUNC(D2CLIENT, D2WIN_10178, void, __fastcall, (D2ShadowLightGammaPaletteStrc *pBuffer), 0xCBFDC)   -- now D2WIN_10178
//  D2FUNC(D2CLIENT, D2WIN_10131_GetTextDimensions, void, __fastcall, (const Unicode *pText, int *pWidth, int *pHeight), 0xCBFE2)   -- now D2WIN_10131_GetTextDimensions
//  D2FUNC(D2CLIENT, D2WIN_10132, void, __fastcall, (const Unicode *wszText, int nX, int nY, DWORD dwColor, DrawMode eDrawMode, int nColor), 0xCBFE8)   -- now D2WIN_10132
//  D2FUNC(D2CLIENT, D2WIN_10129_DrawFramedText, void, __fastcall, (const Unicode *wszText, int nX, int nY, int nTextColor, int nAlign), 0xCBFEE)   -- now D2WIN_10129_DrawFramedText
//  D2FUNC(D2CLIENT, D2WIN_10128, unsigned int, __fastcall, (uint8_t a1), 0xCBFF4)   -- now D2WIN_10128
//  D2FUNC(D2CLIENT, D2WIN_10168_WINMAIN_CreateScreenshot, void, __stdcall, (), 0xCBFFA)   -- now D2WIN_10168_WINMAIN_CreateScreenshot
//  D2FUNC(D2CLIENT, D2WIN_10123, int, __fastcall, (const void *wszText, int nTextLength), 0xCC000)   -- now D2WIN_10123
//  D2FUNC(D2CLIENT, D2WIN_10134_DrawCellFile, void, __fastcall, (D2CellFileStrc *pCellFile, int nX, int nY, int nDiv, DrawMode eDrawMode, int nColor), 0xCC006)   -- now D2WIN_10134_DrawCellFile
//  D2FUNC(D2CLIENT, D2WIN_10119_DrawCroppedText, void, __fastcall, (const Unicode *wszText, int32_t nX, int32_t nY, int32_t a4, int32_t a5, int32_t a6), 0xCC00C)   -- now D2WIN_10119_DrawCroppedText
//  D2FUNC(D2CLIENT, D2WIN_10116, void, __stdcall, (), 0xCC012)   -- now D2WIN_10116
//  D2FUNC(D2CLIENT, D2WIN_10130_DrawFramedTextEx, void, __fastcall, (const Unicode *wszText, int nX, int nY, int nTextColor, int nAlign, int nRectangleColor), 0xCC018)   -- now D2WIN_10130_DrawFramedTextEx
//  D2FUNC(D2CLIENT, D2WIN_10124, void, __stdcall, (), 0xCC01E)   -- now D2WIN_10124
//  D2FUNC(D2CLIENT, D2WIN_10207, int, __stdcall, (), 0xCC024)   -- now D2WIN_10207
//  D2FUNC(D2CLIENT, D2WIN_10206, int, __stdcall, (), 0xCC02A)   -- now D2WIN_10206
//  D2FUNC(D2CLIENT, D2WIN_10185, int, __stdcall, (), 0xCC030)   -- now D2WIN_10185
//  D2FUNC(D2CLIENT, D2WIN_10175_GetPL2Shift, uint8_t *, __fastcall, (int nIndex), 0xCC036)   -- now D2WIN_10175_GetPL2Shift
//  D2FUNC(D2CLIENT, FOG_DisplayError, int, __cdecl, (_DWORD, _DWORD, _DWORD, _DWORD), 0xCC03C)   -- now FOG_DisplayError
//  D2FUNC(D2CLIENT, FOG_FReadFile, int, __fastcall, (_DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD, _DWORD), 0xCC042)   -- now FOG_FReadFile

///////////////////////////////////////////////////////////////////////////////
//
//  NOT EXPRESSIBLE THROUGH D2FUNC -- listed, not dropped.
//
//  Each line is a real function in the database. The reason it cannot be a
//  D2FUNC is on the line. To reach one of these, take its address with
//  GetDllOffset() directly and cast it to a hand-written typedef.
//
///////////////////////////////////////////////////////////////////////////////

// 0x11A0     _DllMain@12                                  the name is not a C identifier
// 0x45BD0    DATATBLS_ApplyRatio                          __userpurge -- no MSVC keyword for a register argument list
// 0xCBEF2    D2SOUND_10002_GetDirectSound                 undeclared type(s): LPDIRECTSOUND

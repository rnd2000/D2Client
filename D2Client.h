#pragma once

//  D2Client.h -- the module interface D2Client.dll presents to the rest of the game, and the declarations of the functions rewritten from it.
//  Hand-written; D2Constants.h, D2Structs.h, D2Ptrs.h and D2Vars.h are generated. Addresses are of D2Client.dll 1.10, image base 0x6FAA0000.
//
//  The whole module interface is one function pointer: gpQueryInterfaceVtbl at 0x6FB74108 is one 4-byte item holding DLLMAIN_Entrypoint
//  (0x6FAA11D0), which has no code reference and one data reference.
//
//  gdwBitMasks (Fog #10259) and sgptDataTables (D2Common #10042) are imported by name, as the original imports them. A data import slot holds
//  the address of the exported object: Fog exports the array, so one load of the slot and the element; D2Common exports a pointer, so two
//  loads and then the field. The compiler gets that depth by itself from these declarations.
extern "C" __declspec(dllimport) const uint32_t gdwBitMasks[32];        // Fog #10259; gdwInvBitMasks follows
extern "C" __declspec(dllimport) D2DataTablesStrc *sgptDataTables;   // D2Common #10042


//  The D2Lang Unicode functions D2Client imports are declared by struct Unicode in D2Structs.h, dllimport, and bound by name.
//
//  D2Sound.#10002 hands back DirectSound's own interface pointer. LPDIRECTSOUND is opaque on purpose: this rewrite never calls a method on it,
//  it only passes it back. Guarded, so including the real header first still wins.
#ifndef DIRECTSOUND_VERSION
struct IDirectSound;
typedef struct IDirectSound *LPDIRECTSOUND;
#endif

//  D2Lang's two case tables, private in Unicode, handed out by their friend. Not an inline member of Unicode: the class is dllimport, and a
//  build that does not inline such a member calls it through an import D2Lang does not export.
struct UnicodeTables
{
    static const uint16_t *toUpperTable() { return Unicode::_toUpperTable; }
    static const uint16_t *toLowerTable() { return Unicode::_toLowerTable; }
};


//  ---- rewritten from D2Client.dll ----

//  D2Client.0x6FAA11C0, export ordinal 10004 -- the only export this module gives a name to; 10001, 10002 and 10003 are by ordinal only.
extern "C" D2ClientInterface* __cdecl QueryInterface();

//  D2Client.0x6FAA11D0 -- returns int, as WINMAIN_D2ClientEntrypoint returns the eax it hands back unchanged.
int32_t __fastcall DLLMAIN_Entrypoint(D2ConfigStrc* pConfig);

//  D2Client.0x6FAA2050, C:\projects\D2\head\Diablo2\Source\D2Client\CORE\WINMAIN.CPP
int32_t __stdcall WINMAIN_D2ClientEntrypoint(HMODULE hModule, D2ConfigStrc* pConfig);

//  D2Client.0x6FB61C1D -- the PE entry point (RVA 0xC1C1D), the CRT's own.
extern "C" BOOL __stdcall DllEntryPoint(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);

//  D2Client.0x6FAA25D0, CORE\WNDPROC.CPP -- the message pump. Each caller hands it an idle callback; a non-zero return from that callback
//  leaves the pump and becomes this function's return value.
int32_t __fastcall WNDPROC_WindowRunMessagePump(int32_t(__stdcall* pfnIdle)(int32_t));

//  D2Client.0x6FAA8B10..0x6FAA8BB0, 0x6FAA8CB0 and 0x6FAA9420..0x6FAA9440, GAME\Game.cpp -- the input-state accessors, the graphics-teardown
//  handler and the leave-game flag pair.
void __fastcall GAME_SetRunKeyHeld(int32_t bHeld);
void __fastcall GAME_SetRunToggled(int32_t bRun);
int32_t __cdecl GAME_GetRunToggled();
void __fastcall GAME_SetStandStillKeyHeld(int32_t bHeld);
uint8_t __cdecl GAME_GetMovementFlags();
void __cdecl GAME_SuppressHeldClick();
int32_t __cdecl GAME_GetLeftClickModifierFlags();
int32_t __cdecl GAME_GetRightClickModifierFlags();
uint32_t __cdecl GAME_GetLeftButtonDown();
BOOL __cdecl GAME_GetLeftClickIssuedThisFrame();
int32_t __stdcall GAME_TeardownGraphics(int32_t nArg1, int32_t nArg2, int32_t nArg3);
void __fastcall GAME_SetLeaveGameSent(int32_t bSent);
int32_t __cdecl GAME_GetLeaveGameSent();

//  D2Client.0x6FAA8BC0..0x6FAA9100 but 0x6FAA8CB0, GAME\Game.cpp -- the game window's message handlers, Storm's SMSGHANDLER shape.
void __stdcall GAME_WindowOnClose(SMSGHANDLER_PARAMS *pMsg);
void __stdcall GAME_WindowOnLButtonDown(SMSGHANDLER_PARAMS *pMsg);
void __stdcall GAME_WindowOnLButtonUp(SMSGHANDLER_PARAMS *pMsg);
void __stdcall GAME_WindowOnRButtonDown(SMSGHANDLER_PARAMS *pMsg);
void __stdcall GAME_WindowOnRButtonUp(SMSGHANDLER_PARAMS *pMsg);
void __stdcall GAME_WindowOnMouseWheel(SMSGHANDLER_PARAMS *pMsg);
void __stdcall GAME_WindowOnMButtonUp(SMSGHANDLER_PARAMS *pMsg);
void __stdcall GAME_WindowOnMButtonDown(SMSGHANDLER_PARAMS *pMsg);
void __stdcall GAME_WindowOnXButtonDown(SMSGHANDLER_PARAMS *pMsg);
void __stdcall GAME_WindowOnXButtonUp(SMSGHANDLER_PARAMS *pMsg);
void __stdcall GAME_WindowOnSysCommand(SMSGHANDLER_PARAMS *pMsg);
void __stdcall GAME_WindowConsumeMessage(SMSGHANDLER_PARAMS *pMsg);
void __stdcall GAME_WindowOnActivateApp(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FAA8970, 0x6FAA9230..0x6FAA9290, GAME\Game.cpp -- gGameData's table destructor (__thiscall in the image, `this` alone, so
//  __fastcall), the Clear Screen key, the act environment re-applied, and the act preloaded from the seed packet.
void __fastcall GAME_GameDataDestructTable(SGameDataTableStrc *pTable);
int32_t __stdcall GAME_CmdTblActionClearScreen();
void __cdecl GAME_EnvReapplyActEnvironment();
void __fastcall GAME_PreloadActData(uint8_t nAct, uint32_t dwGameSeed, int32_t nObjectSeed, int32_t nTownLevelId);

//  D2Client.0x6FAAC320..0x6FAAD240, GAME\Game.cpp -- gGameData's container: its vtable slots and destructors, Storm's list primitives (TSLink,
//  TSList, TSGrowableArray), the bucket array's destructors, the record's destructors and allocator, and the chunk destructor.
//  The __thiscall members with stack arguments take an unused edx; those with `this` alone are plain __fastcall.
void __fastcall GAME_SGameDataFreeNode(SGameDataTableStrc *pTable, int32_t nEdxUnused, SGameDataNodeStrc *pNode);
void __fastcall GAME_SGameDataHashDestroyAll(SGameDataTableStrc *pTable);
SGameDataNodeStrc *__fastcall GAME_SGameDataAllocNode(SGameDataTableStrc *pTable, int32_t nEdxUnused, TSExplicitList *pList, int32_t nExtraBytes, int32_t nAllocFlags);
void __fastcall GAME_SGameDataListDestruct(SGameDataTableStrc *pTable);
void __fastcall GAME_SGameDataListDestroyAll(SGameDataTableStrc *pTable);
SGameDataTableStrc *__fastcall GAME_SGameDataVecDelDtor2(SGameDataTableStrc *pTable, int32_t nEdxUnused, uint8_t bDelete);
SGameDataNodeStrc *__fastcall GAME_SGameDataNodeVecDelDtor(SGameDataNodeStrc *pNode, int32_t nEdxUnused, uint8_t bDelete);
SGameDataTableStrc *__fastcall GAME_SGameDataVecDelDtor1(SGameDataTableStrc *pTable, int32_t nEdxUnused, uint8_t bDelete);
void __fastcall GAME_SGameDataDestructHashTable(SGameDataTableStrc *pTable);
SGameDataTableStrc *__fastcall GAME_SGameDataVecDelDtor0(SGameDataTableStrc *pTable, int32_t nEdxUnused, uint8_t bDelete);
void __fastcall GAME_SGameDataHashUnlinkNodesAndFree(TSExplicitListArray *pArray);
void __fastcall GAME_SGameDataUnlinkNode(SGameDataNodeStrc *pNode);
SGameDataNodeStrc *__fastcall GAME_SGameDataAllocAndLinkNode(TSExplicitList *pList, int32_t nEdxUnused, int32_t eLinkMode, int32_t nExtraBytes, int32_t nAllocFlags);
void __fastcall GAME_SGameDataHashUnlinkListsAndFree(TSExplicitListArray *pArray);
void __fastcall GAME_ListDestruct(TSExplicitList *pList);
SGameDataTableStrc *__fastcall GAME_SGameDataVecDelDtor3(SGameDataTableStrc *pTable, int32_t nEdxUnused, uint8_t bDelete);
void __fastcall GAME_SGameDataDestructor(SGameDataTableStrc *pTable);
TSLink *__fastcall GAME_ListNextLink(TSLink *pLink, int32_t nEdxUnused, int32_t nLinkOffset);
TSLink *__fastcall GAME_ListGetLinkSlot(TSExplicitList *pList, int32_t nEdxUnused, char *pNode);
void __fastcall GAME_ListUnlinkAllNodes(TSExplicitList *pList);
void *__fastcall GAME_ListVecDelDtor(TSExplicitList *pList, int32_t nEdxUnused, uint8_t bDelete);
void __fastcall GAME_SGameDataFreeRecordArray(SGameDataChunkStrc *pChunk);
void __fastcall GAME_ListUnlinkNode(TSLink *pLink);
TSLink *__fastcall GAME_ListNodeToLink(char *pNode, int32_t nLinkOffset);

//  D2Client.0x6FAA9450, GAME\Game.cpp -- everything a game needs before its first frame. It installs GAME_DrawGameScene into
//  gpfnActiveMessageCallback.
void __cdecl GAME_InitGame();

//  D2Client.0x6FAA9640, GAME\Game.cpp -- the draw half of a frame, called through gpfnActiveMessageCallback. bNoDefaultCursor = 0 draws the cursor.
void __fastcall GAME_DrawGameScene(int32_t bNoDefaultCursor);

//  D2Client.0x6FAA9850, GAME\Game.cpp -- start the disconnect / exit sequence with a message id, unless one is already running. An id of 29 or
//  above is clamped to 9, "unknown failure".
void __fastcall GAME_BeginDisconnect(D2C_JoinErrors nMessageId);

//  D2Client.0x6FAA9AF0, GAME\Game.cpp -- the body of a frame, and the idle callback GAME_AppModeGame hands to WNDPROC_WindowRunMessagePump.
//  A non-zero return ends that pump; the parameter is its iteration counter and is never read.
int32_t __stdcall GAME_UpdateFrame(int32_t nIdleIteration);

//  D2Client.0x6FAAA4E0, GAME\Game.cpp -- gafnAppModeEntryPoints[APPMODE_STARTUP].
D2C_AppMode __fastcall GAME_AppModeStartup();

//  D2Client.0x6FAAB370, GAME\Game.cpp -- gafnAppModeEntryPoints[APPMODE_GAME], which runs the main loop,
//  WNDPROC_WindowRunMessagePump(GAME_UpdateFrame).
D2C_AppMode __fastcall GAME_AppModeGame();

//  D2Client.0x6FAADD00, GAME\Msg.cpp -- gafnAppModeEntryPoints[APPMODE_JOIN_GAME].
D2C_AppMode __fastcall MSG_AppModeJoinGame();

//  D2Client.0x6FAADE60, GAME\Msg.cpp -- gafnAppModeEntryPoints[APPMODE_LEAVE_GAME].
D2C_AppMode __fastcall MSG_AppModeLeaveGame();

//  D2Client.0x6FAABB40, GAME\Game.cpp -- gafnAppModeEntryPoints[APPMODE_CLOSEWINDOW], the only handler that returns APPMODE_SHOULDQUIT.
D2C_AppMode __fastcall GAME_AppModeCloseWindow();

//  D2Client.0x6FAABB70..0x6FAABF10, GAME\Game.cpp -- the launch-parameter appliers. GAME_ApplyLaunchParams walks the 48 slots of
//  gLaunchParamHandlers over the D2ConfigStrc Game.exe hands WINMAIN_D2ClientEntrypoint; every applier is void.
//  GAME_NullFunc is the do-nothing slot of that table, of gSCmdTable and of CUnit.cpp's table walks.
void __fastcall GAME_ApplyParamDisableSaving(D2ConfigStrc *pConfig);
void __fastcall GAME_ApplyParamStartAct(D2ConfigStrc *pConfig);
void __fastcall GAME_ApplyParamJoinInfo(D2ConfigStrc *pConfig);
void __fastcall GAME_ApplyParamBattleNetIp(D2ConfigStrc *pConfig);
void __fastcall GAME_ApplyParamRealmIp(D2ConfigStrc *pConfig);
void __fastcall GAME_ApplyParamPlayerClass(D2ConfigStrc *pConfig);
void __fastcall GAME_ApplyParamGDI(D2ConfigStrc *pConfig);
void __fastcall GAME_ApplyParamGlide(D2ConfigStrc *pConfig);
void __fastcall GAME_ApplyParamOpenGL(D2ConfigStrc *pConfig);
void __fastcall GAME_ApplyParamDirect3D(D2ConfigStrc *pConfig);
void __fastcall GAME_ApplyParamRave(D2ConfigStrc *pConfig);
void __fastcall GAME_ApplyParamGamma(D2ConfigStrc *pConfig);
void __fastcall GAME_ApplyParamVSync(D2ConfigStrc *pConfig);
void __fastcall GAME_ApplyParamEnableLog(D2ConfigStrc *pConfig);
void __fastcall GAME_ApplyParamInitSeed(D2ConfigStrc *pConfig);
void __fastcall GAME_ApplyParamCharacterName(D2ConfigStrc *pConfig);
void __fastcall GAME_ApplyParamRealm(D2ConfigStrc *pConfig);
void __fastcall GAME_ApplyParamSetArenaFlag0(D2ConfigStrc *pConfig);
void __fastcall GAME_ApplyParamArenaPlayerClass(D2ConfigStrc *pConfig);
void __fastcall GAME_ApplyParamComInterface(D2ConfigStrc *pConfig);
void __cdecl GAME_NullFunc();
void __fastcall GAME_ApplyParamCompressedData(D2ConfigStrc *pConfig);
void __fastcall GAME_ApplyParamCompileTxt(D2ConfigStrc *pConfig);
BOOL __fastcall GAME_ApplyLaunchParams(D2ConfigStrc *pConfig);

//  D2Client.0x6FAABF20..0x6FAAC310, GAME\Game.cpp -- the client's state accessors, the mouse-handler pair and the leave after the ending
//  cinematic. The prototypes with no argument are __cdecl; with no argument no convention is observable.
int32_t __cdecl GAME_IsDisconnecting();
void __cdecl GAME_ResetClickState();
void __cdecl GAME_ResetMouseButtonState();
uint32_t __cdecl GAME_GetFrameCount();
uint32_t __cdecl GAME_GetFrameTick();
uint32_t __cdecl GAME_GetInteractThrottle();
void __fastcall GAME_SetInteractThrottle(uint32_t dwTick);
int32_t __cdecl GAME_GetRecentTickRate();
int32_t __cdecl GAME_GetRecentSkipRate();
int32_t __cdecl UNRESOLVED_sub_6FAABFE0();
eD2GameTypes __cdecl GAME_GetGameType();
void __fastcall GAME_ExtendCatchUpDeadline(int32_t nExtraMs);
int32_t __cdecl GAME_IsGreenBloodEnabled();
void __fastcall GAME_SetDifficulty(D2C_Difficulties nDifficulty);
void __fastcall GAME_SetExpansionGame(uint8_t bExpansion);
void __fastcall GAME_SetLadderGame(int32_t bLadder);
int32_t __cdecl GAME_IsLadderGame();
int32_t __cdecl GAME_IsExpansionGame();
uint8_t __cdecl GAME_GetDifficulty();
void __cdecl GAME_UnregisterMouseInputHandlers();
void __cdecl GAME_RegisterMouseInputHandlers();
void __cdecl GAME_LeaveGameAfterCinematic();

//  D2Client.0x6FAA9610..0x6FAAB360, GAME\Game.cpp -- the rest of the file between its claimers but GAME_ServerThreadProc, the beta slides and
//  the Diablo-vanquished screen. The prototypes with no argument are __cdecl.
void __cdecl j_ENV_UnloadImages();
void __fastcall GAME_SetActiveMessageCallback(int32_t nIndex);
int32_t __stdcall GAME_DrawDisconnectMessage(int32_t nUnused);
void __cdecl GAME_SendPingIfDue();
void __fastcall GAME_OnPongUpdateLatency(const uint8_t *pPacket);
void __fastcall GAME_SetSkillBlockUntilFrame(int32_t nFrame);
int32_t __cdecl GAME_GetSkillBlockUntilFrame();
int32_t __cdecl GAME_GetNetworkLatency();
int32_t __cdecl GAME_GetNetworkLatencyRunningMean();
void __cdecl GAME_MarkClassicCompletion();
void __cdecl GAME_SetGameTeardownActive();
void __cdecl GAME_SetDisconnecting();
BOOL __cdecl GAME_NeedsWin95WheelWorkaround();
int32_t __cdecl GAME_HasNativeMouseWheel();
BOOL __cdecl GAME_IsSupportedWindows();
BOOL __cdecl GAME_IsWin2000OrXp();
void __fastcall GAME_SetD2xOutCinematicPending(BOOL bPending);
struct BnClientInterface *__cdecl GAME_GetComInterface();

//  D2Client.0x6FAAA3B0..0x6FAAA7C1, GAME\Game.cpp -- the open-game server thread and the beta slide show. CRT_VectorConstructorIterator
//  (0x6FAAA950) is the compiler's and is not rewritten.
DWORD __stdcall GAME_ServerThreadProc(LPVOID lpThreadParameter);
void __stdcall GAME_BetaSlidesOnButtonDown(SMSGHANDLER_PARAMS *pMsg);
void __stdcall GAME_BetaSlidesOnKeyDown(SMSGHANDLER_PARAMS *pMsg);
int32_t __stdcall GAME_BetaSlidesTick(int32_t nUnused);

//  D2Client.0x6FAAA7D0..0x6FAAB1E9, GAME\Game.cpp -- the Diablo-vanquished screen and its three handlers.
void __stdcall GAME_VanquishedOnButtonDown(SMSGHANDLER_PARAMS *pMsg);
void __stdcall GAME_VanquishedOnButtonUp(SMSGHANDLER_PARAMS *pMsg);
void __stdcall GAME_VanquishedOnKeyDown(SMSGHANDLER_PARAMS *pMsg);
int32_t __stdcall GAME_ShowDiabloVanquishedMessage(int32_t nIdlePass);

//  ---- GAME_InitGame's callees, first tier ----

//  D2Client.0x6FB3F330, Monster.cpp -- memset the eight gapClientPlayers slots.
void __cdecl MONSTER_ClearPlayerSlots();

//  D2Client.0x6FB28020, CUnit.cpp -- drop both unit hash tables, 0xC00 bytes each, without freeing anything. They are contiguous in memory.
void __cdecl CUNIT_ClearUnitTables();

//  D2Client.0x6FAD2F60, chat.cpp -- empty the eight-slot chat history. Both cursors go to 7, not 0, so the first line written lands at index 0.
void __cdecl CHAT_ResetHistory();

//  D2Client.0x6FAD1790..0x6FAD1D80, chat.cpp -- the five IME handlers, reached only through gChatImeHandlers (0x6FB78320).
void __stdcall CHAT_OnImeComposition(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CHAT_OnImeStartComposition(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CHAT_OnImeEndComposition(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CHAT_OnImeNotify(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CHAT_ImeIgnoreMessage(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FAD1DA0..0x6FAD2720, chat.cpp -- the chat box's hooks, its draw and its click.
void __cdecl CHAT_RegisterInputHandlers();
void __cdecl CHAT_UnregisterHandlers();
void __cdecl CHAT_DrawInputLine();
void __fastcall CHAT_DrawImeComposition(int32_t nX, int32_t nY);
void __stdcall CHAT_OnLButtonDown(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FAD2760..0x6FAD2DB0, chat.cpp -- the history keys, the character handler, and the prefix reader the character handler calls.
void __stdcall CHAT_OnHistoryKey(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CHAT_HandleInputKey(SMSGHANDLER_PARAMS *pMsg);
BOOL __fastcall CHAT_ParseCommandPrefix(const char **ppszInput, uint8_t *pnKind, char *pszTargetName);

//  D2Client.0x6FAD2F90..0x6FAD34A0, chat.cpp -- the input line's reset, the Battle.net message pump GAME_UpdateFrame calls once a frame, and
//  the two command matchers the character handler calls.
void __cdecl CHAT_ClearInputLine();
void __cdecl CHAT_Update();
BOOL __fastcall CHAT_RunBattleNetCommand(char *pszInput, const char *pszCommand);
BOOL __fastcall CHAT_HandleWhisperCommand(char *szInput, const char *szCommand, int32_t bWhisperInGame);

//  D2Client.0x6FAB71B0, View.cpp -- move the viewport origin, and raise dwFlags bit 0 only when it actually moved.
void __fastcall VIEW_SetOrigin(D2ClientViewStrc *pView, int32_t nX, int32_t nY);

//  QUEST\Quests.cpp -- packet 0x50's dispatch on the quest id, packet 0x5E's 37 bytes and the two exports that read them (ordinals 10001 and
//  10002, `extern "C"` for D2Client.def), and packet 0x61's cinematic.
void __fastcall QUESTS_DispatchQuestPacket(D2GSPacketSrv50 *pCmd);
void __fastcall QUESTS_StoreSrv5EPayload(D2GSPacketSrv5E *pPacket);
extern "C" int32_t __stdcall QUESTS_AreStatesReceived(int32_t nUnused);
extern "C" int32_t __stdcall QUESTS_GetQuestState(int32_t nUnused, D2Quests nQuest);
void __fastcall QUESTS_PlayCinematicFromPacket(D2GSPacketSrv61 *pPacket);

//  QuestsFX.cpp -- packet 0x89's handler, the fade starter, the twenty-slot trigger table's ten handlers, two monster-death effects, and the
//  dispatcher.
void __fastcall QUESTSFX_FireTrigger(D2GSPacketSrv89 *pPacket);
void __fastcall QUESTSFX_StartScreenFade(int32_t nFadeOut, int32_t nDarkHold, int32_t nFadeIn, int32_t nLevelId, int32_t bForce);
void __cdecl QUESTSFX_TriggerCairnStones();
void __cdecl QUESTSFX_TriggerDenOfEvil();
void __cdecl QUESTSFX_TriggerHoradricStaff();
void __cdecl QUESTSFX_TriggerTaintedSun();
void __cdecl QUESTSFX_TriggerDiabloAppears();
void __cdecl QUESTSFX_TriggerHellforge();
void __cdecl QUESTSFX_ArmTriggerTimer();
void __fastcall QUESTSFX_OnOverseerDeath(D2UnitStrc *pUnit);
void __cdecl QUESTSFX_TriggerAnya();
void __cdecl QUESTSFX_TriggerNihlathak();
void __cdecl QUESTSFX_TriggerWorldstoneShake();
void __fastcall QUESTSFX_OnAncientDeath(D2UnitStrc *pUnit);
void __fastcall QUESTSFX_FireTriggerById(int32_t eTrigger);

//  QuestsFX.cpp -- the arms of MONUNIQUE_MonUModCltQuestComplete's dispatch on the quest monsters' class ids.
void __fastcall QUESTSFX_OnBloodRavenDeath(D2UnitStrc *pUnit);
void __fastcall QUESTSFX_SpawnTowerChestEffect(D2UnitStrc *pUnit);
void __fastcall QUESTSFX_OnCompellingOrbDeath(D2UnitStrc *pUnit);
void __fastcall QUESTSFX_OnMephistoDeath(D2UnitStrc *pUnit);
void __fastcall QUESTSFX_OnAndarielDeath(D2UnitStrc *pUnit);
void __fastcall QUESTSFX_OnRadamentDeath(D2UnitStrc *pUnit);
void __fastcall QUESTSFX_OnMaggotQueenDeath(D2UnitStrc *pUnit);
void __fastcall QUESTSFX_OnDurielDeath(D2UnitStrc *pUnit);
void __fastcall QUESTSFX_OnIzualDeath(D2UnitStrc *pUnit);
void __fastcall QUESTSFX_SetWatchedUnit(D2UnitStrc *pUnit);
void __cdecl QUESTSFX_OnBaalDeath();

//  QuestsFX.cpp -- Tyrael's descent, the quest light override, the Den of Evil's ambient lights, the screen fade, the Worldstone shake, the
//  game-end countdown and its packet-armed deadline. OnRoomActivated and SpawnAmbientMissiles are `__thiscall` in the original, `this` only.
void __fastcall QUESTSFX_OnTyrael3Created(D2UnitStrc *pMonster);
int32_t __fastcall QUESTSFX_GetQuestLightOverride(D2ActiveRoomStrc *pRoom, uint8_t *pnRed, uint8_t *pnGreen, uint8_t *pnBlue);
void __fastcall QUESTSFX_OnRoomActivated(D2ActiveRoomStrc *pRoom);
void __fastcall QUESTSFX_SpawnAmbientMissiles(D2ActiveRoomStrc *pRoom);
void __cdecl QUESTSFX_TickScreenFade();
void __fastcall QUESTSFX_SpawnWorldstoneShake(D2ActiveRoomStrc *pRoom);
void __cdecl QUESTSFX_DrawGameEndCountdown();
void __fastcall QUESTSFX_ArmGameEndDeadline(int32_t nDelaySeconds);

//  D2Client.0x6FABA580, QuestsFX.cpp -- clear the quest-effect state: two sentinels to -1, eleven dwords and four flags to 0.
void __cdecl QUESTSFX_Reset();

//  D2Client.0x6FB09C90, minipanel.cpp -- read "Diablo II\Resolution" and apply it. The value slot is deliberately left uninitialised, as the
//  original leaves it.
void __cdecl MINIPANEL_LoadResolutionFromRegistry();

//  D2Client.0x6FB099F0 / 0x6FB09730 / 0x6FB098D0 / 0x6FB09C50 / 0x6FB09B80 / 0x6FB09BF0, minipanel.cpp -- gMinipanelMessageHandlers, the action
//  it runs, and the panel's load and unload. The press arms a button and the release acts; MINIPANEL_IsPointOverMiniPanelRow tests a
//  different rectangle from the one the two handlers use, and applies none of the three placement offsets.
void __stdcall MINIPANEL_OnLButtonDown(SMSGHANDLER_PARAMS *pMsg);
void __stdcall MINIPANEL_OnLButtonUp(SMSGHANDLER_PARAMS *pMsg);
void __fastcall MINIPANEL_RunButtonAction(int32_t nButton);
BOOL __fastcall MINIPANEL_IsPointOverMiniPanelRow(int32_t nXPos, int32_t nYPos);
void __cdecl MINIPANEL_LoadImages();
void __cdecl MINIPANEL_SaveAndUnload();

//  D2Client.0x6FB08D90 / 0x6FB08DC0 / 0x6FB09180 / 0x6FB09270 / 0x6FB092B0 / 0x6FB09390 / 0x6FB09CC0, minipanel.cpp -- "Always Run" loaded
//  (the tail jump that ends UI_InitUiForPlayerUnit) and saved (by UI_ResetUiSubsystems); the button tables, the bitmap and the buttons under
//  MINIPANEL_Draw; and the waypoint panel's test for the mini panel, whose box is not the one the buttons answer in.
void __cdecl MINIPANEL_LoadAlwaysRunFromRegistry();
int32_t __cdecl MINIPANEL_SaveAlwaysRunToRegistry();
void __fastcall MINIPANEL_DrawBackgroundSegment(int32_t nSegment);
void __fastcall MINIPANEL_DrawButtonCel(int32_t nFrame, int32_t nXPos, int32_t nYPos, int32_t nUnused);
void __fastcall MINIPANEL_LayOutButtons(int32_t nSegment);
void __fastcall MINIPANEL_DrawButtons(int32_t nSegment);
BOOL __cdecl MINIPANEL_WaypointIsMouseOverMiniPanel();

//  D2Client.0x6FB08DE0 / 0x6FB09460, minipanel.cpp -- the panel drawn in one of three layouts or hidden, by which windows are open; and the
//  label of the button under the cursor, with the keys bound to its command.
void __cdecl MINIPANEL_Draw();
void __fastcall MINIPANEL_DrawButtonHint(int32_t nLayout);

//  D2Client.0x6FB09D80 / 0x6FB09F60, UI\showitems.cpp -- the ground-item labels. The fitter places one label clear of the ones already placed
//  and answers 0 when it had to move it, so the caller loops; the drawer walks the player's room and its neighbours, gates on dwFlagEx bit 7,
//  builds each label's text and colour, and draws the group from each record's LEFT and BOTTOM. The array is gShowItemLabels, 32 records
//  of 288 bytes, and the count is gnNumShowItems.
int32_t __fastcall SHOWITEMS_FitLabelRect(int32_t *pnLeft, int32_t *pnTop, int32_t *pnRight, int32_t *pnBottom, int32_t nAnchor, int32_t nMaxRight, int32_t nMinLeft);
void __cdecl SHOWITEMS_DrawGroundItemLabels(void);

//  D2Client.0x6FAD1290 / 0x6FAD1470, UI/char.cpp -- gStatsMessageHandlers. The character screen's button class and text helpers come first,
//  then the panel's draw and its mouse handlers. The stat buttons are four records of fourteen bytes; shift-click spends every point in chunks
//  of 32. The six `nEdxUnused` members are `__thiscall` in the original.
int32_t *__fastcall CHAR_UiButtonConstruct(D2ClientButtonStrc *pButton, int32_t nEdxUnused, int32_t nX, int32_t nY, int32_t nImageIndex, void *pfnOnClick);  // 0x6FACF8D0
D2ClientButtonStrc *__fastcall CHAR_UiButtonVecDelDtor(D2ClientButtonStrc *pButton, int32_t nEdxUnused, uint8_t bFreeObject); // 0x6FACF990
void __fastcall CHAR_UiButtonDestruct(D2ClientButtonStrc *pButton);                    // 0x6FACF9C0
void __fastcall CHAR_UiButtonDraw(D2ClientButtonStrc *pButton);                        // 0x6FACF9D0
int32_t __fastcall CHAR_UiButtonOnMouseDown(D2ClientButtonStrc *pButton, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);  // 0x6FACFAD0
int32_t __fastcall CHAR_UiButtonOnMouseUp(D2ClientButtonStrc *pButton, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);  // 0x6FACFB30
int32_t __fastcall CHAR_UiButtonOnChar(D2ClientButtonStrc *pButton, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);  // 0x6FACFB90
void __fastcall CHAR_UiButtonProxyDestruct(int32_t *pProxy);                               // 0x6FACFBC0
uint32_t **__fastcall CHAR_UiButtonProxyVecDelDtor(uint32_t **pProxy, int32_t nEdxUnused, uint8_t bFreeObject); // 0x6FACFC20
int32_t *__fastcall CHAR_UiButtonProxyConstruct(int32_t *pProxy, int32_t nEdxUnused, int32_t nX, int32_t nY, int32_t nImageIndex, void *pfnOnClick);  // 0x6FACFC50
void __fastcall CHAR_DrawTextCentered(int32_t nLeft, int32_t nY, int32_t nRight, const Unicode *wszText, int32_t nColor);  // 0x6FACFCD0
int32_t __fastcall CHAR_TextCutNextLine(Unicode *pText, Unicode **ppRest);                 // 0x6FACFD10
void __stdcall CHAR_StatsOnLButtonDown(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CHAR_StatsOnLButtonUp(SMSGHANDLER_PARAMS *pMsg);
void __cdecl CHAR_CharPanelDraw();                                            // 0x6FACFD60
int32_t __fastcall CHAR_CharPanelGetWeaponBlockPercent(D2UnitStrc *pUnit);        // 0x6FAD1010
int32_t __fastcall CHAR_CharPanelChanceToHitPercent(int32_t nAttackRating);           // 0x6FAD10D0
void __cdecl CHAR_MarkGoldDialogClosed();                                   // 0x6FAD16C0
void __cdecl CHAR_MarkGoldDialogOpen();                                     // 0x6FAD16D0
void __cdecl CHAR_CharPanelMeasureLabelWidths();                              // 0x6FAD16E0

//  D2Client.0x6FB06320 / 0x6FB06480, UI/QuestLog.cpp -- gQuestlogMessageHandlers: four records over two functions; both mouse buttons go to
//  the same pair. The close box moves four ways, indexed by the UI shift and the resolution together.
void __stdcall QUESTLOG_OnMouseDown(SMSGHANDLER_PARAMS *pMsg);
void __stdcall QUESTLOG_OnMouseUp(SMSGHANDLER_PARAMS *pMsg);

//  ---- GAME_InitGame's callees, second tier ----

//  D2Client.0x6FAA6A10, GAME\Env.cpp -- create the three weather slot pools and prime the wind. ENV_Shutdown at 0x6FAA6AB0 is the other half.
void __cdecl ENV_Init();

//  D2Client.0x6FAB6FF0, GAME\View.cpp -- allocate the client's one viewport, 60092 bytes, after asserting the three gouraud tables exist.
D2ClientViewStrc *__cdecl VIEW_Alloc();

//  D2Client.0x6FAB5760, GAME\scroll.cpp -- slide the world sideways so an open panel does not cover the player. Declared `__fastcall`: the
//  original's `__thiscall` is the same ABI for a single register argument.
void __fastcall SCROLL_SetWindowMode(D2C_UIpanels nPanel);

//  D2Client.0x6FAB5750 and 0x6FAB5870..0x6FAB5A15, GAME\scroll.cpp -- the panel-mode getter, the viewport origin's four getters, the
//  click-to-subtile conversion and the per-frame origin update.
D2C_UIpanels __cdecl SCROLL_GetUiShiftScreen();
int32_t __cdecl SCROLL_GetViewportOriginXRaw();
int32_t __cdecl SCROLL_GetViewportOriginYRaw();
int32_t __cdecl SCROLL_GetViewportOriginX();
int32_t __cdecl SCROLL_GetViewportOriginY();
void __fastcall SCROLL_ScreenToGameCoords(int32_t *pnX, int32_t *pnY);
void __cdecl SCROLL_UpdateViewportOrigin();

//  D2Client.0x6FAB5A20..0x6FAB5D5A, GAME\Select.cpp -- the selection's getter, setter, worker, clear and unlock, and the character panel's two
//  reference monsters.
D2UnitStrc *__cdecl SELECT_GetSelectedUnit();
int32_t __cdecl SELECT_GetToHitRefMonster();
int32_t __cdecl SELECT_GetToBeHitRefMonster();
void __fastcall SELECT_SetSelectedUnit(D2UnitStrc *pUnit);
void __fastcall SELECT_ApplySelectedUnit(D2UnitStrc *pUnit);
int32_t __cdecl SELECT_ClearSelectedUnit();
void __cdecl SELECT_UnlockSelection();

//  D2Client.0x6FAB5D60, GAME\Select.cpp -- the per-frame scan for the unit under the cursor.
void __cdecl SELECT_UpdateSelectedUnit();

//  D2Client.0x6FAB6430..0x6FAB687F, GAME\Select.cpp -- a pet's display state, the click region, the skill target resolver and Iron Golem's
//  find test.
void __fastcall SELECT_RosterPetsSetHirelingDisplayState(D2UnitStrc *pOwner, D2UnitStrc *pPet, int32_t nState);
int32_t __fastcall SELECT_SetClickRegion(const int32_t *pRect);
D2UnitStrc *__fastcall SELECT_SkillsResolveClientTarget(D2SkillStrc *pSkill, D2UnitStrc *pTargetUnit, int32_t *pnX, int32_t *pnY, uint8_t nCastFlags, int32_t bAllowSearch);
BOOL __fastcall SELECT_SkillsIsIronGolemTarget(D2UnitStrc *pUnit, D2UnitFindArgStrc *pUnitFindArg);

//  D2Client.0x6FAB6880..0x6FAB6AB9, GAME\Select.cpp -- the enemy probe in front of a unit, its unit-find test, and the reference monsters'
//  reset. D2Common's UNITS_GetXPosition/YPosition pair, emitted out of line after them, is declared in the Shared.cpp section.
D2UnitStrc *__fastcall SELECT_SkillsFindEnemyInFrontOfUnit(D2UnitStrc *pUnit, D2SkillsTxt *pSkillsTxt);
int32_t __fastcall SELECT_UnitFindIsMonster(D2UnitStrc *pUnit, void *pContext);
void __cdecl SELECT_ResetRefMonsters();

//  D2Client.0x6FAADEA0, GAME\PalShift.cpp -- build the palette-shift tables: eight bytes per monstats.txt row, RandTransforms.dat, GreenBlood.dat.
void __cdecl PALSHIFT_Init();

//  D2Client.0x6FAADFB0..0x6FAAE5AD, GAME\PalShift.cpp -- the free at a game's end, the per-class palshift.dat cache and its green-blood copy,
//  the table a monster is drawn through, a unique monster's shift roll, and D2Common's two monstats header inlines emitted out of line
//  (the first, DATATBLS_GetMonStatsTxtRecord, is in the Shared.cpp section).
void __cdecl PALSHIFT_Shutdown();
void __fastcall PALSHIFT_EnsureMonsterPalette(D2C_MonsterIds nMonsterClassId);
D2ClientPalShiftStrc *__fastcall PALSHIFT_FindCacheEntry(D2C_MonsterIds nMonsterClassId);
void __fastcall PALSHIFT_ApplyGreenBlood(D2C_MonsterIds nMonsterClassId, D2ClientPalShiftStrc *pPalShift);
uint8_t *__fastcall PALSHIFT_GetUnitShiftTable(D2UnitStrc *pUnit);
uint32_t __fastcall PALSHIFT_RollUniqueShift(D2UnitStrc *pUnit, uint16_t nOffset);
uint8_t *__cdecl PALSHIFT_GetGreenBloodRemap();
int32_t __cdecl PALSHIFT_GetMonStatsRecordCount();

//  D2Client.0x6FAAE5B0..0x6FAAEC7D, GAME\Record.cpp -- the demo recorder and player behind the `-r` and `-p` switches. Window messages are
//  logged into a chain of 500-record holders, written to Record.dr1 after a game, and replayed frame by frame.
void __fastcall RECORD_LogEvent(uint32_t nEventId, WPARAM wParam, LPARAM lParam);
void __cdecl RECORD_SaveToFile();
void __cdecl RECORD_FreeBufferList();
void __cdecl RECORD_Play();
int32_t __cdecl RECORD_GetRecordingRequested();
int32_t __stdcall RECORD_CleanupHandler(int32_t nArg1, int32_t nArg2, int32_t nArg3);
void __cdecl RECORD_InstallSaveOnCleanup();
void __cdecl RECORD_SetRecordPlaybackLoaded();
int32_t __cdecl RECORD_GetPlaybackRequested();
int32_t __cdecl RECORD_GetRecordPlaybackLoaded();
void __cdecl RECORD_RequestRecording();
void __cdecl RECORD_ReadNextForFrame();

//  D2Client.0x6FB241A0, UI\ui.cpp -- tear down the previous loading screen and put up a new one. The teardown branch, where per-act resources
//  are swapped, is skipped the first time through.
void __cdecl UI_LoadScreenBegin();

//  D2Client.0x6FAA13A0, Core\MemoryPartitions.cpp -- divide physical memory between the sound, tile and sprite caches, and halt on a machine
//  with less than 22 MB + 512000.
void __fastcall MEMPART_InitMemoryPartitions(int32_t hPool, int32_t bInitTileCache);

//  D2Client.0x6FAA14E0 and 0x6FAA1500, Core\MemoryPartitions.cpp -- the three caches' teardown, and the one-byte `retn` fifteen sites call.
void __fastcall MEMPART_DestroyCaches(BOOL bFreeLRU);
void __cdecl MEMPART_Nullsub1();

//  D2Client.0x6FAA1510 and 0x6FAA1CA0..0x6FAA1CF5, Core\Video.cpp -- the registry progress mask and the four cinematic-state one-liners.
void __fastcall VIDEO_RecordCinematicProgress(int32_t nCinematicIndex);
void __cdecl VIDEO_SetGameCompletedClassic();
void __cdecl VIDEO_ResetCinematicState();
void __cdecl VIDEO_SetActVIntroPending();
uint8_t __cdecl VIDEO_IsCinematicPlaying();

//  D2Client.0x6FAA1680, Core\Video.cpp -- the cinematic player.
void __fastcall VIDEO_PlayCinematic(int32_t nCinematicId);

//  D2Client.0x6FAA2860..0x6FAA2E21, Draw.cpp (no assert names the file; the name is ours) -- the gamma, the renderer choice and the /framerate
//  overlay.
void __fastcall DRAW_SetGamma(uint32_t nGamma);
void __cdecl DRAW_SelectGdiDisplay();
void __cdecl DRAW_SetDisplayTypeGlide();
void __cdecl DRAW_SetDisplayTypeOpenGL();
void __cdecl DRAW_SetDisplayTypeDirect3D();
void __cdecl DRAW_SetDisplayTypeRave();
int32_t __cdecl DRAW_GetDisplayType();
BOOL __cdecl DRAW_IsDisplayType3D();
void __cdecl DRAW_GameDrawDebugOverlay();
void __cdecl DRAW_GameToggleDebugOverlay();
void __cdecl DRAW_ToggleDebugOverlay();

//  D2Client.0x6FAA2E60..0x6FAA4277, Draw\dFloor.cpp -- the context wrapper, the edge tile (`__thiscall` in the original: pCtx in ecx, an unread
//  edx, the rest on the stack), the three light grids and the distance estimate placed here by position.
void __fastcall DFLOOR_DrawFloors(D2ClientViewStrc *pView);
//  The tile, `__thiscall` in the original like the edge tile.
void __fastcall DFLOOR_DrawFloorTile(D2FloorDrawCtxStrc *pCtx, int32_t nEdxUnused, D2DrlgTileDataStrc *pTile, BOOL bFloorFilter, int32_t nXPos, int32_t nYPos, int32_t nWorldX, int32_t nWorldY);
void __fastcall DFLOOR_DrawEdgeTile(D2FloorDrawCtxStrc *pCtx, int32_t nEdxUnused, int32_t nWorldX, int32_t nWorldY);
void __fastcall DFLOOR_FillLightGrid8x8(D2ClientFloorVertexStrc *pVertices, int32_t nSubtileX, int32_t nSubtileY, int32_t bFlatAmbient);
void __fastcall DFLOOR_FillEdgeLightGrid6x6(D2ClientFloorVertexStrc *pVertices, int32_t nScreenX, int32_t nScreenY, int32_t nSubtileX, int32_t nSubtileY, int32_t bFlatAmbient, int32_t nSideShift, int32_t bCornersOnly);
void __fastcall DFLOOR_BuildTileLightGrid(D2ClientFloorVertexStrc *pVertices, int32_t nTileX, int32_t nTileY, int32_t nBais, int32_t nHalfShift, int32_t bCornersOnly);
uint32_t __fastcall DFLOOR_APPROX_Distance(int32_t nDX, int32_t nDY);
//  The view-grid pass and its fades.
void __fastcall DFLOOR_DrawViewGridFloors(D2ClientViewStrc *pView);
//  The room pass, `__thiscall` in the original.
void __fastcall DFLOOR_DrawRoomFloors(D2FloorDrawCtxStrc *pCtx, int32_t nEdxUnused, D2ClientViewStrc *pView);

//  ---- the window: WNDPROC.CPP and WINMAIN.CPP ----

//  D2Client.0x6FAA23B0..0x6FAA2549, CORE\WNDPROC.CPP -- the resolution switch and the six handlers of sgWndProcHandlers, a table nothing
//  registers.
void __fastcall WNDPROC_SetResolutionMode(D2GameResolutionMode nMode);
void __stdcall WNDPROC_OnDestroy(SMSGHANDLER_PARAMS *pMsg);
void __stdcall WNDPROC_OnActivateApp(SMSGHANDLER_PARAMS *pMsg);
void __stdcall WNDPROC_OnDefault(SMSGHANDLER_PARAMS *pMsg);
void __stdcall WNDPROC_RedirectToClose(SMSGHANDLER_PARAMS *pMsg);
void __stdcall WNDPROC_OnIgnoredMessage(SMSGHANDLER_PARAMS *pMsg);
void __stdcall WNDPROC_RedirectToUser(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FAA2550, CORE\WNDPROC.CPP -- the window procedure. WINMAIN_D2ClientEntrypoint hands it to D2Win.
LRESULT __stdcall WNDPROC_WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

//  D2Client.0x6FAA26C0, CORE\WNDPROC.CPP -- register a 12-byte-per-record table of message handlers with Storm. The four-way kind switch is
//  an inlined singular.
void __fastcall WNDPROC_RegisterMessageHandlers(HWND hWnd, const D2WinCommandRegisterStrc* pCallbacks, size_t nCallbacks);

//  D2Client.0x6FAA2780, CORE\WNDPROC.CPP -- the exact mirror, arm for arm.
void __fastcall WNDPROC_UnregisterMessageHandlers(HWND hWnd, const D2WinCommandRegisterStrc* pCallbacks, size_t nCallbacks);

//  D2Client.0x6FAA2840, CORE\WNDPROC.CPP -- save Storm's message-registration state for the main window, so a game's handlers can be dropped
//  wholesale.
void __cdecl WNDPROC_PushStormMessageState();

//  D2Client.0x6FAA2850, CORE\WNDPROC.CPP -- the counterpart, called from GAME_AppModeCloseWindow.
void __cdecl WNDPROC_PopStormMessageState();

//  D2Client.0x6FAA1D00, CORE\WINMAIN.CPP -- push an app mode onto the pending queue. The queue's five-slot array and three-slot shift are an
//  off-by-one in the original that cannot be reached.
void __fastcall WINMAIN_AppModeEnqueue(D2C_AppMode nAppMode);

//  D2Client.0x6FAA1D40, CORE\WINMAIN.CPP -- take the front of that queue. Both users reach it by tail jump, so the popped mode becomes their
//  return.
D2C_AppMode __cdecl WINMAIN_AppModeDequeue();

//  D2Client.0x6FAA1DA0..0x6FAA1EA0, CORE\WINMAIN.CPP -- the six command-line switch handlers, reached only through WINMAIN.cpp's own
//  gaCommandLineOptions, which Storm is handed. Each has the table's signature.
int32_t __stdcall WINMAIN_CmdArgDisplayHelp(void *pParam, int32_t nUnused);
int32_t __stdcall WINMAIN_CmdArgPreCache(void *pParam, int32_t nUnused);
int32_t __stdcall WINMAIN_CmdArgRecord(void *pParam, int32_t nUnused);
int32_t __stdcall WINMAIN_CmdArgPlay(void *pParam, int32_t nUnused);
int32_t __stdcall WINMAIN_CmdArgSet(void *pParam, int32_t nUnused);
int32_t __stdcall WINMAIN_CmdArgMode(void *pParam, int32_t nUnused);

//  D2Client.0x6FAA1F50 -- ordinal export #10003, exported by D2Client.def.
extern "C" void __cdecl WINMAIN_DeactivateApp();

//  D2Client.0x6FAA1F70..0x6FAA2030, CORE\WINMAIN.CPP -- the launch configuration's accessors: gpLaunchConfig itself, its BnClient interface,
//  the game name, password and server address (NULL when absent or empty), the hardcore bit, and the dead bit's setter.
D2ConfigStrc *__cdecl WINMAIN_GetConfig();
struct BnClientInterface *__cdecl WINMAIN_GetBnClientInterface();
char *__cdecl WINMAIN_GetConfigGameName();
char *__cdecl WINMAIN_GetConfigGamePassword();
char *__cdecl WINMAIN_GetConfigServerIP();
int32_t __cdecl WINMAIN_IsHardcoreGame();
void __cdecl WINMAIN_MarkCharacterDead();

//  D2Client.0x6FAA2040, CORE\WINMAIN.CPP -- set gbAppModeContinue, which decides what WINMAIN_D2ClientEntrypoint returns once the dispatch loop
//  ends.
void __fastcall WINMAIN_SetAppModeContinue(BOOL bContinue);

//  D2Client.0x6FAA22D0 and 0x6FAA2340, CORE\WINMAIN.CPP -- the two callbacks WINMAIN_D2ClientEntrypoint hands SCmdProcess: Storm's
//  extra-argument callback and its error callback. The same help-box block; the first answers 0, the second whatever eax holds at its exit.
int32_t __stdcall WINMAIN_SCmdDisplayHelpOnExtraArg(int32_t nCmdArg);
int32_t __stdcall WINMAIN_SCmdDisplayHelp(int32_t nCmdArg);

//  ---- the View unit: GAME\View.cpp, with WALL2_WallInitSquares from GAME\Wall2.cpp ----

//  D2Client.0x6FAB6AC0, GAME\View.cpp -- build the five gouraud tables. Two hold palette-row pointers and three hold shade bytes; two of the
//  five are never read by anything.
void __cdecl VIEW_AllocGouraudTables();

//  D2Client.0x6FAB6EB0, GAME\View.cpp -- release those five, the screen-shake sound handle and the two parallax images. Gated on the first
//  table alone.
void __cdecl VIEW_FreeGouraudTables();

//  D2Client.0x6FAB70D0, GAME\View.cpp -- free the draw grid, then the view.
void __fastcall VIEW_Free(D2ClientViewStrc *pView);

//  D2Client.0x6FAB70F0, GAME\View.cpp -- raise dwFlags bit 0.
void __fastcall VIEW_MarkDirty(D2ClientViewStrc *pView);

//  D2Client.0x6FAB7100, GAME\View.cpp -- set the view rect and rebuild the cull rect, which does not depend on it. Null-tolerant, unlike VIEW_Draw.
void __fastcall VIEW_SetRect(D2ClientViewStrc *pView, int32_t xLeft, int32_t yTop, int32_t xRight, int32_t yBottom);

//  D2Client.0x6FAB7180, GAME\View.cpp -- copy the view rect out as four absolute edges.
void __fastcall VIEW_GetRect(D2ClientViewStrc *pView, int32_t *pnLeft, int32_t *pnTop, int32_t *pnRight, int32_t *pnBottom);

//  D2Client.0x6FAB71E0, GAME\View.cpp -- read the origin back.
void __fastcall VIEW_GetOrigin(D2ClientViewStrc *pView, int32_t *pnX, int32_t *pnY);

//  D2Client.0x6FAB7200, GAME\View.cpp -- draw the world and clear the redraw bit, which it clears even on the frame it declines to draw.
void __fastcall VIEW_Draw(D2ClientViewStrc *pView, int32_t bSkipUnits);

//  D2Client.0x6FAB7F00, GAME\Wall2.cpp -- zero the four draw-grid fields of a freshly allocated view, and deliberately not the fifth.
void __fastcall WALL2_WallInitSquares(D2ClientViewStrc *pView);

//  D2Client.0x6FAB7F20, GAME\Wall2.cpp -- free the draw grid and clear the same four fields, leaving nSquareCount set; safe because the build
//  tests the pointer before the count.
void __fastcall WALL2_WallFreeSquares(D2ClientViewStrc *pView);

//  D2Client.0x6FAB7F60, GAME\Wall2.cpp -- rebuild the per-frame draw grid and walk the player's rooms through WALL2_WallAddRoomToSquareGrid,
//  which it hands over as a pointer.
void __fastcall WALL2_WallBuildSquareGrid(D2ClientViewStrc *pView, int32_t bSkipUnits);

//  D2Client.0x6FAB8290, GAME\Wall2.cpp -- fill the grid for one room: wall tiles, roof tiles, units.
//  BUG: the draw-order sort never compares the last node in a list, and a node cursor is incremented before its bound is tested. Reproduced.
int32_t __stdcall WALL2_WallAddRoomToSquareGrid(D2ActiveRoomStrc *pRoom, D2ClientWallRoomArgsStrc *pArgs);

//  GAME\Wall2.cpp -- the three after WALL2_WallAddRoomToSquareGrid, all called only by it: which way a wall tile fades, and the two appenders
//  of a square's shadow list. pContext is &pView->dwLayerMask.
BOOL __fastcall WALL2_WallIsPlayerBehindTile(D2ActiveRoomStrc *pRoom, D2DrlgTileDataStrc *pTile);
void __fastcall WALL2_WallAppendTileToShadowList(uint32_t *pContext, D2ClientWallSquareStrc *pSquare, D2DrlgTileDataStrc *pTile, D2ActiveRoomStrc *pRoom);
void __fastcall WALL2_WallAppendUnitToShadowList(uint32_t *pContext, D2ClientWallSquareStrc *pSquare, D2UnitStrc *pUnit);

//  D2Client.0x6FAA6340, Draw\dWall.cpp -- draw every square's wall list and then its unit list. No branch: every node on pUnitList is a unit.
void __fastcall DWALL_EnvDrawCellUnits(D2ClientViewStrc *pView);

//  D2Client.0x6FAA63E0, Draw\dWall.cpp -- draw one square's wall tiles and run the 500 ms transparency fade, which only advances under 3D
//  acceleration.
void __fastcall DWALL_DrawRoomWalls(D2ClientWallSquareStrc *pRoom, uint32_t fOnlyDrawShadowLayer, D2ClientViewStrc *pView);

//  D2Client.0x6FAA6670, Draw\dWall.cpp -- draw the tile layer of every square, per-corner lit. Carries the timed expiry but no fade ramp.
void __fastcall DWALL_EnvDrawRoomTilesLit(D2ClientViewStrc *pView);

//  D2Client.0x6FAA6860, Draw\dWall.cpp -- walk each square's shadow list and dispatch on the node's own bits: tile shadow, unit shadow, or a
//  lit unit. Draws the wall list after the shadows, where DWALL_EnvDrawCellUnits draws it before.
void __fastcall DWALL_EnvDrawCellShadowsAndUnits(D2ClientViewStrc *pView);

//  D2Client.0x6FAA6990..0x6FAA6A0B, arena.cpp (no assert names the file; the name is ours) -- the accessors of gdwArenaFlags and
//  gnArenaPlayerClass.
BOOL __cdecl ARENA_IsArenaMode();
int32_t __cdecl ARENA_GetArenaFlags();
void __fastcall ARENA_SetArenaFlags(uint32_t dwArenaFlags);
void __fastcall ARENA_SetArenaFlag0(int32_t bClear);
void __fastcall ARENA_SetArenaPlayerClass(int32_t nPlayerClass);
int32_t __cdecl ARENA_GetArenaPlayerClass();
BOOL __cdecl ARENA_IsArenaPlayerClassForced();

//  D2Client.0x6FAB7260 -- the per-frame view maintenance and the two level ambient effects, then the world draw. Sole caller of the three
//  ENV_Draw* passes in dWall.cpp. The binary does not name its translation unit; ViewEffects.cpp is our name.
void __fastcall VIEWEFFECTS_EnvUpdateViewAndLevelEffects(D2ClientViewStrc *pView);

//  D2Client.0x6FAB7590 -- place one arcane star: position, leftward velocity and colour, four rolls out of the environment seed.
void __fastcall VIEWEFFECTS_EnvRespawnArcaneStar(int32_t nIndex, int32_t bAtRightEdge);

//  D2Client.0x6FAB76F0 -- the Rocky Summit parallax: four tiled backdrop rows and ten drifting clouds. Carries a dead origin latch, reproduced.
void __fastcall VIEWEFFECTS_EnvDrawParallax(D2UnitStrc *pUnit);

//  D2Client.0x6FAB7C50 -- one frame of screen shake, added into the caller's camera offset.
void __fastcall VIEWEFFECTS_UpdateScreenShake(int32_t *pViewOffsetX, int32_t *pViewOffsetY);

//  D2Client.0x6FAB7DC0 -- ramp the rumble sound toward the shake's magnitude, at most six volume steps per call.
void __fastcall VIEWEFFECTS_UpdateScreenShakeSound(int32_t nMagnitude);

//  D2Client.0x6FAB7E70 -- arm a shake. A zero hold is a no-op.
void __fastcall VIEWEFFECTS_StartScreenShake(int32_t nMagnitude, int32_t nRampUpMs, int32_t nHoldMs, int32_t nRampDownMs);

//  D2Client.0x6FAB7EB0 -- read the current offset, returning 0 and touching nothing when no shake is running.
int32_t __fastcall VIEWEFFECTS_GetScreenShakeOffset(int32_t *pOffsetX, int32_t *pOffsetY);

//  D2Client.0x6FAB7EE0 -- cancel a shake without stopping its sound.
void __cdecl VIEWEFFECTS_ResetScreenShake();

//  D2Client.0x6FAA6AB0, GAME\Env.cpp -- free the three weather pools.
void __cdecl ENV_Shutdown();

//  D2Client.0x6FAA6AE0, GAME\Env.cpp -- set the weather state 0..3. One of its two gates is dead: gbEnvRainStateLocked is never written anywhere.
void __fastcall ENV_SetRainState(uint32_t eState);

//  D2Client.0x6FAA6D40, GAME\Env.cpp -- reset the weather and decide whether this level gets any. The parameter is an int flag.
int32_t __fastcall ENV_ResetWeather(int32_t bForce);

//  D2Client.0x6FAA7050, GAME\Env.cpp -- build the five colour ramps and load the eight rain and bubble sprites.
void __fastcall ENV_LoadImages(D2C_Levels nLevelId);

//  D2Client.0x6FAA7590, GAME\Env.cpp -- roll one rain splash. Triangular scatter in pixels without perspective, 16.16 fixed point with it.
void __fastcall ENV_SpawnSplash(int32_t nX, int32_t nY, int32_t nPerspX, int32_t nPerspY);

//  D2Client.0x6FAA7760, GAME\Env.cpp -- roll one mud bubble. The near-twin of the splash, differing in four measurable ways.
void __fastcall ENV_SpawnBubble(int32_t nX, int32_t nY, int32_t nPerspX, int32_t nPerspY);

//  D2Client.0x6FAA7D70, GAME\Env.cpp -- roll one weather particle, rain or snow.
void __fastcall ENV_SpawnParticle(D2SeedStrc *pSeed);

//  D2Client.0x6FAA7440, GAME\Env.cpp -- release the eight weather sprites.
void __cdecl ENV_UnloadImages();

//  D2Client.0x6FAA7490, GAME\Env.cpp -- drop the weather in flight when the player warps. Its `void *` parameter is kept.
int32_t __fastcall ENV_OnLevelChange(void *pParam);

//  D2Client.0x6FAA74C0, GAME\Env.cpp -- arm one lightning flash. Its one caller is Thunder Storm's client effect, and it always passes 0.
int32_t __fastcall ENV_TriggerLightningFlash(int32_t bPlayThunder);

//  D2Client.0x6FAA74F0, GAME\Env.cpp -- advance every mud bubble a frame.
void __cdecl ENV_UpdateBubbles();

//  D2Client.0x6FAA7900, GAME\Env.cpp -- does the player's level have Mud set. It fetches Rain too and throws it away.
int32_t __cdecl ENV_IsPlayerLevelMuddy();

//  D2Client.0x6FAA80C0, GAME\Env.cpp -- top up the particle pool, ease the wind toward its target, and count the thunder timer down.
void __fastcall ENV_UpdateWeatherIntensity(D2SeedStrc *pSeed);

//  D2Client.0x6FAA7940, GAME\Env.cpp -- the per-frame weather tick and its four-phase state machine.
void __cdecl ENV_UpdateWeather();

//  D2Client.0x6FAA8240, GAME\Env.cpp -- draw the lightning flash and fire the thunderclap.
void __cdecl ENV_UpdateLightningAndThunder();

//  D2Client.0x6FAA8690, GAME\Env.cpp -- draw one pool of splashes or bubbles. Under perspective an off-screen or un-drawn particle loses its
//  slot; the flat path keeps it.
void __fastcall ENV_DrawParticleArray(D2DataArrayStrc *pArray, D2CellFileStrc **pImageArray);

//  D2Client.0x6FAA8830, GAME\Env.cpp -- draw both small pools.
void __cdecl ENV_DrawSplashesAndBubbles();

//  D2Client.0x6FAA8870, GAME\Env.cpp -- should the rain be drawn.
BOOL __cdecl ENV_ShouldDrawRain();

//  D2Client.0x6FAA88B0, GAME\Env.cpp -- read the weather intensity back.
double __cdecl ENV_GetWeatherParticleIntensity();

//  D2Client.0x6FAA88C0, GAME\Env.cpp -- store whether the player's room is drawn with line-of-sight culling; nothing reads it.
void __fastcall ENV_SetPlayerRoomLosDraw(int32_t bLosDraw);

//  D2Client.0x6FAA4280, Draw\dLightMap.cpp -- allocate a light, fill it and push it at the head of gpDynamicLights. NULL when nRadiusSubtiles <= 0.
D2ClientLightStrc *__fastcall DLIGHTMAP_AddLight(D2UnitStrc *pUnit, int32_t nType, int32_t nRadiusSubtiles, int32_t nIntensity, int32_t nRed, int32_t nGreen, int32_t nBlue);

//  D2Client.0x6FAA43A0, Draw\dLightMap.cpp -- move the target radius only; the live radius eases toward it in DLIGHTMAP_Update.
void __fastcall DLIGHTMAP_SetTargetRadius(D2ClientLightStrc *ptLight, int32_t nRadiusSubtiles);

//  D2Client.0x6FAA43E0, Draw\dLightMap.cpp -- set both radii at once; a static light's baked map is freed and invalidated.
void __fastcall DLIGHTMAP_SetRadius(D2ClientLightStrc *ptLight, int32_t nRadiusSubtiles);

//  D2Client.0x6FAA4460, Draw\dLightMap.cpp -- the live radius, back in subtiles.
int32_t __fastcall DLIGHTMAP_GetRadius(D2ClientLightStrc *ptLight);

//  D2Client.0x6FAA4490, Draw\dLightMap.cpp -- nRed/nGreen/nBlue; intensity untouched.
void __fastcall DLIGHTMAP_SetColor(D2ClientLightStrc *ptLight, uint8_t nRed, uint8_t nGreen, uint8_t nBlue);

//  D2Client.0x6FAA44D0, Draw\dLightMap.cpp -- unlink from gpDynamicLights and free the node and its baked map. Asserts when it is not on the list.
void __fastcall DLIGHTMAP_RemoveLight(D2ClientLightStrc *pLight);

//  D2Client.0x6FAA4560, Draw\dLightMap.cpp -- the owner is gone: orphan the light so DLIGHTMAP_Update shrinks it away.
void __fastcall DLIGHTMAP_ReleaseLight(D2ClientLightStrc *ptLight);

//  D2Client.0x6FAA45C0, Draw\dLightMap.cpp -- the room's ambient colour, a three-level fallback: the room's override, its own RGB, then the
//  act's environment colour -- each level tried only while the colour is black. Returns pnBlue.
uint8_t *__fastcall DLIGHTMAP_GetRoomLightColor(D2ActiveRoomStrc *pRoom, int32_t *pnIntensity, uint8_t *pnRed, uint8_t *pnGreen, uint8_t *pnBlue, int32_t bUseRoomOverride);

//  D2Client.0x6FAA46A0, Draw\dLightMap.cpp -- zero the ambient light grid. Two instructions: DLIGHTMAP_RebuildAmbientLight(bClear = 1).
void __cdecl DLIGHTMAP_ClearAmbientLight();

//  D2Client.0x6FAA46B0, Draw\dLightMap.cpp -- refill the 48x48 ambient grid with the player's room colour, then repaint each adjacent room's
//  own clipped rectangle. bClear -- or a missing player -- zeroes it instead.
void __fastcall DLIGHTMAP_RebuildAmbientLight(int32_t bClear);

//  D2Client.0x6FAA4A30, Draw\dLightMap.cpp -- one-time init: register the act callback and build the 256-entry 16.16 reciprocal table.
//  Returns the last quotient written; the one caller discards it.
int32_t __cdecl DLIGHTMAP_Init();

//  D2Client.0x6FAA4A70, Draw\dLightMap.cpp -- the 0..2 quality level, chosen from the measured frame rate and held against changes for 2000 ms.
int32_t __cdecl DLIGHTMAP_UpdateQualityLevel();

//  D2Client.0x6FAA4AF0, Draw\dLightMap.cpp -- per frame: centre the grid on the player, rebuild the ambient, mask the visible cells and update
//  every dynamic light.
void __cdecl DLIGHTMAP_UpdateForPlayer();

//  D2Client.0x6FAA4D00, Draw\dLightMap.cpp -- the per-light frame update: re-read the owner's position, ease the radius toward the target,
//  then dispatch by nType and quality -- the static map, the inline shadowed falloff, or ApplyLightSource.
void __fastcall DLIGHTMAP_Update(D2ClientLightStrc *pLight, int32_t nQualityLevel);

//  D2Client.0x6FAA51D0, Draw\dLightMap.cpp -- accumulate one light into one ambient cell: intensity saturates at 255, colour blends by weight
//  through gnReciprocal16; gbColoredLights picks colour or monochrome.
void __fastcall DLIGHTMAP_AddLightToGrid(int32_t nX, int32_t nY, int32_t nIntensity, uint8_t nRed, uint8_t nGreen, uint8_t nBlue);

//  D2Client.0x6FAA5300, Draw\dLightMap.cpp -- the cheap radial path, with the quality-0 per-unit-type caps, the octagonal distance (983, 407)
//  and a linear falloff to zero at the radius.
void __fastcall DLIGHTMAP_ApplyLightSource(D2ClientLightStrc *pLight, int32_t nQualityLevel);

//  D2Client.0x6FAA54D0, Draw\dLightMap.cpp -- fill the light's square of the 64-wide gStaticLightGrid from the 48x48 ambient grid, .nLit per
//  cell scaled by sixteen.
void __fastcall DLIGHTMAP_SeedStaticGridFromAmbient(D2ClientLightStrc *pLight);

//  D2Client.0x6FAA55D0, Draw\dLightMap.cpp -- the Bresenham setup: classify the direction from a cell toward the light into gnPropagateMode
//  and the three globals PropagateCell reads.
int32_t __fastcall DLIGHTMAP_SetupPropagation(int32_t nX, int32_t nY, D2ClientLightStrc *pLight);

//  D2Client.0x6FAA5680, Draw\dLightMap.cpp -- propagate into one cell: reads prefer gStaticLightGrid, writes go to gLightGrid, 8.8 blends for
//  the diagonals.
void __fastcall DLIGHTMAP_PropagateCell(int32_t nX, int32_t nY);

//  D2Client.0x6FAA57E0, Draw\dLightMap.cpp -- bake a static light's map: the 64x64 collision fill, the same ring walk as Update's, then the
//  square copied into pnStaticMap and fStaticValid set.
void __fastcall DLIGHTMAP_BuildStaticMap(D2ClientLightStrc *pLight);

//  D2Client.0x6FAA5BC0, Draw\dLightMap.cpp -- spend the baked map of a static light, at any quality: the octagonal falloff scaled per cell by
//  (8 - (map >> 1)) >> 3, cells >= 16 skipped before the distance is taken.
void __fastcall DLIGHTMAP_ApplyLight(D2ClientLightStrc *pLight);

//  D2Client.0x6FAA5DC0, Draw\dLightMap.cpp -- teardown: free every light and its baked map, then NULL the head.
void __cdecl DLIGHTMAP_FreeAll();

//  D2Client.0x6FAA5E10, Draw\dLightMap.cpp -- the dungeon act callback: invalidate every static light whose map the changed room touches.
void __fastcall DLIGHTMAP_ActCallback(D2ActiveRoomStrc *pRoom);

//  D2Client.0x6FAA60B0..0x6FAA633F, dLightMap.cpp by position (no file literal) -- the seven after the bracket. The grid lookup takes eighths
//  of a subtile and clamps.
void __fastcall DLIGHTMAP_GetLightAtPoint(int32_t nX, int32_t nY, uint8_t *pnIntensity, uint8_t *pnRed, uint8_t *pnGreen, uint8_t *pnBlue);
void __cdecl DLIGHTMAP_DebugDrawLightGrid();
void __cdecl DLIGHTMAP_StartWarpFlash();
void __cdecl DLIGHTMAP_DrawWarpFlash();
void __cdecl DLIGHTMAP_CancelWarpFlash();
void __cdecl DLIGHTMAP_EnvUpdateAmbientLightCache();

//  D2Client.0x6FB6B91C, D2Hell\SRC\Archive.cpp -- FOG_FOpenFile with a trace on failure. Not an import despite the FOG_ prefix. The last argument
//  suppresses the trace only for a missing file.
int32_t __fastcall ARCHIVE_OpenFileOrTrace(int32_t nUnused, const char *pszFileName, void **phFile, int32_t bQuietIfMissing);

//  D2Client.0x6FB6B958, D2Hell\SRC\Archive.cpp -- FOG_FCloseFile behind an hFile assert. hArchive is unused, here and in the whole file.
int32_t __fastcall ARCHIVE_CloseFile(void *hArchive, void *hFile);

//  D2Client.0x6FB6B97E, D2Hell\SRC\Archive.cpp -- the low dword of the file size. A zero answer kills the process.
uint32_t __fastcall ARCHIVE_GetFileSize(void *hArchive, void *hFile, uint32_t *pdwSizeHigh);

//  D2Client.0x6FB6B9ED, D2Hell\SRC\Archive.cpp -- read exactly that many bytes or die. A short read is fatal too, not a return value.
int32_t __fastcall ARCHIVE_ReadFile(void *hArchive, void *hFile, void *pBuffer, uint32_t dwBytesToRead);

//  D2Client.0x6FB6BA94, D2Hell\SRC\Archive.cpp -- open, size, allocate, read, close. The buffer is 800 bytes larger than the file.
void *__fastcall ARCHIVE_LoadFile(void *hArchive, const char *szFilename, uint32_t *pdwFileSize, const char *szSrcFile, int32_t nSrcLine);

//  D2Client.0x6FB269F0, UNIT\CUnit.cpp -- look a unit up by the server's id in gpUnitTableById. A hit whose dwUnitType disagrees traces and
//  then kills the process.
D2UnitStrc *__fastcall CUNIT_FindUnitByGuidAndType(uint32_t dwUnitGUID, D2C_UnitTypes nUnitType);

//  D2Client.0x6FB26A60, UNIT\CUnit.cpp -- the same walk over gpUnitTableByClientId, the table a unit lands in when dwFlags bit 21 is set.
D2UnitStrc *__fastcall CUNIT_FindUnitByClientIdAndType(int32_t nUnitId, D2C_UnitTypes nUnitType);

//  D2Client.0x6FB26AD0, UNIT\CUnit.cpp -- the owner of a missile or a minion, or NULL when dwFlagEx bit 10 is clear. NULL in is an assert that
//  exits. A missile owner is looked up in the client-id table, by a folded constant row and not by the unit's flags.
D2UnitStrc *__fastcall CUNIT_GetOwnerUnit(D2UnitStrc *pUnit);

//  D2Client.0x6FB26BE0, UNIT\CUnit.cpp -- drop a unit from gpUnitTableById.
void __fastcall CUNIT_RemoveUnitById(int32_t nUnitId, D2C_UnitTypes nUnitType);

//  D2Client.0x6FB26C00, UNIT\CUnit.cpp -- unlink one bucket entry and destroy it. Matches on both keys and, unlike the lookups, walks past a
//  type mismatch instead of asserting. An item also loses the cursor's hover.
void __fastcall CUNIT_UnlinkAndFreeUnit(D2UnitStrc **pTable, int32_t nBucket, int32_t nUnitId, D2C_UnitTypes nUnitType);

//  D2Client.0x6FB26C70, UNIT\CUnit.cpp -- the client-id twin of CUNIT_RemoveUnitById, identical to the byte but for the table.
void __fastcall CUNIT_RemoveUnitByClientId(int32_t nUnitId, D2C_UnitTypes nUnitType);

//  D2Client.0x6FB26C90, UNIT\CUnit.cpp -- which room holds these subtile coordinates. (0,0) answers NULL; a failure after all three search
//  steps warns and kills the process. The adjacent-room walk is unsigned.
D2ActiveRoomStrc *__fastcall CUNIT_FindRoomForCoords(int32_t nX, int32_t nY);

//  D2Client.0x6FB26DD0, UNIT\CUnit.cpp -- move a unit and do everything a teleport implies. The fourth parameter is not a room id: it gates
//  the warp flash and is forwarded to ENV_ResetWeather's bForce.
void __fastcall CUNIT_WarpUnit(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t bIsLevelChange);

//  D2Client.0x6FB27040, UNIT\CUnit.cpp -- allocate and initialise, the plain form. Its NULL test is one of the four that keep CUNIT_InitUnit's
//  own unguarded write latent.
D2UnitStrc *__fastcall CUNIT_CreateUnit(uint32_t dwUnitId, int32_t nClassId, int32_t nPosX, int32_t nPosY, D2C_UnitTypes dwUnitType, int32_t nItemMode);

//  D2Client.0x6FB27080, UNIT\CUnit.cpp -- fill a fresh unit in and dispatch on its type. A tail chunk at 0x6FB2723D lies outside the reported
//  range.
//  BUG: pUnit is tested for NULL and then written through regardless. LATENT: all four callers test the allocation.
D2UnitStrc *__fastcall CUNIT_InitUnit(D2UnitStrc *pUnit, uint32_t dwUnitId, int32_t nClassId, int32_t nPosX, int32_t nPosY, D2C_UnitTypes dwUnitType, int32_t nItemMode, uint8_t nObjectMode);

//  D2Client.0x6FB27280, UNIT\CUnit.cpp -- add, and on a clash evict the unit already holding that id and retry. Finding itself is a fatal assert.
D2UnitStrc *__fastcall CUNIT_RegisterUnit(D2UnitStrc *pUnit);

//  D2Client.0x6FB27320, UNIT\CUnit.cpp -- the bucket walk, parameterised by table and bucket. Same body and same line-88 assert as the two lookups.
D2UnitStrc *__fastcall CUNIT_FindUnitInTable(D2UnitStrc **pTable, int32_t nBucket, int32_t nUnitId, D2C_UnitTypes nUnitType);

//  D2Client.0x6FB27390, UNIT\CUnit.cpp -- insert if the id is free, 1 on success and 0 when taken. The bucket stays sorted by descending
//  dwUnitId, compared unsigned.
int32_t __fastcall CUNIT_AddUnitToTable(D2UnitStrc *pUnit);

//  D2Client.0x6FB27570, UNIT\CUnit.cpp -- the player form: allocate, init, name.
D2UnitStrc *__fastcall CUNIT_CreatePlayerUnit(uint32_t dwPlayerId, D2C_PlayerClasses nCharClass, int32_t nX, int32_t nY, char *szName);

//  D2Client.0x6FB275C0, UNIT\CUnit.cpp -- the object form: allocate, init, store pObjectData->InteractType, and run OBJECT_InitShrine for a shrine.
D2UnitStrc *__fastcall CUNIT_CreateObjectUnit(uint32_t dwUnitId, int32_t nObjectId, int32_t nPosX, int32_t nPosY, D2C_UnitTypes dwUnitType, int32_t nItemMode, uint8_t nObjectMode, uint8_t nInteractType);

//  D2Client.0x6FB27630, UNIT\CUnit.cpp -- a monster from a seven-dword assign block (packet 0xAC's, or CUNIT_CreateClientOnlyUnit's). Refreshes
//  an existing hireling instead of recreating it; picks the palette shift last. Uses CUNIT_RegisterUnit's answer untested and returns the
//  allocated unit.
D2UnitStrc *__fastcall CUNIT_CreateMonsterUnit(int32_t *pAssign, uint8_t *pComponents, uint8_t *pMonUmod, uint16_t wNameSeed, int32_t nBossHcIdx, int32_t bAsyncClientUnit);

//  D2Client.0x6FB27D50, UNIT\CUnit.cpp -- a unit the server never sees, with an id from gdwNextClientUnitId (pre-incremented, wrapping before
//  -1). Monsters go through CUNIT_CreateMonsterUnit with bAsyncClientUnit 1.
D2UnitStrc *__fastcall CUNIT_CreateClientOnlyUnit(int32_t nClassId, int32_t nPosX, int32_t nPosY, D2C_UnitTypes dwUnitType, int32_t nItemMode);

//  D2Client.0x6FB27E10, UNIT\CUnit.cpp -- the room's ally count follows a monster's STAT_ALIGNMENT into and out of UNIT_ALIGNMENT_GOOD.
void __fastcall CUNIT_OnUnitAlignmentChanged(D2UnitStrc *pUnit, uint8_t nOldAlign, uint8_t nNewAlign);

//  D2Client.0x6FB27E70, UNIT\CUnit.cpp -- spawn the room's presets whose flag word has bit 0 set: the ones the server skips.
void __fastcall CUNIT_CreatePresetUnitsInRoom(D2ActiveRoomStrc *pRoom);

//  D2Client.0x6FB27ED0, UNIT\CUnit.cpp -- tear a unit down after its unlink: lights, particles, sounds, the per-type free, events, packets, and
//  the player pointer through an inlined CUNIT_SetPlayerUnit(NULL).
void __fastcall CUNIT_DestroyUnit(D2UnitStrc *pUnit);

//  D2Client.0x6FB28040, UNIT\CUnit.cpp -- destroy every unit in both tables, rows in gUnitTypeDestroyOrder's order {1, 0, 2, 3, 4, 5}, the
//  bucket rewritten with the successor after every destroy.
void __cdecl CUNIT_DestroyAllUnits();

//  D2Client.0x6FB280F0, UNIT\CUnit.cpp -- the per-frame pass: eight walks in a fixed order, client-id units looked up again after their
//  update, then nine CUNIT_ForEachUnitInTable passes.
void __cdecl CUNIT_UpdateAllUnits();

//  D2Client.0x6FB28350, UNIT\CUnit.cpp -- a fastcall callback over one row's 128 buckets, the successor read before each call.
void __fastcall CUNIT_ForEachUnitInTable(D2UnitStrc **pTable, void (__fastcall *pfnCallback)(D2UnitStrc *pUnit));

//  D2Client.0x6FB28380, UNIT\CUnit.cpp -- STATLIST_FreeModStats as a callback: a call and a return, taken by address three times.
void __fastcall CUNIT_FreeUnitModStats(D2UnitStrc *pUnit);

//  D2Client.0x6FB28390, UNIT\CUnit.cpp -- the one writer of gpPlayerUnit. Everything it calls still sees the old player; the assignment is last.
void __fastcall CUNIT_SetPlayerUnit(D2UnitStrc *pPlayer);

//  D2Client.0x6FB283D0, UNIT\CUnit.cpp -- the local player, one load.
D2UnitStrc *__fastcall CUNIT_GetPlayer();

//  D2Client.0x6FB283E0, UNIT\CUnit.cpp -- TRUE with no player unit or with one in PLRMODE_DEAD.
BOOL __cdecl CUNIT_IsPlayerDeadOrMissing();

//  D2Client.0x6FB28400, UNIT\CUnit.cpp -- the unit's +0x54, a D2ClientUnitGfxInfoStrc; NULL for no unit.
D2ClientUnitGfxInfoStrc *__fastcall CUNIT_GfxGetUnitGfxInfo(D2UnitStrc *pUnit);

//  D2Client.0x6FB28410..0x6FB284A0, UNIT\CUnit.cpp -- the unit's two palette shifts, nPaletteShift1/2 of its graphics block. A NULL unit asserts
//  "hUnit" and exits.
void __fastcall CUNIT_SetUnitPaletteShift1(D2UnitStrc *pUnit, int32_t nShift);
int32_t __fastcall CUNIT_GetUnitPaletteShift1(D2UnitStrc *pUnit);
void __fastcall CUNIT_SetUnitPaletteShift2(D2UnitStrc *pUnit, int32_t nShift);
int32_t __fastcall CUNIT_GetUnitPaletteShift2(D2UnitStrc *pUnit);

//  D2Client.0x6FB284D0, UNIT\CUnit.cpp -- a room tile's lvlwarp.txt select box, corner plus size, tested inclusively against a screen point.
BOOL __fastcall CUNIT_IsPointInWarpSelectBox(D2UnitStrc *pWarp, int32_t nScreenX, int32_t nScreenY);

//  D2Client.0x6FB285D0..0x6FB286A0, UNIT\CUnit.cpp -- gpSkillSelectProcs[1..6], the skills.txt SelectProc target tests. The first argument is unread.
BOOL __fastcall CUNIT_SkillsSelectProc1_CorpseExplosion(int32_t nUnused, D2UnitStrc *pUnit);
BOOL __fastcall CUNIT_SkillsSelectProc2_RaiseSkeleton(int32_t nUnused, D2UnitStrc *pUnit);
BOOL __fastcall CUNIT_SkillsSelectProc3_Revive(int32_t nUnused, D2UnitStrc *pUnit);
BOOL __fastcall CUNIT_SkillsSelectProc4_FindPotion(int32_t nUnused, D2UnitStrc *pUnit);
BOOL __fastcall CUNIT_SkillsSelectProc6_GrimWard(int32_t nUnused, D2UnitStrc *pUnit);
BOOL __fastcall CUNIT_SkillsSelectProc5_FindItem(int32_t nUnused, D2UnitStrc *pUnit);

//  D2Client.0x6FB286B0, UNIT\CUnit.cpp -- this unit is the selected one, and the selected one is not a door.
BOOL __fastcall CUNIT_ShouldHighlightUnit(D2UnitStrc *pUnit);

//  D2Client.0x6FB286F0, UNIT\CUnit.cpp -- the cursor-targeting decision: the UI shift, the client-id table, three target tests, the select
//  procs, line of sight, then (bTestGeometry) the per-type geometry and component boxes.
BOOL __fastcall CUNIT_HitTestUnit(D2UnitStrc *pUnit, int32_t nScreenX, int32_t nScreenY, int32_t bTestGeometry);

//  D2Client.0x6FB28D90, UNIT\CUnit.cpp -- one component's screen box, padded and stretched to 32x32; nUnused1 and nUnused2 are not read (`retn 10h`).
BOOL __fastcall CUNIT_GetComponentScreenBox(D2UnitStrc *pUnit, int32_t nUnused1, int32_t nComponent, int32_t nAnimFrame, D2BoundingBoxStrc *pBox, int32_t nUnused2);

//  D2Client.0x6FB28F20, UNIT\CUnit.cpp -- a skill's skills.txt SelectProc, called with the caller's pPlayer and pUnit passed straight through.
BOOL __fastcall CUNIT_SkillsRunSelectProcById(D2UnitStrc *pPlayer, D2UnitStrc *pUnit, int32_t nSkillId);

//  D2Client.0x6FB28F90, UNIT\CUnit.cpp -- look the target up by the server's id and set it as the unit's path target; NULL when it is not there.
void __fastcall CUNIT_SetUnitPathTargetById(D2UnitStrc *pUnit, D2C_UnitTypes nTargetType, int32_t nTargetId);

//  D2Client.0x6FB29010, UNIT\CUnit.cpp -- face the path target, at once (bSnap 1) or over time (0); its head is CUNIT_GetPathTargetUnit inlined.
void __fastcall CUNIT_FaceUnitAtPathTarget(D2UnitStrc *pUnit, int32_t bSnap);

//  D2Client.0x6FB29100 and 0x6FB291C0, UNIT\CUnit.cpp -- an NPC turned to face the player after a mode change, and the player turned to face a
//  unit while it stands; neither snaps.
void __fastcall CUNIT_TurnUnitToFacePlayer(D2UnitStrc *pUnit);
void __fastcall CUNIT_TurnPlayerToFaceUnit(D2UnitStrc *pUnit);

//  D2Client.0x6FB29290, UNIT\CUnit.cpp -- may the player start a mouse command: the cursor item, the unit type, a switch on the mode. Every
//  caller passes the skill in edx and the body never reads it.
BOOL __fastcall CUNIT_CanPlayerIssueCommand(D2UnitStrc *pPlayer, D2SkillStrc *pUnusedSkill);

//  D2Client.0x6FB29370, UNIT\CUnit.cpp -- the player's room, or NULL before there is a player. The null test is on the unit, not on the room.
D2ActiveRoomStrc *__cdecl CUNIT_GetPlayerRoom();

//  D2Client.0x6FB29390, UNIT\CUnit.cpp -- on a change of the period of day, OBJECT_ApplyDayNightMode over the object row of both unit tables.
void __cdecl CUNIT_RefreshObjectsOnDayNightChange();

//  D2Client.0x6FB29450 and 0x6FB29480, UNIT\CUnit.cpp -- the unit's server-packet queue at +0xD8, read and written; both assert "ptUnit".
D2PacketListStrc *__fastcall CUNIT_GetUnitPacketQueue(D2UnitStrc *pUnit);
void __fastcall CUNIT_SetUnitPacketQueue(D2UnitStrc *pUnit, D2PacketListStrc *pQueue);

//  D2Client.0x6FB294B0..0x6FB29550, UNIT\CUnit.cpp -- the dead flag: cleared (and CANBEATTACKED set, never cleared), set, and tested with the modes.
void __fastcall CUNIT_SetUnitAlive(D2UnitStrc *pUnit);
void __fastcall CUNIT_SetUnitDead(D2UnitStrc *pUnit);
BOOL __fastcall CUNIT_IsUnitDead(D2UnitStrc *pUnit);

//  D2Client.0x6FB29590, UNIT\CUnit.cpp -- is the unit drawn flat on the floor, by type; a dead monster unless monstats2 UNFLATDEAD.
BOOL __fastcall CUNIT_ShouldDrawUnitUnder(D2UnitStrc *pUnit);

//  D2Client.0x6FB296A0, UNIT\CUnit.cpp -- the path's target, a stale one dropped first; asserts "hPath" at CUnit.cpp:2372.
D2UnitStrc *__fastcall CUNIT_GetPathTargetUnit(D2UnitStrc *pUnit);

//  D2Client.0x6FB296E0, UNIT\CUnit.cpp -- remove and clear the unit's light.
void __fastcall CUNIT_GfxFreeUnitLightMap(D2UnitStrc *pUnit);

//  D2Client.0x6FB29700..0x6FB297A0, UNIT\CUnit.cpp -- the callback over every player, monster or object unit of gpUnitTableById, the unit in ecx.
void __fastcall CUNIT_ForEachPlayerUnit(void (__fastcall *pfnUnitFunction)(D2UnitStrc *pUnit));
void __fastcall CUNIT_ForEachMonsterUnit(void (__fastcall *pfnUnitFunction)(D2UnitStrc *pUnit));
void __fastcall CUNIT_ForEachObjectUnit(void (__fastcall *pfnUnitFunction)(D2UnitStrc *pUnit));

//  D2Client.0x6FB297F0, UNIT\CUnit.cpp -- the name shown for a monster, an object or an item; NULL for the other types.
Unicode *__fastcall CUNIT_GetUnitName(D2UnitStrc *pUnit);

//  D2Client.0x6FB29A40 and 0x6FB29A80, UNIT\CUnit.cpp -- the +0x58 component-draw counter, spent one per draw and armed to 3.
BOOL __fastcall CUNIT_ShouldDrawUnitComponents(D2UnitStrc *pUnit);
void __fastcall CUNIT_SetUnitComponentDrawCount(D2UnitStrc *pUnit);

//  D2Client.0x6FB29AB0, UNIT\CUnit.cpp -- +0x68, the client arm dwStartLight.
void __fastcall CUNIT_SetUnitStartLight(D2UnitStrc *pUnit, int32_t nStartLight);

//  D2Client.0x6FB29AE0, UNIT\CUnit.cpp -- free every item unit of the inventory, or (bKeepItemsHeldElsewhere) keep those another unit holds.
void __fastcall CUNIT_FreeUnitInventory(D2UnitStrc *pUnit, int32_t bKeepItemsHeldElsewhere);

//  D2Client.0x6FB29D10, UNIT\CUnit.cpp -- GFX_AddOverlay, kind 1 for a positive nTargetOrStateId and kind 2 otherwise, and the steal sounds.
void __fastcall CUNIT_AddOverlay(D2UnitStrc *pUnit, int32_t nOverlayId, int32_t nTargetOrStateId);

//  D2Client.0x6FB29D80..0x6FB2A1C0, UNIT\CUnit.cpp -- the alignment half of target selection, and its two skill-flag tests (TARGETPET, TARGETALLY).
BOOL __fastcall CUNIT_IsUnitSelectableTarget(D2UnitStrc *pSource, D2UnitStrc *pTarget);
BOOL __fastcall CUNIT_IsSelectableAsPetTarget(D2UnitStrc *pSource, D2UnitStrc *pTarget);
BOOL __fastcall CUNIT_IsSelectableAsAllyTarget(D2UnitStrc *pSource, D2UnitStrc *pTarget);

//  D2Client.0x6FB2A2D0, UNIT\CUnit.cpp -- single player: copy the server unit's path into the client's and reconcile the anim mode.
void __fastcall CUNIT_ResyncUnitFromServer(D2UnitStrc *pUnit);

//  D2Client.0x6FB2A700 .. 0x6FB2A830, UNIT\CUnitEvent.cpp -- the per-unit event list on D2UnitStrc +0x90 (pCltTimerList): free it, add at the
//  head, remove by the key (+4, +8), and fire every record whose +0 DWORD is the event.
void __fastcall CUNITEVENT_FreeAllForUnit(D2UnitStrc *pUnit);
D2UnitEventStrc *__fastcall CUNITEVENT_AddEvent(D2UnitStrc *pUnit, int32_t nCltEvent, int32_t nSkillId, int32_t nSkillLevel, void *pfnHandler, int32_t nKind, int32_t nParam);
void __fastcall CUNITEVENT_RemoveEventsByKey(D2UnitStrc *pUnit, int32_t nKind, int32_t nParam);
int32_t __fastcall CUNITEVENT_FireEvent(int32_t nEvent, D2UnitStrc *pUnit, D2UnitStrc *pSourceUnit, int32_t nUnused);

//  D2Client.0x6FB57AC0, ENGINE\Cursor.cpp -- draw the cursor, or the item on it. The no-item arm is a tail jump through
//  gCursorDescs[gnCursorType], and the index is not range-checked.
void __cdecl CURSOR_DrawCursorDefault();

//  D2Client.0x6FB57BC0 and 0x6FB57BD0, ENGINE\Cursor.cpp -- the mouse position, one load each.
int32_t __fastcall CURSOR_GetMouseX();
int32_t __fastcall CURSOR_GetMouseY();

//  D2Client.0x6FB57330, ENGINE\Cursor.cpp -- the WM_MOUSEMOVE handler. Only clamps and calls SetCursorPos when the position was out of range.
void __stdcall CURSOR_OnMouseMove(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FB579A0 and 0x6FB57A00, ENGINE\Cursor.cpp -- two fields of gCursorDescs, each behind the same pair of asserts. The duration is
//  fixed point: nFrameCount << 8.
int32_t __fastcall CURSOR_GetCursorLoops(int32_t nCursorType);
int32_t __fastcall CURSOR_GetCursorFrameDuration(int32_t nCursorType);

//  D2Client.0x6FB57700, ENGINE\Cursor.cpp -- draw the cursor and advance its animation. gnCursorFrame is 8.8 fixed point; only 16 of every 64
//  draws advance the default cursor, and the roll needs a player. Declared `__fastcall`, as gCursorDescs' pDrawFunction is.
void __fastcall CURSOR_DrawCurrent();

//  D2Client.0x6FB57BE0, ENGINE\Cursor.cpp -- move the pointer, in client coordinates. Windowed, the border and caption are added back by hand.
void __fastcall CURSOR_SetCursorPos(int32_t nX, int32_t nY);

//  ENGINE\Cursor.cpp, the rest of the unit: three message handlers, the two ends of the cursor's life, and the accessors.
void __stdcall CURSOR_OnNonClientMouseMove(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CURSOR_UpdateCursorPosInGame(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CURSOR_OnPanelMouseUp(SMSGHANDLER_PARAMS *pMsg);
void __fastcall CURSOR_SetBuysellCursor(D2UnitStrc *pUnit, int32_t nFrame);
void __fastcall CURSOR_SetCursorBuySell(int32_t nFrame, int32_t bAltFrame);
int32_t __fastcall CURSOR_SetHeldUnit(D2UnitStrc *pUnit);
D2UnitStrc *__cdecl CURSOR_GetHeldUnit();
void __fastcall CURSOR_Init(int32_t nClickOffsetX);
void __cdecl CURSOR_UnloadCursors();
void __fastcall CURSOR_DrawCursorBuySell();
int32_t __fastcall CURSOR_GetCursorState();

//  D2Client.0x6FB242E0, UI\ui.cpp -- draw the loading screen, and build it on the first call. The progress bar is network games only.
void __fastcall UI_DrawLoadingScreen(int32_t nProgress);

//  D2Client.0x6FAAD260..0x6FAAD556, GAME\Msg.cpp -- the session packets (D2CLTSYS_ ids: 0x66 NEWGAME, 0x6A JOINACT, 0x6C the keepalive,
//  0x68 ENDGAME, 0x67 JOINGAME) and the load bar's ratio. The senders are void: nothing reads what they leave in eax.
void __fastcall MSG_Send_D2CLTSYS_NEWGAME(const char *szName);
void __cdecl MSG_Send_D2CLTSYS_JOINACT();
void __cdecl MSG_SendPing6C();
void __cdecl MSG_Send_D2CLTSYS_ENDGAME();
void __fastcall MSG_Send_D2CLTSYS_JOINGAME(int16_t nGameId, int32_t nTokenId);
void __fastcall MSG_SetLoadProgressRatio(int32_t nDone, int32_t nTotal);
int32_t __cdecl MSG_GetLoadProgressRatio();

//  D2Client.0x6FAAD560, GAME\Msg.cpp -- upload the local .d2s 255 bytes at a time.
//  BUG: the original over-sends by one byte on every packet. Reproduced.
void __fastcall MSG_Send_D2CLTSYS_OPENCHAR(int32_t bShowLoadingScreen);

//  D2Client.0x6FAAD830..0x6FAADBBF, GAME\Msg.cpp -- the packet senders. MSG_SendPacketToServer drops a packet identical to the last one sent
//  inside a 50 or 200 ms window (id 0x3A never); the other eleven build one packet each and end in it. All twelve are void: nothing in the
//  module reads what they leave in eax.
void __fastcall MSG_SendCmd1(uint8_t nCmd);
void __fastcall MSG_SendPacketToServer(uint8_t *pPacket, int32_t nLen);
void __fastcall MSG_SendCmdWord(uint8_t nCmd, int16_t nValue);
void __fastcall MSG_SendCmdXY(uint8_t nCmd, int16_t nX, int16_t nY);
void __fastcall MSG_SendCmd3Words(uint8_t nCmd, int16_t nWord1, int16_t nWord2, int16_t nWord3);
void __fastcall MSG_SendServerItemMessage(uint8_t nCmd, int32_t nUnitGUID);
void __fastcall MSG_SendCmdUnit(uint8_t nCmd, int32_t nUnitType, int32_t nUnitGUID);
void __fastcall MSG_SendCmdUnitParam(uint8_t nCmd, int32_t nUnitType, int32_t nUnitGUID, int32_t nParam);
void __fastcall MSG_SendCmd4Dwords(uint8_t nCmd, int32_t nDword1, int32_t nDword2, int32_t nDword3, int32_t nDword4);
void __fastcall MSG_SendClt5D(D2C_PartyCommands nType, int32_t dwPlayerGUID, int32_t bToggle);
void __fastcall MSG_SendClt5E(D2C_PartyCommands nType, int32_t dwPlayerGUID);
void __fastcall MSG_SendChatMessage(const void *pMsg);

//  D2Client.0x6FAADBC0 and 0x6FAADC70, GAME\Msg.cpp -- the join pump's callback, and the release of the local game (a tail call into D2Game).
BOOL __stdcall MSG_JoinGameMessageLoop(int32_t nUnused);
int32_t __cdecl MSG_FreeGame();

//  D2Client.0x6FAADC90, GAME\Msg.cpp -- the continue test of MSG_AppModeJoinGame's message pump.
int32_t __cdecl MSG_IsJoinGameLoopDone();

//  D2Client.0x6FAADCA0 and 0x6FAADDF0, GAME\Msg.cpp -- the connection wait's and the leave pump's callbacks. WNDPROC_WindowRunMessagePump
//  stops on non-zero.
BOOL __stdcall MSG_JoinGameIdlePoll(int32_t nUnused);
BOOL __stdcall MSG_LeaveGameMessageLoop(int32_t nUnused);

//  D2Client.0x6FB21B70, UI\ui.cpp (written in ui_panels.cpp) -- the in-game UI draw pass. Around fifty panels, hints and labels in one
//  function, and the order they appear in is the z-order. gUiWindowStates[UI_UNKNOWN18] suppresses the whole of it.
void __fastcall UIPANELS_DrawInGamePanels(D2ClientViewStrc *pView);

//  D2Client.0x6FB20590, UI\ui.cpp -- queue the one-shot floating text.
void __fastcall UIPANELS_SetTransientText(const Unicode *pwszText, int32_t nX, int32_t nY, int32_t nColor);

//  D2Client.0x6FB20670, UI\ui.cpp -- the monster health-bar fill, on a 0..128 scale rather than 0..100. Draws nothing; every callee is a query.
int32_t __fastcall UIPANELS_GetUnitLifeFill128(D2UnitStrc *pUnit);

//  D2Client.0x6FB21970, UI\ui.cpp -- draw the pending one-shot text and then wipe the 80-Unicode buffer, so a message lasts exactly one frame.
void __cdecl UIPANELS_DrawTransientText();

//  D2Client.0x6FB21A00, UI\ui.cpp -- the charge-up icon strip. Unreachable in a stock client: its gate is in .data's zero-filled tail and
//  nothing writes it. Reproduced rather than removed.
void __cdecl UIPANELS_DrawChargeUpIcons();

//  D2Client.0x6FB22280, UI\ui.cpp -- one line centred in the play-area band.
//  BUG: copies the argument into a 128-Unicode local with no length check.
void __fastcall UIPANELS_DrawTextCenteredInPanelRect(const Unicode *pwszText, int32_t nFont, int32_t nY, int32_t nColor);

//  D2Client.0x6FB22560, UI\ui.cpp -- the CUNIT_ForEachPlayerUnit callback that labels other players.
//  BUG: its final clip test bounds Y by gnScreenWidth and X by gnScreenHeight. Reproduced.
void __fastcall UIPANELS_BuildOtherPlayerLabel(D2UnitStrc *pUnit);

//  ---- UI\ui.cpp, the label builders ----

//  D2Client.0x6FB20500 -- strip every colour code from a string in place. Quadratic by construction; the strlen < 500 assert is what bounds it.
const Unicode *__fastcall UIPANELS_TextStripColorCodes(Unicode *pszText);

//  D2Client.0x6FB205D0 -- put a string and a colour into the unit-label scratch. A null pointer clears the text and leaves the colour alone.
void __fastcall UIPANELS_SetUnitLabelText(const Unicode *pText, int32_t nColour);

//  D2Client.0x6FB20600 -- should this unit get a floating name label, and as a side effect build its first line. The BOOL is
//  MONSTER_IsMinion's answer.
BOOL __fastcall UIPANELS_ShouldDrawNameLabel(D2UnitStrc *pUnit, D2C_StringIndices nPrefixStr);

//  D2Client.0x6FB21570 -- build the immunity line into gwszUnitLabelLine2. Six packed ten-byte records, bounded by the address of the next global.
void __fastcall UIPANELS_BuildMonsterImmunityLabel(const D2UnitStrc *pUnit);

//  D2Client.0x6FB21680 -- the floating label over a unit and its life-bar fill, on the same 0..128 scale as UIPANELS_GetUnitLifeFill128.
//  Anim mode 17 is a corpse. void: neither caller reads eax.
void __fastcall UIPANELS_BuildUnitLabel(D2UnitStrc *pUnit, int32_t *pnLifeBarFill, int32_t *pbHostileOrCorpse, Unicode *pOutBuffer, int32_t nMaxChars);

//  D2Client.0x6FB21790 -- tile a bordered frame round a RECT. One counter drives all four edges, so the border is one sequence walked round
//  the box.
void __fastcall UIPANELS_DrawTiledFrame(RECT *pRect);

//  D2Client.0x6FB20740, UI\ui.cpp -- an item's ground label, coloured by quality. pszOut == NULL draws it, which is what both callers do.
//  A quest item is forced to the unique colour unless its code is 'leg ', which is Wirt's Leg.
//  BUG: gnLabelScreenY is moved up by 16 BEFORE pUnit is tested, so a row is consumed per call and not per label. Reproduced.
void __fastcall UIPANELS_BuildItemLabelText(D2UnitStrc *pUnit, Unicode *pszOut, int32_t nSize);

//  D2Client.0x6FB20A30, UI\ui.cpp -- the floating name label over any unit, and the driver for every builder above it.
//  BUG: the general monster arm inlines DATATBLS_GetMonStatsTxtRecord, produces NULL on an out-of-range class id and reads a field off it with
//  no test, while the 417/418 arm a thousand bytes away checks the same NULL. Reproduced.
void __fastcall UIPANELS_DrawUnitNameLabel(D2UnitStrc *pUnit);

//  ---- UI\ui.cpp, the window-state core ----

//  D2Client.0x6FB23230 -- read gUiWindowStates[nUIId] with the NUM_UI bound. An id out of range kills the process at ui.cpp:3097.
int32_t __fastcall UI_GetUiWindowState(D2C_UIvars nUIId);

//  D2Client.0x6FB23860 -- the same read with no bound, eight bytes of it.
int32_t __fastcall UI_GetUiWindowStateRaw(D2C_UIvars nUIId);

//  D2Client.0x6FB23890 / 0x6FB238A0 / 0x6FB238B0 -- the 100 ms debounce that stops the click closing one panel from opening another. Enabling
//  and arming are separate calls. UI_ArmPanelToggleDelay's return is not dead: MINIPANEL_RunButtonAction reads it.
void __cdecl UI_ClearPanelToggleDelay();
void __cdecl UI_EnablePanelToggleDelay();
uint32_t __cdecl UI_ArmPanelToggleDelay();

//  D2Client.0x6FB23260 -- open, close or toggle a panel. The only writer of gUiWindowStates, and zero is ON: UI_TURNON = 0, UI_TURNOFF = 1,
//  UI_TOGGLE = 2. Every panel the request conflicts with is resolved first through gaUiToggleRules, the 38 x 38 matrix, and the recursion is
//  real -- a cell value of 1 closes the other panel through this same function.
int32_t __fastcall UI_SetUIToggle(D2C_UIvars nUIId, D2C_UiStates nUIState, int32_t bCheckMousePos);

//  D2Client.0x6FB22D10, UI\ui.cpp -- write one entry of a panel's handler table from the player's bindings, or the shipped default. bPrimary
//  is 1 for the primary binding. The original inlines this function at seven of its eight call sites.
void __fastcall UI_SetPanelHandlerKey(int32_t nEntry, D2HotkeyIndices eCmd, BOOL bPrimary, const D2PanelKeyBindStrc *pBind, uint16_t nDefaultKey);

//  D2Client.0x6FB22AA0, UI\ui.cpp -- run that eight times over one panel's descriptor: two binding slots for each of clear-screen, show-belt,
//  clear-messages, and swap-weapons behind FOG_IsExpansion. Six panels use it.
void __fastcall UI_ApplyPanelKeyBindings(const D2PanelKeyBindStrc *pBind);

//  D2Client.0x6FB226C0, UI\ui.cpp -- hook a panel's input handlers when it opens. A 38-case jump table.
void __fastcall UI_RegisterPanelHandlers(D2C_UIvars nUiId);

//  D2Client.0x6FB22D60, UI\ui.cpp -- drop them when it closes, arm for arm. Three of its arms tail-call functions that live in three other
//  files: SKILLSTREE_ClearCellState, QUESTLOG_ClearPanelState, TEXT_CloseMessageLog.
void __fastcall UI_UnregisterPanelHandlers(D2C_UIvars nUiId);

//  D2Client.0x6FB247E0, UI\ui.cpp -- is the gold dialog up? The dialog object is the flag, not gbGoldDialogOpen, which is the re-entry guard.
BOOL __cdecl UI_IsGoldDialogOpen();

//  D2Client.0x6FB23A60, UI\ui.cpp -- close the gold dialog and destroy the five objects it built, each through vtable slot 0 with the deleting
//  flag. Reads the amount out of the spinner first, so OnOk can close and then send.
void __cdecl UI_CloseGoldEntryDialog();

//  D2Client.0x6FB23B40, UI\ui.cpp -- open it. The mode picks the prompt and the stat the amount is capped against; 1 drop, 2 trade, 3 deposit,
//  4 withdraw, and 0, 5, 6 and 7 are tested for but cannot happen.
void __fastcall UI_OpenGoldEntryDialog(int32_t nGoldDialogMode);

//  D2Client.0x6FB23F60 / 0x6FB23F70 / 0x6FB24010 / 0x6FB240C0, UI\ui.cpp -- the four callbacks the dialog framework drives. None has a code
//  caller: each is passed as a function pointer to a control's constructor, so the return values go to the framework.
int32_t __cdecl UI_GoldDialogOnCancel();
int32_t __cdecl UI_GoldDialogStepAmount();
int32_t __cdecl UI_GoldDialogOnOk();
int32_t __cdecl UI_GoldDialogOnCancelButton();

//  ---- UI\ui.cpp, the accessor tail ----
//  The name prefixes say which subsystem each function serves, not which file it lives in: all of them are ui.cpp.

//  D2Client.0x6FB200E0 -- the trade panel's clear-messages key. Tail-jumps to SMsgBreakHandlerChain, so the callee's `retn 4` returns for it.
void __stdcall UI_TradeOnClearMessagesKey(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FB20100 -- "<DATA\GLOBAL>\UI\<name>" then ARCHIVE_CellFileLoad.
D2CellFileStrc *__fastcall UI_LoadImageFromUiDir(const char *szName);

//  D2Client.0x6FB203C0 -- recompute every cached palette index at once. An act change invalidates all eight, which is why they are cached here
//  and not lazily at each site.
uint8_t __cdecl UI_CachePaletteIndices();

//  D2Client.0x6FB203F0 -- upper-case in place, code points below 0x100 only.
void __fastcall UI_TextToUpperInPlace(struct Unicode *pText);

//  D2Client.0x6FB23800 -- close every open closeable panel; the automap only when asked.
int32_t __fastcall UI_CloseAllPanels(int32_t bIncludeAutomap, int32_t nToggleArg);

//  D2Client.0x6FB23870 / 0x6FB23880 -- six bytes each, one global returned.
D2CellFileStrc *__cdecl UI_GetPentSpinImage();
D2CellFileStrc *__cdecl UI_GetFocusImage();

//  D2Client.0x6FB238D0 -- post a synthetic WM_MOUSEMOVE so the UI re-evaluates hover state without the mouse having moved.
LRESULT __cdecl UI_PostSyntheticMouseMove();

//  D2Client.0x6FB23910 / 0x6FB23960 -- the per-player UI coming up and going away. The pair either side of the local player unit's life, and
//  not the pair either side of the game's.
void __cdecl UI_InitUiForPlayerUnit();
int32_t __cdecl UI_ResetUiSubsystems();

//  D2Client.0x6FB239F0 -- a level's display name; level 8 is special-cased in code, so Den of Evil is not something levels.txt can express.
int32_t __fastcall UI_LevelsGetLevelName(int32_t nLevelId);

//  D2Client.0x6FB23A40 -- client packet 0x27 for the item on the cursor, its id passed as both parameters.
void __cdecl UI_CursorSendUseHeldItem();

//  D2Client.0x6FB240D0 -- give the automap its arrow keys back, or take them away. The three callers are DIALOG_Create, CreateFromImage and Close.
void __fastcall UI_AutomapSetInputEnabled(D2UnkOutdoorStrc2 *bEnable);

//  D2Client.0x6FB24110 -- the buy/sell button art, loaded once and never unloaded.
D2CellFileStrc *__cdecl UI_GetBuySellButtonImage();

//  D2Client.0x6FB24150 -- the loading screen's own palette, for the three disc-prompt screens that run before the UI palette exists.
int32_t __cdecl UI_LoadScreenLoadPalette();

//  D2Client.0x6FB245C0 -- tear the loading screen down. The last three writes happen whether or not there was an image.
void __cdecl UI_UnloadLoadingScreen();

//  D2Client.0x6FB24650 / 0x6FB24660 -- is a load in progress, and a 50 ms throttle whose unsigned compare survives the GetTickCount wrap.
int32_t __cdecl UI_IsLoadingScreenActive();
int32_t __cdecl UI_ThrottleTick50ms();

//  D2Client.0x6FB24690 / 0x6FB24780 -- the horizontal strip no side panel covers, and the y below which the screen is the control panel. Only
//  left and right of the RECT mean anything; bottom gets the screen width.
BOOL __fastcall UI_GetPlayAreaBand(RECT *pBand);
int32_t __cdecl UI_GetPlayAreaBottom();

//  D2Client.0x6FB24790 -- put the Configure Controls panel's input back, the same two tables UI_RegisterPanelHandlers' UI_CONFIG arm registers.
void __cdecl UI_ConfigRegisterPanelHandlers();

//  D2Client.0x6FB247D0 -- close the mercenary hire list. Its parameter is declared and never read.
void __fastcall UI_NpcMenuCloseHireListDialog(int32_t a1);

//  ---- UI\ui.cpp, the last five ----

//  D2Client.0x6FB20160 / 0x6FB202C0 -- every client UI subsystem up and down, the same subsystems in the same order. Init has no `retn` of
//  its own: it tail-jumps into CONFIG_InitControlsScreen.
void __cdecl UI_InitUiSubsystems();
void __cdecl UI_ShutdownUiSubsystems();

//  D2Client.0x6FB20430 -- prepend "<tbl 3994><digit>" to a string in place. The caller's text is the second strcat operand; the other order
//  would put the colour code at the end and colour nothing.
wchar_t *__fastcall UI_AddColorCodeToWstring(wchar_t *wszText, uint32_t nColor);

//  D2Client.0x6FB22320, ui_panels.cpp -- the death panel, drawn every frame by UIPANELS_DrawInGamePanels once the player is dead. Two cel
//  files 48 pixels apart out of the language directory, then one of three tbl messages.
void __cdecl UIPANELS_GameShowDeathMessage();

//  D2Client.0x6FB245B0 -- seven bytes; the loading screen picks its artwork from the act this sets.
void __fastcall UI_SetLoadScreenAct(int32_t nAct);


//  ---- UI\useitem.cpp, the Hellforge anvil panel ----

//  D2Client.0x6FB24840 / 0x6FB248A0 -- bring the panel up and take it down.
void __cdecl USEITEM_AnvilInit();
void __cdecl USEITEM_AnvilShutdown();

//  D2Client.0x6FB249A0 -- slot 4 of gAnvilMsgHandlers, WM_MOUSEMOVE. The hover highlight is live only while the item is still on the cursor.
void __stdcall USEITEM_AnvilOnMouseMove(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FB249D0 -- the panel's third exit, from NPCMENU_OnInteractionStop: the NPC ended the conversation. Does nothing in the
//  finished state 3.
void __cdecl USEITEM_AnvilCloseOnInteractionStop();

//  D2Client.0x6FB24B20 -- slots 0 and 3, WM_LBUTTONUP and WM_RBUTTONUP, both bound to this one function. It claims the message before
//  deciding anything.
void __stdcall USEITEM_AnvilOnButtonUp(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FB24CD0 -- did the release land on a panel button? USEITEM_AnvilOnButtonUp only handles the click itself when this says no.
int32_t __fastcall USEITEM_AnvilOnPanelClick(int32_t nMouseX, int32_t nMouseY);

//  D2Client.0x6FB24F80 -- the matching mouse-down hit test. Clears every button's press marker, then sets it on the one under the cursor.
void __fastcall USEITEM_AnvilOnPanelButtonDown(int32_t nX, int32_t nY);

//  D2Client.0x6FB251E0 -- ESC and SPACE, entries 5 and 6 of the table. Acts only in states 1 and 2; a closed or finished panel just eats the key.
void __stdcall USEITEM_AnvilOnCloseKey(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FB25A10 / 0x6FB25BA0 -- open the panel, and open it in mode 1. Mode 0 arrives on the server's packet and carries
//  gnAnvilInteractGUID; mode 1 is entered locally and cleans up through NPCMENU_NpcCloseInteraction.
int32_t __cdecl USEITEM_AnvilOpenPanel();
int32_t __fastcall USEITEM_AnvilOpenPanelLocal(D2UnitGUID nNpcGUID);

//  D2Client.0x6FB25320 -- the frame tick. It asserts the panel is open rather than returning, so calling it closed halts the client.
void __cdecl USEITEM_AnvilTickPanel();

//  D2Client.0x6FB257B0 -- the server packet. Sub-commands 0, 1, 4, 5, 6 and 7 exist; 2 and 3 halt the client through the default arm's assert.
void __fastcall USEITEM_AnvilOnPanelPacket(D2GSPacketSrv58 *pPacket);

//  D2Client.0x6FB25000 -- mouse down, both buttons, rate-limited to one action per 400 ms. What may go in the slot depends on the panel mode:
//  the vendor's own test in mode 1, the Horadric Staff's item code in mode 0.
void __stdcall USEITEM_AnvilOnButtonDown(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FB25500 -- draw it. Each button's pressed frame is the next cel after its normal one, and the Upgrade label is rewritten three
//  ways so one record serves four transactions.
int32_t __cdecl USEITEM_AnvilDrawPanel();

//  D2Client.0x6FACB6A0 .. 0x6FACB6D0, UI\automap.cpp -- the automap panel's four arrow keys, records 0..3 of gAutomapMessageHandlers. Ten bytes
//  each: one add or sub on gnAutomapPanX / gnAutomapPanY, then `retn 4`. They claim nothing, so the key goes on down Storm's handler chain --
//  which is why opening the chat box has to unregister them. The table is their only reference.
void __stdcall AUTOMAP_OnArrowDown(SMSGHANDLER_PARAMS *pMsg);
void __stdcall AUTOMAP_OnArrowUp(SMSGHANDLER_PARAMS *pMsg);
void __stdcall AUTOMAP_OnArrowLeft(SMSGHANDLER_PARAMS *pMsg);
void __stdcall AUTOMAP_OnArrowRight(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FACB6E0 .. 0x6FACBA3F and 0x6FACF820, UI\automap.cpp -- the settings run: the face this unit shows the rest of the client, and
//  what the ESC menu's automap page does nothing but call. AUTOMAP_GetFade writes: fade 1 is illegal in mode 1 and the getter coerces the
//  global while it is there. AUTOMAP_SetMode does nothing at all when the mode has not changed.
void __fastcall AUTOMAP_RecomputeCenterOffsets(int32_t bForce);
int32_t __cdecl AUTOMAP_GetFade();
int32_t __fastcall AUTOMAP_SetFade(int32_t nFade);
int32_t __cdecl AUTOMAP_GetCenters();
int32_t __fastcall AUTOMAP_SetCentersAndSave(int32_t bCenters);
int32_t __cdecl AUTOMAP_GetLeft();
int32_t __fastcall AUTOMAP_ApplyAutomapLeftSetting(int32_t bAutomapLeft);
int32_t __cdecl AUTOMAP_GetShowParty();
int32_t __fastcall AUTOMAP_SetShowPartyAndSave(int32_t bShow);
int32_t __cdecl AUTOMAP_GetPartyNames();
int32_t __fastcall AUTOMAP_SetPartyNamesAndSave(int32_t bShow);
int32_t __cdecl AUTOMAP_GetMode();
void __fastcall AUTOMAP_SetMode(int32_t nMode);

//  D2Client.0x6FACB760 / 0x6FACF490 / 0x6FACF380, UI/automap.cpp -- the geometry and the settings load, everything AUTOMAP_SetMode reaches.
//  UpdateClipRect recurses on two latched inputs. LoadIconCels has the expansion gate in one arm and not the other. LoadSettings presets
//  every local to its default before the registry read, which the ESC menu's Refresh callbacks do not.
void __cdecl AUTOMAP_UpdateClipRect();
void __cdecl AUTOMAP_LoadIconCels();
int32_t __cdecl AUTOMAP_LoadSettings();

//  D2Client.0x6FACBAF0 / 0x6FACF670 / 0x6FACF730, UI/automap.cpp -- the lifecycle. The tick reveals on distance moved, not on time, through
//  an octagonal approximation, and it halts the client when there is no player. Shutdown walks three lists; the layer walk takes the
//  successor before freeing the node. InitSymbolColors resolves eleven slots that carry nine distinct colours.
void __cdecl AUTOMAP_Tick();
void __cdecl AUTOMAP_Shutdown();
void __cdecl AUTOMAP_InitSymbolColors();

//  D2Client.0x6FACBA40 / 0x6FACD0C0 / 0x6FACD120 / 0x6FACCD50 / 0x6FACC580 / 0x6FACC5B0, UI/automap.cpp -- the cell tree, an AVL tree in the
//  original source (five assert texts name ptLess, ptMore and sBalence). RebalanceTree is the insert -- its first argument is the new cell,
//  and three of its six rotations are inlined copies of the two rotate functions.
D2AutomapCellStrc *__cdecl AUTOMAP_AllocCell();
D2AutomapCellStrc *__fastcall AUTOMAP_RotateLeft(D2AutomapCellStrc **pptRoot);
D2AutomapCellStrc *__fastcall AUTOMAP_RotateRight(D2AutomapCellStrc **pptRoot);
int32_t __fastcall AUTOMAP_RebalanceTree(D2AutomapCellStrc *ptNew, D2AutomapCellStrc **ppRoot);
void __fastcall AUTOMAP_MeasureCellTree(D2AutomapCellStrc *pNode, int32_t *pnSize);
void __fastcall AUTOMAP_WriteCellTree(D2AutomapCellStrc *pNode);

//  D2Client.0x6FACD3C0 / 0x6FACD560 / 0x6FACD650 / 0x6FACD180 / 0x6FACD660 / 0x6FACD840, UI/automap.cpp -- what puts things on the map.
//  AUTOMAP_RebuildAutomapLayer and AUTOMAP_AddTown are the DRLG's two callbacks and have no code caller here; D2Common picks between them by
//  level id, which makes AUTOMAP_AddTown's own assert unreachable. The layer-switch block is written five times in this module with no
//  out-of-line copy anywhere, so it stays written out.
void __fastcall AUTOMAP_AddTile(D2DrlgTileDataStrc *pTile, D2DrlgRoomStrc *pRoom, D2AutomapCellStrc **ppTreeRoot);
void __fastcall AUTOMAP_AddUnit(D2UnitStrc *pUnit, int32_t nCelNum, D2AutomapCellStrc **ppTreeRoot);
void __cdecl AUTOMAP_DelayReveal();
void __fastcall AUTOMAP_AddRoom(D2ActiveRoomStrc *pRoom, int32_t bRevealAll, D2AutomapLayerStrc *pLayer);
void __stdcall AUTOMAP_RebuildAutomapLayer(D2ActiveRoomStrc *pRoom);
void __stdcall AUTOMAP_AddTown(int32_t nLevelId, int32_t nPickedFile, int32_t nCenterX, int32_t nCenterY);

//  D2Client.0x6FACC2B0 / 0x6FACC080 / 0x6FACBCD0 / 0x6FACC610, UI/automap.cpp -- the explored map on disk. `<player>.map` is a 24-byte index
//  over four data files and the last four games are remembered; a data file is a 400-byte table of 100 per-layer chain heads and then records
//  of six-byte {cel, x, y} triples. The layer index is unbounded at both ends, and LoadCelData's chain truncation drops the whole chain for
//  the first record and only the successors for any later one.
int32_t __stdcall AUTOMAP_SaveExploredMap();
void __fastcall AUTOMAP_WriteExploredLayer(D2AutomapDataStrc *pData);
void __cdecl AUTOMAP_RebuildDrawLists();
void __stdcall AUTOMAP_LoadCelData();

//  D2Client.0x6FACF0D0 / 0x6FACF120 / 0x6FACF210 / 0x6FACF060 / 0x6FACEF10 / 0x6FACF290 / 0x6FACED50, UI/automap.cpp -- the corner labels and
//  the party marker. Six of the seven share one shape: right-align at gnViewportRight - width - 16, draw on gnAutomapTextY, advance it by 16.
//  DrawLevelName is the only one that can skip the advance.
void __cdecl AUTOMAP_DrawLevelName();
void __cdecl AUTOMAP_DrawVersionLabel();
void __cdecl AUTOMAP_DrawHostIP();
void __cdecl AUTOMAP_DrawExpansionLabel();
void __cdecl AUTOMAP_DrawGameInfo();
void __cdecl AUTOMAP_DrawDifficultyLabel();
void __fastcall AUTOMAP_DrawPartyMember(D2RosterUnitStrc *pRosterEntry);

//  D2Client.0x6FACECF0 / 0x6FACEC40 / 0x6FACE8F0 / 0x6FACE510 / 0x6FACE230 / 0x6FACDCB0, UI/automap.cpp -- the draw pass. AUTOMAP_DrawUnit
//  draws nothing: it decides whether a unit belongs on the map and writes the colour through its out-parameter.
void __fastcall AUTOMAP_DrawSymbol(int32_t nX, int32_t nY, uint8_t nColor);
void __fastcall AUTOMAP_DrawUnitName(const char *pszName, int32_t nX, int32_t nY, uint8_t nTextColor);
BOOL __fastcall AUTOMAP_DrawUnit(D2UnitStrc *pUnit, uint8_t *pColor);
void __cdecl AUTOMAP_DrawUnits();
void __fastcall AUTOMAP_DrawCellList(D2AutomapCellStrc *pCellList, D2ClientAutomapDrawCtxStrc *pDrawCtx);
void __cdecl AUTOMAP_Draw();

//  D2Client.0x6FAD81F0 .. 0x6FADA140, help.cpp -- the drawing half's loader, unloader, centring helper, wash and three of the overlay's draws.
//  The wash's colour, gnHelpDimColor, is written by nothing, so it is palette index 0; the 800x600 overlay draws a frame of its own, not a copy
//  of HELP_DrawBackground's.
void __cdecl HELP_LoadImages();
int32_t __fastcall HELP_CenterTextX(D2C_StringIndices nTblIdx, int32_t nCenterX);
void __cdecl HELP_DimGameScreen();
void __cdecl HELP_DrawScreen();
void __cdecl HELP_DrawBackground();
void __cdecl HELP_DrawKeyBulletCels();
void __cdecl HELP_DrawLeaderLines();
void __cdecl HELP_DrawLabelsWide();
void __cdecl HELP_DrawLabels();
void __cdecl HELP_DrawCloseButton();
void __cdecl HELP_UnloadImages();

//  D2Client.0x6FADA190 / 0x6FADA220, help.cpp -- the help screen's two mouse handlers. Both claim the message unconditionally; the close button
//  is a fixed rectangle chosen by resolution, and the release only closes when the press landed on it too.
void __stdcall HELP_HelpScreenOnLButtonDown(SMSGHANDLER_PARAMS *pMsg);
void __stdcall HELP_HelpScreenOnLButtonUp(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FADA2D0, help.cpp -- ESC and SPACE, both bound to this one. The only dismiss handler in the panel set that does not claim its
//  key, so the key travels on. The other five all break the chain.
void __stdcall HELP_HelpScreenOnDismissKey(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FADA310, help.cpp -- three bytes of `retn 4`, registered in three panels' tables. It claims nothing, so the message goes on
//  exactly as if the record were absent.
void __stdcall HELP_Nullsub5(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FADA320, help.cpp -- put the mini-panel back if the help screen was what opened it. Called from UI_UnregisterPanelHandlers.
int32_t __cdecl HELP_CloseMiniPanelIfOpened();

//  D2Client.0x6FADA350, help.cpp -- the help button's hint, hidden by six panels and, once the registry says the button was clicked, replaced
//  by turning the button panel off.
void __cdecl HELP_DrawHelpHint();

//  D2Client.0x6FADA5F0 / 0x6FADA660, help.cpp -- the little help button. It asks the cursor where it is rather than reading the message, and
//  the release writes `Help Menu = 1` to the registry, so one click is permanent. Both tail-jump to SMsgBreakHandlerChain; Storm discards what
//  a handler returns, so both are void.
void __stdcall HELP_HelpButtonOnLButtonDown(SMSGHANDLER_PARAMS *pMsg);
void __stdcall HELP_HelpButtonOnLButtonUp(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FADA710 .. 0x6FADA9E0, Ui\Hireables.cpp -- the portrait strip's predicate, the class icons' load and free with the pet-icon
//  cache, the life-bar colours behind their latch, and the pet-icon lookup. gpClassIcons is in the icon strings' order, not D2C_CharClass's.
BOOL __cdecl HIREABLES_PanelPortraitsReserveTopStrip();
void __cdecl HIREABLES_PanelLoadHirelingIcons();
void __cdecl HIREABLES_PanelFreeHirelingIcons();
void __fastcall HIREABLES_PanelInitLifeBarColors(int32_t bForce);
void __cdecl HIREABLES_PanelInvalidateLifeBarColors();
D2CellFileStrc *__fastcall HIREABLES_PanelGetHirelingIcon(D2UnitStrc *pUnit, int32_t nPetType, uint32_t *pdwGUID, int32_t *pbFlag);

//  D2Client.0x6FADAB00 / 0x6FADB180, Ui\Hireables.cpp -- the portrait strip (party, hireling, pets by pettype.txt's icontype, the popup and the
//  hint) and one life bar, whose colour byte travels in bIsPet's stack slot.
void __cdecl HIREABLES_PanelDrawHirelingIconHint();
void __fastcall HIREABLES_PanelDrawPartyLifeBar(uint32_t dwGUID, int32_t nX, int32_t bIsPet);

//  D2Client.0x6FADB280 .. 0x6FADB890, Ui\Hireables.cpp -- the four label drawers (font 6, cut to 66 or 46 pixels from the buffer's or the
//  text's end), the label-row step, the icontype-3 pet icon, and the switch that shows or hides UI_HIRICONS.
void __fastcall HIREABLES_PanelDrawTruncatedHintLabel(Unicode *wszText, int32_t nX, int32_t nY, int32_t nColor);
void __fastcall HIREABLES_PanelDrawIconNameClipped(D2UnitStrc *pUnit, int32_t nX, int32_t nY);
void __fastcall HIREABLES_PanelDrawIconPetTypeClipped(int32_t nPetType, int32_t nX, int32_t nY);
void __fastcall HIREABLES_PanelDrawIconNumberClipped(int32_t nValue, int32_t nX);
void __fastcall HIREABLES_PanelToggleHintIndent(int32_t *pnPos, int32_t *pnStagger);
BOOL __fastcall HIREABLES_PanelDrawHirelingHintRow(D2UnitStrc *pOwner, int32_t nPetType, int32_t *pnX, int32_t *pnY, int32_t *pnStagger);
void __cdecl HIREABLES_PanelUpdateHirelingIconsVisible();

//  D2Client.0x6FADBA90 / 0x6FADBD70 / 0x6FADBF00, Ui\Hireables.cpp -- whether the cursor item can go to the hireling (the argument only
//  decides whether it speaks), the "Show Portraits" key cycling gnPartyPortraitMode, and the reset that arms the hireling popup from the
//  registry.
BOOL __fastcall HIREABLES_CanGiveCursorItem(BOOL bPlaySound);
void __cdecl HIREABLES_CmdTblActionShowPortraits();
void __cdecl HIREABLES_HireIconsResetAndShow();

//  D2Client.0x6FADBF50 .. 0x6FADC6D0, InsertDisc.cpp (the name is ours) -- the play / expansion disc prompt: its idle draw (pings, then the
//  scene routed to INSERTDISC_DrawInsertDiscPromptScene), the two identical Show functions D2Win's archive loaders call and retry on while
//  they answer 1, and the four handlers of the file's own gInsertDiscMessageHandlers.
BOOL __stdcall INSERTDISC_DrawInsertDiscPrompt(int32_t nUnused);
int32_t __cdecl INSERTDISC_DrawInsertDiscPromptScene();
int32_t __stdcall INSERTDISC_ShowInsertPlayDiscPrompt();
int32_t __stdcall INSERTDISC_ShowInsertExpansionDiscPrompt();
void __stdcall INSERTDISC_OnLButtonDown(SMSGHANDLER_PARAMS *pMsg);
void __stdcall INSERTDISC_OnLButtonUp(SMSGHANDLER_PARAMS *pMsg);
void __stdcall INSERTDISC_OnEnter(SMSGHANDLER_PARAMS *pMsg);
void __stdcall INSERTDISC_OnEscape(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FADC6E0 .. 0x6FADCF00, InsertDisc.cpp -- the cinematics prompt: OK and CANCEL, the second line from gnDiscPromptKind, its idle
//  draw (the scene routed to INSERTDISC_DrawInsertCinematicsDiscPromptScene), the Show function that answers "retry" unless CANCEL or Escape
//  was taken, the four handlers of the file's own gCineDiscMessageHandlers, and the kind's one setter.
BOOL __stdcall INSERTDISC_DrawInsertCinematicsDiscPrompt(int32_t nUnused);
int32_t __cdecl INSERTDISC_DrawInsertCinematicsDiscPromptScene();
int32_t __stdcall INSERTDISC_ShowInsertCinematicsDiscPrompt();
void __stdcall INSERTDISC_CineDiscOnLButtonDown(SMSGHANDLER_PARAMS *pMsg);
void __stdcall INSERTDISC_CineDiscOnLButtonUp(SMSGHANDLER_PARAMS *pMsg);
void __stdcall INSERTDISC_CineDiscOnEnter(SMSGHANDLER_PARAMS *pMsg);
void __stdcall INSERTDISC_CineDiscOnEscape(SMSGHANDLER_PARAMS *pMsg);
void __fastcall INSERTDISC_SetDiscPromptKind(int32_t nKind);

//  D2Client.0x6FB07A50 .. 0x6FB081A0, UI\Restart.cpp -- the ESC menu's whole handler table, seven records. Every one claims the message, six
//  by tail jump. UP and DOWN walk separators and disabled rows in two unbounded loops; LEFT and RIGHT step the highlighted row's value with
//  one unsigned compare serving both ends of the range.
void __stdcall ESCMENU_OnLButtonDown(SMSGHANDLER_PARAMS *pMsg);
void __stdcall ESCMENU_OnLButtonUp(SMSGHANDLER_PARAMS *pMsg);
void __stdcall ESCMENU_OnDownKey(SMSGHANDLER_PARAMS *pMsg);
void __stdcall ESCMENU_OnUpKey(SMSGHANDLER_PARAMS *pMsg);
void __stdcall ESCMENU_OnLeftKey(SMSGHANDLER_PARAMS *pMsg);
void __stdcall ESCMENU_OnRightKey(SMSGHANDLER_PARAMS *pMsg);
void __stdcall ESCMENU_OnEnter(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FB07AA0 / 0x6FB07B30 / 0x6FB07D20, UI\Restart.cpp -- the three the table calls. HitTestRow turns a screen Y into a row and is
//  the single place the menu does that; DragSlider is what makes the sliders draggable; ActivateSelectedRow is where ENTER and a click meet,
//  and the only path that hands a row the Storm message record.
int32_t __fastcall ESCMENU_HitTestRow(int32_t nMouseY);
void __fastcall ESCMENU_DragSlider(int32_t nMouseX, int32_t nMouseY);
void __fastcall ESCMENU_ActivateSelectedRow(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FB078B0 .. 0x6FB07A3C, UI\Restart.cpp -- the automap page's five rows, an OnChange and a Refresh each. The five Refresh ones
//  run at menu load (ESCMENU_LoadPageImages calls +0x118 backwards off &dwCount) and four of them on every draw too, from the +0x11C slot.
//  All ten return void, which is the row callback's contract.
void __fastcall ESCMENU_AutomapModeOnChange(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_AutomapModeRefresh(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_AutomapFadeOnChange(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_AutomapFadeRefresh(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_AutomapCenterOnChange(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_AutomapCenterRefresh(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_AutomapPartyOnChange(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_AutomapPartyRefresh(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_AutomapPartyNamesOnChange(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_AutomapPartyNamesRefresh(D2ClientEscMenuItemStrc *pItem, int32_t nArg);

//  D2Client.0x6FB08700 / 0x6FB08860 / 0x6FB07030 / 0x6FB06FC0, UI\Restart.cpp -- open the menu on a page, close it, and the main page's
//  Return to Game and Exit rows: the only original code that walks every UI window id. SetPage turns off every window but 0 and 9 and
//  records six of them; CloseMenu(TRUE) turns those back on, and both rows are CloseMenu(TRUE), inlined in the original. All four return void.
void __fastcall ESCMENU_SetPage(int32_t bCloseUiWindows, int32_t nPage);
void __fastcall ESCMENU_CloseMenu(int32_t bRestorePanels);
void __fastcall ESCMENU_OnReturnToGame(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg);
void __fastcall ESCMENU_OnExitGame(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FB06FB0 / 0x6FB07090 / 0x6FB070A0 / 0x6FB070B0 / 0x6FB07A40 / 0x6FB070C0, UI\Restart.cpp -- the six command rows that move
//  between the menu's pages: five SetPage(0, n) tail jumps, and the Configure Controls row, which is CloseMenu(FALSE) and the UI_CONFIG
//  toggle. Slot ABI, void.
void __fastcall ESCMENU_GotoOptionsPage(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg);
void __fastcall ESCMENU_GotoSoundPage(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg);
void __fastcall ESCMENU_GotoVideoPage(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg);
void __fastcall ESCMENU_GotoAutomapPage(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg);
void __fastcall ESCMENU_GotoMainPage(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg);
void __fastcall ESCMENU_OpenConfigPanel(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FB07080, 0x6FB070F0 .. 0x6FB073DE, UI\Restart.cpp -- the sound page: three sliders that rescale 21 steps to 0..100 with float
//  constants, the 3D and EAX switches read live on every draw, the NPC speech choice (whose Refresh is its OnChange inlined), and six jmp
//  wrappers into other units, each with its target's signature.
int32_t __cdecl j_SOUNDCHAN_IsSystemReady_2(void);
void __fastcall ESCMENU_SoundVolumeOnChange(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_SoundVolumeRefresh(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_MusicVolumeOnChange(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_MusicVolumeRefresh(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
int32_t __cdecl j_SOUNDCHAN_Get3DEnabled(void);
int32_t __cdecl j_SOUNDCHAN_Toggle3dSound(void);
void __fastcall ESCMENU_Sound3DRefresh(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
BOOL __cdecl j_SOUNDCHAN_Is3DEnabled(void);
void __fastcall ESCMENU_Sound3DBiasOnChange(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_Sound3DBiasRefresh(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
int32_t __cdecl j_SOUNDCHAN_GetEaxEnabled(void);
int32_t __cdecl j_SOUNDCHAN_ToggleEaxEffects(void);
void __fastcall ESCMENU_SoundEaxRefresh(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_NpcSpeechOnChange(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_NpcSpeechRefresh(D2ClientEscMenuItemStrc *pItem, int32_t nArg);

//  D2Client.0x6FB073E0 .. 0x6FB078AB and 0x6FB079F0, UI\Restart.cpp -- the video page, whose every Refresh applies its setting once per game,
//  and the automap PartyNames rows' enabling wrapper. The sliders' x87 arithmetic is float-constant exact at 53 bits.
void __fastcall ESCMENU_LightQualityOnChange(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_LightQualityRefresh(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_BlendShadowOnChange(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_BlendShadowRefresh(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_ResolutionOnChange(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_ResolutionRefresh(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
int32_t __stdcall j_D2gfx_10007(void);
void __fastcall ESCMENU_PerspectiveOnChange(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_PerspectiveRefresh(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
int32_t __stdcall j_D2gfx_10019(void);
void __fastcall ESCMENU_GammaOnChange(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_GammaRefresh(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_ContrastOnChange(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
void __fastcall ESCMENU_ContrastRefresh(D2ClientEscMenuItemStrc *pItem, int32_t nArg);
int32_t __cdecl j_AUTOMAP_GetShowParty(void);

//  D2Client.0x6FB081D0 / 0x6FB083C0 / 0x6FB08540 / 0x6FB08640, UI\Restart.cpp -- load the menu once per game (every page's images, and every
//  row's Refresh, which applies its setting) and unload it. The page walkers' parameters carry the names their own asserts spell.
void __fastcall ESCMENU_LoadPageImages(D2ClientEscMenuInfoStrc *ptMenuInfo, D2ClientEscMenuItemStrc *ptMenuItem);
void __cdecl ESCMENU_LoadImages(void);
void __fastcall ESCMENU_UnloadPageImages(D2ClientEscMenuInfoStrc *ptMenuInfo, D2ClientEscMenuItemStrc *ptMenuItem);
void __cdecl ESCMENU_UnloadImages(void);

//  D2Client.0x6FB088B0, UI\Restart.cpp -- draw the menu, every frame it is up: the rows by type, the slider's two boxes, bar and knob, and the
//  two pentagrams spinning in opposite directions.
void __cdecl ESCMENU_Draw(void);

//  D2Client.0x6FAFD8A0 .. 0x6FAFDBD0, config.cpp -- the keyboard half of the Configure Controls screen's table. All six open with the same
//  GAME_IsDisconnecting gate that sets bReturn = 0 and claims nothing. UP and DOWN move the row and skip separators in an unbounded walk; LEFT
//  and RIGHT move between the two binding columns. DELETE/BACKSPACE unbind and ESCAPE/SPACE cancel -- the dismiss path restores the binding
//  snapshot, so there is no keyboard route to accepting.
void __stdcall CONFIG_OnArrowUp(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CONFIG_OnArrowDown(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CONFIG_OnArrowLeft(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CONFIG_OnArrowRight(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CONFIG_OnEraseKey(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CONFIG_OnDismissKey(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FAFD520 .. 0x6FAFDA40, config.cpp -- the mouse half and the two hit tests, which complete gConfigMessageHandlers. Both hit
//  tests answer a sentinel that is not -1: the button test returns 3 for a miss and the row test returns the last row. OnMouseUp settles the
//  bind column's direction -- `(nMouseX < boundary) == gnCfgBindColumn`, so column 1 is the left one.
int32_t __fastcall CONFIG_HitTestButton(int32_t nMouseX, int32_t nMouseY);
int32_t __fastcall CONFIG_HitTestRow(int32_t nMouseY);
void __stdcall CONFIG_OnMouseDown(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CONFIG_OnMouseUp(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CONFIG_OnMouseWheel(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CONFIG_OnEnterBeginCapture(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FAFC260 .. 0x6FAFC5B0 and 0x6FAFD470, config.cpp -- the screen itself: the three button callbacks (Default, Accept, Cancel --
//  all void, their one dispatcher discards eax), the setup the UI subsystems run once a game, the list selector, the button layout, the two
//  list movers and the panel's enter/leave hook. CONFIG_SuspendKeyCommands is the UI_CONFIG arm of the register/unregister pair and describes
//  its bSuspend = 1 half.
void __cdecl CONFIG_OnDefaultButton(void);
void __cdecl CONFIG_OnAcceptButton(void);
void __cdecl CONFIG_OnCancelButton(void);
void __cdecl CONFIG_InitControlsScreen(void);
void __cdecl CONFIG_SelectControlsList(void);
void __cdecl CONFIG_LayoutControlsButtons(void);
void __fastcall CONFIG_ScrollList(int32_t nDelta);
void __cdecl CONFIG_UnloadTextSliderImage(void);
void __fastcall CONFIG_SuspendKeyCommands(int32_t bSuspend);
void __fastcall CONFIG_MoveSelection(int32_t nDelta);

//  D2Client.0x6FAFDC60 .. 0x6FAFE250, config.cpp -- the six capture handlers, the game's whole input while the screen waits for a key. They do
//  not share one shape: a left or right click cancels and binds nothing, KeyUp binds only when armed, and the three mouse captures bind
//  pseudo-keys 0x100..0x104 without a sound.
void __stdcall CONFIG_CaptureMouseButton(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CONFIG_CaptureKeyDown(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CONFIG_CaptureKeyUp(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CONFIG_CaptureMiddleButton(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CONFIG_CaptureXButton(SMSGHANDLER_PARAMS *pMsg);
void __stdcall CONFIG_CaptureMouseWheel(SMSGHANDLER_PARAMS *pMsg);

//  D2Client.0x6FAFCE10, config.cpp -- the screen's whole mouse model: seven regions, a press owned by the region it began in, the list
//  following the cursor, arrows and track on a 50 ms clock, the thumb dragged from its recorded start. Called by the press, the release, a
//  capture-ending click and every draw on which the cursor has moved or the button is down.
void __fastcall CONFIG_UpdateControlsMouseState(int32_t nMouseX, int32_t nMouseY);

//  D2Client.0x6FAFC620, config.cpp -- the panel itself, every frame UI_CONFIG is open: the box and frames, the fifteen visible rows with both
//  bindings coloured and the bind column blinking during a capture, the scrollbar, the buttons and a refused bind's message. void: its one
//  caller discards eax.
void __cdecl CONFIG_DrawKeyBindingsPanel(void);

//  The original's import table, declared in D2Imports.h (generated). It must come after DLLmain.h (which pulls in D2Ptrs.h), because its
//  #defines point the old D2CLIENT_ spelling at the direct import. Every .cpp here includes DLLmain.h and then this file.
#include "D2Imports.h"

//  D2MissileStrc.dwFlags is a D2C_MissileSpawnFlags, and a field of enum type takes only that enum: without these operators `A | B`, `|=` and
//  `^=` on it are integers, and an integer does not convert to an enum. winnt.h's macro defines |, &, ^, ~ and their compound forms on the
//  enum's own underlying type. D2Constants.h, which declares the enum, is generated and cannot carry them.
DEFINE_ENUM_FLAG_OPERATORS(D2C_MissileSpawnFlags)

//  ---- the five panel handler tables ----
//  Each is reached only through its panel's SMSGHANDLER table; nothing calls any of them by name.
void __stdcall PANEL_BeltOnDismissKey(SMSGHANDLER_PARAMS *pMsg);              // panel.cpp
void __stdcall HIREABLES_HirIconsOnF11(SMSGHANDLER_PARAMS *pMsg);                 // Hireables.cpp

//  ---- the belt: panel.cpp, 0x6FAFE9F0 and 0x6FB012E0..0x6FB01C2A ----
//  PANEL_BeltUseColumn0..3 are called only by CMDTBL_ActionUseBelt1..4, which are reached only through the key-function table;
//  PANEL_BeltHitTestPanel is called by name from the input handlers of inv.cpp, panel.cpp, skillstree.cpp, useitem.cpp and char.cpp.
//  The four UseColumn bodies are one body with the column constant changed, kept written out as the original kept them.
int32_t __cdecl PANEL_BeltGetRowCount(void);                                      // panel.cpp
void __fastcall PANEL_BeltOnClickAtPixel(D2UnitStrc *pPlayer, D2InventoryStrc *pInventory, int32_t nX, int32_t nY);  // panel.cpp
void __fastcall PANEL_BeltUpdateHoveredItem(D2UnitStrc *pPlayer, D2InventoryStrc *pInventory, int32_t nX, int32_t nY);  // panel.cpp
void __fastcall PANEL_BeltUseItemInSlot(D2UnitStrc *pUnit, D2InventoryStrc *pInventory, int32_t nSlotId, int32_t bShiftHeld);  // panel.cpp
void __cdecl PANEL_BeltUseColumn0(void);                                      // panel.cpp
void __cdecl PANEL_BeltUseColumn1(void);                                      // panel.cpp
void __cdecl PANEL_BeltUseColumn2(void);                                      // panel.cpp
void __cdecl PANEL_BeltUseColumn3(void);                                      // panel.cpp
void __fastcall PANEL_BeltSetColumnHasItem(uint32_t nColumn, uint8_t bHasItem);  // panel.cpp
void __cdecl PANEL_BeltResetHoverState(void);                                 // panel.cpp
int32_t __fastcall PANEL_BeltHitTestPanel(SMSGHANDLER_PARAMS *pMsg);              // panel.cpp
void __cdecl PANEL_BeltClearHoverItem(void);                                  // panel.cpp
void __cdecl PANEL_BeltShowPanel(void);                                       // panel.cpp

//  ---- panel.cpp: the artwork and the palette cache ----
//  The two image passes, six palette mixers, the hint-phase getter and two draw-code helpers.
void __cdecl PANEL_LoadImages(void);                                     // panel.cpp
void __cdecl PANEL_UnloadImages(void);                                   // panel.cpp
void __cdecl PANEL_CacheStaminaColourRed(void);                          // panel.cpp
void __cdecl PANEL_CacheStaminaColourAmber(void);                        // panel.cpp
uint8_t __cdecl PANEL_CacheStaminaColourBlue(void);              // panel.cpp
void __cdecl PANEL_CacheHighlightColours(void);                          // panel.cpp
int32_t __fastcall PANEL_InvHintPhaseSuppressed(void);                            // panel.cpp
BOOL __fastcall PANEL_IsPointOverRunButton(int32_t nXPos, int32_t nYPos);        // panel.cpp
int32_t __cdecl PANEL_ArmLevelChangeTimer(void);                             // panel.cpp
void __cdecl PANEL_SetBoxFillColorRed(void);                                // panel.cpp
void __cdecl PANEL_SetBoxFillColorBlack(void);                              // panel.cpp

//  ---- panel.cpp: the buttons and the hints ----
void __cdecl PANEL_DrawHostileIndicator(void);                              // panel.cpp
void __fastcall PANEL_SpellSelDrawSkillButton(D2UnitStrc *pUnit, D2SkillStrc *pSkill, int32_t nXPos, int32_t nYPos, int32_t bLeftHand);  // panel.cpp
void __cdecl PANEL_DrawSkillButtons(void);                               // panel.cpp
void __cdecl PANEL_DrawRunHint(void);                                    // panel.cpp
void __cdecl PANEL_DrawMiniPanelToggleButton(void);                      // panel.cpp
void __cdecl PANEL_DrawMinipanelToggleHint(void);                        // panel.cpp

//  ---- panel.cpp: the globes ----
int32_t __fastcall PANEL_GetSmoothedStatValue(int32_t nStatId);                     // panel.cpp
void __cdecl PANEL_DrawOrbHoverText(void);                                  // panel.cpp
void __cdecl PANEL_DrawHealthGlobe(void);                                // panel.cpp
void __cdecl PANEL_DrawManaGlobe(void);                                  // panel.cpp

//  ---- panel.cpp: the warning icons and the bars ----
void __cdecl PANEL_DrawStatusWarningIcons(void);                            // panel.cpp
void __cdecl PANEL_DrawExperienceBar(void);                              // panel.cpp
void __cdecl PANEL_DrawStaminaBar(void);                                 // panel.cpp

//  ---- panel.cpp: the quantity warnings and the hint ----
void __cdecl PANEL_DrawQuantityWarningIcons(void);                          // panel.cpp
void __cdecl PANEL_DrawBeltItemHint(void);                               // panel.cpp

//  ---- panel.cpp: the panel itself and the belt ----
void __cdecl PANEL_Draw800BorderFrame(void);                             // panel.cpp
void __cdecl PANEL_DrawBelt(void);                                       // panel.cpp
void __stdcall TEXT_IniScrollOnMouseDown(SMSGHANDLER_PARAMS *pMsg);          // text.cpp
void __stdcall NEWSTATS_RecipeScrollOnLButtonDown(SMSGHANDLER_PARAMS *pMsg);     // newstats.cpp
void __stdcall SPELLSEL_MiniSkillOnLButtonDown(SMSGHANDLER_PARAMS *pMsg);        // spellsel.cpp

//  ---- text.cpp: the unit's spine ----
//  Lifecycle, list teardown and the accessors. The unit is 0x6FB1A7C0..0x6FB1F36E, 58 functions.
void __fastcall TEXT_FreeParsedLines(D2ClientTextLineStrc *pFirstLine);  // text.cpp
void __stdcall TEXT_FreeParsedText(void **ppParsedText);                 // text.cpp
void __fastcall TEXT_FreeMessageList(D2TextDisplayListStrc *pList);      // text.cpp
void __cdecl TEXT_Init();                                                // text.cpp
void __fastcall TEXT_SetDisplayDelayAndSave(int32_t nDelay);                 // text.cpp
void __fastcall TEXT_SetCallback(void *pfnCallback, int32_t nContext);       // text.cpp
void __cdecl TEXT_StopPopupSpeechSound();                                // text.cpp
D2UnitStrc *__cdecl TEXT_GetPopupSourceUnit();                           // text.cpp
void __fastcall TEXT_MeasureMessageBox(D2ClientPopupStrc *pPopup);       // text.cpp
void __fastcall TEXT_FreeUnitHoverMsg(D2HoverTextStrc *pHoverMsg);       // text.cpp
D2TextHeaderStrc *__cdecl TEXT_GetMessageListHead();                     // text.cpp
BOOL __cdecl TEXT_IsPopupActive();                                       // text.cpp
void __cdecl TEXT_ExpireAllChatMessages();                               // text.cpp
void __cdecl TEXT_FreeChatAndMessageLogLists();                          // text.cpp
void __cdecl TEXT_ClearMessages();                                       // text.cpp
void __cdecl TEXT_Shutdown();                                            // text.cpp

//  ---- text.cpp: the popup ----
//  The four handlers are reached only through gPopupMessageHandlers.
void __fastcall TEXT_PopupSetMessageHandlersActive(int32_t bActive);             // text.cpp
void __fastcall TEXT_PopupSetOwnerUnit(D2UnitStrc *pUnit);                   // text.cpp
void __cdecl TEXT_PopupClearOwnerUnit();                                     // text.cpp
void __cdecl TEXT_ClosePopup();                                          // text.cpp
int32_t __cdecl TEXT_OnPopupDismissed();                                     // text.cpp
int32_t __cdecl j_text_1AE00();                                              // text.cpp
void __stdcall TEXT_PopupOnSysKeyDown(SMSGHANDLER_PARAMS *pMsg);             // text.cpp
void __stdcall TEXT_PopupOnChar(SMSGHANDLER_PARAMS *pMsg);                   // text.cpp
void __stdcall TEXT_PopupOnDismissKey(SMSGHANDLER_PARAMS *pMsg);             // text.cpp
void __stdcall TEXT_PopupOnMouseDown(SMSGHANDLER_PARAMS *pMsg);              // text.cpp
void __fastcall TEXT_DrawHoverBox(D2ClientPopupStrc *pPopup, int32_t nX, int32_t nY); // text.cpp

//  ---- text.cpp: the parser, the openers, the tick ----
struct Unicode **__fastcall TEXT_ParseLinesAndScrollRate(Unicode *pwszText, int32_t *pnLineCount, int16_t *pnScrollRate);  // text.cpp
void __fastcall TEXT_ShowSpeechPopup(D2UnitStrc *pNpc, int16_t nTblStringId, int32_t nUnused);  // text.cpp
void __fastcall TEXT_ShowQuestScrollingText(int32_t nTblStringId, int16_t bIsQuestText, int32_t bNeedsAck);  // text.cpp
void __fastcall TEXT_TickStaticInfoPanel(D2ClientViewStrc *pView);                 // text.cpp

//  ---- text.cpp: what draws ----
//  The hover text over a unit, the popup's own draw, and packet 0x76. High is the player path and Low the monster-and-object one -- 646 bytes
//  against 346, and the difference is three features, not an offset.
void __fastcall TEXT_DrawUnitHoverTextHigh(D2UnitStrc *pUnit);           // text.cpp
void __fastcall TEXT_PlaceHoverMsg(D2UnitStrc *pUnit, D2HoverTextStrc *pHoverMsg, Unicode *wszText, int32_t *pnPos);  // text.cpp
int32_t __fastcall TEXT_DrawHoverMsgIfNotCovered(D2ClientPopupStrc *pPopup, LONG *pnCoords);  // text.cpp
void __fastcall TEXT_DrawUnitHoverTextLow(D2UnitStrc *pUnit);            // text.cpp
BOOL __fastcall TEXT_DrawPopup(D2ClientPopupStrc *pPopup);               // text.cpp
void __fastcall TEXT_ChatClearUnitHoverText(D2GSPacketSrv76 *pPacket);       // text.cpp

//  ---- text.cpp: the chat and quick-message lists ----
//  Two drawers, two builders and the scrollback push they share. The node is 38 bytes packed; the log deep-copies every line so it outlives
//  the display list.
void __cdecl TEXT_DrawChatMessages();                                    // text.cpp
void __cdecl TEXT_DrawQuickMessages();                                   // text.cpp
int32_t __fastcall TEXT_PrintChatMessage(Unicode *wszText, int32_t nColor);           // text.cpp
void __fastcall TEXT_PushMessageLogEntry(D2ClientTextNodeStrc *pSrcMessage); // text.cpp
int32_t __fastcall TEXT_AddQuickMessage(const Unicode *pwszText, int32_t nColor);    // text.cpp

//  ---- text.cpp: the quest scroll ----
//  The panel TEXT_IniScrollOnMouseDown dismisses: what opens it, what draws it, what the server tells it, and the quest-menu string lookup
//  that shares the file. TEXT_QuestGetQuestMenuString returns D2Lang's own buffer, so the pointer is const and belongs to D2Lang.
const Unicode *__fastcall TEXT_QuestGetQuestMenuString(uint16_t nQuestStringId); // text.cpp
void __cdecl TEXT_ScrollDrawQuestScrollPanel();                              // text.cpp
void __cdecl TEXT_IniScrollDrawPanel();                                      // text.cpp
void __fastcall TEXT_DrawQuestScrollFrame(int32_t bAnimate);                 // text.cpp
void __fastcall TEXT_QuestOnCairnStoneOrderCmd(uint8_t *ptCmd);              // text.cpp
void __fastcall TEXT_IniScrollOpen(D2UnitStrc *pTargetItem, D2ItemsTxt *pItemsTxt); // text.cpp

//  ---- text.cpp: the message log and the subtitle ----
//  The log dialog's open/close pair, the close handler the dialog dispatches (no code caller), the off-then-on rebuild, and the CJK-only
//  voice subtitle. CRT_StaticInit17 at 0x6FB1F350 is a compiler-emitted static initialiser, not a function the source had: it is not declared
//  and not written.
void __cdecl TEXT_OpenMessageLog();                                      // text.cpp
int32_t __cdecl TEXT_OnMessageLogClose();                                    // text.cpp
void __cdecl TEXT_CloseMessageLog();                                     // text.cpp
void __fastcall TEXT_ChatShowSpeechSubtitle(D2UnitStrc *pUnit, D2C_Sounds nSoundId); // text.cpp
int32_t __cdecl TEXT_MsgLogReopen();                                             // text.cpp

//  ---- text.cpp: the unit's last three claimers ----
//  The bodies of server packets 0x5A, 0x27 and 0x26 -- every system line the game prints, the NPC's message list, and every line of chat.
void __fastcall TEXT_HandleSystemMessage(D2GSPacketSrv5A *pPacket);      // text.cpp
void __fastcall TEXT_OnMessageListPacket(D2GSPacketSrv27 *pPacket);      // text.cpp
void __fastcall TEXT_HandleChatMessage(D2GSPacketSrv26 *pPacket);        // text.cpp

//  ---- gHireicons and gNewstats handler tables ----
void __stdcall HIREABLES_HireIconsOnLButtonDown(SMSGHANDLER_PARAMS *pMsg);        // Hireables.cpp
void __stdcall HIREABLES_HireIconsOnMouseMove(SMSGHANDLER_PARAMS *pMsg);          // Hireables.cpp
void __stdcall HIREABLES_HireIconsOnRButtonDown(SMSGHANDLER_PARAMS *pMsg);        // Hireables.cpp
void __stdcall NEWSTATS_OnLButtonDown(SMSGHANDLER_PARAMS *pMsg);         // newstats.cpp
void __stdcall NEWSTATS_OnLButtonUp(SMSGHANDLER_PARAMS *pMsg);           // newstats.cpp

//  ---- gNewskills handler table ----
void __stdcall NEWSTATS_NewSkillsOnLButtonDown(SMSGHANDLER_PARAMS *pMsg);        // newstats.cpp
void __stdcall NEWSTATS_NewSkillsOnLButtonUp(SMSGHANDLER_PARAMS *pMsg);          // newstats.cpp

//  ---- newstats.cpp: the recipe scroll, the images, the buttons' visibility and the two hit tests ----
//  NEWSTATS_RecipeScrollShow is a __thiscall in the original, spelled with an unread edx.
void __fastcall NEWSTATS_RecipeScrollShow(const Unicode *pwszText, int32_t nEdxUnused, int32_t nColor);  // newstats.cpp
void __cdecl NEWSTATS_RecipeScrollDraw();                                        // newstats.cpp
void __cdecl NEWSTATS_LoadImages();                                      // newstats.cpp
void __cdecl NEWSTATS_UnloadImages();                                    // newstats.cpp
void __cdecl NEWSTATS_UpdateButtonVisibility();                          // newstats.cpp
BOOL __fastcall NEWSTATS_IsPointOverButton(D2C_UIpanels nUiShift, int32_t nXPos, int32_t nYPos);  // newstats.cpp
BOOL __fastcall NEWSTATS_NewSkillsIsPointOverButton(D2C_UIpanels nUiShift, int32_t nXPos, int32_t nYPos);  // newstats.cpp

//  ---- newstats.cpp: the two buttons' draws, lit and grey ----
void __cdecl NEWSTATS_PanelDrawNewStatsButtonGrey();                             // newstats.cpp
void __cdecl NEWSTATS_PanelDrawNewStatsButton();                                 // newstats.cpp
void __cdecl NEWSTATS_PanelDrawNewSkillButtonGrey();                             // newstats.cpp
void __cdecl NEWSTATS_PanelDrawNewSkillButton();                                 // newstats.cpp

//  ---- gParty and gWaypoint handler tables ----
void __stdcall PARTY_OnLButtonDown(SMSGHANDLER_PARAMS *pMsg);            // Party.cpp
void __stdcall PARTY_OnLButtonUp(SMSGHANDLER_PARAMS *pMsg);              // Party.cpp
void __stdcall WAYPOINT_OnChar(SMSGHANDLER_PARAMS *pMsg);                // waypoint.cpp
void __stdcall WAYPOINT_OnMouseUp(SMSGHANDLER_PARAMS *pMsg);             // waypoint.cpp
void __stdcall WAYPOINT_OnMouseDown(SMSGHANDLER_PARAMS *pMsg);           // waypoint.cpp

//  ---- gQuestscreen and gMercinv handler tables ----
void __stdcall QUESTLOG_QuestScreenOnDismissKey(SMSGHANDLER_PARAMS *pMsg);       // QuestLog.cpp
void __stdcall QUESTLOG_QuestScreenOnMouseDown(SMSGHANDLER_PARAMS *pMsg);        // QuestLog.cpp
void __stdcall QUESTLOG_QuestScreenOnMouseUp(SMSGHANDLER_PARAMS *pMsg);          // QuestLog.cpp
void __stdcall INV_MercInvOnLButtonDown(SMSGHANDLER_PARAMS *pMsg);          // inv.cpp
void __stdcall INV_MercInvOnMouseUp(SMSGHANDLER_PARAMS *pMsg);              // inv.cpp
void __stdcall INV_MercInvOnMouseMove(SMSGHANDLER_PARAMS *pMsg);            // inv.cpp

//  ---- gTrade handler table ----
void __stdcall INV_ConsumeMessage(SMSGHANDLER_PARAMS *pMsg);              // inv.cpp
void __stdcall INV_OnChatKey(SMSGHANDLER_PARAMS *pMsg);                   // inv.cpp
void __stdcall INV_OnKeySwapWeapons(SMSGHANDLER_PARAMS *pMsg);            // inv.cpp
void __stdcall INV_TradeOnDismissKey(SMSGHANDLER_PARAMS *pMsg);             // inv.cpp
void __stdcall INV_TradeOnShowBeltKey(SMSGHANDLER_PARAMS *pMsg);            // inv.cpp
void __stdcall INV_TradeOnMouseMove(SMSGHANDLER_PARAMS *pMsg);              // inv.cpp
void __stdcall INV_TradeOnLButtonDown(SMSGHANDLER_PARAMS *pMsg);            // inv.cpp
void __stdcall INV_TradeOnLButtonUp(SMSGHANDLER_PARAMS *pMsg);              // inv.cpp

//  ---- gGame handler table ----
void __stdcall PANEL_GameOnLButtonDown(SMSGHANDLER_PARAMS *pMsg);             // panel.cpp
void __stdcall PANEL_GameOnLButtonUp(SMSGHANDLER_PARAMS *pMsg);               // panel.cpp
void __stdcall PANEL_GameOnMouseMove(SMSGHANDLER_PARAMS *pMsg);               // panel.cpp
void __stdcall PANEL_GameOnRButtonDown(SMSGHANDLER_PARAMS *pMsg);             // panel.cpp

//  ---- gSkilltree handler table ----
void __stdcall SKILLSTREE_OnLButtonDown(SMSGHANDLER_PARAMS *pMsg);        // skillstree.cpp
void __stdcall SKILLSTREE_OnLButtonUp(SMSGHANDLER_PARAMS *pMsg);          // skillstree.cpp

//  ---- skillstree.cpp, the rest of the unit, in address order ----
void __cdecl SKILLSTREE_ClearSkillPointPending();                         // skillstree.cpp
D2CellFileStrc *__cdecl SKILLSTREE_LoadChartImage();                       // skillstree.cpp
void __cdecl SKILLSTREE_UnloadChartImage();                               // skillstree.cpp
void __cdecl SKILLSTREE_DrawAvailableSkills();                            // skillstree.cpp
void __fastcall SKILLSTREE_DrawSkillIcon(D2C_Skills nSkillId, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nCellState, int32_t nGlobalPaletteShift);   // skillstree.cpp
void __cdecl SKILLSTREE_DrawUnavailableSkills();                          // skillstree.cpp
uint8_t __cdecl SKILLSTREE_CacheBlackIndex();                     // skillstree.cpp
void __cdecl SKILLSTREE_DrawPanel();                                      // skillstree.cpp
void __cdecl SKILLSTREE_DrawLabels();                                     // skillstree.cpp
void __fastcall SKILLSTREE_DrawLabelFromTop480(const Unicode *pwszText, int32_t nY);   // skillstree.cpp
void __fastcall SKILLSTREE_DrawTabLabel(int32_t nMode);                       // skillstree.cpp
BOOL __fastcall SKILLSTREE_IsMouseInRect(int32_t nLeft, int32_t nBottom, int32_t nWidth, int32_t nHeight);   // skillstree.cpp
void __cdecl SKILLSTREE_ClearCellState();                                 // skillstree.cpp

//  ---- gCube handler table ----
void __stdcall INV_ItemPanelOnRButtonDown(SMSGHANDLER_PARAMS *pMsg);        // inv.cpp
void __stdcall INV_CubeOnMouseMove(SMSGHANDLER_PARAMS *pMsg);               // inv.cpp
void __stdcall INV_CubeOnDismissKey(SMSGHANDLER_PARAMS *pMsg);              // inv.cpp
void __stdcall INV_CubeOnLButtonDown(SMSGHANDLER_PARAMS *pMsg);             // inv.cpp
void __stdcall INV_CubeOnLButtonUp(SMSGHANDLER_PARAMS *pMsg);               // inv.cpp

//  ---- gStash handler table ----
void __stdcall INV_StashOnMouseMove(SMSGHANDLER_PARAMS *pMsg);              // inv.cpp
void __stdcall INV_StashOnDismissKey(SMSGHANDLER_PARAMS *pMsg);             // inv.cpp
void __stdcall INV_StashOnLButtonDown(SMSGHANDLER_PARAMS *pMsg);            // inv.cpp
void __stdcall INV_StashOnLButtonUp(SMSGHANDLER_PARAMS *pMsg);              // inv.cpp

//  ---- gNpcshop handler table ----
void __stdcall INV_NpcShopOnDismissKey(SMSGHANDLER_PARAMS *pMsg);           // inv.cpp
void __stdcall INV_NpcShopOnMouseMove(SMSGHANDLER_PARAMS *pMsg);            // inv.cpp
void __stdcall INV_NpcShopOnLButtonDown(SMSGHANDLER_PARAMS *pMsg);          // inv.cpp
void __stdcall INV_NpcShopOnLButtonUp(SMSGHANDLER_PARAMS *pMsg);            // inv.cpp

//  ---- gInv handler table ----
void __stdcall INV_OnLButtonDown(SMSGHANDLER_PARAMS *pMsg);              // inv.cpp
void __stdcall INV_OnLButtonUp(SMSGHANDLER_PARAMS *pMsg);                // inv.cpp
void __stdcall INV_OnMouseMove(SMSGHANDLER_PARAMS *pMsg);                // inv.cpp
void __stdcall INV_OnRButtonDown(SMSGHANDLER_PARAMS *pMsg);              // inv.cpp

//  ---- the shop's state block: inv.cpp 0x6FAE94D0..0x6FAE97BE ----
void __cdecl INV_NpcShopRequestPageRebuild(void);                           // inv.cpp
void __cdecl INV_NpcShopEnterPanelMode19(void);                             // inv.cpp
void __cdecl INV_LeavePanelMode5(void);                                  // inv.cpp
void __cdecl INV_NpcShopLeaveVerifyMode(void);                              // inv.cpp
BOOL __cdecl INV_NpcShopEnterPanelMode6(void);                              // inv.cpp
void __cdecl INV_ResetItemGrids(void);                                   // inv.cpp
void __cdecl INV_NpcShopEnterPanelMode7(void);                              // inv.cpp
void __cdecl INV_NpcShopCloseAndRestoreCursor(void);                        // inv.cpp
void __fastcall INV_NpcShopSetTabRepairAll(int32_t nSlot);                      // inv.cpp
void __fastcall INV_NpcShopSetTabRepair(int32_t nSlot);                         // inv.cpp
void __fastcall INV_NpcShopSetTabBuy(int32_t nSlot);                            // inv.cpp
void __fastcall INV_NpcShopSetTabSell(int32_t nSlot);                           // inv.cpp
void __fastcall INV_NpcShopSetTabEmpty(int32_t nSlot);                          // inv.cpp

//  ---- the panel lifecycle block: inv.cpp ----
void __fastcall INV_CubeFindStashUnit(D2UnitStrc *pUnit);                   // inv.cpp
void __cdecl INV_StashLoadImages(void);                                     // inv.cpp
void __cdecl INV_StashOnPanelClose(void);                                   // inv.cpp
void __cdecl INV_CubeLoadPanelImage(void);                                  // inv.cpp
void __cdecl INV_CubeOnPanelClose(void);                                    // inv.cpp
void __fastcall INV_CubeStartQuestItemAnim(uint32_t dwItemCode);        // inv.cpp
void __cdecl INV_CubeResetQuestItemAnim(void);                              // inv.cpp
void __cdecl INV_TradeRestoreHeldItem(void);                                // inv.cpp
void __cdecl UNRESOLVED_sub_6FAED210(void);                              // inv.cpp
void __cdecl INV_TradeResetPanelState(void);                                // inv.cpp
void __cdecl INV_TradeSetAcceptBlocked(void);                               // inv.cpp
void __cdecl INV_TradeClearAcceptBlocked(void);                             // inv.cpp
void __cdecl INV_ToggleWeaponSwapSet(void);                              // inv.cpp
void __cdecl INV_SwapWeaponsAndLockItems(void);                          // inv.cpp

//  ---- the vendor page block: inv.cpp ----
void INV_ResetItemDescState(void);                                       // inv.cpp
void __fastcall INV_ClearHoverForItem(D2UnitStrc *pItem);                // inv.cpp
void __fastcall INV_GetVendorPageInventory(int32_t nInvPage);                // inv.cpp
void __cdecl INV_ResetPanelState(void);                                  // inv.cpp
void __cdecl INV_NpcShopBuildTabs(void);                                    // inv.cpp

//  ---- the shop-panel and anvil block: inv.cpp ----
void __cdecl INV_NpcShopApplyTabSelection(void);                            // inv.cpp
int32_t __stdcall INV_AnvilForwardMouseMoveToInventory(SMSGHANDLER_PARAMS *pMsg);   // inv.cpp
int32_t __stdcall INV_AnvilForwardLButtonDownToInventory(SMSGHANDLER_PARAMS *pMsg); // inv.cpp
int32_t __stdcall INV_AnvilOnInvCloseButtonRelease(SMSGHANDLER_PARAMS *pMsg);   // inv.cpp
int32_t __fastcall INV_AnvilOnInvCloseButtonPress(SMSGHANDLER_PARAMS *pMsg);    // inv.cpp
void __cdecl INV_NpcShopLoadTabImages(void);                                // inv.cpp
void __cdecl INV_UnloadShopPanelImages(void);                            // inv.cpp
void __fastcall INV_TradeSetPanelMode(int32_t bTradeMode);                      // inv.cpp

//  ---- the panel-state accessors: inv.cpp ----
void __fastcall INV_UpdateToggleSounds(int32_t bFirst, int32_t bSecond);         // inv.cpp
BOOL __cdecl INV_IsPanelModeNormal(void);                                // inv.cpp
BOOL __cdecl INV_IsInventoryMode4(void);                                 // inv.cpp
int32_t __cdecl INV_GetTransactionItemIsPlayers(void);                       // inv.cpp
void __cdecl INV_ClearGoldDialogGuard(void);                              // inv.cpp
void __cdecl INV_ArmGoldDialogGuard(void);                                // inv.cpp
void __fastcall INV_SetHoverEnabled(int32_t bEnabled);                       // inv.cpp

//  ---- the panel art, the palette and two table lookups: inv.cpp ----
void __cdecl INV_InitHighlightColors(void);                              // inv.cpp
void __cdecl INV_LoadPanelImages(void);                                  // inv.cpp
void __cdecl INV_UnloadPanelImages(void);                                // inv.cpp
void __fastcall INV_DrawGridHighlight(D2InventoryGridInfoStrc *pGridInfo, int32_t nCellX, int32_t nCellY, uint8_t nWidth, uint8_t nHeight, uint32_t nColorIdx);  // inv.cpp
BOOL __fastcall INV_CanInsertSocketFiller(D2UnitStrc *pFiller, D2UnitStrc *pTarget);  // inv.cpp
D2SetsTxt *__fastcall INV_DataTblsGetSetsTxtRecord(int32_t nSetId);             // inv.cpp
//  ---- the mercenary's equip rules and three drawing helpers: inv.cpp ----
D2UnitStrc *__fastcall INV_FindComparableEquippedItem(D2InventoryStrc *pInventory, int32_t nFileIndex);  // inv.cpp
void __fastcall INV_DrawSocketedItems(D2UnitStrc *pItem, D2InventoryGridInfoStrc *pGridInfo, int32_t nX, int32_t nY);  // inv.cpp
void __fastcall INV_MercInvDrawSlotHighlights(D2UnitStrc *pMerc);           // inv.cpp
int32_t __fastcall INV_MercInvCanMercEquipItem(D2UnitStrc *pMerc, D2UnitStrc *pItem, int32_t *pbFits);  // inv.cpp
void __fastcall INV_MercInvDrawEmptySlots(D2UnitStrc *pMerc);               // inv.cpp

//  ---- the layout cache and the right-click use path: inv.cpp ----
void __cdecl INV_EnsurePanelLayout(void);                                // inv.cpp
int32_t __fastcall INV_UseItemAtGridCell(D2UnitStrc *pUnit, D2InventoryStrc *pInventory, int32_t nX, int32_t nY, D2C_ItemInvPage nInvPage);  // inv.cpp
void __fastcall INV_PixelToGridCell(D2UnitStrc *pUnit, D2InventoryStrc *pInventory, D2UnitStrc *pItem, uint32_t nX, uint32_t nY, D2InventoryGridInfoStrc *pGridInfo, D2C_ItemInvPage nInvPage);  // inv.cpp
void __fastcall INV_DrawInventoryGridItems(D2UnitStrc *pUnit);           // inv.cpp

//  ---- the three body-slot click handlers: inv.cpp ----
int32_t __fastcall INV_OnBodyLocClickRightArm(D2UnitStrc *pUnit, D2InventoryStrc *pInventory, D2C_PlayerBodyLocs nBodyLoc);  // inv.cpp
int32_t __fastcall INV_OnBodyLocClickLeftArm(D2UnitStrc *pUnit, D2InventoryStrc *pInventory, D2C_PlayerBodyLocs nBodyLoc);  // inv.cpp
int32_t __fastcall INV_OnBodyLocClickGeneric(D2UnitStrc *pUnit, D2InventoryStrc *pInventory, D2C_PlayerBodyLocs nBodyLoc);  // inv.cpp
int32_t __fastcall INV_OnGridLeftClick(D2UnitStrc *pUnit, D2InventoryStrc *pInventory, int32_t nXPos, int32_t nYPos, uint8_t nMouseFlags, D2InventoryGridInfoStrc *pGridInfo, D2C_ItemInvPage nInvPage);  // inv.cpp

//  ---- the item description's lines: inv.cpp ----
void __fastcall INV_AppendEtherealAndSocketsText(D2UnitStrc *pItem, Unicode *wszOut);  // inv.cpp
void __fastcall INV_AppendDurabilityText(D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt);  // inv.cpp
void __fastcall INV_FormatReqLevelLine(Unicode *wszOut, int32_t nLevel);       // inv.cpp
void __fastcall INV_FormatReqStrengthLine(Unicode *wszOut, D2ItemsTxt *pItemsTxt, int32_t nDelta);  // inv.cpp
void __fastcall INV_FormatReqDexterityLine(Unicode *wszOut, D2ItemsTxt *pItemsTxt, int32_t nDelta);  // inv.cpp
void __fastcall INV_CalcElementalDamageRange(D2UnitStrc *pTarget, D2UnitStrc *pItem, D2C_ItemStats nMinStatId, D2C_ItemStats nMaxStatId, int32_t *pnMinDamage, int32_t *pnMaxDamage, BOOL *pbAdjusted);  // inv.cpp
void __fastcall INV_AppendChanceToBlockText(D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt);  // inv.cpp
void __fastcall INV_AppendSkillDamageText(D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt);  // inv.cpp
void __fastcall INV_AppendDefenseText(D2UnitStrc *pUnit, D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt);  // inv.cpp
void __fastcall INV_AppendStackQuantityText(D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt);  // inv.cpp
void __fastcall INV_AppendAttackSpeedText(D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt);  // inv.cpp
void __fastcall INV_AppendSpellDescText(D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt);  // inv.cpp
void __fastcall INV_BuildItemNameText(D2UnitStrc *pItem, Unicode *wszOut, int32_t nMaxLen);  // inv.cpp
void __fastcall INV_DrawItemDescription(D2UnitStrc *pUnit, BOOL bVendorUnit);  // inv.cpp
void __fastcall INV_AppendElementalDamageText(D2UnitStrc *pItem, Unicode *wszOut, D2ItemsTxt *pItemsTxt);  // inv.cpp
void __fastcall INV_AppendPriceFooterText(const Unicode *pwszIn, Unicode *pwszOut);  // inv.cpp
void __fastcall INV_MercInvDrawStatRows(D2UnitStrc *pMerc);                 // inv.cpp
void __cdecl INV_MercInvDrawPanel(void);                                    // inv.cpp
void __fastcall INV_DrawGoldAmount(int32_t nPanelKind);                      // inv.cpp
void __cdecl INV_UpdateAndDrawPanels(void);                              // inv.cpp
void __cdecl INV_StashOpenPanel(void);                                      // inv.cpp
void __cdecl INV_StashTeardownPanel(void);                                  // inv.cpp
void __cdecl INV_CubeOpenPanel(void);                                       // inv.cpp

//  ---- trade.cpp ----
void __cdecl TRADE_ResetState(void);                                     // trade.cpp
void __cdecl TRADE_CloseOnShutdown(void);                                // trade.cpp
void __cdecl TRADE_DrawButtonHints(void);                                // trade.cpp
void __cdecl TRADE_OpenWindow(void);                                     // trade.cpp
void __fastcall TRADE_CloseWindow(int32_t bReopenInventory);                 // trade.cpp
BOOL __cdecl TRADE_IsTradeInProgress(void);                              // trade.cpp
void __cdecl TRADE_SendCancelOnce(void);                                 // trade.cpp
void __fastcall TRADE_HandleStateMessage(uint8_t *pMessage);     // trade.cpp
int32_t __cdecl TRADE_SendAgreeToTrade(void);                                // trade.cpp
int32_t __cdecl TRADE_SendCancel(void);                                      // trade.cpp
int32_t __cdecl TRADE_SendCancelEarlyStamp(void);                            // trade.cpp
void __fastcall TRADE_SetPartnerFromPacket78(D2GSPacketSrv78 *pPacket);  // trade.cpp
Unicode *__cdecl TRADE_GetPartnerName(void);                             // trade.cpp
void __cdecl TRADE_SendAccept(void);                                     // trade.cpp
void __cdecl TRADE_SendGreenCheck(void);                                 // trade.cpp
void __fastcall TRADE_SetGold(int32_t nGold, uint8_t bSelf);                    // trade.cpp
int32_t __cdecl TRADE_GetGoldOther(void);                                    // trade.cpp
int32_t __cdecl TRADE_GetGoldSelf(void);                                     // trade.cpp
void __fastcall TRADE_SendGoldAmount(uint32_t nGold);                // trade.cpp
int32_t __cdecl TRADE_TickTimeout(void);                                     // trade.cpp

//  ---- waypoint.cpp, with the last of useitem.cpp ----
D2WaypointDataStrc *__cdecl WAYPOINT_AllocPanelData(void);               // waypoint.cpp
void __cdecl WAYPOINT_ClosePanel(void);                                  // waypoint.cpp
void __cdecl WAYPOINT_DrawPanel(void);                                   // waypoint.cpp
void __cdecl WAYPOINT_SendCloseOnce(void);                               // waypoint.cpp
void __fastcall WAYPOINT_OnOpenPacket(D2GSPacketSrv63 *pPacket);         // waypoint.cpp
void __fastcall WAYPOINT_ClampActTab(int32_t nAct);                          // waypoint.cpp
void __stdcall WAYPOINT_BuildActWaypointList(void);                      // waypoint.cpp
int32_t __cdecl USEITEM_NullCallback(void);                         // useitem.cpp

//  ---- dialog.cpp ----
void __stdcall DIALOG_OnMouseMove(SMSGHANDLER_PARAMS *pMsg);             // dialog.cpp
int32_t __fastcall DIALOG_HitTestLine(D2ClientDialogStrc *pDialog, SMSGHANDLER_PARAMS *pMsg);  // dialog.cpp
void __stdcall DIALOG_OnMouseUp(SMSGHANDLER_PARAMS *pMsg);               // dialog.cpp
void __stdcall DIALOG_OnMouseDown(SMSGHANDLER_PARAMS *pMsg);             // dialog.cpp
void __stdcall DIALOG_OnCancelKey(SMSGHANDLER_PARAMS *pMsg);             // dialog.cpp
void __stdcall DIALOG_OnMouseWheel(SMSGHANDLER_PARAMS *pMsg);            // dialog.cpp
void __stdcall DIALOG_OnEscape(SMSGHANDLER_PARAMS *pMsg);                // dialog.cpp
void __stdcall DIALOG_OnArrowRight(SMSGHANDLER_PARAMS *pMsg);            // dialog.cpp
void __stdcall DIALOG_OnArrowLeft(SMSGHANDLER_PARAMS *pMsg);             // dialog.cpp
void __stdcall DIALOG_OnArrowUp(SMSGHANDLER_PARAMS *pMsg);               // dialog.cpp
void __stdcall DIALOG_OnArrowDown(SMSGHANDLER_PARAMS *pMsg);             // dialog.cpp
void __stdcall DIALOG_OnEnter(SMSGHANDLER_PARAMS *pMsg);                 // dialog.cpp
void __stdcall DIALOG_OnChar(SMSGHANDLER_PARAMS *pMsg);                  // dialog.cpp
void __cdecl DIALOG_Init(void);                                          // dialog.cpp
void __cdecl DIALOG_Shutdown(void);                                      // dialog.cpp
uint32_t *__fastcall DIALOG_AddControl(D2ClientDialogStrc *pDialog, void *pControl, int32_t nUnused1, int32_t nUnused2);  // dialog.cpp
D2ClientDialogStrc *__fastcall DIALOG_CreateFromImage(int32_t nCenterX, int32_t nCenterY, DIALOGCALLBACK pfnCallback, DIALOGCALLBACK pfnCallback2, int32_t nImageIndex, int32_t nInputMode);  // dialog.cpp
D2ClientDialogStrc *__fastcall DIALOG_Create(int32_t nCenterX, int32_t nCenterY, DIALOGCALLBACK pfnCallback, DIALOGCALLBACK pfnCallback2, int32_t nInputMode, int32_t bVAlignByStaticLines,
    int32_t bAutoSize, int32_t nWidth, int32_t nHeight, int32_t bIgnoreCancelKey, int32_t bGrabsKeyboard);  // dialog.cpp
void __fastcall DIALOG_Close(D2ClientDialogStrc *hDialog);                   // dialog.cpp
int32_t __fastcall DIALOG_Draw(D2ClientDialogStrc *pDialog);                 // dialog.cpp
void __fastcall DIALOG_SetSelectedLine(D2ClientDialogStrc *pDialog, int32_t nIndex);  // dialog.cpp
void __fastcall DIALOG_SetHighlight(D2ClientDialogStrc *pDialog, int32_t eHighlight);  // dialog.cpp
void __fastcall DIALOG_AddLine(D2ClientDialogStrc *pDialog, const Unicode *wszText, int32_t nLineHeight, int32_t nUnused1, int32_t nBaseColor, int32_t nFont, DIALOGCALLBACK pfnOnClick, int32_t bSelectable);  // dialog.cpp
void __fastcall DIALOG_AutoSize(D2ClientDialogStrc *pDialog);            // dialog.cpp
int32_t __fastcall DIALOG_GetSelectedLine(D2ClientDialogStrc *pDialog);      // dialog.cpp
int32_t __cdecl DIALOG_GetNestDepth(void);                                   // dialog.cpp

//  ---- CmdTbl.cpp ----
void __cdecl CMDTBL_ActionCharacterScreen(void);                        // CmdTbl.cpp
void __cdecl CMDTBL_ActionInventoryScreen(void);                        // CmdTbl.cpp
void __cdecl CMDTBL_ActionPartyScreen(void);                            // CmdTbl.cpp
void __cdecl CMDTBL_ActionMessageLog(void);                             // CmdTbl.cpp
void __cdecl CMDTBL_ActionQuestLog(void);                               // CmdTbl.cpp
void __cdecl CMDTBL_ActionChat(void);                                   // CmdTbl.cpp
void __cdecl CMDTBL_ActionHelpScreen(void);                             // CmdTbl.cpp
void __cdecl CMDTBL_ActionAutomap(void);                                // CmdTbl.cpp
void __cdecl CMDTBL_ActionCenterAutomap(void);                          // CmdTbl.cpp
void __cdecl CMDTBL_ActionFadeAutomap(void);                            // CmdTbl.cpp
void __cdecl CMDTBL_ActionPartyOnAutomap(void);                         // CmdTbl.cpp
void __cdecl CMDTBL_ActionNamesOnAutomap(void);                         // CmdTbl.cpp
void __cdecl CMDTBL_ActionToggleAutomapLeft(void);                      // CmdTbl.cpp
void __cdecl CMDTBL_ActionSkillTree(void);                              // CmdTbl.cpp
void __cdecl CMDTBL_ActionSkillSelect(void);                            // CmdTbl.cpp
void __cdecl CMDTBL_ActionSelectSkillSlot0(void);                       // CmdTbl.cpp
void __cdecl CMDTBL_ActionSelectSkillSlot1(void);                       // CmdTbl.cpp
void __cdecl CMDTBL_ActionSelectSkillSlot2(void);                       // CmdTbl.cpp
void __cdecl CMDTBL_ActionSelectSkillSlot3(void);                       // CmdTbl.cpp
void __cdecl CMDTBL_ActionSelectSkillSlot4(void);                       // CmdTbl.cpp
void __cdecl CMDTBL_ActionSelectSkillSlot5(void);                       // CmdTbl.cpp
void __cdecl CMDTBL_ActionSelectSkillSlot6(void);                       // CmdTbl.cpp
void __cdecl CMDTBL_ActionSelectSkillSlot7(void);                       // CmdTbl.cpp
void __cdecl CMDTBL_ActionSelectSkillSlot8(void);                       // CmdTbl.cpp
void __cdecl CMDTBL_ActionSelectSkillSlot9(void);                       // CmdTbl.cpp
void __cdecl CMDTBL_ActionSelectSkillSlot10(void);                      // CmdTbl.cpp
void __cdecl CMDTBL_ActionSelectSkillSlot11(void);                      // CmdTbl.cpp
void __cdecl CMDTBL_ActionSelectSkillSlot12(void);                      // CmdTbl.cpp
void __cdecl CMDTBL_ActionSelectSkillSlot13(void);                      // CmdTbl.cpp
void __cdecl CMDTBL_ActionSelectSkillSlot14(void);                      // CmdTbl.cpp
void __cdecl CMDTBL_ActionSelectSkillSlot15(void);                      // CmdTbl.cpp
void __cdecl CMDTBL_ActionShowBelt(void);                               // CmdTbl.cpp
void __cdecl CMDTBL_ActionUseBelt1(void);                               // CmdTbl.cpp
void __cdecl CMDTBL_ActionUseBelt2(void);                               // CmdTbl.cpp
void __cdecl CMDTBL_ActionUseBelt3(void);                               // CmdTbl.cpp
void __cdecl CMDTBL_ActionUseBelt4(void);                               // CmdTbl.cpp
void __cdecl CMDTBL_ActionSayHelp(void);                                // CmdTbl.cpp
void __cdecl CMDTBL_ActionSayFollowMe(void);                            // CmdTbl.cpp
void __cdecl CMDTBL_ActionSayThisIsForYou(void);                        // CmdTbl.cpp
void __cdecl CMDTBL_ActionSayThanks(void);                              // CmdTbl.cpp
void __cdecl CMDTBL_ActionSaySorry(void);                               // CmdTbl.cpp
void __cdecl CMDTBL_ActionSayBye(void);                                 // CmdTbl.cpp
void __cdecl CMDTBL_ActionSayNowYouDie(void);                           // CmdTbl.cpp
void __cdecl CMDTBL_ActionSayRetreat(void);                             // CmdTbl.cpp
void __cdecl CMDTBL_ActionRun(void);                                    // CmdTbl.cpp
void __cdecl CMDTBL_ActionToggleRunWalk(void);                          // CmdTbl.cpp
void __cdecl CMDTBL_ActionStandStill(void);                             // CmdTbl.cpp
void __cdecl CMDTBL_ActionShowItems(void);                              // CmdTbl.cpp
int32_t __cdecl j_GAME_CmdTblActionClearScreen(void);                           // CmdTbl.cpp
void __cdecl CMDTBL_ActionEscMenu(void);                                // CmdTbl.cpp
void __cdecl CMDTBL_ActionSelectPreviousSkill(void);                    // CmdTbl.cpp
void __cdecl CMDTBL_ActionSelectNextSkill(void);                        // CmdTbl.cpp
void __cdecl CMDTBL_ActionSwapWeapons(void);                            // CmdTbl.cpp
void __cdecl CMDTBL_ActionMercenaryInventory(void);                     // CmdTbl.cpp
void __cdecl j_TEXT_ExpireAllChatMessages(void);                        // CmdTbl.cpp
void __cdecl CMDTBL_ActionRunReleased(void);                            // CmdTbl.cpp
void __cdecl CMDTBL_ActionStandStillReleased(void);                     // CmdTbl.cpp
void __cdecl CMDTBL_ActionShowItemsReleased(void);                      // CmdTbl.cpp
void __cdecl CMDTBL_CallMouse3Down(void);                               // CmdTbl.cpp
void __cdecl CMDTBL_CallMouse3Lift(void);                               // CmdTbl.cpp
void __cdecl CMDTBL_CommandHandleMouse4Down(void);                            // CmdTbl.cpp
void __cdecl CMDTBL_CommandHandleMouse4Lift(void);                            // CmdTbl.cpp
void __cdecl CMDTBL_CommandHandleMouse5Down(void);                            // CmdTbl.cpp
void __cdecl CMDTBL_CommandHandleMouse5Lift(void);                            // CmdTbl.cpp
void __fastcall CMDTBL_CommandHandleMouseWheel(int32_t nDelta);                   // CmdTbl.cpp
int32_t __fastcall CMDTBL_OpenPlayerKeyFile(int32_t dwDesiredAccess, int32_t dwCreationDisposition); // CmdTbl.cpp
int16_t __fastcall CMDTBL_ControlsFindBoundKey(D2HotkeyIndices eCmd, BOOL bPrimary); // CmdTbl.cpp
BOOL __fastcall CMDTBL_ControlsIsActionBound(D2HotkeyIndices eCmd, BOOL bPrimary); // CmdTbl.cpp
void __fastcall CMDTBL_UnbindCommand(int32_t eCmd, int32_t bPrimary);           // CmdTbl.cpp
int16_t __cdecl CMDTBL_SaveBindingsSnapshot(void);                      // CmdTbl.cpp
void __cdecl CMDTBL_CommitBindings(void);                               // CmdTbl.cpp
int16_t __cdecl CMDTBL_RestoreBindingsSnapshot(void);                   // CmdTbl.cpp
void __cdecl CMDTBL_Shutdown(void);                                     // CmdTbl.cpp
const Unicode *__fastcall CMDTBL_ControlsGetBoundKeyNameFallback(D2HotkeyIndices eCmd, BOOL bPrimary); // CmdTbl.cpp
const Unicode *__fastcall CMDTBL_ControlsGetBoundKeyName(D2HotkeyIndices eCmd, BOOL bPrimary); // CmdTbl.cpp
BOOL __fastcall CMDTBL_IsKeyBindable(uint16_t nVKCode);         // CmdTbl.cpp
BOOL __fastcall CMDTBL_BindKey(int32_t eCmd, int32_t bPrimary, uint16_t nVKCode, int16_t *pnErrorTblIdx); // CmdTbl.cpp
void __stdcall CMDTBL_OnKeyDown(SMSGHANDLER_PARAMS *pMsg);              // CmdTbl.cpp
void __stdcall CMDTBL_OnKeyUp(SMSGHANDLER_PARAMS *pMsg);                // CmdTbl.cpp
void __fastcall CMDTBL_SetKeyHookMode(int32_t nMode, int32_t bSwapReleaseHandlers); // CmdTbl.cpp
void __cdecl CMDTBL_LoadDefaultBindings(void);                          // CmdTbl.cpp
void __cdecl CMDTBL_SaveBindingsToFiles(void);                          // CmdTbl.cpp
int16_t __fastcall CMDTBL_CommandLoadDefaultKeyBindings(void *bTryDiskFile);  // CmdTbl.cpp
int16_t __cdecl CMDTBL_COMMAND_PrepareMouseMappings(void);            // CmdTbl.cpp


//  ---- editbox.cpp ----
char *__fastcall EDITBOX_UiEditBoxConstruct(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, int32_t nX, int32_t nY, int32_t nWidth, int32_t nLineCount, int32_t nMaxValue, uint16_t nMaxChars, void *pfnCallback, int32_t nFont); // editbox.cpp
D2ClientEditBoxStrc *__fastcall EDITBOX_UiEditBoxVecDelDtor(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, uint8_t bFreeObject); // editbox.cpp
void __fastcall EDITBOX_UiEditBoxDestruct(D2ClientEditBoxStrc *pEditBox);      // editbox.cpp
int32_t __fastcall EDITBOX_UiEditBoxSetNumericOnly(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, int32_t bNumericOnly); // editbox.cpp
void __fastcall EDITBOX_UiEditBoxDraw(D2ClientEditBoxStrc *pEditBox);          // editbox.cpp
int32_t __fastcall EDITBOX_UiEditBoxSetTextFromInt(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, int32_t nValue); // editbox.cpp
int32_t __fastcall EDITBOX_UiEditBoxSetTextFromAscii(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, const char *szText); // editbox.cpp
int32_t __fastcall EDITBOX_UiEditBoxGetIntValue(D2ClientEditBoxStrc *pEditBox);    // editbox.cpp
char *__fastcall EDITBOX_UiEditBoxGetTextUtf8(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, char *szOut); // editbox.cpp
int32_t __cdecl EDITBOX_ReturnZeroCallback(void);                                // editbox.cpp
int32_t __fastcall EDITBOX_UiEditBoxOnMouseDown(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg); // editbox.cpp
int32_t __fastcall EDITBOX_UiEditBoxOnMouseUp(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg); // editbox.cpp
int32_t __fastcall EDITBOX_UiEditBoxOnChar(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg); // editbox.cpp
int32_t __stdcall EDITBOX_DialogNullCallback(int32_t nUnused);                         // editbox.cpp
int32_t __fastcall EDITBOX_UiEditBoxSetWidth(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, int32_t nWidth); // editbox.cpp
int32_t __fastcall EDITBOX_UiCtrlSetFlag24(void *pControl, int32_t nEdxUnused, int32_t bFlag); // editbox.cpp
int32_t __fastcall EDITBOX_UiEditBoxOnArrowRight(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg); // editbox.cpp
int32_t __fastcall EDITBOX_UiEditBoxOnArrowLeft(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg); // editbox.cpp
void __fastcall EDITBOX_UiCtrlProxyDestruct(int32_t *pProxy);                      // editbox.cpp
uint32_t **__fastcall EDITBOX_UiCtrlProxyVecDelDtor(uint32_t **pProxy, int32_t nEdxUnused, uint8_t bFreeObject); // editbox.cpp
uint32_t *__fastcall EDITBOX_UiCtrlProxyConstructEditbox(uint32_t *pProxy, int32_t nEdxUnused, int32_t nX, int32_t nY, int32_t nWidth, int32_t nLineCount, int32_t nMaxValue, uint16_t nMaxChars, void *pfnCallback, int32_t nFont); // editbox.cpp
//  A jump forwarder passes whatever the caller pushed and the target's `retn` pops it, so each one's argument list is the purge of the
//  implementation slot it jumps to. Slots 1, 11 and 13 purge 0; the rest purge 4.
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot4(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg); // editbox.cpp
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot5(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg); // editbox.cpp
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot6(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg); // editbox.cpp
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot8(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg); // editbox.cpp
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot7(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg); // editbox.cpp
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot9(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg); // editbox.cpp
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot10(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg); // editbox.cpp
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot11(uint32_t **pProxy);                  // editbox.cpp -- slot 11 purges 0
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot13Arg(uint32_t **pProxy, int32_t nEdxUnused, int32_t nUnused); // editbox.cpp
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot15(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg); // editbox.cpp
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot14(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg); // editbox.cpp
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot12(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg); // editbox.cpp
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot16(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg); // editbox.cpp
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot17(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg); // editbox.cpp
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot18(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg); // editbox.cpp

//  ---- npcmenu.cpp: the state core and its accessors ----
BOOL __fastcall NPCMENU_IsVendorGamble(void);                           // npcmenu.cpp
int32_t __cdecl NPCMENU_NpcGetVendorClassId(void);                                 // npcmenu.cpp
D2BitBufferStrc *__cdecl NPCMENU_QuestGetQuestRecord(void);                    // npcmenu.cpp
D2BitBufferStrc *__cdecl NPCMENU_QuestGetQuestRecordSecondary(void);           // npcmenu.cpp
BOOL __cdecl NPCMENU_IsUiStateActive(void);                             // npcmenu.cpp
D2UnitStrc *__cdecl NPCMENU_NPC_GetActiveNpcUnit(void);                      // npcmenu.cpp
int32_t __fastcall NPCMENU_GetInteractingNpc(D2UnitGUID *pNpcGUID);         // npcmenu.cpp
BOOL __cdecl NPCMENU_GetInteractingNpcUnit(void);                        // npcmenu.cpp
BOOL __cdecl NPCMENU_IsAnyDialogOpen(void);                             // npcmenu.cpp
void __fastcall NPCMENU_SetInteractingNpc(int32_t *pNpc);                   // npcmenu.cpp
void __fastcall NPCMENU_SetUnitFlag(D2UnitStrc *pUnit, uint32_t nMask, int32_t bSet); // npcmenu.cpp
void __fastcall NPCMENU_MonsterSetDataFlag(D2UnitStrc *pUnit, int16_t wFlagMask, int32_t bSet); // npcmenu.cpp
bool __fastcall NPCMENU_SpeechNotYetPlayed(int16_t nTblStringId);       // npcmenu.cpp
bool __fastcall NPCMENU_IsSpeechOnlyNpc(D2C_MonsterIds nMonsterClassId); // npcmenu.cpp
int32_t __cdecl NPCMENU_FindEntryIndexForNpc(void);                         // npcmenu.cpp
void __cdecl NPCMENU_RerollAllSpeechLines(void);                        // npcmenu.cpp
void __fastcall NPCMENU_SetRowCount2(int32_t nRecord);                      // npcmenu.cpp
void __fastcall NPCMENU_SetRowCount3(int32_t nRecord);                      // npcmenu.cpp
void __fastcall NPCMENU_SetRowCount4(int32_t nRecord);                      // npcmenu.cpp
void __fastcall NPCMENU_EnableIdentifyRow(int32_t nRecord);                 // npcmenu.cpp
void __fastcall NPCMENU_AddHirelingOffer(D2GSPacketSrv4E *pPacket);     // npcmenu.cpp
void __cdecl NPCMENU_ClearHirelingOffers(void);                         // npcmenu.cpp
void __fastcall NPCMENU_RefreshHireIconsForOffer(int16_t wMercName);    // npcmenu.cpp
void __fastcall NPCMENU_NoteOpenedEunuchObject(D2UnitStrc *pObject);    // npcmenu.cpp
void __fastcall NPCMENU_SetResurrectMercOffer(D2C_StringIndices wNameTblId, int32_t nCost); // npcmenu.cpp
void __fastcall NPCMENU_MarkEntriesFromPacket91(uint16_t *pPacket); // npcmenu.cpp
void __cdecl NPCMENU_TeardownHireList(void);                            // npcmenu.cpp
void __fastcall NPCMENU_QuestCopySecondaryQuestRecord(uint8_t *pBuffer);       // npcmenu.cpp
void __fastcall j_INV_NpcShopRequestPageRebuild(D2UnitStrc *pNpc);        // npcmenu.cpp
//  ---- npcmenu.cpp: the quest unlocks and the item service ----
int32_t __cdecl NPCMENU_QuestUnlock_KashyaHire(void);                       // npcmenu.cpp
int32_t __cdecl NPCMENU_QuestUnlock_WarrivGoEast(void);                     // npcmenu.cpp
int32_t __cdecl NPCMENU_QuestUnlock_MeshifSailEast(void);                   // npcmenu.cpp
int32_t __cdecl NPCMENU_QuestUnlock_CharsiImbue(void);                      // npcmenu.cpp
int32_t __cdecl NPCMENU_OnItemServiceClicked(void);                         // npcmenu.cpp
int32_t __cdecl NPCMENU_QuestUnlock_LarzukSockets(void);                    // npcmenu.cpp
int32_t __cdecl NPCMENU_QuestUnlock_AnyaPersonalize(void);                  // npcmenu.cpp
int32_t __cdecl NPCMENU_QuestUnlock_QualKehkHire(void);                     // npcmenu.cpp
int32_t __cdecl NPCMENU_QuestUnlock_TyraelTravel(void);                     // npcmenu.cpp
void __fastcall NPCMENU_SendQuestItemAction(D2UnitGUID dwItemGUID);     // npcmenu.cpp
//  ---- npcmenu.cpp: the teardown and the callbacks built on it ----
void __fastcall NPCMENU_NpcFreeVendorInventory(D2UnitStrc *pNpc);              // npcmenu.cpp
void __fastcall NPCMENU_NpcResyncVendorInventory(uint32_t dwNpcGUID);      // npcmenu.cpp
void __fastcall NPCMENU_NpcEndInteraction(uint32_t dwNpcGUID);             // npcmenu.cpp
void __cdecl NPCMENU_NpcCloseInteraction(void);                                // npcmenu.cpp
void __cdecl NPCMENU_NpcCloseInteraction_Dup1(void);                           // npcmenu.cpp
void __cdecl NPCMENU_NpcCloseInteraction_Dup2(void);                           // npcmenu.cpp
void __cdecl NPCMENU_NpcShopCloseAndEndInteraction(void);                      // npcmenu.cpp
int32_t __cdecl NPCMENU_OnTalkMenuCancel(void);                             // npcmenu.cpp
int32_t __cdecl NPCMENU_OnTalkMenuClickAway(void);                          // npcmenu.cpp
int32_t __cdecl NPCMENU_OnMainMenuCancel(void);                             // npcmenu.cpp
int32_t __cdecl NPCMENU_OnMainMenuClickAway(void);                          // npcmenu.cpp
int32_t __cdecl NPCMENU_OnInterruptCancel(void);                            // npcmenu.cpp
int32_t __cdecl NPCMENU_OnInterruptClickAway(void);                         // npcmenu.cpp
int32_t __cdecl NPCMENU_OnGoEastClicked(void);                              // npcmenu.cpp
int32_t __cdecl NPCMENU_OnSailEastClicked(void);                            // npcmenu.cpp
int32_t __cdecl NPCMENU_OnTravelToHarrogathClicked(void);                   // npcmenu.cpp
int32_t __cdecl NPCMENU_OnSailWestClicked(void);                            // npcmenu.cpp
int32_t __cdecl NPCMENU_OnTravelWestClicked(void);                          // npcmenu.cpp
//  ---- npcmenu.cpp: the teardown's other forms ----
int32_t __cdecl NPCMENU_OnTradeClicked(void);                               // npcmenu.cpp
int32_t __cdecl NPCMENU_OnGambleClicked(void);                              // npcmenu.cpp
int32_t __cdecl NPCMENU_OnInterruptNewsClicked(void);                       // npcmenu.cpp
int32_t __cdecl NPCMENU_OnResultContinueClicked(void);                      // npcmenu.cpp
void __fastcall NPCMENU_AbortOnPlayerAction(D2UnitStrc *pUnit);         // npcmenu.cpp
void __fastcall NPCMENU_EndInteraction(uint32_t dwNpcGUID);         // npcmenu.cpp
//  ---- npcmenu.cpp: the lifecycle and the frame ----
void __cdecl NPCMENU_Init(void);                                        // npcmenu.cpp
void __cdecl NPCMENU_Shutdown(void);                                    // npcmenu.cpp
void __cdecl NPCMENU_Reset(void);                                       // npcmenu.cpp
void __cdecl NPCMENU_CloseAllDialogs(void);                             // npcmenu.cpp
void __cdecl NPCMENU_TickAndValidate(void);                             // npcmenu.cpp
void __cdecl NPCMENU_EndInteractionByUiState(void);                     // npcmenu.cpp
//  ---- npcmenu.cpp: the item-message acknowledgements ----
void __fastcall NPCMENU_AckItemMessage(uint32_t dwItemGUID);                  // npcmenu.cpp
void __fastcall NPCMENU_AckItemAction_ToInventoryGrid(uint32_t dwItemGUID);   // npcmenu.cpp
void __fastcall NPCMENU_AckItemAction_RemoveFromBodyLoc(uint32_t dwItemGUID); // npcmenu.cpp
void __fastcall NPCMENU_AckItemAction_InsertIntoBodyLoc(uint32_t dwItemGUID); // npcmenu.cpp
void __fastcall NPCMENU_AckItemAction_ToStorePage(uint32_t dwItemGUID);       // npcmenu.cpp
void __fastcall NPCMENU_AckItemAction_SetInvPage(uint32_t dwItemGUID);        // npcmenu.cpp
void __fastcall NPCMENU_ClearSelectionIfStoreItem(D2UnitGUID dwItemGUID);         // npcmenu.cpp
//  ---- npcmenu.cpp: the speech popups ----
void __fastcall NPCMENU_PickRandomSpeechLine(D2NpcMenuEntryStrc *pSpeechState);   // npcmenu.cpp
int32_t __cdecl NPCMENU_ShowDescriptorSpeech(void);                                   // npcmenu.cpp
void __fastcall NPCMENU_ShowSpeechLine(int32_t nSpeechLine, uint8_t bTakeLineFromDescriptor); // npcmenu.cpp
int32_t __cdecl NPCMENU_OnTalkLineClicked(void);                                      // npcmenu.cpp
int32_t __cdecl NPCMENU_OnJerhynMoreGossipClicked(void);                              // npcmenu.cpp
int32_t __cdecl NPCMENU_OnCainCubeGossipClicked(void);                                // npcmenu.cpp
int32_t __fastcall NPCMENU_OnSpeechDismissed(void *pArg, int32_t bDismissed, int32_t bByMouse); // npcmenu.cpp
//  ---- npcmenu.cpp: the talk menu and the speech queue ----
int32_t __cdecl NPCMENU_OnTalkClicked(void);                                          // npcmenu.cpp
void __fastcall NPCMENU_BuildTalkMenu(int32_t nUnused, D2C_StringIndices nTitleTblIdx, int32_t nMenuEntryIdx); // npcmenu.cpp
int32_t __cdecl NPCMENU_PlayClassGreeting(void);                                      // npcmenu.cpp
int32_t __fastcall NPCMENU_ChooseNextSpeech(int32_t nContext, int32_t bDismissed, int32_t bDismissedByMouse); // npcmenu.cpp
//  ---- npcmenu.cpp: the hire list ----
int32_t __cdecl NPCMENU_DrawHireMercenaryList(void);                                  // npcmenu.cpp
int32_t __stdcall NPCMENU_OnHireListBack(void);                                       // npcmenu.cpp
int32_t __cdecl NPCMENU_OnHireListConfirmClicked(void);                               // npcmenu.cpp
void __cdecl NPCMENU_ConfirmHireMercenary(void);                                  // npcmenu.cpp
int32_t __fastcall NPCMENU_ToggleResurrectMercRow(int32_t nDescriptorIndex, int32_t bAdd);    // npcmenu.cpp
//  ---- npcmenu.cpp: the transaction's wait and answer ----
void __fastcall NPCMENU_SendTransactionAndWait(int32_t bSkippedVerify, int32_t nMouseKeyFlags); // npcmenu.cpp
int32_t __cdecl NPCMENU_OnIdentifyClicked(void);                                      // npcmenu.cpp
int32_t __cdecl NPCMENU_OnResurrectMercClicked(void);                                 // npcmenu.cpp
void __fastcall NPCMENU_OnTransactionResult(D2GSPacketSrv2A *pPacket);            // npcmenu.cpp
void __cdecl NPCMENU_ShowTransactionResult(void);                                 // npcmenu.cpp
//  ---- npcmenu.cpp: the price, the transaction and the confirm dialog ----
BOOL __fastcall NPCMENU_TransactionCost(D2UnitStrc *pItem, BOOL bIsOwned, int32_t *pnCost, Unicode *wszOut, int32_t nSize);  // npcmenu.cpp
BOOL __fastcall NPCMENU_BeginTransaction(D2UnitStrc *pPlayer, D2UnitStrc *pItem, int32_t bPlayerOwnsItem, int32_t nMouseX, int32_t nMouseY, int32_t nMouseKeyFlags, int32_t bQuickTransaction, int32_t bRepairAll);  // npcmenu.cpp
void __cdecl NPCMENU_BuildConfirmDialog(void);                                    // npcmenu.cpp
int32_t __cdecl NPCMENU_OnGraphicActionVerified(void);                                // npcmenu.cpp
int32_t __cdecl NPCMENU_OnConfirmDialogDismissed(void);                               // npcmenu.cpp
//  ---- npcmenu.cpp: the main menu, its opener and the interrupt dialog ----
void __cdecl NPCMENU_BuildMainMenu(void);                                         // npcmenu.cpp
void __fastcall NPCMENU_OpenMainMenu(D2UnitStrc *pNpc, int32_t bAlreadyInteracting);  // npcmenu.cpp
void __cdecl NPCMENU_BuildInterruptDialog(void);                                  // npcmenu.cpp
//  ---- npcmenu.cpp: the conversation's doors ----
void __fastcall NPCMENU_ClearNpcAlertOverlay(D2UnitStrc *pUnit);                  // npcmenu.cpp
void __fastcall NPCMENU_OnNpcCmdPacket(D2GSPacketSrv50 *ptCmd);                   // npcmenu.cpp
void __fastcall NPCMENU_OnInteractionStop(uint8_t nUnitType, int32_t dwGuid); // npcmenu.cpp
void __fastcall NPCMENU_BeginInteraction(uint8_t nUnitType, int32_t dwNpcGUID, uint8_t *pQuestData, uint8_t nAbortUnitType);  // npcmenu.cpp
void __fastcall NPCMENU_ShowNpcAlert(D2GSPacketSrv8A *pPacket);                   // npcmenu.cpp
//  ---- SCmd.cpp: the table, the router and the unit queue ----
D2PacketListStrc *__fastcall SCMD_AllocUnitPacketQueue(D2UnitStrc *pUnit);        // SCmd.cpp
void __fastcall SCMD_RouteServerPackets(uint8_t *pBuffer, int32_t nSize);       // SCmd.cpp
void __fastcall SCMD_FreeUnitPacketQueue(D2UnitStrc *pUnit);                        // SCmd.cpp
void __fastcall SCMD_ProcessUnitPacketQueue(D2UnitStrc *pUnit);                     // SCmd.cpp
//  ---- SCmd.cpp: npcmenu.cpp's doors ----
void __fastcall SCMD_HandleSrv28_NpcMenu(D2GSPacketSrv28 *pPacket);               // SCmd.cpp
void __fastcall SCMD_HandleSrv62_NpcInteractionStop(D2GSPacketSrv62 *pPacket);    // SCmd.cpp
void __fastcall SCMD_HandleSrv29_QuestRecord(D2GSPacketSrv29 *pPacket);           // SCmd.cpp
void __fastcall SCMD_HandleSrv2A_TransactionResult(D2GSPacketSrv2A *pPacket);     // SCmd.cpp
void __fastcall SCMD_HandleSrv42_ClearCursor(D2GSPacketSrv42 *pPacket);           // SCmd.cpp
void __fastcall SCMD_HandleSrv50_QuestSpecial(D2GSPacketSrv50 *pPacket);          // SCmd.cpp
void __fastcall SCMD_HandleSrv4F_ClearHirelingOffers(D2GSPacketSrv4F *pPacket);   // SCmd.cpp
void __fastcall SCMD_HandleSrv4E_AddHirelingOffer(D2GSPacketSrv4E *pPacket);      // SCmd.cpp
void __fastcall SCMD_HandleSrv91_NpcIntros(D2GSPacketSrv91 *pPacket);             // SCmd.cpp
void __fastcall SCMD_HandleSrv8A_NpcAlert(D2GSPacketSrv8A *pPacket);              // SCmd.cpp
void __fastcall SCMD_HandleSrv9B_ResurrectMercOffer(D2GSPacketSrv9B *pPacket);    // SCmd.cpp
//  ---- SCmd.cpp: the game's start and the units that come and go ----
void __fastcall SCMD_HandleSrv01_GameFlags(D2GSPacketSrv01 *pPacket);             // SCmd.cpp
void __fastcall SCMD_HandleSrv03_GameSeed(D2GSPacketSrv03 *pPacket);              // SCmd.cpp
void __fastcall SCMD_HandleSrv02_ActLoaded(D2GSPacketSrv02 *pPacket);             // SCmd.cpp
void __fastcall SCMD_HandleSrv04_LoadComplete(D2GSPacketSrv04 *pPacket);          // SCmd.cpp
void __fastcall SCMD_HandleSrv05_UnloadComplete(D2GSPacketSrv05 *pPacket);        // SCmd.cpp
void __fastcall SCMD_HandleSrv06_GameExit(D2GSPacketSrv06 *pPacket);              // SCmd.cpp
void __fastcall SCMD_HandleSrv07_MapReveal(D2GSPacketSrv07 *pPacket);             // SCmd.cpp
void __fastcall SCMD_HandleSrv08_MapHide(D2GSPacketSrv08 *pPacket);               // SCmd.cpp
void __fastcall SCMD_HandleSrv09_SpawnUnit(D2GSPacketSrv09 *pPacket);             // SCmd.cpp
void __fastcall SCMD_HandleSrv51_SpawnObject(D2GSPacketSrv51 *pPacket);           // SCmd.cpp
void __fastcall SCMD_HandleSrv52_QuestList(D2GSPacketSrv52 *pPacket);             // SCmd.cpp
void __fastcall SCMD_HandleSrv0A_RemoveUnit(D2GSPacketSrv0A *pPacket);            // SCmd.cpp
void __fastcall SCMD_HandleSrv0B_SetPlayerUnit(D2GSPacketSrv0B *pPacket);         // SCmd.cpp
//  ---- SCmd.cpp: the character file, and what a unit is told to do ----
void __fastcall SCMD_SaveCharacterFile(uint8_t *pBuffer, int32_t nLen);                 // SCmd.cpp
void __fastcall SCMD_HandleSrv0C_Action(D2UnitStrc *pUnit, D2GSPacketSrv0C *pPacket);                     // SCmd.cpp
void __fastcall SCMD_HandleSrv0D_ActionAtXY(D2UnitStrc *pUnit, D2GSPacketSrv0D *pPacket);                 // SCmd.cpp
void __fastcall SCMD_HandleSrv0E_ActionOnTarget(D2UnitStrc *pUnit, D2GSPacketSrv0E *pPacket);             // SCmd.cpp
void __fastcall SCMD_HandleSrv0F_ActionAtXYFromPos(D2UnitStrc *pUnit, D2GSPacketSrv0F *pPacket);          // SCmd.cpp
void __fastcall SCMD_HandleSrv10_ActionOnTargetFromPos(D2UnitStrc *pUnit, D2GSPacketSrv10 *pPacket);      // SCmd.cpp
void __fastcall SCMD_HandleSrv67_ActionAtXYPathed(D2UnitStrc *pUnit, D2GSPacketSrv67 *pPacket);           // SCmd.cpp
void __fastcall SCMD_HandleSrv68_ActionOnTargetPathedFromPos(D2UnitStrc *pUnit, D2GSPacketSrv68 *pPacket); // SCmd.cpp
void __fastcall SCMD_HandleSrv69_ActionAtXY(D2UnitStrc *pUnit, D2GSPacketSrv69 *pPacket);                 // SCmd.cpp
void __fastcall SCMD_HandleSrv6A_ActionOnTarget(D2UnitStrc *pUnit, D2GSPacketSrv6A *pPacket);             // SCmd.cpp
void __fastcall SCMD_HandleSrv6B_ActionAtXYFromPos(D2UnitStrc *pUnit, D2GSPacketSrv6B *pPacket);          // SCmd.cpp
void __fastcall SCMD_HandleSrv6C_ActionOnTargetFromPos(D2UnitStrc *pUnit, D2GSPacketSrv6C *pPacket);      // SCmd.cpp
void __fastcall SCMD_HandleSrv6D_PierceAtXY(D2UnitStrc *pUnit, D2GSPacketSrv6D *pPacket);                 // SCmd.cpp
void __fastcall SCMD_HandleSrv17_ApplyRemoteUnitAction(D2UnitStrc *pUnit, uint8_t *pPacket);                    // SCmd.cpp
void __fastcall SCMD_HandleSrv4C_UnitAction(D2UnitStrc *pUnit, D2GSPacketSrv4C *pPacket);                 // SCmd.cpp
void __fastcall SCMD_HandleSrv4D_UnitAction(D2UnitStrc *pUnit, D2GSPacketSrv4D *pPacket);                 // SCmd.cpp
//  ---- SCmd.cpp: overlays, warps, positions, a monster's stats, skill effects ----
void __fastcall SCMD_HandleSrv11_AddOverlay(D2GSPacketSrv11 *pPacket);            // SCmd.cpp
void __fastcall SCMD_HandleSrv15_ReassignPlayer(D2GSPacketSrv15 *pPacket);        // SCmd.cpp
void __fastcall SCMD_HandleSrv16_UnitsUpdate(D2GSPacketSrv16 *pPacket);           // SCmd.cpp
void __fastcall SCMD_RemergeItemStatLists(D2UnitStrc *pUnit);                     // SCmd.cpp
void __fastcall SCMD_HandleSrv9E_A2(uint8_t *pPacket);                    // SCmd.cpp
void __fastcall SCMD_HandleSrvA3_SkillEffect(D2GSPacketSrvA3 *pPacket);           // SCmd.cpp
void __fastcall SCMD_HandleSrvA5_EndSkillMove(D2GSPacketSrvA5 *pPacket);          // SCmd.cpp
void __fastcall SCMD_HandleSrvA4_PreloadMonsterCofs(D2GSPacketSrvA4 *pPacket);    // SCmd.cpp
//  ---- SCmd.cpp: the player's stats, and where the server says it is ----
void __fastcall SCMD_HandleSrv19_1F_AttributeUpdate(uint8_t *pPacket);    // SCmd.cpp
void __fastcall SCMD_HandleSrv20_AttributeUpdate(D2GSPacketSrv20 *pPacket);       // SCmd.cpp
void __fastcall SCMD_HandleSrv18_PlayerAttributes(D2GSPacketSrv18 *pPacket);      // SCmd.cpp
void __fastcall SCMD_HandleSrv95_LifeManaUpdate(D2GSPacketSrv95 *pPacket);        // SCmd.cpp
void __fastcall SCMD_HandleSrv96_WalkVerify(D2GSPacketSrv96 *pPacket);            // SCmd.cpp
//  ---- SCmd.cpp: skills, casts, chat and sounds ----
void __fastcall SCMD_HandleSrv21_UpdateSkills(D2GSPacketSrv21 *pPacket);          // SCmd.cpp
void __fastcall SCMD_HandleSrv93_SkillPageLevels(D2GSPacketSrv93 *pPacket);       // SCmd.cpp
void __fastcall SCMD_HandleSrv94_BaseSkillLevels(D2GSPacketSrv94 *pPacket);       // SCmd.cpp
void __fastcall SCMD_HandleSrv22_SetSkill(D2GSPacketSrv22 *pPacket);              // SCmd.cpp
void __fastcall SCMD_HandleSrv23_SetActiveSkill(D2GSPacketSrv23 *pPacket);        // SCmd.cpp
void __fastcall SCMD_HandleSrv98_MinionSpawnClass(D2GSPacketSrv98 *pPacket);      // SCmd.cpp
void __fastcall SCMD_HandleSrv99_CastAtUnit(D2GSPacketSrv99 *pPacket);            // SCmd.cpp
void __fastcall SCMD_HandleSrv9A_CastAtXY(D2GSPacketSrv9A *pPacket);              // SCmd.cpp
void __fastcall SCMD_HandleSrv26_ChatMessage(D2GSPacketSrv26 *pPacket);           // SCmd.cpp
void __fastcall SCMD_HandleSrv76_ClearUnitHoverText(D2GSPacketSrv76 *pPacket);    // SCmd.cpp
void __fastcall SCMD_HandleSrv5A_SystemMessage(D2GSPacketSrv5A *pPacket);         // SCmd.cpp
void __fastcall SCMD_HandleSrv27_OverheadText(D2GSPacketSrv27 *pPacket);          // SCmd.cpp
void __fastcall SCMD_HandleSrv2C_PlayUnitSound(D2GSPacketSrv2C *pPacket);         // SCmd.cpp
//  ---- SCmd.cpp: items, environment, monsters, the roster, portals ----
void __fastcall SCMD_HandleSrv3E_ItemStatUpdate(D2GSPacketSrv3E *pPacket);        // SCmd.cpp
void __fastcall SCMD_HandleSrv3F_CursorItemSkill(D2GSPacketSrv3F *pPacket);       // SCmd.cpp
void __fastcall SCMD_HandleSrv40_SetItemFlag(D2GSPacketSrv40 *pPacket);           // SCmd.cpp
void __fastcall SCMD_HandleSrv47_RefreshItem(D2GSPacketSrv47 *pPacket);           // SCmd.cpp
void __fastcall SCMD_HandleSrv48_RefreshUnit(D2GSPacketSrv48 *pPacket);           // SCmd.cpp
void __fastcall SCMD_HandleSrv53_Environment(D2GSPacketSrv53 *pPacket);           // SCmd.cpp
void __fastcall j_MEMPART_Nullsub1(void *pPacket);                                         // SCmd.cpp
void __fastcall SCMD_HandleSrv57_MonsterEnchants(D2GSPacketSrv57 *pPacket);       // SCmd.cpp
void __fastcall SCMD_HandleSrv58_AnvilPanel(D2GSPacketSrv58 *pPacket);            // SCmd.cpp
void __fastcall SCMD_HandleSrv59_AssignPlayer(D2GSPacketSrv59 *pPacket);          // SCmd.cpp
void __fastcall SCMD_HandleSrv5B_RosterAddPlayer(D2GSPacketSrv5B *pPacket);       // SCmd.cpp
void __fastcall SCMD_HandleSrv5C_RosterRemovePlayer(D2GSPacketSrv5C *pPacket);    // SCmd.cpp
void __fastcall SCMD_HandleSrv5D_QuestState(D2GSPacketSrv5D *pPacket);            // SCmd.cpp
void __fastcall SCMD_HandleSrv5E_QuestAvailability(D2GSPacketSrv5E *pPacket);     // SCmd.cpp
void __fastcall SCMD_HandleSrv92_RefreshInventory(D2GSPacketSrv92 *pPacket);      // SCmd.cpp
void __fastcall SCMD_HandleSrv5F_PortalFlags(D2GSPacketSrv5F *pPacket);           // SCmd.cpp
void __fastcall SCMD_HandleSrv60_TownPortalState(D2GSPacketSrv60 *pPacket);       // SCmd.cpp
void __fastcall SCMD_HandleSrv61_PlayCinematic(D2GSPacketSrv61 *pPacket);         // SCmd.cpp
void __fastcall SCMD_HandleSrv63_WaypointMenu(D2GSPacketSrv63 *pPacket);          // SCmd.cpp
void __fastcall SCMD_HandleSrv65_RosterUpdate(D2GSPacketSrv65 *pPacket);          // SCmd.cpp
//  ---- SCmd.cpp: missiles, trade, pets, hotkeys, the roster ----
void __fastcall SCMD_HandleSrv73_SpawnMissile(D2GSPacketSrv73 *pPacket);          // SCmd.cpp
void __fastcall SCMD_HandleSrv74_AssignCorpse(D2GSPacketSrv74 *pPacket);          // SCmd.cpp
void __fastcall SCMD_HandleSrv75_PartyRosterUpdate(D2GSPacketSrv75 *pPacket);     // SCmd.cpp
void __fastcall SCMD_HandleSrv77_TradeState(D2GSPacketSrv77 *pPacket);            // SCmd.cpp
void __fastcall SCMD_HandleSrv78_TradePartner(D2GSPacketSrv78 *pPacket);          // SCmd.cpp
void __fastcall SCMD_HandleSrv79_TradeGold(D2GSPacketSrv79 *pPacket);             // SCmd.cpp
void __fastcall SCMD_HandleSrv7A_RosterPetsOnPetPacket(D2GSPacketSrv7A *pPacket);                 // SCmd.cpp
void __fastcall SCMD_HandleSrv81_AddPet(D2GSPacketSrv81 *pPacket);                // SCmd.cpp
void __fastcall SCMD_HandleSrv7B_AssignSkillHotkey(D2GSPacketSrv7B *pPacket);     // SCmd.cpp
void __fastcall SCMD_HandleSrv7C_ReleaseItemLock(D2GSPacketSrv7C *pPacket);       // SCmd.cpp
void __fastcall SCMD_HandleSrv7D_ItemBreakRepair(D2GSPacketSrv7D *pPacket);       // SCmd.cpp
void __fastcall SCMD_HandleSrv7E_LoadActCofData(D2GSPacketSrv7E *pPacket);        // SCmd.cpp
void __fastcall SCMD_HandleSrv7F_PartyLifeUpdate(D2GSPacketSrv7F *pPacket);       // SCmd.cpp
void __fastcall SCMD_HandleSrv90_PartyPosition(D2GSPacketSrv90 *pPacket);         // SCmd.cpp
void __fastcall SCMD_HandleSrv82_PortalOwner(D2GSPacketSrv82 *pPacket);           // SCmd.cpp
void __fastcall SCMD_HandleSrv89_QuestTrigger(D2GSPacketSrv89 *pPacket);          // SCmd.cpp
void __fastcall SCMD_HandleSrv8B_SetPartyButton(D2GSPacketSrv8B *pPacket);        // SCmd.cpp
void __fastcall SCMD_HandleSrv8C_PlayerRelation(D2GSPacketSrv8C *pPacket);        // SCmd.cpp
void __fastcall SCMD_HandleSrv8D_SetPartyId(D2GSPacketSrv8D *pPacket);            // SCmd.cpp
void __fastcall SCMD_HandleSrv8E_RosterOnCorpseAssignment(D2GSPacketSrv8E *pPacket);                // SCmd.cpp
void __fastcall SCMD_HandleSrv97_WeaponSwitch(D2GSPacketSrv97 *pPacket);          // SCmd.cpp
void __fastcall SCMD_HandleSrv8F_Pong(D2GSPacketSrv8F *pPacket);                  // SCmd.cpp
//  ---- SCmd.cpp: item messages, rune words, states ----
void __fastcall SCMD_HandleSrv9C_ItemDropPickup(D2GSPacketSrv9C *pPacket);                     // SCmd.cpp
void __fastcall SCMD_HandleSrv9D_ItemEquip(D2GSPacketSrv9D *pPacket);                 // SCmd.cpp
void __fastcall SCMD_HandleSrvA6_RunesRecord(D2GSPacketSrvA6 *pPacket);           // SCmd.cpp
void __fastcall SCMD_HandleSrvA7_StateOn(D2GSPacketSrvA7 *pPacket);               // SCmd.cpp
void __fastcall SCMD_HandleSrvA8_StateOnWithStats(D2GSPacketSrvA8 *pPacket);      // SCmd.cpp
void __fastcall SCMD_HandleSrvA9_StateOff(D2GSPacketSrvA9 *pPacket);              // SCmd.cpp
//  ---- SCmd.cpp: state lists, health, monsters ----
void __fastcall SCMD_HandleSrvAA_AllStates(D2GSPacketSrvAA *pPacket);             // SCmd.cpp
void __fastcall SCMD_HandleSrvAB_UnitHealth(D2GSPacketSrvAB *pPacket);            // SCmd.cpp
void __fastcall SCMD_HandleSrvAC_SpawnMonster(D2GSPacketSrvAC *pPacket);          // SCmd.cpp
void __fastcall SCMD_HandleSrvAD_ServerMemoryProbe(D2GSPacketSrvAD *pPacket);           // SCmd.cpp
//  ---- Item.cpp: the item unit's spine -- mode, sound, teardown ----
void __fastcall ITEM_DetachItemFromUnit(D2UnitStrc *pItem, D2UnitStrc *pOwner, int32_t bForceExpire, int32_t bClampVitals);  // Item.cpp
void __fastcall ITEM_SetClientItemMode(D2UnitStrc *pItem, D2ActiveRoomStrc *pRoom, int32_t nX, int32_t nY, int32_t nItemMode, int32_t bInitStats);  // Item.cpp
D2C_Sounds __fastcall ITEM_GetDropSound(D2UnitStrc *pItem, int32_t *pnDropSfxFrame);        // Item.cpp
void __fastcall ITEM_FreeUnitData(D2UnitStrc *pItem);                               // Item.cpp
void __fastcall ITEM_OnUnitModeUpdate(D2UnitStrc *pUnit);                           // Item.cpp
void __fastcall ITEM_DoClientAction(D2C_ClientActions nAction, D2UnitStrc *pItem, int32_t bSet, int32_t nMode);  // Item.cpp
uint16_t __fastcall ITEM_CacheWeaponClassForBodyLoc(D2UnitStrc *pItem, uint8_t nBodyLoc);  // Item.cpp
D2C_Sounds __fastcall ITEM_GetDropSoundNoFrame(D2UnitStrc *pItem);                      // Item.cpp
D2C_Sounds __fastcall ITEM_GetUseSound(D2UnitStrc *pItem);                              // Item.cpp
void __cdecl ITEM_ClearWeaponClassCache();                                          // Item.cpp
//  ---- Item.cpp: the small holders of an item's client state ----
void __fastcall ITEM_RefreshUnitFromMsg(const void *pMsg);                          // Item.cpp
void __fastcall ITEM_RefreshUnitFromPacket(const void *pPacket);                    // Item.cpp
void __fastcall j_ITEM_RefreshActiveItems(D2UnitStrc *pUnit);                       // Item.cpp
void __fastcall ITEM_SetItemDisplayState(D2UnitStrc *pItem, uint16_t nX, int32_t nY, int32_t nAnimMode, int32_t bResolveRoom, int32_t nStorePage);  // Item.cpp
int32_t __fastcall ITEM_SetChargedSkillCharges(D2UnitStrc *pUnit, int32_t nSkillId, int32_t nSkillLevel, int32_t nCharges);  // Item.cpp
void __fastcall ITEM_OnItemStatUpdate(uint32_t dwItemGUID, uint8_t bSetStat, int32_t nStatId, int32_t nValue, int32_t nStatParam);  // Item.cpp
void __fastcall ITEM_OnCursorItemSkill(D2GSPacketSrv3F *pPacket);                   // Item.cpp
void __fastcall ITEM_SetItemFlagFromPacket(const void *pPacket);                    // Item.cpp
void __fastcall ITEM_ClearUninterruptableState(D2UnitStrc *pUnit);                  // Item.cpp
void __fastcall ITEM_SetUninterruptableState(D2UnitStrc *pUnit);                    // Item.cpp
void __fastcall ITEM_OnUninterruptableStateRemoved(D2UnitStrc *pUnit, int32_t nState, D2StatListStrc *pUnusedStatList);  // Item.cpp
BOOL __fastcall ITEM_IsItemLocked(D2UnitStrc *pUnit);                               // Item.cpp
//  ---- Item.cpp: the refresh hub and the socket modifiers ----
void __fastcall ITEM_RefreshActiveItems(D2UnitStrc *pUnit);                         // Item.cpp
void __fastcall ITEM_ApplyGemMods(D2UnitStrc *pItem, D2UnitStrc *pTarget, int32_t nUnused, int32_t bIgnoreBroken);  // Item.cpp
void __fastcall ITEM_OnItemRefPacket7C(D2GSPacketSrv7C *pPacket);                   // Item.cpp
void __fastcall ITEM_OnItemPairPacket7D(D2GSPacketSrv7D *pPacket);                  // Item.cpp
void __fastcall ITEM_OnPacket92(D2UnitStrc *pPlayer);                               // Item.cpp
void __fastcall ITEM_NpcMenuAckItemActionAndDropCursorItem(void *pPacket);               // Item.cpp
//  ---- Item.cpp: the first four item messages and the two creators ----
void __fastcall ITEM_ItemMsg00_PutOnGround(D2GSPacketSrv9C *pPacket);               // Item.cpp
D2UnitStrc *__fastcall ITEM_CreateItemFromItemExMsg(D2GSPacketSrv9C *pPacket, D2ItemSaveStrc *pItemSave, uint16_t nX, uint16_t nY, int32_t bAnnounce);  // Item.cpp
void __fastcall ITEM_ItemMsg01_ToCursor(D2GSPacketSrv9C *pPacket);                  // Item.cpp
void __fastcall ITEM_ItemMsg02_DropToGround(D2GSPacketSrv9C *pPacket);              // Item.cpp
void __fastcall ITEM_ItemMsg03_OnGround(D2GSPacketSrv9C *pPacket);                  // Item.cpp
D2UnitStrc *__fastcall ITEM_CreateItemFromItemUnitExMsg(D2GSPacketSrv9D *pPacket, D2ItemSaveStrc *pItemSave, uint16_t nX, uint16_t nY, int32_t bAnnounce);  // Item.cpp
//  ---- Item.cpp: the store page, the inventory grid, the cursor ----
//  ItemMsg12 returns int: it tail-returns a callee's result, which is a real return the caller ignores.
void __fastcall ITEM_ItemMsg04_ToStorePage(D2GSPacketSrv9C *pPacket);               // Item.cpp
void __fastcall ITEM_PlaceAtStorePage(uint16_t nX, uint16_t nY, int32_t nStorePage, D2UnitStrc *pPlayer, D2UnitStrc *pItem);  // Item.cpp
void __fastcall ITEM_ItemMsg05_ToInventoryGrid(D2GSPacketSrv9D *pPacket);           // Item.cpp
void __fastcall ITEM_ItemMsg0A_UpdateItem(D2GSPacketSrv9C *pPacket);                // Item.cpp
int32_t __fastcall ITEM_ItemMsg12_RecreateToCursor(D2GSPacketSrv9C *pPacket);           // Item.cpp
//  ---- Item.cpp: the body locations ----
void __fastcall ITEM_ItemMsg06_InsertIntoBodyLoc(D2GSPacketSrv9D *pPacket);         // Item.cpp
void __fastcall ITEM_ItemMsg07_SwapBodyLocItems(D2GSPacketSrv9D *pPacket);          // Item.cpp
void __fastcall ITEM_ItemMsg08_RemoveFromBodyLoc(D2GSPacketSrv9D *pPacket);         // Item.cpp
void __fastcall ITEM_ItemMsg09_EquipOnPlayer(D2GSPacketSrv9D *pPacket);             // Item.cpp
void __fastcall ITEM_UnequipSwitchedOutItem(D2GSPacketSrv9D *pPacket, D2ItemSaveStrc *pItemSave);  // Item.cpp
//  ---- Item.cpp: the weapon switch, the store, the container, the belt ----
void __fastcall ITEM_ItemMsg17_WeaponSwitch(D2GSPacketSrv9D *pPacket);              // Item.cpp
void __fastcall ITEM_ItemMsg0B0C_UpdateNpcStore(D2GSPacketSrv9C *pPacket);          // Item.cpp
void __fastcall ITEM_ItemMsg0D_SwapInContainer(D2GSPacketSrv9C *pPacket);           // Item.cpp
void __fastcall ITEM_ItemMsg0E_SetInvPage(D2GSPacketSrv9C *pPacket);                // Item.cpp
//  ---- Item.cpp: the belt, the auto-unequip, the socket ----
void __fastcall ITEM_ItemMsg0F_RemoveFromBelt(D2GSPacketSrv9C *pPacket);            // Item.cpp
void __fastcall ITEM_ItemMsg10_SwapInBelt(D2GSPacketSrv9C *pPacket);                // Item.cpp
void __fastcall ITEM_ItemMsg11_AutoUnequip(D2GSPacketSrv9D *pPacket);               // Item.cpp
void __fastcall ITEM_ItemMsg13_ItemInSocket(D2GSPacketSrv9D *pPacket);              // Item.cpp
//  ---- Item.cpp: the last three messages ----
//  ItemMsg15 returns the rebuilt item on purpose; ItemMsg16 tail-returns CURSOR_SetHeldUnit's int.
void __fastcall ITEM_ItemMsg14_Unknown(D2GSPacketSrv9D *pPacket);                   // Item.cpp
D2UnitStrc *__fastcall ITEM_ItemMsg15_UpdateStats(D2GSPacketSrv9D *pPacket);        // Item.cpp
int32_t __fastcall ITEM_ItemMsg16_Unknown(D2GSPacketSrv9D *pPacket);                    // Item.cpp

//  ---- Listbox.cpp, the list box ----
//  The bridge/pImpl pair: the twelve-byte proxy a dialog holds and the 1574774-byte body behind it. Every member is `__thiscall` in the
//  original and written `__fastcall` with an `nEdxUnused` where it takes more than `this`. The body's vtable is still the original's, so
//  everything reached through a slot -- the input handlers, Draw, AddEntry, GetEntryUserData -- is written and inert.
//  LISTBOX_UiCtrlBaseDestruct and the proxy forwarders FwdSlot1, FwdSlot13 and FwdSlot2 are the four functions every control class shares,
//  written here by position.
D2CellFileStrc *__cdecl LISTBOX_LoadScrollBarImage(void);
void __cdecl LISTBOX_UnloadScrollBarImage(void);
D2ClientListBoxStrc *__fastcall LISTBOX_ImplConstruct(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, int32_t nX, int32_t nY, int32_t nWidth, int32_t nHeight, int32_t nEntryGap, int32_t nDefaultColor, int32_t nFont, void *pfnOnSelect,
                                                      uint8_t bWideLayout);
D2ClientListBoxStrc *__fastcall LISTBOX_ImplVecDelDtor(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, uint8_t bDelete);
void __fastcall LISTBOX_UiCtrlBaseDestruct(int32_t *pControl);
int32_t __fastcall LISTBOX_ImplGetNumEntries(D2ClientListBoxStrc *pListBox);
void __fastcall LISTBOX_ImplRemoveAll(D2ClientListBoxStrc *pListBox);
void __fastcall LISTBOX_ImplUpdateScrollThumb(D2ClientListBoxStrc *pListBox);
int32_t __fastcall LISTBOX_CountFittingEntries(D2ClientListBoxStrc *pListBox);
void __fastcall LISTBOX_DrawColumnText(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, Unicode *pwszText, int32_t nX, int32_t nY, int32_t nColor);
int32_t __fastcall LISTBOX_ImplDraw(D2ClientListBoxStrc *pListBox);
void __fastcall LISTBOX_ImplAddEntry(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, const Unicode *pwsCol0, const Unicode *pwsCol1, const Unicode *pwsCol2, const Unicode *pwsCol3, const Unicode *pwsCol4, const Unicode *pwsCol5, int32_t nUserData,
                                     int32_t nColor, int32_t bUseDefaultColor, int32_t nFont);
int32_t __fastcall LISTBOX_ScrollByTrackClick(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t __fastcall LISTBOX_ImplOnMouseWheel(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t __fastcall LISTBOX_ImplOnMouseDown(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t __fastcall LISTBOX_ImplOnMouseUp(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t __fastcall LISTBOX_ImplOnArrowUp(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t __fastcall LISTBOX_ImplOnArrowDown(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t __fastcall LISTBOX_HitTestEntry(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t __fastcall LISTBOX_ImplOnMouseMove(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t __fastcall LISTBOX_ImplOnChar(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t __fastcall LISTBOX_ImplGetSelection(D2ClientListBoxStrc *pListBox);
int32_t __fastcall LISTBOX_ProxyDestruct(int32_t *pProxy, int32_t nEdxUnused);
uint32_t **__fastcall LISTBOX_ProxyVecDelDtor(uint32_t **pProxy, int32_t nEdxUnused, uint8_t bFreeObject);
void *__fastcall LISTBOX_Construct(void *pProxy, int32_t nEdxUnused, int32_t nX, int32_t nY, int32_t nWidth, int32_t nHeight, int32_t nEntryGap, int32_t nDefaultColor, int32_t nFont, void *pfnOnSelect, uint8_t bWideLayout);
int32_t __fastcall LISTBOX_UiCtrlProxyFwdSlot1(uint32_t **pProxy);
int32_t __fastcall LISTBOX_GetEntryUserData(uint32_t **pProxy, int32_t nEdxUnused, int32_t nEntryIndex);
int32_t __fastcall LISTBOX_UiCtrlProxyFwdSlot13(uint32_t **pProxy);
int32_t __fastcall LISTBOX_UiCtrlProxyFwdSlot2(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);

//  ---- Gfx.cpp (ENGINE\Gfx.cpp) ----

//  D2Client.0x6FB5B0F0: a box from a corner and a size, handed to D2Gfx's DrawBox as two corners.
void __fastcall GFX_DrawFilledRect(int32_t nX, int32_t nY, int32_t nWidth, int32_t nHeight, uint8_t nColor, int32_t eDrawMode);
//  Gfx.cpp: an overlay tested, drawn at a position, ticked, an aura handed on, an entry removed; a mode's COF and animation node found, and
//  loaded (0x6FB57CD0..0x6FB58713).
BOOL __fastcall GFX_HasOverlay(D2UnitStrc *pUnit, int32_t nOverlayId);
void __fastcall GFX_DrawOverlayAt(D2UnitStrc *pUnit, int32_t nOverlayId, int32_t nX, int32_t nY);
void __fastcall GFX_TickUnitOverlays(D2UnitStrc *pUnit);
void __fastcall GFX_CycleAuraOverlay(D2UnitStrc *pUnit, D2ClientUnitGfxInfoStrc *pGfxInfo, D2ClientGfxEntryStrc *pEntry);
void __fastcall GFX_RemoveGfxEntry(D2UnitStrc *pUnit, D2ClientGfxEntryStrc *pEntry);
void *__fastcall GFX_GetUnitCofData(D2UnitStrc *pUnit, int32_t nAnimMode);
D2ClientCofNodeStrc *__fastcall GFX_GetUnitAnimNode(D2UnitStrc *pUnit, int32_t nWeaponClassCode, int32_t nAnimMode);
D2ClientCofNodeStrc *__fastcall GFX_RebuildUnitGfx(D2UnitStrc *pUnit, int32_t nWeaponClassCode, int32_t nAnimMode);
//  Gfx.cpp: a mode's COF node got or built, a path's title, a COF loaded into a cache, a node freed by its COF, a monster's and an object's
//  mode loaded, the caches set up and freed, a gfx block made and freed, the animation list freed whole or but its death nodes
//  (0x6FB58720..0x6FB5948D).
void *__fastcall GFX_GetUnitCofNode(D2UnitStrc *pUnit, void **ppReplacedCofData, int32_t nAnimMode);
void __fastcall GFX_ExtractFileTitle(const char *szPath, char *szTitle);
D2ClientCofCacheNodeStrc *__fastcall GFX_LoadCofFile(D2ClientCofCacheStrc *pCache, const char *pszFileName);
BOOL __fastcall GFX_FreeUnitAnimNodeByCof(D2UnitStrc *pUnit, D2ClientCofCacheNodeStrc *pCofFile);
void __fastcall GFX_LoadMonsterModeGfx(D2UnitStrc *pUnit, int32_t nMode);
void __fastcall GFX_LoadObjectModeGfx(D2UnitStrc *pUnit, int32_t nMode);
void __fastcall GFX_InitCelCache(int32_t nCofMemoryBudget);
void __cdecl GFX_FreeCelCache(void);
D2ClientUnitGfxInfoStrc *__cdecl GFX_AllocUnitGfxInfo(void);
void __fastcall GFX_FreeUnitGfxInfo(D2UnitStrc *pUnit);
void __fastcall GFX_FreeUnitAnimList(D2UnitStrc *pUnit);
void __fastcall GFX_FreeUnitAnimListButDeath(D2UnitStrc *pUnit);
//  Gfx.cpp: a one-cel unit's shadow, a unit's shadow, a one-cel unit's hit flash and the flash's colour (0x6FB59490..0x6FB59F1A).
void __fastcall GFX_DrawUnitCelShadow(D2UnitStrc *pUnit);
void __fastcall GFX_DrawUnitShadow(D2UnitStrc *pUnit);
void __fastcall GFX_DrawUnitCelHilight(D2UnitStrc *pUnit);
uint8_t __fastcall GFX_GetUnitHilightPaletteIndex(D2UnitStrc *pUnit);
//  Gfx.cpp: a unit's hit flash by layers, an item at a screen position, a unit's overlays (0x6FB59F20..0x6FB5A71B).
void __fastcall GFX_DrawUnitHilight(D2UnitStrc *pUnit);
void __fastcall GFX_DrawInventoryItem(D2UnitStrc *pItem, int32_t nX, int32_t nY);
void __fastcall GFX_DrawUnitOverlays(D2UnitStrc *pUnit, uint32_t nColor, int32_t bPreDrawLayer, int32_t nX, int32_t nY, int32_t bShiftUp8);
//  Gfx.cpp: a unit's cels drawn (0x6FB5A720..0x6FB5B0EA).
int32_t __fastcall GFX_DrawUnitCels(D2UnitStrc *pUnit, int32_t nColor, int32_t nX, int32_t nY, int32_t bGhostly, int32_t bDrawCel);
//  Gfx.cpp: a unit's graphics reset, its mode flags set, the preload sweep, one mode loaded, one monster mode preloaded, the particle block
//  got and stored (0x6FB5B120..0x6FB5BA60).
void __fastcall GFX_ResetUnitGfx(D2UnitStrc *pUnit);
void __fastcall GFX_SetAllUnitModesLoaded(D2UnitStrc *pUnit, uint8_t bLoaded);
void __fastcall GFX_PreloadVisibleUnits(int32_t bAsync);
int32_t __fastcall GFX_AsyncLoadBlock(D2UnitStrc *pUnit, int32_t nAnimMode, int32_t bAsync);
void __fastcall GFX_PreloadAnimMode(D2UnitStrc *pUnit, int32_t nAnimMode, int32_t bAsync);
D2ClientParticleStrc *__fastcall GFX_GetUnitParticles(D2UnitStrc *pUnit);
void __fastcall GFX_SetUnitParticles(D2UnitStrc *pUnit, D2ClientParticleStrc *pParticle);
//  Gfx.cpp: an overlay added, a flagEx test, one and every overlay removed, the two palette shifts stored and read (0x6FB5BA70..0x6FB5C075).
void __fastcall GFX_AddOverlay(D2UnitStrc *pUnit, D2_OverlayIds nOverlayId, D2C_OverlayTypes nKind, int32_t nRandomStartFrames, int32_t nTargetOrStateId, int32_t nX, int32_t nY, int32_t nRateJitter);
int32_t __fastcall GFX_TestFlagEx(D2UnitStrc *pUnit, int32_t nFlagEx);
void __fastcall GFX_RemoveOverlay(D2UnitStrc *pUnit, int32_t nUnused, D2_OverlayIds nOverlayId);
void __fastcall GFX_RemoveAllOverlays(D2UnitStrc *pUnit);
void __fastcall GFX_SetPaletteShift1(D2ClientUnitGfxInfoStrc *pGfxData, int32_t nShift);
int32_t __fastcall GFX_GetPaletteShift1(D2ClientUnitGfxInfoStrc *ptInfo);
void __fastcall GFX_SetPaletteShift2(D2ClientUnitGfxInfoStrc *pGfxData, int32_t nShift);
int32_t __fastcall GFX_GetPaletteShift2(D2ClientUnitGfxInfoStrc *ptInfo);
//  Gfx.cpp: a unit's current mode loaded, its mode-loaded flags cleared (0x6FB5C080..0x6FB5C3BE).
void __fastcall GFX_RebuildUnitAnimForMode(D2UnitStrc *pUnit, int32_t nUnused);
void __fastcall GFX_ClearUnitModesLoaded(D2UnitStrc *pUnit, D2Composits nComponent);
//  Gfx.cpp: a unit component's cel offsets, a point against a unit's COF box, the COF box on screen (0x6FB5C3C0..0x6FB5C9DB).
void __fastcall GFX_GetUnitCel(D2UnitStrc *pUnit, D2Composits nComponent, int32_t *pnOffsetX, int32_t *pnOffsetY);
BOOL __fastcall GFX_HitTestUnitCof(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nHitX, int32_t nHitY);
BOOL __fastcall GFX_IsUnitCofOnScreen(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nUnused, int32_t bShadow);
//  Gfx.cpp: an act's COF block loaded and the other cache freed, a monstats2.txt row, a monster's COF files, the COF name's hash, a monster
//  mode's COF path (0x6FB5C9E0..0x6FB5D288).
void __fastcall GFX_LoadActCofData(uint8_t nActIndex);
void __cdecl GFX_FreeCofCache(void);
D2MonStats2Txt *__fastcall GFX_GetMonStats2Record(int32_t nMonStats2Id);
void __fastcall GFX_AddMonsterCofFiles(int32_t nMonsterId, BOOL bValidateSprites);
uint32_t __fastcall GFX_HashCofName(const char *pszFileName);
void __fastcall GFX_BuildMonsterCofName(D2MonStatsTxt *pMonStatsTxt, int32_t nMonsterId, int32_t nMonModeIndex, int32_t *pnWeaponClassCode, LPSTR pszOut);
//  Gfx.cpp: the preload when loading completes, a monster's modes, one and every player mode preloaded (0x6FB5D290..0x6FB5DA35); the memory
//  figures, a monster's preload record, one unit preloaded (0x6FB5DA70..0x6FB5DD63).
void __fastcall GFX_PreloadOnLoadComplete(uint8_t nAct);
void __fastcall GFX_PreloadAllAnimModes(D2UnitStrc *pUnit, int32_t bAsync);
void __fastcall GFX_PreloadPlayerMode(D2UnitStrc *pUnit, int32_t nAnimMode, int32_t bAsync);
void __fastcall GFX_PreloadAllPlayerModes(D2UnitStrc *pUnit, int32_t bAsync);
//  D2Client.0x6FB5DA40: a living player given CLTACTION_NEUTRAL.
void __fastcall GFX_StopIfAlive(D2UnitStrc *pUnit);
int32_t __cdecl GFX_GetGfxMemoryUsed(void);
int32_t __cdecl GFX_GetCofMemoryUsed(void);
int32_t __cdecl GFX_GetCofMemoryBudget(void);
D2ClientModePreloadStrc *__fastcall GFX_GetMonsterPreloadEntry(int32_t nMonsterId);
void __fastcall GFX_PreloadUnit(D2UnitStrc *pUnit);

//  ---- mods.cpp: the five short helpers ----
//  D2Client.0x6FAF12C0 / 0x6FAF13C0 / 0x6FAF3160 / 0x6FAF32B0 / 0x6FAF33C0: the blunt weapon's implicit "+50% Damage to Undead", a rune's
//  property set as text, the magic-mods block with its last line re-joined round a separator, and the two set-bonus appenders. All five
//  void: no caller reads eax.
void __fastcall MODS_AppendBluntUndeadBonusText(D2UnitStrc *pUnit, Unicode *wszOut, int32_t nMaxLen);
void __fastcall MODS_AppendPropertySetText(D2UnitStrc *pUnit, D2GemsTxt *pGemsTxt, int32_t nPropSet, Unicode *wszOut, int32_t nMaxLen, const Unicode *wszSep);
void __fastcall MODS_AppendMagicModsText(D2UnitStrc *pUnit, Unicode *wszOut, int32_t nMaxLen, int32_t bAppendNewline, const Unicode *wszSep);
void __fastcall MODS_AppendSetBonusText(D2UnitStrc *pPlayer, D2UnitStrc *pItem, Unicode *wszOut, int32_t nMaxLen);
void __fastcall MODS_AppendStateModsText(D2UnitStrc *pUnit, D2UnitStrc *pItem, Unicode *wszOut, int32_t nMaxLen);

//  ---- mods.cpp: the socket filler and the damage lines ----
//  D2Client.0x6FAF1480 / 0x6FAF3460: what a gem or rune grants in each of four slots (three asserts that exit), and the damage lines the
//  property list draws itself -- non-zero tells MODS_BuildStatsText the stat is handled.
void __fastcall MODS_BuildSocketFillerText(D2UnitStrc *pItem, Unicode *wszOut, int32_t nMaxLen);
int32_t __fastcall MODS_FormatElementalDamageText(D2ClientDamageGroupStrc *pDamageGroups, D2C_ItemStats nStatId, Unicode *pwszDest);

//  ---- mods.cpp: the property list ----
//  D2Client.0x6FAF19C0: an elixir's lines, or the item's stat list under a state, merged with a second state's and its socketed items', then
//  its damage groups and a line per described stat. nMaxLen is not read.
void __fastcall MODS_BuildStatsText(D2UnitStrc *pUnit, Unicode *wszOut, int32_t nMaxLen, int32_t bAppendUndeadBonus, D2C_States nState, D2C_States nSourceState, int32_t nFlag, int32_t bAppendNewline);

//  ---- mods.cpp: one property line ----
//  D2Client.0x6FAF21C0: a described stat's value as its line, by the stat's itemstatcost.txt description columns -- 28 descfuncs, the
//  descgroup scan and the descstr2 suffix. wszOut is emptied first. nLayer is the stat's parameter, not a layer.
BOOL __fastcall MODS_FormatPropertyLine(D2UnitStrc *pUnit, D2StatListStrc *pStatList, D2C_ItemStats nStatId, int32_t nLayer, int32_t nValue, Unicode *wszOut);


//  ---- SkillDesc.cpp ----
//  Declared in the order the unit was written, not in address order.
void __fastcall SKILLDESC_AttFunc1(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnAttackRatingA, int32_t *pnColorA, int32_t *pnAttackRatingB, int32_t *pnColorB);
int32_t __fastcall SKILLDESC_ComputeAttackRating(D2UnitStrc *pUnit, int32_t *pnAttackRating, int32_t *pnColor, int32_t nSkillLevel, D2SkillStrc *pSkill, D2UnitStrc *pWeapon);
void __fastcall SKILLDESC_AttFunc5(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnAttackRatingA, int32_t *pnColorA, int32_t *pnAttackRatingB, int32_t *pnColorB);
void __fastcall SKILLDESC_AttFunc2(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnAttackRatingA, int32_t *pnColorA, int32_t *pnAttackRatingB, int32_t *pnColorB);
int32_t __fastcall SKILLDESC_AttackRatingCallback(D2UnitStrc *pUnit, int32_t *pnValue, int32_t *pnColor, int32_t nUnusedA, int32_t nUnusedB, int32_t nSkillLevel, D2SkillStrc *pSkill, int32_t nUnusedC, D2UnitStrc *pWeapon);
int32_t __fastcall SKILLDESC_ForEachWieldedWeapon(D2UnitStrc *pUnit, int32_t *pnValueA, int32_t *pnColorA, int32_t nArgA, int32_t *pnValueB, int32_t *pnColorB, int32_t nArgB, int32_t nShared, void *pfnCompute, int32_t nExtraA, int32_t nExtraB);
D2UnitStrc *__fastcall SKILLDESC_PrepareOffHandWeapon(D2UnitStrc *pUnit);
D2UnitStrc *__fastcall SKILLDESC_PrepareMainHandWeapon(D2UnitStrc *pUnit);
void __fastcall SKILLDESC_AttFunc3(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnAttackRatingA, int32_t *pnColorA, int32_t *pnAttackRatingB, int32_t *pnColorB);
void __fastcall SKILLDESC_AttFunc4(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnAttackRatingA, int32_t *pnColorA, int32_t *pnAttackRatingB, int32_t *pnColorB);
void __fastcall SKILLDESC_DrawTwoLines(Unicode *pwszLine1, Unicode *pwszLine2, int32_t nX, int32_t nY, int32_t nRight, int32_t nColor1, int32_t nColor2);
void __fastcall SKILLDESC_DamFunc6(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc7(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc1(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc20(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc19(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc2(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc21(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc3(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc4(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc22(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc5(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc24(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc23(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc13(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc8(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc9(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc10(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc11(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc12(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc14(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc15(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc16(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc17(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
void __fastcall SKILLDESC_DamFunc18(D2UnitStrc *pUnit, D2SkillStrc *pSkill, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nLeft, int32_t nY, int32_t nRight);
int32_t __fastcall SKILLDESC_FormatInt(struct Unicode *pwszOut, int32_t nValue, int32_t bSigned);
struct Unicode *__fastcall SKILLDESC_FormatDamageRange(struct Unicode *wszOut, int32_t nMin, int32_t nMax, int32_t bSigned, int32_t bAllowK);
int32_t __fastcall SKILLDESC_FormatRangeOrValue(struct Unicode *pwszOut, int32_t nMin, int32_t nMax, int32_t bSigned);
int32_t __fastcall SKILLDESC_AddWeaponPhysDamage(D2UnitStrc *pUnit, int32_t *pnMin, int32_t *pnMax, int32_t *pnColor, int32_t nExtraDamagePercent, int32_t nFlatBonus, D2SkillStrc *pSkill, int32_t nSrcDamOverride,
                                             D2UnitStrc *pWeapon, int32_t bUseItemOwnStats);
int32_t __cdecl SKILLDESC_GetChargeUpDamagePercent(void);
int32_t __fastcall SKILLDESC_AddItemElemDamage(D2UnitStrc *pUnit, int32_t *pnMin, int32_t *pnMax, int32_t *pnColor, int32_t nUnused, D2SkillStrc *pSkill);
int32_t __fastcall SKILLDESC_AddChargeUpElemDamage(D2UnitStrc *pUnit, int32_t *pnMin, int32_t *pnMax, int32_t *pnColor);
int32_t __fastcall SKILLDESC_AddElemDamageByType(D2UnitStrc *pUnit, int32_t *pnMin, int32_t *pnMax, uint8_t nEType);
int32_t __fastcall SKILLDESC_AddTotalDamageCallback(D2UnitStrc *pUnit, int32_t *pnMin, int32_t *pnMax, int32_t *pnColor, int32_t nExtraDamagePercent, int32_t nFlatBonus, D2SkillStrc *pSkill, int32_t nSrcDamOverride, D2UnitStrc *pWeapon);
int32_t __fastcall SKILLDESC_AddElemDamageAndColor(D2UnitStrc *pUnit, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t *pnMinTotal, int32_t *pnMaxTotal, int32_t *pnColor);
int32_t __fastcall SKILLDESC_ApplyFireMasteryToRange(D2SkillsTxt *pSkillsTxt, int32_t *pnMin, int32_t *pnMax);
int32_t __fastcall SKILLDESC_ApplyLightningMasteryToRange(D2SkillsTxt *pSkillsTxt, int32_t *pnMin, int32_t *pnMax);
void __fastcall SKILLDESC_DrawDualWieldDamage(D2UnitStrc *pUnit, int32_t nShared, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, void *pfnCompute, int32_t nExtraA, int32_t nExtraB, int32_t nX, int32_t nY, int32_t nRight, int32_t bAddElemDamage);
int32_t __fastcall SKILLDESC_ComputeThrowDamage(D2UnitStrc *pUnit, int32_t *pnMin, int32_t *pnMax, int32_t *pnColor, int32_t nExtraDamagePercent, int32_t nUnusedA, D2SkillStrc *pSkill, int32_t nUnusedB, D2UnitStrc *pWeapon);
void __fastcall SKILLDESC_DrawDamageValue(int32_t nUnused, int32_t nMin, int32_t nMax, int32_t nColor, int32_t nLeft, int32_t nY, int32_t nRight);
int32_t __fastcall SKILLDESC_BuildDescLine(D2UnitStrc *pPlayer, struct Unicode *pwszText, D2SkillsTxt *pSkillsTxt, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nCharges, int32_t nLine, int32_t nColor);
int32_t __fastcall SKILLDESC_FormatFixedPoint(struct Unicode *pwszOut, int32_t nWhole, int32_t nFraction, int32_t bSigned);
int32_t __fastcall SKILLDESC_FormatFixedPointOrInt(struct Unicode *pwszOut, int32_t nWhole, int32_t nFraction, int32_t bSigned);
struct Unicode *__fastcall SKILLDESC_FormatFracRange(struct Unicode *pwszOut, int32_t nMinInt, int32_t nMinFrac, int32_t nMaxInt, int32_t nMaxFrac, int32_t bSigned);
int32_t __fastcall SKILLDESC_AppendFramesAsSeconds(struct Unicode *pwszText, int32_t nStrIdx, int32_t nFrames, int32_t bSigned);
int32_t __fastcall SKILLDESC_AppendScaledDamageRange(struct Unicode *pwszText, int32_t nStrIdxA, int32_t nStrIdxB, int32_t nMin, int32_t nMax, int32_t nFracScale);
int32_t __fastcall SKILLDESC_AppendTextLine(struct Unicode *pwszText, int32_t nStrIdx);
int32_t __fastcall SKILLDESC_AppendTextThenValue(struct Unicode *pwszText, int32_t nStrIdx, int32_t nValue, int32_t bSigned);
int32_t __fastcall SKILLDESC_AppendFixedPointValue(struct Unicode *pwszText, int32_t nStrIdxA, int32_t nStrIdxB, int32_t nValue, int32_t nFracScale, int32_t bSigned);
int32_t __fastcall SKILLDESC_AppendIntAndFraction(struct Unicode *pwszText, int32_t nStrIdx, int32_t nInt, int32_t nFrac, int32_t bSigned);
int32_t __fastcall SKILLDESC_AppendValueThenText(struct Unicode *pwszText, int32_t nStrIdx, int32_t nValue, int32_t bSigned);
int32_t __fastcall SKILLDESC_AppendValueThenTextPair(struct Unicode *pwszText, int32_t nStrIdxA, int32_t nStrIdxB, int32_t nValue, int32_t bSigned);
int32_t __fastcall SKILLDESC_AppendValueBetweenTexts(struct Unicode *pwszText, int32_t nStrIdxA, int32_t nStrIdxB, int32_t nValue, int32_t bSigned);
int32_t __fastcall SKILLDESC_AppendRangeBetweenTexts(struct Unicode *pwszText, int32_t nStrIdxA, int32_t nStrIdxB, int32_t nMin, int32_t nMax, int32_t bSigned);
int32_t __fastcall SKILLDESC_AppendDamageRangeLine(struct Unicode *pwszText, int32_t nStrIdx, int32_t nMin, int32_t nMax, int32_t bSigned, int32_t bTrailingSpace);
int32_t __fastcall SKILLDESC_FormatChargesText(struct Unicode *wszOut, int32_t nCharges);
int32_t __fastcall SKILLDESC_AppendElemLengthLine(D2UnitStrc *pUnit, struct Unicode *pwszText, D2C_Skills nSkillId, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel);
int32_t __fastcall SKILLDESC_AppendElemDamageLine(struct Unicode *pwszText, int32_t nMin, int32_t nMax, int32_t nEType, int32_t nStrIdxOverride);
int32_t __fastcall SKILLDESC_AppendSkillElemDamageLine(D2UnitStrc *pUnit, struct Unicode *pwszText, D2C_Skills nSkillId, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nStrIdxOverride);
int32_t __fastcall SKILLDESC_AppendPhysDamageLine(D2UnitStrc *pUnit, struct Unicode *pwszText, D2C_Skills nSkillId, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nPercentBonus, int32_t nFlatBonus);
int32_t __fastcall SKILLDESC_AppendToHitLine(D2UnitStrc *pUnit, struct Unicode *pwszText, D2C_Skills nSkillId, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel);
int32_t __fastcall SKILLDESC_AppendAveragePerSecondLine(struct Unicode *pwszText, int32_t nMin, int32_t nMax, int32_t nStrIdx);
int32_t __fastcall SKILLDESC_AppendValueThenTextLine(struct Unicode *pwszText, int32_t nStrIdxLabel, int32_t nStrIdxSuffix, int32_t nValue, int32_t bSigned, int32_t bPercent);
int32_t __fastcall SKILLDESC_AppendValueOverValueLine(struct Unicode *pwszText, int16_t nStrIdxLabel, int32_t nStrIdxSuffix, int32_t nValue, int32_t nMax, int32_t bSigned);
int32_t __fastcall SKILLDESC_AppendTextPairLine(struct Unicode *pwszText, int32_t nStrIdxLabel, int32_t nStrIdxText);
int32_t __fastcall SKILLDESC_AppendTextThenValueLine(struct Unicode *pwszText, int32_t nStrIdxLabel, int32_t nStrIdxText, int32_t nValue, int32_t bSigned, int32_t bPercent);
int32_t __fastcall SKILLDESC_AppendSprintfLine(struct Unicode *pwszText, int32_t nStrIdxFormat, int32_t nValue);
int32_t __fastcall SKILLDESC_ValidateMonsterId(D2C_MonsterIds nMonsterId);
void __fastcall SKILLDESC_DrawQuantityHint(int32_t nQuantity, int32_t nStringId);
int32_t __fastcall SKILLDESC_GetLinearParamByLevel(int32_t nSkillId, int32_t nSkillLevel);
void __fastcall SKILLDESC_SkillsBuildSkillLevelText(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nCharges, int32_t nIconLeft, int32_t nIconBottom);
void __fastcall SKILLDESC_SkillsBuildSkillRequirementText(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nCharges, int32_t nIconLeft, int32_t nIconBottom);
void __fastcall SKILLDESC_SkillsBuildFinishingMoveText(D2UnitStrc *pUnit, int32_t nSkillId, int32_t nSkillLevel, int32_t nCharges, int32_t nTextColor, int32_t nX, int32_t nY);
void __fastcall SKILLDESC_DrawSkillInfo(D2UnitStrc *pUnit, D2SkillStrc *pSkill, int32_t nLayoutRow);
void __fastcall SKILLDESC_DrawBothSkillInfos(D2UnitStrc *pUnit);
int32_t __fastcall SKILLDESC_GetAttackRatingForHand(D2UnitStrc *pUnit, int32_t bLeftHand);


//  ---- spellsel.cpp ----
void __cdecl SPELLSEL_Init();
void __cdecl SPELLSEL_Shutdown();
D2CellFileStrc *__fastcall SPELLSEL_GetSkillIconImage(int16_t *pnSkillId);
void __fastcall SPELLSEL_SetMiniSkillMode(int32_t nMode);
int32_t __fastcall SPELLSEL_GetSkillIconState(D2UnitStrc *pUnit, D2SkillStrc *pSkill);
void __fastcall SPELLSEL_DrawSkillChargesTooltip(int16_t *pnSkillId, int32_t nTextColor, int32_t nCharges, int32_t nX, int32_t nY);
void __fastcall SPELLSEL_DrawFinishingMoveTooltip(int16_t *pnSkillId, int32_t nTextColor, int32_t nSkillLevel, int32_t nCharges, int32_t nX, int32_t nY);
int32_t __fastcall SPELLSEL_SkillDescGetSkillQuantity_Always0(D2UnitStrc *pUnit, int32_t nSkillId);
void __fastcall SPELLSEL_SetHotkeySlot(int32_t nSlot, int32_t nSkillId, int32_t nMode, D2UnitGUID nOwnerGUID);
void __fastcall SPELLSEL_ControlsDrawBoundKeyLabel(D2HotkeyIndices eCmd, int32_t nX, int32_t nY, int32_t nColor);
void __cdecl SPELLSEL_Nullsub2();
void __fastcall SPELLSEL_DrawHotkeyLabelMode1(int16_t *pnSkillId, D2UnitGUID nOwnerGUID, int32_t nX, int32_t nY);
void __fastcall SPELLSEL_DrawHotkeyLabelMode0(int16_t *pnSkillId, D2UnitGUID nOwnerGUID, int32_t nX, int32_t nY);
void __fastcall SPELLSEL_DrawSkillButtonIcon(D2CellFileStrc *hSkillIconImage, int16_t *pnSkillId, D2UnitGUID nOwnerGUID, int32_t nCharges, int32_t nPaletteShift, int32_t nXPos, int32_t nYPos, int32_t bLeftHand);
void __fastcall SPELLSEL_DrawSelectorSkillIcon(int16_t *pnSkillId, D2UnitGUID nOwnerGUID, int32_t nValue, int32_t nPaletteShift, int32_t nXPos, int32_t nYPos, int32_t bGrowRight);
void __cdecl SPELLSEL_DrawBothPanels();
void __fastcall SPELLSEL_DrawPanelSkills(D2UnitStrc *pPlayer, int32_t nStartX, int32_t bGrowRight, int32_t bHoverTextPass, int32_t bSkipIconDraw);
void __fastcall SPELLSEL_CycleSkill(int32_t nDirection);
void __cdecl SPELLSEL_UnbindEmptyScrollHotkeys();
void __fastcall SPELLSEL_BindHoveredSkillToSlot(int32_t nSlot);
void __fastcall SPELLSEL_SkillsSelectHotkeySlot(int32_t nSlot);
//  The spinner; `__thiscall` as `__fastcall` with nEdxUnused.
int32_t *__fastcall SPELLSEL_UiSpinnerConstruct(D2ClientSpinnerStrc *pSpinner, int32_t nEdxUnused, int32_t nX, int32_t nY, int32_t bHorizontal, void *pfnOnChange);
D2ClientSpinnerStrc *__fastcall SPELLSEL_UiSpinnerVecDelDtor(D2ClientSpinnerStrc *pSpinner, int32_t nEdxUnused, uint8_t bFreeObject);
void __fastcall SPELLSEL_UiSpinnerDestruct(D2ClientSpinnerStrc *pSpinner);
void __fastcall SPELLSEL_UiSpinnerDraw(D2ClientSpinnerStrc *pSpinner);
int32_t __fastcall SPELLSEL_UiSpinnerGetPressedArrow(D2ClientSpinnerStrc *pSpinner);
int32_t __fastcall SPELLSEL_UiSpinnerTickAutoRepeat(D2ClientSpinnerStrc *pSpinner);
int32_t __fastcall SPELLSEL_UiSpinnerOnMouseWheel(D2ClientSpinnerStrc *pSpinner, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t __fastcall SPELLSEL_UiSpinnerOnMouseDown(D2ClientSpinnerStrc *pSpinner, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t __fastcall SPELLSEL_UiSpinnerOnMouseUp(D2ClientSpinnerStrc *pSpinner, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
void __fastcall SPELLSEL_UiSpinnerProxyDestruct(int32_t *pProxy);
uint32_t **__fastcall SPELLSEL_UiSpinnerProxyVecDelDtor(uint32_t **pProxy, int32_t nEdxUnused, uint8_t bFreeObject);
int32_t *__fastcall SPELLSEL_UiSpinnerProxyConstruct(int32_t *pProxy, int32_t nEdxUnused, int32_t nX, int32_t nY, int32_t bHorizontal, void *pfnOnChange);
int32_t __fastcall SPELLSEL_UiCtrlProxyFwdSlot3(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg);
int32_t __fastcall SPELLSEL_UiCtrlProxyFwdSlot12Arg(uint32_t **pProxy, int32_t nEdxUnused, int32_t nUnused);


//  ---- Quest.cpp: the QUEST_ functions of the module ----
//  The first seven below are QuestLog.cpp's by address; Quest.cpp keeps the seven from QUEST_ShouldPlayAreaEntrySpiel. The two record
//  accessors and the secondary copy are in npcmenu.cpp, the quest-menu string and the cairn stone order command in text.cpp.
void __cdecl QUESTLOG_QuestResetAllState();
void __cdecl j_QUESTLOG_QuestUnloadPanelImages();
void __cdecl QUESTLOG_QuestUnloadPanelImages();
void __fastcall QUESTLOG_QuestOnQuestLogStateCmd(uint8_t *ptCmd);
void __fastcall QUESTLOG_QuestOnQuestStatePacket(int32_t nUnused, D2GSPacketSrv5D *pPacket);
void __fastcall QUESTLOG_QuestMakeCainUntargetable(D2UnitStrc *pUnit);
void __fastcall QUESTLOG_QuestMakeFrozenAnyaUntargetable(D2UnitStrc *pUnit);
BOOL __fastcall QUEST_ShouldPlayAreaEntrySpiel(D2Quests nQuestKey);
void __cdecl QUEST_PreloadDurielCof();
void __cdecl QUEST_PreloadAncientStatueCof();
void __fastcall QUEST_ArmZooEffect(int32_t nValue);
uint8_t __cdecl QUEST_IsZooArmed();
bool __cdecl QUEST_ZooChickensPresent();
void __fastcall QUEST_SpawnZooChickens(int32_t nXPos, int32_t nYPos);


//  ---- QuestLog.cpp: the quest log's state ----
//  The three resets the UI layer calls, the speech callback the popup layer fires, the act tab and the close that tells the server what has
//  been read. The two QUESTLOG_ mouse handlers and the three QUESTSCREEN_ ones are declared with the handler tables above.
void __cdecl QUESTLOG_ClearPanelState(void);
void __cdecl QUESTLOG_ClearEntryState(void);
void __cdecl QUESTLOG_ClearActSelection(void);
int32_t __fastcall QUESTLOG_OnSpeechFinished(uint32_t nContext, uint32_t nDismissed, uint32_t bDismissedByMouse);
void __fastcall QUESTLOG_SetActTab(int32_t nAct);
void __cdecl QUESTLOG_AcknowledgeAndClose(void);

//  ---- QuestLog.cpp: the panel's lifecycle ----
//  Its six images, the toggle every opener goes through, and the body of server packet 0x52.
void __cdecl QUESTLOG_LoadPanelImages(void);
void __fastcall QUESTLOG_TogglePanel(int32_t bCheckMousePos);
void __fastcall QUESTLOG_ApplyQuestListPacket(D2GSPacketSrv52 *pPacket);

//  ---- QuestLog.cpp: which quest an act page opens on ----
//  Which quest an act page opens on, and the frame drawn around it.
void __fastcall QUESTLOG_SelectActPage(int32_t nAct, int32_t bClearSelections);
void __cdecl QUESTLOG_DrawFrameAndTitle(void);

//  ---- QuestLog.cpp: the quest display info ----
//  The one that decides what a quest says: it fills a whole D2QuestLogEntryStrc from the two quest records and the text block.
void __fastcall QUESTLOG_BuildQuestDisplayInfo(D2QuestLogEntryStrc *pEntry);

//  ---- QuestLog.cpp: the quest screen, drawn once a frame ----
void __cdecl QUESTLOG_Draw(void);

//  ---- Party.cpp: the row list and the panel draw ----
//  The row list's free and build, the party panel's draw and four of its helpers. PARTY_OnLButtonDown/Up are declared with the handler tables.
void __cdecl PARTY_FreeRowList();
void __cdecl PARTY_BuildRowList();
void __cdecl PARTY_DrawPanel();
void __fastcall PARTY_DrawCel(D2CellFileStrc *hImage, int32_t nX, int32_t nY, int32_t nFrame);
void __fastcall PARTY_DrawLevelLabel(int32_t nLevel, int32_t nY, int32_t nColor);
void __fastcall PARTY_DrawNameLabel(const char *pszName, int32_t nY, int32_t nColor, int32_t bInParty);
void __fastcall PARTY_DrawClassLabel(int32_t nCharClass, int32_t nY, int32_t nColor);

//  ---- Party.cpp: the other draw helpers and the hit test ----
void __fastcall PARTY_DrawButtonLabel(D2RosterUnitStrc *pUnusedEntry, D2C_PartyButtons nButtonKind, int32_t nY, int32_t bSameParty, int16_t nPartyId);
void __fastcall PARTY_DrawRowButtons(D2ClientPartyRowStrc *pRow, int32_t nUnusedY);
void __fastcall PARTY_DrawAreaLabel(D2ClientPartyRowStrc *pRow, int32_t nY, int32_t nColor);
void __cdecl PARTY_DrawPartyGroupDividers();
BOOL __fastcall PARTY_IsMouseOverRect(int32_t nX, int32_t nY, int32_t nWidth, int32_t nHeight);

//  ---- Party.cpp: the hints ----
//  Three of the hint's four: the leave and first-button hints, the backdrop and the two-line drawer PARTY_DrawHoverHint calls.
void __cdecl PARTY_DrawControlButtonHints();
void __fastcall PARTY_DrawHintBackdrop(D2C_StringIndices nTblLine1, D2C_StringIndices nTblLine2, int32_t nCenterX, int32_t bTwoLines);
void __fastcall PARTY_DrawHintLines(D2C_StringIndices nTblLine1, D2C_StringIndices nTblLine2, int32_t nCenterX, int32_t nUnused, int32_t nY1, int32_t nY2, int32_t nColor);

//  ---- Party.cpp: the hover hints and the image loaders ----
void __cdecl PARTY_DrawHoverHint();
void __cdecl PARTY_LoadImages();
void __cdecl PARTY_UnloadImages();

//  ---- Roster.cpp ----
//  D2Client.0x6FAAEC80..0x6FAAF5F2, GAME\Roster.cpp: packets 0x5B, 0x75 and 0x5C's roster changes, the teardown, the iterator and the display
//  name.
void __fastcall ROSTER_AddPlayer(uint32_t dwUnitGUID, int32_t nClass, const char *szName, int16_t nLevel, int16_t nPartyId, int16_t nUnused, D2C_PartyButtons eButton, int16_t nGuildFlags, const char *szGuildTag, const char *szGuildName);
int32_t __fastcall ROSTER_EntryHasCorpse(D2RosterUnitStrc *pRosterEntry, uint32_t dwCorpseId);
void __fastcall ROSTER_UpdatePartyState(D2UnitGUID dwUnitId, int16_t nPartyId, int16_t nLevel, int16_t nUnusedFlags, D2C_PartyButtons eButton);
void __fastcall ROSTER_RemovePlayer(uint32_t dwPlayerGUID);
void __cdecl ROSTER_FreeAll();
D2RosterUnitStrc *__fastcall ROSTER_PartyGetNextEntry(D2RosterUnitStrc *pEntry);
char *__fastcall ROSTER_GetDisplayName(D2RosterUnitStrc *pEntry);

//  0x6FAAF600..0x6FAAFE21: the rebuild's steps out of line, packet 0x65, three accessors and three searches.
D2RosterUnitStrc *__fastcall ROSTER_SortEntriesByName(D2RosterUnitStrc *pList);
D2RosterUnitStrc *__fastcall ROSTER_SortEntriesByKillCount(D2RosterUnitStrc *pList);
D2RosterGroupStrc *__fastcall ROSTER_FindListNode(int16_t nPartyId);
D2RosterGroupStrc *__fastcall ROSTER_FindOrAddListNode(int16_t nPartyId);
void __cdecl ROSTER_FreeList();
void __fastcall ROSTER_PartyExtractPartyMembers(D2RosterGroupStrc *pGroup);
void __stdcall ROSTER_SortList();
void __fastcall ROSTER_MoveListNodeAndEntryToFront(D2UnitGUID dwUnitId, int16_t nPartyId);
void __fastcall ROSTER_ResortAllGroups();
void __cdecl ROSTER_FlattenGroupsIntoEntryList();
void __fastcall ROSTER_MoveEntryToFront(D2UnitGUID dwUnitId);
void __fastcall ROSTER_OnPartyUpdate(uint32_t dwUnitId, int32_t nKillCount);
int32_t __fastcall ROSTER_EntryGetKillCount(D2RosterUnitStrc *pEntry);
int32_t __fastcall ROSTER_EntryGetLevel(D2RosterUnitStrc *pEntry);
int32_t __fastcall ROSTER_EntryGetClassId(D2RosterUnitStrc *pEntry);
D2RosterUnitStrc *__fastcall ROSTER_FindEntryByUnitOrCorpse(D2UnitGUID dwUnitId);
int16_t __fastcall ROSTER_PartyGetSharedPartyId(D2RosterUnitStrc *pRosterEntry);
int16_t __fastcall ROSTER_FindPartyIdByUnit(D2UnitStrc *pUnit);

//  0x6FAAFE30..0x6FAB087B: the four info-flag predicates, the three toggles, the party button (0x5E), leave-party and the hostile box, the
//  hostility test and the GUID accessor.
BOOL __fastcall ROSTER_EntryAllowsLoot(D2RosterUnitStrc *pEntry);
BOOL __fastcall ROSTER_EntryIsIgnored(D2RosterUnitStrc *pEntry);
BOOL __fastcall ROSTER_EntryIsSquelched(D2RosterUnitStrc *pEntry);
BOOL __fastcall ROSTER_EntryIsHostile(D2RosterUnitStrc *pEntry);
D2C_PartyButtons __fastcall ROSTER_EntryGetPartyButton(D2RosterUnitStrc *pEntry);
void __fastcall ROSTER_ToggleLootAndNotify(D2RosterUnitStrc *pEntry);
void __fastcall ROSTER_ToggleIgnoreAndNotify(D2RosterUnitStrc *pEntry);
void __fastcall ROSTER_ToggleSquelchAndNotify(D2RosterUnitStrc *pEntry);
void __fastcall ROSTER_PartySendButtonCommand(D2RosterUnitStrc *pEntry);
void __cdecl ROSTER_PartySendLeaveParty();
void __fastcall ROSTER_PartySendHostileToggle(D2RosterUnitStrc *pEntry);
BOOL __fastcall ROSTER_IsHostileTo(D2UnitGUID dwPlayerGUID, D2UnitGUID dwTargetGUID);
int32_t __fastcall ROSTER_GetEntryUnitId(D2RosterUnitStrc *pRosterEntry);

//  0x6FAB0880..0x6FAB0DB6: packets 0x7F, 0x90 and 0x82 into the entry, the GUID lookups by id, corpse or portal, and the info-list head.
void __fastcall ROSTER_UpdateLifeAndLevel(int32_t dwUnitGUID, int32_t nLifePercentage, uint16_t nLevelId);
void __fastcall ROSTER_SetEntryPosition(uint32_t dwUnitId, int32_t nX, int32_t nY);
int32_t __fastcall ROSTER_GetPartyLife(uint32_t dwUnitId);
BOOL __fastcall ROSTER_IsUnitNotSquelched(D2UnitGUID dwUnitId);
void __fastcall ROSTER_GetEntryPosition(D2RosterUnitStrc *pRosterEntry, int32_t *pnX, int32_t *pnY);
int32_t __fastcall ROSTER_GetEntryLevelId(D2RosterUnitStrc *pRosterEntry);
char *__fastcall ROSTER_GetNameEx(uint32_t dwUnitId);
char *__fastcall ROSTER_GetPlayerDisplayName(D2UnitStrc *pUnit);
void __fastcall ROSTER_SetEntryPortalGuids(uint32_t dwUnitId, uint32_t dwSrcPortalGUID, uint32_t dwDstPortalGUID);
char *__fastcall ROSTER_GetNameExByPortalGuid(uint32_t dwPortalGUID);
BOOL __fastcall ROSTER_HasOtherPartyMember(D2UnitStrc *pUnit);
D2RosterInfoStrc **__fastcall ROSTER_GetInfoListByUnitOrCorpse(uint32_t dwUnitId);

//  0x6FAB0DC0..0x6FAB14BD: packets 0x8D, 0x8B, 0x8C and 0x8E into the roster, then the ten that name no file -- loot and party tests, the
//  four directed predicates, the sex of a class and the party life.
void __fastcall ROSTER_SetPartyId(int32_t dwPlayerGUID, int16_t nPartyGUID);
void __fastcall ROSTER_SetPartyButton(int32_t nUnitId, uint8_t eButton);
void __fastcall ROSTER_ApplyInfoFlags(D2UnitGUID dwPlayerGUID, D2UnitGUID dwTargetGUID, uint16_t nFlags);
void __fastcall ROSTER_FindCorpseEntry(uint32_t dwUnitId, uint32_t dwCorpseId);
void __fastcall ROSTER_RemoveCorpseFromEntry(int32_t nUnitId, int32_t nCorpseId);
BOOL __fastcall ROSTER_CanUnitLootFrom(D2UnitGUID dwLooterGUID, D2UnitGUID dwOwnerGUID);
BOOL __fastcall ROSTER_UnitsShareParty(D2UnitGUID dwUnitIdA, D2UnitGUID dwUnitIdB);
BOOL __cdecl ROSTER_IsAnyoneHostileToPlayer();
BOOL __fastcall ROSTER_EntryIsHostileToPlayer(D2RosterUnitStrc *pEntry);
BOOL __fastcall ROSTER_PlayerIsHostileToEntry(D2RosterUnitStrc *pEntry);
BOOL __fastcall ROSTER_EntryAllowsPlayerLoot(D2RosterUnitStrc *pEntry);
BOOL __fastcall ROSTER_PlayerAllowsEntryLoot(D2RosterUnitStrc *pEntry);
BOOL __fastcall ROSTER_EntryIsMale(D2RosterUnitStrc *pRosterEntry);
void __fastcall ROSTER_SetPartyLife(uint32_t dwUnitId, uint8_t nLife);
int32_t __fastcall ROSTER_GetOtherPlayerPartyLife(uint32_t dwUnitId);

//  ---- RosterPets.cpp ----
//  0x6FAB14C0..0x6FAB1CA2, GAME\RosterPets.cpp, the whole unit: the pet list, packets 0x7A, 0x81 and 0x7F into it, and its accessors.
void __cdecl ROSTERPETS_FreeAll();
void __fastcall ROSTERPETS_FindOrAddPet(uint32_t dwUnitId, uint32_t dwOwnerId, int32_t nPetTypeId, int32_t nMonStatsId);
void __fastcall ROSTERPETS_OnPetPacket81(uint32_t dwUnitId, uint32_t dwOwnerId, int32_t nPetTypeId, uint32_t nMonStatsId, uint32_t *pPetInfo);
void __fastcall ROSTERPETS_RemovePetOrMarkHireling(uint32_t dwUnitId);
int32_t __fastcall ROSTERPETS_GetAutomapClass(D2UnitGUID dwPetUnitId);
D2ClientPetInfoStrc *__fastcall ROSTERPETS_GetPetInfo(uint32_t dwPetUnitId);
int32_t __fastcall ROSTERPETS_CountByType(D2UnitStrc *pOwner, int32_t nPetTypeId);
int32_t __fastcall ROSTERPETS_GetAssociatedUnitGUID(D2UnitStrc *pUnit, int32_t nKind, int32_t bIncludeInactive);
BOOL __fastcall ROSTERPETS_UnitHasPets(D2UnitStrc *pUnit);
void __cdecl ROSTERPETS_UpdateSkeletonPalettes();
void __fastcall ROSTERPETS_UpdateLifeAndArea(uint32_t dwPetUnitId, int32_t nLifePercent, uint16_t wArea);
int32_t __fastcall ROSTERPETS_GetLifePercent(uint32_t dwPetUnitId);
int32_t __fastcall ROSTERPETS_GetMonStatsId(uint32_t dwPetUnitId);
int32_t __fastcall ROSTERPETS_GetOwnerId(uint32_t dwPetUnitId);
int32_t __fastcall ROSTERPETS_GetPetType(uint32_t dwPetUnitId);
int32_t __fastcall ROSTERPETS_IsPetOwnerHostile(D2UnitStrc *pPet);
void __fastcall ROSTERPETS_UpdateRaisedUndead(uint32_t dwOwnerGuid);

//  D2Client.0x6FABA610 .. 0x6FABAD50, Skills\Skills.cpp -- the casts the server replays (packets 0x99/0x9A), the point a unit is heading to
//  and one step in front of it, the two vector helpers of SKILLS_CltDoFunc17 and SKILLS_CltDoFunc80, and the two missile spawners at that point.
int32_t __fastcall SKILLS_ClientCastAtTarget(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, D2UnitStrc *pTarget, int32_t nX, int32_t nY, uint16_t nParam);
int32_t __fastcall SKILLS_ClientCastAtUnit(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, D2UnitStrc *pTarget, uint16_t nParam);
int32_t __fastcall SKILLS_ClientCastAtPosition(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nX, int32_t nY, uint16_t nParam);
void __fastcall SKILLS_GetPosInFrontOfUnit(D2UnitStrc *pUnit, int32_t *pnX, int32_t *pnY);
int32_t __fastcall SKILLS_GetPathTargetPos(D2UnitStrc *pUnit, int32_t *pnX, int32_t *pnY);
void __fastcall SKILLS_GrowVectorToMinLength(int32_t *pX, int32_t *pY);
void __fastcall SKILLS_ShrinkPerpendicularVector(int32_t *pX, int32_t *pY);
D2UnitStrc *__fastcall SKILLS_MissileClientSpawnAimedAtTargetUnit(D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId, D2C_Skills nSkillId, int32_t nSkillLevel);
D2UnitStrc *__fastcall SKILLS_MissileClientSpawnOnTargetUnit(D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId, D2C_Skills nSkillId, int32_t nSkillLevel);

//  D2Client.0x6FABAEB0 .. 0x6FABB910, Skills\Skills.cpp -- the ammo test of the bow and throwing start functions, the straight client
//  missile, the D2C_AuraFilters target filter, and the nearby-target scan with the two functions built on it and their two visitors.
BOOL __fastcall SKILLS_HasAmmoForShot(D2UnitStrc *pUnit);
D2UnitStrc *__fastcall SKILLS_SpawnCltMissileStraight(D2C_MissilesHcIdx nMissileId, D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nSpawnOffsetX, int32_t nSpawnOffsetY, int32_t nTargetX, int32_t nTargetY);
BOOL __fastcall SKILLS_UnitMatchesTargetMask(D2UnitStrc *pUnit, D2UnitStrc *pTarget, D2C_AuraFilters nMask);
void __fastcall SKILLS_ForEachNearbyTarget(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nRadius, D2C_AuraFilters nMask, int32_t(__fastcall *pfnVisit)(D2ClientNearbyTargetCtxStrc *pCtx, D2UnitStrc *pTarget),
                                           void *pUserData, BOOL bSkipNoAuraMonsters);
int32_t __fastcall SKILLS_CountNearbyTargets(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nRadius, D2C_AuraFilters nMask);
int32_t __fastcall SKILLS_VisitCountUnits(D2ClientNearbyTargetCtxStrc *pCtx, D2UnitStrc *pTarget);
D2UnitStrc *__fastcall SKILLS_FindNextNearbyTargetById(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nRadius, D2C_AuraFilters nMask, uint32_t dwAfterUnitId, int32_t *pnCount);
int32_t __fastcall SKILLS_NextTargetByIdVisitor(D2ClientNearbyTargetCtxStrc *pCtx, D2UnitStrc *pTarget);

//  D2Client.0x6FABB960 .. 0x6FABBEB0, Skills\Skills.cpp -- the Skills.txt `cltstfunc` 1..4 (Attack, Throw, Unsummon, Left Hand Swing),
//  `cltdofunc` 1 and 2 (Attack, Throw), and the swap in front of SKILLS_RunCltStFunc.
BOOL __fastcall SKILLS_CltStFunc1(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_CltStFunc2(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_CltStFunc3(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_CltStFunc4(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc1(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc2(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_RunCltStFuncSwapped(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, D2UnitGUID nOwnerGUID);

//  D2Client.0x6FB77488 and 0x6FB775A8, Skills\Skills.cpp -- the two client dispatch tables: gpSkillCltStFuncs (Skills.txt `cltstfunc`) and
//  gpSkillCltDoFuncs (every other client column), slot N holding SKILLS_CltStFuncN or SKILLS_CltDoFuncN. One signature serves both, BOOL
//  being int.
using SKILLS_CltFuncFn = int32_t (__fastcall *)(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
//  The two tables are `inline` variables in Skills.h; a file that reads them includes it.

//  D2Client.0x6FABBED0, Skills\Skills.cpp -- the client's start of a skill: the LineOfSight, corpse, ally and town gates, the assignment, the
//  mode, the Skills.txt `cltstfunc` dispatch, the start sounds, the cast overlay and the cast light.
int32_t __fastcall SKILLS_RunCltStFunc(D2UnitStrc *pUnit, D2C_Skills nSkillId, D2UnitGUID nOwnerGUID, int32_t nSkillLevel);

//  D2Client.0x6FABC6B0, 0x6FABCA30 and 0x6FABD2A0 .. 0x6FABD400, Skills\Skills.cpp -- the Skills.txt `cltdofunc` dispatch and its caller for
//  the used skill; the two starts a client action makes, on a unit and at a point, and the swap of a dual wielder's hands.
int32_t __fastcall SKILLS_RunClientDoFunc(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t bUseTarget);
void __fastcall SKILLS_RunUsedSkillClientDoFunc(D2UnitStrc *pUnit);
int32_t __fastcall SKILLS_CltStartSkillOnUnit(D2UnitStrc *pUnit, int32_t *pParams);
void __fastcall SKILLS_CltAlternateDualWieldHand(D2UnitStrc *pUnit, int32_t *pParams);
int32_t __fastcall SKILLS_CltStartSkillAtCoords(D2UnitStrc *pUnit, int32_t *pParams);

//  D2Client.0x6FABCA70 .. 0x6FABCD70, Skills\Skills.cpp -- the two halves of server packet 0xA3, chosen by the skill's PROGRESSIVE flag.
void __fastcall SKILLS_ApplyClientSkillEffectNonProgressive(D2UnitStrc *pUnit, D2UnitStrc *pTarget, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nX, int32_t nY, uint8_t nSeed);
void __fastcall SKILLS_ApplyClientSkillEffect(D2UnitStrc *pUnit, D2UnitStrc *pTarget, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nX, int32_t nY, int32_t nCharges);

//  D2Client.0x6FABD4D0 .. 0x6FABD690, Skills\Skills.cpp -- the ring of 64 missiles around a unit, and the array of up to 64 from one point.
D2UnitStrc *__fastcall SKILLS_SpawnMissileRingFromUnit(D2UnitStrc *pOwner, D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId, D2C_MissilesHcIdx nAltMissileId, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nVelocity);
D2UnitStrc *__fastcall SKILLS_SpawnMissileRing(D2MissileStrc *pParams, D2MissileStrc *pAltParams);
D2UnitStrc *__fastcall SKILLS_SpawnMissileArray(int32_t nMissiles, D2MissileStrc *pParams);

//  D2Client.0x6FABD8A0 .. 0x6FABDB00, Skills\Skills.cpp -- two scatters on the unit's own seed, the direction callback one installs, a melee
//  swing through the action dispatcher, an animation frame slowed.
void __fastcall SKILLS_SpawnScatteredMissiles(D2UnitStrc *pUnit, D2MissileStrc *pParams, int32_t nJitter, int32_t nCount);
void __fastcall SKILLS_SetUnitDirection(D2UnitStrc *pUnit, int32_t nDirection);
int32_t __fastcall SKILLS_DispatchUnitAction22(D2UnitStrc *pUnit, D2UnitStrc *pTarget);
void __fastcall SKILLS_ReduceFixedFieldByPercent(D2UnitStrc *pUnit, int32_t nPercent);
int32_t __fastcall SKILLS_SpawnClientEffectScatter(D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId, int32_t nCount, int32_t nRadius, D2C_Skills nSkillId, int32_t nSkillLevel);

//  D2Client.0x6FABDD20 .. 0x6FABE200, Skills\Skills.cpp -- the stat list that ends a movement skill and its remove callback, levels added to a
//  page, the landing check, the melee reach, a temporary attack rate.
void __fastcall SKILLS_PostSkillStatWithRemoveCallback(D2UnitStrc *pUnit);
void __fastcall SKILLS_StatRemoveCallback_EndMoveSkill(D2UnitStrc *pUnit, int32_t nState, D2StatListStrc *pStatList);
void __fastcall SKILLS_AddLevelsToSkillPage(D2UnitStrc *pUnit, int32_t nDelta, int32_t nElement, uint8_t nSkillPage);
BOOL __fastcall SKILLS_CanMissileLandAtAimPoint(D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId);
int32_t __fastcall SKILLS_GetExtendedMeleeRange(D2UnitStrc *pUnit);
void __fastcall SKILLS_ApplyAttackRateStat(D2UnitStrc *pUnit, int32_t nValue);

//  D2Client.0x6FABA5E0, 0x6FABD780 and 0x6FABE260, Skills.cpp -- the client's state handlers, each written in the Skills*.cpp unit its
//  address is in.
int32_t __fastcall SKILLS_CltDoFunc85(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
void __fastcall SKILLS_StatesRunActiveStateFuncs(D2UnitStrc *pUnit);
D2UnitEventStrc *__fastcall SKILLS_StatesAddCltEventFunc(D2UnitStrc *pUnit, int32_t nCltEvent, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nCltEventFunc, int32_t nKind, int32_t nParam);
//  D2Client.0x6FAC2C90 and 0x6FAC2D90, SkillsDru.cpp -- the `hurricane` state's `cltactivefunc` 90 and its debris.
int32_t __fastcall SKILLS_CltDoFunc90(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
void __fastcall SKILLS_StatesSpawnHurricaneCltMissile(D2UnitStrc *pUnit, D2SkillsTxt *pSkillsTxt, D2C_Skills nSkillId, int32_t nSkillLevel);
//  D2Client.0x6FAC4870 .. 0x6FAC4BC0, SkillsEMon.cpp -- the `attached` and `vine_beast` states' `cltactivefunc` 88 and 89 and the vine
//  beast's missile match.
int32_t __fastcall SKILLS_CltDoFunc88(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc89(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_StatesMatchVineBeastNeutralMissile(D2UnitStrc *pUnit, void *pUnused);
//  D2Client.0x6FAC9350, SkillsPal.cpp -- the `sanctuary` state's `cltactivefunc` 86.
int32_t __fastcall SKILLS_CltDoFunc86(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
//  D2Client.0x6FACB1F0 .. 0x6FACB3C0, SkillsSor.cpp -- the `chillingarmor` state's `clteventfunc` 1 and the `shiverarmor` state's
//  `cltactivefunc` 87.
int32_t __fastcall SKILLS_StatesCltEventFunc1(int32_t nEvent, D2UnitStrc *pUnit, D2UnitStrc *pMissile, int32_t nUnused, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc87(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
//  D2Client.0x6FB4BB40 .. 0x6FB4D510, States.cpp -- the States.txt `setfunc` and `remfunc` handlers of gpStateSetFuncs and gpStateRemFuncs,
//  named by slot, and the state functions beside them. The two dispatchers are PlrSkills.cpp's, below.
void __fastcall STATES_SetFunc1_Progressive(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_RemFunc1_Progressive(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_SetFunc2_CloakOfShadows(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_SetFunc3_Blaze(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_SetFunc4_SourceUnit(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_RemFunc2_SourceUnit(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_SetFunc5_ChangeClass(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_RemFunc3_BoneArmor(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_SetFunc6_BoneArmor(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_RemFunc5_CycloneArmor(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_SetFunc8_CycloneArmor(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_SetFunc7_Hurricane(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_RemFunc4_Hurricane(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_SetFunc9_RemFunc6_ShrineSkill(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_SetFunc10_Redeemed(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_RemFunc7_Uninterruptable(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_SetFunc11_VineBeast(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_SetFunc12_Invis(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_RemFunc8_Invis(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_SetFunc13_Attached(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_RemFunc9_Attached(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_RemFunc10_Pregnant(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_SetFunc14_Shapeshift(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_SetFunc15_Revive(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_SetFunc16_SkillCharges(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_RemFunc11_SkillCharges(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_SetFunc17_RestInPeace(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_SetFunc18_CorpseExp(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_SetFunc19_RemFunc12_SkillDelay(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_ApplyStateOverlays(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall STATES_ToggleStateWithSound(D2UnitStrc *pUnit, D2C_States nState);
void __fastcall STATES_RefreshUnitColorShift(D2UnitStrc *pUnit);

//  D2Client.0x6FB4B980 .. 0x6FB4DA80, UNIT\PlrSkills.cpp -- the unit less its 32-function STATES_ block (States.cpp, above): the used skill's
//  sequence-mode effect, a state's end and its stats from the server, and the client's use state of a skill.
void __fastcall PLRSKILLS_SkillsDrawUnitEffectBehind(D2UnitStrc *pUnit, int32_t nDrawParam);
void __fastcall PLRSKILLS_SkillsDrawUnitEffectInFront(D2UnitStrc *pUnit, int32_t nDrawParam);
void __fastcall PLRSKILLS_EndStateAndSkill(D2UnitStrc *pUnit, D2C_States nState);
D2StatListStrc *__fastcall PLRSKILLS_SetStateStat(D2UnitStrc *pUnit, D2StatListStrc *pStatList, D2C_States nState, D2C_ItemStats nStatId, int32_t nValue, uint16_t nLayer);
D2C_SkillUseStates __fastcall PLRSKILLS_SkillsGetUseStateForClient(D2UnitStrc *pUnit, D2SkillStrc *pSkill);
//  D2Client.0x6FB4D920 and 0x6FB4DA00: the two dispatchers of gpStateSetFuncs and gpStateRemFuncs.
void __fastcall PLRSKILLS_StatesRunStateSetFunc(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall PLRSKILLS_StatesRunStateRemFunc(D2UnitStrc *pUnit, D2C_States nStateId);

//  D2Client.0x6FB4DEC0 .. 0x6FB4F460, UNIT\UnitMode.cpp -- a unit's path and position against the server's: the stale path target, the room
//  re-cache, the walk to a point, the position correction and the move to coordinates; the walk to a unit, a frame of the walk, a frame of
//  the unit, and the interaction with a unit; the client command, the action by unit type, and the queued command; the walk test, the walk
//  with the command queued, the hit overlay, the server's memory probe answered, and the two mode setters.
void __fastcall UNITMODE_ClearStaleTargetUnit(D2UnitStrc *pUnit);
void __fastcall UNITMODE_RecacheUnitRoom(D2UnitStrc *pUnit);
int32_t __fastcall UNITMODE_StartMoveToLocation(D2UnitStrc *pUnit, int32_t nX, int32_t nY);
void __fastcall UNITMODE_SyncUnitPosition(D2UnitStrc *pUnit, uint16_t nX, uint16_t nY, uint32_t nToleranceMode, int32_t nNextX, int32_t nNextY);
void __fastcall UNITMODE_MoveUnitToCoords(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t bSnap);
int32_t __fastcall UNITMODE_StartMoveToUnit(D2UnitStrc *pUnit, int32_t nTargetType, int32_t nTargetGUID);
BOOL __fastcall UNITMODE_StepUnitAlongPath(D2UnitStrc *pUnit);
void __fastcall UNITMODE_UpdateUnit(D2UnitStrc *pUnit);
void __fastcall UNITMODE_InteractWithUnit(uint16_t nUnitType, int32_t nUnitGUID);
void __fastcall UNITMODE_PerformClientCommand(int32_t nClientCmd, D2UnitStrc *pUnit, int32_t nTargetTypeOrX, int32_t nTargetGuidOrY, int32_t nUnused);
void __fastcall UNITMODE_DispatchActionByUnitType(D2C_ClientActions nAction, D2UnitStrc *pUnit, void *pActionParams, BOOL bInterruptKnockback);
void __fastcall UNITMODE_RunQueuedCommand(D2UnitStrc *pUnit);
BOOL __fastcall UNITMODE_MustMoveToTarget(D2UnitStrc *pUnit, D2UnitStrc *pTarget, int32_t nClientCmd);
void __fastcall UNITMODE_MoveToTargetAndQueueCommand(D2UnitStrc *pUnit, int32_t nQueuedCmd, int32_t nTargetType, int32_t nTargetGUID);
void __fastcall UNITMODE_PlayHitOverlayForMode(D2UnitStrc *pUnit, int32_t nMode);
void __cdecl UNITMODE_FlushServerMemoryProbe();
void __fastcall UNITMODE_SetAnimMode(D2UnitStrc *pUnit, int32_t nMode);
void __fastcall UNITMODE_SetAnimModeSilent(D2UnitStrc *pUnit, int32_t nMode);

//  D2Client.0x6FB3F270 .. 0x6FB3F340, UNIT\Monster.cpp -- the units near one visited until a callback answers, and a player into and out of
//  gapClientPlayers. MONSTER_ClearPlayerSlots, the fourth, is declared with the GAME_InitGame callees above.
int32_t __fastcall MONSTER_ForEachUnitNearby(D2UnitStrc *pUnit, int32_t nCallbackCtx, int32_t nCallbackParam, int32_t (__fastcall *pfnVisit)(D2UnitStrc *, D2UnitStrc *, int32_t, int32_t));
void __fastcall MONSTER_RegisterPlayerSlot(D2UnitStrc *pPlayer);
void __fastcall MONSTER_UnregisterPlayerSlot(D2UnitStrc *pPlayer);

//  D2Client.0x6FB3F350 .. 0x6FB3FC2C, UNIT\Monster.cpp -- a client-only monster's tick, the threat search's callback, the four moves (nAction
//  with the seven-int block; the flee answers 0 for a blocked step) and the idle timer's setter.
void __fastcall MONSTER_CritterTickAmbient(D2UnitStrc *pMonster);
int32_t __fastcall MONSTER_CritterFindNearestThreatCallback(D2UnitStrc *pCritter, D2UnitStrc *pCandidate, D2UnitStrc **ppNearest, int32_t *pnBestDistance);
void __fastcall MONSTER_CritterMoveToUnit(D2UnitStrc *pUnit, D2UnitStrc *pTarget, D2C_ClientActions nAction, int32_t nVelocityPercent);
int32_t __fastcall MONSTER_CritterFleeFromTarget(D2UnitStrc *pUnit, D2UnitStrc *pTarget, uint8_t nDistance, D2C_ClientActions nAction, int32_t nVelocityPercent);
void __fastcall MONSTER_CritterWanderRandom(D2UnitStrc *pUnit, uint8_t nDistance, D2C_ClientActions nAction, int32_t nVelocityPercent);
void __fastcall MONSTER_CritterStandStill(D2UnitStrc *pUnit);
int32_t *__fastcall MONSTER_CritterSetCooldown(D2UnitStrc *pUnit, int32_t nTicks);

//  D2Client.0x6FB3FC30 .. 0x6FB40967, UNIT\Monster.cpp -- the ticks MONSTER_CritterTickAmbient's switch dispatches (the fliers, the maggot,
//  the crawlers, the snake, the bunny, the Worldstone chips) and the crawlers' corpse callback.
void __fastcall MONSTER_CritterTickBird(D2UnitStrc *pBird, D2UnitStrc *pTarget, int32_t nNearest);
void __fastcall MONSTER_CritterTickMaggot(D2UnitStrc *pMaggot, D2UnitStrc *pTarget, int32_t nNearest);
void __fastcall MONSTER_CritterTickCrawler(D2UnitStrc *pUnit, D2UnitStrc *pTarget, uint32_t nNearest);
int32_t __fastcall MONSTER_CritterPickRandomCorpseCallback(D2UnitStrc *pCritter, D2UnitStrc *pCandidate, D2UnitStrc **ppChosen, int32_t *pnDistance);
void __fastcall MONSTER_CritterTickSnake(D2UnitStrc *pSnake, D2UnitStrc *pThreat, uint32_t nNearest);
void __fastcall MONSTER_CritterTickBunny(D2UnitStrc *pBunny, D2UnitStrc *pTarget, int32_t nNearest);
void __fastcall MONSTER_EnvTickWorldstoneChips(D2UnitStrc *pUnit, D2UnitStrc *pTarget, int32_t nNearest);

//  D2Client.0x6FB40970 .. 0x6FB40D55, UNIT\Monster.cpp -- a mode test, a skill's sound, the monstats bind, the accessors of pMonsterData's
//  client slots, the light, and five type-flag tests.
int32_t __fastcall MONSTER_IsMonModeEnabled(D2C_MonsterIds nMonsterClassId, D2C_MonModes nMode);
D2C_Sounds __fastcall MONSTER_GetSkillSound(D2UnitStrc *pUnit, D2C_Skills nSkillId);
void __fastcall MONSTER_BindMonStatsRecord(D2UnitStrc *pUnit, int16_t nAiControl);
int32_t *__fastcall MONSTER_GetClientTimerPtr(D2UnitStrc *pUnit);
int32_t __fastcall MONSTER_GetHirelingOwnerGUID(D2UnitStrc *pUnit);
void __fastcall MONSTER_SetHirelingOwnerGUID(D2UnitStrc *pUnit, int32_t nOwnerGUID);
int32_t *__fastcall MONSTER_GetClientFlagPtr(D2UnitStrc *pUnit);
void __fastcall MONSTER_UpdateClientLight(D2UnitStrc *pUnit);
BOOL __fastcall MONSTER_IsChampion(D2UnitStrc *pUnit);
BOOL __fastcall MONSTER_IsGhostly(D2UnitStrc *pUnit);
BOOL __fastcall MONSTER_IsUnique(D2UnitStrc *pUnit);
BOOL __fastcall MONSTER_IsMinion(D2UnitStrc *pUnit);
BOOL __fastcall MONSTER_IsSuperUnique(D2UnitStrc *pUnit);

//  D2Client.0x6FB40D60 .. 0x6FB418F6, UNIT\Monster.cpp -- the client's construction of a monster, its class's links, the vile child's match
//  against its mother, and the free.
void __fastcall MONSTER_InitClientUnit(D2UnitStrc *pUnit, D2ActiveRoomStrc *pRoom, int32_t nX, int32_t nY, D2C_MonsterIds nMonClassId, int32_t nAnimMode);
void __fastcall MONSTER_LinkOwnedUnits(D2UnitStrc *pUnit, D2C_MonsterIds nClassId);
int32_t __fastcall MONSTER_MatchVileMotherDirection(D2UnitStrc *pChild, D2UnitStrc *pOther, int32_t nUnused1, int32_t nUnused2);
void __fastcall MONSTER_FreeUnitData(D2UnitStrc *pUnit);

//  D2Client.0x6FB41900 .. 0x6FB42403, UNIT\Monster.cpp -- a class changed in place, the animation advanced by its mode's descriptor, and a
//  monster's animation frame.
void __fastcall MONSTER_ChangeMonsterClass(D2UnitStrc *pUnit, D2C_MonsterIds nNewMonClassId);
int32_t __fastcall MONSTER_AdvanceAnimForMode(D2UnitStrc *pUnit, const D2ClientMonModeDescStrc *pModeDesc);
void __fastcall MONSTER_UpdateAnimMode(D2UnitStrc *pUnit);

//  D2Client.0x6FB42420 .. 0x6FB42D32, UNIT\Monster.cpp -- an attack frame's missile, an animation's special end, and a unit's blood
//  (bByChance non-zero: a 1-in-25 roll first).
void __fastcall MONSTER_SpawnAttackMissile(D2UnitStrc *pUnit);
void __fastcall MONSTER_DoClientSplEndEffect(D2UnitStrc *pMonster);
void __fastcall MONSTER_SpawnBlood(D2UnitStrc *pUnit, int32_t nCount, int32_t bByChance);

//  D2Client.0x6FB42D40, UNIT\Monster.cpp -- a monster's client action, the MONSTER arm of the dispatch (pParams: seven ints).
void __fastcall MONSTER_DoClientAction(D2C_ClientActions nAction, D2UnitStrc *pUnit, int32_t *pParams);

//  D2Client.0x6FB44460 .. 0x6FB44A4D, UNIT\Monster.cpp -- the vulture's flight, a hireling built on the client, the revive test, packet
//  0x98's store (the evil hut's minion class), and the flag-word and monster-data accessors.
void __fastcall MONSTER_SetVultureFlightState(D2C_ClientActions nAction, D2UnitStrc *pUnit, int32_t *pUnusedParams);
void __fastcall MONSTER_InitHireling(D2UnitStrc *pMonster, const D2ClientHirelingInitStrc *pHirelingInfo);
BOOL __fastcall MONSTER_CanBeRevived(D2UnitStrc *pUnit);
void __fastcall MONSTER_SetMinionSpawnClassId(const D2GSPacketSrv98 *pPacket);
BOOL __fastcall MONSTER_TestFlag(D2UnitStrc *pUnit, uint32_t dwMask);
void __fastcall MONSTER_SetFlagEx(D2UnitStrc *pUnit, int32_t nFlagEx, int32_t bSet);
int32_t __fastcall MONSTER_GetBaseId(int32_t nMonClassId);
uint8_t *__fastcall MONSTER_GetMonUmodArray(D2UnitStrc *pUnit);
void __fastcall MONSTER_SetNameSeed(D2UnitStrc *pUnit, uint16_t wNameSeed);
void __fastcall MONSTER_SetBossHcIdx(D2UnitStrc *pUnit, uint16_t wBossHcIdx);
int16_t __fastcall MONSTER_GetBossHcIdx(D2UnitStrc *pUnit);
BOOL __fastcall MONSTER_TestTypeFlag(D2UnitStrc *pUnit, D2C_MonTypeFlags wMask);
void __fastcall MONSTER_SetTypeFlag(D2UnitStrc *pUnit, D2C_MonTypeFlags wMask, int32_t bSet);
void __fastcall MONSTER_SetAuraRingEnabled(D2UnitStrc *pUnit, int32_t bEnabled);

//  D2Client.0x6FB44A70 .. 0x6FB451C1, UNIT\MonUnique.cpp -- a boss's name, the other monumod.txt handlers gpMonUModHandlers holds (light,
//  durieldead, teleport, the resistances) and the walk that runs them.
void __fastcall MONUNIQUE_BuildMonsterNameText(D2UnitStrc *pMonster, D2C_MonUMods nModId, int32_t bUnique);
void __fastcall MONUNIQUE_ModLight(D2UnitStrc *pMonster, D2C_MonUMods nModId, int32_t bUnique);
void __fastcall MONUNIQUE_ModDurielDead(D2UnitStrc *pMonster, D2C_MonUMods nModId, int32_t bUnique);
void __fastcall MONUNIQUE_ModTeleport(D2UnitStrc *pMonster, D2C_MonUMods nModId, int32_t bUnique);
void __fastcall MONUNIQUE_ModResists(D2UnitStrc *pMonster, D2C_MonUMods nModId, int32_t bUnique);
void __fastcall MONUNIQUE_ApplyModifiers(D2UnitStrc *pMonster);

//  D2Client.0x6FB451D0 .. 0x6FB45B4F, UNIT\MonUnique.cpp -- seven of gpMonUModCltFuncs' client hooks (goboom, fire, questcomplete, the three
//  elemental deaths, durieldead, lightning's two) and the ring they draw.
void __fastcall MONUNIQUE_MonUModCltGoBoom(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique);
void __fastcall MONUNIQUE_MonUModCltFire(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique);
void __fastcall MONUNIQUE_MonUModCltQuestComplete(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique);
void __fastcall MONUNIQUE_MonUModCltElementalDeath(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique);
void __fastcall MONUNIQUE_MonUModCltDurielDead(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique);
void __fastcall MONUNIQUE_MonUModCltGetHitOverlay(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique);
void __fastcall MONUNIQUE_MonUModSpawnAuraMissileRing(D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId, int32_t nSkillLevel);
void __fastcall MONUNIQUE_MonUModCltLightning(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique);

//  D2Client.0x6FB45B60 .. 0x6FB462B4, UNIT\MonUnique.cpp -- the last five client hooks (scarab, multishot, firespike_explode,
//  suicideminion_explode, worms_on_death), the pregnant state's burst, and gpMonUModCltFuncs' dispatcher with its five entry points.
void __fastcall MONUNIQUE_MonUModCltScarab(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique);
void __fastcall MONUNIQUE_MonUModCltMultishotSpawn(D2UnitStrc *pMissile, D2C_MonUMods nModId, int32_t bUnique);
void __fastcall MONUNIQUE_MonUModCltFireSpikeExplode(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique);
void __fastcall MONUNIQUE_MonUModCltSuicideMinionExplode(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique);
void __fastcall MONUNIQUE_MonUModPlayPregnantBurst(D2UnitStrc *pUnit, D2C_States nStateId);
void __fastcall MONUNIQUE_MonUModCltWormsOnDeath(D2UnitStrc *pUnit, D2C_MonUMods nModId, int32_t bUnique);
void __fastcall MONUNIQUE_MonUModRunCltFuncsCol0(D2UnitStrc *pUnit);
void __fastcall MONUNIQUE_MonUModRunCltFuncsForUnit(D2UnitStrc *pUnit, D2UnitStrc *pMissile, int32_t nColumn);
void __fastcall MONUNIQUE_MonUModRunCltFuncsCol1(D2UnitStrc *pUnit);
void __fastcall MONUNIQUE_MonUModRunCltFuncsCol2(D2UnitStrc *pUnit);
void __fastcall MONUNIQUE_MonUModRunCltFuncsCol3(D2UnitStrc *pUnit);
void __fastcall MONUNIQUE_MonUModRunCltFuncsOnMissileSpawn(D2UnitStrc *pUnit, D2UnitStrc *pMissile);

//  D2Client.0x6FB462C0, UNIT\MonUnique.cpp -- a boss's enchantment label.
void __fastcall MONUNIQUE_MonUModBuildMonsterModifierLabel(D2UnitStrc *pUnit, Unicode *pwszOut, int32_t nCapacity, D2C_StringIndices nPrefixStr);

//  D2Client.0x6FB464A0 .. 0x6FB467D6, UNIT\Object.cpp's head -- an NPC's greeting, the sound of a spoken line and the NPC speech option's
//  audio switch.
D2C_Sounds __fastcall OBJECT_PickNpcSpeechSound(D2UnitStrc *pUnit, int32_t nSpeechKind);
D2C_Sounds __fastcall OBJECT_FindSpeechSoundByTblIndex(D2C_StringIndices nTblStringId);
void __fastcall OBJECT_SetNpcSpeechAudio(int32_t bAudio);

//  D2Client.0x6FB467E0 .. 0x6FB46C26, UNIT\Object.cpp -- the day/night appearance, its light, client construction and idle overlay.
int32_t __fastcall OBJECT_ApplyDayNightMode(D2UnitStrc *pObject, int32_t bSkipAnimRebuild);
void __fastcall OBJECT_SetLight(D2UnitStrc *pObject, int32_t nRadius, int32_t nRed, int32_t nGreen, int32_t nBlue);
void __fastcall OBJECT_InitClientUnit(D2UnitStrc *pObject, D2ActiveRoomStrc *pRoom, int32_t nX, int32_t nY, D2Objects nObjectId, uint8_t nMode);
void __fastcall OBJECT_AddIdleOverlay(D2UnitStrc *pObject);
//  D2Client.0x6FB46D10 .. 0x6FB470C8, UNIT\Object.cpp -- freeing an object, and a frame of its animation: the general one, the door's, and the
//  door's end.
void __fastcall OBJECT_FreeClientUnit(D2UnitStrc *pObject);
void __fastcall OBJECT_AdvanceAnim(D2UnitStrc *pUnit);
void __fastcall OBJECT_AdvanceDoorAnim(D2UnitStrc *pUnit);
void __fastcall OBJECT_FinishDoorAnim(D2UnitStrc *pUnit);
//  D2Client.0x6FB470D0 .. 0x6FB4765B, UNIT\Object.cpp -- the shrines: the four client-side effects (gShrineChart's pfnClientEffect column), a
//  booster shrine's overlays (its pfnApplyOverlays) and a shrine's creation.
void __fastcall OBJECT_ShrineOfEnirhsEffect(D2UnitStrc *pShrine, D2UnitStrc *pPlayer, D2ShrinesTxt *ptShrineData);
void __fastcall OBJECT_StormShrineEffect(D2UnitStrc *pShrine, D2UnitStrc *pPlayer, D2ShrinesTxt *ptShrineData);
void __fastcall OBJECT_ExplodingShrineEffect(D2UnitStrc *pShrine, D2UnitStrc *pPlayer, D2ShrinesTxt *ptShrineData);
void __fastcall OBJECT_PoisonShrineEffect(D2UnitStrc *pShrine, D2UnitStrc *pPlayer, D2ShrinesTxt *ptShrineData);
void __fastcall OBJECT_ApplyShrineOverlays(D2UnitStrc *pObject, D2ShrinesTxt *ptShrineData);
void __fastcall OBJECT_InitShrine(D2UnitStrc *pUnit);
//  D2Client.0x6FB47660 .. 0x6FB47BED, UNIT\Object.cpp -- an object's client action, a door's mode set from the server, and a shrine's sound.
void __fastcall OBJECT_DoClientAction(D2C_ClientActions nAction, D2UnitStrc *pUnit, int32_t *pParams);
void __fastcall OBJECT_SetClientMode(D2UnitStrc *pObject, int32_t bSelectable, int32_t nMode);
void __fastcall OBJECT_PlayShrineSound(D2UnitStrc *pObject, D2UnitStrc *pSourceUnit);
//  D2Client.0x6FB47BF0 .. 0x6FB480F4, UNIT\Object.cpp -- the first seven of sgpfnProcessFunctions' bodies, `__thiscall` in the original.
uint8_t __fastcall OBJECT_ProcessRipple(D2UnitStrc *pUnit);
uint8_t __fastcall OBJECT_ProcessRippleWithSound(D2UnitStrc *pUnit);
uint8_t __fastcall OBJECT_ProcessDrinker(D2UnitStrc *pUnit);
uint8_t __fastcall OBJECT_ProcessGesturer(D2UnitStrc *pUnit);
uint8_t __fastcall OBJECT_ProcessTurner(D2UnitStrc *pUnit);
uint8_t __fastcall OBJECT_ProcessSkeletonSpawn(D2UnitStrc *pUnit);
uint8_t __fastcall OBJECT_ProcessIceBubbles(D2UnitStrc *pUnit);
//  D2Client.0x6FB48100 .. 0x6FB4847B, UNIT\Object.cpp -- the table's other ten bodies and OBJECT_ClientProcess, its dispatcher.
uint8_t __fastcall OBJECT_ProcessNothing(D2UnitStrc *pUnit);
uint8_t __fastcall OBJECT_ProcessClientSmoke(D2UnitStrc *pUnit);
uint8_t __fastcall OBJECT_ProcessAncientsAltar(D2UnitStrc *pUnit);
uint8_t __fastcall OBJECT_ProcessInvisibleAncient(D2UnitStrc *pUnit);
uint8_t __fastcall OBJECT_ProcessHoradricOrifice(D2UnitStrc *pUnit);
uint8_t __fastcall OBJECT_ProcessRogueBonfire(D2UnitStrc *pUnit);
uint8_t __fastcall OBJECT_ProcessFrozenAnya(D2UnitStrc *pUnit);
uint8_t __fastcall OBJECT_ProcessKeeper(D2UnitStrc *pUnit);
uint8_t __fastcall OBJECT_ProcessBaalPortal(D2UnitStrc *pUnit);
uint8_t __fastcall OBJECT_ProcessZoo(D2UnitStrc *pUnit);
uint8_t __fastcall OBJECT_ClientProcess(D2UnitStrc *pUnit);
//  D2Client.0x6FB48480 .. 0x6FB4856F, UNIT\Object.cpp -- a portal's state from packet 0x60, and a town portal's owner name, read and set.
void __fastcall OBJECT_ApplyPortalState(D2GSPacketSrv60 *ptCmd);
char *__fastcall OBJECT_GetPortalOwnerName(D2UnitStrc *pObject);
int32_t __fastcall OBJECT_SetPortalOwnerName(D2UnitStrc *pObject, const char *pszName);

//  D2Client.0x6FB48570 .. 0x6FB4B7BA, UNIT\Player.cpp -- a player unit on the client: its light radius and its stat callback, its construction
//  and teardown, and a frame of its animation mode; the client action it takes, the skill gate, the aura range and the click guard; the mouse
//  click, the move a release resumes, and what a click does; the click sent as a skill command, the move sent as where the path can get, the
//  skill click on a unit and the interaction; the click's point pushed out and a held move throttled, the gate of the press, the right button
//  held, the release, a player's body assigned, another player put into a room, the queued target, and the stop.
void __fastcall PLAYER_UpdateLightRadius(D2UnitStrc *pUnit, int32_t nRadiusDelta);
void __fastcall PLAYER_OnStatValueChanged(struct D2GameStrc *pGame, D2UnitStrc *pOwner, D2UnitStrc *pOther, int32_t nLayer_StatId, int32_t nPreviousValue, int32_t nNewValue);
void __fastcall PLAYER_InitClientUnit(D2UnitStrc *pPlayer, D2ActiveRoomStrc *pRoom, int32_t nX, int32_t nY);
void __fastcall PLAYER_FreeClientUnit(D2UnitStrc *pPlayer);
void __fastcall PLAYER_AdvanceAnimMode(D2UnitStrc *pUnit);
BOOL __fastcall PLAYER_DoClientAction(D2C_ClientActions nAction, D2UnitStrc *pUnit, int32_t *pParams, BOOL bInterruptKnockback);
BOOL __fastcall PLAYER_CanUseSkill(D2UnitStrc *pUnit, D2SkillStrc **ppSkill, int32_t *pnUseState);
BOOL __fastcall PLAYER_IsUnitInAuraRange(D2SkillStrc *pSkill, D2UnitStrc *pTarget, int32_t *pnRange, int32_t *pnDistance);
void __cdecl PLAYER_ResetClickGuard();
int32_t __fastcall PLAYER_OnMouseClick(int32_t nClickEvent, int32_t nX, int32_t nY, int32_t nMoveFlags);
void __fastcall PLAYER_ResumeQueuedMove(D2ClientClickStrc *pClick);
void __fastcall PLAYER_DispatchClickAction(D2ClientClickStrc *pClick);
void __fastcall PLAYER_SendClickCommand(D2ClientClickStrc *pClick, int32_t bOnUnit);
void __fastcall PLAYER_SendMoveToReachablePoint(int32_t nClientCmd, D2UnitStrc *pUnit, int32_t nX, int32_t nY);
void __fastcall PLAYER_DispatchSkillClick(D2ClientClickStrc *pClick, D2C_UnitTypes nTargetType, uint32_t dwTargetGuid);
void __fastcall PLAYER_InteractOrApproachTarget(D2ClientClickStrc *pClick, int32_t nTargetType, int32_t nTargetGuid);
int32_t __fastcall PLAYER_ClampClickTargetAndThrottle(D2UnitStrc *pUnit, D2ClientClickStrc *pClick);
BOOL __fastcall PLAYER_ShouldHandleClick(D2ClientClickStrc *pClick);
int32_t __fastcall PLAYER_IssueRightSkillCommand(D2ClientClickStrc *pClick);
int32_t __fastcall PLAYER_ReleaseClickAndResetHireling(D2ClientClickStrc *pClick);
void __fastcall PLAYER_AssignCorpse(D2GSPacketSrv74 *ptCmd);
void __fastcall PLAYER_SetUnitRoomAndPos(D2UnitStrc *pUnit, D2ActiveRoomStrc *pRoom, int32_t nX, int16_t nY);
int32_t __fastcall PLAYER_GetQueuedTargetType(D2UnitStrc *pUnit);
int32_t __fastcall PLAYER_GetQueuedTargetGuid(D2UnitStrc *pUnit);
//  D2Client.0x6FABE2A0 .. 0x6FABE7F0, Skills\SkillsAma.cpp -- the Skills.txt `cltstfunc` 11..14 and 53 (the bow shots, Jab, Strafe, Fend, Zeal
//  and Fury), and `cltdofunc` 16 and 17 (Jab's sound, the fan of missiles).
BOOL __fastcall SKILLS_CltStFunc11(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_CltStFunc12(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_CltStFunc13(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_CltStFunc14(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_CltStFunc53(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc16(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc17(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);

//  D2Client.0x6FABEA20 .. 0x6FABF2A0, Skills\SkillsAma.cpp -- the Skills.txt `cltdofunc` 18..22 (Guided Arrow and Bone Spirit, Charged Strike,
//  Strafe, Fend with Zeal and Fury, Lightning Strike).
int32_t __fastcall SKILLS_CltDoFunc18(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc19(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc20(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc21(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc22(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);

//  D2Client.0x6FABF4B0 .. 0x6FABFAC0, Skills\SkillsAss.cpp -- the Skills.txt `cltstfunc` 5..8 (Psychic Hammer and Dragon Flight, Dragon Talon,
//  Cloak of Shadows, Blade Fury), `cltdofunc` 3..5 (Psychic Hammer, Dragon Talon, Shock Field), and the progressive missile and calc selectors.
BOOL __fastcall SKILLS_CltStFunc5(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc3(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_CltStFunc6(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc4(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_GetClientMissileForSkill(D2UnitStrc *pUnit, D2C_Skills nSkillId);
int32_t __fastcall SKILLS_EvaluateProgressiveSkillCalc(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc5(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_CltStFunc7(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_StartChannelState(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_CltStFunc8(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);

//  D2Client.0x6FABFAD0 .. 0x6FABFE90, Skills\SkillsAss.cpp -- the Skills.txt `cltdofunc` 6..8 (Blade Fury, Dragon Tail, Mind Blast),
//  `cltstfunc` 9 and 10 (Dragon Tail's start, and one no stock row names), and the disc scatter of `cltprgfunc` 9.
int32_t __fastcall SKILLS_CltDoFunc6(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_CltStFunc9(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc7(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc8(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_CltStFunc10(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
void __fastcall SKILLS_ScatterClientMissilesInDisc(D2UnitStrc *pUnit, int32_t nSeed, D2C_MissilesHcIdx nMissileId, int32_t nCenterX, int32_t nCenterY, int32_t nRadius, D2C_Skills nSkillId, int32_t nSkillLevel);

//  D2Client.0x6FAC00B0 .. 0x6FAC06E0, Skills\SkillsAss.cpp -- the Skills.txt `cltprgfunc` 9, 10, 11 and 83 (the charge releases of Fists of
//  Fire and Blades of Ice, Claws of Thunder, and Royal Strike's second), and the two missile initialisers the novas install.
int32_t __fastcall SKILLS_CltDoFunc9(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc10(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc11(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
void __fastcall SKILLS_MissileInitClientNovaBolt(D2UnitStrc *pMissile, int32_t nLowSeed);
int32_t __fastcall SKILLS_CltDoFunc83(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
void __fastcall SKILLS_MissileBuildWaveringPath(D2UnitStrc *pMissile, int32_t nSeed);

//  D2Client.0x6FAC08E0 .. 0x6FAC0B80, Skills\SkillsAss.cpp -- the Skills.txt `cltprgfunc` 12..15 (the charge releases of Tiger Strike and
//  Cobra Strike, and Royal Strike's first and third).
int32_t __fastcall SKILLS_CltDoFunc12(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc13(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc14(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc15(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);

//  D2Client.0x6FAC0D30 .. 0x6FAC1070, Skills\SkillsBar.cpp -- the Skills.txt `cltstfunc` 26..28 (Find Potion and Grim Ward, Double Swing, Find
//  Item) and `cltdofunc` 38..42 (Find Potion, Double Swing and Frenzy, Find Item, Grim Ward, Double Throw).
BOOL __fastcall SKILLS_CltStFunc26(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc38(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_CltStFunc27(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc39(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc42(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_CltStFunc28(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc40(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc41(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
//  Leap's start, `cltstfunc` 29, and the point it lands on.
BOOL __fastcall SKILLS_CltStFunc29(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_ResolveClientTargetPoint(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t *pnX, int32_t *pnY);
//  A leap's charge move and arc, the collision it lifts and puts back, and `cltdofunc` 43 with the landing it shares with 44.
BOOL __fastcall SKILLS_StartChargeMove(D2UnitStrc *pUnit, D2SkillStrc *pSkill);
void __fastcall SKILLS_StartLeapArc(D2UnitStrc *pUnit, D2SkillStrc *pSkill);
void __fastcall SKILLS_RestoreCollisionFootprint(D2UnitStrc *pUnit);
void __fastcall SKILLS_ApplyDefaultCollision(D2UnitStrc *pUnit, int32_t nX, int32_t nY);
int32_t __fastcall SKILLS_CltDoFunc43(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_CltDoFuncLeap(D2UnitStrc *pUnit, D2SkillStrc *pSkill, BOOL bShowDustOverlay);
//  Leap Attack's start and do (`cltstfunc` 30, `cltdofunc` 44), Whirlwind's spin copy, start, do (`cltstfunc` 31, `cltdofunc` 45) and end.
int32_t __fastcall SKILLS_CltStFunc30(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc44(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
void __fastcall SKILLS_DrawSpinCopy(D2UnitStrc *pUnit, int32_t nDrawParam, BOOL bFirstThird);
int32_t __fastcall SKILLS_CltStFunc31(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc45(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
void __fastcall SKILLS_StopWhirlwindSound(D2UnitStrc *pUnit, D2C_Skills nSkillId);

//  D2Client.0x6FAC2840 .. 0x6FAC2BE0, Skills\SkillsDru.cpp -- the shape-shift overlay (`cltstfunc` 45: Wearwolf, Wearbear, Delerium Change),
//  the seeded volley, and `cltdofunc` 67..69 (Firestorm, Twister, Tornado).
BOOL __fastcall SKILLS_CltStFunc45(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_SpawnSeededMissileVolley(D2UnitStrc *pUnit, int32_t nCount, D2C_MissilesHcIdx nMissileId, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nAnimSpeed, int32_t bFirstAimedAtTargetUnit, int32_t bRandomFacing);
int32_t __fastcall SKILLS_CltDoFunc67(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc68(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc69(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
//  `cltprgfunc1` 92 and 91 (Armageddon and Diablogeddon, Volcano), run by packet 0xA3. The hurricane state's two are declared in the state
//  handlers' block above.
int32_t __fastcall SKILLS_CltDoFunc92(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc91(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);

//  D2Client.0x6FAC3310 .. 0x6FAC3C90, Skills\SkillsEMon.cpp -- the Wake of Destruction sentry, Imp Inferno and Imp Fireball (`cltdofunc`
//  70..72), Baal's taunt, the catapults and the teleports' start (`cltstfunc` 46..48), and the teleports' do (`cltdofunc` 73 and 74).
int32_t __fastcall SKILLS_CltDoFunc70(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc71(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc72(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc46(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc47(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc48(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_CltDoFunc73(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc74(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
//  The vines' start (`cltstfunc` 49), the Siege Beast's stomp and the Death Maul (`cltdofunc` 75 and 76), the inferno sentry's, the death
//  sentry's and Fenris's start and do (`cltstfunc` 52, 50, 51; `cltdofunc` 77, 78, 79).
int32_t __fastcall SKILLS_CltStFunc49(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc75(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc76(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc52(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc77(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc50(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc78(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc51(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc79(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
//  The cyclers' progressive (`cltprgfunc1` 93), the two slots no stock row names (80, 82) and Baal's cold missiles (`cltdofunc` 81). The
//  `attached` and `vine_beast` states' two are declared in the state handlers' block above.
int32_t __fastcall SKILLS_CltDoFunc93(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc80(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc81(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc82(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);

//  D2Client.0x6FAC51E0 .. 0x6FAC5A50, Skills\SkillsMon.cpp -- the sand maggots (`cltdofunc` 46 and 47, `cltstfunc` 32 and 33; slot 52,
//  Submerge, holds `cltdofunc` 47's address), Andariel's spray (`cltstfunc` 34, `cltdofunc` 48), the swarm's move (`cltstfunc` 35,
//  `cltdofunc` 49) and the nests (`cltstfunc` 36, `cltdofunc` 50).
int32_t __fastcall SKILLS_CltDoFunc46(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc32(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc33(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc47(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc34(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc48(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc35(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc49(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc36(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc50(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);

//  D2Client.0x6FAC5B50 .. 0x6FAC6460, Skills\SkillsMon.cpp -- the gargoyle trap (`cltdofunc` 51), submerging and emerging (`cltstfunc` 37 and
//  38), the fetish aura and the infernos (`cltdofunc` 53 and 54, over SKILLS_MissileCltSpawnWithSkillRange), resurrecting (`cltstfunc` 39),
//  the prime poison nova (`cltdofunc` 55) and Diablo's lightning (`cltstfunc` 40).
int32_t __fastcall SKILLS_CltDoFunc51(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc37(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc53(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc54(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
D2UnitStrc *__fastcall SKILLS_MissileCltSpawnWithSkillRange(D2UnitStrc *pUnit, D2C_MissilesHcIdx nMissileId, const int32_t *pVelJitter, int32_t nTargetX, int32_t nTargetY, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc38(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc39(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc55(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc40(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);

//  D2Client.0x6FAC64E0 .. 0x6FAC6B70, Skills\SkillsMon.cpp -- Diablo's lightning (`cltdofunc` 56), the finger mage's spider (57), Diablo's
//  wall of fire (58, its missiles started by SKILLS_MissileStartSeededPathOrStall, stored in the block as the start) and Diablo's run
//  (`cltstfunc` 41, `cltdofunc` 59).
int32_t __fastcall SKILLS_CltDoFunc56(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc57(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc58(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
void __fastcall SKILLS_MissileStartSeededPathOrStall(D2UnitStrc *pMissile, int32_t nSeed);
int32_t __fastcall SKILLS_CltStFunc41(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc59(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);

//  D2Client.0x6FAC6DB0 .. 0x6FAC75C0, Skills\SkillsMon.cpp -- the desert turret (`cltdofunc` 60), the arcane tower (61), the mosquito
//  (`cltstfunc` 42, `cltdofunc` 62), a start no row names (`cltstfunc` 43), the regurgitator's meal (63) and the maggot queen's death (64).
int32_t __fastcall SKILLS_CltDoFunc60(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc61(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc42(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc62(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc43(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc63(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc64(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);

//  D2Client.0x6FAC7650 .. 0x6FAC7D60, Skills\SkillsMon.cpp -- a slot no row names (65), Hell Meteor's start (`cltstfunc` 44), the doom
//  knight's and the necromage's missiles (`cltdofunc` 94 and 95), the hirelings' missile (66) and the spider's trail.
int32_t __fastcall SKILLS_CltDoFunc65(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc44(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc94(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc95(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc66(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
D2UnitStrc *__fastcall SKILLS_LaySpiderGooTrail(D2UnitStrc *pUnit);

//  D2Client.0x6FAC7F90 .. 0x6FAC8440, Skills\SkillsNec.cpp -- the curses (`cltstfunc` 18, `cltdofunc` 30), Teeth and Bone Spear's cast
//  (`cltstfunc` 19) and raising skeletons and mages (`cltstfunc` 20, `cltdofunc` 31).
int32_t __fastcall SKILLS_CltStFunc18(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc30(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc19(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc20(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc31(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);

//  D2Client.0x6FAC8530 .. 0x6FAC8D00, Skills\SkillsNec.cpp -- the two corpse explosions (`cltstfunc` 21, `cltdofunc` 32 and 33 and the effects
//  they hand their parameters to), the bone wall and prison (`cltstfunc` 22), the iron golem (23) and Revive (24).
int32_t __fastcall SKILLS_CltStFunc21(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_SpawnCorpseExplosionEffect(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nX, int32_t nY, D2C_MonsterIds nCorpseClassId);
int32_t __fastcall SKILLS_CltDoFunc32(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_SpawnPoisonExplosionEffect(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel, int32_t nX, int32_t nY, D2C_MonsterIds nCorpseClassId);
int32_t __fastcall SKILLS_CltDoFunc33(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc22(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc23(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_CltStFunc24(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);

//  D2Client.0x6FAC8EE0 .. 0x6FAC91F0, Skills\SkillsPal.cpp -- Sacrifice's blood, Blessed Hammer and Fist of the Heavens (`cltdofunc` 34..36);
//  the `sanctuary` state's function is declared in the state handlers' block above.
int32_t __fastcall SKILLS_CltDoFunc34(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc35(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc36(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);

//  D2Client.0x6FAC9620 .. 0x6FAC9E50, Skills\SkillsPal.cpp -- Charge: its motion trail (called from PlrSkills.cpp), its start (`cltstfunc`
//  25), its do (`cltdofunc` 37) and its end (called from SCmd.cpp and Skills.cpp for `srvdofunc` 67).
void __fastcall SKILLS_DrawMotionTrail(D2UnitStrc *pUnit, int32_t nDrawParam, int32_t bBehindUnit);
int32_t __fastcall SKILLS_CltStFunc25(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc37(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
void __fastcall SKILLS_EndUninterruptableSkill(D2UnitStrc *pUnit, D2C_Skills nSkillId);

//  D2Client.0x6FAC9E80 .. 0x6FACA3D0, Skills\SkillsSor.cpp -- Charged Bolt's seeded path (the init callback of every seeded volley) and its do
//  (`cltdofunc` 23), Inferno's start (`cltstfunc` 15), the animation helper (called from SkillsEMon.cpp), Inferno's stream (called from
//  SkillsEMon.cpp too) and its do (`cltdofunc` 24), and the `cltstfunc` 16 no row names.
void __fastcall SKILLS_MissileStartSeededPath(D2UnitStrc *pMissile, int32_t nSeed);
int32_t __fastcall SKILLS_CltDoFunc23(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc15(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
void __fastcall SKILLS_SetMissileAnimSpeedDivisor(D2UnitStrc *pUnit, int32_t nDivisor);
D2UnitStrc *__fastcall SKILLS_SpawnInfernoStream(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_CltDoFunc24(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc16(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);

//  D2Client.0x6FACA570 .. 0x6FACADF0, Skills\SkillsSor.cpp -- the novas, Fire Wall, Chain Lightning, Meteor's and Blizzard's centres and
//  Frozen Orb (`cltdofunc` 25..29), Chain Lightning's `ItemCltEffect` (slot 96) and Hydra's start (`cltstfunc` 17).
int32_t __fastcall SKILLS_CltDoFunc25(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc26(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc27(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltDoFunc96(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_CltDoFunc28(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
int32_t __fastcall SKILLS_CltStFunc17(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);
BOOL __fastcall SKILLS_CltDoFunc29(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);

//  D2Client.0x6FACAE70 .. 0x6FACB070, Skills\SkillsSor.cpp -- Blaze's trail (called from UNITMODE_StepUnitAlongPath) and Thunder Storm's
//  strike (`cltprgfunc1` 84); the two state functions of the unit are declared in the state handlers' block above.
void __fastcall SKILLS_LayBlazeTrail(D2UnitStrc *pUnit);
int32_t __fastcall SKILLS_CltDoFunc84(D2UnitStrc *pUnit, D2C_Skills nSkillId, int32_t nSkillLevel);

//  D2Client.0x6FB2FF20 .. 0x6FB306F0, UNIT\Missile.cpp -- the ground fire spray, pCltDoFunc 2, 3, 4 and 5, the trail sub-missile (nLoops is
//  overwritten) and the cloud puffs. The pCltDoFunc handlers are `__thiscall` in the original and its table; with one argument,
//  `__fastcall` is the same ABI.
void __fastcall MISSILE_SprayRandomOverArea(D2UnitStrc *pMissile, int32_t *paMissileIds, int32_t nMissileCount, int32_t nRadius, int32_t nRange, int32_t nChancePercent);
void __fastcall MISSILE_CltDoFunc2(D2UnitStrc *pMissile);
D2UnitStrc *__fastcall MISSILE_CltSpawnTrailSubMissile(D2UnitStrc *pMissile, int32_t nSubMissileId, int32_t nRange, int32_t nLoops);
void __fastcall MISSILE_CltDoFunc3(D2UnitStrc *pMissile);
void __fastcall MISSILE_EmitCloudPuffs(D2UnitStrc *pMissile, int32_t nEveryNFrames, int32_t nCount, int32_t nSpread, int32_t nPuffMissileId);
void __fastcall MISSILE_CltDoFunc4(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc5(D2UnitStrc *pMissile);

//  D2Client.0x6FB30820 .. 0x6FB30E80, UNIT\Missile.cpp -- pCltDoFunc 6 (the fire-wall makers), 7 (the homing step), 8 (the lightning bolts)
//  and 9 (the meteor centre), and the on-spawn hook pCltDoFunc 8 and 22 store, which no code calls.
void __fastcall MISSILE_CltDoFunc6(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc7(D2UnitStrc *pMissile);
void __fastcall MISSILE_RandomiseSpawnAppearance(D2UnitStrc *pMissile, D2UnitStrc *pParent);
void __fastcall MISSILE_CltDoFunc8(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc9(D2UnitStrc *pMissile);

//  D2Client.0x6FB31180 .. 0x6FB31990, UNIT\Missile.cpp -- the periodic drop behind pCltDoFunc 10, 13 and 24, pCltDoFunc 10 and 13 (the
//  blizzard centres), 11 (the end of a one-cycle animation), 12 (a screen shake) and 67 (three timed sub-missiles); 12 and 67 have no stock row.
D2UnitStrc *__fastcall MISSILE_DropPeriodicSubMissile(D2UnitStrc *pMissile, int32_t nRadius, int32_t nPeriod, int32_t nSubMissileId);
void __fastcall MISSILE_CltDoFunc10(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc11(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc12(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc67(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc13(D2UnitStrc *pMissile);

//  D2Client.0x6FB31BA0 .. 0x6FB32470, UNIT\Missile.cpp -- pCltDoFunc 14 (a homing step, no stock row), 15 (fingermagespider: the step and a
//  trail of pieces), 16 (Diablo's fire wall), 17 (the curse and mind blast centres) and 18 (the bone spear's trail).
void __fastcall MISSILE_CltDoFunc14(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc15(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc16(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc17(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc18(D2UnitStrc *pMissile);

//  D2Client.0x6FB32610 .. 0x6FB32CB0, UNIT\Missile.cpp -- pCltDoFunc 19 (the frozen orb's spray), 20 (its nova's turn), 21 and 22 (a death
//  sequence's bolts and their turning trail) and 23 (a light that never runs out).
void __fastcall MISSILE_CltDoFunc19(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc20(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc21(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc22(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc23(D2UnitStrc *pMissile);

//  D2Client.0x6FB32CE0 .. 0x6FB33440, UNIT\Missile.cpp -- pCltDoFunc 24 (the Cairn Stones opening), 25 (the mist trails) and 26 (Mephisto's
//  death).
void __fastcall MISSILE_CltDoFunc24(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc25(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc26(D2UnitStrc *pMissile);

//  D2Client.0x6FB33680 .. 0x6FB33D50, UNIT\Missile.cpp -- the scattered-target spawn of pCltDoFunc 26 and 29, pCltDoFunc 27 (the four-way
//  zig-zag of Mephisto's and Andariel's fire walls) and 28 (a burst round the owner on one frame), and Andariel's death cloud: its scattered
//  burst and its per-frame step.
D2UnitStrc *__fastcall MISSILE_CltSpawnScatteredSubMissile(D2UnitStrc *pOwner, D2UnitStrc *pMissile, int32_t nSubMissileId, int32_t nSpread);
void __fastcall MISSILE_CltDoFunc27(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc28(D2UnitStrc *pMissile);
void __fastcall MISSILE_SpawnScatteredSubMissiles(D2UnitStrc *pOwner, D2UnitStrc *pMissile, int32_t nMissileId, int32_t nDensity);
void __fastcall MISSILE_StepAndarielCloud(D2UnitStrc *pOwner, D2UnitStrc *pMissile, int32_t nMissileId, int32_t nFrame, int32_t nStage);

//  D2Client.0x6FB33F90 .. 0x6FB347B0, UNIT\Missile.cpp -- the falling debris of Andariel's death (a random piece of a range, dropped at a
//  point of a disc) and the handler that runs her whole death, pCltDoFunc 30 (the tower chest's mist), and 43 and 44, the two missiles that
//  ride their owner.
void __fastcall MISSILE_SpawnRandomSubMissile(D2UnitStrc *pOwner, D2UnitStrc *pMissile, int32_t nMissileId, int32_t nRadius);
void __fastcall MISSILE_CltDoFunc29(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc30(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc43(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc44(D2UnitStrc *pMissile);

//  D2Client.0x6FB348F0 .. 0x6FB35110, UNIT\Missile.cpp -- pCltDoFunc 45 (the distraction fog's puffs), 46 (the lightning javelins' side
//  trails), 68 (a trail from each step), 47 (the molten boulder's bounce sound), 48 (Fissure's cracks), 49 (the vines' trail) and 50
//  (Volcano's debris).
void __fastcall MISSILE_CltDoFunc45(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc46(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc68(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc47(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc48(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc49(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc50(D2UnitStrc *pMissile);

//  D2Client.0x6FB353C0 .. 0x6FB35B40, UNIT\Missile.cpp -- pCltDoFunc 51 (the recyclers' vine and burst), 52 (a pair of trails either side),
//  53 (tigerfury's trail), 56 (Baal's taunt control), 57 (the blade shield's riding blades) and 58 (a path that wobbles, royal strike's
//  chaos ice).
void __fastcall MISSILE_CltDoFunc51(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc52(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc53(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc56(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc57(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc58(D2UnitStrc *pMissile);

//  D2Client.0x6FB35D10 .. 0x6FB36570, UNIT\Missile.cpp -- a missile on every subtile of a disc, and two rings of one from a missile; then the
//  first of the pCltHitFunc handlers (gpMissileCltHitFuncs, whose answer MISSILE_ClientOnHit tests): 64 (eight directions, no stock row),
//  1 (a fire burst), 2 (the gas clouds' rings), 3 (the potions' explosion and debris) and 9 (the holy bolts).
void __fastcall MISSILE_SpawnOverArea(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nRadius, int32_t nMissileId, int32_t nSkillId, int32_t nLevel, int32_t nOneInChance, int32_t nStep);
void __fastcall MISSILE_SpawnFromMissile(D2UnitStrc *pMissile, int32_t nMissileId, int32_t nRingDirectionStep, int32_t nFullRingStep, int32_t nCltParam);
int32_t __fastcall MISSILE_CltHitFunc64(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc1(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc2(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc3(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
BOOL __fastcall MISSILE_CltHitFunc9(D2UnitStrc *pMissile, D2UnitStrc *pTarget);

//  D2Client.0x6FB366D0 .. 0x6FB36DF0, UNIT\Missile.cpp -- pCltHitFunc 10 (the progressive overlay on the unit hit, 23 rows), 12 (immolation
//  arrow's fire), 13 (the homing arrow's and bone spirit's hit: 1 taken, 0 flies on), 14 (the ice burst's shards) and 16 (the chain
//  lightning's next jump).
int32_t __fastcall MISSILE_CltHitFunc10(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc12(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
BOOL __fastcall MISSILE_CltHitFunc13(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc14(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc16(D2UnitStrc *pMissile, D2UnitStrc *pTarget);

//  D2Client.0x6FB37060 .. 0x6FB37800, UNIT\Missile.cpp -- pCltHitFunc 18 (the meteor's impact), 19 (an explosion from a range of ids), 24
//  (bone spear's splinters; answers 0), 25 and 26 (bolts forked to the units around: Lightning Fury, Fist of the Heavens) and the visitor the
//  last two hand SKILLS_ForEachNearbyTarget.
int32_t __fastcall MISSILE_CltHitFunc18(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc19(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc24(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc25(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_SpawnOnUnitCallback(D2ClientNearbyTargetCtxStrc *pCtx, D2UnitStrc *pUnit);
int32_t __fastcall MISSILE_CltHitFunc26(D2UnitStrc *pMissile, D2UnitStrc *pTarget);

//  D2Client.0x6FB379F0 .. 0x6FB38160, UNIT\Missile.cpp -- a ring of a missile around a unit and pCltHitFunc 28 (the green panther pot's gas
//  ring), 29 (Grim Ward's stages; the level stored as the skill), 30 and 4 (the frozen orb's 64-point ring, twice), 32 (the fire head's bite)
//  and 33 (Mephisto's big rock breaking).
int32_t __fastcall MISSILE_SpawnRingOfMissiles(D2UnitStrc *pOwner, D2UnitStrc *pOrigin, int32_t nMissileId, int32_t nSkillId, int32_t nLevel, int32_t nStep);
int32_t __fastcall MISSILE_CltHitFunc28(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc29(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc30(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc4(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc32(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc33(D2UnitStrc *pMissile, D2UnitStrc *pTarget);

//  D2Client.0x6FB38360 .. 0x6FB38BF0, UNIT\Missile.cpp -- pCltHitFunc 34 (hides Mephisto), 36 (a falling meteor's fire), 44 (a missile
//  landing), the charged ball's bolts' initialiser and pCltHitFunc 46, 47 (the spike ball), 50 and the boulder's impact it shares with 52,
//  and 51 (the trailing javelins' bolts).
int32_t __fastcall MISSILE_CltHitFunc34(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc36(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc44(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
void __fastcall MISSILE_StartSeededPathFromOrigin(D2UnitStrc *pMissile);
int32_t __fastcall MISSILE_CltHitFunc46(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc47(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc50(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
void __fastcall MISSILE_SpawnBoulderImpactDebris(D2UnitStrc *pMissile, int32_t nRockCount, int32_t nRockVelocity, int32_t nFlyingRockMissile, int32_t nGroundFireMissile);
int32_t __fastcall MISSILE_CltHitFunc51(D2UnitStrc *pMissile, D2UnitStrc *pTarget);

//  D2Client.0x6FB38D20 .. 0x6FB39650, UNIT\Missile.cpp -- pCltHitFunc 52 (the rolling boulder's impact), 53 (the boulder emerging), 54 and 56
//  (vines withering and fading), 55 (a rock landing), 57 (Baal's summoning), 60 (Nihlathak's corpse) and 61 (the worldstone shake's end); the
//  light-quality setter and getter; and three of gaMissileTargetTests' predicates.
int32_t __fastcall MISSILE_CltHitFunc52(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc53(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc54(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc55(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc56(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc57(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc60(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc61(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
void __fastcall MISSILE_EnvSetLightQuality(int32_t nQuality);
int32_t __cdecl MISSILE_GetLightQuality();
BOOL __fastcall MISSILE_IsMonsterTarget(D2UnitStrc *pUnit, D2ClientMissileTargetCtxStrc *pCtx);
BOOL __fastcall MISSILE_IsEnemyTarget(D2UnitStrc *pUnit, D2ClientMissileTargetCtxStrc *pCtx);
BOOL __fastcall MISSILE_IsPlayerOrMonsterTarget(D2UnitStrc *pUnit, D2ClientMissileTargetCtxStrc *pCtx);

//  D2Client.0x6FB396F0 .. 0x6FB39ED0, UNIT\Missile.cpp -- the fourth target predicate (missiles that can be shot down); a missile unit's
//  client half, made and freed; pCltHitFunc 31 (ice melting) and 40 (the healing bolt taken by the owner's pet); pCltDoFunc 37 (Diablo
//  appearing); and the hf* chain -- pCltDoFunc 38, 39, 40, 41 and pCltHitFunc 42, 43.
BOOL __fastcall MISSILE_IsTargetableMissile(D2UnitStrc *pUnit);
void __fastcall MISSILE_InitClientUnit(D2UnitStrc *pUnit, D2ActiveRoomStrc *pRoom, int32_t nX, int32_t nY, int32_t nUnused);
void __fastcall MISSILE_FreeUnitData(D2UnitStrc *pUnit);
int32_t __fastcall MISSILE_CltHitFunc31(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc40(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
void __fastcall MISSILE_CltDoFunc37(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc41(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc40(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc39(D2UnitStrc *pMissile);
int32_t __fastcall MISSILE_CltHitFunc42(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc43(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
void __fastcall MISSILE_CltDoFunc38(D2UnitStrc *pMissile);

//  D2Client.0x6FB3A130 .. 0x6FB3A960, UNIT\Missile.cpp -- pCltDoFunc 42 (izual control), 31 (the horadric staff's seven lights), pCltHitFunc
//  37 (the hand of god's holy bolts) and pCltDoFunc 32 (Radament's death, which spawns the hands).
void __fastcall MISSILE_CltDoFunc42(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc31(D2UnitStrc *pMissile);
int32_t __fastcall MISSILE_CltHitFunc37(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
void __fastcall MISSILE_CltDoFunc32(D2UnitStrc *pMissile);

//  D2Client.0x6FB3AAA0 .. 0x6FB3B3B0, UNIT\Missile.cpp -- pCltDoFunc 33 (the Tainted Sun's control), pCltHitFunc 38 (a flash spawning a
//  ball), pCltDoFunc 34 (the ball, by the control's phase), pCltDoFunc 35 (the Maggot Queen's death spawning globs) and pCltHitFunc 39 (a
//  glob's splat).
void __fastcall MISSILE_CltDoFunc33(D2UnitStrc *pMissile);
int32_t __fastcall MISSILE_CltHitFunc38(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
void __fastcall MISSILE_CltDoFunc34(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc35(D2UnitStrc *pMissile);
int32_t __fastcall MISSILE_CltHitFunc39(D2UnitStrc *pMissile, D2UnitStrc *pTarget);

//  D2Client.0x6FB3B4B0 .. 0x6FB3BDA0, UNIT\Missile.cpp -- pCltDoFunc 36 (Duriel's death), pCltHitFunc 41 (its rocks), pCltDoFunc 61 (the
//  Overseer's catapults) and its shot, pCltDoFunc 62 (Nihlathak's vanishing) and its debris.
void __fastcall MISSILE_CltDoFunc36(D2UnitStrc *pMissile);
int32_t __fastcall MISSILE_CltHitFunc41(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
void __fastcall MISSILE_CltDoFunc61(D2UnitStrc *pMissile);
void __fastcall MISSILE_LaunchCatapultShot(D2UnitStrc *pMonster, D2UnitStrc *pMissile, int32_t nMissileId);
void __fastcall MISSILE_CltDoFunc62(D2UnitStrc *pMissile);
void __fastcall MISSILE_SpawnNihlathakDebris(D2UnitStrc *pOwner, D2UnitStrc *pMissile);

//  D2Client.0x6FB3BFE0 .. 0x6FB3C8A0, UNIT\Missile.cpp -- pCltDoFunc 63 (Nihlathak's debris turning), 64 (Baal's death: the spirits, the
//  heads, Tyrael's debris) and its falling piece, 65 (the spirits) and 66 (the worldstone shake riding the player).
void __fastcall MISSILE_CltDoFunc63(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc64(D2UnitStrc *pMissile);
void __fastcall MISSILE_SpawnFallingSubMissile(D2UnitStrc *pMissile, int32_t nX, int32_t nY);
void __fastcall MISSILE_CltDoFunc65(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc66(D2UnitStrc *pMissile);

//  D2Client.0x6FB3CA50 .. 0x6FB3D200, UNIT\Missile.cpp -- pCltDoFunc 59 (the worldstone chips), pCltHitFunc 48 (the catapult cold ball
//  landing), pCltDoFunc 54 (Anya's steam) and its vent, and pCltDoFunc 55 (the Ancients' death cloud).
void __fastcall MISSILE_CltDoFunc59(D2UnitStrc *pMissile);
int32_t __fastcall MISSILE_CltHitFunc48(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
void __fastcall MISSILE_CltDoFunc54(D2UnitStrc *pMissile);
void __fastcall MISSILE_SpawnSteamVentNearby(D2UnitStrc *pOwner, D2UnitStrc *pMissile, int32_t nX, int32_t nY);
void __fastcall MISSILE_CltDoFunc55(D2UnitStrc *pMissile);

//  D2Client.0x6FB3D380 .. 0x6FB3DBB0, UNIT\Missile.cpp -- pCltDoFunc 60 (Hurricane's debris), pCltHitFunc 62 and 63 (Baal's taunt controls),
//  a missile's client frame and its do-function table, pCltDoFunc 1 (the flight step every other do function ends in) and a client missile's
//  end.
void __fastcall MISSILE_CltDoFunc60(D2UnitStrc *pMissile);
int32_t __fastcall MISSILE_CltHitFunc62(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
int32_t __fastcall MISSILE_CltHitFunc63(D2UnitStrc *pMissile, D2UnitStrc *pTarget);
void __fastcall MISSILE_ClientUpdate(D2UnitStrc *pMissile);
void __fastcall MISSILE_CltDoFunc1(D2UnitStrc *pMissile);
void __fastcall MISSILE_DestroyClientMissile(D2UnitStrc *pMissile);

//  D2Client.0x6FB3DC50, UNIT\Missile.cpp -- a client missile's hit and its hit-function table.
D2UnitStrc *__fastcall MISSILE_ClientOnHit(D2UnitStrc *pMissile, D2UnitStrc *pTarget, int32_t bForceExplode);

//  D2Client.0x6FB3E070, UNIT\Missile.cpp -- the client missile spawner every other one goes through.
D2UnitStrc *__fastcall MISSILE_ClientSpawn(D2MissileStrc *pParams);

//  D2Client.0x6FB3EAF0 .. 0x6FB3ED40, UNIT\Missile.cpp -- the three MISSILE_ClientSpawn wrappers (at a point, from a unit for its owner, at
//  where the owner is heading) and a skill effect's spawn.
D2UnitStrc *__fastcall MISSILE_ClientSpawnSimple(D2UnitStrc *pOwner, int32_t nMissileId, int32_t nSpawnX, int32_t nSpawnY, int32_t nSkillId, int32_t nSkillLevel);
D2UnitStrc *__fastcall MISSILE_ClientSpawnForOwner(D2UnitStrc *pUnit, int32_t nMissileId, int32_t nTargetX, int32_t nTargetY, int32_t nSkillId, int32_t nSkillLevel);
D2UnitStrc *__fastcall MISSILE_ClientSpawnSkillEffect(D2UnitStrc *pUnit, int32_t nMissileId, int32_t nSpawnOffsetX, int32_t nSpawnOffsetY, int32_t nTargetX, int32_t nTargetY, int32_t nSkillId, int32_t nSkillLevel, int32_t nGfxArg);
D2UnitStrc *__fastcall MISSILE_ClientSpawnAtPathTarget(D2UnitStrc *pUnit, int32_t nMissileId, int32_t nSkillId, int32_t nSkillLevel);

//  Shared.cpp holds the functions no unit can claim.
//
//  D2Common's two seed routines, out of line at 0x6FAA88D0 and 0x6FAA88F0: one step of the seed, stored back and returned whole; a roll below
//  nMax that advances it. SEED_RollLimitedRandomNumberInline has no address: the header's inline, for the sites that inline it rather than
//  call 0x6FAA88F0.
uint64_t __fastcall SEED_RollRandomNumber(D2SeedStrc *pSeed);
int32_t __fastcall SEED_RollLimitedRandomNumber(D2SeedStrc *pSeed, int32_t nMax);
uint32_t __fastcall SEED_RollLimitedRandomNumberInline(D2SeedStrc *pSeed, int32_t nMax);

//  The other functions whose inlined copies stand in several modules: the room that holds a subtile (NULL for a NULL room), D2Common's
//  monstats and skills record lookups (NULL out of range), the monstats2 row through wMonStatsEx (monstats2 is not indexed by class id),
//  D2Common's UNITS_GetXPosition/YPosition pair, the four-way scaled multiply-divide D2_ApplyRatio (ecx nValue, edx nMultiplier, the stack
//  nDivisor), the clear of a player's queued command, the command's setter and getter, and D2Common's setitems.txt lookup.
D2ActiveRoomStrc *__fastcall CLIENT_FindRoomContainingSubtile(D2ActiveRoomStrc *pRoom, int32_t nSubtileX, int32_t nSubtileY);
D2MonStatsTxt *__fastcall DATATBLS_GetMonStatsTxtRecord(int32_t nMonStatsId);
int32_t __fastcall UNITS_GetXPosition(D2UnitStrc *pUnit);
int32_t __fastcall UNITS_GetYPosition(D2UnitStrc *pUnit);
int32_t __fastcall DATATBLS_ApplyRatio(int32_t nValue, int32_t nMultiplier, int32_t nDivisor);
D2SetItemsTxt *__fastcall DATATBLS_GetSetItemsTxtRecord(int32_t nFileIndex);
void __fastcall PLAYER_ClearQueuedCommand(D2UnitStrc *pPlayer);
D2SkillsTxt *__fastcall DATATBLS_GetSkillsTxtRecordById(int32_t nSkillId);
D2MonStats2Txt *__fastcall DATATBLS_GetMonStats2TxtRecord(int32_t nMonStatsId);
void __fastcall PLAYER_SetQueuedCommand(int32_t nCmd, D2UnitStrc *pPlayer, int32_t nTargetType, D2UnitGUID nTargetGUID);
int32_t __fastcall PLAYER_GetQueuedCommand(D2UnitStrc *pPlayer);
D2InventoryStrc *__fastcall GFXUTIL_GetUnitDisplayInventory(D2UnitStrc *pUnit, BOOL *pbFromOwner);
D2OverlayTxt *__fastcall GFXUTIL_GetOverlayTxtRecord(int32_t nOverlayId);

//  D2Client.0x6FB522F0 -- monstats.txt's BaseId of a monster unit, the class id where its row cannot be read, -1 for NULL or another type.
int32_t __fastcall MONSTER_GetBaseMonsterId(D2UnitStrc *pUnit);

//  Handlers nothing registers, and orphans.
void __stdcall UI_SendWmCloseAndReset(SMSGHANDLER_PARAMS *pMsg);
int16_t __fastcall ROSTER_GetPartyIdForUnit(D2UnitStrc *pUnit);

//  D2Client.0x6FB3EDF0 / 0x6FB3EFB0 -- two orphans between Missile.cpp and Monster.cpp: up to nCount client-only monsters of one class placed
//  near a unit, and a room's critters from its levels.txt row.
void __fastcall UNITS_FindFreeSpotForMonster(D2UnitStrc *pUnit, int32_t nMonStatsId, int32_t nCount);
void __fastcall ENV_SpawnRoomCritters(D2ActiveRoomStrc *pRoom);

//  D2Client.0x6FADCF10 / 0x6FADCFE0 -- the item-name formatter, two orphans between the disc prompts and inv.cpp: `%N` replaced in place, and
//  the variadic, NULL-terminated formatter with its 'a'/'n' grammar directive.
void __fastcall LANG_SubstituteArg(Unicode *wszText, int32_t nIndex, const Unicode *wszValue);
void __cdecl LANG_FormatString(Unicode *wszOut, const Unicode *wszFormat, ...);

//  D2Client.0x6FB0A440 / 0x6FB6BD9A / 0x6FB6BDAB -- three orphans: a skill's skilldesc.txt string as a tbl index (STR_IDX_5382_dummy on any
//  failed bound), and View.cpp's executable VirtualAlloc and its VirtualFree, which lie after D2Hell's Fault.cpp.
int16_t __fastcall SKILLS_GetSkillDescStringIndex(D2C_Skills nSkillId, int32_t nStringField);
LPVOID __fastcall CLIENT_VirtualAlloc(SIZE_T dwSize);
BOOL __fastcall CLIENT_VirtualFree(LPVOID lpAddress);

//  ---- CORE\ARCHIVE.CPP -- CoreArchive.cpp ----
//  A cel file by name -- MAX_PATH copy, the extension for nGfxType, read whole from D2Win's archive, normalised -- or 0; the second also hands
//  back the file's size; the third frees one. The handle is a D2CellFileStrc *. CELLFILE_, not LoadImage: a Win32 macro.
D2CellFileStrc *__fastcall ARCHIVE_CellFileLoad(char *pszName, int32_t nGfxType);
D2CellFileStrc *__fastcall ARCHIVE_CellFileLoadWithSize(char *pszName, uint32_t *pdwFileSize, int32_t nGfxType);
void __fastcall ARCHIVE_CellFileFree(D2CellFileStrc *pCellFile);

//  ---- CORE\Lng.cpp ----
//  The language module: the `-lng` switch, the load (the switch's path, the registry's, D2.LNG), the free, and the message source registered
//  with Fault.cpp, which nothing in D2Client calls.
int32_t __cdecl LNG_RegisterCmdLineArguments(void);
int32_t __cdecl LNG_Initialize(void);
void __cdecl LNG_Shutdown(void);
int32_t __stdcall LNG_MessageSource(DWORD dwMessageId, LPSTR pszBuffer, int32_t nMaxLength);

//  ---- UNIT\PlayerList.cpp ----
//  Each roster player's list of D2RosterInfoStrc, one per other player: the head made and freed, an entry found or made, a flag set, tested
//  (the masked word, not a BOOL), the low word replaced. GetOrAdd is void: no caller reads what it leaves in eax.
D2RosterInfoStrc **__cdecl PLAYERLIST_RosterAllocPlayerInfoList(void);
void __fastcall PLAYERLIST_RosterFreePlayerInfoList(D2RosterInfoStrc **ppInfoHead);
void __fastcall PLAYERLIST_RosterGetOrAddInfoEntry(uint32_t dwPlayerGUID, uint32_t dwTargetGUID);
void __fastcall PLAYERLIST_RosterSetInfoFlag(uint32_t dwPlayerGUID, uint32_t dwTargetGUID, int32_t nFlag, int32_t bSet);
int32_t __fastcall PLAYERLIST_RosterTestInfoFlag(uint32_t dwPlayerGUID, uint32_t dwTargetGUID, D2C_RosterInfoFlags nFlag);
void __fastcall PLAYERLIST_RosterReplaceInfoFlags(uint32_t dwPlayerGUID, uint32_t dwTargetGUID, int32_t nFlags);

//  ---- UNIT\UnitGfx.cpp ----
//  A new unit's graphics record (asserts, halts on an unknown type), and the per-unit draw: skipped, the line-of-sight flag -- a unit the
//  local player cannot see is not drawn -- where, and which draw path.
void __fastcall UNITGFX_UnitInitGfx(D2UnitStrc *pUnit);
void __fastcall UNITGFX_GfxDrawUnit(D2UnitStrc *pUnit, uint32_t dwLight, int32_t bForce);

//  ---- D2Hell\SRC\Fault.cpp ----
//  D2Hell's fault module compiled into D2Client: the assert handler slot (stored, never called), the message-source list and its lock, the
//  OK/Cancel error box, and the Storm list template for TMESSAGESOURCE. The `__thiscall` members are `__fastcall` with an unused edx.
void *__fastcall FAULT_SetAssertHandler(void *pfnNewHandler);
void __fastcall FAULT_RegisterMessageSource(int32_t (__stdcall *pfnMessageSource)(DWORD dwMessageId, LPSTR pszBuffer, int32_t nMaxLength));
void __cdecl FAULT_Init(void);
void __cdecl FAULT_AtExit(void);
BOOL __fastcall FAULT_PromptErrorOrCancel(LPCSTR lpText);
void __fastcall FAULT_TLinkedListFreeAll(TSExplicitList *pList);
D2MessageSourceStrc *__fastcall FAULT_TLinkedListAllocNode(TSExplicitList *pList, int32_t nEdxUnused, uint32_t nLinkType, uint32_t nExtraBytes, uint32_t dwAllocFlags);
void __fastcall FAULT_TLinkUnlinkThunk(TSLink *pLink);
void __fastcall FAULT_TLinkedListLink(TSExplicitList *pList, int32_t nEdxUnused, D2MessageSourceStrc *pNode, uint32_t nLinkType, D2MessageSourceStrc *pExisting);
void __fastcall FAULT_TLinkUnlink(TSLink *pLink);

//  ---- Engine\GfxUtil.cpp ----
//  The helpers under Gfx.cpp's unit drawing: the class a shapeshifted unit is drawn as, the inventory it is drawn wearing, the five mode-token
//  caches, a unit's cel measured and culled, a cel culled, the components a unit has. GetUnitCelSize is void: no caller reads eax. Then the
//  request's identity and COF-name tokens, the request filled and resolved (it draws nothing), a cached monster COF's cel context, the token
//  padder, and the overlay.txt and uniqueitems.txt getters; the identity function, the three COF layer-byte accessors and the layer order, the
//  drawable-type test, the loaded-mode flag got and set, the facing turned into a cel direction, and a COF frame's bottom measured; a
//  component's draw mode from a fade or an item, its palette, whether it is drawn, and an overlay's frame count.
int32_t __fastcall GFXUTIL_GetEffectiveClassId(D2UnitStrc *pUnit);
void __cdecl GFXUTIL_CacheTokens(void);
void __fastcall GFXUTIL_GetUnitCelSize(D2UnitStrc *pUnit, int32_t nDirection, int32_t nFrame, int32_t *pnWidth, int32_t *pnHeight);
BOOL __fastcall GFXUTIL_IsUnitCelOnScreen(D2UnitStrc *pUnit, int32_t nX, int32_t nY);
int32_t __fastcall GFXUTIL_IsCelVisible(D2GfxCellStrc *pCel, int32_t nX, int32_t nY, int32_t bShear);
BOOL __fastcall GFXUTIL_UnitHasComponent(D2UnitStrc *pUnit, int32_t nComponent);
int32_t __fastcall GFXUTIL_ResolveUnitAnimIdentity(D2UnitStrc *pUnit, D2GfxDataStrc *pGfxData, int32_t *pnClass, int32_t nModeOverride);
int32_t __fastcall GFXUTIL_FillGfxData(D2GfxDataStrc *pGfxData, D2UnitStrc *pUnit, D2CellFileStrc *pCellFile, int32_t nDirection, int32_t nFrame, int32_t *pnClass, int32_t nComponent, int32_t nModeOverride);
int32_t __fastcall GFXUTIL_FillCelContextTokens(D2GfxDataStrc *pGfxData, int32_t nDirection, int32_t nFrame, int32_t nComponent, int32_t nAnimMode, int32_t dwWClass,
                                            D2ClientCofCacheNodeStrc *pCofNode, D2ClientMonCofTokensStrc *pMonTokens, D2MonStatsTxt *pMonStatsTxt, int32_t nCofDirection);
void __fastcall GFXUTIL_CopyTokenPadded(const char *pszToken, char *pszOut);
D2UniqueItemsTxt *__fastcall GFXUTIL_GetUniqueItemsTxtRecord(int32_t nUniqueItemId);
void *__fastcall GFXUTIL_ReturnArg(void *p);
int32_t __fastcall GFXUTIL_GetCofLayerOverrideTransLvl(uint8_t *pCofData, int32_t nComponent);
int32_t __fastcall GFXUTIL_GetCofLayerCastsShadow(uint8_t *pCofData, int32_t nComponent);
uint8_t *__fastcall GFXUTIL_GetCofLayerOrder(uint8_t *pCofData, int32_t nDirection, int32_t nFrame);
int32_t __fastcall GFXUTIL_GetCofLayerTransLvl(uint8_t *pCofData, int32_t nComponent);
BOOL __fastcall GFXUTIL_IsDrawableUnitType(D2UnitStrc *pUnit);
uint8_t __fastcall GFXUTIL_IsUnitModeLoaded(D2UnitStrc *pUnit, int32_t nMode);
void __fastcall GFXUTIL_SetUnitModeLoaded(D2UnitStrc *pUnit, int32_t nMode, uint8_t bLoaded);
void __fastcall GFXUTIL_ResolveAnimDirection(D2UnitStrc *pUnit, D2C_UnitTypes nUnitType, int32_t nClassId, int32_t nAnimMode, int32_t *pnUnitDirection, int32_t *pnCelDirection, int32_t *pnNumDirections, int32_t nCofDirections, int32_t bMayRemap);
int32_t __fastcall GFXUTIL_MeasureLayersBottomY(D2UnitStrc *pUnit, uint8_t *pCofData, uint8_t *pLayerOrder, int32_t *pnBottomY, int32_t nDirection, int32_t nFrame);
BOOL __fastcall GFXUTIL_ResolveUnitTransLevel(D2UnitStrc *pUnit, D2Composits nComponent, int32_t *pnTransLevel, int32_t nItemClassId);
uint8_t *__fastcall GFXUTIL_GetComponentColor(D2UnitStrc *pUnit, int32_t nComponent, uint8_t *pDefaultPalette);
BOOL __fastcall GFXUTIL_IsUnitComponentVisible(D2UnitStrc *pUnit, D2Composits nComponent);
int32_t __fastcall GFXUTIL_GetOverlayFrameCount(D2UnitStrc *pUnit, D2ClientGfxEntryStrc *pEntry);

//  ---- ENGINE\Particle.cpp ----
//  A unit's particle block, D2ClientParticleStrc at +30h of its gfx info: the block allocated and freed, its screen offsets and position read,
//  its position, velocity, acceleration and target set, a bounce started and its count read, the countdown ended; the per-frame step, a
//  vertical arc started, the attached flag, the motion ended and restarted, the motion's test, and the arrival's direction.
void __stdcall PARTICLE_AllocUnitParticles(D2UnitStrc *pUnit);
void __stdcall PARTICLE_FreeUnitParticles(D2UnitStrc *pUnit);
int32_t __stdcall PARTICLE_GetUnitParticleOffsetX(D2UnitStrc *pUnit);
int32_t __stdcall PARTICLE_GetUnitParticleOffsetY(D2UnitStrc *pUnit);
int32_t __stdcall PARTICLE_GetUnitParticleOffsetZ(D2UnitStrc *pUnit);
int32_t __stdcall PARTICLE_GetUnitGfxCoordX(D2UnitStrc *pUnit);
int32_t __stdcall PARTICLE_GetUnitGfxCoordY(D2UnitStrc *pUnit);
int32_t __stdcall PARTICLE_GetUnitGfxCoordZ(D2UnitStrc *pUnit);
int32_t __stdcall PARTICLE_GetUnitGfxCoordZRaw(D2UnitStrc *pUnit);
D2ClientParticleStrc *__stdcall PARTICLE_SetUnitGfxCoords(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nZ);
D2ClientParticleStrc *__stdcall PARTICLE_SetUnitGfxVector(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nZ, int32_t bScale);
D2ClientParticleStrc *__stdcall PARTICLE_SetUnitGfxVector2(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nZ, int32_t bScale);
D2ClientParticleStrc *__stdcall PARTICLE_SetUnitParticleVector(D2UnitStrc *pUnit, int32_t nX, int32_t nY, int32_t nZ, int32_t bFixedPoint);
D2ClientParticleStrc *__stdcall PARTICLE_StartUnitBounce(D2UnitStrc *pUnit, int32_t nBouncesLeft, int32_t nRestitution);
int32_t __stdcall PARTICLE_GetUnitBouncesLeft(D2UnitStrc *pUnit);
D2ClientParticleStrc *__fastcall PARTICLE_ClearUnitTimedMotion(D2UnitStrc *pUnit);
void __stdcall PARTICLE_UpdateUnitParticle(D2UnitStrc *pUnit);
void __fastcall PARTICLE_StartUnitArc(D2UnitStrc *pUnit, int32_t nStartHeight, int32_t nDurationFrames);
D2ClientParticleStrc *__fastcall PARTICLE_SetUnitAttached(D2UnitStrc *pUnit, int32_t bAttached);
D2ClientParticleStrc *__stdcall PARTICLE_StopUnitMotion(D2UnitStrc *pUnit);
D2ClientParticleStrc *__stdcall PARTICLE_SetUnitParticleFlag(D2UnitStrc *pUnit);
BOOL __stdcall PARTICLE_IsUnitMotionActive(D2UnitStrc *pUnit);
D2ClientParticleStrc *__fastcall PARTICLE_SetUnitRisesToTarget(D2UnitStrc *pUnit, int32_t bRising);

//  ---- SoundChan.cpp (Sound\SoundChan.cpp): a wave buffer prepared, the EAX listener set, an instance started on a channel, the code-integrity
//  ---- probe, a channel detached and stopped, every channel stopped, a channel's mix sent (0x6FB52330..0x6FB52CEA)
int32_t __fastcall SOUNDCHAN_PrepareWaveBuffer(D2SoundsTxt *ptHeader);
void __fastcall SOUNDCHAN_ApplyEaxEnvironment(int32_t nEnvironmentId);
D2SoundChannelStrc *__fastcall SOUNDCHAN_StartInstance(D2SoundInstanceStrc *pSoundInstance);
void __cdecl SOUNDCHAN_SendCodeIntegrityProbe();
void __fastcall SOUNDCHAN_DetachInstance(D2SoundChannelStrc *pChannel);
void __fastcall SOUNDCHAN_StopAndDetach(D2SoundChannelStrc *pChannel);
void __cdecl SOUNDCHAN_StopAllAndDetach();
void __fastcall SOUNDCHAN_ApplyVolumeAndPosition(D2SoundChannelStrc *ptChannel);
//  ---- SoundChan.cpp: the channels up, the finished ones let go, a stream position, the channels created and shut down (0x6FB52CF0..0x6FB5320A)
int32_t __cdecl SOUNDCHAN_IsSystemReady();
int32_t __cdecl SOUNDCHAN_IsSystemReady_2();
void __cdecl SOUNDCHAN_ReapFinished();
uint32_t __fastcall SOUNDCHAN_GetStreamPosition(D2SoundChannelStrc *pChannel);
void __cdecl SOUNDCHAN_CreateSystem();
void __cdecl SOUNDCHAN_ShutdownSystem_2();
BOOL __fastcall SOUNDCHAN_CreateChannels(int32_t nKindMode);
void __cdecl SOUNDCHAN_ShutdownSystem();
//  ---- SoundChan.cpp: the 3D and EAX getters, the two toggles, the oldest channel playing an id, a channel's buffer playing
//  ---- (0x6FB53210..0x6FB537F1)
int32_t __cdecl SOUNDCHAN_Get3DEnabled();
int32_t __cdecl SOUNDCHAN_GetEaxEnabled();
BOOL __cdecl SOUNDCHAN_Is3DEnabled();
BOOL __cdecl SOUNDCHAN_IsEaxEnabled();
int32_t __cdecl SOUNDCHAN_Toggle3dSound();
int32_t __cdecl SOUNDCHAN_ToggleEaxEffects();
D2SoundChannelStrc *__fastcall SOUNDCHAN_FindOldestPlaying(D2C_Sounds nSoundId);
int32_t __fastcall SOUNDCHAN_IsPlaybackObjectActive(D2SoundChannelStrc *pChannel);

//  ---- SoundEnv.cpp: the ambience, weather, event and footstep state reset, and brought up to date once a tick (0x6FB53800..0x6FB53BFF)
void __cdecl SOUNDENV_ResetEnvironmentState();
void __fastcall SOUNDENV_UpdateEnvironment(int32_t nTick);

//  ---- SoundHdr.cpp (Sound\SoundHdr.cpp): the sounds.txt and soundenviron.txt accessors, the player's level and its sound environment
//  ---- (0x6FB53C00..0x6FB53D85)
D2SoundsTxt *__fastcall SOUNDHDR_GetRecord(D2C_Sounds nSoundId);
D2C_Sounds __fastcall SOUNDHDR_GetGroupBaseIndex(D2C_Sounds nSoundId);
uint8_t __fastcall SOUNDHDR_GetGroupSize(D2C_Sounds nSoundId);
int32_t __fastcall SOUNDHDR_IsLooping(D2C_Sounds nSoundId);
int32_t __cdecl SOUNDHDR_GetPlayerLevelId();
int32_t __cdecl SOUNDHDR_GetCurrentSoundEnvironId();
D2SoundEnvironTxt *__fastcall SOUNDHDR_GetEnvRecord(int32_t nSoundEnvId);
int32_t __cdecl SOUNDHDR_GetEnvironLookupMin();
int32_t __cdecl SOUNDHDR_GetEnvironLookupMax();
int32_t *__cdecl SOUNDHDR_GetEnvironLookup();
//  ---- SoundHdr.cpp: the sound system brought up and shut down, sounds.txt and soundenviron.txt loaded (0x6FB53D90..0x6FB549B3)
void __cdecl SOUNDHDR_Init();
void __cdecl SOUNDHDR_LoadSoundsTxt();
void __cdecl SOUNDHDR_Shutdown();
//  ---- SoundHdr.cpp: a group's counts adjusted and cleared, the async loads serviced, the pump, every sound stopped, a sound's data freed
//  ---- (0x6FB549C0..0x6FB54D81)
void __fastcall SOUNDHDR_AdjustRefCount(D2C_Sounds nSoundId, int32_t nDelta);
void __fastcall SOUNDHDR_ClearRefCount(D2C_Sounds nSoundId);
void __fastcall SOUNDHDR_ServiceAsyncLoads(uint32_t dwTickCount);
uint32_t __cdecl SOUNDHDR_GetPreloadCursor();
void __cdecl SOUNDHDR_PumpPreload();
void __cdecl SOUNDHDR_StopAllPending();
void __fastcall SOUNDHDR_FreeSoundData(D2C_Sounds nSoundId);
//  ---- SoundHdr.cpp: a sound's wave data loaded, the two falloff distances, a group's pick, the cache budget, a sound's path, a cache entry
//  ---- touched (0x6FB54D90..0x6FB55518)
void __fastcall SOUNDHDR_LoadWaveData(D2C_Sounds nSoundId, int32_t bForceLoad, int32_t bServicePass);
double __fastcall SOUNDHDR_GetRangeParam(D2C_Sounds nSoundId);
double __fastcall SOUNDHDR_GetFalloffDistance(D2C_Sounds nSoundId);
D2C_Sounds __fastcall SOUNDHDR_PickRandomInGroup(D2C_Sounds nSoundId);
void __fastcall SOUNDHDR_SetCacheBudget(uint32_t nBytes);
void __fastcall SOUNDHDR_BuildSoundPath(D2C_Sounds nSoundId, char *pszPath);
void __fastcall SOUNDHDR_TouchCacheEntry(D2C_Sounds nSoundId);

//  ---- SoundInst.cpp (Sound\SoundInst.cpp): an instance's state, its unit let go, its release, its handle by id, its position, a sound
//  ---- started, a unit's sound position, the base volume (0x6FB55520..0x6FB55BE2)
int32_t __fastcall SOUNDINST_IsInstanceActive(int32_t hInstance);
int32_t __fastcall SOUNDINST_IsInstanceStillPlaying(int32_t hInstance);
void __fastcall SOUNDINST_DetachFromUnit(int32_t hInstance, D2UnitStrc *pUnit, int32_t bForce);
void __fastcall SOUNDINST_MarkStarted(int32_t hInstance);
int32_t __fastcall SOUNDINST_GetHandleForSoundId(D2C_Sounds nSoundId);
void __fastcall SOUNDINST_SetPosition(int32_t hInstance, int32_t x, int32_t y, int32_t z);
int32_t __fastcall SOUNDINST_PlaySound(D2C_Sounds nSoundId, D2UnitStrc *pUnit, int32_t nDelayFrames, int32_t nPlayFlags, int32_t nStartBlock);
void __fastcall SOUNDINST_GetUnitSoundPosition(D2UnitStrc *pUnit, D2C_Sounds nSoundId, float *pOut);
int32_t __fastcall SOUNDINST_GetInstanceVolume(int32_t hInstance);
void __fastcall SOUNDINST_SetVolumeByHandle(int32_t hInstance, int32_t nVolume);

//  ---- SoundInst.cpp: a volume set or faded, whether two units hold an instance, the six releases of a set of instances, the speech and music
//  ---- queries, a handle's sound id (0x6FB55BF0..0x6FB560B1)
void __fastcall SOUNDINST_UpdateForUnit(int32_t hInstance, int32_t nVolume, uint32_t dwFadeBeginDelay, uint32_t nFadeTicks);
BOOL __fastcall SOUNDINST_HasActiveChannel(int32_t hInstance);
void __fastcall SOUNDINST_StopSoundsById(D2C_Sounds nSoundId);
void __cdecl SOUNDINST_MarkAllEnvironStarted();
void __fastcall SOUNDINST_StopSceneSounds(int32_t nKeepGroupA, int32_t nKeepGroupB);
void __fastcall SOUNDINST_StopEventSounds(int32_t nKeepGroupA, int32_t nKeepGroupB);
void __cdecl SOUNDINST_StopSpeechSounds();
void __cdecl SOUNDINST_StopAllInstances();
int32_t __cdecl SOUNDINST_IsAnySpeechActive();
int32_t __cdecl SOUNDINST_IsMusicPlaying();
D2C_Sounds __fastcall SOUNDINST_GetSoundIdFromHandle(int32_t hInstance);

//  ---- SoundInst.cpp: the sound heartbeat (0x6FB560C0..0x6FB56A9D)
void __fastcall SOUNDINST_ServiceInstances(uint32_t nTick);

//  ---- SoundInst.cpp: playback brought up, every instance freed, a handle's play position, two instances ranked, a unit within earshot, a
//  ---- speech line on a unit, the chaos debug (0x6FB56AA0..0x6FB56D3D)
void __cdecl SOUNDINST_ResetPlayback();
void __stdcall SOUNDINST_FreeAllInstances();
int32_t __fastcall SOUNDINST_GetPlayPosition(int32_t hInstance);
int32_t __fastcall SOUNDINST_ComparePriority(D2SoundInstanceStrc *pA, D2SoundInstanceStrc *pB, int32_t bUseDistance, int32_t bUseAge);
int32_t __fastcall SOUNDINST_IsUnitInEarshot(D2UnitStrc *pUnit);
BOOL __fastcall SOUNDINST_IsSpeechPlayingForUnit(D2UnitStrc *pUnit);
void __cdecl SOUNDINST_ToggleChaosDebug();

//  ---- SoundSong.cpp (src\Sound\SoundSong.cpp): the music, the stings, the player-seed rolls, the two distances, a distance on a ring
//  ---- (0x6FB56D40..0x6FB57325)
void __stdcall SOUNDSONG_Init();
void __fastcall SOUNDSONG_UpdateMusic(uint32_t nTick);
int32_t __fastcall SOUNDSONG_ScheduleSting(D2C_Sounds nSound1, int32_t nDelay1, int32_t nDuration, int32_t nSongStateIndex, D2C_Sounds nSound2, int32_t nDelay2, int32_t bPlaySound2);
void __fastcall SOUNDSONG_ScheduleSting2(int32_t nDelayFrames, int32_t nPending);
uint32_t __fastcall SOUNDSONG_RollRandom(int32_t nRange);
int32_t __fastcall SOUNDSONG_RollRandomInRange(int32_t nMin, int32_t nMax);
int32_t __fastcall SOUNDSONG_RandomOffset(int32_t nRange);
double __stdcall SOUNDSONG_Distance(float x, float y);
double __stdcall SOUNDSONG_DistanceSquared(float x, float y);
int32_t __fastcall SOUNDSONG_GetCircularDistance(int32_t nA, int32_t nB, int32_t nModulus);

//  ---- UnitSnd.cpp (the image's UNIT\UnitSnd.cpp): the level's sound state, a hit class's sounds, a unit's sound list, the floor under a unit
//  ---- (0x6FB4F4A0..0x6FB4FCD4), and the two helpers (0x6FB50850..0x6FB5090E)
void __cdecl UNITSND_ResetLevelSoundState();
D2C_Sounds __fastcall UNITSND_GetHitClassSound(D2UnitStrc *pUnit, int32_t nHitClass);
uint32_t __fastcall UNITSND_GetHitClassSoundDelay(D2UnitStrc *pUnit, int32_t nHitClass);
void __fastcall UNITSND_GetImpactSoundsForHitClass(D2UnitStrc *pUnit, int32_t nHitClass, int32_t nMode, D2C_Sounds *pnWeaponSound, D2C_Sounds *pnLayerSound);
void __fastcall UNITSND_LinkSoundToUnit(D2UnitStrc *pUnit, int32_t hInstance);
void __fastcall UNITSND_UnlinkSoundFromUnit(D2UnitStrc *pUnit, int32_t hInstance);
int32_t __fastcall UNITSND_FindPlayingSoundInGroup(D2UnitStrc *pUnit, D2C_Sounds nSoundId);
int32_t __fastcall UNITSND_GetFirstSoundOnUnit(D2UnitStrc *pUnit);
void __fastcall UNITSND_StopAllSoundsOnUnit(D2UnitStrc *pUnit, int32_t bForce);
int32_t __fastcall UNITSND_GetSurfaceMaterialUnderUnit(D2UnitStrc *pUnit);
int32_t __fastcall UNITSND_GetSoundClassId(D2UnitStrc *pUnit);
int32_t __fastcall UNITSND_GetSoundUnitType(D2UnitStrc *pUnit);

//  ---- UnitSnd.cpp: a unit's footstep sound, its monsounds.txt record, a footstep, a monster's skill sounds stopped (0x6FB4FCE0..0x6FB50365),
//  ---- and a unit's sound list's address (0x6FB50910..0x6FB50935)
D2C_Sounds __fastcall UNITSND_GetFootstepSoundId(D2UnitStrc *pUnit, int32_t bRunning);
D2MonSoundsTxt *__fastcall UNITSND_GetMonSoundsRecordForUnit(D2UnitStrc *pUnit);
void __fastcall UNITSND_PlayFootstep(D2UnitStrc *pUnit);
void __fastcall UNITSND_StopMonsterSkillSounds(D2UnitStrc *pUnit);
D2SoundListNodeStrc **__fastcall UNITSND_GetSoundListHead(D2UnitStrc *pUnit);

//  ---- UnitSnd.cpp: a unit's mode sound, and its five sound members' accessors (0x6FB50370..0x6FB5084D, 0x6FB50940..0x6FB50A28)
void __fastcall UNITSND_UpdateUnitModeSound(D2UnitStrc *pUnit);
void __fastcall UNITSND_SetSfxStarted(D2UnitStrc *pUnit, uint8_t bStarted);
void __fastcall UNITSND_SetSfxMode(D2UnitStrc *pUnit, int32_t nMode);
uint32_t __fastcall UNITSND_GetSfxTicks(D2UnitStrc *pUnit);
uint32_t __fastcall UNITSND_GetSfxAsyncTicks(D2UnitStrc *pUnit);
void __fastcall UNITSND_SetSfxAsyncTicks(D2UnitStrc *pUnit, uint32_t nFrame);

//  ---- UnitSnd.cpp: a unit's attack sounds, the sound for a mode it enters, a monster's voices stopped, a hit or death, a monster's skill
//  ---- sound (0x6FB50A30..0x6FB514D0)
void __fastcall UNITSND_PlayAttackSound(D2UnitStrc *pUnit, int32_t nMode, int32_t bDelayed);
void __fastcall UNITSND_PlayModeSound(D2UnitStrc *pUnit, int32_t nMode, int32_t bModeGiven);
void __fastcall UNITSND_StopMonsterVoices(D2UnitStrc *pUnit);
void __fastcall UNITSND_PlayHitOrDeathSound(D2UnitStrc *pUnit, int32_t nMode);
void __fastcall UNITSND_PlayMonsterSkillSound(D2UnitStrc *pUnit, int32_t nMode);

//  ---- UnitSnd.cpp: a player's sound event, a unit's sound event (server packet 0x2C), a unit's sounds counted in or out, the player's line on
//  ---- entering an area, a monster's greeting (0x6FB514E0..0x6FB522E7)
void __fastcall UNITSND_PlayPlayerSoundEvent(D2UnitStrc *pPlayer, uint16_t nSoundEvent);
void __fastcall UNITSND_PlayUnitSoundEvent(uint32_t dwUnitGUID, uint8_t nUnitType, uint16_t nSoundEvent);
void __fastcall UNITSND_AdjustUnitSoundRefCounts(D2UnitStrc *pUnit, int32_t nDelta);
void __cdecl UNITSND_PlayAreaEntryQuestLine(void);
void __fastcall UNITSND_PlayMonsterInitSound(D2UnitStrc *pUnit);

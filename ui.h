//  ui.h -- the globals of ui.cpp: the input tables of the panels and the six key-binding records, the label and toggle-rule tables, and the
//  scalars, image handles and text buffers of the UI core. Address range 0x6FB6FC18..0x6FBBA974. Each is a C++17 inline variable, one object
//  for every file that includes this header. They stand in the original's order, by address, except that a table a binding record points at
//  comes before the record.
//  An input table is an array of D2WinCommandRegisterStrc, 12 bytes each: { dwType, nMessage, pfnHandler }. The tables in .rdata are const,
//  and so are the six binding records. Six tables lie in .data and are not static data: UI_SetPanelHandlerKey ends in
//  `pBind->pHandlers[nEntry].nMessageOrKey = nKey`, so the key-binding pass writes the player's key into them when their panel opens, and what
//  is compiled in is only the default. Each binding record points at these tables.
//  ui.cpp's four registrars hand Storm these tables. SEvtUnregisterHandler removes an entry only when the handler address matches, so a table
//  registered by one copy of the registrars and unregistered by another leaves a handler stuck.
//  gMiniskillMessageHandlers, gHiriconsMessageHandlers and gConfigWheelHandler are one record each, not an array of one.
//  The globals past .data's raw data (it ends at 0x6FB9B000) are zero at load, and ours are zero-initialised. The text buffers
//  (gwszTransientText, gwszDrawTextScratch, gwszUnitLabelLine2, gwszUnitLabelLine1) are constructed through Unicode(0), which writes the zeros
//  the original's static initialisers write.
//  Include it after D2Client.h, which declares every handler named here.
#pragma once

//  D2Client 0x6FB6FC18
//  Input table of the UI_GAME panel: 4 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline const D2WinCommandRegisterStrc gGameMessageHandlers[4] =
{
    { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &PANEL_GameOnLButtonDown },   // [0]
    { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &PANEL_GameOnLButtonUp },   // [1]
    { D2_WINPROC_MESSAGE, WM_MOUSEMOVE, &PANEL_GameOnMouseMove },   // [2]
    { D2_WINPROC_MESSAGE, WM_RBUTTONDOWN, &PANEL_GameOnRButtonDown },   // [3]
};
static_assert(sizeof(gGameMessageHandlers) == 48, "gGameMessageHandlers: the item's 48 bytes in the .i64");

//  D2Client 0x6FB6FC48
//  Input table of the UI_INVENTORY panel: 4 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline const D2WinCommandRegisterStrc gInvMessageHandlers[4] =
{
    { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &INV_OnLButtonDown },   // [0]
    { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &INV_OnLButtonUp },   // [1]
    { D2_WINPROC_MESSAGE, WM_RBUTTONDOWN, &INV_OnRButtonDown },   // [2]
    { D2_WINPROC_MESSAGE, WM_MOUSEMOVE, &INV_OnMouseMove },   // [3]
};
static_assert(sizeof(gInvMessageHandlers) == 48, "gInvMessageHandlers: the item's 48 bytes in the .i64");

//  D2Client 0x6FB8EA30
//  Input table of the UI_NPCSHOP panel: 8 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline D2WinCommandRegisterStrc gNpcshopMessageHandlers[8] =
{
    { D2_WINPROC_MESSAGE, WM_RBUTTONDOWN, &INV_ItemPanelOnRButtonDown },   // [0]
    { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &INV_NpcShopOnLButtonDown },   // [1]
    { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &INV_NpcShopOnLButtonUp },   // [2]
    { D2_WINPROC_MESSAGE, WM_MOUSEMOVE, &INV_NpcShopOnMouseMove },   // [3]
    { D2_WINPROC_KEYDOWN, VK_ESCAPE, &INV_NpcShopOnDismissKey },   // [4]
    { D2_WINPROC_KEYDOWN, VK_SPACE, &INV_NpcShopOnDismissKey },   // [5]
    { D2_WINPROC_KEYDOWN, VK_RETURN, &INV_OnChatKey },   // [6]
    { D2_WINPROC_KEYDOWN, 'W', &INV_OnKeySwapWeapons },   // [7]
};
static_assert(sizeof(gNpcshopMessageHandlers) == 96, "gNpcshopMessageHandlers: the item's 96 bytes in the .i64");

//  D2Client 0x6FB6FC78
//  Rebindable keys for the npc shop panel: Clear Screen in both slots (entries 4 and 5 of gNpcshopMessageHandlers) and Swap Weapons secondary
//  (entry 7).
inline const D2PanelKeyBindStrc gNpcshopPanelKeys =
{
    .nClearScreenPrimary = 4,
    .nShowBeltPrimary = -1,
    .nClearScreenSecondary = 5,
    .nShowBeltSecondary = -1,
    .nClearMessagesPrimary = -1,
    .nClearMessagesSecondary = -1,
    .nSwapWeaponsPrimary = 7,
    .nSwapWeaponsSecondary = -1,
    .pHandlers = gNpcshopMessageHandlers,
};
static_assert(sizeof(gNpcshopPanelKeys) == 36, "gNpcshopPanelKeys: the item's 36 bytes in the .i64");

//  D2Client 0x6FB6FC9C
//  Input table of the UI_SKILLTREE panel: 2 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline const D2WinCommandRegisterStrc gSkilltreeMessageHandlers[2] =
{ { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &SKILLSTREE_OnLButtonDown }, { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &SKILLSTREE_OnLButtonUp } };
static_assert(sizeof(gSkilltreeMessageHandlers) == 24, "gSkilltreeMessageHandlers: the item's 24 bytes in the .i64");

//  D2Client 0x6FB6FCB4
//  Input table of the UI_MINISKILL panel: one record, registered by UI_RegisterPanelHandlers when the panel opens.
inline const D2WinCommandRegisterStrc gMiniskillMessageHandlers = { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &SPELLSEL_MiniSkillOnLButtonDown };
static_assert(sizeof(gMiniskillMessageHandlers) == 12, "gMiniskillMessageHandlers: the item's 12 bytes in the .i64");

//  D2Client 0x6FB6FCC0
//  Input table of the UI_STATSCREEN panel: 2 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline const D2WinCommandRegisterStrc gStatsMessageHandlers[2] =
{ { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &CHAR_StatsOnLButtonDown }, { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &CHAR_StatsOnLButtonUp } };
static_assert(sizeof(gStatsMessageHandlers) == 24, "gStatsMessageHandlers: the item's 24 bytes in the .i64");

//  D2Client 0x6FB6FCD8
//  Input table of the UI_NEWSTATS panel: 2 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline const D2WinCommandRegisterStrc gNewstatsMessageHandlers[2] =
{ { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &NEWSTATS_OnLButtonDown }, { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &NEWSTATS_OnLButtonUp } };
static_assert(sizeof(gNewstatsMessageHandlers) == 24, "gNewstatsMessageHandlers: the item's 24 bytes in the .i64");

//  D2Client 0x6FB6FCF0
//  Input table of the UI_HIRICONS panel: one record, registered by UI_RegisterPanelHandlers when the panel opens.
inline const D2WinCommandRegisterStrc gHiriconsMessageHandlers = { D2_WINPROC_KEYDOWN, VK_F11, &HIREABLES_HirIconsOnF11 };
static_assert(sizeof(gHiriconsMessageHandlers) == 12, "gHiriconsMessageHandlers: the item's 12 bytes in the .i64");

//  D2Client 0x6FB6FCFC
//  Input table of the UI_NEWSKILLS panel: 2 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline const D2WinCommandRegisterStrc gNewskillsMessageHandlers[2] =
{ { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &NEWSTATS_NewSkillsOnLButtonDown }, { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &NEWSTATS_NewSkillsOnLButtonUp } };
static_assert(sizeof(gNewskillsMessageHandlers) == 24, "gNewskillsMessageHandlers: the item's 24 bytes in the .i64");

//  D2Client 0x6FB6FD18
//  The UI_ESCMENU panel's input table, 7 records, registered by UI_RegisterPanelHandlers with a literal count of 7 and dropped again by
//  UI_UnregisterPanelHandlers. The table ends at 0x6FB6FD6C; the four bytes to 0x6FB6FD70 are alignment padding.
inline const D2WinCommandRegisterStrc gEscmenuMessageHandlers[7] =
{
    { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &ESCMENU_OnLButtonDown },   // [0]
    { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &ESCMENU_OnLButtonUp },   // [1]
    { D2_WINPROC_KEYDOWN, VK_DOWN, &ESCMENU_OnDownKey },   // [2]
    { D2_WINPROC_KEYDOWN, VK_UP, &ESCMENU_OnUpKey },   // [3]
    { D2_WINPROC_KEYDOWN, VK_LEFT, &ESCMENU_OnLeftKey },   // [4]
    { D2_WINPROC_KEYDOWN, VK_RIGHT, &ESCMENU_OnRightKey },   // [5]
    { D2_WINPROC_KEYDOWN, VK_RETURN, &ESCMENU_OnEnter },   // [6]
};
static_assert(sizeof(gEscmenuMessageHandlers) == 84, "gEscmenuMessageHandlers: the item's 84 bytes in the .i64");

//  D2Client 0x6FB6FD70
//  The Configure Controls panel's input table: 13 records, 9 functions. Registered by UI_RegisterPanelHandlers case UI_CONFIG and by
//  UI_ConfigRegisterPanelHandlers after a capture. gConfigWheelHandler at 0x6FB6FE0C is a separate one-record wheel table.
inline const D2WinCommandRegisterStrc gConfigMessageHandlers[13] =
{
    { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &CONFIG_OnMouseDown },   // [0]
    { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &CONFIG_OnMouseUp },   // [1]
    { D2_WINPROC_MESSAGE, WM_RBUTTONDOWN, &CONFIG_OnMouseDown },   // [2]
    { D2_WINPROC_MESSAGE, WM_RBUTTONUP, &CONFIG_OnMouseUp },   // [3]
    { D2_WINPROC_KEYDOWN, VK_DOWN, &CONFIG_OnArrowDown },   // [4]
    { D2_WINPROC_KEYDOWN, VK_UP, &CONFIG_OnArrowUp },   // [5]
    { D2_WINPROC_KEYDOWN, VK_LEFT, &CONFIG_OnArrowLeft },   // [6]
    { D2_WINPROC_KEYDOWN, VK_RIGHT, &CONFIG_OnArrowRight },   // [7]
    { D2_WINPROC_KEYDOWN, VK_RETURN, &CONFIG_OnEnterBeginCapture },   // [8]
    { D2_WINPROC_KEYDOWN, VK_DELETE, &CONFIG_OnEraseKey },   // [9]
    { D2_WINPROC_KEYDOWN, VK_BACK, &CONFIG_OnEraseKey },   // [10]
    { D2_WINPROC_KEYDOWN, VK_ESCAPE, &CONFIG_OnDismissKey },   // [11]
    { D2_WINPROC_KEYDOWN, VK_SPACE, &CONFIG_OnDismissKey },   // [12]
};
static_assert(sizeof(gConfigMessageHandlers) == 156, "gConfigMessageHandlers: the item's 156 bytes in the .i64");

//  D2Client 0x6FB6FE0C
//  Input table of the UI_CONFIG_WHEEL panel: one record, registered by UI_RegisterPanelHandlers when the panel opens.
inline const D2WinCommandRegisterStrc gConfigWheelHandler = { D2_WINPROC_MESSAGE, WM_MOUSEWHEEL, &CONFIG_OnMouseWheel };
static_assert(sizeof(gConfigWheelHandler) == 12, "gConfigWheelHandler: the item's 12 bytes in the .i64");

//  D2Client 0x6FB6FE18
//  Input table of the UI_AUTOMAP panel: 5 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline const D2WinCommandRegisterStrc gAutomapMessageHandlers[5] =
{
    { D2_WINPROC_KEYDOWN, VK_UP, &AUTOMAP_OnArrowUp },   // [0]
    { D2_WINPROC_KEYDOWN, VK_DOWN, &AUTOMAP_OnArrowDown },   // [1]
    { D2_WINPROC_KEYDOWN, VK_RIGHT, &AUTOMAP_OnArrowRight },   // [2]
    { D2_WINPROC_KEYDOWN, VK_LEFT, &AUTOMAP_OnArrowLeft },   // [3]
    { D2_WINPROC_KEYDOWN, VK_END, &HELP_Nullsub5 },   // [4]
};
static_assert(sizeof(gAutomapMessageHandlers) == 60, "gAutomapMessageHandlers: the item's 60 bytes in the .i64");

//  D2Client 0x6FB6FE54
//  Input table of the UI_QUESTLOG panel: 4 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline const D2WinCommandRegisterStrc gQuestlogMessageHandlers[4] =
{
    { D2_WINPROC_MESSAGE, WM_RBUTTONDOWN, &QUESTLOG_OnMouseDown },   // [0]
    { D2_WINPROC_MESSAGE, WM_RBUTTONUP, &QUESTLOG_OnMouseUp },   // [1]
    { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &QUESTLOG_OnMouseDown },   // [2]
    { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &QUESTLOG_OnMouseUp },   // [3]
};
static_assert(sizeof(gQuestlogMessageHandlers) == 48, "gQuestlogMessageHandlers: the item's 48 bytes in the .i64");

//  D2Client 0x6FB8EA90
//  Input table of the UI_QUESTSCREEN panel: 7 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline D2WinCommandRegisterStrc gQuestscreenMessageHandlers[7] =
{
    { D2_WINPROC_MESSAGE, WM_RBUTTONDOWN, &QUESTLOG_QuestScreenOnMouseDown },   // [0]
    { D2_WINPROC_MESSAGE, WM_RBUTTONUP, &QUESTLOG_QuestScreenOnMouseUp },   // [1]
    { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &QUESTLOG_QuestScreenOnMouseDown },   // [2]
    { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &QUESTLOG_QuestScreenOnMouseUp },   // [3]
    { D2_WINPROC_KEYDOWN, VK_ESCAPE, &QUESTLOG_QuestScreenOnDismissKey },   // [4]
    { D2_WINPROC_KEYDOWN, VK_ESCAPE, &QUESTLOG_QuestScreenOnDismissKey },   // [5]
    { D2_WINPROC_KEYDOWN, VK_ESCAPE, &QUESTLOG_QuestScreenOnDismissKey },   // [6]
};
static_assert(sizeof(gQuestscreenMessageHandlers) == 84, "gQuestscreenMessageHandlers: the item's 84 bytes in the .i64");

//  D2Client 0x6FB6FE84
//  Rebindable keys for the quest screen: Clear Screen only, entries 5 and 6 of gQuestscreenMessageHandlers, both static-defaulted to Escape.
inline const D2PanelKeyBindStrc gQuestscreenPanelKeys =
{
    .nClearScreenPrimary = 5,
    .nShowBeltPrimary = -1,
    .nClearScreenSecondary = 6,
    .nShowBeltSecondary = -1,
    .nClearMessagesPrimary = -1,
    .nClearMessagesSecondary = -1,
    .nSwapWeaponsPrimary = -1,
    .nSwapWeaponsSecondary = -1,
    .pHandlers = gQuestscreenMessageHandlers,
};
static_assert(sizeof(gQuestscreenPanelKeys) == 36, "gQuestscreenPanelKeys: the item's 36 bytes in the .i64");

//  D2Client 0x6FB6FEA8
//  Input table of the UI_INISCROLL panel: 2 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline const D2WinCommandRegisterStrc gIniscrollMessageHandlers[2] =
{ { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &TEXT_IniScrollOnMouseDown }, { D2_WINPROC_MESSAGE, WM_RBUTTONDOWN, &TEXT_IniScrollOnMouseDown } };
static_assert(sizeof(gIniscrollMessageHandlers) == 24, "gIniscrollMessageHandlers: the item's 24 bytes in the .i64");

//  D2Client 0x6FB6FEC0
//  Input table of the UI_WAYPOINT panel: 5 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline const D2WinCommandRegisterStrc gWaypointMessageHandlers[5] =
{
    { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &WAYPOINT_OnMouseDown },   // [0]
    { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &WAYPOINT_OnMouseUp },   // [1]
    { D2_WINPROC_MESSAGE, WM_RBUTTONDOWN, &WAYPOINT_OnMouseDown },   // [2]
    { D2_WINPROC_MESSAGE, WM_RBUTTONUP, &WAYPOINT_OnMouseUp },   // [3]
    { D2_WINPROC_MESSAGE, WM_CHAR, &WAYPOINT_OnChar },   // [4]
};
static_assert(sizeof(gWaypointMessageHandlers) == 60, "gWaypointMessageHandlers: the item's 60 bytes in the .i64");

//  D2Client 0x6FB6FEFC
//  Input table of the UI_MINIPANEL panel: 2 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline const D2WinCommandRegisterStrc gMinipanelMessageHandlers[2] =
{ { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &MINIPANEL_OnLButtonDown }, { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &MINIPANEL_OnLButtonUp } };
static_assert(sizeof(gMinipanelMessageHandlers) == 24, "gMinipanelMessageHandlers: the item's 24 bytes in the .i64");

//  D2Client 0x6FB6FF14
//  Input table of the UI_PARTYSCREEN panel: 2 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline const D2WinCommandRegisterStrc gPartyMessageHandlers[2] =
{ { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &PARTY_OnLButtonDown }, { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &PARTY_OnLButtonUp } };
static_assert(sizeof(gPartyMessageHandlers) == 24, "gPartyMessageHandlers: the item's 24 bytes in the .i64");

//  D2Client 0x6FB8EAE8
//  Input table of the UI_MPTRADE panel: 13 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline D2WinCommandRegisterStrc gTradeMessageHandlers[13] =
{
    { D2_WINPROC_MESSAGE, WM_RBUTTONDOWN, &INV_ConsumeMessage },   // [0]
    { D2_WINPROC_MESSAGE, WM_RBUTTONUP, &INV_ConsumeMessage },   // [1]
    { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &INV_TradeOnLButtonDown },   // [2]
    { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &INV_TradeOnLButtonUp },   // [3]
    { D2_WINPROC_MESSAGE, WM_MOUSEMOVE, &INV_TradeOnMouseMove },   // [4]
    { D2_WINPROC_KEYDOWN, VK_ESCAPE, &INV_TradeOnDismissKey },   // [5]
    { D2_WINPROC_KEYDOWN, VK_SPACE, &INV_TradeOnDismissKey },   // [6]
    { D2_WINPROC_KEYDOWN, VK_OEM_3, &INV_TradeOnShowBeltKey },   // [7]
    { D2_WINPROC_KEYDOWN, VK_RETURN, &INV_OnChatKey },   // [8]
    { D2_WINPROC_KEYDOWN, VK_SPACE, &INV_TradeOnDismissKey },   // [9]
    { D2_WINPROC_KEYDOWN, VK_OEM_3, &INV_TradeOnShowBeltKey },   // [10]
    { D2_WINPROC_KEYDOWN, 'N', &UI_TradeOnClearMessagesKey },   // [11]
    { D2_WINPROC_KEYDOWN, 'W', &INV_OnKeySwapWeapons },   // [12]
};
static_assert(sizeof(gTradeMessageHandlers) == 156, "gTradeMessageHandlers: the item's 156 bytes in the .i64");

//  D2Client 0x6FB6FF2C
//  Rebindable keys for the multiplayer trade panel, the only one of the six that uses all four actions: Clear Screen (5, 6), Show Belt (7, 10),
//  Clear Messages (11) and Swap Weapons (12).
inline const D2PanelKeyBindStrc gTradePanelKeys =
{
    .nClearScreenPrimary = 5,
    .nShowBeltPrimary = 7,
    .nClearScreenSecondary = 6,
    .nShowBeltSecondary = 10,
    .nClearMessagesPrimary = 11,
    .nClearMessagesSecondary = -1,
    .nSwapWeaponsPrimary = 12,
    .nSwapWeaponsSecondary = -1,
    .pHandlers = gTradeMessageHandlers,
};
static_assert(sizeof(gTradePanelKeys) == 36, "gTradePanelKeys: the item's 36 bytes in the .i64");

//  D2Client 0x6FB8EB88
//  Input table of the UI_STASH panel: 8 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline D2WinCommandRegisterStrc gStashMessageHandlers[8] =
{
    { D2_WINPROC_MESSAGE, WM_RBUTTONDOWN, &INV_ItemPanelOnRButtonDown },   // [0]
    { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &INV_StashOnLButtonDown },   // [1]
    { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &INV_StashOnLButtonUp },   // [2]
    { D2_WINPROC_MESSAGE, WM_MOUSEMOVE, &INV_StashOnMouseMove },   // [3]
    { D2_WINPROC_KEYDOWN, VK_ESCAPE, &INV_StashOnDismissKey },   // [4]
    { D2_WINPROC_KEYDOWN, VK_SPACE, &INV_StashOnDismissKey },   // [5]
    { D2_WINPROC_KEYDOWN, VK_RETURN, &INV_OnChatKey },   // [6]
    { D2_WINPROC_KEYDOWN, 'W', &INV_OnKeySwapWeapons },   // [7]
};
static_assert(sizeof(gStashMessageHandlers) == 96, "gStashMessageHandlers: the item's 96 bytes in the .i64");

//  D2Client 0x6FB6FF50
//  Rebindable keys for the stash: Clear Screen (4, 5) and Swap Weapons secondary (7).
inline const D2PanelKeyBindStrc gStashPanelKeys =
{
    .nClearScreenPrimary = 4,
    .nShowBeltPrimary = -1,
    .nClearScreenSecondary = 5,
    .nShowBeltSecondary = -1,
    .nClearMessagesPrimary = -1,
    .nClearMessagesSecondary = -1,
    .nSwapWeaponsPrimary = 7,
    .nSwapWeaponsSecondary = -1,
    .pHandlers = gStashMessageHandlers,
};
static_assert(sizeof(gStashPanelKeys) == 36, "gStashPanelKeys: the item's 36 bytes in the .i64");

//  D2Client 0x6FB8EBE8
//  Input table of the UI_CUBE panel: 7 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline D2WinCommandRegisterStrc gCubeMessageHandlers[7] =
{
    { D2_WINPROC_MESSAGE, WM_RBUTTONDOWN, &INV_ItemPanelOnRButtonDown },   // [0]
    { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &INV_CubeOnLButtonDown },   // [1]
    { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &INV_CubeOnLButtonUp },   // [2]
    { D2_WINPROC_MESSAGE, WM_MOUSEMOVE, &INV_CubeOnMouseMove },   // [3]
    { D2_WINPROC_KEYDOWN, VK_ESCAPE, &INV_CubeOnDismissKey },   // [4]
    { D2_WINPROC_KEYDOWN, VK_SPACE, &INV_CubeOnDismissKey },   // [5]
    { D2_WINPROC_KEYDOWN, VK_RETURN, &INV_OnChatKey },   // [6]
};
static_assert(sizeof(gCubeMessageHandlers) == 84, "gCubeMessageHandlers: the item's 84 bytes in the .i64");

//  D2Client 0x6FB6FF74
//  Rebindable keys for the Horadric Cube panel: Clear Screen only (4, 5). Unlike the shop and stash it has no swap-weapons entry.
inline const D2PanelKeyBindStrc gCubePanelKeys =
{
    .nClearScreenPrimary = 4,
    .nShowBeltPrimary = -1,
    .nClearScreenSecondary = 5,
    .nShowBeltSecondary = -1,
    .nClearMessagesPrimary = -1,
    .nClearMessagesSecondary = -1,
    .nSwapWeaponsPrimary = -1,
    .nSwapWeaponsSecondary = -1,
    .pHandlers = gCubeMessageHandlers,
};
static_assert(sizeof(gCubePanelKeys) == 36, "gCubePanelKeys: the item's 36 bytes in the .i64");

//  D2Client 0x6FB8EC3C
//  Input table of the UI_BELTROWS panel: 2 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline D2WinCommandRegisterStrc gBeltMessageHandlers[2] =
{ { D2_WINPROC_KEYDOWN, VK_SPACE, &PANEL_BeltOnDismissKey }, { D2_WINPROC_KEYDOWN, VK_SPACE, &PANEL_BeltOnDismissKey } };
static_assert(sizeof(gBeltMessageHandlers) == 24, "gBeltMessageHandlers: the item's 24 bytes in the .i64");

//  D2Client 0x6FB6FF98
//  Rebindable keys for the belt rows: Clear Screen in both slots, entries 0 and 1 of gBeltMessageHandlers, both static-defaulted to Space.
inline const D2PanelKeyBindStrc gBeltPanelKeys =
{
    .nShowBeltPrimary = -1,
    .nClearScreenSecondary = 1,
    .nShowBeltSecondary = -1,
    .nClearMessagesPrimary = -1,
    .nClearMessagesSecondary = -1,
    .nSwapWeaponsPrimary = -1,
    .nSwapWeaponsSecondary = -1,
    .pHandlers = gBeltMessageHandlers,
};
static_assert(sizeof(gBeltPanelKeys) == 36, "gBeltPanelKeys: the item's 36 bytes in the .i64");

//  D2Client 0x6FB6FFBC
//  Input table of the UI_HELPSCREEN panel: 5 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline const D2WinCommandRegisterStrc gHelpscreenMessageHandlers[5] =
{
    { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &HELP_HelpScreenOnLButtonDown },   // [0]
    { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &HELP_HelpScreenOnLButtonUp },   // [1]
    { D2_WINPROC_KEYDOWN, VK_SPACE, &HELP_HelpScreenOnDismissKey },   // [2]
    { D2_WINPROC_KEYDOWN, VK_ESCAPE, &HELP_HelpScreenOnDismissKey },   // [3]
    { D2_WINPROC_MESSAGE, WM_CHAR, &HELP_Nullsub5 },   // [4]
};
static_assert(sizeof(gHelpscreenMessageHandlers) == 60, "gHelpscreenMessageHandlers: the item's 60 bytes in the .i64");

//  D2Client 0x6FB6FFF8
//  Input table of the UI_HELPBUTTON panel: 2 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline const D2WinCommandRegisterStrc gHelpbuttonMessageHandlers[2] =
{ { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &HELP_HelpButtonOnLButtonDown }, { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &HELP_HelpButtonOnLButtonUp } };
static_assert(sizeof(gHelpbuttonMessageHandlers) == 24, "gHelpbuttonMessageHandlers: the item's 24 bytes in the .i64");

//  D2Client 0x6FB70010
//  Input table of the UI_HIREICONS panel: 3 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline const D2WinCommandRegisterStrc gHireiconsMessageHandlers[3] =
{
    { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &HIREABLES_HireIconsOnLButtonDown },   // [0]
    { D2_WINPROC_MESSAGE, WM_MOUSEMOVE, &HIREABLES_HireIconsOnMouseMove },   // [1]
    { D2_WINPROC_MESSAGE, WM_RBUTTONDOWN, &HIREABLES_HireIconsOnRButtonDown },   // [2]
};
static_assert(sizeof(gHireiconsMessageHandlers) == 36, "gHireiconsMessageHandlers: the item's 36 bytes in the .i64");

//  D2Client 0x6FB70034
//  Input table of the UI_MERCINV panel: 3 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline const D2WinCommandRegisterStrc gMercinvMessageHandlers[3] =
{
    { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &INV_MercInvOnLButtonDown },   // [0]
    { D2_WINPROC_MESSAGE, WM_LBUTTONUP, &INV_MercInvOnMouseUp },   // [1]
    { D2_WINPROC_MESSAGE, WM_MOUSEMOVE, &INV_MercInvOnMouseMove },   // [2]
};
static_assert(sizeof(gMercinvMessageHandlers) == 36, "gMercinvMessageHandlers: the item's 36 bytes in the .i64");

//  D2Client 0x6FB70058
//  Input table of the UI_RECIPESCROLL panel: 2 records, registered by UI_RegisterPanelHandlers when the panel opens.
inline const D2WinCommandRegisterStrc gRecipescrollMessageHandlers[2] =
{ { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, &NEWSTATS_RecipeScrollOnLButtonDown }, { D2_WINPROC_MESSAGE, WM_MOUSEMOVE, &HELP_Nullsub5 } };
static_assert(sizeof(gRecipescrollMessageHandlers) == 24, "gRecipescrollMessageHandlers: the item's 24 bytes in the .i64");

//  D2Client 0x6FB70070
//  6 records of 10 bytes, `{ int nStatId; uint16_t nTblIndex; int nColor; }`, driving the immunity line of a monster's hover label. Walked by
//  UIPANELS_BuildMonsterImmunityLabel, which prints a record when the unit's nStatId is >= 100.
//      36 damageresist -> 10085 'Immune to Physical'  colour 4
//      37 magicresist  -> 10086 'Immune to Magic'     colour 8
//      39 fireresist   -> 10081 'Immune to Fire'      colour 1
//      41 lightresist  -> 10083 'Immune to Lightning' colour 9
//      43 coldresist   -> 10082 'Immune to Cold'      colour 3
//      45 poisonresist -> 10084 'Immune to Poison'    colour 2
//  Six strides of 10 from 0x6FB70070 end at 0x6FB700AC, this table's end. The walk starts at +4 (`mov edi, 6FB70074h`, 0x6FB215BE) and so ends
//  on gbUiWindowCloseable (`cmp edi, 6FB700B0h`, 0x6FB21644), a bound four bytes past the table, which ui_panels.cpp computes from this table
//  with an integer cursor.
inline const uint8_t gMonImmunityLabels[60] =
{
    36,   // [0]
    0,   // [1]
    0,   // [2]
    0,   // [3]
    101,   // [4]
    39,   // [5]
    4,   // [6]
    0,   // [7]
    0,   // [8]
    0,   // [9]
    37,   // [10]
    0,   // [11]
    0,   // [12]
    0,   // [13]
    102,   // [14]
    39,   // [15]
    8,   // [16]
    0,   // [17]
    0,   // [18]
    0,   // [19]
    39,   // [20]
    0,   // [21]
    0,   // [22]
    0,   // [23]
    97,   // [24]
    39,   // [25]
    1,   // [26]
    0,   // [27]
    0,   // [28]
    0,   // [29]
    41,   // [30]
    0,   // [31]
    0,   // [32]
    0,   // [33]
    99,   // [34]
    39,   // [35]
    9,   // [36]
    0,   // [37]
    0,   // [38]
    0,   // [39]
    43,   // [40]
    0,   // [41]
    0,   // [42]
    0,   // [43]
    98,   // [44]
    39,   // [45]
    3,   // [46]
    0,   // [47]
    0,   // [48]
    0,   // [49]
    45,   // [50]
    0,   // [51]
    0,   // [52]
    0,   // [53]
    100,   // [54]
    39,   // [55]
    2,   // [56]
};
static_assert(sizeof(gMonImmunityLabels) == 60, "gMonImmunityLabels: the item's 60 bytes in the .i64");

//  D2Client 0x6FB700B0
//  Parallel array to gUiWindowStates, 38 entries: 1 means UI_CloseAllPanels may close that window. The automap (id 10) is deliberately not
//  marked, which is why it needs its own argument to be closed.
inline const int32_t gbUiWindowCloseable[38] =
{ 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1 };
static_assert(sizeof(gbUiWindowCloseable) == 152, "gbUiWindowCloseable: the item's 152 bytes in the .i64");

//  D2Client 0x6FB8EA28
//  Screen X the unit name label is centred on, set before UIPANELS_DrawUnitNameLabel runs and used by every draw inside it. UIPANELS_DrawInGamePanels
//  writes it, and UIPANELS_DrawUnitNameLabel writes it itself too (0x6FB20AE8, 0x6FB20B04).
inline int32_t gnLabelScreenX = -1;
static_assert(sizeof(gnLabelScreenX) == 4, "gnLabelScreenX: the item's 4 bytes in the .i64");

//  D2Client 0x6FB8EA2C
//  Screen Y for the unit name label. The monster line is drawn at this minus 72; object and item lines add NameOffsetFromObject instead.
inline int32_t gnLabelScreenY = -1;
static_assert(sizeof(gnLabelScreenY) == 4, "gnLabelScreenY: the item's 4 bytes in the .i64");

//  D2Client 0x6FB8EC54
//  The quest-specific level-entry title table: 2 records of 13 bytes, packed, which is the stride UIPANELS_DrawInGamePanels walks it with:
//      +0x00  int  nLevelId
//      +0x04  int  nQuestId
//      +0x08  char szTitle[5]    the art stem, e.g. "A1Q1"
//      [0]  level 8  = LEVEL_DENOFEVIL       quest 1  "A1Q1"
//      [1]  level 20 = LEVEL_FORGOTTENTOWER  quest 5  "A1Q5"
//  The path is built with ACT1 hardcoded, so this can only fire in act 1. The walk reads +4 and +8 by address (`mov edi, 6FB8EC58h` 0x6FB21E77,
//  `lea eax, [esi+edx*4+6FB8EC5Ch]` 0x6FB21EA4), never the head.
inline D2ClientLevelEntryTitleStrc gLevelEntryQuestTitles[2] =
{
    { .nLevelId = LEVEL_DENOFEVIL, .nQuestId = QUEST_A1Q1_DENOFEVIL, .szTitle = "A1Q1" },   // [0]
    { .nLevelId = LEVEL_FORGOTTENTOWER, .nQuestId = QUEST_A1Q5_COUNTESS, .szTitle = "A1Q5" },   // [1]
};
static_assert(sizeof(gLevelEntryQuestTitles) == 26, "gLevelEntryQuestTitles: the item's 26 bytes in the .i64");

//  D2Client 0x6FB8EC70
//  The record count of gLevelEntryQuestTitles: 2. It is the loop bound in UIPANELS_DrawInGamePanels, which walks records by 13 bytes and gives up
//  when the index reaches this. A count rather than a terminator, so the table cannot be extended by appending to it alone.
inline int32_t gnLevelEntryQuestTitleCount = 2;
static_assert(sizeof(gnLevelEntryQuestTitleCount) == 4, "gnLevelEntryQuestTitleCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FB8EC74
//  The right inset of the charge-up icon strip: UIPANELS_DrawChargeUpIcons computes `nX = gnScreenWidth - this`.
inline int32_t gnChargeUpIconRightInset = 148;
static_assert(sizeof(gnChargeUpIconRightInset) == 4, "gnChargeUpIconRightInset: the item's 4 bytes in the .i64");

//  D2Client 0x6FB8EC78
//  The bottom inset of the charge-up icon strip: `nY = gnScreenHeight - this` in UIPANELS_DrawChargeUpIcons. It equals
//  gnChargeUpIconBottomInset800 next door because stock data gives both the same number.
inline int32_t gnChargeUpIconBottomInset = 64;
static_assert(sizeof(gnChargeUpIconBottomInset) == 4, "gnChargeUpIconBottomInset: the item's 4 bytes in the .i64");

//  D2Client 0x6FB8EC7C
//  The same bottom inset as gnChargeUpIconBottomInset, used instead of it when D2GFX_GetResolutionMode() == D2GAMERES_800x600. Both hold 64 in
//  stock data, so the branch changes nothing today.
inline int32_t gnChargeUpIconBottomInset800 = 64;
static_assert(sizeof(gnChargeUpIconBottomInset800) == 4, "gnChargeUpIconBottomInset800: the item's 4 bytes in the .i64");

//  D2Client 0x6FB8EC80
//  The rows gaUiToggleRules points at: 38 rows of 38 ints from 0x6FB8EC80 to 0x6FB90310, where gaUiToggleRules begins. Every slot of
//  gaUiToggleRules but three points into it on a 152-byte boundary (slot [9] is row 18), none at rows 19, 20 and 21. No instruction names it:
//  UI_SetUIToggle reads it only through the table.
inline int32_t gnUiToggleRuleRows[38][38] =
{
    { 0, 2, 0, 1, 1, 0, 0, 0, 1, 1, 0, 3, 3, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1 },   // [0]
    { 0, 0, 2, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1 },   // [1]
    { 0, 1, 1, 2, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1 },   // [2]
    { 0, 1, 0, 1, 2, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1 },   // [3]
    { 0, 1, 2, 2, 2, 2, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 2, 0, 1, 0, 1, 1, 1, 1, 1, 1, 2, 1 },   // [4]
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 },   // [5]
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1 },   // [6]
    { 0, 2, 2, 2, 2, 0, 0, 0, 3, 2, 0, 2, 0, 2, 3, 2, 2, 0, 3, 0, 3, 3, 3, 3, 2, 3, 3, 3, 3, 3, 3, 0, 2, 2, 2, 0, 2, 2 },   // [7]
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1 },   // [8]
    { 0, 2, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 3, 2, 3, 3, 3, 0, 1, 0, 2, 3, 3, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 0, 2, 1 },   // [9]
    { 0, 0, 2, 2, 2, 0, 0, 0, 0, 2, 0, 0, 3, 0, 0, 2, 0, 0, 3, 0, 2, 0, 2, 3, 2, 3, 3, 3, 3, 3, 3, 0, 2, 2, 2, 0, 2, 1 },   // [10]
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },   // [11]
    { 0, 2, 2, 2, 2, 2, 0, 0, 4, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2, 2, 2, 3, 2, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 0, 2 },   // [12]
    { 0, 0, 1, 1, 0, 1, 0, 0, 4, 1, 0, 1, 1, 0, 1, 3, 1, 2, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1 },   // [13]
    { 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 3, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1 },   // [14]
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 3, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1 },   // [15]
    { 0, 3, 3, 3, 3, 3, 0, 0, 4, 3, 3, 3, 3, 0, 3, 2, 0, 0, 3, 0, 2, 2, 2, 3, 2, 1, 1, 1, 1, 1, 1, 3, 2, 2, 2, 0, 3, 1 },   // [16]
    { 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 3, 0, 3, 1, 1, 0, 1, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1 },   // [17]
    { 0, 2, 2, 2, 2, 2, 0, 0, 1, 2, 2, 2, 2, 2, 1, 2, 2, 0, 1, 0, 1, 0, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 0, 2, 1 },   // [18]
    { 0, 1, 1, 1, 1, 2, 0, 0, 1, 1, 1, 1, 3, 0, 3, 1, 1, 0, 1, 0, 1, 1, 2, 3, 2, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 0, 1, 1 },   // [19]
    { 0, 1, 1, 1, 1, 2, 0, 0, 1, 1, 1, 1, 3, 0, 3, 1, 1, 0, 1, 0, 1, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1 },   // [20]
    { 0, 1, 1, 1, 1, 2, 0, 0, 1, 1, 1, 1, 3, 0, 3, 1, 1, 0, 1, 0, 1, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1 },   // [21]
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },   // [22]
    { 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 3, 0, 3, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1 },   // [23]
    { 0, 2, 2, 2, 2, 0, 0, 0, 2, 2, 2, 2, 3, 0, 3, 2, 2, 0, 2, 0, 2, 0, 2, 3, 2, 3, 3, 3, 3, 3, 3, 0, 3, 2, 2, 0, 2, 2 },   // [24]
    { 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1 },   // [25]
    { 0, 2, 2, 2, 2, 0, 0, 0, 3, 2, 2, 2, 3, 0, 3, 2, 1, 0, 1, 0, 2, 0, 2, 3, 2, 3, 3, 3, 3, 3, 3, 0, 3, 2, 2, 0, 2, 1 },   // [26]
    { 0, 2, 2, 2, 2, 0, 0, 0, 3, 1, 2, 2, 3, 0, 3, 2, 1, 0, 1, 0, 2, 0, 2, 3, 2, 3, 3, 3, 3, 3, 3, 0, 1, 2, 2, 0, 2, 1 },   // [27]
    { 0, 2, 2, 2, 2, 2, 0, 0, 3, 1, 2, 2, 3, 0, 3, 2, 2, 0, 1, 0, 2, 0, 2, 3, 2, 3, 3, 3, 3, 3, 3, 1, 1, 2, 2, 0, 2, 1 },   // [28]
    { 0, 2, 2, 2, 2, 2, 0, 0, 3, 1, 2, 2, 3, 0, 3, 2, 2, 0, 1, 0, 2, 0, 2, 3, 2, 3, 3, 3, 3, 3, 3, 0, 1, 2, 2, 0, 2, 1 },   // [29]
    { 0, 2, 2, 2, 2, 2, 0, 0, 3, 1, 2, 2, 3, 0, 3, 2, 2, 0, 1, 0, 2, 0, 2, 3, 2, 3, 3, 3, 3, 3, 3, 0, 1, 2, 2, 0, 2, 1 },   // [30]
    { 0, 2, 2, 2, 2, 2, 0, 0, 3, 1, 2, 2, 3, 0, 3, 2, 2, 0, 1, 0, 2, 0, 2, 3, 2, 3, 3, 3, 3, 3, 3, 1, 1, 2, 2, 0, 2, 1 },   // [31]
    { 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 2, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 3, 3, 1 },   // [32]
    { 0, 2, 2, 2, 2, 2, 0, 0, 3, 1, 2, 2, 3, 0, 3, 2, 2, 0, 1, 0, 2, 0, 2, 3, 2, 3, 3, 3, 3, 3, 3, 1, 1, 0, 0, 0, 2, 1 },   // [33]
    { 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 1, 2, 1, 2, 2, 2, 2, 2, 1, 0, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 2, 1 },   // [34]
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2 },   // [35]
    { 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 2, 1 },   // [36]
    { 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 2 },   // [37]
};
static_assert(sizeof(gnUiToggleRuleRows) == 5776, "gnUiToggleRuleRows: the item's 5776 bytes in the .i64");

//  D2Client 0x6FBBA7A0
//  Three rows of 38 ints, past .data's raw data and so all zero: gaUiToggleRules' slots [0], [19] and [35] point at them. A zero rule is
//  "nothing", so those three panels never close or refuse another. Declared before gaUiToggleRules, out of address order, because that table's
//  initializer names it.
inline int32_t gnUiToggleRuleRowsZero[3][38];
static_assert(sizeof(gnUiToggleRuleRowsZero) == 456, "gnUiToggleRuleRowsZero: the item's 456 bytes in the .i64");

//  D2Client 0x6FB90310
//  A 38 x 38 panel-interaction matrix, gaUiToggleRules[nOpen][nRequested]: the row is a panel already open and the column the one being asked
//  for. The cell is an action for UI_SetUIToggle, the only reader:
//      0 nothing   1 close the open one   2 refuse   3 error and trace   4 end the NPC conversation first
//  Rows 0, 19 and 35 (UI_GAME, UI_HIRICONS, UI_HIREICONS) point past .data's raw size and are all 0 at run time; nothing conflicts with them.
//  The other 35 rows hold real data in 0x6FB8EC80..0x6FB90278. Length 38; sgNumStateTbl begins at 0x6FB903A8, one past this table's end.
inline int32_t *gaUiToggleRules[38] =
{
    gnUiToggleRuleRowsZero[0],   // [0]
    gnUiToggleRuleRows[0],   // [1]
    gnUiToggleRuleRows[1],   // [2]
    gnUiToggleRuleRows[2],   // [3]
    gnUiToggleRuleRows[3],   // [4]
    gnUiToggleRuleRows[4],   // [5]
    gnUiToggleRuleRows[5],   // [6]
    gnUiToggleRuleRows[6],   // [7]
    gnUiToggleRuleRows[7],   // [8]
    gnUiToggleRuleRows[18],   // [9]
    gnUiToggleRuleRows[8],   // [10]
    gnUiToggleRuleRows[9],   // [11]
    gnUiToggleRuleRows[10],   // [12]
    gnUiToggleRuleRows[11],   // [13]
    gnUiToggleRuleRows[12],   // [14]
    gnUiToggleRuleRows[13],   // [15]
    gnUiToggleRuleRows[14],   // [16]
    gnUiToggleRuleRows[15],   // [17]
    gnUiToggleRuleRows[16],   // [18]
    gnUiToggleRuleRowsZero[1],   // [19]
    gnUiToggleRuleRows[17],   // [20]
    gnUiToggleRuleRows[22],   // [21]
    gnUiToggleRuleRows[23],   // [22]
    gnUiToggleRuleRows[24],   // [23]
    gnUiToggleRuleRows[25],   // [24]
    gnUiToggleRuleRows[26],   // [25]
    gnUiToggleRuleRows[27],   // [26]
    gnUiToggleRuleRows[28],   // [27]
    gnUiToggleRuleRows[29],   // [28]
    gnUiToggleRuleRows[30],   // [29]
    gnUiToggleRuleRows[31],   // [30]
    gnUiToggleRuleRows[32],   // [31]
    gnUiToggleRuleRows[33],   // [32]
    gnUiToggleRuleRows[34],   // [33]
    gnUiToggleRuleRows[35],   // [34]
    gnUiToggleRuleRowsZero[2],   // [35]
    gnUiToggleRuleRows[36],   // [36]
    gnUiToggleRuleRows[37],   // [37]
};
static_assert(sizeof(gaUiToggleRules) == 152, "gaUiToggleRules: the item's 152 bytes in the .i64");

//  D2Client 0x6FB903A8
//  The UI state table's element count: 38, read-only. UI_SetUIToggle asserts `sgNumStateTbl == NUM_UI` on every call (`cmp sgNumStateTbl, 26h`
//  at 0x6FB232E9, the assert and exit on the failure arm); nothing writes it. It is read only there, at 0x6FB232E9, 0x6FB233E2 and 0x6FB23433.
inline int32_t sgNumStateTbl = 38;
static_assert(sizeof(sgNumStateTbl) == 4, "sgNumStateTbl: the item's 4 bytes in the .i64");

//  D2Client 0x6FB903AC
//  Gates the 100 ms panel-toggle debounce. Set by UI_EnablePanelToggleDelay, cleared by UI_ClearPanelToggleDelay, and read by
//  UI_ArmPanelToggleDelay and by six arms of UI_UnregisterPanelHandlers' per-panel jump table.
inline BOOL gbPanelToggleDelayEnabled = 1;
static_assert(sizeof(gbPanelToggleDelayEnabled) == 4, "gbPanelToggleDelayEnabled: the item's 4 bytes in the .i64");

//  D2Client 0x6FB904DC
//  The string `EXPANSION`, referenced once, by UIPANELS_DrawInGamePanels, which draws it on the in-game panels.
inline char gszExpansion[10] = "EXPANSION";
static_assert(sizeof(gszExpansion) == 10, "gszExpansion: the item's 10 bytes in the .i64");

//  D2Client 0x6FBB9FC0
//  A copy of gbGameExitPending, taken at the end of UI_LoadScreenBegin and re-taken by UI_DrawLoadingScreen, which then tests it and skips the
//  whole draw when it is set: the loading screen stops drawing itself once the server has told the client to leave, rather than sitting over a
//  game that is already unwinding. gbGameExitPending is what SCMD_HandleSrv05 raises, alongside the three-minute disconnect deadline.
inline int32_t gbLoadScreenExitPending;
static_assert(sizeof(gbLoadScreenExitPending) == 4, "gbLoadScreenExitPending: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9FC4
//  The gold-entry dialog object: built by UI_OpenGoldEntryDialog, which hands it to DIALOG_AddLine (0x6FB23D24) and to DIALOG_AddControl four
//  times (0x6FB23DB1, 0x6FB23E17, 0x6FB23E83, 0x6FB23EEB); drawn by UIPANELS_DrawInGamePanels; and nulled by UI_CloseGoldEntryDialog, which is
//  what makes UI_IsGoldDialogOpen a simple null test. UI_InitUiSubsystems also writes it once, at startup, from a register.
inline D2ClientDialogStrc *gpGoldDialog;
static_assert(sizeof(gpGoldDialog) == 4, "gpGoldDialog: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9FC8
//  The gold dialog's step button, built by SPELLSEL_UiSpinnerProxyConstruct (0x6FB23D90) with UI_GoldDialogStepAmount as its callback
//  (`push 6FB23F70h`, 0x6FB23D7D). UI_GoldDialogStepAmount reads the pressed direction from vtable slot +44 and the step size from +48; not
//  null-checked there, unlike the spinner beside it.
inline int32_t gpGoldDialogStepButton;
static_assert(sizeof(gpGoldDialogStepButton) == 4, "gpGoldDialogStepButton: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9FCC
//  The spinner proxy holding the amount, built by EDITBOX_UiCtrlProxyConstructEditbox (0x6FB23DF6). UI_GoldDialogStepAmount reads its value
//  through vtable +44 and writes the new one back through +56, both behind a null check, so the dialog still works after a failed control
//  allocation, it just stops tracking the value.
inline int32_t gpGoldDialogSpinner;
static_assert(sizeof(gpGoldDialogSpinner) == 4, "gpGoldDialogSpinner: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9FD0
//  The gold dialog's OK button proxy: `CHAR_UiButtonProxyConstruct(this, 250, 287, 0, UI_GoldDialogOnOk)`, added to gpGoldDialog and then sent
//  vtable slot 48 with 1 (the enable call). Written by UI_InitUiSubsystems, UI_OpenGoldEntryDialog and UI_CloseGoldEntryDialog, read by the
//  latter two. Destroyed by UI_CloseGoldEntryDialog through vtable slot 0 with argument 1 (a vector deleting destructor) and nulled, which is
//  what makes closing twice harmless. The dialog builds four controls: the step button (0x6FB23D90), the spinner (0x6FB23DF6), OK (0x6FB23E62)
//  and Cancel (0x6FB23ECA); DIALOG_AddLine (0x6FB23D24) adds a line.
inline int32_t gpGoldDialogOkButton;
static_assert(sizeof(gpGoldDialogOkButton) == 4, "gpGoldDialogOkButton: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9FD4
//  The gold dialog's Cancel button proxy: `CHAR_UiButtonProxyConstruct(this, 355, 287, 1, UI_GoldDialogOnCancelButton)`, the same Y as OK and
//  105 pixels to its right. Written by UI_InitUiSubsystems, UI_CloseGoldEntryDialog and UI_OpenGoldEntryDialog, read by
//  UI_CloseGoldEntryDialog, which destroys and nulls it like the rest.
inline int32_t gpGoldDialogCancelButton;
static_assert(sizeof(gpGoldDialogCancelButton) == 4, "gpGoldDialogCancelButton: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9FD8
//  Balances the dialog's suspension of the command-table key hook, so the number keys type digits instead of firing hotkeys:
//      open:   if ( !gbGoldDialogKeyHookSuspended )
//              { CMDTBL_SetKeyHookMode(0, 1); gbGoldDialogKeyHookSuspended = 1; }
//      close:  if (  gbGoldDialogKeyHookSuspended )
//              { CMDTBL_SetKeyHookMode(1, 1); gbGoldDialogKeyHookSuspended = 0; }
//  A guarded pair, not a plain flag: the guard is what makes UI_OpenGoldEntryDialog safe to call over an open dialog, which its own early-out
//  path does. UI_InitUiSubsystems clears it at startup.
inline BOOL gbGoldDialogKeyHookSuspended;
static_assert(sizeof(gbGoldDialogKeyHookSuspended) == 4, "gbGoldDialogKeyHookSuspended: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB9FE0
//  A shared Unicode scratch buffer for text about to be drawn: 256 elements, constructed through Unicode(0); gnUnitLabelColor begins 0x200 bytes
//  on. UIPANELS_DrawInGamePanels, UIPANELS_DrawUnitNameLabel and UIPANELS_SetUnitLabelText each begin by storing 0 into its first character
//  (UIPANELS_SetUnitLabelText only when its text is NULL, otherwise it copies into it) and then pass its address as the destination of a string
//  build.
inline Unicode gwszDrawTextScratch[256];
static_assert(sizeof(gwszDrawTextScratch) == 512, "gwszDrawTextScratch: the item's 512 bytes in the .i64");

//  D2Client 0x6FBBA1E0
//  Colour for the floating unit-name label, written only by UIPANELS_SetUnitLabelText and read only by UIPANELS_DrawInGamePanels. A null pText
//  terminates the scratch buffer but leaves this colour alone, so the value can outlive the text it was set for; safe only because nothing
//  reads it while the text is empty.
inline int32_t gnUnitLabelColor;
static_assert(sizeof(gnUnitLabelColor) == 4, "gnUnitLabelColor: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA1E4
//  `DATA\GLOBAL\ui\menu\boxpieces`, the tiled frame's corner and edge art. Loaded once by UI_InitUiSubsystems (immediately after gpImgFocus16,
//  from the same stack path buffer), used by UIPANELS_DrawTiledFrame, and freed and nulled by UI_ShutdownUiSubsystems.
inline D2CellFileStrc *gpImgUiBoxPieces;
static_assert(sizeof(gpImgUiBoxPieces) == 4, "gpImgUiBoxPieces: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA1E8
//  The level-name banner shown on entering a level, held only while it is on screen. UIPANELS_DrawInGamePanels loads, draws and unloads it:
//    * the player has no room -> use whatever is loaded and fall through to the expiry test;
//    * DUNGEON_GetLevelIdFromRoom differs from gLevelId -> unload it and null it, store the new level id, and set gLevelEntryFrameCount to 0x78
//      or 0x78+0x88 depending on whether the old level id was zero;
//    * gLevelEntryFrameCount has reached 0x78 (120 frames) -> unload and null.
//  UI_ResetUiSubsystems and UI_ShutdownUiSubsystems read it to free it and clear it for the two shutdown paths (0x6FB2398D / 0x6FB2399C,
//  0x6FB20391 / 0x6FB203A0).
inline D2CellFileStrc *gpImgLevelEntryTitle;
static_assert(sizeof(gpImgLevelEntryTitle) == 4, "gpImgLevelEntryTitle: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA1F0
//  Buffer for the one-shot floating message: 160 bytes (0xA0), 80 wide characters. Written by UIPANELS_SetTransientText, drawn and then memset
//  to zero by UIPANELS_DrawTransientText (`rep stosd` of 40 dwords), so it holds at most one message at a time.
inline Unicode gwszTransientText[80];
static_assert(sizeof(gwszTransientText) == 160, "gwszTransientText: the item's 160 bytes in the .i64");

//  D2Client 0x6FBBA290
//  Requested x for the floating message; UIPANELS_DrawTransientText centres the box on it and then clamps the box inside the screen.
inline int32_t gnTransientTextX;
static_assert(sizeof(gnTransientTextX) == 4, "gnTransientTextX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA294
//  Requested y for the floating message, clamped to at least one line height and no lower than gnScreenHeight - 31.
inline int32_t gnTransientTextY;
static_assert(sizeof(gnTransientTextY) == 4, "gnTransientTextY: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA298
//  Colour stored alongside the floating message by UIPANELS_SetTransientText.
inline int32_t gnTransientTextColor;
static_assert(sizeof(gnTransientTextColor) == 4, "gnTransientTextColor: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA2A0
//  The first line of the unit hover label, a 256-character Unicode buffer, filled by MONUNIQUE_MonUModBuildMonsterModifierLabel through
//  UIPANELS_ShouldDrawNameLabel with either `<name> Minion` or the enchantment list. gwszUnitLabelLine2 is the immunity line and sits at
//  exactly +0x200; UIPANELS_DrawInGamePanels draws this one first.
inline Unicode gwszUnitLabelLine1[256];
static_assert(sizeof(gwszUnitLabelLine1) == 512, "gwszUnitLabelLine1: the item's 512 bytes in the .i64");

//  D2Client 0x6FBBA4A0
//  The second line of the unit hover label, a 256-character Unicode buffer, written by UIPANELS_BuildMonsterImmunityLabel and
//  UIPANELS_DrawUnitNameLabel and drawn by UIPANELS_DrawInGamePanels through UIPANELS_DrawTextCenteredInPanelRect. gwszUnitLabelLine1 is
//  immediately before it and the same size (0x6FBBA2A0 + 0x200 = 0x6FBBA4A0). UIPANELS_DrawInGamePanels adds a line height before drawing this
//  one when gwszDrawTextScratch or gwszUnitLabelLine1 is not empty (`cmp ..., bx / jne` 0x6FB22155, `je` 0x6FB22165, D2WIN_10125 at 0x6FB22167).
inline Unicode gwszUnitLabelLine2[256];
static_assert(sizeof(gwszUnitLabelLine2) == 512, "gwszUnitLabelLine2: the item's 512 bytes in the .i64");

//  D2Client 0x6FBBA6A0
//  The colour UIPANELS_DrawInGamePanels passes to UIPANELS_DrawTextCenteredInPanelRect for line 2. UIPANELS_BuildMonsterImmunityLabel sets it
//  to 4; the per-immunity colours are embedded in the text by AddColorCodeToWstring instead. It sits at gwszUnitLabelLine2 + 0x200,
//  immediately after that buffer's 256 characters.
inline int32_t gnUnitLabelLine2Color;
static_assert(sizeof(gnUnitLabelLine2Color) == 4, "gnUnitLabelLine2Color: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA6A8
//  UI toggle states, int[38] indexed by D2C_UIvars. NUM_UI is 0x26; both UI_GetUiWindowState and UI_SetUIToggle assert and exit(-1) past it,
//  and base + 38*4 lands exactly on gnPanelToggleDelayUntil.
inline int32_t gUiWindowStates[38];
static_assert(sizeof(gUiWindowStates) == 152, "gUiWindowStates: the item's 152 bytes in the .i64");

//  D2Client 0x6FBBA740
//  The tick count until which panel toggling is suppressed: `GetTickCount() + 100` when armed. UI_SetUIToggle is the reader that makes it
//  matter; UI_UnregisterPanelHandlers and UI_ArmPanelToggleDelay write it.
inline uint32_t gnPanelToggleDelayUntil;
static_assert(sizeof(gnPanelToggleDelayUntil) == 4, "gnPanelToggleDelayUntil: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA744
//  What the gold dialog is for. Read by UI_GoldDialogOnOk to choose the packet and by UI_GoldDialogStepAmount to choose the balance to clamp
//  against:
//      0, 1   drop gold on the ground -- client command 80 on the player unit
//      2      offer gold in a trade
//      3      command 79 sub-id 20, clamped against STAT_GOLD
//      4      command 79 sub-id 19, clamped against STAT_GOLDBANK
//  3 and 4 are the stash pair, and the clamp is what tells them apart: mode 4 limits the amount by what is in the stash, so it is the
//  withdrawal; mode 3 limits it by what the player is carrying, so it is the deposit.
inline int32_t gnGoldDialogMode;
static_assert(sizeof(gnGoldDialogMode) == 4, "gnGoldDialogMode: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA748
inline int32_t gnBorderXDifference;
static_assert(sizeof(gnBorderXDifference) == 4, "gnBorderXDifference: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA74C
inline int32_t gnBorderYDifference;
static_assert(sizeof(gnBorderYDifference) == 4, "gnBorderYDifference: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA754
//  Write-only: UI_RegisterPanelHandlers sets it to 1 in its UI_AUTOMAP arm and UI_UnregisterPanelHandlers clears it in the matching one,
//  tracking whether the automap's five message handlers are registered, and nothing reads it (it is not exported either, so no other module
//  can). LATENT: a flag nothing reads cannot change behaviour. It lies past .data's raw size and starts at 0.
inline BOOL gbAutomapHandlersRegistered;
static_assert(sizeof(gbAutomapHandlersRegistered) == 4, "gbAutomapHandlersRegistered: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA758
//  Raised by UI_LoadScreenBegin and cleared by UI_UnloadLoadingScreen; read only through UI_IsLoadingScreenActive, whose only caller is
//  GFX_PreloadUnit. UI_LoadScreenBegin writes it twice, once from a register early and once as a literal 1 later, so the two paths through it
//  agree on the end state.
inline int32_t gbLoadingScreenActive;
static_assert(sizeof(gbLoadingScreenActive) == 4, "gbLoadingScreenActive: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA768
//  `DATA\GLOBAL\UI\CURSOR\Pentspin`, the spinning pentagram, loaded by UI_InitUiSubsystems and released by UI_ShutdownUiSubsystems. Reached
//  only through UI_GetPentSpinImage.
inline D2CellFileStrc *gpImgPentSpin;
static_assert(sizeof(gpImgPentSpin) == 4, "gpImgPentSpin: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA76C
//  The shop buy/sell button image (`DATA\GLOBAL\ui\panel\buysellbtn`): wsprintfA (0x6FB2412E), ARCHIVE_CellFileLoad (0x6FB2413D), the store
//  (0x6FB24142). UI_GetBuySellButtonImage both fills and reads it, a load-once cache, and UI_ShutdownUiSubsystems releases it.
inline D2CellFileStrc *gpImgBuySellButton;
static_assert(sizeof(gpImgBuySellButton) == 4, "gpImgBuySellButton: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA770
//  `DATA\GLOBAL\UI\CURSOR\focus16`, loaded and released alongside gpImgPentSpin and reached only through UI_GetFocusImage, whose one caller is
//  DIALOG_Draw (0x6FAD6FE7). What the cel is used for is not established.
inline D2CellFileStrc *gpImgFocus16;
static_assert(sizeof(gpImgFocus16) == 4, "gpImgFocus16: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA774
//  Write-only: UI_LoadScreenBegin (0x6FB24228), UI_DrawLoadingScreen (0x6FB2434A) and UI_UnloadLoadingScreen (0x6FB24630) write it, and nothing
//  in the module reads it. All three writes sit beside the loading screen's own state, so it is loading-screen bookkeeping whose reader was
//  removed or never existed. Not named: a name would assert a role the module does not use it for.
inline int32_t UNRESOLVED_dword_6FBBA774;
static_assert(sizeof(UNRESOLVED_dword_6FBBA774) == 4, "UNRESOLVED_dword_6FBBA774: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA778
//  Handle of the loading-screen image (loaded by UI_LoadScreenBegin from UI\LoadingScreen / UI\Loading\Loadingscreen), released by
//  UI_UnloadLoadingScreen. The assert at 0x6FB243FD reloads this address (6FB243F4 mov eax, <here>; cmp eax, ebx; jnz) and pushes the message
//  "sghLoadScreenProgress", the stringified condition, so that is the author's name for this handle: the loading screen's progress animation,
//  whose frame count D2CMP_CelFileGetCelsPerDirection reads and whose current frame is gnLoadScreenFrame.
inline D2CellFileStrc *sghLoadScreenProgress;
static_assert(sizeof(sghLoadScreenProgress) == 4, "sghLoadScreenProgress: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA77C
//  The act the loading screen is showing. Written by UI_SetLoadScreenAct as that function's first instruction, so it is a plain setter, and
//  read by UI_LoadScreenBegin and UI_UnloadLoadingScreen to pick the artwork.
inline int32_t gnLoadScreenAct;
static_assert(sizeof(gnLoadScreenAct) == 4, "gnLoadScreenAct: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA780
//  Set to 1 while the death panel is on screen and cleared the moment UIPANELS_GameShowDeathMessage finds the player alive; read by nothing
//  (both writes are in that function, and it is not exported). LATENT: a flag no code reads cannot change behaviour. It lies past .data's raw
//  size and starts at 0. Named for what the two writes say: one sits beside the panel being drawn, the other beside the not-dead early return.
inline BOOL gbDeathMessageShown;
static_assert(sizeof(gbDeathMessageShown) == 4, "gbDeathMessageShown: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA784
inline D2C_Levels gLevelId;
static_assert(sizeof(gLevelId) == 4, "gLevelId: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA788
inline int32_t gLevelEntryFrameCount;
static_assert(sizeof(gLevelEntryFrameCount) == 4, "gLevelEntryFrameCount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA78C
//  `DATA\LOCAL\UI\<lang>\youdiedhardcore`, the death banner for a hardcore character; the path is built from DATA\LOCAL, which makes it a
//  localised asset, the language directory being the second %s. UIPANELS_GameShowDeathMessage loads each of the two banners when its handle is
//  NULL and draws each (D2WIN_10134_DrawCellFile 0x6FB223B2 and 0x6FB22414); its hardcore test (WINMAIN_IsHardcoreGame, 0x6FB22434) comes after
//  both draws. Freed and nulled by UI_ResetUiSubsystems; the pair is loaded and released together.
inline D2CellFileStrc *gpImgYouDiedHardcore;
static_assert(sizeof(gpImgYouDiedHardcore) == 4, "gpImgYouDiedHardcore: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA790
//  `DATA\LOCAL\UI\<lang>\youdiedinst`, the ordinary death banner, the one that carries the respawn instructions, as against
//  gpImgYouDiedHardcore four bytes below. Same lifetime: loaded by UIPANELS_GameShowDeathMessage, unloaded and nulled by UI_ResetUiSubsystems,
//  which frees the two in consecutive instruction pairs. Named after the file; reading `inst` as `instructions` is inference.
inline D2CellFileStrc *gpImgYouDiedInst;
static_assert(sizeof(gpImgYouDiedInst) == 4, "gpImgYouDiedInst: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA794
//  Gates UIPANELS_DrawChargeUpIcons (`if (this)`). Nothing writes it and it lies past .data's raw size, so it is zero and the icon strip never
//  draws.
inline int32_t gbChargeUpIconsEnabled;
static_assert(sizeof(gbChargeUpIconsEnabled) == 4, "gbChargeUpIconsEnabled: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA798
//  The horizontal step between charge-up icons: UIPANELS_DrawChargeUpIcons does `nX -= this` after each icon drawn. It lies past .data's raw
//  size and is zero, and it is unreachable today because the gate gbChargeUpIconsEnabled is never set.
inline int32_t gnChargeUpIconStepX;
static_assert(sizeof(gnChargeUpIconStepX) == 4, "gnChargeUpIconStepX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA968
//  The amount currently entered in the gold-entry dialog, the one UI_OpenGoldEntryDialog opens for drop, trade, bank deposit and bank
//  withdraw. UI_OpenGoldEntryDialog zeroes it when the dialog opens; UI_CloseGoldEntryDialog stores the value read back out of the edit control
//  when it closes; UI_GoldDialogStepAmount reads and re-stores it; UI_GoldDialogOnOk is the only pure reader.
inline int32_t gnGoldDialogAmount;
static_assert(sizeof(gnGoldDialogAmount) == 4, "gnGoldDialogAmount: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA96C
//  The cel frame index of the loading screen's progress animation (sghLoadScreenProgress): seeded to 0, incremented once per call when
//  nProgress is non-zero, clamped to CelsPerDirection - 1, handed to TEXTURE_CelDraw as pData.field_08. The clamp is a signed compare
//  (`cmp ecx, eax` then `jl` at 0x6FB24532), so it is an int. The name is ours, not the original's.
inline int32_t gnLoadScreenFrame;
static_assert(sizeof(gnLoadScreenFrame) == 4, "gnLoadScreenFrame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBBA970
//  The palette index the open-game loading bar is filled with, built once by UI_DrawLoadingScreen as D2Win_10034_MixRGB(0x60, 0, 0), a dark
//  red. Its own value doubles as the one-shot guard: the pair is only built when this byte is still zero, so index 0 can never be the fill
//  colour and the build happens once per process. It is handed to GFX_DrawFilledRect for the progress rectangle; gnLoadBarFrameColour next door
//  is the outline.
inline uint8_t gnLoadBarFillColour;
static_assert(sizeof(gnLoadBarFillColour) == 1, "gnLoadBarFillColour: the item's 1 bytes in the .i64");

//  D2Client 0x6FBBA971
//  The palette index the open-game loading bar is outlined with: D2Win_10034_MixRGB(0xFF, 0xFF, 0), yellow. Built in the same one-shot block as
//  gnLoadBarFillColour and handed to D2GFX_DrawBox for the 260x20 frame; the dark-red fill is drawn inside it two pixels in.
inline uint8_t gnLoadBarFrameColour;
static_assert(sizeof(gnLoadBarFrameColour) == 1, "gnLoadBarFrameColour: the item's 1 bytes in the .i64");

//  D2Client 0x6FBBA974
//  The GetTickCount stamp UI_ThrottleTick50ms compares against and advances.
inline uint32_t gdwUiThrottleTick;
static_assert(sizeof(gdwUiThrottleTick) == 4, "gdwUiThrottleTick: the item's 4 bytes in the .i64");

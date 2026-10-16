//  CmdTbl.h -- the globals of CmdTbl.cpp that are ours: the key-command table, the default and the live key bindings, the hook tables and their
//  registration flags, the mouse-button and wheel handlers and the unknown-key-name buffer.
//  Each is a C++17 inline variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above a global
//  gives its address in the original. gDefaultCommandTable is written (CMDTBL_CommandLoadDefaultKeyBindings copies a default.key over it), so it
//  is not const. gwszUnknownKeyNameFallback is built by its declaration, through the Unicode constructor the original's static initialiser calls.
//  The original stops every walk over gKeyBindings by comparing a cursor with an address past the array; ours count 114 records.
//  Include it after D2Client.h, which declares every function named here.
#pragma once

//  The type of sgtKeyFunctions' two function columns. The commands are no-argument functions declared __cdecl in a void (__fastcall *)() slot
//  (and slot 42's release is D2Win's void __stdcall(void) screenshot export); with no argument nothing is placed in a register or popped,
//  so the conventions make the same call.
using CMDTBL_KeyFn = void (__fastcall *)();

//  D2Client 0x6FB78500
//  The stock key bindings: 114 records, two per command for commands 0..56, that seed gKeyBindings; the row whose bPrimaryHotkey is 1 is the primary.
inline HotkeyMappingEntry gDefaultCommandTable[114] =
{
    { .eCmd = HOTKEY_STATSCREEN, .VKCode = D2VK_A, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_STATSCREEN, .VKCode = D2VK_C, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_INVENTORY, .VKCode = D2VK_B, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_INVENTORY, .VKCode = D2VK_I, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_PARTY, .VKCode = D2VK_P, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_PARTY, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_MSGLOG, .VKCode = D2VK_M, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_MSGLOG, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_QUESTS, .VKCode = D2VK_Q, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_QUESTS, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_CHAT, .VKCode = D2VK_RETURN, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_CHAT, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_HELP, .VKCode = D2VK_H, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_HELP, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_AUTOMAP, .VKCode = D2VK_TAB, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_AUTOMAP, .VKCode = D2VK_MBUTTON, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_MINIMAP_POSITION, .VKCode = D2VK_F9, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_MINIMAP_POSITION, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_FADEAUTOMAP, .VKCode = D2VK_F10, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_FADEAUTOMAP, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_PARTYONMAP, .VKCode = D2VK_F11, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_PARTYONMAP, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_NAMESONMAP, .VKCode = D2VK_F12, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_NAMESONMAP, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SKILLTREE, .VKCode = D2VK_T, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SKILLTREE, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SKILLSELECT, .VKCode = D2VK_S, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SKILLSELECT, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SKILL1, .VKCode = D2VK_F1, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SKILL1, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SKILL2, .VKCode = D2VK_F2, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SKILL2, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SKILL3, .VKCode = D2VK_F3, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SKILL3, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SKILL4, .VKCode = D2VK_F4, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SKILL4, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SKILL5, .VKCode = D2VK_F5, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SKILL5, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SKILL6, .VKCode = D2VK_F6, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SKILL6, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SKILL7, .VKCode = D2VK_F7, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SKILL7, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SHOWBELT, .VKCode = D2VK_OEM_3, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SHOWBELT, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_POTION1, .VKCode = D2VK_1, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_POTION1, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_POTION2, .VKCode = D2VK_2, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_POTION2, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_POTION3, .VKCode = D2VK_3, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_POTION3, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_POTION4, .VKCode = D2VK_4, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_POTION4, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SPEECH1, .VKCode = D2VK_NUMPAD0, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SPEECH1, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SPEECH2, .VKCode = D2VK_NUMPAD1, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SPEECH2, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SPEECH3, .VKCode = D2VK_NUMPAD2, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SPEECH3, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SPEECH4, .VKCode = D2VK_NUMPAD3, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SPEECH4, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SPEECH5, .VKCode = D2VK_NUMPAD4, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SPEECH5, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SPEECH6, .VKCode = D2VK_NUMPAD5, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SPEECH6, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SPEECH7, .VKCode = D2VK_NUMPAD6, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SPEECH7, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_RUN, .VKCode = D2VK_CONTROL, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_RUN, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_RUNWALK, .VKCode = D2VK_R, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_RUNWALK, .VKCode = D2VK_MBUTTON5, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_STANDSTILL, .VKCode = D2VK_SHIFT, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_STANDSTILL, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SHOWITEMS, .VKCode = D2VK_MENU, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SHOWITEMS, .VKCode = D2VK_MBUTTON4, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_CLEARSCREEN, .VKCode = D2VK_SPACE, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_CLEARSCREEN, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_PREVSKILL, .VKCode = D2VK_WHEEL_UP, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_PREVSKILL, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_NEXTSKILL, .VKCode = D2VK_WHEEL_DOWN, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_NEXTSKILL, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_CLEARMESSAGES, .VKCode = D2VK_N, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_CLEARMESSAGES, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SCREENSHOT, .VKCode = D2VK_SNAPSHOT, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SCREENSHOT, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_PORTRAITS, .VKCode = D2VK_Z, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_PORTRAITS, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SWITCHWEAP, .VKCode = D2VK_W, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SWITCHWEAP, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_MINIMAP, .VKCode = D2VK_V, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_MINIMAP, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SKILL8, .VKCode = D2VK_F8, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SKILL8, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SKILL9, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SKILL9, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SKILL10, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SKILL10, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SKILL11, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SKILL11, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SKILL12, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SKILL12, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SKILL13, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SKILL13, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SKILL14, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SKILL14, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SKILL15, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SKILL15, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SKILL16, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SKILL16, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_MERCINV, .VKCode = D2VK_O, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_MERCINV, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_SPEECH8, .VKCode = D2VK_NUMPAD7, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_SPEECH8, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
    { .eCmd = HOTKEY_ESCMENU, .VKCode = D2VK_ESCAPE, .bPrimaryHotkey = 1 },
    { .eCmd = HOTKEY_ESCMENU, .VKCode = D2VK_INVALID, .bPrimaryHotkey = 0 },
};
static_assert(sizeof(gDefaultCommandTable) == 1140, "gDefaultCommandTable: the item's 1140 bytes in the .i64");

//  D2Client 0x6FB78978
//  Every bindable key command: 57 records of 12 bytes { FKeyDown, FKeyLift, bFlag } indexed by eCmd; FKeyLift is set for only four commands.
inline KeyFunctions sgtKeyFunctions[57] =
{
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionCharacterScreen, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionInventoryScreen, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionPartyScreen, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionMessageLog, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionQuestLog, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionChat, .FKeyLift = NULL, .bUnk0x8 = 1 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionHelpScreen, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionAutomap, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionCenterAutomap, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionFadeAutomap, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionPartyOnAutomap, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionNamesOnAutomap, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSkillTree, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSkillSelect, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSelectSkillSlot0, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSelectSkillSlot1, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSelectSkillSlot2, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSelectSkillSlot3, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSelectSkillSlot4, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSelectSkillSlot5, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSelectSkillSlot6, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSelectSkillSlot7, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionShowBelt, .FKeyLift = NULL, .bUnk0x8 = 1 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionUseBelt1, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionUseBelt2, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionUseBelt3, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionUseBelt4, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSayHelp, .FKeyLift = NULL, .bUnk0x8 = 1 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSayFollowMe, .FKeyLift = NULL, .bUnk0x8 = 1 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSayThisIsForYou, .FKeyLift = NULL, .bUnk0x8 = 1 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSayThanks, .FKeyLift = NULL, .bUnk0x8 = 1 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSaySorry, .FKeyLift = NULL, .bUnk0x8 = 1 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSayBye, .FKeyLift = NULL, .bUnk0x8 = 1 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSayNowYouDie, .FKeyLift = NULL, .bUnk0x8 = 1 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionRun, .FKeyLift = (CMDTBL_KeyFn)&CMDTBL_ActionRunReleased, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionToggleRunWalk, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionStandStill, .FKeyLift = (CMDTBL_KeyFn)&CMDTBL_ActionStandStillReleased, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionShowItems, .FKeyLift = (CMDTBL_KeyFn)&CMDTBL_ActionShowItemsReleased, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&j_GAME_CmdTblActionClearScreen, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSelectPreviousSkill, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSelectNextSkill, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&j_TEXT_ExpireAllChatMessages, .FKeyLift = NULL, .bUnk0x8 = 1 },
    { .FKeyDown = NULL, .FKeyLift = (CMDTBL_KeyFn)&D2WIN_10168_WINMAIN_CreateScreenshot, .bUnk0x8 = 1 },
    { .FKeyDown = (CMDTBL_KeyFn)&HIREABLES_CmdTblActionShowPortraits, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSwapWeapons, .FKeyLift = NULL, .bUnk0x8 = 1 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionToggleAutomapLeft, .FKeyLift = NULL, .bUnk0x8 = 1 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSelectSkillSlot8, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSelectSkillSlot9, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSelectSkillSlot10, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSelectSkillSlot11, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSelectSkillSlot12, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSelectSkillSlot13, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSelectSkillSlot14, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSelectSkillSlot15, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionMercenaryInventory, .FKeyLift = NULL, .bUnk0x8 = 0 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionSayRetreat, .FKeyLift = NULL, .bUnk0x8 = 1 },
    { .FKeyDown = (CMDTBL_KeyFn)&CMDTBL_ActionEscMenu, .FKeyLift = NULL, .bUnk0x8 = 0 },
};
static_assert(sizeof(sgtKeyFunctions) == 684, "sgtKeyFunctions: the item's 684 bytes in the .i64");

//  D2Client 0x6FB78C24
//  The full key-command hook, 4 records: WM_KEYDOWN and WM_SYSKEYDOWN to CMDTBL_OnKeyDown, WM_KEYUP and WM_SYSKEYUP to CMDTBL_OnKeyUp.
inline D2WinCommandRegisterStrc gKeyCommandHandlers[4] =
{
    { D2_WINPROC_MESSAGE,   WM_KEYDOWN,                 &CMDTBL_OnKeyDown },
    { D2_WINPROC_MESSAGE,   WM_SYSKEYDOWN,              &CMDTBL_OnKeyDown },
    { D2_WINPROC_MESSAGE,   WM_KEYUP,                   &CMDTBL_OnKeyUp },
    { D2_WINPROC_MESSAGE,   WM_SYSKEYUP,                &CMDTBL_OnKeyUp },
};
static_assert(sizeof(gKeyCommandHandlers) == 48, "gKeyCommandHandlers: the item's 48 bytes in the .i64");

//  D2Client 0x6FB78C54
//  The release-only key hook, 2 records: WM_KEYUP and WM_SYSKEYUP to CMDTBL_OnKeyUp, installed by CMDTBL_SetKeyHookMode in place of the full one.
inline D2WinCommandRegisterStrc gKeyCommandReleaseHandlers[2] =
{
    { D2_WINPROC_MESSAGE,   WM_KEYUP,                   &CMDTBL_OnKeyUp },
    { D2_WINPROC_MESSAGE,   WM_SYSKEYUP,                &CMDTBL_OnKeyUp },
};
static_assert(sizeof(gKeyCommandReleaseHandlers) == 24, "gKeyCommandReleaseHandlers: the item's 24 bytes in the .i64");

//  D2Client 0x6FBB2D24
//  The 1-character key name CMDTBL_ControlsGetBoundKeyNameFallback builds for a VK code its switch does not recognise.
inline Unicode gwszUnknownKeyNameFallback[4];
static_assert(sizeof(gwszUnknownKeyNameFallback) == 8, "gwszUnknownKeyNameFallback: the item's 8 bytes in the .i64");

//  D2Client 0x6FBB3630
//  Set by CMDTBL_SetKeyHookMode: 0 unhooked, 1 full hook, 2 restricted (only commands whose bFlag is set fire).
inline int32_t gnKeyHookMode = 0;
static_assert(sizeof(gnKeyHookMode) == 4, "gnKeyHookMode: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB3634
//  Set while gKeyCommandHandlers is registered, so a repeated CMDTBL_SetKeyHookMode does not register it twice.
inline BOOL gbKeyCommandHandlersRegistered = 0;
static_assert(sizeof(gbKeyCommandHandlersRegistered) == 4, "gbKeyCommandHandlersRegistered: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB3638
//  The same guard for gKeyCommandReleaseHandlers.
inline BOOL gbKeyCommandReleaseHandlersRegistered = 0;
static_assert(sizeof(gbKeyCommandReleaseHandlersRegistered) == 4, "gbKeyCommandReleaseHandlersRegistered: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB2D20
//  The handler of mouse button 5 on release, or null; the original's own name (the assert at CmdTbl.cpp:885 pushes it).
inline FARPROC sgpfnMouse5LiftFn;
static_assert(sizeof(sgpfnMouse5LiftFn) == 4, "sgpfnMouse5LiftFn: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB2D2C
//  The handler of mouse button 5 on press, or null; the original's own name (the assert at CmdTbl.cpp:873 pushes it).
inline FARPROC sgpfnMouse5DownFn;
static_assert(sizeof(sgpfnMouse5DownFn) == 4, "sgpfnMouse5DownFn: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB2D30
//  The wheel-down handler, or null; CMDTBL_CommandHandleMouseWheel reads it on a negative notch count (assert at CmdTbl.cpp:907).
inline FARPROC sgpfnMouseWheelDownFn;
static_assert(sizeof(sgpfnMouseWheelDownFn) == 4, "sgpfnMouseWheelDownFn: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB2D34
//  The handler of mouse button 4 on release, or null; the original's own name (the assert at CmdTbl.cpp:861 pushes it).
inline FARPROC sgpfnMouse4LiftFn;
static_assert(sizeof(sgpfnMouse4LiftFn) == 4, "sgpfnMouse4LiftFn: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB2D38
//  The live key bindings: 114 records, two per command for commands 0..56, seeded from gDefaultCommandTable and rewritten by Configure Controls.
inline HotkeyMappingEntry gKeyBindings[114];
static_assert(sizeof(gKeyBindings) == 1140, "gKeyBindings: the item's 1140 bytes in the .i64");

//  D2Client 0x6FBB31AC
//  The handler of mouse button 4 on press, or null; the original's own name (the assert at CmdTbl.cpp:848 pushes it).
inline FARPROC sgpfnMouse4DownFn;
static_assert(sizeof(sgpfnMouse4DownFn) == 4, "sgpfnMouse4DownFn: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB31B0
//  The controls screen's undo copy of gKeyBindings: written by CMDTBL_SaveBindingsSnapshot, copied back by CMDTBL_RestoreBindingsSnapshot on Cancel.
inline HotkeyMappingEntry sgSavedCmdMapping[114];
static_assert(sizeof(sgSavedCmdMapping) == 1140, "sgSavedCmdMapping: the item's 1140 bytes in the .i64");

//  D2Client 0x6FBB3624
//  What the middle mouse button does on press, or null; the original's own name (the assert in CMDTBL_CallMouse3Down, CmdTbl.cpp:825).
inline FARPROC sgpfnMouse3DownFn;
static_assert(sizeof(sgpfnMouse3DownFn) == 4, "sgpfnMouse3DownFn: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB3628
//  What the middle mouse button does on release, or null; the original's own name (the assert in CMDTBL_CallMouse3Lift, CmdTbl.cpp:837).
inline FARPROC sgpfnMouse3LiftFn;
static_assert(sizeof(sgpfnMouse3LiftFn) == 4, "sgpfnMouse3LiftFn: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB362C
//  The wheel-up handler, or null; CMDTBL_CommandHandleMouseWheel reads it on a positive notch count (assert at CmdTbl.cpp:899).
inline FARPROC sgpfnMouseWheelUpFn;
static_assert(sizeof(sgpfnMouseWheelUpFn) == 4, "sgpfnMouseWheelUpFn: the item's 4 bytes in the .i64");

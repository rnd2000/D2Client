#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "WINMAIN.h"
#include "CmdTbl.h"

//  CmdTbl.cpp -- the key-command table of D2Client.dll 1.10: the key-command actions, the mouse trampolines, the key-binding machinery
//  (load, save, bind, unbind, key names) and the two key dispatchers.
//  The original source file is Ui\CmdTbl.cpp (the asserts in this unit name it). Address range 0x6FAD3720..0x6FAD5CA0.
//
//  sgtKeyFunctions is 57 records of 12 bytes { FKeyDown; FKeyLift; bFlag }, indexed by the command id. Only four commands have a lift handler:
//  34 Run, 36 Stand Still, 37 Show Items and 42, the screenshot, which has no down handler. bFlag is set on fourteen commands, the ones that keep
//  working while the chat box has the keyboard: CMDTBL_SetKeyHookMode mode 2 runs a command only if its flag is set.
//  The handlers sit beside their siblings in source order, not in command order; the high ids are expansion additions.
//  No action takes an argument or returns a value: the dispatcher calls each entry with nothing pushed and discards eax.

//  __FILE__ for this unit: the string the mouse trampolines push in their asserts. The line numbers are the original's own.
static const char szCmdTblCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\Ui\\CmdTbl.cpp";

//  The registry key and value CMDTBL_ActionHelpScreen remembers itself under.
static const char szKeyName[] = "Diablo II";
static const char aHelpMenu[] = "Help Menu";

//  The original's names of the six mouse-hook globals and the two wheel ones, as the asserts push them.
static const char szSgpfnMouse3DownFn[] = "sgpfnMouse3DownFn";
static const char szSgpfnMouse3LiftFn[] = "sgpfnMouse3LiftFn";
static const char szSgpfnMouse4DownFn[] = "sgpfnMouse4DownFn";
static const char szSgpfnMouse4LiftFn[] = "sgpfnMouse4LiftFn";
static const char szSgpfnMouse5DownFn[] = "sgpfnMouse5DownFn";
static const char szSgpfnMouse5LiftFn[] = "sgpfnMouse5LiftFn";
static const char szSgpfnMouseWheelUpFn[] = "sgpfnMouseWheelUpFn";
static const char szSgpfnMouseWheelDownFn[] = "sgpfnMouseWheelDownFn";

//  The two dispatchers' asserts: they name sgtKeyFunctions and sgCurrCmdMapping (the original's name of gKeyBindings).
static const char szKeyFunctionsDown[] = "sgtKeyFunctions[sgCurrCmdMapping[idx].eCmd].FKeyDown";
static const char szKeyFunctionsLift[] = "sgtKeyFunctions[sgCurrCmdMapping[idx].eCmd].FKeyLift";

//  default.key is exactly twelve bytes with its terminator, which is why the original appends it with three dword stores instead of a call.
static const char aDefaultKey[] = "default.key";
static const char aDataLocal[] = "DATA\\LOCAL";

static_assert(UI_TOGGLE == 2 && UI_TURNON == 0 && UI_TURNOFF == 1,
              "most of the screen actions below are one UI_SetUIToggle, and "
              "which of the three they ask for is the whole difference");

//1.10f: D2Client.0x6FAD3720
//  Command 0. The screen toggles differ in the UI id, in whether bCheckMousePos is 1 or 0, and in what has to be true first. The character,
//  inventory, party and skill-tree screens pass 1 and refuse while the cursor is somewhere that forbids it; the message log, chat and help screen
//  pass 0.
void __cdecl CMDTBL_ActionCharacterScreen(void)
{
    UI_SetUIToggle(UI_STATSCREEN, UI_TOGGLE, 1);
}

//1.10f: D2Client.0x6FAD3730
//  Command 1.
void __cdecl CMDTBL_ActionInventoryScreen(void)
{
    UI_SetUIToggle(UI_INVENTORY, UI_TOGGLE, 1);
}

//1.10f: D2Client.0x6FAD3750
//  Command 2. Asks the game type first: single player answers 0 and the screen never opens.
void __cdecl CMDTBL_ActionPartyScreen(void)
{
    if (GAME_GetGameType())
    {
        UI_SetUIToggle(UI_PARTYSCREEN, UI_TOGGLE, 1);
    }
}

//1.10f: D2Client.0x6FAD3770
//  Command 3.
void __cdecl CMDTBL_ActionMessageLog(void)
{
    UI_SetUIToggle(UI_MSGLOG, UI_TOGGLE, 0);
}

//1.10f: D2Client.0x6FAD3790
//  Command 4.
void __cdecl CMDTBL_ActionQuestLog(void)
{
    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing())
    {
        QUESTLOG_TogglePanel(1);
    }
}

//1.10f: D2Client.0x6FAD37B0
//  Command 5. bFlag is set, so the chat key keeps working while the chat box is open, which is what closes it.
void __cdecl CMDTBL_ActionChat(void)
{
    if (!GAME_IsDisconnecting())
    {
        UI_SetUIToggle(UI_CHATBOX, UI_TOGGLE, 0);
    }
}

//1.10f: D2Client.0x6FAD37D0
//  Command 6. Opening the help screen writes HelpMenu = 1 under the game's registry key, and only when the value is missing or zero.
//  The original's extra local is a frame slot made by push ecx; SRegLoadValue overwrites it before the test that reads it.
void __cdecl CMDTBL_ActionHelpScreen(void)
{
    uint32_t dwValue;

    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing())
    {
        UI_SetUIToggle(UI_HELPSCREEN, UI_TOGGLE, 0);
        if (!STORM_SRegLoadValue(szKeyName, aHelpMenu, 1, &dwValue) || !dwValue)
        {
            dwValue = 1;
            STORM_SRegSaveValue(szKeyName, aHelpMenu, 1, 1);
        }
    }
}

//1.10f: D2Client.0x6FAD3840
//  Commands 7..11 and 45 are the automap block. This is the automap toggle: opening the automap does nothing else, closing it recentres,
//  which is why the test after the toggle reads the state back rather than assuming it.
void __cdecl CMDTBL_ActionAutomap(void)
{
    UI_SetUIToggle(UI_AUTOMAP, UI_TOGGLE, 0);
    if (!UI_GetUiWindowState(UI_AUTOMAP))
    {
        AUTOMAP_RecomputeCenterOffsets(0);
    }
}

//1.10f: D2Client.0x6FAD3870
//  Command 8: AUTOMAP_RecomputeCenterOffsets(bForce = 1); bForce means recentre even if the map already is.
void __cdecl CMDTBL_ActionCenterAutomap(void)
{
    AUTOMAP_RecomputeCenterOffsets(1);
}

//1.10f: D2Client.0x6FAD3880
//  Command 9. The fade cycles through four settings, so the key walks a ring rather than toggling.
void __cdecl CMDTBL_ActionFadeAutomap(void)
{
    AUTOMAP_SetFade((AUTOMAP_GetFade() + 1) % 4);
}

//1.10f: D2Client.0x6FAD38A0
//  Command 10.
void __cdecl CMDTBL_ActionPartyOnAutomap(void)
{
    AUTOMAP_SetShowPartyAndSave(AUTOMAP_GetShowParty() == 0);
}

//1.10f: D2Client.0x6FAD38C0
//  Command 11.
void __cdecl CMDTBL_ActionNamesOnAutomap(void)
{
    AUTOMAP_SetPartyNamesAndSave(AUTOMAP_GetPartyNames() == 0);
}

//1.10f: D2Client.0x6FAD38E0
//  Command 45, an expansion addition.
//  DIFFERENCE: the original tail-jumps into AUTOMAP_ApplyAutomapLeftSetting and so returns its answer (Left == 0); nothing reads it, and ours is
//  void and drops it.
void __cdecl CMDTBL_ActionToggleAutomapLeft(void)
{
    AUTOMAP_ApplyAutomapLeftSetting(AUTOMAP_GetLeft() == 0);
}

//1.10f: D2Client.0x6FAD3900
//  Command 12.
void __cdecl CMDTBL_ActionSkillTree(void)
{
    UI_SetUIToggle(UI_SKILLTREE, UI_TOGGLE, 1);
}

//1.10f: D2Client.0x6FAD3920
//  Command 13. Opening the mini skill selector always puts it in mode 0, so the two stores are one action and not a toggle plus a reset.
void __cdecl CMDTBL_ActionSkillSelect(void)
{
    UI_SetUIToggle(UI_MINISKILL, UI_TOGGLE, 0);
    SPELLSEL_SetMiniSkillMode(0);
}


//1.10f: D2Client.0x6FAD3940
//  Skill slot 0, command 14. The sixteen skill-slot actions are one body with the slot number baked in. Slots 0..8 need: not disconnecting,
//  not dead; slots 9..15 also need GAME_IsExpansionGame(). The command ids split differently: slots 0..7 are commands 14..21, slots 8..15 are 46..53.
//  With the mini skill selector open the hovered skill is bound to the slot; with it closed the slot is selected.
void __cdecl CMDTBL_ActionSelectSkillSlot0(void)
{
    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing())
    {
        if (UI_GetUiWindowState(UI_MINISKILL))
        {
            SPELLSEL_BindHoveredSkillToSlot(0);
        }
        else
        {
            SPELLSEL_SkillsSelectHotkeySlot(0);
        }
    }
}

//1.10f: D2Client.0x6FAD3970
//  Skill slot 1, command 15.
void __cdecl CMDTBL_ActionSelectSkillSlot1(void)
{
    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing())
    {
        if (UI_GetUiWindowState(UI_MINISKILL))
        {
            SPELLSEL_BindHoveredSkillToSlot(1);
        }
        else
        {
            SPELLSEL_SkillsSelectHotkeySlot(1);
        }
    }
}

//1.10f: D2Client.0x6FAD39A0
//  Skill slot 2, command 16.
void __cdecl CMDTBL_ActionSelectSkillSlot2(void)
{
    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing())
    {
        if (UI_GetUiWindowState(UI_MINISKILL))
        {
            SPELLSEL_BindHoveredSkillToSlot(2);
        }
        else
        {
            SPELLSEL_SkillsSelectHotkeySlot(2);
        }
    }
}

//1.10f: D2Client.0x6FAD39D0
//  Skill slot 3, command 17.
void __cdecl CMDTBL_ActionSelectSkillSlot3(void)
{
    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing())
    {
        if (UI_GetUiWindowState(UI_MINISKILL))
        {
            SPELLSEL_BindHoveredSkillToSlot(3);
        }
        else
        {
            SPELLSEL_SkillsSelectHotkeySlot(3);
        }
    }
}

//1.10f: D2Client.0x6FAD3A00
//  Skill slot 4, command 18.
void __cdecl CMDTBL_ActionSelectSkillSlot4(void)
{
    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing())
    {
        if (UI_GetUiWindowState(UI_MINISKILL))
        {
            SPELLSEL_BindHoveredSkillToSlot(4);
        }
        else
        {
            SPELLSEL_SkillsSelectHotkeySlot(4);
        }
    }
}

//1.10f: D2Client.0x6FAD3A30
//  Skill slot 5, command 19.
void __cdecl CMDTBL_ActionSelectSkillSlot5(void)
{
    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing())
    {
        if (UI_GetUiWindowState(UI_MINISKILL))
        {
            SPELLSEL_BindHoveredSkillToSlot(5);
        }
        else
        {
            SPELLSEL_SkillsSelectHotkeySlot(5);
        }
    }
}

//1.10f: D2Client.0x6FAD3A60
//  Skill slot 6, command 20.
void __cdecl CMDTBL_ActionSelectSkillSlot6(void)
{
    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing())
    {
        if (UI_GetUiWindowState(UI_MINISKILL))
        {
            SPELLSEL_BindHoveredSkillToSlot(6);
        }
        else
        {
            SPELLSEL_SkillsSelectHotkeySlot(6);
        }
    }
}

//1.10f: D2Client.0x6FAD3A90
//  Skill slot 7, command 21.
void __cdecl CMDTBL_ActionSelectSkillSlot7(void)
{
    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing())
    {
        if (UI_GetUiWindowState(UI_MINISKILL))
        {
            SPELLSEL_BindHoveredSkillToSlot(7);
        }
        else
        {
            SPELLSEL_SkillsSelectHotkeySlot(7);
        }
    }
}

//1.10f: D2Client.0x6FAD3AC0
//  Skill slot 8, command 46.
void __cdecl CMDTBL_ActionSelectSkillSlot8(void)
{
    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing())
    {
        if (UI_GetUiWindowState(UI_MINISKILL))
        {
            SPELLSEL_BindHoveredSkillToSlot(8);
        }
        else
        {
            SPELLSEL_SkillsSelectHotkeySlot(8);
        }
    }
}

//1.10f: D2Client.0x6FAD3AF0
//  Skill slot 9, command 47.
void __cdecl CMDTBL_ActionSelectSkillSlot9(void)
{
    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing() && GAME_IsExpansionGame())
    {
        if (UI_GetUiWindowState(UI_MINISKILL))
        {
            SPELLSEL_BindHoveredSkillToSlot(9);
        }
        else
        {
            SPELLSEL_SkillsSelectHotkeySlot(9);
        }
    }
}

//1.10f: D2Client.0x6FAD3B30
//  Skill slot 10, command 48.
void __cdecl CMDTBL_ActionSelectSkillSlot10(void)
{
    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing() && GAME_IsExpansionGame())
    {
        if (UI_GetUiWindowState(UI_MINISKILL))
        {
            SPELLSEL_BindHoveredSkillToSlot(10);
        }
        else
        {
            SPELLSEL_SkillsSelectHotkeySlot(10);
        }
    }
}

//1.10f: D2Client.0x6FAD3B70
//  Skill slot 11, command 49.
void __cdecl CMDTBL_ActionSelectSkillSlot11(void)
{
    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing() && GAME_IsExpansionGame())
    {
        if (UI_GetUiWindowState(UI_MINISKILL))
        {
            SPELLSEL_BindHoveredSkillToSlot(11);
        }
        else
        {
            SPELLSEL_SkillsSelectHotkeySlot(11);
        }
    }
}

//1.10f: D2Client.0x6FAD3BB0
//  Skill slot 12, command 50.
void __cdecl CMDTBL_ActionSelectSkillSlot12(void)
{
    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing() && GAME_IsExpansionGame())
    {
        if (UI_GetUiWindowState(UI_MINISKILL))
        {
            SPELLSEL_BindHoveredSkillToSlot(12);
        }
        else
        {
            SPELLSEL_SkillsSelectHotkeySlot(12);
        }
    }
}

//1.10f: D2Client.0x6FAD3BF0
//  Skill slot 13, command 51.
void __cdecl CMDTBL_ActionSelectSkillSlot13(void)
{
    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing() && GAME_IsExpansionGame())
    {
        if (UI_GetUiWindowState(UI_MINISKILL))
        {
            SPELLSEL_BindHoveredSkillToSlot(13);
        }
        else
        {
            SPELLSEL_SkillsSelectHotkeySlot(13);
        }
    }
}

//1.10f: D2Client.0x6FAD3C30
//  Skill slot 14, command 52.
void __cdecl CMDTBL_ActionSelectSkillSlot14(void)
{
    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing() && GAME_IsExpansionGame())
    {
        if (UI_GetUiWindowState(UI_MINISKILL))
        {
            SPELLSEL_BindHoveredSkillToSlot(14);
        }
        else
        {
            SPELLSEL_SkillsSelectHotkeySlot(14);
        }
    }
}

//1.10f: D2Client.0x6FAD3C70
//  Skill slot 15, command 53.
void __cdecl CMDTBL_ActionSelectSkillSlot15(void)
{
    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing() && GAME_IsExpansionGame())
    {
        if (UI_GetUiWindowState(UI_MINISKILL))
        {
            SPELLSEL_BindHoveredSkillToSlot(15);
        }
        else
        {
            SPELLSEL_SkillsSelectHotkeySlot(15);
        }
    }
}

//1.10f: D2Client.0x6FAD3CB0
//  Command 22, the belt; bFlag is set, so the belt opens while the chat box has the keyboard. Asks three questions in order: is there a player,
//  is there something in the belt slot, and is that thing an ITEMTYPE_BELT.
void __cdecl CMDTBL_ActionShowBelt(void)
{
    D2UnitStrc *pPlayer;
    D2UnitStrc *pBelt;

    pPlayer = CUNIT_GetPlayer();
    if (pPlayer)
    {
        pBelt = D2COMMON_INVENTORY_GetItemFromBodyLoc(pPlayer->pInventory, BODYLOC_BELT);
        if (pBelt)
        {
            if (D2COMMON_ITEMS_CheckItemTypeId(pBelt, ITEMTYPE_BELT))
            {
                UI_SetUIToggle(UI_BELTROWS, UI_TOGGLE, 0);
            }
        }
    }
}

//1.10f: D2Client.0x6FAD3CF0
//  Command 23, a five-byte jmp thunk. The belt keys are numbered from one and the belt columns from zero.
void __cdecl CMDTBL_ActionUseBelt1(void)
{
    PANEL_BeltUseColumn0();
}

//1.10f: D2Client.0x6FAD3D00
//  Command 24, a jmp thunk.
void __cdecl CMDTBL_ActionUseBelt2(void)
{
    PANEL_BeltUseColumn1();
}

//1.10f: D2Client.0x6FAD3D10
//  Command 25, a jmp thunk.
void __cdecl CMDTBL_ActionUseBelt3(void)
{
    PANEL_BeltUseColumn2();
}

//1.10f: D2Client.0x6FAD3D20
//  Command 26, a jmp thunk.
void __cdecl CMDTBL_ActionUseBelt4(void)
{
    PANEL_BeltUseColumn3();
}


//1.10f: D2Client.0x6FAD3D30
//  Command 27. The eight chat phrases are MSG_SendCmdWord(63, N) and nothing else (63 is 0x3F, the client's say-phrase packet; N runs 25..32),
//  all with bFlag set, so they fire while you are typing. Say Retreat is command 55 where the other seven are 27..33, and its tbl key is
//  CfgSay7X where theirs are CfgSay0..CfgSay6: the X is the expansion's own suffix.
void __cdecl CMDTBL_ActionSayHelp(void)
{
    MSG_SendCmdWord(63, 25);
}

//1.10f: D2Client.0x6FAD3D40
//  Command 28.
void __cdecl CMDTBL_ActionSayFollowMe(void)
{
    MSG_SendCmdWord(63, 26);
}

//1.10f: D2Client.0x6FAD3D50
//  Command 29.
void __cdecl CMDTBL_ActionSayThisIsForYou(void)
{
    MSG_SendCmdWord(63, 27);
}

//1.10f: D2Client.0x6FAD3D60
//  Command 30.
void __cdecl CMDTBL_ActionSayThanks(void)
{
    MSG_SendCmdWord(63, 28);
}

//1.10f: D2Client.0x6FAD3D70
//  Command 31.
void __cdecl CMDTBL_ActionSaySorry(void)
{
    MSG_SendCmdWord(63, 29);
}

//1.10f: D2Client.0x6FAD3D80
//  Command 32.
void __cdecl CMDTBL_ActionSayBye(void)
{
    MSG_SendCmdWord(63, 30);
}

//1.10f: D2Client.0x6FAD3D90
//  Command 33.
void __cdecl CMDTBL_ActionSayNowYouDie(void)
{
    MSG_SendCmdWord(63, 31);
}

//1.10f: D2Client.0x6FAD3DA0
//  Command 55, Say Retreat.
void __cdecl CMDTBL_ActionSayRetreat(void)
{
    MSG_SendCmdWord(63, 32);
}


//1.10f: D2Client.0x6FAD3DB0
//  Commands 34..37 are the movement block. Command 34, the only one of the four whose work is conditional. The key-held flag is raised
//  unconditionally, before any of the tests, so the client knows the key is down even when it refuses to act on it; the animation change and
//  the packet need a living player already in mode 2 (walking) with stamina left.
void __cdecl CMDTBL_ActionRun(void)
{
    D2UnitStrc *pUnit;

    GAME_SetRunKeyHeld(1);
    pUnit = CUNIT_GetPlayer();
    if (pUnit && !GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing() && pUnit->dwUnitType == UNIT_PLAYER && pUnit->dwAnimMode == 2)
    {
        if (D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_STAMINA, 0))
        {
            UNITMODE_SetAnimMode(pUnit, 3);
            MSG_SendCmd1(83);
        }
    }
}

//1.10f: D2Client.0x6FAD3E10
//  Command 35: the run lock, a setting that is flipped; command 34 reports a held key.
void __cdecl CMDTBL_ActionToggleRunWalk(void)
{
    GAME_SetRunToggled(GAME_GetRunToggled() == 0);
}

//1.10f: D2Client.0x6FAD3E30
//  Command 36. One store; its lift handler is the matching clear.
void __cdecl CMDTBL_ActionStandStill(void)
{
    GAME_SetStandStillKeyHeld(1);
}

//1.10f: D2Client.0x6FAD3E40
//  Command 37. UI_HOLDALT is turned on rather than toggled, so holding the key twice is the same as holding it once; the lift handler turns
//  it off.
void __cdecl CMDTBL_ActionShowItems(void)
{
    UI_SetUIToggle(UI_HOLDALT, UI_TURNON, 0);
}

//1.10f: D2Client.0x6FAD3E50
//  Command 38, a five-byte thunk: jmp GAME_CmdTblActionClearScreen, whose body is in another translation unit. A thunk keeps its target's
//  signature, which is why it returns int where every action beside it is void.
int32_t __cdecl j_GAME_CmdTblActionClearScreen(void)
{
    return GAME_CmdTblActionClearScreen();
}

//1.10f: D2Client.0x6FAD3E60
//  Command 56, the last id, an expansion addition. Esc is refused outright while an NPC dialog or a text popup is up. Otherwise it closes
//  the menu if the menu is open; if not, it tries to close every panel first and opens the menu only when there was nothing to close.
void __cdecl CMDTBL_ActionEscMenu(void)
{
    if (!NPCMENU_IsAnyDialogOpen() && !TEXT_IsPopupActive())
    {
        if (UI_GetUiWindowState(UI_ESCMENU))
        {
            ESCMENU_CloseMenu(1);
        }
        else if (!UI_CloseAllPanels(0, 1))
        {
            ESCMENU_SetPage(1, 0);
        }
    }
}

//1.10f: D2Client.0x6FAD3EB0
//  Command 39. The two skill-cycling keys are one function with the sign flipped.
void __cdecl CMDTBL_ActionSelectPreviousSkill(void)
{
    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing())
    {
        SPELLSEL_CycleSkill(-1);
    }
}

//1.10f: D2Client.0x6FAD3ED0
//  Command 40.
void __cdecl CMDTBL_ActionSelectNextSkill(void)
{
    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing())
    {
        SPELLSEL_CycleSkill(1);
    }
}

//1.10f: D2Client.0x6FAD3EF0
//  Command 44, an expansion addition; bFlag is set. Refuses while any of the three trading panels (the shop, the player trade and the stash)
//  is open, then calls INV_SwapWeaponsAndLockItems, which carries the expansion test of its own.
void __cdecl CMDTBL_ActionSwapWeapons(void)
{
    if (!UI_GetUiWindowState(UI_NPCSHOP) && !UI_GetUiWindowState(UI_MPTRADE) && !UI_GetUiWindowState(UI_STASH))
    {
        INV_SwapWeaponsAndLockItems();
    }
}

//1.10f: D2Client.0x6FAD3F20
//  Command 54, an expansion addition. Three gates in this order: the game must be expansion, the player must have a hireling (pet type 7
//  through ROSTERPETS_GetAssociatedUnitGUID, which answers -1 when there is none), and then FOG_IsExpansion is asked as well.
void __cdecl CMDTBL_ActionMercenaryInventory(void)
{
    if (GAME_IsExpansionGame())
    {
        if (ROSTERPETS_GetAssociatedUnitGUID(CUNIT_GetPlayer(), 7, 0) != -1)
        {
            if (FOG_IsExpansion())
            {
                UI_SetUIToggle(UI_MERCINV, UI_TOGGLE, 1);
            }
        }
    }
}

//1.10f: D2Client.0x6FAD3F60
//  Command 41; bFlag is set, so clearing the chat log works while typing. A five-byte jmp thunk to TEXT_ExpireAllChatMessages, void like its target.
void __cdecl j_TEXT_ExpireAllChatMessages(void)
{
    TEXT_ExpireAllChatMessages();
}

//1.10f: D2Client.0x6FAD3F70
//  Command 34's FKeyLift, the mirror of CMDTBL_ActionRun with one asymmetry: the run lock is consulted here and not there, so releasing the key
//  stops the run only when run-toggle is off.
void __cdecl CMDTBL_ActionRunReleased(void)
{
    D2UnitStrc *pUnit;

    GAME_SetRunKeyHeld(0);
    pUnit = CUNIT_GetPlayer();
    if (pUnit)
    {
        if (!GAME_GetRunToggled() && pUnit->dwUnitType == UNIT_PLAYER && pUnit->dwAnimMode == 3)
        {
            UNITMODE_SetAnimMode(pUnit, 2);
            MSG_SendCmd1(84);
        }
    }
}

//1.10f: D2Client.0x6FAD3FB0
//  Command 36's FKeyLift, the matching clear.
void __cdecl CMDTBL_ActionStandStillReleased(void)
{
    GAME_SetStandStillKeyHeld(0);
}

//1.10f: D2Client.0x6FAD3FC0
//  Command 37's FKeyLift: turns UI_HOLDALT off and also clears the selected unit, so letting go of the item-label key drops whatever the labels
//  had highlighted.
void __cdecl CMDTBL_ActionShowItemsReleased(void)
{
    UI_SetUIToggle(UI_HOLDALT, UI_TURNOFF, 0);
    SELECT_ClearSelectedUnit();
}


//1.10f: D2Client.0x6FAD3FE0
//  The mouse trampolines: six of them, 57 bytes each, and CMDTBL_CommandHandleMouseWheel is the seventh, both directions in one function.
//  Each reads its handler global, returns when it is null, runs IsBadCodePtr and asserts on a bad pointer, then JUMPS rather than calls, so
//  the handler returns to this function's caller. The handler globals are filled by CMDTBL_COMMAND_PrepareMouseMappings. They are void: the
//  callers (GAME_WindowOnMButtonDown and GAME_WindowOnMButtonUp) overwrite eax unread.
//  Assert sgpfnMouse3DownFn, CmdTbl.cpp:825.
void __cdecl CMDTBL_CallMouse3Down(void)
{
    FARPROC pfnHandler;

    pfnHandler = sgpfnMouse3DownFn;
    if (pfnHandler)
    {
        if (IsBadCodePtr(pfnHandler))
        {
            FOG_DisplayAssert(szSgpfnMouse3DownFn, szCmdTblCpp, 825);
            exit(-1);
        }
        pfnHandler();
    }
}

//1.10f: D2Client.0x6FAD4020
//  Assert sgpfnMouse3LiftFn, CmdTbl.cpp:837.
void __cdecl CMDTBL_CallMouse3Lift(void)
{
    FARPROC pfnHandler;

    pfnHandler = sgpfnMouse3LiftFn;
    if (pfnHandler)
    {
        if (IsBadCodePtr(pfnHandler))
        {
            FOG_DisplayAssert(szSgpfnMouse3LiftFn, szCmdTblCpp, 837);
            exit(-1);
        }
        pfnHandler();
    }
}

//1.10f: D2Client.0x6FAD4060
//  Assert sgpfnMouse4DownFn, CmdTbl.cpp:848.
void __cdecl CMDTBL_CommandHandleMouse4Down(void)
{
    FARPROC pfnHandler;

    pfnHandler = sgpfnMouse4DownFn;
    if (pfnHandler)
    {
        if (IsBadCodePtr(pfnHandler))
        {
            FOG_DisplayAssert(szSgpfnMouse4DownFn, szCmdTblCpp, 848);
            exit(-1);
        }
        pfnHandler();
    }
}

//1.10f: D2Client.0x6FAD40A0
//  Assert sgpfnMouse4LiftFn, CmdTbl.cpp:861.
void __cdecl CMDTBL_CommandHandleMouse4Lift(void)
{
    FARPROC pfnHandler;

    pfnHandler = sgpfnMouse4LiftFn;
    if (pfnHandler)
    {
        if (IsBadCodePtr(pfnHandler))
        {
            FOG_DisplayAssert(szSgpfnMouse4LiftFn, szCmdTblCpp, 861);
            exit(-1);
        }
        pfnHandler();
    }
}

//1.10f: D2Client.0x6FAD40E0
//  Assert sgpfnMouse5DownFn, CmdTbl.cpp:873.
void __cdecl CMDTBL_CommandHandleMouse5Down(void)
{
    FARPROC pfnHandler;

    pfnHandler = sgpfnMouse5DownFn;
    if (pfnHandler)
    {
        if (IsBadCodePtr(pfnHandler))
        {
            FOG_DisplayAssert(szSgpfnMouse5DownFn, szCmdTblCpp, 873);
            exit(-1);
        }
        pfnHandler();
    }
}

//1.10f: D2Client.0x6FAD4120
//  Assert sgpfnMouse5LiftFn, CmdTbl.cpp:885.
void __cdecl CMDTBL_CommandHandleMouse5Lift(void)
{
    FARPROC pfnHandler;

    pfnHandler = sgpfnMouse5LiftFn;
    if (pfnHandler)
    {
        if (IsBadCodePtr(pfnHandler))
        {
            FOG_DisplayAssert(szSgpfnMouse5LiftFn, szCmdTblCpp, 885);
            exit(-1);
        }
        pfnHandler();
    }
}

//1.10f: D2Client.0x6FAD4160
//  Both wheel directions in one function. The notch count is (short)HIWORD(wParam) / 120, signed, and zero notches reach neither handler:
//  the test is <= 0 then < 0 on one side and the else on the other. Each arm is the same read / IsBadCodePtr / assert / jump as the trampolines.
void __fastcall CMDTBL_CommandHandleMouseWheel(int32_t nDelta)
{
    if (nDelta <= 0)
    {
        if (nDelta < 0 && sgpfnMouseWheelDownFn)
        {
            if (IsBadCodePtr(sgpfnMouseWheelDownFn))
            {
                FOG_DisplayAssert(szSgpfnMouseWheelDownFn, szCmdTblCpp, 907);
                exit(-1);
            }
            (sgpfnMouseWheelDownFn)();
        }
    }
    else if (sgpfnMouseWheelUpFn)
    {
        if (IsBadCodePtr(sgpfnMouseWheelUpFn))
        {
            FOG_DisplayAssert(szSgpfnMouseWheelUpFn, szCmdTblCpp, 899);
            exit(-1);
        }
        (sgpfnMouseWheelUpFn)();
    }
}


//1.10f: D2Client.0x6FAD4760
//  Opens the player's .key file, trying two paths in order. With a save name set (-save <name>) it builds <save>\<name>\, creates the hierarchy
//  if it is missing and opens <that>\<player>.key; if that open fails, or no save name was given, it falls back to <save>\<player>.key.
//  The caller passes the desired access and the creation disposition through untouched, so one function serves the load (read, OPEN_EXISTING)
//  and the save (write, CREATE_ALWAYS). The three 260-byte locals are each cleared by writing gszEmptyString into the first byte and
//  memsetting the rest, the original's own idiom.
int32_t __fastcall CMDTBL_OpenPlayerKeyFile(int32_t dwDesiredAccess, int32_t dwCreationDisposition)
{
    char szSavePath[260];
    char szDir[260];
    char szFile[260];
    int32_t hFile;

    szFile[0] = gszEmptyString;
    memset(&szFile[1], 0, 0x100u);
    szDir[0] = gszEmptyString;
    memset(&szDir[1], 0, 0x100u);
    szSavePath[0] = gszEmptyString;
    memset(&szSavePath[1], 0, 259);

    FOG_GetSavePath((uint32_t)szFile, 260);
    if (*(char *)gszRealmName)
    {
        STORM_SStrPrintf(szSavePath, 0x104u, "%s%s\\", szFile, (const char *)gszRealmName);
        FOG_CreatePathHierarchy(szSavePath);
        STORM_SStrPrintf(szDir, 0x104u, "%s%s.key", szSavePath, gLocalPlayerName);
        hFile = FOG_CreateFileA((uint32_t)szDir, dwDesiredAccess, 0, 0, dwCreationDisposition, 128, 0);
        if (hFile != -1)
        {
            return hFile;
        }
    }

    //  reached both when there is no save name and when that open failed
    STORM_SStrPrintf(szDir, 0x104u, "%s%s.key", szFile, gLocalPlayerName);
    return FOG_CreateFileA((uint32_t)szDir, dwDesiredAccess, 0, 0, dwCreationDisposition, 128, 0);
}


//1.10f: D2Client.0x6FAD4B60
//  Returns the key bound to a command, or -1. The walk steps the 10-byte records comparing eCmd and bPrimaryHotkey; a command with no record
//  answers -1, and so does one whose record holds -1. CMDTBL_ControlsIsActionBound, CMDTBL_UnbindCommand, CMDTBL_ControlsGetBoundKeyName and its
//  fallback make the same walk and differ only in what they do when they find the record.
int16_t __fastcall CMDTBL_ControlsFindBoundKey(D2HotkeyIndices eCmd, BOOL bPrimary)
{
    int32_t nSlot;

    for (nSlot = 0; ; ++nSlot)
    {
        if (nSlot >= 114)
        {
            return -1;
        }
        if (gKeyBindings[nSlot].eCmd == eCmd && gKeyBindings[nSlot].bPrimaryHotkey == bPrimary)
        {
            break;
        }
    }
    return (int16_t)gKeyBindings[nSlot].VKCode;
}

//1.10f: D2Client.0x6FAD4F00
//  Is the command bound at all: the same walk, answering on the VK code. A missing record and a record holding -1 both answer 0.
BOOL __fastcall CMDTBL_ControlsIsActionBound(D2HotkeyIndices eCmd, BOOL bPrimary)
{
    int32_t nSlot;

    for (nSlot = 0; ; ++nSlot)
    {
        if (nSlot >= 114)
        {
            return 0;
        }
        if (gKeyBindings[nSlot].eCmd == eCmd && gKeyBindings[nSlot].bPrimaryHotkey == bPrimary)
        {
            break;
        }
    }
    return gKeyBindings[nSlot].VKCode != D2VK_INVALID;
}

//1.10f: D2Client.0x6FAD4EE0
//  Unbinds a command. Unlike the other walks it does not stop at the first match: it runs the whole array and clears every record that names
//  the command, so a binding duplicated into two records is cleared in both.
void __fastcall CMDTBL_UnbindCommand(int32_t eCmd, int32_t bPrimary)
{
    int32_t nSlot;

    for (nSlot = 0; nSlot < 114; ++nSlot)
    {
        if (gKeyBindings[nSlot].eCmd == eCmd && gKeyBindings[nSlot].bPrimaryHotkey == bPrimary)
        {
            gKeyBindings[nSlot].VKCode = D2VK_INVALID;
        }
    }
}

//1.10f: D2Client.0x6FAD4E90
//  The snapshot pair: Configure Controls edits the live array and can be cancelled, so the screen takes a copy on the way in and puts it back on
//  the way out. Both halves re-run the mouse mapping afterwards, because a restored binding changes which command each mouse button reaches.
//  The int16_t return is that call's; nothing reads it.
int16_t __cdecl CMDTBL_SaveBindingsSnapshot(void)
{
    memcpy(sgSavedCmdMapping, gKeyBindings, 114 * sizeof(HotkeyMappingEntry));
    return CMDTBL_COMMAND_PrepareMouseMappings();
}

//1.10f: D2Client.0x6FAD4EB0
//  Run when the controls screen is accepted (Cancel restores the snapshot instead): writes both binding files, then re-maps the mouse.
void __cdecl CMDTBL_CommitBindings(void)
{
    CMDTBL_SaveBindingsToFiles();
    CMDTBL_COMMAND_PrepareMouseMappings();
}

//1.10f: D2Client.0x6FAD4EC0
int16_t __cdecl CMDTBL_RestoreBindingsSnapshot(void)
{
    memcpy(gKeyBindings, sgSavedCmdMapping, 114 * sizeof(HotkeyMappingEntry));
    return CMDTBL_COMMAND_PrepareMouseMappings();
}

//1.10f: D2Client.0x6FAD49D0
//  CMDTBL_SetKeyHookMode(0, 0): unhooks the key handlers and asks for no release-table swap, so a shutdown leaves nothing registered at all.
void __cdecl CMDTBL_Shutdown(void)
{
    CMDTBL_SetKeyHookMode(0, 0);
}

//1.10f: D2Client.0x6FAD4F50
//  Returns the long name Configure Controls shows for whatever is bound to a command: the same walk as the others; a missing record and a record
//  holding -1 both answer tbl 3762 KeyNone. The table is split in two by the VK code at 0xDF: everything at or below is a real Windows virtual
//  key, and 256..260 are the five mouse pseudo-codes (which is why a mouse binding never reaches the key dispatchers). Case 4 and case 256 share
//  tbl 3766 KeyMButton: the middle button is VK_MBUTTON to Windows and pseudo-code 0x100 here.
//  The default arm does not answer KeyNone: it clears the 8-byte global gwszUnknownKeyNameFallback (a Unicode[4]), writes the VK code's low byte
//  into its first character and returns it, so a key with no tbl entry shows as the letter its code is. The buffer is global rather than a local
//  because the caller keeps the pointer, which makes it shared by every unnamed key at once.
const Unicode *__fastcall CMDTBL_ControlsGetBoundKeyNameFallback(D2HotkeyIndices eCmd, BOOL bPrimary)
{
    const Unicode *pwszKeyName;
    int32_t nSlot;
    //  unsigned, as the bytes use the word: cmp ax,0FFFFh and cmp ax,0DFh / jbe, and a zero-extension before the two switches; a signed word would
    //  sign-extend
    uint16_t nVKCode;

    for (nSlot = 0; ; ++nSlot)
    {
        if (nSlot >= 114)
        {
            return D2LANG_GetStringFromTblIndex(STR_IDX_3762_KeyNone);
        }
        if (gKeyBindings[nSlot].eCmd == eCmd && gKeyBindings[nSlot].bPrimaryHotkey == bPrimary)
        {
            break;
        }
    }

    nVKCode = (uint16_t)gKeyBindings[nSlot].VKCode;
    if (nVKCode == 0xFFFF)
    {
        return D2LANG_GetStringFromTblIndex(STR_IDX_3762_KeyNone);
    }

    if (nVKCode <= 0xDFu)
    {
        switch (nVKCode)
        {
        case 1:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3763_KeyLButton);
            break;
        case 2:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3764_KeyRButton);
            break;
        case 3:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3765_KeyCancel);
            break;
        case 4:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3766_KeyMButton);
            break;
        case 8:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3790_KeyBack);
            break;
        case 9:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3791_KeyTab);
            break;
        case 12:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3792_KeyClear);
            break;
        case 13:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3793_KeyReturn);
            break;
        case 16:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3794_KeyShift);
            break;
        case 17:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3795_KeyControl);
            break;
        case 18:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3796_KeyMenu);
            break;
        case 19:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3797_KeyPause);
            break;
        case 20:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3798_KeyCapital);
            break;
        case 21:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3771_KeyKana);
            break;
        case 23:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3772_KeyJunja);
            break;
        case 24:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3773_KeyFinal);
            break;
        case 25:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3774_KeyKanji);
            break;
        case 27:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3775_KeyEscape);
            break;
        case 28:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3776_KeyConvert);
            break;
        case 29:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3777_KeyNonConvert);
            break;
        case 30:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3778_KeyAccept);
            break;
        case 31:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3779_KeyModeChange);
            break;
        case 32:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3799_KeySpace);
            break;
        case 33:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3800_KeyPrior);
            break;
        case 34:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3801_KeyNext);
            break;
        case 35:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3802_KeyEnd);
            break;
        case 36:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3803_KeyHome);
            break;
        case 37:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3780_KeyLeft);
            break;
        case 38:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3781_KeyUp);
            break;
        case 39:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3782_KeyRight);
            break;
        case 40:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3783_KeyDown);
            break;
        case 41:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3784_KeySelect);
            break;
        case 42:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3804_KeyPrint);
            break;
        case 43:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3785_KeyExecute);
            break;
        case 44:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3805_KeySnapshot);
            break;
        case 45:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3806_KeyInsert);
            break;
        case 46:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3807_KeyDelete);
            break;
        case 47:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3808_KeyHelp);
            break;
        case 91:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3786_KeyLWin);
            break;
        case 92:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3787_KeyRWin);
            break;
        case 93:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3788_KeyApps);
            break;
        case 96:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3809_KeyNumPad0);
            break;
        case 97:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3810_KeyNumPad1);
            break;
        case 98:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3811_KeyNumPad2);
            break;
        case 99:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3812_KeyNumPad3);
            break;
        case 100:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3813_KeyNumPad4);
            break;
        case 101:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3814_KeyNumPad5);
            break;
        case 102:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3815_KeyNumPad6);
            break;
        case 103:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3816_KeyNumPad7);
            break;
        case 104:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3817_KeyNumPad8);
            break;
        case 105:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3818_KeyNumPad9);
            break;
        case 106:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3819_KeyMultiply);
            break;
        case 107:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3820_KeyAdd);
            break;
        case 108:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3821_KeySeparator);
            break;
        case 109:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3822_KeySubtract);
            break;
        case 110:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3823_KeyDecimal);
            break;
        case 111:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3824_KeyDivide);
            break;
        case 112:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3825_KeyF1);
            break;
        case 113:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3826_KeyF2);
            break;
        case 114:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3827_KeyF3);
            break;
        case 115:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3828_KeyF4);
            break;
        case 116:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3829_KeyF5);
            break;
        case 117:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3830_KeyF6);
            break;
        case 118:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3831_KeyF7);
            break;
        case 119:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3832_KeyF8);
            break;
        case 120:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3833_KeyF9);
            break;
        case 121:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3834_KeyF10);
            break;
        case 122:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3835_KeyF11);
            break;
        case 123:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3836_KeyF12);
            break;
        case 124:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3837_KeyF13);
            break;
        case 125:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3838_KeyF14);
            break;
        case 126:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3839_KeyF15);
            break;
        case 127:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3840_KeyF16);
            break;
        case 128:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3841_KeyF17);
            break;
        case 129:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3842_KeyF18);
            break;
        case 130:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3843_KeyF19);
            break;
        case 131:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3844_KeyF20);
            break;
        case 132:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3845_KeyF21);
            break;
        case 133:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3846_KeyF22);
            break;
        case 134:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3847_KeyF23);
            break;
        case 135:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3848_KeyF24);
            break;
        case 144:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3789_KeyNumLock);
            break;
        case 145:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3849_KeyScroll);
            break;
        case 186:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3850_KeySemicolon);
            break;
        case 187:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3851_KeyEqual);
            break;
        case 188:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3852_KeyComma);
            break;
        case 189:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3853_KeyMinus);
            break;
        case 190:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3854_KeyPeriod);
            break;
        case 191:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3855_KeySlash);
            break;
        case 192:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3856_KeyTilde);
            break;
        case 219:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3857_KeyLBracket);
            break;
        case 220:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3858_KeyBackslash);
            break;
        case 221:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3859_KeyRBracket);
            break;
        case 222:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3860_KeyApostrophe);
            break;
        default:
            //  no tbl name: print the code itself as one character
            memset(gwszUnknownKeyNameFallback, 0, 4 * sizeof(Unicode));
            gwszUnknownKeyNameFallback[0].ch = (uint8_t)nVKCode;
            pwszKeyName = gwszUnknownKeyNameFallback;
            break;
        }
    }
    else
    {
        switch (nVKCode)
        {
        case 256:
        case 257:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3767_Key4Button);
            break;
        case 258:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3768_Key5Button);
            break;
        case 259:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3769_KeyWheelUp);
            break;
        case 260:
            pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3770_KeyWheelDown);
            break;
        default:
            memset(gwszUnknownKeyNameFallback, 0, 4 * sizeof(Unicode));
            gwszUnknownKeyNameFallback[0].ch = (uint8_t)nVKCode;
            pwszKeyName = gwszUnknownKeyNameFallback;
            break;
        }
    }
    return pwszKeyName;
}

//1.10f: D2Client.0x6FAD56B0
//  Returns the short key name: the Shorthand... tbl strings drawn on the skill bar and in the panels rather than in Configure Controls.
//  It is a subset with a fall-through, not a second table: thirty-one keys have a shorthand, and everything else, with an unbound command
//  (which reaches the switch with -1), goes to the long table, so a key with no abbreviation shows its full name and one with neither shows itself.
//  The walk differs from the others in one detail: a command with no record does not return early but sets nVKCode = 0xFFFF (the bindings' -1,
//  as a word) and joins the switch. The word is unsigned, as the bytes take it: it is zero-extended before the switch.
const Unicode *__fastcall CMDTBL_ControlsGetBoundKeyName(D2HotkeyIndices eCmd, BOOL bPrimary)
{
    const Unicode *pwszKeyName;
    int32_t nSlot;
    uint16_t nVKCode;

    nVKCode = 0xFFFF;
    for (nSlot = 0; nSlot < 114; ++nSlot)
    {
        if (gKeyBindings[nSlot].eCmd == eCmd && gKeyBindings[nSlot].bPrimaryHotkey == bPrimary)
        {
            nVKCode = (uint16_t)gKeyBindings[nSlot].VKCode;
            break;
        }
    }

    switch (nVKCode)
    {
    case 8:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3884_ShorthandKeyBackspace);
        break;
    case 16:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3887_ShorthandKeyShift);
        break;
    case 17:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3888_ShorthandKeyControl);
        break;
    case 27:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3870_ShorthandKeyEscape);
        break;
    case 33:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3892_ShorthandKeyPageUp);
        break;
    case 34:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3893_ShorthandKeyPageDown);
        break;
    case 44:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3895_ShorthandKeyPrintScreen);
        break;
    case 45:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3896_ShorthandKeyInsert);
        break;
    case 46:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3897_ShorthandKeyDelete);
        break;
    case 96:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3899_ShorthandKeyNumPad0);
        break;
    case 97:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3900_ShorthandKeyNumPad1);
        break;
    case 98:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3901_ShorthandKeyNumPad2);
        break;
    case 99:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3902_ShorthandKeyNumPad3);
        break;
    case 100:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3903_ShorthandKeyNumPad4);
        break;
    case 101:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3904_ShorthandKeyNumPad5);
        break;
    case 102:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3905_ShorthandKeyNumPad6);
        break;
    case 103:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3906_ShorthandKeyNumPad7);
        break;
    case 104:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3907_ShorthandKeyNumPad8);
        break;
    case 105:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3908_ShorthandKeyNumPad9);
        break;
    case 106:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3909_ShorthandKeyNumPad_STAR_);
        break;
    case 107:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3910_ShorthandKeyNumPad_PLUS_);
        break;
    case 108:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3912_ShorthandKeyNumPad_DOT_);
        break;
    case 109:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3911_ShorthandKeyNumPad_MINUS_);
        break;
    case 111:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3913_ShorthandKeyNumPad_SLASH_);
        break;
    case 144:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3883_ShorthandKeyNumLock);
        break;
    case 145:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3914_ShorthandKeyScroll);
        break;
    case 256:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3861_ShorthandKeyMButton);
        break;
    case 257:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3862_ShorthandKey4Button);
        break;
    case 258:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3863_ShorthandKey5Button);
        break;
    case 259:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3864_ShorthandKeyWheelUp);
        break;
    case 260:
        pwszKeyName = D2LANG_GetStringFromTblIndex(STR_IDX_3865_ShorthandKeyWheelDown);
        break;
    default:
        pwszKeyName = CMDTBL_ControlsGetBoundKeyNameFallback(eCmd, bPrimary);
        break;
    }
    return pwszKeyName;
}


//1.10f: D2Client.0x6FAD4B90
//  May this key be bound: two switches, split at 0x2A. A blacklist and a whitelist together; the default of both arms is
//  isalpha(code) || isdigit(code), so a letter or a digit is bindable unless it is a listed reject. Refused: the Windows keys (0x5B, 0x5C),
//  the application key (0x5D), NumLock (0x90), the separator (0x2B), mouse buttons 1..4 (bound through the pseudo-codes instead), and the
//  modifier and arrow families (0x15..0x1F, 0x25..0x29). Allowed includes 0x100..0x104, the mouse pseudo-codes, so a mouse button can be typed
//  into the binding screen.
BOOL __fastcall CMDTBL_IsKeyBindable(uint16_t nVKCode)
{
    if (nVKCode > 0x2Au)
    {
        switch (nVKCode)
        {
        case 0x2Bu:
        case 0x5Bu:
        case 0x5Cu:
        case 0x5Du:
        case 0x90u:
            return 0;
        case 0x2Cu:
        case 0x2Du:
        case 0x2Eu:
        case 0x2Fu:
        case 0x60u:
        case 0x61u:
        case 0x62u:
        case 0x63u:
        case 0x64u:
        case 0x65u:
        case 0x66u:
        case 0x67u:
        case 0x68u:
        case 0x69u:
        case 0x6Au:
        case 0x6Bu:
        case 0x6Cu:
        case 0x6Du:
        case 0x6Eu:
        case 0x6Fu:
        case 0x70u:
        case 0x71u:
        case 0x72u:
        case 0x73u:
        case 0x74u:
        case 0x75u:
        case 0x76u:
        case 0x77u:
        case 0x78u:
        case 0x79u:
        case 0x7Au:
        case 0x7Bu:
        case 0x7Cu:
        case 0x7Du:
        case 0x7Eu:
        case 0x7Fu:
        case 0x80u:
        case 0x81u:
        case 0x82u:
        case 0x83u:
        case 0x84u:
        case 0x85u:
        case 0x86u:
        case 0x87u:
        case 0x91u:
        case 0xBAu:
        case 0xBBu:
        case 0xBCu:
        case 0xBDu:
        case 0xBEu:
        case 0xBFu:
        case 0xC0u:
        case 0xDBu:
        case 0xDCu:
        case 0xDDu:
        case 0xDEu:
        case 0x100u:
        case 0x101u:
        case 0x102u:
        case 0x103u:
        case 0x104u:
            return 1;
        default:
            break;
        }
        if (isalpha(nVKCode) || isdigit(nVKCode))
        {
            return 1;
        }
        return 0;
    }

    if (nVKCode == 42)
    {
        return 1;
    }
    switch (nVKCode)
    {
        case 0x1u:
        case 0x2u:
        case 0x3u:
        case 0x4u:
        case 0x15u:
        case 0x17u:
        case 0x18u:
        case 0x19u:
        case 0x1Bu:
        case 0x1Cu:
        case 0x1Du:
        case 0x1Eu:
        case 0x1Fu:
        case 0x25u:
        case 0x26u:
        case 0x27u:
        case 0x28u:
        case 0x29u:
        return 0;
        case 0x8u:
        case 0x9u:
        case 0xCu:
        case 0xDu:
        case 0x10u:
        case 0x11u:
        case 0x12u:
        case 0x13u:
        case 0x14u:
        case 0x20u:
        case 0x21u:
        case 0x22u:
        case 0x23u:
        case 0x24u:
        return 1;
    default:
        break;
    }
    if (isalpha(nVKCode) || isdigit(nVKCode))
    {
        return 1;
    }
    return 0;
}

//1.10f: D2Client.0x6FAD4D20
//  Binds a key to a command and reports why it refused through a tbl index the caller can show: 3977 the mouse button needs a Windows version
//  that has it, 3978 the wheel needs wheel support and a command with no key-lift handler (the wheel has no release to send), 3979 the key is
//  not bindable at all. The version tests are per pseudo-code: 0x100 wants GAME_IsSupportedWindows, 0x101 and 0x102 want Windows 2000 or XP,
//  and the two wheel codes want the Win95 workaround or native wheel support.
//  PrintScreen (44, 0x2C) is refused for any command that has a down handler, so it can be bound only to the screenshot, command 42.
//  Before binding, the array is searched for the key; a record that holds it is cleared to -1 and its index remembered. If the command has no
//  record at all, the key goes back into the record it was taken from. eCmd is bounded by < 56: command 56, the Esc menu, cannot be rebound.
BOOL __fastcall CMDTBL_BindKey(int32_t eCmd, int32_t bPrimary, uint16_t nVKCode, int16_t *pnErrorTblIdx)
{
    int32_t nTakenFrom;
    int32_t nSlot;

    if (pnErrorTblIdx)
    {
        *pnErrorTblIdx = 0;
    }
    if (eCmd >= 56)
    {
        return 0;
    }
    if (!CMDTBL_IsKeyBindable(nVKCode))
    {
        if (pnErrorTblIdx)
        {
            *pnErrorTblIdx = 3979;
        }
        return 0;
    }

    switch (nVKCode)
    {
    case 0x100u:
        if (!GAME_IsSupportedWindows())
        {
            if (pnErrorTblIdx)
            {
                *pnErrorTblIdx = 3977;
            }
            return 0;
        }
        break;
    case 0x101u:
    case 0x102u:
        if (!GAME_IsWin2000OrXp())
        {
            if (pnErrorTblIdx)
            {
                *pnErrorTblIdx = 3977;
            }
            return 0;
        }
        break;
    case 0x103u:
    case 0x104u:
        if ((!GAME_NeedsWin95WheelWorkaround() && !GAME_HasNativeMouseWheel()) || sgtKeyFunctions[eCmd].FKeyLift)
        {
            if (pnErrorTblIdx)
            {
                *pnErrorTblIdx = 3978;
            }
            return 0;
        }
        break;
    default:
        break;
    }

    //  PrintScreen belongs to the one command with no down handler
    if (nVKCode == 44 && sgtKeyFunctions[eCmd].FKeyDown)
    {
        if (pnErrorTblIdx)
        {
            *pnErrorTblIdx = 3979;
        }
        return 0;
    }

    //  take the key from whoever holds it
    nTakenFrom = 114;
    for (nSlot = 0; nSlot < 114; ++nSlot)
    {
        if ((int16_t)gKeyBindings[nSlot].VKCode == (int16_t)nVKCode)
        {
            nTakenFrom = nSlot;
            gKeyBindings[nSlot].VKCode = D2VK_INVALID;
            break;
        }
    }

    for (nSlot = 0; nSlot < 114; ++nSlot)
    {
        if (gKeyBindings[nSlot].eCmd == eCmd && gKeyBindings[nSlot].bPrimaryHotkey == bPrimary)
        {
            gKeyBindings[nSlot].VKCode = (D2VirtualKeys)nVKCode;
            return 1;
        }
    }

    //  the command has no record: put the key back where it came from
    if (nTakenFrom != 114)
    {
        gKeyBindings[nTakenFrom].VKCode = (D2VirtualKeys)nVKCode;
    }
    return 0;
}

//1.10f: D2Client.0x6FAD5A00
//  A key press turned into a command. The repeat bit is tested first, so a held key produces one command. The walk compares the message's
//  wParam against each record's VK code and takes the first record whose command is real (!= 0xFFFF), under 57, and has a down handler.
//  Alt+F4 is let through to Windows: key 115 is F4, and with Alt held the handler does nothing, not even consume, so the message reaches
//  DefWindowProc and closes the game. In mode 2 (gnKeyHookMode, stored by CMDTBL_SetKeyHookMode) the command's bFlag has to be set. The entry is
//  checked with IsBadCodePtr, with an assert behind it, before it is called.
void __stdcall CMDTBL_OnKeyDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nSlot;
    int32_t eCmd;

    if ((pMsg->lParam & 0x40000000) != 0)
    {
        return;
    }

    for (nSlot = 0; ; ++nSlot)
    {
        if (nSlot >= 114)
        {
            return;
        }
        if ((uint16_t)gKeyBindings[nSlot].VKCode == pMsg->wParam)
        {
            eCmd = gKeyBindings[nSlot].eCmd;
            if (eCmd != 0xFFFF && eCmd < 57 && sgtKeyFunctions[eCmd].FKeyDown)
            {
                break;
            }
        }
    }

    //  Alt+F4 goes to Windows
    if (gKeyBindings[nSlot].VKCode == 115 && (((uint16_t)GetKeyState(18) >> 8) & 0x80u) != 0)
    {
        return;
    }
    //  mode 2: only the flagged commands
    if (gnKeyHookMode == 2 && !sgtKeyFunctions[gKeyBindings[nSlot].eCmd].bUnk0x8)
    {
        return;
    }

    if (IsBadCodePtr((FARPROC)sgtKeyFunctions[gKeyBindings[nSlot].eCmd].FKeyDown))
    {
        FOG_DisplayAssert(szKeyFunctionsDown, szCmdTblCpp, 2062);
        exit(-1);
    }
    sgtKeyFunctions[gKeyBindings[nSlot].eCmd].FKeyDown();
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAD5B00
//  The release counterpart: the same walk against FKeyLift. Only four records have one, so nearly every key-up falls off the end of the array.
//  There is no repeat test (a key-up has no repeat) and no Alt+F4 guard (F4's release is harmless).
void __stdcall CMDTBL_OnKeyUp(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nSlot;
    int32_t eCmd;

    for (nSlot = 0; ; ++nSlot)
    {
        if (nSlot >= 114)
        {
            return;
        }
        if ((uint16_t)gKeyBindings[nSlot].VKCode == pMsg->wParam)
        {
            eCmd = gKeyBindings[nSlot].eCmd;
            if (eCmd != 0xFFFF && eCmd < 57 && sgtKeyFunctions[eCmd].FKeyLift)
            {
                break;
            }
        }
    }

    if (gnKeyHookMode == 2 && !sgtKeyFunctions[gKeyBindings[nSlot].eCmd].bUnk0x8)
    {
        return;
    }

    if (IsBadCodePtr((FARPROC)sgtKeyFunctions[gKeyBindings[nSlot].eCmd].FKeyLift))
    {
        FOG_DisplayAssert(szKeyFunctionsLift, szCmdTblCpp, 2100);
        exit(-1);
    }
    sgtKeyFunctions[gKeyBindings[nSlot].eCmd].FKeyLift();
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAD5BE0
//  Hooks or unhooks the key handlers and records the mode the dispatchers consult. Two tables: the full four-record one (WM_KEYDOWN and
//  WM_SYSKEYDOWN to CMDTBL_OnKeyDown, WM_KEYUP and WM_SYSKEYUP to CMDTBL_OnKeyUp) and a two-record release-only one. bSwapReleaseHandlers is
//  used twice, in opposite arms of the same if:
//      nMode != 0   if (flag) unregister the RELEASE table, then register the COMMAND table
//      nMode == 0   unregister the COMMAND table, then if (flag) register the RELEASE table
//  So the flag asks for key-up events to keep arriving while key-down commands are suspended. Both registrations sit behind their own latch,
//  so the swap is idempotent.
void __fastcall CMDTBL_SetKeyHookMode(int32_t nMode, int32_t bSwapReleaseHandlers)
{
    gnKeyHookMode = nMode;

    if (nMode)
    {
        if (!gbKeyCommandHandlersRegistered)
        {
            gbKeyCommandHandlersRegistered = 1;
            if (bSwapReleaseHandlers && gbKeyCommandReleaseHandlersRegistered)
            {
                WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gKeyCommandReleaseHandlers, 2);
                gbKeyCommandReleaseHandlersRegistered = 0;
            }
            WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gKeyCommandHandlers, 4);
        }
    }
    else if (gbKeyCommandHandlersRegistered == 1)
    {
        gbKeyCommandHandlersRegistered = 0;
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gKeyCommandHandlers, 4);
        if (bSwapReleaseHandlers && !gbKeyCommandReleaseHandlersRegistered)
        {
            WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gKeyCommandReleaseHandlers, 2);
            gbKeyCommandReleaseHandlersRegistered = 1;
        }
    }
}


//1.10f: D2Client.0x6FAD41E0
//  Loads the player's bindings and validates them before use. The file is 1142 bytes: a version word of 37 and 114 ten-byte records. Anything
//  else (a short read, a wrong version, a failed open) falls through to the defaults. Two checks run on the loaded data: every one of the 57
//  commands must appear (an older build's file is rejected whole, not merged), and no key may appear twice (every record against every other,
//  skipping -1 and itself); a failure of either goes to CMDTBL_CommandLoadDefaultKeyBindings. Only when both pass is gKeyBindings cleared to -1
//  and overwritten, and the mouse mapping re-run; the clear is redundant with the copy, but the original does it first.
//  Both paths end at CMDTBL_SetKeyHookMode(1, 0), which arms the keyboard at startup; only the fallback saves the files first.
//  The original inlines CMDTBL_CommandLoadDefaultKeyBindings here.
void __cdecl CMDTBL_LoadDefaultBindings(void)
{
    void *hFile;
    int32_t nBytesRead;
    uint32_t aFileData[286];
    int32_t nCmd;
    int32_t nEntry;
    int32_t nOther;
    int32_t bFound;
    HotkeyMappingEntry *pEntry;
    int32_t nSlot;

    hFile = (void *)CMDTBL_OpenPlayerKeyFile(0x80000000, 3);
    if (hFile == (void *)-1)
    {
        goto USE_DEFAULTS;
    }

    memset(aFileData, 0, 1142);
    nBytesRead = 0;
    FOG_10110((uint32_t)hFile, (uint32_t)aFileData, 1142, (uint32_t)&nBytesRead, 0);
    FOG_CloseFile(hFile);

    if (nBytesRead != 1142 || (uint16_t)aFileData[0] != 37)
    {
        goto USE_DEFAULTS;
    }

    //  every command must have a record
    pEntry = (HotkeyMappingEntry *)((char *)aFileData + 2);
    for (nCmd = 0; nCmd < 57; ++nCmd)
    {
        bFound = 0;
        for (nEntry = 0; nEntry < 114; ++nEntry)
        {
            if (pEntry[nEntry].eCmd == nCmd)
            {
                bFound = 1;
                break;
            }
        }
        if (!bFound)
        {
            goto USE_DEFAULTS;
        }
    }

    //  and no key may be bound twice
    for (nEntry = 0; nEntry < 114; ++nEntry)
    {
        if (pEntry[nEntry].VKCode == D2VK_INVALID)
        {
            continue;
        }
        for (nOther = 0; nOther < 114; ++nOther)
        {
            if (nOther != nEntry && pEntry[nEntry].VKCode == pEntry[nOther].VKCode)
            {
                goto USE_DEFAULTS;
            }
        }
    }

    for (nSlot = 0; nSlot < 114; ++nSlot)
    {
        gKeyBindings[nSlot].VKCode = D2VK_INVALID;
    }
    memcpy(gKeyBindings, (char *)aFileData + 2, 114 * sizeof(HotkeyMappingEntry));
    CMDTBL_COMMAND_PrepareMouseMappings();
    //  both paths end at the hook
    goto HOOK_KEYS;

USE_DEFAULTS:
    CMDTBL_CommandLoadDefaultKeyBindings((void *)1);
    CMDTBL_SaveBindingsToFiles();   //  fallback path only

HOOK_KEYS:
    //  mode 1, no swap. This is what arms the keyboard: without it every key is dead until something else calls CMDTBL_SetKeyHookMode, opening a
    //  panel for one.
    CMDTBL_SetKeyHookMode(1, 0);
}

//1.10f: D2Client.0x6FAD4890
//  Writes both files. The player's .key first: a version word of 37 and the 114 records, 1142 bytes. The array is pre-filled with -1 before the
//  copy, which is dead work (the memcpy that follows replaces all 1140 bytes) and is kept because the original does it. Then <install>\default.key,
//  1146 bytes: a six-byte header { 21335 (0x5357, WS read as a little-endian word), 37 (the same version), 1146 (its own length, header included) }
//  and the same 1140 bytes of records, the header written as one dword and one word. The path is built by appending twelve bytes of aDefaultKey
//  to the install path; the original does it as three dword stores, ours as a strcat.
//  A failure to open the second file is silent: the first is already written, so the player's bindings survive and only the shared default is lost.
void __cdecl CMDTBL_SaveBindingsToFiles(void)
{
    void *hFile;
    void *hDefault;
    int32_t nBytesWritten;
    //  BUG: the original writes each file's header and body as ONE call from the header local's address (2 + 1140 bytes, and 4 + 2 for the
    //  default file's header), relying on the body its compiler placed behind it.
    //  DIFFERENCE: ours is the defined program: each file is one object and each write starts at its base, because reading or writing past one
    //  object into the next is undefined in C++. The bytes on disk are identical.
    uint8_t abDefaultHeader[6];
    uint8_t abBindingsFile[1142];
    char szPath[260];
    int32_t nSlot;

    hFile = (void *)CMDTBL_OpenPlayerKeyFile(0x40000000, 2);
    if (hFile == (void *)-1)
    {
        return;
    }

    memset(abBindingsFile + 2, 0, 0x474u);
    nBytesWritten = 0;
    *(int16_t *)abBindingsFile = 37;
    //  dead: the memcpy below replaces every byte of it. The fill starts at +4 of each record, which is where VKCode sits.
    for (nSlot = 0; nSlot < 114; ++nSlot)
    {
        *(int16_t *)&abBindingsFile[2 + 10 * nSlot + 4] = -1;
    }
    memcpy(abBindingsFile + 2, gKeyBindings, 114 * sizeof(HotkeyMappingEntry));
    FOG_10111((uint32_t)hFile, (uint32_t)abBindingsFile, 1142, (uint32_t)&nBytesWritten, 0);
    FOG_CloseFile(hFile);

    FOG_GetInstallPath((uint32_t)szPath, 220);
    strcat(szPath, aDefaultKey);
    hDefault = (void *)FOG_CreateFileA((uint32_t)szPath, 0x40000000, 0, 0, 2, 128, 0);
    if (hDefault != (void *)-1)
    {
        //  21335 at +0 and 37 at +2: the original stores them as two words, which is the dword 37 << 16 | 21335
        *(int32_t *)abDefaultHeader = 2446167;
        *(int16_t *)(abDefaultHeader + 4) = 1146;
        FOG_10111((uint32_t)hDefault, (uint32_t)abDefaultHeader, 6, (uint32_t)&nBytesWritten, 0);
        FOG_10111((uint32_t)hDefault, (uint32_t)gKeyBindings, 1140, (uint32_t)&nBytesWritten, 0);
        FOG_CloseFile(hDefault);
    }
}

//1.10f: D2Client.0x6FAD49E0
//  Loads the defaults from disk if there is a default.key beside the game, from the MPQ if not. The disk file must be exactly 1146 bytes;
//  anything else joins the MPQ path, which reads data\local\CMD\<lang>\default.key, so the defaults are per language (D2Lang is asked for the
//  code first). Either way the buffer is validated on its three header words (magic 21335, version 37, length 1146), and a file that fails is
//  freed and ignored rather than replaced: gDefaultCommandTable keeps whatever it had. Then the live array is cleared to -1, overwritten from
//  the defaults, and the mouse mapping re-run. The `this` parameter is a flag, not an object: zero skips the disk file and goes to the MPQ.
int16_t __fastcall CMDTBL_CommandLoadDefaultKeyBindings(void *bTryDiskFile)
{
    int32_t nLangCode;
    int32_t nBytesRead;
    char szPath[260];
    //  BUG: the original declares a 1144-byte buffer and reads 1146 bytes into it, so the last two bytes land on the local its compiler placed
    //  immediately behind; it then copies only 1144 and puts those two back by hand.
    //  DIFFERENCE: ours is the defined program, since the read is a write past an array, which is undefined in C++: one object of 1146 bytes and one
    //  copy of 1146. The bytes that reach pFile are identical.
    uint8_t aReadBuffer[1146];
    uint16_t *pFile;
    void *hFile;
    int32_t nSlot;

    D2LANG_10006((uint32_t)&nLangCode, 0);
    pFile = NULL;

    if (bTryDiskFile)
    {
        FOG_GetInstallPath((uint32_t)szPath, 220);
        strcat(szPath, aDefaultKey);
        hFile = (void *)FOG_CreateFileA((uint32_t)szPath, 0x80000000, 0, 0, 3, 128, 0);
        if (hFile != (void *)-1)
        {
            FOG_10110((uint32_t)hFile, (uint32_t)aReadBuffer, 1146, (uint32_t)&nBytesRead, 0);
            FOG_CloseFile(hFile);
            if (nBytesRead == 1146)
            {
                pFile = (uint16_t *)FOG_AllocPool(0, 1146, szCmdTblCpp, 1319, 0);
                memcpy(pFile, aReadBuffer, 1146);
            }
        }
    }

    if (!pFile)
    {
        sprintf(szPath, "%s\\CMD\\%s\\default.key", aDataLocal, (const char *)&nLangCode);
        pFile = (uint16_t *)ARCHIVE_LoadFile(D2WIN_GetArchive(), szPath, NULL, szCmdTblCpp, 1334);
    }

    if (pFile)
    {
        if (pFile[0] == 21335 && pFile[1] == 37 && pFile[2] == 1146)
        {
            memcpy(gDefaultCommandTable, pFile + 3, 114 * sizeof(HotkeyMappingEntry));
            FOG_Free(pFile, szCmdTblCpp, 1348, 0);
        }
        else
        {
            //  a bad file is dropped; the table keeps what it had
            FOG_Free(pFile, szCmdTblCpp, 1353, 0);
        }
    }

    for (nSlot = 0; nSlot < 114; ++nSlot)
    {
        gKeyBindings[nSlot].VKCode = D2VK_INVALID;
    }
    memcpy(gKeyBindings, gDefaultCommandTable, 114 * sizeof(HotkeyMappingEntry));
    return CMDTBL_COMMAND_PrepareMouseMappings();
}

//1.10f: D2Client.0x6FAD4470
//  Resolves the mouse bindings once, so a mouse event costs a global read and an indirect jump. All eight hook globals are cleared, then the
//  bindings are walked and every record whose VK code is above 0xDF (one of the five mouse pseudo-codes 0x100..0x104) has its command's FKeyDown
//  and FKeyLift copied into the matching pair. It is re-run after every path that can change a binding: loading either file, taking or restoring
//  the snapshot, and closing the controls screen. Each code is admitted only if the machine has it: 0x100 wants GAME_IsSupportedWindows,
//  0x101 and 0x102 want Windows 2000 or XP, and the two wheel codes want the Win95 workaround or native wheel support and a command with no lift
//  handler (the tests CMDTBL_BindKey makes when the binding is created, repeated because a binding file can carry a code this machine cannot serve).
//  The original's jump table gives each pseudo-code its own arm that re-tests the same value; here the validity switch is written once and the
//  assignment is switched, the same in effect.
int16_t __cdecl CMDTBL_COMMAND_PrepareMouseMappings(void)
{
    int32_t nSlot;
    int32_t eCmd;
    int32_t bUsable;

    sgpfnMouseWheelUpFn = NULL;
    sgpfnMouseWheelDownFn = NULL;
    sgpfnMouse3DownFn = NULL;
    sgpfnMouse3LiftFn = NULL;
    sgpfnMouse4DownFn = NULL;
    sgpfnMouse4LiftFn = NULL;
    sgpfnMouse5DownFn = NULL;
    sgpfnMouse5LiftFn = NULL;

    for (nSlot = 0; nSlot < 114; ++nSlot)
    {
        if ((uint16_t)gKeyBindings[nSlot].VKCode <= 0xDFu)
        {
            continue;
        }
        eCmd = gKeyBindings[nSlot].eCmd;
        if (eCmd >= HOTKEY_MAX_COUNT)
        {
            continue;
        }

        //  The cast is the original's and eax, ebx (ebx holds 0FFFFh): the code is zero-extended before add eax, 0FFFFFF00h / cmp eax, 4.
        //  D2VirtualKeys has a signed base (D2VK_INVALID is -1), so without the cast this switch and the one below would sign-extend (movsx).
        switch ((uint16_t)gKeyBindings[nSlot].VKCode)
        {
        case 0x100:
            bUsable = GAME_IsSupportedWindows();
            break;
        case 0x101:
        case 0x102:
            bUsable = GAME_IsWin2000OrXp();
            break;
        case 0x103:
        case 0x104:
            bUsable = (GAME_NeedsWin95WheelWorkaround() || GAME_HasNativeMouseWheel()) && !sgtKeyFunctions[eCmd].FKeyLift;
            break;
        default:
            bUsable = 0;
            break;
        }
        if (!bUsable)
        {
            continue;
        }

        switch ((uint16_t)gKeyBindings[nSlot].VKCode)
        {
        case 0x100:
            sgpfnMouse3DownFn = (FARPROC)sgtKeyFunctions[eCmd].FKeyDown;
            sgpfnMouse3LiftFn = (FARPROC)sgtKeyFunctions[eCmd].FKeyLift;
            break;
        case 0x101:
            sgpfnMouse4DownFn = (FARPROC)sgtKeyFunctions[eCmd].FKeyDown;
            sgpfnMouse4LiftFn = (FARPROC)sgtKeyFunctions[eCmd].FKeyLift;
            break;
        case 0x102:
            sgpfnMouse5DownFn = (FARPROC)sgtKeyFunctions[eCmd].FKeyDown;
            sgpfnMouse5LiftFn = (FARPROC)sgtKeyFunctions[eCmd].FKeyLift;
            break;
        case 0x103:
            sgpfnMouseWheelUpFn = (FARPROC)sgtKeyFunctions[eCmd].FKeyDown;
            break;
        case 0x104:
            sgpfnMouseWheelDownFn = (FARPROC)sgtKeyFunctions[eCmd].FKeyDown;
            break;
        default:
            break;
        }
    }
    return 0;
}

#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "CoreArchive.h"
#include "ui.h"

//  ui.cpp -- the UI core of D2Client.dll 1.10: the loading screen, the window-state core (UI_SetUIToggle and its readers), the registration of
//  the panels' input handlers, the gold-entry dialog, a run of small accessors, and the subsystem init and shutdown functions.
//  The original source file is UI\ui.cpp (the asserts of UI_DrawLoadingScreen, UI_OpenGoldEntryDialog and UI_CloseGoldEntryDialog name it);
//  it is split here across ui.cpp and ui_panels.cpp. Address range 0x6FB20000..0x6FB24840.

//  Two shared literals, each one object in the original: both sprintf calls push the first, and the language branch picks between it and the
//  second.
static const char szDataGlobal[] = "DATA\\GLOBAL";
static const char szDataLocal[] = "DATA\\LOCAL";

//  The allocation tag every SMemAlloc in this unit passes (one object in the original, shared with other callers there), and below it the
//  original's source path, as UI_DrawLoadingScreen's assert pushes it.
static const char szNew[] = "new";

static const char szUiCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UI\\ui.cpp";

//1.10f: D2Client.0x6FB241A0
//  Tears down the previous loading screen and puts up a new one. One caller, GAME_InitGame, which follows it with UI_DrawLoadingScreen(1).
//  The teardown branch is where per-act resources are swapped: the act palette through D2Win_10177 and D2CMP_10097, then eleven
//  cache-rebuilding calls, then the level's environment images and a 2000 ms extension of the catch-up deadline. All of it is skipped the
//  first time through, when sghLoadScreenProgress is still null.
//  The image comes from DATA\LOCAL\UI\LoadingScreen when STRTABLE_GetLanguage says localised, otherwise from DATA\GLOBAL\UI\Loading\Loadingscreen.
//  gnLoadScreenAct feeds D2Win_10177 as a byte (`mov cl, byte ptr gnLoadScreenAct`, 0x6FB241B8) and D2CMP_10097 as a dword (0x6FB241C9);
//  the cast below is the byte load.
//  UNRESOLVED_dword_6FBBA774 (0x6FBBA774) is written by this function, UI_DrawLoadingScreen and UI_UnloadLoadingScreen and read by nothing;
//  what it is is unknown. void: the caller reads nothing.

//  780 = 3 * 260, which is `sub esp, 30Ch` at 0x6FB241A6: three MAX_PATH buffers and no other local.
static_assert(3 * 260 == 0x30C,
              "UI_LoadScreenBegin's frame is three MAX_PATH buffers and nothing else");

void __cdecl UI_LoadScreenBegin()
{
    char pszName[260];
    char pszPalPL2Filename[260];
    char pszPalDatFilename[260];

    if (sghLoadScreenProgress)
    {
        ARCHIVE_CellFileFree(sghLoadScreenProgress);
        sghLoadScreenProgress = NULL;

        D2WIN_10177((uint8_t)gnLoadScreenAct);
        D2CMP_10097(gnLoadScreenAct);
        AUTOMAP_InitSymbolColors();
        INV_InitHighlightColors();
        PANEL_SetBoxFillColorRed();
        PANEL_SetBoxFillColorBlack();
        SKILLSTREE_CacheBlackIndex();
        PANEL_CacheStaminaColourRed();
        PANEL_CacheStaminaColourAmber();
        PANEL_CacheHighlightColours();
        PANEL_CacheStaminaColourBlue();
        ENV_LoadImages((D2C_Levels)gnPreloadedLevelId);
        GAME_ExtendCatchUpDeadline(2000);
    }

    //  the unnamed global: written here, never read
    UNRESOLVED_dword_6FBBA774 = 0;

    sghLoadScreenProgress = NULL;
    gbLoadingScreenActive = 0;

    sprintf(pszPalDatFilename, "%s\\Palette\\Loading\\pal.dat", szDataGlobal);
    sprintf(pszPalPL2Filename, "%s\\Palette\\Loading\\pal.pl2", szDataGlobal);
    D2WIN_10028(pszPalDatFilename, pszPalPL2Filename);

    if (D2LANG_STRTABLE_GetLanguage())
        sprintf(pszName, "%s\\UI\\LoadingScreen", szDataLocal);
    else
        sprintf(pszName, "%s\\UI\\Loading\\Loadingscreen", szDataGlobal);

    sghLoadScreenProgress = ARCHIVE_CellFileLoad(pszName, 0);
    gnLoadScreenFrame = 0;
    gbLoadingScreenActive = 1;
    MSG_SetLoadProgressRatio(0, 0);
    gbLoadScreenExitPending = gbGameExitPending;
}


//1.10f: D2Client.0x6FB242E0
//  Draws the loading screen, and on the first call builds it. Its assert names ui.cpp:6559. GAME_PreloadActData and GAME_InitGame put it up,
//  GFX_PreloadOnLoadComplete and GFX_PreloadVisibleUnits drive the bar as the load proceeds, and MSG_Send_D2CLTSYS_OPENCHAR uses it as a
//  clock. sghLoadScreenProgress is the handle of the loading screen's progress animation (the assert's condition reloads it), and
//  gnLoadScreenFrame is the animation's current frame.
//  D2GFX_WINDOW_GetState is called twice (0x6FB242EB and 0x6FB24308), with GAME_GetGameType and GAME_SendPingIfDue between the calls, so a
//  state change across the ping is visible to the second test: the screen can be cleared and then not drawn on.
//  The progress bar is only for network games -- types 6, 7, 8 and 9, OBNET_JOIN, LAN_JOIN, OBNET_HOST and LAN_HOST -- and only while
//  gbLoadScreenExitPending is clear. Its two colours are built once, and the fill colour's own value is the guard (`cmp gnLoadBarFillColour,
//  bl` with bl zero), so index 0 can never be the fill colour.
//  Geometry: frame (w/2 - 130, h/2 + 158) .. (w/2 + 130, h/2 + 178), 260 x 20; inner (w/2 - 128, h/2 + 160) .. (w/2 + 128, h/2 + 176),
//  256 x 16; fill from w/2 - 128, h/2 + 160, ratio wide, 16 tall. The fill's width is the progress ratio in pixels, which is why
//  MSG_GetLoadProgressRatio runs 0..256. `gnScreenWidth / 2` is a signed division (`cdq; sub eax, edx; sar eax, 1`).
//  The frame clamp is a signed compare (`cmp ecx, eax` / `jl` at 0x6FB24534), so gnLoadScreenFrame is an int.
//  BUG: only three of D2GfxDataStrc's eighteen dwords are written -- pCellFile, nFrame and nDirection (0x6FB24545, 0x6FB24565, 0x6FB24576) --
//  and the rest reach TEXTURE_CelDraw as whatever was on the stack. LATENT: a plain cel draw reads no further. Reproduced: the local is left
//  uninitialised.
//  The frame counter advances at the very end and outside the D2GFX_WINDOW_GetState guard: a minimised window still advances the animation.
//  void: all callers discard the result.
static_assert(sizeof(D2GfxDataStrc) == 0x48,
              "UI_DrawLoadingScreen's pData is [esp+60h] and the next local "
              "is [esp+A8h] -- 0x48 between them");
static_assert(offsetof(D2GfxDataStrc, pCellFile) == 0x04
              && offsetof(D2GfxDataStrc, nFrame) == 0x08
              && offsetof(D2GfxDataStrc, nDirection) == 0x0C,
              "the three members UI_DrawLoadingScreen writes are +4, +8 and "
              "+0Ch (0x6FB24545, 0x6FB24565, 0x6FB24576)");

void __fastcall UI_DrawLoadingScreen(int32_t nProgress)
{
    eD2GameTypes eGameType;
    int32_t nCelsPerDirection;
    int32_t nFrame;
    int32_t nProgressRatio;
    D2GfxDataStrc pData;
    char pszName[260];
    char pszPalPL2Filename[260];
    char pszPalDatFilename[260];

    if (!D2GFX_WINDOW_GetState())
        D2GFX_ClearScreen(0);

    eGameType = GAME_GetGameType();
    GAME_SendPingIfDue();

    if (!D2GFX_WINDOW_GetState())
    {
        D2WIN_10179();

        if (D2GFX_StartDraw(1, 0, 0, 0))
        {
            if (!sghLoadScreenProgress)
            {
                UNRESOLVED_dword_6FBBA774 = 0;
                sghLoadScreenProgress = NULL;
                gbLoadingScreenActive = 0;

                sprintf(pszPalDatFilename, "%s\\Palette\\Loading\\pal.dat", szDataGlobal);
                sprintf(pszPalPL2Filename, "%s\\Palette\\Loading\\pal.pl2", szDataGlobal);
                D2WIN_10028(pszPalDatFilename, pszPalPL2Filename);

                if (D2LANG_STRTABLE_GetLanguage())
                    sprintf(pszName, "%s\\UI\\LoadingScreen", szDataLocal);
                else
                    sprintf(pszName, "%s\\UI\\Loading\\Loadingscreen", szDataGlobal);

                sghLoadScreenProgress = ARCHIVE_CellFileLoad(pszName, 0);
                gnLoadScreenFrame = 0;
                gbLoadingScreenActive = 1;
                MSG_SetLoadProgressRatio(0, 0);
                gbLoadScreenExitPending = gbGameExitPending;

                if (!sghLoadScreenProgress)
                {
                    FOG_DisplayAssert((uint32_t)"sghLoadScreenProgress", (uint32_t)szUiCpp, 6559);
                    exit(-1);
                }
            }

            if (eGameType == GAMETYPE_OBNET_HOST || eGameType == GAMETYPE_LAN_HOST || eGameType == GAMETYPE_OBNET_JOIN || eGameType == GAMETYPE_LAN_JOIN)
            {
                if (!gnLoadBarFillColour)
                {
                    gnLoadBarFillColour = D2WIN_10034_MixRGB(0x60, 0, 0);
                    gnLoadBarFrameColour = D2WIN_10034_MixRGB(0xFF, 0xFF, 0);
                }

                if (!gbLoadScreenExitPending)
                {
                    D2GFX_DrawBox(gnScreenWidth / 2 - 130, gnScreenHeight / 2 + 158, gnScreenWidth / 2 + 130, gnScreenHeight / 2 + 178, gnLoadBarFrameColour, 1);
                    D2GFX_DrawBox(gnScreenWidth / 2 - 128, gnScreenHeight / 2 + 160, gnScreenWidth / 2 + 128, gnScreenHeight / 2 + 176, 0, 5);

                    nProgressRatio = MSG_GetLoadProgressRatio();
                    if (nProgressRatio)
                        GFX_DrawFilledRect(gnScreenWidth / 2 - 128, gnScreenHeight / 2 + 160, nProgressRatio, 16, gnLoadBarFillColour, 5);
                }
            }

            nCelsPerDirection = D2CMP_CelFileGetCelsPerDirection(sghLoadScreenProgress);
            nFrame = gnLoadScreenFrame;
            if (nFrame >= nCelsPerDirection)
            {
                nFrame = nCelsPerDirection - 1;
                gnLoadScreenFrame = nCelsPerDirection - 1;
            }

            pData.pCellFile = sghLoadScreenProgress;
            pData.nFrame = nFrame;
            pData.nDirection = 0;
            D2GFX_TEXTURE_CelDraw(&pData, gnScreenWidth / 2 - 128, gnScreenHeight / 2 + 128, 0xFFFFFFFF, 5, NULL);

            D2GFX_EndScene();
            GAME_ResetClickState();
            GAME_ResetMouseButtonState();
        }
    }

    if (nProgress)
        ++gnLoadScreenFrame;
}


//  Window-state core: UI_SetUIToggle is the only writer of gUiWindowStates, the 38-entry array that says which panel is open, and
//  UI_GetUiWindowState reads it with a bound. UI_TURNON = 0, UI_TURNOFF = 1, UI_TOGGLE = 2: zero is on.

static_assert(NUM_UI == 38,
              "UI_SetUIToggle asserts sgNumStateTbl == NUM_UI at ui.cpp:4988, "
              "and gaUiToggleRules ends at 0x6FB903A7 with sgNumStateTbl at "
              "0x6FB903A8 -- 152 bytes, 38 pointers");
static_assert(UI_TURNON == 0 && UI_TURNOFF == 1 && UI_TOGGLE == 2,
              "the bound at ui.cpp:5087 is `nUIState <= UI_TOGGLE`, so "
              "UI_TOGGLE must be the largest of the three");
static_assert(PANEL_NONE == 0 && PANEL_RIGHT == 1 && PANEL_LEFT == 2
              && PANEL_BOTH == 3,
              "SCROLL_SetWindowMode is called with 0..3 as literals at "
              "0x6FB235B1 onwards; Binary Ninja prints them as bare integers");


//1.10f: D2Client.0x6FB23230
//  Reads a window state, with the bound: an id out of range asserts and ends the process here rather than reading a neighbouring global.
int32_t __fastcall UI_GetUiWindowState(D2C_UIvars nUIId)
{
    if ((uint32_t)nUIId >= NUM_UI)
    {
        FOG_DisplayAssert((uint32_t)"nUIId < NUM_UI", (uint32_t)szUiCpp, 3097);
        exit(-1);
    }
    return gUiWindowStates[nUIId];
}


//1.10f: D2Client.0x6FB23860
//  The same read without the bound (`mov eax, gUiWindowStates[ecx*4]; retn`); the callers pass a literal. Kept distinct from the guarded
//  one because the original keeps them distinct.
int32_t __fastcall UI_GetUiWindowStateRaw(D2C_UIvars nUIId)
{
    return gUiWindowStates[nUIId];
}


//1.10f: D2Client.0x6FB23890
//  Cancels the panel-toggle debounce outright, both halves at once.
void __cdecl UI_ClearPanelToggleDelay()
{
    gbPanelToggleDelayEnabled = 0;
    gnPanelToggleDelayUntil = 0;
}


//1.10f: D2Client.0x6FB238A0
//  Arms the debounce mechanism, so that the next UI_ArmPanelToggleDelay sets a deadline. Enabling and arming are two separate calls in the
//  original.
void __cdecl UI_EnablePanelToggleDelay()
{
    gbPanelToggleDelayEnabled = 1;
}


//1.10f: D2Client.0x6FB238B0
//  Sets the deadline gnPanelToggleDelayUntil at now + 100 ms, if the mechanism is enabled. UI_SetUIToggle refuses a chatbox or escape-menu
//  toggle while it is still in the future, so this is a 100 ms debounce on the click that closed a panel.
//  The return is read (once, in MINIPANEL_RunButtonAction), which is why it is not void: it is the flag on the disabled path and the
//  deadline on the enabled one, whatever eax happens to hold.
uint32_t __cdecl UI_ArmPanelToggleDelay()
{
    uint32_t nResult;

    nResult = gbPanelToggleDelayEnabled;
    if (gbPanelToggleDelayEnabled)
    {
        nResult = GetTickCount() + 100;
        gnPanelToggleDelayUntil = nResult;
    }
    return nResult;
}


//1.10f: D2Client.0x6FB23260
//  Opens, closes or toggles a panel; the only writer of gUiWindowStates. In order: the request is vetted, every panel it conflicts with is
//  dealt with, the state is written, and the screen layout is moved to match.
//  gaUiToggleRules[nOpen][nRequested] is 38 x 38. For every id whose state is 1 the cell says what to do about it:
//      0   nothing, keep looking
//      1   close it -- UI_SetUIToggle(nOpen, UI_TURNOFF, 0), recursively: opening the inventory closes the skill tree through this
//      2   refuse the request outright
//      3   an error: FOG_Trace, and the toggle fails
//      4   end the NPC conversation first, then keep looking
//  Case 3 has no jump-table entry: `cmp eax, 4 / ja default`, and the entry for 3 points at the default arm, so the error path is reached
//  by 3 and by anything above 4. Rows 0, 19 and 35 (UI_GAME, UI_HIRICONS, UI_HIREICONS) are all zero: they lie past .data's raw size, where
//  the loader writes zeros.
//  The asserts at ui.cpp 5086, 5087 and 5067 are this function's own; `sgNumStateTbl == NUM_UI` at 4988 belongs to an inlined callee.
//  The switch on nUIId ends in five labels that several cases jump into, and `goto` is kept because the original's arms merge. The oddest is
//  `set_right`: the left-hand group reaches it when the panel is closing and UI_ANVIL when it is opening.
//  bCheckMousePos: clearing it skips the test of where the cursor is; setting it requires the cursor in the outer quarter on the relevant
//  side before the panel drags it across.
int32_t __fastcall UI_SetUIToggle(D2C_UIvars nUIId, D2C_UiStates nUIState, int32_t bCheckMousePos)
{
    D2UnitStrc *pPlayer;
    D2C_UIvars nOpenId;
    D2C_UIpanels nPanel;
    D2UnitGUID pNpcGUID;
    int32_t nStateBefore;
    int32_t nStateNow;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t bToggled;

    if ((uint32_t)nUIId >= NUM_UI)
    {
        FOG_DisplayAssert((uint32_t)"nUIId < NUM_UI", (uint32_t)szUiCpp, 5086);
        exit(-1);
    }
    if ((uint32_t)nUIState > UI_TOGGLE)
    {
        FOG_DisplayAssert((uint32_t)"nUIState <= UI_TOGGLE", (uint32_t)szUiCpp, 5087);
        exit(-1);
    }

    nStateBefore = gUiWindowStates[nUIId];

    //  The three expansion-only panels are silently refused in classic. FOG_Trace (variadic) and FOG_IsExpansion (no recorded calling
    //  convention) are called through D2Ptrs; D2Imports.h leaves them alone.
    if ((!FOG_IsExpansion() || !GAME_IsExpansionGame()) && (nUIId == UI_HIREICONS || nUIId == UI_MERCINV || nUIId == UI_RECIPESCROLL))
    {
        return 0;
    }

    //  ui.cpp:4988: the line number says this assert is an inlined callee's
    if (sgNumStateTbl != NUM_UI)
    {
        FOG_DisplayAssert((uint32_t)"sgNumStateTbl == NUM_UI", (uint32_t)szUiCpp, 4988);
        exit(-1);
    }

    //  Everything up to `apply` runs only when the request would open something: a turn-on, or a toggle of a panel that is currently off.
    if (nUIState == UI_TURNON || (nUIState == UI_TOGGLE && !gUiWindowStates[nUIId]))
    {
        if (nUIId == UI_CHATBOX || nUIId == UI_ESCMENU)
        {
            //  A dialog is up and the message log is closed, or the debounce has not expired.
            if ((DIALOG_GetNestDepth() && nUIId == UI_CHATBOX && !gUiWindowStates[UI_MSGLOG]) || gnPanelToggleDelayUntil > GetTickCount())
            {
                return 0;
            }

            if (nUIId == UI_ESCMENU)
            {
                pPlayer = CUNIT_GetPlayer();
                if (!pPlayer)
                    return 0;

                //  Esc on a corpse is the resurrect key, not the menu. dwAnimMode is one arm of the anonymous union at +0x10, shared with
                //  dwItemMode and dwMissileMode and selected by dwUnitType; 17 is the dead mode.
                if (pPlayer->dwUnitType == UNIT_PLAYER && pPlayer->dwAnimMode == 17)
                {
                    CUNIT_SetUnitAlive(pPlayer);
                    MSG_SendCmd1(65);

                    //  On hardcore there is nothing to resurrect to: the client tells the server, drops both connection flags and starts
                    //  disconnecting.
                    if (WINMAIN_IsHardcoreGame())
                    {
                        MSG_Send_D2CLTSYS_ENDGAME();
                        gbInGame = 0;
                        gbConnectedToGame = 0;
                        WINMAIN_MarkCharacterDead();
                        WINMAIN_SetAppModeContinue(0);
                        GAME_SetDisconnecting();
                    }
                    return 0;
                }
            }
        }

        //  A popup blocks everything except five panels that are allowed over it. The list is the original's, in its order.
        if (TEXT_IsPopupActive() && nUIId != UI_AUTOMAP && nUIId != UI_HIRICONS && nUIId != UI_QUESTLOG && nUIId != UI_NEWSTATS && nUIId != UI_NEWSKILLS)
        {
            return 0;
        }

        //  the conflict walk
        for (nOpenId = UI_GAME; (int32_t)nOpenId < (int32_t)sgNumStateTbl; nOpenId = (D2C_UIvars)(nOpenId + 1))
        {
            if (gUiWindowStates[nOpenId] != 1)
                continue;

            switch (gaUiToggleRules[nOpenId][nUIId])
            {
            case 0:
                break;

            case 1:
                UI_SetUIToggle(nOpenId, UI_TURNOFF, 0);
                break;

            case 2:
                return 0;

            case 4:
                if (NPCMENU_GetInteractingNpc(&pNpcGUID))
                    NPCMENU_NpcEndInteraction(pNpcGUID);
                break;

            default:
                //  Case 3 lands here too -- its jump-table entry is the default arm. UI_GAME asks about itself only if the matrix is corrupt,
                //  so that one is an assert rather than a trace.
                if (nUIId == UI_GAME)
                {
                    FOG_DisplayAssert((uint32_t)"nUIId != UI_STATE_SHARE", (uint32_t)szUiCpp, 5067);
                    exit(-1);
                }
                FOG_Trace(" requested %d but %d says it's an error in ui.cpp\n", nUIId, nOpenId);
                return 0;
            }
        }
    }

    //  apply

    pPlayer = CUNIT_GetPlayer();
    if (nUIState == UI_TURNOFF)
    {
        gUiWindowStates[nUIId] = 0;
    }
    else if (nUIState == UI_TOGGLE)
    {
        //  `pPlayer->dwUnitType` non-zero means not a player: UNIT_PLAYER is 0. So a dead player, or a unit that is not the player at all,
        //  gets only the chatbox and nothing else.
        if (pPlayer && (pPlayer->dwUnitType != UNIT_PLAYER || CUNIT_IsUnitDead(pPlayer)))
        {
            if (nUIId == UI_CHATBOX)
            {
                gUiWindowStates[UI_CHATBOX] = gUiWindowStates[UI_CHATBOX] == 0;
            }
        }
        else
        {
            gUiWindowStates[nUIId] = gUiWindowStates[nUIId] == 0;
        }
    }
    else if (!pPlayer || (pPlayer->dwUnitType == UNIT_PLAYER && !CUNIT_IsUnitDead(pPlayer)))
    {
        gUiWindowStates[nUIId] = 1;
    }

    nStateNow = gUiWindowStates[nUIId];

    //  The message table is registered when the panel turns on and dropped when it turns off, and only on a change, which is what the two
    //  reads either side of the write are for.
    if (nStateBefore)
    {
        if (!nStateNow)
            UI_UnregisterPanelHandlers(nUIId);
    }
    else if (nStateNow)
    {
        UI_RegisterPanelHandlers(nUIId);
        if (nUIId == UI_CHATBOX)
            CHAT_ClearInputLine();
    }

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();

    switch (nUIId)
    {
    case UI_INVENTORY:
        INV_ResetItemDescState();
        goto right_hand_panel;

    //  the left-hand group
    case UI_STATSCREEN:
    case UI_QUESTSCREEN:
    case UI_INISCROLL:
    case UI_WAYPOINT:
    case UI_PARTYSCREEN:
    case UI_MERCINV:
    case UI_RECIPESCROLL:
        if (gUiWindowStates[UI_INVENTORY] || gUiWindowStates[UI_SKILLTREE])
        {
            nPanel = PANEL_BOTH;
            if (!nStateNow)
            {
    set_right:
                nPanel = PANEL_RIGHT;
            }
            SCROLL_SetWindowMode(nPanel);
            goto toggled;
        }
        if (nStateNow)
        {
            SCROLL_SetWindowMode(PANEL_LEFT);
            if (!bCheckMousePos || nMouseX >= gnScreenWidth - gnScreenWidth / 4)
            {
                goto toggled;
            }
            goto push_cursor_right;
        }
        SCROLL_SetWindowMode(PANEL_NONE);
        if (!bCheckMousePos || nMouseX <= gnScreenWidth / 2)
            goto toggled;
        goto push_cursor_left;

    //  the right-hand group
    case UI_SKILLTREE:
    right_hand_panel:
        if (gUiWindowStates[UI_STATSCREEN] || gUiWindowStates[UI_WAYPOINT] || gUiWindowStates[UI_QUESTSCREEN] || gUiWindowStates[UI_INISCROLL] || gUiWindowStates[UI_MERCINV] || gUiWindowStates[UI_RECIPESCROLL] || gUiWindowStates[UI_PARTYSCREEN])
        {
            SCROLL_SetWindowMode(nStateNow ? PANEL_BOTH : PANEL_LEFT);
            bToggled = 1;
        }
        else
        {
            if (nStateNow)
            {
                SCROLL_SetWindowMode(PANEL_RIGHT);
                if (bCheckMousePos && nMouseX > gnScreenWidth / 4)
                {
    push_cursor_left:
                    CURSOR_SetCursorPos(nMouseX - gnScreenWidth / 4, nMouseY);
                    return 1;
                }
            }
            else
            {
                SCROLL_SetWindowMode(PANEL_NONE);
                if (bCheckMousePos && nMouseX < gnScreenWidth / 2)
                {
    push_cursor_right:
                    CURSOR_SetCursorPos(nMouseX + gnScreenWidth / 4, nMouseY);
                    return 1;
                }
            }
    toggled:
            bToggled = 1;
        }
        break;

    //  the panels that take the whole screen or none of it
    case UI_NPCSHOP:
    case UI_STASH:
    case UI_CUBE:
    case UI_STEEGSTONE:
    case UI_GUILDVAULT:
    case UI_UNKNOWN29:
    case UI_UNKNOWN32:
        if (!nStateNow)
            goto clear_panels;
        SCROLL_SetWindowMode(PANEL_BOTH);
        return 1;

    case UI_ANVIL:
        //  Opening the anvil takes the right side, which is the same tail the left-hand group uses when it closes.
        if (nStateNow)
            goto set_right;
    clear_panels:
        SCROLL_SetWindowMode(PANEL_NONE);
        return 1;

    default:
        goto toggled;
    }

    return bToggled;
}


//  Panel-handler registration, four functions at 0x6FB226C0..0x6FB2318D, the other half of UI_SetUIToggle: that function decides whether a
//  panel opens, these decide what the mouse and the keyboard then reach.
//  SMSGHANDLER is twelve bytes -- { nKind, nMessageOrKey, pfnHandler } -- consumed by D2Win's WREGISTER_RegisterCommand. nKind is 0 for a
//  window message, 1 for a WM_COMMAND id, 2 for a key-up code and 3 for a key-down code. Twenty-eight panels have a table; UI_MSGLOG opens
//  a dialog instead.

static_assert(sizeof(D2WinCommandRegisterStrc) == 12,
              "UI_SetPanelHandlerKey addresses an entry as `lea ecx,[esi+esi*2]` "
              "then `[edx+ecx*4+4]` -- entry * 12. D2MOO's D2WinCommandRegisterStrc "
              "is the same three fields at the same twelve bytes");
static_assert(offsetof(D2WinCommandRegisterStrc, nMessageOrKey) == 4,
              "the key is the +4 in `[edx+ecx*4+4]`, and it is the only field "
              "UI_ApplyPanelKeyBindings ever writes");
static_assert(offsetof(D2PanelKeyBindStrc, pHandlers) == 0x20,
              "`mov edx, [edx+20h]` at 0x6FB22D52 loads the table out of the "
              "descriptor before the entry is indexed");
static_assert(sizeof(D2PanelKeyBindStrc) == 36,
              "eight int slots then the table pointer, and three of the six "
              "descriptors are laid out back to back at exactly 0x24: "
              "gStashPanelKeys 0xCFF50, gCubePanelKeys 0xCFF74, gBeltPanelKeys "
              "0xCFF98. The other three are not adjacent and say nothing");
static_assert(offsetof(D2PanelKeyBindStrc, nClearScreenPrimary) == 0x00
              && offsetof(D2PanelKeyBindStrc, nShowBeltPrimary) == 0x04
              && offsetof(D2PanelKeyBindStrc, nClearScreenSecondary) == 0x08
              && offsetof(D2PanelKeyBindStrc, nShowBeltSecondary) == 0x0C
              && offsetof(D2PanelKeyBindStrc, nClearMessagesPrimary) == 0x10
              && offsetof(D2PanelKeyBindStrc, nClearMessagesSecondary) == 0x14
              && offsetof(D2PanelKeyBindStrc, nSwapWeaponsPrimary) == 0x18
              && offsetof(D2PanelKeyBindStrc, nSwapWeaponsSecondary) == 0x1C,
              "the eight blocks read +04, +00, +10, +18, +0C, +08, +14, +1C in "
              "that order -- the run order is NOT the field order, and the "
              "listing below is only readable against these offsets");
static_assert(HOTKEY_SWITCHWEAP == 0x2C,
              "`mov edx, 2Ch` at 0x6FB22D03 is the eCmd of the one call the "
              "compiler left out of line");


//1.10f: D2Client.0x6FB22D10
//  Sets one entry of a panel's handler table from the player's bindings; the one caller is the block UI_ApplyPanelKeyBindings runs eight
//  times. A panel ships with Escape hard-coded in its static table; this makes it answer to whatever the player actually bound.
//  The bound key is used only if it is neither 0xFFFF nor above 0xDF (`cmp ax, 0FFFFh / jz` at 0x6FB22D36 and `cmp ax, 0DFh / jbe` at
//  0x6FB22D3C); otherwise the default stays.
//  BUG: the 0xFFFF test can never fire. CMDTBL_ControlsIsActionBound (0x6FAD4F00) and CMDTBL_ControlsFindBoundKey (0x6FAD4B60) scan the same
//  table for the record matching (eCmd, bPrimary) and return the same nVKCode, so once IsActionBound has answered true the key is not -1.
//  LATENT: reproduced because the original asks the question a second time; only 0xE0, the floor of the unassigned VK range, is a refusal
//  that can happen here.
//  bPrimary is 1 for the primary binding: with the stock bindings the two entries swap rather than one dying -- HOTKEY_CLEARSCREEN is bound to
//  Space as primary and nothing as secondary, so the primary entry becomes Space and the secondary falls back to Escape, and both keys dismiss.
void __fastcall UI_SetPanelHandlerKey(int32_t nEntry, D2HotkeyIndices eCmd, BOOL bPrimary, const D2PanelKeyBindStrc *pBind, uint16_t nDefaultKey)
{
    uint16_t nKey;

    if (nEntry == -1)
        return;

    if (!CMDTBL_ControlsIsActionBound(eCmd, bPrimary) || (nKey = (uint16_t)CMDTBL_ControlsFindBoundKey(eCmd, bPrimary)) == 0xFFFF || nKey >= 0xE0)
    {
        nKey = nDefaultKey;
    }

    pBind->pHandlers[nEntry].nMessageOrKey = nKey;
}


//1.10f: D2Client.0x6FB22AA0
//  Rewrites a panel's handler table from the player's key bindings. Six callers, all arms of UI_RegisterPanelHandlers: npc shop, quest
//  screen, trade, stash, cube and the belt rows, each with its own descriptor.
//  Eight blocks, each the work of UI_SetPanelHandlerKey with the -1 guard at every call site; the original inlines UI_SetPanelHandlerKey in
//  seven of them. The eighth block tests nEntry != -1 twice, once at the call site (`cmp ecx, -1 / jz` at 0x6FB22CF9) and once inside the
//  callee. The eight run in an order that is not the descriptor's:
//      +04 ShowBelt pri 0xC0   +00 ClearScreen pri 0x1B   +10 ClearMsgs pri 0x4E
//      FOG_IsExpansion -> +18 SwapWeapons pri 0x57
//      +0C ShowBelt sec 0xC0   +08 ClearScreen sec 0x1B   +14 ClearMsgs sec 0x4E
//      FOG_IsExpansion -> +1C SwapWeapons sec 0x57, the one left out of line
//  FOG_IsExpansion is called twice, at 0x6FB22BA2 and 0x6FB22CED, not once around both swap-weapon blocks.
//  This writes through pBind into the panel's static table, so a panel registered twice is rewritten twice -- harmlessly, because the
//  second pass computes the same answer from the same bindings.
void __fastcall UI_ApplyPanelKeyBindings(const D2PanelKeyBindStrc *pBind)
{
    if (pBind->nShowBeltPrimary != -1)
        UI_SetPanelHandlerKey(pBind->nShowBeltPrimary, HOTKEY_SHOWBELT, 1, pBind, 0xC0);
    if (pBind->nClearScreenPrimary != -1)
        UI_SetPanelHandlerKey(pBind->nClearScreenPrimary, HOTKEY_CLEARSCREEN, 1, pBind, 0x1B);
    if (pBind->nClearMessagesPrimary != -1)
        UI_SetPanelHandlerKey(pBind->nClearMessagesPrimary, HOTKEY_CLEARMESSAGES, 1, pBind, 0x4E);

    if (FOG_IsExpansion())
    {
        if (pBind->nSwapWeaponsPrimary != -1)
            UI_SetPanelHandlerKey(pBind->nSwapWeaponsPrimary, HOTKEY_SWITCHWEAP, 1, pBind, 0x57);
    }

    if (pBind->nShowBeltSecondary != -1)
        UI_SetPanelHandlerKey(pBind->nShowBeltSecondary, HOTKEY_SHOWBELT, 0, pBind, 0xC0);
    if (pBind->nClearScreenSecondary != -1)
        UI_SetPanelHandlerKey(pBind->nClearScreenSecondary, HOTKEY_CLEARSCREEN, 0, pBind, 0x1B);
    if (pBind->nClearMessagesSecondary != -1)
        UI_SetPanelHandlerKey(pBind->nClearMessagesSecondary, HOTKEY_CLEARMESSAGES, 0, pBind, 0x4E);

    if (FOG_IsExpansion())
    {
        if (pBind->nSwapWeaponsSecondary != -1)
            UI_SetPanelHandlerKey(pBind->nSwapWeaponsSecondary, HOTKEY_SWITCHWEAP, 0, pBind, 0x57);
    }
}


//1.10f: D2Client.0x6FB226C0
//  Hooks a panel's input handlers when it opens: a 38-case jump table on nUiId (`cmp ecx, 25h` then `jmp [table + ecx*4]`) that decides which
//  handler table belongs to which panel. Called from UI_SetUIToggle and mirrored arm for arm by UI_UnregisterPanelHandlers.
//  Nine of the 38 ids fall to the default and register nothing: 8, 13, 14, 18, 27, 28, 29, 30 and 32.
//  UI_CONFIG is the only arm that registers two tables: the thirteen-record panel table unconditionally, and the single wheel handler only
//  when the machine has a wheel or needs the Win95 workaround. The unregister arm takes them off in the opposite order.
//  Two arms end in a tail jump rather than a call: `jmp TEXT_OpenMessageLog` (0x6FB22976) leaves ecx holding this function's own nUiId, but
//  that callee takes no argument; `jmp MEMPART_Nullsub1` ends the chatbox arm the same way.
//  BUG: gbAutomapHandlersRegistered is set to 1 when the automap's handlers go on and cleared when they come off, and nothing reads it (it is
//  not exported either). LATENT: a flag no code reads cannot change behaviour. It lies past .data's raw size and is zero at load.
void __fastcall UI_RegisterPanelHandlers(D2C_UIvars nUiId)
{
    switch (nUiId)
    {
    case UI_GAME:
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gGameMessageHandlers, 4);
        break;

    case UI_INVENTORY:
        INV_ResetPanelState();
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gInvMessageHandlers, 4);
        break;

    case UI_STATSCREEN:
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gStatsMessageHandlers, 2);
        break;

    case UI_MINISKILL:
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gMiniskillMessageHandlers, 1);
        break;

    case UI_SKILLTREE:
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gSkilltreeMessageHandlers, 2);
        break;

    case UI_CHATBOX:
        CHAT_RegisterInputHandlers();
        CMDTBL_SetKeyHookMode(0, 1);
        //  Opening the chat box takes the automap's handlers off, and the unregister arm puts them back. Both test the same state slot.
        if (gUiWindowStates[UI_AUTOMAP])
        {
            WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gAutomapMessageHandlers, 5);
        }
        MEMPART_Nullsub1();
        break;

    case UI_NEWSTATS:
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gNewstatsMessageHandlers, 2);
        break;

    case UI_NEWSKILLS:
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gNewskillsMessageHandlers, 2);
        break;

    case UI_ESCMENU:
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gEscmenuMessageHandlers, 7);
        SELECT_SetSelectedUnit(NULL);
        break;

    case UI_AUTOMAP:
        //  The handlers handed to Storm are our tables. SEvtUnregisterHandler removes an entry only when the handler address matches, so a
        //  panel opened by one copy of the registrars and closed by another keeps the opener's handlers registered.
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gAutomapMessageHandlers, 5);
        gbAutomapHandlersRegistered = 1;
        break;

    case UI_CONFIG:
        CONFIG_SuspendKeyCommands(1);
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gConfigMessageHandlers, 13);
        if (GAME_HasNativeMouseWheel() || GAME_NeedsWin95WheelWorkaround())
        {
            WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gConfigWheelHandler, 1);
        }
        break;

    case UI_NPCSHOP:
        CMDTBL_SetKeyHookMode(2, 1);
        INV_NpcShopBuildTabs();
        UI_ApplyPanelKeyBindings(&gNpcshopPanelKeys);
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gNpcshopMessageHandlers, 8);
        break;

    case UI_QUESTSCREEN:
        UI_ApplyPanelKeyBindings(&gQuestscreenPanelKeys);
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gQuestscreenMessageHandlers, 7);
        break;

    case UI_INISCROLL:
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gIniscrollMessageHandlers, 2);
        break;

    case UI_QUESTLOG:
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gQuestlogMessageHandlers, 4);
        break;

    case UI_HIRICONS:
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gHiriconsMessageHandlers, 1);
        break;

    case UI_WAYPOINT:
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gWaypointMessageHandlers, 5);
        break;

    case UI_MINIPANEL:
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gMinipanelMessageHandlers, 2);
        break;

    case UI_PARTYSCREEN:
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gPartyMessageHandlers, 2);
        PARTY_BuildRowList();
        break;

    case UI_MPTRADE:
        CMDTBL_SetKeyHookMode(0, 1);
        TRADE_OpenWindow();
        UNRESOLVED_sub_6FAED210();
        UI_ApplyPanelKeyBindings(&gTradePanelKeys);
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gTradeMessageHandlers, 13);
        break;

    case UI_MSGLOG:
        //  a tail jump in the original; the callee takes no argument
        TEXT_OpenMessageLog();
        break;

    case UI_STASH:
        CMDTBL_SetKeyHookMode(2, 1);
        INV_StashLoadImages();
        UI_ApplyPanelKeyBindings(&gStashPanelKeys);
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gStashMessageHandlers, 8);
        break;

    case UI_CUBE:
        CMDTBL_SetKeyHookMode(2, 1);
        INV_CubeLoadPanelImage();
        UI_ApplyPanelKeyBindings(&gCubePanelKeys);
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gCubeMessageHandlers, 7);
        break;

    case UI_BELTROWS:
        UI_ApplyPanelKeyBindings(&gBeltPanelKeys);
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gBeltMessageHandlers, 2);
        break;

    case UI_HELPSCREEN:
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gHelpscreenMessageHandlers, 5);
        break;

    case UI_HELPBUTTON:
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gHelpbuttonMessageHandlers, 2);
        break;

    case UI_HIREICONS:
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gHireiconsMessageHandlers, 3);
        break;

    case UI_MERCINV:
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gMercinvMessageHandlers, 3);
        break;

    case UI_RECIPESCROLL:
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gRecipescrollMessageHandlers, 2);
        break;

    default:
        return;
    }
}


//1.10f: D2Client.0x6FB22D60
//  Drops a panel's input handlers when it closes: the mirror of UI_RegisterPanelHandlers, a jump table on the same 38 ids. Called from
//  UI_SetUIToggle and from CONFIG_OnMouseUp. Six of the ids fall to the default: 13, 27, 28, 29, 30 and 32, three fewer than the register
//  side, because closing does work for ids that opening does not.
//  The switch merges into two labels. Eight arms reach `debounce`, which re-arms the panel-toggle delay; two reach `hook_mode_normal`: the
//  stash arm only when the toggle delay is disabled, the cube arm unconditionally. One label, two conditions, the same shape as
//  UI_SetUIToggle's `set_right`, and `goto` is kept for the same reason.
//  Three arms end in a tail call to a function in another file: the skill tree clears its hover cells (SKILLSTREE_ClearCellState), the quest
//  log its panel state (QUESTLOG_ClearPanelState) and the message log closes its dialog (TEXT_CloseMessageLog).
//  UI_MPTRADE does its whole arm only when TRADE_TickTimeout answers false, and it re-arms the debounce inside that block rather than
//  falling into the shared label, so a trade that times out neither unregisters nor debounces.
//  UI_UNKNOWN18 registers nothing and unregisters nothing: its whole unregister arm is the debounce, and it has no register arm. MEMPART_Nullsub1
//  is a single `retn`; the chatbox and iniscroll arms call it and the compiler tail-jumped two of those. UNRESOLVED_sub_6FAED210, in the
//  trade arm, writes zero to a global that four functions write and none reads.
void __fastcall UI_UnregisterPanelHandlers(D2C_UIvars nUiId)
{
    switch (nUiId)
    {
    case UI_GAME:
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gGameMessageHandlers, 4);
        return;

    case UI_INVENTORY:
        UI_CloseGoldEntryDialog();
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gInvMessageHandlers, 4);
        goto debounce;

    case UI_STATSCREEN:
        UI_CloseGoldEntryDialog();
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gStatsMessageHandlers, 2);
        goto debounce;

    case UI_MINISKILL:
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gMiniskillMessageHandlers, 1);
        return;

    case UI_SKILLTREE:
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gSkilltreeMessageHandlers, 2);
        //  This arm debounces inline and then tail-calls, so it cannot use the shared label below.
        if (gbPanelToggleDelayEnabled)
            gnPanelToggleDelayUntil = GetTickCount() + 100;
        SKILLSTREE_ClearCellState();
        return;

    case UI_CHATBOX:
        CHAT_UnregisterHandlers();
        MEMPART_Nullsub1();
        //  Nine panels hold the key hook in its other mode. Only when none of them is open does closing the chat box put it back to normal.
        if (!gUiWindowStates[UI_NPCSHOP] && !gUiWindowStates[UI_MPTRADE] && !gUiWindowStates[UI_STASH] && !gUiWindowStates[UI_CUBE] && !gUiWindowStates[UI_STEEGSTONE] && !gUiWindowStates[UI_GUILDVAULT] && !gUiWindowStates[UI_UNKNOWN29]
            && !gUiWindowStates[UI_UNKNOWN30] && !gUiWindowStates[UI_UNKNOWN32])
        {
            CMDTBL_SetKeyHookMode(1, 1);
        }
        if (gUiWindowStates[UI_AUTOMAP])
        {
            WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gAutomapMessageHandlers, 5);
        }
        return;

    case UI_NEWSTATS:
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gNewstatsMessageHandlers, 2);
        return;

    case UI_NEWSKILLS:
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gNewskillsMessageHandlers, 2);
        return;

    case UI_NPCMENU:
    case UI_ANVIL:
        if (gbPanelToggleDelayEnabled)
            gnPanelToggleDelayUntil = GetTickCount() + 100;
        NPCMENU_EndInteractionByUiState();
        return;

    case UI_ESCMENU:
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gEscmenuMessageHandlers, 7);
        SELECT_ClearSelectedUnit();
        return;

    case UI_AUTOMAP:
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gAutomapMessageHandlers, 5);
        gbAutomapHandlersRegistered = 0;
        return;

    case UI_CONFIG:
        //  The wheel handler comes off first here and goes on last there.
        if (GAME_HasNativeMouseWheel() || GAME_NeedsWin95WheelWorkaround())
        {
            WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gConfigWheelHandler, 1);
        }
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gConfigMessageHandlers, 13);
        CONFIG_SuspendKeyCommands(0);
        return;

    case UI_NPCSHOP:
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gNpcshopMessageHandlers, 8);
        NPCMENU_EndInteractionByUiState();
        CMDTBL_SetKeyHookMode(1, 1);
        return;

    case UI_QUESTSCREEN:
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gQuestscreenMessageHandlers, 7);
        QUESTLOG_AcknowledgeAndClose();
        goto debounce;

    case UI_INISCROLL:
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gIniscrollMessageHandlers, 2);
        MEMPART_Nullsub1();
        goto debounce;

    case UI_QUESTLOG:
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gQuestlogMessageHandlers, 4);
        QUESTLOG_ClearPanelState();
        return;

    case UI_UNKNOWN18:
        //  No handlers either way: the whole arm is the debounce, and there is no register arm at all.
        goto debounce;

    case UI_HIRICONS:
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gHiriconsMessageHandlers, 1);
        return;

    case UI_WAYPOINT:
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gWaypointMessageHandlers, 5);
        WAYPOINT_SendCloseOnce();
        goto debounce;

    case UI_MINIPANEL:
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gMinipanelMessageHandlers, 2);
        return;

    case UI_PARTYSCREEN:
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gPartyMessageHandlers, 2);
        PARTY_FreeRowList();
    debounce:
        if (gbPanelToggleDelayEnabled)
            gnPanelToggleDelayUntil = GetTickCount() + 100;
        return;

    case UI_MPTRADE:
        if (!TRADE_TickTimeout())
        {
            SCROLL_SetWindowMode(PANEL_NONE);
            UI_CloseGoldEntryDialog();
            WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gTradeMessageHandlers, 13);
            if (gbPanelToggleDelayEnabled)
                gnPanelToggleDelayUntil = GetTickCount() + 100;
            TRADE_CloseWindow(0);
            CMDTBL_SetKeyHookMode(1, 1);
        }
        return;

    case UI_MSGLOG:
        TEXT_CloseMessageLog();
        return;

    case UI_STASH:
        UI_CloseGoldEntryDialog();
        SCROLL_SetWindowMode(PANEL_NONE);
        INV_StashOnPanelClose();
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gStashMessageHandlers, 8);
        //  The delay branch is inverted here: without it the arm jumps straight into the shared hook-mode label the cube arm owns.
        if (!gbPanelToggleDelayEnabled)
            goto hook_mode_normal;
        gnPanelToggleDelayUntil = GetTickCount() + 100;
        CMDTBL_SetKeyHookMode(1, 1);
        break;

    case UI_CUBE:
        SCROLL_SetWindowMode(PANEL_NONE);
        INV_CubeOnPanelClose();
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gCubeMessageHandlers, 7);
        if (gbPanelToggleDelayEnabled)
            gnPanelToggleDelayUntil = GetTickCount() + 100;
    hook_mode_normal:
        CMDTBL_SetKeyHookMode(1, 1);
        break;

    case UI_BELTROWS:
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gBeltMessageHandlers, 2);
        break;

    case UI_HELPSCREEN:
        HELP_CloseMiniPanelIfOpened();
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gHelpscreenMessageHandlers, 5);
        break;

    case UI_HELPBUTTON:
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gHelpbuttonMessageHandlers, 2);
        break;

    case UI_HIREICONS:
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gHireiconsMessageHandlers, 3);
        break;

    case UI_MERCINV:
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gMercinvMessageHandlers, 3);
        INV_ResetItemDescState();
        break;

    case UI_RECIPESCROLL:
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gRecipescrollMessageHandlers, 2);
        break;

    default:
        return;
    }
}


//  Gold-entry dialog: "How much Gold would you like to drop / offer / deposit / withdraw?", the number box under it, and the callbacks the
//  dialog framework drives. The dialog's controls are C++ objects reached through their vtables, and the original addresses a slot as a byte
//  offset from the vtable pointer. `__thiscall` is legal on a function pointer where it is not on a free function, so the shapes are spelled
//  out and the offsets stay visible.
typedef int32_t  (__thiscall *GoldCtrlGetFn)(uint32_t pThis);
typedef int32_t  (__thiscall *GoldCtrlGetArgFn)(uint32_t pThis, int32_t nArg);
typedef void (__thiscall *GoldCtrlSetFn)(uint32_t pThis, int32_t nArg);

#define GOLD_VSLOT(fn, obj, nByteOff) (*(fn *)((*(uint32_t *)(UINT_PTR)(obj)) + (nByteOff)))

static_assert(sizeof(D2SplittedTextStrc) == 8,
              "D2Win_10199 returns a list of { Unicode *wszLine; next; } and "
              "UI_OpenGoldEntryDialog frees both fields, at ui.cpp 6148 and "
              "6149 -- two fields and nothing larger");
static_assert(STAT_GOLD == 14 && STAT_GOLDBANK == 15,
              "the four arms push 0Eh for carried gold and 0Fh for banked; "
              "`push 0Fh` at 0x6FB23C2D is the withdraw arm's own");
static_assert(UI_CHATBOX == 5 && UI_TURNOFF == 1,
              "INV_ArmGoldDialogGuard's `mov ecx,5 / mov edx,1` CLOSES the chat "
              "box -- which is why it is no longer called INV_OpenPanel5");


//1.10f: D2Client.0x6FB247E0
//  Is the gold dialog up? The dialog object is the flag, because Close nulls it. Sole caller TRADE_DrawButtonHints, which hides its hints
//  while the dialog covers them. Not gbGoldDialogOpen, which is the re-entry guard the five button handlers test before opening one.
BOOL __cdecl UI_IsGoldDialogOpen()
{
    return gpGoldDialog != 0;
}


//1.10f: D2Client.0x6FB23A60
//  Closes the dialog and destroys everything it built; calling it twice is harmless because every pointer is nulled. The amount is read out
//  of the spinner first, before the controls die, which is what lets UI_GoldDialogOnOk close and then still send a value. Five objects are
//  destroyed through vtable slot 0 with the argument 1, the scalar-deleting destructor, which frees as well as destructs. The two arms at
//  the end are tail jumps to functions in other files. The mode > 4 arm below them is unreachable: gnGoldDialogMode's only writer caps at 4.
void __cdecl UI_CloseGoldEntryDialog()
{
    uint32_t nSpinner;

    if (gbGoldDialogKeyHookSuspended)
    {
        CMDTBL_SetKeyHookMode(1, 1);
        gbGoldDialogKeyHookSuspended = 0;
    }

    //  The original re-loads gpGoldDialogSpinner into this slot once per branch rather than keeping it live, so the reads are written out.
    nSpinner = (uint32_t)gpGoldDialogSpinner;
    if (gpGoldDialogSpinner)
    {
        gnGoldDialogAmount = GOLD_VSLOT(GoldCtrlGetFn, gpGoldDialogSpinner, 44)((uint32_t)gpGoldDialogSpinner);
        nSpinner = (uint32_t)gpGoldDialogSpinner;
    }
    if (gpGoldDialog)
    {
        DIALOG_Close(gpGoldDialog);
        nSpinner = (uint32_t)gpGoldDialogSpinner;
        gpGoldDialog = 0;
    }
    if (gpGoldDialogStepButton)
    {
        GOLD_VSLOT(GoldCtrlSetFn, gpGoldDialogStepButton, 0)((uint32_t)gpGoldDialogStepButton, 1);
        nSpinner = (uint32_t)gpGoldDialogSpinner;
        gpGoldDialogStepButton = 0;
    }
    if (nSpinner)
    {
        GOLD_VSLOT(GoldCtrlSetFn, nSpinner, 0)(nSpinner, 1);
        gpGoldDialogSpinner = 0;
    }
    if (gpGoldDialogOkButton)
    {
        GOLD_VSLOT(GoldCtrlSetFn, gpGoldDialogOkButton, 0)((uint32_t)gpGoldDialogOkButton, 1);
        gpGoldDialogOkButton = 0;
    }
    if (gpGoldDialogCancelButton)
    {
        GOLD_VSLOT(GoldCtrlSetFn, gpGoldDialogCancelButton, 0)((uint32_t)gpGoldDialogCancelButton, 1);
        gpGoldDialogCancelButton = 0;
    }

    if (gnGoldDialogMode)
    {
        if (gnGoldDialogMode > 0 && gnGoldDialogMode <= 4)
        {
            //  a tail jump in the original (0x6FB23B1C); the callee's eax is not a value, so neither is this function's
            INV_ClearGoldDialogGuard();
            return;
        }

        //  UNREACHABLE: the only writer of gnGoldDialogMode is UI_OpenGoldEntryDialog, whose dispatch rejects anything above 4. Kept because
        //  the original keeps it.
        FOG_Trace("not implemented dummy %s %d ", szUiCpp, 5830);
        return;
    }

    //  Mode 0 is reachable: gnGoldDialogMode is BSS and this runs at shutdown and from the panel arms before any dialog has opened.
    CHAR_MarkGoldDialogClosed();
}


//1.10f: D2Client.0x6FB23B40
//  Opens the dialog. One register argument picks the prompt and the stat the amount is capped against:
//      1  drop      strDropGoldHowMuch    STAT_GOLD      INV, CUBE
//      2  trade     strTradeGoldHowMuch   STAT_GOLD      TRADE
//      3  deposit   strBankGoldDeposit    STAT_GOLD      STASH
//      4  withdraw  strBankGoldWithdraw   STAT_GOLDBANK  STASH
//  It bails to the teardown when there is no player or the cursor is holding an item, so the dialog cannot open over a held item.
//  Four controls are built in order -- step button, spinner, OK, Cancel -- each `SMemAlloc(12, "new", -1, 0)` then a proxy constructor, each
//  added to the dialog. The prompt is split at 200 pixels by D2Win_10199 and the list is freed as it is walked: the line, then the node.
//  The dispatch is `cmp edi, 4 / ja default` over a five-entry jump table, so only modes 0..4 survive it. Three tests further down read the
//  same value back -- `cmp ebx, 5` (0x6FB23C9F, the sprintf arm with STAT_GOLDBANK + STAT_GOLD), `cmp ebx, 7` (0x6FB23F01) and `cmp ebx, 6`
//  (0x6FB23F06), the last two seeding the spinner -- and none can be true; only `cmp ebx, 3` can fire. Modes 5, 6 and 7 were planned and
//  never wired up, and the tests for them were left behind. Mode 0 cannot happen either, which takes CHAR_MarkGoldDialogOpen with it: its
//  five call sites load ecx with 1, 2, 3, 4 and 1. All of it is reproduced. LATENT: no input reaches any of it.
//  The original's frame carries an exception-state variable (`[esp+22Ch+var_4]` takes 0, -1, 1, -1, 2, -1, 3, -1 around the four
//  constructions) and an SEH record: MSVC's unwinding bookkeeping for the C++ objects in scope. Ours has neither. The 256-element
//  constructor loop is reproduced.
void __fastcall UI_OpenGoldEntryDialog(int32_t nGoldDialogMode)
{
    D2C_StringIndices nPromptStrIdx;
    D2C_ItemStats nStatId;
    int32_t nStatValue;
    int32_t pLines;
    D2SplittedTextStrc *pSplitText;
    D2SplittedTextStrc *pSplitNext;
    void *pThis;
    int32_t *pControl;
    uint32_t *pSpinner;

    if (!CUNIT_GetPlayer() || D2COMMON_INVENTORY_GetCursorItem(CUNIT_GetPlayer()->pInventory))
    {
        goto bail;
    }
    if (gpGoldDialog)
        return;

    if (!gbGoldDialogKeyHookSuspended)
    {
        CMDTBL_SetKeyHookMode(0, 1);
        gbGoldDialogKeyHookSuspended = 1;
    }
    gnGoldDialogAmount = 0;

    switch (nGoldDialogMode)
    {
    case 0:
        //  UNREACHABLE: no call site passes 0. This is the only caller of CHAR_MarkGoldDialogOpen, so that function is dead with it.
        CHAR_MarkGoldDialogOpen();
        nPromptStrIdx = STR_IDX_4033_strDropGoldHowMuch;
        nStatId = STAT_GOLD;
        break;
    case 1:
        nPromptStrIdx = STR_IDX_4033_strDropGoldHowMuch;
        INV_ArmGoldDialogGuard();
        nStatId = STAT_GOLD;
        break;
    case 2:
        nPromptStrIdx = STR_IDX_4046_strTradeGoldHowMuch;
        INV_ArmGoldDialogGuard();
        nStatId = STAT_GOLD;
        break;
    case 3:
        nPromptStrIdx = STR_IDX_4049_strBankGoldDeposit;
        INV_ArmGoldDialogGuard();
        nStatId = STAT_GOLD;
        break;
    case 4:
        nPromptStrIdx = STR_IDX_4050_strBankGoldWithdraw;
        INV_ArmGoldDialogGuard();
        nStatId = STAT_GOLDBANK;
        break;
    default:
    bail:
        UI_CloseGoldEntryDialog();
        return;
    }

    nStatValue = D2COMMON_STATLIST_UnitGetStatValue(CUNIT_GetPlayer(), nStatId, 0);
    gnGoldDialogMode = nGoldDialogMode;
    gpGoldDialog = DIALOG_CreateFromImage(215, 140, (DIALOGCALLBACK)UI_GoldDialogOnCancel, NULL, 0, 1);
    DIALOG_SetHighlight(gpGoldDialog, 1);
    D2WIN_10127_SetFont(D2FONT_FONT16);

    Unicode wszText[256];
    memset(wszText, 0, sizeof(wszText));

    if (nGoldDialogMode == 5)
    {
        //  UNREACHABLE -- the dispatch above rejected 5. It would have put the player's whole purse into the prompt; with the stock tables
        //  that changes nothing, because none of the four strings has a format specifier and sprintf would have produced what strcpy does.
        int32_t nTotalGold = D2COMMON_STATLIST_UnitGetStatValue(CUNIT_GetPlayer(), STAT_GOLD, 0) + D2COMMON_STATLIST_UnitGetStatValue(CUNIT_GetPlayer(), STAT_GOLDBANK, 0);
        Unicode::sprintf(256, wszText, D2LANG_GetStringFromTblIndex(nPromptStrIdx), nTotalGold);
    }
    else
    {
        Unicode::strcpy(wszText, D2LANG_GetStringFromTblIndex(nPromptStrIdx));
    }

    pSplitText = D2WIN_10199(wszText, &pLines, 200);
    if (pSplitText)
    {
        do
        {
            DIALOG_AddLine(gpGoldDialog, pSplitText->wszLine, 24, 0, 4, 1, NULL, 0);
            pSplitNext = pSplitText->pNextLine;
            FOG_Free((uint32_t)(UINT_PTR)pSplitText->wszLine, (uint32_t)(UINT_PTR)szUiCpp, 6148, 0);
            FOG_Free((uint32_t)(UINT_PTR)pSplitText, (uint32_t)(UINT_PTR)szUiCpp, 6149, 0);
            pSplitText = pSplitNext;
        }
        while (pSplitNext);
    }

    pThis = STORM_SMemAlloc(0xC, szNew, -1, 0);
    pControl = pThis ? SPELLSEL_UiSpinnerProxyConstruct((int32_t *)pThis, 0, 223, 219, 0, (void *)UI_GoldDialogStepAmount)
                     : NULL;
    gpGoldDialogStepButton = (int32_t)pControl;
    DIALOG_AddControl(gpGoldDialog, pControl, 0, 0);

    pThis = STORM_SMemAlloc(0xC, szNew, -1, 0);
    pSpinner = pThis ? EDITBOX_UiCtrlProxyConstructEditbox((uint32_t *)pThis, 0, 258, 228, 100, 1, nStatValue, 0xA, (void *)USEITEM_NpcMenuWaitDialogRefuseClose, 1)
                     : NULL;
    gpGoldDialogSpinner = (int32_t)pSpinner;
    DIALOG_AddControl(gpGoldDialog, pSpinner, 0, 0);
    if (gpGoldDialogSpinner)
    {
        GOLD_VSLOT(GoldCtrlSetFn, gpGoldDialogSpinner, 68)((uint32_t)gpGoldDialogSpinner, 1);
    }

    pThis = STORM_SMemAlloc(0xC, szNew, -1, 0);
    pControl = pThis ? CHAR_UiButtonProxyConstruct((int32_t *)pThis, 0, 250, 287, 0, (void *)UI_GoldDialogOnOk)
                     : NULL;
    gpGoldDialogOkButton = (uint32_t)pControl;
    DIALOG_AddControl(gpGoldDialog, pControl, 0, 0);
    //  BUG: the OK button is called through without a NULL test (`mov ecx, gpGoldDialogOkButton / mov edx,[ecx]` at 0x6FB23E88), though the
    //  branch two lines up, which has an explicit `else pControl = NULL`, can make it NULL; the spinner's call (0x6FB23E1C) is guarded.
    //  REACHABLE: on allocation failure of the 12-byte control.
    GOLD_VSLOT(GoldCtrlSetFn, gpGoldDialogOkButton, 48)((uint32_t)gpGoldDialogOkButton, 1);

    pThis = STORM_SMemAlloc(0xC, szNew, -1, 0);
    pControl = pThis ? CHAR_UiButtonProxyConstruct((int32_t *)pThis, 0, 355, 287, 1, (void *)UI_GoldDialogOnCancelButton)
                     : NULL;
    gpGoldDialogCancelButton = (uint32_t)pControl;
    DIALOG_AddControl(gpGoldDialog, pControl, 0, 0);

    if (nGoldDialogMode == 2)
    {
        TRADE_SendGreenCheck();
    }
    //  Only the 3 can fire; 7 and 6 are the abandoned modes.
    else if (nGoldDialogMode == 3 || nGoldDialogMode == 7 || nGoldDialogMode == 6)
    {
        if (gpGoldDialogSpinner)
        {
            GOLD_VSLOT(GoldCtrlSetFn, gpGoldDialogSpinner, 56)((uint32_t)gpGoldDialogSpinner, nStatValue);
        }
    }
}


//1.10f: D2Client.0x6FB23F60
//  The dialog's cancel path, passed to DIALOG_CreateFromImage rather than called: Close and nothing else, which makes it the cancel arm
//  (UI_GoldDialogOnOk opens with the same call and then sends something).
int32_t __cdecl UI_GoldDialogOnCancel()
{
    UI_CloseGoldEntryDialog();
    return 0;
}


//1.10f: D2Client.0x6FB23F70
//  A step button was pressed. The button reports which half was clicked and how much it is worth through two separate virtual calls, and
//  this does the arithmetic:
//      up    amount += step                 clamped to the balance
//      down  if (step <= amount) amount -= step;  else amount = 0
//  The subtraction is guarded and the addition is not, so the amount cannot go below zero and has no upper clamp beyond the balance test.
//  The balance is STAT_GOLDBANK when the mode is 4 and STAT_GOLD otherwise; unlike its siblings in UI_OpenGoldEntryDialog this test can
//  fire, because INV_StashOnLButtonUp passes 4.
int32_t __cdecl UI_GoldDialogStepAmount()
{
    int32_t nStepDirection;
    uint32_t nStepSize;
    uint32_t nGoldDialogAmount;
    D2C_ItemStats nStatId;
    uint32_t nBalance;

    nStepDirection = GOLD_VSLOT(GoldCtrlGetFn, gpGoldDialogStepButton, 44)((uint32_t)gpGoldDialogStepButton);
    nStepSize = (uint32_t)GOLD_VSLOT(GoldCtrlGetArgFn, gpGoldDialogStepButton, 48)((uint32_t)gpGoldDialogStepButton, 0);

    if (gpGoldDialogSpinner)
    {
        gnGoldDialogAmount = GOLD_VSLOT(GoldCtrlGetFn, gpGoldDialogSpinner, 44)((uint32_t)gpGoldDialogSpinner);
    }
    nGoldDialogAmount = (uint32_t)gnGoldDialogAmount;

    if (gnGoldDialogMode == 4)
        nStatId = STAT_GOLDBANK;
    else
        nStatId = STAT_GOLD;
    nBalance = (uint32_t)D2COMMON_STATLIST_UnitGetStatValue(CUNIT_GetPlayer(), nStatId, 0);

    if (nStepDirection == 1)
    {
        nGoldDialogAmount += nStepSize;
        if (nGoldDialogAmount > nBalance)
            nGoldDialogAmount = nBalance;
    }
    else if (nStepDirection == 2)
    {
        if (nStepSize <= nGoldDialogAmount)
        {
            //  The inner test is the original's; it cannot change the answer, because nStepSize <= 0 already implies the subtraction is safe.
            if (nGoldDialogAmount)
                nGoldDialogAmount -= nStepSize;
        }
        else
        {
            nGoldDialogAmount = 0;
        }
    }

    gnGoldDialogAmount = (int32_t)nGoldDialogAmount;
    if (gpGoldDialogSpinner)
    {
        GOLD_VSLOT(GoldCtrlSetFn, gpGoldDialogSpinner, 56)((uint32_t)gpGoldDialogSpinner, (int32_t)nGoldDialogAmount);
    }
    return 0;
}


//1.10f: D2Client.0x6FB24010
//  OK was pressed. Closes first, which is safe because Close reads the amount out of the spinner before destroying it, then sends what the
//  mode says:
//      0, 1   MSG_SendCmdUnit(80, the player's unit id, amount)
//      2      TRADE_SendGoldAmount(amount)
//      3      MSG_SendCmd3Words(79, 20, amount >> 16, amount)
//      4      MSG_SendCmd3Words(79, 19, amount >> 16, amount)
//  Modes 2, 3 and 4 also play sound 221. A zero amount sends nothing except in mode 2, which sends TRADE_SendGoldAmount(0): clearing the gold
//  you had offered is a real action where dropping zero gold is not. The two MSG_SendCmd3Words arms split the amount because command 79
//  carries three 16-bit fields, and the sub-id, 20 or 19, tells them apart on the server. The original reads the high half straight out
//  of memory (`mov ax, word ptr gnGoldDialogAmount+2`) rather than shifting.
int32_t __cdecl UI_GoldDialogOnOk()
{
    D2UnitStrc *pPlayer;
    int32_t dwUnitId;

    UI_CloseGoldEntryDialog();

    if (gnGoldDialogAmount)
    {
        switch (gnGoldDialogMode)
        {
        case 0:
        case 1:
            pPlayer = CUNIT_GetPlayer();
            dwUnitId = pPlayer ? (int32_t)pPlayer->dwUnitId : -1;
            MSG_SendCmdUnit(80, dwUnitId, gnGoldDialogAmount);
            return 1;
        case 2:
            TRADE_SendGoldAmount((uint32_t)gnGoldDialogAmount);
            goto play;
        case 3:
            MSG_SendCmd3Words(79, 20, (int16_t)(gnGoldDialogAmount >> 16), (int16_t)gnGoldDialogAmount);
            goto play;
        case 4:
            MSG_SendCmd3Words(79, 19, (int16_t)(gnGoldDialogAmount >> 16), (int16_t)gnGoldDialogAmount);
        play:
            SOUNDINST_PlaySound(SOUND_ITEM_GOLD, NULL, 0, 0, 0);
            break;
        default:
            return 1;
        }
    }
    else if (gnGoldDialogMode == 2)
    {
        TRADE_SendGoldAmount(0);
        return 1;
    }
    return 1;
}


//1.10f: D2Client.0x6FB240C0
//  The Cancel button, not the same thing as UI_GoldDialogOnCancel above: that one is the dialog's own cancel callback, this one is the
//  button's, passed to CHAR_UiButtonProxyConstruct at (355, 287). The only difference in the code is the return value.
int32_t __cdecl UI_GoldDialogOnCancelButton()
{
    UI_CloseGoldEntryDialog();
    return 1;
}


//  Small accessors and helpers: a global returned, an image lazily loaded, a table walked once. Most are six to ninety bytes.

//  A literal this half of the unit needs and the other half does not.
static const char szDiabloIIKey[] = "Diablo II";

static_assert(NUM_UI == 38,
              "UI_CloseAllPanels walks 0..37 and indexes gbUiWindowCloseable "
              "with the same counter -- two parallel arrays of 38, so a panel "
              "added to one has to be added to the other");
static_assert(offsetof(RECT, left) == 0 && offsetof(RECT, right) == 8,
              "UI_GetPlayAreaBand's caller centres at (left + right) >> 1, and "
              "only RECT puts the X pair at +0 and +8 in four ints");


//1.10f: D2Client.0x6FB200E0
//  The trade panel's clear-messages key: expires the chat, claims the message, and tail-jumps to SMsgBreakHandlerChain so that the callee's
//  `retn 4` returns for this one. The key in the static table is overwritten by UI_ApplyPanelKeyBindings from HOTKEY_CLEARMESSAGES.
void __stdcall UI_TradeOnClearMessagesKey(SMSGHANDLER_PARAMS *pMsg)
{
    TEXT_ExpireAllChatMessages();
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FB20100
//  Loads a UI image by name: builds "<DATA\GLOBAL>\UI\<name>" and hands it to ARCHIVE_CellFileLoad. The original assembles the prefix with
//  wsprintfA and appends the name with an inlined strcpy.
D2CellFileStrc *__fastcall UI_LoadImageFromUiDir(const char *szName)
{
    CHAR szPath[260];
    size_t nPrefixLen;
    CHAR *pDest;

    wsprintfA(szPath, "%s\\UI\\", szDataGlobal);
    nPrefixLen = STORM_SStrLen(szPath);
    pDest = &szPath[nPrefixLen];
    while (*szName) *pDest++ = *szName++;
    *pDest = 0;

    return ARCHIVE_CellFileLoad(szPath, 0);
}


//1.10f: D2Client.0x6FB203C0
//  Recomputes every cached palette index the UI holds: eight calls and nothing else. Sole caller GAME_PreloadActData. Every one of the eight
//  is a nearest-match against the active palette, so an act change invalidates all of them at once.
uint8_t __cdecl UI_CachePaletteIndices()
{
    INV_InitHighlightColors();
    PANEL_SetBoxFillColorRed();
    PANEL_SetBoxFillColorBlack();
    SKILLSTREE_CacheBlackIndex();
    PANEL_CacheStaminaColourRed();
    PANEL_CacheStaminaColourAmber();
    PANEL_CacheHighlightColours();
    return (uint8_t)PANEL_CacheStaminaColourBlue();
}


//1.10f: D2Client.0x6FB203F0
//  Upper-cases a Unicode string in place; null-safe, sole caller SKILLDESC_DrawSkillInfo. Only code points below 0x100 are mapped; anything
//  above is copied through untouched, because Unicode::_toUpperTable is 256 entries of the Latin-1 range. The table is a data import
//  reached with one dereference of its slot.
void __fastcall UI_TextToUpperInPlace(struct Unicode *pText)
{
    int32_t nLen;
    int32_t i;
    uint16_t ch;

    if (!pText)
        return;

    nLen = Unicode::strlen(pText);
    for (i = 0; i < nLen; ++i)
    {
        ch = pText[i].ch;
        if (ch < 0x100u)
            ch = UnicodeTables::toUpperTable()[pText[i].ch];
        pText[i].ch = ch;
    }
}


//1.10f: D2Client.0x6FB23800
//  Closes everything. The answer is read: two callers follow it with `test eax, eax` and two more reach it by tail jump, so the return
//  becomes theirs. It walks all 38 window ids and closes each one that is open and marked closeable in the parallel table. The automap is
//  closed only when bIncludeAutomap is set (it is not in the closeable table, so it survives an ordinary close-all); the stash and the
//  inventory also call INV_ResetItemDescState, because those two can leave a hovered item behind.
int32_t __fastcall UI_CloseAllPanels(int32_t bIncludeAutomap, int32_t nToggleArg)
{
    int32_t bClosedAny;
    D2C_UIvars i;

    bClosedAny = 0;
    for (i = UI_GAME; i < NUM_UI; i = (D2C_UIvars)(i + 1))
    {
        if (gUiWindowStates[i] == 1 && (gbUiWindowCloseable[i] == 1 || (bIncludeAutomap && i == UI_AUTOMAP)))
        {
            UI_SetUIToggle(i, UI_TURNOFF, nToggleArg);
            bClosedAny = 1;
            if (i == UI_STASH || i == UI_INVENTORY)
                INV_ResetItemDescState();
        }
    }
    return bClosedAny;
}


//1.10f: D2Client.0x6FB23870
//  The spinning pentagrams either side of the highlighted escape-menu line. Callers ESCMENU_Draw and ESCMENU_LoadImages.
D2CellFileStrc *__cdecl UI_GetPentSpinImage()
{
    return gpImgPentSpin;
}


//1.10f: D2Client.0x6FB23880
//  Sole caller DIALOG_Draw.
D2CellFileStrc *__cdecl UI_GetFocusImage()
{
    return gpImgFocus16;
}


//1.10f: D2Client.0x6FB238D0
//  Posts a synthetic WM_MOUSEMOVE, the cursor packed into lParam as `(y << 16) | x`. This is how the client makes the UI re-evaluate hover
//  state without the mouse having moved, after a panel opens or closes for instance.
LRESULT __cdecl UI_PostSyntheticMouseMove()
{
    LPARAM lParam;

    lParam = (LPARAM)(((uint16_t)CURSOR_GetMouseY() << 16) | (uint16_t)CURSOR_GetMouseX());
    return SendMessageA(D2GFX_WINDOW_GetWindow(), 0x200u, 0, lParam);
}


//1.10f: D2Client.0x6FB23910
//  Brings the per-player UI up. Sole caller CUNIT_SetPlayerUnit; the positional partner of UI_ResetUiSubsystems (not UI_InitUiSubsystems,
//  which is game start and pairs with UI_ShutdownUiSubsystems). Eleven subsystems are brought up in order, then `Always Run` is read out of
//  the registry and handed to the run toggle by MINIPANEL_LoadAlwaysRunFromRegistry, which the original reaches by a tail `jmp` at 0x6FB2394C.
void __cdecl UI_InitUiForPlayerUnit()
{
    PARTY_LoadImages();
    CHAR_CharPanelMeasureLabelWidths();
    SKILLSTREE_LoadChartImage();
    SPELLSEL_Init();
    TRADE_ResetState();
    INV_ResetItemDescState();
    PANEL_BeltClearHoverItem();
    INV_CubeResetQuestItemAnim();
    QUESTLOG_ClearActSelection();
    HIREABLES_HireIconsResetAndShow();
    AUTOMAP_RecomputeCenterOffsets(1);
    MINIPANEL_LoadAlwaysRunFromRegistry();
}


//1.10f: D2Client.0x6FB23960
//  Takes the per-player UI away: the middle member of the lifecycle trio, called when the local player unit is destroyed. It finishes by
//  dropping whatever is on the cursor: a held item must not outlive the unit that was holding it. MEMPART_Nullsub1 is one byte, `retn`; it
//  pops nothing, which `void __cdecl()` says.
int32_t __cdecl UI_ResetUiSubsystems()
{
    PARTY_UnloadImages();
    MEMPART_Nullsub1();
    SKILLSTREE_UnloadChartImage();
    SPELLSEL_Shutdown();
    QUESTLOG_ClearEntryState();
    NPCMENU_Reset();
    TRADE_CloseOnShutdown();
    TEXT_FreeChatAndMessageLogLists();
    HIREABLES_PanelInvalidateLifeBarColors();

    if (gpImgLevelEntryTitle)
    {
        ARCHIVE_CellFileFree(gpImgLevelEntryTitle);
        gpImgLevelEntryTitle = NULL;
    }
    if (gpImgYouDiedHardcore)
    {
        ARCHIVE_CellFileFree(gpImgYouDiedHardcore);
        gpImgYouDiedHardcore = NULL;
    }
    if (gpImgYouDiedInst)
    {
        ARCHIVE_CellFileFree(gpImgYouDiedInst);
        gpImgYouDiedInst = NULL;
    }

    MINIPANEL_SaveAlwaysRunToRegistry();
    return CURSOR_SetHeldUnit(NULL);
}


//1.10f: D2Client.0x6FB239F0
//  A level's display name: levels.txt wszLevelName normally, and tbl strLevelLoadFailed when the record is missing. Level 8 is special-cased
//  in code: it asks the quest state and answers either Den of Evil or Cave Level 1, so the first cave keeps its generic name until the quest
//  is known (levels.txt cannot express it).
int32_t __fastcall UI_LevelsGetLevelName(int32_t nLevelId)
{
    D2LevelsTxt *pRecord;

    pRecord = D2COMMON_DATATBLS_GetLevelsTxtRecord(nLevelId);
    if (!pRecord)
        return (int32_t)D2LANG_GetStringFromTblIndex(STR_IDX_5389_strLevelLoadFailed);
    if (nLevelId != 8)
        return (int32_t)pRecord->wszLevelName;
    if (QUESTS_GetQuestState(0, QUEST_A1Q1_DENOFEVIL))
        return (int32_t)D2LANG_GetStringFromTblIndex(STR_IDX_5048_Den_of_Evil);
    return (int32_t)D2LANG_GetStringFromTblIndex(STR_IDX_5047_Cave_Level_1);
}


//1.10f: D2Client.0x6FB23A40
//  Uses the item on the cursor: client packet 0x27 with the item's unit id passed as both parameters, the "use this, no separate target"
//  case. void: eax at the exit is the held unit pointer (NULL) on the empty path and MSG_SendCmdUnit's answer on the sending one, and no
//  call site reads either.
void __cdecl UI_CursorSendUseHeldItem()
{
    D2UnitStrc *pHeld;

    pHeld = CURSOR_GetHeldUnit();
    if (pHeld)
        MSG_SendCmdUnit(39, (int32_t)pHeld->dwUnitId, (int32_t)pHeld->dwUnitId);
}


//1.10f: D2Client.0x6FB240D0
//  Gives the automap its input back, or takes it away: opening a dialog takes the automap's arrow keys away and closing it gives them back.
//  Callers DIALOG_Create, DIALOG_CreateFromImage and DIALOG_Close. Nothing happens either way unless the automap is actually open.
void __fastcall UI_AutomapSetInputEnabled(D2UnkOutdoorStrc2 *bEnable)
{
    if (bEnable)
    {
        if (gUiWindowStates[UI_AUTOMAP])
        {
            WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gAutomapMessageHandlers, 5);
        }
    }
    else if (gUiWindowStates[UI_AUTOMAP])
    {
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gAutomapMessageHandlers, 5);
    }
}


//1.10f: D2Client.0x6FB24110
//  The buy/sell button art, loaded once: after the first call it is one compare and a return. Nothing ever unloads it (the image lives as
//  long as the client does), so the cel survives a return to the main menu.
D2CellFileStrc *__cdecl UI_GetBuySellButtonImage()
{
    CHAR szName[260];

    if (!gpImgBuySellButton)
    {
        wsprintfA(szName, "%s\\ui\\panel\\buysellbtn", szDataGlobal);
        gpImgBuySellButton = ARCHIVE_CellFileLoad(szName, 0);
    }
    return gpImgBuySellButton;
}


//1.10f: D2Client.0x6FB24150
//  The loading screen's own palette: two paths under DATA\GLOBAL, both handed to D2Win ordinal 10028. Its three callers are the disc-prompt
//  screens, which run before the normal UI palette exists, which is why they load their own.
int32_t __cdecl UI_LoadScreenLoadPalette()
{
    char szPalDat[260];
    char szPalPl2[260];

    sprintf(szPalDat, "%s\\Palette\\Loading\\pal.dat", szDataGlobal);
    sprintf(szPalPl2, "%s\\Palette\\Loading\\pal.pl2", szDataGlobal);
    return D2WIN_10028(szPalDat, szPalPl2);
}


//1.10f: D2Client.0x6FB245C0
//  Tears the loading screen down: the image goes, then eleven subsystem resets, then the act's environment art and the catch-up deadline.
//  The three globals at the end are cleared whether or not there was an image; that is the only part outside the guard. void: both callers
//  discard the register.
void __cdecl UI_UnloadLoadingScreen()
{
    if (sghLoadScreenProgress)
    {
        ARCHIVE_CellFileFree(sghLoadScreenProgress);
        sghLoadScreenProgress = NULL;
        D2WIN_10177(gnLoadScreenAct);
        D2CMP_10097(gnLoadScreenAct);
        AUTOMAP_InitSymbolColors();
        INV_InitHighlightColors();
        PANEL_SetBoxFillColorRed();
        PANEL_SetBoxFillColorBlack();
        SKILLSTREE_CacheBlackIndex();
        PANEL_CacheStaminaColourRed();
        PANEL_CacheStaminaColourAmber();
        PANEL_CacheHighlightColours();
        PANEL_CacheStaminaColourBlue();
        ENV_LoadImages((D2C_Levels)gnPreloadedLevelId);
        GAME_ExtendCatchUpDeadline(2000);
    }
    UNRESOLVED_dword_6FBBA774 = 0;
    sghLoadScreenProgress = NULL;
    gbLoadingScreenActive = 0;
}


//1.10f: D2Client.0x6FB24650
//  Sole caller GFX_PreloadUnit, which uses it to decide whether it is allowed to block on a load.
int32_t __cdecl UI_IsLoadingScreenActive()
{
    return gbLoadingScreenActive;
}


//1.10f: D2Client.0x6FB24660
//  Yes at most once every 50 milliseconds; sole caller ESCMENU_Draw. `GetTickCount() - stamp <= 0x32` is an unsigned compare on the
//  difference, so the 49.7-day tick wrap costs one skipped tick rather than jamming the throttle for weeks, which the naive
//  `stamp + 50 < now` would have done.
int32_t __cdecl UI_ThrottleTick50ms()
{
    uint32_t dwNow;

    dwNow = GetTickCount();
    if (dwNow - gdwUiThrottleTick <= 0x32)
        return 0;
    gdwUiThrottleTick = dwNow;
    return 1;
}


//1.10f: D2Client.0x6FB24690
//  The horizontal strip no side panel is covering; FALSE when there is none left. Callers UIPANELS_DrawTextCenteredInPanelRect and
//  UIPANELS_DrawUnitNameLabel, both of which need somewhere legal to put text. Six window ids make it give up outright; seven more push the
//  left edge to the midline; the inventory or the skill tree pull the right edge in to the same midline. The answer is `left != right`.
//  The ids are read straight out of gUiWindowStates rather than through UI_GetUiWindowState; 27 through 30 and 32 have enum names but no
//  panel behind them.
BOOL __fastcall UI_GetPlayAreaBand(RECT *pBand)
{
    pBand->left = 0;
    pBand->right = gnScreenWidth;
    pBand->top = 0;
    //  BUG: bottom gets gnScreenWidth, not the height. LATENT: only left and right are read again, here or in either caller.
    pBand->bottom = gnScreenWidth;

    if (gUiWindowStates[UI_MPTRADE] || gUiWindowStates[UI_STASH] || gUiWindowStates[UI_CUBE] || gUiWindowStates[UI_NPCSHOP] || gUiWindowStates[UI_GUILDVAULT] || gUiWindowStates[UI_STEEGSTONE])
    {
        return 0;
    }

    if (gUiWindowStates[UI_UNKNOWN29] || gUiWindowStates[UI_PARTYSCREEN] || gUiWindowStates[UI_STATSCREEN] || gUiWindowStates[UI_QUESTSCREEN] || gUiWindowStates[UI_MERCINV] || gUiWindowStates[UI_RECIPESCROLL] || gUiWindowStates[UI_WAYPOINT])
    {
        pBand->left = gnScreenWidth / 2;
    }
    if (gUiWindowStates[UI_INVENTORY] || gUiWindowStates[UI_SKILLTREE])
        pBand->right = gnScreenWidth / 2;

    return pBand->left != pBand->right;
}


//1.10f: D2Client.0x6FB24780
//  `gnScreenHeight - 49`: the y below which the screen belongs to the bottom control panel rather than to the world. One caller,
//  PLAYER_SendClickCommand, which uses it to ignore clicks on the panel. Named for the 49 rather than the arithmetic: the constant is the
//  panel's height and it is in no data file.
int32_t __cdecl UI_GetPlayAreaBottom()
{
    return gnScreenHeight - 49;
}


//1.10f: D2Client.0x6FB24790
//  Puts the configure-controls panel's input back; called by every capture handler once a binding is taken or cancelled. The same two
//  tables UI_RegisterPanelHandlers' UI_CONFIG arm registers, in the same order.
void __cdecl UI_ConfigRegisterPanelHandlers()
{
    WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gConfigMessageHandlers, 13);
    if (GAME_HasNativeMouseWheel() || GAME_NeedsWin95WheelWorkaround())
    {
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gConfigWheelHandler, 1);
    }
}


//1.10f: D2Client.0x6FB247D0
//  Closes the mercenary hire list: the control is released through its own vtable slot 0 with a 1, the scalar-deleting destructor, and both
//  handles are nulled. The parameter is declared and never read. void: the bytes are `call TEXT_MsgLogReopen` (0x6FB247D0) and
//  `jmp NPCMENU_TeardownHireList` (0x6FB247D5), so eax on the way out is the teardown's, and the one xref is the tail jump of
//  WNDPROC_SetResolutionMode, which is void. The tail jump is a call here.
void __fastcall UI_NpcMenuCloseHireListDialog(int32_t a1)
{
    TEXT_MsgLogReopen();
    NPCMENU_TeardownHireList();             //  `jmp NPCMENU_TeardownHireList` (0x6FB247D5)
}


//  DATATBLS_GetMonStats2TxtRecord (0x6FB247F0) is written in Shared.cpp, because its inlined copies stand in seven modules.


//  The registry value UI_InitUiSubsystems consults before deciding whether the mini panel starts open.
static const char szMiniPanel[] = "Mini Panel";

static_assert(1024 * sizeof(Unicode) == 2048,
              "UI_AddColorCodeToWstring frames a 2048-byte scratch buffer at "
              "[ebp-800h], which is 1024 Unicodes -- and the constructor loop "
              "runs 1023 of them because the first is stored before it");
static_assert(STR_IDX_3994_colorcode == 3994,
              "the tbl index UI_AddColorCodeToWstring fetches, read off the "
              "`mov ecx, <n>` before its D2LANG_GetStringFromTblIndex");


//1.10f: D2Client.0x6FB245B0
//  `gnLoadScreenAct = nAct;`. Sole caller GAME_PreloadActData, and the loading screen picks its artwork from this.
void __fastcall UI_SetLoadScreenAct(int32_t nAct)
{
    gnLoadScreenAct = nAct;
}


//1.10f: D2Client.0x6FB20430
//  Puts a colour code in front of a string, in place: builds "<tbl 3994 colorcode><digit><the caller's text>" in a 1024-Unicode scratch
//  buffer and copies it back over the caller's own. The caller's string is the second operand of the second strcat (the bytes at 0x6FB204D3
//  and 0x6FB204DD say which way round), otherwise the colour code would land at the end and colour nothing. Two constructor loops come
//  before either concatenation -- 1023 Unicodes for the scratch buffer and 3 for the digit -- and the buffer is memset to zero as well.
//  Does nothing to an empty string, which is checked before anything is built.
wchar_t *__fastcall UI_AddColorCodeToWstring(wchar_t *wszText, uint32_t nColor)
{
    CHAR szDigit[4];

    if (!*wszText)
        return wszText;

    //  1024: the loop at 0x6FB20463 runs the closure 3FFh times from &wszScratch[1] and the store at 0x6FB20454 does element 0 on its own,
    //  so the array is 1023 + 1. The declaration constructs all 1024 where the original constructs 1023: one extra call into D2Lang,
    //  writing the same zero the line below writes again.
    struct Unicode wszScratch[1024];

    wszScratch[0].ch = 0;
    memset(&wszScratch[1], 0, sizeof(wszScratch) - sizeof(wszScratch[0]));

    Unicode::strcpy(wszScratch, D2LANG_GetStringFromTblIndex(STR_IDX_3994_colorcode));

    //  three, not four: 0x6FB20490 loads edi with 3 and the loop runs from wszDigit[0], so every element is constructed. Declared here rather
    //  than at the top, because the original constructs it after the strcpy above.
    struct Unicode wszDigit[3];

    wsprintfA(szDigit, "%c", (char)(nColor + 48));
    Unicode::win2Unicode(wszDigit, szDigit, 2);

    Unicode::strcat(wszScratch, wszDigit);
    Unicode::strcat(wszScratch, (const struct Unicode *)wszText);
    Unicode::strcpy((struct Unicode *)wszText, wszScratch);

    return wszText;
}


//1.10f: D2Client.0x6FB20160
//  Brings every client UI subsystem up. Sole caller SCMD_HandleSrv01_GameFlags, the handler for the server's game-flags packet.
//  UI_ShutdownUiSubsystems runs the same subsystems in the same order, one down.
//  It has no `retn` of its own: the body ends `pop edi; pop esi; add esp, 108h; jmp CONFIG_InitControlsScreen`, a tail jump, written as a
//  plain call here. The mini panel is the one thing that is not unconditional: it is toggled open unless the registry says otherwise, and
//  a missing value opens it too (`!SRegLoadValue(...) || !pValue`).
void __cdecl UI_InitUiSubsystems()
{
    uint32_t nMiniPanel;
    CHAR szName[260];

    PANEL_LoadImages();
    MINIPANEL_LoadImages();
    TEXT_Init();
    NEWSTATS_LoadImages();
    HIREABLES_PanelLoadHirelingIcons();
    NPCMENU_Init();
    AUTOMAP_LoadSettings();
    WAYPOINT_AllocPanelData();
    TRADE_ResetState();

    gpGoldDialog = 0;
    gpGoldDialogStepButton = 0;
    gpGoldDialogSpinner = 0;
    gpGoldDialogCancelButton = 0;
    gpGoldDialogOkButton = 0;
    gbGoldDialogKeyHookSuspended = 0;

    INV_LoadPanelImages();
    memset(gUiWindowStates, 0, NUM_UI * sizeof(int32_t));   // `mov ecx, 26h` / `rep stosd` (0x6FB201C0): the array's 38 ints exactly

    UI_SetUIToggle(UI_GAME, UI_TURNON, 0);
    //  A missing registry value opens the mini panel, the same as a zero one.
    if (!STORM_SRegLoadValue(szDiabloIIKey, szMiniPanel, 1u, &nMiniPanel) || !nMiniPanel)
        UI_SetUIToggle(UI_MINIPANEL, UI_TOGGLE, 0);
    UI_SetUIToggle(UI_HELPBUTTON, UI_TURNON, 0);

    wsprintfA(szName, "%s\\UI\\CURSOR\\Pentspin", szDataGlobal);
    gpImgPentSpin = ARCHIVE_CellFileLoad(szName, 0);
    wsprintfA(szName, "%s\\UI\\CURSOR\\focus16", szDataGlobal);
    gpImgFocus16 = ARCHIVE_CellFileLoad(szName, 0);
    wsprintfA(szName, "%s\\UI\\menu\\boxpieces", szDataGlobal);
    gpImgUiBoxPieces = ARCHIVE_CellFileLoad(szName, 0);

    USEITEM_AnvilInit();
    DIALOG_Init();
    INV_NpcShopLoadTabImages();
    LISTBOX_LoadScrollBarImage();
    QUESTLOG_QuestResetAllState();
    CMDTBL_LoadDefaultBindings();
    ESCMENU_LoadImages();

    //  a tail jump in the original
    CONFIG_InitControlsScreen();
}


//1.10f: D2Client.0x6FB202C0
//  Brings every UI subsystem down. Sole caller GAME_AppModeGame, on the way out of a game; subsystem for subsystem the mirror of
//  UI_InitUiSubsystems. Only two things here are conditional: the skill tree and the spell selector come down only if there is still a
//  player unit, and each of the six images is released only if it was loaded.
void __cdecl UI_ShutdownUiSubsystems()
{
    UI_SetUIToggle(UI_GAME, UI_TURNOFF, 0);
    PANEL_UnloadImages();

    if (CUNIT_GetPlayer())
    {
        SPELLSEL_Shutdown();
        SKILLSTREE_UnloadChartImage();
    }

    D2WIN_10116();
    TEXT_Shutdown();
    MINIPANEL_SaveAndUnload();
    HELP_UnloadImages();
    NEWSTATS_UnloadImages();
    HIREABLES_PanelFreeHirelingIcons();
    NPCMENU_Shutdown();
    AUTOMAP_Shutdown();
    WAYPOINT_ClosePanel();
    TRADE_CloseOnShutdown();
    UI_CloseGoldEntryDialog();
    INV_UnloadPanelImages();
    USEITEM_AnvilShutdown();
    //  takes no argument
    j_QUESTLOG_QuestUnloadPanelImages();
    INV_UnloadShopPanelImages();
    DIALOG_Shutdown();
    LISTBOX_UnloadScrollBarImage();

    if (gpImgPentSpin)
    {
        ARCHIVE_CellFileFree(gpImgPentSpin);
        gpImgPentSpin = 0;
    }
    if (gpImgFocus16)
    {
        ARCHIVE_CellFileFree(gpImgFocus16);
        gpImgFocus16 = 0;
    }
    if (gpImgUiBoxPieces)
    {
        ARCHIVE_CellFileFree(gpImgUiBoxPieces);
        gpImgUiBoxPieces = NULL;
    }
    if (gpImgBuySellButton)
    {
        ARCHIVE_CellFileFree(gpImgBuySellButton);
        gpImgBuySellButton = NULL;
    }
    if (gpImgLevelEntryTitle)
    {
        ARCHIVE_CellFileFree(gpImgLevelEntryTitle);
        gpImgLevelEntryTitle = NULL;
    }

    CMDTBL_Shutdown();
    ESCMENU_UnloadImages();

    //  a tail jump in the original, to CONFIG_UnloadTextSliderImage (0x6FAFC590)
    CONFIG_UnloadTextSliderImage();
}

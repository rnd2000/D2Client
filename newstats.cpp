#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "CoreArchive.h"
#include "ui.h"
#include "inv.h"

//  newstats.cpp -- the recipe scroll panel and the "you have unspent points" buttons of D2Client.dll 1.10: the stat-point and skill-point red
//  buttons, their lit and greyed draws, their press and release handlers, and the hit tests the dialog handler asks.
//  No assert in this unit names the original's source file, so the file name is ours. Address range 0x6FAEE990..0x6FAEFA3F.

//  Layout checks: the unit has no struct of its own; what it leans on is the panel ids and constants the original carries as literals.
static_assert(UI_NEWSTATS == 6 && UI_STATSCREEN == 2,
              "the stat pair: the new-stat button and the sheet it opens");
static_assert(UI_NEWSKILLS == 7 && UI_SKILLTREE == 4,
              "the skill pair, the same shape");
static_assert(UI_ESCMENU == 9 && UI_RECIPESCROLL == 37,
              "the two this file also has to see closed");

static_assert(STAT_STATPTS == 4,
              "NEWSTATS_UpdateButtonVisibility's `push 4` (0x6FAEEC28) -- row 4 of "
              "ItemStatCost.txt, statpts");

//  The image paths, as the original's .rdata holds them.
static const char szMenuRecipescroll[] = "menu\\recipescroll";    // 0x6FB79A88
static const char szPanelLevelsocket[] = "Panel\\Levelsocket";    // 0x6FB79A9C
static const char szPanelLevel[]       = "Panel\\Level";          // 0x6FB79AB0


//1.10f: D2Client.0x6FAEE990
//  Puts a text on the recipe scroll and opens it: copies pwszText into gwszRecipeScrollText, stores the colour in gnRecipeScrollColor and calls
//  UI_SetUIToggle(UI_RECIPESCROLL, UI_TURNON, 0). The panel's only opener is TEXT_HandleChatMessage's message type 7, which reaches this by a tail
//  jump. The original is a __thiscall whose `this` is the source string (ecx), with the colour on the stack; a free function cannot say that, so it
//  is spelled __fastcall with an unread edx between the two. The last five instructions are a tail call: the zero stored over this function's own
//  stack argument becomes UI_SetUIToggle's bCheckMousePos, and UI_SetUIToggle's retn 4 returns for both. Written as a call. void: its one caller is a
//  tail jump from TEXT_HandleChatMessage, a void function, so UI_SetUIToggle's answer reaches nobody. BUG: Unicode::strcpy runs to the source's NUL
//  with no bound. LATENT: the destination holds 4096 characters and the one caller copies from a terminated 256-character buffer
//  (SCMD_HandleSrv26_ChatMessage takes the text off the wire with SStrCopy(..., 256), at most 255 characters and a NUL), and no stock server sends
//  message type 7 at all.
void __fastcall NEWSTATS_RecipeScrollShow(const Unicode *pwszText, int32_t nEdxUnused, int32_t nColor)
{
    Unicode::strcpy(gwszRecipeScrollText, pwszText);
    gnRecipeScrollColor = nColor;
    UI_SetUIToggle(UI_RECIPESCROLL, UI_TURNON, 0);
}

//1.10f: D2Client.0x6FAEE9C0
//  Draws the recipe scroll: the four cels of `menu\recipescroll` and the text NEWSTATS_RecipeScrollShow stored, in its colour. Its caller is
//  UIPANELS_DrawInGamePanels.
//  Drawn only in an expansion game (GAME_IsExpansionGame, the game's flag, not the install's) with UI_RECIPESCROLL on. The image has no loader: it
//  is loaded here on the first frame that draws it and kept in gpRecipeScrollImage, which INV_UnloadPanelImages releases. A failed load draws
//  nothing and is tried again next frame.
//  The cels take the left panels' four-cel layout: frames 0 and 1 at y = h - 224 and 2 and 3 at y = h - 48, each pair at x = 0 and x = 256, all
//  shifted by gnBorderXDifference and gnBorderYDifference. The text goes at font 4 (D2FONT_FONTFORMAL10) to x = gnBorderXDifference + 80,
//  y = 130 - gnBorderYDifference: the cels are measured up from the screen's bottom and the text down from its top. gnBorderYDifference is carried
//  negative, so both come out in the same centred 640x480 frame. The font is restored after the text.
//  The draw record (D2GfxDataStrc, 72 bytes) is cleared whole and then nDirection and nFrame are stored 0 again before pCellFile; the C writes
//  those stores as the bytes make them. The first cel's pPalette is the zero still in eax from the clear; the other three push 0.
void __cdecl NEWSTATS_RecipeScrollDraw()
{
    D2CellFileStrc *pImage;
    enum Font nOldFont;
    D2GfxDataStrc tCelContext;

    if (!GAME_IsExpansionGame())
    {
        return;
    }
    if (!UI_GetUiWindowState(UI_RECIPESCROLL))
    {
        return;
    }

    pImage = gpRecipeScrollImage;
    if (!pImage)
    {
        pImage = UI_LoadImageFromUiDir(szMenuRecipescroll);
        gpRecipeScrollImage = pImage;
        if (!pImage)
        {
            return;
        }
    }

    memset(&tCelContext, 0, sizeof(tCelContext));
    tCelContext.nDirection = 0;
    tCelContext.nFrame = 0;
    tCelContext.pCellFile = pImage;
    D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
    tCelContext.nFrame = 1;
    D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference + 256, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
    tCelContext.nFrame = 2;
    D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
    tCelContext.nFrame = 3;
    D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference + 256, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);

    nOldFont = D2WIN_10127_SetFont(D2FONT_FONTFORMAL10);
    D2WIN_10117_DrawText(gwszRecipeScrollText, gnBorderXDifference + 80, 130 - gnBorderYDifference, gnRecipeScrollColor, 0);
    D2WIN_10127_SetFont(nOldFont);
}


//1.10f: D2Client.0x6FAEEB30
//  A click anywhere in the left half of the screen closes the recipe scroll: UI_SetUIToggle(UI_RECIPESCROLL, UI_TURNOFF, 0), then the handler
//  chain is broken (a tail jump into SMsgBreakHandlerChain, written as a call). The panel is unused in 1.10, so nothing exercises the hit box.
//  The hit box is x 0 .. gnScreenWidth / 2 and y 0 .. gnScreenHeight - 49, both bounds inclusive; unlike its sibling TEXT_IniScrollOnMouseDown it
//  bounds both axes below at zero. It has the shape of HELP_HelpButtonOnLButtonDown, whose box is 35 x 36 pixels.
//  gnScreenWidth / 2 is a signed halving in the original (cdq / sub / sar), so a plain / 2 on an int reproduces it.
void __stdcall NEWSTATS_RecipeScrollOnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nBoxBottom;
    int32_t nBoxRight;
    int32_t nMouseX;
    int32_t nMouseY;

    nBoxBottom = gnScreenHeight - 49;
    nBoxRight = gnScreenWidth / 2;
    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();

    if (nMouseX >= 0 && nMouseX <= nBoxRight && nMouseY >= 0 && nMouseY <= nBoxBottom)
    {
        UI_SetUIToggle(UI_RECIPESCROLL, UI_TURNOFF, 0);
        GAME_ResetMouseButtonState();
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
    }
}

//1.10f: D2Client.0x6FAEEBA0
//  Loads the two cels of the red buttons: `Panel\Level`, the button, into gpImgLevelUpButton and `Panel\Levelsocket`, its recess, into
//  gpImgLevelUpSocket. Called by UI_InitUiSubsystems. Both buttons are drawn from these cels, and so are CHAR_CharPanelDraw, HELP_DrawHelpHint and
//  QUESTLOG_DrawFrameAndTitle.
//  void: the second handle is left in eax, and the one call site discards it.
void __cdecl NEWSTATS_LoadImages()
{
    gpImgLevelUpButton = UI_LoadImageFromUiDir(szPanelLevel);
    gpImgLevelUpSocket = UI_LoadImageFromUiDir(szPanelLevelsocket);
}

//1.10f: D2Client.0x6FAEEBC0
//  Releases them: each handle is tested, released through ARCHIVE_CellFileFree and zeroed. Called by UI_ShutdownUiSubsystems.
void __cdecl NEWSTATS_UnloadImages()
{
    if (gpImgLevelUpButton)
    {
        ARCHIVE_CellFileFree(gpImgLevelUpButton);
        gpImgLevelUpButton = 0;
    }
    if (gpImgLevelUpSocket)
    {
        ARCHIVE_CellFileFree(gpImgLevelUpSocket);
        gpImgLevelUpSocket = 0;
    }
}

//1.10f: D2Client.0x6FAEEC00
//  Puts each red button up or takes it down, every frame, from the player's unspent points. Called by UIPANELS_DrawInGamePanels.
//      UI_NEWSTATS   on  when STAT_STATPTS is non-zero and the character sheet (UI_STATSCREEN) is shut
//                    off when STAT_STATPTS is zero
//      UI_NEWSKILLS  on  when STAT_SKILLPTS > 0 and the skill tree is shut
//                    off when STAT_SKILLPTS <= 0
//  Nothing is done while the options screen (UI_CONFIG) is open. An open button is only ever taken down by its points running out; opening its
//  sheet does not close it here, the release handlers do that.
//  BUG: the two stats are tested differently: statpts with test / jnz and jz, newskills with a signed jg and jle. So a negative skill-point count
//  counts as none and a negative stat-point count as some. INERT: both stats are unsigned in ItemStatCost.txt and the server spends both guarded,
//  so no count is negative.
//  Both come from STATLIST_GetUnitBaseStat, layer 0. The player is not tested, and need not be: the D2Common call answers 0 for a NULL unit, so
//  with no player both buttons come down.
//  The two new-stats toggles share one call in the bytes: `mov edx, 1` and `xor edx, edx` both reach the same `mov ecx, 6 / push 0 / call
//  UI_SetUIToggle`. The new-skills toggles are two calls. The three pushes for STATLIST_GetUnitBaseStat are made once, before the branch on the
//  window state, and each arm calls with them.
void __cdecl NEWSTATS_UpdateButtonVisibility()
{
    D2UnitStrc *pPlayer;

    if (UI_GetUiWindowState(UI_CONFIG))
    {
        return;
    }

    pPlayer = CUNIT_GetPlayer();

    if (UI_GetUiWindowState(UI_NEWSTATS))
    {
        if (!D2COMMON_STATLIST_GetUnitBaseStat(pPlayer, STAT_STATPTS, 0))
        {
            UI_SetUIToggle(UI_NEWSTATS, UI_TURNOFF, 0);
        }
    }
    else if (D2COMMON_STATLIST_GetUnitBaseStat(pPlayer, STAT_STATPTS, 0) && !UI_GetUiWindowState(UI_STATSCREEN))
    {
        UI_SetUIToggle(UI_NEWSTATS, UI_TURNON, 0);
    }

    if (UI_GetUiWindowState(UI_NEWSKILLS))
    {
        if (D2COMMON_STATLIST_GetUnitBaseStat(pPlayer, STAT_SKILLPTS, 0) <= 0)
        {
            UI_SetUIToggle(UI_NEWSKILLS, UI_TURNOFF, 0);
        }
    }
    else if (D2COMMON_STATLIST_GetUnitBaseStat(pPlayer, STAT_SKILLPTS, 0) > 0 && !UI_GetUiWindowState(UI_SKILLTREE))
    {
        UI_SetUIToggle(UI_NEWSKILLS, UI_TURNON, 0);
    }
}

//1.10f: D2Client.0x6FAEECC0
//  Is a point over the new-stats button? Called by DIALOG_OnMouseDown, which asks it only while UI_NEWSTATS is on and passes
//  SCROLL_GetUiShiftScreen() first -- a panel state, hence the parameter type D2C_UIpanels.
//      PANEL_LEFT (2)   x in (w/2 + 40, w/2 + 74)
//      otherwise        x in (40, 74)
//      both             y in (h - 139, h - 102)
//  Every bound is exclusive (jle, jge), so the box is 33 by 36: the plain and the shifted arms of NEWSTATS_OnLButtonDown, and not its 800x600 arm,
//  which this test does not have. w / 2 is the signed halving that a plain / 2 on an int compiles to.
//  The two x arms end on one jge in the bytes, the shifted arm jumping to it after its own cmp; the test is written out, each arm with its own
//  upper bound.
BOOL __fastcall NEWSTATS_IsPointOverButton(D2C_UIpanels nUiShift, int32_t nXPos, int32_t nYPos)
{
    int32_t nHalfWidth;

    if (nUiShift == PANEL_LEFT)
    {
        nHalfWidth = gnScreenWidth / 2;
        if (nXPos <= nHalfWidth + 40 || nXPos >= nHalfWidth + 74)
        {
            return FALSE;
        }
    }
    else if (nXPos <= 40 || nXPos >= 74)
    {
        return FALSE;
    }

    if (nYPos <= gnScreenHeight - 139 || nYPos >= gnScreenHeight - 102)
    {
        return FALSE;
    }
    return TRUE;
}

//1.10f: D2Client.0x6FAEED20
//  Is a point over the new-skills button? The mirror of the test above, measured from a right edge. Called by DIALOG_OnMouseDown under
//  UI_NEWSKILLS, with the same first argument.
//      edge = w - w/2 when nUiShift is PANEL_RIGHT (1), w otherwise
//      x in (edge - 73, edge - 40), y in (h - 138, h - 102)
//  Every bound is exclusive: 32 by 35, one column narrower and one row shorter at the top than the stats box. w - w/2 is the centre rounded up.
//  The panel test is an equality with 1.
//  BUG: this test has no 800x600 arm at all and takes the computed edge box at every resolution, where NEWSTATS_NewSkillsOnLButtonDown and
//  NEWSTATS_NewSkillsOnLButtonUp use the 800x600 box. REACHABLE: the NPC dialogs' click-away callbacks answer 1, so with one open at 800x600 a
//  click on the drawn button passes the dialog and a click in the phantom edge box is consumed.
BOOL __fastcall NEWSTATS_NewSkillsIsPointOverButton(D2C_UIpanels nUiShift, int32_t nXPos, int32_t nYPos)
{
    int32_t nEdge;

    if (nUiShift == PANEL_RIGHT)
    {
        nEdge = gnScreenWidth - gnScreenWidth / 2;
    }
    else
    {
        nEdge = gnScreenWidth;
    }

    if (nXPos <= nEdge - 73 || nXPos >= nEdge - 40 || nYPos <= gnScreenHeight - 138 || nYPos >= gnScreenHeight - 102)
    {
        return FALSE;
    }
    return TRUE;
}


//1.10f: D2Client.0x6FAEED80
//  Pressing the "you have unspent stat points" button. Three hit boxes, one per layout, chosen by D2GFX_GetResolutionMode() and
//  SCROLL_GetUiShiftScreen():
//      800x600        MouseX 207..239, MouseY (h-42)..(h-9)   -- the CURSOR, fetched with GetMouseX / GetMouseY
//      shifted (2)    nXpos (w/2+41)..(w/2+73), nYpos (h-138)..(h-103)
//      otherwise      nXpos 41..73, same y band
//  All bounds are exclusive on both ends (every test is <= or >= against the edge), so the plain and shifted boxes are 33 by 36 pixels (x 41..73,
//  y h-138..h-103) and the 800x600 box 33 by 33. The shifted layout is the plain one moved right by half the screen; the y band is identical.
//  BUG: the 800x600 arm reads the cursor and the other two read the message; the message carries where the click landed, the cursor where it is
//  now. NOT ESTABLISHED: whether the two can differ here.
//  D2ExpRes patches this function three times, for 1024x768 and 1280x768; each site is named on the line its detour lands on. The last y
//  comparison is one instruction serving two arms in the bytes (a jmp from the plain arm, fall-through from the 800x600 arm); written with a goto.
void __stdcall NEWSTATS_OnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    int32_t nUiShiftScreen;
    int32_t bAboveBottomEdge;
    int32_t nMouseX;
    int32_t nMouseY;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;

    //  BUG: the one rejection that leaves bReturn alone (jnz skips the store, while every other rejection stores 0 first). NOT ESTABLISHED: Storm
    //  presets bReturn to 0, so the silence shows only after a handler that writes 1 and lets the chain go on.
    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600 && UI_GetUiWindowState(UI_ESCMENU))
    {
        return;
    }

    nUiShiftScreen = SCROLL_GetUiShiftScreen();

    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600)
    {
        nMouseX = CURSOR_GetMouseX();
        nMouseY = CURSOR_GetMouseY();
        //  D2ExpRes NewStatsButtonClickdown800x600 lands on the 206.
        if (nMouseX <= 206 || nMouseX >= 240 || nMouseY <= gnScreenHeight - 42)
        {
            goto NOT_HANDLED;
        }
        bAboveBottomEdge = nMouseY < gnScreenHeight - 8;
        goto SHARED_TAIL;
    }

    if (nUiShiftScreen != PANEL_LEFT)
    {
        //  D2ExpRes NewStatsButtonClickdown lands on the 40.
        if (nXpos <= 40 || nXpos >= 74 || nYpos <= gnScreenHeight - 139)
        {
            goto NOT_HANDLED;
        }
        bAboveBottomEdge = nYpos < gnScreenHeight - 102;
    SHARED_TAIL:
        if (!bAboveBottomEdge)
        {
        NOT_HANDLED:
            pMsg->bReturn = 0;
            return;
        }
    }
    else
    {
        //  D2ExpRes NewStatsButtonClickdownCharsheetOpen lands on the w / 2 + 40.
        if (nXpos <= gnScreenWidth / 2 + 40 || nXpos >= gnScreenWidth / 2 + 74 || nYpos <= gnScreenHeight - 139 || nYpos >= gnScreenHeight - 102)
        {
            goto NOT_HANDLED;
        }
    }

    gbNewStatsButtonDown = 1;
    SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
    CURSOR_UpdateCursorPosInGame(pMsg);

    if (nUiShiftScreen != PANEL_LEFT || !NPCMENU_GetInteractingNpcUnit())
    {
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
    }
}

//1.10f: D2Client.0x6FAEEFD0
//  Releasing the button opens the character sheet. The same three layouts and boxes as the press above, with two differences:
//  The 800x600 arm opens the sheet without closing the button panel. The other arm issues two toggles, UI_SetUIToggle(UI_NEWSTATS, UI_TURNOFF)
//  then UI_SetUIToggle(UI_STATSCREEN, UI_TURNON), and the 800x600 arm only the second. Both clear gbNewStatsButtonDown. REFUTED as a defect: at
//  800x600 NEWSTATS_PanelDrawNewStatsButton skips its hiding set and keeps the button lit in the control panel's strip, and
//  NEWSTATS_UpdateButtonVisibility takes it down when the points run out.
//  A release outside the box still clears the pressed flag: every rejection lands on the same tail (gbNewStatsButtonDown = 0 and bReturn = 0), so
//  dragging off the button cancels it; the press handler has no matching path.
//  CURSOR_OnPanelMouseUp runs first, before any hit test, on every path that gets past the escape-menu guard.
//  The escape-menu early-out has the same shape and the same silence as the press handler's: at 800x600 with the menu open the function returns
//  having written nothing at all.
//  D2ExpRes patches this function three times, the mirror of the press handler's: NewStatsButtonRelease, NewStatsButtonRelease800x600 and
//  NewStatsButtonReleaseCharsheetOpen.
void __stdcall NEWSTATS_OnLButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    int32_t nMouseX;
    int32_t nMouseY;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;

    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600 && UI_GetUiWindowState(UI_ESCMENU))
    {
        return;
    }

    CURSOR_OnPanelMouseUp(pMsg);

    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600)
    {
        if (gbNewStatsButtonDown)
        {
            nMouseX = CURSOR_GetMouseX();
            nMouseY = CURSOR_GetMouseY();
            //  D2ExpRes NewStatsButtonRelease800x600 lands on the 206.
            if (nMouseX > 206 && nMouseX < 240 && nMouseY > gnScreenHeight - 42 && nMouseY < gnScreenHeight - 8)
            {
                gbNewStatsButtonDown = 0;
                //  one toggle here, two on the other arm
                UI_SetUIToggle(UI_STATSCREEN, UI_TURNON, 0);
                pMsg->bReturn = 1;
                pMsg->lResult = 0;
                STORM_SMsgBreakHandlerChain(pMsg);
                return;
            }
        }
        goto RELEASE_OUTSIDE;
    }

    if (gbNewStatsButtonDown)
    {
        if (SCROLL_GetUiShiftScreen() == PANEL_LEFT)
        {
            //  D2ExpRes NewStatsButtonReleaseCharsheetOpen lands on the w / 2 + 40.
            if (nXpos <= gnScreenWidth / 2 + 40 || nXpos >= gnScreenWidth / 2 + 74 || nYpos <= gnScreenHeight - 139 || nYpos >= gnScreenHeight - 102)
            {
                goto RELEASE_OUTSIDE;
            }
        }
        //  D2ExpRes NewStatsButtonRelease lands on the 40.
        else if (nXpos <= 40 || nXpos >= 74 || nYpos <= gnScreenHeight - 139 || nYpos >= gnScreenHeight - 102)
        {
            goto RELEASE_OUTSIDE;
        }

        gbNewStatsButtonDown = 0;
        UI_SetUIToggle(UI_NEWSTATS, UI_TURNOFF, 0);
        UI_SetUIToggle(UI_STATSCREEN, UI_TURNON, 0);
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        return;
    }

RELEASE_OUTSIDE:
    gbNewStatsButtonDown = 0;
    pMsg->bReturn = 0;
}

//1.10f: D2Client.0x6FAEEEA0
//  Pressing the "you have unspent skill points" button; the mirror of NEWSTATS_OnLButtonDown. Three hit boxes, one per layout, chosen by
//  D2GFX_GetResolutionMode() and SCROLL_GetUiShiftScreen():
//      800x600        MouseX 563..597, MouseY (h-42)..(h-8)   -- the CURSOR
//      PANEL_RIGHT    nXpos (halfX-73)..(halfX-40),  nYpos (h-138)..(h-102)
//      otherwise      nXpos (w-73)..(w-40),          same y band
//  where halfX is gnScreenWidth - gnScreenWidth / 2, the centre rounded up. All bounds are exclusive on both ends: every comparison is jle or jge
//  against the edge.
//  SCROLL_GetUiShiftScreen() returns gUiShiftScreen, a panel state and not a pixel offset: PANEL_NONE 0, PANEL_RIGHT 1, PANEL_LEFT 2, PANEL_BOTH 3.
//  This handler tests 1 and the stats handler tests 2: the skill tree is the right panel, so the skills button steps left of centre when the
//  right panel opens, and the character sheet is the left panel, so the stats button steps right of centre. They never both fire.
//  BUG: neither handler tests PANEL_BOTH, so with both panels open each falls into its edge box. NEWSTATS_PanelDrawNewSkillButton does test it
//  outside 800x600 (UiShiftScreen == 3 clears gbNewSkillsButtonDown and returns without drawing), so with both panels open the button is
//  invisible while this handler still accepts a click in the right-edge box, which then sits under the right-hand panel. NOT ESTABLISHED: whether
//  anything reaches it depends on the order of the message handler chain.
//  The two buttons are not the same size:
//      skills   x edge-73 .. edge-40   y h-138 .. h-102   32 x 35
//      stats    x 40 .. 74             y h-139 .. h-102   33 x 36
//  so the skills box is one column narrower and one row shorter, and the row it loses is at the top. At 800x600 the asymmetry vanishes: both are
//  33 x 33 and their centres are 580 and 223, which sum to 803, mirrored about the middle of an 800-wide screen.
//  The first early-out does not write bReturn, exactly as in NEWSTATS_OnLButtonDown, and its verdict is the same.
//  SCROLL_GetUiShiftScreen() is called before the resolution branch, and its value survives in ebp to gate the tail. So even on the 800x600 path,
//  a press with the right panel open and an NPC being talked to sets gbNewSkillsButtonDown, plays the sound, moves the cursor, and then declines
//  to consume the message.
//  One jge is reached two ways: by a jmp from the PANEL_RIGHT arm and by fall-through from the 800x600 arm; the plain arm does not share it. It is
//  written with a goto. This is the opposite wiring from NEWSTATS_OnLButtonDown, where the shared instruction is reached by a jmp from the plain
//  arm and by fall-through from the 800x600 one.
//  D2ExpRes patches this function three times; the other NewSkills sites are in the drawing code, named at their lines below.
void __stdcall NEWSTATS_NewSkillsOnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    int32_t nUiShiftScreen;
    int32_t nHalfScreenX;
    int32_t bInsideBottom;
    int32_t nMouseX;
    int32_t nMouseY;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;

    //  BUG: the one rejection that leaves bReturn alone, as in NEWSTATS_OnLButtonDown; the verdict is the same.
    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600 && UI_GetUiWindowState(UI_ESCMENU))
    {
        return;
    }

    nUiShiftScreen = SCROLL_GetUiShiftScreen();

    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600)
    {
        nMouseX = CURSOR_GetMouseX();
        nMouseY = CURSOR_GetMouseY();
        //  D2ExpRes NewSkillsButtonClickdown800x600 lands on the 563.
        if (nMouseX <= 563 || nMouseX >= 597 || nMouseY <= gnScreenHeight - 42)
        {
            goto NOT_HANDLED;
        }
        bInsideBottom = nMouseY < gnScreenHeight - 8;
        goto SHARED_TAIL;
    }

    if (nUiShiftScreen == PANEL_RIGHT)
    {
        nHalfScreenX = gnScreenWidth - gnScreenWidth / 2;
        //  D2ExpRes NewSkillsButtonClickdownSkilltabOpen lands on the - 73.
        if (nXpos <= nHalfScreenX - 73 || nXpos >= nHalfScreenX - 40 || nYpos <= gnScreenHeight - 138)
        {
            goto NOT_HANDLED;
        }
        bInsideBottom = nYpos < gnScreenHeight - 102;
    SHARED_TAIL:
        if (!bInsideBottom)
        {
        NOT_HANDLED:
            pMsg->bReturn = 0;
            return;
        }
    }
    else
    {
        //  D2ExpRes NewSkillsButtonClickdown lands on the - 73.
        if (nXpos <= gnScreenWidth - 73 || nXpos >= gnScreenWidth - 40 || nYpos <= gnScreenHeight - 138 || nYpos >= gnScreenHeight - 102)
        {
            goto NOT_HANDLED;
        }
    }

    gbNewSkillsButtonDown = 1;
    SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
    CURSOR_UpdateCursorPosInGame(pMsg);

    if (nUiShiftScreen != PANEL_RIGHT || !NPCMENU_GetInteractingNpcUnit())
    {
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
    }
}

//1.10f: D2Client.0x6FAEF130
//  Releasing the button opens the skill tree. The same three boxes as the press above, and two things beside the stats release handler:
//  The 800x600 arm again issues one toggle where the other issues two: the non-800x600 path does UI_SetUIToggle(UI_NEWSKILLS, UI_TURNOFF, 0)
//  then UI_SetUIToggle(UI_SKILLTREE, UI_TURNON, 0), the 800x600 path only the second. As in NEWSTATS_OnLButtonUp, the button stays lit at 800x600
//  and NEWSTATS_UpdateButtonVisibility takes it down when the points run out.
//  A release outside the box still clears the pressed flag, on every path including the one that never armed it, so gbNewSkillsButtonDown = 0
//  runs even when it was already 0.
//  CURSOR_OnPanelMouseUp runs first, before any hit test, on every path past the escape-menu guard, and that guard has the same silence as the
//  press handler's: it returns having written nothing.
//  The PANEL_RIGHT and plain arms differ only in the anchor, but the bytes hold two copies of the comparison chain, so the duplicated form is
//  what is written.
//  D2ExpRes patches this function three times, the mirror of the press handler's: NewSkillsButtonRelease, NewSkillsButtonReleaseSkilltabOpen and
//  NewSkillsButtonRelease800x600.
void __stdcall NEWSTATS_NewSkillsOnLButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    int32_t nHalfScreenX;
    int32_t nMouseX;
    int32_t nMouseY;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;

    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600 && UI_GetUiWindowState(UI_ESCMENU))
    {
        return;
    }

    CURSOR_OnPanelMouseUp(pMsg);

    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600)
    {
        if (gbNewSkillsButtonDown)
        {
            nMouseX = CURSOR_GetMouseX();
            nMouseY = CURSOR_GetMouseY();
            //  D2ExpRes NewSkillsButtonRelease800x600 lands on the 563.
            if (nMouseX > 563 && nMouseX < 597 && nMouseY > gnScreenHeight - 42 && nMouseY < gnScreenHeight - 8)
            {
                gbNewSkillsButtonDown = 0;
                //  one toggle here, two on the other arm
                UI_SetUIToggle(UI_SKILLTREE, UI_TURNON, 0);
                pMsg->bReturn = 1;
                pMsg->lResult = 0;
                STORM_SMsgBreakHandlerChain(pMsg);
                return;
            }
        }
        goto RELEASE_OUTSIDE;
    }

    if (gbNewSkillsButtonDown)
    {
        if (SCROLL_GetUiShiftScreen() == PANEL_RIGHT)
        {
            nHalfScreenX = gnScreenWidth - gnScreenWidth / 2;
            //  D2ExpRes NewSkillsButtonReleaseSkilltabOpen lands on the - 73.
            if (nXpos <= nHalfScreenX - 73 || nXpos >= nHalfScreenX - 40 || nYpos <= gnScreenHeight - 138 || nYpos >= gnScreenHeight - 102)
            {
                goto RELEASE_OUTSIDE;
            }
        }
        //  D2ExpRes NewSkillsButtonRelease lands on the - 73.
        else if (nXpos <= gnScreenWidth - 73 || nXpos >= gnScreenWidth - 40 || nYpos <= gnScreenHeight - 138 || nYpos >= gnScreenHeight - 102)
        {
            goto RELEASE_OUTSIDE;
        }

        gbNewSkillsButtonDown = 0;
        UI_SetUIToggle(UI_NEWSKILLS, UI_TURNOFF, 0);
        UI_SetUIToggle(UI_SKILLTREE, UI_TURNON, 0);
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        return;
    }

RELEASE_OUTSIDE:
    gbNewSkillsButtonDown = 0;
    pMsg->bReturn = 0;
}


//1.10f: D2Client.0x6FAEF2A0
//  The new-stats button, greyed -- at 800x600 only. Called by UIPANELS_DrawInGamePanels when UI_NEWSTATS is off; the other arm of the same test
//  calls NEWSTATS_PanelDrawNewStatsButton.
//  At any other resolution it draws nothing. At 800x600 it draws frame 2 of `Panel\Level` (gpImgLevelUpButton) at (206, h - 8), in the control
//  panel's strip, and, while the cursor is strictly inside x 206..240, y h-42..h-8 with the escape menu shut, tbl 3986 (strlvlup) framed and
//  centred (nAlign 1) at (221, 550) -- before the cel. 550 is a constant; at the one resolution this arm runs in it is h - 50. The text colour is
//  the zero GetUiWindowState has just answered.
//  The record is cleared whole and nDirection stored 0 again, as in NEWSTATS_RecipeScrollDraw; the C writes the stores as the bytes make them.
//  D2ExpRes patches this function three times, at the lines named below.
void __cdecl NEWSTATS_PanelDrawNewStatsButtonGrey()
{
    int32_t nMouseX;
    int32_t nMouseY;
    D2GfxDataStrc tCelContext;

    if (D2GFX_GetResolutionMode() != D2GAMERES_800x600)
    {
        return;
    }

    memset(&tCelContext, 0, sizeof(tCelContext));
    tCelContext.nDirection = 0;
    tCelContext.pCellFile = gpImgLevelUpButton;
    tCelContext.nFrame = 2;

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    //  D2ExpRes GreyNewStatsButtonMouseHoverOver800x600 lands on the 206.
    if (nMouseX > 206 && nMouseX < 240 && nMouseY > gnScreenHeight - 42 && nMouseY < gnScreenHeight - 8 && !UI_GetUiWindowState(UI_ESCMENU))
    {
        //  D2ExpRes GreyNewStatsString800x600 lands on the 550.
        D2WIN_10129_DrawFramedText(D2LANG_GetStringFromTblIndex(STR_IDX_3986_strlvlup), 221, 550, 0, 1);
    }
    //  D2ExpRes GreyNewStatsButton800x600 lands on the - 8.
    D2GFX_TEXTURE_CelDraw(&tCelContext, 206, gnScreenHeight - 8, 0xFFFFFFFF, 5, NULL);
}

//1.10f: D2Client.0x6FAEF360
//  The new-stats button, lit: the socket, the label above it and the red button in it, drawn pressed while it is held. Called by
//  UIPANELS_DrawInGamePanels when UI_NEWSTATS is on; the jump after the call goes past the grey draw, to the new-skills test.
//  Outside 800x600 the button is not drawn -- and gbNewStatsButtonDown is cleared, so a press in progress is dropped -- when, in this order:
//      both panels are open (the panel state is PANEL_BOTH)
//      UI_STATSCREEN is open            the sheet the button opens
//      UI_PARTYSCREEN AND UI_INVENTORY are open
//      UI_PARTYSCREEN AND UI_SKILLTREE are open
//      UI_NPCSHOP is open
//  At 800x600 the resolution test jumps past all five: the button sits in the control panel's strip there, below every panel.
//  The socket and the label: x is 40, or w/2 + 40 while the left panel is open (PANEL_LEFT; the signed halving). The socket, frame 0 of
//  `Panel\Levelsocket`, is measured -- CelGetWidth(CelGetHandle(&record)) -- and so is tbl 3986's text, both before the resolution test, so at
//  800x600 they are measured and not used. Outside 800x600 the label is drawn at (socket/2 - text/2 + x + 1, h - 142), colour 0, not centred by
//  D2Win but by hand on the socket's width, with the + 1 the bytes have. Then the socket at (x, h - 105). At 800x600 neither is drawn.
//  The button: the record is cleared again for `Panel\Level`, and the frame is 1 while gbNewStatsButtonDown is set and the cursor is strictly
//  inside the box, 0 otherwise:
//      800x600      x 206..240          y h-42..h-8
//      PANEL_LEFT   x w/2+40..w/2+74    y h-139..h-102
//      otherwise    x 40..74            y h-139..h-102
//  NEWSTATS_OnLButtonDown's three boxes, number for number, though that handler tests the message's point on the last two arms and this draw the
//  cursor on all three. The 800x600 arm reads GetMouseX before GetMouseY, the other two GetMouseY first; the C keeps each order.
//  The last y compare of the plain arm and of the 800x600 arm is one instruction in the bytes, reached by a jmp from the plain arm and by
//  fall-through from the 800x600 arm (the press handler's wiring); each arm is written with its own four compares.
//  With the frame chosen, the button cel's width and tbl 3986's text width are measured again and neither answer is read; written as the two
//  nested calls with their answers dropped, since the rewrite makes every call the original makes.
//  The draw: at 800x600 the button goes to (206, h - 8) and, while the cursor is strictly inside its box with the escape menu shut, tbl 3986 is
//  framed at (221, 550) before it -- NEWSTATS_PanelDrawNewStatsButtonGrey's lines, with the frame chosen above in place of frame 2. Elsewhere it
//  goes to (x + 3, h - 109), three right of and four above the socket, with no hover text: the label above the socket is always there.
//  BUG: neither cel handle is tested, here or in the other three draws, so a load that answered 0 reaches CelGetHandle and CelDraw as the record's
//  pCellFile, where NEWSTATS_RecipeScrollDraw tests its image. LATENT: a missing file makes ARCHIVE_CellFileLoad answer 0 and the first
//  CelGetHandle then asserts on the cel context and exits, but both cels ship with the game.
//  D2ExpRes patches this function eight times, at the lines named below.
void __cdecl NEWSTATS_PanelDrawNewStatsButton()
{
    D2C_UIpanels nUiShift;
    int32_t nXPos;
    int32_t nSocketWidth;
    int32_t nTextWidth;
    int32_t nMouseX;
    int32_t nMouseY;
    BOOL bPressed;
    D2GfxDataStrc tCelContext;

    nUiShift = SCROLL_GetUiShiftScreen();
    if (D2GFX_GetResolutionMode() != D2GAMERES_800x600 && (nUiShift == PANEL_BOTH || UI_GetUiWindowState(UI_STATSCREEN) || (UI_GetUiWindowState(UI_PARTYSCREEN) && UI_GetUiWindowState(UI_INVENTORY)) || (UI_GetUiWindowState(UI_PARTYSCREEN)
                && UI_GetUiWindowState(UI_SKILLTREE)) || UI_GetUiWindowState(UI_NPCSHOP)))
    {
        gbNewStatsButtonDown = 0;
        return;
    }

    if (nUiShift == PANEL_LEFT)
    {
        nXPos = gnScreenWidth / 2 + 40;
    }
    else
    {
        nXPos = 40;
    }

    //  the socket, and the label centred on it
    memset(&tCelContext, 0, sizeof(tCelContext));
    tCelContext.nDirection = 0;
    tCelContext.pCellFile = gpImgLevelUpSocket;
    tCelContext.nFrame = 0;
    nSocketWidth = D2CMP_CelGetWidth(D2CMP_CelGetHandle(&tCelContext));
    nTextWidth = D2WIN_10121_GetTextWidth(D2LANG_GetStringFromTblIndex(STR_IDX_3986_strlvlup));
    if (D2GFX_GetResolutionMode() != D2GAMERES_800x600)
    {
        D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_3986_strlvlup), nSocketWidth / 2 - nTextWidth / 2 + nXPos + 1, gnScreenHeight - 142, 0, 0);
        D2GFX_TEXTURE_CelDraw(&tCelContext, nXPos, gnScreenHeight - 105, 0xFFFFFFFF, 5, NULL);
    }

    //  the button, frame 1 while it is held under the cursor
    memset(&tCelContext, 0, sizeof(tCelContext));
    tCelContext.nDirection = 0;
    tCelContext.pCellFile = gpImgLevelUpButton;
    bPressed = FALSE;
    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600)
    {
        if (gbNewStatsButtonDown)
        {
            nMouseX = CURSOR_GetMouseX();
            nMouseY = CURSOR_GetMouseY();
            //  D2ExpRes NewStatsButtonClickdown2800x600 lands on the 206.
            bPressed = nMouseX > 206 && nMouseX < 240 && nMouseY > gnScreenHeight - 42 && nMouseY < gnScreenHeight - 8;
        }
    }
    else if (gbNewStatsButtonDown)
    {
        nMouseY = CURSOR_GetMouseY();
        nMouseX = CURSOR_GetMouseX();
        if (nUiShift == PANEL_LEFT)
        {
            //  D2ExpRes NewStatsButtonClickdown2CharsheetOpen lands on the w / 2 + 40.
            bPressed = nMouseX > gnScreenWidth / 2 + 40 && nMouseX < gnScreenWidth / 2 + 74 && nMouseY > gnScreenHeight - 139 && nMouseY < gnScreenHeight - 102;
        }
        else
        {
            //  D2ExpRes NewStatsButtonClickdown2 lands on the 40.
            bPressed = nMouseX > 40 && nMouseX < 74 && nMouseY > gnScreenHeight - 139 && nMouseY < gnScreenHeight - 102;
        }
    }
    tCelContext.nFrame = bPressed ? 1 : 0;

    //  measured again, and neither answer is read
    D2CMP_CelGetWidth(D2CMP_CelGetHandle(&tCelContext));
    D2WIN_10121_GetTextWidth(D2LANG_GetStringFromTblIndex(STR_IDX_3986_strlvlup));

    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600)
    {
        nMouseX = CURSOR_GetMouseX();
        nMouseY = CURSOR_GetMouseY();
        //  D2ExpRes NewStatsButtonMouseHoverOver800x600 lands on the 206.
        if (nMouseX > 206 && nMouseX < 240 && nMouseY > gnScreenHeight - 42 && nMouseY < gnScreenHeight - 8 && !UI_GetUiWindowState(UI_ESCMENU))
        {
            //  D2ExpRes NewStatsString800x600 lands on the 550.
            D2WIN_10129_DrawFramedText(D2LANG_GetStringFromTblIndex(STR_IDX_3986_strlvlup), 221, 550, 0, 1);
        }
        //  D2ExpRes NewStatsButton800x600 lands on the - 8.
        D2GFX_TEXTURE_CelDraw(&tCelContext, 206, gnScreenHeight - 8, 0xFFFFFFFF, 5, NULL);
    }
    else
    {
        //  D2ExpRes NewStatsString lands on the height load here (named for a string although none is drawn after it on this path), and
        //  NewStatsButton on the - 109.
        D2GFX_TEXTURE_CelDraw(&tCelContext, nXPos + 3, gnScreenHeight - 109, 0xFFFFFFFF, 5, NULL);
    }
}

//1.10f: D2Client.0x6FAEF670
//  The new-skills button, greyed -- at 800x600 only. Called by UIPANELS_DrawInGamePanels when UI_NEWSKILLS is off.
//  NEWSTATS_PanelDrawNewStatsButtonGrey instruction for instruction, with the button's own numbers: the cel at (563, h - 8), the hover box
//  x 563..597, y h-42..h-8, and tbl 3987 (strnewskl) framed at (578, 550). In both, the label's x is 15 right of the cel's, and the two hover
//  boxes' centres, 223 and 580, sum to 803, as the press handlers' 800x600 boxes do.
//  D2ExpRes patches this function three times, at the lines named below.
void __cdecl NEWSTATS_PanelDrawNewSkillButtonGrey()
{
    int32_t nMouseX;
    int32_t nMouseY;
    D2GfxDataStrc tCelContext;

    if (D2GFX_GetResolutionMode() != D2GAMERES_800x600)
    {
        return;
    }

    memset(&tCelContext, 0, sizeof(tCelContext));
    tCelContext.nDirection = 0;
    tCelContext.pCellFile = gpImgLevelUpButton;
    tCelContext.nFrame = 2;

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    //  D2ExpRes GreyNewSkillsButtonMouseHoverOver800x600 lands on the 563.
    if (nMouseX > 563 && nMouseX < 597 && nMouseY > gnScreenHeight - 42 && nMouseY < gnScreenHeight - 8 && !UI_GetUiWindowState(UI_ESCMENU))
    {
        //  D2ExpRes GreyNewSkillsString800x600 lands on the 550.
        D2WIN_10129_DrawFramedText(D2LANG_GetStringFromTblIndex(STR_IDX_3987_strnewskl), 578, 550, 0, 1);
    }
    //  D2ExpRes GreyNewSkillsButton800x600 lands on the - 8.
    D2GFX_TEXTURE_CelDraw(&tCelContext, 563, gnScreenHeight - 8, 0xFFFFFFFF, 5, NULL);
}

//1.10f: D2Client.0x6FAEF730
//  The new-skills button, lit. Called by UIPANELS_DrawInGamePanels when UI_NEWSKILLS is on. NEWSTATS_PanelDrawNewStatsButton's shape, measured from
//  the right edge, with tbl 3987 (strnewskl); what differs:
//  The hiding set is not the mirror of the stats button's. Outside 800x600, in this order:
//      both panels are open (PANEL_BOTH)
//      UI_SKILLTREE is open             the tree the button opens
//      UI_NPCSHOP is open
//      UI_PARTYSCREEN AND UI_INVENTORY are open
//  so four conditions where the stats button has five: party screen plus skill tree hides the stats button and not this one.
//  gbNewSkillsButtonDown is cleared on the way out; at 800x600 none of it runs.
//  The edge: x is edge - 73, where edge is w - w/2 (the centre rounded up) while the right panel is open (PANEL_RIGHT), and w otherwise. The
//  socket, the label on it and the button's offset of three right and four up are the stats button's.
//  The pressed box:
//      800x600       x 563..597                  y h-42..h-8
//      PANEL_RIGHT   x edge-73..edge-40          y h-138..h-102
//      otherwise     x w-73..w-40                y h-138..h-102
//  every bound strict: NEWSTATS_NewSkillsOnLButtonDown's boxes, one column narrower and one row shorter at the top than the stats button's. The
//  panel test here is an equality with 1, and the edge is recomputed for it. The shared jge is reached by the jmp of the PANEL_RIGHT arm, not the
//  plain one: the opposite wiring from the stats draw, and the same as NEWSTATS_NewSkillsOnLButtonDown's against its sibling; each draw is wired
//  as its press handler.
//  The rest -- the throwaway second measurement, the 800x600 hover text at (578, 550), the cel at (563, h - 8) or at (x + 3, h - 109) -- is the
//  stats button's.
//  D2ExpRes patches this function eight times, at the lines named below.
void __cdecl NEWSTATS_PanelDrawNewSkillButton()
{
    D2C_UIpanels nUiShift;
    int32_t nXPos;
    int32_t nSocketWidth;
    int32_t nTextWidth;
    int32_t nEdge;
    int32_t nMouseX;
    int32_t nMouseY;
    BOOL bPressed;
    D2GfxDataStrc tCelContext;

    nUiShift = SCROLL_GetUiShiftScreen();
    if (D2GFX_GetResolutionMode() != D2GAMERES_800x600 && (nUiShift == PANEL_BOTH || UI_GetUiWindowState(UI_SKILLTREE) || UI_GetUiWindowState(UI_NPCSHOP) || (UI_GetUiWindowState(UI_PARTYSCREEN) && UI_GetUiWindowState(UI_INVENTORY))))
    {
        gbNewSkillsButtonDown = 0;
        return;
    }

    if (nUiShift == PANEL_RIGHT)
    {
        nXPos = gnScreenWidth - gnScreenWidth / 2 - 73;
    }
    else
    {
        nXPos = gnScreenWidth - 73;
    }

    //  the socket, and the label centred on it
    memset(&tCelContext, 0, sizeof(tCelContext));
    tCelContext.nDirection = 0;
    tCelContext.pCellFile = gpImgLevelUpSocket;
    tCelContext.nFrame = 0;
    nSocketWidth = D2CMP_CelGetWidth(D2CMP_CelGetHandle(&tCelContext));
    nTextWidth = D2WIN_10121_GetTextWidth(D2LANG_GetStringFromTblIndex(STR_IDX_3987_strnewskl));
    if (D2GFX_GetResolutionMode() != D2GAMERES_800x600)
    {
        D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_3987_strnewskl), nSocketWidth / 2 - nTextWidth / 2 + nXPos + 1, gnScreenHeight - 142, 0, 0);
        D2GFX_TEXTURE_CelDraw(&tCelContext, nXPos, gnScreenHeight - 105, 0xFFFFFFFF, 5, NULL);
    }

    //  the button, frame 1 while it is held under the cursor
    memset(&tCelContext, 0, sizeof(tCelContext));
    tCelContext.nDirection = 0;
    tCelContext.pCellFile = gpImgLevelUpButton;
    bPressed = FALSE;
    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600)
    {
        if (gbNewSkillsButtonDown)
        {
            nMouseX = CURSOR_GetMouseX();
            nMouseY = CURSOR_GetMouseY();
            //  D2ExpRes NewSkillsButtonClickdown2800x600 lands on the 563.
            bPressed = nMouseX > 563 && nMouseX < 597 && nMouseY > gnScreenHeight - 42 && nMouseY < gnScreenHeight - 8;
        }
    }
    else if (gbNewSkillsButtonDown)
    {
        nMouseY = CURSOR_GetMouseY();
        nMouseX = CURSOR_GetMouseX();
        if (nUiShift == PANEL_RIGHT)
        {
            nEdge = gnScreenWidth - gnScreenWidth / 2;
            //  D2ExpRes NewSkillsButtonClickdown2SkilltabOpen lands on the - 73 of the PANEL_RIGHT arm.
            bPressed = nMouseX > nEdge - 73 && nMouseX < nEdge - 40 && nMouseY > gnScreenHeight - 138 && nMouseY < gnScreenHeight - 102;
        }
        else
        {
            //  D2ExpRes NewSkillsButtonClickdown2 lands on the - 73.
            bPressed = nMouseX > gnScreenWidth - 73 && nMouseX < gnScreenWidth - 40 && nMouseY > gnScreenHeight - 138 && nMouseY < gnScreenHeight - 102;
        }
    }
    tCelContext.nFrame = bPressed ? 1 : 0;

    //  measured again, and neither answer is read
    D2CMP_CelGetWidth(D2CMP_CelGetHandle(&tCelContext));
    D2WIN_10121_GetTextWidth(D2LANG_GetStringFromTblIndex(STR_IDX_3987_strnewskl));

    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600)
    {
        nMouseX = CURSOR_GetMouseX();
        nMouseY = CURSOR_GetMouseY();
        //  D2ExpRes NewSkillsButtonMouseHoverOver800x600 lands on the 563.
        if (nMouseX > 563 && nMouseX < 597 && nMouseY > gnScreenHeight - 42 && nMouseY < gnScreenHeight - 8 && !UI_GetUiWindowState(UI_ESCMENU))
        {
            //  D2ExpRes NewSkillsString800x600 lands on the 550.
            D2WIN_10129_DrawFramedText(D2LANG_GetStringFromTblIndex(STR_IDX_3987_strnewskl), 578, 550, 0, 1);
        }
        //  D2ExpRes NewSkillsButton800x600 lands on the - 8.
        D2GFX_TEXTURE_CelDraw(&tCelContext, 563, gnScreenHeight - 8, 0xFFFFFFFF, 5, NULL);
    }
    else
    {
        //  D2ExpRes NewSkillsString lands on the height load here (named for a string, as the stats button's is), and NewSkillsButton on the - 109.
        D2GFX_TEXTURE_CelDraw(&tCelContext, nXPos + 3, gnScreenHeight - 109, 0xFFFFFFFF, 5, NULL);
    }
}

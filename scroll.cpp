#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "CoreArchive.h"
#include "scroll.h"

//  scroll.cpp -- the viewport and side-panel shift of D2Client.dll 1.10: SCROLL_SetWindowMode (slides the world sideways while a side panel is
//  open), the viewport origin with its getters and its per-frame update, and the screen-to-game coordinate conversion.
//  The original source file is GAME\scroll.cpp (SCROLL_SetWindowMode's halt names it). Address range 0x6FAB5750..0x6FAB5A15, eight functions.

//  The source file name, as SCROLL_SetWindowMode's halt hands it to FOG_DisplayHalt.
static const char szSourceFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\GAME\\scroll.cpp";

//  The panel mode is stored and returned as a whole dword, which lets the global be typed as the enum.
static_assert(sizeof(D2C_UIpanels) == 4,
              "gUiShiftScreen is one dword, loaded and stored whole");

//1.10f: D2Client.0x6FAB5750
//  Returns gUiShiftScreen, the D2C_UIpanels value SCROLL_SetWindowMode stored: which side panels are open. Callers compare it for equality
//  or switch on it. No argument, so the calling convention is not observable; __cdecl like its neighbours.
D2C_UIpanels __cdecl SCROLL_GetUiShiftScreen()
{
    return gUiShiftScreen;
}

//1.10f: D2Client.0x6FAB5760
//  Stores the panel mode and slides the world sideways so that an open panel does not cover the player. Takes a D2C_UIpanels in ecx.
//
//      PANEL_NONE   adjust  0    rect [0,  0] .. [xRight,     yBottom]
//      PANEL_RIGHT  adjust -a    rect [-q, 0] .. [xRight - q, yBottom]
//      PANEL_LEFT   adjust +a    rect [+q, 0] .. [xRight + q, yBottom]
//      PANEL_BOTH   adjust  0    rect [0,  0] .. [xRight,     yBottom]
//
//  with a = gnScreenWidth / 4 and q = gnViewportRight / 4: the two single-panel cases keep the viewport's width and slide it a quarter
//  screen. The adjust and the rectangle use different globals; they agree only at 640x480 with no status bar.
//  PANEL_BOTH is not PANEL_NONE even though its rect and adjust are equal: it alone tail-calls GAME_ResetMouseButtonState, so opening the
//  second panel drops a held mouse button.
//  Two jump tables share one range check: the default that halts belongs to the first switch, the second is unguarded.
//  `cdq / and edx, 3 / add / sar 2` is a signed divide by four, so -(x / 4) and x / -4 agree.
//  __fastcall stands for the original's __thiscall (the same ABI for one register argument).

//  The enum values are the jump table's index: the order of the four labels is the order of these values.
static_assert(PANEL_NONE == 0 && PANEL_RIGHT == 1
              && PANEL_LEFT == 2 && PANEL_BOTH == 3,
              "SCROLL_SetWindowMode indexes two jump tables with D2C_UIpanels");

void __fastcall SCROLL_SetWindowMode(D2C_UIpanels nPanel)
{
    int32_t nAdjust;
    int32_t nQuarter;

    switch (nPanel)
    {
    case PANEL_NONE:
    case PANEL_BOTH:
        nAdjust = 0;
        break;

    case PANEL_RIGHT:
        nAdjust = -(gnScreenWidth / 4);
        break;

    case PANEL_LEFT:
        nAdjust = gnScreenWidth / 4;
        break;

    default:
        FOG_DisplayHalt("Setting scroll window mode", szSourceFile, 93);
        exit(-1);
    }

    gUiPlayerPosAdjust = nAdjust;
    gUiShiftScreen = nPanel;
    D2GFX_SetViewMatrixAdjust(nAdjust);

    nQuarter = gnViewportRight / 4;

    switch (nPanel)
    {
    case PANEL_NONE:
        VIEW_SetRect(gpView, 0, 0, gnViewportRight, gnViewportBottom);
        break;

    case PANEL_RIGHT:
        VIEW_SetRect(gpView, -nQuarter, 0, gnViewportRight - nQuarter, gnViewportBottom);
        break;

    case PANEL_LEFT:
        VIEW_SetRect(gpView, nQuarter, 0, gnViewportRight + nQuarter, gnViewportBottom);
        break;

    case PANEL_BOTH:
        VIEW_SetRect(gpView, 0, 0, gnViewportRight, gnViewportBottom);
        GAME_ResetMouseButtonState();
        break;
    }
}


//1.10f: D2Client.0x6FAB5870
//  Raw X origin, gnViewportOriginX. gnViewportOriginX/Y are the client-space point drawn at the screen's top-left, recomputed once a frame
//  by SCROLL_UpdateViewportOrigin. The drawing code asks the adjusted getters (X minus the panel slide, Y minus 8); only AUTOMAP_Draw asks
//  the raw ones.
int32_t __cdecl SCROLL_GetViewportOriginXRaw()
{
    return gnViewportOriginX;
}

//1.10f: D2Client.0x6FAB5880
//  Raw Y origin, gnViewportOriginY.
int32_t __cdecl SCROLL_GetViewportOriginYRaw()
{
    return gnViewportOriginY;
}

//1.10f: D2Client.0x6FAB5890
//  Returns gnViewportOriginX minus gUiPlayerPosAdjust (0 or +/- a quarter of the screen width, set by SCROLL_SetWindowMode). A point
//  measured from this origin lands gUiPlayerPosAdjust further right: a quarter screen away from the open panel.
int32_t __cdecl SCROLL_GetViewportOriginX()
{
    return gnViewportOriginX - gUiPlayerPosAdjust;
}

//1.10f: D2Client.0x6FAB58A0
//  Returns gnViewportOriginY plus the literal -8; the vertical origin does not follow the panels.
int32_t __cdecl SCROLL_GetViewportOriginY()
{
    return gnViewportOriginY - 8;
}

//1.10f: D2Client.0x6FAB58B0
//  A screen pixel to a game subtile, in place. With the perspective view on (D2GFX_CheckPerspective) the click is first undone out of the
//  perspective projection; the arithmetic is all signed. `nY + nY / -40` is `nY - nY / 40` (C truncates toward zero), written as the
//  division the constant 0x99999999 encodes. The horizontal factor `40 - nY'/4` is 40 on the top row, zero where nY' is 160..163 and
//  negative below, so the correction pushes a click away from the vertical centre line near the top of the screen and pulls it toward the
//  line near the bottom.
//  Then the viewport origin is added (X as SCROLL_GetViewportOriginX does, Y without its -8) and D2Common turns client space into
//  subtiles. Both results come back through the pointers; void: the caller overwrites eax.
void __fastcall SCROLL_ScreenToGameCoords(int32_t *pnX, int32_t *pnY)
{
    int32_t nX = *pnX;
    int32_t nY = *pnY;
    int32_t nScreenYCorrected;
    int32_t nHalfWidth;

    nScreenYCorrected = nY;
    if (D2GFX_CheckPerspective())
    {
        nScreenYCorrected = nY + nY / -40;
        nHalfWidth = gnScreenWidth / 2;
        nX += (40 - nScreenYCorrected / 4) * (nX - nHalfWidth) / nHalfWidth;
    }

    nY = gnViewportOriginY + nScreenYCorrected;
    nX += gnViewportOriginX - gUiPlayerPosAdjust;
    D2COMMON_DUNGEON_ClientToGameSubtileCoords(&nX, &nY);

    *pnX = nX;
    *pnY = nY;
}

//1.10f: D2Client.0x6FAB5960
//  Computes where the world is drawn this frame:
//      gnViewportOriginX = playerX - gnScreenWidth / 2
//      gnViewportOriginY = playerY - gnScreenHeight / 2 + 16
//  A screen shake, when one is running, is added to both.
//  BUG: gnUiShiftOffsetX is stored every frame (-160 first, then gnScreenWidth / 2 - 160 for PANEL_LEFT and 0 for PANEL_BOTH) and never
//  read. INERT: nothing in D2Client reads it; the three stores are a dead computation, reproduced.
//  BUG: the player is not tested; CUNIT_GetPlayer's answer goes straight to D2Common #10333 and #10334, which exit on NULL. LATENT: the one
//  caller, GAME_DrawGameScene, hands the same player to #10333 untested earlier in the frame, so a NULL player halts there first, and
//  nothing writes gpPlayerUnit in between.
//  void: GAME_DrawGameScene calls SCROLL_GetUiShiftScreen next, which overwrites eax.
void __cdecl SCROLL_UpdateViewportOrigin()
{
    D2C_UIpanels nPanel;
    D2UnitStrc *pPlayer;
    int32_t nPlayerX;
    int32_t nPlayerY;
    int32_t nShakeX;
    int32_t nShakeY;

    nPanel = gUiShiftScreen;
    gnUiShiftOffsetX = -160;
    if (nPanel == PANEL_LEFT)
        gnUiShiftOffsetX = gnScreenWidth / 2 - 160;
    else if (nPanel == PANEL_BOTH)
        gnUiShiftOffsetX = 0;

    pPlayer = CUNIT_GetPlayer();
    nPlayerX = D2COMMON_UNITS_GetClientCoordX(pPlayer);
    nPlayerY = D2COMMON_UNITS_GetClientCoordY(pPlayer);
    gnViewportOriginX = nPlayerX - gnScreenWidth / 2;
    gnViewportOriginY = nPlayerY - gnScreenHeight / 2 + 16;

    if (VIEWEFFECTS_GetScreenShakeOffset(&nShakeX, &nShakeY))
    {
        gnViewportOriginX += nShakeX;
        gnViewportOriginY += nShakeY;
    }
}

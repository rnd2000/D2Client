#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Lng.h"
#include "CoreArchive.h"
#include "Restart.h"

//  minipanel.cpp -- the mini panel of D2Client.dll 1.10 and three registry settings: loading the "Resolution" value, loading and saving
//  "Always Run", drawing the panel (the layout, the background, the buttons, the hint of the button under the cursor), its mouse handlers
//  (gMinipanelMessageHandlers: press and release), the button actions, loading and unloading the artwork, and the two rectangle tests
//  MINIPANEL_IsPointOverMiniPanelRow and MINIPANEL_WaypointIsMouseOverMiniPanel.
//  Sixteen functions, address range 0x6FB08D90..0x6FB09D46. No assert in this unit names the original's source file, so the file name is ours.

//1.10f: D2Client.0x6FB09C90
//  Reads the "Resolution" value of the registry key "Diablo II" (one SRegLoadValue call) and applies it: a value of exactly 1 selects
//  D2GAMERES_800x600 (the 2 the original passes, `mov ecx, 2`), anything else D2GAMERES_640x480 (`xor ecx, ecx`). Called by
//  SCMD_HandleSrv61_PlayCinematic and GAME_InitGame.
static_assert(D2GAMERES_640x480 == 0,
              "WNDPROC_SetResolutionMode's default arm is `xor ecx, ecx`");
static_assert(D2GAMERES_800x600 == 2,
              "WNDPROC_SetResolutionMode's high arm is `mov ecx, 2` at 0x6FB09CB0");

void __cdecl MINIPANEL_LoadResolutionFromRegistry()
{
    //  BUG: the original does not initialise this slot: its whole prologue is push ecx / lea eax, [esp+4+pValue] (an initialiser would be
    //  push 0), so the slot goes into SRegLoadValue holding whatever was in ecx. SRegLoadValue writes it only when its read succeeds with
    //  type 1 or 4, so with no "Resolution" value the game starts at 800x600 only if that leftover is exactly 1. REACHABLE: the leftover is a
    //  callee's at both call sites (GAME_InitGame, SCMD_HandleSrv61_PlayCinematic's tail jump); its outcome NOT ESTABLISHED.
    //  DIFFERENCE: this C reads its own uninitialised local, not the caller's ecx, so with no value it compares whatever our frame holds --
    //  the original's defect with our frame's leftover in place of its ecx. Kept on purpose.
    uint32_t nValue;
    D2GameResolutionMode nMode;

    STORM_SRegLoadValue("Diablo II", "Resolution", 0, &nValue);

    nMode = D2GAMERES_640x480;
    if (nValue == 1)
        nMode = D2GAMERES_800x600;

    WNDPROC_SetResolutionMode(nMode);
}


static const char szAlwaysRun[] = "Always Run";                 // 0x6FB8CC24


//1.10f: D2Client.0x6FB08D90
//  Reads "Always Run" and hands it to the run toggle (GAME_SetRunToggled); the unit's first function. Its one reference is a tail jump, the
//  last thing UI_InitUiForPlayerUnit does. Unlike MINIPANEL_LoadResolutionFromRegistry the slot is initialised (to 0), so the toggle starts
//  off unless Storm writes the slot.
void __cdecl MINIPANEL_LoadAlwaysRunFromRegistry()
{
    uint32_t nAlwaysRun;

    nAlwaysRun = 0;
    STORM_SRegLoadValue(szKeyName, szAlwaysRun, 0, &nAlwaysRun);
    GAME_SetRunToggled(nAlwaysRun);
}


//1.10f: D2Client.0x6FB08DC0
//  Saves the run toggle as "Always Run"; one caller, UI_ResetUiSubsystems. The value is saved as it is -- not inverted, as
//  MINIPANEL_SaveAndUnload inverts "Mini Panel" -- and MINIPANEL_LoadAlwaysRunFromRegistry hands the same value back.
void __cdecl MINIPANEL_SaveAlwaysRunToRegistry()
{
    uint32_t nRunToggled;

    nRunToggled = GAME_GetRunToggled();
    STORM_SRegSaveValue(szKeyName, szAlwaysRun, 0, nRunToggled);
}


//  Window ids of MINIPANEL_Draw's chains that no other file pins.
//  The same x offsets recur across the drawing functions and the two click handlers, with no shared helper: w/2 - 205, - 202, - 84, - 74,
//  + 35, + 56, + 57, and the strip offsets -118 and +119. Each function computes its own.
static_assert(UI_STEEGSTONE == 0x1B && UI_GUILDVAULT == 0x1C && UI_UNKNOWN29 == 0x1D,
              "MINIPANEL_Draw asks `mov ecx, 1Bh` (0x6FB08ECC), `mov ecx, 1Ch` "
              "(0x6FB08E38) and `mov ecx, 1Dh` (0x6FB08E4A)");


//1.10f: D2Client.0x6FB08DE0
//  Draws the mini panel in one of three places, or hides it; one caller, UIPANELS_DrawInGamePanels, while UI_MINIPANEL is on. The
//  placements: layout 1 left of centre, beside the inventory side; layout 2 centred, when nothing is open; layout 3 right of centre, beside a
//  left-hand window. With both sides open it hides the panel: 0 goes to the four helpers it calls there, and each of them does nothing with it.
//  LayOutButtons fills the four button tables everything else reads, DrawBackgroundSegment draws the panel's bitmap, DrawButtons the buttons,
//  DrawButtonHint the label of the one under the cursor. The original inlines MINIPANEL_DrawBackgroundSegment for layouts 3 and 2; layout 3's
//  buttons are not routed into DrawButtons(3): the original calls MINIPANEL_DrawButtonCel from a loop here, where DrawButtons carries it inlined.
//  Two questions, each kept twice. bWindowOpen and gbMiniPanelWindowOpen: fifteen window states, in the order the original asks them (the
//  trade screen, the stash, the cube, the shop, the guild vault, 29, the message log, the skill selector, the ini scroll, the party screen,
//  the mercenary's inventory, the stat sheet, the quest screen, the waypoints, the Steeg stone), then the mercenary's inventory again, which
//  can only repeat a 1 the chain already wrote. bInvSideOpen and gbMiniPanelInvSideOpen: eight states, every one of them also in the first
//  chain; then the belt, when UI_BELTROWS or PANEL_InvHintPhaseSuppressed holds and PANEL_BeltGetRowCount() is above 1 (signed); then the
//  inventory or the skill tree -- whose else clears the global and leaves the local.
//  So the pair parts in exactly one case, the one the click handlers need: the belt's extra rows with neither the inventory nor the skill
//  tree up draw layout 1 and leave gbMiniPanelInvSideOpen 0, because the handlers add the belt's -118 themselves, from
//  PANEL_InvHintPhaseSuppressed.
//      window  inv side
//      no      no       layout 2: the bitmap in the centre, LayOutButtons(2), DrawButtons(1) (it asks only for non-zero) and the hint
//      no      yes      layout 1: the four helpers with 1
//      yes     no       layout 3: the bitmap at the right, LayOutButtons(3), a loop that calls MINIPANEL_DrawButtonCel, the hint
//      yes     yes      hidden: the four helpers with 0, and gbMiniPanelHidden set; 0 in the other three
//  BUG: the handlers shift their strip -118 for the belt's rows and -118 again for gbMiniPanelInvSideOpen, so with the inventory or the skill
//  tree up while the belt's rows show they test (w/2 - 320, w/2 - 147), and this draws layout 1, whose buttons run from w/2 - 202 to
//  w/2 - 35 (- 56 in single player). REACHABLE: the hint follows the layout; the click answers on the first two buttons and the left twelve
//  columns of the third, and on none from the fourth on. The rows stay up away from the belt only when the Show Belt command pins them, and
//  gaUiToggleRules lets pinned rows and the inventory coexist in either order.
void __cdecl MINIPANEL_Draw()
{
    int32_t bWindowOpen;
    int32_t bInvSideOpen;
    int32_t nButtonCount;
    int32_t i;

    bWindowOpen = FALSE;
    if (UI_GetUiWindowState(UI_MPTRADE) || UI_GetUiWindowState(UI_STASH)
        || UI_GetUiWindowState(UI_CUBE) || UI_GetUiWindowState(UI_NPCSHOP)
        || UI_GetUiWindowState(UI_GUILDVAULT) || UI_GetUiWindowState(UI_UNKNOWN29)
        || UI_GetUiWindowState(UI_MSGLOG) || UI_GetUiWindowState(UI_MINISKILL)
        || UI_GetUiWindowState(UI_INISCROLL) || UI_GetUiWindowState(UI_PARTYSCREEN)
        || UI_GetUiWindowState(UI_MERCINV) || UI_GetUiWindowState(UI_STATSCREEN)
        || UI_GetUiWindowState(UI_QUESTSCREEN) || UI_GetUiWindowState(UI_WAYPOINT)
        || UI_GetUiWindowState(UI_STEEGSTONE))
    {
        bWindowOpen = TRUE;
        gbMiniPanelWindowOpen = 1;
    }
    else
    {
        gbMiniPanelWindowOpen = 0;
    }

    //  asked a second time; the chain above has already answered it
    if (UI_GetUiWindowState(UI_MERCINV))
    {
        bWindowOpen = TRUE;
        gbMiniPanelWindowOpen = 1;
    }

    bInvSideOpen = FALSE;
    if (UI_GetUiWindowState(UI_MPTRADE) || UI_GetUiWindowState(UI_STASH) || UI_GetUiWindowState(UI_GUILDVAULT) || UI_GetUiWindowState(UI_CUBE) || UI_GetUiWindowState(UI_NPCSHOP) || UI_GetUiWindowState(UI_MSGLOG)
        || UI_GetUiWindowState(UI_STEEGSTONE) || UI_GetUiWindowState(UI_MINISKILL))
    {
        bInvSideOpen = TRUE;
        gbMiniPanelInvSideOpen = 1;
    }

    if ((UI_GetUiWindowState(UI_BELTROWS) || PANEL_InvHintPhaseSuppressed()) && PANEL_BeltGetRowCount() > 1)
    {
        bInvSideOpen = TRUE;
        gbMiniPanelInvSideOpen = 1;
    }

    if (UI_GetUiWindowState(UI_INVENTORY) || UI_GetUiWindowState(UI_SKILLTREE))
    {
        bInvSideOpen = TRUE;
        gbMiniPanelInvSideOpen = 1;
    }
    else
    {
        //  the global only: bInvSideOpen keeps what the tests above set
        gbMiniPanelInvSideOpen = 0;
    }

    if (bWindowOpen)
    {
        if (bInvSideOpen)
        {
            MINIPANEL_DrawBackgroundSegment(0);
            MINIPANEL_LayOutButtons(0);
            MINIPANEL_DrawButtons(0);
            MINIPANEL_DrawButtonHint(0);
            gbMiniPanelHidden = 1;
            return;
        }

        MINIPANEL_DrawBackgroundSegment(3);
        MINIPANEL_LayOutButtons(3);

        nButtonCount = 8;
        if (gbMiniPanelSinglePlayer)
            nButtonCount = 7;
        for (i = 0; i < nButtonCount; ++i)
        {
            if (gbMiniPanelButtonPressed[i] == 1)
                MINIPANEL_DrawButtonCel(gaMiniPanelButtonFrame[i] + 1, gaMiniPanelButtonX[i], gaMiniPanelButtonY[i], 1);
            else
                MINIPANEL_DrawButtonCel(gaMiniPanelButtonFrame[i], gaMiniPanelButtonX[i], gaMiniPanelButtonY[i], 1);
        }

        MINIPANEL_DrawButtonHint(3);
        gbMiniPanelHidden = 0;
        return;
    }

    if (bInvSideOpen)
    {
        MINIPANEL_DrawBackgroundSegment(1);
        MINIPANEL_LayOutButtons(1);
        MINIPANEL_DrawButtons(1);
        MINIPANEL_DrawButtonHint(1);
        gbMiniPanelHidden = 0;
        return;
    }

    MINIPANEL_DrawBackgroundSegment(2);
    MINIPANEL_LayOutButtons(2);
    MINIPANEL_DrawButtons(1);
    MINIPANEL_DrawButtonHint(2);
    gbMiniPanelHidden = 0;
}


//1.10f: D2Client.0x6FB09180
//  Draws the panel's bitmap at one of the three places: one cel of gpImgMiniPanel (frame 0, the block is cleared) at
//      1   w/2 - 205                                   left
//      2   w/2 - 74 - 3 single player, w/2 - 84 - 3     centre
//      3   w/2 + 56 - 2 single player, w/2 + 35 - 2     right
//  all on y = h - 47, gamma -1, draw mode 5. 0 draws nothing, and nor does anything above 3: the clear happens for any non-zero segment, then
//  `dec eax` three times finds no arm. MINIPANEL_Draw calls it with 1 and with 0; its layouts 2 and 3 draw the same arms at the site.
//  Panel\minipanel_s is a different file from Panel\minipanel (MINIPANEL_LoadImages), and every x here is its own literal.
void __fastcall MINIPANEL_DrawBackgroundSegment(int32_t nSegment)
{
    D2GfxDataStrc tData;
    int32_t nX;

    if (!nSegment)
        return;

    memset(&tData, 0, sizeof(tData));
    tData.pCellFile = gpImgMiniPanel;

    switch (nSegment)
    {
    case 1:
        D2GFX_TEXTURE_CelDraw(&tData, gnScreenWidth / 2 - 205, gnScreenHeight - 47, 0xFFFFFFFF, 5, NULL);
        break;

    case 2:
        if (gbMiniPanelSinglePlayer)
            nX = gnScreenWidth / 2 - 74;
        else
            nX = gnScreenWidth / 2 - 84;
        D2GFX_TEXTURE_CelDraw(&tData, nX - 3, gnScreenHeight - 47, 0xFFFFFFFF, 5, NULL);
        break;

    case 3:
        if (gbMiniPanelSinglePlayer)
            nX = gnScreenWidth / 2 + 56;
        else
            nX = gnScreenWidth / 2 + 35;
        D2GFX_TEXTURE_CelDraw(&tData, nX - 2, gnScreenHeight - 47, 0xFFFFFFFF, 5, NULL);
        break;
    }
}


//1.10f: D2Client.0x6FB09270
//  One button cel: frame nFrame of gpImgMiniPanelButtons at (nXPos, nYPos) through a cleared D2GfxDataStrc, gamma -1, draw mode 5.
//  MINIPANEL_Draw calls it once, for layout 3's buttons; MINIPANEL_DrawButtons holds two copies of it.
//  nUnused is never read, and it is there: retn 8 purges two stack arguments, nYPos and this one, and the one caller pushes both.
void __fastcall MINIPANEL_DrawButtonCel(int32_t nFrame, int32_t nXPos, int32_t nYPos, int32_t nUnused)
{
    D2GfxDataStrc tData;

    (void)nUnused;

    memset(&tData, 0, sizeof(tData));
    tData.pCellFile = gpImgMiniPanelButtons;
    tData.nFrame = nFrame;
    D2GFX_TEXTURE_CelDraw(&tData, nXPos, nYPos, 0xFFFFFFFF, 5, NULL);
}


//1.10f: D2Client.0x6FB092B0
//  Fills the four eight-slot tables the draws, the hint and both click handlers read -- gaMiniPanelButtonX, gaMiniPanelButtonY,
//  gaMiniPanelButtonId and gaMiniPanelButtonFrame -- for 8 buttons, 7 in single player, 21 pixels apart on y = h - 50. Segment 0 writes nothing.
//  The first button's x, by segment:
//      1   w/2 - 202                    the bitmap's - 205, plus 3
//      2   w/2 - 74 single player,      the bitmap's own origin before its
//          w/2 - 84 otherwise           - 3, so 3 in from its left edge
//      3   w/2 + 57 single player,      the bitmap is at + 56 - 2 and + 35
//          w/2 + 35 otherwise           - 2: 3 in, and 2 in for multiplayer
//  and any other non-zero segment takes the centre's. The single-player flag is read once, and that copy decides the count, the x and the skip.
//  Single player skips the party screen: from index 3 on the id is one higher and the frame two higher, so its seven buttons are ids 0 1 2 4 5
//  6 7 on frames 0 2 4 8 10 12 14; multiplayer's eight are ids 0..7 on frames 0..14 in twos. MINIPANEL_RunButtonAction and the hint switch on
//  the id; the cel file is indexed by the frame, whose odd neighbour is the pressed cel (MINIPANEL_DrawButtons).
void __fastcall MINIPANEL_LayOutButtons(int32_t nSegment)
{
    int32_t bSinglePlayer;
    int32_t nFrameSkip;
    int32_t nButtonCount;
    int32_t nIdSkip;
    int32_t nHalfWidth;
    int32_t nButtonX;
    int32_t nButtonY;
    int32_t i;

    bSinglePlayer = gbMiniPanelSinglePlayer;
    nFrameSkip = 0;
    nButtonCount = 8;
    if (bSinglePlayer)
        nButtonCount = 7;
    nIdSkip = 0;

    if (!nSegment)
        return;

    nHalfWidth = gnScreenWidth / 2;
    nButtonX = nHalfWidth - 84;
    if (bSinglePlayer)
        nButtonX = nHalfWidth - 74;
    nButtonY = gnScreenHeight - 50;

    if (nSegment == 1)
    {
        nButtonX = nHalfWidth - 202;
    }
    else if (nSegment == 3)
    {
        nButtonX = nHalfWidth + 57;
        if (!bSinglePlayer)
            nButtonX = nHalfWidth + 35;
    }

    gaMiniPanelButtonX[0] = nButtonX;
    gaMiniPanelButtonId[0] = 0;
    gaMiniPanelButtonFrame[0] = 0;
    gaMiniPanelButtonY[0] = nButtonY;
    nButtonX += 21;

    for (i = 1; i < nButtonCount; ++i)
    {
        gaMiniPanelButtonX[i] = nButtonX;
        gaMiniPanelButtonY[i] = nButtonY;
        if (bSinglePlayer)
        {
            if (i == 3)
            {
                nFrameSkip += 2;
                ++nIdSkip;
            }
            gaMiniPanelButtonId[i] = nIdSkip + i;
            gaMiniPanelButtonFrame[i] = nFrameSkip + 2 * i;
        }
        else
        {
            gaMiniPanelButtonId[i] = i;
            gaMiniPanelButtonFrame[i] = 2 * i;
        }
        nButtonX += 21;
    }
}


//1.10f: D2Client.0x6FB09390
//  Draws the buttons LayOutButtons placed: frame gaMiniPanelButtonFrame[i], or the next one while gbMiniPanelButtonPressed[i] is exactly 1 (a
//  press MINIPANEL_OnLButtonDown armed and the release has not yet cleared). Segment 0 draws nothing; the count is 8 unless the single-player
//  flag makes it 7. Both arms are MINIPANEL_DrawButtonCel inlined, and the compiler merged their tails: each arm clears a D2GfxDataStrc of its
//  own, and both jump to one CelDraw call, with the palette, the draw mode and the gamma pushed once, before the branch on the pressed test.
void __fastcall MINIPANEL_DrawButtons(int32_t nSegment)
{
    int32_t nButtonCount;
    int32_t i;

    nButtonCount = 8;
    if (!nSegment)
        return;

    if (gbMiniPanelSinglePlayer)
        nButtonCount = 7;

    for (i = 0; i < nButtonCount; ++i)
    {
        if (gbMiniPanelButtonPressed[i] == 1)
            MINIPANEL_DrawButtonCel(gaMiniPanelButtonFrame[i] + 1, gaMiniPanelButtonX[i], gaMiniPanelButtonY[i], 1);
        else
            MINIPANEL_DrawButtonCel(gaMiniPanelButtonFrame[i], gaMiniPanelButtonX[i], gaMiniPanelButtonY[i], 1);
    }
}


static_assert(STR_IDX_4169_minipanelchar == 0x1049 && STR_IDX_4170_minipanelinv == 0x104A
                  && STR_IDX_4171_minipaneltree == 0x104B
                  && STR_IDX_4172_minipanelparty == 0x104C
                  && STR_IDX_4173_minipanelautomap == 0x104D
                  && STR_IDX_4174_minipanelmessage == 0x104E
                  && STR_IDX_4175_minipanelquest == 0x104F
                  && STR_IDX_4176_minipanelmenubtn == 0x1050,
              "MINIPANEL_DrawButtonHint's eight arms load `mov ebx, 1049h` .. "
              "`mov ebx, 1050h` (0x6FB0954D..0x6FB095BE), one per button id");
static_assert(HOTKEY_STATSCREEN == 0 && HOTKEY_INVENTORY == 1 && HOTKEY_PARTY == 2
                  && HOTKEY_MSGLOG == 3 && HOTKEY_QUESTS == 4 && HOTKEY_SKILLTREE == 0x0C,
              "the same arms' commands: `xor esi, esi`, `mov esi, edi` with edi 1, "
              "`mov esi, 0Ch`, 2, 3 and 4; HOTKEY_AUTOMAP (7) is pinned in text.cpp "
              "and HOTKEY_MAX_COUNT (39h) in config.cpp");


//1.10f: D2Client.0x6FB09460
//  Draws the label of the button under the cursor; called by MINIPANEL_Draw in every arm with the layout it drew, and with 0 when the panel is
//  hidden, which draws nothing after the mouse and the count are read.
//  The strip is the click handlers': centre w/2, -118 for layout 1 and +119 for layout 3, and (centre - 84, centre + 89) by (h - 69, h - 47)
//  around it -- the offset taken here from the layout, where the handlers take it from PANEL_InvHintPhaseSuppressed and the two globals.
//  Inside the strip each button is asked in turn by x alone, (x, x + 20); the walk does not stop at a hit, and at the 21-pixel pitch there is
//  only one to find. The hit button's id picks the tbl string and the command:
//      0  4169 minipanelchar       HOTKEY_STATSCREEN
//      1  4170 minipanelinv        HOTKEY_INVENTORY
//      2  4171 minipaneltree       HOTKEY_SKILLTREE
//      3  4172 minipanelparty      HOTKEY_PARTY
//      4  4173 minipanelautomap    HOTKEY_AUTOMAP
//      5  4174 minipanelmessage    HOTKEY_MSGLOG
//      6  4175 minipanelquest      HOTKEY_QUESTS
//      7  4176 minipanelmenubtn    none: the flag stays 0 and the command HOTKEY_MAX_COUNT
//  An id outside 0..7 draws nothing (the original's unsigned `ja` takes a negative one too; the C's int switch compiles to the same test).
//  The label is the string, then " (%s)" -- tbl 4178, minipanelspecial -- with the name of each key bound to the command, the primary first:
//  the shape HELP_DrawHelpHint builds for the help button, with CMDTBL_ControlsGetBoundKeyNameFallback where that one calls
//  CMDTBL_ControlsGetBoundKeyName. The flag and the command are tested together, though every id that sets one sets the other. The two
//  buffers are declared where the original constructs them: the label, 200 Unicode, on every hit, and the key suffix, 30, only when a command
//  is asked; each suffix is cleared (`rep stosd`, 0Fh dwords) before its sprintf. It is drawn by D2Win #10129 at (x - 3, y - 26), colour 0,
//  last argument 1.
//  BUG: the label is copied and appended with no bound. LATENT: each suffix appends at most 29 units, and the longest of these tbl strings in
//  the stock language packs is 21 bytes, so a label reaches 79 units of its 200.
void __fastcall MINIPANEL_DrawButtonHint(int32_t nLayout)
{
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nOffsetX;
    int32_t nButtonCount;
    int32_t nStripCenterX;
    int32_t nButtonX;
    int32_t nButtonY;
    int32_t bNamesKeys;
    D2HotkeyIndices eCmd;
    D2C_StringIndices nStringId;
    const Unicode *pwszKeyName;
    const Unicode *pwszFormat;
    int32_t i;

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    nOffsetX = 0;
    nButtonCount = 8;
    if (gbMiniPanelSinglePlayer)
        nButtonCount = 7;

    if (!nLayout)
        return;

    if (nLayout == 1)
        nOffsetX = -118;
    else if (nLayout == 3)
        nOffsetX = 119;

    nStripCenterX = gnScreenWidth / 2 + nOffsetX;
    if (nMouseX <= nStripCenterX - 84 || nMouseX >= nStripCenterX + 89 || nMouseY <= gnScreenHeight - 69 || nMouseY >= gnScreenHeight - 47)
    {
        return;
    }

    for (i = 0; i < nButtonCount; ++i)
    {
        nButtonX = gaMiniPanelButtonX[i];
        if (nMouseX <= nButtonX || nMouseX >= nButtonX + 20)
            continue;

        nButtonY = gaMiniPanelButtonY[i];
        bNamesKeys = FALSE;
        eCmd = HOTKEY_MAX_COUNT;
        switch (gaMiniPanelButtonId[i])
        {
        case 0:
            nStringId = STR_IDX_4169_minipanelchar;
            bNamesKeys = TRUE;
            eCmd = HOTKEY_STATSCREEN;
            break;

        case 1:
            nStringId = STR_IDX_4170_minipanelinv;
            bNamesKeys = TRUE;
            eCmd = HOTKEY_INVENTORY;
            break;

        case 2:
            nStringId = STR_IDX_4171_minipaneltree;
            bNamesKeys = TRUE;
            eCmd = HOTKEY_SKILLTREE;
            break;

        case 3:
            nStringId = STR_IDX_4172_minipanelparty;
            bNamesKeys = TRUE;
            eCmd = HOTKEY_PARTY;
            break;

        case 4:
            nStringId = STR_IDX_4173_minipanelautomap;
            bNamesKeys = TRUE;
            eCmd = HOTKEY_AUTOMAP;
            break;

        case 5:
            nStringId = STR_IDX_4174_minipanelmessage;
            bNamesKeys = TRUE;
            eCmd = HOTKEY_MSGLOG;
            break;

        case 6:
            nStringId = STR_IDX_4175_minipanelquest;
            bNamesKeys = TRUE;
            eCmd = HOTKEY_QUESTS;
            break;

        case 7:
            nStringId = STR_IDX_4176_minipanelmenubtn;
            break;

        default:
            continue;
        }

        {
            Unicode wszText[200];

            Unicode::strcpy(wszText, D2LANG_GetStringFromTblIndex(nStringId));
            if (bNamesKeys && eCmd != HOTKEY_MAX_COUNT)
            {
                Unicode wszKeySuffix[30];

                if ((uint16_t)CMDTBL_ControlsFindBoundKey(eCmd, TRUE) != 0xFFFF)
                {
                    memset(wszKeySuffix, 0, sizeof(wszKeySuffix));
                    pwszKeyName = CMDTBL_ControlsGetBoundKeyNameFallback(eCmd, TRUE);
                    pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_4178_minipanelspecial);
                    Unicode::sprintf(30, wszKeySuffix, pwszFormat, pwszKeyName);
                    Unicode::strcat(wszText, wszKeySuffix);
                }
                if ((uint16_t)CMDTBL_ControlsFindBoundKey(eCmd, FALSE) != 0xFFFF)
                {
                    memset(wszKeySuffix, 0, sizeof(wszKeySuffix));
                    pwszKeyName = CMDTBL_ControlsGetBoundKeyNameFallback(eCmd, FALSE);
                    pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_4178_minipanelspecial);
                    Unicode::sprintf(30, wszKeySuffix, pwszFormat, pwszKeyName);
                    Unicode::strcat(wszText, wszKeySuffix);
                }
            }
            D2WIN_10129_DrawFramedText(wszText, nButtonX - 3, nButtonY - 26, 0, 1);
        }
    }
}


//  the three artwork names and the registry value, in the order MINIPANEL_LoadImages pushes them
static const char szPanelMiniPanel[] = "Panel\\minipanel";      // 0x6FB8CC70
static const char szPanelMiniPanelS[] = "Panel\\minipanel_s";   // 0x6FB8CC5C
static const char szPanelMiniPanelBtn[] = "Panel\\minipanelbtn";// 0x6FB8CC48
static const char szMiniPanel[] = "Mini Panel";                 // 0x6FB8CC80

//  the debug print that shipped
static const char szDbgQuestLogToggle[] = "Panel quest log toggle\n";  // 0x6FB8CC30


//1.10f: D2Client.0x6FB099F0
//  WM_LBUTTONDOWN for the mini panel, the first record of gMinipanelMessageHandlers (registered by UI_RegisterPanelHandlers under
//  UI_MINIPANEL; no code caller; WM_LBUTTONUP, MINIPANEL_OnLButtonUp, is the second). It arms a button rather than acting: the action happens
//  on the release. A held item, a hidden panel or no player all stop it before the hit test. A click on a button claims the message whether or
//  not the button is armed; one inside the strip that misses every button returns having written nothing, as one outside the strip does.
//  The strip is placed by three offsets, and both handlers compute it from scratch, with no shared helper: start at 0; -118 when
//  PANEL_InvHintPhaseSuppressed() and the belt has more than one row (the belt's extra rows push the strip left); -118 again when the
//  inventory side is open; +119 when a window is open.
//      centre = gnScreenWidth / 2 + that
//      the strip is  centre - 84 .. centre + 89  x  height - 69 .. height - 47
//  The box is not symmetric: 84 left of centre and 89 right of it. This handler breaks out of its button scan as soon as a button contains
//  the point; MINIPANEL_OnLButtonUp does not, and would act on every button whose 20-pixel span contains the click. The buttons do not
//  overlap, so it acts at most once.
//  While UI_ESCMENU is open this handler will not arm a button unless the button is the menu itself (id 7), with one exception carved out by
//  index:  GAME_GetGameType() ? (bWithin = idx < 5) : (bWithin = idx < 4)  and the arm (the lit cel) happens only when !bWithin: the character
//  sheet to the automap are not lit, the message log, the quest log and the menu are. Multiplayer has the extra Party Screen button at index
//  3, which is why the count differs; both counts cover the same buttons. Every hit still plays sound 4, sets gbMiniPanelButtonDown and
//  claims the message. The arm does not decide the action: OnLButtonUp acts on the button under the release whether or not it was armed. What
//  keeps ids 0 to 5 from acting over the ESC menu is UI_SetUIToggle: gaUiToggleRules' row 9 is 2 (refuse) in all six of their windows'
//  columns. So the message-log button lights and does nothing, the quest-log button goes through QUESTLOG_TogglePanel, and the menu acts.
void __stdcall MINIPANEL_OnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    D2UnitStrc *pPlayer;
    D2InventoryStrc *pInventory;
    int32_t nXpos;
    int32_t nYpos;
    int32_t nOffsetX;
    int32_t nStripCenterX;
    int32_t nButtonIdx;
    int32_t nButtonX;
    int32_t nButtonCount;
    int32_t bWithinCount;

    nButtonCount = 8;
    if (gbMiniPanelSinglePlayer)
        nButtonCount = 7;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;

    pInventory = pPlayer->pInventory;
    if (!pInventory)
        return;

    if (D2COMMON_INVENTORY_GetCursorItem(pInventory))
        return;

    if (gbMiniPanelHidden)
        return;

    //  the player is fetched a second time and the second copy is what the type and mode tests below read
    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;

    nOffsetX = 0;
    if (PANEL_InvHintPhaseSuppressed() && PANEL_BeltGetRowCount() > 1)
        nOffsetX = -118;
    if (gbMiniPanelInvSideOpen)
        nOffsetX -= 118;
    if (gbMiniPanelWindowOpen)
        nOffsetX += 119;

    nStripCenterX = nOffsetX + gnScreenWidth / 2;
    if (nXpos <= nStripCenterX - 84 || nXpos >= nStripCenterX + 89 || nYpos <= gnScreenHeight - 69 || nYpos >= gnScreenHeight - 47)
    {
        return;
    }

    for (nButtonIdx = 0; ; ++nButtonIdx)
    {
        if (nButtonIdx >= nButtonCount)
            return;
        nButtonX = gaMiniPanelButtonX[nButtonIdx];
        if (nXpos > nButtonX && nXpos < nButtonX + 20)
            break;
    }

    if (gaMiniPanelButtonId[nButtonIdx] == 7 || (pPlayer->dwUnitType == UNIT_PLAYER && pPlayer->dwAnimMode != 17 && (!UI_GetUiWindowState(UI_ESCMENU) || (GAME_GetGameType() ? (bWithinCount = nButtonIdx < 5) : (bWithinCount = nButtonIdx < 4),
                    !bWithinCount))))
    {
        gbMiniPanelButtonPressed[nButtonIdx] = 1;
    }

    SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
    gbMiniPanelButtonDown = 1;
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FB09730
//  WM_LBUTTONUP for the mini panel, the half that acts. It returns immediately unless gbMiniPanelButtonDown is set, so a release the panel
//  did not see the press for does nothing, and the flag is cleared before every other test, so one press arms exactly one release.
//  A click that misses the strip writes pMsg->bReturn = 0 and does not break the chain, so it falls through to whatever is registered below.
//  A click that lands claims the message. gbMiniPanelFlagUnread is cleared on both early-out paths and read nowhere in the module.
void __stdcall MINIPANEL_OnLButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    D2UnitStrc *pPlayer;
    D2InventoryStrc *pInventory;
    int32_t nOffsetX;
    int32_t nButtonCount;
    int32_t nXpos;
    int32_t nYpos;
    int32_t nButtonScreenX;
    int32_t i;
    int32_t nButtonX;
    int32_t nButton;

    pPlayer = CUNIT_GetPlayer();
    nOffsetX = 0;

    if (!gbMiniPanelButtonDown)
        return;
    gbMiniPanelButtonDown = 0;

    if (!pPlayer)
        return;

    pInventory = pPlayer->pInventory;
    if (!pInventory || D2COMMON_INVENTORY_GetCursorItem(pInventory))
        return;

    if (CURSOR_GetCursorState() != 7)
        CURSOR_SetHeldUnit(NULL);

    nButtonCount = 8;
    if (gbMiniPanelSinglePlayer)
        nButtonCount = 7;

    if (gbMiniPanelHidden)
    {
        gbMiniPanelFlagUnread = 0;
        return;
    }

    //  fetched again, as in the press handler
    pPlayer = CUNIT_GetPlayer();
    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;

    memset(gbMiniPanelButtonPressed, 0, 4 * nButtonCount);

    if (PANEL_InvHintPhaseSuppressed() && PANEL_BeltGetRowCount() > 1)
        nOffsetX = -118;
    if (gbMiniPanelInvSideOpen)
        nOffsetX -= 118;
    if (gbMiniPanelWindowOpen)
        nOffsetX += 119;

    nButtonScreenX = gnScreenWidth / 2 + nOffsetX;
    if (nXpos <= nButtonScreenX - 84 || nXpos >= nButtonScreenX + 89 || nYpos <= gnScreenHeight - 69 || nYpos >= gnScreenHeight - 47)
    {
        pMsg->bReturn = 0;
        gbMiniPanelFlagUnread = 0;
        return;
    }

    //  no break: the scan runs to the end. The buttons do not overlap, so it acts at most once -- unlike the press handler, which stops
    for (i = 0; i < nButtonCount; ++i)
    {
        nButtonX = gaMiniPanelButtonX[i];
        if (nXpos > nButtonX && nXpos < nButtonX + 20)
        {
            nButton = gaMiniPanelButtonId[i];
            if (nButton == 7 || (pPlayer && pPlayer->dwUnitType == UNIT_PLAYER && pPlayer->dwAnimMode != 17))
            {
                MINIPANEL_RunButtonAction(nButton);
            }
        }
    }

    GAME_ResetClickState();
    GAME_ResetMouseButtonState();
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FB098D0
//  One button's action; sole caller MINIPANEL_OnLButtonUp. Six of the eight are the same three lines -- read the window's state and close it
//  or open it:
//      0 -> UI_STATSCREEN    1 -> UI_INVENTORY    2 -> UI_SKILLTREE
//      3 -> UI_PARTYSCREEN   4 -> UI_AUTOMAP      5 -> UI_MSGLOG
//  Two exceptions:
//    - button 3 returns without touching anything in a single-player game. That is a second guard, not the mechanism: in single player
//      MINIPANEL_LoadImages drops the count to 7 and the Party Screen id is not in gaMiniPanelButtonId at all, so the button does not exist
//    - button 5 closes UI_BELTROWS first if it is open, then toggles the message log -- because UI_SetUIToggle would not: gaUiToggleRules
//      does not close the rows when the log opens (row 31, column 24 is 0), though pinning the rows closes the log (row 24, column 31 is 1).
//      The log's own key, CMDTBL_ActionMessageLog, has no such step, so the two can be up together; this button keeps them apart
//  Button 6 is the quest log, and it carries the shipped debug print. Button 7 is the menu: close every panel, clear the toggle delay, set the
//  ESC menu's page, then re-enable and re-arm the delay -- the delay is dropped and rebuilt around the page change rather than left running.
//  The single argument is in ecx (the switch runs on ecx); edx is never read: every occurrence of edx in this function is a write. The
//  quest-log arm is a tail jump to QUESTLOG_TogglePanel after `xor ecx, ecx`, so edx arrives at the callee untouched, holding whatever
//  OutputDebugStringA left; the callee looks at ecx and nothing else.
void __fastcall MINIPANEL_RunButtonAction(int32_t nButton)
{
    int32_t bWindowOpen;
    D2C_UIvars nUiWindow;

    switch (nButton)
    {
    case 0:
        bWindowOpen = UI_GetUiWindowState(UI_STATSCREEN);
        nUiWindow = UI_STATSCREEN;
        break;

    case 1:
        bWindowOpen = UI_GetUiWindowState(UI_INVENTORY);
        nUiWindow = UI_INVENTORY;
        break;

    case 2:
        bWindowOpen = UI_GetUiWindowState(UI_SKILLTREE);
        nUiWindow = UI_SKILLTREE;
        break;

    case 3:
        if (GAME_GetGameType() == GAMETYPE_SINGLEPLAYER)
            return;
        bWindowOpen = UI_GetUiWindowState(UI_PARTYSCREEN);
        nUiWindow = UI_PARTYSCREEN;
        break;

    case 4:
        bWindowOpen = UI_GetUiWindowState(UI_AUTOMAP);
        nUiWindow = UI_AUTOMAP;
        break;

    case 5:
        if (UI_GetUiWindowState(UI_BELTROWS))
            UI_SetUIToggle(UI_BELTROWS, UI_TURNOFF, 0);
        bWindowOpen = UI_GetUiWindowState(UI_MSGLOG);
        nUiWindow = UI_MSGLOG;
        break;

    case 6:
        //  a debug print that survived into the retail binary; reproduced
        OutputDebugStringA(szDbgQuestLogToggle);
        QUESTLOG_TogglePanel(0);
        return;

    case 7:
        UI_CloseAllPanels(0, 0);
        UI_ClearPanelToggleDelay();
        ESCMENU_SetPage(1, 0);
        UI_EnablePanelToggleDelay();
        UI_ArmPanelToggleDelay();
        return;

    default:
        return;
    }

    if (bWindowOpen)
        UI_SetUIToggle(nUiWindow, UI_TURNOFF, 0);
    else
        UI_SetUIToggle(nUiWindow, UI_TOGGLE, 0);
}


//1.10f: D2Client.0x6FB09C50
//  Is the point over the mini panel's row; two callers, DIALOG_OnMouseDown and DIALOG_OnMouseUp, which use it to let a click through to the
//  panel underneath a dialog. It tests width/2 - 74 .. width/2 + 71 by height - 69 .. height - 50: the single-player centre row's buttons,
//  MINIPANEL_LayOutButtons' first at width/2 - 74 and the seventh answering up to width/2 + 71, less that last column. These X bounds are not
//  the background's: MINIPANEL_DrawBackgroundSegment draws the single-player centre bitmap at width/2 - 77.
//  BUG: this is a different rectangle from the one the mouse handlers use (centre - 84 .. centre + 89 by height - 69 .. height - 47) -- narrower
//  on both sides, shorter at the bottom, and it does not apply the three placement offsets at all, so with the belt extended or a window open
//  the two disagree about where the panel is. In multiplayer, whose centre row answers width/2 - 83 .. + 82, the first button's left ten
//  columns and the last button's right twelve are not let through: there an NPC dialog's click-away ends the conversation first. REACHABLE:
//  under a dialog the row test is single player's centre row.
BOOL __fastcall MINIPANEL_IsPointOverMiniPanelRow(int32_t nXPos, int32_t nYPos)
{
    return nXPos > gnScreenWidth / 2 - 74 && nXPos < gnScreenWidth / 2 + 71 && nYPos < gnScreenHeight - 50 && nYPos > gnScreenHeight - 69;
}


//1.10f: D2Client.0x6FB09B80
//  Loads the artwork and settles the button count; the one place gbMiniPanelSinglePlayer is written (from GAME_GetGameType()), and every
//  count in the unit reads it. Single player takes a different panel bitmap -- Panel\minipanel_s, with 7 buttons -- rather than the same one
//  with a button hidden; multiplayer takes Panel\minipanel with 8. Panel\minipanelbtn holds the buttons either way.
//  BUG: both handles are stored untested, and every draw hands them to D2Gfx untested where MINIPANEL_SaveAndUnload tests them. LATENT: the
//  three files ship with the game.
void __cdecl MINIPANEL_LoadImages()
{
    D2CellFileStrc *hPanelImage;
    int32_t nButtonCount;

    if (GAME_GetGameType())
    {
        hPanelImage = UI_LoadImageFromUiDir(szPanelMiniPanel);
        gbMiniPanelSinglePlayer = 0;
    }
    else
    {
        hPanelImage = UI_LoadImageFromUiDir(szPanelMiniPanelS);
        gbMiniPanelSinglePlayer = 1;
    }

    gpImgMiniPanel = hPanelImage;
    gpImgMiniPanelButtons = UI_LoadImageFromUiDir(szPanelMiniPanelBtn);

    nButtonCount = 8;
    if (gbMiniPanelSinglePlayer)
        nButtonCount = 7;

    memset(gbMiniPanelButtonPressed, 0, 4 * nButtonCount);
}


//1.10f: D2Client.0x6FB09BF0
//  Persists the setting and releases the art; called from UI_ShutdownUiSubsystems.
//  The saved value is inverted: a 1 reaches the registry when the in-memory state is zero, so the registry entry and the flag have opposite
//  senses.
//  ESCMENU_SetPage writes gEscMenuUiWindows[21].nSavedState every time the menu opens with bCloseUiWindows set: zeroed, then the minipanel's
//  state stored, because record 21, UI_MINIPANEL, has both flags set (both writes are (gEscMenuUiWindows+8)[esi]); ESCMENU_CloseMenu reads it
//  the same way. So the value saved here is whether the minipanel was on the last time the ESC menu hid the panels, not whether it is on now.
//  NOT ESTABLISHED: whether any exit path saves a stale value that way.
void __cdecl MINIPANEL_SaveAndUnload()
{
    STORM_SRegSaveValue(szKeyName, szMiniPanel, 1, gEscMenuUiWindows[21].nSavedState == 0);

    if (gpImgMiniPanel)
    {
        ARCHIVE_CellFileFree(gpImgMiniPanel);
        gpImgMiniPanel = NULL;
    }

    if (gpImgMiniPanelButtons)
    {
        ARCHIVE_CellFileFree(gpImgMiniPanelButtons);
        gpImgMiniPanelButtons = NULL;
    }
}


//1.10f: D2Client.0x6FB09CC0
//  Is the cursor over the mini panel while the waypoint panel is up; both callers are waypoint.cpp's, WAYPOINT_OnMouseUp and
//  WAYPOINT_OnMouseDown, each of which returns at once on a yes, consuming nothing, so the click goes on down the chain.
//  The mouse is read first, both axes, and only then the two window states, UI_WAYPOINT and UI_MINIPANEL; either closed is a no. The box is
//  layout 3's, the one MINIPANEL_Draw picks while a left-hand window such as the waypoint panel is open:
//      x in (nPanelLeft, nPanelLeft + 20 * nButtonCount)
//            nPanelLeft = w/2 + 56 single player, w/2 + 35 otherwise
//      y in (h - 76, h - 50)
//  The single-player flag is read once, for both the left edge and the count.
//  BUG: it is not the box the buttons answer in. Layout 3 puts the first button at w/2 + 57 in single player and w/2 + 35 otherwise, 21 apart,
//  and the click handlers take x in (button, button + 20). So 20 per button ends the box short of the last button by seven columns in either
//  mode: it stops before w/2 + 195 (multiplayer) and w/2 + 196 (single player), and the last button answers up to w/2 + 201 and w/2 + 202. In
//  y the handlers' strip is (h - 69, h - 47), this box (h - 76, h - 50): the strip's bottom three rows, h - 50 to h - 48, are outside it. On
//  those columns and rows the waypoint handlers do not step aside -- the press handler takes any x past w/2 as off its panel and closes it.
//  REACHABLE: the waypoint handlers are registered after the mini panel's and run first, so a click there closes the waypoint panel where one
//  column left or one row up it does not. The other way, the box's rows h - 75 to h - 69 above the strip, the gaps between buttons and, in
//  single player, column w/2 + 57 step the panel aside for a press no button takes.
//  BUG: it does not ask whether the panel is drawn: with the inventory side open as well, MINIPANEL_Draw hides it, and this still answers for
//  the box. REACHABLE: the waypoint panel opens over the inventory or the skill tree, and coexists with the belt's pinned rows.
BOOL __cdecl MINIPANEL_WaypointIsMouseOverMiniPanel()
{
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t bSinglePlayer;
    int32_t nPanelLeft;
    int32_t nRowBottom;
    int32_t nButtonCount;

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    if (!UI_GetUiWindowState(UI_WAYPOINT) || !UI_GetUiWindowState(UI_MINIPANEL))
        return FALSE;

    bSinglePlayer = gbMiniPanelSinglePlayer;
    if (bSinglePlayer)
        nPanelLeft = gnScreenWidth / 2 + 56;
    else
        nPanelLeft = gnScreenWidth / 2 + 35;
    nRowBottom = gnScreenHeight - 50;
    nButtonCount = 8;
    if (bSinglePlayer)
        nButtonCount = 7;

    return nMouseX > nPanelLeft && nMouseX < nPanelLeft + 20 * nButtonCount && nMouseY > nRowBottom - 26 && nMouseY < nRowBottom;
}

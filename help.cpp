#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "CoreArchive.h"
#include "inv.h"
#include "help.h"

//  help.cpp -- the in-game help screen of D2Client.dll 1.10: loading and drawing the three help cels, the two label layouts (800x600 and every
//  other resolution), and the mouse and key handlers of the help screen and the help button.
//  Address range 0x6FAD81F0..0x6FADA70B. No assert in this unit names the original's source file, so the file name is ours.

//  Registry names: the key "Diablo II" and the value "Help Menu" (written by HELP_HelpButtonOnLButtonUp, read by HELP_DrawHelpHint).
static const char szDiabloIIKey[] = "Diablo II";
static const char szHelpMenu[] = "Help Menu";


//1.10f: D2Client.0x6FAD81F0
//  Loads the three help cels: the border (gpHelpBackgroundCel; 800helpborder at 800x600, helpborder otherwise), the pointer (gpHelpPointerCel)
//  and the bullet (gpHelpBulletCel). It is called on every pass while the help screen is on; the border is freed and loaded again each time, so
//  it follows the resolution, while the pointer and the bullet are loaded once and stay.
//  BUG: a cel that fails to load stays NULL, and the draw functions pass it to D2GFX_TEXTURE_CelDraw untested. LATENT: all four .dc6 files
//  (800helpborder, helpborder, helpwhitebullet, helpyellowbullet) are in the stock install.
void __cdecl HELP_LoadImages()
{
    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600)
    {
        if (gpHelpBackgroundCel)
        {
            ARCHIVE_CellFileFree(gpHelpBackgroundCel);
            gpHelpBackgroundCel = 0;
        }
        gpHelpBackgroundCel = UI_LoadImageFromUiDir("Menu\\800helpborder");
    }
    else
    {
        if (gpHelpBackgroundCel)
        {
            ARCHIVE_CellFileFree(gpHelpBackgroundCel);
            gpHelpBackgroundCel = 0;
        }
        gpHelpBackgroundCel = UI_LoadImageFromUiDir("Menu\\helpborder");
    }

    if (!gpHelpPointerCel)
        gpHelpPointerCel = UI_LoadImageFromUiDir("Menu\\helpwhitebullet");
    if (!gpHelpBulletCel)
        gpHelpBulletCel = UI_LoadImageFromUiDir("Menu\\helpyellowbullet");
}


//1.10f: D2Client.0x6FAD8280
//  Returns the left edge that centres the tbl string nTblIdx on nCenterX; the width is halved by a signed divide. The string is copied into a
//  Unicode[100] only to be measured. The original inlines this function in most of the centred labels of HELP_DrawLabelsWide and
//  HELP_DrawLabels.
//  BUG: the copy has no length check, so a string of 100 units or more would overrun wszText. LATENT: only the help labels are passed, and the
//  longest of them in the six stock language packs checked is 78 bytes (English: 17).
int32_t __fastcall HELP_CenterTextX(D2C_StringIndices nTblIdx, int32_t nCenterX)
{
    Unicode wszText[100];

    Unicode::strcpy(wszText, D2LANG_GetStringFromTblIndex(nTblIdx));
    return nCenterX - D2WIN_10121_GetTextWidth(wszText) / 2;
}


//1.10f: D2Client.0x6FAD82E0
//  Dims the game behind the help screen: a half-transparent (DRAWMODE_TRANS50) box over the whole screen except the bottom 49 rows, in palette
//  index gnHelpDimColor.
//  BUG: gnHelpDimColor is never written (the loader zero-fills it), so the colour is always palette index 0; the call before it,
//  PANEL_SetBoxFillColorBlack, sets a different global (gnUiBoxFillColor) that this function does not read. HARMLESS: entry 0 of every palette
//  in the stock install is 000000, so the box is black whatever index a mix would have picked.
//  DIFFERENCE: the original pushes the colour as the whole register, whose upper bytes are PANEL_SetBoxFillColorBlack's leftovers; ours passes
//  the byte. Every render driver masks the colour to its byte, so nothing reads the difference.
void __cdecl HELP_DimGameScreen()
{
    PANEL_SetBoxFillColorBlack();
    GFX_DrawFilledRect(0, 0, gnScreenWidth, gnScreenHeight - 49, gnHelpDimColor, DRAWMODE_TRANS50);
}


//1.10f: D2Client.0x6FAD8310
//  Draws the help overlay. Turns the mini-panel on if it is off, recording that in gbHelpOpenedMiniPanel (HELP_CloseMiniPanelIfOpened reads it
//  to put the panel back), then draws one of two layouts. The 800x600 arm does not call HELP_DrawBackground and HELP_DrawLeaderLines: it draws
//  its own frame (seven frames of the border cel, where HELP_DrawBackground draws eight) and its own eleven pointers with line pairs
//  (HELP_DrawLeaderLines draws nine) at other positions, shares HELP_DrawCloseButton and HELP_DrawKeyBulletCels (inline there in the original),
//  and ends in HELP_DrawLabelsWide.
//  void: both arms end in a tail jump into a label pass, so the original leaves eax as that pass leaves it; the one caller does not read it.
void __cdecl HELP_DrawScreen()
{
    D2GfxDataStrc tData;

    if (!UI_GetUiWindowState(UI_MINIPANEL))
    {
        UI_SetUIToggle(UI_MINIPANEL, UI_TURNON, 0);
        gbHelpOpenedMiniPanel = 1;
    }

    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600)
    {
        memset(&tData, 0, sizeof(tData));
        tData.pCellFile = gpHelpBackgroundCel;
        tData.nDirection = 0;
        tData.nFrame = 0;
        D2GFX_TEXTURE_CelDraw(&tData, 0, 256, 0xFFFFFFFF, 5, NULL);
        tData.nFrame = 1;
        D2GFX_TEXTURE_CelDraw(&tData, 0, 512, 0xFFFFFFFF, 5, NULL);
        tData.nFrame = 2;
        D2GFX_TEXTURE_CelDraw(&tData, 66, 20, 0xFFFFFFFF, 5, NULL);
        tData.nFrame = 3;
        D2GFX_TEXTURE_CelDraw(&tData, 322, 20, 0xFFFFFFFF, 5, NULL);
        tData.nFrame = 4;
        D2GFX_TEXTURE_CelDraw(&tData, 535, 256, 0xFFFFFFFF, 5, NULL);
        tData.nFrame = 5;
        D2GFX_TEXTURE_CelDraw(&tData, 780, 256, 0xFFFFFFFF, 5, NULL);
        tData.nFrame = 6;
        D2GFX_TEXTURE_CelDraw(&tData, 780, 512, 0xFFFFFFFF, 5, NULL);

        HELP_DrawCloseButton();
        HELP_DrawKeyBulletCels();

        memset(&tData, 0, sizeof(tData));
        tData.pCellFile = gpHelpPointerCel;
        tData.nDirection = 0;
        tData.nFrame = 0;
        D2GFX_TEXTURE_CelDraw(&tData, 55, 535, 0xFFFFFFFF, 5, NULL);
        D2GFX_DrawLine(58, 486, 58, 532, 0xFF, 0xFF);
        D2GFX_DrawLine(59, 486, 59, 532, 0xFF, 0xFF);
        D2GFX_TEXTURE_CelDraw(&tData, 130, 564, 0xFFFFFFFF, 5, NULL);
        D2GFX_DrawLine(133, 445, 133, 561, 0xFF, 0xFF);
        D2GFX_DrawLine(134, 445, 134, 561, 0xFF, 0xFF);
        D2GFX_TEXTURE_CelDraw(&tData, 217, 575, 0xFFFFFFFF, 5, NULL);
        D2GFX_DrawLine(220, 375, 220, 572, 0xFF, 0xFF);
        D2GFX_DrawLine(221, 375, 221, 572, 0xFF, 0xFF);
        D2GFX_TEXTURE_CelDraw(&tData, 260, 585, 0xFFFFFFFF, 5, NULL);
        D2GFX_DrawLine(263, 526, 263, 582, 0xFF, 0xFF);
        D2GFX_DrawLine(264, 526, 264, 582, 0xFF, 0xFF);
        D2GFX_TEXTURE_CelDraw(&tData, 310, 585, 0xFFFFFFFF, 5, NULL);
        D2GFX_DrawLine(313, 486, 313, 582, 0xFF, 0xFF);
        D2GFX_DrawLine(314, 486, 314, 582, 0xFF, 0xFF);
        D2GFX_TEXTURE_CelDraw(&tData, 365, 565, 0xFFFFFFFF, 5, NULL);
        D2GFX_DrawLine(368, 525, 368, 562, 0xFF, 0xFF);
        D2GFX_DrawLine(369, 525, 369, 562, 0xFF, 0xFF);
        D2GFX_TEXTURE_CelDraw(&tData, 445, 540, 0xFFFFFFFF, 5, NULL);
        D2GFX_DrawLine(448, 451, 448, 537, 0xFF, 0xFF);
        D2GFX_DrawLine(449, 451, 449, 537, 0xFF, 0xFF);
        D2GFX_TEXTURE_CelDraw(&tData, 530, 569, 0xFFFFFFFF, 5, NULL);
        D2GFX_DrawLine(533, 525, 533, 566, 0xFF, 0xFF);
        D2GFX_DrawLine(534, 525, 534, 566, 0xFF, 0xFF);
        D2GFX_TEXTURE_CelDraw(&tData, 572, 575, 0xFFFFFFFF, 5, NULL);
        D2GFX_DrawLine(575, 375, 575, 572, 0xFF, 0xFF);
        D2GFX_DrawLine(576, 375, 576, 572, 0xFF, 0xFF);
        D2GFX_TEXTURE_CelDraw(&tData, 670, 564, 0xFFFFFFFF, 5, NULL);
        D2GFX_DrawLine(673, 445, 673, 561, 0xFF, 0xFF);
        D2GFX_DrawLine(674, 445, 674, 561, 0xFF, 0xFF);
        D2GFX_TEXTURE_CelDraw(&tData, 740, 535, 0xFFFFFFFF, 5, NULL);
        D2GFX_DrawLine(743, 486, 743, 532, 0xFF, 0xFF);
        D2GFX_DrawLine(744, 486, 744, 532, 0xFF, 0xFF);

        HELP_DrawLabelsWide();
    }
    else
    {
        HELP_DrawBackground();
        HELP_DrawCloseButton();
        HELP_DrawKeyBulletCels();
        HELP_DrawLeaderLines();
        HELP_DrawLabels();
    }
}


//1.10f: D2Client.0x6FAD8920
//  Draws the frame for every resolution but 800x600: eight frames of gpHelpBackgroundCel. The 800x600 arm of HELP_DrawScreen draws a frame of
//  its own.
void __cdecl HELP_DrawBackground()
{
    D2GfxDataStrc tData;

    memset(&tData, 0, sizeof(tData));
    tData.pCellFile = gpHelpBackgroundCel;
    tData.nDirection = 0;
    tData.nFrame = 0;
    D2GFX_TEXTURE_CelDraw(&tData, 0, 256, 0xFFFFFFFF, 5, NULL);
    tData.nFrame = 1;
    D2GFX_TEXTURE_CelDraw(&tData, 256, 256, 0xFFFFFFFF, 5, NULL);
    tData.nFrame = 2;
    D2GFX_TEXTURE_CelDraw(&tData, 0, gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
    tData.nFrame = 3;
    D2GFX_TEXTURE_CelDraw(&tData, 256, gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
    tData.nFrame = 4;
    D2GFX_TEXTURE_CelDraw(&tData, 320, 256, 0xFFFFFFFF, 5, NULL);
    tData.nFrame = 5;
    D2GFX_TEXTURE_CelDraw(&tData, 576, 256, 0xFFFFFFFF, 5, NULL);
    tData.nFrame = 6;
    D2GFX_TEXTURE_CelDraw(&tData, 320, gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
    tData.nFrame = 7;
    D2GFX_TEXTURE_CelDraw(&tData, 576, gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
}


//1.10f: D2Client.0x6FAD8A70
//  Draws the eight key bullets: frame 0 of gpHelpBulletCel, one every 20 pixels.
void __cdecl HELP_DrawKeyBulletCels()
{
    D2GfxDataStrc tData;
    int32_t nY;
    int32_t nBullets;

    nY = 73;
    memset(&tData, 0, sizeof(tData));
    tData.nDirection = 0;
    tData.pCellFile = gpHelpBulletCel;
    tData.nFrame = 0;
    for (nBullets = 8; nBullets; --nBullets)
    {
        D2GFX_TEXTURE_CelDraw(&tData, 104, nY, 0xFFFFFFFF, 5, NULL);
        nY += 20;
    }
}


//1.10f: D2Client.0x6FAD8AD0
//  Draws the nine pointers of the 640x480 layout: a frame-0 pointer cel each, with a vertical line two pixels wide drawn as two one-pixel
//  D2GFX_DrawLine calls side by side (colour and alpha 0xFF). The 800x600 arm of HELP_DrawScreen draws eleven of its own.
void __cdecl HELP_DrawLeaderLines()
{
    D2GfxDataStrc tData;

    memset(&tData, 0, sizeof(tData));
    tData.pCellFile = gpHelpPointerCel;
    tData.nDirection = 0;
    tData.nFrame = 0;
    D2GFX_TEXTURE_CelDraw(&tData, 51, 414, 0xFFFFFFFF, 5, NULL);
    D2GFX_DrawLine(54, 365, 54, 411, 0xFF, 0xFF);
    D2GFX_DrawLine(55, 365, 55, 411, 0xFF, 0xFF);
    D2GFX_TEXTURE_CelDraw(&tData, 127, 444, 0xFFFFFFFF, 5, NULL);
    D2GFX_DrawLine(130, 325, 130, 441, 0xFF, 0xFF);
    D2GFX_DrawLine(131, 325, 131, 441, 0xFF, 0xFF);
    D2GFX_TEXTURE_CelDraw(&tData, 178, 464, 0xFFFFFFFF, 5, NULL);
    D2GFX_DrawLine(181, 405, 181, 461, 0xFF, 0xFF);
    D2GFX_DrawLine(182, 405, 182, 461, 0xFF, 0xFF);
    D2GFX_TEXTURE_CelDraw(&tData, 227, 464, 0xFFFFFFFF, 5, NULL);
    D2GFX_DrawLine(230, 365, 230, 461, 0xFF, 0xFF);
    D2GFX_DrawLine(231, 365, 231, 461, 0xFF, 0xFF);
    D2GFX_TEXTURE_CelDraw(&tData, 284, 445, 0xFFFFFFFF, 5, NULL);
    D2GFX_DrawLine(287, 405, 287, 442, 0xFF, 0xFF);
    D2GFX_DrawLine(288, 405, 288, 442, 0xFF, 0xFF);
    D2GFX_TEXTURE_CelDraw(&tData, 350, 415, 0xFFFFFFFF, 5, NULL);
    D2GFX_DrawLine(353, 325, 353, 411, 0xFF, 0xFF);
    D2GFX_DrawLine(354, 325, 354, 411, 0xFF, 0xFF);
    D2GFX_TEXTURE_CelDraw(&tData, 445, 450, 0xFFFFFFFF, 5, NULL);
    D2GFX_DrawLine(448, 405, 448, 446, 0xFF, 0xFF);
    D2GFX_DrawLine(449, 405, 449, 446, 0xFF, 0xFF);
    D2GFX_TEXTURE_CelDraw(&tData, 503, 445, 0xFFFFFFFF, 5, NULL);
    D2GFX_DrawLine(506, 325, 506, 441, 0xFF, 0xFF);
    D2GFX_DrawLine(507, 325, 507, 441, 0xFF, 0xFF);
    D2GFX_TEXTURE_CelDraw(&tData, 582, 415, 0xFFFFFFFF, 5, NULL);
    D2GFX_DrawLine(585, 365, 585, 411, 0xFF, 0xFF);
    D2GFX_DrawLine(586, 365, 586, 411, 0xFF, 0xFF);
}


static_assert(STR_IDX_3986_strlvlup == 0xF92 && STR_IDX_3987_strnewskl == 0xF93
              && STR_IDX_4198_Strhelp1 == 0x1066 && STR_IDX_4199_StrHelp2 == 0x1067
              && STR_IDX_4200_StrHelp3 == 0x1068 && STR_IDX_4201_StrHelp4 == 0x1069
              && STR_IDX_4202_StrHelp5 == 0x106A && STR_IDX_4203_StrHelp6 == 0x106B
              && STR_IDX_4204_StrHelp7 == 0x106C && STR_IDX_4205_StrHelp8 == 0x106D
              && STR_IDX_4206_StrHelp8a == 0x106E && STR_IDX_4207_StrHelp9 == 0x106F
              && STR_IDX_4208_StrHelp10 == 0x1070 && STR_IDX_4209_StrHelp11 == 0x1071
              && STR_IDX_4210_StrHelp12 == 0x1072 && STR_IDX_4211_StrHelp13 == 0x1073
              && STR_IDX_4212_StrHelp14 == 0x1074 && STR_IDX_4213_StrHelp14a == 0x1075
              && STR_IDX_4214_StrHelp15 == 0x1076 && STR_IDX_4215_StrHelp16 == 0x1077
              && STR_IDX_4216_StrHelp16a == 0x1078 && STR_IDX_4217_StrHelp17 == 0x1079
              && STR_IDX_4218_StrHelp18 == 0x107A && STR_IDX_4219_StrHelp19 == 0x107B
              && STR_IDX_4220_StrHelp20 == 0x107C && STR_IDX_4221_StrHelp21 == 0x107D
              && STR_IDX_4222_StrHelp22 == 0x107E,
              "the two label passes load every one of these as a `mov ecx, <imm>` before "
              "D2LANG_GetStringFromTblIndex -- 1066h for the title, 1030h (pinned in "
              "char.cpp) for Close");
static_assert(HOTKEY_RUN == 0x22 && HOTKEY_STANDSTILL == 0x24 && HOTKEY_SHOWITEMS == 0x25,
              "`mov ecx, 22h`, `mov ecx, 24h` and `mov ecx, 25h` before "
              "CMDTBL_ControlsGetBoundKeyNameFallback name the run, stand-still and show-items "
              "keys; HOTKEY_AUTOMAP and HOTKEY_HELP are pinned in text.cpp and above");


//1.10f: D2Client.0x6FAD8E50
//  Draws the text of the 800x600 overlay (HELP_DrawScreen ends in a tail jump here): the title (tbl 4198) and Close (tbl 4144) in the font the
//  caller left; then, in font D2FONT_FONTFORMAL12, five lines that name a key (run, show items, stand still, automap and this screen), three
//  plain lines and twenty labels centred on fixed x; then the caller's font is put back. The original inlines HELP_CenterTextX for all but the
//  last three centred strings.
//  Each key line asks for the key name before its format, as the original calls them, so the C holds both in locals; each line's buffer is its
//  own array in the original, so each is declared in a block of its own. void: reached by a tail jump from the void HELP_DrawScreen, so the
//  final SetFont answer goes nowhere.
void __cdecl HELP_DrawLabelsWide()
{
    int32_t nX;
    const Unicode *pwszKeyName;
    const Unicode *pwszFormat;
    enum Font ePrevFont;

    nX = HELP_CenterTextX(STR_IDX_4198_Strhelp1, 380);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4198_Strhelp1), nX, 17, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4144_strClose, 701);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4144_strClose), nX, 73, 0, 0);
    ePrevFont = D2WIN_10127_SetFont(D2FONT_FONTFORMAL12);
    {
        Unicode wszRunLine[100];

        pwszKeyName = CMDTBL_ControlsGetBoundKeyNameFallback(HOTKEY_RUN, TRUE);
        pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_4199_StrHelp2);
        Unicode::sprintf(100, wszRunLine, pwszFormat, pwszKeyName);
        D2WIN_10117_DrawText(wszRunLine, 117, 78, 0, 0);
    }
    {
        Unicode wszShowItemsLine[100];

        pwszKeyName = CMDTBL_ControlsGetBoundKeyNameFallback(HOTKEY_SHOWITEMS, TRUE);
        pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_4200_StrHelp3);
        Unicode::sprintf(100, wszShowItemsLine, pwszFormat, pwszKeyName);
        D2WIN_10117_DrawText(wszShowItemsLine, 117, 98, 0, 0);
    }
    {
        Unicode wszStandStillLine[100];

        pwszKeyName = CMDTBL_ControlsGetBoundKeyNameFallback(HOTKEY_STANDSTILL, TRUE);
        pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_4201_StrHelp4);
        Unicode::sprintf(100, wszStandStillLine, pwszFormat, pwszKeyName);
        D2WIN_10117_DrawText(wszStandStillLine, 117, 118, 0, 0);
    }
    {
        Unicode wszAutomapLine[100];

        pwszKeyName = CMDTBL_ControlsGetBoundKeyNameFallback(HOTKEY_AUTOMAP, TRUE);
        pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_4202_StrHelp5);
        Unicode::sprintf(100, wszAutomapLine, pwszFormat, pwszKeyName);
        D2WIN_10117_DrawText(wszAutomapLine, 117, 138, 0, 0);
    }
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4203_StrHelp6), 117, 158, 0, 0);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4204_StrHelp7), 117, 178, 0, 0);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4205_StrHelp8), 117, 198, 0, 0);
    {
        Unicode wszHelpKeyLine[100];

        pwszKeyName = CMDTBL_ControlsGetBoundKeyNameFallback(HOTKEY_HELP, TRUE);
        pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_4206_StrHelp8a);
        Unicode::sprintf(100, wszHelpKeyLine, pwszFormat, pwszKeyName);
        D2WIN_10117_DrawText(wszHelpKeyLine, 117, 218, 0, 0);
    }
    nX = HELP_CenterTextX(STR_IDX_4207_StrHelp9, 59);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4207_StrHelp9), nX, 481, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4208_StrHelp10, 134);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4208_StrHelp10), nX, 410, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4209_StrHelp11, 134);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4209_StrHelp11), nX, 425, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4210_StrHelp12, 134);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4210_StrHelp12), nX, 440, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_3986_strlvlup, 222);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_3986_strlvlup), nX, 375, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4212_StrHelp14, 264);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4212_StrHelp14), nX, 506, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4213_StrHelp14a, 264);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4213_StrHelp14a), nX, 521, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4214_StrHelp15, 314);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4214_StrHelp15), nX, 481, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4215_StrHelp16, 369);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4215_StrHelp16), nX, 505, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4216_StrHelp16a, 369);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4216_StrHelp16a), nX, 520, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4217_StrHelp17, 449);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4217_StrHelp17), nX, 401, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4218_StrHelp18, 449);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4218_StrHelp18), nX, 416, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4219_StrHelp19, 449);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4219_StrHelp19), nX, 431, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4220_StrHelp20, 449);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4220_StrHelp20), nX, 446, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4221_StrHelp21, 534);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4221_StrHelp21), nX, 520, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_3987_strnewskl, 572);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_3987_strnewskl), nX, 375, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4211_StrHelp13, 674);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4211_StrHelp13), nX, 410, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4209_StrHelp11, 674);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4209_StrHelp11), nX, 425, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4210_StrHelp12, 674);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4210_StrHelp12), nX, 440, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4222_StrHelp22, 744);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4222_StrHelp22), nX, 481, 0, 0);
    D2WIN_10127_SetFont(ePrevFont);
}


//1.10f: D2Client.0x6FAD97D0
//  Draws the text of the other layout (HELP_DrawScreen ends in a tail jump here): the same program as HELP_DrawLabelsWide at other positions,
//  less the two red-button labels (tbl 3986 strlvlup, tbl 3987 strnewskl) that only the wide layout draws: the title and Close, the five key
//  lines, the three plain ones and eighteen centred labels. The original inlines HELP_CenterTextX for all but the last two centred strings.
//  void, for the same reason as HELP_DrawLabelsWide.
void __cdecl HELP_DrawLabels()
{
    int32_t nX;
    const Unicode *pwszKeyName;
    const Unicode *pwszFormat;
    enum Font ePrevFont;

    nX = HELP_CenterTextX(STR_IDX_4198_Strhelp1, 305);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4198_Strhelp1), nX, 17, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4144_strClose, 541);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4144_strClose), nX, 73, 0, 0);
    ePrevFont = D2WIN_10127_SetFont(D2FONT_FONTFORMAL12);
    {
        Unicode wszRunLine[100];

        pwszKeyName = CMDTBL_ControlsGetBoundKeyNameFallback(HOTKEY_RUN, TRUE);
        pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_4199_StrHelp2);
        Unicode::sprintf(100, wszRunLine, pwszFormat, pwszKeyName);
        D2WIN_10117_DrawText(wszRunLine, 117, 78, 0, 0);
    }
    {
        Unicode wszShowItemsLine[100];

        pwszKeyName = CMDTBL_ControlsGetBoundKeyNameFallback(HOTKEY_SHOWITEMS, TRUE);
        pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_4200_StrHelp3);
        Unicode::sprintf(100, wszShowItemsLine, pwszFormat, pwszKeyName);
        D2WIN_10117_DrawText(wszShowItemsLine, 117, 98, 0, 0);
    }
    {
        Unicode wszStandStillLine[100];

        pwszKeyName = CMDTBL_ControlsGetBoundKeyNameFallback(HOTKEY_STANDSTILL, TRUE);
        pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_4201_StrHelp4);
        Unicode::sprintf(100, wszStandStillLine, pwszFormat, pwszKeyName);
        D2WIN_10117_DrawText(wszStandStillLine, 117, 118, 0, 0);
    }
    {
        Unicode wszAutomapLine[100];

        pwszKeyName = CMDTBL_ControlsGetBoundKeyNameFallback(HOTKEY_AUTOMAP, TRUE);
        pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_4202_StrHelp5);
        Unicode::sprintf(100, wszAutomapLine, pwszFormat, pwszKeyName);
        D2WIN_10117_DrawText(wszAutomapLine, 117, 138, 0, 0);
    }
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4203_StrHelp6), 117, 158, 0, 0);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4204_StrHelp7), 117, 178, 0, 0);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4205_StrHelp8), 117, 198, 0, 0);
    {
        Unicode wszHelpKeyLine[100];

        pwszKeyName = CMDTBL_ControlsGetBoundKeyNameFallback(HOTKEY_HELP, TRUE);
        pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_4206_StrHelp8a);
        Unicode::sprintf(100, wszHelpKeyLine, pwszFormat, pwszKeyName);
        D2WIN_10117_DrawText(wszHelpKeyLine, 117, 218, 0, 0);
    }
    nX = HELP_CenterTextX(STR_IDX_4207_StrHelp9, 55);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4207_StrHelp9), nX, 360, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4208_StrHelp10, 131);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4208_StrHelp10), nX, 290, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4209_StrHelp11, 131);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4209_StrHelp11), nX, 305, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4210_StrHelp12, 131);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4210_StrHelp12), nX, 320, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4212_StrHelp14, 182);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4212_StrHelp14), nX, 385, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4213_StrHelp14a, 182);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4213_StrHelp14a), nX, 400, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4214_StrHelp15, 231);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4214_StrHelp15), nX, 360, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4215_StrHelp16, 288);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4215_StrHelp16), nX, 385, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4216_StrHelp16a, 288);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4216_StrHelp16a), nX, 400, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4217_StrHelp17, 354);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4217_StrHelp17), nX, 275, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4218_StrHelp18, 354);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4218_StrHelp18), nX, 290, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4219_StrHelp19, 354);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4219_StrHelp19), nX, 305, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4220_StrHelp20, 354);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4220_StrHelp20), nX, 320, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4221_StrHelp21, 449);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4221_StrHelp21), nX, 400, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4211_StrHelp13, 507);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4211_StrHelp13), nX, 290, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4209_StrHelp11, 507);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4209_StrHelp11), nX, 305, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4210_StrHelp12, 507);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4210_StrHelp12), nX, 320, 0, 0);
    nX = HELP_CenterTextX(STR_IDX_4222_StrHelp22, 586);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4222_StrHelp22), nX, 360, 0, 0);
    D2WIN_10127_SetFont(ePrevFont);
}


//1.10f: D2Client.0x6FADA0D0
//  Draws the help screen's close button: UI_GetBuySellButtonImage's cel, frame 10 (11 while gbHelpCloseButtonDown is set). Its position is the
//  left edge and the bottom of the box the help screen's mouse handlers test. It asks for the resolution again although its caller has just
//  branched on it.
void __cdecl HELP_DrawCloseButton()
{
    D2GfxDataStrc tData;

    memset(&tData, 0, sizeof(tData));
    tData.pCellFile = UI_GetBuySellButtonImage();
    tData.nDirection = 0;
    tData.nFrame = (gbHelpCloseButtonDown != 0) + 10;
    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600)
        D2GFX_TEXTURE_CelDraw(&tData, 685, 56, 0xFFFFFFFF, 5, NULL);
    else
        D2GFX_TEXTURE_CelDraw(&tData, 525, 56, 0xFFFFFFFF, 5, NULL);
}


//1.10f: D2Client.0x6FADA140
//  Frees each of the three help cels that is loaded and zeroes its pointer.
void __cdecl HELP_UnloadImages()
{
    if (gpHelpBackgroundCel)
    {
        ARCHIVE_CellFileFree(gpHelpBackgroundCel);
        gpHelpBackgroundCel = 0;
    }
    if (gpHelpPointerCel)
    {
        ARCHIVE_CellFileFree(gpHelpPointerCel);
        gpHelpPointerCel = 0;
    }
    if (gpHelpBulletCel)
    {
        ARCHIVE_CellFileFree(gpHelpBulletCel);
        gpHelpBulletCel = 0;
    }
}


//  Handlers of the help screen (gHelpscreenMessageHandlers, five records, registered under UI_HELPSCREEN) and of the help button
//  (gHelpbuttonMessageHandlers, two records, registered under UI_HELPBUTTON):
//
//      WM_LBUTTONDOWN  HELP_HelpScreenOnLButtonDown        WM_LBUTTONDOWN  HELP_HelpButtonOnLButtonDown
//      WM_LBUTTONUP    HELP_HelpScreenOnLButtonUp          WM_LBUTTONUP    HELP_HelpButtonOnLButtonUp
//      VK_SPACE        HELP_HelpScreenOnDismissKey
//      VK_ESCAPE       HELP_HelpScreenOnDismissKey
//      WM_CHAR         HELP_Nullsub5
//
//  Storm presets bReturn to 0 and runs every registered handler in turn; a handler claims the message by setting bReturn and lResult and
//  calling SMsgBreakHandlerChain, which stops the walk. Storm discards what a handler returns, so every handler here is void.

static_assert(offsetof(SMSGHANDLER_PARAMS, nXpos) == 0x0C
              && offsetof(SMSGHANDLER_PARAMS, nYpos) == 0x0E,
              "the two helpscreen button handlers read `mov di, [esi+0Ch]` and "
              "`mov bx, [esi+0Eh]` after zeroing the register, so the position "
              "is a pair of zero-extended 16-bit fields at +0x0C -- Ghidra "
              "prints the same two offsets");
static_assert(offsetof(SMSGHANDLER_PARAMS, bReturn) == 0x18
              && offsetof(SMSGHANDLER_PARAMS, lResult) == 0x1C,
              "every claiming handler here writes [esi+18h] and [esi+1Ch], and "
              "Storm's SMsgDispatchMessage builds the same 32-byte block with "
              "those two at ebp-8 and ebp-4");
static_assert(UI_MINIPANEL == 0x15 && UI_HELPSCREEN == 0x21 && UI_HELPBUTTON == 0x22,
              "`mov ecx, 15h`, `mov ecx, 21h` and `mov ecx, 22h` are the three "
              "panel ids these handlers pass to UI_SetUIToggle");
static_assert(UI_TURNOFF == 1 && UI_TOGGLE == 2,
              "the dismiss key passes `mov edx, 1` and the close button "
              "`mov edx, 2` -- the close button TOGGLES where the key turns off");


//1.10f: D2Client.0x6FADA190
//  A click on the help screen. It claims the message whatever happens. The close button is a fixed rectangle chosen by resolution and tested
//  against the position in the message (bounds inclusive); a hit only arms gbHelpCloseButtonDown, a miss calls GAME_ResetMouseButtonState, so
//  clicking anywhere else on the help screen drops whatever the mouse was in the middle of.
void __stdcall HELP_HelpScreenOnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    BOOL bWithinX;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;

    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600)
        bWithinX = nXpos >= 685 && nXpos <= 717;
    else
        bWithinX = nXpos >= 525 && nXpos <= 557;

    if (bWithinX && nYpos >= 24 && nYpos <= 56)
        gbHelpCloseButtonDown = 1;
    else
        GAME_ResetMouseButtonState();

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FADA220
//  The release. The close happens only if the press landed on the button too: gbHelpCloseButtonDown is tested before anything else, and any
//  failure clears it. The gotos are the original's control flow: four tests jump to one miss label and the hit path falls into a shared claim.
void __stdcall HELP_HelpScreenOnLButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    BOOL bWithinX;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;

    if (!gbHelpCloseButtonDown)
        goto miss;

    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600)
    {
        if (nXpos < 685)
            goto miss;
        bWithinX = nXpos <= 717;
    }
    else
    {
        if (nXpos < 525)
            goto miss;
        bWithinX = nXpos <= 557;
    }

    if (!bWithinX || nYpos < 24 || nYpos > 56)
        goto miss;

    UI_SetUIToggle(UI_HELPSCREEN, UI_TOGGLE, 0);
    //  Redundant, as in HELP_HelpScreenOnDismissKey.
    HELP_CloseMiniPanelIfOpened();
    goto claim;

miss:
    gbHelpCloseButtonDown = 0;
    GAME_ResetMouseButtonState();

claim:
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FADA2D0
//  ESC or SPACE (both bound to this function) turn the help screen off. void: the original leaves HELP_CloseMiniPanelIfOpened's result in eax,
//  and Storm discards what a handler returns.
//  BUG: unlike the dismiss-key handlers of the five other panels that bind one, this one does not claim its key (no bReturn, no
//  SMsgBreakHandlerChain), so ESC or SPACE closes the help screen and then reaches whatever else listens on that key. LATENT: with the stock
//  bindings CMDTBL_OnKeyDown, on the message list that Storm walks before the key list, runs first, and its command calls UI_CloseAllPanels,
//  which closes the help screen and unregisters this handler; it matters only for a rebound key, or when the command refuses to close the
//  screen (an NPC dialog, a popup), and then only for an older handler of the same key.
void __stdcall HELP_HelpScreenOnDismissKey(SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    UI_SetUIToggle(UI_HELPSCREEN, UI_TURNOFF, 0);

    //  Redundant, as in the original: UI_SetUIToggle has already closed the mini-panel (UI_UnregisterPanelHandlers calls
    //  HELP_CloseMiniPanelIfOpened) and cleared gbHelpOpenedMiniPanel, so this finds the flag clear and does nothing.
    HELP_CloseMiniPanelIfOpened();
}


//1.10f: D2Client.0x6FADA310
//  A do-nothing handler with the SMSGHANDLER signature; the stack pop of its `retn 4` is what makes it usable as one. It is registered in three
//  tables, to sit on a message the panel does not want:
//
//      gHelpscreenMessageHandlers     WM_CHAR
//      gAutomapMessageHandlers        VK_END
//      gRecipescrollMessageHandlers   WM_MOUSEMOVE
//
//  It writes no bReturn and breaks no chain, so it swallows nothing: the message reaches every other handler and then DefWindowProcA as if the
//  record were absent. The generic name is kept; the original's is not known.
void __stdcall HELP_Nullsub5(SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);
}


//1.10f: D2Client.0x6FADA320
//  Puts the mini-panel back if the help screen is what opened it (gbHelpOpenedMiniPanel); a mini-panel the player opened is left alone. Returns
//  the flag when it was clear, otherwise UI_SetUIToggle's result; no caller reads it.
int32_t __cdecl HELP_CloseMiniPanelIfOpened()
{
    int32_t result;

    result = gbHelpOpenedMiniPanel;
    if (gbHelpOpenedMiniPanel)
    {
        result = UI_SetUIToggle(UI_MINIPANEL, UI_TURNOFF, 0);
        gbHelpOpenedMiniPanel = 0;
    }

    return result;
}


static_assert(STR_IDX_4177_minipanelHelp == 0x1051 && STR_IDX_4178_minipanelspecial == 0x1052
              && HOTKEY_HELP == 6,
              "HELP_DrawHelpHint loads `mov ecx, 1051h` and `mov ecx, 1052h` for its two tbl "
              "strings and `mov ecx, 6` for the command whose keys it names");


//1.10f: D2Client.0x6FADA350
//  Draws the help button's hint. It is hidden while the skill tree, the skill selector, the inventory, the shop, the trade screen or the stash
//  is open (gbHelpHintSuppressed records that for the button's handlers). gnHelpMenuSeen starts at -1; on the first pass it is zeroed and
//  loaded from the registry ("Help Menu" under "Diablo II", the value HELP_HelpButtonOnLButtonUp writes), and once it is set the button panel
//  is turned off and nothing is drawn.
//  Otherwise the hint is tbl 4177 (minipanelHelp) followed by " (%s)" (tbl 4178, minipanelspecial) for each key bound to HOTKEY_HELP, the
//  primary first, centred on gnScreenWidth - 58 by a signed halving; then the level-up button's socket and button art (frame 1 while
//  gbHelpButtonDown is set) are drawn. Each key name is asked for before its format, as the original calls them.
void __cdecl HELP_DrawHelpHint()
{
    const Unicode *pwszKeyName;
    const Unicode *pwszFormat;
    int32_t nTextWidth;

    if (UI_GetUiWindowState(UI_SKILLTREE) || UI_GetUiWindowState(UI_MINISKILL) || UI_GetUiWindowState(UI_INVENTORY) || UI_GetUiWindowState(UI_NPCSHOP) || UI_GetUiWindowState(UI_MPTRADE) || UI_GetUiWindowState(UI_STASH))
    {
        gbHelpHintSuppressed = 1;
        return;
    }

    gbHelpHintSuppressed = 0;
    if (gnHelpMenuSeen == -1)
    {
        gnHelpMenuSeen = 0;
        STORM_SRegLoadValue(szDiabloIIKey, szHelpMenu, 1, (uint32_t *)&gnHelpMenuSeen);
    }
    if (gnHelpMenuSeen)
    {
        UI_SetUIToggle(UI_HELPBUTTON, UI_TURNOFF, 0);
        return;
    }

    D2GfxDataStrc tData;
    memset(&tData, 0, sizeof(tData));
    Unicode wszText[200];
    Unicode wszKeySuffix[30];

    Unicode::strcpy(wszText, D2LANG_GetStringFromTblIndex(STR_IDX_4177_minipanelHelp));
    if ((uint16_t)CMDTBL_ControlsFindBoundKey(HOTKEY_HELP, TRUE) != 0xFFFF)
    {
        memset(wszKeySuffix, 0, sizeof(wszKeySuffix));
        pwszKeyName = CMDTBL_ControlsGetBoundKeyName(HOTKEY_HELP, TRUE);
        pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_4178_minipanelspecial);
        Unicode::sprintf(30, wszKeySuffix, pwszFormat, pwszKeyName);
        Unicode::strcat(wszText, wszKeySuffix);
    }
    if ((uint16_t)CMDTBL_ControlsFindBoundKey(HOTKEY_HELP, FALSE) != 0xFFFF)
    {
        memset(wszKeySuffix, 0, sizeof(wszKeySuffix));
        pwszKeyName = CMDTBL_ControlsGetBoundKeyName(HOTKEY_HELP, FALSE);
        pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_4178_minipanelspecial);
        Unicode::sprintf(30, wszKeySuffix, pwszFormat, pwszKeyName);
        Unicode::strcat(wszText, wszKeySuffix);
    }

    nTextWidth = D2WIN_10121_GetTextWidth(wszText);
    D2WIN_10117_DrawText(wszText, gnScreenWidth - nTextWidth / 2 - 58, gnScreenHeight - 197, 0, 0);

    tData.pCellFile = gpImgLevelUpSocket;
    tData.nDirection = 0;
    tData.nFrame = 0;
    D2GFX_TEXTURE_CelDraw(&tData, gnScreenWidth - 75, gnScreenHeight - 160, 0xFFFFFFFF, 5, NULL);
    tData.pCellFile = gpImgLevelUpButton;
    tData.nFrame = gbHelpButtonDown != 0;
    D2GFX_TEXTURE_CelDraw(&tData, gnScreenWidth - 72, gnScreenHeight - 164, 0xFFFFFFFF, 5, NULL);
}


//1.10f: D2Client.0x6FADA5F0
//  A press on the little help button. It consults gbHelpHintSuppressed before doing anything, and it asks the cursor where it is
//  (CURSOR_GetMouseX/Y) rather than reading the position out of the message. The button's box (bounds inclusive) is anchored to the
//  bottom-right corner. A hit sets gbHelpButtonDown and claims the message.
//  nMouseY first holds the suppress flag, as the original's register does, and is then overwritten with the cursor's y. void: the original
//  leaves one of three values in eax (the flag, GetMouseY's answer, or on the hit path SMsgBreakHandlerChain's), and Storm discards all of
//  them.
void __stdcall HELP_HelpButtonOnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nMouseY;
    int32_t nBoxBottom;
    int32_t nBoxLeft;
    int32_t nMouseX;

    nMouseY = gbHelpHintSuppressed;
    if (!nMouseY)
    {
        nBoxBottom = gnScreenHeight - 160;
        nBoxLeft = gnScreenWidth - 75;
        nMouseX = CURSOR_GetMouseX();
        nMouseY = CURSOR_GetMouseY();

        if (nMouseX >= nBoxLeft && nMouseX <= nBoxLeft + 35 && nMouseY >= nBoxBottom - 36 && nMouseY <= nBoxBottom)
        {
            gbHelpButtonDown = 1;
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
            STORM_SMsgBreakHandlerChain(pMsg);
            return;
        }
    }

}


//1.10f: D2Client.0x6FADA660
//  The release over the help button: the same box test as the press, recomputed rather than shared. On a hit the button panel goes off, the
//  help screen toggles on, gnHelpMenuSeen is set and "Help Menu" = 1 is written under "Diablo II" in the registry, so the button is a one-time
//  prompt: once clicked, the value persists across runs and HELP_DrawHelpHint, which reads it, stops drawing the hint. The message is claimed.
//  gbHelpButtonDown is set by the press, cleared here and read by HELP_DrawHelpHint for the pressed frame.
//  BUG: unlike the help screen's close button, which requires the press to have landed on it, this does not test gbHelpButtonDown, so a release
//  over the button opens the help screen even when the press began elsewhere. REACHABLE: this record is registered after
//  PANEL_GameOnLButtonUp's and GAME_WindowOnLButtonUp's (UI_HELPBUTTON is turned on after UI_GAME) and Storm runs the newest first; a hit
//  claims the release, so neither sees it (GAME_WindowOnLButtonUp's clear of gbLeftButtonDown does not happen).
void __stdcall HELP_HelpButtonOnLButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nMouseY;
    int32_t nBoxBottom;
    int32_t nBoxLeft;
    int32_t nMouseX;

    nMouseY = gbHelpHintSuppressed;
    if (!nMouseY)
    {
        nBoxBottom = gnScreenHeight - 160;
        nBoxLeft = gnScreenWidth - 75;
        nMouseX = CURSOR_GetMouseX();
        nMouseY = CURSOR_GetMouseY();

        if (nMouseX >= nBoxLeft && nMouseX <= nBoxLeft + 35 && nMouseY >= nBoxBottom - 36 && nMouseY <= nBoxBottom)
        {
            UI_SetUIToggle(UI_HELPBUTTON, UI_TURNOFF, 0);
            UI_SetUIToggle(UI_HELPSCREEN, UI_TOGGLE, 0);
            gnHelpMenuSeen = 1;
            STORM_SRegSaveValue(szDiabloIIKey, szHelpMenu, 1, 1);
            gbHelpButtonDown = 0;
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
            STORM_SMsgBreakHandlerChain(pMsg);
            return;
        }
    }

}

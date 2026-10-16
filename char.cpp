#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "CoreArchive.h"
#include "ui.h"
#include "inv.h"
#include "panel.h"
#include "char.h"

//  char.cpp -- the character screen of D2Client.dll 1.10: CHAR_CharPanelDraw and its mouse handlers CHAR_StatsOnLButtonDown and
//  CHAR_StatsOnLButtonUp (the records of gStatsMessageHandlers, registered under UI_STATSCREEN), the chance-to-hit and weapon-block figures it
//  shows, the UI button class the gold dialog uses, and a few small helpers.
//  The original source file is UI\char.cpp (the asserts in this unit name it). Address range 0x6FACF8D0..0x6FAD170E.

//  gCharPanelStatButtons holds four 14-byte records, one per stat button (Strength, Dexterity, Vitality, Energy), walked by the draw and by
//  both handlers:
//      +0   dword  X, relative to the panel's left edge
//      +4   dword  Y, relative to the 640x480 design height (the bottom of the button band)
//      +8   dword  the pressed flag
//      +12  word   what MSG_SendCmdWord sends for that stat
//  A click is accepted for nButtonBottom - 502 < y < nButtonBottom - 480, with nButtonBottom = gnScreenHeight + gnBorderYDifference + record.Y
//  (480 is the design height; the band is 22 pixels tall), and for x within 40 pixels right of gnBorderXDifference + record.X.
static_assert(sizeof(int32_t) * 3 + sizeof(int16_t) == 14,
              "a stat-button record is three dwords and a word, and "
              "gCharPanelStatButtons is unsigned __int8[56] -- four of them");

//  The original's assert strings: its source path and the two assert expressions.
static const char szCharCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UI\\char.cpp";
static const char szDifficultyLevelAssert[] = "nDifficultyLevel < NUM_DIFFICULTY_LEVELS";
static const char szPtStats[] = "ptStats";

//  Layout checks for CHAR_CharPanelGetWeaponBlockPercent.
static_assert(offsetof(D2SLayerStatIdStrc, nLayer) == 0
                  && offsetof(D2SLayerStatIdStrc, nStat) == 2,
              "mov si, word ptr [esp+ebp*8+layer]: a stat is {word nLayer, "
              "word nStat} and then the int value at +4");
static_assert(BODYLOC_RARM == 4 && BODYLOC_LARM == 5,
              "push 4 at 0x6FAD103C and push 5 at 0x6FAD1044");
static_assert(STAT_PASSIVE_WEAPONBLOCK == 0x15C, "push 15Ch at 0x6FAD105B");

//  Layout checks for CHAR_CharPanelChanceToHitPercent.
static_assert(offsetof(D2MonStatsTxt, nAlign) == 0x4C
                  && offsetof(D2MonStatsTxt, nLevel) == 0xAA,
              "mov al, [ebx+4Ch] and movsx ..., word ptr [ebx+eax*2+0AAh]");
static_assert(offsetof(D2DataTablesStrc, pCharStatsTxt) == 0xBC4
                  && offsetof(D2DataTablesStrc, nCharStatsTxtRecordCount) == 0xBC8
                  && sizeof(D2CharStatsTxt) == 49 * 4
                  && offsetof(D2CharStatsTxt, dwToHitFactor) == 0x3C,
              "cmp ecx, [edx+0BC8h]; mov ecx, [edx+0BC4h]; lea ecx, [ecx+edi*4] "
              "with edi = 49 * class; mov eax, [ecx+3Ch]");
static_assert(sizeof(D2MonStatsInitStrc) == 14 * 4
                  && offsetof(D2MonStatsInitStrc, nAC) == 8,
              "mov ecx, 0Eh / rep stosd clears it, and nAC is [esp+54h+var_38+8]");
static_assert(STAT_LEVEL == 12, "push 0Ch at 0x6FAD124D");

//  Layout check for CHAR_CharPanelMeasureLabelWidths.
static_assert(sizeof(D2CharPanelLabelStrc) * 15 == 0x10E
                  && offsetof(D2CharPanelLabelStrc, wStringId) == 0x0C
                  && offsetof(D2CharPanelLabelStrc, nTextWidth) == 0x0E,
              "mov cx, [esi+6FB7801Ch] / mov [esi+6FB7801Eh], eax over a base of "
              "6FB78010h, fifteen records of 12h bytes");


//  The UI button class (a D2ClientButtonStrc) and two text helpers. In the button, nY is the bottom edge, bPressed is set by a press on it,
//  bFireOnEnter makes Enter click it (OnChar) and pfnOnClick is `int __cdecl()`. gUiButtonImages[nImageIndex] is {wFrame, wCaptionStrId}: 0 is
//  {16, tbl 3401 'ok'} and 1 is {10, tbl 4142 'cancel'}.
//  Vtable slots of gVtblUiButtonImpl: 0 VecDelDtor, 1 Draw, 3 OnMouseDown, 4 OnMouseUp, 6 OnChar; slot 0 of gVtblUiButtonProxy is
//  ProxyVecDelDtor.
//  The members are free functions: those that take only `this` are __fastcall (the same ABI for one register argument), the others carry an
//  explicit nEdxUnused so that ecx, edx and the stack line up exactly as __thiscall's do.
//  DIFFERENCE: Construct, ProxyDestruct and ProxyConstruct open with an exception-handling frame in the original (an fs:0 frame and an EH
//  state); ours has none. Nothing here throws, so no value the program computes changes.

//  Allocation tags passed to STORM_SMemAlloc and STORM_SMemFree.
static const char szNew[] = "new";
static const char szDelete[] = "delete";

static_assert(sizeof(D2ClientButtonStrc) == 0x36,
              "push 36h at 0x6FACFC78 is the button the proxy allocates");
static_assert(offsetof(D2ClientButtonStrc, nX) == 0x08
                  && offsetof(D2ClientButtonStrc, nY) == 0x0C
                  && offsetof(D2ClientButtonStrc, nHeight) == 0x18
                  && offsetof(D2ClientButtonStrc, nWidth) == 0x1C
                  && offsetof(D2ClientButtonStrc, bPressed) == 0x20
                  && offsetof(D2ClientButtonStrc, bFireOnEnter) == 0x24
                  && offsetof(D2ClientButtonStrc, pfnOnClick) == 0x28
                  && offsetof(D2ClientButtonStrc, field_2C) == 0x2C
                  && offsetof(D2ClientButtonStrc, wFrame) == 0x30
                  && offsetof(D2ClientButtonStrc, nImageIndex) == 0x32,
              "[esi+8], [esi+0Ch], [esi+18h], [esi+1Ch], [ecx+20h], [ecx+24h], "
              "call [ecx+28h], [esi+2Ch], mov [esi+30h], cx and [esi+32h], eax");
static_assert(offsetof(D2UiButtonImageStrc, wCaptionStrId) == 2,
              "mov cx, word_6FB78008[eax*4] against word ptr 6FB7800Ah[eax*4]");
static_assert(offsetof(SMSGHANDLER_PARAMS, wParam) == 0x08
                  && offsetof(SMSGHANDLER_PARAMS, lParam) == 0x0C,
              "cmp dword ptr [eax+8], 0Dh and test dword ptr [eax+0Ch], 40000000h");


//1.10f: D2Client.0x6FACF8D0
//  The button's constructor: installs the vtable, takes its frame from gUiButtonImages, clears the three state fields and measures its own cel
//  once, so that the hit tests have a width and a height. The stores follow the original's order: x and y land after the cel handle is taken.
int32_t *__fastcall CHAR_UiButtonConstruct(D2ClientButtonStrc *pButton, int32_t nEdxUnused, int32_t nX, int32_t nY, int32_t nImageIndex, void *pfnOnClick)
{
    D2GfxDataStrc tCelContext;
    D2GfxCellStrc *pCel;

    pButton->pVtbl = &gVtblUiButtonImpl;
    pButton->nImageIndex = nImageIndex;
    pButton->wFrame = gUiButtonImages[nImageIndex].wFrame;
    memset(&tCelContext, 0, sizeof(tCelContext));
    pButton->pfnOnClick = (int32_t (__cdecl *)())pfnOnClick;
    pButton->bPressed = 0;
    pButton->field_2C = 0;
    tCelContext.pCellFile = UI_GetBuySellButtonImage();
    tCelContext.nDirection = 0;
    tCelContext.nFrame = pButton->wFrame;
    pCel = D2CMP_CelGetHandle(&tCelContext);
    pButton->nX = nX;
    pButton->nY = nY;
    pButton->bFireOnEnter = 0;
    pButton->nHeight = D2CMP_CelGetHeight(pCel);
    pButton->nWidth = D2CMP_CelGetWidth(pCel);
    return (int32_t *)pButton;
}


//1.10f: D2Client.0x6FACF990
//  Slot 0 of gVtblUiButtonImpl, the scalar-deleting destructor: destructs, and frees the object when bit 0 of bFreeObject is set.
D2ClientButtonStrc *__fastcall CHAR_UiButtonVecDelDtor(D2ClientButtonStrc *pButton, int32_t nEdxUnused, uint8_t bFreeObject)
{
    CHAR_UiButtonDestruct(pButton);
    if ((bFreeObject & 1) != 0 && pButton)
    {
        STORM_SMemFree(pButton, szDelete, -1, 0);
    }
    return pButton;
}


//1.10f: D2Client.0x6FACF9C0
//  The destructor: reinstalls the class's own vtable, then tail-jumps into the shared base destructor.
void __fastcall CHAR_UiButtonDestruct(D2ClientButtonStrc *pButton)
{
    pButton->pVtbl = &gVtblUiButtonImpl;
    LISTBOX_UiCtrlBaseDestruct((int32_t *)pButton);
}


//1.10f: D2Client.0x6FACF9D0
//  Slot 1, draw: the cel at (nX, nY) with frame wFrame + bPressed, then a caption above the button while the cursor is over it (Font16, centred
//  on the button's width, five pixels above its top). The cursor is taken as two zero-extended words and compared signed against the int
//  fields; the caption's x is (nWidth - w) / 2, truncating, and is not tested for fitting. void: the dispatcher calls slot 1 as void, and the
//  eax the original leaves is the mouse x, the bottom y or SetFont's answer depending on the path.
void __fastcall CHAR_UiButtonDraw(D2ClientButtonStrc *pButton)
{
    D2GfxDataStrc tCelContext;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nPrevFont;
    const Unicode *pwszCaption;
    int32_t nTextWidth;

    memset(&tCelContext, 0, sizeof(tCelContext));
    tCelContext.pCellFile = UI_GetBuySellButtonImage();
    tCelContext.nFrame = pButton->wFrame + pButton->bPressed;
    tCelContext.nDirection = 0;
    D2GFX_TEXTURE_CelDraw(&tCelContext, pButton->nX, pButton->nY, 0xFFFFFFFF, 5, NULL);

    nMouseX = (uint16_t)CURSOR_GetMouseX();
    nMouseY = (uint16_t)CURSOR_GetMouseY();
    if (nMouseX < pButton->nX || nMouseX > pButton->nX + pButton->nWidth)
        return;
    if (nMouseY < pButton->nY - pButton->nHeight || nMouseY > pButton->nY)
        return;

    nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT16);
    pwszCaption = D2LANG_GetStringFromTblIndex((D2C_StringIndices)gUiButtonImages[pButton->nImageIndex].wCaptionStrId);
    nTextWidth = D2WIN_10121_GetTextWidth(pwszCaption);
    D2WIN_10117_DrawText(pwszCaption, pButton->nX + (pButton->nWidth - nTextWidth) / 2, pButton->nY - pButton->nHeight - 5, 0, 0);
    D2WIN_10127_SetFont((enum Font)nPrevFont);
}


//1.10f: D2Client.0x6FACFAD0
//  Slot 3, a press: inside the button it arms it (bPressed), plays the button-click sound and returns 1, otherwise 0; DIALOG_OnMouseDown reads
//  the answer. The cursor is the message's two word fields, zero-extended.
int32_t __fastcall CHAR_UiButtonOnMouseDown(D2ClientButtonStrc *pButton, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    if (nXpos < pButton->nX || nXpos > pButton->nX + pButton->nWidth || nYpos < pButton->nY - pButton->nHeight || nYpos > pButton->nY)
    {
        return 0;
    }

    pButton->bPressed = 1;
    SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
    return 1;
}


//1.10f: D2Client.0x6FACFB30
//  Slot 4, a release. Only an armed button answers: inside, it disarms and returns what its click handler returns; outside, it clears field
//  +0x2C and disarms. An unarmed button returns 0 and touches nothing.
int32_t __fastcall CHAR_UiButtonOnMouseUp(D2ClientButtonStrc *pButton, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;

    if (!pButton->bPressed)
        return 0;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    if (nXpos >= pButton->nX && nXpos <= pButton->nX + pButton->nWidth && nYpos >= pButton->nY - pButton->nHeight && nYpos <= pButton->nY)
    {
        pButton->bPressed = 0;
        return pButton->pfnOnClick();
    }

    pButton->field_2C = 0;
    pButton->bPressed = 0;
    return 0;
}


//1.10f: D2Client.0x6FACFB90
//  Slot 6, a character: a button marked bFireOnEnter clicks on Enter (wParam 13), but not on a repeat (lParam bit 30). Construct clears the
//  flag and nothing in this file sets it.
int32_t __fastcall CHAR_UiButtonOnChar(D2ClientButtonStrc *pButton, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    if (pButton->bFireOnEnter && (pMsg->lParam & 0x40000000) == 0 && pMsg->wParam == VK_RETURN)
    {
        return pButton->pfnOnClick();
    }
    return 0;
}


//1.10f: D2Client.0x6FACFBC0
//  The proxy's destructor: the wrapped button is sent its own slot 0 with the free flag, then the base destructor runs.
void __fastcall CHAR_UiButtonProxyDestruct(int32_t *pProxy)
{
    void (__thiscall ***pControl)(uint32_t, int32_t);

    *pProxy = (int32_t)(UINT_PTR)&gVtblUiButtonProxy;
    pControl = (void (__thiscall ***)(uint32_t, int32_t))(UINT_PTR)pProxy[2];
    if (pControl)
    {
        (**pControl)((uint32_t)(UINT_PTR)pControl, 1);
    }
    LISTBOX_UiCtrlBaseDestruct(pProxy);
}


//1.10f: D2Client.0x6FACFC20
//  Slot 0 of gVtblUiButtonProxy, the proxy's scalar-deleting destructor.
uint32_t **__fastcall CHAR_UiButtonProxyVecDelDtor(uint32_t **pProxy, int32_t nEdxUnused, uint8_t bFreeObject)
{
    CHAR_UiButtonProxyDestruct((int32_t *)pProxy);
    if ((bFreeObject & 1) != 0 && pProxy)
    {
        STORM_SMemFree(pProxy, szDelete, -1, 0);
    }
    return pProxy;
}


//1.10f: D2Client.0x6FACFC50
//  Builds a proxy around a new button (the gold dialog makes its OK and Cancel this way). The vtable is stored before the allocation returns; a
//  failed allocation stores a null control.
int32_t *__fastcall CHAR_UiButtonProxyConstruct(int32_t *pProxy, int32_t nEdxUnused, int32_t nX, int32_t nY, int32_t nImageIndex, void *pfnOnClick)
{
    D2ClientButtonStrc *pButton;
    int32_t *pConstructed;

    *pProxy = (int32_t)(UINT_PTR)&gVtblUiButtonProxy;
    pButton = (D2ClientButtonStrc *)STORM_SMemAlloc(0x36, szNew, -1, 0);
    if (pButton)
    {
        pConstructed = CHAR_UiButtonConstruct(pButton, 0, nX, nY, nImageIndex, pfnOnClick);
    }
    else
    {
        pConstructed = NULL;
    }
    pProxy[2] = (int32_t)(UINT_PTR)pConstructed;
    return pProxy;
}


//1.10f: D2Client.0x6FACFCD0
//  Draws a line centred in [nLeft, nRight]. The box width is nRight - nLeft + 1; the slack is halved with a signed shift, and only when the
//  text is narrower, so wider text starts at nLeft and is not clipped. The last argument of D2WIN_10117_DrawText (bCentered) is always 0.
void __fastcall CHAR_DrawTextCentered(int32_t nLeft, int32_t nY, int32_t nRight, const Unicode *wszText, int32_t nColor)
{
    int32_t nX;
    int32_t nBoxWidth;
    int32_t nTextWidth;

    nX = nLeft;
    nBoxWidth = nRight - nLeft + 1;
    nTextWidth = D2WIN_10121_GetTextWidth(wszText);
    if (nTextWidth < nBoxWidth)
        nX += (nBoxWidth - nTextWidth) >> 1;
    D2WIN_10117_DrawText(wszText, nX, nY, nColor, 0);
}


//1.10f: D2Client.0x6FACFD10
//  Cuts a string at its first newline, in place. *ppRest is cleared first; on success the newline becomes a terminator and *ppRest is the
//  character after it. Returns 1 on success, 0 when there is no newline.
//  The test nCutIndex >= nLen can never hold: the walk stops on a newline only before the terminator, so the index is always below
//  Unicode::strlen. It is kept because the original has it, with the terminator written before it.
int32_t __fastcall CHAR_TextCutNextLine(Unicode *pText, Unicode **ppRest)
{
    Unicode *pCursor;
    int32_t nLen;
    int32_t nCutIndex;

    pCursor = pText;
    *ppRest = NULL;
    nLen = Unicode::strlen(pText);
    nCutIndex = 0;
    if (!pText->ch)
        return 0;

    while (pCursor->ch != 10)
    {
        ++pCursor;
        ++nCutIndex;
        if (!pCursor->ch)
            return 0;
    }

    pText[nCutIndex].ch = 0;
    if (nCutIndex >= nLen)
        return 0;

    *ppRest = pCursor + 1;
    return 1;
}


//  Immediates used by CHAR_CharPanelDraw.
static_assert(offsetof(D2GfxDataStrc, pCellFile) == 4
                  && offsetof(D2GfxDataStrc, nFrame) == 8
                  && offsetof(D2GfxDataStrc, nDirection) == 0x0C
                  && sizeof(D2GfxDataStrc) == 0x12 * 4,
              "mov ecx, 12h / rep stosd clears the cel context; the image goes "
              "to +4 and the frame to +8");
static_assert(sizeof(POINT) * 3 == 24,
              "gCharPanelHiliteBoxes: lea esi, [esi+esi*2] / shl esi, 3 -- a row "
              "is three POINTs, read at +0/+4, +8/+0Ch and +10h/+14h");
static_assert(STAT_HITPOINTS == 6 && STAT_MAXHP == 7 && STAT_MANA == 8
                  && STAT_MAXMANA == 9 && STAT_STAMINA == 10
                  && STAT_MAXSTAMINA == 11 && STAT_EXPERIENCE == 13
                  && STAT_NEXTEXP == 30 && STAT_ARMORCLASS == 31
                  && STAT_MAGICRESIST == 37 && STAT_FIRERESIST == 39
                  && STAT_LIGHTRESIST == 41 && STAT_COLDRESIST == 43
                  && STAT_POISONRESIST == 45,
              "the stat-row switch: add ecx, -6 / cmp ecx, 27h over "
              "byte_6FAD0FC0, then add eax, -25h / cmp eax, 8 for the resists");
static_assert(STAT_MAXMAGICRESIST == 0x26 && STAT_MAXFIRERESIST == 0x28
                  && STAT_MAXLIGHTRESIST == 0x2A && STAT_MAXCOLDRESIST == 0x2C
                  && STAT_MAXPOISONRESIST == 0x2E
                  && STAT_ARMORCLASS_VS_HTH == 0x21,
              "push 26h / 28h / 2Ah / 2Ch / 2Eh, and push 21h at 0x6FAD0265");
static_assert(STATE_HOLYSHIELD == 0x65 && LANGUAGE_JAPANESE == 6
                  && PLRMODE_DEAD == 0x11 && WEAPONCLASS_HT2 == 13,
              "push 65h at 0x6FAD0D14, cmp eax, 6 at 0x6FAD0D5E, "
              "cmp [eax+10h], 11h at 0x6FAD0AAB, cmp slot, 0Dh at 0x6FAD0762");
static_assert(NUM_FONTS == 14 && D2FONT_FONT8 == 0 && D2FONT_FONT16 == 1
                  && D2FONT_FONT6 == 6 && DRAWMODE_TRANS75 == 2,
              "mov ebp, 0Eh is the no-font-change sentinel; SetFont(0/1/6); "
              "push 2 as the draw mode");
static_assert(STR_IDX_4144_strClose == 0x1030
                  && STR_IDX_4075_strchrstat == 0xFEB
                  && STR_IDX_4076_strchrrema == 0xFEC
                  && STR_IDX_4159_charavghit == 0x103F
                  && STR_IDX_10103_charmonsterX == 0x2777
                  && STR_IDX_10104_charmontohit1X == 0x2778
                  && STR_IDX_10105_charmontohit2X == 0x2779,
              "mov ecx, 1030h / 0FEBh / 0FECh / 103Fh / 2777h / 2778h / 2779h");
static_assert(offsetof(D2MonStatsInitStrc, nTH) == 0x0C
                  && offsetof(D2DifficultyLevelsTxt, dwResistPenalty) == 0
                  && offsetof(D2CharStatsTxt, wszClassName) == 0,
              "mov edi, [esp+3C0h+var_31C+0Ch]; add esi, [ecx]; the class "
              "record itself is handed to GetTextWidth as its name");


//1.10f: D2Client.0x6FACFD60
//  Draws the character screen, once a frame while UI_STATSCREEN is open, in the eleven numbered sections of the body. The original inlines
//  CHAR_DrawTextCentered at several sites and CHAR_TextCutNextLine in the label loop; ours calls them.
//  D2ExpRes patches eight sites in this function; each is marked on the line its detour lands on.
void __cdecl CHAR_CharPanelDraw()
{
    D2UnitStrc *pUnit;
    D2GfxDataStrc tCelContext;
    int32_t nColor;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nX;
    int32_t nY;
    UINT_PTR nButton;   // an integer cursor over gCharPanelStatButtons
    int32_t nButtonX;
    int32_t nButtonY;
    uint32_t i;
    int32_t nLabelLeft;
    int32_t nLabelRight;
    int32_t nLabelY;
    Unicode *pwszLabelRest;
    int32_t nChanceToHitLeftSkill;
    int32_t nChanceToHitRightSkill;
    D2UnitStrc *pDefender;
    int32_t nDefenseTotal;
    int32_t nDifficulty;
    int32_t nRefMonsterClass;
    D2MonStatsTxt *pRefMonStats;
    D2MonStatsInitStrc tMonStats;
    int32_t nRefMonsterLevel;
    int32_t nGameType;
    int32_t nToHit;
    int32_t nDefense;
    int32_t nPercent;
    int32_t nToBeHitPercent;
    int32_t bOverLeftSkillRow;
    int32_t bOverRightSkillRow;
    int32_t nHiliteRow;
    int32_t nHiliteValue;
    uint8_t nHiliteColor;      //  a byte, as GFX_DrawFilledRect's colour parameter is
    D2MonStatsTxt *pNameMonStats;
    const Unicode *pwszMonsterName;
    const Unicode *pwszFormat;
    int32_t nToBeHitClamped;
    D2UnitStrc *pBlocker;
    int32_t nBlockRate;
    int32_t nWeaponClass;
    D2CharStatsTxt *pCharStatsTxt;
    char *szPlayerName;
    uint32_t nNameWidth;
    int32_t nStatRow;
    int32_t *pRow;
    D2C_ItemStats nStatId;
    int32_t bCheckFits;
    int32_t nStatValue;
    int32_t nBaseValue;
    D2UnitStrc *pPlayer;
    D2DifficultyLevelsTxt *pDifficultyTxt;
    int32_t nMaxResistBonus;
    int32_t nResistCap;
    D2InventoryStrc *pInventory;
    int32_t nClass;
    int32_t nLevel;
    int32_t nPrevFont;
    int32_t nTextWidth;
    int32_t nTextHeight;
    char szBaseStat[12];
    char szStatValue[128];

    pUnit = CUNIT_GetPlayer();

    //  ---- 1. the background ------------------------------------------------
    memset(&tCelContext, 0, sizeof(tCelContext));
    tCelContext.pCellFile = gpImgInvCharPanel;
    nColor = 0;
    tCelContext.nDirection = 0;
    tCelContext.nFrame = 0;
    D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
    tCelContext.nFrame = 1;
    D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference + 256, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
    tCelContext.nFrame = 2;
    D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
    tCelContext.nFrame = 3;
    D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference + 256, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();

    //  BUG: a Unicode[50] is constructed here and never read. LATENT: it only costs the construction.
    Unicode wszUnused[50];
    (void)wszUnused;

    //  ---- 2. "Close" over the close button ---------------------------------
    //  D2ExpRes CharSheetClosebuttonMouseHoverOver lands on the first compare
    if (nMouseX >= gnBorderXDifference + 128 && nMouseX <= gnBorderXDifference + 160 && nMouseY >= gnBorderYDifference + gnScreenHeight - 92 && nMouseY <= gnBorderYDifference + gnScreenHeight - 60)
    {
        //  D2ExpRes CharSheetClosestring lands on the y
        nY = gnBorderYDifference + gnScreenHeight - 95;
        nX = gnBorderXDifference + 143;
        D2WIN_10129_DrawFramedText(D2LANG_GetStringFromTblIndex(STR_IDX_4144_strClose), nX, nY, 0, 1);
    }

    //  ---- 3. unspent points ------------------------------------------------
    if (D2COMMON_STATLIST_GetUnitBaseStat(pUnit, STAT_STATPTS, 0))
    {
        tCelContext.pCellFile = gpImgSkillPoints;
        tCelContext.nFrame = 0;
        D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference + 3, gnBorderYDifference + gnScreenHeight - 116, 0xFFFFFFFF, 5, NULL);

        Unicode wszPoints[10];

        D2WIN_10127_SetFont(D2FONT_FONT6);

        //  D2ExpRes CharSheetStatPointsString / CharSheetRemainingString land on the two y computations.
        CHAR_DrawTextCentered(gnBorderXDifference + 11, gnBorderYDifference + gnScreenHeight - 125, gnBorderXDifference + 88, D2LANG_GetStringFromTblIndex(STR_IDX_4075_strchrstat), 1);
        CHAR_DrawTextCentered(gnBorderXDifference + 11, gnBorderYDifference + gnScreenHeight - 117, gnBorderXDifference + 88, D2LANG_GetStringFromTblIndex(STR_IDX_4076_strchrrema), 1);

        wsprintfA(szBaseStat, "%i", D2COMMON_STATLIST_GetUnitBaseStat(pUnit, STAT_STATPTS, 0));
        D2WIN_10127_SetFont(D2FONT_FONT16);
        Unicode::win2Unicode(wszPoints, szBaseStat, 9);

        //  D2ExpRes CharSheetStatPointsRemainingValue lands on the y.
        CHAR_DrawTextCentered(gnBorderXDifference + 92, gnBorderYDifference + gnScreenHeight - 120, gnBorderXDifference + 127, wszPoints, 0);
    }

    if (D2COMMON_STATLIST_GetUnitBaseStat(pUnit, STAT_STATPTS, 0))
    {
        nButton = (UINT_PTR)&gCharPanelStatButtons[4];
        do
        {
            nButtonX = *((uint32_t *)nButton - 1);
            nButtonY = gnBorderYDifference + *(uint32_t *)nButton;
            tCelContext.pCellFile = gpImgLevelUpSocket;
            nY = nButtonY + gnScreenHeight - 480;
            nX = gnBorderXDifference + nButtonX;
            tCelContext.nFrame = 0;
            D2GFX_TEXTURE_CelDraw(&tCelContext, nX + 5, nY + 5, 0xFFFFFFFF, 5, NULL);

            tCelContext.pCellFile = gpImgLevelUpButton;
            //  D2ExpRes CharSheetGreyButtons lands on this store
            tCelContext.nFrame = *((uint32_t *)nButton + 1) != 0;
            D2GFX_TEXTURE_CelDraw(&tCelContext, nX + 8, nY + 1, 0xFFFFFFFF, 5, NULL);

            nButton += 14;
        }
        //  DIFFERENCE: the original's loop ends on a signed compare of its pointer against the table + 60, four bytes past the 56-byte table. A
        //  pointer there would leave our table, so the cursor is an integer bounded from the table, compared signed as the original does, and
        //  read only inside the table.
        while ((int32_t)nButton < (int32_t)((UINT_PTR)gCharPanelStatButtons + 60));
    }

    //  ---- 4. the fifteen labels --------------------------------------------
    D2WIN_10127_SetFont(D2FONT_FONT6);
    for (i = 0; i < 15; ++i)
    {
        Unicode wszLabel[100];

        Unicode::strncpy(wszLabel, D2LANG_GetStringFromTblIndex((D2C_StringIndices)gCharPanelLabels[i].wStringId), 99);
        nLabelLeft = gnBorderXDifference + gCharPanelLabels[i].nLeft;
        nLabelRight = gnBorderXDifference + gCharPanelLabels[i].nRight;
        nLabelY = gnBorderYDifference + gCharPanelLabels[i].nY;
        wszLabel[99].ch = 0;
        nY = nLabelY + gnScreenHeight - 480;

        if (CHAR_TextCutNextLine(wszLabel, &pwszLabelRest))
        {
            CHAR_DrawTextCentered(nLabelLeft, nY - 4, nLabelRight, wszLabel, 0);
            CHAR_DrawTextCentered(nLabelLeft, nY + 4, nLabelRight, pwszLabelRest, 0);
        }
        else
        {
            CHAR_DrawTextCentered(nLabelLeft, nY, nLabelRight, wszLabel, 0);
        }
    }

    //  ---- 5. the skills and the two chance-to-hit figures ------------------
    //  The two chance-to-hit figures are one per mouse skill: SKILLDESC_GetAttackRatingForHand's flag picks UNITS_GetLeftSkill or
    //  UNITS_GetRightSkill, not a hand.
    SKILLDESC_DrawBothSkillInfos(pUnit);
    D2WIN_10127_SetFont(D2FONT_FONT8);
    nChanceToHitLeftSkill = CHAR_CharPanelChanceToHitPercent(SKILLDESC_GetAttackRatingForHand(pUnit, 1));
    nChanceToHitRightSkill = CHAR_CharPanelChanceToHitPercent(SKILLDESC_GetAttackRatingForHand(pUnit, 0));

    //  ---- 6. the reference monster's chance to hit the player --------------
    //  The same computation as CHAR_CharPanelChanceToHitPercent with the roles swapped, written out rather than called:
    //  SELECT_GetToBeHitRefMonster() is the attacker, its nTH the rating, and the defence is UNITS_GetDefense + STAT_ARMORCLASS_VS_HTH of a
    //  unit fetched again with CUNIT_GetPlayer:
    //      nPercent  = 100 * TH / (TH + DEF)             100 when both are 0
    //      nToBeHit  = 2 * nPercent * mlvl / (clvl + mlvl)
    //  nTH is asked for up to three times, with nFlags 8, 16 and 32, until one answer is non-zero (the stats buffer is cleared before each).
    //  The classic handicap is 10 / 15 here, where ChanceToHitPercent's is 10 / 12. A reference outside monstats sets the figure to 0 with no
    //  assert, while section 8 asserts on the same getter. The result is clamped to 5..95 after section 7, not here.
    pDefender = CUNIT_GetPlayer();
    nPercent = 100;
    nDefenseTotal = D2COMMON_UNITS_GetDefense(pDefender);
    nDefenseTotal += D2COMMON_STATLIST_UnitGetStatValue(pDefender, STAT_ARMORCLASS_VS_HTH, 0);

    nDifficulty = (uint8_t)GAME_GetDifficulty();
    if (nDifficulty >= NUM_DIFFICULTIES)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR)szDifficultyLevelAssert, (uint32_t)(UINT_PTR)szCharCpp, 470);
        exit(-1);
    }

    nRefMonsterClass = SELECT_GetToBeHitRefMonster();
    if (nRefMonsterClass < 0 || nRefMonsterClass >= sgptDataTables->nMonStatsTxtRecordCount || (pRefMonStats = &sgptDataTables->pMonStatsTxt[nRefMonsterClass]) == NULL)
    {
        nToBeHitPercent = 0;
    }
    else
    {
        memset(&tMonStats, 0, sizeof(tMonStats));
        nRefMonsterLevel = (int16_t)pRefMonStats->nLevel[nDifficulty];
        nGameType = GAME_IsLadderGame();
        D2COMMON_DATATBLS_CalculateMonsterStatsByLevel(nRefMonsterClass, nGameType, nDifficulty, nRefMonsterLevel, 8, &tMonStats);
        nToHit = tMonStats.nTH;
        if (!nToHit)
        {
            memset(&tMonStats, 0, sizeof(tMonStats));
            nRefMonsterLevel = (int16_t)pRefMonStats->nLevel[nDifficulty];
            nGameType = GAME_IsLadderGame();
            D2COMMON_DATATBLS_CalculateMonsterStatsByLevel(nRefMonsterClass, nGameType, nDifficulty, nRefMonsterLevel, 16, &tMonStats);
            nToHit = tMonStats.nTH;
            if (!nToHit)
            {
                memset(&tMonStats, 0, sizeof(tMonStats));
                nRefMonsterLevel = (int16_t)pRefMonStats->nLevel[nDifficulty];
                nGameType = GAME_IsLadderGame();
                D2COMMON_DATATBLS_CalculateMonsterStatsByLevel(nRefMonsterClass, nGameType, nDifficulty, nRefMonsterLevel, 32, &tMonStats);
                nToHit = tMonStats.nTH;
            }
        }

        if (pRefMonStats->nAlign != 1 && !GAME_IsExpansionGame() && GAME_GetDifficulty())
        {
            nToHit = 10 * nToHit / 15;
        }

        nDefense = nDefenseTotal;
        if (nDefense < 0)
        {
            nToHit -= nDefense;
            nDefense = 0;
        }
        if (nToHit < 0)
        {
            nDefense -= nToHit;
            nToHit = 0;
        }
        if (nToHit || nDefense)
            nPercent = 100 * nToHit / (nToHit + nDefense);

        nRefMonsterLevel = (int16_t)pRefMonStats->nLevel[nDifficulty];
        //  BUG: the divisor (clvl + mlvl) is never tested for zero. LATENT: the local player's level is at least 1.
        nToBeHitPercent = nRefMonsterLevel * nPercent * 2 / (D2COMMON_STATLIST_UnitGetStatValue(pDefender, STAT_LEVEL, 0) + nRefMonsterLevel);
    }

    //  ---- 7. the highlight over a chance-to-hit row ------------------------
    {
        Unicode wszHitLine[128];

        PANEL_SetBoxFillColorBlack();

        bOverLeftSkillRow = 0;
        bOverRightSkillRow = 0;
        if (nMouseX >= gnBorderXDifference + 162 && nMouseX <= gnBorderXDifference + 320 && nMouseY >= gnBorderYDifference + gnScreenHeight - 334 && nMouseY <= gnBorderYDifference + gnScreenHeight - 315)
        {
            bOverLeftSkillRow = 1;
        }
        else if (nMouseX >= gnBorderXDifference + 162 && nMouseX <= gnBorderXDifference + 320 && nMouseY >= gnBorderYDifference + gnScreenHeight - 308 && nMouseY <= gnBorderYDifference + gnScreenHeight - 289)
        {
            bOverRightSkillRow = 1;
        }

        if (!nChanceToHitLeftSkill)
            bOverLeftSkillRow = 0;
        if (!nChanceToHitRightSkill)
            bOverRightSkillRow = 0;

        if (bOverLeftSkillRow || bOverRightSkillRow)
        {
            if (bOverLeftSkillRow)
            {
                nHiliteRow = 0;
                nHiliteValue = nChanceToHitLeftSkill;
            }
            else
            {
                nHiliteRow = 1;
                nHiliteValue = nChanceToHitRightSkill;
            }

            if (nHiliteValue < 5)
                nHiliteValue = 5;
            else if (nHiliteValue > 95)
                nHiliteValue = 95;

            //  BUG: gnCharPanelHiliteColor, which both this box and the tooltip below use as their colour, is never written (the loader
            //  zero-fills it), so the colour is always palette index 0. NOT ESTABLISHED: whether 0 was meant cannot be told from inside the
            //  module. The box reads the low byte and the tooltip masks the dword with 0FFh; both are uint8_t here.
            nHiliteColor = (uint8_t)gnCharPanelHiliteColor;
            GFX_DrawFilledRect(gnBorderXDifference + gCharPanelHiliteBoxes[nHiliteRow][0].x, gCharPanelHiliteBoxes[nHiliteRow][0].y + gnBorderYDifference + gnScreenHeight - 480, 155, 30, nHiliteColor, DRAWMODE_TRANS75);

            nX = gnBorderXDifference + gCharPanelHiliteBoxes[nHiliteRow][1].x;
            nY = gnBorderYDifference + gCharPanelHiliteBoxes[nHiliteRow][1].y + gnScreenHeight - 480;
            D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4159_charavghit), nX, nY, 0, 0);

            nRefMonsterClass = SELECT_GetToHitRefMonster();
            if (nRefMonsterClass < 0 || nRefMonsterClass >= sgptDataTables->nMonStatsTxtRecordCount || (pNameMonStats = &sgptDataTables->pMonStatsTxt[nRefMonsterClass]) == NULL)
            {
                FOG_DisplayAssert((uint32_t)(UINT_PTR)szPtStats, (uint32_t)(UINT_PTR)szCharCpp, 236);
                exit(-1);
            }

            pwszMonsterName = D2LANG_GetStringFromTblIndex((D2C_StringIndices)pNameMonStats->wNameStr);
            pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_10103_charmonsterX);
            Unicode::sprintf(128, wszHitLine, pwszFormat, pwszMonsterName, nHiliteValue);
            D2WIN_10117_DrawText(wszHitLine, gnBorderXDifference + gCharPanelHiliteBoxes[nHiliteRow][2].x, gnBorderYDifference + gCharPanelHiliteBoxes[nHiliteRow][2].y + gnScreenHeight - 480, 0, 0);
        }
    }

    nToBeHitClamped = nToBeHitPercent;
    if (nToBeHitClamped < 5)
        nToBeHitClamped = 5;
    else if (nToBeHitClamped > 95)
        nToBeHitClamped = 95;

    //  ---- 8. the tooltip over the defence box ------------------------------
    {
        Unicode wszBlockLine[128];

        if (nMouseX >= gnBorderXDifference + 173 && nMouseX <= gnBorderXDifference + 311 && nMouseY >= gnBorderYDifference + gnScreenHeight - 286 && nMouseY <= gnBorderYDifference + gnScreenHeight - 267)
        {
            pBlocker = CUNIT_GetPlayer();
            nBlockRate = D2COMMON_UNITS_GetBlockRate(pBlocker, GAME_IsExpansionGame());
            if (nBlockRate <= 0)
            {
                //  the passive weapon block counts only with two claws
                nBlockRate = CHAR_CharPanelGetWeaponBlockPercent(pBlocker);
                if (nBlockRate > 0)
                {
                    //  BUG: the weapon-class out slot is not initialised here; in the original it still holds the right skill's chance to hit,
                    //  and D2Common returns without writing it on two paths, so that value is carried in (nWeaponClass starts as
                    //  nChanceToHitRightSkill). LATENT: neither path is open for a unit that has a block stat.
                    nWeaponClass = nChanceToHitRightSkill;
                    D2COMMON_COMPOSIT_GetWeaponClassId(pBlocker, pBlocker->pInventory, &nWeaponClass, -1, 1);
                    if (nWeaponClass != WEAPONCLASS_HT2)
                        nBlockRate = 0;
                }
            }

            nRefMonsterClass = SELECT_GetToBeHitRefMonster();
            if (nRefMonsterClass < 0 || nRefMonsterClass >= sgptDataTables->nMonStatsTxtRecordCount || (pNameMonStats = &sgptDataTables->pMonStatsTxt[nRefMonsterClass]) == NULL)
            {
                FOG_DisplayAssert((uint32_t)(UINT_PTR)szPtStats, (uint32_t)(UINT_PTR)szCharCpp, 348);
                exit(-1);
            }

            pwszMonsterName = D2LANG_GetStringFromTblIndex((D2C_StringIndices)pNameMonStats->wNameStr);
            if (nBlockRate)
            {
                pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_10105_charmontohit2X);
                Unicode::sprintf(128, wszBlockLine, pwszFormat, nBlockRate, nToBeHitClamped, pwszMonsterName, nToBeHitClamped);
            }
            else
            {
                pwszFormat = D2LANG_GetStringFromTblIndex(STR_IDX_10104_charmontohit1X);
                Unicode::sprintf(128, wszBlockLine, pwszFormat, pwszMonsterName, nToBeHitClamped);
            }

            D2WIN_10132(wszBlockLine, gnBorderXDifference + 139, gnBorderYDifference + gnScreenHeight - 315, (uint8_t)gnCharPanelHiliteColor, DRAWMODE_TRANS75, 0);
        }
    }

    //  ---- 9. the close button ----------------------------------------------
    D2WIN_10127_SetFont(D2FONT_FONT16);
    tCelContext.pCellFile = UI_GetBuySellButtonImage();
    tCelContext.nFrame = (gbStatsButtonDown != 0) + 10;
    //  D2ExpRes CharSheetClosebutton lands on the y
    D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference + 128, gnBorderYDifference + gnScreenHeight - 60, 0xFFFFFFFF, 5, NULL);

    Unicode wszValue[128];

    //  ---- 10. class and name -----------------------------------------------
    if (pUnit && pUnit->dwUnitType == UNIT_PLAYER)
    {
        if (pUnit->dwClassId < 0 || (int32_t)pUnit->dwClassId >= sgptDataTables->nCharStatsTxtRecordCount)
        {
            pCharStatsTxt = NULL;
        }
        else
        {
            pCharStatsTxt = &sgptDataTables->pCharStatsTxt[pUnit->dwClassId];
        }

        //  BUG: a NULL charstats record is not tested, so its name (at +0) would be passed to D2Win as NULL. LATENT: the seven class ids match
        //  charstats' seven records.
        CHAR_DrawTextCentered(gnBorderXDifference + 193, gnBorderYDifference + gnScreenHeight - 455, gnBorderXDifference + 310, pCharStatsTxt->wszClassName, 0);

        szPlayerName = ROSTER_GetPlayerDisplayName(pUnit);
        nNameWidth = Unicode::unicodenwidth(szPlayerName, (int32_t)strlen(szPlayerName)) + 1;
        if (nNameWidth > 11)
            D2WIN_10127_SetFont(nNameWidth < 14 ? D2FONT_FONT8 : D2FONT_FONT6);
        Unicode::win2Unicode(wszValue, szPlayerName, 128);

        //  D2ExpRes CharSheetCharname lands on the y.
        CHAR_DrawTextCentered(gnBorderXDifference + 13, gnBorderYDifference + gnScreenHeight - 455, gnBorderXDifference + 160, wszValue, 0);
        D2WIN_10127_SetFont(D2FONT_FONT16);
    }

    //  ---- 11. the eighteen stat rows ---------------------------------------
    //  Each row of gCharPanelStatRows is {left, y, right, stat} (pRow[0..3]). nColor is 0 by default, 3 (blue) above base or in a blue state, 1
    //  (red) below base, in a red state or for a negative resistance, and 4 for a resistance at its cap. bCheckFits marks the rows whose text
    //  is drawn in Font8 (and the font put back) when the value is 1000 or more or the text is as wide as its box; 14 (NUM_FONTS) is the "font
    //  not changed" sentinel.
    for (nStatRow = 0; (uint32_t)nStatRow < 18; ++nStatRow)
    {
        pRow = &gCharPanelStatRows[4 * nStatRow];
        bCheckFits = 0;
        nStatId = (D2C_ItemStats)pRow[3];
        nStatValue = D2COMMON_STATLIST_UnitGetStatValue(pUnit, nStatId, 0);
        nBaseValue = D2COMMON_STATLIST_GetUnitBaseStat(pUnit, nStatId, 0);

        switch (pRow[3])
        {
        case STAT_HITPOINTS:
            nStatValue >>= 8;
            pPlayer = CUNIT_GetPlayer();
            if (pPlayer && pPlayer->dwUnitType == UNIT_PLAYER && pPlayer->dwAnimMode != PLRMODE_DEAD && nStatValue >= 0 && nStatValue <= 1)
            {
                nStatValue = 1;
            }
            wsprintfA(szStatValue, "%ld", nStatValue);
            bCheckFits = 1;
            break;

        case STAT_MAXHP:
        case STAT_MAXMANA:
        case STAT_MAXSTAMINA:
            if (nStatValue > nBaseValue)
                nColor = 3;
            else if (nStatValue < nBaseValue)
                nColor = 1;
            nStatValue >>= 8;
            wsprintfA(szStatValue, "%ld", nStatValue);
            bCheckFits = 1;
            break;

        case STAT_MANA:
        case STAT_STAMINA:
            nStatValue >>= 8;
            wsprintfA(szStatValue, "%ld", nStatValue);
            bCheckFits = 1;
            break;

        case STAT_NEXTEXP:
            nLevel = D2COMMON_STATLIST_GetUnitBaseStat(pUnit, STAT_LEVEL, 0);
            nClass = pUnit ? (int32_t)pUnit->dwClassId : -1;
            if (nLevel != D2COMMON_DATATBLS_GetMaxLevel(nClass))
                nStatValue = (int32_t)D2COMMON_DATATBLS_GetLevelThreshold(nClass, nLevel);
            //  fall through: both are printed the same way
        case STAT_EXPERIENCE:
            D2LANG_STR_GroupUintDigits(wszValue, nStatValue, 128);
            CHAR_DrawTextCentered(gnBorderXDifference + pRow[0], gnBorderYDifference + pRow[1] + gnScreenHeight - 480, gnBorderXDifference + pRow[2], wszValue, 0);
            nColor = 0;
            continue;

        case STAT_ARMORCLASS:
            nStatValue = D2COMMON_UNITS_GetDefense(pUnit);
            if (D2COMMON_STATES_CheckStateMaskArmBlueOnUnit(pUnit))
                nColor = 3;
            //  BUG: pUnit is not tested before pUnit->pInventory is read, where every other use of pUnit here goes through a function that
            //  tests it. LATENT: the only caller draws while the local player exists.
            pInventory = pUnit->pInventory;
            if (pInventory && D2COMMON_INVENTORY_GetEquippedShield(pInventory, NULL) && D2COMMON_STATES_CheckState(pUnit, STATE_HOLYSHIELD))
            {
                nColor = 3;
            }
            if (D2COMMON_STATES_CheckStateMaskArmRedOnUnit(pUnit))
                nColor = 1;
            wsprintfA(szStatValue, "%ld", nStatValue);
            if (nStatValue >= 1000 || D2LANG_STRTABLE_GetLanguage() == LANGUAGE_JAPANESE)
                bCheckFits = 1;
            break;

        case STAT_MAGICRESIST:
        case STAT_FIRERESIST:
        case STAT_LIGHTRESIST:
        case STAT_COLDRESIST:
        case STAT_POISONRESIST:
            pDifficultyTxt = D2COMMON_DATATBLS_GetDifficultyLevelsTxtRecord((uint8_t)GAME_GetDifficulty());
            nMaxResistBonus = 0;
            switch (pRow[3])
            {
            case STAT_MAGICRESIST:
                nMaxResistBonus = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_MAXMAGICRESIST, 0);
                break;
            case STAT_FIRERESIST:
                if (D2COMMON_STATES_CheckStateMaskRFBlueOnUnit(pUnit))
                    nColor = 3;
                if (D2COMMON_STATES_CheckStateMaskRFRedOnUnit(pUnit))
                    nColor = 1;
                nMaxResistBonus = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_MAXFIRERESIST, 0);
                break;
            case STAT_LIGHTRESIST:
                if (D2COMMON_STATES_CheckStateMaskRLBlueOnUnit(pUnit))
                    nColor = 3;
                if (D2COMMON_STATES_CheckStateMaskRLRedOnUnit(pUnit))
                    nColor = 1;
                nMaxResistBonus = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_MAXLIGHTRESIST, 0);
                break;
            case STAT_COLDRESIST:
                if (D2COMMON_STATES_CheckStateMaskRCBlueOnUnit(pUnit))
                    nColor = 3;
                if (D2COMMON_STATES_CheckStateMaskRCRedOnUnit(pUnit))
                    nColor = 1;
                nMaxResistBonus = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_MAXCOLDRESIST, 0);
                break;
            case STAT_POISONRESIST:
                if (D2COMMON_STATES_CheckStateMaskRPBlueOnUnit(pUnit))
                    nColor = 3;
                if (D2COMMON_STATES_CheckStateMaskRPRedOnUnit(pUnit))
                    nColor = 1;
                nMaxResistBonus = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_MAXPOISONRESIST, 0);
                break;
            default:
                //  unreachable: the outer switch lets only these five in
                break;
            }

            if (GAME_IsExpansionGame())
            {
                nStatValue += (int32_t)pDifficultyTxt->dwResistPenalty;
            }
            else
            {
                //  Redundant, as in the original: it reloads 0 here from a frame slot; bCheckFits was already 0.
                bCheckFits = 0;
                switch ((uint8_t)GAME_GetDifficulty())
                {
                case 1:
                    nStatValue -= 20;
                    break;
                case 2:
                    nStatValue -= 50;
                    break;
                }
            }

            nResistCap = nMaxResistBonus + 75;
            if (nResistCap >= 95)
                nResistCap = 95;
            if (nStatValue <= -100)
                nStatValue = -100;

            if (nStatValue >= nResistCap)
            {
                nStatValue = nResistCap;
                if (nColor != 3)
                {
                    nColor = 4;
                    wsprintfA(szStatValue, "%ld", nStatValue);
                    break;
                }
            }
            if (nStatValue < 0)
                nColor = 1;
            wsprintfA(szStatValue, "%ld", nStatValue);
            break;

        default:
            if (nStatValue > nBaseValue)
                nColor = 3;
            else if (nStatValue < nBaseValue)
                nColor = 1;
            wsprintfA(szStatValue, "%ld", nStatValue);
            break;
        }

        Unicode::win2Unicode(wszValue, szStatValue, 128);
        wszValue[127].ch = 0;

        nPrevFont = NUM_FONTS;
        if (bCheckFits)
        {
            if (nStatValue >= 1000)
            {
                nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT8);
            }
            else
            {
                D2WIN_10131_GetTextDimensions(wszValue, &nTextWidth, &nTextHeight);
                if (nTextWidth >= pRow[2] - pRow[0])
                    nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT8);
            }
        }

        CHAR_DrawTextCentered(gnBorderXDifference + pRow[0], gnBorderYDifference + pRow[1] + gnScreenHeight - 480, gnBorderXDifference + pRow[2], wszValue, nColor);

        nColor = 0;
        if (nPrevFont != NUM_FONTS)
            D2WIN_10127_SetFont((enum Font)nPrevFont);
    }
}


//1.10f: D2Client.0x6FAD1010
//  Returns the best STAT_PASSIVE_WEAPONBLOCK that applies, the number the character screen's block line adds on. Up to 32 instances of the stat
//  are copied out with STATLIST_CopyStats, and an instance's layer is read as an item type: layer 0 applies to anything, any other layer only
//  when the item in the left arm slot, or else the one in the right, is of that type. A unit without an inventory still walks its stats, so a
//  layer-0 instance is still returned.
int32_t __fastcall CHAR_CharPanelGetWeaponBlockPercent(D2UnitStrc *pUnit)
{
    int32_t nBestPercent;
    D2InventoryStrc *pInventory;
    D2UnitStrc *pRightArmItem;
    D2UnitStrc *pLeftArmItem;
    int32_t nStatCount;
    int32_t iStat;
    int32_t nValue;
    int32_t nLayer;
    D2StatStrc aStats[32];

    nBestPercent = 0;
    if (!pUnit)
        return 0;

    pInventory = pUnit->pInventory;
    pRightArmItem = NULL;
    pLeftArmItem = NULL;
    if (pInventory)
    {
        pRightArmItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RARM);
        pLeftArmItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_LARM);
    }

    nStatCount = D2COMMON_STATLIST_CopyStats(pUnit, STAT_PASSIVE_WEAPONBLOCK, aStats, 32);
    for (iStat = 0; iStat < nStatCount; ++iStat)
    {
        nValue = aStats[iStat].nValue;
        nLayer = aStats[iStat].nLayer;

        if (nLayer == 0)
        {
            //  BUG: a layer-0 instance is assigned without being compared with the running maximum, so it replaces it: a layer-0 value copied
            //  after a larger item-typed one lowers the answer (the original's jle at 0x6FAD107D jumps to the plain mov at 0x6FAD10A9, past the
            //  cmp at 0x6FAD10A5). LATENT: the stat's only stock source is the skill Weapon Block, whose passive stats carry its item type h2h
            //  as their layer, so no layer-0 instance exists in stock data.
            nBestPercent = nValue;
            continue;
        }

        if ((pLeftArmItem && D2COMMON_ITEMS_CheckItemTypeId(pLeftArmItem, (D2C_ItemTypes)nLayer)) || (pRightArmItem && D2COMMON_ITEMS_CheckItemTypeId(pRightArmItem, (D2C_ItemTypes)nLayer)))
        {
            if (nValue > nBestPercent)
                nBestPercent = nValue;
        }
    }

    return nBestPercent;
}


//1.10f: D2Client.0x6FAD10D0
//  Returns the character screen's chance to hit (clamped to 5..95) for an attack rating; CHAR_CharPanelDraw calls it once per mouse skill. The
//  reference is one monster class, SELECT_GetToHitRefMonster(), built for the current difficulty at its monstats level; its nAC is the defence:
//      nPercent     = 100 * AR / (AR + AC)            100 when both are 0
//      nChanceToHit = 2 * nPercent * clvl / (clvl + mlvl), clamped 5..95
//  with AR the argument plus the class's charstats ToHitFactor (for a real player unit only) and mlvl the reference's level again. A negative
//  side is folded into the other, not clamped. A classic-only handicap (an unaligned reference, a non-expansion game, a difficulty past Normal)
//  scales the defence by 10 / 12, signed. mlvl is read signed (the (int16_t) casts) and the difficulty is the byte GAME_GetDifficulty returned.
//  pPlayer may be NULL here: STATLIST_UnitGetStatValue answers 0 for a NULL unit, so the divisor is then mlvl alone. The NULL test on the
//  monstats row (the ptStats assert) is the original's.
int32_t __fastcall CHAR_CharPanelChanceToHitPercent(int32_t nAttackRating)
{
    D2UnitStrc *pPlayer;
    int32_t nDifficulty;
    int32_t nRefMonsterClass;
    D2MonStatsTxt *pMonStatsTxt;
    D2MonStatsInitStrc tMonStats;
    int32_t nRefMonsterLevel;
    int32_t nGameType;
    int32_t nRefDefense;
    int32_t nEffectiveAR;
    int32_t nClassId;
    D2CharStatsTxt *pCharStatsTxt;
    int32_t nPercent;
    int32_t nPlayerLevel;
    int32_t nChanceToHit;

    if (!nAttackRating)
        return 0;

    pPlayer = CUNIT_GetPlayer();
    nPercent = 100;

    nDifficulty = (uint8_t)GAME_GetDifficulty();
    if (nDifficulty >= 3)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR)szDifficultyLevelAssert, (uint32_t)(UINT_PTR)szCharCpp, 398);
        exit(-1);
    }

    nRefMonsterClass = SELECT_GetToHitRefMonster();
    if (nRefMonsterClass < 0 || nRefMonsterClass >= sgptDataTables->nMonStatsTxtRecordCount || (pMonStatsTxt = &sgptDataTables->pMonStatsTxt[nRefMonsterClass]) == NULL)
    {
        FOG_DisplayAssert((uint32_t)(UINT_PTR)szPtStats, (uint32_t)(UINT_PTR)szCharCpp, 403);
        exit(-1);
    }

    memset(&tMonStats, 0, sizeof(tMonStats));
    nRefMonsterLevel = (int16_t)pMonStatsTxt->nLevel[nDifficulty];
    nGameType = GAME_IsLadderGame();
    D2COMMON_DATATBLS_CalculateMonsterStatsByLevel(nRefMonsterClass, nGameType, nDifficulty, nRefMonsterLevel, 2, &tMonStats);
    nRefDefense = tMonStats.nAC;

    if (pMonStatsTxt->nAlign != 1 && !GAME_IsExpansionGame() && GAME_GetDifficulty())
    {
        nRefDefense = 10 * nRefDefense / 12;
    }

    nEffectiveAR = nAttackRating;
    if (pPlayer && pPlayer->dwUnitType == UNIT_PLAYER)
    {
        nClassId = pPlayer->dwClassId;
        if (nClassId >= 0 && nClassId < sgptDataTables->nCharStatsTxtRecordCount)
        {
            pCharStatsTxt = &sgptDataTables->pCharStatsTxt[nClassId];
            if (pCharStatsTxt)
                nEffectiveAR = nAttackRating + (int32_t)pCharStatsTxt->dwToHitFactor;
        }
    }

    if (nRefDefense < 0)
    {
        nEffectiveAR -= nRefDefense;
        nRefDefense = 0;
    }
    if (nEffectiveAR < 0)
    {
        nRefDefense -= nEffectiveAR;
        nEffectiveAR = 0;
    }
    if (nEffectiveAR || nRefDefense)
        nPercent = 100 * nEffectiveAR / (nRefDefense + nEffectiveAR);

    nPlayerLevel = D2COMMON_STATLIST_UnitGetStatValue(pPlayer, STAT_LEVEL, 0);
    //  BUG: the divisor (mlvl + clvl) is never tested for zero, which would fault. LATENT: the only caller draws for the local player, whose
    //  level is at least 1, and the reference level is never negative.
    nChanceToHit = nPlayerLevel * nPercent * 2 / ((int16_t)pMonStatsTxt->nLevel[nDifficulty] + nPlayerLevel);
    if (nChanceToHit < 5)
        return 5;
    if (nChanceToHit > 95)
        return 95;
    return nChanceToHit;
}


//1.10f: D2Client.0x6FAD1290
//  The WM_LBUTTONDOWN record of gStatsMessageHandlers. Three regions, tested in order:
//    1. the close button, a fixed 32 x 32 at gnBorderXDifference + 128 and gnScreenHeight - 92; it only arms gbStatsButtonDown,
//       the release closes the panel
//    2. the panel, from the border origin to mid-screen; a stat button is armed only when the player has unspent points
//       (STAT_STATPTS), with none the click is still claimed and nothing happens
//    3. everything else, which is the 800x600 fall-through
//  D2ExpRes patches one site in this function, CharSheetClosebuttonClickdown, to re-place the close button for 1024x768 and 1280x768. Replacing
//  the function wholesale disables that patch.
void __stdcall CHAR_StatsOnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    D2UnitStrc *pPlayer;
    int32_t nButtonIndex;
    uint8_t *pButton;
    int32_t nButtonBottom;
    int32_t nButtonLeft;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;

    //  the close button. The first compare is on the low word of x, as in the original; a negative x would pass it as a large unsigned and fail
    //  the second, so the asymmetry cannot let a click through.
    if ((uint16_t)nXpos >= gnBorderXDifference + 128 && nXpos <= gnBorderXDifference + 160 && nYpos >= gnBorderYDifference + gnScreenHeight - 92 && nYpos <= gnBorderYDifference + gnScreenHeight - 60)
    {
        gbStatsButtonDown = 1;
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
        STORM_SMsgBreakHandlerChain(pMsg);
        return;
    }

    //  the panel
    if (nXpos >= gnBorderXDifference && nXpos <= gnScreenWidth / 2 - 1 && nYpos >= gnBorderYDifference && nYpos <= gnBorderYDifference + gnScreenHeight - 49)
    {
        pPlayer = CUNIT_GetPlayer();
        if (D2COMMON_STATLIST_GetUnitBaseStat(pPlayer, STAT_STATPTS, 0))
        {
            nButtonIndex = 0;
            pButton = gCharPanelStatButtons;
            while (1)
            {
                nButtonBottom = gnScreenHeight + gnBorderYDifference + *((uint32_t *)pButton + 1);
                if (nYpos > nButtonBottom - 502 && nYpos < nButtonBottom - 480)
                {
                    nButtonLeft = gnBorderXDifference + *(uint32_t *)pButton;
                    if (nXpos > nButtonLeft && nXpos < nButtonLeft + 40)
                        break;
                }

                pButton += 14;
                ++nButtonIndex;
                //  Signed compare against one past the end of the table, as in the original.
                if ((int32_t)pButton >= (int32_t)(gCharPanelStatButtons + 56))
                    goto claim_only;
            }

            *(uint32_t *)&gCharPanelStatButtons[14 * nButtonIndex + 8] = 1;
            CURSOR_UpdateCursorPosInGame(pMsg);
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
            SOUNDINST_PlaySound(SOUND_CURSOR_POINT_DROP, NULL, 0, 0, 0);
            STORM_SMsgBreakHandlerChain(pMsg);
            return;
        }

    claim_only:
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        return;
    }

    //  The 800x600 fall-through (written out again in CHAR_StatsOnLButtonUp): at 800x600 a click that is outside the panel, outside the belt,
    //  left of centre and above the control bar is claimed anyway, and an item held on the cursor is used; anything else sets bReturn = 0 and
    //  falls through.
    if (D2GFX_GetResolutionMode() != D2GAMERES_800x600 || PANEL_BeltHitTestPanel(pMsg) || pMsg->nXpos > gnScreenWidth / 2 || pMsg->nYpos >= gnScreenHeight - 47)
    {
        pMsg->bReturn = 0;
        return;
    }

    if (CURSOR_GetCursorState() == 6)
        UI_CursorSendUseHeldItem();

    pMsg->bReturn = 1;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FAD1470
//  The WM_LBUTTONUP record of gStatsMessageHandlers, the half that spends the points. The same three regions as the press, with one difference
//  in each:
//    1. the close button closes the panel (the original tail-jumps into UI_SetUIToggle; nothing reads what it leaves in eax,
//       so this is void)
//    2. the panel walk clears the pressed flag of every button it passes, before testing it, so a release anywhere in the
//       panel disarms all four
//    3. the fall-through is identical
//  With Shift held (GetKeyState(VK_SHIFT) < 0) it spends all remaining points, otherwise one. The points are sent at most 32 at a time, because
//  the count travels in the high byte of the word as n - 1.
//  D2ExpRes patches one site here too, CharSheetClosebuttonRelease.
void __stdcall CHAR_StatsOnLButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    D2UnitStrc *pPlayer;
    UINT_PTR nButtonY;   // an integer cursor over gCharPanelStatButtons, as in CHAR_CharPanelDraw
    int32_t nButtonBottom;
    int32_t nButtonX;
    int32_t nStatPoints;
    int32_t nSpendNow;
    int32_t iButton;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;

    CURSOR_OnPanelMouseUp(pMsg);
    gbStatsButtonDown = 0;

    //  the close button, with the same low-word compare of x as in the press handler
    if ((uint16_t)nXpos >= gnBorderXDifference + 128 && nXpos <= gnBorderXDifference + 160 && nYpos >= gnBorderYDifference + gnScreenHeight - 92 && nYpos <= gnBorderYDifference + gnScreenHeight - 60)
    {
        //  a tail jump in the original
        UI_SetUIToggle(UI_STATSCREEN, UI_TURNOFF, 0);
        return;
    }

    if (nXpos >= gnBorderXDifference && nXpos <= gnScreenWidth / 2 - 1 && nYpos >= gnBorderYDifference && nYpos <= gnBorderYDifference + gnScreenHeight - 49)
    {
        pPlayer = CUNIT_GetPlayer();
        if (!D2COMMON_STATLIST_GetUnitBaseStat(pPlayer, STAT_STATPTS, 0))
            goto claim_only;

        iButton = 0;
        nButtonY = (UINT_PTR)&gCharPanelStatButtons[4];
        while (1)
        {
            nButtonBottom = *(uint32_t *)nButtonY + gnBorderYDifference + gnScreenHeight;

            //  every button the walk passes is disarmed, hit or not
            *((uint32_t *)nButtonY + 1) = 0;

            if (nYpos > nButtonBottom - 502 && nYpos < nButtonBottom - 480)
            {
                nButtonX = gnBorderXDifference + *((uint32_t *)nButtonY - 1);
                if (nXpos > nButtonX && nXpos < nButtonX + 40)
                    break;
            }

            nButtonY += 14;
            ++iButton;
            //  Signed compare against the table + 60, four bytes past its end, as in the original.
            if ((int32_t)nButtonY >= (int32_t)((UINT_PTR)gCharPanelStatButtons + 60))
                goto claim_only;
        }

        nStatPoints = D2COMMON_STATLIST_UnitGetStatValue(pPlayer, STAT_STATPTS, 0);
        if (GetKeyState(VK_SHIFT) < 0)
        {
            if (!nStatPoints)
                goto claim_only2;
        }
        else
        {
            nStatPoints = 1;
        }

        do
        {
            nSpendNow = nStatPoints;
            if (nStatPoints >= 32)
                nSpendNow = 32;
            nStatPoints -= nSpendNow;

            //  58 (0x3A) is the command id
            MSG_SendCmdWord(58, *(uint16_t *)&gCharPanelStatButtons[14 * iButton + 12] | (int16_t)(((uint16_t)nSpendNow - 1) << 8));
        }
        while (nStatPoints);

    claim_only2:
        GAME_ResetMouseButtonState();
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        return;

    claim_only:
        GAME_ResetMouseButtonState();
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        return;
    }

    //  the same 800x600 fall-through as the press handler
    if (D2GFX_GetResolutionMode() != D2GAMERES_800x600 || PANEL_BeltHitTestPanel(pMsg) || pMsg->nXpos > gnScreenWidth / 2 || pMsg->nYpos >= gnScreenHeight - 47)
    {
        pMsg->bReturn = 0;
        return;
    }

    if (CURSOR_GetCursorState() == 6)
        UI_CursorSendUseHeldItem();

    pMsg->bReturn = 1;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FAD16C0
//  Clears gbGoldDialogOpenUnread; UI_CloseGoldEntryDialog reaches it through a tail jump. Nothing in the module reads the global; this and
//  CHAR_MarkGoldDialogOpen are both kept because the original has them.
void __cdecl CHAR_MarkGoldDialogClosed()
{
    gbGoldDialogOpenUnread = 0;
}


//1.10f: D2Client.0x6FAD16D0
//  Sets gbGoldDialogOpenUnread. Called only from the mode-0 arm of UI_OpenGoldEntryDialog, which is unreachable.
void __cdecl CHAR_MarkGoldDialogOpen()
{
    gbGoldDialogOpenUnread = 1;
}


//1.10f: D2Client.0x6FAD16E0
//  Measures the fifteen labels of the character screen once, when the player unit is set (UI_InitUiForPlayerUnit calls it):
//      for (i = 0; i < 15; ++i)
//          label[i].nTextWidth = Unicode::strlen(tbl(label[i].wStringId));
//  The "width" is a character count (Unicode::strlen), not a pixel measurement. Only the low word of the string index is loaded (a short
//  index).
void __cdecl CHAR_CharPanelMeasureLabelWidths()
{
    uint32_t i;
    const Unicode *pwszLabel;

    for (i = 0; i < 15; ++i)
    {
        pwszLabel = D2LANG_GetStringFromTblIndex((D2C_StringIndices)gCharPanelLabels[i].wStringId);
        gCharPanelLabels[i].nTextWidth = Unicode::strlen(pwszLabel);
    }
}

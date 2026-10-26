#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "editbox.h"

//  editbox.cpp -- the edit box control of D2Client.dll 1.10: the edit box (construct, destruct, draw, the character, mouse and arrow handlers,
//  the text accessors) and its control proxy, which holds one control and forwards every vtable slot to it.
//  The original source file is C:\projects\D2\head\Diablo2\Source\D2Client\Ui\editbox.cpp (the asserts in this unit name it).
//  Address range 0x6FAD7540..0x6FAD81F0.
//
//  The edit box's vtable slots: 0 VecDelDtor, 1 Draw, 2 (wheel) null, 3 OnMouseDown, 4 OnMouseUp, 5 (mouse move) null, 6 OnChar, 7 (arrow up) null,
//  8 (arrow down) null, 9 OnArrowRight, 10 OnArrowLeft, 11 GetIntValue, 12 GetTextUtf8, 13 EDITBOX_ReturnZeroCallback, 14 SetTextFromAscii,
//  15 SetTextFromInt, 16 SetWidth, 17 EDITBOX_UiCtrlSetFlag24, 18 SetNumericOnly. An edit box answers left and right and ignores up and down.
//
//  D2ClientEditBoxStrc is 90 bytes. Its members used below: nBoxX and nBoxY (where the caller asked for the box), nTextX and nTextY (nBoxX + 1
//  and nBoxY), nWidth (pixels, measured or estimated), nLineCount (1 means single line, which scrolls), bNumericOnly (picks the character
//  filter and the value ceiling), bAllowKeyRepeat and pfnCharFilter (never set by anything), nBlinkTick (incremented once per Draw),
//  bCaretVisible (20 frames on, 10 off) and bDrawBackground (set when the caller asked for no line count).

//  __FILE__ for this unit, at 0x6FB79038.
static const char szEditBoxCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\Ui\\editbox.cpp";
//  The data root the one cel path is built on -- `aDataGlobal` at 0x6FB743EC.
static const char aDataGlobal[] = "DATA\\GLOBAL";
//  Storm's allocation tags, and the ten zeroes the width estimate measures.
static const char szNew[] = "new";
static const char szDelete[] = "delete";
static const char szTenZeroes[] = "0000000000";
static const char szCaret[] = "_";

//  The edit box's members and its proxy's are free functions. The original's are __thiscall, which a free function cannot say: those that take
//  only `this` are written __fastcall (the same ABI for one register argument), and the others carry an explicit nEdxUnused, so that ecx, edx
//  and the stack line up exactly as __thiscall's do. The extra parameter is never read.
//  DIFFERENCE: nEdxUnused is an argument the original does not have; the machine code agrees. The one call site outside this file,
//  EDITBOX_UiCtrlProxyConstructEditbox in ui.cpp, passes 0 for it.

static_assert(sizeof(D2ClientEditBoxStrc) == 90,
              "EDITBOX_UiCtrlProxyConstructEditbox allocates 0x5A bytes for one of "
              "these, so the struct and that constant prove each other");


//1.10f: D2Client.0x6FAD7540
//  Builds an edit box. It allocates two text buffers of 2 * nMaxChars + 2 bytes each, both zeroed: pwszText is the content, pwszTextAlt a scratch
//  copy that Draw cuts at the caret to measure the prefix.
//  When the caller gives no positive width, nWidth = (nMaxChars + 2) * (GetTextWidth("0000000000") / 10): ten zeroes in the box's font, so in a
//  proportional font it is an estimate (EDITBOX_UiEditBoxSetWidth exists to correct it). A positive nWidth is used unchanged.
//  nLineCount <= 0 is forced to 1 and raises bDrawBackground, which is what turns on the filled rectangle in Draw.
//  bNumericOnly starts at 1, so an edit box is numeric until something says otherwise; the only thing that could is vtable slot 18, which
//  no code in the module indexes.
char *__fastcall EDITBOX_UiEditBoxConstruct(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, int32_t nX, int32_t nY, int32_t nWidth, int32_t nLineCount, int32_t nMaxValue, uint16_t nMaxChars, int32_t (__cdecl *pfnCallback)(), int32_t nFont)
{
    uint32_t nBufferBytes;
    Unicode *pwszAltBuffer;
    char szWidthSample[12];
    Unicode wszText[11];
    char szPath[260];
    int32_t nLines;

    pEditBox->pVtbl = &gVtblUiEditBoxImpl;
    sprintf(szPath, "%s\\ui\\menu\\textslid", aDataGlobal);
    pEditBox->hCelFile = ARCHIVE_CellFileLoad(szPath, 0);

    nBufferBytes = 2 * nMaxChars + 2;
    pEditBox->nBoxX = nX;
    pEditBox->nBoxY = nY;
    pEditBox->bMouseDown = 0;
    pEditBox->pfnCallback = pfnCallback;
    pEditBox->nBlinkTick = 0;
    pEditBox->pwszText = (Unicode *)FOG_Alloc(nBufferBytes, szEditBoxCpp, 93, 0);
    pwszAltBuffer = (Unicode *)FOG_Alloc(nBufferBytes, szEditBoxCpp, 94, 0);
    pEditBox->pwszTextAlt = pwszAltBuffer;
    memset(pEditBox->pwszText, 0, nBufferBytes);
    memset(pEditBox->pwszTextAlt, 0, nBufferBytes);

    pEditBox->nCaretPos = 0;
    pEditBox->nTextLength = 0;
    pEditBox->nMaxLen = nMaxChars;
    pEditBox->nMaxValue = nMaxValue;
    pEditBox->nFont = nFont;
    pEditBox->nTextY = nY;
    pEditBox->nTextX = nX + 1;
    pEditBox->bNumericOnly = 1;
    pEditBox->bCaretVisible = 1;

    strcpy(szWidthSample, szTenZeroes);
    //  DIFFERENCE: the original runs Unicode's default constructor over all eleven elements here; it has no observable effect and is left out.
    D2WIN_10127_SetFont((enum Font)pEditBox->nFont);
    Unicode::win2Unicode(wszText, szWidthSample, 11);
    if (nWidth > 0)
    {
        pEditBox->nWidth = nWidth;
    }
    else
    {
        pEditBox->nWidth = (nMaxChars + 2) * (D2WIN_10121_GetTextWidth(wszText) / 10);
    }

    nLines = nLineCount;
    if (nLineCount > 0)
    {
        pEditBox->bDrawBackground = 0;
    }
    else
    {
        nLines = 1;
        pEditBox->bDrawBackground = 1;
    }
    pEditBox->nLineCount = nLines;
    pEditBox->bAllowKeyRepeat = 0;
    pEditBox->pfnCharFilter = 0;
    return (char *)pEditBox;
}

//1.10f: D2Client.0x6FAD7730
//  Vtable slot 0, the scalar-deleting destructor: runs the destructor and, when bit 0 of the flag is set, hands the object back to Storm.
D2ClientEditBoxStrc *__fastcall EDITBOX_UiEditBoxVecDelDtor(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, uint8_t bFreeObject)
{
    EDITBOX_UiEditBoxDestruct(pEditBox);
    if ((bFreeObject & 1) != 0 && pEditBox)
    {
        STORM_SMemFree(pEditBox, szDelete, -1, 0);
    }
    return pEditBox;
}

//1.10f: D2Client.0x6FAD7760
//  The destructor. It reinstalls the class's vtable first, as a compiler-written destructor does (the store is inert: nothing derives from the
//  edit box), then frees and nulls both text buffers, unloads the cel file and calls the base destructor.
void __fastcall EDITBOX_UiEditBoxDestruct(D2ClientEditBoxStrc *pEditBox)
{
    pEditBox->pVtbl = &gVtblUiEditBoxImpl;

    if (pEditBox->pwszText)
    {
        FOG_Free(pEditBox->pwszText, szEditBoxCpp, 142, 0);
        pEditBox->pwszText = NULL;
    }
    if (pEditBox->pwszTextAlt)
    {
        FOG_Free(pEditBox->pwszTextAlt, szEditBoxCpp, 147, 0);
        pEditBox->pwszTextAlt = NULL;
    }
    //  no null check here, unlike the two above
    ARCHIVE_CellFileFree(pEditBox->hCelFile);
    LISTBOX_UiCtrlBaseDestruct((int32_t *)pEditBox);
}

//1.10f: D2Client.0x6FAD77F0
//  Vtable slot 18: bNumericOnly = x; returns what it stored. No code in the module calls it; the only reference is the vtable entry.
int32_t __fastcall EDITBOX_UiEditBoxSetNumericOnly(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, int32_t bNumericOnly)
{
    pEditBox->bNumericOnly = bNumericOnly;
    return bNumericOnly;
}

//1.10f: D2Client.0x6FAD7800
//  Draws the box, its text and its caret, in two layouts chosen by nLineCount.
//  The caret blinks 20 frames on and 10 off: nBlinkTick is incremented per call, a visible caret is hidden when nBlinkTick % 20 is 0 and a hidden
//  one shown when nBlinkTick % 10 is 0. EDITBOX_UiEditBoxOnMouseUp resets the tick, so a click restarts the cycle.
//  Single line: when the whole string is wider than nWidth, the first visible character is advanced one at a time until the remainder fits, and
//  the remainder is drawn, so a full box shows its tail. The caret is placed by copying the text into pwszTextAlt, cutting the copy at the caret
//  and measuring it.
//  BUG: the scroll measures the remainder from the start of the string, not from the caret, so typing in the middle of a long string can leave
//  the caret off the left edge.
//  Multi-line: D2Win_10199 splits the text at nWidth. Each returned line is drawn 20 pixels below the last and then both the line's text and its
//  node are freed, so the splitter's list is consumed inside the loop. The caret goes on the first line whose running character count reaches
//  nCaretPos, and bCaretPlaced stops it being drawn twice. An empty box draws the caret alone at the text origin (the nCharsDrawn == 0 arm shared
//  with the single-line path).
void __fastcall EDITBOX_UiEditBoxDraw(D2ClientEditBoxStrc *pEditBox)
{
    Unicode wszCaret[2];
    Unicode dest[200];
    char szCaretAscii[2];
    int32_t nLineStart;
    int32_t nTextLen;
    int32_t nCaretPos;
    int32_t nTextWidth;
    D2SplittedTextStrc *pSplitText;
    D2SplittedTextStrc *pSplitNext;
    int32_t nCharsBefore;
    int32_t nLineLen;
    int32_t nCharsDrawn;
    int32_t bCaretPlaced;
    int32_t nCaretInLine;
    int32_t nIndex;
    int32_t nX;
    int32_t nY;
    int32_t pLines;

    nIndex = 0;
    ++pEditBox->nBlinkTick;
    if (pEditBox->bDrawBackground)
    {
        GFX_DrawFilledRect(pEditBox->nTextX - 10, pEditBox->nTextY - 16, pEditBox->nWidth, 18, 0, 2);
    }
    D2WIN_10127_SetFont((enum Font)pEditBox->nFont);

    strcpy(szCaretAscii, szCaret);
    Unicode::win2Unicode(wszCaret, szCaretAscii, 2);

    //  20 frames visible, 10 hidden -- one toggle, two tests
    if (pEditBox->bCaretVisible)
    {
        if (!(pEditBox->nBlinkTick % 20u))
        {
            pEditBox->bCaretVisible = 0;
        }
    }
    else if (!(pEditBox->nBlinkTick % 10u))
    {
        pEditBox->bCaretVisible = 1;
    }

    if (pEditBox->nLineCount == 1)
    {
        nLineStart = 0;
        if (D2WIN_10121_GetTextWidth(pEditBox->pwszText) > pEditBox->nWidth)
        {
            nTextLen = Unicode::strlen(pEditBox->pwszText);
            if (nTextLen > 0)
            {
                do
                {
                    if (D2WIN_10121_GetTextWidth(&pEditBox->pwszText[nLineStart]) <= pEditBox->nWidth)
                    {
                        break;
                    }
                    ++nLineStart;
                }
                while (nLineStart < nTextLen);
            }
        }

        D2WIN_10117_DrawText(&pEditBox->pwszText[nLineStart], pEditBox->nTextX, pEditBox->nTextY, 0, 0);

        nCaretPos = pEditBox->nCaretPos;
        if (nCaretPos < pEditBox->nMaxLen && pEditBox->bCaretVisible && nCaretPos <= pEditBox->nTextLength)
        {
            if (Unicode::strlen(&pEditBox->pwszText[nLineStart]))
            {
                //  measure the prefix by cutting a copy at the caret
                Unicode::strcpy(pEditBox->pwszTextAlt, pEditBox->pwszText);
                pEditBox->pwszTextAlt[pEditBox->nCaretPos].ch = 0;
                nY = pEditBox->nTextY;
                nTextWidth = D2WIN_10121_GetTextWidth(pEditBox->pwszTextAlt);
                D2WIN_10117_DrawText(wszCaret, pEditBox->nTextX + nTextWidth, nY, 0, 0);
                return;
            }
            goto CARET_AT_ORIGIN;
        }
        return;
    }

    pSplitText = D2WIN_10199(pEditBox->pwszText, &pLines, pEditBox->nWidth);
    nCharsBefore = 0;
    nCharsDrawn = 0;
    bCaretPlaced = 0;
    if (!pSplitText)
    {
        goto NO_LINES;
    }

    do
    {
        D2WIN_10117_DrawText(pSplitText->wszLine, pEditBox->nTextX, nIndex + pEditBox->nTextY, 0, 0);
        nLineLen = Unicode::strlen(pSplitText->wszLine);
        pSplitNext = pSplitText->pNextLine;
        nCharsBefore += nLineLen;
        ++nCharsDrawn;
        nIndex += 20;

        if (pEditBox->nCaretPos < pEditBox->nMaxLen && pEditBox->bCaretVisible && nCharsBefore >= pEditBox->nCaretPos && !bCaretPlaced)
        {
            bCaretPlaced = 1;
            nCaretInLine = nLineLen + pEditBox->nCaretPos - nCharsBefore;
            if (nCaretInLine < 0)
            {
                nCaretInLine = 0;
            }
            if (Unicode::strlen(pSplitText->wszLine))
            {
                //  BUG: a line longer than 199 characters abandons the loop and leaks the rest of the split list (the local is 200 elements).
                if (Unicode::strlen(pSplitText->wszLine) > 199)
                {
                    return;
                }
                Unicode::strcpy(dest, pSplitText->wszLine);
                dest[nCaretInLine].ch = 0;
                nY = pEditBox->nTextY + nIndex - 20;
                nX = pEditBox->nTextX + D2WIN_10121_GetTextWidth(dest);
            }
            else
            {
                nX = pEditBox->nTextX;
                nY = pEditBox->nTextY + nIndex - 20;
            }
            D2WIN_10117_DrawText(wszCaret, nX, nY, 0, 0);
        }

        FOG_Free(pSplitText->wszLine, szEditBoxCpp, 259, 0);
        FOG_Free(pSplitText, szEditBoxCpp, 260, 0);
        pSplitText = pSplitNext;
    }
    while (pSplitNext);

    if (nCharsDrawn)
    {
        return;
    }

NO_LINES:
    if (pEditBox->bCaretVisible)
    {
CARET_AT_ORIGIN:
        D2WIN_10117_DrawText(wszCaret, pEditBox->nTextX, pEditBox->nTextY, 0, 0);
    }
}

//1.10f: D2Client.0x6FAD7B00
//  Vtable slot 15, sets the text from an integer: sprintf("%d") into a 20-byte local, and if strlen(Buffer) > nMaxLen the buffer is cut at
//  Buffer[nMaxLen - 1]; win2Unicode is then given nMaxLen.
//  BUG: a string longer than nMaxLen is cut to nMaxLen - 1, one shorter than it needs to be. HARMLESS: the text buffer holds nMaxLen characters
//  plus a terminator, and a string of exactly nMaxLen characters is left whole.
//  BUG: it does not clamp against nMaxValue, which EDITBOX_UiEditBoxGetIntValue assumes.
int32_t __fastcall EDITBOX_UiEditBoxSetTextFromInt(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, int32_t nValue)
{
    char szBuffer[20];
    uint32_t nMaxLen;
    int32_t nTextLen;

    sprintf(szBuffer, "%d", nValue);
    nMaxLen = pEditBox->nMaxLen;
    if (strlen(szBuffer) > nMaxLen)
    {
        szBuffer[nMaxLen - 1] = 0;
    }
    Unicode::win2Unicode(pEditBox->pwszText, szBuffer, nMaxLen);
    nTextLen = Unicode::strlen(pEditBox->pwszText);
    pEditBox->nCaretPos = nTextLen;
    pEditBox->nTextLength = nTextLen;
    return nTextLen;
}

//1.10f: D2Client.0x6FAD7B60
//  Vtable slot 14, sets the text from an ASCII string: the same three lines as SetTextFromInt without the truncation. win2Unicode is handed
//  nMaxLen and is trusted to stop there.
int32_t __fastcall EDITBOX_UiEditBoxSetTextFromAscii(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, const char *szText)
{
    int32_t nTextLen;

    Unicode::win2Unicode(pEditBox->pwszText, szText, pEditBox->nMaxLen);
    nTextLen = Unicode::strlen(pEditBox->pwszText);
    pEditBox->nCaretPos = nTextLen;
    pEditBox->nTextLength = nTextLen;
    return nTextLen;
}

//1.10f: D2Client.0x6FAD7B90
//  Vtable slot 11, reads the box as an integer. Empty text answers 0 without converting; otherwise the Unicode is converted to UTF-8 into a
//  100-byte local and passed to atoi, and the result is `nValue > nMaxValue ? 0 : nValue`.
//  BUG: a value above the ceiling answers 0, not the ceiling. OnChar applies the ceiling to typed text, so the value is in range only when typed,
//  not when the text was set through slot 14 or 15.
int32_t __fastcall EDITBOX_UiEditBoxGetIntValue(D2ClientEditBoxStrc *pEditBox)
{
    char szText[100];
    int32_t nValue;

    if (!Unicode::strlen(pEditBox->pwszText))
    {
        return 0;
    }
    Unicode::toUtf(szText, pEditBox->pwszText, pEditBox->nMaxLen);
    nValue = atoi(szText);
    return nValue > pEditBox->nMaxValue ? 0 : nValue;
}

//1.10f: D2Client.0x6FAD7BE0
//  Vtable slot 12, copies the text out as UTF-8; the whole body is one forwarded call. The caller owns the buffer and is told nothing about how
//  much was written beyond nMaxLen.
char *__fastcall EDITBOX_UiEditBoxGetTextUtf8(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, char *szOut)
{
    return Unicode::toUtf(szOut, pEditBox->pwszText, pEditBox->nMaxLen);
}

//1.10f: D2Client.0x6FAD7C00
//  Three bytes, `xor eax, eax; retn`. Slot 13 of the edit box's vtable and slot 11 of the button's: two classes share one stub.
int32_t __cdecl EDITBOX_ReturnZeroCallback(void)
{
    return 0;
}

//1.10f: D2Client.0x6FAD7C10
//  Vtable slot 3, mouse down: raises bMouseDown and answers 0, which the dialog's dispatch reads as not handled, so a click on an edit box is
//  recorded here and still reaches the dialog's own line hit-testing.
int32_t __fastcall EDITBOX_UiEditBoxOnMouseDown(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    pEditBox->bMouseDown = 1;
    return 0;
}

//1.10f: D2Client.0x6FAD7C20
//  Vtable slot 4, mouse up: restarts the caret blink (nBlinkTick = 0 puts the cycle back at the start of its visible phase, so a click always
//  leaves the caret on) and answers 0, like the press. A release without a matching press does nothing.
int32_t __fastcall EDITBOX_UiEditBoxOnMouseUp(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    if (pEditBox->bMouseDown)
    {
        pEditBox->nBlinkTick = 0;
        pEditBox->bMouseDown = 0;
    }
    return 0;
}

//1.10f: D2Client.0x6FAD7C40
//  Vtable slot 6, a character: every one of the object's text fields is maintained here. A handled character consumes the message and answers 1.
//  The filter is chosen three ways: pfnCharFilter set (call it; its answer decides; nothing in the module sets it), bNumericOnly (isdigit(c)),
//  otherwise D2Win_10128(c). In the third arm a Return or Escape character does not reach the editing code at all: it falls out of the chain and
//  fires pfnCallback with no arguments, so Enter and Escape are the box's done keys only when neither a custom filter nor numeric mode is on.
//  A numeric box has no way to signal done, because isdigit rejects 13 and 27.
//  Backspace (8) copies each character from the caret to min(nTextLength, nMaxLen) - 1 one place left, drops the last and moves the caret back.
//  Delete (46) does the same from the caret forward without touching the caret. Both are no-ops at the ends.
//  Insertion at the end of the text is a straight append; in the middle the tail is shifted right one character at a time from the end towards
//  the caret, so the loop counts down. The capacity test before either is nTextLength < nMaxLen - 1.
//  The line limit is enforced by undoing the insert: after every accepted character the text is re-split with D2Win_10199 at the box's width, the
//  split list is freed, and if the line count now exceeds nLineCount the character just inserted is removed again and the caret stepped back.
//  A full box silently refuses.
//  The value ceiling is applied here, not in the accessor: a numeric box with no custom filter converts its text with Unicode::toUtf, reads it
//  with atoi, and if the result exceeds nMaxValue replaces the whole buffer with the ceiling printed back. A non-numeric box takes the other
//  arm and only refreshes nTextLength.
int32_t __fastcall EDITBOX_UiEditBoxOnChar(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    char szText[2048];
    int8_t c;
    int32_t (__fastcall *pfnCharFilter)(D2ClientEditBoxStrc *, SMSGHANDLER_PARAMS *);
    int32_t bAccept;
    int32_t nTextLength;
    int32_t nCaretPos;
    int32_t i;
    int32_t nBound;
    D2SplittedTextStrc *pSplit;
    D2SplittedTextStrc *pSplitNext;
    uint32_t nMaxValue;
    int32_t pLines;

    if (!pEditBox->bAllowKeyRepeat && (pMsg->lParam & 0x40000000) != 0)
    {
        return 1;
    }
    if (pMsg->wParam > 0x7F)
    {
        goto CONSUME;
    }

    c = (int8_t)pMsg->wParam;
    pfnCharFilter = pEditBox->pfnCharFilter;
    if (pfnCharFilter)
    {
        bAccept = pfnCharFilter(pEditBox, pMsg);
    }
    else if (pEditBox->bNumericOnly)
    {
        bAccept = isdigit(c);
    }
    else
    {
        bAccept = D2WIN_10128(c);
        if (c == 13 || c == 27)
        {
            //  the box's "done" keys -- only on this arm
            pEditBox->pfnCallback();
            goto CONSUME;
        }
    }

    if (c == 8)
    {
        nTextLength = pEditBox->nTextLength;
        if (nTextLength > 0)
        {
            nCaretPos = pEditBox->nCaretPos;
            if (nCaretPos != nTextLength && nCaretPos)
            {
                for (i = nCaretPos - 1; ; ++i)
                {
                    nBound = pEditBox->nMaxLen;
                    if (pEditBox->nTextLength < (uint16_t)nBound)
                    {
                        nBound = pEditBox->nTextLength;
                    }
                    if (i >= nBound - 1)
                    {
                        break;
                    }
                    pEditBox->pwszText[i].ch = pEditBox->pwszText[i + 1].ch;
                }
            }
            pEditBox->pwszText[--pEditBox->nTextLength].ch = 0;
        }
        if (pEditBox->nCaretPos)
        {
            --pEditBox->nCaretPos;
        }
    }
    else if (c == 46)
    {
        nTextLength = pEditBox->nTextLength;
        if (nTextLength > 0)
        {
            nCaretPos = pEditBox->nCaretPos;
            if (nCaretPos != nTextLength)
            {
                for (;;)
                {
                    nBound = pEditBox->nMaxLen;
                    if (pEditBox->nTextLength < (uint16_t)nBound)
                    {
                        nBound = pEditBox->nTextLength;
                    }
                    if (nCaretPos >= nBound)
                    {
                        break;
                    }
                    ++nCaretPos;
                    pEditBox->pwszText[nCaretPos - 1].ch = pEditBox->pwszText[nCaretPos].ch;
                }
                pEditBox->pwszText[--pEditBox->nTextLength].ch = 0;
            }
        }
    }
    else if (bAccept)
    {
        nTextLength = pEditBox->nTextLength;
        nBound = pEditBox->nMaxLen;
        //  one stricter than the buffer needs
        if (nTextLength < (uint16_t)nBound - 1)
        {
            nCaretPos = pEditBox->nCaretPos;
            if (nCaretPos == nTextLength)
            {
                pEditBox->pwszText[nCaretPos].ch = c;
                ++pEditBox->nCaretPos;
                pEditBox->pwszText[pEditBox->nCaretPos].ch = 0;
                ++pEditBox->nTextLength;
            }
            else
            {
                //  shift the tail right, counting DOWN from the end
                if (nTextLength < nBound)
                {
                    nBound = pEditBox->nTextLength;
                }
                if (nBound >= nCaretPos)
                {
                    do
                    {
                        pEditBox->pwszText[nBound].ch = pEditBox->pwszText[nBound - 1].ch;
                        --nBound;
                    }
                    while (nBound >= pEditBox->nCaretPos);
                }
                if (pEditBox->nTextLength < pEditBox->nMaxLen)
                {
                    ++pEditBox->nTextLength;
                }
                pEditBox->pwszText[pEditBox->nTextLength].ch = 0;
                pEditBox->pwszText[pEditBox->nCaretPos].ch = c;
            }
        }

        if (pEditBox->nTextLength)
        {
            //  re-wrap to find out whether the box just overflowed
            D2WIN_10127_SetFont((enum Font)pEditBox->nFont);
            pSplit = D2WIN_10199(pEditBox->pwszText, &pLines, pEditBox->nWidth);
            while (pSplit)
            {
                pSplitNext = pSplit->pNextLine;
                FOG_Free(pSplit->wszLine, szEditBoxCpp, 515, 0);
                FOG_Free(pSplit, szEditBoxCpp, 516, 0);
                pSplit = pSplitNext;
            }
            if (pLines > pEditBox->nLineCount)
            {
                //  undo the insert
                --pEditBox->nCaretPos;
                --pEditBox->nTextLength;
                pEditBox->pwszText[pEditBox->nTextLength].ch = 0;
            }
        }

        if (!pEditBox->pfnCharFilter && Unicode::strlen(pEditBox->pwszText))
        {
            if (pEditBox->bNumericOnly)
            {
                Unicode::toUtf(szText, pEditBox->pwszText, pEditBox->nMaxLen);
                nMaxValue = pEditBox->nMaxValue;
                if ((uint32_t)atoi(szText) > nMaxValue)
                {
                    sprintf(szText, "%d", nMaxValue);
                    memset(pEditBox->pwszText, 0, 2 * pEditBox->nMaxLen);
                    Unicode::win2Unicode(pEditBox->pwszText, szText, pEditBox->nMaxLen);
                    pEditBox->nTextLength = Unicode::strlen(pEditBox->pwszText);
                    pEditBox->nCaretPos = pEditBox->nTextLength;
                }
            }
            else
            {
                pEditBox->nTextLength = Unicode::strlen(pEditBox->pwszText);
            }
        }
    }
    else if (c == 32 || c == 27 || c == 13)
    {
        return 0;
    }

CONSUME:
    pMsg->lResult = 0;
    pMsg->bReturn = 1;
    STORM_SMsgBreakHandlerChain(pMsg);
    return 1;
}

//1.10f: D2Client.0x6FAD7F60
//  Five bytes, `xor eax, eax; retn 4`: the do-nothing vtable entry. It fills the edit box's wheel, mouse-move, arrow-up and arrow-down slots and
//  slots of the button's. It takes an argument and purges it, which separates it from EDITBOX_ReturnZeroCallback.
int32_t __stdcall EDITBOX_DialogNullCallback(int32_t nUnused)
{
    return 0;
}

//1.10f: D2Client.0x6FAD7F70
//  Vtable slot 16: nWidth = x; returns what it stored. No code calls it. The field decides the background box, the single-line scroll threshold
//  and the wrap width handed to D2Win_10199; a caller can use it to correct the constructor's estimate.
int32_t __fastcall EDITBOX_UiEditBoxSetWidth(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, int32_t nWidth)
{
    pEditBox->nWidth = nWidth;
    return nWidth;
}

//1.10f: D2Client.0x6FAD7F80
//  +0x24 = x. It is not an edit box method: it fills slot 17 of the edit box's vtable and slot 12 of the button's, so it belongs to a base the two
//  share. The name says the offset because the two readers do not agree on a meaning: in the edit box the field gates auto-repeat
//  (EDITBOX_UiEditBoxOnChar), in the button it means "Enter presses me" (CHAR_UiButtonOnChar).
//  Nothing in the module calls it, so the field keeps what the constructor left, zero, and every edit box in 1.10 ignores auto-repeat.
int32_t __fastcall EDITBOX_UiCtrlSetFlag24(void *pControl, int32_t nEdxUnused, int32_t bFlag)
{
    *((int32_t *)pControl + 9) = bFlag;
    return bFlag;
}

//1.10f: D2Client.0x6FAD7F90
//  Vtable slot 9, right arrow: moves the caret one character towards the end, stopping at nTextLength. Answers 1, so unlike the mouse handlers
//  it consumes the message.
int32_t __fastcall EDITBOX_UiEditBoxOnArrowRight(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    if (pEditBox->nCaretPos < pEditBox->nTextLength)
    {
        ++pEditBox->nCaretPos;
    }
    return 1;
}

//1.10f: D2Client.0x6FAD7FB0
//  Vtable slot 10, left arrow: the mirror, stopping at 0. Up and down are EDITBOX_DialogNullCallback.
int32_t __fastcall EDITBOX_UiEditBoxOnArrowLeft(D2ClientEditBoxStrc *pEditBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    if (pEditBox->nCaretPos > 0)
    {
        --pEditBox->nCaretPos;
    }
    return 1;
}


//  The control proxy: eighteen functions, 398 bytes. A proxy holds one control at +0x08 and forwards to it. A dialog's control list holds proxies,
//  not controls: DIALOG_AddControl is handed what EDITBOX_UiCtrlProxyConstructEditbox returns.
//  The forwarders are tail jumps in the original (mov ecx, [ecx+8]; mov eax, [ecx]; jmp dword ptr [eax+N]), which leave the stack arguments
//  untouched. The C cannot be written as a jump, so each is a returned call through the same slot, and it passes the target's argument on with
//  it: a target that purges 4 bytes, as EDITBOX_DialogNullCallback does, would otherwise leave the caller's stack four bytes short.
//  Two forwarders serve targets that disagree, because the same bytes sit in two proxy tables: FwdSlot14 reaches EDITBOX_UiEditBoxSetTextFromAscii
//  (purge 4) here and LISTBOX_ImplRemoveAll (purge 0) in the list box, and FwdSlot15 reaches EDITBOX_UiEditBoxSetTextFromInt (purge 4) and
//  LISTBOX_ImplAddEntry (purge 40). A jump carries whatever was pushed, so the call site is what must match; the C here writes the edit box's path.
//  Each forwarder is named after the slot it reaches, which is not always its own position: proxy slot 12 reaches impl 13, 13 reaches 14,
//  14 reaches 15 and 15 reaches 12.
//  Slots 1, 2, 3 and the 12Arg forwarder of this proxy vtable are filled by functions in other units (0x6FAF11D0, 0x6FAF12B0, 0x6FB1A780,
//  0x6FB1A790).

//1.10f: D2Client.0x6FAD7FD0
//  Destroys the proxy and the control it holds: the wrapped control is sent its own slot 0, the scalar-deleting destructor, with the flag set,
//  so the proxy frees what it wraps. Then the shared base destructor runs.
void __fastcall EDITBOX_UiCtrlProxyDestruct(int32_t *pProxy)
{
    void (__thiscall ***pControl)(uint32_t, int32_t);

    *pProxy = (int32_t)&gVtblUiControlProxy;
    pControl = (void (__thiscall ***)(uint32_t, int32_t))pProxy[2];
    if (pControl)
    {
        (**pControl)((uint32_t)pControl, 1);
    }
    LISTBOX_UiCtrlBaseDestruct(pProxy);
}

//1.10f: D2Client.0x6FAD8030
//  The proxy's scalar-deleting destructor, slot 0, shaped like the edit box's.
uint32_t **__fastcall EDITBOX_UiCtrlProxyVecDelDtor(uint32_t **pProxy, int32_t nEdxUnused, uint8_t bFreeObject)
{
    EDITBOX_UiCtrlProxyDestruct((int32_t *)pProxy);
    if ((bFreeObject & 1) != 0 && pProxy)
    {
        STORM_SMemFree(pProxy, szDelete, -1, 0);
    }
    return pProxy;
}

//1.10f: D2Client.0x6FAD8060
//  Builds a proxy around a new edit box; the only constructor of either class that a caller outside this file uses.
//  SMemAlloc(0x5A, "new", -1, 0) is the edit box, 0x5A being sizeof(D2ClientEditBoxStrc) (the static_assert at the head of this file ties the
//  two together). The allocation is checked, and a failure stores a null control rather than halting, which every forwarder would then
//  dereference. Its nine parameters are the edit box's own, passed through untouched.
uint32_t *__fastcall EDITBOX_UiCtrlProxyConstructEditbox(uint32_t *pProxy, int32_t nEdxUnused, int32_t nX, int32_t nY, int32_t nWidth, int32_t nLineCount, int32_t nMaxValue, uint16_t nMaxChars, int32_t (__cdecl *pfnCallback)(), int32_t nFont)
{
    D2ClientEditBoxStrc *pEditBox;
    char *pConstructed;

    *pProxy = (uint32_t)&gVtblUiControlProxy;
    pEditBox = (D2ClientEditBoxStrc *)STORM_SMemAlloc(0x5A, szNew, -1, 0);
    if (pEditBox)
    {
        pConstructed = EDITBOX_UiEditBoxConstruct(pEditBox, 0, nX, nY, nWidth, nLineCount, nMaxValue, nMaxChars, pfnCallback, nFont);
    }
    else
    {
        pConstructed = NULL;
    }
    pProxy[2] = (uint32_t)pConstructed;
    return pProxy;
}

//1.10f: D2Client.0x6FAD8100
//  Forwards to the wrapped control's slot 4, the edit box's OnMouseUp.
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot4(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    return (*(int32_t(__thiscall **)(uint32_t, SMSGHANDLER_PARAMS *))(**(pProxy + 2) + 16))((uint32_t)*(pProxy + 2), pMsg);
}

//1.10f: D2Client.0x6FAD8110
//  Forwards to the wrapped control's slot 5, the edit box's (mouse move) null.
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot5(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    return (*(int32_t(__thiscall **)(uint32_t, SMSGHANDLER_PARAMS *))(**(pProxy + 2) + 20))((uint32_t)*(pProxy + 2), pMsg);
}

//1.10f: D2Client.0x6FAD8120
//  Forwards to the wrapped control's slot 6, the edit box's OnChar.
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot6(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    return (*(int32_t(__thiscall **)(uint32_t, SMSGHANDLER_PARAMS *))(**(pProxy + 2) + 24))((uint32_t)*(pProxy + 2), pMsg);
}

//1.10f: D2Client.0x6FAD8130
//  Forwards to the wrapped control's slot 8, the edit box's (arrow down) null.
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot8(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    return (*(int32_t(__thiscall **)(uint32_t, SMSGHANDLER_PARAMS *))(**(pProxy + 2) + 32))((uint32_t)*(pProxy + 2), pMsg);
}

//1.10f: D2Client.0x6FAD8140
//  Forwards to the wrapped control's slot 7, the edit box's (arrow up) null.
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot7(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    return (*(int32_t(__thiscall **)(uint32_t, SMSGHANDLER_PARAMS *))(**(pProxy + 2) + 28))((uint32_t)*(pProxy + 2), pMsg);
}

//1.10f: D2Client.0x6FAD8150
//  Forwards to the wrapped control's slot 9, the edit box's OnArrowRight.
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot9(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    return (*(int32_t(__thiscall **)(uint32_t, SMSGHANDLER_PARAMS *))(**(pProxy + 2) + 36))((uint32_t)*(pProxy + 2), pMsg);
}

//1.10f: D2Client.0x6FAD8160
//  Forwards to the wrapped control's slot 10, the edit box's OnArrowLeft.
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot10(uint32_t **pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    return (*(int32_t(__thiscall **)(uint32_t, SMSGHANDLER_PARAMS *))(**(pProxy + 2) + 40))((uint32_t)*(pProxy + 2), pMsg);
}

//1.10f: D2Client.0x6FAD8170
//  Forwards to the wrapped control's slot 11, the edit box's GetIntValue.
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot11(uint32_t **pProxy)
{
    return (*(int32_t(__thiscall **)(uint32_t))(**(pProxy + 2) + 44))((uint32_t)*(pProxy + 2));
}

//1.10f: D2Client.0x6FAD8180
//  Forwards to the wrapped control's slot 13, the edit box's EDITBOX_ReturnZeroCallback. It sits in proxy slot 12 and reaches impl slot 13.
//  It is not a tail jump: it calls, then `retn 4`, so it takes one stack argument, does not pass it on, and pops it itself, because impl slot 13
//  takes nothing and purges nothing.
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot13Arg(uint32_t **pProxy, int32_t nEdxUnused, int32_t nUnused)
{
    (*(int32_t(__thiscall **)(uint32_t))(**(pProxy + 2) + 52))((uint32_t)*(pProxy + 2));
    return 0;
}

//1.10f: D2Client.0x6FAD8190
//  Forwards to the wrapped control's slot 15, the edit box's SetTextFromInt. It sits in proxy slot 14 and reaches impl slot 15.
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot15(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg)
{
    return (*(int32_t(__thiscall **)(uint32_t, int32_t))(**(pProxy + 2) + 60))((uint32_t)*(pProxy + 2), nArg);
}

//1.10f: D2Client.0x6FAD81A0
//  Forwards to the wrapped control's slot 14, the edit box's SetTextFromAscii. It sits in proxy slot 13 and reaches impl slot 14.
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot14(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg)
{
    return (*(int32_t(__thiscall **)(uint32_t, int32_t))(**(pProxy + 2) + 56))((uint32_t)*(pProxy + 2), nArg);
}

//1.10f: D2Client.0x6FAD81B0
//  Forwards to the wrapped control's slot 12, the edit box's GetTextUtf8. It sits in proxy slot 15 and reaches impl slot 12.
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot12(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg)
{
    return (*(int32_t(__thiscall **)(uint32_t, int32_t))(**(pProxy + 2) + 48))((uint32_t)*(pProxy + 2), nArg);
}

//1.10f: D2Client.0x6FAD81C0
//  Forwards to the wrapped control's slot 16, the edit box's SetWidth.
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot16(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg)
{
    return (*(int32_t(__thiscall **)(uint32_t, int32_t))(**(pProxy + 2) + 64))((uint32_t)*(pProxy + 2), nArg);
}

//1.10f: D2Client.0x6FAD81D0
//  Forwards to the wrapped control's slot 17, the edit box's EDITBOX_UiCtrlSetFlag24.
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot17(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg)
{
    return (*(int32_t(__thiscall **)(uint32_t, int32_t))(**(pProxy + 2) + 68))((uint32_t)*(pProxy + 2), nArg);
}

//1.10f: D2Client.0x6FAD81E0
//  Forwards to the wrapped control's slot 18, the edit box's SetNumericOnly.
int32_t __fastcall EDITBOX_UiCtrlProxyFwdSlot18(uint32_t **pProxy, int32_t nEdxUnused, int32_t nArg)
{
    return (*(int32_t(__thiscall **)(uint32_t, int32_t))(**(pProxy + 2) + 72))((uint32_t)*(pProxy + 2), nArg);
}

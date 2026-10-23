#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "CoreArchive.h"
#include "dialog.h"

//  dialog.cpp -- the modal dialog of D2Client.dll 1.10: a box of text lines and controls with a message table, a cancel key, line selection by
//  mouse, keys and wheel, and two constructors (a plain one and one whose background is a cel). The original's source file is
//  D2Client\UI\dialog.cpp (the path twenty-one of its twenty-six functions push in asserts). Address range 0x6FAD5CA0..0x6FAD7540.
//
//  One dialog at a time: D2ClientDialogStrc has a pNext and gpDialogList is its head, so the shape is a stack, but it never has two entries.
//  Both constructors assert gnDialogNestDepth == 1 on the way out and DIALOG_Close asserts it reaches 0 on the way in, so a second dialog
//  halts the game before it exists; and both constructors erase the link anyway (see the pNext lines in them).
//
//  The dialog's two callbacks are not the same kind of thing:
//      pfnCallback   +0x60   dismiss. Fired by OnCancelKey, OnEscape, OnChar and by OnMouseDown when the click lands outside the box. Called
//                            with nothing pushed and its answer never read.
//      pfnCallback2  +0x64   "a click went outside -- is it mine?" Only OnMouseDown fires it, and only a non-zero answer lets the NEWSTATS /
//                            NEWSKILLS button tests run. When it is null the same path falls back to pfnCallback and asserts it non-null.
//  Neither is passed an argument and only pfnCallback2's answer is looked at, so the DIALOGCALLBACK typedef, int (__stdcall *)(int), does not
//  describe either slot; the casts at the call sites are what the original compiles to.
//
//  Two guards that look like one: dwCreateTick + 80 > GetTickCount() (80 milliseconds since creation) swallows mouse and Escape traffic so the
//  click that opened a dialog cannot at once dismiss it; nFramesDrawn >= 5 (five drawn frames) gates the dismiss callback only, and it is
//  incremented once per DIALOG_Draw, so a dialog that is never drawn can never be cancelled.
//
//  The message table's cancel keys are patched at construction: gDialogMessageHandlers is 14 records and records 0 and 1 carry VK_ESCAPE in the
//  image, but both constructors overwrite them with the player's primary and secondary bindings of HOTKEY_CLEARSCREEN, rejecting -1 and anything
//  at or above 0xE0. Record 11 is a separate, unpatched VK_ESCAPE that reaches DIALOG_OnEscape, so Escape works whatever the binding.
//
//  The line record is 272 bytes, ten of them starting at +0x68 (0x68 + 10 * 272 + 8 is 2832, the allocation size):
//      +0x000  Unicode wszText[120]
//      +0x0F0  int nLineHeight        int nCenterOffset
//      +0x0F8  int nTextWidth         int nUnused1
//      +0x100  int nFont              int nBaseColor
//      +0x108  pfnOnSelect            int bSelectable
//  pfnOnSelect is the per-line callback; unlike the dialog's own two its answer is read, by OnMouseUp and OnEnter, both treating non-zero as
//  handled.

//  __FILE__ for this unit, the string every assert here pushes (0x6FB76B48). The line numbers below are the original's own.
static const char szDialogCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UI\\dialog.cpp";
static const char szPDialogNull[] = "pDialog != NULL";
static const char szZero[] = "0";
static const char szHCel[] = "hCel";
static const char szIndexLtNumLines[] = "index < pDialog->NumLines";
static const char szLineIdxSelectable[] = "pDialog->Line[idx].bSelectable";
static const char szNumLinesLtMax[] = "pDialog->NumLines < MAX_DIALOG_LINES";
static const char szBaseColorVsHighlight[] = "BaseColor != FONTCOLOR_BLUE || pDialog->eHighlight != DIALOG_HIGHLIGHT_COLOR";
static const char szAutoSizeTrue[] = "pDialog->bAutoSize == TRUE";
static const char szMaxWidth[] = "MaxWidth <= DIALOG_MAX_WIDTH";
static const char szMaxHeight[] = "MaxHeight <= DIALOG_MAX_HEIGHT";

//  The data root DIALOG_CreateFromImage builds its one cel path on.
static const char aDataGlobal[] = "DATA\\GLOBAL";

static_assert(sizeof(D2ClientDialogStrc) == 2832,
              "DIALOG_Create allocates 2832 bytes and writes 26 fields into "
              "them; the line stride and the allocation prove each other");
static_assert(sizeof(D2ClientDialogLineStrc) == 272,
              "ten lines at 272 bytes, 0x68 of header and the two trailing "
              "dwords -- 0x68 + 2720 + 8 == 2832");


//1.10f: D2Client.0x6FAD5CA0
//  The cursor moved over the dialog. Nothing happens for the first 80 ms of a dialog's life. After that the control list gets first refusal
//  through vtable slot +20, and a control that answers 1 consumes the message; if there is no control list, or none of them wanted it, the
//  dialog hit-tests its own lines.
//  The two outcomes consume differently: a control that handles the move makes this break the handler chain; the line path does not -- it
//  changes the selection, plays sound 1 and lets the message run on, which is why the `goto` below is kept. Both nSelectedLine and
//  nPressedLine are set to the hit line, so a move re-arms the press guard as well as the highlight.
void __stdcall DIALOG_OnMouseMove(SMSGHANDLER_PARAMS *pMsg)
{
    D2ClientDialogStrc *pDialog;
    uint32_t *pControl;
    int32_t nWasSelected;
    int32_t nHitLine;

    pDialog = gpDialogList;
    if (pDialog->dwCreateTick + 80 > GetTickCount())
    {
        return;
    }

    pControl = (uint32_t *)pDialog->pControlList;
    if (pControl)
    {
        while (!*pControl || (*(int32_t(__thiscall **)(uint32_t, SMSGHANDLER_PARAMS *)) (*(uint32_t *)*pControl + 20))(*pControl, pMsg) != 1)
        {
            pControl = (uint32_t *)pControl[1];
            if (!pControl)
            {
                goto HIT_TEST_LINES;
            }
        }
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        return;
    }

HIT_TEST_LINES:
    nWasSelected = pDialog->nSelectedLine;
    nHitLine = DIALOG_HitTestLine(pDialog, pMsg);
    pDialog->nSelectedLine = nHitLine;
    pDialog->nPressedLine = nHitLine;
    if (nHitLine != nWasSelected && nHitLine != -1)
    {
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
    }
}

//1.10f: D2Client.0x6FAD5D30
//  Returns which line the point is on, or -1. Walks the lines accumulating nLineHeight; a line is a candidate only if bSelectable is set, so a
//  caption can never be hit. The box, exclusive at both ends, is
//      x   nLeft + 15  ..  nLeft + nWidth - 15
//      y   nLineY - 11 ..  nLineY + 4              where nLineY is the accumulated top plus this line's own height
//  The Y band is the height of one row of font 1 and is not derived from nLineHeight, so a line given an unusually large height leaves dead
//  space below it. A null dialog answers -1 rather than asserting, unlike every other function in this file.
int32_t __fastcall DIALOG_HitTestLine(D2ClientDialogStrc *pDialog, SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nAccumHeight;
    int32_t nXpos;
    int32_t nYpos;
    int32_t iLine;
    int32_t nLeft;
    int32_t nLineY;

    nAccumHeight = 0;
    if (!pDialog)
    {
        return -1;
    }

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    if (pDialog->NumLines <= 0)
    {
        return -1;
    }

    for (iLine = 0; iLine < pDialog->NumLines; ++iLine)
    {
        if (pDialog->Lines[iLine].bSelectable == 1)
        {
            nLeft = pDialog->nLeft;
            if (nXpos > nLeft + 15 && nXpos < pDialog->nWidth + nLeft - 15)
            {
                nLineY = nAccumHeight + pDialog->nTop + pDialog->Lines[iLine].nLineHeight;
                if (nYpos > nLineY - 11 && nYpos < nLineY + 4)
                {
                    return iLine;
                }
            }
        }
        nAccumHeight += pDialog->Lines[iLine].nLineHeight;
    }

    return -1;
}

//1.10f: D2Client.0x6FAD5DC0
//  A mouse button came up; the only place a line's own callback is fired by the mouse.
//  The minipanel gets a veto first: if it is open and the point is over one of its rows, this does not consume the message and only finishes
//  the press bookkeeping. Everything else consumes before doing anything, so the assert at :291 is reached with the chain already broken.
//  bLinePressActive says the press went to the lines rather than to a control. For the callback to fire the dialog must be more than 80 ms old,
//  the release must hit-test to the same line the press recorded, that index must be inside 0..NumLines-1 and the line must have a pfnOnSelect.
//  The index checked (nPressedLine, against the fresh hit test) and the index used (Lines[nSelectedLine]) are different fields; they are equal
//  on every path that gets here, since DIALOG_OnMouseMove and DIALOG_OnMouseDown write the two together.
//  A callback answering 0 clears bLinePressActive; a non-zero answer leaves it set, so the dialog stays "pressed" until something else clears it.
void __stdcall DIALOG_OnMouseUp(SMSGHANDLER_PARAMS *pMsg)
{
    D2ClientDialogStrc *pDialog;
    uint32_t *pControl;
    int32_t bPressWasOnLines;
    int32_t nHitLine;
    int32_t (*pfnOnSelect)(void);

    pDialog = gpDialogList;

    if (UI_GetUiWindowState(UI_MINIPANEL) && MINIPANEL_IsPointOverMiniPanelRow(pMsg->nXpos, pMsg->nYpos))
    {
        if (!pDialog)
        {
            return;
        }
        bPressWasOnLines = (pDialog->bLinePressActive == 1);
        goto FINISH_PRESS;
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    if (!pDialog)
    {
        FOG_DisplayAssert(szPDialogNull, szDialogCpp, 291);
        exit(-1);
    }

    if (pDialog->bLinePressActive == 1)
    {
        if (pDialog->dwCreateTick + 80 > GetTickCount())
        {
            return;
        }
        nHitLine = DIALOG_HitTestLine(pDialog, pMsg);
        //  tested on nPressedLine, fetched from nSelectedLine
        if (pDialog->nPressedLine != nHitLine || nHitLine < 0 || nHitLine >= pDialog->NumLines || (pfnOnSelect = (int32_t (*)(void)) pDialog->Lines[pDialog->nSelectedLine].pfnOnSelect) == NULL)
        {
            pDialog->bLinePressActive = 0;
            return;
        }
        bPressWasOnLines = (pfnOnSelect() == 0);
FINISH_PRESS:
        if (!bPressWasOnLines)
        {
            return;
        }
        pDialog->bLinePressActive = 0;
        return;
    }

    //  the press did not go to the lines, so the controls get the release
    for (pControl = (uint32_t *)pDialog->pControlList; pControl; pControl = (uint32_t *)pControl[1])
    {
        if (*pControl && (*(int32_t(__thiscall **)(uint32_t, SMSGHANDLER_PARAMS *)) (*(uint32_t *)*pControl + 16))(*pControl, pMsg) == 1)
        {
            break;
        }
    }
}

//1.10f: D2Client.0x6FAD5EC0
//  A mouse button went down; the longest of the handlers, because a click outside the box is what dismisses a dialog and that path has to ask
//  three other panels whether the click was really theirs. nXpos and nYpos are the 16-bit fields of lParam, loaded once, zero-extended, and
//  reused for every test.
//  Inside the box: controls first, vtable slot +12; one that answers 1 consumes and returns. Otherwise bLinePressActive is raised, the lines
//  are hit-tested, and both nSelectedLine and nPressedLine take the answer. A hit on a new line plays sound 1 and consumes.
//  Outside the box (grown by 4 pixels on every side before the test, so a click in the frame counts as inside) and not over a minipanel row,
//  the dialog is being dismissed:
//      pfnCallback2 set    ask it. A zero answer means "not mine" and the message is dropped. A non-zero answer means the click may belong
//                          to the NEWSTATS or NEWSKILLS button, and each is asked in turn
//      pfnCallback2 null   fire pfnCallback (the dismiss) and consume. It is asserted non-null at :426
//  The two panel tests do not behave the same way: NEWSTATS consumes and falls through to ask NEWSKILLS as well; NEWSKILLS consumes and
//  returns. So a point over both buttons breaks the chain twice, as in the original.
void __stdcall DIALOG_OnMouseDown(SMSGHANDLER_PARAMS *pMsg)
{
    D2ClientDialogStrc *pDialog;
    uint32_t *pControl;
    int32_t nWasSelected;
    int32_t nHitLine;
    int32_t nLeft;
    int32_t nTop;
    int32_t nXpos;
    int32_t nYpos;
    int32_t (*pfnCallback2)(void);
    void (*pfnCallback)(void);

    pDialog = gpDialogList;
    if (!pDialog)
    {
        FOG_DisplayAssert(szPDialogNull, szDialogCpp, 347);
        exit(-1);
    }

    if (pDialog->dwCreateTick + 80 > GetTickCount())
    {
        goto CONSUME;
    }

    pControl = (uint32_t *)pDialog->pControlList;
    if (pControl)
    {
        while (!*pControl || (*(int32_t(__thiscall **)(uint32_t, SMSGHANDLER_PARAMS *)) (*(uint32_t *)*pControl + 12))(*pControl, pMsg) != 1)
        {
            pControl = (uint32_t *)pControl[1];
            if (!pControl)
            {
                goto HIT_TEST_LINES;
            }
        }
        goto CONSUME;
    }

HIT_TEST_LINES:
    nWasSelected = pDialog->nSelectedLine;
    pDialog->bLinePressActive = 1;
    nHitLine = DIALOG_HitTestLine(pDialog, pMsg);
    pDialog->nSelectedLine = nHitLine;
    pDialog->nPressedLine = nHitLine;
    if (nHitLine != nWasSelected && nHitLine != -1)
    {
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
        goto CONSUME;
    }

    //  the box, grown by four on every side
    nLeft = pDialog->nLeft;
    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    if (nXpos >= nLeft - 4 && nXpos <= pDialog->nWidth + nLeft + 4)
    {
        nTop = pDialog->nTop;
        if (nYpos >= nTop - 4 && nYpos <= pDialog->nHeight + nTop + 4)
        {
            goto CONSUME;
        }
    }

    if (UI_GetUiWindowState(UI_MINIPANEL) && MINIPANEL_IsPointOverMiniPanelRow(nXpos, nYpos))
    {
        return;
    }

    pfnCallback2 = (int32_t (*)(void))pDialog->pfnCallback2;
    if (pfnCallback2)
    {
        if (!pfnCallback2())
        {
            return;
        }
        if (UI_GetUiWindowState(UI_NEWSTATS) && NEWSTATS_IsPointOverButton(SCROLL_GetUiShiftScreen(), nXpos, nYpos))
        {
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
            STORM_SMsgBreakHandlerChain(pMsg);
            //  and NEWSKILLS is still asked
        }
        if (UI_GetUiWindowState(UI_NEWSKILLS) && NEWSTATS_NewSkillsIsPointOverButton(SCROLL_GetUiShiftScreen(), nXpos, nYpos))
        {
            goto CONSUME;
        }
        return;
    }

    pfnCallback = (void (*)(void))pDialog->pfnCallback;
    if (!pfnCallback)
    {
        FOG_DisplayAssert(szZero, szDialogCpp, 426);
        exit(-1);
    }
    pfnCallback();

CONSUME:
    pMsg->lResult = 0;
    pMsg->bReturn = 1;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAD60C0
//  The bound cancel key: records 0 and 1 of the table, whose key codes are whatever the player has bound to HOTKEY_CLEARSCREEN (a primary and
//  a secondary key). The outer condition is `pDialog->bGrabsKeyboard || !GetUiWindowState(UI_CHATBOX)`: a dialog that grabs the keyboard
//  always acts, and one that does not is silent while the chat box is open -- it does not even consume.
//  The flag is captured as `bIgnoreCancelKey == 0` before the message is consumed, and only that copy gates the callback: a non-zero
//  bIgnoreCancelKey suppresses the dismiss, and the key is swallowed either way. The dismiss also needs nFramesDrawn >= 5.
void __stdcall DIALOG_OnCancelKey(SMSGHANDLER_PARAMS *pMsg)
{
    D2ClientDialogStrc *pDialog;
    BOOL bCancelAllowed;
    void (*pfnCallback)(void);

    pDialog = gpDialogList;
    if (!pDialog->bGrabsKeyboard && UI_GetUiWindowState(UI_CHATBOX))
    {
        return;
    }

    bCancelAllowed = (pDialog->bIgnoreCancelKey == 0);
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    if (!bCancelAllowed)
    {
        return;
    }

    if (!gpDialogList)
    {
        FOG_DisplayAssert(szPDialogNull, szDialogCpp, 450);
        exit(-1);
    }
    if (gpDialogList->nFramesDrawn >= 5)
    {
        pfnCallback = (void (*)(void))gpDialogList->pfnCallback;
        if (pfnCallback)
        {
            pfnCallback();
            UI_ArmPanelToggleDelay();
        }
    }
}

//1.10f: D2Client.0x6FAD6150
//  The wheel moves the selection. It is registered separately (gDialogWheelHandlers, one record) and only when GAME_HasNativeMouseWheel says
//  the build has wheel support. The notch count is (short)HIWORD(wParam) / 120, signed; zero notches consume without doing anything.
//  Wheel up steps the selection backward and wheel down forward, which agrees with the up and down arrow keys and disagrees with left and
//  right (see the arrow handlers).
//  Controls get slot +8 first. Unlike every other handler here, a control that consumes the wheel makes this return without breaking the
//  chain, because the break lives on the fall-through path only.
void __stdcall DIALOG_OnMouseWheel(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nWheelNotches;
    D2ClientDialogStrc *pDialog;
    uint32_t *pControl;
    int32_t nSelected;
    int32_t nNumLines;

    if (!HIWORD(pMsg->nKey))
    {
        goto CONSUME;
    }
    nWheelNotches = (int16_t)HIWORD(pMsg->nKey) / 120;
    if (!nWheelNotches)
    {
        goto CONSUME;
    }

    pDialog = gpDialogList;
    if (!pDialog)
    {
        FOG_DisplayAssert(szPDialogNull, szDialogCpp, 505);
        exit(-1);
    }

    pControl = (uint32_t *)pDialog->pControlList;
    if (pControl)
    {
        while (!*pControl || (*(int32_t(__thiscall **)(uint32_t, SMSGHANDLER_PARAMS *)) (*(uint32_t *)*pControl + 8))(*pControl, pMsg) != 1)
        {
            pControl = (uint32_t *)pControl[1];
            if (!pControl)
            {
                goto STEP_SELECTION;
            }
        }
        return;
    }

STEP_SELECTION:
    if (nWheelNotches <= 0)
    {
        //  wheel DOWN -- forward, like the down arrow
        if (pDialog->nSelectableCount > 1)
        {
            nSelected = pDialog->nSelectedLine;
            nNumLines = pDialog->NumLines;
            do
            {
                if (++nSelected < nNumLines)
                {
                    if (nSelected < 0)
                    {
                        nSelected = nNumLines - 1;
                    }
                }
                else
                {
                    nSelected = 0;
                }
            }
            while (!pDialog->Lines[nSelected].bSelectable);
            SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
            pDialog->nSelectedLine = nSelected;
        }
    }
    else if (pDialog->nSelectableCount > 1)
    {
        //  wheel UP -- backward, like the up arrow
        nSelected = pDialog->nSelectedLine;
        nNumLines = pDialog->NumLines;
        do
        {
            if (--nSelected < nNumLines)
            {
                if (nSelected < 0)
                {
                    nSelected = nNumLines - 1;
                }
            }
            else
            {
                nSelected = 0;
            }
        }
        while (!pDialog->Lines[nSelected].bSelectable);
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
        pDialog->nSelectedLine = nSelected;
    }

CONSUME:
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FAD6270
//  Escape: record 11, and the one cancel path the key bindings cannot move. It is DIALOG_OnCancelKey with the conditions dropped and the timer
//  test inverted: no bIgnoreCancelKey, no chat box, no bGrabsKeyboard. Escape always consumes and, after the 80 ms guard, always fires the
//  dismiss; so a dialog built with bIgnoreCancelKey set refuses its bound key and still answers Escape. The two arms consume identically and
//  differ only in what follows, which is the shape the original compiles from.
void __stdcall DIALOG_OnEscape(SMSGHANDLER_PARAMS *pMsg)
{
    D2ClientDialogStrc *pDialog;
    void (*pfnCallback)(void);

    pDialog = gpDialogList;
    if (pDialog && pDialog->dwCreateTick + 80 > GetTickCount())
    {
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        return;
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    if (!gpDialogList)
    {
        FOG_DisplayAssert(szPDialogNull, szDialogCpp, 450);
        exit(-1);
    }
    if (gpDialogList->nFramesDrawn >= 5)
    {
        pfnCallback = (void (*)(void))gpDialogList->pfnCallback;
        if (pfnCallback)
        {
            pfnCallback();
            UI_ArmPanelToggleDelay();
        }
    }
}

//1.10f: D2Client.0x6FAD6300
//  The four arrow handlers (OnArrowRight, OnArrowLeft, OnArrowUp, OnArrowDown, in address order) are one body instantiated four times, 178
//  bytes each; they differ in the assert line, the control vtable slot and the step:
//                  assert line   control vtable slot   step
//      OnArrowUp       626            +0x1C            dec
//      OnArrowDown     656            +0x20            inc
//      OnArrowRight    565            +0x24            dec
//      OnArrowLeft     597            +0x28            inc
//  BUG: left steps the same way as down and right the same way as up, while the wheel handler agrees with the vertical pair; the horizontal
//  pair is the one out of step.
//  One arm is dead in half the copies:
//      if ( --n < NumLines ) { if ( n < 0 ) n = NumLines - 1; }
//      else                  n = 0;
//  In the decrementing copies n can only fall, so `n < NumLines` holds on every pass, the else is unreachable and the wrap is the inner
//  `n < 0`. In the incrementing copies the else is the wrap and the inner test is the dead one. Each is written out in full four times, as the
//  image has it.
//  The loop is a do/while over bSelectable and would spin forever if no line were selectable, which is why every copy is guarded by
//  nSelectableCount > 1 (maintained by DIALOG_AddLine).
void __stdcall DIALOG_OnArrowRight(SMSGHANDLER_PARAMS *pMsg)
{
    D2ClientDialogStrc *pDialog;
    uint32_t *pControl;
    int32_t nSelected;
    int32_t nNumLines;

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    pDialog = gpDialogList;
    if (!pDialog)
    {
        FOG_DisplayAssert(szPDialogNull, szDialogCpp, 565);
        exit(-1);
    }

    pControl = (uint32_t *)pDialog->pControlList;
    if (pControl)
    {
        while (!*pControl || (*(int32_t(__thiscall **)(uint32_t, SMSGHANDLER_PARAMS *)) (*(uint32_t *)*pControl + 36))(*pControl, pMsg) != 1)
        {
            pControl = (uint32_t *)pControl[1];
            if (!pControl)
            {
                goto STEP_SELECTION;
            }
        }
        return;
    }

STEP_SELECTION:
    if (pDialog->nSelectableCount > 1)
    {
        nSelected = pDialog->nSelectedLine;
        nNumLines = pDialog->NumLines;
        do
        {
            if (--nSelected < nNumLines)
            {
                if (nSelected < 0)
                {
                    nSelected = nNumLines - 1;
                }
            }
            else
            {
                nSelected = 0;
            }
        }
        while (!pDialog->Lines[nSelected].bSelectable);
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
        pDialog->nSelectedLine = nSelected;
    }
}

//1.10f: D2Client.0x6FAD63C0
void __stdcall DIALOG_OnArrowLeft(SMSGHANDLER_PARAMS *pMsg)
{
    D2ClientDialogStrc *pDialog;
    uint32_t *pControl;
    int32_t nSelected;
    int32_t nNumLines;

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    pDialog = gpDialogList;
    if (!pDialog)
    {
        FOG_DisplayAssert(szPDialogNull, szDialogCpp, 597);
        exit(-1);
    }

    pControl = (uint32_t *)pDialog->pControlList;
    if (pControl)
    {
        while (!*pControl || (*(int32_t(__thiscall **)(uint32_t, SMSGHANDLER_PARAMS *)) (*(uint32_t *)*pControl + 40))(*pControl, pMsg) != 1)
        {
            pControl = (uint32_t *)pControl[1];
            if (!pControl)
            {
                goto STEP_SELECTION;
            }
        }
        return;
    }

STEP_SELECTION:
    if (pDialog->nSelectableCount > 1)
    {
        nSelected = pDialog->nSelectedLine;
        nNumLines = pDialog->NumLines;
        do
        {
            if (++nSelected < nNumLines)
            {
                if (nSelected < 0)
                {
                    nSelected = nNumLines - 1;
                }
            }
            else
            {
                nSelected = 0;
            }
        }
        while (!pDialog->Lines[nSelected].bSelectable);
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
        pDialog->nSelectedLine = nSelected;
    }
}

//1.10f: D2Client.0x6FAD6480
void __stdcall DIALOG_OnArrowUp(SMSGHANDLER_PARAMS *pMsg)
{
    D2ClientDialogStrc *pDialog;
    uint32_t *pControl;
    int32_t nSelected;
    int32_t nNumLines;

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    pDialog = gpDialogList;
    if (!pDialog)
    {
        FOG_DisplayAssert(szPDialogNull, szDialogCpp, 626);
        exit(-1);
    }

    pControl = (uint32_t *)pDialog->pControlList;
    if (pControl)
    {
        while (!*pControl || (*(int32_t(__thiscall **)(uint32_t, SMSGHANDLER_PARAMS *)) (*(uint32_t *)*pControl + 28))(*pControl, pMsg) != 1)
        {
            pControl = (uint32_t *)pControl[1];
            if (!pControl)
            {
                goto STEP_SELECTION;
            }
        }
        return;
    }

STEP_SELECTION:
    if (pDialog->nSelectableCount > 1)
    {
        nSelected = pDialog->nSelectedLine;
        nNumLines = pDialog->NumLines;
        do
        {
            if (--nSelected < nNumLines)
            {
                if (nSelected < 0)
                {
                    nSelected = nNumLines - 1;
                }
            }
            else
            {
                nSelected = 0;
            }
        }
        while (!pDialog->Lines[nSelected].bSelectable);
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
        pDialog->nSelectedLine = nSelected;
    }
}

//1.10f: D2Client.0x6FAD6540
void __stdcall DIALOG_OnArrowDown(SMSGHANDLER_PARAMS *pMsg)
{
    D2ClientDialogStrc *pDialog;
    uint32_t *pControl;
    int32_t nSelected;
    int32_t nNumLines;

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    pDialog = gpDialogList;
    if (!pDialog)
    {
        FOG_DisplayAssert(szPDialogNull, szDialogCpp, 656);
        exit(-1);
    }

    pControl = (uint32_t *)pDialog->pControlList;
    if (pControl)
    {
        while (!*pControl || (*(int32_t(__thiscall **)(uint32_t, SMSGHANDLER_PARAMS *)) (*(uint32_t *)*pControl + 32))(*pControl, pMsg) != 1)
        {
            pControl = (uint32_t *)pControl[1];
            if (!pControl)
            {
                goto STEP_SELECTION;
            }
        }
        return;
    }

STEP_SELECTION:
    if (pDialog->nSelectableCount > 1)
    {
        nSelected = pDialog->nSelectedLine;
        nNumLines = pDialog->NumLines;
        do
        {
            if (++nSelected < nNumLines)
            {
                if (nSelected < 0)
                {
                    nSelected = nNumLines - 1;
                }
            }
            else
            {
                nSelected = 0;
            }
        }
        while (!pDialog->Lines[nSelected].bSelectable);
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
        pDialog->nSelectedLine = nSelected;
    }
}

//1.10f: D2Client.0x6FAD6600
//  Enter fires the selected line, and gets out of the chat box's way. The message is consumed unconditionally, before anything else. Then two
//  gates: nFramesDrawn >= 5, and either the dialog grabs the keyboard or neither binding of HOTKEY_CHAT is the Return key (each check is
//  guarded by its own CMDTBL_ControlsIsActionBound, so an unbound slot does not count). So a player who has bound chat to Enter can still
//  open the chat box over a dialog that does not grab the keyboard.
//  If nSelectedLine is out of range the handler fires nothing: it walks for the first selectable line and selects it, so the next Enter has
//  something to press; a dialog with no selectable line at all returns without writing the field.
//  When there is a selection, pfnOnSelect is fired and its answer is read: exactly 1 arms the panel toggle delay.
void __stdcall DIALOG_OnEnter(SMSGHANDLER_PARAMS *pMsg)
{
    D2ClientDialogStrc *pDialog;
    int32_t nSelected;
    int32_t (*pfnOnSelect)(void);
    int32_t nNumLines;
    int32_t iLine;

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    pDialog = gpDialogList;
    if (!pDialog)
    {
        FOG_DisplayAssert(szPDialogNull, szDialogCpp, 712);
        exit(-1);
    }

    if (gpDialogList->nFramesDrawn < 5)
    {
        return;
    }
    if (!gpDialogList->bGrabsKeyboard && ((CMDTBL_ControlsIsActionBound(HOTKEY_CHAT, 1) && CMDTBL_ControlsFindBoundKey(HOTKEY_CHAT, 1) == 13) || (CMDTBL_ControlsIsActionBound(HOTKEY_CHAT, 0) && CMDTBL_ControlsFindBoundKey(HOTKEY_CHAT, 0) == 13)))
    {
        return;
    }

    nSelected = pDialog->nSelectedLine;
    if (nSelected < 0 || nSelected >= pDialog->NumLines)
    {
        //  nothing selected -- select the first selectable line instead
        nNumLines = pDialog->NumLines;
        if (nNumLines > 0)
        {
            for (iLine = 0; !pDialog->Lines[iLine].bSelectable; ++iLine)
            {
                if (iLine + 1 >= nNumLines)
                {
                    return;
                }
            }
            pDialog->nSelectedLine = iLine;
        }
        return;
    }

    pfnOnSelect = (int32_t (*)(void))pDialog->Lines[nSelected].pfnOnSelect;
    if (pfnOnSelect)
    {
        if (pfnOnSelect() == 1)
        {
            UI_ArmPanelToggleDelay();
        }
    }
}

//1.10f: D2Client.0x6FAD6700
//  WM_CHAR: the router, and the only handler here that calls another one.
//  When the dialog does not grab the keyboard and wParam matches either binding of HOTKEY_CHAT, it returns at once without consuming (the bound
//  key is truncated to 16 bits, the width CMDTBL_ControlsFindBoundKey returns). Otherwise controls get first refusal, vtable slot +24, and a
//  control that answers anything non-zero makes this return without consuming. Then three cases on wParam:
//      Return (13)   hand the whole message to DIALOG_OnEnter and return. The original calls it rather than duplicating it
//      Escape (27)   the dismiss path, identical to DIALOG_OnEscape's
//      anything else consume it only when nInputMode is exactly 1; otherwise return and let the character through
//  nInputMode is a value, not a flag: the test is `!= 1`, so a dialog created with any other number passes every ordinary keystroke on, which
//  is what lets an edit-box control inside a dialog receive typing.
//  gpDialogList is re-read three times on the way through and the local taken at the top is used for the flag tests; the original keeps both.
void __stdcall DIALOG_OnChar(SMSGHANDLER_PARAMS *pMsg)
{
    D2ClientDialogStrc *pDialogAtEntry;
    D2ClientDialogStrc *pDialogFresh;
    uint32_t *pControl;
    WPARAM wParam;
    void (*pfnCallback)(void);

    pDialogFresh = gpDialogList;
    pDialogAtEntry = gpDialogList;
    if (!pDialogAtEntry)
    {
        FOG_DisplayAssert(szPDialogNull, szDialogCpp, 750);
        exit(-1);
    }

    for (pControl = (uint32_t *)pDialogAtEntry->pControlList; pControl; pControl = (uint32_t *)pControl[1])
    {
        if (*pControl)
        {
            if ((*(int32_t(__thiscall **)(uint32_t, SMSGHANDLER_PARAMS *)) (*(uint32_t *)*pControl + 24))(*pControl, pMsg))
            {
                return;
            }
            pDialogFresh = gpDialogList;
        }
    }

    wParam = pMsg->wParam;
    if (!pDialogAtEntry->bGrabsKeyboard)
    {
        if ((CMDTBL_ControlsIsActionBound(HOTKEY_CHAT, 1) && (uint16_t)CMDTBL_ControlsFindBoundKey(HOTKEY_CHAT, 1) == wParam) || (CMDTBL_ControlsIsActionBound(HOTKEY_CHAT, 0) && (uint16_t)CMDTBL_ControlsFindBoundKey(HOTKEY_CHAT, 0) == wParam))
        {
            return;
        }
        pDialogFresh = gpDialogList;
    }

    if (pMsg->wParam == 13)
    {
        DIALOG_OnEnter(pMsg);
        return;
    }

    if (pMsg->wParam != 27)
    {
        if (pDialogAtEntry->nInputMode != 1)
        {
            return;
        }
        goto CONSUME;
    }

    if (pDialogFresh && pDialogFresh->dwCreateTick + 80 > GetTickCount())
    {
CONSUME:
        pMsg->lResult = 0;
        pMsg->bReturn = 1;
        STORM_SMsgBreakHandlerChain(pMsg);
        return;
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    if (!gpDialogList)
    {
        FOG_DisplayAssert(szPDialogNull, szDialogCpp, 450);
        exit(-1);
    }
    if (gpDialogList->nFramesDrawn >= 5)
    {
        pfnCallback = (void (*)(void))gpDialogList->pfnCallback;
        if (pfnCallback)
        {
            pfnCallback();
            UI_ArmPanelToggleDelay();
        }
    }
}

//1.10f: D2Client.0x6FAD6860
//  Eleven bytes, one store: the handlers-registered flag. Both constructors set the flag and DIALOG_Close clears it; this is the only write
//  that runs before any dialog has existed, which makes the first DIALOG_Shutdown safe -- without it the flag is whatever the loader left and
//  the shutdown would unregister handlers that were never registered.
void __cdecl DIALOG_Init(void)
{
    gbDialogHandlersRegistered = 0;
}

//1.10f: D2Client.0x6FAD6870
//  The emergency teardown, not the ordinary close: DIALOG_Close is what a dialog's own dismissal runs, this is what the UI subsystem runs on
//  the way down. It touches gnDialogNestDepth not at all, frees the whole chain rather than one record, and leaks the control lists and any
//  owned image. The chain walk can only ever free one, because both constructors erase pNext with their second memset: the `while (pNext)` is
//  the intent and one free is the effect.
//  The handler unregistration is guarded by gbDialogHandlersRegistered, and the wheel table is only unregistered when the build has native
//  wheel support, the same test the constructor used (GAME_HasNativeMouseWheel reads a value fixed at startup).
void __cdecl DIALOG_Shutdown(void)
{
    D2ClientDialogStrc *pDialog;
    D2ClientDialogStrc *pNext;

    if (gbDialogHandlersRegistered)
    {
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gDialogMessageHandlers, 14);
        if (GAME_HasNativeMouseWheel())
        {
            WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gDialogWheelHandlers, 1);
        }
        --gnDialogWheelHandlersRefCount;
    }

    pDialog = gpDialogList;
    if (pDialog)
    {
        do
        {
            pNext = pDialog->pNext;
            FOG_Free(pDialog, szDialogCpp, 822, 0);
            pDialog = pNext;
        }
        while (pNext);
    }
    gpDialogList = NULL;
}

//1.10f: D2Client.0x6FAD68F0
//  Pushes a control onto the dialog: an 8-byte { pObject, pNext } node from FOG_Alloc, inserted at the head of pControlList.
//  The control objects have vtables and every input handler above dispatches to them before falling back to the dialog's own lines:
//  +8 wheel, +12 mouse down, +16 mouse up, +20 mouse move, +24 char, +28/+32/+36/+40 the four arrows. A control answering 1 consumes the event.
//  The compiler re-reads pDialog->pControlList right after writing it, so a head insertion uses two slots for one pointer:
//      pOldHead = pDialog->pControlList;
//      pNode    = FOG_Alloc(8, ..., 860, 0);
//      pDialog->pControlList = pNode;
//      pNode[1]  = pOldHead;
//      pStored   = pDialog->pControlList;    <-- re-read, not reused
//      *pStored  = pControl;
//  Both are written out because naming them apart shows they are the same node.
//  BUG: FOG_Alloc is unchecked: a failed allocation stores null as the list head and dereferences it on the next line.
//  Four parameters over a retn 8; nUnused1 and nUnused2 are read by nothing in the body (the call sites pass values that read like
//  coordinates, and they are ignored).
uint32_t *__fastcall DIALOG_AddControl(D2ClientDialogStrc *pDialog, void *pControl, int32_t nUnused1, int32_t nUnused2)
{
    void *pOldHead;
    uint32_t *pNode;
    uint32_t *pStored;

    if (!pDialog)
    {
        FOG_DisplayAssert(szPDialogNull, szDialogCpp, 856);
        exit(-1);
    }

    pOldHead = pDialog->pControlList;
    pNode = (uint32_t *)FOG_Alloc(8, szDialogCpp, 860, 0);
    pDialog->pControlList = pNode;
    pNode[1] = (uint32_t)pOldHead;
    pStored = (uint32_t *)pDialog->pControlList;
    *pStored = (uint32_t)pControl;
    return pStored;
}

//1.10f: D2Client.0x6FAD6950
//  A dialog whose background is a cel: the image constructor. It refuses two ways before allocating: a null dismiss callback, and an
//  nImageIndex of 1 or more (see the index test below). gDialogImageNames has one entry, so the bound is the table's real extent.
//  The size comes from the cel rather than from the caller (D2CMP_CelGetWidth / CelGetHeight of the loaded image), and bOwnsImage is raised so
//  DIALOG_Close unloads it. There is no autosize and no line measurement: an image dialog is the image.
//  It shares the pNext defect with DIALOG_Create, at the same three instruction positions.
//  hCel is asserted at :950, so a missing or broken cel halts the game rather than drawing nothing.
D2ClientDialogStrc *__fastcall DIALOG_CreateFromImage(int32_t nCenterX, int32_t nCenterY, DIALOGCALLBACK pfnCallback, DIALOGCALLBACK pfnCallback2, int32_t nImageIndex, int32_t nInputMode)
{
    D2ClientDialogStrc *pDialog;
    D2GfxCellStrc *pCel;
    uint16_t nBoundKey;
    D2GfxDataStrc gfxRequest;
    char szPath[260];

    UI_AutomapSetInputEnabled(NULL);
    INV_SetHoverEnabled(0);
    GAME_ResetClickState();

    if (!pfnCallback)
    {
        return NULL;
    }
    //  BUG: the test is from above only: `jl` (0x6FAD6997) is signed, so a negative index is not refused. LATENT: the one caller,
    //  UI_OpenGoldEntryDialog, pushes 0 (0x6FB23C46), and so does ours.
    //  DIFFERENCE: in ours gDialogImageNames is dialog.h's one-element array, so a negative index would be undefined behaviour where the
    //  original reads its own .data; no caller makes one.
    if (nImageIndex >= 1)
    {
        return NULL;
    }

    ++gnDialogNestDepth;
    pDialog = (D2ClientDialogStrc *)FOG_Alloc(2832, szDialogCpp, 921, 0);
    memset(pDialog, 0, sizeof(D2ClientDialogStrc));
    //  BUG: the link is written between the two memsets and the second one erases it (it clears +0xB0C), so pNext is always nullptr. LATENT:
    //  nothing walks the chain while the nest-depth assert holds.
    pDialog->pNext = gpDialogList;
    gpDialogList = pDialog;
    memset(pDialog, 0, sizeof(D2ClientDialogStrc));

    pDialog->nWidth = 0;
    pDialog->nHeight = 0;
    pDialog->NumLines = 0;
    pDialog->nSelectableCount = 0;
    pDialog->nSelectedLine = -1;
    pDialog->eHighlight = 2;
    pDialog->nLeft = 0;
    pDialog->nTop = 0;
    pDialog->pControlList = NULL;
    pDialog->pfnCallback = pfnCallback;
    pDialog->pfnCallback2 = pfnCallback2;
    pDialog->bAutoSize = 0;
    pDialog->bVAlignByStaticLines = 0;
    pDialog->dwCreateTick = GetTickCount();
    pDialog->nPressedLine = -1;
    pDialog->bLinePressActive = 0;
    pDialog->nInputMode = nInputMode;
    pDialog->nFramesDrawn = 0;
    pDialog->bOwnsImage = 1;

    sprintf(szPath, "%s\\ui\\menu\\%s", aDataGlobal, gDialogImageNames[nImageIndex]);
    pDialog->hImage = ARCHIVE_CellFileLoad(szPath, 0);

    memset(&gfxRequest, 0, sizeof(gfxRequest));
    gfxRequest.pCellFile = pDialog->hImage;
    gfxRequest.nDirection = 0;
    gfxRequest.nFrame = 0;
    pCel = D2CMP_CelGetHandle(&gfxRequest);
    if (!pCel)
    {
        FOG_DisplayAssert(szHCel, szDialogCpp, 950);
        exit(-1);
    }
    pDialog->nWidth = D2CMP_CelGetWidth(pCel);
    pDialog->nHeight = D2CMP_CelGetHeight(pCel);
    pDialog->nCenterY = nCenterY;
    pDialog->nCenterX = nCenterX;
    pDialog->nLeft = nCenterX;
    pDialog->nTop = nCenterY;

    gbDialogHandlersRegistered = 1;
    //  the table's two cancel-key slots are patched from the player's binding
    if (CMDTBL_ControlsIsActionBound(HOTKEY_CLEARSCREEN, 1))
    {
        nBoundKey = CMDTBL_ControlsFindBoundKey(HOTKEY_CLEARSCREEN, 1);
        if (nBoundKey < 0xE0u)
        {
            gDialogMessageHandlers[0].nMessageOrKey = nBoundKey;
        }
    }
    if (CMDTBL_ControlsIsActionBound(HOTKEY_CLEARSCREEN, 0))
    {
        nBoundKey = CMDTBL_ControlsFindBoundKey(HOTKEY_CLEARSCREEN, 0);
        if (nBoundKey < 0xE0u)
        {
            gDialogMessageHandlers[1].nMessageOrKey = nBoundKey;
        }
    }

    WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gDialogMessageHandlers, 14);
    if (GAME_HasNativeMouseWheel())
    {
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gDialogWheelHandlers, 1);
    }
    ++gnDialogWheelHandlersRefCount;

    if (gnDialogNestDepth != 1)
    {
        FOG_DisplayAssert(szZero, szDialogCpp, 974);
        exit(-1);
    }
    return pDialog;
}

//1.10f: D2Client.0x6FAD6BE0
//  The ordinary constructor, and this function is the struct definition: FOG_Alloc(2832, ...) followed by the field writes.
//  The two position pairs start equal (nCenterX into both nCenterX and nLeft, nCenterY into both nCenterY and nTop) and DIALOG_AutoSize moves
//  the second pair, so the first is what the caller asked for and the second is where it ended up.
//  eHighlight is initialised to 2; the assert in DIALOG_AddLine shows DIALOG_HIGHLIGHT_COLOR is 1, so the default is the other mode, the
//  animated focus cel that DIALOG_Draw puts either side of the selected line.
//  The dismiss callback is the only argument validated: null returns without allocating, and without undoing the three UI calls at the top.
//  nHeight is written twice, once as 0 among the defaults and once from the parameter later on; both are in the bytes.
//  bVAlignByStaticLines is named after the count it actually uses (the non-selectable lines; see DIALOG_AutoSize).
D2ClientDialogStrc *__fastcall DIALOG_Create(int32_t nCenterX, int32_t nCenterY, DIALOGCALLBACK pfnCallback, DIALOGCALLBACK pfnCallback2, int32_t nInputMode, int32_t bVAlignByStaticLines, int32_t bAutoSize, int32_t nWidth,
                                             int32_t nHeight, int32_t bIgnoreCancelKey, int32_t bGrabsKeyboard)
{
    D2ClientDialogStrc *pDialog;
    uint16_t nBoundKey;

    UI_AutomapSetInputEnabled(NULL);
    INV_SetHoverEnabled(0);
    GAME_ResetClickState();

    if (!pfnCallback)
    {
        return NULL;
    }

    ++gnDialogNestDepth;
    pDialog = (D2ClientDialogStrc *)FOG_Alloc(2832, szDialogCpp, 1005, 0);
    memset(pDialog, 0, sizeof(D2ClientDialogStrc));
    //  BUG: the link is written between the two memsets and the second one erases it (it clears +0xB0C), so pNext is always nullptr. LATENT:
    //  nothing walks the chain while the nest-depth assert holds.
    pDialog->pNext = gpDialogList;
    gpDialogList = pDialog;
    memset(pDialog, 0, sizeof(D2ClientDialogStrc));

    pDialog->nCenterX = nCenterX;
    pDialog->nLeft = nCenterX;
    pDialog->pfnCallback = pfnCallback;
    pDialog->pfnCallback2 = pfnCallback2;
    pDialog->bAutoSize = bAutoSize;
    pDialog->nHeight = 0;
    pDialog->NumLines = 0;
    pDialog->nSelectableCount = 0;
    pDialog->nSelectedLine = -1;
    pDialog->eHighlight = 2;
    pDialog->pControlList = NULL;
    pDialog->nCenterY = nCenterY;
    pDialog->nTop = nCenterY;
    pDialog->bVAlignByStaticLines = bVAlignByStaticLines;
    pDialog->bIgnoreCancelKey = bIgnoreCancelKey;
    pDialog->nWidth = nWidth;
    pDialog->dwCreateTick = GetTickCount();
    pDialog->bLinePressActive = 0;
    pDialog->nFramesDrawn = 0;
    pDialog->bOwnsImage = 0;
    pDialog->hImage = NULL;
    pDialog->nHeight = nHeight;
    pDialog->nInputMode = nInputMode;
    pDialog->nPressedLine = -1;
    pDialog->bGrabsKeyboard = bGrabsKeyboard;

    gbDialogHandlersRegistered = 1;
    if (CMDTBL_ControlsIsActionBound(HOTKEY_CLEARSCREEN, 1))
    {
        nBoundKey = CMDTBL_ControlsFindBoundKey(HOTKEY_CLEARSCREEN, 1);
        if (nBoundKey < 0xE0u)
        {
            gDialogMessageHandlers[0].nMessageOrKey = nBoundKey;
        }
    }
    if (CMDTBL_ControlsIsActionBound(HOTKEY_CLEARSCREEN, 0))
    {
        nBoundKey = CMDTBL_ControlsFindBoundKey(HOTKEY_CLEARSCREEN, 0);
        if (nBoundKey < 0xE0u)
        {
            gDialogMessageHandlers[1].nMessageOrKey = nBoundKey;
        }
    }

    WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gDialogMessageHandlers, 14);
    if (GAME_HasNativeMouseWheel())
    {
        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gDialogWheelHandlers, 1);
    }
    ++gnDialogWheelHandlersRefCount;

    if (gnDialogNestDepth != 1)
    {
        FOG_DisplayAssert(szZero, szDialogCpp, 1040);
        exit(-1);
    }
    return pDialog;
}

//1.10f: D2Client.0x6FAD6DC0
//  The ordinary close, and it asserts the nest depth reaches zero: `--gnDialogNestDepth` is tested for non-zero, so closing a dialog that was
//  never constructed, or closing twice, halts the game. That assert and the two in the constructors are what keep the list one deep.
//  It undoes the constructor in order: the automap gets its input back, the handlers come off, hover is re-enabled, the click region is
//  cleared, the control nodes are freed one at a time, and an owned image is unloaded. The control objects are not freed, only their nodes;
//  whoever called DIALOG_AddControl owns the object.
//  It is handed the dialog in ecx and does not read it: the body writes ecx (`mov ecx, 1` for UI_AutomapSetInputEnabled) before it reads it
//  and closes gpDialogList's head whatever was passed. __fastcall is this module's spelling for a free function with one ecx argument.
void __fastcall DIALOG_Close(D2ClientDialogStrc *hDialog)
{
    D2ClientDialogStrc *pDialog;
    uint32_t *pControl;
    uint32_t *pNextControl;
    D2ClientDialogStrc *pNext;

    if (--gnDialogNestDepth)
    {
        FOG_DisplayAssert(szZero, szDialogCpp, 1089);
        exit(-1);
    }

    UI_AutomapSetInputEnabled((D2UnkOutdoorStrc2 *)1);
    WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gDialogMessageHandlers, 14);
    if (GAME_HasNativeMouseWheel())
    {
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gDialogWheelHandlers, 1);
    }
    INV_SetHoverEnabled(1);
    SELECT_SetClickRegion(NULL);

    pDialog = gpDialogList;
    gbDialogHandlersRegistered = 0;

    pControl = (uint32_t *)pDialog->pControlList;
    if (pControl)
    {
        do
        {
            pNextControl = (uint32_t *)pControl[1];
            FOG_Free(pControl, szDialogCpp, 1072, 0);
            pControl = pNextControl;
        }
        while (pNextControl);
    }

    if (pDialog->bOwnsImage)
    {
        if (pDialog->hImage)
        {
            ARCHIVE_CellFileFree(pDialog->hImage);
        }
    }

    pNext = pDialog->pNext;
    FOG_Free(pDialog, szDialogCpp, 1083, 0);
    gpDialogList = pNext;
}

//1.10f: D2Client.0x6FAD6EB0
//  Draws the dialog: background, controls, then the lines.
//  This is the only writer of nFramesDrawn, and it increments before anything is drawn, so the count includes the frame in progress. Five of
//  these have to happen before any of the dismiss paths will fire the callback, so a dialog that is constructed and never drawn cannot be
//  cancelled at all.
//  The background is an image or a frame, never both. bOwnsImage picks: a cel drawn at (nLeft, nTop + nHeight), the cel's Y being its bottom,
//  or a filled box plus UIPANELS_DrawTiledFrame over a RECT whose right and bottom are one pixel inside the box. The rect is built before the
//  branch and is unused on the image path.
//  When the selected line is reached and eHighlight is 2, two copies of UI_GetFocusImage's cel are drawn, one 24 pixels left of the text and
//  one just past its right edge, at frame `gnDialogCelFrameCounter % 7`, and the counter is incremented after the pair. So it advances once
//  per selected line per frame, which in practice is once per frame since a dialog cannot have two selected lines.
//  eHighlight == 1 instead recolours the line to 3 and draws no cel. The two are exclusive in effect but tested independently, so a third
//  value gives neither.
//  The font is saved once around the whole pass and restored by the return expression, `return D2WIN_10127_SetFont(nPrevFont)`, which is why
//  this function has an int return that nothing reads.
int32_t __fastcall DIALOG_Draw(D2ClientDialogStrc *pDialog)
{
    D2GfxDataStrc pData;
    RECT pRect;
    int32_t nWidth;
    int32_t nTop;
    int32_t nLeft;
    int32_t nRight;
    int32_t nBottomEdge;
    int32_t bOwnsImage;
    uint32_t *pControl;
    int32_t nPrevFont;
    int32_t nYOffset;
    int32_t iLine;
    int32_t nLineX;
    int32_t nLineY;
    int32_t nColor;
    int32_t eHighlight;

    if (!pDialog)
    {
        FOG_DisplayAssert(szPDialogNull, szDialogCpp, 1177);
        exit(-1);
    }

    memset(&pData, 0, sizeof(pData));
    nWidth = pDialog->nWidth;
    pData.nDirection = 0;
    nTop = pDialog->nTop;
    bOwnsImage = pDialog->bOwnsImage;
    ++pDialog->nFramesDrawn;
    nLeft = pDialog->nLeft;
    pRect.top = nTop;
    pRect.left = nLeft;
    nRight = nWidth + nLeft - 1;
    nBottomEdge = nTop + pDialog->nHeight;
    pRect.right = nRight;
    pRect.bottom = nBottomEdge - 1;

    if (bOwnsImage)
    {
        if (pDialog->hImage)
        {
            pData.pCellFile = pDialog->hImage;
            pData.nFrame = 0;
            D2GFX_TEXTURE_CelDraw(&pData, nLeft, nBottomEdge, 0xFFFFFFFF, 5, NULL);
        }
    }
    else
    {
        GFX_DrawFilledRect(nLeft, nTop, nRight - nLeft + 1, nBottomEdge - nTop, 0, 1);
        UIPANELS_DrawTiledFrame(&pRect);
    }

    for (pControl = (uint32_t *)pDialog->pControlList; pControl; pControl = (uint32_t *)pControl[1])
    {
        if (*pControl)
        {
            (*(void(__thiscall **)(uint32_t))(*(uint32_t *)*pControl + 4))(*pControl);
        }
    }

    nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT16);
    nYOffset = 0;
    if (pDialog->NumLines > 0)
    {
        iLine = 0;
        do
        {
            nColor = pDialog->Lines[iLine].nBaseColor;
            D2WIN_10127_SetFont((enum Font)pDialog->Lines[iLine].nFont);
            nLineX = pDialog->nLeft + pDialog->Lines[iLine].nCenterOffset;
            nLineY = nYOffset + pDialog->nTop + pDialog->Lines[iLine].nLineHeight;

            if (pDialog->nSelectedLine == iLine)
            {
                eHighlight = pDialog->eHighlight;
                if (eHighlight == 1)
                {
                    nColor = 3;
                }
                if (eHighlight == 2)
                {
                    pData.pCellFile = UI_GetFocusImage();
                    pData.nFrame = gnDialogCelFrameCounter % 7;
                    D2GFX_TEXTURE_CelDraw(&pData, nLineX - 24, nLineY + 4, 0xFFFFFFFF, 5, NULL);
                    D2GFX_TEXTURE_CelDraw(&pData, pDialog->Lines[iLine].nTextWidth + nLineX + 2, nLineY + 4, 0xFFFFFFFF, 5, NULL);
                    ++gnDialogCelFrameCounter;
                }
            }

            D2WIN_10117_DrawText(pDialog->Lines[iLine].wszText, nLineX, nLineY, nColor, 0);
            nYOffset += pDialog->Lines[iLine].nLineHeight;
            ++iLine;
        }
        while (iLine < pDialog->NumLines);
    }

    return D2WIN_10127_SetFont((enum Font)nPrevFont);
}

//1.10f: D2Client.0x6FAD7090
//  Selects a line by index, with three asserts and a walk that exists only to reach the third:
//      :1266  pDialog != NULL
//      :1272  index < pDialog->NumLines      -- but not index >= -1
//      :1306  pDialog->Line[idx].bSelectable
//      :1310  "0"                            -- the walk ran off the end
//  -1 is the "nothing selected" value and is taken before the walk. Any other negative index passes the :1272 test, then walks looking for a
//  match it can never find, runs past NumLines and halts at :1310; so an out-of-range index halts either way, and which assert fires depends
//  on its sign.
//  The walk is a linear scan for an index the caller already has, stepping 68 dwords a record; the original does not index the array.
void __fastcall DIALOG_SetSelectedLine(D2ClientDialogStrc *pDialog, int32_t nIndex)
{
    int32_t nNumLines;
    int32_t iLine;

    if (!pDialog)
    {
        FOG_DisplayAssert(szPDialogNull, szDialogCpp, 1266);
        exit(-1);
    }

    nNumLines = pDialog->NumLines;
    if (nIndex >= nNumLines)
    {
        FOG_DisplayAssert(szIndexLtNumLines, szDialogCpp, 1272);
        exit(-1);
    }

    if (nIndex == -1)
    {
        pDialog->nSelectedLine = -1;
        return;
    }

    iLine = 0;
    if (nNumLines <= 0)
    {
        FOG_DisplayAssert(szZero, szDialogCpp, 1310);
        exit(-1);
    }
    while (nIndex != iLine)
    {
        ++iLine;
        if (iLine >= nNumLines)
        {
            FOG_DisplayAssert(szZero, szDialogCpp, 1310);
            exit(-1);
        }
    }
    if (!pDialog->Lines[iLine].bSelectable)
    {
        FOG_DisplayAssert(szLineIdxSelectable, szDialogCpp, 1306);
        exit(-1);
    }
    pDialog->nSelectedLine = nIndex;
}

//1.10f: D2Client.0x6FAD7150
//  Sets the highlight mode (eHighlight): one store behind one assert.
void __fastcall DIALOG_SetHighlight(D2ClientDialogStrc *pDialog, int32_t eHighlight)
{
    if (!pDialog)
    {
        FOG_DisplayAssert(szPDialogNull, szDialogCpp, 1321);
        exit(-1);
    }
    pDialog->eHighlight = eHighlight;
}

//1.10f: D2Client.0x6FAD7180
//  Appends a line; ten at most, asserted at :1373.
//  The highlight assert, `BaseColor != FONTCOLOR_BLUE || pDialog->eHighlight != DIALOG_HIGHLIGHT_COLOR`, refuses colour 3 with highlight mode
//  1, the combination that would make a selected line indistinguishable from an unselected one, because mode 1 recolours the selection to 3.
//  The text is clamped to 119 and terminated twice:
//      nMax = Unicode::strlen(wszText) + 1;    room for the terminator
//      if ( nMax > 119 ) nMax = 119;
//      Unicode::strncpy(dest, wszText, nMax);
//      dest[nMax] = 0;
//  The buffer is 120 Unicode, so dest[119] is the last element and the explicit terminator lands inside it on every path: no overflow, and
//  the belt-and-braces terminator is the original's.
//  A dialog with bAutoSize re-runs DIALOG_AutoSize over every line; one without measures just this line and centres it in the fixed width.
//  The centring is `(nWidth - nTextWidth + 1) / 2 + 1`: rounded up, then shifted one more pixel right, as in the original.
//  The click region is set from the current box on every added line, and the first selectable line added to a dialog with nothing selected
//  becomes the selection.
void __fastcall DIALOG_AddLine(D2ClientDialogStrc *pDialog, const Unicode *wszText, int32_t nLineHeight, int32_t nUnused1, int32_t nBaseColor, int32_t nFont, DIALOGCALLBACK pfnOnClick, int32_t bSelectable)
{
    int32_t nIndex;
    int32_t nMax;
    int32_t nPrevFont;
    int32_t nTextWidth;
    int32_t nWidth;
    int32_t pRect[4];

    nIndex = pDialog->NumLines;
    if (nIndex >= 10)
    {
        FOG_DisplayAssert(szNumLinesLtMax, szDialogCpp, 1373);
        exit(-1);
    }
    pDialog->NumLines = nIndex + 1;

    pDialog->Lines[nIndex].nLineHeight = nLineHeight;
    pDialog->Lines[nIndex].nFont = nFont;
    pDialog->Lines[nIndex].nCenterOffset = 0;
    pDialog->Lines[nIndex].nUnusedParam = nUnused1;

    if (nBaseColor == 3 && pDialog->eHighlight == 1)
    {
        FOG_DisplayAssert(szBaseColorVsHighlight, szDialogCpp, 1385);
        exit(-1);
    }

    pDialog->Lines[nIndex].nBaseColor = nBaseColor;
    pDialog->Lines[nIndex].pfnOnSelect = (int32_t)pfnOnClick;
    pDialog->Lines[nIndex].bSelectable = bSelectable;

    nMax = Unicode::strlen(wszText) + 1;
    if (nMax > 119)
    {
        nMax = 119;
    }
    Unicode::strncpy(pDialog->Lines[nIndex].wszText, wszText, nMax);
    pDialog->Lines[nIndex].wszText[nMax].ch = 0;

    if (bSelectable)
    {
        ++pDialog->nSelectableCount;
    }

    if (pDialog->bAutoSize)
    {
        DIALOG_AutoSize(pDialog);
    }
    else
    {
        nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT16);
        D2WIN_10127_SetFont((enum Font)pDialog->Lines[nIndex].nFont);
        nTextWidth = D2WIN_10121_GetTextWidth(pDialog->Lines[nIndex].wszText);
        nWidth = pDialog->nWidth;
        if (nTextWidth < nWidth)
        {
            pDialog->Lines[nIndex].nCenterOffset = (nWidth - nTextWidth + 1) / 2 + 1;
        }
        else
        {
            pDialog->Lines[nIndex].nCenterOffset = 0;
        }
        pDialog->Lines[nIndex].nTextWidth = nTextWidth;
        D2WIN_10127_SetFont((enum Font)nPrevFont);
    }

    pRect[0] = pDialog->nLeft;
    pRect[1] = pDialog->nTop;
    pRect[2] = pDialog->nLeft + pDialog->nWidth;
    pRect[3] = pDialog->nTop + pDialog->nHeight;
    SELECT_SetClickRegion(pRect);

    if (pDialog->nSelectedLine == -1)
    {
        if (bSelectable)
        {
            DIALOG_SetSelectedLine(pDialog, nIndex);
        }
    }
}

//1.10f: D2Client.0x6FAD7320
//  Measures the dialog and places it: two passes over the lines, with the size decided between them.
//  Pass one measures and counts the non-selectable lines. That count, not the line count, is what the vertical anchor multiplies:
//      nLeft = nCenterX - MaxWidth / 2;
//      if ( bVAlignByStaticLines ) nTop = nCenterY - 21 * nStaticLines;
//      else                        nTop = nCenterY - MaxHeight / 3;
//  so a dialog aligned this way is placed by its captions and ignores its options. 21 is the font row height this file uses throughout.
//  The padding is asymmetric: MaxWidth = widest line + 20, MaxHeight = summed line heights + 15. Both are then asserted against the screen,
//  width against `gnScreenWidth - 80` and height against `gnScreenHeight - 98`, so a dialog too large for the screen halts the game rather
//  than clipping.
//  The clamps run after the asserts and use different margins:
//      right edge past gnScreenWidth - 10   ->  nLeft = gnScreenWidth - MaxWidth
//      bottom past gnScreenHeight - 58      ->  nTop = height - MaxHeight - 48
//      then nLeft and nTop are floored at 10
//  The right clamp leaves no margin and the bottom leaves ten, as in the original. gnScreenHeight is re-read once in the middle of that
//  sequence; the re-read is kept because it is in the bytes, and nothing between can change it.
//  Pass two re-centres every line inside the new width, saving and restoring the font per line rather than once around the loop, as the
//  original does.
void __fastcall DIALOG_AutoSize(D2ClientDialogStrc *pDialog)
{
    int32_t iLine;
    int32_t nWidestLine;
    int32_t nTotalHeight;
    int32_t nStaticLines;
    int32_t nTextWidth;
    int32_t MaxWidth;
    int32_t MaxHeight;
    int32_t nLeft;
    int32_t nScreenHeight;
    int32_t nPrevFont;
    int32_t nWidth;

    nWidestLine = 0;
    nTotalHeight = 0;
    nStaticLines = 0;

    if (!pDialog->bAutoSize)
    {
        FOG_DisplayAssert(szAutoSizeTrue, szDialogCpp, 1450);
        exit(-1);
    }

    nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT16);
    if (pDialog->NumLines > 0)
    {
        iLine = 0;
        do
        {
            nTextWidth = D2WIN_10121_GetTextWidth(pDialog->Lines[iLine].wszText);
            if (nTextWidth > nWidestLine)
            {
                nWidestLine = nTextWidth;
            }
            if (!pDialog->Lines[iLine].bSelectable)
            {
                ++nStaticLines;
            }
            nTotalHeight += pDialog->Lines[iLine].nLineHeight;
            ++iLine;
        }
        while (iLine < pDialog->NumLines);
    }

    MaxWidth = nWidestLine + 20;
    MaxHeight = nTotalHeight + 15;
    D2WIN_10127_SetFont((enum Font)nPrevFont);

    nLeft = pDialog->nCenterX - MaxWidth / 2;
    pDialog->nLeft = nLeft;
    if (pDialog->bVAlignByStaticLines)
    {
        pDialog->nTop = pDialog->nCenterY - 21 * nStaticLines;
    }
    else
    {
        pDialog->nTop = pDialog->nCenterY - MaxHeight / 3;
    }

    if (MaxWidth > gnScreenWidth - 80)
    {
        FOG_DisplayAssert(szMaxWidth, szDialogCpp, 1501);
        exit(-1);
    }
    nScreenHeight = gnScreenHeight;
    if (MaxHeight > gnScreenHeight - 98)
    {
        FOG_DisplayAssert(szMaxHeight, szDialogCpp, 1506);
        exit(-1);
    }

    if (MaxWidth + nLeft > gnScreenWidth - 10)
    {
        pDialog->nLeft = gnScreenWidth - MaxWidth;
        //  re-read, as the bytes have it
        nScreenHeight = gnScreenHeight;
    }
    if (MaxHeight + pDialog->nTop > nScreenHeight - 58)
    {
        pDialog->nTop = nScreenHeight - MaxHeight - 48;
    }
    if (pDialog->nLeft <= 10)
    {
        pDialog->nLeft = 10;
    }
    if (pDialog->nTop <= 10)
    {
        pDialog->nTop = 10;
    }

    pDialog->nWidth = MaxWidth;
    pDialog->nHeight = MaxHeight;

    if (pDialog->NumLines > 0)
    {
        iLine = 0;
        do
        {
            //  the font is saved and restored PER LINE here
            nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT16);
            D2WIN_10127_SetFont((enum Font)pDialog->Lines[iLine].nFont);
            nTextWidth = D2WIN_10121_GetTextWidth(pDialog->Lines[iLine].wszText);
            nWidth = pDialog->nWidth;
            if (nTextWidth < nWidth)
            {
                pDialog->Lines[iLine].nCenterOffset = (nWidth - nTextWidth + 1) / 2 + 1;
            }
            else
            {
                pDialog->Lines[iLine].nCenterOffset = 0;
            }
            pDialog->Lines[iLine].nTextWidth = nTextWidth;
            D2WIN_10127_SetFont((enum Font)nPrevFont);
            ++iLine;
        }
        while (iLine < pDialog->NumLines);
    }
}

//1.10f: D2Client.0x6FAD7520
//  Returns which line is selected, or -1 when there is no dialog. nSelectedLine (+0x44) is what DIALOG_OnMouseUp indexes to fetch the callback
//  it fires; nPressedLine (+0x48) holds the line the mouse went down on and is only compared against a fresh hit test, the drag guard.
int32_t __fastcall DIALOG_GetSelectedLine(D2ClientDialogStrc *pDialog)
{
    if (pDialog)
    {
        return pDialog->nSelectedLine;
    }
    return -1;
}

//1.10f: D2Client.0x6FAD7530
//  Returns gnDialogNestDepth, which carries the invariant the whole file rests on: both constructors assert it is exactly 1 on the way out and
//  DIALOG_Close asserts it reaches 0, so two dialogs cannot be under construction at once. Callers use it as "is a dialog open".
int32_t __cdecl DIALOG_GetNestDepth(void)
{
    return gnDialogNestDepth;
}

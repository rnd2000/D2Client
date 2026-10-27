#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Listbox.h"

//  Placement new, for the construction of the entry columns in LISTBOX_ImplConstruct.
#include <new>

//  Listbox.cpp -- the list box control of D2Client.dll 1.10: a 12-byte proxy (gVtblListBox, the body pointer at +8) wrapping a 1574774-byte body
//  (gVtblListBoxImpl: up to 128 entries of six Unicode[1024] columns), its scroll bar, hit tests, and arrow, track and wheel handlers, plus the
//  base destructor and three proxy forwarders that other control classes share.
//  The original source file is UI\LISTBOX.CPP (the assert at 0x6FB79B08 names it). Address range 0x6FAEFA40..0x6FAF12B7.
//
//  Body entry (stride 12302): Unicode wszCol[6][1024], each NUL-terminated at index 1023; then field_3000, nUserData at +0x3002, nColor at +0x3006
//  and nFont at +0x300A (14 means "use the control's own font"). A row's height is not fixed -- nLineHeight per non-empty column plus one
//  nEntryGap -- so the hit test, the fit count and the thumb update all walk the rows instead of dividing.
//  A UICTRLPROXY_FwdSlotN forwarder is named after the body slot it reaches, not the proxy index it sits at.
//
//  Calling convention: the members are __thiscall (this in ecx, the rest on the stack, callee cleans). A free function cannot say that, so members
//  that take only this are written __fastcall (the same ABI for one register argument) and the others take an unused nEdxUnused second parameter,
//  so that ecx, edx and the stack line up as __thiscall's do.
//  DIFFERENCE: the body's seven dispatches through pListBox->pVtbl call typed slots written in this spelling, so ours passes 0 in edx where the
//  original leaves edx as it stood (0x6FAF057E..0x6FAF0587 it holds the table). Neither side's callee reads edx.
//
//  The original's __FILE__: the assert's source path (0x6FB79B08).
static const char szListBoxCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UI\\LISTBOX.CPP";
//  The data root the scroll bar's cel path is built on.
static const char aDataGlobal[] = "DATA\\GLOBAL";
//  Allocation tag for STORM_SMemAlloc.
static const char szNew[] = "new";
//  Tag the deleting destructors free with (the original keeps it separate from the allocation tag).
static const char szDelete[] = "delete";

//  The entry capacity; the unit's own assert text is "mNumEntries < LISTBOX_MAX_ENTRIES".
#define LISTBOX_MAX_ENTRIES 128

//  +0x18071C is five adjacent dwords: four press flags and the mouse Y the thumb was grabbed at. Index 4 is written and never read (see
//  LISTBOX_ImplOnMouseDown): dragging the thumb does nothing in the shipped game.
enum
{
    LBPRESS_ACTIVE = 0,     // a press is in flight; cleared first on release
    LBPRESS_UP,             // the up arrow is held    -> body slot 7
    LBPRESS_DOWN,           // the down arrow is held  -> body slot 8
    LBPRESS_TRACK,          // the track is held       -> ScrollByTrackClick
    LBPRESS_GRAB_Y,         //  mouse Y when the thumb was pressed; never read
};

//  Layout checks; each message names the instruction its number comes from.
static_assert(offsetof(D2ClientListBoxStrc, entries) == 0x000008,
              "the body's first field beyond the vtable, 0x6FAEFAC0");
static_assert(offsetof(D2ClientListBoxStrc, nX) == 0x180708
              && offsetof(D2ClientListBoxStrc, nY) == 0x18070C
              && offsetof(D2ClientListBoxStrc, nWidth) == 0x180710
              && offsetof(D2ClientListBoxStrc, nHeight) == 0x180714
              && offsetof(D2ClientListBoxStrc, nScrollBarRows) == 0x180718,
              "the constructor's argument stores, 0x6FAEFB80..0x6FAEFBE9");
static_assert(offsetof(D2ClientListBoxStrc, field_18071C) == 0x18071C
              && offsetof(D2ClientListBoxStrc, nTopEntry) == 0x180730
              && offsetof(D2ClientListBoxStrc, nHoveredEntry) == 0x180734
              && offsetof(D2ClientListBoxStrc, nSelection) == 0x180738
              && offsetof(D2ClientListBoxStrc, nStringsStored) == 0x18073C
              && offsetof(D2ClientListBoxStrc, nDefaultColor) == 0x180740
              && offsetof(D2ClientListBoxStrc, nFont) == 0x180744,
              "the press flags and the selection block; the five press dwords "
              "are what OnMouseDown writes at [esi+18071Ch..18072Ch]");
static_assert(offsetof(D2ClientListBoxStrc, wNumEntries) == 0x180748
              && offsetof(D2ClientListBoxStrc, pfnOnSelect) == 0x18074A
              && offsetof(D2ClientListBoxStrc, nVisibleRows) == 0x18074E,
              "wNumEntries is a WORD at an even address and the struct is "
              "packed from there on -- pfnOnSelect sits at +2, not +4");
static_assert(offsetof(D2ClientListBoxStrc, nThumbTop) == 0x180752
              && offsetof(D2ClientListBoxStrc, nThumbBottom) == 0x180756
              && offsetof(D2ClientListBoxStrc, nScrollBarX) == 0x18075A
              && offsetof(D2ClientListBoxStrc, nScrollBarY) == 0x18075E
              && offsetof(D2ClientListBoxStrc, nTrackRows) == 0x180762,
              "the scroll bar block; the thumb rows and the bar's origin are "
              "12 pixels apart in UpdateScrollThumb and ScrollByTrackClick");
static_assert(offsetof(D2ClientListBoxStrc, nLineHeight) == 0x180766
              && offsetof(D2ClientListBoxStrc, nEntryGap) == 0x18076A
              && offsetof(D2ClientListBoxStrc, nHoldTicks) == 0x18076E
              && offsetof(D2ClientListBoxStrc, nDrawCount) == 0x180772,
              "the row metrics and the tail pair; 0x180772 ends exactly where "
              "the allocation ends, 0x180776");
static_assert(offsetof(D2ClientListBoxEntryStrc, wszCol) == 0x0000
              && offsetof(D2ClientListBoxEntryStrc, field_3000) == 0x3000
              && offsetof(D2ClientListBoxEntryStrc, nUserData) == 0x3002
              && offsetof(D2ClientListBoxEntryStrc, nColor) == 0x3006
              && offsetof(D2ClientListBoxEntryStrc, nFont) == 0x300A,
              "the entry's five members, 12302 bytes; GetEntryUserData reads "
              "nUserData at 12302 * i + 12298");
static_assert(LISTBOX_MAX_ENTRIES == 128,
              "the assert's own text and the constructor's 128-iteration loop");
static_assert(offsetof(SMSGHANDLER_PARAMS, nXpos) == 0x0C
              && offsetof(SMSGHANDLER_PARAMS, nYpos) == 0x0E
              && offsetof(SMSGHANDLER_PARAMS, bReturn) == 0x18
              && offsetof(SMSGHANDLER_PARAMS, lResult) == 0x1C,
              "the four message fields this unit reads; the cursors at +12 and "
              "+14, the two the handlers write at the end");

//  Element size 2 and 1024 elements per column, as the six vector-constructor calls in LISTBOX_ImplConstruct pass them.
static_assert(sizeof(Unicode) == 2, "push 2 / push 400h at 0x6FAEFAE2");


//1.10f: D2Client.0x6FAEFA40
//  Loads the scroll bar image, DATA\GLOBAL\ui\menu\textslid, into gpImgListBoxScrollBar: the one image every list box scroll bar is drawn from.
//  Called once, by UI_InitUiSubsystems.
//  void: eax is left holding ARCHIVE_CellFileLoad's own handle, and its one caller does not read it.
void __cdecl LISTBOX_LoadScrollBarImage(void)
{
    char szBuffer[260];
    D2CellFileStrc *pImage;

    sprintf(szBuffer, "%s\\ui\\menu\\textslid", aDataGlobal);
    pImage = ARCHIVE_CellFileLoad(szBuffer, 0);
    gpImgListBoxScrollBar = pImage;
}


//1.10f: D2Client.0x6FAEFA80
//  Releases gpImgListBoxScrollBar and nulls it. Called once, by UI_ShutdownUiSubsystems. void: on the null path the original's retn leaves eax as
//  it found it, and the caller discards it.
void __cdecl LISTBOX_UnloadScrollBarImage(void)
{
    if (gpImgListBoxScrollBar)
    {
        ARCHIVE_CellFileFree(gpImgListBoxScrollBar);
        gpImgListBoxScrollBar = NULL;
    }
}


//1.10f: D2Client.0x6FAEFAA0
//  The list box body constructor. LISTBOX_Construct allocates the 1574774-byte block and hands it here. Forwards the nine arguments into members,
//  except bWideLayout, which is not stored: it only picks 28 versus 18 scroll-bar rows together with the resolution mode. The scroll bar
//  geometry is written at the tail:
//      nScrollBarX = nX + nWidth - 18, nScrollBarY = nY + 16, nTrackRows = (nHeight - 16) / 12 - 1 (1 when nHeight < 32), nLineHeight = 14
//  nHoveredEntry and nSelection are set to -1 above the vtable swap; the four live press flags are zeroed but LBPRESS_GRAB_Y is not (the original
//  leaves the fifth dword untouched).
//  The original's compiler constructs the member array through D2Lang's vector constructor, six calls per entry over 128 entries; the storage here
//  is a plain allocation, so the loop is written out.
//  DIFFERENCE: ours calls D2Lang's one-argument Unicode constructor with 0 (it stores 0) where the original passes D2Lang's default-constructor
//  closure (`mov word ptr [ecx], 0`, which also stores 0); the vector-constructor frames are absent.
//  DIFFERENCE: the original's prologue (0x6FAEFAA0..0x6FAEFAAE) registers an exception-handling frame whose one unwind action runs
//  LISTBOX_UiCtrlBaseDestruct on the half-built body if a member construction throws; ours has none.
D2ClientListBoxStrc *__fastcall LISTBOX_ImplConstruct(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, int32_t nX, int32_t nY, int32_t nWidth, int32_t nHeight, int32_t nEntryGap, int32_t nDefaultColor, int32_t nFont, int32_t (__cdecl *pfnOnSelect)(),
                                                      uint8_t bWideLayout)
{
    int32_t nEntry;
    int32_t nCol;
    int32_t nCh;

    //  The abstract base's table first, as a C++ base constructor installs its own (0x6FAEFAC0). The member is typed for the body's table
    //  (gVtblListBoxImpl, stored below) and the base's is another struct type, so this cast is the only spelling of that relation.
    pListBox->pVtbl = (const D2ListBoxImplVtbl *)&gVtblUiControlBase;

    //  The member array's construction, written out (see the head).
    for (nEntry = 0; nEntry < LISTBOX_MAX_ENTRIES; ++nEntry)
    {
        for (nCol = 0; nCol < 6; ++nCol)
        {
            for (nCh = 0; nCh < 1024; ++nCh)
            {
                new (&pListBox->entries[nEntry].wszCol[nCol][nCh]) Unicode(0);
            }
        }
    }

    pListBox->nY = nY;
    pListBox->nWidth = nWidth;
    pListBox->nHoveredEntry = -1;
    pListBox->nSelection = -1;
    pListBox->nX = nX;
    pListBox->nDefaultColor = nDefaultColor;
    pListBox->nFont = nFont;
    pListBox->pVtbl = &gVtblListBoxImpl;
    pListBox->nHeight = nHeight;
    pListBox->field_18071C[LBPRESS_ACTIVE] = 0;
    pListBox->field_18071C[LBPRESS_TRACK] = 0;
    pListBox->field_18071C[LBPRESS_UP] = 0;
    pListBox->field_18071C[LBPRESS_DOWN] = 0;
    pListBox->nDrawCount = 0;
    pListBox->nTopEntry = 0;
    pListBox->wNumEntries = 0;

    //  The second pass over the same 128 entries: zero the head of each of the six columns and the three trailing dwords, and preset the font.
    //  The store order is the original's: the word at +0x3000, then nColor, then nFont = 1, then nUserData.
    for (nEntry = 0; nEntry < LISTBOX_MAX_ENTRIES; ++nEntry)
    {
        D2ClientListBoxEntryStrc *pEntry = &pListBox->entries[nEntry];

        pEntry->wszCol[0][0].ch = 0;
        pEntry->wszCol[1][0].ch = 0;
        pEntry->wszCol[2][0].ch = 0;
        pEntry->wszCol[3][0].ch = 0;
        pEntry->wszCol[4][0].ch = 0;
        pEntry->wszCol[5][0].ch = 0;
        pEntry->field_3000 = 0;
        pEntry->nColor = 0;
        pEntry->nFont = 1;
        pEntry->nUserData = 0;
    }

    pListBox->pfnOnSelect = pfnOnSelect;
    pListBox->nEntryGap = nEntryGap;
    pListBox->nLineHeight = 14;
    pListBox->nScrollBarX = pListBox->nX + pListBox->nWidth - 18;
    pListBox->nScrollBarY = pListBox->nY + 16;
    if (pListBox->nHeight >= 32)
        pListBox->nTrackRows = (pListBox->nHeight - 16) / 12 - 1;
    else
        pListBox->nTrackRows = 1;
    pListBox->nVisibleRows = 1;
    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600 && bWideLayout)
        pListBox->nScrollBarRows = 28;
    else
        pListBox->nScrollBarRows = 18;
    return pListBox;
}


//1.10f: D2Client.0x6FAEFD00
//  Body vtable slot 0, the vector deleting destructor: LISTBOX_UiCtrlBaseDestruct plus a STORM_SMemFree when bit 0 of the flag is set. Returns the
//  object in eax (the D2UnitStrc * return type stands for that).
D2ClientListBoxStrc *__fastcall LISTBOX_ImplVecDelDtor(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, uint8_t bDelete)
{
    LISTBOX_UiCtrlBaseDestruct(pListBox);
    if ((bDelete & 1) != 0 && pListBox)
        STORM_SMemFree(pListBox, szDelete, -1, 0);
    return pListBox;
}


//1.10f: D2Client.0x6FAEFD30
//  The base destructor every UI control chains into: it puts gVtblUiControlBase back and does nothing else. The destructors of the button, the
//  edit box, the spinner and the list box (and their proxies) use it.
void __fastcall LISTBOX_UiCtrlBaseDestruct(void *pControl)
{
    *(const D2UiControlBaseVtbl **)pControl = &gVtblUiControlBase;
}


//1.10f: D2Client.0x6FAEFD40
//  Body vtable slot 13. Returns the word wNumEntries, zero-extended.
int32_t __fastcall LISTBOX_ImplGetNumEntries(D2ClientListBoxStrc *pListBox)
{
    return pListBox->wNumEntries;
}


//1.10f: D2Client.0x6FAEFD50
//  Body vtable slot 14. Zeroes the head of all six columns of every used entry and sets wNumEntries to 0. It does not clear nUserData, nColor or
//  nFont, which is safe only because AddEntry always rewrites them.
//  The cursor starts at column 1 (hence the negative index on the first store) and steps 6151 Unicodes, one entry of 12302 bytes, at a time.
void __fastcall LISTBOX_ImplRemoveAll(D2ClientListBoxStrc *pListBox)
{
    int32_t nEntry;

    for (nEntry = 0; nEntry < (int32_t)pListBox->wNumEntries; ++nEntry)
    {
        D2ClientListBoxEntryStrc *pEntry = &pListBox->entries[nEntry];

        pEntry->wszCol[0][0].ch = 0;
        pEntry->wszCol[1][0].ch = 0;
        pEntry->wszCol[2][0].ch = 0;
        pEntry->wszCol[3][0].ch = 0;
        pEntry->wszCol[4][0].ch = 0;
        pEntry->wszCol[5][0].ch = 0;
    }
    pListBox->wNumEntries = 0;
}


//1.10f: D2Client.0x6FAEFDB0
//  Body vtable slot 12. Recomputes nVisibleRows by summing each entry's real height (nLineHeight per non-empty column plus nEntryGap) until
//  nHeight - 20 is used up, then draws the thumb as three cels: frame 16 for the top cap, frame 14 repeated for the shaft, frame 15 for the tail,
//  each 12 pixels below the last. nThumbTop and nThumbBottom are left behind in the same 12-pixel rows for ScrollByTrackClick to hit-test.
//  The proportion is a 7-bit fixed point: (nVisibleRows << 7) / wNumEntries scaled by nScrollBarRows and shifted back down, so the thumb never
//  quite reaches the bottom when the fraction rounds down. The bar's top inset is 13, the row pitch 12 and the x is nScrollBarX - 1.
//  nThumbTop is nTopEntry * rows / wNumEntries clamped at 0; the clamp cannot fire, since nTopEntry is never negative. The end clamps per arm: a
//  middle page takes the computed end, nThumbTop plus the shifted proportion, and falls back to the full bar only when that end runs past it; the
//  last page computes from the bottom up; the first page starts at 0.
//  BUG: this walk and LISTBOX_CountFittingEntries disagree at the limit: a row is counted here while the running height is below nHeight - 20
//  (jge skips the increment at 0x6FAEFE74) and drawn by ImplDraw while it is at most that (jg breaks at 0x6FAF007A), so a page that ends exactly
//  on the limit draws one row more than nVisibleRows says. Reproduced as the bytes have it.
void __fastcall LISTBOX_ImplUpdateScrollThumb(D2ClientListBoxStrc *pListBox)
{
    uint16_t wNumEntries = pListBox->wNumEntries;
    int32_t nRowsHeight = 0;
    int32_t nTopEntry = pListBox->nTopEntry;
    int32_t nEntriesLeft;
    int32_t nVisibleRows;
    int32_t nThumbFrac;
    int32_t nThumbTop;
    int32_t nThumbEnd;
    int32_t nThumbRows;
    int32_t nThumbBaseY;
    int32_t nXPos;
    int32_t nYPos;
    int32_t nBodyRow;
    int32_t nBodyRows;
    int32_t nAvailHeight;
    D2GfxDataStrc tData;

    pListBox->nThumbTop = 0;
    pListBox->nThumbBottom = 0;
    pListBox->nVisibleRows = 0;
    if (nTopEntry < (int32_t)wNumEntries)
    {
        D2ClientListBoxEntryStrc *pEntry = &pListBox->entries[nTopEntry];

        nAvailHeight = pListBox->nHeight - 20;
        nEntriesLeft = (int32_t)wNumEntries - nTopEntry;
        do
        {
            if (pEntry->wszCol[0][0].ch)
                nRowsHeight += pListBox->nLineHeight;
            if (pEntry->wszCol[1][0].ch)
                nRowsHeight += pListBox->nLineHeight;
            if (pEntry->wszCol[2][0].ch)
                nRowsHeight += pListBox->nLineHeight;
            if (pEntry->wszCol[3][0].ch)
                nRowsHeight += pListBox->nLineHeight;
            if (pEntry->wszCol[4][0].ch)
                nRowsHeight += pListBox->nLineHeight;
            if (pEntry->wszCol[5][0].ch)
                nRowsHeight += pListBox->nLineHeight;
            nRowsHeight += pListBox->nEntryGap;
            if (nRowsHeight < nAvailHeight)
                ++pListBox->nVisibleRows;
            ++pEntry;
            --nEntriesLeft;
        } while (nEntriesLeft);
    }

    nVisibleRows = pListBox->nVisibleRows;
    if ((int32_t)wNumEntries > nVisibleRows)
    {
        nThumbFrac = (nVisibleRows << 7) / (int32_t)wNumEntries;
        if (nTopEntry)
        {
            if (nTopEntry < (int32_t)wNumEntries - nVisibleRows)
            {
                //  A page in the middle: the computed end, clamped to the full bar (cmp edi, ecx / jl keeps the bar only when the end runs past it).
                nThumbRows = pListBox->nScrollBarRows;
                nThumbTop = nTopEntry * nThumbRows / (int32_t)wNumEntries;
                if (nThumbTop < 0)
                    nThumbTop = 0;
                nThumbEnd = nThumbTop + ((uint32_t)(nThumbFrac * nThumbRows) >> 7);
                if (nThumbRows >= nThumbEnd)
                    nThumbRows = nThumbEnd;
            }
            else
            {
                //  The last page: compute from the bottom edge upwards.
                nThumbRows = pListBox->nScrollBarRows;
                nThumbTop = nThumbRows - ((uint32_t)(nThumbFrac * nThumbRows) >> 7);
                if (nThumbRows < nThumbTop)
                    nThumbTop = pListBox->nScrollBarRows;
            }
        }
        else
        {
            nThumbTop = 0;
            nThumbRows = (uint32_t)(nThumbFrac * pListBox->nScrollBarRows) >> 7;
        }

        nXPos = pListBox->nScrollBarX - 1;
        nThumbBaseY = pListBox->nScrollBarY + 13;
        memset(&tData, 0, sizeof(tData));
        tData.pCellFile = gpImgListBoxScrollBar;
        tData.nDirection = 0;
        tData.nFrame = 16;
        D2GFX_TEXTURE_CelDraw(&tData, nXPos, nThumbBaseY + 12 * nThumbTop, 0xFFFFFFFF, 5, nullptr);

        tData.nFrame = 14;
        nBodyRow = nThumbTop;
        if (nThumbTop < nThumbRows)
        {
            nBodyRows = nThumbRows - nThumbTop;
            nYPos = nThumbBaseY + 4 * (3 * nThumbTop + 3);
            do
            {
                D2GFX_TEXTURE_CelDraw(&tData, nXPos, nYPos, 0xFFFFFFFF, 5, nullptr);
                --nBodyRows;
                nYPos += 12;
            } while (nBodyRows);
            nBodyRow = nThumbRows;
        }

        tData.nFrame = 15;
        D2GFX_TEXTURE_CelDraw(&tData, nXPos, nThumbBaseY + 4 * (3 * nBodyRow + 3), 0xFFFFFFFF, 5, nullptr);

        pListBox->nThumbTop = nThumbTop;
        pListBox->nThumbBottom = nThumbRows;
    }
}


//1.10f: D2Client.0x6FAEFFE0
//  How many entries fit in nHeight - 20 starting at nTopEntry, measured the way UpdateScrollThumb measures them. Returns at least 1 when anything
//  remains, and wNumEntries when they all fit. ImplDraw uses it to bound its loop.
//  The walk starts at entry zero and skips the accumulation for indices below nTopEntry; the stride is 6151 Unicodes per row.
int32_t __fastcall LISTBOX_CountFittingEntries(D2ClientListBoxStrc *pListBox)
{
    int32_t nAccumHeight = 0;
    int32_t wNumEntries = (int32_t)pListBox->wNumEntries;
    int32_t nEntry = 0;
    int32_t nTopEntry;
    int32_t nFitting;

    if (!pListBox->wNumEntries)
        return pListBox->wNumEntries;

    nTopEntry = pListBox->nTopEntry;
    for (;;)
    {
        D2ClientListBoxEntryStrc *pEntry = &pListBox->entries[nEntry];

        if (nEntry >= nTopEntry && nEntry < wNumEntries)
        {
            if (pEntry->wszCol[0][0].ch)
                nAccumHeight += pListBox->nLineHeight;
            if (pEntry->wszCol[1][0].ch)
                nAccumHeight += pListBox->nLineHeight;
            if (pEntry->wszCol[2][0].ch)
                nAccumHeight += pListBox->nLineHeight;
            if (pEntry->wszCol[3][0].ch)
                nAccumHeight += pListBox->nLineHeight;
            if (pEntry->wszCol[4][0].ch)
                nAccumHeight += pListBox->nLineHeight;
            if (pEntry->wszCol[5][0].ch)
                nAccumHeight += pListBox->nLineHeight;
            nAccumHeight += pListBox->nEntryGap;
        }
        if (nAccumHeight > pListBox->nHeight - 20)
            break;
        if (++nEntry >= wNumEntries)
            return pListBox->wNumEntries;
    }
    nFitting = nEntry - nTopEntry;
    if (nFitting < 1)
        return 1;
    return nFitting;
}


//1.10f: D2Client.0x6FAF00A0
//  Draws one line of one row through D2Win's text call, into a 1024-element Unicode buffer on the stack (Unicode's constructor runs over all 1024
//  elements, as the original's vector-constructor loop does). The text is copied in and, when it is wider than the control minus 50 pixels,
//  trimmed one character at a time from the tail until it fits. The two draw calls are identical and the branch around them is the original's:
//  the guard is entered when the text is at least the limit, which the inner test narrows to strictly greater.
void __fastcall LISTBOX_DrawColumnText(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, Unicode *pwszText, int32_t nX, int32_t nY, int32_t nColor)
{
    Unicode wszText[1024];
    int32_t nLength;

    Unicode::strcpy(wszText, pwszText);
    if (D2WIN_10123(wszText, Unicode::strlen(wszText)) >= pListBox->nWidth - 50)
    {
        if (D2WIN_10123(wszText, Unicode::strlen(wszText)) > pListBox->nWidth - 50)
        {
            do
            {
                //  BUG: the original's trim loop stores into wszText[-1] once the text is empty (mov [esp+eax*2+0Ah], bx at 0x6FAF0135,
                //  before the width test) and then never ends: an empty string measures 0, which stays above the negative limit nWidth - 50.
                //  LATENT: it needs nWidth < 50; the two shipped boxes are 490 and the screen width minus 20 wide.
                //  DIFFERENCE: ours drops the out-of-bounds store, which is undefined in C, and keeps the loop.
                nLength = Unicode::strlen(wszText);
                if (nLength > 0)
                    wszText[nLength - 1].ch = 0;
            } while (D2WIN_10123(wszText, Unicode::strlen(wszText)) > pListBox->nWidth - 50);
        }
        D2WIN_10117_DrawText(wszText, nX, nY, nColor, 0);
    }
    else
    {
        D2WIN_10117_DrawText(wszText, nX, nY, nColor, 0);
    }
}
typedef int32_t (__thiscall *LISTBOXIMPLMSGFN)(D2ClientListBoxStrc *pListBox, SMSGHANDLER_PARAMS *pMsg);


//1.10f: D2Client.0x6FAF0190
//  Body vtable slot 1, the per-frame draw. A tiled frame around {nX, nY, nX + nWidth - 1, nY + nHeight - 1}, then the scroll bar out of
//  gpImgListBoxScrollBar: cel 9 (11 while the up arrow is held) at nScrollBarY, cel 13 repeated nTrackRows times every 12 pixels, cel 8 (10
//  while the down arrow is held) at nScrollBarY + 12 * nTrackRows. The thumb itself is drawn by slot 12, not here.
//  Then the font is set and the visible page is walked with two counters: nLineRow counts drawn lines (zeroed before the loop, bumped per
//  non-empty column) and nYAccum counts entries. Every line of a row is drawn at x = nX + 32 and
//      y = nLineRow * nLineHeight + nYAccum * nEntryGap + nY + 25
//  The row colour is the entry's own nColor, except that the selected row is forced to 3, and only when pfnOnSelect is set, so a read-only list
//  has no highlight at all. A font of 14 means "use the control's font".
//  The arrow auto-repeat is inverted from the usual shape: while the cursor sits on a held arrow, nHoldTicks is bumped and slot 7 or 8 is called
//  on every value except a multiple of ten (nHoldTicks && nHoldTicks % 10), so it fires on nine frames in ten rather than after a delay.
//  nHoldTicks is unsigned in the original (inc, then xor edx, edx / div ecx).
//  The arrow slots are called with the address of an uninitialised stack slot where a message would go (lea eax, [esp+30h] at 0x6FAF0580); both
//  handlers ignore their argument, and ours casts an int's address to the slot's SMSGHANDLER_PARAMS *.
//  Returns SetFont's answer: the restore of the previous font, the last call on every exit path.
int32_t __fastcall LISTBOX_ImplDraw(D2ClientListBoxStrc *pListBox)
{
    RECT tRect;
    D2GfxDataStrc tData;
    int32_t nPrevFont;
    int32_t nTrackRow;
    int32_t nRowHeight;
    int32_t nVisibleRows;
    int32_t nLineRow;
    int32_t nYAccum;
    int32_t nEntryIndex;
    int32_t nTextColor;
    int32_t nFont;
    int32_t nMousePacked;
    int32_t nScrollBarY;
    int32_t nDownArrowY;
    uint32_t nHoldTicks;
    int32_t nDummyMsg = 0;

    tRect.left = pListBox->nX;
    tRect.top = pListBox->nY;
    tRect.right = pListBox->nX + pListBox->nWidth - 1;
    tRect.bottom = pListBox->nY + pListBox->nHeight - 1;
    UIPANELS_DrawTiledFrame(&tRect);

    nPrevFont = (int32_t)D2WIN_10127_SetFont((enum Font)pListBox->nFont);

    memset(&tData, 0, sizeof(tData));
    tData.pCellFile = gpImgListBoxScrollBar;
    tData.nDirection = 0;
    tData.nFrame = 2 * (pListBox->field_18071C[LBPRESS_UP] != 0) + 9;
    D2GFX_TEXTURE_CelDraw(&tData, pListBox->nScrollBarX, pListBox->nScrollBarY, 0xFFFFFFFF, 5, nullptr);

    nTrackRow = 0;
    if (pListBox->nTrackRows > 0)
    {
        nRowHeight = 12;
        do
        {
            tData.nFrame = 13;
            D2GFX_TEXTURE_CelDraw(&tData, pListBox->nScrollBarX, pListBox->nScrollBarY + nRowHeight, 0xFFFFFFFF, 5, nullptr);
            ++nTrackRow;
            nRowHeight += 12;
        } while (nTrackRow < pListBox->nTrackRows);
    }

    tData.nFrame = 2 * (pListBox->field_18071C[LBPRESS_DOWN] != 0) + 8;
    D2GFX_TEXTURE_CelDraw(&tData, pListBox->nScrollBarX, pListBox->nScrollBarY + 12 * nTrackRow, 0xFFFFFFFF, 5, nullptr);

    nLineRow = 0;
    nVisibleRows = LISTBOX_CountFittingEntries(pListBox);
    nYAccum = 0;
    nEntryIndex = pListBox->nTopEntry;
    if (pListBox->nTopEntry < pListBox->nTopEntry + nVisibleRows)
    {
        //  nTopEntry < nTopEntry + nVisibleRows is the original's own spelling of "the page is not empty"; the second bound is
        //  the array's end and the break at the bottom is the page's.
        for (nEntryIndex = pListBox->nTopEntry; nEntryIndex < LISTBOX_MAX_ENTRIES; )
        {
            D2ClientListBoxEntryStrc *pEntry = &pListBox->entries[nEntryIndex];

            if (pListBox->nSelection == nEntryIndex && pListBox->pfnOnSelect)
                nTextColor = 3;
            else
                nTextColor = pEntry->nColor;
            nFont = pEntry->nFont;
            if (nFont == 14)
                nFont = pListBox->nFont;
            D2WIN_10127_SetFont((enum Font)nFont);

            if (pEntry->wszCol[0][0].ch)
            {
                LISTBOX_DrawColumnText(pListBox, 0, pEntry->wszCol[0], pListBox->nX + 32, nLineRow * pListBox->nLineHeight + nYAccum * pListBox->nEntryGap + pListBox->nY + 25, nTextColor);
                ++nLineRow;
            }
            if (pEntry->wszCol[1][0].ch)
            {
                LISTBOX_DrawColumnText(pListBox, 0, pEntry->wszCol[1], pListBox->nX + 32, nLineRow * pListBox->nLineHeight + nYAccum * pListBox->nEntryGap + pListBox->nY + 25, nTextColor);
                ++nLineRow;
            }
            if (pEntry->wszCol[2][0].ch)
            {
                LISTBOX_DrawColumnText(pListBox, 0, pEntry->wszCol[2], pListBox->nX + 32, nLineRow * pListBox->nLineHeight + nYAccum * pListBox->nEntryGap + pListBox->nY + 25, nTextColor);
                ++nLineRow;
            }
            if (pEntry->wszCol[3][0].ch)
            {
                LISTBOX_DrawColumnText(pListBox, 0, pEntry->wszCol[3], pListBox->nX + 32, nLineRow * pListBox->nLineHeight + nYAccum * pListBox->nEntryGap + pListBox->nY + 25, nTextColor);
                ++nLineRow;
            }
            if (pEntry->wszCol[4][0].ch)
            {
                LISTBOX_DrawColumnText(pListBox, 0, pEntry->wszCol[4], pListBox->nX + 32, nLineRow * pListBox->nLineHeight + nYAccum * pListBox->nEntryGap + pListBox->nY + 25, nTextColor);
                ++nLineRow;
            }
            if (pEntry->wszCol[5][0].ch)
            {
                LISTBOX_DrawColumnText(pListBox, 0, pEntry->wszCol[5], pListBox->nX + 32, nLineRow * pListBox->nLineHeight + nYAccum * pListBox->nEntryGap + pListBox->nY + 25, nTextColor);
                ++nLineRow;
            }

            ++nEntryIndex;
            ++nYAccum;
            if (nEntryIndex >= nVisibleRows + pListBox->nTopEntry)
                break;
        }
    }

    //  Body vtable slot 12: the thumb is drawn by the same pass that recomputes nVisibleRows.
    pListBox->pVtbl->pfnUpdateScrollThumb(pListBox);
    //  Unsigned counter, as in the original (inc), so the wrap stays defined.
    pListBox->nDrawCount = pListBox->nDrawCount + 1;

    //  The cursor, packed the way the original builds it: Y in the high word, X zero-extended in the low.
    nMousePacked = ((uint16_t)CURSOR_GetMouseY() << 16) | (uint16_t)CURSOR_GetMouseX();
    if ((uint16_t)nMousePacked > pListBox->nScrollBarX)
    {
        //  The two bands are tested in order and the second test repeats the first's range -- the original's && chain, kept as it stands so that a
        //  cursor between the bands falls through to the second.
        nScrollBarY = pListBox->nScrollBarY;
        if ((uint16_t)nMousePacked < pListBox->nScrollBarX + 16 && HIWORD(nMousePacked) > nScrollBarY - 12 && HIWORD(nMousePacked) < nScrollBarY && pListBox->field_18071C[LBPRESS_UP] == 1)
        {
            nHoldTicks = pListBox->nHoldTicks + 1;
            pListBox->nHoldTicks = nHoldTicks;
            if (nHoldTicks && nHoldTicks % 10)      //  unsigned remainder (div)
            {
                ((LISTBOXIMPLMSGFN)pListBox->pVtbl->pfnOnArrowUp)(pListBox, (SMSGHANDLER_PARAMS *)&nDummyMsg);
                return (int32_t)D2WIN_10127_SetFont((enum Font)nPrevFont);
            }
        }
        else if ((uint16_t)nMousePacked > pListBox->nScrollBarX && (uint16_t)nMousePacked < pListBox->nScrollBarX + 16)
        {
            nDownArrowY = pListBox->nScrollBarY + 12 * pListBox->nTrackRows;
            if (HIWORD(nMousePacked) > nDownArrowY - 12 && HIWORD(nMousePacked) < nDownArrowY && pListBox->field_18071C[LBPRESS_DOWN] == 1)
            {
                nHoldTicks = pListBox->nHoldTicks + 1;
                pListBox->nHoldTicks = nHoldTicks;
                if (nHoldTicks)
                {
                    if (nHoldTicks % 10)
                    {
                        ((LISTBOXIMPLMSGFN)pListBox->pVtbl->pfnOnArrowDown)(pListBox, (SMSGHANDLER_PARAMS *)&nDummyMsg);
                    }
                }
            }
        }
    }
    return (int32_t)D2WIN_10127_SetFont((enum Font)nPrevFont);
}


//1.10f: D2Client.0x6FAF0610
//  Body vtable slot 15. Appends one row, doing nothing once wNumEntries reaches 128. Six separate Unicode * columns, each optional: a null
//  pointer leaves that column empty. Each is copied with Unicode::strncpy(dst, src, 1023) and NUL-terminated at index 1023, and nStringsStored
//  is bumped once per column copied. The first AddEntry on an empty list moves nSelection from -1 to 0.
//  The row is written at the current wNumEntries, not at +1: the count is bumped by the final store, after nFont.
void __fastcall LISTBOX_ImplAddEntry(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, const Unicode *pwsCol0, const Unicode *pwsCol1, const Unicode *pwsCol2, const Unicode *pwsCol3, const Unicode *pwsCol4, const Unicode *pwsCol5, int32_t nUserData,
                                     int32_t nColor, int32_t bUseDefaultColor, int32_t nFont)
{
    uint16_t wNumEntries = pListBox->wNumEntries;

    if (wNumEntries >= LISTBOX_MAX_ENTRIES)
        return;

    pListBox->entries[wNumEntries].nUserData = nUserData;
    if (pListBox->nSelection == -1)
        pListBox->nSelection = 0;

    if (pwsCol0)
    {
        Unicode::strncpy(pListBox->entries[wNumEntries].wszCol[0], pwsCol0, 1023);
        pListBox->entries[wNumEntries].wszCol[0][1023].ch = 0;
        ++pListBox->nStringsStored;
    }
    if (pwsCol1)
    {
        Unicode::strncpy(pListBox->entries[wNumEntries].wszCol[1], pwsCol1, 1023);
        pListBox->entries[wNumEntries].wszCol[1][1023].ch = 0;
        ++pListBox->nStringsStored;
    }
    if (pwsCol2)
    {
        Unicode::strncpy(pListBox->entries[wNumEntries].wszCol[2], pwsCol2, 1023);
        pListBox->entries[wNumEntries].wszCol[2][1023].ch = 0;
        ++pListBox->nStringsStored;
    }
    if (pwsCol3)
    {
        Unicode::strncpy(pListBox->entries[wNumEntries].wszCol[3], pwsCol3, 1023);
        pListBox->entries[wNumEntries].wszCol[3][1023].ch = 0;
        ++pListBox->nStringsStored;
    }
    if (pwsCol4)
    {
        Unicode::strncpy(pListBox->entries[wNumEntries].wszCol[4], pwsCol4, 1023);
        pListBox->entries[wNumEntries].wszCol[4][1023].ch = 0;
        ++pListBox->nStringsStored;
    }
    if (pwsCol5)
    {
        Unicode::strncpy(pListBox->entries[wNumEntries].wszCol[5], pwsCol5, 1023);
        pListBox->entries[wNumEntries].wszCol[5][1023].ch = 0;
        ++pListBox->nStringsStored;
    }

    if (bUseDefaultColor)
        pListBox->entries[wNumEntries].nColor = pListBox->nDefaultColor;
    else
        pListBox->entries[wNumEntries].nColor = nColor;
    pListBox->entries[wNumEntries].nFont = nFont;
    ++pListBox->wNumEntries;
}


//1.10f: D2Client.0x6FAF08E0
//  Page scroll for a click on the scroll-bar track above or below the thumb, using nThumbTop and nThumbBottom left behind by slot 12. Moves
//  nSelection a whole page, clamps it into range, then syncs nTopEntry. Returns 0 without scrolling when the whole list already fits or the
//  click landed on the thumb itself.
//  The thumb's edges are at nScrollBarY + 12 * nThumbTop + 13 and nScrollBarY + 12 * nThumbBottom + 13. The page-up clamp is branchless:
//  (nSelection - nVisibleRows) & ((<= 0) - 1) is max(0, x); the page-down clamp falls back to wNumEntries - 1.
int32_t __fastcall LISTBOX_ScrollByTrackClick(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nYpos = pMsg->nYpos;
    int32_t nScrollBarY = pListBox->nScrollBarY;
    int32_t nVisibleRows;
    int32_t nPageDown;
    int32_t nLastEntry;

    if (!pListBox->wNumEntries)
        return 0;
    nVisibleRows = pListBox->nVisibleRows;
    if (nVisibleRows > (int32_t)pListBox->wNumEntries)
        return 0;
    if (nYpos - nScrollBarY <= 0)
        return 0;
    if (pListBox->nThumbTop == pListBox->nThumbBottom)
        return 0;

    if (nYpos < nScrollBarY + 12 * pListBox->nThumbTop + 13)
    {
        //  Above the thumb: one page up, clamped at zero without a branch.
        pListBox->nSelection = (pListBox->nSelection - nVisibleRows)
                               & ((pListBox->nSelection - nVisibleRows <= 0) - 1);
    }
    else if (nYpos > nScrollBarY + 12 * pListBox->nThumbBottom + 13)
    {
        //  Below the thumb: one page down, clamped at the last entry and then, near the end, at the last full page.
        nPageDown = nVisibleRows + pListBox->nSelection;
        if (nPageDown >= (int32_t)pListBox->wNumEntries)
            nPageDown = (int32_t)pListBox->wNumEntries - 1;
        pListBox->nSelection = nPageDown;
        if ((int32_t)pListBox->wNumEntries - pListBox->nSelection < nVisibleRows)
        {
            if ((int32_t)pListBox->wNumEntries < nVisibleRows)
                pListBox->nSelection = 0;
            else
                pListBox->nSelection = (int32_t)pListBox->wNumEntries - nVisibleRows;
        }
    }
    else
    {
        //  Between the edges: the click was on the thumb.
        return 0;
    }

    //  The hard clamp at the array size.
    if (pListBox->nSelection >= LISTBOX_MAX_ENTRIES)
    {
        nLastEntry = (int32_t)pListBox->wNumEntries - 1;
        pListBox->nSelection = nLastEntry;
        if (nLastEntry < 0)
            pListBox->nSelection = 0;
    }
    pListBox->nTopEntry = pListBox->nSelection;
    SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
    //  BUG: the page-follow block that follows is the one that is live in ImplOnArrowDown and ImplOnArrowUp, pasted after nTopEntry = nSelection, so
    //  neither test can fire. UNREACHABLE: the second needs nVisibleRows <= 0, and then the function has already returned 0 at the thumb test
    //  (0x6FAF0933). Reproduced as the original has it.
    if (pListBox->nSelection < pListBox->nTopEntry)
        pListBox->nTopEntry = pListBox->nSelection;
    if (pListBox->nSelection >= pListBox->nTopEntry + pListBox->nVisibleRows)
        pListBox->nTopEntry = pListBox->nTopEntry + 1;
    return 1;
}


//1.10f: D2Client.0x6FAF0A30
//  Body vtable slot 2. The wheel selects the edge row it is moving towards and then reuses the arrow handler: wheel up sets nSelection to
//  nTopEntry and calls slot 7; wheel down sets it to nVisibleRows + nTopEntry - 1 and calls slot 8. Both then mark the message handled and break
//  the chain, so the wheel never reaches anything behind the list.
//  The delta is the high word of wParam, read as a signed word: positive is wheel up. The int return is never read; the original returns 1 on
//  every path.
int32_t __fastcall LISTBOX_ImplOnMouseWheel(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    int16_t nDelta = (int16_t)HIWORD(pMsg->nKey);

    if (nDelta > 0)
    {
        pListBox->nSelection = pListBox->nTopEntry;
        ((LISTBOXIMPLMSGFN)pListBox->pVtbl->pfnOnArrowUp)(pListBox, pMsg);
    }
    else if (nDelta < 0)
    {
        pListBox->nSelection = pListBox->nVisibleRows + pListBox->nTopEntry - 1;
        ((LISTBOXIMPLMSGFN)pListBox->pVtbl->pfnOnArrowDown)(pListBox, pMsg);
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
    return 1;
}


//1.10f: D2Client.0x6FAF0A90
//  Body vtable slot 3. Rejects anything outside nX/nY/nWidth/nHeight, then tests the 16-pixel scroll-bar column at nScrollBarX in three bands: the
//  top 12 px sets LBPRESS_UP and clears nHoldTicks, the bottom 12 px (at nScrollBarY + 12 * nTrackRows) sets LBPRESS_DOWN and clears it too, and
//  the strip between them sets LBPRESS_TRACK and records the grab Y. A click anywhere else sets nHoveredEntry from HitTestEntry, but only when
//  pfnOnSelect is set. A click inside the box always sets LBPRESS_ACTIVE first, including one that lands on neither an entry nor the bar.
//  BUG: LBPRESS_GRAB_Y is written and never read: the release path tests LBPRESS_TRACK and pages by the click's Y through ScrollByTrackClick, and
//  no move handler drags, so the scroll thumb cannot be dragged and a press on it does nothing on release. INERT: the only reference to the
//  field (+0x18072C) is the store at 0x6FAF0BAA.
int32_t __fastcall LISTBOX_ImplOnMouseDown(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nMouseX = pMsg->nXpos;
    int32_t nMouseY = pMsg->nYpos;
    int32_t nTrackTop;

    if (nMouseX <= pListBox->nX)
        return 0;
    if (nMouseX >= pListBox->nX + pListBox->nWidth)
        return 0;
    if (nMouseY <= pListBox->nY || nMouseY >= pListBox->nY + pListBox->nHeight)
        return 0;

    pListBox->field_18071C[LBPRESS_ACTIVE] = 1;
    if (nMouseX <= pListBox->nScrollBarX)
    {
        //  Left of the bar: an entry (or nothing).
    }
    else if (nMouseX < pListBox->nScrollBarX + 16)
    {
        if (nMouseY > pListBox->nScrollBarY - 12 && nMouseY < pListBox->nScrollBarY)
        {
            pListBox->field_18071C[LBPRESS_UP] = 1;
            pListBox->nHoldTicks = 0;
            return 1;
        }
        if (nMouseY > pListBox->nScrollBarY + 12 * pListBox->nTrackRows - 12 && nMouseY < pListBox->nScrollBarY + 12 * pListBox->nTrackRows)
        {
            pListBox->field_18071C[LBPRESS_DOWN] = 1;
            pListBox->nHoldTicks = 0;
            return 1;
        }
        nTrackTop = pListBox->nScrollBarY;
        if (nMouseY < nTrackTop + 12 * pListBox->nTrackRows - 12 && nMouseY > nTrackTop)
        {
            pListBox->field_18071C[LBPRESS_GRAB_Y] = nMouseY;
            pListBox->field_18071C[LBPRESS_TRACK] = 1;
            return 1;
        }
    }

    if (pListBox->pfnOnSelect)
        pListBox->nHoveredEntry = LISTBOX_HitTestEntry(pListBox, 0, pMsg);
    return 1;
}


//1.10f: D2Client.0x6FAF0BF0
//  Body vtable slot 4. Releases the three held flags and, for a press that began on a part, fires that part's action only when the release lands
//  on the same part:
//      LBPRESS_UP     released on the up arrow    -> body slot 7
//      LBPRESS_DOWN   released on the down arrow  -> body slot 8
//      LBPRESS_TRACK  released on the track       -> ScrollByTrackClick
//  LBPRESS_ACTIVE was already cleared, so a release with no press behind it only resets the hover. A release outside the part that was held
//  clears its flag and does nothing else.
//  The entry-select path is stricter than a click: pfnOnSelect fires only when HitTestEntry says the release is on the same entry nHoveredEntry
//  recorded on the press, and only when that entry is not -1. The return is real: DIALOG_OnMouseUp stops at the first control that answers 1.
//  This answers 1 when no press was active (after resetting the hover) and when pfnOnSelect fired, and 0 on every other path, the arrow and
//  track releases included.
//  pfnOnSelect is called with nothing pushed and ecx still holding the entry index it was compared against (0x6FAF0D68); the one callback the
//  game registers, NPCMENU_OnHireListConfirmClicked, does not read ecx as an argument, so the plain __cdecl call below loses nothing.
//  BUG: the release is hit-tested wherever it lands: nothing here tests the box before calling HitTestEntry, and DIALOG_OnMouseUp hands every
//  release to every control, so a press on the rows that is released below the list stores a selection below the drawn page. REACHABLE: one
//  of the two ways into the live second test of LISTBOX_ImplOnArrowUp (see LISTBOX_HitTestEntry).
int32_t __fastcall LISTBOX_ImplOnMouseUp(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nPressedPart;
    int32_t nHitEntry;

    if (!pListBox->field_18071C[LBPRESS_ACTIVE])
    {
        pListBox->nHoveredEntry = -1;
        return 1;
    }
    nPressedPart = pListBox->field_18071C[LBPRESS_UP];
    pListBox->field_18071C[LBPRESS_ACTIVE] = 0;

    if (nPressedPart == 1)
    {
        pListBox->field_18071C[LBPRESS_UP] = 0;
        if (pMsg->nXpos > pListBox->nScrollBarX && pMsg->nXpos < pListBox->nScrollBarX + 16)
        {
            if (pMsg->nYpos > pListBox->nScrollBarY - 12 && pMsg->nYpos < pListBox->nScrollBarY)
            {
                ((LISTBOXIMPLMSGFN)pListBox->pVtbl->pfnOnArrowUp)(pListBox, pMsg);
            }
        }
        pListBox->nHoveredEntry = -1;
        return 0;
    }

    if (pListBox->field_18071C[LBPRESS_DOWN] == 1)
    {
        int32_t nDownArrowY;

        pListBox->field_18071C[LBPRESS_DOWN] = 0;
        nDownArrowY = pListBox->nScrollBarY + 12 * pListBox->nTrackRows;
        if (pMsg->nXpos > pListBox->nScrollBarX && pMsg->nXpos < pListBox->nScrollBarX + 16 && pMsg->nYpos > nDownArrowY - 12 && pMsg->nYpos < nDownArrowY)
        {
            ((LISTBOXIMPLMSGFN)pListBox->pVtbl->pfnOnArrowDown)(pListBox, pMsg);
            pListBox->nHoveredEntry = -1;
            return 0;
        }
        pListBox->nHoveredEntry = -1;
        return 0;
    }

    if (pListBox->field_18071C[LBPRESS_TRACK] == 1)
    {
        pListBox->field_18071C[LBPRESS_TRACK] = 0;
        if (pMsg->nXpos > pListBox->nScrollBarX && pMsg->nXpos < pListBox->nScrollBarX + 16)
        {
            if (pMsg->nYpos < pListBox->nScrollBarY + 12 * pListBox->nTrackRows - 12 && pMsg->nYpos > pListBox->nScrollBarY)
            {
                LISTBOX_ScrollByTrackClick(pListBox, 0, pMsg);
            }
        }
        pListBox->nHoveredEntry = -1;
        return 0;
    }

    if (!pListBox->pfnOnSelect)
        return 0;
    nHitEntry = LISTBOX_HitTestEntry(pListBox, 0, pMsg);
    if (pListBox->nHoveredEntry != nHitEntry || pListBox->nHoveredEntry == -1)
        return 0;
    pListBox->nHoveredEntry = -1;
    pListBox->pfnOnSelect();
    return 1;
}


//1.10f: D2Client.0x6FAF0DA0
//  Body vtable slot 7. Without a pfnOnSelect the list has no cursor, so the selection is first snapped to nTopEntry. Decrements nSelection,
//  plays sound 1, and drags nTopEntry down when the selection moved above the window. The int return is 1 on every path, the early exit at
//  selection 0 included; the body never reads the message.
//  The second adjustment needs the new selection at or past nTopEntry + nVisibleRows, so the old one at least two rows under the last counted
//  row; LISTBOX_HitTestEntry has no page bound, so a press on the rows released below the list selects a row the draw never shows (see
//  LISTBOX_ImplOnMouseUp). In the hire list -- rows of 34 px, seven drawn -- a release more than 284 and less than 312 px under the list's top
//  selects row nTopEntry + 8; a press on the up arrow then moves the selection to nTopEntry + 7 and this test, now true, moves the top one row
//  down. The message log cannot get there: it has no pfnOnSelect, so the selection is snapped to nTopEntry first and the step up always lands
//  above the top. Reproduced as the original has it.
int32_t __fastcall LISTBOX_ImplOnArrowUp(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nNewSelection;

    if (!pListBox->pfnOnSelect)
        pListBox->nSelection = pListBox->nTopEntry;
    if (pListBox->nSelection <= 0)
        return 1;
    pListBox->nSelection = pListBox->nSelection - 1;
    SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
    nNewSelection = pListBox->nSelection;
    if (nNewSelection < pListBox->nTopEntry)
        pListBox->nTopEntry = nNewSelection;
    //  REACHABLE: the selection was two or more rows under the page (see the head of this function).
    if (!(nNewSelection < pListBox->nTopEntry + pListBox->nVisibleRows))
        pListBox->nTopEntry = pListBox->nTopEntry + 1;
    return 1;
}


//1.10f: D2Client.0x6FAF0E20
//  Body vtable slot 8. Mirror of OnArrowUp, with two differences. Without a pfnOnSelect the selection is snapped to the last visible row,
//  nVisibleRows + nTopEntry - 1 clamped to wNumEntries - 1, before moving down; a list with a callback moves from wherever the selection already
//  is. The scroll sync below is the live copy of the block that is dead in ScrollByTrackClick: nTopEntry has not been forced equal to
//  nSelection, so both tests can fire, the first when the selection moved above the window and the second when it moved below it.
int32_t __fastcall LISTBOX_ImplOnArrowDown(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nLastVisibleRow;
    int32_t nSelectionAfter;

    if (!pListBox->pfnOnSelect)
    {
        nLastVisibleRow = pListBox->nVisibleRows + pListBox->nTopEntry - 1;
        pListBox->nSelection = nLastVisibleRow;
        if (nLastVisibleRow >= (int32_t)pListBox->wNumEntries)
            pListBox->nSelection = (int32_t)pListBox->wNumEntries - 1;
    }
    if (pListBox->nSelection < (int32_t)pListBox->wNumEntries - 1)
    {
        pListBox->nSelection = pListBox->nSelection + 1;
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
        nSelectionAfter = pListBox->nSelection;
        if (nSelectionAfter < pListBox->nTopEntry)
            pListBox->nTopEntry = nSelectionAfter;
        if (pListBox->nSelection >= pListBox->nTopEntry + pListBox->nVisibleRows)
            pListBox->nTopEntry = pListBox->nTopEntry + 1;
    }
    return 1;
}


//1.10f: D2Client.0x6FAF0ED0
//  Maps a mouse position to an entry index, or -1. Walks from nTopEntry accumulating the same variable row height the draw uses, so it stays
//  correct for rows with different numbers of filled lines. Returns -1 immediately, without touching nSelection, for any x past
//  nScrollBarX - 15 (jg at 0x6FAF0EE9) and for a list with no pfnOnSelect.
//  The answer is stored in nSelection as well as returned, on hit and on miss: the down handler records it as nHoveredEntry and the up handler
//  compares the release against it, which is how the press-and-release contract is built.
//  The hit test is mouseY strictly inside (nY + start + 12, nY + end + 12), where end - start is the row's lines times nLineHeight and the gap is
//  added only after the test: a band as tall as the row's text, shifted 12 pixels down from the row's top. Column 0's emptiness is tested
//  through a cursor that starts at column 1 and indexes back 1024 wide characters.
//  BUG: the walk has no page bound: it runs to wNumEntries, while ImplDraw draws only the rows LISTBOX_CountFittingEntries lets through, so a row
//  past the page answers a click on the empty space where it would have been drawn. REACHABLE: in the hire list (nHeight 280, rows of 34 px,
//  seven drawn) row nTopEntry + 7 answers in (nY + 250, nY + 278), inside the box under the last drawn row, and a press and release there fire
//  pfnOnSelect for a mercenary the player was never shown (NPCMENU_OnHireListConfirmClicked reads this selection back through slot 11 and hires
//  that offer, or asks to). A release outside the box, which OnMouseUp hit-tests just the same, stores rows further down still: the way into
//  LISTBOX_ImplOnArrowUp's live second test.
int32_t __fastcall LISTBOX_HitTestEntry(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nEntryIndex;
    int32_t nVisibleIndex;
    int32_t nHitIndex;
    int32_t nMouseY;

    if (pMsg->nXpos > pListBox->nScrollBarX - 15 || !pListBox->pfnOnSelect)
        return -1;

    nEntryIndex = pListBox->nTopEntry;
    nMouseY = pMsg->nYpos;
    nVisibleIndex = 0;
    if (nEntryIndex >= (int32_t)pListBox->wNumEntries)
    {
        nEntryIndex = -1;
    }
    else
    {
        D2ClientListBoxEntryStrc *pEntry = &pListBox->entries[nEntryIndex];

        for (;;)
        {
            nHitIndex = nVisibleIndex;
            if (pEntry->wszCol[0][0].ch)
                nVisibleIndex += pListBox->nLineHeight;
            if (pEntry->wszCol[1][0].ch)
                nVisibleIndex += pListBox->nLineHeight;
            if (pEntry->wszCol[2][0].ch)
                nVisibleIndex += pListBox->nLineHeight;
            if (pEntry->wszCol[3][0].ch)
                nVisibleIndex += pListBox->nLineHeight;
            if (pEntry->wszCol[4][0].ch)
                nVisibleIndex += pListBox->nLineHeight;
            if (pEntry->wszCol[5][0].ch)
                nVisibleIndex += pListBox->nLineHeight;

            if (nMouseY > pListBox->nY + nHitIndex + 12 && nMouseY < pListBox->nY + nVisibleIndex + 12)
                break;

            ++pEntry;
            nVisibleIndex += pListBox->nEntryGap;
            if (++nEntryIndex >= (int32_t)pListBox->wNumEntries)
            {
                nEntryIndex = -1;
                break;
            }
        }
    }
    pListBox->nSelection = nEntryIndex;
    return nEntryIndex;
}


//1.10f: D2Client.0x6FAF0FF0
//  Body vtable slot 5. Inside the box, updates the hover entry through HitTestEntry; returns 0 either way, so a move is never consumed. Unlike the
//  mouse handlers it guard-tests the box and returns 0 outside it instead of falling through to a hit test.
int32_t __fastcall LISTBOX_ImplOnMouseMove(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    if (pMsg->nXpos > pListBox->nX && pMsg->nXpos < pListBox->nX + pListBox->nWidth)
    {
        if (pMsg->nYpos > pListBox->nY && pMsg->nYpos < pListBox->nY + pListBox->nHeight)
        {
            LISTBOX_HitTestEntry(pListBox, 0, pMsg);
        }
    }
    return 0;
}


//1.10f: D2Client.0x6FAF1040
//  Body vtable slot 6. A list with no pfnOnSelect swallows nothing. Otherwise the message is consumed and the chain broken first, then Enter
//  with a selection fires the callback, and only when nDrawCount has reached 2, so a key that arrives before the control has been drawn twice is
//  consumed but does nothing. The gate stops the callback, not the chain break, and it is unsigned in the original (cmp ..., 2 / jb at 0x6FAF1066).
//  pfnOnSelect is called here with ecx whatever SMsgBreakHandlerChain left in it.
int32_t __fastcall LISTBOX_ImplOnChar(D2ClientListBoxStrc *pListBox, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    if (!pListBox->pfnOnSelect)
        return 0;

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    if (pListBox->nDrawCount < 2)
        return 0;
    if (pListBox->nSelection != -1 && pMsg->wParam == VK_RETURN)
        pListBox->pfnOnSelect();
    return 1;
}


//1.10f: D2Client.0x6FAF10A0
//  Body vtable slot 11. Returns nSelection, which is -1 when nothing is selected.
int32_t __fastcall LISTBOX_ImplGetSelection(D2ClientListBoxStrc *pListBox)
{
    return pListBox->nSelection;
}


//1.10f: D2Client.0x6FAF10B0
//  Proxy destructor: reinstalls gVtblListBox, destroys the body through its own slot 0 with the delete flag set, then drops to
//  gVtblUiControlBase. The original inlines LISTBOX_UiCtrlBaseDestruct for that last step. The body is read as a pointer to its vtable, three
//  levels deep, because (**ppBody)(ppBody, 1) dereferences twice to reach slot 0.
//  DIFFERENCE: the original's prologue registers an exception-handling frame whose one unwind action runs LISTBOX_UiCtrlBaseDestruct on the proxy
//  if the body's destructor throws; ours has none.
//  void: eax is the body destructor's answer, or on a null body the old exception-list head (fs:0, 0x6FAF10B7); no caller reads it.
void __fastcall LISTBOX_ProxyDestruct(D2UiCtrlProxyStrc *pProxy, int32_t nEdxUnused)
{
    D2ClientListBoxStrc *pBody;

    pProxy->pListBoxVtbl = &gVtblListBox;
    pBody = pProxy->pListBox;
    if (pBody)
        ((D2ClientListBoxStrc *(__thiscall *)(D2ClientListBoxStrc *, uint8_t))pBody->pVtbl->pfnDeletingDtor)(pBody, 1);
    pProxy->pBaseVtbl = &gVtblUiControlBase;
}


//1.10f: D2Client.0x6FAF1100
//  Proxy vtable slot 0: LISTBOX_ProxyDestruct plus a STORM_SMemFree when bit 0 of the flag is set (the compiler's delete convention: the object
//  was heap-allocated, free it).
D2UiCtrlProxyStrc *__fastcall LISTBOX_ProxyVecDelDtor(D2UiCtrlProxyStrc *pProxy, int32_t nEdxUnused, uint8_t bFreeObject)
{
    LISTBOX_ProxyDestruct(pProxy, 0);
    if ((bFreeObject & 1) != 0 && pProxy)
        STORM_SMemFree(pProxy, szDelete, -1, 0);
    return pProxy;
}


//1.10f: D2Client.0x6FAF1130
//  The list box proxy constructor, the 12-byte object dialogs hold: +0x00 vtable (gVtblListBox), +0x04 unused, +0x08 pImpl. Installs the vtable,
//  allocates 1574774 bytes for the body, runs LISTBOX_ImplConstruct on it and stores the result, a null when the allocation failed. Takes nine
//  stack arguments after this (retn 24h).
//  DIFFERENCE: the original's prologue registers an exception-handling frame with two unwind actions: state 1, around ImplConstruct, frees the
//  block with the CRT's operator delete (not with Storm's SMemFree, which matches the SMemAlloc), and state 0 runs LISTBOX_UiCtrlBaseDestruct on
//  the proxy. Neither runs unless ImplConstruct throws; ours has no frame.
D2UiCtrlProxyStrc *__fastcall LISTBOX_Construct(D2UiCtrlProxyStrc *pProxy, int32_t nEdxUnused, int32_t nX, int32_t nY, int32_t nWidth, int32_t nHeight, int32_t nEntryGap, int32_t nDefaultColor,
                                                 int32_t nFont, int32_t (__cdecl *pfnOnSelect)(), uint8_t bWideLayout)
{
    D2ClientListBoxStrc *pBody;
    D2ClientListBoxStrc *pConstructed;

    pProxy->pListBoxVtbl = &gVtblListBox;
    pBody = (D2ClientListBoxStrc *)STORM_SMemAlloc(0x180776, szNew, -1, 0);
    if (pBody)
    {
        pConstructed = LISTBOX_ImplConstruct(pBody, 0, nX, nY, nWidth, nHeight, nEntryGap, nDefaultColor, nFont, pfnOnSelect, bWideLayout);
    }
    else
    {
        pConstructed = nullptr;
    }
    pProxy->pListBox = pConstructed;
    return pProxy;
}


//1.10f: D2Client.0x6FAF11D0
//  Forwards to the wrapped control's slot 1, Draw. A tail jump (mov ecx, [ecx+8] / mov eax, [ecx] / jmp dword ptr [eax+4]) with the body as
//  this and no stack argument. The body pointer is not tested, here or in the other two forwarders below: the failure arm of LISTBOX_Construct
//  cannot run, because Storm's SMemAlloc never returns NULL (it reports and exits), so the untested dereference cannot fault through it.
int32_t __fastcall LISTBOX_UiCtrlProxyFwdSlot1(D2UiCtrlProxyStrc *pProxy)
{
    return (*(int32_t (__thiscall *const *const *)(void *))pProxy->pImpl)[1](pProxy->pImpl);
}


//1.10f: D2Client.0x6FAF11E0
//  Proxy vtable slot 13, the only proxy slot that is real code rather than a forwarder: it reaches into the body's entry array itself and returns
//  entry[i].nUserData, the value AddEntry stored at +0x3002. 12302 * i + 12298 is the entry's last dword, and wNumEntries sits at +1574728, 72
//  bytes past the 128-entry array. The parameter is nEntryIndex rather than the game's Selection because the body already has an nSelection
//  field meaning the highlighted row.
//  BUG: the three asserts run for every index the fast path refuses, and the middle one is inverted. The fast path returns only for
//  0 <= nEntryIndex < wNumEntries (and < 128). The first assert fires while the list is full. The second is the game's own
//  "mNumEntries < Selection", tested as cmp edx, ecx / jl at 0x6FAF1249, the wrong way round for a bounds check: it fires for
//  nEntryIndex == wNumEntries and for every negative index, and lets an index past wNumEntries through to return 0. So the third,
//  Selection >= 0, can never fire. LATENT: the one caller passes the list's own selection, which is a real row whenever the callback runs. They
//  are reproduced as found.
int32_t __fastcall LISTBOX_GetEntryUserData(D2UiCtrlProxyStrc *pProxy, int32_t nEdxUnused, int32_t nEntryIndex)
{
    D2ClientListBoxStrc *pBody = pProxy->pListBox;

    if (nEntryIndex >= 0 && nEntryIndex < (int32_t)pBody->wNumEntries && nEntryIndex < LISTBOX_MAX_ENTRIES)
    {
        return pBody->entries[nEntryIndex].nUserData;
    }
    if ((int32_t)pBody->wNumEntries >= LISTBOX_MAX_ENTRIES)
    {
        FOG_DisplayAssert("mNumEntries < LISTBOX_MAX_ENTRIES", szListBoxCpp, 1132);
        _exit(-1);
    }
    if ((int32_t)pBody->wNumEntries >= nEntryIndex)
    {
        FOG_DisplayAssert("mNumEntries < Selection", szListBoxCpp, 1133);
        _exit(-1);
    }
    if (nEntryIndex < 0)
    {
        FOG_DisplayAssert("Selection >= 0", szListBoxCpp, 1134);
        _exit(-1);
    }
    return 0;
}


//1.10f: D2Client.0x6FAF12A0
//  Forwards to the wrapped control's slot 13, the list box's GetNumEntries. A tail jump (jmp dword ptr [eax+34h]) with no stack argument.
int32_t __fastcall LISTBOX_UiCtrlProxyFwdSlot13(D2UiCtrlProxyStrc *pProxy)
{
    return (*(int32_t (__thiscall *const *const *)(void *))pProxy->pImpl)[13](pProxy->pImpl);
}


//1.10f: D2Client.0x6FAF12B0
//  Forwards to the wrapped control's slot 2, the mouse wheel. A tail jump (jmp dword ptr [eax+8]), so the message the caller pushed stays on the
//  stack and the target's retn 4 pops it; the C passes pMsg on for that reason.
int32_t __fastcall LISTBOX_UiCtrlProxyFwdSlot2(D2UiCtrlProxyStrc *pProxy, int32_t nEdxUnused, SMSGHANDLER_PARAMS *pMsg)
{
    return (*(int32_t (__thiscall *const *const *)(void *, SMSGHANDLER_PARAMS *))pProxy->pImpl)[2](pProxy->pImpl, pMsg);
}

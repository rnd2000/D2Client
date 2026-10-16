#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "CoreArchive.h"
#include "config.h"

//  config.cpp -- the Configure Controls screen of D2Client.dll 1.10, the key-binding page that the ESC menu's Configure Controls entry opens
//  ("Config" here is this screen, not D2ConfigStrc, the launch configuration): the three button callbacks, the setup, the list selector, the button
//  layout, the two list movers, the panel's enter/leave hook, the panel draw, the mouse model and its two hit tests, and the keyboard and capture
//  handlers of gConfigMessageHandlers with their tables.
//  No assert in this unit names the original's source file, so the file name is ours. Address range 0x6FAFC260..0x6FAFE36E, thirty functions;
//  the compiler's static initialiser of the unit (CRT_StaticInit12, 0x6FAFC240) is not rewritten.

static const char szDataGlobal[] = "DATA\\GLOBAL";


//  The font id 0x0D passed to D2WIN_10127_SetFont is D2FONT_FONTINGAMECHAT, asserted to be 13 in text.cpp and not asserted a second time here.
static_assert(offsetof(D2ClientCfgControlsButtonStrc, nHoverColor) == 0x06
              && offsetof(D2ClientCfgControlsButtonStrc, nLeft) == 0x0A
              && offsetof(D2ClientCfgControlsButtonStrc, nRight) == 0x0E
              && offsetof(D2ClientCfgControlsButtonStrc, nTop) == 0x12
              && offsetof(D2ClientCfgControlsButtonStrc, nBottom) == 0x16,
              "the button record is {left, right, top, bottom}, NOT a RECT's "
              "order: HitTestButton compares x against +0x0A and +0x0E and y "
              "against +0x12 and +0x16, and the layout stores right, left, "
              "top, bottom at +0x0E, +0x0A, +0x12, +0x16");


//1.10f: D2Client.0x6FAFC260
//  The Default button (record 1 of gCfgControlsButtons): restores the MPQ's default bindings by passing 0, CMDTBL_CommandLoadDefaultKeyBindings'
//  bTryDiskFile, so a default.key on disk is never read. The callee's __int16 comes back through the jump and nobody reads it: all three button
//  callbacks are void, because the dispatcher, CONFIG_OnMouseUp, never reads what they leave in eax.
void __cdecl CONFIG_OnDefaultButton(void)
{
    CMDTBL_CommandLoadDefaultKeyBindings(NULL);
}


//1.10f: D2Client.0x6FAFC270
//  The Accept button (record 2): CMDTBL_CommitBindings writes the edited bindings to the character's key file and to default.key beside the game
//  and re-runs the mouse mapping, then the panel closes and the ESC menu with it, turning back on the panels the menu hid
//  (ESCMENU_CloseMenu(TRUE)). A tail jump in the original.
void __cdecl CONFIG_OnAcceptButton(void)
{
    CMDTBL_CommitBindings();
    UI_SetUIToggle(UI_CONFIG, UI_TURNOFF, 0);
    ESCMENU_CloseMenu(1);
}


//1.10f: D2Client.0x6FAFC290
//  The Cancel button (record 0, the leftmost): puts back the bindings snapshot that CONFIG_SuspendKeyCommands took when the panel opened, closes
//  the panel and goes back to the ESC menu's page 1 without closing its windows (the 0). A tail jump in the original. CONFIG_OnDismissKey calls it.
void __cdecl CONFIG_OnCancelButton(void)
{
    CMDTBL_RestoreBindingsSnapshot();
    UI_SetUIToggle(UI_CONFIG, UI_TURNOFF, 0);
    ESCMENU_SetPage(0, 1);
}


//1.10f: D2Client.0x6FAFC2C0
//  Once a game, as the UI comes up (the tail of UI_InitUiSubsystems): picks the list, loads TextSlid, lays the buttons out and puts the list at its
//  top. The two row globals are zeroed after the layout, whose tail call to ScrollList(0) therefore places the thumb for the previous first row;
//  the row is 0 at that moment anyway. `sub esp, 104h` is a 260-byte path buffer.
void __cdecl CONFIG_InitControlsScreen(void)
{
    char szPath[260];

    static_assert(sizeof(szPath) == 0x104,
                  "the frame is `sub esp, 104h` and the path is all of it");

    CONFIG_SelectControlsList();
    sprintf(szPath, "%s\\UI\\Menu\\TextSlid", szDataGlobal);
    gpImgCfgTextSlider = ARCHIVE_CellFileLoad(szPath, 0);
    CONFIG_LayoutControlsButtons();
    gnCfgFirstVisibleRow = 0;
    gnCfgSelectedRow = 0;
}


//1.10f: D2Client.0x6FAFC310
//  Picks the classic (51 rows) or the expansion (62 rows) row list and sets the numbers the scrollbar needs: the thumb height in 12-pixel cells (6
//  and 5), the last first row (36 and 47) and the row count. The original computes them from a register loaded with 18: rows on a page 18 - 3 = 15,
//  track 18 * 18 - 78 = 246 pixels, thumb (246 / 12 + 1) * 15 / rows, last first row rows - (18 - 3). The C writes that arithmetic with the 18 in a
//  local; the compiler's duplicated tail in the two arms is one tail here.
void __cdecl CONFIG_SelectControlsList(void)
{
    int32_t nPanelTop;
    int32_t nBase;

    if (GAME_IsExpansionGame())
    {
        gpCfgControlsList = gCfgControlsListExpansion;
        gnCfgControlsRowCount = 62;
    }
    else
    {
        gpCfgControlsList = gCfgControlsList;
        gnCfgControlsRowCount = 51;
    }

    //  computed and never read, as in the original
    nPanelTop = (gnViewportBottom - 420) / 2;
    (void)nPanelTop;

    nBase = 18;
    gnCfgScrollThumbRows = ((18 * nBase - 78) / 12 + 1) * (nBase - 3) / (int32_t)gnCfgControlsRowCount;
    gnCfgMaxFirstVisibleRow = (int32_t)gnCfgControlsRowCount - (nBase - 3);
}


//1.10f: D2Client.0x6FAFC3F0
//  Lays out the three button boxes: record i is centred in the i-th third of a 620-pixel panel and made its label's width plus 10 on either side;
//  all three share the panel's rows +329 .. +367. Then ScrollList(0), as a tail call. The column width is an unsigned division (`mul` by 0AAAAAAABh
//  at 0x6FAFC43C) and its half a signed one (`cdq / sar` at 0x6FAFC447), the shape of dividing by the table's element count; the operand is 620 or
//  621 whatever the screen width, so the two never differ. The loop compares its byte offset against 78 with `jb`, which is `i < 3` unsigned.
void __cdecl CONFIG_LayoutControlsButtons(void)
{
    enum Font nPrevFont;
    uint32_t i;
    int32_t nTextWidth;
    int32_t nMargin;
    uint32_t nColumnWidth;
    int32_t nCentre;
    int32_t nHalfText;
    int32_t nPanelTop;

    nPrevFont = D2WIN_10127_SetFont(D2FONT_FONTINGAMECHAT);
    for (i = 0; i < 3; ++i)
    {
        nTextWidth = D2WIN_10121_GetTextWidth(D2LANG_GetStringFromTblIndex(gCfgControlsButtons[i].nLabelStrId));

        nMargin = (gnScreenWidth - 620) / 2;
        nColumnWidth = (uint32_t)(gnScreenWidth - 2 * nMargin) / 3u;
        nCentre = nMargin + (int32_t)(nColumnWidth * i) + (int32_t)nColumnWidth / 2;
        nHalfText = nTextWidth / 2;

        gCfgControlsButtons[i].nRight = nHalfText + nCentre + 10;
        gCfgControlsButtons[i].nLeft = nCentre - nHalfText - 10;
        nPanelTop = (gnViewportBottom - 420) / 2;
        gCfgControlsButtons[i].nTop = nPanelTop + 329;
        gCfgControlsButtons[i].nBottom = nPanelTop + 367;
    }
    D2WIN_10127_SetFont(nPrevFont);

    CONFIG_ScrollList(0);
}


//1.10f: D2Client.0x6FAFC4C0
//  Moves the first visible row by nDelta and places the thumb; a delta of 0 only re-places the thumb. Beyond +-1 it recurses one step at a time
//  (the redundant test each such arm opens with is the entry test of the `for` that counts the steps). A step clamps `row > 0 ? row - 1 : row`, and
//  its mirror against the upper bound, and stores only an in-range result. The thumb sits at the panel's top + 47 plus the travel the first row has
//  covered:
//  (246 - 12 * thumb cells) pixels of track, in proportion to the first row over the last first row.
void __fastcall CONFIG_ScrollList(int32_t nDelta)
{
    int32_t i;
    int32_t nMaxFirstRow;
    int32_t nPanelTop;
    int32_t nBase;
    int32_t nFirstRow;
    int32_t nNewFirstRow;

    if (nDelta < -1)
    {
        for (i = 0; i < -nDelta; ++i)
            CONFIG_ScrollList(-1);
        return;
    }
    if (nDelta > 1)
    {
        for (i = 0; i < nDelta; ++i)
            CONFIG_ScrollList(1);
        return;
    }

    nMaxFirstRow = (int32_t)gnCfgControlsRowCount;
    nPanelTop = (gnViewportBottom - 420) / 2;
    nBase = 18;
    for (i = 0; i < nBase - 3; ++i)
    {
        if (--nMaxFirstRow <= 0)
        {
            nMaxFirstRow = 0;
            break;
        }
    }

    nFirstRow = (int32_t)gnCfgFirstVisibleRow;
    if (nDelta < 0)
    {
        nNewFirstRow = nFirstRow > 0 ? nFirstRow - 1 : nFirstRow;
        if (nNewFirstRow >= 0)
        {
            nFirstRow = nNewFirstRow;
            gnCfgFirstVisibleRow = nFirstRow;
        }
    }
    else if (nDelta > 0)
    {
        nNewFirstRow = nFirstRow < nMaxFirstRow ? nFirstRow + 1 : nFirstRow;
        if (nNewFirstRow <= nMaxFirstRow)
        {
            nFirstRow = nNewFirstRow;
            gnCfgFirstVisibleRow = nFirstRow;
        }
    }

    //  BUG: nMaxFirstRow is 0 for a list of fifteen rows or fewer, and this is the `idiv esi` at 0x6FAFC57E, a divide error on the first call.
    //  UNREACHABLE: the row count is stored only by SelectControlsList, as 62 or 51, and every path into ScrollList runs it first.
    gnCfgScrollThumbTop = (18 * nBase - 12 * (int32_t)gnCfgScrollThumbRows - 78) * nFirstRow / nMaxFirstRow + nPanelTop + 47;
}


//1.10f: D2Client.0x6FAFC590
//  Once a game, as the UI goes down (the tail of UI_ShutdownUiSubsystems): unloads the TextSlid image.
void __cdecl CONFIG_UnloadTextSliderImage(void)
{
    if (gpImgCfgTextSlider)
    {
        ARCHIVE_CellFileFree(gpImgCfgTextSlider);
        gpImgCfgTextSlider = NULL;
    }
}


//1.10f: D2Client.0x6FAFC5B0
//  The enter/leave hook of UI_CONFIG (bSuspend = 1 when the panel opens, 0 when it closes), called from UI_RegisterPanelHandlers and
//  UI_UnregisterPanelHandlers. Opening takes the bindings snapshot, turns the key hooks off, drops the mouse input handlers and the selected unit,
//  and resets the list to its top; closing puts the hooks and the handlers back. While gbCfgCapturingBind is set only the reset runs: a capture
//  starts by calling UI_UnregisterPanelHandlers(UI_CONFIG) with the flag already set, and unflagged this would switch the game's key hooks and
//  mouse input back on for the length of the capture. A capture ends through UI_ConfigRegisterPanelHandlers, which registers the panel's tables
//  itself and does not come through here. gbCfgCapturingBind is loaded once, after SelectControlsList and before the branch; LayoutControlsButtons
//  is a tail jump from both arms.
void __fastcall CONFIG_SuspendKeyCommands(int32_t bSuspend)
{
    CONFIG_SelectControlsList();

    if (bSuspend)
    {
        if (!gbCfgCapturingBind)
        {
            CMDTBL_SaveBindingsSnapshot();
            CMDTBL_SetKeyHookMode(0, 0);
            GAME_UnregisterMouseInputHandlers();
            SELECT_SetSelectedUnit(NULL);
        }
        gnCfgFirstVisibleRow = 0;
        gnCfgSelectedRow = 0;
    }
    else if (!gbCfgCapturingBind)
    {
        CMDTBL_SetKeyHookMode(1, 0);
        GAME_RegisterMouseInputHandlers();
        SELECT_ClearSelectedUnit();
    }

    CONFIG_LayoutControlsButtons();
}


//1.10f: D2Client.0x6FAFD470
//  Moves the selected row by nDelta and keeps it on the page; a delta of 0 does nothing. Beyond +-1 it recurses a step at a time with the same
//  clamp as ScrollList. The downward bound is exact: from row count - 2 the candidate is count - 1 and the store passes, so the selection can reach
//  the last row; from count - 1 the candidate is count and the store is skipped. The page walk: above the first visible row it makes that row the
//  first and lets ScrollList(0) replace the thumb; otherwise it counts up to fifteen rows from the first visible one and scrolls by one until the
//  selection is inside (the row count is loaded on entry and only the first row is re-read after each scroll). A selection at or past the row count
//  would loop forever; nothing stores one (the store needs < count and HitTestRow answers count - 1 at most).
void __fastcall CONFIG_MoveSelection(int32_t nDelta)
{
    int32_t i;
    int32_t nRowCount;
    int32_t nRow;
    int32_t nNewRow;
    int32_t nFirstRow;

    if (!nDelta)
        return;

    if (nDelta < -1)
    {
        for (i = 0; i < -nDelta; ++i)
            CONFIG_MoveSelection(-1);
        return;
    }
    if (nDelta > 1)
    {
        for (i = 0; i < nDelta; ++i)
            CONFIG_MoveSelection(1);
        return;
    }

    nRowCount = (int32_t)gnCfgControlsRowCount;
    nRow = (int32_t)gnCfgSelectedRow;
    if (nDelta < 0)
    {
        nNewRow = nRow > 0 ? nRow - 1 : nRow;
        if (nNewRow >= 0)
        {
            nRow = nNewRow;
            gnCfgSelectedRow = nRow;
        }
    }
    else if (nDelta > 0)
    {
        nNewRow = nRow < nRowCount ? nRow + 1 : nRow;
        if (nNewRow < nRowCount)
        {
            nRow = nNewRow;
            gnCfgSelectedRow = nRow;
        }
    }

    nFirstRow = (int32_t)gnCfgFirstVisibleRow;
    if (nRow < nFirstRow)
    {
        gnCfgFirstVisibleRow = nRow;
        CONFIG_ScrollList(0);
        return;
    }

    for (;;)
    {
        for (i = 0; nFirstRow < nRowCount && i < 15; ++i)
            ++nFirstRow;
        if (nRow < nFirstRow)
            return;
        CONFIG_ScrollList(1);
        nFirstRow = (int32_t)gnCfgFirstVisibleRow;
    }
}


static_assert(offsetof(D2CfgControlsRowStrc, nTblIndex) == 4,
              "a row's label is `mov cx, [esi+edx+4]` at 0x6FAFC916 and "
              "0x6FAFC92E: a WORD at +4, after the dword eCmd");
static_assert(STR_IDX_3921_CfgFunction == 0xF51
              && STR_IDX_3922_CfgPrimaryKey == 0xF52
              && STR_IDX_3923_CfgSecondaryKey == 0xF53,
              "the three headings: mov ecx, 0F51h / 0F52h / 0F53h at "
              "0x6FAFC731, 0x6FAFC79E and 0x6FAFC7FC");


//1.10f: D2Client.0x6FAFC620
//  Draws the panel on every frame UI_CONFIG is open (UIPANELS_DrawInGamePanels calls it); void, nobody reads what it leaves in eax. Against the
//  panel's left (gnScreenWidth - 620) / 2, its top (gnViewportBottom - 420) / 2 and a column of (gnScreenWidth - 2 * left - 49) / 3, it draws in
//  order:
//      a filled box at left, top, 369 tall (the screen width less both margins wide: 620 or 621)
//      two tiled frames from left to width - left - 3, from top + 3 and from top + 323, both down to top + 368
//      three headings at top + 30 in colour 4: tbl 3921 "Function" at left + 18, 3922 "Key/Button One" a column further, 3923 "Key/Button Two" two
//      further fifteen rows 18 pixels apart from top + 34, each row's text 18 below its top: the label at left + 30, the primary binding a column
//      further, the secondary two
//      the scrollbar: 21 track cels (frame 13) from top + 59, 12 apart, at width - left - 31; one thumb cel (frame 14) per gnCfgScrollThumbRows
//      from the thumb's top + 12, at width - left - 32 (one pixel left of the track, as the bytes have it); the up arrow at top + 47, the down
//      arrow at top + 305 the buttons: each label at its record's nLeft + 10, top + 351
//      the bind error, for 2000 ms after a refused bind, centred on the screen at top + 399 (below the 369-tall box)
//  CONFIG_SelectControlsList runs every frame before anything is drawn; CONFIG_UpdateControlsMouseState only when the cursor has moved since the
//  last draw or the button is down. The arrows are drawn pressed (up 9, down 8) only while their region holds the press and the cursor is on them,
//  else released (11 and 10); the track and the thumb have no pressed frame. The loop is `i < 15 && i < gnCfgControlsRowCount`, both compares
//  signed (hence the casts); a separator row (eCmd HOTKEY_MAX_COUNT) draws nothing and still takes its 18 pixels and its place among the fifteen.
//  The selected row gets an 18-pixel box three columns wide at left + 18; its label is BLUE while a capture is armed, every other label WHITE. A
//  binding is BLUE on the selected row while a capture is armed or when this is the bind column, else DARK_GOLD if the action has a key in this
//  column, GREY if only in the other column, RED if none. CMDTBL_ControlsIsActionBound is asked for the primary, then the secondary, then this
//  column's own again, up to three calls per column as the bytes make them; each `neg / sbb / add ebp, 5` tail is `bound ? 4 : 5`.
//  Blink: while a capture is armed the selected row's bind column is skipped on 5 drawn frames in every 16 ((gnCfgFrameCounter & 0xF) <= 4); the
//  counter, incremented as the last act, counts draws, not time. The primary is skipped when gnCfgBindColumn is non-zero, the secondary when it is
//  zero.
//  gnCfgBindColumn is tested != 0 for the skip, == 1 for the primary's colour and == 0 for the secondary's; only 0 and 1 are ever stored, so the
//  tests agree. When the blink skips the primary the bytes go straight to the secondary's colour (0x6FAFC96D -> 0x6FAFCA44): two independent `if`s.
//  The error line passes bCentered = 1 as well as centring its own x: the original does both.
//  The bytes reload gnScreenWidth and gnViewportBottom after every call; the C computes the panel's edges once, since nothing the function calls
//  writes them. The list pointer is re-read at every use, as in the bytes. `sub esp, 78h` holds the RECT, the 72-byte cel context and the copies of
//  the cursor, the font, the row and its blink flag.
void __cdecl CONFIG_DrawKeyBindingsPanel(void)
{
    enum Font nPrevFont;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nPanelLeft;
    int32_t nPanelTop;
    int32_t nColumn;
    RECT tPanelRect;
    D2GfxDataStrc tCelContext;
    int32_t i;
    int32_t nRow;
    int32_t nRowY;
    int32_t nTextY;
    BOOL bDrawBoundKey;
    int32_t nColor;
    uint32_t nButton;
    int32_t nErrorX;

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    CONFIG_SelectControlsList();
    if (gnCfgLastMouseX != nMouseX || gnCfgLastMouseY != nMouseY || gbCfgMouseButtonDown)
    {
        gnCfgLastMouseX = nMouseX;
        gnCfgLastMouseY = nMouseY;
        CONFIG_UpdateControlsMouseState(nMouseX, nMouseY);
    }

    nPrevFont = D2WIN_10127_SetFont(D2FONT_FONTINGAMECHAT);

    nPanelLeft = (gnScreenWidth - 620) / 2;
    nPanelTop = (gnViewportBottom - 420) / 2;
    nColumn = (gnScreenWidth - 2 * nPanelLeft - 49) / 3;

    GFX_DrawFilledRect(nPanelLeft, nPanelTop, gnScreenWidth - 2 * nPanelLeft, 369, 0, 1);
    tPanelRect.left = nPanelLeft;
    tPanelRect.right = gnScreenWidth - nPanelLeft - 3;
    tPanelRect.top = nPanelTop + 3;
    tPanelRect.bottom = nPanelTop + 368;
    UIPANELS_DrawTiledFrame(&tPanelRect);
    tPanelRect.top = nPanelTop + 323;
    UIPANELS_DrawTiledFrame(&tPanelRect);

    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_3921_CfgFunction), nPanelLeft + 18, nPanelTop + 30, 4, FALSE);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_3922_CfgPrimaryKey), nColumn + nPanelLeft + 18, nPanelTop + 30, 4, FALSE);
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_3923_CfgSecondaryKey), nPanelLeft + 2 * nColumn + 18, nPanelTop + 30, 4, FALSE);

    for (i = 0; i < 15 && i < (int32_t)gnCfgControlsRowCount; ++i)
    {
        nRowY = nPanelTop + 34 + 18 * i;
        nRow = (int32_t)gnCfgFirstVisibleRow + i;
        bDrawBoundKey = !gbCfgCapturingBind || nRow != (int32_t)gnCfgSelectedRow || (gnCfgFrameCounter & 0xF) > 4;

        if (gpCfgControlsList[nRow].eCmd == HOTKEY_MAX_COUNT)
            continue;

        nTextY = nRowY + 18;
        if (nRow == (int32_t)gnCfgSelectedRow)
        {
            GFX_DrawFilledRect(nPanelLeft + 18, nRowY + 1, 3 * nColumn, 18, 0, 1);
            D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex((D2C_StringIndices) gpCfgControlsList[nRow].nTblIndex), nPanelLeft + 30, nTextY, gbCfgCapturingBind ? 3 : 0, FALSE);
        }
        else
        {
            D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex((D2C_StringIndices) gpCfgControlsList[nRow].nTblIndex), nPanelLeft + 30, nTextY, 0, FALSE);
        }

        //  the primary binding, under "Key/Button One"
        if (bDrawBoundKey || !gnCfgBindColumn)
        {
            if (nRow == (int32_t)gnCfgSelectedRow && (gbCfgCapturingBind || gnCfgBindColumn == 1))
                nColor = 3;
            else if (CMDTBL_ControlsIsActionBound(gpCfgControlsList[nRow].eCmd, TRUE) || CMDTBL_ControlsIsActionBound(gpCfgControlsList[nRow].eCmd, FALSE))
                nColor = CMDTBL_ControlsIsActionBound(gpCfgControlsList[nRow].eCmd, TRUE) ? 4 : 5;
            else
                nColor = 1;
            D2WIN_10117_DrawText(CMDTBL_ControlsGetBoundKeyNameFallback(gpCfgControlsList[nRow].eCmd, TRUE), nColumn + nPanelLeft + 30, nTextY, nColor, FALSE);
        }

        //  the secondary binding, under "Key/Button Two"
        if (bDrawBoundKey || gnCfgBindColumn)
        {
            if (nRow == (int32_t)gnCfgSelectedRow && (gbCfgCapturingBind || !gnCfgBindColumn))
                nColor = 3;
            else if (CMDTBL_ControlsIsActionBound(gpCfgControlsList[nRow].eCmd, TRUE) || CMDTBL_ControlsIsActionBound(gpCfgControlsList[nRow].eCmd, FALSE))
                nColor = CMDTBL_ControlsIsActionBound(gpCfgControlsList[nRow].eCmd, FALSE) ? 4 : 5;
            else
                nColor = 1;
            D2WIN_10117_DrawText(CMDTBL_ControlsGetBoundKeyNameFallback(gpCfgControlsList[nRow].eCmd, FALSE), nPanelLeft + 2 * nColumn + 30, nTextY, nColor, FALSE);
        }
    }

    memset(&tCelContext, 0, sizeof(tCelContext));
    tCelContext.pCellFile = gpImgCfgTextSlider;
    tCelContext.nFrame = 13;
    for (i = 0; i < 21; ++i)
    {
        D2GFX_TEXTURE_CelDraw(&tCelContext, gnScreenWidth - nPanelLeft - 31, nPanelTop + 59 + 12 * i, 0xFFFFFFFF, 5, NULL);
    }

    tCelContext.nFrame = 14;
    for (i = 0; i < (int32_t)gnCfgScrollThumbRows; ++i)
    {
        D2GFX_TEXTURE_CelDraw(&tCelContext, gnScreenWidth - nPanelLeft - 32, (int32_t)gnCfgScrollThumbTop + 12 + 12 * i, 0xFFFFFFFF, 5, NULL);
    }

    tCelContext.nFrame = (gaCfgRegionPressed[1] && nMouseX >= gnScreenWidth - nPanelLeft - 31 && nMouseX <= gnScreenWidth - nPanelLeft - 18 && nMouseY >= nPanelTop + 35 && nMouseY <= nPanelTop + 47) ? 9 : 11;
    D2GFX_TEXTURE_CelDraw(&tCelContext, gnScreenWidth - nPanelLeft - 31, nPanelTop + 47, 0xFFFFFFFF, 5, NULL);

    tCelContext.nFrame = (gaCfgRegionPressed[2] && nMouseX >= gnScreenWidth - nPanelLeft - 31 && nMouseX <= gnScreenWidth - nPanelLeft - 18 && nMouseY >= nPanelTop + 293 && nMouseY <= nPanelTop + 305) ? 8 : 10;
    D2GFX_TEXTURE_CelDraw(&tCelContext, gnScreenWidth - nPanelLeft - 31, nPanelTop + 305, 0xFFFFFFFF, 5, NULL);

    //  i < 3 unsigned: the loop compares its byte offset against 78 with `jb`
    for (nButton = 0; nButton < 3; ++nButton)
    {
        D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(gCfgControlsButtons[nButton].nLabelStrId), gCfgControlsButtons[nButton].nLeft + 10, nPanelTop + 351, nButton == gnCfgHoveredButton ? gCfgControlsButtons[nButton].nHoverColor : 4, FALSE);
    }

    if (GetTickCount() < gnCfgBindErrorTick + 2000)
    {
        nErrorX = gnScreenWidth / 2 - D2WIN_10121_GetTextWidth(D2LANG_GetStringFromTblIndex(gnCfgBindErrorStrId)) / 2;
        D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(gnCfgBindErrorStrId), nErrorX, nPanelTop + 399, 1, TRUE);
    }

    D2WIN_10127_SetFont(nPrevFont);
    ++gnCfgFrameCounter;
}


//  The press walk: a press belongs to the region it began in until the button comes up, so before each region of CONFIG_UpdateControlsMouseState is
//  handled the whole press array is walked, and the region is skipped if any other one is pressed. The original repeats this loop eight times
//  inline, once excepting nothing and then once per region; it is written once as a helper. nExcept 7 excepts nothing: &gaCfgRegionPressed[7] is
//  the array's end, which the walk never reaches. The compare of the cursor against the end is signed (`jl`).
static BOOL CONFIG_IsOtherRegionPressed(int32_t nExcept)
{
    int32_t *pRegion;

    for (pRegion = gaCfgRegionPressed; (int32_t)pRegion < (int32_t)&gaCfgRegionPressed[7]; ++pRegion)
    {
        if (*pRegion && pRegion != &gaCfgRegionPressed[nExcept])
            return TRUE;
    }
    return FALSE;
}


//1.10f: D2Client.0x6FAFCE10
//  The Configure Controls screen's mouse model: hover, press and drag in one answer. Called by OnMouseDown and OnMouseUp around a click, by
//  CaptureMouseButton when a click ends a capture, and by the panel draw when the cursor has moved or the button is down. Seven regions, one
//  pressed flag each (gaCfgRegionPressed[i]: the button went down in region i and has not come up), against the panel's left (gnScreenWidth - 620)
//  / 2 and top (gnViewportBottom - 420) / 2, the scrollbar strip running from gnScreenWidth - left - 31 to -18, inclusive except where it says
//  after or before, with column = (gnScreenWidth - 2 * left - 49) / 3 + 6 (OnMouseUp's list bound) and the thumb ending 12 pixels per thumb cell
//  below its top:
//      [0] the list         x from left + 18 to left + 3 * column, y from +40 to +310
//      [1] the up arrow     the strip, y from +35 to +47
//      [2] the down arrow   the strip, y from +293 to +305
//      [3] above the thumb  the strip, y after +47 and before the thumb
//      [4] the thumb        the strip, y from the thumb's top to its end
//      [5] below the thumb  the strip, y after the thumb's end, before +293
//      [6] the buttons      CONFIG_HitTestButton answers something but 3
//  A press that begins outside every region is inert: with nothing pressed and the cursor over nothing the function copies the button state into
//  gbCfgMouseWasDown and returns, and while that is set every call returns at once (after putting gnCfgHoveredButton back to 3) until OnMouseUp
//  clears it.
//  A pressed region: [0] the selection moves up a row above the list and down a row below it (per call), then the row under the cursor becomes the
//  selection and the column under it the bind column, unless that row is a separator, when both go back to what they were; the region stays live
//  off the list while the button is held. [1] [2] the arrows scroll a row every 50 ms (gnCfgScrollRepeatTick against GetTickCount, called twice, to
//  test and to store), only while the cursor is on the arrow. [3] [5] the track pages by fourteen rows, a page of fifteen less one, on the same
//  clock and only while on the track. [4] the thumb: the press records the thumb's top and the cursor's y, and while the button is held the first
//  row becomes (startTop + (y - startY) - (top + 47)) * (last first row + 1) / (246 - 12 * thumb cells), ScrollList moving by the difference; 246
//  is a literal here.
//  [6] the buttons record the press and put the button under the cursor into gnCfgHoveredButton.
//  So one click on a key cell starts a capture: OnMouseDown sets the button flag and calls this, region [0] makes the row and column under the
//  cursor the selection, and OnMouseUp then finds the release on the selected row in the selected column. The original inlines CONFIG_HitTestRow
//  (region [0]) and CONFIG_HitTestButton (region [6]; the same function also calls it for the flag).
//  Kept as the bytes have it: the row is stored before the column is computed, and the row and column to restore are read after the two
//  MoveSelection calls; the page-up and page-down arms compute (gnViewportBottom - 420) / 2 and never use it; the thumb top is read again for a
//  drag's start, since the scrolls of regions [0] to [3] rewrite gnCfgScrollThumbTop after the flags were computed; gbCfgMouseButtonDown is cached
//  in esi and reloaded after each arm that called out, except after the thumb's scroll, so the page-down and button arms use the value from before
//  that call. The panel's top and the strip's right edge are recomputed for every test, and nothing between the tests writes gnViewportBottom or
//  gnScreenWidth. `sub esp, 24h` is the seven region flags and a copy of each argument.
void __fastcall CONFIG_UpdateControlsMouseState(int32_t nMouseX, int32_t nMouseY)
{
    int32_t nPanelLeft;
    int32_t nListColumn;
    int32_t nPanelTop;
    int32_t nStripLeft;
    int32_t nStripRight;
    int32_t nThumbTop;
    int32_t nThumbEnd;
    int32_t bOverList;
    int32_t bOverScrollUp;
    int32_t bOverScrollDown;
    int32_t bOverPageUp;
    int32_t bOverThumb;
    int32_t bOverPageDown;
    int32_t bOverButton;
    int32_t nPrevSelectedRow;
    int32_t nPrevBindColumn;
    int32_t nRow;
    int32_t nDragThumbTop;
    int32_t nUnusedTop;

    gnCfgHoveredButton = 3;
    if (gbCfgMouseWasDown)
        return;

    nPanelLeft = (gnScreenWidth - 620) / 2;
    nListColumn = (gnScreenWidth - 2 * nPanelLeft - 49) / 3 + 6;
    nPanelTop = (gnViewportBottom - 420) / 2;
    nStripLeft = gnScreenWidth - nPanelLeft - 31;
    nStripRight = gnScreenWidth - nPanelLeft - 18;
    nThumbTop = (int32_t)gnCfgScrollThumbTop;
    nThumbEnd = nThumbTop + 12 * (int32_t)gnCfgScrollThumbRows;

    bOverList = nMouseX >= nPanelLeft + 18 && nMouseX <= nPanelLeft + 2 * nListColumn + nListColumn && nMouseY >= nPanelTop + 40 && nMouseY <= nPanelTop + 310;
    bOverScrollUp = nMouseX >= nStripLeft && nMouseX <= nStripRight && nMouseY >= nPanelTop + 35 && nMouseY <= nPanelTop + 47;
    bOverScrollDown = nMouseX >= nStripLeft && nMouseX <= nStripRight && nMouseY >= nPanelTop + 293 && nMouseY <= nPanelTop + 305;
    bOverPageUp = nMouseX >= nStripLeft && nMouseX <= nStripRight && nMouseY > nPanelTop + 47 && nMouseY < nThumbTop;
    bOverThumb = nMouseX >= nStripLeft && nMouseX <= nStripRight && nMouseY >= nThumbTop && nMouseY <= nThumbEnd;
    bOverPageDown = nMouseX >= nStripLeft && nMouseX <= nStripRight && nMouseY < nPanelTop + 293 && nMouseY > nThumbEnd;
    bOverButton = CONFIG_HitTestButton(nMouseX, nMouseY) != 3;

    if (!CONFIG_IsOtherRegionPressed(7) && !bOverList && !bOverScrollUp && !bOverScrollDown && !bOverPageUp && !bOverThumb && !bOverPageDown && !bOverButton)
    {
        gbCfgMouseWasDown = gbCfgMouseButtonDown;
        return;
    }

    //  [0] the list
    if (!CONFIG_IsOtherRegionPressed(0) && (bOverList || gaCfgRegionPressed[0]))
    {
        gaCfgRegionPressed[0] = gbCfgMouseButtonDown;
        if (gbCfgMouseButtonDown)
        {
            if (nMouseY < nPanelTop + 40)
                CONFIG_MoveSelection(-1);
            if (nMouseY > nPanelTop + 310)
                CONFIG_MoveSelection(1);

            nPrevSelectedRow = (int32_t)gnCfgSelectedRow;
            nPrevBindColumn = gnCfgBindColumn;

            nRow = CONFIG_HitTestRow(nMouseY);
            gnCfgSelectedRow = nRow;
            gnCfgBindColumn = nMouseX < nPanelLeft + 2 * ((gnScreenWidth - 2 * nPanelLeft - 49) / 3) + 18;
            if (gpCfgControlsList[nRow].eCmd == HOTKEY_MAX_COUNT)
            {
                gnCfgSelectedRow = nPrevSelectedRow;
                gnCfgBindColumn = nPrevBindColumn;
            }
        }
    }

    //  [1] the up arrow
    if (!CONFIG_IsOtherRegionPressed(1) && bOverScrollUp)
    {
        gaCfgRegionPressed[1] = gbCfgMouseButtonDown;
        if (gbCfgMouseButtonDown && GetTickCount() > gnCfgScrollRepeatTick + 50)
        {
            gnCfgScrollRepeatTick = GetTickCount();
            CONFIG_ScrollList(-1);
        }
    }

    //  [2] the down arrow
    if (!CONFIG_IsOtherRegionPressed(2) && bOverScrollDown)
    {
        gaCfgRegionPressed[2] = gbCfgMouseButtonDown;
        if (gbCfgMouseButtonDown && GetTickCount() > gnCfgScrollRepeatTick + 50)
        {
            gnCfgScrollRepeatTick = GetTickCount();
            CONFIG_ScrollList(1);
        }
    }

    //  [3] the track above the thumb
    if (!CONFIG_IsOtherRegionPressed(3) && bOverPageUp)
    {
        gaCfgRegionPressed[3] = gbCfgMouseButtonDown;
        if (gbCfgMouseButtonDown && GetTickCount() > gnCfgScrollRepeatTick + 50)
        {
            gnCfgScrollRepeatTick = GetTickCount();
            //  computed and never read (0x6FAFD2C5)
            nUnusedTop = (gnViewportBottom - 420) / 2;
            (void)nUnusedTop;
            CONFIG_ScrollList(-14);
        }
    }

    //  [4] the thumb
    if (!CONFIG_IsOtherRegionPressed(4) && (bOverThumb || gaCfgRegionPressed[4]))
    {
        if (bOverThumb && !gaCfgRegionPressed[4] && gbCfgMouseButtonDown)
        {
            nDragThumbTop = (int32_t)gnCfgScrollThumbTop;
            gnCfgThumbDragStartY = nMouseY;
            gnCfgThumbDragStartTop = nDragThumbTop;
        }
        else
        {
            nDragThumbTop = gnCfgThumbDragStartTop;
        }

        gaCfgRegionPressed[4] = gbCfgMouseButtonDown;
        if (gbCfgMouseButtonDown)
        {
            CONFIG_ScrollList((nDragThumbTop - nPanelTop - gnCfgThumbDragStartY + nMouseY - 47) * (gnCfgMaxFirstVisibleRow + 1) / (246 - 12 * (int32_t)gnCfgScrollThumbRows) - (int32_t)gnCfgFirstVisibleRow);
        }
    }

    //  [5] the track below the thumb
    if (!CONFIG_IsOtherRegionPressed(5) && bOverPageDown)
    {
        gaCfgRegionPressed[5] = gbCfgMouseButtonDown;
        if (gbCfgMouseButtonDown && GetTickCount() > gnCfgScrollRepeatTick + 50)
        {
            gnCfgScrollRepeatTick = GetTickCount();
            //  computed and never read (0x6FAFD3E6)
            nUnusedTop = (gnViewportBottom - 420) / 2;
            (void)nUnusedTop;
            CONFIG_ScrollList(14);
        }
    }

    //  [6] the buttons
    if (!CONFIG_IsOtherRegionPressed(6) && bOverButton)
    {
        gaCfgRegionPressed[6] = gbCfgMouseButtonDown;
        gnCfgHoveredButton = CONFIG_HitTestButton(nMouseX, nMouseY);
    }
}


//  The keyboard half of gConfigMessageHandlers: VK_UP, VK_DOWN, VK_LEFT and VK_RIGHT go to CONFIG_OnArrowUp, OnArrowDown, OnArrowLeft and
//  OnArrowRight, VK_DELETE and VK_BACK to CONFIG_OnEraseKey, VK_ESCAPE and VK_SPACE to CONFIG_OnDismissKey. UP and DOWN move the selected row and
//  skip separators; LEFT and RIGHT move between the two binding columns and touch no row.
//  All six open with the same disconnect gate, `if (GAME_IsDisconnecting()) pMsg->bReturn = 0;`, and nothing else: no lResult and no
//  SMsgBreakHandlerChain, so while the client is disconnecting every key on this screen passes straight through to whatever else is listening and
//  then to DefWindowProcA. Written out six times.
//  Bit 30 of lParam is WM_KEYDOWN's previous-key state, set on a repeat: OnEraseKey and OnDismissKey guard their action with
//  `(pMsg->lParam & 0x40000000) == 0`, so neither unbinding nor closing the screen repeats while the key is held; the arrows repeat.
//  Both of those read gbCfgBindArmed as `if (gbCfgBindArmed) { if (not a repeat) <act> } else gbCfgBindArmed = 1;`: the flag being set is what lets
//  the action happen. See CONFIG_OnEnterBeginCapture, the only writer of 0.

static_assert(HOTKEY_MAX_COUNT == 57,
              "a controls-list row whose eCmd is HOTKEY_MAX_COUNT is a "
              "separator, and it is the value the two row walks skip");
static_assert(offsetof(D2CfgControlsRowStrc, eCmd) == 0,
              "`gpCfgControlsList[gnCfgSelectedRow].eCmd` compiles to a read "
              "at the record's base, so the command is its first field");


//1.10f: D2Client.0x6FAFD8A0
//  VK_UP: moves the selection up one selectable row. The click sound plays only when the selection actually moved, which is what the saved
//  nCfgSelectedRow is for, so holding UP at the top of the list is silent.
//  BUG: the row walk `do CONFIG_MoveSelection(-1); while (gpCfgControlsList[gnCfgSelectedRow].eCmd == HOTKEY_MAX_COUNT);` is unbounded: a list
//  whose every row is a separator (57) spins forever, and nothing counts the rows already passed. LATENT: the controls list is built from a static
//  table with real bindings at both ends and its separators inside.
void __stdcall CONFIG_OnArrowUp(SMSGHANDLER_PARAMS *pMsg)
{
    D2CfgControlsRowStrc *pControlsList;
    int32_t nRowBefore;

    if (GAME_IsDisconnecting())
    {
        pMsg->bReturn = 0;
        return;
    }

    pControlsList = gpCfgControlsList;
    nRowBefore = gnCfgSelectedRow;

    do
        CONFIG_MoveSelection(-1);
    while (pControlsList[gnCfgSelectedRow].eCmd == HOTKEY_MAX_COUNT);

    if (nRowBefore != (int32_t)gnCfgSelectedRow)
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FAFD910
//  VK_DOWN: the same function with +1 for -1; the list pointer is latched into a local before the walk and the index is re-read from the global
//  inside it.
void __stdcall CONFIG_OnArrowDown(SMSGHANDLER_PARAMS *pMsg)
{
    D2CfgControlsRowStrc *pControlsList;
    int32_t nRowBefore;

    if (GAME_IsDisconnecting())
    {
        pMsg->bReturn = 0;
        return;
    }

    pControlsList = gpCfgControlsList;
    nRowBefore = gnCfgSelectedRow;

    do
        CONFIG_MoveSelection(1);
    while (pControlsList[gnCfgSelectedRow].eCmd == HOTKEY_MAX_COUNT);

    if (nRowBefore != (int32_t)gnCfgSelectedRow)
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FAFD980
//  VK_LEFT: moves between the binding columns, not rows: from the secondary binding (0, drawn on the right) to the primary (1, on the left). The
//  value is CMDTBL_BindKey's bPrimary, and OnMouseUp's `(nMouseX < boundary) == gnCfgBindColumn` draws the primary on the left.
void __stdcall CONFIG_OnArrowLeft(SMSGHANDLER_PARAMS *pMsg)
{
    if (GAME_IsDisconnecting())
    {
        pMsg->bReturn = 0;
        return;
    }

    if (!gnCfgBindColumn)
    {
        gnCfgBindColumn = 1;
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FAFD9E0
//  VK_RIGHT: the mirror, 1 -> 0. Neither key can move twice, so neither needs the before-and-after comparison the row keys use.
void __stdcall CONFIG_OnArrowRight(SMSGHANDLER_PARAMS *pMsg)
{
    if (GAME_IsDisconnecting())
    {
        pMsg->bReturn = 0;
        return;
    }

    if (gnCfgBindColumn == 1)
    {
        gnCfgBindColumn = 0;
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FAFDB50
//  VK_DELETE and VK_BACK (both records of the table): clear the selected row's binding in the selected column. Sound 2 rather than the arrows' 1,
//  and no repeat, by the lParam bit-30 guard.
void __stdcall CONFIG_OnEraseKey(SMSGHANDLER_PARAMS *pMsg)
{
    if (GAME_IsDisconnecting())
    {
        pMsg->bReturn = 0;
        return;
    }

    if (gbCfgBindArmed)
    {
        if ((pMsg->lParam & 0x40000000) == 0)
        {
            CMDTBL_UnbindCommand(gpCfgControlsList[gnCfgSelectedRow].eCmd, gnCfgBindColumn);
            SOUNDINST_PlaySound(SOUND_CURSOR_SELECT, NULL, 0, 0, 0);
        }
    }
    else
    {
        gbCfgBindArmed = 1;
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FAFDBD0
//  VK_ESCAPE and VK_SPACE: leave the screen and discard every change made on it: CMDTBL_RestoreBindingsSnapshot() puts the bindings back,
//  UI_SetUIToggle(UI_CONFIG, UI_TURNOFF) closes the panel, ESCMENU_SetPage(0, 1) returns to the ESC menu's page 1. So this is Cancel, not close;
//  there is no keyboard route to accepting. The first argument 0 of ESCMENU_SetPage does not close the UI windows on the way (the panel has just
//  been turned off). The original inlines CONFIG_OnCancelButton's three calls here; the one difference is scheduling: `push 0` comes before the two
//  register loads for UI_SetUIToggle.
void __stdcall CONFIG_OnDismissKey(SMSGHANDLER_PARAMS *pMsg)
{
    if (GAME_IsDisconnecting())
    {
        pMsg->bReturn = 0;
        return;
    }

    if (gbCfgBindArmed)
    {
        if ((pMsg->lParam & 0x40000000) == 0)
        {
            CONFIG_OnCancelButton();
            SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
        }
    }
    else
    {
        gbCfgBindArmed = 1;
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//  The mouse half of gConfigMessageHandlers and the two hit tests: WM_LBUTTONDOWN and WM_RBUTTONDOWN go to CONFIG_OnMouseDown, WM_LBUTTONUP and
//  WM_RBUTTONUP to CONFIG_OnMouseUp (a right click does what a left click does on this screen), VK_RETURN to CONFIG_OnEnterBeginCapture and
//  WM_MOUSEWHEEL to CONFIG_OnMouseWheel (gConfigWheelHandler).
//  gbCfgBindArmed means "will act on the next key event". CONFIG_OnEnterBeginCapture, the only writer of 0, clears it the instant a capture begins,
//  and the first capture handler that sees it clear sets it again and does nothing, so it swallows exactly one key event: the release of the key
//  that began the capture (without it ENTER's own key-up would be bound). gbCfgCapturingBind is the separate flag that means "capturing".
//  The hit tests' sentinels: CONFIG_HitTestButton returns the count of the buttons it missed, so a miss answers 3; CONFIG_HitTestRow answers
//  gnCfgControlsRowCount - 1 for anything out of range, so a click below the last row selects the last row (with a row count of 0 the answer is -1;
//  an empty controls list does not occur). The list geometry: the first row's bottom edge is (gnViewportBottom - 420) / 2 + 58, a row is 18 pixels
//  and a page is 15 rows.

static_assert(sizeof(D2CfgControlsRowStrc) == 10,
              "the row index compiles to `lea eax, [edi+edi*4]` then "
              "`[ecx+eax*2]`, which is edi * 10 -- the record is PACKED, and "
              "an unpacked one would be 12 and index wrongly");
static_assert(sizeof(D2ClientCfgControlsButtonStrc) == 26
              && offsetof(D2ClientCfgControlsButtonStrc, pfnOnClick) == 2,
              "the button record is PACKED: a uint16 string id and then six "
              "dwords, 26 bytes, so OnMouseUp's `pfnOnClick()` comes off a "
              "MISALIGNED +2. Written as +4 first and the assertion caught it");


//1.10f: D2Client.0x6FAFD520
//  Which of the three buttons the cursor is over (stride 26, `jb` against 78): the loop counts the buttons it did not hit and returns that count,
//  so a miss answers 3, not -1; OnMouseUp's `!= 3` reads the sentinel. The original inlines it in CONFIG_UpdateControlsMouseState and in
//  CONFIG_OnMouseUp.
int32_t __fastcall CONFIG_HitTestButton(int32_t nMouseX, int32_t nMouseY)
{
    int32_t nMissed;
    uint32_t i;

    nMissed = 0;
    for (i = 0; i < 3; ++i)
    {
        if (nMouseX >= gCfgControlsButtons[i].nLeft && nMouseX <= gCfgControlsButtons[i].nRight && nMouseY >= gCfgControlsButtons[i].nTop && nMouseY <= gCfgControlsButtons[i].nBottom)
        {
            break;
        }
        ++nMissed;
    }

    return nMissed;
}


//1.10f: D2Client.0x6FAFD550
//  A mouse button went down (both buttons): records the press and hands the position to CONFIG_UpdateControlsMouseState, which sets the per-region
//  pressed flags.
void __stdcall CONFIG_OnMouseDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nMouseX;
    int32_t nMouseY;

    if (GAME_IsDisconnecting())
    {
        pMsg->bReturn = 0;
        return;
    }

    nMouseX = pMsg->nXpos;
    nMouseY = pMsg->nYpos;

    gbCfgMouseButtonDown = 1;
    CONFIG_UpdateControlsMouseState(nMouseX, nMouseY);

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FAFD5A0
//  The release (both mouse buttons), the biggest handler of the table. Two independent things can happen, each guarded by the region the press
//  landed in: gaCfgRegionPressed[0] is the list and [6] the button row. A release on the selected row in the selected column begins a capture: the
//  test is `(nMouseX < boundary) == gnCfgBindColumn`, column 1 being the left one (the primary binding); it reads like a precedence accident and is
//  exactly what the bytes do. A button release dispatches gCfgControlsButtons[gnCfgHoveredButton].pfnOnClick(); the original inlines
//  CONFIG_HitTestButton for that.
//  Whatever happened, the press state is cleared on the way out: the whole seven-entry region array, both mouse flags, and one more
//  UpdateControlsMouseState so the hover redraws without the press.
void __stdcall CONFIG_OnMouseUp(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nPanelLeft;
    int32_t nGridThird;
    int32_t nPanelTop;
    int32_t nMissed;

    if (GAME_IsDisconnecting())
    {
        pMsg->bReturn = 0;
        return;
    }

    nMouseX = pMsg->nXpos;
    nMouseY = pMsg->nYpos;

    if (gaCfgRegionPressed[0])
    {
        nPanelLeft = (gnScreenWidth - 620) / 2;
        if (nMouseX >= nPanelLeft + 18)
        {
            nGridThird = (gnScreenWidth - 2 * nPanelLeft - 49) / 3 + 6;
            if (nMouseX <= nPanelLeft + 2 * nGridThird + nGridThird)
            {
                nPanelTop = (gnViewportBottom - 420) / 2;
                if (nMouseY >= nPanelTop + 40
                    && nMouseY <= nPanelTop + 310
                    && CONFIG_HitTestRow(nMouseY) == (int32_t)gnCfgSelectedRow
                    //  `(x < boundary) == column`: column 1 is the LEFT one
                    && (nMouseX < (gnScreenWidth - 620) / 2
                                  + 2 * ((gnScreenWidth
                                          - 2 * ((gnScreenWidth - 620) / 2)
                                          - 49) / 3)
                                  + 18)
                       == gnCfgBindColumn
                    && gpCfgControlsList[gnCfgSelectedRow].eCmd
                       != HOTKEY_MAX_COUNT)
                {
                    //  The capture start: gbCfgCapturingBind, the panel handlers off, the six key handlers on with the three
                    //  conditional one-record tables, and sound 2. CONFIG_OnEnterBeginCapture has the same block. The three OS
                    //  tests behind the extra tables (GAME_IsSupportedWindows: the middle button, GAME_IsWin2000OrXp: the X
                    //  buttons, GAME_HasNativeMouseWheel: the wheel) mean that on an OS without them those inputs cannot be
                    //  bound at all. UI_CONFIG's own table is unregistered first, which is why the keyboard half's handlers
                    //  stop firing during a capture.
                    gbCfgCapturingBind = 1;
                    UI_UnregisterPanelHandlers(UI_CONFIG);

                    WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gCfgCaptureKeyHandlers, 6);

                    if (GAME_IsSupportedWindows())
                    {
                        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gCfgCaptureMButtonHandler, 1);
                    }
                    if (GAME_IsWin2000OrXp())
                    {
                        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gCfgCaptureXButtonHandler, 1);
                    }
                    if (GAME_HasNativeMouseWheel())
                    {
                        WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gCfgCaptureWheelHandler, 1);
                    }

                    SOUNDINST_PlaySound(SOUND_CURSOR_SELECT, NULL, 0, 0, 0);
                }
            }
        }
    }

    if (gaCfgRegionPressed[6] && gnCfgHoveredButton != 3)
    {
        nMissed = CONFIG_HitTestButton(nMouseX, nMouseY);

        if (nMissed == (int32_t)gnCfgHoveredButton)
        {
            gCfgControlsButtons[gnCfgHoveredButton].pfnOnClick();
            SOUNDINST_PlaySound(SOUND_CURSOR_SELECT, NULL, 0, 0, 0);
        }
    }

    memset(gaCfgRegionPressed, 0, 7 * sizeof(int32_t));
    gbCfgMouseWasDown = 0;
    gbCfgMouseButtonDown = 0;
    CONFIG_UpdateControlsMouseState(nMouseX, nMouseY);

    pMsg->lResult = 0;
    pMsg->bReturn = 1;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FAFD830
//  The wheel, its own one-record table because it is registered only when the machine has a wheel. `SHIWORD(wParam) / 120` is the signed notch
//  count and the list scrolls -2 rows per notch, inverted: rolling the wheel away from you moves down the list. A delta smaller than one notch does
//  nothing and makes no sound.
void __stdcall CONFIG_OnMouseWheel(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nNotches;

    if (GAME_IsDisconnecting())
    {
        pMsg->bReturn = 0;
        return;
    }

    nNotches = (int16_t)HIWORD(pMsg->nKey) / 120;
    if (nNotches)
    {
        CONFIG_ScrollList(-2 * nNotches);
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FAFD7E0
//  Which list row is at this screen y; the sole caller is CONFIG_OnMouseUp (the original also inlines it in CONFIG_UpdateControlsMouseState). The
//  gotos are the original's graph: the out-of-range answer is reached from three places: the first-visible row already past the end, the walk
//  running off the end mid-page, and the page filling without a hit.
int32_t __fastcall CONFIG_HitTestRow(int32_t nMouseY)
{
    int32_t nRow;
    int32_t nRowsScanned;
    int32_t nRowBottom;
    int32_t nRowCount;

    nRow = gnCfgFirstVisibleRow;
    nRowsScanned = 0;

    if (gnCfgFirstVisibleRow >= gnCfgControlsRowCount)
        goto out_of_range;

    nRowBottom = (gnViewportBottom - 420) / 2 + 58;
    while (nRowsScanned < 15)
    {
        if (nMouseY <= nRowBottom)
            return nRow;
        ++nRowsScanned;
        nRowBottom += 18;
        if (++nRow >= (int32_t)gnCfgControlsRowCount)
            goto out_of_range;
    }

    if (nRow >= (int32_t)gnCfgControlsRowCount)
        goto out_of_range;
    return nRow;

out_of_range:
    nRowCount = gnCfgControlsRowCount;
    if (nRowCount >= 0)
        return nRowCount - 1;
    return nRowCount;
}


//1.10f: D2Client.0x6FAFDA40
//  VK_RETURN begins a capture: the keyboard's route into the same capture start as OnMouseUp, and the only writer of 0 to gbCfgBindArmed. A
//  separator row cannot be bound: `eCmd != HOTKEY_MAX_COUNT` is tested before anything happens, as OnMouseUp tests it.
void __stdcall CONFIG_OnEnterBeginCapture(SMSGHANDLER_PARAMS *pMsg)
{
    if (GAME_IsDisconnecting())
    {
        pMsg->bReturn = 0;
        return;
    }

    if (gbCfgBindArmed)
    {
        if ((pMsg->lParam & 0x40000000) == 0 && gpCfgControlsList[gnCfgSelectedRow].eCmd != HOTKEY_MAX_COUNT)
        {
            //  The capture start, as in CONFIG_OnMouseUp.
            gbCfgCapturingBind = 1;
            UI_UnregisterPanelHandlers(UI_CONFIG);

            WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gCfgCaptureKeyHandlers, 6);

            if (GAME_IsSupportedWindows())
            {
                WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gCfgCaptureMButtonHandler, 1);
            }
            if (GAME_IsWin2000OrXp())
            {
                WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gCfgCaptureXButtonHandler, 1);
            }
            if (GAME_HasNativeMouseWheel())
            {
                WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gCfgCaptureWheelHandler, 1);
            }

            SOUNDINST_PlaySound(SOUND_CURSOR_SELECT, NULL, 0, 0, 0);
            gbCfgBindArmed = 0;
        }
    }
    else
    {
        gbCfgBindArmed = 1;
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//  The capture: for as long as the screen waits for a key these six handlers are the game's whole input handling. OnEnterBeginCapture and OnMouseUp
//  register four tables:
//      gCfgCaptureKeyHandlers, 6 records of kind 0 (window messages): WM_LBUTTONDOWN and WM_RBUTTONDOWN to CONFIG_CaptureMouseButton, WM_KEYDOWN
//      and WM_SYSKEYDOWN to CONFIG_CaptureKeyDown, WM_KEYUP and WM_SYSKEYUP to CONFIG_CaptureKeyUp
//      gCfgCaptureMButtonHandler, WM_MBUTTONDOWN to CONFIG_CaptureMiddleButton
//      gCfgCaptureXButtonHandler, WM_XBUTTONDOWN to CONFIG_CaptureXButton
//      gCfgCaptureWheelHandler, WM_MOUSEWHEEL to CONFIG_CaptureMouseWheel
//  The panel's own table is kind 3, one record per key; a capture wants any key, so it takes the raw messages, the SYS variants too, which is what
//  makes Alt-combinations and F10 bindable. All six keep the disconnect gate and claim what they handle; they differ otherwise:
//  KeyDown has a repeat test (bit 30) and no armed test: the first press of any key is bound, ESCAPE cancels instead (tear down, sound 1, disarm),
//  and after binding ENTER, DELETE, BACKSPACE or SPACE it disarms (those are the panel's own action keys: the panel is back the moment the bind
//  lands, and disarmed its handlers only re-arm on that key's next press); any other key leaves the screen armed. KeyUp has no repeat test, because
//  a WM_KEYUP always carries bit 30, and binds only when armed: ENTER disarms as it starts a capture, and ENTER's own release arrives here, re-arms
//  and binds nothing. A key Windows sends only as a release (PrintScreen) is bound here once armed, and CMDTBL_BindKey takes PrintScreen for the
//  screenshot command alone. The mouse pseudo-keys are CMDTBL_BindKey's: 0x100 the middle button, 0x101 and 0x102 the X buttons (HIWORD(wParam) 1
//  and 2; any other value binds nothing and still claims the message), 0x103 and 0x104 the wheel forward and back (only the sign of the notch count
//  reaches the binding; a delta under one notch binds nothing). The three mouse captures play no sound on success, where both keyboard ones play
//  sound 2, and read the error slot only when the bind fails.
//  CMDTBL_BindKey clears the error slot on entry and sets it only on its three refusals (tbl 3977, 3978, 3979), so a successful bind leaves 0. The
//  slot is the dword that `push ecx` makes (three captures) or the handler's own pMsg argument slot (XButton and MouseWheel); all five zero it as a
//  dword and test its low word, and the C gives each a local __int16. A refusal's tbl index goes to gnCfgBindErrorStrId with the tick, for the
//  panel draw.
//  Every bind passes gnCfgBindColumn as CMDTBL_BindKey's bPrimary: 1 is the primary binding, in the left column, 0 the secondary, on the right.

static_assert(VK_ESCAPE == 0x1B && VK_RETURN == 0x0D && VK_DELETE == 0x2E
              && VK_BACK == 0x08 && VK_SPACE == 0x20,
              "CaptureKeyDown compares wParam against 1Bh, then 0Dh, 2Eh, 8 "
              "and 20h (0x6FAFDD63, 0x6FAFDEAB..0x6FAFDEBA)");


//  The capture's teardown: unregister the key table, then the wheel, X-button and middle-button tables, each behind the predicate that gated its
//  registration and in the reverse of the registration order; put the panel's two tables back with UI_ConfigRegisterPanelHandlers; clear
//  gbCfgCapturingBind. The original repeats it in seven handlers.
static void CONFIG_EndBindCapture(void)
{
    WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gCfgCaptureKeyHandlers, 6);
    if (GAME_HasNativeMouseWheel())
    {
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gCfgCaptureWheelHandler, 1);
    }
    if (GAME_IsWin2000OrXp())
    {
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gCfgCaptureXButtonHandler, 1);
    }
    if (GAME_IsSupportedWindows())
    {
        WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), &gCfgCaptureMButtonHandler, 1);
    }
    UI_ConfigRegisterPanelHandlers();
    gbCfgCapturingBind = 0;
}


//1.10f: D2Client.0x6FAFDC60
//  A left or right click during a capture (records 0 and 1 of the key table). It binds nothing: the capture ends, and the click becomes a press on
//  the panel (gbCfgMouseButtonDown set, passed to CONFIG_UpdateControlsMouseState), so its release reaches OnMouseUp, which starts a new capture if
//  the click landed on the selected row in the selected column. The two main buttons cannot be bound from this screen. The position is read as two
//  zero-extended words (`mov di, [esi+0Ch]`, `mov bx, [esi+0Eh]`, each register cleared first).
void __stdcall CONFIG_CaptureMouseButton(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nMouseX;
    int32_t nMouseY;

    if (GAME_IsDisconnecting())
    {
        pMsg->bReturn = 0;
        return;
    }

    nMouseX = pMsg->nXpos;
    nMouseY = pMsg->nYpos;

    CONFIG_EndBindCapture();
    gbCfgMouseButtonDown = 1;
    CONFIG_UpdateControlsMouseState(nMouseX, nMouseY);

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FAFDD30
//  A key goes down during a capture (records 2 and 3). A repeat is claimed and ignored; ESCAPE cancels; anything else is offered to CMDTBL_BindKey
//  for the selected row and column. ENTER, DELETE, BACKSPACE and SPACE disarm.
void __stdcall CONFIG_CaptureKeyDown(SMSGHANDLER_PARAMS *pMsg)
{
    int16_t nErrorTblIdx;

    if (GAME_IsDisconnecting())
    {
        pMsg->bReturn = 0;
        return;
    }

    nErrorTblIdx = 0;
    if ((pMsg->lParam & 0x40000000) == 0)
    {
        if (pMsg->wParam == VK_ESCAPE)
        {
            CONFIG_EndBindCapture();
            SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
            gbCfgBindArmed = 0;
        }
        else if (CMDTBL_BindKey(gpCfgControlsList[gnCfgSelectedRow].eCmd, gnCfgBindColumn, (uint16_t)pMsg->wParam, &nErrorTblIdx))
        {
            CONFIG_EndBindCapture();
            SOUNDINST_PlaySound(SOUND_CURSOR_SELECT, NULL, 0, 0, 0);
            if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_DELETE || pMsg->wParam == VK_BACK || pMsg->wParam == VK_SPACE)
            {
                gbCfgBindArmed = 0;
            }
            else
            {
                gbCfgBindArmed = 1;
            }
        }
        else
        {
            gbCfgBindArmed = 1;
        }

        if (nErrorTblIdx)
        {
            gnCfgBindErrorStrId = (D2C_StringIndices)nErrorTblIdx;
            gnCfgBindErrorTick = GetTickCount();
        }
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FAFDF10
//  A key comes up during a capture (records 4 and 5). Disarmed, it re-arms and does nothing else; armed, it offers the key to CMDTBL_BindKey. The
//  armed flag is loaded before the error slot is cleared.
void __stdcall CONFIG_CaptureKeyUp(SMSGHANDLER_PARAMS *pMsg)
{
    int16_t nErrorTblIdx;

    if (GAME_IsDisconnecting())
    {
        pMsg->bReturn = 0;
        return;
    }

    nErrorTblIdx = 0;
    if (gbCfgBindArmed)
    {
        if (CMDTBL_BindKey(gpCfgControlsList[gnCfgSelectedRow].eCmd, gnCfgBindColumn, (uint16_t)pMsg->wParam, &nErrorTblIdx))
        {
            CONFIG_EndBindCapture();
            SOUNDINST_PlaySound(SOUND_CURSOR_SELECT, NULL, 0, 0, 0);
        }

        if (nErrorTblIdx)
        {
            gnCfgBindErrorStrId = (D2C_StringIndices)nErrorTblIdx;
            gnCfgBindErrorTick = GetTickCount();
        }
    }
    else
    {
        gbCfgBindArmed = 1;
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FAFE040
//  The middle button during a capture: gCfgCaptureMButtonHandler, registered only where GAME_IsSupportedWindows. Pseudo-key 0x100; no sound on
//  success.
void __stdcall CONFIG_CaptureMiddleButton(SMSGHANDLER_PARAMS *pMsg)
{
    int16_t nErrorTblIdx;

    if (GAME_IsDisconnecting())
    {
        pMsg->bReturn = 0;
        return;
    }

    nErrorTblIdx = 0;
    if (CMDTBL_BindKey(gpCfgControlsList[gnCfgSelectedRow].eCmd, gnCfgBindColumn, 0x100, &nErrorTblIdx))
    {
        CONFIG_EndBindCapture();
    }
    else if (nErrorTblIdx)
    {
        gnCfgBindErrorStrId = (D2C_StringIndices)nErrorTblIdx;
        gnCfgBindErrorTick = GetTickCount();
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FAFE140
//  An X button during a capture: gCfgCaptureXButtonHandler, registered only on Windows 2000 or XP. HIWORD(wParam) names the button: 1 is pseudo-key
//  0x101 and 2 is 0x102, tested by `dec eax / jz`, `dec eax / jnz` (0x6FAFE168), and the two arms push their constant and share one call. Any other
//  value binds nothing and is still claimed. No sound on success.
void __stdcall CONFIG_CaptureXButton(SMSGHANDLER_PARAMS *pMsg)
{
    uint16_t wButton;
    int16_t nErrorTblIdx;

    if (GAME_IsDisconnecting())
    {
        pMsg->bReturn = 0;
        return;
    }

    wButton = HIWORD(pMsg->wParam);
    if (wButton == 1 || wButton == 2)
    {
        nErrorTblIdx = 0;
        if (CMDTBL_BindKey(gpCfgControlsList[gnCfgSelectedRow].eCmd, gnCfgBindColumn, wButton == 1 ? 0x101 : 0x102, &nErrorTblIdx))
        {
            CONFIG_EndBindCapture();
        }
        else if (nErrorTblIdx)
        {
            gnCfgBindErrorStrId = (D2C_StringIndices)nErrorTblIdx;
            gnCfgBindErrorTick = GetTickCount();
        }
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FAFE250
//  The wheel during a capture: gCfgCaptureWheelHandler, registered only where GAME_HasNativeMouseWheel. `SHIWORD(wParam) / 120` is the signed notch
//  count, and only its sign reaches the binding (`setl cl / add ecx, 103h`, 0x6FAFE291): 0x103 forward, 0x104 back. No sound on success.
void __stdcall CONFIG_CaptureMouseWheel(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nNotches;
    int16_t nErrorTblIdx;

    if (GAME_IsDisconnecting())
    {
        pMsg->bReturn = 0;
        return;
    }

    nNotches = (int16_t)HIWORD(pMsg->wParam) / 120;
    if (nNotches)
    {
        nErrorTblIdx = 0;
        if (CMDTBL_BindKey(gpCfgControlsList[gnCfgSelectedRow].eCmd, gnCfgBindColumn, (uint16_t)((nNotches < 0) + 0x103), &nErrorTblIdx))
        {
            CONFIG_EndBindCapture();
        }
        else if (nErrorTblIdx)
        {
            gnCfgBindErrorStrId = (D2C_StringIndices)nErrorTblIdx;
            gnCfgBindErrorTick = GetTickCount();
        }
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

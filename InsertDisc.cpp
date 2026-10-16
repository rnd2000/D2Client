#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "CoreArchive.h"
#include "ui.h"
#include "InsertDisc.h"

//  InsertDisc.cpp -- the "Please Insert the Diablo II ... Disc" prompts of D2Client.dll 1.10: the play / expansion prompt and the cinematics
//  prompt, each with an idle draw, a one-frame scene draw, a Show function and four message handlers, plus INSERTDISC_SetDiscPromptKind.
//  Sixteen functions, address range 0x6FADBF50..0x6FADCF07. No assert names the original's source file, so the file name is ours.
//  LANG_SubstituteArg and LANG_FormatString, which follow the prompts, are not part of the unit (LANG_FormatString is called from three
//  modules) and are written in Shared.cpp.
//  A prompt is a "retry?" callback, not a message: VIDEO_PlayCinematic hands the INSERTDISC_ShowInsert*DiscPrompt functions to D2Win's archive
//  loaders (D2WIN_ARCHIVE_LoadVideoArchives #10172, D2WIN_ARCHIVE_LoadExpansionArchives #10171) as ARCHIVE_ShowMessageFunctionPtr, int
//  (__stdcall *)(). The loader calls one when an archive cannot be opened and retries while it answers non-zero. So the play and expansion
//  prompts, which answer 1 always, wait for the disc for as long as it takes: OK closes one prompt and the loader, finding no disc, shows the
//  next. The cinematics prompt is the one that can give up.
//  Each prompt runs a nested message pump -- WNDPROC_WindowRunMessagePump with the prompt's draw as its idle function -- and returns when the
//  draw answers non-zero. The game's own frame, GAME_UpdateFrame, is the outer pump's idle function, so it does not run while a prompt is up.

//  The two artwork names, as the image holds them. The first (0x6FB79268) is InsertDisc.h's pszName and the loads below name that; the
//  second (0x6FB79248) stays a literal here.
static const char szOkCancelBtn[] = "Data\\Global\\UI\\Menu\\okcancelbtn";   // 0x6FB79248


//1.10f: D2Client.0x6FADBF50
//  One frame of the play / expansion prompt, and its idle function: handed to WNDPROC_WindowRunMessagePump by both Show functions below. It
//  pings the server first, every frame (MSG_SendPing6C): the prompt can come up in a game (VIDEO_PlayCinematic), and the outer frame that would
//  otherwise keep the connection busy is not running. Then it draws one frame, pumps the sound preload, and answers whether the button has
//  been pressed. The argument is the pump's idle counter and is not read; retn 4 keeps it. The original inlines
//  INSERTDISC_DrawInsertDiscPromptScene here.
//  The panel is two frame cels side by side, two lines of text in font 1 (D2FONT_FONT16), and one button with "OK" on it, all placed through
//  gnBorderXDifference / gnBorderYDifference (x and y below):
//      frame     cel gpDiscPromptFrameCel, frame 0 at (x+188, 328-y) and frame 1 at (x+444, 328-y)
//      line 1    tbl 5168 "Please Insert the Diablo II", centred between gnScreenWidth / 2 - 132 and x+452, at 190-y
//      line 2    tbl 21880 "Expansion Disc" when FOG_IsExpansion(), else tbl 5169 "Play Disc"; the same span, at 205-y
//      button    cel gpDiscPromptButtonCel, frame gnDiscPromptButtonFrame, at (x+272, 318-y); tbl 5102 "OK" centred between x+272 and
//                x+368, at 310-y
//  The division is signed (cdq; sub eax, edx; sar ecx, 1) and gnScreenWidth is an int.
//  BUG: the second line is not the caller's to choose. The play prompt and the expansion prompt draw with this one function, and it asks
//  FOG_IsExpansion(), not which prompt is up, so in an expansion install every one of them reads "Expansion Disc", the play prompt included.
//  REACHABLE: whether that names the wrong disc is NOT ESTABLISHED. D2Win's expansion-archive loader hands the play prompt to the d2char.mpq
//  and d2music.mpq loads and the expansion prompt to the three d2X*.mpq in an expansion install, and the play prompt to the same two otherwise,
//  so one pair of archives is asked for as the "Expansion Disc" in one install and the "Play Disc" in the other; which retail disc carries
//  them is not known.
//  BUG: the two cel handles are drawn as the Show function's ARCHIVE_CellFileLoad left them, with no test; ARCHIVE_CellFileLoad answers 0 for a
//  file that will not open and the handle stays 0. LATENT: both files ship loose in this install (data\global\ui\BIGMENU\popupok.dc6 and
//  data\global\ui\MENU\okcancelbtn.dc6).
BOOL __stdcall INSERTDISC_DrawInsertDiscPrompt(int32_t nUnused)
{
    UNREFERENCED_PARAMETER(nUnused);

    MSG_SendPing6C();
    INSERTDISC_DrawInsertDiscPromptScene();
    SOUNDHDR_PumpPreload();
    return gbDiscPromptDone != 0;
}

//1.10f: D2Client.0x6FADC180
//  The prompt's frame, out of line: WINDOW_GetState, StartDraw, the panel, the cursor, EndScene -- the body INSERTDISC_DrawInsertDiscPrompt
//  inlines, without the ping, the sound pump and the button test. It answers what the last call it made answered: D2GFX_WINDOW_GetState()'s
//  non-zero on the first exit, D2GFX_StartDraw()'s zero on the second, and D2GFX_EndScene()'s from the tail jump. No stack argument.
//  BUG: this function is never called. Its one reference is slot 1 of gafnMessageCallbacks, which GAME_SetActiveMessageCallback(1) installs as
//  gpfnActiveMessageCallback and which only GAME_UpdateFrame calls -- the outer pump's idle function, which does not run while a prompt pumps
//  its own messages; the Show functions install slot 1 just before their nested pump and slot 0 just after it, and the nested pump calls only
//  its own idle function. UNREACHABLE: gpfnActiveMessageCallback is read only in GAME_UpdateFrame.
int32_t __cdecl INSERTDISC_DrawInsertDiscPromptScene()
{
    int32_t nResult = D2GFX_WINDOW_GetState();
    if (nResult)
        return nResult;
    nResult = D2GFX_StartDraw(1, 0, 0, 0);
    if (!nResult)
        return nResult;

    D2GFX_ClearScreen(0);
    D2WIN_10179();

    //  rep stosd of 12h dwords over the frame's one local
    D2GfxDataStrc tCelData;
    memset(&tCelData, 0, sizeof(tCelData));
    tCelData.pCellFile = gpDiscPromptFrameCel;
    tCelData.nDirection = 0;
    tCelData.nFrame = 0;
    D2GFX_TEXTURE_CelDraw(&tCelData, gnBorderXDifference + 188, 328 - gnBorderYDifference, 0xFFFFFFFF, 5, NULL);
    tCelData.nFrame = 1;
    D2GFX_TEXTURE_CelDraw(&tCelData, gnBorderXDifference + 444, 328 - gnBorderYDifference, 0xFFFFFFFF, 5, NULL);

    const Font nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT16);
    CHAR_DrawTextCentered(gnScreenWidth / 2 - 132, 190 - gnBorderYDifference, gnBorderXDifference + 452, D2LANG_GetStringFromTblIndex(STR_IDX_5168_Please_Insert_the_Diablo_II), 0);
    const Unicode *wszDiscName = FOG_IsExpansion() ? D2LANG_GetStringFromTblIndex(STR_IDX_21880_x)
                                     : D2LANG_GetStringFromTblIndex(STR_IDX_5169_Play_Disc);
    CHAR_DrawTextCentered(gnScreenWidth / 2 - 132, 205 - gnBorderYDifference, gnBorderXDifference + 452, wszDiscName, 0);
    D2WIN_10127_SetFont(nPrevFont);

    memset(&tCelData, 0, sizeof(tCelData));
    tCelData.nDirection = 0;
    tCelData.pCellFile = gpDiscPromptButtonCel;
    tCelData.nFrame = gnDiscPromptButtonFrame;
    D2GFX_TEXTURE_CelDraw(&tCelData, gnBorderXDifference + 272, 318 - gnBorderYDifference, 0xFFFFFFFF, 5, NULL);

    const Font nPrevFont2 = D2WIN_10127_SetFont(D2FONT_FONT16);
    CHAR_DrawTextCentered(gnBorderXDifference + 272, 310 - gnBorderYDifference, gnBorderXDifference + 368, D2LANG_GetStringFromTblIndex(STR_IDX_5102_OK), 0);
    D2WIN_10127_SetFont(nPrevFont2);
    CURSOR_DrawCursorDefault();
    return D2GFX_EndScene();
}

//  Escape is a KEYDOWN record and so is Enter; the two button records are plain messages. The original carries the kinds as the literals 0
//  and 3.
static_assert(D2_WINPROC_MESSAGE == 0 && D2_WINPROC_KEYDOWN == 3,
              "D2WindowProcCallbackType: the handler tables carry 0 and 3");

//1.10f: D2Client.0x6FADC390
//  Shows the prompt; handed to D2Win's archive loaders by VIDEO_PlayCinematic, which retries while it answers non-zero.
//  INSERTDISC_ShowInsertPlayDiscPrompt and INSERTDISC_ShowInsertExpansionDiscPrompt are the same function twice (two bodies in the original,
//  identical instruction for instruction); which one D2Win calls is the slot it was handed in, and the text they show is
//  INSERTDISC_DrawInsertDiscPrompt's choice.
//  The sequence: clear gbDiscPromptDone, D2WIN_10176(0), load the three cels -- this prompt's frame, the cinematics prompt's frame and the
//  button, the first two from the same popupok file (only the cinematics prompt's two draws read gpCinematicsPromptFrameCel) -- D2WIN_10177(0),
//  GAME_SetActiveMessageCallback(1), register the table, pump with INSERTDISC_DrawInsertDiscPrompt until it answers non-zero, unregister,
//  GAME_SetActiveMessageCallback(0), release each cel it holds, restore the palette, and answer 1 whatever the button did.
//  GAME_SetActiveMessageCallback takes both callbacks from gafnMessageCallbacks, whose one reader's three callers are the three Show
//  functions; slot 0, restored at the end, is Game.cpp's GAME_DrawGameScene.
int32_t __stdcall INSERTDISC_ShowInsertPlayDiscPrompt()
{
    gbDiscPromptDone = 0;
    D2WIN_10176(0);
    gpDiscPromptFrameCel = ARCHIVE_CellFileLoad(pszName, 0);
    gpCinematicsPromptFrameCel = ARCHIVE_CellFileLoad(pszName, 0);
    gpDiscPromptButtonCel = ARCHIVE_CellFileLoad((char *)szOkCancelBtn, 0);
    D2WIN_10177(0);
    GAME_SetActiveMessageCallback(1);
    WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gInsertDiscMessageHandlers, 4);
    WNDPROC_WindowRunMessagePump(INSERTDISC_DrawInsertDiscPrompt);
    WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gInsertDiscMessageHandlers, 4);
    GAME_SetActiveMessageCallback(0);
    if (gpDiscPromptFrameCel)
    {
        ARCHIVE_CellFileFree(gpDiscPromptFrameCel);
        gpDiscPromptFrameCel = 0;
    }
    if (gpCinematicsPromptFrameCel)
    {
        ARCHIVE_CellFileFree(gpCinematicsPromptFrameCel);
        gpCinematicsPromptFrameCel = 0;
    }
    if (gpDiscPromptButtonCel)
    {
        ARCHIVE_CellFileFree(gpDiscPromptButtonCel);
        gpDiscPromptButtonCel = 0;
    }
    UI_LoadScreenLoadPalette();
    return 1;
}

//1.10f: D2Client.0x6FADC470
//  INSERTDISC_ShowInsertPlayDiscPrompt again, instruction for instruction (above).
int32_t __stdcall INSERTDISC_ShowInsertExpansionDiscPrompt()
{
    gbDiscPromptDone = 0;
    D2WIN_10176(0);
    gpDiscPromptFrameCel = ARCHIVE_CellFileLoad(pszName, 0);
    gpCinematicsPromptFrameCel = ARCHIVE_CellFileLoad(pszName, 0);
    gpDiscPromptButtonCel = ARCHIVE_CellFileLoad((char *)szOkCancelBtn, 0);
    D2WIN_10177(0);
    GAME_SetActiveMessageCallback(1);
    WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gInsertDiscMessageHandlers, 4);
    WNDPROC_WindowRunMessagePump(INSERTDISC_DrawInsertDiscPrompt);
    WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gInsertDiscMessageHandlers, 4);
    GAME_SetActiveMessageCallback(0);
    if (gpDiscPromptFrameCel)
    {
        ARCHIVE_CellFileFree(gpDiscPromptFrameCel);
        gpDiscPromptFrameCel = 0;
    }
    if (gpCinematicsPromptFrameCel)
    {
        ARCHIVE_CellFileFree(gpCinematicsPromptFrameCel);
        gpCinematicsPromptFrameCel = 0;
    }
    if (gpDiscPromptButtonCel)
    {
        ARCHIVE_CellFileFree(gpDiscPromptButtonCel);
        gpDiscPromptButtonCel = 0;
    }
    UI_LoadScreenLoadPalette();
    return 1;
}


//1.10f: D2Client.0x6FADC550
//  The four handlers of the play / expansion prompt. The button is 96 by 32: its left edge at gnBorderXDifference + 272, its bottom edge at
//  310 - gnBorderYDifference. The hit test is inclusive on all four sides and signed (jl/jg), reads the border globals before the mouse and
//  the mouse x before the y -- the shape GAME_Vanquished's two handlers have with other edges. The same add r, 60h / lea r, [r-20h] pair
//  stands at several sites in the module and no function consists of it, so each site writes it in place.
//  Three of the four claim the message and end in a tail jump to STORM_SMsgBreakHandlerChain; Escape does neither. The press arms
//  gbDiscPromptButtonDown, one flag for both prompts' buttons.
//  A press. On the button it shows the pressed frame, arms gbDiscPromptButtonDown and plays sound 4; anywhere else it resets the mouse button
//  state (GAME_ResetMouseButtonState). Either way bReturn = 1 and lResult = 0, in that order on both paths. SOUNDINST_PlaySound's three
//  stack arguments are 0 each.
void __stdcall INSERTDISC_OnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    const int32_t nButtonBottom = 310 - gnBorderYDifference;
    const int32_t nButtonLeft = gnBorderXDifference + 272;
    const int32_t nMouseX = CURSOR_GetMouseX();
    const int32_t nMouseY = CURSOR_GetMouseY();
    if (nMouseX >= nButtonLeft && nMouseX <= nButtonLeft + 96 && nMouseY >= nButtonBottom - 32 && nMouseY <= nButtonBottom)
    {
        gnDiscPromptButtonFrame = 1;
        gbDiscPromptButtonDown = 1;
        SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
    }
    else
    {
        GAME_ResetMouseButtonState();
    }
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FADC5F0
//  A release. The pressed frame ends wherever it happens. If a press armed the button and the release is on it, the prompt is done and the
//  flag is cleared; otherwise the mouse button state is reset. Both paths store lResult before bReturn.
//  BUG: a missed release leaves the button armed. Only the hit path clears gbDiscPromptButtonDown (Enter is the one other clear), so a press on
//  the button dragged off and released leaves it 1, the press handler's miss path does not clear it, and a later release on the button fires
//  the prompt whatever press began it. No Show function clears the flag, so the armed state also carries into the next prompt, of either
//  kind -- the cinematics prompt's handlers share it. REACHABLE and benign: the fire it allows is an OK, which asks for the disc again.
void __stdcall INSERTDISC_OnLButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    gnDiscPromptButtonFrame = 0;
    if (gbDiscPromptButtonDown)
    {
        const int32_t nButtonBottom = 310 - gnBorderYDifference;
        const int32_t nButtonLeft = gnBorderXDifference + 272;
        const int32_t nMouseX = CURSOR_GetMouseX();
        const int32_t nMouseY = CURSOR_GetMouseY();
        if (nMouseX >= nButtonLeft && nMouseX <= nButtonLeft + 96 && nMouseY >= nButtonBottom - 32 && nMouseY <= nButtonBottom)
        {
            gbDiscPromptDone = 1;
            gbDiscPromptButtonDown = 0;
            pMsg->lResult = 0;
            pMsg->bReturn = 1;
            STORM_SMsgBreakHandlerChain(pMsg);
            return;
        }
    }
    GAME_ResetMouseButtonState();
    pMsg->lResult = 0;
    pMsg->bReturn = 1;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FADC690
//  Enter: the prompt is done, the button disarmed, the mouse state reset, the message claimed.
void __stdcall INSERTDISC_OnEnter(SMSGHANDLER_PARAMS *pMsg)
{
    gbDiscPromptDone = 1;
    gbDiscPromptButtonDown = 0;
    GAME_ResetMouseButtonState();
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FADC6D0
//  Escape: one store and retn 4. It clears gbDiscPromptDone, which the Show function cleared on entry and which is 1 only between an OK or
//  Enter and the pump's next idle call (the pump idles only when no message is waiting), so it cancels nothing; at most it takes back an OK
//  given in the same pass of the pump, and the prompt stays.
//  BUG: it neither sets bReturn nor breaks the handler chain, so Storm passes the key on to whatever is registered under the prompt. Storm
//  dispatches WM_KEYDOWN's MESSAGE list before its KEY list, and in a game the MESSAGE list holds CMDTBL_OnKeyDown (a prompt comes up in
//  one through SCMD_HandleSrv61 -> QUESTS_PlayCinematicFromPacket -> VIDEO_PlayCinematic, which turns no key handler off), so with the keys
//  hooked for commands (gnKeyHookMode 1), ESC's command, CMDTBL_ActionEscMenu, has already closed the game menu, or the open panels, or
//  opened the menu (nothing under an NPC dialog or a popup) before this record is reached, and every other bound key runs its command too.
//  REACHABLE: what that leaves on screen is drawn when the prompt ends.
void __stdcall INSERTDISC_OnEscape(SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);
    gbDiscPromptDone = 0;
}

//  The cinematics prompt: the same frame with two buttons, OK and CANCEL, and the one prompt that can give up. Its second line is tbl 5170
//  "Cinematics Disc" while gnDiscPromptKind is 0 and tbl 21880 "Expansion Disc" otherwise -- the flag INSERTDISC_SetDiscPromptKind stores,
//  not FOG_IsExpansion(), which neither this prompt's draws call.
//  BUG: the "Expansion Disc" line is never drawn. VIDEO_PlayCinematic sets the kind to bExpansionDisc and hands this prompt to D2Win only
//  when that is FALSE. UNREACHABLE: INSERTDISC_SetDiscPromptKind is the kind's one writer, its two calls are both in VIDEO_PlayCinematic, and
//  the TRUE one precedes the play prompt, which reads FOG_IsExpansion(); VIDEO_PlayCinematic is this prompt's one caller.
//      frame     cel gpCinematicsPromptFrameCel -- loaded from the same popupok file as the play prompt's frame -- at the same two places
//      lines     as the play prompt's, the second from gnDiscPromptKind
//      buttons   cel gpDiscPromptButtonCel, frame gnDiscPromptButtonFrame at (x+206, 318-y) and gnDiscPromptButton2Frame at (x+338, 318-y);
//                tbl 5102 "OK" between x+206 and x+302 and tbl 5103 "CANCEL" between x+338 and x+434, both at 310-y
//  The button handlers set gbDiscPromptAccepted (OK, Enter) or gbDiscPromptCancelled (CANCEL, Escape); the draw answers either, which ends
//  the pump; the Show function answers NOT CANCELLED, which is the loader's "retry" -- so OK asks for the disc again, and CANCEL makes the
//  loader give up and VIDEO_PlayCinematic skip the cinematic. Only the draw reads gbDiscPromptAccepted.

//1.10f: D2Client.0x6FADC6E0
//  The cinematics prompt's idle function: ping, draw one frame, pump the sound preload, and answer whether either button has been taken --
//  gbDiscPromptAccepted tested, then gbDiscPromptCancelled, each jnz to mov eax, 1. The argument is the pump's idle counter, not read; retn 4.
//  The original inlines INSERTDISC_DrawInsertCinematicsDiscPromptScene here.
BOOL __stdcall INSERTDISC_DrawInsertCinematicsDiscPrompt(int32_t nUnused)
{
    UNREFERENCED_PARAMETER(nUnused);

    MSG_SendPing6C();
    INSERTDISC_DrawInsertCinematicsDiscPromptScene();
    SOUNDHDR_PumpPreload();
    return gbDiscPromptAccepted || gbDiscPromptCancelled;
}

//1.10f: D2Client.0x6FADC970
//  The two-button frame, out of line -- INSERTDISC_DrawInsertDiscPromptScene's counterpart, with the same three answers (WINDOW_GetState's
//  non-zero, StartDraw's zero, EndScene's from the tail jump).
//  BUG: this function is never called. Its one reference is slot 2 of gafnMessageCallbacks, which only GAME_UpdateFrame calls. UNREACHABLE:
//  slot 2 is installed only around the cinematics prompt's nested pump, when GAME_UpdateFrame does not run.
//  gnDiscPromptButton2Frame is loaded before the first button is drawn: the compiler cannot carry a global's load across a call into another
//  module, so the source read it first, and so does this C.
int32_t __cdecl INSERTDISC_DrawInsertCinematicsDiscPromptScene()
{
    int32_t nResult = D2GFX_WINDOW_GetState();
    if (nResult)
        return nResult;
    nResult = D2GFX_StartDraw(1, 0, 0, 0);
    if (!nResult)
        return nResult;

    D2GFX_ClearScreen(0);
    D2WIN_10179();

    D2GfxDataStrc tCelData;
    memset(&tCelData, 0, sizeof(tCelData));
    tCelData.pCellFile = gpCinematicsPromptFrameCel;
    tCelData.nDirection = 0;
    tCelData.nFrame = 0;
    D2GFX_TEXTURE_CelDraw(&tCelData, gnBorderXDifference + 188, 328 - gnBorderYDifference, 0xFFFFFFFF, 5, NULL);
    tCelData.nFrame = 1;
    D2GFX_TEXTURE_CelDraw(&tCelData, gnBorderXDifference + 444, 328 - gnBorderYDifference, 0xFFFFFFFF, 5, NULL);

    const Font nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT16);
    CHAR_DrawTextCentered(gnScreenWidth / 2 - 132, 190 - gnBorderYDifference, gnBorderXDifference + 452, D2LANG_GetStringFromTblIndex(STR_IDX_5168_Please_Insert_the_Diablo_II), 0);
    const D2C_StringIndices nDiscName = gnDiscPromptKind ? STR_IDX_21880_x : STR_IDX_5170_Cinematics_Disc;
    CHAR_DrawTextCentered(gnScreenWidth / 2 - 132, 205 - gnBorderYDifference, gnBorderXDifference + 452, D2LANG_GetStringFromTblIndex(nDiscName), 0);
    D2WIN_10127_SetFont(nPrevFont);

    memset(&tCelData, 0, sizeof(tCelData));
    tCelData.pCellFile = gpDiscPromptButtonCel;
    const int32_t nCancelFrame = gnDiscPromptButton2Frame;
    tCelData.nFrame = gnDiscPromptButtonFrame;
    tCelData.nDirection = 0;
    D2GFX_TEXTURE_CelDraw(&tCelData, gnBorderXDifference + 206, 318 - gnBorderYDifference, 0xFFFFFFFF, 5, NULL);
    tCelData.nFrame = nCancelFrame;
    D2GFX_TEXTURE_CelDraw(&tCelData, gnBorderXDifference + 338, 318 - gnBorderYDifference, 0xFFFFFFFF, 5, NULL);

    const Font nPrevFont2 = D2WIN_10127_SetFont(D2FONT_FONT16);
    CHAR_DrawTextCentered(gnBorderXDifference + 206, 310 - gnBorderYDifference, gnBorderXDifference + 302, D2LANG_GetStringFromTblIndex(STR_IDX_5102_OK), 0);
    CHAR_DrawTextCentered(gnBorderXDifference + 338, 310 - gnBorderYDifference, gnBorderXDifference + 434, D2LANG_GetStringFromTblIndex(STR_IDX_5103_CANCEL), 0);
    D2WIN_10127_SetFont(nPrevFont2);
    CURSOR_DrawCursorDefault();
    return D2GFX_EndScene();
}


//1.10f: D2Client.0x6FADCBD0
//  The play prompt's sequence with the cinematics prompt's pieces: both answers cleared (gbDiscPromptCancelled first),
//  GAME_SetActiveMessageCallback(2), gCineDiscMessageHandlers, the pump on INSERTDISC_DrawInsertCinematicsDiscPrompt. The answer is read
//  straight after the pump, before the unregister: 1 unless CANCEL or Escape was taken.
//  BUG: a pass of the pump that sets both answers -- OK and then Escape before the next idle call -- answers 0: the cancel wins whichever
//  came first, where the play prompt lets the Escape take the OK back. REACHABLE and benign.
int32_t __stdcall INSERTDISC_ShowInsertCinematicsDiscPrompt()
{
    gbDiscPromptCancelled = 0;
    gbDiscPromptAccepted = 0;
    D2WIN_10176(0);
    gpDiscPromptFrameCel = ARCHIVE_CellFileLoad(pszName, 0);
    gpCinematicsPromptFrameCel = ARCHIVE_CellFileLoad(pszName, 0);
    gpDiscPromptButtonCel = ARCHIVE_CellFileLoad((char *)szOkCancelBtn, 0);
    D2WIN_10177(0);
    GAME_SetActiveMessageCallback(2);
    WNDPROC_RegisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gCineDiscMessageHandlers, 4);
    WNDPROC_WindowRunMessagePump(INSERTDISC_DrawInsertCinematicsDiscPrompt);
    const int32_t bRetry = gbDiscPromptCancelled == 0;
    WNDPROC_UnregisterMessageHandlers(D2GFX_WINDOW_GetWindow(), gCineDiscMessageHandlers, 4);
    GAME_SetActiveMessageCallback(0);
    if (gpDiscPromptFrameCel)
    {
        ARCHIVE_CellFileFree(gpDiscPromptFrameCel);
        gpDiscPromptFrameCel = 0;
    }
    if (gpCinematicsPromptFrameCel)
    {
        ARCHIVE_CellFileFree(gpCinematicsPromptFrameCel);
        gpCinematicsPromptFrameCel = 0;
    }
    if (gpDiscPromptButtonCel)
    {
        ARCHIVE_CellFileFree(gpDiscPromptButtonCel);
        gpDiscPromptButtonCel = 0;
    }
    UI_LoadScreenLoadPalette();
    return bRetry;
}


//1.10f: D2Client.0x6FADCCC0
//  The cinematics prompt's four handlers. Two buttons 96 by 32 on one baseline, OK's left edge at gnBorderXDifference + 206 and CANCEL's at
//  + 338, both bottoms at 310 - gnBorderYDifference; the play prompt's test for each, and each test reads the border globals and then the
//  mouse afresh -- two GetMouseX/GetMouseY pairs when the first test misses.
//  A press: OK shows its pressed frame, else CANCEL shows its own; either way the tail both share arms gbDiscPromptButtonDown (before the
//  sound), plays sound 4 and claims the message (bReturn, then lResult 0). A press on neither resets the mouse button state and claims it too.
void __stdcall INSERTDISC_CineDiscOnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    const int32_t nOkBottom = 310 - gnBorderYDifference;
    const int32_t nOkLeft = gnBorderXDifference + 206;
    int32_t nMouseX = CURSOR_GetMouseX();
    int32_t nMouseY = CURSOR_GetMouseY();
    if (nMouseX >= nOkLeft && nMouseX <= nOkLeft + 96 && nMouseY >= nOkBottom - 32 && nMouseY <= nOkBottom)
    {
        gnDiscPromptButtonFrame = 1;
    }
    else
    {
        const int32_t nCancelBottom = 310 - gnBorderYDifference;
        const int32_t nCancelLeft = gnBorderXDifference + 338;
        nMouseX = CURSOR_GetMouseX();
        nMouseY = CURSOR_GetMouseY();
        if (nMouseX < nCancelLeft || nMouseX > nCancelLeft + 96 || nMouseY < nCancelBottom - 32 || nMouseY > nCancelBottom)
        {
            GAME_ResetMouseButtonState();
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
            STORM_SMsgBreakHandlerChain(pMsg);
            return;
        }
        gnDiscPromptButton2Frame = 1;
    }
    gbDiscPromptButtonDown = 1;
    SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FADCDB0
//  A release. Both pressed frames end wherever it happens. If a press armed a button, the release is tested against OK and then, whatever
//  that answered, against CANCEL, each hit taking its answer and disarming; then the message is claimed (lResult, then bReturn). If none was
//  armed, the mouse button state is reset and the message claimed.
//  BUG: a release on neither button leaves the press armed and the mouse state as it was: the armed path has no miss arm -- no
//  GAME_ResetMouseButtonState and no disarm, where the play prompt's handler resets on a miss. The next release on a button takes it.
//  REACHABLE: the release, not the press, decides which button fires -- one flag arms both, so a press on OK released on CANCEL cancels even
//  without a miss -- and after a press released off both buttons, a later press anywhere released on either fires that one.
void __stdcall INSERTDISC_CineDiscOnLButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    gnDiscPromptButtonFrame = 0;
    gnDiscPromptButton2Frame = 0;
    if (gbDiscPromptButtonDown)
    {
        const int32_t nOkBottom = 310 - gnBorderYDifference;
        const int32_t nOkLeft = gnBorderXDifference + 206;
        int32_t nMouseX = CURSOR_GetMouseX();
        int32_t nMouseY = CURSOR_GetMouseY();
        if (nMouseX >= nOkLeft && nMouseX <= nOkLeft + 96 && nMouseY >= nOkBottom - 32 && nMouseY <= nOkBottom)
        {
            gbDiscPromptAccepted = 1;
            gbDiscPromptButtonDown = 0;
        }
        const int32_t nCancelBottom = 310 - gnBorderYDifference;
        const int32_t nCancelLeft = gnBorderXDifference + 338;
        nMouseX = CURSOR_GetMouseX();
        nMouseY = CURSOR_GetMouseY();
        if (nMouseX >= nCancelLeft && nMouseX <= nCancelLeft + 96 && nMouseY >= nCancelBottom - 32 && nMouseY <= nCancelBottom)
        {
            gbDiscPromptCancelled = 1;
            gbDiscPromptButtonDown = 0;
        }
        pMsg->lResult = 0;
        pMsg->bReturn = 1;
        STORM_SMsgBreakHandlerChain(pMsg);
    }
    else
    {
        GAME_ResetMouseButtonState();
        pMsg->lResult = 0;
        pMsg->bReturn = 1;
        STORM_SMsgBreakHandlerChain(pMsg);
    }
}

//1.10f: D2Client.0x6FADCEB0
//  Enter: OK -- accepted, disarmed, the mouse state reset, the message claimed.
void __stdcall INSERTDISC_CineDiscOnEnter(SMSGHANDLER_PARAMS *pMsg)
{
    gbDiscPromptAccepted = 1;
    gbDiscPromptButtonDown = 0;
    GAME_ResetMouseButtonState();
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FADCEF0
//  Escape: CANCEL -- one store and retn 4. Unlike the play prompt's Escape this one does cancel: the draw answers on gbDiscPromptCancelled and
//  the Show function returns 0.
//  BUG: like it, this neither sets bReturn nor breaks the chain, so the key goes on to whatever is registered under the prompt -- and in a
//  game ESC's command has already run on the MESSAGE list (INSERTDISC_OnEscape): Escape cancels the cinematic and does what ESC does in the
//  game. REACHABLE.
void __stdcall INSERTDISC_CineDiscOnEscape(SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);
    gbDiscPromptCancelled = 1;
}

//1.10f: D2Client.0x6FADCF00
//  Stores the kind that selects the cinematics prompt's second line (0: tbl 5170 "Cinematics Disc", otherwise tbl 21880 "Expansion Disc");
//  called twice from VIDEO_PlayCinematic, with bExpansionDisc and with TRUE.
void __fastcall INSERTDISC_SetDiscPromptKind(int32_t nKind)
{
    gnDiscPromptKind = nKind;
}

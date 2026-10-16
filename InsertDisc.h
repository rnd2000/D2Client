//  InsertDisc.h -- the globals of InsertDisc.cpp: the two handler tables (0x6FB791E8, 0x6FB79218), the popupok path pszName (0x6FB79268) and
//  the disc prompts' state (0x6FBB36B0..0x6FBB36D4). Each is a C++17 inline variable, so every file that includes this header shares one
//  object. The ten from 0x6FBB36B0 on lie past the end of .data's raw data (0x6FB9B000): the loader zero-fills them, and ours are
//  zero-initialised.
//  The comment line "D2Client 0x6F..." above a global gives its address in the original. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB791E8
//  The "insert the disc" prompt's table -- 4 records (WM_LBUTTONDOWN, WM_LBUTTONUP, VK_ESCAPE, VK_RETURN), registered by both
//  INSERTDISC_ShowInsertPlayDiscPrompt and INSERTDISC_ShowInsertExpansionDiscPrompt, so one dialog serves both prompts.
inline D2WinCommandRegisterStrc gInsertDiscMessageHandlers[4] =
{
    { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, INSERTDISC_OnLButtonDown },
    { D2_WINPROC_MESSAGE, WM_LBUTTONUP, INSERTDISC_OnLButtonUp },
    { D2_WINPROC_KEYDOWN, VK_ESCAPE, INSERTDISC_OnEscape },
    { D2_WINPROC_KEYDOWN, VK_RETURN, INSERTDISC_OnEnter },
};
static_assert(sizeof(gInsertDiscMessageHandlers) == 48,
              "gInsertDiscMessageHandlers: 4 records, 48 bytes, 0x6FB791E8");

//  D2Client 0x6FB79218
//  The cinematics prompt's table -- 4 records of the same kinds as gInsertDiscMessageHandlers, with different handlers; registered and
//  unregistered by INSERTDISC_ShowInsertCinematicsDiscPrompt, whose one caller is VIDEO_PlayCinematic.
inline D2WinCommandRegisterStrc gCineDiscMessageHandlers[4] =
{
    { D2_WINPROC_MESSAGE, WM_LBUTTONDOWN, INSERTDISC_CineDiscOnLButtonDown },
    { D2_WINPROC_MESSAGE, WM_LBUTTONUP, INSERTDISC_CineDiscOnLButtonUp },
    { D2_WINPROC_KEYDOWN, VK_ESCAPE, INSERTDISC_CineDiscOnEscape },
    { D2_WINPROC_KEYDOWN, VK_RETURN, INSERTDISC_CineDiscOnEnter },
};
static_assert(sizeof(gCineDiscMessageHandlers) == 48,
              "gCineDiscMessageHandlers: 4 records, 48 bytes, 0x6FB79218");

//  D2Client 0x6FB79268
inline char pszName[31] = "Data\\Global\\UI\\Bigmenu\\popupok";
static_assert(sizeof(pszName) == 31, "pszName: the item's 31 bytes in the .i64");

//  D2Client 0x6FBB36B0
//  Selects which disc name INSERTDISC_DrawInsertCinematicsDiscPromptScene and its copy inlined in INSERTDISC_DrawInsertCinematicsDiscPrompt
//  print on the second line, in place of the FOG_IsExpansion() test the other two use: 0 is tbl 5170 "Cinematics Disc", anything else 21880
//  "Expansion Disc". Written by INSERTDISC_SetDiscPromptKind.
inline int32_t gnDiscPromptKind;
static_assert(sizeof(gnDiscPromptKind) == 4, "gnDiscPromptKind: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB36B4
//  Cel handle for the two halves of the disc prompt's frame, drawn at x+188 and x+444. Used by INSERTDISC_DrawInsertDiscPrompt.
inline D2CellFileStrc *gpDiscPromptFrameCel;
static_assert(sizeof(gpDiscPromptFrameCel) == 4, "gpDiscPromptFrameCel: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB36B8
//  Cel handle for the OK / CANCEL buttons of the disc prompts.
inline D2CellFileStrc *gpDiscPromptButtonCel;
static_assert(sizeof(gpDiscPromptButtonCel) == 4, "gpDiscPromptButtonCel: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB36BC
//  Frame cel used by the two-button variants (INSERTDISC_DrawInsertCinematicsDiscPrompt and INSERTDISC_DrawInsertCinematicsDiscPromptScene),
//  distinct from gpDiscPromptFrameCel. All three Show functions load it from the same popupok file as gpDiscPromptFrameCel and read it to
//  free it; only the cinematics prompt's two draws read it otherwise.
inline D2CellFileStrc *gpCinematicsPromptFrameCel;
static_assert(sizeof(gpCinematicsPromptFrameCel) == 4, "gpCinematicsPromptFrameCel: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB36C0
//  The pressed-button state of the disc prompts, one flag shared by both prompts' buttons: armed by INSERTDISC_OnLButtonDown and
//  INSERTDISC_CineDiscOnLButtonDown, checked by the two release handlers, cleared by a hit release and by Enter (INSERTDISC_OnEnter). It is
//  not cleared on entry: no Show function clears it. The same armed / checked / cleared shape as gbNewStatsButtonDown.
inline BOOL gbDiscPromptButtonDown;
static_assert(sizeof(gbDiscPromptButtonDown) == 4, "gbDiscPromptButtonDown: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB36C4
//  Frame index into gpDiscPromptButtonCel for the first button -- its pressed / released state.
inline int32_t gnDiscPromptButtonFrame;
static_assert(sizeof(gnDiscPromptButtonFrame) == 4, "gnDiscPromptButtonFrame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB36C8
//  Frame index for the second button of the two-button prompts (CANCEL).
inline int32_t gnDiscPromptButton2Frame;
static_assert(sizeof(gnDiscPromptButton2Frame) == 4, "gnDiscPromptButton2Frame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB36CC
//  Set when the play / expansion prompt's button has been clicked. INSERTDISC_DrawInsertDiscPrompt is the one draw function that returns on
//  it (`!= 0`), which ends the caller's per-frame loop. The cinematics prompt's draw returns on gbDiscPromptAccepted and
//  gbDiscPromptCancelled instead; the two scene draws answer D2GFX_EndScene()'s value on the path that draws (a tail jump), and their early
//  exits return D2GFX_WINDOW_GetState's or D2GFX_StartDraw's eax.
inline int32_t gbDiscPromptDone;
static_assert(sizeof(gbDiscPromptDone) == 4, "gbDiscPromptDone: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB36D0
//  The cinematics prompt was accepted. Set to 1 by INSERTDISC_CineDiscOnEnter and by INSERTDISC_CineDiscOnLButtonUp when the release lands in
//  the OK rect. INSERTDISC_DrawInsertCinematicsDiscPrompt is the idle callback of a WNDPROC_WindowRunMessagePump, so returning non-zero is
//  what ends the prompt: it tests this and gbDiscPromptCancelled in turn, and the pump exits on either. The play and expansion prompts hand
//  the same pump INSERTDISC_DrawInsertDiscPrompt as the idle callback, which polls one flag, gbDiscPromptDone.
inline BOOL gbDiscPromptAccepted;
static_assert(sizeof(gbDiscPromptAccepted) == 4, "gbDiscPromptAccepted: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB36D4
//  The cinematics prompt was cancelled: set to 1 by INSERTDISC_CineDiscOnEscape and by INSERTDISC_CineDiscOnLButtonUp when the release lands
//  in the Cancel rect. Cleared by INSERTDISC_ShowInsertCinematicsDiscPrompt before it registers the handlers and starts the pump, and tested
//  by that same function after the pump returns, which is how the caller learns which way the prompt ended. Its twin gbDiscPromptAccepted is
//  only read by the idle callback: both end the pump, and only the cancel is reported back.
inline BOOL gbDiscPromptCancelled;
static_assert(sizeof(gbDiscPromptCancelled) == 4, "gbDiscPromptCancelled: the item's 4 bytes in the .i64");

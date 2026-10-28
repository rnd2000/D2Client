#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "Draw.h"
#include "WINMAIN.h"
#include "CoreArchive.h"
#include "Restart.h"

//  Restart.cpp -- the ESC menu of D2Client.dll 1.10: the main, options, sound, video and automap pages with their row callbacks, the key and
//  mouse handlers of the menu, the loaders and unloaders of the page images, and ESCMENU_Draw.
//  The original source file is UI\Restart.cpp (three asserts in this unit name it); it is named for what the menu is for, leaving the game.
//  Address range 0x6FB06FB0..0x6FB08D79.

//  Registry names this half of the unit needs. In the original "AutoMapMode" is in this file's own literal block and "AutoMapFade" is in
//  automap.cpp's, so the fade row references a string that belongs to the file it configures. Only the contents are read, so one copy serves.
static const char szDiabloIIKey[] = "Diablo II";
static const char szAutoMapMode[] = "AutoMapMode";
static const char szAutoMapFade[] = "AutoMapFade";

//  __FILE__ as this unit's asserts push it (the directory is spelled `UI`), and the two data roots the image loaders format into.
static const char szRestartCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UI\\Restart.cpp";
static const char aDataLocal[] = "DATA\\LOCAL";
static const char aDataGlobal[] = "DATA\\GLOBAL";


//  gEscmenuMessageHandlers, 7 records registered by UI_RegisterPanelHandlers under UI_ESCMENU:
//
//      WM_LBUTTONDOWN  ESCMENU_OnLButtonDown      VK_LEFT   OnLeftKey
//      WM_LBUTTONUP    ESCMENU_OnLButtonUp        VK_RIGHT  OnRightKey
//      VK_DOWN         ESCMENU_OnDownKey          VK_RETURN OnEnter
//      VK_UP           ESCMENU_OnUpKey
//
//  All seven claim the message, which breaks Storm's chain: six by a tail jump after rewriting pMsg into their own argument slot,
//  OnLButtonDown and OnLButtonUp by calling it and then `retn 4`.
//
//  D2ClientEscMenuItemStrc is 1360 bytes (`shl 4 / add / lea [x+x*4] / shl 4` is x * 1360). The fields these handlers touch:
//
//      +0x000  dwType        1 or 2; -1 is a separator
//      +0x110  pfnIsEnabled  int __cdecl(); a NULL pointer means yes
//      +0x114  pfnOnChange   void __fastcall(pItem, int)
//      +0x120  dwCount       how many settings the row cycles through
//      +0x124  nValue        which one
//
//  `sgptMenuInfo->nItemCount` is the row count and `sgptMenuItem` the array; both point into whichever page ESCMENU_SetPage last built.
//
//  The two row types differ only at the ends of the range, and LEFT and RIGHT mirror each other:
//
//              type 1 (wraps)                type 2 (clamps)
//      LEFT    below 0 -> dwCount - 1        below 0 -> 0
//      RIGHT   above max -> 0                above max -> dwCount - 1
//
//  All four tests are the same unsigned compare on a value that has just been stepped: `(unsigned)nValue > (unsigned)(dwCount - 1)`.
//  Decrementing 0 gives 0xFFFFFFFF, which is above the maximum, so one comparison catches both ends. The old value is saved before the change
//  and compared after, so pfnOnChange and the click sound fire only on a real move: LEFT on a type-2 row already at 0 is silent.
//
//  pfnOnChange takes two arguments, (pRow, edx). ESCMENUITEMFN's second parameter is an int, so the two call sites that pass a pointer are
//  written with a cast. ESCMENU_ActivateSelectedRow passes pMsg at both of its calls (ESCMENU_OnExitGame reads pMsg->hWnd); ESCMENU_DragSlider,
//  ESCMENU_OnLeftKey and ESCMENU_OnRightKey pass 0.

static_assert(sizeof(D2ClientEscMenuItemStrc) == 1360,
              "the row index compiles to `shl 4 / add / lea [x+x*4] / shl 4`, "
              "which is x * 1360, and every one of the five reloads in "
              "OnLeftKey repeats it");
static_assert(offsetof(D2ClientEscMenuItemStrc, pfnIsEnabled) == 0x110
              && offsetof(D2ClientEscMenuItemStrc, pfnOnChange) == 0x114
              && offsetof(D2ClientEscMenuItemStrc, dwCount) == 0x120
              && offsetof(D2ClientEscMenuItemStrc, nValue) == 0x124,
              "the four fields these seven touch, and the offsets Binary "
              "Ninja prints as +0x110, +0x114, +0x120 and +0x124");
static_assert(offsetof(D2ClientEscMenuInfoStrc, nItemCount) == 0,
              "`mov edx, [eax]` on sgptMenuInfo is the row count, so it is the "
              "first field");


//  gEscMenuUiWindows, 38 records of { bCloseOnEscMenu, bRestoreFlag, nSavedState }; the record index is the UI window id (every loop over it
//  steps `inc edi` for the id beside `add esi, 0Ch` for the record and stops at `cmp esi, 1C8h`):
//
//      bCloseOnEscMenu  clear for UI_GAME (0) and UI_ESCMENU (9) only
//      bRestoreFlag     set for UI_NEWSTATS 6, UI_NEWSKILLS 7, UI_AUTOMAP 10, UI_QUESTLOG 17, UI_MINIPANEL 21 and UI_HIREICONS 35
//      nSavedState      zero; ESCMENU_SetPage is its only writer
//
//  Opening the menu with bCloseUiWindows set turns off every window that is on except ids 0 and 9 and records the state of six of them;
//  closing it with bRestorePanels set turns back on whichever of those six were on. Nothing resets nSavedState but the next such opening.
//  ESCMENU_SetPage, ESCMENU_CloseMenu, ESCMENU_OnExitGame and ESCMENU_OnReturnToGame touch the table; MINIPANEL_SaveAndUnload reads record 21
//  directly.

static_assert(offsetof(D2ClientEscMenuUiWindowStrc, bCloseOnEscMenu) == 0
              && offsetof(D2ClientEscMenuUiWindowStrc, bRestoreFlag) == 4
              && offsetof(D2ClientEscMenuUiWindowStrc, nSavedState) == 8,
              "SetPage reads gEscMenuUiWindows[esi], +4[esi] and +8[esi] in "
              "that order, and the two flags are the pair a transcription "
              "could swap -- D2Structs.h pins only the 12-byte size");
static_assert(offsetof(SMSGHANDLER_PARAMS, hWnd) == 0,
              "OnExitGame's `mov eax, [esi]` is the window SendMessageA gets "
              "WM_CLOSE, so hWnd is the record's first field");
static_assert(UI_CONFIG == 11,
              "OpenConfigPanel's second toggle loads `mov ecx, 0Bh`, the "
              "configuration screen");


//1.10f: D2Client.0x6FB06FB0
//  The "Options" row of the main page, and the "SPrevious" row of every page below it: `mov edx, 1 / xor ecx, ecx / jmp ESCMENU_SetPage`.
//  The six rows that move between pages are command rows (dwType 0), each reached only through a pfnOnChange slot:
//
//      GotoOptionsPage   SetPage(0, 1)
//      GotoSoundPage     SetPage(0, 2)   the options page
//      GotoVideoPage     SetPage(0, 3)   the options page
//      GotoAutomapPage   SetPage(0, 4)   the options page
//      GotoMainPage      SetPage(0, 0)   the options page
//      OpenConfigPanel   CloseMenu(FALSE), then UI_CONFIG on
//
//  Every page change passes bCloseUiWindows = 0, so moving around the menu never touches the record its opening made. There is no history:
//  the SPrevious row of the sound page and of both video and both automap tables is this function, and the options page's Previous is
//  GotoMainPage.
//  The six take the slot's ABI, (pItem, pMsg) in ecx and edx: ESCMENU_ActivateSelectedRow, the only dispatcher of a dwType 0 row, loads both
//  and discards eax, and none of the six reads either register before writing it.
void __fastcall ESCMENU_GotoOptionsPage(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pItem);
    UNREFERENCED_PARAMETER(pMsg);

    ESCMENU_SetPage(0, 1);
}


//1.10f: D2Client.0x6FB06FC0
//  The main page's "Exit" row, reached only through row 1's pfnOnChange slot in gEscMenuMainItems:
//
//      WINMAIN_SetAppModeContinue(FALSE)   WINMAIN_D2ClientEntrypoint will return 3 or 4 by game type, not 0
//      SendMessageA(hWnd, WM_CLOSE, 0, 0)  to the window the input came to
//      GAME_SetLeaveGameSent(1)
//      ESCMENU_CloseMenu(TRUE)             inlined in the original
//
//  SendMessageA returns only once the window procedure has handled WM_CLOSE, so that has happened before the leave flag is set; the order is
//  the original's. pItem arrives in ecx and is never read. pMsg is the Storm record ESCMENU_ActivateSelectedRow forwards, the only dispatch
//  that passes one, and only its hWnd is read.
void __fastcall ESCMENU_OnExitGame(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pItem);

    WINMAIN_SetAppModeContinue(FALSE);
    SendMessageA(pMsg->hWnd, WM_CLOSE, 0, 0);
    GAME_SetLeaveGameSent(1);
    ESCMENU_CloseMenu(TRUE);
}


//1.10f: D2Client.0x6FB07030
//  The main page's "ReturnToGame" row, reached only through row 2's pfnOnChange slot. The original inlines ESCMENU_CloseMenu(TRUE) here, with
//  the bRestorePanels test folded away, and nothing else. Neither argument is read, in either register. Row 2 is the main page's last row, so
//  it is the one ESCMENU_SetPage highlights when Esc opens the menu.
void __fastcall ESCMENU_OnReturnToGame(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pItem);
    UNREFERENCED_PARAMETER(pMsg);

    ESCMENU_CloseMenu(TRUE);
}


//1.10f: D2Client.0x6FB07080
//  `jmp SOUNDCHAN_IsSystemReady_2` -- the pfnIsEnabled of four rows: the options page's SoundOptions, and Sound, Music and NPCSpeech on the
//  sound page.
int32_t __cdecl j_SOUNDCHAN_IsSystemReady_2(void)
{
    return SOUNDCHAN_IsSystemReady_2();
}


//1.10f: D2Client.0x6FB07090
//  The "SoundOptions" row of the options page: SetPage(0, 2).
void __fastcall ESCMENU_GotoSoundPage(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pItem);
    UNREFERENCED_PARAMETER(pMsg);

    ESCMENU_SetPage(0, 2);
}


//1.10f: D2Client.0x6FB070A0
//  The "VideoOptions" row of the options page: SetPage(0, 3). Which of the two video tables the page shows is SetPage's FOG_IsExpansion
//  question, not this row's.
void __fastcall ESCMENU_GotoVideoPage(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pItem);
    UNREFERENCED_PARAMETER(pMsg);

    ESCMENU_SetPage(0, 3);
}


//1.10f: D2Client.0x6FB070B0
//  The "AutoMapOptions" row of the options page: SetPage(0, 4); the same FOG_IsExpansion choice of table is made inside SetPage.
void __fastcall ESCMENU_GotoAutomapPage(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pItem);
    UNREFERENCED_PARAMETER(pMsg);

    ESCMENU_SetPage(0, 4);
}


//1.10f: D2Client.0x6FB070C0
//  The "CfgOptions" row of the options page, the one row on any page that opens a different panel instead of a page. The original inlines
//  ESCMENU_CloseMenu(FALSE) here (its window-9 half, with no restore loop), so the menu closes without turning the hidden windows back on and
//  the configuration screen takes its place. The record the opening made waits, either for CONFIG_OnAcceptButton's CloseMenu(1) or for
//  CONFIG_OnCancelButton and CONFIG_OnDismissKey to put the menu back on page 1 with SetPage(0, 1).
void __fastcall ESCMENU_OpenConfigPanel(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pItem);
    UNREFERENCED_PARAMETER(pMsg);

    ESCMENU_CloseMenu(FALSE);
    UI_SetUIToggle(UI_CONFIG, UI_TURNON, 0);
}


//  The sound page: six rows between a title and SPrevious, ten callbacks of the page's own and six five-byte wrappers around other units'
//  functions (one of them placed above ESCMENU_GotoSoundPage), as gEscMenuSoundItems has them:
//
//      row  label      kind        IsEnabled         OnChange
//      1    Sound      slider, 21  IsSystemReady_2   SoundVolumeOnChange
//      2    Music      slider, 21  IsSystemReady_2   MusicVolumeOnChange
//      3    3DSound    2 choices   Get3DEnabled      j_Toggle3dSound
//      4    EAX        2 choices   GetEaxEnabled     j_ToggleEaxEffects
//      5    3DBias     slider, 21  Is3DEnabled       Sound3DBiasOnChange
//      6    NPCSpeech  3 choices   IsSystemReady_2   NpcSpeechOnChange
//
//  Each IsEnabled is the j_SOUND wrapper of that name. The options page's SoundOptions row is gated by j_SOUNDCHAN_IsSystemReady_2 too, so
//  without a sound system the page cannot be entered.
//
//  Every row's Refresh runs once when the menu loads: ESCMENU_LoadPageImages calls the +0x118 slot, reaching it backwards off `&dwCount`.
//  Rows 3 and 4 hold their Refresh in the +0x11C draw slot as well, so the 3D and EAX rows re-read the live switch on every draw, while the
//  sliders and NPCSpeech show what was loaded or last set.
//
//  The sliders (Sound, Music, 3DBias) are one shape three times, identical but for the D2Sound ordinal -- 21 steps into 0..100 and back:
//
//      OnChange   Set( (int)(100.0f / (unsigned)(dwCount - 1) * nValue) )
//      Refresh    nValue = (int)((unsigned)(dwCount - 1) * (Get() + 1) * 0.01f)
//
//  Both constants are floats: `fdivr dword ptr` on 100.0f (gfPercentDivisor) and `fmul dword ptr` on 0.01f (gfPercentScale). The unsigned
//  conversion is `fild qword` over a zero-extended dword; the nValue and Get() + 1 factors are `fimul dword`, signed, although the compiler
//  staged a zero high dword for both first. Refresh calls Get() before it reads dwCount, and a local keeps that order.
//  The + 1 is what makes the round trip land: 0.01f is 0.0099999998, so 20 * 100 * 0.01f is 19.9999996 and would truncate to 19. With it, the
//  5n a step writes reads back as step n for every n in 0..20. A stored value one short of a multiple of five still reads a step low (4
//  reads 0), and only a writer other than these rows could store one.
//
//  NPC speech: OnChange hands the raw choice to D2Sound (#10053), then re-reads the row and switches (`sub eax, 0 / jz / dec / jz / dec / jnz`,
//  MSVC's form for a dense three-case switch):
//
//      0   OBJECT_SetNpcSpeechAudio(1)   TEXT_SetDisplayDelayAndSave(0)
//      1   OBJECT_SetNpcSpeechAudio(0)   TEXT_SetDisplayDelayAndSave(1)
//      2   OBJECT_SetNpcSpeechAudio(1)   TEXT_SetDisplayDelayAndSave(1)
//
//  TEXT_SetDisplayDelayAndSave stores its argument in sgTextState.nTextDisplayDelay and saves it under "Text Display Beta", so the registry
//  keeps a flag for which choices 1 and 2 are the same 1, while D2Sound keeps the choice itself.
//  NpcSpeechRefresh loads the stored choice (#10052) into the row and, if SOUNDCHAN_IsSystemReady_2, runs OnChange's whole body, so a menu
//  load re-applies the stored choice, registry write included, whenever the sound system is up.
//
//  The six wrappers j_SOUNDCHAN_IsSystemReady_2, _Get3DEnabled, _Is3DEnabled, _GetEaxEnabled, _Toggle3dSound and _ToggleEaxEffects are a
//  `jmp` each into another unit, and every reference to them is a slot in the options or sound table, so they are written as the one-line
//  wrappers they compile to, each with its target's signature. Two of them sit in pfnOnChange slots, which call with ecx and edx loaded;
//  their `__cdecl ()` targets read neither.

static_assert(offsetof(D2ClientEscMenuItemStrc, pfnRefresh) == 0x118
              && offsetof(D2ClientEscMenuItemStrc, pfnOnDraw) == 0x11C
              && offsetof(D2ClientEscMenuItemStrc, dwCount)
                     - offsetof(D2ClientEscMenuItemStrc, pfnRefresh) == 8,
              "LoadPageImages calls pfnRefresh as `[ebx-8]` with ebx at dwCount, "
              "and ESCMENU_Draw calls pfnOnDraw as `[ecx+11Ch]`");


//1.10f: D2Client.0x6FB070F0
//  The Sound slider moved: rescale the step to 0..100 and hand it to D2Sound, which stores the setting itself (no registry write here,
//  unlike the automap rows).
void __fastcall ESCMENU_SoundVolumeOnChange(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    D2SOUND_10047_SetMasterVolumeSetting((int32_t)(gfPercentDivisor / (double)(pItem->dwCount - 1) * pItem->nValue));
}


//1.10f: D2Client.0x6FB07130
//  The Sound slider's value back into the row, at menu load. D2Sound is asked before dwCount is read, as the bytes order it.
void __fastcall ESCMENU_SoundVolumeRefresh(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nVolume;

    UNREFERENCED_PARAMETER(pMsg);

    nVolume = D2SOUND_10046_GetMasterVolumeSetting();
    pItem->nValue = (int32_t)((double)(pItem->dwCount - 1) * (nVolume + 1) * 0.01f);
}


//1.10f: D2Client.0x6FB07180
//  The Music slider moved: the Sound slider's shape into D2Sound #10049.
void __fastcall ESCMENU_MusicVolumeOnChange(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    D2SOUND_10049_SetMusicVolumeSetting((int32_t)(gfPercentDivisor / (double)(pItem->dwCount - 1) * pItem->nValue));
}


//1.10f: D2Client.0x6FB071C0
//  The Music slider's value back into the row, from D2Sound #10048.
void __fastcall ESCMENU_MusicVolumeRefresh(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nVolume;

    UNREFERENCED_PARAMETER(pMsg);

    nVolume = D2SOUND_10048_GetMusicVolumeSetting();
    pItem->nValue = (int32_t)((double)(pItem->dwCount - 1) * (nVolume + 1) * 0.01f);
}


//1.10f: D2Client.0x6FB07210
//  `jmp SOUNDCHAN_Get3DEnabled` -- the 3DSound row's pfnIsEnabled.
int32_t __cdecl j_SOUNDCHAN_Get3DEnabled(void)
{
    return SOUNDCHAN_Get3DEnabled();
}


//1.10f: D2Client.0x6FB07220
//  `jmp SOUNDCHAN_Toggle3dSound` -- the 3DSound row's pfnOnChange. The row has already stepped its nValue when this runs, and the toggle
//  takes no argument: SoundChan decides the new state itself, and Sound3DRefresh reads it back into the row on the next draw.
//  void: no caller reads what the original leaves in eax.
void __cdecl j_SOUNDCHAN_Toggle3dSound(void)
{
    SOUNDCHAN_Toggle3dSound();
}


//1.10f: D2Client.0x6FB07230
//  The 3D row's value is the live switch; runs at menu load and on every draw of the row, since both of its slots hold this.
void __fastcall ESCMENU_Sound3DRefresh(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    pItem->nValue = SOUNDCHAN_Is3DEnabled();
}


//1.10f: D2Client.0x6FB07240
//  `jmp SOUNDCHAN_Is3DEnabled` -- the 3DBias row's pfnIsEnabled, so the bias slider is live only while 3D sound is on.
BOOL __cdecl j_SOUNDCHAN_Is3DEnabled(void)
{
    return SOUNDCHAN_Is3DEnabled();
}


//1.10f: D2Client.0x6FB07250
//  The 3D bias slider moved: the Sound slider's shape into D2Sound #10051.
void __fastcall ESCMENU_Sound3DBiasOnChange(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    D2SOUND_10051_SetPositionalBiasSetting((int32_t)(gfPercentDivisor / (double)(pItem->dwCount - 1) * pItem->nValue));
}


//1.10f: D2Client.0x6FB07290
//  The 3D bias slider's value back into the row, from D2Sound #10050.
void __fastcall ESCMENU_Sound3DBiasRefresh(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nBias;

    UNREFERENCED_PARAMETER(pMsg);

    nBias = D2SOUND_10050_GetPositionalBiasSetting();
    pItem->nValue = (int32_t)((double)(pItem->dwCount - 1) * (nBias + 1) * 0.01f);
}


//1.10f: D2Client.0x6FB072E0
//  `jmp SOUNDCHAN_GetEaxEnabled` -- the EAX row's pfnIsEnabled.
int32_t __cdecl j_SOUNDCHAN_GetEaxEnabled(void)
{
    return SOUNDCHAN_GetEaxEnabled();
}


//1.10f: D2Client.0x6FB072F0
//  `jmp SOUNDCHAN_ToggleEaxEffects` -- the EAX row's pfnOnChange, the 3DSound row's shape.
void __cdecl j_SOUNDCHAN_ToggleEaxEffects(void)
{
    SOUNDCHAN_ToggleEaxEffects();
}


//1.10f: D2Client.0x6FB07300
//  The EAX row's value is the live switch; runs at menu load and on every draw.
void __fastcall ESCMENU_SoundEaxRefresh(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    pItem->nValue = SOUNDCHAN_IsEaxEnabled();
}


//1.10f: D2Client.0x6FB07310
//  The NPC speech row changed. A value past 2 has no arm, and only D2Sound hears about it.
void __fastcall ESCMENU_NpcSpeechOnChange(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    D2SOUND_10053_SetNpcSpeechSetting(pItem->nValue);

    switch (pItem->nValue)
    {
    case 0:
        OBJECT_SetNpcSpeechAudio(1);
        TEXT_SetDisplayDelayAndSave(0);
        break;

    case 1:
        OBJECT_SetNpcSpeechAudio(0);
        TEXT_SetDisplayDelayAndSave(1);
        break;

    case 2:
        OBJECT_SetNpcSpeechAudio(1);
        TEXT_SetDisplayDelayAndSave(1);
        break;
    }
}


//1.10f: D2Client.0x6FB07370
//  Loads the stored NPC speech choice and re-applies it, at menu load. The original inlines ESCMENU_NpcSpeechOnChange here; ours calls it with
//  edx 0, which OnChange never reads. It calls SOUNDCHAN_IsSystemReady_2 directly, not through the page's wrapper.
void __fastcall ESCMENU_NpcSpeechRefresh(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    pItem->nValue = D2SOUND_10052_GetNpcSpeechSetting();

    if (SOUNDCHAN_IsSystemReady_2())
        ESCMENU_NpcSpeechOnChange(pItem, 0);
}


//1.10f: D2Client.0x6FB073E0
//  The video page: fourteen functions serving two tables that differ by one row:
//
//      classic  expansion  label         kind         IsEnabled
//      -        1          Resolution    2 choices    -  (bExpansionOnly)
//      1        2          LightQuality  3 choices    -
//      2        3          BlendShadow   2 choices    -
//      3        4          Perspective   2 choices    j_D2gfx_10007
//      4        5          Gamma         slider, 21   j_D2gfx_10019
//      5        6          Contrast      slider, 100  -
//
//  No video row has a draw hook, so each Refresh runs once per game: SCMD_HandleSrv01_GameFlags -> UI_InitUiSubsystems -> ESCMENU_LoadImages
//  -> ESCMENU_LoadPageImages, which calls +0x118 for every row. Every one of these Refreshes applies the setting, not only displays it, so
//  joining a game is what puts the stored light quality, shadows, resolution, perspective, gamma and contrast into effect.
//
//  A missing registry value: four rows load straight into pItem->nValue, so an absent value leaves the table's own and applies that:
//
//      LightQuality 2   high: SetLowQuality(0), SetLightQuality(1)
//      BlendShadow  1   on
//      Resolution   0   640x480
//      Perspective  1   on -- and PerspectiveRefresh saves what it loaded whenever the renderer can do perspective, so on such a renderer
//                       the first game writes Perspective = 1
//
//  The other two load into a defaulted local: Gamma 155, the middle of its 55..255 range, and Contrast 100 -- or 0 when D2GFX_GetDisplayType()
//  is 4 (`sub eax, 4 / neg / sbb / and eax, 64h`).
//
//  The sliders:
//
//      Gamma     change   g = (int)(200.0f / (unsigned)(dwCount - 1) * n + 55.0f)
//                refresh  n = (int)((unsigned)(dwCount - 1) * (int)(g - 54) * 0.005f)
//      Contrast  change   saved (int)(100.0f / (unsigned)(dwCount - 1) * n), applied n
//                refresh  n = (int)((unsigned)(s + 1) * (unsigned)(dwCount - 1) * 0.01f), then saved as the change saves it
//
//  Every constant is a float in .rdata (100, 200, 55, 0.01 and 0.005), so they are float literals. The integer factors are not all converted
//  alike, and each is written the way the bytes convert it: `fild qword` over a zero-extended dword is unsigned (dwCount - 1, and Contrast's
//  stored + 1), `fimul dword` is signed (the step, and Gamma's stored - 54).
//  Gamma's - 54 is the sound sliders' + 1 in another form: the change writes 10n + 55, and 20 * (10n + 1) * 0.005f is a hair under n + 0.1, so
//  every step reads back as itself. Contrast re-saves on every game: the stored value goes to a step and back and is written, so a value off
//  the grid snaps to it; the step it applies as D2Gfx option 11 is the raw step, while the registry gets the rescaled one, and with 100 steps
//  the two differ only at the top, 99 against 100.
//
//  LightQualityOnChange saves the choice and switches as ESCMENU_NpcSpeechOnChange does, over D2GFX_SetLowQuality and MISSILE_EnvSetLightQuality:
//
//      0   low quality on,  light quality 0
//      1   low quality off, light quality 0
//      2   low quality off, light quality 1
//
//  LightQualityRefresh loads and runs the same switch but without the save, and no out-of-line function holds the switch alone, so the
//  switch is written twice, as the bytes have it. The same goes for Resolution's two-way mapping and BlendShadow's one call.
//
//  The Light Quality row changed: save the choice, then the three-way switch above; a value past 2 is saved and applies nothing.
void __fastcall ESCMENU_LightQualityOnChange(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    STORM_SRegSaveValue(szDiabloIIKey, "Light Quality", 0, pItem->nValue);

    switch (pItem->nValue)
    {
    case 0:
        D2GFX_SetLowQuality(1);
        MISSILE_EnvSetLightQuality(0);
        break;

    case 1:
        D2GFX_SetLowQuality(0);
        MISSILE_EnvSetLightQuality(0);
        break;

    case 2:
        D2GFX_SetLowQuality(0);
        MISSILE_EnvSetLightQuality(1);
        break;
    }
}


//1.10f: D2Client.0x6FB07440
//  Loads the Light Quality choice into the row and applies it, once per game. No default: an absent value leaves the table's 2.
void __fastcall ESCMENU_LightQualityRefresh(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    STORM_SRegLoadValue(szDiabloIIKey, "Light Quality", 0, (uint32_t *)&pItem->nValue);

    switch (pItem->nValue)
    {
    case 0:
        D2GFX_SetLowQuality(1);
        MISSILE_EnvSetLightQuality(0);
        break;

    case 1:
        D2GFX_SetLowQuality(0);
        MISSILE_EnvSetLightQuality(0);
        break;

    case 2:
        D2GFX_SetLowQuality(0);
        MISSILE_EnvSetLightQuality(1);
        break;
    }
}


//1.10f: D2Client.0x6FB074A0
//  The Blended Shadows row changed.
void __fastcall ESCMENU_BlendShadowOnChange(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    STORM_SRegSaveValue(szDiabloIIKey, "Blended Shadows", 0, pItem->nValue);
    D2GFX_SetBlendShadows(pItem->nValue);
}


//1.10f: D2Client.0x6FB074D0
//  Loads and applies the Blended Shadows choice, once per game; the table's 1 when absent.
void __fastcall ESCMENU_BlendShadowRefresh(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    STORM_SRegLoadValue(szDiabloIIKey, "Blended Shadows", 0, (uint32_t *)&pItem->nValue);
    D2GFX_SetBlendShadows(pItem->nValue);
}


//1.10f: D2Client.0x6FB07500
//  The Resolution row changed, expansion only. Choice 1 is 800x600 and anything else 640x480: `xor ecx, ecx / dec esi / jnz`, then `mov ecx, 2`.
void __fastcall ESCMENU_ResolutionOnChange(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    D2GameResolutionMode nMode;

    UNREFERENCED_PARAMETER(pMsg);

    STORM_SRegSaveValue(szDiabloIIKey, "Resolution", 0, pItem->nValue);

    nMode = D2GAMERES_640x480;
    if (pItem->nValue == 1)
        nMode = D2GAMERES_800x600;

    WNDPROC_SetResolutionMode(nMode);
}


//1.10f: D2Client.0x6FB07540
//  Loads and applies the resolution, once per game in an expansion game; the table's 0, 640x480, when absent.
void __fastcall ESCMENU_ResolutionRefresh(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    D2GameResolutionMode nMode;

    UNREFERENCED_PARAMETER(pMsg);

    STORM_SRegLoadValue(szDiabloIIKey, "Resolution", 0, (uint32_t *)&pItem->nValue);

    nMode = D2GAMERES_640x480;
    if (pItem->nValue == 1)
        nMode = D2GAMERES_800x600;

    WNDPROC_SetResolutionMode(nMode);
}


//1.10f: D2Client.0x6FB07570
//  `jmp D2GFX_CheckUnitPerspective` (D2Gfx #10007) -- the Perspective rows' pfnIsEnabled. A wrapper with its target's signature.
int32_t __stdcall j_D2gfx_10007(void)
{
    return D2GFX_CheckUnitPerspective();
}


//1.10f: D2Client.0x6FB07580
//  The Perspective row changed: save, switch D2Gfx's perspective, and set the view's rectangle again, since the projection changes the drawable
//  area.
void __fastcall ESCMENU_PerspectiveOnChange(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    STORM_SRegSaveValue(szDiabloIIKey, "Perspective", 0, pItem->nValue);
    D2GFX_SetPerspective(pItem->nValue);
    VIEW_SetRect(gpView, 0, 0, gnViewportRight, gnViewportBottom);
}


//1.10f: D2Client.0x6FB075D0
//  Loads the Perspective choice and applies it only if the renderer can, once per game. When D2GFX_CheckUnitPerspective says yes, the loaded
//  value is saved back (a no-op for a stored value, but for an absent one it writes the table's 1) and applied as the change applies it. When
//  it says no, the row is forced to 0 and the stored preference is left alone.
void __fastcall ESCMENU_PerspectiveRefresh(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    STORM_SRegLoadValue(szDiabloIIKey, "Perspective", 0, (uint32_t *)&pItem->nValue);

    if (D2GFX_CheckUnitPerspective())
    {
        STORM_SRegSaveValue(szDiabloIIKey, "Perspective", 0, pItem->nValue);
        D2GFX_SetPerspective(pItem->nValue);
        VIEW_SetRect(gpView, 0, 0, gnViewportRight, gnViewportBottom);
    }
    else
    {
        pItem->nValue = 0;
    }
}


//1.10f: D2Client.0x6FB07640
//  `jmp D2GFX_GammaCanBeControlled` (D2Gfx #10019) -- the Gamma rows' pfnIsEnabled; the prototype is the target's.
int32_t __stdcall j_D2gfx_10019(void)
{
    return D2GFX_GammaCanBeControlled();
}


//1.10f: D2Client.0x6FB07650
//  The Gamma slider moved: 21 steps onto 55..255, saved and applied.
void __fastcall ESCMENU_GammaOnChange(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nGamma;

    UNREFERENCED_PARAMETER(pMsg);

    nGamma = (int32_t)(200.0f / (double)(pItem->dwCount - 1) * pItem->nValue + 55.0f);
    STORM_SRegSaveValue(szDiabloIIKey, "Gamma", 0, nGamma);
    DRAW_SetGamma(nGamma);
}


//1.10f: D2Client.0x6FB076C0
//  Places the Gamma slider and re-applies what it shows, once per game. 155 when the value is absent. Only when the hardware can set gamma is
//  the step turned back into a gamma, saved and applied; otherwise the row still shows a position and nothing is written.
void __fastcall ESCMENU_GammaRefresh(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    uint32_t nStoredGamma;
    int32_t nGamma;

    UNREFERENCED_PARAMETER(pMsg);

    nStoredGamma = 155;
    STORM_SRegLoadValue(szDiabloIIKey, "Gamma", 0, &nStoredGamma);

    pItem->nValue = (int32_t)((double)(pItem->dwCount - 1) * (int32_t)(nStoredGamma - 54) * 0.005f);

    if (D2GFX_GammaCanBeControlled())
    {
        nGamma = (int32_t)(200.0f / (double)(pItem->dwCount - 1) * pItem->nValue + 55.0f);
        STORM_SRegSaveValue(szDiabloIIKey, "Gamma", 0, nGamma);
        DRAW_SetGamma(nGamma);
    }
}


//1.10f: D2Client.0x6FB07780
//  The Contrast slider moved. The registry gets the step rescaled to 0..100; D2Gfx option 11 gets the step itself, re-read from the row;
//  D2GFX_SetGamma(0) follows.
void __fastcall ESCMENU_ContrastOnChange(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    STORM_SRegSaveValue(szDiabloIIKey, "Contrast", 0, (int32_t)(gfPercentDivisor / (double)(pItem->dwCount - 1) * pItem->nValue));
    D2GFX_SetOption(11, pItem->nValue);
    D2GFX_SetGamma(0);
}


//1.10f: D2Client.0x6FB077F0
//  Places the Contrast slider, re-saves and applies, once per game. The default depends on the renderer; the stored value goes to a step and
//  is saved back from it, so an off-grid value snaps. The bytes keep (double)(dwCount - 1) on the x87 stack across both expressions, which is
//  one local here.
void __fastcall ESCMENU_ContrastRefresh(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    uint32_t nStoredContrast;
    double dSteps;

    UNREFERENCED_PARAMETER(pMsg);

    nStoredContrast = D2GFX_GetDisplayType() != 4 ? 100 : 0;
    STORM_SRegLoadValue(szDiabloIIKey, "Contrast", 0, &nStoredContrast);

    dSteps = (double)(pItem->dwCount - 1);
    pItem->nValue = (int32_t)((double)(nStoredContrast + 1) * dSteps * 0.01f);

    STORM_SRegSaveValue(szDiabloIIKey, "Contrast", 0, (int32_t)(gfPercentDivisor / dSteps * pItem->nValue));
    D2GFX_SetOption(11, pItem->nValue);
    D2GFX_SetGamma(0);
}


//1.10f: D2Client.0x6FB078B0
//  The automap page's five rows -- ten callbacks, every one reached only through a slot in gEscMenuAutomapClassicItems or
//  gEscMenuAutomapExpansionItems:
//
//      Mode        expansion only -- classic has no Mode row
//      Fade
//      Center
//      Party
//      PartyNames
//
//  The rows' dwCount is 2, 4, 2, 2, 2 in both tables. The callbacks' clamps accept 0..2 (Mode) and 0..4 (Fade), one wider than the row can
//  step to; the fade hotkey of CmdTbl cycles `% 4` as well. The extra value can only arrive from the registry.
//
//  The Refresh half runs at menu load, and four of the five on every draw: ESCMENU_LoadPageImages reaches the +0x118 slot eight bytes back off
//  a pointer to dwCount (`mov eax, [ebx-8]`, `call eax`, ecx the row and edx 0) once per row, and the +0x11C slot of Fade, Center, Party and
//  PartyNames holds the same function as their +0x118, in both tables, so those four run on every draw of their row too, with edx = 0.
//  Mode's +0x11C is NULL. All five put the stored setting into the row when the menu loads, and four keep re-reading it while the page is on
//  screen.
//
//  The OnChange half splits two ways: Mode and Fade clamp the row's value, apply it and write the registry in the callback. Center, Party and
//  PartyNames are eleven bytes each, `mov ecx, [ecx+124h]` and a tail jump into an AUTOMAP_Set*AndSave that does both.
//
//  The Automap Mode row changed: three choices, so a value above 2 wraps to 0; the unsigned compare makes that catch an underflow too.
void __fastcall ESCMENU_AutomapModeOnChange(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    if ((uint32_t)pItem->nValue > 2u)
        pItem->nValue = 0;

    AUTOMAP_SetMode(pItem->nValue);
    STORM_SRegSaveValue(szDiabloIIKey, szAutoMapMode, 0, pItem->nValue);
}


//1.10f: D2Client.0x6FB078F0
//  Loads the saved automap mode into the row, once per menu load by ESCMENU_LoadPageImages, in an expansion game.
//  BUG: nStoredMode goes to SRegLoadValue by address without being initialised and the return is not checked. Storm does not write the buffer
//  when the read fails, so with no registry value present the clamp decides on stack garbage: above the maximum it becomes 0, and 0..maximum
//  is used as if it had been read. `AutoMapMode` is referenced only by this row's OnChange (the save) and this Refresh (the load), so where
//  the value is absent (the row never changed on that install) the mode applied at menu load is decided by whatever that stack slot held.
//  REACHABLE: it runs at every menu load, once per game joined from packet 0x01's UI_InitUiSubsystems.
//  DIFFERENCE: the local stays uninitialised here. The call goes to another module, so the compiler cannot treat the read as undefined, and
//  our stack holds different garbage from the original's; the difference exists and cannot be closed.
//  The second store to nStoredMode, inside the clamp, goes to a local nothing reads again -- reproduced. The row does not take the clamped
//  value directly: it applies it and then reads AUTOMAP_GetMode back, so whatever the automap decides is what the row shows.
void __fastcall ESCMENU_AutomapModeRefresh(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    uint32_t nMode;
    uint32_t nStoredMode;

    UNREFERENCED_PARAMETER(pMsg);

    STORM_SRegLoadValue(szDiabloIIKey, szAutoMapMode, 0, &nStoredMode);

    nMode = nStoredMode;
    if (nStoredMode > 2)
    {
        nMode = 0;
        nStoredMode = 0;
    }

    AUTOMAP_SetMode(nMode);
    pItem->nValue = AUTOMAP_GetMode();
}


//1.10f: D2Client.0x6FB07930
//  The Automap Fade row changed: five choices.
//  BUG: the registry is written twice. AUTOMAP_SetFade saves `AutoMapFade` itself and this saves it again on the next line, with the same
//  value. HARMLESS: the second write stores what the first did.
void __fastcall ESCMENU_AutomapFadeOnChange(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    if ((uint32_t)pItem->nValue > 4u)
        pItem->nValue = 0;

    AUTOMAP_SetFade(pItem->nValue);
    STORM_SRegSaveValue(szDiabloIIKey, szAutoMapFade, 0, pItem->nValue);
}


//1.10f: D2Client.0x6FB07970
//  Loads the saved fade into the row, at menu load through +0x118 and on every draw of its row through +0x11C.
//  BUG: the same uninitialised read as the Mode refresh: the local goes to SRegLoadValue unchecked, so with no `AutoMapFade` value present
//  the clamp decides on stack garbage. AUTOMAP_SetFade(0) is called only when the garbage is above 4, and SetFade saves `AutoMapFade`, after
//  which the read succeeds. The row shows AUTOMAP_GetFade() either way. REACHABLE: it runs at every menu load and on every draw besides.
//  DIFFERENCE: as in the Mode refresh, the local stays uninitialised and our stack holds different garbage from the original's.
//  It differs from Mode in the shape of the clamp: where Mode computes a corrected value and applies it either way, Fade only calls
//  AUTOMAP_SetFade when the stored value was out of range, so a value in 0..4 is read back from the automap without being applied to it first.
void __fastcall ESCMENU_AutomapFadeRefresh(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    uint32_t nStoredFade;

    UNREFERENCED_PARAMETER(pMsg);

    STORM_SRegLoadValue(szDiabloIIKey, szAutoMapFade, 0, &nStoredFade);

    if (nStoredFade > 4)
    {
        nStoredFade = 0;
        AUTOMAP_SetFade(0);
    }

    pItem->nValue = AUTOMAP_GetFade();
}


//1.10f: D2Client.0x6FB079B0
//  The Centre-on-player row changed: `mov ecx, [ecx+124h]` and a tail jump into AUTOMAP_SetCentersAndSave.
void __fastcall ESCMENU_AutomapCenterOnChange(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    AUTOMAP_SetCentersAndSave(pItem->nValue);
}


//1.10f: D2Client.0x6FB079C0
//  Loads the Centre flag back, at menu load and on every draw of its row (both slots hold it). There is no registry read of its own:
//  AUTOMAP_GetCenters returns the live flag, which AUTOMAP_LoadSettings has already filled from the registry.
void __fastcall ESCMENU_AutomapCenterRefresh(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    pItem->nValue = AUTOMAP_GetCenters();
}


//1.10f: D2Client.0x6FB079D0
//  The Show-party row changed: `mov ecx, [ecx+124h]` and a tail jump, like Center.
void __fastcall ESCMENU_AutomapPartyOnChange(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    AUTOMAP_SetShowPartyAndSave(pItem->nValue);
}


//1.10f: D2Client.0x6FB079E0
//  Loads the Show-party flag back, at menu load and on every draw of its row.
void __fastcall ESCMENU_AutomapPartyRefresh(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    pItem->nValue = AUTOMAP_GetShowParty();
}


//1.10f: D2Client.0x6FB079F0
//  `jmp AUTOMAP_GetShowParty` -- the PartyNames rows' pfnIsEnabled, in both automap tables, so the names row is live only while party markers
//  are shown. Its Refresh asks the same question again for the value.
int32_t __cdecl j_AUTOMAP_GetShowParty(void)
{
    return AUTOMAP_GetShowParty();
}


//1.10f: D2Client.0x6FB07A00
//  The Party-names row changed: `mov ecx, [ecx+124h]` and a tail jump.
void __fastcall ESCMENU_AutomapPartyNamesOnChange(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    AUTOMAP_SetPartyNamesAndSave(pItem->nValue);
}


//1.10f: D2Client.0x6FB07A10
//  Loads the Party-names flag back, at menu load and on every draw of its row; the only one of the five that is not a single accessor.
//  Party names are shown only when the party markers are: the row reads `AUTOMAP_GetShowParty() && AUTOMAP_GetPartyNames()`, so turning the
//  party display off makes this row read 0 whatever its own setting says. The dependency is one-way: nothing here writes the party flag.
void __fastcall ESCMENU_AutomapPartyNamesRefresh(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pMsg);

    if (AUTOMAP_GetShowParty() && AUTOMAP_GetPartyNames())
        pItem->nValue = 1;
    else
        pItem->nValue = 0;
}


//1.10f: D2Client.0x6FB07A40
//  The "Previous" row of the options page: SetPage(0, 0), `xor edx, edx / xor ecx, ecx / jmp ESCMENU_SetPage`.
void __fastcall ESCMENU_GotoMainPage(D2ClientEscMenuItemStrc *pItem, SMSGHANDLER_PARAMS *pMsg)
{
    UNREFERENCED_PARAMETER(pItem);
    UNREFERENCED_PARAMETER(pMsg);

    ESCMENU_SetPage(0, 0);
}


//1.10f: D2Client.0x6FB07A50
//  A click on the ESC menu: move the highlight to whatever row was hit, hand the position to the slider, and go into dragging.
//  A miss does not clear the selection: ESCMENU_HitTestRow answers -1 off any row, and the -1 is tested before the store, so clicking between
//  rows leaves the highlight where it was.
//  ESCMENU_DragSlider runs unconditionally, hit or miss -- it does its own hit test against the slider track. `gnEscMenuDragging` is set
//  either way, which is what makes the matching release meaningful.
void __stdcall ESCMENU_OnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nHitRow;

    nMouseY = pMsg->nYpos;
    nMouseX = pMsg->nXpos;

    nHitRow = ESCMENU_HitTestRow(nMouseY);
    if (nHitRow != -1)
        gnEscMenuSelectedItem = nHitRow;

    ESCMENU_DragSlider(nMouseX, nMouseY);
    gnEscMenuDragging = 1;

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FB07AA0
//  Returns the row at this screen Y, or -1; the single place the ESC menu turns a cursor into a row.
//  The menu is centred on `(gnScreenHeight - 80) / 2` and its half-height is `nItemCount * nRowHeight / 2`. Outside that band it answers -1
//  without looking at a row. Both divisions are the signed idiom, `cdq / sub eax, edx / sar eax, 1`.
//  The scan keeps the last match, not the first: it walks every row and overwrites `nHitRow` each time the cursor is past that row's Y, so
//  the answer is the last row the cursor cleared. A row whose `nScreenY` is 0 is skipped; nScreenY is 0 in every record on disk and
//  ESCMENU_Draw writes it per frame (0 for rows the page does not show), so this function finds nothing until the menu has been drawn once.
//  `pnRowY += 340` is 340 dwords, which is 1360 bytes, the row stride.
//  BUG: the comparison is `nMouseY >= pRow->nScreenY + nItemCount` (`add eax, edi` at 0x6FB07AEB): edi is the item count (`mov edi, [eax]`
//  loads sgptMenuInfo +0x00, and the same register is the loop bound), where a member of D2ClientEscMenuInfoStrc was probably meant, most
//  likely `nLabelYOffset` at +0x08, which ESCMENU_Draw adds to this very field to place the label. Every band therefore starts about thirty
//  pixels higher than intended (rows are nRowHeight apart, 45 or 50, and the offsets are 3 to 8 against 34 or 39, so the bands stay disjoint
//  and ordered), and the row you get is the one whose slot the cursor is in rather than the one whose text it is on. REACHABLE: every click
//  on this menu does it. Reproduced, not repaired; which member was meant is a hypothesis, what is proved is the value added.
int32_t __fastcall ESCMENU_HitTestRow(int32_t nMouseY)
{
    int32_t nRowCount;
    int32_t nHitRow;
    int32_t nHalfHeight;
    int32_t nCentreY;
    int32_t iRow;
    D2ClientEscMenuItemStrc *pRow;
    ESCMENUENABLEDFN pfnRowEnabled;

    nRowCount = sgptMenuInfo->nItemCount;
    nHitRow = -1;
    nHalfHeight = nRowCount * sgptMenuInfo->nRowHeight / 2;
    nCentreY = (gnScreenHeight - 80) / 2;

    if (nMouseY > nCentreY - nHalfHeight && nMouseY < nCentreY + nHalfHeight)
    {
        if (nRowCount > 0)
        {
            for (iRow = 0; iRow < nRowCount; ++iRow)
            {
                pRow = &sgptMenuItem[iRow];
                //  BUG: the item count is added here.
                if (pRow->nScreenY && nMouseY >= nRowCount + pRow->nScreenY)
                    nHitRow = iRow;
            }

            if (nHitRow >= 0)
            {
                pRow = &sgptMenuItem[nHitRow];
                if (pRow->dwType == -1)
                    return -1;

                pfnRowEnabled = pRow->pfnIsEnabled;
                if (pfnRowEnabled)
                {
                    if (!pfnRowEnabled())
                        return -1;
                }
            }
        }
    }

    return nHitRow;
}


//1.10f: D2Client.0x6FB07B30
//  Sets a slider row's value from the cursor's X; what makes the ESC menu's sliders draggable rather than click-once. Called by ESCMENU_Draw
//  every frame and by ESCMENU_OnLButtonDown on the press.
//  While gbEscMenuSliderDragging is clear both gates apply: the row under the cursor must be the selected one, and the X must be inside the
//  track. Once it is set both are skipped, so the drag keeps tracking the cursor after it has left the row and the track. It is set here and
//  cleared in ESCMENU_OnLButtonUp.
//  Two track positions, chosen by the row's own image: with hImage set left = gnScreenWidth / 2 - 60, clear left = gnScreenWidth / 2 - 145.
//  The gate tests `left + 1` and `left + 290`; the value uses `left + 12` and `left + 277`, a 265-pixel track inset inside a 289-pixel gate.
//  nTrackLeft and nTrackLeftMinus1 differ by one and are computed separately, as in the bytes.
//
//      value = (int)(x - left - 12) / (132.5 / (dwCount - 1)) + 1  ... / 2
//
//  in double, converted through a 64-bit intermediate, then halved as an integer. 132.5 is half of 265, so the divisor is half a step and
//  the `+ 1` and `/ 2` together are a round-to-nearest done in fixed point. Below the track the value clamps to 0 and above it to
//  `dwCount - 1`; those two clamps are the only paths that do not go through the float.
//  `dwCount - 1` reaches the conversion to double unsigned, as the original's `fild qword` over a zero high dword converts it: a count of 0
//  would make it 0xFFFFFFFF rather than -1, and the divisor enormous rather than negative.
//  It ends the way the arrow keys do: pfnOnChange and the click sound, but only if the value actually moved.
void __fastcall ESCMENU_DragSlider(int32_t nMouseX, int32_t nMouseY)
{
    int32_t nSelected;
    D2ClientEscMenuItemStrc *pRow;
    int32_t nValueBefore;
    ESCMENUENABLEDFN pfnEnabled;
    int32_t bEnabled;
    int32_t nScreenMidX;
    int32_t nTrackLeft;
    int32_t nTrackLeftMinus1;

    if (gbEscMenuSliderDragging)
    {
        nSelected = gnEscMenuSelectedItem;
    }
    else
    {
        if (ESCMENU_HitTestRow(nMouseY) != gnEscMenuSelectedItem)
        {
            return;
        }
        nSelected = gnEscMenuSelectedItem;
    }

    pRow = &sgptMenuItem[nSelected];
    nValueBefore = pRow->nValue;
    pfnEnabled = pRow->pfnIsEnabled;

    if (!pfnEnabled || (bEnabled = pfnEnabled(), nSelected = gnEscMenuSelectedItem, bEnabled))
    {
        pRow = &sgptMenuItem[nSelected];
        if (pRow->dwType == 2)
        {
            nScreenMidX = gnScreenWidth / 2;
            nTrackLeft = nScreenMidX - 59;
            nTrackLeftMinus1 = nScreenMidX - 60;

            if (!pRow->hImage)
            {
                nTrackLeft = nScreenMidX - 144;
                nTrackLeftMinus1 = nScreenMidX - 145;
            }

            if (gbEscMenuSliderDragging || (nMouseX > nTrackLeft && nMouseX < nTrackLeft + 289))
            {
                if (nMouseX >= nTrackLeftMinus1 + 12)
                {
                    if (nMouseX <= nTrackLeftMinus1 + 277)
                    {
                        pRow->nValue = (int32_t)(int64_t)((double)(nMouseX - (nTrackLeftMinus1 + 12)) / (132.5 / (double)(pRow->dwCount - 1)) + 1.0) / 2;
                    }
                    else
                    {
                        pRow->nValue = pRow->dwCount - 1;
                    }
                }
                else
                {
                    pRow->nValue = 0;
                }

                nSelected = gnEscMenuSelectedItem;
                gbEscMenuSliderDragging = 1;
            }
        }
    }

    pRow = &sgptMenuItem[nSelected];
    if (nValueBefore != pRow->nValue)
    {
        if (pRow->pfnOnChange)
            pRow->pfnOnChange(pRow, 0);
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
    }
}


//1.10f: D2Client.0x6FB07CC0
//  The release: the row is activated only if the release lands on the same row the press selected, `HitTestRow(y) == gnEscMenuSelectedItem`.
//  Releasing on a different row does nothing at all.
//  The Y is loaded unconditionally, before the dragging test (`xor ecx, ecx / mov cx, [esi+0Eh]` at 0x6FB07CCA, both above the `jz`). Both
//  drag flags are cleared on every path, including the one that activated a row.
void __stdcall ESCMENU_OnLButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nMouseY;

    nMouseY = pMsg->nYpos;

    if (gnEscMenuDragging)
    {
        if (ESCMENU_HitTestRow(nMouseY) == gnEscMenuSelectedItem)
        {
            ESCMENU_ActivateSelectedRow(pMsg);
        }
    }

    GAME_ResetClickState();
    gnEscMenuDragging = 0;
    gbEscMenuSliderDragging = 0;

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FB07D20
//  What Enter and a click do to the highlighted row; the one place keyboard and mouse activation meet.
//
//      dwType 0   a command row: call pfnOnChange, sound 2
//      dwType 1   a cycling value row: advance nValue with the wrap, call pfnOnChange, sound 1
//      anything   nothing at all, sliders included -- those are dragged
//      else
//
//  Sound 2 is the menu-select click and 1 the value tick, which is the same sound the arrow keys play.
//  The sound sits on different sides of the null check: in the dwType 0 arm `SOUNDINST_PlaySound(2, ...)` is inside `if (pfnOnChange)`, in the
//  dwType 1 arm `SOUNDINST_PlaySound(1, ...)` is outside it. So a command row with no handler is silent, and a value row with no handler
//  still ticks while its number moves. Reproduced.
//  This is the one path that passes pMsg: `mov edx, esi` at 0x6FB07DE8 and 0x6FB07E09, where esi is this function's own argument, so both
//  dispatches here are `pfnOnChange(pRow, pMsg)`; the other three sites in the module pass 0. That is what ESCMENU_OnExitGame needs: it reads
//  `pMsg->hWnd` and posts WM_CLOSE to it.
//  The gate is called with nothing: `mov eax, [<row>+110h] / test / jz / call eax` at 0x6FB07D48, with neither ecx nor edx set -- pfnIsEnabled
//  really is `int __cdecl()`. Its answer is read, unlike pfnOnChange's, so a disabled row swallows the keypress in silence.
void __fastcall ESCMENU_ActivateSelectedRow(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nSelected;
    ESCMENUENABLEDFN pfnIsEnabled;
    D2ClientEscMenuItemStrc *pRow;

    nSelected = gnEscMenuSelectedItem;
    pfnIsEnabled = sgptMenuItem[nSelected].pfnIsEnabled;

    if (pfnIsEnabled)
    {
        if (!pfnIsEnabled())
            return;
        nSelected = gnEscMenuSelectedItem;
    }

    pRow = &sgptMenuItem[nSelected];

    if (pRow->dwType)
    {
        if (pRow->dwType == 1)
        {
            ++pRow->nValue;

            nSelected = gnEscMenuSelectedItem;
            pRow = &sgptMenuItem[nSelected];
            if ((uint32_t)pRow->nValue > pRow->dwCount - 1)
            {
                pRow->nValue = 0;
                nSelected = gnEscMenuSelectedItem;
            }

            pRow = &sgptMenuItem[nSelected];
            if (pRow->pfnOnChange)
                pRow->pfnOnChange(pRow, pMsg);

            SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
        }
    }
    else
    {
        if (pRow->pfnOnChange)
        {
            pRow->pfnOnChange(pRow, pMsg);
            SOUNDINST_PlaySound(SOUND_CURSOR_SELECT, NULL, 0, 0, 0);
        }
    }
}


//1.10f: D2Client.0x6FB07E20
//  VK_DOWN: move the highlight to the next row that is neither a separator nor disabled, wrapping past the last to row 0. The inner loop walks
//  separators, the outer one re-runs the inner one when the row it landed on says it is disabled, so a run of disabled rows is crossed in one
//  keypress. DOWN tests `> nItemCount - 1` and restarts at 0, UP tests `< 0` and restarts at `nItemCount - 1`; both comparisons are signed in
//  the bytes (`jle` in DOWN), which is what lets the decrement below zero be seen at all.
//  The click sound fires whatever happened, including when the highlight came back to the row it started on.
//  BUG: neither loop is bounded: a page whose rows are all separators spins in the inner loop (0x6FB07E26..0x6FB07E59), and a page whose
//  every non-separator row answers 0 from pfnIsEnabled spins in the outer one (0x6FB07E20..0x6FB07E69); nothing counts the rows already
//  passed. LATENT: every page ESCMENU_SetPage builds has at least one enabled row. Written as the original is, not bounded.
void __stdcall ESCMENU_OnDownKey(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nSelected;
    D2ClientEscMenuItemStrc *pRow;
    ESCMENUENABLEDFN pfnCanChange;

    do
    {
        nSelected = gnEscMenuSelectedItem;
        do
        {
            gnEscMenuSelectedItem = ++nSelected;
            if (nSelected > sgptMenuInfo->nItemCount - 1)
            {
                nSelected = 0;
                gnEscMenuSelectedItem = 0;
            }
            pRow = &sgptMenuItem[nSelected];
        }
        while (pRow->dwType == -1);

        pfnCanChange = pRow->pfnIsEnabled;
    }
    while (pfnCanChange && !pfnCanChange());

    SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FB07EA0
//  VK_UP: the same function as OnDownKey with `--` for `++`; the wrap is the only real difference: below row 0 it restarts at
//  `nItemCount - 1`.
//  BUG: the two loops are unbounded, as in ESCMENU_OnDownKey. LATENT: every page has at least one enabled row.
void __stdcall ESCMENU_OnUpKey(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nSelected;
    D2ClientEscMenuItemStrc *pRow;
    ESCMENUENABLEDFN pfnCanChange;

    do
    {
        nSelected = gnEscMenuSelectedItem;
        do
        {
            gnEscMenuSelectedItem = --nSelected;
            if (nSelected < 0)
            {
                nSelected = sgptMenuInfo->nItemCount - 1;
                gnEscMenuSelectedItem = nSelected;
            }
            pRow = &sgptMenuItem[nSelected];
        }
        while (pRow->dwType == -1);

        pfnCanChange = pRow->pfnIsEnabled;
    }
    while (pfnCanChange && !pfnCanChange());

    SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FB07F20
//  VK_LEFT: step the highlighted row's value down one.
//  The row is re-indexed from the two globals four times, and that is the source's shape rather than the compiler's: at 0x6FB07F89 the
//  reload comes immediately after the store two instructions above it, with no call in between to force it. So the original wrote
//  `sgptMenuItem[gnEscMenuSelectedItem]` out at each step, and the local below is reassigned once per reload to say so.
//  A disabled row -- pfnIsEnabled present and answering 0 -- skips straight to the comparison at the end, which then finds nothing changed,
//  so a disabled row is silent rather than special-cased.
//  A `dwType` other than 1 or 2 also falls through to that comparison. The bytes get there as two decrements, `dec edx / jz` then
//  `dec edx / jnz`, which is a switch with two live arms.
void __stdcall ESCMENU_OnLeftKey(SMSGHANDLER_PARAMS *pMsg)
{
    D2ClientEscMenuItemStrc *pRow;
    int32_t nValueBefore;
    ESCMENUENABLEDFN pfnCanChange;

    pRow = &sgptMenuItem[gnEscMenuSelectedItem];
    nValueBefore = pRow->nValue;
    pfnCanChange = pRow->pfnIsEnabled;

    if (!pfnCanChange || pfnCanChange())
    {
        pRow = &sgptMenuItem[gnEscMenuSelectedItem];

        if (pRow->dwType == 1)
        {
            --pRow->nValue;
            pRow = &sgptMenuItem[gnEscMenuSelectedItem];
            if ((uint32_t)pRow->nValue > pRow->dwCount - 1)
                pRow->nValue = pRow->dwCount - 1;
        }
        else if (pRow->dwType == 2)
        {
            --pRow->nValue;
            pRow = &sgptMenuItem[gnEscMenuSelectedItem];
            if ((uint32_t)pRow->nValue > pRow->dwCount - 1)
                pRow->nValue = 0;
        }
    }

    pRow = &sgptMenuItem[gnEscMenuSelectedItem];
    if (nValueBefore != pRow->nValue)
    {
        if (pRow->pfnOnChange)
            pRow->pfnOnChange(pRow, 0);
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FB08060
//  VK_RIGHT: the same code as its mirror with the increment and the two wrap constants exchanged: type 1 goes round to 0, type 2 stops at
//  `dwCount - 1`.
//  The unsigned test is the same in all four arms, `nValue > dwCount - 1`, and only what is written back differs; that is what lets one
//  comparison serve an increment past the top and a decrement below the bottom.
void __stdcall ESCMENU_OnRightKey(SMSGHANDLER_PARAMS *pMsg)
{
    D2ClientEscMenuItemStrc *pRow;
    int32_t nValueBefore;
    ESCMENUENABLEDFN pfnCanChange;

    pRow = &sgptMenuItem[gnEscMenuSelectedItem];
    nValueBefore = pRow->nValue;
    pfnCanChange = pRow->pfnIsEnabled;

    if (!pfnCanChange || pfnCanChange())
    {
        pRow = &sgptMenuItem[gnEscMenuSelectedItem];

        if (pRow->dwType == 1)
        {
            ++pRow->nValue;
            pRow = &sgptMenuItem[gnEscMenuSelectedItem];
            if ((uint32_t)pRow->nValue > pRow->dwCount - 1)
                pRow->nValue = 0;
        }
        else if (pRow->dwType == 2)
        {
            ++pRow->nValue;
            pRow = &sgptMenuItem[gnEscMenuSelectedItem];
            if ((uint32_t)pRow->nValue > pRow->dwCount - 1)
                pRow->nValue = pRow->dwCount - 1;
        }
    }

    pRow = &sgptMenuItem[gnEscMenuSelectedItem];
    if (nValueBefore != pRow->nValue)
    {
        if (pRow->pfnOnChange)
            pRow->pfnOnChange(pRow, 0);
        SOUNDINST_PlaySound(SOUND_CURSOR_PASS, NULL, 0, 0, 0);
    }

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FB081A0
//  VK_RETURN: ESCMENU_ActivateSelectedRow and the claim, nothing else, so Enter and a mouse release on the selected row reach the same
//  function.
void __stdcall ESCMENU_OnEnter(SMSGHANDLER_PARAMS *pMsg)
{
    ESCMENU_ActivateSelectedRow(pMsg);

    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//  Loading the menu and unloading it: ESCMENU_LoadPageImages (one page's images, and every row's Refresh), ESCMENU_LoadImages (every page,
//  three widgets, the pentagram), ESCMENU_UnloadPageImages (one page's images) and ESCMENU_UnloadImages (every page, the widgets). Two of
//  them push Restart.cpp to an assert, which names this file. ESCMENU_LoadImages runs once per game (SCMD_HandleSrv01_GameFlags ->
//  UI_InitUiSubsystems) and ESCMENU_UnloadImages from UI_ShutdownUiSubsystems.
//
//  The asserts name things: `ptMenuInfo` and `ptMenuItem` are the page walkers' parameters, and
//
//      ptMenuItem[ii].dwCount > 0 && ptMenuItem[ii].dwCount <= MAX_CHOICE
//      ptMenuItem[ii].pcChoiceFile[jj][0] != 0
//
//  give ii and jj (the loop counters), dwCount (a DWORD: its `jbe`/`ja` compares, as every other use in the unit) and pcChoiceFile (the
//  choice files). MAX_CHOICE is 4 (`cmp eax, 4 / ja`), the length of both choice arrays. The count is checked only on a type-1 row: sliders
//  carry 21 and 100 and would fail it.
//
//  The loader: both locals start as `""` (the first byte copied from the pool's empty string, the rest cleared: 259 bytes of a MAX_PATH
//  buffer, three of a four-byte language code) and D2Lang #10006 fills the language code. Then per row: a row flagged bExpansionOnly in a
//  classic game is skipped whole, Refresh included; otherwise a label image loads from `DATA\LOCAL\UI\<lang>\<name>` if the row names one, a
//  type-1 row's choice images load from the same place, and then, image or not, pfnRefresh runs with (row, 0).
//
//  The unloaders do not mirror it: UnloadPageImages frees every non-NULL handle and clears it, on every row, with no expansion test
//  (harmless: a skipped row's handles are still NULL). UnloadImages clears sgptMenuInfo and sgptMenuItem first and walks the pages sound,
//  options, main, then video and automap -- not the reverse of the load's main, options, sound, video, automap. Neither calls a row callback.
//
//  Both page choosers make their last call once, for either table: each arm loads its own two arguments and the expansion arm jumps,
//  `jmp short`, into the classic arm's `call`. A call in each arm and one call after two arguments chosen in each compile to the same bytes;
//  this writes a call in each arm, the way the video page's call already sits in them.
//
//  LoadImages sizes the selection pentagram by walking every cel of UI_GetPentSpinImage's cel file through a zeroed D2GfxDataStrc
//  (pCellFile and nFrame) and keeps the widest in gnEscMenuPentagramWidth. ESCMENU_Draw aligns the left pentagram with it.

static constexpr uint32_t MAX_CHOICE = 4;

static_assert(MAX_CHOICE == _countof(((D2ClientEscMenuItemStrc *)0)->hChoiceImage)
              && MAX_CHOICE == _countof(((D2ClientEscMenuItemStrc *)0)->pcChoiceFile)
              && sizeof(((D2ClientEscMenuItemStrc *)0)->pcChoiceFile[0]) == 260,
              "MAX_CHOICE is the `cmp eax, 4 / ja` of the dwCount assert, and "
              "each choice file is the 0x104 the loader's `add edi, 104h` steps");
static_assert(offsetof(D2ClientEscMenuItemStrc, bExpansionOnly) == 0x004
              && offsetof(D2ClientEscMenuItemStrc, szImageFile) == 0x00C
              && offsetof(D2ClientEscMenuItemStrc, pcChoiceFile) == 0x12C
              && offsetof(D2ClientEscMenuItemStrc, hImage) == 0x53C
              && offsetof(D2ClientEscMenuItemStrc, hChoiceImage) == 0x540,
              "the loaders reach bExpansionOnly as [ebx-11Ch], the label as "
              "[ebx-114h] and the choices as [ebx+0Ch] with ebx at +0x120, and "
              "the handles at +0x53C and +0x540");


//1.10f: D2Client.0x6FB081D0
//  Loads one page's images and runs every row's Refresh.
void __fastcall ESCMENU_LoadPageImages(D2ClientEscMenuInfoStrc *ptMenuInfo, D2ClientEscMenuItemStrc *ptMenuItem)
{
    char szPath[260];
    char szLang[4];
    int32_t ii;
    uint32_t jj;
    D2ClientEscMenuItemStrc *pRow;

    if (!ptMenuInfo)
    {
        FOG_DisplayAssert("ptMenuInfo", szRestartCpp, 1316);
        exit(-1);
    }

    if (!ptMenuItem)
    {
        FOG_DisplayAssert("ptMenuItem", szRestartCpp, 1317);
        exit(-1);
    }

    szPath[0] = gszEmptyString;
    memset(&szPath[1], 0, 259);
    szLang[0] = gszEmptyString;
    memset(&szLang[1], 0, 3);

    D2LANG_10006(szLang, 0);

    for (ii = 0; ii < ptMenuInfo->nItemCount; ++ii)
    {
        pRow = &ptMenuItem[ii];

        if (!pRow->bExpansionOnly || FOG_IsExpansion())
        {
            if (pRow->szImageFile[0])
            {
                sprintf(szPath, "%s\\UI\\%s\\%s", aDataLocal, szLang, pRow->szImageFile);
                pRow->hImage = ARCHIVE_CellFileLoad(szPath, 0);

                if (pRow->dwType == 1)
                {
                    if (!(pRow->dwCount > 0 && pRow->dwCount <= MAX_CHOICE))
                    {
                        FOG_DisplayAssert("ptMenuItem[ii].dwCount > 0 && ptMenuItem[ii].dwCount <= MAX_CHOICE", szRestartCpp, 1377);
                        exit(-1);
                    }

                    for (jj = 0; jj < pRow->dwCount; ++jj)
                    {
                        if (!pRow->pcChoiceFile[jj][0])
                        {
                            FOG_DisplayAssert("ptMenuItem[ii].pcChoiceFile[jj][0] != 0", szRestartCpp, 1380);
                            exit(-1);
                        }

                        sprintf(szPath, "%s\\UI\\%s\\%s", aDataLocal, szLang, pRow->pcChoiceFile[jj]);
                        pRow->hChoiceImage[jj] = ARCHIVE_CellFileLoad(szPath, 0);
                    }
                }
            }

            if (pRow->pfnRefresh)
                pRow->pfnRefresh(pRow, 0);
        }
    }
}


//1.10f: D2Client.0x6FB083C0
//  Loads the whole menu, once per game: five pages, the slider's three widget cels out of `DATA\GLOBAL\UI\Widgets`, the menu pointed at its
//  main page, and the pentagram's widest cel. The two page pointers are stored between pushes of the first sprintf in the bytes; that is
//  scheduling, and they go before it here.
void __cdecl ESCMENU_LoadImages(void)
{
    char szPath[260];
    D2GfxDataStrc tCelContext;
    uint32_t nCelCount;
    int32_t nWidestCel;
    int32_t nCelWidth;

    ESCMENU_LoadPageImages(&gEscMenuMain, gEscMenuMainItems);
    ESCMENU_LoadPageImages(&gEscMenuOptions, gEscMenuOptionsItems);
    ESCMENU_LoadPageImages(&gEscMenuSound, gEscMenuSoundItems);

    if (FOG_IsExpansion())
    {
        ESCMENU_LoadPageImages(&gEscMenuVideoExpansion, gEscMenuVideoExpansionItems);
        ESCMENU_LoadPageImages(&gEscMenuAutomapExpansion, gEscMenuAutomapExpansionItems);
    }
    else
    {
        ESCMENU_LoadPageImages(&gEscMenuVideoClassic, gEscMenuVideoClassicItems);
        ESCMENU_LoadPageImages(&gEscMenuAutomapClassic, gEscMenuAutomapClassicItems);
    }

    szPath[0] = gszEmptyString;
    memset(&szPath[1], 0, 259);

    sgptMenuInfo = &gEscMenuMain;
    sgptMenuItem = gEscMenuMainItems;

    sprintf(szPath, "%s\\UI\\Widgets\\OptBar", aDataGlobal);
    ghEscMenuOptBar = ARCHIVE_CellFileLoad(szPath, 0);
    sprintf(szPath, "%s\\UI\\Widgets\\OptBarC", aDataGlobal);
    ghEscMenuOptBarC = ARCHIVE_CellFileLoad(szPath, 0);
    sprintf(szPath, "%s\\UI\\Widgets\\OptSkull", aDataGlobal);
    ghEscMenuOptSkull = ARCHIVE_CellFileLoad(szPath, 0);

    memset(&tCelContext, 0, sizeof(tCelContext));
    tCelContext.pCellFile = UI_GetPentSpinImage();
    nCelCount = D2CMP_CELCMP_CelFileGetCelsPerDirection(tCelContext.pCellFile);

    nWidestCel = 0;
    for (tCelContext.nFrame = 0; tCelContext.nFrame < nCelCount; ++tCelContext.nFrame)
    {
        nCelWidth = D2CMP_CELCMP_CelGetWidth(D2CMP_CELCMP_CelGetHandle(&tCelContext));
        if (nWidestCel <= nCelWidth)
            nWidestCel = nCelWidth;
    }

    gnEscMenuPentagramWidth = nWidestCel;
}


//1.10f: D2Client.0x6FB08540
//  Unloads one page: every non-NULL handle freed and cleared, on every row, with the same choice-count assert as the loader.
void __fastcall ESCMENU_UnloadPageImages(D2ClientEscMenuInfoStrc *ptMenuInfo, D2ClientEscMenuItemStrc *ptMenuItem)
{
    int32_t ii;
    uint32_t jj;
    D2ClientEscMenuItemStrc *pRow;

    if (!ptMenuInfo)
    {
        FOG_DisplayAssert("ptMenuInfo", szRestartCpp, 1466);
        exit(-1);
    }

    if (!ptMenuItem)
    {
        FOG_DisplayAssert("ptMenuItem", szRestartCpp, 1467);
        exit(-1);
    }

    for (ii = 0; ii < ptMenuInfo->nItemCount; ++ii)
    {
        pRow = &ptMenuItem[ii];

        if (pRow->hImage)
        {
            ARCHIVE_CellFileFree(pRow->hImage);
            pRow->hImage = NULL;
        }

        if (pRow->dwType == 1)
        {
            if (!(pRow->dwCount > 0 && pRow->dwCount <= MAX_CHOICE))
            {
                FOG_DisplayAssert("ptMenuItem[ii].dwCount > 0 && ptMenuItem[ii].dwCount <= MAX_CHOICE", szRestartCpp, 1496);
                exit(-1);
            }

            for (jj = 0; jj < pRow->dwCount; ++jj)
            {
                if (pRow->hChoiceImage[jj])
                {
                    ARCHIVE_CellFileFree(pRow->hChoiceImage[jj]);
                    pRow->hChoiceImage[jj] = NULL;
                }
            }
        }
    }
}


//1.10f: D2Client.0x6FB08640
//  Unloads the whole menu, from UI_ShutdownUiSubsystems. The page pointers are cleared first; the pages go sound, options, main, then video
//  and automap; the three widgets last.
void __cdecl ESCMENU_UnloadImages(void)
{
    sgptMenuInfo = NULL;
    sgptMenuItem = NULL;

    ESCMENU_UnloadPageImages(&gEscMenuSound, gEscMenuSoundItems);
    ESCMENU_UnloadPageImages(&gEscMenuOptions, gEscMenuOptionsItems);
    ESCMENU_UnloadPageImages(&gEscMenuMain, gEscMenuMainItems);

    if (FOG_IsExpansion())
    {
        ESCMENU_UnloadPageImages(&gEscMenuVideoExpansion, gEscMenuVideoExpansionItems);
        ESCMENU_UnloadPageImages(&gEscMenuAutomapExpansion, gEscMenuAutomapExpansionItems);
    }
    else
    {
        ESCMENU_UnloadPageImages(&gEscMenuVideoClassic, gEscMenuVideoClassicItems);
        ESCMENU_UnloadPageImages(&gEscMenuAutomapClassic, gEscMenuAutomapClassicItems);
    }

    if (ghEscMenuOptBar)
    {
        ARCHIVE_CellFileFree(ghEscMenuOptBar);
        ghEscMenuOptBar = NULL;
    }

    if (ghEscMenuOptBarC)
    {
        ARCHIVE_CellFileFree(ghEscMenuOptBarC);
        ghEscMenuOptBarC = NULL;
    }

    if (ghEscMenuOptSkull)
    {
        ARCHIVE_CellFileFree(ghEscMenuOptSkull);
        ghEscMenuOptSkull = NULL;
    }
}


//1.10f: D2Client.0x6FB08700
//  Opens the menu on a page.
//  With bCloseUiWindows set it first walks gEscMenuUiWindows: every record's nSavedState is zeroed, and each window with bCloseOnEscMenu set
//  has its state recorded when bRestoreFlag says so and is turned off if it is on. UI_GetUiWindowState is asked twice for those six
//  windows, once to record and once to decide; the original's redundancy, kept.
//  The order of the first two stores is the compiler's: the bytes load +0 into eax, then clear +8, then test eax (0x6FB0870D, 0x6FB08713,
//  0x6FB0871D). Two distinct members cannot alias, so this clears first.
//  Window 9 is then turned ON whether or not the loop ran, and the page is chosen:
//
//      0  gEscMenuMain                3  gEscMenuVideoExpansion or ...Classic
//      1  gEscMenuOptions             4  gEscMenuAutomapExpansion or ...Classic
//      2  gEscMenuSound                  -- by FOG_IsExpansion, asked each time
//
//  The highlight lands on the page's last row, and that is the way back on every page: ReturnToGame on the main page, Previous on the
//  options page, SPrevious on the three below it. So Esc opens the menu with Return to Game selected.
//  Any other nPage keeps whichever page was last set, and 5 then moves the highlight to row 1. All nine callers pass a constant 0..4: 0 from
//  CMDTBL_ActionEscMenu, MINIPANEL_RunButtonAction and ESCMENU_GotoMainPage; 1 from CONFIG_OnCancelButton, CONFIG_OnDismissKey and
//  ESCMENU_GotoOptionsPage; 2, 3 and 4 from the Sound, Video and Automap stubs. So the page-5 store is reproduced and dead in the shipped
//  binary; nothing in the binary says what page 5 was for.
//  void: the original leaves sgptMenuInfo in eax from the item-count load, and all nine call sites discard it.
void __fastcall ESCMENU_SetPage(int32_t bCloseUiWindows, int32_t nPage)
{
    D2C_UIvars nUIId;

    if (bCloseUiWindows)
    {
        for (nUIId = UI_GAME; nUIId < NUM_UI; nUIId = (D2C_UIvars)(nUIId + 1))
        {
            gEscMenuUiWindows[nUIId].nSavedState = 0;

            if (gEscMenuUiWindows[nUIId].bCloseOnEscMenu)
            {
                if (gEscMenuUiWindows[nUIId].bRestoreFlag)
                    gEscMenuUiWindows[nUIId].nSavedState = UI_GetUiWindowState(nUIId);

                if (UI_GetUiWindowState(nUIId))
                    UI_SetUIToggle(nUIId, UI_TURNOFF, 0);
            }
        }
    }

    UI_SetUIToggle(UI_ESCMENU, UI_TURNON, 0);

    switch (nPage)
    {
    case 0:
        sgptMenuInfo = &gEscMenuMain;
        sgptMenuItem = gEscMenuMainItems;
        break;

    case 1:
        sgptMenuInfo = &gEscMenuOptions;
        sgptMenuItem = gEscMenuOptionsItems;
        break;

    case 2:
        sgptMenuInfo = &gEscMenuSound;
        sgptMenuItem = gEscMenuSoundItems;
        break;

    case 3:
        if (FOG_IsExpansion())
        {
            sgptMenuInfo = &gEscMenuVideoExpansion;
            sgptMenuItem = gEscMenuVideoExpansionItems;
        }
        else
        {
            sgptMenuInfo = &gEscMenuVideoClassic;
            sgptMenuItem = gEscMenuVideoClassicItems;
        }
        break;

    case 4:
        if (FOG_IsExpansion())
        {
            sgptMenuInfo = &gEscMenuAutomapExpansion;
            sgptMenuItem = gEscMenuAutomapExpansionItems;
        }
        else
        {
            sgptMenuInfo = &gEscMenuAutomapClassic;
            sgptMenuItem = gEscMenuAutomapClassicItems;
        }
        break;
    }

    gnEscMenuSelectedItem = sgptMenuInfo->nItemCount - 1;
    if (nPage == 5)
        gnEscMenuSelectedItem = 1;
}


//1.10f: D2Client.0x6FB08860
//  Closes the menu and turns back on what it hid. Window 9 is turned off only if it is on. Then, when bRestorePanels is set, every record
//  whose nSavedState is non-zero is turned back ON (`xor edx, edx` at 0x6FB08897, UI_TURNON). Only the six bRestoreFlag windows can hold
//  one; ESCMENU_SetPage zeroed the other thirty-two when it opened the menu. The loop does not clear what it restores.
//  Two callers, both by `jmp` and both passing 1: CMDTBL_ActionEscMenu (Esc while the menu is up) and CONFIG_OnAcceptButton. The original
//  inlines this function in ESCMENU_OnExitGame and ESCMENU_OnReturnToGame (TRUE) and in ESCMENU_OpenConfigPanel (FALSE).
//  bRestorePanels gates the restore, not a close.
void __fastcall ESCMENU_CloseMenu(int32_t bRestorePanels)
{
    D2C_UIvars nUIId;

    if (UI_GetUiWindowState(UI_ESCMENU))
        UI_SetUIToggle(UI_ESCMENU, UI_TURNOFF, 0);

    if (bRestorePanels)
    {
        for (nUIId = UI_GAME; nUIId < NUM_UI; nUIId = (D2C_UIvars)(nUIId + 1))
        {
            if (gEscMenuUiWindows[nUIId].nSavedState)
                UI_SetUIToggle(nUIId, UI_TURNON, 0);
        }
    }
}


//  ESCMENU_Draw, the last function of the unit; its asserts name the file too (`sgptMenuInfo` at line 1726, `sgptMenuItem` at 1727).
//  UIPANELS_DrawInGamePanels calls it every frame the menu is up.
//
//  Layout: the page block is centred vertically, `(gnScreenHeight - 80) / 2` less half of nRowHeight * nItemCount, both halvings signed
//  (`cdq / sub / sar`). Only when the cursor has moved since the last frame does it look at the mouse again: a held button
//  (gnEscMenuDragging, set by OnLButtonDown) drags the slider, otherwise the row under the cursor becomes the selection.
//  Each row then has its nScreenY cleared, so a row hidden in a classic game keeps 0, and a bExpansionOnly row in a classic game is skipped
//  without taking a slot, so the rows below close up. A shown row gets draw mode `4 * enabled + 1`: 5, DRAWMODE_NORMAL, or 1, DRAWMODE_TRANS50
//  when its pfnIsEnabled answers 0. Its pfnOnDraw runs with (row, 0) -- the six rows that re-read a live switch -- and then its type decides
//  the drawing:
//
//      -1, 0   the label, centred on the screen (D2Win #10134, nDiv 1)
//      1       the label at mid - 230 (nDiv 0) and hChoiceImage[nValue] at mid + 230 (nDiv 2)
//      2       the label at mid - 230 if the row has one, then the slider
//
//  sgptMenuItem is re-read from the global after every call, as the bytes do -- a row callback could move it -- so the C indexes the global
//  each time rather than caching a row across a call. Inside the slider arm the bytes cache the row once, after the label, and so does this.
//  D2Win #10134 is `__fastcall (pCellFile, nX, nY, nDiv, eDrawMode, nColor)`.
//
//  The slider: with a label the track starts at mid - 60 and the boxes at mid - 59; without one, mid - 145 and mid - 144 (ESCMENU_DragSlider's
//  two positions). The knob sits at `track left - (int)(-265.0f / (dwCount - 1) * nValue)`, the float read off `D8 3D` on 0xC3848000, so 265
//  pixels of travel. Two boxes 30 high fill the 289 pixels behind it, split at the knob's offset + 12:
//
//                   left box         right box        bar            knob Y
//      uniform      TRANS50 (1)      TRANS50 (1)      OptBarC        slider Y - 1
//      gauge        TRANS75 (2)      TRANS25 (0)      OptBar         slider Y - 2
//
//  bUniformTrack is set on 3DBias, Gamma and Contrast. The bar is drawn at mid + 230 with nDiv 2, or at mid with nDiv 1 on a labelless row,
//  and the knob is OptSkull through a zeroed D2GfxDataStrc, drawn with the row's mode.
//
//  The pentagram: two copies of the pent-spin cel beside the selected row, at mid - width - 249 and mid + 249, always DRAWMODE_NORMAL. The
//  right one plays frame n and the left one frame `8 - n` -- except that frame 0 stays 0 -- so the two spin in opposite directions. Every
//  50 ms (UI_ThrottleTick50ms) the frame steps `(n + 1) % 8` with a signed modulo, `and edx, 80000007h / jns`.

static_assert(DRAWMODE_TRANS25 == 0 && DRAWMODE_TRANS50 == 1
              && DRAWMODE_TRANS75 == 2 && DRAWMODE_NORMAL == 5,
              "ESCMENU_Draw's modes are `4 * enabled + 1`, the slider boxes' "
              "`2 - uniform` and `uniform != 0`, and `push 5` for the pentagram");
static_assert(offsetof(D2ClientEscMenuItemStrc, nScreenY) == 0x008
              && offsetof(D2ClientEscMenuItemStrc, bUniformTrack) == 0x128
              && offsetof(D2ClientEscMenuInfoStrc, nRowHeight) == 0x04
              && offsetof(D2ClientEscMenuInfoStrc, nLabelYOffset) == 0x08
              && offsetof(D2ClientEscMenuInfoStrc, nPentYOffset) == 0x0C
              && offsetof(D2ClientEscMenuInfoStrc, nSliderYOffset) == 0x10,
              "Draw writes a row's +8 and reads +0x128, and reads the page "
              "header's +4, +8, +0Ch and +10h");


//1.10f: D2Client.0x6FB088B0
//  Draws the menu; see the layout above.
//  BUG: `hChoiceImage[nValue]` is read with no check against dwCount, and Fade's refresh can leave a value past the row's choices:
//  AUTOMAP_LoadSettings loads AutoMapFade with no clamp and the Fade refresh lets 4 through (it resets only above 4) into a row of four
//  choices, so hChoiceImage[4] -- the next row's dwType, 1 for Center -- goes to D2Win as a cel file. LATENT: no in-game path writes a 4 (the
//  hotkey cycles `% 4` and the row steps within its count); a registry edit reaches it.
//  BUG: the Mode refresh applies 0..2 from an uninitialised local where AutoMapMode is absent, into a row of two choices, so a 2 draws
//  hChoiceImage[2], NULL in the file image. NOT ESTABLISHED: whether AUTOMAP_SetMode keeps a 2 and what D2Win does with a NULL cel file.
void __cdecl ESCMENU_Draw(void)
{
    D2GfxDataStrc tPentagram;
    D2GfxDataStrc tKnob;
    D2ClientEscMenuItemStrc *pRow;
    ESCMENUENABLEDFN pfnIsEnabled;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nSelected;
    int32_t nMenuTop;
    int32_t ii;
    int32_t nSlot;
    int32_t bEnabled;
    int32_t eDrawMode;
    int32_t nRowY;
    int32_t nLabelY;
    int32_t bNoLabel;
    int32_t nSliderY;
    int32_t nScreenMidX;
    int32_t nTrackLeft;
    int32_t nKnobX;
    int32_t nKnobY;
    int32_t nFilledWidth;
    int32_t nBoxX;
    int32_t nBarX;
    int32_t nBarDiv;
    int32_t nPentagramY;

    if (!sgptMenuInfo)
    {
        FOG_DisplayAssert("sgptMenuInfo", szRestartCpp, 1726);
        exit(-1);
    }

    if (!sgptMenuItem)
    {
        FOG_DisplayAssert("sgptMenuItem", szRestartCpp, 1727);
        exit(-1);
    }

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();

    nMenuTop = (gnScreenHeight - 80) / 2 - sgptMenuInfo->nRowHeight * sgptMenuInfo->nItemCount / 2;

    if (gnEscMenuLastMouseY != (uint32_t)nMouseY || gnEscMenuLastMouseX != (uint32_t)nMouseX)
    {
        gnEscMenuLastMouseY = nMouseY;
        gnEscMenuLastMouseX = nMouseX;

        if (gnEscMenuDragging)
        {
            ESCMENU_DragSlider(nMouseX, nMouseY);
        }
        else
        {
            nSelected = ESCMENU_HitTestRow(nMouseY);
            if (nSelected != -1)
                gnEscMenuSelectedItem = nSelected;
        }
    }

    nSlot = 0;
    for (ii = 0; ii < sgptMenuInfo->nItemCount; ++ii)
    {
        sgptMenuItem[ii].nScreenY = 0;

        if (sgptMenuItem[ii].bExpansionOnly && !FOG_IsExpansion())
            continue;

        bEnabled = 1;
        pfnIsEnabled = sgptMenuItem[ii].pfnIsEnabled;
        if (pfnIsEnabled && !pfnIsEnabled())
            bEnabled = 0;
        eDrawMode = 4 * (bEnabled != 0) + 1;

        pRow = &sgptMenuItem[ii];
        if (pRow->pfnOnDraw)
            pRow->pfnOnDraw(pRow, 0);

        //  nLabelYOffset is read before nScreenY is stored, as the bytes order it; both are ints behind two pointers, so the order is kept.
        nRowY = sgptMenuInfo->nRowHeight * nSlot + nMenuTop;
        nLabelY = sgptMenuInfo->nLabelYOffset + nRowY;
        sgptMenuItem[ii].nScreenY = nRowY;

        switch (sgptMenuItem[ii].dwType)
        {
        case -1:
        case 0:
            D2WIN_10134_DrawCellFile(sgptMenuItem[ii].hImage, gnScreenWidth / 2, nLabelY, 1, (DrawMode)eDrawMode, -1);
            break;

        case 1:
            D2WIN_10134_DrawCellFile(sgptMenuItem[ii].hImage, gnScreenWidth / 2 - 230, nLabelY, 0, (DrawMode)eDrawMode, -1);
            D2WIN_10134_DrawCellFile(sgptMenuItem[ii].hChoiceImage[sgptMenuItem[ii].nValue], gnScreenWidth / 2 + 230, nLabelY, 2, (DrawMode)eDrawMode, -1);
            break;

        case 2:
            if (sgptMenuItem[ii].hImage)
            {
                D2WIN_10134_DrawCellFile(sgptMenuItem[ii].hImage, gnScreenWidth / 2 - 230, nLabelY, 0, (DrawMode)eDrawMode, -1);
            }

            pRow = &sgptMenuItem[ii];
            bNoLabel = pRow->hImage == NULL;
            nSliderY = sgptMenuInfo->nSliderYOffset + nRowY;
            nScreenMidX = gnScreenWidth / 2;
            nTrackLeft = bNoLabel ? nScreenMidX - 145 : nScreenMidX - 60;

            nKnobX = nTrackLeft - (int32_t)(-265.0f / (double)(pRow->dwCount - 1) * pRow->nValue);
            nKnobY = nSliderY - (pRow->bUniformTrack == 0) - 1;
            nFilledWidth = nKnobX - nTrackLeft + 12;
            nBoxX = bNoLabel ? nScreenMidX - 144 : nScreenMidX - 59;

            GFX_DrawFilledRect(nBoxX, nSliderY - 30, nFilledWidth, 30, 0, pRow->bUniformTrack ? DRAWMODE_TRANS50 : DRAWMODE_TRANS75);
            GFX_DrawFilledRect(nBoxX + nFilledWidth, nSliderY - 30, 289 - nFilledWidth, 30, 0, pRow->bUniformTrack != 0);

            nBarX = gnScreenWidth / 2 + 230;
            nBarDiv = 2;
            if (bNoLabel)
            {
                nBarX = gnScreenWidth / 2;
                nBarDiv = 1;
            }

            D2WIN_10134_DrawCellFile(pRow->bUniformTrack ? ghEscMenuOptBarC : ghEscMenuOptBar, nBarX, nSliderY, nBarDiv, (DrawMode)eDrawMode, -1);

            memset(&tKnob, 0, sizeof(tKnob));
            tKnob.pCellFile = ghEscMenuOptSkull;
            D2GFX_TEXTURE_CelDraw(&tKnob, nKnobX, nKnobY, 0xFFFFFFFF, eDrawMode, NULL);
            break;
        }

        ++nSlot;
    }

    memset(&tPentagram, 0, sizeof(tPentagram));
    tPentagram.pCellFile = UI_GetPentSpinImage();
    if (gnEscMenuPentagramFrame)
        tPentagram.nFrame = 8 - gnEscMenuPentagramFrame;
    else
        tPentagram.nFrame = 0;

    nPentagramY = sgptMenuItem[gnEscMenuSelectedItem].nScreenY + sgptMenuInfo->nPentYOffset;

    D2GFX_TEXTURE_CelDraw(&tPentagram, gnScreenWidth / 2 - gnEscMenuPentagramWidth - 249, nPentagramY, 0xFFFFFFFF, DRAWMODE_NORMAL, NULL);

    tPentagram.nFrame = gnEscMenuPentagramFrame;
    D2GFX_TEXTURE_CelDraw(&tPentagram, gnScreenWidth / 2 + 249, nPentagramY, 0xFFFFFFFF, DRAWMODE_NORMAL, NULL);

    if (UI_ThrottleTick50ms())
        gnEscMenuPentagramFrame = (gnEscMenuPentagramFrame + 1) % 8;
}

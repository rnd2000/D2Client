//  Restart.h -- the globals of Restart.cpp that are ours: the ESC menu's seven page tables (a MenuInfo and an item array each), the float
//  literals its sliders compute with, the table of UI windows it closes and restores, and the ESC menu's and the mini panel's state.
//  Each is a C++17 inline variable, one object for every file that includes this header. The comment line "D2Client 0x6F..." above a global
//  gives its address in the original. The values are the image's, written by name: a function pointer as the function at that address, an
//  enum value by its member. The state past the end of .data's raw data (0x6FB9B000) is zero-filled by the loader and so zero-initialised here.
//  Six slots of the page tables take a cast, each the image's own pairing of a function with a slot of another type, to an ABI the function
//  meets: j_SOUNDCHAN_Toggle3dSound and j_SOUNDCHAN_ToggleEaxEffects are `int __cdecl()` in an ESCMENUITEMFN slot, `void
//  __fastcall(D2ClientEscMenuItemStrc *, SMSGHANDLER_PARAMS *)`: they read neither register and pop nothing, and the slot pushes nothing.
//  j_D2gfx_10007 and j_D2gfx_10019 (two slots each) are `int __stdcall()` in an ESCMENUENABLEDFN slot, `int __cdecl()`: no argument, so
//  nothing to pop either way.
//  The run-time fields start as the image has them: hImage and the four hChoiceImage are NULL until ESCMENU_LoadPageImages fills them, and
//  nValue is 0 until a page reads its setting. The page pointer sgptMenuItem is set by ESCMENU_SetPage, so it points here.
//  Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB7E048
//  The three top-page items: `Options`, `Exit`, `ReturnToGame`, all of them type 0 (a plain button, one cel). 1360 bytes each, so the array
//  ends exactly on gEscMenuOptions (0x6FB7E048 + 3 * 1360 = 0x6FB7F038).
inline D2ClientEscMenuItemStrc gEscMenuMainItems[3] =
{
    {   // [0]
        .dwType = 0,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "Options",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_GotoOptionsPage,
        .pfnRefresh = NULL,
        .pfnOnDraw = NULL,
        .dwCount = 0,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [1]
        .dwType = 0,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "Exit",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_OnExitGame,
        .pfnRefresh = NULL,
        .pfnOnDraw = NULL,
        .dwCount = 0,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [2]
        .dwType = 0,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "ReturnToGame",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_OnReturnToGame,
        .pfnRefresh = NULL,
        .pfnOnDraw = NULL,
        .dwCount = 0,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
};
static_assert(sizeof(gEscMenuMainItems) == 4080, "gEscMenuMainItems: the item's 4080 bytes in the .i64");

//  D2Client 0x6FB7F050
//  `SoundOptions`, `VideoOptions`, `AutoMapOptions`, `CfgOptions`, `Previous` -- all type 0. 5 * 1360 ends exactly on gEscMenuSound.
inline D2ClientEscMenuItemStrc gEscMenuOptionsItems[5] =
{
    {   // [0]
        .dwType = 0,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "SoundOptions",
        .pfnIsEnabled = &j_SOUNDCHAN_IsSystemReady_2,
        .pfnOnChange = &ESCMENU_GotoSoundPage,
        .pfnRefresh = NULL,
        .pfnOnDraw = NULL,
        .dwCount = 0,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [1]
        .dwType = 0,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "VideoOptions",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_GotoVideoPage,
        .pfnRefresh = NULL,
        .pfnOnDraw = NULL,
        .dwCount = 0,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [2]
        .dwType = 0,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "AutoMapOptions",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_GotoAutomapPage,
        .pfnRefresh = NULL,
        .pfnOnDraw = NULL,
        .dwCount = 0,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [3]
        .dwType = 0,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "CfgOptions",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_OpenConfigPanel,
        .pfnRefresh = NULL,
        .pfnOnDraw = NULL,
        .dwCount = 0,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [4]
        .dwType = 0,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "Previous",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_GotoMainPage,
        .pfnRefresh = NULL,
        .pfnOnDraw = NULL,
        .dwCount = 0,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
};
static_assert(sizeof(gEscMenuOptionsItems) == 6800, "gEscMenuOptionsItems: the item's 6800 bytes in the .i64");

//  D2Client 0x6FB80AF8
//  The Sound page, and the clearest example of the item types:
//
//      SoundOptions  type -1  the page title
//      Sound         type  2  slider, 21 steps
//      Music         type  2  slider, 21 steps
//      3DSound       type  1  choice, 2
//      EAX           type  1  choice, 2
//      3DBias        type  2  slider, 21 steps
//      NPCSpeech     type  1  choice, 3
//      SPrevious     type  0  button
//
//  Each non-title item carries its handler in the callback slot at +280. 8 * 1360 ends exactly on gEscMenuVideoClassic.
inline D2ClientEscMenuItemStrc gEscMenuSoundItems[8] =
{
    {   // [0]
        .dwType = -1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "SoundOptions",
        .pfnIsEnabled = NULL,
        .pfnOnChange = NULL,
        .pfnRefresh = NULL,
        .pfnOnDraw = NULL,
        .dwCount = 0,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [1]
        .dwType = 2,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "Sound",
        .pfnIsEnabled = &j_SOUNDCHAN_IsSystemReady_2,
        .pfnOnChange = &ESCMENU_SoundVolumeOnChange,
        .pfnRefresh = &ESCMENU_SoundVolumeRefresh,
        .pfnOnDraw = NULL,
        .dwCount = 21,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [2]
        .dwType = 2,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "Music",
        .pfnIsEnabled = &j_SOUNDCHAN_IsSystemReady_2,
        .pfnOnChange = &ESCMENU_MusicVolumeOnChange,
        .pfnRefresh = &ESCMENU_MusicVolumeRefresh,
        .pfnOnDraw = NULL,
        .dwCount = 21,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [3]
        .dwType = 1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "3DSound",
        .pfnIsEnabled = &j_SOUNDCHAN_Get3DEnabled,
        .pfnOnChange = (ESCMENUITEMFN)&j_SOUNDCHAN_Toggle3dSound,
        .pfnRefresh = &ESCMENU_Sound3DRefresh,
        .pfnOnDraw = &ESCMENU_Sound3DRefresh,
        .dwCount = 2,
        .nValue = 1,
        .bUniformTrack = 0,
        .pcChoiceFile = { "SmallOff", "SmallOn", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [4]
        .dwType = 1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "EAX",
        .pfnIsEnabled = &j_SOUNDCHAN_GetEaxEnabled,
        .pfnOnChange = (ESCMENUITEMFN)&j_SOUNDCHAN_ToggleEaxEffects,
        .pfnRefresh = &ESCMENU_SoundEaxRefresh,
        .pfnOnDraw = &ESCMENU_SoundEaxRefresh,
        .dwCount = 2,
        .nValue = 1,
        .bUniformTrack = 0,
        .pcChoiceFile = { "SmallOff", "SmallOn", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [5]
        .dwType = 2,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "3DBias",
        .pfnIsEnabled = &j_SOUNDCHAN_Is3DEnabled,
        .pfnOnChange = &ESCMENU_Sound3DBiasOnChange,
        .pfnRefresh = &ESCMENU_Sound3DBiasRefresh,
        .pfnOnDraw = NULL,
        .dwCount = 21,
        .nValue = 0,
        .bUniformTrack = 1,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [6]
        .dwType = 1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "NPCSpeech",
        .pfnIsEnabled = &j_SOUNDCHAN_IsSystemReady_2,
        .pfnOnChange = &ESCMENU_NpcSpeechOnChange,
        .pfnRefresh = &ESCMENU_NpcSpeechRefresh,
        .pfnOnDraw = NULL,
        .dwCount = 3,
        .nValue = 2,
        .bUniformTrack = 0,
        .pcChoiceFile = { "AudioOnly", "TextOnly", "AudioText", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [7]
        .dwType = 0,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "SPrevious",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_GotoOptionsPage,
        .pfnRefresh = NULL,
        .pfnOnDraw = NULL,
        .dwCount = 0,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
};
static_assert(sizeof(gEscMenuSoundItems) == 10880, "gEscMenuSoundItems: the item's 10880 bytes in the .i64");

//  D2Client 0x6FB83590
//  `VideoOptions` (title), `LightQuality` (choice, 3), `BlendShadow` (choice, 2), `Perspective` (choice, 2, GFX_TogglePerspective), `Gamma`
//  (slider, 21), `Contrast` (slider, 100), `SPrevious`. The expansion table is the same list with `Resolution` inserted second. 7 * 1360 ends
//  exactly on gEscMenuVideoExpansion.
inline D2ClientEscMenuItemStrc gEscMenuVideoClassicItems[7] =
{
    {   // [0]
        .dwType = -1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "VideoOptions",
        .pfnIsEnabled = NULL,
        .pfnOnChange = NULL,
        .pfnRefresh = NULL,
        .pfnOnDraw = NULL,
        .dwCount = 0,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [1]
        .dwType = 1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "LightQuality",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_LightQualityOnChange,
        .pfnRefresh = &ESCMENU_LightQualityRefresh,
        .pfnOnDraw = NULL,
        .dwCount = 3,
        .nValue = 2,
        .bUniformTrack = 0,
        .pcChoiceFile = { "Low", "Medium", "High", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [2]
        .dwType = 1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "BlendShadow",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_BlendShadowOnChange,
        .pfnRefresh = &ESCMENU_BlendShadowRefresh,
        .pfnOnDraw = NULL,
        .dwCount = 2,
        .nValue = 1,
        .bUniformTrack = 0,
        .pcChoiceFile = { "SmallOff", "SmallOn", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [3]
        .dwType = 1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "Perspective",
        .pfnIsEnabled = (ESCMENUENABLEDFN)&j_D2gfx_10007,
        .pfnOnChange = &ESCMENU_PerspectiveOnChange,
        .pfnRefresh = &ESCMENU_PerspectiveRefresh,
        .pfnOnDraw = NULL,
        .dwCount = 2,
        .nValue = 1,
        .bUniformTrack = 0,
        .pcChoiceFile = { "SmallOff", "SmallOn", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [4]
        .dwType = 2,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "Gamma",
        .pfnIsEnabled = (ESCMENUENABLEDFN)&j_D2gfx_10019,
        .pfnOnChange = &ESCMENU_GammaOnChange,
        .pfnRefresh = &ESCMENU_GammaRefresh,
        .pfnOnDraw = NULL,
        .dwCount = 21,
        .nValue = 0,
        .bUniformTrack = 1,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [5]
        .dwType = 2,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "Contrast",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_ContrastOnChange,
        .pfnRefresh = &ESCMENU_ContrastRefresh,
        .pfnOnDraw = NULL,
        .dwCount = 100,
        .nValue = 50,
        .bUniformTrack = 1,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [6]
        .dwType = 0,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "SPrevious",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_GotoOptionsPage,
        .pfnRefresh = NULL,
        .pfnOnDraw = NULL,
        .dwCount = 0,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
};
static_assert(sizeof(gEscMenuVideoClassicItems) == 9520, "gEscMenuVideoClassicItems: the item's 9520 bytes in the .i64");

//  D2Client 0x6FB85AD8
//  gEscMenuVideoClassicItems with `Resolution` (choice, 2) inserted as item 1. That item is the only one on the page with bExpansionOnly set,
//  which is belt and braces: it lives in a table the classic build never loads and carries the flag ESCMENU_LoadPageImages checks. 8 * 1360
//  ends exactly on gEscMenuAutomapClassic.
inline D2ClientEscMenuItemStrc gEscMenuVideoExpansionItems[8] =
{
    {   // [0]
        .dwType = -1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "VideoOptions",
        .pfnIsEnabled = NULL,
        .pfnOnChange = NULL,
        .pfnRefresh = NULL,
        .pfnOnDraw = NULL,
        .dwCount = 0,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [1]
        .dwType = 1,
        .bExpansionOnly = 1,
        .nScreenY = 0,
        .szImageFile = "Resolution",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_ResolutionOnChange,
        .pfnRefresh = &ESCMENU_ResolutionRefresh,
        .pfnOnDraw = NULL,
        .dwCount = 2,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "640x480", "800x600", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [2]
        .dwType = 1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "LightQuality",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_LightQualityOnChange,
        .pfnRefresh = &ESCMENU_LightQualityRefresh,
        .pfnOnDraw = NULL,
        .dwCount = 3,
        .nValue = 2,
        .bUniformTrack = 0,
        .pcChoiceFile = { "Low", "Medium", "High", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [3]
        .dwType = 1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "BlendShadow",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_BlendShadowOnChange,
        .pfnRefresh = &ESCMENU_BlendShadowRefresh,
        .pfnOnDraw = NULL,
        .dwCount = 2,
        .nValue = 1,
        .bUniformTrack = 0,
        .pcChoiceFile = { "SmallOff", "SmallOn", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [4]
        .dwType = 1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "Perspective",
        .pfnIsEnabled = (ESCMENUENABLEDFN)&j_D2gfx_10007,
        .pfnOnChange = &ESCMENU_PerspectiveOnChange,
        .pfnRefresh = &ESCMENU_PerspectiveRefresh,
        .pfnOnDraw = NULL,
        .dwCount = 2,
        .nValue = 1,
        .bUniformTrack = 0,
        .pcChoiceFile = { "SmallOff", "SmallOn", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [5]
        .dwType = 2,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "Gamma",
        .pfnIsEnabled = (ESCMENUENABLEDFN)&j_D2gfx_10019,
        .pfnOnChange = &ESCMENU_GammaOnChange,
        .pfnRefresh = &ESCMENU_GammaRefresh,
        .pfnOnDraw = NULL,
        .dwCount = 21,
        .nValue = 0,
        .bUniformTrack = 1,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [6]
        .dwType = 2,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "Contrast",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_ContrastOnChange,
        .pfnRefresh = &ESCMENU_ContrastRefresh,
        .pfnOnDraw = NULL,
        .dwCount = 100,
        .nValue = 50,
        .bUniformTrack = 1,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [7]
        .dwType = 0,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "SPrevious",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_GotoOptionsPage,
        .pfnRefresh = NULL,
        .pfnOnDraw = NULL,
        .dwCount = 0,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
};
static_assert(sizeof(gEscMenuVideoExpansionItems) == 10880, "gEscMenuVideoExpansionItems: the item's 10880 bytes in the .i64");

//  D2Client 0x6FB88570
//  `AutoMapOptions` (title), `AutoMapFade` (choice, 4 -- the only item that uses all of MAX_CHOICE), `AutoMapCenter`, `AutoMapParty`,
//  `AutoMapPartyNames` (choice, 2 each), `SPrevious`. 6 * 1360 ends exactly on gEscMenuAutomapExpansion.
inline D2ClientEscMenuItemStrc gEscMenuAutomapClassicItems[6] =
{
    {   // [0]
        .dwType = -1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "AutoMapOptions",
        .pfnIsEnabled = NULL,
        .pfnOnChange = NULL,
        .pfnRefresh = NULL,
        .pfnOnDraw = NULL,
        .dwCount = 0,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [1]
        .dwType = 1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "AutoMapFade",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_AutomapFadeOnChange,
        .pfnRefresh = &ESCMENU_AutomapFadeRefresh,
        .pfnOnDraw = &ESCMENU_AutomapFadeRefresh,
        .dwCount = 4,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "SmallNo", "Center", "Everything", "Auto" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [2]
        .dwType = 1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "AutoMapCenter",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_AutomapCenterOnChange,
        .pfnRefresh = &ESCMENU_AutomapCenterRefresh,
        .pfnOnDraw = &ESCMENU_AutomapCenterRefresh,
        .dwCount = 2,
        .nValue = 1,
        .bUniformTrack = 0,
        .pcChoiceFile = { "SmallNo", "SmallYes", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [3]
        .dwType = 1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "AutoMapParty",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_AutomapPartyOnChange,
        .pfnRefresh = &ESCMENU_AutomapPartyRefresh,
        .pfnOnDraw = &ESCMENU_AutomapPartyRefresh,
        .dwCount = 2,
        .nValue = 1,
        .bUniformTrack = 0,
        .pcChoiceFile = { "SmallNo", "SmallYes", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [4]
        .dwType = 1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "AutoMapPartyNames",
        .pfnIsEnabled = &j_AUTOMAP_GetShowParty,
        .pfnOnChange = &ESCMENU_AutomapPartyNamesOnChange,
        .pfnRefresh = &ESCMENU_AutomapPartyNamesRefresh,
        .pfnOnDraw = &ESCMENU_AutomapPartyNamesRefresh,
        .dwCount = 2,
        .nValue = 1,
        .bUniformTrack = 0,
        .pcChoiceFile = { "SmallNo", "SmallYes", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [5]
        .dwType = 0,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "SPrevious",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_GotoOptionsPage,
        .pfnRefresh = NULL,
        .pfnOnDraw = NULL,
        .dwCount = 0,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
};
static_assert(sizeof(gEscMenuAutomapClassicItems) == 8160, "gEscMenuAutomapClassicItems: the item's 8160 bytes in the .i64");

//  D2Client 0x6FB8A568
//  The expansion Automap page's seven items: gEscMenuAutomapClassicItems with `AutoMapMode` (choice, 2) inserted as item 1 and carrying
//  bExpansionOnly, as `Resolution` does on the video page.
inline D2ClientEscMenuItemStrc gEscMenuAutomapExpansionItems[7] =
{
    {   // [0]
        .dwType = -1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "AutoMapOptions",
        .pfnIsEnabled = NULL,
        .pfnOnChange = NULL,
        .pfnRefresh = NULL,
        .pfnOnDraw = NULL,
        .dwCount = 0,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [1]
        .dwType = 1,
        .bExpansionOnly = 1,
        .nScreenY = 0,
        .szImageFile = "AutoMapMode",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_AutomapModeOnChange,
        .pfnRefresh = &ESCMENU_AutomapModeRefresh,
        .pfnOnDraw = NULL,
        .dwCount = 2,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "Full", "Mini", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [2]
        .dwType = 1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "AutoMapFade",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_AutomapFadeOnChange,
        .pfnRefresh = &ESCMENU_AutomapFadeRefresh,
        .pfnOnDraw = &ESCMENU_AutomapFadeRefresh,
        .dwCount = 4,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "SmallNo", "Center", "Everything", "Auto" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [3]
        .dwType = 1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "AutoMapCenter",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_AutomapCenterOnChange,
        .pfnRefresh = &ESCMENU_AutomapCenterRefresh,
        .pfnOnDraw = &ESCMENU_AutomapCenterRefresh,
        .dwCount = 2,
        .nValue = 1,
        .bUniformTrack = 0,
        .pcChoiceFile = { "SmallNo", "SmallYes", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [4]
        .dwType = 1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "AutoMapParty",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_AutomapPartyOnChange,
        .pfnRefresh = &ESCMENU_AutomapPartyRefresh,
        .pfnOnDraw = &ESCMENU_AutomapPartyRefresh,
        .dwCount = 2,
        .nValue = 1,
        .bUniformTrack = 0,
        .pcChoiceFile = { "SmallNo", "SmallYes", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [5]
        .dwType = 1,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "AutoMapPartyNames",
        .pfnIsEnabled = &j_AUTOMAP_GetShowParty,
        .pfnOnChange = &ESCMENU_AutomapPartyNamesOnChange,
        .pfnRefresh = &ESCMENU_AutomapPartyNamesRefresh,
        .pfnOnDraw = &ESCMENU_AutomapPartyNamesRefresh,
        .dwCount = 2,
        .nValue = 1,
        .bUniformTrack = 0,
        .pcChoiceFile = { "SmallNo", "SmallYes", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
    {   // [6]
        .dwType = 0,
        .bExpansionOnly = 0,
        .nScreenY = 0,
        .szImageFile = "SPrevious",
        .pfnIsEnabled = NULL,
        .pfnOnChange = &ESCMENU_GotoOptionsPage,
        .pfnRefresh = NULL,
        .pfnOnDraw = NULL,
        .dwCount = 0,
        .nValue = 0,
        .bUniformTrack = 0,
        .pcChoiceFile = { "", "", "", "" },
        .hImage = NULL,
        .hChoiceImage = { NULL, NULL, NULL, NULL },
    },
};
static_assert(sizeof(gEscMenuAutomapExpansionItems) == 9520, "gEscMenuAutomapExpansionItems: the item's 9520 bytes in the .i64");

//  D2Client 0x6FB6FB50
//  0.01, the reciprocal of gfPercentDivisor: the change handlers `fdivr` by 100.0 to turn a slider into a fraction, the refresh handlers
//  `fmul` by 0.01 (ESCMENU_SoundVolumeRefresh, MusicVolumeRefresh, Sound3DBiasRefresh and ContrastRefresh, the same four sliders that read
//  gfPercentDivisor going in). Named for the value like its twin.
inline const float gfPercentScale = 0.009999999776482582f;
static_assert(sizeof(gfPercentScale) == 4, "gfPercentScale: the item's 4 bytes in the .i64");

//  D2Client 0x6FB6FB54
//  55.0, the low end of the gamma range, read by ESCMENU_GammaOnChange and ESCMENU_GammaRefresh: with gfConst200 the slider maps onto
//  [55, 255]. GammaRefresh runs the same three instructions to go back the other way.
inline const float gfGammaMin = 55.0f;
static_assert(sizeof(gfGammaMin) == 4, "gfGammaMin: the item's 4 bytes in the .i64");

//  D2Client 0x6FB6FB58
//  The pooled literal 200.0. ESCMENU_GammaOnChange and ESCMENU_GammaRefresh `fdivr` by it to span the gamma range with gfGammaMin (55 + 200 =
//  255); SOUNDHDR_GetRangeParam `fld`s it as a switch case value. Named for the value, since identical float literals are pooled and these
//  readers share nothing.
inline const float gfConst200 = 200.0f;
static_assert(sizeof(gfConst200) == 4, "gfConst200: the item's 4 bytes in the .i64");

//  D2Client 0x6FB6FB5C
//  0.005, exactly 1/200, the reciprocal of gfConst200: ESCMENU_GammaRefresh `fmul`s by it where ESCMENU_GammaOnChange `fdivr`s by 200.0, so the
//  pair is the same conversion run in both directions, as gfPercentScale (0.01) against gfPercentDivisor (100.0).
inline const float gfGammaScale = 0.004999999888241291f;
static_assert(sizeof(gfGammaScale) == 4, "gfGammaScale: the item's 4 bytes in the .i64");

//  D2Client 0x6FB6FB60
//  The constant 1.0, `fadd`ed in ESCMENU_DragSlider.
inline const float gfOne = 1.0f;
static_assert(sizeof(gfOne) == 4, "gfOne: the item's 4 bytes in the .i64");

//  D2Client 0x6FB6FB64
//  132.5, half the slider track: ESCMENU_DragSlider `fdivr`s by it, and ESCMENU_Draw `fdivr`s by -265.0 (gfSliderWidthNeg), which is exactly
//  -2 x 132.5. The drag works from the centre outwards and the draw from one end.
inline const float gfSliderHalfWidth = 132.5f;
static_assert(sizeof(gfSliderHalfWidth) == 4, "gfSliderHalfWidth: the item's 4 bytes in the .i64");

//  D2Client 0x6FB6FB68
//  -265.0, the full slider track, negated: exactly -2 x gfSliderHalfWidth (132.5), `fdivr`ed by ESCMENU_Draw.
inline const float gfSliderWidthNeg = -265.0f;
static_assert(sizeof(gfSliderWidthNeg) == 4, "gfSliderWidthNeg: the item's 4 bytes in the .i64");

//  D2Client 0x6FB7DE68
//  38 records indexed by UI window id; 0x6FB7DE68 + 38 * 12 is exactly gEscMenuMain. In the image bCloseOnEscMenu is clear for 0 and 9 only,
//  bRestoreFlag is set for 6, 7, 10, 17, 21 and 35, and nSavedState is zero. ESCMENU_SetPage writes nSavedState; ESCMENU_CloseMenu,
//  OnExitGame and OnReturnToGame read it by index, and MINIPANEL_SaveAndUnload reads record 21 directly.
inline D2ClientEscMenuUiWindowStrc gEscMenuUiWindows[38] =
{
    {},   // [0]
    { .bCloseOnEscMenu = 1 },   // [1]
    { .bCloseOnEscMenu = 1 },   // [2]
    { .bCloseOnEscMenu = 1 },   // [3]
    { .bCloseOnEscMenu = 1 },   // [4]
    { .bCloseOnEscMenu = 1 },   // [5]
    { .bCloseOnEscMenu = 1, .bRestoreFlag = 1 },   // [6]
    { .bCloseOnEscMenu = 1, .bRestoreFlag = 1 },   // [7]
    { .bCloseOnEscMenu = 1 },   // [8]
    {},   // [9]
    { .bCloseOnEscMenu = 1, .bRestoreFlag = 1 },   // [10]
    { .bCloseOnEscMenu = 1 },   // [11]
    { .bCloseOnEscMenu = 1 },   // [12]
    { .bCloseOnEscMenu = 1 },   // [13]
    { .bCloseOnEscMenu = 1 },   // [14]
    { .bCloseOnEscMenu = 1 },   // [15]
    { .bCloseOnEscMenu = 1 },   // [16]
    { .bCloseOnEscMenu = 1, .bRestoreFlag = 1 },   // [17]
    { .bCloseOnEscMenu = 1 },   // [18]
    { .bCloseOnEscMenu = 1 },   // [19]
    { .bCloseOnEscMenu = 1 },   // [20]
    { .bCloseOnEscMenu = 1, .bRestoreFlag = 1 },   // [21]
    { .bCloseOnEscMenu = 1 },   // [22]
    { .bCloseOnEscMenu = 1 },   // [23]
    { .bCloseOnEscMenu = 1 },   // [24]
    { .bCloseOnEscMenu = 1 },   // [25]
    { .bCloseOnEscMenu = 1 },   // [26]
    { .bCloseOnEscMenu = 1 },   // [27]
    { .bCloseOnEscMenu = 1 },   // [28]
    { .bCloseOnEscMenu = 1 },   // [29]
    { .bCloseOnEscMenu = 1 },   // [30]
    { .bCloseOnEscMenu = 1 },   // [31]
    { .bCloseOnEscMenu = 1 },   // [32]
    { .bCloseOnEscMenu = 1 },   // [33]
    { .bCloseOnEscMenu = 1 },   // [34]
    { .bCloseOnEscMenu = 1, .bRestoreFlag = 1 },   // [35]
    { .bCloseOnEscMenu = 1 },   // [36]
    { .bCloseOnEscMenu = 1 },   // [37]
};
static_assert(sizeof(gEscMenuUiWindows) == 456, "gEscMenuUiWindows: the item's 456 bytes in the .i64");

//  D2Client 0x6FB7E030
//  MenuInfo of the ESC menu's top page, 24 bytes, immediately followed by its item array; nItemCount 3 (Options, Exit, ReturnToGame).
inline D2ClientEscMenuInfoStrc gEscMenuMain = { .nItemCount = 3, .nRowHeight = 50, .nLabelYOffset = 39, .nPentYOffset = 51 };
static_assert(sizeof(gEscMenuMain) == 24, "gEscMenuMain: the item's 24 bytes in the .i64");

//  D2Client 0x6FB7F038
//  MenuInfo of the Options page; nItemCount 5, the same 24-byte shape as gEscMenuMain.
inline D2ClientEscMenuInfoStrc gEscMenuOptions = { .nItemCount = 5, .nRowHeight = 50, .nLabelYOffset = 39, .nPentYOffset = 51 };
static_assert(sizeof(gEscMenuOptions) == 24, "gEscMenuOptions: the item's 24 bytes in the .i64");

//  D2Client 0x6FB80AE0
//  MenuInfo of the Sound page; nItemCount 8.
inline D2ClientEscMenuInfoStrc gEscMenuSound = { .nItemCount = 8, .nRowHeight = 45, .nLabelYOffset = 34, .nPentYOffset = 49, .nSliderYOffset = 36 };
static_assert(sizeof(gEscMenuSound) == 24, "gEscMenuSound: the item's 24 bytes in the .i64");

//  D2Client 0x6FB83578
//  MenuInfo of the Video page as the classic build shows it; nItemCount 7. ESCMENU_SetPage, ESCMENU_LoadImages and ESCMENU_UnloadImages pick
//  between this and gEscMenuVideoExpansion through FOG_IsExpansion.
inline D2ClientEscMenuInfoStrc gEscMenuVideoClassic =
{ .nItemCount = 7, .nRowHeight = 45, .nLabelYOffset = 34, .nPentYOffset = 49, .nSliderYOffset = 36 };
static_assert(sizeof(gEscMenuVideoClassic) == 24, "gEscMenuVideoClassic: the item's 24 bytes in the .i64");

//  D2Client 0x6FB85AC0
//  MenuInfo of the Video page in the expansion build; nItemCount 8, one more than the classic table.
inline D2ClientEscMenuInfoStrc gEscMenuVideoExpansion =
{ .nItemCount = 8, .nRowHeight = 45, .nLabelYOffset = 34, .nPentYOffset = 49, .nSliderYOffset = 36 };
static_assert(sizeof(gEscMenuVideoExpansion) == 24, "gEscMenuVideoExpansion: the item's 24 bytes in the .i64");

//  D2Client 0x6FB88558
//  MenuInfo of the Automap page as the classic build shows it; nItemCount 6.
inline D2ClientEscMenuInfoStrc gEscMenuAutomapClassic =
{ .nItemCount = 6, .nRowHeight = 45, .nLabelYOffset = 34, .nPentYOffset = 49, .nSliderYOffset = 36 };
static_assert(sizeof(gEscMenuAutomapClassic) == 24, "gEscMenuAutomapClassic: the item's 24 bytes in the .i64");

//  D2Client 0x6FB8A550
//  MenuInfo of the Automap page in the expansion build; nItemCount 7.
inline D2ClientEscMenuInfoStrc gEscMenuAutomapExpansion =
{ .nItemCount = 7, .nRowHeight = 45, .nLabelYOffset = 34, .nPentYOffset = 49, .nSliderYOffset = 36 };
static_assert(sizeof(gEscMenuAutomapExpansion) == 24, "gEscMenuAutomapExpansion: the item's 24 bytes in the .i64");

//  D2Client 0x6FBB6F38
//  Left edge of each mini-panel button, filled at layout time. The hit test is strict at both ends, x < mouse < x + 20, so the buttons are 20
//  pixels wide and the array is 8 entries (7 in single player).
inline int32_t gaMiniPanelButtonX[8];
static_assert(sizeof(gaMiniPanelButtonX) == 32, "gaMiniPanelButtonX: the item's 32 bytes in the .i64");

//  D2Client 0x6FBB6F58
//  Top edge of each mini-panel button. MINIPANEL_DrawButtonHint uses it only to place the tooltip, at `y - 26`; the vertical hit test is
//  against the screen bottom rather than against this.
inline int32_t gaMiniPanelButtonY[8];
static_assert(sizeof(gaMiniPanelButtonY) == 32, "gaMiniPanelButtonY: the item's 32 bytes in the .i64");

//  D2Client 0x6FBB6F78
//  Which button each slot holds -- the value the hint's switch and the action dispatcher both key on. The slots are positional and the ids are
//  not: in single player the panel has seven slots and the Party Screen id is simply absent from the array, rather than slot 3 being blanked.
inline int32_t gaMiniPanelButtonId[8];
static_assert(sizeof(gaMiniPanelButtonId) == 32, "gaMiniPanelButtonId: the item's 32 bytes in the .i64");

//  D2Client 0x6FBB6F98
//  One dword per mini-panel button, 1 while it is held down. Read by MINIPANEL_Draw and MINIPANEL_DrawButtons to add 1 to the button's cel
//  frame; set by MINIPANEL_OnLButtonDown, cleared wholesale by MINIPANEL_OnLButtonUp. 8 entries (index 7 is read and written in multiplayer);
//  MINIPANEL_LoadImages clears 8 dwords in a multiplayer game and 7 in single player. MINIPANEL_OnLButtonUp's wholesale clear is skipped on
//  three exits: no player, an item on the cursor, the panel hidden.
inline int32_t gbMiniPanelButtonPressed[8];
static_assert(sizeof(gbMiniPanelButtonPressed) == 32, "gbMiniPanelButtonPressed: the item's 32 bytes in the .i64");

//  D2Client 0x6FBB6FB8
//  The base cel frame in gpImgMiniPanelButtons for each mini-panel button, written by MINIPANEL_LayOutButtons and read by MINIPANEL_Draw and
//  MINIPANEL_DrawButtons, which add 1 to it while the button is held. 8 entries; the neighbour gnEscMenuSelectedItem is esc-menu state and this
//  array is not. The frame is 2 * the button's id, not 2 * its slot: in single player MINIPANEL_LayOutButtons adds 2 at slot 3, so slots 3..6
//  get 8, 10, 12, 14.
inline int32_t gaMiniPanelButtonFrame[8];
static_assert(sizeof(gaMiniPanelButtonFrame) == 32, "gaMiniPanelButtonFrame: the item's 32 bytes in the .i64");

//  D2Client 0x6FBB6FD8
//  Index of the highlighted row on the current page. ESCMENU_SetPage puts it on the last item (`nItemCount - 1`), which is the Previous/Return
//  entry, so ESC lands on the way out; for page 5 it gets item 1 instead. ESCMENU_Draw moves it as the mouse moves, and it is what the
//  pentagram is drawn beside.
inline int32_t gnEscMenuSelectedItem;
static_assert(sizeof(gnEscMenuSelectedItem) == 4, "gnEscMenuSelectedItem: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB6FDC
//  The current ESC-menu page's header, one of the seven gEscMenu* pairs. Named from the assert in ESCMENU_Draw (Restart.cpp line 1726). Set by
//  ESCMENU_SetPage and ESCMENU_LoadImages; cleared by ESCMENU_UnloadImages. D2ClientEscMenuInfoStrc, 24 bytes -- exactly the gap between every
//  page's header and its item array:
//
//      +0x00  nItemCount      the item array starts at +24
//      +0x04  nRowHeight      the pitch; nItemCount * this is the block height
//      +0x08  nLabelYOffset   row top -> the label cel's baseline
//      +0x0C  nPentYOffset    row top -> the selection pentagram
//      +0x10  nSliderYOffset  row top -> a type-2 row's track; 0 on the two pages with no slider, Main and Options
//      +0x14  field_14        zero in all seven pages, read by nothing
//
//  Main and Options are {n, 50, 39, 51, 0, 0}; the other five {n, 45, 34, 49, 36, 0}.
inline D2ClientEscMenuInfoStrc *sgptMenuInfo;
static_assert(sizeof(sgptMenuInfo) == 4, "sgptMenuInfo: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB6FE0
//  The current ESC-menu page's item array, the pair of sgptMenuInfo. Named from the assert at Restart.cpp line 1727. Written by
//  ESCMENU_SetPage and ESCMENU_LoadImages; cleared by ESCMENU_UnloadImages. D2ClientEscMenuItemStrc, 1360 bytes, three dispatch shapes:
//
//      +0x110 pfnIsEnabled  no argument is set up; return LIVE
//      +0x114 pfnOnChange   ecx = row, edx = the message record on the activate path, 0 elsewhere; return discarded
//      +0x118 pfnRefresh    ecx = row, edx = 0; ESCMENU_LoadPageImages only
//      +0x11C pfnOnDraw     ecx = row, edx = 0; ESCMENU_Draw only
//
//  +0x11C is never its own function: where set it repeats +0x118. At ESCMENU_Draw's `call eax` for pfnIsEnabled (0x6FB089CE) ecx holds
//  sgptMenuItem itself, the base the slot was read through, not a row.
inline D2ClientEscMenuItemStrc *sgptMenuItem;
static_assert(sizeof(sgptMenuItem) == 4, "sgptMenuItem: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB6FE4
//  The selection pentagram's animation frame, cycled `(n + 1) % 8` at the end of every ESCMENU_Draw that UI_ThrottleTick50ms says should
//  advance. The two pentagrams are drawn out of phase: the left one at `8 - n`, the right at `n`, so they spin in opposite directions.
inline int32_t gnEscMenuPentagramFrame;
static_assert(sizeof(gnEscMenuPentagramFrame) == 4, "gnEscMenuPentagramFrame: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB6FE8
//  Non-zero while the left button is held on the ESC menu: set by ESCMENU_OnLButtonDown on any left press (it calls ESCMENU_DragSlider and
//  stores 1 after it with no test), cleared by ESCMENU_OnLButtonUp. While it is set, a mouse move in ESCMENU_Draw drags the slider
//  (ESCMENU_DragSlider) instead of moving the row highlight, which is why dragging past a row does not change the selection.
inline BOOL gnEscMenuDragging;
static_assert(sizeof(gnEscMenuDragging) == 4, "gnEscMenuDragging: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB6FEC
//  Set while an ESC-menu slider is being dragged: ESCMENU_DragSlider sets it once the cursor has landed inside a slider's track, and
//  ESCMENU_OnLButtonUp clears it. While it is set, ESCMENU_DragSlider skips both of its guards -- the row under the cursor no longer has to be
//  the selected one, and the X no longer has to be inside the track -- so a drag continues off the end of the slider and off the row.
inline BOOL gbEscMenuSliderDragging;
static_assert(sizeof(gbEscMenuSliderDragging) == 4, "gbEscMenuSliderDragging: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB6FF0
//  `DATA\GLOBAL\UI\Widgets\OptBar` -- the empty slider trough drawn behind every type-2 ESC-menu item. Loaded by ESCMENU_LoadImages, freed by
//  ESCMENU_UnloadImages.
inline D2CellFileStrc *ghEscMenuOptBar;
static_assert(sizeof(ghEscMenuOptBar) == 4, "ghEscMenuOptBar: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB6FF4
//  `DATA\GLOBAL\UI\Widgets\OptBarC` -- the slider trough in its 'on' colour; ESCMENU_Draw picks it over ghEscMenuOptBar depending on the item's
//  +296.
inline D2CellFileStrc *ghEscMenuOptBarC;
static_assert(sizeof(ghEscMenuOptBarC) == 4, "ghEscMenuOptBarC: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB6FF8
//  `DATA\GLOBAL\UI\Widgets\OptSkull` -- the slider knob, drawn at `-265.0 / (dwCount - 1) * nValue` along a 265-pixel track.
inline D2CellFileStrc *ghEscMenuOptSkull;
static_assert(sizeof(ghEscMenuOptSkull) == 4, "ghEscMenuOptSkull: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB6FFC
//  The widest cel in the selection-pentagram cel file, measured once by ESCMENU_LoadImages by walking every cel of
//  D2CMP_CELCMP_CelFileGetCelsPerDirection. ESCMENU_Draw subtracts it when placing the LEFT pentagram (`gnScreenWidth / 2 - width - 249`) so the pair
//  stays symmetric about the centre whatever the animation frame is.
inline int32_t gnEscMenuPentagramWidth;
static_assert(sizeof(gnEscMenuPentagramWidth) == 4, "gnEscMenuPentagramWidth: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB7000
//  Last mouse position ESCMENU_Draw saw, with gnEscMenuLastMouseY. The hover test and the slider drag only run when one of the two changed.
inline int32_t gnEscMenuLastMouseX;
static_assert(sizeof(gnEscMenuLastMouseX) == 4, "gnEscMenuLastMouseX: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB7004
//  The Y half of the pair; see gnEscMenuLastMouseX.
inline int32_t gnEscMenuLastMouseY;
static_assert(sizeof(gnEscMenuLastMouseY) == 4, "gnEscMenuLastMouseY: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB7008
//  The mini panel's background cel: `Panel\minipanel` in a multiplayer game, `Panel\minipanel_s` in single player. MINIPANEL_LoadImages picks
//  between them on GAME_GetGameType and sets gbMiniPanelSinglePlayer with it -- two different bitmaps, not one with a button hidden, which is
//  why the button count drops from 8 to 7 at the same time.
inline D2CellFileStrc *gpImgMiniPanel;
static_assert(sizeof(gpImgMiniPanel) == 4, "gpImgMiniPanel: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB700C
//  `Panel\minipanelbtn`, the mini panel's button cel. One image for both game types, unlike the background beside it. Released by
//  MINIPANEL_SaveAndUnload.
inline D2CellFileStrc *gpImgMiniPanelButtons;
static_assert(sizeof(gpImgMiniPanelButtons) == 4, "gpImgMiniPanelButtons: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB7010
//  Set by MINIPANEL_Draw when an inventory-side window is open -- inventory, skill tree, stash, cube, shop, trade, guild vault, message log,
//  mini skill, or the belt rows. Read by MINIPANEL_OnLButtonDown and MINIPANEL_OnLButtonUp, where it shifts the panel's hit-test 118 pixels
//  left, which identifies the group as the right-hand side of the screen.
inline int32_t gbMiniPanelInvSideOpen;
static_assert(sizeof(gbMiniPanelInvSideOpen) == 4, "gbMiniPanelInvSideOpen: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB7014
//  Set by MINIPANEL_Draw when any of about fifteen UI windows is open -- the inventory-side group plus quest, waypoint, merc inventory, stats
//  and party. Read by MINIPANEL_OnLButtonDown and MINIPANEL_OnLButtonUp, where it shifts the hit-test 119 pixels right. With both flags set the
//  shifts (-118, then +119) net +1; MINIPANEL_Draw hides the panel entirely in that case, and gbMiniPanelHidden makes
//  MINIPANEL_OnLButtonDown return before the shift is used.
inline int32_t gbMiniPanelWindowOpen;
static_assert(sizeof(gbMiniPanelWindowOpen) == 4, "gbMiniPanelWindowOpen: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB7018
//  1 while the mini-panel is not being drawn -- set by MINIPANEL_Draw on the branch where a left-hand and a right-hand window are both open and
//  there is no room for it, cleared on every branch that draws something. MINIPANEL_OnLButtonDown returns early when it is set, so the buttons
//  cannot be clicked through whatever covers them: the drawing pass telling the input pass what it did, one frame earlier.
inline int32_t gbMiniPanelHidden;
static_assert(sizeof(gbMiniPanelHidden) == 4, "gbMiniPanelHidden: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB701C
//  One reference in the module, a write of 0 by MINIPANEL_OnLButtonUp (0x6FB098BB) on the panel-hidden and click-missed paths, where
//  gbMiniPanelButtonDown has already been cleared; nothing reads it. It sits between gbMiniPanelHidden and gbMiniPanelSinglePlayer. The name
//  says "unread", not a meaning.
inline int32_t gbMiniPanelFlagUnread;
static_assert(sizeof(gbMiniPanelFlagUnread) == 4, "gbMiniPanelFlagUnread: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB7020
//  Set by MINIPANEL_LoadImages when `GAME_GetGameType()` is 0. It selects the single-player mini-panel bitmap and drops the button count from
//  8 to 7, the missing one being the Party Screen. In single player the button does not exist -- the panel is drawn with seven and the hit
//  test never produces index 3 -- so the `GAME_GetGameType()` test inside MINIPANEL_RunButtonAction's case 3 is a second guard.
inline BOOL gbMiniPanelSinglePlayer;
static_assert(sizeof(gbMiniPanelSinglePlayer) == 4, "gbMiniPanelSinglePlayer: the item's 4 bytes in the .i64");

//  D2Client 0x6FBB7024
//  The mini panel button's pressed state: armed by MINIPANEL_OnLButtonDown, read and cleared by MINIPANEL_OnLButtonUp, and read by nothing
//  else, so the flag exists only to pair a release with its press.
inline BOOL gbMiniPanelButtonDown;
static_assert(sizeof(gbMiniPanelButtonDown) == 4, "gbMiniPanelButtonDown: the item's 4 bytes in the .i64");

//  CoreArchive.h -- the globals of CoreArchive.cpp's data that are ours: the window caption, the path literal of the cel file loaders, the screen
//  and viewport sizes and the high-resolution flag. Each is a C++17 `inline` variable, one object for every file that includes this header. The
//  comment line "D2Client 0x6F..." above a global gives its address in the original.
//  The screen globals are here by position: the data of the original's CORE\ARCHIVE.CPP comes first in the image, and they are read by many other
//  units. A header that declares a global initialised from these (QuestLog.h does for gQuestLogCloseRects) includes this one first, so that the
//  C++17 initialisation order described at gnViewportBottom holds for it too. Include it after D2Client.h.
#pragma once

//  D2Client 0x6FB6E0AC
//  Points at "D2Client.dll", the caption FAULT_PromptErrorOrCancel passes to MessageBoxA. Head of a four-pointer group in .rdata: this caption,
//  gpszLngLibName, gpszLngInitFailedText and gpszLngErrorText, the strings the LNG start-up path needs.
inline const LPCSTR gpszD2ClientCaption = "D2Client.dll";
static_assert(sizeof(gpszD2ClientCaption) == 4, "gpszD2ClientCaption: the item's 4 bytes in the .i64");

//  D2Client 0x6FB740A0
//  The path literal the three cel file functions pass as __FILE__.
inline char szSrcFile[61] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\CORE\\ARCHIVE.CPP";
static_assert(sizeof(szSrcFile) == 61, "szSrcFile: the item's 61 bytes in the .i64");

//  D2Client 0x6FB740EC
//  The screen width, 640 in the image.
inline int32_t gnScreenWidth = 640;
static_assert(sizeof(gnScreenWidth) == 4, "gnScreenWidth: the item's 4 bytes in the .i64");

//  D2Client 0x6FB740F0
//  The screen height, 480 in the image.
inline int32_t gnScreenHeight = 480;
static_assert(sizeof(gnScreenHeight) == 4, "gnScreenHeight: the item's 4 bytes in the .i64");

//  D2Client 0x6FB740F4
//  A third copy of the screen width, written by WNDPROC_SetResolutionMode from the same value it puts in gnViewportRight.
inline int32_t gnDrawMaxWidth = 640;
static_assert(sizeof(gnDrawMaxWidth) == 4, "gnDrawMaxWidth: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9A700
//  The bottom edge of the play area: gnScreenHeight - 40, assigned by WNDPROC_SetResolutionMode. GAME_WindowOnRButtonDown compares a click's y
//  against it, so it is the boundary between the playfield and the strip below.
//  The image holds 0 at this address, in the zero-filled part of .data. The original's CRT_StaticInit02 fills it at load from gnScreenHeight
//  (440 at the image's 480), and the declaration does what the initialiser did. The order is C++17's for inline variables defined in one header
//  (partially-ordered initialization): a variable defined before another in every translation unit that defines the other is initialised first,
//  and gnScreenHeight is defined above this one in every file that includes CoreArchive.h (its literal initialiser also makes it
//  constant-initialised, which happens before any dynamic initialisation).
inline int32_t gnViewportBottom = gnScreenHeight - 40;
static_assert(sizeof(gnViewportBottom) == 4, "gnViewportBottom: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9A704
//  The right edge of the play area. WNDPROC_SetResolutionMode assigns it the screen width, 640 or 800, in every branch, so it tracks gnScreenWidth
//  exactly; it is still a separate global, so the two are not interchangeable.
//  The image holds 0 here too; the original's CRT_StaticInit01 fills it at load from gnScreenWidth (640 at the image). The declaration does what
//  the initialiser did, in the order described at gnViewportBottom.
inline int32_t gnViewportRight = gnScreenWidth;
static_assert(sizeof(gnViewportRight) == 4, "gnViewportRight: the item's 4 bytes in the .i64");

//  D2Client 0x6FB9A708
//  1 for 800x600 and 0 for 640x480: written in exactly two places, both in WNDPROC_SetResolutionMode, and read mostly as the `bHigherRes`
//  argument of the DATATBLS_Get*FromBeltsTxt calls.
inline int32_t gbHighResolution;
static_assert(sizeof(gbHighResolution) == 4, "gbHighResolution: the item's 4 bytes in the .i64");

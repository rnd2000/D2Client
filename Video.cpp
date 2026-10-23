#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "WINMAIN.h"
#include "Video.h"
#include "Lng.h"

//  Video.cpp -- the cinematic player of D2Client.dll 1.10: VIDEO_PlayCinematic plays one of the eight Smacker videos (and the expansion's
//  act V intro after Diablo), VIDEO_RecordCinematicProgress keeps a progress mask in the registry, and four one-liners keep the cinematic
//  state flags. The original source file is Core\Video.cpp (the assert "VideoId < NumVideos", line 237, in VIDEO_PlayCinematic names it).
//  Address range 0x6FAA1510..0x6FAA1CF5, six functions.

//  The source file name, as the assert in VIDEO_PlayCinematic pushes it.
static const char szSourceFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\Core\\Video.cpp";

//  The registry value the progress is kept in and its format; the key, "Diablo II", is szKeyName.
static const char szAuxBattleNet[] = "Aux Battle.net";
static const char szQuadFormat[] = "%d.%d.%d.%d";

//  The video id is the record's first dword (the index is scaled by three dwords).
static_assert(offsetof(D2CinematicEntry, nVideoId) == 0,
              "VIDEO_RecordCinematicProgress reads the video id at +0");

//1.10f: D2Client.0x6FAA1510
//  Records how far into the cinematics the player has got, as a bit mask in the fourth field of a dotted quad, "216.148.246.<mask>", stored
//  in the registry value "Aux Battle.net" under "Diablo II". void: neither call reads the stored rank or SRegSaveString's answer.
//
//  The stored rank: nMask starts at 0x22 and sscanf fills it with the fourth field; its low byte is decoded highest rank first: 0x01 is 7,
//  0x80 is 6, 0x10 is 5, 0x08 is 4, 0x40 is 3, and otherwise 2 if 0x04 is set, else 1. The three first fields are parsed into ints and
//  never read.
//
//  The new rank, from the video id: 2, 3 and 4 are themselves, 5 and 6 are both 5, 7 is 7, and anything else is 1. Only a rank strictly
//  above the stored one is written (unsigned bytes), as the mask for that rank: 2 0x26, 3 0x62, 4 0x2A, 5 0xB2, 7 0x23, else 0x22 --
//  rank 5's mask carries 0x80 too, so it reads back as 6. The first three fields are written as 216, 148 and 246 whatever they were.
//  The new rank is stored into nMask's low byte and read back masked with 0xFF; one variable is two here.
void __fastcall VIDEO_RecordCinematicProgress(int32_t nCinematicIndex)
{
    int32_t nVideoId;
    int32_t nQuadField1;
    int32_t nQuadField2;
    int32_t nQuadField3;
    int32_t nMask;
    int32_t nNewMask;
    uint8_t nMaskByte;
    uint8_t nStoredRank;
    uint8_t nRank;
    char szValue[100];

    nVideoId = gCinematics[nCinematicIndex].nVideoId;
    nMask = 0x22;

    if (STORM_SRegLoadString(szKeyName, szAuxBattleNet, 0, szValue, sizeof(szValue)))
    {
        sscanf(szValue, szQuadFormat, &nQuadField1, &nQuadField2, &nQuadField3, &nMask);
        nMaskByte = (uint8_t)nMask;
        if (nMaskByte & 0x01)
            nStoredRank = 7;
        else if (nMaskByte & 0x80)
            nStoredRank = 6;
        else if (nMaskByte & 0x10)
            nStoredRank = 5;
        else if (nMaskByte & 0x08)
            nStoredRank = 4;
        else if (nMaskByte & 0x40)
            nStoredRank = 3;
        else
            nStoredRank = (uint8_t)(((nMaskByte & 0x04) != 0) + 1);
    }
    else
    {
        //  BUG: when the value cannot be read the stored rank is 0x22 (34), the raw mask and not its decoding (which would be 1). No rank reaches 34,
        //  so nothing is ever written: a registry without the value never gets one from here. LATENT: D2Launch writes the value,
        //  "216.148.246.34", whenever it cannot read it, before any game starts.
        nStoredRank = 0x22;
    }

    switch (nVideoId)
    {
    case 2:
        nRank = 2;
        break;
    case 3:
        nRank = 3;
        break;
    case 4:
        nRank = 4;
        break;
    case 5:
    case 6:
        nRank = 5;
        break;
    case 7:
        nRank = 7;
        break;
    default:
        nRank = 1;
        break;
    }

    if (nRank <= nStoredRank)
        return;

    switch (nRank)
    {
    case 2:
        nNewMask = 0x26;
        break;
    case 3:
        nNewMask = 0x62;
        break;
    case 4:
        nNewMask = 0x2A;
        break;
    case 5:
        nNewMask = 0xB2;
        break;
    case 7:
        nNewMask = 0x23;
        break;
    default:
        nNewMask = 0x22;
        break;
    }

    sprintf(szValue, szQuadFormat, 216, 148, 246, nNewMask);
    STORM_SRegSaveString(szKeyName, szAuxBattleNet, 0, szValue);
}

//  The player's record is a 12-byte stride, read at +4 (the name), +8 and +9.
static_assert(sizeof(D2CinematicEntry) == 12
              && offsetof(D2CinematicEntry, pszFileName) == 4
              && offsetof(D2CinematicEntry, bLocalized) == 8
              && offsetof(D2CinematicEntry, bHasResolutionSuffix) == 9,
              "VIDEO_PlayCinematic's record: 12 bytes, read at +4, +8, +9");
//  The player's UI id, 0x12, is loaded before all three UI_SetUIToggle calls.
static_assert(UI_UNKNOWN18 == 0x12, "the player's UI id is 18");
//  D2Lang's language table gives language 0 the code "ENG".
static_assert(LANGUAGE_ENGLISH == 0,
              "`test eax, eax / jz` at 0x6FAA18E4 is the English test");

//1.10f: D2Client.0x6FAA1680
//  Plays one cinematic and, when the flag for it is set, the expansion's intro after it. Called by GAME_AppModeGame with 5 and 7 and by
//  QUESTS_PlayCinematicFromPacket, which zero-extends the packet's byte at +1 and tail-jumps here: an id is 0..255, and 8 and up from a
//  server hits the assert. The original has the video file search written out twice, for the id and for video 6.
//
//  gCinematics is eight 12-byte records. +9 says the name carries a `%s` for the size, "640x292" or "640x146"; +8, bLocalized, says the
//  file is localised: under DATA\LOCAL\video\<language>\ when set, DATA\GLOBAL\video\ when clear. The half-height "640x146" goes with
//  resolution mode 2, D2GAMERES_800x600; the full size keeps mode 0.
//
//  The assert `VideoId < NumVideos` is line 237 and signed. An entry while a cinematic is on screen returns at once with gbCinematicPlaying
//  already set and the progress already recorded; every other path out clears the playing flag.
//
//  The file search: a localised name is tried in the table's own language first -- D2Lang #10006 copies, for argument 0, one of 13 strings,
//  and every one is three letters and a NUL, so a 4-byte buffer is exact. When Storm cannot open that file, a second name is built: "ENG"
//  for any language but English, and "CHI" for English. Whether the second opens changes nothing but a close: the name is used either way.
//
//  Video 7 is played only if "d2game.dll" opens in the install directory (FOG_GetInstallPath, an inline strcat, CreateFileA with
//  OPEN_EXISTING); why that file, the bytes do not say. When it does not open, nothing is played and command 0x48 is not sent.
//
//  Diablo in an expansion game: gbActVIntroPending is set by quest 23 (A4Q2, Diablo) in an expansion game
//  (QUESTLOG_QuestOnQuestStatePacket). With it set the player plays video 6, D2X_INTRO, after the first, records its progress when it
//  opens, and loads act V's palette (D2Win #10177 with 4). After video 5 it jumps straight on: the UI stays in its cinematic state between
//  the two. After any other video it first closes the cutscene as usual, and D2Win #10004 then runs twice. Once video 6's archives load,
//  the id is 6, so video 7's teardown below does not follow.
//
//  D2Win #10003 decides whether a cutscene opens at all (the test here reads eax). The per-frame callback is MSG_SendPing6C, except that the
//  first video gets NULL while gbGameCompletedClassic is set: no keep-alive through the ending of a game about to be left. Video 6 always
//  pings. D2Win #10004 gets D2Gfx's resolution mode. The way out: command 0x48, then the classic completion's leave or video 7's teardown
//  and leave, then the play disc's archives back, the playing flag cleared, the trace.
void __fastcall VIDEO_PlayCinematic(int32_t nCinematicId)
{
    D2CinematicEntry *pEntry;
    D2GameResolutionMode nResolution;
    BOOL bArchivesLoaded;
    BOOL bCutsceneOpened;
    BOOL bExpansionDisc;
    HANDLE hGameDll;
    int32_t hVideoFile;
    char szLanguage[4];
    char szFileName[260];
    char szBaseName[260];
    char szInstallPath[260];

    if (nCinematicId >= gnCinematicCount)
    {
        FOG_DisplayAssert("VideoId < NumVideos", szSourceFile, 237);
        exit(-1);
    }

    gbCinematicPlaying = 1;
    VIDEO_RecordCinematicProgress(nCinematicId);
    if (gbCinematicOnScreen)
        return;

    //  The original compares again; it cannot fail, the assert above has exited.
    if (nCinematicId >= gnCinematicCount)
    {
        MSG_SendCmd1(0x48);
        gbCinematicPlaying = 0;
        return;
    }

    bArchivesLoaded = FALSE;
    pEntry = &gCinematics[nCinematicId];
    nResolution = D2GAMERES_640x480;
    if (pEntry->bHasResolutionSuffix)
    {
        if (gbUseFullSizeCinematics)
        {
            sprintf(szBaseName, pEntry->pszFileName, "640x292");
        }
        else
        {
            nResolution = D2GAMERES_800x600;
            sprintf(szBaseName, pEntry->pszFileName, "640x146");
        }
    }
    else
    {
        //  The original copies the name with an inline byte-copy loop.
        strcpy(szBaseName, pEntry->pszFileName);
    }

    FOG_Trace("video sound prep start %d\n", GetTickCount());
    SOUNDHDR_StopAllPending();
    SOUNDSONG_Init();
    FOG_Trace("video sound prep end %d\n", GetTickCount());
    D2WIN_ARCHIVE_UnloadExpansionArchives();

    bCutsceneOpened = FALSE;
    bExpansionDisc = FALSE;
    if (nCinematicId == 7)
    {
        bExpansionDisc = TRUE;
        FOG_GetInstallPath((uint32_t)(UINT_PTR)szInstallPath, sizeof(szInstallPath));
        strcat(szInstallPath, "d2game.dll");
        hGameDll = CreateFileA(szInstallPath, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hGameDll == INVALID_HANDLE_VALUE)
        {
            //  HARMLESS: called again at the label, the ending's own teardown; two identical calls, reproduced.
            GAME_SetD2xOutCinematicPending(FALSE);
            goto D2X_OUT_TEARDOWN;
        }
        CloseHandle(hGameDll);
    }
    else if (nCinematicId == 6)
    {
        bExpansionDisc = TRUE;
    }

    INSERTDISC_SetDiscPromptKind(bExpansionDisc);
    FOG_Trace("switch to video cd %d\n", GetTickCount());
    if (D2WIN_ARCHIVE_LoadVideoArchives(bExpansionDisc ? INSERTDISC_ShowInsertPlayDiscPrompt : INSERTDISC_ShowInsertCinematicsDiscPrompt, NULL, bExpansionDisc))
    {
        bArchivesLoaded = TRUE;

        if (pEntry->bLocalized)
        {
            D2LANG_10006((uint32_t)(UINT_PTR)szLanguage, 0);
            sprintf(szFileName, "%s\\video\\%s\\%s", "DATA\\LOCAL", szLanguage, szBaseName);
            if (STORM_SFileOpenFileEx(0, szFileName, 0, &hVideoFile))
            {
                STORM_SFileCloseFile((void *)(UINT_PTR)hVideoFile);
            }
            else if (D2LANG_STRTABLE_GetLanguage() != LANGUAGE_ENGLISH)
            {
                sprintf(szFileName, "%s\\video\\%s\\%s", "DATA\\LOCAL", "ENG", szBaseName);
                if (STORM_SFileOpenFileEx(0, szFileName, 0, &hVideoFile))
                    STORM_SFileCloseFile((void *)(UINT_PTR)hVideoFile);
            }
            else
            {
                sprintf(szFileName, "%s\\video\\%s\\%s", "DATA\\LOCAL", "CHI", szBaseName);
                if (STORM_SFileOpenFileEx(0, szFileName, 0, &hVideoFile))
                    STORM_SFileCloseFile((void *)(UINT_PTR)hVideoFile);
            }
        }
        else
        {
            sprintf(szFileName, "%s\\video\\%s", "DATA\\GLOBAL", szBaseName);
        }

        gbCinematicOnScreen = 1;
        DLIGHTMAP_CancelWarpFlash();
        UI_SetUIToggle(UI_UNKNOWN18, UI_TURNON, FALSE);
        if (D2WIN_10003())
        {
            if (gbGameCompletedClassic)
                D2GFX_WINDOW_OpenSmackCutscene(szFileName, nResolution, NULL);
            else
                D2GFX_WINDOW_OpenSmackCutscene(szFileName, nResolution, MSG_SendPing6C);
            bCutsceneOpened = TRUE;
        }

        //  Video 5 with the flag set runs straight on into video 6.
        if (nCinematicId != 5 || !gbActVIntroPending)
        {
            D2WIN_10004(D2GFX_GetResolutionMode());
            UI_SetUIToggle(UI_UNKNOWN18, UI_TURNOFF, FALSE);
            gbCinematicOnScreen = 0;
        }
    }

    if (gbActVIntroPending)
    {
        if (bArchivesLoaded)
            D2WIN_ARCHIVE_UnloadExpansionArchives();
        if (bCutsceneOpened)
            D2WIN_10004(D2GFX_GetResolutionMode());

        INSERTDISC_SetDiscPromptKind(TRUE);
        if (D2WIN_ARCHIVE_LoadVideoArchives(INSERTDISC_ShowInsertPlayDiscPrompt, NULL, TRUE))
        {
            nCinematicId = 6;
            if (gbUseFullSizeCinematics)
            {
                sprintf(szBaseName, gCinematics[6].pszFileName, "640x292");
            }
            else
            {
                nResolution = D2GAMERES_800x600;
                sprintf(szBaseName, gCinematics[6].pszFileName, "640x146");
            }

            if (gCinematics[6].bLocalized)
            {
                D2LANG_10006((uint32_t)(UINT_PTR)szLanguage, 0);
                sprintf(szFileName, "%s\\video\\%s\\%s", "DATA\\LOCAL", szLanguage, szBaseName);
                if (STORM_SFileOpenFileEx(0, szFileName, 0, &hVideoFile))
                {
                    STORM_SFileCloseFile((void *)(UINT_PTR)hVideoFile);
                }
                else if (D2LANG_STRTABLE_GetLanguage() != LANGUAGE_ENGLISH)
                {
                    sprintf(szFileName, "%s\\video\\%s\\%s", "DATA\\LOCAL", "ENG", szBaseName);
                    if (STORM_SFileOpenFileEx(0, szFileName, 0, &hVideoFile))
                        STORM_SFileCloseFile((void *)(UINT_PTR)hVideoFile);
                }
                else
                {
                    sprintf(szFileName, "%s\\video\\%s\\%s", "DATA\\LOCAL", "CHI", szBaseName);
                    if (STORM_SFileOpenFileEx(0, szFileName, 0, &hVideoFile))
                        STORM_SFileCloseFile((void *)(UINT_PTR)hVideoFile);
                }
            }
            else
            {
                sprintf(szFileName, "%s\\video\\%s", "DATA\\GLOBAL", szBaseName);
            }

            if (D2WIN_10003())
            {
                D2GFX_WINDOW_OpenSmackCutscene(szFileName, nResolution, MSG_SendPing6C);
                VIDEO_RecordCinematicProgress(6);
            }
        }

        D2WIN_10004(D2GFX_GetResolutionMode());
        UI_SetUIToggle(UI_UNKNOWN18, UI_TURNOFF, FALSE);
        gbCinematicOnScreen = 0;
        gbActVIntroPending = 0;
        D2WIN_10177(4);
    }

    MSG_SendCmd1(0x48);
    if (gbGameCompletedClassic)
    {
        gbGameCompletedClassic = 0;
        GAME_LeaveGameAfterCinematic();
    }
    else if (nCinematicId == 7)
    {
D2X_OUT_TEARDOWN:
        GAME_SetD2xOutCinematicPending(FALSE);
        gbGameCompletedClassic = 0;
        GAME_SetGameTeardownActive();
        GAME_LeaveGameAfterCinematic();
    }

    D2WIN_ARCHIVE_UnloadExpansionArchives();
    D2WIN_ARCHIVE_LoadExpansionArchives(INSERTDISC_ShowInsertPlayDiscPrompt, INSERTDISC_ShowInsertExpansionDiscPrompt, NULL, gpLaunchConfig);
    gbCinematicPlaying = 0;
    FOG_Trace("switch to play cd %d\n", GetTickCount());
}

//  Cinematic state: four one-liners on the byte flags at 0x6FB9A820..0x6FB9A824; VIDEO_PlayCinematic is their reader.

//1.10f: D2Client.0x6FAA1CA0
//  Sets gbGameCompletedClassic: the classic game is completed. Reached by a tail jump from QUESTLOG_QuestOnQuestStatePacket's
//  game-completed case on its classic arm.
void __cdecl VIDEO_SetGameCompletedClassic()
{
    gbGameCompletedClassic = 1;
}

//1.10f: D2Client.0x6FAA1CB0
//  Once per game entry, from GAME_AppModeGame: clears the three flags and chooses the cinematics' size, full size unless Fog's #10191 answers
//  non-zero. The stores are in the bytes' order: classic, full size, expansion, playing.
void __cdecl VIDEO_ResetCinematicState()
{
    gbGameCompletedClassic = 0;
    gbUseFullSizeCinematics = FOG_10191() == 0;
    gbActVIntroPending = 0;
    gbCinematicPlaying = 0;
}

//1.10f: D2Client.0x6FAA1CE0
//  Quest 23 (A4Q2, Diablo) completed in an expansion game: the same case's expansion arm, reached by a tail jump. Not the expansion's end --
//  the player reads the flag to go on into act V.
void __cdecl VIDEO_SetActVIntroPending()
{
    gbActVIntroPending = 1;
}

//1.10f: D2Client.0x6FAA1CF0
//  Whether VIDEO_PlayCinematic is running; the callers are in the quest log.
uint8_t __cdecl VIDEO_IsCinematicPlaying()
{
    return gbCinematicPlaying;
}

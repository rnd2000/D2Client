#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "CoreArchive.h"
#include "ui.h"
#include "inv.h"
#include "QuestLog.h"

//  QuestLog.cpp -- the quest log of D2Client.dll 1.10: the quest-log frame and its close box, the quest screen (act tabs, quest icons, close and
//  speech buttons), opening and closing the panel, drawing it, building each entry's display info, the quest-list packets, and the quest
//  callbacks that sit among them (the QUEST_ functions at the end of the file).
//  The original source file is UI\QuestLog.cpp (four functions of this unit push its name to an assert). Address range 0x6FB04140..0x6FB06FAF.
//
//  gQuestlogMessageHandlers is four records registered under UI_QUESTLOG, the RIGHT button first: WM_RBUTTONDOWN and WM_LBUTTONDOWN both reach
//  QUESTLOG_OnMouseDown, WM_RBUTTONUP and WM_LBUTTONUP both reach QUESTLOG_OnMouseUp, so a right-click on the close box closes it exactly as a
//  left-click does. gQuestscreenMessageHandlers is seven records over three functions, registered under UI_QUESTSCREEN: both button-downs reach
//  QUESTLOG_QuestScreenOnMouseDown, both ups QUESTLOG_QuestScreenOnMouseUp, and the remaining three slots are all VK_ESCAPE pointing at
//  QUESTLOG_QuestScreenOnDismissKey -- three separate rebindable key bindings for one handler, at 0x6FB8EAC8, 0x6FB8EAD4 and 0x6FB8EAE0.
//
//  The close box moves four ways. gQuestLogCloseRects is four rects and the row is picked from two things at once -- whether the UI is shifted
//  (`SCROLL_GetUiShiftScreen() == 2`) and whether the resolution is 800x600:
//
//      the NEW-STATS panel open and the old one closed
//          shifted      2 * (res != 800x600) + 1     ->  1 or 3
//          not shifted  (res != 800x600) ? 2 : 0     ->  0 or 2
//      otherwise
//          not shifted  (res != 800x600) ? 2 : 0     ->  0 or 2
//          shifted      800x600, or the STATSCREEN open  ->  1
//                       else                             ->  3
//
//  The panel's own origin is `(gnScreenWidth - 640) / 2` when shifted and 0 when not. The vertical offset is `gnScreenHeight - 480` on both
//  edges, so the rect's Y is stored in 640x480 space like the character screen's stat buttons. The origin is computed TWICE, once inside the
//  index selection and once after it, and the two are equal on all four paths -- the first feeds the left edge and the second the right.
//  The selector is written out in QUESTLOG_OnMouseDown, in QUESTLOG_OnMouseUp and twice in QUESTLOG_DrawFrameAndTitle.
//
//  The two close-box handlers are the same function with the test inverted: the press REJECTS a click outside the rect (`<=`, `>=`) and the
//  release ACCEPTS one inside it (`>`, `<`) -- and the release adds one term the press does not have, gbQuestLogCloseBoxDown, so a release only
//  closes the panel if the press armed it. The shared half is written twice.

//  Constants the file leans on.
static_assert(UI_QUESTLOG == 17 && UI_QUESTSCREEN == 15,
              "two DIFFERENT panels, and this file toggles both");
static_assert(UI_NEWSTATS == 6 && UI_STATSCREEN == 2,
              "and two more, which the close path turns off together");
static_assert(sizeof(D2WinCommandRegisterStrc) == 12,
              "the SMSGHANDLER record stride -- the three data slots that "
              "reference QUESTLOG_QuestScreenOnDismissKey are 12 bytes apart, which "
              "is how seven records over three functions was read");


//1.10f: D2Client.0x6FB06320
//
//  A press on the quest log, bound to BOTH mouse buttons. It arms gbQuestLogCloseBoxDown and claims the message when the click is inside the
//  close box, and clears the flag and passes the message on when it is not. Nothing else on the panel is hit-tested here: the act tabs and the
//  quest icons are handled by QUESTLOG_QuestScreenOnMouseDown, which is a different table.
void __stdcall QUESTLOG_OnMouseDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    int32_t nUiShiftScreen;
    D2GameResolutionMode eResolution;
    int32_t iCloseRect;
    int32_t nPanelLeft;
    int32_t nCloseLeft;
    int32_t nPanelLeft2;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    nUiShiftScreen = SCROLL_GetUiShiftScreen();

    if (UI_GetUiWindowState(UI_NEWSTATS) && !UI_GetUiWindowState(UI_STATSCREEN))
    {
        eResolution = D2GFX_GetResolutionMode();
        if (nUiShiftScreen == 2)
        {
            iCloseRect = 2 * (eResolution != D2GAMERES_800x600) + 1;
            nPanelLeft = (gnScreenWidth - 640) / 2;
        }
        else
        {
            iCloseRect = (eResolution != D2GAMERES_800x600) ? 2 : 0;
            nPanelLeft = 0;
        }
    }
    else
    {
        eResolution = D2GFX_GetResolutionMode();
        if (nUiShiftScreen != 2)
        {
            iCloseRect = (eResolution != D2GAMERES_800x600) ? 2 : 0;
            nPanelLeft = 0;
        }
        else
        {
            if (eResolution == D2GAMERES_800x600 || UI_GetUiWindowState(UI_STATSCREEN))
            {
                iCloseRect = 1;
            }
            else
            {
                iCloseRect = 3;
            }
            nPanelLeft = (gnScreenWidth - 640) / 2;
        }
    }

    nCloseLeft = nPanelLeft + gQuestLogCloseRects[iCloseRect].nLeft;

    //  the same value again; the first feeds the left edge and this the right
    if (nUiShiftScreen == 2)
        nPanelLeft2 = (gnScreenWidth - 640) / 2;
    else
        nPanelLeft2 = 0;

    if (nXpos <= nCloseLeft || nXpos >= nPanelLeft2 + gQuestLogCloseRects[iCloseRect].nRight || nYpos <= gQuestLogCloseRects[iCloseRect].nTop + gnScreenHeight - 480 || nYpos >= gQuestLogCloseRects[iCloseRect].nBottom + gnScreenHeight - 480)
    {
        gbQuestLogCloseBoxDown = 0;
        pMsg->bReturn = 0;
        return;
    }

    gbQuestLogCloseBoxDown = 1;
    SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FB06480
//
//  A release on the quest log, the half that closes it. The rect arithmetic above the final test is the press handler's; what differs is the
//  test's sense and the extra term:
//
//      the press    rejects when the point is OUTSIDE
//      the release  acts when it is INSIDE and gbQuestLogCloseBoxDown
//
//  and closing is two calls, not one: `UI_SetUIToggle(UI_QUESTLOG, UI_TURNOFF, 0)` puts the window away and `QUESTLOG_TogglePanel(0)` runs the
//  panel's own teardown -- unload the images, stamp the close tick, tell the server. Either alone would leave the other half stale.
//  QUESTLOG_TogglePanel takes one argument: edx at this call holds whatever UI_SetUIToggle left, and the callee never reads it.
void __stdcall QUESTLOG_OnMouseUp(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    int32_t nUiShiftScreen;
    D2GameResolutionMode eResolution;
    int32_t iCloseRect;
    int32_t nPanelLeft;
    int32_t nCloseLeft;
    int32_t nPanelLeft2;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    nUiShiftScreen = SCROLL_GetUiShiftScreen();

    if (UI_GetUiWindowState(UI_NEWSTATS) && !UI_GetUiWindowState(UI_STATSCREEN))
    {
        eResolution = D2GFX_GetResolutionMode();
        if (nUiShiftScreen == 2)
        {
            iCloseRect = 2 * (eResolution != D2GAMERES_800x600) + 1;
            nPanelLeft = (gnScreenWidth - 640) / 2;
        }
        else
        {
            iCloseRect = (eResolution != D2GAMERES_800x600) ? 2 : 0;
            nPanelLeft = 0;
        }
    }
    else
    {
        eResolution = D2GFX_GetResolutionMode();
        if (nUiShiftScreen != 2)
        {
            iCloseRect = (eResolution != D2GAMERES_800x600) ? 2 : 0;
            nPanelLeft = 0;
        }
        else
        {
            if (eResolution == D2GAMERES_800x600 || UI_GetUiWindowState(UI_STATSCREEN))
            {
                iCloseRect = 1;
            }
            else
            {
                iCloseRect = 3;
            }
            nPanelLeft = (gnScreenWidth - 640) / 2;
        }
    }

    nCloseLeft = nPanelLeft + gQuestLogCloseRects[iCloseRect].nLeft;

    if (nUiShiftScreen == 2)
        nPanelLeft2 = (gnScreenWidth - 640) / 2;
    else
        nPanelLeft2 = 0;

    if (nXpos > nCloseLeft && nXpos < nPanelLeft2 + gQuestLogCloseRects[iCloseRect].nRight && nYpos > gQuestLogCloseRects[iCloseRect].nTop + gnScreenHeight - 480 && nYpos < gQuestLogCloseRects[iCloseRect].nBottom + gnScreenHeight - 480
        && gbQuestLogCloseBoxDown)
    {
        UI_SetUIToggle(UI_QUESTLOG, UI_TURNOFF, 0);
        QUESTLOG_TogglePanel(0);
        gbQuestLogCloseBoxDown = 0;
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        return;
    }

    gbQuestLogCloseBoxDown = 0;
    pMsg->bReturn = 0;
}

//  The path literal both asserts in this file push. The second assert's text is
//  "sgQuestLog.nPageActiveButtonCount <= sizeof(sgQuestLogButtonPositions)/sizeof(QUESTLOGBUTTONPOSITIONT)", so gnQuestLogEntryCount is a
//  member of a struct called `sgQuestLog` in the original.
static const char szQuestLogCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UI\\QuestLog.cpp";

static const char szQuestLogButtonAssert[] = "sgQuestLog.nPageActiveButtonCount <= sizeof(sgQuestLogButtonPositions)/sizeof(QUESTLOGBUTTONPOSITIONT)";

//1.10f: D2Client.0x6FB060C0
//
//  The quest screen's dismiss key. It has no code caller: its only references are three data slots at 0x6FB8EAC8, 0x6FB8EAD4 and 0x6FB8EAE0,
//  twelve bytes apart (the SMSGHANDLER record stride). All three are VK_ESCAPE in the stock table and all three are rebindable:
//  UI_ApplyPanelKeyBindings rewrites entries 5 and 6 of gQuestscreenPanelKeys, so Escape is a default.
//  It consumes the message FIRST and closes the panel afterwards. `gnQuestLogPanelState == 2` is the only state that gets cleared; the other
//  values fall through with the panel state left standing, and the images are unloaded and the panel toggled off either way.
//  The close tick is GetTickCount(), stored where callers read it. void: Storm's SMSGHANDLER typedef returns void (eax still holds the tick at
//  the retn).
void __stdcall QUESTLOG_QuestScreenOnDismissKey(SMSGHANDLER_PARAMS *pMsg)
{
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    if (gnQuestLogPanelState == 2)
    {
        gnQuestLogPanelState = 0;
        gbQuestLogCloseBoxDown = 0;
        gbQuestScreenSpeechPlaying = 0;
    }

    QUESTLOG_QuestUnloadPanelImages();
    UI_SetUIToggle(UI_QUESTSCREEN, UI_TURNOFF, 1);
    gdwQuestLogCloseTick = GetTickCount();
}

//1.10f: D2Client.0x6FB05A30
//
//  Pressing a mouse button on the quest screen. WM_LBUTTONDOWN and WM_RBUTTONDOWN both point here, so the right button does what the left one does.
//  Three regions in order: the ACT TABS along the top, the six QUEST ICONS, and then the close and speech buttons at the bottom.
//
//  The entry gate has a resolution seam, and both handlers of this panel carry it:
//
//      800x600     nYpos <= gnScreenHeight - 47
//      otherwise   nYpos <= gnScreenHeight - 49
//
//  The tab strip is a search loop that walks `nTabX` forward (64-wide tabs in expansion, 80-wide in classic, 320 total, 28 tall); the waypoint
//  panel has the same geometry written as nested comparisons.
//  Two of the loop's guards can never fire. `iActTab` starts at 0 and only increments, so `iActTab != -1` is always true; and in classic the
//  loop exits at `nTabXClassic >= 320` with iActTab already 4, so the `nAct == 4` refusal for non-expansion is unreachable from here. Both are
//  written out.
//
//  Switching acts flushes every unread quest: before the tab changes, every entry whose `nState` is 0 gets
//  `QUESTRECORD_SetQuestState(record, nQuestId, 12)` and a client command 0x58 carrying the quest id. State 12 is the "seen" bit; the walk is
//  over `gnQuestLogEntryCount` entries of 618 bytes each.
//
//  `gnQuestLogEntryCount > 6` halts at an assert (QuestLog.cpp:1848), and the icon boxes come from `sgQuestLogButtonPositions`, six records of
//  16 bytes. Each box is 56 by 52 and EXCLUSIVE at the far edge (`>= left`, `< left + 56`).
//
//  The close box and the speech box disagree on their edges, and they sit in adjacent arms of one `if`; reproduced:
//
//      close   278..313, 392..425   INCLUSIVE both ends
//      speech  229..260, 390..424   EXCLUSIVE both ends
void __stdcall QUESTLOG_QuestScreenOnMouseDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    int32_t nPanelX;
    int32_t nPanelY;
    int32_t iActTab;
    int32_t nTabX;
    int32_t nAct;
    int32_t iEntry;
    int32_t iButton;
    int32_t nHitX;
    D2BitBufferStrc *pQuestRecord;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;

    if (VIDEO_IsCinematicPlaying())
    {
        return;
    }
    if (nXpos > gnScreenWidth / 2)
    {
        return;
    }
    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600)
    {
        if (nYpos > gnScreenHeight - 47)
        {
            return;
        }
    }
    else if (nYpos > gnScreenHeight - 49)
    {
        return;
    }

    nPanelX = nXpos - gnBorderXDifference;
    nPanelY = nYpos + gnBorderYDifference;

    if (gbQuestScreenSpeechPlaying)
    {
        goto CONSUME;
    }

    if ((uint32_t)nPanelY < 0x1C && (uint32_t)nPanelX < 0x140)
    {
        iActTab = 0;
        nTabX = 0;
        if (FOG_IsExpansion() && GAME_IsExpansionGame())
        {
            while (nPanelX < nTabX || nPanelX > nTabX + 64)
            {
                nTabX += 64;
                ++iActTab;
                if (nTabX >= 320)
                {
                    goto ICONS;
                }
            }
        }
        else
        {
            while (nPanelX < nTabX || nPanelX > nTabX + 80)
            {
                nTabX += 80;
                ++iActTab;
                if (nTabX >= 320)
                {
                    goto ICONS;
                }
            }
        }

        nAct = iActTab;
        //  iActTab is 0..4 here, so this can never fail
        if (iActTab != -1)
        {
            if (iActTab < 5 && iActTab != gnQuestLogActTab)
            {
                //  and classic never reaches tab 4, so this cannot fire either
                if ((!FOG_IsExpansion() || !GAME_IsExpansionGame()) && nAct == 4)
                {
                    goto CONSUME;
                }

                //  every unread quest is marked seen before the act changes
                for (iEntry = 0; iEntry < (int32_t)gnQuestLogEntryCount; ++iEntry)
                {
                    if (!gQuestLogEntries[iEntry].nState)
                    {
                        pQuestRecord = NPCMENU_QuestGetQuestRecord();
                        D2COMMON_QUESTRECORD_SetQuestState(pQuestRecord, gQuestLogEntries[iEntry].nQuestStateFlag, 12);
                        MSG_SendCmdWord(88, (int16_t)gQuestLogEntries[iEntry].nQuestStateFlag);
                    }
                }

                QUESTLOG_SetActTab(nAct);
                QUESTLOG_SelectActPage(gnQuestLogActTab, 1);
                gbQuestScreenIconDown = 0;
                gbQuestScreenCloseButtonDown = 0;
                gbQuestLogCloseBoxDown = 0;
                gnQuestScreenPressedQuest = -1;
                gbQuestScreenSpeechButtonDown = 0;
                gbDurielCofLoaded = 0;
                gbAncientStatueCofLoaded = 0;
                gbZooArmed = 0;
                gbZooChickensSpawned = 0;
            }
            goto CONSUME;
        }
    }

ICONS:
    if ((uint32_t)gnQuestLogEntryCount > 6)   // `jbe`, unsigned (0x6FB05C3B)
    {
        FOG_DisplayAssert((uint32_t)szQuestLogButtonAssert, (uint32_t)szQuestLogCpp, 1848);
        exit(-1);
    }

    if ((int32_t)gnQuestLogEntryCount > 0)
    {
        for (iButton = 0; ; ++iButton)
        {
            if (iButton >= (int32_t)gnQuestLogEntryCount)
            {
                goto BOTTOM_BUTTONS;
            }
            nHitX = sgQuestLogButtonPositions[iButton].nHitX;
            if (nPanelX >= nHitX && nPanelX < nHitX + 56 && nPanelY >= sgQuestLogButtonPositions[iButton].nHitY && nPanelY < sgQuestLogButtonPositions[iButton].nHitY + 52)
            {
                break;
            }
        }

        //  iButton cannot be -1 here either
        if (iButton != -1 && iButton < (int32_t)gnQuestLogEntryCount)
        {
            for (iEntry = 0; iEntry < (int32_t)gnQuestLogEntryCount; ++iEntry)
            {
                if (iButton == gQuestLogEntries[iEntry].nSlotInPage && gQuestLogEntries[iEntry].bHasText)
                {
                    gbQuestScreenIconDown = 1;
                    gnQuestScreenPressedQuest = iButton;
                    SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
                }
            }
            goto CONSUME;
        }
    }

BOTTOM_BUTTONS:
    if (nPanelX >= 278 && nPanelX <= 313 && nPanelY >= 392 && nPanelY <= 425)
    {
        gbQuestScreenCloseButtonDown = 1;
    }
    else if (nPanelX > 229 && nPanelX < 260 && nPanelY > 390 && nPanelY < 424)
    {
        gbQuestScreenSpeechButtonDown = 1;
    }
    else
    {
        goto CONSUME;
    }
    SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);

CONSUME:
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FB05E40
//
//  Releasing a mouse button on the quest screen. Which arm runs is decided by what the PRESS armed, not by where the cursor is:
//  gbQuestScreenCloseButtonDown first, then the quest icons if gnQuestScreenPressedQuest is set, then the speech button. The hit tests are the
//  press handler's, repeated.
//
//  BUG: the high half of the string id is never written. The speech arm computes the entry's stride in ecx (309 * iEntry, 618 / 2) and then
//  `mov cx, gQuestLogEntries.wTailStringId[ecx*2]` (0x6FB05FB7) replaces only the LOW WORD. ecx is the first __fastcall argument of
//  TEXT_ShowQuestScrollingText and its prototype is `int nTblStringId`, so the string id arrives with `(309 * iEntry) >> 16` in its upper half.
//  LATENT: `gnQuestLogEntryCount > 6` halts the game earlier in this same function, so iEntry is at most 5, 309 * 5 is 1545 and the upper half is
//  zero; it would take 213 quest entries. Reproduced through the same partial store.
//
//  3724 and 3725 are the two "no speech" ids (`cmp cx, 0E8Ch` and `cmp cx, 0E8Dh`, compared as WORDS, so those two tests are unaffected by
//  the contamination above).
//
//  The reset tail runs on every path: nine globals cleared, including the two cinematic-cof flags and the two zoo flags, whichever arm ran and
//  whether or not anything was hit. The message is then consumed only if the cursor is inside the panel, using the same 47/49 resolution seam
//  as the press handler.
void __stdcall QUESTLOG_QuestScreenOnMouseUp(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nPanelX;
    int32_t nPanelY;
    int32_t iButton;
    int32_t iEntry;
    int32_t nHitX;
    int32_t nTailStringId;
    int32_t bInsidePanel;

    nPanelX = pMsg->nXpos - gnBorderXDifference;
    nPanelY = pMsg->nYpos + gnBorderYDifference;

    if (gbQuestScreenCloseButtonDown)
    {
        if (nPanelX >= 278 && nPanelX <= 313 && nPanelY >= 392 && nPanelY <= 425)
        {
            QUESTLOG_TogglePanel(0);
        }
    }
    else if (!gbQuestScreenSpeechPlaying)
    {
        if (gnQuestScreenPressedQuest == -1)
        {
            if (gbQuestScreenSpeechButtonDown == 1 && nPanelX > 229 && nPanelX < 260 && nPanelY > 390 && nPanelY < 424 && gnQuestLogSelectedQuest != -1 && (int32_t)gnQuestLogEntryCount > 0)
            {
                for (iEntry = 0; ; ++iEntry)
                {
                    if (iEntry >= (int32_t)gnQuestLogEntryCount)
                    {
                        goto RESET;
                    }
                    if (gQuestLogEntries[iEntry].nSlotInPage == gnQuestLogSelectedQuest)
                    {
                        break;
                    }
                }

                //  iEntry cannot be -1
                if (iEntry != -1)
                {
                //  the partial store the original makes (the low word of ecx only)
                    nTailStringId = 309 * iEntry;
                    *(uint16_t *)&nTailStringId = gQuestLogEntries[iEntry].wTailStringId;
                    if ((uint16_t)nTailStringId != 3724 && (uint16_t)nTailStringId != 3725)
                    {
                        TEXT_ShowQuestScrollingText(nTailStringId, 1, 0);
                        TEXT_SetCallback((void *)QUESTLOG_OnSpeechFinished, 0);
                        gbQuestScreenSpeechPlaying = 1;
                    }
                }
            }
        }
        else
        {
            if ((uint32_t)gnQuestLogEntryCount > 6)   // `jbe`, unsigned (0x6FB05EC8)
            {
                FOG_DisplayAssert((uint32_t)szQuestLogButtonAssert, (uint32_t)szQuestLogCpp, 1848);
                exit(-1);
            }

            iButton = -1;
            if ((int32_t)gnQuestLogEntryCount > 0)
            {
                for (iButton = 0; ; ++iButton)
                {
                    if (iButton >= (int32_t)gnQuestLogEntryCount)
                    {
                        iButton = -1;
                        break;
                    }
                    nHitX = sgQuestLogButtonPositions[iButton].nHitX;
                    if (nPanelX >= nHitX && nPanelX < nHitX + 56 && nPanelY >= sgQuestLogButtonPositions[iButton].nHitY && nPanelY < sgQuestLogButtonPositions[iButton].nHitY + 52)
                    {
                        break;
                    }
                }
            }

            if (iButton == (int32_t)gnQuestScreenPressedQuest)
            {
                gnQuestLogLastClickedPerAct[gnQuestLogActTab] = iButton;
                gnQuestLogSelectedQuest = iButton;
            }
        }
    }

RESET:
    gbQuestScreenIconDown = 0;
    gbQuestScreenCloseButtonDown = 0;
    gbQuestLogCloseBoxDown = 0;
    gnQuestScreenPressedQuest = -1;
    gbQuestScreenSpeechButtonDown = 0;
    gbDurielCofLoaded = 0;
    gbAncientStatueCofLoaded = 0;
    gbZooArmed = 0;
    gbZooChickensSpawned = 0;

    if (pMsg->nXpos <= gnScreenWidth / 2)
    {
        if (D2GFX_GetResolutionMode() == D2GAMERES_800x600)
        {
            bInsidePanel = pMsg->nYpos <= gnScreenHeight - 47;
        }
        else
        {
            bInsidePanel = pMsg->nYpos <= gnScreenHeight - 49;
        }

        if (bInsidePanel)
        {
            GAME_ResetMouseButtonState();
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
            STORM_SMsgBreakHandlerChain(pMsg);
        }
    }
}


//1.10f: D2Client.0x6FB06D20
//
//  State 1 goes back to 0, and every other value is left alone. One caller, UI_UnregisterPanelHandlers' UI_QUESTLOG arm, which reaches it by
//  tail jump at 0x6FB22FA7.
//  State 1 is the log ARMED but not open: QUESTLOG_QuestOnQuestStatePacket sets it when a quest state arrives and
//  UI_SetUIToggle(UI_QUESTLOG, UI_TURNON) is refused. State 2 is the panel actually up, and the test here is what leaves 2 standing when the
//  handler set is torn down.
void __cdecl QUESTLOG_ClearPanelState(void)
{
    if (gnQuestLogPanelState == 1)
    {
        gnQuestLogPanelState = 0;
    }
}


//1.10f: D2Client.0x6FB06D40
//
//  Zeroes both 41-byte arrays (gQuestLogEntryFlags and gQuestLogEntryColours) with one index, do-while, `cmp eax, 29h` at 0x6FB06D51. 41 is
//  the quest count the whole file works in: 41 rows in gQuestLogQuestDefs, 41 status bytes in packet 0x52, 41 bytes in each of these two
//  arrays. Sole caller UI_ResetUiSubsystems; the original inlines the same loop in QUESTLOG_QuestResetAllState.
//  void: eax at the `retn` is the loop counter left by `inc eax` at 0x6FB06D50, not a value the function chose.
void __cdecl QUESTLOG_ClearEntryState(void)
{
    int32_t iQuest;

    iQuest = 0;
    do
    {
        gQuestLogEntryFlags[iQuest] = 0;
        gQuestLogEntryColours[iQuest] = 0;
        ++iQuest;
    }
    while (iQuest < 41);
}


//1.10f: D2Client.0x6FB06D60
//
//  Five stores of -1, one per act, written out rather than looped: `or eax, 0FFFFFFFFh` once and five `mov`s at 0x6FB06D63, 0x6FB06D68,
//  0x6FB06D6D, 0x6FB06D72 and 0x6FB06D77. QUESTLOG_SetActTab clears a six-element pair of arrays with a loop, so the original wrote five
//  statements here and a loop there.
//  -1 rather than 0 because 0 is a valid quest index -- the same convention gnQuestLogLastClickedPerAct and gnQuestScreenPressedQuest use.
//  Sole caller UI_InitUiForPlayerUnit.
void __cdecl QUESTLOG_ClearActSelection(void)
{
    gnQuestLogSelectedPerAct[0] = -1;
    gnQuestLogSelectedPerAct[1] = -1;
    gnQuestLogSelectedPerAct[2] = -1;
    gnQuestLogSelectedPerAct[3] = -1;
    gnQuestLogSelectedPerAct[4] = -1;
}


//1.10f: D2Client.0x6FB060A0
//
//  The quest description's speech has finished. No code caller, and three parameters of which the body reads one.
//  QUESTLOG_QuestScreenOnMouseUp installs it with `TEXT_SetCallback(this, 0)` when a quest icon is released and the description starts
//  speaking; the popup layer fires it and it clears the flag and unhooks itself by storing a NULL callback.
//  The three parameters come from the dispatcher, not from the body (`mov edx, ecx` at 0x6FB060A0 overwrites edx before anything can read it,
//  and the stack argument is never touched). text.cpp calls the slot as
//
//      (*gpfnPopupCallback)(*gpPopupCallbackArg, 1, bDismissedByMouse)
//      (*gpfnPopupCallback)(*gpPopupCallbackArg, 0, 0)
//
//  so ecx is the context, edx says whether the popup was dismissed, and the one stack argument says whether the mouse did it (`retn 4`).
//  The `return 0` is real (`xor eax, eax` at 0x6FB060B0), and text.cpp's two call sites read it: zero means the callback has NOT taken the popup
//  over, so the caller carries on with its own teardown.
int32_t __fastcall QUESTLOG_OnSpeechFinished(uint32_t nContext, uint32_t nDismissed, uint32_t bDismissedByMouse)
{
    gbQuestScreenSpeechPlaying = 0;
    TEXT_SetCallback(NULL, (int32_t)nContext);
    return 0;
}


//1.10f: D2Client.0x6FB05D80
//
//  Sets the act tab, clamped down to the furthest act the character has reached. Two callers: QUESTLOG_QuestScreenOnMouseDown (0x6FB05BB8) and
//  QUESTLOG_TogglePanel (0x6FB061C5).
//
//  The clamp is a fall-through ladder: `lea eax, [esi-1]` / `cmp eax, 3` / `ja default` at 0x6FB05D8A is a jump table over tabs 1..4, and
//  every arm falls into the one below it:
//
//      tab 4   needs quest 26      else drop to 3 and re-test
//      tab 3   needs quest 23      else drop to 2
//      tab 2   needs quest 15      else drop to 1
//      tab 1   needs quest 7       else drop to 0
//      tab 0   tested against nothing
//
//  so one call can walk a tab-4 request all the way down to Act I. The fourth argument of each rung is 0 = QFLAG_REWARDGRANTED, so the ladder
//  asks whether the act's last quest has PAID OUT, not whether it is finished. Three of the four ids are rows gQuestLogQuestDefs itself marks
//  empty -- 7, 15 and 23, no icon and no text block, the act terminators for acts 1, 2 and 3. 26 is a real row on act 4's page; act 4's own
//  terminator is quest 28 and the ladder does not use it.
//
//  BUG: an act out of range is not clamped twice. `cmp esi, 5` / `jl` is SIGNED, so only `nAct >= 5` becomes 0; a NEGATIVE act survives the
//  test, misses the jump table (`lea eax, [esi-1]` under an UNSIGNED `ja`) and is stored as it arrived. Both callers pass 0..4.
//
//  The icon state is cleared through one cursor, from gnQuestLogIconLastTick up to gnQuestLogActTab's address (0x6FB05E1B), which is
//  `&gnQuestLogIconLastTick[6]`; 0x18 is both the array's length in bytes and the distance back to gnQuestLogIconFrame. Written as the counted
//  loop over six that computes it: the six is the six quest slots of an act page.
//  So an icon does not carry its animation frame from one act's page to the next, and the selection is dropped twice over --
//  gnQuestLogSelectedQuest before the loop and gnQuestScreenPressedQuest after it, both from the `or ecx, 0FFFFFFFFh` at 0x6FB05DF6.
void __fastcall QUESTLOG_SetActTab(int32_t nAct)
{
    int32_t nTab;
    int32_t iSlot;
    D2BitBufferStrc *pQuestRecord;

    nTab = nAct;
    if (nTab >= 5)
    {
        nTab = 0;
    }

    switch (nTab)
    {
    case 4:
        pQuestRecord = NPCMENU_QuestGetQuestRecord();
        if (!D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, 26, 0))
        {
            nTab = 3;
        }
        //  falls through -- the jump table's case 3 is this instruction
    case 3:
        pQuestRecord = NPCMENU_QuestGetQuestRecord();
        if (!D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, 23, 0))
        {
            nTab = 2;
        }
        //  falls through
    case 2:
        pQuestRecord = NPCMENU_QuestGetQuestRecord();
        if (!D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, 15, 0))
        {
            nTab = 1;
        }
        //  falls through
    case 1:
        pQuestRecord = NPCMENU_QuestGetQuestRecord();
        if (!D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, 7, 0))
        {
            nTab = 0;
        }
        break;
    default:
        break;
    }

    gnQuestLogActTab = nTab;
    gnQuestLogSelectedQuest = -1;

    for (iSlot = 0; iSlot < 6; ++iSlot)
    {
        gnQuestLogIconFrame[iSlot] = 0;
        gnQuestLogIconLastTick[iSlot] = 0;
    }

    gnQuestScreenPressedQuest = -1;
}


//1.10f: D2Client.0x6FB062B0
//
//  Closing the quest log is not only a teardown: it tells the server which quests the player has now read. Sole caller
//  UI_UnregisterPanelHandlers, so it runs when the whole handler set goes down rather than when Escape is pressed.
//  Every visible entry whose `nState` is zero gets `QUESTRECORD_SetQuestState(record, nQuestId, 12)` -- 12 is QFLAG_UPDATEQUESTLOG -- and client
//  command 0x58 carrying the quest id. QUESTLOG_QuestScreenOnMouseDown does the same thing before it changes act.
//  The field the test reads is nState, at +614: `mov eax, [esi+265h]` at 0x6FB062CC, where esi is `&gQuestLogEntries[0].nQuestStateFlag`
//  (struct offset 1). The cursor steps `add esi, 26Ah` = 618 = sizeof(D2QuestLogEntryStrc).
//  `gnQuestLogActTab >= 5` skips the acknowledgement entirely and still clears the flags and unloads the images. Five is the act count: a
//  classic game cannot reach tab 5 through QUESTLOG_SetActTab, which clamps at 4.
//  `mov cl, 58h` at 0x6FB062E9 writes only CL before `call MSG_SendCmdWord`, whose first parameter is declared `int`. It is inert: the callee
//  (0x6FAAD990) builds a three-byte command out of `(char)ecx` and the 16-bit value and sends those three bytes. Ours passes a whole `88` and
//  the packet is the same.
void __cdecl QUESTLOG_AcknowledgeAndClose(void)
{
    int32_t iEntry;
    D2BitBufferStrc *pQuestRecord;

    if (gnQuestLogActTab < 5)
    {
        for (iEntry = 0; iEntry < (int32_t)gnQuestLogEntryCount; ++iEntry)
        {
            if (!gQuestLogEntries[iEntry].nState)
            {
                pQuestRecord = NPCMENU_QuestGetQuestRecord();
                D2COMMON_QUESTRECORD_SetQuestState(pQuestRecord, gQuestLogEntries[iEntry].nQuestStateFlag, 12);
                MSG_SendCmdWord(88, (int16_t)gQuestLogEntries[iEntry].nQuestStateFlag);
            }
        }
    }

    gbQuestLogCloseBoxDown = 0;
    gnQuestLogPanelState = 0;
    gbQuestScreenSpeechPlaying = 0;
    QUESTLOG_QuestUnloadPanelImages();
}


//  QUESTLOG_TogglePanel calls `UI_SetUIToggle(15, ...)` (`mov ecx, 0Fh` at 0x6FB06167 and 0x6FB06201), and 15 is UI_QUESTSCREEN, not
//  UI_QUESTLOG. So the key binding and both mouse handlers put up the quest SCREEN; UI_QUESTLOG (17) is the frame whose close box
//  QUESTLOG_OnMouseUp tests, and QUESTLOG_QuestOnQuestStatePacket is what turns THAT one on.

//  The copy length QUESTLOG_ApplyQuestListPacket leans on: 42 is one header byte plus 41 status bytes, and 41 is the length of
//  gQuestLogEntryFlags, which is why the payload of the copy lands exactly on that array.
static_assert(sizeof(D2GSPacketSrv52) == 42,
              "the 0x52 copy is a header byte and 41 status bytes, and the "
              "41 are gQuestLogEntryFlags itself");

static const char aDataGlobal[] = "DATA\\GLOBAL";


//1.10f: D2Client.0x6FB058D0
//
//  Loads the panel's six images, each guarded by its own handle, so a second call costs six loads of a global and nothing else. The whole body
//  is inside `if (!VIDEO_IsCinematicPlaying())`. In the order the bytes load them: questbackground, the tabs, questsockets, questdone, invps,
//  questlast.
//  The tabs fork on expansion and the test needs BOTH halves -- `FOG_IsExpansion() && GAME_IsExpansionGame()` at 0x6FB0591C and 0x6FB05925 --
//  so a classic game running on an expansion install still gets `questtabs`. The two arms share the `sprintf` and the `ARCHIVE_CellFileLoad`
//  that follow (0x6FB0593D jumps into the other arm's `push` sequence at 0x6FB0594E).
//  The pair of QUESTLOG_QuestUnloadPanelImages, which frees the same six and the 27 icons besides.
//  void: `al` at the `retn` is VIDEO_IsCinematicPlaying's on the refusal path and the last ARCHIVE_CellFileLoad's handle otherwise; nothing
//  reads it.
void __cdecl QUESTLOG_LoadPanelImages(void)
{
    char szBuffer[260];

    if (VIDEO_IsCinematicPlaying())
    {
        return;
    }

    if (!gQuestLogBackgroundImage)
    {
        sprintf(szBuffer, "%s\\ui\\menu\\questbackground", aDataGlobal);
        gQuestLogBackgroundImage = ARCHIVE_CellFileLoad(szBuffer, 0);
    }
    if (!gQuestLogTabsImage)
    {
        if (FOG_IsExpansion() && GAME_IsExpansionGame())
        {
            sprintf(szBuffer, "%s\\ui\\menu\\expquesttabs", aDataGlobal);
        }
        else
        {
            sprintf(szBuffer, "%s\\ui\\menu\\questtabs", aDataGlobal);
        }
        gQuestLogTabsImage = ARCHIVE_CellFileLoad(szBuffer, 0);
    }
    if (!gQuestLogSocketsImage)
    {
        sprintf(szBuffer, "%s\\ui\\menu\\questsockets", aDataGlobal);
        gQuestLogSocketsImage = ARCHIVE_CellFileLoad(szBuffer, 0);
    }
    if (!gQuestLogDoneImage)
    {
        sprintf(szBuffer, "%s\\ui\\menu\\questdone", aDataGlobal);
        gQuestLogDoneImage = ARCHIVE_CellFileLoad(szBuffer, 0);
    }
    if (!gQuestLogInvPsImage)
    {
        sprintf(szBuffer, "%s\\ui\\menu\\invps", aDataGlobal);
        gQuestLogInvPsImage = ARCHIVE_CellFileLoad(szBuffer, 0);
    }
    if (!gQuestLogLastImage)
    {
        sprintf(szBuffer, "%s\\ui\\menu\\questlast", aDataGlobal);
        gQuestLogLastImage = ARCHIVE_CellFileLoad(szBuffer, 0);
    }
}


//1.10f: D2Client.0x6FB06120
//
//  Opening and closing the panel. Everything turns on gnQuestLogPanelState: 0 closed, 1 armed, 2 open.
//  The argument goes straight through to UI_SetUIToggle as `bCheckMousePos`. The key binding (CMDTBL_ActionQuestLog, 0x6FAD37A7) passes 1 and
//  the three mouse paths (QUESTLOG_QuestScreenOnMouseUp 0x6FB05E9E, QUESTLOG_OnMouseUp 0x6FB065A6, MINIPANEL_RunButtonAction 0x6FB09998) pass 0,
//  so opening the log from the keyboard asks the UI layer to look at where the cursor is and opening it by clicking does not.
//
//  The three arms are not three paths. The compiled switch has cases 0, 1 and 2 and the close block AFTER it; what the bytes do:
//
//      state 0   the 250 ms debounce and the dead-player test, then the shared OPEN path at 0x6FB06157
//      state 1   state := 0, then `jmp 0x6FB06157` -- straight into the same OPEN path, SKIPPING both tests
//      state 2   state := 0, the close box and the speech flag cleared, then falls into the CLOSE path
//      anything else   falls into the CLOSE path with nothing cleared
//
//  It is written as `if / else if / else` with the close block inside the else.
//  BUG: the last arm is real: `cmp eax, 2 / jnz 0x6FB061F7` at 0x6FB061E0 jumps INTO the close block, not past it. So a state of 3 would
//  unload the images and toggle the window off while leaving the state at 3. UNREACHABLE: nothing writes a third value, the four writers store
//  0, 1 or 2.
//
//  The 250 ms debounce: `GetTickCount() - gdwQuestLogCloseTick < 0FAh`, UNSIGNED (`jb` at 0x6FB06144), so the wrap at 49.7 days behaves as
//  the subtraction does. gdwQuestLogCloseTick is stamped by the close path below and by QUESTLOG_QuestScreenOnDismissKey.
//  The panel opens on the act the player is standing in: `UNITS_GetRoom(player)` -> `DUNGEON_GetLevelIdFromRoom(room)` ->
//  `DRLG_GetActNoFromLevelId(level)`, each NULL-tested before the next, and any failure falls to act 0 (`xor ecx, ecx` at 0x6FB061C3). The act
//  is a BYTE (`mov cl, al` / `and ecx, 0FFh` at 0x6FB061AC).
//  Client command 0x40 at the end tells the server the log is open; `mov cl, 40h` is the ABI for MSG_SendCmd1's `char` parameter.
void __fastcall QUESTLOG_TogglePanel(int32_t bCheckMousePos)
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    int32_t nLevelId;
    int32_t nAct;
    //  player -> room -> level -> act, act 0 on any failure

    if (gnQuestLogPanelState == 0)
    {
        if (GetTickCount() - gdwQuestLogCloseTick < 250)
        {
            return;
        }
        if (CUNIT_IsPlayerDeadOrMissing())
        {
            return;
        }
    }
    else if (gnQuestLogPanelState == 1)
    {
        //  armed, not open: neither the debounce nor the dead test is run
        gnQuestLogPanelState = 0;
    }
    else
    {
        if (gnQuestLogPanelState == 2)
        {
            gnQuestLogPanelState = 0;
            gbQuestLogCloseBoxDown = 0;
            gbQuestScreenSpeechPlaying = 0;
        }

        //  and anything that is not 0, 1 or 2 arrives here too
        QUESTLOG_QuestUnloadPanelImages();
        UI_SetUIToggle(UI_QUESTSCREEN, UI_TURNOFF, bCheckMousePos);
        gdwQuestLogCloseTick = GetTickCount();
        j_text_1AE00();
        return;
    }

    if (VIDEO_IsCinematicPlaying())
    {
        return;
    }
    if (UI_SetUIToggle(UI_QUESTSCREEN, UI_TURNON, bCheckMousePos) != 1)
    {
        return;
    }

    QUESTLOG_LoadPanelImages();
    gnQuestLogPanelState = 2;

    pPlayer = CUNIT_GetPlayer();
    if (pPlayer && (pRoom = D2COMMON_UNITS_GetRoom(pPlayer)) != NULL && (nLevelId = D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom)) != LEVEL_NONE)
    {
        nAct = D2COMMON_DRLG_GetActNoFromLevelId((D2C_Levels)nLevelId);
    }
    else
    {
        nAct = 0;
    }

    QUESTLOG_SetActTab(nAct);
    QUESTLOG_SelectActPage(gnQuestLogActTab, 0);
    MSG_SendCmd1(64);
}


//1.10f: D2Client.0x6FB06230
//
//  Server packet 0x52, the quest list. It has no caller of its own: its only reference is the five-byte `jmp` at 0x6FAB2400 that is
//  SCMD_HandleSrv52_QuestList, entry 82 of the packet table.
//
//  The copy lands on two globals at once: `mov ecx, 0Ah` / `rep movsd` / `movsw` at 0x6FB0623E writes 42 bytes from the packet's first byte to
//  0x6FBB6015 -- the 0x52 header byte into the one-byte gQuestListPacketCopy and the 41 status bytes into gQuestLogEntryFlags itself.
//  0x6FBB6015 + 42 = 0x6FBB603F, the byte after the flags array, so the copy stops exactly at the end of it; there is no second copy of the
//  quest list anywhere in the module. That is also what the `gQuestLogEntryFlags[-1]` store of QUESTLOG_QuestOnQuestStatePacket would
//  corrupt (LATENT): not "the byte before the array" but this header byte.
//  DIFFERENCE: the two are objects of ours (QuestLog.h) and a 42-byte copy into the one-byte object would write past it, so they are written
//  by name -- the header byte, then the 41 states -- the same 42 bytes from the same packet, in the same order.
//
//  Then it redraws, but only if the panel is up: the close box is disarmed unconditionally; the rest is skipped while a cinematic plays.
//  `gnQuestLogPanelState == 2` -- open -- reloads the images, and the act page is re-selected either way, with `bClearSelections` 1. The
//  re-select is a tail jump at 0x6FB0626D, so this function is void like QUESTLOG_SelectActPage.
void __fastcall QUESTLOG_ApplyQuestListPacket(D2GSPacketSrv52 *pPacket)
{
    static_assert(sizeof(D2GSPacketSrv52::pQuestList) == sizeof(gQuestLogEntryFlags), "packet 0x52's 41 states are gQuestLogEntryFlags' 41 bytes");
    gQuestListPacketCopy = pPacket->nHeader;
    memcpy(gQuestLogEntryFlags, pPacket->pQuestList, sizeof(gQuestLogEntryFlags));

    gbQuestLogCloseBoxDown = 0;

    if (VIDEO_IsCinematicPlaying())
    {
        return;
    }

    if (gnQuestLogPanelState == 2)
    {
        QUESTLOG_LoadPanelImages();
    }
    QUESTLOG_SelectActPage(gnQuestLogActTab, 1);
}


//  The two enum values QUESTLOG_DrawFrameAndTitle's bytes lean on WITHOUT spelling them: the compiler kept `esi` at 2 and used it as three
//  different constants in nine instructions -- the UI shift code 2, the resolution mode in `cmp eax, esi` (0x6FB0669C, 0x6FB066B3) and the
//  window id in `mov ecx, esi` (0x6FB066B7).
static_assert(D2GAMERES_800x600 == 2 && UI_STATSCREEN == 2,
              "one register held both, and the disassembly reads as `cmp "
              "eax, esi` / `mov ecx, esi`");
static_assert(sizeof(D2GfxDataStrc) == 72,
              "`mov ecx, 12h` / `rep stosd` at 0x6FB06656 -- 18 dwords is "
              "the whole cel context");


//1.10f: D2Client.0x6FB056D0
//
//  Prepares one act's page: loads its icons, then decides which quest is highlighted. Four callers: QUESTLOG_Draw,
//  QUESTLOG_QuestScreenOnMouseDown, QUESTLOG_TogglePanel and QUESTLOG_ApplyQuestListPacket (the last by tail jump).
//
//  A classic client asked for act 4 leaves before loading anything: `(!FOG_IsExpansion() || !GAME_IsExpansionGame()) && nAct == 4` returns at
//  once -- act 4 zero-based is Act V, the expansion's. It returns before the five per-act selections would be cleared, so a `bClearSelections`
//  passed with act 4 in a classic game does nothing.
//
//  The icons are lazy and per act: `gQuestLogActIconRange[nAct]` is a first/last pair (the bytes index it with `edi*8`) into
//  gQuestLogIconNames, and each missing image is loaded from `DATA\GLOBAL\ui\menu\<name>`. The upper bound is re-read from the table on every
//  iteration (0x6FB05767), not hoisted.
//
//  The scan: with no selection remembered for this act, all 41 quest definitions are walked. A record qualifies when its act matches, it has an
//  icon, its icon index is below 27 (`cmp al, 1Bh` UNSIGNED -- 27 is the length of gQuestLogIconNames and gQuestLogIconImages) and that image is
//  loaded. Each one that qualifies gets a full QUESTLOG_BuildQuestDisplayInfo run into a 618-byte stack record, and then:
//
//      nFlag0x263 set, or nState == 0   take this quest and STOP
//      otherwise                        keep looking
//      nState 1 or 2 or 3               count it as "seen something"
//      nState 3, first one only         remember it as the fallback
//
//  The cursor is the break: `mov ebp, 29h` at 0x6FB05812 sets the index to 41 and the loop's own `inc` then carries it past the bound.
//  Written that way.
//  The ladder re-tests what it has already excluded: reaching the fallback arm means nState is 1 or 3, and the bytes still compare against 2
//  (0x6FB0583A) and 1 (0x6FB0583F). Both are written out: the first can never be taken, the second is what keeps state 1 out of the fallback.
//  Last, `gnQuestLogLastClickedPerAct[nAct]` beats the fallback: opening the log twice returns to the quest last clicked rather than to the
//  first completed one on the page.
//  The 300-word construction before each Build call is the Unicode default-constructor closure once per element (0x6FB057D5), which zeroes
//  `pInfo.wszText`. It is written as the 300 stores it is: declaring `Unicode wszText[300]` in C++ would call that constructor.
void __fastcall QUESTLOG_SelectActPage(int32_t nAct, int32_t bClearSelections)
{
    int32_t iIcon;
    int32_t iQuestDef;
    int32_t i;
    int32_t nCompletedSeen;
    int32_t nFirstCompleted;
    int32_t nSelected;
    int32_t nQuestToSelect;
    D2QuestLogQuestDefStrc *pDef;
    D2QuestLogEntryStrc pInfo;
    char szBuffer[260];

    nCompletedSeen = 0;
    nFirstCompleted = -1;
    gnQuestLogSelectedQuest = -1;

    if ((!FOG_IsExpansion() || !GAME_IsExpansionGame()) && nAct == 4)
    {
        return;
    }

    for (iIcon = gQuestLogActIconRange[nAct][0]; iIcon <= gQuestLogActIconRange[nAct][1]; ++iIcon)
    {
        if (!gQuestLogIconImages[iIcon])
        {
            sprintf(szBuffer, "%s\\ui\\menu\\%s", aDataGlobal, gQuestLogIconNames[iIcon]);
            gQuestLogIconImages[iIcon] = ARCHIVE_CellFileLoad(szBuffer, 0);
        }
    }

    if (gnQuestLogSelectedPerAct[nAct] != -1)
    {
        gnQuestLogSelectedQuest = gnQuestLogSelectedPerAct[nAct];
    }
    else
    {
        nSelected = gnQuestLogSelectedQuest;

        for (iQuestDef = 0; iQuestDef < 41; ++iQuestDef)
        {
            pDef = &gQuestLogQuestDefs[iQuestDef];
            if (pDef->nAct != nAct || !pDef->bHasIcon || pDef->nIconIndex >= 27 || !gQuestLogIconImages[pDef->nIconIndex])
            {
                continue;
            }

            //  the constructor closure, 300 times -- it zeroes the 300
            //  words QUESTLOG_BuildQuestDisplayInfo is about to fill
            for (i = 0; i < 300; ++i)
            {
                pInfo.wszText[i] = 0;
            }

            //  `mov ecx, [ebx+0Bh]` at 0x6FB057E3 with ebx = &pDef->nIconIndex (struct + 1), so the read is struct + 12: the QUESTSTATEFLAG
            //  index, NOT the D2Quests id at struct + 8. QUESTLOG_BuildQuestDisplayInfo scans that same column for it.
            pInfo.nQuestStateFlag = pDef->nQuestStateFlag;
            QUESTLOG_BuildQuestDisplayInfo(&pInfo);

            if (pInfo.nFlag0x263 || !pInfo.nState)
            {
                //  `mov ebp, 29h` -- the cursor is the break, and the state ladder below still runs for this record
                iQuestDef = 41;
                nSelected = pInfo.nSlotInPage;
                gnQuestLogSelectedQuest = pInfo.nSlotInPage;
            }
            else
            {
                nSelected = gnQuestLogSelectedQuest;
            }

            if (pInfo.nState == 1 || pInfo.nState == 3)
            {
                //  only 1 and 3 arrive here, and the bytes compare against 2 anyway (0x6FB0583A) -- that test can never be taken
                if (pInfo.nState != 2 && pInfo.nState != 1 && nFirstCompleted == -1)
                {
                    nFirstCompleted = pInfo.nSlotInPage;
                }
                ++nCompletedSeen;
            }
            else if (pInfo.nState == 2)
            {
                ++nCompletedSeen;
            }
        }

        if (nCompletedSeen && nSelected == -1)
        {
            nQuestToSelect = gnQuestLogLastClickedPerAct[nAct];
            if (nQuestToSelect == -1)
            {
                nQuestToSelect = nFirstCompleted;
            }
            if (nQuestToSelect != -1)
            {
                gnQuestLogSelectedQuest = nQuestToSelect;
            }
        }
    }

    if (bClearSelections)
    {
        gnQuestLogSelectedPerAct[0] = -1;
        gnQuestLogSelectedPerAct[1] = -1;
        gnQuestLogSelectedPerAct[2] = -1;
        gnQuestLogSelectedPerAct[3] = -1;
        gnQuestLogSelectedPerAct[4] = -1;
    }
}


//1.10f: D2Client.0x6FB065F0
//
//  Draws the quest log's close box and the panel title (tbl 3928, `CfgQuestLog`), and is also where the panel notices the player has died.
//
//  Three refusals, and the first one closes the panel:
//
//      the player is dead or missing   UI_SetUIToggle(UI_QUESTLOG, TURNOFF, 0),
//                                      and state 1 is dropped to 0
//      SCROLL_GetUiShiftScreen() == 3  both panels open -- draw nothing
//      UI_QUESTSCREEN is up            the screen draws its own frame
//
//  The first is the only writer of the panel state in this function, and it clears the close box with it.
//
//  The box is the level-up button's cel pair: `gpImgLevelUpSocket` (`Panel\Levelsocket`) and `gpImgLevelUpButton` (`Panel\Level`), both loaded
//  by NEWSTATS_LoadImages and shared by CHAR_CharPanelDraw, the four PANEL_DrawNew*Button functions and HELP_DrawHelpHint. The quest log borrows
//  them: the socket at the rect's corner, the button three right and four up from it, frame 1 while the cursor is inside the box and the press
//  is armed.
//  The heading is centred on the SOCKET's width, not on the panel's: `CelGetWidth(CelGetHandle(&tCelContext))` runs while the context still
//  holds the socket, and the title's X is `width / 2 - textWidth / 2 + rect.nLeft + 1`. The `+ 1` is in the bytes (`lea edi, [eax+edx+1]`,
//  0x6FB06721).
//
//  The table is 640x480-relative and this is where it is rebased: gQuestLogCloseRects is computed once, at load, when gnScreenWidth and
//  gnScreenHeight hold their 640 and 480, and nothing recomputes it. Every read here adds `gnScreenHeight - 480` to a Y and
//  `(gnScreenWidth - 640) / 2` to an X when the UI is shifted.
//  `SCROLL_GetUiShiftScreen()` is called FOUR times in this body (0x6FB06631, 0x6FB06732, 0x6FB06786 and 0x6FB067E1) and re-tested each time;
//  the original does not cache it.
//  The panel origin is computed TWICE inside the hit test, once for the left edge (`add esi, eax` at 0x6FB0683B) and once for the right
//  (0x6FB068A8), from two separate `(gnScreenWidth - 640) / 2` sequences at 0x6FB0684F and 0x6FB06885. The two are equal on every path, as in
//  QUESTLOG_OnMouseDown, and are written out for the same reason.
void __cdecl QUESTLOG_DrawFrameAndTitle(void)
{
    int32_t nUiShiftScreen;
    D2GameResolutionMode eResolution;
    int32_t iRect;
    int32_t nPanelLeft;
    int32_t nPanelLeft2;
    int32_t nCloseLeft;
    int32_t nTitleX;
    int32_t nTitleY;
    int32_t nSocketX;
    int32_t nSocketY;
    int32_t nButtonX;
    int32_t nButtonY;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nCelWidth;
    const Unicode *wszTitle;
    D2GfxDataStrc tCelContext;

    if (CUNIT_IsPlayerDeadOrMissing())
    {
        UI_SetUIToggle(UI_QUESTLOG, UI_TURNOFF, 0);
        if (gnQuestLogPanelState == 1)
        {
            gbQuestLogCloseBoxDown = 0;
            gnQuestLogPanelState = 0;
        }
        return;
    }

    nUiShiftScreen = SCROLL_GetUiShiftScreen();
    if (nUiShiftScreen == 3)
    {
        return;
    }
    if (UI_GetUiWindowState(UI_QUESTSCREEN))
    {
        return;
    }

    memset(&tCelContext, 0, sizeof(tCelContext));
    tCelContext.nDirection = 0;
    tCelContext.pCellFile = gpImgLevelUpSocket;
    tCelContext.nFrame = 0;

    if (UI_GetUiWindowState(UI_NEWSTATS) && !UI_GetUiWindowState(UI_STATSCREEN))
    {
        eResolution = D2GFX_GetResolutionMode();
        if (nUiShiftScreen == 2)
        {
            iRect = 2 * (eResolution != D2GAMERES_800x600) + 1;
        }
        else
        {
            iRect = (eResolution != D2GAMERES_800x600) ? 2 : 0;
        }
    }
    else
    {
        eResolution = D2GFX_GetResolutionMode();
        if (nUiShiftScreen == 2)
        {
            if (eResolution == D2GAMERES_800x600 || UI_GetUiWindowState(UI_STATSCREEN))
            {
                iRect = 1;
            }
            else
            {
                iRect = 3;
            }
        }
        else
        {
            iRect = (eResolution != D2GAMERES_800x600) ? 2 : 0;
        }
    }

    nCelWidth = D2CMP_CelGetWidth(D2CMP_CelGetHandle(&tCelContext));
    wszTitle = D2LANG_GetStringFromTblIndex(STR_IDX_3928_CfgQuestLog);
    nTitleX = nCelWidth / 2 - D2WIN_10121_GetTextWidth(wszTitle) / 2 + gQuestLogCloseRects[iRect].nLeft + 1;
    nTitleY = gQuestLogCloseRects[iRect].nTitleY + gnScreenHeight - 480;
    if (SCROLL_GetUiShiftScreen() == 2)
    {
        nTitleX += (gnScreenWidth - 640) / 2;
    }

    //  the string is fetched a SECOND time for the draw
    wszTitle = D2LANG_GetStringFromTblIndex(STR_IDX_3928_CfgQuestLog);
    D2WIN_10117_DrawText(wszTitle, nTitleX, nTitleY, 0, 0);

    nSocketX = gQuestLogCloseRects[iRect].nLeft;
    nSocketY = gQuestLogCloseRects[iRect].nBottom + gnScreenHeight - 480;
    nButtonX = nSocketX;
    nButtonY = nSocketY;
    if (SCROLL_GetUiShiftScreen() == 2)
    {
        nSocketX += (gnScreenWidth - 640) / 2;
        nButtonX = nSocketX;
    }

    D2GFX_TEXTURE_CelDraw(&tCelContext, nSocketX, nSocketY, 0xFFFFFFFF, 5, NULL);
    tCelContext.pCellFile = gpImgLevelUpButton;

    if (gbQuestLogCloseBoxDown)
    {
        nMouseY = CURSOR_GetMouseY();
        nMouseX = CURSOR_GetMouseX();
        nUiShiftScreen = SCROLL_GetUiShiftScreen();

        if (UI_GetUiWindowState(UI_NEWSTATS) && !UI_GetUiWindowState(UI_STATSCREEN))
        {
            eResolution = D2GFX_GetResolutionMode();
            if (nUiShiftScreen == 2)
            {
                iRect = 2 * (eResolution != D2GAMERES_800x600) + 1;
                nPanelLeft = (gnScreenWidth - 640) / 2;
            }
            else
            {
                iRect = (eResolution != D2GAMERES_800x600) ? 2 : 0;
                nPanelLeft = 0;
            }
        }
        else
        {
            eResolution = D2GFX_GetResolutionMode();
            if (nUiShiftScreen == 2)
            {
                if (eResolution == D2GAMERES_800x600 || UI_GetUiWindowState(UI_STATSCREEN))
                {
                    iRect = 1;
                }
                else
                {
                    iRect = 3;
                }
                nPanelLeft = (gnScreenWidth - 640) / 2;
            }
            else
            {
                iRect = (eResolution != D2GAMERES_800x600) ? 2 : 0;
                nPanelLeft = 0;
            }
        }

        nCloseLeft = nPanelLeft + gQuestLogCloseRects[iRect].nLeft;
        if (nUiShiftScreen == 2)
        {
            nPanelLeft2 = (gnScreenWidth - 640) / 2;
        }
        else
        {
            nPanelLeft2 = 0;
        }

        if (nMouseX > nCloseLeft && nMouseX < nPanelLeft2 + gQuestLogCloseRects[iRect].nRight && nMouseY > gQuestLogCloseRects[iRect].nTop + gnScreenHeight - 480)
        {
            //  set BEFORE the last edge is tested (0x6FB068BC) and put back
            //  at the label below when that test fails
            tCelContext.nFrame = 1;
            if (nMouseY < gQuestLogCloseRects[iRect].nBottom + gnScreenHeight - 480)
            {
                goto DRAW_BUTTON;
            }
        }
    }

    tCelContext.nFrame = 0;

DRAW_BUTTON:
    //  one call, reached from both arms -- 0x6FB068F2
    D2GFX_TEXTURE_CelDraw(&tCelContext, nButtonX + 3, nButtonY - 4, 0xFFFFFFFF, 5, NULL);
}


//  The assert this function can reach (0x6FB7DCEC). QUESTLOG_MAX_CHARS is 300: the assert string is
//  "Unicode::strlen(pszTemplate) < QUESTLOG_MAX_CHARS - 20" and the test is `cmp eax, 118h` = 280, so the 300 every `strncpy` here carries is
//  that constant under its own name, and the 299 that follows each copy is `QUESTLOG_MAX_CHARS - 1`.
static const char szTemplateLengthAssert[] = "Unicode::strlen(pszTemplate) < QUESTLOG_MAX_CHARS - 20";


//1.10f: D2Client.0x6FB04D50
//
//  Fills one D2QuestLogEntryStrc: the title, the body text, the tail string, the icon, the slot, the state and the colour. Three callers --
//  QUESTLOG_Draw, QUESTLOG_SelectActPage and QUEST_ShouldPlayAreaEntrySpiel -- and every one of them reads the record afterwards rather than
//  the return, which is why the prototype is `void`. It is the only function that reads the quest record; everything else displays what this
//  fills in.
//
//  The entry is found by the flag index, not by the quest id. The scan at 0x6FB04D86 walks the `nQuestStateFlag` column (+12 of each 16-byte
//  record) and compares it with `pEntry->nQuestStateFlag`, so what a caller puts in that field is the QUESTSTATEFLAG index, the same value
//  QUESTRECORD_GetQuestState takes as `nQuest`, and NOT the D2Quests id at +8. Both callers agree: QUESTLOG_SelectActPage reads `[ebx+0Bh]` off
//  `&pDef->nIconIndex`, which is +12, and QUEST_ShouldPlayAreaEntrySpiel searches the +8 column for its argument and then hands THIS function
//  the +12 value it found beside it. Client command 0x58 carries the same flag index: the server's handler reads the word at +1, refuses 41 and
//  up and calls QUESTRECORD_SetQuestState with the record of the game's difficulty, that word and 12.
//  The scan's bound is `&gQuestLogQuestDefs[41].nQuestStateFlag` = 0x6FB7DBC4: 41 records of 16 bytes is 656, 0x6FB7D928 + 656 = 0x6FB7DBB8,
//  and +12 is the cursor's own field offset.
//
//  The answer goes out through the record, never the register: six of the seven exits set `pEntry->nState` and return; the seventh sets it to 2,
//  which is "nothing to show". Three of them normalise a quest bit to 0 or 1 with `neg / sbb / neg` (0x6FB05564) or with an explicit if/else
//  (0x6FB050D9) rather than storing the callee's answer, so the C writes `!= 0` at those three and the bytes are reproduced whatever
//  QUESTRECORD_GetQuestState returns.
//
//  Three of the quest-bit tests compare with 1, not with zero (every other test of the same call is `test eax, eax`): quest 14 flag 3
//  (`cmp eax, 1`, 0x6FB04EF0), quest 14 flag 4 (0x6FB04F3A) and quest 21 flag 4 (0x6FB055B1). They are written as `== 1`; if the export ever
//  returned a bit value other than 1 these would be different questions.
//
//  The text block is indexed by the state colour. Every variable-index read is `[pTextBlock + 4 * n + 2]` or `[pTextBlock + 4 * n + 4]`
//  (0x6FB0526A, 0x6FB05465, 0x6FB0552C and four more). With `tStates` at offset 6 and four bytes a pair, that is
//  `tStates[n - 1].wTextStringId` and `tStates[n - 1].wTailStringId` for n >= 1, and `wDescStringId` / `wStateIndex` for n == 0. The
//  constant-index sites prove the off-by-one from the other side: colour 7 reads `[edi+1Eh]` = tStates[6], colour 5 reads `[edi+16h]` =
//  tStates[4], colour 6 reads `[edi+18h]` = tStates[5]. The C keeps the pointer form the bytes use rather than converting to
//  `tStates[n - 1]`, because converting would need n >= 1 on every path and that is a property of the shipped data, not of the code.
//
//  The seven exits, in the order the bytes reach them:
//
//      no record, no text block, or the text block has nothing to say
//                              nState = 2
//      flag 0 and flag 13      colour 13, the quest's own description,
//                              nState = the UPDATEQUESTLOG bit
//      flag 0, not 13          colour 11, the same shape
//      the three Seven Tombs
//      special cases           colours 7, 5 and 6, nState = 3
//      a state index in the
//      text block              colour = that index + 1, nState = 3
//      COMPLETEDBEFORE         colour 10 (or 5 / 6 for quest 37, 4 / 5 for
//                              quest 34), nState = 3, or 1 for quest 27
//      nothing yet, but the
//      other record knows      the "thank you, come again" line, nState = 3
//      everything else         the colour's own line, nState = 3, and
//                              nState = the UPDATEQUESTLOG bit at colour 13
//
//  The two quest records are not the same record: `NPCMENU_QuestGetQuestRecord()` is the live one and
//  `NPCMENU_QuestGetQuestRecordSecondary()` is the other difficulty's. Only the last two arms read the secondary, and they are the ones that
//  say "you have done this before".
//
//  The title 3714 is Act I quest 1, and it counts monsters: with the Den of Evil's title (3714 `qstsa1q1`) and a state colour of 3 or 4, the
//  body is the template plus the number still alive: `sprintf(szBuffer, "%d", gnMonstersRemaining)`, `Unicode::toUnicode` into a ten-element
//  stack array, `strcpy` the template and `strcat` the number. Everything else takes a plain `strncpy`.
//  Three of those tests are already decided when they are reached. The arm is entered on `wTitleStringId == 3714` (0x6FB05253), and inside it
//  the bytes test `wTitleStringId != 3714` twice more -- at 0x6FB05292, where it can only be false, and at 0x6FB052E2, where it feeds
//  `setnz / dec / and` and can only produce gnMonstersRemaining. Written out.
//  The 299th word is zeroed on one path and not the other: the `strncpy` path stores it at 0x6FB05375; the `strcpy`/`strcat` path jumps from
//  0x6FB05359 to 0x6FB0537E and steps over that store. Reproduced.
//
//  The game type is asked in two different shapes:
//
//      6FB05443   setnz dl / add edx, 0E91h      3729 + (type != 0)
//      6FB055E3   mov ecx, 0E92h / jnz / 0E91h   3730, then 3729 if type == 0
//
//  the same choice between `qstsThankYouComeAgain` and its multiplayer twin, written twice in the original and kept twice here.
void __fastcall QUESTLOG_BuildQuestDisplayInfo(D2QuestLogEntryStrc *pEntry)
{
    int32_t nQuestStateFlag;
    int32_t iDef;
    int32_t nEntry;
    UINT_PTR nDefFlag;
    D2QuestLogTextBlockStrc *pTextBlock;
    D2BitBufferStrc *pQuestRecord;
    D2BitBufferStrc *pQuestRecord2;
    BOOL bRewardGranted;
    BOOL bPrimaryGoalDone;
    BOOL bRewardPending;
    uint8_t nStateColour;
    uint8_t nStateIndex;
    int32_t nStatusStringId;
    const Unicode *pSrc;
    const Unicode *pTemplate;

    nQuestStateFlag = pEntry->nQuestStateFlag;
    iDef = 0;
    nEntry = 0;

    pEntry->wTitleStringId = STR_IDX_3724_qstsxxx;
    pEntry->wTailStringId = STR_IDX_3724_qstsxxx;
    pEntry->wszText[0] = 0;
    pEntry->nFlag0x263 = 0;
    pEntry->bHasText = 0;

    //  The +0xC column, stepped 16 bytes at a time from 0x6FB7D934 and stopped at 0x6FB7DBC4 (`cmp eax, 6FB7DBC4h` / `jl`, 0x6FB04D93..0x6FB04D98)
    //  -- &gQuestLogQuestDefs[41].nQuestStateFlag, twelve bytes past the table. The table is an object of ours and a pointer stepped there would
    //  leave it, so the cursor is an integer, the bound the column's start plus the table's size, compared signed as the bytes compare it; it is
    //  read only while it is inside the table.
    nDefFlag = (UINT_PTR)&gQuestLogQuestDefs[0].nQuestStateFlag;
    while (*(int32_t *)nDefFlag != nQuestStateFlag)
    {
        nDefFlag += sizeof(D2QuestLogQuestDefStrc);
        ++iDef;
        if ((int32_t)nDefFlag >= (int32_t)((UINT_PTR)&gQuestLogQuestDefs[0].nQuestStateFlag + 41 * sizeof(D2QuestLogQuestDefStrc)))
        {
            nEntry = iDef;
            goto NOTHING_TO_SHOW;
        }
    }

    nEntry = iDef;
    //  iDef only ever counts up from 0, so this can never fire
    if (iDef == -1)
    {
        goto NOTHING_TO_SHOW;
    }

    pEntry->nSlotInPage = gQuestLogQuestDefs[iDef].nSlotInPage;
    pEntry->nIconIndex = gQuestLogQuestDefs[iDef].nIconIndex;

    pQuestRecord = NPCMENU_QuestGetQuestRecord();
    pQuestRecord2 = NPCMENU_QuestGetQuestRecordSecondary();

    nStateColour = gQuestLogEntryFlags[iDef];
    pEntry->nStateColour = nStateColour;

    if (!gQuestLogQuestDefs[iDef].pTextBlock)
    {
        goto NOTHING_TO_SHOW;
    }

    bRewardGranted = D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, gQuestLogQuestDefs[iDef].nQuestStateFlag, 0);
    bPrimaryGoalDone = D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, gQuestLogQuestDefs[iDef].nQuestStateFlag, 13);
    bRewardPending = D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, gQuestLogQuestDefs[iDef].nQuestStateFlag, 1);

    pTextBlock = gQuestLogQuestDefs[iDef].pTextBlock;

    if (pTextBlock == &gQuestLogTextBlocks[22] && bRewardPending)   // 0x6FB7D71C, the Siege of Harrogath text block
    {
        if (!nStateColour)
        {
            nStateColour = 4;
        }
        goto BY_COLOUR;
    }

    if (pTextBlock != &gQuestLogTextBlocks[11] || bRewardGranted)   // 0x6FB7D45C, the Seven Tombs text block
    {
        if (bPrimaryGoalDone)
        {
            if (!bRewardGranted)
            {
                goto BY_STATE_INDEX;
            }
            nStatusStringId = STR_IDX_3726_qstsComplete;
            nStateIndex = 13;
        }
        else
        {
            if (!bRewardGranted)
            {
                goto COMPLETED_BEFORE;
            }
            nStatusStringId = STR_IDX_3728_qstsprevious;
            nStateIndex = 11;
        }

        pEntry->nStateColour = nStateIndex;
        //  pTextBlock was tested above and cannot be NULL here
        if (pTextBlock)
        {
            pEntry->wTitleStringId = pTextBlock->wTitleStringId;
            Unicode::strncpy((Unicode *)pEntry->wszText, D2LANG_GetStringFromTblIndex((D2C_StringIndices)nStatusStringId), 300);
            pEntry->wszText[299] = 0;
            pEntry->wTailStringId = pTextBlock->wDescStringId;
            gQuestLogEntryColours[nEntry] = nStateIndex;
            pEntry->bHasText = 1;
            pEntry->nState = D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, gQuestLogQuestDefs[iDef].nQuestStateFlag, 12) != 0;
            return;
        }
        goto NOTHING_TO_SHOW;
    }

    //  the Seven Tombs block, with no reward granted yet
    if (!bPrimaryGoalDone)
    {
        if ((D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, 12, 0) || D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, 12, 13)) && nStateColour == 1)
        {
            pEntry->wTitleStringId = pTextBlock->wTitleStringId;
            Unicode::strncpy((Unicode *)pEntry->wszText, D2LANG_GetStringFromTblIndex((D2C_StringIndices) pTextBlock->tStates[6].wTextStringId), 300);
            pEntry->wszText[299] = 0;
            gQuestLogEntryColours[nEntry] = 7;
            nStateIndex = 7;
            pEntry->wTailStringId = pTextBlock->tStates[6].wTailStringId;
            goto SET_STATE_3;
        }

        //  `cmp eax, 1`, not a zero test
        if (D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, 14, 3) == 1)
        {
            pEntry->wTitleStringId = pTextBlock->wTitleStringId;
            Unicode::strncpy((Unicode *)pEntry->wszText, D2LANG_GetStringFromTblIndex((D2C_StringIndices) pTextBlock->tStates[4].wTextStringId), 300);
            pEntry->wszText[299] = 0;
            gQuestLogEntryColours[nEntry] = 5;
            nStateIndex = 5;
            pEntry->wTailStringId = pTextBlock->tStates[4].wTailStringId;
            goto SET_STATE_3;
        }

        if (D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, 14, 4) == 1)
        {
            pEntry->wTitleStringId = pTextBlock->wTitleStringId;
            Unicode::strncpy((Unicode *)pEntry->wszText, D2LANG_GetStringFromTblIndex((D2C_StringIndices) pTextBlock->tStates[5].wTextStringId), 300);
            pEntry->wszText[299] = 0;
            gQuestLogEntryColours[nEntry] = 6;
            nStateIndex = 6;
            pEntry->wTailStringId = pTextBlock->tStates[5].wTailStringId;
            goto SET_STATE_3;
        }

        goto COMPLETED_BEFORE;
    }

BY_STATE_INDEX:
    if (pTextBlock->wStateIndex != 0xFFFF)
    {
        if (!bRewardPending)
        {
            goto NOT_STARTED;
        }
        if (pTextBlock != &gQuestLogTextBlocks[19])   // 0x6FB7D65C, the Fallen Angel text block
        {
            nStateIndex = (uint8_t)pTextBlock->wStateIndex + 1;
            pEntry->wTitleStringId = pTextBlock->wTitleStringId;
            Unicode::strncpy((Unicode *)pEntry->wszText, D2LANG_GetStringFromTblIndex((D2C_StringIndices) (&pTextBlock->wDescStringId)[2 * nStateIndex]), 300);
            pEntry->wszText[299] = 0;
            pEntry->nFlag0x263 = nStateIndex != (uint8_t)
                                   gQuestLogEntryColours[nEntry];
            pEntry->wTailStringId = (&pTextBlock->wStateIndex)[2 * nStateIndex];
            gQuestLogEntryColours[nEntry] = nStateIndex;
            pEntry->nState = 3;
            pEntry->bHasText = 1;
            pEntry->nStateColour = nStateIndex;
            return;
        }
    }

COMPLETED_BEFORE:
    if (bRewardPending && D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, gQuestLogQuestDefs[iDef].nQuestStateFlag, 15))
    {
        nStateIndex = 10;

        if (pEntry->nQuestStateFlag == 37)
        {
            if (!D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, 37, 8) || D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, 37, 9))
            {
                //  the SAME bit asked again -- 0x6FB05143 and 0x6FB0516D
                if (!D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, 37, 8))
                {
                    nStateIndex = 5;
                }
            }
            else
            {
                nStateIndex = 6;
            }
        }

        if (pEntry->nQuestStateFlag == 34)
        {
            nStateIndex = (uint8_t)
                (D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, 38, 4) != 0) + 4;
        }

        pEntry->nStateColour = nStateIndex;

        //  pTextBlock cannot be NULL here either
        if (pTextBlock && (&pTextBlock->wDescStringId)[2 * nStateIndex] != STR_IDX_3725_qstsnull)
        {
            pEntry->wTitleStringId = pTextBlock->wTitleStringId;
            Unicode::strncpy((Unicode *)pEntry->wszText, D2LANG_GetStringFromTblIndex((D2C_StringIndices) (&pTextBlock->wDescStringId)[2 * nStateIndex]), 300);
            pEntry->wszText[299] = 0;
            pEntry->wTailStringId = (&pTextBlock->wStateIndex)[2 * nStateIndex];
            gQuestLogEntryColours[nEntry] = nStateIndex;
            if (pEntry->nQuestStateFlag == 27)
            {
                pEntry->nState = 1;
            }
            else
            {
                pEntry->nState = 3;
            }
            pEntry->bHasText = 1;
            return;
        }

        goto NOTHING_TO_SHOW;
    }

NOT_STARTED:
    if (!nStateColour)
    {
        if (D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord2, gQuestLogQuestDefs[iDef].nQuestStateFlag, 13))
        {
            //  the third `cmp eax, 1` -- 0x6FB055B1
            if (pEntry->nQuestStateFlag == 21 && !D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, 21, 0) && D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, 21, 4) == 1)
            {
                nStatusStringId = STR_IDX_989_qstsa3q53;
            }
            else
            {
                if (bPrimaryGoalDone || D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, gQuestLogQuestDefs[iDef].nQuestStateFlag, 1))
                {
                    goto NOTHING_TO_SHOW;
                }
                nStatusStringId = STR_IDX_3730_qstsThankYouComeAgainMulti;
                if (gnGameType == GAMETYPE_SINGLEPLAYER)
                {
                    nStatusStringId = STR_IDX_3729_qstsThankYouComeAgain;
                }
            }
        }
        else
        {
            if (!D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord2, gQuestLogQuestDefs[iDef].nQuestStateFlag, 15) || bPrimaryGoalDone || D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, gQuestLogQuestDefs[iDef].nQuestStateFlag, 1))
            {
                goto NOTHING_TO_SHOW;
            }
            nStatusStringId = STR_IDX_3730_qstsThankYouComeAgainMulti;
            if (gnGameType == GAMETYPE_SINGLEPLAYER)
            {
                nStatusStringId = STR_IDX_3729_qstsThankYouComeAgain;
            }
        }

        Unicode::strncpy((Unicode *)pEntry->wszText, D2LANG_GetStringFromTblIndex((D2C_StringIndices)nStatusStringId), 300);
        pEntry->wszText[299] = 0;
        pEntry->wTailStringId = STR_IDX_3725_qstsnull;
        pEntry->bHasText = 1;
        pEntry->nState = 3;
        return;
    }

BY_COLOUR:
    //  the same NULL the entry test above already refused
    if (!gQuestLogQuestDefs[iDef].pTextBlock)
    {
        goto NOTHING_TO_SHOW;
    }

    if (pTextBlock->wTitleStringId != STR_IDX_3714_qstsa1q1)
    {
        pEntry->wTitleStringId = pTextBlock->wTitleStringId;

        if ((bRewardGranted || bPrimaryGoalDone || D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, gQuestLogQuestDefs[iDef].nQuestStateFlag, 1) || !pQuestRecord2 || !D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord2,
                    gQuestLogQuestDefs[iDef].nQuestStateFlag, 13) || pEntry->nQuestStateFlag == 19) && !D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, gQuestLogQuestDefs[iDef].nQuestStateFlag, 14))
        {
            nStatusStringId = (&pTextBlock->wDescStringId)[2 * nStateColour];
            if (nStatusStringId == STR_IDX_3727_qstsother && gnGameType == GAMETYPE_SINGLEPLAYER)
            {
                nStatusStringId = STR_IDX_3729_qstsThankYouComeAgain;
            }
        }
        else
        {
            nStatusStringId = (gnGameType != GAMETYPE_SINGLEPLAYER) + 3729;
        }

        Unicode::strncpy((Unicode *)pEntry->wszText, D2LANG_GetStringFromTblIndex((D2C_StringIndices)nStatusStringId), 300);

        if (pTextBlock->wTitleStringId == STR_IDX_22622_qstsa5q2 && nStatusStringId == STR_IDX_22624_qstsa5q22)
        {
            if (gnBarbsToBeRescued)
            {
                Unicode::sprintf(300, (Unicode *)pEntry->wszText, D2LANG_GetStringFromTblIndex(STR_IDX_22624_qstsa5q22), gnBarbsToBeRescued);
            }
            else
            {
                Unicode::strncpy((Unicode *)pEntry->wszText, D2LANG_GetStringFromTblIndex(STR_IDX_3729_qstsThankYouComeAgain), 300);
            }
        }

        pEntry->wszText[299] = 0;
        pEntry->nFlag0x263 = 0;
        if (nStateColour != gQuestLogEntryColours[nEntry] && gQuestLogQuestDefs[iDef].pTextBlock)
        {
            pEntry->nFlag0x263 = 1;
        }
        pEntry->wTailStringId = (&pTextBlock->wStateIndex)[2 * nStateColour];
        gQuestLogEntryColours[nEntry] = nStateColour;
        pEntry->nState = 3;
        pEntry->bHasText = 1;
        if (nStateColour == 13)
        {
            goto UPDATE_BIT;
        }
        return;
    }

    //  the Den of Evil's own page: the template, and the count when the
    //  colour says the den is being cleared
    pTemplate = D2LANG_GetStringFromTblIndex((D2C_StringIndices)(&pTextBlock->wDescStringId)[2 * nStateColour]);

    if (nStateColour != 3 && nStateColour != 4)
    {
        pSrc = pTemplate;
        goto COPY_TEMPLATE;
    }

    //  reached only with wTitleStringId == 3714, so the first half of this
    //  test can never be true
    if (pTextBlock->wTitleStringId != STR_IDX_3714_qstsa1q1 || gnMonstersRemaining <= 1)
    {
        pSrc = D2LANG_GetStringFromTblIndex(STR_IDX_3739_qstsa1q140);
        goto COPY_TEMPLATE;
    }

    {
        //  ten elements, one default-constructor call each in the original (0x6FB052BE); the declaration constructs them here, with 0, which
        //  stores the same zero
        Unicode wszCount[10];
        char szCount[8];

        //  the same dead test again: `setnz / dec / and` at 0x6FB052E2 can
        //  only hand gnMonstersRemaining through
        sprintf(szCount, "%d", pTextBlock->wTitleStringId != STR_IDX_3714_qstsa1q1 ? 0 : gnMonstersRemaining);
        Unicode::toUnicode(wszCount, szCount, 9);

        Unicode::strcpy((Unicode *)pEntry->wszText, pTemplate);
        if (Unicode::strlen(pTemplate) >= 280)
        {
            FOG_DisplayAssert((uint32_t)szTemplateLengthAssert, (uint32_t)szQuestLogCpp, 1274);
            exit(-1);
        }
        Unicode::strcat((Unicode *)pEntry->wszText, wszCount);
    }
    //  and this path does NOT zero wszText[299] -- 0x6FB05359 jumps over the
    //  store the other one makes
    goto FINISH_BY_COLOUR;

COPY_TEMPLATE:
    Unicode::strncpy((Unicode *)pEntry->wszText, pSrc, 300);
    pEntry->wszText[299] = 0;

FINISH_BY_COLOUR:
    pEntry->wTitleStringId = pTextBlock->wTitleStringId;
    pEntry->nFlag0x263 = 0;
    if (nStateColour != gQuestLogEntryColours[nEntry])
    {
        pEntry->nFlag0x263 = 1;
    }
    gQuestLogEntryColours[nEntry] = nStateColour;
    pEntry->nState = 3;
    pEntry->wTailStringId = (&pTextBlock->wStateIndex)[2 * nStateColour];
    pEntry->bHasText = 1;
    pEntry->nStateColour = nStateColour;
    if (nStateColour != 13)
    {
        return;
    }

UPDATE_BIT:
    //  `neg / sbb / neg` at 0x6FB05564 -- the answer is normalised to 0 or 1
    pEntry->nState = D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, gQuestLogQuestDefs[iDef].nQuestStateFlag, 12) != 0;
    return;

SET_STATE_3:
    pEntry->nState = 3;
    pEntry->bHasText = 1;
    pEntry->nStateColour = nStateIndex;
    return;

NOTHING_TO_SHOW:
    pEntry->nState = 2;
}


//1.10f: D2Client.0x6FB042D0
//
//  The quest screen, drawn once a frame. Sole caller UIPANELS_DrawInGamePanels.
//
//  Three refusals, and the panel frees its own images: the player is dead or missing, UI window 18 is up, or a cinematic is playing. State 2 is
//  dropped to 0 with the close box and the speech flag, the images are unloaded and `UI_SetUIToggle(UI_QUESTSCREEN, UI_TURNOFF, 0)` closes the
//  window. The three teardowns are three separate copies in the bytes (0x6FB042F1, 0x6FB04337 and 0x6FB04378), each with its own epilogue and
//  `retn`.
//
//  The background is four quarters: frames 0..3 of `questbackground` at (0,-224), (256,-224), (0,-48) and (256,-48) from the panel origin, so
//  each quarter is 256 wide and the bottom pair sits 176 below the top pair. On the first draw after the images are gone it loads them and calls
//  `QUESTLOG_SelectActPage(0, 1)`, which is why the log always opens on Act I with the selection cleared -- and if the load fails the background
//  is skipped and the tabs still draw.
//
//  What each quest state draws:
//
//      0   the icon animating through gQuestLogIconFrame, one frame per
//          100 ms, and at frame 25 it stops at 24 and tells the server the
//          quest has been read (QUESTRECORD_SetQuestState flag 12 and
//          client command 0x58). Frame 1 plays sound 14, expansion only
//      1   the DONE cel when this slot is the pressed one, else icon frame
//          24
//      2   icon frame 26
//      3   icon frame 0, or 25 when this slot is pressed; and this is where
//          `bHasPictureQuest` is armed, for title 928 (`qstsa2q6`) with
//          state colour 7
//
//  and every entry, whatever its state, gets the socket cel underneath -- frame 1 and `gnQuestLogSelectedSocketYOffset` when the slot is the
//  selected quest, frame 0 and `gnQuestLogSocketYOffset` otherwise.
//
//  The picture needs three things at once: `bHasPictureQuest` (title 928, colour 7), `gnQuestLogSelectedQuest == 5`, and a frame in 0..6 --
//  anything outside that range draws frame 0, so the animation is seven frames. Drawn at the panel origin + (110, -110) with draw mode 3, where
//  everything else here uses 5.
//
//  The description is split and freed line by line: `D2WIN_10199(wszText, &nSplitLines, 270)` returns a linked list; each line is drawn 20
//  pixels below the last and then BOTH the line and its node are released with `FOG_Free`. The list is consumed as it is walked, so nothing
//  survives the frame. `nSplitLines` and the walk counter share one stack slot in the original (`[esp+104h]`); their lives do not overlap --
//  the walk is finished before the split runs -- so the C uses two locals.
//
//  BUG: the quest walk reads one record past the table. It ends on `++nLines > 41` and the increment is at the BOTTOM (0x6FB049EC), so the 42nd
//  iteration runs and reads `gQuestLogQuestDefs[41]`, four bytes past a 41-record array. What is there is the four globals after the table:
//
//      0x6FB7DBB8  gnQuestLogDoneYOffset             FF FF FF FF
//      0x6FB7DBBC  gnQuestLogSelectedSocketYOffset   FF FF FF FF
//      0x6FB7DBC0  gnQuestLogTextX                   10 00 00 00
//      0x6FB7DBC4  gnQuestLogTitleY                  F8 00 00 00
//
//  so the phantom record's `nAct` is the byte at 0x6FB7DBBB = 0xFF, and the walk's first test is `nAct == gnQuestLogActTab` with the tab
//  clamped to 0..4 by QUESTLOG_SetActTab. It can never match. LATENT: the first global has no writer (its one reference is a read in this
//  function), so the -1 is the value at rest and stays the value; the read is of loaded, named memory, so it does not fault and costs one
//  comparison a frame. Reproduced.
//  DIFFERENCE: the table and the four globals after it are objects of ours (QuestLog.h), so record [41] of our table is no memory of ours. The
//  42nd pass reads `tPastEnd` instead: a record built, before the walk, from the four globals' bytes in the image's order
//  (0x6FB7DBB8..0x6FB7DBC7) -- what the original's pointer reads, with their current values. The walk selects each record by its index rather
//  than stepping one pointer past the table.
//
//  The entry array is cleared six times over:
//
//      6FB04602  mov edx, 6
//      6FB04607  mov ecx, 39Fh          927 dwords = 3708 bytes
//      6FB0460C  xor eax, eax
//      6FB0460E  mov edi, offset gQuestLogEntries     <- INSIDE the loop
//      6FB04613  dec edx
//      6FB04614  rep stosd
//      6FB04616  jnz short 6FB04607
//
//  927 dwords is the WHOLE array (6 x 618) and `edi` is reloaded with the array's head on every pass, so the same 3708 bytes are zeroed six
//  times. Five of the six do nothing. Written as the bytes have it.
//
//  The act tabs come out of a stack array that is filled twice: five x positions in expansion (5, 67, 129, 191, 253) and four in classic (5, 82,
//  159, 236), written into the same five stack slots at the top of EVERY iteration and indexed by the tab number. In classic the fifth slot is
//  never written -- and never read either, because `if (nTab > 3) break` comes before the index. The C declares the array at function scope, as
//  the frame has it, and fills it in the loop.
//
//  One CelDraw call, two argument sequences: the active and inactive tab draws each push their own six arguments and then jump to the same
//  `call D2GFX_TEXTURE_CelDraw` at 0x6FB045EF (0x6FB04555 is the jump), and the C writes the two calls out.
//
//  Eleven D2ExpRes detours land in this function. Each site is commented on the line it lands on; the rewrite keeps the stock numbers.
void __cdecl QUESTLOG_Draw(void)
{
    D2GfxDataStrc tCelContext;
    int32_t anTabX[5];
    int32_t nTab;
    int32_t nActiveTabY;
    int32_t nQuest;
    D2BitBufferStrc *pQuestRecord;
    BOOL bQuestState;
    int32_t nEntryCount;
    D2QuestLogQuestDefStrc *pDef;
    D2QuestLogQuestDefStrc tPastEnd;   // the 16 bytes after gQuestLogQuestDefs, read on the 42nd pass
    int32_t nHoveredSlot;
    int32_t nIconFrame;
    uint32_t dwTick;
    int32_t nSlot;
    int32_t nDrawX;
    int32_t nDrawY;
    int32_t nSocketYOffset;
    int32_t nLines;
    int32_t nSplitLines;
    uint8_t bDone;
    uint8_t bHasPictureQuest;
    int32_t iEntry;
    uint8_t *pEntrySlot;
    int32_t nEntryOffset;
    const Unicode *wszTitle;
    D2SplittedTextStrc *pSplitText;
    D2SplittedTextStrc *pSplitNext;
    int32_t iLineY;
    int32_t nMouseXRel;
    int32_t nMouseYRel;
    const Unicode *wszText;
    int32_t i;

    memset(&tCelContext, 0, sizeof(tCelContext));
    tCelContext.nDirection = 0;

    if (CUNIT_IsPlayerDeadOrMissing())
    {
        if (gnQuestLogPanelState == 2)
        {
            gnQuestLogPanelState = 0;
            gbQuestLogCloseBoxDown = 0;
            gbQuestScreenSpeechPlaying = 0;
        }
        QUESTLOG_QuestUnloadPanelImages();
        UI_SetUIToggle(UI_QUESTSCREEN, UI_TURNOFF, 0);
        return;
    }

    if (UI_GetUiWindowState(UI_UNKNOWN18))
    {
        if (gnQuestLogPanelState == 2)
        {
            gnQuestLogPanelState = 0;
            gbQuestLogCloseBoxDown = 0;
            gbQuestScreenSpeechPlaying = 0;
        }
        QUESTLOG_QuestUnloadPanelImages();
        UI_SetUIToggle(UI_QUESTSCREEN, UI_TURNOFF, 0);
        return;
    }

    if (VIDEO_IsCinematicPlaying())
    {
        if (gnQuestLogPanelState == 2)
        {
            gnQuestLogPanelState = 0;
            gbQuestLogCloseBoxDown = 0;
            gbQuestScreenSpeechPlaying = 0;
        }
        QUESTLOG_QuestUnloadPanelImages();
        UI_SetUIToggle(UI_QUESTSCREEN, UI_TURNOFF, 0);
        return;
    }

    if (!gQuestLogBackgroundImage)
    {
        QUESTLOG_LoadPanelImages();
        QUESTLOG_SelectActPage(0, 1);
    }
    if (gQuestLogBackgroundImage)
    {
        tCelContext.pCellFile = gQuestLogBackgroundImage;
        tCelContext.nFrame = 0;
        D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
        tCelContext.nFrame = 1;
        D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference + 256, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
        tCelContext.nFrame = 2;
        D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
        tCelContext.nFrame = 3;
        D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference + 256, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
    }

    if (gQuestLogTabsImage)
    {
        tCelContext.pCellFile = gQuestLogTabsImage;

        for (nTab = 0; nTab < 5; ++nTab)
        {
            nActiveTabY = 0;
            if (FOG_IsExpansion() && GAME_IsExpansionGame())
            {
                nActiveTabY = 1;
                //  D2ExpRes detour: QuestActsignsX [Quest]
                anTabX[0] = 5;
                anTabX[1] = 67;
                anTabX[2] = 129;
                anTabX[3] = 191;
                anTabX[4] = 253;
            }
            else
            {
                anTabX[0] = 5;
                anTabX[1] = 82;
                anTabX[2] = 159;
                anTabX[3] = 236;
                //  the fifth slot is neither written nor read in classic
                if (nTab > 3)
                {
                    break;
                }
            }

            if (nTab == gnQuestLogActTab)
            {
                tCelContext.nFrame = 2 * nTab;
                //  D2ExpRes detour: QuestActsignsActiveY [Quest]
                D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference + anTabX[nTab], gnScreenHeight + gnBorderYDifference + nActiveTabY - 448, 0xFFFFFFFF, 5, NULL);
                continue;
            }

            //  tab 0 draws unconditionally; 1..4 each need their act's last
            //  quest to have paid out, and tab 4 needs the expansion besides
            if ((uint32_t)(nTab - 1) <= 3)
            {
                bQuestState = 1;
                switch (nTab)
                {
                case 1:
                    nQuest = 7;
                    goto TAB_GATE;
                case 2:
                    nQuest = 15;
                    goto TAB_GATE;
                case 3:
                    nQuest = 23;
TAB_GATE:
                    pQuestRecord = NPCMENU_QuestGetQuestRecord();
                    if (!D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, nQuest, 0))
                    {
                        continue;
                    }
                    break;
                case 4:
                    pQuestRecord = NPCMENU_QuestGetQuestRecord();
                    //  `test eax, eax / jnz / xor bl, bl` -- normalised
                    bQuestState = D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, 26, 0) != 0;
                    if (FOG_IsExpansion() && GAME_IsExpansionGame() && bQuestState)
                    {
                        break;
                    }
                    continue;
                }
            }

            tCelContext.nFrame = 2 * nTab + 1;
            //  D2ExpRes detour: QuestActsignsNotActiveY [Quest]
            D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference + anTabX[nTab], gnScreenHeight + gnBorderYDifference + nActiveTabY - 448, 0xFFFFFFFF, 5, NULL);
        }
    }

    //  six passes over the WHOLE array
    for (i = 0; i < 6; ++i)
    {
        memset(gQuestLogEntries, 0, 6 * sizeof(D2QuestLogEntryStrc));   // the array's six records exactly
    }

    bHasPictureQuest = 0;

    if (gnQuestLogActTab < 5)
    {
        nEntryCount = 0;
        gnQuestLogEntryCount = 0;
        nLines = 0;
        bDone = 0;
        pDef = &gQuestLogQuestDefs[0];
        memcpy((char *)&tPastEnd + 0, &gnQuestLogDoneYOffset, 4);              // 0x6FB7DBB8
        memcpy((char *)&tPastEnd + 4, &gnQuestLogSelectedSocketYOffset, 4);    // 0x6FB7DBBC
        memcpy((char *)&tPastEnd + 8, &gnQuestLogTextX, 4);                    // 0x6FB7DBC0
        memcpy((char *)&tPastEnd + 12, &gnQuestLogTitleY, 4);                  // 0x6FB7DBC4

        do
        {
            nHoveredSlot = -1;

            if (pDef->nAct == gnQuestLogActTab && pDef->bHasIcon && pDef->nIconIndex < 27 && gQuestLogIconImages[pDef->nIconIndex] && nEntryCount < 6)
            {
                //  the +12 column again -- the QUESTSTATEFLAG index, which
                //  is what QUESTLOG_BuildQuestDisplayInfo scans for
                gQuestLogEntries[nEntryCount].nQuestStateFlag = pDef->nQuestStateFlag;
                QUESTLOG_BuildQuestDisplayInfo(&gQuestLogEntries[gnQuestLogEntryCount]);

                switch (gQuestLogEntries[gnQuestLogEntryCount].nState)
                {
                case 0:
                    gQuestLogEntries[gnQuestLogEntryCount].bShown = 1;
                    nIconFrame = gnQuestLogIconFrame[gnQuestLogEntryCount];
                    if (nIconFrame >= 25)
                    {
                        nIconFrame = 24;
                        pQuestRecord = NPCMENU_QuestGetQuestRecord();
                        D2COMMON_QUESTRECORD_SetQuestState(pQuestRecord, pDef->nQuestStateFlag, 12);
                        MSG_SendCmdWord(88, (int16_t)pDef->nQuestStateFlag);
                    }
                    tCelContext.nFrame = nIconFrame;
                    tCelContext.pCellFile = gQuestLogIconImages[pDef->nIconIndex];
                    D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference + sgQuestLogButtonPositions[pDef->nSlotInPage].nDrawX, sgQuestLogButtonPositions[pDef->nSlotInPage].nDrawY - gnBorderYDifference, 0xFFFFFFFF, 5, NULL);

                    dwTick = GetTickCount();
                    if (!gnQuestLogIconLastTick[gnQuestLogEntryCount])
                    {
                        gnQuestLogIconLastTick[gnQuestLogEntryCount] = dwTick;
                    }
                    if (dwTick - gnQuestLogIconLastTick[gnQuestLogEntryCount] > 100)
                    {
                        gnQuestLogIconLastTick[gnQuestLogEntryCount] = dwTick;
                        if (++gnQuestLogIconFrame[gnQuestLogEntryCount] == 1)
                        {
                            if (FOG_IsExpansion())
                            {
                                SOUNDINST_PlaySound(SOUND_CURSOR_QUESTDONE, NULL, 0, 0, 0);
                            }
                        }
                    }
                    nHoveredSlot = pDef->nSlotInPage;
                    ++gnQuestLogEntryCount;
                    break;

                case 1:
                    gQuestLogEntries[gnQuestLogEntryCount].bShown = 1;
                    nSlot = pDef->nSlotInPage;
                    if (gnQuestScreenPressedQuest == nSlot)
                    {
                        tCelContext.pCellFile = gQuestLogDoneImage;
                        tCelContext.nFrame = pDef->nIconIndex;
                        nDrawY = gnQuestLogDoneYOffset + sgQuestLogButtonPositions[nSlot].nDrawY - gnBorderYDifference;
                        nDrawX = gnQuestLogDoneXOffset + sgQuestLogButtonPositions[nSlot].nDrawX;
                    }
                    else
                    {
                        tCelContext.nFrame = 24;
                        tCelContext.pCellFile = gQuestLogIconImages[pDef->nIconIndex];
                        nDrawX = sgQuestLogButtonPositions[nSlot].nDrawX;
                        nDrawY = sgQuestLogButtonPositions[nSlot].nDrawY - gnBorderYDifference;
                    }
                    D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference + nDrawX, nDrawY, 0xFFFFFFFF, 5, NULL);
                    nHoveredSlot = pDef->nSlotInPage;
                    ++gnQuestLogEntryCount;
                    break;

                case 2:
                    gQuestLogEntries[gnQuestLogEntryCount].bShown = 1;
                    nSlot = pDef->nSlotInPage;
                    tCelContext.pCellFile = gQuestLogIconImages[pDef->nIconIndex];
                    tCelContext.nFrame = 26;
                    D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference + sgQuestLogButtonPositions[nSlot].nDrawX, sgQuestLogButtonPositions[nSlot].nDrawY - gnBorderYDifference, 0xFFFFFFFF, 5, NULL);
                    nHoveredSlot = pDef->nSlotInPage;
                    ++gnQuestLogEntryCount;
                    break;

                case 3:
                    if (gQuestLogEntries[gnQuestLogEntryCount].wTitleStringId == STR_IDX_928_qstsa2q6 && gQuestLogEntries[gnQuestLogEntryCount].nStateColour == 7)
                    {
                        bHasPictureQuest = 1;
                    }
                    gQuestLogEntries[gnQuestLogEntryCount].bShown = 1;
                    nSlot = pDef->nSlotInPage;
                    tCelContext.pCellFile = gQuestLogIconImages[pDef->nIconIndex];
                    tCelContext.nFrame = (gnQuestScreenPressedQuest != nSlot) ? 0 : 25;
                    D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference + sgQuestLogButtonPositions[nSlot].nDrawX, sgQuestLogButtonPositions[nSlot].nDrawY - gnBorderYDifference, 0xFFFFFFFF, 5, NULL);
                    nHoveredSlot = pDef->nSlotInPage;
                    ++gnQuestLogEntryCount;
                    break;

                default:
                    break;
                }

                tCelContext.pCellFile = gQuestLogSocketsImage;
                nSlot = pDef->nSlotInPage;
                if (nHoveredSlot == (int32_t)gnQuestLogSelectedQuest)
                {
                    tCelContext.nFrame = 1;
                    nSocketYOffset = gnQuestLogSelectedSocketYOffset;
                }
                else
                {
                    tCelContext.nFrame = 0;
                    nSocketYOffset = gnQuestLogSocketYOffset;
                }
                D2GFX_TEXTURE_CelDraw(&tCelContext, sgQuestLogButtonPositions[nSlot].nDrawX + gnBorderXDifference - 4, sgQuestLogButtonPositions[nSlot].nDrawY - gnBorderYDifference + nSocketYOffset + 5, 0xFFFFFFFF, 5, NULL);

                nEntryCount = gnQuestLogEntryCount;
            }

            //  the increment is at the BOTTOM, so the 42nd pass reads the record after the table -- the original's gQuestLogQuestDefs[41],
            //  tPastEnd here
            if (++nLines > 41 || nEntryCount >= 6)
            {
                bDone = 1;
            }
            pDef = nLines < 41 ? &gQuestLogQuestDefs[nLines] : &tPastEnd;   // the original's `add esi, 10h`
        }
        while (!bDone);

        if (bHasPictureQuest && gnQuestLogSelectedQuest == 5)
        {
            tCelContext.pCellFile = gQuestLogInvPsImage;
            tCelContext.nFrame = gnQuestLogPictureFrame;
            if ((int32_t)gnQuestLogPictureFrame < 0 || (int32_t)gnQuestLogPictureFrame > 6)
            {
                tCelContext.nFrame = 0;
            }
            //  D2ExpRes detour: QuestActIISignOfTrueTomb [Quest]
            D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference + 110, gnBorderYDifference + gnScreenHeight - 110, 0xFFFFFFFF, 3, NULL);
        }
    }

    D2WIN_10127_SetFont((enum Font)8);

    if ((int32_t)gnQuestLogSelectedQuest != -1 && (int32_t)gnQuestLogSelectedQuest < 6 && (int32_t)gnQuestLogSelectedQuest < (int32_t)gnQuestLogEntryCount && (int32_t)gnQuestLogEntryCount > 0)
    {
        iEntry = 0;
        pEntrySlot = &gQuestLogEntries[0].nSlotInPage;
        while (*pEntrySlot != (int32_t)gnQuestLogSelectedQuest)
        {
            ++iEntry;
            pEntrySlot += 618;
            if (iEntry >= (int32_t)gnQuestLogEntryCount)
            {
                goto BOTTOM_BUTTONS;
            }
        }

        //  iEntry only counts up from 0, so this can never fire
        if (iEntry != -1)
        {
            nEntryOffset = iEntry;
            if (gQuestLogEntries[nEntryOffset].bShown)
            {
                if (gQuestLogEntries[nEntryOffset].wTitleStringId != STR_IDX_3724_qstsxxx)
                {
                    wszTitle = D2LANG_GetStringFromTblIndex((D2C_StringIndices)gQuestLogEntries[nEntryOffset].wTitleStringId);
                    //  D2ExpRes detour: QuestQuestnameX [Quest]
                    D2WIN_10117_DrawText(wszTitle, gnBorderXDifference - D2WIN_10121_GetTextWidth(wszTitle) / 2 + 160, gnQuestLogTitleY - gnBorderYDifference, 0, 0);
                }

                if (!gbQuestScreenSpeechPlaying && gQuestLogEntries[nEntryOffset].wszText[0])
                {
                    pSplitText = D2WIN_10199((const Unicode *) gQuestLogEntries[nEntryOffset].wszText, &nSplitLines, 270);
                    if (pSplitText)
                    {
                        iLineY = 0;
                        do
                        {
                            D2WIN_10117_DrawText(pSplitText->wszLine, gnQuestLogTextX + gnBorderXDifference, gnQuestLogTextY + iLineY - gnBorderYDifference, 0, 0);
                            pSplitNext = pSplitText->pNextLine;
                            FOG_Free((uint32_t)pSplitText->wszLine, (uint32_t)szQuestLogCpp, 2257, 0);
                            FOG_Free((uint32_t)pSplitText, (uint32_t)szQuestLogCpp, 2258, 0);
                            iLineY += 20;
                            pSplitText = pSplitNext;
                        }
                        while (pSplitNext);
                    }
                }
            }
        }
    }

BOTTOM_BUTTONS:
    tCelContext.pCellFile = UI_GetBuySellButtonImage();
    tCelContext.nFrame = (gbQuestScreenCloseButtonDown != 0) + 10;
    //  D2ExpRes detour: QuestCloseButton [Quest]
    D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference + 278, gnBorderYDifference + gnScreenHeight - 58, 0xFFFFFFFF, 5, NULL);

    nMouseXRel = CURSOR_GetMouseX() - gnBorderXDifference;
    nMouseYRel = CURSOR_GetMouseY() + gnBorderYDifference;

    //  D2ExpRes detour: QuestCloseButtonMouseHoverOver [Quest]
    if (nMouseXRel >= 278 && nMouseXRel <= 313 && nMouseYRel >= 392 && nMouseYRel <= 425)
    {
        D2WIN_10127_SetFont((enum Font)9);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4144_strClose);
        //  D2ExpRes detour: QuestCloseString [Quest]
        D2WIN_10129_DrawFramedText(wszText, gnBorderXDifference + 296, gnBorderYDifference + gnScreenHeight - 95, 0, 1);
    }

    tCelContext.pCellFile = gQuestLogLastImage;
    tCelContext.nFrame = gbQuestScreenSpeechButtonDown != 0;
    //  D2ExpRes detour: QuestSpeechButton [Quest]
    D2GFX_TEXTURE_CelDraw(&tCelContext, gnBorderXDifference + 226, gnBorderYDifference + gnScreenHeight - 58, 0xFFFFFFFF, 5, NULL);

    //  D2ExpRes detour: QuestSpeechButtonMouseHoverOver [Quest]
    if (nMouseXRel > 229 && nMouseXRel < 260 && nMouseYRel > 390 && nMouseYRel < 424)
    {
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_3720_strplaylast);
        D2WIN_10127_SetFont((enum Font)9);
        //  D2ExpRes detour: QuestSpeechString [Quest]
        D2WIN_10129_DrawFramedText(wszText, gnBorderXDifference + 241, gnBorderYDifference + gnScreenHeight - 95, 0, 1);
    }
}


//  The QUEST_ functions whose bytes lie in this unit's address block, in address order, with the assertions and the registry strings only they
//  read.

static_assert(MONSTER_CAIN1 == 146,
              "QUESTLOG_QuestMakeCainUntargetable: cmp dword ptr [ecx+4], 92h");
static_assert(STR_IDX_3708_stsa1q3alert == 0xE7C && STR_IDX_3710_stsa3q1alert == 0xE7E,
              "the two alert strings: mov ecx, 0E7Ch / mov ecx, 0E7Eh");
static_assert(STR_IDX_666_CainAct4IntroGossip1 == 666,
              "the clamp stores 0x29A -- 666, which is also this table index; "
              "it is a count sentinel here, not a string id");
//  Not re-asserted here: MONSTER_DURIEL (the quest-13 alert arm), and D2QuestLogQuestDefStrc's nQuestId, nAct and nSlotInPage (the walks of
//  QUESTLOG_QuestOnQuestStatePacket).

//  The two registry strings, QUESTLOG_QuestResetAllState's. Each file that writes this key carries its own copy. "GAMEOVER" is the case the
//  bytes use at 0x6FB7DCA4.
static const char szDiabloIIKey[] = "Diablo II";
static const char szGameover[] = "GAMEOVER";


//1.10f: D2Client.0x6FB04140
//
//  Resets everything the quest log owns, from UI_InitUiSubsystems on the way into a game. Sole caller UI_InitUiSubsystems at 0x6FB2029B.
//  The original inlines QUESTLOG_ClearEntryState here.
//
//  The pieces, in the bytes' order:
//
//      STORM_SRegSaveValue("Diablo II", "GAMEOVER", 0, 0)  -- the one thing
//          that outlives the process; `dwValue` and `dwFlags` are the two
//          zero pushes at 0x6FB04144/0x6FB04145
//      gnQuestLogPanelState and the six image pointers to NULL
//      27 dwords of gQuestLogIconImages, `mov ecx, 1Bh; rep stosd`
//      gnMonstersRemaining, gnBarbsToBeRescued and gdwQuestLogCloseTick to 0
//      41 flags and 41 colours, one `bl` store each per iteration
//      the four mouse flags, the four one-shot flags
//          (gbDurielCofLoaded, gbAncientStatueCofLoaded, gbZooArmed,
//          gbZooChickensSpawned) and gbQuestScreenSpeechPlaying to 0
//      the five gnQuestLogLastClickedPerAct entries to -1 -- written as
//          `or eax, -1` once at 0x6FB041B1 and then stored five times,
//          interleaved with four of those byte flags
//      gnQuestScreenPressedQuest to -1
//
//  The last-clicked array is five entries and the compiler interleaved its clears with the flags, which is why the two must not be written as
//  two tidy groups: the bytes go flag, -1, flag, -1, flag, -1, flag, -1, then FIVE byte stores -- the four one-shot flags and
//  gbQuestScreenSpeechPlaying (0x6FB041E0..0x6FB041F8) -- then the last -1 after gnQuestScreenPressedQuest.
//  The 41 is `gQuestLogQuestDefs`' count, read whole for the sweep: 656 bytes at 0x6FB7D928, records of 16.
void __cdecl QUESTLOG_QuestResetAllState(void)
{
    STORM_SRegSaveValue(szDiabloIIKey, szGameover, 0, 0);

    gnQuestLogPanelState = 0;
    gQuestLogBackgroundImage = NULL;
    gQuestLogTabsImage = NULL;
    gQuestLogSocketsImage = NULL;
    gQuestLogDoneImage = NULL;
    gQuestLogInvPsImage = NULL;
    gQuestLogLastImage = NULL;

    //  `mov ecx, 1Bh` -- 27 dwords, which is gQuestLogIconImages' extent
    memset(gQuestLogIconImages, 0, 0x1B * sizeof(D2CellFileStrc *));   // the array's 108 bytes exactly

    gnMonstersRemaining = 0;
    gnBarbsToBeRescued = 0;
    gdwQuestLogCloseTick = 0;

    QUESTLOG_ClearEntryState();

    gbQuestScreenIconDown = 0;
    gnQuestLogLastClickedPerAct[0] = -1;
    gbQuestScreenCloseButtonDown = 0;
    gnQuestLogLastClickedPerAct[1] = -1;
    gbQuestLogCloseBoxDown = 0;
    gnQuestLogLastClickedPerAct[2] = -1;
    gbQuestScreenSpeechButtonDown = 0;
    gnQuestLogLastClickedPerAct[3] = -1;
    gbDurielCofLoaded = 0;
    gbAncientStatueCofLoaded = 0;
    gbZooArmed = 0;
    gbZooChickensSpawned = 0;
    gbQuestScreenSpeechPlaying = 0;
    gnQuestScreenPressedQuest = -1;
    gnQuestLogLastClickedPerAct[4] = -1;
}


//1.10f: D2Client.0x6FB04210
//
//  A five-byte jump thunk to QUESTLOG_QuestUnloadPanelImages (`jmp` and an `align 10h`), taken by UI_ShutdownUiSubsystems at 0x6FB20329 while
//  every other caller reaches the real function directly. A linker artefact, not a second entry point, but it has its own address and is its
//  own function here. A jmp thunk's signature is its target's, and 0x6FB04220 takes nothing.
void __cdecl j_QUESTLOG_QuestUnloadPanelImages(void)
{
    QUESTLOG_QuestUnloadPanelImages();
}


//1.10f: D2Client.0x6FB04220
//
//  Unloads the quest panels' images -- six named pointers and the 27-entry icon array -- each null-checked, unloaded through
//  ARCHIVE_CellFileFree, and the slot cleared after.
//  The bytes unload, in order, background (0x6FBB5F95), tabs (0x6FBB5F99), last (0x6FBB600D), sockets (0x6FBB5F91), done (0x6FBB5F9D) and
//  invps (0x6FBB6011) -- the last three AFTER `last`, so the source order is not the declaration order and is kept as the bytes have it.
//  The walk's bound is the next global's address:
//
//      mov  esi, offset gQuestLogIconImages ; 0x6FBB5FA1
//      ...  add esi, 4
//      cmp  esi, offset gQuestLogLastImage  ; 0x6FBB600D
//      jl   short loc_6FB042A7
//
//  `(0x6FBB600D - 0x6FBB5FA1) / 4 = 27`, and the sentinel is the ADDRESS of gQuestLogLastImage, not its contents.
void __cdecl QUESTLOG_QuestUnloadPanelImages(void)
{
    D2CellFileStrc **pIconImage;

    if (gQuestLogBackgroundImage)
    {
        ARCHIVE_CellFileFree(gQuestLogBackgroundImage);
        gQuestLogBackgroundImage = NULL;
    }
    if (gQuestLogTabsImage)
    {
        ARCHIVE_CellFileFree(gQuestLogTabsImage);
        gQuestLogTabsImage = NULL;
    }
    if (gQuestLogLastImage)
    {
        ARCHIVE_CellFileFree(gQuestLogLastImage);
        gQuestLogLastImage = NULL;
    }
    if (gQuestLogSocketsImage)
    {
        ARCHIVE_CellFileFree(gQuestLogSocketsImage);
        gQuestLogSocketsImage = NULL;
    }
    if (gQuestLogDoneImage)
    {
        ARCHIVE_CellFileFree(gQuestLogDoneImage);
        gQuestLogDoneImage = NULL;
    }
    if (gQuestLogInvPsImage)
    {
        ARCHIVE_CellFileFree(gQuestLogInvPsImage);
        gQuestLogInvPsImage = NULL;
    }

    pIconImage = gQuestLogIconImages;
    do
    {
        if (*pIconImage)
        {
            ARCHIVE_CellFileFree(*pIconImage);
            *pIconImage = 0;
        }
        ++pIconImage;
    }
    //  `cmp esi, 6FBB600Dh` / `jl` (0x6FB042B7): gQuestLogLastImage's ADDRESS, the end of gQuestLogIconImages in the image. The two are separate
    //  objects of ours, so the bound is the array's own one-past-end, compared signed as the bytes compare it.
    while ((int32_t)pIconImage < (int32_t)(gQuestLogIconImages + 27));
}


//1.10f: D2Client.0x6FB06280
//
//  The Act 1 Den of Evil status, out of server packet 0x50 case 1. It has no code caller: QUESTS_DispatchQuestPacket tail-jumps here from
//  0x6FAB8E37.
//
//  Three words of the payload, zero-extended into three dwords:
//
//      +3  ->  gnMonstersRemaining      (nMonstersToBeKilled)
//      +5  ->  gnQuestLogPictureFrame   (which tomb picture QUESTLOG_Draw shows)
//      +7  ->  gnBarbsToBeRescued       (nBarbsToBeRescued: the act-5
//                                        soldier count)
//
//  The guard is `*(WORD *)(ptCmd + 1) == 1`: the word is the packet's nQuestId and 1 is QUEST_A1Q1_DENOFEVIL, which is exactly why the +3 word
//  is the Den's count. The original decrements it and tests the flags (`mov ax, [ecx+1]; dec eax; jnz`) where the C compares.
void __fastcall QUESTLOG_QuestOnQuestLogStateCmd(uint8_t *ptCmd)
{
    if (*(uint16_t *)(ptCmd + 1) == 1)
    {
        gnMonstersRemaining = *(uint16_t *)(ptCmd + 3);
        gnQuestLogPictureFrame = *(uint16_t *)(ptCmd + 5);
        gnBarbsToBeRescued = *(uint16_t *)(ptCmd + 7);
    }
}


//1.10f: D2Client.0x6FB06900
//
//  Server packet 0x5D, the quest state message, six bytes. Sole caller SCMD_HandleSrv5D (0x6FAB3E1B), which copies the packet to its own stack
//  and calls with ecx = 1; the first parameter is a constant no line here reads, hence `nUnused`.
//
//  Four ways in by `nFlags`:
//
//      & 1   seven quests get an effect: 3 and 15 push an alert line into
//            the chat (tbl 3708, 3710), 4 makes every cain1 untargetable,
//            10 re-applies the act environment, 13 preloads Duriel's COF,
//            23 disconnects, 33 plays sound 237
//      & 2   4 plays 241; 8/15/18/22/35 play 7; 23 is Diablo down -- the
//            end of a classic game, act V's intro to come in an expansion
//            one; 32 plays 217; 33 plays 243; 36 sets the pending
//            out-cinematic -- and nothing under & 2 falls through either
//      == 16 quest 10 plays 2456 then 2474; quest 33 plays `field_4` as a
//            sound id
//      & 0x20  32 stores `field_4` into gnBarbsToBeRescued (the act-5
//            soldiers again), 33 makes drehyaiced untargetable, and
//            anything else falls to the count arm below
//
//  Everything else -- no flags at all included -- joins at `record_state`.
//
//  The count arm, and the 666:
//
//      movsx eax, word ptr [esi+4]     ; the count, SIGNED
//      cmp   eax, 5
//      mov   gnMonstersRemaining, eax  ; stored FIRST, unconditionally
//      jg    ...                       ; > 5  -> overwrite with 0x29A
//      test  eax, eax
//      jge   ...                       ; 0..5
//      <0                              -> overwrite with 0x29A
//
//  The clamp is 0..5 or 666, and it is the original's: a count outside that window is replaced by 0x29A (666), a sentinel here, and
//  QUESTLOG_BuildQuestDisplayInfo formats the value as a plain number into the Den of Evil row. The bytes are `jg` and the `jge` after `test`,
//  both SIGNED, which is what the C writes. Reproduced: the same packet's nState colours the row and the colour-3/4 branch of
//  QUESTLOG_BuildQuestDisplayInfo prints this global into the Den of Evil row.
//  The `== 5` sub-case is shared with quest 23, whose `field_4` is a countdown in seconds: it arms the game-end deadline as `max(1, field_4)`
//  and otherwise skips the walk entirely -- a count of exactly 5 leaves the log flag alone.
//
//  The walk, and the store at -1: the count arm walks `gQuestLogQuestDefs` for dense id 1 (the Den's own record) exactly like the two walks
//  under `record_state`, but on walking past the end it sets the index to -1 and stores anyway:
//
//      6FB06B70  or    ecx, 0FFFFFFFFh
//      6FB06B77  mov   gQuestLogEntryFlags[ecx], dl
//
//  BUG: `gQuestLogEntryFlags[-1]` is 0x6FBB6015, the byte before the array. LATENT: a record with dense id 1 exists in the static table (row 1
//  of gQuestLogQuestDefs carries dense id 1), so the walk always finds it. Written out because the bytes have it.
//  DIFFERENCE: both tables are objects of ours (QuestLog.h), and two things the bytes do leave our objects. The three walks step a pointer to
//  &gQuestLogQuestDefs[41].nQuestId, 0x6FB7DBC0 -- eight bytes past the table (`cmp eax, 6FB7DBC0h` / `jl`, 0x6FB06AE3, 0x6FB06B69,
//  0x6FB06BF0): the cursor is an integer, stepped and compared signed as they are, and read only inside the table. And the store at -1 reaches
//  0x6FBB6015, which is gQuestListPacketCopy: index -1 of our flags array is no object, so that arm stores into gQuestListPacketCopy by name --
//  the byte the original writes, with the value it writes.
//
//  BUG: the record test passes NULL on:
//
//      if ((!pQuestRecord || !GetQuestState(pQuestRecord, 1, 0))
//          && !GetQuestState(pQuestRecord, 1, 1))
//
//  When `pQuestRecord` is NULL the first half short-circuits TRUE and the SECOND call still runs, with the NULL the test just found: the bytes
//  jump from `test edi, edi; jz` straight to the second call at 0x6FB06B24 with edi = 0. QUESTRECORD_GetQuestState asserts its record and then
//  dereferences it, so the guard does not guard. LATENT: the record is NULL only between NPCMENU_Reset's Shutdown/Init pair and after
//  UI_ShutdownUiSubsystems, neither of which can dispatch a packet.
//
//  BUG: record_state is reached with pPacket NULL, and uses it. `if (!pPacket) goto record_state` is the first line, and the label's first acts
//  are `pPacket->nQuestId` reads (0x6FB06ACF, 0x6FB06BE0), so the null test precedes an unconditional use. LATENT: the one caller passes its own
//  stack copy.
//
//  TEXT_PrintChatMessage's second argument is only dl: `xor dl, dl` at 0x6FB06959/0x6FB0697A leaves edx's upper bytes holding the packet
//  pointer, and the callee truncates to one byte where it stores it, so a plain 0 is the same value to the same byte.
void __fastcall QUESTLOG_QuestOnQuestStatePacket(int32_t nUnused, D2GSPacketSrv5D *pPacket)
{
    uint8_t nAlertFlags;
    Unicode *pAlertText;
    int32_t nQuestId;
    int32_t iQuestDef;
    UINT_PTR nDefQuestId;   // an integer cursor: the walks below end past gQuestLogQuestDefs
    D2BitBufferStrc *pQuestRecord;
    D2QuestLogQuestDefStrc *pQuestDef;
    int32_t nQuestArg;
    int32_t nCountdown;
    int16_t nCountdownSeconds;

    if (!pPacket)
    {
        goto record_state;
    }

    nAlertFlags = pPacket->nFlags;
    if ((nAlertFlags & 1) != 0)
    {
        switch (pPacket->nQuestId)
        {
        case 3:
            pAlertText = (Unicode *)D2LANG_GetStringFromTblIndex(STR_IDX_3708_stsa1q3alert);
            TEXT_PrintChatMessage(pAlertText, 0);
            break;
        case 4:
            CUNIT_ForEachMonsterUnit(QUESTLOG_QuestMakeCainUntargetable);
            break;
        case 10:
            GAME_EnvReapplyActEnvironment();
            break;
        case 13:
            GFX_AddMonsterCofFiles(MONSTER_DURIEL, 1);
            break;
        case 15:
            pAlertText = (Unicode *)D2LANG_GetStringFromTblIndex(STR_IDX_3710_stsa3q1alert);
            TEXT_PrintChatMessage(pAlertText, 0);
            break;
        case 23:
            GAME_SetDisconnecting();
            break;
        case 33:
            SOUNDINST_PlaySound(SOUND_ITEM_POTION, NULL, 0, 0, 0);
            break;
        default:
            return;
        }
        return;
    }

    if ((nAlertFlags & 2) != 0)
    {
        switch (pPacket->nQuestId)
        {
        case 4:
            SOUNDINST_PlaySound(SOUND_ITEM_RING, NULL, 0, 0, 0);
            break;
        case 8:
        case 15:
        case 18:
        case 22:
        case 35:
            SOUNDINST_PlaySound(SOUND_CURSOR_LEVEL_UP, NULL, 0, 0, 0);
            break;
        case 23:
            if (GAME_IsExpansionGame())
            {
                VIDEO_SetActVIntroPending();
            }
            else
            {
                GAME_MarkClassicCompletion();
                VIDEO_SetGameCompletedClassic();
            }
            break;
        case 32:
            SOUNDINST_PlaySound(SOUND_ITEM_GEM, NULL, 0, 0, 0);
            break;
        case 33:
            SOUNDINST_PlaySound(SOUND_ITEM_SCROLL, NULL, 0, 0, 0);
            break;
        case 36:
            GAME_SetD2xOutCinematicPending(1);
            break;
        default:
            return;
        }
        return;
    }

    if (nAlertFlags == 16)
    {
        nQuestId = pPacket->nQuestId;
        if (nQuestId == 10)
        {
            SOUNDINST_PlaySound(SOUND_NECROMANCER_CONFUSE, NULL, 0, 0, 0);
            SOUNDINST_PlaySound(SOUND_NECROMANCER_REVERSEVAMPIRE, NULL, 0, 0, 0);
        }
        else if (nQuestId == 33)
        {
            SOUNDINST_PlaySound((D2C_Sounds)(int16_t)pPacket->field_4, NULL, 0, 0, 0);
        }
        return;
    }

    if ((nAlertFlags & 0x20) == 0)
    {
        goto record_state;
    }

    nQuestId = pPacket->nQuestId;
    if (nQuestId == 32)
    {
        gnBarbsToBeRescued = (int16_t)pPacket->field_4;
        goto record_state;
    }
    if (nQuestId == 33)
    {
        CUNIT_ForEachMonsterUnit(QUESTLOG_QuestMakeFrozenAnyaUntargetable);
        return;
    }

    pQuestRecord = NPCMENU_QuestGetQuestRecord();
    if ((!pQuestRecord || !D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, 1, 0)) && !D2COMMON_QUESTRECORD_GetQuestState(pQuestRecord, 1, 1))
    {
        nQuestArg = (int16_t)pPacket->field_4;
        gnMonstersRemaining = nQuestArg;
        if (nQuestArg > 5 || nQuestArg < 0)
        {
            //  the clamp: 0x29A is stored where the count was
            gnMonstersRemaining = 666;
        }
        else if (nQuestArg == 5)
        {
            if (pPacket->nQuestId == 23)
            {
                nCountdownSeconds = pPacket->field_4;
                nCountdown = 1;
                if (nCountdownSeconds >= 1)
                {
                    nCountdown = nCountdownSeconds;
                }
                QUESTSFX_ArmGameEndDeadline(nCountdown);
                return;
            }
            goto record_state;
        }

        iQuestDef = 0;
        nDefQuestId = (UINT_PTR)&gQuestLogQuestDefs[0].nQuestId;
        while (*(int32_t *)nDefQuestId != 1)
        {
            nDefQuestId += sizeof(D2QuestLogQuestDefStrc);
            ++iQuestDef;
            if ((int32_t)nDefQuestId >= (int32_t)((UINT_PTR)&gQuestLogQuestDefs[0].nQuestId + 41 * sizeof(D2QuestLogQuestDefStrc)))
            {
                //  not found: the index goes to -1 and the store below lands one byte BEFORE the flags array. Latent.
                iQuestDef = -1;
                break;
            }
        }
        if (iQuestDef == -1)
        {
            //  the original's gQuestLogEntryFlags[-1], 0x6FBB6015 (`mov [ecx+6FBB6016h], dl`, 0x6FB06B77): the byte before the array is
            //  gQuestListPacketCopy, an object of its own, so it is named
            gQuestListPacketCopy = pPacket->nState;
        }
        else
        {
            gQuestLogEntryFlags[iQuestDef] = pPacket->nState;
        }
        return;
    }
    return;

record_state:
    if (gnQuestLogPanelState)
    {
        if (UI_GetUiWindowState(UI_QUESTSCREEN))
        {
            iQuestDef = 0;
            nQuestId = pPacket->nQuestId;
            nDefQuestId = (UINT_PTR)&gQuestLogQuestDefs[0].nQuestId;
            while (*(int32_t *)nDefQuestId != nQuestId)
            {
                nDefQuestId += sizeof(D2QuestLogQuestDefStrc);
                ++iQuestDef;
                if ((int32_t)nDefQuestId >= (int32_t)((UINT_PTR)&gQuestLogQuestDefs[0].nQuestId + 41 * sizeof(D2QuestLogQuestDefStrc)))
                {
                    return;
                }
            }
            //  iQuestDef cannot be -1 here, and the bytes keep the test
            if (iQuestDef != -1)
            {
                if (pPacket->nState >= 0x0E)
                {
                    FOG_Trace("Bad ptCmd->bNewStatus = %d for %d alert %d\n", pPacket->nState, nQuestId, pPacket->nFlags);
                }
                gQuestLogEntryFlags[iQuestDef] = pPacket->nState;
                if (!gbQuestScreenSpeechPlaying)
                {
                    pQuestDef = &gQuestLogQuestDefs[iQuestDef];
                    if (pQuestDef->nAct == gnQuestLogActTab)
                    {
                        gnQuestLogSelectedQuest = pQuestDef->nSlotInPage;
                    }
                }
            }
        }
    }
    else if (UI_SetUIToggle(UI_QUESTLOG, UI_TURNON, 0) == 1)
    {
        iQuestDef = 0;
        nDefQuestId = (UINT_PTR)&gQuestLogQuestDefs[0].nQuestId;
        while (*(int32_t *)nDefQuestId != pPacket->nQuestId)
        {
            nDefQuestId += sizeof(D2QuestLogQuestDefStrc);
            ++iQuestDef;
            if ((int32_t)nDefQuestId >= (int32_t)((UINT_PTR)&gQuestLogQuestDefs[0].nQuestId + 41 * sizeof(D2QuestLogQuestDefStrc)))
            {
                gnQuestLogPanelState = 1;
                return;
            }
        }
        //  iQuestDef cannot be -1 here either
        if (iQuestDef != -1)
        {
            gnQuestLogSelectedPerAct[gQuestLogQuestDefs[iQuestDef].nAct] = gQuestLogQuestDefs[iQuestDef].nSlotInPage;
        }
        gnQuestLogPanelState = 1;
    }
}


//1.10f: D2Client.0x6FB06CE0
//
//  Quest 4, the Search for Cain: every cain1 in the client's monster table loses UNITFLAG_TARGETABLE, so the caged Deckard Cain can no longer
//  be clicked once the quest's alert arrives. Called as the ForEachMonsterUnit callback of QUESTLOG_QuestOnQuestStatePacket's flag-1 case 4.
//
//      test ecx, ecx ; jz
//      cmp  dword ptr [ecx+4], 92h     ; dwClassId == 146 == MONSTER_CAIN1
//      mov  eax, [ecx+0C4h]            ; dwFlags
//      and  al, 0FDh                   ; clear bit 1
//      mov  [ecx+0C4h], eax
//
//  +4 and +0xC4 are dwClassId and dwFlags -- the same two offsets QUESTLOG_QuestMakeFrozenAnyaUntargetable below touches -- and the callback
//  contract is `void (__fastcall *)(D2UnitStrc *)`, the unit in ecx.
//  146 is cain1 by hcIdx; by line it would read cain2 (monstats.txt carries its Expansion marker at row 410). The two candidates differ by the
//  marker AND by intent: quest 4 is 'The Search for Cain' (tbl 3717), and cain1 is the Tristram Cain the quest is about.
void __fastcall QUESTLOG_QuestMakeCainUntargetable(D2UnitStrc *pUnit)
{
    if (pUnit)
    {
        if (pUnit->dwClassId == MONSTER_CAIN1)
        {
            uint32_t dwFlags = pUnit->dwFlags;
            //  `and al, 0FDh` -- only the low byte is masked and the whole
            //  dword is written back, which is `&= ~UNITFLAG_TARGETABLE`.
            dwFlags &= ~(uint32_t)UNITFLAG_TARGETABLE;
            pUnit->dwFlags = dwFlags;
        }
    }
}


//1.10f: D2Client.0x6FB06D00
//
//  Quest 33, Prison of Ice: `drehyaiced` -- Anya frozen in the block of ice in the Frozen River -- loses UNITFLAG_TARGETABLE. The same shape as
//  QUESTLOG_QuestMakeCainUntargetable above with `dwClassId == 527` where that one tests 146, and called the same way from the flag-0x20 case
//  33 of QUESTLOG_QuestOnQuestStatePacket.
//  527 is `drehyaiced` by hcIdx: monstats.txt carries its Expansion marker at row 410, so by line 527 reads `nihlathakboss`.
//  Bit 1 of dwFlags -- UNITFLAG_TARGETABLE -- is whether the unit can be selected at all.
void __fastcall QUESTLOG_QuestMakeFrozenAnyaUntargetable(D2UnitStrc *pUnit)
{
    if (pUnit)
    {
        if (pUnit->dwClassId == MONSTER_DREHYAICED)
        {
            uint32_t dwFlags = pUnit->dwFlags;
            //  `and al, 0FDh` -- only the low byte is masked and the whole
            //  dword is written back, which is `&= ~UNITFLAG_TARGETABLE`.
            dwFlags &= ~(uint32_t)UNITFLAG_TARGETABLE;
            pUnit->dwFlags = dwFlags;
        }
    }
}

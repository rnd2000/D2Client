#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "CoreArchive.h"
#include "ui.h"
#include "waypoint.h"

//  waypoint.cpp -- the waypoint panel of D2Client.dll 1.10: allocating and tearing down the panel, drawing it, building the nine rows of the
//  act on show, and the mouse and key handlers in gWaypointMessageHandlers (five table slots over three functions, because the left and right
//  buttons share a handler each). Address range 0x6FB25BC0..0x6FB269E2. No function in this unit asserts, so the original's source file name
//  is not known and the file name is ours; the ten functions are all WAYPOINT_*.
//  Panel space: the handlers convert the cursor into panel space (x - gnBorderXDifference, y + gnBorderYDifference; gnBorderYDifference is
//  carried negative, so the Y conversion adds it) and test there.
//  Tables: gWaypointCoords has six int32 per record (24 bytes: nX, nY, nTextX, nTextY, nClickX, nClickY); the draw pass reads the first two
//  pairs of a record, the mouse handlers the click pair. gWaypointList is D2WaypointTableStrc, 5 bytes packed { int32 nLevelId; bool bActivated }.
//  The original walks the activation bytes with a cursor that starts at &gWaypointList[0].bActivated, four bytes into record zero, steps five
//  and ends on an end address four bytes past the list: nine entries, the nine waypoints of an act.

//  Layout checks for the numbers this file relies on.
static_assert(offsetof(D2WaypointTableStrc, bActivated) == 4,
              "the activation cursor starts FOUR BYTES into record zero");
static_assert(sizeof(D2WaypointTableStrc) == 5,
              "and steps FIVE -- which is what makes the 0x6FBBA9FD "
              "end-address sentinel exactly nine entries");
static_assert(sizeof(D2WaypointDataStrc) == 16,
              "WAYPOINTS_AllocWaypointData's allocation, and the shape of "
              "packet 0x63's pData that gets copied into it");
static_assert(offsetof(SMSGHANDLER_PARAMS, nXpos) == 0x0C
                  && offsetof(SMSGHANDLER_PARAMS, nYpos) == 0x0E,
              "the mouse position every handler here hit-tests; nYpos is "
              "the HIGH half of the same dword as nXpos");
static_assert(offsetof(SMSGHANDLER_PARAMS, bReturn) == 0x18
                  && offsetof(SMSGHANDLER_PARAMS, lResult) == 0x1C,
              "what a handler answers with -- it returns void and Storm "
              "reads both of these out of the record");
static_assert(UI_WAYPOINT == 20,
              "the waypoint panel's id, as UI_SetUIToggle is given it");
static_assert(UI_TURNON == 0 && UI_TURNOFF == 1 && UI_TOGGLE == 2,
              "UI_SetUIToggle's nUIState, and TURNON being 0 is the one that "
              "reads backwards");
static_assert(STR_IDX_3990_waypointsheader == 3990
                  && STR_IDX_3991_nowaypoints == 3991
                  && STR_IDX_4130_strUiMenu1 == 4130,
              "the three tbl indices this panel pushes; the name carries "
              "the number so a renumbered enum cannot pass");

//  The data root the three panel image paths are built on.
static const char aDataGlobal[] = "DATA\\GLOBAL";

//1.10f: D2Client.0x6FB25BC0
//  Allocates the panel's own copy of the waypoint flags (WAYPOINTS_AllocWaypointData with a NULL pool; 16 bytes, uint16 nFlags[8], the shape of
//  the pData field of server packet 0x63 that is copied into it) and clears gbWaypointCloseSent. That clear is not redundant with the one in
//  WAYPOINT_OnOpenPacket: this runs once, when the client builds its panels, and the handler's runs per session.
//  void: no caller reads what the original leaves in eax.
void __cdecl WAYPOINT_AllocPanelData(void)
{
    D2WaypointDataStrc *pData;

    pData = D2COMMON_WAYPOINTS_AllocWaypointData(NULL);
    gpWaypointPanelData = pData;
    gbWaypointCloseSent = 0;
}

//1.10f: D2Client.0x6FB25BE0
//  Tears the panel down: sends the close command, frees the panel data, and unloads and nulls the background, tabs and icons images, so that
//  the draw pass reloads them on the next open. The original inlines WAYPOINT_SendCloseOnce here: the flag clear, a player unit, and that
//  player in a room, with CUNIT_GetPlayer called twice, once to test and once to pass. Nothing here turns the UI toggle off; the callers do that.
void __cdecl WAYPOINT_ClosePanel(void)
{
    if (!gbWaypointCloseSent)
    {
        if (CUNIT_GetPlayer())
        {
            if (D2COMMON_UNITS_GetRoom(CUNIT_GetPlayer()))
            {
                MSG_SendCmdUnit(73, gnWaypointUnitGUID, 0);
                gbWaypointCloseSent = 1;
            }
        }
    }

    if (gpWaypointPanelData)
    {
        D2COMMON_WAYPOINTS_FreeWaypointData(NULL, (D2WaypointDataStrc *)gpWaypointPanelData);
    }
    gpWaypointPanelData = NULL;

    if (gpWaypointBackgroundImage)
    {
        ARCHIVE_CellFileFree(gpWaypointBackgroundImage);
        gpWaypointBackgroundImage = 0;
    }
    if (gpWaypointTabsImage)
    {
        ARCHIVE_CellFileFree(gpWaypointTabsImage);
        gpWaypointTabsImage = 0;
    }
    if (gpWaypointIconsImage)
    {
        ARCHIVE_CellFileFree(gpWaypointIconsImage);
        gpWaypointIconsImage = 0;
    }
}

//1.10f: D2Client.0x6FB25C70
//  Draws the whole panel every frame, 1486 bytes: the images on first use, then the background, the tabs, the rows, the cancel button, the
//  hover caption and the header line. The original inlines WAYPOINT_SendCloseOnce in the death arm.
//  Images: gpWaypointBackgroundImage ui\menu\waygatebackground; gpWaypointTabsImage ui\menu\waygatetabs, or expwaygatetabs in an expansion
//  game (FOG_IsExpansion() && GAME_IsExpansionGame(), the pair that also decides the tab count); gpWaypointIconsImage ui\menu\waygateicons.
//  The background is four cels (frames 0..3) in a 2 x 2 grid at x = 0 and 256, y = height - 224 and height - 48; only the Y coordinates
//  carry gnScreenHeight.
//  Tabs: the five X positions are an array (expansion 5, 67, 129, 191, 253 with five tabs and nTabYAdjust 1; classic 3, 81, 159, 237 with
//  four tabs and nTabYAdjust 0). The classic arm never writes the fifth slot and the loop bound never reads it. A tab's cel frame is 2 * i
//  when selected and 2 * i + 1 otherwise. Tabs 1..4 are skipped unless their act is unlocked: quests 7, 15 and 23 for acts 2..4, and quest
//  26 for act 5, which also requires the expansion; the quest is read before the expansion test.
//  Rows: the coordinate cursor starts at the head of gWaypointCoords and steps one 24-byte record; this pass reads the icon (nX, nY) and
//  the label (nTextX, nTextY). An activated destination draws an icon, cel 0 or 3 with +1 for the hover, and its label in colour 0; an
//  unactivated one draws no icon and its label in colour 5. Hover recolours the label to 3, and so does the row the player is standing in,
//  which additionally draws cel 0 over whatever it just drew.
//  Header line: tbl 3990 when gnWaypointOtherActivatedCount is non-zero, tbl 3991 ("No Other Waypoints Activated") when it is zero. The
//  counter leaves out the row the player stands in, so standing on the only activated waypoint of an act prints the second string.
//  D2ExpRes patches nine sites in this function; each is marked on the line its detour lands on. A mod that replaces this function
//  wholesale disables all nine.
void __cdecl WAYPOINT_DrawPanel(void)
{
    D2GfxDataStrc pData;
    char szPath[260];
    int32_t nTabYAdjust;
    int32_t nMouseXRel;
    int32_t nMouseYRel;
    int32_t nTabCount;
    int32_t nTabX[5];
    int32_t i;
    int32_t nTabFrame;
    int32_t nQuest;
    BOOL bQuestState;
    int32_t nLevelIdFromRoom;
    int32_t iWaypoint;
    D2WaypointTableStrc *pWaypoint;
    const D2WaypointCoordTableStrc *pRowLayout;
    int32_t nTextColor;
    const Unicode *pwszText;
    int32_t nTextWidth;
    int32_t nTextY;

    if (!gpWaypointBackgroundImage)
    {
        sprintf(szPath, "%s\\ui\\menu\\waygatebackground", aDataGlobal);
        gpWaypointBackgroundImage = ARCHIVE_CellFileLoad(szPath, 0);
    }
    if (!gpWaypointTabsImage)
    {
        if (FOG_IsExpansion() && GAME_IsExpansionGame())
        {
            sprintf(szPath, "%s\\ui\\menu\\expwaygatetabs", aDataGlobal);
        }
        else
        {
            sprintf(szPath, "%s\\ui\\menu\\waygatetabs", aDataGlobal);
        }
        gpWaypointTabsImage = ARCHIVE_CellFileLoad(szPath, 0);
    }
    if (!gpWaypointIconsImage)
    {
        sprintf(szPath, "%s\\ui\\menu\\waygateicons", aDataGlobal);
        gpWaypointIconsImage = ARCHIVE_CellFileLoad(szPath, 0);
    }

    //  dying with the panel up closes it, and the close command still goes out (the inlined WAYPOINT_SendCloseOnce)
    if (CUNIT_IsPlayerDeadOrMissing())
    {
        UI_SetUIToggle(UI_WAYPOINT, UI_TURNOFF, 0);
        if (!gbWaypointCloseSent)
        {
            if (CUNIT_GetPlayer())
            {
                if (D2COMMON_UNITS_GetRoom(CUNIT_GetPlayer()))
                {
                    MSG_SendCmdUnit(73, gnWaypointUnitGUID, 0);
                    gbWaypointCloseSent = 1;
                }
            }
        }
    }

    nTabYAdjust = 1;
    D2WIN_10127_SetFont(D2FONT_FONT16);

    memset(&pData, 0, sizeof(pData));
    pData.pCellFile = gpWaypointBackgroundImage;
    pData.nDirection = 0;
    pData.nFrame = 0;
    D2GFX_TEXTURE_CelDraw(&pData, gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
    pData.nFrame = 1;
    D2GFX_TEXTURE_CelDraw(&pData, gnBorderXDifference + 256, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
    pData.nFrame = 2;
    D2GFX_TEXTURE_CelDraw(&pData, gnBorderXDifference, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
    pData.nFrame = 3;
    D2GFX_TEXTURE_CelDraw(&pData, gnBorderXDifference + 256, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);

    //  panel space: X subtracts the border and Y adds it, because gnBorderYDifference is carried negative
    nMouseXRel = CURSOR_GetMouseX() - gnBorderXDifference;
    nMouseYRel = gnBorderYDifference + CURSOR_GetMouseY();

    pData.pCellFile = gpWaypointTabsImage;
    nTabCount = 5;
    if (FOG_IsExpansion() && GAME_IsExpansionGame())
    {
        //  D2ExpRes WaypointsActsignsX lands here.
        nTabX[0] = 5;
        nTabX[1] = 67;
        nTabX[2] = 129;
        nTabX[3] = 191;
        nTabX[4] = 253;
    }
    else
    {
        nTabYAdjust = 0;
        nTabCount = 4;
        nTabX[0] = 3;
        nTabX[1] = 81;
        nTabX[2] = 159;
        nTabX[3] = 237;
        //  nTabX[4] is left unwritten: the classic arm does not write it and the loop bound never reads it
    }

    for (i = 0; i < nTabCount; ++i)
    {
        if (i == gnWaypointActTab)
        {
            nTabFrame = 2 * i;
        }
        else
        {
            //  acts 2..5 need their unlock quest; act 1 always shows
            if ((uint32_t)(i - 1) <= 3)
            {
                switch (i)
                {
                case 1:
                    nQuest = 7;
                    goto QUEST_GATE;
                case 2:
                    nQuest = 15;
                    goto QUEST_GATE;
                case 3:
                    nQuest = 23;
QUEST_GATE:
                    if (!D2COMMON_QUESTRECORD_GetQuestState(NPCMENU_QuestGetQuestRecord(), nQuest, 0))
                    {
                        continue;
                    }
                    break;
                case 4:
                    //  the quest is read first and the expansion tested after
                    bQuestState = D2COMMON_QUESTRECORD_GetQuestState(NPCMENU_QuestGetQuestRecord(), 26, 0);
                    if (FOG_IsExpansion() && GAME_IsExpansionGame() && bQuestState)
                    {
                        break;
                    }
                    continue;
                }
            }
            nTabFrame = 2 * i + 1;
        }
        pData.nFrame = nTabFrame;
        //  D2ExpRes WaypointsActsignsY lands on the y.
        D2GFX_TEXTURE_CelDraw(&pData, gnBorderXDifference + nTabX[i], nTabYAdjust - gnBorderYDifference + 33, 0xFFFFFFFF, 5, NULL);
    }

    //  the cancel button shares the buy/sell button strip; cel 10 up, 11 down
    pData.pCellFile = UI_GetBuySellButtonImage();
    pData.nFrame = (gbWaypointCancelDown != 0) + 10;
    //  D2ExpRes WaypointsCancelButton lands on the x and y.
    D2GFX_TEXTURE_CelDraw(&pData, gnBorderXDifference + 273, 417 - gnBorderYDifference, 0xFFFFFFFF, 5, NULL);

    nLevelIdFromRoom = D2COMMON_DUNGEON_GetLevelIdFromRoom(D2COMMON_UNITS_GetRoom(CUNIT_GetPlayer()));
    pData.pCellFile = gpWaypointIconsImage;

    if (gnWaypointRowCount > 0)
    {
        iWaypoint = 0;
        pWaypoint = gWaypointList;
        //  the head of the row table, stepped a record at a time
        pRowLayout = &gWaypointCoords[0];
        do
        {
            if (pWaypoint->bActivated)
            {
                if (pWaypoint->nLevelId == nLevelIdFromRoom)
                {
                    pData.nFrame = (gnWaypointHoverIndex == iWaypoint);
                }
                else
                {
                    pData.nFrame = (gnWaypointHoverIndex == iWaypoint) + 3;
                }
                D2GFX_TEXTURE_CelDraw(&pData, gnBorderXDifference + pRowLayout->nX, pRowLayout->nY - gnBorderYDifference, 0xFFFFFFFF, 5, NULL);
                nTextColor = 0;
            }
            else
            {
                nTextColor = 5;
            }

            if (!gbWaypointCancelDown && gnWaypointHoverIndex != -1 && iWaypoint == gnWaypointHoverIndex)
            {
                nTextColor = 3;
            }

            //  the row the player is standing in: cel 0 on top, colour 3
            if (pWaypoint->nLevelId == nLevelIdFromRoom)
            {
                nTextColor = 3;
                pData.nFrame = 0;
                D2GFX_TEXTURE_CelDraw(&pData, gnBorderXDifference + pRowLayout->nX, pRowLayout->nY - gnBorderYDifference, 0xFFFFFFFF, 5, NULL);
            }

            D2WIN_10117_DrawText(UI_LevelsGetLevelName(pWaypoint->nLevelId), gnBorderXDifference + pRowLayout->nTextX, pRowLayout->nTextY - gnBorderYDifference, nTextColor, 0);

            ++iWaypoint;
            ++pRowLayout;
            ++pWaypoint;
        }
        while (iWaypoint < gnWaypointRowCount);
    }

    //  the cancel caption, drawn only while the cursor is over the button
    if (nMouseXRel >= 273 && nMouseXRel <= 308 && nMouseYRel >= 387 && nMouseYRel <= 420)
    {
        //  D2ExpRes WaypointsCancelButtonMouseHoverOver lands here.
        pwszText = D2LANG_GetStringFromTblIndex(STR_IDX_4130_strUiMenu1);
        nTextWidth = D2WIN_10121_GetTextWidth(pwszText) / 2;
        //  D2ExpRes WaypointsCancelStringBackground lands on this box.
        D2GFX_DrawBox(gnBorderXDifference - nTextWidth + 287, 370 - gnBorderYDifference, nTextWidth + gnBorderXDifference + 294, 387 - gnBorderYDifference, 0, 2);
        //  D2ExpRes WaypointsCancelString lands on this text.
        D2WIN_10117_DrawText(pwszText, gnBorderXDifference - nTextWidth + 292, 385 - gnBorderYDifference, 0, 0);
    }

    //  Both arms carry their own GetStringFromTblIndex, GetTextWidth and Y, and only the halving and the draw are shared. That duplication is
    //  why D2ExpRes has two entries for one line, WaypointsStringY1 and WaypointsStringY2; hoisting the pair out would leave the plugin one
    //  patch site where it expects two.
    if (gnWaypointOtherActivatedCount)
    {
        pwszText = D2LANG_GetStringFromTblIndex(STR_IDX_3990_waypointsheader);
        nTextWidth = D2WIN_10121_GetTextWidth(pwszText);
        //  D2ExpRes WaypointsStringY1 lands on this y.
        nTextY = 48 - gnBorderYDifference;
    }
    else
    {
        pwszText = D2LANG_GetStringFromTblIndex(STR_IDX_3991_nowaypoints);
        nTextWidth = D2WIN_10121_GetTextWidth(pwszText);
        //  D2ExpRes WaypointsStringY2 lands on this y.
        nTextY = 48 - gnBorderYDifference;
    }
    //  D2ExpRes WaypointsStringX lands on this x.
    D2WIN_10117_DrawText(pwszText, gnBorderXDifference - nTextWidth / 2 + 160, nTextY, 0, 0);
}

//1.10f: D2Client.0x6FB26250
//  Any key closes the panel except TAB (WM_CHAR with wParam 9) and ALT+F4 (WM_SYSCHAR with wParam 0x73), which are passed on. Anything else
//  sends the close command and turns the panel off. This is the fifth slot of gWaypointMessageHandlers and has no code caller.
//  The close command is sent at most once: gbWaypointCloseSent guards it, and all three handlers in this file raise the flag from the same
//  three-step guard (the flag clear, a player unit, that player in a room); CUNIT_GetPlayer is called twice, once to test and once to pass.
//  Command 0x49 goes out through MSG_SendCmdUnit(0x49, gnWaypointUnitGUID, 0); the second argument is the waypoint object's GUID (client
//  packet 0x49 is { nHeader, nWaypointGUID, nLevelId }), not a unit type. The original's mov cl, 49h leaves the upper 24 bits of ecx as the
//  last call left them; MSG_SendCmdUnit stores only cl into a byte of the packet, so nothing reads them.
void __stdcall WAYPOINT_OnChar(SMSGHANDLER_PARAMS *pMsg)
{
    uint32_t uMessage;

    uMessage = pMsg->uMessage;
    if ((uMessage == 258 && pMsg->wParam == 9) || (uMessage == 260 && pMsg->wParam == 115))
    {
        return;
    }

    if (!gbWaypointCloseSent && CUNIT_GetPlayer())
    {
        if (D2COMMON_UNITS_GetRoom(CUNIT_GetPlayer()))
        {
            MSG_SendCmdUnit(73, gnWaypointUnitGUID, 0);
            gbWaypointCloseSent = 1;
        }
    }

    UI_SetUIToggle(UI_WAYPOINT, UI_TURNOFF, 1);
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FB262E0
//  The close command, out of line: the only copy of it that is a function. The same seven lines are inlined into WAYPOINT_ClosePanel,
//  WAYPOINT_DrawPanel's death arm, WAYPOINT_OnChar and WAYPOINT_OnMouseUp's cancel branch.
//  void: the one caller does not read the answer, and the early return is guarded on the low byte of eax only (a partial-width write).
void __cdecl WAYPOINT_SendCloseOnce(void)
{
    if (!gbWaypointCloseSent && CUNIT_GetPlayer())
    {
        if (D2COMMON_UNITS_GetRoom(CUNIT_GetPlayer()))
        {
            MSG_SendCmdUnit(73, gnWaypointUnitGUID, 0);
            gbWaypointCloseSent = 1;
        }
    }
}

//1.10f: D2Client.0x6FB26320
//  Opens the panel from server packet 0x63, { uint8 nHeader; uint32 dwUnitId; uint16 pData[8] }: takes the GUID from +1 and hands +5 to
//  WAYPOINTS_CopyAndValidateWaypointData, whose argument is a D2WaypointDataStrc of uint16 nFlags[8]. Called from SCMD_HandleSrv63_WaypointMenu.
//  Everything is gated on UI_SetUIToggle(UI_WAYPOINT, UI_TURNON, 1) answering 1, so a panel that is already up is not reinitialised and the
//  packet is dropped. The act tab comes from the player's own room (room, level id, act) and falls back to act 0 if any step fails.
//  WAYPOINT_BuildActWaypointList is called before the copy and again after it. The first pass fills gWaypointList from the previous
//  session's flags and the second overwrites every row, but the first is not wasted: the builder also caches the act's waypoint-number range
//  in gWaypointActTable and raises that record's bHasWaypoint, and the second pass skips the scan because the first did it. The order is:
//  clamp, build, GUID, copy, clear the close flag, build again.
void __fastcall WAYPOINT_OnOpenPacket(D2GSPacketSrv63 *pPacket)
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    int32_t nLevelId;
    int32_t nAct;

    if (UI_SetUIToggle(UI_WAYPOINT, UI_TURNON, 1) != 1)
    {
        return;
    }

    GAME_ResetClickState();

    pPlayer = CUNIT_GetPlayer();
    if (pPlayer && (pRoom = D2COMMON_UNITS_GetRoom(pPlayer)) != NULL && (nLevelId = D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom)) != LEVEL_NONE)
    {
        nAct = D2COMMON_DRLG_GetActNoFromLevelId((D2C_Levels)nLevelId);
    }
    else
    {
        nAct = 0;
    }

    WAYPOINT_ClampActTab(nAct);
    WAYPOINT_BuildActWaypointList();

    gnWaypointUnitGUID = pPacket->dwUnitId;
    D2COMMON_WAYPOINTS_CopyAndValidateWaypointData((D2WaypointDataStrc *)gpWaypointPanelData, (D2WaypointDataStrc *)pPacket->pData);
    gbWaypointCloseSent = 0;

    WAYPOINT_BuildActWaypointList();
}

//1.10f: D2Client.0x6FB263A0
//  Chooses which act tab the panel may show; the answer is the global gnWaypointActTab, not a return (void). Both callers follow it with
//  WAYPOINT_BuildActWaypointList, which reads it, and this is the only writer of gnWaypointActTab.
//  The switch is real in the bytes (a jump table) but every arm falls through into the next, so one pass can walk an act-5 request all the
//  way down to 0; the goto chain below is that fall-through:
//      act 4 needs quest 28    else 3
//      act 3 needs quest 23    else 2
//      act 2 needs quest 15    else 1
//      act 1 needs quest  7    else 0
//  A player who has not finished quest 7 is clamped to act 1's tab whichever tab was asked for. The values are quest states, not unit states.
void __fastcall WAYPOINT_ClampActTab(int32_t nAct)
{
    int32_t nActClamped;

    nActClamped = nAct;
    if (nAct >= gnWaypointActCount)
    {
        nActClamped = 0;
    }

    switch (nActClamped)
    {
    case 1:
        goto ACT2_GATE;
    case 2:
        goto ACT3_GATE;
    case 3:
        goto ACT4_GATE;
    case 4:
        if (!D2COMMON_QUESTRECORD_GetQuestState(NPCMENU_QuestGetQuestRecord(), 28, 0))
        {
            nActClamped = 3;
        }
ACT4_GATE:
        if (!D2COMMON_QUESTRECORD_GetQuestState(NPCMENU_QuestGetQuestRecord(), 23, 0))
        {
            nActClamped = 2;
        }
ACT3_GATE:
        if (!D2COMMON_QUESTRECORD_GetQuestState(NPCMENU_QuestGetQuestRecord(), 15, 0))
        {
            nActClamped = 1;
        }
ACT2_GATE:
        if (!D2COMMON_QUESTRECORD_GetQuestState(NPCMENU_QuestGetQuestRecord(), 7, 0))
        {
            nActClamped = 0;
        }
        break;
    default:
        break;
    }

    gnWaypointActTab = nActClamped;
}

//1.10f: D2Client.0x6FB26440
//  Rebuilds the nine rows for the act the panel is showing.
//  The clear loop is two cursors over one packed array: it starts at &gWaypointList[0].bActivated, four bytes into row zero, writes the dword
//  behind the cursor and the byte at it, and steps five; nine rows, 0x6FBBA9CC..0x6FBBA9F9.
//  bHasWaypoint means "this act's record has been filled in", not "this act has a waypoint". The first build for an act walks every level id
//  from nFirstLevelId to nLastLevelId, asks D2Common for each one's waypoint number, and records the lowest and the highest; 0xFF is the empty
//  marker of nFirstWaypointNo, which is why the test is against 0xFF and not zero (waypoint 0 is a real waypoint).
//  bActivated does not mean "activated": a waypoint whose level is the one the player is standing in is written with bActivated = 0, down the
//  same path an unactivated one takes, and is not counted into gnWaypointOtherActivatedCount. The flag means "somewhere else you can travel
//  to"; WAYPOINT_DrawPanel still draws the player's own row with its own cel and colour.
//  BUG: the bound on the act index (cmp eax, 5 / jbe at 0x6FB26480) admits act tab 5 into a table of five records, indices 0..4. UNREACHABLE:
//  gnWaypointActTab is written only by WAYPOINT_ClampActTab, which bounds it to 0..4 against gnWaypointActCount; the bound is reproduced as written.
void __stdcall WAYPOINT_BuildActWaypointList(void)
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    int32_t nLevelIdFromRoom;
    UINT_PTR nActivated;
    int32_t nActTab;
    int32_t i;
    BOOL bHasWaypoint;
    int16_t pWaypointNo[2];
    int32_t nLevelId;
    int32_t nWaypointNo;
    int32_t nRow;

    gnWaypointRowCount = 0;
    gnWaypointOtherActivatedCount = 0;

    pPlayer = CUNIT_GetPlayer();
    pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
    nLevelIdFromRoom = D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);

    //  DIFFERENCE: the original's cursor is a pointer that runs on to 0x6FBBA9FD, four bytes past the list (&gWaypointList[9].bActivated); a
    //  pointer there would leave our array, so the cursor is an integer, bounded from the list itself, compared signed as the original compares
    //  it, and written through only inside the list.
    nActivated = (UINT_PTR)&gWaypointList[0].bActivated;
    do
    {
        *(int32_t *)(nActivated - 4) = 0;
        *(uint8_t *)nActivated = 0;
        nActivated += 5;
    }
    while ((int32_t)nActivated < (int32_t)((UINT_PTR)&gWaypointList[0].bActivated + sizeof(gWaypointList)));

    nActTab = gnWaypointActTab;
    if ((uint32_t)nActTab > 5)
    {
        gbWaypointCloseSent = 0;
        return;
    }

    if (!gWaypointActTable[nActTab].bHasWaypoint)
    {
        gWaypointActTable[nActTab].bHasWaypoint = 1;
        for (i = gWaypointActTable[nActTab].nFirstLevelId; i <= gWaypointActTable[nActTab].nLastLevelId; ++i)
        {
            bHasWaypoint = D2COMMON_WAYPOINTS_GetWaypointNoFromLevelId((D2C_Levels)i, pWaypointNo);
            //  the act tab is re-read every iteration, before the test, as the original does
            nActTab = gnWaypointActTab;
            if (bHasWaypoint)
            {
                nWaypointNo = (uint8_t)pWaypointNo[0];
                if (gWaypointActTable[nActTab].nFirstWaypointNo == 0xFF)
                {
                    gWaypointActTable[nActTab].nFirstWaypointNo = (uint8_t)nWaypointNo;
                }
                if (nWaypointNo > gWaypointActTable[nActTab].nLastWaypointNo)
                {
                    gWaypointActTable[nActTab].nLastWaypointNo = (uint8_t)nWaypointNo;
                }
            }
        }
    }

    //  both bounds are unsigned bytes widened to int
    for (nWaypointNo = gWaypointActTable[nActTab].nFirstWaypointNo; nWaypointNo <= gWaypointActTable[gnWaypointActTab].nLastWaypointNo; ++nWaypointNo)
    {
        if (D2COMMON_WAYPOINTS_IsActivated((D2WaypointDataStrc *)gpWaypointPanelData, (uint16_t)nWaypointNo))
        {
            //  the original hands this arm the waypoint-number buffer as its out-parameter and the other arm a slot of its own;
            //  both are four bytes and the scan loop is over by now
            if (D2COMMON_WAYPOINTS_GetLevelIdFromWaypointNo((int16_t)nWaypointNo, (int32_t *)pWaypointNo))
            {
                nRow = gnWaypointRowCount;
                gWaypointList[nRow].nLevelId = *(int32_t *)pWaypointNo;
                if (*(int32_t *)pWaypointNo == nLevelIdFromRoom)
                {
                    //  where the player is standing: not "somewhere else"
                    gWaypointList[nRow].bActivated = 0;
                }
                else
                {
                    gWaypointList[nRow].bActivated = 1;
                    ++gnWaypointOtherActivatedCount;
                }
                gnWaypointRowCount = nRow + 1;
            }
        }
        else if (D2COMMON_WAYPOINTS_GetLevelIdFromWaypointNo((int16_t)nWaypointNo, &nLevelId))
        {
            nRow = gnWaypointRowCount;
            gWaypointList[nRow].nLevelId = nLevelId;
            gWaypointList[nRow].bActivated = 0;
            gnWaypointRowCount = nRow + 1;
        }
    }
}

//1.10f: D2Client.0x6FB265C0
//  Releasing a mouse button over the waypoint panel. Two table slots point here, WM_LBUTTONUP and WM_RBUTTONUP, so the right button does
//  exactly what the left one does. Three regions, depending on what the press left behind:
//      the mini-panel      MINIPANEL_WaypointIsMouseOverMiniPanel() -> return at once, writing nothing and consuming nothing
//      off the panel       x > gnScreenWidth or y > gnScreenHeight - 49 -> clear the press state, consume nothing
//      cancel armed        panel x 273..308, y 387..420, inclusive -> close
//      a destination row   only if the row under the cursor is the same one the press hovered
//  The release must land on the row the press did: the row walk recomputes an index and compares it with gnWaypointHoverIndex, which
//  WAYPOINT_OnMouseDown wrote. A mismatch, including -1 (a miss), falls into the same tail as the cancel button: clear both press globals,
//  consume the message, travel nowhere.
//  The accept path consumes the message twice: bReturn = 1, lResult = 0 and SMsgBreakHandlerChain run, then the travel command goes out, then
//  the same three run again before the function returns. The doubling is in the original and is reproduced.
//  gbWaypointCancelDown gates the whole cancel arm and the row arm is its else, so a release with the cancel flag set never tests the rows,
//  even over one.
//  The entry gate rejects x > gnScreenWidth where WAYPOINT_OnMouseDown rejects x > gnScreenWidth / 2; reproduced.
//  D2ExpRes detours twice here: WaypointsCancelButtonRelease and WaypointsRelease (the row-loop coordinate fetch); rewriting the function
//  leaves them nothing to attach to.
void __stdcall WAYPOINT_OnMouseUp(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nPanelX;
    int32_t nPanelY;
    int32_t nWaypointIndex;
    int32_t nHoverIndex;
    int32_t nRectX;
    UINT_PTR nActivated;
    const D2WaypointCoordTableStrc *pRect;   // the record nWaypointIndex names, inside gWaypointCoords

    nMouseX = pMsg->nXpos;
    nMouseY = pMsg->nYpos;

    if (MINIPANEL_WaypointIsMouseOverMiniPanel())
    {
        return;
    }

    //  note: gnScreenWidth, where the press handler uses gnScreenWidth / 2
    if (nMouseX > gnScreenWidth || nMouseY > gnScreenHeight - 49)
    {
        gbWaypointCancelDown = 0;
        gnWaypointHoverIndex = -1;
        return;
    }

    nPanelX = nMouseX - gnBorderXDifference;
    nPanelY = nMouseY + gnBorderYDifference;

    if (gbWaypointCancelDown)
    {
        //  D2ExpRes WaypointsCancelButtonRelease lands on the cmp this 273 compiles to.
        if (nPanelX >= 273 && nPanelX <= 308 && nPanelY >= 387 && nPanelY <= 420)
        {
            UI_SetUIToggle(UI_WAYPOINT, UI_TURNOFF, 0);
            if (!gbWaypointCloseSent && CUNIT_GetPlayer())
            {
                if (D2COMMON_UNITS_GetRoom(CUNIT_GetPlayer()))
                {
                    MSG_SendCmdUnit(73, gnWaypointUnitGUID, 0);
                    gbWaypointCloseSent = 1;
                }
            }
        }
        goto CANCEL_TAIL;
    }

    if (gnWaypointHoverIndex == -1)
    {
        goto CANCEL_TAIL;
    }

    //  DIFFERENCE: the original's coordinate cursor starts at record 0's nClickY and its activation cursor four bytes into record 0, both stepping
    //  a record, and the walk stops when the activation cursor reaches &gWaypointList[9].bActivated, four bytes past the list. Either pointer
    //  would end past its table in ours, so the coordinates are read by the record index the walk counts, and the activation cursor is an
    //  integer, compared signed as the original compares it and read only inside the list.
    nWaypointIndex = 0;
    nActivated = (UINT_PTR)&gWaypointList[0].bActivated;
    for (;;)
    {
        if (*(const uint8_t *)nActivated)
        {
            pRect = &gWaypointCoords[nWaypointIndex];
            nRectX = pRect->nClickX;
            if (nPanelX > nRectX && nPanelX < nRectX + 280 && nPanelY > pRect->nClickY && nPanelY < pRect->nClickY + 30)
            {
                nHoverIndex = nWaypointIndex;
                break;
            }
        }
        nActivated += 5;
        ++nWaypointIndex;
        //  an end-address sentinel, not a variable
        if ((int32_t)nActivated >= (int32_t)((UINT_PTR)&gWaypointList[0].bActivated + sizeof(gWaypointList)))
        {
            nHoverIndex = -1;
            break;
        }
    }

    if (gnWaypointHoverIndex != nHoverIndex)
    {
        goto CANCEL_TAIL;
    }

    //  consumed once here...
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

    MSG_SendCmdUnit(73, gnWaypointUnitGUID, gWaypointList[gnWaypointHoverIndex].nLevelId);
    gbWaypointCloseSent = 1;
    gbWaypointCancelDown = 0;
    gnWaypointHoverIndex = -1;
    UI_SetUIToggle(UI_WAYPOINT, UI_TURNOFF, 0);
    SOUNDINST_PlaySound(SOUND_PLAYER_TOWNPORTAL_ENTER, NULL, 0, 0, 0);

    //  ...and consumed again here
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
    return;

CANCEL_TAIL:
    gbWaypointCancelDown = 0;
    gnWaypointHoverIndex = -1;
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FB267A0
//  Pressing a mouse button over the waypoint panel. Two table slots, WM_LBUTTONDOWN and WM_RBUTTONDOWN.
//  A click outside the panel closes it and is not consumed: when the cursor is right of gnScreenWidth / 2 or below gnScreenHeight - 49, the
//  function sends the close command and ends in UI_SetUIToggle(UI_WAYPOINT, UI_TURNOFF, 1) (a tail jump in the original; an ordinary call in
//  the last statement here) without touching bReturn, so the click also falls through to whatever is behind the panel.
//  The act tabs are five or four, and the strip is 320 wide either way: FOG_IsExpansion() && GAME_IsExpansionGame() chooses expansion
//  (boundaries 64, 128, 192, 256, five tabs of 64) or classic (boundaries 80, 160, 240, four tabs of 80). 5 x 64 and 4 x 80 are both 320,
//  the nPanelX > 320 that rejects the strip, and the strip is nPanelY <= 30 tall in both.
//  Handling a tab does not stop the function. After a tab click has played its sound, clamped the act, rebuilt the list and consumed the
//  message, control falls through into the destination-row test, the same code a click below the strip reaches by jump. So a tab click also
//  writes gnWaypointHoverIndex = -1, runs the cancel-button and row hit tests against tab-strip coordinates (which match nothing, the rows
//  being below y 30), and reaches the tail and calls SMsgBreakHandlerChain a second time. There is no jump past it in the original.
//  The tail re-reads pMsg->nXpos and nYpos and compares them with gnScreenWidth and gnScreenHeight - 49. The entry gate already required
//  nXpos <= gnScreenWidth / 2, so the first of the two can never fail: a dead test with a different constant from the live one.
//  D2ExpRes detours twice here too: WaypointsClickdown and WaypointsCancelButtonClickdown, the press-side mirror of the two in the release handler.
void __stdcall WAYPOINT_OnMouseDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nPanelX;
    int32_t nPanelY;
    int32_t nActTab;
    int32_t nWaypointIndex;
    int32_t nHoverIndex;
    int32_t nRectX;
    UINT_PTR nActivated;
    const D2WaypointCoordTableStrc *pRect;   // the record nWaypointIndex names, inside gWaypointCoords

    nMouseX = pMsg->nXpos;
    nMouseY = pMsg->nYpos;

    if (MINIPANEL_WaypointIsMouseOverMiniPanel())
    {
        return;
    }

    //  note: gnScreenWidth / 2, where the release handler uses gnScreenWidth
    if (nMouseX > gnScreenWidth / 2 || nMouseY > gnScreenHeight - 49)
    {
        //  outside the panel: close it, and do not consume the click
        if (!gbWaypointCloseSent && CUNIT_GetPlayer())
        {
            if (D2COMMON_UNITS_GetRoom(CUNIT_GetPlayer()))
            {
                MSG_SendCmdUnit(73, gnWaypointUnitGUID, 0);
                gbWaypointCloseSent = 1;
            }
        }
        UI_SetUIToggle(UI_WAYPOINT, UI_TURNOFF, 1);
        return;
    }

    nPanelY = nMouseY + gnBorderYDifference;
    nPanelX = nMouseX - gnBorderXDifference;

    if (FOG_IsExpansion() && GAME_IsExpansionGame())
    {
        if (nPanelY > 30 || nPanelX > 320)
        {
            goto AFTER_TABS;
        }
        if (nPanelX < 64)
        {
            nActTab = 0;
        }
        else if (nPanelX < 128)
        {
            nActTab = 1;
        }
        else if (nPanelX < 192)
        {
            nActTab = 2;
        }
        else
        {
            nActTab = (nPanelX >= 256) + 3;
        }
    }
    else
    {
        if (nPanelY > 30 || nPanelX > 320)
        {
            goto AFTER_TABS;
        }
        if (nPanelX < 80)
        {
            nActTab = 0;
        }
        else if (nPanelX < 160)
        {
            nActTab = 1;
        }
        else
        {
            nActTab = (nPanelX >= 240) + 2;
        }
    }

    if (nActTab != gnWaypointActTab)
    {
        SOUNDINST_PlaySound(SOUND_CURSOR_SWITCH, NULL, 0, 0, 0);
    }
    WAYPOINT_ClampActTab(nActTab);
    WAYPOINT_BuildActWaypointList();
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
    //  and falls through: there is no jump past the row test

AFTER_TABS:
    gnWaypointHoverIndex = -1;

    //  D2ExpRes WaypointsCancelButtonClickdown lands on the cmp this 273 compiles to.
    if (nPanelX < 273 || nPanelX > 308 || nPanelY < 387 || nPanelY > 420)
    {
        //  the same walk as WAYPOINT_OnMouseUp's, read the same way (by record index)
        nWaypointIndex = 0;
        nActivated = (UINT_PTR)&gWaypointList[0].bActivated;
        for (;;)
        {
            if (*(const uint8_t *)nActivated)
            {
                pRect = &gWaypointCoords[nWaypointIndex];
                nRectX = pRect->nClickX;
                if (nPanelX > nRectX && nPanelX < nRectX + 280 && nPanelY > pRect->nClickY && nPanelY < pRect->nClickY + 30)
                {
                    nHoverIndex = nWaypointIndex;
                    break;
                }
            }
            nActivated += 5;
            ++nWaypointIndex;
            if ((int32_t)nActivated >= (int32_t)((UINT_PTR)&gWaypointList[0].bActivated + sizeof(gWaypointList)))
            {
                nHoverIndex = -1;
                break;
            }
        }

        gnWaypointHoverIndex = nHoverIndex;
        if (nHoverIndex == -1)
        {
            goto TAIL;
        }
    }
    else
    {
        gbWaypointCancelDown = 1;
    }

    SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);

TAIL:
    //  the message is re-read, and this bound can never fail
    if (pMsg->nXpos <= gnScreenWidth && pMsg->nYpos <= gnScreenHeight - 49)
    {
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
    }
}

#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "CoreArchive.h"
#include "ui.h"
#include "inv.h"

//  ui_panels.cpp -- the in-game UI draw pass of D2Client.dll 1.10: UIPANELS_DrawInGamePanels, which draws the panels, hints, labels and overlays
//  over the world once a frame, and the helpers it uses (the floating name labels, the one-shot text, the charge-up strip, the death message,
//  the tiled frame). The original source file is UI\ui.cpp, the same file ui.cpp is written from; the split into two files is ours.
//  Address range of the functions: 0x6FB20500 (UIPANELS_TextStripColorCodes) to 0x6FB22560 (UIPANELS_BuildOtherPlayerLabel).

//  The language data root; ui.cpp keeps its own copy of the literal.
static const char szDataLocal[] = "DATA\\LOCAL";


//  The quest-specific level-entry title table: records of 13 bytes, two ints and a five-byte string with no padding, so the original declared it
//  packed or as a char array: { level id, quest id, art stem }.
//
//      level  8 (LEVEL_DENOFEVIL)        quest 1 (QUEST_A1Q1_DENOFEVIL)   "A1Q1"
//      level 20 (LEVEL_FORGOTTENTOWER)   quest 5 (QUEST_A1Q5_COUNTESS)    "A1Q5"
//
//  The quest ids are the ordinary 1-based ones and the art stem is the same number written out. The path is built with ACT1 hardcoded, so the
//  mechanism can only ever fire in act 1, consistent with both records being act 1. The table is D2ClientLevelEntryTitleStrc[2]; the two offsets
//  the walk leans on are asserted below.

static_assert(offsetof(D2ClientLevelEntryTitleStrc, nQuestId) == 4, "QUESTS_GetQuestState gets *pQuestEntry, the level is one int lower");
static_assert(offsetof(D2ClientLevelEntryTitleStrc, szTitle) == 8, "aA1q1 is 0x6FB8EC5C and the table head 0x6FB8EC54 -- eight bytes in");
static_assert(offsetof(D2LevelsTxt, szEntryFile) == 0x145,
              "the level's own title art is [esi+145h] at 0x6FB21E44");


//1.10f: D2Client.0x6FB21B70
//  The in-game UI draw pass: everything drawn over the world, once a frame, from GAME_DrawGameScene. Around fifty panels, hints, labels and
//  overlays in one function, and the order they appear in is the z-order.
//  The first statement after the argument is saved is `if (gUiWindowStates[UI_UNKNOWN18]) return;`, so that window state turns the whole in-game
//  interface off. SCROLL_GetUiShiftScreen() != 3 is the second gate, tested before the hireling label, the automap and the ground labels; 3 is the
//  value that suppresses (UIPANELS_DrawUnitNameLabel makes the same test for itself), and several leaves double-check it rather than trusting the
//  caller.
//  gnBorderXDifference and gnBorderYDifference are set here, once a frame, from the resolution: 80 and -60 at 800x600, zero otherwise, so every panel
//  that positions itself against them is told each frame how far the 640x480 layout has to move. The Y origin runs the other way, hence -60.
//  The level-entry title is armed when the player's room reports a level id different from gLevelId. Its frame counter is computed without a
//  branch (neg eax / sbb eax, eax / and al, 88h / add eax, 78h): 120 when the old gLevelId was 0 and 0 otherwise, and the draw is guarded by
//  < 120, so the title card is skipped on the first level of the session and shown for 120 frames on every level change after it. `and al` rather
//  than `and eax` is what makes it work: for -1 the upper three bytes stay 0xFF, so the value is -120 and +120 gives exactly 0. It is written as
//  the conditional it computes. The guard is a signed compare (cmp eax, 78h then jl at 0x6FB21DDA), so nTitleFrame is a plain int local.
//  The language code is fetched into pXAdjust, an int32_t local used as a char buffer (D2LANG_10006 writes into the caller's buffer, which is
//  passed again as a %s). BUG: the buffer is four bytes and the next two slots are pYAdjust and pState, and pState is still live (it is the view
//  pointer forwarded to TEXT_TickStaticInfoPanel at the very end). LATENT: every D2 language code is three characters (ENG, ESP, DEU, FRA, POR,
//  ITA, JPN, KOR, SIN, CHI, POL, RUS), so three plus a NUL fills the slot and writes nothing past it; a four-character code would corrupt the
//  view pointer. The local is declared the width the frame gives it.
//  pYAdjust is then reused as the out-parameter of NPCMENU_GetInteractingNpc((D2UnitGUID *)&pYAdjust), which overwrites the slot that held the
//  pet label's Y; that is safe only because gnLabelScreenY has already been stored.
//  When UI_HELPSCREEN is up, the function walks every window id and calls UI_SetUIToggle(i, UI_TURNOFF, 0) on each one that is open and marked
//  closeable, resetting the item-description state for the stash and the inventory as it goes, then calls UI_SetUIToggle(UI_HELPSCREEN, UI_TOGGLE,
//  0) and draws. So the close-everything path lives in the draw function rather than in an input handler.
//  The arena kill count is the only arena-mode draw: a twelve-element Unicode buffer filled from a %d and drawn at a fixed (600, 25), unscaled by
//  resolution; it is the one thing here that does not consult gnScreenWidth.
void __fastcall UIPANELS_DrawInGamePanels(D2ClientViewStrc *pView)
{
    D2ClientViewStrc *pState;
    D2UnitStrc *pSelected;
    D2UnitStrc *pPlayer;
    D2UnitStrc *pPet;
    D2RosterUnitStrc *pRosterEntry;
    D2ActiveRoomStrc *pPlayerRoom;
    D2LevelsTxt *pLevelsTxt;
    const D2ClientLevelEntryTitleStrc *pTitles;
    D2CellFileStrc *pTitleImage;
    D2C_Levels eRoomLevelId;
    D2C_UIvars i;
    uint32_t dwPetGUID;
    int32_t nTitleFrame;
    int32_t nPrecisionX, nPrecisionY;
    int32_t nQuarterScreen;
    int32_t nLifeFill, nLabelY, nColor, nTextY, nKillCount, iQuest, iChar;
    BOOL bHavePetLabelPos;
    int32_t pXAdjust;
    int32_t pYAdjust;
    CHAR szKillCount[12];
    char szPath[260];

    pState = pView;

    if (gUiWindowStates[UI_UNKNOWN18])
        return;

    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600)
    {
        gnBorderXDifference = 80;
        gnBorderYDifference = -60;
    }
    else
    {
        gnBorderXDifference = 0;
        gnBorderYDifference = 0;
    }

    D2WIN_10129_DrawFramedText(NULL, 0, 0, 0, 0);
    gwszDrawTextScratch->ch = 0;
    gwszUnitLabelLine2[0].ch = 0;

    if (gUiWindowStates[UI_ESCMENU])
        ESCMENU_Draw();
    if (gUiWindowStates[UI_CONFIG])
        CONFIG_DrawKeyBindingsPanel();

    pSelected = SELECT_GetSelectedUnit();
    UIPANELS_DrawUnitNameLabel(pSelected);

    //  The hireling / pet name label. Four levels of guard before it draws anything, and the perspective and flat paths compute the label
    //  position differently.
    if (gUiWindowStates[UI_HIREICONS] && SCROLL_GetUiShiftScreen() != 3)
    {
        pPlayer = CUNIT_GetPlayer();
        dwPetGUID = (uint32_t)ROSTERPETS_GetAssociatedUnitGUID(pPlayer, 7, 0);

        if (dwPetGUID != (uint32_t)-1 && ROSTERPETS_GetPetInfo(dwPetGUID)->nDisplayState == 1)
        {
            pPet = CUNIT_FindUnitByGuidAndType(dwPetGUID, UNIT_MONSTER);
            if (pPet)
            {
                bHavePetLabelPos = TRUE;

                if (!D2GFX_CheckPerspective())
                {
                    gnLabelScreenX = D2COMMON_UNITS_GetClientCoordX(pPet)
                                             - SCROLL_GetViewportOriginX();
                    gnLabelScreenY = D2COMMON_UNITS_GetClientCoordY(pPet)
                                             - SCROLL_GetViewportOriginY();
                }
                else
                {
                    nPrecisionX = D2COMMON_UNITS_GetPrecisionX(pPet);
                    nPrecisionY = D2COMMON_UNITS_GetPrecisionY(pPet);

                    if (!D2GFX_SCALE_CheckPerspectivePosition(nPrecisionX, nPrecisionY))
                    {
                        bHavePetLabelPos = FALSE;
                    }
                    else
                    {
                        D2GFX_SCALE_AdjustPerspectivePosition(nPrecisionX, nPrecisionY, 0, &pXAdjust, &pYAdjust);

                        //  The UI shift moves the label a quarter screen the way the panels moved: 1 pushes left, 2 right, and anything else
                        //  leaves it alone.
                        if (SCROLL_GetUiShiftScreen() - 1 == 0)
                        {
                            nQuarterScreen = gnScreenWidth / -4;
                            pXAdjust += nQuarterScreen;
                        }
                        else if (SCROLL_GetUiShiftScreen() - 1 == 1)
                        {
                            nQuarterScreen = gnScreenWidth / 4;
                            pXAdjust += nQuarterScreen;
                        }

                        gnLabelScreenX = pXAdjust;
                        gnLabelScreenY = pYAdjust + 16;
                    }
                }

                if (bHavePetLabelPos && !NPCMENU_GetInteractingNpc((D2UnitGUID *)&pYAdjust))
                {
                    nLifeFill = UIPANELS_GetUnitLifeFill128(pPet);
                    nLabelY = gnLabelScreenY - 72;
                    UIPANELS_SetTransientText(CUNIT_GetUnitName(pPet), gnLabelScreenX, nLabelY, nLifeFill);
                }
            }
        }
    }

    //  The warning icons are suppressed while any of the three big panels is open, because they share the same corner.
    if (!gUiWindowStates[UI_INVENTORY] && !gUiWindowStates[UI_NPCSHOP] && !gUiWindowStates[UI_ANVIL])
    {
        PANEL_DrawStatusWarningIcons();
        PANEL_DrawQuantityWarningIcons();
    }

    if (gUiWindowStates[UI_AUTOMAP] && SCROLL_GetUiShiftScreen() != 3)
    {
        AUTOMAP_Draw();
    }

    pPlayerRoom = CUNIT_GetPlayerRoom();
    if (pPlayerRoom)
    {
        eRoomLevelId = (D2C_Levels)D2COMMON_DUNGEON_GetLevelIdFromRoom(pPlayerRoom);

        if (eRoomLevelId == (D2C_Levels)gLevelId)
        {
            pTitleImage = gpImgLevelEntryTitle;
            nTitleFrame = gLevelEntryFrameCount;
        }
        else
        {
            HIREABLES_PanelInitLifeBarColors(1);

            pTitleImage = gpImgLevelEntryTitle;
            if (pTitleImage)
            {
                ARCHIVE_CellFileFree(pTitleImage);
                pTitleImage = NULL;
                gpImgLevelEntryTitle = NULL;
            }

            //  neg/sbb/and al/add: 120 when the OLD level id was zero and 0 otherwise, so the very first level of a session gets no title card.
            nTitleFrame = (gLevelId != 0) ? 0 : 120;
            gLevelId = eRoomLevelId;
            gLevelEntryFrameCount = nTitleFrame;
        }

        if (nTitleFrame < 120)
        {
            //  The char half of the 32-byte slot at [esp+10h] (the "ACT%d" stem). The kill-count half is a Unicode[12] further down and the two never
            //  overlap in time. They are two declarations because the Unicode one is constructed by its declaration.
            char Buffer[32];

            gLevelEntryFrameCount = nTitleFrame + 1;
            pLevelsTxt = D2COMMON_DATATBLS_GetLevelsTxtRecord(eRoomLevelId);

            if (eRoomLevelId < LEVEL_HARROGATH)
            {
                sprintf(Buffer, "ACT%d", D2COMMON_DRLG_GetActNoFromLevelId(eRoomLevelId) + 1);
            }
            else
            {
                //  A hand-rolled strcpy of the expansion stem, byte by byte including the terminator; the loop is what the bytes do.
                iChar = 0;
                do
                {
                    Buffer[iChar] = gszExpansion[iChar];
                }
                while (gszExpansion[iChar++]);
            }

            //  The four-byte language buffer: a language code is three characters plus the NUL.
            D2LANG_10006((uint32_t)&pXAdjust, 0);
            sprintf(szPath, "%s\\UI\\%s\\%s\\%s", szDataLocal, (const char *)&pXAdjust, Buffer, pLevelsTxt->szEntryFile);

            //  ...unless a quest owns this level's title art, in which case the ACT1 tree wins. Two records, both act 1.
            iQuest = 0;
            if (gnLevelEntryQuestTitleCount > 0)
            {
                pTitles = gLevelEntryQuestTitles;
                while (eRoomLevelId != pTitles[iQuest].nLevelId || !QUESTS_GetQuestState(0, pTitles[iQuest].nQuestId))
                {
                    if (++iQuest >= gnLevelEntryQuestTitleCount)
                        goto no_quest_title;
                }
                sprintf(szPath, "%s\\UI\\%s\\ACT1\\%s", szDataLocal, (const char *)&pXAdjust, pTitles[iQuest].szTitle);
            }
no_quest_title:
            if (!gpImgLevelEntryTitle)
                gpImgLevelEntryTitle = ARCHIVE_CellFileLoad(szPath, 0);

            D2WIN_10134_DrawCellFile(gpImgLevelEntryTitle, gnScreenWidth / 2, gnScreenHeight / 2 - 140, 1, (DrawMode)5, 1);

            goto after_title;
        }
    }
    else
    {
        pTitleImage = gpImgLevelEntryTitle;
    }

    if (pTitleImage)
    {
        ARCHIVE_CellFileFree(pTitleImage);
        gpImgLevelEntryTitle = NULL;
    }

after_title:
    HIREABLES_PanelUpdateHirelingIconsVisible();

    if (gUiWindowStates[UI_HIRICONS])
        HIREABLES_PanelDrawHirelingIconHint();
    if (gUiWindowStates[UI_MERCINV])
        INV_MercInvDrawPanel();
    if (gUiWindowStates[UI_RECIPESCROLL])
        NEWSTATS_RecipeScrollDraw();
    if (gUiWindowStates[UI_QUESTSCREEN])
        QUESTLOG_Draw();

    //  Seven ids share one inventory draw, and trade gets its own call after its button hints, so with trade open the panels are drawn twice.
    if (gUiWindowStates[UI_INVENTORY] || gUiWindowStates[UI_NPCSHOP] || gUiWindowStates[UI_ANVIL] || gUiWindowStates[UI_STASH] || gUiWindowStates[UI_CUBE] || gUiWindowStates[UI_GUILDVAULT] || gUiWindowStates[UI_UNKNOWN29])
    {
        INV_UpdateAndDrawPanels();
    }

    if (gUiWindowStates[UI_MPTRADE])
    {
        TRADE_DrawButtonHints();
        INV_UpdateAndDrawPanels();
    }

    UIPANELS_DrawChargeUpIcons();

    if (gUiWindowStates[UI_SKILLTREE])
        SKILLSTREE_DrawPanel();
    if (gUiWindowStates[UI_STATSCREEN])
        CHAR_CharPanelDraw();
    if (gUiWindowStates[UI_PARTYSCREEN])
        PARTY_DrawPanel();
    if (gpGoldDialog)
        DIALOG_Draw(gpGoldDialog);
    if (gUiWindowStates[UI_INISCROLL])
        TEXT_ScrollDrawQuestScrollPanel();
    if (gUiWindowStates[UI_WAYPOINT])
        WAYPOINT_DrawPanel();

    if (gUiWindowStates[UI_HELPSCREEN])
    {
        HELP_LoadImages();
        HELP_DimGameScreen();
    }

    PANEL_Draw800BorderFrame();

    if (gUiWindowStates[UI_MINISKILL])
        SPELLSEL_DrawBothPanels();
    if (gUiWindowStates[UI_BELTROWS])
        PANEL_BeltShowPanel();

    NEWSTATS_UpdateButtonVisibility();

    if (gUiWindowStates[UI_HELPBUTTON])
        HELP_DrawHelpHint();

    //  The state picks which draw is called, lit or grey, not whether one is: the lit one hides itself under some panels outside 800x600, and the
    //  grey one draws only at 800x600.
    if (gUiWindowStates[UI_NEWSTATS])
        NEWSTATS_PanelDrawNewStatsButton();
    else
        NEWSTATS_PanelDrawNewStatsButtonGrey();

    if (gUiWindowStates[UI_NEWSKILLS])
        NEWSTATS_PanelDrawNewSkillButton();
    else
        NEWSTATS_PanelDrawNewSkillButtonGrey();

    if (gUiWindowStates[UI_QUESTLOG])
        QUESTLOG_DrawFrameAndTitle();

    PANEL_DrawOrbHoverText();

    if (gUiWindowStates[UI_MINIPANEL])
        MINIPANEL_Draw();
    if (gUiWindowStates[UI_CHATBOX])
        CHAT_DrawInputLine();

    //  The help screen closes every other closeable window, then toggles itself; this lives in the draw, not in input.
    if (gUiWindowStates[UI_HELPSCREEN])
    {
        for (i = UI_GAME; i < NUM_UI; i = (D2C_UIvars)(i + 1))
        {
            if (gUiWindowStates[i] == 1 && gbUiWindowCloseable[i] == 1)
            {
                UI_SetUIToggle(i, UI_TURNOFF, 0);
                if (i == UI_STASH || i == UI_INVENTORY)
                    INV_ResetItemDescState();
            }
        }
        UI_SetUIToggle(UI_HELPSCREEN, UI_TOGGLE, 0);
        HELP_DrawScreen();
    }

    if (gUiWindowStates[UI_HOLDALT] && SCROLL_GetUiShiftScreen() != 3)
    {
        SHOWITEMS_DrawGroundItemLabels();
        if (GAME_GetGameType())
            CUNIT_ForEachPlayerUnit(UIPANELS_BuildOtherPlayerLabel);
    }

    if (gUiWindowStates[UI_NPCMENU])
        NPCMENU_TickAndValidate();
    if (gUiWindowStates[UI_ANVIL])
        USEITEM_AnvilTickPanel();

    PANEL_DrawHostileIndicator();
    TEXT_TickStaticInfoPanel(pState);
    D2WIN_10124();

    //  The two label lines under a selected monster. Line 2 first, then line 1 above it, and line 1 is coloured by rank.
    pSelected = SELECT_GetSelectedUnit();
    if (pSelected && pSelected->dwUnitType == UNIT_MONSTER)
    {
        nTextY = D2WIN_10125() + 39;
        UIPANELS_DrawTextCenteredInPanelRect(gwszDrawTextScratch, 6, nTextY, gnUnitLabelColor);
        if (gwszDrawTextScratch->ch || gwszUnitLabelLine1[0].ch)
            nTextY += D2WIN_10125();
        UIPANELS_DrawTextCenteredInPanelRect(gwszUnitLabelLine2, 6, nTextY, gnUnitLabelLine2Color);
    }

    UIPANELS_DrawTransientText();

    if (Unicode::strlen(gwszUnitLabelLine1))
    {
        pSelected = SELECT_GetSelectedUnit();
        if (pSelected && pSelected->dwUnitType == UNIT_MONSTER)
        {
            nColor = 0;
            if (MONSTER_IsUnique(pSelected))
                nColor = 4;
            if (MONSTER_IsChampion(pSelected))
                nColor = 3;

            UIPANELS_DrawTextCenteredInPanelRect(gwszUnitLabelLine1, 6, D2WIN_10125() + 39, nColor);
        }
    }

    //  Arena mode's kill count, at a fixed (600, 25) that no resolution touches.
    if (ARENA_IsArenaMode())
    {
        pPlayer = CUNIT_GetPlayer();
        if (pPlayer)
        {
            pRosterEntry = ROSTER_FindEntryByUnitOrCorpse(pPlayer->dwUnitId);
            if (pRosterEntry)
            {
                nKillCount = ROSTER_EntryGetKillCount(pRosterEntry);

                //  12 elements: push 0Ch at 0x6FB22212 is the constructor count and the lea beside it takes &Buffer[0], so the count is the whole
                //  array. The original hands D2Client's own vector-constructor iterator the closure's address; a declaration says the same thing and
                //  lets the compiler emit its own iterator, a CRT helper rather than behaviour.
                Unicode Buffer[12];

                wsprintfA(szKillCount, "%d", nKillCount);
                Unicode::win2Unicode(Buffer, szKillCount, 12);
                D2WIN_10117_DrawText(Buffer, 600, 25, 4, 0);
            }
        }
    }

    UIPANELS_GameShowDeathMessage();
    QUESTSFX_DrawGameEndCountdown();
}

//1.10f: D2Client.0x6FB20590
//  Queues the one-shot floating text. UIPANELS_DrawTransientText draws it on the next UI pass and then wipes the buffer, so exactly one message is
//  ever in flight and a second call before the next frame overwrites the first. __fastcall with four parameters (retn 8: two stack arguments on
//  top of ecx and edx).
//  void: eax is only a scratch register for one store (mov eax, [esp+4+nY] ... mov gnTransientTextY, eax) and stays live to the retn; the
//  null-argument path has no uninitialised return, and the sole caller, in UIPANELS_DrawInGamePanels, never reads it.
void __fastcall UIPANELS_SetTransientText(const Unicode *pwszText, int32_t nX, int32_t nY, int32_t nColor)
{
    //  Cleared before the null test, not after: a null argument still cancels whatever was queued, and leaves the position globals alone.
    gwszTransientText[0].ch = 0;

    if (!pwszText)
        return;

    //  BUG: the copy is unbounded, as the original's (0x6FB205A7). LATENT: the one caller hands it a hireling's name, at most 24 bytes in every
    //  table of the stock install.
    Unicode::strcpy(gwszTransientText, pwszText);   // a string copy into the one buffer, the original's 80 Unicodes
    gnTransientTextX = nX;
    gnTransientTextY = nY;
    gnTransientTextColor = nColor;
}


//1.10f: D2Client.0x6FB20670
//  The fill of the health bar over a monster, on a 0..128 scale, not 0..100: the default is a full 128 and a known percentage becomes
//  (percent << 7) / 100, the same scale UIPANELS_BuildUnitLabel uses with (ROSTER_GetOtherPlayerPartyLife(dwUnitId) << 7) / 100. It draws
//  nothing; every callee is a query.
//  bNoLife is set when bit 2 of dwFlags is on and the unit is a selectable target of the local player, and the test below is
//  `if (!bNoLife && nLifePercent > 0)`: the flag disqualifies the unit from the roster-percentage route outright, and only then is the stat list
//  consulted instead.
//  The floor of 2 is one `if (nFill128 <= 2)` shared by both routes in the original (a goto from the first route into the second's tail); it is
//  written out in each route here. A unit with a max life of zero keeps the initial 128 and never meets the floor: the `if (nMaxLife)` guard skips
//  the clamp.
int32_t __fastcall UIPANELS_GetUnitLifeFill128(D2UnitStrc *pUnit)
{
    int32_t nFill128;
    BOOL bNoLife;
    int32_t nLifePercent;
    int32_t nMaxLife;

    if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER || CUNIT_IsUnitDead(pUnit))
    {
        return 0;
    }

    nFill128 = 128;
    bNoLife = FALSE;

    if (((pUnit->dwFlags >> 2) & 1) != 0)
    {
        if (CUNIT_IsUnitSelectableTarget(CUNIT_GetPlayer(), pUnit))
        {
            bNoLife = TRUE;
        }
    }

    nLifePercent = ROSTERPETS_GetLifePercent(pUnit->dwUnitId);

    if (!bNoLife && nLifePercent > 0)
    {
        nFill128 = (nLifePercent << 7) / 100;
        if (nFill128 <= 2)
            return 2;
    }
    else
    {
        nMaxLife = D2COMMON_STATLIST_GetMaxLifeFromUnit(pUnit);
        if (nMaxLife)
        {
            nFill128 = (D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_HITPOINTS, 0) << 7) / nMaxLife;
            if (nFill128 <= 2)
                return 2;
        }
    }

    if (nFill128 >= 128)
        return 128;
    return nFill128;
}


//1.10f: D2Client.0x6FB21970
//  Draws the pending one-shot text, then wipes it. The last statement is a memset of the whole buffer, so a message survives exactly one frame
//  and the caller has to write it again to keep it up. The memset size is 0xA0, 80 Unicodes of two bytes (gwszTransientText is Unicode[80]); it
//  is written as the product so that a mismatch would not compile.
//  The box is the measured text width + 8, centred on the requested X and pushed inside [0, gnScreenWidth - width]; the Y is clamped to
//  [D2Win_10125() + 7, gnScreenHeight - 31] and then drawn two pixels above the clamped value, so nDrawY is the box's position and nDrawY - 2
//  the text's. `if (nDrawY - nTopLimit < 0)` rather than `if (nDrawY < nTopLimit)` is what the instructions do (a subtraction and a sign test);
//  the two are the same for every value either can hold here.
static_assert(80 * sizeof(Unicode) == 0xA0,
              "gwszTransientText is Unicode[80] in the database and the memset "
              "at the end of UIPANELS_DrawTransientText is 0xA0 bytes -- 80 two-byte "
              "characters");

void __cdecl UIPANELS_DrawTransientText()
{
    int32_t nTextWidth;
    int32_t nTopLimit;
    int32_t nX;
    int32_t nDrawY;

    if (!Unicode::strlen(gwszTransientText))
        return;

    nTextWidth = D2WIN_10121_GetTextWidth(gwszTransientText) + 8;
    nTopLimit = D2WIN_10125() + 7;

    nX = gnTransientTextX - (nTextWidth >> 1);
    nDrawY = gnTransientTextY + 2;

    if (nX < 0)
        nX = 0;
    if (nX + nTextWidth > gnScreenWidth)
        nX = gnScreenWidth - nTextWidth;
    if (nDrawY - nTopLimit < 0)
        nDrawY = nTopLimit;
    if (nDrawY > gnScreenHeight - 31)
        nDrawY = gnScreenHeight - 31;

    D2WIN_10117_DrawText(gwszTransientText, nX, nDrawY - 2, 0, 1);

    //  80 Unicodes, 160 bytes, written as the product rather than 0xA0 so that a size mismatch would not compile
    memset(gwszTransientText, 0, 80 * sizeof(Unicode));
}


//1.10f: D2Client.0x6FB21A00
//  Draws the charge-up icon strip (the Assassin's charge orbs), walked out of sgptDataTables->pProgressiveStates, the same list
//  SKILLDESC_GetChargeUpDamagePercent walks.
//  It never draws in a stock 1.10 client: gbChargeUpIconsEnabled gates the whole body and is zero for the life of the process. The address lies
//  past .data's raw size, so the loader zero-fills it, the module has exactly one reference to it (the read here) and nothing writes it. The if
//  and the body under it are written out in full. Whether the strip is meant to be off is not established.
//  gnChargeUpIconStepX is in the same zero-filled tail and is read the same once, so the horizontal step between icons is also zero today, which
//  would stack every icon on one spot if the gate ever opened.
//  The anchor is nX = gnScreenWidth - 148 (gnChargeUpIconRightInset) and nY = gnScreenHeight - 64 (gnChargeUpIconBottomInset), the third value
//  (gnChargeUpIconBottomInset800, also 64) replacing the second only at 800x600. So 148 is a right inset and the 64s are one bottom inset per
//  resolution; they are equal because stock data gives both the same number.
//  The bounds checks are not all the same shape: nState >= STATE_NONE && nState < nStatesTxtRecordCount is a half-open range and correct.
//  BUG: wStat >= 0 && wStat <= nItemStatCostTxtRecordCount is closed at the top and admits one index past the end. Not reachable today: the
//  strip is never switched on, and it would also need a states.txt row whose stat column names the last record.
//  nProgressiveStates is read as 16 bits (D2Common writes it as a word too) and is an int16_t.
void __cdecl UIPANELS_DrawChargeUpIcons()
{
    D2UnitStrc *pPlayer;
    D2StatesTxt *pStatesTxt;
    D2C_States nState;
    int32_t nStatValue;
    int32_t iState;
    int32_t nX;
    int32_t nY;

    if (!gpAct)
        return;
    if (!CUNIT_GetPlayer())
        return;
    if (!D2COMMON_UNITS_GetRoom(CUNIT_GetPlayer()))
        return;

    //  The gate that is never set (gbChargeUpIconsEnabled is zero for the life of the process).
    if (!gbChargeUpIconsEnabled)
        return;

    nX = gnScreenWidth - gnChargeUpIconRightInset;
    nY = gnScreenHeight - gnChargeUpIconBottomInset;
    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600)
        nY = gnScreenHeight - gnChargeUpIconBottomInset800;

    pPlayer = CUNIT_GetPlayer();

    for (iState = 0; iState < sgptDataTables->nProgressiveStates; ++iState)
    {
        nState = (D2C_States)sgptDataTables->pProgressiveStates[iState];

        if (!D2COMMON_STATES_CheckState(pPlayer, nState))
            continue;
        if (!D2COMMON_STATLIST_GetStatListFromUnitAndState(pPlayer, nState))
            continue;
        //  Both sides are cast to int deliberately. D2C_States has no negative member, so MSVC gives it an unsigned underlying type and
        //  `nState >= count` becomes an unsigned compare (C4018), which would also make `nState < STATE_NONE` dead. The original loads the id out of
        //  a `__int16 *` and compares it signed, so the casts reproduce the instruction rather than paper over the warning.
        if ((int32_t)nState < (int32_t)STATE_NONE || (int32_t)nState >= sgptDataTables->nStatesTxtRecordCount)
        {
            continue;
        }

        pStatesTxt = &sgptDataTables->pStatesTxt[nState];
        if (!pStatesTxt)
            continue;

        //  The `pgsv` flag -- the same bit STATES_RemFunc1_Progressive tests -- is what admits a state to the strip.
        if ((pStatesTxt->nStateFlags[0] & gdwBitMasks[4]) == 0)
            continue;

        //  Both words are read signed and each is set against its int count whole, as the bytes do: `test ax,ax / jle` and
        //  `movsx eax,ax / cmp eax,[ecx+0BC0h] / jge` (0x6FB21AFB..0x6FB21B0B), `test ax,ax / jl` and `movsx eax,ax /
        //  cmp eax,[ecx+0BD4h] / jg` (0x6FB21B11..0x6FB21B21).
        if ((int16_t)pStatesTxt->wPrgOverlay <= 0 || (int16_t)pStatesTxt->wPrgOverlay >= sgptDataTables->nOverlayTxtRecordCount)
        {
            continue;
        }

        //  closed at the top: admits one index past the end
        if ((int16_t)pStatesTxt->wStat < 0 || (int16_t)pStatesTxt->wStat > sgptDataTables->nItemStatCostTxtRecordCount)
        {
            continue;
        }

        nStatValue = D2COMMON_STATLIST_UnitGetStatValue(pPlayer, (D2C_ItemStats)pStatesTxt->wStat, 0);

        //  Three charges maximum, and the overlay id is the base plus the charge count minus one: three consecutive overlay rows.
        if (nStatValue > 0 && nStatValue <= 3)
        {
            GFX_DrawOverlayAt(pPlayer, (int16_t)pStatesTxt->wPrgOverlay + nStatValue - 1, nX, nY);
            nX -= gnChargeUpIconStepX;
        }
    }
}


//1.10f: D2Client.0x6FB22280
//  Draws one line centred in the play-area band, at the caller's font, Y and colour, restoring the previous font afterwards. Three callers, all in
//  UIPANELS_DrawInGamePanels. The centring is ((band.left + band.right) >> 1) - (width >> 1), relative to the band UI_GetPlayAreaBand fills in and
//  not to the screen; +0 and +8 of the band are the pair being averaged.
//  The text is copied into a local Unicode[128] buffer, and that local is measured and drawn (sub esp, 110h is the band rectangle and the
//  128-Unicode buffer and nothing else). BUG: there is no length check, so a caller passing more than 127 characters overruns it. LATENT: all
//  three callers pass a fixed panel-label buffer.
//  void: a trailing call leaves its answer in eax whether or not anything returns it, and none of the three callers reads eax. The 128 default
//  constructors are called before UI_GetPlayAreaBand is even asked, so the cost is paid on the frames where nothing is drawn as well; written in
//  that order because that is the order the instructions are in.
static_assert(sizeof(RECT) + 128 * sizeof(Unicode) == 0x110,
              "UIPANELS_DrawTextCenteredInPanelRect's frame is one RECT and 128 "
              "Unicodes -- `sub esp, 110h` at 0x6FB22280");

void __fastcall UIPANELS_DrawTextCenteredInPanelRect(const Unicode *pwszText, int32_t nFont, int32_t nY, int32_t nColor)
{
    RECT pBand;
    int32_t nPrevFont;
    int32_t nTextWidth;

    Unicode wszText[128];

    if (!UI_GetPlayAreaBand(&pBand))
        return;
    if (!pwszText)
        return;

    nPrevFont = D2WIN_10127_SetFont((Font)nFont);
    Unicode::strcpy(wszText, pwszText);
    nTextWidth = D2WIN_10121_GetTextWidth(wszText);
    D2WIN_10117_DrawText(wszText, ((pBand.left + pBand.right) >> 1) - (nTextWidth >> 1), nY, nColor, 1);
    D2WIN_10127_SetFont((Font)nPrevFont);
}


//  The three tbl indices UIPANELS_GameShowDeathMessage fetches.
static_assert(STR_IDX_5094_Deathmessage == 5094
              && STR_IDX_5095_Deathmessnight == 5095
              && STR_IDX_5096_Harddeathmessage == 5096,
              "the three tbl indices UIPANELS_GameShowDeathMessage fetches, read off the "
              "`mov ecx, <n>` before each D2LANG_GetStringFromTblIndex");


//1.10f: D2Client.0x6FB22320
//  The death panel. Sole caller UIPANELS_DrawInGamePanels; it runs every frame and leaves at once unless the player is dead. Two cel files stacked
//  48 pixels apart, each loaded lazily out of the language directory rather than the global one (D2LANG_10006 fills a stack buffer with the
//  language code, which becomes a path component). Then a message under them:
//
//      hardcore            tbl 5096  Harddeathmessage
//      gold lost           tbl 5094  Deathmessage, with the amount
//      nightmare or hell   tbl 5095  Deathmessnight, and only when not hardcore
//
//  The gold stat is read twice, once to test and once to use, with nothing between the reads that could change it. With no gold lost in a
//  non-hardcore game the goto skips `nY += 48` as well as the gold line, so Deathmessnight rises into the slot the gold message would have
//  taken; that reads as deliberate layout, and it is REACHABLE: any non-hardcore death on nightmare or hell with nothing to lose.
//  gbDeathMessageShown is set here and cleared on the not-dead path, and nothing in the module reads it.
//  The two D2Win_10134_DrawCellFile calls take six arguments (the bytes at 0x6FB223B2 and 0x6FB22414 carry all of them): ecx is the image
//  handle, arriving from either the already-loaded global or ARCHIVE_CellFileLoad's return, and edx is gnScreenWidth / 2.
void __cdecl UIPANELS_GameShowDeathMessage()
{
    D2UnitStrc *pPlayer;
    int32_t nCenterY;
    int32_t nY;
    char szLangCode[4];
    char szPath[260];
    int32_t nGoldLost;
    int32_t nTextWidth;
    const Unicode *pwszNight;
    int32_t nX;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
        return;
    if (!CUNIT_IsUnitDead(pPlayer))
    {
        gbDeathMessageShown = 0;
        return;
    }

    nCenterY = gnScreenHeight / 2 - 94;
    D2LANG_10006((uint32_t)(UINT_PTR)szLangCode, 0);

    //  Six arguments, two of them in registers (the image handle and gnScreenWidth / 2).
    if (gpImgYouDiedHardcore || (sprintf(szPath, "%s\\UI\\%s\\youdiedhardcore", szDataLocal, szLangCode), (gpImgYouDiedHardcore = ARCHIVE_CellFileLoad(szPath, 0)) != NULL))
    {
        D2WIN_10134_DrawCellFile(gpImgYouDiedHardcore, gnScreenWidth / 2, nCenterY, 1, (DrawMode)5, 1);
    }

    nY = nCenterY + 48;
    gbDeathMessageShown = 1;

    if (gpImgYouDiedInst || (sprintf(szPath, "%s\\UI\\%s\\youdiedinst", szDataLocal, szLangCode), (gpImgYouDiedInst = ARCHIVE_CellFileLoad(szPath, 0)) != NULL))
    {
        D2WIN_10134_DrawCellFile(gpImgYouDiedInst, gnScreenWidth / 2, nY, 1, (DrawMode)5, 1);
    }

    Unicode wszText[256];

    if (WINMAIN_IsHardcoreGame())
    {
        Unicode::strcpy(wszText, D2LANG_GetStringFromTblIndex(STR_IDX_5096_Harddeathmessage));
    }
    else
    {
        //  Read twice, once to test and once to use.
        if (!D2COMMON_STATLIST_UnitGetStatValue(pPlayer, STAT_GOLDLOST, 0))
            goto difficulty_line;
        nGoldLost = D2COMMON_STATLIST_UnitGetStatValue(pPlayer, STAT_GOLDLOST, 0);
        Unicode::sprintf(256, wszText, D2LANG_GetStringFromTblIndex(STR_IDX_5094_Deathmessage), nGoldLost);
    }

    nY += 48;
    D2WIN_10127_SetFont(D2FONT_FONT30);
    nTextWidth = D2WIN_10121_GetTextWidth(wszText);
    D2WIN_10117_DrawText(wszText, gnViewportRight / 2 - nTextWidth / 2, nY, 1, 1);

difficulty_line:
    if (GAME_GetDifficulty())
    {
        if (!WINMAIN_IsHardcoreGame())
        {
            D2WIN_10127_SetFont(D2FONT_FONT30);
            //  The tbl string is fetched twice, once to measure and once to draw, as the original does.
            pwszNight = D2LANG_GetStringFromTblIndex(STR_IDX_5095_Deathmessnight);
            nX = gnViewportRight / 2 - D2WIN_10121_GetTextWidth(pwszNight) / 2;
            D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_5095_Deathmessnight), nX, nY + 48, 1, 1);
        }
    }
}


//1.10f: D2Client.0x6FB22560
//  The per-unit callback UIPANELS_DrawInGamePanels hands to CUNIT_ForEachPlayerUnit, and only in multiplayer. It puts a name label over every
//  player but the local one. It converts the unit's client coordinates into view-relative screen ones, lifts them 70 pixels to sit above the head,
//  and shifts by a quarter screen when one side panel is open, rejecting anything that has crossed the midline in the process. UI shift 3, both
//  panels open, returns at once.
//  SetRect is a USER32 import and is called directly. The rectangle it builds only converts screen space to client space: the label offset is
//  the constant 70 and the rectangle contributes nothing but rc.left and rc.top.
//  void: its only reference is the argument of CUNIT_ForEachPlayerUnit (0x6FB220F1), and the walker never reads eax.
void __fastcall UIPANELS_BuildOtherPlayerLabel(D2UnitStrc *pUnit)
{
    D2CoordStrc pClientCoords;
    RECT rc;
    int32_t nUiShiftScreen;
    int32_t nLabelX;
    int32_t nLabelY;
    int32_t xLeft, yTop;
    int32_t pnLeft, pnTop, pnRight, pnBottom;
    int32_t pnLifeBarFill;
    int32_t pbHostileOrCorpse;

    nUiShiftScreen = SCROLL_GetUiShiftScreen();
    if (nUiShiftScreen == 3 || pUnit == CUNIT_GetPlayer())
        return;

    VIEW_GetRect(gpView, &pnLeft, &pnTop, &pnRight, &pnBottom);
    VIEW_GetOrigin(gpView, &xLeft, &yTop);
    SetRect(&rc, xLeft, yTop, xLeft + pnRight - pnLeft, pnBottom + yTop - pnTop);

    D2COMMON_UNITS_GetClientCoords(pUnit, &pClientCoords);

    nLabelX = pClientCoords.nX - rc.left;
    nLabelY = -70 - rc.top + pClientCoords.nY;
    pClientCoords.nY = nLabelY;
    pClientCoords.nX = nLabelX;

    if (nUiShiftScreen == 1)
    {
        nLabelX += gnScreenWidth / -4;
        pClientCoords.nX = nLabelX;
        if (nLabelX > gnScreenWidth / 2)
            return;
    }
    else if (nUiShiftScreen == 2)
    {
        nLabelX += gnScreenWidth / 4;
        pClientCoords.nX = nLabelX;
        if (nLabelX < gnScreenWidth / 2)
            return;
    }

    //  BUG: the final clip test has its axes crossed. The label's Y is bounded by gnScreenWidth (cmp edi, esi at 0x6FB22667, esi = gnScreenWidth)
    //  and its X by gnScreenHeight (mov eax, gnScreenHeight at 0x6FB22670). REACHABLE: at 800x600 it suppresses the label of any player whose X has
    //  reached 650, the right eighth of the screen, and lets a Y between 600 and 800 through a test it should have failed; at 640x480 the X limit
    //  becomes 530 and the Y limit 640, so labels vanish over the right sixth. Only a square resolution hides it. Reproduced exactly, with the
    //  globals in the order the instructions use them.
    if (nLabelY > 0 && nLabelY < gnScreenWidth && nLabelX > -50 && nLabelX < gnScreenHeight + 50)
    {
        Unicode pOutBuffer[128];

        UIPANELS_BuildUnitLabel(pUnit, &pnLifeBarFill, &pbHostileOrCorpse, pOutBuffer, 128);
    }
}

//  Unicode::strcat, Unicode::Personalize and Unicode::strstr are declared in D2Structs.h and linked from D2Lang.lib. Personalize's fifth parameter
//  is ELANGUAGE rather than D2C_Language: the two name the same enum and mangle differently, so the cast is at the call site.

//  __FILE__ for this unit, the string UIPANELS_TextStripColorCodes' assert pushes; ui.cpp has its own copy.
static const char szUiCppFile[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UI\\ui.cpp";


//1.10f: D2Client.0x6FB20500
//  Strips every colour code out of a string, in place. A code is whatever tbl 3994 "colorcode" holds plus the one character after it, which is the
//  colour selector.
//  It is quadratic: Unicode::strstr is re-run from the start of the string on every iteration rather than from where it left off, so removing n
//  codes costs n scans of the whole string. The assert above the loop, `strlen < 500` (ui.cpp:895), bounds it and kills the process rather than
//  returning. nCodeLen is computed once above the loop.
//  The copy is `nTailLen = Unicode::strlen(i); memcpy(i, &i[nCodeLen + 1], 2 * (nTailLen - nCodeLen));`: the source is one past the code plus the
//  character it swallows, and the count is the tail minus the code minus its argument, plus one for the terminator, so the NUL is carried along
//  rather than written back separately. The `2 *` is the wide character.
//  A code at the very end of the string, with no character after it, ends the loop and is left in place (`if (!i[nCodeLen].ch) break;`): the
//  original's behaviour.
const Unicode *__fastcall UIPANELS_TextStripColorCodes(Unicode *pszText)
{
    const Unicode *pwszCode;
    Unicode *i;
    int32_t nCodeLen;
    int32_t nTailLen;

    if ((int32_t)Unicode::strlen(pszText) >= 500)
    {
        FOG_DisplayAssert("Unicode::strlen(pszSrc) < 500", szUiCppFile, 895);
        exit(-1);
    }

    pwszCode = D2LANG_GetStringFromTblIndex(STR_IDX_3994_colorcode);
    nCodeLen = Unicode::strlen(pwszCode);

    for (i = Unicode::strstr(pszText, pwszCode); i; i = Unicode::strstr(pszText, pwszCode))
    {
        if (!i[nCodeLen].ch)
            break;
        nTailLen = Unicode::strlen(i);
        memcpy(i, &i[nCodeLen + 1], 2 * (nTailLen - nCodeLen));
    }

    return pszText;
}


//1.10f: D2Client.0x6FB205D0
//  Puts a string and a colour into the unit-label scratch. A null pointer terminates the buffer and leaves the colour alone, which is only safe
//  because nothing reads the colour when the text is empty. Sole caller UIPANELS_DrawUnitNameLabel. void: what the original leaves in eax is
//  Unicode::strcpy's destination, which the one call site does not read.
void __fastcall UIPANELS_SetUnitLabelText(const Unicode *pText, int32_t nColour)
{
    if (pText)
    {
        //  BUG: the copy is unbounded, as the original's (0x6FB205E7). LATENT: a label is at most 52 bytes in every table of the stock install.
        Unicode::strcpy(gwszDrawTextScratch, pText);   // a string copy into the one buffer, the original's 256 Unicodes
        gnUnitLabelColor = nColour;
    }
    else
    {
        gwszDrawTextScratch->ch = 0;
    }
}


//1.10f: D2Client.0x6FB20600
//  Should this unit get a floating name label, and as a side effect build its first line. Two refusals -- UI shift 3, and a conversation in
//  progress -- then the label buffer is cleared and the answer is whether the unit is a unique or a minion. The BOOL is MONSTER_IsMinion's answer,
//  returned unchanged when it is false; UIPANELS_DrawUnitNameLabel tests it (test eax, eax / jnz) on the next instruction. The second argument is
//  a tbl string index (the caller loads 275Dh, 10077, at 0x6FB20FFD).
//  bIsMinion is initialised here where the original leaves it undefined on the unique path; not observable, since that path returns 1 before the
//  variable is read.
BOOL __fastcall UIPANELS_ShouldDrawNameLabel(D2UnitStrc *pUnit, D2C_StringIndices nPrefixStr)
{
    BOOL bIsMinion = FALSE;

    if (SCROLL_GetUiShiftScreen() == 3)
        return FALSE;

    D2UnitGUID pNpcGUID;
    if (NPCMENU_GetInteractingNpc(&pNpcGUID))
        return FALSE;

    gwszUnitLabelLine1[0].ch = 0;

    if (MONSTER_IsUnique(pUnit) || (bIsMinion = MONSTER_IsMinion(pUnit)) != 0)
    {
        MONUNIQUE_MonUModBuildMonsterModifierLabel(pUnit, gwszUnitLabelLine1, 256, nPrefixStr);
        return TRUE;
    }

    return bIsMinion;
}


//1.10f: D2Client.0x6FB21570
//  Builds the second line of a monster's hover label, its immunity list, and leaves it in gwszUnitLabelLine2 for UIPANELS_DrawInGamePanels to
//  draw. gMonImmunityLabels is six packed ten-byte records, { int nStatId; int16_t wStrIdx; int nColor; } (the colour at +6 is unaligned, which
//  is what proves the packing). The walk starts at base + 4 and steps ten, so the cursor points at the tbl index and reads the stat as p[-1].
//  Each immunity is shown when its stat reaches 100, in the table's order, which is why Physical and Magic come before the four elements:
//
//      stat 36 damageresist  tbl 10085 'Immune to Physical'   colour 4
//      stat 37 magicresist   tbl 10086 'Immune to Magic'      colour 8
//      stat 39 fireresist    tbl 10081 'Immune to Fire'       colour 1
//      stat 41 lightresist   tbl 10083 'Immune to Lightning'  colour 9
//      stat 43 coldresist    tbl 10082 'Immune to Cold'       colour 3
//      stat 45 poisonresist  tbl 10084 'Immune to Poison'     colour 2
//
//  The separator is ", ": tbl 3995 "space" is fetched first and tbl 3852 "KeyComma" second, and the appends (0x6FB21602 and 0x6FB2160C) take
//  them in the other order, comma then space. gnUnitLabelLine2Color is set to 4 at the end, the line's base colour; the per-immunity colours are
//  colour codes embedded in the text itself.
void __fastcall UIPANELS_BuildMonsterImmunityLabel(const D2UnitStrc *pUnit)
{
    const Unicode *pwszSpace;
    const Unicode *pwszComma;
    const Unicode *pwszImmunityName;
    UINT_PTR nImmunityRec;   // an integer cursor, bounded from the table itself

    pwszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
    pwszComma = D2LANG_GetStringFromTblIndex(STR_IDX_3852_KeyComma);

    Unicode dest[256];
    dest[0].ch = 0;

    //  The walk starts at the table's +4 (mov edi, 6FB70074h, 0x6FB215BE), steps 10 bytes six times and stops at +64. The original compares the
    //  cursor with the address of the next global, gbUiWindowCloseable (cmp edi, 6FB700B0h / jl, 0x6FB21644). Here both are separate objects of
    //  ours and a pointer at +64 would be four bytes past the table, so the cursor is an integer, bounded from the table itself and read only
    //  inside it. The compare is signed, as the bytes' jl.
    nImmunityRec = (UINT_PTR)&gMonImmunityLabels[4];
    do
    {
        if (D2COMMON_STATLIST_UnitGetStatValue(pUnit, *(const D2C_ItemStats *)(nImmunityRec - 4), 0) >= 100)
        {
            //  A fresh 256-Unicode buffer per immunity, constructed inside the loop; the declaration is what constructs it, so it stays inside
            //  the `if`.
            Unicode wszText[256];

            //  comma, then space
            if (dest[0].ch)
            {
                Unicode::strcat(dest, pwszComma);
                Unicode::strcat(dest, pwszSpace);
            }

            pwszImmunityName = D2LANG_GetStringFromTblIndex((D2C_StringIndices)*(const uint16_t *)nImmunityRec);
            Unicode::strcpy(wszText, pwszImmunityName);
            UI_AddColorCodeToWstring((wchar_t *)wszText, *(const uint32_t *)(nImmunityRec + 2));
            Unicode::strcat(dest, wszText);
        }
        nImmunityRec += 10;
    }
    while ((int32_t)nImmunityRec < (int32_t)((UINT_PTR)gMonImmunityLabels + 64));

    //  BUG: the copy is unbounded, as the original's (0x6FB21659), and so is every strcat above into dest. LATENT: the six names with their codes
    //  and separators are at most 204 bytes in every table of the stock install.
    Unicode::strcpy(gwszUnitLabelLine2, dest);   // 256 Unicodes into 256
    gnUnitLabelLine2Color = 4;
}


//1.10f: D2Client.0x6FB21680
//  Builds the floating label over a unit, and the life-bar fill with it. Two out-parameters and a destination buffer; two callers,
//  UIPANELS_DrawUnitNameLabel and UIPANELS_BuildOtherPlayerLabel, both passing 128. A plain __fastcall with five parameters.
//  Anim mode 17 is a corpse. That branch takes the roster entry's szName at +0x00 (the plain name, not the szNameEx that ROSTER_GetNameEx
//  returns), widens it, and runs Unicode::Personalize with tbl 2315 "Corpse", which produces the possessive form in whatever way the language
//  demands. The fill is zeroed and the flag set. Otherwise the fill is (ROSTER_GetOtherPlayerPartyLife(id) << 7) / 100, a percent rescaled to
//  0..128, the same scale UIPANELS_GetUnitLifeFill128 uses, so the label and the bar speak the same units.
//  pbHostileOrCorpse is 0 or 1 and nothing else: 1 for a corpse and 1 when ROSTER_IsHostileTo. Both callers keep it in a local called
//  nTextColor, so it selects the label's colour.
//  void: nothing reads what the corpse branch leaves in eax (Unicode::Personalize leaves its own output buffer there on every path).
void __fastcall UIPANELS_BuildUnitLabel(D2UnitStrc *pUnit, int32_t *pnLifeBarFill, int32_t *pbHostileOrCorpse, Unicode *pOutBuffer, int32_t nMaxChars)
{
    char *szDisplayName;
    const Unicode *pwszCorpse;
    D2C_Language eLanguage;
    uint32_t dwUnitId;
    uint32_t dwTargetGUID;
    uint32_t dwPlayerGUID;
    D2UnitStrc *pPlayer;

    szDisplayName = ROSTER_GetPlayerDisplayName(pUnit);

    //  dwAnimMode is one arm of the anonymous union at +0x10 (dwAnimMode / dwItemMode / dwMissileMode, selected by dwUnitType), so reading the
    //  anim-mode arm off a player is correct.
    if (pUnit && pUnit->dwAnimMode == 17)
    {
        *pnLifeBarFill = 0;

        Unicode wszRosterName[128];

        Unicode::win2Unicode(wszRosterName, szDisplayName, 128);
        eLanguage = (D2C_Language)D2LANG_STRTABLE_GetLanguage();
        pwszCorpse = D2LANG_GetStringFromTblIndex(STR_IDX_2315_Corpse);
        Unicode::Personalize(pOutBuffer, wszRosterName, pwszCorpse, nMaxChars, (ELANGUAGE)eLanguage);
        *pbHostileOrCorpse = 1;
        return;
    }

    dwUnitId = pUnit ? pUnit->dwUnitId : (uint32_t)-1;
    *pnLifeBarFill = (ROSTER_GetOtherPlayerPartyLife(dwUnitId) << 7) / 100;
    *pbHostileOrCorpse = 0;

    dwTargetGUID = pUnit ? pUnit->dwUnitId : (uint32_t)-1;
    pPlayer = CUNIT_GetPlayer();
    dwPlayerGUID = pPlayer ? pPlayer->dwUnitId : (uint32_t)-1;

    if (ROSTER_IsHostileTo(dwPlayerGUID, dwTargetGUID))
        *pbHostileOrCorpse = 1;

    //  nMaxChars is 128 at both call sites and this path hard-codes 128 instead of using it: the argument really is ignored here.
    Unicode::win2Unicode(pOutBuffer, szDisplayName, 128);
}


//1.10f: D2Client.0x6FB21790
//  Draws a bordered frame by tiling cels in 12-pixel steps. Callers: TEXT_DrawPopup, LISTBOX_ImplDraw, DIALOG_Draw and the two panel draws of
//  config.cpp. The parameter is a RECT in {left, top, right, bottom} order, as every caller writes it and the body reads it.
//  One counter drives all four edges:
//
//      corners       0 top-left   1 top-right   8 bottom-left   9 bottom-right
//      top edge      2..7    ++nTileSeq % 6 + 2     12 px pitch, y = top + 12
//      bottom edge  16..21   ++nTileSeq % 6 + 16    12 px pitch, y = bottom + 10
//      left edge    10..12   ++nTileSeq % 3 + 10    12 px pitch, x = left - 4
//      right edge   13..15   ++nTileSeq % 3 + 13    12 px pitch, x = right - 7
//
//  nTileSeq is never reset between the edges, so the variant a tile gets depends on how many tiles preceded it anywhere on the border: one
//  sequence walked round the box rather than four independent runs, which is why the same rectangle always looks identical and two rectangles of
//  different widths do not line up with each other. The four insets are deliberately asymmetric: the left edge sits 4 px outside pRect->left, the
//  right 7 px inside pRect->right, the top 12 below pRect->top and the bottom 10 below pRect->bottom + 1.
void __fastcall UIPANELS_DrawTiledFrame(RECT *pRect)
{
    D2GfxDataStrc pData;
    int32_t nLeft;
    int32_t nTop;
    int32_t nBottom;
    int32_t nRight;
    int32_t nRightInner;
    int32_t nRightLimit;
    int32_t nBottomLimit;
    int32_t nTileSeq;
    int32_t nPos;

    nLeft = pRect->left;
    nTop = pRect->top;
    nBottom = pRect->bottom + 1;
    nRight = pRect->right + 1;
    nRightInner = pRect->right - 11;

    memset(&pData, 0, sizeof(pData));
    pData.pCellFile = gpImgUiBoxPieces;
    pData.nDirection = 0;

    nTileSeq = 0;
    nRightLimit = nRightInner - 1;
    nBottomLimit = nBottom - 1;

    //  top edge
    for (nPos = nLeft + 12; nPos < nRightLimit; nPos += 12)
    {
        pData.nFrame = ++nTileSeq % 6 + 2;
        D2GFX_TEXTURE_CelDraw(&pData, nPos, nTop + 12, 0xFFFFFFFF, 5, NULL);
    }

    //  bottom edge: it starts at left + 13, one pixel further in than the top edge's + 12
    for (nPos = nLeft + 13; nPos < nRightLimit; nPos += 12)
    {
        pData.nFrame = ++nTileSeq % 6 + 16;
        D2GFX_TEXTURE_CelDraw(&pData, nPos, nBottom + 10, 0xFFFFFFFF, 5, NULL);
    }

    //  left edge
    for (nPos = nTop + 12 + 10; nPos < nBottomLimit; nPos += 12)
    {
        pData.nFrame = ++nTileSeq % 3 + 10;
        D2GFX_TEXTURE_CelDraw(&pData, nLeft - 4, nPos, 0xFFFFFFFF, 5, NULL);
    }

    //  right edge
    for (nPos = nTop + 12 + 12; nPos < nBottomLimit; nPos += 12)
    {
        pData.nFrame = ++nTileSeq % 3 + 13;
        D2GFX_TEXTURE_CelDraw(&pData, nRight - 7, nPos, 0xFFFFFFFF, 5, NULL);
    }

    //  the four corners, last and unsequenced
    pData.nFrame = 0;
    D2GFX_TEXTURE_CelDraw(&pData, nLeft, nTop + 12, 0xFFFFFFFF, 5, NULL);
    pData.nFrame = 1;
    D2GFX_TEXTURE_CelDraw(&pData, nRightInner, nTop + 12, 0xFFFFFFFF, 5, NULL);
    pData.nFrame = 8;
    D2GFX_TEXTURE_CelDraw(&pData, nLeft, nBottom + 1, 0xFFFFFFFF, 5, NULL);
    pData.nFrame = 9;
    D2GFX_TEXTURE_CelDraw(&pData, nRightInner, nBottom + 1, 0xFFFFFFFF, 5, NULL);
}


//  The field offsets the next functions read as raw displacements; each assertion pins one to the layout of the generated structs (wszLevelWarp
//  is reached as a bare +446).

static_assert(offsetof(D2ItemsTxt, nQuest) == 0x12A,
              "`mov cl, [eax+12Ah]` at 0x6FB207F3 is ItemsTxt.nQuest");
static_assert(offsetof(D2ItemsTxt, dwCode) == 0x80,
              "`cmp dword ptr [eax+80h], 2067656Ch` at 0x6FB207FD is the "
              "four-character item code compared against 'leg '");
static_assert(offsetof(D2MonStatsTxt, wNameStr) == 6,
              "`cmp [edi+6], bx` at 0x6FB20D88 and `mov cx, [eax+6]` at "
              "0x6FB21188 are both MonStatsTxt.wNameStr");
static_assert(offsetof(D2MonStats2Txt, nPixHeight) == 0x0D,
              "`mov cl, [eax+0Dh]` at 0x6FB20DA0 is what the label rises by");
static_assert(offsetof(D2ObjectsTxt, nSubClass) == 0x167,
              "`mov al, [eax+167h]` at 0x6FB21268 selects the object label");
static_assert(offsetof(D2ObjectDataStrc, InteractType) == 4,
              "`mov dl, [ecx+4]` at 0x6FB21292 reads the portal's DESTINATION "
              "LEVEL out of the byte D2MOO also calls InteractType");
static_assert(offsetof(D2LvlWarpTxt, dwSelectDX) == 0x0C
              && offsetof(D2LvlWarpTxt, dwSelectDY) == 0x10,
              "`[eax+0Ch]` and `[eax+10h]` at 0x6FB2149D are the halves added "
              "to dwSelectX/Y to centre the warp label");
static_assert(offsetof(D2LevelsTxt, szLevelName) == 0xF5,
              "`lea ecx, [edi+0F5h]` at 0x6FB21402 hands the ASCII name to "
              "D2LANG_GetStringByReferenceString");
static_assert(offsetof(D2LevelsTxt, wszLevelName) == 0x16E,
              "`add eax, 16Eh` at 0x6FB212C1 is the portal label's level name");
static_assert(offsetof(D2LevelsTxt, wszLevelWarp) == 0x1BE,
              "`lea eax, [edx+1BEh]` at 0x6FB21501 -- the WARP name, one "
              "80-byte Unicode[40] past wszLevelName, and the reason a stair "
              "reads `To The ...` where the portal above it does not");


//1.10f: D2Client.0x6FB20740
//  Builds an item's ground label, the name coloured by quality, either into a caller's buffer or straight to the screen. Two callers:
//  UIPANELS_DrawUnitNameLabel, for the item under the cursor, and SHOWITEMS_DrawGroundItemLabels, for every item on the floor while ALT is held.
//  Both pass pszOut = NULL, so the drawing arm is the one that runs and the buffer arm has no caller in this module.
//  It moves the label row before it knows it will draw (mov ecx, gnLabelScreenY / sub ecx, 10h / mov gnLabelScreenY, ecx, and only then
//  test esi, esi), so the row advances per call and not per label drawn, and a null unit still consumes a row; that is what makes a column of
//  ground labels stack upward by 16 pixels each.
//  The colour switch is a six-entry jump table over quality - 4, so qualities 1..3 (inferior, normal, superior) fall to the default and are
//  coloured only if socketed or ethereal:
//
//      ITEMQUAL_MAGIC     3      ITEMQUAL_CRAFT      8
//      ITEMQUAL_SET       2      ITEMQUAL_TEMPERED  10
//      ITEMQUAL_RARE      9      socketed/ethereal   5
//      ITEMQUAL_UNIQUE    4      everything else     0
//
//  Then a quest item is forced to 4, unless its code is `leg `, Wirt's Leg (0x2067656C little-endian): the only quest item in the three item
//  tables that is a usable weapon, and this is the line that keeps it white.
//  ITEMTYPE_GOLD builds "<amount> <name>" itself out of STAT_GOLD; everything else goes to INV_BuildItemNameText. The gold path formats through
//  the ANSI wsprintfA into a twelve-byte buffer (exactly the width of "-2147483648" plus its NUL) and then converts nine characters of it.
//  BUG: nine, not twelve, so a gold pile of ten digits would lose its last digit. Not reachable in a stock game, where the carried maximum is
//  far below 10^9.
//  The two asserts (ui.cpp:1473 and ui.cpp:1484, `Unicode::strlen(...) < nSize`) are the buffer arm's only bounds check, and both call
//  FOG_DisplayAssert and then exit(-1): they kill the process rather than truncate. They protect only pszOut; the 128-Unicode locals are never
//  checked against anything.
void __fastcall UIPANELS_BuildItemLabelText(D2UnitStrc *pUnit, Unicode *pszOut, int32_t nSize)
{
    const Unicode *pwszUnitName;
    const Unicode *pwszSpace;
    const D2ItemsTxt *pItemsTxt;
    int32_t nTextColor;
    int32_t nGold;
    CHAR szGold[12];

    gnLabelScreenY -= 16;
    if (!pUnit)
        return;

    pwszUnitName = CUNIT_GetUnitName(pUnit);
    nTextColor = 0;

    switch (D2COMMON_ITEMS_GetItemQuality(pUnit))
    {
    case ITEMQUAL_MAGIC:    nTextColor = 3;  break;
    case ITEMQUAL_SET:      nTextColor = 2;  break;
    case ITEMQUAL_RARE:     nTextColor = 9;  break;
    case ITEMQUAL_UNIQUE:   nTextColor = 4;  break;
    case ITEMQUAL_CRAFT:    nTextColor = 8;  break;
    case ITEMQUAL_TEMPERED: nTextColor = 10; break;
    default:
        //  The line numbers are the original's own, pushed as the assert's third argument; they are not this file's lines and must not be
        //  replaced with __LINE__.
        if (D2COMMON_ITEMS_CheckItemFlag(pUnit, IFLAG_SOCKETED, 1437, szUiCppFile) || D2COMMON_ITEMS_CheckItemFlag(pUnit, IFLAG_ETHEREAL, 1441, szUiCppFile))
        {
            nTextColor = 5;
        }
        break;
    }

    //  'leg ' -- 0x2067656C, little-endian, Wirt's Leg
    pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(pUnit->dwClassId);
    if (pItemsTxt->nQuest && pItemsTxt->dwCode != 0x2067656C)
        nTextColor = 4;

    //  The original zeroes all 256 bytes -- element 0 by its own store and 1..127 by `rep stosd` + `stosw` -- and then runs the default
    //  constructor closure over elements 1..127. Both are kept: the zeroing is what the buffer observably starts as, and the 127 calls are calls
    //  the original makes.
    //  128: the loop runs 127 times from &wszText[1] and the store does element 0, so the array is 127 + 1. The declaration constructs all 128
    //  where the original constructs 127; the extra call writes the zero the store below writes again.
    Unicode wszText[128];

    wszText[0].ch = 0;
    memset(&wszText[1], 0, 127 * sizeof(Unicode));

    if (D2COMMON_ITEMS_GetItemType(pUnit) == ITEMTYPE_GOLD)
    {
        nGold = D2COMMON_STATLIST_UnitGetStatValue(pUnit, STAT_GOLD, 0);

        Unicode wszGold[10];

        //  wsprintfA is a USER32 import and is called directly, not through D2Client's IAT. Nine characters are converted out of a twelve-byte
        //  buffer.
        wsprintfA(szGold, "%i", nGold);
        Unicode::win2Unicode(wszGold, szGold, 9);

        Unicode::strcpy(wszText, wszGold);
        pwszSpace = D2LANG_GetStringFromTblIndex(STR_IDX_3995_space);
        Unicode::strcat(wszText, pwszSpace);
        Unicode::strcat(wszText, pwszUnitName);

        if (!pszOut)
        {
            D2WIN_10129_DrawFramedText(wszText, gnLabelScreenX, gnLabelScreenY, nTextColor, 1);
            return;
        }

        UI_AddColorCodeToWstring((wchar_t *)wszText, nTextColor);
        if (Unicode::strlen(wszText) >= nSize)
        {
            FOG_DisplayAssert("Unicode::strlen(szString) < nSize", szUiCppFile, 1473);
            exit(-1);
        }
        Unicode::strcpy(pszOut, wszText);
        return;
    }

    //  128, on the same terms as the buffer above it.
    Unicode wszName[128];

    wszName[0].ch = 0;
    memset(&wszName[1], 0, 127 * sizeof(Unicode));

    //  The capacity passed is 256 and the buffer is 128 (push 100h at 0x6FB20978, beside the 127-element loop that constructs wszName), the one
    //  mismatch among the builder's eight call sites.
    //  BUG: the builder's only length test is against the capacity, and only in two arms; in the original wszName is the frame's highest local
    //  (sub esp, 228h before the pushes), so a name of 128 characters puts its terminator on the return address. LATENT: over the stock tables no
    //  arm's name reaches 128; the widest bound is the magic arm's 117, personalised and quest-coloured.
    INV_BuildItemNameText(pUnit, wszName, 256);

    if (!pszOut)
    {
        D2WIN_10129_DrawFramedText(wszName, gnLabelScreenX, gnLabelScreenY, nTextColor, 1);
        return;
    }

    UI_AddColorCodeToWstring((wchar_t *)wszName, nTextColor);
    if (Unicode::strlen(wszName) >= nSize)
    {
        FOG_DisplayAssert("Unicode::strlen(szItemName) < nSize", szUiCppFile, 1484);
        exit(-1);
    }
    Unicode::strcpy(pszOut, wszName);
}


//1.10f: D2Client.0x6FB20A30
//  The floating name label over a unit: the hover text for players, monsters, NPCs, corpses, objects, items and level warps. Most of its bytes
//  are per-unit-type special cases rather than layout. One caller, UIPANELS_DrawInGamePanels, through SELECT_GetSelectedUnit.
//  The position is computed once, into gnLabelScreenX and gnLabelScreenY, which are set at the top and read by every arm and by
//  UIPANELS_BuildItemLabelText, which also moves Y. Two routes in:
//
//      perspective off   the unit's client coordinates minus the viewport origin, and nothing else
//      perspective on    UNITS_GetPrecisionX/Y through SCALE_AdjustPerspectivePosition, then a quarter of the screen width added or
//                        subtracted according to SCROLL_GetUiShiftScreen, then Y + 16
//
//  SCROLL_GetUiShiftScreen is called twice, once as the top gate and again for the shift, with a perspective test between them; two calls here
//  as well. The division is signed and negated after the fact on the left-shift arm (cdq / and edx,3 / add / sar 2 / neg), which is -(w / 4) and
//  not w / -4; the two agree for every int under C's truncation.
//  Three monster ids are special, and not in the same way:
//
//      356  dopplezon        the Amazon's Decoy. ROSTERPETS_GetOwnerId, then ROSTER_GetNameEx: it wears the owner's name, in the owner's
//                            party-life colour, and the hostility test is local player against owner
//      417  shadowwarrior    the Assassin's two shadows. They take their own monstats name and write it into the unit's monster data with
//      418  shadowmaster     MONSTERS_SetMonsterNameInMonsterData, then draw through CUNIT_GetUnitName
//
//  The write is what makes it work: CUNIT_GetUnitName's monster arm returns the stored monster-data name whenever it is non-empty and only then
//  falls back to monstats. A shadow is a player copy and would otherwise carry a player's name. Anim mode 0 or 12 on a Decoy returns with nothing
//  drawn; 12 is the death mode, so a dying Decoy has no label at all, unlike a dying monster, which gets the `(Corpse)` line below.
//  The corpse suffix is built two ways round: STRTABLE_GetLanguage() != 0 wraps tbl 2315 "Corpse" in the two single-character globals
//  gszAffixOpenParen "(" and gszAffixCloseParen ")", and language 0 appends the bare string.
//  Seven hard-coded monster ids get the unique colour: 243, 333, 242, 211, 544, 570, 229, tested in that order after the unique and champion
//  tests have already run, so they override both. Written out as the original writes them.
//  The object arm: for a portal (nSubClass & 4) the destination level comes from pUnit->pObjectData->InteractType, the byte at +4. nSubClass is
//  tested signed (test al, al / js at 0x6FB2138E), so the local is a signed char here even though the struct member is unsigned: the bytes test
//  bit 7, not any of the high bits.
//  The warp arm uses a different name field: lea eax, [edx+1BEh] is wszLevelWarp, one 80-byte Unicode[40] past wszLevelName, which is why a
//  staircase reads "To The Cave Level 1" where the level itself is "Cave Level 1". Level 8 overrides both with tbl 5085 / 5086 on the Den of
//  Evil quest state, the same pair of strings the object arm switches on with 5047/5048 instead.
void __fastcall UIPANELS_DrawUnitNameLabel(D2UnitStrc *pUnit)
{
    D2UnitStrc *pPlayer;
    D2ActiveRoomStrc *pRoom;
    const D2MonStatsTxt *pMonStats;
    const D2MonStatsTxt *pMonStatsRec;
    const D2LvlWarpTxt *pLvlWarp;
    const D2LevelsTxt *pLevelsTxt;
    const Unicode *pwszSrc;
    const Unicode *pwszName;
    char *pszRosterName;
    int32_t pXAdjust;
    int32_t pYAdjust;
    D2UnitGUID pNpcGUID;
    uint32_t dwOwnerGUID;
    uint32_t dwLocalPlayerGUID;
    int32_t nPrecisionX, nPrecisionY;
    int32_t nQuarterScreenShift;
    int32_t nUiShift;
    int32_t nClassId;
    int32_t nPrevFont;
    int32_t nMonFlags1;
    int32_t nLifeFill;
    int32_t nTextColor;
    int32_t nLabelColor;
    int32_t bIsCorpse;
    int32_t nParenIdx;
    int32_t nLevelId;
    int32_t nObjectId;
    int32_t nWarpX, nWarpY;
    uint32_t nNameOffset;
    int8_t nSubClass;
    char chParen;
    D2C_StringIndices nStrId;
    RECT pBand;
    char szPortalOwner[128];

    if (SCROLL_GetUiShiftScreen() == 3)
        return;
    if (!pUnit)
        return;

    //  A level warp further than 100 units away gets no label. The distance is to the local player, and CUNIT_GetPlayer may return NULL;
    //  UNITS_GetDistanceToOtherUnit is D2Common's and takes it as it comes.
    if (pUnit->dwUnitType == UNIT_TILE)
    {
        pPlayer = CUNIT_GetPlayer();
        if (D2COMMON_UNITS_GetDistanceToOtherUnit(pUnit, pPlayer) > 100)
            return;
    }

    if (!D2GFX_CheckPerspective())
    {
        gnLabelScreenX = D2COMMON_UNITS_GetClientCoordX(pUnit)
                                   - SCROLL_GetViewportOriginX();
        gnLabelScreenY = D2COMMON_UNITS_GetClientCoordY(pUnit)
                                   - SCROLL_GetViewportOriginY();
    }
    else
    {
        nPrecisionX = D2COMMON_UNITS_GetPrecisionX(pUnit);
        nPrecisionY = D2COMMON_UNITS_GetPrecisionY(pUnit);
        if (!D2GFX_SCALE_CheckPerspectivePosition(nPrecisionX, nPrecisionY))
            return;

        D2GFX_SCALE_AdjustPerspectivePosition(nPrecisionX, nPrecisionY, 0, &pXAdjust, &pYAdjust);

        nUiShift = SCROLL_GetUiShiftScreen();
        if (nUiShift == 1)
        {
            nQuarterScreenShift = -(gnScreenWidth / 4);
            pXAdjust += nQuarterScreenShift;
        }
        else if (nUiShift == 2)
        {
            nQuarterScreenShift = gnScreenWidth / 4;
            pXAdjust += nQuarterScreenShift;
        }

        gnLabelScreenX = pXAdjust;
        gnLabelScreenY = pYAdjust + 16;
    }

    switch (pUnit->dwUnitType)
    {
    case UNIT_PLAYER:
    {
        Unicode wszText[128];

        //  pYAdjust and nTextColor are out-parameters here: the perspective code is finished with the first and it is reused as the life-bar
        //  fill. It is the frame the original chose.
        UIPANELS_BuildUnitLabel(pUnit, &pYAdjust, &nTextColor, wszText, 128);
        D2WIN_10130_DrawFramedTextEx(wszText, gnLabelScreenX, gnLabelScreenY - 72, nTextColor, 1, pYAdjust);
        return;
    }

    case UNIT_MONSTER:
    {
        nClassId = pUnit->dwClassId;

        //  417 shadowwarrior / 418 shadowmaster
        if (nClassId == 417 || nClassId == 418)
        {
            if (nClassId < 0 || nClassId >= sgptDataTables->nMonStatsTxtRecordCount)
            {
                goto clear_label_scratch;
            }
            pMonStats = &sgptDataTables->pMonStatsTxt[nClassId];
            //  The inlined accessor's own null test, kept as written. It can only fire if pMonStatsTxt itself is null, and it is the test the
            //  general arm below is missing.
            if (!pMonStats)
                goto clear_label_scratch;

            pwszName = D2LANG_GetStringFromTblIndex((D2C_StringIndices)pMonStats->wNameStr);
            D2COMMON_MONSTERS_SetMonsterNameInMonsterData(pUnit, pwszName);

            if (NPCMENU_GetInteractingNpc(&pNpcGUID))
                goto clear_label_scratch;
            goto draw_plain_monster_name;
        }

        //  Bit 9 of dwFlags is the NPC marker; an NPC gets the plain name and no rank colouring.
        pNpcGUID = (pUnit->dwFlags >> 9) & 1;
        if (pNpcGUID)
        {
            if (NPCMENU_GetInteractingNpc(&pNpcGUID))
                goto clear_label_scratch;

        draw_plain_monster_name:
            nLifeFill = UIPANELS_GetUnitLifeFill128(pUnit);
            D2WIN_10130_DrawFramedTextEx(CUNIT_GetUnitName(pUnit), gnLabelScreenX, gnLabelScreenY - 72, 0, 1, nLifeFill);

        clear_label_scratch:
            gwszDrawTextScratch->ch = 0;
            gwszUnitLabelLine2[0].ch = 0;
            gwszUnitLabelLine1[0].ch = 0;
            return;
        }

        //  356 dopplezon -- the Decoy, which wears its owner's name.
        if (nClassId == 356)
        {
            Unicode wszText[128];

            dwOwnerGUID = ROSTERPETS_GetOwnerId(pUnit->dwUnitId);
            if (dwOwnerGUID != (uint32_t)-1)
            {
                //  dwAnimMode is the +0x10 union's first arm. 0 is uninitialised and 12 is death, and either returns with nothing drawn.
                if (pUnit->dwAnimMode == 0 || pUnit->dwAnimMode == 12)
                {
                    return;
                }

                pszRosterName = ROSTER_GetNameEx(dwOwnerGUID);
                if (pszRosterName)
                {
                    Unicode::toUnicode(wszText, pszRosterName, 128);

                    nTextColor = 0;
                    nLifeFill = (ROSTER_GetOtherPlayerPartyLife(dwOwnerGUID) << 7) / 100;

                    pPlayer = CUNIT_GetPlayer();
                    dwLocalPlayerGUID = pPlayer ? pPlayer->dwUnitId : (uint32_t)-1;
                    if (ROSTER_IsHostileTo(dwLocalPlayerGUID, dwOwnerGUID))
                    {
                        nTextColor = 1;
                    }

                    D2WIN_10130_DrawFramedTextEx(wszText, gnLabelScreenX, gnLabelScreenY - 72, nTextColor, 1, nLifeFill);
                    return;
                }
            }
            //  No owner, or no roster name: fall into the general arm with the label buffer already constructed and unused.
        }

        nPrevFont = D2WIN_10127_SetFont(D2FONT_FONT16);
        nClassId = pUnit->dwClassId;
        pYAdjust = nPrevFont;
        pNpcGUID = nClassId;

        //  BUG: the monstats record comes from an inlined DATATBLS_GetMonStatsTxtRecord whose bounds test produces a null, and this arm reads
        //  through it with nothing in between (xor edi, edi at 0x6FB20D4A, then mov al, [edi+0Dh] at 0x6FB20D73), so an out-of-range class id
        //  faults at address 0x0D. The other inlined copy, in the 417/418 arm (0x6FB21151), honours the null (cmp eax, ebx / jz). NOT ESTABLISHED:
        //  it needs a dwClassId outside [0, 704) (stock monstats.txt has 704 records) on a live UNIT_MONSTER, which nothing in this function can
        //  produce; the packet handler SCMD_HandleSrvAC_SpawnMonster (0x6FAB4BD0) reads the id as a signed 16-bit field and its bounds check
        //  sits inside a branch (0x6FAB4C37) that can be skipped, and what the rest of unit creation does with such an id has not been read.
        //  Not reachable in normal play either way; the open question is a malformed server.
        if (nClassId < 0 || nClassId >= sgptDataTables->nMonStatsTxtRecordCount)
        {
            pMonStatsRec = NULL;
        }
        else
        {
            pMonStatsRec = &sgptDataTables->pMonStatsTxt[nClassId];
        }

        bIsCorpse = 0;

        //  2048, from the iterator's own count argument. The original hands D2Client's vector-constructor iterator the closure's address; a
        //  declaration says the same thing and lets the compiler emit its own iterator.
        Unicode wszMonsterLine[2048];

        nMonFlags1 = pMonStatsRec->nMonStatsFlags[1];

        //  nMonStatsFlags[1] is the second byte of the flag array, not the first, so these two bits are not the first byte's ISMELEE and
        //  PRIMEEVIL; no name for them is claimed here.
        if ((nMonFlags1 & gdwBitMasks[1]) == 0 && (nMonFlags1 & gdwBitMasks[7]) != 0 && pMonStatsRec->wNameStr)
        {
            const D2MonStats2Txt *pMonStats2Rec = DATATBLS_GetMonStats2TxtRecord(pUnit->dwClassId);
            if (pMonStats2Rec)
                gnLabelScreenY -= pMonStats2Rec->nPixHeight;

            //  Two spaces, the name, a space -- and a fourth space appended after the corpse block below, so an ordinary monster's line is
            //  "  <name>  " and a corpse's is "  <name> (Corpse)  ".
            Unicode::strcpy(wszMonsterLine, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
            Unicode::strcat(wszMonsterLine, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
            Unicode::strcat(wszMonsterLine, CUNIT_GetUnitName(pUnit));
            Unicode::strcat(wszMonsterLine, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));

            if (pUnit->dwAnimMode == 12)
            {
                //  A corpse in town gets no label at all, and the font is left switched: the return at 0x6FB20E22 goes to the shared epilogue
                //  without a D2Win_10127_SetFont. Reproduced; the next caller of SetFont sets it again anyway.
                pRoom = D2COMMON_UNITS_GetRoom(pUnit);
                if (D2COMMON_DUNGEON_IsRoomInTown(pRoom))
                    return;

                bIsCorpse = 1;
                if (D2LANG_STRTABLE_GetLanguage())
                {
                    Unicode src[2];

                    //  BUG: copy-until-NUL of the paren into pXAdjust, a four-byte slot the perspective code has finished with; its next two
                    //  slots are live (the monster class id that the seven unique-colour comparisons test, and the font to restore). LATENT: both
                    //  source globals are exactly two bytes, "(" and ")" as wide characters, and nothing in the module writes either, so the
                    //  margin is exactly zero and never crossed. The buffer is declared the width the frame gives it, as the language-code slot
                    //  in UIPANELS_DrawInGamePanels is.
                    nParenIdx = 0;
                    do
                    {
                        chParen = gszAffixOpenParen[nParenIdx];
                        ((char *)&pXAdjust)[nParenIdx++] = chParen;
                    }
                    while (chParen);

                    Unicode::win2Unicode(src, (const char *)&pXAdjust, 2);
                    Unicode::strcat(wszMonsterLine, src);
                    Unicode::strcat(wszMonsterLine, D2LANG_GetStringFromTblIndex(STR_IDX_2315_Corpse));

                    nParenIdx = 0;
                    do
                    {
                        chParen = gszAffixCloseParen[nParenIdx];
                        ((char *)&pXAdjust)[nParenIdx++] = chParen;
                    }
                    while (chParen);

                    Unicode::win2Unicode(src, (const char *)&pXAdjust, 2);
                    pwszSrc = src;
                    Unicode::strcat(wszMonsterLine, pwszSrc);
                }
                else
                {
                    pwszSrc = D2LANG_GetStringFromTblIndex(STR_IDX_2315_Corpse);
                    Unicode::strcat(wszMonsterLine, pwszSrc);
                }

                Unicode::strcat(wszMonsterLine, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
            }

            Unicode::strcat(wszMonsterLine, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));

            nTextColor = 0;
            nLifeFill = UIPANELS_GetUnitLifeFill128(pUnit);
            nLabelColor = 0;

            //  Champion overrides unique, corpse overrides both, and the seven ids below override everything. In that order.
            if (MONSTER_IsUnique(pUnit))
            {
                nTextColor = 4;
                nLabelColor = 4;
            }
            if (MONSTER_IsChampion(pUnit))
            {
                nTextColor = 3;
                nLabelColor = 3;
            }
            if (bIsCorpse)
            {
                nTextColor = 1;
                nLabelColor = 1;
            }
            if (pNpcGUID == 243 || pNpcGUID == 333 || pNpcGUID == 242 || pNpcGUID == 211 || pNpcGUID == 544 || pNpcGUID == 570 || pNpcGUID == 229)
            {
                nTextColor = 4;
                nLabelColor = 4;
            }

            //  The monster line is centred in the play area at a fixed y=32, not over the unit: this is the banner across the top of the screen,
            //  not the floating label.
            if (!UI_GetPlayAreaBand(&pBand))
            {
                D2WIN_10127_SetFont((Font)pYAdjust);
                return;
            }

            D2WIN_10130_DrawFramedTextEx(wszMonsterLine, ((pBand.left + pBand.right) >> 1) - (D2WIN_10121_GetTextWidth(wszMonsterLine) >> 1), 32, nTextColor, 0, nLifeFill);

            if (bIsCorpse)
            {
                gwszUnitLabelLine1[0].ch = 0;
                D2WIN_10127_SetFont((Font)pYAdjust);
                return;
            }

            pMonStats = DATATBLS_GetMonStatsTxtRecord(pUnit->dwClassId);

            nStrId = STR_IDX_0_WarrivAct1IntroGossip1;
            if (D2COMMON_MONSTERS_IsDemon(pUnit))
                nStrId = STR_IDX_10078_DemonID;
            else if (D2COMMON_MONSTERS_IsUndead(pUnit))
                nStrId = STR_IDX_10077_UndeadDescriptX;

            if (!UIPANELS_ShouldDrawNameLabel(pUnit, nStrId))
            {
                if (nStrId)
                {
                    //  512 here. The object arm below constructs 256 under the same name; the two are different buffers.
                    Unicode wszObjectLabel[512];

                    Unicode::strcpy(wszObjectLabel, D2LANG_GetStringFromTblIndex(nStrId));
                    Unicode::strcat(wszObjectLabel, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));

                    //  5382 `dummy` is the no-string sentinel.
                    if (pMonStats->wDescStr != STR_IDX_5382_dummy)
                    {
                        Unicode::strcat(wszObjectLabel, D2LANG_GetStringFromTblIndex((D2C_StringIndices)pMonStats->wDescStr));
                    }
                    UIPANELS_SetUnitLabelText(wszObjectLabel, nLabelColor);
                }
                else if (pMonStats->wDescStr != STR_IDX_5382_dummy)
                {
                    UIPANELS_SetUnitLabelText(D2LANG_GetStringFromTblIndex((D2C_StringIndices)pMonStats->wDescStr), nLabelColor);
                }
            }

            UIPANELS_BuildMonsterImmunityLabel(pUnit);
            D2WIN_10127_SetFont((Font)pYAdjust);
            return;
        }

        //  Not a named monster: line 1 is cleared and the plain name is drawn where the unit is, raised by the record's pixel height.
        gwszUnitLabelLine1[0].ch = 0;
        if (NPCMENU_GetInteractingNpc(&pNpcGUID) || TEXT_IsPopupActive())
        {
            D2WIN_10127_SetFont((Font)nPrevFont);
        }
        else
        {
            const D2MonStats2Txt *pRec = DATATBLS_GetMonStats2TxtRecord(pUnit->dwClassId);
            if (pRec)
                gnLabelScreenY -= pRec->nPixHeight;

            D2WIN_10129_DrawFramedText(CUNIT_GetUnitName(pUnit), gnLabelScreenX, gnLabelScreenY, 0, 1);
            D2WIN_10127_SetFont((Font)pYAdjust);
        }
        return;
    }

    case UNIT_OBJECT:
    {
        //  Three buffers are constructed before the two refusals below, so a conversation in progress still pays for all of them. The
        //  declarations are what construct them, so they stay here rather than moving to the top of the function.
        //
        //  128 and 256, not the 200 and 512 the tile and monster arms use: those arms cannot run in the same call, so the compiler gave the
        //  pairs one stack slot each.
        Unicode wszWarpLabel[128];
        Unicode wszObjectLabel[256];
        Unicode wszLabel[128];

        wszLabel[0].ch = 0;
        memset(&wszLabel[1], 0, 127 * sizeof(Unicode));

        if (NPCMENU_GetInteractingNpc(&pNpcGUID) || TEXT_IsPopupActive())
        {
            return;
        }

        pwszName = CUNIT_GetUnitName(pUnit);
        nObjectId = pUnit->dwClassId;
        nNameOffset = D2COMMON_UNITS_GetNameOffsetFromObject(pUnit);
        //  `and edi, 0FFFFh`: the class id is truncated to 16 bits before the lookup, and the lookup itself is unchecked.
        nSubClass = (int8_t)D2COMMON_DATATBLS_GetObjectsTxtRecord((uint16_t)nObjectId)->nSubClass;

        if (nSubClass & 4)
        {
            //  A portal. Its label is the destination level's name plus, on battle.net, the owner's name in parentheses.
            Unicode wszText[128];

            nLevelId = pUnit->pObjectData->InteractType;
            pLevelsTxt = D2COMMON_DATATBLS_GetLevelsTxtRecord(nLevelId);
            if (!pLevelsTxt)
            {
                pwszSrc = D2LANG_GetStringFromTblIndex(STR_IDX_5389_strLevelLoadFailed);
            }
            else if (nLevelId != 8)
            {
                pwszSrc = pLevelsTxt->wszLevelName;
            }
            else
            {
                //  Level 8 is the Den of Evil, which is called Cave Level 1 until the quest is started.
                pwszSrc = D2LANG_GetStringFromTblIndex(QUESTS_GetQuestState(0, QUEST_A1Q1_DENOFEVIL) ? STR_IDX_5048_Den_of_Evil : STR_IDX_5047_Cave_Level_1);
            }

            Unicode::strcpy(wszLabel, pwszSrc);

            //  Two separate equality tests in the bytes, not one `>= 2`: `test eax,eax / jz` then `cmp eax,1 / jz`. Kept as written.
            if (gnGameType != GAMETYPE_SINGLEPLAYER && gnGameType != GAMETYPE_SINGLEPLAYER_UNCAPPED)
            {
                char *pszOwner = ROSTER_GetNameExByPortalGuid(pUnit->dwUnitId);
                if (!pszOwner && strlen(OBJECT_GetPortalOwnerName(pUnit)))
                {
                    //  Called twice on this arm, once for the strlen and again for the argument (0x6FB2130D and 0x6FB21322).
                    pszOwner = OBJECT_GetPortalOwnerName(pUnit);
                }
                if (pszOwner)
                {
                    sprintf(szPortalOwner, " (%s) ", pszOwner);
                    Unicode::win2Unicode(wszText, szPortalOwner, 128);
                    Unicode::strcat(wszLabel, wszText);
                }
            }

            D2WIN_10129_DrawFramedText(wszLabel, gnLabelScreenX, nNameOffset + gnLabelScreenY, 0, 1);
            return;
        }

        //  Bit 7 set means no label at all. Tested signed.
        if (nSubClass < 0)
            return;

        if (nSubClass & 0x40)
        {
            //  A waypoint: its own name, a space, a newline, then the level it stands in, read through the ASCII name rather than the Unicode one.
            pRoom = D2COMMON_UNITS_GetRoom(pUnit);
            if (pRoom)
            {
                pLevelsTxt = D2COMMON_DATATBLS_GetLevelsTxtRecord(D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom));

                Unicode::strcpy(wszObjectLabel, pwszName);
                Unicode::strcat(wszObjectLabel, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
                Unicode::strcat(wszObjectLabel, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
                Unicode::strcat(wszObjectLabel, (const Unicode *)D2LANG_GetStringByReferenceString((uint32_t)pLevelsTxt->szLevelName));

                D2WIN_10129_DrawFramedText(wszObjectLabel, gnLabelScreenX, nNameOffset + gnLabelScreenY, 0, 1);
            }
        }
        else
        {
            D2WIN_10129_DrawFramedText(pwszName, gnLabelScreenX, nNameOffset + gnLabelScreenY, 0, 1);
        }
        return;
    }

    case UNIT_ITEM:
        //  While ALT is held SHOWITEMS_DrawGroundItemLabels has already drawn every ground label, so the hovered one is suppressed to avoid
        //  drawing it twice.
        if (!gUiWindowStates[UI_HOLDALT])
            UIPANELS_BuildItemLabelText(pUnit, NULL, 0);
        return;

    case UNIT_TILE:
    {
        pRoom = D2COMMON_UNITS_GetRoom(pUnit);
        nLevelId = D2COMMON_DUNGEON_GetWarpDestinationLevel(pRoom, pUnit->dwClassId);
        pLevelsTxt = D2COMMON_DATATBLS_GetLevelsTxtRecord(nLevelId);
        if (!pLevelsTxt)
            return;

        pLvlWarp = D2COMMON_DUNGEON_GetLvlWarpTxtRecordFromRoomAndUnit(pRoom, pUnit);
        if (!pLvlWarp)
            return;

        //  The label sits at the centre of the warp's select box, which is why the two halves are shifted right by one before being added.
        //  Unsigned shifts in the bytes: shr, not sar.
        nWarpX = gnLabelScreenX + pLvlWarp->dwSelectX + (pLvlWarp->dwSelectDX >> 1);
        nWarpY = gnLabelScreenY + pLvlWarp->dwSelectY + (pLvlWarp->dwSelectDY >> 1);

        //  200 here, 128 in the object arm: two buffers, one slot. The loop runs 199 times from &wszWarpLabel[1] and the store does element 0,
        //  so this one is 199 + 1.
        Unicode wszWarpLabel[200];

        wszWarpLabel[0].ch = 0;
        memset(&wszWarpLabel[1], 0, 199 * sizeof(Unicode));

        //  wszLevelWarp, not wszLevelName
        pwszSrc = pLevelsTxt->wszLevelWarp;
        if (nLevelId == 8)
        {
            pwszSrc = D2LANG_GetStringFromTblIndex(QUESTS_GetQuestState(0, QUEST_A1Q1_DENOFEVIL) ? STR_IDX_5086_To_The_Den_of_Evil : STR_IDX_5085_To_The_Cave_Level_1);
        }

        Unicode::strncpy(wszWarpLabel, pwszSrc, 199);
        D2WIN_10129_DrawFramedText(wszWarpLabel, nWarpX, nWarpY, 0, 1);
        return;
    }

    default:
        return;
    }
}

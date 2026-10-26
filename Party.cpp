#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "CoreArchive.h"
#include "ui.h"
#include "Party.h"

//  Party.cpp -- the party panel of D2Client.dll 1.10: the panel draw, the row list, the hover hints and the two mouse handlers
//  (gPartyMessageHandlers). The original source file is UI\Party.cpp (the asserts in this unit name it). Twenty functions over
//  0x6FB01C70..0x6FB0405F, from PARTY_FreeRowList to PARTY_UnloadImages.
//
//  Panel space and screen space: every hit test is written in screen coordinates with the panel constant converted,
//      x:  screen = panel + gnBorderXDifference        (+80)
//      y:  screen = panel - gnBorderYDifference        (-60, stored negative, so the subtraction adds it back)
//  The two signs are opposite and that is correct.
//
//  The row list: gpPartyRowList is a singly-linked list of D2ClientPartyRowStrc, 209 bytes each, with five D2ClientPartyButtonStrc of 32 bytes at
//  the front. The original walks it with a raw `int *` stepping 32 bytes and reading negative offsets from the cursor; the C indexes
//  Buttons[iRow], the same addresses.
//  Buttons[i].field_00 is the button kind selector and decides the hit box: zero gives a 20-pixel-wide box, non-zero a 53-pixel one.
//  Buttons[i].nButtonType is the button's function: 0 the party button, 1 hostile, 2 loot, 3 ignore, 4 squelch (the switch in PARTY_OnLButtonUp).
//  nLevel (0xBD) is stored by PARTY_BuildRowList. nNameBandBottom (0xC9) is nRowY + 13, the bottom edge of the 104-by-26 band over the row's
//  name and class lines, whose hover shows the relation hint.
//
//  D2ClientPartyRowStrc is packed: nClassId at 0xB9 and pNext at 0xCD are byte offsets, which no aligned layout gives.
static_assert(sizeof(D2ClientPartyRowStrc) == 209
                  && offsetof(D2ClientPartyRowStrc, Buttons) == 0x00
                  && offsetof(D2ClientPartyRowStrc, szName) == 0xA0
                  && offsetof(D2ClientPartyRowStrc, nClassId) == 0xB9
                  && offsetof(D2ClientPartyRowStrc, nLevelId) == 0xC1
                  && offsetof(D2ClientPartyRowStrc, dwUnitId) == 0xC5
                  && offsetof(D2ClientPartyRowStrc, pNext) == 0xCD,
              "one row of the party list, packed; the five buttons come "
              "FIRST and the name is 25 bytes at 0xA0");
static_assert(sizeof(D2ClientPartyButtonStrc) == 32
                  && offsetof(D2ClientPartyButtonStrc, nX) == 0x04
                  && offsetof(D2ClientPartyButtonStrc, nY) == 0x08
                  && offsetof(D2ClientPartyButtonStrc, hImage) == 0x0C
                  && offsetof(D2ClientPartyButtonStrc, nFrame) == 0x10
                  && offsetof(D2ClientPartyButtonStrc, nButtonType) == 0x14
                  && offsetof(D2ClientPartyButtonStrc, eButton) == 0x18,
              "and one button of five, 32 bytes -- so Buttons[5] is exactly "
              "the 160 bytes szName starts after");
static_assert(UI_PARTYSCREEN == 22,
              "the party screen's panel id, as UI_SetUIToggle is given it");

//  The two row fields PARTY_BuildRowList stores whole, and the tbl strings the functions load as immediates.
static_assert(offsetof(D2ClientPartyRowStrc, nLevel) == 0xBD
                  && offsetof(D2ClientPartyRowStrc, nNameBandBottom) == 0xC9,
              "mov [esi+0BDh], eax at 0x6FB01E99 and mov [esi+0C9h], edx at "
              "0x6FB01D48");
static_assert(STR_IDX_4143_close == 0x102F && STR_IDX_4152_Leave == 0x1038,
              "PARTY_DrawPanel: mov ecx, 102Fh at 0x6FB0230D and mov ecx, 1038h "
              "at 0x6FB02238");
static_assert(STR_IDX_4154_partycharama == 0x103A
                  && STR_IDX_4155_partycharsor == 0x103B
                  && STR_IDX_4156_partycharbar == 0x103C
                  && STR_IDX_4157_partycharnec == 0x103D
                  && STR_IDX_4158_partycharpal == 0x103E
                  && STR_IDX_10097_Druid == 0x2771
                  && STR_IDX_10098_Assassin == 0x2772,
              "PARTY_DrawClassLabel's seven loads, 0x6FB025BC..0x6FB025E6 -- "
              "the barbarian's string is 4156 and the barbarian's class is 4");

//  PARTY_DrawButtonLabel's five captions, loaded as immediates.
static_assert(STR_IDX_4148_Yourparty == 0x1034 && STR_IDX_4149_Inparty == 0x1035
                  && STR_IDX_4150_Invite == 0x1036 && STR_IDX_4151_Accept == 0x1037
                  && STR_IDX_5103_CANCEL == 0x13EF,
              "PARTY_DrawButtonLabel: mov ecx, 1034h / 1035h / 1036h / 1037h / "
              "13EFh at 0x6FB02669, 0x6FB0267A, 0x6FB0268B, 0x6FB02692 and "
              "0x6FB026A0");

//  The seven hint strings PARTY_DrawControlButtonHints loads as immediates.
static_assert(STR_IDX_4100_strParty1 == 0x1004 && STR_IDX_4101_strParty2 == 0x1005
                  && STR_IDX_4102_strParty3 == 0x1006 && STR_IDX_4103_strParty4 == 0x1007
                  && STR_IDX_4104_strParty5 == 0x1008 && STR_IDX_4105_strParty6 == 0x1009
                  && STR_IDX_4120_strParty24 == 0x1018,
              "PARTY_DrawControlButtonHints: mov ecx, 1018h at 0x6FB02BBF; mov edx / "
              "mov ecx, 1005h / 1004h at 0x6FB02CC0 / 0x6FB02CC5, 1007h / 1006h "
              "at 0x6FB02DC9 / 0x6FB02DCE, 1009h / 1008h at 0x6FB02EDB / 0x6FB02EE0");

//  The seventeen click-hint strings PARTY_DrawHoverHint hands PARTY_DrawHintLines as immediates. The relation hints 10110..10121 are not here:
//  they are data, the four gaParty*Hint tables.
static_assert(STR_IDX_4106_strParty7 == 0x100A && STR_IDX_4107_strParty8 == 0x100B
                  && STR_IDX_4108_strParty9 == 0x100C && STR_IDX_4109_strParty10 == 0x100D
                  && STR_IDX_4110_strParty11 == 0x100E && STR_IDX_4111_strParty12 == 0x100F
                  && STR_IDX_4112_strParty13 == 0x1010 && STR_IDX_4113_strParty14 == 0x1011
                  && STR_IDX_4114_strParty15 == 0x1012 && STR_IDX_4115_strParty16 == 0x1013
                  && STR_IDX_4116_strParty17 == 0x1014 && STR_IDX_4117_strParty18 == 0x1015
                  && STR_IDX_4118_strParty19 == 0x1016 && STR_IDX_4119_strParty22 == 0x1017
                  && STR_IDX_4121_strParty25 == 0x1019 && STR_IDX_4122_StrParty26 == 0x101A
                  && STR_IDX_4123_StrParty27 == 0x101B,
              "PARTY_DrawHoverHint: the mov ecx / mov edx immediates of its nine "
              "PARTY_DrawHintLines calls, 1010h/1011h at 0x6FB034EB/0x6FB034E6 "
              "through 1016h/1017h at 0x6FB0369F/0x6FB03698");

//  The roster enums this file spells by name where the original carries the number.
static_assert(ROSTERBUTTONTYPE_HOSTILE == 1 && ROSTERBUTTONTYPE_LOOT == 2
                  && ROSTERBUTTONTYPE_IGNORE == 3 && ROSTERBUTTONTYPE_SQUELCH == 4,
              "PARTY_BuildRowList: mov dword ptr [esi+34h] / [esi+54h] / [esi+74h] / "
              "[esi+94h], 1 / 2 / 3 / 4 at 0x6FB01DA5, 0x6FB01DDD, 0x6FB01E0E and "
              "0x6FB01E51");
static_assert(PARTYBUTTON_INVITE == 0 && PARTYBUTTON_ACCEPT == 2 && PARTYBUTTON_LEAVE == 3
                  && PARTYBUTTON_CANCEL == 4,
              "PARTY_DrawButtonLabel's jump table at 0x6FB026E0: slot 0 loads 1036h "
              "(0x6FB0268B), 2 1037h, 3 1038h, 4 13EFh");
static_assert(ROSTERINFOFLAG_HOSTILE == 8,
              "PARTY_DrawRowButtons: push 8 at 0x6FB02817 for PLAYERLIST_RosterTestInfoFlag");


//  The path literal the one assert pushes. In the original it is __FILE__, a single object at 0x6FB7D008.
static const char szPartyCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UI\\Party.cpp";

//  Two command prefixes at 0x6FB7D080 and 0x6FB7D074, both with a trailing space. The second is Party.h's gszUnsquelchCommand; the first has no
//  global and stays a literal here.
static const char szSquelchCommand[] = "/squelch ";

//1.10f: D2Client.0x6FB01C70
//  Frees the row list and nulls its head. Each row's next pointer is read before its FOG_Free. Called by UI_UnregisterPanelHandlers when the
//  party screen closes.
void __cdecl PARTY_FreeRowList()
{
    D2ClientPartyRowStrc *pRow;
    D2ClientPartyRowStrc *pNext;

    pRow = gpPartyRowList;
    while (pRow)
    {
        pNext = pRow->pNext;
        FOG_Free(pRow, szPartyCpp, 264, 0);
        pRow = pNext;
    }
    gpPartyRowList = NULL;
}

//1.10f: D2Client.0x6FB01CB0
//  Rebuilds the row list from the roster: one D2ClientPartyRowStrc per player but the first (the local player, whose name is the panel's title).
//  The walk starts at the roster's second entry. Called whenever the roster changes and by UI_RegisterPanelHandlers when the party screen opens.
//  The original inlines PARTY_FreeRowList at the start.
//  nRowY starts at 90 and steps 38 a row, in panel space, and every button is placed against it. Button 0 (the party button) takes
//  gpPartyButtonsImage and, in eButton, ROSTER_EntryGetPartyButton; the four boxes take gpPartyBoxesImage and eButton 0. The frame of each box is
//  the lower or the higher of a pair by its predicate (hostile 4/6, loot 12/14, ignore 0/2, squelch 8/10), and any non-zero answer counts as true.
//  The row is zeroed first (209 bytes, the whole record) and appended at the tail, found by walking from the head each time; it becomes
//  gpPartyRowList only when that head was NULL.
//  D2ExpRes patches seven sites here, each marked on its line.
void __cdecl PARTY_BuildRowList()
{
    D2ClientPartyRowStrc *pRow;
    D2ClientPartyRowStrc *pHead;
    D2RosterUnitStrc *pEntry;
    D2C_PartyButtons eButton;
    int32_t bOn;
    int32_t nRowY;
    int32_t nRow;

    PARTY_FreeRowList();

    //  D2ExpRes PartyAllButtons1 lands on the first row's y
    nRowY = 90;
    pEntry = ROSTER_PartyGetNextEntry(ROSTER_PartyGetNextEntry(NULL));

    //  BUG: the loop runs while `pEntry || nRow > 8` (`cmp [nRow], 8 / jle`, 0x6FB01D05, signed). Past the roster's last entry a row count above 8
    //  would keep it going with pEntry NULL, and ROSTER_PartyGetNextEntry(NULL) is the roster's head, so the walk would start over instead of ending.
    //  LATENT: nRow cannot pass 8: a roster of N entries gives N - 1 rows, and the roster never holds more than eight.
    for (nRow = 0; pEntry || nRow > 8; ++nRow)
    {
        pHead = gpPartyRowList;
        pRow = (D2ClientPartyRowStrc *)FOG_Alloc(sizeof(D2ClientPartyRowStrc), szPartyCpp, 333, 0);
        memset(pRow, 0, sizeof(D2ClientPartyRowStrc));
        pRow->nNameBandBottom = nRowY + 13;

        eButton = ROSTER_EntryGetPartyButton(pEntry);
        pRow->Buttons[0].field_00 = 1;
        //  D2ExpRes PartyInviteButton lands on this x
        pRow->Buttons[0].nX = 190;
        pRow->Buttons[0].nY = nRowY;
        pRow->Buttons[0].hImage = gpPartyButtonsImage;
        pRow->Buttons[0].nFrame = 0;
        pRow->Buttons[0].nButtonType = 0;
        pRow->Buttons[0].eButton = eButton;

        bOn = ROSTER_EntryIsHostile(pEntry);
        pRow->Buttons[1].field_00 = 0;
        //  D2ExpRes PartyHostileButton lands on this x
        pRow->Buttons[1].nX = 14;
        pRow->Buttons[1].nY = nRowY + 7;
        pRow->Buttons[1].hImage = gpPartyBoxesImage;
        pRow->Buttons[1].nFrame = bOn ? 6 : 4;
        pRow->Buttons[1].nButtonType = ROSTERBUTTONTYPE_HOSTILE;
        //  the bytes store 0 in each box's eButton: a box carries no party button (0 is PARTYBUTTON_INVITE only by value)
        pRow->Buttons[1].eButton = (D2C_PartyButtons)0;

        bOn = ROSTER_EntryAllowsLoot(pEntry);
        pRow->Buttons[2].field_00 = 0;
        //  D2ExpRes PartyLootCorpsesButton lands on this x
        pRow->Buttons[2].nX = 245;
        pRow->Buttons[2].nY = nRowY;
        pRow->Buttons[2].hImage = gpPartyBoxesImage;
        pRow->Buttons[2].nFrame = bOn ? 14 : 12;
        pRow->Buttons[2].nButtonType = ROSTERBUTTONTYPE_LOOT;
        pRow->Buttons[2].eButton = (D2C_PartyButtons)0;

        bOn = ROSTER_EntryIsIgnored(pEntry);
        pRow->Buttons[3].field_00 = 0;
        //  D2ExpRes PartyHearButton lands on this x
        pRow->Buttons[3].nX = 266;
        pRow->Buttons[3].nY = nRowY;
        pRow->Buttons[3].hImage = gpPartyBoxesImage;
        pRow->Buttons[3].nFrame = bOn ? 2 : 0;
        pRow->Buttons[3].nButtonType = ROSTERBUTTONTYPE_IGNORE;
        pRow->Buttons[3].eButton = (D2C_PartyButtons)0;

        bOn = ROSTER_EntryIsSquelched(pEntry);
        pRow->Buttons[4].field_00 = 0;
        //  D2ExpRes PartySquelchButton lands on this x
        pRow->Buttons[4].nX = 287;
        pRow->Buttons[4].nY = nRowY;
        pRow->Buttons[4].hImage = gpPartyBoxesImage;
        pRow->Buttons[4].nFrame = bOn ? 10 : 8;
        pRow->Buttons[4].nButtonType = ROSTERBUTTONTYPE_SQUELCH;
        pRow->Buttons[4].eButton = (D2C_PartyButtons)0;

        pRow->dwUnitId = ROSTER_GetEntryUnitId(pEntry);
        //  BUG: the name is copied with no bound (a byte loop, the inlined strcpy, 0x6FB01E79..0x6FB01E81) into the 25-byte szName.
        //  LATENT: the source is the entry's 26-byte name, "%s {%s}" or the bare name, which comes to at most 22 characters, 23 bytes with the
        //  terminator.
        strcpy(pRow->szName, ROSTER_GetDisplayName(pEntry));
        pRow->nClassId = ROSTER_EntryGetClassId(pEntry);
        //  zero-extended: ROSTER_EntryGetLevel clears eax before its word load (`xor eax, eax`), and answers 0 for a NULL entry
        pRow->nLevel = ROSTER_EntryGetLevel(pEntry);
        pRow->nLevelId = ROSTER_GetEntryLevelId(pEntry);

        if (pHead)
        {
            while (pHead->pNext)
            {
                pHead = pHead->pNext;
            }
            pHead->pNext = pRow;
        }
        else
        {
            gpPartyRowList = pRow;
        }

        //  D2ExpRes PartyAllButtons2 lands on the row pitch
        nRowY += 38;
        pEntry = ROSTER_PartyGetNextEntry(pEntry);
    }
}

//1.10f: D2Client.0x6FB01F10
//  Draws the party panel once a frame while it is open (UIPANELS_DrawInGamePanels calls it under gUiWindowStates[UI_PARTYSCREEN]). With dx and dy
//  for gnBorderXDifference and gnBorderYDifference, in order: the frame (cels 0..3 of gpPartyPanelImage as a 2x2 block), the title (the local
//  player's name, centred between dx + 20 and dx + 190), at most eight rows, the LEAVE button and caption while the local player is in a party,
//  the close button (cel 10 of UI_GetBuySellButtonImage, 11 while gbPartyButtonDown), the hints and the dividers between party groups, and the
//  "close" tooltip.
//  The title's font follows the name's length, compared unsigned: up to 11 the font is not set at all, 12 and 13 set Font8, 14 and more Font6;
//  Font8 is set after the draw either way.
//  The row colour is the row's relation to the local player: 0 in no party (a shared party id of 0xFFFF), 2 in the local player's party, 8 in
//  another party, 1 when ROSTER_EntryIsHostileToPlayer (tested last, it overrides the rest). The party-id tests are WORD compares, because
//  ROSTER_PartyGetSharedPartyId answers in AX alone. Only a row of the local player's own party gets PARTY_DrawAreaLabel, and only when its
//  nLevelId is set.
//  PARTY_DrawRowButtons is handed nRowY and never reads it (nUnusedY), and PARTY_DrawButtonLabel's first argument, the row's roster entry, is
//  never read either (pUnusedEntry); the original loads both for the calls.
//  A ten-element Unicode array is constructed (0x6FB020B3..0x6FB020C2) and never used; `Unicode awszUnused[10]` makes MSVC construct it the same
//  way.
//  D2ExpRes patches nine sites here, each marked on its line.
void __cdecl PARTY_DrawPanel()
{
    D2GfxDataStrc tCelData;
    D2UnitStrc *pPlayer;
    char *szTitle;
    uint32_t nCount;
    Unicode *pwszTitle;
    D2ClientPartyRowStrc *pRow;
    D2RosterUnitStrc *pMyEntry;
    D2RosterUnitStrc *pEntry;
    D2RosterUnitStrc *pFirstEntry;
    int32_t nRowY;
    int32_t nColor;
    int32_t nRowsDrawn;
    int32_t bInMyParty;
    int16_t nMyPartyId;
    int16_t nPartyId;

    memset(&tCelData, 0, sizeof(tCelData));
    tCelData.pCellFile = gpPartyPanelImage;
    tCelData.nDirection = 0;
    tCelData.nFrame = 0;
    D2GFX_TEXTURE_CelDraw(&tCelData, gnBorderXDifference, gnScreenHeight + gnBorderYDifference - 224, 0xFFFFFFFF, 5, NULL);
    tCelData.nFrame = 1;
    D2GFX_TEXTURE_CelDraw(&tCelData, gnBorderXDifference + 256, gnScreenHeight + gnBorderYDifference - 224, 0xFFFFFFFF, 5, NULL);
    tCelData.nFrame = 2;
    D2GFX_TEXTURE_CelDraw(&tCelData, gnBorderXDifference, gnScreenHeight + gnBorderYDifference - 48, 0xFFFFFFFF, 5, NULL);
    tCelData.nFrame = 3;
    D2GFX_TEXTURE_CelDraw(&tCelData, gnBorderXDifference + 256, gnScreenHeight + gnBorderYDifference - 48, 0xFFFFFFFF, 5, NULL);

    pPlayer = CUNIT_GetPlayer();
    if (pPlayer)
    {
        szTitle = ROSTER_GetPlayerDisplayName(pPlayer);
        if (szTitle)
        {
            nCount = Unicode::unicodenwidth(szTitle, (int32_t)strlen(szTitle)) + 1;
            if (nCount > 11)
            {
                D2WIN_10127_SetFont(nCount < 14 ? D2FONT_FONT8 : D2FONT_FONT6);
            }
            pwszTitle = (Unicode *)FOG_Alloc(2 * nCount, szPartyCpp, 529, 0);
            Unicode::toUnicode(pwszTitle, szTitle, (int32_t)nCount);
            //  D2ExpRes PartyOwnCharname lands on the right edge
            CHAR_DrawTextCentered(gnBorderXDifference + 20, 31 - gnBorderYDifference, gnBorderXDifference + 190, pwszTitle, 0);
            D2WIN_10127_SetFont(D2FONT_FONT8);
            FOG_Free(pwszTitle, szPartyCpp, 538, 0);
        }
    }

    pRow = gpPartyRowList;
    //  D2ExpRes PartyOtherStrings2 lands on the first row's y
    nRowY = 90;
    Unicode awszUnused[10];
    (void)awszUnused;       //  constructed and never read
    pMyEntry = ROSTER_PartyGetNextEntry(NULL);
    nMyPartyId = ROSTER_PartyGetSharedPartyId(pMyEntry);
    pEntry = ROSTER_PartyGetNextEntry(pMyEntry);
    nRowsDrawn = 0;
    while (pRow)
    {
        nColor = 0;
        if (!pEntry || nRowsDrawn >= 8)
        {
            break;
        }

        bInMyParty = 0;
        nPartyId = ROSTER_PartyGetSharedPartyId(pEntry);
        if ((uint16_t)ROSTER_PartyGetSharedPartyId(pEntry) != 0xFFFF)
        {
            if (nMyPartyId == nPartyId)
            {
                nColor = 2;
                bInMyParty = 1;
            }
            else
            {
                nColor = 8;
            }
        }
        if (ROSTER_EntryIsHostileToPlayer(pEntry))
        {
            nColor = 1;
        }

        D2WIN_10127_SetFont(D2FONT_FONT8);
        PARTY_DrawNameLabel(pRow->szName, nRowY, nColor, nPartyId != -1);
        //  D2ExpRes PartyClassStringY lands on this y
        PARTY_DrawClassLabel(pRow->nClassId, nRowY + 12, nColor);
        //  D2ExpRes PartyLvlStringY lands on this y
        PARTY_DrawLevelLabel(pRow->nLevel, nRowY + 12, nColor);
        if (bInMyParty && pRow->nLevelId)
        {
            PARTY_DrawAreaLabel(pRow, nRowY, nColor);
        }
        PARTY_DrawRowButtons(pRow, nRowY);
        PARTY_DrawButtonLabel(pEntry, pRow->Buttons[0].eButton, nRowY, nMyPartyId == nPartyId, nPartyId);

        pRow = pRow->pNext;
        //  D2ExpRes PartyOtherStrings1 lands on the row pitch
        nRowY += 38;
        ++nRowsDrawn;
        pEntry = ROSTER_PartyGetNextEntry(pEntry);
        D2WIN_10127_SetFont(D2FONT_FONT16);
    }

    pFirstEntry = ROSTER_PartyGetNextEntry(NULL);
    D2WIN_10127_SetFont(D2FONT_FONT8);
    if ((uint16_t)ROSTER_PartyGetSharedPartyId(pFirstEntry) != 0xFFFF)
    {
        //  D2ExpRes PartyLeaveButton lands on this cel
        PARTY_DrawCel(gpPartyButtonsImage, 227, 35, 0);
        //  D2ExpRes PartyLeaveString lands on this text
        CHAR_DrawTextCentered(gnBorderXDifference + 227, 33 - gnBorderYDifference, gnBorderXDifference + 280, D2LANG_GetStringFromTblIndex(STR_IDX_4152_Leave), 2);
    }
    D2WIN_10127_SetFont(D2FONT_FONT16);

    memset(&tCelData, 0, sizeof(tCelData));
    tCelData.pCellFile = UI_GetBuySellButtonImage();
    tCelData.nFrame = (gbPartyButtonDown != 0) + 10;
    tCelData.nDirection = 0;
    //  D2ExpRes PartyCloseButton lands on this cel
    D2GFX_TEXTURE_CelDraw(&tCelData, gnBorderXDifference + 278, 421 - gnBorderYDifference, 0xFFFFFFFF, 5, NULL);

    PARTY_DrawControlButtonHints();
    PARTY_DrawHoverHint();
    PARTY_DrawPartyGroupDividers();
    if (PARTY_IsMouseOverRect(278, 421, 32, 32))
    {
        //  D2ExpRes PartyCloseString lands on this text
        D2WIN_10129_DrawFramedText(D2LANG_GetStringFromTblIndex(STR_IDX_4143_close), gnBorderXDifference + 293, 386 - gnBorderYDifference, 0, 1);
    }
}

//1.10f: D2Client.0x6FB02330
//  Draws one cel at a panel position, (nX + dx, nY - dy), white, draw mode 5. One caller: PARTY_DrawPanel's LEAVE button.
void __fastcall PARTY_DrawCel(D2CellFileStrc *hImage, int32_t nX, int32_t nY, int32_t nFrame)
{
    D2GfxDataStrc tCelData;

    memset(&tCelData, 0, sizeof(tCelData));
    tCelData.nDirection = 0;
    tCelData.nFrame = nFrame;
    tCelData.pCellFile = hImage;
    D2GFX_TEXTURE_CelDraw(&tCelData, gnBorderXDifference + nX, nY - gnBorderYDifference, 0xFFFFFFFF, 5, NULL);
}

//1.10f: D2Client.0x6FB02390
//  Draws a row's level, "Lvl: NN", at (dx + 266, nY - dy). Called by PARTY_DrawPanel with the row's nLevel and nRowY + 12.
//  Four pieces, not a format string: tbl 3368 ("Lvl") copied with Unicode::strncpy capped at 20, 3997 (":") and 3995 (" ") appended, then
//  wsprintfA "%2u" into a char buffer, Unicode::win2Unicode of at most 9, and one more strcat. The two Unicode buffers are constructed element by
//  element, 100 and then 10, which `Unicode wszLabel[100]` and `wszNumber[10]`, declared in that order, make MSVC do too.
void __fastcall PARTY_DrawLevelLabel(int32_t nLevel, int32_t nY, int32_t nColor)
{
    Unicode wszLabel[100];
    Unicode wszNumber[10];
    char szNumber[100];

    Unicode::strncpy(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_3368_Level), 20);
    Unicode::strcat(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_3997_colon));
    Unicode::strcat(wszLabel, D2LANG_GetStringFromTblIndex(STR_IDX_3995_space));
    wsprintfA(szNumber, "%2u", nLevel);
    Unicode::win2Unicode(wszNumber, szNumber, 9);
    Unicode::strcat(wszLabel, wszNumber);
    //  D2ExpRes PartyLvlStringX lands on this x
    D2WIN_10117_DrawText(wszLabel, gnBorderXDifference + 266, nY - gnBorderYDifference, nColor, 0);
}

//1.10f: D2Client.0x6FB02480
//  Draws a row's name at (dx + 36, nY - dy - 2), cut to fit and ended with "...". Called by PARTY_DrawPanel.
//  The width budget is chosen by bInParty, the fourth argument: 95 pixels for a player in any party, 135 for one in none. nColor only colours
//  the text.
//  The name goes into a 100-element Unicode buffer. The cursor starts at &wszName[99] and steps down before each store, so the zeroing runs from
//  wszName[98] downward, re-measuring after each, until the width fits. The "..." is appended afterwards and its own width is not budgeted: a cut
//  name ends up past the budget by up to the ellipsis. `char szDots[5] = "..."` is what MSVC emits for the bytes' four-byte copy and a zeroed
//  fifth.
void __fastcall PARTY_DrawNameLabel(const char *pszName, int32_t nY, int32_t nColor, int32_t bInParty)
{
    Unicode wszName[100];
    Unicode::toUnicode(wszName, pszName, 100);
    Unicode *pCut = &wszName[99];
    char szDots[5] = "...";
    Unicode wszDots[5];

    if (bInParty)
    {
        if (D2WIN_10121_GetTextWidth(wszName) > 95)
        {
            while (D2WIN_10121_GetTextWidth(wszName) > 95)
            {
                --pCut;
                pCut->ch = 0;
            }
            Unicode::toUnicode(wszDots, szDots, 5);
            Unicode::strcat(wszName, wszDots);
        }
    }
    else if (D2WIN_10121_GetTextWidth(wszName) > 135)
    {
        while (D2WIN_10121_GetTextWidth(wszName) > 135)
        {
            --pCut;
            pCut->ch = 0;
        }
        Unicode::toUnicode(wszDots, szDots, 5);
        Unicode::strcat(wszName, wszDots);
    }

    //  D2ExpRes PartyOtherCharnames lands on this x
    D2WIN_10117_DrawText(wszName, gnBorderXDifference + 36, nY - gnBorderYDifference - 2, nColor, 0);
}

//1.10f: D2Client.0x6FB025B0
//  Draws a row's class at (dx + 36, nY - dy): the name's x, and PARTY_DrawPanel passes nRowY + 12 where the name drew at nRowY - 2, 14 pixels
//  lower.
//  A switch from class to tbl string, and the classic five are not in class order in the table: 4154 Amazon, 4155 Sorceress, 4156 Barbarian,
//  4157 Necromancer, 4158 Paladin, against the class ids 0 Amazon, 1 Sorceress, 2 Necromancer, 3 Paladin, 4 Barbarian. The expansion two take
//  their plain names, 10097 Druid and 10098 Assassin. The bound is `cmp ecx, 6 / ja` (0x6FB025B0), unsigned, so a class outside 0..6, a
//  negative one included, draws nothing.
void __fastcall PARTY_DrawClassLabel(int32_t nCharClass, int32_t nY, int32_t nColor)
{
    D2C_StringIndices nStringId;

    switch (nCharClass)
    {
    case PCLASS_AMAZON:
        nStringId = STR_IDX_4154_partycharama;
        break;
    case PCLASS_SORCERESS:
        nStringId = STR_IDX_4155_partycharsor;
        break;
    case PCLASS_NECROMANCER:
        nStringId = STR_IDX_4157_partycharnec;
        break;
    case PCLASS_PALADIN:
        nStringId = STR_IDX_4158_partycharpal;
        break;
    case PCLASS_BARBARIAN:
        nStringId = STR_IDX_4156_partycharbar;
        break;
    case PCLASS_DRUID:
        nStringId = STR_IDX_10097_Druid;
        break;
    case PCLASS_ASSASSIN:
        nStringId = STR_IDX_10098_Assassin;
        break;
    default:
        return;
    }

    //  D2ExpRes PartyClassStringX lands on this x
    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(nStringId), gnBorderXDifference + 36, nY - gnBorderYDifference, nColor, 0);
}

//1.10f: D2Client.0x6FB02640
//  Draws the caption of a row's first button, centred across 53 pixels from x = dx + 190 (or the offset below), at nY - dy - 2. Called by
//  PARTY_DrawPanel, which passes the row's Buttons[0].eButton as the kind:
//      kind   tbl                          x      colour
//      0      4150 "INVITE"                190    0
//      1      4148 "In your party"         155    2      bSameParty
//             4149 "In a party"            175    8      otherwise
//      2      4151 "ACCEPT"                190    0
//      3      4152 "LEAVE"                 190    0
//      4      5103 "CANCEL"                190    0
//      other  SetFont(Font8), nothing drawn
//  The switch is written on the D2C_PartyButtons names; its jump table is in that order. The bound is `cmp edx, 4 / ja` (0x6FB02643): unsigned, so
//  a negative kind takes the default arm, the only one that touches the font. Kind 1 returns without drawing and without that SetFont when
//  nPartyId is 0xFFFF, a WORD compare (`cmp word ptr [esp+10h], 0FFFFh`, 0x6FB02658), where nPartyId is the fifth argument. bSameParty is the
//  caller's `nMyPartyId == nPartyId`. pUnusedEntry is the row's roster entry, loaded into ecx at the call and never read.
//  D2ExpRes patches three sites here, each marked on its line.
void __fastcall PARTY_DrawButtonLabel(D2RosterUnitStrc *pUnusedEntry, D2C_PartyButtons nButtonKind, int32_t nY, int32_t bSameParty, int16_t nPartyId)
{
    D2C_StringIndices nStringId;
    int32_t nColor;
    int32_t nX;
    int32_t nLeft;

    (void)pUnusedEntry;
    nColor = 0;
    //  D2ExpRes PartyInviteCancelString lands on the default x
    nX = 190;
    switch (nButtonKind)
    {
    case PARTYBUTTON_INVITE:
        nStringId = STR_IDX_4150_Invite;
        break;
    case PARTYBUTTON_NONE:
        if (nPartyId == -1)
        {
            return;
        }
        if (bSameParty)
        {
            nStringId = STR_IDX_4148_Yourparty;
            //  D2ExpRes PartyYourPartyString lands on this colour
            nColor = 2;
            nX = 155;
        }
        else
        {
            nStringId = STR_IDX_4149_Inparty;
            //  D2ExpRes PartyAPartyString lands on this colour
            nColor = 8;
            nX = 175;
        }
        break;
    case PARTYBUTTON_ACCEPT:
        nStringId = STR_IDX_4151_Accept;
        break;
    case PARTYBUTTON_LEAVE:
        nStringId = STR_IDX_4152_Leave;
        break;
    case PARTYBUTTON_CANCEL:
        nStringId = STR_IDX_5103_CANCEL;
        break;
    default:
        D2WIN_10127_SetFont(D2FONT_FONT8);
        return;
    }

    nLeft = gnBorderXDifference + nX;
    CHAR_DrawTextCentered(nLeft, nY - gnBorderYDifference - 2, nLeft + 53, D2LANG_GetStringFromTblIndex(nStringId), nColor);
}

//1.10f: D2Client.0x6FB02700
//  Draws a row's five button cels, and a red box over its hostile button when that button is refused. Called by PARTY_DrawPanel once a row.
//  The first button (nButtonType 0, the party button) is not drawn while its eButton is 1 (PARTYBUTTON_NONE, the kind PARTY_DrawButtonLabel draws
//  as a party status instead of a caption). The four boxes are always drawn, except the loot box in a softcore game (WINMAIN_IsHardcoreGame),
//  where PARTY_OnLButtonUp's loot arm does nothing either.
//  Before the draw each box's frame is clamped into its own four-cel range, in the cel data only (the button keeps its frame). The compares are
//  unsigned on an int field, so the C compares an unsigned copy: hostile 4..7 (out of range gives 4), loot 12..15 (12), ignore 0..3 (0), squelch
//  8..11 (8). In softcore the loot box's clamp is still made; only its draw is skipped.
//  The red box: after its cel, the hostile button (nButtonType re-read from the button) gets a 20x20 box at (dx + nX, nY - dy - 19) in draw mode
//  1 when any of three holds: 1. the local player is not in a town room; 2. the row's player is below level 9 and
//  PLAYERLIST_RosterTestInfoFlag(the local player's id, the row's dwUnitId, ROSTERINFOFLAG_HOSTILE) answers 0; 3. the roster's head, the local
//  player, is below level 9. The press handlers refuse the hostile press on 1 and 3; 2 is tested here only.
//  The colour is D2WIN_10034_MixRGB(255, 0, 0)'s byte, stored in gnUiBoxFillColor and passed to GFX_DrawFilledRect.
//  nUnusedY is never read: edx is first written at 0x6FB027B5.
void __fastcall PARTY_DrawRowButtons(D2ClientPartyRowStrc *pRow, int32_t nUnusedY)
{
    D2GfxDataStrc tCelData;
    D2UnitStrc *pPlayer;
    D2ClientPartyButtonStrc *pButton;
    D2ActiveRoomStrc *pRoom;
    uint32_t nFrame;
    uint8_t nColor;
    int32_t nKind;
    int32_t iButton;
    int32_t bDisabled;

    (void)nUnusedY;
    pPlayer = CUNIT_GetPlayer();
    memset(&tCelData, 0, sizeof(tCelData));
    tCelData.nDirection = 0;

    for (iButton = 0; iButton < 5; ++iButton)
    {
        pButton = &pRow->Buttons[iButton];
        nKind = pButton->nButtonType;
        if (nKind || pButton->eButton != PARTYBUTTON_NONE)
        {
            tCelData.pCellFile = pButton->hImage;
            nFrame = (uint32_t)pButton->nFrame;
            tCelData.nFrame = nFrame;
            switch (nKind)
            {
            case ROSTERBUTTONTYPE_HOSTILE:
                if (nFrame < 4 || nFrame > 7)
                {
                    tCelData.nFrame = 4;
                }
                break;
            case ROSTERBUTTONTYPE_LOOT:
                if (nFrame < 12 || nFrame > 15)
                {
                    tCelData.nFrame = 12;
                }
                //  softcore: clamped, and not drawn
                if (!WINMAIN_IsHardcoreGame())
                {
                    goto HOSTILE_BOX;
                }
                break;
            case ROSTERBUTTONTYPE_IGNORE:
                if (nFrame > 3)
                {
                    tCelData.nFrame = 0;
                }
                break;
            case ROSTERBUTTONTYPE_SQUELCH:
                if (nFrame < 8 || nFrame > 11)
                {
                    tCelData.nFrame = 8;
                }
                break;
            }
            D2GFX_TEXTURE_CelDraw(&tCelData, gnBorderXDifference + pButton->nX, pButton->nY - gnBorderYDifference, 0xFFFFFFFF, 5, NULL);
        }

    HOSTILE_BOX:
        if (pButton->nButtonType == ROSTERBUTTONTYPE_HOSTILE)
        {
            bDisabled = 0;
            pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
            if (!pRoom || !D2COMMON_DUNGEON_IsRoomInTown(pRoom))
            {
                bDisabled = 1;
            }
            if (pRow->nLevel < 9)
            {
                //  BUG: the -1 for a NULL player (`or ecx, 0FFFFFFFFh`, 0x6FB02809) is dead. UNREACHABLE: UNITS_GetRoom has already asserted the
                //  pointer, and pPlayer is never NULL here (the caller chain, PARTY_DrawPanel from UIPANELS_DrawInGamePanels from GAME_DrawGameScene,
                //  hands the player to UNITS_GetClientCoordX earlier in the same frame, which asserts it the same way).
                //  ROSTERINFOFLAG_HOSTILE is the original's 8.
                if (!PLAYERLIST_RosterTestInfoFlag(pPlayer ? pPlayer->dwUnitId : 0xFFFFFFFF, pRow->dwUnitId, ROSTERINFOFLAG_HOSTILE))
                {
                    bDisabled = 1;
                }
            }
            //  zero-extended, as in PARTY_BuildRowList
            if (ROSTER_EntryGetLevel(ROSTER_PartyGetNextEntry(NULL)) < 9 || bDisabled)
            {
                //  DIFFERENCE: the original pushes MixRGB's whole eax, 0x100 | index (D2CMP leaves its palette size in eax); ours passes the
                //  byte. Every render driver masks the colour to its byte, so nothing reads the difference.
                nColor = D2WIN_10034_MixRGB(0xFF, 0, 0);
                gnUiBoxFillColor = nColor;
                GFX_DrawFilledRect(gnBorderXDifference + pButton->nX, pButton->nY - gnBorderYDifference - 19, 20, 20, nColor, DRAWMODE_TRANS50);
            }
        }
    }
}

//1.10f: D2Client.0x6FB028A0
//  Draws the area a party member is in, at (dx + 126, nY - dy + 12): the level name of the row's nLevelId (UI_LevelsGetLevelName). Called by
//  PARTY_DrawPanel only for a row of the local player's own party whose nLevelId is set.
//  A name wider than 140 pixels is cut and keeps its last letter. The name goes into a 255-element Unicode buffer. When GetTextWidth answers more
//  than 140 (`cmp eax, 8Ch / jle`, 0x6FB028F0): 1. the name's last character is read before anything is cut; 2. the suffix is made from "....",
//  four dots and a NUL, converted into a 5-element Unicode buffer, and its fourth dot is overwritten with that last character; 3. the budget is
//  139 minus the suffix's width, and the name is cut from wszText[253] downward, one character zeroed and the whole string measured again after
//  each, until it fits; 4. the suffix is appended. So a cut name ends in "..." and its own last letter, which keeps a trailing number on screen;
//  whether it was meant so the bytes cannot say, and nothing here is a defect. The cursor starts at &wszText[254] and steps down before each
//  store, so wszText[254] is never written and the first stores land on the terminator or past it, the same shape as PARTY_DrawNameLabel's cut.
//  The declarations are the extents the construction loops give: 255 and 5.
//  D2ExpRes patches one site here, marked on its line.
void __fastcall PARTY_DrawAreaLabel(D2ClientPartyRowStrc *pRow, int32_t nY, int32_t nColor)
{
    Unicode wszText[255];
    Unicode::strcpy(wszText, (const Unicode *)(UINT_PTR)UI_LevelsGetLevelName(pRow->nLevelId));
    Unicode *pCut = &wszText[254];

    if (D2WIN_10121_GetTextWidth(wszText) > 140)
    {
        uint16_t chLast = wszText[Unicode::strlen(wszText) - 1].ch;
        char szDots[5] = "....";
        Unicode wszDots[5];
        int32_t nBudget;

        Unicode::toUnicode(wszDots, szDots, 5);
        wszDots[3].ch = chLast;
        nBudget = 139 - D2WIN_10121_GetTextWidth(wszDots);
        while (D2WIN_10121_GetTextWidth(wszText) > nBudget)
        {
            --pCut;
            pCut->ch = 0;
        }
        Unicode::strcat(wszText, wszDots);
    }

    //  D2ExpRes PartyLocation lands on this x
    D2WIN_10117_DrawText(wszText, gnBorderXDifference + 126, nY - gnBorderYDifference + 12, nColor, 0);
}

//1.10f: D2Client.0x6FB029D0
//  Draws the bars between parties, after the rows. Called by PARTY_DrawPanel.
//  gnPartyRowPartyIds[8] is filled with 0xFFFF, and then the roster is walked from its head (the local player first, the entry
//  PARTY_BuildRowList skips) and each entry's ROSTER_PartyGetSharedPartyId, masked to its word, goes into the next slot: slot i is roster entry i,
//  and the panel's row k is slot k + 1. Then, for i from 0 to 6, a bar is drawn at y = 67 + 38 i - dy when slot i + 1's id differs from slot i's
//  and slot i's is not 0xFFFF: cels 0 and 1 of gpPartyBarImage side by side, at dx + 12 and dx + 267. An entry in no party closes no group: 0xFFFF
//  is what ROSTER_PartyGetSharedPartyId answers for no party and for a party of one.
//  The loop ends on a signed compare of its pointer against &gnPartyRowPartyIds[7]; the C walks an int index, whose `< 7` is the same test. The
//  cel data is cleared again before every bar.
//  D2ExpRes patches four sites here, each marked on its line.
void __cdecl PARTY_DrawPartyGroupDividers()
{
    D2GfxDataStrc tCelData;
    D2RosterUnitStrc *pEntry;
    int32_t *pPartyId;
    int32_t nBarY;
    int32_t i;

    pEntry = ROSTER_PartyGetNextEntry(NULL);
    for (i = 0; i < 8; ++i)
    {
        gnPartyRowPartyIds[i] = 0xFFFF;
    }

    //  BUG: the fill has no bound. It writes one dword per roster entry into an array of eight, and the dword after the array is gpPartyRowList in
    //  the original (0x6FBB5EF8 + 32 is 0x6FBB5F18), so a ninth entry would store its party id over the head of the list. LATENT, with no margin:
    //  the roster never holds more than eight entries, so the fill writes at most eight dwords.
    //  DIFFERENCE: gnPartyRowPartyIds is our own array, so a ninth entry would write past it, not over gpPartyRowList.
    pPartyId = gnPartyRowPartyIds;
    while (pEntry)
    {
        *pPartyId = (uint16_t)ROSTER_PartyGetSharedPartyId(pEntry);
        pEntry = ROSTER_PartyGetNextEntry(pEntry);
        ++pPartyId;
    }

    //  D2ExpRes PartyPartybar4 lands on the first bar's y
    nBarY = 67;
    for (i = 0; i < 7; ++i)
    {
        if (gnPartyRowPartyIds[i + 1] != gnPartyRowPartyIds[i] && gnPartyRowPartyIds[i] != 0xFFFF)
        {
            memset(&tCelData, 0, sizeof(tCelData));
            tCelData.pCellFile = gpPartyBarImage;
            tCelData.nDirection = 0;
            tCelData.nFrame = 0;
            //  D2ExpRes PartyPartybar2 lands on the left cel's x
            D2GFX_TEXTURE_CelDraw(&tCelData, gnBorderXDifference + 12, nBarY - gnBorderYDifference, 0xFFFFFFFF, 5, NULL);
            tCelData.nFrame = 1;
            //  D2ExpRes PartyPartybar3 lands on the right cel's x
            D2GFX_TEXTURE_CelDraw(&tCelData, gnBorderXDifference + 267, nBarY - gnBorderYDifference, 0xFFFFFFFF, 5, NULL);
        }
        //  D2ExpRes PartyPartybar1 lands on the row pitch
        nBarY += 38;
    }
}

//1.10f: D2Client.0x6FB02AC0
//  Whether the cursor is in a panel box whose left edge is nX and whose bottom edge is nY, in panel space, nWidth to the right and nHeight up:
//  screen x (dx + nX)..(dx + nX + nWidth), y (nY - dy - nHeight)..(nY - dy), every edge inclusive and every compare signed. The original calls it
//  once, for the close button's tooltip (278, 421, 32, 32), and inlines it in the other box tests of this file.
BOOL __fastcall PARTY_IsMouseOverRect(int32_t nX, int32_t nY, int32_t nWidth, int32_t nHeight)
{
    int32_t nMouseX;
    int32_t nMouseY;

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    if (nMouseX < gnBorderXDifference + nX || nMouseX > gnBorderXDifference + nX + nWidth || nMouseY < nY - gnBorderYDifference - nHeight || nMouseY > nY - gnBorderYDifference)
    {
        return FALSE;
    }
    return TRUE;
}

//1.10f: D2Client.0x6FB02B20
//  Draws the hints over the LEAVE button and over each row's first button, every frame. Called by PARTY_DrawPanel. It builds nothing: it tests
//  the cursor and draws a hint box and its text. The box is black (gnUiBoxFillColor = MixRGB(0, 0, 0) at the top, for PARTY_DrawHintBackdrop) and
//  the text colour 0:
//      over                             when               tbl        centre, y
//      the LEAVE box, x (dx+227)..      the local player   4120       213, 55
//        (dx+280), y (24-dy)..(35-dy)   is in a party
//      a row's first button, 53x20      eButton 0          4100/4101  218, 50/65
//                                       eButton 2          4102/4103  218, 50/65
//                                       eButton 4          4104/4105  218, 50/65
//  Kinds 1 (PARTYBUTTON_NONE) and 3 (LEAVE) show none. The LEAVE box and its gate are PARTY_OnLButtonUp's (the shared party id compared as a WORD,
//  `cmp ax, 0FFFFh` at 0x6FB02B5A), and the row box is PARTY_OnLButtonDown's wide kind. Every row is tested, with no stop after a hit, and a row
//  whose Buttons[0].nButtonType is not 0 is skipped (0x6FB02CA3).
//  The original inlines PARTY_DrawHintLines in each of the four hints, step for step with constant arguments, and PARTY_IsMouseOverRect in the two
//  box tests (the LEAVE test with its constants folded). Here the hints are written out, each line with a buffer slot of its own, and the box
//  tests are calls.
void __cdecl PARTY_DrawControlButtonHints()
{
    D2ClientPartyRowStrc *pRow;
    D2ClientPartyRowStrc *pNext;
    D2RosterUnitStrc *pMyEntry;
    D2RosterUnitStrc *pEntry;
    int32_t nKind;
    int32_t nX;

    pRow = gpPartyRowList;
    pMyEntry = ROSTER_PartyGetNextEntry(NULL);
    gnUiBoxFillColor = D2WIN_10034_MixRGB(0, 0, 0);
    if ((uint16_t)ROSTER_PartyGetSharedPartyId(pMyEntry) != 0xFFFF)
    {
        if (PARTY_IsMouseOverRect(227, 35, 53, 11))
        {
            PARTY_DrawHintBackdrop(STR_IDX_4120_strParty24, (D2C_StringIndices)0, 213, 0);
            {
                Unicode wszLine[100];
                Unicode::strcpy(wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_4120_strParty24));
                nX = 213 - D2WIN_10121_GetTextWidth(wszLine) / 2;
                D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4120_strParty24), gnBorderXDifference + nX, 55 - gnBorderYDifference, 0, 0);
            }
        }
    }

    //  advanced with the rows and never read, as in the original (the calls are observable)
    pEntry = ROSTER_PartyGetNextEntry(pMyEntry);
    while (pRow)
    {
        if (PARTY_IsMouseOverRect(pRow->Buttons[0].nX, pRow->Buttons[0].nY, 53, 20) && !pRow->Buttons[0].nButtonType)
        {
            nKind = pRow->Buttons[0].eButton;
            if (nKind == PARTYBUTTON_INVITE)
            {
                PARTY_DrawHintBackdrop(STR_IDX_4100_strParty1, STR_IDX_4101_strParty2, 218, 1);
                {
                    Unicode wszLine[100];
                    Unicode::strcpy(wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_4100_strParty1));
                    nX = 218 - D2WIN_10121_GetTextWidth(wszLine) / 2;
                    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4100_strParty1), gnBorderXDifference + nX, 50 - gnBorderYDifference, 0, 0);
                }
                {
                    Unicode wszLine[100];
                    Unicode::strcpy(wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_4101_strParty2));
                    nX = 218 - D2WIN_10121_GetTextWidth(wszLine) / 2;
                    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4101_strParty2), gnBorderXDifference + nX, 65 - gnBorderYDifference, 0, 0);
                }
            }
            else if (nKind == PARTYBUTTON_ACCEPT)
            {
                PARTY_DrawHintBackdrop(STR_IDX_4102_strParty3, STR_IDX_4103_strParty4, 218, 1);
                {
                    Unicode wszLine[100];
                    Unicode::strcpy(wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_4102_strParty3));
                    nX = 218 - D2WIN_10121_GetTextWidth(wszLine) / 2;
                    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4102_strParty3), gnBorderXDifference + nX, 50 - gnBorderYDifference, 0, 0);
                }
                {
                    Unicode wszLine[100];
                    Unicode::strcpy(wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_4103_strParty4));
                    nX = 218 - D2WIN_10121_GetTextWidth(wszLine) / 2;
                    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4103_strParty4), gnBorderXDifference + nX, 65 - gnBorderYDifference, 0, 0);
                }
            }
            else if (nKind == PARTYBUTTON_CANCEL)
            {
                PARTY_DrawHintBackdrop(STR_IDX_4104_strParty5, STR_IDX_4105_strParty6, 218, 1);
                {
                    Unicode wszLine[100];
                    Unicode::strcpy(wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_4104_strParty5));
                    nX = 218 - D2WIN_10121_GetTextWidth(wszLine) / 2;
                    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4104_strParty5), gnBorderXDifference + nX, 50 - gnBorderYDifference, 0, 0);
                }
                {
                    Unicode wszLine[100];
                    Unicode::strcpy(wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_4105_strParty6));
                    nX = 218 - D2WIN_10121_GetTextWidth(wszLine) / 2;
                    D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(STR_IDX_4105_strParty6), gnBorderXDifference + nX, 65 - gnBorderYDifference, 0, 0);
                }
            }
        }

        pNext = pRow->pNext;
        if (pEntry)
        {
            pEntry = ROSTER_PartyGetNextEntry(pEntry);
        }
        pRow = pNext;
    }
}

//1.10f: D2Client.0x6FB03010
//  Draws a hint's backdrop: a box 10 wider than the wider of its one or two tbl lines, centred on nCenterX, in draw mode 2 (DRAWMODE_TRANS75):
//  y = 35 - dy and 30 high for two lines, 42 - dy and 15 high for one. Called by PARTY_DrawHintLines and by the four hints written out in
//  PARTY_DrawControlButtonHints.
//  Each line is copied into a 100-element Unicode buffer of its own to be measured. "Is there a second line" is `test bp, bp` (0x6FB03055), a
//  16-bit test, which D2C_StringIndices' uint16_t base gives the C as it stands. Half the width is a signed divide by 2. The colour is
//  gnUiBoxFillColor, passed as a byte.
void __fastcall PARTY_DrawHintBackdrop(D2C_StringIndices nTblLine1, D2C_StringIndices nTblLine2, int32_t nCenterX, int32_t bTwoLines)
{
    Unicode wszLine1[100];
    int32_t nWidth1;
    int32_t nWidth2;
    int32_t nWidth;
    int32_t nY;
    int32_t nHeight;

    Unicode::strcpy(wszLine1, D2LANG_GetStringFromTblIndex(nTblLine1));
    nWidth1 = D2WIN_10121_GetTextWidth(wszLine1);
    nWidth2 = 0;
    if (nTblLine2)
    {
        Unicode wszLine2[100];
        Unicode::strcpy(wszLine2, D2LANG_GetStringFromTblIndex(nTblLine2));
        nWidth2 = D2WIN_10121_GetTextWidth(wszLine2);
    }

    nY = 35;
    nHeight = 30;
    if (!bTwoLines)
    {
        nY = 42;
        nHeight = 15;
    }

    if (nWidth1 > nWidth2)
    {
        nWidth = nWidth1;
    }
    else
    {
        nWidth = nWidth2;
    }
    //  DIFFERENCE: the original loads gnUiBoxFillColor into the low byte of a register that holds something else (line 2's width, or bTwoLines) and
    //  pushes the register; ours passes the byte. Every render driver masks the colour to its byte, so nothing reads the difference.
    GFX_DrawFilledRect(gnBorderXDifference - nWidth / 2 + nCenterX - 5, nY - gnBorderYDifference, nWidth + 10, nHeight, gnUiBoxFillColor, DRAWMODE_TRANS75);
}

//1.10f: D2Client.0x6FB03110
//  Draws the hover hints over a row's name and over its four boxes, every frame after PARTY_DrawControlButtonHints. Called by PARTY_DrawPanel.
//  The rows are walked with the roster in step: pEntry starts at the roster's second entry, the first row's player, and steps with every row; here
//  it is read, it is the player the hint describes. Before the walk the box colour is set black and the local player's room is tested once for
//  a town. The original inlines PARTY_IsMouseOverRect for each box test, with the x constants folded for the name band.
//  Over the name (the band is PARTY_IsMouseOverRect(36, nNameBandBottom, 104, 26)) two lines are composed:
//      line 1  hostility              index 2a + b  a PlayerIsHostileToEntry
//                                                   b EntryIsHostileToPlayer
//      line 2  loot, hardcore only    index 2a + b  a PlayerAllowsEntryLoot
//                                                   b EntryAllowsPlayerLoot
//  from four tables of WORD tbl indices (gaPartyHostileHintMale/Female, gaPartyLootHintMale/Female), the male or the female one by
//  ROSTER_EntryIsMale (0 for classes 0, 1 and 6). When a == b the string is copied as it stands, and when they differ it is a format with the
//  player's name in its %s, through Unicode::sprintf over 128. The four predicates answer exactly 0 or 1, so the index is 0..3. The name is
//  ROSTER_GetDisplayName's, or gszErrorPlayerName when that answers NULL.
//  Line 1 is coloured 8 when the local player is hostile to the entry and 1 when the entry is hostile to the local player, the second winning;
//  line 2 9 and 4 the same way for loot. The box is (dx + 36, 35 - dy), as wide as the wider line plus 10 and 16 high, 30 in a hardcore game, in
//  draw mode 2; the lines go to (dx + 41, 50 - dy) and (dx + 41, 65 - dy). Line 2 is drawn in softcore too, and is empty (D2Lang's default
//  constructor writes 0). A name hit does not end the walk: the next row is tested as well.
//  Otherwise buttons 1..4 are tested with PARTY_IsMouseOverRect(nX, nY, 20, 20), and a hit draws one hint through PARTY_DrawHintLines and returns:
//      nButtonType  when                                       tbl        centre
//      1            the roster head or the row below level 9   4122/4123  160
//                   else outside town, in colour 1             4121       110
//                   else ROSTER_EntryIsHostile                 4108/4109  110
//                   else                                       4106/4107  110
//      2            hardcore, ROSTER_EntryAllowsLoot           4112/4113  255
//                   hardcore, else                             4110/4111  255
//                   softcore: no hint, the next box is tested
//      3            ROSTER_EntryIsIgnored                      4114/4115  276
//                   else                                       4116/4117  276
//      4            ROSTER_EntryIsSquelched ? 4119 : 4118, one line       296
//  Two lines go to y 50 and 65, one to y 55. The level gate comes before the town gate, so a player below level 9 is never told about the town.
//  The centre column is PARTY_DrawHintLines' first stack argument, nCenterX.
//  D2ExpRes patches ten sites here, each marked on its line.
void __cdecl PARTY_DrawHoverHint()
{
    D2ClientPartyRowStrc *pRow;
    D2ClientPartyButtonStrc *pButton;
    D2RosterUnitStrc *pEntry;
    D2ActiveRoomStrc *pRoom;
    D2C_StringIndices nClickHint;
    int32_t bInTown;
    int32_t iButton;

    pRow = gpPartyRowList;
    pEntry = ROSTER_PartyGetNextEntry(NULL);
    pRoom = D2COMMON_UNITS_GetRoom(CUNIT_GetPlayer());
    bInTown = 0;
    pEntry = ROSTER_PartyGetNextEntry(pEntry);
    gnUiBoxFillColor = D2WIN_10034_MixRGB(0, 0, 0);
    if (pRoom && D2COMMON_DUNGEON_IsRoomInTown(pRoom))
    {
        bInTown = 1;
    }

    while (pRow)
    {
        //  D2ExpRes PartyFriendlyHostileStrings3 lands on the band's x
        if (PARTY_IsMouseOverRect(36, pRow->nNameBandBottom, 104, 26))
        {
            int32_t nColor1;
            int32_t nColor2;
            int32_t nHeight;
            int32_t bPlayerHostile;
            int32_t bPlayerAllowsLoot;
            int32_t bEntryHostile;
            int32_t bEntryAllowsLoot;
            int32_t bMale;
            int32_t iHint;
            D2C_StringIndices nStringId;
            int32_t nWidth1;
            int32_t nWidth2;
            const char *pszName;
            char szErrorName[128];

            nColor1 = 0;
            nColor2 = 0;
            Unicode wszLine1[128];
            Unicode wszLine2[128];
            Unicode wszName[128];
            nHeight = 16;

            pszName = ROSTER_GetDisplayName(pEntry);
            //  BUG: gszErrorPlayerName is copied when ROSTER_GetDisplayName answers NULL. UNREACHABLE: it never does (its NULL arm only logs a
            //  warning, and both arms reach `lea eax, [esi+66h]`).
            //  BUG: a NULL pEntry would come back as 0x66 and fault reading it in Unicode::toUnicode. LATENT: pEntry is NULL on a name hit only if
            //  the rows outnumber the roster's entries after the first, and every function that changes the roster rebuilds the rows.
            if (!pszName)
            {
                strcpy(szErrorName, gszErrorPlayerName);   // 18 bytes into 128
                pszName = szErrorName;
            }
            Unicode::toUnicode(wszName, pszName, 128);

            bPlayerHostile = ROSTER_PlayerIsHostileToEntry(pEntry);
            bPlayerAllowsLoot = ROSTER_PlayerAllowsEntryLoot(pEntry);
            bEntryHostile = ROSTER_EntryIsHostileToPlayer(pEntry);
            bEntryAllowsLoot = ROSTER_EntryAllowsPlayerLoot(pEntry);

            //  line 1, hostility: index 2a + b, each 0 or 1
            bMale = ROSTER_EntryIsMale(pEntry);
            iHint = 2 * bPlayerHostile + bEntryHostile;
            if (bMale)
            {
                nStringId = (D2C_StringIndices)gaPartyHostileHintMale[iHint];
            }
            else
            {
                nStringId = (D2C_StringIndices)gaPartyHostileHintFemale[iHint];
            }
            if (bPlayerHostile == bEntryHostile)
            {
                Unicode::strcpy(wszLine1, D2LANG_GetStringFromTblIndex(nStringId));
            }
            else
            {
                Unicode::sprintf(128, wszLine1, D2LANG_GetStringFromTblIndex(nStringId), wszName);
            }

            //  line 2, loot, hardcore only
            if (WINMAIN_IsHardcoreGame())
            {
                nHeight = 30;
                bMale = ROSTER_EntryIsMale(pEntry);
                iHint = 2 * bPlayerAllowsLoot + bEntryAllowsLoot;
                if (bMale)
                {
                    nStringId = (D2C_StringIndices)gaPartyLootHintMale[iHint];
                }
                else
                {
                    nStringId = (D2C_StringIndices)gaPartyLootHintFemale[iHint];
                }
                if (bPlayerAllowsLoot == bEntryAllowsLoot)
                {
                    Unicode::strcpy(wszLine2, D2LANG_GetStringFromTblIndex(nStringId));
                }
                else
                {
                    Unicode::sprintf(128, wszLine2, D2LANG_GetStringFromTblIndex(nStringId), wszName);
                }
            }

            if (bPlayerHostile)
            {
                nColor1 = 8;
            }
            if (bEntryHostile)
            {
                nColor1 = 1;
            }
            if (bPlayerAllowsLoot)
            {
                nColor2 = 9;
            }
            if (bEntryAllowsLoot)
            {
                nColor2 = 4;
            }

            nWidth1 = D2WIN_10121_GetTextWidth(wszLine1) + 10;
            nWidth2 = D2WIN_10121_GetTextWidth(wszLine2) + 10;
            if (nWidth1 > nWidth2)
            {
                nWidth2 = nWidth1;
            }
            //  DIFFERENCE: the original pushes ecx, whose low byte is the colour and whose upper bits are whatever D2WIN_10121_GetTextWidth
            //  left in it; ours passes the byte. Every render driver masks the colour to its byte, so nothing reads the difference.
            //  D2ExpRes PartyFriendlyHostileStrings2 lands on the box's y
            GFX_DrawFilledRect(gnBorderXDifference + 36, 35 - gnBorderYDifference, nWidth2, nHeight, gnUiBoxFillColor, DRAWMODE_TRANS75);
            //  D2ExpRes PartyFriendlyHostileStrings1 lands on line 1's y
            D2WIN_10117_DrawText(wszLine1, gnBorderXDifference + 41, 50 - gnBorderYDifference, nColor1, 0);
            //  D2ExpRes PartyLootCorpsesStrings lands on line 2's y
            D2WIN_10117_DrawText(wszLine2, gnBorderXDifference + 41, 65 - gnBorderYDifference, nColor2, 0);
        }
        else
        {
            for (iButton = 1; iButton < 5; ++iButton)
            {
                pButton = &pRow->Buttons[iButton];
                if (!PARTY_IsMouseOverRect(pButton->nX, pButton->nY, 20, 20))
                {
                    continue;
                }

                switch (pButton->nButtonType)
                {
                case ROSTERBUTTONTYPE_HOSTILE:
                    //  zero-extended, as in PARTY_BuildRowList
                    if (ROSTER_EntryGetLevel(ROSTER_PartyGetNextEntry(NULL)) < 9 || pRow->nLevel < 9)
                    {
                        //  D2ExpRes PartyBothPlayersMustBeString lands on this call
                        PARTY_DrawHintLines(STR_IDX_4122_StrParty26, STR_IDX_4123_StrParty27, 160, 0, 50, 65, 0);
                        return;
                    }
                    if (!bInTown)
                    {
                        //  D2ExpRes PartyMustBeToggledString lands on this call
                        PARTY_DrawHintLines(STR_IDX_4121_strParty25, (D2C_StringIndices)0, 110, 0, 55, 0, 1);
                        return;
                    }
                    //  D2ExpRes PartyClickToGoStrings lands on this test
                    if (ROSTER_EntryIsHostile(pEntry))
                    {
                        PARTY_DrawHintLines(STR_IDX_4108_strParty9, STR_IDX_4109_strParty10, 110, 0, 50, 65, 0);
                    }
                    else
                    {
                        PARTY_DrawHintLines(STR_IDX_4106_strParty7, STR_IDX_4107_strParty8, 110, 0, 50, 65, 0);
                    }
                    return;

                case ROSTERBUTTONTYPE_LOOT:
                    //  softcore: no hint, and the next box is tested
                    if (!WINMAIN_IsHardcoreGame())
                    {
                        break;
                    }
                    //  D2ExpRes PartyClickToAllowLootStrings lands on this test
                    if (ROSTER_EntryAllowsLoot(pEntry))
                    {
                        PARTY_DrawHintLines(STR_IDX_4112_strParty13, STR_IDX_4113_strParty14, 255, 0, 50, 65, 0);
                    }
                    else
                    {
                        PARTY_DrawHintLines(STR_IDX_4110_strParty11, STR_IDX_4111_strParty12, 255, 0, 50, 65, 0);
                    }
                    return;

                case ROSTERBUTTONTYPE_IGNORE:
                    //  D2ExpRes PartyClickToAllowStopStrings lands on this test
                    if (ROSTER_EntryIsIgnored(pEntry))
                    {
                        PARTY_DrawHintLines(STR_IDX_4114_strParty15, STR_IDX_4115_strParty16, 276, 0, 50, 65, 0);
                    }
                    else
                    {
                        PARTY_DrawHintLines(STR_IDX_4116_strParty17, STR_IDX_4117_strParty18, 276, 0, 50, 65, 0);
                    }
                    return;

                case ROSTERBUTTONTYPE_SQUELCH:
                    if (ROSTER_EntryIsSquelched(pEntry))
                    {
                        nClickHint = STR_IDX_4119_strParty22;
                    }
                    else
                    {
                        nClickHint = STR_IDX_4118_strParty19;
                    }
                    //  D2ExpRes PartyClickToSquelchStrings lands on this call
                    PARTY_DrawHintLines(nClickHint, (D2C_StringIndices)0, 296, 0, 55, 0, 0);
                    return;
                }
            }
        }

        pRow = pRow->pNext;
        if (pEntry)
        {
            pEntry = ROSTER_PartyGetNextEntry(pEntry);
        }
    }
}

//1.10f: D2Client.0x6FB036C0
//  Draws a whole hint: the backdrop, then one or two tbl lines, each centred on nCenterX and drawn at (dx + nCenterX - width / 2, nY - dy) in
//  nColor. Called by PARTY_DrawHoverHint; PARTY_DrawControlButtonHints writes it out four times.
//  nTblLine2 picks the arm (`test bp, bp`, 0x6FB036CB): 0 draws nTblLine1 at nY1 over a one-line backdrop, anything else both lines, at nY1 and
//  nY2, over a two-line one. The one-line arm does not read nY2, and nothing reads nUnused.
//  Each line is: a 100-element Unicode buffer constructed, the tbl string copied in and measured, and then the string fetched from the tbl again
//  and drawn (the measured copy is not what is drawn). The two-line arm constructs its one buffer twice; the C gives each line a block of its own.
void __fastcall PARTY_DrawHintLines(D2C_StringIndices nTblLine1, D2C_StringIndices nTblLine2, int32_t nCenterX, int32_t nUnused, int32_t nY1, int32_t nY2, int32_t nColor)
{
    int32_t nX;

    (void)nUnused;
    if (!nTblLine2)
    {
        PARTY_DrawHintBackdrop(nTblLine1, (D2C_StringIndices)0, nCenterX, 0);
        {
            Unicode wszLine[100];
            Unicode::strcpy(wszLine, D2LANG_GetStringFromTblIndex(nTblLine1));
            nX = nCenterX - D2WIN_10121_GetTextWidth(wszLine) / 2;
            D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(nTblLine1), gnBorderXDifference + nX, nY1 - gnBorderYDifference, nColor, 0);
        }
        return;
    }

    PARTY_DrawHintBackdrop(nTblLine1, nTblLine2, nCenterX, 1);
    {
        Unicode wszLine[100];
        Unicode::strcpy(wszLine, D2LANG_GetStringFromTblIndex(nTblLine1));
        nX = nCenterX - D2WIN_10121_GetTextWidth(wszLine) / 2;
        D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(nTblLine1), gnBorderXDifference + nX, nY1 - gnBorderYDifference, nColor, 0);
    }
    {
        Unicode wszLine[100];
        Unicode::strcpy(wszLine, D2LANG_GetStringFromTblIndex(nTblLine2));
        nX = nCenterX - D2WIN_10121_GetTextWidth(wszLine) / 2;
        D2WIN_10117_DrawText(D2LANG_GetStringFromTblIndex(nTblLine2), gnBorderXDifference + nX, nY2 - gnBorderYDifference, nColor, 0);
    }
}

//1.10f: D2Client.0x6FB03870
//  Handles WM_LBUTTONDOWN on the party panel. Three things can be under the cursor, tested in this order:
//      1. the panel's CLOSE button, screen x (dx+278)..(dx+310) and y (389-dy)..(421-dy), inclusive on all four edges, so 33 by 33;
//      2. nothing at all, if the cursor is in the right half of the screen;
//      3. a per-player ROW button, walked over the row list.
//  The original reads the cursor four times in this function (CURSOR_GetMouseX at 0x6FB03877 and again at 0x6FB038A3: the half-screen test uses
//  the first and the close-button test the second) and up to twice more per row; nothing moves the mouse between them, so they agree.
//  The original inlines PARTY_IsMouseOverRect for the seven box tests of the two press handlers. The row's cursor is read inside each kind's
//  arm, after the kind test (field_00), because the inlined function's own two calls come after it.
//  The hostile button is refused below level 9, and the level read is `ROSTER_EntryGetLevel(ROSTER_PartyGetNextEntry(NULL))`: the first roster
//  entry, fetched fresh, with the clicked row's entry ignored. Reproduced. The callee clears eax before its word load (`xor eax, eax`), so the
//  compare against 9 is exact.
//  pSecondEntry, the roster's second entry advanced in lockstep with the row list, is never read here (PARTY_OnLButtonUp reads its twin).
//  Reproduced: the calls are observable.
//  The original carries the row list in a register and in a stack slot, assigned from one another at the top of every outer iteration, so they
//  never differ; one variable here.
void __stdcall PARTY_OnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nFirstMouseX;
    int32_t nRowX;
    int32_t nRowY;
    int32_t iRow;
    int32_t nLevel;
    D2ActiveRoomStrc *pRoom;
    D2RosterUnitStrc *pSecondEntry;
    D2ClientPartyRowStrc *pRowList;
    D2ClientPartyButtonStrc *pButton;
    D2UnitStrc *pPlayer;

    nFirstMouseX = CURSOR_GetMouseX();
    pSecondEntry = ROSTER_PartyGetNextEntry(ROSTER_PartyGetNextEntry(NULL));
    pRowList = gpPartyRowList;
    pPlayer = CUNIT_GetPlayer();

    if (PARTY_IsMouseOverRect(278, 421, 32, 32))
    {
        gbPartyButtonDown = 1;
        pMsg->bReturn = 1;
        goto CONSUME;
    }

    //  the first cursor read decides this one
    if (nFirstMouseX >= gnScreenWidth / 2)
    {
        pMsg->bReturn = 0;
        return;
    }

    while (pRowList)
    {
        for (iRow = 0; iRow < 5; ++iRow)
        {
            nRowX = pRowList->Buttons[iRow].nX;
            nRowY = pRowList->Buttons[iRow].nY;

            //  the kind is tested before the cursor is read
            if (pRowList->Buttons[iRow].field_00)
            {
                if (PARTY_IsMouseOverRect(nRowX, nRowY, 53, 20))
                {
                    pButton = &pRowList->Buttons[iRow];
                    if (pButton->eButton != PARTYBUTTON_NONE)
                    {
                        pButton->nFrame = 1;
                    }
                    pButton->field_1C = 1;
                    gbPartyRowButtonDown = 1;
                    pMsg->bReturn = 1;
                    goto CONSUME;
                }
                continue;
            }

            if (!PARTY_IsMouseOverRect(nRowX, nRowY, 20, 20))
            {
                continue;
            }

            if (pRowList->Buttons[iRow].nButtonType == ROSTERBUTTONTYPE_HOSTILE)
            {
                nLevel = ROSTER_EntryGetLevel(ROSTER_PartyGetNextEntry(NULL));
                if (nLevel < 9)
                {
                    goto ACCEPT_NO_PRESS;
                }
                //  the original tests this twice; the second test is never taken
                if (pRowList->Buttons[iRow].nButtonType == ROSTERBUTTONTYPE_HOSTILE)
                {
                    pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
                    if (!pRoom || !D2COMMON_DUNGEON_IsRoomInTown(pRoom))
                    {
                        continue;
                    }
                }
            }

            if (!pRowList->Buttons[iRow].field_1C)
            {
                pButton = &pRowList->Buttons[iRow];
                pButton->nFrame = pButton->nFrame + 1;
                pButton->field_1C = 1;
                gbPartyRowButtonDown = 1;
                goto ACCEPT_NO_PRESS;
            }
        }

        pRowList = pRowList->pNext;
        if (pSecondEntry)
        {
            pSecondEntry = ROSTER_PartyGetNextEntry(pSecondEntry);
        }
    }

ACCEPT_NO_PRESS:
    pMsg->bReturn = 1;

CONSUME:
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FB03AC0
//  Handles WM_LBUTTONUP on the party panel, its whole command surface. Four outcomes, tested in order:
//      right half of the screen        bReturn = 0 and nothing else
//      the CLOSE box                   UI_SetUIToggle(UI_PARTYSCREEN, UI_TURNOFF)
//      the LEAVE box                   ROSTER_PartySendLeaveParty()
//      a row button                    the switch below
//  The LEAVE box is screen x (dx+227)..(dx+280) and y (24-dy)..(35-dy), 54 by 12, and the arm is refused when
//  `(uint16_t)ROSTER_PartyGetSharedPartyId(pFirstEntry) == 0xFFFF` (the id is compared as a WORD).
//  The switch on nButtonType:
//      1   hostile   ROSTER_PartySendHostileToggle
//      2   loot      ROSTER_ToggleLootAndNotify, but only when WINMAIN_IsHardcoreGame(); in a softcore game the case does nothing, and the
//                    frame does not advance either
//      3   ignore    ROSTER_ToggleIgnoreAndNotify
//      4   squelch   ROSTER_ToggleSquelchAndNotify, and then a chat command to Battle.net
//  bToggledOn is one slot reused by all four arms for the toggle's new state.
//  The squelch arm calls through a struct of function pointers, not a C++ vtable: the one at +0x14 takes its argument in ecx, the buffer is the
//  `this`. BnClientInterface has no body, so the call is written through a cast and the member is not named. The command buffer is 512 bytes; the
//  original inlines both string operations, written here as strcpy/strcat.
//  A release that hits nothing undoes the press: when gbPartyRowButtonDown is clear the function walks every row of every list node and, for each
//  button with field_1C set, clears it and decrements nFrame, the inverse of PARTY_OnLButtonDown's nFrame + 1.
//  After ROSTER_PartySendButtonCommand the row list is re-walked from the head for the node whose dwUnitId matches the one saved before the call,
//  and running off the end halts the game at the original's assert (Party.cpp:1251): the send can rebuild the list, so the node in hand may be
//  freed.
//  The level gate and the doubled hostile test are as in PARTY_OnLButtonDown, and so is the inlined PARTY_IsMouseOverRect (four box tests here).
void __stdcall PARTY_OnLButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nFirstMouseX;
    int32_t nRowX;
    int32_t nRowY;
    int32_t iRow;
    int32_t nLevel;
    int32_t bToggledOn;
    uint32_t dwRowKey;
    D2C_PartyButtons eButton;
    D2ActiveRoomStrc *pRoom;
    D2RosterUnitStrc *pFirstEntry;
    D2RosterUnitStrc *pEntry;
    D2ClientPartyRowStrc *pRowList;
    D2ClientPartyRowStrc *pSearch;
    D2ClientPartyButtonStrc *pButton;
    D2UnitStrc *pPlayer;
    struct BnClientInterface *pBnInterface;
    char szCommandBuf[512];

    nFirstMouseX = CURSOR_GetMouseX();
    pFirstEntry = ROSTER_PartyGetNextEntry(NULL);
    pRowList = gpPartyRowList;

    //  cleared before any test, including on the arm that declines below
    gbPartyButtonDown = 0;
    pPlayer = CUNIT_GetPlayer();

    if (nFirstMouseX >= gnScreenWidth / 2)
    {
        pMsg->bReturn = 0;
        return;
    }

    if (PARTY_IsMouseOverRect(278, 421, 32, 32))
    {
        UI_SetUIToggle(UI_PARTYSCREEN, UI_TURNOFF, 0);
        goto RESET_AND_CONSUME;
    }

    if (PARTY_IsMouseOverRect(227, 35, 53, 11) && (uint16_t)ROSTER_PartyGetSharedPartyId(pFirstEntry) != 0xFFFF)
    {
        ROSTER_PartySendLeaveParty();
        goto RESET_AND_CONSUME;
    }

    if (gbPartyRowButtonDown)
    {
        pEntry = ROSTER_PartyGetNextEntry(pFirstEntry);
        while (pEntry && pRowList)
        {
            for (iRow = 0; iRow < 5; ++iRow)
            {
                nRowX = pRowList->Buttons[iRow].nX;
                nRowY = pRowList->Buttons[iRow].nY;

                //  the kind is tested before the cursor is read
                if (pRowList->Buttons[iRow].field_00)
                {
                    if (!PARTY_IsMouseOverRect(nRowX, nRowY, 53, 20))
                    {
                        continue;
                    }

                    if (pRowList->Buttons[iRow].eButton != PARTYBUTTON_NONE)
                    {
                        //  the list may be rebuilt by the send, so the node is re-found by its unit id afterwards
                        dwRowKey = pRowList->dwUnitId;
                        ROSTER_PartySendButtonCommand(pEntry);
                        eButton = ROSTER_EntryGetPartyButton(pEntry);

                        pSearch = gpPartyRowList;
                        while (pSearch && pSearch->dwUnitId != dwRowKey)
                        {
                            pSearch = pSearch->pNext;
                        }
                        if (!pSearch)
                        {
                            FOG_DisplayAssert("ptEntry != NULL", szPartyCpp, 1251);
                            exit(-1);
                        }
                        pSearch->Buttons[iRow].eButton = eButton;
                    }
                    goto RESET_AND_CONSUME;
                }

                if (!PARTY_IsMouseOverRect(nRowX, nRowY, 20, 20))
                {
                    continue;
                }

                if (pRowList->Buttons[iRow].nButtonType == ROSTERBUTTONTYPE_HOSTILE)
                {
                    nLevel = ROSTER_EntryGetLevel(ROSTER_PartyGetNextEntry(NULL));
                    if (nLevel < 9)
                    {
                        pMsg->bReturn = 1;
                        pMsg->lResult = 0;
                        STORM_SMsgBreakHandlerChain(pMsg);
                        return;
                    }
                    //  the original tests this twice; the second test is never taken
                    if (pRowList->Buttons[iRow].nButtonType == ROSTERBUTTONTYPE_HOSTILE)
                    {
                        pRoom = D2COMMON_UNITS_GetRoom(pPlayer);
                        if (!pRoom || !D2COMMON_DUNGEON_IsRoomInTown(pRoom))
                        {
                            continue;
                        }
                    }
                }

                bToggledOn = 0;
                switch (pRowList->Buttons[iRow].nButtonType)
                {
                case ROSTERBUTTONTYPE_HOSTILE:
                    ROSTER_PartySendHostileToggle(pEntry);
                    bToggledOn = ROSTER_EntryIsHostile(pEntry);
                    break;

                case ROSTERBUTTONTYPE_LOOT:
                    //  softcore does nothing at all here
                    if (WINMAIN_IsHardcoreGame())
                    {
                        ROSTER_ToggleLootAndNotify(pEntry);
                        bToggledOn = ROSTER_EntryAllowsLoot(pEntry);
                    }
                    break;

                case ROSTERBUTTONTYPE_IGNORE:
                    ROSTER_ToggleIgnoreAndNotify(pEntry);
                    bToggledOn = ROSTER_EntryIsIgnored(pEntry);
                    break;

                case ROSTERBUTTONTYPE_SQUELCH:
                    ROSTER_ToggleSquelchAndNotify(pEntry);
                    bToggledOn = ROSTER_EntryIsSquelched(pEntry);
                    pBnInterface = WINMAIN_GetBnClientInterface();
                    if (pBnInterface)
                    {
                        strcpy(szCommandBuf, bToggledOn ? szSquelchCommand : gszUnsquelchCommand);
                        strcat(szCommandBuf, ROSTER_GetDisplayName(pEntry));
                        //  a struct of function pointers; +0x14 takes ecx
                        (*(void(__fastcall **)(char *))((char *)pBnInterface + 0x14))(szCommandBuf);
                    }
                    break;

                default:
                    break;
                }

                if (bToggledOn)
                {
                    pButton = &pRowList->Buttons[iRow];
                    pButton->nFrame = pButton->nFrame + 2;
                }
            }

            pRowList = pRowList->pNext;
            pEntry = ROSTER_PartyGetNextEntry(pEntry);
        }
        gbPartyRowButtonDown = 0;
    }
    else if (pRowList)
    {
        //  the release hit nothing: undo every pressed frame
        for (;;)
        {
            for (iRow = 0; iRow < 5; ++iRow)
            {
                if (pRowList->Buttons[iRow].field_1C)
                {
                    pRowList->Buttons[iRow].field_1C = 0;
                    pRowList->Buttons[iRow].nFrame = pRowList->Buttons[iRow].nFrame - 1;
                }
            }
            if (!pRowList->pNext)
            {
                break;
            }
            pRowList = pRowList->pNext;
        }
    }

RESET_AND_CONSUME:
    GAME_ResetMouseButtonState();
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FB03FA0
//  Loads the panel's five images through UI_LoadImageFromUiDir. Called by UI_InitUiForPlayerUnit.
//      gpPartyPanelImage       Menu\party
//      gpPartyButtonsImage     Menu\partybuttons
//      gpPartyBoxesImage       Menu\partyboxes
//      gpPartyGoldButtonImage  Panel\inv_goldbtn
//      gpPartyBarImage         Menu\partybar
//  The third path is Party.h's gszPartyBoxesPath; the other four are literals here. The gold button's image is the inventory panel's art.
void __cdecl PARTY_LoadImages()
{
    gpPartyPanelImage = UI_LoadImageFromUiDir("Menu\\party");
    gpPartyButtonsImage = UI_LoadImageFromUiDir("Menu\\partybuttons");
    gpPartyBoxesImage = UI_LoadImageFromUiDir(gszPartyBoxesPath);
    gpPartyGoldButtonImage = UI_LoadImageFromUiDir("Panel\\inv_goldbtn");
    gpPartyBarImage = UI_LoadImageFromUiDir("Menu\\partybar");
}

//1.10f: D2Client.0x6FB03FF0
//  Frees the five images and nulls each handle, in the same order, each behind its own NULL test. Called by UI_ResetUiSubsystems.
void __cdecl PARTY_UnloadImages()
{
    if (gpPartyPanelImage)
    {
        ARCHIVE_CellFileFree(gpPartyPanelImage);
        gpPartyPanelImage = NULL;
    }
    if (gpPartyButtonsImage)
    {
        ARCHIVE_CellFileFree(gpPartyButtonsImage);
        gpPartyButtonsImage = NULL;
    }
    if (gpPartyBoxesImage)
    {
        ARCHIVE_CellFileFree(gpPartyBoxesImage);
        gpPartyBoxesImage = NULL;
    }
    if (gpPartyGoldButtonImage)
    {
        ARCHIVE_CellFileFree(gpPartyGoldButtonImage);
        gpPartyGoldButtonImage = NULL;
    }
    if (gpPartyBarImage)
    {
        ARCHIVE_CellFileFree(gpPartyBarImage);
        gpPartyBarImage = NULL;
    }
}

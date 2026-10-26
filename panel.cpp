#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "CoreArchive.h"
#include "Party.h"
#include "panel.h"

//  panel.cpp -- the control panel of D2Client.dll 1.10 and the handlers of the game screen's control bar: the belt (its boxes, hover state, hint, and
//  use by key or click), the panel artwork and its palette cache, the skill buttons, the hostile icon and the hints, the two globes, the experience
//  and stamina bars, the warning icons, and the four mouse handlers of the play area (gGame).
//  The original source file is UI\panel.cpp (the assert in PANEL_BeltUseItemInSlot names it). The unit's functions lie at 0x6FAFE370..0x6FB01C50.
//
//  Layout checks for the constants and offsets this file reads.
static_assert(UI_BELTROWS == 31 && UI_MINISKILL == 3 && UI_ESCMENU == 9,
              "the belt, the mini-skill popup and the ESC menu -- the "
              "three panel ids THIS unit toggles");
static_assert(offsetof(D2InvRectStrc, nLeft) == 0
                  && offsetof(D2InvRectStrc, nRight) == 4,
              "the belt rect's first pair is Left, RIGHT -- NOT Windows' "
              "left, top, and DATATBLS_GetInvRectFromBeltsTxt fills it");

//  __FILE__ and the assert string this unit pushes: the path at 0x6FB7CF50, pushed by PANEL_BeltUseItemInSlot's assert (with "ptItemStats") and by
//  PANEL_DrawSkillButtons. The belt's assert is about an items.txt row, but its source file is this one.
static const char szPanelCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UI\\panel.cpp";
static const char szPtItemStats[] = "ptItemStats";


//1.10f: D2Client.0x6FB01A00
//  The belt's dismiss key: UI_CloseAllPanels(1, 0) and nothing else. gBeltMessageHandlers holds two records, both VK_SPACE -> this function: the belt
//  registers the action in both of its key-binding slots (gBeltPanelKeys entries 0 and 1), and VK_SPACE is only the static default in each, rewritten
//  by UI_ApplyPanelKeyBindings from the player's configuration. Nothing in the image calls this function; its only reference is the table.
void __stdcall PANEL_BeltOnDismissKey(SMSGHANDLER_PARAMS *pMsg)
{
    UI_CloseAllPanels(1, 0);
}

//  The artwork paths, with their addresses in the original. Panel\ctrlpnl_popbelt is not here: it is gszPopBeltPath in panel.h.
static const char szPanelBtn[]         = "Panel\\btn";          // 0x6FB7CF44
static const char szDataLocal[]        = "DATA\\LOCAL";         // 0x6FB74410
static const char szFmtInvChar6[]      = "%s\\UI\\InvChar6";    // 0x6FB7CF34
static const char szFmtInvChar[]       = "%s\\UI\\InvChar";     // 0x6FB7CF24
static const char szPanelInvChar6[]    = "Panel\\InvChar6";     // 0x6FB7CF14
static const char szPanelInvChar[]     = "Panel\\InvChar";      // 0x6FB7CF04
static const char szPanelArrow[]       = "Panel\\Arrow";        // 0x6FB7CEF8
static const char szPanelInvwarn[]     = "Panel\\Invwarn";      // 0x6FB7CEE8
static const char szPanelDuroIcon[]    = "Panel\\DuroIcon";     // 0x6FB7CED8
static const char szPanelGemSocket[]   = "Panel\\GemSocket";    // 0x6FB7CEC8
static const char szPanelHlthmana[]    = "Panel\\Hlthmana";     // 0x6FB7CEA0
static const char szPanelOverlap[]     = "Panel\\overlap";      // 0x6FB7CE90
static const char szPanelMenubutton[]  = "Panel\\menubutton";   // 0x6FB7CE7C
static const char szPanelMenupanel[]   = "Panel\\menupanel";    // 0x6FB7CE6C
static const char szPanelSkillpoints[] = "Panel\\skillpoints";  // 0x6FB7CE58
static const char szPanelRunbutton[]   = "Panel\\runbutton";    // 0x6FB7CE48
static const char szPanelHostilepic[]  = "Panel\\hostilepic";   // 0x6FB7CE34

//1.10f: D2Client.0x6FAFE370
//  Loads the control panel's artwork -- fourteen images, then a cleared block. Its caller is UI_InitUiSubsystems. The language is asked before
//  FOG_IsExpansion, the order of the bytes.
//  The inventory backdrop is the one image with a choice: InvChar6 for an expansion game -- FOG_IsExpansion() and GAME_IsExpansionGame(), both -- and
//  InvChar otherwise. For the four CJK languages, Japanese to Chinese (Traditional), the file comes from DATA\LOCAL\UI\ through sprintf and
//  ARCHIVE_CellFileLoad instead of UI_LoadImageFromUiDir. The range tests are signed (jl, jg), so both sides are cast.
//  The stores are scheduled one load late: from Panel\Arrow on, each `mov ecx, <next path>` is loaded before the previous result is stored. That is
//  the compiler's scheduling, written one load per line. The last act clears gOrbAnimState; PANEL_UnloadImages ends with the same clear.
void __cdecl PANEL_LoadImages()
{
    D2C_Language nLanguage;
    int32_t bExpansion;
    char szPath[260];

    gpImgPanelButton = UI_LoadImageFromUiDir(szPanelBtn);

    nLanguage = D2LANG_STRTABLE_GetLanguage();
    bExpansion = 0;
    if (FOG_IsExpansion() && GAME_IsExpansionGame())
    {
        bExpansion = 1;
    }

    if ((int32_t)nLanguage >= (int32_t)LANGUAGE_JAPANESE && (int32_t)nLanguage <= (int32_t)LANGUAGE_CHINESETWN)
    {
        sprintf(szPath, bExpansion ? szFmtInvChar6 : szFmtInvChar, szDataLocal);
        gpImgInvCharPanel = ARCHIVE_CellFileLoad(szPath, 0);
    }
    else
    {
        gpImgInvCharPanel = UI_LoadImageFromUiDir(bExpansion ? szPanelInvChar6 : szPanelInvChar);
    }

    gpImgPanelArrow = UI_LoadImageFromUiDir(szPanelArrow);
    ghWarnIconCel = UI_LoadImageFromUiDir(szPanelInvwarn);
    gpImgDuroIcon = UI_LoadImageFromUiDir(szPanelDuroIcon);
    gpImgGemSocket = UI_LoadImageFromUiDir(szPanelGemSocket);
    gpImgPopBelt = UI_LoadImageFromUiDir(gszPopBeltPath);
    gpImgGlobeFill = UI_LoadImageFromUiDir(szPanelHlthmana);
    gpImgGlobeOverlay = UI_LoadImageFromUiDir(szPanelOverlap);
    gpImgMiniPanelToggle = UI_LoadImageFromUiDir(szPanelMenubutton);
    gpImgMenuPanel = UI_LoadImageFromUiDir(szPanelMenupanel);
    gpImgSkillPoints = UI_LoadImageFromUiDir(szPanelSkillpoints);
    gpImgRunButton = UI_LoadImageFromUiDir(szPanelRunbutton);
    gpImgHostilePic = UI_LoadImageFromUiDir(szPanelHostilepic);

    memset(gOrbAnimState, 0, sizeof(gOrbAnimState));   //  clears the whole array
}

//1.10f: D2Client.0x6FAFE4D0
//  Releases the control panel's artwork -- each handle null-tested, released through ARCHIVE_CellFileFree and nulled, then the same clear of
//  gOrbAnimState. Its caller is UI_ShutdownUiSubsystems.
//  It releases seventeen handles, not fourteen: gPanel800CtrlPnl7Image, gPanel800BorderImage and gPanelCtrlPnl7Image, which PANEL_LoadImages never
//  loads, are released too, in the middle of the run. The releases are written out one by one, as the bytes have them.
void __cdecl PANEL_UnloadImages()
{
    if (gpImgPanelButton)
    {
        ARCHIVE_CellFileFree(gpImgPanelButton);
        gpImgPanelButton = NULL;
    }
    if (gpImgInvCharPanel)
    {
        ARCHIVE_CellFileFree(gpImgInvCharPanel);
        gpImgInvCharPanel = NULL;
    }
    if (gpImgPanelArrow)
    {
        ARCHIVE_CellFileFree(gpImgPanelArrow);
        gpImgPanelArrow = NULL;
    }
    if (ghWarnIconCel)
    {
        ARCHIVE_CellFileFree(ghWarnIconCel);
        ghWarnIconCel = NULL;
    }
    if (gpImgDuroIcon)
    {
        ARCHIVE_CellFileFree(gpImgDuroIcon);
        gpImgDuroIcon = NULL;
    }
    if (gpImgGemSocket)
    {
        ARCHIVE_CellFileFree(gpImgGemSocket);
        gpImgGemSocket = NULL;
    }
    if (gpImgPopBelt)
    {
        ARCHIVE_CellFileFree(gpImgPopBelt);
        gpImgPopBelt = NULL;
    }
    if (gPanel800CtrlPnl7Image)
    {
        ARCHIVE_CellFileFree(gPanel800CtrlPnl7Image);
        gPanel800CtrlPnl7Image = NULL;
    }
    if (gPanel800BorderImage)
    {
        ARCHIVE_CellFileFree(gPanel800BorderImage);
        gPanel800BorderImage = NULL;
    }
    if (gPanelCtrlPnl7Image)
    {
        ARCHIVE_CellFileFree(gPanelCtrlPnl7Image);
        gPanelCtrlPnl7Image = NULL;
    }
    if (gpImgGlobeFill)
    {
        ARCHIVE_CellFileFree(gpImgGlobeFill);
        gpImgGlobeFill = NULL;
    }
    if (gpImgGlobeOverlay)
    {
        ARCHIVE_CellFileFree(gpImgGlobeOverlay);
        gpImgGlobeOverlay = NULL;
    }
    if (gpImgMiniPanelToggle)
    {
        ARCHIVE_CellFileFree(gpImgMiniPanelToggle);
        gpImgMiniPanelToggle = 0;
    }
    if (gpImgMenuPanel)
    {
        ARCHIVE_CellFileFree(gpImgMenuPanel);
        gpImgMenuPanel = NULL;
    }
    if (gpImgSkillPoints)
    {
        ARCHIVE_CellFileFree(gpImgSkillPoints);
        gpImgSkillPoints = NULL;
    }
    if (gpImgRunButton)
    {
        ARCHIVE_CellFileFree(gpImgRunButton);
        gpImgRunButton = NULL;
    }
    if (gpImgHostilePic)
    {
        ARCHIVE_CellFileFree(gpImgHostilePic);
        gpImgHostilePic = NULL;
    }

    memset(gOrbAnimState, 0, sizeof(gOrbAnimState));   //  clears the whole array
}

//1.10f: D2Client.0x6FAFE930
//  gnStaminaBarColours[0] = MixRGB(255, 0, 0). void: nothing reads the al the original leaves.
void __cdecl PANEL_CacheStaminaColourRed()
{
    gnStaminaBarColours[0] = D2WIN_10034_MixRGB(0xFF, 0, 0);
}

//1.10f: D2Client.0x6FAFE950
//  gnStaminaBarColours[1] = MixRGB(0xF4, 0xC0, 0x4C), amber. void: nothing reads the al the original leaves.
void __cdecl PANEL_CacheStaminaColourAmber()
{
    gnStaminaBarColours[1] = D2WIN_10034_MixRGB(0xF4, 0xC0, 0x4C);
}

//1.10f: D2Client.0x6FAFE970
//  gnStaminaBarColours[2] = MixRGB(0, 0, 255).
//  void: the index is left in al for UI_CachePaletteIndices' tail jump to hand on, and nothing reads it.
void __cdecl PANEL_CacheStaminaColourBlue()
{
    uint8_t nIndex;

    nIndex = D2WIN_10034_MixRGB(0, 0, 0xFF);
    gnStaminaBarColours[2] = nIndex;
}

//1.10f: D2Client.0x6FAFE990
//  All four gnPanelHighlightColours at half intensity, in this order: MixRGB(128, 0, 0), (0, 128, 0), (0, 0, 128) and (128, 128, 0).
void __cdecl PANEL_CacheHighlightColours()
{
    gnPanelHighlightColours[0] = D2WIN_10034_MixRGB(0x80, 0, 0);
    gnPanelHighlightColours[1] = D2WIN_10034_MixRGB(0, 0x80, 0);
    gnPanelHighlightColours[2] = D2WIN_10034_MixRGB(0, 0, 0x80);
    gnPanelHighlightColours[3] = D2WIN_10034_MixRGB(0x80, 0x80, 0);
}

//1.10f: D2Client.0x6FAFE9E0
//  `return gnHintPhaseSuppressed != 0;`. The flag's only writer is PANEL_DrawBelt; its readers through this getter are the item panels' mouse
//  handlers, INV_DrawItemDescription and the mini-panel.
int32_t __fastcall PANEL_InvHintPhaseSuppressed()
{
    return gnHintPhaseSuppressed != 0;
}

//1.10f: D2Client.0x6FAFFF90
//  The run button's rectangle: x in [width/2 - 145, width/2 - 128] and y in [height - 28, height - 8], every edge inclusive and the halving signed.
//  Its one caller is PANEL_Draw800BorderFrame.
BOOL __fastcall PANEL_IsPointOverRunButton(int32_t nXPos, int32_t nYPos)
{
    if (nXPos >= gnScreenWidth / 2 - 145 && nXPos <= gnScreenWidth / 2 - 128 && nYPos >= gnScreenHeight - 28 && nYPos <= gnScreenHeight - 8)
    {
        return TRUE;
    }
    return FALSE;
}

//1.10f: D2Client.0x6FB00C10
//  When the player's room is known and its level id differs from gnPanelLastLevelId, the id is remembered and gnPanelLevelChangeTimer is set to 60,
//  which nothing reads; reproduced whole. The one call is PANEL_Draw800BorderFrame's tail.
void __cdecl PANEL_ArmLevelChangeTimer()
{
    D2ActiveRoomStrc *pRoom;
    int32_t nLevelId;

    pRoom = CUNIT_GetPlayerRoom();
    if (!pRoom)
    {
        return;
    }

    nLevelId = D2COMMON_DUNGEON_GetLevelIdFromRoom(pRoom);
    if (gnPanelLastLevelId != nLevelId)
    {
        gnPanelLastLevelId = nLevelId;
        gnPanelLevelChangeTimer = 60;
    }
}

//1.10f: D2Client.0x6FB01C30
//  gnUiBoxFillColor = MixRGB(255, 0, 0). PANEL_SetBoxFillColorBlack follows at every call site and writes the same byte, so this store never
//  survives; both calls are the original's. void: nothing reads the al the original leaves.
void __cdecl PANEL_SetBoxFillColorRed()
{
    gnUiBoxFillColor = D2WIN_10034_MixRGB(0xFF, 0, 0);
}

//1.10f: D2Client.0x6FB01C50
//  gnUiBoxFillColor = MixRGB(0, 0, 0). Called by the three palette-cache runs in ui.cpp, CHAR_CharPanelDraw and HELP_DimGameScreen. void: nothing
//  reads the al the original leaves.
void __cdecl PANEL_SetBoxFillColorBlack()
{
    gnUiBoxFillColor = D2WIN_10034_MixRGB(0, 0, 0);
}

//  The control panel's buttons and hints. Every position is an immediate against the screen size. The mini-panel toggle's rectangle, [width/2 - 8,
//  width/2 + 5] x [height - 39, height - 13], is tested by two of them and is the one PANEL_GameOnLButtonDown tests before it sets
//  gbMiniPanelTogglePressed.

static_assert(UI_AUTOMAP == 10 && HOTKEY_RUNWALK == 35 && SKILL_ATTACK == 0,
              "`mov ecx, 0Ah` at 0x6FAFE6DE, `mov ecx, 23h` at 0x6FAFFEB2, and "
              "`push 0` into SKILLS_SetLeftActiveSkill at 0x6FAFE89D");
static_assert(SKILLSFLAGINDEX_INTOWN == 8,
              "PANEL_SpellSelDrawSkillButton tests byte 1 of dwFlags against "
              "gdwBitMasks[0] (0x6FAFE778) -- how flag index 8 compiles");
static_assert(STR_IDX_4167_panelmini == 0x1047 && STR_IDX_4168_panelcmini == 0x1048
                  && STR_IDX_4178_minipanelspecial == 0x1052
                  && STR_IDX_4179_RunOn == 0x1053,
              "the four tbl immediates, 0x1047 / 0x1048 at 0x6FB00CA6 / 0x6FB00C91 "
              "and 0x1052 / 0x1053 at 0x6FAFFEEA / 0x6FAFFE97");

//1.10f: D2Client.0x6FAFE650
//  The hostile icon, top right: gpImgHostilePic at (width - 58, 45), or at (width - 58, 115) with the automap open. Its one caller is
//  UIPANELS_DrawInGamePanels.
//  Drawn only when ROSTER_IsAnyoneHostileToPlayer, none of STASH, NPCSHOP, MSGLOG, INVENTORY and SKILLTREE is open, and the trade window is closed or
//  a trade is in progress. The cel context on the stack is zeroed and given only the image.
void __cdecl PANEL_DrawHostileIndicator()
{
    D2GfxDataStrc tCelContext;
    int32_t bPanelOpen;
    int32_t nAutomapYOffset;

    memset(&tCelContext, 0, sizeof(tCelContext));
    bPanelOpen = 0;
    nAutomapYOffset = 0;

    if (!ROSTER_IsAnyoneHostileToPlayer())
    {
        return;
    }

    if (UI_GetUiWindowState(UI_STASH) || UI_GetUiWindowState(UI_NPCSHOP) || UI_GetUiWindowState(UI_MSGLOG) || UI_GetUiWindowState(UI_INVENTORY) || UI_GetUiWindowState(UI_SKILLTREE))
    {
        bPanelOpen = 1;
    }

    if (UI_GetUiWindowState(UI_MPTRADE) && !TRADE_IsTradeInProgress())
    {
        return;
    }

    if (bPanelOpen)
    {
        return;
    }

    tCelContext.pCellFile = gpImgHostilePic;
    tCelContext.nDirection = 0;
    tCelContext.nFrame = 0;
    if (UI_GetUiWindowState(UI_AUTOMAP))
    {
        nAutomapYOffset = 70;
    }
    D2GFX_TEXTURE_CelDraw(&tCelContext, gnScreenWidth - 58, nAutomapYOffset + 45, 0xFFFFFFFF, 1, NULL);
}

//1.10f: D2Client.0x6FAFE730
//  One skill button: the icon, then the tooltip the skill needs. Two calls, both from PANEL_DrawSkillButtons -- bLeftHand 1 for the left button and 0
//  for the right -- and the flag goes untouched into SPELLSEL_DrawSkillButtonIcon.
//  A skill without the InTown flag (SKILLSFLAGINDEX_INTOWN: byte 1 of the skills.txt flags against gdwBitMasks[0]), drawn while the player's room is
//  a town, gets icon state 1 whatever SPELLSEL_GetSkillIconState said.
//  Level and charges: SKILLS_GetSkillInfo fills owner, level and charges for an item-granted skill. When it fails, the level comes from
//  SKILLS_GetSkillLevel and a quantity from SKILLS_GetQuantity, and a non-zero quantity takes the charges tooltip over the finishing-move one. The
//  answer is tested twice, once before each call, as the bytes have it.
void __fastcall PANEL_SpellSelDrawSkillButton(D2UnitStrc *pUnit, D2SkillStrc *pSkill, int32_t nXPos, int32_t nYPos, int32_t bLeftHand)
{
    D2SkillsTxt *pSkillsTxt;
    D2CellFileStrc *hIcon;
    int32_t nIconState;
    D2ActiveRoomStrc *pRoom;
    D2UnitGUID nOwnerGUID;
    int32_t nSkillLevel;
    int32_t nCharges;
    BOOL bInfo;
    int32_t nQuantity;

    if (!pUnit || !pSkill)
    {
        return;
    }

    pSkillsTxt = D2COMMON_SKILLS_GetSkillsTxtRecordFromSkill(pSkill);
    if (!pSkillsTxt)
    {
        return;
    }

    hIcon = SPELLSEL_GetSkillIconImage(&pSkillsTxt->nSkillId);
    nIconState = SPELLSEL_GetSkillIconState(pUnit, pSkill);
    if (!(((uint8_t *)pSkillsTxt->dwFlags)[SKILLSFLAGINDEX_INTOWN / 8] & (uint8_t)gdwBitMasks[SKILLSFLAGINDEX_INTOWN % 8]))
    {
        pRoom = D2COMMON_UNITS_GetRoom(pUnit);
        if (pRoom && D2COMMON_DUNGEON_IsRoomInTown(pRoom))
        {
            nIconState = 1;
        }
    }

    nOwnerGUID = (D2UnitGUID)-1;
    nSkillLevel = 0;
    nCharges = 0;
    bInfo = D2COMMON_SKILLS_GetSkillInfo(pSkill, &nOwnerGUID, NULL, &nSkillLevel, &nCharges);
    if (!bInfo)
    {
        nSkillLevel = D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill, 1);
    }
    nQuantity = 0;
    if (!bInfo)
    {
        nQuantity = D2COMMON_SKILLS_GetQuantity(pSkill);
    }

    SPELLSEL_DrawSkillButtonIcon(hIcon, &pSkillsTxt->nSkillId, nOwnerGUID, nCharges, nIconState, nXPos, nYPos, bLeftHand);
    if (nQuantity)
    {
        SPELLSEL_DrawSkillChargesTooltip(&pSkillsTxt->nSkillId, nIconState, nQuantity, nXPos, nYPos);
    }
    else
    {
        SPELLSEL_DrawFinishingMoveTooltip(&pSkillsTxt->nSkillId, nIconState, nSkillLevel, nCharges, nXPos, nYPos);
    }
}

//1.10f: D2Client.0x6FAFE850
//  The two skill buttons: left at x 117, right at width - 165, both at the screen height. Its one caller is PANEL_Draw800BorderFrame.
//  It repairs the selection before drawing it: a selected skill whose level, bonuses included, is not above 0 is removed from the player, the side
//  falls back to SKILL_ATTACK with no owner, and the skill is read again. The two sides have the same shape.
//  SKILLS_GetSkillIdFromSkill and SKILLS_RemoveSkill each take a file and a line: szPanelCpp with 640 (0x280) for the left side and 651 (0x28B) for
//  the right.
void __cdecl PANEL_DrawSkillButtons()
{
    D2UnitStrc *pPlayer;
    D2SkillStrc *pSkill;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        return;
    }

    pSkill = D2COMMON_UNITS_GetLeftSkill(pPlayer);
    if (pSkill && D2COMMON_SKILLS_GetSkillLevel(pPlayer, pSkill, 1) <= 0)
    {
        D2COMMON_SKILLS_RemoveSkill(pPlayer, (D2C_Skills)D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szPanelCpp, 640), szPanelCpp, 640);
        D2COMMON_SKILLS_SetLeftActiveSkill(pPlayer, SKILL_ATTACK, (D2UnitGUID)-1);
        pSkill = D2COMMON_UNITS_GetLeftSkill(pPlayer);
    }
    PANEL_SpellSelDrawSkillButton(pPlayer, pSkill, 117, gnScreenHeight, 1);

    pSkill = D2COMMON_UNITS_GetRightSkill(pPlayer);
    if (pSkill && D2COMMON_SKILLS_GetSkillLevel(pPlayer, pSkill, 1) <= 0)
    {
        D2COMMON_SKILLS_RemoveSkill(pPlayer, (D2C_Skills)D2COMMON_SKILLS_GetSkillIdFromSkill(pSkill, szPanelCpp, 651), szPanelCpp, 651);
        D2COMMON_SKILLS_SetRightActiveSkill(pPlayer, SKILL_ATTACK, (D2UnitGUID)-1);
        pSkill = D2COMMON_UNITS_GetRightSkill(pPlayer);
    }
    PANEL_SpellSelDrawSkillButton(pPlayer, pSkill, gnScreenWidth - 165, gnScreenHeight, 0);
}

//1.10f: D2Client.0x6FAFFE60
//  The run hint: tbl 4179 `Run`, then for each key bound to HOTKEY_RUNWALK -- slot 1 first, then slot 0 -- tbl 4178 ` (%s)` filled with that key's
//  name and appended. Framed at (width/2 - 145, height - 23), colour 0, alignment 1. Its one caller is PANEL_Draw800BorderFrame.
//  Both buffers are arrays of Unicode, whose imported default constructor runs over them (200 and 30 elements). Each arm zeroes the fragment again
//  before sprintf fills it, and fetches the key's name before the format. CMDTBL_ControlsFindBoundKey answers 16 bits, compared with 0xFFFF.
//  Unicode::strcat's operands are the text, then the fragment.
void __cdecl PANEL_DrawRunHint()
{
    Unicode wszText[200];
    Unicode wszFragment[30];
    const Unicode *wszKeyName;

    Unicode::strcpy(wszText, D2LANG_GetStringFromTblIndex(STR_IDX_4179_RunOn));

    if (CMDTBL_ControlsFindBoundKey(HOTKEY_RUNWALK, 1) != -1)
    {
        memset(wszFragment, 0, sizeof(wszFragment));
        wszKeyName = CMDTBL_ControlsGetBoundKeyName(HOTKEY_RUNWALK, 1);
        Unicode::sprintf(30, wszFragment, D2LANG_GetStringFromTblIndex(STR_IDX_4178_minipanelspecial), wszKeyName);
        Unicode::strcat(wszText, wszFragment);
    }

    if (CMDTBL_ControlsFindBoundKey(HOTKEY_RUNWALK, 0) != -1)
    {
        memset(wszFragment, 0, sizeof(wszFragment));
        wszKeyName = CMDTBL_ControlsGetBoundKeyName(HOTKEY_RUNWALK, 0);
        Unicode::sprintf(30, wszFragment, D2LANG_GetStringFromTblIndex(STR_IDX_4178_minipanelspecial), wszKeyName);
        Unicode::strcat(wszText, wszFragment);
    }

    D2WIN_10129_DrawFramedText(wszText, gnScreenWidth / 2 - 145, gnScreenHeight - 23, 0, 1);
}

//1.10f: D2Client.0x6FB001B0
//  The mini-panel toggle button at (width/2 - 8, height - 16), from gpImgMiniPanelToggle in draw mode 5. Frame 0 or 1 with the mini-panel closed, 2
//  or 3 with it open; the odd frame is pressed, which needs both gbMiniPanelTogglePressed and the cursor inside the toggle's rectangle, so the button
//  lets go visually while the mouse is dragged off it.
//  UI_MINIPANEL is asked first, the flag is loaded once before the branch on it, and only then is the mouse read -- Y then X in both arms. The two
//  arms are the same test with different frames, written here as one test and a choice; the call order is the bytes'.
void __cdecl PANEL_DrawMiniPanelToggleButton()
{
    D2GfxDataStrc tCelContext;
    int32_t bMiniPanelOpen;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t bPressedOver;

    memset(&tCelContext, 0, sizeof(tCelContext));
    tCelContext.pCellFile = gpImgMiniPanelToggle;
    tCelContext.nDirection = 0;

    bMiniPanelOpen = UI_GetUiWindowState(UI_MINIPANEL);
    bPressedOver = 0;
    if (gbMiniPanelTogglePressed)
    {
        nMouseY = CURSOR_GetMouseY();
        nMouseX = CURSOR_GetMouseX();
        if (nMouseX >= gnScreenWidth / 2 - 8 && nMouseX <= gnScreenWidth / 2 + 5 && nMouseY >= gnScreenHeight - 39 && nMouseY <= gnScreenHeight - 13)
        {
            bPressedOver = 1;
        }
    }

    if (bMiniPanelOpen)
    {
        tCelContext.nFrame = bPressedOver ? 3 : 2;
    }
    else
    {
        tCelContext.nFrame = bPressedOver ? 1 : 0;
    }

    D2GFX_TEXTURE_CelDraw(&tCelContext, gnScreenWidth / 2 - 8, gnScreenHeight - 16, 0xFFFFFFFF, 5, NULL);
}

//1.10f: D2Client.0x6FB00C40
//  The toggle's hover label: with the cursor inside the toggle's rectangle, tbl 4168 `Close Mini Panel` when the mini-panel is open and 4167 `Open
//  Mini Panel` when it is not, framed at (width/2 - 1, height - 39), colour 0, alignment 1.
//  Reached by a tail jump from PANEL_Draw800BorderFrame. The mouse is read X then Y here, the other order from the button's.
void __cdecl PANEL_DrawMinipanelToggleHint()
{
    int32_t nMouseX;
    int32_t nMouseY;
    D2C_StringIndices nString;

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    if (nMouseX >= gnScreenWidth / 2 - 8 && nMouseX <= gnScreenWidth / 2 + 5 && nMouseY >= gnScreenHeight - 39 && nMouseY <= gnScreenHeight - 13)
    {
        nString = UI_GetUiWindowState(UI_MINIPANEL) ? STR_IDX_4168_panelcmini : STR_IDX_4167_panelmini;
        D2WIN_10129_DrawFramedText(D2LANG_GetStringFromTblIndex(nString), gnScreenWidth / 2 - 1, gnScreenHeight - 39, 0, 1);
    }
}

//  The globes, the smoothing behind them, and the readout that appears when the cursor is over one. Every stat here is 8.8 fixed point.
//  PANEL_GetSmoothedStatValue returns it unshifted; the globes divide it by an equally unshifted maximum, so the fraction cancels; only
//  PANEL_DrawOrbHoverText shifts both sides down by 8 before printing.
//  D2GFX_GetDisplayType is asked three times and used once: it decides one pixel of the health globe's regen band (28 against 29) and one of the mana
//  globe's (width - 112 against - 111). The other two calls are dead: in PANEL_DrawHealthGlobe the branch on display type 1 or 3 reaches two call
//  sites whose pushed arguments are identical (0x6FAFFAF1 and 0x6FAFFB08), and in PANEL_DrawManaGlobe (0x6FAFFC36) the answer is dropped without a
//  branch. Both calls are reproduced.
//  PANEL_DrawOrbHoverText runs the Unicode constructor over three arrays of 100 and writes two; the one at [esp+0x1A4] is never touched after its
//  loop at 0x6FAFF0F4. Those 100 calls are reproduced.

static_assert(STATE_POISON == 2 && STATE_HEALTHPOT == 100 && STATE_MANAPOT == 106,
              "the three states these globes push: `push 2` at 0x6FAFFAC8, "
              "`push 64h` at 0x6FAFFA31 and `push 6Ah` at 0x6FAFFBA1");
static_assert(STR_IDX_4165_panelhealth == 0x1045 && STR_IDX_4166_panelmana == 0x1046,
              "`mov ecx, 1045h` at 0x6FAFF18B and `mov ecx, 1046h` at 0x6FAFF25D");

//1.10f: D2Client.0x6FAFF2D0
//  The value a globe is animating towards, in 8.8 fixed point. Four callers: both globes, the hover text and PANEL_DrawStaminaBar. Exactly three
//  stats are accepted -- 6 life, 8 mana, 10 stamina -- each with its own maximum and its own orb slot; anything else returns 0, and so does a unit
//  whose maximum is 0.
//  gOrbAnimState is int[12], indexed 4 * slot:
//
//      +0  the value being animated TO      +1  the frame that was set on
//      +2  the value being animated FROM    +3  the frame THAT was set on
//
//  While GAME_GetFrameCount() is 0 or 1 (an unsigned compare) both values are seeded with the true one, so a globe starts full instead of filling up
//  over the first frames. The life orb alone also snaps both whenever the value drops (bNotLife is cleared only in the life arm), which is why damage
//  shows at once and healing slides.
//  A change smaller than max/8 keeps the old start frame and the old start value -- unless the new value is the maximum, which starts from itself --
//  so small ticks ride the animation already running. A change of max/8 or more restarts from the current value at the current frame.
//  The lerp is x87 in the original (fild (nFramesElapsed + 1), fidiv nSpanFrames, fimul nSpanValue, then __ftol, which truncates toward zero as a C
//  cast does). The span is clamped to 7..15 frames, so an orb takes between seven and fifteen frames however far it has to travel.
int32_t __fastcall PANEL_GetSmoothedStatValue(int32_t nStatId)
{
    D2UnitStrc *pPlayer;
    int32_t nValue;
    int32_t nMax;
    int32_t nOrbSlot;
    int32_t nOrbBase;
    int32_t bNotLife;
    int32_t nLastValue;
    int32_t nDelta;
    int32_t nLastFrame;
    int32_t nFramesElapsed;
    int32_t nSpanFrames;
    int32_t nFromValue;
    int32_t nSpanValue;

    bNotLife = 1;
    pPlayer = CUNIT_GetPlayer();
    nValue = D2COMMON_STATLIST_UnitGetStatValue(pPlayer, (D2C_ItemStats)nStatId, 0);

    if (nStatId == STAT_HITPOINTS)
    {
        nMax = D2COMMON_STATLIST_GetMaxLifeFromUnit(pPlayer);
        nOrbSlot = 0;
        bNotLife = 0;
    }
    else if (nStatId == STAT_MANA)
    {
        nMax = D2COMMON_STATLIST_GetMaxManaFromUnit(pPlayer);
        nOrbSlot = 1;
    }
    else if (nStatId == STAT_STAMINA)
    {
        nMax = D2COMMON_STATLIST_GetMaxStaminaFromUnit(pPlayer);
        nOrbSlot = 2;
    }
    else
    {
        return 0;
    }

    if (!nMax)
    {
        return 0;
    }

    nOrbBase = 4 * nOrbSlot;

    if (GAME_GetFrameCount() <= 1)
    {
        gOrbAnimState[nOrbBase + 2] = nValue;
        gOrbAnimState[nOrbBase] = nValue;
    }

    if (!bNotLife && nValue < gOrbAnimState[nOrbBase])
    {
        gOrbAnimState[nOrbBase + 2] = nValue;
        gOrbAnimState[nOrbBase] = nValue;
        gOrbAnimState[nOrbBase + 1] = GAME_GetFrameCount();
        gOrbAnimState[nOrbBase + 3] = gOrbAnimState[nOrbBase + 1];
    }

    nLastValue = gOrbAnimState[nOrbBase];
    if (nLastValue != nValue)
    {
        nDelta = nLastValue - nValue;
        if (nDelta < 0)
        {
            nDelta = -nDelta;
        }
        if (nDelta < nMax / 8)
        {
            gOrbAnimState[nOrbBase + 3] = gOrbAnimState[nOrbBase + 1];
            if (nValue == nMax)
            {
                gOrbAnimState[nOrbBase + 2] = nValue;
            }
            else
            {
                gOrbAnimState[nOrbBase + 2] = nLastValue;
            }
        }
        else
        {
            gOrbAnimState[nOrbBase + 2] = nValue;
            gOrbAnimState[nOrbBase + 3] = GAME_GetFrameCount();
        }
        gOrbAnimState[nOrbBase] = nValue;
        gOrbAnimState[nOrbBase + 1] = GAME_GetFrameCount();
    }

    nLastFrame = gOrbAnimState[nOrbBase + 1];
    nFramesElapsed = GAME_GetFrameCount() - nLastFrame;
    nSpanFrames = nLastFrame - gOrbAnimState[nOrbBase + 3];
    if (nSpanFrames < 7)
    {
        nSpanFrames = 7;
    }
    else if (nSpanFrames > 15)
    {
        nSpanFrames = 15;
    }

    if (nValue > 0 && nFramesElapsed < nSpanFrames)
    {
        nFromValue = gOrbAnimState[nOrbBase + 2];
        nSpanValue = gOrbAnimState[nOrbBase] - nFromValue;
        if (nSpanValue)
        {
            nValue = nFromValue + (int32_t)((double)(nFramesElapsed + 1) / (double)nSpanFrames * (double)nSpanValue);
        }
    }

    if (nValue < 0)
    {
        return 0;
    }
    if (nValue > nMax)
    {
        return nMax;
    }
    return nValue;
}

//1.10f: D2Client.0x6FAFF0B0
//  The `Life: x / y` readout under a globe the cursor is over. Its one caller is UIPANELS_DrawInGamePanels.
//  Two hard-coded rectangles, both 81 by 61 inclusive and anchored to the bottom corners: life globe x 30 .. 110, y height-75 .. height-15; mana
//  globe x width-111 .. width-31, y height-75 .. height-15. The life side is tested first and the mana side only when it misses.
//  The `nearly dead` floor is a signed pair of tests (jl, jg): a live player (unit type 0, anim mode not PLRMODE_DEAD) whose shifted life is 0 or 1
//  prints 1. The text is built into one buffer and appended to another with Unicode::strcat (the text, then the line) and centred by hand: `65 -
//  width/2` on the life side and `gnScreenWidth - width/2 - 65` on the mana side, both at height - 95.
void __cdecl PANEL_DrawOrbHoverText()
{
    D2UnitStrc *pPlayer;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nValue;
    int32_t nMax;
    int32_t nTextWidth;
    Unicode wszText[100];
    //  Constructed by the original and never used.
    Unicode wszUnused[100];

    pPlayer = CUNIT_GetPlayer();
    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();

    if (nMouseX >= 30 && nMouseX <= 110 && nMouseY >= gnScreenHeight - 75 && nMouseY <= gnScreenHeight - 15)
    {
        D2UnitStrc *pUnit;
        Unicode wszLine[100];

        nValue = PANEL_GetSmoothedStatValue(STAT_HITPOINTS) >> 8;
        nMax = D2COMMON_STATLIST_GetMaxLifeFromUnit(pPlayer) >> 8;

        pUnit = CUNIT_GetPlayer();
        if (pUnit && pUnit->dwUnitType == UNIT_PLAYER && pUnit->dwAnimMode != PLRMODE_DEAD && nValue >= 0 && nValue <= 1)
        {
            nValue = 1;
        }

        Unicode::sprintf(100, wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_4165_panelhealth), nValue, nMax);
        Unicode::strcat(wszText, wszLine);
        nTextWidth = D2WIN_10121_GetTextWidth(wszText);
        D2WIN_10117_DrawText(wszText, 65 - nTextWidth / 2, gnScreenHeight - 95, 0, 0);
    }
    else if (nMouseX >= gnScreenWidth - 111 && nMouseX <= gnScreenWidth - 31 && nMouseY >= gnScreenHeight - 75 && nMouseY <= gnScreenHeight - 15)
    {
        Unicode wszLine[100];

        nValue = PANEL_GetSmoothedStatValue(STAT_MANA) >> 8;
        nMax = D2COMMON_STATLIST_GetMaxManaFromUnit(pPlayer) >> 8;

        Unicode::sprintf(100, wszLine, D2LANG_GetStringFromTblIndex(STR_IDX_4166_panelmana), nValue, nMax);
        Unicode::strcat(wszText, wszLine);
        nTextWidth = D2WIN_10121_GetTextWidth(wszText);
        D2WIN_10117_DrawText(wszText, gnScreenWidth - nTextWidth / 2 - 65, gnScreenHeight - 95, 0, 0);
    }
}

//1.10f: D2Client.0x6FAFF9D0
//  The health globe: 80 scanlines of gpImgGlobeFill from y = height - 13 upward at x 29, then gpImgGlobeOverlay over it at (28, height - 5). Its one
//  caller is PANEL_Draw800BorderFrame. Nothing is drawn when the maximum is 0.
//  Three things the mana globe does not do: a fill of 1 or 2 lines is forced up to 2 for a live player, so a nearly-dead character's globe never
//  reads as empty; STATE_POISON switches the fill to cel frame 2; the smoothed value is not clamped to the maximum first.
//  While STATE_HEALTHPOT is on, a band is drawn from the current height up to `80 * STAT_HPREGEN / 100`, capped at 80 lines and drawn only when it is
//  ahead of the liquid. That band is the one place the display type matters: x 28 for display type 6, x 29 otherwise.
//  Both globes' overlays are drawn with eDrawMode 5; what differs is the cel frame in nFrame, 0 here and 1 there.
void __cdecl PANEL_DrawHealthGlobe()
{
    D2UnitStrc *pPlayer;
    D2UnitStrc *pUnit;
    int32_t nValue;
    int32_t nMax;
    int32_t nLifeLines;
    int32_t nRegenLines;
    D2GfxDataStrc tCelContext;

    pPlayer = CUNIT_GetPlayer();
    nValue = PANEL_GetSmoothedStatValue(STAT_HITPOINTS);
    nMax = D2COMMON_STATLIST_GetMaxLifeFromUnit(pPlayer);
    if (!nMax)
    {
        return;
    }

    nLifeLines = 80 * nValue / nMax;
    if (nLifeLines > 0 && nLifeLines <= 2)
    {
        pUnit = CUNIT_GetPlayer();
        if (pUnit && pUnit->dwUnitType == UNIT_PLAYER && pUnit->dwAnimMode != PLRMODE_DEAD)
        {
            nLifeLines = 2;
        }
    }

    memset(&tCelContext, 0, sizeof(tCelContext));
    tCelContext.nDirection = 0;
    tCelContext.pCellFile = gpImgGlobeFill;
    tCelContext.nFrame = 0;

    if (D2COMMON_STATES_CheckState(CUNIT_GetPlayer(), STATE_HEALTHPOT))
    {
        nRegenLines = 80 * D2COMMON_STATLIST_UnitGetStatValue(CUNIT_GetPlayer(), STAT_HPREGEN, 0) / 100;
        if (nRegenLines > 80)
        {
            nRegenLines = 80;
        }
        if (nRegenLines > nLifeLines)
        {
            if (D2GFX_GetDisplayType() == 6)
            {
                D2GFX_TEXTURE_CelDrawEx(&tCelContext, 28, gnScreenHeight - 13, nLifeLines, nRegenLines - nLifeLines, 0);
            }
            else
            {
                D2GFX_TEXTURE_CelDrawEx(&tCelContext, 29, gnScreenHeight - 13, nLifeLines, nRegenLines - nLifeLines, 0);
            }
        }
    }

    tCelContext.nFrame = D2COMMON_STATES_CheckState(CUNIT_GetPlayer(), STATE_POISON) ? 2 : 0;

    if (nLifeLines)
    {
        //  The branch on the display type reaches two identical calls; the call stays because the bytes make it.
        D2GFX_GetDisplayType();
        D2GFX_TEXTURE_CelDrawEx(&tCelContext, 29, gnScreenHeight - 13, 0, nLifeLines, 5);
    }

    tCelContext.pCellFile = gpImgGlobeOverlay;
    tCelContext.nFrame = 0;
    D2GFX_TEXTURE_CelDraw(&tCelContext, 28, gnScreenHeight - 5, 0xFFFFFFFF, 5, NULL);
}

//1.10f: D2Client.0x6FAFFB60
//  The mana globe, the health globe's twin at the other corner: the fill at (width - 111, height - 13) and the overlay at (width - 110, height - 9),
//  cel frame 1 in both. Its one caller is PANEL_Draw800BorderFrame.
//  What differs from the health globe: the smoothed value is clamped to the maximum before the division, there is no floor at two lines and no
//  poisoned frame, and the fill is drawn even when it is zero lines high -- the `if (nLines)` the health globe has is not here. STATE_MANAPOT gets
//  the same preview band, from STAT_MANARECOVERY, at width - 112 for display type 6 and width - 111 otherwise.
void __cdecl PANEL_DrawManaGlobe()
{
    D2UnitStrc *pPlayer;
    int32_t nValue;
    int32_t nMax;
    int32_t nManaLines;
    int32_t nRecoveryLines;
    int32_t nXPos;
    D2GfxDataStrc tCelContext;

    pPlayer = CUNIT_GetPlayer();
    nValue = PANEL_GetSmoothedStatValue(STAT_MANA);
    nMax = D2COMMON_STATLIST_GetMaxManaFromUnit(pPlayer);
    if (!nMax)
    {
        return;
    }

    if (nValue > nMax)
    {
        nValue = nMax;
    }
    nManaLines = 80 * nValue / nMax;

    memset(&tCelContext, 0, sizeof(tCelContext));
    tCelContext.nDirection = 0;
    tCelContext.pCellFile = gpImgGlobeFill;
    tCelContext.nFrame = 1;

    if (D2COMMON_STATES_CheckState(CUNIT_GetPlayer(), STATE_MANAPOT))
    {
        nRecoveryLines = 80 * D2COMMON_STATLIST_UnitGetStatValue(CUNIT_GetPlayer(), STAT_MANARECOVERY, 0) / 100;
        if (nRecoveryLines > 80)
        {
            nRecoveryLines = 80;
        }
        if (nRecoveryLines > nManaLines)
        {
            if (D2GFX_GetDisplayType() == 6)
            {
                nXPos = gnScreenWidth - 112;
            }
            else
            {
                nXPos = gnScreenWidth - 111;
            }
            D2GFX_TEXTURE_CelDrawEx(&tCelContext, nXPos, gnScreenHeight - 13, nManaLines, nRecoveryLines - nManaLines, 0);
        }
    }

    //  Asked and dropped, without even a branch.
    D2GFX_GetDisplayType();
    D2GFX_TEXTURE_CelDrawEx(&tCelContext, gnScreenWidth - 111, gnScreenHeight - 13, 0, nManaLines, 5);

    tCelContext.pCellFile = gpImgGlobeOverlay;
    tCelContext.nFrame = 1;
    D2GFX_TEXTURE_CelDraw(&tCelContext, gnScreenWidth - 110, gnScreenHeight - 9, 0xFFFFFFFF, 5, NULL);
}

//  Layout checks for the warning icons' body-location table, gaWarnIconBodyLocs in panel.h: seven dwords, nothing writes it. The loops' bound in the
//  bytes is the address of the next item -- the Panel\hostilepic string -- so the count is seven.

static_assert(BODYLOC_HEAD == 1 && BODYLOC_TORSO == 3 && BODYLOC_BELT == 8
                  && BODYLOC_FEET == 9 && BODYLOC_GLOVES == 10,
              "the five values the table at 0x6FB7CE18 holds beside RARM and "
              "LARM, in this order: 1, 3, 8, 9, 10");
static_assert(ITEMTYPE_ANY_SHIELD == 51 && STATEMASK_STAMBARBLUE == 24
                  && STATE_SHRINE_STAMINA == 136,
              "`push 33h` at 0x6FAFEF2C, `push 18h` at 0x6FB0004A and "
              "`push 88h` at 0x6FB000F2");
static_assert(STR_IDX_4163_panelexp == 0x1043
                  && STR_IDX_4164_panelstamina == 0x1044,
              "`mov ecx, 1043h` at 0x6FAFFD7E and `mov ecx, 1044h` at "
              "0x6FB0015F");

//1.10f: D2Client.0x6FAFEE70
//  The durability warning icons at (width - 60, 110) -- 140 with the automap open, and 43 lower again when someone in the party is hostile. Its one
//  caller is UIPANELS_DrawInGamePanels, which suppresses it while the inventory, the shop or the anvil is open.
//  It scans first and then draws all seven: the first loop looks for one equipped item that has durability, is not stackable, has a maximum, and is
//  at or under 20 percent. If none is found the function returns having drawn nothing. If one is, the second loop draws an icon for every slot in the
//  table -- the ones that are fine included, and empty slots too, which get the fine-state frame. The exception is an empty hand, skipped at
//  0x6FAFEF72.
//  The cel index is 11 * state + slot: three states -- 0 fine, 1 at or under 20 percent, 2 broken, which is a percentage of zero or less -- times
//  eleven slot offsets. The offsets: HEAD +1, BELT +2, FEET +3, TORSO +4, RARM +6, or +7 with a shield and +10 with a quiver; LARM +8, or +5 and +9;
//  and +0 for anything else -- the gloves the table holds, and the rings it does not. The bytes test the quiver first and the shield second
//  (0x6FAFEF9F, 0x6FAFEFBE).
void __cdecl PANEL_DrawStatusWarningIcons()
{
    D2InventoryStrc *pInventory;
    D2UnitStrc *pItem;
    int32_t nScan;
    int32_t nSlot;
    int32_t nMaxDurability;
    int32_t nDuraPercent;
    int32_t nDuraState;
    int32_t bIsShield;
    int32_t nQuiverType;
    int32_t nIconCel;
    int32_t nHostileYOffset;
    D2GfxDataStrc tCelContext;

    pInventory = CUNIT_GetPlayer()->pInventory;

    for (nScan = 0; nScan < 7; ++nScan)
    {
        pItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, gaWarnIconBodyLocs[nScan]);
        if (!pItem || !D2COMMON_ITEMS_HasDurability(pItem) || D2COMMON_ITEMS_CheckIfStackable(pItem))
        {
            continue;
        }
        nMaxDurability = D2COMMON_STATLIST_GetMaxDurabilityFromUnit(pItem);
        if (!nMaxDurability)
        {
            continue;
        }
        if (100 * D2COMMON_STATLIST_UnitGetStatValue(pItem, STAT_DURABILITY, 0) / nMaxDurability <= 20)
        {
            break;
        }
    }

    if (nScan >= 7)
    {
        return;
    }

    for (nSlot = 0; nSlot < 7; ++nSlot)
    {
        pItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, gaWarnIconBodyLocs[nSlot]);
        nDuraState = 0;
        bIsShield = 0;
        nQuiverType = 0;

        if (pItem)
        {
            if (D2COMMON_ITEMS_HasDurability(pItem) && !D2COMMON_ITEMS_CheckIfStackable(pItem))
            {
                nMaxDurability = D2COMMON_STATLIST_GetMaxDurabilityFromUnit(pItem);
                bIsShield = D2COMMON_ITEMS_CheckItemTypeId(pItem, ITEMTYPE_ANY_SHIELD);
                nQuiverType = D2COMMON_ITEMS_GetQuiverType(pItem);
                if (nMaxDurability)
                {
                    nDuraPercent = 100 * D2COMMON_STATLIST_UnitGetStatValue(pItem, STAT_DURABILITY, 0) / nMaxDurability;
                    if (nDuraPercent > 0)
                    {
                        nDuraState = (nDuraPercent <= 20);
                    }
                    else
                    {
                        nDuraState = 2;
                    }
                }
            }
        }
        else if (gaWarnIconBodyLocs[nSlot] == BODYLOC_RARM || gaWarnIconBodyLocs[nSlot] == BODYLOC_LARM)
        {
            continue;
        }

        nIconCel = 11 * nDuraState;
        switch (gaWarnIconBodyLocs[nSlot])
        {
        case BODYLOC_HEAD:
            nIconCel += 1;
            break;
        case BODYLOC_TORSO:
            nIconCel += 4;
            break;
        case BODYLOC_RARM:
            if (nQuiverType)
            {
                nIconCel += 10;
            }
            else if (bIsShield)
            {
                nIconCel += 7;
            }
            else
            {
                nIconCel += 6;
            }
            break;
        case BODYLOC_LARM:
            if (nQuiverType)
            {
                nIconCel += 9;
            }
            else if (bIsShield)
            {
                nIconCel += 5;
            }
            else
            {
                nIconCel += 8;
            }
            break;
        case BODYLOC_BELT:
            nIconCel += 2;
            break;
        case BODYLOC_FEET:
            nIconCel += 3;
            break;
        default:
            break;
        }

        nHostileYOffset = 0;
        memset(&tCelContext, 0, sizeof(tCelContext));
        tCelContext.nDirection = 0;
        tCelContext.pCellFile = gpImgDuroIcon;
        tCelContext.nFrame = nIconCel;

        if (UI_GetUiWindowState(UI_AUTOMAP))
        {
            if (ROSTER_IsAnyoneHostileToPlayer())
            {
                nHostileYOffset = 43;
            }
            D2GFX_TEXTURE_CelDraw(&tCelContext, gnScreenWidth - 60, nHostileYOffset + 140, 0xFFFFFFFF, 1, NULL);
        }
        else
        {
            D2GFX_TEXTURE_CelDraw(&tCelContext, gnScreenWidth - 60, 110, 0xFFFFFFFF, 1, NULL);
        }
    }
}

//1.10f: D2Client.0x6FAFFCA0
//  The experience bar and its hover tooltip. Its one caller is PANEL_Draw800BorderFrame.
//  The span is two thresholds: the upper bound is DATATBLS_GetLevelThreshold(class, the base level stat) and the lower
//  DATATBLS_GetLevelThreshold(class, the current level - 1). With no player the class is -1 and both lookups take it.
//  BUG: the two thresholds use different level stats. LATENT: they agree for an ordinary character, but anything that makes STAT_LEVEL differ from
//  its base skews the bar one way.
//  Every comparison here is unsigned. When the progress reaches 0x226B902 both it and the span are shifted right by 7, which keeps `progress * 119`
//  inside 32 bits. Then `nBarWidth = 119 * progress / span`.
//  BUG: a width above 0x77 -- which is 119, the full bar -- is set to zero rather than clamped to the maximum, so a progress past the span draws an
//  empty bar.
//  The bar is two D2GFX_DrawLine runs a pixel apart, at height - 38 and - 37, and is drawn only below the class's maximum level. The tooltip is tbl
//  4163 `Experience: %u / %u`, formatted from the current experience and the upper threshold -- not from the span the bar is scaled by.
void __cdecl PANEL_DrawExperienceBar()
{
    D2UnitStrc *pPlayer;
    int32_t nClassId;
    uint32_t nLevel;
    uint32_t nExperience;
    uint32_t nBaseLevel;
    uint32_t nUpperThreshold;
    uint32_t nLowerThreshold;
    uint32_t nLevelSpan;
    uint32_t nProgress;
    uint32_t nBarWidth;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nBarY;
    int32_t nXStart;
    int32_t nXEnd;
    Unicode wszText[100];

    pPlayer = CUNIT_GetPlayer();
    if (pPlayer)
    {
        nClassId = pPlayer->dwClassId;
    }
    else
    {
        nClassId = -1;
    }

    nLevel = D2COMMON_STATLIST_UnitGetStatValue(pPlayer, STAT_LEVEL, 0);
    nExperience = D2COMMON_STATLIST_UnitGetStatValue(pPlayer, STAT_EXPERIENCE, 0);
    nBaseLevel = D2COMMON_STATLIST_GetUnitBaseStat(pPlayer, STAT_LEVEL, 0);
    nUpperThreshold = D2COMMON_DATATBLS_GetLevelThreshold(nClassId, nBaseLevel);
    nLowerThreshold = D2COMMON_DATATBLS_GetLevelThreshold(nClassId, nLevel - 1);
    nLevelSpan = nUpperThreshold - nLowerThreshold;

    if (nExperience <= nLowerThreshold)
    {
        nProgress = 0;
    }
    else
    {
        nProgress = nExperience - nLowerThreshold;
    }

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();

    if (nMouseX >= gnScreenWidth / 2 - 146 && nMouseX <= gnScreenWidth / 2 - 23 && nMouseY >= gnScreenHeight - 43 && nMouseY <= gnScreenHeight - 34)
    {
        Unicode::sprintf(100, wszText, D2LANG_GetStringFromTblIndex(STR_IDX_4163_panelexp), nExperience, nUpperThreshold);
        D2WIN_10129_DrawFramedText(wszText, gnScreenWidth / 2 - 146, gnScreenHeight - 51, 0, 1);
    }

    if (!nLevelSpan)
    {
        return;
    }

    if (nProgress >= 0x226B902)
    {
        nProgress >>= 7;
        nLevelSpan >>= 7;
    }
    nBarWidth = 119 * nProgress / nLevelSpan;
    if (nBarWidth > 0x77)
    {
        nBarWidth = 0;
    }

    if (nLevel < (uint32_t)D2COMMON_DATATBLS_GetMaxLevel(nClassId) && nBarWidth)
    {
        nBarY = gnScreenHeight - 38;
        nXStart = gnScreenWidth / 2 - 144;
        nXEnd = nXStart + nBarWidth;
        D2GFX_DrawLine(nXStart, nBarY, nXEnd, nBarY, 0xFF, 0xFF);
        D2GFX_DrawLine(nXStart, nBarY + 1, nXEnd, nBarY + 1, 0xFF, 0xFF);
    }
}

//1.10f: D2Client.0x6FAFFFD0
//  The stamina bar under the panel, and its tooltip. Its one caller is PANEL_Draw800BorderFrame.
//  While gnStaminaBarColours[0] is still 0 the three indices are mixed inline -- the same three values, in the same order, that
//  PANEL_CacheStaminaColourRed, Amber and Blue write -- so the bar fills the cache itself the first time it draws if the palette pass has not run.
//  Amber is the normal colour. Blue when the smoothed value is more than five past the maximum, or when STATES_CheckStateMaskOnUnit(pPlayer, 24)
//  passes (the argument is a mask index, not a states.txt id); that arm also raises the maximum to the value, so an over-full bar renders full. Red
//  when the maximum is not positive, or when the width would be under 25 of 102 -- and in the first of those the width is zero, so the red is a
//  colour nothing is drawn in.
//  The tooltip reads the smoothed value a second time: PANEL_GetSmoothedStatValue(10) is called twice, once for the bar and once for the text. The
//  text's copy is divided by 256 -- a signed divide -- and so is the maximum. STATE_SHRINE_STAMINA, or a value past that maximum, clamps the text to
//  the maximum and turns it colour 3.
void __cdecl PANEL_DrawStaminaBar()
{
    D2UnitStrc *pPlayer;
    int32_t nValue;
    int32_t nMax;
    uint32_t nColor;
    int32_t nWidth;
    int32_t nStamina;
    int32_t nMaxStamina;
    int32_t nTextColor;
    int32_t nMouseX;
    int32_t nMouseY;
    Unicode wszText[100];

    pPlayer = CUNIT_GetPlayer();
    nValue = PANEL_GetSmoothedStatValue(STAT_STAMINA);
    nMax = D2COMMON_STATLIST_GetMaxStaminaFromUnit(pPlayer);

    if (!gnStaminaBarColours[0])
    {
        gnStaminaBarColours[0] = D2WIN_10034_MixRGB(0xFF, 0, 0);
        gnStaminaBarColours[1] = D2WIN_10034_MixRGB(0xF4, 0xC0, 0x4C);
        gnStaminaBarColours[2] = D2WIN_10034_MixRGB(0, 0, 0xFF);
    }

    nColor = gnStaminaBarColours[1];
    if (nMax + 5 < nValue || D2COMMON_STATES_CheckStateMaskOnUnit(pPlayer, STATEMASK_STAMBARBLUE))
    {
        nMax = nValue;
        nColor = gnStaminaBarColours[2];
    }

    if (nMax <= 0)
    {
        nWidth = 0;
        nColor = gnStaminaBarColours[0];
    }
    else
    {
        nWidth = 102 * nValue / nMax;
        if (nWidth < 25)
        {
            nColor = gnStaminaBarColours[0];
        }
    }

    GFX_DrawFilledRect(gnScreenWidth / 2 - 127, gnScreenHeight - 27, nWidth, 18, nColor, 2);

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    nStamina = PANEL_GetSmoothedStatValue(STAT_STAMINA) / 256;
    nTextColor = 0;
    nMaxStamina = D2COMMON_STATLIST_GetMaxStaminaFromUnit(pPlayer) / 256;

    if (nStamina > nMaxStamina || D2COMMON_STATES_CheckState(pPlayer, STATE_SHRINE_STAMINA))
    {
        nStamina = nMaxStamina;
        nTextColor = 3;
    }

    if (nMouseX >= gnScreenWidth / 2 - 127 && nMouseX <= gnScreenWidth / 2 - 25 && nMouseY >= gnScreenHeight - 27 && nMouseY <= gnScreenHeight - 9)
    {
        Unicode::sprintf(100, wszText, D2LANG_GetStringFromTblIndex(STR_IDX_4164_panelstamina), nStamina, nMaxStamina);
        D2WIN_10129_DrawFramedText(wszText, gnScreenWidth / 2 - 76, gnScreenHeight - 52, nTextColor, 1);
    }
}

//  Layout checks for the quantity-warning rows, gaQtyWarnLevels in panel.h. The code walks the table with the cursor on a row's last field
//  (`[ebx-10h]` is the kind and `[ebx]` the lowest threshold), so the rows begin at 0x6FB7CDB0, not at the 0x6FB7CDC0 the first instruction loads. A
//  quantity above the highest draws nothing; otherwise it is the base frame, +1 or +2 as it crosses each threshold downward.
//  gaWarnIconSlotUsed is six bytes: the clear is a dword store and a word store, and both slot loops are bounded at 6.

static_assert(ITEMQUAL_SUPERIOR == 3 && STR_IDX_3998_newline == 0xF9E,
              "`cmp eax, 3` at 0x6FB00ADF and `mov ecx, 0F9Eh` at 0x6FB00BBC");

//  The icon draw the original inlines in PANEL_DrawQuantityWarningIcons. The slot search takes the first free of six, marks it, and steps the icon 40
//  pixels left per slot from `gnScreenWidth - 55`. With all six taken nothing is marked and the icon is drawn on top of the first one -- the overflow
//  is drawn, not dropped.
static void UI_DrawWarnIcon(int32_t nCelFrame)
{
    D2GfxDataStrc tCelContext;
    int32_t nSlot;
    int32_t nXPos;
    int32_t nYPos;

    nYPos = 155;
    nXPos = gnScreenWidth - 55;
    for (nSlot = 0; nSlot < 6; ++nSlot)
    {
        if (!gaWarnIconSlotUsed[nSlot])
        {
            gaWarnIconSlotUsed[nSlot] = 1;
            nXPos -= 40 * nSlot;
            break;
        }
    }

    memset(&tCelContext, 0, sizeof(tCelContext));
    tCelContext.pCellFile = ghWarnIconCel;
    tCelContext.nDirection = 0;
    tCelContext.nFrame = nCelFrame;

    if (UI_GetUiWindowState(UI_AUTOMAP))
    {
        if (ROSTER_IsAnyoneHostileToPlayer())
        {
            nYPos = 198;
        }
        nYPos += 30;
    }

    D2GFX_TEXTURE_CelDraw(&tCelContext, nXPos, nYPos, 0xFFFFFFFF, 5, NULL);
}

//1.10f: D2Client.0x6FAFEA40
//  The ammunition and low-quantity warnings. Its sole caller is UIPANELS_DrawInGamePanels, once a frame, and it begins by clearing all six icon
//  slots.
//  The no-ammo icon runs the test both ways round: if either hand holds a weapon with an ITEMS_GetAmmoType and the other hand does not hold an item
//  of that type, cel frame 2 is drawn. So it fires for a bow with no quiver and equally for a crossbow holding arrows.
//  The count includes the spares in the backpack: for each weapon hand the item's `qntwarning` kind picks the matching rows of the level table, and
//  the quantity compared against them is the equipped item's STAT_QUANTITY plus every other item on inventory page 0 whose items.txt dwCode matches.
//  That summing is skipped for magic, set, rare, unique, crafted and tempered items (ITEMS_IsMagSetRarUniCrfOrTmp), which is why a magic quiver never
//  counts the plain ones.
//  The row scan does not stop at the first match: it walks all five rows for each hand, so an item whose kind appears twice would draw twice.
void __cdecl PANEL_DrawQuantityWarningIcons()
{
    D2InventoryStrc *pInventory;
    D2UnitStrc *pRightHand;
    D2UnitStrc *pLeftHand;
    D2UnitStrc *pWarnItem;
    D2UnitStrc *pNode;
    D2UnitStrc *pOther;
    D2ItemsTxt *pItemsTxt;
    uint32_t nQuantity;
    uint32_t nBodyLocIdx;
    uint32_t nLevel;
    uint8_t nQtyWarnKind;
    int32_t nClassId;
    const D2QtyWarnLevelStrc *pRow;

    //  The original clears it with a dword store and a word store: six bytes, the array exactly.
    memset(gaWarnIconSlotUsed, 0, sizeof(gaWarnIconSlotUsed));

    pInventory = CUNIT_GetPlayer()->pInventory;
    pRightHand = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_RARM);
    pLeftHand = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_LARM);

    if ((pRightHand && D2COMMON_ITEMS_GetAmmoType(pRightHand) && (!pLeftHand || !D2COMMON_ITEMS_CheckItemTypeId(pLeftHand, (D2C_ItemTypes)D2COMMON_ITEMS_GetAmmoType(pRightHand)))) || (pLeftHand && D2COMMON_ITEMS_GetAmmoType(pLeftHand) && (!pRightHand
                || !D2COMMON_ITEMS_CheckItemTypeId(pRightHand, (D2C_ItemTypes)D2COMMON_ITEMS_GetAmmoType(pLeftHand)))))
    {
        UI_DrawWarnIcon(2);
    }

    for (nBodyLocIdx = 0; nBodyLocIdx < gnQtyWarnBodyLocCount; ++nBodyLocIdx)
    {
        pWarnItem = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, gaQtyWarnBodyLocs[nBodyLocIdx]);
        if (!pWarnItem)
        {
            continue;
        }
        nQtyWarnKind = D2COMMON_ITEMS_GetQtyWarnCount(pWarnItem);
        if (!nQtyWarnKind)
        {
            continue;
        }

        for (nLevel = 0; nLevel < gnQtyWarnLevelCount; ++nLevel)
        {
            pRow = &gaQtyWarnLevels[nLevel];
            if (nQtyWarnKind != pRow->nKind)
            {
                continue;
            }

            pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(pWarnItem->dwClassId);
            nQuantity = D2COMMON_STATLIST_UnitGetStatValue(pWarnItem, STAT_QUANTITY, 0);

            if (!D2COMMON_ITEMS_IsMagSetRarUniCrfOrTmp(pWarnItem))
            {
                for (pNode = D2COMMON_INVENTORY_GetFirstItem(pInventory); pNode; pNode = D2COMMON_INVENTORY_GetNextItem(pNode))
                {
                    pOther = D2COMMON_INVENTORY_UnitIsItem(pNode);
                    if (pOther == pWarnItem)
                    {
                        continue;
                    }
                    if (D2COMMON_ITEMS_GetInvPage(pOther))
                    {
                        continue;
                    }
                    nClassId = pOther ? pOther->dwClassId : -1;
                    if (D2COMMON_DATATBLS_GetItemsTxtRecord(nClassId)->dwCode == pItemsTxt->dwCode)
                    {
                        nQuantity += D2COMMON_STATLIST_UnitGetStatValue(pOther, STAT_QUANTITY, 0);
                    }
                }
            }

            if (nQuantity > pRow->nThresholdLow)
            {
                if (nQuantity > pRow->nThresholdMid)
                {
                    if (nQuantity > pRow->nThresholdHigh)
                    {
                        continue;
                    }
                    UI_DrawWarnIcon(pRow->nBaseFrame);
                }
                else
                {
                    UI_DrawWarnIcon(pRow->nBaseFrame + 1);
                }
            }
            else
            {
                UI_DrawWarnIcon(pRow->nBaseFrame + 2);
            }
        }
    }
}

//1.10f: D2Client.0x6FB00990
//  The belt item's hover tooltip. Its sole caller is PANEL_Draw800BorderFrame.
//  Gated on gbBeltHoverActive and gpBeltHoverItem, refused while the cursor holds an item, and refused for a box past 3 unless the belt panel is
//  visible or hovered. The hint is drawn only when both gnBeltHintX and gnBeltHintY are non-negative -- which is what PANEL_BeltResetHoverState's -1s
//  are for, and why they are -1 rather than 0.
//  A missing items.txt record asserts with `ptItemStats` and panel.cpp line 1556 (`push 614h` at 0x6FB00A9E), then exits: the same contract as
//  PANEL_BeltUseItemInSlot's, the same two strings, a different line.
//  Three Unicode arrays -- 256 for the magic mods, 128 for the name, 384 for the text -- are zeroed and then constructed element by element; the
//  declaration is that construction loop. The fourth, 64 wide, is built inside the panel-mode arm for NPCMENU_TransactionCost's line, with the tbl
//  3998 `newline` separator.
//  A quality of ITEMQUAL_SUPERIOR skips the magic-mods text entirely, and the two halves are coloured before they are joined: colour 3 on the mods,
//  colour 0 on the name.
void __cdecl PANEL_DrawBeltItemHint()
{
    D2UnitStrc *pPlayer;
    int32_t nClassId;
    Unicode wszMagicMods[256];
    Unicode wszName[128];
    Unicode wszText[384];

    memset(wszMagicMods, 0, sizeof(wszMagicMods));
    memset(wszName, 0, sizeof(wszName));
    memset(wszText, 0, sizeof(wszText));

    pPlayer = CUNIT_GetPlayer();

    if (!gbBeltHoverActive || !gpBeltHoverItem || D2COMMON_INVENTORY_GetCursorItem(pPlayer->pInventory))
    {
        return;
    }
    if (gnBeltHoverBox > 3 && !gbBeltPanelVisible && !gbBeltPanelHovered)
    {
        return;
    }

    nClassId = gpBeltHoverItem ? gpBeltHoverItem->dwClassId : -1;
    if (!D2COMMON_DATATBLS_GetItemsTxtRecord(nClassId))
    {
        FOG_DisplayAssert(szPtItemStats, szPanelCpp, 1556);
        exit(-1);
    }

    INV_BuildItemNameText(gpBeltHoverItem, wszName, 128);
    if (D2COMMON_ITEMS_GetItemQuality(gpBeltHoverItem) != ITEMQUAL_SUPERIOR)
    {
        MODS_AppendMagicModsText(gpBeltHoverItem, wszMagicMods, 256, 1, NULL);
    }

    Unicode::strcpy(wszText, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszMagicMods, 3));
    Unicode::strcat(wszText, (const Unicode *)UI_AddColorCodeToWstring((wchar_t *)wszName, 0));

    if (gnBeltHintX < 0 || gnBeltHintY < 0)
    {
        return;
    }

    GFXUTIL_ReturnArg(wszText);

    if (INV_IsPanelModeNormal())
    {
        Unicode wszCost[64];
        int32_t nCost;

        memset(wszCost, 0, sizeof(wszCost));
        if (NPCMENU_TransactionCost(gpBeltHoverItem, 1, &nCost, wszCost, 64) && wszCost[0].ch)
        {
            Unicode::strcat(wszText, D2LANG_GetStringFromTblIndex(STR_IDX_3998_newline));
            Unicode::strcat(wszText, wszCost);
        }
    }

    D2WIN_10129_DrawFramedText(wszText, gnBeltHintX, gnBeltHintY, 0, 1);
}

//  The three panel images, with their addresses in the original.
static const char szPanel800BorderFrame[] = "Panel\\800BorderFrame";  // 0x6FB7CFB0
static const char szPanelCtrlPnl7[]       = "Panel\\CtrlPnl7";        // 0x6FB7CFA0
static const char szPanel800CtrlPnl7[]    = "Panel\\800CtrlPnl7";     // 0x6FB7CF8C
//  The belt assert's own string, at 0x6FB7CFC8.
static const char szAssertBeltFrame[]     = "nFrame >= 0 && NUM_PANELBELTS";

static_assert(ITEMTYPE_BELT == 19 && D2FONT_FONT16 == 1,
              "`cmp eax, 13h` at 0x6FB002E7 and `mov ecx, 1` into "
              "D2Win_10127_SetFont at 0x6FB002BE");
static_assert(HOTKEY_POTION1 == 23 && HOTKEY_POTION2 == 24
                  && HOTKEY_POTION3 == 25 && HOTKEY_POTION4 == 26,
              "gnBeltSlotHotkeys (0x6FB7CD90) holds 17h..1Ah, read out of the "
              "PE -- the four belt-column keys CMDTBL_ActionUseBelt1..4 carry");

//1.10f: D2Client.0x6FAFF480
//  The control panel and the 800x600 filler frame, and the frame from which every other draw in this unit is reached. Its one caller is
//  UIPANELS_DrawInGamePanels.
//  At 800x600 it fills the sides with Panel\800BorderFrame -- cels 0..4 on the right for shift screen 2 or 3, cels 5..9 on the left for 1 or 3
//  (SCROLL_GetUiShiftScreen is decoded as bits; each side asks the getter twice, as the bytes do) -- and the control panel is Panel\800CtrlPnl7, six
//  cels. At the lower resolution it is Panel\CtrlPnl7, five cels, and the two 800 images are released. Each image is loaded once into its global and
//  kept. `D2GFX_GetDisplayType() == 3` becomes a one-pixel Y offset on the two end cels of the control panel, and nothing else.
//  The run button: frames 2 and 3 while GAME_GetRunToggled, 0 and 1 while it is off; the odd frame needs both gbRunButtonDown and the cursor inside
//  the run button's rectangle, which is PANEL_IsPointOverRunButton's. The hint under it is drawn from the same rectangle, tested again here.
//  An array of 100 Unicodes is built on the stack and nothing reads it (CRT_VectorConstructorIterator at 0x6FAFF8CB), the same waste
//  PANEL_DrawOrbHoverText has in a different shape. It is reproduced by the declaration below and deliberately not by a call.
//  The last call is a tail jump (0x6FAFF9BF) into PANEL_DrawMinipanelToggleHint.
void __cdecl PANEL_Draw800BorderFrame()
{
    D2GfxDataStrc tCelContext;
    int32_t nDisplayOffset;
    int32_t nMouseX;
    int32_t nMouseY;
    //  Constructed by the original and never used; the declaration constructs all hundred, and a call by hand would construct them twice.
    Unicode wszUnused[100];

    nDisplayOffset = (D2GFX_GetDisplayType() == 3);

    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600)
    {
        if (SCROLL_GetUiShiftScreen() == 2 || SCROLL_GetUiShiftScreen() == 3)
        {
            memset(&tCelContext, 0, sizeof(tCelContext));
            if (!gPanel800BorderImage)
            {
                gPanel800BorderImage = UI_LoadImageFromUiDir(szPanel800BorderFrame);
            }
            tCelContext.pCellFile = gPanel800BorderImage;
            tCelContext.nDirection = 0;
            tCelContext.nFrame = 0;
            D2GFX_TEXTURE_CelDraw(&tCelContext, 0, 253, 0xFFFFFFFF, 5, NULL);
            tCelContext.nFrame = 1;
            D2GFX_TEXTURE_CelDraw(&tCelContext, 256, 63, 0xFFFFFFFF, 5, NULL);
            tCelContext.nFrame = 2;
            D2GFX_TEXTURE_CelDraw(&tCelContext, 0, 484, 0xFFFFFFFF, 5, NULL);
            tCelContext.nFrame = 3;
            D2GFX_TEXTURE_CelDraw(&tCelContext, 0, 553, 0xFFFFFFFF, 5, NULL);
            tCelContext.nFrame = 4;
            D2GFX_TEXTURE_CelDraw(&tCelContext, 256, 553, 0xFFFFFFFF, 5, NULL);
        }

        if (SCROLL_GetUiShiftScreen() == 1 || SCROLL_GetUiShiftScreen() == 3)
        {
            if (!gPanel800BorderImage)
            {
                gPanel800BorderImage = UI_LoadImageFromUiDir(szPanel800BorderFrame);
            }
            memset(&tCelContext, 0, sizeof(tCelContext));
            tCelContext.pCellFile = gPanel800BorderImage;
            tCelContext.nDirection = 0;
            tCelContext.nFrame = 5;
            D2GFX_TEXTURE_CelDraw(&tCelContext, 400, 63, 0xFFFFFFFF, 5, NULL);
            tCelContext.nFrame = 6;
            D2GFX_TEXTURE_CelDraw(&tCelContext, 544, 253, 0xFFFFFFFF, 5, NULL);
            tCelContext.nFrame = 7;
            D2GFX_TEXTURE_CelDraw(&tCelContext, 713, 484, 0xFFFFFFFF, 5, NULL);
            tCelContext.nFrame = 8;
            D2GFX_TEXTURE_CelDraw(&tCelContext, 544, 553, 0xFFFFFFFF, 5, NULL);
            tCelContext.nFrame = 9;
            D2GFX_TEXTURE_CelDraw(&tCelContext, 400, 553, 0xFFFFFFFF, 5, NULL);
        }
    }

    memset(&tCelContext, 0, sizeof(tCelContext));

    if (D2GFX_GetResolutionMode() == D2GAMERES_800x600)
    {
        if (gPanelCtrlPnl7Image)
        {
            ARCHIVE_CellFileFree(gPanelCtrlPnl7Image);
            gPanelCtrlPnl7Image = NULL;
        }
        if (!gPanel800CtrlPnl7Image)
        {
            gPanel800CtrlPnl7Image = UI_LoadImageFromUiDir(szPanel800CtrlPnl7);
        }
        tCelContext.pCellFile = gPanel800CtrlPnl7Image;
        tCelContext.nDirection = 0;
        tCelContext.nFrame = 0;
        D2GFX_TEXTURE_CelDraw(&tCelContext, 0, gnScreenHeight - nDisplayOffset, 0xFFFFFFFF, 5, NULL);
        tCelContext.nFrame = 1;
        D2GFX_TEXTURE_CelDraw(&tCelContext, 165, gnScreenHeight, 0xFFFFFFFF, 5, NULL);
        tCelContext.nFrame = 2;
        D2GFX_TEXTURE_CelDraw(&tCelContext, 293, gnScreenHeight, 0xFFFFFFFF, 5, NULL);
        tCelContext.nFrame = 3;
        D2GFX_TEXTURE_CelDraw(&tCelContext, 421, gnScreenHeight, 0xFFFFFFFF, 5, NULL);
        tCelContext.nFrame = 4;
        D2GFX_TEXTURE_CelDraw(&tCelContext, 549, gnScreenHeight, 0xFFFFFFFF, 5, NULL);
        tCelContext.nFrame = 5;
        D2GFX_TEXTURE_CelDraw(&tCelContext, gnScreenWidth - 117, gnScreenHeight - nDisplayOffset, 0xFFFFFFFF, 5, NULL);
    }
    else
    {
        if (!gPanelCtrlPnl7Image)
        {
            gPanelCtrlPnl7Image = UI_LoadImageFromUiDir(szPanelCtrlPnl7);
        }
        if (gPanel800CtrlPnl7Image)
        {
            ARCHIVE_CellFileFree(gPanel800CtrlPnl7Image);
            gPanel800CtrlPnl7Image = NULL;
        }
        if (gPanel800BorderImage)
        {
            ARCHIVE_CellFileFree(gPanel800BorderImage);
            gPanel800BorderImage = NULL;
        }
        tCelContext.pCellFile = gPanelCtrlPnl7Image;
        tCelContext.nDirection = 0;
        tCelContext.nFrame = 0;
        D2GFX_TEXTURE_CelDraw(&tCelContext, 0, gnScreenHeight - nDisplayOffset, 0xFFFFFFFF, 5, NULL);
        tCelContext.nFrame = 1;
        D2GFX_TEXTURE_CelDraw(&tCelContext, gnScreenWidth / 2 - 155, gnScreenHeight, 0xFFFFFFFF, 5, NULL);
        tCelContext.nFrame = 2;
        D2GFX_TEXTURE_CelDraw(&tCelContext, gnScreenWidth / 2 - 27, gnScreenHeight, 0xFFFFFFFF, 5, NULL);
        tCelContext.nFrame = 3;
        D2GFX_TEXTURE_CelDraw(&tCelContext, gnScreenWidth / 2 + 101, gnScreenHeight, 0xFFFFFFFF, 5, NULL);
        tCelContext.nFrame = 4;
        D2GFX_TEXTURE_CelDraw(&tCelContext, gnScreenWidth - 117, gnScreenHeight - nDisplayOffset, 0xFFFFFFFF, 5, NULL);
    }

    PANEL_DrawHealthGlobe();
    PANEL_DrawManaGlobe();
    PANEL_DrawExperienceBar();

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();

    memset(&tCelContext, 0, sizeof(tCelContext));
    tCelContext.nDirection = 0;
    tCelContext.pCellFile = gpImgRunButton;

    if (GAME_GetRunToggled())
    {
        if (gbRunButtonDown && PANEL_IsPointOverRunButton(nMouseX, nMouseY))
        {
            tCelContext.nFrame = 3;
        }
        else
        {
            tCelContext.nFrame = 2;
        }
    }
    else if (gbRunButtonDown && PANEL_IsPointOverRunButton(nMouseX, nMouseY))
    {
        tCelContext.nFrame = 1;
    }
    else
    {
        tCelContext.nFrame = 0;
    }

    D2GFX_TEXTURE_CelDraw(&tCelContext, gnScreenWidth / 2 - 145, gnScreenHeight - 10, 0xFFFFFFFF, 5, NULL);

    if (nMouseX >= gnScreenWidth / 2 - 145 && nMouseX <= gnScreenWidth / 2 - 128 && nMouseY >= gnScreenHeight - 28 && nMouseY <= gnScreenHeight - 8)
    {
        PANEL_DrawRunHint();
    }

    PANEL_DrawStaminaBar();
    PANEL_DrawMiniPanelToggleButton();
    PANEL_DrawBelt();
    PANEL_DrawSkillButtons();
    PANEL_DrawBeltItemHint();
    PANEL_ArmLevelChangeTimer();
    PANEL_DrawMinipanelToggleHint();
}

//1.10f: D2Client.0x6FB002B0
//  The belt on the control panel: the equipped BODYLOC_BELT item sets gnBeltsTxtIndex, belts.txt gives every box its rectangle, and the items in
//  those boxes are drawn with their hotkey letters. Its one caller is PANEL_Draw800BorderFrame. The original inlines UNITS_GetXPosition here and, six
//  times, PANEL_CacheHighlightColours: it re-caches all four gnPanelHighlightColours before every highlight it draws.
//  What decides the rows: with no belt equipped the index is 2, the four-box default. With one, the type comes from UNITS_GetBeltType and a negative
//  type asserts (line 2788) and exits. The pinned-open rows are cleared when neither UI_BELTROWS nor gbBeltPanelHovered holds, and again whenever the
//  ESC menu is up.
//  The row strip is three cels of gpImgPopBelt at 32-pixel steps, and which of them are drawn is a switch on the index: 0 and 5 draw two rows, 1 and
//  4 draw one, 2 draws none and leaves gnHintPhaseSuppressed clear, everything else draws three. Every arm but 2 sets that flag, which is the flag
//  PANEL_InvHintPhaseSuppressed reports and the mini-panel and the item panels read.
//  The box loop walks the belts.txt box count, but stops past box 3 unless the hint phase is suppressed, so a closed belt draws its first row only.
//  Each item is drawn into its rectangle at the item's own X path coordinate, with a 29 by 29 highlight behind it: red for an item being targeted,
//  not useable or locked, green for the hovered one, and none otherwise. The hotkey letter is drawn only for boxes 0..3 and only when the item is
//  useable, in font 16, and it is truncated to 28 pixels by zeroing characters from the end until it fits -- a loop, not a measure.
//  With an item on the cursor and a live hover box the same 29 by 29 box is drawn in one of three colours: green when the box is empty and the item
//  is beltable, olive when INVENTORY_GetUseableItemFromBeltSlot accepts the swap, red when it does not.
//  BUG: the assert `nFrame >= 0 && NUM_PANELBELTS` tests that a count is non-zero instead of bounding the frame, so the compiler folded the second
//  half away and only the lower bound (`cmp eax, edi / jl` at 0x6FB002F2) survives. The upper bound is therefore not in this function.
void __cdecl PANEL_DrawBelt()
{
    D2InventoryStrc *pInventory;
    D2UnitStrc *pBeltUnit;
    D2UnitStrc *pItem;
    D2UnitStrc *pCursorItem;
    D2UnitStrc *pSwapItem;
    D2BeltsTxt tRecord;
    D2BeltsTxt tHoverRecord;
    D2InvRectStrc tBeltRect;
    int32_t nBeltType;
    int32_t nSlotId;
    int32_t nItemBox;
    int32_t nRowYOffset;
    int32_t nRow;
    int32_t nTextLen;
    uint8_t *pTextEnd;
    char abRowVisible[3];
    D2GfxDataStrc tCelContext;
    Unicode wszKey[100];

    D2WIN_10127_SetFont(D2FONT_FONT16);

    pInventory = CUNIT_GetPlayer()->pInventory;
    pBeltUnit = D2COMMON_INVENTORY_GetItemFromBodyLoc(pInventory, BODYLOC_BELT);
    if (pBeltUnit)
    {
        if (D2COMMON_ITEMS_GetItemType(pBeltUnit) == ITEMTYPE_BELT)
        {
            nBeltType = D2COMMON_UNITS_GetBeltType(pBeltUnit);
            if (nBeltType < 0)
            {
                FOG_DisplayAssert(szAssertBeltFrame, szPanelCpp, 2788);
                exit(-1);
            }
            gnBeltsTxtIndex = nBeltType;
        }
    }
    else
    {
        gnBeltsTxtIndex = 2;
    }

    if (!UI_GetUiWindowState(UI_BELTROWS) && !gbBeltPanelHovered)
    {
        gbBeltPanelVisible = 0;
    }
    if (UI_GetUiWindowState(UI_ESCMENU))
    {
        gbBeltPanelVisible = 0;
    }
    gnHintPhaseSuppressed = 0;

    if (gbBeltPanelVisible)
    {
        abRowVisible[0] = 1;
        abRowVisible[1] = 1;
        abRowVisible[2] = 1;

        switch (gnBeltsTxtIndex)
        {
        case 0:
        case 5:
            abRowVisible[2] = 0;
            gnHintPhaseSuppressed = 1;
            break;
        case 1:
        case 4:
            abRowVisible[1] = 0;
            abRowVisible[2] = 0;
            gnHintPhaseSuppressed = 1;
            break;
        case 2:
            abRowVisible[0] = 0;
            abRowVisible[1] = 0;
            abRowVisible[2] = 0;
            gnHintPhaseSuppressed = 0;
            break;
        default:
            gnHintPhaseSuppressed = 1;
            break;
        }

        memset(&tCelContext, 0, sizeof(tCelContext));
        tCelContext.nDirection = 0;
        tCelContext.pCellFile = gpImgPopBelt;
        tCelContext.nFrame = 0;

        nRowYOffset = 0;
        for (nRow = 0; nRow < 3; ++nRow)
        {
            if (abRowVisible[nRow])
            {
                D2GFX_TEXTURE_CelDraw(&tCelContext, gnScreenWidth / 2 + 21, gnScreenHeight - nRowYOffset - 41, 0xFFFFFFFF, 5, NULL);
            }
            nRowYOffset += 32;
        }
    }

    D2COMMON_DATATBLS_GetBeltsTxtRecord(gnBeltsTxtIndex, gbHighResolution, &tRecord);

    for (nSlotId = 0; nSlotId < tRecord.nBoxes; ++nSlotId)
    {
        if (nSlotId > 3 && !gnHintPhaseSuppressed)
        {
            break;
        }

        pItem = D2COMMON_INVENTORY_GetItemFromBeltSlot(pInventory, nSlotId);
        if (!pItem)
        {
            continue;
        }

        nItemBox = UNITS_GetXPosition(pItem);

        if (D2COMMON_ITEMS_CheckItemFlag(pItem, IFLAG_TARGETING, 1484, szPanelCpp) || !D2COMMON_ITEMS_CheckIfUseable(pItem) || ITEM_IsItemLocked(pItem))
        {
            PANEL_CacheHighlightColours();
            D2COMMON_DATATBLS_GetInvRectFromBeltsTxt(gnBeltsTxtIndex, gbHighResolution, &tBeltRect, nItemBox);
            GFX_DrawFilledRect(tBeltRect.nLeft, tBeltRect.nTop, 29, 29, gnPanelHighlightColours[0], 0);
        }
        else if (pItem == gpBeltHoverItem && gbBeltHoverActive)
        {
            PANEL_CacheHighlightColours();
            D2COMMON_DATATBLS_GetInvRectFromBeltsTxt(gnBeltsTxtIndex, gbHighResolution, &tBeltRect, nItemBox);
            GFX_DrawFilledRect(tBeltRect.nLeft, tBeltRect.nTop, 29, 29, gnPanelHighlightColours[1], 0);
        }
        else
        {
            PANEL_CacheHighlightColours();
        }

        D2COMMON_DATATBLS_GetInvRectFromBeltsTxt(gnBeltsTxtIndex, gbHighResolution, &tBeltRect, nItemBox);
        GFX_DrawInventoryItem(pItem, tBeltRect.nLeft, tBeltRect.nTop);

        if ((uint32_t)nSlotId < 4 && D2COMMON_ITEMS_CheckIfUseable(pItem))
        {
            memset(wszKey, 0, sizeof(wszKey));

            if (CMDTBL_ControlsFindBoundKey(gnBeltSlotHotkeys[nSlotId], 1) != -1 || CMDTBL_ControlsFindBoundKey(gnBeltSlotHotkeys[nSlotId], 0) != -1)
            {
                Unicode::strcpy(wszKey, CMDTBL_ControlsGetBoundKeyName(gnBeltSlotHotkeys[nSlotId], 1));

                nTextLen = Unicode::strlen(wszKey);
                if (D2WIN_10121_GetTextWidth(wszKey) > 28)
                {
                    pTextEnd = (uint8_t *)wszKey + 2 * nTextLen;
                    do
                    {
                        *(uint16_t *)pTextEnd = 0;
                        pTextEnd -= 2;
                    }
                    while (D2WIN_10121_GetTextWidth(wszKey) > 28);
                }

                D2WIN_10117_DrawText(wszKey, tBeltRect.nLeft + 2, tBeltRect.nBottom - 2, 4, 0);
            }
        }
    }

    if (gnBeltsTxtIndex != 2 && !UI_GetUiWindowState(UI_BELTROWS) && !gbBeltPanelVisible)
    {
        return;
    }

    pCursorItem = D2COMMON_INVENTORY_GetCursorItem(pInventory);
    if (!pCursorItem || !gbBeltHoverActive)
    {
        return;
    }

    D2COMMON_DATATBLS_GetBeltsTxtRecord(gnBeltsTxtIndex, gbHighResolution, &tHoverRecord);
    if (gnBeltHoverBox < 0 || gnBeltHoverBox >= tHoverRecord.nBoxes)
    {
        return;
    }

    if (!D2COMMON_INVENTORY_GetItemFromBeltSlot(pInventory, gnBeltHoverBox) && D2COMMON_ITEMS_CheckIfBeltable(pCursorItem))
    {
        PANEL_CacheHighlightColours();
        D2COMMON_DATATBLS_GetInvRectFromBeltsTxt(gnBeltsTxtIndex, gbHighResolution, &tBeltRect, gnBeltHoverBox);
        GFX_DrawFilledRect(tBeltRect.nLeft, tBeltRect.nTop, 29, 29, gnPanelHighlightColours[1], 0);
        return;
    }

    if (D2COMMON_INVENTORY_GetUseableItemFromBeltSlot(pInventory, pCursorItem, gnBeltHoverBox, &pSwapItem))
    {
        PANEL_CacheHighlightColours();
        D2COMMON_DATATBLS_GetInvRectFromBeltsTxt(gnBeltsTxtIndex, gbHighResolution, &tBeltRect, gnBeltHoverBox);
        GFX_DrawFilledRect(tBeltRect.nLeft, tBeltRect.nTop, 29, 29, gnPanelHighlightColours[3], 0);
    }
    else
    {
        PANEL_CacheHighlightColours();
        D2COMMON_DATATBLS_GetInvRectFromBeltsTxt(gnBeltsTxtIndex, gbHighResolution, &tBeltRect, gnBeltHoverBox);
        GFX_DrawFilledRect(tBeltRect.nLeft, tBeltRect.nTop, 29, 29, gnPanelHighlightColours[0], 0);
    }
}

//  Layout checks for the belt's records. Every box this run hit-tests comes out of belts.txt: DATATBLS_GetBeltsTxtRecord(gnBeltsTxtIndex,
//  gbHighResolution, &rec) gives the count and DATATBLS_GetInvRectFromBeltsTxt(..., i) each rectangle, so a belt with more boxes needs no code change
//  here.

//1.10f: D2Client.0x6FAFE9F0
//  How many rows the equipped belt has: a switch on gnBeltsTxtIndex returning 3, 2, 1 or 4. It is belts.txt `numboxes` divided by four, hardcoded;
//  every arm checks out against the stock table:
//
//      idx 0 belt 12 -> 3     idx 1 sash 8        -> 2
//      idx 5 heavy 12 -> 3    idx 4 light belt 8  -> 2
//      idx 2 default 4 -> 1   default (girdle/uber belt 16) -> 4
//
//  So the function agrees with the data and does not read it: a mod that changes numboxes moves the belt boxes -- DATATBLS_GetInvRectFromBeltsTxt
//  reads the table -- and does not move this. Three mini-panel callers test its answer with `> 1` to decide whether the belt occupies more than one
//  row.
//
//  Layout checks for the record offsets this run reads. nBoxes is one byte: belts.txt `numboxes` is a byte field at offset 4, and every read here
//  casts it (uint8_t).
static_assert(offsetof(D2BeltsTxt, nBoxes) == 0x04
                  && offsetof(D2BeltsTxt, beltBoxes) == 0x08,
              "PANEL_BeltHitTestPanel's record sits at [esp+8]: `mov eax, [esp+0Ch]` "
              "at 0x6FB01B85 is nBoxes, `[esp+eax+10h]` at 0x6FB01B94 is "
              "beltBoxes[nBoxes - 4]");
static_assert(offsetof(D2ItemsTxt, nUseable) == 0x11D
                  && offsetof(D2ItemsTxt, nUnique) == 0x129,
              "PANEL_BeltUseItemInSlot: `cmp byte ptr [eax+129h], 1` at 0x6FB0193B "
              "and `cmp byte ptr [eax+11Dh], 1` at 0x6FB0194D");

int32_t __cdecl PANEL_BeltGetRowCount()
{
    switch (gnBeltsTxtIndex)
    {
    case 0:
    case 5:
        return 3;
    case 1:
    case 4:
        return 2;
    case 2:
        return 1;
    default:
        return 4;
    }
}

//1.10f: D2Client.0x6FB012E0
//  Turns a click on the belt into a slot and acts on it. The box comes from belts.txt: a linear scan over
//  DATATBLS_GetInvRectFromBeltsTxt(gnBeltsTxtIndex, gbHighResolution, &pInvRect, nBeltBox) bounded by pRecord.nBoxes, the belts.txt row's own box
//  count. The rectangles are inclusive on all four edges, as everywhere the belt is hit-tested. pPlayer is the unit that owns pInventory; the body
//  never reads it.
//  The `nBeltBox != -1` test cannot fail: the scan starts at 0 and only increments. Its body is the whole of the useful function; it is written out
//  because the bytes are there.
//  Both items are fetched first -- the cursor's and the box's -- and then the function returns when CURSOR_GetCursorState() is 6 (`cmp eax, 6 / je`
//  at 0x6FB0138B), before either branch below.
//  With the cursor holding a beltable item it places. ITEMS_CheckIfBeltable gates the item; a cursor item that is not beltable falls through to the
//  pick-up test below and fails it on `!pCursorItem`. ITEM_IsItemLocked blocks the send for anything the server has not acknowledged. Into an
//  occupied box it sends command 37 (swap) with both unit ids; into an empty one, command 35 (place) with the box index, and
//  ITEM_SetUninterruptableState marks the cursor item. The placement sound plays even when the item is locked and nothing was sent: both locked arms
//  jump straight to it (0x6FB013AF, 0x6FB013BB), past the marking.
//  With the cursor empty it picks up: an item in the box, no cursor item and not locked: MSG_SendServerItemMessage 36 with the box item's id, then
//  the item is marked uninterruptable.
void __fastcall PANEL_BeltOnClickAtPixel(D2UnitStrc *pPlayer, D2InventoryStrc *pInventory, int32_t nX, int32_t nY)
{
    int32_t nBeltBox;
    D2UnitStrc *pCursorItem;
    D2UnitStrc *pBeltItem;
    BOOL bItemLocked;
    D2InvRectStrc pInvRect;
    D2BeltsTxt pRecord;

    D2COMMON_DATATBLS_GetBeltsTxtRecord(gnBeltsTxtIndex, gbHighResolution, &pRecord);
    nBeltBox = 0;
    if (!pRecord.nBoxes)
    {
        return;
    }

    for (;;)
    {
        D2COMMON_DATATBLS_GetInvRectFromBeltsTxt(gnBeltsTxtIndex, gbHighResolution, &pInvRect, nBeltBox);
        if (nX >= pInvRect.nLeft && nX <= pInvRect.nRight && nY >= pInvRect.nTop && nY <= pInvRect.nBottom)
        {
            break;
        }
        if (++nBeltBox >= pRecord.nBoxes)
        {
            return;
        }
    }

    //  cannot be negative -- the loop starts at 0
    if (nBeltBox != -1)
    {
        pCursorItem = D2COMMON_INVENTORY_GetCursorItem(pInventory);
        pBeltItem = D2COMMON_INVENTORY_GetItemFromBeltSlot(pInventory, nBeltBox);

        if (CURSOR_GetCursorState() == 6)
        {
            return;
        }

        if (pCursorItem && D2COMMON_ITEMS_CheckIfBeltable(pCursorItem))
        {
            bItemLocked = ITEM_IsItemLocked(pCursorItem);
            if (pBeltItem)
            {
                if (!bItemLocked)
                {
                    MSG_SendCmdUnit(37, pCursorItem->dwUnitId, pBeltItem->dwUnitId);
                    goto MARK_UNINTERRUPTABLE;
                }
            }
            else if (!bItemLocked)
            {
                MSG_SendCmdUnit(35, pCursorItem->dwUnitId, (uint16_t)nBeltBox);
MARK_UNINTERRUPTABLE:
                ITEM_SetUninterruptableState(pCursorItem);
            }
            SOUNDINST_PlaySound(ITEM_GetDropSoundNoFrame(pCursorItem), NULL, 0, 0, 0);
            return;
        }

        if (pBeltItem && !pCursorItem && !ITEM_IsItemLocked(pBeltItem))
        {
            MSG_SendServerItemMessage(36, pBeltItem->dwUnitId);
            ITEM_SetUninterruptableState(pBeltItem);
        }
    }
}

//1.10f: D2Client.0x6FB01590
//  Hit-tests the cursor against the belt's boxes and updates the hover state the belt panel draws from. Both call sites are in PANEL_GameOnMouseMove.
//  It is the same belts.txt scan as PANEL_BeltOnClickAtPixel. On a miss it returns having changed nothing at all, so the previous hover survives a
//  mouse move that leaves the belt. The original inlines UNITS_GetXPosition here.
//  Three outcomes on a hit:
//
//    cursor carrying an item (and cursor state not 6 or 8)
//        box marked live, box id recorded, hovered item cleared: you cannot get a tooltip while dragging.
//    box empty
//        box id recorded, box marked not live, item cleared.
//    box holds an item
//        box marked live, and when the item differs from the last one the hint anchor is recomputed.
//
//  The anchor is taken from the box the item claims: DATATBLS_GetInvRectFromBeltsTxt is called again with the item's own X coordinate as the box
//  index, which is only sane because a stored item keeps its grid position in its path. So the anchor comes from the box the item says it is in, not
//  from the box the cursor found; for a well-formed belt they are the same box. The hint lands at `rect.nLeft + 14, rect.nTop`.
//  pPlayer is never read: the caller supplies the unit that owns pInventory, and the body does not use it. PANEL_BeltOnClickAtPixel takes the same
//  unit in the same slot.
void __fastcall PANEL_BeltUpdateHoveredItem(D2UnitStrc *pPlayer, D2InventoryStrc *pInventory, int32_t nX, int32_t nY)
{
    int32_t nBeltBox;
    D2UnitStrc *pCursorItem;
    D2UnitStrc *pBeltItem;
    int32_t nItemBox;
    D2InvRectStrc pInvRect;
    D2BeltsTxt pRecord;

    D2COMMON_DATATBLS_GetBeltsTxtRecord(gnBeltsTxtIndex, gbHighResolution, &pRecord);
    nBeltBox = 0;
    if (!pRecord.nBoxes)
    {
        return;
    }

    for (;;)
    {
        D2COMMON_DATATBLS_GetInvRectFromBeltsTxt(gnBeltsTxtIndex, gbHighResolution, &pInvRect, nBeltBox);
        if (nX >= pInvRect.nLeft && nX <= pInvRect.nRight && nY >= pInvRect.nTop && nY <= pInvRect.nBottom)
        {
            break;
        }
        if (++nBeltBox >= pRecord.nBoxes)
        {
            return;
        }
    }

    if (nBeltBox < 0)
    {
        return;
    }

    pCursorItem = D2COMMON_INVENTORY_GetCursorItem(pInventory);
    if (CURSOR_GetCursorState() == 6 || CURSOR_GetCursorState() == 8)
    {
        return;
    }

    if (pCursorItem)
    {
        gbBeltHoverActive = 1;
        gnBeltHoverBox = nBeltBox;
        goto CLEAR_HOVERED_ITEM;
    }

    pBeltItem = D2COMMON_INVENTORY_GetItemFromBeltSlot(pInventory, nBeltBox);
    gnBeltHoverBox = nBeltBox;
    if (!pBeltItem)
    {
        gbBeltHoverActive = 0;
CLEAR_HOVERED_ITEM:
        gpBeltHoverItem = NULL;
        gpBeltHoverItemLast = 0;
        return;
    }

    gbBeltHoverActive = 1;
    if (pBeltItem != gpBeltHoverItemLast)
    {
        nItemBox = UNITS_GetXPosition(pBeltItem);
        D2COMMON_DATATBLS_GetInvRectFromBeltsTxt(gnBeltsTxtIndex, gbHighResolution, &pInvRect, nItemBox);
        gpBeltHoverItem = pBeltItem;
        gpBeltHoverItemLast = pBeltItem;
        gnBeltHintX = pInvRect.nLeft + 14;
        gnBeltHintY = pInvRect.nTop;
    }
}

//1.10f: D2Client.0x6FB018A0
//  Uses whatever is in one belt slot. It is the body behind PANEL_BeltUseColumn0..3 and PANEL_GameOnRButtonDown. It refuses while the cursor holds an
//  item or the ESC menu is up. Then by the item's items.txt row:
//
//      unique or quest   TEXT_IniScrollOpen -- shown, not consumed
//      useable           client command 38, unless the item is locked
//      anything else     player sound 0x14, the refusal grunt
//
//  A missing items.txt record asserts with the source string `ptItemStats` and line 2633, then `exit(-1)`.
//  bShiftHeld is the give-to-mercenary modifier and is LOD-only: the function nulls it first when GAME_IsExpansionGame is false. With it set and a
//  hireling present (ROSTERPETS_GetAssociatedUnitGUID(pUnit, 7, 0) returning something other than -1), the merc's sound event 84 plays instead of the
//  item's own use sound. The flag is also passed through to the server in command 38's nUnitGUID slot, so this is the client half of shift-clicking a
//  potion to your mercenary.
//  DATATBLS_GetInventoryGridInfo is computed and never read; the call is kept.
void __fastcall PANEL_BeltUseItemInSlot(D2UnitStrc *pUnit, D2InventoryStrc *pInventory, int32_t nSlotId, int32_t bShiftHeld)
{
    int32_t nUnitGUID;
    int32_t nInventoryRecordId;
    D2UnitStrc *pBeltItem;
    D2ItemsTxt *pItemsTxt;
    int32_t nAssociatedGUID;
    D2InventoryGridInfoStrc pInventoryGridInfo;

    if (GAME_IsExpansionGame())
    {
        nUnitGUID = bShiftHeld;
    }
    else
    {
        nUnitGUID = 0;
    }

    if (D2COMMON_INVENTORY_GetCursorItem(pInventory) || UI_GetUiWindowState(UI_ESCMENU))
    {
        return;
    }

    nInventoryRecordId = D2COMMON_UNITS_GetInventoryRecordId(pUnit, INVPAGE_INVENTORY, GAME_IsExpansionGame());
    D2COMMON_DATATBLS_GetInventoryGridInfo(nInventoryRecordId, gbHighResolution, &pInventoryGridInfo);

    pBeltItem = D2COMMON_INVENTORY_GetItemFromBeltSlot(pInventory, nSlotId);
    if (!pBeltItem)
    {
        return;
    }

    pItemsTxt = D2COMMON_DATATBLS_GetItemsTxtRecord(pBeltItem->dwClassId);
    if (!pItemsTxt)
    {
        FOG_DisplayAssert(szPtItemStats, szPanelCpp, 2633);
        exit(-1);
    }

    if (pItemsTxt->nUnique == 1 || pItemsTxt->nQuest == 1)
    {
        TEXT_IniScrollOpen(pBeltItem, pItemsTxt);
        return;
    }

    if (pItemsTxt->nUseable != 1)
    {
        UNITSND_PlayPlayerSoundEvent(pUnit, 0x14);
        return;
    }

    if (ITEM_IsItemLocked(pBeltItem))
    {
        return;
    }

    MSG_SendCmdUnitParam(38, pBeltItem->dwUnitId, nUnitGUID, 0);
    ITEM_SetUninterruptableState(pBeltItem);

    nAssociatedGUID = nUnitGUID ? ROSTERPETS_GetAssociatedUnitGUID(pUnit, 7, 0) : -1;
    if (!nUnitGUID || nAssociatedGUID == -1)
    {
        SOUNDINST_PlaySound(ITEM_GetUseSound(pBeltItem), NULL, 0, 0, 0);
    }
    else
    {
        UNITSND_PlayUnitSoundEvent(nAssociatedGUID, 1, 84);
    }
    gbBeltHoverActive = 0;
}

//1.10f: D2Client.0x6FB01A10
//  One of the four belt-key actions, reached from CMDTBL_ActionUseBelt1. Guarded three ways before it does anything: not disconnecting, the local
//  player alive and present, and gbBeltColumnHasItem[0] == 1, so a key press on an empty belt column is dropped here rather than sent.
//  Then PANEL_BeltUseItemInSlot(pPlayer, pPlayer->pInventory, 0, GetKeyState(VK_SHIFT) & 0x8000). The column index is the only thing that differs
//  across the four; the shift state is read live at the moment of the press, not taken from a cached modifier flag.
void __cdecl PANEL_BeltUseColumn0()
{
    D2UnitStrc *pPlayer;

    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing() && gbBeltColumnHasItem[0] == 1)
    {
        pPlayer = CUNIT_GetPlayer();
        PANEL_BeltUseItemInSlot(pPlayer, pPlayer->pInventory, 0, GetKeyState(VK_SHIFT) & 0x8000);
    }
}

//1.10f: D2Client.0x6FB01A50
//  One of the four belt-key actions, reached from CMDTBL_ActionUseBelt2. gbBeltColumnHasItem[1]; the rest is PANEL_BeltUseColumn0's.
void __cdecl PANEL_BeltUseColumn1()
{
    D2UnitStrc *pPlayer;

    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing() && gbBeltColumnHasItem[1] == 1)
    {
        pPlayer = CUNIT_GetPlayer();
        PANEL_BeltUseItemInSlot(pPlayer, pPlayer->pInventory, 1, GetKeyState(VK_SHIFT) & 0x8000);
    }
}

//1.10f: D2Client.0x6FB01A90
//  One of the four belt-key actions, reached from CMDTBL_ActionUseBelt3. gbBeltColumnHasItem[2]; the rest is PANEL_BeltUseColumn0's.
void __cdecl PANEL_BeltUseColumn2()
{
    D2UnitStrc *pPlayer;

    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing() && gbBeltColumnHasItem[2] == 1)
    {
        pPlayer = CUNIT_GetPlayer();
        PANEL_BeltUseItemInSlot(pPlayer, pPlayer->pInventory, 2, GetKeyState(VK_SHIFT) & 0x8000);
    }
}

//1.10f: D2Client.0x6FB01AD0
//  One of the four belt-key actions, reached from CMDTBL_ActionUseBelt4. gbBeltColumnHasItem[3]; the rest is PANEL_BeltUseColumn0's.
void __cdecl PANEL_BeltUseColumn3()
{
    D2UnitStrc *pPlayer;

    if (!GAME_IsDisconnecting() && !CUNIT_IsPlayerDeadOrMissing() && gbBeltColumnHasItem[3] == 1)
    {
        pPlayer = CUNIT_GetPlayer();
        PANEL_BeltUseItemInSlot(pPlayer, pPlayer->pInventory, 3, GetKeyState(VK_SHIFT) & 0x8000);
    }
}

//1.10f: D2Client.0x6FB01B10
//  `if (nColumn < 4) gbBeltColumnHasItem[nColumn] = bHasItem;` The bounds check is unsigned, so a negative column folds into the large values the
//  check rejects rather than writing behind the array.
//  Three callers, all item messages (ITEM_ItemMsg0F_RemoveFromBelt, ITEM_ItemMsg15_UpdateStats, ITEM_ItemMsg0E_SetInvPage), and every one passes the
//  item's belt X coordinate in ecx. In a 4x4 belt that coordinate is the column, which is what ties this setter to the four PANEL_BeltUseColumn
//  readers. There is no return value: it is an ordinary __fastcall of two arguments.
void __fastcall PANEL_BeltSetColumnHasItem(uint32_t nColumn, uint8_t bHasItem)
{
    if (nColumn < 4)
    {
        gbBeltColumnHasItem[nColumn] = bHasItem;
    }
}

//1.10f: D2Client.0x6FB01B20
//  Clears the belt hover completely: gnBeltHoverBox, gnBeltHintX and gnBeltHintY to -1, gbBeltHoverActive to 0, and both item pointers to null. The
//  -1s matter -- 0 is a valid box and a valid coordinate, so the reset value has to be out of range rather than zero. void: the eax the original
//  leaves is the zero stored into the globals, and nothing reads it.
void __cdecl PANEL_BeltResetHoverState()
{
    gnBeltHoverBox = -1;
    gnBeltHintX = -1;
    gnBeltHintY = -1;
    gbBeltHoverActive = 0;
    gpBeltHoverItem = NULL;
    gpBeltHoverItemLast = 0;
}

//1.10f: D2Client.0x6FB01B50
//  Is the mouse over the belt? Every caller is an input handler. Two rectangles, one per belt state:
//
//    open (gbBeltPanelVisible): the union of rec.beltBoxes[nBoxes - 4] and rec.beltBoxes[3] out of belts.txt -- the top-left and bottom-right of the
//        visible row of boxes, so the test is data.
//    closed: a fixed rectangle at the bottom centre of the screen, 23..145 pixels right of gnScreenWidth / 2 and 39..10 pixels up from the bottom,
//        so the test is hard-coded geometry.
//
//  pMsg->nXpos and pMsg->nYpos are the two words at +12 and +14 of SMSGHANDLER_PARAMS.
int32_t __fastcall PANEL_BeltHitTestPanel(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    D2BeltsTxt pRecord;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    D2COMMON_DATATBLS_GetBeltsTxtRecord(gnBeltsTxtIndex, gbHighResolution, &pRecord);

    if (gbBeltPanelVisible)
    {
        if (nXpos >= pRecord.beltBoxes[pRecord.nBoxes - 4].nLeft && nXpos <= pRecord.beltBoxes[3].nRight && nYpos >= pRecord.beltBoxes[pRecord.nBoxes - 4].nTop && nYpos <= pRecord.beltBoxes[3].nBottom)
        {
            return 1;
        }
    }
    else if (nYpos > gnScreenHeight - 48 && nXpos >= gnScreenWidth / 2 + 23 && nXpos <= gnScreenWidth / 2 + 145 && nYpos >= gnScreenHeight - 39 && nYpos <= gnScreenHeight - 10)
    {
        return 1;
    }
    return 0;
}

//1.10f: D2Client.0x6FB01C10
//  Clears only the hovered-item pair, leaving the box id and the hint anchor standing: the narrower of the two resets, it drops the tooltip without
//  forgetting which box the cursor is over. void: the eax the original leaves is the zero stored into the globals, and nothing reads it.
void __cdecl PANEL_BeltClearHoverItem()
{
    gpBeltHoverItem = NULL;
    gpBeltHoverItemLast = 0;
}

//1.10f: D2Client.0x6FB01C20
//  `gbBeltPanelVisible = 1;` and return.
void __cdecl PANEL_BeltShowPanel()
{
    gbBeltPanelVisible = 1;
}


//  The four handlers for the play area itself (gGame). This is the panel that is always up: it takes the mouse everywhere the other panels are not,
//  and its job is almost entirely the control bar along the bottom of the screen. Five boxes live there, and every one of them is a literal in these
//  four functions rather than a table:
//
//      x 117..165                    the LEFT mini-skill button
//      x w-165..w-117                the RIGHT mini-skill button
//      x w/2-8 .. w/2+5              the mini-panel toggle, y h-39..h-13
//      x w/2-145 .. w/2-128          the run/walk toggle, y h-28..h-8
//      x w/2+23 .. w/2+145           the belt, y h-39..h-10
//
//  The bar's own top edge is `gnScreenHeight - 48`. None of these use gnBorderXDifference or gnBorderYDifference: the control bar is anchored to the
//  screen, not to the shifted UI.

//1.10f: D2Client.0x6FB00CD0
//  Pressing the left button in the play area. Above the control bar it declines: `nYpos <= gnScreenHeight - 48` and the belt hit test both failing
//  means the press is in the world, and the function writes `bReturn = 0` and stops. Walking and attacking are not done here -- they are the frame
//  loop's job, from the cursor state.
//  The belt arms gbGameLButtonDown and consumes with a sound. The two mini-skill buttons arm their own flag and consume without one -- the sound for
//  those comes on release. The mini-panel and run toggles arm and play the click. So three of five press paths are silent and two are not.
//  gbGameBarBtnDownUnread is set on every accepted press, including the two that fall through the box chain entirely: ARM_BAR is reached both by the
//  boxes and by the goto that skips them, so any press below `gnScreenHeight - 48` sets it. Nothing in the module reads it.
void __stdcall PANEL_GameOnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    int32_t nHalfWidth;

    if (GAME_IsDisconnecting() || CUNIT_IsPlayerDeadOrMissing())
    {
        pMsg->bReturn = 0;
        return;
    }

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;

    if (PANEL_BeltHitTestPanel(pMsg))
    {
        gbGameLButtonDown = 1;
        goto CONSUME;
    }

    if (nYpos <= gnScreenHeight - 48)
    {
        //  in the world: not ours
        pMsg->bReturn = 0;
        return;
    }

    if (nXpos >= 117 && nXpos <= 165 && nYpos <= gnScreenHeight)
    {
        gbGameLButtonDown = 1;
        gbMiniSkillLeftBtnDownUnread = 1;
    }
    else if (nXpos >= gnScreenWidth - 165 && nXpos <= gnScreenWidth - 117 && nYpos <= gnScreenHeight)
    {
        gbGameLButtonDown = 1;
        gbMiniSkillRightBtnDownUnread = 1;
    }
    else
    {
        nHalfWidth = gnScreenWidth / 2;
        if (nXpos >= nHalfWidth - 8 && nXpos <= nHalfWidth + 5 && nYpos >= gnScreenHeight - 39 && nYpos <= gnScreenHeight - 13)
        {
            gbMiniPanelTogglePressed = 1;
        }
        else if (nXpos >= nHalfWidth - 145 && nXpos <= nHalfWidth - 128 && nYpos >= gnScreenHeight - 28 && nYpos <= gnScreenHeight - 8)
        {
            gbRunButtonDown = 1;
        }
        else
        {
            //  no box, but still below the bar's top edge
            goto ARM_BAR;
        }
        gbGameLButtonDown = 1;
        SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
    }

ARM_BAR:
    gbGameBarBtnDownUnread = 1;

CONSUME:
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FB01430
//  Moving the cursor in the play area -- this is the function that makes the belt pop up.
//  The belt opens on hover and closes on leave. With the belt not showing, a cursor inside x (w/2+23)..(w/2+145), y (h-39)..(h-10) sets
//  `gbBeltPanelVisible` and `gbBeltPanelHovered` and hands the position to `PANEL_BeltUpdateHoveredItem`. With it showing, leaving the belt's own hit
//  test clears the hover -- but only clears `gbBeltPanelVisible` if UI_BELTROWS is off. With the rows pinned open the panel stays up and just stops
//  being hovered.
//  The escape menu suppresses the pop-up but not the consume: `UI_GetUiWindowStateRaw(UI_ESCMENU)` guards the two flag writes and the hover update,
//  and the `bReturn = 1` sits outside that guard. So with the menu up the belt does not appear and the mouse-move is still swallowed.
//  One early-out does nothing at all: `GAME_GetLeftClickIssuedThisFrame()` returning true skips the whole body without writing `bReturn` -- the only
//  path of the four that leaves the caller's value standing. The three failure conditions above it all write 0.
void __stdcall PANEL_GameOnMouseMove(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nX;
    int32_t nY;
    D2UnitStrc *pPlayer;

    if (!gbInGame || GAME_IsDisconnecting() || CUNIT_IsPlayerDeadOrMissing())
    {
        pMsg->bReturn = 0;
        return;
    }

    nX = pMsg->nXpos;
    nY = pMsg->nYpos;
    pPlayer = CUNIT_GetPlayer();

    //  no bReturn written on this path
    if (GAME_GetLeftClickIssuedThisFrame())
    {
        return;
    }

    if (gbBeltPanelVisible)
    {
        if (PANEL_BeltHitTestPanel(pMsg))
        {
            if (UI_GetUiWindowState(UI_BELTROWS))
            {
                pMsg->bReturn = 1;
                pMsg->lResult = 0;
            }
            PANEL_BeltUpdateHoveredItem(pPlayer, pPlayer->pInventory, nX, nY);
        }
        else
        {
            //  the rows pinned open keep the panel up
            if (!UI_GetUiWindowState(UI_BELTROWS))
            {
                gbBeltPanelVisible = 0;
            }
            gbBeltPanelHovered = 0;
            pMsg->bReturn = 0;
            gbBeltHoverActive = 0;
        }
        return;
    }

    if (nX < gnScreenWidth / 2 + 23 || nX > gnScreenWidth / 2 + 145 || nY < gnScreenHeight - 39 || nY > gnScreenHeight - 10)
    {
        pMsg->bReturn = 0;
        gbBeltHoverActive = 0;
        return;
    }

    //  the escape menu suppresses the pop-up but not the consume below it
    if (!UI_GetUiWindowStateRaw(UI_ESCMENU))
    {
        gbBeltPanelVisible = 1;
        gbBeltPanelHovered = 1;
        PANEL_BeltUpdateHoveredItem(pPlayer, pPlayer->pInventory, nX, nY);
    }
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
}

//1.10f: D2Client.0x6FB01730
//  Right-clicking the belt -- the only thing this handler does. Outside the belt's hit test it writes `bReturn = 0`, clears `gbBeltHoverActive` and
//  stops, so a right-click in the world is not this panel's business.
//  The belt boxes come from belts.txt, not from a literal: `DATATBLS_GetBeltsTxtRecord(gnBeltsTxtIndex, gbHighResolution, &record)` gives the box
//  count and `DATATBLS_GetInvRectFromBeltsTxt(..., &rect, n)` gives each rectangle, both inclusive on all four edges. So the belt is the one part of
//  the control bar whose geometry is data rather than code.
//  bReturn is set to 1 before the search and back to 0 on a miss: the handler claims the message as soon as the belt hit test passes, then un-claims
//  it if no box contains the cursor. So a right-click on the belt's border -- inside the panel, outside every slot -- falls through to the next
//  handler after this one has already run the cursor-state work below.
//  The cursor-state work runs first, always: states 6 and 8 are handled before any box is looked at -- `UI_CursorSendUseHeldItem()` for 6 and
//  `MSG_SendServerItemMessage(76, -1)` for 8. Both happen whenever the belt panel is hit, even when the search then misses.
//  `nBeltSlot >= 0` after a loop that starts at 0 and only increments cannot fail; it is written out, and its `else` reaches the same `bReturn = 0`
//  the empty-belt path does. `(pMsg->wParam >> 2) & 1` is the shift bit of the WM_RBUTTONDOWN wParam, MK_SHIFT being 0x0004.
void __stdcall PANEL_GameOnRButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    int32_t nBeltSlot;
    D2UnitStrc *pPlayer;
    D2InvRectStrc pInvRect;
    D2BeltsTxt pRecord;

    if (GAME_IsDisconnecting() || CUNIT_IsPlayerDeadOrMissing())
    {
        pMsg->bReturn = 0;
        return;
    }

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;
    pPlayer = CUNIT_GetPlayer();

    if (!PANEL_BeltHitTestPanel(pMsg))
    {
        pMsg->bReturn = 0;
        gbBeltHoverActive = 0;
        return;
    }

    nBeltSlot = 0;
    pMsg->bReturn = 1;
    pMsg->lResult = 0;

    if (CURSOR_GetCursorState() == 6)
    {
        UI_CursorSendUseHeldItem();
    }
    else if (CURSOR_GetCursorState() == 8)
    {
        MSG_SendServerItemMessage(76, -1);
    }

    D2COMMON_DATATBLS_GetBeltsTxtRecord(gnBeltsTxtIndex, gbHighResolution, &pRecord);
    if (!pRecord.nBoxes)
    {
        pMsg->bReturn = 0;
        return;
    }

    for (;;)
    {
        D2COMMON_DATATBLS_GetInvRectFromBeltsTxt(gnBeltsTxtIndex, gbHighResolution, &pInvRect, nBeltSlot);
        if (nXpos >= pInvRect.nLeft && nXpos <= pInvRect.nRight && nYpos >= pInvRect.nTop && nYpos <= pInvRect.nBottom)
        {
            break;
        }
        if (++nBeltSlot >= pRecord.nBoxes)
        {
            //  inside the panel, outside every slot
            pMsg->bReturn = 0;
            return;
        }
    }

    //  cannot be negative -- the loop starts at 0
    if (nBeltSlot >= 0)
    {
        PANEL_BeltUseItemInSlot(pPlayer, pPlayer->pInventory, nBeltSlot, (pMsg->wParam >> 2) & 1);
        STORM_SMsgBreakHandlerChain(pMsg);
    }
    else
    {
        pMsg->bReturn = 0;
    }
}

//1.10f: D2Client.0x6FB00E20
//  Releasing the left button in the play area. It is the largest of the four because it carries the same five boxes twice, tested in two different
//  orders: once under `gbBeltPanelVisible` and once under `nY > gnScreenHeight - 48`, and the two chains are not the same:
//
//      belt visible     mini-panel, then LEFT skill, then RIGHT skill, then run
//      belt not visible belt, then LEFT skill, then RIGHT skill, then run, then mini-panel
//
//  The mini-panel moves from first to last, and only the second chain does the cursor-state work and the parity flip. There is no shared copy in the
//  bytes; both are written out.
//  The mini-panel toggle is two different calls: already open -- `UI_SetUIToggle(UI_MINIPANEL, UI_TURNOFF, 0)`; not open --
//  `UI_SetUIToggle(UI_MINIPANEL, UI_TOGGLE, 0)`. UI_TOGGLE on a closed panel and UI_TURNOFF on an open one are what a plain UI_TOGGLE would have done
//  in both cases; the distinction is in the bytes and is reproduced.
//  The two skill buttons toggle differently in the two chains: under `gbBeltPanelVisible` both call `UI_SetUIToggle(UI_MINISKILL, UI_TOGGLE, 0)`; in
//  the other chain both read the state first and call UI_TURNOFF or UI_TURNON explicitly. Same effect, two spellings, one function.
//  With no button down it still closes the skill selector: the `else` on `gbGameLButtonDown` -- a release inside either mini-skill box with
//  UI_MINISKILL open turns it off, even though no press armed anything. That is how the selector closes when the press that opened it was consumed
//  elsewhere.
//  gbMiniPanelToggleParityUnread is flipped once per mini-panel release and read by nothing but its own flip; the store is reproduced.
void __stdcall PANEL_GameOnLButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    D2UnitStrc *pPlayer;
    int32_t nX;
    int32_t nY;
    int32_t nHalfWidth;
    int32_t bRunToggled;

    if (GAME_IsDisconnecting() || CUNIT_IsPlayerDeadOrMissing())
    {
        pMsg->bReturn = 0;
        return;
    }

    pPlayer = CUNIT_GetPlayer();
    nX = pMsg->nXpos;
    nY = pMsg->nYpos;
    gbMiniSkillRightBtnDownUnread = 0;
    gbMiniSkillLeftBtnDownUnread = 0;

    if (!gbGameLButtonDown)
    {
        if (((nX >= 117 && nX <= 165 && nY >= gnScreenHeight - 48 && nY <= gnScreenHeight) || (nX >= gnScreenWidth - 165 && nX <= gnScreenWidth - 117 && nY >= gnScreenHeight - 48 && nY <= gnScreenHeight)) && UI_GetUiWindowState(UI_MINISKILL))
        {
            UI_SetUIToggle(UI_MINISKILL, UI_TURNOFF, 0);
        }
        gbGameLButtonDown = 0;
        pMsg->bReturn = 0;
        goto CLEAR_PRESS_FLAGS;
    }

    nHalfWidth = gnScreenWidth / 2;

    if (gbBeltPanelVisible)
    {
        if (PANEL_BeltHitTestPanel(pMsg))
        {
            if (CURSOR_GetCursorState() == 6)
            {
                UI_CursorSendUseHeldItem();
            }
            else if (CURSOR_GetCursorState() == 8)
            {
                MSG_SendServerItemMessage(76, -1);
            }
            PANEL_BeltOnClickAtPixel(pPlayer, pPlayer->pInventory, nX, nY);
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
            STORM_SMsgBreakHandlerChain(pMsg);
            gbGameLButtonDown = 0;
            goto CLEAR_PRESS_FLAGS;
        }

        //  the mini-panel is first in this chain and last in the other
        if (nX >= nHalfWidth - 8 && nX <= nHalfWidth + 5 && nY >= gnScreenHeight - 39 && nY <= gnScreenHeight - 13)
        {
            if (UI_GetUiWindowState(UI_MINIPANEL))
            {
                UI_SetUIToggle(UI_MINIPANEL, UI_TURNOFF, 0);
            }
            else
            {
                UI_SetUIToggle(UI_MINIPANEL, UI_TOGGLE, 0);
            }
        }
        else if (nX >= 117 && nX <= 165 && nY >= gnScreenHeight - 48 && nY <= gnScreenHeight)
        {
            UI_SetUIToggle(UI_MINISKILL, UI_TOGGLE, 0);
            SPELLSEL_SetMiniSkillMode(1);
        }
        else if (nX >= gnScreenWidth - 165 && nX <= gnScreenWidth - 117 && nY >= gnScreenHeight - 48 && nY <= gnScreenHeight)
        {
            UI_SetUIToggle(UI_MINISKILL, UI_TOGGLE, 0);
            SPELLSEL_SetMiniSkillMode(0);
        }
        else if (nX >= nHalfWidth - 145 && nX <= nHalfWidth - 128 && nY >= gnScreenHeight - 28 && nY <= gnScreenHeight - 8)
        {
            bRunToggled = GAME_GetRunToggled();
            GAME_SetRunToggled(bRunToggled == 0);
        }
        gbGameLButtonDown = 0;
        goto CLEAR_PRESS_FLAGS;
    }

    if (nY <= gnScreenHeight - 48)
    {
        pMsg->bReturn = 0;
        goto CLEAR_PRESS_FLAGS;
    }

    //  the belt box comes first in this chain
    if (nX >= nHalfWidth + 23 && nX <= nHalfWidth + 145 && nY >= gnScreenHeight - 39 && nY <= gnScreenHeight - 10)
    {
        if (CURSOR_GetCursorState() == 6)
        {
            UI_CursorSendUseHeldItem();
        }
        else if (CURSOR_GetCursorState() == 8)
        {
            MSG_SendServerItemMessage(76, -1);
        }
        PANEL_BeltOnClickAtPixel(pPlayer, pPlayer->pInventory, nX, nY);
    }
    else if (nX >= 117 && nX <= 165 && nY <= gnScreenHeight)
    {
        if (CURSOR_GetCursorState() == 6)
        {
            UI_CursorSendUseHeldItem();
        }
        else if (CURSOR_GetCursorState() == 8)
        {
            MSG_SendServerItemMessage(76, -1);
        }
        //  the state is read here, where the other chain just toggles
        if (UI_GetUiWindowState(UI_MINISKILL))
        {
            UI_SetUIToggle(UI_MINISKILL, UI_TURNOFF, 0);
        }
        else
        {
            UI_SetUIToggle(UI_MINISKILL, UI_TURNON, 0);
        }
        SPELLSEL_SetMiniSkillMode(1);
    }
    else if (nX >= gnScreenWidth - 165 && nX <= gnScreenWidth - 117 && nY <= gnScreenHeight)
    {
        if (CURSOR_GetCursorState() == 6)
        {
            UI_CursorSendUseHeldItem();
        }
        else if (CURSOR_GetCursorState() == 8)
        {
            MSG_SendServerItemMessage(76, -1);
        }
        if (UI_GetUiWindowState(UI_MINISKILL))
        {
            UI_SetUIToggle(UI_MINISKILL, UI_TURNOFF, 0);
        }
        else
        {
            UI_SetUIToggle(UI_MINISKILL, UI_TURNON, 0);
        }
        SPELLSEL_SetMiniSkillMode(0);
    }
    else if (nX >= nHalfWidth - 145 && nX <= nHalfWidth - 128 && nY >= gnScreenHeight - 28 && nY <= gnScreenHeight - 8)
    {
        bRunToggled = GAME_GetRunToggled();
        GAME_SetRunToggled(bRunToggled == 0);
    }
    else if (nX >= nHalfWidth - 8 && nX <= nHalfWidth + 5 && nY >= gnScreenHeight - 39 && nY <= gnScreenHeight - 13)
    {
        UI_SetUIToggle(UI_MINIPANEL, UI_GetUiWindowState(UI_MINIPANEL) ? UI_TURNOFF : UI_TOGGLE, 0);
        if (gbMiniPanelTogglePressed)
        {
            gbMiniPanelToggleParityUnread = (gbMiniPanelToggleParityUnread == 0);
        }
    }

    gbGameLButtonDown = 0;
    gbGameBarBtnDownUnread = 0;
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);

CLEAR_PRESS_FLAGS:
    gbMiniPanelTogglePressed = 0;
    gbRunButtonDown = 0;
}

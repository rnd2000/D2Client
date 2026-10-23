#define _D2VARS_H

#include "DLLmain.h"
#include "D2Client.h"
#include "Game.h"
#include "CoreArchive.h"
#include "ui.h"
#include "skillstree.h"

//  skillstree.cpp -- the skill tree panel of D2Client.dll 1.10 (UI_SKILLTREE): the chart image, the skill icons, the tab labels, the close button,
//  and the two mouse handlers SKILLSTREE_OnLButtonDown and SKILLSTREE_OnLButtonUp (the records of gSkilltreeMessageHandlers).
//  The original source file is UI\skillstree.cpp (the assert in SKILLSTREE_LoadChartImage names it). Address range 0x6FB16300..0x6FB185B7.

//  The skill grid is three columns and six rows of 48-pixel cells. Every coordinate is a literal in the original, written out in each walk; ours
//  writes them once, in SKILLTREE_CellBottom and SKILLTREE_CellLeft:
//      column 1..3  gnScreenWidth - gnBorderXDifference - 305, - 236, - 167
//      row 1..6     gnBorderYDifference + gnScreenHeight - 418, - 350, - 282, - 214, - 145, - 77
//  The column pitch is 69 and the row pitch is 68 except between rows 4 and 5, where it is 69; those are the original's values. A cell is
//  x in (left, left + 48) and y in (bottom - 48, bottom), exclusive on all four edges. The row's value is the cell's BOTTOM edge: the box runs
//  48 pixels up from it and the icon is drawn up from it.
//  The close button moves with the tab: gnSkillTreeTab 1, 2, 3 put its left edge at gnScreenWidth - gnBorderXDifference - 149, - 220, - 305. It is
//  34 wide and 34 tall, its bottom edge is gnBorderYDifference + gnScreenHeight - 63, and its box is INCLUSIVE on all four edges, the opposite
//  convention from the cells. Any other tab value skips the button's hit test in both handlers.
//  A skill is on screen only if its nSkillPage matches gnSelectedSkillPage.

//  Layout checks for the values this file reads.
static_assert(UI_SKILLTREE == 4,
              "the skill tree's panel id, as UI_SetUIToggle is given it");
static_assert(STAT_SKILLPTS == 5,
              "STATLIST_GetUnitBaseStat's stat id -- row 5 of "
              "ItemStatCost.txt is `newskills`, the unspent points");
static_assert(STATE_UNINTERRUPTABLE == 54,
              "the state the tree checks before spending a point");
static_assert(offsetof(D2DataTablesStrc, pSkillsTxt) == 0xB98
                  && offsetof(D2DataTablesStrc, nSkillsTxtRecordCount) == 0xBA0,
              "the records and their count, eight bytes apart with the "
              "linker pointer between them");

static_assert(offsetof(D2ClientSkillTreeChartStrc, pImage) == 0x00
                  && offsetof(D2ClientSkillTreeChartStrc, szFileName) == 0x04,
              "the chart record: the handle at +0, the 30-byte file name at "
              "+4");

//  The fields the two walks read: the row's id and skill-point cost in skills.txt and the tree page in skilldesc.txt.
static_assert(offsetof(D2SkillsTxt, nSkillId) == 0x00
                  && offsetof(D2SkillsTxt, dwSkPoints) == 0x170
                  && offsetof(D2SkillDescTxt, nSkillPage) == 0x02,
              "the id at +0 the walks re-index by, the cost at +0x170 the "
              "affordability test reads, and the page at +2 of the desc row");

//  The fields the icon drawer reads: skilldesc.txt's row, column and cel, and the skills.txt flag byte at +5, tested against gdwBitMasks[2].
static_assert(offsetof(D2SkillDescTxt, nSkillRow) == 0x03
                  && offsetof(D2SkillDescTxt, nSkillColumn) == 0x04
                  && offsetof(D2SkillDescTxt, nIconCel) == 0x07,
              "the desc bytes the icon drawer's row and column switches and "
              "its cel read");
static_assert(offsetof(D2SkillsTxt, dwFlags) == 0x04,
              "the flag byte the icon drawer tests against gdwBitMasks[2]");
static_assert(D2FONT_FONTFORMAL10 == 4,
              "the two-digit level font the icon drawer switches to");

//  tbl string 4144 (Close), the one string the panel draws, loaded as the immediate 0x1030.
static_assert(STR_IDX_4144_strClose == 0x1030,
              "the close string SKILLSTREE_DrawPanel draws, `mov ecx, 1030h` "
              "at 0x6FB17055");

//  The label strings (stock string.tbl 4227..4251 and the expansion block 22509..22517, first and last of each) and the class values the label
//  chain dispatches on.
static_assert(STR_IDX_4227_StrSklTree1 == 0x1083
                  && STR_IDX_4240_StrSklTree14 == 0x1090,
              "the headings and the first stock block, `mov ecx, 1083h` at "
              "0x6FB17080 and `1090h` at 0x6FB17381");
static_assert(STR_IDX_4241_StrSklTree15 == 0x1091
                  && STR_IDX_4251_StrSklTree25 == 0x109B,
              "the second stock block, `mov ecx, 1091h` at 0x6FB173ED and "
              "`109Bh` at 0x6FB176B4");
static_assert(STR_IDX_22509_StrSklTree26 == 0x57ED
                  && STR_IDX_22517_StrSklTree34 == 0x57F5,
              "the expansion block the druid's and the assassin's tabs draw "
              "from, `mov ecx, 57EDh` at 0x6FB1779C and `57F5h` at "
              "0x6FB176FA");
static_assert(PCLASS_AMAZON == 0 && PCLASS_SORCERESS == 1
                  && PCLASS_NECROMANCER == 2 && PCLASS_PALADIN == 3
                  && PCLASS_BARBARIAN == 4 && PCLASS_DRUID == 5,
              "the classes the label chain picks its tab names by -- "
              "PCLASS_ASSASSIN's 6 is pinned by SkillDesc.cpp");

//  The three tab-label strings.
static_assert(STR_IDX_4224_StrSklTreea == 0x1080
                  && STR_IDX_4226_StrSklTreec == 0x1082,
              "the hover labels SKILLSTREE_DrawTabLabel draws, `mov ecx, "
              "1082h` at 0x6FB1799A and `1080h` at 0x6FB179FA");


//  The assert text and the source path of SKILLSTREE_LoadChartImage.
static const char szAssertSkillTreeChart[] = "sgptPlayerSkillTreeChart[gbPlayerType].pszFileName";  // 0x6FB8D054
static const char szSkillstreeCpp[] = "C:\\projects\\D2\\head\\Diablo2\\Source\\D2Client\\UI\\skillstree.cpp";  // 0x6FB8D088


//  The unit's static helpers, declared ahead of the draw functions that call them.
static D2SkillsTxt *SKILLTREE_LookupSkill(int32_t nClassId, int32_t nPosition, D2SkillDescTxt **ppSkillDesc, int32_t *pnSkillId);
static int32_t SKILLTREE_CellBottom(int32_t nSkillRow);
static int32_t SKILLTREE_CellLeft(int32_t nSkillColumn);


//1.10f: D2Client.0x6FB16300
//  Clears gbSkillPointSendPending (one dword store). SCMD_HandleSrv21_UpdateSkills tail-jumps here.
//  BUG: gbSkillPointSendPending is read by SKILLSTREE_OnLButtonUp before it sends client command 59 and by SKILLSTREE_DrawAvailableSkills, but no
//  code of the module ever sets it, so that guard cannot fire. UNREACHABLE: nothing arms the flag.
void __cdecl SKILLSTREE_ClearSkillPointPending()
{
    gbSkillPointSendPending = 0;
}


//1.10f: D2Client.0x6FB16310
//  Loads the current class's chart image: the file name of the class's record in sgptPlayerSkillTreeChart goes through UI_LoadImageFromUiDir, and
//  the answer is stored back into the same record's pImage and returned. A record is a handle plus a 30-byte file name, 34 bytes per class.
//  BUG: the compiler puts the ADDRESS of the record's name buffer in ecx, so `if (!szFileName)` is always false and its assert path is dead;
//  reproduced as the bytes have it. UNREACHABLE: the name is stored inline in the record, so its address is never NULL.
D2CellFileStrc *__cdecl SKILLSTREE_LoadChartImage()
{
    char *szFileName;
    D2CellFileStrc *hChartImage;

    szFileName = sgptPlayerSkillTreeChart[gnPlayerClass].szFileName;
    if (!szFileName)
    {
        FOG_DisplayAssert(szAssertSkillTreeChart, szSkillstreeCpp, 136);
        _exit(-1);
    }
    hChartImage = UI_LoadImageFromUiDir(szFileName);
    sgptPlayerSkillTreeChart[gnPlayerClass].pImage = hChartImage;
    return hChartImage;
}


//1.10f: D2Client.0x6FB16370
//  Releases the current class's chart image: the record's handle goes through ARCHIVE_CellFileFree and is nulled. void: eax at the return is
//  the class id left over from indexing the record a second time, and both callers discard it.
void __cdecl SKILLSTREE_UnloadChartImage()
{
    D2CellFileStrc *pImage;

    pImage = sgptPlayerSkillTreeChart[gnPlayerClass].pImage;
    if (pImage)
    {
        ARCHIVE_CellFileFree(pImage);
        sgptPlayerSkillTreeChart[gnPlayerClass].pImage = NULL;
    }
}


//1.10f: D2Client.0x6FB163B0
//  Draws the icons that can be allocated, and the unspent-point counter above them.
//  The counter is the player's STAT_SKILLPTS printed with "%i" into a 12-byte char buffer (eleven characters and the NUL), widened with toUnicode
//  and drawn at gnScreenWidth - gnBorderXDifference - 52, gnBorderYDifference + gnScreenHeight - 400. The colour is 1 when a player exists and
//  carries STATE_UNINTERRUPTABLE, or when gbSkillPointSendPending is set; that flag is never armed, so the first condition alone decides.
//  Then the walk over every position of the class: the shared lookup (SKILLTREE_LookupSkill), the page filter, CheckRequiredSkills first and, for
//  what passes, three availability tests: the level against skills.txt's wMaxLvl (20 when that field is not positive), CheckRequiredAttributes,
//  and the formula's answer against the unspent points. A cell that fails goes to state -1 / palette 5; one that passes goes to 0/0, except that
//  an already armed cell (state 1) is left alone.
//  nColumn and nPosition are two counters advanced in lockstep; the available arm re-loads one from the other, which changes nothing, and both
//  are kept as the original wrote them.
//  The icon goes through SKILLSTREE_DrawSkillIcon with the level the player owns (bBonus 1) or 0.
//  void: eax at the return is the player pointer (NULL without a player) or the last SKILLS_GetPlayerSkillCount answer; the one caller,
//  SKILLSTREE_DrawPanel, does not read it.
void __cdecl SKILLSTREE_DrawAvailableSkills()
{
    int32_t nSkillCount;
    D2UnitStrc *pUnit2;
    D2UnitStrc *pUnit;
    D2UnitStrc *pPlayer;
    int32_t nPosition;
    int32_t nClassId2;
    int32_t nSkillId2;
    int32_t nSkillId;
    int32_t wMaxLvl;
    D2SkillsTxt *pSkillsTxt;
    D2SkillDescTxt *pSkillDescTxt;
    int32_t SkillLevel;
    D2SkillStrc *pSkill;
    D2SkillStrc *pSkill2;
    uint32_t dwSkPoints;
    int32_t nAvailable;
    int32_t nColumnCopy;
    int32_t nSkillLevel;
    int32_t nSkillLevelHere;
    int32_t nColumn;
    int32_t nClassId;
    int32_t dwClassId;
    int32_t nUnitBaseStat;
    CHAR src[12];
    Unicode dest[10];

    pUnit2 = CUNIT_GetPlayer();
    pUnit = pUnit2;
    if (pUnit2)
    {
        dwClassId = pUnit2->dwClassId;
        nUnitBaseStat = D2COMMON_STATLIST_GetUnitBaseStat(pUnit2, STAT_SKILLPTS, 0);
        wsprintfA(src, "%i", nUnitBaseStat);
        Unicode::toUnicode(dest, src, 9);
        pPlayer = CUNIT_GetPlayer();
        if ((pPlayer != NULL && D2COMMON_STATES_CheckState(pPlayer, STATE_UNINTERRUPTABLE)) || gbSkillPointSendPending)
        {
            D2WIN_10117_DrawText(dest, gnScreenWidth - gnBorderXDifference - 52, gnBorderYDifference + gnScreenHeight - 400, 1, 0);
        }
        else
        {
            D2WIN_10117_DrawText(dest, gnScreenWidth - gnBorderXDifference - 52, gnBorderYDifference + gnScreenHeight - 400, 0, 0);
        }
        nPosition = 0;
        nClassId2 = dwClassId;
        nUnitBaseStat = D2COMMON_STATLIST_GetUnitBaseStat(pUnit2, STAT_SKILLPTS, 0);
        nColumn = 0;
        nSkillCount = D2COMMON_SKILLS_GetPlayerSkillCount(dwClassId);
        if (nSkillCount > 0)
        {
            do
            {
                pSkillDescTxt = NULL;
                pSkillsTxt = SKILLTREE_LookupSkill(nClassId2, nPosition, &pSkillDescTxt, &nSkillId2);
                if (pSkillsTxt && gnSelectedSkillPage == (int8_t)pSkillDescTxt->nSkillPage)
                {
                    if (D2COMMON_SKILLS_CheckRequiredSkills(pUnit2, (D2C_Skills)nSkillId2))
                    {
                        nClassId = 1;
                        SkillLevel = 0;
                        pSkill = D2COMMON_SKILLS_GetSkillById(pUnit2, (D2C_Skills)nSkillId2, 0xFFFFFFFF);
                        if (pSkill)
                            SkillLevel = D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill, 0);
                        nSkillId = pSkillsTxt->nSkillId;
                        if (nSkillId < 0 || nSkillId >= sgptDataTables->nSkillsTxtRecordCount || (wMaxLvl = (int16_t)sgptDataTables->pSkillsTxt[nSkillId].wMaxLvl, wMaxLvl <= 0))
                        {
                            wMaxLvl = 20;
                        }
                        if (SkillLevel >= wMaxLvl)
                            nClassId = 0;
                        if (!D2COMMON_SKILLS_CheckRequiredAttributes(pUnit, (D2C_Skills)nSkillId2))
                            nClassId = 0;
                        dwSkPoints = pSkillsTxt->dwSkPoints;
                        nAvailable = 1;
                        if (dwSkPoints != -1)
                            nAvailable = D2COMMON_SKILLS_EvaluateSkillFormula(pUnit, dwSkPoints, (D2C_Skills)nSkillId2, SkillLevel);
                        if (nUnitBaseStat >= nAvailable && nClassId)
                        {
                            nColumnCopy = nColumn;
                            if (gnSkillTreeCellState[nColumn] != 1)
                            {
                                gnSkillTreeCellState[nColumn] = 0;
                                gnSkillTreeCellPalette[nColumn] = 0;
                            }
                        }
                        else
                        {
                            nColumnCopy = nColumn;
                            gnSkillTreeCellState[nColumnCopy] = -1;
                            gnSkillTreeCellPalette[nColumnCopy] = 5;
                        }
                        nSkillLevel = 0;
                        if (pSkill)
                            nSkillLevel = D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill, 1);
                        SKILLSTREE_DrawSkillIcon((D2C_Skills)nSkillId2, pSkillsTxt, nSkillLevel, gnSkillTreeCellState[nColumnCopy], gnSkillTreeCellPalette[nColumnCopy]);
                        nPosition = nColumn;
                        pUnit2 = pUnit;
                    }
                    else
                    {
                        gnSkillTreeCellState[nPosition] = -1;
                        gnSkillTreeCellPalette[nPosition] = 5;
                        pSkill2 = D2COMMON_SKILLS_GetSkillById(pUnit2, (D2C_Skills)pSkillsTxt->nSkillId, 0xFFFFFFFF);
                        if (pSkill2 && (nSkillLevelHere = D2COMMON_SKILLS_GetSkillLevel(pUnit2, pSkill2, 1)) != 0)
                        {
                            SKILLSTREE_DrawSkillIcon((D2C_Skills)nSkillId2, pSkillsTxt, nSkillLevelHere, gnSkillTreeCellState[nPosition], gnSkillTreeCellPalette[nPosition]);
                        }
                        else
                        {
                            SKILLSTREE_DrawSkillIcon((D2C_Skills)nSkillId2, pSkillsTxt, 0, gnSkillTreeCellState[nPosition], gnSkillTreeCellPalette[nPosition]);
                        }
                    }
                }
                nClassId2 = dwClassId;
                nColumn = ++nPosition;
                nSkillCount = D2COMMON_SKILLS_GetPlayerSkillCount(dwClassId);
            }
            while (nPosition < nSkillCount);
        }
    }
}


//1.10f: D2Client.0x6FB16A50
//  The other tree pass: the icons that cannot be allocated yet. The same walk and page filter as SKILLSTREE_DrawAvailableSkills, with no
//  requirement test: every matching cell goes to state -1 / palette 5, except that a skill the player already owns (GetSkillById answers and its
//  level is above 0) goes back to 0/0 and is drawn with that level.
//  When gbHasUnspentSkillPoints is zero the pass first draws a centred "0" (formatted into a 12-byte char buffer) over the counter's box,
//  gnScreenWidth - gnBorderXDifference - 65 to - 25, gnBorderYDifference + gnScreenHeight - 400, where the twin draws the real count.
//  The loop condition calls GetPlayerSkillCount(pUnit->dwClassId) on every iteration and the lookup goes through pUnit->dwClassId again; that
//  is the original's own shape. The skills.txt record pointer is spilled to a stack slot and re-loaded for the DrawSkillIcon call; ours keeps
//  one local.
//  void: eax at the return is NULL without a player and the last GetPlayerSkillCount answer otherwise; SKILLSTREE_DrawPanel discards it.
void __cdecl SKILLSTREE_DrawUnavailableSkills()
{
    int32_t nSkillCount;
    D2UnitStrc *pUnit;
    int32_t i;
    D2C_Skills ClassSkillId;
    int32_t nSkillId;
    D2SkillsTxt *pSkillsTxt;
    D2SkillDescTxt *pSkillDescTxt;
    int32_t SkillLevel;
    D2SkillStrc *pSkill;
    CHAR src[12];
    Unicode dest[10];

    pUnit = CUNIT_GetPlayer();
    if (pUnit)
    {
        if (!gbHasUnspentSkillPoints)
        {
            wsprintfA(src, "%d", 0);
            Unicode::toUnicode(dest, src, 9);
            CHAR_DrawTextCentered(gnScreenWidth - gnBorderXDifference - 65, gnBorderYDifference + gnScreenHeight - 400, gnScreenWidth - gnBorderXDifference - 25, dest, 0);
        }
        for (i = 0; ; ++i)
        {
            nSkillCount = D2COMMON_SKILLS_GetPlayerSkillCount(pUnit->dwClassId);
            if (i >= nSkillCount)
                break;
            pSkillDescTxt = NULL;
            pSkillsTxt = SKILLTREE_LookupSkill(pUnit->dwClassId, i, &pSkillDescTxt, &nSkillId);
            if (!pSkillsTxt)
                continue;
            if (gnSelectedSkillPage != (int8_t)pSkillDescTxt->nSkillPage)
                continue;

            ClassSkillId = (D2C_Skills)nSkillId;
            gnSkillTreeCellState[i] = -1;
            gnSkillTreeCellPalette[i] = 5;
            SkillLevel = 0;
            pSkill = D2COMMON_SKILLS_GetSkillById(pUnit, ClassSkillId, 0xFFFFFFFF);
            if (pSkill)
            {
                SkillLevel = D2COMMON_SKILLS_GetSkillLevel(pUnit, pSkill, 1);
                if (SkillLevel > 0)
                {
                    gnSkillTreeCellState[i] = 0;
                    gnSkillTreeCellPalette[i] = 0;
                }
            }
            SKILLSTREE_DrawSkillIcon(ClassSkillId, pSkillsTxt, SkillLevel, gnSkillTreeCellState[i], gnSkillTreeCellPalette[i]);
        }
    }
}


//1.10f: D2Client.0x6FB166C0
//  Draws one icon: the cel, then the level number or the requirement tooltip. The original writes the grid's coordinates inline here; ours uses
//  SKILLTREE_CellBottom and SKILLTREE_CellLeft. It never looks at gnSelectedSkillPage -- it is told which skill to draw.
//  The cell is the two switches over skilldesc.txt's nSkillRow and nSkillColumn. The cel is nIconCel of the same row, advanced by one when
//  nCellState is 1 (the pressed frame; state 1 is what SKILLSTREE_OnLButtonDown stores), drawn with TEXTURE_CelDrawColor through a D2GfxDataStrc
//  cleared with memset: the image in pCellFile, direction 0, the cel in nFrame.
//  pSkillsTxt2 below is the same row as the pSkillsTxt argument, looked up a second time: the parameter is used only for its flag byte at +5, the
//  local for wSkillDesc. Redundant, but the original does it.
//  The hover test (MouseX in (left, left + 48) and MouseY in (bottom - 48, bottom)) runs twice: the first pass forces the palette shift to 3,
//  the second decides whether to draw the tooltip. In between, a clear bit in the flag byte (dwFlags[0]'s second byte against gdwBitMasks[2])
//  forces the shift down to 1.
//  The level comes from GetSkillLevel with bBonus 0; if that is 0 while the bonus level is not, and the cell is not the "cannot take" state -1,
//  the palette shift is forced to 3. GetSkillById and GetSkillLevel get whatever GetPlayer returns, with no null guard.
//  When the cursor is over the cell the tooltip is SKILLDESC_SkillsBuildSkillRequirementText for state -1 and
//  SKILLDESC_SkillsBuildSkillLevelText otherwise, with nCharges 0; GetPlayer is called once per arm, as the original does.
//  The level number under the icon is drawn only when either level is non-zero: wsprintf "%d" into a 12-byte char buffer, toUnicode into a
//  10-Unicode buffer that is constructed only inside the block, font 1 for one digit and font 4 with a four-pixel left shift for two, and the
//  colour from the bonus: >0 is 3, <0 is 1, 0 stays 0. Drawn at (left + 48, bottom + 12).
//  BUG: wszScratch, 1024 Unicodes, is constructed at entry and never read, per icon and per frame. INERT: it only costs the construction.
//  D2ExpRes patches this function once, SkillIcons, on the row read.
void __fastcall SKILLSTREE_DrawSkillIcon(D2C_Skills nSkillId, D2SkillsTxt *pSkillsTxt, int32_t nSkillLevel, int32_t nCellState, int32_t nGlobalPaletteShift)
{
    Unicode wszScratch[1024];
    D2SkillsTxt *pSkillsTxt2;
    int32_t wSkillDesc;
    D2SkillDescTxt *pSkillDescTxt;
    int32_t nCellBottom;
    int32_t nCellLeft;
    int32_t nMouseX;
    int32_t nMouseY;
    D2CellFileStrc *SkillIconImage;
    int32_t nIconCel;
    int32_t SkillLevel;
    D2UnitStrc *pPlayer;
    D2SkillStrc *pSkill;
    int32_t BonusSkillLevelFromSkillId;
    int32_t nBonusSkillLevelFromSkillId;
    int32_t nColor;
    CHAR src[12];
    D2GfxDataStrc pData;

    if ((int32_t)nSkillId >= (int32_t)SKILL_ATTACK && (int32_t)nSkillId < sgptDataTables->nSkillsTxtRecordCount)
    {
        pSkillsTxt2 = &sgptDataTables->pSkillsTxt[(int32_t)nSkillId];
        if (pSkillsTxt2)
        {
            wSkillDesc = (int16_t)pSkillsTxt2->wSkillDesc;
            if (wSkillDesc >= 0 && wSkillDesc < sgptDataTables->nSkillDescTxtRecordCount)
            {
                pSkillDescTxt = &sgptDataTables->pSkillDescTxt[wSkillDesc];
                if (pSkillDescTxt)
                {
                    //  D2ExpRes SkillIcons lands on the row read and redoes the position from its ini.
                    nCellBottom = SKILLTREE_CellBottom(pSkillDescTxt->nSkillRow);
                    nCellLeft = SKILLTREE_CellLeft(pSkillDescTxt->nSkillColumn);
                    nMouseX = CURSOR_GetMouseX();
                    nMouseY = CURSOR_GetMouseY();
                    SkillIconImage = SPELLSEL_GetSkillIconImage(NULL);
                    nIconCel = pSkillDescTxt->nIconCel;
                    if (nCellState == 1)
                        ++nIconCel;
                    if (nMouseX > nCellLeft && nMouseX < nCellLeft + 48 && nMouseY > nCellBottom - 48 && nMouseY < nCellBottom)
                    {
                        nGlobalPaletteShift = 3;
                    }
                    if ((HIBYTE(pSkillsTxt->dwFlags[0]) & (uint8_t)gdwBitMasks[2]) == 0)
                    {
                        nGlobalPaletteShift = 1;
                    }
                    pPlayer = CUNIT_GetPlayer();
                    pSkill = D2COMMON_SKILLS_GetSkillById(pPlayer, nSkillId, 0xFFFFFFFF);
                    SkillLevel = D2COMMON_SKILLS_GetSkillLevel(pPlayer, pSkill, 0);
                    BonusSkillLevelFromSkillId = D2COMMON_SKILLS_GetBonusSkillLevelFromSkillId(pPlayer, nSkillId);
                    if (!SkillLevel && BonusSkillLevelFromSkillId && nCellState != -1)
                    {
                        nGlobalPaletteShift = 3;
                    }
                    memset(&pData, 0, sizeof(pData));
                    pData.pCellFile = SkillIconImage;
                    pData.nDirection = 0;
                    pData.nFrame = nIconCel;
                    D2GFX_TEXTURE_CelDrawColor(&pData, nCellLeft, nCellBottom, 0xFFFFFFFF, 5, nGlobalPaletteShift);
                    if (nMouseX > nCellLeft && nMouseX < nCellLeft + 48 && nMouseY > nCellBottom - 48 && nMouseY < nCellBottom)
                    {
                        if (nCellState == -1)
                        {
                            SKILLDESC_SkillsBuildSkillRequirementText(CUNIT_GetPlayer(), nSkillId, nSkillLevel, 0, nCellLeft, nCellBottom);
                        }
                        else
                        {
                            SKILLDESC_SkillsBuildSkillLevelText(CUNIT_GetPlayer(), nSkillId, nSkillLevel, 0, nCellLeft, nCellBottom);
                        }
                    }
                    if (SkillLevel || nSkillLevel)
                    {
                        Unicode dest[10];

                        wsprintfA(src, "%d", nSkillLevel);
                        Unicode::toUnicode(dest, src, 9);
                        if (nSkillLevel <= 9)
                        {
                            D2WIN_10127_SetFont(D2FONT_FONT16);
                        }
                        else
                        {
                            D2WIN_10127_SetFont(D2FONT_FONTFORMAL10);
                            nCellLeft -= 4;
                        }
                        nBonusSkillLevelFromSkillId = D2COMMON_SKILLS_GetBonusSkillLevelFromSkillId(CUNIT_GetPlayer(), nSkillId);
                        if (nBonusSkillLevelFromSkillId <= 0)
                        {
                            nColor = nBonusSkillLevelFromSkillId < 0;
                        }
                        else
                        {
                            nColor = 3;
                        }
                        D2WIN_10117_DrawText(dest, nCellLeft + 48, nCellBottom + 12, nColor, 0);
                    }
                }
            }
        }
    }
}

//1.10f: D2Client.0x6FB16BE0
//  Caches the palette index that renders as black: D2WIN_10034_MixRGB(0, 0, 0) answers it, it is kept in gnBlackPaletteIndex and returned as well.
//  Nothing reads the cache: SKILLSTREE_DrawTabLabel stores its own MixRGB(0, 0, 0) into the same global on every call, before the global is read,
//  so what this function stores is overwritten before anything reads it.
uint8_t __cdecl SKILLSTREE_CacheBlackIndex()
{
    uint8_t result;

    result = D2WIN_10034_MixRGB(0, 0, 0);
    gnBlackPaletteIndex = result;
    return result;
}


//1.10f: D2Client.0x6FB16C00
//  Draws the whole skill tree panel, once a frame while UI_SKILLTREE is open (UIPANELS_DrawInGamePanels is the only caller). It is the unit's
//  top-level draw: the chart image, the two walks, the close button with tbl 4144 (Close), and SKILLSTREE_DrawTabLabel at the end. The cursor is
//  read here, so the close button's hover state is decided here.
//  The chart is eight TEXTURE_CelDraw calls through one D2GfxDataStrc: the image's cels 0..3 at the four corners of the panel (left edges
//  gnScreenWidth - gnBorderXDifference - 320 and - 64, bottoms gnBorderYDifference + gnScreenHeight - 224 and - 48), then the same four corners
//  again with the current page's cels, 4 * gnSelectedSkillPage + 0..3. gbSkillTreeDrawn is set to 1 before the first of the eight.
//  Then SKILLSTREE_DrawLabels, and the walk is picked by the player's unspent points: STAT_SKILLPTS above zero draws the available pass and sets
//  gbHasUnspentSkillPointsUnread and gbHasUnspentSkillPoints to 1, otherwise the unavailable pass and both to 0.
//  The close button's column comes from gnSkillTabXOffsets at page + 3 * class, and the switch over that offset sets gnSkillTreeTab to 1, 2 or 3;
//  any other value would leave the tab as it was. None comes: with the page in 1..3 the index runs 1..21, and entries 1..21 of the table (22
//  dwords) are all -149, -220 or -305. gnSkillTreeTab starts at 1 and these three stores are its only writers, so the tab is always 1..3.
//  The button is UI_GetBuySellButtonImage's image, cel 10 or 11 by gbSkillTreeButtonDown, drawn at gnScreenWidth - gnBorderXDifference +
//  nTabXOffset, gnBorderYDifference + gnScreenHeight - 63.
//  The close text: tab 1 tests its own 34 x 34 box inline, tabs 2 and 3 go through SKILLSTREE_IsMouseInRect -- the same box, inclusive on all
//  four edges. On a hit the string is tbl 4144 (Close), drawn at gnBorderYDifference + gnScreenHeight - 98 and 134 / 205 / 290 pixels left of
//  the right border by tab; either way the panel ends with SKILLSTREE_DrawTabLabel.
//  D2ExpRes patches this function three times: SkillsCloseButton, SkillsCloseButtonMouseHoverOver and SkillsCloseString; each is marked on the
//  line its detour lands on.
void __cdecl SKILLSTREE_DrawPanel()
{
    D2UnitStrc *pPlayer;
    int32_t nTabCelBase;
    int32_t nTabXOffset;
    int32_t nXPos;
    int32_t nTabBottom;
    int32_t nTabLeft;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nX;
    int32_t nY;
    const Unicode *pCloseString;
    D2GfxDataStrc pData;

    pPlayer = CUNIT_GetPlayer();
    D2WIN_10127_SetFont(D2FONT_FONT16);
    nTabCelBase = gnSelectedSkillPage;
    memset(&pData, 0, sizeof(pData));
    pData.pCellFile = sgptPlayerSkillTreeChart[gnPlayerClass].pImage;
    gbSkillTreeDrawn = 1;
    pData.nDirection = 0;
    pData.nFrame = 0;
    D2GFX_TEXTURE_CelDraw(&pData, gnScreenWidth - gnBorderXDifference - 320, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
    pData.nFrame = 1;
    D2GFX_TEXTURE_CelDraw(&pData, gnScreenWidth - gnBorderXDifference - 64, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
    pData.nFrame = 2;
    D2GFX_TEXTURE_CelDraw(&pData, gnScreenWidth - gnBorderXDifference - 320, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
    pData.nFrame = 3;
    D2GFX_TEXTURE_CelDraw(&pData, gnScreenWidth - gnBorderXDifference - 64, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
    nTabCelBase *= 4;
    pData.nFrame = nTabCelBase;
    D2GFX_TEXTURE_CelDraw(&pData, gnScreenWidth - gnBorderXDifference - 320, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
    pData.nFrame = nTabCelBase + 1;
    D2GFX_TEXTURE_CelDraw(&pData, gnScreenWidth - gnBorderXDifference - 64, gnBorderYDifference + gnScreenHeight - 224, 0xFFFFFFFF, 5, NULL);
    pData.nFrame = nTabCelBase + 2;
    D2GFX_TEXTURE_CelDraw(&pData, gnScreenWidth - gnBorderXDifference - 320, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
    pData.nFrame = nTabCelBase + 3;
    D2GFX_TEXTURE_CelDraw(&pData, gnScreenWidth - gnBorderXDifference - 64, gnBorderYDifference + gnScreenHeight - 48, 0xFFFFFFFF, 5, NULL);
    SKILLSTREE_DrawLabels();
    if (D2COMMON_STATLIST_GetUnitBaseStat(pPlayer, STAT_SKILLPTS, 0) <= 0)
    {
        gbHasUnspentSkillPointsUnread = 0;
        gbHasUnspentSkillPoints = 0;
        SKILLSTREE_DrawUnavailableSkills();
    }
    else
    {
        gbHasUnspentSkillPointsUnread = 1;
        gbHasUnspentSkillPoints = 1;
        SKILLSTREE_DrawAvailableSkills();
    }
    nTabXOffset = gnSkillTabXOffsets[2 * gnPlayerClass + gnSelectedSkillPage + gnPlayerClass];
    switch (nTabXOffset)
    {
    case -149:
        gnSkillTreeTab = 1;
        break;
    case -220:
        gnSkillTreeTab = 2;
        break;
    case -305:
        gnSkillTreeTab = 3;
        break;
    }
    nXPos = gnScreenWidth - gnBorderXDifference + nTabXOffset;
    memset(&pData, 0, sizeof(pData));
    pData.pCellFile = UI_GetBuySellButtonImage();
    pData.nFrame = (gbSkillTreeButtonDown != 0) + 10;
    pData.nDirection = 0;
    //  D2ExpRes SkillsCloseButton lands here.
    D2GFX_TEXTURE_CelDraw(&pData, nXPos, gnBorderYDifference + gnScreenHeight - 63, 0xFFFFFFFF, 5, NULL);
    if (gnSkillTreeTab == 1)
    {
        //  D2ExpRes SkillsCloseButtonMouseHoverOver lands on the box below.
        nTabBottom = gnBorderYDifference + gnScreenHeight - 63;
        nTabLeft = gnScreenWidth - gnBorderXDifference - 149;
        nMouseX = CURSOR_GetMouseX();
        nMouseY = CURSOR_GetMouseY();
        if (nMouseX >= nTabLeft && nMouseX <= nTabLeft + 34 && nMouseY >= nTabBottom - 34 && nMouseY <= nTabBottom)
        {
            //  D2ExpRes SkillsCloseString lands on the position below.
            nY = gnBorderYDifference + gnScreenHeight - 98;
            nX = gnScreenWidth - gnBorderXDifference - 134;
            goto DRAW_CLOSE_STRING;
        }
    }
    else if (gnSkillTreeTab == 2)
    {
        if (SKILLSTREE_IsMouseInRect(gnScreenWidth - gnBorderXDifference - 220, gnBorderYDifference + gnScreenHeight - 63, 34, 34))
        {
            nY = gnBorderYDifference + gnScreenHeight - 98;
            nX = gnScreenWidth - gnBorderXDifference - 205;
            goto DRAW_CLOSE_STRING;
        }
    }
    else if (gnSkillTreeTab == 3)
    {
        if (SKILLSTREE_IsMouseInRect(gnScreenWidth - gnBorderXDifference - 305, gnBorderYDifference + gnScreenHeight - 63, 34, 34))
        {
            nY = gnBorderYDifference + gnScreenHeight - 98;
            nX = gnScreenWidth - gnBorderXDifference - 290;
            goto DRAW_CLOSE_STRING;
        }
    }
    goto DRAW_TAB_LABEL;

DRAW_CLOSE_STRING:
    pCloseString = D2LANG_GetStringFromTblIndex(STR_IDX_4144_strClose);
    D2WIN_10129_DrawFramedText(pCloseString, nX, nY, 0, 1);

DRAW_TAB_LABEL:
    SKILLSTREE_DrawTabLabel(gnSelectedSkillPage);
}

//1.10f: D2Client.0x6FB17080
//  Draws the tree's labels: three headings, then one class's tab names. Every class draws the same three headings in the panel's right column --
//  tbl 4227 (Skill), 4228 (Choices) and 4229 (Remaining), bottom-anchored 455, 443 and 431 pixels up -- and then a switch over gnPlayerClass
//  supplies the tab names below them. The cases are written in the order the original's chain tests them (0, 3, 2, 4, 1, 6, 5), and a class
//  outside the list draws nothing else. The class is the only input: no tab, no skill points, no cursor.
//  The column is gnViewportRight - gnBorderXDifference - 90 to gnViewportRight - gnBorderXDifference, 90 pixels wide, measured off the viewport
//  and not the screen, and every line goes through CHAR_DrawTextCentered with colour 0 -- the lines SKILLSTREE_DrawLabelFromTop480 draws
//  included. The arms differ only in how they spell Y. The amazon gets nine lines with the Y inline, three per tab; the paladin and the
//  necromancer six; the barbarian two inline and three through the helper; the sorceress, the assassin and the druid only the helper's.
//  The inline Y counts UP from the bottom edge, the helper's DOWN from the top of the 480-pixel design frame: an inline line is at
//  gnBorderYDifference + gnScreenHeight - K, and the helper adds gnBorderYDifference + gnScreenHeight - 480 to its nY. The barbarian's two-line
//  Combat / Masteries shows it: the first line inline at - 216, the second through the helper at 276, which is - 204, twelve pixels below.
//  The stock strings are 4227..4251 (StrSklTree1..25); 22509..22517 are the expansion block (StrSklTree26..34, 2509..2517 of
//  expansionstring.tbl), and the druid's and the assassin's tabs are the only ones that live there.
//  D2ExpRes patches six of the centred draws, all on the - 90 of the left edge: SkillsBowAndCrossbowSkills1/2/3 on the amazon's three bow lines,
//  SkillsCombatSkills1/2 on the paladin's last two and SkillsCurses on the necromancer's last; each is marked on its line.
void __cdecl SKILLSTREE_DrawLabels()
{
    const Unicode *wszText;

    wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4227_StrSklTree1);
    CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 455, gnViewportRight - gnBorderXDifference, wszText, 0);
    wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4228_StrSklTree2);
    CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 443, gnViewportRight - gnBorderXDifference, wszText, 0);
    wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4229_StrSklTree3);
    CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 431, gnViewportRight - gnBorderXDifference, wszText, 0);

    switch (gnPlayerClass)
    {
    case PCLASS_AMAZON:
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4232_StrSklTree6);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 330, gnViewportRight - gnBorderXDifference, wszText, 0);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4233_StrSklTree7);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 318, gnViewportRight - gnBorderXDifference, wszText, 0);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4230_StrSklTree4);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 306, gnViewportRight - gnBorderXDifference, wszText, 0);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4234_StrSklTree8);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 222, gnViewportRight - gnBorderXDifference, wszText, 0);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4235_StrSklTree9);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 210, gnViewportRight - gnBorderXDifference, wszText, 0);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4230_StrSklTree4);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 198, gnViewportRight - gnBorderXDifference, wszText, 0);
        //  D2ExpRes SkillsBowAndCrossbowSkills1 lands here.
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4236_StrSklTree10);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 121, gnViewportRight - gnBorderXDifference, wszText, 0);
        //  D2ExpRes SkillsBowAndCrossbowSkills2 lands here.
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4237_StrSklTree11);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 109, gnViewportRight - gnBorderXDifference, wszText, 0);
        //  D2ExpRes SkillsBowAndCrossbowSkills3 lands here.
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4230_StrSklTree4);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 97, gnViewportRight - gnBorderXDifference, wszText, 0);
        break;
    case PCLASS_PALADIN:
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4238_StrSklTree12);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 324, gnViewportRight - gnBorderXDifference, wszText, 0);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4239_StrSklTree13);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 312, gnViewportRight - gnBorderXDifference, wszText, 0);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4240_StrSklTree14);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 216, gnViewportRight - gnBorderXDifference, wszText, 0);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4239_StrSklTree13);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 204, gnViewportRight - gnBorderXDifference, wszText, 0);
        //  D2ExpRes SkillsCombatSkills1 lands here.
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4241_StrSklTree15);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 110, gnViewportRight - gnBorderXDifference, wszText, 0);
        //  D2ExpRes SkillsCombatSkills2 lands here.
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4230_StrSklTree4);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 98, gnViewportRight - gnBorderXDifference, wszText, 0);
        break;
    case PCLASS_NECROMANCER:
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4242_StrSklTree16);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 324, gnViewportRight - gnBorderXDifference, wszText, 0);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4231_StrSklTree5);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 312, gnViewportRight - gnBorderXDifference, wszText, 0);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4243_StrSklTree17);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 222, gnViewportRight - gnBorderXDifference, wszText, 0);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4244_StrSklTree18);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 210, gnViewportRight - gnBorderXDifference, wszText, 0);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4231_StrSklTree5);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 198, gnViewportRight - gnBorderXDifference, wszText, 0);
        //  D2ExpRes SkillsCurses lands here.
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4245_StrSklTree19);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 104, gnViewportRight - gnBorderXDifference, wszText, 0);
        break;
    case PCLASS_BARBARIAN:
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4246_StrSklTree20);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 318, gnViewportRight - gnBorderXDifference, wszText, 0);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4247_StrSklTree21);
        CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnBorderYDifference + gnScreenHeight - 216, gnViewportRight - gnBorderXDifference, wszText, 0);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4248_StrSklTree22);
        SKILLSTREE_DrawLabelFromTop480(wszText, 276);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4247_StrSklTree21);
        SKILLSTREE_DrawLabelFromTop480(wszText, 370);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4230_StrSklTree4);
        SKILLSTREE_DrawLabelFromTop480(wszText, 382);
        break;
    case PCLASS_SORCERESS:
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4249_StrSklTree23);
        SKILLSTREE_DrawLabelFromTop480(wszText, 156);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4231_StrSklTree5);
        SKILLSTREE_DrawLabelFromTop480(wszText, 168);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4250_StrSklTree24);
        SKILLSTREE_DrawLabelFromTop480(wszText, 264);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4231_StrSklTree5);
        SKILLSTREE_DrawLabelFromTop480(wszText, 276);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4251_StrSklTree25);
        SKILLSTREE_DrawLabelFromTop480(wszText, 370);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4231_StrSklTree5);
        SKILLSTREE_DrawLabelFromTop480(wszText, 382);
        break;
    case PCLASS_ASSASSIN:
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_22516_StrSklTree33);
        SKILLSTREE_DrawLabelFromTop480(wszText, 150);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_22517_StrSklTree34);
        SKILLSTREE_DrawLabelFromTop480(wszText, 162);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_22514_StrSklTree31);
        SKILLSTREE_DrawLabelFromTop480(wszText, 264);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_22515_StrSklTree32);
        SKILLSTREE_DrawLabelFromTop480(wszText, 276);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_22513_StrSklTree30);
        SKILLSTREE_DrawLabelFromTop480(wszText, 370);
        break;
    case PCLASS_DRUID:
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_22512_StrSklTree29);
        SKILLSTREE_DrawLabelFromTop480(wszText, 162);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_22510_StrSklTree27);
        SKILLSTREE_DrawLabelFromTop480(wszText, 264);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_22511_StrSklTree28);
        SKILLSTREE_DrawLabelFromTop480(wszText, 276);
        wszText = D2LANG_GetStringFromTblIndex(STR_IDX_22509_StrSklTree26);
        SKILLSTREE_DrawLabelFromTop480(wszText, 370);
        break;
    }
}


//1.10f: D2Client.0x6FB177C0
//  Draws one label line, centred in the panel's column (not right-aligned): the box is the same 90-pixel column SKILLSTREE_DrawLabels draws in,
//  gnViewportRight - gnBorderXDifference - 90 to gnViewportRight - gnBorderXDifference, and the Y comes out as
//  gnScreenHeight + gnBorderYDifference + nY - 480, so a caller counts in pixels down from the top of the 640x480 design frame. Colour 0 is
//  hard-coded, so no caller can tint a line. The call is CHAR_DrawTextCentered with the column's two edges, which centres the text in
//  [nLeft, nRight].
void __fastcall SKILLSTREE_DrawLabelFromTop480(const Unicode *pwszText, int32_t nY)
{
    CHAR_DrawTextCentered(gnViewportRight - gnBorderXDifference - 90, gnScreenHeight + gnBorderYDifference + nY - 480, gnViewportRight - gnBorderXDifference, pwszText, 0);
}

//1.10f: D2Client.0x6FB17800
//  Draws the hovered tab's name. SKILLSTREE_DrawPanel calls it every frame with gnSelectedSkillPage as nMode. gnBlackPaletteIndex is recomputed
//  here (MixRGB(0, 0, 0) again) rather than taken from the cache SKILLSTREE_CacheBlackIndex fills.
//  The hit column is MouseX in [width - borderX - 89, width - borderX + 1] -- 91 pixels at the panel's right edge, one pixel to the right of the
//  column SKILLSTREE_DrawLabels draws in. The cursor is read at entry. Outside the column nothing is drawn, but the font is still set.
//  Three rows 108 pixels apart at borderY + height - 372 / - 264 / - 156, with windows [-372, -264), [-264, -156] and [-156, -48], and the page
//  picks which two are tested: page 1 the top and middle rows, page 2 the top and bottom, page 3 the middle and bottom; any other page tests
//  none. The windows share their edges and each page tests in its own order, so -264 answers the middle row on page 1 and the top row on page 2,
//  and -156 the bottom row on page 2 and the middle on page 3.
//  On a hit the row's label (tbl 4224, 4225, 4226, "Skill Tab 1/2/3" on the stock tables, top, middle, bottom in that order) is drawn over a
//  filled box at (width - borderX - 89, nLabelY + 5), 90 x 20, colour gnBlackPaletteIndex, eDrawMode 6. The font is 1 for the whole function.
//  BUG: the text call's right edge is the literal 90, not the column's right edge, so the name is left-aligned in its box. REACHABLE: cosmetic.
//  D2ExpRes patches six of the comparisons: SkillTabMouseHoverOver1 on the column test's left edge and 2..6 on the five row constants; each is
//  marked on its line.
void __fastcall SKILLSTREE_DrawTabLabel(int32_t nMode)
{
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nLabelY;
    int32_t bAnyTabHit;
    int32_t bTabBotHit;
    int32_t bTabMidHit;
    int32_t bTabTopHit;
    int32_t nTabMidY;
    int32_t nTabTopY;
    int32_t nTabBotY;
    uint32_t dwColor;
    const Unicode *wszText;

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    //  BUG: a Unicode[50] is constructed here and never read. INERT: it only costs the construction.
    Unicode wszScratch[50];
    nLabelY = 0;
    bAnyTabHit = 0;
    bTabBotHit = 0;
    bTabMidHit = 0;
    bTabTopHit = 0;
    gnBlackPaletteIndex = D2WIN_10034_MixRGB(0, 0, 0);
    //  D2ExpRes SkillTabMouseHoverOver1 lands here.
    if (nMouseX < gnScreenWidth - gnBorderXDifference - 89 || nMouseX > gnScreenWidth - gnBorderXDifference + 1)
    {
        goto DRAW_HIT_LABEL;
    }
    if (nMode == 1)
    {
        //  D2ExpRes SkillTabMouseHoverOver5 lands here.
        if (nMouseY > gnBorderYDifference + gnScreenHeight - 156 || nMouseY < gnBorderYDifference + gnScreenHeight - 264)
        {
            if (nMouseY <= gnBorderYDifference + gnScreenHeight - 264)
            {
                nTabTopY = gnBorderYDifference + gnScreenHeight - 372;
                if (nMouseY >= nTabTopY)
                {
                    goto TAB_TOP_HIT;
                }
            }
            goto DRAW_HIT_LABEL;
        }
        nTabMidY = gnBorderYDifference + gnScreenHeight - 264;
        goto TAB_MID_HIT;
    }
    if (nMode == 2)
    {
        //  D2ExpRes SkillTabMouseHoverOver6 lands here.
        if (nMouseY <= gnBorderYDifference + gnScreenHeight - 264)
        {
            nTabTopY = gnBorderYDifference + gnScreenHeight - 372;
            if (nMouseY >= nTabTopY)
            {
                goto TAB_TOP_HIT;
            }
        }
        //  D2ExpRes SkillTabMouseHoverOver2 lands here.
        if (nMouseY <= gnBorderYDifference + gnScreenHeight - 48)
        {
            nTabBotY = gnBorderYDifference + gnScreenHeight - 156;
            goto TAB_BOT_HIT;
        }
        goto DRAW_HIT_LABEL;
    }
    if (nMode == 3)
    {
        //  D2ExpRes SkillTabMouseHoverOver3 lands here.
        nTabBotY = gnBorderYDifference + gnScreenHeight - 156;
        if (nMouseY <= nTabBotY)
        {
            nTabMidY = gnBorderYDifference + gnScreenHeight - 264;
            if (nMouseY >= nTabMidY)
            {
                goto TAB_MID_HIT;
            }
        }
        //  D2ExpRes SkillTabMouseHoverOver4 lands here.
        if (nMouseY > gnBorderYDifference + gnScreenHeight - 48)
        {
            goto DRAW_HIT_LABEL;
        }
        goto TAB_BOT_HIT;
    }
    goto DRAW_HIT_LABEL;

TAB_TOP_HIT:
    bTabTopHit = 1;
    nLabelY = nTabTopY;
    bAnyTabHit = 1;
    goto DRAW_HIT_LABEL;

TAB_MID_HIT:
    bTabMidHit = 1;
    nLabelY = nTabMidY;
    bAnyTabHit = 1;
    goto DRAW_HIT_LABEL;

TAB_BOT_HIT:
    if (nMouseY >= nTabBotY)
    {
        bTabBotHit = 1;
        nLabelY = nTabBotY;
        bAnyTabHit = 1;
    }

DRAW_HIT_LABEL:
    D2WIN_10127_SetFont(D2FONT_FONT16);
    dwColor = bAnyTabHit;
    if (bAnyTabHit)
    {
        //  `mov al, gnBlackPaletteIndex` over the flag in eax: the low byte is replaced and the upper three are kept (zero, the flag being 1).
        dwColor = (dwColor & 0xFFFFFF00u) | gnBlackPaletteIndex;
        GFX_DrawFilledRect(gnScreenWidth - gnBorderXDifference - 89, nLabelY + 5, 90, 20, dwColor, 6);
        if (bTabBotHit)
        {
            wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4226_StrSklTreec);
            CHAR_DrawTextCentered(gnScreenWidth - gnBorderXDifference - 89, nLabelY + 20, 90, wszText, 0);
        }
        if (bTabMidHit)
        {
            wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4225_StrSklTreeb);
            CHAR_DrawTextCentered(gnScreenWidth - gnBorderXDifference - 89, nLabelY + 20, 90, wszText, 0);
        }
        if (bTabTopHit)
        {
            wszText = D2LANG_GetStringFromTblIndex(STR_IDX_4224_StrSklTreea);
            CHAR_DrawTextCentered(gnScreenWidth - gnBorderXDifference - 89, nLabelY + 20, 90, wszText, 0);
        }
    }
}

//1.10f: D2Client.0x6FB17A30
//  Is the cursor inside a bottom-anchored rectangle: nLeft <= x <= nLeft + nWidth and nBottom - nHeight <= y <= nBottom, both edges inclusive. The
//  fourth argument grows UPWARDS from the second, which is how the tree's cels are placed and not how a RECT is usually written. The cursor is
//  read here, through GetMouseX and GetMouseY, and is not an argument.
BOOL __fastcall SKILLSTREE_IsMouseInRect(int32_t nLeft, int32_t nBottom, int32_t nWidth, int32_t nHeight)
{
    int32_t nMouseX;
    int32_t nMouseY;

    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    return nMouseX >= nLeft && nMouseX <= nLeft + nWidth && nMouseY >= nBottom - nHeight && nMouseY <= nBottom;
}


//  The skill-grid geometry, written once and used by both handlers and the icon drawer; the original computes it inline in each. The row helper
//  answers the cell's BOTTOM edge: the icon is drawn up from it and the hit box runs 48 pixels above it.
//  BUG: a row or column outside 1..6 and 1..3 falls to a coordinate of ZERO instead of being excluded: a zero row puts the cell above the
//  screen's top edge, out of sight and out of the cursor's reach, and a zero column puts it against the left edge at its row's height.
//  LATENT: only a skilldesc.txt row or column outside the grid reaches it.
static int32_t SKILLTREE_CellBottom(int32_t nSkillRow)
{
    switch (nSkillRow)
    {
    case 1: return gnBorderYDifference + gnScreenHeight - 418;
    case 2: return gnBorderYDifference + gnScreenHeight - 350;
    case 3: return gnBorderYDifference + gnScreenHeight - 282;
    case 4: return gnBorderYDifference + gnScreenHeight - 214;
    case 5: return gnBorderYDifference + gnScreenHeight - 145;
    case 6: return gnBorderYDifference + gnScreenHeight - 77;
    default: return 0;
    }
}

static int32_t SKILLTREE_CellLeft(int32_t nSkillColumn)
{
    switch (nSkillColumn)
    {
    case 1: return gnScreenWidth - gnBorderXDifference - 305;
    case 2: return gnScreenWidth - gnBorderXDifference - 236;
    case 3: return gnScreenWidth - gnBorderXDifference - 167;
    default: return 0;
    }
}

//  The record lookup the two message handlers, the two draw passes and SKILLSTREE_DrawSkillIcon repeat, with the same four bounds checks in the
//  same order. Returns the skills.txt record and writes the skilldesc.txt one through the out-parameter; NULL means "skip this position".
static D2SkillsTxt *SKILLTREE_LookupSkill(int32_t nClassId, int32_t nPosition, D2SkillDescTxt **ppSkillDesc, int32_t *pnSkillId)
{
    int32_t nClassSkillId;
    int32_t wSkillDesc;
    D2SkillsTxt *pSkillsTxt;

    nClassSkillId = D2COMMON_SKILLS_GetClassSkillId(nClassId, nPosition);
    *pnSkillId = nClassSkillId;
    if (nClassSkillId < 0 || nClassSkillId >= sgptDataTables->nSkillsTxtRecordCount)
    {
        return NULL;
    }
    pSkillsTxt = &sgptDataTables->pSkillsTxt[nClassSkillId];
    if (!pSkillsTxt)
    {
        return NULL;
    }
    wSkillDesc = (int16_t)pSkillsTxt->wSkillDesc;
    if (wSkillDesc < 0 || wSkillDesc >= sgptDataTables->nSkillDescTxtRecordCount)
    {
        return NULL;
    }
    *ppSkillDesc = &sgptDataTables->pSkillDescTxt[wSkillDesc];
    if (!*ppSkillDesc)
    {
        return NULL;
    }
    return pSkillsTxt;
}

//1.10f: D2Client.0x6FB17A80
//  Pressing the left button on the skill tree. The original writes the grid's coordinates inline in each of the two walks; ours uses
//  SKILLTREE_CellBottom and SKILLTREE_CellLeft.
//  The same click means two different things: the grid is walked twice in one press, under complementary conditions, and exactly one walk runs:
//      unspent points == 0   send client command 0x3C (D2GSPacketClt3C) with the skill id and CLOSE the tree -- this makes it the active skill
//      unspent points >  0   set the cell's state to 1 and its palette to 3, update the cursor, play sound 5 -- this ARMS the cell, and the
//                            release spends the point
//  Nothing here casts. The assign path refuses a skill with (pSkillsTxt->dwFlags[0] & LOBYTE(gdwBitMasks[4])) != 0; gdwBitMasks is read by its
//  bare name.
//  The assign path consumes the message once per skill: CONSUME_IN_LOOP is inside the loop, so every position whose records resolve and that
//  fails the page, the row or the cell test writes bReturn and lResult and calls SMsgBreakHandlerChain before moving to the next. A sorceress has
//  thirty skills, so one click that lands on no cell breaks the handler chain up to thirty times. Reproduced exactly.
//  The page tabs are a band, not three boxes: x from gnScreenWidth - gnBorderXDifference - 88 to gnScreenWidth - gnBorderXDifference, and the
//  page is chosen by y alone, in three ranges that are not contiguous, every bound exclusive (h is gnBorderYDifference + gnScreenHeight):
//      page 3   (h - 372, h - 265)
//      page 2   (h - 264, h - 157)
//      page 1   (h - 156, gnScreenHeight - 49)   -- this last bound has no gnBorderYDifference
//  Two pixels fall between each pair of ranges and belong to no page (h - 265 and h - 264, h - 157 and h - 156). The three tests are three
//  separate ifs rather than a chain, so a y in two ranges would take the last; they do not overlap, so nothing follows. The bottom bound of
//  page 1 is measured from the screen and the other five from the shifted panel. Reproduced.
//  D2ExpRes patches this function seven times: SkillIconsClickdown, SkillIconsNoSP, SkillTabClick1, SkillTabClick2, SkillTabClick3,
//  SkillTabClick4 and SkillsCloseButtonClickdown. Replacing the function wholesale disables all of them.
void __stdcall SKILLSTREE_OnLButtonDown(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    int32_t nClassId;
    int32_t nPosition;
    int32_t nSkillId;
    int32_t nCellBottom;
    int32_t nCellLeft;
    int32_t nCloseX;
    int32_t nCloseY;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nPageBefore;
    D2UnitStrc *pPlayer;
    D2SkillsTxt *pSkillsTxt;
    D2SkillDescTxt *pSkillDescTxt;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;

    if (PANEL_BeltHitTestPanel(pMsg))
    {
        return;
    }

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        return;
    }
    nClassId = pPlayer->dwClassId;

    //  in the control bar: not ours
    if (nYpos > gnScreenHeight - 48)
    {
        return;
    }

    if (!gbSkillTreeDrawn)
    {
        GAME_ResetMouseButtonState();
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
        return;
    }

    //  ---- the assign walk (command 0x3C), taken only with no points ----
    if (nXpos > gnScreenWidth - gnBorderXDifference - 320 && nXpos < gnScreenWidth - gnBorderXDifference - 88 && D2COMMON_STATLIST_GetUnitBaseStat(pPlayer, STAT_SKILLPTS, 0) <= 0)
    {
        for (nPosition = 0; nPosition < D2COMMON_SKILLS_GetPlayerSkillCount(nClassId); ++nPosition)
        {
            pSkillDescTxt = NULL;
            pSkillsTxt = SKILLTREE_LookupSkill(nClassId, nPosition, &pSkillDescTxt, &nSkillId);
            if (!pSkillsTxt)
            {
                continue;
            }
            if (gnSelectedSkillPage != (int8_t)pSkillDescTxt->nSkillPage)
            {
                goto CONSUME_IN_LOOP;
            }

            nCellBottom = SKILLTREE_CellBottom(pSkillDescTxt->nSkillRow);
            if (nYpos <= nCellBottom - 48 || nYpos >= nCellBottom)
            {
                //  A row miss takes the same path as a page or cell miss: on to the consume at the bottom of the loop, then the next position.
                goto CONSUME_IN_LOOP;
            }

            nCellLeft = SKILLTREE_CellLeft(pSkillDescTxt->nSkillColumn);
            if (nXpos <= nCellLeft || nXpos >= nCellLeft + 48 || gnSkillTreeCellState[nPosition] || (pSkillsTxt->dwFlags[0] & (uint8_t)gdwBitMasks[4]) != 0)
            {
                goto CONSUME_IN_LOOP;
            }

            MSG_SendCmdUnit(60, pSkillsTxt->nSkillId, -1);
            UI_SetUIToggle(UI_SKILLTREE, UI_TURNOFF, 0);
            continue;

        CONSUME_IN_LOOP:
            //  once per skill, not once per click
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
            STORM_SMsgBreakHandlerChain(pMsg);
        }

        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
    }

    //  ---- the page tabs ----
    if (nXpos >= gnScreenWidth - gnBorderXDifference - 88)
    {
        if (nXpos <= gnScreenWidth - gnBorderXDifference)
        {
            nPageBefore = gnSelectedSkillPage;
            if (nYpos > gnBorderYDifference + gnScreenHeight - 372 && nYpos < gnBorderYDifference + gnScreenHeight - 265)
            {
                gnSelectedSkillPage = 3;
            }
            if (nYpos > gnBorderYDifference + gnScreenHeight - 264 && nYpos < gnBorderYDifference + gnScreenHeight - 157)
            {
                gnSelectedSkillPage = 2;
            }
            //  and this bound alone is measured from the screen
            if (nYpos > gnBorderYDifference + gnScreenHeight - 156 && nYpos < gnScreenHeight - 49)
            {
                gnSelectedSkillPage = 1;
            }
            if (gnSelectedSkillPage != nPageBefore)
            {
                SOUNDINST_PlaySound(SOUND_CURSOR_SWITCH, NULL, 0, 0, 0);
            }
        }
        pMsg->bReturn = 1;
        pMsg->lResult = 0;
        STORM_SMsgBreakHandlerChain(pMsg);
    }

    //  ---- the close button, wherever the tab put it ----
    switch (gnSkillTreeTab)
    {
    case 1:
        nCloseX = gnScreenWidth - gnBorderXDifference - 149;
        break;
    case 2:
        nCloseX = gnScreenWidth - gnBorderXDifference - 220;
        break;
    case 3:
        nCloseX = gnScreenWidth - gnBorderXDifference - 305;
        break;
    default:
        goto SELECT_WALK;
    }

    nCloseY = gnBorderYDifference + gnScreenHeight - 63;
    nMouseX = CURSOR_GetMouseX();
    nMouseY = CURSOR_GetMouseY();
    if (nMouseX >= nCloseX && nMouseX <= nCloseX + 34 && nMouseY >= nCloseY - 34 && nMouseY <= nCloseY)
    {
        SOUNDINST_PlaySound(SOUND_CURSOR_BUTTON_CLICK, NULL, 0, 0, 0);
        gbSkillTreeButtonDown = 1;
    }

SELECT_WALK:
    //  ---- the arm walk, taken only with points to spend ----
    if (nXpos <= gnScreenWidth - gnBorderXDifference - 320 || nXpos >= gnScreenWidth - gnBorderXDifference - 88)
    {
        pMsg->bReturn = 0;
        return;
    }

    if (D2COMMON_STATLIST_GetUnitBaseStat(pPlayer, STAT_SKILLPTS, 0) <= 0)
    {
        goto CONSUME;
    }

    for (nPosition = 0; nPosition < D2COMMON_SKILLS_GetPlayerSkillCount(nClassId); ++nPosition)
    {
        pSkillDescTxt = NULL;
        pSkillsTxt = SKILLTREE_LookupSkill(nClassId, nPosition, &pSkillDescTxt, &nSkillId);
        if (!pSkillsTxt)
        {
            continue;
        }
        if (gnSelectedSkillPage != (int8_t)pSkillDescTxt->nSkillPage)
        {
            continue;
        }

        nCellBottom = SKILLTREE_CellBottom(pSkillDescTxt->nSkillRow);
        if (nYpos <= nCellBottom - 48 || nYpos >= nCellBottom)
        {
            continue;
        }

        nCellLeft = SKILLTREE_CellLeft(pSkillDescTxt->nSkillColumn);
        if (nXpos > nCellLeft && nXpos < nCellLeft + 48 && !gnSkillTreeCellState[nPosition])
        {
            gnSkillTreeCellState[nPosition] = 1;
            gnSkillTreeCellPalette[nPosition] = 3;
            CURSOR_UpdateCursorPosInGame(pMsg);
            pMsg->bReturn = 1;
            pMsg->lResult = 0;
            SOUNDINST_PlaySound(SOUND_CURSOR_POINT_DROP, NULL, 0, 0, 0);
            STORM_SMsgBreakHandlerChain(pMsg);
            return;
        }
    }

CONSUME:
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}

//1.10f: D2Client.0x6FB18100
//  Releasing the left button on the skill tree -- where the point is spent. The original writes the grid's coordinates inline in the walk; ours
//  uses SKILLTREE_CellBottom and SKILLTREE_CellLeft.
//  The close button is disarmed by the release wherever it lands: gbSkillTreeButtonDown is cleared first, then the box is tested; a release
//  outside it leaves the tree open and consumes the message anyway. A gnSkillTreeTab outside 1..3 consumes without computing a box.
//  Spending a point needs four things to agree:
//      unspent points > 0            or the handler consumes and stops
//      the cell's state is 1         set by the press
//      the skill's page is showing   gnSelectedSkillPage
//      the cursor is in the cell     the same 48 by 48 as the press
//  and then two more before the packet goes out:
//      SKILLS_EvaluateSkillFormula(pUnit, dwSkPoints, ...) <= unspent points, unless dwSkPoints is -1, which skips the check entirely
//      the player is not STATE_UNINTERRUPTABLE
//  wMaxLvl is skills.txt's wMaxLvl for the record, or 20 when that field is zero or negative -- a default in the client, not in the file.
//  gbSkillPointSendPending = 0 runs right after the send, inside a block guarded by !gbSkillPointSendPending; it was already zero, so the store
//  does nothing.
//  Whether the point is spent or refused, gnSkillTreeCellState[iCell] and gnSkillTreeCellPalette[iCell] go back to 0, and the walk clears every
//  OTHER cell it passes whose state is not -1 as it goes. The walk returns at the cell it acts on, so the clearing stops there.
//  D2ExpRes patches this function four times: NewSkillButtonReleaseProblemFixedSkilltabOpen, SkillIconsRelease, SkillsCloseButtonRelease1 and
//  SkillsCloseButtonRelease2. Replacing the function wholesale disables all of them.
void __stdcall SKILLSTREE_OnLButtonUp(SMSGHANDLER_PARAMS *pMsg)
{
    int32_t nXpos;
    int32_t nYpos;
    int32_t nClassId;
    int32_t nPosition;
    int32_t iCell;
    int32_t nSkillId;
    int32_t nCellBottom;
    int32_t nCellLeft;
    int32_t nCloseX;
    int32_t nCloseY;
    int32_t nMouseX;
    int32_t nMouseY;
    int32_t nUnspentPoints;
    int32_t nSkillLevel;
    int32_t nMaxLevel;
    D2UnitStrc *pPlayer;
    D2SkillStrc *pSkill;
    D2SkillsTxt *pSkillsTxt;
    D2SkillDescTxt *pSkillDescTxt;

    nXpos = pMsg->nXpos;
    nYpos = pMsg->nYpos;

    pPlayer = CUNIT_GetPlayer();
    if (!pPlayer)
    {
        return;
    }
    nClassId = pPlayer->dwClassId;

    CURSOR_OnPanelMouseUp(pMsg);
    if (PANEL_BeltHitTestPanel(pMsg))
    {
        return;
    }

    if (!gbSkillTreeDrawn)
    {
        goto RESET_AND_CONSUME;
    }

    if (gbSkillTreeButtonDown)
    {
        gbSkillTreeButtonDown = 0;
        switch (gnSkillTreeTab)
        {
        case 1:
            nCloseX = gnScreenWidth - gnBorderXDifference - 149;
            break;
        case 2:
            nCloseX = gnScreenWidth - gnBorderXDifference - 220;
            break;
        case 3:
            nCloseX = gnScreenWidth - gnBorderXDifference - 305;
            break;
        default:
            goto CONSUME;
        }
        nCloseY = gnBorderYDifference + gnScreenHeight - 63;
        nMouseX = CURSOR_GetMouseX();
        nMouseY = CURSOR_GetMouseY();
        if (nMouseX >= nCloseX && nMouseX <= nCloseX + 34 && nMouseY >= nCloseY - 34 && nMouseY <= nCloseY)
        {
            UI_SetUIToggle(UI_SKILLTREE, UI_TOGGLE, 0);
        }
        goto CONSUME;
    }

    if (nXpos <= gnScreenWidth / 2)
    {
        pMsg->bReturn = 0;
        return;
    }

    nUnspentPoints = D2COMMON_STATLIST_GetUnitBaseStat(pPlayer, STAT_SKILLPTS, 0);
    if (nUnspentPoints <= 0)
    {
        goto RESET_AND_CONSUME;
    }

    for (nPosition = 0; nPosition < D2COMMON_SKILLS_GetPlayerSkillCount(nClassId); ++nPosition)
    {
        iCell = nPosition;
        pSkillDescTxt = NULL;
        pSkillsTxt = SKILLTREE_LookupSkill(nClassId, nPosition, &pSkillDescTxt, &nSkillId);
        if (!pSkillsTxt)
        {
            continue;
        }

        if (gnSelectedSkillPage != (int8_t)pSkillDescTxt->nSkillPage)
        {
            goto CLEAR_CELL;
        }

        nCellBottom = SKILLTREE_CellBottom(pSkillDescTxt->nSkillRow);
        nCellLeft = SKILLTREE_CellLeft(pSkillDescTxt->nSkillColumn);
        if (nYpos <= nCellBottom - 48 || nYpos >= nCellBottom || nXpos <= nCellLeft || nXpos >= nCellLeft + 48 || gnSkillTreeCellState[iCell] != 1)
        {
            goto CLEAR_CELL;
        }

        //  dwSkPoints == -1 skips the affordability check entirely
        if (pSkillsTxt->dwSkPoints != -1)
        {
            nSkillLevel = 0;
            pSkill = D2COMMON_SKILLS_GetSkillById(pPlayer, (D2C_Skills)nSkillId, 0xFFFFFFFF);
            if (pSkill)
            {
                nSkillLevel = D2COMMON_SKILLS_GetSkillLevel(pPlayer, pSkill, 0);
            }
            if (D2COMMON_SKILLS_EvaluateSkillFormula(pPlayer, pSkillsTxt->dwSkPoints, (D2C_Skills)nSkillId, nSkillLevel) > nUnspentPoints)
            {
                goto DESELECT_AND_CONSUME;
            }
        }

        nSkillLevel = 0;
        pSkill = D2COMMON_SKILLS_GetSkillById(pPlayer, (D2C_Skills)nSkillId, 0xFFFFFFFF);
        if (pSkill)
        {
            nSkillLevel = D2COMMON_SKILLS_GetSkillLevel(pPlayer, pSkill, 0);
        }

        nMaxLevel = 20;
        if (pSkillsTxt->nSkillId >= 0 && pSkillsTxt->nSkillId < sgptDataTables->nSkillsTxtRecordCount)
        {
            if ((int16_t)sgptDataTables->pSkillsTxt[pSkillsTxt->nSkillId].wMaxLvl > 0)
            {
                nMaxLevel = (int16_t)sgptDataTables->pSkillsTxt[pSkillsTxt->nSkillId].wMaxLvl;
            }
        }

        //  BUG: the cap is tested with <=, so a skill already at its maximum still sends. NOT ESTABLISHED: whether the server refuses the point.
        if (nSkillLevel <= nMaxLevel)
        {
            pPlayer = CUNIT_GetPlayer();
            if ((!pPlayer || !D2COMMON_STATES_CheckState(pPlayer, STATE_UNINTERRUPTABLE)) && !gbSkillPointSendPending)
            {
                MSG_SendCmdWord(59, (int16_t)nSkillId);
                //  it was already 0 to get here
                gbSkillPointSendPending = 0;
            }
        }
        goto DESELECT_AND_CONSUME;

    CLEAR_CELL:
        if (gnSkillTreeCellState[nPosition] != -1)
        {
            gnSkillTreeCellState[nPosition] = 0;
            gnSkillTreeCellPalette[nPosition] = 0;
        }
    }

RESET_AND_CONSUME:
    GAME_ResetMouseButtonState();

CONSUME:
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
    return;

DESELECT_AND_CONSUME:
    gnSkillTreeCellState[iCell] = 0;
    gnSkillTreeCellPalette[iCell] = 0;
    pMsg->bReturn = 1;
    pMsg->lResult = 0;
    STORM_SMsgBreakHandlerChain(pMsg);
}


//1.10f: D2Client.0x6FB18580
//  Drops every armed cell back to 0: walks the player's skills and reduces each gnSkillTreeCellState entry that is exactly 1 to 0; the palette
//  array is not touched. Nothing happens without a player. SKILLS_GetPlayerSkillCount is called inside the loop on every iteration, not hoisted;
//  that is the original's own shape.
void __cdecl SKILLSTREE_ClearCellState()
{
    D2UnitStrc *pPlayer;
    int32_t i;

    pPlayer = CUNIT_GetPlayer();
    if (pPlayer)
    {
        for (i = 0; i < D2COMMON_SKILLS_GetPlayerSkillCount(pPlayer->dwClassId); ++i)
        {
            if (gnSkillTreeCellState[i] == 1)
                gnSkillTreeCellState[i] = 0;
        }
    }
}
